// Hyperbolic Rogue - PC movement
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file pcmove.cpp
 *  \brief PC movements
 */

#include "hyper.h"

namespace hr {

EX bool keepLightning = false;

EX bool seenSevenMines = false;

/** \brief have we been warned about the Haunted Woods? */
EX bool hauntedWarning;

/** \brief is the Survivalist achievement still valid? have we received it? */
EX bool survivalist;

EX void fail_survivalist() {
  changes.value_set(survivalist, false);
  }

/** \brief last move was invisible */
EX bool invismove = false;
/** \brief last move was invisible due to Orb of Fish (thus Fish still see you)*/
EX bool invisfish = false;

/** \brief if false, make the PC look in direction cwt.spin (after attack); otherwise, make them look the other direction (after move) */
EX bool flipplayer = true;

/** \brief Cellwalker describing the single player. Also used temporarily in shmup and multiplayer modes. */
EX cellwalker cwt;

EX cell*& singlepos() { return cwt.at; }
EX inline bool singleused() { return !(shmup::on || multi::players > 1); }

/** \brief should we center the screen on the PC? */
EX bool playermoved = true;

/** \brief did the player cheat? how many times? */
EX int  cheater = 0;

/** \brief lands visited -- unblock some modes */
EX bool landvisited[landtypes];

EX int noiseuntil; // noise until the given turn

EX void createNoise(int t) { 
  noiseuntil = max(noiseuntil, turncount+t);
  invismove = false;
  if(shmup::on) shmup::visibleFor(100 * t);
  }

#if HDR
enum eLastmovetype { lmSkip, lmMove, lmAttack, lmPush, lmTree, lmInstant };
extern eLastmovetype lastmovetype, nextmovetype;

enum eForcemovetype { fmSkip, fmMove, fmAttack, fmInstant, fmActivate };
extern eForcemovetype forcedmovetype;
#endif

EX namespace orbbull {
  cell *prev[MAXPLAYER];
  eLastmovetype prevtype[MAXPLAYER];
  int count;

  bool is(cell *c1, cell *c2, cell *c3) {
    int lp = neighborId(c2, c1);
    int ln = neighborId(c2, c3);
    return lp >= 0 && ln >= 0 && anglestraight(c2, lp, ln);
    }
  
  EX void gainBullPowers() {
    items[itOrbShield]++; orbused[itOrbShield] = true;
    items[itOrbThorns]++; orbused[itOrbThorns] = true;
    items[itOrbHorns]++; orbused[itOrbHorns] = true;
    }

  EX void check() {
    int cp = multi::cpid;
    if(cp < 0 || cp >= MAXPLAYER) cp = 0;

    if(!items[itOrbBull]) {
      prev[cp] = NULL;
      return;
      }
    
    bool seq = false;
    
    if(prev[cp] && prevtype[cp] == lmMove && lastmovetype == lmMove)
      seq = is(prev[cp], lastmove, cwt.at);
      
    if(prev[cp] && prevtype[cp] == lmMove && lastmovetype == lmAttack)
      seq = is(prev[cp], cwt.at, lastmove);

    if(prev[cp] && prevtype[cp] == lmAttack && lastmovetype == lmAttack && count)
      seq = lastmove == prev[cp];

    if(prev[cp] && prevtype[cp] == lmAttack && lastmovetype == lmMove && count)
      seq = cwt.at == prev[cp];
    
    prev[cp] = lastmove; prevtype[cp] = lastmovetype;
    
    if(seq) {
      if(lastmovetype == lmMove) count++;
      gainBullPowers();
      }
    else count = 0;
    }
EX }

bool pcmove::checkNeedMove(bool checkonly, bool attacking) {
  if(items[itOrbDomination] > ORBBASE && cwt.at->monst) 
    return false;
  int flags = 0;
  if(cwt.at->monst) {
    if(vmsg(miRESTRICTED)) {
      if(isMountable(cwt.at->monst))
        addMessage(XLAT("You need to dismount %the1!", cwt.at->monst));
      else
        addMessage(XLAT("You need to move to give space to %the1!", cwt.at->monst));
      }
    }
  else if(cwt.at->wall == waRoundTable) {
    if(markOrb2(itOrbAether)) return false;
    if(vmsg(miRESTRICTED)) 
      addMessage(XLAT("It would be impolite to land on the table!"));
    }
  else if(cwt.at->wall == waLake) {
    if(markOrb2(itOrbAether)) return false;
    if(markOrb2(itOrbFish)) return false;
    if(in_gravity_zone(cwt.at) && passable(cwt.at, NULL, P_ISPLAYER)) return false;
    flags |= AF_FALL;
    if(vmsg(miWALL)) addMessage(XLAT("Ice below you is melting! RUN!"));
    }
  else if(!attacking && cellEdgeUnstable(cwt.at)) {
    if(markOrb2(itOrbAether)) return false;
    if(in_gravity_zone(cwt.at) && passable(cwt.at, NULL, P_ISPLAYER)) return false;
    if(vmsg(miRESTRICTED)) addMessage(XLAT("Nothing to stand on here!"));
    return true;
    }
  else if(among(cwt.at->wall, waSea, waCamelotMoat, waLake, waDeepWater)) {
    if(markOrb(itOrbFish)) return false;
    if(markOrb2(itOrbAether)) return false;
    if(in_gravity_zone(cwt.at) && passable(cwt.at, NULL, P_ISPLAYER)) return false;
    if(vmsg(miWALL)) addMessage(XLAT("You have to run away from the water!"));
    }
  else if(cwt.at->wall == waClosedGate) {
    if(markOrb2(itOrbAether)) return false;
    if(vmsg(miWALL)) addMessage(XLAT("The gate is closing right on you! RUN!"));
    }
  else if(isFire(cwt.at) && !markOrb(itOrbWinter) && !markOrb(itCurseWater) && !markOrb2(itOrbShield)) {
    if(markOrb2(itOrbAether)) return false;
    if(vmsg(miWALL)) addMessage(XLAT("This spot will be burning soon! RUN!"));
    }
  else if(cwt.at->wall == waMagma && !markOrb(itOrbWinter) && !markOrb(itCurseWater) && !markOrb2(itOrbShield)) {
    if(markOrb2(itOrbAether)) return false;
    if(in_gravity_zone(cwt.at) && passable(cwt.at, cwt.at, P_ISPLAYER)) return false;
    if(vmsg(miWALL)) addMessage(XLAT("Run away from the lava!"));
    }
  else if(cwt.at->wall == waChasm) {
    if(markOrb2(itOrbAether)) return false;
    if(in_gravity_zone(cwt.at) && passable(cwt.at, cwt.at, P_ISPLAYER)) return false;
    flags |= AF_FALL;
    if(vmsg(miWALL)) addMessage(XLAT("The floor has collapsed! RUN!"));
    }
  else if(items[itOrbAether] > ORBBASE && !passable(cwt.at, NULL, P_ISPLAYER | P_NOAETHER)) {
    if(markOrb2(itOrbAether)) return false;
    vmsg(miWALL);
    return true;
    }
  else if(!passable(cwt.at, NULL, P_ISPLAYER)) {
    if(isFire(cwt.at)) return false; // already checked: have Shield
    if(markOrb2(itOrbAether)) return false;
    if(vmsg(miWALL)) addMessage(XLAT("Your Aether power has expired! RUN!"));
    }
  else return false;
  if(hardcore && !checkonly) 
    killHardcorePlayer(multi::cpid, flags);
  return true;
  }

EX cell *lastmove;
eLastmovetype lastmovetype, nextmovetype;
eForcemovetype forcedmovetype;

#if HDR
struct pcmove {
  bool switchplaces;
  bool checkonly;
  bool errormsgs;
  int origd;
  bool fmsMove, fmsAttack, fmsActivate;
  int d;
  int subdir;
  bool boatmove;
  bool good_tortoise;
  flagtype attackflags;

  bool movepcto();
  bool actual_move();
  bool stay();
  bool after_instant(bool kl);
  
