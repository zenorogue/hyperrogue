// Hyperbolic Rogue -- main graphics file

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// basic graphics:

namespace hr {

int inmirrorcount = 0;

bool spatial_graphics;
bool wmspatial, wmescher, wmplain, wmblack, wmascii;
bool mmspatial, mmhigh, mmmon, mmitem;

int detaillevel = 0;

hookset<bool(int sym, int uni)> *hooks_handleKey;
hookset<bool(cell *c, const transmatrix& V)> *hooks_drawcell;
purehookset hooks_frame, hooks_markers;

ld animation_factor = 1;
int animation_lcm = 0;

ld ptick(int period, ld phase = 0) {
  if(animation_lcm) animation_lcm = animation_lcm * (period / gcd(animation_lcm, period));
  return (ticks * animation_factor) / period + phase * 2 * M_PI;
  }

ld fractick(int period, ld phase = 0) {
  ld t = ptick(period, phase) / 2 / M_PI;
  t -= floor(t);
  if(t<0) t++;
  return t;
  }
  
ld sintick(int period, ld phase) {
  return sin(ptick(period, phase));
  }

transmatrix spintick(int period, ld phase = 0) {
  return spin(ptick(period, phase));
  }

#define WOLNIEJ 1
#define BTOFF 0x404040
#define BTON  0xC0C000

// #define PANDORA

int colorbar;

bool inHighQual; // taking high quality screenshot
bool auraNOGL;    // aura without GL

// 
int axestate;

int ticks;
int frameid;

bool camelotcheat;
bool nomap;

eItem orbToTarget;
eMonster monsterToSummon;

int sightrange_bonus = 0;

string mouseovers;

int darken = 0;

struct fallanim {
  int t_mon, t_floor, pid;
  eWall walltype;
  eMonster m;
  fallanim() { t_floor = 0; t_mon = 0; pid = 0; walltype = waNone; }
  };

map<cell*, fallanim> fallanims;

bool doHighlight() {
  return (hiliteclick && darken < 2) ? !mmhigh : mmhigh;
  }

eModel pmodel = mdDisk;

int dlit;

ld spina(cell *c, int dir) {
  return 2 * M_PI * dir / c->type;
  }

// cloak color 
int cloakcolor(int rtr) {
  rtr -= 28;
  rtr /= 2;
  rtr %= 10;
  if(rtr < 0) rtr += 10;
  // rtr = time(NULL) % 10;
  int cc[10] = {
    0x8080FF, 0x80FFFF, 0x80FF80, 0xFF8080, 0xFF80FF, 0xFFFF80, 
    0xFFFFC0, 0xFFD500, 0x421C52, 0
    };
  return cc[rtr];
  }

int firegradient(double p) {
  return gradient(0xFFFF00, 0xFF0000, 0, p, 1);
  }
  
int firecolor(int phase, int mul) {
  return gradient(0xFFFF00, 0xFF0000, -1, sintick(100*mul, phase/200./M_PI), 1);
  }

int watercolor(int phase) {
  return 0x0080C0FF + 256 * int(63 * sintick(50, phase/100./M_PI));
  }

int aircolor(int phase) {
  return 0x8080FF00 | int(32 + 32 * sintick(200, phase * 1. / S21));
  }

int fghostcolor(cell *c) {
  int phase = int(fractick(650, (int)(size_t)c) * 4000);
  if(phase < 1000)      return gradient(0xFFFF80, 0xA0C0FF,    0, phase, 1000);
  else if(phase < 2000) return gradient(0xA0C0FF, 0xFF80FF, 1000, phase, 2000);
  else if(phase < 3000) return gradient(0xFF80FF, 0xFF8080, 2000, phase, 3000);
  else if(phase < 4000) return gradient(0xFF8080, 0xFFFF80, 3000, phase, 4000);
  return 0xFFD500;
  }

int weakfirecolor(int phase) {
  return gradient(0xFF8000, 0xFF0000, -1, sintick(500, phase/1000./M_PI), 1);
  }

color_t fc(int ph, color_t col, int z) {
  if(items[itOrbFire]) col = darkena(firecolor(ph), 0, 0xFF);
  if(items[itOrbAether]) col = (col &~0XFF) | (col&0xFF) / 2;
  for(int i=0; i<numplayers(); i++) if(multi::playerActive(i))
    if(items[itOrbFish] && isWatery(playerpos(i)) && z != 3) return watercolor(ph);
  if(invismove) 
    col = 
      shmup::on ?
        (col &~0XFF) | (int((col&0xFF) * .25))
      : (col &~0XFF) | (int((col&0xFF) * (100+100*sintick(500)))/200);
  return col;
  }

int lightat, safetyat;
void drawLightning() { lightat = ticks; }
void drawSafety() { safetyat = ticks; }

void drawShield(const transmatrix& V, eItem it) {
#if CAP_CURVE
  float ds = ptick(300);
  color_t col = iinf[it].color;
  if(it == itOrbShield && items[itOrbTime] && !orbused[it])
    col = (col & 0xFEFEFE) / 2;
  if(sphere && cwt.at->land == laHalloween && !wmblack && !wmascii)
    col = 0;
  double d = it == itOrbShield ? hexf : hexf - .1;
  int mt = sphere ? 7 : 5;
  for(ld a=0; a<=S84*mt+1e-6; a+=pow(.5, vid.linequality))
    curvepoint(V*xspinpush0(a * M_PI/S42, d + sin(ds + M_PI*2*a/4/mt)*.1));
  queuecurve(darkena(col, 0, 0xFF), 0x8080808, PPR::LINE);
#endif
  }

void drawSpeed(const transmatrix& V) {
#if CAP_CURVE
  ld ds = ptick(10);
  color_t col = darkena(iinf[itOrbSpeed].color, 0, 0xFF);
  for(int b=0; b<S84; b+=S14) {
    PRING(a)
      curvepoint(V*xspinpush0((ds+b+a) * M_PI/S42, hexf*a/S84));
    queuecurve(col, 0x8080808, PPR::LINE);
    }
#endif
  }

int ctof(cell *c) {
  #if CAP_IRR
  if(IRREGULAR) return irr::ctof(c);
  #endif
  if(PURE) return 1;
  // if(euclid) return 0;
  if(!c) return 1;
  if(binarytiling) return c->type == 7;
  return ishept(c) ? 1 : 0;
  // c->type == 6 ? 0 : 1;
  }

int ctof012(cell *c) {
  return ishept(c)?1:ishex1(c)?0:2;
  }

void drawSafety(const transmatrix& V, int ct) {
#if CAP_QUEUE
  ld ds = ptick(50);
  color_t col = darkena(iinf[itOrbSafety].color, 0, 0xFF);
  for(int a=0; a<ct; a++)
    queueline(V*xspinpush0((ds+a*S84/ct) * M_PI/S42, 2*hexf), V*xspinpush0((ds+(a+(ct-1)/2)*S84/ct) * M_PI / S42, 2*hexf), col, vid.linequality);
#endif
  }

void drawFlash(const transmatrix& V) {
#if CAP_CURVE
  float ds = ptick(300);
  color_t col = darkena(iinf[itOrbFlash].color, 0, 0xFF);
  col &= ~1;
  for(int u=0; u<5; u++) {
    ld rad = hexf * (2.5 + .5 * sin(ds+u*.3));
    PRING(a) curvepoint(V*xspinpush0(a * M_PI / S42, rad));
    queuecurve(col, 0x8080808, PPR::LINE);
    }
#endif
  }

void drawLove(const transmatrix& V, int hdir) {
#if CAP_CURVE
  float ds = ptick(300);
  color_t col = darkena(iinf[itOrbLove].color, 0, 0xFF);
  col &= ~1;
  for(int u=0; u<5; u++) {
    PRING(a) {
      double d = (1 + cos(a * M_PI/S42)) / 2;
      double z = a; if(z>S42) z = S84-z;
      if(z <= 10) d += (10-z) * (10-z) * (10-z) / 3000.;

      ld rad = hexf * (2.5 + .5 * sin(ds+u*.3)) * d;
      curvepoint(V*xspinpush0((S42+hdir+a-1) * M_PI/S42, rad));
      }
    queuecurve(col, 0x8080808, PPR::LINE);
    }
#endif
  }

void drawWinter(const transmatrix& V, ld hdir) {
#if CAP_QUEUE
  float ds = ptick(300);
  color_t col = darkena(iinf[itOrbWinter].color, 0, 0xFF);
  for(int u=0; u<20; u++) {
    ld rad = sin(ds+u * 2 * M_PI / 20) * M_PI / S7;
    queueline(V*xspinpush0(M_PI+hdir+rad, hexf*.5), V*xspinpush0(M_PI+hdir+rad, hexf*3), col, 2 + vid.linequality);
    }
#endif
  }

void drawLightning(const transmatrix& V) {
#if CAP_QUEUE
  color_t col = darkena(iinf[itOrbLightning].color, 0, 0xFF);
  for(int u=0; u<20; u++) {
    ld leng = 0.5 / (0.1 + (rand() % 100) / 100.0);
    ld rad = rand() % 1000;
    queueline(V*xspinpush0(rad, hexf*0.3), V*xspinpush0(rad, hexf*leng), col, 2 + vid.linequality);
    }
#endif
  }

ld displayspin(cell *c, int d) {
  if(0);
  #if CAP_ARCM
  else if(archimedean) {
    if(PURE) {
      auto& t1 = arcm::current.get_triangle(c->master, d-1);
      return -(t1.first + M_PI / c->type);
      }
    else if(DUAL) {
      auto& t1 = arcm::current.get_triangle(c->master, 2*d);
      return -t1.first;
      }
    else { /* BITRUNCATED */
      auto& t1 = arcm::current.get_triangle(c->master, d);
      return -t1.first;
      }
    }
  #endif
  #if CAP_IRR
  else if(IRREGULAR) {
    auto id = irr::cellindex[c];
    auto& vs = irr::cells[id];
    if(d < 0 || d >= c->type) return 0;
    auto& p = vs.jpoints[vs.neid[d]];
    return -atan2(p[1], p[0]);
    }
  #endif
  #if CAP_BT
  else if(binarytiling) {
    if(d == NODIR) return 0;
    if(d == c->type-1) d++;
    return -(d+2)*M_PI/4;
    }
  #endif
  else if(masterless)
    return - d * 2 * M_PI / c->type;
  else
    return M_PI - d * 2 * M_PI / c->type;
  }

double hexshiftat(cell *c) {
  if(binarytiling) return 0;
  if(ctof(c) && S7==6 && S3 == 4 && BITRUNCATED) return hexshift + 2*M_PI/S7;
  if(ctof(c) && (S7==8 || S7 == 4) && S3 == 3 && BITRUNCATED) return hexshift + 2*M_PI/S7;
  if(hexshift && ctof(c)) return hexshift;
  return 0;
  }

transmatrix ddspin(cell *c, int d, ld bonus) {
  return spin(displayspin(c, d) + bonus - hexshiftat(c));
  }

transmatrix iddspin(cell *c, int d, ld bonus) {
  return spin(hexshiftat(c) - displayspin(c, d) + bonus);
  }

void drawPlayerEffects(const transmatrix& V, cell *c, bool onplayer) {
  if(!onplayer && !items[itOrbEmpathy]) return;
  if(items[itOrbShield] > (shmup::on ? 0 : ORBBASE)) drawShield(V, itOrbShield);
  if(items[itOrbShell] > (shmup::on ? 0 : ORBBASE)) drawShield(V, itOrbShell);

  if(items[itOrbSpeed]) drawSpeed(V); 

  if(onplayer && (items[itOrbSword] || items[itOrbSword2])) {
    using namespace sword;
  
    if(shmup::on) {
#if CAP_SHAPES
      if(items[itOrbSword])
        queuepoly(V*spin(shmup::pc[multi::cpid]->swordangle), (peace::on ? shMagicShovel : shMagicSword), darkena(iinf[itOrbSword].color, 0, 0xC0 + 0x30 * sintick(200)));
  
      if(items[itOrbSword2])
        queuepoly(V*spin(shmup::pc[multi::cpid]->swordangle+M_PI), (peace::on ? shMagicShovel : shMagicSword), darkena(iinf[itOrbSword2].color, 0, 0xC0 + 0x30 * sintick(200)));
#endif
      }                  
    
    else {
      int& ang = angle[multi::cpid];
      ang %= S42;

#if CAP_QUEUE || CAP_SHAPES
      transmatrix Vnow = gmatrix[c] * rgpushxto0(inverse(gmatrix[c]) * tC0(V)) * ddspin(c,0,M_PI); // (IRREGULAR ? ddspin(c,0,M_PI) : spin(-hexshiftat(c)));
#endif
      
#if CAP_QUEUE
      if(!euclid) for(int a=0; a<S42; a++) {
        int dda = S42 + (-1-2*a);
        if(a == ang && items[itOrbSword]) continue;
        if(PURE && a%3 != ang%3) continue;
        if((a+S21)%S42 == ang && items[itOrbSword2]) continue;
        bool longer = sword::pos(cwt.at, a-1) != sword::pos(cwt.at, a+1);
        color_t col = darkena(0xC0C0C0, 0, 0xFF);
        queueline(Vnow*xspinpush0(dda * M_PI / S42, PURE ? 0.6 * scalefactor : longer ? 0.36 : 0.4), Vnow*xspinpush0(dda * M_PI/S42, PURE ? 0.7 * scalefactor : longer ? 0.44 : 0.42), col, 1);
        }
#endif

#if CAP_SHAPES
      if(items[itOrbSword])
        queuepoly(Vnow*spin(M_PI+(-1-2*ang)*2*M_PI/S84), (peace::on ? shMagicShovel : shMagicSword), darkena(iinf[itOrbSword].color, 0, 0x80 + 0x70 * sintick(200)));
  
      if(items[itOrbSword2])
        queuepoly(Vnow*spin((-1-2*ang)*2*M_PI/S84), (peace::on ? shMagicShovel : shMagicSword), darkena(iinf[itOrbSword2].color, 0, 0x80 + 0x70 * sintick(200)));
#endif
      }
    }

  if(onplayer && items[itOrbSafety]) drawSafety(V, c->type);

  if(onplayer && items[itOrbFlash]) drawFlash(V); 
  if(onplayer && items[itOrbLove]) drawLove(V, 0); // displaydir(c, cwt.spin)); 

  if(items[itOrbWinter]) 
    drawWinter(V, 0); // displaydir(c, cwt.spin));
  
  if(onplayer && items[itOrbLightning]) drawLightning(V);
  
  if(safetyat > 0) {
    int tim = ticks - safetyat;
    if(tim > 2500) safetyat = 0;
    for(int u=tim; u<=2500; u++) {
      if((u-tim)%250) continue;
      ld rad = hexf * u / 250;
      color_t col = darkena(iinf[itOrbSafety].color, 0, 0xFF);
      PRING(a)
        curvepoint(V*xspinpush0(a * M_PI / S42, rad));
      queuecurve(col, 0, PPR::LINE);
      }
    }
  }

void drawStunStars(const transmatrix& V, int t) {
#if CAP_SHAPES
  for(int i=0; i<3*t; i++) {
    transmatrix V2 = V * spin(M_PI * 2 * i / (3*t) + ptick(200));
    queuepolyat(V2, shFlailBall, 0xFFFFFFFF, PPR::STUNSTARS);
    }
#endif
  }

namespace tortoise {

  // small is 0 or 2
  void draw(const transmatrix& V, int bits, int small, int stuntime) {

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
    
    for(int i=0; i<12; i++) {
      color_t col = 
        i == 0 ? shellcolor:
        i <  8 ? scutecolor :
        skincolor;
      int b = getBit(bits, i);
      int d = darkena(col, 0, 0xFF);
      if(i >= 1 && i <= 7) if(b) { d = darkena(col, 1, 0xFF); b = 0; }
      
      if(i >= 8 && i <= 11 && stuntime >= 3) continue;

      queuepoly(V, shTortoise[i][b+small], d);
      if((i >= 5 && i <= 7) || (i >= 9 && i <= 10))
        queuepoly(V * Mirror, shTortoise[i][b+small], d);
      
      if(i == 8) {
        for(int k=0; k<stuntime; k++) {
          eyecolor &= 0xFEFEFE; 
          eyecolor >>= 1;
          }
        queuepoly(V, shTortoise[12][b+small], darkena(eyecolor, 0, 0xFF));
        queuepoly(V * Mirror, shTortoise[12][b+small], darkena(eyecolor, 0, 0xFF));
        }
      }
#endif
    }

