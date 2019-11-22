// Hyperbolic Rogue -- main graphics file
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file graph.cpp
 *  \brief Drawing cells, monsters, items, etc.
 */

#include "hyper.h"
namespace hr {

EX int last_firelimit;
EX int firelimit;

EX int inmirrorcount = 0;

/** wall optimization: do not draw things beyond walls */
EX bool wallopt;

EX bool spatial_graphics;
EX bool wmspatial, wmescher, wmplain, wmblack, wmascii;
EX bool mmspatial, mmhigh, mmmon, mmitem;

EX int detaillevel = 0;

EX bool first_cell_to_draw = true;

EX bool in_perspective() {
  return among(pmodel, mdPerspective, mdGeodesic);
  }

EX bool hide_player() {
  return GDIM == 3 && playermoved && vid.yshift == 0 && vid.sspeed > -5 && in_perspective() && (first_cell_to_draw || elliptic) && (WDIM == 3 || vid.camera == 0) && !inmirrorcount
#if CAP_RACING     
   && !(racing::on && !racing::use_standard_centering() && !racing::player_relative)
#endif
     ;
  }

EX hookset<bool(int sym, int uni)> *hooks_handleKey;
EX hookset<bool(cell *c, const transmatrix& V)> *hooks_drawcell;
EX purehookset hooks_frame, hooks_markers;

EX ld animation_factor = 1;
EX int animation_lcm = 0;

EX ld ptick(int period, ld phase IS(0)) {
  if(animation_lcm) animation_lcm = animation_lcm * (period / gcd(animation_lcm, period));
  return (ticks * animation_factor) / period + phase * 2 * M_PI;
  }

EX ld fractick(int period, ld phase IS(0)) {
  ld t = ptick(period, phase) / 2 / M_PI;
  t -= floor(t);
  if(t<0) t++;
  return t;
  }
  
EX ld sintick(int period, ld phase IS(0)) {
  return sin(ptick(period, phase));
  }

EX transmatrix spintick(int period, ld phase IS(0)) {
  return spin(ptick(period, phase));
  }

#define WOLNIEJ 1
#define BTOFF 0x404040
#define BTON  0xC0C000

// #define PANDORA

int colorbar;

EX bool inHighQual; // taking high quality screenshot
EX bool auraNOGL;    // aura without GL

// 
int axestate;

EX int ticks;
EX int frameid;

EX bool camelotcheat;
EX bool nomap;

EX eItem orbToTarget;
EX eMonster monsterToSummon;

EX int sightrange_bonus = 0;

EX string mouseovers;

EX int darken = 0;

struct fallanim {
  int t_mon, t_floor, pid;
  eWall walltype;
  eMonster m;
  fallanim() { t_floor = 0; t_mon = 0; pid = 0; walltype = waNone; }
  };

map<cell*, fallanim> fallanims;

EX bool doHighlight() {
  return (hiliteclick && darken < 2) ? !mmhigh : mmhigh;
  }

int dlit;

ld spina(cell *c, int dir) {
  return 2 * M_PI * dir / c->type;
  }

// cloak color 
EX int cloakcolor(int rtr) {
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
  
EX int firecolor(int phase IS(0), int mul IS(1)) {
  return gradient(0xFFFF00, 0xFF0000, -1, sintick(100*mul, phase/200./M_PI), 1);
  }

EX int watercolor(int phase) {
  return 0x0080C0FF + 256 * int(63 * sintick(50, phase/100./M_PI));
  }

EX int aircolor(int phase) {
  return 0x8080FF00 | int(32 + 32 * sintick(200, phase * 1. / cgi.S21));
  }

EX int fghostcolor(cell *c) {
  int phase = int(fractick(650, (int)(size_t)c) * 4000);
  if(phase < 1000)      return gradient(0xFFFF80, 0xA0C0FF,    0, phase, 1000);
  else if(phase < 2000) return gradient(0xA0C0FF, 0xFF80FF, 1000, phase, 2000);
  else if(phase < 3000) return gradient(0xFF80FF, 0xFF8080, 2000, phase, 3000);
  else if(phase < 4000) return gradient(0xFF8080, 0xFFFF80, 3000, phase, 4000);
  return 0xFFD500;
  }

EX int weakfirecolor(int phase) {
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

EX int lightat, safetyat;
EX void drawLightning() { lightat = ticks; }
EX void drawSafety() { safetyat = ticks; }

void drawShield(const transmatrix& V, eItem it) {
#if CAP_CURVE
  float ds = ptick(300);
  color_t col = iinf[it].color;
  if(it == itOrbShield && items[itOrbTime] && !orbused[it])
    col = (col & 0xFEFEFE) / 2;
  if(sphere && cwt.at->land == laHalloween && !wmblack && !wmascii)
    col = 0;
  double d = it == itOrbShield ? cgi.hexf : cgi.hexf - .1;
  int mt = sphere ? 7 : 5;
#if MAXMDIM >= 4
  if(GDIM == 3)
    queueball(V * zpush(cgi.GROIN1), cgi.human_height / 2, darkena(col, 0, 0xFF), itOrbShield);
#else
  if(1) ;
#endif  
  else {
    for(ld a=0; a<=cgi.S84*mt+1e-6; a+=pow(.5, vid.linequality))
      curvepoint(V*xspinpush0(a * M_PI/cgi.S42, d + sin(ds + M_PI*2*a/4/mt)*.1));
    queuecurve(darkena(col, 0, 0xFF), 0x8080808, PPR::LINE);
    }
#endif
  }

void drawSpeed(const transmatrix& V) {
#if CAP_CURVE
  ld ds = ptick(10);
  color_t col = darkena(iinf[itOrbSpeed].color, 0, 0xFF);
#if MAXMDIM >= 4
  if(GDIM == 3) queueball(V * zpush(cgi.GROIN1), cgi.human_height * 0.55, col, itOrbSpeed);
  else
#endif
  for(int b=0; b<cgi.S84; b+=cgi.S14) {
    PRING(a)
      curvepoint(V*xspinpush0((ds+b+a) * M_PI/cgi.S42, cgi.hexf*a/cgi.S84));
    queuecurve(col, 0x8080808, PPR::LINE);
    }
#endif
  }

void drawSafety(const transmatrix& V, int ct) {
#if CAP_QUEUE
  ld ds = ptick(50);
  color_t col = darkena(iinf[itOrbSafety].color, 0, 0xFF);
  #if MAXMDIM >= 4
  if(GDIM == 3) {
    queueball(V * zpush(cgi.GROIN1), 2*cgi.hexf, col, itOrbSafety);
    return;
    }
  #endif
  for(int a=0; a<ct; a++)
    queueline(V*xspinpush0((ds+a*cgi.S84/ct) * M_PI/cgi.S42, 2*cgi.hexf), V*xspinpush0((ds+(a+(ct-1)/2)*cgi.S84/ct) * M_PI / cgi.S42, 2*cgi.hexf), col, vid.linequality);
#endif
  }

void drawFlash(const transmatrix& V) {
#if CAP_CURVE
  float ds = ptick(300);
  color_t col = darkena(iinf[itOrbFlash].color, 0, 0xFF);
  col &= ~1;
  for(int u=0; u<5; u++) {
    ld rad = cgi.hexf * (2.5 + .5 * sin(ds+u*.3));
    #if MAXMDIM >= 4
    if(GDIM == 3) {
      queueball(V * zpush(cgi.GROIN1), rad, col, itOrbFlash);
      }
    #else
    if(1) ;
    #endif
    else {
      PRING(a) curvepoint(V*xspinpush0(a * M_PI / cgi.S42, rad));
      queuecurve(col, 0x8080808, PPR::LINE);
      }
    }
#endif
  }

EX ld cheilevel(ld v) {
  return cgi.FLOOR + (cgi.HEAD - cgi.FLOOR) * v;
  }

EX transmatrix chei(const transmatrix V, int a, int b) {
#if MAXMDIM >= 4
  if(GDIM == 2) return V;
  return V * zpush(cheilevel((a+.5) / b));
#else
  return V;
#endif
  }

void drawLove(const transmatrix& V, int hdir) {
#if CAP_CURVE
  float ds = ptick(300);
  color_t col = darkena(iinf[itOrbLove].color, 0, 0xFF);
  col &= ~1;
  for(int u=0; u<5; u++) {
    PRING(a) {
      double d = (1 + cos(a * M_PI/cgi.S42)) / 2;
      double z = a; if(z>cgi.S42) z = cgi.S84-z;
      if(z <= 10) d += (10-z) * (10-z) * (10-z) / 3000.;

      ld rad = cgi.hexf * (2.5 + .5 * sin(ds+u*.3)) * d;
      transmatrix V1 = chei(V, u, 5);
      curvepoint(V1*xspinpush0((cgi.S42+hdir+a-1) * M_PI/cgi.S42, rad));
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
    transmatrix V1 = chei(V, u, 20);
    queueline(V1*xspinpush0(M_PI+hdir+rad, cgi.hexf*.5), V1*xspinpush0(M_PI+hdir+rad, cgi.hexf*3), col, 2 + vid.linequality);
    }
#endif
  }

void drawLightning(const transmatrix& V) {
#if CAP_QUEUE
  color_t col = darkena(iinf[itOrbLightning].color, 0, 0xFF);
  for(int u=0; u<20; u++) {
    ld leng = 0.5 / (0.1 + (rand() % 100) / 100.0);
    ld rad = rand() % 1000;
    transmatrix V1 = chei(V, u, 20);
    queueline(V1*xspinpush0(rad, cgi.hexf*0.3), V1*xspinpush0(rad, cgi.hexf*leng), col, 2 + vid.linequality);
    }
#endif
  }

#define UNTRANS (GDIM == 3 ? 0x000000FF : 0)

EX void drawPlayerEffects(const transmatrix& V, cell *c, bool onplayer) {
  if(!onplayer && !items[itOrbEmpathy]) return;
  if(items[itOrbShield] > (shmup::on ? 0 : ORBBASE)) drawShield(V, itOrbShield);
  if(items[itOrbShell] > (shmup::on ? 0 : ORBBASE)) drawShield(V, itOrbShell);

  if(items[itOrbSpeed]) drawSpeed(V); 

  if(onplayer && (items[itOrbSword] || items[itOrbSword2])) {
    using namespace sword;
  
    if(shmup::on && SWORDDIM == 2) {
#if CAP_SHAPES
      if(items[itOrbSword])
        queuepoly(V*spin(shmup::pc[multi::cpid]->swordangle), (peace::on ? cgi.shMagicShovel : cgi.shMagicSword), darkena(iinf[itOrbSword].color, 0, 0xC0 + 0x30 * sintick(200)));
  
      if(items[itOrbSword2])
        queuepoly(V*spin(shmup::pc[multi::cpid]->swordangle+M_PI), (peace::on ? cgi.shMagicShovel : cgi.shMagicSword), darkena(iinf[itOrbSword2].color, 0, 0xC0 + 0x30 * sintick(200)));
#endif
      }                  

    else if(SWORDDIM == 3) {
#if CAP_SHAPES
      transmatrix Vsword = 
        shmup::on ? V * shmup::swordmatrix[multi::cpid] * cspin(2, 0, M_PI/2) 
                  : gmatrix[c] * rgpushxto0(inverse(gmatrix[c]) * tC0(V)) * sword::dir[multi::cpid].T;

      if(items[itOrbSword])
        queuepoly(Vsword * cspin(1,2, ticks / 150.), (peace::on ? cgi.shMagicShovel : cgi.shMagicSword), darkena(iinf[itOrbSword].color, 0, 0xC0 + 0x30 * sintick(200)));
  
      if(items[itOrbSword2])
        queuepoly(Vsword * pispin * cspin(1,2, ticks / 150.), (peace::on ? cgi.shMagicShovel : cgi.shMagicSword), darkena(iinf[itOrbSword2].color, 0, 0xC0 + 0x30 * sintick(200)));
#endif
      }
    
    else {
      int& ang = sword::dir[multi::cpid].angle;
      ang %= sword::sword_angles;

#if CAP_QUEUE || CAP_SHAPES
      transmatrix Vnow = gmatrix[c] * rgpushxto0(inverse(gmatrix[c]) * tC0(V)) * ddspin(c,0,M_PI);
#endif

      int adj = 1 - ((sword_angles/cwt.at->type)&1);
      
#if CAP_QUEUE
      if(!euclid && !hybri) for(int a=0; a<sword_angles; a++) {
        if(a == ang && items[itOrbSword]) continue;
        if((a+sword_angles/2)%sword_angles == ang && items[itOrbSword2]) continue;
        bool longer = sword::pos2(cwt.at, a-1) != sword::pos2(cwt.at, a+1);
        if(sword_angles > 48 && !longer) continue;
        color_t col = darkena(0xC0C0C0, 0, 0xFF);
        ld l0 = PURE ? 0.6 * cgi.scalefactor : longer ? 0.36 : 0.4;
        ld l1 = PURE ? 0.7 * cgi.scalefactor : longer ? 0.44 : 0.42;
#if MAXMDIM >= 4
        hyperpoint h0 = GDIM == 3 ? xpush(l0) * zpush(cgi.FLOOR - cgi.human_height/50) * C0 : xpush0(l0);
        hyperpoint h1 = GDIM == 3 ? xpush(l1) * zpush(cgi.FLOOR - cgi.human_height/50) * C0 : xpush0(l1);
#else
        hyperpoint h0 = xpush0(l0);
        hyperpoint h1 = xpush0(l1);
#endif
        transmatrix T = Vnow*spin((sword_angles + (-adj-2*a)) * M_PI / sword_angles);
        queueline(T*h0, T*h1, col, 1, PPR::SUPERLINE);
        }
#endif

#if CAP_SHAPES
      if(items[itOrbSword])
        queuepoly(Vnow*spin(M_PI+(-adj-2*ang)*M_PI/sword_angles), (peace::on ? cgi.shMagicShovel : cgi.shMagicSword), darkena(iinf[itOrbSword].color, 0, 0x80 + 0x70 * sintick(200)));
  
      if(items[itOrbSword2])
        queuepoly(Vnow*spin((-adj-2*ang)*M_PI/sword_angles), (peace::on ? cgi.shMagicShovel : cgi.shMagicSword), darkena(iinf[itOrbSword2].color, 0, 0x80 + 0x70 * sintick(200)));
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
      ld rad = cgi.hexf * u / 250;
      color_t col = darkena(iinf[itOrbSafety].color, 0, 0xFF);
      PRING(a)
        curvepoint(V*xspinpush0(a * M_PI / cgi.S42, rad));
      queuecurve(col, 0, PPR::LINE);
      }
    }
  }

void drawStunStars(const transmatrix& V, int t) {
#if CAP_SHAPES
  for(int i=0; i<3*t; i++) {
    transmatrix V2 = V * spin(M_PI * 2 * i / (3*t) + ptick(200));
#if MAXMDIM >= 4
    if(GDIM == 3) V2 = V2 * zpush(cgi.HEAD);
#endif
    queuepolyat(V2, cgi.shFlailBall, 0xFFFFFFFF, PPR::STUNSTARS);
    }
#endif
  }

EX namespace tortoise {

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

      queuepoly(V, cgi.shTortoise[i][b+small], d);
      if((i >= 5 && i <= 7) || (i >= 9 && i <= 10))
        queuepoly(V * Mirror, cgi.shTortoise[i][b+small], d);
      
      if(i == 8) {
        for(int k=0; k<stuntime; k++) {
          eyecolor &= 0xFEFEFE; 
          eyecolor >>= 1;
          }
        queuepoly(V, cgi.shTortoise[12][b+small], darkena(eyecolor, 0, 0xFF));
        queuepoly(V * Mirror, cgi.shTortoise[12][b+small], darkena(eyecolor, 0, 0xFF));
        }
      }
#endif
    }

  EX int getMatchColor(int bits) {
    int mcol = 1;
    double d = tortoise::getScent(bits);
    
    if(d > 0) mcol = 0xFFFFFF;
    else if(d < 0) mcol = 0;
    
      int dd = 0xFF * (atan(fabs(d)/2) / (M_PI/2));
      
    return gradient(0x487830, mcol, 0, dd, 0xFF);
    }
  EX };

double footfun(double d) {
  d -= floor(d);
  return
    d < .25 ? d :
    d < .75 ? .5-d :
    d-1;
  }

EX bool ivoryz;
                                 
void animallegs(const transmatrix& V, eMonster mo, color_t col, double footphase) {
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
  if(GDIM == 3) {
    if(x[0]) queuepolyat(V * front_leg_move * cspin(0, 2, rightfoot / leg_length) * front_leg_move_inverse, *x[0], col, PPR::MONSTER_FOOT);
    if(x[0]) queuepolyat(V * Mirror * front_leg_move * cspin(0, 2, leftfoot / leg_length) * front_leg_move_inverse, *x[0], col, PPR::MONSTER_FOOT);
    if(x[1]) queuepolyat(V * rear_leg_move * cspin(0, 2, -rightfoot / leg_length) * rear_leg_move_inverse, *x[1], col, PPR::MONSTER_FOOT);
    if(x[1]) queuepolyat(V * Mirror * rear_leg_move * cspin(0, 2, -leftfoot / leg_length) * rear_leg_move_inverse, *x[1], col, PPR::MONSTER_FOOT);
    return;
    }
#endif

  const transmatrix VL = mmscale(V, cgi.ALEG0);
  const transmatrix VAML = mmscale(V, cgi.ALEG);

  if(x[0]) queuepolyat(VL * xpush(rightfoot), *x[0], col, PPR::MONSTER_FOOT);
  if(x[0]) queuepolyat(VL * Mirror * xpush(leftfoot), *x[0], col, PPR::MONSTER_FOOT);
  if(x[1]) queuepolyat(VL * xpush(-rightfoot), *x[1], col, PPR::MONSTER_FOOT);
  if(x[1]) queuepolyat(VL * Mirror * xpush(-leftfoot), *x[1], col, PPR::MONSTER_FOOT);

  if(x[2]) queuepolyat(VAML * xpush(rightfoot/2), *x[2], col, PPR::MONSTER_FOOT);
  if(x[2]) queuepolyat(VAML * Mirror * xpush(leftfoot/2), *x[2], col, PPR::MONSTER_FOOT);
  if(x[3]) queuepolyat(VAML * xpush(-rightfoot/2), *x[3], col, PPR::MONSTER_FOOT);
  if(x[3]) queuepolyat(VAML * Mirror * xpush(-leftfoot/2), *x[3], col, PPR::MONSTER_FOOT);
#endif
  }

EX bool noshadow;

#if CAP_SHAPES
EX void ShadowV(const transmatrix& V, const hpcshape& bp, PPR prio IS(PPR::MONSTER_SHADOW)) {
  if(WDIM == 2 && GDIM == 3 && bp.shs != bp.she) {
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


#if CAP_SHAPES
transmatrix otherbodyparts(const transmatrix& V, color_t col, eMonster who, double footphase) {

#define VFOOT ((GDIM == 2 || prod) ? V : mmscale(V, cgi.LEG0))
#define VLEG mmscale(V, cgi.LEG)
#define VGROIN mmscale(V, cgi.GROIN)
#define VBODY mmscale(V, cgi.BODY)
#define VBODY1 mmscale(V, cgi.BODY1)
#define VBODY2 mmscale(V, cgi.BODY2)
#define VBODY3 mmscale(V, cgi.BODY3)
#define VNECK mmscale(V, cgi.NECK)
#define VHEAD  mmscale(V, cgi.HEAD)
#define VHEAD1 mmscale(V, cgi.HEAD1)
#define VHEAD2 mmscale(V, cgi.HEAD2)
#define VHEAD3 mmscale(V, cgi.HEAD3)

#define VALEGS V
#define VABODY mmscale(V, cgi.ABODY)
#define VAHEAD mmscale(V, cgi.AHEAD)

#define VFISH V
#define VBIRD  ((GDIM == 3 || (where && bird_disruption(where))) ? (WDIM == 2 ? mmscale(V, cgi.BIRD) : V) : mmscale(V, cgi.BIRD + .05 * sintick(1000, (int) (size_t(where))/1000.)))
#define VGHOST  mmscale(V, cgi.GHOST)

#define VSLIMEEYE  mscale(V, cgi.FLATEYE)

  // if(!mmspatial && !footphase && who != moSkeleton) return;
  
  footphase /= SCALE;
  double rightfoot = footfun(footphase / .4 / 2.5) / 4 * 2.5 * SCALE;
  
  const double wobble = -1;

  // todo

  if(detaillevel >= 2 && GDIM == 2) { 
    transmatrix VL = mmscale(V, cgi.LEG1);
    queuepoly(VL * xpush(rightfoot*3/4), cgi.shHumanLeg, col);
    queuepoly(VL * Mirror * xpush(-rightfoot*3/4), cgi.shHumanLeg, col);
    }

  if(GDIM == 2) {
    transmatrix VL = mmscale(V, cgi.LEG);
    queuepoly(VL * xpush(rightfoot/2), cgi.shHumanLeg, col);
    queuepoly(VL * Mirror * xpush(-rightfoot/2), cgi.shHumanLeg, col);
    }

  if(detaillevel >= 2 && GDIM == 2) { 
    transmatrix VL = mmscale(V, cgi.LEG3);
    queuepoly(VL * xpush(rightfoot/4), cgi.shHumanLeg, col);
    queuepoly(VL * Mirror * xpush(-rightfoot/4), cgi.shHumanLeg, col);
    }

  transmatrix Tright, Tleft;
  
  if(GDIM == 2 || hybri) {
    Tright = VFOOT * xpush(rightfoot);
    Tleft = VFOOT * Mirror * xpush(-rightfoot);
    }
  #if MAXMDIM >= 4
  else {
    transmatrix V1 = V;
    if(WDIM == 2) V1 = V1 * zpush(cgi.GROIN);
    Tright = V1 * cspin(0, 2, rightfoot/ leg_length);
    Tleft  = V1 * Mirror * cspin(2, 0, rightfoot / leg_length);
    if(WDIM == 2) Tleft = Tleft * zpush(-cgi.GROIN), Tright = Tright * zpush(-cgi.GROIN);
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
  else if(isTroll(who) || who == moMonkey || who == moYeti || who == moRatling || who == moRatlingAvenger || who == moGoblin) {
    queuepoly(Tright, cgi.shYetiFoot, col);
    queuepoly(Tleft, cgi.shYetiFoot, col);
    }
  else {
    queuepoly(Tright, cgi.shHumanFoot, col);
    queuepoly(Tleft, cgi.shHumanFoot, col);
    }

  if(GDIM == 3 || !mmspatial) return spin(rightfoot * wobble);  

  if(detaillevel >= 2 && who != moZombie)
    queuepoly(mmscale(V, cgi.NECK1), cgi.shHumanNeck, col);
  if(detaillevel >= 1) {
    queuepoly(VGROIN, cgi.shHumanGroin, col);
    if(who != moZombie) queuepoly(VNECK, cgi.shHumanNeck, col);
    }
  if(detaillevel >= 2) {
    queuepoly(mmscale(V, cgi.GROIN1), cgi.shHumanGroin, col);
    if(who != moZombie) queuepoly(mmscale(V, cgi.NECK3), cgi.shHumanNeck, col);
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

EX color_t kind_outline(eItem it) {
  int k = itemclass(it);
  if(k == IC_TREASURE)
    return OUTLINE_TREASURE;
  else if(k == IC_ORB)
    return OUTLINE_ORB;
  else
    return OUTLINE_OTHER;
  }

EX transmatrix face_the_player(const transmatrix V) {
  if(GDIM == 2) return V;
  if(prod) return mscale(V, cos(ptick(750)) * cgi.plevel / 16);
  if(hybri) return V * zpush(cos(ptick(750)) * cgi.plevel / 16);
  transmatrix dummy; /* used only in prod anyways */
  if(nonisotropic) return spin_towards(V, dummy, C0, 2, 0);
  return rgpushxto0(tC0(V));
  }

EX hpcshape& orbshape(eOrbshape s) {
  switch(s) {
     case osLove: return cgi.shLoveRing;
     case osRanged: return cgi.shTargetRing;
     case osOffensive: return cgi.shSawRing;
     case osFriend: return cgi.shPeaceRing;
     case osUtility: return cgi.shGearRing;
     case osDirectional: return cgi.shSpearRing;
     case osWarping: return cgi.shHeptaRing;
     default: return cgi.shRing;
     }
  }

EX bool drawItemType(eItem it, cell *c, const transmatrix& V, color_t icol, int pticks, bool hidden) {
#if !CAP_SHAPES
  return it;
#else
  char xch = iinf[it].glyph;
  auto sinptick = [c, pticks] (int period) { return c ? sintick(period) : sin(animation_factor * pticks / period);};
  auto spinptick = [c, pticks] (int period, ld phase=0) { return c ? spintick(period, phase) : spin((animation_factor * pticks + phase) / period); };
  int ct6 = c ? ctof(c) : 1;
  hpcshape *xsh = 
    (it == itPirate || it == itKraken) ? &cgi.shPirateX :
    (it == itBuggy || it == itBuggy2) ? &cgi.shPirateX :
    it == itHolyGrail ? &cgi.shGrail :
    isElementalShard(it) ? &cgi.shElementalShard :
    (it == itBombEgg || it == itTrollEgg) ? &cgi.shEgg :
    it == itHunting ? &cgi.shTriangle :
    it == itDodeca ? &cgi.shDodeca :
    xch == '*' ? &cgi.shGem[ct6] : 
    xch == '(' ? &cgi.shKnife : 
    it == itShard ? &cgi.shMFloor.b[0] :
    it == itTreat ? &cgi.shTreat :
    it == itSlime ? &cgi.shEgg :
    xch == '%' ? &cgi.shDaisy : xch == '$' ? &cgi.shStar : xch == ';' ? &cgi.shTriangle :
    xch == '!' ? &cgi.shTriangle : it == itBone ? &cgi.shNecro : it == itStatue ? &cgi.shStatue :
    it == itIvory ? &cgi.shFigurine : 
    xch == '?' ? &cgi.shBookCover : 
    it == itKey ? &cgi.shKey : 
    it == itRevolver ? &cgi.shGun :
    NULL;
   
  if(c && doHighlight()) 
    poly_outline = kind_outline(it);

#if MAXMDIM >= 4
  if(c && WDIM == 3) addradar(V, iinf[it].glyph, icol, kind_outline(it));
#endif
  
  if(GDIM == 3 && mapeditor::drawUserShape(V, mapeditor::sgItem, it, darkena(icol, 0, 0xFF), c)) return false;
    
  if(WDIM == 3 && c == centerover && in_perspective() && hdist0(tC0(V)) < cgi.orbsize * 0.25) return false;

  transmatrix Vit = V;
  if(GDIM == 3 && WDIM == 2 && c && it != itBabyTortoise) Vit = mscale(V, cgi.STUFF);
  if(c && prod)
    Vit = mscale(Vit, sin(ptick(750)) * cgi.plevel / 4);
  else if(c && sl2)
    Vit = Vit * zpush(sin(ptick(750)) * cgi.plevel / 4);
  else
    if(GDIM == 3 && c && it != itBabyTortoise) Vit = face_the_player(Vit);
  // V * cspin(0, 2, ptick(618, 0));

  if(c && history::includeHistory && history::infindhistory.count(c)) poly_outline = OUTLINE_DEAD;

  if(!mmitem && it) return true;
  
  else if(it == itSavedPrincess) {
    drawMonsterType(moPrincess, c, V, icol, 0, icol);
    return false;
    }
  
  else if(it == itStrongWind) {
    queuepoly(Vit * spinptick(750), cgi.shFan, darkena(icol, 0, 255));
    }

  else if(it == itWarning) {
    queuepoly(Vit * spinptick(750), cgi.shTriangle, darkena(icol, 0, 255));
    }
    
  else if(it == itBabyTortoise) {
    int bits = c ? tortoise::babymap[c] : tortoise::last;
    int over = c && c->monst == moTortoise;
    tortoise::draw(Vit * spinptick(5000) * ypush(cgi.crossf*.15), bits, over ? 4 : 2, 0);
    // queuepoly(Vit, cgi.shHeptaMarker, darkena(tortoise::getMatchColor(bits), 0, 0xC0));
    }
  
  else if(it == itCompass) {
    transmatrix V2;
    #if CAP_CRYSTAL
    if(cryst) {
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
    if(GDIM == 3) {
      queuepoly(Vit, cgi.shRing, 0xFFFFFFFF);
      if(WDIM == 2) V2 = mscale(V2, cgi.STUFF);
      V2 = V2 * cspin(1, 2, M_PI * sintick(100) / 39);
      queuepoly(V2, cgi.shCompass3, 0xFF0000FF);
      queuepoly(V2 * pispin, cgi.shCompass3, 0x000000FF);      
      }
    else {
      if(c) V2 = V2 * spin(M_PI * sintick(100) / 30);
      queuepoly(V2, cgi.shCompass1, 0xFF8080FF);
      queuepoly(V2, cgi.shCompass2, 0xFFFFFFFF);
      queuepoly(V2, cgi.shCompass3, 0xFF0000FF);
      queuepoly(V2 * pispin, cgi.shCompass3, 0x000000FF);
      }
    xsh = NULL;
    }

  else if(it == itPalace) {
    ld h = cgi.human_height;
    #if MAXMDIM >= 4
    if(GDIM == 3 && WDIM == 2) {
      dynamicval<qfloorinfo> qfi2(qfi, qfi);
      transmatrix V2 = V * spin(ticks / 1500.);
      /* divisors should be higher than in plate renderer */
      qfi.fshape = &cgi.shMFloor2;
      draw_shapevec(c, V2 * zpush(-h/30), qfi.fshape->levels[0], 0xFFD500FF, PPR::WALL);

      qfi.fshape = &cgi.shMFloor3;
      draw_shapevec(c, V2 * zpush(-h/25), qfi.fshape->levels[0], darkena(icol, 0, 0xFF), PPR::WALL);

      qfi.fshape = &cgi.shMFloor4;
      draw_shapevec(c, V2 * zpush(-h/20), qfi.fshape->levels[0], 0xFFD500FF, PPR::WALL);
      }
    else if(WDIM == 3 && c) {
      transmatrix V2 = Vit * spin(ticks / 1500.);
      draw_floorshape(c, V2 * zpush(h/100), cgi.shMFloor3, 0xFFD500FF);
      draw_floorshape(c, V2 * zpush(h/50), cgi.shMFloor4, darkena(icol, 0, 0xFF));
      queuepoly(V2, cgi.shGem[ct6], 0xFFD500FF);
      }
    else if(WDIM == 3 && !c) {
      queuepoly(Vit, cgi.shGem[ct6], 0xFFD500FF);
      }
    else 
    #endif
    {
      transmatrix V2 = Vit * spin(ticks / 1500.);
      draw_floorshape(c, V2, cgi.shMFloor3, 0xFFD500FF);
      draw_floorshape(c, V2, cgi.shMFloor4, darkena(icol, 0, 0xFF));
      queuepoly(V2, cgi.shGem[ct6], 0xFFD500FF);
      }
    xsh = NULL;
    }
  
  else if(mapeditor::drawUserShape(V, mapeditor::sgItem, it, darkena(icol, 0, 0xFF), c)) ;
  
  else if(it == itRose) {
    for(int u=0; u<4; u++)
      queuepoly(Vit * spinptick(1500) * spin(2*M_PI / 3 / 4 * u), cgi.shRoseItem, darkena(icol, 0, hidden ? 0x30 : 0xA0));
    }

  else if(it == itBarrow && c) {
    for(int i = 0; i<c->landparam; i++)
      queuepolyat(Vit * spin(2 * M_PI * i / c->landparam) * xpush(.15) * spinptick(1500), *xsh, darkena(icol, 0, hidden ? 0x40 : 
        (highwall(c) && wmspatial) ? 0x60 : 0xFF),
        PPR::HIDDEN);

//      queuepoly(Vit*spin(M_PI+(1-2*ang)*2*M_PI/cgi.S84), cgi.shMagicSword, darkena(0xC00000, 0, 0x80 + 0x70 * sin(ticks / 200.0)));
    }
    
  else if(xsh) {
    if(it == itFireShard) icol = firecolor(100);
    if(it == itWaterShard) icol = watercolor(100) >> 8;
    
    if(it == itZebra) icol = 0xFFFFFF;
    if(it == itLotus) icol = 0x101010;
    if(it == itSwitch) icol = minf[active_switch()].color;
    
    transmatrix V2 = Vit * spinptick(1500);
  
    if(xsh == &cgi.shBookCover && mmitem) {
      if(GDIM == 3)
        queuepoly(V2 * cpush(2, 1e-3), cgi.shBook, 0x805020FF);
      else
        queuepoly(V2, cgi.shBook, 0x805020FF);
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
      queuepolyat(Vit * spinptick(1500), cgi.shFishTail, col, PPR::ITEM_BELOW);

    queuepolyat(Vit, cgi.shDisk, darkena(icol1, 0, inice ? 0x80 : hidden ? 0x20 : 0xC0), prio);

    queuepolyat(Vit * spinptick(1500), orbshape(iinf[it].orbshape), col, prio);
    }

  else if(it) return true;

  return false;
#endif
  }

#if CAP_SHAPES
color_t skincolor = 0xD0C080FF;

void humanoid_eyes(const transmatrix& V, color_t ecol, color_t hcol = skincolor) {
  if(GDIM == 3) {
    queuepoly(VHEAD, cgi.shPHeadOnly, hcol);
    queuepoly(VHEAD, cgi.shSkullEyes, ecol);
    }
  }

EX void drawTerraWarrior(const transmatrix& V, int t, int hp, double footphase) {
  ShadowV(V, cgi.shPBody);
  color_t col = linf[laTerracotta].color;
  int bcol = darkena(false ? 0xC0B23E : col, 0, 0xFF);
  const transmatrix VBS = otherbodyparts(V, bcol, moDesertman, footphase);
  queuepoly(VBODY * VBS, cgi.shPBody, bcol);
  if(!peace::on) queuepoly(VBODY * VBS * Mirror, cgi.shPSword, darkena(0xC0C0C0, 0, 0xFF));
  queuepoly(VBODY1 * VBS, cgi.shTerraArmor1, darkena(t > 0 ? 0x4040FF : col, 0, 0xFF));
  if(hp >= 4) queuepoly(VBODY2 * VBS, cgi.shTerraArmor2, darkena(t > 1 ? 0xC00000 : col, 0, 0xFF));
  if(hp >= 2) queuepoly(VBODY3 * VBS, cgi.shTerraArmor3, darkena(t > 2 ? 0x612600 : col, 0, 0xFF));
  queuepoly(VHEAD, cgi.shTerraHead, darkena(t > 4 ? 0x202020 : t > 3 ? 0x504040 : col, 0, 0xFF));
  queuepoly(VHEAD1, cgi.shPFace, bcol);
  humanoid_eyes(V, 0x400000FF, darkena(col, 0, 0xFF));
  }
#endif

void drawPlayer(eMonster m, cell *where, const transmatrix& V, color_t col, double footphase, bool stop = false) {
  charstyle& cs = getcs();

  if(mapeditor::drawplayer && !mapeditor::drawUserShape(V, mapeditor::sgPlayer, cs.charid, cs.skincolor, where)) {
  
    if(cs.charid >= 8) {
      /* famililar */
      if(!mmspatial && !footphase) {
        if(stop) return;
        queuepoly(VALEGS, cgi.shWolfLegs, fc(150, cs.dresscolor, 4));   
        }
      else {
        ShadowV(V, cgi.shWolfBody);
        if(stop) return;
        animallegs(VALEGS, moWolf, fc(500, cs.dresscolor, 4), footphase);
        }
      queuepoly(VABODY, cgi.shWolfBody, fc(0, cs.skincolor, 0));
      queuepoly(VAHEAD, cgi.shFamiliarHead, fc(500, cs.haircolor, 2));
      if(!shmup::on || shmup::curtime >= shmup::getPlayer()->nextshot) {
        color_t col = items[itOrbDiscord] ? watercolor(0) : fc(314, cs.eyecolor, 3);
        queuepoly(VAHEAD, cgi.shFamiliarEye, col);
        queuepoly(VAHEAD * Mirror, cgi.shFamiliarEye, col);
        }

      if(knighted)
        queuepoly(VABODY, cgi.shKnightCloak, darkena(cloakcolor(knighted), 1, 0xFF));

      if(tortoise::seek())
        tortoise::draw(VABODY, tortoise::seekbits, 4, 0);
      }
    else if(cs.charid >= 6) {
      /* dog */
      if(!mmspatial && !footphase) {
        if(stop) return;
        queuepoly(VABODY, cgi.shDogBody, fc(0, cs.skincolor, 0));
        }
      else {
        ShadowV(V, cgi.shDogTorso);          
        if(stop) return;
        animallegs(VALEGS, moRunDog, fc(500, cs.dresscolor, 4), footphase);
        queuepoly(VABODY, cgi.shDogTorso, fc(0, cs.skincolor, 0));
        }
      queuepoly(VAHEAD, cgi.shDogHead, fc(150, cs.haircolor, 2));

      if(!shmup::on || shmup::curtime >= shmup::getPlayer()->nextshot) {
        color_t col = items[itOrbDiscord] ? watercolor(0) : fc(314, cs.eyecolor, 3);
        queuepoly(VAHEAD, cgi.shWolf1, col);
        queuepoly(VAHEAD, cgi.shWolf2, col);
        }

      color_t colnose = items[itOrbDiscord] ? watercolor(0) : fc(314, 0xFF, 3);
      queuepoly(VAHEAD, cgi.shWolf3, colnose);

      if(knighted)
        queuepoly(VABODY, cgi.shKnightCloak, darkena(cloakcolor(knighted), 1, 0xFF));

      if(tortoise::seek())
        tortoise::draw(VABODY, tortoise::seekbits, 4, 0);
      }
    else if(cs.charid >= 4) {
      /* cat */
      if(!mmspatial && !footphase) {
        if(stop) return;
        queuepoly(VALEGS, cgi.shCatLegs, fc(500, cs.dresscolor, 4));
        }
      else {
        ShadowV(V, cgi.shCatBody);
        if(stop) return;
        animallegs(VALEGS, moRunDog, fc(500, cs.dresscolor, 4), footphase);
        }
      queuepoly(VABODY, cgi.shCatBody, fc(0, cs.skincolor, 0));
      queuepoly(VAHEAD, cgi.shCatHead, fc(150, cs.haircolor, 2));
      if(!shmup::on || shmup::curtime >= shmup::getPlayer()->nextshot) {
        color_t col = items[itOrbDiscord] ? watercolor(0) : fc(314, cs.eyecolor, 3);
        queuepoly(VAHEAD * xpush(.04), cgi.shWolf1, col);
        queuepoly(VAHEAD * xpush(.04), cgi.shWolf2, col);
        }

      if(knighted)
        queuepoly(VABODY, cgi.shKnightCloak, darkena(cloakcolor(knighted), 1, 0xFF));

      if(tortoise::seek())
        tortoise::draw(VABODY, tortoise::seekbits, 4, 0);
      }
    else {
       /* human */
      ShadowV(V, (cs.charid&1) ? cgi.shFemaleBody : cgi.shPBody);
      if(stop) return;

      const transmatrix VBS = otherbodyparts(V, fc(0, cs.skincolor, 0), items[itOrbFish] ? moWaterElemental : moPlayer, footphase);
  
  
      queuepoly(VBODY * VBS, (cs.charid&1) ? cgi.shFemaleBody : cgi.shPBody, fc(0, cs.skincolor, 0));      
  
      if(cs.charid&1)
        queuepoly(VBODY1 * VBS, cgi.shFemaleDress, fc(500, cs.dresscolor, 4));
  
      if(cs.charid == 2)
        queuepoly(VBODY2 * VBS, cgi.shPrinceDress,  fc(400, cs.dresscolor, 5));
      if(cs.charid == 3) 
        queuepoly(VBODY2 * VBS, cgi.shPrincessDress,  fc(400, cs.dresscolor2, 5));
        
      if(items[itOrbSide3])
        queuepoly(VBODY * VBS, (cs.charid&1) ? cgi.shFerocityF : cgi.shFerocityM, fc(0, cs.skincolor, 0));
  
      if(items[itOrbHorns]) {
        queuepoly(VBODY * VBS, cgi.shBullHead, items[itOrbDiscord] ? watercolor(0) : 0xFF000030);
        queuepoly(VBODY * VBS, cgi.shBullHorn, items[itOrbDiscord] ? watercolor(0) : 0xFF000040);
        queuepoly(VBODY * VBS * Mirror, cgi.shBullHorn, items[itOrbDiscord] ? watercolor(0) : 0xFF000040);
        }
  
      if(items[itOrbSide1] && !shmup::on)
        queuepoly(VBODY * VBS * spin(-M_PI/24), cs.charid >= 2 ? cgi.shSabre : cgi.shPSword, fc(314, cs.swordcolor, 3)); // 3 not colored
      
      transmatrix VWPN = cs.lefthanded ? VBODY * VBS * Mirror : VBODY * VBS;
      
      if(peace::on) ;
      else if(racing::on) {
  #if CAP_RACING
        if(racing::trophy[multi::cpid])
          queuepoly(VWPN, cgi.shTrophy, racing::trophy[multi::cpid]);
  #endif
        }
      else if(items[itOrbThorns])
        queuepoly(VWPN, cgi.shHedgehogBladePlayer, items[itOrbDiscord] ? watercolor(0) : 0x00FF00FF);
      else if(!shmup::on && items[itOrbDiscord])
        queuepoly(VWPN, cs.charid >= 2 ? cgi.shSabre : cgi.shPSword, watercolor(0));
      else if(items[itRevolver])
        queuepoly(VWPN, cgi.shGunInHand, fc(314, cs.swordcolor, 3)); // 3 not colored
      else if(items[itOrbSlaying]) {
        queuepoly(VWPN, cgi.shFlailTrunk, fc(314, cs.swordcolor, 3));
        queuepoly(VWPN, cgi.shHammerHead, fc(314, cs.swordcolor, 3));
        }
      else if(!shmup::on)
        queuepoly(VWPN, cs.charid >= 2 ? cgi.shSabre : cgi.shPSword, fc(314, cs.swordcolor, 3)); // 3 not colored
      else if(shmup::curtime >= shmup::getPlayer()->nextshot)
        queuepoly(VWPN, cgi.shPKnife, fc(314, cs.swordcolor, 3)); // 3 not colored
      
      if(items[itOrbBeauty]) {
        if(cs.charid&1)
          queuepoly(VHEAD1, cgi.shFlowerHair, darkena(iinf[itOrbBeauty].color, 0, 0xFF));
        else
          queuepoly(VWPN, cgi.shFlowerHand, darkena(iinf[itOrbBeauty].color, 0, 0xFF));
        }
      
      if(where && where->land == laWildWest) {
        queuepoly(VHEAD1, cgi.shWestHat1, darkena(cs.swordcolor, 1, 0XFF));
        queuepoly(VHEAD2, cgi.shWestHat2, darkena(cs.swordcolor, 0, 0XFF));
        }
  
      if(cheater && !autocheat) {
        queuepoly(VHEAD1, (cs.charid&1) ? cgi.shGoatHead : cgi.shDemon, darkena(0xFFFF00, 0, 0xFF));
        // queuepoly(V, shHood, darkena(0xFF00, 1, 0xFF));
        }
      else {
        queuepoly(VHEAD, cgi.shPFace, fc(500, cs.skincolor, 1));
        queuepoly(VHEAD1, (cs.charid&1) ? cgi.shFemaleHair : cgi.shPHead, fc(150, cs.haircolor, 2));
        }      
      
      humanoid_eyes(V, cs.eyecolor, cs.skincolor);
  
      if(knighted)
        queuepoly(VBODY * VBS, cgi.shKnightCloak, darkena(cloakcolor(knighted), 1, 0xFF));
  
      if(tortoise::seek())
        tortoise::draw(VBODY * VBS * ypush(-cgi.crossf*.25), tortoise::seekbits, 4, 0);
      }
    }
  }

EX int wingphase(int period, int phase IS(0)) {
  ld t = fractick(period, phase);
  const int WINGS2 = WINGS * 2;
  int ti = int(t * WINGS2) % WINGS2;
  if(ti > WINGS) ti = WINGS2 - ti;
  return ti;
  }

transmatrix wingmatrix(int period, int phase = 0) {
  ld t = fractick(period, phase) * 2 * M_PI;
  transmatrix Vwing = Id;
  Vwing[1][1] = .85 + .15 * sin(t);
  return Vwing;
  }

void drawMimic(eMonster m, cell *where, const transmatrix& V, color_t col, double footphase) {
  charstyle& cs = getcs();
  
  if(mapeditor::drawUserShape(V, mapeditor::sgPlayer, cs.charid, darkena(col, 0, 0x80), where)) return;
  
  if(cs.charid >= 8) {
    queuepoly(VABODY, cgi.shWolfBody, darkena(col, 0, 0xC0));
    ShadowV(V, cgi.shWolfBody);

    if(mmspatial || footphase)
      animallegs(VALEGS, moWolf, darkena(col, 0, 0xC0), footphase);
    else 
      queuepoly(VABODY, cgi.shWolfLegs, darkena(col, 0, 0xC0));

    queuepoly(VABODY, cgi.shFamiliarHead, darkena(col, 0, 0xC0));
    queuepoly(VAHEAD, cgi.shFamiliarEye, darkena(col, 0, 0xC0));
    queuepoly(VAHEAD * Mirror, cgi.shFamiliarEye, darkena(col, 0, 0xC0));
    }
  else if(cs.charid >= 6) {
    ShadowV(V, cgi.shDogBody);
    queuepoly(VAHEAD, cgi.shDogHead, darkena(col, 0, 0xC0));
    if(mmspatial || footphase) {
      animallegs(VALEGS, moRunDog, darkena(col, 0, 0xC0), footphase);
      queuepoly(VABODY, cgi.shDogTorso, darkena(col, 0, 0xC0));
      }
    else 
      queuepoly(VABODY, cgi.shDogBody, darkena(col, 0, 0xC0));
    queuepoly(VABODY, cgi.shWolf1, darkena(col, 1, 0xC0));
    queuepoly(VABODY, cgi.shWolf2, darkena(col, 1, 0xC0));
    queuepoly(VABODY, cgi.shWolf3, darkena(col, 1, 0xC0));
    }
  else if(cs.charid >= 4) {
    ShadowV(V, cgi.shCatBody);
    queuepoly(VABODY, cgi.shCatBody, darkena(col, 0, 0xC0));
    queuepoly(VAHEAD, cgi.shCatHead, darkena(col, 0, 0xC0));
    if(mmspatial || footphase) 
      animallegs(VALEGS, moRunDog, darkena(col, 0, 0xC0), footphase);
    else 
      queuepoly(VALEGS, cgi.shCatLegs, darkena(col, 0, 0xC0));
    queuepoly(VAHEAD * xpush(.04), cgi.shWolf1, darkena(col, 1, 0xC0));
    queuepoly(VAHEAD * xpush(.04), cgi.shWolf2, darkena(col, 1, 0xC0));
    }
  else {
    const transmatrix VBS = otherbodyparts(V, darkena(col, 0, 0x40), m, footphase);
    queuepoly(VBODY * VBS, (cs.charid&1) ? cgi.shFemaleBody : cgi.shPBody,  darkena(col, 0, 0X80));

    if(!shmup::on) {
      bool emp = items[itOrbEmpathy] && m != moShadow;
      if(items[itOrbThorns] && emp)
        queuepoly(VBODY * VBS, cgi.shHedgehogBladePlayer, darkena(col, 0, 0x40));
      if(items[itOrbSide1] && !shmup::on)
        queuepoly(VBODY * VBS * spin(-M_PI/24), cs.charid >= 2 ? cgi.shSabre : cgi.shPSword, darkena(col, 0, 0x40));      
      if(items[itOrbSide3] && emp)
        queuepoly(VBODY * VBS, (cs.charid&1) ? cgi.shFerocityF : cgi.shFerocityM, darkena(col, 0, 0x40));

      queuepoly(VBODY * VBS, (cs.charid >= 2 ? cgi.shSabre : cgi.shPSword), darkena(col, 0, 0XC0));
      }
    else if(!where || shmup::curtime >= shmup::getPlayer()->nextshot)
      queuepoly(VBODY * VBS, cgi.shPKnife, darkena(col, 0, 0XC0));

    if(knighted)
      queuepoly(VBODY3 * VBS, cgi.shKnightCloak, darkena(col, 1, 0xC0));

    queuepoly(VHEAD1, (cs.charid&1) ? cgi.shFemaleHair : cgi.shPHead,  darkena(col, 1, 0XC0));
    queuepoly(VHEAD, cgi.shPFace,  darkena(col, 0, 0XC0));
    if(cs.charid&1)
      queuepoly(VBODY1 * VBS, cgi.shFemaleDress,  darkena(col, 1, 0XC0));
    if(cs.charid == 2)
      queuepoly(VBODY2 * VBS, cgi.shPrinceDress,  darkena(col, 1, 0XC0));
    if(cs.charid == 3)
      queuepoly(VBODY2 * VBS, cgi.shPrincessDress,  darkena(col, 1, 0XC0));

    humanoid_eyes(V,  0xFF, darkena(col, 0, 0x40));
    }
  }

EX bool drawMonsterType(eMonster m, cell *where, const transmatrix& V1, color_t col, double footphase, color_t asciicol) {

#if MAXMDIM >= 4
  if(GDIM == 3 && m != moPlayer && asciicol != NOCOLOR)
    addradar(V1, minf[m].glyph, asciicol, isFriendly(m) ? 0x00FF00FF : 0xFF0000FF);
#endif

#if CAP_SHAPES
  char xch = minf[m].glyph;
  
  transmatrix V = V1;
  if(WDIM == 3 && (classflag(m) & CF_FACE_UP) && where && !hybri) V = V1 * cspin(0, 2, M_PI/2);

  // if(GDIM == 3) V = V * cspin(0, 2, M_PI/2);

  if(m == moTortoise && where && where->stuntime >= 3)
    drawStunStars(V, where->stuntime-2);
  else if (m == moTortoise || m == moPlayer || (where && !where->stuntime)) ;
  else if(where && !(isMetalBeast(m) && where->stuntime == 1))
    drawStunStars(V, where->stuntime);
  
  if(mapeditor::drawUserShape(V, mapeditor::sgMonster, m, darkena(col, 0, 0xFF), where))
    return false;

  switch(m) {
    case moTortoise: {
      int bits = where ? tortoise::getb(where) : tortoise::last;
      tortoise::draw(V, bits, 0, where ? where->stuntime : 0);
      if(tortoise::seek() && !tortoise::diff(bits) && where)
        queuepoly(V, cgi.shRing, darkena(0xFFFFFF, 0, 0x80 + 0x70 * sintick(200)));
      return false;
      }
    
    case moPlayer:
      drawPlayer(m, where, V, col, footphase);  
      return false;

    case moMimic: case moShadow: case moIllusion:
      drawMimic(m, where, V, col, footphase);
      return false;
    
    case moBullet:
      ShadowV(V, cgi.shKnife);
      queuepoly(VBODY * spin(-M_PI/4), cgi.shKnife, getcs().swordcolor);
      return false;
    
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
      return false;
      }
    
    case moGolem: case moGolemMoved: {
      ShadowV(V, cgi.shPBody);
      const transmatrix VBS = otherbodyparts(V, darkena(col, 1, 0XC0), m, footphase);
      queuepoly(VBODY * VBS, cgi.shPBody, darkena(col, 0, 0XC0));
      queuepoly(VHEAD, cgi.shGolemhead, darkena(col, 1, 0XFF));
      humanoid_eyes(V, 0xC0C000FF, darkena(col, 0, 0xFF));
      return false;
      }
    
    case moEvilGolem: case moIceGolem: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 2, 0xC0), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, darkena(col, 0, 0XC0));
      queuepoly(VHEAD, cgi.shGolemhead, darkena(col, 1, 0XFF));
      humanoid_eyes(V, 0xFF0000FF, darkena(col, 0, 0xFF));
      return false;
      }

    case moFalsePrincess: case moRoseLady: case moRoseBeauty: {
      princess:
      bool girl = princessgender() == GEN_F;
      bool evil = !isPrincess(m);
  
      int facecolor = evil ? 0xC0B090FF : 0xD0C080FF;
      
      ShadowV(V, girl ? cgi.shFemaleBody : cgi.shPBody);
      const transmatrix VBS = otherbodyparts(V, facecolor, m, footphase);
      queuepoly(VBODY * VBS, girl ? cgi.shFemaleBody : cgi.shPBody, facecolor);
  
      if(m == moPrincessArmed) 
        queuepoly(VBODY * VBS * Mirror, vid.cs.charid < 2 ? cgi.shSabre : cgi.shPSword, 0xFFFFFFFF);
      
      if((m == moFalsePrincess || m == moRoseBeauty) && where && where->cpdist == 1)
        queuepoly(VBODY * VBS, cgi.shPKnife, 0xFFFFFFFF);
  
      if(m == moRoseLady) {
        queuepoly(VBODY * VBS, cgi.shPKnife, 0xFFFFFFFF);
        queuepoly(VBODY * VBS * Mirror, cgi.shPKnife, 0xFFFFFFFF);
        }
  
      if(girl) {
        queuepoly(VBODY1 * VBS, cgi.shFemaleDress,  evil ? 0xC000C0FF : 0x00C000FF);
        if(vid.cs.charid < 2) 
          queuepoly(VBODY2 * VBS, cgi.shPrincessDress, (evil ? 0xC040C0C0 : 0x8080FFC0) | UNTRANS);
        }
      else {
        if(vid.cs.charid < 2) 
          queuepoly(VBODY1 * VBS, cgi.shPrinceDress,  evil ? 0x802080FF : 0x404040FF);
        }    
  
      if(m == moRoseLady) {
  //    queuepoly(V, girl ? cgi.shGoatHead : cgi.shDemon,  0x800000FF);
        // make her hair a bit darker to stand out in 3D
        queuepoly(VHEAD1, girl ? cgi.shFemaleHair : cgi.shPHead,  evil ? 0x500050FF : GDIM == 3 ? 0x666A64FF : 0x332A22FF);
        }
      else if(m == moRoseBeauty) {
        if(girl) {
          queuepoly(VHEAD1, cgi.shBeautyHair,  0xF0A0D0FF);
          queuepoly(VHEAD2, cgi.shFlowerHair,  0xC00000FF);
          }
        else {
          queuepoly(VHEAD1, cgi.shPHead,  0xF0A0D0FF);
          queuepoly(VBS, cgi.shFlowerHand,  0xC00000FF);
          queuepoly(VBODY2 * VBS, cgi.shSuspenders,  0xC00000FF);
          }
        }
      else {
        queuepoly(VHEAD1, girl ? cgi.shFemaleHair : cgi.shPHead,  
          evil ? 0xC00000FF : 0x332A22FF);
        }
      queuepoly(VHEAD, cgi.shPFace,  facecolor);
      humanoid_eyes(V, evil ? 0x0000C0FF : 0x00C000FF, facecolor);
      return false;
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
        queuepoly(VAHEAD * Mirror, cgi.shFamiliarEye, 0xFF);
        }
      return false;
      }
    
    case moReptile: {
      ShadowV(V, cgi.shReptileBody);
      animallegs(VALEGS, moReptile, darkena(col, 0, 0xFF), footphase);
      queuepoly(VABODY, cgi.shReptileBody, darkena(col, 0, 0xFF));
      queuepoly(VAHEAD, cgi.shReptileHead, darkena(col, 0, 0xFF));
      queuepoly(VAHEAD, cgi.shReptileEye, darkena(col, 3, 0xFF));
      queuepoly(VAHEAD * Mirror, cgi.shReptileEye, darkena(col, 3, 0xFF));
      if(GDIM == 2) queuepoly(VABODY, cgi.shReptileTail, darkena(col, 2, 0xFF));
      return false;
      }
    
    case moSalamander: {
      ShadowV(V, cgi.shReptileBody);
      animallegs(VALEGS, moReptile, darkena(0xD00000, 1, 0xFF), footphase);
      queuepoly(VABODY, cgi.shReptileBody, darkena(0xD00000, 0, 0xFF));
      queuepoly(VAHEAD, cgi.shReptileHead, darkena(0xD00000, 1, 0xFF));
      queuepoly(VAHEAD, cgi.shReptileEye, darkena(0xD00000, 0, 0xFF));
      queuepoly(VAHEAD * Mirror, cgi.shReptileEye, darkena(0xD00000, 0, 0xFF));
      queuepoly(VABODY, cgi.shReptileTail, darkena(0xD08000, 0, 0xFF));
      return false;
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
      return false;
      }
    
    case moMouse: case moMouseMoved: {
      queuepoly(VALEGS, cgi.shMouse, darkena(col, 0, 0xFF));
      queuepoly(VALEGS, cgi.shMouseLegs, darkena(col, 1, 0xFF));
      queuepoly(VALEGS, cgi.shMouseEyes, 0xFF);
      return false;
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
      return false;
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
      return false;
      }
    
    case moShark: case moGreaterShark: case moCShark:
      queuepoly(VFISH, cgi.shShark, darkena(col, 0, 0xFF));
      return false;

    case moEagle: case moParrot: case moBomberbird: case moAlbatross:
    case moTameBomberbird: case moWindCrow: case moTameBomberbirdMoved:
    case moSandBird: case moAcidBird: {
      ShadowV(V, cgi.shEagle);
      auto& sh = GDIM == 3 ? cgi.shAnimatedEagle[wingphase(200)] : cgi.shEagle;
      if(m == moParrot && GDIM == 3)
        queuepolyat(VBIRD, sh, darkena(col, 0, 0xFF), PPR::SUPERLINE);
      else
        queuepoly(VBIRD, sh, darkena(col, 0, 0xFF));
      return false;
      }
    
    case moSparrowhawk: case moWestHawk: {
      ShadowV(V, cgi.shHawk);
      auto& sh = GDIM == 3 ? cgi.shAnimatedHawk[wingphase(200)] : cgi.shHawk;
      queuepoly(VBIRD, sh, darkena(col, 0, 0xFF));
      return false;
      }
    
    case moButterfly: {
      transmatrix Vwing = wingmatrix(100);
      ShadowV(V * Vwing, cgi.shButterflyWing);
      if(GDIM == 2)
        queuepoly(VBIRD * Vwing, cgi.shButterflyWing, darkena(col, 0, 0xFF));
      else
        queuepoly(VBIRD, cgi.shAnimatedButterfly[wingphase(100)], darkena(col, 0, 0xFF));
      queuepoly(VBIRD, cgi.shButterflyBody, darkena(col, 2, 0xFF));
      return false;
      }
    
    case moGadfly: {
      transmatrix Vwing = wingmatrix(100);
      ShadowV(V * Vwing, cgi.shGadflyWing);
      queuepoly(VBIRD * Vwing, GDIM == 2 ? cgi.shGadflyWing : cgi.shAnimatedGadfly[wingphase(100)], darkena(col, 0, 0xFF));
      queuepoly(VBIRD, cgi.shGadflyBody, darkena(col, 1, 0xFF));
      queuepoly(VBIRD, cgi.shGadflyEye, darkena(col, 2, 0xFF));
      queuepoly(VBIRD * Mirror, cgi.shGadflyEye, darkena(col, 2, 0xFF));
      return false;
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
      queuepoly(V*Mirror, cgi.shBatFang, darkena(0xFFC0C0, 0, 0xFF));
      queuepoly(V, cgi.shBatEye, darkena(00000000, 0, 0xFF));
      queuepoly(V*Mirror, cgi.shBatEye, darkena(00000000, 0, 0xFF)); */
      return false;
      }
    
    case moGargoyle: {
      ShadowV(V, cgi.shGargoyleWings);
      queuepoly(VBIRD, GDIM == 2 ? cgi.shGargoyleWings : cgi.shAnimatedGargoyle[wingphase(300)], darkena(col, 0, 0xD0));
      queuepoly(VBIRD, GDIM == 2 ? cgi.shGargoyleBody : cgi.shAnimatedGargoyle2[wingphase(300)], darkena(col, 0, 0xFF));
      return false;
      }
    
    case moZombie: {
      int c = darkena(col, where && where->land == laHalloween ? 1 : 0, 0xFF);
      const transmatrix VBS = otherbodyparts(V, c, m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBODY * VBS, cgi.shPBody, c);
      return false;
      }
    
    case moTerraWarrior: {
      drawTerraWarrior(V, 7, (where ? where->hitpoints : 7), footphase);
      return false;
      }
    
    case moVariantWarrior: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, darkena(0xFFD500, 0, 0xF0));
      if(!peace::on) queuepoly(VBS, cgi.shPSword, 0xFFFF00FF);
      queuepoly(VHEAD, cgi.shHood, 0x008000FF);
      humanoid_eyes(V, 0xFFFF00FF);
      return false;
      }
    
    case moDesertman: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, darkena(col, 0, 0xC0));
      if(!peace::on) queuepoly(VBS, cgi.shPSword, 0xFFFF00FF);
      queuepoly(VHEAD, cgi.shHood, 0xD0D000C0 | UNTRANS);
      humanoid_eyes(V, 0x301800FF);
      return false;
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
      return false;
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
      return false;
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
      return false;
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
      return false;
      }
    
    case moSkeleton: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(0xFFFFFF, 0, 0xFF), moSkeleton, footphase);
      queuepoly(VBS, cgi.shSkeletonBody, darkena(0xFFFFFF, 0, 0xFF));
      if(GDIM == 2) queuepoly(VHEAD, cgi.shSkull, darkena(0xFFFFFF, 0, 0xFF));
      if(GDIM == 2) queuepoly(VHEAD1, cgi.shSkullEyes, 0x000000FF);
      humanoid_eyes(V, 0x000000FF, 0xFFFFFFFF);      
      ShadowV(V, cgi.shSkeletonBody);
      queuepoly(VBS, cgi.shSabre, 0xFFFFFFFF);
      return false;
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
      return false;
      }
    
    case moCrystalSage: {
      const transmatrix VBS = VBODY * otherbodyparts(V, 0xFFFFFFFF, m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, 0xFFFFFFFF);
      queuepoly(VHEAD1, cgi.shPHead, 0xFFFFFFFF);
      queuepoly(VHEAD, cgi.shPFace, 0xFFFFFFFF);
      humanoid_eyes(V, 0xFFFFFFFF, 0xC0C0C0FF);
      return false;
      }
    
    case moHedge: {
      ShadowV(V, cgi.shPBody);
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
      queuepoly(VBS, cgi.shPBody, darkena(col, 0, 0xFF));
      queuepoly(VBS, cgi.shHedgehogBlade, 0xC0C0C0FF);
      queuepoly(VHEAD1, cgi.shPHead, 0x804000FF);
      queuepoly(VHEAD, cgi.shPFace, 0xF09000FF);
      humanoid_eyes(V, 0x00D000FF);
      return false;
      }
    
    case moYeti: case moMonkey: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shYeti, darkena(col, 0, 0xC0));
      queuepoly(VHEAD1, cgi.shPHead, darkena(col, 0, 0xFF));
      humanoid_eyes(V, 0x000000FF, darkena(col, 0, 0xFF));
      return false;
      }
    
    case moResearcher: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, darkena(0xFFFF00, 0, 0xC0));
      queuepoly(VHEAD, cgi.shAztecHead, darkena(col, 0, 0xFF));
      queuepoly(VHEAD1, cgi.shAztecCap, darkena(0xC000C0, 0, 0xFF));
      humanoid_eyes(V, 0x000000FF);
      return false;
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
        queuepoly(VAHEAD, cgi.shFamiliarEye, darkena(0xFFFF000, 0, 0xFF));
        queuepoly(VAHEAD * Mirror, cgi.shFamiliarEye, darkena(0xFFFF000, 0, 0xFF));
        }
      return false;
      }
    
    case moRanger: {
      ShadowV(V, cgi.shPBody);
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      queuepoly(VBS, cgi.shPBody, darkena(col, 0, 0xC0));
      if(!peace::on) queuepoly(VBS, cgi.shPSword, darkena(col, 0, 0xFF));
      queuepoly(VHEAD, cgi.shArmor, darkena(col, 1, 0xFF));
      humanoid_eyes(V, 0x000000FF);
      return false;
      }
    
    case moNarciss: {
      ShadowV(V, cgi.shPBody);
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      queuepoly(VBS, cgi.shFlowerHand, darkena(col, 0, 0xFF));
      queuepoly(VBS, cgi.shPBody, 0xFFE080FF);
      if(!peace::on) queuepoly(VBS, cgi.shPKnife, 0xC0C0C0FF);
      queuepoly(VHEAD, cgi.shPFace, 0xFFE080FF);
      queuepoly(VHEAD1, cgi.shPHead, 0x806A00FF);
      humanoid_eyes(V, 0x000000FF);
      return false;
      }

    case moMirrorSpirit: {
      ShadowV(V, cgi.shPBody);
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0x90), m, footphase);
      queuepoly(VBS, cgi.shPBody, darkena(col, 0, 0x90));
      if(!peace::on) queuepoly(VBS * Mirror, cgi.shPSword, darkena(col, 0, 0xD0));
      queuepoly(VHEAD1, cgi.shPHead, darkena(col, 1, 0x90));
      queuepoly(VHEAD2, cgi.shPFace, darkena(col, 1, 0x90));
      queuepoly(VHEAD, cgi.shArmor, darkena(col, 0, 0xC0));
      humanoid_eyes(V, 0xFFFFFFFF, darkena(col, 0, 0xFF));
      return false;
      }
    
    case moJiangshi: {
      ShadowV(V, cgi.shJiangShi);
      auto z2 = WDIM == 3 ? 0 : GDIM == 3 ? -abs(sin(footphase * M_PI * 2)) * cgi.human_height/3 : geom3::lev_to_factor(abs(sin(footphase * M_PI * 2)) * cgi.human_height);
      auto V0 = V;
      auto V = mmscale(V0, z2);
      otherbodyparts(V, darkena(col, 0, 0xFF), m, m == moJiangshi ? 0 : footphase);
      queuepoly(VBODY, cgi.shJiangShi, darkena(col, 0, 0xFF));
      queuepoly(VBODY1, cgi.shJiangShiDress, darkena(0x202020, 0, 0xFF));
      queuepoly(VHEAD, cgi.shTerraHead, darkena(0x101010, 0, 0xFF));
      queuepoly(VHEAD1, cgi.shPFace, darkena(col, 0, 0xFF));
      queuepoly(VHEAD2, cgi.shJiangShiCap1, darkena(0x800000, 0, 0xFF));
      queuepoly(VHEAD3, cgi.shJiangShiCap2, darkena(0x400000, 0, 0xFF));
      humanoid_eyes(V, 0x000000FF, darkena(col, 0, 0xFF));
      return false;
      }
    
    case moGhost: case moSeep: case moFriendlyGhost: {
      if(m == moFriendlyGhost) col = fghostcolor(where);
      queuepolyat(VGHOST, cgi.shGhost, darkena(col, 0, m == moFriendlyGhost ? 0xC0 : 0x80), GDIM == 3 ? PPR::SUPERLINE : cgi.shGhost.prio);
      queuepolyat(VGHOST, cgi.shGhostEyes, 0xFF, GDIM == 3 ? PPR::SUPERLINE : cgi.shEyes.prio);
      return false;
      }
    
    case moVineSpirit: {
      queuepoly(VGHOST, cgi.shGhost, 0xD0D0D0C0 | UNTRANS);
      queuepolyat(VGHOST, cgi.shGhostEyes, 0xFF0000FF, GDIM == 3 ? PPR::SUPERLINE : cgi.shGhostEyes.prio);
      return false;
      }
    
    case moFireFairy: {
      col = firecolor(0);
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shFemaleBody);
      queuepoly(VBS, cgi.shFemaleBody, darkena(col, 0, 0XC0));
      queuepoly(VHEAD, cgi.shWitchHair, darkena(col, 1, 0xFF));
      queuepoly(VHEAD1, cgi.shPFace, darkena(col, 0, 0XFF));
      humanoid_eyes(V, darkena(col, 1, 0xFF));
      return false;
      }      
      
    case moSlime: {
      queuepoly(VFISH, cgi.shSlime, darkena(col, 0, 0x80));
      queuepoly(VSLIMEEYE, cgi.shSlimeEyes, 0xFF);
      return false;
      }

    case moKrakenH: {
      queuepoly(VFISH, cgi.shKrakenHead, darkena(col, 0, 0xD0));
      queuepoly(VFISH, cgi.shKrakenEye, 0xFFFFFFC0 | UNTRANS);
      queuepoly(VFISH, cgi.shKrakenEye2, 0xC0);
      queuepoly(VFISH * Mirror, cgi.shKrakenEye, 0xFFFFFFC0 | UNTRANS);
      queuepoly(VFISH * Mirror, cgi.shKrakenEye2, 0xC0);
      return false;
      }

    case moKrakenT: {
      queuepoly(VFISH, cgi.shSeaTentacle, darkena(col, 0, 0xD0));
      return false;
      }

    case moCultist: case moPyroCultist: case moCultistLeader: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, darkena(col, 0, 0xC0));
      if(!peace::on) queuepoly(VBS, cgi.shPSword, darkena(col, 2, 0xFF));
      queuepoly(VHEAD, cgi.shHood, darkena(col, 1, 0xFF));
      humanoid_eyes(V, 0x00FF00FF);
      return false;
      }

    case moPirate: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, darkena(0x404040, 0, 0xFF));
      queuepoly(VBS, cgi.shPirateHook, darkena(0xD0D0D0, 0, 0xFF));
      queuepoly(VHEAD, cgi.shPFace, darkena(0xFFFF80, 0, 0xFF));
      queuepoly(VHEAD1, cgi.shEyepatch, darkena(0, 0, 0xC0));
      queuepoly(VHEAD2, cgi.shPirateHood, darkena(col, 0, 0xFF));
      humanoid_eyes(V, 0x000000FF);
      return false;
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
        transmatrix V1 = V * zpush(cgi.AHEAD - zc(0.4) - zc(0.98) + cgi.HEAD); // * cpush(0, cgi.scalefactor * (-0.1));
        queuepoly(V1, cgi.shRatHead, darkena(col, 0, 0xFF));

        /*
        queuepoly(V1, cgi.shFamiliarEye, darkena(eyecol, 0, 0xFF));
        queuepoly(V1 * Mirror, cgi.shFamiliarEye, darkena(eyecol, 0, 0xFF));
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
      return false;
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
      return false;
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
      return false;
      }

    case moNecromancer: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shPBody);
      queuepoly(VBS, cgi.shPBody, 0xC00000C0 | UNTRANS);
      queuepoly(VHEAD, cgi.shHood, darkena(col, 1, 0xFF));
      humanoid_eyes(V, 0xFF0000FF);
      return false;
      }

    case moDraugr: {
      const transmatrix VBS = VBODY * otherbodyparts(V, 0x483828D0 | UNTRANS, m, footphase);
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
      queuepoly(VHEAD1, cgi.shWightCloak, (0x605040A0 | UNTRANS) + 0x10101000 * b);
      return false;
      }

    case moVoidBeast: {
      const transmatrix VBS = VBODY * otherbodyparts(V, 0x080808D0 | UNTRANS, m, footphase);
      queuepoly(VBS, cgi.shPBody, 0x080808D0 | UNTRANS);
      queuepoly(VHEAD, cgi.shPHead, 0x080808D0 | UNTRANS);
      queuepoly(VHEAD, cgi.shWightCloak, 0xFF0000A0 | UNTRANS);
      humanoid_eyes(V, 0xFF0000FF, 0x080808FF);
      return false;
      }

    case moGoblin: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shYeti);
      queuepoly(VBS, cgi.shYeti, darkena(col, 0, 0xC0));
      queuepoly(VHEAD, cgi.shArmor, darkena(col, 1, 0XFF));
      humanoid_eyes(V, 0x800000FF, darkena(col, 0, 0xFF));
      return false;
      }

    case moLancer: case moFlailer: case moMiner: {
      transmatrix V2 = V;
      if(m == moLancer)
        V2 = V * spin((where && where->type == 6) ? -M_PI/3 : -M_PI/2 );
      transmatrix Vh = mmscale(V2, cgi.HEAD);
      transmatrix Vb = mmscale(V2, cgi.BODY);
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
      return false;
      }

    case moTroll: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shYeti);
      queuepoly(VBS, cgi.shYeti, darkena(col, 0, 0xC0));
      queuepoly(VHEAD1, cgi.shPHead, darkena(col, 1, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, darkena(col, 2, 0XFF));
      humanoid_eyes(V, 0x004000FF, darkena(col, 0, 0xFF));
      return false;
      }        

    case moFjordTroll: case moForestTroll: case moStormTroll: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shYeti);
      queuepoly(VBS, cgi.shYeti, darkena(col, 0, 0xC0));
      queuepoly(VHEAD1, cgi.shPHead, darkena(col, 1, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, darkena(col, 2, 0XFF));
      humanoid_eyes(V, 0x004000FF, darkena(col, 0, 0xFF));
      return false;
      }

    case moDarkTroll: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shYeti);
      queuepoly(VBS, cgi.shYeti, darkena(col, 0, 0xC0));
      queuepoly(VHEAD1, cgi.shPHead, darkena(col, 1, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, 0xFFFFFF80 | UNTRANS);
      humanoid_eyes(V, 0x000000FF, darkena(col, 0, 0xFF));
      return false;
      }        

    case moRedTroll: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shYeti);
      queuepoly(VBS, cgi.shYeti, darkena(col, 0, 0xC0));
      queuepoly(VHEAD1, cgi.shPHead, darkena(0xFF8000, 0, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, 0xFFFFFF80 | UNTRANS);
      humanoid_eyes(V, 0x000000FF, darkena(col, 0, 0xFF));
      return false;
      }        

    case moEarthElemental: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
      ShadowV(V, cgi.shWaterElemental);
      queuepoly(VBS, cgi.shWaterElemental, darkena(col, 0, 0xC0));
      queuepoly(VHEAD1, cgi.shFemaleHair, darkena(col, 0, 0XFF));
      queuepoly(VHEAD, cgi.shPFace, 0xF0000080 | UNTRANS);
      humanoid_eyes(V, 0xD0D000FF, darkena(col, 1, 0xFF));
      return false;
      }        

    case moWaterElemental: {
      const transmatrix VBS = VBODY * otherbodyparts(V, watercolor(50), m, footphase);
      ShadowV(V, cgi.shWaterElemental);
      queuepoly(VBS, cgi.shWaterElemental, watercolor(0));
      queuepoly(VHEAD1, cgi.shFemaleHair, watercolor(100));
      queuepoly(VHEAD, cgi.shPFace, watercolor(200));
      humanoid_eyes(V, 0x0000FFFF, watercolor(150));
      return false;
      }        

    case moFireElemental: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(firecolor(50), 0, 0xFF), m, footphase);
      ShadowV(V, cgi.shWaterElemental);
      queuepoly(VBS, cgi.shWaterElemental, darkena(firecolor(0), 0, 0xFF));
      queuepoly(VHEAD1, cgi.shFemaleHair, darkena(firecolor(100), 0, 0xFF));
      queuepoly(VHEAD, cgi.shPFace, darkena(firecolor(200), 0, 0xFF));
      humanoid_eyes(V, darkena(firecolor(200), 0, 0xFF), darkena(firecolor(50), 0, 0xFF));
      return false;
      }

    case moAirElemental: {
      const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0x40), m, footphase);
      ShadowV(V, cgi.shWaterElemental);
      queuepoly(VBS, cgi.shWaterElemental, darkena(col, 0, 0x80));
      queuepoly(VHEAD1, cgi.shFemaleHair, darkena(col, 0, 0x80));
      queuepoly(VHEAD, cgi.shPFace, darkena(col, 0, 0x80));
      humanoid_eyes(V, 0xFFFFFFFF, darkena(col, 1, 0xFF));
      return false;
      }        

    case moWorm: case moWormwait: case moHexSnake: {
      queuepoly(V, cgi.shWormHead, darkena(col, 0, 0xFF));
      queuepolyat(V, cgi.shWormEyes, 0xFF, PPR::ONTENTACLE_EYES);
      return false;
      }

    case moDragonHead: {
      queuepoly(V, cgi.shDragonHead, darkena(col, 0, 0xFF));
      queuepolyat(V, cgi.shDragonEyes, 0xFF, PPR::ONTENTACLE_EYES);          
      int noscolor = 0xFF0000FF;
      queuepoly(V, cgi.shDragonNostril, noscolor);
      queuepoly(V * Mirror, cgi.shDragonNostril, noscolor);
      return false;
      }

    case moDragonTail: {
      queuepoly(V, cgi.shDragonSegment, darkena(col, 0, 0xFF));
      return false;
      }

    case moTentacle: case moTentaclewait: case moTentacleEscaping: {
      queuepoly(V, cgi.shTentHead, darkena(col, 0, 0xFF));
      ShadowV(V, cgi.shTentHead, PPR::GIANTSHADOW);
      return false;
      }
    
    case moAsteroid: {
      queuepoly(V, cgi.shAsteroid[1], darkena(col, 0, 0xFF));
      return false;
      }

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
    queuepoly(VAHEAD * Mirror, cgi.shBullHorn, darkena(0xFFFFFF, 0, 0xFF));
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
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    queuepoly(VBS, cgi.shPBody, darkena(col, 1, 0xC0));
    ShadowV(V, cgi.shPBody);
    int acol = col;
    if(xch == 'D') acol = 0xD0D0D0;
    queuepoly(VHEAD, cgi.shDemon, darkena(acol, 0, 0xFF));
    humanoid_eyes(V, 0xFF0000FF, 0xC00000FF);
    }
  else if(isMagneticPole(m)) {
    if(m == moNorthPole)
      queuepolyat(VBODY * spin(M_PI), cgi.shTentacle, 0x000000C0, PPR::TENTACLE1);
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
  else if(isWitch(m)) {
    const transmatrix VBS = VBODY * otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    int cc = 0xFF;
    if(m == moWitchGhost) cc = 0x85 + 120 * sintick(160);
    if(m == moWitchWinter && where) drawWinter(V, 0);
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
    queuepoly(V, cgi.shILeaf[0], darkena(col, 0, 0xFF));
    }

  else return true;
  
  return false;
#else
  return true;
#endif
  }

bool drawMonsterTypeDH(eMonster m, cell *where, const transmatrix& V, color_t col, bool dh, ld footphase, color_t asciicol) {
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

EX transmatrix playerV;

EX bool applyAnimation(cell *c, transmatrix& V, double& footphase, int layer) {
  if(!animations[layer].count(c)) return false;
  animation& a = animations[layer][c];

  int td = ticks - a.ltick;
  ld aspd = td / 1000.0 * exp(vid.mspeed);
  ld R;
  again:
  
  if(sl2) {
    a.wherenow = slr::translate(tC0(a.wherenow));
    hyperpoint h = tC0(inverse(a.wherenow));
    hyperpoint ie = slr::get_inverse_exp(h, 0);
    auto R = hypot_d(3, ie);
    aspd *= (1+R+(shmup::on?1:0));
    if(R < aspd || std::isnan(R) || std::isnan(aspd) || R > 10) {
      animations[layer].erase(c);
      return false;
      }
    a.wherenow = nisot::parallel_transport(a.wherenow, tangent_length(h, aspd));
    a.footphase += a.attacking == 2 ? -aspd : aspd;
    // todo attack animation, rotate correctly
    footphase = a.footphase;
    V = V * a.wherenow;
    a.ltick = ticks;
    return true;
    }

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
    hyperpoint wnow;
    if(a.attacking == 1)
      wnow = tC0(inverse(a.wherenow) * a.attackat);
    else
      wnow = tC0(inverse(a.wherenow));
    
    if(prod) {
      auto d = product_decompose(wnow);
      ld dist = d.first / R * aspd;
      if(abs(dist) > abs(d.first)) dist = -d.first;
      a.wherenow = mscale(a.wherenow, dist);
      /* signed_sqrt to prevent precision errors */
      aspd *= signed_sqrt(R*R - d.first * d.first) / R;
      }
    a.wherenow = a.wherenow * rspintox(wnow);
    a.wherenow = a.wherenow * xpush(aspd);
    fixmatrix(a.wherenow);
    a.footphase += a.attacking == 2 ? -aspd : aspd;
    footphase = a.footphase;
    V = V * a.wherenow;
    if(a.mirrored) V = V * Mirror;
    if(a.attacking == 2) V = V * pispin;
    // if(GDIM == 3) V = V * cspin(0, 2, M_PI/2);
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
bool chainAnimation(cell *c, cell *c2, transmatrix& V, const transmatrix &Vwhere, ld& length) {
  hyperpoint h = C0;
  if(animations[LAYER_BIG].count(c2)) h = animations[LAYER_BIG][c2].wherenow * h;
  h = inverse(V) * Vwhere * h;
  length = hdist0(h);
  V = V * rspintox(h);
  return true;  
  }

