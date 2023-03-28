// Hyperbolic Rogue - Map effects
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file mapeffects.cpp
 *  \brief Routines handling the map effects
 */

#include "hyper.h"
namespace hr {

EX void initcell(cell *c) {
  c->mpdist = INFD;   // minimum distance from the player, ever
  c->cpdist = INFD;   // current distance from the player
  c->pathdist = PINFD;// current distance from the player, along paths (used by yetis)
  c->landparam = 0; c->landflags = 0; c->wparam = 0;
  c->listindex = -1;
  c->wall  = waNone;
  c->item  = itNone;
  c->monst = moNone;
  c->bardir = NODIR;
  c->mondir = NODIR;
  c->barleft = c->barright = laNone;
  c->land = laNone;
  c->ligon = 0;
  c->stuntime = 0;
  c->monmirror = 0;
  }

EX bool doesnotFall(cell *c) {
  changes.ccell(c);
  if(c->wall == waChasm) return false;
  else if(cellUnstable(c) && !in_gravity_zone(c)) {
    fallingFloorAnimation(c);
    c->wall = waChasm;
    return false;
    }
  return true;
  }

EX bool doesFall(cell *c) { return !doesnotFall(c); }

EX bool doesFallSound(cell *c) { 
  if(c->land != laMotion && c->land != laZebra)
    playSound(c, "trapdoor");  
  return !doesnotFall(c); 
  }

EX void destroyBoats(cell *c, cell *c2, bool strandedToo) {
  changes.ccell(c);
  if(c->wall == waBoat) placeWater(c, c2);
  if(strandedToo && c->wall == waStrandedBoat) c->wall = waNone;
  shmup::destroyBoats(c);
  }

#if HDR
enum eGravity { gsNormal, gsLevitation, gsAnti };
#endif
EX eGravity gravity_state, last_gravity_state;

EX bool bird_disruption(cell *c) {
  return c->cpdist <= 5 && items[itOrbGravity];
  }

EX bool in_gravity_zone(cell *c) {
  return gravity_state && c->cpdist <= 5;
  }

EX int gravity_zone_diff(cell *c) {
  if(in_gravity_zone(c)) {
    if(gravity_state == gsLevitation) return 0;
    if(gravity_state == gsAnti) return -1;
    }
  return 1;
  }

EX bool isJWall(cell *c) {
  return isWall(c) || c->monst == passive_switch; 
  }

EX eGravity get_static_gravity(cell *c) {
  if(isGravityLand(c->land))
    return gsLevitation;
  if(among(c->wall, waArrowTrap, waFireTrap, waClosePlate, waOpenPlate, waTrapdoor))
    return gsNormal;
  forCellEx(c2, c) if(isJWall(c2))
    return gsAnti;
  if(isWatery(c) || isChasmy(c) || among(c->wall, waMagma, waMineUnknown, waMineMine, waMineOpen))
    return gsLevitation;
  return gsNormal;
  }

EX eGravity get_move_gravity(cell *c, cell *c2) {
  if(isGravityLand(c->land) && isGravityLand(c2->land)) {
    int d = gravityLevelDiff(c, c2);
    if(d > 0) return gsNormal;
    if(d == 0) return gsLevitation;
    if(d < 0) return gsAnti;
    return gsNormal;
    }
  else {
    if(snakelevel(c) != snakelevel(c2)) {
      int d = snakelevel(c2) - snakelevel(c);
      if(d > 0) return gsAnti;
      if(d == -3) return gsLevitation;
      return gsNormal;
      }
    forCellEx(c3, c) if(isJWall(c3))
      return gsAnti;
    forCellEx(c3, c2) if(isJWall(c3))
      return gsAnti;
    if(isWatery(c2) && c->wall == waBoat && !againstCurrent(c2, c))
      return gsNormal;
    if(isWatery(c2) || isChasmy(c2) || among(c2->wall, waMagma, waMineUnknown, waMineMine, waMineOpen) || anti_alchemy(c2, c))
      return gsLevitation;
    return gsNormal;
    }
  }

EX bool isWarped(cell *c) {
  return isWarpedType(c->land) || (c->land == laCanvas && canvasfloor == caflWarp) || (!inmirrororwall(c->land) && (items[itOrb37] && c->cpdist <= 4));
  }

EX bool nonAdjacent(cell *c, cell *c2) {
  if(isWarped(c) && isWarped(c2) && warptype(c) == warptype(c2)) {
    /* int i = neighborId(c, c2);
    cell *c3 = c->modmove(i+1), *c4 = c->modmove(i-1);
    if(c3 && !isTrihepta(c3)) return false;
    if(c4 && !isTrihepta(c4)) return false; */
    return true;
    }
  return false;
  }

EX bool nonAdjacentPlayer(cell *c, cell *c2) {
  return nonAdjacent(c, c2) && !markOrb(itOrb37);
  }

EX bool thruVine(cell *c, cell *c2) {
  return (cellHalfvine(c) && c2->wall == c->wall && c2 != c);
  // ((c->wall == waFloorC || c->wall == waFloorD) && c2->wall == c->wall && !c2->item && !c->item);
  }

EX void useup(cell *c) {
  changes.ccell(c);
  c->wparam--;
  if(c->wparam == 0) {
    drawParticles(c, c->wall == waFire ? 0xC00000 : winf[c->wall].color, 10, 50);
    if(c->wall == waTempFloor)
      c->wall = waChasm;
    else if(c->wall == waTempBridge || c->wall == waTempBridgeBlocked || c->wall == waBurningDock || c->land == laBrownian)
      placeWater(c, c);
    else {
      c->wall = c->land == laCaribbean ? waCIsland2 : waNone;
      }
    }
  }

EX bool earthFloor(cell *c) {
  changes.ccell(c);
  if(c->monst) return false;
  if(c->wall == waDeadwall) { c->wall = waDeadfloor; return true; }
  if(c->wall == waDune) { c->wall = waNone; return true; }
  if(c->wall == waStone && c->land != laTerracotta) { c->wall = waNone; return true; }
  if(c->wall == waAncientGrave || c->wall == waFreshGrave || c->wall == waRuinWall) {
    c->wall = waNone;
    return true;
    }
  if(c->land == laWet && among(c->wall, waDeepWater, waShallow, waStone)) {
    c->wall = waNone;
    return true;
    }
  if((c->wall == waSea || c->wall == waNone) && c->land == laOcean) {
    c->wall = waCIsland;
    return true;
    }
  if(c->wall == waSea && c->land == laCaribbean) {
    c->wall = waCIsland;
    return true;
    }
  if(c->wall == waSea && c->land == laWarpSea)
    c->wall = waNone;
  if(c->wall == waBoat && c->land == laWarpSea)
    c->wall = waStrandedBoat;
  if(c->wall == waMercury) {
    c->wall = waNone;
    return true;
    }
  if((c->wall == waBarrowDig || c->wall == waBarrowWall) && c->land == laBurial) {
    c->item = itNone;
    c->wall = waNone;
    return true;
    }
  if(c->wall == waPlatform && c->land == laMountain) {
    c->wall = waNone;
    return true;
    }
  if(c->wall == waChasm && c->land == laHunting) {
    c->wall = waNone;
    return true;
    }
  if(c->land == laCursed && among(c->wall, waDeepWater, waShallow)) {
    c->wall = waNone;
    return true;
    }
  return false;
  }

EX bool earthWall(cell *c) {
  changes.ccell(c);
  if(c->wall == waDeadfloor || c->wall == waDeadfloor2 || c->wall == waEarthD) {
    c->item = itNone;
    c->wall = waDeadwall;
    return true;
    }
  if(c->land == laWet && among(c->wall, waDeepWater, waShallow, waNone)) {
    c->wall = waStone;
    return true;
    }
  if(c->wall == waNone && c->land == laFrog)
    c->wall = waStone;
  if(c->wall == waNone && c->land == laEclectic)
    c->wall = waDeadwall;
  if(c->wall == waNone && c->land == laMountain) {
    c->wall = waPlatform;
    return true;
    }
  if(c->wall == waNone && c->land == laDesert) {
    c->item = itNone;
    c->wall = waDune;
    return true;
    }
  if(c->wall == waNone && c->land == laRuins) {
    c->item = itNone;
    c->wall = waRuinWall;
    return true;
    }
  if(c->wall == waNone && isElemental(c->land)) {
    c->item = itNone;
    c->wall = waStone;
    return true;
    }
  if(c->wall == waNone && c->land == laRedRock) {
    c->item = itNone;
    c->wall = waRed3;
    return true;
    }
  if(c->wall == waNone && c->land == laSnakeNest) {
    c->item = itNone;
    c->wall = waRed3;
    return true;
    }
  if(c->wall == waNone && c->land == laBurial) {
    c->item = itNone;
    c->wall = waBarrowDig;
    return true;
    }
  if(c->wall == waNone && c->land == laHunting) {
    c->item = itNone;
    c->wall = waChasm;
    return true;
    }
  if(c->wall == waNone && c->land == laTerracotta) {
    c->wall = waMercury;
    return true;
    }
  if(c->wall == waArrowTrap && c->land == laTerracotta) {
    destroyTrapsOn(c);
    c->wall = waMercury;
    return true;
    }
  if(c->land == laCursed && among(c->wall, waNone, waShallow)) {
    c->wall = waSea;
    return true;
    }
  if(c->wall == waCIsland || c->wall == waCIsland2 || (c->wall == waNone && c->land == laOcean)) {
    c->item = itNone;
    c->wall = waSea;
    if(c->land == laOcean) c->landparam = 40;
    return true;
    }
  return false;
  }

EX bool snakepile(cell *c, eMonster m) {
  changes.ccell(c);
  if(c->wall == waSea && c->land == laOcean) {
    c->land = laBrownian, c->landparam = 0;
    }
  if(c->land == laWestWall) return false;
  if(c->land == laBrownian) {
    if(c->wall == waNone) {
      #if CAP_COMPLEX2
      c->landparam += brownian::level;
      #endif
      return true;
      }
    if(c->wall == waSea || c->wall == waBoat) {
      c->wall = waNone;
      c->landparam++;
      return true;
      }
    }
  if(c->item && c->wall != waRed3) c->item = itNone;
  if(c->wall == waRed1 || c->wall == waOpenGate) c->wall = waRed2;
  else if(c->wall == waRed2) c->wall = waRed3;
  else if(doesFall(c)) return false;
  else if((c->wall == waSea && c->land == laLivefjord))
    c->wall = waNone;
  else if((c->wall == waSea && isWarpedType(c->land)))
    c->wall = waShallow;
  else if(isGravityLand(c->land)) {
    if(m == moHexSnake)
      c->wall = waPlatform;
    else
      c->wall = waDeadTroll2;
    }
  else if(c->wall == waNone || isAlchAny(c) ||
    c->wall == waCIsland || c->wall == waCIsland2 ||
    c->wall == waOpenPlate || c->wall == waClosePlate ||
    c->wall == waMineUnknown || c->wall == waMineOpen || isReptile(c->wall)) {
    if(isReptile(c->wall)) kills[moReptile]++;
    c->wall = waRed1;
    if(among(m, moDarkTroll, moBrownBug)) c->wall = waDeadfloor2;
    }
  else if(c->wall == waDeadfloor)
    c->wall = waDeadfloor2; 
  else if(c->wall == waDeadfloor2) {
    if(m == moDarkTroll && c->land == laDeadCaves) return false;
    else
      c->wall = waDeadwall; 
    }
  else if(c->wall == waRubble || c->wall == waGargoyleFloor || c->wall == waGargoyleBridge ||
    c->wall == waTempFloor || c->wall == waTempBridge || c->wall == waPetrifiedBridge) {
    if(c->land == laWhirlpool) return false;
    c->wall = waRed2; 
    if(m == moDarkTroll) c->wall = waDeadwall;
    }
  else if(c->wall == waCavefloor) c->wall = waCavewall;
  else if(c->wall == waSea && c->land == laCaribbean) c->wall = waShallow;
  else if(c->wall == waSea && c->land == laWhirlpool) return false;
  else if(c->wall == waSea) c->wall = waShallow;
  else if(c->wall == waDeepWater) c->wall = waShallow;
  else if(c->wall == waShallow) c->wall = waNone;
  else if(c->wall == waLake) c->wall = waShallow;
  else if(isWateryOrBoat(c) || c->wall == waFrozenLake) c->wall = waNone;
  else if(isWateryOrBoat(c) || c->wall == waFrozenLake) c->wall = waNone;
  else if(cellHalfvine(c)) {
    destroyHalfvine(c, waRed1);
    if(c->wall == waRed1 && m == moDarkTroll) c->wall = waDeadfloor2;
    }
  else return false;
  return true;
  }

EX bool makeflame(cell *c, int timeout, bool checkonly) {
  changes.ccell(c);
  if(!checkonly) destroyTrapsOn(c);
  if(itemBurns(c->item) && !isWatery(c) && c->wall != waShallow) {
    if(checkonly) return true;
    if(c->cpdist <= 7)
      addMessage(XLAT("%The1 burns!", c->item));
    c->item = itNone;
    }
  if(cellUnstable(c)) {
    if(checkonly) return true;
    doesFall(c);
    }
  else if(c->wall == waChasm || c->wall == waOpenGate || c->wall == waRed2 || c->wall == waRed3 ||
    c->wall == waTower)
    return false;
  else if(c->wall == waBoat) {
    if(isPlayerOn(c) && markOrb(itOrbWinter)) {
      if(!checkonly) addMessage(XLAT("%the1 protects your boat!", itOrbWinter));
      return true;
      }
    if(checkonly) return true;
    if(c->cpdist <= 7)
      addMessage(XLAT("%The1 burns!", winf[c->wall].name));
    drawFireParticles(c, 24);
    placeWater(c, c);
    if(isIcyLand(c)) HEAT(c) += 1;
    }
  else if(c->wall == waNone && c->land == laCocytus) {
    if(checkonly) return true;
    c->wall = waLake, HEAT(c) += 1;
    }
  else if(c->wall == waFireTrap) {
    if(checkonly) return true;
    if(c->wparam == 0) c->wparam = 1;
    }
  else if(c->wall == waFrozenLake) {
    if(checkonly) return true;
    drawParticles(c, MELTCOLOR, 8, 8);
    c->wall = waLake, HEAT(c) += 1;
    }
  else if(c->wall == waIcewall) {
    if(checkonly) return true;
    drawParticles(c, MELTCOLOR, 8, 8);
    c->wall = waNone;
    }
  else if(c->wall == waMineMine) {
    if(checkonly) return true;
    explodeMine(c);
    }
  else if(c->wall != waCTree && c->wall != waBigTree && c->wall != waSmallTree && 
    c->wall != waVinePlant && !passable(c, NULL, P_MONSTER | P_MIRROR) &&
    c->wall != waSaloon && c->wall != waRose) return false;
  // reptiles are able to use the water to put the fire off
  else if(c->wall == waReptileBridge) return false;
  else if(c->wall == waDock) {
    if(checkonly) return true;
    c->wall = waBurningDock;
    c->wparam = 3;
    return false;
    }
  else {
    eWall w = eternalFire(c) ? waEternalFire : waFire;
    if(!checkonly) drawFireParticles(c, 10);
    if(w == c->wall) return false;
    if(checkonly) return true;
    if(isReptile(c->wall)) kills[moReptile]++;
    destroyHalfvine(c);
    if(!isFire(c)) c->wparam = 0;
    c->wall = w;
    c->wparam = max(c->wparam, (char) timeout);
    if(c->land == laBrownian) c->landparam = 0;
    }
  return true;
  }

EX bool makeshallow(cell *c, int timeout, bool checkonly) {
  changes.ccell(c);
  if(!checkonly) destroyTrapsOn(c);
  #if CAP_COMPLEX2
  if(c->land == laBrownian) {
    if(checkonly) return true;
    brownian::dissolve(c, 1);
    }
  #endif
  if(c->wall == waChasm || c->wall == waOpenGate || c->wall == waRed2 || c->wall == waRed3 ||
    c->wall == waTower)
    return false;
  else if(c->wall == waNone && c->land == laCocytus) {
    if(checkonly) return true;
    c->wall = waLake;
    }
  else if(c->wall == waFireTrap) {
    if(checkonly) return true;
    c->wall = waShallow;
    }
  else if(c->wall == waFrozenLake) {
    if(checkonly) return true;
    drawParticles(c, MELTCOLOR, 8, 8);
    c->wall = waLake;
    }
  else if(isFire(c)) {
    if(checkonly) return true;
    c->wall = waNone;
    }
  else if(c->wall == waMineMine) {
    if(checkonly) return true;
    c->wall = waShallow;
    }
  else if(among(c->wall, waNone, waRubble, waDeadfloor2, waCavefloor, waDeadfloor, waFloorA, waFloorB) && !cellUnstable(c) && !isGravityLand(c)) {
    if(checkonly) return true;
    c->wall = waShallow;
    }
  else if(c->wall == waDock) {
    if(checkonly) return true;
    c->wall = waSea;
    c->wparam = 3;
    return false;
    }
  return true;
  }

EX void explosion(cell *c, int power, int central) {
  changes.ccell(c);
  playSound(c, "explosion");
  drawFireParticles(c, 30, 150);
  
  #if CAP_COMPLEX2
  brownian::dissolve_brownian(c, 2);
  #endif
  makeflame(c, central, false);
  
  forCellEx(c2, c) {
    changes.ccell(c2);
    destroyTrapsOn(c2);
    #if CAP_COMPLEX2
    brownian::dissolve_brownian(c2, 1);
    #endif
    if(c2->wall == waRed2 || c2->wall == waRed3)
      c2->wall = waRed1;
    else if(c2->wall == waDeadTroll || c2->wall == waDeadTroll2 || c2->wall == waPetrified || c2->wall == waGargoyle) {
      c2->wall = waNone;
      makeflame(c2, power/2, false);
      }
    else if(c2->wall == waPetrifiedBridge || c2->wall == waGargoyleBridge) {
      placeWater(c, c);
      }
    else if(c2->wall == waPalace || c2->wall == waOpenGate || c2->wall == waClosedGate ||
      c2->wall == waSandstone || c2->wall == waMetal || c2->wall == waSaloon || c2->wall == waRuinWall) {
      c2->wall = waNone;
      makeflame(c2, power/2, false);
      }
    else if(c2->wall == waTower)
      c2->wall = waRubble;
    else if(c2->wall == waBarrowWall)
      c2->wall = waBarrowDig;
    else if(c2->wall == waBarrowDig)
      c2->wall = waNone;
    else if(c2->wall == waFireTrap) {
      if(c2->wparam == 0)
        c2->wparam = 1;
      }
    else if(c2->wall == waExplosiveBarrel)
      explodeBarrel(c2);
    else makeflame(c2, power, false);
    }
  }

EX void explodeMine(cell *c) {
  if(c->wall != waMineMine)
    return;
  
  changes.ccell(c);
  c->wall = waMineOpen;
  explosion(c, 20, 20);
  #if CAP_COMPLEX2
  changes.at_commit(mine::auto_teleport_charges);
  #endif
  }

EX void explodeBarrel(cell *c) {
  if(c->wall != waExplosiveBarrel)
    return;
  
  changes.ccell(c);
  c->wall = waNone;  
  explosion(c, 20, 20);
  }

EX bool mayExplodeMine(cell *c, eMonster who) {
  if(c->wall != waMineMine) return false;
  if(ignoresPlates(who)) return false;
  explodeMine(c);
  return true;
  }

EX void flameHalfvine(cell *c, int val) {
  changes.ccell(c);
  if(itemBurns(c->item)) {
    addMessage(XLAT("%The1 burns!", c->item));
    c->item = itNone;
    }
  c->wall = waPartialFire;
  c->wparam = val;
  }

EX bool destroyHalfvine(cell *c, eWall newwall IS(waNone), int tval IS(6)) {
  if(cellHalfvine(c)) {
    changes.ccell(c);
    forCellEx(c1, c) if(c1->wall == c->wall) {
      changes.ccell(c1);
      if(newwall == waPartialFire) flameHalfvine(c1, tval);
      else if(newwall == waRed1) c1->wall = waVinePlant;
      else c1->wall = newwall;
      }
    if(newwall == waPartialFire) flameHalfvine(c, tval);
    else c->wall = newwall;
    return true;
    }
  return false;
  }

EX int coastvalEdge(cell *c) { return coastval(c, laIvoryTower); }

EX int gravityLevel(cell *c) {
  if(c->land == laIvoryTower || c->land == laEndorian)
    return coastval(c, laIvoryTower);
  if(c->land == laDungeon)
    return -coastval(c, laIvoryTower);
  if(c->land == laMountain)
    return 1-celldistAlt(c);
  return 0;
  }

EX int gravityLevelDiff(cell *c, cell *d) {
  if(c->land != laWestWall || d->land != laWestWall) {
    int res = gravityLevel(c) - gravityLevel(d);
    if(res > 1) return 1;
    if(res < -1) return -1;
    return res;
    }
  
  if(shmup::on) return 0;

  int nid = neighborId(c, d);
  int id1 = parent_id(c, 1, coastvalEdge) + 1;
  int di1 = angledist(c->type, id1, nid);

  int id2 = parent_id(c, -1, coastvalEdge) - 1;
  int di2 = angledist(c->type, id2, nid);
  
  if(di1 < di2) return 1;
  if(di1 > di2) return -1;
  return 0;
  }    

EX bool canUnstable(eWall w, flagtype flags) {
  return w == waNone || w == waCanopy || w == waOpenPlate || w == waClosePlate ||
    w == waOpenGate || ((flags & MF_STUNNED) && (w == waLadder || w == waTrunk || w == waSolidBranch || w == waWeakBranch
    || w == waBigBush || w == waSmallBush));
  }

EX bool cellEdgeUnstable(cell *c, flagtype flags IS(0)) {
  if(!isGravityLand(c->land) || !canUnstable(c->wall, flags)) return false;
  if(shmup::on && c->land == laWestWall) return false;
  forCellEx(c2, c) {
    if(isAnyIvy(c2->monst) && 
      c->land == laMountain && !(flags & MF_IVY)) return false;
    int d = gravityLevelDiff(c, c2);
    if(d == gravity_zone_diff(c)) {
      if(againstWind(c2, c)) return false;
      if(!passable(c2, NULL, P_MONSTER | P_DEADLY))
        return false;
      if(isWorm(c2))
        return false;
      }
    if(WDIM == 3) {
      if(d == 0 && !passable(c2, NULL, P_MONSTER | P_DEADLY)) return false;
      if(d == -1 && !passable(c2, NULL, P_MONSTER | P_DEADLY)) forCellEx(c3, c2) if(c3 != c && gravityLevelDiff(c3, c) == 0) return false;
      }
    }
  return true;
  }

int tidalphase;

EX int tidalsize, tide[200];

EX void calcTidalPhase() {
  if(!tidalsize) {
    for(int i=0; i<5; i++) tide[tidalsize++] = 1;

    for(int i=1; i<4; i++) for(int j=0; j<3; j++) 
      tide[tidalsize++] = i;

    for(int i=4; i<7; i++) for(int j=0; j<2; j++) 
      tide[tidalsize++] = i;

    for(int i=7; i<20; i++)
      tide[tidalsize++] = i;

    for(int i=20; i<23; i++) for(int j=0; j<2; j++) 
      tide[tidalsize++] = i;
    
    for(int i=23; i<26; i++) for(int j=0; j<3; j++) 
      tide[tidalsize++] = i;
    
    for(int i=0; i+i<tidalsize; i++) tide[tidalsize++] = 27 - tide[i];
    
    /* printf("tidalsize = %d\n", tidalsize);
    for(int i=0; i<tidalsize; i++) printf("%d ", tide[i]);
    printf("\n"); */
    }
  tidalphase = tide[
    (shmup::on ? shmup::curtime/600 : turncount)
    % tidalsize];
  if(peace::on)
    tidalphase = 5 + tidalphase / 6;
  }

EX int tidespeed() {
  return tide[(turncount+6) % tidalsize] - tidalphase;
  }

EX bool recalcTide;

#if HDR
#define SEADIST LHU.bytes[0]
#define LANDDIST LHU.bytes[1]
#define CHAOSPARAM LHU.bytes[2]
#endif

#if CAP_FIELD
EX int lavatide(cell *c, int t) {
  int tc = (shmup::on ? shmup::curtime/400 : turncount);
  return (windmap::at(c) + (tc+t)*4) & 255;
  }
#endif

EX void checkTide(cell *c) {
  if(c->land == laOcean) {
    int t = c->landparam;
    
    if(ls::any_chaos()) {
      char& csd(c->SEADIST); if(csd == 0) csd = 7;
      char& cld(c->LANDDIST); if(cld == 0) cld = 7;
      int seadist=csd, landdist=cld;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->move(i);
        if(!c2) continue;
        if(c2->land == laBarrier || c2->land == laOceanWall) ;
        else if(c2->land == laOcean) 
          seadist = min(seadist, c2->SEADIST ? c2->SEADIST+1 : 7),
          landdist = min(landdist, c2->LANDDIST ? c2->LANDDIST+1 : 7);
        else if(isSealand(c2->land)) seadist = 1;
        else landdist = 1;
        }
      if(seadist < csd) csd = seadist, recalcTide = true;
      if(landdist < cld) cld = landdist, recalcTide = true;
      if(seadist == 1 && landdist == 1) t = 15;
      else t = c->CHAOSPARAM = 1 + (29 * (landdist-1)) / (seadist+landdist-2);
      }
    
    if(c->wall == waStrandedBoat || c->wall == waBoat)
      c->wall = t >= tidalphase ? waBoat : waStrandedBoat;
    if(c->wall == waSea || c->wall == waNone)
      c->wall = t >= tidalphase ? waSea : waNone;
    if(isFire(c) && t >= tidalphase)
      c->wall = waSea;
    }
  #if CAP_FIELD
  if(c->land == laVolcano) {
    int id = lavatide(c, 0);
    if(id < 96) {
      if(c->wall == waNone || isWateryOrBoat(c) || c->wall == waVinePlant || isAlch(c)) {
        if(isWateryOrBoat(c) || isAlch(c)) 
          playSound(c, "steamhiss");
        c->wall = waMagma;
        if(itemBurns(c->item)) {
          addMessage(XLAT("%The1 burns!", c->item)), c->item = itNone;
          playSound(c, "steamhiss", 30);
          }
        }
      }
    else if(c->wall == waMagma) c->wall = waNone;
    }
  #endif
  }

