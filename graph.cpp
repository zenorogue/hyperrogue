// Hyperbolic Rogue -- main graphics file

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// basic graphics:

int inmirrorcount = 0;

bool wmspatial, wmescher, wmplain, wmblack, wmascii;
bool mmspatial, mmhigh, mmmon, mmitem;

int detaillevel = 0;

hookset<bool(int sym, int uni)> *hooks_handleKey;
hookset<void(cell *c, const transmatrix& V)> *hooks_drawcell;
purehookset hooks_frame;

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
  
int firecolor(int phase) {
  return gradient(0xFFFF00, 0xFF0000, -1, sin((phase + ticks)/100.0), 1);
  }

int watercolor(int phase) {
  return 0x0080C0FF + 256 * int(63 * sin((ticks + phase) / 50.));
  }

int aircolor(int phase) {
  return 0x8080FF00 | int(32 + 32 * sin(ticks/200.0 + 2 * phase * M_PI / (S21+.0)));
  }

int fghostcolor(int phase, cell *c) {
  phase += (int)(size_t)c;  
  phase %= 4000;
  if(phase<0) phase+=4000;
  if(phase < 1000)      return gradient(0xFFFF80, 0xA0C0FF,    0, phase, 1000);
  else if(phase < 2000) return gradient(0xA0C0FF, 0xFF80FF, 1000, phase, 2000);
  else if(phase < 3000) return gradient(0xFF80FF, 0xFF8080, 2000, phase, 3000);
  else if(phase < 4000) return gradient(0xFF8080, 0xFFFF80, 3000, phase, 4000);
  return 0xFFD500;
  }

int weakfirecolor(int phase) {
  return gradient(0xFF8000, 0xFF0000, -1, sin((phase + ticks)/500.0), 1);
  }

int fc(int ph, int col, int z) {
  if(items[itOrbFire]) col = darkena(firecolor(ph), 0, 0xFF);
  if(items[itOrbAether]) col = (col &~0XFF) | (col&0xFF) / 2;
  for(int i=0; i<numplayers(); i++) if(multi::playerActive(i))
    if(items[itOrbFish] && isWatery(playerpos(i)) && z != 3) return watercolor(ph);
  if(invismove) 
    col = 
      shmup::on ?
        (col &~0XFF) | (int((col&0xFF) * .25))
      : (col &~0XFF) | (int((col&0xFF) * (100+100*sin(ticks / 500.)))/200);
  return col;
  }

int lightat, safetyat;
void drawLightning() { lightat = ticks; }
void drawSafety() { safetyat = ticks; }

void drawShield(const transmatrix& V, eItem it) {
#if CAP_CURVE
  float ds = ticks / 300.;
  int col = iinf[it].color;
  if(it == itOrbShield && items[itOrbTime] && !orbused[it])
    col = (col & 0xFEFEFE) / 2;
  if(sphere && cwt.c->land == laHalloween && !wmblack && !wmascii)
    col = 0;
  double d = it == itOrbShield ? hexf : hexf - .1;
  int mt = sphere ? 7 : 5;
  for(int a=0; a<=S84*mt; a++)
    curvepoint(V*ddi0(a, d + sin(ds + M_PI*2*a/4/mt)*.1));
  queuecurve(darkena(col, 0, 0xFF), 0x8080808, PPR_LINE);
#endif
  }

void drawSpeed(const transmatrix& V) {
#if CAP_CURVE
  ld ds = ticks / 10.;
  int col = darkena(iinf[itOrbSpeed].color, 0, 0xFF);
  for(int b=0; b<S84; b+=S14) {
    for(int a=0; a<=S84; a++)
      curvepoint(V*ddi0(ds+b+a, hexf*a/S84));
    queuecurve(col, 0x8080808, PPR_LINE);
    }
#endif
  }

int ctof(cell *c) {
  if(nonbitrunc && !gp::on) return 1;
  // if(euclid) return 0;
  return ishept(c) ? 1 : 0;
  // c->type == 6 ? 0 : 1;
  }

int ctof012(cell *c) {
  return ishept(c)?1:ishex1(c)?0:2;
  }

void drawSafety(const transmatrix& V, int ct) {
#if CAP_QUEUE
  ld ds = ticks / 50.;
  int col = darkena(iinf[itOrbSafety].color, 0, 0xFF);
  for(int a=0; a<ct; a++)
    queueline(V*ddi0(ds+a*S84/ct, 2*hexf), V*ddi0(ds+(a+(ct-1)/2)*S84/ct, 2*hexf), col);
#endif
  }

void drawFlash(const transmatrix& V) {
#if CAP_CURVE
  float ds = ticks / 300.;
  int col = darkena(iinf[itOrbFlash].color, 0, 0xFF);
  col &= ~1;
  for(int u=0; u<5; u++) {
    ld rad = hexf * (2.5 + .5 * sin(ds+u*.3));
    for(int a=0; a<=S84; a++) curvepoint(V*ddi0(a, rad));
    queuecurve(col, 0x8080808, PPR_LINE);
    }
#endif
  }

void drawLove(const transmatrix& V, int hdir) {
#if CAP_CURVE
  float ds = ticks / 300.;
  int col = darkena(iinf[itOrbLove].color, 0, 0xFF);
  col &= ~1;
  for(int u=0; u<5; u++) {
    for(int a=0; a<=S84; a++) {
      double d = (1 + cos(a * M_PI/S42)) / 2;
      int z = a; if(z>S42) z = S84-z;
      if(z <= 10) d += (10-z) * (10-z) * (10-z) / 3000.;

      ld rad = hexf * (2.5 + .5 * sin(ds+u*.3)) * d;
      curvepoint(V*ddi0(S42+hdir+a-1, rad));
      }
    queuecurve(col, 0x8080808, PPR_LINE);
    }
#endif
  }

void drawWinter(const transmatrix& V, int hdir) {
#if CAP_QUEUE
  float ds = ticks / 300.;
  int col = darkena(iinf[itOrbWinter].color, 0, 0xFF);
  for(int u=0; u<20; u++) {
    ld rad = 6 * sin(ds+u * 2 * M_PI / 20);
    queueline(V*ddi0(S42+hdir+rad, hexf*.5), V*ddi0(S42+hdir+rad, hexf*3), col, 2);
    }
#endif
  }

void drawLightning(const transmatrix& V) {
#if CAP_QUEUE
  int col = darkena(iinf[itOrbLightning].color, 0, 0xFF);
  for(int u=0; u<20; u++) {
    ld leng = 0.5 / (0.1 + (rand() % 100) / 100.0);
    ld rad = rand() % S84;
    queueline(V*ddi0(rad, hexf*0.3), V*ddi0(rad, hexf*leng), col, 2);
    }
#endif
  }

int displaydir(cell *c, int d) {
  if(euclid)
    return - d * S84 / c->type;
  else
    return S42 - d * S84 / c->type;
  }

double hexshiftat(cell *c) {
  if(ctof(c) && S7==6 && S3 == 4 && !nonbitrunc) return hexshift + 2*M_PI/S7;
  if(ctof(c) && (S7==8 || S7 == 4) && S3 == 3 && !nonbitrunc) return hexshift + 2*M_PI/S7;
  if(hexshift && ctof(c)) return hexshift;
  return 0;
  }

transmatrix ddspin(cell *c, int d, int bonus) {
  int hdir = displaydir(c, d) + bonus;
  double ha = hexshiftat(c);
  if(ha) return spin(-ha) * getspinmatrix(hdir);
  return getspinmatrix(hdir);
  }

transmatrix iddspin(cell *c, int d, int bonus) {
  int hdir = displaydir(c, d) + bonus;
  double ha = hexshiftat(c);
  if(ha) return spin(ha) * getspinmatrix(-hdir);
  return getspinmatrix(-hdir);
  }

void drawPlayerEffects(const transmatrix& V, cell *c, bool onplayer) {
  if(!onplayer && !items[itOrbEmpathy]) return;
  if(items[itOrbShield] > (shmup::on ? 0 : ORBBASE)) drawShield(V, itOrbShield);
  if(items[itOrbShell] > (shmup::on ? 0 : ORBBASE)) drawShield(V, itOrbShell);

  if(items[itOrbSpeed]) drawSpeed(V); 

  if(onplayer && (items[itOrbSword] || items[itOrbSword2])) {
    using namespace sword;
  
    double esh = euclid ? M_PI - M_PI*3/S84 + 2.5 * M_PI/S42: 0;
    
    if(shmup::on) {
#if CAP_POLY
      if(items[itOrbSword])
        queuepoly(V*spin(esh+shmup::pc[multi::cpid]->swordangle), (peace::on ? shMagicShovel : shMagicSword), darkena(iinf[itOrbSword].color, 0, 0xC0 + 0x30 * sin(ticks / 200.0)));
  
      if(items[itOrbSword2])
        queuepoly(V*spin(esh+shmup::pc[multi::cpid]->swordangle+M_PI), (peace::on ? shMagicShovel : shMagicSword), darkena(iinf[itOrbSword2].color, 0, 0xC0 + 0x30 * sin(ticks / 200.0)));
#endif
      }                  
    
    else {
      int& ang = angle[multi::cpid];
      ang %= S42;
      
      transmatrix Vnow = gmatrix[c] * rgpushxto0(inverse(gmatrix[c]) * tC0(V)) * spin(-hexshiftat(c));
      
#if CAP_QUEUE
      if(!euclid) for(int a=0; a<S42; a++) {
        int dda = S42 + (-1-2*a);
        if(a == ang && items[itOrbSword]) continue;
        if(nonbitrunc && !gp::on && a%3 != ang%3) continue;
        if((a+S21)%S42 == ang && items[itOrbSword2]) continue;
        bool longer = sword::pos(cwt.c, a-1) != sword::pos(cwt.c, a+1);
        int col = darkena(0xC0C0C0, 0, 0xFF);
        queueline(Vnow*ddi0(dda, nonbitrunc ? 0.6 * gp::scale : longer ? 0.36 : 0.4), Vnow*ddi0(dda, nonbitrunc ? 0.7 * gp::scale : longer ? 0.44 : 0.42), col, 1);
        }
#endif

#if CAP_POLY
      if(items[itOrbSword])
        queuepoly(Vnow*spin(esh+M_PI+(-1-2*ang)*2*M_PI/S84), (peace::on ? shMagicShovel : shMagicSword), darkena(iinf[itOrbSword].color, 0, 0x80 + 0x70 * sin(ticks / 200.0)));
  
      if(items[itOrbSword2])
        queuepoly(Vnow*spin(esh+(-1-2*ang)*2*M_PI/S84), (peace::on ? shMagicShovel : shMagicSword), darkena(iinf[itOrbSword2].color, 0, 0x80 + 0x70 * sin(ticks / 200.0)));
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
      int col = darkena(iinf[itOrbSafety].color, 0, 0xFF);
      for(int a=0; a<S84; a++)
        queueline(V*ddi0(a, rad), V*ddi0(a+1, rad), col, 0);
      }
    }
  }

void drawStunStars(const transmatrix& V, int t) {
#if CAP_POLY
  for(int i=0; i<3*t; i++) {
    transmatrix V2 = V * spin(M_PI * 2 * i / (3*t) + M_PI * ticks/600.);
    queuepolyat(V2, shFlailBall, 0xFFFFFFFF, PPR_STUNSTARS);
    }
#endif
  }

namespace tortoise {