// push down the queue after q-th element, `down` absolute units down,
// based on cell c and transmatrix V
// do change the zoom factor? do change the priorities?

EX int cellcolor(cell *c) {
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

EX bool dont_face_pc = false;

EX bool drawMonster(const transmatrix& Vparam, int ct, cell *c, color_t col, bool mirrored, color_t asciicol) {
  #if CAP_SHAPES

  bool darkhistory = history::includeHistory && history::inkillhistory.count(c);
  
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
  
  bool half_elliptic = elliptic && GDIM == 3 && WDIM == 2;
  
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
  
    transmatrix Vb0 = Vb;
    if(c->mondir != NODIR && GDIM == 3 && isAnyIvy(c)) {
      queueline(tC0(Vparam), Vparam  * tC0(currentmap->adj(c, c->mondir)), (col << 8) + 0xFF, 0);
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
        
        if(c->monmirror) Vb = Vb * Mirror;
  
        if(mapeditor::drawUserShape(Vb, mapeditor::sgMonster, c->monst, (col << 8) + 0xFF, c)) 
          return false;

        if(isIvy(c) || isMutantIvy(c) || c->monst == moFriendlyIvy)
          queuepoly(Vb, cgi.shIBranch, (col << 8) + 0xFF);
/*         else if(c->monst < moTentacle && wormstyle == 0) {
          ShadowV(Vb, cgi.shTentacleX, PPR::GIANTSHADOW);
          queuepoly(mmscale(Vb, cgi.ABODY), cgi.shTentacleX, 0xFF);
          queuepoly(mmscale(Vb, cgi.ABODY), cgi.shTentacle, (col << 8) + 0xFF);
          } */
//        else if(c->monst < moTentacle) {
//          }

        else if(c->monst == moDragonHead || c->monst == moDragonTail) {
          char part = dragon::bodypart(c, dragon::findhead(c));
          if(part != '2') {
            queuepoly(mmscale(Vb, cgi.ABODY), cgi.shDragonSegment, darkena(col, 0, 0xFF));
            ShadowV(Vb, cgi.shDragonSegment, PPR::GIANTSHADOW);
            }
          }
        else {
          if(c->monst == moTentacleGhost) {
            hyperpoint V0 = history::on ? tC0(Vs) : inverse(cwtV) * tC0(Vs);
            hyperpoint V1 = spintox(V0) * V0;
            Vs = cwtV * rspintox(V0) * rpushxto0(V1) * pispin;
            drawMonsterType(moGhost, c, Vs, col, footphase, asciicol);
            col = minf[moTentacletail].color;
            }
          /*
          queuepoly(mmscale(Vb, cgi.ABODY), cgi.shTentacleX, 0xFFFFFFFF);
          queuepoly(mmscale(Vb, cgi.ABODY), cgi.shTentacle, (col << 8) + 0xFF);
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
              transmatrix Vbx = Vb;
              if(WDIM == 2) Vbx = Vbx * spin(sin(M_PI * i / 6.) * wav / (i+.1));
              Vbx = Vbx * xpush(length * (i) / 12.0);
              // transmatrix Vbx2 = Vnext * xpush(length2 * i / 6.0);
              // Vbx = Vbx * rspintox(inverse(Vbx) * Vbx2 * C0) * pispin;
              ShadowV(Vbx, sh, PPR::GIANTSHADOW);
              queuepoly(mmscale(Vbx, cgi.ABODY), sh, (col0 << 8) + 0xFF);
              }
            });
          }
        }
        
      else {
        transmatrix T = Vparam * ddspin(c, c->mondir);
        color_t col = darkena(0x606020, 0, 0xFF);
        for(int u=-1; u<=1; u++)
          queueline(T*xspinpush0(M_PI/2, u*cgi.crossf/5), T*xspinpush(0, cgi.crossf)*xspinpush0(M_PI/2, u*cgi.crossf/5), col, 2 + vid.linequality);
        }
      }

    if(mmmon) {
      if(isAnyIvy(c)) {
        if(hybri) {
          queuepoly(Vb, cgi.shILeaf[ctof(c)], darkena(col, 0, 0xFF));
          for(int a=0; a<c->type-2; a++)
            queuepoly(Vb * spin(a * 2 * M_PI / (c->type-2)), cgi.shILeaf[2], darkena(col, 0, 0xFF));
          }
        else if(GDIM == 3) {
          hyperpoint V0 = tC0(Vb);
          transmatrix Vs = rspintox(V0) * xpush(hdist0(V0)) * cspin(0, 2, -M_PI/2);
          queuepoly(Vs, cgi.shILeaf[1], darkena(col, 0, 0xFF));
          }
        else {
          if(c->monmirror) Vb = Vb * Mirror;
          queuepoly(mmscale(Vb, cgi.ABODY), cgi.shILeaf[ctof(c)], darkena(col, 0, 0xFF));
          ShadowV(Vb, cgi.shILeaf[ctof(c)], PPR::GIANTSHADOW);
          }
        }
      else if(m == moWorm || m == moWormwait || m == moHexSnake) {
        Vb = Vb * pispin;
        if(c->monmirror) Vb = Vb * Mirror;
        transmatrix Vbh = mmscale(Vb, cgi.AHEAD);
        queuepoly(Vbh, cgi.shWormHead, darkena(col, 0, 0xFF));
        queuepolyat(Vbh, cgi.shWormEyes, 0xFF, PPR::ONTENTACLE_EYES);
        ShadowV(Vb, cgi.shWormHead, PPR::GIANTSHADOW);
        }
      else if(m == moDragonHead) {
        if(c->monmirror) Vb = Vb * Mirror;
        transmatrix Vbh = mmscale(Vb, cgi.AHEAD);
        ShadowV(Vb, cgi.shDragonHead, PPR::GIANTSHADOW);
        queuepoly(Vbh, cgi.shDragonHead, darkena(col, c->hitpoints?0:1, 0xFF));
        queuepolyat(Vbh/* * pispin */, cgi.shDragonEyes, 0xFF, PPR::ONTENTACLE_EYES);
        
        int noscolor = (c->hitpoints == 1 && c->stuntime ==1) ? 0xFF0000FF : 0xFF;
        queuepoly(Vbh, cgi.shDragonNostril, noscolor);
        queuepoly(Vbh * Mirror, cgi.shDragonNostril, noscolor);
        }
      else if(m == moTentacle || m == moTentaclewait || m == moTentacleEscaping) {
        Vb = Vb * pispin;
        if(c->monmirror) Vb = Vb * Mirror;
        transmatrix Vbh = mmscale(Vb, cgi.AHEAD);
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
            Vb = Vb * pispin;
            }
          else {
            Vb = Vb0 * ddspin(c, nd, M_PI);
            }
          if(c->monmirror) Vb = Vb * Mirror;
          transmatrix Vbb = mmscale(Vb, cgi.ABODY);
          queuepoly(Vbb, cgi.shDragonTail, darkena(col, c->hitpoints?0:1, 0xFF));
          ShadowV(Vb, cgi.shDragonTail, PPR::GIANTSHADOW);
          }
        else if(true) {
          if(nospinb) {
            ld length;
            chainAnimation(c, c2, Vb, Vparam * currentmap->adj(c, nd), length);
            Vb = Vb * pispin;
            double ang = chainAngle(c, Vb, c->move(c->mondir), currentmap->spin_angle(c, c->mondir) - currentmap->spin_angle(c, nd), Vparam);
            ang /= 2;
            Vb = Vb * spin(M_PI-ang);
            }
          else {
            /* todo what if no spin_angle */
            ld hdir0 = currentmap->spin_angle(c, nd) + M_PI;
            ld hdir1 = currentmap->spin_angle(c, c->mondir);
            while(hdir1 > hdir0 + M_PI) hdir1 -= 2*M_PI;
            while(hdir1 < hdir0 - M_PI) hdir1 += 2*M_PI;
            Vb = Vb0 * spin((hdir0 + hdir1)/2 + M_PI);
            }
          if(c->monmirror) Vb = Vb * Mirror;
          transmatrix Vbb = mmscale(Vb, cgi.ABODY);
          if(part == 'l' || part == '2') {
            queuepoly(Vbb, cgi.shDragonLegs, darkena(col, c->hitpoints?0:1, 0xFF));
            }
          queuepoly(Vbb, cgi.shDragonWings, darkena(col, c->hitpoints?0:1, 0xFF));
          }
        }
      else {
        if(c->monst == moTentacletail && c->mondir == NODIR) {
          if(c->monmirror) Vb = Vb * Mirror;
          queuepoly(GDIM == 3 ? mmscale(Vb, cgi.ABODY) : Vb, cgi.shWormSegment, darkena(col, 0, 0xFF));
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
            Vb = Vb * pispin;
            }
          else {
            Vb = Vb0 * ddspin(c, nd, M_PI);
            }
          if(c->monmirror) Vb = Vb * Mirror;
          transmatrix Vbb = mmscale(Vb, cgi.ABODY) * pispin;
          hpcshape& sh = hexsnake ? cgi.shWormTail : cgi.shSmallWormTail;
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
      if(mirrored != mirr && half_elliptic) continue;
      transmatrix T = Id;
      nospins = applyAnimation(cwt.at, T, footphase, LAYER_SMALL);
      if(nospins) 
        Vs = Vs * ddspin(c, cw.spin, 0) * iddspin(cwt.at, cwt.spin, 0) * T;
      else
        Vs = Vs * ddspin(c, cw.spin, 0);
      if(mirr) Vs = Vs * Mirror;
      if(inmirrorcount&1) mirr = !mirr;
      col = mirrorcolor(geometry == gElliptic ? det(Vs) < 0 : mirr);
      if(!mouseout() && !nospins && GDIM == 2) {
        hyperpoint P2 = Vs * inverse(cwtV) * mouseh;
        queuechr(P2, 10, 'x', 0xFF00);
        }     
      if(!nospins && flipplayer) Vs = Vs * pispin;
      if(mmmon) {
        drawMonsterType(moMimic, c, Vs, col, footphase, asciicol);
        drawPlayerEffects(Vs, c, false);
        }
      }
    return !mmmon;
    }
  
  else if(!mmmon) return true;
  
  // illusions face randomly
  
  else if(c->monst == moIllusion) {
    multi::cpid = 0;
    if(c->monmirror) Vs = Vs * Mirror;
    drawMonsterType(c->monst, c, Vs, col, footphase, asciicol);
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
      Vs = Vs * ddspin(c, d, 0);
      }
    if(c->monmirror) Vs = Vs * Mirror;
    return drawMonsterTypeDH(m, c, Vs, col, darkhistory, footphase, asciicol);
    }

  else if(c->monst == moKrakenT) {
    if(c->hitpoints == 0) col = 0x404040;
    if(nospinb) {
      ld length;
      chainAnimation(c, c->move(c->mondir), Vb, Vparam * currentmap->adj(c, c->mondir), length);
      Vb = Vb * pispin;
      Vb = Vb * xpush(cgi.tentacle_length - cellgfxdist(c, c->mondir));
      }
    else if(NONSTDVAR) {
      transmatrix T = currentmap->adj(c, c->mondir);
      Vb = Vb * T * rspintox(tC0(inverse(T))) * xpush(cgi.tentacle_length);
      }
    else {
      Vb = Vb * ddspin(c, c->mondir, M_PI);
      Vb = Vb * xpush(cgi.tentacle_length - cellgfxdist(c, c->mondir));
      }
    if(c->monmirror) Vb = Vb * Mirror;
      
      // if(ctof(c) && !masterless) Vb = Vb * xpush(hexhexdist - hcrossf);
      // return (!BITRUNCATED) ? tessf * gp::scale : (c->type == 6 && (i&1)) ? hexhexdist : cgi.crossf;
    return drawMonsterTypeDH(m, c, Vb, col, darkhistory, footphase, asciicol);
    }

  // golems, knights, and hyperbugs don't face the player (mondir-controlled)
  // also whatever in the lineview mode, and whatever in the quotient geometry

  else if((hasFacing(c) && c->mondir != NODIR) || history::on || quotient || euwrap || dont_face_pc) {
    if(c->monst == moKrakenH) Vs = Vb, nospins = nospinb;
    if(!nospins && c->mondir < c->type) Vs = Vs * ddspin(c, c->mondir, M_PI);
    if(c->monst == moPair) Vs = Vs * xpush(-.12);
    if(c->monmirror) Vs = Vs * Mirror;
    if(isFriendly(c)) drawPlayerEffects(Vs, c, false);
    return drawMonsterTypeDH(m, c, Vs, col, darkhistory, footphase, asciicol);
    }

  else {
    // other monsters face the player
    
    if(!nospins) {
      if(WDIM == 2 || prod) {
        hyperpoint V0 = inverse(cwtV) * tC0(Vs);
        ld z = 0;
        if(prod) {
          auto d = product_decompose(V0);
          z = d.first;
          V0 = d.second;
          }
          
        hyperpoint V1 = spintox(V0) * V0;
  
        if(hypot_d(2, tC0(Vs)) > 1e-3) {
          Vs = cwtV * rspintox(V0) * rpushxto0(V1) * pispin;
          if(prod) Vs = mscale(Vs, z);
          }
        }
      else if(!sl2) {
        hyperpoint V0 = inverse(cwtV) * tC0(Vs);
        Vs = cwtV * rspintox(V0) * xpush(hdist0(V0)) * cspin(0, 2, -M_PI);
        // cwtV * rgpushxto0(inverse(cwtV) * tC0(Vs));
        }
      if(c->monst == moHunterChanging)  
        Vs = Vs * (hybri ? spin(M_PI) : cspin(WDIM-2, WDIM-1, M_PI));
      }
    if(c->monmirror) Vs = Vs * Mirror;
    
    if(c->monst == moShadow) 
      multi::cpid = c->hitpoints;
    
    return drawMonsterTypeDH(m, c, Vs, col, darkhistory, footphase, asciicol);
    }
  
  for(int i=0; i<numplayers(); i++) if(c == playerpos(i) && !shmup::on && mapeditor::drawplayer && 
    !(hardcore && !canmove)) {
    bool mirr = multi::players > 1 ? multi::player[i].mirrored : cwt.mirrored;
    if(half_elliptic && mirr != mirrored) continue;
    if(!nospins) {
      Vs = playerV;
      if(multi::players > 1 ? multi::flipped[i] : flipplayer) Vs = Vs * pispin;
      }
    else {
      if(mirr) Vs = Vs * Mirror;
      }
    multi::cpid = i;

    drawPlayerEffects(Vs, c, true);
    if(!mmmon) return true;
    
    if(hide_player()) {
      first_cell_to_draw = false;
      if(WDIM == 2 && GDIM == 3)
        drawPlayer(moPlayer, c, Vs, col, footphase, true);
      }
    
    else if(isWorm(m)) {
      ld depth = geom3::factor_to_lev(wormhead(c) == c ? cgi.AHEAD : cgi.ABODY);
      footphase = 0;
      int q = ptds.size();
      drawMonsterType(moPlayer, c, Vs, col, footphase, asciicol);
      pushdown(c, q, Vs, -depth, true, false);
      }
    
    else if(mmmon)
      drawMonsterType(moPlayer, c, Vs, col, footphase, asciicol);
    }

