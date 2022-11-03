// Hyperbolic Rogue - monster movement
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file monstermove.cpp
 *  \brief monster movement
 */

#include "hyper.h"

namespace hr {

EX int turncount;

EX int mutantphase;

EX int sagephase = 0;

/** list of cells that the monsters are targetting (PCs, allies, Thumpers, etc.) */
EX vector<cell*> targets;

/** monsters to move, ordered by the number of possible good moves */
grow_vector<vector<cell*>> movesofgood;

EX vector<pair<cell*, int> > butterflies;

EX void addButterfly(cell *c) {
  if(shmup::on) return;
  for(int i=0; i<isize(butterflies); i++)
    if(butterflies[i].first == c) {
      butterflies[i].second = 0;
      return;
      }
  butterflies.push_back(make_pair(c, 0));
  }

EX void makeTrollFootprints(cell *c) {
  if(c->land != laTrollheim) return;
  if(c->item == itTrollEgg && c->landparam) return;
  c->landparam = turncount + 100;
  }

EX bool hasPrincessWeapon(eMonster m) {
  return m == moPalace || m == moFatGuard;
  }

EX void sageheat(cell *c, double v) {
  HEAT(c) += v;
  if(c->wall == waFrozenLake && HEAT(c) > .6) c->wall = waLake;
  }

bool sagefresh = true;

/** effect of moving monster m from cf to ct
 *  this is called from moveMonster, or separately from moveIvy/moveWorm,
 *  or when a dead bird falls (then m == moDeadBird)
 */

EX void moveEffect(const movei& mi, eMonster m) {

  auto& cf = mi.s;
  auto& ct = mi.t;
  if(cf) destroyWeakBranch(cf, ct, m);

  mayExplodeMine(ct, m);
  
  #if CAP_COMPLEX2
  if(!isNonliving(m)) terracotta::check_around(ct);
  #endif
 
  if(ct->wall == waMineUnknown && !ct->item && !ignoresPlates(m) && normal_gravity_at(ct)) 
    ct->landparam |= 2; // mark as safe

  if((ct->wall == waClosePlate || ct->wall == waOpenPlate) && !ignoresPlates(m) && normal_gravity_at(ct))
    toggleGates(ct, ct->wall);
  if(m == moDeadBird && cf == ct && cellUnstable(cf) && normal_gravity_at(ct)) {
    fallingFloorAnimation(cf);
    cf->wall = waChasm;
    }
  
  if(ct->wall == waReptile) ct->wparam = -1;
    
  if(ct->wall == waArrowTrap && !ignoresPlates(m) && normal_gravity_at(ct))
    activateArrowTrap(ct);

  if(ct->wall == waFireTrap && !ignoresPlates(m) && ct->wparam == 0 && normal_gravity_at(ct)) {
    playSound(ct, "click");
    ct->wparam = 1;
    }
    
  if(cf && isPrincess(m)) princess::move(mi);
  
  #if CAP_COMPLEX2
  if(cf && m == moKnight) camelot::move_knight(cf, ct);
  #endif
  
  if(cf && m == moTortoise) {
    tortoise::move_adult(cf, ct);
    }
  
  if(cf && ct->item == itBabyTortoise && !cf->item) {
    cf->item = itBabyTortoise;
    ct->item = itNone;
    animateMovement(mi.rev(), LAYER_BOAT);
    tortoise::move_baby(cf, ct);
    }
  
  #if CAP_COMPLEX2
  if(isDie(m) && mi.proper())
    dice::roll(mi);
  #endif
  }

EX void check_beauty(cell *ct, cell *cf, eMonster m) {
  bool adj = false;
  if(ct->cpdist == 1 && (items[itOrb37] || !nonAdjacent(cf,ct)) && markOrb(itOrbBeauty) && !isFriendly(ct))
    adj = true;

  if(!adj && items[itOrbEmpathy] && items[itOrbBeauty] && !isFriendly(ct)) {
    for(int i=0; i<ct->type; i++) if(ct->move(i) && isFriendly(ct->move(i)))
      adj = true, markOrb(itOrbEmpathy), markOrb(itOrbBeauty);
    }

  if(adj && ct->stuntime == 0 && !isMimic(m)) {
    ct->stuntime = 2;
    checkStunKill(ct);
    }
  }

EX void moveMonster(const movei& mi) {
  auto& cf = mi.s;
  auto& ct = mi.t;
  eMonster m = cf->monst;
  changes.ccell(cf);
  changes.ccell(ct);
  bool fri = isFriendly(cf);
  if(isDragon(m)) {
    printf("called for Dragon\n");
    return;
    }
  if(m != moMimic) animateMovement(mi, LAYER_SMALL);
  // the following line is necessary because otherwise plates disappear only inside the sight range
  if(cellUnstable(cf) && !ignoresPlates(m)) {
    fallingFloorAnimation(cf);
    changes.ccell(cf);
    cf->wall = waChasm;
    }
  moveEffect(mi, m);
  if(ct->wall == waCamelotMoat && 
    (m == moShark || m == moCShark || m == moGreaterShark))
      achievement_gain_once("MOATSHARK");
  if(m == moTentacleGhost) { 
    changes.ccell(cf);
    cf->monst = moTentacletail;
    m = moGhost;
    }
  else cf->monst = moNone;  
  if(ct->monst == moTentacletail && m == moGhost) {
    ct->monst = moTentacleGhost;
    }
  else {
    ct->monst = m;
    if(m == moWolf) ct->monst = moWolfMoved;
    if(m == moHunterChanging) ct->stuntime = 1;
    int d =neighborId(ct, cf);
    if(ct->monst != moTentacleGhost)
      ct->mondir = d;
    if(d >= 0)
      ct->monmirror = cf->monmirror ^ ct->c.mirror(d);
    }
  ct->hitpoints = cf->hitpoints;
  ct->stuntime = cf->stuntime;
  
  if(isMagneticPole(m) || m == moPair) {
    if(cf->mondir == 15) {
      ct->monst = moPirate;
      return;
      }
    cell *other_pole = cf->move(cf->mondir);
    if(other_pole) {
      ct->mondir = neighborId(ct, other_pole),
      other_pole->mondir = neighborId(other_pole, ct);
      }
    }
  
  if(fri || isBug(m) || items[itOrbDiscord]) stabbingAttack(mi, m);
  
  if(mi.d == JUMP && m == moVaulter) {
    cell *cm = common_neighbor(cf, ct);
    changes.ccell(cm);
    if(cm->wall == waShrub) cm->wall = waNone;
    if(cm->wall == waSmallTree) cm->wall = waNone;
    if(cm->wall == waBigTree) cm->wall = waSmallTree;
    if(cm->wall == waExplosiveBarrel) explodeBarrel(cm);
    if(cm->monst)
      attackMonster(cm, AF_NORMAL | AF_MSG | AF_GETPLAYER, m);
    ct->mondir = JUMP;
    }

  if(isLeader(m)) {
    if(ct->wall == waBigStatue) {
      ct->wall = cf->wall;
      ct->wparam = cf->wparam;
      cf->wall = waBigStatue;
      animateMovement(mi.rev(), LAYER_BOAT);
      }

    moveBoatIfUsingOne(mi);
    }
  
  if(isTroll(m)) { makeTrollFootprints(ct); makeTrollFootprints(cf); }
    
  int inc = incline(cf, ct);

  if(m == moEarthElemental) {
    if(!passable(ct, cf, 0)) earthFloor(ct);
    earthMove(mi);
    }

  if(m == moWaterElemental) {
    placeWater(ct, cf);
    for(int i=0; i<ct->type; i++) {
      cell *c2 = ct->move(i);
      if(!c2) continue;
      if(c2->wall == waBoat && !(isPlayerOn(c2) && markOrb(itOrbWater))) {
        addMessage(XLAT("%The1 is washed away!", c2->wall, moWaterElemental));
        placeWater(c2, ct);
        }
      else if(c2->wall == waStrandedBoat) {
        addMessage(XLAT("%The1 is washed away!", c2->wall, moWaterElemental));
        c2->wall = waNone;
        }
      else if(c2->wall == waDeadTroll) {
        addMessage(XLAT("%The1 is washed away!", c2->wall, moWaterElemental));
        c2->wall = waCavefloor;
        }
      else if(c2->wall == waDeadTroll2) {
        addMessage(XLAT("%The1 is washed away!", c2->wall, moWaterElemental));
        c2->wall = waNone;
        }
      else if(isFire(c2) && c2->wall != waEternalFire) {
        addMessage(XLAT("%The1 is extinguished!", c2->wall, moWaterElemental));
        if(c2->wall == waBurningDock)
          c2->wall = waDock;
        else
          c2->wall = waNone;
        }
      if(shmup::on && isWatery(c2)) shmup::destroyBoats(c2);
      }
    }

  if(m == moGreaterShark) for(int i=0; i<ct->type; i++) {
    cell *c3 = ct->move(i);
    if(c3 && c3->wall == waBoat)
      makeflame(c3, 5, false);
    }

  // lancers pierce our friends :(
  if(m == moLancer) { 
    // printf("lancer stab?\n");
    forCellEx(c3, ct) if(!logical_adjacent(cf, m, c3)) {
      if(canAttack(ct, moLancer, c3, c3->monst, AF_LANCE | AF_GETPLAYER)) {
        attackMonster(c3, AF_LANCE | AF_MSG | AF_GETPLAYER, m);
        }
      // this looks the same as effect graphically as exploding right away,
      // except that it does not kill the lancer
      if(c3->wall == waExplosiveBarrel) 
        c3->wall = waFireTrap, c3->wparam = 2;
      }
    }
  
  if(m == moWitchFire) makeflame(cf, 10, false);
  if(m == moFireElemental) { makeflame(cf, 20, false); if(cf->wparam < 20) cf->wparam = 20; }
  
  check_beauty(ct, cf, m);
  
  if(!cellEdgeUnstable(ct)) {
    if(isMetalBeast(m)) ct->stuntime += 2;
    if(m == moTortoise) ct->stuntime += 3;
    if(m == moWorldTurtle) ct->stuntime += 3;
    if(m == moDraugr && ct->land != laBurial && ct->land != laHalloween) ct->stuntime += 2;
    if(m == moBrownBug && snakelevel(ct) < snakelevel(cf)) ct->stuntime += 2;
    if(m == moBrownBug && snakelevel(ct) < snakelevel(cf) - 1) ct->stuntime += 2;
    if(m == moBrownBug && isWatery(ct) && !isWatery(cf)) ct->stuntime += 2;
    }
  
  if(isWitch(m) && ct->item == itOrbLife && passable(cf, NULL, P_MIRROR)) {
    // note that Fire Witches don't pick up Orbs of Life,
    addMessage(XLAT("%The1 picks up %the2!", moWitch, ct->item));
    cf->monst = moEvilGolem; ct->item = itNone;
    }
  else if(m == moWitch) {
    bool pickup = false;
    if(ct->item == itOrbFlash)
      pickup = true, m = moWitchFlash;
    if(ct->item == itOrbWinter)
      pickup = true, m = moWitchWinter;
    if(ct->item == itOrbAether)
      pickup = true, m = moWitchGhost;
    if(ct->item == itOrbFire)
      pickup = true, m = moWitchFire;
    if(ct->item == itOrbSpeed)
      pickup = true, m = moWitchSpeed;
    if(ct->item == itOrbLife)
      pickup = true, cf->monst = moEvilGolem;
    if(pickup) {
      addMessage(XLAT("%The1 picks up %the2!", moWitch, ct->item));
      ct->monst = m; ct->item = itNone;
      // speedwitches are stunned to prevent them from making a move
      // immediately
      if(m == moWitchSpeed) ct->stuntime = 1;
      }
    }

  if(m == moAirElemental) airmap.push_back(make_pair(ct, 0));
  if(m == moWolf && ct->land == laVolcano) ct->monst = moLavaWolf;
  if(m == moLavaWolf && isIcyLand(ct)) ct->monst = moWolfMoved;
  
  if(m == moPair) ct->stuntime++;

  if(inc == -3 && ct->monst == moReptile)
    ct->stuntime =3;
  else if(inc == 2 && ct->monst == moReptile)
    ct->stuntime = 2;
  else if(inc == 3 && ct->monst == moReptile)
    ct->stuntime = 3;
  else if(inc == -3 && !survivesFall(ct->monst) && !passable(cf, ct, P_MONSTER)) {
    addMessage(XLAT("%The1 falls!", ct->monst));
    fallMonster(ct, AF_FALL);
    if(isBull(m) && cf->wall == waRed3)
      ct->wall = waRed1;
    }
  if(isThorny(ct->wall) && !survivesThorns(ct->monst)) {
    addMessage(XLAT("%The1 is killed by thorns!", ct->monst));
    playSound(ct, "hit-rose");
    if(isBull(ct->monst)) ct->wall = waNone;
    fallMonster(ct, AF_CRUSH);
    }
  if(sword::at(ct) && canAttack(NULL, moPlayer, ct, m, AF_SWORD_INTO)) {
    attackMonster(ct, AF_SWORD_INTO | AF_MSG, moPlayer);
    achievement_gain_once("GOSWORD");
    }

  if(ct->mpdist == 7 && cf->mpdist > 7) {
    playSeenSound(ct);
    }
  }

EX bool cannotGo(eMonster m, cell *c) {
  if(m == moCrystalSage && (c->land != laCocytus || HEAT(c) > SAGEMELT || allPlayersInBoats()))
    return true;
  return false;
  }

EX bool wantsToStay(eMonster m) {
  return m == moCrystalSage && allPlayersInBoats();
  }

EX bool batsAfraid(cell *c) {
  // bats 
  for(int i=0; i<isize(targets); i++)
    if(c == targets[i] || isNeighbor(c, targets[i])) {
      if(!targets[i]->monst && invismove) continue;
      bool enear = false;
      forCellEx(c, targets[i])
        forCellEx(c2, c)
          forCellEx(c3, c2)
            if(isActiveEnemy(c3, targets[i]->monst) && c3->monst != moBat && 
              passable_for(c3->monst, c2, c3, 0) &&
              passable_for(c3->monst, c, c2, 0)
              )
            enear = true;
      if(!enear) return true;
      }
  return false;
  }

EX int angledist(int t, int d1, int d2) {
  int dd = d1 - d2;
  while(dd<0) dd += t;
  while(dd>t/2) dd -= t;
  if(dd<0) dd = -dd;
  return dd;
  }

EX int angledistButterfly(int t, int d1, int d2, bool mirrored) {
  int dd = d1 - d2;
  if(mirrored) dd = -dd;
  while(dd<0) dd += t;
  return dd;
  }

EX int angledist(cell *c, int d1, int d2) { 
  return angledist(c->type, d1, d2);
  }

EX bool anglestraight(cell *c, int d1, int d2) { 
  return angledist(c->type, d1, d2) >= c->type / 2;
  }

EX int bulldist(cell *c) {
  int low = 0;
  forCellEx(c2, c) if(c2->cpdist < c->cpdist) low++;
  return 8 * c->cpdist - low;
  }

EX int bulldistance(cell *c, cell *d) {
  int low = 0;
  int cd = celldistance(c, d);
  forCellEx(c2, c) if(celldistance(c2, d) < cd) low++;
  return 8 * cd - low;
  }

EX int landheattype(cell *c) {
  if(isIcyLand(c)) return 0;
  if(c->land == laVolcano) return 2;
  return 1;
  }

/** for the monster at c1, evaluation of the move to c2
 *  @param mf what moves are allowed
 */

EX int moveval(cell *c1, cell *c2, int d, flagtype mf) {
  if(!c2) return -5000;
  
  eMonster m = c1->monst;

  // Angry Beasts can only go forward
  if(m == moRagingBull && c1->mondir != NODIR && !anglestraight(c1, c1->mondir, d)) return -1700;

  // never move against a rose
  if(againstRose(c1, c2) && !ignoresSmell(m)) return -1600;

  // worms cannot attack if they cannot move
  if(isWorm(m) && !passable_for(c1->monst, c2, c1, P_MONSTER)) return -1700;
  
  if(canAttack(c1, m, c2, c2->monst, AF_GETPLAYER | mf) && !(mf & MF_NOATTACKS)) {
    if(m == moRagingBull && c1->mondir != NODIR) return -1700;
    if(mf & MF_MOUNT) {
      if(c2 == dragon::target) return 3000;
      else if(isFriendlyOrBug(c2)) return 500;
      else return 2000;
      }
    if(isPlayerOn(c2)) return peace::on ? -1700 : 2500;
    else if(isFriendlyOrBug(c2)) return peace::on ? -1600 : 2000;
    else return 500;
    }
  
  if(!passable_for(c1->monst, c2, c1, 0)) 
    return 
      // never move into a wall
      (passable_for(c1->monst, c2, c1, P_DEADLY)) ? -1300 : 
      -1700; // move impossible
  
  if(slowMover(m) && nogoSlow(c2, c1)) return -1300;
    
  if(isPlayerOn(c2)) return -1700; // probably shielded

  if((mf & MF_MOUNT) && c2 == dragon::target) return 3000;

  // crystal sages would die out of Cocytus
  if(cannotGo(m, c2)) return -600;
  
  // Rose Beauties keep to the Rose Garden
  if(m == moRoseBeauty && c2->land != laRose) return -600;
  
  if(wantsToStay(m)) return 750;
  
  if((m == moRatling || m == moRatlingAvenger) && lastmovetype == lmSkip && !items[itFatigue]) return 650;

  if(m == moLancer) { 
    bool lancerok = true;
    forCellEx(c3, c2) if(c1 != c3 && !logical_adjacent(c1, m, c3))
      if(canAttack(c2, moLancer, c3, c3->monst, AF_LANCE | AF_ONLY_ENEMY))
        lancerok = false;
    if(!lancerok) return 750;
    }

  bool hunt = true;
  
  if(m == moLavaWolf) {
    // prefers to keep to volcano
    int clht = landheattype(c1);
    int dlht = landheattype(c2);
    if(dlht > clht) return 1510;
    if(dlht < clht) return 700;
    // will not hunt the player if these rules do not allow it
    bool onlava = false;
    for(cell *c: targets) {
      if(landheattype(c) >= clht) onlava = true;
      forCellEx(cc, c) if(landheattype(cc) >= clht) onlava = true;
      }
    if(!onlava) hunt = false;
    }
  
  if(m == moWolf) {
    int val = 1500;
    if(c2->land == laVolcano) return 1510;
    if(heat::absheat(c2) <= heat::absheat(c1))
      return 900;
    for(int i=0; i<c1->type; i++) {
      cell *c3 = c1->move(i);
      if(heat::absheat(c3) > heat::absheat(c2))
        val--;
      }
    return val;
    }
  
  if((mf & MF_MOUNT) && dragon::target) 
    return 1500 + celldistance(c1, dragon::target) - celldistance(c2, dragon::target);

  // Goblins avoid getting near the Sword
  if(m == moGoblin && sword::isnear(c2)) return 790;
  if(m == moBat && batsAfraid(c2)) return 790;
  
  if(m == moButterfly)
    return 1500 + angledistButterfly(c1->type, c1->mondir, d, c1->monmirror);
  
  if(m == moRagingBull && c1->mondir != NODIR)
    return 1500 - bulldist(c2);
  
  // actually they just run away
  if(m == moHunterChanging && c2->pathdist > c1->pathdist) return 1600;
  
  if((mf & MF_PATHDIST) && !pathlock) printf("using MF_PATHDIST without path\n"); 
  
  int bonus = 0;
  if(m == moBrownBug && snakelevel(c2) < snakelevel(c1)) bonus = -10;

  if(hunt && (mf & MF_PATHDIST) && c2->pathdist < c1->pathdist && !peace::on) return 1500 + bonus; // good move
  
  // prefer straight direction when wandering
  int dd = angledist(c1, c1->mondir, d);
    
  // goblins blocked by anglophobia prefer to move around than to stay
  if(m == moGoblin) {
    bool swn = false;
    forCellEx(c3, c1) if(sword::isnear(c3)) swn = true;
    if(swn) dd += 210;
    }

  return 800 + dd;
  }

// stay value
EX int stayval(cell *c, flagtype mf) {
  if(isShark(c->monst) && !isWatery(c))
    return 525;
  if(againstRose(c, NULL) && !ignoresSmell(c->monst)) return -1500;
  if(!passable_for(c->monst, c, NULL, P_MONSTER | P_MIRROR))
    return 525;
  if(cellEdgeUnstable(c))
    return -1500;
  if(isRatling(c->monst) && lastmovetype != lmSkip)
    return 700;
  // Goblins avoid staying near the Sword (if there is no choice, movement is preferred)
  if(c->monst == moGoblin && sword::isnear(c)) return 780;
  // Vikings move in a roughly straight line even if they cannot detect you
  if(c->monst == moViking && c->wall == waBoat)
    return 750;
  // in peaceful, all monsters are wandering
  if(peace::on && c->monst != moTortoise) return 750;
  if(isWorm(c->monst)) return 550;
  if(c->monst == moRagingBull) return -1690; // worse than to stay in place
  if(c->monst == moBat && batsAfraid(c)) return 575;
  if(c->monst == moHunterGuard) return 1600; // prefers to stay in place
  // Lava Wolves will wander if not hunting
  if(c->monst == moLavaWolf) return 750;
  return 1000;
  }

EX int totalbulldistance(cell *c, int k) {
  shpos.resize(SHSIZE);
  int tbd = 0;
  for(int p: player_indices()) {
    cell *c2  = shpos[(cshpos+SHSIZE-k-1)%SHSIZE][p];
    if(c2) tbd += bulldistance(c, c2);
    }
  return tbd;
  }

EX void determinizeBull(cell *c, vector<int>& posdir) {
  // determinize the Angry Beast movement:
  // use the previous PC's positions as the tiebreaker
  int nc = isize(posdir);
  for(int k=0; k<SHSIZE && nc>1; k++) {
    vector<int> pts(nc);
    for(int d=0; d<nc; d++) pts[d] = totalbulldistance(c->cmove(posdir[d]), k);

    int bestpts = 1000;
    for(int d=0; d<nc; d++) if(pts[d] < bestpts) bestpts = pts[d];
    int nc0 = 0;
    for(int d=0; d<nc; d++) if(pts[d] == bestpts) posdir[nc0++] = posdir[d];
    nc = nc0;
    }
  posdir.resize(nc);
  }

EX int determinizeBullPush(cellwalker bull) {
  vector<int> dirs(2);
  int positive;
  bull += wstep;
  cell *c2 = bull.at;
  if(!(c2->type & 1)) return 1; // irrelevant
  int d = c2->type / 2;
  bull += d; dirs[0] = positive = bull.spin;
  bull -= 2*d; dirs[1] = bull.spin;
  determinizeBull(c2, dirs);
  if(dirs[0] == positive) return -1;
  return 1;
  }    

vector<int> global_posdir;

EX int pickMoveDirection(cell *c, flagtype mf) {
  int bestval = stayval(c, mf);
  global_posdir = {-1};

  // printf("stayval [%p, %s]: %d\n", c, dnameof(c->monst), bestval);
  for(int d=0; d<c->type; d++) {
    cell *c2 = c->move(d);
    int val = moveval(c, c2, d, mf);
    // printf("[%d] %p: val=%5d pass=%d\n", d, c2, val, passable(c2,c,0));
    if(val > bestval) global_posdir.clear(), bestval = val;
    if(val == bestval) global_posdir.push_back(d);
    }
  
  if(c->monst == moRagingBull) 
    determinizeBull(c, global_posdir);
    
  return hrand_elt(global_posdir, -1);
  }

EX int pickDownDirection(cell *c, flagtype mf) {
  vector<int> downs;
  int bestdif = -100;
  forCellIdEx(c2, i, c) {
    if(gravityLevelDiff(c2, c) < 0 && passable_for(c->monst, c2, c, P_MIRROR) &&
      !isPlayerOn(c2)) {
      int cdif = i-c->mondir;
      if(cdif < 0) cdif += c->type;
      if(cdif > c->type/2) cdif = cdif - c->type;
      if(cdif < 0) cdif = -2*cdif+1; else cdif = 2*cdif;
      // printf("i=%d md=%d dif=%d\n", i, c->mondir, cdif);
      if(c2->wall == waClosePlate || c->wall == waClosePlate)
        cdif += 20;
      if(cdif > bestdif) bestdif = cdif, downs.clear();
      if(cdif == bestdif) downs.push_back(i);
      }
    }
  return hrand_elt(downs, -1);
  }

// Angry Beast attack
// note: this is done both before and after movement
EX void beastAttack(cell *c, bool player, bool targetdir) {
  if(c->mondir == NODIR) return;
  forCellIdEx(c2, d, c) {
    bool opposite = targetdir ? (d==c->mondir) : anglestraight(c, d, c->mondir);
    int flags = AF_BULL;
    if(player) flags |= AF_GETPLAYER;
    if(!opposite) flags |= AF_ONLY_FBUG;
    if(canAttack(c, moRagingBull, c2, c2->monst, flags)) {
      attackMonster(c2, flags | AF_MSG, moRagingBull);
      if(c2->monst && c2->stuntime) {
        cellwalker bull (c, d);
        int subdir = determinizeBullPush(bull);
        auto mi = determinePush(bull, subdir, [c2] (movei mi) { return passable(mi.t, c2, P_BLOW) && !isPlayerOn(mi.t); });
        if(mi.proper())
          pushMonster(mi);
        }
      }
    if(c2->wall == waThumperOff) {
      playSound(c2, "click");
      c2->wall = waThumperOn;
      c2->wparam = 100;
      }
    if(c2->wall == waExplosiveBarrel) {
      playSound(c2, "click");
      explodeBarrel(c2);
      }
    if(c2->wall == waThumperOn) {
      cellwalker bull (c, d);
      int subdir = determinizeBullPush(bull);
      auto mi = determinePush(bull, subdir, [c] (movei mi) { return canPushThumperOn(mi, c); });
      if(mi.proper())
        pushThumper(mi);
      }
    }
  }

EX bool quantum;

EX cell *moveNormal(cell *c, flagtype mf) {
  eMonster m = c->monst;
  if(isPowerMonster(m) && !playerInPower()) return NULL;
  
  int d;
  
  if(c->stuntime) {
    if(cellEdgeUnstable(c, MF_STUNNED)) d = pickDownDirection(c, mf), global_posdir = {d};
    else return NULL;
    }
  else {
    // Angry Beasts attack all neighbors first
    if(m == moRagingBull) beastAttack(c, true, false);
    d = pickMoveDirection(c, mf);
    }
  if(d == -1) {
    stayEffect(c);
    return c;
    }
  
  if(!quantum) {
    movei mi(c, d);
    auto& c2 = mi.t;
    if(isPlayerOn(c2)) {
      if(m == moCrusher) {
        addMessage(XLAT("%The1 raises his weapon...", m));
        crush_next.push_back(c2);
        c->stuntime = 7;
        return c2;
        }      
      killThePlayerAt(m, c2, 0);
      return c2;
      }
  
    eMonster m2 = c2->monst;
    
    if(m2 && m == moCrusher) {
      addMessage(XLAT("%The1 raises his weapon...", m));
      crush_next.push_back(c2);
      c->stuntime = 7;
      return c2;
      }
    else if(m2) {
      attackMonster(c2, AF_NORMAL | AF_MSG, m);
      animateAttack(mi, LAYER_SMALL);
      if(m == moFlailer && m2 == moIllusion) 
        attackMonster(c, 0, m2);
      return c2;
      }
    
    moveMonster(mi);
    if(m == moRagingBull) beastAttack(c2, false, false);
    return c2;
    }
  else {
    bool attacking = false;
    for(int dir: global_posdir) {
      cell *c2 = c->move(dir);

      if(isPlayerOn(c2)) {
        killThePlayerAt(m, c2, 0); 
        attacking = true;
        }

      else {
        eMonster m2 = c2->monst;
        if(m2) {
          attackMonster(c2, AF_NORMAL | AF_MSG, m);
          if(m == moFlailer && m2 == moIllusion) 
            attackMonster(c, 0, m2);
          attacking = true;
          }
        }
      }
    
    if(!attacking) for(int dir: global_posdir) {
      movei mi(c, dir);
      if(!c->monst) c->monst = m;
      moveMonster(mi);
      if(m == moRagingBull) beastAttack(mi.t, false, false);
      }
    return c->move(d);
    }
  }

EX void mountmove(const movei& mi, bool fp) {
  for(int i=0; i<numplayers(); i++) {
    if(playerpos(i) == mi.s) {
      animateMovement(mi, LAYER_SMALL);
      if(multi::players > 1) {
        multi::player[i].at = mi.t;
        multi::player[i].spin = mi.rev_dir_force();
        multi::flipped[i] = fp;
        }
      else {
        cwt.at = mi.t;
        cwt.spin = mi.rev_dir_force();
        flipplayer = fp;
        }
      afterplayermoved();
      }
    if(lastmountpos[i] == mi.s && mi.s) {
      lastmountpos[i] = mi.t;
      }
    else if(lastmountpos[i] == mi.t)  {
      lastmountpos[i] = NULL;
      }
    }
  }

EX void moveWorm(cell *c) {

  bool mounted = isMounted(c);
  
  if(c->monst == moWormwait) { c->monst = moWorm; return; }
  else if(c->monst == moTentaclewait) { c->monst = moTentacle; return; }
  else if(c->monst == moTentacleEscaping) {    
    // explodeAround(c);
    forCellEx(c2, c)
      if(canAttack(c, c->monst, c2, c2->monst, mounted ? AF_ONLY_ENEMY : (AF_GETPLAYER | AF_ONLY_FBUG))) {
        attackMonster(c2, AF_NORMAL | AF_MSG | AF_GETPLAYER, c->monst);
        }
    cell *c2 = c;
    vector<cell*> allcells;
    while(c2->mondir != NODIR) {
      allcells.push_back(c2);
      c2 = c2->move(c2->mondir);
      if(!c2) { allcells.pop_back(); break; }
      }
    allcells.push_back(c2);
    for(int i=isize(allcells)-2; i>=0; i--) {
      cell *cmt = allcells[i+1];
      cell *cft = allcells[i];
      auto mi = moveimon(cft);
      if(cft->monst != moTentacleGhost && cmt->monst != moTentacleGhost)
        mountmove(mi, false);
      animateMovement(mi, LAYER_BIG);
      }
    c->monst = moNone;
    if(c->mondir != NODIR) c->move(c->mondir)->monst = moTentacleEscaping;
    return;
    }
  else if(c->monst != moWorm && c->monst != moTentacle) return;

  eMonster m = c->monst;
  int id = m - moWorm;
  
  int mf = MF_PATHDIST | AF_EAT;
  
  if(mounted) mf ^= (MF_MOUNT | MF_PATHDIST);
  
  // without this, in 3D geometries, Sandworms explode because no land around them is generated yet
  forCellCM(c2, c) setdist(c2, 8, c);
  
  int dir = pickMoveDirection(c, mf);
  
  if(c->wall == waRose) {
    addMessage(XLAT("%The1 eats %the2!", c->monst, c->wall));
    c->wall = waNone;
    dir = -1;
    }
  
  if(dir == -1) {
    int spices = 0;
    if(id) {
      addMessage(XLAT("Cthulhu withdraws his tentacle!"));
      kills[moTentacle]++;
      c->monst = moTentacleEscaping;
      moveWorm(c);
      }
    else {
      kills[moWorm]++;
      spices = 3;
      }
    eItem loc = treasureType(c->land);
    bool spiceSeen = false;
    while(c && (c->monst == moWorm || c->monst == moWormtail || c->monst == moTentacle || c->monst == moTentacletail)) {
      // if(!id) 
      explodeAround(c);
      drawParticles(c, minf[c->monst].color, 16);
      if(spices > 0 && c->land == laDesert) {
        if(notDippingForExtra(itSpice, loc)) {
          c->item = itSpice;
          if(c->cpdist <= 6) spiceSeen = true;
          }
        spices--;
        }
      c->monst = moNone;
      if(c->mondir != NODIR) c = c->move(c->mondir);
      }
    if(!id) {
      if(spiceSeen)
        addMessage(XLAT("The sandworm explodes in a cloud of Spice!"));
      else
        addMessage(XLAT("The sandworm explodes!"));
      playSound(NULL, "explosion");
      if(geometry == gZebraQuotient)
        achievement_gain_once("ZEBRAWORM", rg::special_geometry);
      }
    return;
    }
  
  movei mi(c, dir);
  auto& goal = mi.t;

  if(isPlayerOn(goal) || goal->monst) 
    attackMonster(goal, AF_EAT | AF_MSG | AF_GETPLAYER, c->monst);
  
  if(1) {
    goal->monst = eMonster(moWormwait + id);
    moveEffect(mi, eMonster(moWormwait + id));
      
    animateMovement(mi, LAYER_BIG);
    c->monst = eMonster(moWormtail + id);
    goal->mondir = mi.rev_dir_or(NODIR);
    goal->monmirror = c->monmirror ^ c->c.mirror(dir);
    setdist(goal, 6, nullptr);
  
    mountmove(mi, true);
    
    if(id) {
      cell *c2 = c, *c3 = c2;
      while(c2->monst == moTentacletail || c2->monst == moTentacleGhost) {
        auto mim = moveimon(c2).rev();
        if(!mim.proper()) return;
        c3 = c2, c2 = mim.s;
        if(c3->monst != moTentacleGhost && c2->monst != moTentacleGhost) 
          mountmove(mim, true);
        animateMovement(mim, LAYER_BIG);
        }
      }
    
    cell *c2 = c, *c3 = c2;
    for(int a=0; a<WORMLENGTH; a++) {
      if(c2->monst == moWormtail) {
        movei mim = moveimon(c2).rev();
        if(!mim.proper()) {
          drawParticles(c2, (linf[c2->land].color & 0xF0F0F0), 16);
          return;
          }
        c3 = c2, c2 = mim.s;
        mountmove(mim, true);
        animateMovement(mim, LAYER_BIG);
        }
      }
    
    if(c2->monst == moWormtail) c2->monst = moNone, c3->mondir = NODIR;
    }

  }

EX void ivynext(cell *c) {
  cellwalker cw(c, c->mondir, c->monmirror);
  
  // check the mirroring status
  cell *c2 = c;
  while(true) {
    if(c2->monst == moIvyRoot) break;
    if(!proper(c2, c2->mondir))
      return; /* incorrect data */
    if(!isIvy(c2->monst)) break;
    if(c2->c.mirror(c2->mondir)) cw.mirrored = !cw.mirrored;
    c2 = c2->move(c2->mondir);
    }
  
  cw.at->monst = moIvyWait;
  bool findleaf = false;
  while(true) {
    cw += 1;
    if(cw.spin == signed(cw.at->mondir)) {
      if(findleaf) { 
        cw.at->monst = moIvyHead; break;
        }
      cw.at->monst = moIvyWait;
      cw += wstep;
      continue;
      }
    cw += wstep;
    if(cw.at->monst == moIvyWait && signed(cw.at->mondir) == cw.spin) {
      cw.at->monst = moIvyBranch;
      findleaf = true; continue;
      }
    cw += wstep;
    }
  }

// this removes Ivy, but also potentially causes Vines to grow
EX void removeIvy(cell *c) {
  eMonster m = c->monst;
  c->monst = moNone; // NEWYEARFIX
  for(int i=0; i<c->type; i++)
  // note that semi-vines don't count
    if(c->move(i) && c->move(i)->wall == waVinePlant) {
      destroyHalfvine(c);
      if (!do_not_touch_this_wall(c))
        c->wall = waVinePlant;
      }
  if(c->wall != waVinePlant) {
    if(m == moIvyDead)
      m = moIvyWait;
    drawParticles(c, minf[m].color, 2);
    }
  }

EX void moveivy() {
  if(isize(ivies) == 0) return;
  if(racing::on) return;
  pathdata pd(moIvyRoot);
  for(int i=0; i<isize(ivies); i++) {
    cell *c = ivies[i];
    cell *co = c;
    if(c->monst != moIvyHead) continue;
    ivynext(c);

    int pd = c->pathdist;
    
    movei mi(nullptr, nullptr, NODIR);
      
    while(c->monst != moIvyRoot) {
      if(!isIvy(c->monst)) {
        raiseBuggyGeneration(c, "that's not an Ivy!");
        break;
        }
      if(c->mondir == NODIR) {
        raiseBuggyGeneration(c, "wrong mondir!");
        break;
        }
        
      forCellIdEx(c2, j, c) {
        if(canAttack(c, c->monst, c2, c2->monst, AF_ONLY_FRIEND | AF_GETPLAYER)) {
          if(isPlayerOn(c2)) 
            killThePlayerAt(c->monst, c2, 0);
          else {
            if(attackJustStuns(c2, 0, c->monst))
              addMessage(XLAT("The ivy attacks %the1!", c2->monst));
            else if(isNonliving(c2->monst))
              addMessage(XLAT("The ivy destroys %the1!", c2->monst));
            else
              addMessage(XLAT("The ivy kills %the1!", c2->monst));
            attackMonster(c2, AF_NORMAL, c->monst);
            }
          continue;
          }
        if(c2 && c2->pathdist < pd && passable(c2, c, 0) && !strictlyAgainstGravity(c2, c, false, MF_IVY))
          mi = movei(c, j), pd = c2->pathdist;
        }
      c = c->move(c->mondir);
      }
    
    auto& mto = mi.t;

    if(mto && mto->cpdist) {
      animateMovement(mi, LAYER_BIG);
      mto->monst = moIvyWait, mto->mondir = mi.rev_dir_or(NODIR);
      mto->monmirror = mi.s->monmirror ^ mi.mirror();
      moveEffect(mi, moIvyWait);
      // if this is the only branch, we want to move the head immediately to mto instead
      if(mi.s->monst == moIvyHead) {
        mto->monst = moIvyHead; co->monst = moIvyBranch;
        }
      }
    else if(!proper(co, co->mondir) || !co->move(co->mondir)) ; /* should not happen */
    else if(co->move(co->mondir)->monst != moIvyRoot) {
      // shrink useless branches, but do not remove them completely (at the root)
      if(co->monst == moIvyHead) co->move(co->mondir)->monst = moIvyHead;
      removeIvy(co);
      }
    }
  }

vector<cell*> gendfs;

int targetcount;

EX bool isTargetOrAdjacent(cell *c) {
  for(int i=0; i<targetcount; i++)
    if(gendfs[i] == c || isNeighbor(gendfs[i], c)) 
      return true;
  return false;
  }

EX void groupmove2(const movei& mi, eMonster movtype, flagtype mf) {
  auto& c = mi.s;
  auto& from = mi.t; // note: we are moving from 'c' to 'from'!'
  if(!c) return;

  if(c->pathdist == 0) return;

  if(movtype == moKrakenH && isTargetOrAdjacent(from)) ;
/*  else if(passable_for(movtype, from, c, P_ONPLAYER | P_CHAIN | P_MONSTER)) ;
  else if(canAttack(c, movtype, from, from->monst, AF_GETPLAYER)) ; */
  else if(from->wall == waThumperOn) ;
  else if(passable_for(movtype, from, c, P_CHAIN | P_MONSTER)) ;
  else if(canAttack(c, movtype, from, from->monst, AF_GETPLAYER | AF_NOSHIELD)) ;
  else if(isMagneticPole(movtype)) {
    // a special case here -- we have to ignore the illegality of 
    // the 'second' move due to an adjacent opposite pole
    forCellIdEx(c2, d, c)
      if(c2->monst == movtype) {
        cell *c3 = c2->move(c2->mondir);
        eMonster m2 = c3->monst;
        c3->monst = moNone;
        bool ok = 
           passable_for(movtype, from, c, P_CHAIN | P_MONSTER) 
        && passable_for(movtype, c, c2, P_CHAIN | P_MONSTER);
        c3->monst = m2;
        if(ok) groupmove2(movei(c, d).rev(), movtype, mf);
        }
    }
  else return;
  
  if(from->monst) {
    if(mf & MF_MOUNT) {
      // don't go through friends
      if(isFriendlyOrBug(from)) return;
      }
    else {
      // go through the player (even mounted)
      if(isPlayerOn(from)) ;
      // go through the mounted dragon
      else if(isDragon(from->monst) && isFriendlyOrBug(from)) ;
      // but not through other worms
      else if(isWorm(from)) return;
      // go through other friends
      else if(isFriendlyOrBug(from)) ;
      else return;
      }
    }
  
  // Kraken movement
  if(movtype == moKrakenH && c->monst == moKrakenT && c->stuntime == 0) 
    kraken::trymove(c);
  
  if(movegroup(c->monst) == movtype) {

    int af = AF_ONLY_FBUG | AF_GETPLAYER;
    if(mf & MF_MOUNT) af = 0;

    if(!passable_for(movtype, from, c, P_ONPLAYER | P_MONSTER)) return;
    if(!ignoresSmell(c->monst) && againstRose(c, from)) return;
    if((mf & MF_ONLYEAGLE) && c->monst != moEagle && c->monst != moBat) 
      return;
    if((mf & MF_ONLYEAGLE) && bird_disruption(c) && markOrb(itOrbGravity)) return;
    // in the gravity lands, eagles cannot ascend in their second move
    if((mf & MF_ONLYEAGLE) && gravityLevelDiff(c, from) < 0) {
      onpath_mark(c);
      return;
      }
    if((mf & MF_NOFRIEND) && isFriendly(c)) return;
    if((mf & MF_MOUNT) && !isMounted(c)) return;
    if(isRatling(c->monst) && lastmovetype == lmSkip) return;

    if(c->stuntime) return;
    if(c->monst == moBat && batsAfraid(from)) return;
    
    // note: move from 'c' to 'from'!
    if(!(mf & MF_NOATTACKS)) for(int j=0; j<c->type; j++) 
      if(c->move(j) && canAttack(c, c->monst, c->move(j), c->move(j)->monst, af)) {
        attackMonster(c->move(j), AF_NORMAL | AF_GETPLAYER | AF_MSG, c->monst);
        animateAttack(movei(c, j), LAYER_SMALL);
        onpath_mark(c);
        // XLATC eagle
        return;
        }
    
    if(from->cpdist == 0 || from->monst) { onpath_mark(c); return; }
    
    if(movtype == moDragonHead) {
      dragon::move(mi);
      return;
      }
    
    moveMonster(mi);
    
    onpath_mark(from);

    if(isDie(mi.t->monst)) {
      /* other dice will not pathfind through the original cell */
      /* this makes it easier for the player to roll dice correctly */
      onpath_mark(c);
      return;
      }
    }
  onpath_mark(c);
  // MAXGCELL
  if(isize(gendfs) < 1000 || c->cpdist <= 6) gendfs.push_back(c);
  }

EX void groupmove(eMonster movtype, flagtype mf) {
  pathdata pd(0);
  gendfs.clear();
  
  if(mf & MF_MOUNT) {
    if(dragon::target) gendfs.push_back(dragon::target);
    if(movtype == moDragonHead) {
      for(int i=0; i<isize(dcal); i++) {
        cell *c = (i == 0 && dragon::target) ? dragon::target : dcal[i];
        if(!c->monst) continue;
        if(isFriendlyOrBug(c)) continue;
        forCellIdEx(c2, d, c) if(c2->monst && isMounted(c2)) {
          groupmove2(movei(c,d).rev(),movtype,mf);
          }
        } 
      }
    }
  else {
    if(!peace::on) for(int i=0; i<isize(targets); i++) gendfs.push_back(targets[i]);
  
    if(invisfish && (movtype == moSlime || movtype == moShark || movtype == moKrakenH))
      for(cell *pc: player_positions()) 
        gendfs.push_back(pc);
    }
  
  targetcount = isize(gendfs);
  
  for(int i=0; i<isize(gendfs); i++) {
    cell *c = gendfs[i];
    vector<int> dirtable;
    
    forCellIdAll(c2,t,c) dirtable.push_back(t);
    hrandom_shuffle(dirtable);

    for(auto& t: dirtable) {
      groupmove2(movei(c, t).rev(),movtype,mf);
      }
      
    if(movtype == moEagle && c->monst == moNone && !isPlayerOn(c) && !bird_disruption(c)) {
      cell *c2 = whirlwind::jumpFromWhereTo(c, false);
      groupmove2(movei(c2, c, STRONGWIND), movtype, mf);
      }
    
    if(frog_power(movtype) && c->monst == moNone && !isPlayerOn(c)) {
      forCellEx(c2, c) forCellEx(c3, c2)
        groupmove2(movei(c3, c, JUMP), movtype, mf);
      }
    }

  if(movtype != moDragonHead) for(int i=0; i<isize(dcal); i++) {
    cell *c = dcal[i];
    if((mf & MF_ONLYEAGLE) && c->monst != moEagle && c->monst != moBat) return;
    if(movegroup(c->monst) == movtype && c->pathdist != 0) {
      cell *c2 = moveNormal(c, mf);
      if(c2) onpath_mark(c2);
      }
    }
  }

// Hex monsters

vector<cell*> hexdfs;

EX void moveHexSnake(const movei& mi, bool mounted) {
  // note: move from 'c' to 'from'!
  auto& from = mi.t;
  auto& c = mi.s;
  setdist(from, 6, nullptr);
  if(from->wall == waBoat) from->wall = waSea;
  moveEffect(mi, c->monst);
  from->monst = c->monst; from->mondir = mi.rev_dir_or(NODIR); from->hitpoints = c->hitpoints;
  c->monst = moHexSnakeTail;
  preventbarriers(from);
  
  animateMovement(mi, LAYER_BIG);
  mountmove(mi, true);
  
  cell *c2 = c, *c3=c2;
  for(int a=0;; a++) if(c2->monst == moHexSnakeTail) {
    if(a == ROCKSNAKELENGTH) { c2->monst = moNone, c3->mondir = NODIR; break; }
    auto mim = moveimon(c2).rev();
    if(!mim.proper()) break;
    mountmove(mim, true);
    animateMovement(mim, LAYER_BIG);
    c3 = c2, c2 = mim.s;
    }
    else break;
  }

EX void snakeAttack(cell *c, bool mounted) {
  for(int j=0; j<c->type; j++) 
    if(c->move(j) && canAttack(c, moHexSnake, c->move(j), c->move(j)->monst, 
      mounted ? AF_ONLY_ENEMY : (AF_ONLY_FBUG | AF_GETPLAYER))) {
        eMonster m2 = c->move(j)->monst;
        attackMonster(c->move(j), AF_NORMAL | AF_GETPLAYER | AF_MSG, moHexSnake);
        spread_plague(c, c->move(j), j, moHexSnake);
        produceGhost(c->move(j), moHexSnake, m2);
        }
  }

EX bool goodmount(cell *c, bool mounted) {
  if(mounted) return isMounted(c);
  else return !isMounted(c);
  }

EX int inpair(cell *c, int colorpair) {
  return (colorpair >> pattern_threecolor(c)) & 1;
  }

EX int snake_pair(cell *c) {
  if(c->mondir == NODIR)
    return (1 << pattern_threecolor(c));
  else
    return (1 << pattern_threecolor(c)) | (1 << pattern_threecolor(c->move(c->mondir)));
  }

// note: move from 'c' to 'from'!
EX void hexvisit(cell *c, cell *from, int d, bool mounted, int colorpair) {
  if(!c) return;
  if(cellUnstable(c) || cellEdgeUnstable(c)) return;
  if(c->pathdist == 0) return;
  
  if(cellUnstableOrChasm(c) || cellUnstableOrChasm(from)) return;
  
  /* if(c->monst == moHexSnake)
    printf("%p:%p %s %d\n", from, c, dnameof(from->monst), passable(from, c, true, false, false)); */

  if(from->cpdist && (!passable(from, c, P_MONSTER|P_WIND|P_FISH))) return;
  
  if(c->monst == moHexSnake && snake_pair(c) == colorpair) {
    // printf("got snake\n");
    
    if(!inpair(from, colorpair)) return;
    if(!goodmount(c, mounted)) return;
    
    if(canAttack(c, moHexSnake, from, from->monst, AF_EAT | (mounted ? AF_ONLY_ENEMY : AF_ONLY_FBUG | AF_GETPLAYER))) {
      attackMonster(from, AF_MSG | AF_EAT | AF_GETPLAYER, c->monst);
      }
    
    if(from->cpdist == 0 || from->monst) return;

    snakeAttack(c, mounted);
    moveHexSnake(movei(from, d).rev(), mounted);
    }

  onpath_mark(c);

  // MAXGCELL
  if(isize(hexdfs) < 2000 || c->cpdist <= 6) 
    hexdfs.push_back(c);
  }

EX void movehex(bool mounted, int colorpair) {
  pathdata pd(3);
  hexdfs.clear();
  
  if(mounted) { 
    if(dragon::target && dragon::target->monst != moHexSnake) {
      hexdfs.push_back(dragon::target); 
      onpath_mark(dragon::target);
      }
    }
  else for(cell *c: targets) {
    hexdfs.push_back(c);
    onpath_mark(c);
    }
  //hexdfs.push_back(cwt.at);
  
  for(int i=0; i<isize(hexdfs); i++) {
    cell *c = hexdfs[i];
    vector<int> dirtable;
    for(int t=0; t<c->type; t++) if(c->move(t) && inpair(c->move(t), colorpair))
      dirtable.push_back(t);
      
    hrandom_shuffle(dirtable);
    for(auto& t: dirtable) {
      hexvisit(c->move(t), c, t, mounted, colorpair);
      }
    }
  }

EX void movehex_rest(bool mounted) {
  pathdata pd(4);
  for(int i=0; i<isize(hexsnakes); i++) {
    cell *c = hexsnakes[i];
    int colorpair;
    if(c->monst == moHexSnake) {
      colorpair = snake_pair(c);
      if(!goodmount(c, mounted)) continue;
      vector<int> dirtable = hrandom_permutation(c->type);
      for(int u=0; u<c->type; u++) {
        createMov(c, dirtable[u]);
        if(inpair(c->move(dirtable[u]), colorpair))
          hexvisit(c, c->move(dirtable[u]), c->c.spin(dirtable[u]), mounted, colorpair);
        }
      }
    if(c->monst == moHexSnake) {
      snakeAttack(c, mounted);
      kills[moHexSnake]++;
      playSound(c, "die-troll");
      cell *c2 = c;
      while(c2->monst == moHexSnakeTail || c2->monst == moHexSnake) {
        if(c2->monst != moHexSnake && c2->mondir != NODIR)
          snakepile(c2, moHexSnake);
        snakepile(c2, moHexSnake);
        c2->monst = moNone; 
        if(c2->mondir == NODIR) break;
        c2 = c2->move(c2->mondir);
        }
      }
    }
  }

EX void movemutant() {
  manual_celllister mcells;
  for(cell *c: currentmap->allcells()) mcells.add(c);
  if(!closed_or_bounded)
    for(int i=0; i<isize(mcells.lst); i++) {
      cell *c = mcells.lst[i];
      if(c->land == laClearing && c->monst != moMutant && !pseudohept(c))
        forCellEx(c2, c) forCellEx(c3, c2) if(celldistAlt(c3) < celldistAlt(c))
          mcells.add(c3);
      }
  
  vector<cell*> young;
  for(cell *c: mcells.lst)
    if(c->monst == moMutant && c->stuntime == mutantphase)
      young.push_back(c);
  
  for(int j=1; j<isize(young); j++)
    swap(young[j], young[hrand(j+1)]);

  mutantphase++;
  mutantphase &= 15;
  
  for(int i=0; i<isize(young); i++) {
    cell *c = young[i];
    for(int j=0; j<c->type; j++) {
      movei mi(c, j);
      auto& c2 = mi.t;
      if(!c2) continue;

      if(c2->monst != moMutant && canAttack(c, moMutant, c2, c2->monst, AF_ONLY_FBUG | AF_GETPLAYER)) {
        attackMonster(c2, AF_NORMAL | AF_MSG | AF_GETPLAYER, moMutant);
        continue;
        }
      
      if(isPlayerOn(c2)) continue;

      if((c2->land == laOvergrown || !pseudohept(c2)) && passable(c2, c, 0)) {
        if(c2->land == laClearing && !closed_or_bounded && c2->mpdist > 7) continue;
        c2->monst = moMutant;
        c2->mondir = c->c.spin(j);
        c2->stuntime = mutantphase;
        animateMovement(mi, LAYER_BIG);
        }
      }
    }  
  }

#if HDR
#define SHSIZE 16
#endif

EX vector<array<cell*, MAXPLAYER>> shpos;
EX int cshpos = 0;

EX cell *lastmountpos[MAXPLAYER];

EX void clearshadow() {
  shpos.resize(SHSIZE);
  for(int i=0; i<SHSIZE; i++) for(int p=0; p<MAXPLAYER; p++)
    shpos[i][p] = NULL;
  }

/** \brief kill the shadow by clearing its history -- c is provided for multiplayer */
EX void kill_shadow_at(cell *c) {
  for(int p=0; p<MAXPLAYER; p++)
    if(shpos[cshpos][p] == c)
      for(int i=0; i<SHSIZE; i++)
        changes.value_set(shpos[i][p], (cell*) nullptr);
  }

EX void moveshadow() {

  cell *shfrom = NULL;

  shpos.resize(SHSIZE);
  for(int p: player_indices()) {
    cell *c = shpos[cshpos][p];
    if(c && c->monst == moShadow) {
      for(int j=0; j<c->type; j++) 
        if(c->move(j) && canAttack(c, moShadow, c->move(j), c->move(j)->monst, AF_ONLY_FBUG | AF_GETPLAYER))
          attackMonster(c->move(j), AF_NORMAL | AF_MSG | AF_GETPLAYER, c->monst);
      c->monst = moNone;
      shfrom = c;
      }
    shpos[cshpos][p] = playerpos(p);
    }
  cshpos = (cshpos+1) % SHSIZE;
  for(int p: player_indices()) {
    cell* where = shpos[cshpos][p];
    if(where && where->monst == moNone && where->cpdist && among(where->land, laGraveyard, laCursed)) {
      if(sword::at(where)) {
        kill_shadow_at(where);
        fightmessage(moShadow, moPlayer, false, AF_SWORD_INTO);
        continue;
        }
      if(shfrom) animateMovement(match(shfrom, where), LAYER_SMALL);
      where->monst = moShadow;
      where->hitpoints = p;
      where->stuntime = 0;
      // the Shadow sets off the mines and stuff
      moveEffect(movei(where, where, NODIR), moShadow);
      // Beauty kills the Shadow
      check_beauty(where, where, moShadow);
      }
    }
  }

EX void moveghosts() {

  if(invismove) return;
  movesofgood.clear();

  for(int i=0; i<isize(ghosts); i++) {
    cell *c = ghosts[i];
    
    if(c->stuntime) continue;    
    if(isPowerMonster(c) && !playerInPower()) continue;

    if(isGhostMover(c->monst)) {
      int goodmoves = 0;

      for(int k=0; k<c->type; k++) if(c->move(k) && c->move(k)->cpdist < c->cpdist)
        if(ghostmove(c->monst, c->move(k), c, 0) && !isPlayerOn(c->move(k)))
          goodmoves++;
      
      movesofgood.grow(goodmoves).push_back(c);
      }
    }
  
  for(auto& v: movesofgood) for(cell *c: v) {

    if(c->stuntime) continue;    
    if(isPowerMonster(c) && !playerInPower()) continue;
    
    if(isGhostMover(c->monst) && c->cpdist >= 1) {
      
      vector<int> mdir;

      for(int p: {0, 1}) for(int j=0; j<c->type; j++) if(p == 1 || (c->move(j) && isPlayerOn(c->move(j))))
        if(c->move(j) && canAttack(c, c->monst, c->move(j), c->move(j)->monst, AF_GETPLAYER | AF_ONLY_FBUG)) {
          // XLATC ghost/greater shark
          
          attackMonster(c->move(j), AF_NORMAL | AF_MSG | AF_GETPLAYER, c->monst);
          goto nextghost;
          }
    
      for(int k=0; k<c->type; k++) if(c->move(k) && c->move(k)->cpdist < c->cpdist)
        if(ghostmove(c->monst, c->move(k), c, 0))
          mdir.push_back(k);
      if(mdir.empty()) continue;
      int d = hrand_elt(mdir);
      cell *c2 = c->move(d);
      if(c2->monst == moTortoise && c2->stuntime > 1) {
        addMessage(XLAT("%The1 scares %the2 a bit!", c->monst, c2->monst));
        c2->stuntime = 1;
        }
      else moveMonster(movei(c, d));
      
      }
    nextghost: ;
    }
  }

/** for an ally m at c, evaluate staying in place */
EX int stayvalue(eMonster m, cell *c) {
  if(!passable_for(c->monst, c, NULL, P_MONSTER | P_MIRROR))
    return -1501;
  if(cellEdgeUnstable(c))
    return -1501;
  if(againstRose(c, NULL) && !ignoresSmell(c->monst)) return -1500;
  return 100;
  }

/** for an ally m at c, evaluate moving to c2 */
EX int movevalue(eMonster m, cell *c, int dir, flagtype flags) {

  auto mi = movei(c, dir);
  auto& c2 = mi.t;
  int val = 0;
  
  if(isPlayerOn(c2)) val = -3000;
  else if(againstRose(c, c2)  && !ignoresSmell(m)) return -1200;
  else if(m == moPrincess && c2->stuntime && hasPrincessWeapon(c2->monst) && 
    !cellDangerous(c) && !cellDangerous(c2) && canAttack(c, m, c2, c2->monst, AF_IGNORE_UNARMED | flags)) {
    val = 15000;
    }
  else if(canAttack(c,m,c2,c2->monst,flags)) 
    val = 
    (!passable_for(c->monst, c, NULL, P_MONSTER | P_MIRROR)) ? 100 :
    (m == moPrincessArmed && isPrincess(c2->monst)) ? 14000 : // jealousy!
    isActiveEnemy(c2,m) ? 12000 :
    among(c2->monst, moSkeleton, moMetalBeast, moReptile, moTortoise, moSalamander, moTerraWarrior, moBrownBug) ? -400 :
    isIvy(c2) ? 8000 :
    isInactiveEnemy(c2,m) ? 1000 :
    -500;

  else if(passable_for(m, c2, c, 0)) {
#if CAP_COMPLEX2
    if(mine::marked_mine(c2) && !ignoresPlates(m))
      val = 50;
    else
#endif
      val = 4000;

    int tk = tkills();
    changes.init(true);
    moveMonster(mi);
    int tk2 = tkills();
    bool b = monstersnear(mi.t, m);
    changes.rollback();
    if(b) val = 50;
    else if(tk2 > tk) val += 1000 + 200 * (tk2 - tk);
    }
  else if(passable_for(m, c2, c, P_DEADLY)) return -1100;
  else return -1750;

  if(c->monst == moGolem ) {
    val -= c2->pathdist;
    }
  else if(c->monst == moFriendlyGhost )
    val += c2->cpdist - 40;
  else if(c->monst == moMouse) {
    int d;
    if(!euclid && (c2->land != laPalace || !c2->master->alt)) d = 200;
    else d = celldistAlt(c2);
    // first rule: suicide if the Princess is killed,
    // by monstersnear or jumping into a chasm
    princess::info *i = princess::getPrisonInfo(c);
    if(i && !i->princess) {
      if(val == 50 || c2->wall == waChasm) val = 20000;
      }
    // second rule: move randomly if the Princess is saved
    if(i && i->bestdist > 6)
      ;
    // third rule: do not get too far from the Princess
    else if(d > 150)
      val -= (700+d);
    // fourth rule: do not get too far from the Rogue
    // NOTE: since Mouse is not a target, we can use
    // the full pathfinding here instead of cpdist!
    else if(c2->pathdist > 3 && c2->pathdist <= 19)
      val -= (500+c2->pathdist * 10);
    else if(c2->pathdist > 19)
      val -= (700);
    // fifth rule: get close to the Princess, to point the way
    else
      val -= (250+d);
    /*
    // avoid stepping on trapdoors and plates
    // (REMOVED BECAUSE MICE NO LONGER ACTIVATE TRAPDOORS AND PLATES)
    // note that the Mouse will still step on the trapdoor
    // if it wants to get close to you and there is no other way
    if(c2->wall == waTrapdoor)
      val -= 5;
    */
    }
  if(isPrincess(c->monst)) {
    
    int d = c2->pathdist;
    if(d <= 3) val -= d;
    else val -= 10 * d;
    
    // the Princess also avoids stepping on pressure plates
    if(c2->wall == waClosePlate || c2->wall == waOpenPlate || c2->wall == waTrapdoor)
      val -= 5;
    }
  if(c->monst == moTameBomberbird) {
    int d = c2->pathdist;
    if(d == 1 && c->pathdist > 1) d = 5;
    if(d == 2 && c->pathdist > 2) d = 4;
    val -= d;
    }
  if(c->monst == moKnight && (eubinary || c2->master->alt)) {
    val -= celldistAlt(c2);
    // don't go to external towers
    if(c2->wall == waTower && c2->wparam == 1 && !c2->monst)
      return 60;
    }
  return val;
  }

EX void movegolems(flagtype flags) {
  if(items[itOrbEmpathy] && items[itOrbSlaying])
    flags |= AF_CRUSH;
  int qg = 0;
  for(int i=0; i<isize(golems); i++) {
    cell *c = golems[i];
    eMonster m = c->monst;
    pathdata pd(m, false);
    if(c->stuntime) continue;
    if(m == moGolem || m == moKnight || m == moTameBomberbird || m == moPrincess ||
      m == moPrincessArmed || m == moMouse || m == moFriendlyGhost) {
      if(m == moGolem) qg++;
      if(m == moFriendlyGhost) markOrb(itOrbUndeath);

      auto recorduse = orbused;

      DEBB(DF_TURN, ("stayval"));
      int bestv = stayvalue(m, c);
      vector<int> bdirs;

      DEBB(DF_TURN, ("moveval"));
      for(int k=0; k<c->type; k++) if(c->move(k)) {
        int val = movevalue(m, c, k, flags);

        if(val > bestv) bestv = val, bdirs.clear();
        if(val == bestv) bdirs.push_back(k);
        }
      
      if(m == moTameBomberbird) {
        cell *c2 = whirlwind::jumpDestination(c);
        if(c2 && !c2->monst) {
          int val = movevalue(m, c, STRONGWIND, flags);
          // printf("val = %d bestv = %d\n", 
          if(val > bestv) bestv = val, bdirs.clear();
          if(val == bestv) bdirs.push_back(STRONGWIND);
          }
        }

      orbused = recorduse;
      
//    printf("stayvalue = %d, result = %d, bq = %d\n", stayvalue(m,c), bestv, bq);
        
      if(bdirs.empty()) continue;
      int dir = hrand_elt(bdirs);
      auto mi = movei(c, dir);
      auto& c2 = mi.t;
      if(c2->monst) {
        bool revenge = (m == moPrincess);
        bool jealous = (isPrincess(c->monst) && isPrincess(c2->monst));
        eMonster m2 = c2->monst;
        if(revenge) {
          playSound(c2, princessgender() ? "dzia-princess" : "dzia-prince");
          addMessage(XLAT("%The1 takes %his1 revenge on %the2!", m, c2->monst));
          }
        if(revenge || jealous) flags |= AF_CRUSH;
        else if((flags & AF_CRUSH) && !canAttack(c, m, c2, c2->monst, flags ^ AF_CRUSH ^ AF_MUSTKILL))
          markOrb(itOrbEmpathy), markOrb(itOrbSlaying);
        attackMonster(c2, flags | AF_MSG, m);
        animateAttack(movei(c, dir), LAYER_SMALL);
        spread_plague(c, c2, dir, m);
        produceGhost(c2, m2, m);
        sideAttack(c, dir, m, 0);
        if(revenge) c->monst = m = moPrincessArmed;
        if(jealous) {
          playSound(c2, princessgender() ? "dzia-princess" : "dzia-prince");
          addMessage("\"That should teach you to take me seriously!\"");
          }
        }
      else {
        passable_for(m, c2, c, P_DEADLY);
        DEBB(DF_TURN, ("move"));
        moveMonster(mi);
        if(m != moTameBomberbird && m != moFriendlyGhost) 
          moveBoatIfUsingOne(mi);
          
        if(c2->monst == m) {          
          if(m == moGolem) c2->monst = moGolemMoved;
          if(m == moMouse) c2->monst = moMouseMoved;
          if(m == moPrincess) c2->monst = moPrincessMoved;
          if(m == moPrincessArmed) c2->monst = moPrincessArmedMoved;
          if(m == moTameBomberbird) c2->monst = moTameBomberbirdMoved;
          if(m == moKnight) c2->monst = moKnightMoved;
          if(m == moFriendlyGhost) c2->stuntime = 1;
          }

        empathyMove(mi);
        }
      DEBB(DF_TURN, ("other"));
      }
    }
  achievement_count("GOLEM", qg, 0);
  }

/** note: butterflies don't use moveNormal for two reasons:
 *  1) to make sure that they move AFTER bulls
 *  2) to make sure that they move offscreen
 */
EX void moveButterflies() {
  int j = 0;
  for(int i=0; i<isize(butterflies); i++) {
    cell* c = butterflies[i].first;
    if(c->monst == moButterfly) {
      /* // don't move if under attack of a bull
      bool underattack = false;
      forCellEx(c3, c) 
        if(c3->monst == moRagingBull && c3->mondir != NODIR &&
          angledist(c3->type, c3->mondir, neighborId(c3, c)) == 3 &&
          canAttack(c3, moRagingBull, c, c->monst, AF_BULL)
          )
            underattack = true;
      if(underattack) continue; */
      cell *c2 = moveNormal(c, 0);
      if(butterflies[i].second < 50 && c2)
        butterflies[j++] = make_pair(c2, butterflies[i].second+1);
      }
    }
  butterflies.resize(j);
  }

// assume pathdist
EX void specialMoves() {
  for(int i=0; i<isize(dcal); i++) {
    cell *c = dcal[i];
    
    if(c->stuntime) continue;
    
    eMonster m = c->monst;
    
    if(m == moHunterGuard && items[itHunting] >= 10)
      c->monst = moHunterChanging;
    
    if ((havewhat & HF_FAILED_AMBUSH) && hyperbolic && !quotient) {
      if(m == moHunterDog)
        c->monst = moHunterChanging;
      forCellEx(c2, c)
        if(c2->monst == moHunterDog)
          c2->monst = moHunterChanging;
      }
    
    if(m == moSleepBull && !peace::on) {
      bool wakeup = false;
      forCellEx(c2, c) if(c2->monst == moGadfly) {
        addMessage(XLAT("%The1 wakes up %the2.", c2->monst, m));
        wakeup = true;
        }
      for(int i=0; i<isize(targets); i++) {
        cell *t = targets[i];
        if(celldistance(c, t) <= 2) wakeup = true;
        }
      if(wakeup) {
        playSound(NULL, "bull");
        c->monst = m = moRagingBull;
        c->mondir = NODIR;
        }
      }
    
    if(m == moNecromancer) {
      pathdata pd(moNecromancer);
      int gravenum = 0, zombienum = 0;
      cell *gtab[8], *ztab[8];
      for(int j=0; j<c->type; j++) if(c->move(j)) {
        if(c->move(j)->wall == waFreshGrave) gtab[gravenum++] = c->move(j);
        if(passable(c->move(j), c, 0) && c->move(j)->pathdist < c->pathdist)
          ztab[zombienum++] = c->move(j);
        }
      if(gravenum && zombienum) {
        cell *gr = gtab[hrand(gravenum)];
        gr->wall = waAncientGrave;
        gr->monst = moGhost;
        gr->stuntime = 1;
        ztab[hrand(zombienum)]->monst = moZombie;
        ztab[hrand(zombienum)]->stuntime = 1;
        addMessage(XLAT("%The1 raises some undead!", m));
        playSound(c, "necromancy");
        }
      }
    
    else if(m == moOutlaw) {
      for(cell *c1: gun_targets(c))
        if(canAttack(c, moOutlaw, c1, c1->monst, AF_GETPLAYER | AF_ONLY_FBUG | AF_GUN)) {
          attackMonster(c1, AF_GETPLAYER | AF_ONLY_FBUG | AF_GUN, moOutlaw);
          c->stuntime = 1;
          break;
          }
      }

    else if(m == moWitchFlash && flashWouldKill(c, AF_GETPLAYER | AF_ONLY_FBUG) && !flashWouldKill(c, false)) {
      addMessage(XLAT("%The1 activates her Flash spell!", m));
      m = moWitch;
      activateFlashFrom(c, moWitchFlash, AF_MAGIC | AF_GETPLAYER | AF_MSG);
      c->stuntime = 1;
      }

    else if(m == moCrystalSage && c->cpdist <= 4 && isIcyLand(cwt.at) && cwt.at->wall != waBoat) {
      // only one sage attacks
      if(sagefresh) {
        sagefresh = false;
        if(sagephase == 0) {
          addMessage(XLAT("%The1 shows you two fingers.", m));
          addMessage(XLAT("You wonder what does it mean?"));
          }
        else if(sagephase == 1) {
          addMessage(XLAT("%The1 shows you a finger.", m));
          addMessage(XLAT("You think about possible meanings."));
          }
        else {
          addMessage(XLAT("%The1 moves his finger downwards.", m));
          addMessage(XLAT("Your brain is steaming."));
          }
        sagephase++;
        for(int i=0; i<isize(targets); i++) {
          cell *t = targets[i];
          if(celldistance(c, t) > 4) continue;
          sageheat(t, .0);
          for(int i=0; i<t->type; i++)
            sageheat(t->move(i), .3);
          }
        }
      c->stuntime = 1;
      }
    
    else if(m == moPyroCultist && !peace::on) {
      bool shot = false;
      bool dont_approach = false;
      // smaller range on the sphere
      int firerange = (sphere || getDistLimit() < 5) ? 2 : 4;
      for(int i=0; i<isize(targets); i++) {
        cell *t = targets[i];
        if(celldistance(c,t) <= firerange && makeflame(t, 20, true)) {
          if(isPlayerOn(t)) 
            addMessage(XLAT("%The1 throws fire at you!", m));
          else
            addMessage(XLAT("%The1 throws fire at %the2!", m, t->monst));
          makeflame(t, 20, false);
          playSound(t, "fire");
          c->monst = moCultist;
          shot = true;
          }
        if(celldistance(c,t) <= 3 && !sphere) dont_approach = true;
        }
      if(shot || dont_approach) c->stuntime = 1;
      }

    else if(m == moHexer && c->item && (classflag(c->item) & IF_CURSE) && !peace::on) {
      // bool dont_approach = false;
      // smaller range on the sphere
      int firerange = (sphere || getDistLimit() < 5) ? 2 : 4;
      
      bool dont_approach = false;
      for(int i=0; i<isize(targets); i++) {
        cell *t = targets[i];
        
        if(isPlayerOn(t)) {
          int d = celldistance(c,t);
          if(d <= firerange) {
            addMessage(XLAT("%The1 curses you with %the2!", m, c->item));
            playSound(t, "fire");
            animate_item_throw(c, t, c->item);
            items[c->item] += orbcharges(c->item);
            c->item = itNone;
            c->stuntime = 1;
            }
          if(d == firerange+1) dont_approach = true;
          }
        }
      
      if(dont_approach) c->stuntime = 1;
      }

    else if(m == moVampire) {
      for(int i=0; i<isize(targets); i++) {
        cell *t = targets[i];
        if(celldistance(c,t) <= 2) {
          bool msg = false;
          for(int i=0; i<ittypes; i++)
            if(itemclass(eItem(i)) == IC_ORB && items[i] && items[itOrbTime] && !orbused[i]) {
              orbused[i] = true;
              msg = true;
              }
          if(msg) addMessage(XLAT("%The1 drains your powers!", m));
          c->stuntime = 1;
          }
        }
      }
    }
  }

EX void moveworms() {
  if(!isize(worms)) return;
  pathdata pd(moWorm);
  int wrm = isize(worms);
  for(int i=0; i<wrm; i++) {
    moveWorm(worms[i]);
    }
  }

EX void refreshFriend(cell *c) {
  if(c->monst == moGolemMoved) c->monst = moGolem;
  if(c->monst == moMouseMoved) c->monst = moMouse;
  if(c->monst == moPrincessMoved) c->monst = moPrincess;
  if(c->monst == moPrincessArmedMoved) c->monst = moPrincessArmed;
  if(c->monst == moKnightMoved) c->monst = moKnight;
  if(c->monst == moTameBomberbirdMoved) c->monst = moTameBomberbird;
  }

EX void consMove(cell *c, eMonster param) {
  eMonster m = c->monst;

  if(movegroup(m) != moYeti) return;
  
  if(m == moWitchSpeed) havewhat |= HF_FAST;
  bool slow = slowMover(m);
  if(slow) havewhat |= HF_SLOW;

  if(param == moYeti && slow) return;
  if(param == moTortoise && !slow) return;
  if(param == moWitchSpeed && m != moWitchSpeed) return;
  
  if(isActiveEnemy(c, moPlayer)) {
    int goodmoves = 0;
    for(int t=0; t<c->type; t++) {
      cell *c2 = c->move(t);
      if(c2 && c2->pathdist < c->pathdist)
        goodmoves++;
      }
    movesofgood.grow(goodmoves).push_back(c);
    }
  else 
    movesofgood.grow(0).push_back(c);
  }

EX void moveNormals(eMonster param) {
  pathdata pd(param);
  
  movesofgood.clear();

  for(int i=0; i<isize(pathqm); i++) 
    consMove(pathqm[i], param);
  
  int dcs = isize(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->pathdist == PINFD) consMove(c, param);
    }

