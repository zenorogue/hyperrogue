// Hyperbolic Rogue

// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

// Orb-related routines

bool markOrb(eItem it) {
  if(!items[it]) return false;
  orbused[it] = true;
  return true;
  }

bool markEmpathy(eItem it) {
  if(!items[itOrbEmpathy]) return false;
  if(!markOrb(it)) return false;
  markOrb(itOrbEmpathy);
  return true;
  }

bool markOrb2(eItem it) {
  if(!items[it]) return false;
  orbused[it] = true;
  return items[it] > 1;
  }

int fixpower(int qty) {
  if(markOrb(itOrbEnergy)) qty = (qty+1)/2;
  return qty;
  }

void useupOrb(eItem it, int qty) {
  items[it] -= fixpower(qty);
  if(items[it] < 0) items[it] = 0;
  }

void drainOrb(eItem it, int target) {
  if(items[it] > target) useupOrb(it, items[it] - target);
  }

void empathyMove(cell *c, cell *cto, int dir) {
  if(!items[itOrbEmpathy]) return;

  if(items[itOrbFire]) {
    invismove = false;
    if(makeflame(c, 10, false)) markEmpathy(itOrbFire);
    }

  if(items[itOrbDigging]) {           
   if(dir != STRONGWIND && earthMove(c, dir)) 
     markEmpathy(itOrbDigging), invismove = false;
   }

  if(items[itOrbWinter] && isIcyLand(c) && c->wall == waNone) {
    invismove = false;
    c->wall = waIcewall;
    markEmpathy(itOrbWinter);
    }
  }

bool reduceOrbPower(eItem it, int cap) {
  if(items[it] && (lastorbused[it] || (it == itOrbShield && items[it]>3) || !markOrb(itOrbPreserve))) {
    items[it] -= numplayers();
    if(isHaunted(cwt.c->land)) survivalist = false;
    if(items[it] < 0) items[it] = 0;
    if(items[it] > cap) items[it] = cap;
    if(items[it] == 0 && it == itOrbLove) 
      princess::bringBack();
    return true;
    }
  if(items[it] > cap) items[it] = cap;
  return false;
  }

void reduceOrbPowerAlways(eItem it) {
  if(items[it]) {
    items[it] -= numplayers();
    if(items[it] < 0) items[it] = 0;
    }
  }

void reduceOrbPowers() {
  if(getMount()) markOrb(itOrbDomination);
  for(int i=0; i<ittypes; i++) 
    lastorbused[i] = orbused[i], orbused[i] = false;
  if(items[itOrbShield]) orbused[itOrbShield] = lastorbused[itOrbShield];
  reduceOrbPower(itOrbPreserve, cwt.c->land == laCaribbean ? 777 : 150);
  if(invismove && !invisfish) markOrb(itOrbInvis);
  reduceOrbPower(itOrbLightning, 777);
  reduceOrbPower(itOrbSpeed, 67);
  reduceOrbPower(itOrbShield, 77);
  reduceOrbPower(itOrbShell, 150);
  reduceOrbPower(itOrbFlash, 777);
  reduceOrbPower(itOrbWinter, 77);
  reduceOrbPower(itOrbFire, 77);
  reduceOrbPower(itOrbIllusion, 111);
  reduceOrbPower(itOrbDragon, 111);
  reduceOrbPower(itOrbPsi, 111);
  reduceOrbPower(itOrbInvis, 77);
  reduceOrbPower(itOrbGhost, 77);
  reduceOrbPower(itOrbDigging, 100);
  reduceOrbPower(itOrbTeleport, 200);
  reduceOrbPower(itOrbTelekinesis, 150);
  reduceOrbPowerAlways(itOrbSafety);
  reduceOrbPower(itOrbThorns, 150);
  reduceOrbPower(itOrbWater, 150);
  reduceOrbPower(itOrbAir, 150);
  reduceOrbPower(itOrbFrog, 77);
  reduceOrbPower(itOrbDiscord, 67);
  reduceOrbPower(itOrbSummon, 333);
  reduceOrbPower(itOrbMatter, 333);
  reduceOrbPower(itOrbFish, 77);
  if(!items[itSavedPrincess]) items[itOrbLove] = 0;
  reduceOrbPower(itOrbLove, 777);
  reduceOrbPower(itOrbStunning, 100);
  reduceOrbPower(itOrbLuck, 333);
  reduceOrbPower(itOrbUndeath, 77);
  reduceOrbPower(itOrbFreedom, 77);
  reduceOrbPower(itOrbEmpathy, 77);
  markOrb(itOrb37); reduceOrbPower(itOrb37, 333);
  reduceOrbPower(itOrbSkunk, 77);
  reduceOrbPower(itOrbEnergy, 77);
  reduceOrbPower(itOrbDomination, 120);
  if(cwt.c->land != laWildWest)
    reduceOrbPower(itRevolver, 6);
  whirlwind::calcdirs(cwt.c); 
  items[itStrongWind] = !items[itOrbGhost] && whirlwind::qdirs == 1 && !euclid;
  }

