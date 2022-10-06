// Hyperbolic Rogue - basic game routines
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file game.cpp
 *  \brief basic game routines
 */

#include "hyper.h"
namespace hr {

/** \brief the main random number generator for the game.
 *  
 * All the random calls related to the game mechanics (land generation, AI...) should use hrngen.
 *
 * Random calls not related to the game mechanics (graphical effects) should not use hrngen.
 *
 * This ensures that the game should unfold exactly the same if given the same seed and the same input.
 */
EX std::mt19937 hrngen;

/** \brief initialize \link hrngen \endlink */
EX void shrand(int i) {
  hrngen.seed(i);
  }

/** \brief generate a large number with \link hrngen \endlink */
EX int hrandpos() { return hrngen() & HRANDMAX; }

/** \brief A random integer from [0..i), generated from \link hrngen \endlink.
 *
 *  We are using our own implementations rather than ones from <random>,
 *  to make sure that they return the same values on different compilers.
 **/

EX int hrand(int i) { 
  unsigned d = hrngen() - hrngen.min();
  long long m = (long long) (hrngen.max() - hrngen.min()) + 1;
  m /= i;
  d /= m;
  if(d < (unsigned) i) return d;
  return hrand(i);
  }

#if HDR
template<class T, class... U> T pick(T x, U... u) { std::initializer_list<T> i = {x,u...}; return *(i.begin() + hrand(1+sizeof...(u))); }
template<class T> void hrandom_shuffle(T* x, int n) { for(int k=1; k<n; k++) swap(x[k], x[hrand(k+1)]); }
template<class T> void hrandom_shuffle(T& container) { hrandom_shuffle(container.data(), isize(container)); }
template<class U> auto hrand_elt(U& container) -> decltype(container[0]) { return container[hrand(isize(container))]; }
template<class T, class U> T hrand_elt(U& container, T default_value) { 
  if(container.empty()) return default_value; 
  return container[hrand(isize(container))]; 
  }
#endif

EX vector<int> hrandom_permutation(int qty) {
  vector<int> res(qty);
  for(int i=0; i<qty; i++) res[i] = i;
  hrandom_shuffle(res);
  return res;
  }

/** Use \link hrngen \endlink to generate a floating point number between 0 and 1.
 */

EX ld hrandf() { 
  return (hrngen() - hrngen.min()) / (hrngen.max() + 1.0 - hrngen.min());
  }

/** Returns an integer corresponding to the current state of \link hrngen \endlink.
 */
EX int hrandstate() {
  std::mt19937 r2 = hrngen;
  return r2() & HRANDMAX;
  }

EX int lastsafety;

EX bool usedSafety = false;
EX eLand safetyland;
EX int safetyseed;

EX bool childbug = false;

/** Is `w` killed if the part of an ivy `killed` is killed? */
EX bool isChild(cell *w, cell *killed) {
  if(isAnyIvy(w->monst)) {
    int lim = 0;
    // printf("w = %p mondir = %d **\n", w, w->mondir);
    while(w != killed && w->mondir != NODIR) {
      lim++; if(lim == 100000) {
        childbug = true;
        printf("childbug!\n");
        w->item = itBuggy; break; 
        }
      if(!isAnyIvy(w->monst)) { 
        return false;
        }
      w = w->move(w->mondir);
      // printf("w = %p mondir = %d\n", w, w->mondir);
      }
    
    }
  return w == killed;
  }

EX eMonster active_switch() {
  return eMonster(passive_switch ^ moSwitch1 ^ moSwitch2);
  }

EX vector<cell*> crush_now, crush_next;
  
EX int getDistLimit() { return cgi.base_distlimit; }

EX void activateFlashFrom(cell *cf, eMonster who, flagtype flags);

EX bool saved_tortoise_on(cell *c) {
  return 
    (c->monst == moTortoise && c->item == itBabyTortoise &&
    !((tortoise::getb(c) ^ tortoise::babymap[c]) & tortoise::mask));
  }

EX bool normal_gravity_at(cell *c) {
  return !in_gravity_zone(c);
  }

EX int countMyGolems(eMonster m) {
  int g=0, dcs = isize(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->monst == m) g++;
    }
  return g;
  }

EX int savePrincesses() {
  int g=0, dcs = isize(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(isPrincess(c->monst)) princess::save(c);
    }
  return g;
  }

EX int countMyGolemsHP(eMonster m) {
  int g=0, dcs = isize(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->monst == m) g += c->hitpoints;
    }
  return g;
  }

