// Hyperbolic Rogue -- land availability
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file landlock.cpp
 *  \brief unlocking lands, which lands could be found beyond the wall, validity of various lands depending on the settings
 */

#include "hyper.h"
namespace hr {

EX bool in_full_game() {
  if(ls::single()) return false;
  return true;
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

#if HDR
enum eLandStructure { lsNiceWalls, lsChaos, lsPatchedChaos, lsTotalChaos, lsChaosRW, lsWallChaos, lsSingle, lsNoWalls, lsGUARD };
#endif

EX eLandStructure land_structure;

EX namespace ls {

EX bool single() { return land_structure == lsSingle; }

EX bool any_chaos() { return among(land_structure, lsChaos, lsPatchedChaos, lsWallChaos, lsTotalChaos, lsChaosRW); }
EX bool std_chaos() { return land_structure == lsChaos; }
EX bool wall_chaos() { return land_structure == lsWallChaos; }
EX bool patched_chaos() { return land_structure == lsPatchedChaos; }

EX bool any_order() { return among(land_structure, lsNiceWalls, lsNoWalls); }
EX bool nice_walls() { return land_structure == lsNiceWalls; }
EX bool no_walls() { return land_structure == lsNoWalls; }

EX bool any_nowall() { return no_walls() || std_chaos(); }
EX bool any_wall() { return nice_walls() || wall_chaos(); }

EX int chaoticity() {
  if(land_structure == lsTotalChaos) return 100;
  if(land_structure == lsChaosRW) return 80;
  if(land_structure == lsPatchedChaos) return 60;
  if(land_structure == lsChaos) return 40;
  if(land_structure == lsWallChaos) return 30;
  if(land_structure == lsSingle) return 0;
  return 10;  
  }

EX bool tame_chaos() { return any_chaos() && chaoticity() < 35; }
EX }

EX string land_structure_name(bool which) {
  switch(land_structure) {
    case lsNiceWalls:
      return XLAT("standard");
    case lsChaos:
      return XLAT("Chaos mode");
    case lsPatchedChaos:
      return XLAT("patched Chaos");
    case lsWallChaos:
      return XLAT("excessive walls");
    case lsTotalChaos:
      return XLAT("total chaos");
    case lsChaosRW:
      return XLAT("random-walk chaos");
    case lsSingle:
      return which ? XLAT("single land: ") + XLATN(linf[specialland].name) : XLAT("single land");
    case lsNoWalls:
      return XLAT("wall-less");
    default:
      return "error structure";
    }
  }

EX void fix_land_structure_choice() {  
  if(closed_or_bounded) {
    if(land_structure != lsTotalChaos && land_structure != lsChaosRW)
      land_structure = lsSingle;
    }
  if(tactic::on || princess::challenge)
    land_structure = lsSingle;
  if(yendor::on)
    land_structure = yendor::get_land_structure();
  if(!nice_walls_available() && land_structure == lsNiceWalls)
    land_structure = lsNoWalls;
  if(!nice_walls_available() && land_structure == lsWallChaos)
    land_structure = lsChaos;
  if(walls_not_implemented() && among(land_structure, lsChaos, lsNoWalls))
    land_structure = lsSingle;
  if(land_structure == lsPatchedChaos && !(stdeuc || nil || cryst || (euclid && WDIM == 3)))
    land_structure = lsSingle;
  if(closed_or_bounded && !among(land_structure, lsChaosRW, lsTotalChaos, lsSingle))
    land_structure = lsSingle;
  }

EX bool landUnlockedRPM(eLand n) {
  if(isRandland(n) == 2) return true;
  if(isRandland(n) == 1)
    return (autocheat || cheater || hiitemsMax(treasureType(n)) >= 10);
  return false;
  }

EX int lands_for_hell() {
  return casual ? 40 : 9;
  }

EX int variant_unlock_value() {
  return inv::on ? 75 : 30;
  }

EX bool landUnlocked(eLand l) {
  if(randomPatternsMode) {
    return landUnlockedRPM(l);
    }
  
  if(all_unlocked) {
    if(autocheat || hiitemsMax(treasureType(l)) >= 10) return true;
    }
  
  back:
  
  if(princess::challenge) return among(l, laPalace, laPrincessQuest);
  
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
    #define ACCONLY3(x,y,z)
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
  if(ls::single() && isCrossroads(specialland)) return true;
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
  if(l1 == laGraveyard && l2 == laDice) return true;
  if(l1 == laGraveyard && l2 == laRuins) return true;
  if(l1 == laGraveyard && l2 == laRedRock) return true;
  if(l1 == laGraveyard && l2 == laEmerald) return true;
  if(l1 == laDeadCaves && l2 == laEmerald) return true;
  if(l1 == laDeadCaves && l2 == laCaves) return true;
  if(l1 == laWarpSea && l2 == laKraken) return true;
  if(l1 == laPrairie && l2 == laCrossroads3) return true;
  if(l1 == laPrairie && l2 == laCrossroads4) return true;
  if(l1 == laWet && l2 == laDesert) return true;
  if(l1 == laFrog && l2 == laMotion) return true;
  if(l1 == laBull && l2 == laTerracotta) return true;
  if(l1 == laReptile && l2 == laTerracotta) return true;
  if(l1 == laBull && l2 == laDeadCaves) return true;
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
  if(all_unlocked) return pick(laEAir, laEWater, laEEarth, laEFire);
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
  if(ls::any_chaos()) return items[itStatue] >= 1;
  return items[itStatue] >= 10 || items[itGrimoire] >= 10;
  }

bool lchance(eLand l) { 
  if(ls::single() || racing::on || ((geometry || GOLDBERG) && specialland == laElementalWall)) return true;
  if(ls::any_chaos()) return hrand(100) < 25;
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
    (old == laOcean && (ls::any_chaos() ? hrand(2) : !generatingEquidistant));
  }

EX hookset<eLand(eLand)> hooks_nextland;

EX bool all_unlocked = false;

EX vector<eLand> cheatdest_list;

EX eLand getNewLand(eLand old) {

  #if CAP_LEGACY
  if(legacy_racing()) {
    if(old == laMirror && hrand(10) >= ((tactic::on || racing::on) ? 0 : markOrb(itOrbLuck) ? 5 : 2)) return laMirrored;
    if(old == laTerracotta && hrand(5) >= ((tactic::on || racing::on) ? 0 : markOrb(itOrbLuck) ? 2 : 1) && !weirdhyperbolic) return laTerracotta;
    }
  #endif

  eLand l = callhandlers(laNone, hooks_nextland, old);
  if(l) return l;
  
  if(cheatdest != old && cheatdest != laElementalWall) if(!isCyclic(cheatdest) && !isTechnicalLand(cheatdest)) return cheatdest;
  
  if(cheatdest_list.size()) {
    eLand l = cheatdest_list[0];
    std::rotate(cheatdest_list.begin(), cheatdest_list.begin()+1, cheatdest_list.end());
    return l;
    }

  if(old == laTortoise) return laDragon;

  if(yendor::on && ls::any_chaos()) {
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

  if(tactic::on) return specialland;
  if(specialland != old && easy_to_find_specialland && specialland != laElementalWall) return specialland;

  if(specialland != old && easy_specialland && specialland != laElementalWall) {
    easy_specialland--;
    return specialland;
    }

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
  
  if(isWarpedType(old) && (hrand(100) < 25) && ls::std_chaos()) return eLand(old ^ laWarpCoast ^ laWarpSea);

  if(createOnSea(old)) 
      return getNewSealand(old);

  if(old == laGraveyard && generatingEquidistant)
    return laHaunted;
  
  if(old == laOcean && gold() >= R60 && hrand(100) < 75 && !rlyehComplete() && !all_unlocked) 
    return laRlyeh;
    
  if(old == laRlyeh && !rlyehComplete() && !all_unlocked)
    return laOcean;
    
  eLand tab[16384];
  int cnt = 0;
  
  // return (hrand(2)) ? laMotion : laJungle;
  
  for(eLand l: {
    laCrossroads, laIce, laDesert, laJungle, laMotion, laHunting, laAlchemist, laCaves,
    laMinefield, laPalace, laReptile, laSwitch, laBurial, laDungeon, laRuins, laZebra,
    laStorms, laWhirlwind, laOvergrown, laBlizzard, laDryForest, laWineyard, laVolcano,
    laDeadCaves, laRedRock, laVariant, laHell, laCocytus, laPower,
    laBull, laTerracotta, laRose, laGraveyard, laHive, laDragon, laTrollheim,
    laWet, laFrog, laEclectic, laCursed, laDice,
    laCrossroads5,
    })
    if(landUnlocked(l)) tab[cnt++] = l;    

  struct clos {
    eLand l1;
    eLand l2;
    int f1;
    int f2;
    };
  
  for(clos c: { 
    clos{laZebra, laMotion, 2, 2}, {laZebra, laHunting, 2, 2},
    {laDragon, laReptile, 5, 5},
    {laVariant, laRuins, 5, 5}, {laVariant, laEmerald, 5, 5}, {laVariant, laGraveyard, 5, 5},
    {laPalace, laDungeon, 5, 0},
    {laJungle, laOvergrown, 5, 5},
    {laIce, laBlizzard, 5, 5}, {laCocytus, laBlizzard, 5, 5}, {laHell, laCocytus, 5, 5}, {laIce, laCocytus, 5, 5},
    {laWhirlwind, laBlizzard, 5, 5},
    {laAlchemist, laVolcano, 5, 5},
    {laDesert, laRedRock, 5, 5},
    {laFrog, laReptile, 2, 2}, {laFrog, laSwitch, 2, 2}, {laFrog, laZebra, 2, 2},
    {laEclectic, laStorms, 3, 3}, {laEclectic, laIce, 3, 3}, {laEclectic, laPalace, 3, 3}, {laEclectic, laDeadCaves, 3, 3},
    
    {laEFire, laDragon, 5, 5}, {laEWater, laLivefjord, 5, 5}, {laEEarth, laDeadCaves, 5, 5}, {laEAir, laWhirlwind, 5, 5},
    }) {
    if(old == c.l1 && landUnlocked(c.l2)) for(int i=0; i<c.f1; i++) tab[cnt++] = c.l2;
    if(old == c.l2 && landUnlocked(c.l1)) for(int i=0; i<c.f2; i++) tab[cnt++] = c.l1;
    }
  
  // these lands tend to crash while generating equidistant
  for(eLand l: {laIvoryTower, laEndorian, laWestWall})
    if(landUnlocked(l) && !generatingEquidistant)
      tab[cnt++] = l;

  // the intermediate lands
  if(all_unlocked || gold() >= R30) {
    tab[cnt++] = laCrossroads;
    tab[cnt++] = geometry ? laMirrorOld : laMirror;
    tab[cnt++] = laOcean;
    tab[cnt++] = laLivefjord;
    if(all_unlocked || kills[moVizier]) tab[cnt++] = laEmerald;
    tab[cnt++] = laWarpCoast;
    if(euclid) tab[cnt++] = laWarpSea;
    tab[cnt++] = laDocks;
    }

  // the advanced lands
  if(all_unlocked || gold() >= R60) {
    tab[cnt++] = laCrossroads;
    if(!generatingEquidistant) tab[cnt++] = laCrossroads2;
    if(all_unlocked || rlyehComplete()) tab[cnt++] = laRlyeh;
    else if(ls::std_chaos() && (old == laWarpCoast || old == laLivefjord || old == laOcean)) 
      tab[cnt++] = laRlyeh;
    if((all_unlocked || items[itStatue] >= U5) && ls::std_chaos())
      tab[cnt++] = laTemple;
    if(old == laCrossroads || old == laCrossroads2) tab[cnt++] = laOcean;
    if(old == laOcean) tab[cnt++] = laCrossroads;
    if(items[itGold] >= U5 && items[itFernFlower] >= U5 && !kills[moVizier] && !all_unlocked)
      tab[cnt++] = laEmerald;
    }

  if(all_unlocked || gold() >= R90) {
    if(!ls::std_chaos()) tab[cnt++] = laPrairie;
    if(old == laPrairie) LIKELY tab[cnt++] = laBull;
    if(old == laBull && !ls::any_chaos()) LIKELY tab[cnt++] = laPrairie;
    tab[cnt++] = laDual;
    tab[cnt++] = laSnakeNest;
    }
  
  if(landUnlocked(laTrollheim)) {
    if(isTrollLand(old)) LIKELY tab[cnt++] = laTrollheim;
    if(old == laTrollheim) for(int i=0; i<landtypes; i++) {
      eLand l2 = eLand(i);
      if(isTrollLand(l2)) LIKELY tab[cnt++] = l2;
      }
    }

  if(landUnlocked(laElementalWall)) {
    tab[cnt++] = randomElementalLandWeighted();    
    }
  
  if(landUnlocked(laHell)) {
    if(!generatingEquidistant && old != laPrairie) tab[cnt++] = laCrossroads3;
    }
  
  if(items[itHell] >= U10) {
    if(old == laCrossroads || old == laCrossroads2) tab[cnt++] = laOcean;
    if(old == laOcean) tab[cnt++] = laCrossroads2;
    }
  
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
  laMirror, laMirrorOld, laMinefield, laPalace, laPrincessQuest, laZebra, laSwitch, laReptile, laWet,
  laOcean, laDocks, laWarpCoast, laLivefjord, laKraken, laCaribbean, laBrownian, laWhirlpool, laRlyeh, laTemple,
  laIvoryTower, laEndorian, laWestWall, laDungeon, laMountain, 
  laCrossroads2, 
  laDryForest, laWineyard, laDeadCaves, laGraveyard, laHaunted, laHive, 
  laRedRock, laVolcano,
  laDragon, laTortoise, laDice,
  laOvergrown, laClearing, laStorms, laBurial, laWhirlwind, 
  laBlizzard,
  laFrog, laEclectic, laCursed,
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
  static const flagtype switch_to_single = 64;
  }

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
  if((eubinary || sol) && isCyclic(l) && l != specialland) return false;
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
  land_validity_t pattern_compatibility_notrec = {2, qm2, "Patterns compatible."}; 
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
  land_validity_t single_only = {2, q0 | switch_to_single, "Available in single land mode only." };
  }

// old Daily Challenges should keep their validity forever
// set this number for historical values of land_validity

EX int old_daily_id = 1000000;

const int landscapes_when = 177;

EX const int frog_when = 205;

EX const int cursed_when = 386;

EX const int walls_when = 388;

// check if the given land should appear in lists
EX land_validity_t& land_validity(eLand l) {

  bool stdeucx = stdeuc;
  if(euclid && quotient) stdeucx = false;

  using namespace lv;
  
  if(l == laDice && geometry == gNormal && PURE)
    return dont_work;

  if(l == laDice && WDIM == 3)
    return dont_work;
  
  if(old_daily_id < frog_when && among(l, laFrog, laEclectic, laWet))
    return not_implemented;

  if(old_daily_id < cursed_when && among(l, laCursed, laDice))
    return not_implemented;
  
  if(arb::in() && among(l, laWarpCoast, laDual, laEclectic, laReptile, laKraken))
    return not_implemented;
  
  if(l == laEclectic && !(geometry == gNormal && BITRUNCATED))
    return pattern_not_implemented_weird;
  
  if(l == laFrog && shmup::on)
    return not_in_shmup;

  if(walls_not_implemented() && isCrossroads(l))
    return no_walls;
  
  if(mhybrid || hybrid::pmap) {
    if(among(l, laPrincessQuest, laPrairie, laMirrorOld, laMirror, laDual, laWarpCoast, laKraken, laBrownian, laWhirlpool, laWestWall, laHive, laClearing, laWhirlwind, laBlizzard, laBull, laTerracotta, laCrossroads5,
      laEndorian, laDungeon, laMountain))
      return lv::not_implemented;
    if(among(l, laReptile, laDragon, laTortoise))
      return lv::bad_graphics;
    if((hybrid::actual_geometry == gRotSpace || geometry == gRotSpace) && l == laDryForest)
      return lv::hedgehogs;
    if(mhybrid && hybrid::underlying && hybrid::underlying_cgip) {
      return *PIU(&land_validity(l));
      }
    }
  
  #if !CAP_FIELD
  if(among(l, laPrairie, laBlizzard, laVolcano))
    return disabled;
  #endif

  #if !CAP_COMPLEX2
  if(among(l, laBrownian, laWestWall, laVariant))
    return disabled;
  #endif

  if(l == laMinefield && closed_or_bounded)
    return special_geo3;
  
  if(l == laAsteroids) {
    if(!shmup::on) return shmup_only;
    if(!closed_manifold) return bounded_only;
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
    if(l == laWineyard && hyperbolic && !bt::in() && S7 == 6) return lv::pattern_special;
    if(l == laEmerald && hyperbolic && !bt::in() && S7 == 12) return lv::pattern_special;
    if(l == laZebra) return pattern_not_implemented_random;
    if(among(l, laWhirlpool, laPrairie, laWestWall, laBull)) return lv::not_3d;
    if(l == laEndorian && geometry == gKiteDart3) return not_implemented;
    if(l == laEndorian && sol) return not_implemented;
    if(l == laEndorian && hyperbolic && !quotient) return lv::pattern_special;
    if(l == laIvoryTower && hyperbolic && bt::in()) return lv::pattern_special;
    if(l == laDungeon || l == laBrownian) return not_implemented;
    if(l == laKraken) return bt::in() ? not_binary : not_implemented;
    if(l == laBurial && !shmup::on) return not_implemented;
    if(l == laMirrorOld && !shmup::on) return not_implemented;
    }
  
  if(l == laBrownian) {
    if(quotient || !hyperbolic || cryst) return dont_work;
    }
  
  if(bt::in()) {
    if(among(l, laMountain, laTemple)) return lv::pattern_compatibility;
    if(among(l, laDungeon, laIvoryTower, laOcean, laEndorian)) return lv::pattern_compatibility;
    if(among(l, laCaribbean, laCamelot)) return lv::pattern_compatibility_notrec;
    // Clearing -- does not generate
    /* laCamelot, laCaribbean -> they are OK but not recommended */
    }
  
  #if CAP_ARCM
  if(arcm::in()) {
    if(among(l, laPower, laZebra, laFrog, laWineyard) && arcm::current.have_line) return lv::pattern_defined;
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

  // mirrors do not work in kite and sol
  if(among(l, laMirror, laMirrorOld) && (kite::in() || sol))
    return dont_work;
  
  if(isCrossroads(l) && geometry == gBinary4)
    return not_implemented;

  if(bt::in() && among(l, laMirror, laMirrorOld))
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
  if(l == laHalloween && !closed_or_bounded)
    return bounded_only;
  
  // Crystal World is designed for nice_dual geometries
  if(l == laDual && (!has_nice_dual() || nonisotropic))
    return dont_work;
  
  if(l == laHaunted && ls::std_chaos())
    return not_in_chaos;
  
  // standard, non-PTM specific
  if(l == laCrossroads5 && old_daily_id < 999 && tactic::on)
    return not_in_ptm;
    
  // standard non-PTM non-chaos specific
  if((l == laCrossroads5 || l == laCrossroads2) && (geometry || ls::any_chaos() || ls::no_walls()))
    return some0;
    
  // special construction in the Chaos mode
  if(ls::any_chaos() && (l == laTemple || l == laHive || l == laOcean || l == laHaunted))
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
    
  if(arcm::in() && l == laPrairie) return dont_work;

  if((IRREGULAR || arcm::in()) && among(l, laBlizzard, laVolcano) && !sphere)
    return dont_work;

  if(arcm::in() && DUAL && l == laCrossroads4)
    return not_implemented;
  
  if(geometry == gKiteDart3 && l == laGraveyard)
    return lv::pattern_special;
  
  // equidistant-based lands
  if(isEquidLand(l)) {
    // no equidistants supported in chaos mode
    if(ls::any_chaos()) 
      return not_in_chaos;
    // the algorithm fails in Archimedean DUAL
    if(arcm::in() && DUAL)
      return not_implemented;
    // the algorithm fails in Binary4
    if(geometry == gBinary4)
      return not_implemented;
    // no equidistants supported in these geometries (big sphere is OK though)
    if(closed_or_bounded && !bigsphere)
      return unbounded_only_except_bigsphere;
    // Yendorian only implemented in standard
    if(l == laEndorian && geometry)
      return not_implemented;
    // special Euclidean implementations
    if(euclid && !ls::single() && old_daily_id > 9999) return single_only;
    if(euclid && (l == laIvoryTower || l == laMountain || l == laOcean || l == laMountain)) {
      return special_geo;
      }
    // in other geometries, it works
    if(geometry)
      return ok;
    }

  if(euclid && l == laCaribbean && !ls::single() && old_daily_id > 9999) return single_only;
  
  if(l == laPrincessQuest && ls::any_chaos())
    return not_in_chaos;
   
  if(l == laPrincessQuest && tactic::on)
    return not_in_ptm;
    
  if(l == laPrincessQuest && (!stdeucx || NONSTDVAR))
    return not_implemented;
  
  if(l == laPrincessQuest && shmup::on)
    return not_in_shmup;

  if(l == laPrincessQuest && multi::players > 1)
    return not_in_multi;

  if(l == laMountain && ls::any_chaos())
    return not_in_chaos;
  
  if(l == laBrownian && ls::any_chaos())
    return not_in_chaos;
  
  // works correctly only in some geometries
  if(l == laClearing && ls::any_chaos())
    return not_in_chaos;
  
  if(l == laClearing)
    if(!(stdeucx || geometry == gBinaryTiling || a38 || (a45 && BITRUNCATED) || (a47 && BITRUNCATED)) || NONSTDVAR)
    if(!closed_or_bounded)
      return not_implemented;

  // does not work in non-bitrunc a4
  if(l == laOvergrown && a4 && !BITRUNCATED)
    return some0;

  // does not work in bounded either
  if(l == laOvergrown && closed_or_bounded)
    return some0;
  
  // horocycle-based lands, not available in bounded geometries nor in Chaos mode
  if(l == laWhirlpool || l == laCamelot || l == laCaribbean || l == laTemple || l == laHive) {
    if(ls::any_chaos()) {
      if(l == laTemple || l == laHive) 
        return special_chaos;
      return not_in_chaos;
      }
    if(arcm::in() || kite::in()) return not_implemented;
    if(closed_or_bounded) return unbounded_only;
    if(INVERSE) return not_implemented;
    }
  
  if(ls::any_chaos() && isCrossroads(l))
    return not_in_chaos;
  
  // this pattern does not work on elliptic and small spheres
  if((l == laBlizzard || l == laVolcano) && elliptic && S7 < 5 && !arcm::in())
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
  if(l == laCA && !closed_or_bounded)
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
  
  if(among(l, laEmerald, laCamelot, laDryForest) && valence() != 3 && old_daily_id >= 65)
    return hedgehogs;  

  // laPower and laEmerald and laPalace -> [partial] in quotients and hyperbolic_non37
  if((l == laPower || l == laEmerald || l == laPalace || l == laWildWest) && !randomPatternsMode) {
    if(euclid || bigsphere) ;
    else if(old_daily_id <= 65 && a45) ;
    else if(!hyperbolic_37) return l == laWildWest ? some0 : pattern_not_implemented_random;
    else if(quotient) return pattern_incompatibility;
    }

  if(among(l, laEmerald, laCamelot, laDryForest) && valence() != 3)
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
  
  if(l == laTrollheim && !stdeucx && !closed_or_bounded)
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

  if(among(l, laCrossroads, laCrossroads2, laCrossroads3, laCrossroads5) && weirdhyperbolic && old_daily_id < walls_when)
    return no_great_walls;

  // Crossroads IV is great in weird hyperbolic
  if(l == laCrossroads4 && weirdhyperbolic && !quotient)
    return full_game;

  // OK in small bounded worlds, and in Euclidean  
  if(l == laCrossroads4 && quotient)
    return some0;

  if(among(l, laZebra, laFrog) && quotient && geometry != gZebraQuotient && !randomPatternsMode)
    return pattern_incompatibility;
  
  if(among(l, laZebra, laFrog) && !(stdeucx || (a4 && !BITRUNCATED) || a46 || (geometry == gZebraQuotient && old_daily_id > 106)) && !randomPatternsMode)
    return pattern_not_implemented_weird;
  
  if(l == laCrossroads3 && euclid)
    return inaccurate; // because it is not accurate

  if(l == laPrairie) {
    if(GOLDBERG) return not_implemented;
    else if(stdeucx || (bigsphere && BITRUNCATED && !elliptic) || (geometry == gFieldQuotient)) ;
    else if(!closed_or_bounded) return not_implemented;
    else return unbounded_only;
    }
  
  if(l == laTerracotta && !stdeucx && !(bigsphere))
    return great_walls_missing;

  // highlight Crossroads on Euclidean
  if(euclid && !quotient && (l == laCrossroads || l == laCrossroads4) && !kite::in())
    return full_game; 

  if(sol && among(l, laCrossroads, laCrossroads4))
    return full_game; 
  
  if(sol && l == laCamelot)
    return not_implemented;

  if(euclid && quotient && !closed_or_bounded && l == laCrossroads && euc::sdxy().second == -2 * euc::sdxy().first)
    return full_game;
  
  if(euclid && quotient && !closed_or_bounded && l == laCrossroads4 && euc::sdxy().second == 0)
    return full_game;
  
  // highlight Zebra-based lands on Zebra Quotient!
  if(among(l, laZebra, laWhirlwind, laStorms, laWarpCoast, laWarpSea, laFrog) && geometry == gZebraQuotient)
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
  
  if(l == laStorms && euclid && closed_manifold)
    return interesting;
  
  if(l == laMagnetic)
    return land_not_implemented;

  if(shmup::on && among(l, laMirror, laMirrorOld) && among(geometry, gElliptic, gZebraQuotient, gKleinQuartic, gBolza, gBolza2, gMinimal))
    return known_buggy;

  // these don't appear in normal game, but do appear in special modes
  if(l == laWildWest && !randomPatternsMode)
    return out_of_theme;
  
  if(l == laIce && STDVAR && hyperbolic_37 && !quotient && !arcm::in() && !bt::in())
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
