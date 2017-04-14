// Hyperbolic Rogue

// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

// land generation routines

vector<cell*> buggycells;

cell *pathTowards(cell *pf, cell *pt) {

  while(celldist(pt) > celldist(pf)) {
    if(isNeighbor(pf, pt)) return pt;
    cell *pn = NULL;
    forCellEx(pn2, pt) if(celldist(pn2) < celldist(pt)) pn = pn2;
    pt = pn;
    }

  if(isNeighbor(pf, pt)) return pt;
  forCellEx(pn2, pt) if(celldist(pn2) < celldist(pt)) return pn2;
  return NULL;
  }

bool buildBarrierNowall(cell *c, eLand l2, bool force = false);

bool safety = false;

eLand firstland = laIce, euclidland = laIce;

bool timerghost = true;
eLand lastland;

int lastexplore;

bool randomPatternsMode = false;
int randompattern[landtypes];

bool chaosmode = false;
bool chaosUnlocked = false;
bool chaosAchieved = false;

// returns: 2 = treasure increaser, 1 = just appears, 0 = does not appear
int isNative(eLand l, eMonster m) {
  switch(l) {
    case laIce: 
      return (m == moWolf || m == moWolfMoved || m == moYeti) ? 2 : 0;

    case laJungle: 
      return (m == moIvyRoot || m == moMonkey) ? 2 : 
        (isIvy(m) || m == moEagle || m == moMonkey) ? 1 : 0;

    case laCaves: 
      return (m == moGoblin || m == moTroll) ? 2 : m == moSeep ? 1 : 0;
      
    case laDesert: 
      return (m == moDesertman || m == moWorm) ? 2 : 0;

    case laAlchemist: 
      return (m == moSlime) ? 2 : 0;

    case laMirror: 
      return (m == moEagle || m == moRanger || m == moMirror || m == moMirage) ? 1 : 0;
      
    case laMotion: 
      return (m == moRunDog) ? 2 : 0;

    case laGraveyard: 
      return (m == moZombie || m == moNecromancer) ? 2 : 
        m == moGhost ? 1 : 0;
      
    case laRlyeh: 
      return 
        (m == moCultist || m == moTentacle || m == moPyroCultist) ? 2 :
        (m == moCultistLeader || isIvy(m)) ? 1 : 0;
        
    case laDryForest: 
      return (m == moHedge || m == moFireFairy) ? 2 : 0;

    case laHell: 
      return (m == moLesser) ? 2 : 0;
      
    case laCocytus: 
      return (m == moShark || m == moGreaterShark || m == moCrystalSage) ? 2 :
        m == moYeti ? 1 : 0;

    case laCrossroads: case laCrossroads2: case laCrossroads3: case laCrossroads4: 
    case laCrossroads5:
    case laNone: case laBarrier: case laOceanWall: case laCanvas: return 0;
    
    case laEmerald: 
      return (m == moFlailer || m == moLancer || m == moMiner) ? 2 : 
        m == moHedge ? 1 : 0;
      
    case laWineyard: 
      return (m == moVineSpirit || m == moVineBeast) ? 2 : 0;
    
    case laHive: 
      return isBug(m) ? 1 : 0;

    case laDeadCaves: 
      return (m == moEarthElemental || m == moDarkTroll) ? 2 : 
        m == moGoblin ? 1 : 0;

    case laPower: 
      return (isWitch(m) || m == moEvilGolem) ? 1 : 0;

    case laCamelot: 
      return (m == moKnight || m == moHedge || m == moFlailer || m == moLancer) ? 1 : 0;
      
    case laTemple: 
      return (m == moTentacle || m == moCultist || m == moPyroCultist || m == moCultistLeader) ? 1 : 0;
    
    case laCaribbean: 
      return (m == moPirate || m == moParrot || m == moCShark) ? 1 : 0;

    case laRedRock: return (m == moRedTroll || m == moHexSnake) ? 2 : 0;
    
    case laMinefield: 
      return (m == moBomberbird || m == moTameBomberbird) ? 1 : 0;
      
    case laOcean: 
      return (m == moAlbatross) ? 2 : (m == moPirate || m == moCShark) ? 1 : 0;
      
    case laWhirlpool: 
      return (m == moPirate || m == moCShark) ? 1 : 0;
      
    case laPalace: case laPrincessQuest:
      return (m == moPalace || m == moFatGuard || m == moSkeleton || m == moVizier) ? 2 : 
        m == moSkeleton ? 1 : 0;

    case laLivefjord: 
      return m == moViking ? 2 : (m == moFjordTroll || m == moWaterElemental) ? 1 : 0;
    
    case laIvoryTower: 
      return (m == moFamiliar || m == moGargoyle) ? 2 : 0;
      
    case laZebra: return (m == moOrangeDog) ? 2 : 0;

    case laEAir: case laEEarth: case laEWater: case laEFire: 
    case laElementalWall: 
      if(m == elementalOf(l)) return 2;
      return (m == moAirElemental || m == moEarthElemental || m == moWaterElemental || m == moFireElemental) ? 1 : 0;
    
    case laStorms: 
      return (m == moMetalBeast || m == moMetalBeast2 || m == moStormTroll) ? 1 : 0;
    
    case laOvergrown: 
      return (m == moMutant || m == moForestTroll) ? 1 : 0;
      
    case laWildWest: 
      return (m == moOutlaw) ? 2 : 0;

    case laHalloween: 
      return 1;

    case laClearing: 
      return (m == moMutant || m == moRedFox) ? 1 : 0;
    
    case laHaunted: case laHauntedWall: case laHauntedBorder: 
      return (m == moGhost || m == moFriendlyGhost) ? 1 : 0;
      
    case laWhirlwind: 
      return (m == moAirElemental || m == moWindCrow) ? 2 : 0;
    
    case laRose: 
      return (m == moFalsePrincess || m == moRoseBeauty || m == moRoseLady) ? 2 : 0;
      
    case laWarpCoast: case laWarpSea: 
      return m == moRatling ? 2 : m == moRatlingAvenger ? 1 : 0;
    
    case laDragon: 
      return (isDragon(m) || m == moFireElemental) ? 1 : 0;
    
    case laEndorian: 
      return (m == moResearcher || m == moSparrowhawk) ? 2 : 0;
      
    case laTortoise: 
      return m == moTortoise ? 1 : 0;
    
    case laTrollheim: 
      return isTroll(m) ? 1 : 0;

    case laKraken: 
      return m == moKrakenH ? 2 : (m == moViking || m == moKrakenT) ? 1 : 0;

    case laBurial: 
      return m == moDraugr ? 1 : 0;
    
    case laDungeon: 
      return 
        m == moBat ? 2 :
        m == moSkeleton || m == moGhost ? 1 : 
        0;
      
    case laMountain:
      return
        m == moEagle || m == moMonkey || isIvy(m) || m == moFriendlyIvy ? 1 : 0;
      
    case laReptile:
      return m == moReptile ? 1 : 0;
    
    case laBull:
      return (m == moSleepBull || m == moRagingBull || m == moButterfly || m == moGadfly) ? 1 : 0;

    case laPrairie:
      return (m == moRagingBull || m == moHerdBull || m == moGadfly) ? 1 : 0;
    
    case laCA: return false;
    }
  return false;
  }

eItem treasureType(eLand l) {
  switch(l) {
    case laIce: return itDiamond;
    case laJungle: return itRuby;
    case laCaves: return itGold;
    case laDesert: return itSpice;

    case laAlchemist: return itElixir;
    case laMirror: return itShard;
    case laMotion: return itFeather;

    case laGraveyard: return itBone;
    case laRlyeh: return itStatue;
    case laDryForest: return itFernFlower;    

    case laHell: return itHell;
    case laCocytus: return itSapphire;
    case laCrossroads: return itHyperstone;
    case laCrossroads2: return itHyperstone;
    case laCrossroads3: return itHyperstone;
    case laCrossroads4: return itHyperstone;
    case laCrossroads5: return itHyperstone;
    
    case laNone: return itNone;
    case laBarrier: return itNone;
    case laOceanWall: return itNone;
    case laCanvas: return itNone;
    
    case laEmerald: return itEmerald;
    case laWineyard: return itWine;
    case laHive: return itRoyalJelly;
    case laDeadCaves: return itSilver;
    case laPower: return itPower;
    case laCamelot: return itHolyGrail;
    case laTemple: return itGrimoire;
    
    case laCaribbean: return itPirate;
    case laRedRock: return itRedGem;
    
    case laMinefield: return itBombEgg;
    case laOcean: return itCoast;
    case laWhirlpool: return itWhirlpool;
    case laPalace: return itPalace;
    case laLivefjord: return itFjord;
    
    case laIvoryTower: return itIvory;
    case laZebra: return itZebra;

    case laEAir: case laEEarth: case laEWater: case laEFire: 
    case laElementalWall: return itElemental;
    
    case laPrincessQuest: return itSavedPrincess;
    
    case laStorms: return itFulgurite;
    case laOvergrown: return itMutant;
    case laWildWest: return itBounty;
    case laHalloween: return itTreat;
    case laClearing: return itMutant2;
    case laHaunted: case laHauntedWall: case laHauntedBorder: return itLotus;
    case laWhirlwind: return itWindstone;
    
    case laRose: return itRose;
    case laWarpCoast: case laWarpSea: return itCoral;
    
    case laDragon: return itDragon;
    case laEndorian: return itApple;
    case laTortoise: return itBabyTortoise;
    
    case laTrollheim: return itTrollEgg;
    case laKraken: return itKraken;
    case laBurial: return itBarrow;
    
    case laDungeon: return itSlime;
    case laMountain: return itAmethyst;
    case laReptile: return itDodeca;
    
    case laBull: return itBull;
    case laPrairie: return itGreenGrass;
    
    case laCA: return itNone;
    }
  return itNone;
  }

eItem wanderingTreasure(cell *c) {
  eLand l = c->land;
  if(l == laEFire) return itFireShard;
  if(l == laEWater) return itWaterShard;
  if(l == laEAir) return itAirShard;
  if(l == laEEarth) return itEarthShard;
  if(l == laElementalWall) return itNone;
  if(l == laMirror && c->type != 6) return itNone;
  if(l == laEmerald) {
    forCellEx(c2, c) if(c2->wall == waCavewall) return itNone;
    }
  if(l == laMinefield && c->wall == waMineMine) return itNone;
  if(l == laBurial && hrand(2)) return itOrbSword;
  if(l == laKraken) return itOrbFish;
  return treasureType(l);
  }

#define ORBLINES 54

struct orbinfo {
  eLand l;
  int lchance;
  int gchance;
  eItem orb;
  };

orbinfo orbinfos[ORBLINES] = {
  {laGraveyard, 200, 200,itGreenStone}, // must be first so that it does not reduce 
  // chance of other orbs
  {laJungle, 1200, 1500,itOrbLightning},
  {laIce, 2000, 1500,itOrbFlash},
  {laCaves, 1800, 2000,itOrbLife},
  {laAlchemist, 800, 800,itOrbSpeed},
  {laDesert, 2500, 1500,itOrbShield},
  {laHell, 2000, 1000,itOrbYendor},
  {laRlyeh, 1500, 1500,itOrbTeleport},
  {laMotion, 2000, 700, itOrbSafety},    
  {laIce, 1500, 0, itOrbWinter},
  {laDragon, 2500, 0, itOrbWinter},
  {laDryForest, 2500, 0, itOrbWinter}, 
  {laCocytus, 1500, 1500, itOrbWinter}, 
  {laCaves, 1200, 0, itOrbDigging},
  {laDryForest, 500, 4500, itOrbThorns},
  {laDeadCaves, 1800, 0, itGreenStone},
  {laDeadCaves, 1800, 1500, itOrbDigging},
  {laEmerald, 1500, 3500, itOrbPsi},
  {laWineyard, 900, 1200, itOrbAether},
  {laHive, 800, 1200, itOrbInvis},
  {laPower, 0, 3000, itOrbFire},
  {laMinefield, 0, 3500, itOrbFriend},
  {laTemple, 0, 3000, itOrbDragon},
  {laCaribbean, 0, 3500, itOrbTime},
  {laRedRock, 0, 2500, itOrbSpace},
  {laCamelot, 1000, 1500, itOrbIllusion},
  {laOcean, 0, 3000, itOrbEmpathy},
  {laOcean, 0, 0, itOrbAir},
  {laPalace, 0, 4000, itOrbDiscord},
  {laZebra, 500, 2100, itOrbFrog},
  {laLivefjord, 0, 1800, itOrbFish},
  {laPrincessQuest, 0, 200, itOrbLove},
  {laIvoryTower, 500, 4000, itOrbMatter},
  {laElementalWall, 1500, 4000, itOrbSummon},
  {laStorms, 1000, 2500, itOrbStunning},
  {laOvergrown, 1000, 800, itOrbLuck},
  {laWhirlwind, 1250, 3000, itOrbAir},
  {laHaunted, 1000, 5000, itOrbUndeath},
  {laClearing, 5000, 5000, itOrbFreedom},
  {laRose, 2000, 8000, itOrbBeauty},
  {laWarpCoast, 2000, 8000, itOrb37},
  {laDragon, 500, 5000, itOrbDomination},
  {laTortoise, 2500, 1500, itOrbShell},
  {laEndorian, 150, 2500, itOrbEnergy},
  {laEndorian, 450, 0, itOrbTeleport},
  {laKraken, 500, 2500, itOrbSword},
  {laBurial, 500, 2500, itOrbSword2},
  {laTrollheim, 750, 1800, itOrbStone},
  {laMountain, 400, 3500, itOrbNature},
  {laDungeon, 120, 2500, itOrbRecall},
  {laReptile, 500, 2100, itOrbDash},
  {laBull, 720, 3000, itOrbHorns},
  {laPrairie, 0, 3500, itOrbBull},
  {laWhirlpool, 0, 2000, itOrbWater}, // must be last because it generates a boat
  };

bool isElemental(eLand l);

eItem orbType(eLand l) {
  if(isElemental(l)) l = laElementalWall;
  if(l == laMirror) return itShard;
  for(int i=0; i<ORBLINES; i++) 
    if(orbinfos[i].l == l && orbinfos[i].gchance)
      return orbinfos[i].orb;
  return itNone;
  }

enum eOrbLandRelation { 
  olrForbidden, // never appears: forbidden
  olrDangerous, // never appears: would be dangerous
  olrUseless,   // never appears: useless here
  olrNoPrizes,  // no prizes in this land
  olrNoPrizeOrb,// orb not allowed as a prize
  olrPrize25,   // prize for collecting 25
  olrPrize3,    // prize for collecting 3
  olrNative,    // native orb in this land
  olrNative1,    // native orb in this land (1)
  olrGuest,     // extra orb in this land
  olrPNative,   // Land of Power: native
  olrPBasic,    // Land of Power: basic orbs
  olrPPrized,   // Land of Power: prized orbs
  olrPNever,    // Land of Power: foreign orbs
  olrHub,       // hub lands
  olrMonster,   // available from a monster
  olrAlways     // always available
  };

string olrDescriptions[] = {
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
  "always available"
  };

eOrbLandRelation getOLR(eItem it, eLand l) {

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
  
  if(it == itOrbSafety && l == laCrossroads5) return olrDangerous;
  if(it == itOrbFire && l == laKraken) return olrUseless;
  if(it == itOrbDragon && l == laKraken) return olrUseless;
  if(it == itOrbDigging && l == laKraken) return olrUseless;
  if(it == itOrbIllusion && l == laKraken) return olrUseless;
  
  // if(it == itOrbYendor && l == laWhirlpool) return olrUseless;
  if(it == itOrbYendor && l == laWhirlwind) return olrUseless;
  
  if(it == itOrbLife && (l == laKraken)) return olrUseless;
  
  if(it == itOrbAir && l == laAlchemist) return olrUseless;
  // if(it == itOrbShield && l == laMotion) return olrUseless;

  if(it == itOrbIllusion && l == laCamelot) return olrNative1;
  if(it == itOrbLove) return olrNoPrizeOrb;    
  if(orbType(l) == it) return olrNative;
  if(it == itOrbWinter && (l == laIce || l == laDryForest || l == laDragon))
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
   if(it == itOrbLuck && l == laCamelot)
    return olrUseless;
   if(it == itOrbLuck && l == laHaunted)
    return olrUseless;
   if(it == itOrbNature && l == laWineyard)
    return olrDangerous;
  if(it == itOrbDigging && l == laCaves)
    return olrGuest;
  if(it == itOrbFrog && (l == laPalace || l == laPrincessQuest))
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
  
  if(l == laCocytus)
    if(it == itOrbDragon || it == itOrbFire || it == itOrbFlash || it == itOrbLightning)
      return olrDangerous;
  
  if(it == itOrbSafety)
    if(l == laCaves || l == laLivefjord || l == laRedRock || l == laCocytus || l == laHell ||
      l == laDesert || l == laAlchemist || l == laDeadCaves || l == laMinefield || isHaunted(l) ||
      l == laDragon) 
      return olrDangerous;
    
  if(it == itOrbMatter)
    if(l == laCaves || l == laEmerald || l == laAlchemist || l == laCaribbean || 
      l == laMinefield || l == laCocytus) return olrUseless;

  if(l == laPrincessQuest)
    if(it == itOrbAether || it == itOrbFlash || it == itOrbTeleport || it == itOrbSummon || it == itOrbFreedom)
      return olrForbidden;
    
  if(l == laTemple)
    return olrNoPrizes;
  
  if(it == itOrbDigging) {
    if(l == laCaves || l == laOcean || l == laLivefjord || l == laEmerald ||
      l == laDesert || l == laDeadCaves || l == laRedRock || l == laCaribbean || l == laGraveyard ||
      l == laMountain)
        return olrPrize25;
    return olrUseless;
    }
  
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
  
  if(it == itOrbWinter && l != laRlyeh && l != laTemple) 
    return olrUseless;
  
  if(it == itOrbLife && l == laMotion)
    return olrUseless;
  
  if(it == itOrbFish && l == laKraken)
    return olrAlways;
    
  if(it == itOrbSword && l == laBurial)
    return olrAlways;
    
  if(it == itOrbFish && l != laOcean && l != laLivefjord && l != laWhirlpool && l != laCamelot &&
    l != laTortoise)
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
    if(it == itOrbDragon || it == itOrbFire || it == itOrbLightning)
      return olrDangerous;
    
  if(l == laDungeon) {
    if(it == itOrbSafety || it == itOrbFrog || 
      it == itOrbTeleport || it == itOrbMatter || it == itOrbNature ||
      it == itOrbAether || it == itOrbSummon || it == itOrbStone) 
      return olrForbidden;
    }
  
  return olrPrize25;
  }

int landMultiplier(eLand l) {
  if(l == laCamelot || l == laPrincessQuest) return 10;
  return 1;
  }

// 2 = always available, 1 = highscore required, 0 = never available
int isRandland(eLand l) {
  if(l == laIce || l == laDesert || l == laCaves || l == laWildWest)
    return 2;
  for(int i=0; i<RANDLANDS; i++) if(randlands[i] == l) return 1;
  return 0;
  }

bool landUnlocked(eLand l) {
  if(randomPatternsMode) {
    int i = isRandland(l);
    if(i == 2) return true;
    if(i == 1) return hiitemsMax(treasureType(l)) >= 10;
    return false;
    }
  
  switch(l) {
    case laOvergrown: 
      return gold() >= 60 && items[itRuby] >= 10;
    
    case laStorms: case laWhirlwind: 
      return gold() >= 60;
    
    case laWildWest: case laHalloween:
      return false;
      
    case laIce: case laJungle: case laCaves: case laDesert: 
    case laMotion: case laCrossroads:  case laAlchemist:
      return true;

    case laMirror: case laMinefield: case laPalace:
    case laOcean: case laLivefjord:
      return gold() >= 30;
    
    case laCaribbean: case laWhirlpool:
      return exploreland[0][laOcean] || items[itCoast] || items[itStatue];
    
    case laRlyeh: case laDryForest: case laWineyard: case laCrossroads2:
      return gold() >= 60;
    
    case laDeadCaves:
      return gold() >= 60 && items[itGold] >= 10;

    case laGraveyard:
      return tkills() >= 100;

    case laHive:
      return tkills() >= 100 && gold() >= 60;

    case laRedRock:
      return gold() >= 60 && items[itSpice] >= 10;
    
    case laEmerald:
      return (items[itFernFlower] >= 5 && items[itGold] >= 5) || kills[moVizier];
    
    case laCamelot:
      return items[itEmerald] >= 5;
    
    case laHell: case laCrossroads3:
      return hellUnlocked();
      
    case laPower: 
      return items[itHell] >= 10;

    case laCocytus: 
      return items[itHell] >= 10 && items[itDiamond] >= 10;

    case laTemple:
      return items[itStatue] >= 5;

    case laClearing:
      return items[itMutant] >= 5;

    case laIvoryTower: return gold() >= 30;
    case laZebra: return gold() >= 30 && items[itFeather] >= 10;

    case laEAir: case laEEarth: case laEWater: case laEFire:  case laElementalWall:
      return elementalUnlocked();
    
    case laBarrier: case laNone: case laOceanWall: case laCanvas: case laCA:
      return false;
    
    case laHaunted: case laHauntedWall: case laHauntedBorder: 
      return items[itBone] >= 10;

    case laPrincessQuest: return kills[moVizier] && !shmup::on && multi::players == 1;
    
    case laRose: 
      return gold() >= 60;
      
    case laWarpCoast: case laWarpSea: 
      return gold() >= 30;
      
    case laCrossroads4:
      return gold() >= 200;
    
    case laEndorian:
      return items[itIvory] >= 10;
    
    case laTortoise:
      return tortoise::seek();
    
    case laDragon:
      return killtypes() >= 20;
    
    case laKraken:
      return items[itFjord] >= 10;

    case laBurial:
      return items[itKraken] >= 10;

    case laTrollheim:
      return trollUnlocked();
    
    case laDungeon:
      return items[itPalace] >= 5 && items[itIvory] >= 5;

    case laMountain:
      return items[itRuby] >= 5 && items[itIvory] >= 5;
      
    case laReptile:
      return gold() >= 30 && items[itElixir] >= 10;
    
    case laPrairie:
    case laBull:
      return gold() >= 90;
    
    case laCrossroads5:
      return gold() >= 300;
    }
  return false;
  }

int orbsUnlocked() {
  int i = 0;
  for(int t=0; t<ittypes; t++) 
    if(itemclass(eItem(t)) == IC_TREASURE && items[t] >= 10)
      i++;
  return i;
  }

bool hellUnlocked() {
  return orbsUnlocked() >= 9;
  }

void countHyperstoneQuest(int& i1, int& i2) {
  i1 = 0; i2 = 0;
  for(int t=1; t<ittypes; t++) 
    if(t != itHyperstone && t != itBounty && t != itTreat &&
    itemclass(eItem(t)) == IC_TREASURE) {
      i2++; if(items[t] >= 10) i1++;
      }
  }  

bool hyperstonesUnlocked() {
  int i1, i2;
  if(tactic::on && isCrossroads(tactic::lasttactic) && !tactic::trailer) return true;
  countHyperstoneQuest(i1, i2);
  return i1 == i2;
  }

// reduce c->mpdist to d; also generate the landscape

bool buggyGeneration = false;

bool checkBarriersBack(cellwalker bb, int q=5, bool cross = false);

bool checkBarriersFront(cellwalker bb, int q=5, bool cross = false) {

  if(bb.c->type == 6) 
    return false;

  if(bb.c->mpdist < BARLEV) return false;
  if(bb.c->mpdist == BUGLEV) return false;
  if(bb.c->bardir != NODIR) return false;
  if(bb.spin == (purehepta ? 3 : 0)) {q--; if(!q) return true; }

  if(!cross) for(int i=0; i<7; i++) {
    cellwalker bb2 = bb;
    cwspin(bb2, i); cwstep(bb2); 
    if(bb2.c->bardir != NODIR) return false; 
    if(!purehepta) { 
      cwspin(bb2, 4); cwstep(bb2);
      if(bb2.c->bardir != NODIR) return false;
      }
    }

  cwstep(bb); 
  if(!purehepta) { cwspin(bb, 3); cwstep(bb); cwspin(bb, 3); cwstep(bb); }
  return checkBarriersBack(bb, q);
  }

bool hasbardir(cell *c) {
  return c->bardir != NODIR && c->bardir != NOBARRIERS;
  }

void preventbarriers(cell *c) {
  if(c && c->bardir == NODIR) c->bardir = NOBARRIERS;
  }

bool checkBarriersBack(cellwalker bb, int q, bool cross) {
  // printf("back, %p, s%d\n", bb.c, bb.spin);

  // if(mark) { printf("mpdist = %d [%d] bardir = %d spin=%d q=%d cross=%d\n", bb.c->mpdist, BARLEV, bb.c->bardir, bb.spin, q, cross); }
  
  if(bb.c->mpdist < BARLEV) return false;
  if(bb.c->mpdist == BUGLEV) return false;
  if(bb.c->bardir != NODIR) return false;

  // if(bb.spin == 0 && bb.c->mpdist == INFD) return true;
  
  if(!cross) for(int i=0; i<7; i++) {
    cellwalker bb2 = bb;
    cwspin(bb2, i); cwstep(bb2); 
    if(bb2.c->bardir != NODIR) return false;
    if(!purehepta) {
      cwspin(bb2, 4); cwstep(bb2);
      if(bb2.c->bardir != NODIR) return false;
      }
    }

  cwspin(bb, 3); cwstep(bb); cwspin(bb, purehepta ? 5 : 4); 
  // bool create = cwstepcreates(bb);
  cwstep(bb); cwspin(bb, 3); 
  // if(create && bb.spin == 0) return true;
  return checkBarriersFront(bb, q);
  }

const eLand NOWALLSEP = laNone;
const eLand NOWALLSEP_USED = laWhirlpool;

void setland(cell *c, eLand l);

