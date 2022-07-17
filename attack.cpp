// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file gameutil.cpp
 *  \brief routines related to attacking and killing
 */

#include "hyper.h"

namespace hr {

/** how many instances of each monster type has been killed */
EX array<int, motypes> kills;

int* killtable[] = {
    &kills[moYeti], &kills[moWolf], &
    kills[moRanger], &kills[moTroll], &kills[moGoblin], &
    kills[moWorm], &kills[moDesertman], &kills[moIvyRoot], &
    kills[moMonkey], &kills[moEagle], &kills[moSlime], &kills[moSeep], &
    kills[moRunDog], &
    kills[moCultist], &kills[moTentacle], &kills[moPyroCultist], &
    kills[moLesser], &kills[moGreater], &
    kills[moZombie], &kills[moGhost], &kills[moNecromancer], &
    kills[moHedge], &kills[moFireFairy], &
    kills[moCrystalSage], &kills[moShark], &kills[moGreaterShark], &
    kills[moMiner], &kills[moFlailer], &kills[moLancer], &
    kills[moVineSpirit], &kills[moVineBeast], &
    kills[moBug0], &kills[moBug1], &kills[moBug2], &
    kills[moDarkTroll], &kills[moEarthElemental], &
    kills[moWitch], &kills[moEvilGolem], &kills[moWitchFlash], &kills[moWitchFire], &
    kills[moWitchWinter], &kills[moWitchSpeed], &
    kills[moCultistLeader], &
    kills[moPirate], &kills[moCShark], &kills[moParrot], &
    kills[moHexSnake], &kills[moRedTroll], &
    kills[moPalace], &kills[moSkeleton], &kills[moFatGuard], &kills[moVizier], &
    kills[moViking], &kills[moFjordTroll], &kills[moWaterElemental], &
    kills[moAlbatross], &kills[moBomberbird], &
    kills[moAirElemental], &kills[moFireElemental], &
    kills[moGargoyle], &kills[moFamiliar], &kills[moOrangeDog], &
    items[itMutant], &kills[moMetalBeast], &kills[moMetalBeast2], &
    kills[moOutlaw], &kills[moForestTroll], &kills[moStormTroll], &
    kills[moRedFox], &kills[moWindCrow], &
    kills[moFalsePrincess], &kills[moRoseLady], &
    kills[moRoseBeauty], &
    kills[moRatling], &kills[moRatlingAvenger], &
    kills[moDragonHead], &
    kills[moGadfly], &kills[moSparrowhawk], &kills[moResearcher],
    &kills[moKrakenH], &kills[moDraugr],
    &kills[moBat], &kills[moReptile],
    &kills[moHerdBull], &kills[moSleepBull], &kills[moRagingBull],
    &kills[moButterfly],
    &kills[moNarciss], &kills[moMirrorSpirit],
    &kills[moHunterDog], &kills[moIceGolem], &kills[moVoidBeast],
    &kills[moJiangshi], &kills[moTerraWarrior],
    &kills[moSalamander], &kills[moLavaWolf],
    &kills[moSwitch1], &kills[moSwitch2],
    &kills[moMonk], &kills[moCrusher], &kills[moHexDemon], &kills[moAltDemon], &kills[moPair],
    &kills[moBrownBug], &kills[moAcidBird],
    &kills[moFallingDog], &kills[moVariantWarrior], &kills[moWestHawk],
    &kills[moPike], &kills[moRusalka], &kills[moFrog], &kills[moPhaser], &kills[moVaulter],
    &kills[moHexer], &kills[moAnimatedDie], &kills[moAngryDie],
    NULL
    };

EX int tkills() {
  int res = 0;
  for(int i=0; killtable[i]; i++) res += killtable[i][0];
  return res;
  }

EX int killtypes() {
  int res = 0;
  for(int i=0; killtable[i]; i++) if(killtable[i][0]) res++;
  return res;
  }


EX bool arrow_stuns(eMonster m) {
  return among(m, moCrusher, moMonk, moAltDemon, moHexDemon, moGreater, moGreaterM, moHedge);
  }

EX bool canAttack(cell *c1, eMonster m1, cell *c2, eMonster m2, flagtype flags) {

  // cannot eat worms
  if((flags & AF_EAT) && isWorm(m2)) return false;
  
  if(m1 == passive_switch || m2 == passive_switch) return false;
  
  if((flags & AF_GETPLAYER) && isPlayerOn(c2)) m2 = moPlayer;
  
  if(!m2) return false;
  
  if(m2 == moPlayer && peace::on) return false;

  if((flags & AF_WEAK) && isIvy(c2)) return false;

  if((flags & AF_MUSTKILL) && attackJustStuns(c2, flags, m1))
    return false;
  
  if((flags & AF_ONLY_FRIEND) && m2 != moPlayer && !isFriendly(c2)) return false;
  if((flags & AF_ONLY_FBUG)   && m2 != moPlayer && !isFriendlyOrBug(c2)) return false;
  if((flags & AF_ONLY_ENEMY) && (m2 == moPlayer || isFriendlyOrBug(c2))) return false;
  
  if(isFriendly(c2) && markOrb(itOrbEmpathy) && (flags & (AF_STAB | AF_MAGIC | AF_HORNS | AF_SWORD | AF_SWORD_INTO | AF_SIDE | AF_PLAGUE)))
    return false;
  
  if(m1 == moArrowTrap && arrow_stuns(m2)) return true;
  
  if(isDie(m2) && !(flags & (AF_MAGIC | AF_CRUSH))) return false;
  
  if(among(m2, moAltDemon, moHexDemon, moPair, moCrusher, moNorthPole, moSouthPole, moMonk) && !(flags & (AF_EAT | AF_MAGIC | AF_BULL | AF_CRUSH)))
    return false;
  
  if(m2 == moHedge && !(flags & (AF_STAB | AF_TOUGH | AF_EAT | AF_MAGIC | AF_LANCE | AF_SWORD_INTO | AF_HORNS | AF_BULL | AF_CRUSH)))
    if(!checkOrb(m1, itOrbThorns)) return false;
  
  // krakens do not try to fight even with Discord
  if((m1 == moKrakenT || m1 == moKrakenH) && 
     (m2 == moKrakenT || m2 == moKrakenH))
    return false;
  
  if(m2 == moDraugr && !(flags & (AF_SWORD | AF_MAGIC | AF_SWORD_INTO | AF_HORNS | AF_CRUSH))) return false;

  // if(m2 == moHerdBull && !(flags & AF_MAGIC)) return false;
  if(isBull(m2) && !(flags & (AF_MAGIC | AF_HORNS | AF_SWORD_INTO | AF_CRUSH))) return false;
  if(m2 == moButterfly && !(flags & (AF_MAGIC | AF_BULL | AF_HORNS | AF_SWORD_INTO | AF_CRUSH))) return false;
  
  if(!(flags & AF_NOSHIELD) && ((flags & AF_NEXTTURN) ? checkOrb2 : checkOrb)(m2, itOrbShield)) return false;
  
  if((flags & AF_STAB) && m2 != moHedge)
    if(!checkOrb(m1, itOrbThorns)) return false;

  if(flags & AF_BACK) {
    if(m2 == moFlailer && !c2->stuntime) flags |= AF_IGNORE_UNARMED;
    else if(m2 == moVizier && !isUnarmed(m1)) ;
    else return false;
    }

  if(flags & AF_APPROACH) {
    if(m2 == moLancer) ;
    else if((flags & AF_HORNS) && checkOrb(m1, itOrbHorns)) ;
    else return false;
    }
  
  if(!(flags & AF_IGNORE_UNARMED) && isUnarmed(m1)) return false;
  
  if(m2 == moGreater || m2 == moGreaterM)
    if(!(flags & (AF_MAGIC | AF_SWORD_INTO | AF_HORNS | AF_CRUSH))) return false;
    
  if(!(flags & (AF_GUN | AF_SWORD | AF_SWORD_INTO | AF_MAGIC | AF_PLAGUE)))
    if(c1 != c2 && !logical_adjacent(c1, m1, c2)) return false;

  if(!(flags & (AF_LANCE | AF_STAB | AF_BACK | AF_APPROACH | AF_GUN | AF_MAGIC | AF_PLAGUE | AF_SIDE)))
    if(c1 && c2 && againstRose(c1, c2) && !ignoresSmell(m1))
      return false;
  
  if(m2 == moShadow && !(flags & (AF_SWORD | AF_SWORD_INTO | AF_CRUSH))) return false;
  if(isWorm(m2) && m2 != moTentacleGhost && !isDragon(m2)) return false;
  
  // dragon can't attack itself, or player who mounted it
  if(c1 && c2 && isWorm(c1->monst) && isWorm(c2->monst) && wormhead(c1) == wormhead(c2)
    && m1 != moTentacleGhost && m2 != moTentacleGhost)
    return false;
    
  // if(m2 == moTortoise && !(flags & AF_MAGIC)) return false;
  
  if(m2 == moRoseBeauty)
    if(!(flags & (AF_MAGIC | AF_LANCE | AF_GUN | AF_SWORD_INTO | AF_BULL | AF_CRUSH))) 
    if(!isMimic(m1))
    if(!checkOrb(m1, itOrbBeauty) && !checkOrb(m1, itOrbAether) && !checkOrb(m1, itOrbShield))
    if(!c1 || !c2 || !withRose(c1,c2))
      return false;
  
  if(m2 == moFlailer && !c2->stuntime)
    if(!(flags & (AF_MAGIC | AF_TOUGH | AF_EAT | AF_HORNS | AF_LANCE | AF_BACK | AF_SWORD_INTO | AF_BULL | AF_CRUSH))) return false;

  return true;
  }

EX bool petrify(cell *c, eWall walltype, eMonster m) {
  destroyHalfvine(c);
  destroyTrapsOn(c);
  playSound(c, "die-troll");
  
  if(walltype == waIcewall && !isIcyLand(c->land))
    return false;
  
  if(c->land == laWestWall) return false;
  
  if(isWateryOrBoat(c) && c->land == laWhirlpool) {
    c->wall = waSea;
    return false;
    }
  
  if(c->wall == waRoundTable) return false;
  
  if(walltype == waGargoyle && cellUnstableOrChasm(c)) 
    walltype = waGargoyleFloor;
  else if(walltype == waGargoyle && isWatery(c)) 
    walltype = waGargoyleBridge;
  else if(walltype == waPetrified && isWatery(c))
    walltype = waPetrifiedBridge;
  else if((c->wall == waTempBridge || c->wall == waTempBridgeBlocked) && c->land == laWhirlpool) {
    c->wall = waTempBridgeBlocked;
    return true;
    }
  else if(!doesnotFall(c)) {
    fallingFloorAnimation(c, walltype, m);
    return true;
    }

  if(isReptile(c->wall)) kills[moReptile]++;
  destroyHalfvine(c);
  c->wall = walltype;
  c->wparam = m;
  c->item = itNone;
  return true;
  }
        
EX void killIvy(cell *c, eMonster who) {
  if(c->monst == moIvyDead) return;
  changes.ccell(c);
  if(checkOrb(who, itOrbStone)) petrify(c, waPetrified, c->monst);
  c->monst = moIvyDead; // NEWYEARFIX
  for(int i=0; i<c->type; i++) if(c->move(i))
    if(isIvy(c->move(i)) && c->move(i)->mondir == c->c.spin(i))
      killIvy(c->move(i), who), kills[moIvyDead]++;
  }

struct spillinfo {
  eWall orig;
  int spill_a, spill_b;
  spillinfo() { spill_a = spill_b = 0; }
  };

map<cell*, spillinfo> spillinfos;

EX void reset_spill() {
  spillinfos.clear();
  }

EX void record_spillinfo(cell *c, eWall t) {
  if(!isAlchAny(t)) return;
  auto& si = spillinfos[c];
  if(si.spill_a == 0 && si.spill_b == 0) si.orig = c->wall;
  if(si.spill_a == 0 && si.spill_b == 0) si.orig = c->wall;
  if(t == waFloorA) si.spill_a++;
  if(t == waFloorB) si.spill_b++;
  }

EX void prespill(cell* c, eWall t, int rad, cell *from) {
  if(againstWind(c, from)) return;
  changes.ccell(c);
  // these monsters block spilling
  if(c->monst == moSeep || c->monst == moVineSpirit || c->monst == moShark ||
    c->monst == moGreaterShark || c->monst == moParrot || c->monst == moCShark)
    return;
  // turn statues into Slimes!
  if(among(c->wall, waBigStatue, waTerraWarrior) && t != waNone) {
    c->wall = waNone;
    c->monst = moSlimeNextTurn;
    }
  // slimedeath spill
  if((c->monst == moSlime || c->monst == moSlimeNextTurn) && t == waNone) {
    c->wall = waNone; attackMonster(c, 0, moNone);
    }
  if(c->wall == waClosedGate) {
    c->wall = waPalace;
    return;
    }
  // no slime in Whirlpool
  if(c->land == laWhirlpool) return;
  // these walls block spilling completely
  if(c->wall == waIcewall || c->wall == waBarrier ||  c->wall == waWarpGate ||
    c->wall == waDeadTroll || c->wall == waDeadTroll2 || 
    c->wall == waDune || c->wall == waAncientGrave || 
    c->wall == waThumperOff || c->wall == waThumperOn ||
    c->wall == waFreshGrave || c->wall == waColumn || c->wall == waPartialFire ||
    c->wall == waDeadwall || c->wall == waWaxWall || c->wall == waCamelot || c->wall == waRoundTable ||
    c->wall == waBigStatue || c->wall == waRed1 || c->wall == waRed2 || c->wall == waRed3 ||
    c->wall == waTower ||
    c->wall == waPalace || 
    c->wall == waPlatform || c->wall == waStone || c->wall == waTempWall ||
    c->wall == waTempFloor || c->wall == waTempBridge || c->wall == waPetrifiedBridge || c->wall == waTempBridgeBlocked || 
    c->wall == waSandstone || c->wall == waCharged || c->wall == waGrounded ||
    c->wall == waMetal || c->wall == waSaloon || c->wall == waFan ||
    c->wall == waBarrowDig || c->wall == waBarrowWall ||
    c->wall == waMirrorWall)
    return;
  if(c->wall == waFireTrap) {
    if(c->wparam == 0) c->wparam = 1;
    return;
    }
  if(c->wall == waExplosiveBarrel) 
    explodeBarrel(c);
  destroyTrapsOn(c);
  // these walls block further spilling
  if(c->wall == waCavewall || cellUnstable(c) || c->wall == waSulphur ||
    c->wall == waSulphurC || c->wall == waLake || c->wall == waChasm ||
    c->wall == waBigTree || c->wall == waSmallTree || c->wall == waTemporary ||
    c->wall == waVinePlant || isFire(c) || c->wall == waBonfireOff || c->wall == waVineHalfA || c->wall == waVineHalfB ||
    c->wall == waCamelotMoat || c->wall == waSea || c->wall == waCTree ||
    c->wall == waRubble || c->wall == waGargoyleFloor || c->wall == waGargoyle ||
    c->wall == waRose || c->wall == waPetrified || c->wall == waPetrifiedBridge || c->wall == waRuinWall ||
    among(c->wall, waDeepWater, waShallow)) {
      record_spillinfo(c, t);
      t = waTemporary;
      }

  if(c->wall == waSulphur) {
    // remove the center as it would not look good
    for(int i=0; i<c->type; i++) if(c->move(i) && c->move(i)->wall == waSulphurC)
      c->move(i)->wall = waSulphur;
    }
    
  if(isReptile(c->wall)) {
    if(c->monst || isPlayerOn(c)) kills[moReptile]++;
    else c->monst = moReptile, c->stuntime = 3, c->hitpoints = 3;
    }
    
  destroyHalfvine(c);
  if(c->wall == waTerraWarrior) kills[waTerraWarrior]++;
  record_spillinfo(c, t);
  c->wall = t;
  // destroy items...
  c->item = itNone;
  // block spill
  if(t == waTemporary) return;
  // cwt.at->item = itNone;
  if(rad) for(auto p: adj_minefield_cells_full(c)) {
    prespill(p.c, conditional_flip_slime(p.mirrored, t), rad-1, c);
    }
  }

EX eWall conditional_flip_slime(bool flip, eWall t) {
  if(flip && t == waFloorA) return waFloorB;
  if(flip && t == waFloorB) return waFloorA;
  return t;
  }

EX void spillfix(cell* c, eWall t, int rad) {
  if(c->wall == waTemporary) {
    changes.ccell(c);
    c->wall = t;
    }
  if(rad) for(auto p: adj_minefield_cells_full(c)) {
    spillfix(p.c, conditional_flip_slime(p.mirrored, t), rad-1);
    }
  }

EX void spill(cell* c, eWall t, int rad) {
  if(isAlchAny(c) && (spillinfos[c].spill_a || spillinfos[c].spill_b) && isAlchAny(spillinfos[c].orig))
    t = spillinfos[c].orig;
  prespill(c,t,rad, c);
  spillfix(c,t,rad);
  for(auto si: spillinfos) {
    if(si.second.spill_a && si.second.spill_b)
    si.first->wall =
      si.second.spill_a > si.second.spill_b ? waFloorA :
      si.second.spill_b > si.second.spill_a ? waFloorB :
      isAlchAny(si.second.orig) ? si.second.orig :
      waNone;
    }
  }

EX void degradeDemons() {
  addMessage(XLAT("You feel more experienced in demon fighting!"));
  playSound(cwt.at, "levelup");  
  int dcs = isize(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->monst == moGreaterM || c->monst == moGreater) {
      changes.ccell(c);
      achievement_gain_once("DEMONSLAYER");
      if(c->monst == moGreaterM) c->monst = moLesserM;
      if(c->monst == moGreater) c->monst = moLesser;
      }
    }  
  shmup::degradeDemons();
  }