  int getMatchColor(int bits) {
    int mcol = 1;
    double d = tortoise::getScent(bits);
    
    if(d > 0) mcol = 0xFFFFFF;
    else if(d < 0) mcol = 0;
    
      int dd = 0xFF * (atan(fabs(d)/2) / (M_PI/2));
      
    return gradient(0x487830, mcol, 0, dd, 0xFF);
    }
  };

double footfun(double d) {
  d -= floor(d);
  return
    d < .25 ? d :
    d < .75 ? .5-d :
    d-1;
  }

bool ivoryz;
                                 
void animallegs(const transmatrix& V, eMonster mo, color_t col, double footphase) {
#if CAP_SHAPES
  footphase /= SCALE;
  
  bool dog = mo == moRunDog;
  bool bug = mo == moBug0 || mo == moMetalBeast;

  if(bug) footphase *= 2.5;
  
  double rightfoot = footfun(footphase / .4 / 2) / 4 * 2;
  double leftfoot = footfun(footphase / .4 / 2 - (bug ? .5 : dog ? .1 : .25)) / 4 * 2;
  
  if(bug) rightfoot /= 2.5, leftfoot /= 2.5;
  
  if(!footphase) rightfoot = leftfoot = 0;

  transmatrix VAML = mmscale(V, 1.04);
  
  hpcshape* sh[6][4] = {
    {&shDogFrontPaw, &shDogRearPaw, &shDogFrontLeg, &shDogRearLeg},
    {&shWolfFrontPaw, &shWolfRearPaw, &shWolfFrontLeg, &shWolfRearLeg},
    {&shReptileFrontFoot, &shReptileRearFoot, &shReptileFrontLeg, &shReptileRearLeg},
    {&shBugLeg, NULL, NULL, NULL},
    {&shTrylobiteFrontClaw, &shTrylobiteRearClaw, &shTrylobiteFrontLeg, &shTrylobiteRearLeg},
    {&shBullFrontHoof, &shBullRearHoof, &shBullFrontHoof, &shBullRearHoof},
    };

  hpcshape **x = sh[mo == moRagingBull ? 5 : mo == moBug0 ? 3 : mo == moMetalBeast ? 4 : mo == moRunDog ? 0 : mo == moReptile ? 2 : 1];

  if(x[0]) queuepolyat(V * xpush(rightfoot), *x[0], col, PPR::MONSTER_FOOT);
  if(x[0]) queuepolyat(V * Mirror * xpush(leftfoot), *x[0], col, PPR::MONSTER_FOOT);
  if(x[1]) queuepolyat(V * xpush(-rightfoot), *x[1], col, PPR::MONSTER_FOOT);
  if(x[1]) queuepolyat(V * Mirror * xpush(-leftfoot), *x[1], col, PPR::MONSTER_FOOT);


  if(x[2]) queuepolyat(VAML * xpush(rightfoot/2), *x[2], col, PPR::MONSTER_FOOT);
  if(x[2]) queuepolyat(VAML * Mirror * xpush(leftfoot/2), *x[2], col, PPR::MONSTER_FOOT);
  if(x[3]) queuepolyat(VAML * xpush(-rightfoot/2), *x[3], col, PPR::MONSTER_FOOT);
  if(x[3]) queuepolyat(VAML * Mirror * xpush(-leftfoot/2), *x[3], col, PPR::MONSTER_FOOT);
#endif
  }

bool noshadow;

#if CAP_SHAPES
void ShadowV(const transmatrix& V, const hpcshape& bp, PPR prio) {
  if(mmspatial) { 
    if(model_needs_depth() || noshadow) 
      return; // shadows break the depth testing
    dynamicval<color_t> p(poly_outline, OUTLINE_TRANS);
    queuepolyat(V, bp, SHADOW_MON, prio); 
    }
  }
#endif


#if CAP_SHAPES
transmatrix otherbodyparts(const transmatrix& V, color_t col, eMonster who, double footphase) {

#define VFOOT V
#define VLEG mmscale(V, geom3::LEG)
#define VGROIN mmscale(V, geom3::GROIN)
#define VBODY mmscale(V, geom3::BODY)
#define VNECK mmscale(V, geom3::NECK)
#define VHEAD mmscale(V, geom3::HEAD)

#define VALEGS V
#define VABODY mmscale(V, geom3::ABODY)
#define VAHEAD mmscale(V, geom3::AHEAD)

#define VFISH V
#define VBIRD  ((where && bird_disruption(where)) ? V : mmscale(V, geom3::BIRD + .05 * sintick(1000, (int) (size_t(where))/1000.)))
#define VGHOST  mmscale(V, geom3::GHOST)

#define VSLIMEEYE  mscale(V, geom3::FLATEYE)

  // if(!mmspatial && !footphase && who != moSkeleton) return;
  
  footphase /= SCALE;
  double rightfoot = footfun(footphase / .4 / 2.5) / 4 * 2.5;
  
  const double wobble = -1;

  // todo

  if(detaillevel >= 2) { 
    transmatrix VL = mmscale(V, geom3::LEG1);
    queuepoly(VL * xpush(rightfoot*3/4), shHumanLeg, col);
    queuepoly(VL * Mirror * xpush(-rightfoot*3/4), shHumanLeg, col);
    }

  if(true) {
    transmatrix VL = mmscale(V, geom3::LEG);
    queuepoly(VL * xpush(rightfoot/2), shHumanLeg, col);
    queuepoly(VL * Mirror * xpush(-rightfoot/2), shHumanLeg, col);
    }

  if(detaillevel >= 2) { 
    transmatrix VL = mmscale(V, geom3::LEG3);
    queuepoly(VL * xpush(rightfoot/4), shHumanLeg, col);
    queuepoly(VL * Mirror * xpush(-rightfoot/4), shHumanLeg, col);
    }

  if(who == moWaterElemental) {
    double fishtail = footfun(footphase / .4) / 4 * 1.5;
    queuepoly(VFOOT * xpush(fishtail), shFishTail, watercolor(100));
    }
  else if(who == moSkeleton) {
    queuepoly(VFOOT * xpush(rightfoot), shSkeletalFoot, col);
    queuepoly(VFOOT * Mirror * xpush(-rightfoot), shSkeletalFoot, col);
    return spin(rightfoot * wobble);
    }
  else if(isTroll(who) || who == moMonkey || who == moYeti || who == moRatling || who == moRatlingAvenger || who == moGoblin) {
    queuepoly(VFOOT * xpush(rightfoot), shYetiFoot, col);
    queuepoly(VFOOT * Mirror * xpush(-rightfoot), shYetiFoot, col);
    }
  else {
    queuepoly(VFOOT * xpush(rightfoot), shHumanFoot, col);
    queuepoly(VFOOT * Mirror * xpush(-rightfoot), shHumanFoot, col);
    }

  if(!mmspatial) return spin(rightfoot * wobble);

  if(detaillevel >= 2 && who != moZombie)
    queuepoly(mmscale(V, geom3::NECK1), shHumanNeck, col);
  if(detaillevel >= 1) {
    queuepoly(VGROIN, shHumanGroin, col);
    if(who != moZombie) queuepoly(VNECK, shHumanNeck, col);
    }
  if(detaillevel >= 2) {
    queuepoly(mmscale(V, geom3::GROIN1), shHumanGroin, col);
    if(who != moZombie) queuepoly(mmscale(V, geom3::NECK3), shHumanNeck, col);
    }
  
  return spin(rightfoot * wobble);
  
  return Id;
  }
#endif

bool drawstar(cell *c) {
  for(int t=0; t<c->type; t++) 
    if(c->move(t) && c->move(t)->wall != waSulphur && c->move(t)->wall != waSulphurC &&
     c->move(t)->wall != waBarrier)
       return false;
  return true;
  }

bool drawing_usershape_on(cell *c, mapeditor::eShapegroup sg) {
#if CAP_EDIT
  return c && c == mapeditor::drawcell && mapeditor::drawcellShapeGroup() == sg;
#else
  return false;
#endif
  }

bool drawItemType(eItem it, cell *c, const transmatrix& V, int icol, int pticks, bool hidden) {
#if !CAP_SHAPES
  return it;
#else
  char xch = iinf[it].glyph;
  auto sinptick = [c, pticks] (int period) { return c ? sintick(period) : sin(animation_factor * pticks / period);};
  auto spinptick = [c, pticks] (int period, ld phase=0) { return c ? spintick(period, phase) : spin((animation_factor * pticks + phase) / period); };
  int ct6 = c ? ctof(c) : 1;
  hpcshape *xsh = 
    (it == itPirate || it == itKraken) ? &shPirateX :
    (it == itBuggy || it == itBuggy2) ? &shPirateX :
    it == itHolyGrail ? &shGrail :
    isElementalShard(it) ? &shElementalShard :
    (it == itBombEgg || it == itTrollEgg) ? &shEgg :
    it == itHunting ? &shTriangle :
    it == itDodeca ? &shDodeca :
    xch == '*' ? &shGem[ct6] : 
    xch == '(' ? &shKnife : 
    it == itShard ? &shMFloor.b[0] :
    it == itTreat ? &shTreat :
    it == itSlime ? &shEgg :
    xch == '%' ? &shDaisy : xch == '$' ? &shStar : xch == ';' ? &shTriangle :
    xch == '!' ? &shTriangle : it == itBone ? &shNecro : it == itStatue ? &shStatue :
    it == itIvory ? &shFigurine : 
    xch == '?' ? &shBookCover : 
    it == itKey ? &shKey : 
    it == itRevolver ? &shGun :
    NULL;
   
  if(c && doHighlight()) {
    int k = itemclass(it);
    if(k == IC_TREASURE)
      poly_outline = OUTLINE_TREASURE;
    else if(k == IC_ORB)
      poly_outline = OUTLINE_ORB;
    else
      poly_outline = OUTLINE_OTHER;
    }
  
  transmatrix Vit = V;
  if(DIM == 3 && c) Vit = V * cspin(0, 2, ptick(618, 0));

  if(c && conformal::includeHistory && conformal::infindhistory.count(c)) poly_outline = OUTLINE_DEAD;

  if(!mmitem && it) return true;
  
  else if(it == itSavedPrincess) {
    drawMonsterType(moPrincess, c, V, icol, 0);
    return false;
    }
  
  else if(it == itStrongWind) {
    queuepoly(Vit * spinptick(750), shFan, darkena(icol, 0, 255));
    }

  else if(it == itWarning) {
    queuepoly(Vit * spinptick(750), shTriangle, darkena(icol, 0, 255));
    }
    
  else if(it == itBabyTortoise) {
    int bits = c ? tortoise::babymap[c] : tortoise::last;
    int over = c && c->monst == moTortoise;
    tortoise::draw(Vit * spinptick(5000) * ypush(crossf*.15), bits, over ? 4 : 2, 0);
    // queuepoly(Vit, shHeptaMarker, darkena(tortoise::getMatchColor(bits), 0, 0xC0));
    }
  
  else if(it == itCompass) {
    transmatrix V2;
    #if CAP_CRYSTAL
    if(geometry == gCrystal) {
      if(crystal::compass_probability <= 0) return true;
      if(cwt.at->land == laCamelot && celldistAltRelative(cwt.at) < 0) crystal::used_compass_inside = true;
      V2 = V * spin(crystal::compass_angle() + M_PI);
      }
    else
    #endif
    if(1) {
      cell *c1 = c ? findcompass(c) : NULL;
      if(c1) {
        transmatrix P = ggmatrix(c1);
        hyperpoint P1 = tC0(P);
        
        if(isPlayerOn(c)) {
          queuechr(P1, 2*vid.fsize, 'X', 0x10100 * int(128 + 100 * sintick(150)));
    //      queuestr(V, 1, its(compassDist(c)), 0x10101 * int(128 - 100 * sin(ticks / 150.)), 1);
          queuestr(P1, vid.fsize, its(-compassDist(c)), 0x10101 * int(128 - 100 * sintick(150)));
          addauraspecial(P1, 0xFF0000, 0);
          }
        
        V2 = V * rspintox(inverse(V) * P1);
        }
      else V2 = V;
      }
    if(c) V2 = V2 * spin(M_PI * sintick(100) / 30);
    queuepoly(V2, shCompass1, 0xFF8080FF);
    queuepoly(V2, shCompass2, 0xFFFFFFFF);
    queuepoly(V2, shCompass3, 0xFF0000FF);
    queuepoly(V2 * pispin, shCompass3, 0x000000FF);
    xsh = NULL;
    }

  else if(it == itPalace) {
    transmatrix V2 = Vit * spin(ticks / 1500.);
    draw_floorshape(c, V2, shMFloor3, 0xFFD500FF);
    draw_floorshape(c, V2, shMFloor4, darkena(icol, 0, 0xFF));
    queuepoly(V2, shGem[ct6], 0xFFD500FF);
    xsh = NULL;
    }
  
  else if(mapeditor::drawUserShape(Vit, mapeditor::sgItem, it, darkena(icol, 0, 0xFF), c)) ;
  
  else if(it == itRose) {
    for(int u=0; u<4; u++)
      queuepoly(Vit * spinptick(1500) * spin(2*M_PI / 3 / 4 * u), shRose, darkena(icol, 0, hidden ? 0x30 : 0xA0));
    }

  else if(it == itBarrow && c) {
    for(int i = 0; i<c->landparam; i++)
      queuepolyat(Vit * spin(2 * M_PI * i / c->landparam) * xpush(.15) * spinptick(1500), *xsh, darkena(icol, 0, hidden ? 0x40 : 
        (highwall(c) && wmspatial) ? 0x60 : 0xFF),
        PPR::HIDDEN);

//      queuepoly(Vit*spin(M_PI+(1-2*ang)*2*M_PI/S84), shMagicSword, darkena(0xC00000, 0, 0x80 + 0x70 * sin(ticks / 200.0)));
    }
    
  else if(xsh) {
    if(it == itFireShard) icol = firecolor(100);
    if(it == itWaterShard) icol = watercolor(100) >> 8;
    
    if(it == itZebra) icol = 0xFFFFFF;
    if(it == itLotus) icol = 0x101010;
    if(it == itSwitch) icol = minf[active_switch()].color;
    
    transmatrix V2 = Vit * spinptick(1500);
  
    if(xsh == &shBookCover && mmitem) {
      if(DIM == 3)
        queuepoly(V2 * cpush(2, 1e-3), shBook, 0x805020FF);
      else
        queuepoly(V2, shBook, 0x805020FF);
      }
    
    PPR pr = PPR::ITEM;
    int alpha = hidden ? (it == itKraken ? 0xC0 : 0x40) : 0xF0;
    if(c && c->wall == waIcewall) pr = PPR::HIDDEN, alpha = 0x80;

    queuepolyat(V2, *xsh, darkena(icol, 0, alpha), pr);

    if(it == itZebra)
      queuepolyat(Vit * spinptick(1500, .5/(ct6+6)), *xsh, darkena(0x202020, 0, hidden ? 0x40 : 0xF0), PPR::ITEMb);
    }
  
  else if(xch == 'o' || it == itInventory) {
    if(it == itOrbFire) icol = firecolor(100);
    PPR prio = PPR::ITEM;
    bool inice = c && c->wall == waIcewall;
    if(inice) prio = PPR::HIDDEN;

    int icol1 = icol;
    if(it == itOrbFire) icol = firecolor(200);
    if(it == itOrbFriend || it == itOrbDiscord) icol = 0xC0C0C0;
    if(it == itOrbFrog) icol = 0xFF0000;
    if(it == itOrbDash) icol = 0xFF0000;
    if(it == itOrbFreedom) icol = 0xC0FF00;
    if(it == itOrbAir) icol = 0xFFFFFF;
    if(it == itOrbUndeath) icol = minf[moFriendlyGhost].color;
    if(it == itOrbRecall) icol = 0x101010;
    if(it == itOrbSlaying) icol = 0xFF0000;
    color_t col = darkena(icol, 0, int(0x80 + 0x70 * sinptick(300)));
    
    if(it == itOrbFish)
      queuepolyat(Vit * spinptick(1500), shFishTail, col, PPR::ITEM_BELOW);

    queuepolyat(Vit, shDisk, darkena(icol1, 0, inice ? 0x80 : hidden ? 0x20 : 0xC0), prio);
    hpcshape& sh = 
      it == itOrbLove ? shLoveRing :
      isRangedOrb(it) ? shTargetRing :
      isOffensiveOrb(it) ? shSawRing :
      isFriendOrb(it) ? shPeaceRing :
      isUtilityOrb(it) ? shGearRing :
      isDirectionalOrb(it) ? shSpearRing :
      among(it, itOrb37, itOrbGravity) ? shHeptaRing :
      shRing;
    queuepolyat(Vit * spinptick(1500), sh, col, prio);
    }

  else if(it) return true;

  return false;
#endif
  }

#if CAP_SHAPES
void drawTerraWarrior(const transmatrix& V, int t, int hp, double footphase) {
  ShadowV(V, shPBody);
  color_t col = linf[laTerracotta].color;
  int bcol = darkena(false ? 0xC0B23E : col, 0, 0xFF);
  const transmatrix VBS = VBODY * otherbodyparts(V, bcol, moDesertman, footphase);
  queuepoly(VBS, shPBody, bcol);
  if(!peace::on) queuepoly(VBS * Mirror, shPSword, darkena(0xC0C0C0, 0, 0xFF));
  queuepoly(VBS, shTerraArmor1, darkena(t > 0 ? 0x4040FF : col, 0, 0xFF));
  if(hp >= 4) queuepoly(VBS, shTerraArmor2, darkena(t > 1 ? 0xC00000 : col, 0, 0xFF));
  if(hp >= 2) queuepoly(VBS, shTerraArmor3, darkena(t > 2 ? 0x612600 : col, 0, 0xFF));
  queuepoly(VHEAD, shTerraHead, darkena(t > 4 ? 0x202020 : t > 3 ? 0x504040 : col, 0, 0xFF));
  queuepoly(VHEAD, shPFace, bcol);
  }
#endif

bool drawMonsterType(eMonster m, cell *where, const transmatrix& V, color_t col, double footphase) {

#if CAP_SHAPES
  char xch = minf[m].glyph;

  if(m == moTortoise && where && where->stuntime >= 3)
    drawStunStars(V, where->stuntime-2);
  else if (m == moTortoise || m == moPlayer || (where && !where->stuntime)) ;
  else if(where && !(isMetalBeast(m) && where->stuntime == 1))
    drawStunStars(V, where->stuntime);

  if(m == moTortoise) {
    int bits = where ? tortoise::getb(where) : tortoise::last;
    tortoise::draw(V, bits, 0, where ? where->stuntime : 0);
    if(tortoise::seek() && !tortoise::diff(bits) && where)
      queuepoly(V, shRing, darkena(0xFFFFFF, 0, 0x80 + 0x70 * sintick(200)));
    }
    
  else if(m == moPlayer) {
  
    charstyle& cs = getcs();

    if(mapeditor::drawplayer && !mapeditor::drawUserShape(V, mapeditor::sgPlayer, cs.charid, cs.skincolor, where)) {
    
      if(cs.charid >= 8) {
        if(!mmspatial && !footphase)
          queuepoly(VALEGS, shWolfLegs, fc(150, cs.dresscolor, 4));   
        else {
          ShadowV(V, shWolfBody);
          animallegs(VALEGS, moWolf, fc(500, cs.dresscolor, 4), footphase);
          }
        queuepoly(VABODY, shWolfBody, fc(0, cs.skincolor, 0));
        queuepoly(VAHEAD, shFamiliarHead, fc(500, cs.haircolor, 2));
        if(!shmup::on || shmup::curtime >= shmup::getPlayer()->nextshot) {
          color_t col = items[itOrbDiscord] ? watercolor(0) : fc(314, cs.swordcolor, 3);
          queuepoly(VAHEAD, shFamiliarEye, col);
          queuepoly(VAHEAD * Mirror, shFamiliarEye, col);
          }

        if(knighted)
          queuepoly(VABODY, shKnightCloak, darkena(cloakcolor(knighted), 1, 0xFF));
  
        if(tortoise::seek())
          tortoise::draw(VABODY, tortoise::seekbits, 4, 0);
        }
      else if(cs.charid >= 6) {
        if(!mmspatial && !footphase)
          queuepoly(VABODY, shDogBody, fc(0, cs.skincolor, 0));
        else {
          ShadowV(V, shDogTorso);          
          animallegs(VALEGS, moRunDog, fc(500, cs.dresscolor, 4), footphase);
          queuepoly(VABODY, shDogTorso, fc(0, cs.skincolor, 0));
          }
        queuepoly(VAHEAD, shDogHead, fc(150, cs.haircolor, 2));

        if(!shmup::on || shmup::curtime >= shmup::getPlayer()->nextshot) {
          color_t col = items[itOrbDiscord] ? watercolor(0) : fc(314, cs.swordcolor, 3);
          queuepoly(VAHEAD, shWolf1, col);
          queuepoly(VAHEAD, shWolf2, col);
          }

        color_t colnose = items[itOrbDiscord] ? watercolor(0) : fc(314, 0xFF, 3);
        queuepoly(VAHEAD, shWolf3, colnose);

        if(knighted)
          queuepoly(VABODY, shKnightCloak, darkena(cloakcolor(knighted), 1, 0xFF));
  
        if(tortoise::seek())
          tortoise::draw(VABODY, tortoise::seekbits, 4, 0);
        }
      else if(cs.charid >= 4) {
        if(!mmspatial && !footphase)
          queuepoly(VALEGS, shCatLegs, fc(500, cs.dresscolor, 4));
        else {
          ShadowV(V, shCatBody);
          animallegs(VALEGS, moRunDog, fc(500, cs.dresscolor, 4), footphase);
          }
        queuepoly(VABODY, shCatBody, fc(0, cs.skincolor, 0));
        queuepoly(VAHEAD, shCatHead, fc(150, cs.haircolor, 2));
        if(!shmup::on || shmup::curtime >= shmup::getPlayer()->nextshot) {
          color_t col = items[itOrbDiscord] ? watercolor(0) : fc(314, cs.swordcolor, 3);
          queuepoly(VAHEAD * xpush(.04), shWolf1, col);
          queuepoly(VAHEAD * xpush(.04), shWolf2, col);
          }

        if(knighted)
          queuepoly(VABODY, shKnightCloak, darkena(cloakcolor(knighted), 1, 0xFF));
  
        if(tortoise::seek())
          tortoise::draw(VABODY, tortoise::seekbits, 4, 0);
        }
      else {

      const transmatrix VBS = VBODY * otherbodyparts(V, fc(0, cs.skincolor, 0), items[itOrbFish] ? moWaterElemental : moPlayer, footphase);

      queuepoly(VBS, (cs.charid&1) ? shFemaleBody : shPBody, fc(0, cs.skincolor, 0));      

      ShadowV(V, (cs.charid&1) ? shFemaleBody : shPBody);

      if(cs.charid&1)
        queuepoly(VBS, shFemaleDress, fc(500, cs.dresscolor, 4));

      if(cs.charid == 2)
        queuepoly(VBS, shPrinceDress,  fc(400, cs.dresscolor, 5));
      if(cs.charid == 3) 
        queuepoly(VBS, shPrincessDress,  fc(400, cs.dresscolor2, 5));
        
      if(items[itOrbSide3])
        queuepoly(VBS, (cs.charid&1) ? shFerocityF : shFerocityM, fc(0, cs.skincolor, 0));

      if(items[itOrbHorns]) {
        queuepoly(VBS, shBullHead, items[itOrbDiscord] ? watercolor(0) : 0xFF000030);
        queuepoly(VBS, shBullHorn, items[itOrbDiscord] ? watercolor(0) : 0xFF000040);
        queuepoly(VBS * Mirror, shBullHorn, items[itOrbDiscord] ? watercolor(0) : 0xFF000040);
        }

      if(items[itOrbSide1] && !shmup::on)
        queuepoly(VBS * spin(-M_PI/24), cs.charid >= 2 ? shSabre : shPSword, fc(314, cs.swordcolor, 3)); // 3 not colored
      
      transmatrix VWPN = cs.lefthanded ? VBS * Mirror : VBS;
      
      if(peace::on) ;
      else if(racing::on) {
#if CAP_RACING
        if(racing::trophy[multi::cpid])
          queuepoly(VWPN, shTrophy, racing::trophy[multi::cpid]);
#endif
        }
      else if(items[itOrbThorns])
        queuepoly(VWPN, shHedgehogBladePlayer, items[itOrbDiscord] ? watercolor(0) : 0x00FF00FF);
      else if(!shmup::on && items[itOrbDiscord])
        queuepoly(VWPN, cs.charid >= 2 ? shSabre : shPSword, watercolor(0));
      else if(items[itRevolver])
        queuepoly(VWPN, shGunInHand, fc(314, cs.swordcolor, 3)); // 3 not colored
      else if(items[itOrbSlaying]) {
        queuepoly(VWPN, shFlailTrunk, fc(314, cs.swordcolor, 3));
        queuepoly(VWPN, shHammerHead, fc(314, cs.swordcolor, 3));
        }
      else if(!shmup::on)
        queuepoly(VWPN, cs.charid >= 2 ? shSabre : shPSword, fc(314, cs.swordcolor, 3)); // 3 not colored
      else if(shmup::curtime >= shmup::getPlayer()->nextshot)
        queuepoly(VWPN, shPKnife, fc(314, cs.swordcolor, 3)); // 3 not colored
      
      if(items[itOrbBeauty]) {
        if(cs.charid&1)
          queuepoly(VHEAD, shFlowerHair, darkena(iinf[itOrbBeauty].color, 0, 0xFF));
        else
          queuepoly(VWPN, shFlowerHand, darkena(iinf[itOrbBeauty].color, 0, 0xFF));
        }
      
      if(where && where->land == laWildWest) {
        queuepoly(VHEAD, shWestHat1, darkena(cs.swordcolor, 1, 0XFF));
        queuepoly(VHEAD, shWestHat2, darkena(cs.swordcolor, 0, 0XFF));
        }

      if(cheater && !autocheat) {
        queuepoly(VHEAD, (cs.charid&1) ? shGoatHead : shDemon, darkena(0xFFFF00, 0, 0xFF));
        // queuepoly(V, shHood, darkena(0xFF00, 1, 0xFF));
        }
      else {
        queuepoly(VHEAD, shPFace, fc(500, cs.skincolor, 1));
        queuepoly(VHEAD, (cs.charid&1) ? shFemaleHair : shPHead, fc(150, cs.haircolor, 2));
        }      

      if(knighted)
        queuepoly(VBS, shKnightCloak, darkena(cloakcolor(knighted), 1, 0xFF));

      if(tortoise::seek())
        tortoise::draw(VBS * ypush(-crossf*.25), tortoise::seekbits, 4, 0);
      }


      }
    }
  
  else if(mapeditor::drawUserShape(V, mapeditor::sgMonster, m, darkena(col, 0, 0xFF), where)) {
    return false;
    }

  else if(isMimic(m) || m == moShadow || m == moIllusion) {
    charstyle& cs = getcs();
    
    if(mapeditor::drawUserShape(V, mapeditor::sgPlayer, cs.charid, darkena(col, 0, 0x80), where)) return false;
    
    if(cs.charid >= 8) {
      queuepoly(VABODY, shWolfBody, darkena(col, 0, 0xC0));
      ShadowV(V, shWolfBody);

      if(mmspatial || footphase)
        animallegs(VALEGS, moWolf, darkena(col, 0, 0xC0), footphase);
      else 
        queuepoly(VABODY, shWolfLegs, darkena(col, 0, 0xC0));

      queuepoly(VABODY, shFamiliarHead, darkena(col, 0, 0xC0));
      queuepoly(VAHEAD, shFamiliarEye, darkena(col, 0, 0xC0));
      queuepoly(VAHEAD * Mirror, shFamiliarEye, darkena(col, 0, 0xC0));
      }
    else if(cs.charid >= 6) {
      ShadowV(V, shDogBody);
      queuepoly(VAHEAD, shDogHead, darkena(col, 0, 0xC0));
      if(mmspatial || footphase) {
        animallegs(VALEGS, moRunDog, darkena(col, 0, 0xC0), footphase);
        queuepoly(VABODY, shDogTorso, darkena(col, 0, 0xC0));
        }
      else 
        queuepoly(VABODY, shDogBody, darkena(col, 0, 0xC0));
      queuepoly(VABODY, shWolf1, darkena(col, 1, 0xC0));
      queuepoly(VABODY, shWolf2, darkena(col, 1, 0xC0));
      queuepoly(VABODY, shWolf3, darkena(col, 1, 0xC0));
      }
    else if(cs.charid >= 4) {
      ShadowV(V, shCatBody);
      queuepoly(VABODY, shCatBody, darkena(col, 0, 0xC0));
      queuepoly(VAHEAD, shCatHead, darkena(col, 0, 0xC0));
      if(mmspatial || footphase) 
        animallegs(VALEGS, moRunDog, darkena(col, 0, 0xC0), footphase);
      else 
        queuepoly(VALEGS, shCatLegs, darkena(col, 0, 0xC0));
      queuepoly(VAHEAD * xpush(.04), shWolf1, darkena(col, 1, 0xC0));
      queuepoly(VAHEAD * xpush(.04), shWolf2, darkena(col, 1, 0xC0));
      }
    else {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0x40), m, footphase);
      queuepoly(VBS, (cs.charid&1) ? shFemaleBody : shPBody,  darkena(col, 0, 0X80));
  
      if(!shmup::on) {
        bool emp = items[itOrbEmpathy] && m != moShadow;
        if(items[itOrbThorns] && emp)
          queuepoly(VBS, shHedgehogBladePlayer, darkena(col, 0, 0x40));
        if(items[itOrbSide1] && !shmup::on)
          queuepoly(VBS * spin(-M_PI/24), cs.charid >= 2 ? shSabre : shPSword, darkena(col, 0, 0x40));      
        if(items[itOrbSide3] && emp)
          queuepoly(VBS, (cs.charid&1) ? shFerocityF : shFerocityM, darkena(col, 0, 0x40));

        queuepoly(VBS, (cs.charid >= 2 ? shSabre : shPSword), darkena(col, 0, 0XC0));
        }
      else if(!where || shmup::curtime >= shmup::getPlayer()->nextshot)
        queuepoly(VBS, shPKnife, darkena(col, 0, 0XC0));

      if(knighted)
        queuepoly(VBS, shKnightCloak, darkena(col, 1, 0xC0));
  
      queuepoly(VHEAD, (cs.charid&1) ? shFemaleHair : shPHead,  darkena(col, 1, 0XC0));
      queuepoly(VHEAD, shPFace,  darkena(col, 0, 0XC0));
      if(cs.charid&1)
        queuepoly(VBS, shFemaleDress,  darkena(col, 1, 0XC0));
      if(cs.charid == 2)
        queuepoly(VBS, shPrinceDress,  darkena(col, 1, 0XC0));
      if(cs.charid == 3)
        queuepoly(VBS, shPrincessDress,  darkena(col, 1, 0XC0));
      }
    }
  
  else if(m == moBullet) {
    ShadowV(V, shKnife);
    queuepoly(VBODY * spin(-M_PI/4), shKnife, getcs().swordcolor);
    }
  
  else if(m == moKnight || m == moKnightMoved) {
    ShadowV(V, shPBody);
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(0xC0C0A0, 0, 0xC0), m, footphase);
    queuepoly(VBS, shPBody, darkena(0xC0C0A0, 0, 0xC0));
    if(!racing::on) 
      queuepoly(VBS, shPSword, darkena(0xFFFF00, 0, 0xFF));
    queuepoly(VBS, shKnightArmor, darkena(0xD0D0D0, 1, 0xFF));
    color_t col;
    if(!eubinary && where && where->master->alt)
      col = cloakcolor(roundTableRadius(where));
    else
      col = cloakcolor(newRoundTableRadius());
    queuepoly(VBS, shKnightCloak, darkena(col, 1, 0xFF));
    queuepoly(VHEAD, shPHead, darkena(0x703800, 1, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xC0C0A0, 0, 0XFF));
    return false;
    }
  
  else if(m == moGolem || m == moGolemMoved) {
    ShadowV(V, shPBody);
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0XC0), m, footphase);
    queuepoly(VBS, shPBody, darkena(col, 0, 0XC0));
    queuepoly(VHEAD, shGolemhead, darkena(col, 1, 0XFF));
    }

  else if(isPrincess(m) || m == moFalsePrincess || m == moRoseLady || m == moRoseBeauty) {
  
    bool girl = princessgender() == GEN_F;
    bool evil = !isPrincess(m);

    int facecolor = evil ? 0xC0B090FF : 0xD0C080FF;
    
    ShadowV(V, girl ? shFemaleBody : shPBody);
    const transmatrix VBS = VBODY * otherbodyparts(V, facecolor, m, footphase);
    queuepoly(VBS, girl ? shFemaleBody : shPBody, facecolor);

    if(m == moPrincessArmed) 
      queuepoly(VBS * Mirror, vid.cs.charid < 2 ? shSabre : shPSword, 0xFFFFFFFF);
    
    if((m == moFalsePrincess || m == moRoseBeauty) && where && where->cpdist == 1)
      queuepoly(VBS, shPKnife, 0xFFFFFFFF);

    if(m == moRoseLady) {
      queuepoly(VBS, shPKnife, 0xFFFFFFFF);
      queuepoly(VBS * Mirror, shPKnife, 0xFFFFFFFF);
      }

    if(girl) {
      queuepoly(VBS, shFemaleDress,  evil ? 0xC000C0FF : 0x00C000FF);
      if(vid.cs.charid < 2) 
        queuepoly(VBS, shPrincessDress, evil ? 0xC040C0C0 : 0x8080FFC0);
      }
    else {
      if(vid.cs.charid < 2) 
        queuepoly(VBS, shPrinceDress,  evil ? 0x802080FF : 0x404040FF);
      }    

    if(m == moRoseLady) {
//    queuepoly(V, girl ? shGoatHead : shDemon,  0x800000FF);
      queuepoly(VHEAD, girl ? shFemaleHair : shPHead,  evil ? 0x500050FF : 0x332A22FF);
      }
    else if(m == moRoseBeauty) {
      if(girl) {
        queuepoly(VHEAD, shBeautyHair,  0xF0A0D0FF);
        queuepoly(VHEAD, shFlowerHair,  0xC00000FF);
        }
      else {
        queuepoly(VHEAD, shPHead,  0xF0A0D0FF);
        queuepoly(VBS, shFlowerHand,  0xC00000FF);
        queuepoly(VBS, shSuspenders,  0xC00000FF);
        }
      }
    else {
      queuepoly(VHEAD, girl ? shFemaleHair : shPHead,  
        evil ? 0xC00000FF : 0x332A22FF);
      }
    queuepoly(VHEAD, shPFace,  facecolor);
    }

  else if(m == moWolf || m == moRedFox || m == moWolfMoved || m == moLavaWolf) {
    ShadowV(V, shWolfBody);
    if(mmspatial || footphase)
      animallegs(VALEGS, moWolf, darkena(col, 0, 0xFF), footphase);
    else
      queuepoly(VALEGS, shWolfLegs, darkena(col, 0, 0xFF));
    queuepoly(VABODY, shWolfBody, darkena(col, 0, 0xFF));
    if(m == moRedFox) {
      queuepoly(VABODY, shFoxTail1, darkena(col, 0, 0xFF));
      queuepoly(VABODY, shFoxTail2, darkena(0xFFFFFF, 0, 0xFF));
      }
    queuepoly(VAHEAD, shWolfHead, darkena(col, 0, 0xFF));
    queuepoly(VAHEAD, shWolfEyes, darkena(col, 3, 0xFF));
    }

  else if(isBull(m)) {
    ShadowV(V, shBullBody);
    int hoofcol = darkena(gradient(0, col, 0, .65, 1), 0, 0xFF);
    if(mmspatial || footphase)
      animallegs(VALEGS, moRagingBull, hoofcol, footphase);
    queuepoly(VABODY, shBullBody, darkena(gradient(0, col, 0, .80, 1), 0, 0xFF));
    queuepoly(VAHEAD, shBullHead, darkena(col, 0, 0xFF));
    queuepoly(VAHEAD, shBullHorn, darkena(0xFFFFFF, 0, 0xFF));
    queuepoly(VAHEAD * Mirror, shBullHorn, darkena(0xFFFFFF, 0, 0xFF));
    }

  else if(m == moReptile) {
    ShadowV(V, shReptileBody);
    animallegs(VALEGS, moReptile, darkena(col, 0, 0xFF), footphase);
    queuepoly(VABODY, shReptileBody, darkena(col, 0, 0xFF));
    queuepoly(VAHEAD, shReptileHead, darkena(col, 0, 0xFF));
    queuepoly(VAHEAD, shReptileEye, darkena(col, 3, 0xFF));
    queuepoly(VAHEAD * Mirror, shReptileEye, darkena(col, 3, 0xFF));
    queuepoly(VABODY, shReptileTail, darkena(col, 2, 0xFF));
    }

  else if(m == moSalamander) {
    ShadowV(V, shReptileBody);
    animallegs(VALEGS, moReptile, darkena(0xD00000, 1, 0xFF), footphase);
    queuepoly(VABODY, shReptileBody, darkena(0xD00000, 0, 0xFF));
    queuepoly(VAHEAD, shReptileHead, darkena(0xD00000, 1, 0xFF));
    queuepoly(VAHEAD, shReptileEye, darkena(0xD00000, 0, 0xFF));
    queuepoly(VAHEAD * Mirror, shReptileEye, darkena(0xD00000, 0, 0xFF));
    queuepoly(VABODY, shReptileTail, darkena(0xD08000, 0, 0xFF));
    }

  else if(m == moVineBeast) {
    ShadowV(V, shWolfBody);
    if(mmspatial || footphase)
      animallegs(VALEGS, moWolf, 0x00FF00FF, footphase);
    else
      queuepoly(VALEGS, shWolfLegs, 0x00FF00FF);
    queuepoly(VABODY, shWolfBody, darkena(col, 1, 0xFF));
    queuepoly(VAHEAD, shWolfHead, darkena(col, 0, 0xFF));
    queuepoly(VAHEAD, shWolfEyes, 0xFF0000FF);
    }
  else if(m == moMouse || m == moMouseMoved) {
    queuepoly(VALEGS, shMouse, darkena(col, 0, 0xFF));
    queuepoly(VALEGS, shMouseLegs, darkena(col, 1, 0xFF));
    queuepoly(VALEGS, shMouseEyes, 0xFF);
    }
  else if(isBug(m)) {
    ShadowV(V, shBugBody);
    if(!mmspatial && !footphase)
      queuepoly(VABODY, shBugBody, darkena(col, 0, 0xFF));
    else {
      animallegs(VALEGS, moBug0, darkena(col, 0, 0xFF), footphase);
      queuepoly(VABODY, shBugAntenna, darkena(col, 1, 0xFF));
      }
    queuepoly(VABODY, shBugArmor, darkena(col, 1, 0xFF));
    }
  else if(among(m, moRunDog, moHunterDog, moHunterGuard, moHunterChanging, moFallingDog)) {
    if(!mmspatial && !footphase) 
      queuepoly(VABODY, shDogBody, darkena(col, 0, 0xFF));
    else {
      ShadowV(V, shDogTorso);
      queuepoly(VABODY, shDogTorso, darkena(col, 0, 0xFF));
      animallegs(VALEGS, moRunDog, m == moFallingDog ? 0xFFFFFFFF : darkena(col, 0, 0xFF), footphase);
      }
    queuepoly(VAHEAD, shDogHead, darkena(col, 0, 0xFF));

    {
    dynamicval<color_t> dp(poly_outline);
    int eyecolor = 0x202020;
    bool redeyes = false;
    if(m == moHunterDog) eyecolor = 0xFF0000, redeyes = true;
    if(m == moHunterGuard) eyecolor = 0xFF6000, redeyes = true;
    if(m == moHunterChanging) eyecolor = 0xFFFF00, redeyes = true;
    int eyes = darkena(eyecolor, 0, 0xFF);

    if(redeyes) poly_outline = eyes;
    queuepoly(VAHEAD, shWolf1, eyes).flags |= POLY_FORCEWIDE;
    queuepoly(VAHEAD, shWolf2, eyes).flags |= POLY_FORCEWIDE;
    }
    queuepoly(VAHEAD, shWolf3, darkena(m == moRunDog ? 0x202020 : 0x000000, 0, 0xFF));
    }
  else if(m == moOrangeDog) {
    if(!mmspatial && !footphase) 
      queuepoly(VABODY, shDogBody, darkena(0xFFFFFF, 0, 0xFF));
    else {
      ShadowV(V, shDogTorso);
      queuepoly(VABODY, shDogTorso, darkena(0xFFFFFF, 0, 0xFF));
      animallegs(VALEGS, moRunDog, darkena(0xFFFFFF, 0, 0xFF), footphase);
      }
    queuepoly(VAHEAD, shDogHead, darkena(0xFFFFFF, 0, 0xFF));
    queuepoly(VABODY, shDogStripes, darkena(0x303030, 0, 0xFF));
    queuepoly(VAHEAD, shWolf1, darkena(0x202020, 0, 0xFF));
    queuepoly(VAHEAD, shWolf2, darkena(0x202020, 0, 0xFF));
    queuepoly(VAHEAD, shWolf3, darkena(0x202020, 0, 0xFF));
    }
  else if(m == moShark || m == moGreaterShark || m == moCShark)
    queuepoly(VFISH, shShark, darkena(col, 0, 0xFF));
  else if(m == moEagle || m == moParrot || m == moBomberbird || m == moAlbatross || 
    m == moTameBomberbird || m == moWindCrow || m == moTameBomberbirdMoved ||
    m == moSandBird || m == moAcidBird) {
    ShadowV(V, shEagle);
    queuepoly(VBIRD, shEagle, darkena(col, 0, 0xFF));
    }
  else if(among(m, moSparrowhawk, moWestHawk)) {
    ShadowV(V, shHawk);
    queuepoly(VBIRD, shHawk, darkena(col, 0, 0xFF));
    }
  else if(m == moButterfly) {
    transmatrix Vwing = Id;
    Vwing[1][1] = .85 + .15 * sintick(100);
    ShadowV(V * Vwing, shButterflyWing);
    queuepoly(VBIRD * Vwing, shButterflyWing, darkena(col, 0, 0xFF));
    queuepoly(VBIRD, shButterflyBody, darkena(col, 2, 0xFF));
    }
  else if(m == moGadfly) {
    transmatrix Vwing = Id;
    Vwing[1][1] = .85 + .15 * sintick(100);
    ShadowV(V * Vwing, shGadflyWing);
    queuepoly(VBIRD * Vwing, shGadflyWing, darkena(col, 0, 0xFF));
    queuepoly(VBIRD, shGadflyBody, darkena(col, 1, 0xFF));
    queuepoly(VBIRD, shGadflyEye, darkena(col, 2, 0xFF));
    queuepoly(VBIRD * Mirror, shGadflyEye, darkena(col, 2, 0xFF));
    }
  else if(m == moVampire || m == moBat) {
    // vampires have no shadow and no mirror images
    if(m == moBat) ShadowV(V, shBatWings);
    if(m == moBat || !inmirrorcount) {
      queuepoly(VBIRD, shBatWings, darkena(0x303030, 0, 0xFF));
      queuepoly(VBIRD, shBatBody, darkena(0x606060, 0, 0xFF));
      }
    /* queuepoly(V, shBatMouth, darkena(0xC00000, 0, 0xFF));
    queuepoly(V, shBatFang, darkena(0xFFC0C0, 0, 0xFF));
    queuepoly(V*Mirror, shBatFang, darkena(0xFFC0C0, 0, 0xFF));
    queuepoly(V, shBatEye, darkena(00000000, 0, 0xFF));
    queuepoly(V*Mirror, shBatEye, darkena(00000000, 0, 0xFF)); */
    }
  else if(m == moGargoyle) {
    ShadowV(V, shGargoyleWings);
    queuepoly(VBIRD, shGargoyleWings, darkena(col, 0, 0xD0));
    queuepoly(VBIRD, shGargoyleBody, darkena(col, 0, 0xFF));
    }
  else if(m == moZombie) {
    int c = darkena(col, where && where->land == laHalloween ? 1 : 0, 0xFF);
    const transmatrix VBS = VBODY * otherbodyparts(V, c, m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBS, shPBody, c);
    }
  else if(m == moTerraWarrior)
    drawTerraWarrior(V, 7, (where ? where->hitpoints : 7), footphase);
  else if(m == moVariantWarrior) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBS, shPBody, darkena(0xFFD500, 0, 0xF0));
    if(!peace::on) queuepoly(VBS, shPSword, 0xFFFF00FF);
    queuepoly(VHEAD, shHood, 0x008000FF);
    }
  else if(m == moDesertman) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBS, shPBody, darkena(col, 0, 0xC0));
    if(!peace::on) queuepoly(VBS, shPSword, 0xFFFF00FF);
    queuepoly(VHEAD, shHood, 0xD0D000C0);
    }
  /* else if(isSwitch(m)) {
    otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0xC0));
    if(!peace::on) queuepoly(VBODY, shPSword, 0xFFFF00FF);
    queuepoly(VHEAD, shHood, darkena(col, 0, 0xFF));
    } */
  else if(m == moMonk) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    ShadowV(V, shRaiderBody);
    queuepoly(VBS, shRaiderBody, darkena(col, 0, 0xFF));
    queuepoly(VBS, shRaiderShirt, darkena(col, 2, 0xFF));
    if(!peace::on) queuepoly(VBODY, shPKnife, 0xFFC0C0C0);
    queuepoly(VBS, shRaiderArmor, darkena(col, 1, 0xFF));
    queuepolyat(VBS, shRatCape2, darkena(col, 2, 0xFF), PPR::MONSTER_ARMOR0);
    queuepoly(VHEAD, shRaiderHelmet, darkena(col, 0, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xC0C0A0, 0, 0XFF));
    }
  else if(m == moCrusher) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shRaiderBody);
    queuepoly(VBS, shRaiderBody, darkena(col, 0, 0xFF));
    queuepoly(VBS, shRaiderShirt, darkena(col, 2, 0xFF));
    queuepoly(VBS, shRaiderArmor, darkena(col, 1, 0xFF));
    queuepoly(VBS, shFlailTrunk, darkena(col, 1, 0XFF));
    queuepoly(VBS, shHammerHead, darkena(col, 0, 0XFF));
    queuepoly(VHEAD, shRaiderHelmet, darkena(col, 0, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xC0C0A0, 0, 0XFF));
    }
  else if(m == moPair) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shRaiderBody);
    queuepoly(VBS, shRaiderBody, darkena(col, 0, 0xFF));
    queuepoly(VBS, shRaiderShirt, darkena(col, 2, 0xFF));
    queuepoly(VBS, shRaiderArmor, darkena(col, 1, 0xFF));
    queuepoly(VBS, shPickAxe, darkena(0xA0A0A0, 0, 0XFF));
    queuepoly(VHEAD, shRaiderHelmet, darkena(col, 0, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xC0C0A0, 0, 0XFF));
    }
  else if(m == moAltDemon || m == moHexDemon) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    ShadowV(V, shRaiderBody);
    queuepoly(VBS, shRaiderBody, darkena(col, 0, 0xFF));
    queuepoly(VBS, shRaiderShirt, darkena(col, 2, 0xFF));
    queuepoly(VBS, shRaiderArmor, darkena(col, 1, 0xFF));
    if(!peace::on) queuepoly(VBS, shPSword, 0xFFD0D0D0);
    queuepoly(VHEAD, shRaiderHelmet, darkena(col, 0, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xC0C0A0, 0, 0XFF));
    }
  else if(m == moPalace || m == moFatGuard || m == moVizier || m == moSkeleton) {
    if(m == moSkeleton) {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(0xFFFFFF, 0, 0xFF), moSkeleton, footphase);
      queuepoly(VBS, shSkeletonBody, darkena(0xFFFFFF, 0, 0xFF));
      queuepoly(VHEAD, shSkull, darkena(0xFFFFFF, 0, 0xFF));
      queuepoly(VHEAD, shSkullEyes, darkena(0, 0, 0xFF));
      ShadowV(V, shSkeletonBody);
      queuepoly(VBS, shSabre, 0xFFFFFFFF);
      }
    else {
      ShadowV(V, shPBody);
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(0xFFD500, 0, 0xFF), m, footphase);
      if(m == moFatGuard) {
        queuepoly(VBS, shFatBody, darkena(0xC06000, 0, 0xFF));
        col = 0xFFFFFF;
        if(!where || where->hitpoints >= 3)
          queuepoly(VBS, shKnightCloak, darkena(0xFFC0C0, 1, 0xFF));
        }
      else {
        queuepoly(VBS, shPBody, darkena(0xFFD500, 0, 0xFF));
        queuepoly(VBS, shKnightArmor, m == moVizier ? 0xC000C0FF :
          darkena(0x00C000, 1, 0xFF));
        if(where && where->hitpoints >= 3)
          queuepoly(VBS, shKnightCloak, m == moVizier ? 0x800080Ff :
            darkena(0x00FF00, 1, 0xFF));
        }
      queuepoly(VHEAD, shTurban1, darkena(col, 1, 0xFF));
      if(!where || where->hitpoints >= 2)
        queuepoly(VHEAD, shTurban2, darkena(col, 0, 0xFF));
      queuepoly(VBS, shSabre, 0xFFFFFFFF);
      }
      
    drawStunStars(V, where ? where->stuntime : 0);
    }
  else if(m == moCrystalSage) {
    const transmatrix VBS = VBODY * otherbodyparts(V, 0xFFFFFFFF, m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBS, shPBody, 0xFFFFFFFF);
    queuepoly(VHEAD, shPHead, 0xFFFFFFFF);
    queuepoly(VHEAD, shPFace, 0xFFFFFFFF);
    }
  else if(m == moHedge) {
    ShadowV(V, shPBody);
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    queuepoly(VBS, shPBody, darkena(col, 0, 0xFF));
    queuepoly(VBS, shHedgehogBlade, 0xC0C0C0FF);
    queuepoly(VHEAD, shPHead, 0x804000FF);
    queuepoly(VHEAD, shPFace, 0xF09000FF);
    }
  else if(m == moYeti || m == moMonkey) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBS, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(col, 0, 0xFF));
    }
  else if(m == moResearcher) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBS, shPBody, darkena(0xFFFF00, 0, 0xC0));
    queuepoly(VHEAD, shAztecHead, darkena(col, 0, 0xFF));
    queuepoly(VHEAD, shAztecCap, darkena(0xC000C0, 0, 0xFF));
    }
  else if(m == moFamiliar) {
    /* queuepoly(V, shFemaleBody, darkena(0xC0B070, 0, 0xFF));
    queuepoly(V, shPFace, darkena(0xC0B070, 0, 0XFF));
    queuepoly(V, shWizardCape1, 0x601000FF);
    queuepoly(V, shWizardCape2, 0x781800FF);
    queuepoly(V, shWizardHat1, 0x902000FF);
    queuepoly(V, shWizardHat2, 0xA82800FF); */
    
   // queuepoly(V, shCatBody, darkena(0x601000, 0, 0xFF));
   // queuepoly(V, shGargoyleBody, darkena(0x903000, 0, 0xFF));
   
   ShadowV(V, shWolfBody);
   queuepoly(VABODY, shWolfBody, darkena(0xA03000, 0, 0xFF));
    if(mmspatial || footphase)
      animallegs(VALEGS, moWolf, darkena(0xC04000, 0, 0xFF), footphase);
    else
     queuepoly(VALEGS, shWolfLegs, darkena(0xC04000, 0, 0xFF));
   
   queuepoly(VAHEAD, shFamiliarHead, darkena(0xC04000, 0, 0xFF));
   // queuepoly(V, shCatLegs, darkena(0x902000, 0, 0xFF));
   if(true) {
     queuepoly(VAHEAD, shFamiliarEye, darkena(0xFFFF000, 0, 0xFF));
     queuepoly(VAHEAD * Mirror, shFamiliarEye, darkena(0xFFFF000, 0, 0xFF));
     }
    }
  else if(m == moRanger) {
    ShadowV(V, shPBody);
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    queuepoly(VBS, shPBody, darkena(col, 0, 0xC0));
    if(!peace::on) queuepoly(VBS, shPSword, darkena(col, 0, 0xFF));
    queuepoly(VHEAD, shArmor, darkena(col, 1, 0xFF));
    }
  else if(m == moNarciss) {
    ShadowV(V, shPBody);
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    queuepoly(VBS, shFlowerHand, darkena(col, 0, 0xFF));
    queuepoly(VBS, shPBody, 0xFFE080FF);
    if(!peace::on) queuepoly(VBS, shPKnife, 0xC0C0C0FF);
    queuepoly(VHEAD, shPFace, 0xFFE080FF);
    queuepoly(VHEAD, shPHead, 0x806A00FF);
    }
  else if(m == moMirrorSpirit) {
    ShadowV(V, shPBody);
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0x90), m, footphase);
    queuepoly(VBS, shPBody, darkena(col, 0, 0x90));
    if(!peace::on) queuepoly(VBS * Mirror, shPSword, darkena(col, 0, 0xD0));
    queuepoly(VHEAD, shPHead, darkena(col, 1, 0x90));
    queuepoly(VHEAD, shPFace, darkena(col, 1, 0x90));
    queuepoly(VHEAD, shArmor, darkena(col, 0, 0xC0));
    }
  else if(m == moJiangshi) {
    ShadowV(V, shJiangShi);
    auto z2 = geom3::lev_to_factor(abs(sin(footphase * M_PI * 2)) * geom3::human_height);
    auto V0 = V;
    auto V = mmscale(V0, z2);
    otherbodyparts(V, darkena(col, 0, 0xFF), m, m == moJiangshi ? 0 : footphase);
    queuepoly(VBODY, shJiangShi, darkena(col, 0, 0xFF));
    queuepoly(VBODY, shJiangShiDress, darkena(0x202020, 0, 0xFF));
    queuepoly(VHEAD, shTerraHead, darkena(0x101010, 0, 0xFF));
    queuepoly(VHEAD, shPFace, darkena(col, 0, 0xFF));
    queuepoly(VHEAD, shJiangShiCap1, darkena(0x800000, 0, 0xFF));
    queuepoly(VHEAD, shJiangShiCap2, darkena(0x400000, 0, 0xFF));
    }
  else if(m == moGhost || m == moSeep || m == moFriendlyGhost) {
    if(m == moFriendlyGhost) col = fghostcolor(where);
    queuepoly(VGHOST, shGhost, darkena(col, 0, m == moFriendlyGhost ? 0xC0 : 0x80));
    queuepoly(VGHOST, shEyes, 0xFF);
    }
  else if(m == moVineSpirit) {
    queuepoly(VGHOST, shGhost, 0xD0D0D0C0);
    queuepoly(VGHOST, shEyes, 0xFF0000FF);
    }
  else if(m == moFireFairy) {
    col = firecolor(0);
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shFemaleBody);
    queuepoly(VBS, shFemaleBody, darkena(col, 0, 0XC0));
    queuepoly(VHEAD, shWitchHair, darkena(col, 1, 0xFF));
    queuepoly(VHEAD, shPFace, darkena(col, 0, 0XFF));
    }
  else if(m == moSlime) {
    queuepoly(VFISH, shSlime, darkena(col, 0, 0x80));
    queuepoly(VSLIMEEYE, shEyes, 0xFF);
    }
  else if(isSwitch(m)) {
    queuepoly(VFISH, shJelly, darkena(col, 0, 0xD0));
    queuepolyat(VBODY, shJelly, darkena(col, 0, 0xD0), PPR::MONSTER_BODY);
    queuepolyat(VHEAD, shJelly, darkena(col, 0, 0xD0), PPR::MONSTER_HEAD);
    queuepolyat(VHEAD, shEyes, 0xFF, PPR::MONSTER_HEAD);
    }
  else if(m == moKrakenH) {
    queuepoly(VFISH, shKrakenHead, darkena(col, 0, 0xD0));
    queuepoly(VFISH, shKrakenEye, 0xFFFFFFC0);
    queuepoly(VFISH, shKrakenEye2, 0xC0);
    queuepoly(VFISH * Mirror, shKrakenEye, 0xFFFFFFC0);
    queuepoly(VFISH * Mirror, shKrakenEye2, 0xC0);
    }
  else if(m == moKrakenT) {
    queuepoly(VFISH, shSeaTentacle, darkena(col, 0, 0xD0));
    }
  else if(m == moCultist || m == moPyroCultist || m == moCultistLeader) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBS, shPBody, darkena(col, 0, 0xC0));
    if(!peace::on) queuepoly(VBS, shPSword, darkena(col, 2, 0xFF));
    queuepoly(VHEAD, shHood, darkena(col, 1, 0xFF));
    }
  else if(m == moPirate) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBS, shPBody, darkena(0x404040, 0, 0xFF));
    queuepoly(VBS, shPirateHook, darkena(0xD0D0D0, 0, 0xFF));
    queuepoly(VHEAD, shPFace, darkena(0xFFFF80, 0, 0xFF));
    queuepoly(VHEAD, shEyepatch, darkena(0, 0, 0xC0));
    queuepoly(VHEAD, shPirateHood, darkena(col, 0, 0xFF));
    }
  else if(m == moRatling || m == moRatlingAvenger) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VLEG, shRatTail, darkena(col, 0, 0xFF));
    queuepoly(VBS, shYeti, darkena(col, 1, 0xFF));
    queuepoly(VHEAD, shRatHead, darkena(col, 0, 0xFF));

    float t = sintick(1000, where ? where->cpdist*M_PI : 0);
    int eyecol = t > 0.92 ? 0xFF0000 : 0;
    
    queuepoly(VHEAD, shWolf1, darkena(eyecol, 0, 0xFF));
    queuepoly(VHEAD, shWolf2, darkena(eyecol, 0, 0xFF));
    queuepoly(VHEAD, shWolf3, darkena(0x202020, 0, 0xFF));
    
    if(m == moRatlingAvenger) {
      queuepoly(VBS, shRatCape2, 0x484848FF);
      queuepoly(VHEAD, shRatCape1, 0x303030FF);
      }
    }
  else if(m == moViking) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBS, shPBody, darkena(0xE00000, 0, 0xFF));
    if(!peace::on) queuepoly(VBS, shPSword, darkena(0xD0D0D0, 0, 0xFF));
    queuepoly(VBS, shKnightCloak, darkena(0x404040, 0, 0xFF));
    queuepoly(VHEAD, shVikingHelmet, darkena(0xC0C0C0, 0, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xFFFF80, 0, 0xFF));
    }
  else if(m == moOutlaw) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBS, shPBody, darkena(col, 0, 0xFF));
    queuepoly(VBS, shKnightCloak, darkena(col, 1, 0xFF));
    queuepoly(VHEAD, shWestHat1, darkena(col, 2, 0XFF));
    queuepoly(VHEAD, shWestHat2, darkena(col, 1, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xFFFF80, 0, 0xFF));
    queuepoly(VBODY, shGunInHand, darkena(col, 1, 0XFF));
    }
  else if(m == moNecromancer) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBS, shPBody, 0xC00000C0);
    queuepoly(VHEAD, shHood, darkena(col, 1, 0xFF));
    }
  else if(m == moDraugr) {
    const transmatrix VBS = VBODY * otherbodyparts(V, 0x483828D0, m, footphase);
    queuepoly(VBS, shPBody, 0x483828D0);
    queuepoly(VBS, shPSword, 0xFFFFD0A0);
    queuepoly(VHEAD, shPHead, 0x483828D0);
    // queuepoly(V, shSkull, 0xC06020D0);
    //queuepoly(V, shSkullEyes, 0x000000D0);
//  queuepoly(V, shWightCloak, 0xC0A080A0);
    int b = where ? where->cpdist : 0;
    b--;
    if(b < 0) b = 0;
    if(b > 6) b = 6;
    queuepoly(VHEAD, shWightCloak, 0x605040A0 + 0x10101000 * b);
    }
  else if(m == moVoidBeast) {
    const transmatrix VBS = VBODY * otherbodyparts(V, 0x080808D0, m, footphase);
    queuepoly(VBS, shPBody, 0x080808D0);
    queuepoly(VHEAD, shPHead, 0x080808D0);
    queuepoly(VHEAD, shWightCloak, 0xFF0000A0);
    }
  else if(m == moGoblin) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBS, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shArmor, darkena(col, 1, 0XFF));
    }
  else if(m == moLancer || m == moFlailer || m == moMiner) {
    transmatrix V2 = V;
    if(m == moLancer)
      V2 = V * spin((where && where->type == 6) ? -M_PI/3 : -M_PI/2 );
    transmatrix Vh = mmscale(V2, geom3::HEAD);
    transmatrix Vb = mmscale(V2, geom3::BODY);
    Vb = Vb * otherbodyparts(V2, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V2, shPBody);
    queuepoly(Vb, shPBody, darkena(col, 0, 0xC0));
    queuepoly(Vh, m == moFlailer ? shArmor : shHood, darkena(col, 1, 0XFF));
    if(m == moMiner)
      queuepoly(Vb, shPickAxe, darkena(0xC0C0C0, 0, 0XFF));
    if(m == moLancer)
      queuepoly(Vb, shPike, darkena(col, 0, 0XFF));
    if(m == moFlailer) {
      queuepoly(Vb, shFlailBall, darkena(col, 0, 0XFF));
      queuepoly(Vb, shFlailChain, darkena(col, 1, 0XFF));
      queuepoly(Vb, shFlailTrunk, darkena(col, 0, 0XFF));
      }
    }
  else if(m == moTroll) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBS, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(col, 1, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(col, 2, 0XFF));
    }        
  else if(m == moFjordTroll || m == moForestTroll || m == moStormTroll) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBS, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(col, 1, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(col, 2, 0XFF));
    }        
  else if(m == moDarkTroll) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBS, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(col, 1, 0XFF));
    queuepoly(VHEAD, shPFace, 0xFFFFFF80);
    }        
  else if(m == moRedTroll) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBS, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(0xFF8000, 0, 0XFF));
    queuepoly(VHEAD, shPFace, 0xFFFFFF80);
    }        
  else if(m == moEarthElemental) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shWaterElemental);
    queuepoly(VBS, shWaterElemental, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shFemaleHair, darkena(col, 0, 0XFF));
    queuepoly(VHEAD, shPFace, 0xF0000080);
    }        
  else if(m == moWaterElemental) {
    const transmatrix VBS = VBODY * otherbodyparts(V, watercolor(50), m, footphase);
    ShadowV(V, shWaterElemental);
    queuepoly(VBS, shWaterElemental, watercolor(0));
    queuepoly(VHEAD, shFemaleHair, watercolor(100));
    queuepoly(VHEAD, shPFace, watercolor(200));
    }        
  else if(m == moFireElemental) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(firecolor(50), 0, 0xFF), m, footphase);
    ShadowV(V, shWaterElemental);
    queuepoly(VBS, shWaterElemental, darkena(firecolor(0), 0, 0xFF));
    queuepoly(VHEAD, shFemaleHair, darkena(firecolor(100), 0, 0xFF));
    queuepoly(VHEAD, shPFace, darkena(firecolor(200), 0, 0xFF));
    }
  else if(m == moAirElemental) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0x40), m, footphase);
    ShadowV(V, shWaterElemental);
    queuepoly(VBS, shWaterElemental, darkena(col, 0, 0x80));
    queuepoly(VHEAD, shFemaleHair, darkena(col, 0, 0x80));
    queuepoly(VHEAD, shPFace, darkena(col, 0, 0x80));
    }        
  else if((xch == 'd' || xch == 'D') && m != moDragonHead && m != moDragonTail) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    queuepoly(VBS, shPBody, darkena(col, 1, 0xC0));
    ShadowV(V, shPBody);
    int acol = col;
    if(xch == 'D') acol = 0xD0D0D0;
    queuepoly(VHEAD, shDemon, darkena(acol, 0, 0xFF));
    }
  else if(isMagneticPole(m)) {
    if(m == moNorthPole)
      queuepolyat(VBODY * spin(M_PI), shTentacle, 0x000000C0, PPR::TENTACLE1);
    queuepolyat(VBODY, shDisk, darkena(col, 0, 0xFF), PPR::MONSTER_BODY);
    }
  else if(isMetalBeast(m) || m == moBrownBug) {
    ShadowV(V, shTrylobite);
    if(!mmspatial)
      queuepoly(VABODY, shTrylobite, darkena(col, 1, 0xC0));
    else {
      queuepoly(VABODY, shTrylobiteBody, darkena(col, 1, 0xFF));
      animallegs(VALEGS, moMetalBeast, darkena(col, 1, 0xFF), footphase);
      }
    int acol = col;
    queuepoly(VAHEAD, shTrylobiteHead, darkena(acol, 0, 0xFF));
    }
  else if(m == moEvilGolem || m == moIceGolem) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 2, 0xC0), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBS, shPBody, darkena(col, 0, 0XC0));
    queuepoly(VHEAD, shGolemhead, darkena(col, 1, 0XFF));
    }
  else if(isWitch(m)) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    int cc = 0xFF;
    if(m == moWitchGhost) cc = 0x85 + 120 * sintick(160);
    if(m == moWitchWinter && where) drawWinter(V, 0);
    if(m == moWitchFlash && where) drawFlash(V);
    if(m == moWitchSpeed && where) drawSpeed(V);
    if(m == moWitchFire) col = firecolor(0);
    ShadowV(V, shFemaleBody);
    queuepoly(VBS, shFemaleBody, darkena(col, 0, cc));
