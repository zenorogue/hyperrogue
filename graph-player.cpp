// Hyperbolic Rogue -- player/mimic graphics file
// Copyright (C) 2011-2025 Zeno Rogue, see 'hyper.cpp' for details

#include "hyper.h"
namespace hr {

EX int lightat, safetyat;
EX void drawLightning() { lightat = ticks; }
EX void drawSafety() { safetyat = ticks; }

EX void drawShield(const shiftmatrix& V, eItem it) {
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
    queueball(V * lzpush(cgi.GROIN1), cgi.human_height / 2, darkena(col, 0, 0xFF), itOrbShield);
#else
  if(1) ;
#endif  
  else {
    for(ld a=0; a<=cgi.S84*mt+1e-6; a+=pow(.5, vid.linequality))
      curvepoint(xspinpush0(a * cgi.S_step, d + sin(ds + 90._deg*a/mt)*.1));
    queuecurve(V, darkena(col, 0, 0xFF), 0x8080808, PPR::LINE);
    }
#endif
  }

EX void drawSpeed(const shiftmatrix& V, ld scale IS(1)) {
#if CAP_CURVE
  ld ds = ptick(10);
  color_t col = darkena(iinf[itOrbSpeed].color, 0, 0xFF);
#if MAXMDIM >= 4
  if(GDIM == 3) queueball(V * lzpush(cgi.GROIN1), cgi.human_height * 0.55, col, itOrbSpeed);
  else
#endif
  for(int b=0; b<cgi.S84; b+=cgi.S14) {
    PRING(a)
      curvepoint(xspinpush0((ds+b+a) * cgi.S_step, cgi.hexf*a/cgi.S84*scale));
    queuecurve(V, col, 0x8080808, PPR::LINE);
    }
#endif
  }

EX void drawSafety(const shiftmatrix& V, int ct) {
  if(inHighQual) return;
#if CAP_QUEUE
  ld ds = ptick(50);
  color_t col = darkena(iinf[itOrbSafety].color, 0, 0xFF);
  #if MAXMDIM >= 4
  if(GDIM == 3) {
    queueball(V * lzpush(cgi.GROIN1), 2*cgi.hexf, col, itOrbSafety);
    return;
    }
  #endif
  for(int a=0; a<ct; a++)
    queueline(V*xspinpush0((ds+a*cgi.S84/ct) * cgi.S_step, 2*cgi.hexf), V*xspinpush0((ds+(a+(ct-1)/2)*cgi.S84/ct) * cgi.S_step, 2*cgi.hexf), col, vid.linequality);
#endif
  }

EX void drawFlash(const shiftmatrix& V) {
#if CAP_CURVE
  float ds = ptick(300);
  color_t col = darkena(iinf[itOrbFlash].color, 0, 0xFF);
  col &= ~1;
  for(int u=0; u<5; u++) {
    ld rad = cgi.hexf * (2.5 + .5 * sin(ds+u*.3));
    #if MAXMDIM >= 4
    if(GDIM == 3) {
      queueball(V * lzpush(cgi.GROIN1), rad, col, itOrbFlash);
      }
    #else
    if(1) ;
    #endif
    else {
      PRING(a) curvepoint(xspinpush0(a * cgi.S_step, rad));
      queuecurve(V, col, 0x8080808, PPR::LINE);
      }
    }
#endif
  }

EX void drawLove(const shiftmatrix& V, int hdir) {
#if CAP_CURVE
  float ds = ptick(300);
  color_t col = darkena(iinf[itOrbLove].color, 0, 0xFF);
  col &= ~1;
  for(int u=0; u<5; u++) {
    shiftmatrix V1 = chei(V, u, 5);
    PRING(a) {
      double d = (1 + cos(a * cgi.S_step)) / 2;
      double z = a; if(z>cgi.S42) z = cgi.S84-z;
      if(z <= 10) d += (10-z) * (10-z) * (10-z) / 3000.;

      ld rad = cgi.hexf * (2.5 + .5 * sin(ds+u*.3)) * d;
      curvepoint(xspinpush0((cgi.S42+hdir+a-1) * cgi.S_step, rad));
      }
    queuecurve(V1, col, 0x8080808, PPR::LINE);
    }
#endif
  }

EX void drawWinter(const shiftmatrix& V, ld hdir, color_t col) {
#if CAP_QUEUE
  float ds = ptick(300);
  col = darkena(col, 0, 0xFF);
  for(int u=0; u<20; u++) {
    ld rad = sin(ds+u * TAU / 20) * M_PI / S7;
    shiftmatrix V1 = chei(V, u, 20);
    queueline(V1*xspinpush0(M_PI+hdir+rad, cgi.hexf*.5), V1*xspinpush0(M_PI+hdir+rad, cgi.hexf*3), col, 2 + vid.linequality);
    }
#endif
  }

EX void drawLightning(const shiftmatrix& V) {
#if CAP_QUEUE
  float ds = ptick(600);
  color_t col = darkena(iinf[itOrbLightning].color, 0, 0xFF);
  for(int u=0; u<20; u++) {
    ld leng, rad;
    if(vid.flasheffects) {
      leng = 0.5 / (0.1 + (rand() % 100) / 100.0);
      rad = rand() % 1000;
      }
    else {
      if(u % 5) leng = 1.25 + sintick(200, ld(u) * 1.25) * 0.25;
      else leng = 2 + sintick(200, ld(u) * 1.25);
      rad = (u + ds) * TAU / 20;
      }
    shiftmatrix V1 = chei(V, u, 20);
    queueline(V1*xspinpush0(rad, cgi.hexf*0.3), V1*xspinpush0(rad, cgi.hexf*leng), col, 2 + vid.linequality);
    }
#endif
  }

EX void drawCurse(const shiftmatrix& V, eItem it) {
#if CAP_QUEUE
  float ds = ptick(450) + (int(it) * 5.5); // Extra offset so both Gluttony and Repulsion are easily visible
  color_t col = darkena(iinf[it].color, 0, 0xFF);
  for(int u=0; u<20; u++) {
    ld leng, rad;
    if(vid.flasheffects) {
      leng = 0.6 + 0.3 * randd();
      rad = rand() % 1000;
      }
    else {
      leng = 0.85 + sintick(150, ld(u) * 1.25) * 0.15;
      rad = (u + ds) * TAU / 20;
      }
    shiftmatrix V1 = chei(V, u, 20);
    queueline(V1*xspinpush0(rad, cgi.hexf*0.3), V1*xspinpush0(rad, cgi.hexf*leng), col, 2 + vid.linequality);
    }
#endif
  }

EX void drawPlayerEffects(const shiftmatrix& V, const shiftmatrix& Vparam, cell *c, eMonster m) {
  bool onplayer = m == moPlayer;
  if(!onplayer && !items[itOrbEmpathy]) return;
  if(items[itOrbShield] > (shmup::on ? 0 : ORBBASE)) drawShield(V, itOrbShield);
  if(items[itOrbShell] > (shmup::on ? 0 : ORBBASE)) drawShield(V, itOrbShell);

  if(items[itOrbSpeed]) drawSpeed(V, (items[itOrbSpeed] % 2) ? 1.1 : 0.8);
  if(items[itCurseGluttony]) drawCurse(V, itCurseGluttony); 
  if(items[itCurseRepulsion]) drawCurse(V, itCurseRepulsion); 

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
      shiftmatrix Vsword = 
        shmup::on ? V * shmup::swordmatrix[multi::cpid] * cspin90(2, 0)
                  : Vparam * rgpushxto0(inverse_shift(gmatrix[c], tC0(V))) * sword::dir[multi::cpid].T;

      if(items[itOrbSword])
        queuepoly(Vsword * cspin(1,2, ticks / 150.), (peace::on ? cgi.shMagicShovel : cgi.shMagicSword), darkena(iinf[itOrbSword].color, 0, 0xC0 + 0x30 * sintick(200)));
  
      if(items[itOrbSword2])
        queuepoly(Vsword * lpispin() * cspin(1,2, ticks / 150.), (peace::on ? cgi.shMagicShovel : cgi.shMagicSword), darkena(iinf[itOrbSword2].color, 0, 0xC0 + 0x30 * sintick(200)));
#endif
      }
    
