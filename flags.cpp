// Hyperbolic Rogue
// implementation of various simple flags for lands, items, monsters, and walls

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details


/*
rewritten
*/

// itemclass
// slimegroup (wall)

/*
      it == itOrbLove ? shLoveRing :
      isRangedOrb(it) ? shTargetRing :
      isOffensiveOrb(it) ? shSawRing :
      isFriendOrb(it) ? shPeaceRing :
      isUtilityOrb(it) ? shGearRing :
      isDirectionalOrb(it) ? shSpearRing :
      among(it, itOrb37, itOrbGravity) ? shHeptaRing :
      shRing;
*/

namespace hr {

const flagtype& classflag(eItem it) { return iinf[it].flags; }
const flagtype& classflag(eWall w) { return winf[w].flags; }
const flagtype& classflag(eMonster m) { return minf[m].flags; }
const flagtype& classflag(eLand l) { return linf[l].flags; }

#define ANYFLAGCHECK(name, cond, field, enum) bool name(enum w) { flagtype flag = classflag(w); return cond; } bool name(cell *c) { return name(c->field); }

#define MONFLAGCHECK(name, cond) ANYFLAGCHECK(name, cond, monst, eMonster)
#define WALLFLAGCHECK(name, cond) ANYFLAGCHECK(name, cond, wall, eWall)
#define ITEMFLAGCHECK(name, cond) ANYFLAGCHECK(name, cond, item, eItem)
#define LANDFLAGCHECK(name, cond) ANYFLAGCHECK(name, cond, land, eLand)

WALLFLAGCHECK(isWatery, flag & WF_WATER)
WALLFLAGCHECK(isBoat, flag & WF_BOAT)
WALLFLAGCHECK(isChasmy, flag & WF_CHASM)
WALLFLAGCHECK(isWateryOrBoat, flag & (WF_WATER | WF_BOAT))
WALLFLAGCHECK(isNoFlight, flag & WF_NOFLIGHT)
WALLFLAGCHECK(isFire, flag & WF_FIRE)
WALLFLAGCHECK(isThumper, flag & WF_THUMPER)
WALLFLAGCHECK(isActivable, flag & WF_ACTIVABLE)
WALLFLAGCHECK(hasTimeout, flag & WF_TIMEOUT)
WALLFLAGCHECK(isOnCIsland, flag & WF_CISLAND)
WALLFLAGCHECK(cellHalfvine, flag & WF_HALFVINE)
WALLFLAGCHECK(isAlch, flag & WF_ALCHEMY)
WALLFLAGCHECK(isAlchAny, flag & WF_ALCHEMY)
WALLFLAGCHECK(realred, flag & WF_RED)
WALLFLAGCHECK(isWall, flag & WF_WALL)
WALLFLAGCHECK(isPushable, flag & WF_PUSHABLE)
WALLFLAGCHECK(conegraphtype, flag & WF_CONE)
WALLFLAGCHECK(isStandardTree, flag & WF_STDTREE)
WALLFLAGCHECK(isGrave, flag & WF_GRAVE)
WALLFLAGCHECK(isReptile, flag & WF_REPTILE)
WALLFLAGCHECK(useHeatColoring, flag & WF_HEATCOLOR)

LANDFLAGCHECK(generateAll, flag & LF_GENERATE_ALL)
LANDFLAGCHECK(isIcyLand, flag & LF_ICY)
LANDFLAGCHECK(isGravityLand, flag & LF_GRAVITY)
LANDFLAGCHECK(isEquidLand, flag & LF_EQUI)
LANDFLAGCHECK(isWarped, flag & LF_WARPED)
LANDFLAGCHECK(isCyclic, flag & LF_CYCLIC)
LANDFLAGCHECK(isTechnicalLand, flag & LF_TECHNICAL)
LANDFLAGCHECK(is_mirrorland, flag & LF_MIRROR)
LANDFLAGCHECK(isSealand, flag & LF_SEA)
LANDFLAGCHECK(isCoastal, flag & LF_COASTAL)
LANDFLAGCHECK(isPureSealand, flag & LF_PURESEA)
LANDFLAGCHECK(isElemental, flag & LF_ELEMENTAL)
LANDFLAGCHECK(isHaunted, flag & LF_HAUNTED)
LANDFLAGCHECK(isTrollLand, flag & LF_TROLL)
LANDFLAGCHECK(inmirror, flag & LF_INMIRROR)
LANDFLAGCHECK(inmirrororwall, flag & LF_INMIRRORORWALL)

MONFLAGCHECK(isGhostable, !(flag & MF_NOGHOST))
MONFLAGCHECK(isRaider, flag & MF_RAIDER)
MONFLAGCHECK(isMimic, flag & MF_MIMIC)
MONFLAGCHECK(isPrincess, flag & MF_PRINCESS)
MONFLAGCHECK(isGolemOrKnight, flag & MF_GOK)
MONFLAGCHECK(isNonliving, flag & MF_NONLIVING)
MONFLAGCHECK(isMetalBeast, flag & MF_METAL)
MONFLAGCHECK(isStunnable, flag & MF_STUNNABLE)
MONFLAGCHECK(hasHitpoints, flag & MF_HP)
MONFLAGCHECK(isMountable, flag & MF_MOUNTABLE)
MONFLAGCHECK(isFriendlyType, flag & MF_FRIENDLY)
MONFLAGCHECK(isFriendlyOrPlayer, flag & (MF_FRIENDLY | MF_PLAYER))
MONFLAGCHECK(isBug, flag & MF_BUG)
MONFLAGCHECK(isIvy, flag & MF_IVY)
MONFLAGCHECK(isMonsterPart, flag & MF_PART)
MONFLAGCHECK(isMutantIvy, flag & MF_MUTANTIVY)
MONFLAGCHECK(isAnyIvy, flag & MF_ANYIVY)
MONFLAGCHECK(isBulletType, flag & MF_BULLET)
MONFLAGCHECK(isDemon, flag & MF_DEMON)
MONFLAGCHECK(isWorm, flag & MF_WORM)
MONFLAGCHECK(isWitch, flag & MF_WITCH)
MONFLAGCHECK(isAngryBird, (flag & MF_BIRD) && !(flag & MF_FRIENDLY))
MONFLAGCHECK(isBird, flag & MF_BIRD)
MONFLAGCHECK(slowMover, flag & MF_SLOWMOVER)
MONFLAGCHECK(isMagneticPole, flag & MF_MAGNETIC)
MONFLAGCHECK(isSwitch, flag & MF_SWITCH)
MONFLAGCHECK(isGhost, flag & MF_GHOST)
MONFLAGCHECK(isShark, flag & MF_SHARK)
MONFLAGCHECK(isSlimeMover, flag & MF_SLIME)
MONFLAGCHECK(isDragon, flag & MF_DRAGON)
MONFLAGCHECK(isKraken, flag & MF_KRAKEN)
MONFLAGCHECK(isBlowableMonster, !(flag & MF_NOBLOW))
MONFLAGCHECK(isMultitile, flag & MF_MULTITILE)
MONFLAGCHECK(isLeader, flag & MF_LEADER)
MONFLAGCHECK(isFlyingType, flag & MF_FLYING)
MONFLAGCHECK(attackThruVine, flag & MF_ATTACK_THRU_VINE)
MONFLAGCHECK(attackNonAdjacent, flag & MF_ATTACK_NONADJACENT)
MONFLAGCHECK(noHighlight, flag & MF_NOHIGHLIGHT)
MONFLAGCHECK(isInactiveEnemyType, flag & MF_INACTIVE)
MONFLAGCHECK(isUnarmed, flag & MF_UNARMED)
MONFLAGCHECK(ignoresPlatesType, flag & MF_IGNORE_PLATE)
MONFLAGCHECK(isBull, flag & MF_BULL)
MONFLAGCHECK(isTroll, flag & MF_TROLL)
MONFLAGCHECK(ignoresSmellType, flag & MF_IGNORE_SMELL)
MONFLAGCHECK(isRatling, flag & MF_RATLING)
MONFLAGCHECK(isGhostMover, flag & MF_GHOSTMOVER)
MONFLAGCHECK(isPowerMonster, flag & MF_POWER)

ITEMFLAGCHECK(isElementalShard, flag & IF_SHARD)
ITEMFLAGCHECK(itemBurns, !(flag & IF_FIREPROOF))
ITEMFLAGCHECK(isProtectionOrb, flag & IF_PROTECTION)
ITEMFLAGCHECK(isEmpathyOrb, flag & IF_EMPATHY)
ITEMFLAGCHECK(isRangedOrb, flag & IF_RANGED)

eMonster movegroup(eMonster m);

// watery

bool boatStrandable(cell *c) {
  return c->wall == waNone && (c->land == laLivefjord || c->land == laOcean);
  }

// monster/wall types

bool isFireOrMagma(cell *w) {
  return isFire(w) || w->wall == waMagma;
  }

int mirrorcolor(bool mirrored) {
  return winf[mirrored ? waMirror : waCloud].color;
  }

bool isMounted(cell *c) {
  if(c && c->monst && c->monst != moTentacleGhost && isMountable(c->monst)) {
    for(int i=0; i<numplayers(); i++) {
      if(playerpos(i)->monst && sameMonster(c, playerpos(i))) 
        return true;
      if(lastmountpos[i] && lastmountpos[i]->monst && sameMonster(c, lastmountpos[i]))
        return true;
      }
    }
  return false;
  }

int itemclass(eItem it) { return iinf[it].itemclass; }

bool isFriendly(eMonster m) { return isFriendlyType(m); }

bool isFriendly(cell *c) { 
  return isMounted(c) || isFriendly(c->monst); 
  }

bool isFriendlyOrBug(cell *c) { // or killable discord!
  // do not attack the stunned Princess
  if(isPrincess(c->monst) && c->stuntime) return false;
  return isFriendly(c) || isBug(c) || (c->monst && markOrb(itOrbDiscord) && !c->stuntime);
  }

bool cellUnstable(cell *c) {
  return (c->land == laMotion && c->wall == waNone) || c->wall == waTrapdoor;
  }

bool cellUnstableOrChasm(cell *c) {
  return 
    (c->land == laMotion && c->wall == waNone) || 
    c->wall == waChasm || c->wall == waTrapdoor;
  }

eMonster elementalOf(eLand l) {
  if(l == laEFire) return moFireElemental;
  if(l == laEWater) return moWaterElemental;
  if(l == laEAir) return moAirElemental;
  if(l == laEEarth) return moEarthElemental;
  return moNone;
  }

eItem localshardof(eLand l) {
  return eItem(itFireShard + (l - laEFire));
  }

int snakelevel(cell *c) { 
#if CAP_COMPLEX2
  if(c->land == laBrownian && among(c->wall, waNone, waMineMine, waFire)) return min(c->landparam / brownian::level, 3);
#endif
  return winf[c->wall].snakelevel;
  }

// from-to

eSlimegroup slimegroup(cell *c) {
  return winf[c->wall].sg;
  }

bool isFlying(eMonster m) {
  return isFlyingType(m) || checkOrb(m, itOrbAether);
  }

bool survivesChasm(eMonster m) {
  return isFlying(m);
  }

bool ignoresPlates(eMonster m) {
  return ignoresPlatesType(m) || isFlying(m);
  }

bool isInactiveEnemy(cell *w, eMonster forwho) {
  if(isInactiveEnemyType(w->monst)) return true;
  if(w->monst && ((forwho == moPlayer) ? realstuntime(w) : realstuntime(w) > 1) && !isFriendly(w))
    return true;
  return false;
  }

// forpc = true (for PC), false (for golems)
bool isActiveEnemy(cell *w, eMonster forwho) {
  if(((forwho == moPlayer) ? realstuntime(w) : realstuntime(w) > 1))
    return false;
  if(w->monst == passive_switch) return false;
  if(w->monst == moNone) return false;
  if(isFriendly(w)) return false;
  if(isInactiveEnemy(w, forwho)) return false;
  return true;
  }

bool isArmedEnemy(cell *w, eMonster forwho) {
  return w->monst != moCrystalSage && w->monst != moCrusher && isActiveEnemy(w, forwho);
  }

bool isHive(eLand l) {
  return l == laHive;
  }
  
bool eternalFire(cell *c) {
  return c->land == laDryForest || (c->land == laPower && !smallbounded) || c->land == laMinefield ||
    c->land == laEFire || c->land == laElementalWall;
  }
  
bool haveRangedOrb() {
  return 
    items[itOrbPsi] || items[itOrbDragon] || items[itOrbTeleport] ||
    items[itOrbIllusion] || items[itOrbSpace] || items[itOrbAir] ||
    items[itOrbFrog] || items[itOrbSummon] || items[itOrbMatter] ||
    items[itRevolver] || items[itOrbStunning] || items[itStrongWind] ||
    items[itOrbDomination] || items[itOrbNature] || items[itOrbDash] ||
    items[itOrbMorph] || items[itOrbPhasing];
  }

bool isRevivalOrb(eItem i) {
  return i == itOrbLife || i == itOrbFriend || i == itOrbUndeath;
  }

bool isFriendlyGhost(eMonster m) {
  return m == moFriendlyGhost || (markEmpathy(itOrbAether) && isFriendly(m));
  }

bool survivesWater(eMonster m) {
  return 
    m == moShark || m == moGreaterShark || m == moCShark || 
    isGhost(m) || m == moWitchGhost || m == moShadow ||
    isBird(m) || m == moWaterElemental || m == moAirElemental ||
    isWorm(m) || isIvy(m) || isDragon(m) || isKraken(m) ||
    m == moMutant || m == moFriendlyIvy || 
    (isFriendly(m) && markOrb(itOrbFish)) ||
    m == moTortoise; // Tortoises and Ivies survive, but don't go through water
  }

// survives Mercury or Sulphur or Lava
bool survivesPoison(eMonster m, eWall p) {
  return
    isGhost(m) || m == moWitchGhost || m == moShadow ||
    isBird(m) || m == moAirElemental || isDragon(m) || isWorm(m);
  }

// flying even if stunned
bool isPermanentFlying(eMonster m) {
  return m == moAirElemental || isGhost(m);
  }

bool survivesFire(eMonster m) {
  return
    isGhost(m) || m == moWitchWinter || m == moWitchGhost ||
    m == moBomberbird || m == moTameBomberbird || m == moTameBomberbirdMoved ||
    (isFriendly(m) && markOrb(itOrbWinter)) || m == moFireElemental ||
    isDragon(m) || m == moShadow;
  }

bool survivesWall(eMonster m) {
  return isGhost(m);
  }

bool survivesThorns(eMonster m) {
  return isGhost(m) || m == moSkeleton || m == moDraugr;
  }

bool survivesFall(eMonster m) {
  return isBird(m) || m == moAirElemental || m == moSkeleton || isDragon(m) || m == moShadow || isGhost(m);
  }

bool isThorny(eWall w) { return w == waRose; }

bool checkOrb(eMonster m1, eItem orb) {
  if(m1 == moPlayer) return markOrb(orb);
  if(isFriendly(m1)) return markEmpathy(orb);
  return false;
  }

bool checkOrb2(eMonster m1, eItem orb) {
  if(m1 == moPlayer) return markOrb2(orb);
  if(isFriendly(m1)) return markEmpathy2(orb);
  return false;
  }

bool ignoresSmell(eMonster m) {
  return 
    m == moHexSnake || isIvy(m) || isMutantIvy(m) || isGhostMover(m) || isSlimeMover(m) ||
    m == moRoseLady || checkOrb(m, itOrbBeauty) || checkOrb(m, itOrbAether) || checkOrb(m, itOrbShield);
  }

bool highwall(cell *c) {
  if(c->wall == waGlass) return false;
  if(wmescher && wmspatial && c->wall == waBarrier && c->land == laOceanWall)
    return false;
  // if(wmspatial && isTree(c)) return false;
  if(isGrave(c->wall)) return true;
  if(c->wall == waMirrorWall) return false;
  if(c->wall == waEditStatue) return false;
  return winf[c->wall].glyph == '#' || c->wall == waClosedGate;
  }

int chasmgraph(cell *c) {
  if(c->wall == waChasm || c->wall == waInvisibleFloor) return 2;
  if(isChasmy(c)) return 1;
  if(isWateryOrBoat(c)) return 1;
  if(wmescher && c->wall == waBarrier && c->land == laOceanWall) return 1;
  if(c->wall == waReptileBridge) return 1;
  return 0;
  }

bool conegraph(cell *c) {
  return wmescher && wmspatial && conegraphtype(c);
  (c->wall == waDune || c->wall == waBigTree || c->wall == waSmallTree || c->wall == waCTree || (c->wall == waBarrier && c->land == laOceanWall));
  }

bool hornStuns(cell *c) {
  eMonster m = c->monst;
  return 
    m == moRagingBull || m == moSleepBull || m == moHerdBull ||
    m == moButterfly || m == moGreater || m == moGreaterM || m == moDraugr ||
    m == moHedge || m == moFlailer || m == moVizier || m == moReptile || m == moSalamander || 
    m == moPair || m == moAltDemon || m == moHexDemon || m == moMonk || m == moCrusher ||
    attackJustStuns(c, AF_NORMAL, moNone);
  }


}