  // small is 0 or 2
  void draw(const transmatrix& V, int bits, int small, int stuntime) {

#if CAP_POLY      
    int eyecolor = getBit(bits, tfEyeHue) ? 0xFF0000 : 0xC0C0C0;
    int shellcolor = getBit(bits, tfShellHue) ? 0x00C040 : 0xA06000;
    int scutecolor = getBit(bits, tfScuteHue) ? 0x00C040 : 0xA06000;
    int skincolor = getBit(bits, tfSkinHue) ? 0x00C040 : 0xA06000;
    if(getBit(bits, tfShellSat)) shellcolor = gradient(shellcolor, 0xB0B0B0, 0, .5, 1);
    if(getBit(bits, tfScuteSat)) scutecolor = gradient(scutecolor, 0xB0B0B0, 0, .5, 1);
    if(getBit(bits, tfSkinSat)) skincolor = gradient(skincolor, 0xB0B0B0, 0, .5, 1);
    if(getBit(bits, tfShellDark)) shellcolor = gradient(shellcolor, 0, 0, .5, 1);
    if(getBit(bits, tfSkinDark)) skincolor = gradient(skincolor, 0, 0, .5, 1);
    
    for(int i=0; i<12; i++) {
      int col = 
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
                                 
void animallegs(const transmatrix& V, eMonster mo, int col, double footphase) {
#if CAP_POLY
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

  if(x[0]) queuepolyat(V * xpush(rightfoot), *x[0], col, PPR_MONSTER_FOOT);
  if(x[0]) queuepolyat(V * Mirror * xpush(leftfoot), *x[0], col, PPR_MONSTER_FOOT);
  if(x[1]) queuepolyat(V * xpush(-rightfoot), *x[1], col, PPR_MONSTER_FOOT);
  if(x[1]) queuepolyat(V * Mirror * xpush(-leftfoot), *x[1], col, PPR_MONSTER_FOOT);


  if(x[2]) queuepolyat(VAML * xpush(rightfoot/2), *x[2], col, PPR_MONSTER_FOOT);
  if(x[2]) queuepolyat(VAML * Mirror * xpush(leftfoot/2), *x[2], col, PPR_MONSTER_FOOT);
  if(x[3]) queuepolyat(VAML * xpush(-rightfoot/2), *x[3], col, PPR_MONSTER_FOOT);
  if(x[3]) queuepolyat(VAML * Mirror * xpush(-leftfoot/2), *x[3], col, PPR_MONSTER_FOOT);
#endif
  }

void ShadowV(const transmatrix& V, const hpcshape& bp, int prio) {
#if CAP_POLY
  if(mmspatial) { 
    if(pmodel == mdHyperboloid || pmodel == mdBall || pmodel == mdHemisphere) 
      return; // shadows break the depth testing
    dynamicval<int> p(poly_outline, OUTLINE_TRANS);
    queuepolyat(V, bp, SHADOW_MON, prio); 
    }
#endif
  }


void otherbodyparts(const transmatrix& V, int col, eMonster who, double footphase) {

#if CAP_POLY

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
#define VBIRD  mmscale(V, geom3::BIRD + .05 * sin((int) (size_t(where)) + ticks / 1000.))
#define VGHOST  mmscale(V, geom3::GHOST)

#define VSLIMEEYE  mscale(V, geom3::FLATEYE)

  // if(!mmspatial && !footphase && who != moSkeleton) return;
  
  footphase /= SCALE;
  double rightfoot = footfun(footphase / .4 / 2.5) / 4 * 2.5;

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
    return;
    }
  else if(isTroll(who) || who == moMonkey || who == moYeti || who == moRatling || who == moRatlingAvenger || who == moGoblin) {
    queuepoly(VFOOT * xpush(rightfoot), shYetiFoot, col);
    queuepoly(VFOOT * Mirror * xpush(-rightfoot), shYetiFoot, col);
    }
  else {
    queuepoly(VFOOT * xpush(rightfoot), shHumanFoot, col);
    queuepoly(VFOOT * Mirror * xpush(-rightfoot), shHumanFoot, col);
    }

  if(!mmspatial) return;

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
#endif
  }

bool drawstar(cell *c) {
  for(int t=0; t<c->type; t++) 
    if(c->mov[t] && c->mov[t]->wall != waSulphur && c->mov[t]->wall != waSulphurC &&
     c->mov[t]->wall != waBarrier)
       return false;
  return true;
  }

bool drawItemType(eItem it, cell *c, const transmatrix& V, int icol, int ticks, bool hidden) {
  char xch = iinf[it].glyph;
#if !CAP_POLY
  return it;
#else
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
    it == itShard ? &shMFloor[0] :
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
  
  if(c && conformal::includeHistory && eq(c->aitmp, sval)) poly_outline = OUTLINE_DEAD;
    
  if(!mmitem && it) return true;
  
  else if(it == itSavedPrincess) {
    drawMonsterType(moPrincess, c, V, icol, 0);
    return false;
    }
  
  else if(it == itStrongWind) {
    queuepoly(V * spin(ticks / 750.), shFan, darkena(icol, 0, 255));
    }

  else if(it == itWarning) {
    queuepoly(V * spin(ticks / 750.), shTriangle, darkena(icol, 0, 255));
    }
    
  else if(it == itBabyTortoise) {
    int bits = c ? tortoise::babymap[c] : tortoise::last;
    int over = c && c->monst == moTortoise;
    tortoise::draw(V * spin(ticks / 5000.) * ypush(crossf*.15), bits, over ? 4 : 2, 0);
    // queuepoly(V, shHeptaMarker, darkena(tortoise::getMatchColor(bits), 0, 0xC0));
    }
  
  else if(it == itCompass) {
    cell *c1 = c ? findcompass(c) : NULL;
    transmatrix V2;
    if(c1) {
      transmatrix P = shmup::ggmatrix(c1);
      hyperpoint P1 = tC0(P);
      
      if(isPlayerOn(c)) {
        queuechr(P1, 2*vid.fsize, 'X', 0x10100 * int(128 + 100 * sin(ticks / 150.)));
  //      queuestr(V, 1, its(compassDist(c)), 0x10101 * int(128 - 100 * sin(ticks / 150.)), 1);
        queuestr(P1, vid.fsize, its(-compassDist(c)), 0x10101 * int(128 - 100 * sin(ticks / 150.)));
        addauraspecial(P1, 0x0000FF, 0);
        }
      
      V2 = V * rspintox(inverse(V) * P1);
      }
    else V2 = V;
    V2 = V2 * spin(M_PI * sin(ticks/100.) / 30);
    queuepoly(V2, shCompass1, 0xFF8080FF);
    queuepoly(V2, shCompass2, 0xFFFFFFFF);
    queuepoly(V2, shCompass3, 0xFF0000FF);
    queuepoly(V2 * pispin, shCompass3, 0x000000FF);
    xsh = NULL;
    }

  else if(it == itPalace) {
    transmatrix V2 = V * spin(ticks / 1500.);
    queuepoly(V2, shMFloor3[ct6], 0xFFD500FF);
    queuepoly(V2, shMFloor4[ct6], darkena(icol, 0, 0xFF));
    queuepoly(V2, shGem[ct6], 0xFFD500FF);
    xsh = NULL;
    }
  
  else if(mapeditor::drawUserShape(V, 2, it, darkena(icol, 0, 0xFF), c)) ;
  
  else if(it == itRose) {
    for(int u=0; u<4; u++)
      queuepoly(V * spin(ticks / 1500.) * spin(2*M_PI / 3 / 4 * u), shRose, darkena(icol, 0, hidden ? 0x30 : 0xA0));
    }

  else if(it == itBarrow && c) {
    for(int i = 0; i<c->landparam; i++)
      queuepolyat(V * spin(2 * M_PI * i / c->landparam) * xpush(.15) * spin(ticks / 1500.), *xsh, darkena(icol, 0, hidden ? 0x40 : 
        (highwall(c) && wmspatial) ? 0x60 : 0xFF),
        PPR_HIDDEN);

//      queuepoly(V*spin(M_PI+(1-2*ang)*2*M_PI/S84), shMagicSword, darkena(0xC00000, 0, 0x80 + 0x70 * sin(ticks / 200.0)));
    }
    
  else if(xsh) {
    if(it == itFireShard) icol = firecolor(100);
    if(it == itWaterShard) icol = watercolor(100) >> 8;
    
    if(it == itZebra) icol = 0xFFFFFF;
    if(it == itLotus) icol = 0x101010;
    if(it == itSwitch) icol = minf[active_switch()].color;
    
    transmatrix V2 = V * spin(ticks / 1500.);
  
    if(xsh == &shBookCover && mmitem)
      queuepoly(V2, shBook, 0x805020FF);
    
    int pr = PPR_ITEM;
    int alpha = hidden ? (it == itKraken ? 0xC0 : 0x40) : 0xF0;
    if(c && c->wall == waIcewall) pr = PPR_HIDDEN, alpha = 0x80;

    queuepolyat(V2, *xsh, darkena(icol, 0, alpha), pr);

    if(it == itZebra)
      queuepolyat(V * spin(ticks / 1500. + M_PI/(ct6+6)), *xsh, darkena(0x202020, 0, hidden ? 0x40 : 0xF0), PPR_ITEMb);
    /* if(it == itSwitch)
      queuepolyat(V * spin(ticks / 1500. + M_PI/S3), *xsh, darkena(minf[active_switch()].color, 0, hidden ? 0x40 : 0xF0), PPR_ITEMb); */
    }
  
  else if(xch == 'o' || it == itInventory) {
    if(it == itOrbFire) icol = firecolor(100);
    int pr = PPR_ITEM;
    bool inice = c && c->wall == waIcewall;
    if(inice) pr = PPR_HIDDEN;

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
    int col = darkena(icol, 0, int(0x80 + 0x70 * sin(ticks / 300.)));
    
    if(it == itOrbFish)
      queuepolyat(V * spin(ticks / 1500.), shFishTail, col, PPR_ITEM_BELOW);

    queuepolyat(V, shDisk, darkena(icol1, 0, inice ? 0x80 : hidden ? 0x20 : 0xC0), pr);
    hpcshape& sh = 
      it == itOrbLove ? shLoveRing :
      isRangedOrb(it) ? shTargetRing :
      isOffensiveOrb(it) ? shSawRing :
      isFriendOrb(it) ? shPeaceRing :
      isUtilityOrb(it) ? shGearRing :
      isDirectionalOrb(it) ? shSpearRing :
      it == itOrb37 ? shHeptaRing :
      shRing;
    queuepolyat(V * spin(ticks / 1500.), sh, col, pr);
    }

  else if(it) return true;

  return false;
#endif
  }

void drawTerraWarrior(const transmatrix& V, int t, int hp, double footphase) {
#if CAP_POLY
  ShadowV(V, shPBody);
  int col = linf[laTerracotta].color;
  int bcol = darkena(false ? 0xC0B23E : col, 0, 0xFF);
  otherbodyparts(V, bcol, moDesertman, footphase);
  queuepoly(VBODY, shPBody, bcol);
  if(!peace::on) queuepoly(VBODY * Mirror, shPSword, darkena(0xC0C0C0, 0, 0xFF));
  queuepoly(VBODY, shTerraArmor1, darkena(t > 0 ? 0x4040FF : col, 0, 0xFF));
  if(hp >= 4) queuepoly(VBODY, shTerraArmor2, darkena(t > 1 ? 0xC00000 : col, 0, 0xFF));
  if(hp >= 2) queuepoly(VBODY, shTerraArmor3, darkena(t > 2 ? 0x612600 : col, 0, 0xFF));
  queuepoly(VHEAD, shTerraHead, darkena(t > 4 ? 0x202020 : t > 3 ? 0x504040 : col, 0, 0xFF));
  queuepoly(VHEAD, shPFace, bcol);
#endif
  }

bool drawMonsterType(eMonster m, cell *where, const transmatrix& V, int col, double footphase) {

  char xch = minf[m].glyph;

#if CAP_POLY
  if(m == moTortoise && where && where->stuntime >= 3)
    drawStunStars(V, where->stuntime-2);
  else if (m == moTortoise || m == moPlayer || (where && !where->stuntime)) ;
  else if(where && !(isMetalBeast(m) && where->stuntime == 1))
    drawStunStars(V, where->stuntime);
    
  if(m == moTortoise) {
    int bits = where ? tortoise::getb(where) : tortoise::last;
    tortoise::draw(V, bits, 0, where ? where->stuntime : 0);
    if(tortoise::seek() && !tortoise::diff(bits) && where)
      queuepoly(V, shRing, darkena(0xFFFFFF, 0, 0x80 + 0x70 * sin(ticks / 200.0)));
    }
    
  else if(m == moPlayer) {
  
    charstyle& cs = getcs();
      
    bool havus = mapeditor::drawUserShape(V, 0, cs.charid, cs.skincolor, where);
    
    if(mapeditor::drawplayer && !havus) {
    
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
          int col = items[itOrbDiscord] ? watercolor(0) : fc(314, cs.swordcolor, 3);
          queuepoly(VAHEAD, shFamiliarEye, col);
          queuepoly(VAHEAD * Mirror, shFamiliarEye, col);
          }
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
          int col = items[itOrbDiscord] ? watercolor(0) : fc(314, cs.swordcolor, 3);
          queuepoly(VAHEAD, shWolf1, col);
          queuepoly(VAHEAD, shWolf2, col);
          }

        int colnose = items[itOrbDiscord] ? watercolor(0) : fc(314, 0xFF, 3);
        queuepoly(VAHEAD, shWolf3, colnose);
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
          int col = items[itOrbDiscord] ? watercolor(0) : fc(314, cs.swordcolor, 3);
          queuepoly(VAHEAD * xpush(.04), shWolf1, col);
          queuepoly(VAHEAD * xpush(.04), shWolf2, col);
          }
        }
      else {

      otherbodyparts(V, fc(0, cs.skincolor, 0), items[itOrbFish] ? moWaterElemental : moPlayer, footphase);

      queuepoly(VBODY, (cs.charid&1) ? shFemaleBody : shPBody, fc(0, cs.skincolor, 0));      

      ShadowV(V, (cs.charid&1) ? shFemaleBody : shPBody);

      if(cs.charid&1)
        queuepoly(VBODY, shFemaleDress, fc(500, cs.dresscolor, 4));

      if(cs.charid == 2)
        queuepoly(VBODY, shPrinceDress,  fc(400, cs.dresscolor, 5));
      if(cs.charid == 3) 
        queuepoly(VBODY, shPrincessDress,  fc(400, cs.dresscolor2, 5));
        
      if(items[itOrbSide3])
        queuepoly(VBODY, (cs.charid&1) ? shFerocityF : shFerocityM, fc(0, cs.skincolor, 0));

      if(items[itOrbHorns]) {
        queuepoly(VBODY, shBullHead, items[itOrbDiscord] ? watercolor(0) : 0xFF000030);
        queuepoly(VBODY, shBullHorn, items[itOrbDiscord] ? watercolor(0) : 0xFF000040);
        queuepoly(VBODY * Mirror, shBullHorn, items[itOrbDiscord] ? watercolor(0) : 0xFF000040);
        }

      if(items[itOrbSide1] && !shmup::on)
        queuepoly(VBODY * spin(-M_PI/24), cs.charid >= 2 ? shSabre : shPSword, fc(314, cs.swordcolor, 3)); // 3 not colored
      
      if(peace::on) ;
      else if(items[itOrbThorns])
        queuepoly(VBODY, shHedgehogBladePlayer, items[itOrbDiscord] ? watercolor(0) : 0x00FF00FF);
      else if(!shmup::on && items[itOrbDiscord])
        queuepoly(VBODY, cs.charid >= 2 ? shSabre : shPSword, watercolor(0));
      else if(items[itRevolver])
        queuepoly(VBODY, shGunInHand, fc(314, cs.swordcolor, 3)); // 3 not colored
      else if(items[itOrbSlaying]) {
        queuepoly(VBODY, shFlailTrunk, fc(314, cs.swordcolor, 3));
        queuepoly(VBODY, shHammerHead, fc(314, cs.swordcolor, 3));
        }
      else if(!shmup::on)
        queuepoly(VBODY, cs.charid >= 2 ? shSabre : shPSword, fc(314, cs.swordcolor, 3)); // 3 not colored
      else if(shmup::curtime >= shmup::getPlayer()->nextshot)
        queuepoly(VBODY, shPKnife, fc(314, cs.swordcolor, 3)); // 3 not colored
      
      if(items[itOrbBeauty]) {
        if(cs.charid&1)
          queuepoly(VHEAD, shFlowerHair, darkena(iinf[itOrbBeauty].color, 0, 0xFF));
        else
          queuepoly(VBODY, shFlowerHand, darkena(iinf[itOrbBeauty].color, 0, 0xFF));
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
      }

      if(knighted)
        queuepoly(VBODY, shKnightCloak, darkena(cloakcolor(knighted), 1, 0xFF));

      if(tortoise::seek())
        tortoise::draw(VBODY * ypush(-crossf*.25), tortoise::seekbits, 4, 0);

      }
    }
  
  else if(mapeditor::drawUserShape(V, 1, m, darkena(col, 0, 0xFF), where)) return false;

  else if(isMimic(m) || m == moShadow || m == moIllusion) {
    charstyle& cs = getcs();
    
    if(mapeditor::drawUserShape(V, 0, (cs.charid&1)?1:0, darkena(col, 0, 0x80), where)) return false;
    
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
      otherbodyparts(V, darkena(col, 0, 0x40), m, footphase);
      queuepoly(VBODY, (cs.charid&1) ? shFemaleBody : shPBody,  darkena(col, 0, 0X80));
  
      if(!shmup::on) {
        bool emp = items[itOrbEmpathy] && m != moShadow;
        if(items[itOrbThorns] && emp)
          queuepoly(VBODY, shHedgehogBladePlayer, darkena(col, 0, 0x40));
        if(items[itOrbSide1] && !shmup::on)
          queuepoly(VBODY * spin(-M_PI/24), cs.charid >= 2 ? shSabre : shPSword, darkena(col, 0, 0x40));      
        if(items[itOrbSide3] && emp)
          queuepoly(VBODY, (cs.charid&1) ? shFerocityF : shFerocityM, darkena(col, 0, 0x40));

        queuepoly(VBODY, (cs.charid >= 2 ? shSabre : shPSword), darkena(col, 0, 0XC0));
        }
      else if(!where || shmup::curtime >= shmup::getPlayer()->nextshot)
        queuepoly(VBODY, shPKnife, darkena(col, 0, 0XC0));

      if(knighted)
        queuepoly(VBODY, shKnightCloak, darkena(col, 1, 0xC0));
  
      queuepoly(VHEAD, (cs.charid&1) ? shFemaleHair : shPHead,  darkena(col, 1, 0XC0));
      queuepoly(VHEAD, shPFace,  darkena(col, 0, 0XC0));
      if(cs.charid&1)
        queuepoly(VBODY, shFemaleDress,  darkena(col, 1, 0XC0));
      if(cs.charid == 2)
        queuepoly(VBODY, shPrinceDress,  darkena(col, 1, 0XC0));
      if(cs.charid == 3)
        queuepoly(VBODY, shPrincessDress,  darkena(col, 1, 0XC0));
      }
    }
  
  else if(m == moBullet) {
    ShadowV(V, shKnife);
    queuepoly(VBODY * spin(-M_PI/4), shKnife, getcs().swordcolor);
    }
  
  else if(m == moKnight || m == moKnightMoved) {
    ShadowV(V, shPBody);
    otherbodyparts(V, darkena(0xC0C0A0, 0, 0xC0), m, footphase);
    queuepoly(VBODY, shPBody, darkena(0xC0C0A0, 0, 0xC0));
    queuepoly(VBODY, shPSword, darkena(0xFFFF00, 0, 0xFF));
    queuepoly(VBODY, shKnightArmor, darkena(0xD0D0D0, 1, 0xFF));
    int col;
    if(!euclid && where && where->master->alt)
      col = cloakcolor(roundTableRadius(where));
    else
      col = cloakcolor(newRoundTableRadius());
    queuepoly(VBODY, shKnightCloak, darkena(col, 1, 0xFF));
    queuepoly(VHEAD, shPHead, darkena(0x703800, 1, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xC0C0A0, 0, 0XFF));
    return false;
    }
  
  else if(m == moGolem || m == moGolemMoved) {
    ShadowV(V, shPBody);
    otherbodyparts(V, darkena(col, 1, 0XC0), m, footphase);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0XC0));
    queuepoly(VHEAD, shGolemhead, darkena(col, 1, 0XFF));
    }

  else if(isPrincess(m) || m == moFalsePrincess || m == moRoseLady || m == moRoseBeauty) {
  
    bool girl = princessgender() == GEN_F;
    bool evil = !isPrincess(m);

    int facecolor = evil ? 0xC0B090FF : 0xD0C080FF;
    
    ShadowV(V, girl ? shFemaleBody : shPBody);
    otherbodyparts(V, facecolor, m, footphase);
    queuepoly(VBODY, girl ? shFemaleBody : shPBody, facecolor);

    if(m == moPrincessArmed) 
      queuepoly(VBODY, vid.cs.charid < 2 ? shSabre : shPSword, 0xFFFFFFFF);
    
    if((m == moFalsePrincess || m == moRoseBeauty) && where && where->cpdist == 1)
      queuepoly(VBODY, shPKnife, 0xFFFFFFFF);

    if(m == moRoseLady) {
      queuepoly(VBODY, shPKnife, 0xFFFFFFFF);
      queuepoly(VBODY * Mirror, shPKnife, 0xFFFFFFFF);
      }

    if(girl) {
      queuepoly(VBODY, shFemaleDress,  evil ? 0xC000C0FF : 0x00C000FF);
      if(vid.cs.charid < 2) 
        queuepoly(VBODY, shPrincessDress, evil ? 0xC040C0C0 : 0x8080FFC0);
      }
    else {
      if(vid.cs.charid < 2) 
        queuepoly(VBODY, shPrinceDress,  evil ? 0x802080FF : 0x404040FF);
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
        queuepoly(VBODY, shFlowerHand,  0xC00000FF);
        queuepoly(VBODY, shSuspenders,  0xC00000FF);
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
  else if(m == moRunDog || m == moHunterDog || m == moHunterGuard || m == moHunterChanging) {
    if(!mmspatial && !footphase) 
      queuepoly(VABODY, shDogBody, darkena(col, 0, 0xFF));
    else {
      ShadowV(V, shDogTorso);
      queuepoly(VABODY, shDogTorso, darkena(col, 0, 0xFF));
      animallegs(VALEGS, moRunDog, darkena(col, 0, 0xFF), footphase);
      }
    queuepoly(VAHEAD, shDogHead, darkena(col, 0, 0xFF));

    {
    dynamicval<int> dp(poly_outline);
    dynamicval<double> dw(minwidth_global);
    int eyecolor = 0x202020;
    bool redeyes = false;
    if(m == moHunterDog) eyecolor = 0xFF0000, redeyes = true;
    if(m == moHunterGuard) eyecolor = 0xFF6000, redeyes = true;
    if(m == moHunterChanging) eyecolor = 0xFFFF00, redeyes = true;
    int eyes = darkena(eyecolor, 0, 0xFF);

    if(redeyes) poly_outline = eyes, minwidth_global = 1;
    queuepoly(VAHEAD, shWolf1, eyes);
    queuepoly(VAHEAD, shWolf2, eyes);
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
    m == moSandBird) {
    ShadowV(V, shEagle);
    queuepoly(VBIRD, shEagle, darkena(col, 0, 0xFF));
    }
  else if(m == moSparrowhawk) {
    ShadowV(V, shHawk);
    queuepoly(VBIRD, shHawk, darkena(col, 0, 0xFF));
    }
  else if(m == moButterfly) {
    transmatrix Vwing = Id;
    Vwing[1][1] = .85 + .15 * sin(ticks / 100.0);
    ShadowV(V * Vwing, shButterflyWing);
    queuepoly(VBIRD * Vwing, shButterflyWing, darkena(col, 0, 0xFF));
    queuepoly(VBIRD, shButterflyBody, darkena(col, 2, 0xFF));
    }
  else if(m == moGadfly) {
    transmatrix Vwing = Id;
    Vwing[1][1] = .85 + .15 * sin(ticks / 100.0);
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
    otherbodyparts(V, c, m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, c);
    }
  else if(m == moTerraWarrior)
    drawTerraWarrior(V, 7, (where ? where->hitpoints : 7), footphase);
  else if(m == moDesertman) {
    otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0xC0));
    if(!peace::on) queuepoly(VBODY, shPSword, 0xFFFF00FF);
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
    otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    ShadowV(V, shRaiderBody);
    queuepoly(VBODY, shRaiderBody, darkena(col, 0, 0xFF));
    queuepoly(VBODY, shRaiderShirt, darkena(col, 2, 0xFF));
    if(!peace::on) queuepoly(VBODY, shPKnife, 0xFFC0C0C0);
    queuepoly(VBODY, shRaiderArmor, darkena(col, 1, 0xFF));
    queuepolyat(VBODY, shRatCape2, darkena(col, 2, 0xFF), PPR_MONSTER_ARMOR0);
    queuepoly(VHEAD, shRaiderHelmet, darkena(col, 0, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xC0C0A0, 0, 0XFF));
    }
  else if(m == moCrusher) {
    otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shRaiderBody);
    queuepoly(VBODY, shRaiderBody, darkena(col, 0, 0xFF));
    queuepoly(VBODY, shRaiderShirt, darkena(col, 2, 0xFF));
    queuepoly(VBODY, shRaiderArmor, darkena(col, 1, 0xFF));
    queuepoly(VBODY, shFlailTrunk, darkena(col, 1, 0XFF));
    queuepoly(VBODY, shHammerHead, darkena(col, 0, 0XFF));
    queuepoly(VHEAD, shRaiderHelmet, darkena(col, 0, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xC0C0A0, 0, 0XFF));
    }
  else if(m == moPair) {
    otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shRaiderBody);
    queuepoly(VBODY, shRaiderBody, darkena(col, 0, 0xFF));
    queuepoly(VBODY, shRaiderShirt, darkena(col, 2, 0xFF));
    queuepoly(VBODY, shRaiderArmor, darkena(col, 1, 0xFF));
    queuepoly(VBODY, shPickAxe, darkena(0xA0A0A0, 0, 0XFF));
    queuepoly(VHEAD, shRaiderHelmet, darkena(col, 0, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xC0C0A0, 0, 0XFF));
    }
  else if(m == moAltDemon || m == moHexDemon) {
    otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    ShadowV(V, shRaiderBody);
    queuepoly(VBODY, shRaiderBody, darkena(col, 0, 0xFF));
    queuepoly(VBODY, shRaiderShirt, darkena(col, 2, 0xFF));
    queuepoly(VBODY, shRaiderArmor, darkena(col, 1, 0xFF));
    if(!peace::on) queuepoly(VBODY, shPSword, 0xFFD0D0D0);
    queuepoly(VHEAD, shRaiderHelmet, darkena(col, 0, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xC0C0A0, 0, 0XFF));
    }
  else if(m == moPalace || m == moFatGuard || m == moVizier || m == moSkeleton) {
    queuepoly(VBODY, shSabre, 0xFFFFFFFF);
    if(m == moSkeleton) {
      otherbodyparts(V, darkena(0xFFFFFF, 0, 0xFF), moSkeleton, footphase);
      queuepoly(VBODY, shSkeletonBody, darkena(0xFFFFFF, 0, 0xFF));
      queuepoly(VHEAD, shSkull, darkena(0xFFFFFF, 0, 0xFF));
      queuepoly(VHEAD, shSkullEyes, darkena(0, 0, 0xFF));
      ShadowV(V, shSkeletonBody);
      }
    else {
      ShadowV(V, shPBody);
      otherbodyparts(V, darkena(0xFFD500, 0, 0xFF), m, footphase);
      if(m == moFatGuard) {
        queuepoly(VBODY, shFatBody, darkena(0xC06000, 0, 0xFF));
        col = 0xFFFFFF;
        if(!where || where->hitpoints >= 3)
          queuepoly(VBODY, shKnightCloak, darkena(0xFFC0C0, 1, 0xFF));
        }
      else {
        queuepoly(VBODY, shPBody, darkena(0xFFD500, 0, 0xFF));
        queuepoly(VBODY, shKnightArmor, m == moVizier ? 0xC000C0FF :
          darkena(0x00C000, 1, 0xFF));
        if(where && where->hitpoints >= 3)
          queuepoly(VBODY, shKnightCloak, m == moVizier ? 0x800080Ff :
            darkena(0x00FF00, 1, 0xFF));
        }
      queuepoly(VHEAD, shTurban1, darkena(col, 1, 0xFF));
      if(!where || where->hitpoints >= 2)
        queuepoly(VHEAD, shTurban2, darkena(col, 0, 0xFF));
      }
      
    drawStunStars(V, where ? where->stuntime : 0);
    }
  else if(m == moCrystalSage) {
    otherbodyparts(V, 0xFFFFFFFF, m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, 0xFFFFFFFF);
    queuepoly(VHEAD, shPHead, 0xFFFFFFFF);
    queuepoly(VHEAD, shPFace, 0xFFFFFFFF);
    }
  else if(m == moHedge) {
    ShadowV(V, shPBody);
    otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0xFF));
    queuepoly(VBODY, shHedgehogBlade, 0xC0C0C0FF);
    queuepoly(VHEAD, shPHead, 0x804000FF);
    queuepoly(VHEAD, shPFace, 0xF09000FF);
    }
  else if(m == moYeti || m == moMonkey) {
    otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(col, 0, 0xFF));
    }
  else if(m == moResearcher) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, darkena(0xFFFF00, 0, 0xC0));
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
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0xC0));
    if(!peace::on) queuepoly(VBODY, shPSword, darkena(col, 0, 0xFF));
    queuepoly(VHEAD, shArmor, darkena(col, 1, 0xFF));
    }
  else if(m == moNarciss) {
    ShadowV(V, shPBody);
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    queuepoly(VBODY, shFlowerHand, darkena(col, 0, 0xFF));
    queuepoly(VBODY, shPBody, 0xFFE080FF);
    if(!peace::on) queuepoly(VBODY, shPKnife, 0xC0C0C0FF);
    queuepoly(VHEAD, shPFace, 0xFFE080FF);
    queuepoly(VHEAD, shPHead, 0x806A00FF);
    }
  else if(m == moMirrorSpirit) {
    ShadowV(V, shPBody);
    otherbodyparts(V, darkena(col, 0, 0x90), m, footphase);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0x90));
    if(!peace::on) queuepoly(VBODY * Mirror, shPSword, darkena(col, 0, 0xD0));
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
    if(m == moFriendlyGhost) col = fghostcolor(ticks, where);
    queuepoly(VGHOST, shGhost, darkena(col, 0, m == moFriendlyGhost ? 0xC0 : 0x80));
    queuepoly(VGHOST, shEyes, 0xFF);
    }
  else if(m == moVineSpirit) {
    queuepoly(VGHOST, shGhost, 0xD0D0D0C0);
    queuepoly(VGHOST, shEyes, 0xFF0000FF);
    }
  else if(m == moFireFairy) {
    col = firecolor(0);
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shFemaleBody);
    queuepoly(VBODY, shFemaleBody, darkena(col, 0, 0XC0));
    queuepoly(VHEAD, shWitchHair, darkena(col, 1, 0xFF));
    queuepoly(VHEAD, shPFace, darkena(col, 0, 0XFF));
    }
  else if(m == moSlime) {
    queuepoly(VFISH, shSlime, darkena(col, 0, 0x80));
    queuepoly(VSLIMEEYE, shEyes, 0xFF);
    }
  else if(isSwitch(m)) {
    queuepoly(VFISH, shJelly, darkena(col, 0, 0xD0));
    queuepolyat(VBODY, shJelly, darkena(col, 0, 0xD0), PPR_MONSTER_BODY);
    queuepolyat(VHEAD, shJelly, darkena(col, 0, 0xD0), PPR_MONSTER_HEAD);
    queuepolyat(VHEAD, shEyes, 0xFF, PPR_MONSTER_HEAD);
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
    otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0xC0));
    if(!peace::on) queuepoly(VBODY, shPSword, darkena(col, 2, 0xFF));
    queuepoly(VHEAD, shHood, darkena(col, 1, 0xFF));
    }
  else if(m == moPirate) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, darkena(0x404040, 0, 0xFF));
    queuepoly(VBODY, shPirateHook, darkena(0xD0D0D0, 0, 0xFF));
    queuepoly(VHEAD, shPFace, darkena(0xFFFF80, 0, 0xFF));
    queuepoly(VHEAD, shEyepatch, darkena(0, 0, 0xC0));
    queuepoly(VHEAD, shPirateHood, darkena(col, 0, 0xFF));
    }
  else if(m == moRatling || m == moRatlingAvenger) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VLEG, shRatTail, darkena(col, 0, 0xFF));
    queuepoly(VBODY, shYeti, darkena(col, 1, 0xFF));
    queuepoly(VHEAD, shRatHead, darkena(col, 0, 0xFF));

    float t = sin(ticks / 1000.0 + (where ? where->cpdist : 0));
    int eyecol = t > 0.92 ? 0xFF0000 : 0;
    
    queuepoly(VHEAD, shWolf1, darkena(eyecol, 0, 0xFF));
    queuepoly(VHEAD, shWolf2, darkena(eyecol, 0, 0xFF));
    queuepoly(VHEAD, shWolf3, darkena(0x202020, 0, 0xFF));
    
    if(m == moRatlingAvenger) {
      queuepoly(VBODY, shRatCape2, 0x484848FF);
      queuepoly(VHEAD, shRatCape1, 0x303030FF);
      }
    }
  else if(m == moViking) {
    otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, darkena(0xE00000, 0, 0xFF));
    if(!peace::on) queuepoly(VBODY, shPSword, darkena(0xD0D0D0, 0, 0xFF));
    queuepoly(VBODY, shKnightCloak, darkena(0x404040, 0, 0xFF));
    queuepoly(VHEAD, shVikingHelmet, darkena(0xC0C0C0, 0, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xFFFF80, 0, 0xFF));
    }
  else if(m == moOutlaw) {
    otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0xFF));
    queuepoly(VBODY, shKnightCloak, darkena(col, 1, 0xFF));
    queuepoly(VHEAD, shWestHat1, darkena(col, 2, 0XFF));
    queuepoly(VHEAD, shWestHat2, darkena(col, 1, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xFFFF80, 0, 0xFF));
    queuepoly(VBODY, shGunInHand, darkena(col, 1, 0XFF));
    }
  else if(m == moNecromancer) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, 0xC00000C0);
    queuepoly(VHEAD, shHood, darkena(col, 1, 0xFF));
    }
  else if(m == moDraugr) {
    otherbodyparts(V, 0x483828D0, m, footphase);
    queuepoly(VBODY, shPBody, 0x483828D0);
    queuepoly(VBODY, shPSword, 0xFFFFD0A0);
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
    otherbodyparts(V, 0x080808D0, m, footphase);
    queuepoly(VBODY, shPBody, 0x080808D0);
    queuepoly(VHEAD, shPHead, 0x080808D0);
    queuepoly(VHEAD, shWightCloak, 0xFF0000A0);
    }
  else if(m == moGoblin) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBODY, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shArmor, darkena(col, 1, 0XFF));
    }
  else if(m == moLancer || m == moFlailer || m == moMiner) {
    transmatrix V2 = V;
    if(m == moLancer)
      V2 = V * spin((where && where->type == 6) ? -M_PI/3 : -M_PI/2 );
    transmatrix Vh = mmscale(V2, geom3::HEAD);
    transmatrix Vb = mmscale(V2, geom3::BODY);
    otherbodyparts(V2, darkena(col, 1, 0xFF), m, footphase);
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
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBODY, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(col, 1, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(col, 2, 0XFF));
    }        
  else if(m == moFjordTroll || m == moForestTroll || m == moStormTroll) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBODY, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(col, 1, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(col, 2, 0XFF));
    }        
  else if(m == moDarkTroll) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBODY, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(col, 1, 0XFF));
    queuepoly(VHEAD, shPFace, 0xFFFFFF80);
    }        
  else if(m == moRedTroll) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBODY, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(0xFF8000, 0, 0XFF));
    queuepoly(VHEAD, shPFace, 0xFFFFFF80);
    }        
  else if(m == moEarthElemental) {
    otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shWaterElemental);
    queuepoly(VBODY, shWaterElemental, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shFemaleHair, darkena(col, 0, 0XFF));
    queuepoly(VHEAD, shPFace, 0xF0000080);
    }        
  else if(m == moWaterElemental) {
    otherbodyparts(V, watercolor(50), m, footphase);
    ShadowV(V, shWaterElemental);
    queuepoly(VBODY, shWaterElemental, watercolor(0));
    queuepoly(VHEAD, shFemaleHair, watercolor(100));
    queuepoly(VHEAD, shPFace, watercolor(200));
    }        
  else if(m == moFireElemental) {
    otherbodyparts(V, darkena(firecolor(50), 0, 0xFF), m, footphase);
    ShadowV(V, shWaterElemental);
    queuepoly(VBODY, shWaterElemental, darkena(firecolor(0), 0, 0xFF));
    queuepoly(VHEAD, shFemaleHair, darkena(firecolor(100), 0, 0xFF));
    queuepoly(VHEAD, shPFace, darkena(firecolor(200), 0, 0xFF));
    }
  else if(m == moAirElemental) {
    otherbodyparts(V, darkena(col, 0, 0x40), m, footphase);
    ShadowV(V, shWaterElemental);
    queuepoly(VBODY, shWaterElemental, darkena(col, 0, 0x80));
    queuepoly(VHEAD, shFemaleHair, darkena(col, 0, 0x80));
    queuepoly(VHEAD, shPFace, darkena(col, 0, 0x80));
    }        
  else if((xch == 'd' || xch == 'D') && m != moDragonHead && m != moDragonTail) {
    otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    queuepoly(VBODY, shPBody, darkena(col, 1, 0xC0));
    ShadowV(V, shPBody);
    int acol = col;
    if(xch == 'D') acol = 0xD0D0D0;
    queuepoly(VHEAD, shDemon, darkena(acol, 0, 0xFF));
    }
  else if(isMagneticPole(m)) {
    if(m == moNorthPole)
      queuepolyat(VBODY * spin(M_PI), shTentacle, 0x000000C0, PPR_TENTACLE1);
    queuepolyat(VBODY, shDisk, darkena(col, 0, 0xFF), PPR_MONSTER_BODY);
    }
  else if(isMetalBeast(m)) {
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
    otherbodyparts(V, darkena(col, 2, 0xC0), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0XC0));
    queuepoly(VHEAD, shGolemhead, darkena(col, 1, 0XFF));
    }
  else if(isWitch(m)) {
    otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    int cc = 0xFF;
    if(m == moWitchGhost) cc = 0x85 + 120 * sin(ticks / 160.0);
    if(m == moWitchWinter && where) drawWinter(V, 0);
    if(m == moWitchFlash && where) drawFlash(V);
    if(m == moWitchSpeed && where) drawSpeed(V);
    if(m == moWitchFire) col = firecolor(0);
    ShadowV(V, shFemaleBody);
    queuepoly(VBODY, shFemaleBody, darkena(col, 0, cc));
//    queuepoly(cV2, ct, shPSword, darkena(col, 0, 0XFF));
//    queuepoly(V, shHood, darkena(col, 0, 0XC0));
    if(m == moWitchFire) col = firecolor(100);
    queuepoly(VHEAD, shWitchHair, darkena(col, 1, cc));
    if(m == moWitchFire) col = firecolor(200);
    queuepoly(VHEAD, shPFace, darkena(col, 0, cc));
    if(m == moWitchFire) col = firecolor(300);
    queuepoly(VBODY, shWitchDress, darkena(col, 1, 0XC0));
    }

  // just for the HUD glyphs...
  else if(isIvy(m) || isMutantIvy(m) || m == moFriendlyIvy) {
    queuepoly(V, shILeaf[0], darkena(col, 0, 0xFF));
    }
  else if(m == moWorm || m == moWormwait || m == moHexSnake) {
    queuepoly(V, shWormHead, darkena(col, 0, 0xFF));
    queuepolyat(V, shEyes, 0xFF, PPR_ONTENTACLE_EYES);
    }
  else if(m == moDragonHead) {
    queuepoly(V, shDragonHead, darkena(col, 0, 0xFF));
    queuepolyat(V, shEyes, 0xFF, PPR_ONTENTACLE_EYES);          
    int noscolor = 0xFF0000FF;
    queuepoly(V, shDragonNostril, noscolor);
    queuepoly(V * Mirror, shDragonNostril, noscolor);
    }
  else if(m == moDragonTail) {
    queuepoly(V, shDragonSegment, darkena(col, 0, 0xFF));
    }
  else if(m == moTentacle || m == moTentaclewait || m == moTentacleEscaping) {
    queuepoly(V, shTentHead, darkena(col, 0, 0xFF));
    ShadowV(V, shTentHead, PPR_GIANTSHADOW);
    }

  else return true;
  
  return false;
