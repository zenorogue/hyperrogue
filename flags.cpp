// Hyperbolic Rogue
// implementation of various simple flags for lands, items, monsters, and walls

// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

bool isIcyLand(eLand l) {
  return l == laIce || l == laCocytus;
  }

bool isIcyLand(cell *c) {
  return isIcyLand(c->land);
  }

bool isGravityLand(eLand l) {
  return l == laIvoryTower || l == laEndorian;
  }

// watery

bool isWatery(cell *c) {
  return c->wall == waCamelotMoat || c->wall == waSea || c->wall == waLake;
  }

bool isChasmy(cell *c) {
  return c->wall == waChasm || c->wall == waSulphur || c->wall == waSulphurC;
  }

bool isWateryOrBoat(cell *c) {
  return isWatery(c) || c->wall == waBoat;
  }

bool isNoFlight(cell *c) {
  return 
    c->wall == waBoat || c->wall == waVineHalfA || c->wall == waVineHalfB ||
    c->wall == waStrandedBoat || c->wall == waTrunk;
  }

bool boatStrandable(cell *c) {
  return c->wall == waNone && (c->land == laLivefjord || c->land == laOcean);
  }

// monster/wall types

bool isFire(cell *w) {
  return w->wall == waFire || w->wall == waPartialFire || w->wall == waEternalFire;
  }

bool isThumper(eWall w) {
  return w == waThumperOff || w == waThumperOn;
  }

bool isThumper(cell *c) {
  return isThumper(c->wall);
  }

bool isActivable(cell *c) {
  return c->wall == waThumperOff || c->wall == waBonfireOff;
  }

bool hasTimeout(cell *c) {
  return c->wall == waThumperOn || c->wall == waFire || c->wall == waPartialFire ||
    c->wall == waTempWall || c->wall == waTempFloor || c->wall == waTempBridge;
  }

bool isMimic(eMonster m) {
  return m == moMirror || m == moMirage;
  }

bool isMimic(cell *c) { 
  return isMimic(c->monst);
  }

bool isPrincess(eMonster m) {
  return 
    m == moPrincess || m == moPrincessMoved ||
    m == moPrincessArmed || m == moPrincessArmedMoved;
  }

bool isGolemOrKnight(eMonster m) {
  return 
    m == moGolem  || m == moGolemMoved || 
    m == moKnight || m == moKnightMoved ||
    m == moTameBomberbird  || m == moTameBomberbirdMoved ||
    m == moMouse || m == moMouseMoved ||
    m == moFriendlyGhost ||
    isPrincess(m);
  }

bool isGolemOrKnight(cell *c) { return isGolemOrKnight(c->monst); }

bool isNonliving(eMonster m) {
  return 
    m == moMirage || m == moMirror || m == moGolem || m == moGolemMoved ||
    m == moZombie || m == moGhost || m == moShadow || m == moSkeleton ||
    m == moEvilGolem || m == moIllusion || m == moEarthElemental || 
    m == moWaterElemental;
  }

bool isMetalBeast(eMonster m) {
  return m == moMetalBeast || m == moMetalBeast2;
  }

bool isStunnable(eMonster m) {
  return m == moPalace || m == moFatGuard || m == moSkeleton || isPrincess(m) ||
    isMetalBeast(m) || m == moTortoise || isDragon(m);
  }

bool hasHitpoints(eMonster m) {
  return m == moPalace || m == moFatGuard || m == moVizier || isPrincess(m);
  }

bool isMountable(eMonster m) {
  return isWorm(m);
  }

bool isFriendly(eMonster m) {
  return isMimic(m) || isGolemOrKnight(m) || m == moIllusion;
  }

bool isFriendly(cell *c) { 
  if(items[itOrbDomination] && c->monst && sameMonster(c, cwt.c)) return true;
  return isFriendly(c->monst); 
  }

bool isBug(eMonster m) {
  return m >= moBug0 && m < moBug0+BUGCOLORS;
  }

bool isBug(cell *c) {
  return isBug(c->monst);
  }