#endif
  return false;
  }

EX cell *straightDownSeek;
EX hyperpoint straightDownPoint;
EX ld straightDownSpeed;

#define AURA 180

array<array<int,4>,AURA+1> aurac;

int haveaura_cached;

/** 0 = no aura, 1 = standard aura, 2 = Joukowsky aura */
EX int haveaura() {
  if(!(vid.aurastr>0 && !svg::in && (auraNOGL || vid.usingGL))) return 0;
  if(sphere && mdAzimuthalEqui()) return 0;
  if(among(pmodel, mdJoukowsky, mdJoukowskyInverted) && hyperbolic && models::model_transition < 1) 
    return 2;
  if(pmodel == mdFisheye) return 1;
  return pmodel == mdDisk && (!sphere || vid.alpha > 10) && !euclid;
  }
  
vector<pair<int, int> > auraspecials; 

int auramemo;

EX void clearaura() {
  haveaura_cached = haveaura();
  if(!haveaura_cached) return;
  for(int a=0; a<AURA; a++) for(int b=0; b<4; b++) 
    aurac[a][b] = 0;
  auraspecials.clear();
  auramemo = 128 * 128 / vid.aurastr;
  }

void apply_joukowsky_aura(hyperpoint& h) {
  if(haveaura_cached == 2)  {
    hyperpoint ret;
    applymodel(h, ret);
    h = ret;
    }
  if(nonisotropic) {
    h = lp_apply(inverse_exp(h, iTable, true));
    }
  }