void flashAlchemist(cell *c) {
  if(isAlch(c)) {
    if(isAlch(cwt.c))
      c->wall = cwt.c->wall;
    else
      c->wall = eWall(c->wall ^ waFloorB ^ waFloorA);
    }
  }

void flashCell(cell *c, bool msg) {
  flashAlchemist(c);
  if(msg && c->monst && !isWorm(c) && c->monst != moShadow)
    addMessage(XLAT("%The1 is destroyed by the Flash.", c->monst));
  killWithMessage(c, false);
  if(isIcyLand(c))
    HEAT(c) += 2;
  if(c->land == laDryForest)
    c->landparam += 2;
  if(c->wall == waCavewall)  c->wall = waCavefloor;
  if(c->wall == waDeadTroll) c->wall = waCavefloor;
  if(c->wall == waDeadTroll2) c->wall = waNone;
  if(c->wall == waDeadfloor2)  c->wall = waDeadfloor;
  if(c->wall == waGargoyleFloor)  c->wall = waChasm;
  if(c->wall == waGargoyleBridge)  placeWater(c, c);
  if(c->wall == waGargoyle)  c->wall = waNone;
  if(c->wall == waPlatform)  c->wall = waNone;
  if(c->wall == waStone)     c->wall = waNone;
  if(c->wall == waRubble)    c->wall = waNone;
  if(c->wall == waDeadwall)  c->wall = waDeadfloor2;
  if(c->wall == waGiantRug)  c->wall = waNone;
  if(c->wall == waMirror)    c->wall = waNone;
  if(c->wall == waCloud)     c->wall = waNone;
  if(c->wall == waDune)      c->wall = waNone;
  if(c->wall == waSaloon)    c->wall = waNone;
  if(c->wall == waSandstone) c->wall = waNone;
  if(c->wall == waAncientGrave) c->wall = waNone;
  if(c->wall == waFreshGrave) c->wall = waNone;
  if(c->wall == waColumn)    c->wall = waNone;
  if(c->wall == waGlass)     c->wall = waNone;
  if(c->wall == waBigTree || c->wall == waSmallTree)    c->wall = waNone;
  if(c->wall == waBigStatue) c->wall = waNone;
  if(c->wall == waCTree)     c->wall = waCIsland2;
  if(c->wall == waPalace)    c->wall = waRubble;
  if(c->wall == waRose)      c->wall = waNone;
  if(c->wall == waOpenGate || c->wall == waClosedGate) {
    eWall w = c->wall;
    c->wall = waNone;
    for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->wall == w)
      flashCell(c->mov[i], msg);
    }
  if(c->wall == waRed1)      c->wall = waNone;
  else if(c->wall == waRed2)      c->wall = waRed1;
  else if(c->wall == waRed3)      c->wall = waRed2;
  if(hasTimeout(c) && c->wparam < 77) c->wparam = 77;
  if(isActivable(c))
    activateActiv(c, false);
  }

void activateFlashFrom(cell *cf) {
  drawFlash(cf);
  for(int i=0; i<size(dcal); i++) {
    cell *c = dcal[i];
    if(c == cf) continue;
    for(int t=0; t<c->type; t++)
    for(int u=0; u<cf->type; u++)
      if(c->mov[t] == cf->mov[u] && c->mov[t] != NULL) {
        flashCell(c, true);
        }
    }
  }

bool distanceBound(cell *c1, cell *c2, int d) {
  if(!c1 || !c2) return false;
  if(d == 0) return c1 == c2;
  for(int i=0; i<c2->type; i++)
    if(distanceBound(c1, c2->mov[i], d-1)) return true;
  return false;
  }