bool isFriendlyOrBug(cell *c) { // or killable discord!
  // do not attack the stunned Princess
  if(isPrincess(c->monst) && c->stuntime) return false;
  return isFriendly(c) || isBug(c) || (c->monst && markOrb(itOrbDiscord) && !c->stuntime);
  }

bool isIvy(eMonster m) {
  return m == moIvyRoot || m == moIvyHead || m == moIvyBranch || m == moIvyWait ||
    m == moIvyNext || m == moIvyDead;
  }

bool isIvy(cell *c) {
  return isIvy(c->monst);
  }

bool isMonsterPart(eMonster m) {
  return m == moMutant || (isIvy(m) && m != moIvyRoot);
  }
  
bool isMutantIvy(eMonster m) {
  return m == moMutant;
  }

bool isBulletType(eMonster m) { 
  return 
    m == moBullet || m == moFlailBullet || 
    m == moFireball || m == moTongue || m == moAirball; 
  }

bool isMutantIvy(cell *c) { return isMutantIvy(c->monst); }

bool isDemon(eMonster m) {
  return m == moLesser || m == moLesserM || m == moGreater || m == moGreaterM;
  }

bool isDemon(cell *c) {
  return isDemon(c->monst);
  }

bool isWormHead(eMonster m) {
  return m == moWorm || m == moTentacle || m == moHexSnake || m == moDragonHead;
  }

bool isWorm(eMonster m) {
  return m == moWorm || m == moWormtail || m == moWormwait || 
    m == moTentacle || m == moTentacletail || m == moTentaclewait ||
    m == moTentacleEscaping || m == moTentacleGhost ||
    m == moHexSnake || m == moHexSnakeTail ||
    m == moDragonHead || m == moDragonTail;
  }

bool isWorm(cell *c) {
  return isWorm(c->monst);
  }

bool isWitch(eMonster m) {
  // evil golems don't count
  return m >= moWitch && m < moWitch+NUMWITCH-1;
  }

bool isOnCIsland(cell *c) {
  return (c->wall == waCIsland || c->wall == waCTree || c->wall == waCIsland2);
  }

bool isGhostable(eMonster m) {
  return m && !isFriendly(m) && !isIvy(m) && !isMultitile(m) && !isMutantIvy(m);
  }

bool cellUnstable(cell *c) {
  return (c->land == laMotion && c->wall == waNone) || c->wall == waTrapdoor;
  }

bool cellUnstableOrChasm(cell *c) {
  return 
    (c->land == laMotion && c->wall == waNone) || 
    c->wall == waChasm || c->wall == waTrapdoor;
  }

bool cellHalfvine(cell *c) {
  return c->wall == waVineHalfA || c->wall == waVineHalfB;
  }

bool isWarped(eLand l) {
  return l == laGridCoast || l == laGridSea;
  }

bool isElementalShard(eItem i) {
  return 
    i == itFireShard || i == itAirShard || i == itEarthShard || i == itWaterShard;
  }

eMonster elementalOf(eLand l) {
  if(l == laEFire) return moFireElemental;
  if(l == laEWater) return moWaterElemental;
  if(l == laEAir) return moAirElemental;
  if(l == laEEarth) return moEarthElemental;
  return moNone;
  }

int itemclass(eItem i) {
  if(i == 0) return -1;
  if(i < itKey || i == itFernFlower || 
    i == itWine || i == itSilver || i == itEmerald || i == itRoyalJelly || i == itPower ||
    i == itGrimoire || i == itPirate || i == itRedGem || i == itBombEgg ||
    i == itCoast || i == itWhirlpool || i == itPalace || i == itFjord ||
    i == itElemental || i == itZebra || i == itEdge ||
    i == itBounty || i == itFulgurite || i == itMutant || i == itLotus || i == itMutant2 ||
    i == itWindstone || i == itCoral || i == itRose ||
    i == itBabyTortoise || i == itDragon || i == itApple)
    return IC_TREASURE;
  if(i == itKey || i == itOrbYendor || i == itGreenStone || i == itHolyGrail || i == itCompass ||
    i == itSavedPrincess || isElementalShard(i) || i == itRevolver || i == itStrongWind) 
    return IC_OTHER;  
  return IC_ORB;
  }
  