bool checkBarriersNowall(cellwalker bb, int q, int dir, eLand l1=laNone, eLand l2=laNone) {
  if(bb.c->mpdist < BARLEV && l1 == laNone) return false;
  if(bb.c->bardir != NODIR && l1 == laNone) return false;
  
  if(l1 != laNone) { 
    bb.c->bardir = bb.spin; bb.c->barright = l2; bb.c->barleft = NOWALLSEP; 
    setland(bb.c, l1);
    }
  if(q > 10) return true;
  
  if(l1 == laNone) for(int i=0; i<bb.c->type; i++) {
    cell *c1 = bb.c->mov[i];
    if(!c1) continue;
    for(int j=0; j<c1->type; j++) {
      cell *c2 = c1->mov[j];
      if(!c2) continue;
      if(c2 && c2->bardir == NOBARRIERS) 
        return false;
      if(c2 && c2->bardir != NODIR && c2->barleft != NOWALLSEP) 
        return false;
      // note: "far crashes" between NOWALL lines are allowed
      }
    }
  
  if(purehepta) {
    cwspin(bb, 3*dir);
    cwstep(bb);
    cwspin(bb, -3*dir);
    }
  else {
    cwstep(bb);
    cwspin(bb, 2*dir);
    cwstep(bb);
    cwspin(bb, dir);
    }
  return checkBarriersNowall(bb, q+1, -dir, l2, l1);
  }

bool isSealand(eLand l) {
  return l == laOcean || l == laCaribbean || l == laWhirlpool || l == laLivefjord ||
    l == laOceanWall || l == laWarpSea || l == laKraken;
  }

bool isCoastal(eLand l) {
  return l == laWarpSea || l == laWarpCoast || l == laLivefjord || l == laOcean;
  }

bool isPureSealand(eLand l) {
  return l == laCaribbean || l == laKraken;
  }

bool isElemental(eLand l) {
  return l == laEAir || l == laEWater || l == laEEarth || l == laEFire ||
    l == laElementalWall;
  }

bool isHaunted(eLand l) {
  return l == laHaunted || l == laHauntedBorder || l == laHauntedWall;
  }

eWall getElementalWall(eLand l) {
  if(l == laEAir) return waChasm;
  if(l == laEEarth) return waStone;
  if(l == laEFire) return waEternalFire;
  if(l == laEWater) return waSea;
  return waNone;
  }

bool isTrollLand(eLand l) {
  return l == laCaves || l == laStorms || l == laOvergrown ||
    l == laDeadCaves || l == laLivefjord || l == laRedRock;
  }

void setbarrier(cell *c) {
  if(isSealand(c->barleft) && isSealand(c->barright)) {
    bool setbar = c->type == 7;
    if(c->barleft == laKraken || c->barright == laKraken)
    if(c->barleft != laWarpSea && c->barright != laWarpSea)
      setbar = !setbar;
    c->wall = setbar ? waBarrier : waSea;
    c->land = laOceanWall;
    }
  else if(isElemental(c->barleft) && isElemental(c->barright)) {
    c->land = laElementalWall;
    c->wall = getElementalWall(c->barleft);
    }
  else if(c->barleft == laHaunted || c->barright == laHaunted) {
    c->land = laHauntedWall;
    }
  else {
    c->wall = waBarrier;
    c->land = laBarrier;
    }
/*if(isHive(c->barleft) && isHive(c->barright))
    c->wall = waWaxWall, c->land = c->barleft; */
  }

int buildIvy(cell *c, int children, int minleaf) {
  if(c->monst) return 0;
  c->mondir = NODIR;
  c->monst = moIvyRoot;
  
  cell *child = NULL;

  int leaf = 0;
  int leafchild = 0;
  for(int i=0; i<c->type; i++) {
    createMov(c, i);
    if(passable(c->mov[i], c, 0) && c->mov[i]->land == c->land) {
      if(children && !child) 
        child = c->mov[i], leafchild = buildIvy(c->mov[i], children-1, 5);
      else 
        c->mov[i]->monst = (leaf++) ? moIvyWait : moIvyHead,
        c->mov[i]->mondir = c->spn(i);
      }
    }
  
  leaf += leafchild;
  if(leaf < minleaf) {
    if(child) killIvy(child, moNone);
    killIvy(c, moNone);
    return 0;
    }
  else return leaf;
  }

bool grailWasFound(cell *c) {
  if(euclid) return items[itHolyGrail];
  return c->master->alt->alt->emeraldval & GRAIL_FOUND;
  }

int euclidAlt(short x, short y) {
  if(euclidland == laTemple || euclidland == laClearing) {
    return max(int(x), x+y);
    }
  else if(euclidland == laCaribbean || euclidland == laWhirlpool || euclidland == laMountain) {
    return 
      min(
        min(max(int(-x), -x-y) + 3,
        max(int(x+y), int(y)) + 3),
        max(int(x), int(-y)) + 3
        );
    }
  else if(euclidland == laPrincessQuest)
    return eudist(x-EPX, y-EPY);
  else return eudist(x-20, y-10);
  }

bool isCrossroads(eLand l) {
  return l == laCrossroads || l == laCrossroads2 || l == laCrossroads3 ||
    l == laCrossroads4 || l == laCrossroads5;
  }

bool bearsCamelot(eLand l) {
  return isCrossroads(l) && l != laCrossroads2 && l != laCrossroads5;
  }

ld orbprizefun(int tr) {
  if(tr < 10) return 0;
  return .6 + .4 * log(tr/25.) / log(2);
  }

ld orbcrossfun(int tr) {
  if(tr < 10) return 0;
  if(tr > 25) return 1;
  return (tr*2 + 50) / 100.;
  }

bool buildPrizeMirror(cell *c, int freq) {
  if(c->type == 7 && !purehepta) return false;
  if(items[itShard] < 25) return false;
  if(freq && hrand(freq * 100 / orbprizefun(items[itShard])) >= 100)
    return false;
  c->wall = hrand(2) ? waCloud : waMirror;
  return true;
  }                    