EX bool makeEmpty(cell *c) {

  if(c->monst != moPrincess) {
    if(isAnyIvy(c->monst)) killMonster(c, moPlayer, 0);
    else if(c->monst == moPair) {
      if(c->move(c->mondir)->monst == moPair)
        c->move(c->mondir)->monst = moNone;
      }
    else if(isWorm(c->monst)) {
      if(!items[itOrbDomination]) return false;
      }
    else c->monst = moNone;
    }

  if(c->land == laCanvas) ;
  else if(c->land == laCocytus)
    c->wall = waFrozenLake;
  else if(c->land == laAlchemist || c->land == laCanvas)
    ;
  else if(c->land == laDual)
    ;
  else if(c->land == laCaves || c->land == laEmerald)
    c->wall = waCavefloor;
  else if(c->land == laDeadCaves)
    c->wall = waDeadfloor2;
  else if(c->land == laCaribbean || c->land == laOcean || c->land == laWhirlpool || c->land == laLivefjord || c->land == laWarpSea || c->land == laKraken || c->wall == waBoat)
    c->wall = waBoat; // , c->item = itOrbYendor;
  else if(c->land == laMinefield)
    c->wall = waMineOpen;
  else if(c->wall == waFan && closed_manifold)
    ;
  else if(c->wall == waOpenPlate && closed_manifold)
    ;
  else if(c->wall == waTrunk || c->wall == waSolidBranch || c->wall == waWeakBranch)
    ;
  else if(c->wall == waGiantRug)
    ;
  else if(c->wall == waInvisibleFloor)
    ;
  else if(c->wall == waDock)
    ;
  else if(c->wall == waLadder)
    ;
  else if(c->land == laDocks)
    c->wall = waBoat;
  else if(c->wall == waFreshGrave && closed_manifold)
    ;
  else if(c->wall == waBarrier && sphere && WDIM == 3)
    ;
  else if(isReptile(c->wall))
    c->wparam = reptilemax();
  else if(c->wall == waAncientGrave && closed_manifold)
    ;
  else if(c->wall != waRoundTable)
    c->wall = waNone;
  
  if(c->land == laBrownian && c->wall == waNone && c->landparam == 0)
    c->landparam = 1;
    
  if(c->item != itCompass)
    c->item = itNone;
  
  if(c->land == laEclectic) {
    c->wall = waNone;
    forCellEx(c1, c)
      c1->wall = waNone;
    }
  
  if(c->land == laWildWest) {
    forCellEx(c2, c)
    forCellEx(c3, c2)
      if(c3->wall != waBarrier)
        c3->wall = waNone;
    }
  
  return true;
  }