//    queuepoly(cV2, ct, shPSword, darkena(col, 0, 0XFF));
//    queuepoly(V, shHood, darkena(col, 0, 0XC0));
    if(m == moWitchFire) col = firecolor(100);
    queuepoly(VHEAD, shWitchHair, darkena(col, 1, cc));
    if(m == moWitchFire) col = firecolor(200);
    queuepoly(VHEAD, shPFace, darkena(col, 0, cc));
    if(m == moWitchFire) col = firecolor(300);
    queuepoly(VBS, shWitchDress, darkena(col, 1, 0XC0));
    }

  // just for the HUD glyphs...
  else if(isIvy(m) || isMutantIvy(m) || m == moFriendlyIvy) {
    queuepoly(V, shILeaf[0], darkena(col, 0, 0xFF));
    }
  else if(m == moWorm || m == moWormwait || m == moHexSnake) {
    queuepoly(V, shWormHead, darkena(col, 0, 0xFF));
    queuepolyat(V, shEyes, 0xFF, PPR::ONTENTACLE_EYES);
    }
  else if(m == moDragonHead) {
    queuepoly(V, shDragonHead, darkena(col, 0, 0xFF));
    queuepolyat(V, shEyes, 0xFF, PPR::ONTENTACLE_EYES);          
    int noscolor = 0xFF0000FF;
    queuepoly(V, shDragonNostril, noscolor);
    queuepoly(V * Mirror, shDragonNostril, noscolor);
    }
  else if(m == moDragonTail) {
    queuepoly(V, shDragonSegment, darkena(col, 0, 0xFF));
    }
  else if(m == moTentacle || m == moTentaclewait || m == moTentacleEscaping) {
    queuepoly(V, shTentHead, darkena(col, 0, 0xFF));
    ShadowV(V, shTentHead, PPR::GIANTSHADOW);
    }

  else return true;
  
  return false;
#else
  return true;
#endif
  }

bool drawMonsterTypeDH(eMonster m, cell *where, const transmatrix& V, color_t col, bool dh, ld footphase) {
  dynamicval<color_t> p(poly_outline, poly_outline);
  if(dh) {
    poly_outline = OUTLINE_DEAD;
    darken++;
    }
  bool b = drawMonsterType(m,where,V,col, footphase);
  if(dh) {
    darken--;
    }
  return b;
  }

transmatrix playerV;

bool applyAnimation(cell *c, transmatrix& V, double& footphase, int layer) {
  if(!animations[layer].count(c)) return false;
  animation& a = animations[layer][c];

  int td = ticks - a.ltick;
  ld aspd = td / 1000.0 * exp(vid.mspeed);
  ld R;
  again:
  
  if(a.attacking == 1) 
    R = hdist(tC0(a.attackat), tC0(a.wherenow));
  else
    R = hdist0(tC0(a.wherenow));
  aspd *= (1+R+(shmup::on?1:0));
  
  if(R < aspd || std::isnan(R) || std::isnan(aspd) || R > 10) {
    if(a.attacking == 1) { a.attacking = 2; goto again; }
    animations[layer].erase(c);
    return false;
    }
  else {
    if(a.attacking == 1) 
      a.wherenow = a.wherenow * rspintox(tC0(inverse(a.wherenow) * a.attackat));
    else
      a.wherenow = a.wherenow * rspintox(tC0(inverse(a.wherenow)));
    a.wherenow = a.wherenow * xpush(aspd);
    fixmatrix(a.wherenow);
    a.footphase += a.attacking == 2 ? -aspd : aspd;
    footphase = a.footphase;
    V = V * a.wherenow;
    if(a.attacking == 2) V = V * pispin;
    a.ltick = ticks;
    return true;
    }
  }

double chainAngle(cell *c, transmatrix& V, cell *c2, double dft, const transmatrix &Vwhere) {
  if(!gmatrix0.count(c2)) return dft;
  hyperpoint h = C0;
  if(animations[LAYER_BIG].count(c2)) h = animations[LAYER_BIG][c2].wherenow * h;
  h = inverse(V) * Vwhere * calc_relative_matrix(c2, c, C0) * h;
  return atan2(h[1], h[0]);
  }

// equivalent to V = V * spin(-chainAngle(c,V,c2,dft));
bool chainAnimation(cell *c, transmatrix& V, cell *c2, int i, ld bonus, const transmatrix &Vwhere, ld& length) {
  if(!gmatrix0.count(c2)) {
    V = V * ddspin(c,i,bonus);
    length = cellgfxdist(c,i);
    return false;
    }
  hyperpoint h = C0;
  if(animations[LAYER_BIG].count(c2)) h = animations[LAYER_BIG][c2].wherenow * h;
  h = inverse(V) * Vwhere * calc_relative_matrix(c2, c, C0) * h;
  length = hdist0(h);
  V = V * rspintox(h);
  return true;  
  }

// push down the queue after q-th element, `down` absolute units down,
// based on cell c and transmatrix V
// do change the zoom factor? do change the priorities?

int cellcolor(cell *c) {
  if(isPlayerOn(c) || isFriendly(c)) return OUTLINE_FRIEND;
  if(noHighlight(c->monst)) return OUTLINE_NONE;
  if(c->monst) return OUTLINE_ENEMY;
  
  if(c->wall == waMirror) return c->land == laMirror ? OUTLINE_TREASURE : OUTLINE_ORB;

  if(c->item) {
    int k = itemclass(c->item);
    if(k == IC_TREASURE)
      return OUTLINE_TREASURE;
    else if(k == IC_ORB)
      return OUTLINE_ORB;
    else
      return OUTLINE_OTHER;
    }

  return OUTLINE_NONE;
  } 

int taildist(cell *c) {
  int s = 0;
  while(s < 1000 && c->mondir != NODIR && isWorm(c->monst)) {
    s++; c = c->move(c->mondir);
    }
  return s;
  }

int last_wormsegment = -1;
vector<vector< function<void()> > > wormsegments;

void add_segment(int d, const function<void()>& s) {
  if(isize(wormsegments) <= d) wormsegments.resize(d+1);
  last_wormsegment = max(last_wormsegment, d);
  wormsegments[d].push_back(s);
  }

void drawWormSegments() {
  for(int i=0; i<=last_wormsegment; i++) {
    for(auto& f: wormsegments[i]) f();
    wormsegments[i].clear();
    }
  last_wormsegment = -1;
  }

bool dont_face_pc = false;

