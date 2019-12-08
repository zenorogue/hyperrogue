// Hyperbolic Rogue - PC movement
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file checkmove.cpp
 *  \brief PC movements
 */

#include "hyper.h"

namespace hr {

EX bool keepLightning = false;

EX bool seenSevenMines = false;

/** have we been warned about the Haunted Woods? */
EX bool hauntedWarning;

/** is the Survivalist achievement still valid? have we received it? */
EX bool survivalist, got_survivalist;

/** last move was invisible */
EX bool invismove = false;
/** last move was invisible due to Orb of Fish (thus Fish still see you)*/
EX bool invisfish = false;

/** if false, make the PC look in direction cwt.spin (after attack); otherwise, make them look the other direction (after move) */
EX bool flipplayer = true;

/** Cellwalker describing the single player. Also used temporarily in shmup and multiplayer modes. */
EX cellwalker cwt;

EX cell*& singlepos() { return cwt.at; }
EX inline bool singleused() { return !(shmup::on || multi::players > 1); }

/** should we center the screen on the PC? */
EX bool playermoved = true;

/** did the player cheat? how many times? */
EX int  cheater = 0;

/** lands visited -- unblock some modes */
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

EX bool checkNeedMove(bool checkonly, bool attacking) {
  if(items[itOrbDomination] > ORBBASE && cwt.at->monst) 
    return false;
  int flags = 0;
  if(cwt.at->monst) {
    if(checkonly) return true;
    if(isMountable(cwt.at->monst))
      addMessage(XLAT("You need to dismount %the1!", cwt.at->monst));
    else
      addMessage(XLAT("You need to move to give space to %the1!", cwt.at->monst));
    }
  else if(cwt.at->wall == waRoundTable) {
    if(markOrb2(itOrbAether)) return false;
    if(checkonly) return true;
    addMessage(XLAT("It would be impolite to land on the table!"));
    }
  else if(cwt.at->wall == waLake) {
    if(markOrb2(itOrbAether)) return false;
    if(markOrb2(itOrbFish)) return false;
    if(in_gravity_zone(cwt.at) && passable(cwt.at, NULL, P_ISPLAYER)) return false;
    if(checkonly) return true;
    flags |= AF_FALL;
    addMessage(XLAT("Ice below you is melting! RUN!"));
    }
  else if(!attacking && cellEdgeUnstable(cwt.at)) {
    if(markOrb2(itOrbAether)) return false;
    if(checkonly) return true;
    if(in_gravity_zone(cwt.at) && passable(cwt.at, NULL, P_ISPLAYER)) return false;
    addMessage(XLAT("Nothing to stand on here!"));
    }
  else if(cwt.at->wall == waSea || cwt.at->wall == waCamelotMoat) {
    if(markOrb(itOrbFish)) return false;
    if(markOrb2(itOrbAether)) return false;
    if(in_gravity_zone(cwt.at) && passable(cwt.at, NULL, P_ISPLAYER)) return false;
    if(checkonly) return true;
    addMessage(XLAT("You have to run away from the water!"));
    }
  else if(cwt.at->wall == waClosedGate) {
    if(markOrb2(itOrbAether)) return false;
    if(checkonly) return true;
    addMessage(XLAT("The gate is closing right on you! RUN!"));
    }
  else if(isFire(cwt.at) && !markOrb(itOrbWinter) && !markOrb2(itOrbShield)) {
    if(markOrb2(itOrbAether)) return false;
    if(checkonly) return true;
    addMessage(XLAT("This spot will be burning soon! RUN!"));
    }
  else if(cwt.at->wall == waMagma && !markOrb(itOrbWinter) && !markOrb2(itOrbShield)) {
    if(markOrb2(itOrbAether)) return false;
    if(in_gravity_zone(cwt.at) && passable(cwt.at, cwt.at, P_ISPLAYER)) return false;
    if(checkonly) return true;
    addMessage(XLAT("Run away from the magma!"));
    }
  else if(cwt.at->wall == waChasm) {
    if(markOrb2(itOrbAether)) return false;
    if(in_gravity_zone(cwt.at) && passable(cwt.at, cwt.at, P_ISPLAYER)) return false;
    if(checkonly) return true;
    flags |= AF_FALL;
    addMessage(XLAT("The floor has collapsed! RUN!"));
    }
  else if(items[itOrbAether] > ORBBASE && !passable(cwt.at, NULL, P_ISPLAYER | P_NOAETHER)) {
    if(markOrb2(itOrbAether)) return false;
    return true;
    }
  else if(!passable(cwt.at, NULL, P_ISPLAYER)) {
    if(isFire(cwt.at)) return false; // already checked: have Shield
    if(markOrb2(itOrbAether)) return false;
    if(checkonly) return true;
    addMessage(XLAT("Your Aether power has expired! RUN!"));
    }
  else return false;
  if(hardcore) 
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

  void tell_why_cannot_attack();
  void tell_why_impassable();
  void handle_friendly_ivy();

  movei mi, mip;
  pcmove() : mi(nullptr, nullptr, 0), mip(nullptr, nullptr, 0) {}
  };
#endif

EX pcmove pcm;

EX bool movepcto(int d, int subdir IS(1), bool checkonly IS(false)) {
  pcm.checkonly = checkonly;
  pcm.d = d; pcm.subdir = subdir;
  return pcm.movepcto();
  }

bool pcmove::movepcto() {  
  if(dual::state == 1) return dual::movepc(d, subdir, checkonly);
  if(d >= 0 && !checkonly && subdir != 1 && subdir != -1) printf("subdir = %d\n", subdir);
  mip.t = NULL;
  switchplaces = false;

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
    if(!checkonly) addMessage("You just cannot stand in place, those roses smell too nicely.");
    return false;
    }