EX void stunMonster(cell *c2, eMonster killer, flagtype flags) {
  int newtime = (
    c2->monst == moFatGuard ? 2 : 
    c2->monst == moSkeleton && c2->land != laPalace && c2->land != laHalloween ? 7 :
    c2->monst == moTerraWarrior ? min(int(c2->stuntime + 8 - c2->hitpoints), 7) :
    isMetalBeast(c2->monst) ? 7 :
    c2->monst == moTortoise ? 7 :
    c2->monst == moWorldTurtle ? 7 :
    c2->monst == moReptile ? 7 :
    isPrincess(c2->monst) ? 6 :
    // spear stunning
    isBull(c2->monst) ? 3 :
    (c2->monst == moGreater || c2->monst == moGreaterM) ? 5 :
    c2->monst == moButterfly ? 2 :
    c2->monst == moDraugr ? 1 :
    c2->monst == moVizier ? 0 :
    c2->monst == moHedge ? 1 :
    c2->monst == moFlailer ? 1 :
    c2->monst == moSalamander ? 6 :
    c2->monst == moBrownBug ? 3 :
    ((flags & AF_WEAK) && !attackJustStuns(c2, flags &~ AF_WEAK, killer)) ? min(5+c2->stuntime, 15) :
    3);
  if(killer == moArrowTrap) newtime = min(newtime + 3, 7);
  if(!(flags & AF_WEAK) && !isMetalBeast(c2->monst) && !among(c2->monst, moSkeleton, moReptile, moSalamander, moTortoise, moWorldTurtle, moBrownBug)) {
    c2->hitpoints--;
    if(c2->monst == moPrincess)
      playSound(c2, princessgender() ? "hit-princess" : "hit-prince");
    } 
  if(c2->stuntime < newtime) c2->stuntime = newtime;
  if(isBull(c2->monst)) c2->mondir = NODIR;
  checkStunKill(c2);
  }