void checkFreedom(cell *cf) {
  sval++;
  static vector<cell*> avcells;
  avcells.clear();
  avcells.push_back(cf);
  cf->aitmp = sval;
  for(int i=0; i<size(avcells); i++) {
    cell *c = avcells[i];
    if(c->cpdist >= 5) return;
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->mov[i];
      // todo leader
      if(!passable(c2, c, P_ISPLAYER | P_MIRROR | P_LEADER)) continue;
      if(eq(c2->aitmp, sval)) continue;
      bool monsterhere = false;
      for(int j=0; j<c2->type; j++) {
        cell *c3 = c2->mov[j];
        if(c3 && c3->monst && !isFriendly(c3)) 
          monsterhere = true;
        }
      if(!monsterhere) {
        c2->aitmp = sval;
        avcells.push_back(c2);
        }
      }
    }
  addMessage(XLAT("Your %1 activates!", itOrbFreedom));
  drainOrb(itOrbFreedom);
  drawBigFlash(cf);
  for(int i=0; i<size(dcal); i++) {
    cell *c = dcal[i];
    if(c == cf) continue;
    if(c->cpdist > 5) break;
    flashCell(c, true);
    }
  }

void activateFlash() {
  int tk = tkills();
  drawFlash(cwt.c);
  addMessage(XLAT("You activate the Flash spell!"));
  drainOrb(itOrbFlash);
  for(int i=0; i<size(dcal); i++) {
    cell *c = dcal[i];
    if(c->cpdist > 2) break;
    flashCell(c, false);
    }
  achievement_count("FLASH", tkills(), tk);
  }

bool barrierAt(cellwalker& c, int d) {
  if(d >= 7) return true;
  if(d <= -7) return true;
  d = c.spin + d + 42;
  d%=c.c->type;
  if(!c.c->mov[d]) return true;
  // WAS:
  // if(c.c->mov[d]->wall == waBarrier) return true;
  if(c.c->mov[d]->land == laBarrier || c.c->mov[d]->land == laOceanWall ||
    c.c->mov[d]->land == laHauntedWall || 
    c.c->mov[d]->land == laElementalWall) return true;
  return false;
  }

void killAdjacentSharks(cell *c) {
  for(int i=0; i<c->type; i++) {
    cell *c2 = c->mov[i];
    if(c2 && isShark(c2->monst)) {
      c2->ligon = true;
      killMonster(c2);
      killAdjacentSharks(c2);
      }
    }
  }

void castLightningBolt(cellwalker lig) {
  int bnc = 0;
  while(true) {
    // printf("at: %p i=%d d=%d\n", lig.c, i, lig.spin);

    killAdjacentSharks(lig.c);
    
    if(lig.c->mov[lig.spin] == NULL) break;

    cwstep(lig);
    
    cell *c = lig.c;

    flashAlchemist(c);
    if(c->monst == moMetalBeast2 && !c->item) c->item = itFulgurite;
    killWithMessage(c, false);
    if(isIcyLand(c)) HEAT(c) += 2;
    if(c->land == laDryForest) c->landparam += 2;
    bool first = !c->ligon;
    c->ligon = 1;
    
    bool brk = false, spin = false;

    if(c->wall == waGargoyle)  brk = true;
    if(c->wall == waCavewall)  c->wall = waCavefloor, brk = true;
    if(c->wall == waDeadTroll) c->wall = waCavefloor, brk = true;
    if(c->wall == waDeadTroll2)c->wall = waNone, brk = true;
    if(c->wall == waDeadfloor2)c->wall = waDeadfloor;
    if(c->wall == waRubble)    c->wall = waNone;
    if(c->wall == waDeadwall)  c->wall = waDeadfloor2, brk = true;
    if(c->wall == waGlass)     c->wall = waNone, spin = true;
    if(c->wall == waDune)      c->wall = waNone, brk = true;
    if(c->wall == waIcewall)   c->wall = waNone, brk = true;
    if(c->wall == waAncientGrave) c->wall = waNone, spin = true;
    if(c->wall == waFreshGrave) c->wall = waNone, spin = true;
    if(c->wall == waBigStatue) c->wall = waNone, spin = true;
    if(c->wall == waColumn)    c->wall = waNone, spin = true;
    if(c->wall == waStone)     c->wall = waNone, brk = true;
    
    if(c->wall == waGrounded)  brk = true;
    if(c->wall == waFan)       spin = true;
    if(c->wall == waMetal)     c->wall = waCharged, brk = true;
    if(c->wall == waSandstone) c->wall = waNone, c->item = itFulgurite, brk = true;

    if(c->wall == waCharged && first) {
      for(int i=0; i<c->type; i++) 
        // do not do strange things in horocyclic spires
        if(c->mov[i] && c->mov[i]->wall != waCharged) {
          cellwalker lig2(c, i);
          castLightningBolt(lig2);
          }
      brk = true;
      }
                             
    if(c->wall == waBoat && c != cwt.c)    c->wall = waSea, spin = true;
    if(c->wall == waStrandedBoat && c !=cwt.c)    c->wall = waNone, spin = true;

    if((c->wall == waNone || c->wall == waSea) && c->land == laLivefjord)
      c->wall = eWall(c->wall ^ waSea ^ waNone);
    
    if(c->wall == waRed1)      c->wall = waNone;
    if(c->wall == waRed2)      c->wall = waRed1;
    if(c->wall == waRed3)      c->wall = waRed2, brk = true;
    
    if(isActivable(c))         activateActiv(c, false);
    if(c->wall == waBigTree || c->wall == waSmallTree || c->wall == waVinePlant ||
      c->wall == waSaloon)    {
      makeflame(c, 4, false);
      brk = true;
      }
    if(c->wall == waCTree) makeflame(c, 12, false);
    if(c->wall == waRose)  makeflame(c, 60, false);
    if(cellHalfvine(c) && c->mov[lig.spin] && c->wall == c->mov[lig.spin]->wall) {
      destroyHalfvine(c, waPartialFire, 4);
      brk = true;
      }

    if(c == cwt.c)             {bnc++; if(bnc > 10) break; }
    if(spin) cwspin(lig, hrand(lig.c->type));
    
    if(brk) break;
    
    if(c->wall == waBarrier || c->wall == waCamelot || c->wall == waPalace || c->wall == waPlatform ||
      c->wall == waTempWall || c->wall == waWarpGate) {
      int left = -1;
      int right = 1;
      while(barrierAt(lig, left)) left--;
      while(barrierAt(lig, right)) right++;
      cwspin(lig, -(right + left));
      bnc++; if(bnc > 10) break;
      }
    else {
      cwspin(lig, 3);
      if(c->type == 7) cwspin(lig, hrand(2));
      }

    if(c->wall == waCloud) {
      c->wall = waNone;
      mirror::createMirages(c, lig.spin, moLightningBolt);
      }

    if(c->wall == waMirror) {
      c->wall = waNone;
      mirror::createMirrors(c, lig.spin, moLightningBolt);
      break;
      }
    }
  }

