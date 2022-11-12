// Hyperbolic Rogue - passability
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file checkmove.cpp
 *  \brief check whether monster/PC can move in the given direction
 */

#include "hyper.h"

namespace hr {

// === MOVEMENT FUNCTIONS ===

// w = from->move(d)
EX bool againstCurrent(cell *w, cell *from) {
  if(from->land != laWhirlpool) return false;
  if(againstWind(from, w)) return false; // wind is stronger than current
  if(!eubinary && (!from->master->alt || !w->master->alt)) return false;
  int dfrom = celldistAlt(from);
  int dw = celldistAlt(w);
  if(dw < dfrom) return false;
  if(dfrom < dw) return true;
  for(int d=0; d<from->type; d++) 
    if(from->move(d) == w) {
       cell *c3 = from->modmove(d-1);
       if(!c3) return false;
       return celldistAlt(c3) < dfrom;
       }
  return false;
  }

EX bool boatGoesThrough(cell *c) {
  if(isGravityLand(c->land)) return false;
  return
    (c->wall == waNone && c->land != laMotion && c->land != laZebra && c->land != laReptile) || 
    isAlchAny(c) || 
    c->wall == waCavefloor || c->wall == waFrozenLake || isReptile(c->wall) ||
    c->wall == waDeadfloor || c->wall == waCIsland || c->wall == waCIsland2 ||
    c->wall == waMineUnknown || c->wall == waMineMine || c->wall == waMineOpen ||
    c->wall == waBonfireOff || c->wall == waFire || c->wall == waPartialFire ||
    c->wall == waArrowTrap || c->wall == waShallow;
  }

EX void become_water(cell *c) {
  if(isIcyLand(c))
    c->wall = waLake;
  else if(isCoastal(c) || isSealand(c))
    c->wall = waSea;
  else
    c->wall = waDeepWater;
  }

EX void placeWater(cell *c, cell *c2) {
  destroyTrapsOn(c);
  if(isWatery(c)) ;
  else if(c2 && isAlchAny(c2))
    c->wall = c2->wall;
  else become_water(c);
  // destroy the ancient treasure!
  if(c->item == itBarrow) c->item = itNone;
  }

EX int incline(cell *cfrom, cell *cto) {
  return snakelevel(cto) - snakelevel(cfrom);
  }

#define F(x) checkflags(flags,x)

EX bool checkflags(flagtype flags, flagtype x) {
  if(flags & x) return true;
  if(flags & P_ISPLAYER) {
    if((x & P_WINTER)    && (markOrb(itOrbWinter) || markOrb(itCurseWater))) return true;
    if((x & P_IGNORE37)  && markOrb(itOrb37)) return true;
    if((x & P_FISH)      && markOrb(itOrbFish)) return true;
    if((x & P_MARKWATER) && markOrb(itOrbWater)) return true;
    if((x & P_AETHER)    && markOrb2(itOrbAether) && !(flags&P_NOAETHER)) return true;
    if((x & P_WATERCURSE)&& markOrb2(itCurseWater)) return true;
    }
  if(flags & P_ISFRIEND) if(items[itOrbEmpathy]) 
    if(checkflags(flags ^ P_ISPLAYER ^ P_ISFRIEND, x) && markOrb(itOrbEmpathy))
      return true;
  return false;
  }

EX bool strictlyAgainstGravity(cell *w, cell *from, bool revdir, flagtype flags) {
  return
    cellEdgeUnstable(w, flags) && cellEdgeUnstable(from, flags) && 
    !(shmup::on && from == w) && gravityLevelDiff(from, w) != (revdir?-1:1) * gravity_zone_diff(from);
  }

EX bool anti_alchemy(cell *w, cell *from) {
  if(!from) return false;
  if(!isAlchAny(w)) return false;
  if(!isAlchAny(from)) return false;
  if(w->item) return false;
  if(from->item) return false;
  if(!nonorientable) return w->wall != from->wall;
  forCellIdEx(c1, i, w)
    if(c1 == from && (w->c.mirror(i) ? w->wall != from->wall : w->wall == from->wall))
      return false;
  return true;
  }

#if HDR
#define P_MONSTER    Flag(0)  // can move through monsters
#define P_MIRROR     Flag(1)  // can move through mirrors
#define P_REVDIR     Flag(2)  // reverse direction movement
#define P_WIND       Flag(3)  // can move against the wind
#define P_GRAVITY    Flag(4)  // can move against the gravity
#define P_ISPLAYER   Flag(5)  // player-only moves (like the Round Table jump)
#define P_ONPLAYER   Flag(6)  // always can step on the player
#define P_FLYING     Flag(7)  // is flying
#define P_BULLET     Flag(8)  // bullet can fly through more things
#define P_MIRRORWALL Flag(9)  // mirror images go through mirror walls
#define P_JUMP1      Flag(10) // first part of a jump
#define P_JUMP2      Flag(11) // second part of a jump
#define P_TELE       Flag(12) // teleport onto
#define P_BLOW       Flag(13) // Orb of Air -- blow, or push
#define P_AETHER     Flag(14) // aethereal
#define P_FISH       Flag(15) // swimming
#define P_WINTER     Flag(16) // fire resistant
#define P_USEBOAT    Flag(17) // can use boat
#define P_NOAETHER   Flag(18) // disable AETHER
#define P_FRIENDSWAP Flag(19) // can move on friends (to swap with tem)
#define P_ISFRIEND   Flag(20) // is a friend (can use Empathy + Winter/Aether/Fish combo)
#define P_LEADER     Flag(21) // can push statues and use boats
#define P_MARKWATER  Flag(22) // mark Orb of Water as used
#define P_EARTHELEM  Flag(23) // Earth Elemental
#define P_WATERELEM  Flag(24) // Water Elemental
#define P_IGNORE37   Flag(25) // ignore the triheptagonal board
#define P_CHAIN      Flag(26) // for chaining moves with boats
#define P_DEADLY     Flag(27) // suicide moves allowed
#define P_ROSE       Flag(28) // rose smell
#define P_CLIMBUP    Flag(29) // allow climbing up
#define P_CLIMBDOWN  Flag(30) // allow climbing down
#define P_REPTILE    Flag(31) // is reptile
#define P_VOID       Flag(32) // void beast
#define P_PHASE      Flag(33) // phasing movement
#define P_PULLMAGNET Flag(34) // pull the other part of the magnet
#define P_WATERCURSE Flag(35) // Curse of Water
#endif

EX bool passable(cell *w, cell *from, flagtype flags) {
  bool revdir = (flags&P_REVDIR);
  bool vrevdir = revdir ^ bool(flags&P_VOID);

  if(from && from != w && nonAdjacent(from, w) && !F(P_IGNORE37 | P_BULLET)) return false;
  
  if((isWateryOrBoat(w) || w->wall == waShallow) && F(P_WATERCURSE))
    return false;
  
  for(cell *pp: player_positions()) {
    if(w == pp && F(P_ONPLAYER)) return true;
    if(from == pp && F(P_ONPLAYER) && F(P_REVDIR)) return true;

    if(from && !((flags & P_ISPLAYER) && pp->monst)) {
      int i = vrevdir ? incline(w, from) : incline(from, w);
      if(in_gravity_zone(w)) {
        if(gravity_state == gsLevitation) i = 0;
        if(gravity_state == gsAnti && i > 1) i = 1;
        }
      if(i < -1 && F(P_ROSE)) return false;
      if((i > 1) && !F(P_JUMP1 | P_JUMP2 | P_BULLET | P_FLYING | P_BLOW | P_CLIMBUP | P_AETHER | P_REPTILE))
        return false;
      if((i < -2) && !F(P_DEADLY | P_JUMP1 | P_JUMP2 | P_BULLET | P_FLYING | P_BLOW | P_CLIMBDOWN | P_AETHER | P_REPTILE))
        return false;
      }
    }

  if(F(P_ROSE)) {
    if(airdist(w) < 3) return false;
    if(againstWind(w,from)) return false;
    if(isGravityLand(w)) return false;
    }

  if(from && strictlyAgainstGravity(w, from, vrevdir, flags)
    && !((flags & P_ISPLAYER) && shmup::on)
    && !F(P_GRAVITY | P_BLOW | P_JUMP1 | P_JUMP2 | P_FLYING | P_BULLET | P_AETHER)
    ) return false;
  
  if(from && (vrevdir ? againstWind(from,w) : againstWind(w, from)) && !F(P_WIND | P_BLOW | P_JUMP1 | P_JUMP2 | P_BULLET | P_AETHER)) return false;
  
  if(revdir && from && w->monst && passable(from, w, flags &~ (P_REVDIR|P_MONSTER)))
    return true;
  
  if(!shmup::on && sword::at(w, flags & P_ISPLAYER) && !F(P_DEADLY | P_BULLET | P_ROSE))
    return false;

  bool alch1 = anti_alchemy(w, from);
  
  if(alch1) {
    bool alchok = (in_gravity_zone(w) || in_gravity_zone(from));
    alchok = alchok || (F(P_JUMP1 | P_JUMP2 | P_FLYING | P_TELE | P_BLOW | P_AETHER | P_BULLET) 
      && !F(P_ROSE));
    if(!alchok) return false;
    }

  if(from && thruVine(from, w) && !F(P_AETHER)) return false;

  if(w->monst == moMouse && F(P_JUMP1)) ;
  else if(w->monst && isFriendly(w) && F(P_FRIENDSWAP)) ;
  else if(w->monst && !F(P_MONSTER)) return false;

  if(w->wall == waMirror || w->wall == waCloud) 
    return F(P_MIRROR | P_AETHER);
  
  if(w->wall == waMirrorWall)
    return F(P_MIRRORWALL);
  
  if(F(P_BULLET)) {
    if(isFire(w) || w->wall == waBonfireOff || cellHalfvine(w) ||  
      w->wall == waMagma ||
      w->wall == waAncientGrave || w->wall == waFreshGrave || w->wall == waRoundTable)
      return true;
    }

  if(F(P_LEADER)) {
    if(from && from->wall == waBoat && isWatery(w) && from->item == itOrbYendor)
      return false;

    if(from && from->wall == waBoat && isWateryOrBoat(w) && !againstCurrent(w, from))
      return true;
    
    if(from && isWatery(from) && w->wall == waBoat && F(P_CHAIN))
      return true;

    if(from && isWatery(from) && isWatery(w) && F(P_CHAIN) && !againstCurrent(w, from))
      return true;

    if(w->wall == waBigStatue && from && canPushStatueOn(from, flags)) return true;
    }
  
  if(F(P_EARTHELEM)) {
    // cannot go through Living Caves...
    if(w->wall == waCavefloor) return false;
    // but can dig through...
    if(w->wall == waDeadwall || w->wall == waDune || w->wall == waStone)
      return true;
    // and can swim through...
    if(w->wall == waSea && w->land == laLivefjord)
      return true;
    }
  
  if(F(P_WATERELEM)) {
    if(isWatery(w) || boatGoesThrough(w) || 
      w->wall == waBoat ||
      w->wall == waDeadTroll || w->wall == waDeadTroll2) return true;
    return false;
    }
  
  if(isThorny(w->wall) && F(P_BLOW | P_DEADLY)) return true;

  if(isFire(w) || w->wall == waMagma) {
    if(w->wall == waMagma && in_gravity_zone(w)) ;
    else if(!F(P_AETHER | P_WINTER | P_BLOW | P_JUMP1 | P_BULLET | P_DEADLY)) return false;
    }
  
  if(in_gravity_zone(w) && gravity_state == gsAnti && !isGravityLand(w->land) && (!from || !isGravityLand(from->land))) 
  if(!F(P_AETHER | P_BLOW | P_JUMP1 | P_BULLET | P_FLYING)) {
    bool next_to_wall = false;
    forCellEx(c2, w) if(isJWall(c2)) next_to_wall = true;
    if(from) forCellEx(c2, from) if(isJWall(c2)) next_to_wall = true;
    if(!next_to_wall && (!from || incline(from, w) * (vrevdir?-1:1) <= 0)) return false;
    }
    
  if(isWatery(w)) {
    if(in_gravity_zone(w)) ;
    else if(from && from->wall == waBoat && F(P_USEBOAT) && 
      (!againstCurrent(w, from) || F(P_MARKWATER)) && !(from->item == itOrbYendor)) ;
    else if(!F(P_AETHER | P_FISH | P_FLYING | P_BLOW | P_JUMP1 | P_BULLET | P_DEADLY | P_REPTILE)) return false;
    }
  if(isChasmy(w)) {
    if(in_gravity_zone(w)) ;
    else if(!F(P_AETHER | P_FLYING | P_BLOW | P_JUMP1 | P_BULLET | P_DEADLY | P_REPTILE)) return false;  
    }

  if(w->wall == waRoundTable && from && from->wall != waRoundTable && (flags & P_ISPLAYER)) return true;
  if(isNoFlight(w) && F(P_FLYING | P_BLOW | P_JUMP1)) return false;

  if(isWall(w)) {
    // a special case: empathic aethereal beings cannot go through Round Table
    // (but truly aetheral beings can)
    if(w->wall == waRoundTable) {
      if(!(flags & P_AETHER)) return false;
      }
    else if(!F(P_AETHER)) return false;
    }
  return true;
  }

EX vector<pair<cell*, int> > airmap;

EX int airdist(cell *c) {
  if(!(havewhat & HF_AIR)) return 3;
  vector<pair<cell*, int> >::iterator it = 
    lower_bound(airmap.begin(), airmap.end(), make_pair(c,0));
  if(it != airmap.end() && it->first == c) return it->second;
  return 3;
  }

EX ld calcAirdir(cell *c) {
  if(!c || c->monst == moAirElemental || !passable(c, NULL, P_BLOW))
    return 0;
  for(int i=0; i<c->type; i++) {
    cell *c2 = c->move(i);
    if(c2 && c2->monst == moAirElemental) {
      return c->c.spin(i) * TAU / c2->type;
      }
    }
  for(int i=0; i<c->type; i++) {
    cell *c2 = c->move(i);
    if(!c2) continue;
    if(!passable(c2, c, P_BLOW | P_MONSTER)) continue;
    if(!passable(c, c2, P_BLOW | P_MONSTER)) continue;
    for(int i=0; i<c2->type; i++) {
      cell *c3 = c2->move(i);
      if(c3 && c3->monst == moAirElemental) {
        return c2->c.spin(i) * TAU / c3->type;
        }
      }
    }
  return 0;
  }

EX bool againstWind(cell *cto, cell *cfrom) {
  if(!cfrom || !cto) return false;
  int dcto = airdist(cto), dcfrom = airdist(cfrom);
  if(dcto < dcfrom) return true;
  #if CAP_FIELD
  if(cfrom->land == laBlizzard && !shmup::on && cto->land == laBlizzard && dcto == 3 && dcfrom == 3) {
    char vfrom = windmap::at(cfrom);
    char vto = windmap::at(cto);
    int z = (vfrom-vto) & 255;
    if(z >= windmap::NOWINDBELOW && z < windmap::NOWINDFROM)
      return true;
    }
  #endif
  whirlwind::calcdirs(cfrom);
  int d = neighborId(cfrom, cto);
  if(whirlwind::winddir(d) == -1) return true;
  return false;
  }

EX bool ghostmove(eMonster m, cell* to, cell* from, flagtype extra) {
  if(!isGhost(m) && nonAdjacent(to, from)) return false;
  if(sword::at(to, 0)) return false;
  if(!shmup::on && isPlayerOn(to)) return false;
  if(to->monst && !(to->monst == moTentacletail && isGhost(m) && m != moFriendlyGhost)
    && !(to->monst == moTortoise && isGhost(m) && m != moFriendlyGhost) && !(extra & P_MONSTER))
    return false;
  if((m == moWitchGhost || m == moWitchWinter) && to->land != laPower)
    return false;
  if(isGhost(m))
    for(int i=0; i<to->type; i++) if(to->move(i)) {
      if(inmirror(to->move(i))) return false;
      if(to->move(i) && to->move(i) != from && isGhost(to->move(i)->monst) &&
        (to->move(i)->monst == moFriendlyGhost) == (m== moFriendlyGhost))
        return false;
      }
  if(isGhost(m) || m == moWitchGhost) return true;
  if(m == moGreaterShark) return isWatery(to);
  if(m == moWitchWinter) 
    return passable(to, from, P_WINTER | P_ONPLAYER);
  return false;
  }

bool slimepassable(cell *w, cell *c) {
  if(w == c || !c) return true;
  int u = neighborId(c, w);
  if(nonAdjacent(w,c)) return false;
  if(isPlayerOn(w)) return true;
  int group = slimegroup(c);
  if(!group) return false;
  int ogroup = slimegroup(w);
  if(!ogroup) return false;
  bool hv = (group == ogroup);
  if(nonorientable && isAlchAny(c) && isAlchAny(w))
    hv = !anti_alchemy(c, w);

  if(sword::at(w, 0)) return false;
  
  if(w->item) return false;

  // only travel to halfvines correctly
  if(cellHalfvine(c)) {
    int i=0;
    for(int t=0; t<c->type; t++) if(c->move(t) && c->move(t)->wall == c->wall) i=t;
    int z = i-u; if(z<0) z=-z; z%=6;
    if(z>1) return false;
    hv=(group == ogroup);
    }
  // only travel from halfvines correctly
  if(cellHalfvine(w)) {
    int i=0;
    for(int t=0; t<w->type; t++) if(w->move(t) && w->move(t)->wall == w->wall) i=t;
    int z = i-c->c.spin(u); if(z<0) z=-z; z%=6;
    if(z>1) return false;
    hv=(group == ogroup);
    }
  if(!hv) return false;
  return true;
  }

bool sharkpassable(cell *w, cell *c) {
  if(w == c || !c) return true;
  if(nonAdjacent(w,c)) return false;
  if(isPlayerOn(w)) return true;
  if(!isWatery(w) && w->wall != waShallow) return false;
  if(sword::at(w, 0)) return false;
  
  // don't go against the current
  if(isWateryOrBoat(w) && isWateryOrBoat(c)) 
    return !againstCurrent(w, c);

  return true;
  }

EX bool canPushStatueOn(cell *c, flagtype flags) {
  return passable(c, NULL, P_MONSTER | flags) && !snakelevel(c) &&
    !isWorm(c->monst) && !isReptile(c->wall) && !peace::on && 
    !cellHalfvine(c) && !isDie(c->wall) &&
    !among(c->wall, waBoat, waFireTrap, waArrowTrap);
  }

EX void moveBoat(const movei& mi) {
  eWall x = mi.t->wall; mi.t->wall = mi.s->wall; mi.s->wall = x;
  mi.t->mondir = mi.rev_dir_or(NODIR);
  moveItem(mi.s, mi.t, false);
  animateMovement(mi, LAYER_BOAT);
  }

EX void moveBoatIfUsingOne(const movei& mi) {
  if(mi.s->wall == waBoat && isWatery(mi.t)) moveBoat(mi);
  else if(mi.s->wall == waBoat && boatGoesThrough(mi.t) && isFriendly(mi.t) && markEmpathy(itOrbWater)) {
    placeWater(mi.t, mi.s);
    moveBoat(mi);
    }
  }

bool againstMagnet(cell *c1, cell *c2, eMonster m) { // (from, to)
  if(false) forCellEx(c3, c2) { 
    if(c3 == c1) continue;
    if(c3->monst == m)
      return true;
    /* if(c3->monst == otherpole(m) && c3->move(c3->mondir) != c1) {
      int i = 0;
      forCellEx(c4, c3) if(c4->monst == m) i++;
      if(i == 2) return true;
      } */
    }
  if(c1->monst == m && !isNeighbor(c2, c1->move(c1->mondir)))
    return true;
  forCellEx(c3, c1) 
    if(c3->monst != m && isMagneticPole(c3->monst))
      if(!isNeighbor(c3, c2))
        return true;
  return false;
  }

EX bool againstPair(cell *c1, cell *c2, eMonster m) { // (from, to)
  if(c1->monst == m && !isNeighbor(c2, c1->move(c1->mondir)))
    return true;
  return false;
  }

EX bool notNearItem(cell *c) {
  forCellCM(c2, c) if(c2->item) return false;
  return true;
  }

EX bool isNeighbor1(cell *f, cell *w) {
  return !f || f == w || isNeighbor(f, w);
  }

EX bool passable_for(eMonster m, cell *w, cell *from, flagtype extra) {
  cell *dummy;
  if(w->monst && !(extra & P_MONSTER) && !isPlayerOn(w)) 
    return false;
  if(m == moWolf) {
    return (isIcyLand(w) || w->land == laVolcano) && (isPlayerOn(w) || passable(w, from, extra));
    }
  if(isMagneticPole(m))
    return !(w && from && againstMagnet(from, w, m)) && passable(w, from, extra);
  if(m == moPair)
    return !(w && from && againstPair(from, w, m)) && passable(w, from, extra);
  if(m == passive_switch) return false;
  if(minf[m].mgroup == moYeti || isBug(m) || isDemon(m) || m == moHerdBull || m == moMimic || m == moAsteroid) {
    if((isWitch(m) || m == moEvilGolem) && w->land != laPower && w->land != laHalloween)
      return false;
    return passable(w, from, extra);
    }
  if(m == moDragonHead && prairie::isriver(w))
    return false;
  if(isShark(m))
    return sharkpassable(w, from);
  if(isSlimeMover(m))
    return slimepassable(w, from);
  if(m == moKrakenH) {
    if(extra & P_ONPLAYER) {
      if(isPlayerOn(w)) return true;
      }
    if((extra & P_ONPLAYER) && isPlayerOn(w))
      return true;
    if(kraken_pseudohept(w) || kraken_pseudohept(from)) return false;
    if(w->wall != waBoat && !slimepassable(w, from)) return false;
    forCellEx(w2, w) if(w2->wall != waBoat && !passable(w2, w, P_FISH | P_MONSTER)) return false;
    return true;
    }
  if(m == moEarthElemental)
    return passable(w, from, extra | P_EARTHELEM);
  if(m == moWaterElemental) 
    return passable(w, from, extra | P_WATERELEM);
  if(m == moGreaterShark)
    return isWatery(w) || w->wall == waBoat || w->wall == waFrozenLake;
  if(isGhostMover(m) || m == moFriendlyGhost)
    return ghostmove(m, w, from, extra);
    // for the purpose of Shmup this is correct
  if(m == moTameBomberbird)
    return passable(w, from, extra | P_FLYING | P_ISFRIEND);
  if(m == moHexSnake)
    return !pseudohept(w) && passable(w, from, extra|P_WIND|P_FISH);
  if(isBird(m)) {
    if(bird_disruption(w) && (!from || bird_disruption(from)) && markOrb(itOrbGravity))
      return passable(w, from, extra);
    else 
      return passable(w, from, extra | P_FLYING);
    }
  if(m == moReptile)
    return passable(w, from, extra | P_REPTILE);
  if(isDragon(m))
    return passable(w, from, extra | P_FLYING | P_WINTER);
  if(m == moAirElemental)
    return passable(w, from, extra | P_FLYING | P_WIND);
  if(isLeader(m)) {
    if(from && from->wall == waBoat && from->item == itCoral && !from->monst) return false; // don't move Corals!
    return passable(w, from, extra | P_LEADER);
    }
  if(isPrincess(m))
    return passable(w, from, extra | P_ISFRIEND | P_USEBOAT);
  if(isGolemOrKnight(m))
    return passable(w, from, extra | P_ISFRIEND);
  if(isWorm(m))
    return passable(w, from, extra) && !cellUnstable(w) && ((m != moWorm && m != moTentacle) || !cellEdgeUnstable(w));
  if(m == moVoidBeast)
    return passable(w, from, extra | P_VOID);
  if(m == moHexDemon) {
    if(extra & P_ONPLAYER) {
      if(isPlayerOn(w)) return true;
      }
    return !pseudohept(w) && passable(w, from, extra);
    }
  #if CAP_COMPLEX2
  if(m == moAnimatedDie) {
    if(extra & P_ONPLAYER) {
      if(isPlayerOn(w)) return true;
      }
    if(from && isDie(from->monst)) {
      bool ok = false;
      for(int i=0; i<from->type; i++) {
        if(from->move(i) != w) continue;
        if(dice::can_roll(movei(from, i))) ok = true;
        }
      if(!ok) return false;
      }
    if(from && !dice::die_possible(from)) 
      return false;
    else if(!dice::die_possible(w))
      return false;
    else
      return passable(w, from, extra);
    }
  #endif
  if(m == moFrog) {
    return isNeighbor1(from, w) ? passable(w, from, extra) : check_jump(from, w, extra, dummy) == 3;
    }
  if(m == moPhaser)
    return isNeighbor1(from, w) ? passable(w, from, extra) : check_phase(from, w, extra, dummy) == 3;
  if(m == moVaulter)
    return isNeighbor1(from, w) ? passable(w, from, extra) : check_vault(from, w, extra, dummy) == 6;
  if(m == moAltDemon) {
    if(extra & P_ONPLAYER) {
      if(isPlayerOn(w)) return true;
      }
    return (!w || !from || w==from || pseudohept(w) || pseudohept(from)) && passable(w, from, extra);
    }
  if(m == moMonk) {
    if(extra & P_ONPLAYER) {
      if(isPlayerOn(w)) return true;
      }
    return notNearItem(w) && passable(w, from, extra);
    }
  return false;
  }

EX eMonster movegroup(eMonster m) { return minf[m].mgroup; }

EX bool logical_adjacent(cell *c1, eMonster m1, cell *c2) {
  if(!c1 || !c2) return true; // cannot really check
  eMonster m2 = c2->monst;
  if(!isNeighbor(c1, c2))
    return false;
  if(thruVine(c1, c2) && !attackThruVine(m1) && !attackThruVine(m2) &&
    !checkOrb(m1, itOrbAether) && !checkOrb(m2, itOrbAether))
    return false;
  if(nonAdjacent(c1, c2) && !attackNonAdjacent(m1) && !attackNonAdjacent(m2) && 
    !checkOrb(m1, itOrb37) && !checkOrb(m1, itOrbAether) && !checkOrb(m2, itOrbAether))
    return false;
  return true;
  }

EX void buildAirmap() {
  for(int k=0; k<isize(airmap); k++) {
    int d = airmap[k].second;
    if(d == 2) break;
    cell *c = airmap[k].first;
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->move(i);
      if(!c2) continue;
      if(!passable(c2, c, P_BLOW | P_MONSTER)) continue;
      if(!passable(c, c2, P_BLOW | P_MONSTER)) continue;
      airmap.push_back(make_pair(c2, d+1));
      }
    }
  sort(airmap.begin(), airmap.end());  
  }