void placePrizeOrb(cell *c) {
  eLand l = c->land;
  if(isElemental(l)) l = laElementalWall;

  // these two lands would have too much orbs according to normal rules
  if(l == laPalace && hrand(100) >= 20) return;
  if(l == laGraveyard && hrand(100) >= 15) return;
  if(l == laBurial && hrand(100) >= 10) return;
  if(l == laLivefjord && hrand(100) >= 35) return;
  if(l == laMinefield && hrand(100) >= 25) return;
  if(l == laElementalWall && hrand(100) >= 25) return;

  if(l == laPalace && princess::dist(c) < OUT_OF_PRISON)
    l = laPrincessQuest;
  for(int i=0; i<ORBLINES; i++) {
    orbinfo& oi(orbinfos[i]);
    eOrbLandRelation olr = getOLR(oi.orb, l);
    if(olr != olrPrize25 && olr != olrPrize3) continue;
    int treas = items[treasureType(oi.l)];
    if(olr == olrPrize3) treas *= 10;
    if(olr == olrPrize25 || olr == olrPrize3 || olr == olrGuest || olr == olrMonster || olr == olrAlways) {
      if(treas < 25) continue;
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

void placeLocalOrbs(cell *c) {
  eLand l = c->land;
  if(l == laZebra && c->wall == waTrapdoor) return;
  if(isGravityLand(l) && cellEdgeUnstable(c)) return;
  if(isElemental(l)) l = laElementalWall;
  
  for(int i=0; i<ORBLINES; i++) {
    orbinfo& oi(orbinfos[i]);
    if(oi.l != l) continue;
    if(yendor::on && (oi.orb == itOrbSafety || oi.orb == itOrbYendor))
      continue;
    if(!oi.lchance) continue;
    int ch = hrand(oi.lchance);
    if(tactic::trailer && ch < 5) ch = 0;
    if(ch == 0 && items[treasureType(oi.l)] * landMultiplier(oi.l) >= 10) {
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

void placeCrossroadOrbs(cell *c) {  
  for(int i=0; i<ORBLINES; i++) {
    orbinfo& oi(orbinfos[i]);
    if(!oi.gchance) continue;
    int treas = items[treasureType(oi.l)] * landMultiplier(oi.l);
    if(tactic::on && isCrossroads(tactic::lasttactic)) {
      if(oi.orb == itOrbYendor || oi.orb == itOrbSummon || oi.orb == itOrbFish || oi.orb == itOrbDigging || oi.orb == itOrbLove || oi.orb == itOrbLuck)
        continue;
      }
    else {
      if(treas < 10) continue;
      }
    if(oi.orb == itOrbSafety && c->land == laCrossroads5) continue;
    int mul = c->land == laCrossroads5 ? 10 : 1;
    int gch = oi.gchance;
    gch /= orbcrossfun(treas);
    if(hrand(gch) >= mul) continue;
    if(hrand(50+items[itHyperstone]) >= 50) continue;
    c->item = oi.orb;
    if(oi.orb == itOrbWater && c->land != laOcean) c->wall = waStrandedBoat;
    }
  }

void placeOceanOrbs(cell *c) {  
  for(int i=0; i<ORBLINES; i++) {
    orbinfo& oi(orbinfos[i]);
    if(items[treasureType(oi.l)] * landMultiplier(oi.l) < 10) continue;
    if(!oi.gchance) continue;
    if(oi.orb == itOrbLife) continue; // useless
    if(hrand(oi.gchance) >= 20) continue;
    c->item = oi.orb;
    }
  }

bool errorReported = false;

void describeCell(cell *c) {
  if(!c) { printf("NULL\n"); return; }
  printf("describe %p: ", c);
  printf("%-15s", linf[c->land].name);
  printf("%-15s", winf[c->wall].name);
  printf("%-15s", iinf[c->item].name);
  printf("%-15s", minf[c->monst].name);
  printf("LP%08x", c->landparam);
  printf("D%3d", c->mpdist);
  printf("MON%3d", c->mondir);
  printf("\n");
  }

#ifdef BACKTRACE
#include <execinfo.h>
#endif

void raiseBuggyGeneration(cell *c, const char *s) {

  printf("procgen error (%p): %s\n", c, s);
  
  if(!errorReported) {
    addMessage(string("something strange happened in: ") + s);
    errorReported = true;
    }
  // return;

#ifdef LOCAL
  
  describeCell(c);
  for(int i=0; i<c->type; i++) describeCell(c->mov[i]);

  buggyGeneration = true; buggycells.push_back(c);

#else
  c->item = itBuggy;
#endif

#ifdef BACKTRACE
  void *array[1000];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 1000);

  // print out all the frames to stderr
  backtrace_symbols_fd(array, size, STDERR_FILENO);
#endif
  }

void setland(cell *c, eLand l) {
  if(c->land != l)  {
    c->landparam = 0;  
    }
  if(l == laNone) {
    printf("setland\n"); // NONEDEBUG
    }
  c->land = l;
  }

void extendcheck(cell *c) {
  return;
  if(!purehepta && c->landparam == 0 && c->barleft != NOWALLSEP) {
    raiseBuggyGeneration(c, "extend error");
    }
  }
  
void extendBarrierFront(cell *c) {
  int ht = c->landparam;
  extendcheck(c);

  cellwalker bb(c, c->bardir); setbarrier(bb.c);
  cwstep(bb); 
  
  if(!purehepta) {
    bb.c->barleft = c->barleft;
    bb.c->barright = c->barright;
    setbarrier(bb.c);
    bb.c->landparam = (ht-4);
  //printf("[A heat %d]\n", ht-4);

    cwspin(bb, 2); cwstep(bb); setland(bb.c, c->barleft); cwstep(bb);
    cwspin(bb, 2); cwstep(bb); setland(bb.c, c->barright); cwstep(bb);
    cwspin(bb, 2); 
    
    cwspin(bb, 3); cwstep(bb); 
    bb.c->barleft = c->barright;
    bb.c->barright = c->barleft;
    setbarrier(bb.c);
    bb.c->landparam = (ht-4)^2;
  //printf("[B heat %d]\n", (ht-4)^2);
    cwspin(bb, 3); cwstep(bb);
    
    bb.c->landparam = ht ^ 2;
    }

//printf("[C heat %d]\n", (ht)^2);
  bb.c->bardir = bb.spin;
  bb.c->barleft = c->barright;
  bb.c->barright = c->barleft;
  // printf("#1\n");
  extendcheck(bb.c);
  extendBarrier(bb.c);
  
  for(int a=-3; a<=3; a++) if(a) {
    bb.c = c; bb.spin = c->bardir; cwspin(bb, purehepta?-a:a); cwstep(bb); 
    setland(bb.c, a > 0 ? c->barright : c->barleft);
    }
  }

void extendBarrierBack(cell *c) {
  int ht = c->landparam;
  extendcheck(c);

  cellwalker bb(c, c->bardir); setbarrier(bb.c);
  cwspin(bb, 3); cwstep(bb); cwspin(bb, purehepta?5:4); 
  setland(bb.c, purehepta ? c->barleft : c->barright); 
  cwstep(bb); cwspin(bb, 3);
  bb.c->bardir = bb.spin;
  bb.c->barleft = c->barright;
  bb.c->barright = c->barleft;
  bb.c->landparam = ht ^ 11;
  extendcheck(bb.c);
//printf("[D heat %d]\n", (ht^11));

  // needed for CR2 to work
  if(!purehepta) { 
    cwstep(bb); 
    bb.c->barleft = c->barright;
    bb.c->barright = c->barleft;
    bb.c->landparam = (ht^11)-4;
    cwstep(bb);
    }
//printf("[E heat %d]\n", (ht^11));

  // printf("#2\n");
  extendBarrier(bb.c);
  }

eLand oppositeElement(eLand l) {
  if(l == laEFire) return laEWater;
  if(l == laEWater) return laEFire;
  if(l == laEAir) return laEEarth;
  if(l == laEEarth) return laEAir;
  return l;
  }

void extendNowall(cell *c) {

  c->barleft = NOWALLSEP_USED;
  cellwalker cw(c, c->bardir);
  
  if(!purehepta) {
    cwstep(cw);
    setland(cw.c, c->barright);
    }
  
  for(int i=-1; i<2; i+=2) {  
    if(purehepta) {
      cwspin(cw, 3*i);
      cwstep(cw);
      cwspin(cw, -3*i);
      }
    else {
      cwspin(cw, 2*i);
      cwstep(cw);
      }
    setland(cw.c, c->barright);
    if(cw.c->barleft != NOWALLSEP_USED) {
      cw.c->barleft = NOWALLSEP;
      cw.c->barright = c->land;
      if(c->barright == laNone) { 
        printf("barright\n"); 
        }// NONEDEBUG
      setland(cw.c, c->barright);
      if(!purehepta) cwspin(cw, i);
      cw.c->bardir = cw.spin;
      if(!purehepta) cwspin(cw, -i);
      extendcheck(cw.c);
      extendBarrier(cw.c);
      }
    if(purehepta) {
      cwspin(cw, 3*i);
      cwstep(cw);
      cwspin(cw, -3*i);
      }
    else {
      cwstep(cw);
      cwspin(cw, -2*i);
      }
    }
  }

eLand getNewLand(eLand old);

bool gotit = false;

void extendCR5(cell *c) {
  if(purehepta) return;
// if(c->barright == laCrossroads5) extendCR5(c);
  eLand forbidden = c->barleft;
  eLand forbidden2 = laNone;
  cellwalker cw(c, c->bardir);
  for(int u=0; u<2; u++) {
    // if(gotit) break;
    cwspin(cw, 2);
    cwstep(cw);
    cwspin(cw, 2);
    cwstep(cw);
    cwspin(cw, 5);
    if(cw.c->bardir == NODIR) {
      cw.c->landparam = 40;
      cw.c->bardir = cw.spin;
      cw.c->barright = laCrossroads5;
      eLand nland = forbidden;
      for(int i=0; i<10 && (nland == forbidden || nland == forbidden2); i++)
        nland = getNewLand(laCrossroads5);
      cw.c->barleft = forbidden2 = nland;
      landcount[nland]++;
      extendBarrier(cw.c);
      gotit = true;
      }
    else forbidden2 = cw.c->barleft;
    }
  }

void extendBarrier(cell *c) {
  if(buggyGeneration) return;
  
  if(c->barleft == NOWALLSEP_USED) return;

  extendcheck(c);
  
  // printf("build barrier at %p", c);
  if(c->land == laBarrier || c->land == laElementalWall || c->land == laHauntedWall || c->land == laOceanWall) { 
    // printf("-> ready\n");
    return;
    }
//  if(c->wall == waWaxWall) return;
  if(c->mpdist > BARLEV) {
    // printf("-> too far\n");
    return; // == INFD) return;
    }

  if(c->barleft == NOWALLSEP) {
    extendNowall(c);
    return;
    }
  
  if(((c->barleft == laCrossroads3 || c->barright == laCrossroads3) && hrand(100) < 66) ||
    (isElemental(c->barleft) && isElemental(c->barright) && hrand(100) < 25)) {
    
    cellwalker cw(c, c->bardir);
    if(purehepta) {
      cwstep(cw); if(cw.c->wall == waBarrier || cw.c->land == laElementalWall) {
        cwstep(cw); cwspin(cw, 3); cwstep(cw); cwspin(cw, -1); cwstep(cw);
        bool b = buildBarrier4(cw.c, cw.spin, 2, oppositeElement(c->barleft), c->barright);
        if(b) return;
        }
      else {
        bool b = buildBarrier4(c, c->bardir, 1, c->barleft, c->barright);
        if(b) return;
        }
      }
    else {
      cwspin(cw, 3); cwstep(cw); 
      cell *cp = cwpeek(cw, 4);
      if(cp->wall != waBarrier && cp->land != laElementalWall) {
        cwspin(cw, 2);  cwstep(cw);
        bool b = buildBarrier4(cw.c, cw.spin, 2, oppositeElement(c->barleft), c->barright);
        if(b) return;
        }
      else {
        bool b = buildBarrier4(c, c->bardir, 1, c->barleft, c->barright);
        if(b) return;
        }
      }
    }
  
  extendBarrierFront(c);  
  extendBarrierBack(c);
  
  if(c->barright == laCrossroads5) extendCR5(c);
  }

void chasmify(cell *c) {
  c->wall = waChasm; c->item = itNone;
  int q = 0;
  cell *c2[10];
  for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->mpdist > c->mpdist && cellUnstable(c->mov[i]))
    c2[q++] = c->mov[i];
  if(q) {
    cell *c3 = c2[hrand(q)];
    c3->wall = waChasmD;
    }
  }

void chasmifyEarth(cell *c) {
  int q = 0;
  int d2[10];
  for(int i=2; i<=c->type-2; i++) {
    int j = (i+c->mondir)%c->type;
    cell *c2 = c->mov[j];
    if(c2 && c2->mpdist > c->mpdist && (
      c2->wall == waDeadfloor || c2->wall == waDeadwall ||
      c2->wall == waDeadfloor2)) 
      d2[q++] = j;
    }
  if(!q) printf("no further move!\n");
  if(q) {
    int d = d2[hrand(q)];
    cell *c3 = c->mov[d];
    c3->wall = waEarthD;
    for(int i=0; i<c3->type; i++) {
      cell *c4 = createMov(c3, i);
      earthFloor(c4);
      }
    c3->mondir = c->spn(d);
    }
  earthWall(c); c->item = itNone;
  }

void chasmifyElemental(cell *c) {
  int q = 0;
  int d2[10];
  for(int i=2; i<=c->type-2; i++) {
    int j = (i+c->mondir)%c->type;
    cell *c2 = c->mov[j];
    if(c2 && c2->mpdist > c->mpdist && c2->land == c->land)
      d2[q++] = j;
    }
  if(q) {
    int d = d2[hrand(q)];
    cell *c3 = c->mov[d];
    if(!c3->monst) {
      c3->wall = waElementalD;
      for(int i=0; i<c3->type; i++) {
        cell *c4 = createMov(c3, i);
        if(c4->wall != waBarrier) c4->wall = waNone;
        }
      c3->mondir = c->spn(d);
      }
    }
  c->wall = getElementalWall(c->land);
  c->wparam = 100; c->item = itNone;
  }

bool incompatible1(eLand l1, eLand l2) {
  if(isCrossroads(l1) && isCrossroads(l2)) return true;
  if(l1 == laJungle && l2 == laMotion) return true;
  if(l1 == laMirror && l2 == laMotion) return true;
  if(l1 == laPower && l2 == laWineyard) return true;
  if(l1 == laPower && l2 == laDryForest) return true;
  if(l1 == laDragon && l2 == laDryForest) return true;
  if(l1 == laEFire && l2 == laWineyard) return true;
  if(l1 == laEFire && l2 == laDryForest) return true;
  if(l1 == laGraveyard && l2 == laDryForest) return true;
  if(l1 == laGraveyard && l2 == laRedRock) return true;
  if(l1 == laGraveyard && l2 == laEmerald) return true;
  if(l1 == laDeadCaves && l2 == laEmerald) return true;
  if(l1 == laDeadCaves && l2 == laCaves) return true;
  if(l1 == laWarpSea && l2 == laKraken) return true;
  if(isElemental(l1) && isElemental(l2)) return true;
  return false;
  }

eLand randomElementalLand() {
  int i = hrand(4);
  eLand t[4] = { laEFire, laEWater, laEAir, laEEarth };
  return t[i];
  }

int elementalKills() {
  return
    kills[moAirElemental] + kills[moWaterElemental] + kills[moEarthElemental] + kills[moFireElemental];
  }

bool elementalUnlocked() {
  return
    kills[moAirElemental] && kills[moWaterElemental] && kills[moEarthElemental] && kills[moFireElemental];
  }

bool trollUnlocked() {
  return
    kills[moTroll] && kills[moDarkTroll] && kills[moRedTroll] && 
    kills[moStormTroll] && kills[moForestTroll] && kills[moFjordTroll];
  }

eLand randomElementalLandWeighted() {
  int i = hrand(elementalKills());
  i -= kills[moAirElemental]; if(i<0) return laEAir;
  i -= kills[moWaterElemental]; if(i<0) return laEWater;
  i -= kills[moEarthElemental]; if(i<0) return laEEarth;
  i -= kills[moFireElemental]; if(i<0) return laEFire;
  printf("elemental bug\n");
  return laElementalWall;
  }

bool incompatible(eLand nw, eLand old) {
  return (nw == old) || incompatible1(nw, old) || incompatible1(old, nw);
  }

#define HAUNTED_RADIUS (purehepta?5:7)
extern bool generatingEquidistant;

bool rlyehComplete() {
  if(chaosmode) return items[itStatue] >= 1;
  return items[itStatue] >= 10 || items[itGrimoire] >= 10;
  }

bool lchance(eLand l) { 
  if(tactic::on || yendor::on) return true;
  if(chaosmode) return hrand(100) < 25;
  return hrand(100) >= 40 * kills[elementalOf(l)] / (elementalKills()+1); 
  }

eLand pickLandRPM(eLand old) {
  while(true) {
    eLand n = randlands[hrand(RANDLANDS)];
    if(incompatible(n, old)) continue;
    if(isRandland(n) == 2) return n;
    if(hiitemsMax(treasureType(n)) < 10)
      continue;
    return n;
    }
  }

eLand pickluck(eLand l1, eLand l2) {
  int t1 = items[treasureType(l1)];
  int t2 = items[treasureType(l2)];
  if(t1 < t2) return l1;
  if(t2 < t1) return l2;
  if(isCrossroads(l1)) return l1;
  return l2;
  }

#define LIKELY for(int u=0; u<5; u++)

template<class T> T pick(T x, T y) { return hrand(2) ? x : y; }
template<class T> T pick(T x, T y, T z) { switch(hrand(3)) { case 0: return x; case 1: return y; case 2: return z; } return x; }
template<class T> T pick(T x, T y, T z, T v) { switch(hrand(4)) { case 0: return x; case 1: return y; case 2: return z; case 3: return v; } return x; }

bool noChaos(eLand l) {
  if(l == laOcean || l == laTemple) return false;
  return 
    isCrossroads(l) || isCyclic(l) || isHaunted(l) || 
    l == laCaribbean || isGravityLand(l) || l == laPrincessQuest ||
    l == laPrairie;
  }

eLand getNewSealand(eLand old) {
  while(true) {
    eLand p = pick(laOcean, pick(laCaribbean, laLivefjord, laWarpSea, laKraken));
    if(p == laKraken && !landUnlocked(p)) continue;
    if(incompatible(old, p)) continue;
    if(p == old) continue;
    if(chaosmode && noChaos(p)) continue;
    return p;
    }
  }

bool doEndorian = false;

int whichnow=0;

eLand getNewLand(eLand old) {

  /* eLand landtab[10] = {
    laWhirlwind, laRose, laEndorian, laRlyeh,
    laPalace, laOcean, laEmerald, laStorms,
    laGraveyard, laAlchemist 
    }; */
  
  // return landtab[items[itStrongWind]++ % 10];
  // if(old != laPrairie) return laRiver;
  
  #ifdef TOUR
  if(tour::on) {
    eLand l = tour::getNext(old);
    if(l) return l;
    }
  #endif
  
#ifdef LOCAL
  extern bool doAutoplay;
  if(doAutoplay) 
    return pick(laOcean, laLivefjord, laWarpSea, laWarpCoast);
  extern bool doCross;
  if(doCross) {
    whichnow++;
    eLand tabb[30] = {
      laIce, laRedRock, laCaribbean, laWarpCoast, laWhirlwind, laPower,
      laMirror, laPalace, laLivefjord, laAlchemist, laCocytus, 
      laHell, laJungle, laCaves, laDesert, laRlyeh, laStorms,
      laGraveyard, laMotion, laDryForest, laDragon, laZebra, laIvoryTower,
      laTrollheim, laOvergrown, laBurial, laRose, laHive, laEmerald,
      laEmerald
      };
    return tabb[whichnow%30];
    }
#endif

  if(cheatdest != old) if(!isCyclic(cheatdest) && !isTechnicalLand(cheatdest)) return cheatdest;
  
  if(old == laTortoise) return laDragon;

  if(yendor::on && chaosmode) {
    while(true) {
      eLand n = eLand(hrand(landtypes));
      if(n == old) continue;
      if(incompatible(n,old)) continue;
      if(noChaos(n)) continue;
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

  if(tactic::on && !(tactic::trailer && old == firstland)) return firstland;

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
  
  if(isWarped(old) && (hrand(100) < 25) && chaosmode) return eLand(old ^ laWarpCoast ^ laWarpSea);

  if(old == laWarpSea || old == laCaribbean || old == laKraken ||
    (old == laLivefjord && hrand(2)) || 
    (old == laOcean && (chaosmode ? hrand(2) : !generatingEquidistant)))
      return getNewSealand(old);
  
  if(old == laGraveyard && generatingEquidistant)
    return laHaunted;
  
  if(old == laOcean && gold() >= 60 && hrand(100) < 75 && !rlyehComplete()) 
    return laRlyeh;
    
  if(old == laRlyeh && !rlyehComplete())
    return laOcean;
    
  eLand tab[256];
  int cnt = 0;
  
/*   if(isHive(old) && hrand(100) < 90) {
    eLand n = old;
    while(n == old) n = eLand(laHive0 + hrand(3));
    return n;
    } */
  
  // return (hrand(2)) ? laMotion : laJungle;

  // the basic lands, always available
  tab[cnt++] = laCrossroads;
  tab[cnt++] = laIce;
  tab[cnt++] = laDesert;
  tab[cnt++] = laJungle;
  tab[cnt++] = laMotion;
  tab[cnt++] = laAlchemist;
  if(old != laDeadCaves) tab[cnt++] = laCaves;
  
  // the intermediate lands
  if(gold() >= 30) {
    tab[cnt++] = laCrossroads;
    tab[cnt++] = laMirror;
    tab[cnt++] = laOcean;
    tab[cnt++] = laLivefjord;
    tab[cnt++] = laMinefield;
    tab[cnt++] = laPalace;
    if(old == laDragon) LIKELY tab[cnt++] = laReptile;
    if(kills[moVizier]) tab[cnt++] = laEmerald;
    if(items[itFeather] >= 10) tab[cnt++] = laZebra;
    tab[cnt++] = laWarpCoast;
    if(euclid) tab[cnt++] = laWarpSea;
    // Ivory Tower tends to crash while generating equidistant
    if(!generatingEquidistant) tab[cnt++] = laIvoryTower;
    if(items[itElixir] >= 10) tab[cnt++] = laReptile;
    if(items[itIvory] >= 10 && !generatingEquidistant) tab[cnt++] = laEndorian;
    
    if(items[itKraken] >= 10) tab[cnt++] = laBurial;
    }

  if(landUnlocked(laDungeon)) {
     tab[cnt++] = laDungeon;
     if(old == laPalace) LIKELY tab[cnt++] = laDungeon;
     }
  
  // the advanced lands
  if(gold() >= 60) {
    tab[cnt++] = laStorms;
    tab[cnt++] = laWhirlwind;
    tab[cnt++] = laCrossroads;
    if(!generatingEquidistant) tab[cnt++] = laCrossroads2;
    if(items[itRuby] >= 10) {
      tab[cnt++] = laOvergrown;
      if(old == laJungle) LIKELY tab[cnt++] = laOvergrown;
      }
    if(rlyehComplete()) tab[cnt++] = laRlyeh;
    else if(chaosmode && (old == laWarpCoast || old == laLivefjord || old == laOcean)) 
      tab[cnt++] = laRlyeh;
    if(items[itStatue] >= 5 && chaosmode)
      tab[cnt++] = laTemple;
    if(old == laCrossroads || old == laCrossroads2) tab[cnt++] = laOcean;
    if(old == laOcean) tab[cnt++] = laCrossroads;
    if(items[itGold] >= 5 && items[itFernFlower] >= 5 && !kills[moVizier]) 
      tab[cnt++] = laEmerald;
    tab[cnt++] = laDryForest;
    tab[cnt++] = laWineyard;
    if(items[itGold] >= 10) tab[cnt++] = laDeadCaves;
    // tab[cnt++] = laCaribbean;
    if(items[itSpice] >= 10) {
      tab[cnt++] = laRedRock;
      if(old == laDesert) LIKELY tab[cnt++] = laRedRock;
      }
    if(old == laRedRock) LIKELY tab[cnt++] = laDesert;
    if(old == laOvergrown) LIKELY tab[cnt++] = laJungle;
    tab[cnt++] = laRose;
    }
  
  if(gold() >= 90) {
    if(!chaosmode) tab[cnt++] = laPrairie;
    if(old == laPrairie) LIKELY tab[cnt++] = laBull;
    tab[cnt++] = laBull;
    if(old == laBull && !chaosmode) LIKELY tab[cnt++] = laPrairie;
    }
  
  if(gold() >= 300)
    tab[cnt++] = laCrossroads5;
  
  if(tkills() >= 100) {
    tab[cnt++] = laGraveyard;
    if(gold() >= 60) tab[cnt++] = laHive;
    }
  
  if(killtypes() >= 20) {
    tab[cnt++] = laDragon;
    if(old == laReptile) LIKELY tab[cnt++] = laDragon;
    }
  
  if(trollUnlocked()) {
    tab[cnt++] = laTrollheim;
    if(isTrollLand(old)) LIKELY tab[cnt++] = laTrollheim;
    if(old == laTrollheim) for(int i=0; i<landtypes; i++) {
      eLand l2 = eLand(i);
      if(isTrollLand(l2)) LIKELY tab[cnt++] = l2;
      }
    }

  if(elementalUnlocked()) {
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
  
  if(hellUnlocked()) {
    if(!generatingEquidistant && old != laPrairie) tab[cnt++] = laCrossroads3;
    tab[cnt++] = laHell;
    }
  
  if(items[itHell] >= 10) {
    if(items[itDiamond] >= 10) {
      tab[cnt++] = laCocytus;
      if(old == laHell || old == laIce) LIKELY tab[cnt++] = laCocytus;
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
  while(incompatible(n, old) || (chaosmode && noChaos(n))) n = tab[hrand(cnt)];
  
  return n;  
  }

bool notDippingFor(eItem i) {
  int v = items[i] - currentLocalTreasure;
  if(v <= 10) return true;
  if(v >= 20) return false;
  return v >= hrand(10) + 10;
  }

bool notDippingForExtra(eItem i, eItem x) {
  int v = items[i] - min(items[x], currentLocalTreasure);
  if(v <= 10) return true;
  if(v >= 20) return false;
  return v >= hrand(10) + 10;
  }

eLand euland[65536];

eLand switchable(eLand nearland, eLand farland, eucoord c) {
  if(euclidland == laCrossroads4) {
    if(hrand(15) == 0)
      return getNewLand(nearland);
    return nearland;
    }
  else if(nearland == laCrossroads) {
    if(hrand(4) == 0 && (short(c)%3==0))
      return laBarrier;
    return laCrossroads;
    }
  else if(nearland == laBarrier) {
    return getNewLand(farland);
    }
  else {
    if(hrand(20) == 0 && (short(c)%3==0))
      return laBarrier;
    return nearland;
    }
  }

eLand getEuclidLand(eucoord c) {
  if(euland[c]) return euland[c];
  if(c == 0 || c == eucoord(-1) || c == 1)
    return euland[c] = euclidland;
  if(euland[eucoord(c-2)] && ! euland[eucoord(c-1)]) getEuclidLand(c-1);
  if(euland[eucoord(c+2)] && ! euland[eucoord(c+1)]) getEuclidLand(c+1);
  if(euland[eucoord(c-1)]) return 
    euland[c] = switchable(euland[c-1], euland[eucoord(c-2)], c);
  if(euland[eucoord(c+1)]) return 
    euland[c] = switchable(euland[c+1], euland[eucoord(c+2)], c);
  return euland[c] = laCrossroads;
  }

int newRoundTableRadius() {
  return 28 + 2 * items[itHolyGrail];
  }

int getAnthraxData(cell *c, bool b) {
  int d = celldistAlt(c);
  int rad = 28 + 3 * anthraxBonus;
  while(d < -rad) { 
    d += rad + 12;
    rad += 3;
    }
  while(d >= 12) {
    if(rad > 5) rad -= 3;
    else if(rad) rad--;
    d -= rad + 12;
    }
  if(b) return rad;
  return d;
  }

int roundTableRadius(cell *c) {
  if(euclid) return 28;
  if(tactic::on) return getAnthraxData(c, true);
  return c->master->alt->alt->emeraldval & GRAIL_RADIUS_MASK;
  }

int celldistAltRelative(cell *c) {
  if(tactic::on) return getAnthraxData(c, false);
  return celldistAlt(c) - roundTableRadius(c);
  }

void buildCamelotWall(cell *c) {
  c->wall = waCamelot;
  for(int i=0; i<c->type; i++) {
    cell *c2 = createMov(c, i);
    if(c2->wall == waNone && (euclid || (c2->master->alt && c->master->alt)) && celldistAlt(c2) > celldistAlt(c) && c2->monst == moNone)
      c2->wall = waCamelotMoat;
    }
  }

eLand showlist[10] = {
  laHell, laRlyeh, laAlchemist, laGraveyard, laCaves, laDesert, laIce, laJungle, laMotion, laMirror
  };

void buildBarrier(cell *c, int d, eLand l) {
  d %= 7;
  cellwalker bb(c, d);
  
  if(checkBarriersFront(bb) && checkBarriersBack(bb)) {
    c->bardir = d;
    eLand oldland = c->land;
    if(oldland == laNone) {
      raiseBuggyGeneration(c, "oldland is NONE");
      return;
      }
    eLand newland = l ? l : getNewLand(oldland);
    if(showoff) newland = showlist[(showid++) % 10];
    landcount[newland]++;
    if(d == 4 || d == 5 || d == 6) c->barleft = oldland, c->barright = newland;
    else c->barleft = newland, c->barright = oldland;
    c->landparam = 40;
    extendcheck(c);
    }
  }

bool buildBarrier4(cell *c, int d, int mode, eLand ll, eLand lr) {
  d %= 7;
  cellwalker b1(c, d);

  cellwalker b2(c, d);
  if(purehepta) cwstep(b2);
  else { cwstep(b2); cwspin(b2, 3); cwstep(b2); cwspin(b2, 3); cwstep(b2); }
  
  cellwalker b3(c, d);
  if(purehepta) {
    cwspin(b3, -1); cwstep(b3); cwspin(b3, 3);
    }
  else {
    cwstep(b3); cwspin(b3, 4); cwstep(b3); cwspin(b3, 4);
    }

  cellwalker b4(c, d);
  if(purehepta) {
    cwspin(b4, 1); cwstep(b4); cwspin(b4, -3);
    }
  else {
    cwstep(b4); cwspin(b4, -4); cwstep(b4); cwspin(b4, -4);
    }
  
  if(mode == 0) {
    if(!((checkBarriersBack(b1) && checkBarriersBack(b2)))) return false;
    if(!((checkBarriersFront(b3) && checkBarriersFront(b4)))) return false;
    }

  if(mode == 1) {
    if(!(checkBarriersFront(b3, 5, true) && checkBarriersFront(b4, 5, true))) 
      return false;
    }
    
  if(mode == 2) {
    if(!((checkBarriersBack(b1, 5, true) && checkBarriersBack(b2, 5, true)))) 
      return false;
    }
  
  eLand xl = oppositeElement(ll);
  eLand xr = oppositeElement(lr);

  c->bardir = d, c->barleft = ll, c->barright = lr; extendBarrierBack(c);
  
  c= b2.c; d=b2.spin;
  c->bardir = d, c->barleft = xl, c->barright = xr; extendBarrierBack(c);

  c= b3.c; d=b3.spin;
  c->bardir = d, c->barleft = xl, c->barright = lr; extendBarrierFront(c);

  c= b4.c; d=b4.spin;
  c->bardir = d, c->barleft = ll, c->barright = xr; extendBarrierFront(c);
  
  if(!purehepta) for(int a=-3; a<=3; a++) if(a) {
    setland(cwpeek(b1, a), a > 0 ? lr : ll);
    setland(cwpeek(b2, a), a > 0 ? xr : xl);
    setland(cwpeek(b3, a), a > 0 ? lr : xl);
    setland(cwpeek(b4, a), a > 0 ? xr : ll);
    }
  
  if(purehepta) setbarrier(b1.c), setbarrier(b2.c), setbarrier(b3.c), setbarrier(b4.c);

  if(!purehepta) {
    cell *cp;
    cp = cwpeek(b1, 0);
    cp->barleft = ll; cp->barright = lr; setbarrier(cp);
    cp = cwpeek(b2, 0);
    cp->barleft = xl; cp->barright = xr; setbarrier(cp);
    }
  
  return true;
  }    

void buildBarrierStrong(cell *c, int d, bool oldleft) {
  d %= 7;
  cellwalker bb(c, d);

  c->bardir = d;
  eLand oldland = c->land;
  eLand newland = getNewLand(oldland);
  if(showoff) newland = showlist[(showid++) % 10];
  landcount[newland]++;

  if(oldleft) c->barleft = oldland, c->barright = newland;
  else c->barleft = newland, c->barright = oldland;
  extendcheck(bb.c);
  }

void buildCrossroads2(cell *c) {

  if(buggyGeneration) return;

  if(!c) return;
  
  for(int i=0; i<c->type; i++)
    if(c->mov[i] && !c->mov[i]->landparam && c->mov[i]->mpdist < c->mpdist)
      buildCrossroads2(c->mov[i]);
  
  if(hasbardir(c))
    extendBarrier(c);

  if(c->land != laCrossroads2) return;

  if(!c->landparam) {
    for(int i=0; i<c->type; i++) { 
      cell *c2 = createMov(c, i);
      if(c2 && c2->landparam && (c2->land == laCrossroads2 || c2->land == laBarrier)) {
        for(int j=0; j<c2->type; j++) {
          createMov(c2, j);
          cell *c3 = c2->mov[j];
          if(c3 && c3->landparam && (c3->land == laCrossroads2 || c3->land == laBarrier)) {
            int h2 = c2->landparam;
            int h3 = c3->landparam;
            
            if(h2 > 100) { raiseBuggyGeneration(c2, "bad c2 landparam"); return; }
            if(h3 > 100) { raiseBuggyGeneration(c3, "bad c3 landparam"); return; }
            
            // ambiguous
            if(h2/4 == 1 && h3/4 == 3) continue;
            if(h2/4 == 3 && h3/4 == 1) continue;
            
            for(int d=0; d<c2->type; d++)
              if(emeraldtable[h2][d] == h3) {
                int nh = emeraldtable[h2][(42+d + c->spn(i) - j) % c2->type];
                if(c->landparam>0 && c->landparam != nh) { 
                  printf("CONFLICT\n"); 
                  raiseBuggyGeneration(c, "CONFLICT"); 
                  }
                c->landparam = nh;
                }
    
            if(c->landparam == 0) 
              printf("H2 = %d H3=%d\n", c2->landparam, c3->landparam);
            }
          }
        if(c->landparam == 0) {
          printf("H2 = %d\n", c2->landparam);
//        halted = true;
//        c->heat = -1;
          raiseBuggyGeneration(c, "buildCrossroads2x");
          return;
          }
        }
      }
    
    if(c->landparam) {
//    for(int i=0; i<c->type; i++) { 
//      cell *c2 = c->mov[i];
//      buildCrossroads2(c2);
//      }
      }
    else {
      raiseBuggyGeneration(c, "buildCrossroads2");
      return;
      }
    }

  int h = c->landparam;
  
  if(h/4 >= 8 && h/4 <= 11) {
    for(int i=0; i<c->type; i++) { 
      createMov(c, i)->land = laCrossroads2;
      if(!c->mov[i]->landparam) buildCrossroads2(c->mov[i]);
      }
    if(h/4 == 8 || h/4 == 10)
    for(int i=0; i<c->type; i++) { 
      if(c->mov[i] && c->mov[i]->landparam == h-4) {
        bool oldleft = true;
        for(int j=1; j<=3; j++) 
          if(c->mov[(i+j)%7] && c->mov[(i+j)%7]->mpdist < c->mpdist)
            oldleft = false;
            
        c->landparam = h;
        buildBarrierStrong(c, i, oldleft);
        c->landparam = h;
        extendBarrier(c);
        }
      }
    }
  }

extern bool bugtrack;

bool generatingEquidistant = false;

void buildAnotherEquidistant(cell *c) {
  //printf("building another coast\n");
  
  if(yendor::on) return;

  int gdir = -1;
  for(int i=0; i<c->type; i++) {
    if(c->mov[i] && c->mov[i]->mpdist < c->mpdist) gdir = i;
    }
  if(gdir == -1) return;
  
  generatingEquidistant = true;
  
  int radius = c->land == laOcean ? 30 : HAUNTED_RADIUS + 5;

  cellwalker cw(c, (gdir+3) % c->type);
  vector<cell*> coastpath;
  while(size(coastpath) < radius || cw.c->type != 7) {
    // this leads to bugs for some reason!
    if(cw.c->land == laCrossroads2) {
#ifdef AUTOPLAY
      if(doAutoplay) printf("avoiding the Crossroads II\n"); // todo
#endif
      generatingEquidistant = false;
      return;
      }
    if(cw.c->bardir != NODIR) {
      generatingEquidistant = false;
      return;
      }
    /* forCellEx(c2, cw.c) if(c2->bardir != NODIR) {
      generatingEquidistant = false;
      return;
      } */
    coastpath.push_back(cw.c);
    if(cw.c->land == laNone && cw.c->mpdist <= 7) {
      raiseBuggyGeneration(cw.c, "landNone 1");
      for(int i=0; i<size(coastpath); i++) coastpath[i]->item = itPirate;
      return;
      }
    cwstep(cw); cwspin(cw, 3); 
    if(cw.c->type == 7 && hrand(2) == 0) cwspin(cw, 1);
    }
  int mpd[10];
  for(int i=0; i<10; i++) mpd[i] = coastpath[i]->mpdist;
  coastpath.push_back(cw.c);
  // printf("setdists\n");
  for(int i=1; i<size(coastpath) - 1; i++) {
    if(coastpath[i-1]->land == laNone) {
      raiseBuggyGeneration(cwt.c, "landNone 3");
      {for(int i=0; i<10; i++) printf("%d ", mpd[i]);} printf("\n");
      for(int i=0; i<size(coastpath); i++) coastpath[i]->item = itPirate;
      return;
      }
    setdist(coastpath[i], BARLEV, coastpath[i-1]);
    setdist(coastpath[i], BARLEV-1, coastpath[i-1]);
    if(i < size(coastpath) - 5) {
      coastpath[i]->bardir = NOBARRIERS;
//      coastpath[i]->item = itSapphire;
//      forCellEx(c2, coastpath[i]) c2->bardir = NOBARRIERS;
      }
    }
  
  //printf("building barrier\n");
  cell *c2 = coastpath[coastpath.size() - 1];
  int bd = 2 + (hrand(2)) * 3;
  
  bool nowall = false;
  
  if(c2->land == laNone) {
    raiseBuggyGeneration(c2, "landNone 2");
    for(int i=0; i<size(coastpath); i++) coastpath[i]->item = itPirate;
    return;
    }

  if(c2->land != c->land) {
    generatingEquidistant = false;
    return; // prevent gravity anomalies
    }
  
  // else if(c->type == 7 && hrand(10000) < 20 && !isCrossroads(c->land) && gold() >= 200)
  if(c2->type == 7 && gold() >= 200 && hrand(10) < 2 && buildBarrierNowall(c2, laCrossroads4, true))  {
    nowall = true;
    // raiseBuggyGeneration(c2, "check");
    // return;
    }
  else buildBarrier(c2, bd);
  //printf("building barrier II\n");
  if(hasbardir(c2)) extendBarrier(c2);

  for(int i=size(coastpath)-(nowall?1:2); i>=0; i--) {
    for(int j=BARLEV; j>=6; j--)
      setdist(coastpath[i], j, NULL);
    }

  generatingEquidistant = false;
  }

bool bugtrack = false, bugfound = false;

#define UNKNOWN 65535

int coastval(cell *c, eLand base) {
  if(!c) return UNKNOWN;
  if(c->land == laNone) return UNKNOWN;
  if(base == laGraveyard) {
    if(c->land == laHaunted || c->land == laHauntedWall)
      return 0;
    if(c->land != laGraveyard && c->land != laHauntedBorder) return 30;
    }
  else {
    if(c->land == laOceanWall || c->land == laCaribbean || c->land == laWhirlpool ||
      c->land == laLivefjord || c->land == laWarpSea || c->land == laKraken)
      return 30;
    if(c->land  != laOcean && !isGravityLand(c->land) && c->land != laHaunted) {
      return 0;
      }
    }
  if(!c->landparam) return UNKNOWN;
  return c->landparam;
  }

eMonster crossroadsMonster() {

  static eMonster weak[9] = {
    moYeti, moGoblin, moRanger, moOrangeDog, moRunDog, moMonkey, moZombie,
    moDesertman, moCultist
    };
  
  if(hrand(10) == 0) return weak[hrand(9)];

  static eMonster m[24] = {
    moWorm, moTentacle, 
    moTroll, moEagle,
    moLesser, moGreater, moPyroCultist, moGhost,
    moFireFairy, moIvyRoot, moHedge,
    moLancer, moFlailer, moVineBeast,
    moBomberbird, moAlbatross, moRedTroll,
    moWaterElemental, moAirElemental, moFireElemental,
    moFatGuard, moMiner, moPalace, moVizier
    };
  return m[hrand(24)];
  }

eMonster wanderingCrossroadsMonster() {
  while(true) {
    eMonster m = crossroadsMonster();
    if(!isIvy(m) && m != moTentacle) return m;
    }
  }

bool checkInTree(cell *c, int maxv) {
  if(c->landparam <= 3) return false;
  if(!maxv) return false;
  if(c->landflags) return true;
  for(int t=0; t<c->type; t++)
    if(c->mov[t] && c->mov[t]->landparam < c->landparam && checkInTree(c->mov[t], maxv-1))
      return true;
  return false;
  }

void buildEquidistant(cell *c) {
  if(!c) return;
  if(c->landparam) return;
  if(chaosmode) return;
  eLand b = c->land;
  if(!b) { 
    printf("land missing at %p\n", c); 
    describeCell(c);
    for(int i=0; i<c->type; i++) if(c->mov[i])
      describeCell(c->mov[i]);
    // buggycells.push_back(c);
    }
  if(b == laHauntedBorder) b = laGraveyard;
  int mcv = UNKNOWN;

  // find the lowest coastval
  for(int i=0; i<c->type; i++) {
    int cv = coastval(createMov(c,i), b);
    if(cv < mcv) mcv = cv;
    }
  
  int mcv2 = 0;
  
  if(bugfound) { mcv = 1; c->landparam = 10; }
   
  else if(mcv == 0) {
    // if(generatingEquidistant) printf("mcv=0\n");
    c->landparam = 1;
    }
  else {
    // if it appears twice, increase it
    int qcv = 0;
    int sid = 0;
    for(int i=0; i<c->type; i++) 
      if(coastval(c->mov[i], b) == mcv)
        qcv++, sid = i;
    
    // if(generatingEquidistant) printf("qcv=%d mcv=%d\n", qcv, mcv);
    if(qcv >= 2) c->landparam = mcv+1; // (mcv == UNKNOWN ? UNKNOWN : mcv+1);
    else {
      // if(qcv != 1) { printf("qcv = %d\n", qcv);  exit(1); }
      cell *c2 = c->mov[sid];
      int bsid = c->spn(sid);
      for(int j=0; j<7; j++) {
        int q = (bsid+j+42) % c2->type;
        cell *c3 = c2->mov[q];
        if(coastval(c3, b) < mcv) {
          cell *c4 = createMovR(c2, bsid+1);
          if(c4->land == laNone && c2->mpdist <= BARLEV) setdist(c4, BARLEV, c2);
          buildEquidistant(c4);
          mcv2 = coastval(c4, b);
          break;
          }
        q = (bsid-j+42) % c2->type;
        c3 = c2->mov[q];
        if(coastval(c3, b) < mcv) {
          cell *c4 = createMovR(c2, bsid-1);
          if(c4->land == laNone && c2->mpdist <= BARLEV) setdist(c4, BARLEV, c2);
          buildEquidistant(c4);
          mcv2 = coastval(c4, b);
          break;
          }
        }
      if(bugfound) c->item = itSapphire;
      if(mcv2 > mcv) mcv2 = mcv;
      if(mcv2 == 0) mcv2 = mcv;
      c->landparam = mcv2+1;

      /* if(c->heat < 3)
        raiseBuggyGeneration(c, "low heat"); */
      }
    }
  
  if(!c->landparam) {
    // int z = int(c->heat);
    if(c->item || c->monst) 
      printf("building coast over %s/%s, mpdist = %d\n", iinf[c->item].name, minf[c->monst].name,
        c->mpdist);
    if(c->land == laOcean) c->wall = waSea;
    }
  
  if(c->land == laEndorian) {
    if(c->landparam == 1 && c->type == 7) {
      for(int i=0; i<7; i++) {
        int i1 = (i+1) % 7;
        if(c->mov[i] && c->mov[i]->land != laEndorian && c->mov[i]->land != laNone)
        if(c->mov[i1] && c->mov[i1]->land != laEndorian && c->mov[i1]->land != laNone) {
          c->landflags = 2;
          c->wall = waTrunk;
          }
        }
      }
    else if(c->landparam == 1 && c->type == 6) {
      for(int i=0; i<6; i++) {
        int i1 = (i+1) % 6;
        int i2 = (i+2) % 6;
        int i4 = (i+4) % 6;
        int i5 = (i+5) % 6;
        if(c->mov[i] && c->mov[i]->land == laBarrier && c->mov[i]->type == 7)
        if(c->mov[i1] && c->mov[i1]->land != laBarrier)
        if(c->mov[i2] && c->mov[i2]->land != laBarrier) 
        if(c->mov[i4] && c->mov[i4]->land != laBarrier)
        if(c->mov[i5] && c->mov[i5]->land != laBarrier) {
          c->landflags = 2;
          c->wall = waTrunk;
          }

        if(c->mov[i] && c->mov[i]->land != laEndorian && c->mov[i]->land != laNone && c->mov[i]->type == 7)
        if(c->mov[i1] && c->mov[i1]->land != laEndorian && c->mov[i1]->land != laNone)
        if(c->mov[i5] && c->mov[i5]->land != laEndorian && c->mov[i5]->land != laNone) {
          c->landflags = 3;
          c->wall = waTrunk;
          }
        }
      }
    else if(c->landparam > 1) {
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if(c2 && c2->landparam < c->landparam && c2->landflags) {
          bool ok = false;
          if(c2->landflags == 3)
            ok = true;
          else if(c2->landflags == 2) {
            ok = c->mov[(i+1)%c->type]->landparam != c->landparam-1
             &&  c->mov[(i+c->type-1)%c->type]->landparam != c->landparam-1;
            }
          else for(int j=0; j<c2->type; j++) {
            cell *c3 = c2->mov[j];
            if(c3 && c3->landparam < c2->landparam && c3->landflags)
              if(c->spn(i) == (j+3)%c2->type || c->spn(i) == (j+c2->type-3)%c2->type)
                ok = true;
            }
          if(ok) {
            c->wall = waTrunk;
            c->landflags = 1;
            }
          }
        if(c2 && c2->landparam < c->landparam && c2->landflags == 1 && c->type == 7) {
          cell *c3 = c->mov[(i+1)%7];
          if(c3 && c3->landparam < c->landparam && c3->landflags == 1) {
            c->wall = waTrunk;
            c->landflags = 2;
            }
          }
        }
      
      if(!c->landflags && checkInTree(c, 5)) {
        int lev = hrand(100);
        if(lev < 10) c->wall = waSolidBranch;
        else if(lev < 20) c->wall = waWeakBranch;
        else c->wall = waCanopy;
        }
      }
    }
  
  if(c->landparam > 30 && b == laOcean && !generatingEquidistant && hrand(10) < 5) 
    buildAnotherEquidistant(c);

  if(c->landparam > HAUNTED_RADIUS+5 && b == laGraveyard && !generatingEquidistant && hrand(100) < (purehepta?25:5) && items[itBone] >= 10) 
    buildAnotherEquidistant(c);
  }

void beCIsland(cell *c) {
  int i = hrand(3);
  if(i == 0) c->wall = waCIsland;
  if(i == 1) c->wall = waCIsland2;
  if(i == 2) c->wall = waCTree;
  return;
  }

void generateTreasureIsland(cell *c) {
  if(!euclid) generateAlts(c->master);
  if(isOnCIsland(c)) return;
  
  bool src = hrand(100) < 10;
  if(src) {
    beCIsland(c);
    if(c->wall == waCTree) return;
    }
  cell* ctab[7];
  int qc = 0, qlo, qhi;
  for(int i=0; i<c->type; i++) {
    cell *c2 = createMov(c, i);
    if(!euclid) generateAlts(c2->master);
    if((euclid || (c->master->alt && c2->master->alt)) && celldistAlt(c2) < celldistAlt(c)) {
      ctab[qc++] = c2;
      qlo = i; qhi = i;
      while(true) {
        qlo--;
        c2 = createMovR(c, qlo);
        if(!euclid && !c2->master->alt) break;
        if(celldistAlt(c2) >= celldistAlt(c)) break;
        ctab[qc++] = c2;
        }
      while(true) {
        qhi++;
        c2 = createMovR(c, qhi);
        if(!euclid && !c2->master->alt) break;
        if(celldistAlt(c2) >= celldistAlt(c)) break;
        ctab[qc++] = c2;
        }
      break;
      }
    }
  if(!qc) { 
    printf("NO QC\n"); c->wall = waSea; 
    for(int i=0; i<c->type; i++) printf("%d ", celldistAlt(c->mov[i]));
    printf("vs %d\n", celldistAlt(c));
    return; 
    }
  cell* c2 = createMovR(c, (qlo+qhi)/2);
  generateTreasureIsland(c2);
  if(!src) {
    c->wall = c2->wall;
    if(c->wall != waCTree && hrand(100) < 15)
      c->wall = (c->wall == waCIsland ? waCIsland2 : waCIsland);
    }
  if(src && c2->wall == waCTree && (euclid||c->master->alt) && celldistAlt(c) <= -10) {
    bool end = true;
    for(int i=0; i<qc; i++) {
      generateTreasureIsland(ctab[i]);
      if(ctab[i]->wall != waCTree)
        end = false;
      }
    // printf("%p: end=%d, qc=%d, dist=%d\n", c, end, qc, celldistAlt(c));
    if(end) c->item = itPirate;
    else c->item = itCompass;
    }
  }

void buildRedWall(cell *c, int gemchance) {
  if(c->monst) return;
  int ki = PT(kills[moHexSnake] + kills[moRedTroll], 50);
  c->wall = waRed3;
  if(hrand(100+ki) < gemchance + ki)
    c->item = itRedGem;
  if(items[itRedGem] >= 10 && hrand(8000) < gemchance)
    c->item = itOrbSpace;
  else if(hrand(8000) < gemchance * PRIZEMUL)
    placePrizeOrb(c);
  }

int palaceHP() {
  if(tactic::on && isCrossroads(firstland))
    return 4;
  if(items[itPalace] < 3) // 1+2
    return 2;
  else if(items[itPalace] < 10) // 1+2+3+4
    return 3;
  else if(items[itPalace] < 21) // 1+2+3+4+5+6
    return 4;
  else if(items[itPalace] < 35)
    return 5;
  else if(items[itPalace] < 50)
    return 6;
  else return 7;
  }

cell *randomDown(cell *c) {
  cell *tab[7];
  int q=0;
  for(int i=0; i<c->type; i++) 
    if(c->mov[i] && coastval(c->mov[i], laIvoryTower) < coastval(c, laIvoryTower))
      tab[q++] = c->mov[i];
  if(!q) return NULL;
  if(q==1) return tab[0];
  return tab[hrand(q)];
  }

// which=1 => right, which=-1 => left
// set which=1,bonus=1 to get right neighbor on level

typedef int cellfunction(cell*);

cell *chosenDown(cell *c, int which, int bonus, cellfunction* cf) {
  int d = (*cf)(c)-1;
  for(int i=0; i<c->type; i++) {
    if(!c->mov[i]) createMov(c, i);
    if(c->mov[i]->mpdist > BARLEV && cf == coastvalEdge) setdist(c->mov[i], BARLEV, c);
    
    if((*cf)(c->mov[i]) == d) {
      int i2 = (i+which+42)%c->type;
      createMov(c, i2);
      if((*cf)(c->mov[i2]) == d)
        return createMovR(c, i2+bonus);
      else return createMovR(c, i+bonus);
      }
    }
  // printf("nothing down here\n");
  return NULL;
  }

int edgeDepth(cell *c) {
  if(c->land == laIvoryTower || c->land == laEndorian || c->land == laDungeon) 
    return coastvalEdge(c);
  else if(c->land != laBarrier) {
    for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->land == laBarrier)
      return -20+c->cpdist;
    }
  return 0;
  }

int getHauntedDepth(cell *c) {
  if((tactic::on || euclid) && c->land == laHaunted) return celldist(c);
  if(c->land == laHaunted) return c->landparam;
  if(c->land == laHauntedWall) return 0;
  if(c->land == laHauntedBorder || c->land == laGraveyard) return -c->landparam;
  return -100;
  }

int compassDist(cell *c) {
  if(c->master->alt) return celldistAlt(c);
  if(isHaunted(c->land) || c->land == laGraveyard) return getHauntedDepth(c);
  return 500;
  }

int towerval(cell *c, cellfunction* cf) {
  // if(c->land != laEdge) return 0;
  cell *cp1 = chosenDown(c, 1, 1, cf);
  if(!cp1) return 0;
  /* cell *cp2 = chosenDown(cp1, 1, 1);
  if(!cp2) return 0;
  cell *cm1 = chosenDown(c, -1, -1);
  if(!cm1) return 0;
  cell *cm2 = chosenDown(cm1, -1, -1);
  if(!cm2) return 0; */

  /* return 
    (c->type-6) 
    + 2*(cp1->type-6) + 4*(cp2->type-6) 
    + 8*(cm1->type-6) +16*(cm2->type-6); */
  
  int under = 0;
  int cfc = (*cf)(c);
  for(int i=0; i<c->type; i++) {
    if(c->mov[i] && (*cf)(c->mov[i]) < cfc)
      under++;
    }
  return (c->type-6) + 2*(cp1->type-6) + 4*under;
  }

int hardness_empty() {
  return yendor::hardness() * (yendor::hardness() * 3/5 - 2);
  }

int hivehard() {
  return items[itRoyalJelly] + hardness_empty();
  // 0, 5, 40, 135
  }

eMonster randomHyperbug() {
  int h = hivehard();
  if(hrand(200) < h)
    return moBug2;
  return eMonster(moBug0 + hrand(BUGCOLORS));
  // 50: 25/25/50
  // 100: 
  }

#define RANDPATC(c) (randpattern(c,randompattern[c->land]))
#define RANDPAT (randpattern(c,randompattern[c->land]))
#define RANDPAT3(i) (randpatternMajority(c,i,RANDITER))
#define RANDPATV(x) (randpattern(c,randompattern[x]))

bool buildBarrierNowall(cell *c, eLand l2, bool force) {

  int dtab[3] = {0,1,6};
  
  if(c->land == laNone) {
    printf("barrier nowall! [%p]\n", c);
    raiseBuggyGeneration(c, "barrier nowall!");
    return false;
    }
  
  for(int i=0; i<3; i++) {
    int d = dtab[i];
    if(force) d=1;
    cellwalker cw(c, d);
    
    if(force || (checkBarriersNowall(cw, 0, -1) && checkBarriersNowall(cw, 0, 1))) {
      eLand l1 = c->land;
      checkBarriersNowall(cw, 0, -1, l1, l2);
      checkBarriersNowall(cw, 0, 1, l1, l2);
      extendBarrier(c);
      return true;
      }
    }
  
   return false;
  }

void setLandQuotient(cell *c) {
  int fv = zebra40(c);
  if(fv/4 == 4 || fv/4 == 6 || fv/4 == 5 || fv/4 == 10) fv ^= 2;
  if(euclidland == laWarpCoast)
    if(fv%4==0 || fv%4 == 2) setland(c, laWarpSea);
  if(euclidland == laElementalWall)
    setland(c, eLand(laEFire + (fv%4)));
  }

bool quickfind(eLand l) {
  if(l == cheatdest) return true;
#ifdef TOUR
  if(tour::on && tour::quickfind(l)) return true;
#endif
  return false;
  }

void setLandSphere(cell *c) {
  if(euclidland == laWarpCoast)
    setland(c, getHemisphere(c, 0) > 0 ? laWarpCoast : laWarpSea);
  if(euclidland == laElementalWall) {
    int x = getHemisphere(c, 1);
    int y = getHemisphere(c, 2);
    if(x > 0 && y > 0) setland(c, laEFire);
    else if(x > 0 && y < 0) setland(c, laEAir);
    else if(x < 0 && y < 0) setland(c, laEWater);
    else if(x < 0 && y > 0) setland(c, laEEarth);
    else if(x > 0) 
      c->land = laElementalWall, c->barleft = laEAir, c->barright = laEFire;
    else if(x < 0) 
      c->land = laElementalWall, c->barleft = laEEarth, c->barright = laEWater;
    else if(y > 0) 
      c->land = laElementalWall, c->barleft = laEEarth, c->barright = laEFire;
    else if(y < 0) 
      c->land = laElementalWall, c->barleft = laEAir, c->barright = laEWater;
    if(c->land == laElementalWall && c->type != 6)
      c->wall = getElementalWall(hrand(2) ? c->barleft : c->barright);
    }
  if(euclidland == laCrossroads || euclidland == laCrossroads2 || euclidland == laCrossroads3) {
    int x = getHemisphere(c, 1);
    if(x == 0 || (euclidland == laCrossroads3 && getHemisphere(c, 2) == 0)) 
      setland(c, laBarrier), c->wall = waBarrier;
    else setland(c, euclidland);
    if(euclidland == laCrossroads3 && c->type != 6 && c->master->fiftyval == 1)
      c->wall = waBigTree;        
    }
  }

void setLandEuclid(cell *c) {
  setland(c, euclidland);
  if(euclidland == laCrossroads) {
    eucoord x, y;
    decodeMaster(c->master, x, y);
    setland(c, getEuclidLand(y+2*x));
    }
  if(euclidland == laCrossroads4) {
    eucoord x, y;
    decodeMaster(c->master, x, y);
    c->land = getEuclidLand(y);
    }
  if(euclidland == laWhirlpool) {
    c->land = laOcean;
    c->landparam = 99;
    }
  if(euclidland == laPrincessQuest) setland(c, laPalace);
  if(euclidland == laOcean) {
    eucoord x, y;
    decodeMaster(c->master, x, y);
    int y0 = y; if(y>50000) y0 -= 65536; y0 += 10;
    if(y0 == 0) 
      { setland(c, laBarrier); if(ishept(c)) c->land = laRlyeh; }
    else if(y0<0) setland(c, laRlyeh);
    else c->landparam = y0;
    }
  if(euclidland == laIvoryTower || euclidland == laDungeon) {
    eucoord x, y;
    decodeMaster(c->master, x, y);
    int y0 = y; if(y>50000) y0 -= 65536; y0 = -y0; y0 -= 5;
    if(y0 == 0) 
      {setland(c, laBarrier); if(ishept(c)) setland(c, laAlchemist); }
    else if(y0<0) setland(c, laAlchemist);
    else {
      c->landparam = y0;
      }
    }
  if(euclidland == laElementalWall) {
    eucoord x, y;
    decodeMaster(c->master, x, y);
    int y0 = y; if(y>32768) y0 -= 65536;
    int x0 = x +y/2;
    
    int id = 0;
    if(y0&16) id += 2;
    if(x0&16) id += 1;
    
    x0 += 8; y0 += 8;

    y0--; x0++;
    int id2 = 0;
    if(y0&16) id2 += 2;
    if(x0&16) id2 += 1;
    
    setland(c, eLand(laEFire + id));
    
    if(((y0&15) == 15 && (x0&1)) || ((x0&15) == 0 && ((y0+1)&1))) {
      if(c->land == laEFire) c->wall = waEternalFire;
      if(c->land == laEWater) c->wall = waSea;
      if(c->land == laEAir) c->wall = waChasm;
      if(c->land == laEEarth) c->wall = waStone;
      c->barright = c->land;
      c->barleft = eLand(laEFire+id2);
      setland(c, laElementalWall);
      }
    }
  if(euclidland == laCrossroads3) {
    eucoord x, y;
    decodeMaster(c->master, x, y);
    int y0 = y; if(y>32768) y0 -= 65536;
    int x0 = x +y/2;
    
    x0 += 24; y0 += 8;
    
    int id = 0;
    if(y0&16) id ^= 1;
    if(x0&16) id ^= 1;
    
    setland(c, id ? laCrossroads3 : laDesert);
    
    if(((y0&15) == 15 && (x0&1)) || ((x0&15) == 0 && ((y0+1)&1))) {
      setland(c, laBarrier);
      c->wall = waBarrier;
      }
    }
  if(euclidland == laWarpCoast) {
    eucoord x, y;
    decodeMaster(c->master, x, y);
    
    int zz = 2*short(x)+short(y) + 10;
    zz %= 30; if(zz<0) zz += 30;
    if(zz >= 15)
      setland(c, laWarpSea);
    else
      setland(c, laWarpCoast);
    }
  }

void buildBigStuff(cell *c, cell *from) {
  if(sphere || quotient) return;
  bool deepOcean = false;
        
  if(c->land == laOcean) {
    if(!from) deepOcean = true;
    else for(int i=0; i<from->type; i++) {
      cell *c2 = from->mov[i];
      if(c2 && c2->land == laOcean && c2->landparam > 30) {
        deepOcean = true;
        }
      if(c2) forCellEx(c3, c2) if(c3 && c3->land == laOcean && c3->landparam > 30)
        deepOcean = true;
      }
    }
  
  if(c->land == laGraveyard) {
    if(!from) deepOcean = true;
    else for(int i=0; i<from->type; i++) {
      cell *c2 = from->mov[i];
      if(c2 && c2->landparam > HAUNTED_RADIUS+5)
        deepOcean = true;
      }
    }
  
  if(generatingEquidistant) deepOcean = false;
  
  // buildgreatwalls
  
  if(chaosmode) {
    if(c->type == 7 && hrand(10000) < 9000 && c->land && buildBarrierNowall(c, getNewLand(c->land))) 
      {}
    }
  
  else if(c->type == 7 && isWarped(c->land) && hrand(10000) < 3000 && c->land && 
    buildBarrierNowall(c, eLand(c->land ^ laWarpSea ^ laWarpCoast))) ;
  
  else if(c->type == 7 && c->land == laCrossroads4 && hrand(10000) < 7000 && c->land && 
    buildBarrierNowall(c, getNewLand(laCrossroads4))) ;
  
  else if(c->type == 7 && hrand(10000) < 20 && !generatingEquidistant && !yendor::on && !tactic::on && !isCrossroads(c->land) && gold() >= 200 &&
    !isSealand(c->land) && !isHaunted(c->land) && !isGravityLand(c->land) && 
    (c->land != laRlyeh || rlyehComplete()) &&
    c->land != laTortoise && c->land != laPrairie && c->land && 
    !(c->land == laGraveyard && !deepOcean) 
    && c->land != laCanvas
    ) {
    buildBarrierNowall(c, laCrossroads4) ;
    }
  
  else if(c->land == laCrossroads2 && !purehepta)
    buildCrossroads2(c);
  
  else if(c->land == laPrairie && c->LHU.fi.walldist == 0) {
    for(int bd=0; bd<7; bd++) {
      int fval2 = createStep(c->master, bd)->fieldval;
      int wd = fp43.gmul(fval2, fp43.inverses[c->fval-1]);
      if(fp43.distwall[wd] == 0) {
        buildBarrier(c, bd); 
        break;
        }
      }
    }

  else if(c->type == 7 && c->land && hrand(10000) < (
    showoff ? (cwt.c->mpdist > 7 ? 0 : 10000) : 
    isGravityLand(c->land) ? 0 :
    generatingEquidistant ? 0 :
    c->land == laPrairie ? 0 :
    (yendor::on && yendor::nexttostart) ? 10000 :
    princess::challenge ? 0 :
    isElemental(c->land) ? 4000 : 
    (yendor::on && (yendor::generating || !(yendor::clev().flags & YF_WALLS))) ? 0 :
    c->land == laCrossroads3 ? 10000 : 
    c->land == laCrossroads ? 5000 : 
    c->land == laCrossroads2 ? 10000 : 
    c->land == laCrossroads5 ? 10000 : 
    c->land == laCrossroads4 ? 0 : 
    (tactic::on && !tactic::trailer) ? 0 :
    c->land == laCaribbean ? 500 :
    (c->land == laWarpSea || c->land == laWarpCoast) ? 500 :
    c->land == laStorms ? 250 :
    c->land == laCanvas ? 0 :
    c->land == laHaunted ? 0 :
    (c->land == laGraveyard && !deepOcean) ? 0 :
    (c->land == laGraveyard && items[itBone] >= 10) ? 120 :
    c->land == laOcean ? (deepOcean ? (purehepta ? 250 : 2000) : 0) :
    c->land == laDragon ? 120 :
    50))
  {
    
    int bd = 2 + hrand(2) * 3;
    
    buildBarrier(c, bd); 

    /* int bd = 2;      
    buildBarrier4(c, bd, 0, getNewLand(c->land), c->land); */
    }
      
  if((!chaosmode) && bearsCamelot(c->land) && c->type == 7 && 
    (quickfind(laCamelot) || (hrand(2000) < 200 && 
    items[itEmerald] >= 5 && !tactic::on))) {
    int rtr = newRoundTableRadius();
    heptagon *alt = createAlternateMap(c, rtr+14, hsOrigin);
    if(alt) {
      alt->emeraldval = rtr;
      alt->fiftyval = c->land;
      }
    }

  if(!chaosmode) {
  
    // buildbigstuff

    if(c->land == laRlyeh && c->type == 7 && 
      (quickfind(laTemple) || (hrand(2000) < 100 && 
      items[itStatue] >= 5 && !randomPatternsMode && 
      !tactic::on && !yendor::on)))
      createAlternateMap(c, 2, hsA);

    if(c->land == laJungle && c->type == 7 && 
      (quickfind(laMountain) || (hrand(2000) < 100 && 
      !randomPatternsMode && !tactic::on && !yendor::on && landUnlocked(laMountain))))
      createAlternateMap(c, 2, hsA);

    if(c->land == laOvergrown && c->type == 7 && 
      (quickfind(laClearing) || (hrand(2000) < 25 && 
      !randomPatternsMode && items[itMutant] >= 5 &&
      !tactic::on && !yendor::on))) {
      heptagon *h = createAlternateMap(c, 2, hsA);
      if(h) clearing::bpdata[h].root = NULL;
      }

    if(c->land == laStorms && c->type == 7 && hrand(2000) < 1000 && !randomPatternsMode) {
      heptagon *h = createAlternateMap(c, 2, hsA);
      if(h) h->alt->emeraldval = hrand(2);
      }

    if(c->land == laOcean && c->type == 7 && deepOcean && !generatingEquidistant && 
      (quickfind(laWhirlpool) || (
        hrand(2000) < (purehepta ? 500 : 1000) && !tactic::on && !yendor::on)))
      createAlternateMap(c, 2, hsA);

    if(c->land == laCaribbean && c->type == 7)
      createAlternateMap(c, 2, hsA);

    if(c->land == laPalace && c->type == 7 && !princess::generating && !shmup::on && multi::players == 1 &&
      (princess::forceMouse ? (from && from->pathdist != INF) : (hrand(2000) < 20)) && 
      !c->master->alt && 
      (princess::challenge || kills[moVizier]) && !tactic::on && !yendor::on) 
      createAlternateMap(c, 141, hsOrigin, waPalace);
    }
  
  if(hasbardir(c)) extendBarrier(c);
  }

void buildIvoryTower(cell *c) {
  /* if(int(c->landparam) % 5 == 0) 
    c->wall = waCamelot;
    */
  
  if(euclid) {
    eucoord x, y;
    decodeMaster(c->master, x, y);
    string tab[] = {
      ".####...",
      "L...L...",
      ".L..L...",
      "L...L...",
      "........",
      "........"
      };
    int y0 = y; if(y>32768) y0 -= 65536;
    
    y0 += 5; y0 %= 12; if(y0<0) y0+=12;
    
    if(y0 >= 6) { y0 -= 6; x += 4; }
    
    char ch = tab[y0][(x+(y+1)/2)&7];
    
    if(ch == '#')
      c->wall = waPlatform;
    else if(ch == 'L')
      c->wall = waLadder;
    }
  
  else if(true) {
    
    cell *c2 = c;
    cell *c3 = c;
    
    bool rdepths[5];
    for(int i=0; i<5; i++) {
      if(coastvalEdge(c2) == 0) { 
        rdepths[i] = false;
        }
      else {
        cell *c4 = c2;
        if(c2 != c3 && !isNeighbor(c2, c3)) {
          for(int i=0; i<c2->type; i++) if(c2->mov[i] && isNeighbor(c2->mov[i], c3))
            c4 = c2->mov[i];
          }
        rdepths[i] = c2 && c3 && c4 && (c2->landflags == 3 || c3->landflags == 3 || c4->landflags == 3);
        c2 = chosenDown(c2, 1, 0); // if(!c2) break;
        c3 = chosenDown(c3, -1, 0);
        if(!c2) { raiseBuggyGeneration(c, "ivory c2"); return; }
        if(!c3) { raiseBuggyGeneration(c, "ivory c3"); return; }
        }
      }
    
    if(rdepths[3]) {
      c->wall = waPlatform;
//        if(!c4->item) c4->item = itPalace;
      }
    else if(!rdepths[2] && !rdepths[4] && !rdepths[1]) {
      c2 = c;
      c3 = c;
      cell *c4 = chosenDown(c, 1, 1);
      cell *c5 = chosenDown(c, -1, -1);
      for(int i=0; i<3; i++) {
        if(coastvalEdge(c2) == 0) break;
        if(c2 && c4 && c4->landflags == 3 && c2->landflags != 3 && c4 == chosenDown(c2, 1, 1)) 
          c->wall = waLadder;
        if(c3 && c5 && c5->landflags == 3 && c3->landflags != 3 && c5 == chosenDown(c3, -1, -1)) 
          c->wall = waLadder;
        buildEquidistant(c4); buildEquidistant(c5);
        if(c2) c2 = chosenDown(c2,  1, 0);
        if(c3) c3 = chosenDown(c3, -1, 0);
        if(c4) c4 = chosenDown(c4,  1, 0);
        if(c5) c5 = chosenDown(c5, -1, 0);
        }
      }
    }
  else c->wall = waCIsland;
  }

int dungeonFlags(cell *c) {
  if(!c) return 0;
  buildEquidistant(c);
  bool rdepths[5];

  cell *c2 = c;
  cell *c3 = c;
    
  int switchcount = 0;
  for(int i=0; i<5; i++) {
    if(coastvalEdge(c2) == 0) { 
      rdepths[i] = false;
      }
    else {
      cell *c4 = c2;
      if(c2 != c3 && !isNeighbor(c2, c3)) {
        for(int i=0; i<c2->type; i++) if(c2->mov[i] && isNeighbor(c2->mov[i], c3))
          c4 = c2->mov[i];
        }
      rdepths[i] = c2 && c3 && c4 && (c2->landflags == 3 || c3->landflags == 3 || c4->landflags == 3);
      if((c2&&c2->landflags == 1) || (c3&&c3->landflags == 1) || (c4&&c4->landflags == 1))
        switchcount++;
      c2 = chosenDown(c2, 1, 0); // if(!c2) break;
      c3 = chosenDown(c3, -1, 0);
      if(!c2) { raiseBuggyGeneration(c, "ivory c2"); return 0; }
      if(!c3) { raiseBuggyGeneration(c, "ivory c3"); return 0; }
      }
    }
  
  int res = 0;
  
  if(rdepths[3]) res |= 1;
  if(rdepths[2]) res |= 2;
  if(switchcount&1) res |= 4;
  
  return res;
  }

void placeGate(cell *c, eWall w) {
  if(w == waOpenGate) {
    c->wall = waClosedGate;
    toggleGates(c, waOpenPlate, 0);
    }
  if(w == waClosedGate) {
    c->wall = waOpenGate;
    toggleGates(c, waClosePlate, 0);
    }
  }

bool isGate(eWall w) {
  return w == waOpenGate || w == waClosedGate;
  }

void placeRandomGate(cell *c) {
  placeGate(c, hrand(2) ? waOpenGate : waClosedGate);
  }

void buildDungeon(cell *c) {
  /* if(int(c->landparam) % 5 == 0) 
    c->wall = waCamelot;
    */
  
  if(true) {
    
    if(coastvalEdge(c) == 1) forCellEx(c2, c)
      if(c2->land != laBarrier && c2->land != laDungeon) {
        c->wall = waLadder;
        c->wparam = 3;
        }
      
    int df = dungeonFlags(c);
    
    if(df&1) {
      int df1 = dungeonFlags(chosenDown(c,1,1));
      int df2 = dungeonFlags(chosenDown(c,-1,-1));
      
      c->wparam = 0;
      if(hrand(100) < (c->landparam % 5 == 0 ? 80 : 20)) {
        if(!(df1&1)) c->wparam = 1;
        if(!(df2&1)) c->wparam = 2;
        }

      if(df&4) 
        placeRandomGate(c);
      else if(c->wparam == 0 && c->landparam % 5 == 0 &&  hrand(100) < 10) {
        c->wall = waLadder;
        c->wparam = 3 + hrand(2);
        }
      else 
        c->wall = waPlatform;
      }

    if(c->wparam) {
      /* int q = 0;
      cell* downs[7];
      forCellEx(c2, c) {
        buildEquidistant(c2);
        if(coastvalEdge(c2) > coastvalEdge(c)) downs[q++] = c2;
        }
      if(q) downs[hrand(q)]->wall = waLadder;
      */
      cell *c2 = 
        c->wparam == 1 ? chosenDown(c, 1, 2) :
        c->wparam == 2 ? chosenDown(c, -1, -2) :
        c->wparam == 3 ? chosenDown(c, 1, 3) :
        c->wparam == 4 ? chosenDown(c, -1, -3) :
        NULL;
      
      if(c2) {
        c2->wall = c->wall, c2->wparam = c->wparam;
        if(c2->wall == waPlatform && hrand(10) < 2) 
          placeRandomGate(c2);
        if(isGate(c2->wall) && hrand(10) < 2) 
          c2->wall = waPlatform;
        }
      }
    }
  else c->wall = waCIsland;
  }

bool is46(int i) { return i == 4 || i == 6; }

void buildDungeonPlates(cell *c) {
  if(c->wall) return;
  int neargate = 0;
  int neargateDown = 0;
  int neargateEq = 0;
  int qup = 0;
  forCellEx(c2, c) {
    int d = coastvalEdge(c2) - coastvalEdge(c);
    if(isGate(c2->wall)) {
      neargate++;
      if(d>0) neargateDown++;
      if(d==0) neargateEq = 0;
      }
    if(d<0) qup++;
    }
  
  if(!neargate) return;
  
  int hr = 99;
  
  if(neargate == neargateDown && qup == 1)
    hr = hrand(12);
  else if((zebra40(c) >= 40) && !(neargateEq && neargateDown)) 
    hr = hrand(36);
  else if(zebra40(c) >= 40)
    hr = hrand(5000);
    
  if(hr < 5) c->wall = waClosePlate;
  else if(hr < 10) c->wall = waOpenPlate;
  }

bool redtrolls(cell *c) {
  return false; /*
  int cd = getCdata(c, 2);
  cd &= 63;
  return cd < 32; */
  }

eMonster pickTroll(cell *c) { 
  if(redtrolls(c))
    return pick(moTroll,moDarkTroll,moRedTroll);
  else
    return pick(moForestTroll, moStormTroll, moFjordTroll);
  }

void dieTroll(cell *c, eMonster m) {
  if(m == moTroll) c->wall = waDeadTroll;
  else if(m == moDarkTroll) c->wall = waDeadfloor2;
  else if(m == moRedTroll) c->wall = waRed1;
  else c->wall = waDeadTroll2, c->wparam = m;
  }

namespace halloween {
  cell *dragoncells[4];
  }

int reptilemax() {
  int i = items[itDodeca] + yendor::hardness();
  if(i >= 245) return 5;
  int r = (250 - i);
  if(shmup::on) r = (r+2) / 3;
  return r;
  }

bool is02(int i) { return i == 0 || i == 2; }

// This function generates all lands. Warning: it's very long!
void setdist(cell *c, int d, cell *from) {

  if(signed(c->mpdist) <= d) return;
  if(c->mpdist > d+1 && d != BARLEV) setdist(c, d+1, from);
  c->mpdist = d;
  // printf("setdist %p %d [%p]\n", c, d, from);

  if(d <= 3) lastexplore = shmup::on ? shmup::curtime : turncount;

  if(buggyGeneration) {
    if(d < BARLEV) for(int i=0; i<c->type; i++) {
      setdist(createMov(c, i), d+(purehepta?2:1), c);
      }
    if(d >= BARLEV) c->item = itBuggy2;
    return;
    }
  
  if(d >= BARLEV) {
  
    if(!c->land && from->land != laElementalWall && from->land != laHauntedWall && from->land != laOceanWall &&
      from->land != laBarrier) {
        if(!hasbardir(c)) setland(c, from->land);
        }
    if(c->land == laTemple && !tactic::on && !chaosmode) setland(c, laRlyeh);
    if(c->land == laMountain && !tactic::on && !chaosmode) setland(c, laJungle);
    if(c->land == laClearing && !tactic::on) setland(c, laOvergrown);
    if(c->land == laWhirlpool && !tactic::on && !yendor::on) setland(c, laOcean);
    if(c->land == laCamelot && !tactic::on) setland(c, laCrossroads);

    if(euclid) setLandEuclid(c);
    if(sphere) setLandSphere(c);
    if(quotient) { setland(c, euclidland); setLandQuotient(c); }
    
    // if(chaosmode) setland(c, getCLand(c));
    }
  
  if(d == BARLEV && c->land == laCanvas)  {
      c->landparam = mapeditor::generateCanvas(c);
      }

  int hard = yendor::hardness();

  if(d >= BARLEV-1 && c->land == laPrairie)
    prairie::spread(c, from);

  if(d < BARLEV && c->land == laPrairie && !c->landparam) {
    printf("d=%d/%d\n", d, BARLEV);
    raiseBuggyGeneration(c, "No landparam set");
    return;
    }
    
  if(d == BARLEV && !euclid && c != cwt.c) 
    buildBigStuff(c, from);
  
  if(d < 10) {
    explore[d]++;
    exploreland[d][c->land]++;
    
    if(d < BARLEV) for(int i=0; i<c->type; i++) {
      setdist(createMov(c, i), d+(purehepta && d>=3 && d<7?2:1), c);
      if(buggyGeneration) return;
      }
    
    if(d == BARLEV-2 && c->land == laOcean) 
      buildEquidistant(c);

    if(d == BARLEV-2 && (c->land == laGraveyard || c->land == laHauntedBorder || c->land == laHaunted))
      buildEquidistant(c);
    
    if(d == 7 && c->land == laPrairie) {
      if(prairie::isriver(c)) {
        if(shmup::on) prairie::beaststogen.push_back(c); 
        else prairie::generateBeast(c);
        }
      else if(!prairie::nearriver(c) && c->LHU.fi.flowerdist > 7) {
        if(hrand(3000) < items[itGreenGrass] - (prairie::isriver(cwt.c) ? 14 : 0))
          c->monst = moGadfly;
        else buildPrizeMirror(c, 1000);
        }
      }

    if(d == 7)
      prairie::generateTreasure(c);
    
    if(d <= 7 && (c->land == laGraveyard || c->land == laHauntedBorder)) {
      c->land = (c->landparam >= 1 && c->landparam <= HAUNTED_RADIUS) ? laHauntedBorder : laGraveyard;
      }

    if(d == 8 && isGravityLand(c->land)) {
      buildEquidistant(c);
      }

    if(d == 8 && (c->land == laIvoryTower || c->land == laDungeon) && !euclid) {

      if(hrand(1000) < 75 && (c->landparam & 1) ) {
        c->landflags = 3;
        }
      else c->landflags = 0;
      }

    if(d == 8 && c->land == laDungeon && !euclid) {

      if(hrand(1000) < 240 && is02(c->landparam%5) ) {
        c->landflags = 3;
        }
      else if(hrand(1000) < 90)
        c->landflags = 1;
      else c->landflags = 0;
      }

    if(d == 7 && c->land == laIvoryTower) buildIvoryTower(c);
    if(d == 8 && c->land == laDungeon) buildDungeon(c);
    if(d == 7 && c->land == laDungeon) buildDungeonPlates(c);
    
    if(d == 9 && c->land == laPalace) {
      if(cdist50(c) == 3 && polarb50(c) == 1)
        c->wall = waPalace;
      }
  
    if(d == 8 && c->land == laPalace && sphere) {
      if(getHemisphere(c,0) == 1)
        c->wall = waPalace;
      if(getHemisphere(c,0) == 3)
        c->wall = purehepta ? waOpenGate : waClosedGate;
      if(getHemisphere(c,0) == 4 && hrand(100) < 40)
        c->wall = waClosePlate;
      if(getHemisphere(c,0) == 6)
        c->wall = waOpenPlate;
      if(getHemisphere(c,0) == -3)
        c->wall = pick(waClosePlate, waOpenPlate);
      if(getHemisphere(c,0) == -6)
        c->wall = waTrapdoor;
      }

    if(d == 8 && c->land == laPalace && !sphere) {
    
      // note: Princess Challenge brings back the normal Palace generation
      bool lookingForPrincess = !euclid && c->master->alt && !princess::challenge;
      
      bool pgate = false;
      if(purehepta) {
        int i = fiftyval049(c);
        if(i >= 8 && i <= 14 && !polarb50(c)) pgate = true;
        }
      
      if(pgate) {
        switch(princess::generating ? 0 : hrand(2)) {
          case 0: 
            c->wall = waClosedGate;
            break;
          case 1:
            c->wall = waOpenGate;
            break;
          }
        for(int s=0; s<7; s++) if(c->mov[s] && 
          (c->mov[s]->wall == waClosedGate || c->mov[s]->wall == waOpenGate))
            c->wall = c->mov[s]->wall;
        }
      else if(cdist50(c) == 3 && polarb50(c) == 1) {
        int q = 0, s = 0;
        if(!ishept(c)) for(int i=0; i<6; i++)
          if(cdist50(c->mov[i]) == 3 && polarb50(c->mov[i]) == 1 && !ishept(c->mov[i]))
            q++, s += i;
        if(q == 1 && c->mov[s]->land == laPalace) {
          switch(princess::generating ? 0 : hrand(2)) {
            case 0: 
              c->wall = waClosedGate;
              c->mov[s]->wall = waClosedGate;
              break;
            case 1:
              c->wall = waOpenGate;
              c->mov[s]->wall = waOpenGate;
              break;
            }
          }
        }
      else if((hrand(100) < (lookingForPrincess ? (purehepta ? 11 : 7) : 5) && cdist50(c)) ||
        (cdist50(c) == 0 && polarb50(c) && hrand(100) < 60)) {
        c->wall = hrand(100) < (lookingForPrincess ? (purehepta ? 25 : 30):50) ? waClosePlate : waOpenPlate;
        }
      else if(hrand(100) < (lookingForPrincess ? 3 : 5))
        c->wall = waTrapdoor;
      
      if(cdist50(c) == 0 && yendor::path) {
        cell *c2 = c->mov[hrand(c->type)];
        if(c2->wall == waNone) c2->wall = waTrapdoor;
        }
      }
  
    if(d==8 && c->land == laEmerald) {
      if(randomPatternsMode)
        c->wall = RANDPAT3(0) ? waCavewall : waCavefloor;
      else if(euclid) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        if(((y-2)&7) < 4) c->wall = waCavewall;
        else c->wall = waCavefloor;
        }
      else if(sphere) {
        if(getHemisphere(c, 0) < 0) 
          c->wall = waCavewall;
        else c->wall = waCavefloor;
        }
      else if(purehepta) {
        if(polarb50(c)) 
          c->wall = waCavewall;
        else c->wall = waCavefloor;
        }
      else {
        int v = emeraldval(c);
        if((v&3) >= 2) 
          c->wall = waCavewall;
        else c->wall = waCavefloor;
        }
      }
    
    if(d == 8 && isIcyLand(c)) c->landparam = 0;
    if(d == 8 && c->land == laDryForest) c->landparam = 0;

    if(d==8 && c->land == laPower) {
      int v;
      if(randomPatternsMode)
        v = RANDPAT ? 24 : 0;
      else if(euclid) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        int y0 = ((short)y) % 6;
        if(y0<0) y0+=6;
        if(y0 == 3 || y0 == 4) v=24; else v=0;
        }
      else v = emeraldval(c);
      v &= ~3;
      if((v == 24 || v == 32 || v == 56))
        c->wall = waEternalFire;
      else if(hrand(100) < 10) {
        c->wall = waGlass;
        eItem protectedItems[18] = {
          itPower, itPower, itPower, itPower, itPower, itPower,
          itOrbLightning, itOrbLightning, itOrbThorns, itOrbThorns,
          itOrbInvis, itOrbInvis,
          itOrbShield, itOrbTeleport, itOrbPsi,
          itOrbDragon, itOrbIllusion, itOrbTime
          };
        c->item = protectedItems[hrand(18)];
        }
      }

    if(d==8 && c->land == laZebra) {
      if(euclid) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        if(y&1) c->wall = waTrapdoor;
        else c->wall = waNone;
        }
      else
      c->wall = (randomPatternsMode ? RANDPAT : (zebra40(c)&2)) ? waTrapdoor : waNone;
      }
    
    if(d==8 && isElemental(c->land)) {
      if(hrand(c->land == laEAir ? 6 : 25) == 0) {
        if(c->land == laEFire) c->wall = waEternalFire;
        else if(c->land == laEWater) c->wall = waSea;
        else if(c->land == laEAir) c->wall = waChasm;
        else if(c->land == laEEarth) c->wall = waStone;
        }
      }
    
    if(d==8 && c->land == laWineyard) {
      if(euclid) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        int dy = ((short)y)%3; if(dy<0) dy += 3;
        if(dy == 1) c->wall = waVinePlant;
        }
      else {
        int v = emeraldval(c);
        int w = v / 4;
        if(randomPatternsMode) c->wall = RANDPAT ? waVinePlant : waNone;
        else if(w == 9 || w == 10 || w == 7 || w == 8) {
          c->wall = waVinePlant;
          }
        else if(v == 24 || v == 58 || v == 26 || v == 56)
          c->wall = waVineHalfA;
        else if(v == 25 || v == 59 || v == 27 || v == 57)
          c->wall = waVineHalfB;
        else c->wall = waNone;
        }
      }
    
    if(d == 7 && cellHalfvine(c)) {
      int i = -1;
      for(int k=0; k<c->type; k++) if(c->mov[k] && c->mov[k]->wall == c->wall)
        i = 0;
      if(i == -1) c->wall = waNone;
      }

    // 24-58
    // 26-56
        
    if(d == 9) {
    
      // mapgen9

      if(isWarped(c->land) && randomPatternsMode)
        setland(c, RANDPAT ? laWarpCoast : laWarpSea);
      
      // if(c->land == laIce && ((celldist(c) % 10) + 10) % 10 == 5)
      //   c->wall = waColumn;
      
      if(c->land == laReptile) {
        int i = zebra40(c);
        if(i < 40) {
          int cd = getCdata(c, 3);
          cd &= 15;
          if(cd >= 4 && cd < 12) c->wall = waChasm;
          else {
            int ch =  hrand(200);
            c->wall = ch < (50 + items[itDodeca] + hard) ? waReptile : 
             waNone;
            }
          c->wparam = 1 + hrand(reptilemax());
          }
        // c->wall = waReptile; c->wparam = 100;
        }

      if(c->land == laBurial) {
        if(hrand(5000) < 25 && celldist(c) >= 5 && !safety) {
          c->item = itBarrow;
          c->landparam = 2 + hrand(2);
          c->wall = waBarrowDig;
          forCellCM(c2, c) c2->wall = waBarrowWall, c2->item = itNone;
          cell *c2 = createMov(c, hrand(c->type));
          c2->wall = waBarrowDig;
          forCellCM(c3, c2) {
            if(c3 == c || isNeighbor(c3, c)) continue;
            bool adj = false;
            forCellEx(c4, c)
              if(c4 != c2 && isNeighborCM(c3, c4)) adj = true;
            if(adj)
              c3->wall = waBarrowDig;
            else
              c3->wall = waBarrowWall, c3->item = itNone;
            }
          }

        /* if(hrand(25000) < PT(25 + 2 * kills[moDraugr], 40) && notDippingFor(itBarrow)) {
          c->item = itBarrow;
          c->wall = waBarrowCenter;
          forCellCM(c2, c) c2->wall = waBarrowCenter, c2->item = itBarrow;
          forCellCM(c2, c) forCellCM(c3, c2) if(!isNeighbor(c3, c) && c3 != c)
            c3->wall = waBarrowWall, c3->item = itNone;
          int i = hrand(c->type);
          cell *c2, *c3;
          while(true) { 
            c2 = createMov(c, i);
            c3 = createMov(c2, hrand(c2->type));
            if(c3 != c && !isNeighbor(c3, c)) break;
            }
          c3->wall = waBarrowCenter;
          forCellCM(c4, c3) {
            if(c4 == c2 || isNeighbor(c4, c2)) continue;
            bool adj = false;
            forCellEx(c5, c2) if(c5 != c3 && isNeighbor(c5, c4)) adj = true;
            if(adj)
              c4->wall = waBarrowCenter;
            else
              c4->wall = waBarrowWall, c4->item = itNone;
            }
          } */
        }

      if(c->land == laTrollheim) {
        if(hrand(50000) < (chaosmode?1000:5) && c->wall != waBarrier && celldist(c) >= 7 && !safety) {
          bool okay = true;
          forCellCM(c2, c) forCellCM(c3, c2) forCellCM(c4, c3) forCellCM(c5, c4) {
            cell *cx = chaosmode ? c3 : c5;
            if(cx->land != laTrollheim && cx->land != laNone)
              okay = false;
            if(cx->bardir != NODIR) okay = false;
            }
          if(okay) {
            forCellCM(c2, c) forCellCM(c3, c2) forCellCM(c4, c3) forCellCM(c5, c4) {
              cell *cx = chaosmode ? c3 : c5;
              cx->bardir = NOBARRIERS;
              setland(cx, laTrollheim);
              }
            c->item = itTrollEgg;
            forCellCM(c2, c) forCellCM(c3, c2) {
              c3->monst = pickTroll(c);
              c2->item = itTrollEgg;
              }
            }
          }
        }

      if(c->land == laIce) {
        if(randomPatternsMode) c->wall = RANDPAT ? waIcewall : waNone;
        else if(hrand(100) < 5 && c->wall != waBarrier) {
          c->wall = waIcewall;
          for(int i=0; i<c->type; i++) if(hrand(100) < 50) {
            cell *c2 = createMov(c, i);
            setdist(c2, d+1, c);
            if(c2->wall == waBarrier || c2->land != laIce) continue;
            c2->wall = waIcewall;
            for(int j=0; j<c2->type; j++) if(hrand(100) < 20) {
              cell *c3 = createMov(c2, j);
              setdist(c->mov[i], d+2, c);
              if(c3->wall == waBarrier || c3->land != laIce) continue;
              c3->wall = waIcewall;
              }
            }
          }
        }

      if(c->land == laIce || c->land == laCocytus) if(c->wall == waIcewall && items[itDiamond] >= 5 && hrand(200) == 1)
        c->wall = waBonfireOff;

      if(c->land == laDragon) {
        int cd = getCdata(c, 3);
        cd &= 31;
        if(cd >= 6 && cd<12) c->wall = waChasm;
        else if(hrand(1000) == 0) c->wall = waEternalFire;
        }
      
      if(c->land == laCaves) 
        c->wall = (randomPatternsMode ? RANDPAT3(1) : hrand(100) < 55) ? waCavewall : waCavefloor;
      
      if(c->land == laCA) 
        c->wall = (hrand(1000000) < ca::prob * 1000000) ? waFloorA : waNone;
      
      if(c->land == laLivefjord) { 
        int die = (randomPatternsMode ? (RANDPAT3(2)?100:0) : hrand(100));
        if(die < 50)
          c->wall = waSea;
        else
          c->wall = waNone;
        }
      
      if(c->land == laDeadCaves) {
        int die = (randomPatternsMode ? (RANDPAT?100:0) : hrand(100));
        if(die<50) c->wall = waDeadwall;
        else if(die<55) c->wall = waDeadfloor2;
        else c->wall = waDeadfloor;
        }
      
      if(c->land == laAlchemist) 
        c->wall = (randomPatternsMode ? RANDPAT : hrand(2)) ? waFloorA : waFloorB;
      
      if(c->land == laDryForest) {
        if(randomPatternsMode)
          c->wall = RANDPAT ? waNone : RANDPATV(laHell) ? waBigTree : waSmallTree;
        else
          c->wall = 
            (hrand(100) < 50) ? (hrand(100) < 50 ? waBigTree : waSmallTree) : waNone;
        }

      if(c->land == laOvergrown) {
        // 0: 60%
        // 10: 50%
        if(randomPatternsMode)
          c->wall = RANDPAT ? waNone : RANDPATV(laWineyard) ? waBigTree : waSmallTree;
        else
          c->wall = 
            (hrand(50+items[itMutant]/2+hard) < 30) ? (hrand(100) < 50 ? waBigTree : waSmallTree) : waNone;
        }

      if(c->land == laHalloween) {
        if(purehepta) {
          int fv = c->master->fiftyval;
          if(fv == 1 || fv == 4 || fv == 2) 
            c->wall = waChasm;
          if(fv == 3) c->item = itTreat;
          }
        else {
          if(c->type == 5) {
            int fv = c->master->fiftyval;
            if(fv == 3 || fv == 4 || fv == 2 || fv == 5) 
              c->wall = waChasm;
            if(fv == 2) halloween::dragoncells[0] = c;
            if(fv == 5) halloween::dragoncells[3] = c;
            if(fv == 1) c->item = itTreat;
            }
          if(c->type == 6 && !euclid) {
            int fvset = 0;
            for(int i=0; i<6; i+=2) fvset |= 1 << createMov(c, i)->master->fiftyval;
            if(fvset == 35 || fvset == 7) c->wall = waChasm;
            if(fvset == 7) halloween::dragoncells[1] = c;
            if(fvset == 35) halloween::dragoncells[2] = c;
            }
          }
        if(quotient && zebra40(c) == 7) {
          c->item = itTreat;
          halloween::dragoncells[0] = NULL;
          }
        if(quotient && zebra40(c) == 5) {
          c->wall = waChasm;
          }
        }

      if(c->land == laWildWest) {
        if(randomPatternsMode)
          c->wall = RANDPAT ? waNone : waSaloon;
        else if(cdist50(c) <= 2) c->wall = waSaloon;
        
        /*if(i == 3 && polarb50(c) == 1) {
          for(int j=0; j<c->type; j++) if(c->mov[j] && polarb50(c->mov[j]) == 0)
            c->wall = waFloorA;
          } */
        }

      if(c->land == laWhirlwind) {
        if(sphere) 
          c->wall = (pseudohept(c) && (c->master->fiftyval == 0 || c->master->fiftyval == 6)) ?
            waFan : waNone;
        else if(!euclid && zebra3(c) == 0) c->wall = waFan;
        else if(pseudohept(c) && hrand(2000) < 150)
          c->wall = waChasm;
        else if(c->type == 6 && buildPrizeMirror(c, 1000))
          {}
        else
          whirlwind::switchTreasure(c);

        /* bool edge = false;
        for(int i=0; i<c->type; i++) 
         if(zebra3(c->mov[i]) != z)
           edge = true; */
        // if(edge && hrand(100) < 5)
        //    c->wall = waSaloon;
        }
      
      if(c->land == laStorms) {
      
        if(euclid) {
          eucoord x, y;
          decodeMaster(c->master, x, y);
          if(short(x+1)%3 == 0 && short(y)%3 == 0) {
            if(hrand(100) < 50)
              c->wall = hrand(2) ? waCharged : waGrounded;
            }
          else if(ishept(c)) {
            bool sand = false;
            for(int i=0; i<c->type; i++) {
              createMov(c, i);
              decodeMaster(c->mov[i]->master, x, y);
              if(short(x+1)%3 == 0 && short(y)%3 == 0) sand = true;
              }
            if(sand && hrand(100) < 20)
              c->wall = waSandstone;
            }
          }
        else if(sphere) { 
          if(c->type != 6) {
            int id = c->master->fiftyval;
            if(id == 1) c->wall = waCharged;
            if(id == (elliptic && !purehepta ? 3 : 9)) c->wall = waGrounded;
            }
          }
        else if(purehepta) {
          int i = zebra40(c);
          if((i == 5 || i == 8) && hrand(100) < 20) c->wall = hrand(2) ? waCharged : waGrounded;
          else if(i == 15) c->wall = waSandstone;
          }
        else {
          int i = zebra40(c);
          if(i >= 40 && hrand(100) < 50) 
            c->wall = hrand(2) ? waCharged : waGrounded;
          else if(ishept(c) && hrand(100) < 20)
            c->wall = waSandstone;
          }
        
        for(int i=0; i<c->type; i++)
          if(c->mov[i] && c->mov[i]->land != laStorms && c->mov[i]->land != laNone)
            c->wall = waNone;
        }
      
      if(c->land == laGraveyard) {
        if(randomPatternsMode)
          c->wall = RANDPAT ? ((RANDPATV(laCrossroads) || RANDPATV(laCrossroads2)) ? waAncientGrave : waFreshGrave) : waNone;
        else if(pseudohept(c)) 
          c->wall = hrand(5) ? waAncientGrave : waFreshGrave;
        }
      
      if(c->land == laRlyeh)  {
        if(randomPatternsMode) {
          c->wall = RANDPAT ? waColumn : waNone;
          }
        else {
          if(hrand(500) < 5) {
            for(int i=0; i<c->type; i++) {
              cell *c2 = createMov(c, i);
              setdist(c2, d+1, c);
              if(c2 && c2->land == laRlyeh)
                c2->wall = waColumn;
              }
            
            for(int j=0; j<2; j++) {
              int i = hrand(c->type);
              if(c->mov[i] && c->mov[i]->land == laRlyeh)
                c->mov[i]->wall = waNone;
              }
            }
          if(pseudohept(c) && hrand(2)) c->wall = waColumn;
          }
        }
      
      if(c->land == laHell) {
        if(hrand(1000) < 36 && celldist(c) >= 3) {
          for(int i=0; i<c->type; i++) {
            cell *c2 = createMov(c, i);
            setdist(c2, d+1, c);
            if(c2 && c2->land == laHell)
              if(c2->wall != waSulphurC)
                c2->wall = waSulphur;
            }
          
          c->wall = waSulphurC;
          }
        }
      
      if(c->land == laCocytus)  {
        if(c->wall == waNone) c->wall = waFrozenLake;
        if(hrand(100) < 5 && !safety && celldist(c) >= 3) {
          for(int i=0; i<c->type; i++) {
            cell *c2 = createMov(c, i);
            setdist(c2, d+1, c);
            if(c2 && c2->land == laCocytus)
              c2->wall = waLake;
            }
          
          c->wall = waLake;

          if(hrand(500) < 100 + 2 * (items[itSapphire] + hard))
            c->monst = moShark;
          }
        }

      if(isHive(c->land) && hrand(2000) < (chaosmode ? 1000 : purehepta?200:2) && !safety) 
        hive::createBugArmy(c);
      
      // landbigstuff
      
      if(c->land == laPalace && !euclid && c->master->alt) {
        int d = celldistAlt(c);
        if(d <= 150) generateAlts(c->master);
        }
      
      if((bearsCamelot(c->land) && !euclid && !quotient) || c->land == laCamelot) 
      if(euclid || c->master->alt) {
        int d = celldistAltRelative(c);
        if(tactic::on || (d <= 14 && roundTableRadius(c) > 20)) {
          if(!euclid) generateAlts(c->master);
          preventbarriers(c);
          if(d == 10) {
            if(pseudohept(c)) buildCamelotWall(c);
            else {
              if(!euclid) for(int i=0; i<7; i++) generateAlts(c->master->move[i]);
              int q = 0;
              for(int t=0; t<6; t++) {
                createMov(c, t);
                if(celldistAltRelative(c->mov[t]) == 10 && !pseudohept(c->mov[t])) q++;
                }
              if(q == 1) buildCamelotWall(c);
              // towers of Camelot
              if(q == 0 && !purehepta) {
                c->monst = moKnight;
                c->wall = waTower;
                forCellEx(c2, c) {
                  int cr = celldistAltRelative(c2);
                  if(cr == 9) ;
                  else {
                    buildCamelotWall(c2);
                    if(c2->type == 6)
                      c2->wall = waTower, c2->wparam = 1;
                    }
                  }
                for(int i=0; i<c->type; i++) if(celldistAltRelative(c->mov[i]) < d)
                  c->mondir = i;
                }
              }
            }
          if(d == 0) c->wall = waRoundTable;
          if(celldistAlt(c) == 0 && !tactic::on) c->item = itHolyGrail;
          if(d < 0 && hrand(7000) <= 10 + items[itHolyGrail] * 5) {
            eMonster m[3] = { moHedge, moLancer, moFlailer };
            c->monst = m[hrand(3)];
            }
          if(d == 1) {
            // roughly as many knights as table cells
            if(hrand(purehepta ? 2618 : 1720) < 1000) 
              c->monst = moKnight;
            if(!euclid) for(int i=0; i<7; i++) generateAlts(c->master->move[i]);
            for(int i=0; i<c->type; i++) 
              if(c->mov[i] && celldistAltRelative(c->mov[i]) < d)
                c->mondir = (i+3) % 6;
            }
          if(tactic::on && d >= 2 && d <= 8 && hrand(1000) < 10)
            c->item = itOrbSafety;
          if(d == 5 && tactic::on)
            c->item = itGreenStone;
          if(d <= 10) c->land = laCamelot;
          if(d > 10 && !euclid && !tactic::on) {
            setland(c, eLand(c->master->alt->alt->fiftyval));
            if(c->land == laNone) printf("Camelot\n"); // NONEDEBUG
            }
          }
        }
      
      if((c->land == laStorms && !euclid)) {
        if(c->master->alt && c->master->alt->distance <= 2) {
          generateAlts(c->master);
          preventbarriers(c);
          int d = celldistAlt(c);
          if(d <= -2) {
            c->wall = (c->master->alt->alt->emeraldval & 1) ? waCharged : waGrounded;
            c->item = itNone;
            c->monst = moNone;
            }
          else if(d <= -1)
            c->wall = (hrand(100) < 20) ? waSandstone : waNone;
          else if(d <= 0)
            c->wall = waNone;
          }
        }
      
      if(chaosmode && c->land == laTemple) {
        for(int i=0; i<c->type; i++)
          if(pseudohept(c) && c->mov[i] && c->mov[i]->land != laTemple)
            c->wall = waColumn;
        }
      
      else if((c->land == laRlyeh && !euclid) || c->land == laTemple) {
        if(euclid || (c->master->alt && (tactic::on || c->master->alt->distance <= 2))) {
          if(!euclid && !chaosmode) generateAlts(c->master);
          preventbarriers(c);
          int d = celldistAlt(c);
          if(d <= 0) {
            c->land = laTemple, c->wall = waNone, c->monst = moNone, c->item = itNone;
            }
          if(d % TEMPLE_EACH==0) {
            if(pseudohept(c)) 
              c->wall = waColumn;
            else {
              if(!euclid) for(int i=0; i<7; i++) generateAlts(c->master->move[i]);
              int q = 0;
              for(int t=0; t<6; t++) {
                createMov(c, t);
                if(celldistAlt(c->mov[t]) % TEMPLE_EACH == 0 && !ishept(c->mov[t])) q++;
                }
              if(q == 2) c->wall = waColumn;
              }
            }
          }
        }

     if((c->land == laOvergrown && !euclid) || c->land == laClearing) {
        if(euclid || (c->master->alt && (tactic::on || c->master->alt->distance <= 2))) {
          if(!euclid) generateAlts(c->master);
          preventbarriers(c);
          int d = celldistAlt(c);
          if(d <= 0) {
            c->land = laClearing, c->wall = waNone; // , c->monst = moNone, c->item = itNone;
            }
          else if(d == 1 && !tactic::on && !euclid)
            c->wall = waSmallTree, c->monst = moNone, c->item = itNone;
          }
        }

     if((c->land == laJungle && !euclid) || c->land == laMountain) {
        if(euclid || (c->master->alt && (tactic::on || c->master->alt->distance <= 2))) {
          if(!euclid) generateAlts(c->master);
          preventbarriers(c);
          int d = celldistAlt(c);
          if(d <= 0 || (firstland == laMountain && tactic::on)) {
            c->land = laMountain, c->wall = waNone; // , c->monst = moNone, c->item = itNone;
            }
          }
        }

      if(c->land == laOcean || c->land == laWhirlpool) {
        bool fullwhirlpool = false;
        if(tactic::on && tactic::lasttactic == laWhirlpool)
          fullwhirlpool = true;
        if(yendor::on && yendor::clev().l == laWhirlpool)
          fullwhirlpool = true;
        if(euclid || (c->master->alt && (fullwhirlpool || c->master->alt->distance <= 2))) {
          if(!euclid) generateAlts(c->master);
          preventbarriers(c);
          int dd = celldistAlt(c);
          if(dd <= 0 || fullwhirlpool) {
            c->land = laWhirlpool, c->wall = waSea, c->monst = moNone, c->item = itNone;
            }
          }
        }
      
      if(c->land == laKraken) {
        c->wall = waSea;
        }

      if(c->land == laCaribbean) {
        if(!euclid) {
          if(c->master->alt && c->master->alt->distance <= 2) {
            if(!euclid) generateAlts(c->master);
            preventbarriers(c);
            int d = celldistAlt(c);
            if(d <= 0) 
              // c->wall = waChasm;
              generateTreasureIsland(c);
            else
              c->wall = waSea;
            }
          else c->wall = waSea;
          }
        else {
          int d = celldistAlt(c);
          if(d <= 0) 
            generateTreasureIsland(c);
          else
            c->wall = waSea;
          }
        }
      
      if(c->land == laTortoise) {
        using namespace tortoise;
        int f = hrand(30);
        if(f < 21 && ((getBits(c)>>f) & 1)) {
          tflag tf = tflag(f);
          // long neck and saddleback shell help with big trees
          #define PSH(x,y) if(tf == x || tf == y) tf = pseudohept(c) ? x : y;
          PSH(tfLongNeck, tfShell)
          if(tf == tfLongNeck) c->wall = waBigTree;
          if(tf == tfShell) c->wall = waBigTree;
          // brown body elements help with (small) trees
          PSH(tfShellHue, tfScuteHue)
          if(tf == tfShellHue) c->wall = waSmallTree;
          if(tf == tfScuteHue) c->wall = waSmallTree;
          if(tf == tfSkinHue) c->wall = waSmallTree;
          // long legs help with rocks
          PSH(tfFront, tfRear)
          if(tf == tfFront) c->wall = waStone;
          if(tf == tfRear) c->wall = waStone;
          // broad tail and desaturated shell helps with water
          PSH(tfTail, tfShellSat)
          if(tf == tfTail) c->wall = waSea;
          if(tf == tfShellSat) c->wall = waSea;
          #undef PSH
          }
        }

      if(isHive(c->land) && hrand(2000) < 100 && !c->wall && !c->item && !c->monst) {
        int nww = 0;
        for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->wall == waWaxWall)
          nww++;
        if(nww == 0) {
          c->wall = waWaxWall;
          c->monst = moNone;
          c->landparam = hrand(0xFFFFFF + 1);
          }
        /* for(int i=0; i<c->type; i++) {
          if(hrand(6) < 5) {
            createMov(c,i);
            cell *c2 = c->mov[i];
            c2->wall = waWaxWall;
            c2->monst = moNone;
            }
          } */
        }

      if(c->land == laDesert) {
        if(randomPatternsMode)
          c->wall = RANDPAT ? waDune : waNone;
        else {
          if(hrand(100) < 5) {
            for(int i=0; i<c->type; i++) {
              cell *c2 = createMov(c, i);
              setdist(c2, d+1, c);
              if(c2 && c2->land == laDesert)
                c2->wall = waDune;
              }
            
            for(int j=0; j<2; j++) {
              int i = hrand(c->type);
              if(c->mov[i] && c->mov[i]->land == laDesert)
                c->mov[i]->wall = waNone;
              }
            }
          }
  
        if(hrand(300) == 1 && items[itSpice] >= 5) c->wall = waThumperOff;
        }

      if(c->land == laRedRock) {
        if(randomPatternsMode) {
          c->wall = waNone;
          if(!ishept(c)) { if(RANDPAT) buildRedWall(c, 20); }
          else {
            int k = 0;
            for(int i=0; i<20; i++) 
              if(RANDPATC(c->mov[i%7]) && !RANDPATC(c->mov[(i+1)%7]))
                k++;
            if(k>=4) buildRedWall(c, 20);
            }
          }
        else if(ishept(c) && hrand(100) < 8 && !c->monst) {
          buildRedWall(c, 80);
          int i = hrand(7);
          buildRedWall(createMovR(c, i), 33);
          if(hrand(2) == 0) 
            buildRedWall(createMovR(createMovR(c, i), c->spn(i)+(hrand(2)?2:4)), 20);
          i += 3 + hrand(2);
          if(hrand(6) < 4)
            buildRedWall(createMovR(c, i), 33);
          if(hrand(2) == 0) 
            buildRedWall(createMovR(createMovR(c, i), c->spn(i)+(hrand(2)?2:4)), 20);
          }
        }
      }

     if(d == 7 && c->land == laReptile && c->wall != waChasm) {
       bool nonchasm = false;
       forCellEx(c2, c) if(c2->wall != waChasm) nonchasm = true;
       if(!nonchasm) c->item = itDodeca;
       }
        

     if(d == 7 && c->land == laKraken && c->wall == waSea && !c->item && !c->monst && !safety) {
       if(hrand(2000) < 3) {
         c->wall = waBoat;
         c->item = itOrbFish;
         c->monst = moViking;
         }
       else if(hrand(2000) < 16) c->item = itOrbFish;
       else if(hrand(500) < kills[moKrakenH]) c->item = itKraken;
       else placeLocalOrbs(c);
       }
      
    if(d == 7 && c->land == laTrollheim && !c->monst && !c->item && celldist(c) >= 3) {
      int cd = getCdata(c, 3);
      cd %= 16;
      if(cd<0) cd += 16;
      if(cd >= 4 && cd < 10 && hrand(100) < 40)
        dieTroll(c, pickTroll(c));
      }

    if(d == 8 && c->land == laBull && !c->monst && !c->item && celldist(c) >= 3) {
      /* int cd = getCdata(c, 3);
      cd &= 15;
      int ce = getCdata(c, 2);
      ce &= 15;
      if(cd >= 8 && ce >= 8) */
      if(hrand(100) < 25)
        c->wall = safety ? pick(waBigTree, waSmallTree) : pick(waStone, waBigTree, waSmallTree);
      }
    
    if(d == 7 && c->land == laBull && c->wall == waNone && !safety) {
      if(hrand(1000) < 20) c->monst = moSleepBull;
      else if(hrand(2500) < items[itBull] + hard - 10) c->monst = moGadfly;
      else if(hrand(1000) < 50) {
        int nearwall = 0;
        int walls[8];
        forCellIdEx(c2, i, c) if(c2->wall) walls[nearwall++] = i;
        if(nearwall && nearwall < c->type) {
          c->monst = moButterfly;
          c->mondir = walls[hrand(nearwall)];
          }
        }
      }

    if(d == 8) {

      if(c->land == laKraken && !pseudohept(c) && hrand(20000) < 10 + 3 * items[itKraken] + 2 * hard && c->wall == waSea && !c->item && !c->monst && !safety) {
        bool ok = true;
        forCellEx(c2, c)
          if(c2->wall != waSea || c2->item || c2->monst) 
            ok = false;
          
        if(ok) {
          c->monst = moKrakenH;
          forCellIdEx(c2, i, c) {
            c2->monst = moKrakenT;
            c2->hitpoints = 1;
            c2->mondir = c->spn(i);
            }
          playSound(c, "seen-kraken");
          }
        }

      if(c->land == laWarpCoast) {
        if(hrand(1000) < 150 && celldist(c) >= 3 && !pseudohept(c)) 
          c->wall = waSmallTree;
        int q = 0;
        if(!purehepta && !chaosmode) for(int i=0; i<c->type; i++) 
          if(c->mov[i] && !isWarped(c->mov[i]->land) && c->mov[i]->land != laCrossroads4) 
            q++;
        if(q == 1) c->wall = waWarpGate;
        }
      
      if(c->land == laWarpSea) {
        c->wall = waSea;
        int q = 0;
        if(!purehepta && !chaosmode) for(int i=0; i<c->type; i++) 
          if(c->mov[i] && !isWarped(c->mov[i]->land)) q++;
        if(q == 1) c->wall = waWarpGate;
        }      
      }

    if(d == 8 && c->land == laKraken && pseudohept(c) && hrand(1000) <= 2) {
      c->wall = waNone;
      forCellEx(c2, c) c2->wall = waNone;
      }

    if(d == 8 && c->land == laOvergrown) {
      if(hrand(15000) < 20 + (2 * items[itMutant] + hard) && !safety) {
        c->item = itMutant;
        c->landparam = items[itMutant] + 5 + hrand(11);
        c->wall = waNone;
        for(int i=0; i<c->type; i++) 
          if(c->mov[i] && (c->mov[i]->wall == waBigTree || c->mov[i]->wall == waSmallTree)) 
            c->mov[i]->wall = waNone;
        }
      else if(hrand(15000) < 20 + (2 * items[itMutant] + hard) && !safety) {
        // for the Yendor Challenge, use only Mutants
        if(!(yendor::on && yendor::clev().l == laMirror)) {
          c->monst = moForestTroll;
          c->wall = waNone;
          }
        }
      }
    
    if(d == 7 && c->item == itMutant) {
      if(c->monst != moMutant && isMultitile(c->monst))
        c->item = itNone;
      else {
        c->stuntime = mutantphase;
        c->monst = moMutant;
        c->mondir = NODIR;
        }
      }

    if(d == 8 && c->land == laCaribbean && !euclid) {
      int mindist  = 9;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if((euclid || c2->master->alt) && celldistAlt(c2) < mindist)
          mindist = celldistAlt(c2);
        }
      if(mindist == 0) beCIsland(c);
      }
                                       
    if(d == 7 && isHaunted(c->land)) {
      c->wall = waNone;

      c->item = itNone; c->monst = moNone;
      
      if(hrand(100) < 25)
        c->wall = hrand(2) ? waBigTree : waSmallTree;
        
      else if(hrand(500) < 2 && ishept(c))
        c->wall = hrand(10) < 3 ? waFreshGrave : waAncientGrave;
        
      else {
        if(hrand(5000) < 30)
          c->item = itGreenStone;
  
        if(hrand(4000) < 10 + items[itLotus] + hard && !safety) 
          c->monst = moGhost;
          
        int depth = getHauntedDepth(c);
        
        if(hrand(500 + depth) < depth - items[itLotus] && !safety)
          c->item = itLotus;
        }
      }
    
    if(d == 7 && c->land == laRedRock && c->wall == waNone)
      buildPrizeMirror(c, 1000);
    
    if(d == 8 && c->land == laRose && hrand(2000) < 100 && !c->wall && !c->item && !c->monst) {
      int nww = 0;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];          
        if(c2 && c2->wall == waRose) nww++;
        for(int j=0; j<c2->type; j++) {
          cell *c3 = c2->mov[j];
          // note: c3->land is required for Android --
          // not strictly equivalent since another land there might be not yet generated
          if(c3 && c3->land != laRose && c3->land) nww++;
          }
        };

      if(nww == 0) {
        c->wall = waRose;
        c->monst = moNone;
        }
      }
    
    if(d == 7 && c->land == laCaribbean && c->wall == waSea) {
      bool coast = false;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if(passable(c2, c, 0) && c2->wall != waBoat) coast = true;
        }
      if(coast && hrand(10) < 5) {
        c->wall = waBoat;
        if(items[itPirate] >= 10 && hrand(100) < 2 && !safety)
          c->item = itOrbTime;
        else if(hrand(100) < 2*PRIZEMUL && !safety) 
          placePrizeOrb(c);
        }
      }

    if(d == 7 && c->land == laCaves && c->wall == waCavewall && hrand(5000) < items[itGold] + hard && !safety)
      c->monst = moSeep;
    
    if(d == 7 && c->land == laLivefjord && c->wall == waSea && hrand(5000) < 15 + items[itFjord] + hard && !safety) {
      if(items[itFjord] >= 5 && hrand(100) < 20)
        c->monst = moWaterElemental;
      else {
        c->monst = moViking;
        c->wall = waBoat;
        }
      }
    
    else if(d == 7 && c->land == laLivefjord && yendor::path && c->wall == waSea && hrand(5000) < 20) {
      c->monst = moViking;
      c->wall = waBoat;
      }
    
    if(d == 7 && c->land == laLivefjord && hrand(2000) < PT(50 + kills[moViking], 100) && notDippingFor(itFjord) && !safety) {
      c->item = itFjord;
      }
    
    if(d == 7 && c->land == laLivefjord && items[itFjord] >= 10 && hrand(2000) < 2)
      c->item = itOrbFish;
    
    if(d == 7 && c->land == laLivefjord && hrand(2000) < 2*PRIZEMUL)
      placePrizeOrb(c);
    
    if(d == 7 && c->land == laLivefjord)
      buildPrizeMirror(c, 1000);
    
    if(d == 7 && c->land == laEmerald && c->wall == waCavewall && hrand(5000) < items[itEmerald] + hard && !safety)
      c->monst = moSeep;
    
    if(d == 7 && c->land == laDeadCaves && c->wall == waDeadwall && hrand(1000) < items[itSilver] + hard && !safety)
      c->monst = moSeep;
    
    if(d == 7 && c->wall == waVinePlant && hrand(100) < (randomPatternsMode ? 2 : 10))
      c->monst = moVineSpirit;

    if(d == 7 && c->land == laOcean && !safety) {
      bool placecolumn = false;
      if(c->landparam % TEMPLE_EACH == 0 && c->landparam <= 24) {
        int q = 0;
        forCellEx(c2, c)
          if(c2->landparam % TEMPLE_EACH == 0 && !ishept(c2)) q++;
        placecolumn = q == 2;
        if(placecolumn) {
          placecolumn = false;
          cell *c2 = c;
          seek:
          forCellEx(c3, c2) if(c3->landparam < c2->landparam) {
            c2 = c3; goto seek;
            }
          forCellEx(c3, c2) forCellEx(c4, c3)
            if(c4->barleft == laRlyeh || c4->barright == laRlyeh || c4->land == laRlyeh)
              placecolumn = true;
          }
        }

      if(placecolumn)
        c->wall = waColumn;
      else if((c->landparam >= 1 && c->landparam <= 25) || chaosmode) {
        if(hrand(1000) < 5)
          c->wall = waBoat;
        if(hrand(1000) < PT(50 + kills[moAlbatross]/2, 150))
          c->item = itCoast;
        if(hrand(15000) < 10 + 2 * items[itCoast] + 2 * hard)
          c->monst = moAlbatross;
        if(items[itCoast] >= 10 && hrand(10000) < 5)
          c->item = itOrbAir;
        else if(items[itCoast] >= 10 && hrand(10000) < 6)
          c->item = itOrbEmpathy;
        if(hrand(10000) < 5*PRIZEMUL)
          placePrizeOrb(c);
        buildPrizeMirror(c, 2000);
        }
      else if(c->landparam > 25) {
        int amberbonus = items[itCoast] - 50;
        // 50 => 10
        if(hrand(15000) < 10 + amberbonus + 2 * hard)
          c->monst = moAlbatross;
        if(hrand(30000) < 20 + 2 * hard) {
          c->wall = waBoat;
          c->monst = moPirate;
          // orbs are possible!
          placeOceanOrbs(c);
          }
        else if(hrand(30000) < 10 + 2 * hard)
          c->monst = moCShark;
        }
      }

    if(d == 7 && c->land == laWarpSea && c->wall == waSea && !safety) {
      if(sphere && c->type != 6 && c->master->fiftyval == 6) {
        c->wall = waBoat;
        c->monst = moRatling;
        }
      if(hrand(12000) < 30 + 2 * items[itCoral] + hard) {
        c->wall = waBoat;
        c->monst = moRatling;
        }
      else if(hrand(6000) < PT(120 + min(kills[moRatling], 250), 240) && notDippingFor(itCoral)) {
        c->wall = waBoat;
        c->item = itCoral;
        }
      }

    if(d == 7 && c->land == laCaribbean && c->wall == waSea && hrand(10000) < 20 + items[itPirate] + 2 * hard && !safety)
      c->monst = moCShark;
    
    if(d == 7 && c->wall == waCTree && hrand(5000) < 100 + items[itPirate] + hard)
      c->monst = moParrot;
    
    // repair the buggy walls flowing in from another land, like ice walls flowing into the Caves
    if(d == 7 && c->land == laCaves && c->wall != waCavewall && c->wall != waCavefloor)
      c->wall = waCavefloor;
    
    if(d == 7 && c->land == laDeadCaves && c->wall != waDeadwall && 
      c->wall != waDeadfloor && c->wall != waDeadfloor2 && c->wall != waEarthD)
      c->wall = waDeadfloor2;
    
    if(d == 7 && c->land == laCocytus && c->wall != waFrozenLake && c->wall != waLake && c->wall != waIcewall)
      c->wall = waFrozenLake;
    
    if(d == 7 && c->land == laAlchemist && c->wall != waFloorA && c->wall != waFloorB)
      c->wall = waFloorA;
    
    if(d == 3 && c->land == laMinefield && safety && (c->wall == waMineMine || c->wall == waMineUnknown))
      c->wall = waMineOpen;

    if(c->land == laCamelot && tactic::on && d == 0) {
      int r = roundTableRadius(c);
      if(r > 28 + 3 * items[itHolyGrail])
        items[itHolyGrail]++;
      }
    
    if(d == 7 && c->land == laMinefield) {
      c->wall = waMineUnknown;
      // 250: rare mines
      // 1250: at 25
      int minefreq = 0;
      int treas = items[itBombEgg];
      // if(treas <= 10) minefreq = 250 + 30 * treas;
      if(treas <= 110) minefreq = 550 + 10 * (treas-10);
      else minefreq = 1550 + (treas - 110);
      
      // experimentation says that 600 is slightly too hard to find the Orb of Yendor
      if(yendor::generating || yendor::path)
        if(minefreq < 550)
          minefreq = 550;
      
      int tfreq = 
        treas < 10 ? 50 + 5 * treas :
        treas < 25 ? 100  + (treas-10) * 2:
        treas < 50 ? 150 + (treas-25) :
        175;

      if(hrand(5000) < minefreq)
        c->wall = waMineMine;
      else if(hrand(5000) < tfreq && !safety) {
        c->item = itBombEgg;
        c->landparam = items[itBombEgg] + 5 + hrand(11);
        }
      else if(hrand(5000) < treas - 20 + yendor::hardness() && !safety)
        c->monst = moBomberbird;
      else if(treas >= 10 && hrand(5000) < 10 && !safety)
        c->item = itOrbFriend;
      else if(hrand(5000) < 10*PRIZEMUL && !safety)
        placePrizeOrb(c);
      }

    if(d == 7 && c->wall == waIcewall && c->land != laIce && c->land != laCocytus)
      c->wall = waNone;
    
    if(d == 7 && c->wall == waRed3 && c->land != laRedRock)
      c->wall = waNone;
    
    if(d == 7 && c->item == itRedGem && c->land != laRedRock)
      c->item = itNone;
    
    if(d == 7 && c->item == itRose && c->land != laRose)
      c->item = itNone;
    
    if(d == 7 && c->wall == waChasmD) {
      chasmify(c);
      }
      
    if(d == 7 && c->wall == waEarthD) {
      chasmifyEarth(c);
      }
    
    if(d == 7 && c->wall == waElementalD) {
      chasmifyElemental(c);
      }
    
    // seal entrances to the Land of Power.
    if(d == 7 && c->land == laPower && c->type == 7) {
      bool onwall = false;
      for(int i=0; i<7; i++) if(c->mov[i] && c->mov[i]->land == laBarrier)
        onwall = true;
      if(!onwall) for(int i=0; i<7; i++) {
        cell *c2 = c->mov[i];
        if(!c2) continue;
        cell *c3 = c2->mov[(c->spn(i) + 3) % 6];
        if(c3->land != laPower && c3->land != laBarrier)
        if(c2->wall != waFire && c2->wall != waGlass) {
          if(isFire(c)) c->monst = moWitchWinter;
          else if(c->wall == waGlass) c->monst = moWitchGhost;
          else c->monst = moEvilGolem;
          }
        }
      }

    if(d == 7 && passable(c, NULL, 0)) {
      if(c->land == laBarrier) c->wall = waBarrier;
      if(c->land == laOceanWall) 
        c->wall = c->type == 7 ? waBarrier : waSea;
      }

    if(d == 7 && c->land == laWhirlpool) 
      whirlpool::generate(c);
    
    if(c->land == laPalace && princess::generating) {
      // no Opening Plates nearby
      if(d <= 7 && c->wall == waOpenPlate && !purehepta)
        c->wall = waNone;
      if(d <= 7 && c->wall == waClosePlate && purehepta)
        c->wall = waOpenPlate;
      // no monsters nearby
      if(d>0) c->monst = moNone;
      // no Plates or Trapdoors in the Princess cell
      if(d < 3 && (c->wall == waClosePlate || c->wall == waOpenPlate || c->wall == waTrapdoor))
        c->wall = waNone;
      if(d > 1) c->item = itNone;
      // the Princess herself
      if(d == 0) {
        c->monst = moPrincess;
        c->hitpoints = palaceHP();
        c->wall = waGiantRug;
        cell *c2 = NULL;
        for(int i=0; i<c->type; i++) {
          cellwalker cw(c, i);
          cwstep(cw);
          cwspin(cw, 4);
          cwstep(cw);
          cwspin(cw, 2);
          cwstep(cw);
          cwspin(cw, 4);
          cwstep(cw);
          cwspin(cw, 2 + hrand(3));
          cwstep(cw);
          if(!c2) c2 = cw.c;
          else if(celldist(cw.c) > celldist(c2)) c2 = cw.c;
          cw.c->monst = moMouse;
          }
        c2->wall = waOpenPlate;
        }
          
      }
    
    if(d == 7 && passable(c, NULL, 0) && !safety) {
      
      if(c->land == laIvoryTower) {
        if(hrand(20000) < 20 + items[itIvory] + hard) {
          if(cellEdgeUnstable(c))
            c->monst = moGargoyle;
          else
            c->monst = moFamiliar;
          }
        else if(c->landparam >= 14 && hrand(2000) < PT(50+kills[moGargoyle]+kills[moFamiliar], 150) && !cellEdgeUnstable(c) ) {
          c->item = itIvory;
          }
        }
      
      if(c->land == laDungeon) {
        int lp = c->landparam * c->landparam;
        if(lp > 100) lp = 100;
        if(c->landparam >= 10 && hrand(20000) < 5*lp + items[itSlime] + hard && !cellEdgeUnstable(c)) {
          c->monst = moSkeleton, c->hitpoints = 3;
          }
        else if(c->landparam >= 10 && hrand(50000) < lp/2 + items[itSlime] + hard) {
          c->monst = moGhost;
          }
        else if(c->landparam >= 10 && hrand(50000) < 3*(100-lp) + 80 + items[itSlime]/3 + hard) {
          c->monst = moBat;
          }
        else if(c->landparam >= 15 && hrand(800) < min(PT(150 + 2 * kills[moBat], 250), 250) && !cellEdgeUnstable(c) && c->wall != waOpenGate) {
          c->item = itSlime;
          }
        }
      
      if(c->land == laEndorian) {
        if(c->wall == waNone && coastval(c, laEndorian) >= 10 && hrand(5000) < 10 + 2 * (items[itApple] + hard))
          c->monst = moSparrowhawk;
        else if(c->wall != waNone && hrand(5000) < 10 + 2 * (items[itApple] + hard))
          c->monst = moResearcher;
        else if(c->wall == waCanopy && !checkInTree(c, 3) && hrand(5000) < PT(300 + 5 * (kills[moSparrowhawk] + kills[moResearcher]), 750))
          c->item = itApple;
        }
    
      if(c->land == laIce) {
        if(hrand(5000) < PT(100 + 2 * (kills[moYeti] + kills[moWolf]), 200) && notDippingFor(itDiamond))
          c->item = itDiamond;
        if(hrand(8000) < 2 * (items[itDiamond] + hard))
          c->monst = hrand(2) ? moYeti : moWolf;
        }

      if(c->land == laTrollheim && !safety) {
        if(hrand(8000) < items[itTrollEgg] + hardness_empty())
          c->monst = pickTroll(c);
        }

      if(c->land == laTortoise) {
        if(hrand(4000) < 50 + items[itBabyTortoise]*2 + hard * 6 && !safety) {
          c->monst = moTortoise;
          c->hitpoints = 3;
          }
        
        int chance = 50 + items[itBabyTortoise]*2;
        if(quickfind(laTortoise)) chance += 150;
        if((tactic::on || euclid) && hrand(4000) < chance && !safety) {
          c->item = itBabyTortoise;
          tortoise::babymap[c] = getBits(c) ^ tortoise::getRandomBits();
          }
        }

      if(c->land == laPalace) {
        bool lookingForPrincess0 = !euclid && c->master->alt;
        bool lookingForPrincess = lookingForPrincess0 && !princess::challenge;
        int hardness = lookingForPrincess ? 5 : items[itPalace] + hard;
        
        if(hrand(5000) < PT(100 + 2 * (kills[moPalace] + kills[moFatGuard] + kills[moVizier] + kills[moSkeleton]), 200) && notDippingFor(itPalace) &&
          c->wall != waOpenGate && !lookingForPrincess0)
          c->item = itPalace;
        if(items[itPalace] >= 10 && hrand(5000) < 16 && c->wall != waOpenGate)
          c->item = hrand(100) < 80 ? itOrbFrog : itOrbDiscord;
        if(hrand(5000) < 20*PRIZEMUL && c->wall != waOpenGate)
          placePrizeOrb(c);
        if(c->wall == waNone) buildPrizeMirror(c, 250);
        if(c->land == laPalace && (euclid || c->master->alt) && celldistAlt(c) <= 150 && !(havewhat&HF_MOUSE) && !princess::generating &&
          princess::getPrisonInfo(c) &&
          (euclid || (princess::getPrisonInfo(c)->bestdist < 6 && princess::getPrisonInfo(c)->princess))) {
          c->monst = moMouse;
          addMessage(XLAT("You hear a distant squeak!"));
          playSound(c, "mousesqueak");
          drawBigFlash(c);
/*          {
  cell *c2= c;          
  z:
  c2->item = itPirate;
  printf("AT %p\n", c2);
  for(int i=0; i<c2->type; i++) if(c2->mov[i] && c2->mov[i]->mpdist < c2->mpdist) {
    c2 = c2->mov[i];
    goto z;
    } 
          } */
          havewhat |= HF_MOUSE;
          }
        else if(hrand(15000) < 10 + hardness) {
          c->monst = moPalace;
          c->hitpoints = palaceHP();
          if(hrand(10 + items[itPalace] + yendor::hardness()) >= 14 && !lookingForPrincess)
            c->monst = moSkeleton;
          }
        else if(hrand(20000) < hardness) {
          c->monst = moFatGuard;
          c->hitpoints = palaceHP();
          }
        else if(hrand(20000) < hardness - 7) {
          c->monst = moVizier;
          c->hitpoints = palaceHP();
          }
        else if(princess::forceVizier && from->pathdist != INF) {
          c->monst = moVizier;
          c->hitpoints = palaceHP();
          princess::forceVizier = false;
          }
        }
      if(c->land == laCaves) {
        if(hrand(5000) < PT(100 + 2 * min(kills[moTroll] + kills[moGoblin], 150), 200) && notDippingFor(itGold))
          c->item = itGold;
        if(hrand(8000) < 10 + 2 * (items[itGold] + hard))
          c->monst = hrand(2) ? moTroll : moGoblin;
        }
      if(c->land == laLivefjord) {
        if(hrand(16000) < 10 + 2 * (items[itFjord] + hard) + (yendor::path ? 90:0))
          c->monst = moFjordTroll;
        }
      if(c->land == laDeadCaves) {
        if(hrand(5000) < PT(100 + 2 * (kills[moDarkTroll] + kills[moEarthElemental]), 200) && notDippingFor(itSilver))
          c->item = itSilver;
        if(hrand(16000) < (items[itSilver] + hard)) {
          c->monst = moEarthElemental;
          for(int i=0; i<c->type; i++) {
            cell *c2 = c->mov[i];
            earthFloor(c2);
            }
          for(int i=0; i<c->type; i++) if(c->mov[i]->mpdist < c->mpdist) c->mondir = i;
          chasmifyEarth(c); c->wall = waDeadfloor2;
          }
        else if(hrand(8000) < 60 + 8 * (items[itSilver] + hard)) {
          if(hrand(100) < 25) {
            }
          else c->monst = hrand(2) ? moDarkTroll : moGoblin;
          }
        }
      if(c->land == laDesert) {
        if(hrand(5000) < PT(100 + 2 * (kills[moWorm] + kills[moDesertman]), 200) && notDippingFor(itSpice))
          c->item = itSpice;
        if(hrand(8000) < 10 + 2 * (items[itSpice] + hard) && !c->monst)
          c->monst = hrand(2) ? moWorm : moDesertman,
          c->mondir = NODIR;
        }
      if(c->land == laRedRock) {
        if(hrand(16000) < 30+items[itRedGem]+hard && !pseudohept(c) && !c->monst && !c->wall) {
          int i = -1;
          for(int t=0; t<6; t++) if(c->mov[t]->mpdist > c->mpdist && !pseudohept(c->mov[t]))
            i = t;
          if(i != -1) {
            c->monst = moHexSnake;
            preventbarriers(c);
            int len = purehepta ? 2 : ROCKSNAKELENGTH;
            cell *c2 = c;
            vector<cell*> rocksnake;
            while(--len) {
              rocksnake.push_back(c2);
              preventbarriers(c2);
              c2->mondir = i;
              createMov(c2, i);
              int j = c2->spn(i);
              cell *c3 = c2->mov[i];
              if(c3->monst || c3->bardir != NODIR || c3->wall) break;
              c2 = c3;
              c2->monst = moHexSnakeTail;
              i = (j + (len%2 ? 2 : 4)) % 6;
              }
            if(size(rocksnake) < ROCKSNAKELENGTH/2 && !purehepta) {
              for(int i=0; i<size(rocksnake); i++) 
                rocksnake[i]->monst = moNone;
              }
            else c2->mondir = NODIR;
            }
          }
        else if(hrand(16000) < 50+items[itRedGem]+hard && (purehepta?hrand(10)<3:!ishept(c)) && !c->monst)
          c->monst = moRedTroll,
          c->mondir = NODIR;
        }
      if(c->land == laDragon) {
        // 40 is the usual rate of dragon generation
        int dchance = 40;
        // but it grows to 400 if no Dragons in sight, to make it faster
        if(cwt.c->land == laDragon && !(havewhat&HF_DRAGON))
          dchance = 400;
        // also, don't generate additional Dragons for newbies
        else if((havewhat&HF_DRAGON) && items[itDragon] < 10)
          dchance = 5; 
          
        if(hrand(150000) < dchance && !c->monst && (!c->wall || c->wall == waChasm)) {
          havewhat |= HF_DRAGON;
          // printf("dragon generated with dchance = %d\n", dchance);
          vector<int> possi;
          for(int t=0; t<6; t++) if(c->mov[t]->mpdist > c->mpdist) possi.push_back(t);
          if(size(possi)) {
            int i = possi[hrand(size(possi))];
            int dragonlength = 6 + items[itDragon] / 2;
            c->monst = moDragonHead; c->hitpoints = 1;
            preventbarriers(c);
            cell *c2 = c;
            int len = dragonlength;
            vector<cell*> dragon;
            while(--len) {
              dragon.push_back(c2);
              preventbarriers(c2);
              c2->mondir = i;
              createMov(c2, i);
              int j = c2->spn(i);
              cell *c3 = c2->mov[i];
              if(c3->monst || c3->bardir != NODIR || c3->wall || c3->mpdist <= 7) break;
              c2 = c3;
              c2->monst = moDragonTail;
              c2->hitpoints = 1;
              i = j + 2 + hrand(c2->type-3);
              i %= c2->type;
              }
            if(size(dragon) < 5 || size(dragon) < dragonlength / 2) {
              for(int i=0; i<size(dragon); i++) 
                dragon[i]->monst = moNone;
              }
            else c2->mondir = NODIR;
            }
          }
        if(!c->monst && !tactic::on && !yendor::on && !euclid && hrand(4000) < 10 && !safety) {
          c->item = itBabyTortoise;
          tortoise::babymap[c] = getBits(c) ^ tortoise::getRandomBits();
          }
        }
      if(c->land == laWineyard) {
        if(hrand(5000) < PT(100 + 2 * (kills[moVineBeast] + kills[moVineSpirit]), 200) && notDippingFor(itWine))
          c->item = itWine;
        if(hrand(8000) < 12 * (items[itWine] + hard))
          c->monst = moVineBeast;
        }
      if(c->land == laZebra) {
        if(c->wall == waNone && hrand(2500) < PT(100 + 2 * (kills[moOrangeDog]), 300) && notDippingFor(itZebra))
          c->item = itZebra;
        if(hrand(10000) < 40 + 2*(items[itZebra] + hard))
          c->monst = moOrangeDog;
        }
      if(isElemental(c->land) && c->land != laElementalWall) {
        eItem localshard = localshardof(c->land);
        int danger = 5 * items[localshard] * items[localshard];
        eMonster elof = elementalOf(c->land);
        int elkills = PT(kills[elof], 25);
        
        if(hrand(8000) < 12 + (items[itElemental] + danger + hard)) {
          c->monst = elof;
          if(c->land != laEAir) chasmifyElemental(c);
          c->wall = waNone;
          if(c->land == laEWater) c->wall = waSea;
          }
        else if(hrand(5000) < 100 + elkills*3 && notDippingFor(itElemental))
          c->item = localshard;
        else if(hrand(5000) < 10 && items[itElemental] >= 10)
          c->item = itOrbSummon;
        else if(hrand(5000) < 10*PRIZEMUL)
          placePrizeOrb(c);
        }
      if(c->land == laEmerald) {
        if(hrand(purehepta?400:1000) < PT(100 + 2 * (kills[moMiner] + kills[moLancer] + kills[moFlailer]), 200) && notDippingFor(itEmerald)) {
          // do not destroy walls!
          bool ok = true;
          for(int i=0; i<c->type; i++) if(c->mov[i]->wall == waCavewall) ok = false;
          if(ok) c->item = itEmerald;
          }
        if(hrand(8000) < 50 + 10 * (items[itEmerald] + hard)) {
          static eMonster emeraldmonsters[4] = { moHedge, moLancer, moFlailer, moMiner };
          c->monst = emeraldmonsters[hrand(4)];
          }
        if(sphere && c->type != 6 && c->master->fiftyval == 5) {
          c->monst = moMiner;
          c->stuntime = 7;
          }
        }
      if(c->land == laBurial && !safety) {
        if(hrand(15000) < 5 + 3 * items[itBarrow] + 4 * hard)
          c->monst = moDraugr;
        else if(hrand(5000) < 20 + (quickfind(laBurial) ? 40 : 0))
          c->item = itOrbSword;
        }
      if(c->land == laJungle) {
        if(hrand(5000) < PT(25 + 2 * (kills[moIvyRoot] + kills[moMonkey]), 40) && notDippingFor(itRuby))
          c->item = itRuby;
        if(hrand(15000) < 5 + 1 * (items[itRuby] + hard))
          c->monst = moMonkey;
        else if(hrand(80000) < 5 + items[itRuby] + hard)
          c->monst = moEagle;
        else if(ishept(c) && c != currentmap->gamestart() && hrand(4000) < 300 + items[itRuby] && !c->monst) {
          int hardchance = items[itRuby] + hard;
          if(hardchance > 25) hardchance = 25;
          bool hardivy = hrand(100) < hardchance;
          if(hardivy ? buildIvy(c, 1, 9) : buildIvy(c, 0, c->type))
            c->item = itRuby;
          }
        }
      if(c->land == laMountain) {
        if(hrand(50000) < 100)
          buildIvy(c, 0, 3);
        else if(hrand(125000) < 100 - celldistAlt(c))
          c->monst = moMonkey;
        else if(hrand(200000) < min(100, -10*celldistAlt(c)) - celldistAlt(c))
          c->monst = moEagle;
        else if(hrand(100) < 5)
          c->wall = waPlatform;
        else if(hrand(100) < 20)
          c->wall = waBigBush;
        else if(hrand(100) < 12)
          c->wall = waSmallBush;
        else if(hrand(500) < -celldistAlt(c) / 5 - items[itAmethyst])
          c->item = itAmethyst;
        }
      if(c->land == laWhirlwind) {
        if(hrand(4500) < items[itWindstone] + hard)
          c->monst = moWindCrow;
        if(hrand(30000) < items[itWindstone] + hard - 5)
          c->monst = moAirElemental;
        }
    
      if(c->land == laStorms) {
        if(hrand(7500) < 25 + (items[itFulgurite] + hard))
          c->monst = (hrand(5) ? moMetalBeast : moMetalBeast2),
          c->hitpoints = 3;
        if(hrand(10000) < 20 + (items[itFulgurite] + hard) && !ishept(c)) {
          c->monst = moStormTroll;
          }
        }
      if(c->land == laWildWest) {
        if(hrand(25000) < 2 + (2 * items[itBounty] + hard) + (items[itRevolver] ? 150:0))
          c->monst = moOutlaw;
        if(hrand(1000) < PT(20 + kills[moOutlaw], 40))
          c->item = itRevolver;
        }
      if(c->land == laAlchemist) {
        if(hrand(5000) < PT(25 + min(kills[moSlime], 200), 100) && notDippingFor(itElixir))
          c->item = itElixir;
        else if(hrand(3500) < 10 + items[itElixir] + hard)
          c->monst = moSlime;
        }
      if(c->land == laRose) {
        if(hrand(2000) < PT(25 + min(kills[moFalsePrincess] + kills[moRoseBeauty] + kills[moRoseLady], 200), 100) && notDippingFor(itRose)) {
          for(int t=0; t<c->type; t++) if(c->mov[t] && c->mov[t]->wall == waRose)
            c->item = itRose;
          }
        else {
          int p = hrand(10000);
          if(p >= 10 + items[itRose] + hard) ;
          else if(p < 10) c->monst = moFalsePrincess;
          else if(p < 13) c->monst = moRoseBeauty;
          else if(p < 18) c->monst = moRoseLady;
          else if(p < 20) c->monst = moRoseBeauty;
          else if(p < 30) c->monst = moFalsePrincess;
          else if(p < 35) c->monst = moRoseLady;
          else if(p < 60) c->monst = moFalsePrincess;
          else if(p < 65) c->monst = moRoseBeauty;
          else c->monst = moFalsePrincess;
          }
        }
      if(c->land == laPower) {
        if(hrand(5000+50*items[itPower]) < 1200) {
          eItem powerorbs[6] = {
            itOrbFlash, itOrbSpeed, itOrbFire, itOrbWinter, itOrbAether, itOrbLife};
          c->item = powerorbs[hrand(6)];
          }
        else if(c->type == 6 && hrand(5000) < 10) 
          c->wall = hrand(2) ? waMirror : waCloud;
        else if(hrand(1000) < 10 + (items[itPower] ? 10:0) + (items[itPower] + hard))
          c->monst = eMonster(moWitch + hrand(NUMWITCH));
        }
      if(isCrossroads(c->land)) {
        if(purehepta && c->land == laCrossroads5 && hrand(100) < 60)
          c->wall = waBarrier;
        else if(c->type == 6 && items[itShard] >= 10 && hrand(8000) < 120*orbcrossfun(items[itShard]))
          c->wall = hrand(2) ? waMirror : waCloud;
        else if(c->type == 6 && tactic::on && isCrossroads(tactic::lasttactic) && hrand(8000) < 120)
          c->wall = hrand(2) ? waMirror : waCloud;
        else if(c->land == laCrossroads4 && hrand(24000) < 10 && tactic::on)
          c->wall = waRose;
        else {
          if(hyperstonesUnlocked() && hrand(25000) < min(PT(tkills(), 2000), 5000) && notDippingFor(itHyperstone))
            c->item = itHyperstone;
          if(hrand(4000) < items[itHyperstone] + 2 * items[itHolyGrail] && !c->monst) {
            // only interesting monsters here!
            eMonster cm = crossroadsMonster();
            if(cm == moIvyRoot) buildIvy(c, 0, c->type);
            else c->monst = cm;
            if(cm == moWorm || cm == moTentacle)
              c->mondir = NODIR;
            c->hitpoints = palaceHP();
            }
          }
        }
      if(c->land == laMirror) {
        if((purehepta?pseudohept(c):!ishept(c)) && hrand(5000) < 120 && notDippingFor(itShard))
          c->wall = hrand(2) ? waMirror : waCloud;
        else if(ishept(c) && hrand(5000) < 10 * PRIZEMUL)
          placePrizeOrb(c);
        else if(hrand(12000) < 8 + items[itShard] + hard)
          c->monst = moRanger;
        else if(hrand(60000) < 8 + items[itShard] + hard)
          c->monst = moEagle;
        }
      if(c->land == laGraveyard) {
        if(hrand(5000) < PT(30 + 4*kills[moZombie] + 6*kills[moNecromancer], 120) && notDippingFor(itBone))
          c->item = itBone;
        if(hrand(20000) < 10 + items[itBone] + hard) {
          eMonster grm[6] = { moZombie, moZombie, moZombie, moGhost, moGhost, moNecromancer};
          c->monst = grm[hrand(6)];
          }
        }
      if(c->land == laRlyeh) {
        if(hrand(5000) < PT(30 + 2 * (kills[moCultist] + kills[moTentacle] + kills[moPyroCultist]), 100) && notDippingFor(itStatue))
          c->item = itStatue;
        if(hrand(8000) < 5 + items[itStatue] + hard && !c->monst)
          c->monst = moTentacle, c->item = itStatue, c->mondir = NODIR;
        else if(hrand(12000) < 5 + items[itStatue] + hard)
          c->monst = hrand(3) ? ((hrand(40) < items[itStatue]-25) ? moCultistLeader : moCultist) : moPyroCultist;
        else if(hrand(8000) < 5 + items[itStatue] + hard && c->type == 6 && !(yendor::on && (yendor::clev().flags & YF_NEAR_TENT)) && celldist(c)>=3) {
          for(int t=0; t<c->type; t++) {
            if(c->mov[t] && c->mov[t]->monst == moNone && (c->wall == waNone || c->wall == waColumn))
              c->mov[t]->wall = ishept(c->mov[t]) ? waColumn : waNone;
            if(c->mov[t]->wall == waColumn)
              c->mov[t]->item = itNone;
            }
          if(buildIvy(c, 0, 3)) c->item = itStatue;
          }
        }
      if(c->land == laTemple) {
        // depth!
        int d = 
          chaosmode ? -15 * items[itGrimoire]:
          (euclid || c->master->alt) ? celldistAlt(c) : 10;
        // remember: d is negative
        if(chaosmode ? hrand(100) < 25 : d % TEMPLE_EACH == 0) {
          if(hrand(5000) < 20 - 2*d && !c->monst)
            c->monst = moTentacle, c->mondir = NODIR;
          }
        else {
          // int d0 = d % TEMPLE_EACH;
          // if(d0<0) d0=-d0;
          if(hrand(100) < 30) //  && d0 != 1 && d0 != TEMPLE_EACH-1)
            c->wall = waBigStatue;
          else if(hrand(20000) < -d)
            c->monst = hrand(3) ? moCultist : moPyroCultist;
          else if(hrand(100000) < -d)
            c->monst = moCultistLeader;
          else if(hrand(5000) < 250)
            c->item = itGrimoire;
          else if(hrand(5000) < 10 && (chaosmode ? items[itGrimoire] >= 10 : -d > TEMPLE_EACH * 10))
            c->item = itOrbDragon;
          }
        }
      if(c->land == laClearing) {
        clearing::generate(c);
        if(pseudohept(c)) {
          int d = -celldistAlt(c);
          if(hrand(2500) < items[itMutant2] + hard - 10)
            c->monst = moRedFox;
          else if(hrand(100 + d) < d)
            c->item = itMutant2;          
          }
        }

      if(c->land == laDryForest) {
        if(hrand(5000) < PT(100 + 2 * (kills[moFireFairy]*2 + kills[moHedge]), 160) && notDippingFor(itFernFlower))
          c->item = itFernFlower;
        if(hrand(4000) < 40 + items[itFernFlower] + hard)
          c->monst = moHedge;
        else if(hrand(8000) < 2 * items[itFernFlower] + hard)
          c->monst = moFireFairy;
        }
      if(c->land == laHell) {
        if(hrand(6000) < PT(120 + (kills[moLesser]), 240) && notDippingFor(itHell))
          c->item = itHell;
        if(hrand(8000) < 40 + items[itHell] * (chaosmode?4:1) + hard)
          c->monst = moLesser;
        else if(hrand(24000) < 40 + items[itHell] * (chaosmode?4:1) + hard)
          c->monst = moGreater;
        }
      if(c->land == laWarpCoast) {
        if(hrand(12000) < 20 + 2 * items[itCoral] + hard) {
          c->monst = moRatling;
          c->stuntime = hrand(2);
          }
        }
      if(c->land == laCocytus) {
        if(hrand(5000) < PT(100 + 2 * (kills[moShark] + kills[moGreaterShark] + kills[moCrystalSage]), 200) && notDippingFor(itSapphire))
          c->item = itSapphire;
        if(hrand(5000) < 2 * (items[itSapphire] + hard)) {
          eMonster ms[3] = { moYeti, moGreaterShark, moCrystalSage };
          c->monst = ms[hrand(3)];
          if(c->monst == moGreaterShark) c->wall = waLake;
          }
        }
      if(c->land == laMotion) {
        if(hrand(1500) < PT(30 + kills[moRunDog], 100) && notDippingFor(itFeather))
          c->item = itFeather;
        if(hrand(20000) < 25 + items[itFeather] + hard) {
          c->monst = moRunDog;
          // preset the movement direction
          // this will make the dog go in the direction of the center,
          // if the player is unreachable/invisible
          for(int d=0; d<c->type; d++) if(c->mov[d] == from) {
            c->mondir = (d+3) % c->type;
            }
          chasmify(c);
          c->wall = shmup::on ? waNone : waChasm;
          }
        }
      if(c->land == laHive) {
        if(isHive(c->land) && hrand(6000) < (hivehard() - 15)) 
          c->monst = randomHyperbug();

        /* if(hrand(1500) < 30 + (kills[moBug0] + kills[moBug1] + kills[moBug2]) && notDippingFor(itRoyalJelly))
          c->item = itRoyalJelly; */
        /* if(hrand(2000) < 2)
          c->monst = eMonster(moBug0 + hrand(3)); */
        }
      if(c->land == laCaribbean) {
//      if(hrand(1500) < 60 && celldistAlt(c) <= -5)
//        c->item = itCompass;
        if(hrand(16000) < 40 + (items[itPirate] + hard))
          c->monst = moPirate;
        }
      if(!c->item && c->wall != waCloud && c->wall != waMirror) {
        if(isCrossroads(c->land))
          placeCrossroadOrbs(c);
        else
          placeLocalOrbs(c);
        }
      }
    }

  if(d == 7) playSeenSound(c);
  