EX void restoreGolems(int qty, eMonster m, int hp IS(0)) {
  int dcs = isize(dcal);
  for(int i=1; qty && i<dcs; i++) {
    cell *c = dcal[i];
    if(m == moTameBomberbird ? 
        (c->mpdist >= 3 && passable(c, NULL, P_FLYING)) : 
        passable(c, NULL, 0)) {
      c->hitpoints = hp / qty;
      c->monst = m, qty--, hp -= c->hitpoints;
      if(m == moPrincess || m == moPrincessArmed)
        princess::newFakeInfo(c);
      }
    }
  }

EX cellwalker recallCell;
EX display_data recallDisplay;

EX bool activateRecall() {
  if(!recallCell.at) {
    addMessage("Error: no recall");
    return false;
    }
  items[itOrbRecall] = 0; items[itOrbSafety] = 0;
  if(!makeEmpty(recallCell.at)) {
    addMessage(XLAT("Your Orb of Recall is blocked by something big!"));
    recallCell.at = NULL;
    return false;
    }

  killFriendlyIvy();
  movecost(cwt.at, recallCell.at, 3);
  playerMoveEffects(movei(cwt.at, recallCell.at, TELEPORT));
  mirror::destroyAll();
  
  sword::reset();

  cwt = recallCell;
  recallCell.at = NULL;
  flipplayer = true;

  centerover = recallDisplay.precise_center;
  View = recallDisplay.view_matrix;
  // local_perspective = recallDisplay.local_perspective;
  gmatrix = recallDisplay.cellmatrices;
  gmatrix0 = recallDisplay.old_cellmatrices;
  current_display->which_copy = recallDisplay.which_copy;

  makeEmpty(cwt.at);
  forCellEx(c2, cwt.at) 
    if(c2->monst != moMutant) 
      c2->stuntime = 4;
  if(shmup::on) shmup::recall();
  if(multi::players > 1) multi::recall();
  bfs();
  checkmove();
  drawSafety();
  addMessage(XLAT("You are recalled!"));
  return true;
  }

EX void saveRecall(cellwalker cw2) {  
  if(!recallCell.at) {
    changes.value_set(recallCell, cw2);
    changes.value_keep(recallDisplay);
    recallDisplay = *current_display;
    }
  }

EX void teleportToLand(eLand l, bool make_it_safe) {
  if(recallCell.at && activateRecall()) 
    return;
  savePrincesses();
  int gg = countMyGolems(moGolem);
  int gb = countMyGolems(moTameBomberbird);
  int gp1 = countMyGolems(moPrincess);
  int gp2 = countMyGolems(moPrincessArmed);
  int gph1 = countMyGolemsHP(moPrincess);
  int gph2 = countMyGolemsHP(moPrincessArmed);
  drawSafety();
  addMessage(XLAT("You fall into a wormhole!"));
  
  eLand f = firstland;
  if(l == laTemple) l = laRlyeh;
  if(l == laClearing) l = laOvergrown;
  if(l == laWhirlpool) l = laOcean;
  if(l == laCrossroads5) l = laCrossroads2; // could not fit!
  if(l == laCamelot && !ls::single())
    l = laCrossroads;
  firstland = l;
  safetyland = l;
  safetyseed = hrandpos();
  clear_euland(firstland);
  safety = make_it_safe; avengers = 0;
  clearMemory();
  initcells();
  initgame();
  firstland = f;
  safety = false;
  restoreGolems(gg, moGolem); 
  restoreGolems(gb, moTameBomberbird); 
  restoreGolems(gp1, moPrincess, gph1); 
  restoreGolems(gp2, moPrincessArmed, gph2); 
  restartGraph();  
  }


EX void activateSafety(eLand l) {
  teleportToLand(l, true);
  #if CAP_SAVE
  if(casual) {
    saveStats();
    savecount++;
    save_turns = turncount;
    }
  #endif
  if(items[itOrbChoice]) {
    items[itOrbChoice] = 0;
    cwt.at->item = itOrbSafety;
    }
  }