int numgates = 0;

EX void toggleGates(cell *c, eWall type, int rad) {
  if(!c) return;
  celllister cl(c, rad, 1000000, NULL);
  for(cell *ct: cl.lst) {
    if(ct->wall == waOpenGate && type == waClosePlate) {
      bool onWorm = false;
      if(isWorm(ct)) onWorm = true;
      for(int i=0; i<ct->type; i++) 
        if(ct->move(i) && ct->move(i)->wall == waOpenGate && isWorm(ct->move(i))) onWorm = true;
      if(!onWorm) {
        changes.ccell(ct);
        ct->wall = waClosedGate, numgates++;
        if(ct->item) {
          playSound(ct, "hit-crush"+pick123());
          addMessage(XLAT("%The1 is crushed!", ct->item));
          ct->item = itNone;
          }
        toggleGates(ct, type, 1);
        }
      }
    if(ct->wall == waClosedGate && type == waOpenPlate) {
      changes.ccell(ct);
      ct->wall = waOpenGate, numgates++;
      toggleGates(ct, type, 1);
      }
    }
  }

EX int toggle_radius(eWall type) {
  if(type == waClosePlate && PURE)
    return 2;
  else
    return (GOLDBERG && !sphere && !a4) ? gp::dist_3() : 3;
  }