  gravity_state = gsNormal;
  
  fmsMove     = forcedmovetype == fmSkip || forcedmovetype == fmMove;
  fmsAttack   = forcedmovetype == fmSkip || forcedmovetype == fmAttack;
  fmsActivate = forcedmovetype == fmSkip || forcedmovetype == fmActivate;
  
  return (d >= 0) ? actual_move() : stay();
  }

bool pcmove::after_move() {
  if(checkonly) return true;
    
  invisfish = false;
  if(items[itOrbFish]) {
     invisfish = true;
     for(int i=0; i<numplayers(); i++) 
       if(multi::playerActive(i) && !isWatery(playerpos(i)))
         invisfish = false;
     if(d < 0) invisfish = false; // no invisibility if staying still
     if(invisfish) invismove = true, markOrb(itOrbFish);
     }
  
  last_gravity_state = gravity_state;
  if(multi::players == 1) monstersTurn();

  save_memory();
  
  check_total_victory();

  if(items[itWhirlpool] && cwt.at->land != laWhirlpool && !whirlpool::escaped) {
    whirlpool::escaped = true;
    achievement_gain("WHIRL1");
    }

  if(items[itLotus] >= 25 && !isHaunted(cwt.at->land) && survivalist && !got_survivalist) {
    got_survivalist = true;
    achievement_gain("SURVIVAL");
    }

  if(seenSevenMines && cwt.at->land != laMinefield) {
    seenSevenMines = false;
    achievement_gain("SEVENMINE");
    }
  
  DEBB(DF_TURN, ("done"));
  return true;
  }

bool pcmove::swing() {
  sideAttack(cwt.at, d, moPlayer, 0);
  animateAttack(mi, LAYER_SMALL);

  if(survivalist && isHaunted(mi.t->land))
    survivalist = false;
  mirror::act(origd, mirror::SPINMULTI | mirror::ATTACK);
  lastmovetype = lmTree; lastmove = mi.t;
  swordAttackStatic();

  return after_move();
  }

bool pcmove::after_instant(bool kl) {
  keepLightning = kl;
  bfs();
  keepLightning = false;
  if(multi::players > 1) { multi::whereto[multi::cpid].d = MD_UNDECIDED; return false; }
  checkmove();
  return true;
  }