#else
  return true;
#endif
  }

bool drawMonsterTypeDH(eMonster m, cell *where, const transmatrix& V, int col, bool dh, ld footphase) {
  dynamicval<int> p(poly_outline, poly_outline);
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
  if(inmirrorcount)
    h = inverse(V) * Vwhere * inverse(gmatrix0[c]) * gmatrix0[c2] * h;
  else
    h = inverse(V) * gmatrix0[c2] * h;
  return atan2(h[1], h[0]);
  }

// equivalent to V = V * spin(-chainAngle(c,V,c2,dft));
bool chainAnimation(cell *c, transmatrix& V, cell *c2, int i, int b, const transmatrix &Vwhere, ld& length) {
  if(!gmatrix0.count(c2)) {
    V = V * ddspin(c,i,b);
    length = cellgfxdist(c,i);
    return false;
    }
  hyperpoint h = C0;
  if(animations[LAYER_BIG].count(c2)) h = animations[LAYER_BIG][c2].wherenow * h;
  if(inmirrorcount)
    h = inverse(V) * Vwhere * inverse(gmatrix0[c]) * gmatrix0[c2] * h;
  else
    h = inverse(V) * gmatrix0[c2] * h;
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
    s++; c = c->mov[c->mondir];
    }
  return s;
  }

int last_wormsegment = -1;
vector<vector< std::function<void()> > > wormsegments;