bool isAlch(eWall w) {
  return w == waFloorA || w == waFloorB;
  }

bool isAlch(cell *c) { return isAlch(c->wall); }
  
bool isAlchAny(eWall w) {
  return w == waFloorA || w == waFloorB || w == waFloorC || w == waFloorD;
  }
  
bool isAlchAny(cell *c) { return isAlchAny(c->wall); }
  
bool realred(eWall w) {
  return w == waRed1 || w == waRed2 || w == waRed3;
  }

int snakelevel(eWall w) {
  if(w == waRed1 || w == waDeadfloor2 || w == waRubble || w == waGargoyleFloor || 
    w == waGargoyleBridge || w == waTempFloor || w == waTempBridge) 
    return 1;
  if(w == waRed2) return 2;
  if(w == waRed3) return 3;
  return 0;
  }

int snakelevel(cell *c) { return snakelevel(c->wall); }

bool isWall(cell *w) {
  if(w->wall == waNone || isAlchAny(w) || 
    w->wall == waCavefloor || w->wall == waFrozenLake || w->wall == waVineHalfA ||
    w->wall == waVineHalfB || w->wall == waDeadfloor || w->wall == waDeadfloor2 ||
    w->wall == waRubble || w->wall == waGargoyleFloor || w->wall == waGargoyleBridge ||
    w->wall == waTempFloor || w->wall == waTempBridge ||
    w->wall == waBoat || w->wall == waCIsland || w->wall == waCIsland2 || 
    w->wall == waRed1 || w->wall == waRed2 || w->wall == waRed3 ||
    w->wall == waMineUnknown || w->wall == waMineMine || w->wall == waMineOpen ||
    w->wall == waStrandedBoat || w->wall == waOpenGate || w->wall == waClosePlate ||
    w->wall == waOpenPlate || w->wall == waTrapdoor || w->wall == waGiantRug ||
    w->wall == waLadder || w->wall == waTrunk || w->wall == waSolidBranch ||
    w->wall == waWeakBranch || w->wall == waCanopy)
    return false;
  if(isWatery(w) || isChasmy(w) || isFire(w)) return false;
  return true;
  }

bool isAngryBird(eMonster m) {
  return m == moEagle || m == moAlbatross || m == moBomberbird || m == moGargoyle ||
    m == moWindCrow || m == moKestrel || m == moNighthawk;
  }

bool isBird(eMonster m) {
  return isAngryBird(m) || m == moTameBomberbird || m == moTameBomberbirdMoved;    
  }

bool normalMover(eMonster m) {
  return
    m == moYeti || m == moRanger || m == moGoblin || m == moTroll || m == moDesertman || 
    m == moMonkey || m == moZombie || m == moNecromancer || m == moCultist || 
    m == moLesser || m == moGreater || m == moRunDog || m == moPyroCultist || 
    m == moFireFairy || m == moCrystalSage || m == moHedge ||
    m == moVineBeast || m == moLancer || m == moFlailer ||
    m == moMiner || m == moDarkTroll || 
    (playerInPower() && (
      (isWitch(m) && m != moWitchGhost && m != moWitchWinter) || m == moEvilGolem
      )) ||
    m == moRedTroll || 
    m == moPalace || m == moFatGuard || m == moSkeleton || m == moVizier ||
    m == moFjordTroll || m == moStormTroll || m == moForestTroll ||
    m == moEdgeMonkey ||
    m == moFireElemental || m == moOrangeDog ||
    isMetalBeast(m) ||
    m == moOutlaw || m == moRedFox || m == moFalsePrincess || m == moRoseLady ||
    m == moRoseBeauty || m == moWolf ||
    m == moTortoise || m == moLemur;
  }

// from-to

bool isGhost(eMonster m) {
  return m == moGhost || m == moTentacleGhost || m == moFriendlyGhost;
  }

bool isGhostMover(eMonster m) {
  return m == moGhost || m == moGreaterShark || m == moTentacleGhost ||
    (playerInPower() && (m == moWitchGhost || m == moWitchWinter));
  }

bool isShark(eMonster m) {
  return m == moShark || m == moCShark || m == moGreaterShark;
  }