bool drawMonster(const transmatrix& Vparam, int ct, cell *c, color_t col) {
  #if CAP_SHAPES

  bool darkhistory = conformal::includeHistory && conformal::inkillhistory.count(c);
  
  if(doHighlight())
    poly_outline = 
      (isPlayerOn(c) || isFriendly(c)) ? OUTLINE_FRIEND : 
      noHighlight(c->monst) ? OUTLINE_NONE :
      OUTLINE_ENEMY;
    
  bool nospins = false, nospinb = false;
  double footphaseb = 0, footphase = 0;
  
  transmatrix Vs = Vparam; nospins = applyAnimation(c, Vs, footphase, LAYER_SMALL);
  transmatrix Vb = Vparam; nospinb = applyAnimation(c, Vb, footphaseb, LAYER_BIG);
//  nospin = true;

  eMonster m = c->monst;
    
  if(isIvy(c) || isWorm(c) || isMutantIvy(c) || c->monst == moFriendlyIvy) {
    
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
    
    transmatrix Vb0 = Vb;
    if(c->mondir != NODIR) {
      
      if(mmmon) {
        ld length;
        // cell *c2 = c->move(c->mondir);
        if(nospinb) 
          chainAnimation(c, Vb, c->move(c->mondir), c->mondir, 0, Vparam, length);
        else {
          Vb = Vb * ddspin(c, c->mondir);
          length = cellgfxdist(c, c->mondir);
          }
        
        if(mapeditor::drawUserShape(Vb, mapeditor::sgMonster, c->monst, (col << 8) + 0xFF, c)) 
          return false;

        if(isIvy(c) || isMutantIvy(c) || c->monst == moFriendlyIvy)
          queuepoly(Vb, shIBranch, (col << 8) + 0xFF);
/*         else if(c->monst < moTentacle && wormstyle == 0) {
          ShadowV(Vb, shTentacleX, PPR::GIANTSHADOW);
          queuepoly(mmscale(Vb, geom3::ABODY), shTentacleX, 0xFF);
          queuepoly(mmscale(Vb, geom3::ABODY), shTentacle, (col << 8) + 0xFF);
          } */
//        else if(c->monst < moTentacle) {
//          }

        else if(c->monst == moDragonHead || c->monst == moDragonTail) {
          char part = dragon::bodypart(c, dragon::findhead(c));
          if(part != '2') {
            queuepoly(mmscale(Vb, geom3::ABODY), shDragonSegment, darkena(col, 0, 0xFF));
            ShadowV(Vb, shDragonSegment, PPR::GIANTSHADOW);
            }
          }
        else {
          if(c->monst == moTentacleGhost) {
            hyperpoint V0 = conformal::on ? tC0(Vs) : inverse(cwtV) * tC0(Vs);
            hyperpoint V1 = spintox(V0) * V0;
            Vs = cwtV * rspintox(V0) * rpushxto0(V1) * pispin;
            drawMonsterType(moGhost, c, Vs, darkena(col, 0, 0xFF), footphase);
            col = minf[moTentacletail].color;
            }
          /*
          queuepoly(mmscale(Vb, geom3::ABODY), shTentacleX, 0xFFFFFFFF);
          queuepoly(mmscale(Vb, geom3::ABODY), shTentacle, (col << 8) + 0xFF);
          ShadowV(Vb, shTentacleX, PPR::GIANTSHADOW);
          */
          bool hexsnake = c->monst == moHexSnake || c->monst == moHexSnakeTail;
          bool thead = c->monst == moTentacle || c->monst == moTentaclewait || c->monst == moTentacleEscaping;
          hpcshape& sh = hexsnake ? shWormSegment : shSmallWormSegment;
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
              transmatrix Vbx = Vb * spin(sin(M_PI * i / 6.) * wav / (i+.1)) * xpush(length * (i) / 12.0);
              // transmatrix Vbx2 = Vnext * xpush(length2 * i / 6.0);
              // Vbx = Vbx * rspintox(inverse(Vbx) * Vbx2 * C0) * pispin;
              ShadowV(Vbx, sh, PPR::GIANTSHADOW);
              queuepoly(mmscale(Vbx, geom3::ABODY), sh, (col0 << 8) + 0xFF);
              }
            });
          }
        }
        
      else {
        int hdir = displayspin(c, c->mondir);
        color_t col = darkena(0x606020, 0, 0xFF);
        for(int u=-1; u<=1; u++)
          queueline(Vparam*xspinpush0(hdir+M_PI/2, u*crossf/5), Vparam*xspinpush(hdir, crossf)*xspinpush0(hdir+M_PI/2, u*crossf/5), col, 2 + vid.linequality);
        }
      }

    if(mmmon) {
      if(isIvy(c) || isMutantIvy(c) || c->monst == moFriendlyIvy) {
        queuepoly(mmscale(Vb, geom3::ABODY), shILeaf[ctof(c)], darkena(col, 0, 0xFF));
        ShadowV(Vb, shILeaf[ctof(c)], PPR::GIANTSHADOW);
        }
      else if(m == moWorm || m == moWormwait || m == moHexSnake) {
        Vb = Vb * pispin;
        transmatrix Vbh = mmscale(Vb, geom3::AHEAD);
        queuepoly(Vbh, shWormHead, darkena(col, 0, 0xFF));
        queuepolyat(Vbh, shEyes, 0xFF, PPR::ONTENTACLE_EYES);
        ShadowV(Vb, shWormHead, PPR::GIANTSHADOW);
        }
      else if(m == moDragonHead) {
        transmatrix Vbh = mmscale(Vb, geom3::AHEAD);
        ShadowV(Vb, shDragonHead, PPR::GIANTSHADOW);
        queuepoly(Vbh, shDragonHead, darkena(col, c->hitpoints?0:1, 0xFF));
        queuepolyat(Vbh/* * pispin */, shEyes, 0xFF, PPR::ONTENTACLE_EYES);
        
        int noscolor = (c->hitpoints == 1 && c->stuntime ==1) ? 0xFF0000FF : 0xFF;
        queuepoly(Vbh, shDragonNostril, noscolor);
        queuepoly(Vbh * Mirror, shDragonNostril, noscolor);
        }
      else if(m == moTentacle || m == moTentaclewait || m == moTentacleEscaping) {
        Vb = Vb * pispin;
        transmatrix Vbh = mmscale(Vb, geom3::AHEAD);
        queuepoly(Vbh, shTentHead, darkena(col, 0, 0xFF));
        ShadowV(Vb, shTentHead, PPR::GIANTSHADOW);
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
            chainAnimation(c, Vb, c2, nd, 0, Vparam, length);
            Vb = Vb * pispin;
            }
          else {
            Vb = Vb0 * ddspin(c, nd, M_PI);
            }
          transmatrix Vbb = mmscale(Vb, geom3::ABODY);
          queuepoly(Vbb, shDragonTail, darkena(col, c->hitpoints?0:1, 0xFF));
          ShadowV(Vb, shDragonTail, PPR::GIANTSHADOW);
          }
        else if(true) {
          if(nospinb) {
            ld length;
            chainAnimation(c, Vb, c2, nd, 0, Vparam, length);
            Vb = Vb * pispin;
            double ang = chainAngle(c, Vb, c->move(c->mondir), displayspin(c, c->mondir) - displayspin(c, nd), Vparam);
            ang /= 2;
            Vb = Vb * spin(M_PI-ang);
            }
          else {
            ld hdir0 = displayspin(c, nd) + M_PI;
            ld hdir1 = displayspin(c, c->mondir);
            while(hdir1 > hdir0 + M_PI) hdir1 -= 2*M_PI;
            while(hdir1 < hdir0 - M_PI) hdir1 += 2*M_PI;
            Vb = Vb0 * spin((hdir0 + hdir1)/2 + M_PI);
            }
          transmatrix Vbb = mmscale(Vb, geom3::ABODY);
          if(part == 'l' || part == '2') {
            queuepoly(Vbb, shDragonLegs, darkena(col, c->hitpoints?0:1, 0xFF));
            }
          queuepoly(Vbb, shDragonWings, darkena(col, c->hitpoints?0:1, 0xFF));
          }
        }
      else {
        if(c->monst == moTentacletail && c->mondir == NODIR) {
          queuepoly(Vb, shWormSegment, darkena(col, 0, 0xFF));
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
            chainAnimation(c, Vb, c2, nd, 0, Vparam, length);
            Vb = Vb * pispin;
            }
          else {
            Vb = Vb0 * ddspin(c, nd, M_PI);
            }
          transmatrix Vbb = mmscale(Vb, geom3::ABODY) * pispin;
          hpcshape& sh = hexsnake ? shWormTail : shSmallWormTail;
          queuepoly(Vbb, sh, darkena(col, 0, 0xFF));
          ShadowV(Vb, sh, PPR::GIANTSHADOW);
          }
        }
      }

    if(!mmmon) return true;
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
      transmatrix Vs = Vparam;
      bool mirr = cw.mirrored;
      transmatrix T = Id;
      nospins = applyAnimation(cwt.at, T, footphase, LAYER_SMALL);
      if(nospins) 
        Vs = Vs * ddspin(c, cw.spin, 0) * iddspin(cwt.at, cwt.spin, 0) * T;
      else
        Vs = Vs * ddspin(c, cw.spin, 0);
      if(mirr) Vs = Vs * Mirror;
      if(inmirrorcount&1) mirr = !mirr;
      col = mirrorcolor(geometry == gElliptic ? det(Vs) < 0 : mirr);
      if(!mouseout() && !nospins) {
        hyperpoint P2 = Vs * inverse(cwtV) * mouseh;
        queuechr(P2, 10, 'x', 0xFF00);
        }     
      if(!nospins && flipplayer) Vs = Vs * pispin;
      if(mmmon) {
        drawMonsterType(moMimic, c, Vs, col, footphase);
        drawPlayerEffects(Vs, c, false);
        }
      }
    return !mmmon;
    }
  
  else if(c->monst && !mmmon) return true;
  
  // illusions face randomly
  
  else if(c->monst == moIllusion) {
    multi::cpid = 0;
    drawMonsterType(c->monst, c, Vs, col, footphase);
    drawPlayerEffects(Vs, c, false);
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
      Vs = Vs * ddspin(c, d);
      }
    return drawMonsterTypeDH(m, c, Vs, col, darkhistory, footphase);
    }

  else if(c->monst == moKrakenT) {
    if(c->hitpoints == 0) col = 0x404040;
    if(nospinb) {
      ld length;
      chainAnimation(c, Vb, c->move(c->mondir), c->mondir, 0, Vparam, length);
      Vb = Vb * pispin;
      Vb = Vb * xpush(tentacle_length - cellgfxdist(c, c->mondir));
      }
    else if(NONSTDVAR) {
      transmatrix T = calc_relative_matrix(c->move(c->mondir), c, c->mondir);
      Vb = Vb * T * rspintox(tC0(inverse(T))) * xpush(tentacle_length);
      }
    else {
      Vb = Vb * ddspin(c, c->mondir, M_PI);
      Vb = Vb * xpush(tentacle_length - cellgfxdist(c, c->mondir));
      }
      
      // if(ctof(c) && !masterless) Vb = Vb * xpush(hexhexdist - hcrossf);
      // return (!BITRUNCATED) ? tessf * gp::scale : (c->type == 6 && (i&1)) ? hexhexdist : crossf;
    return drawMonsterTypeDH(m, c, Vb, col, darkhistory, footphase);
    }

  // golems, knights, and hyperbugs don't face the player (mondir-controlled)
  // also whatever in the lineview mode, and whatever in the quotient geometry

  else if(isFriendly(c) || isBug(c) || (c->monst && conformal::on) || c->monst == moKrakenH || (isBull(c->monst) && c->mondir != NODIR) || c->monst == moButterfly || isMagneticPole(c->monst) ||
    isSwitch(c->monst) || c->monst == moPair || (c->monst && (quotient || euwrap || dont_face_pc))) {
    if(c->monst == moKrakenH) Vs = Vb, nospins = nospinb;
    if(!nospins) Vs = Vs * ddspin(c, c->mondir, M_PI);
    if(c->monst == moPair) Vs = Vs * xpush(-.12);
    if(isFriendly(c)) drawPlayerEffects(Vs, c, false);
    return drawMonsterTypeDH(m, c, Vs, col, darkhistory, footphase);
    }

  else if(c->monst) {
    // other monsters face the player
    
    if(!nospins) {
      hyperpoint V0 = inverse(cwtV) * tC0(Vs);
      hyperpoint V1 = spintox(V0) * V0;

      Vs = cwtV * rspintox(V0) * rpushxto0(V1) * pispin;
      if(c->monst == moHunterChanging)  
        Vs = Vs * spin(M_PI);
      }
    
    if(c->monst == moShadow) 
      multi::cpid = c->hitpoints;
    
    return drawMonsterTypeDH(m, c, Vs, col, darkhistory, footphase);
    }
  
  for(int i=0; i<numplayers(); i++) if(c == playerpos(i) && !shmup::on && mapeditor::drawplayer && 
    !(hardcore && !canmove)) {
    if(!nospins) {
      Vs = playerV;
      if(multi::players > 1 ? multi::flipped[i] : flipplayer) Vs = Vs * pispin;
      }
    else {
      bool mirr = multi::players > 1 ? multi::player[i].mirrored : cwt.mirrored;
      if(mirr) Vs = Vs * Mirror;
      }
    shmup::cpid = i;

    drawPlayerEffects(Vs, c, true);
    if(!mmmon) return true;
    
    if(isWorm(m)) {
      ld depth = geom3::factor_to_lev(wormhead(c) == c ? geom3::AHEAD : geom3::ABODY);
      footphase = 0;
      int q = ptds.size();
      drawMonsterType(moPlayer, c, Vs, col, footphase);
      pushdown(c, q, Vs, -depth, true, false);
      }
    
    else if(mmmon)
      drawMonsterType(moPlayer, c, Vs, col, footphase);
    }

  return false;
#else
  return false;
#endif
  }

double downspin;
cell *straightDownSeek;

#define AURA 180

array<array<int,4>,AURA+1> aurac;

bool haveaura() {
  if(!(vid.aurastr>0 && !svg::in && (auraNOGL || vid.usingGL))) return false;
  if(DIM == 3) return false;
  if(sphere && mdAzimuthalEqui()) return true;
  if(among(pmodel, mdJoukowsky, mdJoukowskyInverted) && hyperbolic && conformal::model_transition < 1) 
    return true;
  return pmodel == mdDisk && (!sphere || vid.alpha > 10) && !euclid;
  }
  
vector<pair<int, int> > auraspecials; 

int auramemo;

void clearaura() { 
  if(!haveaura()) return;
  for(int a=0; a<AURA; a++) for(int b=0; b<4; b++) 
    aurac[a][b] = 0;
  auraspecials.clear();
  auramemo = 128 * 128 / vid.aurastr;
  }

void apply_joukowsky_aura(hyperpoint& h) {
  bool joukowsky = among(pmodel, mdJoukowskyInverted, mdJoukowsky) && hyperbolic && conformal::model_transition < 1;
  if(joukowsky)  {
    hyperpoint ret;
    applymodel(h, ret);
    h = ret;
    }
  }

void addauraspecial(hyperpoint h, color_t col, int dir) {
  if(!haveaura()) return;
  apply_joukowsky_aura(h);
  int r = int(2*AURA + dir + atan2(h[1], h[0]) * AURA / 2 / M_PI) % AURA; 
  auraspecials.emplace_back(r, col);
  }

void addaura(hyperpoint h, color_t col, int fd) {
  if(!haveaura()) return;
  apply_joukowsky_aura(h);

  int r = int(2*AURA + atan2(h[1], h[0]) * AURA / 2 / M_PI) % AURA; 
  aurac[r][3] += auramemo << fd;
  col = darkened(col);
  aurac[r][0] += (col>>16)&255;
  aurac[r][1] += (col>>8)&255;
  aurac[r][2] += (col>>0)&255;
  }
  
void sumaura(int v) {
  int auc[AURA];
  for(int t=0; t<AURA; t++) auc[t] = aurac[t][v];
  int val = 0;
  if(vid.aurasmoothen < 1) vid.aurasmoothen = 1;
  if(vid.aurasmoothen > AURA) vid.aurasmoothen = AURA;
  int SMO = vid.aurasmoothen;
  for(int t=0; t<SMO; t++) val += auc[t];
  for(int t=0; t<AURA; t++) {
    int tt = (t + SMO/2) % AURA;
    aurac[tt][v] = val;
    val -= auc[t];
    val += auc[(t+SMO) % AURA];
    }  
  aurac[AURA][v] = aurac[0][v];
  }

vector<glhr::colored_vertex> auravertices;

void drawaura() {
  if(!haveaura()) return;
  if(vid.stereo_mode) return;
  double rad = current_display->radius;
  if(sphere && !mdAzimuthalEqui()) rad /= sqrt(vid.alpha*vid.alpha - 1);
  
  for(int v=0; v<4; v++) sumaura(v);
  for(auto& p: auraspecials) {
    int r = p.first;
    aurac[r][3] = auramemo;
    for(int k=0; k<3; k++) aurac[r][k] = (p.second >> (16-8*k)) & 255;
    }

#if CAP_SDL || CAP_GL
  double bak[3];
  bak[0] = ((backcolor>>16)&255)/255.;
  bak[1] = ((backcolor>>8)&255)/255.;
  bak[2] = ((backcolor>>0)&255)/255.;
#endif
  
#if CAP_SDL
  if(!vid.usingGL) {
    SDL_LockSurface(s);
    for(int y=0; y<vid.yres; y++)
    for(int x=0; x<vid.xres; x++) {

      ld hx = (x * 1. - current_display->xcenter) / rad;
      ld hy = (y * 1. - current_display->ycenter) / rad / vid.stretch;
  
      if(vid.camera_angle) camrotate(hx, hy);
      
      ld fac = sqrt(hx*hx+hy*hy);
      if(fac < 1) continue;
      ld dd = log((fac - .99999) / .00001);
      ld cmul = 1 - dd/10.;
      if(cmul>1) cmul=1;
      if(cmul<0) cmul=0;
      
      ld alpha = AURA * atan2(hx,hy) / (2 * M_PI);
      if(alpha<0) alpha += AURA;
      if(alpha >= AURA) alpha -= AURA;
      
      int rm = int(alpha);
      double fr = alpha-rm;
      
      if(rm<0 || rm >= AURA) continue;
      
      color_t& p = qpixel(s, x, y);
      for(int c=0; c<3; c++) {
        double c1 = aurac[rm][2-c] / (aurac[rm][3]+.1);
        double c2 = aurac[rm+1][2-c] / (aurac[rm+1][3]+.1);
        const ld one = 1;
        part(p, c) = int(255 * min(one, bak[2-c] + cmul * ((c1 + fr * (c2-c1) - bak[2-c])))); 
        }
      }
    SDL_UnlockSurface(s);
    return;
    }
#endif

#if CAP_GL
  setcameraangle(true);
  
  float cx[AURA+1][11][5];

  double facs[11] = {1, 1.01, 1.02, 1.04, 1.08, 1.70, 1.95, 1.5, 2, 6, 10};
  double cmul[11] = {1,   .8,  .7,  .6,  .5,  .16,  .12,  .08,  .07,  .06, 0};
  double d2[11] = {0, 2, 4, 6.5, 7, 7.5, 8, 8.5, 9, 9.5, 10};

  for(int d=0; d<11; d++) {
    double dd = d2[d];
    cmul[d] = (1- dd/10.);
    facs[d] = .99999 +  .00001 * exp(dd);
    }
  facs[10] = 10;
  cmul[1] = cmul[0];
  
  bool inversion = vid.alpha <= -1 || pmodel == mdJoukowsky;
  bool joukowsky = among(pmodel, mdJoukowskyInverted, mdJoukowsky) && hyperbolic && conformal::model_transition < 1;

  for(int r=0; r<=AURA; r++) for(int z=0; z<11; z++) {
    float rr = (M_PI * 2 * r) / AURA;
    float rad0 = inversion ? rad / facs[z] : rad * facs[z];
    int rm = r % AURA;
    ld c = cos(rr);
    ld s = sin(rr);

    if(joukowsky) {
      ld c1 = c, s1 = s;
      if(inversion)
        conformal::apply_orientation(s1, c1);
      else        
        conformal::apply_orientation(c1, s1);

      ld& mt = conformal::model_transition;
      ld mt2 = 1 - mt;

      ld m = sqrt(c1*c1 + s1*s1 / mt2 / mt2);
      m *= 2;
      if(inversion) rad0 /= m;
      else rad0 *= m;
      }

    cx[r][z][0] = rad0 * c;
    cx[r][z][1] = rad0 * s * vid.stretch;
    
    for(int u=0; u<3; u++)
      cx[r][z][u+2] = bak[u] + (aurac[rm][u] / (aurac[rm][3]+.1) - bak[u]) * cmul[z];
    }
  
  auravertices.clear();
  for(int r=0; r<AURA; r++) for(int z=0;z<10;z++) {
    for(int c=0; c<6; c++) {
      int br = (c == 1 || c == 3 || c == 5) ? r+1 : r;
      int bz = (c == 2 || c == 4 || c == 5) ? z+1 : z;
      auravertices.emplace_back(
        cx[br][bz][0], cx[br][bz][1], cx[br][bz][2], cx[br][bz][3], cx[br][bz][4]
        );
      }
    }
  glflush();
  current_display->set_projection(0, false);
  setcameraangle(true);
  glhr::switch_mode(glhr::gmVarColored, glhr::shader_projection::standard);
  glhr::id_modelview();
  glhr::prepare(auravertices);
  glhr::set_depthtest(false);
  glDrawArrays(GL_TRIANGLES, 0, isize(auravertices));


  setcameraangle(false);
#endif
  }

// int fnt[100][7];

bool bugsNearby(cell *c, int dist = 2) {
  if(!(havewhat&HF_BUG)) return false;
  if(isBug(c)) return true;
  if(dist) for(int t=0; t<c->type; t++) if(c->move(t) && bugsNearby(c->move(t), dist-1)) return true;
  return false;
  }

colortable minecolors = {
  0xFFFFFF, 0xF0, 0xF060, 0xF00000, 
  0x60, 0x600000, 0x00C0C0, 0x000000
  };

colortable distcolors = {
  0xFFFFFF, 0xF0, 0xF060, 0xF00000, 
  0xA0A000, 0xA000A0, 0x00A0A0, 0xFFD500
  };

const char* minetexts[8] = {
  "No mines next to you.",
  "A mine is next to you!",
  "Two mines next to you!",
  "Three mines next to you!",
  "Four mines next to you!",
  "Five mines next to you!",
  "Six mines next to you!",
  "Seven mines next to you!"
  };

int countMinesAround(cell *c) {
  int mines = 0;
  for(int i=0; i<c->type; i++)
    if(c->move(i) && c->move(i)->wall == waMineMine)
      mines++;
  return mines;
  }

transmatrix applyPatterndir(cell *c, const patterns::patterninfo& si) {
  if(NONSTDVAR || binarytiling) return Id;
  transmatrix V = ddspin(c, si.dir, M_PI);
  if(si.reflect) V = V * Mirror;
  return V * iddspin(c, 0, M_PI);
  }

transmatrix applyDowndir(cell *c, const cellfunction& cf) {
  return ddspin(c, patterns::downdir(c, cf), M_PI);
  }

#if CAP_SHAPES
void set_towerfloor(cell *c, const cellfunction& cf = coastvalEdge) {
  if(weirdhyperbolic || sphere) {
    set_floor(shFloor);
    return;
    }
  int j = -1;

  if(masterless) j = 10;
  else if(cf(c) > 1) { 
    int i = towerval(c, cf);
    if(i == 4) j = 0;
    if(i == 5) j = 1;
    if(i == 6) j = 2;
    if(i == 8) j = 3;
    if(i == 9) j = 4;
    if(i == 10) j = 5;
    if(i == 13) j = 6;
    if(PURE) {
      if(i == 7) j = 7;
      if(i == 11) j = 8;
      if(i == 15) j = 9;
      }
    }

  if(j >= 0)
    set_floor(applyDowndir(c, cf), shTower[j]);
  else if(c->wall != waLadder)
    set_floor(shMFloor);
  }

void set_zebrafloor(cell *c) {

  if(masterless) { set_floor(shTower[10]); return; }
  if(weirdhyperbolic) {
    set_floor(shFloor); return;
    }
  
  auto si = patterns::getpatterninfo(c, patterns::PAT_ZEBRA, patterns::SPF_SYM0123);
  
  int j;
  if(PURE) j = 4;
  else if(si.id >=4 && si.id < 16) j = 2;
  else if(si.id >= 16 && si.id < 28) j = 1;
  else if(si.id >= 28 && si.id < 40) j = 3;
  else j = 0;

  set_floor(applyPatterndir(c, si), shZebra[j]);
  }

void set_maywarp_floor(cell *c);

int chasmg;

void set_reptile_floor(cell *c, const transmatrix& V, color_t col, bool nodetails = false) {

  auto si = 
    euclid6 ? 
      patterns::getpatterninfo(c, patterns::PAT_COLORING, patterns::SPF_CHANGEROT)
    :
      patterns::getpatterninfo(c, patterns::PAT_ZEBRA, patterns::SPF_SYM0123);
  
  int j;

  if(!wmescher) j = 4;
  else if(!BITRUNCATED) j = 0;
  else if(si.id < 4) j = 0;
  else if(si.id >=4 && si.id < 16) j = 1;
  else if(si.id >= 16 && si.id < 28) j = 2;
  else if(si.id >= 28 && si.id < 40) j = 3;
  else j = 4;
  
  if(euclid6) j = 0;

  transmatrix D = applyPatterndir(c, si);
  
  if(wmescher)
    set_floor(D, shReptile[j][0]);
  else set_maywarp_floor(c);

  if(nodetails) return;

  int dcol = 0;

  int ecol = -1;

  if(isReptile(c->wall)) {
    unsigned char wp = c->wparam;
    if(wp == 1)        
      ecol = 0xFFFF00;
    else if(wp <= 5)
      ecol = 0xFF0000;
    else
      ecol = 0;
    if(ecol) ecol = gradient(0, ecol, -1, sintick(30), 1);
    }
  
  if(ecol == -1 || ecol == 0) dcol = darkena(col, 1, 0xFF);
  else dcol = darkena(ecol, 0, 0x80);

  dynamicval<color_t> p(poly_outline, 
    doHighlight() && ecol != -1 && ecol != 0 ? OUTLINE_ENEMY : OUTLINE_DEFAULT);

  if(!chasmg) {
    if(wmescher)
      queuepoly(V*D, shReptile[j][1], dcol);
    else
      draw_floorshape(c, V, shMFloor, dcol);
    }
  
  if(ecol != -1) {
    queuepoly(V*D, shReptile[j][2], (ecol << 8) + 0xFF);
    queuepoly(V*D, shReptile[j][3], (ecol << 8) + 0xFF);
    }
  }

void draw_reptile(cell *c, const transmatrix &V, color_t col) {
  auto qfib = qfi;
  set_reptile_floor(c, V, col, chasmg == 2);
  draw_qfi(c, V, col);
  qfi = qfib;
  }      

void set_emeraldfloor(cell *c) {
  if(!masterless && BITRUNCATED) {
    auto si = patterns::getpatterninfo(c, patterns::PAT_EMERALD, patterns::SPF_SYM0123);
  
    int j = -1;

    if(si.id == 8) j = 0;
    else if(si.id == 12) j = 1;
    else if(si.id == 16) j = 2;
    else if(si.id == 20) j = 3;
    else if(si.id == 28) j = 4;
    else if(si.id == 36) j = 5;

    if(j >= 0) {
      set_floor(applyPatterndir(c, si), shEmeraldFloor[j]);
      return;
      }
    }
  
  set_floor(shCaveFloor);
  }

void viewBuggyCells(cell *c, transmatrix V) {
  for(int i=0; i<isize(buggycells); i++)
    if(c == buggycells[i]) {
      queuepoly(V, shPirateX, 0xC000C080);
      return;
      }

  for(int i=0; i<isize(buggycells); i++) {
    cell *c1 = buggycells[i];
    cell *cf = cwt.at;
    
    while(cf != c1) {
      cf = pathTowards(cf, c1);
      if(cf == c) {
        queuepoly(V, shMineMark[1], 0xC000C0D0);
        return;
        }
      }
    }
  }

void drawMovementArrows(cell *c, transmatrix V) {

  if(viewdists) return;

  for(int d=0; d<8; d++) {
  
    movedir md = vectodir(spin(-d * M_PI/4) * tC0(pushone()));
    int u = md.d;
    cellwalker xc = cwt + u + wstep;
    if(xc.at == c) {
      transmatrix fixrot = sphereflip * rgpushxto0(sphereflip * tC0(V));
      // make it more transparent
      color_t col = getcs().uicolor;
      col -= (col & 0xFF) >> 1;
      poly_outline = OUTLINE_DEFAULT;
      queuepoly(fixrot * spin(-d * M_PI/4), shArrow, col);

      if((c->type & 1) && (isStunnable(c->monst) || isPushable(c->wall))) {
        transmatrix Centered = rgpushxto0(tC0(cwtV));
        int sd = md.subdir;
        queuepoly(inverse(Centered) * rgpushxto0(Centered * tC0(V)) * rspintox(Centered*tC0(V)) * spin(-sd * M_PI/S7) * xpush(0.2), shArrow, col);
        }
      else if(!confusingGeometry()) break;
      }
    }
  }
#endif

int celldistAltPlus(cell *c) { return 1000000 + celldistAlt(c); }

bool drawstaratvec(double dx, double dy) {
  return dx*dx+dy*dy > .05;
  }

int reptilecolor(cell *c) {
  int i;
  
  if(archimedean)
    i = c->master->rval0 & 3;
  else {
    i = zebra40(c);
    
    if(!masterless) {
      if(i >= 4 && i < 16) i = 0; 
      else if(i >= 16 && i < 28) i = 1;
      else if(i >= 28 && i < 40) i = 2;
      else i = 3;
      }
    }

  color_t fcoltab[4] = {0xe3bb97, 0xc2d1b0, 0xebe5cb, 0xA0A0A0};
  return fcoltab[i];
  }

ld wavefun(ld x) { 
  return sin(x);
  /* x /= (2*M_PI);
  x -= (int) x;
  if(x > .5) return (x-.5) * 2;
  else return 0; */
  }

colortable nestcolors = { 0x800000, 0x008000, 0x000080, 0x404040, 0x700070, 0x007070, 0x707000, 0x606060 };

color_t floorcolors[landtypes];

void init_floorcolors() {
  for(int i=0; i<landtypes; i++)
    floorcolors[i] = linf[i].color;

  floorcolors[laDesert] = 0xEDC9AF;
  floorcolors[laKraken] = 0x20A020;
  floorcolors[laDocks] = 0x202020;
  floorcolors[laCA] = 0x404040;
  floorcolors[laMotion] = 0xF0F000;
  floorcolors[laGraveyard] = 0x107010;
  floorcolors[laWineyard] = 0x006000;
  floorcolors[laLivefjord] = 0x306030;
    
  floorcolors[laMinefield] = 0x80A080; 
  floorcolors[laCaribbean] = 0x006000;

  floorcolors[laAlchemist] = 0x202020;

  floorcolors[laRlyeh] = 0x004080;
  floorcolors[laHell] = 0xC00000;
  floorcolors[laCrossroads] = 0xFF0000;
  floorcolors[laJungle] = 0x008000;

  floorcolors[laZebra] = 0xE0E0E0;

  floorcolors[laCaves] = 0x202020;
  floorcolors[laEmerald] = 0x202020;
  floorcolors[laDeadCaves] = 0x202020;

  floorcolors[laPalace] = 0x806020;
  
  floorcolors[laHunting] = 0x40E0D0 / 2;

  floorcolors[laBlizzard] = 0x5050C0;
  floorcolors[laCocytus] = 0x80C0FF;
  floorcolors[laIce] = 0x8080FF;
  floorcolors[laCamelot] = 0xA0A0A0;

  floorcolors[laOvergrown] = 0x00C020;
  floorcolors[laClearing] = 0x60E080;
  floorcolors[laHaunted] = 0x609F60;

  floorcolors[laMirror] = floorcolors[laMirrorWall] = floorcolors[laMirrorOld] = 0x808080;
  }