#ifndef NOEDIT
  if(d >= 7 && mapeditor::whichPattern)
    mapeditor::applyModelcell(c);
#endif
  }

bool wchance(int a, int of) {
  of *= 10; 
  a += yendor::hardness() + items[itHolyGrail] + 1;
  if(isCrossroads(cwt.c->land)) 
    a+= items[itHyperstone] * 10;

//if(cwt.c->land == laWhirlwind && !nowhirl) a += items[itWindstone] * 3;

  for(int i=0; i<ittypes; i++) if(itemclass(eItem(i)) == IC_TREASURE)
    a = max(a, (items[i]-10) / 10);
  return hrand(a+of) < a;
  }

void wanderingZebra(cell *start) {
  cell *c = start, *c2 = start;
  for(int it=0; it<100; it++) {
    if(c->cpdist == getDistLimit()) {
      c->monst = moOrangeDog;
      c->stuntime = 0;
      return;
      }
    int q = 0;
    cell *ctab[8];
    for(int i=0; i<c->type; i++) {
      cell *c3 = c->mov[i];
      if(c3 && c3 != c2 && c3->land == laZebra && c3->wall == waNone)
        ctab[q++] = c3;
      }
    if(!q) break;
    c2 = c; c = ctab[hrand(q)];
    }
  }