EX bool attackJustStuns(cell *c2, flagtype f, eMonster attacker) {
  if(f & AF_WEAK)
    return true;
  if(f & AF_HORNS)
    return hornStuns(c2);
  else if(attacker == moArrowTrap && arrow_stuns(c2->monst))
    return true;
  else if((f & AF_SWORD) && c2->monst == moSkeleton)
    return false;
  else if(f & (AF_CRUSH | AF_MAGIC | AF_FALL | AF_EAT | AF_GUN | AF_PSI))
    return false;
  else
    return isStunnable(c2->monst) && c2->hitpoints > 1;
  }
  
EX void minerEffect(cell *c) {
  changes.ccell(c);
  eWall ow = c->wall;
  if(c->wall == waOpenGate || c->wall == waFrozenLake || c->wall == waBoat ||
    c->wall == waStrandedBoat ||
    c->wall == waCIsland || c->wall == waCIsland2 || c->wall == waTrapdoor || 
    c->wall == waGiantRug) ;
  else if(c->wall == waMineUnknown || c->wall == waMineMine || c->wall == waMineOpen)
    c->wall = waMineOpen;
  else if(c->wall == waRed2) c->wall = waRed1;
  else if(c->wall == waRed3) c->wall = waRed2;
  else if(isReptile(c->wall))
    c->wparam = 1; // wake up next turn
  else if(c->wall == waTempFloor) c->wall = waChasm;
  else if(c->wall == waTempBridge || c->wall == waPetrifiedBridge || c->wall == waTempBridgeBlocked) 
    placeWater(c, NULL);
  else if(doesFall(c))
    ow = waNone;
  else
    c->wall = waNone;
  if(c->wall != ow && ow) drawParticles(c, winf[ow].color, 16);
  } 