bool isSlimeMover(eMonster m) {
  return
    m == moSlime || m == moSeep || m == moShark ||
    m == moVineSpirit || m == moCShark || m == moParrot;
  }

bool isBlowableMonster(eMonster m) {
  return m && !(
    isWorm(m) || isIvy(m) || isMutantIvy(m) || isSlimeMover(m) || 
    m == moGhost || m == moGreaterShark ||
    m == moWaterElemental || m == moWitchGhost || isMimic(m)
    );
  }
  
bool isMultitile(eMonster m) {
  return isWorm(m) || isIvy(m) || isMutantIvy(m);
  }
  
bool isSlimeMover(cell *c) {
  return isSlimeMover(c->monst);
  }
  
int slimegroup(cell *c) {
  if(c->wall == waCavewall || c->wall == waDeadwall)
    return 1;
  if(isWatery(c))
    return 2;
  if(c->wall == waFloorA)
    return 3;
  if(c->wall == waFloorB)
    return 4;
  if(c->wall == waVinePlant || cellHalfvine(c))
    return 5;
  if(c->wall == waCTree)
    return 6;
  return 0;
  }

bool isLeader(eMonster m) {
  return m == moPirate || m == moCultistLeader || m == moViking || m == moRatling || m == moRatlingAvenger;
  }

bool isFlying(eMonster m) {
  return isBird(m) || isGhost(m) || m == moAirElemental || isDragon(m) ||
    (isFriendly(m) && checkOrb(m, itOrbGhost));
  }

bool survivesChasm(eMonster m) {
  return isFlying(m);
  }

bool ignoresPlates(eMonster m) {
  return m == moMouse || isFlying(m);
  }

bool itemBurns(eItem it) {
  return it && it != itOrbDragon && it != itOrbFire && it != itDragon;
  }

bool attackThruVine(eMonster m) {
  return m == moGhost || m == moVineSpirit || m == moFriendlyGhost || m == moTentacleGhost;
  }

bool attackNonAdjacent(eMonster m) {
  return m == moGhost || m == moFriendlyGhost || m == moTentacleGhost;
  }

bool isInactiveEnemy(cell *w, eMonster forwho) {
  if(w->monst == moWormtail || w->monst == moWormwait || w->monst == moTentacletail || w->monst == moTentaclewait || w->monst == moHexSnakeTail)
    return true;
  if(w->monst == moLesserM || w->monst == moGreaterM)
    return true;
  if(w->monst == moIvyRoot || w->monst == moIvyWait || w->monst == moIvyNext || w->monst == moIvyDead)
    return true;
  if(w->monst && ((forwho == moPlayer) ? realstuntime(w) : realstuntime(w) > 1) && !isFriendly(w))
    return true;
  return false;
  }

// forpc = true (for PC), false (for golems)
bool isActiveEnemy(cell *w, eMonster forwho) {
  if(((forwho == moPlayer) ? realstuntime(w) : realstuntime(w) > 1))
    return false;
  if(w->monst == moNone) return false;
  if(isFriendly(w)) return false;
  if(isInactiveEnemy(w, forwho)) return false;
  return true;
  }

bool isUnarmed(eMonster m) {
  return 
    m == moMouse || m == moMouseMoved || m == moPrincess || m == moPrincessMoved ||
    m == moCrystalSage;   
  }

bool isArmedEnemy(cell *w, eMonster forwho) {
  return w->monst != moCrystalSage && isActiveEnemy(w, forwho);
  }

bool isHive(eLand l) {
  return l == laHive;
  }
  
bool isIcyWall(cell *c) {
  return c->wall == waNone || c->wall == waIcewall || c->wall == waFrozenLake || c->wall == waLake;
  }

bool eternalFire(cell *c) {
  return c->land == laDryForest || c->land == laPower || c->land == laMinefield ||
    c->land == laEFire || c->land == laElementalWall;
  }
  
bool isCyclic(eLand l) {
  return 
    l == laWhirlpool || l == laTemple || l == laCamelot || l == laClearing;
  }