int getGhostTimer() {
  return shmup::on ? (shmup::curtime - lastexplore) / 350 : turncount - lastexplore;
  }

int getGhostcount() {
  int t = getGhostTimer();
  int ghostcount = 0;
  if(t > 80) ghostcount = (t-80 + hrand(20)) / 20;
  return ghostcount;
  }

int getSeepcount() {
  int t = getGhostTimer();
  int seepcount = 0;
  if(t > 40) seepcount = (t-40 + hrand(20)) / 20;
  return seepcount;
  }

bool canReachPlayer(cell *cf, eMonster m) {
  vector<cell*> v;
  sval++;
  v.push_back(cf); cf->aitmp = sval;
  for(int i=0; i<size(v); i++) {
    cell *c = v[i];
    for(int j=0; j<c->type; j++) {
      cell *c2 = c->mov[j];
      if(!c2) continue;
      if(eq(c2->aitmp, sval)) continue;
      if(!passable_for(m, c2, c, P_MONSTER | P_ONPLAYER | P_CHAIN)) continue;
      if(isPlayerOn(c2)) return true;
      c2->aitmp = sval; v.push_back(c2);
      }
    }
  return false;
  }

bool haveOrbPower() {
  for(int i=0; i<ittypes; i++) if(itemclass(eItem(i)) == IC_ORB && items[i]) return true;
  if(quotient) for(int i=0; i<size(dcal); i++) {
    cell *c = dcal[i];
    if(itemclass(c->item) == IC_ORB) return true;
    }
  else if(sphere) for(int i=0; i<spherecells(); i++) {
    cell *c = getDodecahedron(i)->c7;
    if(itemclass(c->item) == IC_ORB) return true;
    forCellEx(c2, c) if(itemclass(c2->item) == IC_ORB) return true;
    }
  return false;
  }