EX void killMutantIvy(cell *c, eMonster who) {
  if(checkOrb(who, itOrbStone)) petrify(c, waPetrified, moMutant);
  changes.ccell(c);
  removeIvy(c);
  for(int i=0; i<c->type; i++)
    if(c->move(i) && c->move(i)->mondir == c->c.spin(i) && (isMutantIvy(c->move(i)) || c->move(i)->monst == moFriendlyIvy))
      kills[c->move(i)->monst]++, killMutantIvy(c->move(i), who);
  if(c->land == laClearing) clearing::imput(c);
  }

EX bignum ivy_total() {
  return kills[moMutant] + kills[moFriendlyIvy] +
    kills[moIvyRoot] + kills[moIvyHead] + kills[moIvyBranch] + kills[moIvyWait] + kills[moIvyDead]
     + clearing::imputed;
  }

EX void killMonster(cell *c, eMonster who, flagtype deathflags IS(0)) {
  eMonster m = c->monst;
  DEBBI(DF_TURN, ("killmonster ", dnameof(m)));
  
  if(!m) return;
  
  if(m == moKrakenH) return;
  if(m == moKrakenT) {
    if(c->hitpoints && m == moKrakenT) kills[moKrakenT]++;
    c->hitpoints = 0;
    c->stuntime = 1;
    cell *head = kraken::head(c);
    if(kraken::totalhp(head) == 0) {
      kraken::kill(head, who);
      playSound(head, "die-kraken");
      }
    return;
    }
  
  if(isDragon(m)) {
    if(c->hitpoints && m != moDragonHead) kills[moDragonTail]++;
    c->hitpoints = 0;
    c->stuntime = 1;
    cell *head = dragon::findhead(c);
    if(dragon::totalhp(head) == 0) dragon::kill(head, who);
    return;
    }
  if(isWorm(c) && m != moTentacleGhost) return;
  
  bool fallanim = (deathflags & AF_FALL) &&  m != moMimic;

  int pcount = fallanim ? 0 : 16; 
  if(m == moShadow)
    kill_shadow_at(c);

#if CAP_HISTORY
  if(!isBug(m) && !isAnyIvy(m)) {
    history::killhistory.push_back(make_pair(c,m));
    }
#endif
  
  if(m == moGolemMoved) m = moGolem;
  if(m == moKnightMoved) m = moKnight;
  if(m == moSlimeNextTurn) m = moSlime;
  if(m == moLesserM) m = moLesser;
  if(m == moGreater) m = moLesser;
  if(m == moGreaterM) m = moLesser;
  if(isPrincess(m)) m = moPrincess;
  if(m == moTentacleGhost) m = moGhost;
  if(m == moHunterGuard) m = moHunterDog;
  if(m == moHunterChanging) m = moHunterDog;
  if(m == moWolfMoved) m = moWolf;
  if(!isBulletType(m) && m != moIvyDead) kills[m]++;

  if(saved_tortoise_on(c)) c->item = itNone;

  if(!c->item) if(m == moButterfly && (deathflags & AF_BULL))
    c->item = itBull;
  
  if(isRatling(m) && c->wall == waBoat) {
    bool avenge = false;
    for(int i=0; i<c->type; i++) if(!isWarpedType(c->move(i)->land))
      avenge = true;
    if(avenge)
      changes.value_add(avengers, 2);
    }
  
  if(m == moMirrorSpirit && who != moMimic && !(deathflags & (AF_MAGIC | AF_CRUSH))) {
    kills[m]--;
    changes.value_inc(mirrorspirits);
    }
  
  if(isMutantIvy(m) || m == moFriendlyIvy) {
    pcount = 0;
    if(isMutantIvy(m)) changes.at_commit([] { clearing::direct++; });
    changes.value_keep(clearing::imputed);
    bignum s = ivy_total() - 1;
    killMutantIvy(c, who);
    s = ivy_total() - s;
    if(vid.bubbles_special && s > bignum(1))
      drawBubble(c, 0xFFFF00, s.get_str(100), .5);
    if(isize(clearing::imputed.digits) > 11)
      achievement_gain_once("KILLMUTANT");      
    }
  
  if(m == moPrincess) {
    princess::info *i = princess::getPrincessInfo(c);
    if(i) {
      changes.value_keep(*i);
      i->princess = NULL;
      if(i->bestdist == OUT_OF_PALACE) {
        items[itSavedPrincess]--;
        if(items[itSavedPrincess] < 0) {
          printf("princess below 0\n");
          items[itSavedPrincess] = 0;
          }
        if(items[itSavedPrincess] == 0 && !inv::on) {
          items[itOrbLove] = 0;
          changes.value_keep(princess::reviveAt);
          princess::reviveAt = gold(NO_LOVE) + 20;
          }
        }
      if(princess::challenge) changes.at_commit([] { showMissionScreen(); });
      }
    }

  if(m == moGargoyle && c->wall != waMineMine) {
    bool connected = false;
    
    if(isGravityLand(c->land)) {
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->move(i);
        if(c2->wall == waPlatform || c2->wall == waGargoyle || c2->wall == waBarrier ||
          c2->wall == waDeadTroll || c2->wall == waDeadTroll2 || c2->wall == waTrunk ||
          c2->wall == waPetrified || isAlchAny(c2->wall))
          connected = true;
        }
      }
    else {
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->move(i);
        if(!cellUnstableOrChasm(c2) && !isWatery(c2)) connected = true;
        }
      }
    
    if(connected) petrify(c, waGargoyle, m), pcount = 0;
    }
  
  if(m == moIceGolem) {
    if(petrify(c, waIcewall, m)) pcount = 0;
    heat::affect(c, -1);
    forCellEx(c2, c) {
      changes.ccell(c2);      
      heat::affect(c2, -.5);
      }
    }
    
  if(m == moTroll) {
    petrify(c, waDeadTroll, m); pcount = 0;
    forCellEx(c1, c) {
      changes.ccell(c1);
      c1->item = itNone;
      if(c1->wall == waDeadwall) c1->wall = waCavewall;
      if(c1->wall == waDeadfloor2 && !c1->monst && !isPlayerOn(c1)) c1->wall = waCavewall;
      if(c1->wall == waDeadfloor) c1->wall = waCavefloor;
      }
    }
  if(m == moFjordTroll || m == moForestTroll || m == moStormTroll) {
    petrify(c, waDeadTroll2, m);
    }
  if(m == moMiner) {
    pcount = 32;
    playSound(c, "splash" + pick12());
    destroyHalfvine(c);
    minerEffect(c);
    #if CAP_COMPLEX2
    brownian::dissolve_brownian(c, 1);
    #endif
    forCellEx(c1, c) if(passable(c1, c, P_MONSTER | P_MIRROR | P_CLIMBUP | P_CLIMBDOWN)) {
      changes.ccell(c1);
      destroyHalfvine(c1);
      minerEffect(c1);
      #if CAP_COMPLEX2
      brownian::dissolve_brownian(c1, 1);
      #endif
      if(c1->monst == moSlime || c1->monst == moSlimeNextTurn)
        killMonster(c1, who);
      }
    forCellEx(c2, c) {
      changes.ccell(c2);
      if(c2->wall == waPalace) c2->wall = waRubble;
      if(c2->wall == waDeadwall) c2->wall = waDeadfloor2;
      if(c2->wall == waExplosiveBarrel) explodeBarrel(c2);
      }
    }
  if(m == moOrangeDog) {
    if(pcount) for(int i=0; i<8; i++) {
      drawParticle(c, 0xFFFFFF);
      drawParticle(c, 0x202020);
      }
    pcount = 0;
    }
  if(m == moDraugr) {
    if(pcount) drawParticles(c, 0x804000, 8);
    pcount = 0;
    }
  if(m == moPalace) {
    if(pcount) {
      pcount = 4;
      for(int i=0; i<12; i++) drawParticle(c, 0x20C020);
      }
    }
  if(m == moPrincess) {
    playSound(c, princessgender() ? "die-princess" : "die-prince");
    }
  if(m == moVineBeast) 
    petrify(c, waVinePlant, m), pcount = 0;
  if(isBird(m)) moveEffect(movei(c, FALL), moDeadBird);
  if(m == moAcidBird) {
    playSound(c, "die-bomberbird");
    pcount = 64;
    #if CAP_COMPLEX2
    brownian::dissolve(c, 1);
    #endif
    }
  if(m == moBomberbird || m == moTameBomberbird) {
    pcount = 0;
    playSound(c, "die-bomberbird");
    if(c->wall == waNone || c->wall == waMineUnknown || c->wall == waMineOpen ||
      c->wall == waCavefloor || c->wall == waDeadfloor || c->wall == waDeadfloor2 ||
      c->wall == waRubble || c->wall == waGargoyleFloor || 
      c->wall == waCIsland || c->wall == waCIsland2 || 
      c->wall == waStrandedBoat || c->wall == waRed1 || c->wall == waGiantRug) {
      c->wall = waMineMine;
      if(c->item) explodeMine(c);
      else if(c->land == laMinefield) c->landparam = 1;
      }
    else if(c->wall == waFrozenLake)
      c->wall = waLake;
    else if(c->wall == waGargoyleBridge)
      placeWater(c, c);
    else if(c->wall == waRed3 || c->wall == waRed2) {
      c->wall = waRed1;
      for(int i=0; i<c->type; i++) if(c->move(i)->wall == waRed3)
        c->move(i)->wall = waRed2;
      c->item = itNone;
      }
    eWall w = c->wall;
    if(isFire(c) || c->wall == waRose || isReptile(c->wall)) {
      c->wall = waMineMine;
      explodeMine(c);
      if(isReptile(w)) kills[moReptile]++;
      if(w == waReptile) c->wall = waChasm; 
      if(w == waReptileBridge) placeWater(c, NULL); 
      }      
    }
  if(m == moVineSpirit) {
    pcount = 32;
    playSound(c, "die-vinespirit");
    destroyHalfvine(c);
    if(!isFire(c)) c->wall = waNone;
    }
  if(m == moRedTroll) {
    playSound(c, "die-troll");
    if(doesFall(c)) fallingFloorAnimation(c, waRed1, m), pcount = 0;
    else if(snakepile(c, m)) pcount = 0;
    }
  if(m == moBrownBug) {
    if(doesFall(c)) ;
    else if(snakepile(c, m)) pcount = 0;
    }
  if(m == moDarkTroll) {
    playSound(c, "die-troll");
    if(doesFall(c)) fallingFloorAnimation(c, waDeadwall, m), pcount = 0;
    else if(c->wall == waRed1 || c->wall == waRed2 || c->wall == waRed3)
      c->wall = waDeadwall, pcount = 0;
    else if(snakepile(c, m))
      pcount = 0;
    }
  if(isWitch(m) && (isFire(c) || passable(c, NULL, P_MONSTER)) && !c->item) {
    if(m == moWitchFire) c->item = itOrbFire;
    if(m == moWitchFlash) c->item = itOrbFlash;
    if(m == moWitchGhost) c->item = itOrbAether;
    if(m == moWitchWinter) c->item = itOrbWinter;
    if(m == moWitchSpeed) c->item = itOrbSpeed;
    if(isFire(c) && itemBurns(c->item))
      c->item = itNone;
    }
  if(who == moPlayer || (isFriendly(who) && items[itOrbEmpathy])) {
    eItem o = frog_power(m);
    if(o && who != moPlayer) markOrb2(itOrbEmpathy);
    if(o) items[o] += 5;
    }
  if(checkOrb(who, itOrbStone))
    petrify(c, waPetrified, m), pcount = 0;
  if(m == moFireFairy) {
    drawFireParticles(c, 16); pcount = 0;
    playSound(c, "die-fairy");
    playSound(c, "fire");
    makeflame(c, 50, false);
    }
  if(c->monst == moMetalBeast2 && !c->item && who == moLightningBolt && c->wall != waPetrified && c->wall != waChasm)
    c->item = itFulgurite; // this is actually redundant in many cases
  if(m == moPyroCultist && c->item == itNone && c->wall != waChasm && c->wall != waPetrified) {
    // a reward for killing him before he shoots!
    c->item = itOrbDragon;
    }
  if(m == moOutlaw && (c->item == itNone || c->item == itRevolver) && c->wall != waChasm)
    c->item = itBounty;
  // note: an Orb appears underwater!
  if(m == moWaterElemental && c->item == itNone)
    c->item = itOrbWater;

  if(m == moPirate && isOnCIsland(c) && c->item == itNone && (
      eubinary ||
      (c->master->alt && celldistAlt(c) <= 2-getDistLimit()) ||
      isHaunted(c->land)) && !cryst) {
    bool toomany = false;
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->move(i);
      if(c2 && c2->item == itCompass) toomany = true;
      if(c2 && BITRUNCATED) for(int j=0; j<c2->type; j++)
        if(c2->move(j) && c2->move(j)->item == itCompass)
          toomany = true;
      }
    if(!toomany) c->item = itCompass;
    }
  if(m == moSlime) { 
    pcount = 0;
    drawParticles(c, winf[c->wall].color, 80, 200);
    playSound(c, "splash" + pick12());
    c->monst = moNone; 
    spill(c, c->wall, 2); 
    }
  // if(c->monst == moShark) c->heat += 1;
  // if(c->monst == moGreaterShark) c->heat += 10;
  if(isIcyLand(c)) {
    if(m == moCultist) HEAT(c) += 3;
    if(m == moPyroCultist) HEAT(c) += 6;
    if(m == moLesser) HEAT(c) += 10;
    }
  if(m == moLesser && !(kills[m] % 10))
    degradeDemons();
  if(m == moLesser) {
    if(kills[m] % 10) {
      if(vid.bubbles_special)
        drawBubble(c, 0xFF0000, its(kills[m]%10), 1);
      }
    else {
      if(vid.bubbles_special)
        drawBubble(c, 0xFF8000, "+1 XP", .8);
      degradeDemons();
      }
    }
  if(isIvy(c)) {    
    pcount = 0;
    eMonster m = c->monst;
    bignum s = ivy_total() - 1;
    /*if((m == moIvyBranch || m == moIvyHead) && c->move(c->mondir)->monst == moIvyRoot)
      ivynext(c, moIvyNext); */
    killIvy(c, who);
    s = ivy_total() - s;
    if(s > bignum(1) && vid.bubbles_special)
      drawBubble(c, 0xFFFF00, s.get_str(100), .5);
    if(m == moIvyBranch || m == moIvyHead || m == moIvyNext) {
      int qty = 0;
      cell *c2 = proper(c, c->mondir) ? c->move(c->mondir) : nullptr;
      if(!c2) c2 = c; /* should not happen */
      for(int i=0; i<c2->type; i++)
        if(c2->move(i)->monst == moIvyWait && c2->move(i)->mondir == c2->c.spin(i))
          qty++;
      if(c2->monst == moIvyRoot || qty) {
        c->monst = moIvyNext;
        /* c->monst = moIvyHead;
        ivynext(c);
        if(c->monst == moIvyHead) c->monst = moIvyNext;
        else c->monst = moNone; */
        }
      else {
        changes.ccell(c2);
        c2->monst = moIvyHead;
        }
      }
    }
  else if(c->monst == moTentacleGhost)
    c->monst = moTentacletail;
  else c->monst = moNone;

  if(m == moPair && c->move(c->mondir)->monst == moPair) {
    changes.ccell(c->move(c->mondir));
    killMonster(c->move(c->mondir), who, deathflags);
    }

  if(isMagneticPole(m) && c->move(c->mondir)->monst == otherpole(m)) {
    changes.ccell(c->move(c->mondir));
    killMonster(c->move(c->mondir), who, deathflags);
    }
  
  if(m == moEarthElemental) earthWall(c);
  if(m == moAlbatross && items[itOrbLuck]) 
    useupOrb(itOrbLuck, items[itOrbLuck] / 2);
  
  if(m == moAirElemental) {
    changes.value_keep(airmap);
    airmap.clear();
    for(int i=0; i<isize(dcal); i++)
      if(dcal[i]->monst == moAirElemental)
        airmap.push_back(make_pair(dcal[i],0));
    buildAirmap();
    }
  if(m == moMimic) {
    for(auto& m: mirror::mirrors) if(c == m.second.at) {
      drawParticles(c, mirrorcolor(m.second.mirrored), pcount);
      if(c->wall == waMirrorWall)
        drawParticles(c, mirrorcolor(!m.second.mirrored), pcount);
      }
    pcount = 0;
    }

  drawParticles(c, minf[m].color, pcount);
  if(fallanim) {
    fallingMonsterAnimation(c, m);
    }
  }