EX void toggleGates(cell *ct, eWall type) {
  playSound(ct, "click");
  numgates = 0;
  toggleGates(ct, type, toggle_radius(type));
  if(numgates && type == waClosePlate)
    playSound(ct, "closegate");
  if(numgates && type == waOpenPlate)
    playSound(ct, "opengate");
  }

EX void destroyTrapsOn(cell *c) {
  if(c->wall == waArrowTrap) {
    changes.ccell(c);
    c->wall = waNone;
    drawParticles(c, 0xFF0000, 4);
    destroyTrapsAround(c);
    }
  }

EX void destroyTrapsAround(cell *c) {
  forCellEx(c2, c) destroyTrapsOn(c2);
  }

EX void destroyWeakBranch(cell *cf, cell *ct, eMonster who) {
  if(cf && ct && cf->wall == waWeakBranch && cellEdgeUnstable(ct) &&
    gravityLevelDiff(ct, cf) >= 0 && !ignoresPlates(who)) {
    changes.ccell(cf);
    cf->wall = waCanopy;
    if(!cellEdgeUnstable(cf)) { cf->wall = waWeakBranch; return; }
    playSound(cf, "trapdoor");
    drawParticles(cf, winf[waWeakBranch].color, 4);
    }
  if(cf && ct && cf->wall == waSmallBush && cellEdgeUnstable(ct) && 
    gravityLevelDiff(ct, cf) >= 0 && !ignoresPlates(who)) {
    changes.ccell(cf);
    cf->wall = waNone;
    if(!cellEdgeUnstable(cf)) { cf->wall = waSmallBush; return; }
    playSound(cf, "trapdoor");
    drawParticles(cf, winf[waWeakBranch].color, 4);
    }
  }

