// Hyperbolic Rogue -- land availability
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file landlock.cpp
 *  \brief unlocking lands, which lands could be found beyond the wall, validity of various lands depending on the settings
 */

#include "hyper.h"
namespace hr {

EX bool in_full_game() {
  if(tactic::on) return false;
  if(princess::challenge) return false;
  if(chaosmode) return true;
  if(euclid && isCrossroads(specialland)) return true;
  if(weirdhyperbolic && specialland == laCrossroads4) return true;
  if(cryst && isCrossroads(specialland)) return true;
  if((in_s2xe() || nonisotropic || (hybri && hybrid::under_class() != gcSphere)) && isCrossroads(specialland)) return true;
  if(geometry == gNormal && !NONSTDVAR) return true;
  return false;
  }

EX bool nodisplay(eMonster m) {
  return 
    m == moIvyDead ||
    m == moDragonTail ||
    m == moWolfMoved ||
    m == moIvyNext ||
    m == moIvyDead;
  }    

/** returns: 2 = treasure increaser, 1 = just appears, 0 = does not appear */
EX int isNative(eLand l, eMonster m) {
  switch(l) {
    #define LAND(a,b,c,d,e,f,g) case c:
    #define NATIVE(x) return x;
    #include "content.cpp"
    
    case landtypes: return 0;
    }
  return false;
  }

EX eItem treasureType(eLand l) { return linf[l].treasure; }

EX eItem treasureTypeUnlock(eLand l, eItem u) {
  if(u != itOrbLove && l == laPrincessQuest)
    return itPalace;
  return treasureType(l);
  }

EX eLand landof(eItem it) {
  for(int i=0; i<landtypes; i++) if(treasureType(eLand(i)) == it) return eLand(i);
  return laNone;
  }

EX int landMultiplier(eLand l) {
  if(l == laCamelot || l == laPrincessQuest) return 10;
  return 1;
  }

EX bool isCrossroads(eLand l) {
  return l == laCrossroads || l == laCrossroads2 || l == laCrossroads3 ||
    l == laCrossroads4 || l == laCrossroads5;
  }

EX bool bearsCamelot(eLand l) {
  return isCrossroads(l) && l != laCrossroads2 && l != laCrossroads5;
  }

EX bool inmirror(const cellwalker& cw) {
  return inmirror(cw.at->land);
  }

EX eLand oppositeElement(eLand l, eLand l2) {
  if(l == laEFire) return laEWater;
  if(l == laEWater) return laEFire;
  if(l == laEAir) return laEEarth;
  if(l == laEEarth) return laEAir;
  if(l == laMirror && l2 == laMirrored) return laMirrored2;
  if(l == laMirrored2 && l2 == laMirrored) return laMirror;
  return l;
  }

// land unlocking

EX eLand firstland = laIce;
EX eLand specialland = laIce;

EX int chaosmode = 0;

EX bool landUnlockedRPM(eLand n) {
  if(isRandland(n) == 2) return true;
  if(isRandland(n) == 1)
    return (autocheat || cheater || hiitemsMax(treasureType(n)) >= 10);
  return false;
  }

EX int variant_unlock_value() {
  return inv::on ? 75 : 30;
  }

EX bool landUnlocked(eLand l) {
  if(randomPatternsMode) {
    return landUnlockedRPM(l);
    }
  
  back:
  
  switch(l) {
    #define LAND(a,b,c,d,e,f,g) case c:
    #define REQ(x) x return true;
    #define REQAS(x,y) l = x; goto back;
    #define GOLD(x) if(gold() < x) return false;
    #define ITEMS(kind, number) if(items[kind] < number) return false;
    #define NEVER if(1) return false;
    #define ALWAYS ;
    #define KILLS(x) if(tkills() < x) return false;
    #define KILL(x, where) if(!kills[x]) return false;
    #define AKILL(x, where) if(kills[x]) return true;
    #define ORD(a, b) a b
    #define NUMBER(val, required, description) if(val < required) return false;
    #define COND(x,y) if(!(x)) return false;
    #define ITEMS_TOTAL(list, z) { int total = 0; for(eItem x: list) total += items[x]; if(total < z) return false; }
    #define ACCONLY(x)
    #define ACCONLY2(x,y)
    #define ACCONLYF(x)
    #include "content.cpp"

    case landtypes: return false;
    }
  return false;
  }

EX bool required_for_hyperstones(eItem ttype) {
  if(ttype == itHyperstone)
    return false;
  if(among(ttype, itHolyGrail, itSavedPrincess))
    return false;
  if(euclid && among(ttype, itBabyTortoise, itHunting))
    return false;
  return true;
  }

EX void countHyperstoneQuest(int& i1, int& i2) {
  i1 = 0; i2 = 0;
  generateLandList(isLandIngame);
  for(eLand l: landlist) {
    eItem ttype = treasureType(l);
    if(!required_for_hyperstones(ttype)) continue;
    i2++; if(items[ttype] >= R10) i1++;
    }
  }

EX bool hyperstonesUnlocked() {
  int i1, i2;
  if(tactic::on && isCrossroads(specialland) && !tactic::trailer) return true;
  countHyperstoneQuest(i1, i2);
  return i1 == i2;
  }

// 2 = always available, 1 = highscore required, 0 = never available
EX int isRandland(eLand l) {
  if(l == laIce || l == laDesert || l == laCaves || l == laWildWest || l == laDocks)
    return 2;
  for(eLand ll: randlands) if(l == ll) return 1;
  return 0;
  }

EX bool incompatible1(eLand l1, eLand l2) {
  if(isCrossroads(l1) && isCrossroads(l2)) return true;
  if(l1 == laJungle && l2 == laMotion) return true;
  if(l1 == laMirrorOld && l2 == laMotion) return true;
  if(l1 == laPower && l2 == laWineyard) return true;
  if(l1 == laPower && l2 == laDryForest) return true;
  if(l1 == laVolcano && l2 == laDryForest) return true;
  if(l1 == laVolcano && l2 == laWineyard) return true;
  if(l1 == laDragon && l2 == laDryForest) return true;
  if(l1 == laEFire && l2 == laWineyard) return true;
  if(l1 == laEFire && l2 == laDryForest) return true;
  if(l1 == laGraveyard && l2 == laDryForest) return true;
  if(l1 == laGraveyard && l2 == laRuins) return true;
  if(l1 == laGraveyard && l2 == laRedRock) return true;
  if(l1 == laGraveyard && l2 == laEmerald) return true;
  if(l1 == laDeadCaves && l2 == laEmerald) return true;
  if(l1 == laDeadCaves && l2 == laCaves) return true;
  if(l1 == laWarpSea && l2 == laKraken) return true;
  if(l1 == laPrairie && l2 == laCrossroads3) return true;
  if(isElemental(l1) && isElemental(l2)) return true;
  return false;
  }

EX eLand randomElementalLand() {
  int i = hrand(4);
  eLand t[4] = { laEFire, laEWater, laEAir, laEEarth };
  return t[i];
  }

EX int elementalKills() {
  return
    kills[moAirElemental] + kills[moWaterElemental] + kills[moEarthElemental] + kills[moFireElemental];
  }

EX eLand randomElementalLandWeighted() {
  int i = hrand(elementalKills());
  i -= kills[moAirElemental]; if(i<0) return laEAir;
  i -= kills[moWaterElemental]; if(i<0) return laEWater;
  i -= kills[moEarthElemental]; if(i<0) return laEEarth;
  i -= kills[moFireElemental]; if(i<0) return laEFire;
  printf("elemental bug\n");
  return laElementalWall;
  }

EX bool incompatible(eLand nw, eLand old) {
  return (nw == old) || incompatible1(nw, old) || incompatible1(old, nw);
  }

EX bool rlyehComplete() {
  if(chaosmode) return items[itStatue] >= 1;
  return items[itStatue] >= 10 || items[itGrimoire] >= 10;
  }

bool lchance(eLand l) { 
  if(tactic::on || yendor::on || racing::on || ((geometry || GOLDBERG) && specialland == laElementalWall)) return true;
  if(chaosmode) return hrand(100) < 25;
  return hrand(100) >= 40 * kills[elementalOf(l)] / (elementalKills()+1); 
  }

EX eLand pickLandRPM(eLand old) {
  while(true) {
    eLand n = randlands[hrand(isize(randlands))];
    if(incompatible(n, old)) continue;
    if(landUnlockedRPM(n)) return n;
    }
  }

EX eLand pickluck(eLand l1, eLand l2) {
  int t1 = items[treasureType(l1)];
  int t2 = items[treasureType(l2)];
  if(t1 < t2) return l1;
  if(t2 < t1) return l2;
  if(isCrossroads(l1)) return l1;
  return l2;
  }

#define LIKELY for(int u=0; u<5; u++)
#define LIKELY2 for(int u=0; u<2; u++)

/* bool noChaos(eLand l) {
  if(l == laOcean || l == laTemple) return false;
  return 
    isCrossroads(l) || isCyclic(l) || isHaunted(l) || 
    l == laCaribbean || isGravityLand(l) || l == laPrincessQuest ||
    l == laPrairie || l == laHalloween;
  } */

EX eLand getNewSealand(eLand old) {
  while(true) {
    eLand p = pick(laOcean, pick(laCaribbean, laLivefjord, laWarpSea, laKraken, laDocks));
    if(p == laKraken && !landUnlocked(p)) continue;
    if(p == laKraken && peace::on) continue;
    if(incompatible(old, p)) continue;
    if(p == old) continue;
    if(!isLandIngame(p)) continue;
    return p;
    }
  }

EX bool createOnSea(eLand old) {
  return
    old == laWarpSea || old == laCaribbean || old == laKraken ||
    (old == laLivefjord && hrand(2)) || 
    (old == laDocks && hrand(2)) ||
    (old == laOcean && (chaosmode ? hrand(2) : !generatingEquidistant));
  }

EX hookset<eLand(eLand)> *hooks_nextland;

EX eLand getNewLand(eLand old) {

  if(old == laMirror && !chaosmode && hrand(10) >= ((tactic::on || racing::on) ? 0 : markOrb(itOrbLuck) ? 5 : 2)) return laMirrored;
  if(old == laTerracotta && !chaosmode && hrand(5) >= ((tactic::on || racing::on) ? 0 : markOrb(itOrbLuck) ? 2 : 1) && !weirdhyperbolic) return laTerracotta;
    
  eLand l = callhandlers(laNone, hooks_nextland, old);
  if(l) return l;
  
  if(cheatdest != old) if(!isCyclic(cheatdest) && !isTechnicalLand(cheatdest)) return cheatdest;
  
  if(old == laTortoise) return laDragon;

  if(yendor::on && chaosmode) {
    while(true) {
      eLand n = eLand(hrand(landtypes));
      if(n == old) continue;
      if(incompatible(n,old)) continue;
      if(!isLandIngame(n)) continue;
      if(n == laElementalWall || isTechnicalLand(n)) continue;
      if(n == laWildWest) continue;
      if(isElemental(n) && hrand(100) >= 25) continue;
      return n;
      }
    }

  if(markOrb(itOrbLuck)) {
    int i = items[itOrbLuck];
    items[itOrbLuck] = 0;
    eLand l1 = getNewLand(old);
    for(int i=1; i<3; i++)
      l1 = pickluck(l1, getNewLand(old));
    items[itOrbLuck] = i;
    return l1;
    }

  if(randomPatternsMode) return pickLandRPM(old);
  
  if(old == laEEarth && lchance(old)) return hrand(2) ? laEWater : laEFire;
  if(old == laEAir   && lchance(old)) return hrand(2) ? laEWater : laEFire;
  if(old == laEWater && lchance(old)) return hrand(2) ? laEEarth : laEAir;
  if(old == laEFire  && lchance(old)) return hrand(2) ? laEEarth : laEAir;

  #if CAP_RACING  
  if(racing::on && old != laElementalWall) {
    eLand l = old;
    using racing::race_lands;
    while(l == old) l = race_lands[hrand(16)]; // fixed at 16
    if(l == laElementalWall) l = randomElementalLand();
    if(l == laMirror) l = laCrossroads;
    return l;
    }
  #endif

  if(tactic::on && !(tactic::trailer && old == specialland)) return specialland;
  if((weirdhyperbolic || cheater) && specialland != old && specialland != laCrossroads4 && specialland != laIce && !chaosmode && old != laBarrier && !isCyclic(specialland) && specialland != laBrownian)
    return specialland;

  if(yendor::on && (yendor::clev().flags & YF_WALLS)) {
    if(old != yendor::clev().l) return yendor::clev().l;
    else if(old == laOcean) return pick(laLivefjord, laCaribbean);
    }

  if(yendor::on && yendor::nexttostart) {
    eLand l = yendor::nexttostart;
    if(!(yendor::clev().flags & YF_REPEAT)) 
      yendor::nexttostart = laNone;
    return l;
    }
  
  if(old == laDragon && tortoise::seek() && hrand(100) < 50)
    return laTortoise;
  
  if(isWarpedType(old) && (hrand(100) < 25) && chaosmode) return eLand(old ^ laWarpCoast ^ laWarpSea);

  if(createOnSea(old)) 
      return getNewSealand(old);

  if(old == laGraveyard && generatingEquidistant)
    return laHaunted;
  
  if(old == laOcean && gold() >= R60 && hrand(100) < 75 && !rlyehComplete()) 
    return laRlyeh;
    
  if(old == laRlyeh && !rlyehComplete())
    return laOcean;
    
  eLand tab[1024];
  int cnt = 0;
  
  // return (hrand(2)) ? laMotion : laJungle;

  // the basic lands, always available
  tab[cnt++] = laCrossroads;
  tab[cnt++] = laIce;
  tab[cnt++] = laDesert;
  tab[cnt++] = laJungle;
  tab[cnt++] = laMotion;
  if(old == laZebra) LIKELY2 {
    tab[cnt++] = laMotion;
    tab[cnt++] = laHunting;
    }
  tab[cnt++] = laHunting;
  tab[cnt++] = laAlchemist;
  if(old != laDeadCaves) tab[cnt++] = laCaves;
    
  // the intermediate lands
  if(gold() >= R30) {
    tab[cnt++] = laCrossroads;
    tab[cnt++] = geometry ? laMirrorOld : laMirror;
    tab[cnt++] = laOcean;
    tab[cnt++] = laLivefjord;
    tab[cnt++] = laMinefield;
    tab[cnt++] = laPalace;
    if(old == laDragon && items[itElixir] >= U10) LIKELY tab[cnt++] = laReptile;
    if(kills[moVizier]) tab[cnt++] = laEmerald;
    if(kills[moSkeleton]) {
      tab[cnt++] = laRuins;
      if(old == laVariant) LIKELY tab[cnt++] = laRuins;
      }
    if(items[itFeather] >= U10) {
      tab[cnt++] = laZebra;
      if(old == laMotion || old == laHunting) LIKELY2 tab[cnt++] = laZebra;
      }
    tab[cnt++] = laWarpCoast;
    if(euclid) tab[cnt++] = laWarpSea;
    tab[cnt++] = laDocks;
    // Ivory Tower tends to crash while generating equidistant
    if(!generatingEquidistant) tab[cnt++] = laIvoryTower;
    if(items[itElixir] >= U10) tab[cnt++] = laReptile;
    if(items[itElixir] >= U10) tab[cnt++] = laSwitch;
    if(items[itIvory] >= U10 && !generatingEquidistant) tab[cnt++] = laEndorian;
    if(items[itIvory] >= U5 && !generatingEquidistant && items[itFeather] >= U5)
      tab[cnt++] = laWestWall;
    
    if(items[itKraken] >= U10) tab[cnt++] = laBurial;
    }

  if(landUnlocked(laDungeon)) {
     tab[cnt++] = laDungeon;
     if(old == laPalace) LIKELY tab[cnt++] = laDungeon;
     }
  
  // the advanced lands
  if(gold() >= R60) {
    tab[cnt++] = laStorms;
    if(!weirdhyperbolic) tab[cnt++] = laWhirlwind;
    tab[cnt++] = laCrossroads;
    if(!generatingEquidistant) tab[cnt++] = laCrossroads2;
    if(items[itRuby] >= U10) {
      tab[cnt++] = laOvergrown;
      if(old == laJungle) LIKELY tab[cnt++] = laOvergrown;
      }
    if(rlyehComplete()) tab[cnt++] = laRlyeh;
    else if(chaosmode && (old == laWarpCoast || old == laLivefjord || old == laOcean)) 
      tab[cnt++] = laRlyeh;
    if(items[itStatue] >= U5 && chaosmode)
      tab[cnt++] = laTemple;
    if(old == laCrossroads || old == laCrossroads2) tab[cnt++] = laOcean;
    if(old == laOcean) tab[cnt++] = laCrossroads;
    if(items[itGold] >= U5 && items[itFernFlower] >= U5 && !kills[moVizier])
      tab[cnt++] = laEmerald;
    if(old == laVariant && landUnlocked(laEmerald)) LIKELY tab[cnt++] = laEmerald;      
    if(items[itWindstone] >= U5 && items[itDiamond] >= U5) {
      tab[cnt++] = laBlizzard;
      if(old == laIce || old == laCocytus || old == laWhirlwind) 
        LIKELY tab[cnt++] = laBlizzard;
      if(old == laBlizzard) LIKELY tab[cnt++] = laIce;
      if(old == laBlizzard) LIKELY tab[cnt++] = laWhirlwind;
      }
    tab[cnt++] = laDryForest;
    tab[cnt++] = laWineyard;
    if(items[itElixir] >= U10) {
      tab[cnt++] = laVolcano;
      if(old == laAlchemist) LIKELY2 tab[cnt++] = laVolcano;
      if(old == laVolcano) LIKELY2 tab[cnt++] = laAlchemist;
      }
    if(items[itGold] >= U10) tab[cnt++] = laDeadCaves;
    // tab[cnt++] = laCaribbean;
    if(items[itSpice] >= U10) {
      tab[cnt++] = laRedRock;
      if(old == laDesert) LIKELY tab[cnt++] = laRedRock;
      }
    if(old == laRedRock) LIKELY tab[cnt++] = laDesert;
    if(old == laOvergrown) LIKELY tab[cnt++] = laJungle;

    if(items[itIvory] >= U5 && !generatingEquidistant && items[itFeather] >= U5)
      tab[cnt++] = laWestWall;
    }

  if(landUnlocked(laVariant)) {
    tab[cnt++] = laVariant;
    if(old == laRuins) LIKELY tab[cnt++] = laVariant;
    if(old == laGraveyard) LIKELY tab[cnt++] = laVariant;
    if(old == laEmerald) LIKELY tab[cnt++] = laVariant;
    }
  
  if(gold() >= R90) {
    if(!chaosmode) tab[cnt++] = laPrairie;
    if(old == laPrairie) LIKELY tab[cnt++] = laBull;
    tab[cnt++] = laBull;
    if(old == laBull && !chaosmode) LIKELY tab[cnt++] = laPrairie;
    tab[cnt++] = laTerracotta;
    tab[cnt++] = laRose;
    if(chaosmode && geometry) tab[cnt++] = laDual;
    if(chaosmode && geosupport_threecolor()) tab[cnt++] = laSnakeNest;
    }
  
  if(gold() >= R300)
    tab[cnt++] = laCrossroads5;
  
  if(tkills() >= R100) {
    tab[cnt++] = laGraveyard;
    if(gold() >= R60) tab[cnt++] = laHive;
    if(old == laVariant) LIKELY tab[cnt++] = laGraveyard;
    }
  
  if(killtypes() >= R20) {
    tab[cnt++] = laDragon;
    if(old == laReptile) LIKELY tab[cnt++] = laDragon;
    }
  
  if(landUnlocked(laTrollheim)) {
    tab[cnt++] = laTrollheim;
    if(isTrollLand(old)) LIKELY tab[cnt++] = laTrollheim;
    if(old == laTrollheim) for(int i=0; i<landtypes; i++) {
      eLand l2 = eLand(i);
      if(isTrollLand(l2)) LIKELY tab[cnt++] = l2;
      }
    }

  if(landUnlocked(laElementalWall)) {
    tab[cnt++] = randomElementalLandWeighted();
    
    if(old == laDragon) LIKELY tab[cnt++] = laEFire;
    if(old == laEFire) LIKELY tab[cnt++] = laDragon;

    if(old == laLivefjord) LIKELY tab[cnt++] = laEWater;
    if(old == laEWater) LIKELY tab[cnt++] = laLivefjord;
    
    if(old == laDeadCaves) LIKELY tab[cnt++] = laEEarth;
    if(old == laEEarth) LIKELY tab[cnt++] = laDeadCaves;
    
    if(old == laWhirlwind) LIKELY tab[cnt++] = laEAir;
    if(old == laEAir) LIKELY tab[cnt++] = laWhirlwind;
    }
  
  if(landUnlocked(laHell)) {
    if(!generatingEquidistant && old != laPrairie) tab[cnt++] = laCrossroads3;
    tab[cnt++] = laHell;
    }
  
  if(items[itHell] >= U10) {
    if(items[itDiamond] >= U10) {
      tab[cnt++] = laCocytus;
      if(old == laHell || old == laIce || old == laBlizzard) LIKELY tab[cnt++] = laCocytus;
      }
    if(old == laCocytus) LIKELY { tab[cnt++] = laIce; tab[cnt++] = laHell; }
    tab[cnt++] = laPower;
    if(old == laCrossroads || old == laCrossroads2) tab[cnt++] = laOcean;
    if(old == laOcean) tab[cnt++] = laCrossroads2;
    }
  
  // for(int i=0; i<20; i++) tab[cnt++] = laRedRock;
  // for(int i=0; i<20; i++) tab[cnt++] = laCaribbean;
  // for(int i=0; i<20; i++) tab[cnt++] = laCocytus;
  
  // for(int i=0; i<20; i++) tab[cnt++] = laCrossroads;

  eLand n = old;
  while(incompatible(n, old) || !isLandIngame(n)) {
    n = tab[hrand(cnt)];
    if(weirdhyperbolic && specialland == laCrossroads4 && isCrossroads(n))
      n = laCrossroads4;
    }
  
  return n;  
  }

EX vector<eLand> land_over = {
  laIce, laCaves, laDesert, laHunting, laMotion, laJungle, laAlchemist, 
  laCrossroads, 
  laMirror, laMirrorOld, laMinefield, laPalace, laPrincessQuest, laZebra, laSwitch, laReptile, 
  laOcean, laDocks, laWarpCoast, laLivefjord, laKraken, laCaribbean, laBrownian, laWhirlpool, laRlyeh, laTemple,
  laIvoryTower, laEndorian, laWestWall, laDungeon, laMountain, 
  laCrossroads2, 
  laDryForest, laWineyard, laDeadCaves, laGraveyard, laHaunted, laHive, 
  laRedRock, laVolcano,
  laDragon, laTortoise,
  laOvergrown, laClearing, laStorms, laBurial, laWhirlwind, 
  laBlizzard,
  laRuins, laEmerald, laVariant, laCamelot, 
  laPrairie, laBull, laTerracotta, laRose,
  laElementalWall, laTrollheim,
  laHell, laCrossroads3, laCocytus, laPower, laCrossroads4,
  laCrossroads5,
  // EXTRA
  laWildWest, laHalloween, laDual, laSnakeNest, laMagnetic, laCA, laAsteroids
  };

EX vector<eLand> landlist;

#if HDR
template<class T> void generateLandList(T t) {
  landlist.clear();
  for(auto l: land_over) if(t(l)) landlist.push_back(l);    
  }
#endif

#if HDR
namespace lv {
  static const flagtype appears_in_geom_exp = 1;
  static const flagtype display_error_message = 2;
  static const flagtype appears_in_full = 4;
  static const flagtype appears_in_ptm = 8;
  static const flagtype display_in_help = 16;
  static const flagtype one_and_half = 32;
  };

struct land_validity_t {
  int quality_level; // 0 (dont show), 1 (1/2), 2 (ok), 3(1!)
  flagtype flags;
  string msg;
  };
#endif

EX eLand getLandForList(cell *c) {
  eLand l = c->land;
  if(isElemental(l)) return laElementalWall;
  if(l == laWarpSea) return laWarpCoast;
  if(l == laMercuryRiver) return laTerracotta;
  if(l == laBarrier) return laCrossroads;
  if(l == laOceanWall) return laOcean;
  if(l == laPalace && princess::dist(cwt.at) < OUT_OF_PRISON)
    l = laPrincessQuest;
  // princess?
  return l;
  }

EX bool isLandIngame(eLand l) {
  if(isElemental(l)) l = laElementalWall;
  if(dual::state == 2 && !dual::check_side(l)) return false;
  if((euclid || sol) && isCyclic(l) && l != specialland) return false;
  if(l == laCamelot && hyperbolic && WDIM == 3) return false;
  return land_validity(l).flags & lv::appears_in_full;
  }

namespace lv {