void setcolors(cell *c, color_t& wcol, color_t& fcol) {

  wcol = fcol = winf[c->wall].color;

  if(c->wall == waMineMine)
    wcol = fcol = winf[waMineUnknown].color;

  // water colors
  if(isWateryOrBoat(c) || c->wall == waReptileBridge) {
    if(c->land == laOcean)
      fcol = 
        #if CAP_FIELD
        (c->landparam > 25 && !chaosmode) ? ( 
          0x90 + 8 * sintick(1000, windmap::windcodes[windmap::getId(c)] / 256.)
          ) : 
        #endif
        0x1010C0 + int(32 * sintick(500, (chaosmode ? c->CHAOSPARAM : c->landparam)*.75/M_PI));
    else if(c->land == laOceanWall)
      fcol = 0x2020FF;
    else if(c->land == laVariant)
      fcol = 0x002090 + 15 * sintick(300, 0);
    else if(c->land == laKraken) {
      fcol = 0x0000A0;
      int mafcol = (kraken_pseudohept(c) ? 64 : 8);
      /* bool nearshore = false;
      for(int i=0; i<c->type; i++) 
        if(c->move(i)->wall != waSea && c->move(i)->wall != waBoat)
          nearshore = true;
      if(nearshore) mafcol += 30; */
      fcol = fcol + mafcol * (4+sintick(500, ((eubinary||c->master->alt) ? celldistAlt(c) : 0)*.75/M_PI))/5;
      }
    else if(c->land == laDocks) {
      fcol = 0x0000A0;
      }
    else if(c->land == laAlchemist)
      fcol = 0x900090;
    else if(c->land == laWhirlpool)
      fcol = 0x0000C0 + int(32 * sintick(200, ((eubinary||c->master->alt) ? celldistAlt(c) : 0)*.75/M_PI));
    else if(c->land == laLivefjord)
      fcol = 0x000080;
    else if(isWarped(c->land))
      fcol = 0x0000C0 + int((warptype(c)?30:-30) * sintick(600));
    else
      fcol = wcol;
    }

  // floor colors for all the lands
  else switch(c->land) {
    case laBurial: case laTrollheim: case laBarrier: case laOceanWall:
    case laCrossroads2: case laCrossroads3: case laCrossroads4: case laCrossroads5:
    case laRose: case laPower: case laWildWest: case laHalloween: case laRedRock:
    case laDragon: case laStorms: case laTerracotta: case laMercuryRiver:
    case laDesert: case laKraken: case laDocks: case laCA:
    case laMotion: case laGraveyard: case laWineyard: case laLivefjord: 
    case laRlyeh: case laHell: case laCrossroads: case laJungle:
    case laAlchemist: 
      fcol = floorcolors[c->land]; break;
    
    #if CAP_COMPLEX2
    case laVariant: {
      int b = getBits(c);
      fcol = 0x404040;
      for(int a=0; a<21; a++)
        if((b >> a) & 1)
          fcol += variant_features[a].color_change;
      if(c->wall == waAncientGrave)
        wcol = 0x080808;
      else if(c->wall == waFreshGrave)
        wcol = 0x202020;
      break;
      }
    #endif
    
    case laRuins:
      fcol = pseudohept(c) ? 0xC0E0C0 : 0x40A040;
      break;
    
    case laDual:
      fcol = floorcolors[c->land];
      if(c->landparam == 2) fcol = 0x40FF00;
      if(c->landparam == 3) fcol = 0xC0FF00;
      break;

#if CAP_COMPLEX2
    case laBrownian: {
      using brownian::level;
      fcol = wcol = 
        /*
        c->landparam == 0 ? 0x0000F0 : 
        c->landparam < level ? gradient(0x002000, 0xFFFFFF, 1, c->landparam, level-1) :
        c->landparam < 2 * level ? 0xFFFF80 :
        c->landparam < 3 * level ? 0xFF8000 :
        0xC00000; */
       
        c->landparam == 0 ? 0x0000F0 : brownian::get_color(c->landparam);
      break;
      }
#endif
      
#if CAP_FIELD
    case laVolcano: {
      int id = lavatide(c, -1)/4;
      if(c->wall == waMagma) {
        if(id == 95/4-1) fcol = wcol = 0x200000;
        else if(id == 95/4) fcol = wcol = 0x100000;
        else if(id < 48/4) fcol = wcol = gradient(0xF0F000, 0xF00000, 0, id, 48/4);
        else if(id < 96/4) fcol = wcol = gradient(0xF00000, 0x400000, 48/4, id, 95/4-2);
        }
      else if(c->wall == waNone) {
        fcol = wcol = 0x404040;
        if(id == 255/4) fcol = 0xA0A040;
        if(id == 255/4-1) fcol = 0x606040;
        }
      /* {
        if(id/4 == 255/4) fd = 0;
        if(id/4 == 95/4-1 || id/4 == 255/4-1) fd = 1;
        } */
      break;
      }
#endif

    case laMinefield: 
      fcol = floorcolors[c->land];
      if(c->wall == waMineMine && ((cmode & sm::MAP) || !canmove))
        fcol = wcol = 0xFF4040;
      break;
      
    case laCaribbean: 
      if(c->wall != waCIsland && c->wall != waCIsland2)
        fcol = floorcolors[c->land];
      break;

    case laReptile:
      fcol = reptilecolor(c);
      break;

    case laCaves: case laEmerald: case laDeadCaves:
      fcol = 0x202020;
      if(c->land == laEmerald) 
      if(c->wall == waCavefloor || c->wall == waCavewall) {
        fcol = wcol = gradient(winf[waCavefloor].color, 0xFF00, 0, 0.5, 1);
        if(c->wall == waCavewall) wcol = 0xC0FFC0;
        }
      break;
    case laMirror: case laMirrorWall: case laMirrorOld:
      if(c->land == laMirrorWall) fcol = floorcolors[laMirror];
      else fcol = floorcolors[c->land];
      break;
    case laDryForest:
      fcol = gradient(0x008000, 0x800000, 0, c->landparam, 10);
      break;    
    case laMountain:
      if(eubinary || sphere || c->master->alt)
        fcol = celldistAlt(c) & 1 ? 0x604020 : 0x302010;
      else fcol = 0;
      if(c->wall == waPlatform) wcol = 0xF0F0A0;
      break;
    case laCanvas:
      fcol = c->landparam;
      break;
    case laPalace:
      fcol = floorcolors[c->land];
      if(c->wall == waClosedGate || c->wall == waOpenGate)
        fcol = wcol;
      break;
    case laElementalWall:
      fcol = (linf[c->barleft].color>>1) + (linf[c->barright].color>>1);
      break;
    case laZebra:
      fcol = floorcolors[c->land];
      if(c->wall == waTrapdoor) fcol = 0x808080;
      break;
    case laHive:
      fcol = linf[c->land].color;
      if(c->wall == waWaxWall) wcol = c->landparam;
      if(items[itOrbInvis] && c->wall == waNone && c->landparam)
        fcol = gradient(fcol, 0xFF0000, 0, c->landparam, 100);
      if(c->bardir == NOBARRIERS && c->barleft) 
        fcol = minf[moBug0+c->barright].color;
      break;
    case laSwitch:
      fcol = minf[passive_switch].color;
      break;
    case laTortoise:
      fcol = tortoise::getMatchColor(getBits(c));
      if(c->wall == waBigTree) wcol = 0x709000;
      else if(c->wall == waSmallTree) wcol = 0x905000;
      break;
    case laOvergrown: case laClearing:
      fcol = floorcolors[c->land];
      if(c->wall == waSmallTree) wcol = 0x008060;
      else if(c->wall == waBigTree) wcol = 0x0080C0;
      break;
    case laTemple: {
      int d = showoff ? 0 : (eubinary||c->master->alt) ? celldistAlt(c) : 99;
      if(chaosmode)
        fcol = 0x405090;
      else if(d % TEMPLE_EACH == 0)
        fcol = gradient(0x304080, winf[waColumn].color, 0, 0.5, 1);
  //    else if(c->type == 7)
  //      wcol = 0x707070;
      else if(d% 2 == -1)
        fcol = 0x304080;
      else
        fcol = 0x405090;
      break;
      }

    case laWhirlwind:
      if(c->land == laWhirlwind) {
        color_t wcol[4] = {0x404040, 0x404080, 0x2050A0, 0x5050C0};
        fcol = wcol[whirlwind::fzebra3(c)];
        }
      break;

    case laHunting:
      fcol = floorcolors[c->land];
      if(pseudohept(c)) fcol = fcol * 3/4;
      break;
  
    case laIvoryTower:
      fcol = 0x10101 * (32 + (c->landparam&1) * 32) - 0x000010;
      break;
    
    case laWestWall:
      fcol = 0x10101 * ((c->landparam&1) * 32) + floorcolors[c->land];
      break;
    
    case laDungeon: {
      int lp = c->landparam % 5;
        // xcol = (c->landparam&1) ? 0xD00000 : 0x00D000;
      int lps[5] = { 0x402000, 0x302000, 0x202000, 0x282000, 0x382000 };
      fcol = lps[lp];
      if(c->wall == waClosedGate)
        fcol = wcol = 0xC0C0C0;
      if(c->wall == waOpenGate)
        fcol = wcol = 0x404040;
      if(c->wall == waPlatform)
        fcol = wcol = 0xDFB520;
      break;
      }
    
    case laEndorian: {
      int clev = pd_from->land == laEndorian ? edgeDepth(pd_from) : 0;
        // xcol = (c->landparam&1) ? 0xD00000 : 0x00D000;
      fcol = 0x10101 * (32 + (c->landparam&1) * 32) - 0x000010;
      int ed = edgeDepth(c);
      int sr = get_sightrange_ambush();
      
      if(clev == UNKNOWN || ed == UNKNOWN)
        fcol = 0x0000D0;
      else {
        while(ed > clev + sr) ed -= 2;
        while(ed < clev - sr) ed += 2;
        fcol = gradient(fcol, 0x0000D0, clev-sr, edgeDepth(c), clev+sr);
        }
      if(c->wall == waTrunk) fcol = winf[waTrunk].color;
  
      if(c->wall == waCanopy || c->wall == waSolidBranch || c->wall == waWeakBranch) {
        fcol = winf[waCanopy].color;
        if(c->landparam & 1) fcol = gradient(0, fcol, 0, .75, 1);
        }
      break;
      }
    
    #if CAP_FIELD
    case laPrairie:
      if(prairie::isriver(c)) {
        fcol = ((c->LHU.fi.rval & 1) ? 0x402000: 0x503000);
        }
      else {
        fcol = 0x004000 + 0x001000 * c->LHU.fi.walldist;
        fcol += 0x10000 * (255 - 511 / (1 + max((int) c->LHU.fi.flowerdist, 1)));
        // fcol += 0x1 * (511 / (1 + max((int) c->LHU.fi.walldist2, 1)));
        }
      break;
    #endif
  
    case laCamelot: {
      int d = showoff ? 0 : ((eubinary||c->master->alt) ? celldistAltRelative(c) : 0);
  #if CAP_TOUR
      if(!tour::on) camelotcheat = false;
      if(camelotcheat) 
          fcol = (d&1) ? 0xC0C0C0 : 0x606060;
      else 
  #endif
      if(d < 0) {
        fcol = floorcolors[c->land];
        }
      else {
        // a nice floor pattern
        int v = emeraldval(c);
        int v0 = (v&~3);
        bool sw = (v&1);
        if(v0 == 8 || v0 == 12 || v0 == 20 || v0 == 40 || v0 == 36 || v0 == 24)
          sw = !sw;
        if(sw)
          fcol = 0xC0C0C0;
        else
          fcol = 0xA0A0A0;
        }
      break;
      }

    case laIce: case laCocytus: case laBlizzard:
      if(isIcyWall(c)) {
        float h = HEAT(c);
        eLand l = c->land;
        
        color_t colorN04 = l == laCocytus ? 0x4080FF : 0x4040FF;
        color_t colorN10 = 0x0000FF;
        color_t color0 = floorcolors[c->land];
        color_t color02 = 0xFFFFFF;
        color_t color06 = 0xFF0000;
        color_t color08 = 0xFFFF00;
        
        if(h < -1)
          wcol = colorN10;
        else if(h < -0.4)
          wcol = gradient(colorN04, colorN10 , -0.4, h, -1);
        else if(h < 0)
          wcol = gradient(color0, colorN04, 0, h, -0.4);
        else if(h < 0.2)
          wcol = gradient(color0, color02, 0, h, 0.2);
        // else if(h < 0.4)
        //  wcol = gradient(0xFFFFFF, 0xFFFF00, 0.2, h, 0.4);
        else if(h < 0.6)
          wcol = gradient(color02, color06, 0.2, h, 0.6);
        else if(h < 0.8)
          wcol = gradient(color06, color08, 0.6, h, 0.8);
        else
          wcol = color08;
        if(c->wall == waFrozenLake) 
          fcol = wcol;
        else
          fcol = (wcol & 0xFEFEFE) >> 1;
        if(c->wall == waLake)
          fcol = wcol = (wcol & 0xFCFCFC) >> 2;
        }
      break;
    
    case laOcean:
      if(chaosmode)
        fcol = gradient(0xD0A090, 0xD0D020, 0, c->CHAOSPARAM, 30);
      else
        fcol = gradient(0xD0D090, 0xD0D020, -1, sin((double) c->landparam), 1);
      break;
    
    case laSnakeNest: {
      int fv = pattern_threecolor(c);
      fcol = nestcolors[fv&7];
      if(realred(c->wall))
        wcol = fcol * (4 + snakelevel(c)) / 4;
      break;
      }
        
    default:
      if(isElemental(c->land)) fcol = linf[c->land].color;
      if(isWarped(c->land)) {
        fcol = warptype(c) ? 0x80C080 : 0xA06020;
        if(c->wall == waSmallTree) wcol = 0x608000;
        }  
      if(isHaunted(c->land)) {
        int itcolor = 0;
        for(int i=0; i<c->type; i++) if(c->move(i) && c->move(i)->item)
          itcolor = 1;
        if(c->item) itcolor |= 2;
        fcol = floorcolors[laHaunted] + 0x202020 * itcolor;
    
        forCellEx(c2, c) if(c2->monst == moFriendlyGhost)
          fcol = gradient(fcol, fghostcolor(c2), 0, .25, 1);
    
        if(c->monst == moFriendlyGhost) 
          fcol = gradient(fcol, fghostcolor(c), 0, .5, 1);
    
        if(c->wall == waSmallTree) wcol = 0x004000;
        else if(c->wall == waBigTree) wcol = 0x008000;
        }
    }
  
   /* if(c->land == laCaribbean && (c->wall == waCIsland || c->wall == waCIsland2))
     fcol = wcol = winf[c->wall].color; */
   
  switch(c->wall) {
    case waSulphur: case waSulphurC: case waPlatform: case waMercury: case waDock:
    case waAncientGrave: case waFreshGrave: case waThumperOn: case waThumperOff: case waBonfireOff:
    case waRoundTable: case waExplosiveBarrel:
      // floors become fcol
      fcol = wcol;
      break;
    
    case waDeadTroll2: case waPetrifiedBridge: case waPetrified: {
      eMonster m = eMonster(c->wparam);
      if(c->wall == waPetrified || c->wall == waPetrifiedBridge) 
        wcol = gradient(wcol, minf[m].color, 0, .2, 1);
      if(c->wall == waPetrified || isTroll(m)) if(!(m == moForestTroll && c->land == laOvergrown))
        wcol = gradient(wcol, minf[m].color, 0, .4, 1);
      break;
      }
    
    case waFloorA: case waFloorB: // isAlch
      if(c->item && !(conformal::includeHistory && conformal::infindhistory.count(c)))
        fcol = wcol = iinf[c->item].color;
      else
        fcol = wcol;
      break;
    
    case waBoat:
      if(wmascii) wcol = 0xC06000;
      break;
    
    case waEternalFire:
      fcol = wcol = weakfirecolor(1500);
      break;
    
    case waFire: case waPartialFire: case waBurningDock:
      fcol = wcol = firecolor(100);
      break;
    
    case waDeadfloor: case waCavefloor:
      fcol = wcol;
      break;
    
    case waNone:
      if(c->land == laNone)
        wcol = fcol = 0x101010;
      if(c->land == laHive) 
        wcol = fcol;  
      break;

    case waMineUnknown: case waMineMine: 
      if(mineMarkedSafe(c))
        fcol = wcol = gradient(wcol, 0x40FF40, 0, 0.2, 1);
      else if(mineMarked(c))
        fcol = wcol = gradient(wcol, 0xFF4040, -1, sintick(100), 1);
      // fallthrough

    case waMineOpen:
      fcol = wcol;
      if(wmblack || wmascii) fcol >>= 1, wcol >>= 1;
      break;
    
    case waCavewall:
      if(c->land != laEmerald) fcol = winf[waCavefloor].color;
      break;
  
    default:
      break;    
    }

  /* if(false && isAlch2(c, true)) {
    int id = lavatide(c, -1);
    if(id < 96)
      wcol = gradient(0x800000, 0xFF0000, 0, id, 96);
    else 
      wcol = gradient(0x00FF00, 0xFFFF00, 96, id, 255);
    fcol = wcol;
    } */
  
  int rd = rosedist(c);
  if(rd == 1) 
    wcol = gradient(0x804060, wcol, 0,1,3),
    fcol = gradient(0x804060, fcol, 0,1,3);
  if(rd == 2) 
    wcol = gradient(0x804060, wcol, 0,2,3),
    fcol = gradient(0x804060, fcol, 0,2,3);
  
  if(items[itRevolver] && !shmup::on) {
    bool inrange = c->mpdist <= GUNRANGE;
    if(inrange) {
      inrange = false;
      for(int i=0; i<numplayers(); i++) for(cell *c1: gun_targets(playerpos(i))) if(c1 == c) inrange = true;
      }
    if(!inrange)
      fcol = gradient(fcol, 0, 0, 25, 100),
      wcol = gradient(wcol, 0, 0, 25, 100);
    }
    
  if(highwall(c) && !wmspatial)
    fcol = wcol;
  
  if(wmascii && (c->wall == waNone || isWatery(c))) wcol = fcol;
  
  if(!wmspatial && snakelevel(c) && !realred(c->wall)) fcol = wcol;
  
  if(c->wall == waGlass && !wmspatial) fcol = wcol;  
  }

bool noAdjacentChasms(cell *c) {
  forCellEx(c2, c) if(c2->wall == waChasm) return false;
  return true;
  }

// does the current geometry allow nice duals
bool has_nice_dual() {
  #if CAP_IRR
  if(IRREGULAR) return irr::bitruncations_performed > 0;
  #endif
  #if CAP_ARCM
  if(archimedean) return geosupport_football() >= 2;
  #endif
  if(binarytiling) return false;
  if(BITRUNCATED) return true;
  if(a4) return false;
  if((S7 & 1) == 0) return true;
  if(PURE) return false;
  #if CAP_GP
  return (gp::param.first + gp::param.second * 2) % 3 == 0;
  #else
  return false;
  #endif
  }

// does the current geometry allow nice duals
bool is_nice_dual(cell *c) {
  return c->land == laDual && has_nice_dual();
  }

bool use_swapped_duals() {
  return (masterless && !a4) || GOLDBERG;
  }

int wavephase;

#if CAP_SHAPES
void floorShadow(cell *c, const transmatrix& V, color_t col) {
  if(model_needs_depth() || noshadow) 
    return; // shadows break the depth testing
  dynamicval<color_t> p(poly_outline, OUTLINE_TRANS);
  if(qfi.shape) {
    queuepolyat(V * qfi.spin * shadowmulmatrix, *qfi.shape, col, PPR::WALLSHADOW);
    }
  else if(qfi.usershape >= 0)
    mapeditor::drawUserShape(V * qfi.spin * shadowmulmatrix, mapeditor::sgFloor, qfi.usershape, col, c, PPR::WALLSHADOW);
  else 
    draw_shapevec(c, V, qfi.fshape->shadow, col, PPR::WALLSHADOW);
  }

void set_maywarp_floor(cell *c) {
  bool warp = isWarped(c);
  if(warp && !shmup::on && geosupport_football() == 2) {
    if(!stdhyperbolic) {
      set_floor(shTriheptaFloor);
      return;
      }
    auto si = patterns::getpatterninfo(c, patterns::PAT_TYPES, 0);
    if(si.id == 0 || si.id == 1)
      set_floor(shTriheptaFloor);
    else if(si.id >= 14)
      set_floor(shFloor);
    else
      set_floor(applyPatterndir(c, si), shTriheptaSpecial[si.id]);
    }
  else if(is_nice_dual(c)) set_floor(shBigTriangle);
  else set_floor(shFloor);
  }

void escherSidewall(cell *c, int sidepar, const transmatrix& V, color_t col) {
  if(sidepar >= SIDE_SLEV && sidepar <= SIDE_SLEV+2) {
    int sl = sidepar - SIDE_SLEV;
    for(int z=1; z<=4; z++) if(z == 1 || (z == 4 && detaillevel == 2))
      draw_qfi(c, mscale(V, zgrad0(geom3::slev * sl, geom3::slev * (sl+1), z, 4)), col, PPR::REDWALL-4+z+4*sl);
    }
  else if(sidepar == SIDE_WALL) {
    const int layers = 2 << detaillevel;
    for(int z=1; z<layers; z++) 
      draw_qfi(c, mscale(V, zgrad0(0, geom3::actual_wall_height(), z, layers)), col, PPR::WALL3+z-layers);
    }
  else if(sidepar == SIDE_LAKE) {
    const int layers = 1 << (detaillevel-1);
    if(detaillevel) for(int z=0; z<layers; z++) 
      draw_qfi(c, mscale(V, zgrad0(-geom3::lake_top, 0, z, layers)), col, PPR::FLOOR+z-layers);
    }
  else if(sidepar == SIDE_LTOB) {
    const int layers = 1 << (detaillevel-1);
    if(detaillevel) for(int z=0; z<layers; z++) 
      draw_qfi(c, mscale(V, zgrad0(-geom3::lake_bottom, -geom3::lake_top, z, layers)), col, PPR::INLAKEWALL+z-layers);
    }
  else if(sidepar == SIDE_BTOI) {
    const int layers = 1 << detaillevel;
    draw_qfi(c, mscale(V, geom3::INFDEEP), col, PPR::MINUSINF);
    for(int z=1; z<layers; z++) 
      draw_qfi(c, mscale(V, zgrad0(-geom3::lake_bottom, -geom3::lake_top, -z, 1)), col, PPR::LAKEBOTTOM+z-layers);
    }
  }

bool placeSidewall(cell *c, int i, int sidepar, const transmatrix& V, color_t col) {

  if(!qfi.fshape || !qfi.fshape->is_plain || !validsidepar[sidepar] || qfi.usershape >= 0) {
    escherSidewall(c, sidepar, V, col);
    return true;
    }
  
  if(qfi.fshape == &shBigTriangle && pseudohept(c->move(i))) return false;
  if(qfi.fshape == &shTriheptaFloor && !pseudohept(c) && !pseudohept(c->move(i))) return false;

  PPR prio;
  /* if(mirr) prio = PPR::GLASS - 2;
  else */ if(sidepar == SIDE_WALL) prio = PPR::WALL3 - 2;
  else if(sidepar == SIDE_WTS3) prio = PPR::WALL3 - 2;
  else if(sidepar == SIDE_LAKE) prio = PPR::LAKEWALL;
  else if(sidepar == SIDE_LTOB) prio = PPR::INLAKEWALL;
  else if(sidepar == SIDE_BTOI) prio = PPR::BELOWBOTTOM;
  else prio = PPR::REDWALL-2+4*(sidepar-SIDE_SLEV);
  
  transmatrix V2 = V * ddspin(c, i);
 
  if(binarytiling || archimedean || NONSTDVAR) {
    #if CAP_ARCM
    if(archimedean && !PURE)
      i = (i + arcm::parent_index_of(c->master)/DUALMUL + MODFIXER) % c->type;
    #endif
    draw_shapevec(c, V2, qfi.fshape->gpside[sidepar][i], col, prio);
    return false;
    }
  
  queuepolyat(V2, qfi.fshape->side[sidepar][pseudohept(c)], col, prio);
  return false;
  }
#endif

bool openorsafe(cell *c) {
  return c->wall == waMineOpen || mineMarkedSafe(c);
  }

#define Dark(x) darkena(x,0,0xFF)

int gridcolor(cell *c1, cell *c2) {
  if(cmode & sm::DRAW) return Dark(forecolor);
  if(!c2)
    return 0x202020 >> darken;
  int rd1 = rosedist(c1), rd2 = rosedist(c2);
  if(rd1 != rd2) {
    int r = rd1+rd2;
    if(r == 1) return Dark(0x802020);
    if(r == 3) return Dark(0xC02020);
    if(r == 2) return Dark(0xF02020);
    }
  if(chasmgraph(c1) != chasmgraph(c2))
    return Dark(0x808080);
  if(c1->land == laAlchemist && c2->land == laAlchemist && c1->wall != c2->wall && !c1->item && !c2->item)
    return Dark(0xC020C0);
  if((c1->land == laWhirlpool || c2->land == laWhirlpool) && (celldistAlt(c1) != celldistAlt(c2)))
    return Dark(0x2020A0);
  if(c1->land == laMinefield && c2->land == laMinefield && (openorsafe(c1) != openorsafe(c2)))
    return Dark(0xA0A0A0);
  return Dark(0x202020);
  }

#if CAP_SHAPES
void pushdown(cell *c, int& q, const transmatrix &V, double down, bool rezoom, bool repriority) {

  // since we might be changing priorities, we have to make sure that we are sorting correctly
  if(down > 0 && repriority) { 
    int qq = q+1;
    while(qq < isize(ptds))
      if(qq > q && ptds[qq]->prio < ptds[qq-1]->prio) {
        swap(ptds[qq], ptds[qq-1]);
        qq--;
        }
      else qq++;
    }
  
  while(q < isize(ptds)) {
    auto pp = dynamic_cast<dqi_poly*> (&*ptds[q++]);
    if(!pp) continue;
    auto& ptd = *pp;

    double z2;
    
    double z = zlevel(tC0(ptd.V));
    double lev = geom3::factor_to_lev(z);
    double nlev = lev - down;
    
    double xyscale = rezoom ? geom3::scale_at_lev(lev) / geom3::scale_at_lev(nlev) : 1;
    z2 = geom3::lev_to_factor(nlev);
    double zscale = z2 / z;
    
    // xyscale = xyscale + (zscale-xyscale) * (1+sin(ticks / 1000.0)) / 2;
    
    ptd.V = xyzscale( V, xyscale*zscale, zscale)
      * inverse(V) * ptd.V;
      
    if(!repriority) ;
    else if(nlev < -geom3::lake_bottom-1e-3) {
      ptd.prio = PPR::BELOWBOTTOM_FALLANIM;
      if(c->wall != waChasm)
        ptd.color = 0; // disappear!
      }
    else if(nlev < -geom3::lake_top-1e-3)
      ptd.prio = PPR::INLAKEWALL_FALLANIM;
    else if(nlev < 0)
      ptd.prio = PPR::LAKEWALL_FALLANIM;
    }
  }
#endif

// 1 : (floor, water); 2 : (water, bottom); 4 : (bottom, inf)

int shallow(cell *c) {
  if(cellUnstable(c)) return 0;
  else if(
    c->wall == waReptile) return 1;
  else if(c->wall == waReptileBridge ||
    c->wall == waGargoyleFloor ||
    c->wall == waGargoyleBridge ||
    c->wall == waTempFloor ||
    c->wall == waTempBridge ||
    c->wall == waPetrifiedBridge ||
    c->wall == waFrozenLake)
    return 5;
  return 7;
  }

bool allemptynear(cell *c) {
  if(c->wall) return false;
  forCellEx(c2, c) if(c2->wall) return false;
  return true;
  }

static const int trapcol[4] = {0x904040, 0xA02020, 0xD00000, 0x303030};
static const int terracol[8] = {0xD000, 0xE25050, 0xD0D0D0, 0x606060, 0x303030, 0x181818, 0x0080, 0x8080};

bool bright;

// how much to darken
int getfd(cell *c) {
  if(bright) return 0;
  switch(c->land) {
    case laRedRock:
    case laReptile:
    case laCanvas: 
      return 0;
      
    case laSnakeNest:
      return realred(c->wall) ? 0 : 1;
    
    case laTerracotta:
    case laMercuryRiver:
      return (c->wall == waMercury && wmspatial) ? 0 : 1;

    case laKraken:
    case laDocks:
    case laBurial:
    case laIvoryTower:
    case laDungeon:
    case laMountain:
    case laEndorian:
    case laCaribbean:
    case laWhirlwind:
    case laRose:
    case laWarpSea:
    case laTortoise:
    case laDragon:
    case laHalloween:
    case laHunting:
    case laOcean:
    case laLivefjord:
    case laWhirlpool:
    case laAlchemist:
    case laIce:
    case laGraveyard:
    case laBlizzard:
    case laRlyeh:
    case laTemple:
    case laWineyard:
    case laDeadCaves:
    case laPalace:
    case laCA:
    case laDual:
    case laBrownian:
      return 1;
    
    case laVariant:
      if(isWateryOrBoat(c)) return 1;
      return 2;
    
    case laTrollheim:
    default:
      return 2;
    }    
  }

int getSnakelevColor(cell *c, int i, int last, int fd, color_t wcol) {
  color_t col;
  if(c->wall == waTower) 
    col = 0xD0D0D0-i*0x101010;
  else if(c->land == laSnakeNest)
    return darkena(nestcolors[pattern_threecolor(c)] * (5 + i) / 4, 0, 0xFF);
  #if CAP_COMPLEX2
  else if(c->land == laBrownian)
    col = brownian::get_color(c->landparam % brownian::level + (i+1) * brownian::level);
  #endif
  else if(i == last-1)
    col = wcol;
  else
    col = winf[waRed1+i].color;
  return darkena(col, fd, 0xFF);
  }

#if CAP_SHAPES
void draw_wall(cell *c, const transmatrix& V, color_t wcol, color_t& zcol, int ct6, int fd) {
  zcol = wcol;
  color_t wcol0 = wcol;
  int starcol = wcol;        
  if(c->wall == waWarpGate) starcol = 0;
  if(c->wall == waVinePlant) starcol = 0x60C000;

  color_t wcol2 = gradient(0, wcol0, 0, .8, 1);

  if(c->wall == waClosedGate) {
    int hdir = 0;
    for(int i=0; i<c->type; i++) if(c->move(i)->wall == waClosedGate)
      hdir = i;
    transmatrix V2 = mscale(V, wmspatial?geom3::WALL:1) * ddspin(c, hdir, M_PI);
    queuepolyat(V2, shPalaceGate, darkena(wcol, 0, 0xFF), wmspatial?PPR::WALL3A:PPR::WALL);
    starcol = 0;
    }
  
  hpcshape& shThisWall = isGrave(c->wall) ? shCross : shWall[ct6];
  
  if(conegraph(c)) {   
    const int layers = 2 << detaillevel;
    for(int z=1; z<layers; z++) {
      double zg = zgrad0(0, geom3::actual_wall_height(), z, layers);
      draw_qfi(c, xyzscale(V, zg*(layers-z)/layers, zg),
        darkena(gradient(0, wcol, -layers, z, layers), 0, 0xFF), PPR::WALL3+z-layers+2);
      }
    floorShadow(c, V, SHADOW_WALL);
    }
  
  else if(true) {
    if(!wmspatial) {
      if(starcol) queuepoly(V, shThisWall, darkena(starcol, 0, 0xFF));
      }
    else {
      transmatrix Vdepth = mscale(V, geom3::WALL);
      int alpha = 0xFF;
      if(c->wall == waIcewall)
        alpha = 0xC0;

      if(starcol && !(wmescher && c->wall == waPlatform)) 
        queuepolyat(Vdepth, shThisWall, darkena(starcol, 0, 0xFF), PPR::WALL3A);

      draw_qfi(c, Vdepth, darkena(wcol0, fd, alpha), PPR::WALL3);
      floorShadow(c, V, SHADOW_WALL);
      
      if(c->wall == waCamelot) {
        forCellIdEx(c2, i, c) {
          if(placeSidewall(c, i, SIDE_SLEV, V, darkena(wcol2, fd, alpha))) break;
          }
        forCellIdEx(c2, i, c) {
          if(placeSidewall(c, i, SIDE_SLEV+1, V, darkena(wcol2, fd, alpha))) break;
          }
        forCellIdEx(c2, i, c) {
          if(placeSidewall(c, i, SIDE_SLEV+2, V, darkena(wcol2, fd, alpha))) break;
          }
        forCellIdEx(c2, i, c) {
          if(placeSidewall(c, i, SIDE_WTS3, V, darkena(wcol2, fd, alpha))) break;
          }
        }
      else {
        forCellIdEx(c2, i, c) 
          if(!highwall(c2) || conegraph(c2)) {
          if(placeSidewall(c, i, SIDE_WALL, V, darkena(wcol2, fd, alpha))) break;
          }
        }
      }
    }
  }
#endif

bool just_gmatrix;

int colorhash(color_t i) {
  return (i * 0x471211 + i*i*0x124159 + i*i*i*0x982165) & 0xFFFFFF;
  }

void draw_gravity_particles(cell *c, const transmatrix V) {
  int u = (int)(size_t)(c);
  u = ((u * 137) + (u % 1000) * 51) % 1000;
  int tt = ticks + u; fractick(ticks, 900);
  ld r0 = (tt % 900) / 1100.;
  ld r1 = (tt % 900 + 200) / 1100.;
  
  const color_t grav_normal_color = 0x808080FF;
  const color_t antigrav_color = 0xF04040FF;
  const color_t levitate_color = 0x40F040FF;
  
  ld lev = 2;
  
  if(spatial_graphics) {

    switch(gravity_state) {
      case gsNormal:
        for(int i=0; i<6; i++) {
          transmatrix T = V * spin(i*degree*60) * xpush(crossf/3);
          queueline(mmscale(T, 1 + (1-r0) * (lev-1)) * C0, mmscale(T, 1 + (1-r1) * (lev - 1)) * C0, grav_normal_color);
          }
        break;
      
      case gsAnti:
        for(int i=0; i<6; i++) {
          transmatrix T = V * spin(i*degree*60) * xpush(crossf/3);
          queueline(mmscale(T, 1 + r0 * (lev-1)) * C0, mmscale(T, 1 + r1 * (lev-1)) * C0, antigrav_color);
          }
        break;
      
      case gsLevitation:
        for(int i=0; i<6; i++) {
          transmatrix T0 = V * spin(i*degree*60 + tt/60. * degree) * xpush(crossf/3);
          transmatrix T1 = V * spin(i*degree*60 + (tt/60. + 30) * degree) * xpush(crossf/3);
          queueline(mmscale(T0, (lev+1)/2) * C0, mmscale(T1, (lev+1)/2) * C0, levitate_color);
          }
        break;
      }
    }
  
  else {
    switch(gravity_state) {
      case gsNormal:
        for(int i=0; i<6; i++) {
          transmatrix T0 = V * spin(i*degree*60) * xpush(crossf/3 * (1-r0));
          transmatrix T1 = V * spin(i*degree*60) * xpush(crossf/3 * (1-r1));
          queueline(T0 * C0, T1 * C0, grav_normal_color);
          }
        break;
      
      case gsAnti:
        for(int i=0; i<6; i++) {
          transmatrix T0 = V * spin(i*degree*60) * xpush(crossf/3 * r0);
          transmatrix T1 = V * spin(i*degree*60) * xpush(crossf/3 * r1);
          queueline(T0 * C0, T1 * C0, antigrav_color);
          }
        break;
      
      case gsLevitation:
        for(int i=0; i<6; i++) {
          transmatrix T0 = V * spin(i*degree*60 + tt/60. * degree) * xpush(crossf/3);
          transmatrix T1 = V * spin(i*degree*60 + (tt/60. + 30) * degree) * xpush(crossf/3);
          queueline(T0 * C0, T1 * C0, levitate_color);
          }
        break;
      }
    }
  }