void add_segment(int d, const std::function<void()>& s) {
  if(size(wormsegments) <= d) wormsegments.resize(d+1);
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

bool drawMonster(const transmatrix& Vparam, int ct, cell *c, int col) {

  bool darkhistory = conformal::includeHistory && eq(c->aitmp, sval);
  
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
    
    if((m == moHexSnake || m == moHexSnakeTail) && c->land == laSnakeNest) {
      int d = c->mondir;
      if(d == NODIR)
        forCellIdEx(c2, i, c)
          if(among(c2->monst, moHexSnakeTail, moHexSnake) && c2->mondir == c->spin(i))
            d = i;
      if(d == NODIR) { d = hrand(c->type); createMov(c, d); }
      int c1 = nestcolors[pattern_threecolor(c)];
      int c2 = nestcolors[pattern_threecolor(c->mov[d])];
      col = (c1 + c2); // sum works because they are dark and should be brightened
      }

    if(isDragon(c->monst) && c->stuntime == 0) col = 0xFF6000;
    
    transmatrix Vb0 = Vb;
    if(c->mondir != NODIR) {
      
      if(mmmon) {
        ld length;
        // cell *c2 = c->mov[c->mondir];
        if(nospinb) 
          chainAnimation(c, Vb, c->mov[c->mondir], c->mondir, 0, Vparam, length);
        else {
          Vb = Vb * ddspin(c, c->mondir);
          length = cellgfxdist(c, c->mondir);
          }

        if(mapeditor::drawUserShape(Vb, 1, c->monst, (col << 8) + 0xFF, c)) return false;

        if(isIvy(c) || isMutantIvy(c) || c->monst == moFriendlyIvy)
          queuepoly(Vb, shIBranch, (col << 8) + 0xFF);
/*         else if(c->monst < moTentacle && wormstyle == 0) {
          ShadowV(Vb, shTentacleX, PPR_GIANTSHADOW);
          queuepoly(mmscale(Vb, geom3::ABODY), shTentacleX, 0xFF);
          queuepoly(mmscale(Vb, geom3::ABODY), shTentacle, (col << 8) + 0xFF);
          } */
//        else if(c->monst < moTentacle) {
//          }

        else if(c->monst == moDragonHead || c->monst == moDragonTail) {
          char part = dragon::bodypart(c, dragon::findhead(c));
          if(part != '2') {
            queuepoly(mmscale(Vb, geom3::ABODY), shDragonSegment, darkena(col, 0, 0xFF));
            ShadowV(Vb, shDragonSegment, PPR_GIANTSHADOW);
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
          ShadowV(Vb, shTentacleX, PPR_GIANTSHADOW);
          */
          bool hexsnake = c->monst == moHexSnake || c->monst == moHexSnakeTail;
          bool thead = c->monst == moTentacle || c->monst == moTentaclewait || c->monst == moTentacleEscaping;
          hpcshape& sh = hexsnake ? shWormSegment : shSmallWormSegment;
          ld wav = hexsnake ? 0 : 
            c->monst < moTentacle ? 1/1.5 :
            1;
          int col0 = col;
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
              ShadowV(Vbx, sh, PPR_GIANTSHADOW);
              queuepoly(mmscale(Vbx, geom3::ABODY), sh, (col0 << 8) + 0xFF);
              }
            });
          }
        }
        
      else {
        int hdir = displaydir(c, c->mondir);
        int col = darkena(0x606020, 0, 0xFF);
        for(int u=-1; u<=1; u++)
          queueline(Vparam*ddi0(hdir+S21, u*crossf/5), Vparam*ddi(hdir, crossf)*ddi0(hdir+S21, u*crossf/5), col, 2);
        }
      }

    if(mmmon) {
      if(isIvy(c) || isMutantIvy(c) || c->monst == moFriendlyIvy) {
        queuepoly(mmscale(Vb, geom3::ABODY), shILeaf[ctof(c)], darkena(col, 0, 0xFF));
        ShadowV(Vb, shILeaf[ctof(c)], PPR_GIANTSHADOW);
        }
      else if(m == moWorm || m == moWormwait || m == moHexSnake) {
        Vb = Vb * pispin;
        transmatrix Vbh = mmscale(Vb, geom3::AHEAD);
        queuepoly(Vbh, shWormHead, darkena(col, 0, 0xFF));
        queuepolyat(Vbh, shEyes, 0xFF, PPR_ONTENTACLE_EYES);
        ShadowV(Vb, shWormHead, PPR_GIANTSHADOW);
        }
      else if(m == moDragonHead) {
        transmatrix Vbh = mmscale(Vb, geom3::AHEAD);
        ShadowV(Vb, shDragonHead, PPR_GIANTSHADOW);
        queuepoly(Vbh, shDragonHead, darkena(col, c->hitpoints?0:1, 0xFF));
        queuepolyat(Vbh/* * pispin */, shEyes, 0xFF, PPR_ONTENTACLE_EYES);
        
        int noscolor = (c->hitpoints == 1 && c->stuntime ==1) ? 0xFF0000FF : 0xFF;
        queuepoly(Vbh, shDragonNostril, noscolor);
        queuepoly(Vbh * Mirror, shDragonNostril, noscolor);
        }
      else if(m == moTentacle || m == moTentaclewait || m == moTentacleEscaping) {
        Vb = Vb * pispin;
        transmatrix Vbh = mmscale(Vb, geom3::AHEAD);
        queuepoly(Vbh, shTentHead, darkena(col, 0, 0xFF));
        ShadowV(Vb, shTentHead, PPR_GIANTSHADOW);
        }
      else if(m == moDragonTail) {
        cell *c2 = NULL;
        for(int i=0; i<c->type; i++)
          if(c->mov[i] && isDragon(c->mov[i]->monst) && c->mov[i]->mondir == c->spn(i))
            c2 = c->mov[i];
        int nd = neighborId(c, c2);
        char part = dragon::bodypart(c, dragon::findhead(c));
        if(part == 't') {
          if(nospinb) {
            ld length;
            chainAnimation(c, Vb, c2, nd, 0, Vparam, length);
            Vb = Vb * pispin;
            }
          else {
            Vb = Vb0 * ddspin(c, nd, S42);
            }
          transmatrix Vbb = mmscale(Vb, geom3::ABODY);
          queuepoly(Vbb, shDragonTail, darkena(col, c->hitpoints?0:1, 0xFF));
          ShadowV(Vb, shDragonTail, PPR_GIANTSHADOW);
          }
        else if(true) {
          if(nospinb) {
            ld length;
            chainAnimation(c, Vb, c2, nd, 0, Vparam, length);
            Vb = Vb * pispin;
            double ang = chainAngle(c, Vb, c->mov[c->mondir], (displaydir(c, c->mondir) - displaydir(c, nd)) * M_PI / S42, Vparam);
            ang /= 2;
            Vb = Vb * spin(M_PI-ang);
            }
          else {
            int hdir0 = displaydir(c, nd) + S42;
            int hdir1 = displaydir(c, c->mondir);
            while(hdir1 > hdir0 + S42) hdir1 -= S84;
            while(hdir1 < hdir0 - S42) hdir1 += S84;
            Vb = Vb0 * spin((hdir0 + hdir1)/2 * M_PI / S42 + M_PI);
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
            if(c->mov[i] && isWorm(c->mov[i]->monst) && c->mov[i]->mondir == c->spn(i))
              c2 = c->mov[i];
          int nd = neighborId(c, c2);
          if(nospinb) {
            ld length;
            chainAnimation(c, Vb, c2, nd, 0, Vparam, length);
            Vb = Vb * pispin;
            }
          else {
            Vb = Vb0 * ddspin(c, nd, S42);
            }
          transmatrix Vbb = mmscale(Vb, geom3::ABODY) * pispin;
          hpcshape& sh = hexsnake ? shWormTail : shSmallWormTail;
          queuepoly(Vbb, sh, darkena(col, 0, 0xFF));
          ShadowV(Vb, sh, PPR_GIANTSHADOW);
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
    for(auto& m: mirror::mirrors) if(c == m.second.c) 
    for(int d=0; d<copies; d++) {
      multi::cpid = m.first;
      auto cw = m.second;
      if(d&1) cwmirrorat(cw, xdir);
      if(d>=2) cw += 2;
      if(d>=4) cw += 2;
      transmatrix Vs = Vparam;
      bool mirr = cw.mirrored;
      Vs = Vs * ddspin(c, cw.spin-cwt.spin, euclid ? 0 : S42);
      nospins = applyAnimation(cwt.c, Vs, footphase, LAYER_SMALL);
      if(!nospins) Vs = Vs * ddspin(c, cwt.spin);
      if(mirr) Vs = Vs * Mirror;
      if(inmirrorcount&1) mirr = !mirr;
      col = mirrorcolor(geometry == gElliptic ? det(Vs) < 0 : mirr);
      if(!nospins && flipplayer) Vs = Vs * pispin;
      if(mmmon) {
        drawMonsterType(moMimic, c, Vs, col, footphase);
        drawPlayerEffects(Vs, c, false);
        }
      if(!mouseout() && !nospins) {
        transmatrix invxy = Id;
        if(flipplayer) invxy[0][0] = invxy[1][1] = -1;
        
        hyperpoint P2 = Vs * inverse(cwtV) * invxy * mouseh;
        queuechr(P2, 10, 'x', 0xFF00);
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
      for(int i=0; i<c->type; i++) if(c->mov[i] && HEAT(c->mov[i]) > bheat) {
        bheat = HEAT(c->mov[i]);
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
      chainAnimation(c, Vb, c->mov[c->mondir], c->mondir, 0, Vparam, length);
      Vb = Vb * pispin;
      }
    else Vb = Vb * ddspin(c, c->mondir, S42);
    if(gp::on)  Vb = Vb * xpush(crossf * .6);
    else {
      if(weirdhyperbolic || sphere) Vb = Vb * xpush(-(hexhexdist - hcrossf7));
      if(ctof(c) && !euclid) Vb = Vb * xpush(hexhexdist - hcrossf);
      }
    return drawMonsterTypeDH(m, c, Vb, col, darkhistory, footphase);
    }

  // golems, knights, and hyperbugs don't face the player (mondir-controlled)
  // also whatever in the lineview mode, and whatever in the quotient geometry

  else if(isFriendly(c) || isBug(c) || (c->monst && conformal::on) || c->monst == moKrakenH || (isBull(c->monst) && c->mondir != NODIR) || c->monst == moButterfly || isMagneticPole(c->monst) ||
    isSwitch(c->monst) || c->monst == moPair || (c->monst && (quotient || torus || dont_face_pc))) {
    if(c->monst == moKrakenH) Vs = Vb, nospins = nospinb;
    if(!nospins) Vs = Vs * ddspin(c, c->mondir, S42);
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
  }

double downspin;
cell *straightDownSeek;

#define AURA 180

array<array<int,4>,AURA+1> aurac;

bool haveaura() {
  if(!(vid.aurastr>0 && !svg::in && (auraNOGL || vid.usingGL))) return false;
  if(sphere && mdAzimuthalEqui()) return true;
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

void addauraspecial(const hyperpoint& h, int col, int dir) {
  if(!haveaura()) return;
  int r = int(2*AURA + dir + atan2(h[0], h[1]) * AURA / 2 / M_PI) % AURA; 
  auraspecials.emplace_back(r, col);
  }

void addaura(const hyperpoint& h, int col, int fd) {
  if(!haveaura()) return;
  int r = int(2*AURA + atan2(h[0], h[1]) * AURA / 2 / M_PI) % AURA; 
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
  if(stereo::mode) return;
  double rad = vid.radius;
  if(sphere && !mdAzimuthalEqui()) rad /= sqrt(vid.alpha*vid.alpha - 1);
  
  for(int v=0; v<4; v++) sumaura(v);
  for(auto& p: auraspecials) {
    int r = p.first;
    aurac[r][3] = auramemo;
    for(int k=0; k<3; k++) aurac[r][k] = (p.second >> (8*k)) & 255;
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

      ld hx = (x * 1. - vid.xcenter) / rad;
      ld hy = (y * 1. - vid.ycenter) / rad;
  
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
      
      int& p = qpixel(s, x, y);
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

  for(int r=0; r<=AURA; r++) for(int z=0; z<11; z++) {
    float rr = (M_PI * 2 * r) / AURA;
    float rad0 = vid.alpha > -1 ? rad * facs[z] : rad / facs[z];
    int rm = r % AURA;
    cx[r][z][0] = rad0 * sin(rr);
    cx[r][z][1] = rad0 * cos(rr);
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
  glhr::switch_mode(glhr::gmVarColored);
  glhr::id_modelview();
  glhr::prepare(auravertices);
  glhr::set_depthtest(false);
  glDrawArrays(GL_TRIANGLES, 0, size(auravertices));


  setcameraangle(false);
#endif
  }

// int fnt[100][7];

bool bugsNearby(cell *c, int dist = 2) {
  if(!(havewhat&HF_BUG)) return false;
  if(isBug(c)) return true;
  if(dist) for(int t=0; t<c->type; t++) if(c->mov[t] && bugsNearby(c->mov[t], dist-1)) return true;
  return false;
  }

int minecolors[8] = {
  0xFFFFFF, 0xF0, 0xF060, 0xF00000, 
  0x60, 0x600000, 0x00C0C0, 0x000000
  };

int distcolors[8] = {
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
    if(c->mov[i] && c->mov[i]->wall == waMineMine)
      mines++;
  return mines;
  }

transmatrix applyPatterndir(cell *c, const patterns::patterninfo& si) {
  transmatrix V = ddspin(c, si.dir, S42);  
  if(si.reflect) return V * Mirror;  
  return V;
  }

transmatrix applyDowndir(cell *c, cellfunction *cf) {
  return ddspin(c, patterns::downdir(c, cf), S42);
  }

void drawTowerFloor(const transmatrix& V, cell *c, int col, cellfunction *cf = coastvalEdge) {
  if(weirdhyperbolic || sphere) {
    int ct6 = ctof(c);
    qfloor(c, V, PLAINFLOOR, col); return;
    }
  int j = -1;

  if(euclid) j = 10;
  else if((*cf)(c) > 1) { 
    int i = towerval(c, cf);
    if(i == 4) j = 0;
    if(i == 5) j = 1;
    if(i == 6) j = 2;
    if(i == 8) j = 3;
    if(i == 9) j = 4;
    if(i == 10) j = 5;
    if(i == 13) j = 6;
    if(nonbitrunc) {
      if(i == 7) j = 7;
      if(i == 11) j = 8;
      if(i == 15) j = 9;
      }
    }

  if(j >= 0)
    qfloor(c, V, applyDowndir(c, cf), shTower[j], col);
  else if(c->wall != waLadder)
    qfloor(c, V, shMFloor[ctof(c)], col);
  }

void drawZebraFloor(const transmatrix& V, cell *c, int col) {

  if(euclid) { qfloor(c, V, shTower[10], col); return; }
  if(weirdhyperbolic) {
    int ct6 = ctof(c);
    qfloor(c, V, PLAINFLOOR, col); return;
    }
  
  auto si = patterns::getpatterninfo(c, 'z', patterns::SPF_SYM0123);
  
  int j;
  if(nonbitrunc) j = 4;
  else if(si.id >=4 && si.id < 16) j = 2;
  else if(si.id >= 16 && si.id < 28) j = 1;
  else if(si.id >= 28 && si.id < 40) j = 3;
  else j = 0;

  qfloor(c, V, applyPatterndir(c, si), shZebra[j], col);
  }

void qplainfloor(cell *c, bool warp, const transmatrix &V, int col);

void drawReptileFloor(const transmatrix& V, cell *c, int col, bool usefloor) {

  auto si = 
    euclid6 ? 
      patterns::getpatterninfo(c, patterns::PAT_COLORING, patterns::SPF_CHANGEROT)
    :
      patterns::getpatterninfo(c, patterns::PAT_ZEBRA, patterns::SPF_SYM0123);
  
  int j;

  if(!wmescher) j = 4;
  else if(nonbitrunc) j = 0;
  else if(si.id < 4) j = 0;
  else if(si.id >=4 && si.id < 16) j = 1;
  else if(si.id >= 16 && si.id < 28) j = 2;
  else if(si.id >= 28 && si.id < 40) j = 3;
  else j = 4;
  
  if(euclid6) j = 0;

  transmatrix D = applyPatterndir(c, si);
  transmatrix V2 = V * D;
  
  if(wmescher) {
    if(usefloor)
      qfloor(c, V, D, shReptile[j][0], darkena(col, 0, 0xFF));
    else
      queuepoly(V2, shReptile[j][0], darkena(col, 0, 0xFF));
    }
  else 
    qplainfloor(c, isWarped(c), V, darkena(col, 0, 0xFF));

  if(usefloor && chasmg == 2) return;

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
    if(ecol) ecol = gradient(0, ecol, -1, sin(M_PI / 100 * ticks), 1);
    }
  
  if(ecol == -1 || ecol == 0) dcol = darkena(col, 1, 0xFF);
  else dcol = darkena(ecol, 0, 0x80);

  dynamicval<int> p(poly_outline, 
    doHighlight() && ecol != -1 && ecol != 0 ? OUTLINE_ENEMY : OUTLINE_DEFAULT);

  if(!chasmg) {
    if(wmescher)
      queuepoly(V2, shReptile[j][1], dcol);
    else
      queuepoly(V2, shMFloor[ctof(c)], dcol);
    }
  
  if(ecol != -1) {
    queuepoly(V2, shReptile[j][2], (ecol << 8) + 0xFF);
    queuepoly(V2, shReptile[j][3], (ecol << 8) + 0xFF);
    }
  }

void drawEmeraldFloor(const transmatrix& V, cell *c, int col) {
  if(!euclid && !nonbitrunc) {
    auto si = patterns::getpatterninfo(c, 'f', patterns::SPF_SYM0123);
  
    int j = -1;

    if(si.id == 8) j = 0;
    else if(si.id == 12) j = 1;
    else if(si.id == 16) j = 2;
    else if(si.id == 20) j = 3;
    else if(si.id == 28) j = 4;
    else if(si.id == 36) j = 5;

    if(j >= 0) {
      qfloor(c, V, applyPatterndir(c, si), shEmeraldFloor[j], col);
      return;
      }
    }
  
  int ct6 = ctof(c);
  int xct6 = ct6;
  if(gp::on && has_nice_dual() && pseudohept(c) && !ishept(c)) xct6 = 2;

  qfloor(c, V, CAVEFLOOR, col);
  }

double fanframe;

void viewBuggyCells(cell *c, transmatrix V) {
  for(int i=0; i<size(buggycells); i++)
    if(c == buggycells[i]) {
      queuepoly(V, shPirateX, 0xC000C080);
      return;
      }

  for(int i=0; i<size(buggycells); i++) {
    cell *c1 = buggycells[i];
    cell *cf = cwt.c;
    
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
    if(xc.c == c) {
      transmatrix fixrot = rgpushxto0(tC0(V));
      // make it more transparent
      int col = getcs().uicolor;
      col -= (col & 0xFF) >> 1;
      poly_outline = OUTLINE_DEFAULT;
      queuepoly(fixrot * spin(-d * M_PI/4 + (sphereflipped()?M_PI:0))/* * eupush(1,0)*/, shArrow, col);

      if((c->type & 1) && (isStunnable(c->monst) || c->wall == waThumperOn)) {
        transmatrix Centered = rgpushxto0(tC0(cwtV));
        int sd = md.subdir;
        queuepoly(inverse(Centered) * rgpushxto0(Centered * tC0(V)) * rspintox(Centered*tC0(V)) * spin(-sd * M_PI/S7) * xpush(0.2), shArrow, col);
        }
      else break;
      }
    }
  }

int celldistAltPlus(cell *c) { return 1000000 + celldistAlt(c); }

bool drawstaratvec(double dx, double dy) {
  return dx*dx+dy*dy > .05;
  }

int reptilecolor(cell *c) {
  int i = zebra40(c);
  
  if(!euclid) {
    if(i >= 4 && i < 16) i = 0; 
    else if(i >= 16 && i < 28) i = 1;
    else if(i >= 28 && i < 40) i = 2;
    else i = 3;
    }

  int fcoltab[4] = {0xe3bb97, 0xc2d1b0, 0xebe5cb, 0xA0A0A0};
  return fcoltab[i];
  }

ld wavefun(ld x) { 
  return sin(x);
  /* x /= (2*M_PI);
  x -= (int) x;
  if(x > .5) return (x-.5) * 2;
  else return 0; */
  }

const unsigned int nestcolors[8] = { 0x800000, 0x008000, 0x000080, 0x404040, 0x700070, 0x007070, 0x707000, 0x606060 };

void setcolors(cell *c, int& wcol, int &fcol) {

  wcol = fcol = winf[c->wall].color;

  // water colors
  if(isWateryOrBoat(c) || c->wall == waReptileBridge) {
    if(c->land == laOcean)
      fcol = (c->landparam > 25 && !chaosmode) ? ( 
        0x90 + 8 * sin(windmap::windcodes[windmap::getId(c)] * M_PI / 128 - ticks/1000.)
        ) : 
        0x1010C0 + int(32 * sin(ticks / 500. + (chaosmode ? c->CHAOSPARAM : c->landparam)*1.5));
    else if(c->land == laOceanWall)
      fcol = 0x2020FF;
    else if(c->land == laKraken) {
      fcol = 0x0000A0;
      int mafcol = (pseudohept(c) ? 64 : 8);
      /* bool nearshore = false;
      for(int i=0; i<c->type; i++) 
        if(c->mov[i]->wall != waSea && c->mov[i]->wall != waBoat)
          nearshore = true;
      if(nearshore) mafcol += 30; */
      fcol = fcol + mafcol * (4+sin(ticks / 500. + ((euclid||c->master->alt) ? celldistAlt(c) : 0)*1.5))/5;
      }
    else if(c->land == laDocks) {
      fcol = 0x0000A0;
      }
    else if(c->land == laAlchemist)
      fcol = 0x900090;
    else if(c->land == laWhirlpool)
      fcol = 0x0000C0 + int(32 * sin(ticks / 200. + ((euclid||c->master->alt) ? celldistAlt(c) : 0)*1.5));
    else if(c->land == laLivefjord)
      fcol = 0x000080;
    else if(isWarped(c->land))
      fcol = 0x0000C0 + int((warptype(c)?30:-30) * sin(ticks / 600.));
    else
      fcol = wcol;
    }

  // floor colors for all the lands
  else switch(c->land) {
    case laBurial: case laTrollheim: case laBarrier: case laOceanWall:
    case laCrossroads2: case laCrossroads3: case laCrossroads4: case laCrossroads5:
    case laRose: case laPower: case laWildWest: case laHalloween: case laRedRock:
    case laDragon: case laStorms: case laTerracotta: case laMercuryRiver:
      fcol = linf[c->land].color; break;
    
    case laRuins:
      fcol = pseudohept(c) ? 0xC0E0C0 : 0x40A040;
      break;
    
    case laDual:
      fcol = linf[c->land].color;
      if(c->landparam == 2) fcol = 0x40FF00;
      if(c->landparam == 3) fcol = 0xC0FF00;
      break;

    case laDesert: fcol = 0xEDC9AF; break;
    case laKraken: fcol = 0x20A020; break;
    case laDocks: fcol = 0x202020; break;
    case laCA: fcol = 0x404040; break;
    case laMotion: fcol = 0xF0F000; break;
    case laGraveyard: fcol = 0x107010; break;
    case laWineyard: fcol = 0x006000; break;
    case laLivefjord: fcol = 0x306030; break;
    
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

    case laMinefield: 
      fcol = 0x80A080; 
      if(c->wall == waMineMine && ((cmode & sm::MAP) || !canmove))
        fcol = wcol = 0xFF4040;
      break;
      
    case laCaribbean: 
      if(c->wall != waCIsland && c->wall != waCIsland2)
        fcol = 0x006000;
      break;

    case laAlchemist: 
      fcol = 0x202020;
      break;
    case laReptile:
      fcol = reptilecolor(c);
      break;
    case laCrossroads:
      fcol = (stereo::in_anaglyph() ? 0xFF3030 : 0xFF0000);
      break;
    case laCaves: case laEmerald: case laDeadCaves:
      fcol = 0x202020;
      if(c->land == laEmerald) 
      if(c->wall == waCavefloor || c->wall == waCavewall) {
        fcol = wcol = gradient(winf[waCavefloor].color, 0xFF00, 0, 0.5, 1);
        if(c->wall == waCavewall) wcol = 0xC0FFC0;
        }
      break;
    case laJungle:
      fcol = (stereo::in_anaglyph() ? 0x408040 : 0x008000);
      break;
    case laMirror: case laMirrorWall: case laMirrorOld:
      fcol = 0x808080;
      break;
    case laDryForest:
      fcol = gradient(0x008000, 0x800000, 0, c->landparam, 10);
      break;    
    case laMountain:
      if(euclid || sphere || c->master->alt)
        fcol = celldistAlt(c) & 1 ? 0x604020 : 0x302010;
      else fcol = 0;
      if(c->wall == waPlatform) wcol = 0xF0F0A0;
      break;
    case laRlyeh:
      fcol = (stereo::in_anaglyph() ? 0x4080C0 : 0x004080);
      break;
    case laHell:
      fcol = (stereo::in_anaglyph() ? 0xC03030 : 0xC00000);
      break;
    case laCanvas:
      fcol = c->landparam;
      break;
    case laPalace:
      fcol = 0x806020;
      if(c->wall == waClosedGate || c->wall == waOpenGate)
        fcol = wcol;
      break;
    case laElementalWall:
      fcol = (linf[c->barleft].color>>1) + (linf[c->barright].color>>1);
      break;
    case laZebra:
      fcol = 0xE0E0E0;
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
      fcol = (c->land == laOvergrown/* || (celldistAlt(c)&1)*/) ? 0x00C020 : 0x60E080;
      if(c->wall == waSmallTree) wcol = 0x008060;
      else if(c->wall == waBigTree) wcol = 0x0080C0;
      break;
    case laTemple: {
      int d = showoff ? 0 : (euclid||c->master->alt) ? celldistAlt(c) : 99;
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
        int wcol[4] = {0x404040, 0x404080, 0x2050A0, 0x5050C0};
        fcol = wcol[whirlwind::fzebra3(c)];
        }
      break;

    case laHunting:
      // fcol = pseudohept(c) ? 0x205050 : 0x306060;
      fcol = 0x40E0D0;
      fcol /= 2;
      if(pseudohept(c)) fcol = fcol * 3/4;
      break;
  
    case laIvoryTower:
      fcol = 0x10101 * (32 + (c->landparam&1) * 32) - 0x000010;
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
  
    case laCamelot: {
      int d = showoff ? 0 : ((euclid||c->master->alt) ? celldistAltRelative(c) : 0);
  #if CAP_TOUR
      if(!tour::on) camelotcheat = false;
      if(camelotcheat) 
          fcol = (d&1) ? 0xC0C0C0 : 0x606060;
      else 
  #endif
      if(d < 0) {
        fcol = 0xA0A0A0;
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
        bool showcoc = c->land == laCocytus && chaosmode && !wmescher;
        
        int colorN04 = showcoc ? 0x4080FF : 0x4040FF;
        int colorN10 = 0x0000FF;
        int color0 = c->land == laBlizzard ? 0x5050C0 : showcoc ? 0x80C0FF : 0x8080FF;
        int color02 = 0xFFFFFF;
        int color06 = 0xFF0000;
        int color08 = 0xFFFF00;
        
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
        for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->item)
          itcolor = 1;
        if(c->item) itcolor |= 2;
        fcol = 0x609F60 + 0x202020 * itcolor;
    
        forCellEx(c2, c) if(c2->monst == moFriendlyGhost)
          fcol = gradient(fcol, fghostcolor(ticks, c2), 0, .25, 1);
    
        if(c->monst == moFriendlyGhost) 
          fcol = gradient(fcol, fghostcolor(ticks, c), 0, .5, 1);    
    
        if(c->wall == waSmallTree) wcol = 0x004000;
        else if(c->wall == waBigTree) wcol = 0x008000;
        }
    }
  
   /* if(c->land == laCaribbean && (c->wall == waCIsland || c->wall == waCIsland2))
     fcol = wcol = winf[c->wall].color; */
   
  switch(c->wall) {
    case waSulphur: case waSulphurC: case waPlatform: case waMercury: case waDock:
    case waAncientGrave: case waFreshGrave: case waThumperOn: case waThumperOff: case waBonfireOff:
    case waRoundTable:
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
      if(c->item && !(conformal::includeHistory && eq(c->aitmp, sval)))
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
        fcol = wcol = gradient(wcol, 0xFF4040, -1, sin(ticks/100.0), 1);
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
  
  if(items[itRevolver] && c->pathdist > GUNRANGE && !shmup::on)
    fcol = gradient(fcol, 0, 0, 25, 100),
    wcol = gradient(wcol, 0, 0, 25, 100);
    
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
  if(!nonbitrunc) return true;
  if(!gp::on) return false;
  return (gp::param.first + gp::param.second * 2) % 3 == 0;
  }

// does the current geometry allow nice duals
bool is_nice_dual(cell *c) {
  return c->land == laDual && has_nice_dual();
  }

bool use_swapped_duals() {
  return (euclid && !a4) || gp::on;
  }

void floorShadow(cell *c, const transmatrix& V, int col, bool warp) {
  if(pmodel == mdHyperboloid || pmodel == mdBall || pmodel == mdHemisphere) 
    return; // shadows break the depth testing
  if(shmup::on || nbtnice) warp = false;
  dynamicval<int> p(poly_outline, OUTLINE_TRANS);
  if(wmescher && qfi.special) {
    queuepolyat(V * qfi.spin * shadowmulmatrix, *qfi.shape, col, PPR_WALLSHADOW);
    }
  else if(warp) {
    if(euclid) {
      if(ishex1(c))
        queuepolyat(V * pispin, shTriheptaFloorShadow[0], col, PPR_WALLSHADOW);
      else
        queuepolyat(V, shTriheptaFloorShadow[ctof(c)], col, PPR_WALLSHADOW);
      }
    else {
      auto si = patterns::getpatterninfo(c, 0, 0);
      queuepolyat(V * applyPatterndir(c, si), shTriheptaFloorShadow[si.id == 13 ? 2 : ctof(c)], col, PPR_WALLSHADOW);
      }
    }
  else if(is_nice_dual(c)) {
    if(use_swapped_duals() && ishex1(c))
      queuepolyat(V * pispin, shBigTriShadow, col, PPR_WALLSHADOW);
    else
      queuepolyat(V, shBigTriShadow, col, PPR_WALLSHADOW);
    }    
  else {
    queuepolyat(V, shFloorShadow[ctof(c)], col, PPR_WALLSHADOW);
    }
  }

void plainfloor(cell *c, bool warp, const transmatrix &V, int col, int prio) {
  if(warp) {
    if(euclid) {
      if(ishex1(c))
        queuepolyat(V * pispin, shTriheptaFloor[ctof(c)], col, prio);        
      else
        queuepolyat(V, shTriheptaFloor[ctof(c)], col, prio);
      }
    else {
      auto si = patterns::getpatterninfo(c, 0, 0);
      queuepolyat(V * applyPatterndir(c, si), shTriheptaFloor[/*sphere ? ctof(c) :*/ si.id], col, prio);
      }
    }
  else if(is_nice_dual(c)) {
    if(use_swapped_duals() && ishex1(c))
      queuepolyat(V * pispin, shBigTriangle, col, prio);
    else
      queuepolyat(V, shBigTriangle, col, prio);
    }    
  else {
    queuepolyat(V, shFloor[ctof(c)], col, prio);
    }
  }

void qfloor_eswap(cell *c, const transmatrix& V, const hpcshape& sh, int col);

void qplainfloor(cell *c, bool warp, const transmatrix &V, int col) {
  if(warp) {
    auto si = patterns::getpatterninfo(c, 0, 0);
    qfloor(c, V, applyPatterndir(c, si), shTriheptaFloor[si.id], col);
    }
  else if(is_nice_dual(c))
    qfloor_eswap(c, V, shBigTriangle, col);
  else {
    qfloor(c, V, shFloor[ctof(c)], col);
    }
  }

int wavephase;

void warpfloor(cell *c, const transmatrix& V, int col, int prio, bool warp) {
  if(shmup::on || nbtnice) warp = false;
#if CAP_TEXTURE
  if(qfi.tinf) {
    queuetable(V*qfi.spin, qfi.tinf->vertices, size(qfi.tinf->vertices), 0, texture::config.recolor(col), prio);
    lastptd().u.poly.tinf = qfi.tinf;
    }
  else 
#endif
  if(wmescher && qfi.special)
    queuepolyat(V*qfi.spin, *qfi.shape, col, prio);
  else plainfloor(c, warp, V, col, prio);
  }

#define placeSidewallX(a,b,c,d,e,f,g) \
  { if((wmescher && qfi.special) || !validsidepar[c]) { \
    escherSidewall(a,c,d,g); break; } \
    else placeSidewall(a,b,c,d,e,f,g); }
#define placeSidewallXB(a,b,c,d,e,f,g, Break) \
  { if((wmescher && qfi.shape) || !validsidepar[c]) { \
    escherSidewall(a,c,d,g); Break; break; } \
    else placeSidewall(a,b,c,d,e,f,g); }

void escherSidewall(cell *c, int sidepar, const transmatrix& V, int col) {
  if(sidepar >= SIDE_SLEV && sidepar <= SIDE_SLEV+2) {
    int sl = sidepar - SIDE_SLEV;
    for(int z=1; z<=4; z++) if(z == 1 || (z == 4 && detaillevel == 2))
      warpfloor(c, mscale(V, zgrad0(geom3::slev * sl, geom3::slev * (sl+1), z, 4)), col, PPR_REDWALL-4+z+4*sl, false);
    }
  else if(sidepar == SIDE_WALL) {
    const int layers = 2 << detaillevel;
    for(int z=1; z<layers; z++) 
      warpfloor(c, mscale(V, zgrad0(0, geom3::wall_height, z, layers)), col, PPR_WALL3+z-layers, false);
    }
  else if(sidepar == SIDE_LAKE) {
    const int layers = 1 << (detaillevel-1);
    if(detaillevel) for(int z=0; z<layers; z++) 
      warpfloor(c, mscale(V, zgrad0(-geom3::lake_top, 0, z, layers)), col, PPR_FLOOR+z-layers, false);
    }
  else if(sidepar == SIDE_LTOB) {
    const int layers = 1 << (detaillevel-1);
    if(detaillevel) for(int z=0; z<layers; z++) 
      warpfloor(c, mscale(V, zgrad0(-geom3::lake_bottom, -geom3::lake_top, z, layers)), col, PPR_INLAKEWALL+z-layers, false);
    }
  else if(sidepar == SIDE_BTOI) {
    const int layers = 1 << detaillevel;
    warpfloor(c, mscale(V, geom3::INFDEEP), col, PPR_MINUSINF, false);
    for(int z=1; z<layers; z++) 
      warpfloor(c, mscale(V, zgrad0(-geom3::lake_bottom, -geom3::lake_top, -z, 1)), col, PPR_LAKEBOTTOM+z-layers, false);
    }
  }

void placeSidewall(cell *c, int i, int sidepar, const transmatrix& V, bool warp, bool mirr, int col) {
  if(shmup::on || nbtnice) warp = false;
  if(warp && !pseudohept(c) && (!c->mov[i] || !pseudohept(c->mov[i]))) return;
  if(is_nice_dual(c)) {
    if(pseudohept(c)) return;
    bool b = !(i&1);
    if(use_swapped_duals()) {
      if(!ishex1(c)) b = !b;
      if(gp::on) b = !b;
      }
    if(b) return;
    }
  int prio;
  /* if(mirr) prio = PPR_GLASS - 2;
  else */ if(sidepar == SIDE_WALL) prio = PPR_WALL3 - 2;
  else if(sidepar == SIDE_WTS3) prio = PPR_WALL3 - 2;
  else if(sidepar == SIDE_LAKE) prio = PPR_LAKEWALL;
  else if(sidepar == SIDE_LTOB) prio = PPR_INLAKEWALL;
  else if(sidepar == SIDE_BTOI) prio = PPR_BELOWBOTTOM;
  else prio = PPR_REDWALL-2+4*(sidepar-SIDE_SLEV);
  
  transmatrix V2 = V * ddspin(c, i);
  
  // if(sphere && vid.alpha <= 1 && tC0(V2 * xpush(cellgfxdist(c, i)/2))[2] < -.5) return; 
 
  /* int aw = away(V2); prio += aw;
  if(!detaillevel && aw < 0) return;
  */
  
  // queuepoly(V2 * xpush(.1), shSnowball, aw ? 0xFFFFFFFF : 0xFF0000FF);
  // prio += c->cpdist - c->mov[i]->cpdist;  
  
  queuepolyat(V2, 
    (qfi.tinf?shFullFloorSide:mirr?shMFloorSide:warp?(pseudohept(c)&&!ishept(c)?shTriheptaSideGP:shTriheptaSide):is_nice_dual(c)?shBigTriSide:shFloorSide)[sidepar][ctof(c)], col, prio);
  }

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

void pushdown(cell *c, int& q, const transmatrix &V, double down, bool rezoom, bool repriority) {

  // since we might be changing priorities, we have to make sure that we are sorting correctly
  if(down > 0 && repriority) { 
    int qq = q+1;
    while(qq < size(ptds))
      if(qq > q && ptds[qq].prio < ptds[qq-1].prio) {
        swap(ptds[qq], ptds[qq-1]);
        qq--;
        }
      else qq++;
    }
  
  while(q < size(ptds)) {
    polytodraw& ptd = ptds[q++];
    if(ptd.kind == pkPoly) {
    
      double z2;
      
      double z = zlevel(tC0(ptd.u.poly.V));
      double lev = geom3::factor_to_lev(z);
      double nlev = lev - down;
      
      double xyscale = rezoom ? geom3::scale_at_lev(lev) / geom3::scale_at_lev(nlev) : 1;
      z2 = geom3::lev_to_factor(nlev);
      double zscale = z2 / z;
      
      // xyscale = xyscale + (zscale-xyscale) * (1+sin(ticks / 1000.0)) / 2;
      
      ptd.u.poly.V = xyzscale( V, xyscale*zscale, zscale)
        * inverse(V) * ptd.u.poly.V;
        
      if(!repriority) ;
      else if(nlev < -geom3::lake_bottom-1e-3) {
        ptd.prio = PPR_BELOWBOTTOM_FALLANIM;
        if(c->wall != waChasm)
          ptd.col = 0; // disappear!
        }
      else if(nlev < -geom3::lake_top-1e-3)
        ptd.prio = PPR_INLAKEWALL_FALLANIM;
      else if(nlev < 0)
        ptd.prio = PPR_LAKEWALL_FALLANIM;
      }      
    }
  }

bool dodrawcell(cell *c) {
  // do not display out of range cells, unless on torus
  if(c->pathdist == PINFD && geometry != gTorus)
    return false;
  // do not display not fully generated cells, unless a cheater
  if(c->mpdist > 7 && !cheater) return false;
  // in the Yendor Challenge, scrolling back is forbidden
  if(c->cpdist > 7 && yendor::on && !cheater) return false;

  return true;
  }

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

bool viewdists = false;

bool allemptynear(cell *c) {
  if(c->wall) return false;
  forCellEx(c2, c) if(c2->wall) return false;
  return true;
  }

#include "blizzard.cpp"

static const int trapcol[4] = {0x904040, 0xA02020, 0xD00000, 0x303030};
static const int terracol[8] = {0xD000, 0xE25050, 0xD0D0D0, 0x606060, 0x303030, 0x181818, 0x0080, 0x8080};

void qfloor_eswap(cell *c, const transmatrix& V, const hpcshape& sh, int col) {
  if((euclid || gp::on) && ishex1(c)) 
    qfloor(c, V, pispin, sh, col);
  else
    qfloor(c, V, sh, col);
  };

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
      return 1;
    
    case laTrollheim:
    default:
      return 2;
    }    
  }

int getSnakelevColor(cell *c, int i, int last, int fd, int wcol) {
  int col;
  if(c->wall == waTower) 
    col = 0xD0D0D0-i*0x101010;
  else if(c->land == laSnakeNest)
    return darkena(nestcolors[pattern_threecolor(c)] * (5 + i) / 4, 0, 0xFF);
  else if(i == last-1)
    col = wcol;
  else
    col = winf[waRed1+i].color;
  return darkena(col, fd, 0xFF);
  }

void draw_wall(cell *c, const transmatrix& V, int wcol, int& zcol, int ct6, int fd) {
  zcol = wcol;
  int wcol0 = wcol;
  int starcol = wcol;        
  if(c->wall == waWarpGate) starcol = 0;
  if(c->wall == waVinePlant) starcol = 0x60C000;

  int wcol2 = gradient(0, wcol0, 0, .8, 1);

  if(c->wall == waClosedGate) {
    int hdir = 0;
    for(int i=0; i<c->type; i++) if(c->mov[i]->wall == waClosedGate)
      hdir = i;
    transmatrix V2 = mscale(V, wmspatial?geom3::WALL:1) * ddspin(c, hdir, S42);
    queuepolyat(V2, shPalaceGate, darkena(wcol, 0, 0xFF), wmspatial?PPR_WALL3A:PPR_WALL);
    starcol = 0;
    }
  
  hpcshape& shThisWall = isGrave(c->wall) ? shCross : shWall[ct6];
  
  if(conegraph(c)) {   
    const int layers = 2 << detaillevel;
    for(int z=1; z<layers; z++) {
      double zg = zgrad0(0, geom3::wall_height, z, layers);
      warpfloor(c, xyzscale(V, zg*(layers-z)/layers, zg),
        darkena(gradient(0, wcol, -layers, z, layers), 0, 0xFF), PPR_WALL3+z-layers+2, isWarped(c));
      }
    floorShadow(c, V, SHADOW_WALL, isWarped(c));
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

      bool warp = isWarped(c);
      
      if(starcol && !(wmescher && c->wall == waPlatform)) 
        queuepolyat(Vdepth, shThisWall, darkena(starcol, 0, 0xFF), PPR_WALL3A);

      warpfloor(c, Vdepth, darkena(wcol0, fd, alpha), PPR_WALL3, warp);
      floorShadow(c, V, SHADOW_WALL, warp);
      
      if(c->wall == waCamelot) {
        forCellIdEx(c2, i, c) {
          placeSidewallX(c, i, SIDE_SLEV, V, warp, false, darkena(wcol2, fd, alpha));
          }
        forCellIdEx(c2, i, c) {
          placeSidewallX(c, i, SIDE_SLEV+1, V, warp, false, darkena(wcol2, fd, alpha));
          }
        forCellIdEx(c2, i, c) {
          placeSidewallX(c, i, SIDE_SLEV+2, V, warp, false, darkena(wcol2, fd, alpha));
          }
        forCellIdEx(c2, i, c) {
          placeSidewallX(c, i, SIDE_WTS3, V, warp, false, darkena(wcol2, fd, alpha));
          }
        }
      else {
        forCellIdEx(c2, i, c) 
          if(!highwall(c2) || conegraph(c2)) {
          placeSidewallX(c, i, SIDE_WALL, V, warp, false, darkena(wcol2, fd, alpha));
          }
        }
      }
    }
  }

void qfloor_caves(cell* c, const transmatrix& Vf, int col, int ct6, int xct6) {
  /* if(gp::on) {
    if(pseudohept(c))
      qfloor(c, Vf, shCaveFloor[ishept(c) ? 1 : 2], col);
    else if(ishex1(c))
      qfloor(c, Vf, pispin, shCaveFloor[0], col);
    else
      qfloor(c, Vf, shCaveFloor[0], col);
    }
  else */
  qfloor_eswap(c, Vf, CAVEFLOOR, col);
  }

// ptrn qfloor(c, Vf, CAVEFLOOR, darkena(fcol, fd, 0xFF));

void drawcell(cell *c, transmatrix V, int spinv, bool mirrored) {

#if CAP_TEXTURE
  if(texture::saving) {
    texture::config.apply(c, V, 0xFFFFFFFF);
    return;
    }
#endif

  qfi.shape = NULL; qfi.special = false;
  ivoryz = isGravityLand(c->land);

  bool orig = false;
  if(!inmirrorcount) {
    transmatrix& gm = gmatrix[c];
    orig = 
      gm[2][2] == 0 ? true : 
      torus ? hypot(gm[0][2], gm[1][2]) >= hypot(V[0][2], V[1][2]) :
      sphereflipped() ? fabs(gm[2][2]-1) <= fabs(V[2][2]-1) :
      fabs(gm[2][2]-1) >= fabs(V[2][2]-1) - 1e-8;

    if(orig) gm = V;
    }

  // if(behindsphere(V)) return;
  
  callhooks(hooks_drawcell, c, V);
  
  ld dist0 = hdist0(tC0(V)) - 1e-6;
  if(dist0 < geom3::highdetail) detaillevel = 2;
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

  viewBuggyCells(c,V);
  
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
      //  playerV = V * spin(displaydir(c, cwt.spin) * M_PI/S42);
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
      double dfc = euclid ? intval(VC0, C0) : VC0[2];
    
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
      if(cw.spin) V = V * spin(2*M_PI*cw.spin/cw.c->type);
      drawcell(cw.c, V, 0, cw.mirrored);
      inmirrorcount -= cmc;
      return;
      }                  
    
    // int col = 0xFFFFFF - 0x20 * c->maxdist - 0x2000 * c->cpdist;

    if(!buggyGeneration && c->mpdist > 8 && !cheater) return; // not yet generated
    /* if(!buggyGeneration && c->mpdist > 7 && !cheater) {
      int cd = c->mpdist;
      string label = its(cd);
      int dc = distcolors[cd&7];
      queuestr(V, (cd > 9 ? .6 : 1) * .2, label, 0xFF000000 + dc, 1);
      } */
    
    if(c->land == laNone && (cmode & sm::MAP)) {
      queuepoly(V, shTriangle, 0xFF0000FF);
      }
  
    char ch = winf[c->wall].glyph;
    int wcol, fcol, asciicol;
    
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
    int zcol = fcol;      

    if(peace::on && peace::hint && c->land != laTortoise) {
      int d =
        (c->land == laCamelot || (c->land == laCaribbean && celldistAlt(c) <= 0) || (c->land == laPalace && celldistAlt(c))) ? celldistAlt(c):
        celldist(c);

      int dc = 
        0x10101 * (127 + int(127 * sin(ticks / 200. + d*1.5)));
      wcol = gradient(wcol, dc, 0, .3, 1);
      fcol = gradient(fcol, dc, 0, .3, 1);
      }

    if(c->land == laMirrored || c->land == laMirrorWall2 || c->land == laMirrored2) {
      string label = its(c->landparam);
      queuestr(V, 1 * .2, label, 0xFFFFFFFF, 1);
      }
    
    if(viewdists) {
      int cd = (cwt.c == currentmap->gamestart() && numplayers() == 1) ? celldist(c) : celldistance(c, cwt.c);
      string label = its(cd);
      // string label = its(fieldpattern::getriverdistleft(c)) + its(fieldpattern::getriverdistright(c));
      int dc = distcolors[cd&7];
      wcol = gradient(wcol, dc, 0, .4, 1);
      fcol = gradient(fcol, dc, 0, .4, 1);
      /* queuepolyat(V, shFloor[ct6], darkena(gradient(0, distcolors[cd&7], 0, .25, 1), fd, 0xC0),
        PPR_TEXT); */
      queuestr(V, (cd > 9 ? .6 : 1) * .2, label, 0xFF000000 + distcolors[cd&7], 1);
      }
    
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
    
    if(c->land == laNone && c->wall == waNone) 
      queuepoly(V, shTriangle, 0xFFFF0000);

    if(c->wall == waThumperOn) {
      int ds = ticks;
      for(int u=0; u<5; u++) {
        ld rad = hexf * (.3 * u + (ds%1000) * .0003);
        int tcol = darkena(gradient(forecolor, backcolor, 0, rad, 1.5 * hexf), 0, 0xFF);
        for(int a=0; a<S84; a++)
          queueline(V*ddi0(a, rad), V*ddi0(a+1, rad), tcol, 0);
        }
      }
  
    // bool dothept = false;

    /* if(pseudohept(c) && vid.darkhepta) {
      col = gradient(0, col, 0, 0.75, 1);
      } */
      
    eItem it = c->item;
    
    bool hidden = itemHidden(c);
    bool hiddens = itemHiddenFromSight(c);
    
    if(conformal::includeHistory && eq(c->aitmp, sval)) {
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
      for(int t=0; t<c->type; t++) if(c->mov[t] && c->mov[t]->ligon) {
        int hdir = displaydir(c, t);
        int lcol = darkena(gradient(iinf[itOrbLightning].color, 0, 0, tim, 1100), 0, 0xFF);
        queueline(V*ddi0(ticks, hexf/2), V*ddi0(hdir, crossf), lcol, 2);
        }
      }
    
    int ctype = c->type;
    int ct6 = ctof(c);
    int xct6 = ct6;
    if(gp::on && has_nice_dual() && pseudohept(c) && !ishept(c)) xct6 = 2;

    bool error = false;
    
    chasmg = chasmgraph(c);
    
    int fd = getfd(c);
                     
    if(c->wall == waMagma) fd = 0;
    
    poly_outline = OUTLINE_DEFAULT;

    int sl = snakelevel(c);
    
    transmatrix Vd0, Vf0, Vboat0;
    const transmatrix *Vdp =
      (!wmspatial) ? &V : 
      sl ? &(Vd0= mscale(V, geom3::SLEV[sl])) : 
      highwall(c) ? &(Vd0= mscale(V, (1+geom3::WALL)/2)) :
      (chasmg==1) ? &(Vd0 = mscale(V, geom3::LAKE)) :
      &V;
    
    const transmatrix& Vf = (chasmg && wmspatial) ? (Vf0=mscale(V, geom3::BOTTOM)) : V;

    const transmatrix *Vboat = &(*Vdp);
      
    if(DOSHMUP) {
      ld zlev = -geom3::factor_to_lev(zlevel(tC0((*Vdp))));
      shmup::drawMonster(V, c, Vboat, Vboat0, zlev);
      }

    poly_outline = OUTLINE_DEFAULT;

    if(!wmascii) {
    
      // floor
      
#if CAP_EDIT
      auto si = patterns::getpatterninfo0(c);
#endif
        
      bool eoh = euclid || nonbitrunc;

      if(c->wall == waChasm) {
        zcol = 0;
        int rd = rosedist(c);
        if(rd == 1) 
          queuepoly(V, shRoseFloor[ct6], 0x80406020);
        if(rd == 2)
          queuepoly(V, shRoseFloor[ct6], 0x80406040);
        if(c->land == laZebra) fd++;
        if(c->land == laHalloween && !wmblack) {
          transmatrix Vdepth = wmspatial ? mscale(V, geom3::BOTTOM) : V;
          queuepolyat(Vdepth, shFloor[ct6], darkena(firecolor(ticks / 10), 0, 0xDF),
            PPR_LAKEBOTTOM);
          }
        }
              
#if CAP_EDIT
      else if(mapeditor::drawUserShape(V * applyPatterndir(c, si), 3, si.id,
        darkena(fcol, fd, (cmode & sm::DRAW) ? 0xC0 : 0xFF), c));
      
      else if(patterns::whichShape == '7') {
        if(ishept(c))
          qfloor(c, Vf, wmblack ? shBFloor[ct6] : 
            euclid ? shBigHex :
            shBigHepta, darkena(fcol, fd, 0xFF));
        }
      
      else if(patterns::whichShape == '8') {
        qfloor_eswap(c, Vf, shTriheptaFloor[ctof(c)], darkena(fcol, fd, 0xFF));
        }
      
      else if(patterns::whichShape == '6') {
        if(!ishept(c))
          qfloor(c, Vf, 
            wmblack ? shBFloor[ct6] : 
            euclid ? (ishex1(c) ? shBigHexTriangle : shBigHexTriangleRev) :
            shBigTriangle, darkena(fcol, fd, 0xFF));
        }
#endif

#if CAP_TEXTURE
      else if(texture::config.apply(c, Vf, darkena(fcol, fd, 0xFF))) ;
#endif
      
      else if(c->land == laMirrorWall) {
        int d = mirror::mirrordir(c);
        bool onleft = c->type == 7;
        if(c->type == 7 && c->barleft == laMirror)
          onleft = !onleft;
        if(c->type == 6 && c->mov[d]->barleft == laMirror)
          onleft = !onleft;
        if(nonbitrunc) onleft = !onleft;
        
        if(d == -1) {
          for(d=0; d<6; d++) 
            if(c->mov[d] && c->mov[(1+d)%6] && c->mov[d]->land == laMirrorWall && c->mov[(1+d)%6]->land == laMirrorWall)
              break;
          qfi.spin = ddspin(c, d, 0);
          transmatrix V2 = V * qfi.spin;          
          if(!wmblack) for(int d=0; d<6; d++) {
            inmirrorcount+=d;
            qfloor(c, V2 * spin(d*M_PI/3), shHalfFloor[2], darkena(fcol, fd, 0xFF));
            inmirrorcount-=d;
            }          
          if(wmspatial) {
            const int layers = 2 << detaillevel;
            for(int z=1; z<layers; z++) 
              queuepolyat(mscale(V2, zgrad0(0, geom3::wall_height, z, layers)), shHalfMirror[2], 0xC0C0C080, PPR_WALL3+z-layers);
            }
          else
            queuepolyat(V2, shHalfMirror[2], 0xC0C0C080, PPR_WALL3);
          }
        else {
          qfi.spin = ddspin(c, d, S42);
          transmatrix V2 = V * qfi.spin;
          if(!wmblack) {
            inmirrorcount++;
            qfloor(c, mirrorif(V2, !onleft), shHalfFloor[ct6], darkena(fcol, fd, 0xFF));
            inmirrorcount--;
            qfloor(c, mirrorif(V2, onleft), shHalfFloor[ct6], darkena(fcol, fd, 0xFF));
            }
  
          if(wmspatial) {
            const int layers = 2 << detaillevel;
            for(int z=1; z<layers; z++) 
              queuepolyat(mscale(V2, zgrad0(0, geom3::wall_height, z, layers)), shHalfMirror[ct6], 0xC0C0C080, PPR_WALL3+z-layers);
            }
          else 
            queuepolyat(V2, shHalfMirror[ct6], 0xC0C0C080, PPR_WALL3);
          }
        }
      
      else if(c->land == laWineyard && cellHalfvine(c)) {

        int i =-1;
        for(int t=0;t<6; t++) if(c->mov[t] && c->mov[t]->wall == c->wall)
          i = t;

        qfi.spin = ddspin(c, i, S14);
        transmatrix V2 = V * qfi.spin;
        
        if(wmspatial && wmescher) {
          qfi.shape = &shSemiFeatherFloor[0]; qfi.special = true;
          int dk = 1;
          int vcol = winf[waVinePlant].color;
          warpfloor(c, mscale(V, geom3::WALL), darkena(vcol, dk, 0xFF), PPR_WALL3A, false);
          escherSidewall(c, SIDE_WALL, V, darkena(gradient(0, vcol, 0, .8, 1), dk, 0xFF));
          qfloor(c, V2, shSemiFeatherFloor[1], darkena(fcol, dk, 0xFF));
          qfi.shape = &shFeatherFloor[0]; qfi.special = true;
          }
        
        else if(wmspatial) {
          hpcshape *shar = wmplain ? shFloor : shFeatherFloor;
          
          int dk = 1;
          qfloor(c, V, shar[0], darkena(fcol, dk, 0xFF));
          
          int vcol = winf[waVinePlant].color;
          int vcol2 = gradient(0, vcol, 0, .8, 1);
          
          transmatrix Vdepth = mscale(V2, geom3::WALL);

          queuepolyat(Vdepth, shSemiFloor[0], darkena(vcol, dk, 0xFF), PPR_WALL3A);
          {dynamicval<int> p(poly_outline, OUTLINE_TRANS); queuepolyat(V2 * spin(M_PI*2/3), shSemiFloorShadow, SHADOW_WALL, PPR_WALLSHADOW); }
          queuepolyat(V2, shSemiFloorSide[SIDE_WALL], darkena(vcol, dk, 0xFF), PPR_WALL3A-2+away(V2));

          if(validsidepar[SIDE_WALL]) forCellIdEx(c2, j, c) {
            int dis = i-j;
            dis %= 6;
            if(dis<0) dis += 6;
            if(dis != 1 && dis != 5) continue;
            placeSidewall(c, j, SIDE_WALL, V, false, false, darkena(vcol2, fd, 0xFF));
            }
          }
        
        else {        
          hpcshape *shar = shSemiFeatherFloor;
          
          if(wmblack) shar = shSemiBFloor;
          if(wmplain) shar = shSemiFloor;
          
          int dk = wmblack ? 0 : wmplain ? 1 : 1;
          
          qfloor(c, V2, shar[0], darkena(winf[waVinePlant].color, dk, 0xFF));
          qfloor(c, V2, shar[1], darkena(fcol, dk, 0xFF));
          }
        }

      else if(c->land == laReptile || c->wall == waReptile)
        drawReptileFloor(Vf, c, fcol, true);
      
      else if(wmblack == 1 && c->wall == waMineOpen && vid.grid) 
        ;
      
//         else if(true) 
//          qfloor(c, Vf, shWave[wavephase][ct6], darkena(fcol, fd, 0xFF));
//        else if(true) 
//          qfloor(c, Vf, shSeabed[ct6], darkena(fcol, fd, 0xFF));

      else if(wmblack) {
        qfloor(c, Vf, shBFloor[ct6], darkena(fcol, 0, 0xFF));
        int rd = rosedist(c);
        if(rd == 1)
          qfloor(c, Vf, shHeptaMarker, darkena(fcol, 0, 0x80));
        else if(rd == 2)
          qfloor(c, Vf, shHeptaMarker, darkena(fcol, 0, 0x40));
        }
      
      else if(isWarped(c) && euclid) 
        qfloor_eswap(c, Vf, shTriheptaFloor[ctof(c)], darkena(fcol, fd, 0xFF));

      else if(is_nice_dual(c) && !pseudohept(c)) {
        qfloor_eswap(c, Vf, shBigTriangle, darkena(fcol, fd, 0xFF));
        }

      else if(isWarped(c) && !nbtnice && !shmup::on) {
        auto si = patterns::getpatterninfo(c, 0, 0);
        if(si.id < 14)
          qfloor(c, Vf, applyPatterndir(c, si), shTriheptaFloor[si.id], darkena(fcol, fd, 0xFF));
        else
          qfloor(c, Vf, shFloor[ctof(c)], darkena(fcol, fd, 0xFF));
        }

      else if(wmplain) {
        if(wmspatial && highwall(c)) ;
        else qfloor(c, Vf, PLAINFLOOR, darkena(fcol, fd, 0xFF));
        }

      else if(randomPatternsMode && c->land != laBarrier && !isWarped(c->land)) {
        int j = (randompattern[c->land]/5) % 15;
        int dfcol = darkena(fcol, fd, 0xFF);
        int k = randompattern[c->land] % RPV_MODULO;
        int k7 = randompattern[c->land] % 7;
        
        if(k == RPV_ZEBRA && k7 < 2) drawZebraFloor(Vf, c, dfcol);
        else if(k == RPV_EMERALD && k7 == 0) drawEmeraldFloor(Vf, c, dfcol);
        else if(k == RPV_CYCLE && k7 < 4) drawTowerFloor(Vf, c, dfcol, celldist);
 
        else switch(j) {
          case 0:  qfloor(c, Vf, CLOUDFLOOR, dfcol); break;
          case 1:  qfloor(c, Vf, FEATHERFLOOR, dfcol); break;
          case 2:  qfloor(c, Vf, STARFLOOR, dfcol); break;
          case 3:  qfloor_eswap(c, Vf, TRIFLOOR, dfcol); break;
          case 4:  qfloor_eswap(c, Vf, SSTARFLOOR, dfcol); break;
          case 5:  qfloor(c, Vf, OVERFLOOR, dfcol); break;
          case 6:  qfloor(c, Vf, FEATHERFLOOR, dfcol); break;
          case 7:  qfloor_eswap(c, Vf, DEMONFLOOR, dfcol); break;
          case 8:  qfloor_eswap(c, Vf, CROSSFLOOR, dfcol); break;
          case 9:  qfloor(c, Vf, MFLOOR1, dfcol); break;
          case 10: qfloor_caves(c, Vf, dfcol, ct6, xct6); break;
          case 11: qfloor_eswap(c, Vf, POWERFLOOR, dfcol); break;
          case 12: qfloor_eswap(c, Vf, DESERTFLOOR, dfcol); break;
          case 13: qfloor_eswap(c, Vf, CHARGEDFLOOR, dfcol); break;
          case 14: qfloor_eswap(c, Vf, CHARGEDFLOOR, dfcol); break;
          }
        }

      // else if(c->land == laPrairie && !eoh && allemptynear(c) && fieldpattern::getflowerdist(c) <= 1)
      //   queuepoly(Vf, shLeafFloor[ct6], darkena(fcol, fd, 0xFF));

      /* else if(c->land == laPrairie && prairie::isriver(c))
        drawTowerFloor(Vf, c, darkena(fcol, fd, 0xFF), 
          prairie::isleft(c) ? river::towerleft : river::towerright); */
      
      else switch(c->land) {
        case laPrairie:
          qfloor(c, Vf, CLOUDFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laWineyard:
          qfloor(c, Vf, FEATHERFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laZebra:
          drawZebraFloor(Vf, c, darkena(fcol, fd, 0xFF));
          break;
        
        case laMountain:
          drawTowerFloor(Vf, c, darkena(fcol, fd, 0xFF), 
            euclid ? celldist : c->master->alt ? celldistAltPlus : celldist);
          break;
        
        case laEmerald:
          drawEmeraldFloor(Vf, c, darkena(fcol, fd, 0xFF));
          break;
        
        case laRlyeh:
          qfloor_eswap(c, Vf, TRIFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laTemple:
          qfloor_eswap(c, Vf, TRIFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laAlchemist:
          qfloor(c, Vf, CLOUDFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laVolcano:
          qfloor_eswap(c, Vf, LAVAFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laRose:
          qfloor_eswap(c, Vf, ROSEFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laTortoise:
          qfloor_eswap(c, Vf, TURTLEFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laBurial: case laRuins:
          qfloor(c, Vf, BARROWFLOOR, darkena(fcol, fd, 0xFF));
          break;

        case laTrollheim:
          if(!eoh && !gp::on)
            qfloor_eswap(c, Vf, TROLLFLOOR, darkena(fcol, fd, 0xFF));
         else
           qfloor_caves(c, Vf, darkena(fcol, fd, 0xFF), ct6, xct6);
         break;

        case laJungle:
          qfloor(c, Vf, FEATHERFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        /*case laMountain:
          qfloor(c, Vf, FEATHERFLOOR, darkena(fcol, fd, 0xFF));
          break; */
        
        case laGraveyard:
          qfloor_eswap(c, Vf, CROSSFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laDeadCaves:
          qfloor_caves(c, Vf, darkena(fcol, fd, 0xFF), ct6, xct6);
          break;
        
        case laMotion:
          qfloor(c, Vf, MFLOOR1, darkena(fcol, fd, 0xFF));
          break;
        
        case laWhirlwind:
          qfloor_eswap(c, Vf, NEWFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laHell:
          qfloor_eswap(c, Vf, DEMONFLOOR, darkena(fcol, fd, 0xFF));
          break;
              
        case laIce: case laBlizzard:
          qfloor(c, Vf, STARFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laCocytus:
          qfloor_eswap(c, Vf, DESERTFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laSwitch:
          qfloor_eswap(c, Vf, SWITCHFLOOR, darkena(fcol, fd, 0xFF));
          if(ctof(c)) for(int i=0; i<c->type; i++)
            queuepoly(Vf * ddspin(c, i, S6) * xpush(rhexf), shSwitchDisk, darkena(minf[active_switch()].color, fd, 0xFF));
          break;

        case laStorms:
          qfloor_eswap(c, Vf, CHARGEDFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laWildWest:
          qfloor_eswap(c, Vf, SSTARFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laPower:
          qfloor_eswap(c, Vf, POWERFLOOR, darkena(fcol, fd, 0xFF));
          break;

        case laCaves:
          qfloor_caves(c, Vf, darkena(fcol, fd, 0xFF), ct6, xct6);
          break;
        
        case laDesert:
          qfloor_eswap(c, Vf, DESERTFLOOR, darkena(fcol, fd, 0xFF));
          break;

        case laBull:
          qfloor_eswap(c, Vf, BUTTERFLYFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laDryForest:
          qfloor_eswap(c, Vf, DESERTFLOOR, darkena(fcol, fd, 0xFF));
          break;

        case laCaribbean: case laOcean: case laOceanWall: case laWhirlpool:
          qfloor(c, Vf, CLOUDFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laKraken:
          qfloor(c, Vf, FULLFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laDocks:
          qfloor(c, Vf, FULLFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laLivefjord:
          qfloor_caves(c, Vf, darkena(fcol, fd, 0xFF), ct6, xct6);
          break;
        
        case laRedRock: case laSnakeNest:
          qfloor_eswap(c, Vf, DESERTFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laPalace: case laTerracotta:
          qfloor_eswap(c, Vf, PALACEFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laMercuryRiver: {
          if(eoh)
            qfloor(c, Vf, PLAINFLOOR, darkena(fcol, fd, 0xFF));
          else {
            int bridgedir = -1;
            if(c->type == 6) {
              for(int i=1; i<c->type; i+=2)
                if(pseudohept(c->mov[(i+5)%6]) && c->mov[(i+5)%6]->land == laMercuryRiver)
                if(pseudohept(c->mov[(i+1)%6]) && c->mov[(i+1)%6]->land == laMercuryRiver)
                  bridgedir = i;
              }
            if(bridgedir == -1)
              qfloor_eswap(c, Vf, PALACEFLOOR, darkena(fcol, fd, 0xFF));
            else {
              transmatrix bspin = ddspin(c, bridgedir);
              qfloor(c, Vf, bspin, shMercuryBridge[0], darkena(fcol, fd, 0xFF));
              // only needed in one direction
              if(c < c->mov[bridgedir]) {
                bspin = Vf * bspin;
                queuepoly(bspin, shMercuryBridge[1], darkena(fcol, fd+1, 0xFF));
                if(wmspatial) {
                  queuepolyat(mscale(bspin, geom3::LAKE), shMercuryBridge[1], darkena(gradient(0, winf[waMercury].color, 0, 0.8,1), 0, 0x80), PPR_LAKELEV);
                  queuepolyat(mscale(bspin, geom3::BOTTOM), shMercuryBridge[1], darkena(0x202020, 0, 0xFF), PPR_LAKEBOTTOM);
                  }
                }
              }
            }
          break;
          }
        
        case laOvergrown: case laClearing: case laHauntedWall: case laHaunted: case laHauntedBorder:
          qfloor(c, Vf, OVERFLOOR, darkena(fcol, fd, 0xFF));
          break;

        case laHive:
          if(c->wall != waFloorB && c->wall != waFloorA && c->wall != waMirror && c->wall != waCloud) {
            qfloor(c, Vf, PLAINFLOOR, darkena(fcol, 1, 0xFF));
            if(c->wall != waMirror && c->wall != waCloud)
              qfloor(c, Vf, MFLOOR1, darkena(fcol, 2, 0xFF));
            if(c->wall != waMirror && c->wall != waCloud)
              qfloor(c, Vf, MFLOOR2, darkena(fcol, fcol==wcol ? 1 : 2, 0xFF));
            }
          else
            qfloor(c, Vf, PLAINFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laEndorian:
          if(c->wall == waTrunk) 
            qfloor(c, Vf, PLAINFLOOR, darkena(fcol, fd, 0xFF));
    
          else if(c->wall == waCanopy || c->wall == waSolidBranch || c->wall == waWeakBranch) 
            qfloor(c, Vf, FEATHERFLOOR, darkena(fcol, fd, 0xFF));
          
          else 
            drawTowerFloor(Vf, c, darkena(fcol, fd, 0xFF));
          break;
        
        case laDragon:
          if(nonbitrunc) 
            qfloor(c, Vf, PLAINFLOOR, darkena(fcol, fd, 0xFF));
          else          
            qfloor(c, Vf, DRAGONFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laEFire: case laEAir: case laEWater: case laEEarth: case laElementalWall:
          qfloor_eswap(c, Vf, NEWFLOOR, darkena(fcol, fd, 0xFF));
          break;
        
        case laIvoryTower: case laDungeon: 
          drawTowerFloor(Vf, c, darkena(fcol, fd, 0xFF));
          break;

        default: 
          qfloor(c, Vf, PLAINFLOOR, darkena(fcol, fd, 0xFF));
        }
        
      // walls

#if CAP_EDIT

      if(patterns::displaycodes) {
      
        auto si = patterns::getpatterninfo0(c);
        
        for(int i=0; i<c->type; i += si.symmetries) {
          queuepoly(V * applyPatterndir(c,si), shAsymmetric, darkena(0x000000, 0, 0xC0));
          si.dir += si.symmetries;
          }
        
        string label = its(si.id);
        queuestr(V, .5, label, 0xFF000000 + forecolor);
        }
#endif

      if((cmode & sm::NUMBER) && (dialog::editingDetail())) {
        int col = 
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
        if(s >= 1)
          qfloor(c, V, shRedRockFloor[0][xct6], getSnakelevColor(c, 0, 7, fd, wcol));
        if(s >= 2)
          queuepoly(V, shRedRockFloor[1][xct6], getSnakelevColor(c, 1, 7, fd, wcol));
        if(s >= 3)
          queuepoly(V, shRedRockFloor[2][xct6], getSnakelevColor(c, 2, 7, fd, wcol));
        }
      
      if(c->wall == waTower && !wmspatial) {
        qfloor(c, V, shMFloor[ct6], darkena(0xE8E8E8, fd, 0xFF));
        }
      
      if(pseudohept(c) && (
        c->land == laRedRock || 
        vid.darkhepta ||
        (c->land == laClearing && nonbitrunc))) {
        queuepoly((*Vdp), shHeptaMarker, wmblack ? 0x80808080 : 0x00000080);
        }

      if(conformal::includeHistory && eq(c->aitmp, sval-1))
        queuepoly((*Vdp), shHeptaMarker, 0x000000C0);

      char xch = winf[c->wall].glyph;
      
      switch(c->wall) {
      
        case waBigBush:
          if(detaillevel >= 2)
            queuepolyat(mmscale(V, zgrad0(0, geom3::slev, 1, 2)), shHeptaMarker, darkena(wcol, 0, 0xFF), PPR_REDWALL);
          if(detaillevel >= 1)
            queuepolyat(mmscale(V, geom3::SLEV[1]) * pispin, shWeakBranch, darkena(wcol, 0, 0xFF), PPR_REDWALL+1);
          if(detaillevel >= 2)
            queuepolyat(mmscale(V, zgrad0(0, geom3::slev, 3, 2)), shHeptaMarker, darkena(wcol, 0, 0xFF), PPR_REDWALL+2);
          queuepolyat(mmscale(V, geom3::SLEV[2]), shSolidBranch, darkena(wcol, 0, 0xFF), PPR_REDWALL+3);
          break;
        
        case waSmallBush:
          if(detaillevel >= 2)
            queuepolyat(mmscale(V, zgrad0(0, geom3::slev, 1, 2)), shHeptaMarker, darkena(wcol, 0, 0xFF), PPR_REDWALL);
          if(detaillevel >= 1)
            queuepolyat(mmscale(V, geom3::SLEV[1]) * pispin, shWeakBranch, darkena(wcol, 0, 0xFF), PPR_REDWALL+1);
          if(detaillevel >= 2)
            queuepolyat(mmscale(V, zgrad0(0, geom3::slev, 3, 2)), shHeptaMarker, darkena(wcol, 0, 0xFF), PPR_REDWALL+2);
          queuepolyat(mmscale(V, geom3::SLEV[2]), shWeakBranch, darkena(wcol, 0, 0xFF), PPR_REDWALL+3);
          break;
      
        case waSolidBranch:
          queuepoly(V, shSolidBranch, darkena(wcol, 0, 0xFF));
          break;
        
        case waWeakBranch:
          queuepoly(V, shWeakBranch, darkena(wcol, 0, 0xFF));
          break;
      
        case waLadder:
          if(euclid) {
            queuepoly(V, shMFloor[ct6], 0x804000FF);
            queuepoly(V, shMFloor2[ct6], 0x000000FF);
            }
          else {
            queuepolyat(V, shFloor[ct6], 0x804000FF, PPR_FLOOR+1);
            queuepolyat(V, shMFloor[ct6], 0x000000FF, PPR_FLOOR+2);
            }
          break;
        
        case waReptileBridge: {
          Vboat = &(Vboat0 = V);
          dynamicval<qfloorinfo> qfi2(qfi, qfi);
          int col = reptilecolor(c);
          chasmg = 0;
          drawReptileFloor(V, c, col, true);
          forCellIdEx(c2, i, c) if(chasmgraph(c2)) 
            placeSidewallX(c, i, SIDE_LAKE, V, isWarped(c), false, darkena(gradient(0, col, 0, .8, 1), fd, 0xFF));
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
            if(!nospin) Vboat0 = Vboat0 * ddspin(c, c->mondir, S42);
            queuepolyat(Vboat0, shBoatOuter, outcol, PPR_BOATLEV);
            Vboat = &(Vboat0 = V);
            }
          if(c->wall == waBoat) {
            nospin = applyAnimation(c, Vboat0, footphase, LAYER_BOAT);
            }
          if(!nospin) 
            Vboat0 = Vboat0 * ddspin(c, c->mondir, S42);
          else {
            transmatrix Vx;
            if(applyAnimation(c, Vx, footphase, LAYER_SMALL))
              animations[LAYER_SMALL][c].footphase = 0;
            }
          if(wmspatial)
            queuepolyat(mscale(Vboat0, (geom3::LAKE+1)/2), shBoatOuter, outcol, PPR_BOATLEV2);
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
            PPR_BIGSTATUE
            );
          break;
          }
        
        case waSulphurC: {
          if(drawstar(c)) {
            zcol = wcol;
            if(wmspatial) 
              queuepolyat(mscale(V, geom3::HELLSPIKE), shGiantStar[ct6], darkena(wcol, 0, 0x40), PPR_HELLSPIKE);
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
          if((ctype&1) && wmescher) V2 = V * pispin;
          queuepoly(V2, shMFloor[ct6], darkena(winf[c->wall].color, 0, 0xFF));
          queuepoly(V2, shMFloor2[ct6], (!wmblack) ? darkena(fcol, 1, 0xFF) : darkena(0,1,0xFF));
          break;
          }
        
        case waFrozenLake: case waLake: case waCamelotMoat:
        case waSea: case waOpenGate: case waBubble: case waDock:
        case waNone: case waSulphur: case waMercury:
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
            int col = winf[waGlass].color;
            int dcol = darkena(col, 0, 0x80);
            transmatrix Vdepth = mscale((*Vdp), geom3::WALL);
            queuepolyat(Vdepth, shMFloor[ct6], dcol, PPR_WALL); // GLASS
            if(validsidepar[SIDE_WALL]) forCellIdEx(c2, i, c) 
              placeSidewall(c, i, SIDE_WALL, (*Vdp), false, true, dcol);
            }
          break;
        
        case waFan:
          queuepoly(V * spin(M_PI/6 - fanframe * M_PI / 3), shFan, darkena(wcol, 0, 0xFF));
          break;
        
        case waArrowTrap:
          if(c->wparam >= 1)
            queuepoly(V, shDisk, darkena(trapcol[c->wparam&3], 0, 0xFF));
          if(isCentralTrap(c)) arrowtraps.push_back(c);
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
           dynamicval<bool> db(qfi.special, true);
           for(int z=1; z<layers; z++) {
             double zg = zgrad0(-geom3::lake_top, geom3::wall_height, z, layers);
             warpfloor(c, xyzscale(V, zg*(layers-z)/layers, zg),
               darkena(gradient(0, wcol, -layers, z, layers), 0, 0xFF), PPR_WALL3+z-layers+2, isWarped(c));
             }
            }
          else goto wa_default;
          break;
        
        case waMineOpen: {
          int mines = countMinesAround(c);
          if(mines)
            queuepoly(V, shMineMark[ct6], (minecolors[mines] << 8) | 0xFF);
          break;
          }
      
        default: {
          wa_default:
          if(sl && wmspatial) {
      
            bool w = isWarped(c);
            warpfloor(c, (*Vdp), darkena(wcol, fd, 0xFF), PPR_REDWALL-4+4*sl, w);
            floorShadow(c, V, SHADOW_SL * sl, w);
            for(int s=0; s<sl; s++) 
            forCellIdEx(c2, i, c) {
              int sl2 = snakelevel(c2);
              if(s >= sl2)
                placeSidewallX(c, i, SIDE_SLEV+s, V, w, false, getSnakelevColor(c, s, sl, fd, wcol));
              }
            }
          
          else if(highwall(c)) 
            draw_wall(c, V, wcol, zcol, ct6, fd);
 
          else if(xch == '%') {
            if(doHighlight())
              poly_outline = (c->land == laMirror) ? OUTLINE_TREASURE : OUTLINE_ORB;
            
            if(wmspatial) {
              int col = winf[c->wall].color;
              int dcol = darkena(col, 0, 0xC0);
              transmatrix Vdepth = mscale((*Vdp), geom3::WALL);
              queuepolyat(Vdepth, shMFloor[ct6], dcol, PPR_WALL); // GLASS
              if(validsidepar[SIDE_WALL]) forCellIdEx(c2, i, c) 
                placeSidewall(c, i, SIDE_WALL, (*Vdp), false, true, dcol);
              }
            else {
              queuepoly(V, shMirror, darkena(wcol, 0, 0xC0));
              }
            poly_outline = OUTLINE_DEFAULT;
            }
          
          else if(isFire(c) || isThumper(c) || c->wall == waBonfireOff) {
            ld sp = 0;
            if(hasTimeout(c)) sp = ticks / (c->land == laPower ? 5000. : 500.);
            queuepoly(V * spin(sp), shStar, darkena(wcol, 0, 0xF0));
            if(isFire(c) && rand() % 300 < ticks - lastt)
              drawParticle(c, wcol, 75);
            }
          
          else if(xch != '.' && xch != '+' && xch != '>' && xch != ':'&& xch != '-' && xch != ';' && xch != ',' && xch != '&')
            error = true;
          }
        }
      }

    else {
      if(c->wall == waArrowTrap)
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
    
    int sha = shallow(c);

    if(wmspatial && sha) {
      bool w = isWarped(c);
      int col = (highwall(c) || c->wall == waTower) ? wcol : fcol;
      if(!chasmg) {

#define D(v) darkena(gradient(0, col, 0, v * (sphere ? spherity(V * cellrelmatrix(c,i)) : 1), 1), fd, 0xFF)
// #define D(v) darkena(col, fd, 0xFF)

        if(sha & 1) {
          forCellIdEx(c2, i, c) if(chasmgraph(c2)) 
            placeSidewallX(c, i, SIDE_LAKE, V, w, false, D(.8));
          }
        if(sha & 2) {
          forCellIdEx(c2, i, c) if(chasmgraph(c2)) 
            placeSidewallX(c, i, SIDE_LTOB, V, w, false, D(.7));
          }
        if(sha & 4) {
          bool dbot = true;
          forCellIdEx(c2, i, c) if(chasmgraph(c2) == 2) {
            if(dbot) dbot = false,
              warpfloor(c, mscale(V, geom3::BOTTOM), 0x080808FF, PPR_LAKEBOTTOM, isWarped(c));
            placeSidewallX(c, i, SIDE_BTOI, V, w, false, D(.6));
            }
#undef D
          }
        }
      // wall between lake and chasm -- no Escher here
      if(chasmg == 1) forCellIdEx(c2, i, c) if(chasmgraph(c2) == 2) {
        placeSidewall(c, i, SIDE_LAKE, V, w, false, 0x202030FF);
        placeSidewall(c, i, SIDE_LTOB, V, w, false, 0x181820FF);
        placeSidewall(c, i, SIDE_BTOI, V, w, false, 0x101010FF);
        }
      }
    
    if(false) if(chasmg == 2 && wmspatial && sphere) {
      forCellIdEx(c2, i, c) if(chasmgraph(c2) == 0) {
        transmatrix V2 = V * cellrelmatrix(c, i);
        // if(!behindsphere(V2)) continue;
        bool w = isWarped(c2);
        int wcol2, fcol2;
        setcolors(c2, wcol2, fcol2);
        int col = (highwall(c2) || c->wall == waTower) ? wcol2 : fcol2;
        col = gradient(0, col, 0, spherity(V), 1);
        int j = c->spin(i);
        if(ticks % 500 < -250) {
          V2 = V2 * ddspin(c2, j);
          j = 0;
          }
        placeSidewall(c2, j, SIDE_LAKE, V2, w, false, darkena(gradient(0, col, 0, .8, 1), fd, 0xFF));
        placeSidewall(c2, j, SIDE_LTOB, V2, w, false, darkena(gradient(0, col, 0, .7, 1), fd, 0xFF));
        placeSidewall(c2, j, SIDE_BTOI, V2, w, false, darkena(gradient(0, col, 0, .6, 1), fd, 0xFF));
        }
      }
    
    if(chasmg == 1 && wmspatial) {
      int fd0 = fd ? fd-1 : 0;
      
      qfi.shape = &getSeabed(*qfi.shape);
      // this draws the water surface
      warpfloor(c, (*Vdp), darkena(fcol, fd0, 0x80), PPR_LAKELEV, isWarped(c));
      }
    
    if(chasmg) {
      int q = size(ptds);
      int maxtime = euclid || sphere ? 20000 : 1500;
      if(fallanims.count(c)) {
         fallanim& fa = fallanims[c];
         bool erase = true;
         if(fa.t_floor) {
           int t = (ticks - fa.t_floor);
           if(t <= maxtime) {
             erase = false;
             if(fa.walltype == waNone)
               warpfloor(c, V, darkena(fcol, fd, 0xFF), PPR_FLOOR, isWarped(c));
             else {
               int wcol2, fcol2;
               eWall w = c->wall; int p = c->wparam;
               c->wall = fa.walltype; c->wparam = fa.m;
               setcolors(c, wcol2, fcol2);
               int starcol = c->wall == waVinePlant ? 0x60C000 : wcol2;
               c->wall = w; c->wparam = p;
               bool warp = isWarped(c);
               warpfloor(c, mscale(V, geom3::WALL), darkena(starcol, fd, 0xFF), PPR_WALL3, warp);
               queuepolyat(mscale(V, geom3::WALL), shWall[ct6], darkena(wcol2, 0, 0xFF), PPR_WALL3A);
               forCellIdEx(c2, i, c)
                 placeSidewallX(c, i, SIDE_WALL, V, warp, false, darkena(wcol2, 1, 0xFF));
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

#if CAP_EDIT
    if(c == mapeditor::drawcell) {
      if(mapeditor::drawcellShapeGroup() == 2) {
        mapeditor::drawtrans = V;
        }
      qfi_dc = qfi;
      }
#endif

    if(it) {
      if((c->land == laWhirlwind || c->item == itBabyTortoise) && c->wall != waBoat) {
        double footphase = 0;
        Vboat = &(Vboat0 = *Vboat);
        applyAnimation(c, Vboat0, footphase, LAYER_BOAT);
        }

      if(cellHalfvine(c)) {
        int i =-1;
        for(int t=0;t<6; t++) if(c->mov[t] && c->mov[t]->wall == c->wall)
          i = t;
    
        Vboat = &(Vboat0 = *Vboat * ddspin(c, i) * xpush(-.13));
        }
    
      error |= drawItemType(it, c, *Vboat, icol, ticks, hidden);
      }

    if(true) {
      int q = ptds.size();
      error |= drawMonster(V, ctype, c, moncol); 
      if(Vboat != &V && Vboat != &Vboat0 && q != size(ptds)) 
        pushdown(c, q, V, -geom3::factor_to_lev(zlevel(tC0((*Vboat)))),
          !isMultitile(c->monst), false);
      }
      
    if(!shmup::on && sword::at(c)) {
      queuepolyat(V, shDisk, 0xC0404040, PPR_SWORDMARK);
      }
    
    addaura(tC0(V), zcol, fd);

    int ad = airdist(c);
    if(ad == 1 || ad == 2) {

     for(int i=0; i<c->type; i++) {
       cell *c2 = c->mov[i]; 
       if(airdist(c2) < airdist(c)) {
         calcAirdir(c2); // printf("airdir = %d\n", airdir);
         transmatrix V0 = ddspin(c, i, S42);
         
         double ph = ticks / (nonbitrunc?150:75.0) + airdir * M_PI / (S21+.0);
         
         int aircol = 0x8080FF00 | int(32 + 32 * -cos(ph));
         
         double ph0 = ph/2;
         ph0 -= floor(ph0/M_PI)*M_PI;

         poly_outline = OUTLINE_TRANS;
         queuepoly((*Vdp)*V0*xpush(hexf*-cos(ph0)), shDisk, aircol);
         poly_outline = OUTLINE_DEFAULT;
         }
       }
      }

    if(c->land == laBlizzard) {
      if(vid.backeffects) {
        if(c->cpdist <= getDistLimit())
          blizzardcells[c].frame = frameid;
        }
      else {
        forCellIdEx(c2, i, c) if(againstWind(c, c2))
          queuepoly(V * ddspin(c, i) * xpush(cellgfxdist(c, i)/2), shWindArrow, 0x8080FF80);
        }
      }

    if(c->land == laWhirlwind) {
      whirlwind::calcdirs(c);
      
      for(int i=0; i<whirlwind::qdirs; i++) {
        int hdir0 = displaydir(c, whirlwind::dfrom[i]) + S42;
        int hdir1 = displaydir(c, whirlwind::dto[i]);
 
        double ph1 = fanframe;
        
        int aircol = 0xC0C0FF40;
        
        ph1 -= floor(ph1);
        
        if(hdir1 < hdir0-S42) hdir1 += S84;
        if(hdir1 >= hdir0+S42) hdir1 -= S84;
        
        int hdir = (hdir1*ph1+hdir0*(1-ph1));
 
        transmatrix V0 = spin((hdir) * M_PI / S42);
        
        double ldist = nonbitrunc ? crossf : c->type == 6 ? .2840 : 0.3399;
 
        poly_outline = OUTLINE_TRANS;
        queuepoly((*Vdp)*V0*xpush(ldist*(2*ph1-1)), shDisk, aircol);
        poly_outline = OUTLINE_DEFAULT;
        }

      }

    if(error) {
      queuechr(V, 1, ch, darkenedby(asciicol, darken), 2);
      }
    
    if(vid.grid) {
      vid.linewidth *= 3;
      // sphere: 0.3948
      // sphere heptagonal: 0.5739
      // sphere trihepta: 0.3467
      
      // hyper trihepta: 0.2849
      // hyper heptagonal: 0.6150
      // hyper: 0.3798
      
      int prec = sphere ? 3 : 1;
      
      if(gp::on) {
        vid.linewidth *= gp::scale * 2;
        if(isWarped(c) && has_nice_dual()) {
          if(pseudohept(c)) for(int t=0; t<c->type; t++)
            queueline(V * gp::get_corner_position(c, t%c->type, 2),
                      V * gp::get_corner_position(c, (t+1)%c->type, 2),
                      gridcolor(c, c->mov[t]), prec);
          }
        else for(int t=0; t<c->type; t++)
          if(c->mov[t] && c->mov[t] < c)
          queueline(V * gp::get_corner_position(c, t),
                    V * gp::get_corner_position(c, (t+1)%c->type),
                    gridcolor(c, c->mov[t]), prec);
        vid.linewidth /= gp::scale * 2;
        }
      else if(nonbitrunc) {
        double x = hcrossf;
        for(int t=0; t<S7; t++) 
          if(c->mov[t] && c->mov[t] < c)
          queueline(V * ddspin(c,t,-S6) * xpush0(x), 
                    V * ddspin(c,t,S6) * xpush0(x), 
                    gridcolor(c, c->mov[t]), prec);
        }
      else if(isWarped(c)) {
        double x = hexhexdist/2;
        if(!ishept(c)) for(int t=0; t<S6; t++) if(c->mov[t] && ishept(c->mov[t]))
          queueline(V * ddspin(c,t,-S14) * xpush0(x), 
                    V * ddspin(c,t,+S14) * xpush0(x), 
                    gridcolor(c, c->mov[t]), prec);
        }
      else if(ishept(c) && !(euclid&&!a4)) ;
      else {
        double x = hexvdist;
        for(int t=0; t< S6; t++) 
          if((euclid&&!a4) ? c->mov[t]<c : (((t^1)&1) || c->mov[t] < c))
          queueline(V * ddspin(c,t,-S7) * xpush0(x), 
                    V * ddspin(c,t,+S7) * xpush0(x), 
                    gridcolor(c, c->mov[t]), prec);
        }
      vid.linewidth /= 3;
      }

    if(!euclid) {
      bool usethis = false;
      double spd = 1;
      bool rev = false;
      
      if(isGravityLand(cwt.c->land)) {
        if(cwt.c->land == laDungeon) rev = true;
        if(conformal::do_rotate >= 1)
        if(!straightDownSeek || edgeDepth(c) < edgeDepth(straightDownSeek)) {
          usethis = true;
          spd = cwt.c->landparam / 10.;
          }
        }
      
      if(c->master->alt && cwt.c->master->alt &&
        (cwt.c->land == laMountain || 
        (conformal::do_rotate >= 2 && 
          (cwt.c->land == laTemple || cwt.c->land == laWhirlpool || 
          (cheater && (cwt.c->land == laClearing || cwt.c->land == laCaribbean ||
          cwt.c->land == laCamelot || cwt.c->land == laPalace))) 
          ))
        && c->land == cwt.c->land && c->master->alt->alt == cwt.c->master->alt->alt) {
        if(!straightDownSeek || !straightDownSeek->master->alt || celldistAlt(c) < celldistAlt(straightDownSeek)) {
          usethis = true;
          spd = .5;
          if(cwt.c->land == laMountain) rev = true;
          }
        }
  
      if(conformal::do_rotate >= 2 && cwt.c->land == laOcean && cwt.c->landparam < 25) {
        if(!straightDownSeek || coastval(c, laOcean) < coastval(straightDownSeek, laOcean)) {
          usethis = true;
          spd = cwt.c->landparam / 10;
          }
          
        }

      if(usethis) {
        straightDownSeek = c;
        downspin = atan2(VC0[1], VC0[0]);
        downspin -= M_PI/2;
        if(rev) downspin += M_PI;
        downspin += M_PI/180 * conformal::rotation;
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
      int col = darkena(0x505050, 0, 0xFF);
      queueline(tC0(V), V*tC0(heptmove[c->bardir]), col, 2);
      queueline(tC0(V), V*tC0(hexmove[c->bardir]), col, 2);
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
  int color;
  flashdata(int _t, int _s, cell *_w, int col, int sped) { 
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
void drawParticle(cell *c, int col, int maxspeed) {
  if(vid.particles && !confusingGeometry())
    flashes.push_back(flashdata(ticks, rand() % 16, c, col, 1+rand() % maxspeed)); 
  }
void drawParticles(cell *c, int col, int qty, int maxspeed) { 
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

void queuecircleat(cell *c, double rad, int col) {
  if(!c) return;
  if(!gmatrix.count(c)) return;
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
  
  for(cell *c1: crush_now) 
    queuecircleat(c1, .8, darkena(minf[moCrusher].color, 0, 0xFF));

  if(!inHighQual) {

  bool ok = !ISPANDORA || mousepressed;
     
    if(G(dragon::target) && haveMount()) {
      queuechr(Gm0(dragon::target), 2*vid.fsize, 'X', 
        gradient(0, iinf[itOrbDomination].color, -1, sin(ticks/(dragon::whichturn == turncount ? 75. : 150.)), 1));
      }

    /* for(int i=0; i<12; i++) if(c->type == 5 && c->master == &dodecahedron[i])
      queuechr(xc, yc, sc, 4*vid.fsize, 'A'+i, iinf[itOrbDomination].color); */
    
    {
    using namespace yendor;
    if(yii < size(yi) && !yi[yii].found) {
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
        hyperpoint H = tC0(shmup::ggmatrix(keycell));
        queuechr(H, 2*vid.fsize, 'X', 0x10101 * int(128 + 100 * sin(ticks / 150.)));
        queuestr(H, vid.fsize, its(celldistance(cwt.c, yi[yii].key())), 0x10101 * int(128 - 100 * sin(ticks / 150.)));
        addauraspecial(H, iinf[itOrbYendor].color, 0);
        }
      }
    }
    
    if(lmouseover && vid.drawmousecircle && ok && DEFAULTCONTROL && MOBON) {
      queuecircleat(lmouseover, .8, darkena(lmouseover->cpdist > 1 ? 0x00FFFF : 0xFF0000, 0, 0xFF));
      }

    if(global_pushto && vid.drawmousecircle && ok && DEFAULTCONTROL && MOBON) {
      queuecircleat(global_pushto, .6, darkena(0xFFD500, 0, 0xFF));
      }

#if CAP_SDLJOY
    if(joydir.d >= 0) 
      queuecircleat(cwt.c->mov[(joydir.d+cwt.spin) % cwt.c->type], .78 - .02 * sin(ticks/199.0), 
        darkena(0x00FF00, 0, 0xFF));
#endif

    bool m = true;
#if CAP_MODEL
    m = netgen::mode == 0;
#endif
    if(centerover.c && !playermoved && m && !conformal::on)
      queuecircleat(centerover.c, .70 - .06 * sin(ticks/200.0), 
        darkena(int(175 + 25 * sin(ticks / 200.0)), 0, 0xFF));

    if(multi::players > 1 || multi::alwaysuse) for(int i=0; i<numplayers(); i++) {
      multi::cpid = i;
      if(multi::players == 1) multi::player[i] = cwt;
      cell *ctgt = multi::multiPlayerTarget(i);
      queuecircleat(ctgt, .40 - .06 * sin(ticks/200.0 + i * 2 * M_PI / numplayers()), getcs().uicolor);
      }

    // process mouse

    if((vid.axes == 4 || (vid.axes == 1 && !mousing)) && !shmup::on) {
      if(multi::players == 1) {
        forCellAll(c2, cwt.c) IG(c2) drawMovementArrows(c2, Gm(c2));
        }
      else if(multi::players > 1) for(int p=0; p<multi::players; p++) {
        if(multi::playerActive(p) && (vid.axes == 4 || !drawstaratvec(multi::mdx[p], multi::mdy[p]))) 
        forCellAll(c2, multi::player[p].c) IG(c2) {
          multi::cpid = p;
          dynamicval<transmatrix> ttm(cwtV, multi::whereis[p]);
          dynamicval<cellwalker> tcw(cwt, multi::player[p]);
          drawMovementArrows(c2, Gm(c2));
          }
        }
      }
    }

  monsterToSummon = moNone;
  orbToTarget = itNone;

  if(mouseover && targetclick) {
    shmup::cpid = 0;
    orbToTarget = targetRangedOrb(mouseover, roCheck);
    if(orbToTarget == itOrbSummon) {
      monsterToSummon = summonedAt(mouseover);
      queuechr(mousex, mousey, 0, vid.fsize, minf[monsterToSummon].glyph, minf[monsterToSummon].color);
      queuecircleat(mouseover, 0.6, darkena(minf[monsterToSummon].color, 0, 0xFF));
      }
    else if(orbToTarget) {
      queuechr(mousex, mousey, 0, vid.fsize, '@', iinf[orbToTarget].color);
      queuecircleat(mouseover, 0.6, darkena(iinf[orbToTarget].color, 0, 0xFF));
      }
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
    }  
  }

void drawFlashes() {
  for(int k=0; k<size(flashes); k++) {
    flashdata& f = flashes[k];
    transmatrix V;
    
    if(f.spd) try { V = gmatrix.at(f.where); } catch(out_of_range) { 
      f = flashes[size(flashes)-1];
      flashes.pop_back(); k--;
      continue; 
      }
    else V = shmup::ggmatrix(f.where);

    int tim = ticks - f.t;
    
    bool kill = tim > f.size;
    
    if(f.spd) {
      kill = tim > 300;
      int partcol = darkena(f.color, 0, max(255 - tim*255/300, 0));
      poly_outline = OUTLINE_DEFAULT;
      ld gps = gp::on ? gp::scale * 1.6 : 1;
      queuepoly(V * spin(f.angle) * xpush(f.spd * tim * gps / 50000.), shParticle[f.size], partcol);
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
        for(int a=0; a<=S84; a++) curvepoint(V*ddi0(a, rad));
        queuecurve(flashcol, 0x8080808, PPR_LINE);
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
        for(int a=0; a<=S84; a++) curvepoint(V*ddi0(a, rad));
        queuecurve(flashcol, 0x8080808, PPR_LINE);
        }
      }

    if(kill) {
      f = flashes[size(flashes)-1];
      flashes.pop_back(); k--;
      }
    }
  }

bool allowIncreasedSight() {
  if(cheater || autocheat) return true;
  if(peace::on) return true;
#if CAP_TOUR
  if(tour::on) return true;
#endif
#if CAP_ROGUEVIZ
  if(rogueviz::on) return true;
#endif
  if(randomPatternsMode) return true;
  if(quotient || !hyperbolic) return true;
  return false;
  }

bool allowChangeRange() {
  if(cheater || peace::on || randomPatternsMode) return true;
#if CAP_TOUR
  if(tour::on) return true;
#endif
#if CAP_ROGUEVIZ
  if(rogueviz::on) return true;
#endif
  if(sightrange_bonus >= 0) return true;
  return false;
  }

purehookset hooks_drawmap;

transmatrix cview() {
  sphereflip = Id;
  if(sphereflipped()) sphereflip[2][2] = -1;
  return ypush(vid.yshift) * sphereflip * View;
  }

void drawthemap() {

  callhooks(hooks_drawmap);

  frameid++;
  
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

  DEBB(DF_GRAPH, (debugfile,"draw the map\n"));
  fanframe = ticks / (nonbitrunc ? 1000. : 500. ); // (nonbitrunc ? 300 : 150.0) / M_PI;
  
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
  if(!euclid) centerover.c = NULL; 

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
  if(euclid)
    drawEuclidean();
  else
    drawrec(viewctr, hsOrigin, cview());
  drawWormSegments();
  drawBlizzards();
  drawArrowTraps();
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
    lmouseover = mousedest.d >= 0 ? cwt.c->mov[(cwt.spin + mousedest.d) % cwt.c->type] : cwt.c;
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

  if(euclid) 
    Centered = eupush(H[0], H[1]);
  else if(R > 1e-9) Centered = rgpushxto0(H);
  
  Centered = Centered * rgpushxto0(hpxy(dx*5, dy*5));
  if(multi::cpid >= 0) multi::crosscenter[multi::cpid] = Centered;
  
  int rax = vid.axes;
  if(rax == 1) rax = drawstaratvec(dx, dy) ? 2 : 0;
  
  if(rax == 0 || vid.axes == 4) return;

  int starcol = getcs().uicolor;

#if CAP_POLY  
  if(vid.axes == 3)
    queuepoly(Centered, shMovestar, starcol);
#endif
  
  else for(int d=0; d<8; d++) {
    int col = starcol;
#if ISPANDORA
    if(leftclick && (d == 2 || d == 6 || d == 1 || d == 7)) col &= 0xFFFFFF3F;
    if(rightclick && (d == 2 || d == 6 || d == 3 || d == 5)) col &= 0xFFFFFF3F;
    if(!leftclick && !rightclick && (d&1)) col &= 0xFFFFFF3F;
#endif
//  EUCLIDEAN

#if CAP_QUEUE
    if(euclid)
      queueline(tC0(Centered), Centered * ddi0(d * 10.5, 0.5) , col, 0);
    else
//    queueline(tC0(Centered), Centered * spin(M_PI*d/4)* xpush(d==0?.7:d==2?.6:.5) * C0, col >> darken);
      queueline(tC0(Centered), Centered * xspinpush0(M_PI*d/4, d==0?.7:d==2?.5:.2), col, 3);
#endif
    }
  }

// old style joystick control

bool sidescreen;

bool dronemode;

void calcparam() {
  DEBB(DF_GRAPH, (debugfile,"calc param\n"));
  vid.xcenter = vid.xres / 2;
  vid.ycenter = vid.yres / 2;
  
  int realradius = min(vid.xcenter, vid.ycenter);
  
  vid.scrsize = realradius - (inHighQual ? 0 : ISANDROID ? 2 : ISIOS ? 40 : 40);

  sidescreen = false;
  
  if(vid.xres < vid.yres - 2 * vid.fsize && !inHighQual) {
    vid.ycenter = vid.yres - vid.scrsize - vid.fsize;
    }
  else {
    if(vid.xres >= vid.yres * 5/4-16 && (cmode & sm::SIDE))
      sidescreen = true;
#if CAP_TOUR
    if(tour::on && (tour::slides[tour::currentslide].flags & tour::SIDESCREEN))
      sidescreen = true;
#endif

    if(sidescreen) vid.xcenter = vid.yres/2;
    }

  vid.radius = vid.scale * vid.scrsize;  
  realradius = min(realradius, vid.radius);
  
  if(dronemode) { vid.ycenter -= vid.radius; vid.ycenter += vid.fsize/2; vid.ycenter += vid.fsize/2; vid.radius *= 2; }
  
  vid.xcenter += vid.scrsize * vid.xposition;
  vid.ycenter += vid.scrsize * vid.yposition;

  stereo::tanfov = tan(stereo::fov * M_PI / 360);
  
  if(pmodel) 
    stereo::scrdist = vid.xres / 2 / stereo::tanfov;
  else 
    stereo::scrdist = vid.radius;
  stereo::scrdist_text = stereo::scrdist;
  }

int ringcolor = darkena(0xFF, 0, 0xFF);

function<void()> wrap_drawfullmap = drawfullmap;

bool force_sphere_outline = false;

void drawfullmap() {

  DEBB(DF_GRAPH, (debugfile,"draw full map\n"));
    
  ptds.clear();

  if(pmodel == mdTwoPoint) {
    queuechr(xpush(+vid.twopoint_param) * C0, vid.xres / 100, 'X', 0xFF0000);
    queuechr(xpush(-vid.twopoint_param) * C0, vid.xres / 100, 'X', 0xFF0000);
    }
  
  if(!twopoint_do_flips && !stereo::active() && sphere && pmodel == mdTwoPoint) {
    queuereset(vid.usingGL ? mdDisk : mdUnchanged, PPR_CIRCLE);

    for(int b=-1; b<=1; b+=2)
    for(int a=-90; a<=90; a++) {
      using namespace hyperpoint_vec;
      ld x = sin(a * vid.twopoint_param * b / 90);
      ld y = 0;
      ld z = -sqrt(1 - x*x);
      hyperpoint h1;
      applymodel(hpxyz(x,y,z), h1);
      
      h1[1] = abs(h1[1]) * b;
      curvepoint(h1 * vid.radius);
      }

    queuecurve(ringcolor, 0, PPR_CIRCLE);
    queuereset(pmodel, PPR_CIRCLE);
    }
  
  if(!stereo::active() && sphere && pmodel == mdSinusoidal) {
    queuereset(vid.usingGL ? mdDisk : mdUnchanged, PPR_CIRCLE);
    
    for(int a=-45; a<45; a++) {
      curvepoint(hpxyz(cos(a * M_PI / 90) * vid.radius, a * vid.radius / 90, 0));
      }
    for(int a=45; a>=-45; a--) {
      curvepoint(hpxyz(-cos(a * M_PI / 90) * vid.radius, a * vid.radius / 90, 0));
      }
    queuecurve(ringcolor, 0, PPR_CIRCLE);

    queuereset(pmodel, PPR_CIRCLE);
    }

  if(!stereo::active() && vid.grid) {
    ld rad = 0;
    if(pmodel == mdBand && hyperbolic) rad = vid.radius;
    if(pmodel == mdBandEquidistant && sphere) rad = vid.radius / 2;
    if(pmodel == mdBandEquiarea && sphere) rad = vid.radius / M_PI;
    
    if(rad) {
      queuereset(vid.usingGL ? mdDisk : mdUnchanged, PPR_CIRCLE);
      curvepoint(hpxyz(-vid.xcenter, -rad, 0));
      curvepoint(hpxyz(vid.xres-vid.xcenter, -rad, 0));
      queuecurve(ringcolor, 0, PPR_CIRCLE);
      curvepoint(hpxyz(-vid.xcenter, rad, 0));
      curvepoint(hpxyz(vid.xres-vid.xcenter, rad, 0));
      queuecurve(ringcolor, 0, PPR_CIRCLE);
      queuereset(pmodel, PPR_CIRCLE);
      }
    }

  if(!stereo::active() && !euclid && (pmodel == mdDisk || pmodel == mdBall || (sphere && mdAzimuthalEqui()))) {
    double rad = vid.radius;
    bool isbnd = true;
    if(sphere) {
      if(mdAzimuthalEqui())
        ;
      else if(!vid.grid && !elliptic && !force_sphere_outline)
        rad = 0; 
      else if(vid.alpha <= 0)
        ;
      else if(vid.alpha <= 1 && (vid.grid || elliptic)) // mark the equator
        rad = rad * 1 / vid.alpha, isbnd = false;
      else if(vid.grid || force_sphere_outline) // mark the edge
        rad /= sqrt(vid.alpha*vid.alpha - 1);
      }
    if(rad && !haveaura()) {
      queuecircle(vid.xcenter, vid.ycenter, rad, ringcolor, 
        vid.usingGL ? PPR_CIRCLE : PPR_OUTCIRCLE);
      if(isbnd) lastptd().u.cir.boundary = true;
      }
    if(pmodel == mdBall) ballgeometry();
    }
  
  if(pmodel == mdHyperboloid) {
#if CAP_QUEUE
    int col = darkena(0x80, 0, 0x80);
    queueline(hpxyz(0,0,1), hpxyz(0,0,-vid.alpha), col, 0, PPR_CIRCLE);
    queueline(xpush(+4)*C0, hpxyz(0,0,0), col, 0, PPR_CIRCLE);
    queueline(xpush(+4)*C0, hpxyz(0,0,-vid.alpha), col, 0, PPR_CIRCLE);
    queueline(xpush(-4)*C0, hpxyz(0,0,0), col, 0, PPR_CIRCLE);
    queueline(xpush(-4)*C0, hpxyz(0,0,-vid.alpha), col, 0, PPR_CIRCLE);
    queueline(hpxyz(-1,0,0), hpxyz(1,0,0), col, 0, PPR_CIRCLE);
#endif
    }
  
  if(pmodel == mdPolygonal || pmodel == mdPolynomial) 
    polygonal::drawBoundary(darkena(0xFF, 0, 0xFF));
  
  /* if(vid.wallmode < 2 && !euclid && !patterns::whichShape) {
    int ls = size(lines);
    if(ISMOBILE) ls /= 10;
    for(int t=0; t<ls; t++) queueline(View * lines[t].P1, View * lines[t].P2, lines[t].col >> (darken+1));
    } */

  clearaura();
  drawthemap();
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
  drawqueue();
  profile_stop(2);
  }

#ifdef ISMOBILE
int andmode;
#endif

void gamescreen(int _darken) {

  if(ISMOBILE && (cmode & sm::SIDE)) {
    cmode ^= sm::SIDE;
    _darken += 2;
    }
  
  if((cmode & sm::MAYDARK) && !sidescreen) {
    _darken += 2;
    }

  darken = _darken;
  
  if(conformal::includeHistory) conformal::restore();

#if CAP_RUG
  if(rug::rugged) {
    rug::actDraw();
    } else
#endif
  wrap_drawfullmap();

  if(conformal::includeHistory) conformal::restoreBack();

  poly_outline = OUTLINE_DEFAULT;
  
  #if ISMOBILE
  
  buttonclicked = false;
  
  if((cmode & sm::NORMAL) && stereo::mode != stereo::sLR) {
    if(andmode == 0 && shmup::on) {
      using namespace shmupballs;
      calc();
      drawCircle(xmove, yb, rad, OUTLINE_FORE);
      drawCircle(xmove, yb, rad/2, OUTLINE_FORE);
      drawCircle(xfire, yb, rad, 0xFF0000FF);
      drawCircle(xfire, yb, rad/2, 0xFF0000FF);
      }
    else {
      if(andmode != 0) displayabutton(-1, +1, XLAT("MOVE"),  andmode == 0 ? BTON : BTOFF);
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

#if CAP_ROGUEVIZ
  if(!rogueviz::on)
#endif
  mouseovers = XLAT("Press F1 or right click for help");

#if CAP_TOUR  
  if(tour::on) mouseovers = tour::tourhelp;
#endif

  if(!outofmap(mouseh)) getcstat = '-';
  cmode = sm::NORMAL | sm::DOTOUR | sm::CENTER;
  if(viewdists) cmode |= sm::SIDE;
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

  describeMouseover();
  }

vector< function<void()> > screens = { normalscreen };

int cmode;

void drawscreen() {

  if(vid.xres == 0 || vid.yres == 0) return;

  DEBB(DF_GRAPH, (debugfile,"drawscreen\n"));

  calcparam();
  // rug::setVidParam();
  #if CAP_ROGUEVIZ
  rogueviz::fixparam();
  #endif

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
   
  // displaynum(vx,100, 0, 24, 0xc0c0c0, celldist(cwt.c), ":");
  
  lgetcstat = getcstat;
  getcstat = 0; inslider = false;
  
  mouseovers = " ";

  cmode = 0;
  keyhandler = [] (int sym, int uni) { return false; };
  if(!size(screens)) pushScreen(normalscreen);
  screens.back()();

#if !ISMOBILE
  int col = linf[cwt.c->land].color;
  if(cwt.c->land == laRedRock) col = 0xC00000;
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
    for(int i=0; i<c->type; i++) if(c->mov[i]) {
      if(c->mov[i]->land == laMinefield) 
        minefieldNearby = true;
      if(c->mov[i]->wall == waMineMine) {
        bool ep = false;
        if(!ep) mines[p]++, tmines++;
        }
      }
    }

  if((minefieldNearby || tmines) && !items[itOrbAether] && darken == 0 && normal) {
    string s;
    if(tmines > 7) tmines = 7;
    int col = minecolors[tmines];
    
    if(tmines == 7) seenSevenMines = true;
    
    for(int p=0; p<numplayers(); p++) if(multi::playerActive(p))
      displayfr(vid.xres * (p+.5) / numplayers(),
        vid.ycenter - vid.radius * 3/4, 2,
        vid.fsize, 
        XLAT(minetexts[mines[p]]), minecolors[mines[p]], 8);

    if(minefieldNearby && !shmup::on && cwt.c->land != laMinefield && cwt.c->mov[cwt.spin]->land != laMinefield) {
      displayfr(vid.xres/2, vid.ycenter - vid.radius * 3/4 - vid.fsize*3/2, 2,
        vid.fsize, 
        XLAT("WARNING: you are entering a minefield!"), 
        col, 8);
      }
    }

  // SDL_UnlockSurface(s);

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
  linepatterns::clearAll();
  if(currentmap) {
    if(euclid) {
      centerover = vec_to_cellwalker(0);
      }
    else {
      viewctr.h = currentmap->getOrigin();
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
  mouseover = centerover.c = lmouseover = NULL;  
  gmatrix.clear(); gmatrix0.clear();
  clearAnimations();
  });

void resetGeometry() {
  precalc();
  currfp.analyze();
#if CAP_GL
  resetGL();
#endif
  }

//=== animation

map<cell*, animation> animations[ANIMLAYERS];
unordered_map<cell*, transmatrix> gmatrix, gmatrix0;

void animateMovement(cell *src, cell *tgt, int layer) {
  if(vid.mspeed >= 5) return; // no animations!
  if(confusingGeometry()) return;
  if(gmatrix.count(src) && gmatrix.count(tgt)) {
    animation& a = animations[layer][tgt];
    if(animations[layer].count(src)) {
      a = animations[layer][src];
      a.wherenow = inverse(gmatrix[tgt]) * gmatrix[src] * a.wherenow;
      animations[layer].erase(src);
      a.attacking = 0;
      }
    else {
      a.ltick = ticks;
      a.wherenow = inverse(gmatrix[tgt]) * gmatrix[src];
      a.footphase = 0;
      }
    }
  }

void animateAttack(cell *src, cell *tgt, int layer) {
  if(vid.mspeed >= 5) return; // no animations!
  if(gmatrix.count(src) && gmatrix.count(tgt)) {
    bool newanim = !animations[layer].count(src);
    animation& a = animations[layer][src];
    a.attacking = 1;
    a.attackat = rspintox(tC0(inverse(gmatrix[src]) * gmatrix[tgt])) * xpush(hdist(gmatrix[src]*C0, gmatrix[tgt]*C0) / 3);
    if(newanim) a.wherenow = Id, a.ltick = ticks, a.footphase = 0;
    }
  }

vector<pair<cell*, animation> > animstack;

void indAnimateMovement(cell *src, cell *tgt, int layer) {
  if(vid.mspeed >= 5) return; // no animations!
  if(confusingGeometry()) return;
  if(animations[layer].count(tgt)) {
    animation res = animations[layer][tgt];
    animations[layer].erase(tgt);
    animateMovement(src, tgt, layer);
    if(animations[layer].count(tgt)) 
      animstack.push_back(make_pair(tgt, animations[layer][tgt]));
    animations[layer][tgt] = res;
    }
  else {
    animateMovement(src, tgt, layer);
    if(animations[layer].count(tgt)) {
      animstack.push_back(make_pair(tgt, animations[layer][tgt]));
      animations[layer].erase(tgt);
      }
    }
  }

void commitAnimations(int layer) {
  for(int i=0; i<size(animstack); i++)
    animations[layer][animstack[i].first] = animstack[i].second;
  animstack.clear();
  }

void animateReplacement(cell *a, cell *b, int layer) {
  if(vid.mspeed >= 5) return; // no animations!
  static cell c1;
  gmatrix[&c1] = gmatrix[b];
  if(animations[layer].count(b)) animations[layer][&c1] = animations[layer][b];
  animateMovement(a, b, layer);
  animateMovement(&c1, a, layer);
  }

void drawBug(const cellwalker& cw, int col) {
#if CAP_POLY
  initquickqueue();
  transmatrix V = shmup::ggmatrix(cw.c);
  if(cw.spin) V = V * ddspin(cw.c, cw.spin, S42);
  queuepoly(V, shBugBody, col);
  quickqueue();
#endif
  }

cell *viewcenter() {
  if(euclid) return centerover.c;
  else return viewctr.h->c7;
  }

bool inscreenrange(cell *c) {
  if(sphere) return true;
  if(euclid) return celldistance(viewcenter(), c) <= get_sightrange_ambush();
  return heptdistance(viewcenter(), c) <= 5;
  }