EX void addauraspecial(hyperpoint h, color_t col, int dir) {
  if(!haveaura_cached) return;
  apply_joukowsky_aura(h);
  int r = int(2*AURA + dir + atan2(h[1], h[0]) * AURA / 2 / M_PI) % AURA; 
  auraspecials.emplace_back(r, col);
  }

EX void addaura(hyperpoint h, color_t col, int fd) {
  if(!haveaura_cached) return;
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
  if(hyperbolic && pmodel == mdFisheye) {
    ld h = 1;
    h /= vid.fisheye_param;
    ld nrad = h / sqrt(2 + h*h);
    rad *= nrad;
    }
  
  for(int v=0; v<4; v++) sumaura(v);
  for(auto& p: auraspecials) {
    int r = p.first;
    aurac[r][3] = auramemo;
    for(int k=0; k<3; k++) aurac[r][k] = (p.second >> (16-8*k)) & 255;
    }

#if CAP_SDL || CAP_GL
  ld bak[3];
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
      ld fr = alpha-rm;
      
      if(rm<0 || rm >= AURA) continue;
      
      color_t& p = qpixel(s, x, y);
      for(int c=0; c<3; c++) {
        ld c1 = aurac[rm][2-c] / (aurac[rm][3]+.1);
        ld c2 = aurac[rm+1][2-c] / (aurac[rm+1][3]+.1);
        const ld one = 1;
        part(p, c) = int(255 * min(one, bak[2-c] + cmul * ((c1 + fr * (c2-c1) - bak[2-c])))); 
        }
      }
    SDL_UnlockSurface(s);
    return;
    }
