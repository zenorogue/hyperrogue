// Hyperbolic Rogue
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// land statistics and flags

bool nodisplay(eMonster m) {
  return 
    m == moIvyDead ||
    m == moDragonTail ||
    m == moWolfMoved ||
    m == moIvyNext ||
    m == moIvyDead;
  }    

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

    case laMirror: case laMirrored: case laMirrorWall: case laMirrorWall2: case laMirrored2:
      return (m == moMirrorSpirit || m == moNarciss || m == moMimic) ? 1 : 0;
      
    case laMirrorOld:
      return (m == moEagle || m == moRanger || m == moMimic) ? 1 : 0;
      
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
      return (m == moLesser || m == moGreater) ? 2 : 0;
      
    case laCocytus: 
      return (m == moShark || m == moGreaterShark || m == moCrystalSage) ? 2 :
        m == moYeti ? 1 : 0;

    case laCrossroads: case laCrossroads2: case laCrossroads3: case laCrossroads4: 
    case laCrossroads5:
    case laNone: case laBarrier: case laOceanWall: case laCanvas: case laMemory: 
      return 0;
    
    case laEmerald: 
      return (m == moFlailer || m == moLancer || m == moMiner) ? 2 : 
        m == moHedge ? 1 : 0;
      
    case laWineyard: 
      return (m == moVineSpirit || m == moVineBeast) ? 2 : 0;
    
    case laHive: 
      return isBug(m) ? 1 : 0;

    case laDeadCaves: 
      return (m == moEarthElemental || m == moDarkTroll) ? 2 : 
        (m == moGoblin || m == moSeep) ? 1 : 0;

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
      if(m == elementalOf(l) && m) return 2;
      return (m == moAirElemental || m == moEarthElemental || m == moWaterElemental || m == moFireElemental) ? 1 : 0;
    
    case laStorms: 
      return (m == moMetalBeast || m == moMetalBeast2 || m == moStormTroll) ? 1 : 0;
    
    case laOvergrown: 
      return (m == moMutant || m == moForestTroll) ? 1 : 0;
      
    case laWildWest: 
      return (m == moOutlaw) ? 2 : 0;

    case laHalloween: 
      return (m == moGhost || m == moZombie || m == moWitch ||
        m == moLesser || m == moGreater || 
        m == moVampire || m == moDraugr ||
        m == moSkeleton || m == moWitchFire || 
        m == moFlailer || m == moPyroCultist || 
        m == moFatGuard || m == moHedge || 
        m == moLancer || m == moFireFairy || 
        m == moBomberbird || m == moRatlingAvenger || 
        m == moVineBeast || m == moDragonHead || m == moDragonTail) ? 1 : 0;

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
    
    case laVolcano:
      return (m == moLavaWolf || m == moSalamander) ? 2 : 0;
    
    case laTerracotta: case laMercuryRiver:
      return m == moJiangshi ? 2 : m == moTerraWarrior ? 1 : 0;
    
    case laBlizzard:
      return (m == moVoidBeast || m == moIceGolem) ? 2 : 0;

    case laHunting:
      return m == moHunterDog ? 1 : 0;
    
    case laDual: 
      return m == moRatling ? 2 : 0;
    
    case laSnakeNest:
      return m == moHexSnake ? 2 : 0;

    case laCA: return 0;

    case laDocks:
      return among(m, moRatling, moPirate, moCShark, moAlbatross, moFireFairy) ? 2 : 0;
    
    case laSwitch:
      return m == moSwitch1 || m == moSwitch2 ? 2 : 0;
    
    case laRuins:
      return among(m, moPair, moHexDemon, moAltDemon, moMonk, moCrusher) ? 2 :
        m == moSkeleton ? 1 : 0;
      
    case laMagnetic:
      return isMagneticPole(m) ? 2 : 0;
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

    case laMirror: case laMirrored: case laMirrorWall: case laMirrorWall2: case laMirrored2:
    case laMirrorOld:
      return itShard;

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
    case laMemory: return itNone;
    
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
    
    case laVolcano: return itLavaLily;
    case laTerracotta: case laMercuryRiver: return itTerra;
    case laBlizzard: return itBlizzard;
    case laHunting: return itHunting;
    case laDual: return itGlowCrystal;
    case laSnakeNest: return itSnake;
    case laDocks: return itDock;
    case laRuins: return itRuins;
    case laSwitch: return itSwitch;
    case laMagnetic: return itMagnet;
    
    case laCA: return itNone;
    }
  return itNone;
  }

