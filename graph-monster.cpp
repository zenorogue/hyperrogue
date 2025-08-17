// Hyperbolic Rogue -- monster graphics file
// Copyright (C) 2011-2025 Zeno Rogue, see 'hyper.cpp' for details

#include "hyper.h"
namespace hr {

EX color_t fc(int ph, color_t col, int z) {
  if(items[itOrbFire]) col = darkena(firecolor(ph), 0, 0xFF);
  if(items[itOrbAether]) col = (col &~0XFF) | (col&0xFF) / 2;
  for(cell *pc: player_positions()) 
    if(items[itOrbFish] && isWatery(pc) && z != 3) return watercolor(ph);
  if(invismove) 
    col = 
      shmup::on ?
        (col &~0XFF) | (int((col&0xFF) * .25))
      : (col &~0XFF) | (int((col&0xFF) * (100+100*sintick(500)))/200);
  return col;
  }

EX void drawStunStars(const shiftmatrix& V, int t) {
#if CAP_SHAPES
  for(int i=0; i<3*t; i++) {
    shiftmatrix V2 = V * spin(TAU * i / (3*t) + ptick(200));
#if MAXMDIM >= 4
    if(GDIM == 3) V2 = V2 * lzpush(cgi.HEAD);
#endif
    queuepolyat(V2, cgi.shFlailBall, 0xFFFFFFFF, PPR::STUNSTARS);
    }
#endif
  }

EX namespace tortoise {

  // small is 0 or 2
  EX void draw(const shiftmatrix& V, int bits, int small, int stuntime) {

#if CAP_SHAPES
    color_t eyecolor = getBit(bits, tfEyeHue) ? 0xFF0000 : 0xC0C0C0;
    color_t shellcolor = getBit(bits, tfShellHue) ? 0x00C040 : 0xA06000;
    color_t scutecolor = getBit(bits, tfScuteHue) ? 0x00C040 : 0xA06000;
    color_t skincolor = getBit(bits, tfSkinHue) ? 0x00C040 : 0xA06000;
    if(getBit(bits, tfShellSat)) shellcolor = gradient(shellcolor, 0xB0B0B0, 0, .5, 1);
    if(getBit(bits, tfScuteSat)) scutecolor = gradient(scutecolor, 0xB0B0B0, 0, .5, 1);
    if(getBit(bits, tfSkinSat)) skincolor = gradient(skincolor, 0xB0B0B0, 0, .5, 1);
    if(getBit(bits, tfShellDark)) shellcolor = gradient(shellcolor, 0, 0, .5, 1);
    if(getBit(bits, tfSkinDark)) skincolor = gradient(skincolor, 0, 0, .5, 1);
    
    if(bits < 0) { 
      skincolor = 0xC00060;
      shellcolor = 0xFF00FF;
      scutecolor = 0x6000C0;
      eyecolor = 0xFFFFFF;
      }
    
    for(int i=0; i<12; i++) {
      color_t col = 
        i == 0 ? shellcolor:
        i <  8 ? scutecolor :
        skincolor;
      int b = getBit(bits, i);
      int d = darkena(col, 0, 0xFF);
      if(i >= 1 && i <= 7) if(b) { d = darkena(col, 1, 0xFF); b = 0; }
      
      if(i >= 8 && i <= 11 && stuntime >= 3) continue;

      queuepoly(V, cgi.shTortoise[i][b+small], d);
      if((i >= 5 && i <= 7) || (i >= 9 && i <= 10))
        queuepoly(V * lmirror(), cgi.shTortoise[i][b+small], d);
      
      if(i == 8) {
        for(int k=0; k<stuntime; k++) {
          eyecolor &= 0xFEFEFE; 
          eyecolor >>= 1;
          }
        queuepoly(V, cgi.shTortoise[12][b+small], darkena(eyecolor, 0, 0xFF));
        queuepoly(V * lmirror(), cgi.shTortoise[12][b+small], darkena(eyecolor, 0, 0xFF));
        }
      }
#endif
    }

  EX int getMatchColor(int bits) {
    int mcol = 1;
    double d = tortoise::getScent(bits);
    
    if(d > 0) mcol = 0xFFFFFF;
    else if(d < 0) mcol = 0;
    
      int dd = 0xFF * (atan(fabs(d)/2) / 90._deg);
      
    return gradient(0x487830, mcol, 0, dd, 0xFF);
    }
  EX }

double footfun(double d) {
  d -= floor(d);
  return
    d < .25 ? d :
    d < .75 ? .5-d :
    d-1;
  }

EX void animallegs(const shiftmatrix& V, eMonster mo, color_t col, double footphase) {
#if CAP_SHAPES
  footphase /= SCALE;
  
  bool dog = mo == moRunDog;
  bool bug = mo == moBug0 || mo == moMetalBeast;

  if(bug) footphase *= 2.5;
  
  double rightfoot = footfun(footphase / .4 / 2) / 4 * 2;
  double leftfoot = footfun(footphase / .4 / 2 - (bug ? .5 : dog ? .1 : .25)) / 4 * 2;
  
  if(bug) rightfoot /= 2.5, leftfoot /= 2.5;
  
  rightfoot *= SCALE;
  leftfoot *= SCALE;
  
  if(!footphase) rightfoot = leftfoot = 0;

  hpcshape* sh[6][4] = {
    {&cgi.shDogFrontPaw, &cgi.shDogRearPaw, &cgi.shDogFrontLeg, &cgi.shDogRearLeg},
    {&cgi.shWolfFrontPaw, &cgi.shWolfRearPaw, &cgi.shWolfFrontLeg, &cgi.shWolfRearLeg},
    {&cgi.shReptileFrontFoot, &cgi.shReptileRearFoot, &cgi.shReptileFrontLeg, &cgi.shReptileRearLeg},
    {&cgi.shBugLeg, NULL, NULL, NULL},
    {&cgi.shTrylobiteFrontClaw, &cgi.shTrylobiteRearClaw, &cgi.shTrylobiteFrontLeg, &cgi.shTrylobiteRearLeg},
    {&cgi.shBullFrontHoof, &cgi.shBullRearHoof, &cgi.shBullFrontHoof, &cgi.shBullRearHoof},
    };

  hpcshape **x = sh[mo == moRagingBull ? 5 : mo == moBug0 ? 3 : mo == moMetalBeast ? 4 : mo == moRunDog ? 0 : mo == moReptile ? 2 : 1];

#if MAXMDIM >= 4  
  if(GDIM == 3 && !(embedded_plane && gproduct)) {
    if(x[0]) queuepolyat(V * front_leg_move * cspin(0, 2, rightfoot / leg_length) * front_leg_move_inverse, *x[0], col, PPR::MONSTER_FOOT);
    if(x[0]) queuepolyat(V * lmirror() * front_leg_move * cspin(0, 2, leftfoot / leg_length) * front_leg_move_inverse, *x[0], col, PPR::MONSTER_FOOT);
    if(x[1]) queuepolyat(V * rear_leg_move * cspin(0, 2, -rightfoot / leg_length) * rear_leg_move_inverse, *x[1], col, PPR::MONSTER_FOOT);
    if(x[1]) queuepolyat(V * lmirror() * rear_leg_move * cspin(0, 2, -leftfoot / leg_length) * rear_leg_move_inverse, *x[1], col, PPR::MONSTER_FOOT);
    return;
    }
#endif

  const shiftmatrix VL = at_smart_lof(V, cgi.ALEG0);
  const shiftmatrix VAML = at_smart_lof(V, cgi.ALEG);

  if(x[0]) queuepolyat(VL * lxpush(rightfoot), *x[0], col, PPR::MONSTER_FOOT);
  if(x[0]) queuepolyat(VL * lmirror() * lxpush(leftfoot), *x[0], col, PPR::MONSTER_FOOT);
  if(x[1]) queuepolyat(VL * lxpush(-rightfoot), *x[1], col, PPR::MONSTER_FOOT);
  if(x[1]) queuepolyat(VL * lmirror() * lxpush(-leftfoot), *x[1], col, PPR::MONSTER_FOOT);

  if(x[2]) queuepolyat(VAML * lxpush(rightfoot/2), *x[2], col, PPR::MONSTER_FOOT);
  if(x[2]) queuepolyat(VAML * lmirror() * lxpush(leftfoot/2), *x[2], col, PPR::MONSTER_FOOT);
  if(x[3]) queuepolyat(VAML * lxpush(-rightfoot/2), *x[3], col, PPR::MONSTER_FOOT);
  if(x[3]) queuepolyat(VAML * lmirror() * lxpush(-leftfoot/2), *x[3], col, PPR::MONSTER_FOOT);
#endif
  }

EX bool noshadow;

#if CAP_SHAPES
EX void ShadowV(const shiftmatrix& V, const hpcshape& bp, PPR prio IS(PPR::MONSTER_SHADOW)) {
  if(WDIM == 2 && GDIM == 3 && bp.shs != bp.she) {
    if(noshadow) return;
    auto& p = queuepolyat(V, bp, 0x18, PPR::TRANSPARENT_SHADOW); 
    p.outline = 0;
    p.subprio = -100;
    p.offset = bp.shs;
    p.cnt = bp.she - bp.shs;
    p.flags &=~ POLY_TRIANGLES;
    p.tinf = NULL;
    return;
    }
  if(mmspatial) { 
    if(model_needs_depth() || noshadow) 
      return; // shadows break the depth testing
    dynamicval<color_t> p(poly_outline, OUTLINE_TRANS);
    queuepolyat(V, bp, SHADOW_MON, prio); 
    }
  }
#endif

#if HDR
#define VFOOT ((GDIM == 2 || mhybrid) ? V : at_smart_lof(V, cgi.LEG0))
#define VLEG at_smart_lof(V, cgi.LEG)
#define VGROIN at_smart_lof(V, cgi.GROIN)
#define VBODY at_smart_lof(V, cgi.BODY)
#define VBODY1 at_smart_lof(V, cgi.BODY1)
#define VBODY2 at_smart_lof(V, cgi.BODY2)
#define VBODY3 at_smart_lof(V, cgi.BODY3)
#define VNECK at_smart_lof(V, cgi.NECK)
#define VHEAD  at_smart_lof(V, cgi.HEAD)
#define VHEAD1 at_smart_lof(V, cgi.HEAD1)
#define VHEAD2 at_smart_lof(V, cgi.HEAD2)
#define VHEAD3 at_smart_lof(V, cgi.HEAD3)

#define VALEGS V
#define VABODY at_smart_lof(V, cgi.ABODY)
#define VAHEAD at_smart_lof(V, cgi.AHEAD)

#define VFISH V
#define VBIRD  ((GDIM == 3 || (where && bird_disruption(where))) ? (WDIM == 2 ? at_smart_lof(V, cgi.BIRD) : V) : at_smart_lof(V, cgi.BIRD + .05 * sintick(1000, static_cast<int>(reinterpret_cast<size_t>(where))/1000.)))
#define VGHOST  at_smart_lof(V, cgi.GHOST)

#define VSLIMEEYE  orthogonal_move_fol(V, cgi.FLATEYE)
#endif

#if CAP_SHAPES
EX transmatrix otherbodyparts(const shiftmatrix& V, color_t col, eMonster who, double footphase) {

  // if(!mmspatial && !footphase && who != moSkeleton) return;
  
  footphase /= SCALE;
  double rightfoot = footfun(footphase / .4 / 2.5) / 4 * 2.5 * SCALE;
  
  const double wobble = -1;

  // todo

  auto who1 = who;
  if(who == moPlayer) {
    charstyle& cs = getcs();
    auto id = ePlayershape(cs.charid >> 1);
    if(id == pshRatling) who1 = moRatling;
    }

  if(detaillevel >= 2 && GDIM == 2) { 
    shiftmatrix VL = at_smart_lof(V, cgi.LEG1);
    queuepoly(VL * xpush(rightfoot*3/4), cgi.shHumanLeg, col);
    queuepoly(VL * lmirror() * xpush(-rightfoot*3/4), cgi.shHumanLeg, col);
    }

  if(GDIM == 2) {
    shiftmatrix VL = at_smart_lof(V, cgi.LEG);
    queuepoly(VL * xpush(rightfoot/2), cgi.shHumanLeg, col);
    queuepoly(VL * lmirror() * xpush(-rightfoot/2), cgi.shHumanLeg, col);
    }

  if(detaillevel >= 2 && GDIM == 2) { 
    shiftmatrix VL = at_smart_lof(V, cgi.LEG3);
    queuepoly(VL * xpush(rightfoot/4), cgi.shHumanLeg, col);
    queuepoly(VL * lmirror() * xpush(-rightfoot/4), cgi.shHumanLeg, col);
    }

  shiftmatrix Tright, Tleft;
  
  if(GDIM == 2 || mhybrid || cgi.emb->is_euc_in_product()) {
    Tright = VFOOT * xpush(rightfoot);
    Tleft = VFOOT * lmirror() * xpush(-rightfoot);
    }
  #if MAXMDIM >= 4
  else {
    shiftmatrix V1 = V;
    if(WDIM == 2) V1 = V1 * lzpush(cgi.GROIN);
    int zdir = cgi.emb->is_euc_in_nil() ? 1 : 2;
    Tright = V1 * cspin(0, zdir, rightfoot/ leg_length);
    Tleft  = V1 * lmirror() * cspin(zdir, 0, rightfoot / leg_length);
    Tright = V1; Tleft = V1 * lmirror();
    if(WDIM == 2) Tleft = Tleft * lzpush(-cgi.GROIN), Tright = Tright * lzpush(-cgi.GROIN);
    }
  #endif
    
  if(who == moWaterElemental && GDIM == 2) {
    double fishtail = footfun(footphase / .4) / 4 * 1.5;
    queuepoly(VFOOT * xpush(fishtail), cgi.shFishTail, watercolor(100));
    }
  else if(who == moSkeleton) {
    queuepoly(Tright, cgi.shSkeletalFoot, col);
    queuepoly(Tleft, cgi.shSkeletalFoot, col);
    return spin(rightfoot * wobble);
    }
  else if(isTroll(who1) || among(who1, moMonkey, moYeti, moRatling, moRatlingAvenger, moGoblin)) {
    queuepoly(Tright, cgi.shYetiFoot, col);
    queuepoly(Tleft, cgi.shYetiFoot, col);
    }
  else {
    queuepoly(Tright, cgi.shHumanFoot, col);
    queuepoly(Tleft, cgi.shHumanFoot, col);
    }

  if(GDIM == 3 || !mmspatial) return spin(rightfoot * wobble);  

  if(detaillevel >= 2 && who != moZombie)
    queuepoly(at_smart_lof(V, cgi.NECK1), cgi.shHumanNeck, col);
  if(detaillevel >= 1) {
    queuepoly(VGROIN, cgi.shHumanGroin, col);
    if(who != moZombie) queuepoly(VNECK, cgi.shHumanNeck, col);
    }
  if(detaillevel >= 2) {
    queuepoly(at_smart_lof(V, cgi.GROIN1), cgi.shHumanGroin, col);
    if(who != moZombie) queuepoly(at_smart_lof(V, cgi.NECK3), cgi.shHumanNeck, col);
    }
  
  return spin(rightfoot * wobble);
  }
#endif

EX bool drawstar(cell *c) {
  for(int t=0; t<c->type; t++) 
    if(c->move(t) && c->move(t)->wall != waSulphur && c->move(t)->wall != waSulphurC &&
     c->move(t)->wall != waBarrier)
       return false;
  return true;
  }

EX bool drawing_usershape_on(cell *c, mapeditor::eShapegroup sg) {
#if CAP_EDIT
  return c && c == mapeditor::drawcell && mapeditor::drawcellShapeGroup() == sg;
#else
  return false;
#endif
  }

#if CAP_SHAPES
EX color_t skincolor = 0xD0C080FF;

EX void humanoid_eyes(const shiftmatrix& V, color_t ecol, color_t hcol IS(skincolor)) {
  if(GDIM == 3) {
    queuepoly(VHEAD, cgi.shPHeadOnly, hcol);
    queuepoly(VHEAD, cgi.shSkullEyes, ecol);
    }
  }

EX void drawTerraWarrior(const shiftmatrix& V, int t, int hp, double footphase) {
  if(!mmmon) {
    draw_ascii_or_zh(V, 'T', minf[moTerraWarrior].name, gradient(0x202020, 0xFFFFFF, 0, t, 6), 1.5, 1);
    return;
    }
  ShadowV(V, cgi.shPBody);
  color_t col = linf[laTerracotta].color;
  int bcol = darkena(false ? 0xC0B23E : col, 0, 0xFF);
  const transmatrix VBS = otherbodyparts(V, bcol, moDesertman, footphase);
  queuepoly(VBODY * VBS, cgi.shPBody, bcol);
  if(!peace::on) queuepoly(VBODY * VBS * lmirror(), cgi.shPSword, darkena(0xC0C0C0, 0, 0xFF));
  queuepoly(VBODY1 * VBS, cgi.shTerraArmor1, darkena(t > 0 ? 0x4040FF : col, 0, 0xFF));
  if(hp >= 4) queuepoly(VBODY2 * VBS, cgi.shTerraArmor2, darkena(t > 1 ? 0xC00000 : col, 0, 0xFF));
  if(hp >= 2) queuepoly(VBODY3 * VBS, cgi.shTerraArmor3, darkena(t > 2 ? 0x612600 : col, 0, 0xFF));
  queuepoly(VHEAD, cgi.shTerraHead, darkena(t > 4 ? 0x202020 : t > 3 ? 0x504040 : col, 0, 0xFF));
  queuepoly(VHEAD1, cgi.shPFace, bcol);
  humanoid_eyes(V, 0x400000FF, darkena(col, 0, 0xFF));
  }
#endif

EX int wingphase(int period, int phase IS(0)) {
  ld t = fractick(period, phase);
  const int WINGS2 = WINGS * 2;
  int ti = int(t * WINGS2) % WINGS2;
  if(ti > WINGS) ti = WINGS2 - ti;
  return ti;
  }

transmatrix wingmatrix(int period, int phase = 0) {
  ld t = fractick(period, phase) * TAU;
  transmatrix Vwing = Id;
  Vwing[1][1] = .85 + .15 * sin(t);
  return Vwing;
  }

#define UNTRANS (GDIM == 3 ? 0x000000FF : 0)

EX bool drawMonsterType(eMonster m, cell *where, const shiftmatrix& V1, color_t col, double footphase, color_t asciicol) {

#if MAXMDIM >= 4
  if(GDIM == 3 && asciicol != NOCOLOR) {
    addradar(V1, minf[m].glyph, asciicol, isFriendly(m) ? 0x00FF00FF : 0xFF0000FF);
    }
#endif

  char xch = minf[m].glyph;
  
  shiftmatrix V = V1;
  if(WDIM == 3 && (classflag(m) & CF_FACE_UP) && where && !mhybrid) V = V1 * cspin90(0, 2);
  
  #if CAP_SHAPES
  if(among(m, moTortoise, moWorldTurtle) && where && where->stuntime >= 3)
    drawStunStars(V, where->stuntime-2);
  else if (among(m, moTortoise, moWorldTurtle, moMutant) || m == moPlayer || (where && !where->stuntime)) ;
  else if(where && !(isMetalBeast(m) && where->stuntime == 1))
    drawStunStars(V, where->stuntime);
  
  if(mapeditor::drawUserShape(V, mapeditor::sgMonster, m, darkena(col, 0, 0xFF), where))
    return true;
  #endif

  if(!mmmon || !CAP_SHAPES) {
    draw_ascii_or_zh(V1, xch, minf[m].name, asciicol, 1.5, 1);
    return true;
    }

#if CAP_SHAPES
  switch(m) {
    case moTortoise: {
      int bits = where ? tortoise::getb(where) : tortoise::last;
      tortoise::draw(V, bits, 0, where ? where->stuntime : 0);
      if(tortoise::seek() && !tortoise::diff(bits) && where)
        queue_ring(V, cgi.shRing, darkena(0xFFFFFF, 0, 0x80 + 0x70 * sintick(200)), PPR::ITEM);
      return true;
      }
    
    case moWorldTurtle: {
      tortoise::draw(V, -1, 0, where ? where->stuntime : 0);
      return true;
      }
    
    case moPlayer:
      drawPlayer(m, where, V, col, footphase);  
      return true;

    case moMimic: case moShadow: case moIllusion:
      drawMimic(m, where, V, col, footphase);
      return true;
    
    case moBullet:
      if(getcs().charid/2 == pshSpaceship) {
        ShadowV(V, cgi.shKnife);
        queuepoly(VBODY, cgi.shMissile, getcs().swordcolor);
        return true;
        }
      ShadowV(V, cgi.shKnife);
      queuepoly(VBODY * spin270(), cgi.shKnife, getcs().swordcolor);
      return true;
    
    case moKnight: case moKnightMoved: {
      ShadowV(V, cgi.shPBody);
      const transmatrix VBS = otherbodyparts(V, darkena(0xC0C0A0, 0, 0xC0), m, footphase);
      queuepoly(VBODY * VBS, cgi.shPBody, darkena(0xC0C0A0, 0, 0xC0));
      if(!racing::on) 
        queuepoly(VBODY * VBS, cgi.shPSword, darkena(0xFFFF00, 0, 0xFF));
      queuepoly(VBODY1 * VBS, cgi.shKnightArmor, darkena(0xD0D0D0, 1, 0xFF));
      color_t col;
      if(!eubinary && where && where->master->alt)
        col = cloakcolor(roundTableRadius(where));
      else
        col = cloakcolor(newRoundTableRadius());
      queuepoly(VBODY2 * VBS, cgi.shKnightCloak, darkena(col, 1, 0xFF));
      queuepoly(VHEAD1, cgi.shPHead, darkena(0x703800, 1, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, darkena(0xC0C0A0, 0, 0XFF));
      humanoid_eyes(V, 0x000000FF);
      return true;
      }
    
    case moGolem: case moGolemMoved: {
      ShadowV(V, cgi.shPBody);
      const transmatrix VBS = otherbodyparts(V, darkena(col, 1, 0XC0), items[itOrbFish] && items[itOrbEmpathy] ? moWaterElemental : m, footphase);
      queuepoly(VBODY * VBS, cgi.shPBody, darkena(col, 0, 0XC0));
      queuepoly(VHEAD, cgi.shGolemhead, darkena(col, 1, 0XFF));
      humanoid_eyes(V, 0xC0C000FF, darkena(col, 0, 0xFF));
      return true;
      }
    
    case moEvilGolem: case moIceGolem: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 2, 0xC0), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, darkena(col, 0, 0XC0));
      queuepoly(VHEAD, cgi.shGolemhead, darkena(col, 1, 0XFF));
      humanoid_eyes(V, 0xFF0000FF, darkena(col, 0, 0xFF));
      return true;
      }