EX bool isCentralTrap(cell *c) {
  if(c->wall != waArrowTrap) return false;
  int i = 0;
  if(hat::in()) {
    for(int d=0; d<c->type; d++)
      if(c->cmove(d)->wall == waArrowTrap && c->cmodmove(d+c->type/2)->wall == waArrowTrap)
        return true;
    }
  forCellEx(c2, c) if(c2->wall == waArrowTrap) i++;
  return i == 2;
  }

EX array<cell*, 5> traplimits(cell *c) {
  array<cell*, 5> res;
  int q = 0;
  res[2] = c;
  for(int d=0; d<c->type; d++) {
    cellwalker cw(c, d);
    cw += wstep;
    if(cw.at->wall != waArrowTrap) continue;
    if(q && cw.at == res[1]) continue;
    res[1+q*2] = cw.at;
    cw += (cw.at->type/2);
    if((cw.at->type&1) && (cw+wstep).at->wall != waStone) cw += 1;
    if((cw+wstep).at->wall != waStone) continue;
    cw += wstep;
    res[(q++)*4] = cw.at;
    }
  while(q<2) { res[q*4] = res[1+q*2] = NULL; q++; }
  return res;
  }

EX void activateArrowTrap(cell *c) {
  if(c->wall == waArrowTrap && c->wparam == 0) {
    changes.ccell(c);
    playSound(c, "click");
    c->wparam = shmup::on ? 2 : 1;
    forCellEx(c2, c) activateArrowTrap(c2);
    if(shmup::on) shmup::activateArrow(c);
    }
  }