EX void fightmessage(eMonster victim, eMonster attacker, bool stun, flagtype flags) {

  if(isBird(attacker)) {
    playSound(NULL, "hit-axe"+pick123());
    addMessage(XLAT("%The1 claws %the2!", attacker, victim));
    }

  else if(isGhost(attacker))
    addMessage(XLAT("%The1 scares %the2!", attacker, victim));

  else if(isSlimeMover(attacker) && !stun) {
    playSound(NULL, "hit-crush"+pick123());
    addMessage(XLAT("%The1 eats %the2!", attacker, victim));
    }

  else if(flags & AF_EAT) {
    playSound(NULL, "hit-crush"+pick123());
    addMessage(XLAT("%The1 eats %the2!", attacker, victim));
    }

  else if(attacker == moLancer) {
    playSound(NULL, "hit-rose");
    addMessage(XLAT("%The1 pierces %the2!", attacker, victim));
    }

  else if(attacker == moEarthElemental) {
    playSound(NULL, "hit-crush"+pick123());
    addMessage(XLAT("%The1 punches %the2!", attacker, victim));
    }
  
  else if(attacker == moPlayer) {
    if(flags & (AF_SWORD | AF_SWORD_INTO)) {
      playSound(NULL, "hit-axe"+pick123());
      addMessage(XLAT("You slash %the1.", victim)); 
      if(victim == moGoblin) 
        achievement_gain_once("GOBLINSWORD");
      }
    else if(victim == moKrakenT || victim == moDragonTail || victim == moDragonHead) {
      playSound(NULL, "hit-sword"+pick123());
      addMessage(XLAT("You hit %the1.", victim)); // normal
      }
    else if(stun && victim == moVizier) {
      playSound(NULL, "hit-sword"+pick123());
      addMessage(XLAT("You hit %the1.", victim)); // normal
      }
    else if(stun) {
      playSound(NULL, "hit-sword"+pick123());
      addMessage(XLAT("You stun %the1.", victim)); // normal
      }
    else if(isNonliving(victim)) {
      playSound(NULL, "hit-sword"+pick123());
      addMessage(XLAT("You destroy %the1.", victim)); // normal
      }
    else if(flags & AF_STAB) {
      playSound(NULL, "hit-axe"+pick123());
      addMessage(XLAT("You stab %the1.", victim)); // normal
      }
    else if(flags & AF_APPROACH) {
      playSound(NULL, "hit-sword"+pick123());
      if(victim == moLancer)
        addMessage(XLAT("You trick %the1.", victim)); // normal
      else
        addMessage(XLAT("You pierce %the1.", victim)); // normal
      }
    else if(!peace::on) {
      playSound(NULL, "hit-sword"+pick123());
      addMessage(XLAT("You kill %the1.", victim)); // normal
      }
    }

  else {
    if(victim == moKrakenT || victim == moDragonTail || victim == moDragonHead) {
      playSound(NULL, "hit-crush"+pick123());
      addMessage(XLAT("%The1 hits %the2.", attacker, victim)); // normal
      }
    else if(stun && victim == moVizier) {
      playSound(NULL, "hit-sword"+pick123());
      addMessage(XLAT("%The1 hits %the2.", attacker, victim)); // normal
      }
    else if(stun) {
      playSound(NULL, "hit-sword"+pick123());
      addMessage(XLAT("%The1 attacks %the2!", attacker, victim)); // normal
      }
    else if(isNonliving(victim)) {
      playSound(NULL, "hit-sword"+pick123());
      addMessage(XLAT("%The1 destroys %the2!", attacker, victim)); // normal
      }
    else if(flags & AF_STAB) {
      playSound(NULL, "hit-axe"+pick123());
      addMessage(XLAT("%The1 stabs %the2.", attacker, victim));
      }
    else if(flags & AF_APPROACH) {
      playSound(NULL, "hit-sword"+pick123());
      addMessage(XLAT("%The1 tricks %the2.", attacker, victim));
      }
    else {
      playSound(NULL, "hit-sword"+pick123());
      addMessage(XLAT("%The1 kills %the2!", attacker, victim));
      }
    }
  }