  bool perform_actual_move();
  bool after_move();
  bool perform_move_or_jump();
  bool swing();
  bool boat_move();
  bool after_escape();
  bool move_if_okay();
  bool attack();
  
  bool checkNeedMove(bool checkonly, bool attacking);

  void tell_why_cannot_attack();
  void tell_why_impassable();
  void handle_friendly_ivy();

  movei mi, mip;
  pcmove() : mi(nullptr, nullptr, 0), mip(nullptr, nullptr, 0) {}

  bool vmsg(int code);
  };
#endif

EX cell *global_pushto;

bool pcmove::vmsg(int code) { checked_move_issue = code; changes.rollback(); return errormsgs && !checkonly; }

EX bool movepcto(int d, int subdir IS(1), bool checkonly IS(false)) {
  checked_move_issue = miVALID;
  pcmove pcm;
  pcm.checkonly = checkonly;
  pcm.d = d; pcm.subdir = subdir;
  auto b = pcm.movepcto();
  global_pushto = pcm.mip.t;
  return b;
  }

bool pcmove::movepcto() {  
  reset_spill();
  if(dual::state == 1) return dual::movepc(d, subdir, checkonly);
  if(d >= 0 && !checkonly && subdir != 1 && subdir != -1) printf("subdir = %d\n", subdir);
  mip.t = NULL;
  switchplaces = false;
  warning_shown = false;
  suicidal = false;

  if(d == MD_USE_ORB) 
    return targetRangedOrb(multi::whereto[multi::cpid].tgt, roMultiGo);

  errormsgs = multi::players == 1 || multi::cpid == multi::players-1;
  if(hardcore && !canmove) return false;
  if(!checkonly && d >= 0) {
    flipplayer = false;
    if(multi::players > 1) multi::flipped[multi::cpid] = false;
    }
  DEBBI(checkonly ? 0 : DF_TURN, ("movepc"));
  if(!checkonly) invismove = false;  
  boatmove = false;
  
  if(multi::players > 1)
    lastmountpos[multi::cpid] = cwt.at;
  else
    lastmountpos[0] = cwt.at;
  
  if(againstRose(cwt.at, NULL) && d<0 && !scentResistant()) {
    if(vmsg(miRESTRICTED))
      addMessage("You just cannot stand in place, those roses smell too nicely.");
    return false;
    }

  gravity_state = gsNormal;
  
  fmsMove     = forcedmovetype == fmSkip || forcedmovetype == fmMove;
  fmsAttack   = forcedmovetype == fmSkip || forcedmovetype == fmAttack;
  fmsActivate = forcedmovetype == fmSkip || forcedmovetype == fmActivate;
  
  changes.init(checkonly);
  bool b = (d >= 0) ? actual_move() : stay();
  if(checkonly || !b) {
    changes.rollback();
    if(!checkonly) flipplayer = false;
    }
  else if(changes.on) {
    println(hlog, "error: not commited!");
    changes.commit();
    }

  if(!b) {
    // bool try_instant = (forcedmovetype == fmInstant) || (forcedmovetype == fmSkip && !passable(c2, cwt.at, P_ISPLAYER | P_MIRROR | P_USEBOAT | P_FRIENDSWAP));  

    if(items[itOrbFlash]) {
      if(checkonly) { nextmovetype = lmInstant; return true; }
      if(warning_shown || orbProtection(itOrbFlash)) return true;
      activateFlash();
      checkmove();
      return true;
      }

    if(items[itOrbLightning]) {
      if(checkonly) { nextmovetype = lmInstant; return true; }
      if(warning_shown || orbProtection(itOrbLightning)) return true;
      activateLightning();
      checkmove();
      return true;
      }
          
    if(who_kills_me == moOutlaw && items[itRevolver] && !checkonly) {
      cell *c2 = cwt.cpeek();
      forCellEx(c3, c2) forCellEx(c4, c3) if(c4->monst == moOutlaw) {
        eItem i = targetRangedOrb(c4, roCheck);
        if(i == itRevolver) { 
          targetRangedOrb(c4, roKeyboard);
          return true;
          }
        }
      }
    }

  return b;
  }

bool pcmove::after_move() {
  if(checkonly) return true;
    
  invisfish = false;
  if(items[itOrbFish]) {
     invisfish = true;
     for(cell *pc: player_positions()) 
       if(!isWatery(pc))
         invisfish = false;
     if(d < 0) invisfish = false; // no invisibility if staying still
     if(invisfish) invismove = true, markOrb(itOrbFish);
     }
  
  last_gravity_state = gravity_state;
  if(multi::players == 1) monstersTurn();

  save_memory();
  
  check_total_victory();

  if(items[itWhirlpool] && cwt.at->land != laWhirlpool)
    achievement_gain_once("WHIRL1");

  if(items[itLotus] >= 25 && !isHaunted(cwt.at->land) && survivalist)
    achievement_gain_once("SURVIVAL");

  if(seenSevenMines && cwt.at->land != laMinefield) {
    changes.value_set(seenSevenMines, false);
    achievement_gain("SEVENMINE");
    }

  DEBB(DF_TURN, ("done"));
  return true;
  }

bool pcmove::swing() {
  sideAttack(cwt.at, d, moPlayer, 0);

  mirror::act(origd, mirror::SPINMULTI | mirror::ATTACK);
  
  if(monstersnear_add_pmi(movei(cwt.at, STAY))) {
    if(vmsg(miTHREAT))
      wouldkill("You would be killed by %the1!");          
    return false;
    }
  if(checkonly) return true;
  if(changes.on) changes.commit();

  animateAttack(mi, LAYER_SMALL);
  if(survivalist && isHaunted(mi.t->land))
    survivalist = false;
  lastmovetype = lmTree; lastmove = mi.t;
  swordAttackStatic();

  return after_move();
  }

bool pcmove::after_instant(bool kl) {
  changes.commit();
  keepLightning = kl;
  bfs();
  keepLightning = false;
  if(multi::players > 1) { multi::whereto[multi::cpid].d = MD_UNDECIDED; return false; }
  checkmove();
  return true;
  }

EX void copy_metadata(cell *x, const gcell *y) {
  x->wall = y->wall;
  x->monst = y->monst;
  x->item = y->item;
  x->mondir = y->mondir;
  x->stuntime = y->stuntime;
  x->hitpoints = y->hitpoints;
  x->monmirror = y->monmirror;
  x->LHU = y->LHU;
  if(isIcyLand(x)) {
    x->landparam = y->landparam;
    }
  x->wparam = y->wparam;
  }

#if HDR

extern void playSound(cell *c, const string& fname, int vol);

/** \brief A structure to keep track of changes made during the player movement.
 *
 *  This is a singleton object, \link hr::changes \endlink.
 */

struct changes_t {
  vector<reaction_t> rollbacks;
  vector<reaction_t> commits;
  bool on;
  bool checking;
  
  /**
   * \brief Start keeping track of changes, perform changes.
   *
   * init(false) if you intend to commit the changes (if successful), or 
   * init(true) if you just want to check whether the move would be successful, 
   * without performing it if it is.
   */
   
  void init(bool ch) {
    on = true; 
    ccell(cwt.at);
    forCellEx(c1, cwt.at) ccell(c1);
    value_keep(kills);
    value_keep(items);
    value_keep(orbused);
    value_keep(hrngen);
    checking = ch;
    }

  /** \brief Commit the changes. Should only be called after init(false). */

  void commit() { 
    on = false; 
    for(auto& p: commits) p();
    rollbacks.clear();
    commits.clear();
    }

  /** \brief Rollback the changes. */

  void rollback(int pos = 0) { 
    on = false;
    while(!rollbacks.empty()) {
      rollbacks.back()();
      rollbacks.pop_back();
      }
    rollbacks.clear();
    commits.clear();
    }
  
  /** \brief The changes to cell c will be rolled back when rollback() is called. */
  void ccell(cell *c) {
    if(!on) return;
    gcell a = *c;
    rollbacks.push_back([c, a] { copy_metadata(c, &a); });
    }
  