    case moFalsePrincess: case moRoseLady: case moRoseBeauty: {
      princess:
      bool girl = princessgender() == GEN_F;
      bool evil = !isPrincess(m);
  
      int facecolor = evil ? 0xC0B090FF : 0xD0C080FF;

      auto& cs = vid.cs;
      auto id = ePlayershape(cs.charid >> 1);

      auto& body = id == pshRatling ? cgi.shYeti : girl ? cgi.shFemaleBody : cgi.shPBody;
      bool wears_dress = among(id, pshRogue, pshRatling);

      ShadowV(V, body);
      const transmatrix VBS = otherbodyparts(V, facecolor, !evil && items[itOrbFish] && items[itOrbEmpathy] ? moWaterElemental : id == pshRatling ? moYeti : m, footphase);
      queuepoly(VBODY * VBS, body, facecolor);
  
      if(m == moPrincessArmed) 
        queuepoly(VBODY * VBS * lmirror(), id == pshRogue ? cgi.shSabre : cgi.shPSword, 0xFFFFFFFF);
      
      if((m == moFalsePrincess || m == moRoseBeauty) && where && where->cpdist == 1)
        queuepoly(VBODY * VBS, cgi.shPKnife, 0xFFFFFFFF);
  
      if(m == moRoseLady) {
        queuepoly(VBODY * VBS, cgi.shPKnife, 0xFFFFFFFF);
        queuepoly(VBODY * VBS * lmirror(), cgi.shPKnife, 0xFFFFFFFF);
        }
  
      if(girl) {
        queuepoly(VBODY1 * VBS, cgi.shFemaleDress,  evil ? 0xC000C0FF : 0x00C000FF);
        if(wears_dress)
          queuepoly(VBODY2 * VBS, cgi.shPrincessDress, (evil ? 0xC040C0C0 : 0x8080FFC0) | UNTRANS);
        }
      else {
        if(wears_dress)
          queuepoly(VBODY1 * VBS, cgi.shPrinceDress,  evil ? 0x802080FF : 0x404040FF);
        }    

      auto& hair = id == pshRatling ? cgi.shRatHead : girl ? cgi.shFemaleHair : cgi.shPHead;

      if(m == moRoseLady) {
  //    queuepoly(V, girl ? cgi.shGoatHead : cgi.shDemon,  0x800000FF);
        // make her hair a bit darker to stand out in 3D
        queuepoly(VHEAD1, hair,  evil ? 0x500050FF : GDIM == 3 ? 0x666A64FF : 0x332A22FF);
        }
      else if(m == moRoseBeauty) {
        if(girl) {
          queuepoly(VHEAD1, cgi.shBeautyHair,  0xF0A0D0FF);
          queuepoly(VHEAD2, cgi.shFlowerHair,  0xC00000FF);
          }
        else {
          queuepoly(VHEAD1, cgi.shPHead,  0xF0A0D0FF);
          queuepoly(VBODY * VBS, cgi.shFlowerHand,  0xC00000FF);
          queuepoly(VBODY2 * VBS, cgi.shSuspenders,  0xC00000FF);
          }
        }
      else {
        queuepoly(VHEAD1, hair, evil ? 0xC00000FF : 0x332A22FF);
        }
      if(&hair == &cgi.shRatHead) {
        queuepoly(VHEAD, cgi.shWolf1, 0x008000FF);
        queuepoly(VHEAD, cgi.shWolf2, 0x008000FF);
        queuepoly(VHEAD, cgi.shWolf3, darkena(0x202020, 0, 0xFF));
        }
      else queuepoly(VHEAD, cgi.shPFace,  facecolor);
      humanoid_eyes(V, evil ? 0x0000C0FF : 0x00C000FF, facecolor);
      return true;
      }
  