void activateLightning() {
  int tk = tkills();
  drawLightning();
  addMessage(XLAT("You activate the Lightning spell!"));

  for(int i=0; i<size(dcal); i++) if(dcal[i]) dcal[i]->ligon = 0;

  drainOrb(itOrbLightning);
  for(int i=0; i<cwt.c->type; i++) 
    castLightningBolt(cellwalker(cwt.c, i));
    
  elec::afterOrb = true;
  elec::act();
  elec::afterOrb = false;
  
  achievement_count("LIGHTNING", tkills(), tk);
  }

// roCheck: return orb type if successful, 0 otherwise
// roMouse/roKeyboard: 
//    return orb type if successful, eItem(-1) if do nothing, 0 otherwise

bool haveRangedTarget() {
  if(!haveRangedOrb())
    return false;
  for(int i=0; i<size(dcal); i++) {
    cell *c = dcal[i];
    if(targetRangedOrb(c, roCheck)) {
      return true;
      }
    }
  return false;
  }

void teleportTo(cell *dest) {
  if(dest->monst) {
    cwt.c->monst = dest->monst;
    dest->monst = moNone;
    }
  movecost(cwt.c, dest);
  playerMoveEffects(cwt.c, dest);
  cwt.c = dest; cwt.spin = hrand(dest->type); flipplayer = !!(hrand(2));
  drainOrb(itOrbTeleport);

  addMessage(XLAT("You teleport to a new location!"));
  mirror::destroy();

  for(int i=9; i>=0; i--)
    setdist(cwt.c, i, NULL);

  bfs();
  
  if(shmup::on)
    shmup::teleported();
  else
    checkmove();
  }

void jumpTo(cell *dest, eItem byWhat) {
  movecost(cwt.c, dest);

  cell *c1 = cwt.c;
  cwt.c = dest; 
  countLocalTreasure();

  playerMoveEffects(c1, dest);
  if(cwt.c->item != itOrbYendor && cwt.c->item != itHolyGrail)
    collectItem(cwt.c, true);
    
  if(byWhat == itOrbFrog) {
    useupOrb(itOrbFrog, 5);
    addMessage(XLAT("You jump!"));
    }
  
  mirror::destroy();

  for(int i=9; i>=0; i--)
    setdist(cwt.c, i, NULL);
  
  createNoise(1);

  if(shmup::on)
    shmup::teleported();
  else
    monstersTurn();
  }