  /** \brief Set the value of what to value. This change will be rolled back if necessary. */
  template<class T> void value_set(T& what, T value) {
    if(!on) { what = value; return; }
    if(what == value) return;
    T old = what;
    rollbacks.push_back([&what, old] { what = old; });
    what = value;
    }

  /** \brief Add step to the value of what. This change will be rolled back if necessary. */

  template<class T> void value_add(T& what, T step) {
    value_keep(what); what += step;
    }

  template<class T> void value_inc(T& what) { value_add(what, 1); }

  /** \brief Any change to the value of what will be rolled back if necessary. */

  template<class T> void value_keep(T& what) {
    if(!on) return;
    T old = what;
    rollbacks.push_back([&what, old] { what = old; });
    }
  
  /** \brief Like value_keep but for maps. */

  template<class T, class U, class V> void map_value(map<T, U>& vmap, V& key) {
    if(vmap.count(key)) {
      auto val = vmap[key];
      at_rollback([&vmap, key, val] { vmap[key] = val; });
      }
    else {
      at_rollback([&vmap, key] { vmap.erase(key); });
      }
    }
  
  /** \brief Perform the given action on commit. @see LATE */
   
  void at_commit(reaction_t act) {
    if(!on) act();
    else commits.emplace_back(act);
    }

  /** \brief Perform the given action on rollback. */

  void at_rollback(reaction_t act) {
    if(on) rollbacks.emplace_back(act);
    }

