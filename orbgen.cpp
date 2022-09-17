// Hyperbolic Rogue -- orb generation routines
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file orbgen.cpp
 *  \brief Orb generation: which orbs in which lands, functions generating prize/local orbs, etc.
 */

#include "hyper.h"
namespace hr {
// orbgen flags

#if HDR
enum eOrbLandRelation { 
  olrForbidden, // never appears: forbidden
  olrDangerous, // never appears: would be dangerous
  olrUseless,   // never appears: useless here
  olrNoPrizes,  // no prizes in this land
  olrNoPrizeOrb,// orb not allowed as a prize
  olrPrize25,   // prize for collecting 25
  olrPrize3,    // prize for collecting 3
  olrNative,    // native orb in this land
  olrNative1,   // native orb in this land (1)
  olrGuest,     // extra orb in this land
  olrPNative,   // Land of Power: native
  olrPBasic,    // Land of Power: basic orbs
  olrPPrized,   // Land of Power: prized orbs
  olrPNever,    // Land of Power: foreign orbs
  olrHub,       // hub lands
  olrMonster,   // available from a monster
  olrAlways,    // always available
  olrBurns      // burns
  };
#endif

#if HDR
namespace orbgenflags {
  // generates in the given land from 10 treasures, in the classic mode
  static const int LOCAL10 = 1;
  // generates in the Crossroads from 10 treasures, in the classic mode
  static const int CROSS10 = 2;
  // generates in other places from 25 treasures, in the classic mode
  static const int GLOBAL25 = 4;
  // in OSM you get it once at 10 treasures
  static const int OSM_AT10 = 8;


  // 'native' functions return this
  static const int NATIVE = 64;
  // 'local' orb will be also placed in OSM (at 25 treasures); needs LOCAL10
  static const int OSM_LOCAL25 = 128;
  // 'crossroads' orb will be also placed in OSM (at 50 treasures)
  static const int OSM_CROSS50 = 256;
  // 'crossroads' orb will be also placed in OSM (at 25 treasures)
  static const int OSM_CROSS25 = 512;
  // 'global' orb will be also placed in OSM (at 100 treasures)
  static const int OSM_GLOBAL100 = 1024;
  // do not create in the Crossroads in the tactics mode
  static const int NO_TACTIC = (1<<11);
  // guest Orb
  static const int GUEST = (1<<12);
  