void drawcell(cell *c, transmatrix V, int spinv, bool mirrored) {

  cells_drawn++;

#if CAP_TEXTURE
  if(texture::saving) {
    texture::config.apply(c, V, 0xFFFFFFFF);
    draw_qfi(c, V, 0xFFFFFFFF);
    return;
    }

  if((cmode & sm::DRAW) && texture::config.tstate == texture::tsActive && !mouseout() && c) 
    mapeditor::draw_texture_ghosts(c, V);
#endif
 
  bool orig = false;
  if(!inmirrorcount) {
    transmatrix& gm = gmatrix[c];
    orig = 
      gm[DIM][DIM] == 0 ? true : 
      euwrap ? hypot(gm[0][DIM], gm[1][DIM]) >= hypot(V[0][DIM], V[1][DIM]) :
      sphereflipped() ? fabs(gm[DIM][DIM]-1) <= fabs(V[DIM][DIM]-1) :
      fabs(gm[DIM][DIM]-1) >= fabs(V[DIM][DIM]-1) - 1e-8;

    if(orig) gm = V;
    }
  if(just_gmatrix) return;

  #if CAP_SHAPES
  set_floor(shFloor);
  #endif
  ivoryz = isGravityLand(c->land);

  // if(behindsphere(V)) return;
  
  if(callhandlers(false, hooks_drawcell, c, V)) return;
  
  ld dist0 = hdist0(tC0(V)) - 1e-6;
  if(vid.use_smart_range) detaillevel = 2;
  else if(dist0 < geom3::highdetail) detaillevel = 2;
  else if(dist0 < geom3::middetail) detaillevel = 1;
  else detaillevel = 0;

#ifdef BUILDZEBRA
  if(c->type == 6 && c->tmp > 0) {
    int i = c->tmp;
    zebra(cellwalker(c, i&15), 1, i>>4, "", 0);
    }
  
  c->item = eItem(c->heat / 4);
  buildAutomatonRule(c);
#endif

  #if CAP_SHAPES
  viewBuggyCells(c,V);
  #endif
  
  if(conformal::on || inHighQual || sightrange_bonus > gamerange_bonus) checkTide(c);
  
  // save the player's view center
  if(isPlayerOn(c) && !shmup::on) {
    playerfound = true;

    if(multi::players > 1) {
      for(int i=0; i<numplayers(); i++) 
        if(playerpos(i) == c) {
          playerV = V * ddspin(c, multi::player[i].spin);
          if(multi::player[i].mirrored) playerV = playerV * Mirror;
          if(multi::player[i].mirrored == mirrored)
            multi::whereis[i] = playerV;
          }
      }
    else {
      playerV = V * ddspin(c, cwt.spin);
      if(cwt.mirrored) playerV = playerV * Mirror;
      if(orig) cwtV = playerV;
      }
    }
  
  if(1) {
  
    hyperpoint VC0 = tC0(V);
  
    if(inmirrorcount) ;
    else if(intval(mouseh, VC0) < modist) {
      modist2 = modist; mouseover2 = mouseover;
      modist = intval(mouseh, VC0);
      mouseover = c;
      }
    else if(intval(mouseh, VC0) < modist2) {
      modist2 = intval(mouseh, VC0);
      mouseover2 = c;
      }

    if(!euclid) {
      double dfc = euclid ? intval(VC0, C0) : VC0[DIM];
    
      if(dfc < centdist) {
        centdist = dfc;
        centerover = cellwalker(c, spinv, mirrored);
        }
      }
    
    int orbrange = (items[itRevolver] ? 3 : 2);
    
    if(c->cpdist <= orbrange) if(multi::players > 1 || multi::alwaysuse) 
    for(int i=0; i<multi::players; i++) if(multi::playerActive(i)) {
      double dfc = intval(VC0, tC0(multi::crosscenter[i]));
      if(dfc < multi::ccdist[i] && celldistance(playerpos(i), c) <= orbrange) {
        multi::ccdist[i] = dfc;
        multi::ccat[i] = c;
        }
      }

    if(inmirror(c)) {
      if(inmirrorcount >= 10) return;
      cellwalker cw(c, 0, mirrored);
      cw = mirror::reflect(cw);
      int cmc = (cw.mirrored == mirrored) ? 2 : 1;
      inmirrorcount += cmc;
      if(cw.mirrored != mirrored) V = V * Mirror;
      if(cw.spin) V = V * spin(2*M_PI*cw.spin/cw.at->type);
      drawcell(cw.at, V, 0, cw.mirrored);
      inmirrorcount -= cmc;
      return;
      }                  
    
    // color_t col = 0xFFFFFF - 0x20 * c->maxdist - 0x2000 * c->cpdist;

    if(!buggyGeneration && c->mpdist > 8 && !cheater && !autocheat) return; // not yet generated
    /* if(!buggyGeneration && c->mpdist > 7 && !cheater) {
      int cd = c->mpdist;
      string label = its(cd);
      int dc = distcolors[cd&7];
      queuestr(V, (cd > 9 ? .6 : 1) * .2, label, 0xFF000000 + dc, 1);
      } */
    
    #if CAP_SHAPES
    if(c->land == laNone && (cmode & sm::MAP)) {
      queuepoly(V, shTriangle, 0xFF0000FF);
      }
    #endif
  
    char ch = winf[c->wall].glyph;
    color_t wcol, fcol, asciicol;
    
    setcolors(c, wcol, fcol);

    if(inmirror(c)) {
      // for debugging
      if(c->land == laMirrored) fcol = 0x008000;
      if(c->land == laMirrorWall2) fcol = 0x800000;
      if(c->land == laMirrored2) fcol = 0x000080;
      }
    
    for(int k=0; k<inmirrorcount; k++)
      wcol = gradient(wcol, 0xC0C0FF, 0, 0.2, 1),
      fcol = gradient(fcol, 0xC0C0FF, 0, 0.2, 1);

    // addaura(tC0(V), wcol);
    color_t zcol = fcol;      

    if(peace::on && peace::hint && c->land != laTortoise) {
      int d =
        (c->land == laCamelot || (c->land == laCaribbean && celldistAlt(c) <= 0) || (c->land == laPalace && celldistAlt(c))) ? celldistAlt(c):
        celldist(c);

      int dc = 
        0x10101 * (127 + int(127 * sintick(200, d*.75/M_PI)));
      wcol = gradient(wcol, dc, 0, .3, 1);
      fcol = gradient(fcol, dc, 0, .3, 1);
      }

    if(c->land == laMirrored || c->land == laMirrorWall2 || c->land == laMirrored2) {
      string label = its(c->landparam);
      queuestr(V, 1 * .2, label, 0xFFFFFFFF, 1);
      }
 
    if(viewdists) do_viewdist(c, V, wcol, fcol);

    if(cmode & sm::TORUSCONFIG) {
      using namespace torusconfig;
      string label;
      bool small;
      if(tmflags() & TF_SINGLE) {
        int cd = torus_cx * dx + torus_cy * newdy;
        cd %= newqty; if(cd<0) cd += newqty;
        label = its(cd);
        small = cd;
        }
      else {
        small = true;
        label = its(torus_cx) + "," + its(torus_cy);
        }
      queuestr(V, small ? .2 : .6, label, small ? 0xFFFFFFD0 : 0xFFFF0040, 1);
      }

    asciicol = wcol;
    
    #if CAP_SHAPES
    if(c->land == laNone && c->wall == waNone) 
      queuepoly(V, shTriangle, 0xFFFF0000);
    #endif

    if(c->wall == waThumperOn) {
      ld ds = fractick(160);
      for(int u=0; u<5; u++) {
        ld rad = hexf * (.3 * (u + ds));
        int tcol = darkena(gradient(forecolor, backcolor, 0, rad, 1.5 * hexf), 0, 0xFF);
        PRING(a)
          curvepoint(V*xspinpush0(a * M_PI / S42, rad));
        queuecurve(tcol, 0, PPR::LINE);
        }
      }
  
    // bool dothept = false;

    /* if(pseudohept(c) && vid.darkhepta) {
      col = gradient(0, col, 0, 0.75, 1);
      } */
      
    eItem it = c->item;
    
    bool hidden = itemHidden(c);
    bool hiddens = itemHiddenFromSight(c);
    
    if(conformal::includeHistory && conformal::infindhistory.count(c)) {
      hidden = true;
      hiddens = false;
      }
    
    if(hiddens && !(cmode & sm::MAP))
      it = itNone;

    int icol = 0, moncol = 0xFF00FF;
    
    if(it) 
      ch = iinf[it].glyph, asciicol = icol = iinf[it].color;
    
    if(c->monst) {
      ch = minf[c->monst].glyph, moncol = minf[c->monst].color;
      if(c->monst == moMutant) {
        // root coloring
        if(c->stuntime != mutantphase)
          moncol = 
            gradient(0xC00030, 0x008000, 0, (c->stuntime-mutantphase) & 15, 15);
        }
      if(isMetalBeast(c->monst) && c->stuntime) 
        moncol >>= 1;

      if(c->monst == moSlime) {
        moncol = winf[c->wall].color;
        moncol |= (moncol>>1);
        }
      
      asciicol = moncol;

      if(isDragon(c->monst) || isKraken(c->monst)) if(!c->hitpoints)
        asciicol = 0x505050;
      
      if(c->monst == moTortoise)
        asciicol =  tortoise::getMatchColor(tortoise::getb(c));
      
      if(c->monst != moMutant) for(int k=0; k<c->stuntime; k++) 
        asciicol = ((asciicol & 0xFEFEFE) >> 1) + 0x101010;
      }
    
    if(c->cpdist == 0 && mapeditor::drawplayer) { 
      ch = '@'; 
      if(!mmitem) asciicol = moncol = cheater ? 0xFF3030 : 0xD0D0D0; 
      }
    
    if(c->ligon) {
      int tim = ticks - lightat;
      if(tim > 1000) tim = 800;
      if(elec::havecharge && tim > 400) tim = 400;
      for(int t=0; t<c->type; t++) if(c->move(t) && c->move(t)->ligon) {
        ld hdir = displayspin(c, t);
        int lcol = darkena(gradient(iinf[itOrbLightning].color, 0, 0, tim, 1100), 0, 0xFF);
        queueline(V*xspinpush0(ticks * M_PI / S42, hexf/2), V*xspinpush0(hdir, crossf), lcol, 2 + vid.linequality);
        }
      }
    
    int ctype = c->type;
    #if CAP_SHAPES
    int ct6 = ctof(c);
    #endif

    bool error = false;
    
    #if CAP_SHAPES
    chasmg = chasmgraph(c);
    #endif
    
    int fd = getfd(c);
    
    #if CAP_SHAPES
    int flooralpha = 255;
    #endif

#if CAP_EDIT && CAP_TEXTURE
    if((cmode & sm::DRAW) && mapeditor::drawcell && mapeditor::drawcellShapeGroup() == mapeditor::sgFloor && texture::config.tstate != texture::tsActive)
      flooralpha = 0xC0;
#endif

    if(c->wall == waMagma) fd = 0;
    
    poly_outline = OUTLINE_DEFAULT;
    
    int sl = snakelevel(c);
    
    transmatrix Vd0, Vboat0;
    const transmatrix *Vdp =
      (!wmspatial) ? &V : 
      sl ? &(Vd0= mscale(V, geom3::SLEV[sl])) : 
      highwall(c) ? &(Vd0= mscale(V, (1+geom3::WALL)/2)) :
#if CAP_SHAPES
      (chasmg==1) ? &(Vd0 = mscale(V, geom3::LAKE)) :
#endif
      &V;
    
#if CAP_SHAPES
    transmatrix Vf0;
    const transmatrix& Vf = (chasmg && wmspatial) ? (Vf0=mscale(V, geom3::BOTTOM)) : V;
#endif

    const transmatrix *Vboat = &(*Vdp);
      
    shmup::drawMonster(V, c, Vboat, Vboat0, Vdp);

    poly_outline = OUTLINE_DEFAULT;    
    if(!wmascii) {    

#if CAP_EDIT
      auto si = patterns::getpatterninfo0(c);
      if(drawing_usershape_on(c, mapeditor::sgFloor))
        mapeditor::drawtrans = V * applyPatterndir(c, si);
#endif
        
#if CAP_SHAPES
      // floor
      bool eoh = euclid || !BITRUNCATED;

      if(c->wall == waChasm) {
        zcol = 0;
        int rd = rosedist(c);
        if(rd == 1) 
          draw_floorshape(c, V, shRoseFloor, 0x80406020);
        if(rd == 2)
          draw_floorshape(c, V, shRoseFloor, 0x80406040);
        if(c->land == laZebra) fd++;
        if(c->land == laHalloween && !wmblack) {
          transmatrix Vdepth = wmspatial ? mscale(V, geom3::BOTTOM) : V;
          draw_floorshape(c, Vdepth, shFullFloor, darkena(firecolor(0, 10), 0, 0xDF), PPR::LAKEBOTTOM);
          }
        }

#if CAP_EDIT
      else if(mapeditor::haveUserShape(mapeditor::sgFloor, si.id)) {
        qfi.usershape = si.id;
        qfi.spin = applyPatterndir(c, si);
        }
#endif

      else if(patterns::whichShape == '7')
        set_floor(shBigHepta);
      
      else if(patterns::whichShape == '8')
        set_floor(shTriheptaFloor);
      
      else if(patterns::whichShape == '6')
        set_floor(shBigTriangle);

      else if(among(patterns::whichShape, '9', '^'))
        set_floor(shFullFloor);

#if CAP_TEXTURE
      else if(texture::config.apply(c, Vf, darkena(fcol, fd, 0xFF))) ;
#endif
      
      else if(c->land == laMirrorWall) {
        int d = mirror::mirrordir(c);
        bool onleft = c->type == 7;
        if(c->type == 7 && c->barleft == laMirror)
          onleft = !onleft;
        if(c->type == 6 && d != -1 && c->move(d)->barleft == laMirror)
          onleft = !onleft;
        if(PURE) onleft = !onleft;
        
        if(d == -1) {
          for(d=0; d<c->type; d++)
            if(c->move(d) && c->modmove(d+1) && c->move(d)->land == laMirrorWall && c->modmove(d+1)->land == laMirrorWall)
              break;
          qfi.spin = ddspin(c, d, 0);
          transmatrix V2 = V * qfi.spin;
          if(!wmblack) for(int d=0; d<c->type; d++) {
            inmirrorcount+=d;
            queuepolyat(V2 * spin(d*M_PI/S3), shHalfFloor[2], darkena(fcol, fd, 0xFF), PPR::FLOORa);
            inmirrorcount-=d;
            }          
          if(wmspatial) {
            const int layers = 2 << detaillevel;
            for(int z=1; z<layers; z++) 
              queuepolyat(mscale(V2, zgrad0(0, geom3::actual_wall_height(), z, layers)), shHalfMirror[2], 0xC0C0C080, PPR::WALL3+z-layers);
            }
          else
            queuepolyat(V2, shHalfMirror[2], 0xC0C0C080, PPR::WALL3);
          }
        else {
          qfi.spin = ddspin(c, d, M_PI);
          transmatrix V2 = V * qfi.spin;
          if(!wmblack) {
            inmirrorcount++;
            queuepolyat(mirrorif(V2, !onleft), shHalfFloor[ct6], darkena(fcol, fd, 0xFF), PPR::FLOORa);
            inmirrorcount--;
            queuepolyat(mirrorif(V2, onleft), shHalfFloor[ct6], darkena(fcol, fd, 0xFF), PPR::FLOORa);
            }
  
          if(wmspatial) {
            const int layers = 2 << detaillevel;
            for(int z=1; z<layers; z++) 
              queuepolyat(mscale(V2, zgrad0(0, geom3::actual_wall_height(), z, layers)), shHalfMirror[ct6], 0xC0C0C080, PPR::WALL3+z-layers);
            }
          else 
            queuepolyat(V2, shHalfMirror[ct6], 0xC0C0C080, PPR::WALL3);
          }
        }
      
      else if(c->land == laWineyard && cellHalfvine(c)) {

        int i =-1;
        for(int t=0;t<6; t++) if(c->move(t) && c->move(t)->wall == c->wall)
          i = t;

        qfi.spin = ddspin(c, i, M_PI/S3);
        transmatrix V2 = V * qfi.spin;
        
        if(wmspatial && wmescher) {
          set_floor(shSemiFeatherFloor[0]);
          int dk = 1;
          int vcol = winf[waVinePlant].color;
          draw_qfi(c, mscale(V2, geom3::WALL), darkena(vcol, dk, 0xFF), PPR::WALL3A);
          escherSidewall(c, SIDE_WALL, V2, darkena(gradient(0, vcol, 0, .8, 1), dk, 0xFF));
          queuepoly(V2, shSemiFeatherFloor[1], darkena(fcol, dk, 0xFF));
          set_floor(shFeatherFloor);
          }
        
        else if(wmspatial) {
          floorshape& shar = *(wmplain ? (floorshape*)&shFloor : (floorshape*)&shFeatherFloor);
          
          set_floor(shar);

          int vcol = winf[waVinePlant].color;
          int vcol2 = gradient(0, vcol, 0, .8, 1);
          
          transmatrix Vdepth = mscale(V2, geom3::WALL);

          queuepolyat(Vdepth, shSemiFloor[0], darkena(vcol, fd, 0xFF), PPR::WALL3A);
          {dynamicval<color_t> p(poly_outline, OUTLINE_TRANS); queuepolyat(V2 * spin(M_PI*2/3), shSemiFloorShadow, SHADOW_WALL, PPR::WALLSHADOW); }
          queuepolyat(V2, shSemiFloorSide[SIDE_WALL], darkena(vcol, fd, 0xFF), PPR::WALL3A-2+away(V2));

          if(validsidepar[SIDE_WALL]) forCellIdEx(c2, j, c) {
            int dis = i-j;
            dis %= 6;
            if(dis<0) dis += 6;
            if(dis != 1 && dis != 5) continue;
            if(placeSidewall(c, j, SIDE_WALL, V, darkena(vcol2, fd, 0xFF))) break;
            }
          }
        
        else {
          hpcshape *shar = shSemiFeatherFloor;
          
          if(wmblack) shar = shSemiBFloor;
          if(wmplain) shar = shSemiFloor;
          
          queuepoly(V2, shar[0], darkena(winf[waVinePlant].color, fd, 0xFF));
          
          set_floor(qfi.spin, shar[1]);
          }
        }

      else if(c->land == laReptile || c->wall == waReptile) 
        set_reptile_floor(c, Vf, fcol);
      
      else if(wmblack == 1 && c->wall == waMineOpen && vid.grid) 
        ;
      
      else if(wmblack) {
        set_floor(shBFloor[ct6]);
        int rd = rosedist(c);
        if(rd == 1)
          queuepoly(Vf, shHeptaMarker, darkena(fcol, 0, 0x80));
        else if(rd == 2)
          queuepoly(Vf, shHeptaMarker, darkena(fcol, 0, 0x40));
        }
      
      else if(isWarped(c) || is_nice_dual(c))
        set_maywarp_floor(c); 

      else if(wmplain) {
        set_floor(shFloor);
        }

      else if(randomPatternsMode && c->land != laBarrier && !isWarped(c->land)) {
        int j = (randompattern[c->land]/5) % 15;
        int k = randompattern[c->land] % RPV_MODULO;
        int k7 = randompattern[c->land] % 7;
        
        if(k == RPV_ZEBRA && k7 < 2) set_zebrafloor(c);
        else if(k == RPV_EMERALD && k7 == 0) set_emeraldfloor(c);
        else if(k == RPV_CYCLE && k7 < 4) set_towerfloor(c, celldist);
 
        else switch(j) {
          case 0:  set_floor(shCloudFloor); break;
          case 1:  set_floor(shFeatherFloor); break;
          case 2:  set_floor(shStarFloor); break;
          case 3:  set_floor(shTriFloor); break;
          case 4:  set_floor(shSStarFloor); break;
          case 5:  set_floor(shOverFloor); break;
          case 6:  set_floor(shFeatherFloor); break;
          case 7:  set_floor(shDemonFloor); break;
          case 8:  set_floor(shCrossFloor); break;
          case 9:  set_floor(shMFloor); break;
          case 10: set_floor(shCaveFloor); break;
          case 11: set_floor(shPowerFloor); break;
          case 12: set_floor(shDesertFloor); break;
          case 13: set_floor(shChargedFloor); break;
          case 14: set_floor(shLavaFloor); break;
          }
        }

      // else if(c->land == laPrairie && !eoh && allemptynear(c) && fieldpattern::getflowerdist(c) <= 1)
      //   queuepoly(Vf, shLeafFloor[ct6], darkena(fcol, fd, 0xFF));

      /* else if(c->land == laPrairie && prairie::isriver(c))
        
          set_towerfloor(Vf, c, darkena(fcol, fd, 0xFF), 
          prairie::isleft(c) ? river::towerleft : river::towerright); */
      
      else switch(c->land) {
        case laPrairie:
        case laAlchemist:
          set_floor(shCloudFloor);
          break;
        
        case laJungle:
        case laWineyard:
          set_floor(shFeatherFloor);
          break;
        
        case laZebra:
          set_zebrafloor(c);
          break;
        
        case laMountain:
          set_towerfloor(c, euclid ? celldist : c->master->alt ? celldistAltPlus : celldist);
          break;
        
        case laEmerald:
          set_emeraldfloor(c);
          break;
        
        case laRlyeh:
        case laTemple:
          set_floor(shTriFloor);
          break;
        
        case laVolcano:
        case laVariant:
          set_floor(shLavaFloor);
          break;
        
        case laRose:
          set_floor(shRoseFloor);
          break;
        
        case laTortoise:
          set_floor(shTurtleFloor);
          break;
        
        case laBurial: case laRuins:
          set_floor(shBarrowFloor);
          break;

        case laTrollheim:
          set_floor(shTrollFloor);
          break;

        /*case laMountain:
          set_floor(FEATHERFLOOR);
          break; */
        
        case laGraveyard:
          set_floor(shCrossFloor);
          break;
        
        case laMotion:
          set_floor(shMFloor);
          break;
        
        case laWhirlwind:
        case laEFire: case laEAir: case laEWater: case laEEarth: case laElementalWall:
          set_floor(shNewFloor);
          break;
        
        case laHell:
          set_floor(shDemonFloor);
          break;
              
        case laIce: case laBlizzard:
          set_floor(shStarFloor);
          break;
        
        case laSwitch:
          set_floor(shSwitchFloor);
          if(ctof(c) && STDVAR && !archimedean && !binarytiling) for(int i=0; i<c->type; i++)
            queuepoly(Vf * ddspin(c, i, M_PI/S7) * xpush(rhexf), shSwitchDisk, darkena(minf[active_switch()].color, fd, 0xFF));
          break;

        case laStorms:
          set_floor(shChargedFloor);
          break;
        
        case laWildWest:
          set_floor(shSStarFloor);
          break;
        
        case laPower:
          set_floor(shPowerFloor);
          break;
        
        case laCaves:
        case laLivefjord:
        case laDeadCaves:
          set_floor(shCaveFloor);
          break;
        
        case laDesert:
        case laDryForest:
        case laRedRock: case laSnakeNest:
        case laCocytus:
          set_floor(shDesertFloor);
          break;

        case laBull:
          set_floor(shButterflyFloor);
          break;
        
        case laCaribbean: case laOcean: case laOceanWall: case laWhirlpool:
          set_floor(shCloudFloor);
          break;
        
        case laKraken:
        case laDocks:
          set_floor(shFullFloor);
          break;
        
        case laPalace: case laTerracotta:
          set_floor(shPalaceFloor);
          break;
        
        case laDragon:
          set_floor(shDragonFloor);
          break;
        
        case laOvergrown: case laClearing: case laHauntedWall: case laHaunted: case laHauntedBorder:
          set_floor(shOverFloor);
          break;

        case laMercuryRiver: {
          if(eoh)
            set_floor(shFloor);
          else {
            int bridgedir = -1;
            if(c->type == 6) {
              for(int i=1; i<c->type; i+=2)
                if(pseudohept(c->modmove(i-1)) && c->modmove(i-1)->land == laMercuryRiver)
                if(pseudohept(c->modmove(i+1)) && c->modmove(i+1)->land == laMercuryRiver)
                  bridgedir = i;
              }
            if(bridgedir == -1)
              set_floor(shPalaceFloor);
            else {
              transmatrix bspin = ddspin(c, bridgedir);
              set_floor(bspin, shMercuryBridge[0]);
              // only needed in one direction
              if(c < c->move(bridgedir)) {
                bspin = Vf * bspin;
                queuepoly(bspin, shMercuryBridge[1], darkena(fcol, fd+1, 0xFF));
                if(wmspatial) {
                  queuepolyat(mscale(bspin, geom3::LAKE), shMercuryBridge[1], darkena(gradient(0, winf[waMercury].color, 0, 0.8,1), 0, 0x80), PPR::LAKELEV);
                  queuepolyat(mscale(bspin, geom3::BOTTOM), shMercuryBridge[1], darkena(0x202020, 0, 0xFF), PPR::LAKEBOTTOM);
                  }
                }
              }
            }
          break;
          }
        
        case laHive:
          if(c->wall != waFloorB && c->wall != waFloorA && c->wall != waMirror && c->wall != waCloud) {
            fd = 1;
            set_floor(shFloor);
            if(c->wall != waMirror && c->wall != waCloud)
              draw_floorshape(c, V, shMFloor, darkena(fcol, 2, 0xFF), PPR::FLOORa);
            if(c->wall != waMirror && c->wall != waCloud)
              draw_floorshape(c, V, shMFloor2, darkena(fcol, fcol==wcol ? 1 : 2, 0xFF), PPR::FLOORb);
            }
          else
            set_floor(shFloor);
          break;
        
        case laEndorian:
          if(c->wall == waTrunk) 
            set_floor(shFloor);
    
          else if(c->wall == waCanopy || c->wall == waSolidBranch || c->wall == waWeakBranch) 
            set_floor(shFeatherFloor);
          
          else 
            set_towerfloor(c);
          break;
        
        case laIvoryTower: case laDungeon: case laWestWall:
          set_towerfloor(c);
          break;
        
        case laBrownian:
          if(among(c->wall, waSea, waBoat))
            set_floor(shCloudFloor);
          else
            set_floor(shTrollFloor);
          break;

        default: 
          set_floor(shFloor);
        }

      // actually draw the floor

      if(chasmg == 2) ;
      else if(chasmg && wmspatial && detaillevel == 0) {
        draw_qfi(c, (*Vdp), darkena(fcol, fd, 0x80), PPR::LAKELEV);
        }
      else if(chasmg && wmspatial) {
      
        color_t col = c->land == laCocytus ? 0x080808FF : 0x101010FF;

        if(qfi.fshape == &shCloudFloor) 
          set_floor(shCloudSeabed);
        else if(qfi.fshape == &shLavaFloor) 
          set_floor(shLavaSeabed);
        else if(qfi.fshape == &shFloor)
          set_floor(shFullFloor);
        else if(qfi.fshape == &shCaveFloor)
          set_floor(shCaveSeabed);
        
        draw_qfi(c, mscale(V, geom3::BOTTOM), col, PPR::LAKEBOTTOM);

        int fd0 = fd ? fd-1 : 0;      
        draw_qfi(c, (*Vdp), darkena(fcol, fd0, 0x80), PPR::LAKELEV);
        }
      else {
        if(patterns::whichShape == '^') poly_outline = darkena(fcol, fd, flooralpha);
        draw_qfi(c, V, darkena(fcol, fd, flooralpha));
        }
      
      // walls

#if CAP_EDIT

      if(patterns::displaycodes) {
      
        auto si = patterns::getpatterninfo0(c);
        
        for(int i=(si.dir + MODFIXER) % si.symmetries; i<c->type; i += si.symmetries) {
          queuepoly(V * ddspin(c, i) * (si.reflect?Mirror:Id), shAsymmetric, darkena(0x000000, 0, 0xC0));
          si.dir += si.symmetries;
          }
        
        string label = its(si.id & 255);
        color_t col = forecolor ^ colorhash(si.id >> 8);
        queuestr(V, .5, label, 0xFF000000 + col);
        }
#endif

      if((cmode & sm::NUMBER) && (dialog::editingDetail())) {
        color_t col = 
          dist0 < geom3::highdetail ? 0xFF80FF80 :
          dist0 >= geom3::middetail ? 0xFFFF8080 :
          0XFFFFFF80;
#if 1
        queuepoly(V, shHeptaMarker, darkena(col & 0xFFFFFF, 0, 0xFF));
#else
        char buf[64];
        sprintf(buf, "%3.1f", float(dist0));
        

        queuestr(V, .6, buf, col);
#endif
        }

      if(realred(c->wall) && !wmspatial) {
        int s = snakelevel(c);
        if(s >= 1) draw_floorshape(c, V, shRedRockFloor[0], getSnakelevColor(c, 0, 7, fd, wcol));
        if(s >= 2) draw_floorshape(c, V, shRedRockFloor[1], getSnakelevColor(c, 1, 7, fd, wcol));
        if(s >= 3) draw_floorshape(c, V, shRedRockFloor[2], getSnakelevColor(c, 2, 7, fd, wcol));
        }
      
      if(c->wall == waTower && !wmspatial) {
        fcol = 0xE8E8E8;
        set_floor(shMFloor);
        }
      
      if(pseudohept(c) && (
        c->land == laRedRock || 
        vid.darkhepta ||
        (c->land == laClearing && !BITRUNCATED))) {
        queuepoly((*Vdp), shHeptaMarker, wmblack ? 0x80808080 : 0x00000080);
        }

      if(conformal::includeHistory && conformal::inmovehistory.count(c))
        queuepoly((*Vdp), shHeptaMarker, 0x000000C0);

      char xch = winf[c->wall].glyph;
      
      if(DIM == 3) {
        if(isWall(c)) {

          const int darkval[9] = {0,1,1,0,3,3,4,4,0};
          int d = (asciicol & 0xF0F0F0) >> 3;
          for(int a=0; a<9; a++)
            if(c->move(a) && !isWall(c->move(a))) {
              if(a < 4) {
                if(celldistAlt(c) >= celldistAlt(viewctr.at->c7)) continue;
                dynamicval<color_t> p (poly_outline, 0);
                queuepoly(V, shBinaryWall[a], darkena(asciicol - d * darkval[a], 0, 0xFF));
                }
              else {
                queuepoly(V, shBinaryWall[a], darkena(asciicol - d * darkval[a], 0, 0xFF));
                }
              }
          }
        else if(c->wall == waNone) ;
        else error = true;
        }
      
      else switch(c->wall) {
      
        case waBigBush:
          if(detaillevel >= 2)
            queuepolyat(mmscale(V, zgrad0(0, geom3::slev, 1, 2)), shHeptaMarker, darkena(wcol, 0, 0xFF), PPR::REDWALL);
          if(detaillevel >= 1)
            queuepolyat(mmscale(V, geom3::SLEV[1]) * pispin, shWeakBranch, darkena(wcol, 0, 0xFF), PPR::REDWALL+1);
          if(detaillevel >= 2)
            queuepolyat(mmscale(V, zgrad0(0, geom3::slev, 3, 2)), shHeptaMarker, darkena(wcol, 0, 0xFF), PPR::REDWALL+2);
          queuepolyat(mmscale(V, geom3::SLEV[2]), shSolidBranch, darkena(wcol, 0, 0xFF), PPR::REDWALL+3);
          break;
        
        case waSmallBush:
          if(detaillevel >= 2)
            queuepolyat(mmscale(V, zgrad0(0, geom3::slev, 1, 2)), shHeptaMarker, darkena(wcol, 0, 0xFF), PPR::REDWALL);
          if(detaillevel >= 1)
            queuepolyat(mmscale(V, geom3::SLEV[1]) * pispin, shWeakBranch, darkena(wcol, 0, 0xFF), PPR::REDWALL+1);
          if(detaillevel >= 2)
            queuepolyat(mmscale(V, zgrad0(0, geom3::slev, 3, 2)), shHeptaMarker, darkena(wcol, 0, 0xFF), PPR::REDWALL+2);
          queuepolyat(mmscale(V, geom3::SLEV[2]), shWeakBranch, darkena(wcol, 0, 0xFF), PPR::REDWALL+3);
          break;
      
        case waSolidBranch:
          queuepoly(V, shSolidBranch, darkena(wcol, 0, 0xFF));
          break;
        
        case waWeakBranch:
          queuepoly(V, shWeakBranch, darkena(wcol, 0, 0xFF));
          break;
      
        case waLadder:
          if(euclid) {
            draw_floorshape(c, V, shMFloor, 0x804000FF);
            draw_floorshape(c, V, shMFloor2, 0x000000FF);
            }
          else {
            draw_floorshape(c, V, shFloor, 0x804000FF, PPR::FLOOR+1);
            draw_floorshape(c, V, shMFloor, 0x000000FF, PPR::FLOOR+2);
            }
          break;
        
        case waReptileBridge: {
          Vboat = &(Vboat0 = V);
          dynamicval<qfloorinfo> qfi2(qfi, qfi);
          color_t col = reptilecolor(c);
          chasmg = 0;
          set_reptile_floor(c, V, col);
          draw_qfi(c, V, col);
          forCellIdEx(c2, i, c) if(chasmgraph(c2)) 
            if(placeSidewall(c, i, SIDE_LAKE, V, darkena(gradient(0, col, 0, .8, 1), fd, 0xFF))) break;
          chasmg = 1;
          break;
          }
        
        case waTerraWarrior:
          drawTerraWarrior(V, randterra ? (c->landparam & 7) : (5 - (c->landparam & 7)), 7, 0);
          break;
        
        case waBoat: case waStrandedBoat: {
          double footphase;
          bool magical = items[itOrbWater] && (isPlayerOn(c) || (isFriendly(c) && items[itOrbEmpathy]));
          int outcol = magical ? watercolor(0) : 0xC06000FF;
          int incol = magical ? 0x0060C0FF : 0x804000FF;
          bool nospin = false;
  
          Vboat = &(Vboat0 = *Vboat); 
          if(wmspatial && c->wall == waBoat) {
            nospin = c->wall == waBoat && applyAnimation(c, Vboat0, footphase, LAYER_BOAT);
            if(!nospin) Vboat0 = Vboat0 * ddspin(c, c->mondir, M_PI);
            queuepolyat(Vboat0, shBoatOuter, outcol, PPR::BOATLEV);
            Vboat = &(Vboat0 = V);
            }
          if(c->wall == waBoat) {
            nospin = applyAnimation(c, Vboat0, footphase, LAYER_BOAT);
            }
          if(!nospin) 
            Vboat0 = Vboat0 * ddspin(c, c->mondir, M_PI);
          else {
            transmatrix Vx;
            if(applyAnimation(c, Vx, footphase, LAYER_SMALL))
              animations[LAYER_SMALL][c].footphase = 0;
            }
          if(wmspatial)
            queuepolyat(mscale(Vboat0, (geom3::LAKE+1)/2), shBoatOuter, outcol, PPR::BOATLEV2);
          queuepoly(Vboat0, shBoatOuter, outcol);
          queuepoly(Vboat0, shBoatInner, incol);
          break;
          }
        
        case waBigStatue: {
          transmatrix V2 = V;
          double footphase;
          applyAnimation(c, V2, footphase, LAYER_BOAT);
          
          queuepolyat(V2, shStatue, 
            darkena(winf[c->wall].color, 0, 0xFF),
            PPR::BIGSTATUE
            );
          break;
          }
        
        case waSulphurC: {
          if(drawstar(c)) {
            zcol = wcol;
            if(wmspatial) 
              queuepolyat(mscale(V, geom3::HELLSPIKE), shGiantStar[ct6], darkena(wcol, 0, 0x40), PPR::HELLSPIKE);
            else
              queuepoly(V, shGiantStar[ct6], darkena(wcol, 0, 0xFF));
            }
          break;
          }
        
        case waTrapdoor:
          if(c->land == laZebra) break;
          /* fallthrough */
        
        case waClosePlate: case waOpenPlate: {
          transmatrix V2 = V;
          if(wmescher && geosupport_football() == 2 && pseudohept(c) && c->land == laPalace) V2 = V * spin(M_PI / c->type);
          draw_floorshape(c, V2, shMFloor, darkena(winf[c->wall].color, 0, 0xFF));
          draw_floorshape(c, V2, shMFloor2, (!wmblack) ? darkena(fcol, 1, 0xFF) : darkena(0,1,0xFF));
          break;
          }
        
        case waFrozenLake: case waLake: case waCamelotMoat:
        case waSea: case waOpenGate: case waBubble: case waDock:
        case waSulphur: case waMercury:
          break;
        
        case waNone: 
          if(c->land == laBrownian) goto wa_default;
          break;
        
        case waRose: {
          zcol = wcol;
          wcol <<= 1;
          if(c->cpdist > 5)
            wcol = 0xC0C0C0;
          else if(rosephase == 7)
            wcol = 0xFF0000;
          else 
            wcol = gradient(wcol, 0xC00000, 0, rosephase, 6);
          queuepoly(V, shThorns, 0xC080C0FF);
  
          for(int u=0; u<4; u+=2)
            queuepoly(V * spin(2*M_PI / 3 / 4 * u), shRose, darkena(wcol, 0, 0xC0));
          break;
          }

        case waRoundTable: 
          if(wmspatial) goto wa_default;
          break;
        
        case waMirrorWall:
          break;
        
        case waGlass:
          if(wmspatial) {
            color_t col = winf[waGlass].color;
            int dcol = darkena(col, 0, 0x80);
            transmatrix Vdepth = mscale((*Vdp), geom3::WALL);
            draw_floorshape(c, Vdepth, shMFloor, dcol, PPR::WALL); // GLASS
            dynamicval<qfloorinfo> dq(qfi, qfi);
            set_floor(shMFloor);
            if(validsidepar[SIDE_WALL]) forCellIdEx(c2, i, c) 
              if(placeSidewall(c, i, SIDE_WALL, (*Vdp), dcol)) break;
            }
          break;
        
        case waFan:
          queuepoly(V * spintick(PURE ? -1000 : -500, 1/12.), shFan, darkena(wcol, 0, 0xFF));
          break;
        
        case waArrowTrap:
          if(c->wparam >= 1)
            queuepoly(V, shDisk, darkena(trapcol[c->wparam&3], 0, 0xFF));
          if(isCentralTrap(c)) arrowtraps.push_back(c);
          break;
      
        case waFireTrap:
          draw_floorshape(c, V, shMFloor, darkena(0xC00000, 0, 0xFF));
          draw_floorshape(c, V, shMFloor2, darkena(0x600000, 0, 0xFF));
          if(c->wparam >= 1)
            queuepoly(V, shDisk, darkena(trapcol[c->wparam&3], 0, 0xFF));
          break;
      
        case waGiantRug:
          queuepoly(V, shBigCarpet1, darkena(0xC09F00, 0, 0xFF));
          queuepoly(V, shBigCarpet2, darkena(0x600000, 0, 0xFF));
          queuepoly(V, shBigCarpet3, darkena(0xC09F00, 0, 0xFF));
          break;
        
        case waBarrier:
          if(c->land == laOceanWall && wmescher && wmspatial) {
           const int layers = 2 << detaillevel;
           dynamicval<const hpcshape*> ds(qfi.shape, &shCircleFloor);
           dynamicval<transmatrix> dss(qfi.spin, Id);
           for(int z=1; z<layers; z++) {
             double zg = zgrad0(-geom3::lake_top, geom3::actual_wall_height(), z, layers);
             draw_qfi(c, xyzscale(V, zg*(layers-z)/layers, zg),
               darkena(gradient(0, wcol, -layers, z, layers), 0, 0xFF), PPR::WALL3+z-layers+2);
             }
            }
          else goto wa_default;
          break;
        
        case waMineOpen: {
          int mines = countMinesAround(c);
          if(mines)
            queuepoly(V, shMineMark[ct6], darkena(minecolors[mines], 0, 0xFF));
          break;
          }
      
        default: {
          wa_default:
          if(sl && wmspatial) {
      
            draw_qfi(c, (*Vdp), darkena(wcol, fd, 0xFF), PPR::REDWALL-4+4*sl);
            floorShadow(c, V, SHADOW_SL * sl);
            for(int s=0; s<sl; s++) 
            forCellIdEx(c2, i, c) {
              int sl2 = snakelevel(c2);
              if(s >= sl2)
                if(placeSidewall(c, i, SIDE_SLEV+s, V, getSnakelevColor(c, s, sl, fd, wcol))) break;
              }
            }
          
          else if(highwall(c)) 
            draw_wall(c, V, wcol, zcol, ct6, fd);
 
          else if(xch == '%') {
            if(doHighlight())
              poly_outline = (c->land == laMirror) ? OUTLINE_TREASURE : OUTLINE_ORB;
            
            if(wmspatial) {
              color_t col = winf[c->wall].color;
              int dcol = darkena(col, 0, 0xC0);
              transmatrix Vdepth = mscale((*Vdp), geom3::WALL);
              draw_floorshape(c, Vdepth, shMFloor, dcol, PPR::WALL); // GLASS
              dynamicval<qfloorinfo> dq(qfi, qfi);
              set_floor(shMFloor);
              if(validsidepar[SIDE_WALL]) forCellIdEx(c2, i, c) 
                if(placeSidewall(c, i, SIDE_WALL, (*Vdp), dcol)) break;
              }
            else {
              queuepoly(V, shMirror, darkena(wcol, 0, 0xC0));
              }
            poly_outline = OUTLINE_DEFAULT;
            }
          
          else if(c->wall == waExplosiveBarrel) {
           const int layers = 2 << detaillevel;
           for(int z=1; z<=layers; z++) {
             double zg = zgrad0(0, geom3::actual_wall_height(), z, layers);
             queuepolyat(xyzscale(V, zg, zg), shBarrel, darkena((z&1) ? 0xFF0000 : 0xC00000, 0, 0xFF), PPR(PPR::REDWALLm+z));
             }
           }
          
          else if(isFire(c) || isThumper(c) || c->wall == waBonfireOff) {
            auto V2 = V;
            if(hasTimeout(c)) V2 = V2 * spintick(c->land == laPower ? 5000 : 500);
            queuepoly(V2, shStar, darkena(wcol, 0, 0xF0));
            if(isFire(c) && rand() % 300 < ticks - lastt)
              drawParticle(c, wcol, 75);
            }
          
          else if(xch != '.' && xch != '+' && xch != '>' && xch != ':'&& xch != '-' && xch != ';' && xch != ',' && xch != '&')
            error = true;
          }
        }
#else
      error = true;
#endif
      }

    else {
      if(c->wall == waArrowTrap)
        asciicol = trapcol[c->wparam & 3];
      if(c->wall == waFireTrap)
        asciicol = trapcol[c->wparam & 3];
      if(c->wall == waTerraWarrior)
        asciicol = terracol[c->landparam & 7];

      if(c->wall == waMineOpen) {
        int mines = countMinesAround(c);
        if(ch == '.') {
          if(mines == 0) ch = ' ';
          else ch = '0' + mines, asciicol = minecolors[mines];
          }
        else if(ch == '@') asciicol = minecolors[mines];
        }
      if(!(it || c->monst || c->cpdist == 0)) error = true;
      }
    
#if CAP_SHAPES
    int sha = shallow(c);

    if(wmspatial && sha) {
      color_t col = (highwall(c) || c->wall == waTower) ? wcol : fcol;
      if(!chasmg) {

#define D(v) darkena(gradient(0, col, 0, v * (sphere ? spherity(V * cellrelmatrix(c,i)) : 1), 1), fd, 0xFF)
// #define D(v) darkena(col, fd, 0xFF)

        if(sha & 1) {
          forCellIdEx(c2, i, c) if(chasmgraph(c2)) 
            if(placeSidewall(c, i, SIDE_LAKE, V, D(.8))) break;
          }
        if(sha & 2) {
          forCellIdEx(c2, i, c) if(chasmgraph(c2)) 
            if(placeSidewall(c, i, SIDE_LTOB, V, D(.7))) break;
          }
        if(sha & 4) {
          bool dbot = true;
          forCellIdEx(c2, i, c) if(chasmgraph(c2) == 2) {
            if(dbot) dbot = false,
              draw_qfi(c, mscale(V, geom3::BOTTOM), 0x080808FF, PPR::LAKEBOTTOM);
            if(placeSidewall(c, i, SIDE_BTOI, V, D(.6))) break;
            }
#undef D
          }
        }
      // wall between lake and chasm -- no Escher here
      if(chasmg == 1) forCellIdEx(c2, i, c) if(chasmgraph(c2) == 2) {
        dynamicval<qfloorinfo> qfib(qfi, qfi);
        set_floor(shFullFloor);
        placeSidewall(c, i, SIDE_LAKE, V, 0x202030FF);
        placeSidewall(c, i, SIDE_LTOB, V, 0x181820FF);
        placeSidewall(c, i, SIDE_BTOI, V, 0x101010FF);
        }
      }

    if(chasmg) {
      int q = isize(ptds);
      int maxtime = euclid || sphere ? 20000 : 1500;
      if(fallanims.count(c)) {
         fallanim& fa = fallanims[c];
         bool erase = true;
         if(fa.t_floor) {
           int t = (ticks - fa.t_floor);
           if(t <= maxtime) {
             erase = false;
             if(fa.walltype == waNone)
               draw_qfi(c, V, darkena(fcol, fd, 0xFF), PPR::FLOOR);
             else {
               color_t wcol2, fcol2;
               eWall w = c->wall; int p = c->wparam;
               c->wall = fa.walltype; c->wparam = fa.m;
               setcolors(c, wcol2, fcol2);
               int starcol = c->wall == waVinePlant ? 0x60C000 : wcol2;
               c->wall = w; c->wparam = p;
               draw_qfi(c, mscale(V, geom3::WALL), darkena(starcol, fd, 0xFF), PPR::WALL3);
               queuepolyat(mscale(V, geom3::WALL), shWall[ct6], darkena(wcol2, 0, 0xFF), PPR::WALL3A);
               forCellIdEx(c2, i, c)
                 if(placeSidewall(c, i, SIDE_WALL, V, darkena(wcol2, 1, 0xFF))) break;
               }
             pushdown(c, q, V, t*t / 1000000. + t / 1000., true, true);
             }
           }
         if(fa.t_mon) {
           dynamicval<int> d(multi::cpid, fa.pid);
           int t = (ticks - fa.t_mon);
           if(t <= maxtime) {
             erase = false;
             c->stuntime = 0;
             transmatrix V2 = V;
             double footphase = t / 200.0;
             applyAnimation(c, V2, footphase, LAYER_SMALL);
             drawMonsterType(fa.m, c, V2, minf[fa.m].color, footphase);
             pushdown(c, q, V2, t*t / 1000000. + t / 1000., true, true);
             }
           }
         if(erase) fallanims.erase(c);
         }
       }
#endif    

    if(it) {
      if((c->land == laWhirlwind || c->item == itBabyTortoise || c->land == laWestWall) && c->wall != waBoat) {
        double footphase = 0;
        Vboat = &(Vboat0 = *Vboat);
        applyAnimation(c, Vboat0, footphase, LAYER_BOAT);
        }

      if(cellHalfvine(c)) {
        int i =-1;
        for(int t=0;t<6; t++) if(c->move(t) && c->move(t)->wall == c->wall)
          i = t;
    
        Vboat = &(Vboat0 = *Vboat * ddspin(c, i) * xpush(-.13));
        }
    
      error |= drawItemType(it, c, *Vboat, icol, ticks, hidden);
      }

    if(true) {
      #if CAP_SHAPES
      int q = ptds.size();
      #endif
      error |= drawMonster(V, ctype, c, moncol); 
      #if CAP_SHAPES
      if(Vboat != &V && Vboat != &Vboat0 && q != isize(ptds)) 
        pushdown(c, q, V, -geom3::factor_to_lev(zlevel(tC0((*Vboat)))),
          !isMultitile(c->monst), false);
      #endif
      }
      
    #if CAP_SHAPES
    if(!shmup::on && sword::at(c)) {
      queuepolyat(V, shDisk, 0xC0404040, PPR::SWORDMARK);
      }
    #endif

#if CAP_TEXTURE    
    if(!texture::using_aura()) 
#endif 
      addaura(tC0(V), zcol, fd);

    #if CAP_SHAPES
    int ad = airdist(c);
    if(ad == 1 || ad == 2) {

     for(int i=0; i<c->type; i++) {
       cell *c2 = c->move(i); 
       if(airdist(c2) < airdist(c)) {
         calcAirdir(c2); // printf("airdir = %d\n", airdir);
         transmatrix V0 = ddspin(c, i, M_PI);
         
         double ph = ptick(PURE?150:75) + airdir * M_PI / (S21+.0);
         
         int aircol = 0x8080FF00 | int(32 + 32 * -cos(ph));
         
         double ph0 = ph/2;
         ph0 -= floor(ph0/M_PI)*M_PI;

         poly_outline = OUTLINE_TRANS;
         queuepoly((*Vdp)*V0*xpush(hexf*-cos(ph0)), shDisk, aircol);
         poly_outline = OUTLINE_DEFAULT;
         }
       }
      }
    #endif

    #if CAP_SHAPES
    if(c->land == laBlizzard) {
      if(vid.backeffects) {
        if(c->cpdist <= getDistLimit())
          set_blizzard_frame(c, frameid);
        }
      else {
        forCellIdEx(c2, i, c) if(againstWind(c, c2))
          queuepoly(V * ddspin(c, i) * xpush(cellgfxdist(c, i)/2), shWindArrow, 0x8080FF80);
        }
      }
    #endif
    
    if(items[itOrbGravity] && c->cpdist <= 5) 
      draw_gravity_particles(c, V);
    
    #if CAP_SHAPES
    if(c->land == laWhirlwind) {
      whirlwind::calcdirs(c);
      
      for(int i=0; i<whirlwind::qdirs; i++) {
        ld hdir0 = displayspin(c, whirlwind::dfrom[i]) + M_PI;
        ld hdir1 = displayspin(c, whirlwind::dto[i]);
 
        double ph1 = fractick(PURE ? 150 : 75);
        
        int aircol = 0xC0C0FF40;
        
        if(hdir1 < hdir0-M_PI) hdir1 += 2 * M_PI;
        if(hdir1 >= hdir0+M_PI) hdir1 -= 2 * M_PI;
        
        ld hdir = (hdir1*ph1+hdir0*(1-ph1));
 
        transmatrix V0 = spin(hdir);
        
        double ldist = 
          cellgfxdist(c, whirlwind::dfrom[i]) * (1-ph1)/2 + 
          cellgfxdist(c, whirlwind::dto[i]) * ph1/2; 
        // PURE ? crossf : c->type == 6 ? .2840 : 0.3399;
 
        poly_outline = OUTLINE_TRANS;
        queuepoly((*Vdp)*V0*xpush(ldist*(2*ph1-1)), shDisk, aircol);
        poly_outline = OUTLINE_DEFAULT;
        }

      }
    #endif

    #if CAP_QUEUE
    if(error) {
      queuechr(V, 1, ch, darkenedby(asciicol, darken), 2);
      }
    
    if(vid.grid) {
      dynamicval<ld> lw(vid.linewidth, vid.linewidth);

      vid.linewidth *= scalefactor;

      // sphere: 0.3948
      // sphere heptagonal: 0.5739
      // sphere trihepta: 0.3467
      
      // hyper trihepta: 0.2849
      // hyper heptagonal: 0.6150
      // hyper: 0.3798
      
      int prec = sphere ? 3 : 1;
      prec += vid.linequality;
      
      if(0);
      #if CAP_BT
      else if(binarytiling && DIM == 2) {
        ld yx = log(2) / 2;
        ld yy = yx;
        ld xx = 1 / sqrt(2)/2;
        queueline(V * get_horopoint(-yy, xx), V * get_horopoint(yy, 2*xx), gridcolor(c, c->move(binary::bd_right)), prec);
        auto horizontal = [&] (ld y, ld x1, ld x2, int steps, int dir) {
          if(vid.linequality > 0) steps <<= vid.linequality;
          if(vid.linequality < 0) steps >>= -vid.linequality;
          for(int i=0; i<=steps; i++) curvepoint(V * get_horopoint(y, x1 + (x2-x1) * i / steps));
          queuecurve(gridcolor(c, c->move(dir)), 0, PPR::LINE);
          };
        horizontal(yy, 2*xx, xx, 4, binary::bd_up_right);
        horizontal(yy, xx, -xx, 8, binary::bd_up);
        horizontal(yy, -xx, -2*xx, 4, binary::bd_up_left);
        }
      #endif
      #if CAP_BT && MAXDIM == 4
      else if(binarytiling && DIM == 3) {
        binary::queuecube(V, 1, 0xC0C0C080, 0);
        }
      #endif
      else if(isWarped(c) && has_nice_dual()) {
        if(pseudohept(c)) for(int t=0; t<c->type; t++)
          queueline(V * get_warp_corner(c, t%c->type),
                    V * get_warp_corner(c, (t+1)%c->type),
                    gridcolor(c, c->move(t)), prec);
        }
      else {
        for(int t=0; t<c->type; t++)
          if(c->move(t) && c->move(t) < c)
          queueline(V * get_corner_position(c, t%c->type),
                    V * get_corner_position(c, (t+1)%c->type),
                    gridcolor(c, c->move(t)), prec);
        }
      }
    #endif

    if(!euclid) {
      bool usethis = false;
      double spd = 1;
      int side = 0;
      
      if(0);
      
      #if CAP_BT
      else if(binarytiling && conformal::do_rotate >= 2) {
        if(!straightDownSeek || c->master->distance < straightDownSeek->master->distance) {
          usethis = true;
          spd = 1;
          }
        }
      #endif
      
      else if(isGravityLand(cwt.at->land) && cwt.at->land != laMountain) {
        if(cwt.at->land == laDungeon) side = 2;
        if(cwt.at->land == laWestWall) side = 1;
        if(conformal::do_rotate >= 1)
        if(!straightDownSeek || edgeDepth(c) < edgeDepth(straightDownSeek)) {
          usethis = true;
          spd = cwt.at->landparam / 10.;
          }
        }
      
      else if(c->master->alt && cwt.at->master->alt &&
        ((cwt.at->land == laMountain && conformal::do_rotate >= 1)|| 
        (conformal::do_rotate >= 2 && 
          (cwt.at->land == laTemple || cwt.at->land == laWhirlpool || 
          (cheater && (cwt.at->land == laClearing || cwt.at->land == laCaribbean ||
          cwt.at->land == laCamelot || cwt.at->land == laPalace))) 
          ))
        && c->land == cwt.at->land && c->master->alt->alt == cwt.at->master->alt->alt) {
        if(!straightDownSeek || !straightDownSeek->master->alt || celldistAlt(c) < celldistAlt(straightDownSeek)) {
          usethis = true;
          spd = .5;
          if(cwt.at->land == laMountain) side = 2;
          }
        }
  
      else if(conformal::do_rotate >= 2 && cwt.at->land == laOcean && cwt.at->landparam < 25) {
        if(!straightDownSeek || coastval(c, laOcean) < coastval(straightDownSeek, laOcean)) {
          usethis = true;
          spd = cwt.at->landparam / 10;
          }
          
        }

      if(usethis) {
        straightDownSeek = c;
        downspin = atan2(VC0[1], VC0[0]);
        downspin += (side-1) * M_PI/2;
        downspin += conformal::rotation * degree;
        while(downspin < -M_PI) downspin += 2*M_PI;
        while(downspin > +M_PI) downspin -= 2*M_PI;
        downspin = downspin * min(spd, (double)1);
        }
      }
      
  if(!inHighQual) {
    
#if CAP_EDIT
    if((cmode & sm::MAP) && lmouseover && darken == 0 &&
      !mouseout() && 
      (patterns::whichPattern ? patterns::getpatterninfo0(c).id == patterns::getpatterninfo0(lmouseover).id : c == lmouseover)) {
      queuecircle(V, .78, 0x00FFFFFF);
      }

    mapeditor::drawGhosts(c, V, ctype);
#endif
    }
    
    if(vid.grid && c->bardir != NODIR && c->bardir != NOBARRIERS && c->land != laHauntedWall &&
      c->barleft != NOWALLSEP_USED) {
      color_t col = darkena(0x505050, 0, 0xFF);
      queueline(tC0(V), V*tC0(heptmove[c->bardir]), col, 2 + vid.linequality);
      queueline(tC0(V), V*tC0(hexmove[c->bardir]), col, 2 + vid.linequality);
      }
    
#if CAP_MODEL
    netgen::buildVertexInfo(c, V);
#endif
    }
  }