bool haveKraken() {
  for(int i=0; i<spherecells(); i++) {
    cell *c = getDodecahedron(i)->c7;
    if(c->monst == moKrakenH || c->monst == moKrakenT) return true;
    }
  return false;
  }

void wandering() {
  if(!canmove) return;
  int seepcount = getSeepcount();
  int ghostcount = getGhostcount();
  if(cwt.c->land == laCA) ghostcount = 0;

  if(cwt.c->land == laZebra && cwt.c->wall == waNone && wchance(items[itZebra], 20))
    wanderingZebra(cwt.c);
    
  if(sphere || quotient == 1) {
    int maxdist = 0;
    for(int i=0; i<size(dcal); i++) if(dcal[i]->cpdist > maxdist) maxdist = dcal[i]->cpdist;
    for(int i=0; i<size(dcal); i++) if(dcal[i]->cpdist >= maxdist-1) { first7 = i; break; }
    
    if(hrand(5) == 0) {
      // spawn treasure
      }
    }
  
  while(first7 < size(dcal)) {
    int i = first7 + hrand(size(dcal) - first7);
    cell *c = dcal[i];
    
    if((sphere || quotient == 1) && !c->item && hrand(5) == 0 && c->land != laHalloween) {
      if(passable(c, NULL, 0) || euclidland == laKraken) {
        if(!haveOrbPower() && euclidland != laHell) for(int it=0; it<1000 && !c->item; it++)
          placeLocalOrbs(c);
        if(!c->item) c->item = wanderingTreasure(c);
        if(c->item == itShard) {
          c->item = itNone, c->wall = hrand(2) ? waMirror : waCloud;
          }
        if(c->item == itFulgurite) {
          c->item = itNone, c->wall = waSandstone;
          }
        if(c->item == itBarrow) 
          c->landparam = 2 + hrand(2),
          c->wall = waBarrowDig;
        }
      }

    if(!c->monst) c->stuntime = 0;

    if(timerghost && !sphere && quotient != 1) {
      // wandering seeps & ghosts
      if(seepcount && c->wall == waCavewall && !c->monst && canReachPlayer(c, moSlime)) {
        c->monst = moSeep;
        playSeenSound(c);
        seepcount--;
        continue;
        }
      
      if(ghostcount && !c->monst) {
        c->monst = moGhost;
        playSeenSound(c);
        ghostcount--;
        continue;
        }
      }
        
    if((c->wall == waCavewall || c->wall == waDeadwall) && !c->monst &&
      wchance(items[treasureType(c->land)], 10) && canReachPlayer(c, moSlime)) {
      c->monst = moSeep;
      playSeenSound(c);
      continue;
      }
    
    else if(c->wall == waCTree && !c->monst && wchance(items[itPirate], 15) && canReachPlayer(c, moSlime)) {
      c->monst = moParrot;
      playSeenSound(c);
      continue;
      }

    else if(c->land == laEndorian && c->wall == waNone && wchance(items[itApple], 50)) {
      c->monst = moSparrowhawk;
      playSeenSound(c);
      continue;
      }
    
    else if(c->wall == waSea && !c->monst) {
      if(c->land == laCaribbean && wchance(items[itPirate], 15) && canReachPlayer(c, moPirate)) {
        c->monst = moCShark;
        playSeenSound(c);
        continue;
        }
      if(c->land == laWarpSea && avengers && canReachPlayer(c, moPirate)) {
        c->monst = moRatlingAvenger;
        c->wall = waBoat;
        avengers--;
        if(cheater) printf("avenger comes\n");
        playSeenSound(c);
        continue;
        }
      if(c->land == laWarpSea && wchance(items[itCoral], 25) && canReachPlayer(c, moPirate)) {
        c->monst = moRatling;
        c->wall = waBoat;
        playSeenSound(c);
        continue;
        }
      if(c->land == laOcean && (items[itCoast] > 50 || (cwt.c->landparam < 25 && c->landparam < 25)) && wchance(items[itCoast], 25) && canReachPlayer(c, moEagle)) {
        c->monst = moAlbatross;
        playSeenSound(c);
        continue;
        }
      if(c->land == laLivefjord && wchance(items[itFjord], 80) && items[itFjord] >= 10 && canReachPlayer(c, moWaterElemental)) {
        c->monst = moWaterElemental;
        playSeenSound(c);
        continue;
        }
      if(c->land == laKraken && ((sphere && !hrand(15)) || wchance(items[itKraken], 240)) && !pseudohept(c)) {
        bool b = canReachPlayer(c, moKrakenH);
        if(sphere && (haveKraken() || !items[itOrbFish])) { 
          c->monst = moViking; c->wall = waBoat; c->item = itOrbFish; 
          playSeenSound(c);
          continue;
          }        
        if(b) forCellEx(c2, c) if((sphere || c2->cpdist > 7) && !pseudohept(c2)) {
          forCellCM(c3, c2) if(c3->monst || c3->wall != waSea) 
            goto notfound;
          c2->monst = moKrakenH;
          playSeenSound(c2);
          for(int i=0; i<c2->type; i++) {
            c2->mov[i]->monst = moKrakenT;
            c2->mov[i]->hitpoints = 1;
            c2->mov[i]->mondir = c2->spn(i);
            }
          goto found;
          }
        goto notfound;
        found:
        continue;
        }
      notfound:
      break;
      }
    
    else if(c->monst || c->pathdist == PINFD) break;
    
    else if(c->land == laClearing && wchance(items[itMutant2], 150) && items[itMutant2] >= 15 && !c->monst && c->type == 7) 
      c->monst = moRedFox;

    else if(hrand(50) < statuecount * statuecount) 
      c->monst = moCultistLeader;

    else if(c->land == laIce && wchance(items[itDiamond], 10))
      c->monst = hrand(2) ? moWolf : moYeti;

    else if(c->land == laDesert && wchance(items[itSpice], 10))
      c->monst = hrand(10) ? moDesertman : moWorm;

    else if(c->land == laDragon && (items[itDragon] >= 8 || items[itOrbYendor]) && wchance(items[itDragon], 20))
      c->monst = moFireElemental;

    else if(c->land == laRedRock && wchance(items[itRedGem], 10))
      c->monst = hrand(10) ? moRedTroll : moHexSnake;

    else if(c->land == laCaves && wchance(items[itGold], 5))
      c->monst = hrand(3) ? moTroll : moGoblin;

    else if(c->land == laBull && wchance(items[itBull], 40))
      c->monst = moGadfly;

    else if(items[itBull] >= 50 && size(butterflies) && wchance(items[itBull]-49, 25))
      c->monst = moGadfly;

    else if(c->land == laPrairie && cwt.c->LHU.fi.flowerdist > 3 && wchance(items[itGreenGrass], prairie::isriver(cwt.c) ? 150 : 40))
      c->monst = moGadfly;

    else if(c->land == laHive && wchance(hivehard(), 25))
      c->monst = randomHyperbug();

    else if(c->land == laDeadCaves && wchance(items[itSilver], 5))
      c->monst = hrand(20) ? (hrand(3) ? moDarkTroll : moGoblin) : moEarthElemental;

    else if(c->land == laJungle && wchance(items[itRuby], 40))
      c->monst = hrand(10) ? moMonkey : moEagle;

    else if(c->land == laMirror && wchance(items[itShard], 15))
      c->monst = hrand(10) ? moRanger : moEagle;

    else if(c->land == laWarpCoast && wchance(items[itCoral], 40))
      c->monst = moRatling;

    else if(c->land == laBurial && wchance(items[itBarrow], 250))
      c->monst = moDraugr;

    else if(c->land == laTrollheim && wchance(items[itTrollEgg], 150))
      c->monst = pickTroll(c);

    else if(c->land == laRose && wchance(items[itRose], 25))
      c->monst = moFalsePrincess;

    else if(c->land == laHell && wchance(items[itHell], 20))
      c->monst = hrand(3) ? moLesser : moGreater;

    else if(c->land == laStorms && wchance(items[itFulgurite], 30)) {
      c->monst = hrand(3) ? moMetalBeast : moStormTroll;
      c->hitpoints = 3;
      }

    else if(c->land == laWhirlwind && wchance(items[itWindstone], 30)) 
      c->monst = hrand(5) ? moWindCrow : moAirElemental;

    else if(c->land == laWildWest && wchance(items[itBounty], 30)) 
      c->monst = moOutlaw;

    else if(c->land == laEndorian && c->wall == waTrunk && wchance(items[itApple], 30)) 
      c->monst = moResearcher;

    else if(c->land == laOvergrown && wchance(items[itMutant], 50)) 
      c->monst = moForestTroll;

    else if(c->land == laCaribbean && wchance(items[itPirate], 30))
      c->monst = moPirate;

    else if(c->land == laRlyeh && wchance(items[itStatue], 15))
      c->monst = hrand(3) ? moPyroCultist : 
        (hrand(40) < items[itStatue]-25) ? moCultistLeader : moCultist;

    else if(c->land == laGraveyard && wchance(items[itBone], 15))
      c->monst = hrand(5) ? moGhost : moNecromancer;
      
    else if(isHaunted(c->land) && wchance(items[itLotus], 15))
      c->monst = moGhost;
      
    else if(c->land == laDryForest && wchance(items[itFernFlower], 5))
      c->monst = hrand(5) ? moHedge : moFireFairy;
      
    else if(c->land == laCocytus && wchance(items[itSapphire], 45))
      c->monst = moCrystalSage;
      
    else if(c->land == laAlchemist && wchance(items[itElixir], 3) && canReachPlayer(c, moSlime) && c->item == itNone)
      c->monst = moSlime; // ?
    
    else if(isElemental(c->land) && wchance(items[itElemental], 20))
      c->monst = elementalOf(c->land);
    
    else if(c->land == laIvoryTower && wchance(items[itIvory], 20))
      c->monst = cellEdgeUnstable(c) ? moGargoyle : moFamiliar;
    
    else if(c->land == laMinefield && wchance(items[itBombEgg]-20, 400))
      c->monst = moBomberbird;
    
    else if(c->land == laEmerald && wchance(items[itEmerald], 5)) {
      static eMonster m[4] = {moHedge, moLancer, moMiner, moFlailer};      
      c->monst = m[hrand(4)];
      }
    
    else if(c->land == laWineyard && wchance(items[itWine], 10)) {
      c->monst = moVineBeast;
      }
    
    else if(c->land == laPalace && wchance(items[itPalace], 50)) {
      if(princess::dist(c) < OUT_OF_PRISON && !princess::challenge) break;
      
      if(items[itPalace] >= 15 && hrand(100) < 10)
        c->monst = moVizier;
      else if(items[itPalace] >= 5 && hrand(100) < 50)
        c->monst = hrand(2) ? moFatGuard : moSkeleton;
      else c->monst = moPalace;
      c->hitpoints = palaceHP();
      }
    
    else if(c->land == laLivefjord && wchance(items[itFjord], 10)) {
      c->monst = sphere ? pick(moViking, moWaterElemental, moFjordTroll) : moViking;
      }
    
    else if(c->land == laOcean && wchance(items[itCoast], 100)) {
      c->monst = moAlbatross;
      }
    
    else if(c->land == laPower && wchance(items[itPower], 10)) {
      c->monst = eMonster(moWitch + hrand(NUMWITCH));
      }
    
    else if(c->land == laCamelot && hrand(30) == 0 && (euclid || c->master->alt) && celldistAltRelative(c) < 0) {
      eMonster m[3] = { moHedge, moLancer, moFlailer };
      c->monst = m[hrand(3)];
      }
    
    else if(isCrossroads(c->land) && items[itHyperstone] && wchance(items[itHyperstone], 20)) {
      c->monst = wanderingCrossroadsMonster();
      c->hitpoints = palaceHP();
      }
    
    else break;
      
    playSeenSound(c);
    if(c->monst == moWorm || c->monst == moHexSnake) c->mondir = NODIR;
      
    // laMotion -> no respawn!
    }
  }