    case moWolf: case moRedFox: case moWolfMoved: case moLavaWolf: {
      ShadowV(V, cgi.shWolfBody);
      if(mmspatial || footphase)
        animallegs(VALEGS, moWolf, darkena(col, 0, 0xFF), footphase);
      else
        queuepoly(VALEGS, cgi.shWolfLegs, darkena(col, 0, 0xFF));
      queuepoly(VABODY, cgi.shWolfBody, darkena(col, 0, 0xFF));
      if(m == moRedFox) {
        queuepoly(VABODY, cgi.shFoxTail1, darkena(col, 0, 0xFF));
        queuepoly(VABODY, cgi.shFoxTail2, darkena(0xFFFFFF, 0, 0xFF));
        }
      queuepoly(VAHEAD, cgi.shWolfHead, darkena(col, 0, 0xFF));
      queuepoly(VAHEAD, cgi.shWolfEyes, darkena(col, 3, 0xFF));
      if(GDIM == 3) {
        queuepoly(VAHEAD, cgi.shFamiliarEye, 0xFF);
        queuepoly(VAHEAD * lmirror(), cgi.shFamiliarEye, 0xFF);
        }
      return true;
      }
    
    case moReptile: {
      ShadowV(V, cgi.shReptileBody);
      animallegs(VALEGS, moReptile, darkena(col, 0, 0xFF), footphase);
      queuepoly(VABODY, cgi.shReptileBody, darkena(col, 0, 0xFF));
      queuepoly(VAHEAD, cgi.shReptileHead, darkena(col, 0, 0xFF));
      queuepoly(VAHEAD, cgi.shReptileEye, darkena(col, 3, 0xFF));
      queuepoly(VAHEAD * lmirror(), cgi.shReptileEye, darkena(col, 3, 0xFF));
      if(GDIM == 2) queuepoly(VABODY, cgi.shReptileTail, darkena(col, 2, 0xFF));
      return true;
      }
    
    case moSalamander: {
      ShadowV(V, cgi.shReptileBody);
      animallegs(VALEGS, moReptile, darkena(0xD00000, 1, 0xFF), footphase);
      queuepoly(VABODY, cgi.shReptileBody, darkena(0xD00000, 0, 0xFF));
      queuepoly(VAHEAD, cgi.shReptileHead, darkena(0xD00000, 1, 0xFF));
      queuepoly(VAHEAD, cgi.shReptileEye, darkena(0xD00000, 0, 0xFF));
      queuepoly(VAHEAD * lmirror(), cgi.shReptileEye, darkena(0xD00000, 0, 0xFF));
      queuepoly(VABODY, cgi.shReptileTail, darkena(0xD08000, 0, 0xFF));
      return true;
      }
    
    case moFrog: case moPhaser: case moVaulter: {
      ShadowV(V, cgi.shFrogBody);
      const shiftmatrix VL = GDIM == 3 ? V : at_smart_lof(V, cgi.ALEG0);
      color_t xcolor = darkena(0xFF0000, 1, 0xFF);
      int alpha = (m == moPhaser ? 0xC0 : 0xFF);
      if(footphase) {
        queuepoly(VL, cgi.shFrogJumpFoot, darkena(col, 0, alpha));
        queuepoly(VL * lmirror(), cgi.shFrogJumpFoot, darkena(col, 0, alpha));
        queuepoly(VALEGS, cgi.shFrogJumpLeg, xcolor);
        queuepoly(VALEGS * lmirror(), cgi.shFrogJumpLeg, xcolor);
        }
      else {
        queuepoly(VL, cgi.shFrogRearFoot, darkena(col, 0, alpha));
        queuepoly(VL * lmirror(), cgi.shFrogRearFoot, darkena(col, 0, alpha));
        queuepoly(VALEGS, cgi.shFrogRearLeg, xcolor);
        queuepoly(VALEGS * lmirror(), cgi.shFrogRearLeg, xcolor);
        queuepoly(VALEGS, cgi.shFrogRearLeg2, xcolor);
        queuepoly(VALEGS * lmirror(), cgi.shFrogRearLeg2, xcolor);
        }
      queuepoly(VL, cgi.shFrogFrontFoot, darkena(col, 0, alpha));
      queuepoly(VL * lmirror(), cgi.shFrogFrontFoot, darkena(col, 0, alpha));
      queuepoly(VALEGS, cgi.shFrogFrontLeg, xcolor);
      queuepoly(VALEGS * lmirror(), cgi.shFrogFrontLeg, xcolor);
      queuepoly(VABODY, cgi.shFrogBody, darkena(col, 0, alpha));
      queuepoly(VABODY, cgi.shFrogEye, darkena(col, 3, alpha));
      queuepoly(VABODY * lmirror(), cgi.shFrogEye, darkena(col, 3, alpha));
      queuepoly(VABODY, cgi.shFrogStripe, xcolor);
      return true;
      }
    
    case moVineBeast: {
      ShadowV(V, cgi.shWolfBody);
      if(mmspatial || footphase)
        animallegs(VALEGS, moWolf, 0x00FF00FF, footphase);
      else
        queuepoly(VALEGS, cgi.shWolfLegs, 0x00FF00FF);
      queuepoly(VABODY, cgi.shWolfBody, darkena(col, 1, 0xFF));
      queuepoly(VAHEAD, cgi.shWolfHead, darkena(col, 0, 0xFF));
      queuepoly(VAHEAD, cgi.shWolfEyes, 0xFF0000FF);
      return true;
      }
    
    case moMouse: case moMouseMoved: {
      queuepoly(VALEGS, cgi.shMouse, darkena(col, 0, 0xFF));
      queuepoly(VALEGS, cgi.shMouseLegs, darkena(col, 1, 0xFF));
      queuepoly(VALEGS, cgi.shMouseEyes, 0xFF);
      return true;
      }
    
    case moRunDog: case moHunterDog: case moHunterGuard: case moHunterChanging: case moFallingDog: {
      if(!mmspatial && !footphase) 
        queuepoly(VABODY, cgi.shDogBody, darkena(col, 0, 0xFF));
      else {
        ShadowV(V, cgi.shDogTorso);
        queuepoly(VABODY, cgi.shDogTorso, darkena(col, 0, 0xFF));
        animallegs(VALEGS, moRunDog, m == moFallingDog ? 0xFFFFFFFF : darkena(col, 0, 0xFF), footphase);
        }
      queuepoly(VAHEAD, cgi.shDogHead, darkena(col, 0, 0xFF));
  
      {
      dynamicval<color_t> dp(poly_outline);
      int eyecolor = 0x202020;
      bool redeyes = false;
      if(m == moHunterDog) eyecolor = 0xFF0000, redeyes = true;
      if(m == moHunterGuard) eyecolor = 0xFF6000, redeyes = true;
      if(m == moHunterChanging) eyecolor = 0xFFFF00, redeyes = true;
      int eyes = darkena(eyecolor, 0, 0xFF);
  
      if(redeyes) poly_outline = eyes;
      queuepoly(VAHEAD, cgi.shWolf1, eyes).flags |= POLY_FORCEWIDE;
      queuepoly(VAHEAD, cgi.shWolf2, eyes).flags |= POLY_FORCEWIDE;
      }
      queuepoly(VAHEAD, cgi.shWolf3, darkena(m == moRunDog ? 0x202020 : 0x000000, 0, 0xFF));
      return true;
      }
    
    case moOrangeDog: {
      if(!mmspatial && !footphase) 
        queuepoly(VABODY, cgi.shDogBody, darkena(0xFFFFFF, 0, 0xFF));
      else {
        ShadowV(V, cgi.shDogTorso);
        if(GDIM == 2) queuepoly(VABODY, cgi.shDogTorso, darkena(0xFFFFFF, 0, 0xFF));
        animallegs(VALEGS, moRunDog, darkena(0xFFFFFF, 0, 0xFF), footphase);
        }
      queuepoly(VAHEAD, cgi.shDogHead, darkena(0xFFFFFF, 0, 0xFF));
      queuepoly(VABODY, cgi.shDogStripes, GDIM == 2 ? darkena(0x303030, 0, 0xFF) : 0xFFFFFFFF);
      queuepoly(VAHEAD, cgi.shWolf1, darkena(0x202020, 0, 0xFF));
      queuepoly(VAHEAD, cgi.shWolf2, darkena(0x202020, 0, 0xFF));
      queuepoly(VAHEAD, cgi.shWolf3, darkena(0x202020, 0, 0xFF));
      return true;
      }
    
    case moShark: case moGreaterShark: case moCShark: 
      queuepoly(VFISH, cgi.shShark, darkena(col, 0, 0xFF));
      return true;
      
    case moPike:
      queuepoly(VFISH, cgi.shPikeBody, darkena(col, 0, 0xFF));
      queuepoly(VFISH, cgi.shPikeEye, darkena(col, 2, 0xFF));
      queuepoly(VFISH * lmirror(), cgi.shPikeEye, darkena(col, 2, 0xFF));
      return true;

    case moEagle: case moParrot: case moBomberbird: case moAlbatross:
    case moTameBomberbird: case moWindCrow: case moTameBomberbirdMoved:
    case moSandBird: case moAcidBird: {
      ShadowV(V, cgi.shEagle);
      auto& sh = GDIM == 3 ? cgi.shAnimatedEagle[wingphase(200)] : cgi.shEagle;
      if(m == moParrot && GDIM == 3)
        queuepolyat(VBIRD, sh, darkena(col, 0, 0xFF), PPR::SUPERLINE);
      else
        queuepoly(VBIRD, sh, darkena(col, 0, 0xFF));
      return true;
      }
    
    case moSparrowhawk: case moWestHawk: {
      ShadowV(V, cgi.shHawk);
      auto& sh = GDIM == 3 ? cgi.shAnimatedHawk[wingphase(200)] : cgi.shHawk;
      queuepoly(VBIRD, sh, darkena(col, 0, 0xFF));
      return true;
      }
    
    case moButterfly: {
      transmatrix Vwing = wingmatrix(100);
      ShadowV(V * Vwing, cgi.shButterflyWing);
      if(GDIM == 2)
        queuepoly(VBIRD * Vwing, cgi.shButterflyWing, darkena(col, 0, 0xFF));
      else
        queuepoly(VBIRD, cgi.shAnimatedButterfly[wingphase(100)], darkena(col, 0, 0xFF));
      queuepoly(VBIRD, cgi.shButterflyBody, darkena(col, 2, 0xFF));
      return true;
      }
    
    case moGadfly: {
      transmatrix Vwing = wingmatrix(100);
      ShadowV(V * Vwing, cgi.shGadflyWing);
      queuepoly(VBIRD * Vwing, GDIM == 2 ? cgi.shGadflyWing : cgi.shAnimatedGadfly[wingphase(100)], darkena(col, 0, 0xFF));
      queuepoly(VBIRD, cgi.shGadflyBody, darkena(col, 1, 0xFF));
      queuepoly(VBIRD, cgi.shGadflyEye, darkena(col, 2, 0xFF));
      queuepoly(VBIRD * lmirror(), cgi.shGadflyEye, darkena(col, 2, 0xFF));
      return true;
      }
    