struct flashdata {
  int t;
  int size;
  cell *where;
  double angle;
  int spd; // 0 for flashes, >0 for particles
  color_t color;
  flashdata(int _t, int _s, cell *_w, color_t col, int sped) { 
    t=_t; size=_s; where=_w; color = col; 
    angle = rand() % 1000; spd = sped;
    }
  };

vector<flashdata> flashes;

void drawFlash(cell *c) {
  flashes.push_back(flashdata(ticks, 1000, c, iinf[itOrbFlash].color, 0)); 
  }
void drawBigFlash(cell *c) { 
  flashes.push_back(flashdata(ticks, 2000, c, 0xC0FF00, 0)); 
  }
void drawParticle(cell *c, color_t col, int maxspeed) {
  if(vid.particles && !confusingGeometry())
    flashes.push_back(flashdata(ticks, rand() % 16, c, col, 1+rand() % maxspeed)); 
  }
void drawParticles(cell *c, color_t col, int qty, int maxspeed) { 
  if(vid.particles)
    while(qty--) drawParticle(c,col, maxspeed); 
  }
void drawFireParticles(cell *c, int qty, int maxspeed) { 
  if(vid.particles)
    for(int i=0; i<qty; i++)
      drawParticle(c, firegradient(i / (qty-1.)), maxspeed); 
  }
void fallingFloorAnimation(cell *c, eWall w, eMonster m) {
  if(!wmspatial) return;
  fallanim& fa = fallanims[c];
  fa.t_floor = ticks;
  fa.walltype = w; fa.m = m;
  // drawParticles(c, darkenedby(linf[c->land].color, 1), 4, 50);
  }
void fallingMonsterAnimation(cell *c, eMonster m, int id) {
  if(!mmspatial) return;
  fallanim& fa = fallanims[c];
  fa.t_mon = ticks;
  fa.m = m;
  fa.pid = id;
  // drawParticles(c, darkenedby(linf[c->land].color, 1), 4, 50);
  }

#if CAP_QUEUE
void queuecircleat(cell *c, double rad, color_t col) {
  if(!c) return;
  if(!gmatrix.count(c)) return;
  #if CAP_SHAPES
  if(vid.stereo_mode || sphere) {
    dynamicval<color_t> p(poly_outline, col);
    queuepolyat(gmatrix[c] * spintick(100), shGem[1], 0, PPR::LINE);
    return;
    }
  #endif
  queuecircle(gmatrix[c], rad, col);  
  if(!wmspatial) return;
  if(highwall(c))
    queuecircle(mscale(gmatrix[c], geom3::WALL), rad, col);
  int sl;
  if((sl = snakelevel(c))) {
    queuecircle(mscale(gmatrix[c], geom3::SLEV[sl]), rad, col);
    }
  if(chasmgraph(c))
    queuecircle(mscale(gmatrix[c], geom3::LAKE), rad, col);
  }
#endif

#define G(x) x && gmatrix.count(x)
#define IG(x) if(G(x))
#define Gm(x) gmatrix[x]
#define Gm0(x) tC0(gmatrix[x])

#if ISMOBILE==1
#define MOBON (clicked)
#else
#define MOBON true
#endif