EX bool notthateasy(eMonster m) {
  return 
    isMultitile(m) || isStunnable(m) || m == moDraugr;
  }

EX bool attackMonster(cell *c, flagtype flags, eMonster killer) {

  if((flags & AF_GETPLAYER) && isPlayerOn(c)) {
    killThePlayerAt(killer, c, flags);
    return true;
    }

  eMonster m = c->monst;
  int tk = tkills();
  
  int tkt = killtypes();
  
  bool dostun = attackJustStuns(c, flags, killer);
  
  if((flags & AF_BULL) && c->monst == moVizier && c->hitpoints > 1) {
    dostun = true;
    c->stuntime = 2;
    }
  
  bool eu = landUnlocked(laElementalWall);
  bool tu = landUnlocked(laTrollheim);
  
  if(flags & AF_MSG) fightmessage(m, killer, dostun, flags);
  if(dostun) 
    stunMonster(c, killer, flags);
  else 
    killMonster(c, killer, flags);

  if(peace::on) return false;
  
  int ntk = tkills();
  int ntkt = killtypes();
    
  if(tkt < R20 && ntkt >= R20 && in_full_game()) {
    addMessage(XLAT("You hear a distant roar!"));
    playSound(NULL, "message-roar");
    }
  
  if(tk == 0 && ntk > 0 && in_full_game() && !cheater) {
    if(notthateasy(m))
      addMessage(XLAT("Quite tough, for your first fight."));
    else {
      bool more = false;
      forCellEx(c2, cwt.at) forCellEx(c3, c2)
        if(c3->monst) more = true;
      if(!more)
        addMessage(XLAT("That was easy, but groups could be dangerous."));
      }
    }
    
  if(tk < 10 && ntk >= 10 && in_full_game() && !big_unlock)
    addMessage(XLAT("Good to know that your fighting skills serve you well in this strange world."));

  if(tk < R100/2 && ntk >= R100/2 && in_full_game())
    addMessage(XLAT("You wonder where all these monsters go, after their death..."));

  if(tk < R100 && ntk >= R100 && in_full_game())
    addMessage(XLAT("You feel that the souls of slain enemies pull you to the Graveyard..."));
  
  if(!tu && landUnlocked(laTrollheim) && in_full_game()) {
    playSound(c, "message-troll");
    addMessage(XLAT("%The1 says, \"I die, but my clan in Trollheim will avenge me!\"", m));
    }

  if(!eu && landUnlocked(laElementalWall) && in_full_game())
    addMessage(XLAT("After killing %the1, you feel able to reach the Elemental Planes!", m));
  
  if(m == moVizier && c->monst != moVizier && kills[moVizier] == 1 && in_full_game()) {
    addMessage(XLAT("Hmm, he has been training in the Emerald Mine. Interesting..."));
    princess::forceMouse = true;
    }
  
  if(m == moIvyRoot && ntk>tk)
    achievement_gain_once("IVYSLAYER");
    
  return ntk > tk;
  }