void generateAlts(heptagon *h) {
  if(!h->alt) return;
  preventbarriers(h->c7);
  for(int i=0; i<7; i++) preventbarriers(h->c7->mov[i]);
  for(int i=0; i<7; i++) 
    createStep(h->alt, i)->alt = h->alt->alt;
  int relspin = -4; // for horocycles it must go the other way
  if(quotient) relspin = 0;
  else {
  for(int j=0; j<7; j++) for(int i=0; i<7; i++) {
    createStep(h, i);
    if(h->move[i]->alt == h->alt->move[j]) {
      relspin = (i-j+7) % 7;
      break;
      }
    }
  if(relspin == -4 && quotient != 2) {
    if(h->alt != h->alt->alt) {
      printf("relspin {%p:%p}\n", h->alt, h->alt->alt);
      {for(int i=0; i<7; i++) printf("%p ", h->alt->move[i]);} printf(" ALT\n");
      {for(int i=0; i<7; i++) printf("%p ", h->move[i]);} printf(" REAL\n");
      {for(int i=0; i<7; i++) printf("%p ", h->move[i]->alt);} printf(" REAL ALT\n");
      }
    relspin = 3;
    } }
  // h[relspin] matches alt[0]
//printf("{%d~%d}\n", h->distance, h->alt->distance);
  for(int i=0; i<7; i++) {
    int ir = (7+i-relspin)%7;
    heptagon *hm = h->alt->move[ir];
    heptagon *ho = createStep(h, i);
//  printf("[%p:%d ~ %p:%d] %p ~ %p\n", 
//    h, i, h->alt, ir, 
//    ho, hm);
    if(ho->alt && ho->alt != hm) {
      if(ho->alt->alt == hm->alt && !quotient) {
        printf("ERROR: alt cross! [%d -> %d]\n", ho->alt->distance, hm->distance);
        // exit(1);
        }
      continue;
      }
    ho->alt = hm;
    }
  }

