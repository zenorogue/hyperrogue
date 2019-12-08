// Hyperbolic Rogue - checkmate rule
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file checkmove.cpp
 *  \brief Check the validity of move (checkmate rule)
 */

#include "hyper.h"

namespace hr {

#if HDR
#define PUREHARDCORE_LEVEL 10
#endif

/** are we in the hardcore mode */
EX bool hardcore = false;
/** when did we switch to the hardcore mode */
EX int hardcoreAt;

EX bool pureHardcore() { return hardcore && hardcoreAt < PUREHARDCORE_LEVEL; }

/** can we still move? */
EX bool canmove = true;

// how many monsters are near
EX eMonster who_kills_me;

EX int lastkills;

EX bool legalmoves[MAX_EDGE+1];

EX bool hasSafeOrb(cell *c) {
  return 
    c->item == itOrbSafety ||
    c->item == itOrbShield ||
    c->item == itOrbShell  ||
    (c->item == itOrbYendor && yendor::state(c) == yendor::ysUnlocked);
  }

#if HDR
struct stalemate1 {
  eMonster who;
  cell *moveto;
  cell *killed;
  cell *pushto;
  cell *comefrom;
  cell *swordlast[2], *swordtransit[2], *swordnext[2];
  bool isKilled(cell *c);
  stalemate1(eMonster w, cell *mt, cell *ki, cell *pt, cell *cf) : who(w), moveto(mt), killed(ki), pushto(pt), comefrom(cf) {}
  };
#endif

bool stalemate1::isKilled(cell *w) {
  if(w->monst == moNone || w == killed) return true;
  if(!moveto) return false;
  
  for(int b=0; b<2; b++) 
    if((w == swordnext[b] || w == swordtransit[b]) && canAttack(moveto, who, w, w->monst, AF_SWORD))
      return true;
  
  if(logical_adjacent(moveto, who, w) && moveto != comefrom) {
    int wid = neighborId(moveto, w);
    int wfrom = neighborId(moveto, comefrom);
    int flag = AF_APPROACH;
    if(wid >= 0 && wfrom >= 0 && anglestraight(moveto, wfrom, wid)) flag |= AF_HORNS;
    if(canAttack(moveto, who, w, w->monst, flag)) return true;
    }

  if(isNeighbor(w, comefrom) && comefrom == moveto && killed) {
    int d1 = neighborId(comefrom, w);
    int d2 = neighborId(comefrom, killed);
    int di = angledist(comefrom->type, d1, d2);
    if(di && items[itOrbSide1-1+di] && canAttack(moveto, who, w, w->monst, AF_SIDE))
      return true;
    }

  if(logical_adjacent(comefrom, who, w) && logical_adjacent(moveto, who, w) && moveto != comefrom)
    if(canAttack(moveto, who, w, w->monst, AF_STAB))
      return true;

  if(who == moPlayer && (killed || moveto != comefrom) && mirror::isKilledByMirror(w)) return true;
  if(w->monst == moIvyHead || w->monst == moIvyBranch || isMutantIvy(w))
    return isChild(w, killed);

  if(isDragon(w->monst) && killed && isDragon(killed->monst) && killed->hitpoints) {
    cell *head1 = dragon::findhead(w);
    cell *head2 = dragon::findhead(killed);
    if(head1 == head2 && dragon::totalhp(head1) ==1) return true;
    }
  
  if((w->monst == moPair || isMagneticPole(w->monst)) && killed && w->move(w->mondir) == killed)
    return true;
  
  if(w->monst == moKrakenT && killed && killed->monst == moKrakenT && killed->hitpoints) {
    cell *head1 = w->move(w->mondir);
    cell *head2 = killed->move(killed->mondir);
    if(head1 == head2 && kraken::totalhp(head1) == 1) return true;
    }
  
  return false;
  }

EX namespace stalemate {
  EX bool isKilled(cell *w) {
    for(int f=0; f<isize(moves); f++)
      if(moves[f].isKilled(w)) return true;
    
    return false;
    };
EX }

EX bool krakensafe(cell *c) {
  return items[itOrbFish] || items[itOrbAether] || 
    (c->item == itOrbFish && c->wall == waBoat) || 
    (c->item == itOrbAether && c->wall == waBoat);
  }

EX bool monstersnear(stalemate1& sm) {

  cell *c = sm.moveto;
  bool eaten = false;

  if(hardcore && sm.who == moPlayer) return false;

  int res = 0;
  bool fast = false;

  elec::builder b;
  if(elec::affected(c)) { who_kills_me = moLightningBolt; res++; }
  
  if(c->wall == waArrowTrap && c->wparam == 2) {
    who_kills_me = moArrowTrap; res++;
    }
  
  for(auto c1: crush_now) if(c == c1) {
    who_kills_me = moCrusher; res++;
    }

  if(sm.who == moPlayer || items[itOrbEmpathy]) {
    fast = (items[itOrbSpeed] && (items[itOrbSpeed] & 1));
    if(sm.who == moPlayer && sm.moveto->item == itOrbSpeed && !items[itOrbSpeed]) fast = true;
    }
  
  if(havewhat&HF_OUTLAW) {
    for(cell *c1: gun_targets(c)) 
      if(c1->monst == moOutlaw && !c1->stuntime && !stalemate::isKilled(c1)) {
        res++; who_kills_me = moOutlaw;
        }
    }

  for(int t=0; t<c->type; t++) {
    cell *c2 = c->move(t);

    // consider monsters who attack from distance 2
    if(c2) forCellEx(c3, c2) if(c3 != c) {
      // only these monsters can attack from two spots...
      if(!among(c3->monst, moLancer, moWitchSpeed, moWitchFlash)) 
        continue;
      // take logical_adjacent into account
      if(c3->monst != moWitchFlash)
        if(!logical_adjacent(c3, c3->monst, c2) || !logical_adjacent(c2, c3->monst, c))
          continue;
      if(elec::affected(c3) || stalemate::isKilled(c3)) continue;
      if(c3->stuntime > (sm.who == moPlayer ? 0 : 1)) continue;
      // speedwitches can only attack not-fastened monsters,
      // others can only attack if the move is not fastened
      if(c3->monst == moWitchSpeed && items[itOrbSpeed]) continue;
      if(c3->monst != moWitchSpeed && fast) continue;
      // cannot attack if the immediate cell is impassable (except flashwitches)
      if(c3->monst != moWitchFlash) {
        if(!passable(c2, c3, stalemate::isKilled(c2)?P_MONSTER:0)) continue;
        if(isPlayerOn(c2) && items[itOrbFire]) continue;
        }
      // flashwitches cannot attack if it would kill another enemy
      if(c3->monst == moWitchFlash && flashWouldKill(c3, 0)) continue;
      res++, who_kills_me = c3->monst;
      } 

    // consider normal monsters
    if(c2 && 
      isArmedEnemy(c2, sm.who) && 
      !stalemate::isKilled(c2) &&
      (c2->monst != moLancer || isUnarmed(sm.who) || !logical_adjacent(c, sm.who, c2))) {
      eMonster m = c2->monst;
      if(elec::affected(c2)) continue;
      if(fast && c2->monst != moWitchSpeed) continue;
      // Krakens just destroy boats
      if(c2->monst == moKrakenT && onboat(sm)) {
        if(krakensafe(c)) continue;
        else if(warningprotection(XLAT("This move appears dangerous -- are you sure?")) && res == 0) m = moWarning;
        else continue;
        }
      // they cannot attack through vines
      if(!canAttack(c2, c2->monst, c, sm.who, AF_NEXTTURN)) continue;
      if(c2->monst == moWorm || c2->monst == moTentacle || c2->monst == moHexSnake) {
        if(passable_for(c2->monst, c, c2, 0))
          eaten = true;
        else if(c2->monst != moHexSnake) continue;
        }
      res++, who_kills_me = m;
      }
    }

  if(sm.who == moPlayer && res && (markOrb2(itOrbShield) || markOrb2(itOrbShell)) && !eaten)
    res = 0;

  if(sm.who == moPlayer && res && markOrb2(itOrbDomination) && c->monst)
    res = 0;

  return !!res;
  }

EX bool monstersnear2();

EX bool monstersnear2() {
  multi::cpid++;
  bool b = false;
  bool recorduse[ittypes];
  for(int i=0; i<ittypes; i++) recorduse[i] = orbused[i];
  if(multi::cpid == multi::players || multi::players == 1 || multi::checkonly) {
  
    dynamicval<eMonster> sw(passive_switch, passive_switch);

    // check for safe orbs and switching first
    for(auto &sm: stalemate::moves) if(sm.who == moPlayer) {

      if(hasSafeOrb(sm.moveto)) {
        multi::cpid--; return 0;
        }
      if(sm.moveto->item && itemclass(sm.moveto->item) == IC_TREASURE)
        passive_switch = active_switch();
      if(items[itOrbMagnetism]) forCellEx(c2, sm.moveto)
        if(canPickupItemWithMagnetism(c2, sm.comefrom)) {
          if(itemclass(c2->item) == IC_TREASURE)
            passive_switch = active_switch();
          if(hasSafeOrb(c2)) {
            multi::cpid--;
            return 0;
            }
          }
      }    

    for(int i=0; i<isize(stalemate::moves); i++)
    for(int j=0; j<isize(stalemate::moves); j++) if(i != j) {
      if(swordConflict(stalemate::moves[i], stalemate::moves[j])) {
          b = true;
          who_kills_me = moEnergySword;
          }
      if(multi::player[i].at == multi::player[j].at) 
        { b = true; who_kills_me = moFireball; }
      if(celldistance(multi::player[i].at, multi::player[j].at) > 8) 
        { b = true; who_kills_me = moAirball; }
      }

    for(int i=0; !b && i<isize(stalemate::moves); i++)
      b = monstersnear(stalemate::moves[i]);
    }
  else b = !multimove();
  multi::cpid--;
  for(int i=0; i<ittypes; i++) orbused[i] = recorduse[i];
  return b;
  }

EX bool monstersnear(cell *c, cell *nocount, eMonster who, cell *pushto, cell *comefrom) {

  if(peace::on) return 0; // you are safe

  stalemate1 sm(who, c, nocount, pushto, comefrom);
  
  if(who == moPlayer) for(int b=0; b<2; b++) sm.swordlast[b] = sword::pos(multi::cpid, b);
  
  cell *none = NULL;
  cell **wcw = &cwt.at;
  if(who != moPlayer) wcw = &none;
  else if(multi::players > 1) wcw = &multi::player[multi::cpid].at;
  
  dynamicval<cell*> x5(*wcw, c);
  dynamicval<bool> x6(stalemate::nextturn, true);
  dynamicval<sword::sworddir> x7(sword::dir[multi::cpid], 
    who == moPlayer ? sword::shift(comefrom, c, sword::dir[multi::cpid]) :
    sword::dir[multi::cpid]);
  
  for(int b=0; b<2; b++) {
    if(who == moPlayer) {
      sm.swordnext[b] = sword::pos(multi::cpid, b);
      sm.swordtransit[b] = NULL;
      if(sm.swordnext[b] && sm.swordnext[b] != sm.swordlast[b] && !isNeighbor(sm.swordlast[b], sm.swordnext[b])) {
        forCellEx(c2, sm.swordnext[b])
          if(c2 != c && c2 != comefrom && isNeighbor(c2, S3==3 ? sm.swordlast[b] : *wcw))
            sm.swordtransit[b] = c2;
        if(S3 == 4)
          forCellEx(c2, c)
            if(c2 != comefrom && isNeighbor(c2, sm.swordlast[b]))
              sm.swordtransit[b] = c2;
        }
      }
    else {
      sm.swordnext[b] = sm.swordtransit[b] = NULL;
      }
    }

  stalemate::moves.push_back(sm);
  
  // dynamicval<eMonster> x7(stalemate::who, who);
  
  bool b;
  if(who == moPlayer && c->wall == waBigStatue) {
    eWall w = comefrom->wall;
    c->wall = waNone;
    if(doesnotFall(comefrom)) comefrom->wall = waBigStatue;
    b = monstersnear2();
    comefrom->wall = w;
    c->wall = waBigStatue;
    }
  else if(who == moPlayer && isPushable(c->wall)) {
    eWall w = c->wall;
    c->wall = waNone;
    b = monstersnear2();
    c->wall = w;
    }
  else {
    b = monstersnear2();
    }
  stalemate::moves.pop_back();
  return b;
  }

EX namespace stalemate {
  EX vector<stalemate1> moves;
  EX bool  nextturn;

  EX bool isMoveto(cell *c) {
    for(int i=0; i<isize(moves); i++) if(moves[i].moveto == c) return true;
    return false;
    }

  EX bool isKilledDirectlyAt(cell *c) {
    for(int i=0; i<isize(moves); i++) if(moves[i].killed == c) return true;
    return false;
    }
  
  EX bool isPushto(cell *c) {
    for(int i=0; i<isize(moves); i++) if(moves[i].pushto == c) return true;
    return false;
    }
  EX }

EX bool onboat(stalemate1& sm) {
  cell *c = sm.moveto;
  cell *cf = sm.comefrom;
  return (c->wall == waBoat) || (cf->wall == waBoat && c->wall == waSea);
  }
  
EX bool multimove() {
  if(multi::cpid == 0) lastkills = tkills();
  if(!multi::playerActive(multi::cpid)) return !monstersnear2();
  cellwalker bcwt = cwt;
  cwt = multi::player[multi::cpid];
  bool b = movepcto(multi::whereto[multi::cpid]);
  if(b) {
    multi::aftermove = true;
    multi::player[multi::cpid] = cwt;
    multi::whereto[multi::cpid].d = MD_UNDECIDED;
    int curkills = tkills();
    multi::kills[multi::cpid] += (curkills - lastkills);
    lastkills = curkills;
    }
  cwt = bcwt;
  return b;
  }

EX namespace multi { 
  EX bool checkonly = false;
  EX bool aftermove;
  EX }

EX bool swordConflict(const stalemate1& sm1, const stalemate1& sm2) {
  if(items[itOrbSword] || items[itOrbSword2])
  for(int b=0; b<2; b++)
    if(sm1.comefrom == sm2.swordlast[b] || sm1.comefrom == sm2.swordtransit[b] || sm1.comefrom == sm2.swordnext[b])
    if(sm1.moveto == sm2.swordlast[b] || sm1.moveto == sm2.swordtransit[b] || sm1.moveto == sm2.swordnext[b])
      return true;
  return false;
  }

EX void checkmove() {

  if(dual::state == 2) return;
  if(shmup::on) return;

  dynamicval<eGravity> gs(gravity_state, gravity_state);

#if CAP_INV
  if(inv::on) inv::compute();
#endif

  if(multi::players > 1 && !multi::checkonly) return;
  if(hardcore) return;
  bool orbusedbak[ittypes];
  
  // do not activate orbs!
  for(int i=0; i<ittypes; i++) orbusedbak[i] = orbused[i];

  for(int i=0; i<=MAX_EDGE; i++) legalmoves[i] = false;

  canmove = haveRangedTarget();
  items[itWarning]+=2;
  if(movepcto(-1, 0, true)) canmove = legalmoves[MAX_EDGE] = true;
  
  if(vid.mobilecompasssize || !canmove)
    for(int i=0; i<cwt.at->type; i++) 
      if(movepcto(1, -1, true)) 
        canmove = legalmoves[cwt.spin] = true;
  if(vid.mobilecompasssize || !canmove)
    for(int i=0; i<cwt.at->type; i++) 
      if(movepcto(1, 1, true)) 
        canmove = legalmoves[cwt.spin] = true;
  if(kills[moPlayer]) canmove = false;

#if CAP_INV  
  if(inv::on && !canmove && !inv::incheck) {
    if(inv::remaining[itOrbSafety] || inv::remaining[itOrbFreedom])
      canmove = true;
    else {
      inv::check(1);
      checkmove();
      inv::check(-1);
      }
    if(canmove)
      pushScreen(inv::show);
    }
#endif

  if(!canmove) {
    achievement_final(true);
    if(cmode & sm::NORMAL) showMissionScreen();
    }

  if(canmove && timerstopped) {
    timerstart = time(NULL);
    timerstopped = false;
    }
  items[itWarning]-=2;

  for(int i=0; i<ittypes; i++) orbused[i] = orbusedbak[i];
  if(recallCell.at && !markOrb(itOrbRecall)) activateRecall();  
  }


}