EX void pushMonster(const movei& mi) {
  if(mi.s->monst == moPair || isMagneticPole(mi.s)) {
    cell *other = mi.s->move(mi.s->mondir);
    int id = neighborId(mi.t, other);
    if(id == -1) {
      killMonster(mi.s, moPlayer);
      killMonster(other, moPlayer);
      return;
      }
    else {
      moveMonster(mi);
      if(mi.t->monst) {
        mi.t->mondir = id;
        other->mondir = mi.t->c.spin(id);
        }
      }
    }
  else 
    moveMonster(mi);
  auto& cf = mi.s;
  auto& ct = mi.t;
  if(ct->monst == moBrownBug) {
    int t = snakelevel(ct) - snakelevel(cf);
    if(t > 0)
      ct->stuntime = min(ct->stuntime + 2 * t, 7);
    }
  if(isBull(ct->monst)) ct->monst = moRagingBull;
  }

EX void killFriendlyIvy() {
  forCellEx(c2, cwt.at) if(c2->monst == moFriendlyIvy) 
    killMonster(c2, moPlayer, 0);
  }

EX bool monsterPushable(cell *c2) {
  if(markOrb(itCurseWeakness) && (c2->stuntime < 2 || attackJustStuns(c2, 0, moPlayer))) return false;
  if(isMultitile(c2->monst)) return false;
  return (c2->monst != moFatGuard && !(isMetalBeast(c2->monst) && c2->stuntime < 2) && c2->monst != moTortoise && c2->monst != moTerraWarrior && c2->monst != moVizier && c2->monst != moWorldTurtle);
  }  

EX bool should_switchplace(cell *c1, cell *c2) {
  if(isPrincess(c2->monst) || among(c2->monst, moGolem, moIllusion, moMouse, moFriendlyGhost))
    return true;
  
  if(peace::on) return c2->monst;
  return false;
  }

EX bool switchplace_prevent(cell *c1, cell *c2, struct pcmove& m) {
  if(!should_switchplace(c1, c2)) return false;
  if(peace::on && (isMultitile(c2->monst) || saved_tortoise_on(c2) || isDie(c2->monst))) {
    if(m.vmsg(miRESTRICTED)) addMessage(XLAT("Cannot switch places with %the1!", c2->monst));
    return true;
    }
  if(c1->monst && c1->monst != moFriendlyIvy) {
    if(m.vmsg(miRESTRICTED)) addMessage(XLAT("There is no room for %the1!", c2->monst));
    return true;
    }
  if(passable(c1, c2, P_ISFRIEND | (c2->monst == moTameBomberbird ? P_FLYING : 0))) return false;
  if(warningprotection_hit(c2->monst)) return true;
  return false;
  }

EX void handle_switchplaces(cell *c1, cell *c2, bool& switchplaces) {
  if(should_switchplace(c1, c2)) {
    bool pswitch = false;
    if(c2->monst == moMouse)
      princess::mouseSqueak(c2);
    else if(isPrincess(c2->monst)) {
      princess::line(c2);
      princess::move(match(c2, c1));
      }
    else  
      pswitch = true;
    c1->hitpoints = c2->hitpoints;
    c1->stuntime = c2->stuntime;
    placeGolem(c1, c2, c2->monst);
    if(cwt.at->monst != moNone && pswitch)
      addMessage(XLAT("You switch places with %the1.", c2->monst));
    c2->monst = moNone;
    switchplaces = true;
    }
  }  

EX bool flashWouldKill(cell *c, flagtype extra) {
  for(int t=0; t<c->type; t++) {
    cell *c2 = c->move(t);
    for(int u=0; u<c2->type; u++) {
      cell *c3 = c2->move(u);
      if(isWorm(c3)) continue; // immune to Flash
      if(isFriendly(c3)) continue; // player's allies and mounts don't count
      if(c3->monst == moEvilGolem) continue; // evil golems don't count
      if(c3 != c && (c3->monst || isPlayerOn(c3))) {
        bool b = canAttack(NULL, moWitchFlash, c3, c3->monst, AF_MAGIC | extra);
        if(b) return true;
        }
      }
    }
  return false;
  }

EX vector<cell*> gun_targets(cell *c) {
  manual_celllister cl;
  vector<int> dists;
  cl.add(c); dists.push_back(0);
  for(int i=0; i<isize(dists); i++) {
    cell *c1 = cl.lst[i];
    if(dists[i] <= 2)
    forCellEx(c2, c1)
      if(passable(c2, c1, P_BULLET | P_FLYING | P_MONSTER))
        if(cl.add(c2)) dists.push_back(dists[i] + 1);
    }
  return cl.lst;
  }

EX void fallMonster(cell *c, flagtype flags IS(0)) {
  attackMonster(c, flags, moNone);
  }