  for(auto& v: movesofgood) for(cell *c: v) {
    if(minf[c->monst].mgroup == moYeti) {
      moveNormal(c, MF_PATHDIST);
      }
    }
  }

EX void movehex_all() {
  for(int i: snaketypes) {
    movehex(false, i);
    if(!shmup::on && haveMount()) movehex(true, i);
    }
  movehex_rest(false);
  movehex_rest(true);
  }
  
EX void movemonsters() {
  #if CAP_COMPLEX2
  ambush::distance = 0;
  #endif

  DEBB(DF_TURN, ("lava1"));
  orboflava(1);
  
  #if CAP_COMPLEX2
  ambush::check_state();
  #endif

  sagefresh = true;
  turncount++;

  specialMoves();

  DEBB(DF_TURN, ("ghosts"));
  moveghosts();
    
  DEBB(DF_TURN, ("butterflies"));
  moveButterflies();

  DEBB(DF_TURN, ("normal"));
  moveNormals(moYeti);

  DEBB(DF_TURN, ("slow"));
  if(havewhat & HF_SLOW) moveNormals(moTortoise);
  
  if(sagefresh) sagephase = 0;
  
  DEBB(DF_TURN, ("ivy"));
  moveivy();
  DEBB(DF_TURN, ("slimes"));
  groupmove(moSlime, 0);
  DEBB(DF_TURN, ("sharks"));
  if(havewhat & HF_SHARK) groupmove(moShark, 0);
  DEBB(DF_TURN, ("eagles"));
  if(havewhat & HF_BIRD) groupmove(moEagle, 0);
  if(havewhat & HF_EAGLES) groupmove(moEagle, MF_NOATTACKS | MF_ONLYEAGLE);
  DEBB(DF_TURN, ("eagles"));
  if(havewhat & HF_REPTILE) groupmove(moReptile, 0);
  DEBB(DF_TURN, ("jumpers"));
  if(havewhat & HF_JUMP) {
    groupmove(moFrog, 0);
    groupmove(moVaulter, 0);
    groupmove(moPhaser, 0);
    }
  DEBB(DF_TURN, ("air"));
  if(havewhat & HF_AIR) {
    airmap.clear();
    groupmove(moAirElemental, 0); 
    buildAirmap();
    }
  DEBB(DF_TURN, ("earth"));
  if(havewhat & HF_EARTH) groupmove(moEarthElemental, 0);
  DEBB(DF_TURN, ("water"));
  if(havewhat & HF_WATER) groupmove(moWaterElemental, 0);
  DEBB(DF_TURN, ("void"));
  if(havewhat & HF_VOID) groupmove(moVoidBeast, 0);
  DEBB(DF_TURN, ("leader"));
  if(havewhat & HF_LEADER) groupmove(moPirate, 0);
  DEBB(DF_TURN, ("mutant"));
  if((havewhat & HF_MUTANT) || (closed_or_bounded && among(specialland, laOvergrown, laClearing))) movemutant();
  DEBB(DF_TURN, ("bugs"));
  if(havewhat & HF_BUG) hive::movebugs();
  DEBB(DF_TURN, ("whirlpool"));
  if(havewhat & HF_WHIRLPOOL) whirlpool::move();
  DEBB(DF_TURN, ("whirlwind"));
  if(havewhat & HF_WHIRLWIND) whirlwind::move();
  #if CAP_COMPLEX2
  DEBB(DF_TURN, ("westwall"));
  if(havewhat & HF_WESTWALL) westwall::move();
  #endif
  for(cell *pc: player_positions())
    if(pc->item == itOrbSafety) 
      return;
  DEBB(DF_TURN, ("river"));
  if(havewhat & HF_RIVER) prairie::move();
  /* DEBB(DF_TURN, ("magnet"));
  if(havewhat & HF_MAGNET) 
    groupmove(moSouthPole, 0),
    groupmove(moNorthPole, 0); */
  DEBB(DF_TURN, ("bugs"));
  if(havewhat & HF_HEXD) groupmove(moHexDemon, 0);
  if(havewhat & HF_DICE) groupmove(moAnimatedDie, 0);    
  if(havewhat & HF_ALT) groupmove(moAltDemon, 0);
  if(havewhat & HF_MONK) groupmove(moMonk, 0);

  DEBB(DF_TURN, ("worm"));
  cell *savepos[MAXPLAYER];
  
  for(int i=0; i<numplayers(); i++)
    savepos[i] = playerpos(i);

  moveworms();
  if(havewhat & HF_HEX) 
    movehex_all();

  if(havewhat & HF_KRAKEN) kraken::attacks(), groupmove(moKrakenH, 0);
  if(havewhat & HF_DRAGON) groupmove(moDragonHead, MF_NOFRIEND);
  if(haveMount()) groupmove(moDragonHead, MF_MOUNT);

  DEBB(DF_TURN, ("golems"));
  movegolems(0);
  
  DEBB(DF_TURN, ("fresh"));
  moverefresh();
  
  DEBB(DF_TURN, ("lava2"));
  orboflava(2);

  DEBB(DF_TURN, ("shadow"));
  moveshadow();
  
  DEBB(DF_TURN, ("wandering"));
  wandering();
  
  DEBB(DF_TURN, ("rosemap"));
  if(havewhat & HF_ROSE) buildRosemap();

  for(int i=0; i<numplayers(); i++)
    if(savepos[i] != playerpos(i)) {
      bfs(); break;
      }
  }