    case moVampire: case moBat: {
      // vampires have no shadow and no mirror images
      if(m == moBat) ShadowV(V, cgi.shBatWings);
      if(m == moBat || !inmirrorcount) {
        queuepoly(VBIRD, GDIM == 2 ? cgi.shBatWings : cgi.shAnimatedBat[wingphase(100)], darkena(0x303030, 0, 0xFF));
        queuepoly(VBIRD, GDIM == 2 ? cgi.shBatBody : cgi.shAnimatedBat2[wingphase(100)], darkena(0x606060, 0, 0xFF));
        }
      /* queuepoly(V, cgi.shBatMouth, darkena(0xC00000, 0, 0xFF));
      queuepoly(V, cgi.shBatFang, darkena(0xFFC0C0, 0, 0xFF));
      queuepoly(V*lmirror(), cgi.shBatFang, darkena(0xFFC0C0, 0, 0xFF));
      queuepoly(V, cgi.shBatEye, darkena(00000000, 0, 0xFF));
      queuepoly(V*lmirror(), cgi.shBatEye, darkena(00000000, 0, 0xFF)); */
      return true;
      }
    
    case moGargoyle: {
      ShadowV(V, cgi.shGargoyleWings);
      queuepoly(VBIRD, GDIM == 2 ? cgi.shGargoyleWings : cgi.shAnimatedGargoyle[wingphase(300)], darkena(col, 0, 0xD0));
      queuepoly(VBIRD, GDIM == 2 ? cgi.shGargoyleBody : cgi.shAnimatedGargoyle2[wingphase(300)], darkena(col, 0, 0xFF));
      return true;
      }
    
    case moZombie: {
      int c = darkena(col, where && where->land == laHalloween ? 1 : 0, 0xFF);
      const transmatrix VBS = otherbodyparts(V, c, m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBODY * VBS, cgi.shPBody, c);
      return true;
      }
    
    case moTerraWarrior: {
      drawTerraWarrior(V, 7, (where ? where->hitpoints : 7), footphase);
      return true;
      }
    
    case moVariantWarrior: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, darkena(0xFFD500, 0, 0xF0));
      if(!peace::on) queuepoly(VBS, cgi.shPSword, 0xFFFF00FF);
      queuepoly(VHEAD, cgi.shHood, 0x008000FF);
      humanoid_eyes(V, 0xFFFF00FF);
      return true;
      }
    
    case moDesertman: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, darkena(col, 0, 0xC0));
      if(!peace::on) queuepoly(VBS, cgi.shPSword, 0xFFFF00FF);
      queuepoly(VHEAD, cgi.shHood, 0xD0D000C0 | UNTRANS);
      humanoid_eyes(V, 0x301800FF);
      return true;
      }
    
    case moMonk: {
      const transmatrix VBS = otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
      ShadowV(V, cgi.shRaiderBody);
      queuepoly(VBODY * VBS, cgi.shRaiderBody, darkena(col, 0, 0xFF));
      queuepoly(VBODY1 * VBS, cgi.shRaiderShirt, darkena(col, 2, 0xFF));
      if(!peace::on) queuepoly(VBODY * VBS, cgi.shPKnife, 0xFFC0C0C0);
      queuepoly(VBODY2 * VBS, cgi.shRaiderArmor, darkena(col, 1, 0xFF));
      queuepolyat(VBODY3 * VBS, cgi.shRatCape2, darkena(col, 2, 0xFF), PPR::MONSTER_ARMOR0);
      queuepoly(VHEAD1, cgi.shRaiderHelmet, darkena(col, 0, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, darkena(0xC0C0A0, 0, 0XFF));
      humanoid_eyes(V, 0x000000FF);
      return true;
      }
    
    case moCrusher: {
      const transmatrix VBS = otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
      ShadowV(V, cgi.shRaiderBody);
      queuepoly(VBODY * VBS, cgi.shRaiderBody, darkena(col, 0, 0xFF));
      queuepoly(VBODY1 * VBS, cgi.shRaiderShirt, darkena(col, 2, 0xFF));
      queuepoly(VBODY2 * VBS, cgi.shRaiderArmor, darkena(col, 1, 0xFF));
      queuepoly(VBODY * VBS, cgi.shFlailTrunk, darkena(col, 1, 0XFF));
      queuepoly(VBODY1 * VBS, cgi.shHammerHead, darkena(col, 0, 0XFF));
      queuepoly(VHEAD1, cgi.shRaiderHelmet, darkena(col, 0, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, darkena(0xC0C0A0, 0, 0XFF));
      humanoid_eyes(V, 0x000000FF);
      return true;
      }
    
    case moPair: {
      const transmatrix VBS = otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
      ShadowV(V, cgi.shRaiderBody);
      queuepoly(VBODY * VBS, cgi.shRaiderBody, darkena(col, 0, 0xFF));
      queuepoly(VBODY1 * VBS, cgi.shRaiderShirt, darkena(col, 2, 0xFF));
      queuepoly(VBODY2 * VBS, cgi.shRaiderArmor, darkena(col, 1, 0xFF));
      queuepoly(VBODY * VBS, cgi.shPickAxe, darkena(0xA0A0A0, 0, 0XFF));
      queuepoly(VHEAD1, cgi.shRaiderHelmet, darkena(col, 0, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, darkena(0xC0C0A0, 0, 0XFF));
      humanoid_eyes(V, 0x000000FF);
      return true;
      }
    
    case moAltDemon: case moHexDemon: {
      const transmatrix VBS = otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
      ShadowV(V, cgi.shRaiderBody);
      queuepoly(VBODY * VBS, cgi.shRaiderBody, darkena(col, 0, 0xFF));
      queuepoly(VBODY1 * VBS, cgi.shRaiderShirt, darkena(col, 2, 0xFF));
      queuepoly(VBODY2 * VBS, cgi.shRaiderArmor, darkena(col, 1, 0xFF));
      if(!peace::on) queuepoly(VBODY * VBS, cgi.shPSword, 0xFFD0D0D0);
      queuepoly(VHEAD1, cgi.shRaiderHelmet, darkena(col, 0, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, darkena(0xC0C0A0, 0, 0XFF));
      humanoid_eyes(V, 0x000000FF);
      return true;
      }
    
    case moSkeleton: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(0xFFFFFF, 0, 0xFF), moSkeleton, footphase);
      queuepoly(VBS, cgi.shSkeletonBody, darkena(0xFFFFFF, 0, 0xFF));
      if(GDIM == 2) queuepoly(VHEAD, cgi.shSkull, darkena(0xFFFFFF, 0, 0xFF));
      if(GDIM == 2) queuepoly(VHEAD1, cgi.shSkullEyes, 0x000000FF);
      humanoid_eyes(V, 0x000000FF, 0xFFFFFFFF);      
      ShadowV(V, cgi.shSkeletonBody);
      queuepoly(VBS, cgi.shSabre, 0xFFFFFFFF);
      return true;
      }
    
    case moPalace: case moFatGuard: case moVizier: {
      ShadowV(V, cgi.shPBody);
      const transmatrix VBS = otherbodyparts(V, darkena(0xFFD500, 0, 0xFF), m, footphase);
      if(m == moFatGuard) {
        queuepoly(VBODY * VBS, cgi.shFatBody, darkena(0xC06000, 0, 0xFF));
        col = 0xFFFFFF;
        if(!where || where->hitpoints >= 3)
          queuepoly(VBODY1 * VBS, cgi.shKnightCloak, darkena(0xFFC0C0, 1, 0xFF));
        }
      else {
        queuepoly(VBODY * VBS, cgi.shPBody, darkena(0xFFD500, 0, 0xFF));
        queuepoly(VBODY1 * VBS, cgi.shKnightArmor, m == moVizier ? 0xC000C0FF :
          darkena(0x00C000, 1, 0xFF));
        if(where && where->hitpoints >= 3)
          queuepoly(VBODY2 * VBS, cgi.shKnightCloak, m == moVizier ? 0x800080Ff :
            darkena(0x00FF00, 1, 0xFF));
        }
      queuepoly(VHEAD1, cgi.shTurban1, darkena(col, 1, 0xFF));
      if(!where || where->hitpoints >= 2)
        queuepoly(VHEAD2, cgi.shTurban2, darkena(col, 0, 0xFF));
      queuepoly(VBODY * VBS, cgi.shSabre, 0xFFFFFFFF);
      humanoid_eyes(V, 0x301800FF);
      return true;
      }
    
    case moCrystalSage: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, 0xFFFFFFFF, m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, 0xFFFFFFFF);
      queuepoly(VHEAD1, cgi.shPHead, 0xFFFFFFFF);
      queuepoly(VHEAD, cgi.shPFace, 0xFFFFFFFF);
      humanoid_eyes(V, 0xFFFFFFFF, 0xC0C0C0FF);
      return true;
      }
    
    case moHedge: {
      ShadowV(V, cgi.shPBody);
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
      queuepoly(VBS, cgi.shPBody, darkena(col, 0, 0xFF));
      queuepoly(VBS, cgi.shHedgehogBlade, 0xC0C0C0FF);
      queuepoly(VHEAD1, cgi.shPHead, 0x804000FF);
      queuepoly(VHEAD, cgi.shPFace, 0xF09000FF);
      humanoid_eyes(V, 0x00D000FF);
      return true;
      }
    
    case moYeti: case moMonkey: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shYeti, darkena(col, 0, 0xC0));
      queuepoly(VHEAD1, cgi.shPHead, darkena(col, 0, 0xFF));
      humanoid_eyes(V, 0x000000FF, darkena(col, 0, 0xFF));
      return true;
      }
    
    case moResearcher: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, darkena(0xFFFF00, 0, 0xC0));
      queuepoly(VHEAD, cgi.shAztecHead, darkena(col, 0, 0xFF));
      queuepoly(VHEAD1, cgi.shAztecCap, darkena(0xC000C0, 0, 0xFF));
      humanoid_eyes(V, 0x000000FF);
      return true;
      }
    
    case moFamiliar: {
      ShadowV(V, cgi.shWolfBody);
      queuepoly(VABODY, cgi.shWolfBody, darkena(0xA03000, 0, 0xFF));
       if(mmspatial || footphase)
         animallegs(VALEGS, moWolf, darkena(0xC04000, 0, 0xFF), footphase);
       else
        queuepoly(VALEGS, cgi.shWolfLegs, darkena(0xC04000, 0, 0xFF));
      
      queuepoly(VAHEAD, cgi.shFamiliarHead, darkena(0xC04000, 0, 0xFF));
      // queuepoly(V, cgi.shCatLegs, darkena(0x902000, 0, 0xFF));
      if(true) {
        queuepoly(VAHEAD, cgi.shFamiliarEye, darkena(0xFFFF00, 0, 0xFF));
        queuepoly(VAHEAD * lmirror(), cgi.shFamiliarEye, darkena(0xFFFF00, 0, 0xFF));
        }
      return true;
      }
    
    case moRanger: {
      ShadowV(V, cgi.shPBody);
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      queuepoly(VBS, cgi.shPBody, darkena(col, 0, 0xC0));
      if(!peace::on) queuepoly(VBS, cgi.shPSword, darkena(col, 0, 0xFF));
      queuepoly(VHEAD, cgi.shArmor, darkena(col, 1, 0xFF));
      humanoid_eyes(V, 0x000000FF);
      return true;
      }
    
    case moNarciss: {
      ShadowV(V, cgi.shPBody);
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      queuepoly(VBS, cgi.shFlowerHand, darkena(col, 0, 0xFF));
      queuepoly(VBS, cgi.shPBody, 0xFFE080FF);
      if(!peace::on) queuepoly(VBS, cgi.shPKnife, 0xC0C0C0FF);
      queuepoly(VHEAD, cgi.shPFace, 0xFFE080FF);
      queuepoly(VHEAD1, cgi.shPHead, 0x806A00FF);
      humanoid_eyes(V, 0x000000FF);
      return true;
      }

    case moMirrorSpirit: {
      ShadowV(V, cgi.shPBody);
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0x90), m, footphase);
      queuepoly(VBS, cgi.shPBody, darkena(col, 0, 0x90));
      if(!peace::on) queuepoly(VBS * lmirror(), cgi.shPSword, darkena(col, 0, 0xD0));
      queuepoly(VHEAD1, cgi.shPHead, darkena(col, 1, 0x90));
      queuepoly(VHEAD2, cgi.shPFace, darkena(col, 1, 0x90));
      queuepoly(VHEAD, cgi.shArmor, darkena(col, 0, 0xC0));
      humanoid_eyes(V, 0xFFFFFFFF, darkena(col, 0, 0xFF));
      return true;
      }
    
    case moJiangshi: {
      ShadowV(V, cgi.shJiangShi);
      auto z2 = WDIM == 3 ? 0 : GDIM == 3 ? -abs(sin(footphase * TAU)) * cgi.human_height/3 : geom3::lev_to_factor(abs(sin(footphase * TAU)) * cgi.human_height);
      auto V0 = V;
      auto V = at_smart_lof(V0, z2);
      otherbodyparts(V, darkena(col, 0, 0xFF), m, m == moJiangshi ? 0 : footphase);
      queuepoly(VBODY, cgi.shJiangShi, darkena(col, 0, 0xFF));
      queuepoly(VBODY1, cgi.shJiangShiDress, darkena(0x202020, 0, 0xFF));
      queuepoly(VHEAD, cgi.shTerraHead, darkena(0x101010, 0, 0xFF));
      queuepoly(VHEAD1, cgi.shPFace, darkena(col, 0, 0xFF));
      queuepoly(VHEAD2, cgi.shJiangShiCap1, darkena(0x800000, 0, 0xFF));
      queuepoly(VHEAD3, cgi.shJiangShiCap2, darkena(0x400000, 0, 0xFF));
      humanoid_eyes(V, 0x000000FF, darkena(col, 0, 0xFF));
      return true;
      }
    
    case moGhost: case moSeep: case moFriendlyGhost: {
      if(m == moFriendlyGhost) col = fghostcolor(where);
      queuepolyat(VGHOST, cgi.shGhost, darkena(col, 0, m == moFriendlyGhost ? 0xC0 : 0x80), GDIM == 3 ? PPR::SUPERLINE : cgi.shGhost.prio);
      queuepolyat(VGHOST, cgi.shGhostEyes, 0xFF, GDIM == 3 ? PPR::SUPERLINE : cgi.shEyes.prio);
      return true;
      }
    
    case moVineSpirit: {
      queuepoly(VGHOST, cgi.shGhost, 0xD0D0D0C0 | UNTRANS);
      queuepolyat(VGHOST, cgi.shGhostEyes, 0xFF0000FF, GDIM == 3 ? PPR::SUPERLINE : cgi.shGhostEyes.prio);
      return true;
      }
    
    case moFireFairy: {
      col = firecolor(0);
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shFemaleBody);
      queuepoly(VBS, cgi.shFemaleBody, darkena(col, 0, 0XC0));
      queuepoly(VHEAD, cgi.shWitchHair, darkena(col, 1, 0xFF));
      queuepoly(VHEAD1, cgi.shPFace, darkena(col, 0, 0XFF));
      humanoid_eyes(V, darkena(col, 1, 0xFF));
      return true;
      }      
      
    case moRusalka: {
      col = watercolor(0);
      bool girl = princessgender() == GEN_F;
      if(girl) {
        const shiftmatrix VBS = VBODY * otherbodyparts(V, col, m, footphase);
        ShadowV(V, cgi.shFemaleBody);
        queuepoly(VBS, cgi.shFemaleBody, watercolor(100));
        queuepoly(VHEAD1, cgi.shFemaleHair,  watercolor(150));
        // queuepoly(VHEAD2, cgi.shFlowerHair,  watercolor(50));
        // queuepoly(VHEAD, cgi.shWitchHair, watercolor(150));
        queuepoly(VHEAD1, cgi.shPFace, watercolor(200));
        queuepoly(VHEAD1, cgi.shWightCloak, watercolor(50) & 0xFFFFFF80);
        humanoid_eyes(V, col | 0xFF);
        }
      else {
        const shiftmatrix VBS = VBODY * otherbodyparts(V, col, m, footphase);
        ShadowV(V, cgi.shPBody);
        queuepoly(VBS, cgi.shPBody, watercolor(100));

        queuepoly(VBS, cgi.shSuspenders,  watercolor(150));

        queuepoly(VHEAD1, cgi.shPHead,  watercolor(50));
        queuepoly(VHEAD1, cgi.shPFace, watercolor(200));
        queuepoly(VHEAD1, cgi.shWightCloak, watercolor(50) & 0xFFFFFF80);
        humanoid_eyes(V, col | 0xFF);
        }
      return true;
      }      
      
    case moSlime: {
      queuepoly(VFISH, cgi.shSlime, darkena(col, 0, 0x80));
      queuepoly(VSLIMEEYE, cgi.shSlimeEyes, 0xFF);
      return true;
      }

    case moKrakenH: {
      queuepoly(VFISH, cgi.shKrakenHead, darkena(col, 0, 0xD0));
      queuepoly(VFISH, cgi.shKrakenEye, 0xFFFFFFC0 | UNTRANS);
      queuepoly(VFISH, cgi.shKrakenEye2, 0xC0);
      queuepoly(VFISH * lmirror(), cgi.shKrakenEye, 0xFFFFFFC0 | UNTRANS);
      queuepoly(VFISH * lmirror(), cgi.shKrakenEye2, 0xC0);
      return true;
      }

    case moKrakenT: {
      queuepoly(VFISH, cgi.shSeaTentacle, darkena(col, 0, 0xD0));
      return true;
      }

    case moCultist: case moPyroCultist: case moCultistLeader: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, darkena(col, 0, 0xC0));
      if(!peace::on) queuepoly(VBS, cgi.shPSword, darkena(col, 2, 0xFF));
      queuepoly(VHEAD, cgi.shHood, darkena(col, 1, 0xFF));
      humanoid_eyes(V, 0x00FF00FF);
      return true;
      }

    case moPirate: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, darkena(0x404040, 0, 0xFF));
      queuepoly(VBS, cgi.shPirateHook, darkena(0xD0D0D0, 0, 0xFF));
      queuepoly(VHEAD, cgi.shPFace, darkena(0xFFFF80, 0, 0xFF));
      queuepoly(VHEAD1, cgi.shEyepatch, darkena(0, 0, 0xC0));
      queuepoly(VHEAD2, cgi.shPirateHood, darkena(col, 0, 0xFF));
      humanoid_eyes(V, 0x000000FF);
      return true;
      }

    case moRatling: case moRatlingAvenger: {
      const transmatrix VBS = otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shYeti);
      queuepoly(VLEG, cgi.shRatTail, darkena(col, 0, 0xFF));
      queuepoly(VBODY * VBS, cgi.shYeti, darkena(col, 1, 0xFF));
  
      float t = sintick(1000, where ? where->cpdist*M_PI : 0);
      int eyecol = t > 0.92 ? 0xFF0000 : 0;
      
      if(GDIM == 2) {
        queuepoly(VHEAD, cgi.shRatHead, darkena(col, 0, 0xFF));
        queuepoly(VHEAD, cgi.shWolf1, darkena(eyecol, 0, 0xFF));
        queuepoly(VHEAD, cgi.shWolf2, darkena(eyecol, 0, 0xFF));
        queuepoly(VHEAD, cgi.shWolf3, darkena(0x202020, 0, 0xFF));
        if(m == moRatlingAvenger) queuepoly(VHEAD1, cgi.shRatCape1, 0x303030FF);
        }
