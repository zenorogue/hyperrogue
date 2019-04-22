// HyperRogue
// This file contains the routines to convert HyperRogue's old vector graphics into 3D models

// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

#include "earcut.hpp"

namespace hr {

#define S (scalefactor / 0.805578)
#define SH (scalefactor / 0.805578 * geom3::height_width / 1.5)

hyperpoint shcenter;

vector<hyperpoint> get_shape(hpcshape sh) {
  vector<hyperpoint> res;
  for(int i=sh.s; i<sh.e-1; i++) res.push_back(hpc[i]);
  return res;  
  }

hyperpoint get_center(const vector<hyperpoint>& vh) {
  hyperpoint h = Hypc;
  using namespace hyperpoint_vec;
  for(auto h1: vh) h = h + h1;
  return normalize(h);
  }

ld zc(ld z) { return geom3::human_height * (z - 0.5); }

transmatrix zpush(ld z) {
  return cpush(2, z);
  }

void add_cone(ld z0, const vector<hyperpoint>& vh, ld z1) {
  last->flags |= POLY_TRIANGLES;
  for(int i=0; i<isize(vh); i++) {
    hpcpush(zpush(z0) * vh[i]);
    hpcpush(zpush(z0) * vh[(i+1) % isize(vh)]);
    hpcpush(zpush(z1) * shcenter);
    }
  }

void add_prism_sync(ld z0, vector<hyperpoint> vh0, ld z1, vector<hyperpoint> vh1) {
  last->flags |= POLY_TRIANGLES;
  for(int i=0; i<isize(vh0); i++) {
    int i1 = (i+1) % isize(vh0);
    hpcpush(zpush(z0) * vh0[i]);
    hpcpush(zpush(z1) * vh1[i]);
    hpcpush(zpush(z0) * vh0[i1]);
    hpcpush(zpush(z1) * vh1[i]);
    hpcpush(zpush(z0) * vh0[i1]);
    hpcpush(zpush(z1) * vh1[i1]);
    }
  }

void add_prism(ld z0, vector<hyperpoint> vh0, ld z1, vector<hyperpoint> vh1) {
  last->flags |= POLY_TRIANGLES;
  
  struct mixed {
    ld angle;
    int owner;
    hyperpoint h;
    mixed(ld a, int o, hyperpoint _h) : angle(a), owner(o), h(_h) {}
    };
  
  transmatrix T0 = gpushxto0(get_center(vh0));
  transmatrix T1 = gpushxto0(get_center(vh1));
  
  vector<mixed> pairs;
  for(auto h: vh0) pairs.emplace_back(atan2(T0*h), 0, h);
  for(auto h: vh1) pairs.emplace_back(atan2(T1*h), 1, h);
  sort(pairs.begin(), pairs.end(), [&] (const mixed p, const mixed q) { return p.angle < q.angle; });
  
  hyperpoint lasts[2];
  for(auto pp: pairs) lasts[pp.owner] = pp.h;
  
  for(auto pp: pairs) {
    int id = pp.owner;
    hpcpush(zpush(z0) * lasts[0]);
    hpcpush(zpush(z1) * lasts[1]);
    hpcpush(zpush(id == 0 ? z0 : z1) * pp.h);
    lasts[id] = pp.h;
    }
  }
  
void shift_last(ld z) {
  for(int i=last->s; i<isize(hpc); i++) hpc[i] = cpush(2, z) * hpc[i];
  }

void shift_shape(hpcshape& sh, ld z) {
  for(int i=sh.s; i<sh.e; i++) hpc[i] = cpush(2, z) * hpc[i];
  }

extern
hpcshape 
  shSemiFloorSide[SIDEPARS],
  shBFloor[2],
  shWave[8][2],  
  shCircleFloor,
  shBarrel,
  shWall[2], shMineMark[2], shBigMineMark[2], shFan,
  shZebra[5],
  shSwitchDisk,
  shTower[11],
  shEmeraldFloor[6],
  shSemiFeatherFloor[2], 
  shSemiFloor[2], shSemiBFloor[2], shSemiFloorShadow,
  shMercuryBridge[2],
  shTriheptaSpecial[14], 
  shCross, shGiantStar[2], shLake, shMirror,
  shHalfFloor[3], shHalfMirror[3],
  shGem[2], shStar, shDisk, shDiskT, shDiskS, shDiskM, shDiskSq, shRing,   
  shTinyBird, shTinyShark,
  shEgg,
  shSpikedRing, shTargetRing, shSawRing, shGearRing, shPeaceRing, shHeptaRing,
  shSpearRing, shLoveRing,
  shDaisy, shTriangle, shNecro, shStatue, shKey, shWindArrow,
  shGun,
  shFigurine, shTreat,
  shElementalShard,
  // shBranch, 
  shIBranch, shTentacle, shTentacleX, shILeaf[2], 
  shMovestar,
  shWolf, shYeti, shDemon, shGDemon, shEagle, shGargoyleWings, shGargoyleBody,
  shFoxTail1, shFoxTail2,
  shDogBody, shDogHead, shDogFrontLeg, shDogRearLeg, shDogFrontPaw, shDogRearPaw,
  shDogTorso,
  shHawk,
  shCatBody, shCatLegs, shCatHead, shFamiliarHead, shFamiliarEye,
  shWolf1, shWolf2, shWolf3,
  shDogStripes,
  shPBody, shPSword, shPKnife,
  shFerocityM, shFerocityF, 
  shHumanFoot, shHumanLeg, shHumanGroin, shHumanNeck, shSkeletalFoot, shYetiFoot,
  shMagicSword, shMagicShovel, shSeaTentacle, shKrakenHead, shKrakenEye, shKrakenEye2,
  shArrow,
  shPHead, shPFace, shGolemhead, shHood, shArmor, 
  shAztecHead, shAztecCap,
  shSabre, shTurban1, shTurban2, shVikingHelmet, shRaiderHelmet, shRaiderArmor, shRaiderBody, shRaiderShirt,
  shWestHat1, shWestHat2, shGunInHand,
  shKnightArmor, shKnightCloak, shWightCloak,
  shGhost, shEyes, shSlime, shJelly, shJoint, shWormHead, shTentHead, shShark, shWormSegment, shSmallWormSegment, shWormTail, shSmallWormTail,
  shMiniGhost, shMiniEyes,
  shHedgehogBlade, shHedgehogBladePlayer,
  shWolfBody, shWolfHead, shWolfLegs, shWolfEyes,
  shWolfFrontLeg, shWolfRearLeg, shWolfFrontPaw, shWolfRearPaw,
  shFemaleBody, shFemaleHair, shFemaleDress, shWitchDress,
  shWitchHair, shBeautyHair, shFlowerHair, shFlowerHand, shSuspenders, shTrophy,
  shBugBody, shBugArmor, shBugLeg, shBugAntenna,
  shPickAxe, shPike, shFlailBall, shFlailTrunk, shFlailChain, shHammerHead,
  shBook, shBookCover, shGrail,
  shBoatOuter, shBoatInner, shCompass1, shCompass2, shCompass3,
  shKnife, shTongue, shFlailMissile, shTrapArrow,
  shPirateHook, shPirateHood, shEyepatch, shPirateX,
  // shScratch, 
  shHeptaMarker, shSnowball, 
  shSkeletonBody, shSkull, shSkullEyes, shFatBody, shWaterElemental,
  shPalaceGate, shFishTail,
  shMouse, shMouseLegs, shMouseEyes,
  shPrincessDress, shPrinceDress,
  shWizardCape1, shWizardCape2,
  shBigCarpet1, shBigCarpet2, shBigCarpet3,
  shGoatHead, shRose, shThorns,
  shRatHead, shRatTail, shRatEyes, shRatCape1, shRatCape2,
  shWizardHat1, shWizardHat2,
  shTortoise[13][6],
  shDragonLegs, shDragonTail, shDragonHead, shDragonSegment, shDragonNostril, 
  shDragonWings, 
  shSolidBranch, shWeakBranch, shBead0, shBead1,
  shBatWings, shBatBody, shBatMouth, shBatFang, shBatEye,
  shParticle[16], shAsteroid[8],
  shReptile[5][4],
  shReptileBody, shReptileHead, shReptileFrontFoot, shReptileRearFoot,
  shReptileFrontLeg, shReptileRearLeg, shReptileTail, shReptileEye,