EX bool nogoSlow(cell *to, cell *from) {
  if(cellEdgeUnstable(to) && gravityLevelDiff(to, from) >= 0) return true;
  if(cellUnstable(to)) return true;
  return false;
  }

EX void beastcrash(cell *c, cell *beast) {
  if(c->wall == waPetrified || c->wall == waDeadTroll || c->wall == waDeadTroll2 ||
    c->wall == waGargoyle) {
    addMessage(XLAT("%The1 crashes into %the2!", beast->monst, c->wall));
    c->wall = waNone;
    }
  else if(c->wall == waDeadwall || c->wall == waCavewall || c->wall == waSandstone ||
    c->wall == waVinePlant || c->wall == waIcewall ||
    c->wall == waMirror || c->wall == waCloud || c->wall == waBigTree || c->wall ==
    waSmallTree || c->wall == waGlass || c->wall == waClosedGate || c->wall == waStone || c->wall == waRuinWall) {
    addMessage(XLAT("%The1 crashes into %the2!", beast->monst, c->wall));    
    c->wall = waNone;
    }
  else if(cellHalfvine(c)) {
    addMessage(XLAT("%The1 crashes into %the2!", beast->monst, c->wall));
    destroyHalfvine(c);
    }
  else if(c->wall == waThumperOff) {
    addMessage(XLAT("%The1 crashes into %the2!", beast->monst, c->wall));
    c->wall = waThumperOn;
    c->wparam = 100;
    }  
  else if(c->wall == waExplosiveBarrel) {
    addMessage(XLAT("%The1 crashes into %the2!", beast->monst, c->wall));
    explodeBarrel(c);
    }
  else if(realred(c)) {
    addMessage(XLAT("%The1 crashes into %the2!", beast->monst, c->wall));
    if(c->wall == waRed1) c->wall = waNone;
    else if(c->wall == waRed2) c->wall = waRed1;
    else if(c->wall == waRed3) c->wall = waRed2;
    }
  else if(isBull(c->monst) || isSwitch(c->monst)) {
    addMessage(XLAT("%The1 crashes into %the2!", beast->monst, c->monst));
    if(c->monst == moSleepBull) c->monst = moRagingBull, c->stuntime = 3;
    }
  }

EX void stayEffect(cell *c) {
  eMonster m = c->monst;
  if(m == moAirElemental) airmap.push_back(make_pair(c, 0));
  if(m == moRagingBull && c->mondir != NODIR) { 
    playSound(NULL, "hit-axe"+pick123());
    forCellIdEx(c2, d, c) {
      bool opposite = anglestraight(c, d, c->mondir);
      if(opposite) beastcrash(c2, c);
      }
    c->mondir = NODIR; c->stuntime = 3; 
    }
  }

EX int realstuntime(cell *c) {
  if(isMutantIvy(c)) return (c->stuntime - mutantphase) & 15;
  return c->stuntime;
  }


}