#if MAXMDIM >= 4
      else {
        shiftmatrix V1 = V * lzpush(cgi.AHEAD - zc(0.4) - zc(0.98) + cgi.HEAD); // * cpush(0, cgi.scalefactor * (-0.1));
        queuepoly(V1, cgi.shRatHead, darkena(col, 0, 0xFF));

        /*
        queuepoly(V1, cgi.shFamiliarEye, darkena(eyecol, 0, 0xFF));
        queuepoly(V1 * lmirror(), cgi.shFamiliarEye, darkena(eyecol, 0, 0xFF));
        queuepoly(V1, cgi.shWolfEyes, darkena(col, 3, 0xFF));
        */
        queuepoly(V1, cgi.shRatEye1, darkena(eyecol, 0, 0xFF));
        queuepoly(V1, cgi.shRatEye2, darkena(eyecol, 0, 0xFF));
        queuepoly(V1, cgi.shRatEye3, darkena(0x202020, 0, 0xFF));
        if(m == moRatlingAvenger) queuepoly(V1, cgi.shRatCape1, 0x303030FF);
        }
#endif      
      if(m == moRatlingAvenger) {
        queuepoly(VBODY1 * VBS, cgi.shRatCape2, 0x484848FF);
        }
      return true;
      }

    case moViking: {
      const transmatrix VBS = otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBODY * VBS, cgi.shPBody, darkena(0xE00000, 0, 0xFF));
      if(!peace::on) queuepoly(VBODY * VBS, cgi.shPSword, darkena(0xD0D0D0, 0, 0xFF));
      queuepoly(VBODY1 * VBS, cgi.shKnightCloak, darkena(0x404040, 0, 0xFF));
      queuepoly(VHEAD, cgi.shVikingHelmet, darkena(0xC0C0C0, 0, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, darkena(0xFFFF80, 0, 0xFF));
      humanoid_eyes(V, 0x000000FF);
      return true;
      }

    case moOutlaw: {
      const transmatrix VBS = otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBODY * VBS, cgi.shPBody, darkena(col, 0, 0xFF));
      queuepoly(VBODY1 * VBS, cgi.shKnightCloak, darkena(col, 1, 0xFF));
      queuepoly(VHEAD1, cgi.shWestHat1, darkena(col, 2, 0XFF));
      queuepoly(VHEAD2, cgi.shWestHat2, darkena(col, 1, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, darkena(0xFFFF80, 0, 0xFF));
      queuepoly(VBODY * VBS, cgi.shGunInHand, darkena(col, 1, 0XFF));
      humanoid_eyes(V, 0x000000FF);
      return true;
      }

    case moNecromancer: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, 0xC00000C0 | UNTRANS);
      queuepoly(VHEAD, cgi.shHood, darkena(col, 1, 0xFF));
      humanoid_eyes(V, 0xFF0000FF);
      return true;
      }

    case moDraugr: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, 0x483828D0 | UNTRANS, m, footphase);
      queuepoly(VBS, cgi.shPBody, 0x483828D0 | UNTRANS);
      queuepoly(VBS, cgi.shPSword, 0xFFFFD0A0 | UNTRANS);
      queuepoly(VHEAD, cgi.shPHead, 0x483828D0 | UNTRANS);
      humanoid_eyes(V, 0xFF0000FF, 0x483828FF);
      // queuepoly(V, cgi.shSkull, 0xC06020D0);
      //queuepoly(V, cgi.shSkullEyes, 0x000000D0);
  //  queuepoly(V, cgi.shWightCloak, 0xC0A080A0);
      int b = where ? where->cpdist : 0;
      b--;
      if(b < 0) b = 0;
      if(b > 6) b = 6;
      queuepoly(VHEAD1, cgi.shWightCloak, color_t(0x605040A0 | UNTRANS) + color_t(0x10101000 * b));
      return true;
      }

    case moVoidBeast: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, 0x080808D0 | UNTRANS, m, footphase);
      queuepoly(VBS, cgi.shPBody, 0x080808D0 | UNTRANS);
      queuepoly(VHEAD, cgi.shPHead, 0x080808D0 | UNTRANS);
      queuepoly(VHEAD, cgi.shWightCloak, 0xFF0000A0 | UNTRANS);
      humanoid_eyes(V, 0xFF0000FF, 0x080808FF);
      return true;
      }

    case moGoblin: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shYeti);
      queuepoly(VBS, cgi.shYeti, darkena(col, 0, 0xC0));
      queuepoly(VHEAD, cgi.shArmor, darkena(col, 1, 0XFF));
      humanoid_eyes(V, 0x800000FF, darkena(col, 0, 0xFF));
      return true;
      }

    case moLancer: case moFlailer: case moMiner: {
      shiftmatrix V2 = V;
      if(m == moLancer)
        V2 = V * spin((where && where->type == 6) ? -60._deg : -90._deg );
      shiftmatrix Vh = at_smart_lof(V2, cgi.HEAD);
      shiftmatrix Vb = at_smart_lof(V2, cgi.BODY);
      Vb = Vb * otherbodyparts(V2, darkena(col, 1, 0xFF), m, footphase);
      ShadowV(V2, cgi.shPBody);
      queuepoly(Vb, cgi.shPBody, darkena(col, 0, 0xC0));
      queuepoly(Vh, m == moFlailer ? cgi.shArmor : cgi.shHood, darkena(col, 1, 0XFF));
      if(m == moMiner)
        queuepoly(Vb, cgi.shPickAxe, darkena(0xC0C0C0, 0, 0XFF));
      if(m == moLancer)
        queuepoly(Vb, cgi.shPike, darkena(col, 0, 0XFF));
      if(m == moFlailer) {
        queuepoly(Vb, cgi.shFlailBall, darkena(col, 0, 0XFF));
        queuepoly(Vb, cgi.shFlailChain, darkena(col, 1, 0XFF));
        queuepoly(Vb, cgi.shFlailTrunk, darkena(col, 0, 0XFF));
        }
      humanoid_eyes(V, 0x000000FF);
      return true;
      }

    case moTroll: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shYeti);
      queuepoly(VBS, cgi.shYeti, darkena(col, 0, 0xC0));
      queuepoly(VHEAD1, cgi.shPHead, darkena(col, 1, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, darkena(col, 2, 0XFF));
      humanoid_eyes(V, 0x004000FF, darkena(col, 0, 0xFF));
      return true;
      }        

    case moFjordTroll: case moForestTroll: case moStormTroll: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shYeti);
      queuepoly(VBS, cgi.shYeti, darkena(col, 0, 0xC0));
      queuepoly(VHEAD1, cgi.shPHead, darkena(col, 1, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, darkena(col, 2, 0XFF));
      humanoid_eyes(V, 0x004000FF, darkena(col, 0, 0xFF));
      return true;
      }

    case moDarkTroll: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shYeti);
      queuepoly(VBS, cgi.shYeti, darkena(col, 0, 0xC0));
      queuepoly(VHEAD1, cgi.shPHead, darkena(col, 1, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, 0xFFFFFF80 | UNTRANS);
      humanoid_eyes(V, 0x000000FF, darkena(col, 0, 0xFF));
      return true;
      }        

    case moRedTroll: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shYeti);
      queuepoly(VBS, cgi.shYeti, darkena(0xFF8000, 0, 0XFF));
      queuepoly(VHEAD1, cgi.shPHead, darkena(col, 0, 0xC0));
      queuepoly(VHEAD, cgi.shPFace, 0xFFFFFF80 | UNTRANS);
      humanoid_eyes(V, 0x000000FF, darkena(col, 0, 0xFF));
      return true;
      }        

    case moEarthElemental: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
      ShadowV(V, cgi.shWaterElemental);
      queuepoly(VBS, cgi.shWaterElemental, darkena(col, 0, 0xC0));
      queuepoly(VHEAD1, cgi.shFemaleHair, darkena(col, 0, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, 0xF0000080 | UNTRANS);
      humanoid_eyes(V, 0xD0D000FF, darkena(col, 1, 0xFF));
      return true;
      }        

    case moWaterElemental: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, watercolor(50), m, footphase);
      ShadowV(V, cgi.shWaterElemental);
      queuepoly(VBS, cgi.shWaterElemental, watercolor(0));
      queuepoly(VHEAD1, cgi.shFemaleHair, watercolor(100));
      queuepoly(VHEAD, cgi.shPFace, watercolor(200));
      humanoid_eyes(V, 0x0000FFFF, watercolor(150));
      return true;
      }        

    case moFireElemental: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(firecolor(50), 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shWaterElemental);
      queuepoly(VBS, cgi.shWaterElemental, darkena(firecolor(0), 0, 0xFF));
      queuepoly(VHEAD1, cgi.shFemaleHair, darkena(firecolor(100), 0, 0xFF));
      queuepoly(VHEAD, cgi.shPFace, darkena(firecolor(200), 0, 0xFF));
      humanoid_eyes(V, darkena(firecolor(200), 0, 0xFF), darkena(firecolor(50), 0, 0xFF));
      return true;
      }

    case moAirElemental: {
      const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0x40), m, footphase);
      ShadowV(V, cgi.shWaterElemental);
      queuepoly(VBS, cgi.shWaterElemental, darkena(col, 0, 0x80));
      queuepoly(VHEAD1, cgi.shFemaleHair, darkena(col, 0, 0x80));
      queuepoly(VHEAD, cgi.shPFace, darkena(col, 0, 0x80));
      humanoid_eyes(V, 0xFFFFFFFF, darkena(col, 1, 0xFF));
      return true;
      }        

    case moWorm: case moWormwait: case moHexSnake: {
      queuepoly(V, cgi.shWormHead, darkena(col, 0, 0xFF));
      queuepolyat(V, cgi.shWormEyes, 0xFF, PPR::ONTENTACLE_EYES);
      return true;
      }

    case moDragonHead: {
      queuepoly(V, cgi.shDragonHead, darkena(col, 0, 0xFF));
      queuepolyat(V, cgi.shDragonEyes, 0xFF, PPR::ONTENTACLE_EYES);          
      int noscolor = 0xFF0000FF;
      queuepoly(V, cgi.shDragonNostril, noscolor);
      queuepoly(V * lmirror(), cgi.shDragonNostril, noscolor);
      return true;
      }

    case moDragonTail: {
      queuepoly(V, cgi.shDragonSegment, darkena(col, 0, 0xFF));
      return true;
      }

    case moTentacle: case moTentaclewait: case moTentacleEscaping: {
      queuepoly(V, cgi.shTentHead, darkena(col, 0, 0xFF));
      ShadowV(V, cgi.shTentHead, PPR::GIANTSHADOW);
      return true;
      }
    
    case moAsteroid: {
      queuepoly(V, cgi.shAsteroid[1], darkena(col, 0, 0xFF));
      return true;
      }
    
    #if CAP_COMPLEX2
    case moAnimatedDie: case moAngryDie: {
      if(where)
        dice::draw_die(where, V, 1, darkena(col, 0, 0xFF));
      else
        queuepoly(V, cgi.shDodeca, darkena(col, 0, 0xFF));
      return true;
      }
    #endif
      
    default: ;    
    }

  if(isPrincess(m)) goto princess;

  else if(isBull(m)) {
    ShadowV(V, cgi.shBullBody);
    int hoofcol = darkena(gradient(0, col, 0, .65, 1), 0, 0xFF);
    if(mmspatial || footphase)
      animallegs(VALEGS, moRagingBull, hoofcol, footphase);
    queuepoly(VABODY, cgi.shBullBody, darkena(gradient(0, col, 0, .80, 1), 0, 0xFF));
    queuepoly(VAHEAD, cgi.shBullHead, darkena(col, 0, 0xFF));
    queuepoly(VAHEAD, cgi.shBullHorn, darkena(0xFFFFFF, 0, 0xFF));
    queuepoly(VAHEAD * lmirror(), cgi.shBullHorn, darkena(0xFFFFFF, 0, 0xFF));
    }

  else if(isBug(m)) {
    ShadowV(V, cgi.shBugBody);
    if(!mmspatial && !footphase)
      queuepoly(VABODY, cgi.shBugBody, darkena(col, 0, 0xFF));
    else {
      animallegs(VALEGS, moBug0, darkena(col, 0, 0xFF), footphase);
      queuepoly(VABODY, cgi.shBugAntenna, darkena(col, 1, 0xFF));
      }
    queuepoly(VABODY, cgi.shBugArmor, darkena(col, 1, 0xFF));
    }
  else if(isSwitch(m)) {
    queuepoly(VFISH, cgi.shJelly, darkena(col, 0, 0xD0));
    queuepolyat(VBODY, cgi.shJelly, darkena(col, 0, 0xD0), PPR::MONSTER_BODY);
    queuepolyat(VHEAD, cgi.shJelly, darkena(col, 0, 0xD0), PPR::MONSTER_HEAD);
    queuepolyat(VHEAD, cgi.shSlimeEyes, 0xFF, PPR::MONSTER_HEAD);
    }
  else if(isDemon(m)) {
    const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    queuepoly(VBS, cgi.shPBody, darkena(col, 1, 0xC0));
    ShadowV(V, cgi.shPBody);
    int acol = col;
    if(xch == 'D') acol = 0xD0D0D0;
    queuepoly(VHEAD, cgi.shDemon, darkena(acol, 0, 0xFF));
    humanoid_eyes(V, 0xFF0000FF, 0xC00000FF);
    }
  else if(isMagneticPole(m)) {
    if(m == moNorthPole)
      queuepolyat(VBODY * spin180(), cgi.shTentacle, 0x000000C0, PPR::TENTACLE1);
    queuepolyat(VBODY, cgi.shDisk, darkena(col, 0, 0xFF), PPR::MONSTER_BODY);
    }
  else if(isMetalBeast(m) || m == moBrownBug) {
    ShadowV(V, cgi.shTrylobite);
    if(!mmspatial)
      queuepoly(VABODY, cgi.shTrylobite, darkena(col, 1, 0xC0));
    else {
      queuepoly(VABODY, cgi.shTrylobiteBody, darkena(col, 1, 0xFF));
      animallegs(VALEGS, moMetalBeast, darkena(col, 1, 0xFF), footphase);
      }
    int acol = col;
    queuepoly(VAHEAD, cgi.shTrylobiteHead, darkena(acol, 0, 0xFF));
    }
  else if(m == moHexer) {
    const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, cgi.shFemaleBody);
    queuepoly(VBS, cgi.shFemaleBody, darkena(0x800080, 0, 0xFF));
    queuepoly(VHEAD1, cgi.shWitchHair, darkena(0xFF00FF, 1, 0xFF));
    queuepoly(VHEAD, cgi.shPFace, darkena(0xFFFFFF, 0, 0xFF));
    queuepoly(VBS, cgi.shWitchDress, darkena(col, 1, 0XC0));
    humanoid_eyes(V, 0xF000F0FF);
    }
  else if(isWitch(m)) {
    const shiftmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    int cc = 0xFF;
    if(m == moWitchGhost) cc = 0x85 + 120 * sintick(160);
    if(m == moWitchWinter && where) drawWinter(V, 0, iinf[itOrbWinter].color);
    if(m == moWitchFlash && where) drawFlash(V);
    if(m == moWitchSpeed && where) drawSpeed(V);
    if(m == moWitchFire) col = firecolor(0);
    ShadowV(V, cgi.shFemaleBody);
    queuepoly(VBS, cgi.shFemaleBody, darkena(col, 0, cc));
//    queuepoly(cV2, ct, cgi.shPSword, darkena(col, 0, 0XFF));
//    queuepoly(V, cgi.shHood, darkena(col, 0, 0XC0));
    if(m == moWitchFire) col = firecolor(100);
    queuepoly(VHEAD1, cgi.shWitchHair, darkena(col, 1, cc));
    if(m == moWitchFire) col = firecolor(200);
    queuepoly(VHEAD, cgi.shPFace, darkena(col, 0, cc));
    if(m == moWitchFire) col = firecolor(300);
    queuepoly(VBS, cgi.shWitchDress, darkena(col, 1, 0XC0));
    humanoid_eyes(V, 0x000000FF);
    }

  // just for the HUD glyphs...
  else if(isAnyIvy(m)) {
    queuepoly(VBODY, cgi.shILeaf[0], darkena(col, 0, 0xFF));
    }

  else
    draw_ascii_or_zh(V1, minf[m].glyph, minf[m].name, asciicol, 1.5, 1);
  
  return true;