  shTrylobite, shTrylobiteHead, shTrylobiteBody,
  shTrylobiteFrontLeg, shTrylobiteRearLeg, shTrylobiteFrontClaw, shTrylobiteRearClaw,
  
  shBullBody, shBullHead, shBullHorn, shBullRearHoof, shBullFrontHoof,
  
  shButterflyBody, shButterflyWing, shGadflyBody, shGadflyWing, shGadflyEye,

  shTerraArmor1, shTerraArmor2, shTerraArmor3, shTerraHead, shTerraFace, 
  shJiangShi, shJiangShiDress, shJiangShiCap1, shJiangShiCap2,
  
  shAsymmetric,
  
  shPBodyOnly, shPBodyArm, shPBodyHand, shPHeadOnly,
  
  shDodeca;


extern renderbuffer *floor_textures;

basic_textureinfo models_texture;

void add_texture(hpcshape& sh) {
  if(!floor_textures) return;
  auto& utt = models_texture;
  sh.tinf = &utt;
  sh.texture_offset = isize(utt.tvertices);
  for(int i=sh.s; i<isize(hpc); i++) {
    hyperpoint h = hpc[i];
    ld rad = hypot_d(3, h);
    ld factor = 0.50 + (0.17 * h[2] + 0.13 * h[1] + 0.15 * h[0]) / rad;
    utt.tvertices.push_back(glhr::makevertex(0, factor, 0));
    }
  }

vector<hyperpoint> scaleshape(const vector<hyperpoint>& vh, ld s) {
  vector<hyperpoint> res;
  using namespace hyperpoint_vec;
  for(hyperpoint h: vh) res.push_back(normalize(h * s + shcenter * (1-s)));
  return res;
  }

void make_ha_3d(hpcshape& sh, bool isarmor, ld scale) {
  shcenter = C0;

  auto groin = get_shape(shHumanGroin);
  auto body = get_shape(shPBodyOnly);
  auto neck = get_shape(shHumanNeck);
  auto hand = get_shape(shPBodyHand);
  auto arm = get_shape(shPBodyArm);
  groin = scaleshape(groin, scale);
  neck = scaleshape(neck, scale);

  auto fullbody = get_shape(sh);
  
  auto body7 = body[7];
  auto body26 = body[26];
  body.clear();

  bool foundplus = false, foundminus = false;
  for(hyperpoint h: fullbody) {
    if(h[1] > 0.14 * S) {
      if(foundplus) ;
      else foundplus = true, body.push_back(body7);
      }
    else if(h[1] < -0.14 * S) {
      if(foundminus) ;
      else foundminus = true, body.push_back(body26);
      }
    else body.push_back(h);
    }
  
  auto arm8 = arm[8];
  bool armused = false;
  arm.clear();  
  for(hyperpoint h: fullbody) {
    if(h[1] < 0.08 * S) ;
    else if(h[0] > -0.03 * S) {
      if(armused) ;
      else armused = true, arm.push_back(arm8);
      }
    else arm.push_back(h);
    }
  
  auto hand0 = hand[0];
  hand.clear();
  hand.push_back(hand0);
  for(hyperpoint h: fullbody) {
    if(h[1] + h[0] > 0.13 * S) hand.push_back(h);
    }

  bshape(sh, PPR::MONSTER_BODY);
  add_cone(zc(0.4), groin, zc(0.36));
  add_prism_sync(zc(0.4), groin, zc(0.6), groin);
  add_prism(zc(0.6), groin, zc(0.7), body);
  add_prism(zc(0.7), body, zc(0.8), neck);
  
  add_cone(zc(0.8), neck, zc(0.83));

  int at0 = isize(hpc);
  ld h = geom3::human_height;
  
  if(isize(arm) > 3) {
    shcenter = get_center(arm);
    int arm0 = isize(hpc);
    add_prism_sync(geom3::BODY - h*.03, arm, geom3::BODY + h*.03, arm);
    add_cone(geom3::BODY + h*.03, arm, geom3::BODY + h*.05);
    add_cone(geom3::BODY - h*.03, arm, geom3::BODY - h*.05);
    int arm1 = isize(hpc);
    for(int i=arm0; i<arm1; i++) {
      hyperpoint h = hpc[i];
      ld zl = asinh(h[2]);
      h = zpush(-zl) * h;
      ld rad = hdist0(h);
      rad = (rad - 0.1124*S) / (0.2804*S - 0.1124*S);
      rad = 1 - rad;
      rad *= zc(0.7) - geom3::BODY;
      hpc[i] = zpush(rad) * hpc[i];
      }
    }
   // 0.2804 - keep
   // 0.1124 - move
   
  if(isize(hand) > 3) {
    shcenter = get_center(hand);
    add_cone(geom3::BODY, hand, geom3::BODY + 0.05 * geom3::human_height);
    add_cone(geom3::BODY, hand, geom3::BODY - 0.05 * geom3::human_height);
    }

  int at1 = isize(hpc);
  for(int i=at0; i<at1; i++) hpc.push_back(Mirror * hpc[i]);  
  
  add_texture(shPBody);
  shift_last(-geom3::BODY);
  }

/*
void make_humanoid_3d(hpcshape& sh) { make_ha_3d(sh, false, 0.90); }
void make_armor_3d(hpcshape& sh, ld scale = 1) { make_ha_3d(sh, true, scale); }
*/

void make_humanoid_3d(hpcshape& sh) { make_ha_3d(sh, false, 1); }

void addtri(array<hyperpoint, 3> hs, int kind) {
  ld ds[3];
  ds[0] = hdist(hs[0], hs[1]);
  ds[1] = hdist(hs[1], hs[2]);
  ds[2] = hdist(hs[2], hs[0]);
  ld maxds = 0;
  for(int i=0; i<3; i++) maxds = max(ds[i], maxds) - 1e-3;
  
  if(maxds > 0.02*S) for(int i=0; i<3; i++) {
    int j = (i+1) % 3;
    int k = (j+1) % 3;
    if(hdist(hs[i], hs[j]) > maxds) {
      auto hm = mid(hs[i], hs[j]);
      addtri(make_array(hm, hs[i], hs[k]), kind);
      addtri(make_array(hm, hs[j], hs[k]), kind);
      return;
      }
    }
  
  if(kind) {
    hyperpoint ht[3];
    ld hsh[3];
    ld shi[3];
    bool ok = true;
    for(int s=0; s<3; s++) {
      hs[s] = normalize(hs[s]);
      hyperpoint h = hs[s];
      ld zz = zc(0.78);
      hsh[s] = abs(h[1]);
      zz -= h[1] * h[1] / 0.14 / 0.14 * 0.01 / S / S * SH;
      zz -= h[0] * h[0] / 0.10 / 0.10 * 0.01 / S / S * SH;
      if(abs(h[1]) > 0.14*S) ok = false, zz -= (abs(h[1])/S - 0.14) * SH;
      if(abs(h[0]) > 0.08*S) ok = false, zz -= (abs(h[0])/S - 0.08) * (abs(h[0])/S - 0.08) * 25 * SH;
      hpcpush(ht[s] = zpush(zz) * h);
      if(hsh[s] < 0.1*S) shi[s] = -0.5;
      else if(hsh[s] < 0.12*S) shi[s] = -0.1 - 0.4 * (hsh[s]/S - 0.1) / (0.12 - 0.1);
      else shi[s] = -0.1;
      shi[s] *= geom3::human_height;
      }
    if(ok && kind == 1) for(int i=0; i<3; i++) {
      int j = (i+1) % 3;
      hpcpush(ht[i]);
      hpcpush(ht[j]);
      hpcpush(zpush(shi[i]) * ht[i]);
      hpcpush(ht[j]);
      hpcpush(zpush(shi[i]) * ht[i]);
      hpcpush(zpush(shi[i]) * ht[j]);
      }
    }
  else {
    for(int s=0; s<3; s++) {
      hyperpoint h = hs[s];
      ld zz = zc(0.925);
      if(h[0] < -0.05*S) zz += (h[0]/S + 0.05) * SH;
      if(hdist0(h) <= 0.0501*S) {
        zz += sqrt(0.0026 - pow(hdist0(h)/S, 2)) * SH;
        }
      hpcpush(zpush(zz) * h);
      }
    }
  }

void make_armor_3d(hpcshape& sh, int kind = 1) { 

  auto body = get_shape(sh);
  vector<vector<array<ld, 2> >> pts(2);
  
  for(hyperpoint h: body) {
    array<ld, 2> p;
    p[0] = h[0] / h[3];
    p[1] = h[1] / h[3];
    pts[0].emplace_back(p);
    }
  
  bshape(sh, sh.prio);
  
  vector<int> indices = mapbox::earcut<int> (pts);
  
  last->flags |= POLY_TRIANGLES;

  last->flags |= POLY_TRIANGLES;
  for(int k=0; k<isize(indices); k+=3) {
    addtri(make_array(body[indices[k]], body[indices[k+1]], body[indices[k+2]]), kind);
    }
  
  add_texture(sh);
  if(&sh == &shHood || &sh == &shWightCloak || &sh == &shArmor)
    shift_last(-geom3::HEAD);
  else
    shift_last(-geom3::BODY);
  }

void make_foot_3d(hpcshape& sh) {
  auto foot = get_shape(sh);
  auto leg = get_shape(shHumanLeg);
  auto leg5 = scaleshape(leg, 0.8);

  bshape(sh, PPR::MONSTER_BODY);
  shcenter = get_center(leg);
  add_cone(zc(0), foot, zc(0));
  add_prism(zc(0), foot, zc(0.1), leg);
  add_prism_sync(zc(0.1), leg, zc(0.4), leg5);
  add_cone(zc(0.4), leg5, zc(0.45));
  add_texture(sh);
  // shift_last(-geom3::LEG0);
  for(int i=last->s; i<isize(hpc); i++) hpc[i] = cpush(0, -0.0125*S) * hpc[i];
  }

void make_head_only() {

  auto addpt = [] (int d, int u) {
    hpcpush(zpush(zc(0.925) + 0.06 * SH * sin(u * degree)) * xspinpush0(d * degree, 0.05 * S * cos(u * degree)));
    };
  
  bshape(shPHeadOnly, shPHeadOnly.prio);
  last->flags |= POLY_TRIANGLES;
  for(int d=0; d<360; d+=30) 
  for(int u=-90; u<=90; u+=30) {
    addpt(d, u);
    addpt(d+30, u);
    addpt(d, u+30);
    addpt(d+30, u+30);
    addpt(d+30, u);
    addpt(d, u+30);
    }
  
  add_texture(shPHeadOnly);
  shift_last(-geom3::HEAD - 0.01 * SH); 
  }


void make_head_3d(hpcshape& sh) {
  auto head = get_shape(sh);
  vector<vector<array<ld, 2> >> pts(2);
  
  for(hyperpoint h: head) {
    array<ld, 2> p;
    p[0] = h[0] / h[3];
    p[1] = h[1] / h[3];
    pts[0].emplace_back(p);
    }
  
  array<ld, 2> zero = {0,0};
  pts[1].emplace_back(zero);
  head.push_back(C0);
  
  bshape(sh, sh.prio);
  
  vector<int> indices = mapbox::earcut<int> (pts);
  
  last->flags |= POLY_TRIANGLES;
  for(int k=0; k<isize(indices); k+=3) {
    addtri(make_array(head[indices[k]], head[indices[k+1]], head[indices[k+2]]), 0);
    }
  
  add_texture(sh);
  shift_last(-geom3::HEAD);
  }

void make_paw_3d(hpcshape& sh, hpcshape& legsh) {
  auto foot = get_shape(sh);
  auto leg = get_shape(legsh);

  bshape(sh, PPR::MONSTER_BODY);
  shcenter = get_center(leg);
  add_cone(zc(0), foot, zc(0));
  add_prism(zc(0), foot, zc(0.1), leg);
  add_prism_sync(zc(0.1), leg, zc(0.4), leg);
  add_cone(zc(0.4), leg, zc(0.45));
  add_texture(sh);
  shift_last(-geom3::LEG0);
  }

void make_abody_3d(hpcshape& sh, ld tail) {
  auto body = get_shape(sh);
  shcenter = get_center(body);

  vector<hyperpoint> notail;
  ld minx = 9;
  for(hyperpoint h: body) minx = min(minx, h[0]);
  for(hyperpoint h: body) if(h[0] >= minx + tail) notail.push_back(h);

  auto body8 = scaleshape(notail, 0.8);  
  
  bshape(sh, PPR::MONSTER_BODY);
  add_prism(zc(0.4), body8, zc(0.45), body);
  add_prism(zc(0.45), body, zc(0.5), notail);
  add_prism_sync(zc(0.6), body8, zc(0.5), notail);
  add_cone(zc(0.4), body8, zc(0.36));
  add_cone(zc(0.6), body8, zc(0.64));
  add_texture(sh);
  }

void make_ahead_3d(hpcshape& sh) {
  auto body = get_shape(sh);
  shcenter = get_center(body);
  auto body8 = scaleshape(body, 0.5);
  
  bshape(sh, PPR::MONSTER_BODY);
  add_prism_sync(zc(0.4), body8, zc(0.5), body);
  add_prism_sync(zc(0.6), body8, zc(0.5), body);
  add_cone(zc(0.4), body8, zc(0.36));
  add_cone(zc(0.6), body8, zc(0.64));
  add_texture(sh);
  }

void make_skeletal(hpcshape& sh, ld push = 0) {
  auto body = get_shape(sh);
  shcenter = get_center(body);
  
  bshape(sh, PPR::MONSTER_BODY);
  add_prism_sync(zc(0.48), body, zc(0.5), body);
  add_prism_sync(zc(0.52), body, zc(0.5), body);
  add_cone(zc(0.48), body, zc(0.47));
  add_cone(zc(0.52), body, zc(0.53));
  add_texture(sh);
  shift_last(-push);
  }

void make_revolution(hpcshape& sh, int mx = 180, ld push = 0) {
  auto body = get_shape(sh);
  bshape(sh, PPR::MONSTER_BODY);
  int step = (mx == 360 ? 24 : 10);
  for(int i=0; i<isize(body); i++) {
    hyperpoint h0 = body[i];
    hyperpoint h1 = body[(i+1) % isize(body)];
    for(int s=0; s<mx; s+=step) {
      hpcpush(cspin(1, 2, s * degree) * h0);
      hpcpush(cspin(1, 2, s * degree) * h1);
      hpcpush(cspin(1, 2, (s+step) * degree) * h0);
      hpcpush(cspin(1, 2, s * degree) * h1);
      hpcpush(cspin(1, 2, (s+step) * degree) * h0);
      hpcpush(cspin(1, 2, (s+step) * degree) * h1);
      }
    }
  last->flags |= POLY_TRIANGLES;
  add_texture(sh);
  shift_last(-push);
  }

void make_revolution_cut(hpcshape &sh, int each = 180, ld push = 0, ld width = 99) {
  auto body = get_shape(sh);
  int n = isize(body) / 2;
  
  auto gbody = body;
  
  int it = 0;
  
  vector<int> nextid(n);
  vector<int> lastid(n);
  vector<bool> stillin(n, true);
  for(int i=0; i<n; i++) nextid[i] = i+1;
  for(int i=0; i<n; i++) lastid[i] = i-1;

  while(true) {
    it++;
    int cand = -1;
    ld cv = 0;
    for(int i=1; i<n-1; i++) if(stillin[i]) {
      if((gbody[i][0] < gbody[lastid[i]][0] && gbody[i][0] < gbody[nextid[i]][0]) || (gbody[i][0] > gbody[lastid[i]][0] && gbody[i][0] > gbody[nextid[i]][0]) || abs(gbody[i][1]) > width)
      if(abs(gbody[i][1]) > cv)
        cand = i, cv = abs(gbody[i][1]);
      }
    if(cand == -1) break;
    int i = cand;
    lastid[nextid[i]] = lastid[i];
    nextid[lastid[i]] = nextid[i];
    stillin[i] = false;
    }
  
  for(int i=n; i>=0; i--) if(!stillin[i] && !stillin[nextid[i]]) nextid[i] = nextid[nextid[i]];
  for(int i=0; i<n; i++) if(!stillin[i] && !stillin[lastid[i]]) lastid[i] = lastid[lastid[i]];

  for(int i=0; i<n; i++) {
    using namespace hyperpoint_vec;
    if(!stillin[i]) gbody[i] = normalize(gbody[lastid[i]] * (i - lastid[i]) + gbody[nextid[i]] * (nextid[i] - i));
    }

  bshape(sh, PPR::MONSTER_BODY);
  int step = 10;
  for(int i=0; i<n; i++) {
    for(int s=0; s<360; s+=step) {
      auto& tbody = (s % each ? gbody : body);
      auto& nbody = ((s+step) % each ? gbody : body);
      int i1 = (i+1) % isize(body);
      hyperpoint h0 = tbody[i];
      hyperpoint h1 = tbody[i1];
      hyperpoint hs0 = nbody[i];
      hyperpoint hs1 = nbody[i1];
      hpcpush(cspin(1, 2, s * degree) * h0);
      hpcpush(cspin(1, 2, s * degree) * h1);
      hpcpush(cspin(1, 2, (s+step) * degree) * hs0);
      hpcpush(cspin(1, 2, s * degree) * h1);
      hpcpush(cspin(1, 2, (s+step) * degree) * hs0);
      hpcpush(cspin(1, 2, (s+step) * degree) * hs1);
      }
    }
  last->flags |= POLY_TRIANGLES;
  add_texture(sh);
  shift_last(-push);
  }

void disable(hpcshape& sh) {
  sh.s = sh.e = 0;
  }

void clone_shape(hpcshape& sh, hpcshape& target) {
  target = sh;
  target.s = isize(hpc);
  for(int i=sh.s; i<sh.e; i++) hpc.push_back(hpc[i]);
  target.e = isize(hpc);
  }

void animate_bird(hpcshape& orig, hpcshape animated[30], ld body) {
  for(int i=0; i<30; i++) {
    auto& tgt = animated[i];
    clone_shape(orig, tgt);
    ld alpha = sin(12 * degree * i) * 30 * degree;
    for(int i=tgt.s; i<tgt.e; i++) {
      if(abs(hpc[i][1]) > body) {
        ld off = hpc[i][1] > 0 ? body : -body;
        hpc[i][2] += abs(hpc[i][1] - off) * sin(alpha);
        hpc[i][1] = off + (hpc[i][1] - off) * cos(alpha);
        hpc[i] = normalize(hpc[i]);
        }
      }
    }
  }

void make_3d_models() {
  if(DIM == 2) return;
  shcenter = C0;
  
  if(floor_textures) {
    auto& utt = models_texture;
    utt.tvertices.clear();
    utt.texture_id = floor_textures->renderedTexture;
    }
    
  make_humanoid_3d(shPBody);
  make_humanoid_3d(shYeti);
  make_humanoid_3d(shFemaleBody);
  make_humanoid_3d(shRaiderBody);
  make_humanoid_3d(shSkeletonBody);
  make_humanoid_3d(shFatBody);
  make_humanoid_3d(shWaterElemental);
  make_humanoid_3d(shJiangShi);
  
  // shFatBody = shPBody;
  // shFemaleBody = shPBody;
  // shRaiderBody = shPBody;
  // shJiangShi = shPBody;

  make_head_3d(shFemaleHair);
  make_head_3d(shPHead);
  make_head_3d(shTurban1);
  make_head_3d(shTurban2);
  make_head_3d(shAztecHead);
  make_head_3d(shAztecCap);
  make_head_3d(shVikingHelmet);
  make_head_3d(shRaiderHelmet);
  make_head_3d(shWestHat1);
  make_head_3d(shWestHat2);
  make_head_3d(shWitchHair);
  make_head_3d(shBeautyHair);
  make_head_3d(shFlowerHair);
  make_head_3d(shGolemhead);
  make_head_3d(shPirateHood);
  make_head_3d(shEyepatch);
  make_head_3d(shSkull);
  make_head_3d(shRatHead);
  make_head_3d(shDemon);
  make_head_3d(shGoatHead);
  make_head_3d(shRatCape1);
  make_head_3d(shJiangShiCap1);
  make_head_3d(shJiangShiCap2);
  make_head_3d(shTerraHead);
  
  make_armor_3d(shKnightArmor);
  make_armor_3d(shKnightCloak, 2);
  make_armor_3d(shPrinceDress);
  make_armor_3d(shPrincessDress, 2);
  make_armor_3d(shTerraArmor1);
  make_armor_3d(shTerraArmor2);
  make_armor_3d(shTerraArmor3); 
  make_armor_3d(shSuspenders); 
  make_armor_3d(shJiangShiDress); 
  make_armor_3d(shFemaleDress); 
  make_armor_3d(shWightCloak, 2); 
  make_armor_3d(shRaiderArmor); 
  make_armor_3d(shRaiderShirt); 
  make_armor_3d(shArmor); 
  make_armor_3d(shRatCape2, 2); 
  
  make_armor_3d(shHood, 2);
  
  make_foot_3d(shHumanFoot);
  make_foot_3d(shYetiFoot);
  make_skeletal(shSkeletalFoot);
  
  make_paw_3d(shWolfFrontPaw, shWolfFrontLeg);
  make_paw_3d(shWolfRearPaw, shWolfRearLeg);
  make_paw_3d(shDogFrontPaw, shDogFrontLeg);
  make_paw_3d(shDogRearPaw, shDogRearLeg);  
  
  // make_abody_3d(shWolfBody, 0.01);
  // make_ahead_3d(shWolfHead);
  // make_ahead_3d(shFamiliarHead);
  make_revolution_cut(shWolfBody, 30, 0, 0.01*S);
  make_revolution_cut(shWolfHead, 180, geom3::AHEAD - geom3::ABODY);
  make_revolution_cut(shFamiliarHead, 30, geom3::AHEAD - geom3::ABODY);

  // make_abody_3d(shDogTorso, 0.01);
  make_revolution_cut(shDogTorso, 30);
  make_revolution_cut(shDogHead, 180, geom3::AHEAD - geom3::ABODY);
  // make_ahead_3d(shDogHead);

  // make_abody_3d(shCatBody, 0.05);
  // make_ahead_3d(shCatHead);
  make_revolution_cut(shCatBody, 30);
  make_revolution_cut(shCatHead, 180, geom3::AHEAD - geom3::ABODY);

  make_paw_3d(shReptileFrontFoot, shReptileFrontLeg);
  make_paw_3d(shReptileRearFoot, shReptileRearLeg);  
  make_abody_3d(shReptileBody, -1);
  // make_ahead_3d(shReptileHead);
  make_revolution_cut(shReptileHead, 180, geom3::AHEAD - geom3::ABODY);

  make_paw_3d(shBullFrontHoof, shBullFrontHoof);
  make_paw_3d(shBullRearHoof, shBullRearHoof);
  // make_abody_3d(shBullBody, 0.05);
  // make_ahead_3d(shBullHead);
  // make_ahead_3d(shBullHorn);
  make_revolution_cut(shBullBody, 180);
  make_revolution_cut(shBullHead, 60, geom3::AHEAD - geom3::ABODY);
  shift_shape(shBullHorn, -(geom3::AHEAD - geom3::ABODY));
  // make_revolution_cut(shBullHorn, 180, geom3::AHEAD - geom3::ABODY);
  
  make_paw_3d(shTrylobiteFrontClaw, shTrylobiteFrontLeg);
  make_paw_3d(shTrylobiteRearClaw, shTrylobiteRearLeg);
  make_abody_3d(shTrylobiteBody, 0);
  // make_ahead_3d(shTrylobiteHead);
  make_revolution_cut(shTrylobiteHead, 180, geom3::AHEAD - geom3::ABODY);
  
  make_revolution_cut(shShark, 180);

  make_revolution_cut(shGhost, 60);
  make_revolution_cut(shSlime, 60);

  make_revolution_cut(shEagle, 180, 0, 0.05*S);
  make_revolution_cut(shHawk, 180, 0, 0.05*S);

  make_revolution_cut(shTinyBird, 180, 0, 0.025 * S);
  make_revolution_cut(shTinyShark, 90);
  make_revolution_cut(shMiniGhost, 60);

  make_revolution_cut(shGargoyleWings, 180, 0, 0.05*S);
  make_revolution_cut(shGargoyleBody, 180, 0, 0.05*S);
  make_revolution_cut(shGadflyWing, 180, 0, 0.05*S);
  make_revolution_cut(shBatWings, 180, 0, 0.05*S);
  make_revolution_cut(shBatBody, 180, 0, 0.05*S);

  make_revolution_cut(shJelly, 60);
  make_revolution(shFoxTail1);
  make_revolution(shFoxTail2);
  make_revolution(shGadflyBody);
  for(int i=0; i<8; i++)
    make_revolution(shAsteroid[i], 360);
  
  make_revolution_cut(shBugLeg, 60, geom3::ALEG0);

  make_revolution(shBugArmor, 180, geom3::ABODY);
  make_revolution_cut(shBugAntenna, 90, geom3::ABODY);
  
  make_revolution_cut(shButterflyBody, 180);
  
  shift_shape(shWolf1, -0.088 * S);
  shift_shape(shWolf2, -0.088 * S);
  shift_shape(shWolf3, -0.098 * S);
  shift_shape(shFamiliarEye,  -0.088 * S);
  shift_shape(shWolfEyes,  (-0.088 - 0.01 * 0.9) * S);
  
  shift_shape(shEyes, (-3.3) * S / -20);
  for(int i=shEyes.s; i<shEyes.e; i++) hpc[i] = cspin(0, 2, M_PI/2) * hpc[i];
  
  animate_bird(shEagle, shAnimatedEagle, 0.05*S);
  animate_bird(shTinyBird, shAnimatedTinyEagle, 0.05*S/2);

  disable(shWolfRearLeg);
  disable(shWolfFrontLeg);
  disable(shDogRearLeg);
  disable(shDogFrontLeg);
  disable(shReptileFrontLeg);
  disable(shReptileRearLeg);
  disable(shTrylobiteFrontLeg);
  disable(shTrylobiteRearLeg);
  disable(shPFace);
  disable(shJiangShi);

  make_head_only();
  }

#undef S
#undef SH

}