  flagtype q0 = lv::display_error_message | lv::display_in_help | lv::appears_in_geom_exp;
  flagtype q1 = lv::display_error_message | lv::appears_in_geom_exp | lv::appears_in_full | lv::display_in_help;
  flagtype q2 = lv::appears_in_geom_exp | lv::appears_in_full | lv::display_in_help | lv::appears_in_ptm;
  flagtype q3 = lv::appears_in_geom_exp | lv::appears_in_full | lv::display_in_help | lv::appears_in_ptm;
  flagtype qm2= q2 | lv::display_error_message;
  flagtype qm3= q3 | lv::display_error_message;
  
  land_validity_t hedgehogs = { 1, qm2 &~ lv::appears_in_full, "Cannot kill Hedgehog Warriors in this geometry."};

  land_validity_t no_randpattern_version = { 0, q0, "No random pattern version."};  
  land_validity_t no_great_walls = { 0, q0, "Great Walls not implemented."};  
  land_validity_t pattern_incompatibility = { 0, q0, "Pattern incompatible."};  
  land_validity_t pattern_not_implemented_random = { 1, q1 | one_and_half, "Pattern not implemented -- using random."};
  land_validity_t pattern_not_implemented_weird = { 1, q1, "Pattern not implemented."};
  land_validity_t pattern_not_implemented_exclude = { 1, q1 & ~ lv::appears_in_full, "Pattern not implemented."};
  land_validity_t not_enough_space = { 0, q0, "Not enough space."};  
  land_validity_t dont_work = { 0, q0, "Does not work in this geometry."};
  land_validity_t bounded_only = { 0, q0, "This land is designed for bounded worlds."};
  land_validity_t unbounded_only = { 0, q0, "This land is designed for infinite worlds."};                                   
  land_validity_t unbounded_only_except_bigsphere = { 0, q0, "This land is designed for infinite worlds or big spheres."};
  land_validity_t out_of_theme = { 3, qm2 &~ lv::appears_in_full, "Out of theme for the full game."};
  land_validity_t no_game = { 2, q2 &~ lv::appears_in_full, "No game here."};  
  land_validity_t not_in_chaos = { 0, q0, "Does not work in chaos mode."};  
  land_validity_t not_in_full_game = {2, qm2 &~ lv::appears_in_full, "Not in the full game."};
  land_validity_t not_in_full_game3 = {3, qm2 &~ lv::appears_in_full, "Not in the full game."};
  land_validity_t special_chaos = { 2, qm2, "Special construction in the Chaos mode." };
  land_validity_t special_euclidean = { 2, qm2, "Special construction in the Euclidean mode." };  
  land_validity_t special_geo = { 2, qm2, "Special construction in this geometry." };
  land_validity_t special_geo3 = { 3, qm2, "Special construction in this geometry." };
  land_validity_t not_implemented = {0, q0, "Not implemented."};  
  land_validity_t partially_implemented = {1, q1 | one_and_half, "Partially implemented."};  
  land_validity_t ok = {2, q2 &~ lv::display_in_help, "No comments."};  
  land_validity_t not_in_ptm = {0, q0, "Does not work in pure tactics mode."};  
  land_validity_t technical = {0, q0 &~ lv::appears_in_geom_exp, "Technical."};  
  land_validity_t full_game = {3, q3 &~ lv::display_in_help, "Full game."};
  land_validity_t inaccurate = {1, q1, "Somewhat inaccurate."};
  land_validity_t great_walls_missing = {1, q1 | one_and_half, "Mercury rivers not implemented (or could not work) in this geometry."};
  land_validity_t pattern_compatibility = {3, qm3, "Patterns compatible."}; 
  land_validity_t pattern_defined = {3, qm3, "Pattern defined."}; 
  land_validity_t pattern_compatibility_sole = {3, qm3 &~ lv::appears_in_full, "Patterns compatible."}; 
  land_validity_t pattern_compatibility_notrec = {2, qm2 &~ lv::appears_in_full, "Patterns compatible."}; 
  land_validity_t specially_designed = {3, qm3, "This land is specially designed for this geometry."};   
  land_validity_t needs_threecolor = {0, q0, "Three-colorability required."};  
  land_validity_t land_not_implemented = {0, q0 &~ lv::appears_in_geom_exp, "Land not implemented."};  
  land_validity_t interesting = {3, q3, "Special interest."};
  land_validity_t better_version_exists = {0, q0, "Better version exists."};
  land_validity_t dont_work_but_ingame = {1, q0 | lv::appears_in_full, "Does not work in this geometry."};
  land_validity_t ugly_version_infull = {1, q1 | lv::appears_in_full, "Grid does not work in this geometry."};
  land_validity_t ugly_version_nofull = {1, q1, "Grid does not work in this geometry."};
  land_validity_t bad_graphics = {1, q1, "Graphics not implemented in this geometry."};
  land_validity_t some0 = {0, q0, "This land does not work in the current settings. Reason not available."};
  land_validity_t some1 = {1, q1, "This land does not work well in the current settings. Reason not available."};
  land_validity_t known_buggy = {1, q1, "This combination is known to be buggy at the moment."};
  land_validity_t sloppy_pattern = {1, q1 | one_and_half, "Somewhat sloppy pattern."};
  land_validity_t no_fractal_landscapes = {1, q1 | one_and_half, "Fractal landscapes not implemented in this geometry."};
  land_validity_t simplified_walls = { 1, q1, "Only simplified walls implemented."};  
  land_validity_t no_walls = { 0, q0, "No walls implemented."};  
  land_validity_t disabled = {0, q0, "This land has been disabled with compilation flags."};
  land_validity_t pattern_special = {3, qm3, "Special pattern implemented for this geometry."}; 
  land_validity_t not_3d = {0, q0, "This land does not make much sense in 3D."}; 
  land_validity_t not_binary = {0, q0, "This land does not make much sense in binary tiling."}; 
  land_validity_t shmup_only = {0, q0, "This land works only in the shmup mode."}; 
  land_validity_t not_in_shmup = {0, q0, "This land is not available in the shmup mode."}; 
  land_validity_t not_in_multi = {0, q0, "This land is not available in multiplayer."}; 
  }

// old Daily Challenges should keep their validity forever
// set this number for historical values of land_validity

EX int old_daily_id = 1000000;

const int landscapes_when = 177;

// check if the given land should appear in lists
EX land_validity_t& land_validity(eLand l) {

  bool stdeucx = stdeuc;
  if(euclid && quotient) stdeucx = false;

  using namespace lv;
  
  if(walls_not_implemented() && isCrossroads(l))
    return no_walls;
  
  if(hybri || hybrid::pmap) {
    if(among(l, laPrincessQuest, laPrairie, laMirrorOld, laMirror, laDual, laWarpCoast, laKraken, laBrownian, laWhirlpool, laWestWall, laHive, laClearing, laWhirlwind, laBlizzard, laBull, laTerracotta, laCrossroads5,
      laEndorian, laDungeon, laMountain))
      return lv::not_implemented;
    if(among(l, laReptile, laDragon, laTortoise))
      return lv::bad_graphics;
    if((hybrid::actual_geometry == gRotSpace || geometry == gRotSpace) && l == laDryForest)
      return lv::hedgehogs;
    if(hybri) return *PIU(&land_validity(l));
    }
  
  #if !CAP_FIELD
  if(among(l, laPrairie, laBlizzard, laVolcano))
    return disabled;
  #endif

  #if !CAP_COMPLEX2
  if(among(l, laBrownian, laWestWall, laVariant))
    return disabled;
  #endif

  if(l == laMinefield && bounded)
    return special_geo3;
  
  if(l == laAsteroids) {
    if(!shmup::on) return shmup_only;
    if(!bounded) return bounded_only;
    return specially_designed;
    }
  
  if(nil) {
    if(among(l, laCrossroads, laCrossroads2, laCrossroads3, laCrossroads4))
      return lv::full_game;
    if(among(l, laPalace, laGraveyard, laWineyard, laElementalWall))
      return lv::pattern_special;
    if(among(l, laEndorian, laCaribbean, laHaunted, laVolcano, laClearing, laStorms, laBlizzard))
      return lv::not_implemented;
    }

  if(WDIM == 3) {
    if(l == laWarpCoast) return ugly_version_nofull;
    if(l == laWineyard && hyperbolic && !binarytiling && S7 == 6) return lv::pattern_special;
    if(l == laEmerald && hyperbolic && !binarytiling && S7 == 12) return lv::pattern_special;
    if(l == laZebra) return pattern_not_implemented_random;
    if(among(l, laWhirlpool, laPrairie, laWestWall, laBull)) return lv::not_3d;
    if(l == laEndorian && geometry == gKiteDart3) return not_implemented;
    if(l == laEndorian && sol) return not_implemented;
    if(l == laEndorian && hyperbolic && !quotient) return lv::pattern_special;
    if(l == laIvoryTower && hyperbolic && binarytiling) return lv::pattern_special;
    if(l == laDungeon || l == laBrownian) return not_implemented;
    if(l == laKraken) return binarytiling ? not_binary : not_implemented;
    if(l == laBurial && !shmup::on) return not_implemented;
    if(l == laMirrorOld && !shmup::on) return not_implemented;
    }
  
  if(l == laBrownian) {
    if(quotient || !hyperbolic || cryst) return dont_work;
    }
  
  if(binarytiling) {
    if(among(l, laMountain, laTemple)) return lv::pattern_compatibility_sole;
    if(among(l, laDungeon, laIvoryTower, laOcean, laEndorian)) return lv::pattern_compatibility;
    if(among(l, laCaribbean, laCamelot)) return lv::pattern_compatibility_notrec;
    // Clearing -- does not generate
    /* laCamelot, laCaribbean -> they are OK but not recommended */
    }
  
  #if CAP_ARCM
  if(archimedean) {
    if(among(l, laPower, laZebra, laWineyard) && arcm::current.have_line) return lv::pattern_defined;
    // horocycles not implemented
    if(isCyclic(l)) return not_implemented;
    }
  #endif

  #if CAP_CRYSTAL
  if(cryst) {
    if(l == laCamelot) return interesting;
    if(isCrossroads(l)) return full_game;
    }
  #endif

  // Random Pattern allowed only in some specific lands
  if(randomPatternsMode && !isRandland(l))
    return no_randpattern_version;

  if(isElemental(l)) {
    if(l != laElementalWall)
      return technical;
    // not good in Field quotient
    if(geometry == gZebraQuotient)
      return special_geo3;
    if(quotient || sol)
      return no_great_walls;
    if(weirdhyperbolic)
      return simplified_walls;
    // works nice on a big non-tetrahedron-based sphere
    if(sphere && S3 != 3 && GOLDBERG)
      return special_geo3;
    }
  
  // not enough space
  if(l == laStorms && (old_daily_id < 35 ? !BITRUNCATED : PURE) && elliptic) 
    return not_enough_space;
  
  if(l == laStorms && WDIM == 3)
    return not_in_full_game; /* uses too much memory */

  if(l == laStorms && S7 == 3) 
    return not_enough_space;
  
  // does not agree with the pattern
  if(l == laStorms && quotient && geometry != gZebraQuotient) 
    return pattern_not_implemented_random;
  
  // pattern not implemented
  if(l == laStorms && S7 == 8) 
    return pattern_not_implemented_random;
  
  // mirrors do not work in gp
  if(among(l, laMirror, laMirrorOld) && (GOLDBERG && old_daily_id < 33))
    return dont_work;

  // mirrors do not work in penrose and sol
  if(among(l, laMirror, laMirrorOld) && (penrose || sol))
    return dont_work;
  
  if(isCrossroads(l) && geometry == gBinary4)
    return not_implemented;

  if(binarytiling && among(l, laMirror, laMirrorOld))
    return dont_work;

  if(l == laWhirlwind && hyperbolic_not37)
    return pattern_incompatibility;

  // available only in non-standard geometries
  if(l == laMirrorOld && !geometry && STDVAR)
    return better_version_exists;
  
  // available only in standard geometry
  if(l == laMirror && (geometry || NONSTDVAR))
    return not_implemented; 
  
  // Halloween needs bounded world (can be big bounded)
  if(l == laHalloween && !bounded)
    return bounded_only;
  
  // Crystal World is designed for nice_dual geometries
  if(l == laDual && (!has_nice_dual() || nonisotropic))
    return dont_work;
  
  if(l == laHaunted && chaosmode)
    return not_in_chaos;
  
  // standard, non-PTM specific
  if(l == laCrossroads5 && tactic::on)
    return not_in_ptm;
    
  // standard non-PTM non-chaos specific
  if((l == laCrossroads5 || l == laCrossroads2) && (geometry || chaosmode))
    return some0;
    
  // special construction in the Chaos mode
  if(chaosmode && (l == laTemple || l == laHive || l == laOcean || l == laHaunted))
    return special_chaos;
  
  if(l == laWhirlpool && a4)
    return dont_work;

  if(isWarpedType(l) && a4 && GOLDBERG)
    return dont_work;
  
  #if CAP_IRR
  if((isWarpedType(l) || l == laDual) && IRREGULAR && !irr::bitruncations_performed)
    return dont_work;
  
  if(IRREGULAR && among(l, laPrairie, laMirror, laMirrorOld))
    return dont_work;
  #endif
    
  if(archimedean && l == laPrairie) return dont_work;

  if((IRREGULAR || archimedean) && among(l, laBlizzard, laVolcano) && !sphere)
    return dont_work;

  if(archimedean && DUAL && l == laCrossroads4)
    return not_implemented;
  
  if(geometry == gKiteDart3 && l == laGraveyard)
    return lv::pattern_special;
  
  // equidistant-based lands
  if(isEquidLand(l)) {
    // no equidistants supported in chaos mode
    if(chaosmode) 
      return not_in_chaos;
    // the algorithm fails in Archimedean DUAL
    if(archimedean && DUAL)
      return not_implemented;
    // the algorithm fails in Binary4
    if(geometry == gBinary4)
      return not_implemented;
    // no equidistants supported in these geometries (big sphere is OK though)
    if(bounded && !bigsphere)
      return unbounded_only_except_bigsphere;
    // Yendorian only implemented in standard
    if(l == laEndorian && geometry)
      return not_implemented;
    // special Euclidean implementations
    if(euclid && (l == laIvoryTower || l == laMountain || l == laOcean || l == laMountain)) 
      return special_geo;
    // in other geometries, it works
    if(geometry)
      return ok;
    }
  
  if(l == laPrincessQuest && chaosmode)
    return not_in_chaos;
   
  if(l == laPrincessQuest && tactic::on)
    return not_in_ptm;
    
  if(l == laPrincessQuest && (!stdeucx || NONSTDVAR))
    return not_implemented;
  
  if(l == laPrincessQuest && shmup::on)
    return not_in_shmup;

  if(l == laPrincessQuest && multi::players > 1)
    return not_in_multi;

  if(l == laMountain && chaosmode)
    return not_in_chaos;
  
  if(l == laBrownian && chaosmode)
    return not_in_chaos;
  
  // works correctly only in some geometries
  if(l == laClearing && chaosmode)
    return not_in_chaos;
  
  if(l == laClearing)
    if(!(stdeucx || a38 || (a45 && BITRUNCATED) || (a47 && BITRUNCATED)) || NONSTDVAR)
    if(!bounded)
      return not_implemented;

  // does not work in non-bitrunc a4
  if(l == laOvergrown && a4 && !BITRUNCATED)
    return some0;

  // does not work in bounded either
  if(l == laOvergrown && bounded)
    return some0;
  
  // horocycle-based lands, not available in bounded geometries nor in Chaos mode
  if(l == laWhirlpool || l == laCamelot || l == laCaribbean || l == laTemple || l == laHive) {
    if(chaosmode) {
      if(l == laTemple || l == laHive) 
        return special_chaos;
      return not_in_chaos;
      }
    if(archimedean || penrose) return not_implemented;
    if(bounded) return unbounded_only;
    }
  
  if(chaosmode && isCrossroads(l))
    return not_in_chaos;
  
  // this pattern does not work on elliptic and small spheres
  if((l == laBlizzard || l == laVolcano) && elliptic && S7 < 5 && !archimedean)
    return not_enough_space;
  
  // ... and it works in gp only partially
  if((l == laBlizzard || l == laVolcano) && GOLDBERG && (old_daily_id < 33 || !sphere))
    return partially_implemented;

  // Kraken does not really work on odd-sided cells;
  // a nice football pattern will solve the problem by forbidding the Kraken to go there
  // (but we do have to allow it in non-bitrunc standard)
  if(l == laKraken && (S7&1) && !has_nice_dual()) {
    return dont_work_but_ingame;
    }
  
  // works in most spheres, Zebra quotient, and stdeucx
  if(l == laWhirlwind) {
    if(geometry == gZebraQuotient)
      return pattern_compatibility;
    if(stdeucx) ;
    else if(S7 == 4 && BITRUNCATED) return special_geo;
    else if(bigsphere && !BITRUNCATED && !elliptic) return special_geo;
    else return dont_work;
    }
    
  // needs standard/Euclidean (needs fractal landscape)
  if(among(l, laTortoise, laVariant) && !(old_daily_id < landscapes_when ? stdeucx : geometry_supports_cdata()))
    return not_implemented;
  
  // technical lands do not count
  if(l != laCA && isTechnicalLand(l))
    return technical;
  
  // only in bounded geometry, and not in PTM
  if(l == laCA && !bounded)
    return bounded_only;

  if(l == laCA && tactic::on)
    return not_in_ptm;

  if(l == laCA)
    return no_game;
  
  // Dragon Chasm requires unbounded space [partial]
  if(l == laDragon && smallbounded)
    return unbounded_only;
  
  // Graveyard pattern does not work on non-bitrunc weird geometries
  if((l == laGraveyard && !randomPatternsMode) || l == laRuins || l == laRedRock) switch(geosupport_football()) {
    case 0:
      return dont_work;
    case 1:
      return sloppy_pattern;
    default: ;
    }
  
  // Warped Coast does not work on non-bitrunc S3s (except standard heptagonal where we have to keep it)
  if(l == laWarpCoast && (S3==3) && geosupport_football() != 2 && !(old_daily_id >= 33 && geosupport_chessboard())) {
    return ugly_version_infull;
    }

  if(l == laWarpCoast && quotient && geometry != gZebraQuotient && !randomPatternsMode) 
    return pattern_incompatibility;
  
  if(among(l, laEmerald, laCamelot, laDryForest) && VALENCE != 3 && old_daily_id >= 65)
    return hedgehogs;  

  // laPower and laEmerald and laPalace -> [partial] in quotients and hyperbolic_non37
  if((l == laPower || l == laEmerald || l == laPalace || l == laWildWest) && !randomPatternsMode) {
    if(euclid || bigsphere) ;
    else if(old_daily_id <= 65 && a45) ;
    else if(!hyperbolic_37) return l == laWildWest ? some0 : pattern_not_implemented_random;
    else if(quotient) return pattern_incompatibility;
    }

  if(among(l, laEmerald, laCamelot, laDryForest) && VALENCE != 3)
    return hedgehogs;  

  if(l == laWineyard && sol)
    return lv::pattern_special;
  
  // ... wineyard pattern is GOOD only in the standard geometry or Euclidean
  if(l == laWineyard && (NONSTDVAR || sphere) && !randomPatternsMode)
    return pattern_not_implemented_random;

  if(l == laTrollheim && quotient == qFIELD)
    return not_enough_space;
      
  if(l == laStorms && hyperbolic_not37)
    return pattern_not_implemented_random;
  
  if(l == laTrollheim && !stdeucx && !bounded)
    return some1;

  if(l == laReptile && sol) return ugly_version_nofull;

  if(l == laReptile) {
    if(old_daily_id <= 64) {
      if(l == laReptile && (a38 || a4 || sphere || !BITRUNCATED || (quotient && !euclid && geometry != gZebraQuotient)))
        return bad_graphics;
      }
    else {  
      bool reptile_good = false;
      if(hyperbolic_37 && BITRUNCATED) reptile_good = true;
      if(euc::in(2,6)) reptile_good = true;
      if(quotient && geometry != gZebraQuotient && !euclid)
        reptile_good = false;
      if(!reptile_good)
        return bad_graphics;
      }
    }

  if((l == laDragon || l == laReptile) && !stdeucx && !smallbounded && !randomPatternsMode)
    return no_fractal_landscapes;
  
  if(l == laCrossroads && smallsphere) 
    return not_enough_space;
  
  if(l == laCrossroads3 && !stdeucx && !bigsphere)
    return not_enough_space;

  if(among(l, laCrossroads, laCrossroads2, laCrossroads3, laCrossroads5) && weirdhyperbolic)
    return no_great_walls;

  // Crossroads IV is great in weird hyperbolic
  if(l == laCrossroads4 && weirdhyperbolic && !quotient)
    return full_game;

  // OK in small bounded worlds, and in Euclidean  
  if(l == laCrossroads4 && quotient)
    return some0;

  if(l == laZebra && quotient && geometry != gZebraQuotient && !randomPatternsMode)
    return pattern_incompatibility;
  
  if(l == laZebra && !(stdeucx || (a4 && !BITRUNCATED) || a46 || (geometry == gZebraQuotient && old_daily_id > 106)) && !randomPatternsMode)
    return pattern_not_implemented_weird;
  
  if(l == laCrossroads3 && euclid)
    return inaccurate; // because it is not accurate

  if(l == laPrairie) {
    if(GOLDBERG) return not_implemented;
    else if(stdeucx || (bigsphere && BITRUNCATED && !elliptic) || (geometry == gFieldQuotient)) ;
    else if(!bounded) return not_implemented;
    else return unbounded_only;
    }
  
  if(l == laTerracotta && !stdeucx && !(bigsphere))
    return great_walls_missing;

  // highlight Crossroads on Euclidean
  if(euclid && !quotient && (l == laCrossroads || l == laCrossroads4) && !penrose)
    return full_game; 

  if(sol && among(l, laCrossroads, laCrossroads4))
    return full_game; 
  
  if(sol && l == laCamelot)
    return not_implemented;

  if(euclid && quotient && !bounded && l == laCrossroads && euc::sdxy().second == -2 * euc::sdxy().first)
    return full_game;
  
  if(euclid && quotient && !bounded && l == laCrossroads4 && euc::sdxy().second == 0)
    return full_game;
  
  // highlight Zebra-based lands on Zebra Quotient!
  if((l == laZebra || l == laWhirlwind || l == laStorms || l == laWarpCoast || l == laWarpSea) && geometry == gZebraQuotient)
    return pattern_compatibility;
  
  // highlight FP-based lands on Field Quotient!
  if((l == laPrairie || l == laVolcano || l == laBlizzard) && geometry == gFieldQuotient)
    return pattern_compatibility; 

  // highlight Docks-based lands on Bolza and Bolza x2!
  if(l == laDocks && among(geometry, gBolza, gBolza2))
    return pattern_compatibility; 
  
  // these are highlighted whenever allowed
  if(l == laHalloween)
    return specially_designed;

  if(l == laDual && geosupport_threecolor() == 2)
    return specially_designed;
  
  if(l == laDual && !geometry && !GOLDBERG)
    return hyperbolic_37 ? not_in_full_game3 : not_in_full_game;
  
  if(l == laSnakeNest && WDIM == 2) {
    if(geosupport_threecolor() < 2)
      return needs_threecolor;
    else return specially_designed;
    }

  if(l == laDocks && !randomPatternsMode) {
    if(a38 && !GOLDBERG && !nonisotropic) return specially_designed;
    if(a38 && !nonisotropic) return pattern_not_implemented_weird;
    return pattern_not_implemented_exclude;
    }
  
  if(l == laStorms && euclid && bounded) 
    return interesting;
  
  if(l == laMagnetic)
    return land_not_implemented;

  if(shmup::on && among(l, laMirror, laMirrorOld) && among(geometry, gElliptic, gZebraQuotient, gKleinQuartic, gBolza, gBolza2, gMinimal))
    return known_buggy;

  // these don't appear in normal game, but do appear in special modes
  if(l == laWildWest && !randomPatternsMode)
    return out_of_theme;
  
  if(l == laIce && STDVAR && hyperbolic_37 && !quotient && !archimedean && !binarytiling)
    return full_game;

  return ok;
  }

/*
int checkLands() {
  for(int i=0; i<landtypes; i++) {
    eLand l = eLand(i);
    char inover = 'N';
    for(int i=0; i<LAND_OVERX; i++) if(land_over[i] == l) inover = 'X';
    for(int i=0; i<LAND_OVER; i++) if(land_over[i] == l) inover = 'Y';
    char ineuc = 'N';
    for(int i=0; i<LAND_EUC; i++) if(land_euc[i] == l) ineuc = 'Y';
    char insph = 'N';
    for(int i=0; i<LAND_SPH; i++) if(land_sph[i] == l) insph = 'Y';
    char inoct = 'N';
    for(int i=0; i<LAND_OCT; i++) if(land_oct[i] == l) inoct = 'Y';
    char intac = 'N';
    for(auto ti: land_tac) if(l == ti.l) intac = 'Y';
    printf("%c %c %c %c %c %c %c :: %s\n", 
      isTechnicalLand(l) ? 'T' : 'R',
      inover, ineuc, insph, inoct, intac, noChaos(l) ? 'N' : 'Y', linf[i].name);
    }
  exit(0);
  }

auto hookcl = addHook(hooks_args, 100, checkLands); */

}