void telekinesis(cell *dest) {
    
  int cost = dest->cpdist * dest->cpdist;
  
  if(dest->land == laAlchemist && isAlchAny(dest) && isAlchAny(cwt.c))
    dest->wall = cwt.c->wall;

  if(dest->land == laPower && cwt.c->land != laPower && dest->item != itOrbFire && dest->item != itOrbLife) {
    if(itemclass(dest->item) != IC_ORB)
      items[dest->item] ++;
    else
      items[dest->item] += 2;
    addMessage(XLAT("The Orb loses its power as it leaves the Land of Power!"));
    dest->item = itNone;
    }

  if(dest->wall == waGlass) {
    drainOrb(itOrbTelekinesis);
    addMessage(XLAT("Your power is drained by %the1!", dest->wall));
    }    

  moveItem(dest, cwt.c, true);
  collectItem(cwt.c, true);
  useupOrb(itOrbTelekinesis, cost);

  createNoise(3);
  bfs();
  if(!shmup::on) checkmove();
  }

eMonster summonedAt(cell *dest) {
  if(dest->monst) return moNone;
  if(dest->wall == waVineHalfA || dest->wall == waVineHalfB || dest->wall == waVinePlant)
    return moVineSpirit;
  if(dest->wall == waCTree)
    return moParrot;
  if(dest->wall == waLake)
    return moGreaterShark;
  if(dest->wall == waAncientGrave || dest->wall == waFreshGrave)
    return moGhost;
  if(dest->wall == waClosePlate || dest->wall == waOpenPlate)
    return moPalace;
  if(dest->wall == waFloorA || dest->wall == waFloorB)
    return moSlime;
  if(dest->wall == waFloorC || dest->wall == waFloorD)
    return moRatling;
  if(dest->wall == waCavefloor)
    return moTroll;
  if(dest->wall == waDeadfloor)
    return moEarthElemental;
  if(dest->wall == waDeadfloor2)
    return moMiner;
  if(dest->wall == waMineOpen || dest->wall == waMineMine || dest->wall == waMineUnknown)
    return moBomberbird;
  if(dest->wall == waTrapdoor)
    return dest->land == laPalace ? moFatGuard : moOrangeDog;
  if(dest->wall == waSea)
    return 
      isElemental(dest->land) ? moWaterElemental :
      dest->land == laLivefjord ? moViking : 
      dest->land == laGridCoast ? moRatling : 
      moPirate;
  if(dest->wall == waChasm)
    return moAirElemental;
  if(isFire(dest))
    return moFireElemental;
  if(dest->wall == waCavewall || dest->wall == waDeadwall)
    return moSeep;
  if(dest->wall == waRed1 || dest->wall == waRed2 || dest->wall == waRed3)
    return moRedTroll;
  if(dest->wall == waFrozenLake)
    return moFireElemental;
  if(dest->wall == waCIsland || dest->wall == waCIsland2)
    return moWaterElemental;
  if(dest->wall == waRubble || dest->wall == waGargoyleFloor || dest->wall == waGargoyleBridge || dest->wall == waLadder)
    return moGargoyle;
  if(dest->wall == waStrandedBoat)
    return moWaterElemental;
  if(dest->wall == waBoat)
    return moAirElemental;
  if(dest->wall == waStone)
    return moEarthElemental;
  if(dest->wall == waGiantRug)
    return moVizier;
  if(dest->wall == waNone) {
    if(dest->land == laIce) return moFireElemental;
    if(dest->land == laDesert) return moEarthElemental;
    if(dest->land == laJungle) return moWaterElemental;
    if(dest->land == laGraveyard) return moZombie;
    if(dest->land == laRlyeh || dest->land == laTemple) return moPyroCultist;
    if(dest->land == laHell) return moWaterElemental;
    if(dest->land == laPower) return moWitchFire;
    if(dest->land == laWineyard) return moVineBeast;
    if(dest->land == laEmerald) return moMiner;
    if(dest->land == laHive) return dest->type == 7 ? moBug1 : moBug0;
    if(dest->land == laRedRock) return moRedTroll;
    if(dest->land == laOcean) return moEarthElemental;
    if(dest->land == laDryForest) return moFireFairy;
    if(dest->land == laLivefjord) return moFjordTroll;
    if(dest->land == laStorms) return moStormTroll;
    if(dest->land == laOvergrown) return moForestTroll;
    if(dest->land == laIvoryTower) return moAirElemental;
    if(dest->land == laEndorian) return moAirElemental;
    if(dest->land == laEAir) return moAirElemental;
    if(dest->land == laEWater) return moWaterElemental;
    if(dest->land == laEEarth) return moEarthElemental;
    if(dest->land == laEFire) return moFireElemental;
    if(dest->land == laMotion) return moRunDog;
    if(dest->land == laWildWest) return moOutlaw;
    if(dest->land == laClearing) return moForestTroll;
    if(dest->land == laWhirlwind) return moAirElemental;
    if(dest->land == laGridCoast) return moRatling;
    if(dest->land == laRose) return moRoseLady;
    if(dest->land == laDragon) return moFireElemental;
    if(dest->land == laTortoise) return moTortoise;
    if(isHaunted(dest->land)) return moGhost;
    }
  return moNone;
  }