EX void placeGolem(cell *on, cell *moveto, eMonster m) {
  if(on->monst == moFriendlyIvy)
    killMonster(on, moPlayer);
  if(on->monst) {
    addMessage(XLAT("There is no room for %the1!", m));
    return;
    }
  if(passable(on, moveto, P_ISFRIEND | (m == moTameBomberbird ? P_FLYING : 0)))
    on->monst = m;
  else {
    on->monst = m;
    flagtype f = AF_CRUSH;
    if(isFire(on))
      addMessage(XLAT("%The1 burns!", m));
    else if(on->wall == waChasm)
      addMessage(XLAT("%The1 falls!", m)), f = AF_FALL;
    else if(isWatery(on) && isNonliving(m))
      addMessage(XLAT("%The1 sinks!", m)), f = AF_FALL;
    else if(isWatery(on))
      addMessage(XLAT("%The1 drowns!", m)), f = AF_FALL;
    else if(isWall(on))
      addMessage(XLAT("%The1 is crushed!", m));
    else if(m == moTameBomberbird && cwt.at->wall == waBoat)
      return;
    else 
      addMessage(XLAT("%The1 is destroyed!", m));
    
    printf("mondir = %d\n", on->mondir);
    fallMonster(cwt.at, f);
    }                 
  }

EX bool multiRevival(cell *on, cell *moveto) {
  int fl = 0;
  if(items[itOrbAether]) fl |= P_AETHER;
  if(items[itCurseWater]) fl |= P_WATERCURSE;
  if(items[itOrbFish]) fl |= P_FISH;
  if(items[itOrbWinter]) fl |= P_WINTER;
  if(passable(on, moveto, fl)) {
    int id = multi::revive_queue[0];
    for(int i=1; i<isize(multi::revive_queue); i++)
      multi::revive_queue[i-1] = multi::revive_queue[i];
    multi::revive_queue.pop_back();
    multi::player[id].at = on;
    multi::player[id].spin = neighborId(moveto, on);
    if(multi::player[id].spin < 0) multi::player[id].spin = 0;
    multi::flipped[id] = true;
    multi::whereto[id].d = MD_UNDECIDED;
    sword::reset();
    return true;
    }
  return false;
  }

EX eMonster passive_switch = moSwitch2;

EX void checkSwitch() {
  passive_switch = (gold() & 1) ? moSwitch1 : moSwitch2;
  }

EX void pushThumper(const movei& mi) {
  auto &cto = mi.t;
  auto &th = mi.s;
  eWall w = th->wall;
  if(th->land == laAlchemist)
    th->wall = isAlch(cwt.at) ? cwt.at->wall : cto->wall;
  else th->wall = waNone;
  int explode = 0;
  if(cto->wall == waArrowTrap && w == waExplosiveBarrel ) explode = max<int>(cto->wparam, 1);
  if(cto->wall == waFireTrap) {
    if(w == waExplosiveBarrel)
      explode = max<int>(cto->wparam, 1);
    if(w == waThumperOn)
      explode = 2;
    }
  destroyTrapsOn(cto);
  if(cto->wall == waOpenPlate || cto->wall == waClosePlate) {
    toggleGates(cto, cto->wall);
    addMessage(XLAT("%The1 destroys %the2!", w, cto->wall));
    }
  if(cellUnstable(cto) && cto->land == laMotion) {
    addMessage(XLAT("%The1 falls!", w));
    doesFallSound(cto);
    }
  else if(cellUnstableOrChasm(cto)) {
    addMessage(XLAT("%The1 fills the hole!", w));
    cto->wall = w == waThumperOn ? waTempFloor : waNone;
    cto->wparam = th->wparam;
    playSound(cto, "click");
    }
  else if(isWatery(cto)) {
    addMessage(XLAT("%The1 fills the hole!", w));
    cto->wall = w == waThumperOn ? waTempBridge : waShallow;
    cto->wparam = th->wparam;
    playSound(cto, "splash"+pick12());
    }
  else if(cto->wall == waShallow) {
    addMessage(XLAT("%The1 fills the hole!", w));
    cto->wall = waNone;
    playSound(cto, "splash"+pick12());
    }
  else if(w == waCrateCrate && cto->wall == waCrateTarget) {
    cto->wall = waCrateOnTarget;
    th->wall = waNone;
    }    
  else if(w == waCrateOnTarget && cto->wall == waNone) {
    cto->wall = waCrateCrate;
    th->wall = waCrateTarget;
    }    
  else if(w == waCrateOnTarget && cto->wall == waCrateTarget) {
    cto->wall = waCrateOnTarget;
    th->wall = waCrateTarget;
    }
  #if CAP_COMPLEX2
  else if(isDie(w)) {
    th->wall = waNone;
    cto->wall = w;    
    dice::roll(mi);
    if(w == waRichDie && dice::data[cto].happy() > 0) {
      cto->wall = waHappyDie;
      if(cto->land == laDice && th->land == laDice) {
        int q = items[itDice];
        gainItem(itDice);
        if(vid.bubbles_all || (threshold_met(items[itDice]) > threshold_met(q) && vid.bubbles_threshold)) {
          drawBubble(cto, iinf[itDice].color, its(items[itDice]), 0.5);
          }
        addMessage(XLAT("The die is now happy, and you are rewarded!"));
        }
      else {
        addMessage(XLAT("The die is now happy, but won't reward you outside of the Dice Reserve!"));
        }
      }          
    if(w == waHappyDie && dice::data[cto].happy() <= 0) {
      cto->monst = moAngryDie;
      cto->wall = waNone;
      cto->stuntime = 5;
      addMessage(XLAT("You have made a Happy Die angry!"));
      animateMovement(mi, LAYER_SMALL);
      }          
    else
      animateMovement(mi, LAYER_BOAT);
    }
  #endif
  else
    cto->wall = w;
  if(explode) cto->wall = waFireTrap, cto->wparam = explode;
  if(cto->wall == waThumperOn)
    cto->wparam = th->wparam;
  }