EX int rosewave, rosephase;

/** current state of the rose scent
 *  rosemap[c] &3 can be:
 *  0 - wave not reached
 *  1 - wave expanding
 *  2 - wave phase 1
 *  3 - wave phase 2
 */
EX map<cell*, int> rosemap;

EX int rosedist(cell *c) {
  if(!(havewhat&HF_ROSE)) return 0;
  int&r (rosemap[c]);
  if((r&7) == 7) return 0;
  if(r&3) return (r&3)-1;
  return 0;
  }

EX bool againstRose(cell *cfrom, cell *cto) {
  if(rosedist(cfrom) != 1) return false;
  if(cto && rosedist(cto) == 2) return false;
  return true;
  }

EX bool withRose(cell *cfrom, cell *cto) {
  if(rosedist(cfrom) != 1) return false;
  if(rosedist(cto) != 2) return false;
  return true;
  }

EX void buildRosemap() {

  rosephase++; rosephase &= 7;
  
  if((havewhat&HF_ROSE) && !rosephase) {
    rosewave++;
    for(int k=0; k<isize(dcal); k++) {
      cell *c = dcal[k];
      if(c->wall == waRose && c->cpdist <= gamerange() - 2) 
        rosemap[c] = rosewave * 8 + 2;
      }
    }
  
  for(map<cell*, int>::iterator it = rosemap.begin(); it != rosemap.end(); it++) {
    cell *c = it->first;
    int r = it->second;
    if(r < (rosewave) * 8) continue;
    if((r&7) == 2) if(c->wall == waRose || !isWall(c)) for(int i=0; i<c->type; i++) {
      cell *c2 = c->move(i);
      if(!c2) continue;
      // if(snakelevel(c2) <= snakelevel(c) - 2) continue;
      if(!passable(c2, c, P_BLOW | P_MONSTER | P_ROSE)) continue;
      int& r2 = rosemap[c2];
      if(r2 < r) r2 = r-1;
      }
    }

  for(map<cell*, int>::iterator it = rosemap.begin(); it != rosemap.end(); it++) {
    int& r = it->second;
    if((r&7) == 1 || (r&7) == 2 || (r&7) == 3) r++;
    if(airdist(it->first) < 3 || whirlwind::cat(it->first)) r |= 7;
    if(it->first->land == laBlizzard) r |= 7;
    forCellEx(c2, it->first) if(airdist(c2) < 3) r |= 7;
    }

  }

EX bool scentResistant() {
  return markOrb(itOrbBeauty) || markOrb(itOrbAether) || markOrb(itOrbShield);
  }


}