    else {
      int& ang = sword::dir[multi::cpid].angle;
      ang %= sword::sword_angles;

#if CAP_QUEUE || CAP_SHAPES
      shiftmatrix Vnow = Vparam * rgpushxto0(inverse_shift(Vparam, tC0(V))) * ddspin180(c,0);
#endif

      int adj = 1 - ((sword_angles/cwt.at->type)&1);
      
#if CAP_QUEUE
      if(!euclid && !mhybrid) for(int a=0; a<sword_angles; a++) {
        if(a == ang && items[itOrbSword]) continue;
        if((a+sword_angles/2)%sword_angles == ang && items[itOrbSword2]) continue;
        bool longer = sword::pos2(cwt.at, a-1) != sword::pos2(cwt.at, a+1);
        if(sword_angles > 48 && !longer) continue;
        color_t col = darkena(0xC0C0C0, 0, 0xFF);
        ld l0 = PURE ? 0.6 * cgi.scalefactor : longer ? 0.36 : 0.4;
        ld l1 = PURE ? 0.7 * cgi.scalefactor : longer ? 0.44 : 0.42;
#if MAXMDIM >= 4
        hyperpoint h0 = GDIM == 3 ? xpush(l0) * lzpush(cgi.FLOOR - cgi.human_height/50) * C0 : xpush0(l0);
        hyperpoint h1 = GDIM == 3 ? xpush(l1) * lzpush(cgi.FLOOR - cgi.human_height/50) * C0 : xpush0(l1);
#else
        hyperpoint h0 = xpush0(l0);
        hyperpoint h1 = xpush0(l1);
#endif
        shiftmatrix T = Vnow*spin((sword_angles + (-adj-2*a)) * M_PI / sword_angles);
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
    drawWinter(V, 0, iinf[itOrbWinter].color); // displaydir(c, cwt.spin));

  if(items[itOrbFire]) 
    drawWinter(V, 0, iinf[itOrbFire].color); // displaydir(c, cwt.spin));

  if(items[itCurseWater]) 
    drawWinter(V, 0, iinf[itCurseWater].color); // displaydir(c, cwt.spin));
  
  if(onplayer && items[itOrbLightning]) drawLightning(V);
  
  if(safetyat > 0) {
    int tim = ticks - safetyat;
    if(tim > 2500) safetyat = 0;
    for(int u=tim; u<=2500; u++) {
      if((u-tim)%250) continue;
      ld rad = cgi.hexf * u / 250;
      color_t col = darkena(iinf[itOrbSafety].color, 0, 0xFF);
      PRING(a)
        curvepoint(xspinpush0(a * cgi.S_step, rad));
      queuecurve(V, col, 0, PPR::LINE);
      }
    }
  }

#if HDR
struct playershape {
  string name;
  bool is_humanoid;
  bool is_animal;
  };

enum ePlayershape { pshRogue, pshPrincess, pshCat, pshDog, pshFamiliar, pshSpaceship, pshBunny, pshRatling, pshHyperbug, pshSkeleton, pshGUARD };
#endif

EX vector<playershape> playershapes = {
  {"Rogue", true, false},
  {"Princess", true, false},
  {"cat", false, true},
  {"dog", false, true},
  {"Familiar", false, true},
  {"spaceship", false, false},
  {"bunny", false, true},
  {"Ratling", true, false},
  {"hyperbug", false, true},
  {"Skeleton", true, false}
  };

EX void drawPlayer_animal(eMonster m, cell *where, const shiftmatrix& V0, color_t col, double footphase, bool stop IS(false)) {
  charstyle& cs = getcs();

  auto id = ePlayershape(cs.charid >> 1);

  auto V = V0;

  if(id == pshHyperbug) {
    ShadowV(V, cgi.shBugBody);
    if(!mmspatial && !footphase)
      queuepoly(VABODY, cgi.shBugBody, fc(0, cs.skincolor, 0));
    else {
      animallegs(VALEGS, moBug0, fc(500, cs.dresscolor, 4), footphase);
      queuepoly(VABODY, cgi.shBugAntenna, fc(150, cs.skincolor, 2));
      }
    queuepoly(VABODY, cgi.shBugArmor, fc(400, cs.haircolor, 5));
    }

  if(id == pshBunny) {
    auto z2 = WDIM == 3 ? 0 : GDIM == 3 ? -abs(sin(footphase * TAU)) * cgi.human_height/3 : geom3::lev_to_factor(abs(sin(footphase * TAU)) * cgi.human_height * 2);
    auto V0 = V;
    V = at_smart_lof(V0, z2);
    }

  if(id == pshBunny) {
    /* bunny */
    if(!mmspatial && !footphase) {
      if(stop) return;
      queuepoly(VALEGS, cgi.shCatLegs, fc(500, cs.dresscolor, 4));
      }
    else {
      ShadowV(V, cgi.shBunnyBody);
      if(stop) return;
      animallegs(VALEGS, moRunDog, fc(500, cs.dresscolor, 4), footphase);
      }
    queuepoly(VABODY, cgi.shBunnyTail, fc(0, cs.skincolor, 0));
    queuepoly(VABODY, cgi.shBunnyBody, fc(0, cs.skincolor, 0));
    queuepoly(VAHEAD, cgi.shBunnyHead, fc(150, cs.haircolor, 2));
    }
  else if(id == pshFamiliar) {
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
    }
  else if(id == pshDog) {
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
    }
  else if(id == pshCat) {
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
    }

  if(!shmup::on || shmup::curtime >= shmup::getPlayer()->nextshot) {
    color_t col = items[itOrbDiscord] ? watercolor(0) : fc(314, cs.eyecolor, 3);
    if(among(id, pshCat, pshBunny)) {
      queuepoly(VAHEAD * xpush(.04), cgi.shWolf1, col);
      queuepoly(VAHEAD * xpush(.04), cgi.shWolf2, col);
      }
    if(id == pshDog) {
      queuepoly(VAHEAD, cgi.shWolf1, col);
      queuepoly(VAHEAD, cgi.shWolf2, col);
      }
    if(id == pshFamiliar) {
      queuepoly(VAHEAD, cgi.shFamiliarEye, col);
      queuepoly(VAHEAD * lmirror(), cgi.shFamiliarEye, col);
      }
    }

  if(id == pshBunny) {
    queuepoly(VAHEAD, cgi.shBunnyEar, fc(150, cs.haircolor, 2));
    queuepoly(VAHEAD * MirrorY, cgi.shBunnyEar, fc(150, cs.haircolor, 2));
    }

  if(id == pshDog) {
    color_t colnose = items[itOrbDiscord] ? watercolor(0) : fc(314, 0xFF, 3);
    queuepoly(VAHEAD, cgi.shWolf3, colnose);
    }

  #if CAP_COMPLEX2
  if(camelot::knighted)
    queuepoly(VABODY, cgi.shKnightCloak, darkena(cloakcolor(camelot::knighted), 1, 0xFF));
  #endif

  if(tortoise::seek())
    tortoise::draw(VABODY, tortoise::seekbits, 4, 0);
  }

EX void drawPlayer_humanoid(eMonster m, cell *where, const shiftmatrix& V, color_t col, double footphase, bool stop IS(false)) {
  charstyle& cs = getcs();
  auto id = ePlayershape(cs.charid >> 1);

  auto& body = (id == pshRatling) ? cgi.shYeti : (id == pshSkeleton) ? cgi.shSkeletonBody : (cs.charid&1) ? cgi.shFemaleBody : cgi.shPBody;

  ShadowV(V, body);
  if(stop) return;

  const transmatrix VBS = otherbodyparts(V, fc(0, (id == pshRatling || id == pshSkeleton) ? cs.dresscolor : cs.skincolor, 0), items[itOrbFish] ? moWaterElemental : moPlayer, footphase);


  queuepoly(VBODY * VBS, body, fc(0, cs.skincolor, 0));

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
    queuepoly(VBODY * VBS * lmirror(), cgi.shBullHorn, items[itOrbDiscord] ? watercolor(0) : 0xFF000040);
    }

  if(items[itOrbSide1] && !shmup::on)
    queuepoly(VBODY * VBS * spin(-15._deg), cs.charid >= 2 ? cgi.shSabre : cgi.shPSword, fc(314, cs.swordcolor, 3)); // 3 not colored
  
  shiftmatrix VWPN = cs.lefthanded ? VBODY * VBS * lmirror() : VBODY * VBS;
  
  if(peace::on) ;
  else if(racing::on) {
#if CAP_RACING
    if(racing::trophy[multi::cpid])
      queuepoly(VWPN, cgi.shTrophy, racing::trophy[multi::cpid]);
#endif
    }
  else if(bow::crossbow_mode() && cs.charid < 4) {
    queuepoly(VWPN, cgi.shCrossbow, fc(314, cs.bowcolor, 3));
    int ti = items[itCrossbow];
    if(shmup::on) {
      ti = shmup::getPlayer()->nextshot - shmup::curtime;
      if(ti <= 0) ti = 0;
      else ti = 1 + ti / 500;
      }
    shiftmatrix VWPN1 = VWPN, VWPN2 = VWPN;
    if(GDIM == 3) { ld h = cgi.human_height; VWPN1 = VWPN * lzpush(-h/60); VWPN2 = VWPN * lzpush(-h/30); }
    if(ti <= 1) queuepoly(VWPN1, cgi.shCrossbowstringLoaded, fc(314, cs.bowcolor2, 3));
    else if(ti <= 2) queuepoly(VWPN1, cgi.shCrossbowstringSemiloaded, fc(314, cs.bowcolor2, 3));
    else queuepoly(VWPN1, cgi.shCrossbowstringUnloaded, fc(314, cs.bowcolor2, 3));
    if(ti == 0) queuepoly(VWPN2, cgi.shCrossbowBolt, fc(314, cs.swordcolor, 3));
    }
  else if(items[itOrbThorns])
    queuepoly(VWPN, cgi.shHedgehogBladePlayer, items[itOrbDiscord] ? watercolor(0) : 0x00FF00FF);
  else if(items[itRevolver])
    queuepoly(VWPN, cgi.shGunInHand, fc(314, cs.swordcolor, 3)); // 3 not colored
  else if(items[itOrbSlaying]) {
    queuepoly(VWPN, cgi.shFlailTrunk, items[itOrbDiscord] ? watercolor(0) : fc(314, cs.swordcolor, 3));
    queuepoly(VWPN, cgi.shHammerHead, items[itOrbDiscord] ? watercolor(50) : fc(314, cs.swordcolor, 3));
    }
  else if(!shmup::on && items[itOrbDiscord])
    queuepoly(VWPN, cs.charid >= 2 ? cgi.shSabre : cgi.shPSword, watercolor(0));
  else if(items[itCurseWeakness]) {
    /* no weapon shown */
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
  else if(id == pshRatling) {
    queuepoly(VHEAD, cgi.shRatHead, fc(500, cs.haircolor, 1));
    queuepoly(VHEAD, cgi.shWolf1, cs.eyecolor);
    queuepoly(VHEAD, cgi.shWolf2, cs.eyecolor);
    queuepoly(VHEAD, cgi.shWolf3, darkena(0x202020, 0, 0xFF));
    }
  else if(id == pshSkeleton) {
    if(GDIM == 2) queuepoly(VHEAD, cgi.shSkull, fc(500, cs.haircolor, 1));
    if(GDIM == 2) queuepoly(VHEAD1, cgi.shSkullEyes, 0x000000FF);
    }
  else {
    queuepoly(VHEAD, cgi.shPFace, fc(500, cs.skincolor, 1));
    queuepoly(VHEAD1, (cs.charid&1) ? cgi.shFemaleHair : cgi.shPHead, fc(150, cs.haircolor, 2));
    }

  humanoid_eyes(V, cs.eyecolor, cs.skincolor);

  #if CAP_COMPLEX2
  if(camelot::knighted)
    queuepoly(VBODY * VBS, id == pshRatling ? cgi.shRatCape1 : cgi.shKnightCloak, darkena(cloakcolor(camelot::knighted), 1, 0xFF));
  #endif

  if(tortoise::seek())
    tortoise::draw(VBODY * VBS * ypush(-cgi.crossf*.25), tortoise::seekbits, 4, 0);
  }

EX void drawPlayer(eMonster m, cell *where, const shiftmatrix& V, color_t col, double footphase, bool stop IS(false)) {
  charstyle& cs = getcs();

  if(GDIM == 3) {
    addradar(V, '@', cs.uicolor >> 8, 0xFF00FF00);
    }

  if(mapeditor::drawplayer && !mapeditor::drawUserShape(V, mapeditor::sgPlayer, cs.charid, cs.skincolor, where)) {

    auto id = ePlayershape(cs.charid >> 1);
    if(playershapes[id].is_animal) drawPlayer_animal(m, where, V, col, footphase, stop);
    else if(playershapes[id].is_humanoid) drawPlayer_humanoid(m, where, V, col, footphase, stop);
  
    else if(cs.charid >= 10) {
      ShadowV(V, cgi.shSpaceship);
      queuepoly(VBODY, cgi.shSpaceshipBase, fc(150, cs.skincolor, 4));
      queuepoly(VBODY, cgi.shSpaceshipCockpit, fc(150, cs.eyecolor, 4));
      queuepoly(VBODY, cgi.shSpaceshipGun, fc(150, cs.dresscolor, 4));
      queuepoly(VBODY, cgi.shSpaceshipEngine, fc(150, cs.haircolor, 4));
      queuepoly(VBODY * lmirror(), cgi.shSpaceshipGun, fc(150, cs.dresscolor, 4));
      queuepoly(VBODY * lmirror(), cgi.shSpaceshipEngine, fc(150, cs.haircolor, 4));
      }
    }
  }

EX void drawMimic(eMonster m, cell *where, const shiftmatrix& V, color_t col, double footphase) {
  charstyle& cs = getcs();
  auto id = ePlayershape(cs.charid >> 1);
  
  if(mapeditor::drawUserShape(V, mapeditor::sgPlayer, cs.charid, darkena(col, 0, 0x80), where)) return;
  
  if(id == pshSpaceship) {
    ShadowV(V, cgi.shSpaceship);
    queuepoly(VBODY, cgi.shSpaceshipBase, darkena(col, 0, 0xC0));
    queuepoly(VBODY, cgi.shSpaceshipCockpit, darkena(col, 0, 0xC0));
    queuepoly(VBODY, cgi.shSpaceshipGun, darkena(col, 0, 0xC0));
    queuepoly(VBODY, cgi.shSpaceshipEngine, darkena(col, 0, 0xC0));
    queuepoly(VBODY * lmirror(), cgi.shSpaceshipGun, darkena(col, 0, 0xC0));
    queuepoly(VBODY * lmirror(), cgi.shSpaceshipEngine, darkena(col, 0, 0xC0));
    }
  else if(id == pshFamiliar) {
    queuepoly(VABODY, cgi.shWolfBody, darkena(col, 0, 0xC0));
    ShadowV(V, cgi.shWolfBody);

    if(mmspatial || footphase)
      animallegs(VALEGS, moWolf, darkena(col, 0, 0xC0), footphase);
    else 
      queuepoly(VABODY, cgi.shWolfLegs, darkena(col, 0, 0xC0));

    queuepoly(VABODY, cgi.shFamiliarHead, darkena(col, 0, 0xC0));
    queuepoly(VAHEAD, cgi.shFamiliarEye, darkena(col, 0, 0xC0));
    queuepoly(VAHEAD * lmirror(), cgi.shFamiliarEye, darkena(col, 0, 0xC0));
    }
  else if(id == pshDog) {
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
  else if(id == pshBunny) {
    ShadowV(V, cgi.shBunnyBody);
    queuepoly(VAHEAD, cgi.shBunnyHead, darkena(col, 0, 0xC0));
    if(mmspatial || footphase) {
      animallegs(VALEGS, moRunDog, darkena(col, 0, 0xC0), footphase);
      queuepoly(VABODY, cgi.shBunnyBody, darkena(col, 0, 0xC0));
      }
    else
      queuepoly(VABODY, cgi.shBunnyBody, darkena(col, 0, 0xC0));

    queuepoly(VABODY, cgi.shBunnyTail, darkena(col, 1, 0xC0));
    queuepoly(VABODY, cgi.shBunnyHead, darkena(col, 1, 0xC0));

    queuepoly(VAHEAD * xpush(.04), cgi.shWolf1, col);
    queuepoly(VAHEAD * xpush(.04), cgi.shWolf2, col);

    queuepoly(VAHEAD, cgi.shBunnyEar, darkena(col, 1, 0x80));
    queuepoly(VAHEAD * MirrorY, cgi.shBunnyEar, darkena(col, 2, 0x80));
    }
  else if(id == pshHyperbug) {
    ShadowV(V, cgi.shBugBody);
    if(!mmspatial && !footphase)
      queuepoly(VABODY, cgi.shBugBody, darkena(col, 0, 0xC0));
    else {
      animallegs(VALEGS, moBug0, darkena(col, 0, 0xC0), footphase);
      queuepoly(VABODY, cgi.shBugAntenna, darkena(col, 0, 0xC0));
      }
    queuepoly(VABODY, cgi.shBugArmor, darkena(col, 0, 0xC0));
    }
  else if(id == pshCat) {
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
    auto& body = (id == pshRatling) ? cgi.shYeti : (id == pshSkeleton) ? cgi.shSkeletonBody : (cs.charid&1) ? cgi.shFemaleBody : cgi.shPBody;
    queuepoly(VBODY * VBS, body,  darkena(col, 0, 0X80));

    if(bow::crossbow_mode() && cs.charid < 4) {
      shiftmatrix VWPN = cs.lefthanded ? VBODY * VBS * lmirror() : VBODY * VBS;
      color_t col1 = darkena(col, 0, 0x40);
      queuepoly(VWPN, cgi.shCrossbow, col1);
      int ti = items[itCrossbow];
      if(shmup::on) {
        ti = shmup::getPlayer()->nextshot - shmup::curtime;
        if(ti <= 0) ti = 0;
        else ti = 1 + ti / 500;
        }
      shiftmatrix VWPN1 = VWPN, VWPN2 = VWPN;
      if(GDIM == 3) { ld h = cgi.human_height; VWPN1 = VWPN * lzpush(-h/60); VWPN2 = VWPN * lzpush(-h/30); }
      if(ti <= 1) queuepoly(VWPN1, cgi.shCrossbowstringLoaded, col1);
      else if(ti <= 2) queuepoly(VWPN1, cgi.shCrossbowstringSemiloaded, col1);
      else queuepoly(VWPN1, cgi.shCrossbowstringUnloaded, col1);
      if(ti == 0) queuepoly(VWPN2, cgi.shCrossbowBolt, col1);
      }
    else if(!shmup::on) {
      bool emp = items[itOrbEmpathy] && m != moShadow;
      if(items[itOrbThorns] && emp)
        queuepoly(VBODY * VBS, cgi.shHedgehogBladePlayer, darkena(col, 0, 0x40));
      if(items[itOrbSide1] && !shmup::on)
        queuepoly(VBODY * VBS * spin(-15._deg), cs.charid >= 2 ? cgi.shSabre : cgi.shPSword, darkena(col, 0, 0x40));
      if(items[itOrbSide3] && emp)
        queuepoly(VBODY * VBS, (cs.charid&1) ? cgi.shFerocityF : cgi.shFerocityM, darkena(col, 0, 0x40));

      shiftmatrix VWPN = cs.lefthanded ? VBODY * VBS * lmirror() : VBODY * VBS;
      queuepoly(VWPN, (cs.charid >= 2 ? cgi.shSabre : cgi.shPSword), darkena(col, 0, 0XC0));
      }
    else if(!where || shmup::curtime >= shmup::getPlayer()->nextshot)
      queuepoly(VBODY * VBS, cgi.shPKnife, darkena(col, 0, 0XC0));

    #if CAP_COMPLEX2
    if(camelot::knighted)
      queuepoly(VBODY3 * VBS, cgi.shKnightCloak, darkena(col, 1, 0xC0));
    #endif

    if(&body == &cgi.shYeti) {
      queuepoly(VHEAD1, cgi.shRatHead,  darkena(col, 1, 0xC0));
      queuepoly(VHEAD, cgi.shWolf1, darkena(col, 2, 0xC0));
      queuepoly(VHEAD, cgi.shWolf2, darkena(col, 2, 0xC0));
      queuepoly(VHEAD, cgi.shWolf3, darkena(col, 2, 0xC0));
      }
    else if(&body == &cgi.shSkeletonBody) {
      queuepoly(VHEAD1, cgi.shSkull,  darkena(col, 1, 0xC0));
      queuepoly(VHEAD, cgi.shSkullEyes, darkena(col, 2, 0xC0));
      }
    else {
      queuepoly(VHEAD1, (cs.charid&1) ? cgi.shFemaleHair : cgi.shPHead,  darkena(col, 1, 0XC0));
      queuepoly(VHEAD, cgi.shPFace,  darkena(col, 0, 0XC0));
      }
    if(cs.charid&1)
      queuepoly(VBODY1 * VBS, cgi.shFemaleDress,  darkena(col, 1, 0XC0));
    if(cs.charid == 2)
      queuepoly(VBODY2 * VBS, cgi.shPrinceDress,  darkena(col, 1, 0XC0));
    if(cs.charid == 3)
      queuepoly(VBODY2 * VBS, cgi.shPrincessDress,  darkena(col, 1, 0XC0));

    humanoid_eyes(V,  0xFF, darkena(col, 0, 0x40));
    }
  }

EX void draw_movement_arrows(cell *c, const transmatrix& V, int df) {

  if(viewdists) return;
  
  string keylist = "";
  const ld keysize = .6;      

  color_t col = getcs().uicolor;

  for(int d=0; d<8; d++) {
  
    movedir md = vectodir(spin(-d * 45._deg) * smalltangent());
    cellwalker xc = cwt + md.d;
    if(xc.spin != df) continue;
    xc += wstep;
    if(xc.at == c) {
      transmatrix fixrot = sphereflip * rgpushxto0(sphereflip * tC0(V));
      // make it more transparent
      col -= (col & 0xFF) >> 1;
      poly_outline = OUTLINE_DEFAULT;
      
      char key = 0;
      if(vid.axes >= 5)
        key = (vid.axes == 5 ? keys_wasd : keys_vi)[d];
      
      if(vid.axes >= 5) keylist += key;
      else
        queuepoly(shiftless(fixrot * spin(-d * 45._deg)), cgi.shArrow, col);

      if((c->type & 1) && (isStunnable(c->monst) || isPushable(c->wall))) {
        transmatrix Centered = rgpushxto0(unshift(tC0(cwtV)));
        int sd = md.subdir;
        if(keybd_subdir_enabled) sd = keybd_subdir;

        transmatrix T = iso_inverse(Centered) * rgpushxto0(Centered * tC0(V)) * lrspintox(Centered*tC0(V)) * spin(-sd * M_PI/S7) * xpush(0.2);
        
        if(vid.axes >= 5)
          queuestr(shiftless(T), keysize * mapfontscale / 100, s0 + key, col >> 8, 1);
        
        else
          queuepoly(shiftless(T), cgi.shArrow, col);
        }
      else if(!confusingGeometry()) break;
      }
    }
  if(keylist != "") queuestr(shiftless(V), keysize * mapfontscale / 100, keylist, col >> 8, 1);
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

  shiftpoint H = cwtV * tile_center();
  ld R = sqrt(H[0] * H[0] + H[1] * H[1]);
  shiftmatrix Centered;

  if(euclid) 
    Centered = shiftless(eupush(H.h));
  else if(R > 1e-9) Centered = rgpushxto0(H);
  else Centered = shiftless(Id);
  
  Centered = Centered * rgpushxto0(hpxy(dx*5, dy*5));
  if(multi::cpid >= 0) multi::crosscenter[multi::cpid] = Centered;
  
  int rax = vid.axes;
  if(rax == 1) rax = drawstaratvec(dx, dy) ? 2 : 0;
  
  if(rax == 0 || vid.axes >= 4) return;

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
    queueline(tC0(Centered), Centered * xspinpush0(d * 45._deg, cgi.scalefactor/2), col, 3 + vid.linequality);
#endif
    }
  }

}