EX bool canPushThumperOn(movei mi, cell *player) {
  cell *thumper = mi.s;
  cell *tgt = mi.t;
  #if CAP_COMPLEX2
  if(dice::on(thumper) && !dice::can_roll(mi))
    return false;
  #endif
  if(tgt->wall == waBoat || tgt->wall == waStrandedBoat) return false;  
  if(isReptile(tgt->wall)) return false;
  if(isWatery(tgt) && !tgt->monst)
    return true;
  if(tgt->wall == waChasm && !tgt->monst)
    return true;
  return
    passable(tgt, thumper, P_MIRROR) &&
    passable(tgt, player, P_MIRROR) &&
    (!tgt->item || dice::on(thumper));
  }

EX void activateActiv(cell *c, bool msg) {
  if(msg) addMessage(XLAT("You activate %the1.", c->wall));
  if(c->wall == waThumperOff) {
    playSound(c, "click");
    c->wall = waThumperOn;
    }
  if(c->wall == waBonfireOff) {
    playSound(c, "fire");
    c->wall = waFire;
    }
  c->wparam = 100;
  }

/* bool isPsiTarget(cell *dst) {
  return 
    dst->cpdist > 1 &&
    dst->monst && 
    !(isWorm(dst) || dst->monst == moShadow);
  } */                                                      

EX void fixWormBug(cell *c) {
  if(history::includeHistory) return;
  printf("worm bug!\n");
  if(c->monst == moWormtail) c->monst = moWormwait;
  if(c->monst == moTentacletail || c->monst == moTentacleGhost) c->monst = moTentacle;
  if(c->monst == moHexSnakeTail) c->monst = moHexSnake;
  }

EX bool isWormhead(eMonster m) {
  return among(m, 
    moTentacle, moWorm, moHexSnake, moWormwait, moTentacleEscaping,
    moTentaclewait, moDragonHead);   
  }

EX cell *worm_tohead(cell *c) {
 for(int i=0; i<c->type; i++)
   if(c->move(i) && isWorm(c->move(i)->monst) && c->move(i)->mondir == c->c.spin(i))
     return c->move(i);
  return nullptr;
  }

EX cell *wormhead(cell *c) {
  // cell *cor = c;
  int cnt = 0;
  while(cnt < iteration_limit) {
    if(isWormhead(c->monst))
      return c;
    cell *c1 = worm_tohead(c);
    if(!c1) break;
    c = c1;
    cnt++;
    }
  fixWormBug(c);
  return c;
  }  
  
/** \brief currently works for worms only */
EX bool sameMonster(cell *c1, cell *c2) {
  if(!c1 || !c2) return false;
  if(c1 == c2) return true;
  if(isWorm(c1->monst) && isWorm(c2->monst))
    return wormhead(c1) == wormhead(c2);
  if(isKraken(c1->monst) && isKraken(c2->monst))
    return kraken::head(c1) == kraken::head(c2);
  return false;
  }

EX eMonster haveMount() {
  for(cell *pc: player_positions()) {
    eMonster m = pc->monst;
    if(isWorm(m)) return m;
    }
  return moNone;
  }

EX eMonster otherpole(eMonster m) {
  return eMonster(m ^ moNorthPole ^ moSouthPole);
  }
  

}