  void push_push(cell *tgt) {
    pushes.push_back(tgt);
    auto v = [] { pushes.pop_back(); };
    rollbacks.push_back(v);
    commits.push_back(v);
    }
  };
#endif

/** \brief The only instance of hr::changes_t */
EX changes_t changes;

/**
 * Auxiliary function for hr::apply_chaos(). Returns whether the cell attribute LHU 
 * should be switched.
 */
bool switch_lhu_in(eLand l) {
  return among(l, laBrownian, laMinefield, laTerracotta, laHive);
  }

/** \brief how should be the direction from 'src' be mirrored to 'dst' */
EX int chaos_mirror_dir(int dir, cellwalker src, cellwalker dst) {
  if(dir >= dst.at->type) return dir;
  return (dst-src.to_spin(dir)).spin;
  }

#if HDR
template<class T> void swap_data(T& data, cell *c1, cell *c2) {
  changes.map_value(data, c1);
  changes.map_value(data, c2);
  if(data.count(c1) && data.count(c2))
    swap(data[c1], data[c2]);
  else if(data.count(c1))
    data[c2] = data[c1], data.erase(c1);
  else if(data.count(c2))
    data[c1] = data[c2], data.erase(c2);
  }
#endif

/** \brief Apply the Orb of Chaos.
 *
 * We assume that the player moves from cwt.peek, in
 * in the direction given by cwt.spin.
 */
void apply_chaos() {
  auto wa = cwt+1+wstep;
  auto wb = cwt-1+wstep;
  cell *ca = wa.at;
  cell *cb = wb.at;
  if(dice::swap_forbidden(ca, cb)) return;
  if(dice::swap_forbidden(cb, ca)) return;
  if(!items[itOrbChaos] || chaos_forbidden(ca) || chaos_forbidden(cb)) return;
  if(ca && is_paired(ca->monst)) killMonster(ca, moPlayer);
  if(cb && is_paired(cb->monst)) killMonster(cb, moPlayer);
  destroyTrapsOn(ca);
  destroyTrapsOn(cb);
  if (ca->wall == waStone) destroyTrapsAround(ca);
  if (cb->wall == waStone) destroyTrapsAround(cb);
  changes.ccell(ca);
  changes.ccell(cb);
  /* needs to be called separately for Shadows */
  if(ca->monst == moShadow) checkStunKill(ca);
  if(cb->monst == moShadow) checkStunKill(cb);
  gcell coa = *ca;
  gcell cob = *cb;
  if(ca->monst != cb->monst)
    markOrb(itOrbChaos);
  if(ca->wall != cb->wall)
    markOrb(itOrbChaos);
  if(ca->item != cb->item)
    markOrb(itOrbChaos);
  copy_metadata(ca, &cob);
  copy_metadata(cb, &coa);
  if(!switch_lhu_in(ca->land)) ca->LHU = coa.LHU;
  if(!switch_lhu_in(cb->land)) cb->LHU = cob.LHU;
  if(ca->monst && !(isFriendly(ca) && markOrb(itOrbEmpathy))) {
    ca->stuntime = min(ca->stuntime + 3, 15), markOrb(itOrbChaos);
    checkStunKill(ca);
    }
  if(cb->monst && !(isFriendly(cb) && markOrb(itOrbEmpathy))) {
    cb->stuntime = min(cb->stuntime + 3, 15), markOrb(itOrbChaos);
    checkStunKill(cb);
    }
  ca->monmirror = !ca->monmirror;
  cb->monmirror = !cb->monmirror;
  ca->mondir = chaos_mirror_dir(ca->mondir, wb, wa);
  cb->mondir = chaos_mirror_dir(cb->mondir, wa, wb);
  if(isPrincess(ca) && !isPrincess(cb))
    princess::move(movei{cb, ca, JUMP});
  if(isPrincess(cb) && !isPrincess(ca))
    princess::move(movei{ca, cb, JUMP});
  if(ca->monst == moTortoise || cb->monst == moTortoise) {
    tortoise::move_adult(ca, cb);
    }
  if(dice::on(ca) || dice::on(cb)) {
    dice::chaos_swap(wa, wb);
    }
  if(ca->item == itBabyTortoise || cb->item == itBabyTortoise) {
    tortoise::move_baby(ca, cb);
    }
  }
  
bool pcmove::actual_move() {

  origd = d;
  if(d >= 0) {
    cwt += d;
    dynamicval<bool> b(changes.on, false);
    mirror::act(d, mirror::SPINSINGLE);
    d = cwt.spin;
    }
  if(d != -1 && !checkonly) playermoved = true;

  mi = movei(cwt.at, d);
  cell *& c2 = mi.t;
  good_tortoise = c2->monst == moTortoise && tortoise::seek() && !tortoise::diff(tortoise::getb(c2)) && !c2->item;
  
  if(items[itOrbGravity]) {
    if(c2->monst && !should_switchplace(cwt.at, c2))
      gravity_state = get_static_gravity(cwt.at);
    else
      gravity_state = get_move_gravity(cwt.at, c2);
    if(gravity_state) markOrb(itOrbGravity);
    }

  if(againstRose(cwt.at, c2) && !scentResistant()) {
    if(vmsg(miRESTRICTED)) addMessage("Those roses smell too nicely. You have to come towards them.");
    return false;
    }
  
  if(items[itOrbDomination] > ORBBASE && isMountable(c2->monst) && fmsMove) {
    if(checkonly) { nextmovetype = lmMove; return true; }
    if(!isMountable(cwt.at->monst)) dragon::target = NULL;
    movecost(cwt.at, c2, 3);
    
    flipplayer = true; if(multi::players > 1) multi::flipped[multi::cpid] = true;
    invismove = (turncount >= noiseuntil) && items[itOrbInvis] > 0;
    killFriendlyIvy();
    return perform_move_or_jump();
    }
  
  if(isActivable(c2) && fmsActivate) {
    if(checkonly) { nextmovetype = lmInstant; return true; }
    activateActiv(c2, true);
    return after_instant(false);
    }
  
  #if CAP_COMPLEX2
  if(c2->monst == moAnimatedDie) {
    mip = determinePush(cwt, subdir, [] (movei mi) { return canPushThumperOn(mi, cwt.at); });
    if(mip.proper()) {
      auto tgt = roll_effect(mip, dice::data[c2]);
      if(tgt.happy() > 0) {
        changes.ccell(c2);
        c2->monst = moNone;
        c2->wall = waRichDie;
        }
      }
    }
  #endif

  if(isPushable(c2->wall) && !c2->monst && !nonAdjacentPlayer(c2, cwt.at) && fmsMove) {
    mip = determinePush(cwt, subdir, [] (movei mi) { return canPushThumperOn(mi, cwt.at); });
    if(mip.t) changes.ccell(mip.t);
    if(mip.d == NO_SPACE) {
      if(vmsg(miWALL)) addMessage(XLAT("No room to push %the1.", c2->wall));
      return false;
      }
    nextmovetype = lmMove;
    addMessage(XLAT("You push %the1.", c2->wall));
    lastmovetype = lmPush; lastmove = cwt.at;
    pushThumper(mip);
    changes.push_push(mip.t);
    return perform_actual_move();
    }

  if(c2->item == itHolyGrail && roundTableRadius(c2) < newRoundTableRadius()) {
    if(vmsg(miRESTRICTED)) addMessage(XLAT("That was not a challenge. Find a larger castle!"));
    return false;
    }

  if(c2->item == itOrbYendor && !checkonly && !peace::on && !itemHiddenFromSight(c2) && yendor::check(c2)) {
    return false;
    }

  if(isWatery(c2) && !nonAdjacentPlayer(cwt.at,c2) && !c2->monst && cwt.at->wall == waBoat && fmsMove) 
    return boat_move();  
  
  if(!c2->monst && cwt.at->wall == waBoat && cwt.at->item != itOrbYendor && boatGoesThrough(c2) && markOrb(itOrbWater) && !nonAdjacentPlayer(c2, cwt.at) && fmsMove) {

    if(c2->item && !cwt.at->item) moveItem(c2, cwt.at, false), boatmove = true;
    placeWater(c2, cwt.at);
    moveBoat(mi);
    changes.ccell(c2);
    c2->mondir = revhint(cwt.at, d);
    if(c2->item) boatmove = !boatmove;
    return perform_actual_move();
    }
  
  return after_escape();
  }

void blowaway_message(cell *c2) {
  addMessage(airdist(c2) < 3 ? XLAT("The Air Elemental blows you away!") : XLAT("You cannot go against the wind!"));
  }

EX void tortoise_hero_message(cell *c2) {
  bool fem = playergender() == GEN_F;      
  playSound(c2, fem ? "heal-princess" : "heal-prince");
  addMessage(fem ? XLAT("You are now a tortoise heroine!") : XLAT("You are now a tortoise hero!"));
  }

bool pcmove::boat_move() {

  cell *& c2 = mi.t;

  if(againstWind(c2, cwt.at)) {
    if(vmsg(miRESTRICTED)) blowaway_message(c2);
    return false;
    }

  if(againstCurrent(c2, cwt.at) && !markOrb(itOrbWater)) {
    if(markOrb(itOrbFish) || markOrb(itOrbAether) || gravity_state)
      return after_escape();
    if(vmsg(miRESTRICTED)) addMessage(XLAT("You cannot go against the current!"));
    return false;
    }

  if(cwt.at->item == itOrbYendor) {        
    if(markOrb(itOrbFish) || markOrb(itOrbAether) || gravity_state) 
      return after_escape();
    if(vmsg(miRESTRICTED)) addMessage(XLAT("The Orb of Yendor is locked in with powerful magic."));
    return false;
    }

  nextmovetype = lmMove;
  moveBoat(mi);
  boatmove = true;
  return perform_actual_move();
  }

void pcmove::tell_why_cannot_attack() {
  cell *& c2 = mi.t;
  if(c2->monst == moWorm || c2->monst == moWormtail || c2->monst == moWormwait) 
    addMessage(XLAT("You cannot attack Sandworms directly!"));
  else if(c2->monst == moHexSnake || c2->monst == moHexSnakeTail)
    addMessage(XLAT("You cannot attack Rock Snakes directly!"));
  else if(nonAdjacentPlayer(c2, cwt.at))
    addMessage(XLAT("You cannot attack diagonally!"));
  else if(thruVine(c2, cwt.at))
    addMessage(XLAT("You cannot attack through the Vine!"));
  else if(c2->monst == moTentacle || c2->monst == moTentacletail || c2->monst == moTentaclewait || c2->monst == moTentacleEscaping)
    addMessage(XLAT("You cannot attack Tentacles directly!"));
  else if(c2->monst == moHedge && !markOrb(itOrbThorns)) {
    addMessage(XLAT("You cannot attack %the1 directly!", c2->monst));
    addMessage(XLAT("Stab them by walking around them."));
    }
  else if(c2->monst == moRoseBeauty || isBull(c2->monst) || c2->monst == moButterfly) 
    addMessage(XLAT("You cannot attack %the1!", c2->monst));
  else if(c2->monst == moFlailer && !c2->stuntime) {
    addMessage(XLAT("You cannot attack %the1 directly!", c2->monst));
    addMessage(XLAT("Make him hit himself by walking away from him."));
    }
  else if(c2->monst == moShadow)
    addMessage(XLAT("You cannot defeat the Shadow!"));
  else if(c2->monst == moGreater || c2->monst == moGreaterM)
    addMessage(XLAT("You cannot defeat the Greater Demon yet!"));
  else if(c2->monst == moDraugr)
    addMessage(XLAT("Your mundane weapon cannot hurt %the1!", c2->monst));
  else if(isRaider(c2->monst))
    addMessage(XLAT("You cannot attack Raiders directly!"));
  else if(isSwitch(c2->monst))
    addMessage(XLAT("You cannot attack Jellies in their wall form!"));
  else if(c2->monst == moAnimatedDie)
    addMessage(XLAT("You can only push this die if the highest number would be on the top!"));
  else if(c2->monst == moAngryDie)
    addMessage(XLAT("This die is really angry at you!"));
  else if((attackflags & AF_WEAK) && isIvy(c2))
    addMessage(XLAT("You are too weakened to attack %the1!", c2->monst));
  else if(isWorm(cwt.at->monst) && isWorm(c2->monst) && wormhead(cwt.at) == wormhead(c2) && cwt.at->monst != moTentacleGhost && c2->monst != moTentacleGhost)
    addMessage(XLAT("You cannot attack your own mount!"));
  else if(checkOrb(c2->monst, itOrbShield))
    addMessage(XLAT("A magical shield protects %the1!", c2->monst));
  else
    addMessage(XLAT("For some reason... cannot attack!"));
  }

bool pcmove::after_escape() {
  cell*& c2 = mi.t;
  
  bool push_behind = c2->wall == waBigStatue || (among(c2->wall, waCTree, waSmallTree, waBigTree, waShrub, waVinePlant) && !c2->monst && markOrb(itOrbWoods));
  
  if(thruVine(c2, cwt.at) && markOrb(itOrbWoods)) push_behind = true;
  
  if(push_behind && !c2->monst && !nonAdjacentPlayer(c2, cwt.at) && fmsMove) {
    eWall what = c2->wall;
    if(!thruVine(c2, cwt.at) && !canPushStatueOn(cwt.at, P_ISPLAYER)) {
      if(vmsg(miRESTRICTED)) { 
        if(isFire(cwt.at))
          addMessage(XLAT("You have to escape first!"));
        else
          addMessage(XLAT("There is not enough space!"));
        }
      return false;
      }
    
    changes.ccell(c2);
    changes.ccell(cwt.at);
    
    c2->wall = cwt.at->wall;
    c2->wparam = cwt.at->wparam;
    if(doesnotFall(cwt.at)) {
      cwt.at->wall = what;
      if(cellHalfvine(what)) 
        c2->wall = waNone, cwt.at->wall = waVinePlant;
      }
      
    nextmovetype = lmMove;
    addMessage(XLAT("You push %the1 behind you!", what));
    animateMovement(mi.rev(), LAYER_BOAT);
    changes.push_push(cwt.at);
    return perform_actual_move();
    }

  bool attackable;
  attackable = 
    c2->wall == waBigTree ||
    c2->wall == waSmallTree ||
    (c2->wall == waShrub && items[itOrbSlaying]) ||
    c2->wall == waMirrorWall;
  if(attackable && markOrb(itOrbAether) && c2->wall != waMirrorWall)
    attackable = false;
  bool nm; nm = attackable;
  if(forcedmovetype == fmAttack) attackable = true;
  attackable = attackable && (!c2->monst || isFriendly(c2));
  attackable = attackable && !nonAdjacentPlayer(cwt.at,c2);
  
  bool dont_attack = items[itOrbFlash] || items[itOrbLightning];
  
  if(attackable && fmsAttack && !dont_attack && !items[itCurseWeakness]) {
    if(checkNeedMove(checkonly, true)) return false;
    nextmovetype = nm ? lmAttack : lmSkip;
    if(c2->wall == waSmallTree || (c2->wall == waBigTree && markOrb(itOrbSlaying))) {
      drawParticles(c2, winf[c2->wall].color, 4);
      addMessage(XLAT("You chop down the tree."));
      playSound(c2, "hit-axe" + pick123());
      changes.ccell(c2);
      c2->wall = waNone;
      spread_plague(cwt.at, c2, mi.d, moPlayer);
      return swing();
      }
    else if(c2->wall == waShrub && markOrb(itOrbSlaying)) {
      drawParticles(c2, winf[c2->wall].color, 4);
      addMessage(XLAT("You chop down the shrub."));
      playSound(c2, "hit-axe" + pick123());
      changes.ccell(c2);
      c2->wall = waNone;
      spread_plague(cwt.at, c2, mi.d, moPlayer);
      return swing();
      }
    else if(c2->wall == waBigTree) {
      drawParticles(c2, winf[c2->wall].color, 8);
      addMessage(XLAT("You start chopping down the tree."));
      playSound(c2, "hit-axe" + pick123());
      changes.ccell(c2);
      c2->wall = waSmallTree;
      return swing();
      }
    if(!peace::on) {
      if(c2->wall == waMirrorWall)
        addMessage(XLAT("You swing your sword at the mirror."));
      else if(c2->wall)
        addMessage(XLAT("You swing your sword at %the1.", c2->wall));
      else
        addMessage(XLAT("You swing your sword."));
      return swing();
      }
    return false;
    }
  else if(c2->monst == moKnight) {
    #if CAP_COMPLEX2
    if(vmsg(miWALL)) camelot::knightFlavorMessage(c2);
    #endif
    return false;
    }
  else if(c2->monst && (!isFriendly(c2) || c2->monst == moTameBomberbird || isMountable(c2->monst)) && !(peace::on && !good_tortoise))
    return attack();
  else if(!passable(c2, cwt.at, P_USEBOAT | P_ISPLAYER | P_MIRROR | P_MONSTER)) {
    tell_why_impassable();
    return false;
    }
  else if(markOrb(itCurseFatigue) && items[itFatigue] + fatigue_cost(mi) > 10) {
    if(vmsg(miRESTRICTED)) 
      addMessage(XLAT("You are too fatigued!"));
    return false;
    }
  else if(fmsMove) 
    return move_if_okay();
  
  else return false;
  }

bool pcmove::move_if_okay() {
  cell*& c2 = mi.t;
  #if CAP_COMPLEX2
  if(mine::marked_mine(c2) && !mine::safe() && !checkonly && warningprotection(XLAT("Are you sure you want to step there?")))
    return false;
  #endif
  
  if(snakelevel(c2) <= snakelevel(cwt.at)-2) {
    bool can_leave = false;
    forCellEx(c3, c2) if(passable(c3, c2, P_ISPLAYER | P_MONSTER)) can_leave = true;
    if(!can_leave && !checkonly && warningprotection(XLAT("Are you sure you want to step there?")))
      return false;
    }

  if(switchplace_prevent(cwt.at, c2, *this))
    return false;
  if(!checkonly && warningprotection_hit(do_we_stab_a_friend(mi, moPlayer)))
    return false;
  
  nextmovetype = lmMove; 
  return perform_actual_move();
  }

void pcmove::tell_why_impassable() {
  cell*& c2 = mi.t;
  if(nonAdjacentPlayer(cwt.at,c2)) {
    if(vmsg(miRESTRICTED)) addMessage(geosupport_football() < 2 ?
      XLAT("You cannot move between the cells without dots here!") :
      XLAT("You cannot move between the triangular cells here!")
      );
    }
  else if(againstWind(c2, cwt.at)) {
    if(vmsg(miRESTRICTED))
      blowaway_message(c2);
    }
  else if(anti_alchemy(c2, cwt.at)) {
    if(vmsg(miRESTRICTED))
      addMessage(XLAT("Wrong color!"));
    }
  else if(c2->wall == waRoundTable) {
    if(vmsg(miRESTRICTED))
      addMessage(XLAT("It would be impolite to land on the table!"));
    }
  else if(snakelevel(cwt.at) >= 3 && snakelevel(c2) == 0 && !isWall(c2)) {
    if(vmsg(miRESTRICTED))
      addMessage(XLAT("You would get hurt!", c2->wall));
    }
  else if(cellEdgeUnstable(cwt.at) && cellEdgeUnstable(c2)) {
    if(vmsg(miRESTRICTED))
      addMessage(XLAT("Gravity does not allow this!"));
    }
  else if(c2->wall == waChasm && c2->land == laDual) {
    if(vmsg(miRESTRICTED))
      addMessage(XLAT("You cannot move there!"));
    }
  else if(!c2->wall) {
    if(vmsg(miRESTRICTED))
      addMessage(XLAT("You cannot move there!"));
    }
  else {
    if(vmsg(miWALL))
      addMessage(XLAT("You cannot move through %the1!", c2->wall));
    }
  }

bool pcmove::attack() {
  auto& c2 = mi.t;
  if(!fmsAttack) return false;

  if(items[itOrbFlash] || items[itOrbLightning])
    return false;
  
  attackflags = AF_NORMAL;
  if(items[itOrbSpeed]&1) attackflags |= AF_FAST;
  if(items[itOrbSlaying]) attackflags |= AF_CRUSH;
  if(items[itCurseWeakness]) attackflags |= AF_WEAK;
  
  bool ca =canAttack(cwt.at, moPlayer, c2, c2->monst, attackflags);
  
  if(!ca) {
    if(forcedmovetype == fmAttack) {
      if(monstersnear_add_pmi(movei(cwt.at, STAY))) {
        if(vmsg(miTHREAT)) wouldkill("%The1 would get you!");
        return false;
        }
      nextmovetype = lmSkip;
      addMessage(XLAT("You swing your sword at %the1.", c2->monst));
      return swing();
      }
    if(vmsg(miENTITY)) tell_why_cannot_attack();
    return false;
    }
    
  // mip.t=c2 means that the monster is not destroyed and thus
  // still counts for lightning in monstersnear

  mip = movei(c2, nullptr, NO_SPACE);
  
  if(items[itCurseWeakness] || (isStunnable(c2->monst) && c2->hitpoints > 1)) {
    if(monsterPushable(c2))
      mip = determinePush(cwt, subdir, [] (movei mi) { return passable(mi.t, mi.s, P_BLOW); });
    else
      mip.t = c2;
    if(mip.t) changes.ccell(mip.t);
    changes.push_push(mip.t);
    }
  
  if(!(isWatery(cwt.at) && c2->monst == moWaterElemental) && checkNeedMove(checkonly, true))
    return false;
  
  if(c2->monst == moTameBomberbird && warningprotection_hit(moTameBomberbird)) return false;
  
  nextmovetype = lmAttack;

  mirror::act(origd, mirror::SPINMULTI | mirror::ATTACK);
  
  int tk = tkills();
  plague_kills = 0;

  if(good_tortoise) {
    changes.ccell(c2);
    c2->stuntime = 2;
    changes.at_commit([c2] {
      items[itBabyTortoise] += 4;
      updateHi(itBabyTortoise, items[itBabyTortoise]);
      c2->item = itBabyTortoise;
      tortoise::babymap[c2] = tortoise::seekbits;
      tortoise_hero_message(c2);
      achievement_collection(itBabyTortoise);
      });
    }
  else {
    eMonster m = c2->monst;
    if(m) {
      if((attackflags & AF_CRUSH) && !canAttack(cwt.at, moPlayer, c2, c2->monst, attackflags ^ AF_CRUSH ^ AF_MUSTKILL))
        markOrb(itOrbSlaying);
      if(c2->monst == moTerraWarrior && hrand(100) > 2 * items[itTerra]) {
        if(hrand(2 + jiangshi_on_screen) < 2)
          changes.value_add(wandering_jiangshi, 1);
        }
      attackMonster(c2, attackflags | AF_MSG, moPlayer);
      if(m == moRusalka) {
        changes.ccell(cwt.at);
        if(cwt.at->wall == waNone) cwt.at->wall = waShallow;
        else if(cwt.at->wall == waShallow || isAlch(cwt.at->wall)) cwt.at->wall = waDeepWater;
        }
      changes.ccell(c2);
      // salamanders are stunned for longer time when pushed into a wall
      if(c2->monst == moSalamander && (mip.t == c2 || !mip.t)) c2->stuntime = 10;
      if(!c2->monst || isAnyIvy(m)) {
        spread_plague(cwt.at, c2, mi.d, moPlayer);
        produceGhost(c2, m, moPlayer);
        }
      if(mip.proper()) pushMonster(mip);
      animateAttack(mi, LAYER_SMALL);
      }
    }
  
  sideAttack(cwt.at, d, moPlayer, tkills() - tk - plague_kills);
  lastmovetype = lmAttack; lastmove = c2;
  swordAttackStatic();

  if(monstersnear_add_pmi(movei(cwt.at, STAY))) {
    if(vmsg(miTHREAT)) wouldkill("You would be killed by %the1!");
    return false;
    }
  if(checkonly) return true;
  if(changes.on) changes.commit();

  return after_move();
  }

EX bool chaos_forbidden(cell *c) {
  return do_not_touch_this_wall(c) || isMultitile(c->monst);
  }

EX int fatigue_cost(const movei& mi) {
  return
    gravityLevelDiff(mi.t, mi.s) +
    (snakelevel(mi.t) - snakelevel(mi.s)) +
    (againstWind(mi.s, mi.t) ? 0 : 1);
  }

bool alchMayDuplicate(eWall w) {
  return !isDie(w) && w != waBoat && w != waArrowTrap;
}

bool pcmove::perform_actual_move() {
  cell*& c2 = mi.t;
  changes.at_commit([&] {
    flipplayer = true; if(multi::players > 1) multi::flipped[multi::cpid] = true;
    });
  if(c2->item && isAlch(c2)) {
    if(alchMayDuplicate(cwt.at->wall)) {
      c2->wall = conditional_flip_slime(mi.mirror(), cwt.at->wall);
      c2->wparam = cwt.at->wparam;
      }
    else
      c2->wall = waNone;
    }
  #if CAP_COMPLEX2
  if(c2->wall == waRoundTable) {
    addMessage(XLAT("You jump over the table!"));
    }
  
  if(cwt.at->wall == waRoundTable) 
    camelot::roundTableMessage(c2);
  #endif
  
  invismove = (turncount >= noiseuntil) && items[itOrbInvis] > 0;
  
  if(items[itOrbFire]) {
    invismove = false;
    if(makeflame(cwt.at, 10, false)) markOrb(itOrbFire);
    }

  if(items[itCurseWater]) {
    invismove = false;
    if(makeshallow(mi.s, 10, false)) markOrb(itCurseWater);
    }
  
  if(markOrb(itCurseFatigue) && !markOrb(itOrbAether))
    items[itFatigue] += fatigue_cost(mi);

  handle_friendly_ivy();  
  
  if(items[itOrbDigging]) {
    invismove = false;
    if(earthMove(mi)) markOrb(itOrbDigging);
    }

  movecost(cwt.at, c2, 1);

  if(!boatmove && collectItem(c2, cwt.at)) return true;
  if(boatmove && c2->item && cwt.at->item) {
     eItem it = c2->item;
     c2->item = cwt.at->item;
     if(collectItem(c2, cwt.at)) return true;
     eItem it2 = c2->item;
     c2->item = it;
     cwt.at->item = it2;
     }
  if(doPickupItemsWithMagnetism(c2)) return true;

  if(isIcyLand(cwt.at) && cwt.at->wall == waNone && markOrb(itOrbWinter)) {
    invismove = false;
    cwt.at->wall = waIcewall;
    }
  
  if(items[itOrbWinter])
    forCellEx(c3, c2) if(c3->wall == waIcewall && c3->item) {
      changes.ccell(c3);
      markOrb(itOrbWinter);
      if(collectItem(c3, cwt.at)) return true;
      }
  
  movecost(cwt.at, c2, 2);

  handle_switchplaces(cwt.at, c2, switchplaces);
  
  return perform_move_or_jump();
  }

void pcmove::handle_friendly_ivy() {
  cell*& c2 = mi.t;
  bool haveIvy = false;
  forCellEx(c3, cwt.at) if(c3->monst == moFriendlyIvy) haveIvy = true;
    
  bool killIvy = haveIvy;
    
  if(items[itOrbNature]) {
    if(c2->monst != moFriendlyIvy && strictlyAgainstGravity(c2, cwt.at, false, MF_IVY)) {
      invismove = false;
      }
    else if(cwt.at->monst) invismove = false;
    else if(haveIvy || !cellEdgeUnstable(cwt.at, MF_IVY)) {
      cwt.at->monst  = moFriendlyIvy;
      cwt.at->mondir = neighborId(cwt.at, c2);
      invismove = false;
      markOrb(itOrbNature);
      killIvy = false;
      }
    }
  
  if(killIvy) killFriendlyIvy();
  }

bool pcmove::perform_move_or_jump() {
  lastmovetype = lmMove; lastmove = cwt.at;
  apply_chaos();

  stabbingAttack(mi, moPlayer);
  changes.value_keep(cwt);
  cwt += wstep;
  
  mirror::act(origd, mirror::SPINMULTI | mirror::ATTACK | mirror::GO);
  
  auto pmi = player_move_info(mi);
  playerMoveEffects(mi);

  if(mi.t->monst == moFriendlyIvy) changes.ccell(mi.t), mi.t->monst = moNone;
  
  if(monstersnear_add_pmi(pmi)) {
    if(vmsg(miTHREAT)) wouldkill("%The1 would kill you there!");
    return false;
    }
  
  if(checkonly) return true;
  if(changes.on) changes.commit();

  if(switchplaces) {
    indAnimateMovement(mi, LAYER_SMALL);
    indAnimateMovement(mi.rev(), LAYER_SMALL);
    commitAnimations(LAYER_SMALL);
    }
  else
    animateMovement(mi, LAYER_SMALL);
  current_display->which_copy = current_display->which_copy * adj(mi);

  countLocalTreasure();
  landvisited[cwt.at->land] = true;
  afterplayermoved();
  
  return after_move();
  }

bool pcmove::stay() {
  if(items[itOrbGravity]) {
    gravity_state = get_static_gravity(cwt.at);
    if(gravity_state) markOrb(itOrbGravity);
    }
  lastmovetype = lmSkip; lastmove = NULL;
  if(checkNeedMove(checkonly, false))
    return false;
  swordAttackStatic();
  nextmovetype = lmSkip;

  mi = movei(cwt.at, STAY);
  if(last_gravity_state && !gravity_state)
    playerMoveEffects(mi);
  if(d == -2) 
    dropGreenStone(cwt.at);

  items[itFatigue] -= 5;
  if(items[itFatigue] < 0)
    items[itFatigue] = 0;

  if(monstersnear_add_pmi(mi)) {
    if(vmsg(miTHREAT)) wouldkill("%The1 would get you!");
    return false;
    }
  if(checkonly) return true;
  if(changes.on) changes.commit();
  if(cellUnstable(cwt.at) && !markOrb(itOrbAether))
    doesFallSound(cwt.at);    
  
  return after_move();
  }

#if HDR
inline bool movepcto(const movedir& md) { return movepcto(md.d, md.subdir); }
#endif

EX bool warning_shown;

EX bool warningprotection(const string& s) {
  if(hardcore) return false;
  if(multi::activePlayers() > 1) return false;
  if(items[itWarning]) return false;
  warning_shown = true;
  pushScreen([s] () {
    cmode = sm::DARKEN;
    gamescreen();
    dialog::addBreak(250);
    dialog::init(XLAT("WARNING"), 0xFF0000, 150, 100);
    dialog::addBreak(500);
    dialog::addInfo(s);
    dialog::addBreak(500);
    dialog::addItem(XLAT("YES"), 'y');
    dialog::lastItem().scale = 200;
    auto yes = [] () { items[itWarning] = 1; popScreen(); };
    dialog::add_action(yes);
    dialog::add_key_action(SDLK_RETURN, yes);
    dialog::addItem(XLAT("NO"), 'n');
    dialog::lastItem().scale = 200;
    dialog::add_action([] () { items[itWarning] = 0; popScreen(); });
    dialog::display();
    });
  return true;
  }

EX bool warningprotection_hit(eMonster m) {
  if(m && warningprotection(XLAT("Are you sure you want to hit %the1?", m)))
    return true;
  return false;
  }

EX bool playerInWater() { 
  for(int i: player_indices())
    if(isWatery(playerpos(i)) && !playerInBoat(i)) 
      return true;
  return false;
  }

EX int numplayers() {
  return multi::players;
  }

EX vector<cell*> player_positions() {
  vector<cell*> res;
  for(int i=0; i<numplayers(); i++)
    if(multi::playerActive(i))
      res.push_back(playerpos(i));
  return res;
  }

EX vector<int> player_indices() {
  vector<int> res;
  for(int i=0; i<numplayers(); i++)
    if(multi::playerActive(i))
      res.push_back(i);
  return res;
  }

EX cell *playerpos(int i) {
  if(shmup::on) return shmup::playerpos(i);
  if(multi::players > 1) return multi::player[i].at;
  return singlepos();
  }

EX bool allPlayersInBoats() {
  for(cell *pc: player_positions())
    if(pc->wall != waBoat) return true;
  return false;
  }

EX int whichPlayerOn(cell *c) {
  if(singleused()) return c == singlepos() ? 0 : -1;
  for(int i: player_indices())
    if(playerpos(i) == c) return i;
  return -1;
  }

EX bool isPlayerOn(cell *c) {
  return whichPlayerOn(c) >= 0;
  }

EX bool isPlayerInBoatOn(cell *c, int i) {
  return
    (playerpos(i) == c && (
      c->wall == waBoat || c->wall == waStrandedBoat || (shmup::on && shmup::playerInBoat(i))
      ));
  }

EX bool playerInBoat(int i) {
  return isPlayerInBoatOn(playerpos(i), i);
  }

EX bool isPlayerInBoatOn(cell *c) {
  for(int i=0; i<numplayers(); i++) if(isPlayerInBoatOn(c, i)) return true;
  return false;
  }

EX bool playerInPower() {
  if(singleused()) 
    return singlepos()->land == laPower || singlepos()->land == laHalloween;
  for(cell *pc: player_positions())
    if(pc->land == laPower || pc->land == laHalloween)
      return true;
  return false;
  }

EX void playerMoveEffects(movei mi) {
  cell *c1 = mi.s;
  cell *c2 = mi.t;

  if(peace::on) items[itOrbSword] = c2->land == laBurial ? 100 : 0;
  
  changes.value_keep(sword::dir[multi::cpid]);
  sword::dir[multi::cpid] = sword::shift(mi, sword::dir[multi::cpid]);
  
  destroyWeakBranch(c1, c2, moPlayer);

  #if CAP_COMPLEX2
  mine::uncover_full(c2);
  #endif
  
  if((c2->wall == waClosePlate || c2->wall == waOpenPlate) && normal_gravity_at(c2) && !markOrb(itOrbAether))
    toggleGates(c2, c2->wall);

  if(c2->wall == waArrowTrap && c2->wparam == 0 && normal_gravity_at(c2) && !markOrb(itOrbAether))
    activateArrowTrap(c2);
  
  if(c2->wall == waFireTrap && c2->wparam == 0 && normal_gravity_at(c2) &&!markOrb(itOrbAether)) {
    playSound(c2, "click");
    changes.ccell(c2);
    c2->wparam = 1;
    }
  
  if(c2->wall == waReptile)
    c2->wparam = -1;
    
  princess::playernear(c2);

  if(c2->wall == waGlass && items[itOrbAether] > ORBBASE+1) {
    addMessage(XLAT("Your Aether powers are drained by %the1!", c2->wall));
    drainOrb(itOrbAether, 2);
    }
    
  if(cellUnstable(c2) && !markOrb(itOrbAether)) {
    doesFallSound(c2);
    if(c2->land == laMotion && c2->wall == waChasm) c2->mondir = mi.rev_dir_or(NODIR);
    }

  if(c2->wall == waStrandedBoat && markOrb(itOrbWater))
    c2->wall = waBoat;

  if(c2->land == laOcean && c2->wall == waBoat && c2->landparam < 30 && markOrb(itOrbWater))
    c2->landparam = 40;
  
  if((c2->land == laHauntedWall || c2->land == laHaunted) && !hauntedWarning) {
    changes.value_set(hauntedWarning, true);
    addMessage(XLAT("You become a bit nervous..."));
    addMessage(XLAT("Better not to let your greed make you stray from your path."));
    playSound(c2, "nervous");
    }  
  }

EX void afterplayermoved() {
  pregen();
  if(!racing::on)
  setdist(cwt.at, 7 - getDistLimit() - genrange_bonus, NULL);
  prairie::treasures();
  if(generatingEquidistant) {
    printf("Warning: generatingEquidistant set to true\n");
    generatingEquidistant = false;
    }
  }

EX void produceGhost(cell *c, eMonster victim, eMonster who) {
  if(who != moPlayer && !items[itOrbEmpathy]) return;
  if(markOrb(itOrbUndeath) && !c->monst && isGhostable(victim)) {
    changes.ccell(c);
    c->monst = moFriendlyGhost, c->stuntime = 0;
    if(who != moPlayer) markOrb(itOrbEmpathy);
    }
  }

EX bool swordAttack(cell *mt, eMonster who, cell *c, int bb) {
  eMonster m = c->monst;
  if(c->wall == waCavewall) markOrb(bb ? itOrbSword2: itOrbSword);
  if(among(c->wall, waSmallTree, waBigTree, waRose, waCTree, waVinePlant, waBigBush, waSmallBush, waSolidBranch, waWeakBranch, waShrub)
    || thruVine(mt, c)) {
    changes.ccell(c);
    playSound(NULL, "hit-axe"+pick123());
    markOrb(bb ? itOrbSword2: itOrbSword);
    drawParticles(c, winf[c->wall].color, 16);
    addMessage(XLAT("You chop down %the1.", c->wall));
    destroyHalfvine(c);
    c->wall = waNone;
    }
  if(c->wall == waBarrowDig) {
    changes.ccell(c);
    playSound(NULL, "hit-axe"+pick123());
    markOrb(bb ? itOrbSword2: itOrbSword);
    drawParticles(c, winf[c->wall].color, 16);
    c->wall = waNone;
    }
  if(c->wall == waBarrowWall && items[itBarrow] >= 25) {
    changes.ccell(c);
    playSound(NULL, "hit-axe"+pick123());
    markOrb(bb ? itOrbSword2: itOrbSword);
    drawParticles(c, winf[c->wall].color, 16);
    c->wall = waNone;
    }
  if(c->wall == waExplosiveBarrel)
    explodeBarrel(c);
  if(!peace::on && isPlayerOn(c) && whichPlayerOn(c) != multi::cpid && !markOrb(itOrbEmpathy)) killThePlayer(moPlayer, whichPlayerOn(mt), 0);
  if(!peace::on && mt == c && !markOrb(itOrbEmpathy)) killThePlayer(moPlayer, multi::cpid, 0);
  if(!peace::on && canAttack(mt, who, c, m, AF_SWORD)) {
    changes.ccell(c);
    markOrb(bb ? itOrbSword2: itOrbSword);
    int k = tkills();
    attackMonster(c, AF_NORMAL | AF_MSG | AF_SWORD, who);
    if(c->monst == moShadow) c->monst = moNone;
    produceGhost(c, m, who);
    if(tkills() > k) return true;
    }
  return false;
  }

EX void swordAttackStatic(int bb) {
  swordAttack(cwt.at, moPlayer, sword::pos(multi::cpid, bb), bb);
  }

EX void swordAttackStatic() {
  for(int bb = 0; bb < 2; bb++) 
    if(sword::orbcount(bb))
      swordAttackStatic(bb);
  }

EX int plague_kills;

EX void spread_plague(cell *mf, cell *mt, int dir, eMonster who) {
  if(!items[itOrbPlague]) return;
  if(who != moPlayer && !items[itOrbEmpathy]) return;
  forCellEx(mx, mt) if(celldistance(mx, mf) > celldistance(mx, mf->modmove(dir)) && celldistance(mx, mf) <= 4) {
    sideAttackAt(mf, dir, mx, who, itOrbPlague, mt);
    }
  }

EX void sideAttackAt(cell *mf, int dir, cell *mt, eMonster who, eItem orb, cell *pf) {
  eMonster m = mt->monst;
  flagtype f = AF_SIDE;
  if(orb == itOrbPlague) f |= AF_PLAGUE;
  if(items[itOrbSlaying]) f|= AF_CRUSH;
  if(!items[orb]) return;
  auto plague_particles = [&] {
    if(orb == itOrbPlague) {
      for(int i=0; i<16; i++)
        drawDirectionalParticle(pf, neighborId(pf, mt), (i&1) ? orb_auxiliary_color(orb) : iinf[orb].color);
      }
    };
  if(canAttack(mf, who, mt, m, f)) {
    if((f & AF_CRUSH) && !canAttack(mf, who, mt, m, AF_SIDE | AF_MUSTKILL))
      markOrb(itOrbSlaying);
    markOrb(orb);
    changes.ccell(mt);
    plague_particles();
    if(who != moPlayer) markOrb(itOrbEmpathy);
    int kk = 0;
    if(orb == itOrbPlague) kk = tkills();
    if(attackMonster(mt, AF_NORMAL | f | AF_MSG, who) || isAnyIvy(m)) {
      if(orb == itOrbPlague && kk < tkills())
        plague_kills++;
      if(mt->monst != m) spread_plague(mf, mt, dir, who);
      produceGhost(mt, m, who);
      }
    }
  else if(mt->wall == waSmallTree) {
    changes.ccell(mt);
    plague_particles();
    markOrb(orb);
    mt->wall = waNone;
    spread_plague(mf, mt, dir, who);
    }
  else if(mt->wall == waShrub && markEmpathy(itOrbSlaying)) {
    changes.ccell(mt);
    plague_particles();
    markOrb(orb);
    mt->wall = waNone;
    spread_plague(mf, mt, dir, who);
    }
  else if(mt->wall == waBigTree) {
    changes.ccell(mt);
    plague_particles();
    markOrb(orb);
    mt->wall = waSmallTree;
    }
  else if(mt->wall == waExplosiveBarrel && orb != itOrbPlague) {
    changes.ccell(mt);
    explodeBarrel(mt);
    }
  }

EX void sideAttack(cell *mf, int dir, eMonster who, int bonus, eItem orb) {
  if(!items[orb]) return;
  if(who != moPlayer && !items[itOrbEmpathy]) return;
  for(int k: {-1, 1}) {
    int dir1 = dir + k*bonus;
    dir1 = mf->c.fix(dir1);
    cell *mt = mf->move(dir1);
    sideAttackAt(mf, dir1, mt, who, orb, mf);
    }
  }

EX void sideAttack(cell *mf, int dir, eMonster who, int bonuskill) {

  int k = tkills();
  plague_kills = 0;
  sideAttack(mf, dir, who, 1, itOrbSide1);
  sideAttack(mf, dir, who, 2, itOrbSide2);
  sideAttack(mf, dir, who, 3, itOrbSide3);  
  k += plague_kills;

  if(who == moPlayer) {
    int kills = tkills() - k + bonuskill;
    if(kills >= 5) achievement_gain_once("MELEE5");
    }
  }

EX eMonster do_we_stab_a_friend(movei mi, eMonster who) {
  eMonster m = moNone;
  do_swords(mi, who, [&] (cell *c, int bb) { 
    if(!peace::on && canAttack(mi.t, who, c, c->monst, AF_SWORD) && c->monst && isFriendly(c)) m = c->monst;
    });

  for(int t=0; t<mi.s->type; t++) {
    cell *c = mi.s->move(t);
    if(!c) continue;
    
    bool stabthere = false;
    if(logical_adjacent(mi.t, who, c)) stabthere = true;
    
    if(stabthere && canAttack(mi.t,who,c,c->monst,AF_STAB) && isFriendly(c)) 
      return c->monst;
    }
  
  return m;
  }

EX void wouldkill(const char *msg) {
  if(who_kills_me == moWarning)
    addMessage(XLAT("This move appears dangerous -- are you sure?"));
  else if(who_kills_me == moFireball) 
    addMessage(XLAT("Cannot move into the current location of another player!"));
  else if(who_kills_me == moAirball) 
    addMessage(XLAT("Players cannot get that far away!"));
  else if(who_kills_me == moTongue) 
    addMessage(XLAT("Cannot push into another player!"));
  else if(who_kills_me == moCrushball) 
    addMessage(XLAT("Cannot push into the same location!"));
  else
    addMessage(XLAT(msg, who_kills_me));
  }

EX void movecost(cell* from, cell *to, int phase) {
  if(from->land == laPower && to->land != laPower && (phase & 1)) {
    int n=0;
    for(int i=0; i<ittypes; i++)
      if(itemclass(eItem(i)) == IC_ORB && items[i] >= 2 && i != itOrbFire)
        items[i] = 2, n++;
    if(n) 
      addMessage(XLAT("As you leave, your powers are drained!"));
    }
  
#if CAP_TOUR
  if(from->land != to->land && tour::on && (phase & 2)) {
    changes.at_commit([to] { tour::checkGoodLand(to->land); });
    }
#endif
  
  if(to->land == laCrossroads4 && !geometry && (phase & 2) && !cheater) {
    achievement_gain_once("CR4");
    changes.value_set(chaosUnlocked, true);
    }

  if(isHaunted(from->land) && !isHaunted(to->land) && (phase & 2)) {
    updateHi(itLotus, truelotus = items[itLotus]);
    if(items[itLotus] >= 1) achievement_gain_once("LOTUS1");
    if(items[itLotus] >= (big_unlock ? 25 : 10)) achievement_gain_once("LOTUS2");
    if(items[itLotus] >= (big_unlock ? 50 : 25)) achievement_gain_once("LOTUS3");
    if(items[itLotus] >= 50 && !big_unlock) achievement_gain_once("LOTUS4");
    achievement_final(false);
    }
  
  if(geometry == gNormal && celldist(to) == 0 && !usedSafety && gold() >= 100 && (phase & 2))
    achievement_gain_once("COMEBACK");
  
  bool tortoiseOK = 
    to->land == from->land || to->land == laTortoise ||
    (to->land == laDragon && from->land != laTortoise) || 
    ls::any_chaos();
  
  if(tortoise::seek() && !from->item && !tortoiseOK && passable(from, NULL, 0) && (phase & 2)) {
    changes.ccell(from);
    changes.map_value(tortoise::babymap, from);
    from->item = itBabyTortoise;
    tortoise::babymap[from] = tortoise::seekbits;
    addMessage(XLAT("You leave %the1.", itBabyTortoise));
    items[itBabyTortoise]--;
    }
  }

}