#endif
  }

bool drawMonsterTypeDH(eMonster m, cell *where, const shiftmatrix& V, color_t col, bool dh, ld footphase, color_t asciicol) {
  dynamicval<color_t> p(poly_outline, poly_outline);
  if(dh) {
    poly_outline = OUTLINE_DEAD;
    darken++;
    }
  bool b = drawMonsterType(m,where,V,col, footphase, asciicol);
  if(dh) {
    darken--;
    }
  return b;
  }

EX shiftmatrix playerV;

int last_wormsegment = -1;
vector<vector< function<void()> > > wormsegments;

void add_segment(int d, const function<void()>& s) {
  if(isize(wormsegments) <= d) wormsegments.resize(d+1);
  last_wormsegment = max(last_wormsegment, d);
  wormsegments[d].push_back(s);
  }

EX void drawWormSegments() {
  for(int i=0; i<=last_wormsegment; i++) {
    for(auto& f: wormsegments[i]) f();
    wormsegments[i].clear();
    }
  last_wormsegment = -1;
  }

EX bool dont_face_pc = false;

EX shiftmatrix die_target;

int taildist(cell *c) {
  int s = 0;
  while(s < 1000 && c && c->mondir != NODIR && isWorm(c->monst)) {
    s++; c = c->move(c->mondir);
    }
  return s;
  }