eItem treasureTypeUnlock(eLand l, eItem u) {
  if(u != itOrbLove && l == laPrincessQuest)
    return itPalace;
  return treasureType(l);
  }

eLand landof(eItem it) {
  for(int i=0; i<landtypes; i++) if(treasureType(eLand(i)) == it) return eLand(i);
  return laNone;
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

int landMultiplier(eLand l) {
  if(l == laCamelot || l == laPrincessQuest) return 10;
  return 1;
  }

bool isTrollLand(eLand l) {
  return l == laCaves || l == laStorms || l == laOvergrown ||
    l == laDeadCaves || l == laLivefjord || l == laRedRock;
  }

bool isCrossroads(eLand l) {
  return l == laCrossroads || l == laCrossroads2 || l == laCrossroads3 ||
    l == laCrossroads4 || l == laCrossroads5;
  }

bool bearsCamelot(eLand l) {
  return isCrossroads(l) && l != laCrossroads2 && l != laCrossroads5;
  }

bool inmirror(eLand l) {
  return l == laMirrored || l == laMirrorWall2 || l == laMirrored2;
  }

bool inmirrororwall(eLand l) {
  return l == laMirrored || l == laMirrorWall2 || l == laMirrored2 || l == laMirrorWall;
  }

bool inmirror(cell *c) {
  return inmirror(c->land);
  }

bool inmirror(const cellwalker& cw) {
  return inmirror(cw.c->land);
  }

eLand oppositeElement(eLand l, eLand l2) {
  if(l == laEFire) return laEWater;
  if(l == laEWater) return laEFire;
  if(l == laEAir) return laEEarth;
  if(l == laEEarth) return laEAir;
  if(l == laMirror && l2 == laMirrored) return laMirrored2;
  if(l == laMirrored2 && l2 == laMirrored) return laMirror;
  return l;
  }

// land unlocking

eLand firstland = laIce, specialland = laIce;

bool chaosmode = false;

bool landUnlockedRPM(eLand n) {
  if(isRandland(n) == 2) return true;
  if(isRandland(n) == 1)
    return (autocheat || cheater || hiitemsMax(treasureType(n)) >= 10);
  return false;
  }

bool landUnlocked(eLand l) {
  if(randomPatternsMode) {
    return landUnlockedRPM(l);
    }
  
  switch(l) {
    case laOvergrown: 
      return gold() >= R60 && items[itRuby] >= U10;
    
    case laStorms: case laWhirlwind: 
      return gold() >= R60;
    
    case laWildWest: case laHalloween: 
      return false;
      
    case laIce: case laJungle: case laCaves: case laDesert: 
    case laMotion: case laCrossroads:  case laAlchemist:
      return true;

    case laMirror: case laMinefield: case laPalace:
    case laOcean: case laLivefjord: case laMirrored: case laMirrorWall: case laMirrorWall2:
    case laDocks:
    case laMirrored2: 
      return gold() >= R30;
    
    case laCaribbean: case laWhirlpool:
      return gold() >= R30;
    
    case laRlyeh: case laDryForest: case laWineyard: case laCrossroads2:
      return gold() >= R60;
    
    case laDeadCaves:
      return gold() >= R60 && items[itGold] >= U10;

    case laGraveyard:
      return tkills() >= R100;

    case laHive:
      return tkills() >= R100 && gold() >= R60;

    case laRedRock:
      return gold() >= R60 && items[itSpice] >= U10;
    
    case laEmerald:
      return (items[itFernFlower] >= U5 && items[itGold] >= U5) || kills[moVizier];
    
    case laCamelot:
      return items[itEmerald] >= U5;
    
    case laHell: case laCrossroads3:
      return hellUnlocked();
      
    case laPower: 
      return items[itHell] >= U10;

    case laCocytus: 
      return items[itHell] >= U10 && items[itDiamond] >= U10;

    case laTemple:
      return items[itStatue] >= U5;

    case laClearing:
      return items[itMutant] >= U5;

    case laIvoryTower: return gold() >= R30;
    case laZebra: return gold() >= R30 && items[itFeather] >= U10;

    case laEAir: case laEEarth: case laEWater: case laEFire:  case laElementalWall:
      return elementalUnlocked();
    
    case laBarrier: case laNone: case laOceanWall: case laCanvas: case laCA: case laMemory:
      return false;
    
    case laMirrorOld:
      return false;
    
    case laHaunted: case laHauntedWall: case laHauntedBorder: 
      return items[itBone] >= U10;

    case laPrincessQuest: return kills[moVizier] && !shmup::on && multi::players == 1;
    
    case laRose: 
      return gold() >= R90;
      
    case laWarpCoast: case laWarpSea: 
      return gold() >= R30;
      
    case laCrossroads4:
      return gold() >= R200;
    
    case laEndorian:
      return items[itIvory] >= U10;
    
    case laTortoise:
      return tortoise::seek();
    
    case laDragon:
      return killtypes() >= R20;
    
    case laKraken:
      return items[itFjord] >= U10;

    case laBurial:
      return items[itKraken] >= U10;

    case laTrollheim:
      return trollUnlocked();
    
    case laDungeon:
      return items[itPalace] >= U5 && items[itIvory] >= U5;

    case laMountain:
      return items[itRuby] >= U5 && items[itIvory] >= U5;
      
    case laReptile:
      return gold() >= R30 && items[itElixir] >= U10;
    
    case laPrairie:
    case laBull:
      return gold() >= R90;
    
    case laVolcano:
      return gold() >= R60 && items[itElixir] >= U10;
    
    case laHunting:
      return true;
    
    case laTerracotta: case laMercuryRiver:
      return gold() >= R90;
    
    case laBlizzard:
      return items[itDiamond] >= U5 && items[itWindstone] >= U5;
    
    case laCrossroads5:
      return gold() >= R300;
    
    case laDual:
    case laSnakeNest:
      return gold() >= R90;
      
    case laSwitch:
      return gold() >= R30 && items[itElixir] >= U10;
    
    case laRuins:
      return kills[moSkeleton];
    
    case laMagnetic:
      return false; // not implemented
    }
  return false;
  }

void countHyperstoneQuest(int& i1, int& i2) {
  i1 = 0; i2 = 0;
  generateLandList(isLandIngame);
  for(eLand l: landlist) if(l != laCamelot && l != laPrincessQuest) {
    eItem ttype = treasureType(l);
    if(ttype != itHyperstone) {
      i2++; if(items[ttype] >= R10) i1++;
      }
    }
  }

bool hyperstonesUnlocked() {
  int i1, i2;
  if(tactic::on && isCrossroads(specialland) && !tactic::trailer) return true;
  countHyperstoneQuest(i1, i2);
  return i1 == i2;
  }

// 2 = always available, 1 = highscore required, 0 = never available
int isRandland(eLand l) {
  if(l == laIce || l == laDesert || l == laCaves || l == laWildWest || l == laDocks)
    return 2;
  for(eLand ll: randlands) if(l == ll) return 1;
  return 0;
  }

bool incompatible1(eLand l1, eLand l2) {
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
  if(l1 == laGraveyard && l2 == laRedRock) return true;
  if(l1 == laGraveyard && l2 == laEmerald) return true;
  if(l1 == laDeadCaves && l2 == laEmerald) return true;
  if(l1 == laDeadCaves && l2 == laCaves) return true;
  if(l1 == laWarpSea && l2 == laKraken) return true;
  if(l1 == laPrairie && l2 == laCrossroads3) return true;
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
    eLand n = randlands[hrand(size(randlands))];
    if(incompatible(n, old)) continue;
    if(landUnlockedRPM(n)) return n;
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
#define LIKELY2 for(int u=0; u<2; u++)

/* bool noChaos(eLand l) {
  if(l == laOcean || l == laTemple) return false;
  return 
    isCrossroads(l) || isCyclic(l) || isHaunted(l) || 
    l == laCaribbean || isGravityLand(l) || l == laPrincessQuest ||
    l == laPrairie || l == laHalloween;
  } */

eLand getNewSealand(eLand old) {
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

bool createOnSea(eLand old) {
  return
    old == laWarpSea || old == laCaribbean || old == laKraken ||
    (old == laLivefjord && hrand(2)) || 
    (old == laDocks && hrand(2)) ||
    (old == laOcean && (chaosmode ? hrand(2) : !generatingEquidistant));
  }

hookset<eLand(eLand)> *hooks_nextland;

eLand getNewLand(eLand old) {

  if(old == laMirror && !chaosmode && hrand(10) >= (tactic::on ? 0 : markOrb(itOrbLuck) ? 5 : 2)) return laMirrored;
  if(old == laTerracotta && !chaosmode && hrand(5) >= (tactic::on ? 0 : markOrb(itOrbLuck) ? 2 : 1)) return laTerracotta;
    
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

  if(tactic::on && !(tactic::trailer && old == specialland)) return specialland;
  if(weirdhyperbolic && specialland != old) return specialland;

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
    if(kills[moSkeleton]) tab[cnt++] = laRuins;
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
    }
  
  if(killtypes() >= R20) {
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

vector<eLand> land_over = {
  laIce, laCaves, laDesert, laHunting, laMotion, laJungle, laAlchemist, 
  laCrossroads, 
  laMirror, laMirrorOld, laMinefield, laPalace, laPrincessQuest, laZebra, laSwitch, laReptile, 
  laOcean, laDocks, laWarpCoast, laLivefjord, laKraken, laCaribbean, laWhirlpool, laRlyeh, laTemple,
  laIvoryTower, laEndorian, laDungeon, laMountain, 
  laCrossroads2, 
  laDryForest, laWineyard, laDeadCaves, laGraveyard, laHaunted, laHive, 
  laRedRock, laVolcano,
  laDragon, laTortoise,
  laOvergrown, laClearing, laStorms, laBurial, laWhirlwind, 
  laBlizzard,
  laRuins, laEmerald, laCamelot, 
  laPrairie, laBull, laTerracotta, laRose,
  laElementalWall, laTrollheim,
  laHell, laCrossroads3, laCocytus, laPower, laCrossroads4,
  laCrossroads5,
  // EXTRA
  laWildWest, laHalloween, laDual, laSnakeNest, laMagnetic, laCA
  };

vector<eLand> landlist;

template<class T> void generateLandList(T t) {
  landlist.clear();
  for(auto l: land_over) if(t(l)) landlist.push_back(l);    
  }

eLand getLandForList(cell *c) {
  eLand l = c->land;
  if(isElemental(l)) return laElementalWall;
  if(l == laWarpSea) return laWarpCoast;
  if(l == laMercuryRiver) return laTerracotta;
  if(l == laBarrier) return laCrossroads;
  if(l == laOceanWall) return laOcean;
  if(l == laPalace && princess::dist(cwt.c) < OUT_OF_PRISON)
    l = laPrincessQuest;
  // princess?
  return l;
  }

bool isLandIngame(eLand l) {
  if(isElemental(l)) l = laElementalWall;
  if(isLandValid(l) < 2)
    return false;
  if(l == laWildWest)
    return false;
  return true;
  }

// check if the given land should appear in lists
int isLandValid(eLand l) {

  // Random Pattern allowed only in some specific lands
  if(randomPatternsMode && !isRandland(l))
    return 0;

  if(isElemental(l)) {
    if(l != laElementalWall)
      return 0;
    // not good in Field quotient
    if(quotient == 2)
      return 0;
    if(weirdhyperbolic)
      return 0;
    // works nice on a big non-tetrahedron-based sphere
    if(sphere && S3 != 3 && gp::on)
      return 3;
    }
  
  // does not agree with the pattern
  if(l == laStorms && quotient == 2) 
    return 0;
  
  // pattern not implemented
  if(l == laStorms && S7 == 8) 
    return 1;
  
  // not enough space
  if(l == laStorms && nonbitrunc && elliptic) 
    return 0;

  if(l == laStorms && nonbitrunc && S3 == 3) 
    return 0;
  
  // mirrors do not work in gp
  if(among(l, laMirror, laMirrorOld) && gp::on)
    return 0;

  // available only in non-standard geometries
  if(l == laMirrorOld && !geometry)
    return 0;
  
  // available only in standard geometry
  if(l == laMirror && geometry)
    return 0; 
  
  // Halloween needs bounded world (can be big bounded)
  if(l == laHalloween && !bounded)
    return 0;
  
  // these don't appear in normal game, but do appear in special modes
  bool normalgame = !geometry && !tactic::on;  
  if((l == laWildWest || l == laDual) && normalgame)
    return 0;
  
  // Crystal World is designed for nice_dual geometries
  if(l == laDual && !has_nice_dual())
    return 0;
  
  if(l == laHaunted && chaosmode)
    return 0;
  
  // standard, non-PTM specific
  if(l == laCrossroads5 && tactic::on)
    return 0;
    
  // standard non-PTM non-chaos specific
  if((l == laCrossroads5 || l == laCrossroads2) && (geometry || chaosmode))
    return 0;
    
  // equidistant-based lands
  if(l == laDungeon || l == laEndorian || l == laIvoryTower || l == laMountain || l == laOcean) {
    // special construction
    if(chaosmode && l == laOcean) 
      return 2; 
    // no equidistants supported in chaos mode
    if(chaosmode) 
      return 0;
    // no equidistants supported in these geometries (big sphere is OK though)
    if(quotient || elliptic || smallsphere || torus) 
      return 0;
    // Yendorian only implemented in standard
    if(l == laEndorian && geometry)
      return 0;
    // special Euclidean implementations
    if(euclid && (l == laIvoryTower || l == laMountain || l == laOcean)) 
      return 2;
    // in other geometries, it works
    if(geometry)
      return 2;
    }
  
  // equidistant-based lands, but also implemented in Euclidean
  if((l == laIvoryTower || l == laMountain) && (!stdeuc || chaosmode)) {
    if(quotient || euclid || elliptic || smallsphere || chaosmode)  
      return 0; //CHECKMORE
    if(l == laDungeon) return 1;
    return 0;
    }
  
  if(l == laPrincessQuest && (!stdeuc || chaosmode || tactic::on))
    return 0;

  // works correctly only in some geometries
  if(l == laClearing)
    if(chaosmode || !(stdeuc || a38 || (a45 && !nonbitrunc) || (a47 && !nonbitrunc)) || gp::on)
      return 0;

  // does not work in non-bitrunc a4
  if(l == laOvergrown && a4 && nonbitrunc)
    return 0;

  // does not work in bounded either
  if(l == laOvergrown && bounded)
    return 0;
  
  // horocycle-based lands, not available in bounded geometries nor in Chaos mode
  if((l == laWhirlpool || l == laCamelot || l == laCaribbean || l == laTemple) && (bounded || chaosmode))
    return 0;
  
  if(chaosmode && isCrossroads(l))
    return 0;
  
  if(gp::on && !horo_ok() && isCyclic(l))
    return 0;
    
  // Temple and Hive has a special Chaos Mode variant, but they are still essentially unbounded
  if((l == laTemple || l == laHive) && bounded)
    return 0;
    
  // this pattern does not work on elliptic and small spheres
  if((l == laBlizzard || l == laVolcano) && elliptic && S7 < 5)
    return 0;
  
  // ... and it works in gp only partially
  if((l == laBlizzard || l == laVolcano) && gp::on)
    return 1;
  
  // Kraken does not really work on odd-sided cells;
  // a nice football pattern will solve the problem by forbidding the Kraken to go there
  // (but we do have to allow it in non-bitrunc standard)
  if(l == laKraken && (S7&1) && !has_nice_dual()) {
    if(!geometry) return 1;
    return 0;
    }
  
  // needs standard/Euclidean (needs vineyard pattern)
  if(l == laWineyard && !stdeuc)
    return 0;

  // ... fake wineyard pattern
  if(l == laWineyard && gp::on)
    return 1;
  
  // works in most spheres, Zebra quotient, and stdeuc
  if(l == laWhirlwind) {
    if(!(stdeuc || quotient == 1 || (S7 == 4 && !nonbitrunc) || (bigsphere && nonbitrunc && !elliptic)))
      return 0;
    if(gp::on)
      return 1;
    }
    
  // needs standard/Euclidean (needs fractal landscape)
  if(l == laTortoise && !stdeuc)
    return 0;
  
  // technical lands do not count
  if(l != laCA && isTechnicalLand(l))
    return 0;
  
  // only in bounded geometry, and not in PTM
  if(l == laCA && !bounded)
    return 0;

  if(l == laCA && tactic::on)
    return 0;
  
  // Dragon Chasm requires unbounded space [partial]
  if(l == laDragon && bounded)
    return 0;
  
  // Graveyard pattern does not work on non-bitrunc weird geometries
  if(l == laGraveyard) 
    return geosupport_graveyard();
  
  // Warped Coast does not work on non-bitrunc S3s (except standard heptagonal where we have to keep it)
  if(l == laWarpCoast && (S3==3) && !has_nice_dual()) {
    if(!geometry) return 1;
    return 0;
    }
  
  // laPower and laEmerald and laPalace -> [partial] in quotients and weirdhyperbolic
  if((l == laPower || l == laEmerald || l == laPalace) && !stdeuc && !bigsphere)
    return 1;

  if((l == laPower || l == laEmerald || l == laPalace) && gp::on)
    return 1;

  if(l == laDragon && !stdeuc)
    return 1;

  if(l == laTrollheim && quotient == 2)
    return 0;
  
  if(l == laTrollheim && !stdeuc)
    return 1;
  
  if(l == laReptile && (!stdeuc || nonbitrunc))
    return 1;
  
  if(l == laCrossroads && weirdhyperbolic) 
    return 0;
  
  if(l == laCrossroads && smallsphere) 
    return 0;
  
  if(l == laCrossroads3 && !stdeuc && !bigsphere)
    return 0;

  if(among(l, laCrossroads, laCrossroads2, laCrossroads3, laCrossroads5) && gp::on && hyperbolic)
    return 0;

  // Crossroads IV is great in weird hyperbolic
  if(l == laCrossroads4 && weirdhyperbolic)
    return 3;

  // OK in small bounded worlds, and in Euclidean  
  if(l == laCrossroads4 && !(stdeuc || smallbounded))
    return 0;

  if(l == laZebra && !(stdeuc || (a4 && nonbitrunc) || a46 || quotient == 1))
    return 0;
  
  if(l == laZebra && gp::on)
    return 1;
  
  if(l == laCrossroads3 && euclid)
    return 1; // because it is not accurate

  if(l == laPrairie) {
    if(gp::on) return 0;
    else if(stdeuc || (bigsphere && !nonbitrunc && !elliptic) || (quotient == 2)) ;
    else if(!bounded) return 1;
    else return 0;
    }
  
  if(l == laTerracotta && !stdeuc && !(bigsphere))
    return 1;

  // highlight Crossroads on Euclidean
  if(euclid && !torus && (l == laCrossroads || l == laCrossroads4))
    return 3; 
  
  // highlight Zebra-based lands on Zebra Quotient!
  if((l == laZebra || l == laWhirlwind || l == laStorms) && quotient == 1)
    return 3;  
  
  // highlight FP-based lands on Field Quotient!
  if((l == laPrairie || l == laVolcano || l == laBlizzard) && quotient == 2)
    return 3;  
  
  // these are highlighted whenever allowed
  if(l == laHalloween || l == laDual)
    return 3;
  
  if(l == laSnakeNest)
    return geosupport_threecolor() >= 2 ? 3 : 0;
  
  if(l == laDocks && !randomPatternsMode) {
    if(a38 && !gp::on) return 3;
    if(a38) return 1;
    return 0;
    }
  
  if(l == laStorms && torus) 
    return 3;
  
  if(l == laMagnetic)
    return 0;

  return 2;
  }

bool isLandValid2(eLand l) { return isLandValid(l) >= 2; }
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