void summonAt(cell *dest) {
  dest->monst = summonedAt(dest);
  dest->stuntime = 3;
  if(dest->monst == moPirate || dest->monst == moViking || (dest->monst == moRatling && dest->wall == waSea))
    dest->wall = waBoat;
  if(dest->wall == waStrandedBoat)
    dest->wall = waBoat;
  else if(dest->monst == moWaterElemental)
    placeWater(dest, dest);
  if(dest->wall == waStone)
    dest->wall = waNone;
  if(dest->monst == moFireElemental && isFire(dest))
    dest->wall = waNone;
  if(dest->monst == moTortoise)
    tortoise::emap[dest] = dest;
  addMessage(XLAT("You summon %the1!", dest->monst));
  moveEffect(dest, dest, dest->monst);

  if(hasHitpoints(dest->monst))
    dest->hitpoints = palaceHP();

  useupOrb(itOrbSummon, 20);
  createNoise(2);
  bfs();
  checkmove();
  }

bool tempWallPossibleAt(cell *dest) {
  if(dest->monst || (dest->item && !itemHidden(dest))) return false;
  return dest->wall == waChasm || isWatery(dest) || dest->wall == waNone;
  }

void tempWallAt(cell *dest) {
  if(dest->wall == waChasm)
    dest->wall = waTempFloor;
  else if(dest->wall == waNone)
    dest->wall = waTempWall;
  else if(isWatery(dest))
    dest->wall = waTempBridge;
  int len = (items[itOrbMatter]+1) / 2;
  dest->wparam = len;
  useupOrb(itOrbMatter, len);
  dest->item = itNone; // underwater items are destroyed by this
  createNoise(2);
  bfs();
  checkmove();
  }

void psi_attack(cell *dest) {
  if(isNonliving(dest->monst))
    addMessage(XLAT("You destroy %the1 with a mental blast!", dest->monst));
  else if(isDragon(dest->monst))
    addMessage(XLAT("You damage %the1 with a mental blast!", dest->monst));
  else
    addMessage(XLAT("You kill %the1 with a mental blast!", dest->monst));
  killWithMessage(dest, false);
  useupOrb(itOrbPsi, 30);
  createNoise(2);
  bfs();
  checkmove();
  }

void gun_attack(cell *dest) {
  addMessage(XLAT("You shoot %the1!", dest->monst));
  killWithMessage(dest, false);
  items[itRevolver] --;
  bfs();
  checkmove();
  createNoise(5);
  monstersTurn();
  }

void stun_attack(cell *dest) {
  addMessage(XLAT("You stun %the1!", dest->monst));
  dest->stuntime += 5;
  if(isBird(dest->monst)) {
    moveEffect(dest, dest, moDeadBird);
    if(cellUnstableOrChasm(dest)) dest->wall = waChasm;
    if(isWatery(dest) || dest->wall == waChasm || isFire(dest))    
      killMonster(dest);
    }
  useupOrb(itOrbStunning, 10);
  createNoise(3);
  bfs();
  checkmove();
  }

void placeIllusion(cell *c) {
  c->monst = moIllusion;
  useupOrb(itOrbIllusion, 5);
  addMessage(XLAT("You create an Illusion!"));
  bfs();
  checkmove();
  }

void blowoff(cell *cf, cell *ct) {
  addMessage(XLAT("You blow %the1 away!", cf->monst));
  pushMonster(ct, cf);
  if(cf->item == itBabyTortoise) {
    if(!ct->item) {
      ct->item = itBabyTortoise;
      tortoise::babymap[ct] = tortoise::babymap[cf];
      }
    tortoise::babymap.erase(cf);
    cf->item = itNone;
    }
  items[itOrbAir]--;
  createNoise(2);
  bfs();
  checkmove();
  }

void useOrbOfDragon(cell *c) {
  makeflame(c, 20, false);
  addMessage(XLAT("You throw fire!"));
  useupOrb(itOrbDragon, 5);
  createNoise(3);
  bfs();
  checkmove();
  }