EX bool drawMonster(const shiftmatrix& Vparam, int ct, cell *c, color_t col, color_t asciicol) {
  #if CAP_SHAPES

  bool darkhistory = history::includeHistory && history::inkillhistory.count(c);
  color_t outline = OUTLINE_NONE;

  if(doHighlight()) {
    outline =
      (isPlayerOn(c) || isFriendly(c)) ? OUTLINE_FRIEND : 
      noHighlight(c->monst) ? OUTLINE_NONE :
      OUTLINE_ENEMY;
    poly_outline = outline;
    }
    
  // highlight faraway enemies if that's needed
  if (vid.faraway_highlight && c->cpdist >= 6 && vid.faraway_highlight <= get_threat_level(c)) {

    // basic red-green oscillation
    color_t r = ceil(255*abs(sintick(1000, 0.25)));
    color_t g = ceil(255*abs(sintick(1000, 0.00)));
    color_t hlc = (r<<16) + (g<<8);

    hlc = gradient(col, hlc, 0, vid.faraway_highlight_color, 100);
    
    if(c->cpdist == 6) 
      hlc = gradient(0, hlc, 0, 1, 1.5);
    
    addauraspecial(tC0(Vparam), hlc, 0);
    }

  bool nospins = false, nospinb = false;
  double footphaseb = 0, footphase = 0;
  
  shiftmatrix Vs = Vparam; nospins = applyAnimation(c, Vs, footphase, LAYER_SMALL);
  shiftmatrix Vb = Vparam; nospinb = applyAnimation(c, Vb, footphaseb, LAYER_BIG);
//  nospin = true;

  eMonster m = c->monst;
  
  bool half_elliptic = elliptic && GDIM == 3 && WDIM == 2;
  bool mirrored = det(Vparam.T) > 0;
  
  bool res = m;
  
  if(!m) ;
  
  else if(half_elliptic && mirrored != c->monmirror && !isMimic(m)) ;
    
  else if(isAnyIvy(c) || isWorm(c)) {
  
    if((m == moHexSnake || m == moHexSnakeTail) && c->hitpoints == 2) {
      int d = c->mondir;
      if(d == NODIR)
        forCellIdEx(c2, i, c)
          if(among(c2->monst, moHexSnakeTail, moHexSnake) && c2->mondir == c->c.spin(i))
            d = i;
      if(d == NODIR) { d = hrand(c->type); createMov(c, d); }
      int c1 = nestcolors[pattern_threecolor(c)];
      int c2 = nestcolors[pattern_threecolor(c->move(d))];
      col = (c1 + c2); // sum works because they are dark and should be brightened
      }

    if(isDragon(c->monst) && c->stuntime == 0) col = 0xFF6000;
    
    if(GDIM == 3)
      addradar(Vparam, minf[m].glyph, asciicol, isFriendly(m) ? 0x00FF00FF : 0xFF0000FF);
  
    shiftmatrix Vb0 = Vb;
    if(c->mondir != NODIR && GDIM == 3 && isAnyIvy(c)) {
      auto V1 = at_smart_lof(Vparam, cgi.ABODY);
      auto V2 = at_smart_lof(Vparam * currentmap->adj(c, c->mondir), cgi.ABODY);
      queueline(V1 * tile_center(), V2 * tile_center(), (col << 8) + 0xFF, 0);
      }
    else if(c->mondir != NODIR) {
      
      if(mmmon) {
        ld length;
        cell *c2 = c->move(c->mondir);
        
        if(nospinb) {
          chainAnimation(c, c2, Vb, Vparam * currentmap->adj(c, c->mondir), length);
          }
        else {
          Vb = Vb * ddspin(c, c->mondir);
          length = cellgfxdist(c, c->mondir);
          }
        
        shiftmatrix Vbn = Vb;
        if(cgi.emb->no_spin() && c->mondir != NODIR) {
          Vbn = Vparam * currentmap->adj(c, c->mondir);
          ld dummy;
          applyAnimation(c->move(c->mondir), Vbn, dummy, LAYER_BIG);
          }

        if(c->monmirror) Vb = Vb * lmirror();
  
        if(mapeditor::drawUserShape(Vb, mapeditor::sgMonster, c->monst, (col << 8) + 0xFF, c)) 
          return false;

        if(isIvy(c) || isMutantIvy(c) || c->monst == moFriendlyIvy) {
          queuepoly(Vb, cgi.shIBranch, (col << 8) + 0xFF);
          christmas_lights(c, Vb);
          }

        else if(c->monst == moDragonHead || c->monst == moDragonTail) {
          char part = dragon::bodypart(c, dragon::findhead(c));
          if(part != '2') {
            queuepoly(at_smart_lof(Vb, cgi.ABODY), cgi.shDragonSegment, darkena(col, 0, 0xFF));
            ShadowV(Vb, cgi.shDragonSegment, PPR::GIANTSHADOW);
            }
          }
        else {
          if(c->monst == moTentacleGhost) {
            hyperpoint V0 = history::on ? unshift(tC0(Vs)) : inverse_shift(cwtV, tC0(Vs));
            hyperpoint V1 = lspintox(V0) * V0;
            Vs = cwtV * lrspintox(V0) * rpushxto0(V1) * lpispin();
            drawMonsterType(moGhost, c, Vs, col, footphase, asciicol);
            col = minf[moTentacletail].color;
            }
          /*
          queuepoly(at_smart_lof(Vb, cgi.ABODY), cgi.shTentacleX, 0xFFFFFFFF);
          queuepoly(at_smart_lof(Vb, cgi.ABODY), cgi.shTentacle, (col << 8) + 0xFF);
          ShadowV(Vb, cgi.shTentacleX, PPR::GIANTSHADOW);
          */
          bool hexsnake = c->monst == moHexSnake || c->monst == moHexSnakeTail;
          bool thead = c->monst == moTentacle || c->monst == moTentaclewait || c->monst == moTentacleEscaping;
          hpcshape& sh = hexsnake ? cgi.shWormSegment : cgi.shSmallWormSegment;
          ld wav = hexsnake ? 0 : 
            c->monst < moTentacle ? 1/1.5 :
            1;
          color_t col0 = col;
          if(c->monst == moWorm || c->monst == moWormwait)
            col0 = minf[moWormtail].color;
          else if(thead)
            col0 = minf[moTentacletail].color;
          add_segment(taildist(c), [=] () {
            for(int i=11; i>=0; i--) {
              if(i < 3 && (c->monst == moTentacle || c->monst == moTentaclewait)) continue;
              if(doHighlight())
                poly_outline = outline;
              shiftmatrix Vbx = Vb;
              if(cgi.emb->no_spin()) {
                hyperpoint h = inverse_shift(Vb, Vbn * tile_center());
                h = cgi.emb->actual_to_intermediate(h);
                h *= i / 12.;
                Vbx = Vb * cgi.emb->intermediate_to_actual_translation(h);
                }
              else {
                if(WDIM == 2) Vbx = Vbx * spin(sin(TAU * i / 12) * wav / (i+.1));
                Vbx = Vbx * lxpush(length * (i) / 12.0);
                }
              // shiftmatrix Vbx2 = Vnext * xpush(length2 * i / 6.0);
              // Vbx = Vbx * rspintox(inverse(Vbx) * Vbx2 * C0) * lpispin();
              ShadowV(Vbx, sh, PPR::GIANTSHADOW);
              queuepoly(at_smart_lof(Vbx, cgi.ABODY), sh, (col0 << 8) + 0xFF);
              }
            });
          }
        }
        
      else {
        shiftmatrix T = Vparam * ddspin(c, c->mondir);
        color_t col = darkena(0x606020, 0, 0xFF);
        for(int u=-1; u<=1; u++)
          queueline(T*xspinpush0(90._deg, u*cgi.crossf/5), T*xspinpush(0, cgi.crossf)*xspinpush0(90._deg, u*cgi.crossf/5), col, 2 + vid.linequality);
        }
      }

    if(mmmon) {
      if(isAnyIvy(c)) {
        if(mhybrid) {
          queuepoly(Vb, cgi.shILeaf[ctof(c)], darkena(col, 0, 0xFF));
          for(int a=0; a<c->type-2; a++)
            queuepoly(Vb * spin(a * TAU / (c->type-2)), cgi.shILeaf[2], darkena(col, 0, 0xFF));
          }
        else if(GDIM == 3) {
          queuepoly(face_the_player(at_smart_lof(Vb, cgi.ABODY)), cgi.shILeaf[1], darkena(col, 0, 0xFF));
          }
        else {
          if(c->monmirror) Vb = Vb * lmirror();
          queuepoly(at_smart_lof(Vb, cgi.ABODY), cgi.shILeaf[ctof(c)], darkena(col, 0, 0xFF));
          ShadowV(Vb, cgi.shILeaf[ctof(c)], PPR::GIANTSHADOW);
          }
        }
      else if(m == moWorm || m == moWormwait || m == moHexSnake) {
        Vb = Vb * lpispin();
        if(c->monmirror) Vb = Vb * lmirror();
        shiftmatrix Vbh = at_smart_lof(Vb, cgi.AHEAD);
        queuepoly(Vbh, cgi.shWormHead, darkena(col, 0, 0xFF));
        queuepolyat(Vbh, cgi.shWormEyes, 0xFF, PPR::ONTENTACLE_EYES);
        ShadowV(Vb, cgi.shWormHead, PPR::GIANTSHADOW);
        }
      else if(m == moDragonHead) {
        if(c->monmirror) Vb = Vb * lmirror();
        shiftmatrix Vbh = at_smart_lof(Vb, cgi.AHEAD);
        ShadowV(Vb, cgi.shDragonHead, PPR::GIANTSHADOW);
        queuepoly(Vbh, cgi.shDragonHead, darkena(col, c->hitpoints?0:1, 0xFF));
        queuepolyat(Vbh/* * lpispin() */, cgi.shDragonEyes, 0xFF, PPR::ONTENTACLE_EYES);
        
        int noscolor = (c->hitpoints == 1 && c->stuntime ==1) ? 0xFF0000FF : 0xFF;
        queuepoly(Vbh, cgi.shDragonNostril, noscolor);
        queuepoly(Vbh * lmirror(), cgi.shDragonNostril, noscolor);
        }
      else if(m == moTentacle || m == moTentaclewait || m == moTentacleEscaping) {
        Vb = Vb * lpispin();
        if(c->monmirror) Vb = Vb * lmirror();
        shiftmatrix Vbh = at_smart_lof(Vb, cgi.AHEAD);
        queuepoly(Vbh, cgi.shTentHead, darkena(col, 0, 0xFF));
        ShadowV(Vb, cgi.shTentHead, PPR::GIANTSHADOW);
        }
      else if(m == moDragonTail) {
        cell *c2 = NULL;
        for(int i=0; i<c->type; i++)
          if(c->move(i) && isDragon(c->move(i)->monst) && c->move(i)->mondir == c->c.spin(i))
            c2 = c->move(i);
        int nd = neighborId(c, c2);
        char part = dragon::bodypart(c, dragon::findhead(c));
        if(part == 't') {
          if(nospinb) {
            ld length;
            chainAnimation(c, c2, Vb, Vparam * currentmap->adj(c, nd), length);
            Vb = Vb * lpispin();
            }
          else {
            Vb = Vb0 * ddspin180(c, nd);
            }
          if(c->monmirror) Vb = Vb * lmirror();
          shiftmatrix Vbb = at_smart_lof(Vb, cgi.ABODY);
          queuepoly(Vbb, cgi.shDragonTail, darkena(col, c->hitpoints?0:1, 0xFF));
          ShadowV(Vb, cgi.shDragonTail, PPR::GIANTSHADOW);
          }
        else if(true) {
          if(nospinb) {
            ld length;
            chainAnimation(c, c2, Vb, Vparam * currentmap->adj(c, nd), length);
            Vb = Vb * lpispin();
            double ang = chainAngle(c, Vb, c->move(c->mondir), currentmap->spin_angle(c, c->mondir) - currentmap->spin_angle(c, nd), Vparam);
            ang /= 2;
            Vb = Vb * spin(M_PI-ang);
            }
          else {
            /* todo what if no spin_angle */
            ld hdir0 = currentmap->spin_angle(c, nd) + M_PI;
            ld hdir1 = currentmap->spin_angle(c, c->mondir);
            cyclefix(hdir1, hdir0);
            Vb = Vb0 * spin((hdir0 + hdir1)/2 + M_PI);
            }
          if(c->monmirror) Vb = Vb * lmirror();
          shiftmatrix Vbb = at_smart_lof(Vb, cgi.ABODY);
          if(part == 'l' || part == '2') {
            queuepoly(Vbb, cgi.shDragonLegs, darkena(col, c->hitpoints?0:1, 0xFF));
            }
          queuepoly(Vbb, cgi.shDragonWings, darkena(col, c->hitpoints?0:1, 0xFF));
          }
        }
      else {
        if(c->monst == moTentacletail && c->mondir == NODIR) {
          if(c->monmirror) Vb = Vb * lmirror();
          queuepoly(GDIM == 3 ? at_smart_lof(Vb, cgi.ABODY) : Vb, cgi.shWormSegment, darkena(col, 0, 0xFF));
          }
        else if(c->mondir == NODIR) {
          bool hexsnake = c->monst == moHexSnake || c->monst == moHexSnakeTail;
          cell *c2 = NULL;
          for(int i=0; i<c->type; i++)
            if(c->move(i) && isWorm(c->move(i)->monst) && c->move(i)->mondir == c->c.spin(i))
              c2 = c->move(i);
          int nd = neighborId(c, c2);
          if(nospinb) {
            ld length;
            chainAnimation(c, c2, Vb, Vparam * currentmap->adj(c, nd), length);
            Vb = Vb * lpispin();
            }
          else {
            Vb = Vb0 * ddspin180(c, nd);
            }
          if(c->monmirror) Vb = Vb * lmirror();
          shiftmatrix Vbb = at_smart_lof(Vb, cgi.ABODY) * lpispin();
          hpcshape& sh = hexsnake ? cgi.shWormTail : cgi.shSmallWormTail;
          queuepoly(Vbb, sh, darkena(col, 0, 0xFF));
          ShadowV(Vb, sh, PPR::GIANTSHADOW);
          }
        }
      }
    else res = res && drawMonsterType(c->monst, c, Vb, col, footphase, asciicol);
    }

  else if(isMimic(c)) {
    int xdir = 0, copies = 1;
    if(c->wall == waMirrorWall) {
      xdir = mirror::mirrordir(c);
      copies = 2;
      if(xdir == -1) copies = 6, xdir = 0;
      }
    for(auto& m: mirror::mirrors) if(c == m.second.at) 
    for(int d=0; d<copies; d++) {
      multi::cpid = m.first;
      auto cw = m.second;
      if(d&1) cw = cw.mirrorat(xdir);
      if(d>=2) cw += 2;
      if(d>=4) cw += 2;
      shiftmatrix Vs = Vparam;
      bool mirr = cw.mirrored;
      if(mirrored != mirr && half_elliptic) continue;
      shiftmatrix T = shiftless(Id);
      nospins = applyAnimation(cwt.at, T, footphase, LAYER_SMALL);
      if(nospins) 
        Vs = Vs * ddspin(c, cw.spin, 0) * iddspin(cwt.at, cwt.spin, 0) * unshift(T);
      else
        Vs = Vs * ddspin(c, cw.spin, 0);
      if(mirr) Vs = Vs * lmirror();
      if(inmirrorcount&1) mirr = !mirr;
      col = mirrorcolor(geometry == gElliptic ? det(Vs.T) < 0 : mirr);
      if(!mouseout() && !nospins && GDIM == 2) {
        shiftpoint P2 = Vs * inverse_shift(inmirrorcount ? ocwtV : cwtV, mouseh);
        queuestr(P2, 10*mapfontscale/100, "x", 0xFF00);
        }
      if(!nospins && flipplayer) Vs = Vs * lpispin();

      res = res && drawMonsterType(moMimic, c, Vs, col, footphase, asciicol);
      drawPlayerEffects(Vs, Vparam, c, c->monst);
      }
    }

  // illusions face randomly
  
  else if(c->monst == moIllusion) {
    multi::cpid = 0;
    if(c->monmirror) Vs = Vs * lmirror();
    drawMonsterType(c->monst, c, Vs, col, footphase, asciicol);
    drawPlayerEffects(Vs, Vparam, c, c->monst);
    }

  // wolves face the heat
  
  else if(c->monst == moWolf && c->cpdist > 1) {
    if(!nospins) {
      int d = 0;
      double bheat = -999;
      for(int i=0; i<c->type; i++) if(c->move(i) && HEAT(c->move(i)) > bheat) {
        bheat = HEAT(c->move(i));
        d = i;
        }
      Vs = Vs * ddspin(c, d, 0);
      }
    if(c->monmirror) Vs = Vs * lmirror();
    return drawMonsterTypeDH(m, c, Vs, col, darkhistory, footphase, asciicol);
    }

  else if(c->monst == moKrakenT) {
    if(c->hitpoints == 0) col = 0x404040;
    if(nospinb) {
      ld length;
      chainAnimation(c, c->move(c->mondir), Vb, Vparam * currentmap->adj(c, c->mondir), length);
      Vb = Vb * lpispin();
      Vb = Vb * xpush(cgi.tentacle_length - cellgfxdist(c, c->mondir));
      }
    else if(NONSTDVAR) {
      transmatrix T = currentmap->adj(c, c->mondir);
      Vb = Vb * T * lrspintox(tC0(iso_inverse(T))) * xpush(cgi.tentacle_length);
      }
    else {
      Vb = Vb * ddspin180(c, c->mondir);
      Vb = Vb * xpush(cgi.tentacle_length - cellgfxdist(c, c->mondir));
      }
    if(c->monmirror) Vb = Vb * lmirror();
      
      // if(ctof(c) && !masterless) Vb = Vb * xpush(hexhexdist - hcrossf);
      // return (!BITRUNCATED) ? tessf * gp::scale : (c->type == 6 && (i&1)) ? hexhexdist : cgi.crossf;
    res = res && drawMonsterTypeDH(m, c, Vb, col, darkhistory, footphase, asciicol);
    }

  // golems, knights, and hyperbugs don't face the player (mondir-controlled)
  // also whatever in the lineview mode, and whatever in the quotient geometry

  else if(isDie(c->monst)) {
    transmatrix U = inverse_shift(Vparam, Vs);
    U = rgpushxto0(tC0(U));
    die_target = Vparam;
    res = res && drawMonsterTypeDH(m, c, Vparam * U, col, darkhistory, footphase, asciicol);
    }
    
  else if((hasFacing(c) && c->mondir != NODIR) || history::on || quotient || dont_face_pc) {
    if(c->monst == moKrakenH) Vs = Vb, nospins = nospinb;
    if(!nospins && c->mondir < c->type) Vs = Vs * ddspin180(c, c->mondir);
    if(c->monst == moPair) Vs = Vs * xpush(-.12);
    if(c->monmirror) Vs = Vs * lmirror();
    if(isFriendly(c)) drawPlayerEffects(Vs, Vparam, c, c->monst);
    res = res && drawMonsterTypeDH(m, c, Vs, col, darkhistory, footphase, asciicol);
    }

  else {
    // other monsters face the player
    
    if(!nospins) {
      shiftmatrix& where = (c->monst == moMirrorSpirit && inmirrorcount) ? ocwtV : cwtV;
      if(cgi.emb->is_euc_in_product()) { }
      else if(WDIM == 2 || mproduct) {
        hyperpoint V0 = inverse_shift(Vs, where * tile_center());
        if(gproduct) {
          auto d = product_decompose(V0);
          V0 = d.second;
          }
          
        if(hypot_d(2, tC0(unshift(Vs))) > 1e-3) {
          Vs = Vs * lrspintox(V0);
          }
        }
      else if(!sl2) {
        hyperpoint V0 = inverse_shift(Vs, tC0(where));
        Vs = Vs * lrspintox(V0);
        // cwtV * rgpushxto0(inverse(cwtV) * tC0(Vs));
        }
      if(c->monst == moHunterChanging)  
        Vs = Vs * (mhybrid ? spin180() : cspin180(WDIM-2, WDIM-1));
      }
    if(c->monmirror) Vs = Vs * lmirror();
    
    if(c->monst == moShadow) 
      multi::cpid = c->hitpoints;
    
    res = res && drawMonsterTypeDH(m, c, Vs, col, darkhistory, footphase, asciicol);
    }
  
  for(int i=0; i<numplayers(); i++) if(c == playerpos(i) && !shmup::on && mapeditor::drawplayer && 
    !(hardcore && !canmove)) {
    bool mirr = multi::players > 1 ? multi::player[i].mirrored : cwt.mirrored;
    if(half_elliptic && mirr != mirrored) continue;
    if(!nospins) {
      Vs = playerV;
      if(multi::players > 1 ? multi::flipped[i] : flipplayer) Vs = Vs * lpispin();
      }
    else {
      if(mirr) Vs = Vs * lmirror();
      }
    multi::cpid = i;

    asciicol = getcs().uicolor >> 8;

    drawPlayerEffects(Vs, Vparam, c, moPlayer);
    if(inmirrorcount && !mouseout() && !nospins && GDIM == 2) {
      hyperpoint h = inverse_shift(ocwtV, mouseh);
      if(flipplayer) h = lpispin() * h;
      shiftpoint P2 = Vs * h;
      queuestr(P2, mapfontscale / 10, "x", 0xFF00);
      }
    
    if(hide_player()) {
      first_cell_to_draw = false;
      if(WDIM == 2 && GDIM == 3) {
        drawPlayer(moPlayer, c, Vs, col, footphase, true);
        res = true;
        }
      }
    
    else if(isWorm(c->monst)) {
      ld depth = geom3::factor_to_lev(wormhead(c) == c ? cgi.AHEAD : cgi.ABODY);
      footphase = 0;
      int q = isize(ptds);
      res = res | drawMonsterType(moPlayer, c, Vs, col, footphase, asciicol);
      pushdown(c, q, Vs, -depth, true, false);
      }
    
    else res = res | drawMonsterType(moPlayer, c, Vs, col, footphase, asciicol);
    }
#endif
  return res;
  }