bool pcmove::actual_move() {
  origd = d;
  if(d >= 0) {
    cwt += d;
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
    if(!checkonly) addMessage("Those roses smell too nicely. You have to come towards them.");
    return false;
    }
  
  bool try_instant = (forcedmovetype == fmInstant) || (forcedmovetype == fmSkip && !passable(c2, cwt.at, P_ISPLAYER | P_MIRROR | P_USEBOAT | P_FRIENDSWAP));
  
  if(items[itOrbDomination] > ORBBASE && isMountable(c2->monst) && !monstersnear2() && fmsMove) {
    if(checkonly) { nextmovetype = lmMove; return true; }
    if(!isMountable(cwt.at->monst)) dragon::target = NULL;
    movecost(cwt.at, c2, 3);
    
    flipplayer = true; if(multi::players > 1) multi::flipped[multi::cpid] = true;
    invismove = (turncount >= noiseuntil) && items[itOrbInvis] > 0;
    killFriendlyIvy();
    return perform_move_or_jump();
    }
  
  if(items[itOrbFlash] && try_instant) {
    if(checkonly) { nextmovetype = lmInstant; return true; }
    if(orbProtection(itOrbFlash)) return true;
    activateFlash();
    return after_instant(false);
    }

  if(items[itOrbLightning] && try_instant) {
    if(checkonly) { nextmovetype = lmInstant; return true; }
    if(orbProtection(itOrbLightning)) return true;
    activateLightning();
    return after_instant(true);
    }

  if(isActivable(c2) && fmsActivate) {
    if(checkonly) { nextmovetype = lmInstant; return true; }
    activateActiv(c2, true);
    return after_instant(false);
    }

  if(isPushable(c2->wall) && !c2->monst && !nonAdjacentPlayer(c2, cwt.at) && fmsMove) {
    mip = determinePush(cwt, subdir, [c2] (cell *c) { return canPushThumperOn(c, c2, cwt.at); });
    if(mip.d == NO_SPACE) {
      if(!checkonly) addMessage(XLAT("No room to push %the1.", c2->wall));
      return false;
      }
    if(monstersnear(c2, NULL, moPlayer, NULL, cwt.at)) {
      if(!checkonly && errormsgs) wouldkill("%The1 would kill you there!");
      return false;
      }
    if(checkonly) { nextmovetype = lmMove; return true; }
    addMessage(XLAT("You push %the1.", c2->wall));
    lastmovetype = lmPush; lastmove = cwt.at;
    pushThumper(mip);
    return perform_actual_move();
    }

  if(c2->item == itHolyGrail && roundTableRadius(c2) < newRoundTableRadius()) {
    if(!checkonly)
      addMessage(XLAT("That was not a challenge. Find a larger castle!"));
    return false;
    }

  if(c2->item == itOrbYendor && !checkonly && !peace::on && !itemHiddenFromSight(c2) && yendor::check(c2)) {
    return false;
    }

  if(isWatery(c2) && !nonAdjacentPlayer(cwt.at,c2) && !c2->monst && cwt.at->wall == waBoat && fmsMove) 
    return boat_move();  
  
  if(!c2->monst && cwt.at->wall == waBoat && cwt.at->item != itOrbYendor && boatGoesThrough(c2) && markOrb(itOrbWater) && !nonAdjacentPlayer(c2, cwt.at) && fmsMove) {

    if(havePushConflict(cwt.at, checkonly)) return false;
    if(monstersnear(c2,NULL,moPlayer,NULL,cwt.at)) {
      if(!checkonly && errormsgs) wouldkill("%The1 would kill you there!");
      return false;
      }
    
    if(checkonly) { nextmovetype = lmMove; return true; }
    if(c2->item && !cwt.at->item) moveItem(c2, cwt.at, false), boatmove = true;
    placeWater(c2, cwt.at);
    moveBoat(mi);
    c2->mondir = revhint(cwt.at, d);
    if(c2->item) boatmove = !boatmove;
    return perform_actual_move();
    }
  
  return after_escape();
  }