EX void killHardcorePlayer(int id, flagtype flags) {
  charstyle& cs = getcs(id);
  cell *c = playerpos(id);
  if(flags & AF_FALL)
    fallingMonsterAnimation(c, moPlayer);
  else for(int i=0; i<6; i++) {
    drawParticle(c, cs.skincolor >> 8, 100);
    drawParticle(c, cs.haircolor >> 8, 125);
    if(cs.charid)
      drawParticle(c, cs.dresscolor >> 8, 150);
    else
      drawParticle(c, cs.skincolor >> 8, 150);
    if(cs.charid == 3)
      drawParticle(c, cs.dresscolor2 >> 8, 175);
    else 
      drawParticle(c, cs.skincolor >> 8, 150);
    drawParticle(c, cs.swordcolor >> 8, 200);
    }
  
  if(multi::players > 1 && multi::activePlayers() > 1) {
    multi::leaveGame(id);
    }
  else {
    canmove = false;
    achievement_final(true);
    }
  }

EX bool suicidal;

EX void killThePlayer(eMonster m, int id, flagtype flags) {
  if(markOrb(itOrbShield)) return;
  if(shmup::on) {
    multi::cpid = id;
    shmup::killThePlayer(m);
    }
  else if(items[itOrbDomination] && playerpos(id)->monst) {
    addMessage(XLAT("%The1 tries to dismount you!", m));
    attackMonster(playerpos(id), AF_NORMAL, m);
    useupOrb(itOrbDomination, items[itOrbDomination]/2);
    }
  else if(items[itOrbShell] && !(flags & AF_EAT)) {
    addMessage(XLAT("%The1 attacks your shell!", m));
    useupOrb(itOrbShell, 10);
    if(items[itOrbShell] < 1) items[itOrbShell] = 1, orbused[itOrbShell] = true;
    }
  else if(hardcore) {
    addMessage(XLAT("You are killed by %the1!", m));
    killHardcorePlayer(id, flags);
    }
  else if(m == moLightningBolt && lastmovetype == lmAttack && isAlchAny(playerpos(id))) {
    addMessage(XLAT("You are killed by %the1!", m));
    addMessage(XLAT("Don't play with slime and electricity next time, okay?"));
    kills[moPlayer]++;
    items[itOrbSafety] = 0;
    }
  else {
//  printf("confused!\n");
    addMessage(XLAT("%The1 is confused!", m));
    changes.value_set(suicidal, true);
    }
  }

EX void killThePlayerAt(eMonster m, cell *c, flagtype flags) {
  for(int i: player_indices())
    if(playerpos(i) == c)
     killThePlayer(m, i, flags);
  }

#if HDR
template<class T> void do_swords(movei mi, eMonster who, const T& f) {
  for(int bb=0; bb<2; bb++) if(who == moPlayer && sword::orbcount(bb)) {
    cell *sf = sword::pos(mi.s, sword::dir[multi::cpid], bb);
    cell *st = sword::pos(mi.t, sword::shift(mi, sword::dir[multi::cpid]), bb);
    f(st, bb);
    if(sf != st && !isNeighbor(sf,st)) {
      // also attack the in-transit cell
      if(S3 == 3) {
        forCellEx(sb, sf) if(isNeighbor(sb, st) && sb != mi.s && sb != mi.t) f(sb, bb);
        }
      else {
        forCellEx(sb, mi.s) if(isNeighbor(sb, st) && sb != mi.t) f(sb, bb);
        forCellEx(sb, mi.t) if(isNeighbor(sb, sf) && sb != mi.s) f(sb, bb);
        }
      }
    }
  }
#endif

int lastdouble = -3;

EX void stabbingAttack(movei mi, eMonster who, int bonuskill IS(0)) {
  int numsh = 0, numflail = 0, numlance = 0, numslash = 0, numbb[2];
  
  numbb[0] = numbb[1] = 0;

  cell *mf = mi.s;
  cell *mt = mi.t;
  int backdir = mi.rev_dir_mirror();
  
  do_swords(mi, who, [&] (cell *c, int bb) { if(swordAttack(mt, who, c, bb)) numbb[bb]++, numslash++; });
  
  for(int bb=0; bb<2; bb++) achievement_count("SLASH", numbb[bb], 0);

  if(peace::on) return;
  
  for(int t=0; t<mf->type; t++) {
    cell *c = mf->move(t);
    if(!c) continue;
    
    bool stabthere = false, away = true;
    if(logical_adjacent(mt, who, c)) stabthere = true, away = false;
    if(inmirror(c)) c = mirror::reflect(c).at;

    if(stabthere && c->wall == waExplosiveBarrel && markOrb(itOrbThorns))
      explodeBarrel(c);
    
    if(stabthere && canAttack(mt,who,c,c->monst,AF_STAB)) {
      changes.ccell(c);
      if(c->monst != moHedge) { 
        markOrb(itOrbThorns); if(who != moPlayer) markOrb(itOrbEmpathy);
        }
      eMonster m = c->monst;
      int k = tkills();
      if(attackMonster(c, AF_STAB | AF_MSG, who))  {
        spread_plague(mt, c, t, who);
        produceGhost(c, m, who);
        }
      if(tkills() > k) numsh++;
      }

    if(away && c != mt && canAttack(mf,who,c,c->monst,AF_BACK)) {
      changes.ccell(c);
      if(c->monst == moVizier && c->hitpoints > 1) {
        fightmessage(c->monst, who, true, AF_BACK);
        c->hitpoints--;
        // c->stuntime = 1;
        }
      else {
        eMonster m = c->monst;
        if(c->monst != moFlailer) {
          playSound(NULL, "hit-sword"+pick123());
          fightmessage(c->monst, who, false, AF_BACK);
          }
        else {
          playSound(NULL, "hit-sword"+pick123());
          if(who != moPlayer)
            addMessage(XLAT("%The1 tricks %the2.", who, c->monst));
          else
            addMessage(XLAT("You trick %the1.", c->monst));
          }
        if(c->monst == moFlailer && isPrincess(who) && isUnarmed(who))
          achievement_gain_once("PRINCESS_PACIFIST");

        if(attackMonster(c, 0, who)) {
          numflail++;
          spread_plague(mf, c, t, who);
          produceGhost(c, m, who);
          }
        }
      }
    }

  if(!isUnarmed(who)) forCellIdEx(c, t, mt) {
    if(!logical_adjacent(mt, who, c)) continue;
    eMonster mm = c->monst;
    int flag = AF_APPROACH;
    if(proper(mt, backdir) && anglestraight(mt, backdir, t)) flag |= AF_HORNS;
    if(canAttack(mt,who,c,c->monst, flag)) {
      changes.ccell(c);
      if(attackMonster(c, flag | AF_MSG, who)) numlance++;
      spread_plague(mt, c, t, who);
      produceGhost(c, mm, who);
      }
    }

  if(who == moPlayer) {
    if(numsh) achievement_count("STAB", numsh, 0);
    
    if(numlance && numflail && numsh) achievement_gain_once("MELEE3");
  
    if(numlance + numflail + numsh + numslash + bonuskill >= 5) achievement_gain_once("MELEE5");
  
    if(numsh == 2) {
      if(lastdouble == turncount-1) achievement_count("STAB", 4, 0);
      changes.value_set(lastdouble, turncount);
      }
    }
  }

}