#endif

#if CAP_GL
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
  bool joukowsky = among(pmodel, mdJoukowskyInverted, mdJoukowsky) && hyperbolic && models::model_transition < 1;

  for(int r=0; r<=AURA; r++) for(int z=0; z<11; z++) {
    float rr = (M_PI * 2 * r) / AURA;
    float rad0 = inversion ? rad / facs[z] : rad * facs[z];
    int rm = r % AURA;
    ld c = cos(rr);
    ld s = sin(rr);

    if(joukowsky) {
      ld c1 = c, s1 = s;
      if(inversion)
        models::apply_orientation(s1, c1);
      else        
        models::apply_orientation(c1, s1);

      ld& mt = models::model_transition;
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
  current_display->next_shader_flags = GF_VARCOLOR;
  dynamicval<eModel> m(pmodel, mdPixel);
  current_display->set_all(0);
  glhr::id_modelview();
  glhr::prepare(auravertices);
  glhr::set_depthtest(false);
  glDrawArrays(GL_TRIANGLES, 0, isize(auravertices));
#endif
  }

// int fnt[100][7];

bool bugsNearby(cell *c, int dist = 2) {
  if(!(havewhat&HF_BUG)) return false;
  if(isBug(c)) return true;
  if(dist) for(int t=0; t<c->type; t++) if(c->move(t) && bugsNearby(c->move(t), dist-1)) return true;
  return false;
  }

EX colortable minecolors = {
  0xFFFFFF, 0xF0, 0xF060, 0xF00000, 
  0x60, 0x600000, 0x00C0C0, 0x000000, 0x808080, 0xFFD500
  };

EX colortable distcolors = {
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

EX int countMinesAround(cell *c) {
  int mines = 0;
  for(cell *c2: adj_minefield_cells(c))
    if(c2->wall == waMineMine)
      mines++;
  return mines;
  }

EX transmatrix applyPatterndir(cell *c, const patterns::patterninfo& si) {
  if(NONSTDVAR || binarytiling) return Id;
  transmatrix V = ddspin(c, si.dir, M_PI);
  if(si.reflect) V = V * Mirror;
  return V * iddspin(c, 0, M_PI);
  }

EX transmatrix applyDowndir(cell *c, const cellfunction& cf) {
  return ddspin(c, patterns::downdir(c, cf), M_PI);
  }

void draw_movement_arrows(cell *c, const transmatrix& V) {

  if(viewdists) return;

  for(int d=0; d<8; d++) {
  
    movedir md = vectodir(spin(-d * M_PI/4) * smalltangent());
    int u = md.d;
    cellwalker xc = cwt + u + wstep;
    if(xc.at == c) {
      transmatrix fixrot = sphereflip * rgpushxto0(sphereflip * tC0(V));
      // make it more transparent
      color_t col = getcs().uicolor;
      col -= (col & 0xFF) >> 1;
      poly_outline = OUTLINE_DEFAULT;
      queuepoly(fixrot * spin(-d * M_PI/4), cgi.shArrow, col);

      if((c->type & 1) && (isStunnable(c->monst) || isPushable(c->wall))) {
        transmatrix Centered = rgpushxto0(tC0(cwtV));
        int sd = md.subdir;
        queuepoly(inverse(Centered) * rgpushxto0(Centered * tC0(V)) * rspintox(Centered*tC0(V)) * spin(-sd * M_PI/S7) * xpush(0.2), cgi.shArrow, col);
        }
      else if(!confusingGeometry()) break;
      }
    }
  }

EX int celldistAltPlus(cell *c) { return 1000000 + celldistAlt(c); }

bool drawstaratvec(double dx, double dy) {
  return dx*dx+dy*dy > .05;
  }

EX color_t reptilecolor(cell *c) {
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

  color_t reptilecolors[4] = {0xe3bb97, 0xc2d1b0, 0xebe5cb, 0xA0A0A0};
  return reptilecolors[i];
  }

ld wavefun(ld x) { 
  return sin(x);
  /* x /= (2*M_PI);
  x -= (int) x;
  if(x > .5) return (x-.5) * 2;
  else return 0; */
  }

EX colortable nestcolors = { 0x800000, 0x008000, 0x000080, 0x404040, 0x700070, 0x007070, 0x707000, 0x606060 };

color_t floorcolors[landtypes];

EX void init_floorcolors() {
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

EX color_t magma_color(int id) {
  if(id == 95/4-1) return 0x200000;
  else if(id == 95/4) return 0x100000;
  else if(id < 48/4) return gradient(0xF0F000, 0xF00000, 0, id, 48/4);
  else if(id < 96/4) return gradient(0xF00000, 0x400000, 48/4, id, 95/4-2);
  else return winf[waMagma].color;
  }

bool noAdjacentChasms(cell *c) {
  forCellEx(c2, c) if(c2->wall == waChasm) return false;
  return true;
  }

// does the current geometry allow nice duals
EX bool has_nice_dual() {
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
EX bool is_nice_dual(cell *c) {
  return c->land == laDual && has_nice_dual();
  }

EX bool use_swapped_duals() {
  return (masterless && !a4) || GOLDBERG;
  }

#if CAP_SHAPES
EX void floorShadow(cell *c, const transmatrix& V, color_t col) {
  if(model_needs_depth() || noshadow) 
    return; // shadows break the depth testing
  dynamicval<color_t> p(poly_outline, OUTLINE_TRANS);
  if(qfi.shape) {
    queuepolyat(V * qfi.spin * cgi.shadowmulmatrix, *qfi.shape, col, PPR::WALLSHADOW);
    }
  else if(qfi.usershape >= 0)
    mapeditor::drawUserShape(V * qfi.spin * cgi.shadowmulmatrix, mapeditor::sgFloor, qfi.usershape, col, c, PPR::WALLSHADOW);
  else 
    draw_shapevec(c, V, qfi.fshape->shadow, col, PPR::WALLSHADOW);
  }

EX bool use_warp_graphics() {
  if(shmup::on) return false;
  if(geosupport_football() != 2) return false;
  if(chaosmode == 3 || chaosmode == 4) return false;
  return true;
  }

EX void escherSidewall(cell *c, int sidepar, const transmatrix& V, color_t col) {
  if(sidepar >= SIDE_SLEV && sidepar <= SIDE_SLEV+2) {
    int sl = sidepar - SIDE_SLEV;
    for(int z=1; z<=4; z++) if(z == 1 || (z == 4 && detaillevel == 2))
      draw_qfi(c, mscale(V, zgrad0(cgi.slev * sl, cgi.slev * (sl+1), z, 4)), col, PPR::REDWALL-4+z+4*sl);
    }
  else if(sidepar == SIDE_WALL) {
    const int layers = 2 << detaillevel;
    for(int z=1; z<layers; z++) 
      draw_qfi(c, mscale(V, zgrad0(0, geom3::actual_wall_height(), z, layers)), col, PPR::WALL3+z-layers);
    }
  else if(sidepar == SIDE_LAKE) {
    const int layers = 1 << (detaillevel-1);
    if(detaillevel) for(int z=0; z<layers; z++) 
      draw_qfi(c, mscale(V, zgrad0(-vid.lake_top, 0, z, layers)), col, PPR::FLOOR+z-layers);
    }
  else if(sidepar == SIDE_LTOB) {
    const int layers = 1 << (detaillevel-1);
    if(detaillevel) for(int z=0; z<layers; z++) 
      draw_qfi(c, mscale(V, zgrad0(-vid.lake_bottom, -vid.lake_top, z, layers)), col, PPR::INLAKEWALL+z-layers);
    }
  else if(sidepar == SIDE_BTOI) {
    const int layers = 1 << detaillevel;
    draw_qfi(c, mscale(V, cgi.INFDEEP), col, PPR::MINUSINF);
    for(int z=1; z<layers; z++) 
      draw_qfi(c, mscale(V, zgrad0(-vid.lake_bottom, -vid.lake_top, -z, 1)), col, PPR::LAKEBOTTOM+z-layers);
    }
  }

EX bool placeSidewall(cell *c, int i, int sidepar, const transmatrix& V, color_t col) {

  if(!qfi.fshape || !qfi.fshape->is_plain || !cgi.validsidepar[sidepar] || qfi.usershape >= 0) if(GDIM == 2) {
    escherSidewall(c, sidepar, V, col);
    return true;
    }
  if(!qfi.fshape) return true;
  
  if(qfi.fshape == &cgi.shBigTriangle && pseudohept(c->move(i))) return false;
  if(qfi.fshape == &cgi.shTriheptaFloor && !pseudohept(c) && !pseudohept(c->move(i))) return false;

  PPR prio;
  /* if(mirr) prio = PPR::GLASS - 2;
  else */ if(sidepar == SIDE_WALL) prio = PPR::WALL3 - 2;
  else if(sidepar == SIDE_WTS3) prio = PPR::WALL3 - 2;
  else if(sidepar == SIDE_LAKE) prio = PPR::LAKEWALL;
  else if(sidepar == SIDE_LTOB) prio = PPR::INLAKEWALL;
  else if(sidepar == SIDE_BTOI) prio = PPR::BELOWBOTTOM;
  else prio = PPR::REDWALL-2+4*(sidepar-SIDE_SLEV);
  
  dynamicval<bool> ncor(approx_nearcorner, true);
  transmatrix V2 = V * ddspin(c, i);
 
  if(binarytiling || archimedean || NONSTDVAR || penrose) {
    #if CAP_ARCM
    if(archimedean && !PURE)
      i = (i + arcm::parent_index_of(c->master)/DUALMUL + MODFIXER) % c->type;
    #endif
    draw_shapevec(c, V2, qfi.fshape->gpside[sidepar][i], col, prio);
    return false;
    }
  
  queuepolyat(V2, qfi.fshape->side[sidepar][shvid(c)], col, prio);
  return false;
  }
#endif

bool openorsafe(cell *c) {
  return c->wall == waMineOpen || mineMarkedSafe(c);
  }

#define Dark(x) darkena(x,0,0xFF)

EX color_t stdgridcolor = 0x202020FF;

EX int gridcolor(cell *c1, cell *c2) {
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
  if(chasmgraph(c1) != chasmgraph(c2) && c1->land != laAsteroids && c2->land != laAsteroids)
    return Dark(0x808080);
  if(c1->land == laAlchemist && c2->land == laAlchemist && c1->wall != c2->wall && !c1->item && !c2->item)
    return Dark(0xC020C0);
  if((c1->land == laWhirlpool || c2->land == laWhirlpool) && (celldistAlt(c1) != celldistAlt(c2)))
    return Dark(0x2020A0);
  if(c1->land == laMinefield && c2->land == laMinefield && (openorsafe(c1) != openorsafe(c2)))
    return Dark(0xA0A0A0);
  if(!darken) return stdgridcolor;
  return Dark(0x202020);
  }

#if CAP_SHAPES
EX void pushdown(cell *c, int& q, const transmatrix &V, double down, bool rezoom, bool repriority) {
 
  #if MAXMDIM >= 4
  if(GDIM == 3) {
    for(int i=q; i<isize(ptds); i++) {
      auto pp = dynamic_cast<dqi_poly*> (&*ptds[q++]);
      if(!pp) continue;
      auto& ptd = *pp;
      ptd.V = ptd.V * zpush(+down);
      }
    return;
    }
  #endif

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
    else if(nlev < -vid.lake_bottom-1e-3) {
      ptd.prio = PPR::BELOWBOTTOM_FALLANIM;
      if(c->wall != waChasm)
        ptd.color = 0; // disappear!
      }
    else if(nlev < -vid.lake_top-1e-3)
      ptd.prio = PPR::INLAKEWALL_FALLANIM;
    else if(nlev < 0)
      ptd.prio = PPR::LAKEWALL_FALLANIM;
    }
  }
#endif

// 1 : (floor, water); 2 : (water, bottom); 4 : (bottom, inf)

EX int shallow(cell *c) {
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

EX bool bright;

// how much to darken
EX int getfd(cell *c) {
  if(bright) return 0;
  if(among(c->land, laAlchemist, laHell, laVariant) && WDIM == 2 && GDIM == 3) return 0;
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

EX bool just_gmatrix;

EX int colorhash(color_t i) {
  return (i * 0x471211 + i*i*0x124159 + i*i*i*0x982165) & 0xFFFFFF;
  }

EX bool isWall3(cell *c, color_t& wcol) {
  if(isWall(c)) return true;
  if(c->wall == waChasm && c->land == laMemory) { wcol = 0x606000; return true; }
  if(c->wall == waInvisibleFloor) return false;
  // if(chasmgraph(c)) return true;
  if(among(c->wall, waMirror, waCloud, waMineUnknown, waMineMine)) return true;
  return false;
  }

EX bool isWall3(cell *c) { color_t dummy; return isWall3(c, dummy); }

EX bool isSulphuric(eWall w) { return among(w, waSulphur, waSulphurC); }

// 'land color', but a bit twisted for Alchemist Lab
color_t lcolor(cell *c) {
  if(isAlch(c->wall) && !c->item) return winf[c->wall].color;
  return floorcolors[c->land];
  }

EX color_t transcolor(cell *c, cell *c2, color_t wcol) {
  color_t dummy;
  if(isWall3(c2, dummy)) return 0;
  if(c->land != c2->land && c->land != laNone && c2->land != laNone) {
    if(c>c2) return 0;
    if(c->land == laBarrier) return darkena3(lcolor(c2), 0, 0x40);
    if(c2->land == laBarrier) return darkena3(lcolor(c), 0, 0x40);
    return darkena3(gradient(lcolor(c), lcolor(c2), 0, 1, 2), 0, 0x40);
    }
  if(sol && c->land == laWineyard && c2->master->distance < c->master->distance)
    return 0x00800040;
  if(isAlch(c) && !c->item && (c2->item || !isAlch(c2))) return darkena3(winf[c->wall].color, 0, 0x40);
  if(c->wall == c2->wall) return 0;
  if(isFire(c) && !isFire(c2)) return darkena3(wcol, 0, 0x30);
  if(c->wall == waLadder) return darkena3(wcol, 0, 0x30);

  if(c->wall == waChasm && c2->wall != waChasm) return 0x606060A0;
  if(isWateryOrBoat(c) && !isWateryOrBoat(c2)) return 0x0000C060;
  if(isSulphuric(c->wall) && !isSulphuric(c2->wall)) return darkena3(winf[c->wall].color, 0, 0x40);
  if(among(c->wall, waCanopy, waSolidBranch, waWeakBranch) && !among(c2->wall, waCanopy, waSolidBranch, waWeakBranch)) return 0x00C00060;
  if(c->wall == waFloorA && c2->wall == waFloorB && !c->item && !c2->item) return darkena3(0xFF00FF, 0, 0x80);
  if(realred(c->wall) || realred(c2->wall)) {
    int l = snakelevel(c) - snakelevel(c2);
    if(l > 0) return darkena3(floorcolors[laRedRock], 0, 0x30 * l);
    }
  if(among(c->wall, waRubble, waDeadfloor2) && !snakelevel(c2)) return darkena3(winf[c->wall].color, 0, 0x40);
  if(c->wall == waMagma && c2->wall != waMagma) return darkena3(magma_color(lavatide(c, -1)/4), 0, 0x80);
  return 0;
  }

// how much should be the d-th wall darkened in 3D
EX int get_darkval(cell *c, int d) {
  if(hybri) {
    return d >= c->type - 2 ? 4 : 0;
    }
  const int darkval_hbt[9] = {0,2,2,0,6,6,8,8,0};
  const int darkval_s12[12] = {0,1,2,3,4,5,0,1,2,3,4,5};
  const int darkval_e6[6] = {0,4,6,0,4,6};
  const int darkval_e12[12] = {0,4,6,0,4,6,0,4,6,0,4,6};
  const int darkval_e14[14] = {0,0,0,4,6,4,6,0,0,0,6,4,6,4};
  const int darkval_hh[14] = {0,0,0,1,1,1,2,2,2,3,3,3,1,0};
  const int darkval_hrec[7] = {0,0,2,4,2,4,0};
  const int darkval_sol[8] = {0,2,4,4,0,2,4,4};
  const int darkval_arnold[12] = {0,2,0,2,4,5,0,2,0,2,4,5};
  const int darkval_penrose[12] = {0, 2, 0, 2, 4, 4, 6, 6, 6, 6, 6, 6};
  const int darkval_nil[8] = {6,6,0,3,6,6,0,3};
  const int darkval_nih[11] = {0,2,0,2,4,6,6,6,6,6,6};
  if(sphere) return darkval_s12[d];
  if(euclid && S7 == 6) return darkval_e6[d];
  if(euclid && S7 == 12) return darkval_e12[d];
  if(euclid && S7 == 14) return darkval_e14[d];
  if(geometry == gHoroHex) return darkval_hh[d];
  if(geometry == gHoroRec) return darkval_hrec[d];
  if(penrose) return darkval_penrose[d];
  if(geometry == gArnoldCat) return darkval_arnold[d];
  if(sol) return darkval_sol[d];
  if(nih) return darkval_nih[d];
  if(binarytiling) return darkval_hbt[d];
  if(hyperbolic && S7 == 6) return darkval_e6[d];
  if(hyperbolic && S7 == 12) return darkval_s12[d];
  if(nil) return darkval_nil[d];
  return 0;
  }

EX ld mousedist(transmatrix T) {
  if(GDIM == 2) return intval(mouseh, tC0(T));
  hyperpoint T1 = tC0(mscale(T, cgi.FLOOR));
  if(mouseaim_sensitivity) return sqhypot_d(2, T1) + (point_behind(T1) ? 1e10 : 0);
  hyperpoint h1;
  applymodel(T1, h1);
  h1 = h1 - hpxy((mousex - current_display->xcenter) / current_display->radius, (mousey - current_display->ycenter) / current_display->radius);
  return sqhypot_d(2, h1) + (point_behind(T1) ? 1e10 : 0);
  }

vector<hyperpoint> clipping_planes;
EX int noclipped;

void make_clipping_planes() {
#if MAXMDIM >= 4
  clipping_planes.clear();
  if(PIU(sphere) || experimental || vid.stereo_mode == sODS) return;
  auto add_clipping_plane = [] (ld x1, ld y1, ld x2, ld y2) {
    ld z1 = 1, z2 = 1;
    hyperpoint sx = point3(y1 * z2 - y2 * z1, z1 * x2 - z2 * x1, x1 * y2 - x2 * y1);
    sx /= hypot_d(3, sx);
    sx[3] = 0;
    if(nisot::local_perspective_used()) sx = inverse(NLP) * sx;
    clipping_planes.push_back(sx);
    };
  ld tx = current_display->tanfov;
  ld ty = tx * current_display->ysize / current_display->xsize;
  add_clipping_plane(+tx, +ty, -tx, +ty);
  add_clipping_plane(-tx, +ty, -tx, -ty);
  add_clipping_plane(-tx, -ty, +tx, -ty);
  add_clipping_plane(+tx, -ty, +tx, +ty);
#endif
  }

bool celldrawer::cell_clipped() {
  if(WDIM == 3 && pmodel == mdPerspective && !nonisotropic && !in_s2xe()) {
    hyperpoint H = tC0(V);
    if(prod) H = product::inverse_exp(H);
    for(hyperpoint& cpoint: clipping_planes) if((H|cpoint) < -sin_auto(cgi.corner_bonus)) {
      drawcell_in_radar();
      return true;
      }
    noclipped++;
    }
  if(pmodel == mdGeodesic && solnih) {
    hyperpoint H = tC0(V);
    if(abs(H[0]) <= 3 && abs(H[1]) <= 3 && abs(H[2]) <= 3 ) ;
    else {
      hyperpoint H2 = inverse_exp(H, iLazy);
      for(hyperpoint& cpoint: clipping_planes) if((H2|cpoint) < -.4) return true;
      }
    noclipped++;
    }
  if(pmodel == mdGeodesic && nil) {
    hyperpoint H = tC0(V);
    if(abs(H[0]) <= 3 && abs(H[1]) <= 3 && abs(H[2]) <= 3 ) ;
    else {
      hyperpoint H2 = inverse_exp(H, iLazy);
      for(hyperpoint& cpoint: clipping_planes) if((H2|cpoint) < -2) return true;
      }
    noclipped++;
    }
  return false;
  }

EX void gridline(const transmatrix& V1, const hyperpoint h1, const transmatrix& V2, const hyperpoint h2, color_t col, int prec) {
#if MAXMDIM >= 4
  if(WDIM == 2 && GDIM == 3) {
    ld eps = cgi.human_height/100;
    queueline(V1*orthogonal_move(h1,cgi.FLOOR+eps), V2*orthogonal_move(h2,cgi.FLOOR+eps), col, prec);
    queueline(V1*orthogonal_move(h1,cgi.WALL-eps), V2*orthogonal_move(h2,cgi.WALL-eps), col, prec);
    }
  else
#endif
    queueline(V1*h1, V2*h2, col, prec);
  }

EX void gridline(const transmatrix& V, const hyperpoint h1, const hyperpoint h2, color_t col, int prec) {
  gridline(V, h1, V, h2, col, prec);
  }

EX int wall_offset(cell *c) {
  if(prod) return product::cwall_offset;
  if(hybri) return hybrid::wall_offset(c);
  if(penrose && kite::getshape(c->master) == kite::pKite) return 10;
  return 0;
  }

EX void queue_transparent_wall(const transmatrix& V, hpcshape& sh, color_t color) {
  auto& poly = queuepolyat(V, sh, color, PPR::TRANSPARENT_WALL);
  hyperpoint h = V * sh.intester;
  if(in_perspective())
    poly.subprio = int(hdist0(h) * 100000);
  else {
    hyperpoint h2;
    applymodel(h, h2);
    poly.subprio = int(h2[2] * 100000);
    }
  }

#if MAXMDIM >= 4
EX int ceiling_category(cell *c) {
  switch(c->land) {
    case laNone:
    case laMemory:
    case laMirrorWall2:
    case laMirrored:
    case laMirrored2:
    case landtypes:
      return 0;

    /* starry levels */
    case laIce:
    case laCrossroads:
    case laCrossroads2:
    case laCrossroads3:
    case laCrossroads4:
    case laCrossroads5:
    case laJungle:    
    case laGraveyard:
    case laMotion:
    case laRedRock:
    case laZebra:
    case laHunting:
    case laEAir:
    case laStorms:
    case laMountain:
    case laHaunted:
    case laHauntedWall:
    case laHauntedBorder:
    case laWhirlwind:
    case laBurial:
    case laHalloween:
    case laReptile:
    case laVolcano:
    case laBlizzard:
    case laDual:
    case laWestWall:
    case laAsteroids:
      return 1;
    
    case laPower:
    case laWineyard:
    case laDesert:
    case laAlchemist:
    case laDryForest:
    case laCaribbean:
    case laMinefield:
    case laOcean:
    case laWhirlpool:
    case laLivefjord:
    case laEWater:
    case laOceanWall:
    case laWildWest:
    case laOvergrown:
    case laClearing:
    case laRose:
    case laWarpCoast:
    case laWarpSea:
    case laEndorian:
    case laTortoise:
    case laPrairie:
    case laDragon:
    case laSnakeNest:
    case laDocks:
    case laKraken:
    case laBrownian: 
    case laHell:
    case laVariant:    
      return 2;
    
    case laBarrier: 
    case laCaves:
    case laMirror:
    case laMirrorOld:
    case laCocytus:
    case laEmerald:
    case laDeadCaves:
    case laHive:
    case laCamelot:
    case laIvoryTower:
    case laEFire:
    case laEEarth:
    case laElementalWall:
    case laCanvas:
    case laTrollheim:
    case laDungeon:
    case laBull:
    case laCA:
    case laMirrorWall:
    case laTerracotta:
    case laMercuryRiver:
    case laMagnetic:
    case laSwitch:
      return 3;
    
    case laPalace:
    case laPrincessQuest:
    default:
      return 4;
    
    case laRuins:
      return 6;
    
    case laTemple:
    case laRlyeh:
      return 7;
    }
  }

#endif

EX void set_detail_level(const transmatrix& V) {
  ld dist0 = hdist0(tC0(V)) - 1e-6;
  if(vid.use_smart_range) detaillevel = 2;
  else if(dist0 < vid.highdetail) detaillevel = 2;
  else if(dist0 < vid.middetail) detaillevel = 1;
  else detaillevel = 0;

  if((cmode & sm::NUMBER) && (dialog::editingDetail())) {
    color_t col = 
      dist0 < vid.highdetail ? 0xFF80FF80 :
      dist0 >= vid.middetail ? 0xFFFF8080 :
      0XFFFFFF80;
    queuepoly(V, cgi.shHeptaMarker, darkena(col & 0xFFFFFF, 0, 0xFF));
    }
  }

struct flashdata {
  int t;
  int size;
  cell *where;
  double angle;
  double angle2;
  int spd; // 0 for flashes, >0 for particles
  color_t color;
  flashdata(int _t, int _s, cell *_w, color_t col, int sped) { 
    t=_t; size=_s; where=_w; color = col; 
    angle = rand() % 1000; spd = sped;
    if(GDIM == 3) angle2 = acos((rand() % 1000 - 499.5) / 500);
    }
  };

vector<flashdata> flashes;

EX void drawFlash(cell *c) {
  flashes.push_back(flashdata(ticks, 1000, c, iinf[itOrbFlash].color, 0)); 
  }
EX void drawBigFlash(cell *c) { 
  flashes.push_back(flashdata(ticks, 2000, c, 0xC0FF00, 0)); 
  }

EX void drawParticleSpeed(cell *c, color_t col, int speed) {
  if(vid.particles && !confusingGeometry())
    flashes.push_back(flashdata(ticks, rand() % 16, c, col, speed)); 
  }
EX void drawParticle(cell *c, color_t col, int maxspeed IS(100)) {
  drawParticleSpeed(c, col, 1 + rand() % maxspeed);
  }
EX void drawParticles(cell *c, color_t col, int qty, int maxspeed IS(100)) { 
  if(vid.particles)
    while(qty--) drawParticle(c,col, maxspeed); 
  }
EX void drawFireParticles(cell *c, int qty, int maxspeed IS(100)) { 
  if(vid.particles)
    for(int i=0; i<qty; i++)
      drawParticle(c, firegradient(i / (qty-1.)), maxspeed); 
  }
EX void fallingFloorAnimation(cell *c, eWall w IS(waNone), eMonster m IS(moNone)) {
  if(!wmspatial) return;
  fallanim& fa = fallanims[c];
  fa.t_floor = ticks;
  fa.walltype = w; fa.m = m;
  // drawParticles(c, darkenedby(linf[c->land].color, 1), 4, 50);
  }
EX void fallingMonsterAnimation(cell *c, eMonster m, int id IS(multi::cpid)) {
  if(!mmspatial) return;
  fallanim& fa = fallanims[c];
  fa.t_mon = ticks;
  fa.m = m;
  fa.pid = id;
  // drawParticles(c, darkenedby(linf[c->land].color, 1), 4, 50);
  }

void celldrawer::draw_fallanims() {
  cell*& c = cw.at;
  poly_outline = OUTLINE_NONE;
  if(fallanims.count(c)) {
     int q = isize(ptds);
     int maxtime = euclid || sphere ? 20000 : 1500;
     fallanim& fa = fallanims[c];
     bool erase = true;
     if(fa.t_floor) {
       int t = (ticks - fa.t_floor);
       if(t <= maxtime) {
         erase = false;
         if(GDIM == 3)
           draw_shapevec(c, V, qfi.fshape->levels[0], darkena(fcol, fd, 0xFF), PPR::WALL);
         else if(fa.walltype == waNone) {
           draw_qfi(c, V, darkena(fcol, fd, 0xFF), PPR::FLOOR);
           }
         else {
           celldrawer ddalt;
           eWall w = c->wall; int p = c->wparam;
           c->wall = fa.walltype; c->wparam = fa.m;
           ddalt.cw.at = c;
           ddalt.setcolors();
           int starcol = c->wall == waVinePlant ? 0x60C000 : ddalt.wcol;
           c->wall = w; c->wparam = p;
           draw_qfi(c, mscale(V, cgi.WALL), darkena(starcol, fd, 0xFF), PPR::WALL3);
           queuepolyat(mscale(V, cgi.WALL), cgi.shWall[ct6], darkena(ddalt.wcol, 0, 0xFF), PPR::WALL3A);
           forCellIdEx(c2, i, c)
             if(placeSidewall(c, i, SIDE_WALL, V, darkena(ddalt.wcol, 1, 0xFF))) break;
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
         drawMonsterType(fa.m, c, V2, minf[fa.m].color, footphase, NOCOLOR);
         pushdown(c, q, V2, t*t / 1000000. + t / 1000., true, true);
         }
       }
     if(erase) fallanims.erase(c);
     }
   }

#if CAP_QUEUE
EX void queuecircleat(cell *c, double rad, color_t col) {
  if(!c) return;
  if(!gmatrix.count(c)) return;
  if(WDIM == 3) {
    dynamicval<color_t> p(poly_outline, col);
    // we must do hybrid::wall_offset in hybrid because the cached value is likely incorrect
    int ofs = hybri ? hybrid::wall_offset(c) : wall_offset(c);
    for(int i=0; i<c->type; i++) {
      queuepolyat(gmatrix[c], cgi.shWireframe3D[ofs + i], 0, PPR::SUPERLINE);
      }
    return;
    }    
  if(spatial_graphics || GDIM == 3) {
    vector<transmatrix> corners(c->type+1);
    for(int i=0; i<c->type; i++) corners[i] = gmatrix[c] * rgpushxto0(get_corner_position(c, i, 3 / rad));    
    corners[c->type] = corners[0];
    for(int i=0; i<c->type; i++) {
      queueline(mscale(corners[i], cgi.FLOOR) * C0, mscale(corners[i+1], cgi.FLOOR) * C0, col, 2, PPR::SUPERLINE);
      queueline(mscale(corners[i], cgi.WALL) * C0, mscale(corners[i+1], cgi.WALL) * C0, col, 2, PPR::SUPERLINE);
      queueline(mscale(corners[i], cgi.FLOOR) * C0, mscale(corners[i], cgi.WALL) * C0, col, 2, PPR::SUPERLINE);
      }
    return;
    }
  #if CAP_SHAPES
  if(vid.stereo_mode || sphere) {
    dynamicval<color_t> p(poly_outline, col);
    queuepolyat(gmatrix[c] * spintick(100), cgi.shGem[1], 0, PPR::LINE);
    return;
    }
  #endif
  queuecircle(gmatrix[c], rad, col);  
  if(!wmspatial) return;
  if(highwall(c))
    queuecircle(mscale(gmatrix[c], cgi.WALL), rad, col);
  int sl;
  if((sl = snakelevel(c))) {
    queuecircle(mscale(gmatrix[c], cgi.SLEV[sl]), rad, col);
    }
  if(chasmgraph(c))
    queuecircle(mscale(gmatrix[c], cgi.LAKE), rad, col);
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

EX cell *forwardcell() {
  movedir md = vectodir(move_destination_vec(6));
  cellwalker xc = cwt + md.d + wstep;
  return xc.at;
  }

EX void drawMarkers() {

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
          int cd = celldistance(yi[yii].key(), cwt.at);
          if(cd == DISTANCE_UNKNOWN) for(int i2 = 0; i2<YDIST; i2++) {
            int cd2 = celldistance(cwt.at, yi[yii].path[i2]);
            if(cd2 != DISTANCE_UNKNOWN) {
              cd = cd2 + (YDIST-1-i2);
              println(hlog, "i2 = ", i2, " cd = ", celldistance(cwt.at, keycell));
              }
            }
          queuestr(H, vid.fsize, its(cd), 0x10101 * int(128 - 100 * sintick(150)));
          #endif
          addauraspecial(H, iinf[itOrbYendor].color, 0);
          }
        }
      }
  
    #if CAP_RACING
    racing::markers();
    #endif
  
    #if CAP_QUEUE        
    if(lmouseover && vid.drawmousecircle && ok && DEFAULTCONTROL && MOBON && WDIM == 2) {
      queuecircleat(lmouseover, .8, darkena(lmouseover->cpdist > 1 ? 0x00FFFF : 0xFF0000, 0, 0xFF));
      }

    if(global_pushto && vid.drawmousecircle && ok && DEFAULTCONTROL && MOBON && WDIM == 2) {
      queuecircleat(global_pushto, .6, darkena(0xFFD500, 0, 0xFF));
      }
    #endif

#if CAP_SDLJOY && CAP_QUEUE
    if(joydir.d >= 0 && WDIM == 2) 
      queuecircleat(cwt.at->modmove(joydir.d+cwt.spin), .78 - .02 * sintick(199), 
        darkena(0x00FF00, 0, 0xFF));
#endif

    bool m = true;
    ignore(m);
#if CAP_MODEL
    m = netgen::mode == 0;
#endif

    #if CAP_QUEUE
    if(centerover && !playermoved && m && !anims::any_animation() && WDIM == 2)
      queuecircleat(centerover, .70 - .06 * sintick(200), 
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
    if((vid.axes == 4 || (vid.axes == 1 && !mousing)) && !shmup::on && GDIM == 2) {
      if(multi::players == 1) {
        forCellIdAll(c2, d, cwt.at) IG(c2) draw_movement_arrows(c2, Gm(cwt.at) * currentmap->adj(cwt.at, d));
        }
      else if(multi::players > 1) for(int p=0; p<multi::players; p++) {
        if(multi::playerActive(p) && (vid.axes == 4 || !drawstaratvec(multi::mdx[p], multi::mdy[p]))) 
        forCellIdAll(c2, d, multi::player[p].at) IG(c2) {
          multi::cpid = p;
          dynamicval<transmatrix> ttm(cwtV, multi::whereis[p]);
          dynamicval<cellwalker> tcw(cwt, multi::player[p]);
          draw_movement_arrows(c2, Gm(cwt.at) * currentmap->adj(cwt.at, d));
          }
        }
      }
    
    if(GDIM == 3 && !inHighQual && !shmup::on && vid.axes3 && playermoved) {
      cell *c = forwardcell();
      IG(c) queuecircleat(c, .8, getcs().uicolor);
      }
    
    #endif

    if(hybri && !shmup::on) {

      using namespace sword;
      int& ang = sword::dir[multi::cpid].angle;
      ang %= sword_angles;

      int adj = 1 - ((sword_angles/cwt.at->type)&1);
      
      if(items[itOrbSword])
        queuechr(gmatrix[cwt.at] * spin(M_PI+(-adj-2*ang)*M_PI/sword_angles) * xpush0(cgi.sword_size), vid.fsize*2, '+', iinf[itOrbSword].color);
      if(items[itOrbSword2])
        queuechr(gmatrix[cwt.at] * spin((-adj-2*ang)*M_PI/sword_angles) * xpush0(-cgi.sword_size), vid.fsize*2, '+', iinf[itOrbSword2].color);
      }
    if(SWORDDIM == 3 && !shmup::on) {
      if(items[itOrbSword])
        queuechr(gmatrix[cwt.at] * sword::dir[multi::cpid].T * xpush0(cgi.sword_size), vid.fsize*2, '+', iinf[itOrbSword].color);
      if(items[itOrbSword2])
        queuechr(gmatrix[cwt.at] * sword::dir[multi::cpid].T * xpush0(-cgi.sword_size), vid.fsize*2, '+', iinf[itOrbSword2].color);
      }
    }

  monsterToSummon = moNone;
  orbToTarget = itNone;

  if(mouseover && targetclick) {
    multi::cpid = 0;
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
        queuepoly(T, cgi.shDisk, aircol);
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
      int partcol = darkena(f.color, 0, GDIM == 3 ? 255 : max(255 - tim*255/300, 0));
      poly_outline = OUTLINE_DEFAULT;
      ld t = f.spd * tim * cgi.scalefactor / 50000.;
      transmatrix T =
        GDIM == 2 ? V * spin(f.angle) * xpush(t) :
        V * cspin(0, 1, f.angle) * cspin(0, 2, f.angle2) * cpush(2, t);
      queuepoly(T, cgi.shParticle[f.size], partcol);
      #endif
      }
    
    else if(f.size == 1000) {
      for(int u=0; u<=tim; u++) {
        if((u-tim)%50) continue;
        if(u < tim-150) continue;
        ld rad = u * 3 / 1000.;
        rad = rad * (5-rad) / 2;
        rad *= cgi.hexf;
        int flashcol = f.color;
        if(u > 500) flashcol = gradient(flashcol, 0, 500, u, 1100);
        flashcol = darkena(flashcol, 0, 0xFF);
#if MAXMDIM >= 4
        if(GDIM == 3)
          queueball(V * zpush(cgi.GROIN1), rad, flashcol, itDiamond);
        else 
#endif
        {
          PRING(a) curvepoint(V*xspinpush0(a * M_PI / cgi.S42, rad));
          queuecurve(flashcol, 0x8080808, PPR::LINE);
          }
        }
      }
    else if(f.size == 2000) {
      for(int u=0; u<=tim; u++) {
        if((u-tim)%50) continue;
        if(u < tim-250) continue;
        ld rad = u * 3 / 2000.;
        rad = rad * (5-rad) * 1.25;
        rad *= cgi.hexf;
        int flashcol = f.color;
        if(u > 1000) flashcol = gradient(flashcol, 0, 1000, u, 2200);
        flashcol = darkena(flashcol, 0, 0xFF);
#if MAXMDIM >= 4
        if(GDIM == 3)
          queueball(V * zpush(cgi.GROIN1), rad, flashcol, itRuby);
        else 
#endif
        {
          PRING(a) curvepoint(V*xspinpush0(a * M_PI / cgi.S42, rad));
          queuecurve(flashcol, 0x8080808, PPR::LINE);
          }
        }
      }

    if(kill) {
      f = flashes[isize(flashes)-1];
      flashes.pop_back(); k--;
      }
    }
  #endif
  }

EX bool allowIncreasedSight() {
  if(cheater || autocheat) return true;
  if(peace::on) return true;
#if CAP_TOUR
  if(tour::on) return true;
#endif
  if(randomPatternsMode) return true;
  if(racing::on) return true;
  if(quotient || !hyperbolic || archimedean) return true;
  if(WDIM == 3) return true;
  return false;
  }

EX bool allowChangeRange() {
  if(cheater || peace::on || randomPatternsMode) return true;
#if CAP_TOUR
  if(tour::on) return true;
#endif
  if(racing::on) return true;
  if(sightrange_bonus >= 0) return true;
  if(archimedean) return true;
  if(WDIM == 3) return true;
  return false;
  }

EX purehookset hooks_drawmap;

EX transmatrix actual_view_transform;

EX ld wall_radar(cell *c, transmatrix T, transmatrix LPe, ld max) {
  if(!in_perspective() || !vid.use_wall_radar) return max;
  transmatrix ori;
  if(prod) ori = inverse(LPe);
  ld step = max / 20;
  ld fixed_yshift = 0;
  for(int i=0; i<20; i++) {
    T = parallel_transport(T, ori, ztangent(-step));
    virtualRebase(c, T);
    color_t col;
    if(isWall3(c, col) || (WDIM == 2 && GDIM == 3 && tC0(T)[2] > cgi.FLOOR)) { 
      T = parallel_transport(T, ori, ztangent(step));
      step /= 2; i = 17; 
      if(step < 1e-3) break; 
      }
    else fixed_yshift += step;
    }
  return fixed_yshift;
  }

EX void make_actual_view() {
  sphereflip = Id;
  if(sphereflipped()) sphereflip[LDIM][LDIM] = -1;
  actual_view_transform = sphereflip;  
  if(vid.yshift && WDIM == 2) actual_view_transform = ypush(vid.yshift) * actual_view_transform;
  #if MAXMDIM >= 4
  if(GDIM == 3) {
    ld max = WDIM == 2 ? vid.camera : vid.yshift;
    if(max) {
      transmatrix Start = inverse(actual_view_transform * View);
      ld d = wall_radar(centerover, Start, NLP, max);
      actual_view_transform = get_shift_view_of(ztangent(d), actual_view_transform * View) * inverse(View); 
      }
    camera_level = asin_auto(tC0(inverse(actual_view_transform * View))[2]);
    }
  if(nonisotropic) {
    transmatrix T = actual_view_transform * View;
    transmatrix T2 = eupush( tC0(inverse(T)) );
    NLP = T * T2;
    actual_view_transform = inverse(NLP) * actual_view_transform;
    }
  #endif
  #if MAXMDIM >= 4
  if(GDIM == 3 && WDIM == 2) {
    transmatrix T = actual_view_transform * View;
    transmatrix U = inverse(T);
    
    if(T[0][2]) 
      T = spin(-atan2(T[0][2], T[1][2])) * T;
    if(T[1][2] && T[2][2]) 
      T = cspin(1, 2, -atan2(T[1][2], T[2][2])) * T;

    ld z = -asin_auto(tC0(inverse(T)) [2]);
    T = zpush(-z) * T;

    radar_transform = T * U;
    }
  #endif
  Viewbase = View;
  }

EX transmatrix cview() {
  return actual_view_transform * View;
  }

EX void precise_mouseover() {
  if(WDIM == 3) { 
    mouseover2 = mouseover = centerover;
    ld best = HUGE_VAL;
    hyperpoint h = direct_exp(lp_iapply(ztangent(0.01)), 100);

    if(hybri) {
      while(true) {
        ld d1 = geo_dist(C0, tC0(ggmatrix(mouseover2)), iTable);
        cell *nxt = NULL;
        forCellEx(c1, mouseover2) {
          ld d2 = geo_dist(C0, tC0(ggmatrix(c1)), iTable);
          if(d2 < d1) d1 = d2, nxt = c1;
          }
        if(!nxt) break;
        mouseover2 = nxt;
        }
      }

    forCellEx(c1, mouseover2) {
      hyperpoint h1 = tC0(ggmatrix(c1));
      ld dist = geo_dist(h1, h, iTable) - geo_dist(C0, h1, iTable);
      if(dist < best) mouseover = c1, best = dist;
      }
    return; 
    }
  if(!mouseover) return;
  if(GDIM == 3) return;
  cell *omouseover = mouseover;
  for(int loop = 0; loop < 10; loop++) { 
    bool found = false;
    if(!gmatrix.count(mouseover)) return;
    hyperpoint r_mouseh = inverse(gmatrix[mouseover]) * mouseh;
    for(int i=0; i<mouseover->type; i++) {
      hyperpoint h1 = get_corner_position(mouseover, (i+mouseover->type-1) % mouseover->type);
      hyperpoint h2 = get_corner_position(mouseover, i);
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

EX transmatrix Viewbase;

EX bool no_wall_rendering;

EX void drawthemap() {
  check_cgi();
  cgi.require_shapes();

  DEBBI(DF_GRAPH, ("draw the map"));
  
  last_firelimit = firelimit;
  firelimit = 0;

  if(GDIM == 3) make_clipping_planes();
  radarpoints.clear();
  radarlines.clear();
  callhooks(hooks_drawmap);

  frameid++;
  cells_drawn = 0;
  cells_generated = 0;
  noclipped = 0;
  first_cell_to_draw = true;
  
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
  spatial_graphics = spatial_graphics && GDIM == 2;
  #if CAP_RUG
  if(rug::rugged && !rug::spatial_rug) spatial_graphics = false;
  #endif
  if(non_spatial_model())
    spatial_graphics = false;
  if(pmodel == mdDisk && abs(vid.alpha) < 1e-6) spatial_graphics = false;
  
  if(!spatial_graphics) wmspatial = mmspatial = false;
  if(GDIM == 3) wmspatial = mmspatial = true;

  for(int m=0; m<motypes; m++) if(isPrincess(eMonster(m))) 
    minf[m].name = princessgender() ? "Princess" : "Prince";
  
  #if MAXMDIM >= 4
  ray::in_use = ray::requested();
  #endif
  no_wall_rendering = ray::in_use;
  // ray::comparison_mode = true;
  if(ray::comparison_mode) no_wall_rendering = false;
    
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

  for(int i=0; i<multi::players; i++) {
    multi::ccdist[i] = 1e20; multi::ccat[i] = NULL;
    }

  straightDownSeek = NULL;

  #if ISMOBILE
  mouseovers = XLAT("No info about this...");
  #endif
  if(mouseout() && !mousepan) 
    modist = -5;
  playerfound = false;
  // playerfoundL = false;
  // playerfoundR = false;
  
  arrowtraps.clear();

  profile_start(1);
  make_actual_view();
  currentmap->draw();
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
    if(multi::centerplayer != -1) 
      cwtV = multi::whereis[multi::centerplayer];
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
    if(multi::players == 1)
      cwtV = shmup::pc[0]->pat;
    else if(multi::centerplayer != -1) 
      cwtV = shmup::pc[multi::centerplayer]->pat;
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

EX void drawmovestar(double dx, double dy) {

  DEBBI(DF_GRAPH, ("draw movestar"));
  if(viewdists) return;
  if(GDIM == 3) return;

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
    queuepoly(Centered, cgi.shMovestar, starcol);
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

EX bool dronemode;

purehookset hooks_calcparam;

EX int corner_centering;

EX bool permaside;

EX void calcparam() {

  DEBBI(DF_GRAPH, ("calc param"));
  auto cd = current_display;
  
  cd->xtop = vid.xres * cd->xmin;
  cd->ytop = vid.yres * cd->ymin;
  
  cd->xsize = vid.xres * (cd->xmax - cd->xmin);
  cd->ysize = vid.yres * (cd->ymax - cd->ymin);

  cd->xcenter = cd->xtop + cd->xsize / 2;
  cd->ycenter = cd->ytop + cd->ysize / 2;

  if(vid.scale > -1e-2 && vid.scale < 1e-2) vid.scale = 1;
  
  ld realradius = min(cd->xsize / 2, cd->ysize / 2);
  
  cd->scrsize = realradius - (inHighQual ? 0 : ISANDROID ? 2 : ISIOS ? 40 : 40);

  current_display->sidescreen = permaside;
  
  if(vid.xres < vid.yres - 2 * vid.fsize && !inHighQual && !in_perspective()) {
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
  if(GDIM == 3 && in_perspective()) cd->radius = cd->scrsize;
  realradius = min(realradius, cd->radius);
  
  ld aradius = sphere ? cd->radius / (vid.alpha - 1) : cd->radius;
  
  if(dronemode) { cd->ycenter -= cd->radius; cd->ycenter += vid.fsize/2; cd->ycenter += vid.fsize/2; cd->radius *= 2; }
  
  if(corner_centering) {
    cd->ycenter = cd->ytop + cd->ysize - vid.fsize - aradius;
    if(corner_centering == 1)
      cd->xcenter = cd->xtop + vid.fsize + aradius;
    if(corner_centering == 2)
      cd->xcenter = cd->xtop + cd->xsize - vid.fsize - aradius;
    }

  cd->xcenter += cd->scrsize * vid.xposition;
  cd->ycenter += cd->scrsize * vid.yposition;
  
  cd->tanfov = tan(vid.fov * degree / 2);
  
  callhooks(hooks_calcparam);
  reset_projection();
  }

function<void()> wrap_drawfullmap = drawfullmap;

bool force_sphere_outline = false;

EX void drawfullmap() {

  DEBBI(DF_GRAPH, ("draw full map"));
    
  check_cgi();
  cgi.require_shapes();

  ptds.clear();

  
  /*
  if(models::on) {
    char ch = 'A';
    for(auto& v: history::v) {
      queuepoly(ggmatrix(v->base) * v->at, cgi.shTriangle, 0x306090C0);
      queuechr(ggmatrix(v->base) * v->at * C0, 10, ch++, 0xFF0000);
      }      
    }
  */
  
  #if CAP_QUEUE
  draw_boundary(0);
  draw_boundary(1);
  
  draw_model_elements();
  #if MAXMDIM >= 4
  prepare_sky();
  #endif
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

#if ISMOBILE
extern bool wclick;
#endif

EX void gamescreen(int _darken) {

  if(subscreens::split([=] () {
    calcparam();
    compute_graphical_distance();
    gamescreen(_darken);
    })) {
    if(racing::on) return;
    // create the gmatrix
    View = subscreens::player_displays[0].view_matrix;
    centerover = subscreens::player_displays[0].precise_center;
    just_gmatrix = true;
    currentmap->draw();
    just_gmatrix = false;
    return;
    }
  
  if(dual::split([=] () { 
    dual::in_subscreen([=] () { gamescreen(_darken); });
    })) {
    calcparam(); 
    return; 
    }
  
  if((cmode & sm::MAYDARK) && !current_display->sidescreen) {
    _darken += 2;
    }

  darken = _darken;
  
  if(history::includeHistory) history::restore();

  anims::apply();
#if CAP_RUG
  if(rug::rugged) {
    rug::actDraw();
    } else
#endif
  wrap_drawfullmap();
  anims::rollback();

  if(history::includeHistory) history::restoreBack();

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
      if(!haveMobileCompass()) displayabutton(-1, +1, XLAT(andmode == 0 && useRangedOrb ? "FIRE" : andmode == 0 && WDIM == 3 && wclick ? "WAIT" : "MOVE"),  andmode == 0 ? BTON : BTOFF);
      displayabutton(+1, +1, rug::rugged ? "RUG" : XLAT(andmode == 1 ? "BACK" : GDIM == 3 ? "CAM" : "DRAG"),  andmode == 1 ? BTON : BTOFF);
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

EX bool nohelp;

EX void normalscreen() {
  help = "@";

  mouseovers = XLAT("Press F1 or right click for help");

#if CAP_TOUR  
  if(tour::on) mouseovers = tour::tourhelp;
#endif

  if(GDIM == 3 || !outofmap(mouseh)) getcstat = '-';
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

EX vector< function<void()> > screens = { normalscreen };

#if HDR
template<class T> void pushScreen(const T& x) { screens.push_back(x); } 
inline void popScreen() { if(isize(screens)>1) screens.pop_back(); }
inline void popScreenAll() { while(isize(screens)>1) popScreen(); }
typedef void (*cfunction)();
#endif

EX cfunction current_screen_cfunction() {
  auto tgt = screens.back().target<cfunction>();
  if(!tgt) return nullptr;
  return *tgt;
  }

#if HDR
namespace sm {
  static const int NORMAL = 1;
  static const int MISSION = 2;
  static const int HELP = 4;
  static const int MAP = 8;
  static const int DRAW = 16;
  static const int NUMBER = 32;
  static const int SHMUPCONFIG = 64;
  static const int OVERVIEW = 128;
  static const int SIDE = 256;
  static const int DOTOUR = 512;
  static const int CENTER = 1024;
  static const int ZOOMABLE = 4096;
  static const int TORUSCONFIG = 8192;
  static const int MAYDARK = 16384;
  static const int DIALOG_STRICT_X = 32768; // do not interpret dialog clicks outside of the X region
  static const int EXPANSION = (1<<16);
  static const int HEXEDIT = (1<<17);
  };
#endif

EX int cmode;

EX void drawscreen() {

  DEBBI(DF_GRAPH, ("drawscreen"));

  if(vid.xres == 0 || vid.yres == 0) return;

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
  int mines[MAXPLAYER], tmines=0;
  for(int p=0; p<numplayers(); p++) {
    mines[p] = 0;
    cell *c = playerpos(p);
    if(!c) continue;
    for(cell *c2: adj_minefield_cells(c)) {
      if(c2->land == laMinefield) 
        minefieldNearby = true;
      if(c2->wall == waMineMine) {
        bool ep = false;
        if(!ep) mines[p]++, tmines++;
        }
      }
    }

  if((minefieldNearby || tmines) && !items[itOrbAether] && !last_gravity_state && darken == 0 && normal) {
    string s;
    if(tmines > 9) tmines = 9;
    color_t col = minecolors[tmines%10];
    
    if(tmines == 7) seenSevenMines = true;
    
    for(int p=0; p<numplayers(); p++) if(multi::playerActive(p))
      displayfr(vid.xres * (p+.5) / numplayers(),
        current_display->ycenter - current_display->radius * 3/4, 2,
        vid.fsize, 
        mines[p] > 7 ? its(mines[p]) : XLAT(minetexts[mines[p]]), minecolors[mines[p]%10], 8);

    if(minefieldNearby && !shmup::on && cwt.at->land != laMinefield && cwt.peek()->land != laMinefield) {
      displayfr(vid.xres/2, current_display->ycenter - current_display->radius * 3/4 - vid.fsize*3/2, 2,
        vid.fsize, 
        XLAT("WARNING: you are entering a minefield!"), 
        col, 8);
      }
    }

  // SDL_UnlockSurface(s);

  glflush();
  DEBB(DF_GRAPH, ("swapbuffers"));
#if CAP_SDL
#if CAP_GL
  if(vid.usingGL) SDL_GL_SwapBuffers(); else
#endif
  SDL_UpdateRect(s, 0, 0, vid.xres, vid.yres);
#endif
  
//printf("\ec");
  }

EX void restartGraph() {
  DEBBI(DF_INIT, ("restartGraph"));
  
  if(!autocheat) linepatterns::clearAll();
  if(currentmap) {
    resetview();
    if(sphere) View = spin(-M_PI/2);
    }
  }

EX void clearAnimations() {
  for(int i=0; i<ANIMLAYERS; i++) animations[i].clear();
  flashes.clear();
  fallanims.clear();
  }
  
auto graphcm = addHook(clearmemory, 0, [] () {
  DEBBI(DF_MEMORY, ("clear graph memory"));
  mouseover = centerover = lmouseover = NULL;  
  gmatrix.clear(); gmatrix0.clear();
  clearAnimations();
  })
+ addHook(hooks_gamedata, 0, [] (gamedata* gd) {
  gd->store(mouseover);
  gd->store(lmouseover);
  gd->store(animations);
  gd->store(flashes);
  gd->store(fallanims);
  gd->store(radar_transform);
  gd->store(actual_view_transform);
  });

//=== animation

#if HDR
struct animation {
  int ltick;
  double footphase;
  transmatrix wherenow;
  int attacking;
  transmatrix attackat;
  bool mirrored;
  };

// we need separate animation layers for Orb of Domination and Tentacle+Ghost,
// and also to mark Boats
#define ANIMLAYERS 3
#define LAYER_BIG   0 // for worms and krakens
#define LAYER_SMALL 1 // for others
#define LAYER_BOAT  2 // mark that a boat has moved
#endif

EX array<map<cell*, animation>, ANIMLAYERS> animations;

EX int revhint(cell *c, int hint) {
  if(hint >= 0 && hint < c->type) return c->c.spin(hint);
  else return hint;
  }

EX transmatrix adj(const movei& m) {
  if(m.proper()) return currentmap->adj(m.s, m.d);
  else return currentmap->relative_matrix(m.t, m.s, C0);
  }

EX transmatrix iadj(const movei& m) {
  if(m.proper()) return currentmap->iadj(m.s, m.d);
  else return currentmap->relative_matrix(m.s, m.t, C0);
  }

EX void animateMovement(const movei& m, int layer) {
  if(vid.mspeed >= 5) return; // no animations!
  transmatrix T = adj(m);
  animation& a = animations[layer][m.t];
  if(animations[layer].count(m.s)) {
    a = animations[layer][m.s];
    a.wherenow = T * a.wherenow;
    animations[layer].erase(m.s);
    a.attacking = 0;
    }
  else {
    a.ltick = ticks;
    a.wherenow = T;
    a.footphase = 0;
    a.mirrored = false;
    }
  if(m.proper() && m.s->c.mirror(m.d))
    a.mirrored = !a.mirrored;
  }

EX void animateMovement(cell *src, cell *tgt, int layer, int direction_hint) {
  animateMovement(movei(src, tgt, direction_hint), layer);
  }

EX void animateAttack(cell *src, cell *tgt, int layer, int direction_hint) {
  animateAttack(movei(src, tgt, direction_hint), layer);
  }

EX void animateAttack(const movei& m, int layer) {
  if(vid.mspeed >= 5) return; // no animations!
  transmatrix T = adj(m);
  bool newanim = !animations[layer].count(m.s);
  animation& a = animations[layer][m.s];
  a.attacking = 1;
  a.attackat = rspintox(tC0(inverse(T))) * xpush(hdist0(T*C0) / 3);
  if(newanim) a.wherenow = Id, a.ltick = ticks, a.footphase = 0;
  }

vector<pair<cell*, animation> > animstack;

EX void indAnimateMovement(const movei& m, int layer) {
  if(vid.mspeed >= 5) return; // no animations!
  if(animations[layer].count(m.t)) {
    animation res = animations[layer][m.t];
    animations[layer].erase(m.t);
    animateMovement(m, layer);
    if(animations[layer].count(m.t)) 
      animstack.push_back(make_pair(m.t, animations[layer][m.t]));
    animations[layer][m.t] = res;
    }
  else {
    animateMovement(m, layer);
    if(animations[layer].count(m.t)) {
      animstack.push_back(make_pair(m.t, animations[layer][m.t]));
      animations[layer].erase(m.t);
      }
    }
  }

EX void commitAnimations(int layer) {
  for(int i=0; i<isize(animstack); i++)
    animations[layer][animstack[i].first] = animstack[i].second;
  animstack.clear();
  }

EX void drawBug(const cellwalker& cw, color_t col) {
#if CAP_SHAPES
  initquickqueue();
  transmatrix V = ggmatrix(cw.at);
  if(cw.spin) V = V * ddspin(cw.at, cw.spin, M_PI);
  queuepoly(V, cgi.shBugBody, col);
  quickqueue();
#endif
  }

EX bool inscreenrange(cell *c) {
  if(sphere) return true;
  if(euclid) return celldistance(centerover, c) <= get_sightrange_ambush();
  if(nonisotropic) return gmatrix.count(c);
  if(geometry == gCrystal344) return gmatrix.count(c);
  return heptdistance(centerover, c) <= 8;
  }

#if MAXMDIM >= 4
auto hooksw = addHook(hooks_swapdim, 100, [] { clearAnimations(); gmatrix.clear(); gmatrix0.clear(); });
#endif

}