void drawMarkers() {

  if(!(cmode & sm::NORMAL)) return;
  
  callhooks(hooks_markers);
  #if CAP_SHAPES
  viewmat();
  #endif
  
  #if CAP_QUEUE
  for(cell *c1: crush_now) 
    queuecircleat(c1, .8, darkena(minf[moCrusher].color, 0, 0xFF));
  #endif

  if(!inHighQual) {

    bool ok = !ISPANDORA || mousepressed;
    
    ignore(ok);
     
    #if CAP_QUEUE
    if(G(dragon::target) && haveMount()) {
      queuechr(Gm0(dragon::target), 2*vid.fsize, 'X', 
        gradient(0, iinf[itOrbDomination].color, -1, sintick(dragon::whichturn == turncount ? 75 : 150), 1));
      }
    #endif

    /* for(int i=0; i<12; i++) if(c->type == 5 && c->master == &dodecahedron[i])
      queuechr(xc, yc, sc, 4*vid.fsize, 'A'+i, iinf[itOrbDomination].color); */
    
    if(1) {
      using namespace yendor;
      if(yii < isize(yi) && !yi[yii].found) {
        cell *keycell = NULL;
        int i;
        for(i=0; i<YDIST; i++) 
          if(yi[yii].path[i]->cpdist <= get_sightrange_ambush()) {
            keycell = yi[yii].path[i];
            break;
            }
        if(keycell) {
          for(; i<YDIST; i++) {
            cell *c = yi[yii].path[i];
            if(inscreenrange(c))
              keycell = c;
            }
          hyperpoint H = tC0(ggmatrix(keycell));
          #if CAP_QUEUE
          queuechr(H, 2*vid.fsize, 'X', 0x10101 * int(128 + 100 * sintick(150)));
          queuestr(H, vid.fsize, its(celldistance(cwt.at, yi[yii].key())), 0x10101 * int(128 - 100 * sintick(150)));
          #endif
          addauraspecial(H, iinf[itOrbYendor].color, 0);
          }
        }
      }
  
    #if CAP_RACING
    racing::markers();
    #endif
  
    #if CAP_QUEUE        
    if(lmouseover && vid.drawmousecircle && ok && DEFAULTCONTROL && MOBON) {
      queuecircleat(lmouseover, .8, darkena(lmouseover->cpdist > 1 ? 0x00FFFF : 0xFF0000, 0, 0xFF));
      }

    if(global_pushto && vid.drawmousecircle && ok && DEFAULTCONTROL && MOBON) {
      queuecircleat(global_pushto, .6, darkena(0xFFD500, 0, 0xFF));
      }
    #endif

#if CAP_SDLJOY && CAP_QUEUE
    if(joydir.d >= 0) 
      queuecircleat(cwt.at->modmove(joydir.d+cwt.spin), .78 - .02 * sintick(199), 
        darkena(0x00FF00, 0, 0xFF));
#endif

    bool m = true;
    ignore(m);
#if CAP_MODEL
    m = netgen::mode == 0;
#endif

    #if CAP_QUEUE
    if(centerover.at && !playermoved && m && !anims::any_animation())
      queuecircleat(centerover.at, .70 - .06 * sintick(200), 
        darkena(int(175 + 25 * sintick(200)), 0, 0xFF));

    if(multi::players > 1 || multi::alwaysuse) for(int i=0; i<numplayers(); i++) {
      multi::cpid = i;
      if(multi::players == 1) multi::player[i] = cwt;
      cell *ctgt = multi::multiPlayerTarget(i);
      queuecircleat(ctgt, .40 - .06 * sintick(200, i / numplayers()), getcs().uicolor);
      }
    #endif

    // process mouse
    #if CAP_SHAPES
    if((vid.axes == 4 || (vid.axes == 1 && !mousing)) && !shmup::on) {
      if(multi::players == 1) {
        forCellIdAll(c2, d, cwt.at) IG(c2) drawMovementArrows(c2, confusingGeometry() ? Gm(cwt.at) * calc_relative_matrix(c2, cwt.at, d) : Gm(c2));
        }
      else if(multi::players > 1) for(int p=0; p<multi::players; p++) {
        if(multi::playerActive(p) && (vid.axes == 4 || !drawstaratvec(multi::mdx[p], multi::mdy[p]))) 
        forCellIdAll(c2, d, multi::player[p].at) IG(c2) {
          multi::cpid = p;
          dynamicval<transmatrix> ttm(cwtV, multi::whereis[p]);
          dynamicval<cellwalker> tcw(cwt, multi::player[p]);
          drawMovementArrows(c2, confusingGeometry() ? Gm(cwt.at) * calc_relative_matrix(c2, cwt.at, d) : Gm(c2));
          }
        }
      }
    #endif
    }

  monsterToSummon = moNone;
  orbToTarget = itNone;

  if(mouseover && targetclick) {
    shmup::cpid = 0;
    orbToTarget = targetRangedOrb(mouseover, roCheck);
    #if CAP_QUEUE
    if(orbToTarget == itOrbSummon) {
      monsterToSummon = summonedAt(mouseover);
      queuechr(mousex, mousey, 0, vid.fsize, minf[monsterToSummon].glyph, minf[monsterToSummon].color);
      queuecircleat(mouseover, 0.6, darkena(minf[monsterToSummon].color, 0, 0xFF));
      }
    else if(orbToTarget) {
      queuechr(mousex, mousey, 0, vid.fsize, '@', iinf[orbToTarget].color);
      queuecircleat(mouseover, 0.6, darkena(iinf[orbToTarget].color, 0, 0xFF));
      }
    #endif
    #if CAP_SHAPES
    if(orbToTarget && rand() % 200 < ticks - lastt) {
      if(orbToTarget == itOrbDragon)
        drawFireParticles(mouseover, 2);
      else if(orbToTarget == itOrbSummon) {
        drawParticles(mouseover, iinf[orbToTarget].color, 1);
        drawParticles(mouseover, minf[monsterToSummon].color, 1);
        }
      else {
        drawParticles(mouseover, iinf[orbToTarget].color, 2);
        }
      }
    if(items[itOrbAir] && mouseover->cpdist > 1) {
      cell *c1 = mouseover;
      for(int it=0; it<10; it++) {
        int di;
        cell *c2 = blowoff_destination(c1, di);
        if(!c2) break;
        transmatrix T1 = ggmatrix(c1);
        transmatrix T2 = ggmatrix(c2);
        transmatrix T = T1 * rspintox(inverse(T1)*T2*C0) * xpush(hdist(T1*C0, T2*C0) * fractick(50, 0));
        color_t aircol = (orbToTarget == itOrbAir ? 0x8080FF40 : 0x8080FF20);
        queuepoly(T, shDisk, aircol);
        c1 = c2;
        }
      }
    #endif
    }  
  }

void drawFlashes() {
  #if CAP_QUEUE
  for(int k=0; k<isize(flashes); k++) {
    flashdata& f = flashes[k];
    transmatrix V;
    
    if(f.spd) try { V = gmatrix.at(f.where); } catch(out_of_range&) { 
      f = flashes[isize(flashes)-1];
      flashes.pop_back(); k--;
      continue; 
      }
    else V = ggmatrix(f.where);

    int tim = ticks - f.t;
    
    bool kill = tim > f.size;
    
    if(f.spd) {
      #if CAP_SHAPES
      kill = tim > 300;
      int partcol = darkena(f.color, 0, max(255 - tim*255/300, 0));
      poly_outline = OUTLINE_DEFAULT;
      queuepoly(V * spin(f.angle) * xpush(f.spd * tim * scalefactor / 50000.), shParticle[f.size], partcol);
      #endif
      }
    
    else if(f.size == 1000) {
      for(int u=0; u<=tim; u++) {
        if((u-tim)%50) continue;
        if(u < tim-150) continue;
        ld rad = u * 3 / 1000.;
        rad = rad * (5-rad) / 2;
        rad *= hexf;
        int flashcol = f.color;
        if(u > 500) flashcol = gradient(flashcol, 0, 500, u, 1100);
        flashcol = darkena(flashcol, 0, 0xFF);
        PRING(a) curvepoint(V*xspinpush0(a * M_PI / S42, rad));
        queuecurve(flashcol, 0x8080808, PPR::LINE);
        }
      }
    else if(f.size == 2000) {
      for(int u=0; u<=tim; u++) {
        if((u-tim)%50) continue;
        if(u < tim-250) continue;
        ld rad = u * 3 / 2000.;
        rad = rad * (5-rad) * 1.25;
        rad *= hexf;
        int flashcol = f.color;
        if(u > 1000) flashcol = gradient(flashcol, 0, 1000, u, 2200);
        flashcol = darkena(flashcol, 0, 0xFF);
        PRING(a) curvepoint(V*xspinpush0(a * M_PI / S42, rad));
        queuecurve(flashcol, 0x8080808, PPR::LINE);
        }
      }

    if(kill) {
      f = flashes[isize(flashes)-1];
      flashes.pop_back(); k--;
      }
    }
  #endif
  }

bool allowIncreasedSight() {
  if(cheater || autocheat) return true;
  if(peace::on) return true;
#if CAP_TOUR
  if(tour::on) return true;
#endif
  if(randomPatternsMode) return true;
  if(racing::on) return true;
  if(quotient || !hyperbolic || archimedean) return true;
  return false;
  }

bool allowChangeRange() {
  if(cheater || peace::on || randomPatternsMode) return true;
#if CAP_TOUR
  if(tour::on) return true;
#endif
  if(racing::on) return true;
  if(sightrange_bonus >= 0) return true;
  if(archimedean) return true;
  return false;
  }

purehookset hooks_drawmap;

transmatrix cview() {
  sphereflip = Id;
  if(sphereflipped()) sphereflip[DIM][DIM] = -1;
  return ypush(vid.yshift) * sphereflip * View;
  }

void precise_mouseover() {
  if(!mouseover) return;
  cell *omouseover = mouseover;
  for(int loop = 0; loop < 10; loop++) { 
    bool found = false;
    if(!gmatrix.count(mouseover)) return;
    hyperpoint r_mouseh = inverse(gmatrix[mouseover]) * mouseh;
    for(int i=0; i<mouseover->type; i++) {
      hyperpoint h1 = get_corner_position(mouseover, (i+mouseover->type-1) % mouseover->type);
      hyperpoint h2 = get_corner_position(mouseover, i);
      using namespace hyperpoint_vec;
      hyperpoint hx = r_mouseh - h1;
      h2 = h2 - h1;
      ld z = h2[1] * hx[0] - h2[0] * hx[1];
      ld z0 = h2[1] * h1[0] - h2[0] * h1[1];
      if(z * z0 > 0) {
        mouseover2 = mouseover;
        mouseover = mouseover->move(i);
        found = true;
        break;
        }
      }
    if(!found) return;
    }
  // probably some error... just return the original
  mouseover = omouseover;
  }

void drawthemap() {

  callhooks(hooks_drawmap);

  frameid++;
  cells_drawn = 0;
  
  wavephase = (-(ticks / 100)) & 7;

  if(sightrange_bonus > 0 && !allowIncreasedSight()) 
    sightrange_bonus = 0;
  
  profile_frame();
  profile_start(0);
  swap(gmatrix0, gmatrix);
  gmatrix.clear();

  wmspatial = vid.wallmode == 4 || vid.wallmode == 5;
  wmescher = vid.wallmode == 3 || vid.wallmode == 5;
  wmplain = vid.wallmode == 2 || vid.wallmode == 4;
  wmascii = vid.wallmode == 0;
  wmblack = vid.wallmode == 1;
  
  mmitem = vid.monmode >= 1;
  mmmon = vid.monmode >= 2;
  mmhigh = vid.monmode == 3 || vid.monmode == 5;
  mmspatial = vid.monmode == 4 || vid.monmode == 5;
  
  spatial_graphics = wmspatial || mmspatial;
  #if CAP_RUG
  if(rug::rugged && !rug::spatial_rug) spatial_graphics = false;
  #endif
  if(non_spatial_model())
    spatial_graphics = false;
  if(pmodel == mdDisk && abs(vid.alpha) < 1e-6) spatial_graphics = false;
  
  if(!spatial_graphics) wmspatial = mmspatial = false;

  DEBB(DF_GRAPH, (debugfile,"draw the map\n"));
  
  for(int m=0; m<motypes; m++) if(isPrincess(eMonster(m))) 
    minf[m].name = princessgender() ? "Princess" : "Prince";
    
  iinf[itSavedPrincess].name = minf[moPrincess].name;

  for(int i=0; i<NUM_GS; i++) {
    genderswitch_t& g = genderswitch[i];
    if(g.gender != princessgender()) continue;
    minf[g.m].help = g.desc;
    minf[g.m].name = g.name;
    }

  if(mapeditor::autochoose) mapeditor::ew = mapeditor::ewsearch;
  mapeditor::ewsearch.dist = 1e30;
  modist = 1e20; mouseover = NULL; 
  modist2 = 1e20; mouseover2 = NULL; 

  compute_graphical_distance();

  centdist = 1e20; 
  if(!masterless) centerover.at = NULL; 

  for(int i=0; i<multi::players; i++) {
    multi::ccdist[i] = 1e20; multi::ccat[i] = NULL;
    }

  straightDownSeek = NULL; downspin = 0;

  #if ISMOBILE
  mouseovers = XLAT("No info about this...");
  #endif
  if(mouseout()) 
    modist = -5;
  playerfound = false;
  // playerfoundL = false;
  // playerfoundR = false;
  
  arrowtraps.clear();

  profile_start(1);
  if(masterless)
    drawEuclidean();
  #if CAP_BT
  else if(binarytiling)
    binary::draw();
  #endif
  #if CAP_ARCM
  else if(archimedean)
    arcm::draw();
  #endif
  else
    drawStandard();
  drawWormSegments();
  drawBlizzards();
  drawArrowTraps();
  
  precise_mouseover();
  
  ivoryz = false;
  
  linepatterns::drawAll();
  
  callhooks(hooks_frame);
  
  profile_stop(1);
  profile_start(4);
  drawMarkers();
  profile_stop(4);
  drawFlashes();
  
  if(multi::players > 1 && !shmup::on) {
    if(shmup::centerplayer != -1) 
      cwtV = multi::whereis[shmup::centerplayer];
    else {
      hyperpoint h;
      for(int i=0; i<3; i++) h[i] = 0;
      for(int p=0; p<multi::players; p++) if(multi::playerActive(p)) {
        hyperpoint h1 = tC0(multi::whereis[p]);
        for(int i=0; i<3; i++) h[i] += h1[i];
        }
      h = mid(h, h);
      cwtV = rgpushxto0(h);
      }
    }
  
  if(shmup::on) {
    if(shmup::players == 1)
      cwtV = shmup::pc[0]->pat;
    else if(shmup::centerplayer != -1) 
      cwtV = shmup::pc[shmup::centerplayer]->pat;
    else {
      hyperpoint h;
      for(int i=0; i<3; i++) h[i] = 0;
      for(int p=0; p<multi::players; p++) {
        hyperpoint h1 = tC0(shmup::pc[p]->pat);
        for(int i=0; i<3; i++) h[i] += h1[i];
        }
      h = mid(h, h);
      cwtV = rgpushxto0(h);
      }
    }

  #if CAP_SDL
  Uint8 *keystate = SDL_GetKeyState(NULL);
  lmouseover = mouseover;
  bool useRangedOrb = (!(vid.shifttarget & 1) && haveRangedOrb() && lmouseover && lmouseover->cpdist > 1) || (keystate[SDLK_RSHIFT] | keystate[SDLK_LSHIFT]);
  if(!useRangedOrb && !(cmode & sm::MAP) && !(cmode & sm::DRAW) && DEFAULTCONTROL && !mouseout()) {
    dynamicval<eGravity> gs(gravity_state, gravity_state);
    void calcMousedest();
    calcMousedest();
    cellwalker cw = cwt; bool f = flipplayer;
    items[itWarning]+=2;
    
    bool recorduse[ittypes];
    for(int i=0; i<ittypes; i++) recorduse[i] = orbused[i];
    movepcto(mousedest.d, mousedest.subdir, true);
    for(int i=0; i<ittypes; i++) orbused[i] = recorduse[i];
    items[itWarning] -= 2;
    if(cw.spin != cwt.spin) mirror::act(-mousedest.d, mirror::SPINSINGLE);
    cwt = cw; flipplayer = f;
    lmouseover = mousedest.d >= 0 ? cwt.at->modmove(cwt.spin + mousedest.d) : cwt.at;
    }
  #endif
  profile_stop(0);
  }

void drawmovestar(double dx, double dy) {

  if(viewdists) return;

  DEBB(DF_GRAPH, (debugfile,"draw movestar\n"));
  if(!playerfound) return;
  
  if(shmup::on) return;
#if CAP_RUG
  if(rug::rugged && multi::players == 1 && !multi::alwaysuse) return;
#endif

  hyperpoint H = tC0(cwtV);
  ld R = sqrt(H[0] * H[0] + H[1] * H[1]);
  transmatrix Centered = Id;

  if(masterless) 
    Centered = eupush(H);
  else if(R > 1e-9) Centered = rgpushxto0(H);
  
  Centered = Centered * rgpushxto0(hpxy(dx*5, dy*5));
  if(multi::cpid >= 0) multi::crosscenter[multi::cpid] = Centered;
  
  int rax = vid.axes;
  if(rax == 1) rax = drawstaratvec(dx, dy) ? 2 : 0;
  
  if(rax == 0 || vid.axes == 4) return;

  int starcol = getcs().uicolor;
  ignore(starcol);
  
  if(0);

#if CAP_SHAPES
  else if(vid.axes == 3)
    queuepoly(Centered, shMovestar, starcol);
#endif
  
  else for(int d=0; d<8; d++) {
#if CAP_QUEUE
    color_t col = starcol;
#if ISPANDORA
    if(leftclick && (d == 2 || d == 6 || d == 1 || d == 7)) col &= 0xFFFFFF3F;
    if(rightclick && (d == 2 || d == 6 || d == 3 || d == 5)) col &= 0xFFFFFF3F;
    if(!leftclick && !rightclick && (d&1)) col &= 0xFFFFFF3F;
#endif
//  EUCLIDEAN

    queueline(tC0(Centered), Centered * xspinpush0(d * M_PI / 4, euclid ? 0.5 : d==0?.7:d==2?.5:.2), col, 3 + vid.linequality);
#endif
    }
  }

// old style joystick control

bool dronemode;

purehookset hooks_calcparam;

void calcparam() {

  auto cd = current_display;
  
  DEBB(DF_GRAPH, (debugfile,"calc param\n"));
  cd->xtop = vid.xres * cd->xmin;
  cd->ytop = vid.yres * cd->ymin;
  
  cd->xsize = vid.xres * (cd->xmax - cd->xmin);
  cd->ysize = vid.yres * (cd->ymax - cd->ymin);

  cd->xcenter = cd->xtop + cd->xsize / 2;
  cd->ycenter = cd->ytop + cd->ysize / 2;

  if(vid.scale > -1e-2 && vid.scale < 1e-2) vid.scale = 1;
  
  ld realradius = min(cd->xsize / 2, cd->ysize / 2);
  
  cd->scrsize = realradius - (inHighQual ? 0 : ISANDROID ? 2 : ISIOS ? 40 : 40);

  current_display->sidescreen = false;
  
  if(vid.xres < vid.yres - 2 * vid.fsize && !inHighQual) {
    cd->ycenter = vid.yres - cd->scrsize - vid.fsize;
    }
  else {
    if(vid.xres > vid.yres * 4/3+16 && (cmode & sm::SIDE))
      current_display->sidescreen = true;
#if CAP_TOUR
    if(tour::on && (tour::slides[tour::currentslide].flags & tour::SIDESCREEN))
      current_display->sidescreen = true;
#endif

    if(current_display->sidescreen) cd->xcenter = vid.yres/2;
    }

  cd->radius = vid.scale * cd->scrsize;  
  realradius = min(realradius, cd->radius);
  
  if(dronemode) { cd->ycenter -= cd->radius; cd->ycenter += vid.fsize/2; cd->ycenter += vid.fsize/2; cd->radius *= 2; }
  
  cd->xcenter += cd->scrsize * vid.xposition;
  cd->ycenter += cd->scrsize * vid.yposition;

  cd->tanfov = tan(vid.fov * degree / 2);
  
  if(pmodel) 
    cd->scrdist = vid.xres / 2 / cd->tanfov;
  else 
    cd->scrdist = cd->radius;
  cd->scrdist_text = cd->scrdist;

  callhooks(hooks_calcparam);
  }

function<void()> wrap_drawfullmap = drawfullmap;

bool force_sphere_outline = false;

void drawfullmap() {

  DEBB(DF_GRAPH, (debugfile,"draw full map\n"));
    
  ptds.clear();

  
  /*
  if(conformal::on) {
    char ch = 'A';
    for(auto& v: conformal::v) {
      queuepoly(ggmatrix(v->base) * v->at, shTriangle, 0x306090C0);
      queuechr(ggmatrix(v->base) * v->at * C0, 10, ch++, 0xFF0000);
      }      
    }
  */
  
  #if CAP_QUEUE
  draw_boundary(0);
  draw_boundary(1);
  
  draw_model_elements();
  #endif
  
  /* if(vid.wallmode < 2 && !euclid && !patterns::whichShape) {
    int ls = isize(lines);
    if(ISMOBILE) ls /= 10;
    for(int t=0; t<ls; t++) queueline(View * lines[t].P1, View * lines[t].P2, lines[t].col >> (darken+1));
    } */

  clearaura();
  if(!nomap) drawthemap();
  if(!inHighQual) {
    if((cmode & sm::NORMAL) && !rug::rugged) {
      if(multi::players > 1) {
        transmatrix bcwtV = cwtV;
        for(int i=0; i<multi::players; i++) if(multi::playerActive(i))
          cwtV = multi::whereis[i], multi::cpid = i, drawmovestar(multi::mdx[i], multi::mdy[i]);
        cwtV = bcwtV;
        }
      else if(multi::alwaysuse)
        drawmovestar(multi::mdx[0], multi::mdy[0]);
      else 
        drawmovestar(0, 0);
      }
#if CAP_EDIT
    if(cmode & sm::DRAW) mapeditor::drawGrid();
#endif
    }
  profile_start(2);

  drawaura();
  #if CAP_QUEUE
  drawqueue();
  #endif
  profile_stop(2);
  }

void gamescreen(int _darken) {

  if(subscreen_split([=] () {
    calcparam();
    current_display->set_projection(0, false);
    current_display->set_viewport(0);
    compute_graphical_distance();
    gamescreen(_darken);
    })) {
    current_display->set_projection(0, false);
    current_display->set_viewport(0);    
    return;
    }

  if((cmode & sm::MAYDARK) && !current_display->sidescreen) {
    _darken += 2;
    }

  darken = _darken;
  
  if(conformal::includeHistory) conformal::restore();

  anims::apply();
#if CAP_RUG
  if(rug::rugged) {
    rug::actDraw();
    } else
#endif
  wrap_drawfullmap();
  anims::rollback();

  if(conformal::includeHistory) conformal::restoreBack();

  poly_outline = OUTLINE_DEFAULT;
  
  #if ISMOBILE
  
  buttonclicked = false;
  
  if((cmode & sm::NORMAL) && vid.stereo_mode != sLR) {
    if(andmode == 0 && shmup::on) {
      using namespace shmupballs;
      calc();
      drawCircle(xmove, yb, rad, OUTLINE_FORE);
      drawCircle(xmove, yb, rad/2, OUTLINE_FORE);
      drawCircle(xfire, yb, rad, 0xFF0000FF);
      drawCircle(xfire, yb, rad/2, 0xFF0000FF);
      }
    else {
      if(!haveMobileCompass()) displayabutton(-1, +1, XLAT(andmode == 0 && useRangedOrb ? "FIRE" : "MOVE"),  andmode == 0 ? BTON : BTOFF);
      displayabutton(+1, +1, rug::rugged ? "RUG" : XLAT(andmode == 1 ? "BACK" : "DRAG"),  andmode == 1 ? BTON : BTOFF);
      }
    displayabutton(-1, -1, XLAT("INFO"),  andmode == 12 ? BTON : BTOFF);
    displayabutton(+1, -1, XLAT("MENU"), andmode == 3 ? BTON : BTOFF);
    }
  
  #endif
  
  darken = 0;

#if CAP_TEXTURE
  if(texture::config.tstate == texture::tsAdjusting) 
    texture::config.drawRawTexture();
#endif
  }

bool nohelp;

void normalscreen() {
  help = "@";

  mouseovers = XLAT("Press F1 or right click for help");

#if CAP_TOUR  
  if(tour::on) mouseovers = tour::tourhelp;
#endif

  if(!outofmap(mouseh)) getcstat = '-';
  cmode = sm::NORMAL | sm::DOTOUR | sm::CENTER;
  if(viewdists && show_distance_lists) cmode |= sm::SIDE | sm::MAYDARK;
  gamescreen(hiliteclick && mmmon ? 1 : 0); drawStats();
  if(nomenukey || ISMOBILE)
    ;
#if CAP_TOUR
  else if(tour::on) 
    displayButton(vid.xres-8, vid.yres-vid.fsize, XLAT("(ESC) tour menu"), SDLK_ESCAPE, 16);
  else
#endif
    displayButton(vid.xres-8, vid.yres-vid.fsize, XLAT("(v) menu"), 'v', 16);
  keyhandler = handleKeyNormal;

  if(!playerfound && !anims::any_on())
    displayButton(current_display->xcenter, current_display->ycenter, XLAT(mousing ? "find the player" : "press SPACE to find the player"), ' ', 8);

  describeMouseover();
  }

vector< function<void()> > screens = { normalscreen };

int cmode;

void drawscreen() {

  if(vid.xres == 0 || vid.yres == 0) return;

  DEBB(DF_GRAPH, (debugfile,"drawscreen\n"));

  calcparam();
  // rug::setVidParam();

#if CAP_GL
  if(vid.usingGL) setGLProjection();
#endif
  
  #if CAP_SDL
  // SDL_LockSurface(s);
  // unsigned char *b = (unsigned char*) s->pixels;
  // int n = vid.xres * vid.yres * 4;
  // while(n) *b >>= 1, b++, n--;
  // memset(s->pixels, 0, vid.xres * vid.yres * 4);
  #if CAP_GL
  if(!vid.usingGL) 
  #endif
    SDL_FillRect(s, NULL, backcolor);
  #endif
   
  // displaynum(vx,100, 0, 24, 0xc0c0c0, celldist(cwt.at), ":");
  
  lgetcstat = getcstat;
  getcstat = 0; inslider = false;
  
  mouseovers = " ";

  cmode = 0;
  keyhandler = [] (int sym, int uni) {};
  #if CAP_SDL
  joyhandler = [] (SDL_Event& ev) { return false; };
  #endif
  if(!isize(screens)) pushScreen(normalscreen);
  screens.back()();

#if !ISMOBILE
  color_t col = linf[cwt.at->land].color;
  if(cwt.at->land == laRedRock) col = 0xC00000;
  if(!nohelp)
    displayfr(vid.xres/2, vid.fsize,   2, vid.fsize, mouseovers, col, 8);
#endif

  drawmessages();
  
  bool normal = cmode & sm::NORMAL;
  
  if((havewhat&HF_BUG) && darken == 0 && normal) for(int k=0; k<3; k++)
    displayfr(vid.xres/2 + vid.fsize * 5 * (k-1), vid.fsize*2,   2, vid.fsize, 
      its(hive::bugcount[k]), minf[moBug0+k].color, 8);
    
  bool minefieldNearby = false;
  int mines[4], tmines=0;
  for(int p=0; p<numplayers(); p++) {
    mines[p] = 0;
    cell *c = playerpos(p);
    if(!c) continue;
    for(int i=0; i<c->type; i++) if(c->move(i)) {
      if(c->move(i)->land == laMinefield) 
        minefieldNearby = true;
      if(c->move(i)->wall == waMineMine) {
        bool ep = false;
        if(!ep) mines[p]++, tmines++;
        }
      }
    }

  if((minefieldNearby || tmines) && !items[itOrbAether] && !last_gravity_state && darken == 0 && normal) {
    string s;
    if(tmines > 7) tmines = 7;
    color_t col = minecolors[tmines];
    
    if(tmines == 7) seenSevenMines = true;
    
    for(int p=0; p<numplayers(); p++) if(multi::playerActive(p))
      displayfr(vid.xres * (p+.5) / numplayers(),
        current_display->ycenter - current_display->radius * 3/4, 2,
        vid.fsize, 
        XLAT(minetexts[mines[p]]), minecolors[mines[p]], 8);

    if(minefieldNearby && !shmup::on && cwt.at->land != laMinefield && cwt.peek()->land != laMinefield) {
      displayfr(vid.xres/2, current_display->ycenter - current_display->radius * 3/4 - vid.fsize*3/2, 2,
        vid.fsize, 
        XLAT("WARNING: you are entering a minefield!"), 
        col, 8);
      }
    }

  // SDL_UnlockSurface(s);

  glflush();
  DEBT("swapbuffers");
#if CAP_SDL
#if CAP_GL
  if(vid.usingGL) SDL_GL_SwapBuffers(); else
#endif
  SDL_UpdateRect(s, 0, 0, vid.xres, vid.yres);
#endif
  
//printf("\ec");
  }

void restartGraph() {
  DEBB(DF_INIT, (debugfile,"restartGraph\n"));
  
  View = Id;
  if(!autocheat) linepatterns::clearAll();
  if(currentmap) {
    if(masterless) {
      centerover = vec_to_cellwalker(0);
      }
    else {
      viewctr.at = currentmap->getOrigin();
      viewctr.spin = 0;
      viewctr.mirrored = false;
      }
    if(sphere) View = spin(-M_PI/2);
    }
  }

void clearAnimations() {
  for(int i=0; i<ANIMLAYERS; i++) animations[i].clear();
  flashes.clear();
  fallanims.clear();
  }
  
auto graphcm = addHook(clearmemory, 0, [] () {
  DEBB(DF_INIT, (debugfile,"clear graph memory\n"));
  mouseover = centerover.at = lmouseover = NULL;  
  gmatrix.clear(); gmatrix0.clear();
  clearAnimations();
  });

void resetGeometry() {
  precalc();
#if CAP_FIELD
  if(hyperbolic && &currfp != &fieldpattern::fp_invalid) currfp.analyze();
#endif
#if CAP_GL
  resetGL();
#endif
  }

//=== animation

map<cell*, animation> animations[ANIMLAYERS];

int revhint(cell *c, int hint) {
  if(hint >= 0 && hint < c->type) return c->c.spin(hint);
  else return hint;
  }

bool compute_relamatrix(cell *src, cell *tgt, int direction_hint, transmatrix& T) {
  if(confusingGeometry()) {
    T = calc_relative_matrix(src, tgt, revhint(src, direction_hint));
    }
  else {
    if(gmatrix.count(src) && gmatrix.count(tgt))
      T = inverse(gmatrix[tgt]) * gmatrix[src];
    else
      return false;
    }
  return true;
  }


void animateMovement(cell *src, cell *tgt, int layer, int direction_hint) {
  if(vid.mspeed >= 5) return; // no animations!
  transmatrix T;
  if(!compute_relamatrix(src, tgt, direction_hint, T)) return;
  animation& a = animations[layer][tgt];
  if(animations[layer].count(src)) {
    a = animations[layer][src];
    a.wherenow = T * a.wherenow;
    animations[layer].erase(src);
    a.attacking = 0;
    }
  else {
    a.ltick = ticks;
    a.wherenow = T;
    a.footphase = 0;
    }
  }

void animateAttack(cell *src, cell *tgt, int layer, int direction_hint) {
  if(vid.mspeed >= 5) return; // no animations!
  transmatrix T;
  if(!compute_relamatrix(src, tgt, direction_hint, T)) return;
  bool newanim = !animations[layer].count(src);
  animation& a = animations[layer][src];
  a.attacking = 1;
  a.attackat = rspintox(tC0(inverse(T))) * xpush(hdist0(T*C0) / 3);
  if(newanim) a.wherenow = Id, a.ltick = ticks, a.footphase = 0;
  }

vector<pair<cell*, animation> > animstack;

void indAnimateMovement(cell *src, cell *tgt, int layer, int direction_hint) {
  if(vid.mspeed >= 5) return; // no animations!
  if(animations[layer].count(tgt)) {
    animation res = animations[layer][tgt];
    animations[layer].erase(tgt);
    animateMovement(src, tgt, layer, direction_hint);
    if(animations[layer].count(tgt)) 
      animstack.push_back(make_pair(tgt, animations[layer][tgt]));
    animations[layer][tgt] = res;
    }
  else {
    animateMovement(src, tgt, layer, direction_hint);
    if(animations[layer].count(tgt)) {
      animstack.push_back(make_pair(tgt, animations[layer][tgt]));
      animations[layer].erase(tgt);
      }
    }
  }

void commitAnimations(int layer) {
  for(int i=0; i<isize(animstack); i++)
    animations[layer][animstack[i].first] = animstack[i].second;
  animstack.clear();
  }

void animateReplacement(cell *a, cell *b, int layer, int direction_hinta, int direction_hintb) {
  if(vid.mspeed >= 5) return; // no animations!
  static cell c1;
  gmatrix[&c1] = gmatrix[b];
  if(animations[layer].count(b)) animations[layer][&c1] = animations[layer][b];
  animateMovement(a, b, layer, direction_hinta);
  animateMovement(&c1, a, layer, direction_hintb);
  }

void drawBug(const cellwalker& cw, color_t col) {
#if CAP_SHAPES
  initquickqueue();
  transmatrix V = ggmatrix(cw.at);
  if(cw.spin) V = V * ddspin(cw.at, cw.spin, M_PI);
  queuepoly(V, shBugBody, col);
  quickqueue();
#endif
  }

cell *viewcenter() {
  if(masterless) return centerover.at;
  else return viewctr.at->c7;
  }

bool inscreenrange(cell *c) {
  if(sphere) return true;
  if(euclid) return celldistance(viewcenter(), c) <= get_sightrange_ambush();
  return heptdistance(viewcenter(), c) <= 8;
  }

}