EX color_t christmas_color(int d, bool simple) {
  array<color_t, 5> cols = { 0x10101, 0x10100, 0x000001, 0x00100, 0x10000 };
  color_t v = cols[gmod(d, 5)];
  v *= 0x3F;
  int mode;
  if(false) {
    mode = gmod(ticks / 20000, 2) ? 1 : 3;
    }
  else {
    int t = gmod(ticks, 11000);
    if(t > 10000) mode = 4;
    else mode = 1 + gmod(ticks / 11000, 3);
    }
  switch(mode) {
    case 0:
      break;
    case 1:
      if((ticks/100 - d) % 16) v = 0;
      break;
    case 2: {
      int z = gmod(d * 500 - ticks, 8000);
      if(z > 1500) v = 0;
      else if(z < 500) { v /= 0x3F; v *= 0x3F * z / 500; }
      else if(z > 1000) { v /= 0x3F; v *= 0x3F * (1500 - z) / 500; }
      break;
      }
    case 3:
      if((-ticks / 100 - d) % 16) v = 0;
      break;
    case 4:
      v /= 0x3F;
      v *= int(0x20 + 0x1F * (1 - cos(ticks / 500. * TAU)) / 2);
      break;
    }
  return v;
  }

EX std::unordered_map<cell*, array<int, 3>> shines, old_shines;

EX bool festive, festive_date;
EX bool festive_option = true;

EX void christmas_lights(cell *c, const shiftmatrix& Vb) {
  if(!festive) return;
  int lev = 0;
  if(isMutantIvy(c)) lev = (c->stuntime - 1) & 15;
  else {
    auto c1 = c;
    while(lev < 100 && c1->cpdist <= 7 && (isIvy(c1->monst) || c1->monst == moFriendlyIvy) && !among(c1->monst, moIvyRoot) && c1->mondir != NODIR) {
      c1 = c1->cmove(c1->mondir); lev++;
      }
    }
  auto cn = c->cmove(c->mondir);
  ld dist = hdist0(currentmap->adj(c, c->mondir) * C0);
  for(int a: {0, 1, 2}) {
    color_t col = christmas_color(3*lev-a, c->monst == moMutant);
    int bri = max(part(col, 0), max(part(col, 1), part(col, 2)));
    color_t fcol = 0x7E + (col << 9) + 0x1010101 * int(129 * bri / 0x3F);
    queuepoly(Vb * xpush(dist * (a*2+1) / 6), cgi.shChristmasLight, fcol);
    for(int p: {0,1,2}) {
      int val = part(col, p);
      if(!val) continue;
      if(a == 0) {
        shines[c][p] += 2 * val;
        forCellEx(c1, c) shines[c1][p] += val;
        }
      if(a == 2) {
        shines[cn][p] += 2 * val;
        forCellEx(c1, cn) shines[c1][p] += val;
        }
      if(a == 1) {
        shines[c][p] += val;
        shines[cn][p] += val;
        forCellEx(c1, c) shines[c1][p] += val / 2;
        forCellEx(c1, cn) shines[c1][p] += val / 2;
        }
      }
    }
  }

}