heptagon *createAlternateMap(cell *c, int rad, hstate firststate, int special) {

  // check for direction
  int gdir = -1;
  for(int i=0; i<c->type; i++) {
    if(c->mov[i] && c->mov[i]->mpdist < c->mpdist) gdir = i;
    }
  if(gdir < 0) return NULL;
  
  // check for non-crossing
  int bd = 2;
  cellwalker bb(c, bd);
  if(!(checkBarriersFront(bb) && checkBarriersBack(bb))) {
    return NULL;
    }

  // okay, let's go then!
  cellwalker bf(c, gdir);
  cell *cx[rad+1];
  for(int i=0; i<rad; i++) {
    cx[i] = bf.c;
    if(bf.c->type == 6)
      cwspin(bf, 3);
    else
      cwspin(bf, 3 + hrand(2));
    cwstep(bf);
    }
  cx[rad] = bf.c;
  heptagon *h = bf.c->master;
  
  if(special == waPalace) {  
  
    // type 7 is ensured
    cell *c = bf.c;
    
    if(cdist50(c) != 0) return NULL;
    if(polarb50(c) != 1) return NULL;
    }
  
  heptagon *alt = new heptagon;
  allmaps.push_back(new hrmap_alternate(alt));
//printf("new alt {%p}\n", alt);
  alt->s = firststate;
  alt->emeraldval = 0;
  alt->zebraval = 0;
  for(int i=0; i<7; i++) alt->move[i] = NULL;
  alt->distance = 0;
  alt->c7 = NULL;
  alt->alt = alt;
  h->alt = alt;
  
  for(int d=rad; d>=0; d--) {
    generateAlts(cx[d]->master);  
    preventbarriers(cx[d]);
    }

  if(special == waPalace) {

    cell *c = bf.c;

    princess::generating = true;
    c->land = laPalace;
    for(int j=BARLEV; j>=0; j--)
      setdist(c, j, NULL);
    princess::generating = false;
    
    princess::newInfo(c);
    princess::forceMouse = false;  

    if(princess::gotoPrincess && cheater) princess::gotoPrincess=false, cwt.c = c;
    }
  
  return alt;
//for(int d=rad; d>=0; d--) printf("%3d. %p {%d}\n", d, cx[d]->master, cx[d]->master->alt->distance);
  }

namespace halloween {
  vector<cell*> srch;

  cell *farempty(bool lastresort = false) {
    int maxdist = 0;
    cell* validcells[100];
    int qvc = 0;
    int firstfar1 = 0;
    for(int i=1; i<size(dcal); i++) {
      bool okay = 
        dcal[i]->item == itNone && dcal[i]->monst == moNone && dcal[i]->wall == waNone;
      if(lastresort)
        okay = dcal[i]->wall != waChasm && !isMultitile(dcal[i]->monst);
      if(okay) {
        if(dcal[i]->cpdist > maxdist) 
          firstfar1 = qvc,
          maxdist = dcal[i]->cpdist; 
        validcells[qvc++] = dcal[i];
        }
      }
    
    if(qvc == firstfar1) return farempty(true);
    
    return validcells[firstfar1 + hrand(qvc - firstfar1)];
    }
  
  int demoncount;
  int swordpower;
  int dragoncount;

  void reset() {
    demoncount = 0;
    swordpower = 0;
    dragoncount = 0;
    }
  
  eMonster nextDemon() {
    demoncount++;
    if(demoncount % 9 == 0) return moGreater;
    return moLesser;
    }
  
  void putMonster(eMonster m) {
    cell *c = farempty();
    c->hitpoints = 3;
    c->monst = m;
    playSeenSound(c);
    if(!kills[m]) switch(m) {
      case moGhost:
        addMessage(XLAT("Ghosts can move through chasms!"));
        break;
      case moSkeleton:
        addMessage(XLAT("Push Skeletons into the holes!"));
        break;
      case moDraugr:
        addMessage(XLAT("You'll need your magical sword against the Draugar!"));
        break;
      case moLesser:
        addMessage(XLAT("Demons are slow, but you'll need the experience against stronger ones..."));
        break;
      case moGreater:
        addMessage(XLAT("You will need more experience to defeat the Greater Demon!"));
        break;
      case moPyroCultist:
        addMessage(XLAT("Cultists throw fire at you from distance!"));
        break;
      case moFlailer:
        addMessage(XLAT("Defeat Flail Guards by moving away from them."));
        break;
      case moVampire:
        addMessage(XLAT("Vampire Bats drain your magical powers!"));
        break;
      default: break;
      }
    c->stuntime = 2;
    }
  
  void getTreat(cell *where) {
    if(!items[itTreat]) reset();
    gainItem(itTreat);
    farempty()->item = itTreat;
    int itr = items[itTreat];
    items[itOrbTime] += 30;
    int monpower = 19 + itr + hrand(itr);
    int mcount = 0;
    while(monpower > 0) {
      int id = hrand(10000);
    
#define CHANCE(x) (id -= x, id < 0)
      if(CHANCE(400) && itr >= 5) {
        putMonster(moGhost);
        monpower -= 30;
        mcount++;
        }
      else if(CHANCE(400)) {
        putMonster(pick(moWitch, moZombie));
        monpower -= 20;
        mcount++;
        }
      else if(CHANCE(400) && itr >= 5) {
        putMonster(nextDemon());
        monpower -= 10;
        mcount++;
        }
      else if(CHANCE(100) && itr >= 12) {
        putMonster(moVampire);
        monpower -= 10;
        swordpower -= 5;
        if(swordpower < 0) swordpower = 0;
        mcount++;
        }
      else if(CHANCE(400) && swordpower > 0 && !mcount && itr >= 15) {
        putMonster(moDraugr);
        swordpower -= 3;
        monpower -= 100;
        mcount++;
        }
      else if(CHANCE(400) && itr >= 10 && !mcount && itr >= 10) {
        putMonster(moSkeleton);
        monpower -= 60;
        mcount++;
        }
      else if(CHANCE(10) && itr >= 15) {
        putMonster(moWitchFire);
        monpower -= 35;
        mcount++;
        }
      else if(CHANCE(100) && itr >= 5) {
        putMonster(moFlailer);
        monpower -= 35;
        mcount++;
        }
      else if(CHANCE(100) && itr >= 5) {
        putMonster(moPyroCultist);
        monpower -= 35;
        mcount++;
        }        
      else if(CHANCE(5) && itr >= 20 && kills[moSkeleton]) {
        putMonster(moFatGuard);
        monpower -= 35;
        mcount++;
        }        
      else if(CHANCE(5) && itr >= 30 && kills[moFlailer]) {
        putMonster(moHedge);
        monpower -= 35;
        mcount++;
        }        
      else if(CHANCE(5) && itr >= 40 && kills[moHedge]) {
        putMonster(moLancer);
        monpower -= 60;
        mcount++;
        }        
      else if(CHANCE(1) && itr >= 50 && kills[moHedge]) {
        putMonster(moFireFairy);
        monpower -= 40;
        mcount++;
        }        
      else if(CHANCE(5) && itr >= 60) {
        putMonster(moBomberbird);
        monpower -= 25;
        mcount++;
        }        
      else if(CHANCE(5) && itr >= 60) {
        putMonster(moRatlingAvenger);
        monpower -= 30;
        mcount++;
        }        
      else if(CHANCE(5) && itr >= 60) {
        putMonster(moVineBeast);
        monpower -= 30;
        mcount++;
        }        
      else if(CHANCE(5) && itr >= 60) {
        dragoncount++;
        }
      else if(dragoncount && !purehepta && !mcount) {
        bool fill = false;
        for(int i=0; i<4; i++) 
          if(!dragoncells[i] || dragoncells[i]->monst)
            fill = true;
        swap(dragoncells[0], dragoncells[3]);
        swap(dragoncells[1], dragoncells[2]);
        if(fill) continue;
        for(int i=0; i<4; i++) {
          dragoncells[i]->monst = i ? moDragonTail : moDragonHead;
          dragoncells[i]->mondir = i==3 ? NODIR : neighborId(dragoncells[i], dragoncells[i+1]);
          dragoncells[i]->hitpoints = 1;
          dragoncells[i]->stuntime = 1;          
          playSeenSound(dragoncells[i]);
          }
        monpower -= 200;
        mcount++;
        dragoncount--;
        }        
      }
    int id = hrand(100);
    if(items[itTreat] == 1) {
#ifndef MOBILE
      addMessage(XLAT("Hint: use arrow keys to scroll."));
#endif
      }
    else if(items[itTreat] == 2) {
#ifndef MOBILE
      addMessage(XLAT("Hint: press 1 2 3 4 to change the projection."));
#endif
      }
    else if(items[itTreat] == 3) {
      items[itOrbShell] += 20;
      addMessage(XLAT("You gain a protective Shell!"));
      }                                         
    else if(items[itTreat] == 4) {
      items[itOrbShell] += 10;
      addMessage(XLAT("Hint: magical powers from Treats expire after a number of uses."));
      }
    else if(kills[moSkeleton] && CHANCE(10)) {
      addMessage(XLAT("A magical energy sword. Don't waste its power!"));
      items[itOrbSword] += 5; // todo facing
      swordpower += 5;
      }
    else if(kills[moDraugr] && items[itOrbSword] && CHANCE(10)) {
      addMessage(XLAT("Another energy sword!"));
      items[itOrbSword2] += 5;
      swordpower += 5;
      }
    else if(kills[moFlailer] && CHANCE(10)) {
      items[itOrbThorns] += 5;
      addMessage(XLAT("You got Thorns! Stab monsters by moving around them."));
      }
    else if(kills[moGhost] && CHANCE(10)) {
      items[itOrbAether] += 5;
      addMessage(XLAT("Aethereal powers let you go through the holes."));
      }
    else { 
      if(items[itOrbShell] > ORBBASE)
        addMessage(XLAT("The tasty treat increases your protection."));
      else 
        addMessage(XLAT("You gain your protective Shell back!"));
      items[itOrbShell] += 5;
      }
    }
  }

void doOvergenerate() {
  int dcs = size(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->cpdist <= sightrange-6) setdist(c, 1, NULL);
    }
  }