bool pcmove::boat_move() {

  cell *& c2 = mi.t;
  if(havePushConflict(cwt.at, checkonly)) return false;

  if(againstWind(c2, cwt.at)) {
    if(!checkonly) 
      addMessage(XLAT(airdist(c2) < 3 ? "The Air Elemental blows you away!" : "You cannot go against the wind!"));
    return false;
    }

  if(againstCurrent(c2, cwt.at) && !markOrb(itOrbWater)) {
    if(markOrb(itOrbFish) || markOrb(itOrbAether) || gravity_state)
      return after_escape();
    if(!checkonly)
      addMessage(XLAT("You cannot go against the current!"));
    return false;
    }

  if(cwt.at->item == itOrbYendor) {        
    if(markOrb(itOrbFish) || markOrb(itOrbAether) || gravity_state) 
      return after_escape();
    if(!checkonly)
      addMessage(XLAT("The Orb of Yendor is locked in with powerful magic."));
    return false;
    }

  if(monstersnear(c2, NULL, moPlayer, NULL, cwt.at)) {
    if(!checkonly && errormsgs) 
      wouldkill("%The1 would kill you there!");
    return false;
    }
  
  if(checkonly) { nextmovetype = lmMove; return true; }
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
  else if(nonAdjacent(c2, cwt.at))
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
  else if(c2->monst == moVizier && c2->hitpoints > 1 && !(attackflags & AF_FAST)) {
    addMessage(XLAT("You cannot attack %the1 directly!", c2->monst));
    addMessage(XLAT("Hit him by walking away from him."));
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
  else
    addMessage(XLAT("For some reason... cannot attack!"));
  }

bool pcmove::after_escape() {
  cell*& c2 = mi.t;

  if(c2->wall == waBigStatue && !c2->monst && !nonAdjacentPlayer(c2, cwt.at) && fmsMove) {
    if(!canPushStatueOn(cwt.at)) {
      if(!checkonly) {
        if(isFire(cwt.at))
          addMessage(XLAT("You have to escape first!"));
        else
          addMessage(XLAT("There is not enough space!"));
        }
      return false;
      }
    
    if(havePushConflict(cwt.at, checkonly)) return false;
    
    eWall save_c2 = c2->wall;
    eWall save_cw = cwt.at->wall;
    c2->wall = cwt.at->wall;
    if(doesnotFall(cwt.at))
      cwt.at->wall = waBigStatue;
    
    if(monstersnear(c2,NULL,moPlayer,NULL,cwt.at)) {
      if(!checkonly && errormsgs) wouldkill("%The1 would kill you there!");
      c2->wall = save_c2; cwt.at->wall = save_cw;
      return false;
      }        
        
    if(checkonly) { c2->wall = save_c2; cwt.at->wall = save_cw; nextmovetype = lmMove; return true; }
    addMessage(XLAT("You push %the1 behind you!", waBigStatue));
    animateMovement(mi.rev(), LAYER_BOAT);
    return perform_actual_move();
    }

  bool attackable;
  attackable = 
    c2->wall == waBigTree ||
    c2->wall == waSmallTree ||
    c2->wall == waMirrorWall;
  if(attackable && markOrb(itOrbAether) && c2->wall != waMirrorWall)
    attackable = false;
  bool nm; nm = attackable;
  if(forcedmovetype == fmAttack) attackable = true;
  attackable = attackable && (!c2->monst || isFriendly(c2));
  attackable = attackable && !nonAdjacentPlayer(cwt.at,c2);
    
  if(attackable && fmsAttack) {
    if(checkNeedMove(checkonly, true)) return false;
    if(monstersnear(cwt.at,c2,moPlayer,NULL,cwt.at)) {
      if(!checkonly && errormsgs) wouldkill("%The1 would get you!");
      return false;
      }
    if(checkonly) { nextmovetype = nm ? lmAttack : lmSkip; return true; }
    if(c2->wall == waSmallTree) {
      drawParticles(c2, winf[c2->wall].color, 4);
      addMessage(XLAT("You chop down the tree."));
      playSound(c2, "hit-axe" + pick123());
      c2->wall = waNone;
      return swing();
      }
    else if(c2->wall == waBigTree) {
      drawParticles(c2, winf[c2->wall].color, 8);
      addMessage(XLAT("You start chopping down the tree."));
      playSound(c2, "hit-axe" + pick123());
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
    if(!checkonly) camelot::knightFlavorMessage(c2);
    return false;
    }
  else if(c2->monst && (!isFriendly(c2) || c2->monst == moTameBomberbird || isMountable(c2->monst))
    && !(peace::on && !isMultitile(c2->monst) && !good_tortoise)) 
    return attack();
  else if(!passable(c2, cwt.at, P_USEBOAT | P_ISPLAYER | P_MIRROR | P_MONSTER)) {
    if(!checkonly) tell_why_impassable();
    return false;
    }
  else if(fmsMove) 
    return move_if_okay();
  
  else return false;
  }

bool pcmove::move_if_okay() {
  cell*& c2 = mi.t;
  if(mine::marked_mine(c2) && !mine::safe() && !checkonly && warningprotection(XLAT("Are you sure you want to step there?")))
    return false;
  
  if(snakelevel(c2) <= snakelevel(cwt.at)-2) {
    bool can_leave = false;
    forCellEx(c3, c2) if(passable(c3, c2, P_ISPLAYER | P_MONSTER)) can_leave = true;
    if(!can_leave && !checkonly && warningprotection(XLAT("Are you sure you want to step there?")))
      return false;
    }

  if(monstersnear(c2, NULL, moPlayer, NULL, cwt.at)) {
    if(checkonly) return false;

    if(items[itOrbFlash]) {
      if(checkonly) { nextmovetype = lmInstant; return true; }
      if(orbProtection(itOrbFlash)) return true;
      activateFlash();
      checkmove();
      return true;
      }

    if(items[itOrbLightning]) {
      if(checkonly) { nextmovetype = lmInstant; return true; }
      if(orbProtection(itOrbLightning)) return true;
      activateLightning();
      checkmove();
      return true;
      }

    if(who_kills_me == moOutlaw && items[itRevolver]) {
      for(int i=0; i<c2->type; i++) {
        cell *c3 = c2->move(i);
        if(c3) for(int i=0; i<c3->type; i++) {
          cell *c4 = c3->move(i);
          if(c4 && c4->monst == moOutlaw) {
            eItem i = targetRangedOrb(c4, roCheck);
            if(i == itRevolver) { 
              targetRangedOrb(c4, roKeyboard);
              return false;
              }
            }
          }
        }
      }

    if(!checkonly && errormsgs) 
      wouldkill("%The1 would kill you there!");
    return false;
    }

  if(switchplace_prevent(cwt.at, c2, checkonly)) return false;
  if(!checkonly && warningprotection_hit(do_we_stab_a_friend(cwt.at, c2, moPlayer)))
    return false;
  
  if(checkonly) { nextmovetype = lmMove; return true; }
  
  return perform_actual_move();
  }

void pcmove::tell_why_impassable() {
  cell*& c2 = mi.t;
  if(nonAdjacent(cwt.at,c2))
    addMessage(XLAT(
      geosupport_football() < 2 ? 
      "You cannot move between the cells without dots here!" :
      "You cannot move between the triangular cells here!"
      ));
  else if(againstWind(c2, cwt.at))
    addMessage(XLAT(airdist(c2) < 3 ? "The Air Elemental blows you away!" : "You cannot go against the wind!"));
  else if(isAlch(c2))
    addMessage(XLAT("Wrong color!"));
  else if(c2->wall == waRoundTable)
    addMessage(XLAT("It would be impolite to land on the table!"));
  else if(snakelevel(cwt.at) >= 3 && snakelevel(c2) == 0)
    addMessage(XLAT("You would get hurt!", c2->wall));
  else if(cellEdgeUnstable(cwt.at) && cellEdgeUnstable(c2)) {
    addMessage(XLAT("Gravity does not allow this!"));
    }
  else if(c2->wall == waChasm && c2->land == laDual)
    addMessage(XLAT("You cannot move there!"));
  else if(!c2->wall)
    addMessage(XLAT("You cannot move there!"));
  else {
    addMessage(XLAT("You cannot move through %the1!", c2->wall));
    }
  }

bool pcmove::attack() {
  auto& c2 = mi.t;
  if(!fmsAttack) return false;

  attackflags = AF_NORMAL;
  if(items[itOrbSpeed]&1) attackflags |= AF_FAST;
  if(items[itOrbSlaying]) attackflags |= AF_CRUSH;
  
  bool ca =canAttack(cwt.at, moPlayer, c2, c2->monst, attackflags);
  
  if(!ca) {
    if(forcedmovetype == fmAttack) {
      if(monstersnear(cwt.at,c2,moPlayer,NULL,cwt.at)) {
        if(!checkonly && errormsgs) wouldkill("%The1 would get you!");
        return false;
        }
      if(checkonly) { nextmovetype = lmSkip; return true; }
      addMessage(XLAT("You swing your sword at %the1.", c2->monst));
      return swing();
      }
    if(!checkonly) tell_why_cannot_attack();
    return false;
    }
    
  // mip.t=c2 means that the monster is not destroyed and thus
  // still counts for lightning in monstersnear

  mip = movei(c2, nullptr, NO_SPACE);
  
  if(isStunnable(c2->monst) && c2->hitpoints > 1) {
    if(monsterPushable(c2))
      mip = determinePush(cwt, subdir, [c2] (cell *c) { return passable(c, c2, P_BLOW); });
    else
      mip.t = c2;
    }
  if(c2->monst == moTroll || c2->monst == moFjordTroll || 
     c2->monst == moForestTroll || c2->monst == moStormTroll || c2->monst == moVineSpirit)
    mip.t = c2;
  
  if(havePushConflict(mip.t, checkonly)) return false;
  
  if(!(isWatery(cwt.at) && c2->monst == moWaterElemental) && checkNeedMove(checkonly, true))
    return false;
  
  if(monstersnear(cwt.at, c2, moPlayer, mip.t, cwt.at)) {
    if(errormsgs && !checkonly)
      wouldkill("You would be killed by %the1!");          
    return false;
    }

  if(c2->monst == moTameBomberbird && warningprotection_hit(moTameBomberbird)) return false;
  
  if(checkonly) { nextmovetype = lmAttack; return true; }

  mirror::act(origd, mirror::SPINMULTI | mirror::ATTACK);
  
  int tk = tkills();

  if(good_tortoise) {
    items[itBabyTortoise] += 4;
    updateHi(itBabyTortoise, items[itBabyTortoise]);
    c2->item = itBabyTortoise;
    tortoise::babymap[c2] = tortoise::seekbits;
    playSound(c2, playergender() ? "heal-princess" : "heal-prince");
    addMessage(XLAT(playergender() == GEN_F ? "You are now a tortoise heroine!" : "You are now a tortoise hero!"));
    c2->stuntime = 2;
    achievement_collection(itBabyTortoise);
    }
  else {
    eMonster m = c2->monst;
    if(m) {
      if((attackflags & AF_CRUSH) && !canAttack(cwt.at, moPlayer, c2, c2->monst, attackflags ^ AF_CRUSH ^ AF_MUSTKILL))
        markOrb(itOrbSlaying);
      if(c2->monst == moTerraWarrior && hrand(100) > 2 * items[itTerra]) {
        if(hrand(2 + jiangshi_on_screen) < 2)
          wandering_jiangshi++;
        }
      attackMonster(c2, attackflags | AF_MSG, moPlayer);
      // salamanders are stunned for longer time when pushed into a wall
      if(c2->monst == moSalamander && (mip.t == c2 || !mip.t)) c2->stuntime = 10;
      if(!c2->monst) produceGhost(c2, m, moPlayer);
      if(mip.proper()) pushMonster(mip);
      animateAttack(mi, LAYER_SMALL);
      }
    }
  
  sideAttack(cwt.at, d, moPlayer, tkills() - tk);
  lastmovetype = lmAttack; lastmove = c2;
  swordAttackStatic();
  return after_move();
  }

bool pcmove::perform_actual_move() {
  cell*& c2 = mi.t;
  flipplayer = true; if(multi::players > 1) multi::flipped[multi::cpid] = true;
  if(c2->item && isAlch(c2)) {
    if(cwt.at->wall == waBoat)
      c2->wall = waNone;
    else
      c2->wall = cwt.at->wall;
    }
  if(c2->wall == waRoundTable) {
    addMessage(XLAT("You jump over the table!"));
    }
  
  if(cwt.at->wall == waRoundTable) 
    camelot::roundTableMessage(c2);
  
  invismove = (turncount >= noiseuntil) && items[itOrbInvis] > 0;
  
  if(items[itOrbFire]) {
    invismove = false;
    if(makeflame(cwt.at, 10, false)) markOrb(itOrbFire);
    }

  handle_friendly_ivy();  
  
  if(items[itOrbDigging]) {
    invismove = false;
    if(earthMove(mi)) markOrb(itOrbDigging);
    }

  movecost(cwt.at, c2, 1);

  if(!boatmove && collectItem(c2)) return true;
  if(doPickupItemsWithMagnetism(c2)) return true;

  if(isIcyLand(cwt.at) && cwt.at->wall == waNone && markOrb(itOrbWinter)) {
    invismove = false;
    cwt.at->wall = waIcewall;
    }
  
  if(items[itOrbWinter])
    forCellEx(c3, c2) if(c3->wall == waIcewall && c3->item)
      markOrb(itOrbWinter), collectItem(c3);
  
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

  stabbingAttack(cwt.at, mi.t, moPlayer);
  cell *c1 = cwt.at;
  cwt += wstep;
  if(switchplaces) {
    indAnimateMovement(mi, LAYER_SMALL);
    indAnimateMovement(mi.rev(), LAYER_SMALL);
    commitAnimations(LAYER_SMALL);
    }
  else
    animateMovement(mi, LAYER_SMALL);
  current_display->which_copy = current_display->which_copy * adj(mi);
  
  mirror::act(origd, mirror::SPINMULTI | mirror::ATTACK | mirror::GO);

  playerMoveEffects(c1, mi.t);

  if(mi.t->monst == moFriendlyIvy) mi.t->monst = moNone;

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
  if(monstersnear(cwt.at, NULL, moPlayer, NULL, cwt.at)) {
    if(errormsgs && !checkonly) 
      wouldkill("%The1 would get you!");
    return false;
    }
  if(checkonly) { nextmovetype = lmSkip; return true; }
  swordAttackStatic();
  if(d == -2) 
    dropGreenStone(cwt.at);
  if(cellUnstable(cwt.at) && !markOrb(itOrbAether))
    doesFallSound(cwt.at);
  
  if(last_gravity_state && !gravity_state)
    playerMoveEffects(cwt.at, cwt.at);
  
  return after_move();
  }

#if HDR
inline bool movepcto(const movedir& md) { return movepcto(md.d, md.subdir); }
#endif


EX bool warningprotection(const string& s) {
  if(hardcore) return false;
  if(multi::activePlayers() > 1) return false;
  if(items[itWarning]) return false;
  pushScreen([s] () {
    gamescreen(1);
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
  for(int i=0; i<numplayers(); i++) 
    if(multi::playerActive(i) && isWatery(playerpos(i)) && !playerInBoat(i)) 
      return true;
  return false;
  }

EX int numplayers() {
  return multi::players;
  }

EX cell *playerpos(int i) {
  if(shmup::on) return shmup::playerpos(i);
  if(multi::players > 1) return multi::player[i].at;
  return singlepos();
  }

EX bool allPlayersInBoats() {
  for(int i=0; i<numplayers(); i++) 
    if(multi::playerActive(i) && playerpos(i)->wall != waBoat) return true;
  return false;
  }

EX int whichPlayerOn(cell *c) {
  if(singleused()) return c == singlepos() ? 0 : -1;
  for(int i=0; i<numplayers(); i++) 
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
  for(int i=0; i<numplayers(); i++) 
    if(multi::playerActive(i) && (playerpos(i)->land == laPower || playerpos(i)->land == laHalloween))
      return true;
  return false;
  }

EX void playerMoveEffects(cell *c1, cell *c2) {

  if(peace::on) items[itOrbSword] = c2->land == laBurial ? 100 : 0;

  sword::dir[multi::cpid] = sword::shift(c1, c2, sword::dir[multi::cpid]);
  
  destroyWeakBranch(c1, c2, moPlayer);

  mine::uncover_full(c2);
  
  if((c2->wall == waClosePlate || c2->wall == waOpenPlate) && normal_gravity_at(c2) && !markOrb(itOrbAether))
    toggleGates(c2, c2->wall);

  if(c2->wall == waArrowTrap && c2->wparam == 0 && normal_gravity_at(c2) && !markOrb(itOrbAether))
    activateArrowTrap(c2);
  
  if(c2->wall == waFireTrap && c2->wparam == 0 && normal_gravity_at(c2) &&!markOrb(itOrbAether)) {
    playSound(c2, "click");
    c2->wparam = 1;
    }
    
  princess::playernear(c2);

  if(c2->wall == waGlass && items[itOrbAether] > ORBBASE+1) {
    addMessage(XLAT("Your Aether powers are drained by %the1!", c2->wall));
    drainOrb(itOrbAether, 2);
    }
    
  if(cellUnstable(c2) && !markOrb(itOrbAether)) doesFallSound(c2);

  if(c2->wall == waStrandedBoat && markOrb(itOrbWater))
    c2->wall = waBoat;

  if(c2->land == laOcean && c2->wall == waBoat && c2->landparam < 30 && markOrb(itOrbWater))
    c2->landparam = 40;
  
  if((c2->land == laHauntedWall || c2->land == laHaunted) && !hauntedWarning) {
    hauntedWarning = true;
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
    c->monst = moFriendlyGhost, c->stuntime = 0;
    if(who != moPlayer) markOrb(itOrbEmpathy);
    }
  }

EX bool swordAttack(cell *mt, eMonster who, cell *c, int bb) {
  eMonster m = c->monst;
  if(c->wall == waCavewall) markOrb(bb ? itOrbSword2: itOrbSword);
  if(c->wall == waSmallTree || c->wall == waBigTree || c->wall == waRose || c->wall == waCTree || c->wall == waVinePlant ||
    thruVine(mt, c) || c->wall == waBigBush || c->wall == waSmallBush || c->wall == waSolidBranch || c->wall == waWeakBranch) {
    playSound(NULL, "hit-axe"+pick123());
    markOrb(bb ? itOrbSword2: itOrbSword);
    drawParticles(c, winf[c->wall].color, 16);
    addMessage(XLAT("You chop down %the1.", c->wall));
    destroyHalfvine(c);
    c->wall = waNone;
    }
  if(c->wall == waBarrowDig) {
    playSound(NULL, "hit-axe"+pick123());
    markOrb(bb ? itOrbSword2: itOrbSword);
    drawParticles(c, winf[c->wall].color, 16);
    c->wall = waNone;
    }
  if(c->wall == waBarrowWall && items[itBarrow] >= 25) {
    playSound(NULL, "hit-axe"+pick123());
    markOrb(bb ? itOrbSword2: itOrbSword);
    drawParticles(c, winf[c->wall].color, 16);
    c->wall = waNone;
    }
  if(c->wall == waExplosiveBarrel)
    explodeBarrel(c);
  if(!peace::on && canAttack(mt, who, c, m, AF_SWORD)) {
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

EX void sideAttack(cell *mf, int dir, eMonster who, int bonus, eItem orb) {
  if(!items[orb]) return;
  if(who != moPlayer && !items[itOrbEmpathy]) return;
  for(int k: {-1, 1}) {
    cell *mt = mf->modmove(dir + k*bonus);
    eMonster m = mt->monst;
    flagtype f = AF_SIDE;
    if(items[itOrbSlaying]) f|= AF_CRUSH;
    if(canAttack(mf, who, mt, m, f)) {
      if((f & AF_CRUSH) && !canAttack(mf, who, mt, m, AF_SIDE | AF_MUSTKILL))
        markOrb(itOrbSlaying);
      markOrb(orb);
      if(who != moPlayer) markOrb(itOrbEmpathy);
      if(attackMonster(mt, AF_NORMAL | AF_SIDE | AF_MSG, who)) 
        produceGhost(mt, m, who);
      }
    else if(mt->wall == waBigTree)
      mt->wall = waSmallTree;
    else if(mt->wall == waSmallTree)
      mt->wall = waNone;
    else if(mt->wall == waExplosiveBarrel)
      explodeBarrel(mt);
    }
  }

EX void sideAttack(cell *mf, int dir, eMonster who, int bonuskill) {

  int k = tkills();
  sideAttack(mf, dir, who, 1, itOrbSide1);
  sideAttack(mf, dir, who, 2, itOrbSide2);
  sideAttack(mf, dir, who, 3, itOrbSide3);

  if(who == moPlayer) {
    int kills = tkills() - k + bonuskill;
    if(kills >= 5) achievement_gain("MELEE5");
    }
  }

EX eMonster do_we_stab_a_friend(cell *mf, cell *mt, eMonster who) {
  eMonster m = moNone;
  do_swords(mf, mt, who, [&] (cell *c, int bb) { 
    if(!peace::on && canAttack(mt, who, c, c->monst, AF_SWORD) && c->monst && isFriendly(c)) m = c->monst;
    });

  for(int t=0; t<mf->type; t++) {
    cell *c = mf->move(t);
    if(!c) continue;
    
    bool stabthere = false;
    if(logical_adjacent(mt, who, c)) stabthere = true;
    
    if(stabthere && canAttack(mt,who,c,c->monst,AF_STAB) && isFriendly(c)) 
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
  else
    addMessage(XLAT(msg, who_kills_me));
  }

EX bool havePushConflict(cell *pushto, bool checkonly) {
  if(pushto && multi::activePlayers() > 1) {
    for(int i=0; i<multi::players; i++)  if(i != multi::cpid && multi::playerActive(i))
      if(multi::origpos[i] == pushto || multi::origtarget[i] == pushto) {
        if(!checkonly) 
          addMessage(XLAT("Cannot push into another player!"));
        return true;
        }
    for(int i=0; i<isize(stalemate::moves); i++)  {
      if(pushto == stalemate::moves[i].pushto) {
        if(!checkonly)
          addMessage(XLAT("Cannot push into the same location!"));
        return true;
        }
      }
    }
  return false;
  }

bool got_crossroads;

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
  if(from->land != to->land && tour::on && (phase & 2))
    tour::checkGoodLand(to->land);
#endif
  
  if(to->land ==laCrossroads4 && !got_crossroads && !geometry && (phase & 2) && !cheater) {
    achievement_gain("CR4");
    got_crossroads = true;
    chaosUnlocked = true;
    }

  if(isHaunted(from->land) && !isHaunted(to->land) && (phase & 2)) {
    updateHi(itLotus, truelotus = items[itLotus]);
    if(items[itLotus] >= 1) achievement_gain("LOTUS1");
    if(items[itLotus] >= (big_unlock ? 25 : 10)) achievement_gain("LOTUS2");
    if(items[itLotus] >= (big_unlock ? 50 : 25)) achievement_gain("LOTUS3");
    if(items[itLotus] >= 50 && !big_unlock) achievement_gain("LOTUS4");
    achievement_final(false);
    }
  
  if(geometry == gNormal && celldist(to) == 0 && !usedSafety && gold() >= 100 && (phase & 2))
    achievement_gain("COMEBACK");
  
  bool tortoiseOK = 
    to->land == from->land || to->land == laTortoise ||
    (to->land == laDragon && from->land != laTortoise) || 
    chaosmode;
  
  if(tortoise::seek() && !from->item && !tortoiseOK && passable(from, NULL, 0) && (phase & 2)) {
    from->item = itBabyTortoise;
    tortoise::babymap[from] = tortoise::seekbits;
    addMessage(XLAT("You leave %the1.", itBabyTortoise));
    items[itBabyTortoise]--;
    }
  }

}