eItem targetRangedOrb(cell *c, orbAction a) {

  if(!haveRangedOrb()) return itNone;
  
  if(rosedist(cwt.c) == 1) {
    int r = rosemap[cwt.c];
    int r2 = rosemap[c];
    if(r2 <= r) {
      if(a == roKeyboard || a == roMouseForce ) 
        addMessage(XLAT("Those roses smell too nicely. You can only target cells closer to them!"));
      return itNone;
      }
    }

  // (-2) shmup variants
  eItem shmupEffect = shmup::targetRangedOrb(a);
  
  if(shmupEffect) return shmupEffect;
  
  // (-1) distance

  if(c == cwt.c || isNeighbor(cwt.c, c)) {
    if(a == roKeyboard || a == roMouseForce ) 
      addMessage(XLAT("You cannot target that close!"));
    return itNone;
    }
  if(c->cpdist > 7) {
    if(a != roCheck)
      addMessage(XLAT("You cannot target that far away!"));
    return itNone;
    }
  
  // (0-) strong wind
  if(items[itStrongWind] && c->cpdist == 2 && cwt.c == whirlwind::jumpFromWhereTo(c, true) && !monstersnear(c, NULL, moPlayer, NULL, cwt.c)) {
    if(a != roCheck) jumpTo(c, itStrongWind);
    return itStrongWind;
    }
  
  // (0x) control
  if(getMount() && items[itOrbDomination] && dragon::whichturn != turncount) {
    if(a != roCheck) { 
      dragon::target = c;
      dragon::whichturn = turncount;
      addMessage(XLAT("Commanded %the1!", getMount()));
      checkmove();
      }
    return itOrbDomination;
    }
  
  // (0) telekinesis
  if(c->item && !itemHidden(c) && !cwt.c->item && items[itOrbTelekinesis] >= fixpower(c->cpdist * c->cpdist) && !cantGetGrimoire(c, a != roCheck)) {
    if(a != roCheck) telekinesis(c);
    return itOrbTelekinesis;
    }
  
  // (0') air blow
  bool nowhereToBlow = false;
  if(items[itOrbAir] && isBlowableMonster(c->monst)) {
    int d = 0;
    for(; d<c->type; d++) if(c->mov[d] && c->mov[d]->cpdist < c->cpdist) break;
    if(d<c->type) for(int e=d; e<d+c->type; e++) {
      nowhereToBlow = true;
      cell *c2 = c->mov[e % c->type];
      if(c2 && c2->cpdist > c->cpdist && passable(c2, c, P_BLOW)) {
        if(a != roCheck) blowoff(c, c2);
        return itOrbAir;
        }
      }
    }
  
  // (0'') jump
  int jumpstate = 0;
  cell *jumpthru = NULL;

  if(items[itOrbFrog] && c->cpdist == 2) {
    jumpstate = 1;
    int i = items[itOrbGhost];
    if(i) items[itOrbGhost] = i-1;
    for(int i=0; i<cwt.c->type; i++) {
      cell *c2 = cwt.c->mov[i];
      if(isNeighbor(c2, c)) {
        jumpthru = c2;
        if(passable(c2, cwt.c, P_ISPLAYER | P_JUMP1)) {
          jumpstate = 2;
          if(passable(c, c2, P_ISPLAYER | P_JUMP2)) {
            jumpstate = 3;
            break;
            }
          }
        }
      }
    items[itOrbGhost] = i;
    if(jumpstate == 3 && !monstersnear(c, NULL, moPlayer, NULL, c)) {
      jumpstate = 4;
      if(a != roCheck) jumpTo(c, itOrbFrog);
      return itOrbFrog;
      }
    }
  
  // (1) switch with an illusion
  if(items[itOrbTeleport] && c->monst == moIllusion && !cwt.c->monst) {
    if(a != roCheck) teleportTo(c);
    return itOrbTeleport;
    }

  // (2) place illusion
  if(!shmup::on && items[itOrbIllusion] && c->monst == moNone && c->item == itNone && passable(c, NULL, P_MIRROR)) {
    if(a != roCheck) placeIllusion(c);
    return itOrbIllusion;
    }
  
  // (3) teleport
  if(items[itOrbTeleport] && c->monst == moNone && (c->item == itNone || itemHidden(c)) && 
    passable(c, NULL, P_ISPLAYER | P_TELE) && shmup::verifyTeleport()) {
    if(a != roCheck) teleportTo(c);
    return itOrbTeleport;
    }
    
  // (4) remove an illusion
  if(!shmup::on && items[itOrbIllusion] && c->monst == moIllusion) {
    if(a != roCheck) {
      addMessage(XLAT("You take the Illusion away."));
      items[itOrbIllusion] += 3; // 100% effective with the Orb of Energy!
      c->monst = moNone;
      }
    return itOrbIllusion;
    }

  // (4a) colt
  if(!shmup::on && items[itRevolver] && c->monst && canAttack(cwt.c, moPlayer, c, c->monst, AF_GUN) && !monstersnear(cwt.c, c, moPlayer, NULL, cwt.c)
    && c->pathdist <= GUNRANGE) {
    if(a != roCheck) gun_attack(c);
    return itRevolver;
    }    
  
  // (5) psi blast (non-shmup variant)
  if(!shmup::on && items[itOrbPsi] && c->monst && (isDragon(c->monst) || !isWorm(c)) && c->monst != moShadow) {
    if(a != roCheck) psi_attack(c);
    return itOrbPsi;
    }
  
  // (5a) summoning
  if(items[itOrbSummon] && summonedAt(c)) {
    if(a != roCheck) summonAt(c);
    return itOrbSummon;
    }
  
  // (5b) matter
  if(items[itOrbMatter] && tempWallPossibleAt(c)) {
    if(a != roCheck) tempWallAt(c);
    return itOrbMatter;
    }

  // (5c) stun
  if(items[itOrbStunning] && c->monst && !isMultitile(c->monst) && c->stuntime < 3 && !shmup::on) {
    if(a != roCheck) stun_attack(c);
    return itOrbStunning;
    }
  
  // (6) place fire (non-shmup variant)
  if(!shmup::on && items[itOrbDragon] && makeflame(c, 20, true)) {
    if(a != roCheck) useOrbOfDragon(c);
    return itOrbDragon;
    }
  
  if(a == roCheck) return itNone;
  
  if(nowhereToBlow) {
    addMessage(XLAT("Nowhere to blow %the1!", c->monst));
    }  
  else if(jumpstate == 1 && jumpthru && jumpthru->monst) {
    addMessage(XLAT("Cannot jump through %the1!", jumpthru->monst));
    }
  else if(jumpstate == 1 && jumpthru) {
    addMessage(XLAT("Cannot jump through %the1!", jumpthru->wall));
    }
  else if(jumpstate == 2 && c->monst) {
    addMessage(XLAT("Cannot jump on %the1!", c->monst));
    }
  else if(jumpstate == 2 && c->wall) {
    addMessage(XLAT("Cannot jump on %the1!", c->wall));
    }
  else if(jumpstate == 3) {
    addMessage(XLAT("%The1 would get you there!", which));
    }
  else if(items[itOrbAir] && c->monst) {
    addMessage(XLAT("%The1 is immune to wind!", c->monst));
    }  
  else if(items[itOrbPsi] && c->monst) {
    addMessage(XLAT("%The1 is immune to mental blasts!", c->monst));
    }  
  else if(items[itOrbTeleport] && c->monst) {
    addMessage(XLAT("Cannot teleport on a monster!"));
    }
  else if(c->item && items[itOrbTelekinesis]) {
    addMessage(XLAT("Not enough power for telekinesis!"));
    }
  else if(items[itOrbIllusion] && c->item)
    addMessage(XLAT("Cannot cast illusion on an item!"));
  else if(items[itOrbIllusion] && c->monst)
    addMessage(XLAT("Cannot cast illusion on a monster!"));
  else if(items[itOrbIllusion] && !passable(c, NULL, P_MIRROR))
    addMessage(XLAT("Cannot cast illusion here!"));
  else if(items[itOrbTeleport] && !passable(c, NULL, P_TELE | P_ISPLAYER)) {
    addMessage(XLAT("Cannot teleport here!"));
    }
  else if(items[itOrbMatter] && !tempWallPossibleAt(c)) {
    if(c->monst)
      addMessage(XLAT("Cannot create temporary matter on a monster!"));
    else if(c->item)
      addMessage(XLAT("Cannot create temporary matter on an item!"));
    else addMessage(XLAT("Cannot create temporary matter here!"));
    }
  else if(items[itOrbSummon] && !summonedAt(c)) {
    if(c->monst)
      addMessage(XLAT("Cannot summon on a monster!"));
    else
      addMessage(XLAT("No summoning possible here!"));
    }
  else if(items[itOrbTeleport] && c->item) {
    addMessage(XLAT("Cannot teleport on an item!"));
    }
  else if(items[itOrbDragon] && !makeflame(c, 20, true)) {
    addMessage(XLAT("Cannot throw fire there!"));
    }
  else return eItem(0);
  
  return eItem(-1);
  }