#if HDR
template<class T> 
movei determinePush(cellwalker who, int subdir, const T& valid) {
  if(subdir != 1 && subdir != -1) {
    subdir = 1;
    static bool first = true;
    if(first)
      first = false,
      addMessage("bad push: " + its(subdir));
    }
  cellwalker push = who;
  push += wstep;
  cell *c2 = push.at;
  if(bt::in()) {
    auto rd = reverse_directions(push.at, push.spin);
    for(int i: rd) {
      push.spin = i;
      movei mi = movei(push.at, i);
      if(valid(mi)) return mi;
      }
    return movei(c2, NO_SPACE);
    }
  int pd = push.at->type/2;
  push += pd * -subdir;
  movei mi(push.at, push.spin);
  push += wstep;
  if(valid(mi)) return mi;
  if(c2->type&1) {
    push = push + wstep - subdir + wstep;
    if(valid(mi)) return mi;
    }
  if(gravityLevelDiff(push.at, c2) < 0) {
    push = push + wstep + 1 + wstep;
    if(gravityLevelDiff(push.at, c2) < 0) {
      push = push + wstep - 2 + wstep;
      }
    if(gravityLevelDiff(push.at, c2) < 0) {
      push = push + wstep + 1 + wstep;
      }
    movei mi = movei(c2, (push+wstep).spin);
    if(valid(mi)) return mi;
    }
  return movei(c2, NO_SPACE);
  }