  // typical combinations
  static const int S_NATIVE = LOCAL10 | CROSS10 | GLOBAL25 | NATIVE;
  static const int S_GUEST  = LOCAL10 | OSM_AT10 | GUEST;
  static const int S_YENDOR = S_NATIVE | OSM_LOCAL25 | OSM_CROSS50 | OSM_GLOBAL100 | NO_TACTIC;
  static const int S_NAT_NT = S_NATIVE | NO_TACTIC;
  static const int S_NA_O25 = S_NATIVE | OSM_CROSS25;
  }

struct orbinfo {
  int flags;
  eLand l;
  int lchance;
  int gchance;
  eItem orb;
  bool is_native() const { using namespace orbgenflags; return flags & NATIVE; }
  };
#endif

EX vector<orbinfo> orbinfos = {
  {orbgenflags::S_NATIVE, laGraveyard, 200, 200,itGreenStone}, // must be first so that it does not reduce 
  {orbgenflags::S_NATIVE, laJungle, 1200, 1500,itOrbLightning},
  {orbgenflags::S_NATIVE, laIce, 2000, 1500,itOrbFlash},
  {orbgenflags::S_NATIVE, laCaves, 1800, 2000,itOrbLife},
  {orbgenflags::S_NATIVE, laAlchemist, 800, 800,itOrbSpeed},
  {orbgenflags::S_NATIVE, laDesert, 2500, 1500,itOrbShield},
  {orbgenflags::S_YENDOR, laHell, 2000, 1000,itOrbYendor},
  {orbgenflags::S_NATIVE, laRlyeh, 1500, 1500,itOrbTeleport},
  {orbgenflags::S_NA_O25, laMotion, 2000, 700, itOrbSafety},    
  {orbgenflags::S_GUEST,  laIce, 1500, 0, itOrbWinter},
  {orbgenflags::S_GUEST,  laDragon, 2500, 0, itOrbWinter},
  {orbgenflags::S_GUEST,  laDryForest, 2500, 0, itOrbWinter}, 
  {orbgenflags::S_NATIVE, laCocytus, 1500, 1500, itOrbMorph}, 
  {orbgenflags::S_GUEST,  laCocytus, 1500, 0, itOrbWinter}, 
  {orbgenflags::S_GUEST,  laCaves, 1200, 0, itOrbDigging},
  {orbgenflags::S_NATIVE, laDryForest, 500, 4500, itOrbThorns},
  {orbgenflags::S_GUEST,  laDeadCaves, 1800, 0, itGreenStone},
  {orbgenflags::S_NAT_NT, laDeadCaves, 1800, 1500, itOrbDigging},
  {orbgenflags::S_NATIVE, laEmerald, 1500, 3500, itOrbPsi},
  {orbgenflags::S_NATIVE, laWineyard, 900, 1200, itOrbAether},
  {orbgenflags::S_NATIVE, laHive, 800, 1200, itOrbInvis},
  {orbgenflags::S_NATIVE, laPower, 0, 3000, itOrbFire},
  {orbgenflags::S_NATIVE, laMinefield, 0, 3500, itOrbFriend},
  {orbgenflags::S_NATIVE, laTemple, 0, 3000, itOrbDragon},
  {orbgenflags::S_NATIVE, laCaribbean, 0, 3500, itOrbTime},
  {orbgenflags::S_NATIVE, laRedRock, 0, 2500, itOrbSpace},
  {orbgenflags::S_NATIVE, laCamelot, 1000, 1500, itOrbIllusion},
  {orbgenflags::S_NATIVE, laOcean, 0, 1500, itOrbEmpathy},
  {orbgenflags::S_GUEST,  laOcean, 0, 0, itOrbAir},
  {orbgenflags::S_NATIVE, laPalace, 0, 4000, itOrbDiscord},
  {orbgenflags::S_GUEST,  laPalace, 0, 0, itOrbFrog},
  {orbgenflags::S_NATIVE, laZebra, 500, 2100, itOrbFrog},
  {orbgenflags::S_NAT_NT, laLivefjord, 0, 1800, itOrbFish},
  {orbgenflags::S_NAT_NT, laPrincessQuest, 0, 200, itOrbLove},
  {orbgenflags::S_NATIVE, laIvoryTower, 500, 4000, itOrbMatter},
  {orbgenflags::S_NAT_NT, laElementalWall, 1500, 4000, itOrbSummon},
  {orbgenflags::S_NATIVE, laStorms, 1000, 2500, itOrbStunning},
  {orbgenflags::S_NATIVE, laOvergrown, 1000, 800, itOrbWoods},
  {orbgenflags::S_GUEST,  laOvergrown, 1000, 0, itOrbLuck},
  {orbgenflags::S_NATIVE, laWhirlwind, 1250, 3000, itOrbAir},
  {orbgenflags::S_NATIVE, laHaunted, 1000, 5000, itOrbUndeath},
  {orbgenflags::S_NATIVE, laClearing, 5000, 5000, itOrbFreedom},
  {orbgenflags::S_NATIVE, laRose, 2000, 8000, itOrbBeauty},
  {orbgenflags::S_NATIVE, laWarpCoast, 2000, 8000, itOrb37},
  {orbgenflags::S_NATIVE, laDragon, 500, 5000, itOrbDomination},
  {orbgenflags::S_NATIVE, laTortoise, 2500, 1500, itOrbShell},
  {orbgenflags::S_NATIVE, laEndorian, 150, 2500, itOrbEnergy},
  {orbgenflags::S_GUEST,  laEndorian, 450, 0, itOrbTeleport},
  {orbgenflags::S_NATIVE, laKraken, 500, 2500, itOrbSword},
  {orbgenflags::S_NATIVE, laBurial, 500, 2500, itOrbSword2},
  {orbgenflags::S_NATIVE, laTrollheim, 750, 1800, itOrbStone},
  {orbgenflags::S_NATIVE, laMountain, 400, 3500, itOrbNature},
  {orbgenflags::S_NATIVE, laDungeon, 120, 2500, itOrbRecall},
  {orbgenflags::S_NATIVE, laReptile, 500, 2100, itOrbDash},
  {orbgenflags::S_NATIVE, laBull, 720, 3000, itOrbHorns},
  {orbgenflags::S_NATIVE, laPrairie, 0, 3500, itOrbBull},
  {orbgenflags::S_GUEST,  laWhirlpool, 0, 0, itOrbSafety},
  {orbgenflags::S_NATIVE, laVolcano, 0, 7000, itOrbLava},
  {orbgenflags::S_NATIVE, laHunting, 0, 2500, itOrbSide3},
  {orbgenflags::S_NATIVE, laBlizzard, 0, 2000, itOrbWinter},
  {orbgenflags::S_NATIVE, laTerracotta, 800, 2500, itOrbSide1},
  {orbgenflags::S_NATIVE, laDual, 600, 2500, itOrbSide2},
  {orbgenflags::S_GUEST, laSnakeNest, 2000, 0, itOrbDomination},
  {orbgenflags::S_GUEST, laDocks, 3000, 0, itOrbFish},
  {orbgenflags::S_GUEST, laDocks, 3000, 0, itOrbDragon},
  {orbgenflags::S_GUEST, laDocks, 3000, 0, itOrbDash},
  {orbgenflags::S_GUEST, laSwitch, 2000, 0, itOrbSpace}, 
  {orbgenflags::S_NATIVE, laSwitch, 2000, 3000, itOrbPhasing}, 
  {orbgenflags::S_NATIVE, laMagnetic, 2000, 3000, itOrbMagnetism},
  {orbgenflags::S_NATIVE, laRuins, 1200, 2500, itOrbSlaying},
  {orbgenflags::S_NATIVE, laWestWall, 2000, 4200, itOrbGravity},
  {orbgenflags::S_NATIVE, laVariant, 900, 4200, itOrbIntensity},
  {orbgenflags::S_GUEST, laVariant, 4000, 0, itOrbFire},
  {orbgenflags::S_GUEST, laVariant, 4000, 0, itOrbWinter},
  {orbgenflags::S_GUEST, laVariant, 4000, 0, itOrbPhasing},
  {orbgenflags::S_GUEST, laVariant, 4000, 0, itOrbDash},
  {orbgenflags::S_NATIVE, laBrownian, 900, 4200, itOrbChoice},
  {orbgenflags::S_GUEST, laBrownian, 1000, 0, itOrbAir},
  {orbgenflags::S_GUEST, laBrownian, 1000, 0, itOrbDash},
  {orbgenflags::S_GUEST, laBrownian, 1000, 0, itOrbSafety},
  {orbgenflags::S_NATIVE, laFrog, 1000, 1500, itOrbImpact},
  {orbgenflags::S_NATIVE, laEclectic, 1000, 1000, itOrbChaos},
  {orbgenflags::S_GUEST, laEclectic, 4000, 0, itOrbWinter},
  {orbgenflags::S_GUEST, laEclectic, 2000, 0, itOrbLightning},
  {orbgenflags::S_NATIVE, laWet, 800, 2500, itOrbPlague},
  {orbgenflags::S_GUEST, laWet, 1200, 0, itOrbFish},
  {orbgenflags::S_GUEST, laWet, 1200, 0, itOrbAether},
  {orbgenflags::S_GUEST, laWet, 1200, 0, itOrbFrog},
  {orbgenflags::S_NATIVE, laCursed, 400, 1500, itOrbPurity},
  {orbgenflags::S_NAT_NT, laDice, 500, 800, itOrbLuck},
  {orbgenflags::S_GUEST, laDice, 750, 0, itOrbAir},
  {orbgenflags::S_NATIVE, laWhirlpool, 0, 2000, itOrbWater}, // needs to be last
  };

EX eItem nativeOrbType(eLand l) {
  if(isElemental(l)) l = laElementalWall;
  if(inv::on && (l == laMirror || l == laMirrorOld || isCrossroads(l)))
    return itOrbMirror;
  if(l == laMirror || l == laMirrorOld) return itShard;
  for(auto& oi: orbinfos)
    if(oi.l == l && oi.is_native())
      return oi.orb;
  return itNone;
  }

const orbinfo& getNativityOrbInfo(eItem orb) {
  for(auto& oi: orbinfos)
    if(oi.orb == orb && oi.is_native())
      return oi;
  static orbinfo oi; 
  oi.l = laMirror;
  return oi;
  }

EX string olrDescriptions[18] = {
  "forbidden to find in %the1",
  "too dangerous to use in %the1",
  "useless in %the1",
  "only native Orbs allowed in %the1",
  "this Orb is never unlocked globally (only hubs)",
  "collect 25 %2 to unlock it in %the1",
  "collect 3 %2 to unlock it in %the1",
  "native to %the1 (collect 10 %2)",
  "native to %the1 (collect 1 %2)",
  "secondary in %the1 (collect 10 %3, or 25 %2)",
  "the native Orb of %the1",
  "this Orb appears on floors and is used by witches",
  "a prized Orb, it appears only in cabinets",
  "this Orb never appears in %the1",
  "Hub Land: orbs appear here if unlocked in their native land",
  "kill a monster, or collect 25 %2",
  "always available",
  "would be destroyed in %the1"
  };

EX eOrbLandRelation getOLR(eItem it, eLand l) {

  if(l == laMotion && it == itOrbChoice && !inv::on) return olrUseless;
  
  if(l == laPower) {
    if(it == itOrbFire) return olrPNative;

    if(
      it == itOrbFlash || it == itOrbSpeed || it == itOrbWinter || it == itOrbAether ||
      it == itOrbLife) return olrPBasic;

    if(
      it == itOrbLightning || it == itOrbThorns || it == itOrbInvis ||
      it == itOrbShield || it == itOrbTeleport || it == itOrbPsi ||
      it == itOrbDragon || it == itOrbIllusion || it == itOrbTime)
        return olrPPrized;
    
    return olrPNever;
    }
  
  if(it == itOrbWater && l == laWestWall) return olrUseless;
  if(it == itOrbSafety && l == laCrossroads5) return olrDangerous;
  if(it == itOrbFire && l == laKraken) return olrUseless;
  if(it == itOrbDragon && l == laKraken) return olrUseless;
  if(it == itOrbDigging && l == laKraken) return olrUseless;
  if(it == itOrbIllusion && l == laKraken) return olrUseless;
  
  if(it == itOrbYendor && among(l, laWhirlwind, laWestWall)) return olrUseless;
  
  if(it == itOrbLife && (l == laKraken)) return olrUseless;
  
  if(l == laVolcano && it == itOrbSafety) return olrDangerous;
  if(l == laVariant && it == itOrbSafety) return olrDangerous;
  if(l == laVolcano && itemBurns(it)) return olrBurns;
  
  if(it == itOrbAir && l == laAlchemist) return olrUseless;
  // if(it == itOrbShield && l == laMotion) return olrUseless;

  if(it == itOrbIllusion && l == laCamelot) return olrNative1;
  if(it == itOrbLove) return olrNoPrizeOrb;    
  if(nativeOrbType(l) == it) return olrNative;
  
  for(const orbinfo& oi: orbinfos)
    if((oi.flags & orbgenflags::GUEST) && oi.l == l && oi.orb == it)
      return olrGuest;
      
  if(it == itOrbLuck && l == laIvoryTower)
    return olrUseless;
  if(it == itOrbLuck && l == laEndorian)
    return olrUseless;
  if(it == itOrbLuck && l == laDungeon)
    return olrUseless;
  if(it == itOrbWater && l == laRedRock)
    return olrUseless;
  if(it == itOrbLuck && l == laTortoise)
    return olrUseless;
  if(it == itOrbLuck && l == laMountain)
    return olrUseless;
  if(it == itOrbLuck && l == laWestWall)
    return olrUseless;
   if(it == itOrbLuck && l == laCamelot)
    return olrUseless;
   if(it == itOrbLuck && l == laHaunted)
    return olrUseless;
   if(it == itOrbNature && l == laWineyard)
    return olrDangerous;
  if((it == itOrbFrog || it == itOrbDiscord) && l == laPrincessQuest)
    return olrGuest;
  if(it == itOrbDragon && l == laRlyeh)
    return olrMonster;
  
  if(it == itOrbSafety && l == laWhirlpool)
    return olrAlways;
  if(it == itOrbSafety && l == laPrairie)
    return olrAlways;
  if(it == itGreenStone && isHaunted(l))
    return olrAlways;
  if(it == itOrbWater && l == laLivefjord)
    return olrMonster;
  if(isCrossroads(l) || l == laOcean)
    return olrHub;

  if(it == itOrbSlaying && !among(l, 
    laMirror, laHell, laEmerald, laDryForest, laCamelot, laPalace, laStorms, laRose, laTortoise, laBurial, laDungeon, laReptile, 
    laPrairie, laBull, laVolcano, laTerracotta, laRuins, laVariant, laEclectic, laBrownian, laCursed))
    return olrUseless;
  
  if(l == laCocytus)
    if(it == itOrbDragon || it == itOrbFire || it == itOrbFlash || it == itOrbLightning)
      return olrDangerous;
  
  if(it == itOrbSafety)
    if(l == laCaves || l == laLivefjord || l == laRedRock || l == laCocytus || l == laHell ||
      l == laDesert || l == laAlchemist || l == laDeadCaves || l == laMinefield || isHaunted(l) ||
      l == laDragon || l == laWet || l == laCursed)
      return olrDangerous;
    
  if(it == itOrbMatter)
    if(among(l, laCaves, laEmerald, laAlchemist, laCaribbean, laMinefield, laCocytus, laWestWall))
      return olrUseless;

  if(l == laPrincessQuest)
    if(among(it, itOrbAether, itOrbFlash, itOrbTeleport, itOrbSummon, itOrbFreedom, itOrbFriend, itOrbPhasing, itOrbChaos))
      return olrForbidden;
    
  if(l == laTemple)
    return olrNoPrizes;
  
  if(it == itOrbDigging) {
    if(among(l, laCaves, laOcean, laLivefjord, laEmerald, laDesert, laDeadCaves, laRedRock, laCaribbean, laGraveyard,
      laMountain, laHunting, laWarpSea, laWarpCoast, laCursed, laWet))
        return olrPrize25;
    return olrUseless;
    }

  if(it == itOrbWoods && (l == laWarpSea || l == laWarpCoast))
    return olrDangerous;

  if(it == itOrbWoods && !among(l,
    laDryForest, laWineyard, laCaribbean, laOvergrown, laHaunted, laHauntedWall, laHauntedBorder, laTortoise, laFrog, laEclectic,
    laVariant, laBull))
    return olrUseless;
  
  if(it == itShard) {
    if(l == laDesert || l == laIce || l == laJungle || l == laGraveyard ||
      l == laRlyeh || l == laHell || l == laDryForest || l == laWineyard ||
      l == laHive || l == laCamelot || l == laRedRock || l == laPalace ||
      l == laLivefjord || l == laZebra || isElemental(l) || l == laPrincessQuest ||
      l == laDragon || l == laTortoise || l == laBurial || l == laTrollheim ||
      l == laOcean || l == laHaunted || l == laWarpCoast || l == laRose)
      return olrPrize25;
    return olrForbidden;
    }

  if(it == itOrbWater) 
    if(l == laMotion || l == laZebra || l == laIvoryTower || l == laEndorian ||
      l == laMountain || l == laReptile || l == laDungeon)
      return olrUseless;
  
  if(it == itOrbWinter && l == laMinefield)
    return olrForbidden;
  
  if(it == itOrbWinter && !among(l, laRlyeh, laTemple, laVolcano, laCursed)) 
    return olrUseless;
  
  if(it == itOrbLife && l == laMotion)
    return olrUseless;
  
  if(it == itOrbFish && l == laKraken)
    return olrAlways;
    
  if(it == itOrbSword && l == laBurial)
    return olrAlways;
    
  if(it == itOrbFish && !among(l, laOcean, laLivefjord, laWhirlpool, laCamelot, laTortoise, laWarpCoast, laWarpSea, laCocytus, laBrownian, laVariant, laWet, laFrog))
    return olrUseless;

  if(it == itOrbDomination && l != laOcean && l != laRedRock && l != laDesert &&
    l != laRlyeh && l != laDragon)
    return olrUseless;

  if(it == itOrbIllusion) return olrPrize3;
  
  if(l == laTortoise)
    if(it == itOrbFlash || it == itOrbLightning || it == itOrbFreedom ||
      it == itOrbPsi || it == itOrbFriend || it == itOrbDragon)
      return olrForbidden;
    
  if(l == laEndorian)
    if(it == itOrbDragon || it == itOrbFire || it == itOrbLightning || it == itOrbLava)
      return olrDangerous;
    
  if(isHaunted(l) && it == itOrbLava)
    return olrUseless;
  
  if(l == laGraveyard && it == itOrbLava)
    return olrUseless;
    
  if(l == laDryForest)
    if(it == itOrbFire || it == itOrbLightning || it == itOrbLava)
      return olrDangerous;

  if(l == laWet && among(it, itOrbDragon, itOrbLava, itOrbFire))  
    return olrUseless;
    
  if(l == laDungeon) {
    if(it == itOrbSafety || it == itOrbFrog || 
      it == itOrbTeleport || it == itOrbMatter || it == itOrbNature ||
      it == itOrbAether || it == itOrbSummon || it == itOrbStone || it == itOrbChaos)
      return olrForbidden;
    }

  if(l == laTerracotta && it == itOrbFriend)
    return olrDangerous;
  
  return olrPrize25;
  }

EX int orbsUnlocked() {
  int i = 0;
  for(int t=0; t<ittypes; t++) 
    if(itemclass(eItem(t)) == IC_TREASURE && items[t] >= R10)
      i++;
  return i;
  }

EX ld orbprizefun(int tr) {
  if(tactic::on) return 1;
  if(tr < 10) return 0;
  return .6 + .4 * log(tr/25.) / log(2);
  }

EX ld orbcrossfun(int tr) {
  if(tactic::on || (ls::single() && isCrossroads(specialland))) return 1;
  if(tr < 10) return 0;
  if(tr > 25) return 1;
  return (tr*2 + 50) / 100.;
  }

EX bool buildPrizeMirror(cell *c, int freq) {
  if(inv::on) return false;
  if(items[itShard] < 25) return false;
  if(freq && hrand(freq * 100 / orbprizefun(items[itShard])) >= 100)
    return false;
  return mirror::build(c);
  }                    

#if HDR
extern cellwalker cwt;
#endif

EX eLand getPrizeLand(cell *c IS(cwt.at)) {
  eLand l = c->land;
  if(isElemental(l)) l = laElementalWall;
  if(isHaunted(l)) l = laHaunted;
  if(l == laMercuryRiver) l = laTerracotta;
  if(l == laWarpSea) l = laWarpCoast;
  if(l == laPalace && princess::dist(c) < OUT_OF_PRISON)
    l = laPrincessQuest;
  return l;
  }

EX void placePrizeOrb(cell *c) {
  if(peace::on) return;
  if(daily::on) return;
  
  eLand l = getPrizeLand(c);

  // these two lands would have too much orbs according to normal rules
  if(l == laPalace && hrand(100) >= 20) return;
  if(l == laPrincessQuest && hrand(100) >= 20) return;
  if(l == laGraveyard && hrand(100) >= 15) return;
  if(l == laBurial && hrand(100) >= 10) return;
  if(l == laLivefjord && hrand(100) >= 35) return;
  if(l == laMinefield && hrand(100) >= 25) return;
  if(l == laElementalWall && hrand(100) >= 25) return;

  for(auto& oi: orbinfos) {
    if(!(oi.flags & orbgenflags::GLOBAL25)) continue;

    int mintreas = 25;
    if(inv::on) {
      if(oi.flags & orbgenflags::OSM_GLOBAL100) mintreas = 100;
      else continue;
      }

    eOrbLandRelation olr = getOLR(oi.orb, l);
    if(olr != olrPrize25 && olr != olrPrize3) continue;
    int treas = items[treasureType(oi.l)];
    if(olr == olrPrize3) treas *= 10;
    if(olr == olrPrize25 || olr == olrPrize3 || olr == olrGuest || olr == olrMonster || olr == olrAlways) {
      if(treas < mintreas) continue;
      } 
    else continue;

    int gch = oi.gchance;
    if(!gch) continue;
    gch = int(gch / orbprizefun(treas));
    if(hrand(gch) >= 60) continue;
    if(oi.orb == itOrbWater && c->land != laOcean && c->land != laKraken) {
      if(cellHalfvine(c)) continue;
      c->item = oi.orb;
      c->wall = waStrandedBoat;
      return;
      }
    c->item = oi.orb;
    }
  
  // printf("land: %s orb: %s\n", dnameof(l), dnameof(c->item));
  }

// 10 not in chaos, less in chaos
EX int treasureForLocal() {
  return (ls::any_chaos() ? 1+hrand(10) : 10);
  }

EX bool extra_safety_for_memory(cell *c) {
  if(hyperbolic && (arcm::in() || S3 > 3) && !quotient && !tactic::on && in_full_game()) {
    if(hrand(1000) < 1) {
      c->item = itOrbSafety;
      return true;
      }
    }
  return false;
  }

EX void placeLocalOrbs(cell *c) {
  eLand l = c->land;
  if(l == laZebra && c->wall == waTrapdoor) return;
  if(isGravityLand(l) && l != laWestWall && cellEdgeUnstable(c)) return;
  if(isElemental(l)) l = laElementalWall;
  if(daily::on) return;

  if(extra_safety_for_memory(c)) return;

  if(peace::on) return;
  
  for(auto& oi: orbinfos) {
    if(!(oi.flags & orbgenflags::LOCAL10)) continue;
    if(oi.l != l) continue;
    if(yendor::on && (oi.orb == itOrbSafety || oi.orb == itOrbYendor))
      continue;
    if(!oi.lchance) continue;
    int ch = hrand(oi.lchance);
    if(ch == 1 && ls::any_chaos() && hrand(2) == 0 && items[treasureType(oi.l)] * landMultiplier(oi.l) >= (11+hrand(15)))
      ch = 0;
    int tc = items[treasureType(oi.l)] * landMultiplier(oi.l);
    int tcmin = treasureForLocal();
    if(inv::on) {
      if(!(oi.flags & orbgenflags::OSM_LOCAL25))
        tc = 0;
      else
        tcmin = 25;
      }
    if(ch == 0 && tc >= tcmin) {
      // printf("local orb\n");
      c->item = oi.orb;
      if(oi.orb == itOrbWater && c->land != laOcean) c->wall = waStrandedBoat;
      return;
      }
    else if(oi.gchance && (ch >= 1 && ch < 11) && getOLR(itShard, l) == olrPrize25 && l != laRedRock && l != laWhirlwind)
      buildPrizeMirror(c, 10);
    else if(oi.gchance && (ch >= 11 && ch < 11+PRIZEMUL)) 
      placePrizeOrb(c);
    }
  }

EX void placeLocalSpecial(cell *c, int outof, int loc IS(1), int priz IS(1)) {
  if(safety || daily::on || extra_safety_for_memory(c) || peace::on) return;
  int i = hrand(outof);
  if(i < loc && items[treasureType(c->land)] >= treasureForLocal() && !inv::on) 
    c->item = nativeOrbType(c->land);
  else if(i >= loc && i < loc + PRIZEMUL * priz)
    placePrizeOrb(c);
  }

EX void placeCrossroadOrbs(cell *c) {
  if(racing::on) return;
  if(peace::on) return;
  if(daily::on) return;
  for(auto& oi: orbinfos) {
    if(!(oi.flags & orbgenflags::CROSS10)) continue;
    if(!oi.gchance) continue;

    int treas = items[treasureType(oi.l)] * landMultiplier(oi.l);
    int mintreas = 10;
    
    if(inv::on) {
      if(oi.flags & orbgenflags::OSM_CROSS25)
        mintreas = 25;
      else if(oi.flags & orbgenflags::OSM_CROSS50)
        mintreas = 50;
      else continue;
      }
    
    if(ls::single() && isCrossroads(specialland)) {
      if(oi.flags & orbgenflags::NO_TACTIC)
        continue;
      else mintreas = 0;
      }
    if(treas < mintreas) continue;

    if(oi.orb == itOrbSafety && c->land == laCrossroads5) continue;
    int mul = c->land == laCrossroads5 ? 10 : 1;
    int gch = oi.gchance;
    if(!inv::on) gch /= orbcrossfun(treas); else gch /= 2;
    if(hrand(gch) >= mul) continue;
    if(hrand(50+items[itHyperstone]) >= 50) continue;
    c->item = oi.orb;
    if(oi.orb == itOrbWater && c->land != laOcean) c->wall = waStrandedBoat;
    }
  }

EX void placeOceanOrbs(cell *c) {
  if(peace::on) return;
  for(auto& oi: orbinfos) {
    if(!(oi.flags & orbgenflags::CROSS10)) continue;
    
    int treas = items[treasureType(oi.l)] * landMultiplier(oi.l);
    int mintreas = 10;

    if(inv::on) {
      if(oi.flags & orbgenflags::OSM_CROSS25)
        mintreas = 25;
      else if(oi.flags & orbgenflags::OSM_CROSS50)
        mintreas = 50;
      else continue;
      }

    if(treas < mintreas) continue;
    if(!oi.gchance) continue;
    if(oi.orb == itOrbLife) continue; // useless
    if(hrand(oi.gchance) >= 20) continue;
    c->item = oi.orb;
    }
  }

}