bool haveRangedOrb() {
  return 
    items[itOrbPsi] || items[itOrbDragon] || items[itOrbTeleport] ||
    items[itOrbIllusion] || items[itOrbTelekinesis] || items[itOrbAir] ||
    items[itOrbFrog] || items[itOrbSummon] || items[itOrbMatter] ||
    items[itRevolver] || items[itOrbStunning] || items[itStrongWind] ||
    items[itOrbDomination];
  }

bool isOffensiveOrb(eItem it) {
  return it == itOrbLightning || it == itOrbFlash || it == itOrbThorns ||
    it == itOrbDragon || it == itOrbStunning || 
    it == itOrbFreedom || it == itOrbPsi;
  }

bool isRangedOrb(eItem i) {
  return i == itOrbPsi || i == itOrbDragon || i == itOrbTeleport || i == itOrbIllusion ||
    i == itOrbTelekinesis || i == itOrbAir || i == itOrbFrog ||
    i == itOrbSummon || i == itOrbMatter || i == itRevolver || i == itOrbStunning ||
    i == itOrbDomination;
  }

bool isProtectionOrb(eItem i) {
  return i == itOrbWinter || i == itOrbShield || i == itOrbInvis || i == itOrbShell;
  }

bool isEmpathyOrb(eItem i) {
  return 
    i == itOrbFire || i == itOrbDigging || i == itOrbWinter ||
    i == itOrbUndeath || i == itOrbSpeed || i == itOrbShield ||
    i == itOrbGhost || i == itOrbInvis || i == itOrbThorns ||
    i == itOrbWater;
  }

bool isUtilityOrb(eItem i) {
  return i == itOrbSpeed || i == itOrbDigging || 
    i == itOrbSafety || i == itOrbTeleport || i == itOrbGhost ||
    i == itOrbPreserve || i == itOrbTelekinesis || 
    i == itOrbSummon || i == itOrbLuck || i == itOrbEnergy;
  }

bool isFriendOrb(eItem i) {
  return i == itOrbLife || i == itOrbFriend || i == itOrbDiscord || i == itOrbLove ||
    i == itOrbEmpathy || i == itOrbUndeath;
  }

bool isFriendlyGhost(eMonster m) {
  return m == moFriendlyGhost || (markEmpathy(itOrbGhost) && isFriendly(m));
  }

bool isDragon(eMonster m) { return m == moDragonHead || m == moDragonTail; }

bool survivesWater(eMonster m) {
  return 
    m == moShark || m == moGreaterShark || m == moCShark || 
    isGhost(m) || m == moWitchGhost ||
    isBird(m) || m == moWaterElemental || m == moAirElemental ||
    isWorm(m) || isIvy(m) || isDragon(m) ||
    m == moTortoise; // Tortoises and Ivies survive, but don't go through water
  }

bool survivesFire(eMonster m) {
  return
    isGhost(m) || m == moWitchWinter || m == moWitchGhost ||
    m == moBomberbird || m == moTameBomberbird || m == moTameBomberbirdMoved ||
    (isFriendly(m) && markOrb(itOrbWinter)) || isWorm(m) || m == moFireElemental ||
    isDragon(m);
  }

bool survivesMine(eMonster m) {
  return isFlying(m);
  }

bool survivesWall(eMonster m) {
  return isGhost(m);
  }

bool survivesThorns(eMonster m) {
  return isGhost(m) || m == moSkeleton;
  }

bool survivesFall(eMonster m) {
  return isBird(m) || m == moAirElemental || m == moSkeleton || isDragon(m);
  }

bool isThorny(eWall w) { return w == waRose; }

bool checkOrb(eMonster m1, eItem orb) {
  if(m1 == moPlayer) return markOrb(orb);
  if(isFriendly(m1)) return markEmpathy(orb);
  return false;
  }

bool ignoresSmell(eMonster m) {
  return 
    m == moHexSnake || isIvy(m) || isMutantIvy(m) || isGhostMover(m) || isSlimeMover(m) ||
    m == moRoseLady || checkOrb(m, itOrbSkunk) || checkOrb(m, itOrbGhost) || checkOrb(m, itOrbShield);
  }