#endif
 
// for sandworms
EX void explodeAround(cell *c) {
  forCellEx(c2, c) {
      if(isIcyLand(c2)) HEAT(c2) += 0.5;
      eWall ow = c2->wall;
      changes.ccell(c2);
      if((c2->wall == waDune || c2->wall == waIcewall ||
        c2->wall == waAncientGrave || c2->wall == waFreshGrave || 
        c2->wall == waColumn || c2->wall == waThumperOff || c2->wall == waThumperOn ||
        (isFire(c2) && !eternalFire(c2)) ||
        c2->wall == waBigTree || c2->wall == waSmallTree ||
        c2->wall == waVinePlant || c2->wall == waVineHalfA || c2->wall == waVineHalfB)) {
        destroyHalfvine(c2); 
        c2->wall = waNone;
        }
      if(c2->wall == waExplosiveBarrel) explodeBarrel(c2);
      if(c2->wall == waCavewall || c2->wall == waDeadTroll) c2->wall = waCavefloor;
      if(c2->wall == waDeadTroll2) c2->wall = waNone;
      if(c2->wall == waPetrified) c2->wall = waNone;
      if(c2->wall == waDeadfloor2) c2->wall = waDeadfloor;
      if(c2->wall == waGargoyleFloor) c2->wall = waChasm;
      if(c2->wall == waGargoyleBridge || c2->wall == waPetrifiedBridge) placeWater(c2, c2);
      if(c2->wall == waRubble) c2->wall = waNone;
      if(c2->wall == waPlatform) c2->wall = waNone;
      if(c2->wall == waStone) c2->wall = waNone, destroyTrapsAround(c2);
      if(c2->wall == waRose) c2->wall = waNone;
      if(c2->wall == waRuinWall) c2->wall = waNone;
      if(c2->wall == waLadder) c2->wall = waNone;
      if(c2->wall == waGargoyle) c2->wall = waNone;
      if(c2->wall == waSandstone) c2->wall = waNone;
      if(c2->wall == waSaloon) c2->wall = waNone;
      if(c2->wall == waDeadwall) c2->wall = waDeadfloor2;
      if(c2->wall == waBigStatue) c2->wall = waNone;
      if(c2->wall == waPalace || c2->wall == waOpenGate || c2->wall == waClosedGate) 
        c2->wall = waNone;
      if(isAlch(c2) && isAlch(c))
        c2->wall = c->wall;
      if(c2->wall != ow && ow) drawParticles(c2, winf[ow].color, 16);
      }
  }

EX bool earthMove(const movei& mi) {
  auto& from = mi.s;
  bool b = false;
  cell *c2 = mi.t;
  b |= earthWall(from);
  if(!mi.proper()) return b;
  int d = mi.rev_dir_or(0);
  if(c2) for(int u=2; u<=c2->type-2; u++) {
    cell *c3 = c2->modmove(d + u);
    if(c3) b |= earthFloor(c3);
    }
  return b;
  }

EX bool cellDangerous(cell *c) {
  return cellUnstableOrChasm(c) || isFire(c) || c->wall == waClosedGate;
  }


}
