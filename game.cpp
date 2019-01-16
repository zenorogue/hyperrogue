// Hyperbolic Rogue
// main game routines: movement etc.

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

int lastsafety;
int mutantphase;
int turncount;
int rosewave, rosephase;
int avengers, mirrorspirits, wandering_jiangshi, jiangshi_on_screen;

int gamerange_bonus = 0;
int gamerange() { return getDistLimit() + gamerange_bonus; }

cell *lastmove;
enum eLastmovetype {lmSkip, lmMove, lmAttack, lmSpecial, lmPush, lmTree};
eLastmovetype lastmovetype;

bool hauntedWarning;
bool survivalist;

bool hardcore = false;
int hardcoreAt;

set<int> snaketypes;

flagtype havewhat, hadwhat;

#define HF_BUG        Flag(0)
#define HF_EARTH      Flag(1)
#define HF_BIRD       Flag(2)
#define HF_LEADER     Flag(3)
#define HF_HEX        Flag(4)
#define HF_WHIRLPOOL  Flag(5)
#define HF_WATER      Flag(6)
#define HF_AIR        Flag(7)
#define HF_MUTANT     Flag(8)
#define HF_OUTLAW     Flag(9)
#define HF_WHIRLWIND  Flag(10)
#define HF_ROSE       Flag(11)
#define HF_DRAGON     Flag(12)
#define HF_KRAKEN     Flag(13)
#define HF_SHARK      Flag(14)
#define HF_BATS       Flag(15)
#define HF_REPTILE    Flag(16)
#define HF_EAGLES     Flag(17)
#define HF_SLOW       Flag(18)
#define HF_FAST       Flag(19)
#define HF_WARP       Flag(20)
#define HF_MOUSE      Flag(21)
#define HF_RIVER      Flag(22)
#define HF_MIRROR     Flag(23)
#define HF_VOID       Flag(24)
#define HF_HUNTER     Flag(25)
#define HF_FAILED_AMBUSH     Flag(26)
#define HF_MAGNET     Flag(27)
#define HF_HEXD       Flag(28)
#define HF_ALT        Flag(29)
#define HF_MONK       Flag(30)


bool seenSevenMines = false;

bool pureHardcore() { return hardcore && hardcoreAt < PUREHARDCORE_LEVEL; }

bool canmove = true;

int sagephase = 0;

// number of Grails collected, to show you as a knight
int knighted = 0;

bool usedSafety = false;
eLand safetyland;
int safetyseed;

int showid = 0;

bool invismove = false, invisfish = false; // last move was invisible [due to Fish]

int noiseuntil; // noise until the given turn

void createNoise(int t) { 
  noiseuntil = max(noiseuntil, turncount+t);
  invismove = false;
  if(shmup::on) shmup::visibleFor(100 * t);
  }

int currentLocalTreasure;

bool landvisited[landtypes];

extern void DEBT(const char *buf);

#define DEBT(x) DEBB(DF_TURN, (debugfile,"%s\n", x))

bool eq(short a, short b) { return a==b; }

// game state
array<int, ittypes> items;
array<int, motypes> kills;

int explore[10], exploreland[10][landtypes], landcount[landtypes];
map<modecode_t, array<int, ittypes> > hiitems;
bool orbused[ittypes], lastorbused[ittypes];
bool playermoved = true;  // center on the PC?
bool flipplayer = true;   // flip the player image after move, do not flip after attack
int  cheater = 0;         // did the player cheat?

int anthraxBonus = 0;     // for using Safety in tactical Camelot

vector<cell*> dcal;   // queue for cpdist
vector<cell*> pathq;  // queue for pathdist

vector<cell*> offscreen; // offscreen cells to take care off

vector<cell*> pathqm; // list of monsters to move (pathq restriced to monsters)
             
vector<cell*> targets; // list of monster targets

// monsters of specific types to move
vector<cell*> worms, ivies, ghosts, golems, hexsnakes;

vector<cell*> temps;  // temporary changes during bfs
vector<eMonster> tempval;  // restore temps

// a bit nicer way of DFS
vector<int> reachedfrom;

// additional direction information for BFS algorithms
// it remembers from where we have got to this location
// the opposite cell will be added to the queue first,
// which helps the AI
vector<cell*> movesofgood[8];

int first7;           // the position of the first monster at distance 7 in dcal

cellwalker cwt;       // single player character position

inline cell*& singlepos() { return cwt.at; }
inline bool singleused() { return !(shmup::on || multi::players > 1); }

// the main random number generator for the game
// all the random calls related to the game mechanics (land generation, AI...) should use hrngen
// random calls not related to the game mechanics (graphical effects) should not use hrngen
// this ensures that the game should unfold exactly the same if given the same seed and the same input

std::mt19937 hrngen;

void shrand(int i) {
  hrngen.seed(i);
  }

int hrandpos() { return hrngen() & HRANDMAX; }

// using our own implementations rather than ones from <random>,
// to make sure that they return the same values on different compilers

int hrand(int i) { 
  unsigned d = hrngen() - hrngen.min();
  long long m = (long long) (hrngen.max() - hrngen.min()) + 1;
  m /= i;
  d /= m;
  if(d < (unsigned) i) return d;
  return hrand(i);
  }

ld hrandf() { 
  return (hrngen() - hrngen.min()) / (hrngen.max() + 1.0 - hrngen.min());
  }

int hrandstate() {
  std::mt19937 r2 = hrngen;
  return r2() & HRANDMAX;
  }

void initcell(cell *c) {
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
  }

bool doesnotFall(cell *c) {
  if(c->wall == waChasm) return false;
  else if(cellUnstable(c)) {
    fallingFloorAnimation(c);
    c->wall = waChasm;
    return false;
    }
  return true;
  }

bool doesFall(cell *c) { return !doesnotFall(c); }

bool doesFallSound(cell *c) { 
  if(c->land != laMotion && c->land != laZebra)
    playSound(c, "trapdoor");  
  return !doesnotFall(c); 
  }

bool itemHidden(cell *c) {
  return isWatery(c) && !(shmup::on && shmup::boatAt(c));
  }

bool playerInWater() { 
  for(int i=0; i<numplayers(); i++) 
    if(multi::playerActive(i) && isWatery(playerpos(i)) && !playerInBoat(i)) 
      return true;
  return false;
  }

bool itemHiddenFromSight(cell *c) {
  return isWatery(c) && !items[itOrbInvis] && !(items[itOrbFish] && playerInWater())
    && !(shmup::on && shmup::boatAt(c));
  }

int numplayers() {
  return multi::players;
  }

cell *playerpos(int i) {
  if(shmup::on) return shmup::playerpos(i);
  if(multi::players > 1) return multi::player[i].at;
  return singlepos();
  }

bool allPlayersInBoats() {
  for(int i=0; i<numplayers(); i++) 
    if(multi::playerActive(i) && playerpos(i)->wall != waBoat) return true;
  return false;
  }

int whichPlayerOn(cell *c) {
  if(singleused()) return c == singlepos() ? 0 : -1;
  for(int i=0; i<numplayers(); i++) 
    if(playerpos(i) == c) return i;
  return -1;
  }

bool isPlayerOn(cell *c) {
  return whichPlayerOn(c) >= 0;
  }

bool isPlayerInBoatOn(cell *c, int i) {
  return
    (playerpos(i) == c && (
      c->wall == waBoat || c->wall == waStrandedBoat || (shmup::on && shmup::playerInBoat(i))
      ));
  }

bool playerInBoat(int i) {
  return isPlayerInBoatOn(playerpos(i), i);
  }

bool isPlayerInBoatOn(cell *c) {
  for(int i=0; i<numplayers(); i++) if(isPlayerInBoatOn(c, i)) return true;
  return false;
  }

void destroyBoats(cell *c, cell *c2, bool strandedToo) {
  if(c->wall == waBoat) placeWater(c, c2);
  if(strandedToo && c->wall == waStrandedBoat) c->wall = waNone;
  shmup::destroyBoats(c);
  }

bool playerInPower() {
  if(singleused()) 
    return singlepos()->land == laPower || singlepos()->land == laHalloween;
  for(int i=0; i<numplayers(); i++) 
    if(multi::playerActive(i) && (playerpos(i)->land == laPower || playerpos(i)->land == laHalloween))
      return true;
  return false;
  }

eItem localTreasureType() {
  lastland = singlepos()->land;
  return treasureType(lastland);
  }

void countLocalTreasure() {
  eItem i = localTreasureType();
  currentLocalTreasure = i ? items[i] : 0;
  if(i != itHyperstone) for(int i=0; i<isize(dcal); i++) {
    cell *c2 = dcal[i];
    if(c2->cpdist > 3) break;
    eItem i2 = treasureType(c2->land);
    if(i2 && items[i2] < currentLocalTreasure)
      currentLocalTreasure = items[i2];
    }
  }

int gold(int no) {
  int i = 0;
  if(!(no & NO_YENDOR)) i += items[itOrbYendor] * 50;
  if(!(no & NO_GRAIL)) i += items[itHolyGrail] * 10;
  if(!(no & NO_LOVE)) {
    bool love = items[itOrbLove];
#if CAP_INV
    if(inv::on && inv::remaining[itOrbLove])
      love = true;
#endif
#if CAP_DAILY
    if(daily::on) love = false;
#endif
    if(love) i += 30;
    }
  
  if(!(no & NO_TREASURE)) 
    for(int t=0; t<ittypes; t++) 
      if(itemclass(eItem(t)) == IC_TREASURE)
        i += items[t];
  return i;
  }

int maxgold() {
  int mg = 0;
  for(int i=0; i<ittypes; i++) 
    if(itemclass(eItem(i)) == IC_TREASURE && items[i] > mg) 
      mg = items[i];
  return mg;
  }

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
    NULL
    };

int tkills() {
  int res = 0;
  for(int i=0; killtable[i]; i++) res += killtable[i][0];
  return res;
  }

int killtypes() {
  int res = 0;
  for(int i=0; killtable[i]; i++) if(killtable[i][0]) res++;
  return res;
  }

bool isWarped(cell *c) {
  return isWarped(c->land) || (!inmirrororwall(c->land) && (items[itOrb37] && c->cpdist <= 4));
  }

bool nonAdjacent(cell *c, cell *c2) {
  if(isWarped(c) && isWarped(c2) && warptype(c) == warptype(c2)) {
    /* int i = neighborId(c, c2);
    cell *c3 = c->modmove(i+1), *c4 = c->modmove(i-1);
    if(c3 && !isTrihepta(c3)) return false;
    if(c4 && !isTrihepta(c4)) return false; */
    return true;
    }
  return false;
  }

bool nonAdjacentPlayer(cell *c, cell *c2) {
  return nonAdjacent(c, c2) && !markOrb(itOrb37);
  }

bool thruVine(cell *c, cell *c2) {
  return (cellHalfvine(c) && c2->wall == c->wall && c2 != c);
  // ((c->wall == waFloorC || c->wall == waFloorD) && c2->wall == c->wall && !c2->item && !c->item);
  }

// === MOVEMENT FUNCTIONS ===

// w = from->move(d)
bool againstCurrent(cell *w, cell *from) {
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

bool boatGoesThrough(cell *c) {
  if(isGravityLand(c->land)) return false;
  return
    (c->wall == waNone && c->land != laMotion && c->land != laZebra && c->land != laReptile) || 
    isAlchAny(c) || 
    c->wall == waCavefloor || c->wall == waFrozenLake || isReptile(c->wall) ||
    c->wall == waDeadfloor || c->wall == waCIsland || c->wall == waCIsland2 ||
    c->wall == waMineUnknown || c->wall == waMineMine || c->wall == waMineOpen ||
    c->wall == waBonfireOff || c->wall == waFire || c->wall == waPartialFire ||
    c->wall == waArrowTrap;
  }

void placeWater(cell *c, cell *c2) {
  destroyTrapsOn(c);
  if(isWatery(c)) ;
  else if(c2 && isAlchAny(c2))
    c->wall = c2->wall;
  else if(isIcyLand(c))
    c->wall = waLake;
  else
    c->wall = waSea;
  // destroy the ancient treasure!
  if(c->item == itBarrow) c->item = itNone;
  }

int incline(cell *cfrom, cell *cto) {
  return snakelevel(cto) - snakelevel(cfrom);
  }

#define F(x) checkflags(flags,x)

bool checkflags(flagtype flags, int x) {
  if(flags & x) return true;
  if(flags & P_ISPLAYER) {
    if((x & P_WINTER)    && markOrb(itOrbWinter)) return true;
    if((x & P_IGNORE37)  && markOrb(itOrb37)) return true;
    if((x & P_FISH)      && markOrb(itOrbFish)) return true;
    if((x & P_MARKWATER) && markOrb(itOrbWater)) return true;
    if((x & P_AETHER)    && markOrb2(itOrbAether) && !(flags&P_NOAETHER)) return true;
    }
  if(flags & P_ISFRIEND) if(items[itOrbEmpathy]) 
    if(checkflags(flags ^ P_ISPLAYER ^ P_ISFRIEND, x) && markOrb(itOrbEmpathy))
      return true;
  return false;
  }

bool strictlyAgainstGravity(cell *w, cell *from, bool revdir, flagtype flags) {
  return
    cellEdgeUnstable(w, flags) && cellEdgeUnstable(from, flags) && 
    !(shmup::on && from == w) && gravityLevel(w) != gravityLevel(from) + (revdir?1:-1);
  }

bool passable(cell *w, cell *from, flagtype flags) {
  bool revdir = (flags&P_REVDIR);
  bool vrevdir = revdir ^ bool(flags&P_VOID);

  if(from && from != w && nonAdjacent(from, w) && !F(P_IGNORE37 | P_BULLET)) return false;
  
  for(int i=0; i<numplayers(); i++) {
    cell *pp = playerpos(i);
    if(!pp) continue;
    if(w == pp && F(P_ONPLAYER)) return true;
    if(from == pp && F(P_ONPLAYER) && F(P_REVDIR)) return true;

    if(from && !((flags & P_ISPLAYER) && pp->monst)) {
      int i = vrevdir ? incline(w, from) : incline(from, w);
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
    }

  if(from && strictlyAgainstGravity(w, from, vrevdir, flags)
    && !F(P_GRAVITY | P_BLOW | P_JUMP1 | P_JUMP2 | P_FLYING | P_BULLET | P_AETHER)
    ) return false;
  
  if(from && (vrevdir ? againstWind(from,w) : againstWind(w, from)) && !F(P_WIND | P_BLOW | P_JUMP1 | P_JUMP2 | P_BULLET | P_AETHER)) return false;
  
  if(revdir && from && w->monst && passable(from, w, flags &~ (P_REVDIR|P_MONSTER)))
    return true;
  
  if(!shmup::on && sword::at(w, flags & P_ISPLAYER) && !F(P_DEADLY | P_BULLET | P_ROSE))
    return false;

  bool alch1 = w->wall == waFloorA && from && from->wall == waFloorB && !w->item && !from->item;
  alch1 |= w->wall == waFloorB && from && from->wall == waFloorA && !w->item && !from->item;
  
  if(alch1) {
    bool alchok = F(P_JUMP1 | P_JUMP2 | P_FLYING | P_TELE | P_BLOW | P_AETHER | P_BULLET) 
      && !F(P_ROSE);
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
    if(from && from->wall == waBoat && isWateryOrBoat(w) && !againstCurrent(w, from))
      return true;
    
    if(from && isWatery(from) && w->wall == waBoat && F(P_CHAIN))
      return true;

    if(from && isWatery(from) && isWatery(w) && F(P_CHAIN) && !againstCurrent(w, from))
      return true;

    if(w->wall == waBigStatue && from && canPushStatueOn(from)) return true;
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
    if(!F(P_AETHER | P_WINTER | P_BLOW | P_JUMP1 | P_BULLET | P_DEADLY)) return false;
    }
    
  if(isWatery(w)) {
    if(from && from->wall == waBoat && F(P_USEBOAT) && 
      (!againstCurrent(w, from) || F(P_MARKWATER))) ;
    else if(!F(P_AETHER | P_FISH | P_FLYING | P_BLOW | P_JUMP1 | P_BULLET | P_DEADLY | P_REPTILE)) return false;
    }
  if(isChasmy(w)) {
    if(!F(P_AETHER | P_FLYING | P_BLOW | P_JUMP1 | P_BULLET | P_DEADLY | P_REPTILE)) return false;  
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

int airdir;
vector<pair<cell*, int> > airmap;

int airdist(cell *c) {
  if(!(havewhat & HF_AIR)) return 3;
  vector<pair<cell*, int> >::iterator it = 
    lower_bound(airmap.begin(), airmap.end(), make_pair(c,0));
  if(it != airmap.end() && it->first == c) return it->second;
  return 3;
  }

void calcAirdir(cell *c) {
  if(!c || c->monst == moAirElemental || !passable(c, NULL, P_BLOW))
    return;
  for(int i=0; i<c->type; i++) {
    cell *c2 = c->move(i);
    if(c2 && c2->monst == moAirElemental) {
      airdir = c->c.spin(i) * S42 / c2->type;
      return;
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
        airdir = c2->c.spin(i) * S42 / c3->type;
        return;
        }
      }
    }
  return;
  }

bool againstWind(cell *cto, cell *cfrom) {
  if(!cfrom || !cto) return false;
  int dcto = airdist(cto), dcfrom = airdist(cfrom);
  if(dcto < dcfrom) return true;
  if(cfrom->land == laBlizzard && !shmup::on && cto->land == laBlizzard && dcto == 3 && dcfrom == 3) {
    char vfrom = windmap::at(cfrom);
    char vto = windmap::at(cto);
    int z = (vfrom-vto) & 255;
    if(z >= windmap::NOWINDBELOW && z < windmap::NOWINDFROM)
      return true;
    }
  whirlwind::calcdirs(cfrom);
  int d = neighborId(cfrom, cto);
  if(whirlwind::winddir(d) == -1) return true;
  return false;
  }

bool ghostmove(eMonster m, cell* to, cell* from) {
  if(!isGhost(m) && nonAdjacent(to, from)) return false;
  if(sword::at(to, 0)) return false;
  if(!shmup::on && isPlayerOn(to)) return false;
  if(to->monst && !(to->monst == moTentacletail && isGhost(m) && m != moFriendlyGhost)
    && !(to->monst == moTortoise && isGhost(m) && m != moFriendlyGhost))
    return false;
  if((m == moWitchGhost || m == moWitchWinter) && to->land != laPower)
    return false;
  if(isGhost(m))
    for(int i=0; i<to->type; i++) {
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
  int u = dirfromto(c, w);
  if(nonAdjacent(w,c)) return false;
  if(isPlayerOn(w)) return true;
  int group = slimegroup(c);
  if(!group) return false;
  int ogroup = slimegroup(w);
  if(!ogroup) return false;
  bool hv = (group == ogroup);

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
  if(!isWatery(w)) return false;
  if(sword::at(w, 0)) return false;
  
  // don't go against the current
  if(isWateryOrBoat(w) && isWateryOrBoat(c)) 
    return !againstCurrent(w, c);

  return true;
  }

bool canPushStatueOn(cell *c) {
  return passable(c, NULL, P_MONSTER) && c->wall != waBoat && !snakelevel(c) &&
    !isWorm(c->monst) && !isReptile(c->wall) && !peace::on;
  }

void moveBoat(cell *to, cell *from, int direction_hint) {
  eWall x = to->wall; to->wall = from->wall; from->wall = x;
  to->mondir = neighborId(to, from);
  moveItem(from, to, false);
  animateMovement(from, to, LAYER_BOAT, direction_hint);
  }

void moveBoatIfUsingOne(cell *to, cell *from, int direction_hint) {
  if(from->wall == waBoat && isWatery(to)) moveBoat(to, from, direction_hint);
  else if(from->wall == waBoat && boatGoesThrough(to) && isFriendly(to) && markEmpathy(itOrbWater)) {
    placeWater(to, from);
    moveBoat(to, from, direction_hint);
    }
  }

eMonster otherpole(eMonster m) {
  return eMonster(m ^ moNorthPole ^ moSouthPole);
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

bool againstPair(cell *c1, cell *c2, eMonster m) { // (from, to)
  if(c1->monst == m && !isNeighbor(c2, c1->move(c1->mondir)))
    return true;
  return false;
  }

bool notNearItem(cell *c) {
  forCellCM(c2, c) if(c2->item) return false;
  return true;
  }

bool passable_for(eMonster m, cell *w, cell *from, flagtype extra) {
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
  if(normalMover(m) || isBug(m) || isDemon(m) || m == moHerdBull || m == moMimic) {
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
    return ghostmove(m, w, from);
    // for the purpose of Shmup this is correct
  if(m == moTameBomberbird)
    return passable(w, from, extra | P_FLYING | P_ISFRIEND);
  if(m == moHexSnake)
    return !pseudohept(w) && passable(w, from, extra|P_WIND|P_FISH);
  if(isBird(m))
    return passable(w, from, extra | P_FLYING);
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

eMonster movegroup(eMonster m) {
  if(isWitch(m) || m == moEvilGolem) {
    if(m == moWitchGhost) return moWitchGhost;
    if(m == moWitchWinter) return moWitchWinter;
    return moWitch;
    }
  // if(isMagneticPole(m)) return m;
  if(normalMover(m)) return moYeti;
  if(m == moShark || m == moCShark) return moShark;
  if(isSlimeMover(m)) return moSlime;
  if(m == moEarthElemental) return moEarthElemental;
  if(isLeader(m)) return moPirate;
  if(m == moButterfly) return moButterfly;
  if(isAngryBird(m)) return moEagle;
  if(isBird(m)) return moTameBomberbird;
  if(m == moReptile) return moReptile;
  if(m == moGhost) return moGhost;
  if(m == moFriendlyGhost) return moFriendlyGhost;
  if(m == moGreaterShark) return moGreaterShark;
  if(m == moWolf) return moWolf;
  if(isDemon(m)) return moLesser;
  if(isDragon(m)) return moDragonHead;
  if(isBug(m)) return m;
  if(m == moWaterElemental) return moWaterElemental;
  if(m == moAirElemental) return moAirElemental;
  if(isBull(m)) return moRagingBull;
  if(m == moVoidBeast) return moVoidBeast;
  if(m == moAltDemon || m == moHexDemon || m == moMonk)
    return m;
  return moNone;
  }

void useup(cell *c) {
  c->wparam--;
  if(c->wparam == 0) {
    drawParticles(c, c->wall == waFire ? 0xC00000 : winf[c->wall].color, 10, 50);
    if(c->wall == waTempFloor)
      c->wall = waChasm;
    else if(c->wall == waTempBridge || c->wall == waTempBridgeBlocked || c->wall == waBurningDock)
      placeWater(c, c);
    else 
      c->wall = c->land == laCaribbean ? waCIsland2 : waNone;
    }
  }

int realstuntime(cell *c) {
  if(isMutantIvy(c)) return (c->stuntime - mutantphase) & 15;
  return c->stuntime;
  }

bool childbug = false;

// is w killed if killed is killed?
bool isChild(cell *w, cell *killed) {
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

bool logical_adjacent(cell *c1, eMonster m1, cell *c2) {
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

bool canAttack(cell *c1, eMonster m1, cell *c2, eMonster m2, flagtype flags) {

  // cannot eat worms
  if((flags & AF_EAT) && isWorm(m2)) return false;
  
  if(m1 == passive_switch || m2 == passive_switch) return false;
  
  if((flags & AF_GETPLAYER) && isPlayerOn(c2)) m2 = moPlayer;
  
  if(!m2) return false;
  
  if(m2 == moPlayer && peace::on) return false;
  
  if((flags & AF_MUSTKILL) && attackJustStuns(c2, flags))
    return false;
  
  if((flags & AF_ONLY_FRIEND) && m2 != moPlayer && !isFriendly(c2)) return false;
  if((flags & AF_ONLY_FBUG)   && m2 != moPlayer && !isFriendlyOrBug(c2)) return false;
  if((flags & AF_ONLY_ENEMY) && (m2 == moPlayer || isFriendlyOrBug(c2))) return false;
  
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
    
  if(!(flags & (AF_GUN | AF_SWORD | AF_SWORD_INTO | AF_MAGIC)))
    if(c1 != c2 && !logical_adjacent(c1, m1, c2)) return false;

  if(!(flags & (AF_LANCE | AF_STAB | AF_BACK | AF_APPROACH | AF_GUN | AF_MAGIC)))
    if(c1 && c2 && againstRose(c1, c2) && !ignoresSmell(m1))
      return false;
  
  if(m2 == moShadow && !(flags & AF_SWORD)) return false;
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

  if(m2 == moVizier && c2->hitpoints > 1 && !c2->stuntime)
    if(!(flags & (AF_MAGIC | AF_TOUGH | AF_EAT | AF_HORNS | AF_LANCE | AF_BACK | AF_FAST | AF_BULL | AF_CRUSH))) return false;
                       
  return true;
  }

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

bool stalemate::isKilled(cell *w) {
  for(int f=0; f<isize(moves); f++)
    if(moves[f].isKilled(w)) return true;
  
  return false;
  };

bool isNeighbor(cell *c1, cell *c2) {
  for(int i=0; i<c1->type; i++) if(c1->move(i) == c2) return true;
  return false;
  }

bool isNeighborCM(cell *c1, cell *c2) {
  for(int i=0; i<c1->type; i++) if(createMov(c1, i) == c2) return true;
  return false;
  }

int neighborId(cell *ofWhat, cell *whichOne) {
  for(int i=0; i<ofWhat->type; i++) if(ofWhat->move(i) == whichOne) return i;
  return -1;
  }

// how many monsters are near
eMonster who_kills_me;

bool flashWouldKill(cell *c, flagtype extra) {
  for(int t=0; t<c->type; t++) {
    cell *c2 = c->move(t);
    for(int u=0; u<c2->type; u++) {
      cell *c3 = c2->move(u);
      if(isWorm(c3)) continue; // immune to Flash
      if(c3->monst == moEvilGolem) continue; // evil golems don't count
      if(c3 != c && (c3->monst || isPlayerOn(c3)) && !stalemate::isKilled(c3)) {
        bool b = canAttack(NULL, moWitchFlash, c3, c3->monst, AF_MAGIC | extra);
        if(b) return true;
        }
      }
    }
  return false;
  }

vector<cell*> gun_targets(cell *c) {
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

namespace stalemate {
  vector<stalemate1> moves;
  bool  nextturn;

  bool isMoveto(cell *c) {
    for(int i=0; i<isize(moves); i++) if(moves[i].moveto == c) return true;
    return false;
    }

  bool isKilledDirectlyAt(cell *c) {
    for(int i=0; i<isize(moves); i++) if(moves[i].killed == c) return true;
    return false;
    }
  
  bool isPushto(cell *c) {
    for(int i=0; i<isize(moves); i++) if(moves[i].pushto == c) return true;
    return false;
    }
  }

bool onboat(stalemate1& sm) {
  cell *c = sm.moveto;
  cell *cf = sm.comefrom;
  return (c->wall == waBoat) || (cf->wall == waBoat && c->wall == waSea);
  }
  
bool krakensafe(cell *c) {
  return items[itOrbFish] || items[itOrbAether] || 
    (c->item == itOrbFish && c->wall == waBoat) || 
    (c->item == itOrbAether && c->wall == waBoat);
  }

eMonster active_switch() {
  return eMonster(passive_switch ^ moSwitch1 ^ moSwitch2);
  }

vector<cell*> crush_now, crush_next;
  
bool monstersnear(stalemate1& sm) {

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
      if(c3->stuntime) continue;
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

namespace multi { bool aftermove; }

bool monstersnear2();

int lastkills;

bool multimove() {
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

namespace multi { bool checkonly = false; }

bool swordConflict(const stalemate1& sm1, const stalemate1& sm2) {
  if(items[itOrbSword] || items[itOrbSword2])
  for(int b=0; b<2; b++)
    if(sm1.comefrom == sm2.swordlast[b] || sm1.comefrom == sm2.swordtransit[b] || sm1.comefrom == sm2.swordnext[b])
    if(sm1.moveto == sm2.swordlast[b] || sm1.moveto == sm2.swordtransit[b] || sm1.moveto == sm2.swordnext[b])
      return true;
  return false;
  }

bool monstersnear2() {
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

bool monstersnear(cell *c, cell *nocount, eMonster who, cell *pushto, cell *comefrom) {

  if(peace::on) return 0; // you are safe

  stalemate1 sm(who, c, nocount, pushto, comefrom);
  
  if(who == moPlayer) for(int b=0; b<2; b++) sm.swordlast[b] = sword::pos(multi::cpid, b);
  
  cell *none = NULL;
  cell **wcw = &cwt.at;
  if(who != moPlayer) wcw = &none;
  else if(multi::players > 1) wcw = &multi::player[multi::cpid].at;
  
  dynamicval<cell*> x5(*wcw, c);
  dynamicval<bool> x6(stalemate::nextturn, true);
  dynamicval<int> x7(sword::angle[multi::cpid], 
    who == moPlayer ? sword::shift(comefrom, c, sword::angle[multi::cpid]) :
    sword::angle[multi::cpid]);
  
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
  else if(who == moPlayer && c->wall == waThumperOn) {
    c->wall = waNone;
    b = monstersnear2();
    c->wall = waThumperOn;
    }
  else {
    b = monstersnear2();
    }
  stalemate::moves.pop_back();
  return b;
  }

bool petrify(cell *c, eWall walltype, eMonster m) {
  destroyHalfvine(c);
  destroyTrapsOn(c);
  playSound(c, "die-troll");
  
  if(walltype == waIcewall && !isIcyLand(c->land))
    return false;
  
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
        
void killIvy(cell *c, eMonster who) {
  if(c->monst == moIvyDead) return;
  if(checkOrb(who, itOrbStone)) petrify(c, waPetrified, c->monst);
  c->monst = moIvyDead; // NEWYEARFIX
  for(int i=0; i<c->type; i++) if(c->move(i))
    if(isIvy(c->move(i)) && c->move(i)->mondir == c->c.spin(i))
      killIvy(c->move(i), who);
  }

void prespill(cell* c, eWall t, int rad, cell *from) {
  if(againstWind(c, from)) return;
  // these monsters block spilling
  if(c->monst == moSeep || c->monst == moVineSpirit || c->monst == moShark ||
    c->monst == moGreaterShark || c->monst == moParrot || c->monst == moCShark)
    return;
  // turn statues into Slimes!
  if(c->wall == waBigStatue && t != waNone) {
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
  destroyTrapsOn(c);
  // these walls block further spilling
  if(c->wall == waCavewall || cellUnstable(c) || c->wall == waSulphur ||
    c->wall == waSulphurC || c->wall == waLake || c->wall == waChasm ||
    c->wall == waBigTree || c->wall == waSmallTree || c->wall == waTemporary ||
    c->wall == waVinePlant || isFire(c) || c->wall == waBonfireOff || c->wall == waVineHalfA || c->wall == waVineHalfB ||
    c->wall == waCamelotMoat || c->wall == waSea || c->wall == waCTree ||
    c->wall == waRubble || c->wall == waGargoyleFloor || c->wall == waGargoyle ||
    c->wall == waRose || c->wall == waPetrified || c->wall == waPetrifiedBridge || c->wall == waRuinWall)
      t = waTemporary;

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
  c->wall = t;
  // destroy items...
  c->item = itNone;
  // block spill
  if(t == waTemporary) return;
  // cwt.at->item = itNone;
  if(rad) for(int i=0; i<c->type; i++) if(c->move(i))
    prespill(c->move(i), t, rad-1, c);
  }

void spillfix(cell* c, eWall t, int rad) {
  if(c->wall == waTemporary) c->wall = t;
  if(rad) for(int i=0; i<c->type; i++) if(c->move(i))
    spillfix(c->move(i), t, rad-1);
  }

void spill(cell* c, eWall t, int rad) {
  prespill(c,t,rad, c); spillfix(c,t,rad);
  }

void degradeDemons() {
  addMessage(XLAT("You feel more experienced in demon fighting!"));
  int dcs = isize(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->monst == moGreaterM || c->monst == moGreater)
      achievement_gain("DEMONSLAYER");
    if(c->monst == moGreaterM) c->monst = moLesserM;
    if(c->monst == moGreater) c->monst = moLesser;
    shmup::degradeDemons();
    }  
  }

void ivynext(cell *c);

bool earthFloor(cell *c) {
  if(c->monst) return false;
  if(c->wall == waDeadwall) { c->wall = waDeadfloor; return true; }
  if(c->wall == waDune) { c->wall = waNone; return true; }
  if(c->wall == waStone && c->land != laTerracotta) { c->wall = waNone; return true; }
  if(c->wall == waAncientGrave || c->wall == waFreshGrave || c->wall == waRuinWall) {
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
  return false;
  }

bool earthWall(cell *c) {
  if(c->wall == waDeadfloor || c->wall == waDeadfloor2 || c->wall == waEarthD) {
    c->item = itNone;
    c->wall = waDeadwall;
    return true;
    }
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
  if(c->wall == waCIsland || c->wall == waCIsland2 || (c->wall == waNone && c->land == laOcean)) {
    c->item = itNone;
    c->wall = waSea;
    if(c->land == laOcean) c->landparam = 40;
    return true;
    }
  return false;
  }

bool snakepile(cell *c, eMonster m) {
  if(c->item && c->wall != waRed3) c->item = itNone;
  if(c->wall == waRed1 || c->wall == waOpenGate) c->wall = waRed2;
  else if(c->wall == waRed2) c->wall = waRed3;
  else if(doesFall(c)) return false;
  else if((c->wall == waSea && c->land == laLivefjord))
    c->wall = waNone;
  else if((c->wall == waSea && isWarped(c->land)))
    c->wall = waNone;
  else if(isGravityLand(c->land)) {
    if(m == moHexSnake)
      c->wall = waPlatform;
    else
      c->wall = waDeadTroll2;
    }
  else if(c->wall == waNone || isAlchAny(c) ||
    c->wall == waCIsland || c->wall == waCIsland2 ||
    (c->wall == waSea && c->land == laOcean) ||
    c->wall == waOpenPlate || c->wall == waClosePlate ||
    c->wall == waMineUnknown || c->wall == waMineOpen || isReptile(c->wall)) {
    if(isReptile(c->wall)) kills[moReptile]++;
    c->wall = waRed1;
    if(m == moDarkTroll) c->wall = waDeadfloor2;
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
  else if(c->wall == waSea && c->land == laCaribbean) c->wall = waCIsland;
  else if(c->wall == waSea && c->land == laWhirlpool) return false;
  else if(c->wall == waSea) c->wall = waNone;
  else if(isWateryOrBoat(c) || c->wall == waFrozenLake) c->wall = waNone;
  else if(isWateryOrBoat(c) || c->wall == waFrozenLake) c->wall = waNone;
  else if(cellHalfvine(c)) {
    destroyHalfvine(c, waRed1);
    if(c->wall == waRed1 && m == moDarkTroll) c->wall = waDeadfloor2;
    }
  else return false;
  return true;
  }

bool makeflame(cell *c, int timeout, bool checkonly) {
  if(!checkonly) destroyTrapsOn(c);
  if(itemBurns(c->item)) {
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
      addMessage(XLAT("%the1 protects your boat!", itOrbWinter));
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
    }
  return true;
  }

void explodeMine(cell *c) {
  if(c->wall != waMineMine)
    return;
  
  playSound(c, "explosion");
  drawFireParticles(c, 30, 150);
  
  c->wall = waMineOpen;
  makeflame(c, 20, false);
  
  for(int i=0; i<c->type; i++) if(c->move(i)) {
    cell *c2 = c->move(i);
    destroyTrapsOn(c2);
    if(c2->wall == waRed2 || c2->wall == waRed3)
      c2->wall = waRed1;
    else if(c2->wall == waDeadTroll || c2->wall == waDeadTroll2 || c2->wall == waPetrified || c2->wall == waGargoyle) {
      c2->wall = waNone;
      makeflame(c2, 10, false);
      }
    else if(c2->wall == waPetrifiedBridge || c2->wall == waGargoyleBridge) {
      placeWater(c, c);
      }
    else if(c2->wall == waPalace || c2->wall == waOpenGate || c2->wall == waClosedGate ||
      c2->wall == waSandstone || c2->wall == waMetal || c2->wall == waSaloon || c2->wall == waRuinWall) {
      c2->wall = waNone;
      makeflame(c2, 10, false);
      }
    else if(c2->wall == waTower)
      c2->wall = waRubble;
    else if(c2->wall == waBarrowWall)
      c2->wall = waBarrowDig;
    else if(c2->wall == waBarrowDig)
      c2->wall = waNone;
    else makeflame(c2, 20, false);
    }
  }

bool mayExplodeMine(cell *c, eMonster who) {
  if(c->wall != waMineMine) return false;
  if(ignoresPlates(who)) return false;
  explodeMine(c);
  return true;
  }

void stunMonster(cell *c2) {
  int newtime = (
    c2->monst == moFatGuard ? 2 : 
    c2->monst == moSkeleton && c2->land != laPalace && c2->land != laHalloween ? 7 :
    c2->monst == moTerraWarrior ? min(int(c2->stuntime + 8 - c2->hitpoints), 7) :
    isMetalBeast(c2->monst) ? 7 :
    c2->monst == moTortoise ? 7 :
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
    3);
  if(c2->monst != moSkeleton && !isMetalBeast(c2->monst) && c2->monst != moTortoise &&
    c2->monst != moReptile && c2->monst != moSalamander) {
    c2->hitpoints--;
    if(c2->monst == moPrincess)
      playSound(c2, princessgender() ? "hit-princess" : "hit-prince");
    } 
  if(c2->stuntime < newtime) c2->stuntime = newtime;
  if(isBull(c2->monst)) c2->mondir = NODIR;
  checkStunKill(c2);
  }

bool attackJustStuns(cell *c2, flagtype f) {
  if(f & AF_HORNS)
    return hornStuns(c2);
  else if((f & AF_SWORD) && c2->monst == moSkeleton)
    return false;
  else if(f & (AF_CRUSH | AF_MAGIC | AF_FALL | AF_EAT | AF_GUN))
    return false;
  else
    return isStunnable(c2->monst) && c2->hitpoints > 1;
  }
  
void moveEffect(cell *ct, cell *cf, eMonster m, int direction_hint);
  
void flameHalfvine(cell *c, int val) {
  if(itemBurns(c->item)) {
    addMessage(XLAT("%The1 burns!", c->item));
    c->item = itNone;
    }
  c->wall = waPartialFire;
  c->wparam = val;
  }

void minerEffect(cell *c) {
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

bool isRatling(eMonster m) {
  return m == moRatling || m == moRatlingAvenger;
  }

void killMutantIvy(cell *c, eMonster who) {
  if(checkOrb(who, itOrbStone)) petrify(c, waPetrified, moMutant);
  removeIvy(c);
  for(int i=0; i<c->type; i++)
    if(c->move(i)->mondir == c->c.spin(i) && (isMutantIvy(c->move(i)) || c->move(i)->monst == moFriendlyIvy))
      killMutantIvy(c->move(i), who);
  }

void killMonster(cell *c, eMonster who, flagtype deathflags) {
  eMonster m = c->monst;
  DEBB(DF_TURN, (debugfile,"killmonster %s", dnameof(m)));
  
  if(!m) return;
  
  if(m == moKrakenH) return;
  if(m == moKrakenT) {
    if(c->hitpoints && m == moKrakenT) kills[moKrakenT]++;
    c->hitpoints = 0;
    c->stuntime = 1;
    cell *head = kraken::head(c);
    if(kraken::totalhp(head) == 0) kraken::kill(head, who);
    return;
    }
  
  if(isDragon(m)) {
    if(c->hitpoints && m != moDragonHead) kills[moDragonTail]++;
    c->hitpoints = 0;
    c->stuntime = 1;
    cell *head = dragon::findhead(c);
    if(dragon::totalhp(head) == 0) dragon::kill(head, who);
    }
  if(isWorm(c) && m != moTentacleGhost) return;
  
  bool fallanim = (deathflags & AF_FALL) &&  m != moMimic;

  int pcount = fallanim ? 0 : 16; 
  if(m == moShadow) return;

#ifdef HASLINEVIEW
  if(!isBug(m) && !isAnyIvy(m)) {
    conformal::killhistory.push_back(make_pair(c,m));
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
  if(!isBulletType(m)) kills[m]++;

  if(saved_tortoise_on(c)) c->item = itNone;

  if(!c->item) if(m == moButterfly && (deathflags & AF_BULL))
    c->item = itBull;
  
  if(isRatling(m) && c->wall == waBoat) {
    bool avenge = false;
    for(int i=0; i<c->type; i++) if(!isWarped(c->move(i)->land))
      avenge = true;
    if(avenge) { avengers += 2; }
    }
  
  if(m == moMirrorSpirit && who != moMimic && !(deathflags & (AF_MAGIC | AF_CRUSH))) {
    kills[m]--;
    mirrorspirits++;
    }
  
  if(isMutantIvy(m) || m == moFriendlyIvy) {
    pcount = 0;
    killMutantIvy(c, who);
    }
  
  if(m == moPrincess) {
    princess::info *i = princess::getPrincessInfo(c);
    if(i) {
      i->princess = NULL;
      if(i->bestdist == OUT_OF_PALACE) {
        items[itSavedPrincess]--;
        if(items[itSavedPrincess] < 0) {
          printf("princess below 0\n");
          items[itSavedPrincess] = 0;
          }
        if(items[itSavedPrincess] == 0 && !inv::on) {
          items[itOrbLove] = 0;
          princess::reviveAt = gold(NO_LOVE) + 20;
          }
        }
      if(princess::challenge) showMissionScreen();
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
    forCellEx(c2, c) heat::affect(c2, -.5);
    }
    
  if(m == moTroll) {
    petrify(c, waDeadTroll, m); pcount = 0;
    for(int i=0; i<c->type; i++) if(c->move(i)) {
      c->move(i)->item = itNone;
      if(c->move(i)->wall == waDeadwall || c->move(i)->wall == waDeadfloor2) c->move(i)->wall = waCavewall;
      if(c->move(i)->wall == waDeadfloor) c->move(i)->wall = waCavefloor;
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
    for(int i=0; i<c->type; i++) if(passable(c->move(i), c, P_MONSTER | P_MIRROR | P_CLIMBUP | P_CLIMBDOWN)) {
      destroyHalfvine(c->move(i));
      minerEffect(c->move(i));
      if(c->move(i)->monst == moSlime || c->move(i)->monst == moSlimeNextTurn)
        killMonster(c->move(i), who);
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
  if(isBird(m)) moveEffect(c, c, moDeadBird, -1);
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
      isHaunted(c->land)) && geometry != gCrystal) {
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
  if(isIvy(c)) {
    pcount = 0;
    eMonster m = c->monst;
    /*if((m == moIvyBranch || m == moIvyHead) && c->move(c->mondir)->monst == moIvyRoot)
      ivynext(c, moIvyNext); */
    killIvy(c, who);
    if(m == moIvyBranch || m == moIvyHead || m == moIvyNext) {
      int qty = 0;
      cell *c2 = c->move(c->mondir);
      for(int i=0; i<c2->type; i++)
        if(c2->move(i)->monst == moIvyWait && c2->move(i)->mondir == c2->c.spin(i))
          qty++;
      if(c->move(c->mondir)->monst == moIvyRoot || qty) {
        c->monst = moIvyNext;
        /* c->monst = moIvyHead;
        ivynext(c);
        if(c->monst == moIvyHead) c->monst = moIvyNext;
        else c->monst = moNone; */
        }
      else {
        c->move(c->mondir)->monst = moIvyHead;
        }
      }
    }
  else if(c->monst == moTentacleGhost)
    c->monst = moTentacletail;
  else c->monst = moNone;

  if(m == moPair && c->move(c->mondir)->monst == moPair)
    killMonster(c->move(c->mondir), who, deathflags);

  if(isMagneticPole(m) && c->move(c->mondir)->monst == otherpole(m))
    killMonster(c->move(c->mondir), who, deathflags);
  
  if(m == moEarthElemental) earthWall(c);
  if(m == moAlbatross && items[itOrbLuck]) 
    useupOrb(itOrbLuck, items[itOrbLuck] / 2);
  
  if(m == moAirElemental) {
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

void fightmessage(eMonster victim, eMonster attacker, bool stun, flagtype flags) {

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
        achievement_gain("GOBLINSWORD");
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

void fallMonster(cell *c, flagtype flags) {
  attackMonster(c, flags, moNone);
  }

bool notthateasy(eMonster m) {
  return 
    isMultitile(m) || isStunnable(m) || m == moDraugr;
  }

bool attackMonster(cell *c, flagtype flags, eMonster killer) {

  if((flags & AF_GETPLAYER) && isPlayerOn(c)) {
    killThePlayerAt(killer, c, flags);
    return true;
    }

  eMonster m = c->monst;
  int tk = tkills();
  
  int tkt = killtypes();
  
  bool dostun = attackJustStuns(c, flags);
  
  if((flags & AF_BULL) && c->monst == moVizier && c->hitpoints > 1) {
    dostun = true;
    c->stuntime = 2;
    }
  
  bool eu = elementalUnlocked();
  bool tu = trollUnlocked();
  
  if(flags & AF_MSG) fightmessage(m, killer, dostun, flags);
  if(dostun) 
    stunMonster(c);
  else 
    killMonster(c, killer, flags);

  if(peace::on) return false;
  
  int ntk = tkills();
  int ntkt = killtypes();
    
  if(tkt < R20 && ntkt >= R20) {
    addMessage(XLAT("You hear a distant roar!"));
    playSound(NULL, "message-roar");
    }

  if(tk == 0 && ntk > 0 && !tactic::on && !euclid && !sphere && !cheater) {
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
    
  if(tk < 10 && ntk >= 10 && !tactic::on && !euclid && !sphere && !inv::on)
    addMessage(XLAT("Good to know that your fighting skills serve you well in this strange world."));

  if(tk < R100/2 && ntk >= R100/2 && !euclid && !sphere)
    addMessage(XLAT("You wonder where all these monsters go, after their death..."));

  if(tk < R100 && ntk >= R100 && !euclid && !sphere)
    addMessage(XLAT("You feel that the souls of slain enemies pull you to the Graveyard..."));
  
  if(!tu && trollUnlocked()) {
    playSound(c, "message-troll");
    addMessage(XLAT("%The1 says, \"I die, but my clan in Trollheim will avenge me!\"", m));
    }

  if(!eu && elementalUnlocked())
    addMessage(XLAT("After killing %the1, you feel able to reach the Elemental Planes!", m));
  
  if(m == moVizier && c->monst != moVizier && kills[moVizier] == 1) {
    addMessage(XLAT("Hmm, he has been training in the Emerald Mine. Interesting..."));
    princess::forceMouse = true;
    }
  
  if(m == moIvyRoot && ntk>tk)
    achievement_gain("IVYSLAYER");
    
  return ntk > tk;
  }

void pushMonster(cell *ct, cell *cf, int direction_hint) {
  moveMonster(ct, cf, direction_hint);
  }

bool destroyHalfvine(cell *c, eWall newwall, int tval) {
  if(cellHalfvine(c)) {
    for(int t=0; t<c->type; t++) if(c->move(t)->wall == c->wall) {
      if(newwall == waPartialFire) flameHalfvine(c->move(t), tval);
      else if(newwall == waRed1) c->move(t)->wall = waVinePlant;
      else c->move(t)->wall = newwall;
      }
    if(newwall == waPartialFire) flameHalfvine(c, tval);
    else c->wall = newwall;
    return true;
    }
  return false;
  }

int coastvalEdge(cell *c) { return coastval(c, laIvoryTower); }

int gravityLevel(cell *c) { 
  if(c->land == laIvoryTower || c->land == laEndorian)
    return coastval(c, laIvoryTower);
  if(c->land == laDungeon)
    return -coastval(c, laIvoryTower);
  if(c->land == laMountain)
    return 1-celldistAlt(c);
  return 0;
  }

bool canUnstable(eWall w, flagtype flags) {
  return w == waNone || w == waCanopy || w == waOpenPlate || w == waClosePlate ||
    w == waOpenGate || ((flags & MF_STUNNED) && (w == waLadder || w == waTrunk || w == waSolidBranch || w == waWeakBranch
    || w == waBigBush || w == waSmallBush));
  }

bool cellEdgeUnstable(cell *c, flagtype flags) {
  if(!isGravityLand(c->land) || !canUnstable(c->wall, flags)) return false;
  int d = gravityLevel(c);
  for(int i=0; i<c->type; i++) if(c->move(i)) {
    if(isAnyIvy(c->move(i)->monst) && 
      c->land == laMountain && !(flags & MF_IVY)) return false;
    if(gravityLevel(c->move(i)) == d-1) {
      if(againstWind(c->move(i), c)) return false;
      if(!passable(c->move(i), NULL, P_MONSTER | P_DEADLY))
        return false;
      if(isWorm(c->move(i)))
        return false;
      }
    }
  return true;
  }

// find worms and ivies
void settemp(cell *c) {
  temps.push_back(c); tempval.push_back(c->monst); c->monst = moNone;
  }

void findWormIvy(cell *c) {
  while(true) {
    if(c->monst == moWorm || c->monst == moTentacle || c->monst == moWormwait || c->monst == moTentaclewait ||
      c->monst == moTentacleEscaping) {
      worms.push_back(c); settemp(c);
      break;
      }
    else if(c->monst == moHexSnake) {
      hexsnakes.push_back(c); settemp(c);
      }
    else if(c->monst == moWormtail || c->monst == moHexSnakeTail) {
      bool bug = true;
      for(int i=0; i<c->type; i++) {
        cell* c2 = c->move(i);
        if(c2 && isWorm(c2) && c2->mondir != NODIR && c2->move(c2->mondir) == c) {
          settemp(c);
          c = c2;
          bug = false;
          }
        }
      if(bug) break;
      }
    else if(c->monst == moIvyWait) {
      cell* c2 = c->move(c->mondir);
      settemp(c); c=c2;
      }
    else if(c->monst == moIvyHead) {
      ivies.push_back(c); settemp(c);
      break;
      }
    else if(c->monst == moIvyBranch || c->monst == moIvyRoot) {
      bool bug = true;
      for(int i=0; i<c->type; i++) {
        cell* c2 = c->move(i);
        if(c2 && (c2->monst == moIvyHead || c2->monst == moIvyBranch) && c2->move(c2->mondir) == c) {
          settemp(c);
          c = c2;
          bug = false;
          }
        }
      if(bug) break;
      }
    else break;
    }
  }
  
bool bugsfighting;

bool keepLightning = false;

int statuecount;

int tidalphase;

int tidalsize, tide[200];

void calcTidalPhase() {
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

int tidespeed() {
  return tide[(turncount+6) % tidalsize] - tidalphase;
  }

bool recalcTide;

#define SEADIST LHU.bytes[0]
#define LANDDIST LHU.bytes[1]
#define CHAOSPARAM LHU.bytes[2]

int lavatide(cell *c, int t) {
  int tc = (shmup::on ? shmup::curtime/400 : turncount);
  return (windmap::at(c) + (tc+t)*4) & 255;
  }

void checkTide(cell *c) {
  if(c->land == laOcean) {
    int t = c->landparam;
    
    if(chaosmode) {
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
  }

void buildAirmap() {
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

map<cell*, int> rosemap;
// rosemap&3:
// 0 - wave not reached
// 1 - wave expanding
// 2 - wave phase 1
// 3 - wave phase 2

int rosedist(cell *c) {
  if(!(havewhat&HF_ROSE)) return 0;
  int&r (rosemap[c]);
  if((r&7) == 7) return 0;
  if(r&3) return (r&3)-1;
  return 0;
  }

bool againstRose(cell *cfrom, cell *cto) {
  if(rosedist(cfrom) != 1) return false;
  if(cto && rosedist(cto) == 2) return false;
  return true;
  }

bool withRose(cell *cfrom, cell *cto) {
  if(rosedist(cfrom) != 1) return false;
  if(rosedist(cto) != 2) return false;
  return true;
  }

void buildRosemap() {

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

int getDistLimit() { return base_distlimit; }

bool nogoSlow(cell *to, cell *from) {
  if(cellEdgeUnstable(to) && gravityLevel(to) >= gravityLevel(from)) return true;
  if(cellUnstable(to)) return true;
  return false;
  }

// pathdist begin
cell *pd_from;
int pd_range;

void onpath(cell *c, int d) {
  c->pathdist = d;
  pathq.push_back(c);
  }

void onpath(cell *c, int d, int sp) {
  c->pathdist = d;
  pathq.push_back(c);
  reachedfrom.push_back(sp);
  }

void clear_pathdata() {
  for(auto c: pathq) c->pathdist = PINFD;
  pathq.clear(); 
  pathqm.clear();
  reachedfrom.clear(); 
  }

int pathlock = 0;

void compute_graphical_distance() {
  if(pathlock) { printf("path error: compute_graphical_distance\n"); }
  cell *c1 = centerover.at ? centerover.at : pd_from ? pd_from : cwt.at;
  int sr = get_sightrange_ambush();
  if(pd_from == c1 && pd_range == sr) return;
  clear_pathdata();
  
  pd_from = c1;
  pd_range = sr;
  c1->pathdist = 0;
  pathq.push_back(pd_from);

  for(int qb=0; qb<isize(pathq); qb++) {
    cell *c = pathq[qb];
    if(c->pathdist == pd_range) break;
    if(qb == 0) forCellCM(c1, c) ;
    forCellEx(c1, c)
      if(c1->pathdist == PINFD)
        onpath(c1, c->pathdist + 1);
    }
  }

void computePathdist(eMonster param) {
  
  for(cell *c: targets)
    onpath(c, isPlayerOn(c) ? 0 : 1, hrand(c->type));

  int qtarg = isize(targets);
  
  int limit = gamerange();

  for(int qb=0; qb < isize(pathq); qb++) {
    int fd = reachedfrom[qb] + 3;
    cell *c = pathq[qb];
    if(c->monst && !isBug(c) && !(isFriendly(c) && !c->stuntime)) {
      pathqm.push_back(c); 
      continue; // no paths going through monsters
      }
    if(isMounted(c) && !isPlayerOn(c)) {
      // don't treat the Worm you are riding as passable
      pathqm.push_back(c); 
      continue; 
      }
    if(c->cpdist > limit && !(c->land == laTrollheim && turncount < c->landparam)) continue;
    int d = c->pathdist;
    if(d == PINFD - 1) continue;
    for(int j=0; j<c->type; j++) {
      int i = (fd+j) % c->type; 
      // printf("i=%d cd=%d\n", i, c->move(i)->cpdist);
      cell *c2 = c->move(i);

      if(c2 && c2->pathdist == PINFD &&
        passable(c2, (qb<qtarg) && !nonAdjacent(c,c2) && !thruVine(c,c2) ?NULL:c, P_MONSTER | P_REVDIR)) {
        
        if(qb >= qtarg) {
          if(param == moTortoise && nogoSlow(c, c2)) continue;
          if(param == moIvyRoot  && strictlyAgainstGravity(c, c2, false, MF_IVY)) continue;
          if(param == moWorm && (cellUnstable(c) || cellEdgeUnstable(c) || prairie::no_worms(c))) continue;
          if(items[itOrbLava] && c2->cpdist <= 5 && pseudohept(c) && makeflame(c2, 1, true))
            continue;
          }

        onpath(c2, d+1, c->c.spin(i));
        }
      }
    }
  }
// pathdist end

vector<pair<cell*, int> > butterflies;

void addButterfly(cell *c) {
  for(int i=0; i<isize(butterflies); i++)
    if(butterflies[i].first == c) {
      butterflies[i].second = 0;
      return;
      }
  butterflies.push_back(make_pair(c, 0));
  }

// calculate cpdist, 'have' flags, and do general fixings
void bfs() {

  calcTidalPhase(); 
    
  yendor::onpath();
  
  int dcs = isize(dcal);
  for(int i=0; i<dcs; i++) dcal[i]->cpdist = INFD;
  worms.clear(); ivies.clear(); ghosts.clear(); golems.clear(); 
  temps.clear(); tempval.clear(); targets.clear(); 
  statuecount = 0;
  hexsnakes.clear(); 

  hadwhat = havewhat;
  havewhat = 0; jiangshi_on_screen = 0;
  snaketypes.clear();
  if(!(hadwhat & HF_WARP)) { avengers = 0; }
  if(!(hadwhat & HF_MIRROR)) { mirrorspirits = 0; }

  elec::havecharge = false;
  elec::afterOrb = false;
  elec::haveelec = false;
  airmap.clear();
  if(!(hadwhat & HF_ROSE)) rosemap.clear();
  
  dcal.clear(); reachedfrom.clear(); 

  recalcTide = false;
  
  for(int i=0; i<numplayers(); i++) {
    cell *c = playerpos(i);
    if(!c) continue;
    if(c->cpdist == 0) continue;
    c->cpdist = 0;
    checkTide(c);
    dcal.push_back(c);
    reachedfrom.push_back(hrand(c->type));
    if(!invismove) targets.push_back(c);
    }
  
  int distlimit = gamerange();

  for(int i=0; i<numplayers(); i++) {
    cell *c = playerpos(i);
    if(!c) continue;
    if(items[itOrbDomination])
    if(c->monst == moTentacle || c->monst == moTentaclewait || c->monst == moTentacleEscaping)
      worms.push_back(c);
    }
  
  int qb = 0;
  while(true) {
    if(qb == isize(dcal)) break;
    int i, fd = reachedfrom[qb] + 3;
    cell *c = dcal[qb++];
    
    int d = c->cpdist;
    if(d == distlimit) { first7 = qb; break; }
    for(int j=0; j<c->type; j++) if(i = (fd+j) % c->type, c->move(i)) {
      // printf("i=%d cd=%d\n", i, c->move(i)->cpdist);
      cell *c2 = c->move(i);
      if(!c2) continue;
      
      if(isWarped(c2->land)) havewhat |= HF_WARP;
      if(c2->land == laMirror) havewhat |= HF_MIRROR;
      
      if((c->wall == waBoat || c->wall == waSea) &&
        (c2->wall == waSulphur || c2->wall == waSulphurC))
        c2->wall = waSea;
      
      if(c2 && signed(c2->cpdist) > d+1) {
        c2->cpdist = d+1;
        
        // remove treasures
        if(!peace::on && c2->item && c2->cpdist == distlimit && itemclass(c2->item) == IC_TREASURE &&
          c2->item != itBabyTortoise &&
          (items[c2->item] >= (chaosmode?10:20) + currentLocalTreasure || getGhostcount() >= 2)) {
            c2->item = itNone;
            if(c2->land == laMinefield) { c2->landparam &= ~3; }
            }
            
        if(c2->item == itBombEgg && c2->cpdist == distlimit && items[itBombEgg] >= c2->landparam) {
          c2->item = itNone;
          c2->landparam |= 2;
          c2->landparam &= ~1;
          if(!c2->monst) c2->monst = moBomberbird;
          }
        
        if(c2->item == itBarrow && c2->cpdist == distlimit && c2->wall != waBarrowDig) {
          c2->item = itNone;
          }
        
        if(c2->item == itLotus && c2->cpdist == distlimit && items[itLotus] >= getHauntedDepth(c2)) {
          c2->item = itNone;
          }
        
        if(c2->item == itMutant2 && timerghost) {
          bool rotten = true;
          for(int i=0; i<c2->type; i++)
            if(c2->move(i) && c2->move(i)->monst == moMutant)
              rotten = false;
          if(rotten) c2->item = itNone;
          }
        
        if(c2->item == itDragon && (shmup::on ? shmup::curtime-c2->landparam>300000 : 
          turncount-c2->landparam > 500))
          c2->item = itNone;
      
        if(c2->item == itTrollEgg && c2->cpdist == distlimit && !shmup::on && c2->landparam && turncount-c2->landparam > 650)
          c2->item = itNone;
      
        if(c2->item == itMutant && c2->cpdist == distlimit && items[itMutant] >= c2->landparam) {
          c2->item = itNone;
          }
      
        if(c2->item == itIvory && c2->cpdist == distlimit && items[itIvory] >= c2->landparam) {
          c2->item = itNone;
          }
        
        if(c2->item == itAmethyst && c2->cpdist == distlimit && items[itAmethyst] >= -celldistAlt(c2)/5) {
          c2->item = itNone;
          }
        
        if(!keepLightning) c2->ligon = 0;
        dcal.push_back(c2);
        reachedfrom.push_back(c->c.spin(i));
        
        checkTide(c2);
                
        if(c2->wall == waBigStatue && c2->land != laTemple) 
          statuecount++;
          
        if(cellHalfvine(c2) && isWarped(c2)) {
          addMessage(XLAT("%The1 is destroyed!", c2->wall));
          destroyHalfvine(c2);
          }
        
        if(c2->wall == waCharged) elec::havecharge = true;
        if(c2->land == laStorms) elec::haveelec = true;
        
        if(c2->land == laWhirlpool) havewhat |= HF_WHIRLPOOL;
        if(c2->land == laWhirlwind) havewhat |= HF_WHIRLWIND;
        if(c2->land == laPrairie) havewhat |= HF_RIVER;

        if(c2->wall == waRose) havewhat |= HF_ROSE;
        
        if((hadwhat & HF_ROSE) && (rosemap[c2] & 3)) havewhat |= HF_ROSE;
        
        if(c2->monst) {
          if(isHaunted(c2->land) && 
            c2->monst != moGhost && c2->monst != moZombie && c2->monst != moNecromancer)
            survivalist = false;
          if(c2->monst == moHexSnake || c2->monst == moHexSnakeTail) {
            havewhat |= HF_HEX;
            if(c2->mondir != NODIR)
              snaketypes.insert(snake_pair(c2));
            if(c2->monst == moHexSnake) hexsnakes.push_back(c2);
            else findWormIvy(c2);
            }
          else if(c2->monst == moKrakenT || c2->monst == moKrakenH) {
            havewhat |= HF_KRAKEN;
            }
          else if(c2->monst == moDragonHead || c2->monst == moDragonTail) {
            havewhat |= HF_DRAGON;
            }
          else if(c2->monst == moWitchSpeed) 
            havewhat |= HF_FAST;
          else if(c2->monst == moMutant)
            havewhat |= HF_MUTANT;
          else if(c2->monst == moJiangshi)
            jiangshi_on_screen++;
          else if(c2->monst == moOutlaw)
            havewhat |= HF_OUTLAW;
          else if(isGhostMover(c2->monst))
            ghosts.push_back(c2);
          else if(isWorm(c2) || isIvy(c2)) findWormIvy(c2);
          else if(isBug(c2)) {
            havewhat |= HF_BUG;
            targets.push_back(c2);
            }
          else if(isFriendly(c2)) {
            if(c2->monst != moMouse && !markEmpathy(itOrbInvis) && !(isWatery(c2) && markEmpathy(itOrbFish)) &&
              !c2->stuntime) targets.push_back(c2);
            if(c2->monst == moGolem) golems.push_back(c2);
            if(c2->monst == moFriendlyGhost) golems.push_back(c2);
            if(c2->monst == moKnight) golems.push_back(c2);
            if(c2->monst == moTameBomberbird) golems.push_back(c2);
            if(c2->monst == moMouse) { golems.push_back(c2); havewhat |= HF_MOUSE; }
            if(c2->monst == moPrincess || c2->monst == moPrincessArmed) golems.push_back(c2);
            if(c2->monst == moIllusion) {
              if(items[itOrbIllusion]) items[itOrbIllusion]--;
              else c2->monst = moNone;
              }
            }
          else if(c2->monst == moButterfly) {
            addButterfly(c2);
            }
          else if(isAngryBird(c2->monst)) {
            havewhat |= HF_BIRD;
            if(c2->monst == moBat) havewhat |= HF_BATS | HF_EAGLES;
            if(c2->monst == moEagle) havewhat |= HF_EAGLES;
            }
          else if(c2->monst == moReptile) havewhat |= HF_REPTILE;
          else if(isLeader(c2->monst)) havewhat |= HF_LEADER;
          else if(c2->monst == moEarthElemental) havewhat |= HF_EARTH;
          else if(c2->monst == moWaterElemental) havewhat |= HF_WATER;
          else if(c2->monst == moVoidBeast) havewhat |= HF_VOID;
          else if(c2->monst == moHunterDog) havewhat |= HF_HUNTER;
          else if(isMagneticPole(c2->monst)) havewhat |= HF_MAGNET;
          else if(c2->monst == moAltDemon) havewhat |= HF_ALT;
          else if(c2->monst == moHexDemon) havewhat |= HF_HEXD;
          else if(c2->monst == moMonk) havewhat |= HF_MONK;
          else if(c2->monst == moShark || c2->monst == moCShark) havewhat |= HF_SHARK;
          else if(c2->monst == moAirElemental) 
            havewhat |= HF_AIR, airmap.push_back(make_pair(c2,0));
          }
        // pheromones!
        if(c2->land == laHive && c2->landparam >= 50 && c2->wall != waWaxWall) 
          havewhat |= HF_BUG;
        if(c2->wall == waThumperOn)
          targets.push_back(c2);

        }
      }
    }

  while(recalcTide) {
    recalcTide = false;
    for(int i=0; i<isize(dcal); i++) checkTide(dcal[i]);
    }    
  
  int qtemp = isize(temps);
  for(int i=0; i<qtemp; i++) temps[i]->monst = tempval[i];
  
  buildAirmap();
  }

bool makeEmpty(cell *c) {

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

  if(c->land == laCocytus)
    c->wall = waFrozenLake;
  else if(c->land == laAlchemist || c->land == laCanvas)
    ;
  else if(c->land == laDual)
    ;
  else if(c->land == laCaves || c->land == laEmerald)
    c->wall = waCavefloor;
  else if(c->land == laDeadCaves)
    c->wall = waDeadfloor2;
  else if(c->land == laCaribbean || c->land == laOcean || c->land == laWhirlpool || c->land == laLivefjord || c->land == laWarpSea || c->land == laKraken)
    c->wall = waBoat; // , c->item = itOrbYendor;
  else if(c->land == laMinefield)
    c->wall = waMineOpen;
  else if(c->wall == waFan && bounded)
    ;
  else if(c->wall == waOpenPlate && bounded)
    ;
  else if(c->wall == waTrunk || c->wall == waSolidBranch || c->wall == waWeakBranch)
    ;
  else if(c->wall == waGiantRug)
    ;
  else if(c->wall == waDock)
    ;
  else if(c->land == laDocks)
    c->wall = waBoat;
  else if(c->wall == waFreshGrave && bounded)
    ;
  else if(isReptile(c->wall))
    c->wparam = reptilemax();
  else if(c->wall == waAncientGrave && bounded)
    ;
  else
    c->wall = waNone;
    
  if(c->item != itCompass)
    c->item = itNone;
  
  if(c->land == laWildWest) {
    forCellEx(c2, c)
    forCellEx(c3, c2)
      if(c3->wall != waBarrier)
        c3->wall = waNone;
    }
  
  return true;
  }

int numgates = 0;

void toggleGates(cell *c, eWall type, int rad) {
  if(!c) return;
  celllister cl(c, rad, 1000000, NULL);
  for(cell *ct: cl.lst) {
    if(ct->wall == waOpenGate && type == waClosePlate) {
      bool onWorm = false;
      if(isWorm(ct)) onWorm = true;
      for(int i=0; i<ct->type; i++) 
        if(ct->move(i) && ct->move(i)->wall == waOpenGate && isWorm(ct->move(i))) onWorm = true;
      if(!onWorm) {
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
      ct->wall = waOpenGate, numgates++;
      toggleGates(ct, type, 1);
      }
    }
  }

void toggleGates(cell *ct, eWall type) {
  playSound(ct, "click");
  numgates = 0;
  if(type == waClosePlate && PURE)
    toggleGates(ct, type, 2);
  else
    toggleGates(ct, type, (GOLDBERG && !sphere && !a4) ? gp::dist_3() : 3);
  if(numgates && type == waClosePlate)
    playSound(ct, "closegate");
  if(numgates && type == waOpenPlate)
    playSound(ct, "opengate");
  }

void destroyTrapsOn(cell *c) {
  if(c->wall == waArrowTrap) c->wall = waNone, destroyTrapsAround(c);
  }

void destroyTrapsAround(cell *c) {
  forCellEx(c2, c) destroyTrapsOn(c2);
  }

void destroyWeakBranch(cell *cf, cell *ct, eMonster who) {
  if(cf && ct && cf->wall == waWeakBranch && cellEdgeUnstable(ct) &&
    gravityLevel(ct) >= gravityLevel(cf) && !ignoresPlates(who)) {
    cf->wall = waCanopy;
    if(!cellEdgeUnstable(cf)) { cf->wall = waWeakBranch; return; }
    playSound(cf, "trapdoor");
    drawParticles(cf, winf[waWeakBranch].color, 4);
    }
  if(cf && ct && cf->wall == waSmallBush && cellEdgeUnstable(ct) && 
    gravityLevel(ct) >= gravityLevel(cf) && !ignoresPlates(who)) {
    cf->wall = waNone;
    if(!cellEdgeUnstable(cf)) { cf->wall = waSmallBush; return; }
    playSound(cf, "trapdoor");
    drawParticles(cf, winf[waWeakBranch].color, 4);
    }
  }

bool isCentralTrap(cell *c) {
  if(c->wall != waArrowTrap) return false;
  int i = 0;
  forCellEx(c2, c) if(c2->wall == waArrowTrap) i++;
  return i == 2;
  }

array<cell*, 5> traplimits(cell *c) {
  array<cell*, 5> res;
  int q = 0;
  res[2] = c;
  for(int d=0; d<c->type; d++) {
    cellwalker cw(c, d);
    cw += wstep;
    if(cw.at->wall != waArrowTrap) continue;
    res[1+q*2] = cw.at;
    cw += (cw.at->type/2);
    if((cw.at->type&1) && (cw+wstep).at->wall != waStone) cw += 1;
    cw += wstep;
    res[(q++)*4] = cw.at;
    }
  while(q<2) { res[q*4] = res[1+q*2] = NULL; q++; }
  return res;
  }

void activateArrowTrap(cell *c) {
  if(c->wall == waArrowTrap && c->wparam == 0) {
    c->wparam = shmup::on ? 2 : 1;
    forCellEx(c2, c) activateArrowTrap(c2);
    if(shmup::on) shmup::activateArrow(c);
    }
  }


// effect of moving monster m from cf to ct
// this is called from moveMonster, or separately from moveIvy/moveWorm,
// or when a dead bird falls (then m == moDeadBird)

void moveEffect(cell *ct, cell *cf, eMonster m, int direction_hint) {

  if(cf) destroyWeakBranch(cf, ct, m);

  mayExplodeMine(ct, m);
  
  if(!isNonliving(m)) terracottaAround(ct);
 
  if(ct->wall == waMineUnknown && !ct->item && !ignoresPlates(m)) 
    ct->landparam |= 2; // mark as safe

  if((ct->wall == waClosePlate || ct->wall == waOpenPlate) && !ignoresPlates(m))
    toggleGates(ct, ct->wall);
  if(m == moDeadBird && cf == ct && cellUnstable(cf)) {
    fallingFloorAnimation(cf);
    cf->wall = waChasm;
    }
  
  if(ct->wall == waArrowTrap && !ignoresPlates(m))
    activateArrowTrap(ct);
    
  if(cf && isPrincess(m)) princess::move(ct, cf);
  
  if(cf && m == moTortoise) {
    tortoise::emap[ct] = tortoise::getb(cf);
    tortoise::emap.erase(cf);
    }
  
  if(cf && ct->item == itBabyTortoise && !cf->item) {
    cf->item = itBabyTortoise;
    ct->item = itNone;
    animateMovement(ct, cf, LAYER_BOAT, direction_hint);
    tortoise::babymap[cf] = tortoise::babymap[ct];
    tortoise::babymap.erase(ct);
    }
  }

void updateHi(eItem it, int v) {
  if(!yendor::on)
    if(v > hiitems[modecode()][it]) hiitems[modecode()][it] = v;
  }

void gainItem(eItem it) {
  int g = gold();
  items[it]++; if(it != itLotus) updateHi(it, items[it]);
  if(it == itRevolver && items[it] > 6) items[it] = 6;
  achievement_collection(it, gold(), g);
  multi::treasures[multi::cpid]++;
#if CAP_DAILY
    if(daily::on) achievement_final(false);
#endif
  }

string itemcounter(int qty) {
  string s = ""; s += " ("; s += its(qty); s += ")"; return s;
  }

void gainShard(cell *c2, const char *msg) {
  invismove = false;
  string s = XLAT(msg);
  if(is_mirrorland(c2) && !peace::on) {
     gainItem(itShard);
     s += itemcounter(items[itShard]);
     collectMessage(c2, itShard);
     }
  addMessage(s);
  c2->wall = waNone;
  invismove = false;
  }

void playerMoveEffects(cell *c1, cell *c2) {

  if(peace::on) items[itOrbSword] = c2->land == laBurial ? 100 : 0;

  sword::angle[multi::cpid] = sword::shift(c1, c2, sword::angle[multi::cpid]);
  
  destroyWeakBranch(c1, c2, moPlayer);

  bool nomine = (c2->wall == waMineMine || c2->wall == waMineUnknown)  && markOrb(itOrbAether);
  
  if(!nomine) {
    uncoverMines(c2,
      (items[itBombEgg] < 1 && !tactic::on) ? 0 :
      items[itBombEgg] < 20 ? 1 :
      items[itBombEgg] < 30 ? 2 :
      3, 0
      );
    mayExplodeMine(c2, moPlayer);
    }
  
  if((c2->wall == waClosePlate || c2->wall == waOpenPlate) && !markOrb(itOrbAether))
    toggleGates(c2, c2->wall);

  if(c2->wall == waArrowTrap && c2->wparam == 0 && !markOrb(itOrbAether))
    activateArrowTrap(c2);
    
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

void beastcrash(cell *c, cell *beast) {
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
  else if(isBull(c->monst) || isSwitch(c->monst)) {
    addMessage(XLAT("%The1 crashes into %the2!", beast->monst, c->monst));
    if(c->monst == moSleepBull) c->monst = moRagingBull, c->stuntime = 3;
    }
  }

void stayEffect(cell *c) {
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

void makeTrollFootprints(cell *c) {
  if(c->land != laTrollheim) return;
  if(c->item == itTrollEgg && c->landparam) return;
  c->landparam = turncount + 100;
  }

void moveMonster(cell *ct, cell *cf, int direction_hint) {
  eMonster m = cf->monst;
  bool fri = isFriendly(cf);
  if(isDragon(m)) {
    printf("called for Dragon\n");
    return;
    }
  if(m != moMimic) animateMovement(cf, ct, LAYER_SMALL, direction_hint);
  // the following line is necessary because otherwise plates disappear only inside the sight range
  if(cellUnstable(cf) && !ignoresPlates(m)) {
    fallingFloorAnimation(cf);
    cf->wall = waChasm;
    }
  moveEffect(ct, cf, m, direction_hint);
  if(ct->wall == waCamelotMoat && 
    (m == moShark || m == moCShark || m == moGreaterShark))
      achievement_gain("MOATSHARK");
  if(m == moTentacleGhost) { 
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
    if(ct->monst != moTentacleGhost)
      ct->mondir = neighborId(ct, cf);
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
  
  if(fri || isBug(m) || items[itOrbDiscord]) stabbingAttack(cf, ct, m);

  if(isLeader(m)) {
    if(ct->wall == waBigStatue) {
      ct->wall = cf->wall;
      cf->wall = waBigStatue;
      animateMovement(ct, cf, LAYER_BOAT, revhint(cf, direction_hint));
      }

    moveBoatIfUsingOne(ct, cf, revhint(cf, direction_hint));
    }
  
  if(isTroll(m)) { makeTrollFootprints(ct); makeTrollFootprints(cf); }
    
  int inc = incline(cf, ct);

  if(m == moEarthElemental) {
    if(!passable(ct, cf, 0)) earthFloor(ct);
    earthMove(cf, neighborId(cf, ct));
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
    forCellEx(c3, ct) if(!logical_adjacent(cf, m, c3))
      if(canAttack(ct, moLancer, c3, c3->monst, AF_LANCE | AF_GETPLAYER)) {
        attackMonster(c3, AF_LANCE | AF_MSG | AF_GETPLAYER, m);
        }
    }
  
  if(m == moWitchFire) makeflame(cf, 10, false);
  if(m == moFireElemental) { makeflame(cf, 20, false); if(cf->wparam < 20) cf->wparam = 20; }
  
  bool adj = false;  
  if(ct->cpdist == 1 && (items[itOrb37] || !nonAdjacent(cf,ct)) && markOrb(itOrbBeauty))
    adj = true;
    
  if(!adj && items[itOrbEmpathy] && items[itOrbBeauty]) {
    for(int i=0; i<ct->type; i++) if(ct->move(i) && isFriendly(ct->move(i)))
      adj = true, markOrb(itOrbEmpathy), markOrb(itOrbBeauty);
    }
  
  if(adj && ct->stuntime == 0 && !isMimic(m)) {
    ct->stuntime = 2;
    checkStunKill(ct);
    }

  if(!cellEdgeUnstable(ct)) {
    if(isMetalBeast(m)) ct->stuntime += 2;
    if(m == moTortoise) ct->stuntime += 3;
    if(m == moDraugr && ct->land != laBurial && ct->land != laHalloween) ct->stuntime += 2;
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
  else if(inc == -3 && !survivesFall(ct->monst)) {
    addMessage(XLAT("%The1 falls!", ct->monst));
    fallMonster(ct, AF_FALL);
    }
  if(isThorny(ct->wall) && !survivesThorns(ct->monst)) {
    addMessage(XLAT("%The1 is killed by thorns!", ct->monst));
    playSound(ct, "hit-rose");
    if(isBull(ct->monst)) ct->wall = waNone;
    fallMonster(ct, AF_CRUSH);
    }
  if(sword::at(ct) && canAttack(NULL, moPlayer, ct, m, AF_SWORD_INTO)) {
    attackMonster(ct, AF_SWORD_INTO | AF_MSG, moPlayer);
    achievement_gain("GOSWORD");
    }
  }

bool cannotGo(eMonster m, cell *c) {
  if(m == moCrystalSage && (c->land != laCocytus || HEAT(c) > SAGEMELT || allPlayersInBoats()))
    return true;
  return false;
  }

bool wantsToStay(eMonster m) {
  return m == moCrystalSage && allPlayersInBoats();
  }

bool batsAfraid(cell *c) {
  // bats 
  for(int i=-1; i<isize(targets); i++)
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

int angledist(int t, int d1, int d2) {
  int dd = d1 - d2;
  while(dd<0) dd += t;
  while(dd>t/2) dd -= t;
  if(dd<0) dd = -dd;
  return dd;
  }

int angledistButterfly(int t, int d1, int d2) {
  int dd = d1 - d2;
  while(dd<0) dd += t;
  return dd;
  }

int angledist(cell *c, int d1, int d2) { 
  return angledist(c->type, d1, d2);
  }

bool anglestraight(cell *c, int d1, int d2) { 
  return angledist(c->type, d1, d2) >= c->type / 2;
  }

int bulldist(cell *c) {
  int low = 0;
  forCellEx(c2, c) if(c2->cpdist < c->cpdist) low++;
  return 8 * c->cpdist - low;
  }

int bulldistance(cell *c, cell *d) {
  int low = 0;
  int cd = celldistance(c, d);
  forCellEx(c2, c) if(celldistance(c2, d) < cd) low++;
  return 8 * cd - low;
  }

int landheattype(cell *c) {
  if(isIcyLand(c)) return 0;
  if(c->land == laVolcano) return 2;
  return 1;
  }

// move value
int moveval(cell *c1, cell *c2, int d, flagtype mf) {
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
  
  if((m == moRatling || m == moRatlingAvenger) && lastmovetype == lmSkip) return 650;

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
    return 1500 + angledistButterfly(c1->type, c1->mondir, d);
  
  if(m == moRagingBull && c1->mondir != NODIR)
    return 1500 - bulldist(c2);
  
  // actually they just run away
  if(m == moHunterChanging && c2->pathdist > c1->pathdist) return 1600;
  
  if((mf & MF_PATHDIST) && !pathlock) printf("using MF_PATHDIST without path\n"); 

  if(hunt && (mf & MF_PATHDIST) && c2->pathdist < c1->pathdist && !peace::on) return 1500; // good move
  
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
int stayval(cell *c, flagtype mf) {
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

int totalbulldistance(cell *c, int k) {
  int tbd = 0;
  for(int p=0; p<numplayers(); p++) {
    cell *c2  = shpos[p][(cshpos+SHSIZE-k-1)%SHSIZE];
    if(c2) tbd += bulldistance(c, c2);
    }
  return tbd;
  }

void determinizeBull(cell *c, int *posdir, int& nc) {
  // determinize the Angry Beast movement:
  // use the previous PC's positions as the tiebreaker
  for(int k=0; k<SHSIZE && nc>1; k++) {
    int pts[MAX_EDGE];
    for(int d=0; d<nc; d++) pts[d] = totalbulldistance(c->move(posdir[d]), k);

    int bestpts = 1000;
    for(int d=0; d<nc; d++) if(pts[d] < bestpts) bestpts = pts[d];
    int nc0 = 0;
    for(int d=0; d<nc; d++) if(pts[d] == bestpts) posdir[nc0++] = posdir[d];
    nc = nc0;
    }
  }

int determinizeBullPush(cellwalker bull) {
  int nc = 2;
  int dirs[2], positive;
  bull += wstep;
  cell *c2 = bull.at;
  if(!(c2->type & 1)) return 1; // irrelevant
  int d = c2->type / 2;
  bull += d; dirs[0] = positive = bull.spin;
  bull -= 2*d; dirs[1] = bull.spin;
  determinizeBull(c2, dirs, nc);
  if(dirs[0] == positive) return -1;
  return 1;
  }    

int posdir[MAX_EDGE], nc;

int pickMoveDirection(cell *c, flagtype mf) {
  int bestval = stayval(c, mf);
  nc = 1; posdir[0] = -1;

  // printf("stayval [%p, %s]: %d\n", c, dnameof(c->monst), bestval);
  for(int d=0; d<c->type; d++) {
    cell *c2 = c->move(d);
    int val = moveval(c, c2, d, mf);
    // printf("[%d] %p: val=%5d pass=%d\n", d, c2, val, passable(c2,c,0));
    if(val > bestval) nc = 0, bestval = val;
    if(val == bestval) posdir[nc++] = d;
    }
  
  if(c->monst == moRagingBull) 
    determinizeBull(c, posdir, nc);
    
  if(!nc) return -1;
  return posdir[hrand(nc)];
  }

int pickDownDirection(cell *c, flagtype mf) {
  int downs[MAX_EDGE], qdowns = 0;
  int bestdif = -100;
  forCellIdEx(c2, i, c) {
    if(gravityLevel(c2) < gravityLevel(c) && passable_for(c->monst, c2, c, P_MIRROR) &&
      !isPlayerOn(c2)) {
      int cdif = i-c->mondir;
      if(cdif < 0) cdif += c->type;
      if(cdif > c->type/2) cdif = cdif - c->type;
      if(cdif < 0) cdif = -2*cdif+1; else cdif = 2*cdif;
      // printf("i=%d md=%d dif=%d\n", i, c->mondir, cdif);
      if(c2->wall == waClosePlate || c->wall == waClosePlate)
        cdif += 20;
      if(cdif > bestdif) bestdif = cdif, qdowns = 0;
      if(cdif == bestdif) downs[qdowns++] = i;
      }
    }
  if(!qdowns) return -1;
  return downs[hrand(qdowns)];
  }

template<class T> 
cell *determinePush(cellwalker who, cell *c2, int subdir, const T& valid, int& pushdir) {
  if(subdir != 1 && subdir != -1) {
    subdir = 1;
    static bool first = true;
    if(first)
      first = false,
      addMessage("bad push: " + its(subdir));
    }
  cellwalker push = who;
  push += wstep;
  int pd = push.at->type/2;
  push += pd * -subdir;
  push += wstep;
  if(valid(push.at)) return push.at;
  if(c2->type&1) {
    push = push + wstep - subdir + wstep;
    pushdir = (push+wstep).spin;
    if(valid(push.at)) return push.at;
    }
  if(gravityLevel(push.at) < gravityLevel(c2)) {
    push = push + wstep + 1 + wstep;
    if(gravityLevel(push.at) < gravityLevel(c2)) {
      push = push + wstep - 2 + wstep;
      }
    if(gravityLevel(push.at) < gravityLevel(c2)) {
      push = push + wstep + 1 + wstep;
      }
    pushdir = (push+wstep).spin;
    if(valid(push.at)) return push.at;
    }
  return c2;
  }
 
// Angry Beast attack
// note: this is done both before and after movement
void beastAttack(cell *c, bool player) {
  if(c->mondir == NODIR) return;
  forCellIdEx(c2, d, c) {
    bool opposite = anglestraight(c, d, c->mondir);
    int flags = AF_BULL;
    if(player) flags |= AF_GETPLAYER;
    if(!opposite) flags |= AF_ONLY_FBUG;
    if(canAttack(c, moRagingBull, c2, c2->monst, flags)) {
      attackMonster(c2, flags | AF_MSG, moRagingBull);
      if(c2->monst && c2->stuntime) {
        cellwalker bull (c, d);
        int subdir = determinizeBullPush(bull);
        int pushdir = NOHINT;
        cell *c3 = determinePush(bull, c2, subdir, [c2] (cell *c) { return passable(c, c2, P_BLOW); }, pushdir);
        if(c3 && c3 != c2)
          pushMonster(c3, c2, pushdir);
        }
      }
    if(c2->wall == waThumperOff) {
      playSound(c2, "click");
      c2->wall = waThumperOn;
      }
    if(c2->wall == waThumperOn) {
      cellwalker bull (c, d);
      int subdir = determinizeBullPush(bull);
      int pushdir = NOHINT;
      cell *c3 = determinePush(bull, c2, subdir, [c2] (cell *c) { return canPushThumperOn(c, c2, c); }, pushdir);
      if(c3 && c3 != c2)
        pushThumper(c2, c3);
      }
    }
  }

bool quantum;

cell *moveNormal(cell *c, flagtype mf) {
  eMonster m = c->monst;
  
  int d;
  
  if(c->stuntime) {
    if(cellEdgeUnstable(c, MF_STUNNED)) d = pickDownDirection(c, mf), nc = 1, posdir[0] = d;
    else return NULL;
    }
  else {
    // Angry Beasts attack all neighbors first
    if(m == moRagingBull) beastAttack(c, true);
    d = pickMoveDirection(c, mf);
    }
  if(d == -1) {
    stayEffect(c);
    return c;
    }
  
  if(!quantum) {
    cell *c2 = c->move(d);
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
      animateAttack(c, c2, LAYER_SMALL, d);
      if(m == moFlailer && m2 == moIllusion) 
        attackMonster(c, 0, m2);
      return c2;
      }
    
    moveMonster(c2, c, d);
    if(m == moRagingBull) beastAttack(c2, false);
    return c2;
    }
  else {
    bool attacking = false;
    for(int i=0; i<nc; i++) {
      cell *c2 = c->move(posdir[i]);

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
    
    if(!attacking) for(int i=0; i<nc; i++) {
      cell *c2 = c->move(posdir[i]);
      if(!c->monst) c->monst = m;
      moveMonster(c2, c, posdir[i]);
      if(m == moRagingBull) beastAttack(c2, false);
      }
    return c->move(d);
    }
  }

// for sandworms
void explodeAround(cell *c) {
  for(int j=0; j<c->type; j++) {
    cell* c2 = c->move(j);
    if(c2) {
      if(isIcyLand(c2)) HEAT(c2) += 0.5;
      eWall ow = c2->wall;
      if((c2->wall == waDune || c2->wall == waIcewall ||
        c2->wall == waAncientGrave || c2->wall == waFreshGrave || 
        c2->wall == waColumn || c2->wall == waThumperOff || c2->wall == waThumperOn ||
        (isFire(c2) && !eternalFire(c2)) ||
        c2->wall == waBigTree || c2->wall == waSmallTree ||
        c2->wall == waVinePlant || c2->wall == waVineHalfA || c2->wall == waVineHalfB)) {
        destroyHalfvine(c2); 
        c2->wall = waNone;
        }
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
  }

void killHardcorePlayer(int id, flagtype flags) {
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

void killThePlayer(eMonster m, int id, flagtype flags) {
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
    }
  }

void killThePlayerAt(eMonster m, cell *c, flagtype flags) {
  for(int i=0; i<numplayers(); i++) 
    if(playerpos(i) == c) 
      killThePlayer(m, i, flags);
  }

void afterplayermoved() {
  setdist(cwt.at, 7 - getDistLimit() - genrange_bonus, NULL);
  prairie::treasures();
  if(generatingEquidistant) {
    printf("Warning: generatingEquidistant set to true\n");
    generatingEquidistant = false;
    }
  }

void mountmove(cell *c, int spin, bool fp, int id) {
  if(multi::players > 1) {
    multi::player[id].at = c;
    multi::player[id].spin = spin;
    multi::flipped[id] = fp;
    }
  else {
    cwt.at = c;
    cwt.spin = spin;
    flipplayer = fp;
    }
  afterplayermoved();
  }

void mountmove(cell *c, int spin, bool fp, cell *ppos) {
  for(int i=0; i<numplayers(); i++) {
    if(playerpos(i) == ppos) {
      animateMovement(ppos, c, LAYER_SMALL, revhint(c, spin));
      mountmove(c, spin, fp, i);
      }
    if(lastmountpos[i] == ppos && ppos != NULL) {
      lastmountpos[i] = c;
      }
    else if(lastmountpos[i] == c)  {
      lastmountpos[i] = NULL;
      }
    }
  }

void moveWorm(cell *c) {

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
      }
    allcells.push_back(c2);
    for(int i=isize(allcells)-2; i>=0; i--) {
      cell *cmt = allcells[i+1];
      cell *cft = allcells[i];
      if(cft->monst != moTentacleGhost && cmt->monst != moTentacleGhost)
        mountmove(cmt, cft->c.spin(cft->mondir), false, cft);
      animateMovement(cft, cmt, LAYER_BIG, cft->mondir);
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
    while(c->monst == moWorm || c->monst == moWormtail || c->monst == moTentacle || c->monst == moTentacletail) {
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
        achievement_gain("ZEBRAWORM", rg::special_geometry);
      }
    return;
    }
  
  cell* goal = c->move(dir);

  if(isPlayerOn(goal) || goal->monst) 
    attackMonster(goal, AF_EAT | AF_MSG | AF_GETPLAYER, c->monst);
  
  for(int j=0; j<c->type; j++) if(c->move(j) == goal) {
    goal->monst = eMonster(moWormwait + id);
    moveEffect(goal, NULL, eMonster(moWormwait + id), NOHINT);
      
    animateMovement(c, goal, LAYER_BIG, dir);
    c->monst = eMonster(moWormtail + id);
    goal->mondir = c->c.spin(j);
  
    mountmove(goal, goal->mondir, true, c);
    
    if(id) {
      cell *c2 = c, *c3 = c2;
      while(c2->monst == moTentacletail || c2->monst == moTentacleGhost) {
        if(c2->mondir == NODIR) {
          // drawParticles(c2, (linf[c2->land].color & 0xF0F0F0), 16, 50);
          return;
          }
        c3 = c2, c2 = c3->move(c2->mondir);
        if(c3->monst != moTentacleGhost && c2->monst != moTentacleGhost) 
          mountmove(c3, c3->mondir, true, c2);
        animateMovement(c2, c3, LAYER_BIG, c2->c.spin(c2->mondir));
        }
      }
    
    cell *c2 = c, *c3 = c2;
    for(int a=0; a<WORMLENGTH; a++) {
      if(c2->monst == moWormtail) {
        if(c2->mondir == NODIR) {
          drawParticles(c2, (linf[c2->land].color & 0xF0F0F0), 16);
          return;
          }
        c3 = c2, c2 = c3->move(c2->mondir);
        mountmove(c3, c3->mondir, true, c2);
        animateMovement(c2, c3, LAYER_BIG, revhint(c2, c2->mondir));
        }
      }
    
    if(c2->monst == moWormtail) c2->monst = moNone, c3->mondir = NODIR;
    }

  }

void ivynext(cell *c) {
  cellwalker cw(c, c->mondir);
  
  // check the mirroring status
  cell *c2 = c;
  while(true) {
    if(c2->monst == moIvyRoot) break;
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
void removeIvy(cell *c) {
  eMonster m = c->monst;
  c->monst = moNone; // NEWYEARFIX
  for(int i=0; i<c->type; i++)
  // note that semi-vines don't count
    if(c->move(i)->wall == waVinePlant) {
      destroyHalfvine(c);
      c->wall = waVinePlant;
      }
  if(c->wall != waVinePlant) {
    if(m == moIvyDead)
      m = moIvyWait;
    drawParticles(c, minf[m].color, 2);
    }
  }

void moveivy() {
  if(isize(ivies) == 0) return;
  pathdata pd(moIvyRoot);
  for(int i=0; i<isize(ivies); i++) {
    cell *c = ivies[i];
    cell *co = c;
    if(c->monst != moIvyHead) continue;
    ivynext(c);

    cell *mto = NULL;
    int pd = c->pathdist;
    int sp = 0;
    
    while(c->monst != moIvyRoot) {
      if(!isIvy(c->monst)) {
        raiseBuggyGeneration(c, "that's not an Ivy!");
        }
      if(c->mondir == NODIR)
        raiseBuggyGeneration(c, "wrong mondir!");
      for(int j=0; j<c->type; j++) {
        if(c->move(j) && canAttack(c, c->monst, c->move(j), c->move(j)->monst, AF_ONLY_FRIEND | AF_GETPLAYER)) {
          if(isPlayerOn(c->move(j))) 
            killThePlayerAt(c->monst, c->move(j), 0);
          else {
            if(attackJustStuns(c->move(j), 0))
              addMessage(XLAT("The ivy attacks %the1!", c->move(j)->monst));
            else if(isNonliving(c->move(j)->monst))
              addMessage(XLAT("The ivy destroys %the1!", c->move(j)->monst));
            else
              addMessage(XLAT("The ivy kills %the1!", c->move(j)->monst));
            attackMonster(c->move(j), AF_NORMAL, c->monst);
            }
          continue;
          }
        if(c->move(j) && signed(c->move(j)->pathdist) < pd && passable(c->move(j), c, 0)
          && !strictlyAgainstGravity(c->move(j), c, false, MF_IVY))
          mto = c->move(j), pd = mto->pathdist, sp = c->c.spin(j);
        }
      c = c->move(c->mondir);
      }

    if(mto && mto->cpdist) {
      animateMovement(mto->move(sp), mto, LAYER_BIG, mto->c.spin(sp));
      mto->monst = moIvyWait, mto->mondir = sp;
      moveEffect(mto, NULL, moIvyWait, NOHINT);
      // if this is the only branch, we want to move the head immediately to mto instead
      if(mto->move(mto->mondir)->monst == moIvyHead) {
        mto->monst = moIvyHead; co->monst = moIvyBranch;
        }
      }
    else if(co->move(co->mondir)->monst != moIvyRoot) {
      // shrink useless branches, but do not remove them completely (at the root)
      if(co->monst == moIvyHead) co->move(co->mondir)->monst = moIvyHead;
      removeIvy(co);
      }
    }
  }

bool earthMove(cell *from, int dir) {
  bool b = false;
  cell *c2 = from->move(dir);
  int d = from->c.spin(dir);
  b |= earthWall(from);
  if(c2) for(int u=2; u<=c2->type-2; u++) {
    cell *c3 = c2->modmove(d + u);
    if(c3) b |= earthFloor(c3);
    }
  return b;
  }

vector<cell*> gendfs;

int targetcount;

bool isTargetOrAdjacent(cell *c) {
  for(int i=0; i<targetcount; i++)
    if(gendfs[i] == c || isNeighbor(gendfs[i], c)) 
      return true;
  return false;
  }

void groupmove2(cell *c, cell *from, int d, eMonster movtype, flagtype mf) {
  if(!c) return;

  if(c->pathdist == 0) return;

  if(movtype == moKrakenH && isTargetOrAdjacent(from)) ;
/*  else if(passable_for(movtype, from, c, P_ONPLAYER | P_CHAIN | P_MONSTER)) ;
  else if(canAttack(c, movtype, from, from->monst, AF_GETPLAYER)) ; */
  else if(passable_for(movtype, from, c, P_CHAIN | P_MONSTER)) ;
  else if(canAttack(c, movtype, from, from->monst, AF_GETPLAYER)) ;
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
        if(ok) groupmove2(c2, c, d, movtype, mf);
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
    // in the gravity lands, eagles cannot ascend in their second move
    if((mf & MF_ONLYEAGLE) && gravityLevel(c) < gravityLevel(from)) {
      onpath(c, 0);
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
        animateAttack(c, c->move(j), LAYER_SMALL, j);
        onpath(c, 0);
        // XLATC eagle
        return;
        }
    
    if(from->cpdist == 0 || from->monst) { onpath(c, 0); return; }
    
    if(movtype == moDragonHead) {
      dragon::move(from, c);
      return;
      }
    
    moveMonster(from, c, revhint(from, d));
    onpath(from, 0);
    }
  onpath(c, 0);
  // MAXGCELL
  if(isize(gendfs) < 1000 || c->cpdist <= 6) gendfs.push_back(c);
  }

void groupmove(eMonster movtype, flagtype mf) {
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
          groupmove2(c2,c,d,movtype,mf);
          }
        } 
      }
    }
  else {
    if(!peace::on) for(int i=0; i<isize(targets); i++) gendfs.push_back(targets[i]);
  
    if(invisfish && (movtype == moSlime || movtype == moShark || movtype == moKrakenH)) for(int i=0; i<numplayers(); i++) {
      cell *c = playerpos(i);
      if(!c) continue;
      gendfs.push_back(c);
      }
    }
  
  targetcount = isize(gendfs);
  
  for(int i=0; i<isize(gendfs); i++) {
    cell *c = gendfs[i];
    int dirtable[MAX_EDGE], qdirtable=0;
    
    forCellIdAll(c2,t,c) dirtable[qdirtable++] = t;
    hrandom_shuffle(dirtable, qdirtable);

    while(qdirtable--) {
      int t = dirtable[qdirtable];
      groupmove2(c->move(t),c,t,movtype,mf);
      }
      
    if(movtype == moEagle && c->monst == moNone && !isPlayerOn(c)) {
      cell *c2 = whirlwind::jumpFromWhereTo(c, false);
      groupmove2(c2, c, NODIR, movtype, mf);
      }
    }

  if(movtype != moDragonHead) for(int i=0; i<isize(dcal); i++) {
    cell *c = dcal[i];
    if((mf & MF_ONLYEAGLE) && c->monst != moEagle && c->monst != moBat) return;
    if(movegroup(c->monst) == movtype && c->pathdist != 0) {
      cell *c2 = moveNormal(c, mf);
      if(c2) onpath(c2, 0);
      }
    }
  }

// Hex monsters

vector<cell*> hexdfs;

// note: move from 'c' to 'from'!
void moveHexSnake(cell *from, cell *c, int d, bool mounted) {
  if(from->wall == waBoat) from->wall = waSea;
  moveEffect(from, c, c->monst, revhint(from, d));
  from->monst = c->monst; from->mondir = d; from->hitpoints = c->hitpoints;
  c->monst = moHexSnakeTail;
  preventbarriers(from);
  
  animateMovement(c, from, LAYER_BIG, revhint(from, d));
  mountmove(from, from->mondir, true, c);
  
  cell *c2 = c, *c3=c2;
  for(int a=0;; a++) if(c2->monst == moHexSnakeTail) {
    if(a == ROCKSNAKELENGTH) { c2->monst = moNone, c3->mondir = NODIR; break; }
    if(c2->mondir == NODIR) break;
    mountmove(c2, c2->mondir, true, c2->move(c2->mondir));
    animateMovement(c2->move(c2->mondir), c2, LAYER_BIG, revhint(c2, c2->mondir));
    c3 = c2, c2 = c3->move(c2->mondir);
    }
    else break;
  }

void snakeAttack(cell *c, bool mounted) {
  for(int j=0; j<c->type; j++) 
    if(c->move(j) && canAttack(c, moHexSnake, c->move(j), c->move(j)->monst, 
      mounted ? AF_ONLY_ENEMY : (AF_ONLY_FBUG | AF_GETPLAYER))) {
        eMonster m2 = c->move(j)->monst;
        attackMonster(c->move(j), AF_NORMAL | AF_GETPLAYER | AF_MSG, moHexSnake);
        produceGhost(c->move(j), moHexSnake, m2);
        }
  }

bool goodmount(cell *c, bool mounted) {
  if(mounted) return isMounted(c);
  else return !isMounted(c);
  }

int inpair(cell *c, int colorpair) {
  return (colorpair >> pattern_threecolor(c)) & 1;
  }

int snake_pair(cell *c) {
  if(c->mondir == NODIR)
    return (1 << pattern_threecolor(c));
  else
    return (1 << pattern_threecolor(c)) | (1 << pattern_threecolor(c->move(c->mondir)));
  }

// note: move from 'c' to 'from'!
void hexvisit(cell *c, cell *from, int d, bool mounted, int colorpair) {
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
    moveHexSnake(from, c, d, mounted);
    }

  onpath(c, 0);

  // MAXGCELL
  if(isize(hexdfs) < 2000 || c->cpdist <= 6) 
    hexdfs.push_back(c);
  }

void movehex(bool mounted, int colorpair) {
  pathdata pd(3);
  hexdfs.clear();
  
  if(mounted) { 
    if(dragon::target && dragon::target->monst != moHexSnake) {
      hexdfs.push_back(dragon::target); 
      onpath(dragon::target, 0);
      }
    }
  else for(cell *c: targets) {
    hexdfs.push_back(c);
    onpath(c, 0);
    }
  //hexdfs.push_back(cwt.at);
  
  for(int i=0; i<isize(hexdfs); i++) {
    cell *c = hexdfs[i];
    int dirtable[MAX_EDGE], qdirtable=0;
    for(int t=0; t<c->type; t++) if(c->move(t) && inpair(c->move(t), colorpair))
      dirtable[qdirtable++] = t;
      
    hrandom_shuffle(dirtable, qdirtable);
    while(qdirtable--) {
      int t = dirtable[qdirtable];
      hexvisit(c->move(t), c, t, mounted, colorpair);
      }
    }
  }

void movehex_rest(bool mounted) {
  for(int i=0; i<isize(hexsnakes); i++) {
    cell *c = hexsnakes[i];
    int colorpair;
    if(c->monst == moHexSnake) {
      colorpair = snake_pair(c);
      if(!goodmount(c, mounted)) continue;
      int t[MAX_EDGE];
      for(int i=0; i<c->type; i++) t[i] = i;
      for(int j=1; j<c->type; j++) swap(t[j], t[hrand(j+1)]);
      for(int u=0; u<c->type; u++) {
        createMov(c, t[u]);
        if(inpair(c->move(t[u]), colorpair))
          hexvisit(c, c->move(t[u]), c->c.spin(t[u]), mounted, colorpair);
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

void movemutant() {
  vector<cell*> young;
  for(cell *c: currentmap->allcells())
    if(c->monst == moMutant && c->stuntime == mutantphase)
      young.push_back(c);
  
  for(int j=1; j<isize(young); j++)
    swap(young[j], young[hrand(j+1)]);

  mutantphase++;
  mutantphase &= 15;
  
  for(int i=0; i<isize(young); i++) {
    cell *c = young[i];
    if(clearing::buggyplant) {  if(c->monst == moMutant) c->monst=moNone; continue; }
    for(int j=0; j<c->type; j++) {
      cell *c2 = c->move(j);
      if(!c2) continue;

      if(c2->monst != moMutant && canAttack(c, moMutant, c2, c2->monst, AF_ONLY_FBUG | AF_GETPLAYER)) {
        attackMonster(c2, AF_NORMAL | AF_MSG | AF_GETPLAYER, moMutant);
        continue;
        }
      
      if(isPlayerOn(c2)) continue;

      if((c2->land == laOvergrown || !pseudohept(c2)) && passable(c2, c, 0)) {
        if(c2->land == laClearing && !bounded && c2->mpdist > 7) continue;
        c2->monst = moMutant;
        c2->mondir = c->c.spin(j);
        c2->stuntime = mutantphase;
        animateMovement(c, c2, LAYER_BIG, j);
        }
      }
    }  
  }

cell *shpos[MAXPLAYER][SHSIZE];
int cshpos = 0;

cell *lastmountpos[MAXPLAYER];

void clearshadow() {
  for(int i=0; i<SHSIZE; i++) for(int p=0; p<MAXPLAYER; p++)
    shpos[p][i] = NULL;
  }

void moveshadow() {

  cell *shfrom = NULL;

  for(int p=0; p<numplayers(); p++) {
    cell *c = shpos[p][cshpos];
    if(c && c->monst == moShadow) {
      for(int j=0; j<c->type; j++) 
        if(c->move(j) && canAttack(c, moShadow, c->move(j), c->move(j)->monst, AF_ONLY_FBUG | AF_GETPLAYER))
          attackMonster(c->move(j), AF_NORMAL | AF_MSG | AF_GETPLAYER, c->monst);
      c->monst = moNone;
      shfrom = c;
      }
    shpos[p][cshpos] = playerpos(p);
    }
  cshpos = (cshpos+1) % SHSIZE;
  for(int p=0; p<numplayers(); p++) {
    cell* where = shpos[p][cshpos];
    if(where && where->monst == moNone && where->cpdist && where->land == laGraveyard &&
      !sword::at(where)) {
      if(shfrom) animateMovement(shfrom, where, LAYER_SMALL, NOHINT);
      where->monst = moShadow;
      where->hitpoints = p;
      where->stuntime = 0;
      // the Shadow sets off the mines
      mayExplodeMine(where, moShadow);
      }
    }
  }

void moveghosts() {

  if(invismove) return;
  for(int d=0; d<8; d++) movesofgood[d].clear();  

  for(int i=0; i<isize(ghosts); i++) {
    cell *c = ghosts[i];
    
    if(c->stuntime) return;

    if(isGhostMover(c->monst)) {
      int goodmoves = 0;

      for(int k=0; k<c->type; k++) if(c->move(k) && c->move(k)->cpdist < c->cpdist)
        if(ghostmove(c->monst, c->move(k), c) && !isPlayerOn(c->move(k)))
          goodmoves++;
      
      movesofgood[goodmoves].push_back(c);
      }
    }
  
  for(int d=0; d<8; d++) for(int i=0; i<isize(movesofgood[d]); i++) {
    cell *c = movesofgood[d][i];
    if(c->stuntime) return;
    
    if(isGhostMover(c->monst) && c->cpdist >= 1) {
      
      int mdir[MAX_EDGE];

      for(int j=0; j<c->type; j++) 
        if(c->move(j) && canAttack(c, c->monst, c->move(j), c->move(j)->monst, AF_GETPLAYER | AF_ONLY_FBUG)) {
          // XLATC ghost/greater shark
          
          attackMonster(c->move(j), AF_NORMAL | AF_MSG | AF_GETPLAYER, c->monst);
          goto nextghost;
          }
    
      int qmpos = 0;
      for(int k=0; k<c->type; k++) if(c->move(k) && c->move(k)->cpdist < c->cpdist)
        if(ghostmove(c->monst, c->move(k), c))
          mdir[qmpos++] = k;
      if(!qmpos) continue;
      int d = mdir[hrand(qmpos)];
      cell *c2 = c->move(d);
      if(c2->monst == moTortoise && c2->stuntime > 1) {
        addMessage(XLAT("%The1 scares %the2 a bit!", c->monst, c2->monst));
        c2->stuntime = 1;
        }
      else moveMonster(c2, c, d);
      
      }
    nextghost: ;
    }
  }

int lastdouble = -3;

void produceGhost(cell *c, eMonster victim, eMonster who) {
  if(who != moPlayer && !items[itOrbEmpathy]) return;
  if(markOrb(itOrbUndeath) && !c->monst && isGhostable(victim)) {
    c->monst = moFriendlyGhost, c->stuntime = 0;
    if(who != moPlayer) markOrb(itOrbEmpathy);
    }
  }

bool swordAttack(cell *mt, eMonster who, cell *c, int bb) {
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

void swordAttackStatic(int bb) {
  swordAttack(cwt.at, moPlayer, sword::pos(multi::cpid, bb), bb);
  }

void swordAttackStatic() {
  for(int bb = 0; bb < 2; bb++) 
    if(sword::orbcount(bb))
      swordAttackStatic(bb);
  }

void sideAttack(cell *mf, int dir, eMonster who, int bonus, eItem orb) {
  if(!items[orb]) return;
  if(who != moPlayer && !items[itOrbEmpathy]) return;
  for(int k: {-1, 1}) {
    cell *mt = getMovR(mf, dir + k*bonus);
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
    }
  }

void sideAttack(cell *mf, int dir, eMonster who, int bonuskill) {

  int k = tkills();
  sideAttack(mf, dir, who, 1, itOrbSide1);
  sideAttack(mf, dir, who, 2, itOrbSide2);
  sideAttack(mf, dir, who, 3, itOrbSide3);

  if(who == moPlayer) {
    int kills = tkills() - k + bonuskill;
    if(kills >= 5) achievement_gain("MELEE5");
    }
  }

template<class T> void do_swords(cell *mf, cell *mt, eMonster who, const T& f) {
  for(int bb=0; bb<2; bb++) if(who == moPlayer && sword::orbcount(bb)) {
    cell *sf = sword::pos(mf, sword::angle[multi::cpid] + (bb?S21:0));
    cell *st = sword::pos(mt, sword::shift(mf, mt, sword::angle[multi::cpid]) + (bb?S21:0));
    f(st, bb);
    if(sf != st && !isNeighbor(sf,st)) {
      // also attack the in-transit cell
      if(S3 == 3) {
        forCellEx(sb, sf) if(isNeighbor(sb, st) && sb != mf && sb != mt) f(sb, bb);
        }
      else {
        forCellEx(sb, mf) if(isNeighbor(sb, st) && sb != mt) f(sb, bb);
        forCellEx(sb, mt) if(isNeighbor(sb, sf) && sb != mf) f(sb, bb);
        }
      }
    }
  }

eMonster do_we_stab_a_friend(cell *mf, cell *mt, eMonster who) {
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

void stabbingAttack(cell *mf, cell *mt, eMonster who, int bonuskill) {
  int numsh = 0, numflail = 0, numlance = 0, numslash = 0, numbb[2];
  
  numbb[0] = numbb[1] = 0;

  int backdir = neighborId(mt, mf);
  
  do_swords(mf, mt, who, [&] (cell *c, int bb) { if(swordAttack(mt, who, c, bb)) numbb[bb]++, numslash++; });
  
  for(int bb=0; bb<2; bb++) achievement_count("SLASH", numbb[bb], 0);

  if(peace::on) return;
  
  for(int t=0; t<mf->type; t++) {
    cell *c = mf->move(t);
    if(!c) continue;
    
    bool stabthere = false, away = true;
    if(logical_adjacent(mt, who, c)) stabthere = true, away = false;
    
    if(stabthere && canAttack(mt,who,c,c->monst,AF_STAB)) {
      if(c->monst != moHedge) { 
        markOrb(itOrbThorns); if(who != moPlayer) markOrb(itOrbEmpathy);
        }
      eMonster m = c->monst;
      int k = tkills();
      if(attackMonster(c, AF_STAB | AF_MSG, who)) 
        produceGhost(c, m, who);
      if(tkills() > k) numsh++;
      }

    if(away && c != mt && canAttack(mf,who,c,c->monst,AF_BACK)) {
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
          achievement_gain("PRINCESS_PACIFIST");

        if(attackMonster(c, 0, who)) numflail++;
        if(m == moVizier) produceGhost(c, m, who);
        }
      }
    }

  if(!isUnarmed(who)) forCellIdEx(c, t, mt) {
    if(!logical_adjacent(mt, who, c)) continue;
    eMonster mm = c->monst;
    int flag = AF_APPROACH;
    if(anglestraight(mt, backdir, t)) flag |= AF_HORNS;
    if(canAttack(mt,who,c,c->monst, flag)) {
      if(attackMonster(c, flag | AF_MSG, who)) numlance++;
      produceGhost(c, mm, who);
      }
    }

  if(who == moPlayer) {
    if(numsh) achievement_count("STAB", numsh, 0);
    
    if(numlance && numflail && numsh) achievement_gain("MELEE3");
  
    if(numlance + numflail + numsh + numslash + bonuskill >= 5) achievement_gain("MELEE5");
  
    if(numsh == 2) {
      if(lastdouble == turncount-1) achievement_count("STAB", 4, 0);
      lastdouble = turncount;
      }
    }
  }

bool cellDangerous(cell *c) {
  return cellUnstableOrChasm(c) || isFire(c) || c->wall == waClosedGate;
  }

// negative: die, attack friend, stay against rose, hit a wall, move against rose, hit the player

bool hasPrincessWeapon(eMonster m) {
  return m == moPalace || m == moFatGuard;
  }

int stayvalue(eMonster m, cell *c) {
  if(!passable_for(c->monst, c, NULL, P_MONSTER | P_MIRROR))
    return -1501;
  if(cellEdgeUnstable(c))
    return -1501;
  if(againstRose(c, NULL) && !ignoresSmell(c->monst)) return -1500;
  return 100;
  }

// friendly version of moveval
int movevalue(eMonster m, cell *c, cell *c2, flagtype flags) {
  int val = 0;
  
  if(isPlayerOn(c2)) val = -3000;
  else if(againstRose(c, c2)  && !ignoresSmell(m)) return -1200;
  else if(m == moPrincess && c2->stuntime && hasPrincessWeapon(c2->monst) && 
    !cellDangerous(c) && !cellDangerous(c2) && canAttack(c, m, c2, c2->monst, AF_IGNORE_UNARMED | flags)) {
    val = 15000;
    }
  else if(canAttack(c,m,c2,c2->monst,flags)) 
    val = 
    (m == moPrincessArmed && isPrincess(c2->monst)) ? 14000 : // jealousy!
    isActiveEnemy(c2,m) ? 12000 :
    (c2->monst == moSkeleton || c2->monst == moMetalBeast || 
     c2->monst == moReptile || c2->monst == moTortoise ||
     c2->monst == moSalamander || c2->monst == moTerraWarrior) ? -400 :
    isIvy(c2) ? 8000 :
    isInactiveEnemy(c2,m) ? 1000 :
    -500;

  else if(monstersnear(c2, NULL, m, NULL, c)) val = 50; // linked with mouse suicide!
  else if(passable_for(m, c2, c, 0)) {
    if(mineMarked(c2) && !ignoresPlates(m))
      val = 50;
    else
      val = 4000;
    }
  else if(passable_for(m, c2, c, P_DEADLY)) val = -1100;
  else val = -1750;

  if(c->monst == moGolem )
    val -= c2->cpdist;
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
    
    int d = c2->cpdist;
    if(d <= 3) val -= d;
    else val -= 10 * d;
    
    // the Princess also avoids stepping on pressure plates
    if(c2->wall == waClosePlate || c2->wall == waOpenPlate || c2->wall == waTrapdoor)
      val -= 5;
    }
  if(c->monst == moTameBomberbird) {
    int d = c2->cpdist;
    if(d == 1 && c->cpdist > 1) d = 5;
    if(d == 2 && c->cpdist > 2) d = 4;
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

#define STRONGWIND 99

void movegolems(flagtype flags) {
  if(items[itOrbEmpathy] && items[itOrbSlaying])
    flags |= AF_CRUSH;
  pathdata pd(moMouse);
  int qg = 0;
  for(int i=0; i<isize(golems); i++) {
    cell *c = golems[i];
    eMonster m =  c->monst;
    if(c->stuntime) continue;
    if(m == moGolem || m == moKnight || m == moTameBomberbird || m == moPrincess ||
      m == moPrincessArmed || m == moMouse || m == moFriendlyGhost) {
      if(m == moGolem) qg++;
      if(m == moFriendlyGhost) markOrb(itOrbUndeath);

      bool recorduse[ittypes];
      for(int i=0; i<ittypes; i++) recorduse[i] = orbused[i];

      DEBT("stayval");
      int bestv = stayvalue(m, c), bq = 0, bdirs[MAX_EDGE];

      DEBT("moveval");
      for(int k=0; k<c->type; k++) if(c->move(k)) {
        cell *c2 = c->move(k);
        int val = movevalue(m, c, c2, flags);

        if(val > bestv) bestv = val, bq = 0;
        if(val == bestv) bdirs[bq++] = k;
        }
      
      if(m == moTameBomberbird) {
        cell *c2 = whirlwind::jumpDestination(c);
        if(c2 && !c2->monst) {
          int val = movevalue(m, c, c2, flags);
          // printf("val = %d bestv = %d\n", 
          if(val > bestv) bestv = val, bq = 0;
          if(val == bestv) bdirs[bq++] = STRONGWIND;
          }
        }

      for(int i=0; i<ittypes; i++) orbused[i] = recorduse[i];
      
//    printf("stayvalue = %d, result = %d, bq = %d\n", stayvalue(m,c), bestv, bq);
        
      if(bq == 0) continue;
      int dir = bdirs[hrand(bq)];
      cell *c2 = dir != STRONGWIND ? c->move(dir) : whirlwind::jumpDestination(c);
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
        animateAttack(c, c2, LAYER_SMALL, dir);
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
        DEBT("move");
        moveMonster(c2, c, dir);
        if(m != moTameBomberbird && m != moFriendlyGhost) 
          moveBoatIfUsingOne(c2, c, dir);
          
        if(c2->monst == m) {          
          if(m == moGolem) c2->monst = moGolemMoved;
          if(m == moMouse) c2->monst = moMouseMoved;
          if(m == moPrincess) c2->monst = moPrincessMoved;
          if(m == moPrincessArmed) c2->monst = moPrincessArmedMoved;
          if(m == moTameBomberbird) c2->monst = moTameBomberbirdMoved;
          if(m == moKnight) c2->monst = moKnightMoved;
          if(m == moFriendlyGhost) c2->stuntime = 1;
          }

        empathyMove(c, c2, dir);
        }
      DEBT("other");
      }
    }
  achievement_count("GOLEM", qg, 0);
  }

void sageheat(cell *c, double v) {
  HEAT(c) += v;
  if(c->wall == waFrozenLake && HEAT(c) > .6) c->wall = waLake;
  }

void activateFlashFrom(cell *cf, eMonster who, flagtype flags);

vector<cell*> nonmovers;

bool sagefresh = true;

int nearestPathPlayer(cell *c) {
  for(int i=0; i<numplayers(); i++) if(playerpos(i) == c) return i;
  forCellEx(c2, c) if(c2->pathdist < c->pathdist) return nearestPathPlayer(c2);
  for(int i=0; i<numplayers(); i++) if(multi::playerActive(i)) return i;
  return 0;
  }

// note: butterflies don't use moveNormal for two reasons:
// 1) to make sure that they move AFTER bulls
// 2) to make sure that they move offscreen
void moveButterflies() {
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
void specialMoves() {
  for(int i=0; i<isize(dcal); i++) {
    cell *c = dcal[i];
    
    if(c->stuntime) continue;
    
    eMonster m = c->monst;
    
    if(m == moHunterGuard && items[itHunting] >= 10)
      c->monst = moHunterChanging;
    
    if(m == moHunterDog && (havewhat & HF_FAILED_AMBUSH) && hyperbolic && !quotient)
      c->monst = moHunterChanging;
    
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

void moveworms() {
  if(!isize(worms)) return;
  pathdata pd(moWorm);
  int wrm = isize(worms);
  for(int i=0; i<wrm; i++) {
    moveWorm(worms[i]);
    }
  }

void refreshFriend(cell *c) {
  if(c->monst == moGolemMoved) c->monst = moGolem;
  if(c->monst == moMouseMoved) c->monst = moMouse;
  if(c->monst == moPrincessMoved) c->monst = moPrincess;
  if(c->monst == moPrincessArmedMoved) c->monst = moPrincessArmed;
  if(c->monst == moKnightMoved) c->monst = moKnight;
  if(c->monst == moTameBomberbirdMoved) c->monst = moTameBomberbird;
  }

bool saved_tortoise_on(cell *c) {
  return 
    (c->monst == moTortoise && c->item == itBabyTortoise &&
    !((tortoise::getb(c) ^ tortoise::babymap[c]) & tortoise::mask));
  }

void moverefresh(bool turn = true) {
  int dcs = isize(dcal);
  
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    
    if(c->monst == moWolfMoved) c->monst = moWolf;
    if(c->monst == moIvyNext) {
      c->monst = moIvyHead; ivynext(c);
      }
    if(c->monst == moIvyDead) 
      removeIvy(c);
    refreshFriend(c);
    if(c->monst == moSlimeNextTurn) c->monst = moSlime;
    if(c->monst == moLesser && !cellEdgeUnstable(c)) c->monst = moLesserM;
    else if(c->monst == moLesserM) c->monst = moLesser;
    if(c->monst == moGreater && !cellEdgeUnstable(c)) c->monst = moGreaterM;
    else if(c->monst == moGreaterM) c->monst = moGreater;
    
    if(c->monst == moPair && !c->stuntime) {
      cell *c2 = c->move(c->mondir);
      if(c2->monst != moPair) continue;
      if(true) for(int i: {-1, 1}) {
        cell *c3 = c->modmove(c->mondir + i);
        if(among(c3->wall, waRuinWall, waColumn, waStone, waVinePlant, waPalace)) {
          drawParticles(c3, winf[c3->wall].color, 30);
          c3->wall = waNone;
          }
        }
      }
    
    if(c->stuntime && !isMutantIvy(c)) {
      c->stuntime--;
      int breathrange = sphere ? 2 : 3;
      if(c->stuntime == 0 && c->monst == moDragonHead)  {
        // if moDragonHead is renamed to "Dragon Head", we might need to change this
        eMonster subject = c->monst;
        if(!c->hitpoints) c->hitpoints = 1;
        else if(shmup::on && dragon::totalhp(c) > 2 && shmup::dragonbreath(c)) {
          c->hitpoints = 0;
          }
        else if(dragon::totalhp(c) <= 2) ;
        else if(isMounted(c)) {
          if(dragon::target && celldistance(c, dragon::target) <= breathrange && makeflame(dragon::target, 5, true)) {
            addMessage(XLAT("%The1 breathes fire!", subject));
            makeflame(dragon::target, 5, false);
            playSound(dragon::target, "fire");
            c->hitpoints = 0;
            }
          }
        else {
          for(int i=0; i<isize(targets); i++) {
            cell *t = targets[i];
            if(celldistance(c, t) <= breathrange && makeflame(t, 5, true)) {
              if(isPlayerOn(t)) addMessage(XLAT("%The1 breathes fire at you!", subject));
              else if(t->monst)
                addMessage(XLAT("%The1 breathes fire at %the2!", subject, t->monst));
              else
                addMessage(XLAT("%The1 breathes fire!", subject));
              makeflame(t, 5, false);
              playSound(t, "fire");
              c->hitpoints = 0;
              }
            }
          }
        }
      }
    
    // tortoises who have found their children no longer move
    if(saved_tortoise_on(c))
      c->stuntime = 2;
    
    if(c->monst == moReptile) {
      if(c->wall == waChasm || cellUnstable(c)) {
        c->monst = moNone;
        c->wall = waReptile;
        c->wparam = reptilemax();
        playSound(c, "click");
        }
      else if(isChasmy(c) || isWatery(c)) {
        if(c->wall == waMercury) {
          fallMonster(c, AF_FALL);
          c->wall = waNone;
          }
        else {
          c->wall = waReptileBridge;
          c->wparam = reptilemax();
          c->monst = moNone;
          }
        c->item = itNone;
        playSound(c, "click");
        }
      }
        
    if(c->wall == waChasm) {
      if(c->land != laWhirlwind) c->item = itNone;
      
      if(c->monst && !survivesChasm(c->monst) && c->monst != moReptile) {
        if(c->monst != moRunDog && c->land == laMotion) 
          achievement_gain("FALLDEATH1");
        addMessage(XLAT("%The1 falls!", c->monst));
        fallMonster(c, AF_FALL);
        }
      }
    
    else if(isReptile(c->wall) && turn) {
      if(c->monst || isPlayerOn(c)) c->wparam = -1;
      else if(c->cpdist <= 7) {
        c->wparam--;
        if(c->wparam == 0) {
          if(c->wall == waReptile) c->wall = waChasm;
          else placeWater(c, NULL);
          c->monst = moReptile;
          c->hitpoints = 3;
          c->stuntime = 0;
          int gooddirs[MAX_EDGE], qdirs = 0;
          // in the peace mode, a reptile will
          // prefer to walk on the ground, rather than the chasm
          for(int i=0; i<c->type; i++) {
            int i0 = (i+3) % c->type;
            int i1 = (i+c->type-3) % c->type;
            if(c->move(i0) && passable(c->move(i0), c, 0)) 
            if(c->move(i1) && passable(c->move(i1), c, 0)) 
              gooddirs[qdirs++] = i;
            }
          if(qdirs) c->mondir = gooddirs[hrand(qdirs)];
          playSound(c, "click");
          }
        }
      }

    else if(isFire(c)) {
      if(c->monst == moSalamander) c->stuntime = max<int>(c->stuntime, 1);
      else if(c->monst && !survivesFire(c->monst) && !isWorm(c->monst)) {
        addMessage(XLAT("%The1 burns!", c->monst));
        if(isBull(c->monst)) {
          addMessage(XLAT("Fire is extinguished!"));
          c->wall = waNone;
          }
        fallMonster(c, AF_CRUSH);
        }
      if(c->item && itemBurns(c->item)) {
        addMessage(XLAT("%The1 burns!", c->item));
        c->item = itNone;
        }
      }
    
    else if(isWatery(c)) {
      if(c->monst == moLesser || c->monst == moLesserM || c->monst == moGreater || c->monst == moGreaterM)
        c->monst = moGreaterShark;
      if(c->monst && !survivesWater(c->monst)) {
        playSound(c, "splash"+pick12());
        if(isNonliving(c->monst))
          addMessage(XLAT("%The1 sinks!", c->monst));
        else 
          addMessage(XLAT("%The1 drowns!", c->monst));
        if(isBull(c->monst)) {
          addMessage(XLAT("%The1 is filled!", c->wall));
          c->wall = waNone;
          }
        fallMonster(c, AF_FALL);
        }
      }
    else if(c->wall == waSulphur || c->wall == waSulphurC || c->wall == waMercury) {
      if(c->monst && !survivesPoison(c->monst, c->wall)) {
        playSound(c, "splash"+pick12());
        if(isNonliving(c->monst))
          addMessage(XLAT("%The1 sinks!", c->monst));
        else 
          addMessage(XLAT("%The1 drowns!", c->monst));
        if(isBull(c->monst)) {
          addMessage(XLAT("%The1 is filled!", c->wall));
          c->wall = waNone;
          }
        fallMonster(c, AF_FALL);
        }
      }
    else if(c->wall == waMagma) {
      if(c->monst == moSalamander) c->stuntime = max<int>(c->stuntime, 1);
      else if(c->monst && !survivesPoison(c->monst, c->wall)) {
        if(isNonliving(c->monst))
          addMessage(XLAT("%The1 is destroyed by lava!", c->monst));
        else 
          addMessage(XLAT("%The1 is killed by lava!", c->monst));
        playSound(c, "steamhiss", 70);
        fallMonster(c, AF_FALL);
        }
      }
    else if(!isWateryOrBoat(c)) {
      if(c->monst == moGreaterShark)
        c->monst = moGreaterM;
      else if(c->monst == moShark || c->monst == moCShark) {
        addMessage(XLAT("%The1 suffocates!", c->monst));
        fallMonster(c, AF_CRUSH);
        }
      else if(c->monst == moKrakenH) {
        addMessage(XLAT("%The1 suffocates!", c->monst));
        kraken::kill(c, moNone);
        }
      }
    
    if(c->monst == moVineSpirit && !cellHalfvine(c) && c->wall != waVinePlant) {
      addMessage(XLAT("%The1 is destroyed!", c->monst));
      fallMonster(c, AF_CRUSH);
      }
    
    if(c->monst) mayExplodeMine(c, c->monst);
    
    if(c->monst && c->wall == waClosedGate && !survivesWall(c->monst)) {
      playSound(c, "hit-crush"+pick123());
      addMessage(XLAT("%The1 is crushed!", c->monst));
      fallMonster(c, AF_CRUSH);
      }

    if(c->monst && cellUnstable(c) && !ignoresPlates(c->monst) && !shmup::on) 
      doesFallSound(c);
    }
  }

void consMove(cell *c, eMonster param) {
  eMonster m = c->monst;

  if(!normalMover(m)) return;
  
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
    movesofgood[goodmoves].push_back(c);
    }
  else 
    movesofgood[0].push_back(c);
  }

void moveNormals(eMonster param) {
  pathdata pd(param);

  for(int d=0; d<8; d++) movesofgood[d].clear();

  for(int i=0; i<isize(pathqm); i++) 
    consMove(pathqm[i], param);
  
  int dcs = isize(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->pathdist == PINFD) consMove(c, param);
    }

  for(int d=0; d<8; d++) for(int i=0; i<isize(movesofgood[d]); i++) {
    cell *c = movesofgood[d][i];
    if(normalMover(c->monst)) {
      moveNormal(c, MF_PATHDIST);
      }
    }
  }

void markAmbush(cell *c, manual_celllister& cl) {
  if(!cl.add(c)) return;
  forCellEx(c2, c) 
    if(c2->cpdist < c->cpdist) 
      markAmbush(c2, cl);
  }

int ambush_distance;
bool ambushed;

void checkAmbushState() {
  if(havewhat & HF_HUNTER) {
    manual_celllister cl;
    for(cell *c: dcal) {
      if(c->monst == moHunterDog) {
        if(c->cpdist > ambush_distance)
          ambush_distance = c->cpdist;
        markAmbush(c, cl);
        }
      if(c->monst == moHunterGuard && c->cpdist <= 4) 
        markAmbush(c, cl);
      }
    if(items[itHunting] > 5 && items[itHunting] <= 22) {
      int q = 0;
      for(int i=0; i<numplayers(); i++) 
        forCellEx(c2, playerpos(i))
          if(cl.listed(c2))
            q++;
      if(q == 1) havewhat |= HF_FAILED_AMBUSH;
      if(q == 2) {
        for(int i=0; i<numplayers(); i++) 
        forCellEx(c2, playerpos(i))
          if(cl.listed(c2))
            forCellEx(c3, playerpos(i)) 
              if(c3 != c2 && isNeighbor(c2,c3))
              if(cl.listed(c3))
                havewhat |= HF_FAILED_AMBUSH;
        }
      if(havewhat & HF_FAILED_AMBUSH && ambushed) {
        addMessage(XLAT("The Hunting Dogs give up."));
        ambushed = false;
        }        
      }
    }
  
  }

void movehex_all() {
  for(int i: snaketypes) {
    movehex(false, i);
    if(!shmup::on && haveMount()) movehex(true, i);
    }
  movehex_rest(false);
  movehex_rest(true);
  }
  
void movemonsters() {
  ambush_distance = 0;

  DEBT("lava1");
  orboflava(1);
  
  checkAmbushState();

  sagefresh = true;
  turncount++;

  specialMoves();

  DEBT("ghosts");
  moveghosts();
    
  DEBT("butterflies");
  moveButterflies();

  DEBT("normal");
  moveNormals(moYeti);

  DEBT("slow");
  if(havewhat & HF_SLOW) moveNormals(moTortoise);
  
  if(sagefresh) sagephase = 0;
  
  DEBT("ivy");
  moveivy();
  DEBT("slimes");
  groupmove(moSlime, 0);
  DEBT("sharks");
  if(havewhat & HF_SHARK) groupmove(moShark, 0);
  DEBT("eagles");
  if(havewhat & HF_BIRD) groupmove(moEagle, 0);
  if(havewhat & HF_EAGLES) groupmove(moEagle, MF_NOATTACKS | MF_ONLYEAGLE);
  DEBT("eagles");
  if(havewhat & HF_REPTILE) groupmove(moReptile, 0);
  DEBT("air");
  if(havewhat & HF_AIR) {
    airmap.clear();
    groupmove(moAirElemental, 0); 
    buildAirmap();
    }
  DEBT("earth");
  if(havewhat & HF_EARTH) groupmove(moEarthElemental, 0);
  DEBT("water");
  if(havewhat & HF_WATER) groupmove(moWaterElemental, 0);
  DEBT("void");
  if(havewhat & HF_VOID) groupmove(moVoidBeast, 0);
  DEBT("leader");
  if(havewhat & HF_LEADER) groupmove(moPirate, 0);
  DEBT("mutant");
  if((havewhat & HF_MUTANT) || (bounded && among(specialland, laOvergrown, laClearing))) movemutant();
  DEBT("bugs");
  if(havewhat & HF_BUG) hive::movebugs();
  DEBT("whirlpool");
  if(havewhat & HF_WHIRLPOOL) whirlpool::move();
  DEBT("whirlwind");
  if(havewhat & HF_WHIRLWIND) whirlwind::move();
  DEBT("river");
  if(havewhat & HF_RIVER) prairie::move();
  /* DEBT("magnet");
  if(havewhat & HF_MAGNET) 
    groupmove(moSouthPole, 0),
    groupmove(moNorthPole, 0); */
  DEBT("bugs");
  if(havewhat & HF_HEXD) groupmove(moHexDemon, 0);
  if(havewhat & HF_ALT) groupmove(moAltDemon, 0);
  if(havewhat & HF_MONK) groupmove(moMonk, 0);

  DEBT("worm");
  cell *savepos[MAXPLAYER];
  
  for(int i=0; i<numplayers(); i++)
    savepos[i] = playerpos(i);

  moveworms();
  if(havewhat & HF_HEX) 
    movehex_all();

  if(havewhat & HF_KRAKEN) kraken::attacks(), groupmove(moKrakenH, 0);
  if(havewhat & HF_DRAGON) groupmove(moDragonHead, MF_NOFRIEND);
  if(haveMount()) groupmove(moDragonHead, MF_MOUNT);

  DEBT("golems");
  movegolems(0);
  
  DEBT("fresh");
  moverefresh();
  
  DEBT("lava2");
  orboflava(2);

  DEBT("shadow");
  moveshadow();
  
  DEBT("wandering");
  wandering();
  
  DEBT("rosemap");
  if(havewhat & HF_ROSE) buildRosemap();

  for(int i=0; i<numplayers(); i++)
    if(savepos[i] != playerpos(i)) {
      bfs(); break;
      }
  }

// move heat

// move the PC in direction d (or stay in place for d == -1)

bool checkNeedMove(bool checkonly, bool attacking) {
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
    if(checkonly) return true;
    flags |= AF_FALL;
    addMessage(XLAT("Ice below you is melting! RUN!"));
    }
  else if(!attacking && cellEdgeUnstable(cwt.at)) {
    if(markOrb2(itOrbAether)) return false;
    if(checkonly) return true;
    addMessage(XLAT("Nothing to stand on here!"));
    }
  else if(cwt.at->wall == waSea || cwt.at->wall == waCamelotMoat) {
    if(markOrb(itOrbFish)) return false;
    if(markOrb2(itOrbAether)) return false;
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
    if(checkonly) return true;
    addMessage(XLAT("Run away from the magma!"));
    }
  else if(cwt.at->wall == waChasm) {
    if(markOrb2(itOrbAether)) return false;
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

int countMyGolems(eMonster m) {
  int g=0, dcs = isize(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->monst == m) g++;
    }
  return g;
  }

int savePrincesses() {
  int g=0, dcs = isize(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(isPrincess(c->monst)) princess::save(c);
    }
  return g;
  }

int countMyGolemsHP(eMonster m) {
  int g=0, dcs = isize(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->monst == m) g += c->hitpoints;
    }
  return g;
  }

void restoreGolems(int qty, eMonster m, int hp = 0) {
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

void placeItems(int qty, eItem it) {
  int dcs = isize(dcal);
  for(int i=1; qty && i<dcs; i++) {
    cell *c = dcal[i];
    if(!c->monst && !c->item && passable(c, NULL, 0))
      c->item = it, qty--;
    }
  }

cell *recallCell;

bool activateRecall() {
  if(!recallCell) {
    addMessage("Error: no recall");
    return false;
    }
  items[itOrbRecall] = 0; items[itOrbSafety] = 0;
  if(!makeEmpty(recallCell)) {
    addMessage(XLAT("Your Orb of Recall is blocked by something big!"));
    recallCell = NULL;
    return false;
    }

  killFriendlyIvy();
  movecost(cwt.at, recallCell, 3);
  playerMoveEffects(cwt.at, recallCell);
  mirror::destroyAll();
  
  sword::reset();

  cwt.at = recallCell; recallCell = NULL;
  cwt.spin = hrand(cwt.at->type); flipplayer = !!(hrand(2));
  fullcenter(); 
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

void saveRecall(cell *c2) {  
  if(!recallCell) recallCell = c2;
  }

void activateSafety(eLand l) {
  if(recallCell && activateRecall()) 
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
  if(l == laCamelot && !(tactic::on && specialland == laCamelot)) 
    l = laCrossroads;
  firstland = l;
  safetyland = l;
  safetyseed = hrandpos();
  clear_euland(firstland);
  safety = true; avengers = 0;
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

bool legalmoves[MAX_EDGE+1];

bool hasSafeOrb(cell *c) {
  return 
    c->item == itOrbSafety ||
    c->item == itOrbShield ||
    c->item == itOrbShell  ||
    (c->item == itOrbYendor && yendor::state(c) == yendor::ysUnlocked);
  }

void checkmove() {

#if CAP_INV
  if(inv::on) inv::compute();
#endif

  if(multi::players > 1 && !multi::checkonly) return;
  if(hardcore) return;
  bool orbusedbak[ittypes];
  
  // do not activate orbs!
  for(int i=0; i<ittypes; i++) orbusedbak[i] = orbused[i];

  for(int i=0; i<=MAX_EDGE; i++) legalmoves[i] = false;

  canmove = false;
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
  if(recallCell && !markOrb(itOrbRecall)) activateRecall();  
  }

// move the PC. Warning: a very long function! todo: refactor

void placeGolem(cell *on, cell *moveto, eMonster m) {
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

bool multiRevival(cell *on, cell *moveto) {
  int fl = 0;
  if(items[itOrbAether]) fl |= P_AETHER;
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

void movecost(cell* from, cell *to, int phase) {
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
  
  if(to->land ==laCrossroads4 && !chaosUnlocked && !geometry && (phase & 2)) {
    achievement_gain("CR4");
    chaosUnlocked = true;
    }

  if(isHaunted(from->land) && !isHaunted(to->land) && (phase & 2)) {
    updateHi(itLotus, truelotus = items[itLotus]);
    if(items[itLotus] >= 1) achievement_gain("LOTUS1");
    if(items[itLotus] >= (inv::on ? 25 : 10)) achievement_gain("LOTUS2");
    if(items[itLotus] >= (inv::on ? 50 : 25)) achievement_gain("LOTUS3");
    if(items[itLotus] >= 50 && !inv::on) achievement_gain("LOTUS4");
    achievement_final(false);
    }
  
  if(celldist(to) == 0 && !usedSafety && gold() >= 100 && (phase & 2))
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

bool cantGetGrimoire(cell *c2, bool verbose = true) {
  if(chaosmode) return false;
  if(!eubinary && !c2->master->alt) return false;
  if(c2->item == itGrimoire && items[itGrimoire] > celldistAlt(c2)/-TEMPLE_EACH) {
    if(verbose)
      addMessage(XLAT("You already have this Grimoire! Seek new tomes in the inner circles."));
    return true;
    }
  return false;
  }

void gainLife() {
  items[itOrbLife] ++;
  if(items[itOrbLife] > 5 && !shmup::on) items[itOrbLife] = 5;
  }

void collectMessage(cell *c2, eItem which) {
  bool specialmode = 
    yendor::on || tactic::on || princess::challenge || euclid || sphere;
  
  if(which == itDodeca && peace::on) return;  
  if(which == itTreat) ;    
  else if(isElementalShard(which)) {
    int tsh = 
      items[itFireShard] + items[itAirShard] + items[itWaterShard] + items[itEarthShard] +
      items[itElemental];
    if(tsh == 0) {
      addMessage(XLAT("Collect four different Elemental Shards!"));
      addMessage(XLAT("Unbalanced shards in your inventory are dangerous."));
      }
    else {
      string t = XLAT("You collect %the1. (%2)", which, its(items[which]+1));
      addMessage(t);
      }
    }
  else if(which == itKey) {
    addMessage(XLAT("You have found the Key! Now unlock this Orb of Yendor!"));
    }
  else if(which == itGreenStone && !items[itGreenStone])
    addMessage(XLAT("This orb is dead..."));
  else if(which == itGreenStone)
    addMessage(XLAT("Another Dead Orb."));
  else if(itemclass(which) != IC_TREASURE) {
    if(!inv::activating)
      addMessage(XLAT("You have found %the1!", which));
    }
  else if(which == itBabyTortoise) {
    playSound(c2, playergender() ? "speak-princess" : "speak-prince");
    addMessage(XLAT("Aww, poor %1... where is your family?", which));
    }
  else if(gold() == 0 && !specialmode)
    addMessage(XLAT("Wow! %1! This trip should be worth it!", which));
  else if(gold() == 1 && !specialmode)
    addMessage(XLAT("For now, collect as much treasure as possible..."));
  else if(gold() == 2 && !specialmode)
    addMessage(XLAT("Prove yourself here, then find new lands, with new quests..."));
  else if(!items[which] && itemclass(which) == IC_TREASURE)
    addMessage(XLAT("You collect your first %1!", which));
  else if(items[which] == 4 && maxgold() == U5-1 && !specialmode) {
    addMessage(XLAT("You feel that %the2 become%s2 more dangerous.", which, c2->land));
    addMessage(XLAT("With each %1 you collect...", which, c2->land));
    }
  else if(items[which] == 9 && maxgold() == 9 && !specialmode) {
    if(inv::on) {
      addMessage(XLAT("The treasure gives your magical powers!", c2->land));
      if(!ISMOBILE) 
        addMessage(XLAT("Press 'i' to access your magical powers.", c2->land));
      }
    else
      addMessage(XLAT("Are there any magical orbs in %the1?...", c2->land));
    }
  else if(items[which] == R10 && maxgold() == R10 && !specialmode && !inv::on) {
    addMessage(XLAT("You feel that %the1 slowly become%s1 dangerous...", c2->land));
    addMessage(XLAT("Better find some other place."));
    }
  else if(which == itHunting && items[itHunting] == 4 && !specialmode && !ISMOBWEB)
    addMessage(XLAT("Hint: hold Alt to highlights enemies and other important features."));
  else if(which == itSpice && items[itSpice] == U10*7/10 && !specialmode)
    addMessage(XLAT("You have a vision of the future, fighting demons in Hell..."));
  else if(which == itSpice && items[itSpice] == U10-1 && !specialmode)
    addMessage(XLAT("You will be fighting red rock snakes, too..."));
  else if(which == itKraken && items[itKraken] == U10-1 && !specialmode)
    addMessage(XLAT("You feel that a magical weapon is waiting for you..."));
//  else if(which == itFeather && items[itFeather] == 10)
//    addMessage(XLAT("There should be a Palace somewhere nearby..."));
  else if(which == itElixir && items[itElixir] == U5-1 && !specialmode)
    addMessage(XLAT("With this Elixir, your life should be long and prosperous..."));
  else if(which == itRuby && items[itRuby] == U5-1 && !specialmode) {
    addMessage(XLAT("You feel something strange about gravity here..."));
    }
  else if(which == itPalace && items[itPalace] == U5-1 && !specialmode) {
    addMessage(XLAT("The rug depicts a man in a deep dungeon, unable to leave."));
    }
  else if(which == itFeather && items[itFeather] == 25-1 && !specialmode && inv::on)
    addMessage(XLAT("You feel the presence of free saves on the Crossroads."));
  else if(which == itHell && items[itHell] == 25-1 && !specialmode && inv::on)
    addMessage(XLAT("You feel the Orbs of Yendor nearby..."));
  else if(which == itHell && items[itHell] == 50-1 && !specialmode && inv::on)
    addMessage(XLAT("You feel the Orbs of Yendor in the Crossroads..."));
  else if(which == itHell && items[itHell] == 100-1 && !specialmode && inv::on)
    addMessage(XLAT("You feel the Orbs of Yendor everywhere..."));
  else if(which == itBone && items[itBone] % 25 == 24 && !specialmode && inv::on)
    addMessage(XLAT("You have gained an offensive power!"));
  else if(which == itHell && items[itHell] >= 100 && items[itHell] % 25 == 24 && !specialmode && inv::on)
    addMessage(XLAT("A small reward for braving the Hell."));
  else if(which == itIvory && items[itIvory] == U5-1 && !specialmode) {
    addMessage(XLAT("You feel attuned to gravity, ready to face mountains and dungeons."));
    }
  else if(which == itBone && items[itBone] == U5+1 && !specialmode)
    addMessage(XLAT("The Necromancer's Totem contains hellish incantations..."));
  else if(which == itStatue && items[itStatue] == U5+1 && !specialmode)
    addMessage(XLAT("The inscriptions on the Statue of Cthulhu point you toward your destiny..."));
  else if(which == itStatue && items[itStatue] == U5-1 && !specialmode)
    addMessage(XLAT("There must be some temples of Cthulhu in R'Lyeh..."));
  else if(which == itDiamond && items[itDiamond] == U10-2 && !specialmode)
    addMessage(XLAT("Still, even greater treasures lie ahead..."));
  else if(which == itFernFlower && items[itFernFlower] == U5-1 && !specialmode)
    addMessage(XLAT("You overheard Hedgehog Warriors talking about emeralds..."));
  else if(which == itEmerald && items[itEmerald] == U5-1 && !specialmode && !chaosmode)
    addMessage(XLAT("You overhear miners talking about a castle..."));
  else if(which == itEmerald && items[itEmerald] == U5 && !specialmode && !chaosmode)
    addMessage(XLAT("A castle in the Crossroads..."));
  else if(which == itShard) ;
  else {
    int qty = (which == itBarrow) ? c2->landparam : 1;
    string t;
    if(which == itBarrow && items[which] < 25 && items[which] + qty >= 25)
      t = XLAT("Your energy swords get stronger!");
    else if(maxgold() < 25 && items[which] + qty >= 25)
      t = XLAT("You feel even more attuned to the magic of this land!");
    else t = XLAT("You collect %the1. (%2)", which, its(items[which]+qty));
    addMessage(t);
    }
  }

int ambushval;

int ambushSize(cell *c, eItem what) {
  bool restricted = false;
  for(cell *c2: dcal) {
    if(c2->cpdist > 3) break;
    if(c2->monst) restricted = true;
    }

  int qty = items[itHunting];
  if(ambushval)
    return ambushval;
  switch(what) {
    case itCompass:
      return 0;
    
    case itHunting:
      return min(min(qty, max(33-qty, 6)), 15);
    
    case itOrbSide3:
      return restricted ? 10 : 20;
    
    case itOrbFreedom:
      return restricted ? 10 : 60;
    
    case itOrbThorns:
    case itOrb37:
      return 20;
    
    case itOrbLava:
      return 20;
    
    case itOrbBeauty:
      return 35;
    
    case itOrbShell:
      return 35;

    case itOrbPsi:
      // return 40; -> no benefits
      return 20;

    case itOrbDash:
    case itOrbFrog:
      return 40;
    
    case itOrbAir:
    case itOrbDragon:
      return 50;

    case itOrbStunning:
      // return restricted ? 50 : 60; -> no benefits
      return 30;
    
    case itOrbBull:
    case itOrbSpeed:
    case itOrbShield:
      return 60;

    case itOrbInvis:
      return 80;

    case itOrbTeleport:
      return 300;
    
    case itGreenStone:
    case itOrbSafety:
    case itOrbYendor:
      return 0;
    
    case itKey:
      return 16;

    case itWarning:
      return qty;
    
    default:
      return restricted ? 6 : 10;
      break;    
    
    // Flash can survive about 70, but this gives no benefits
    }
  
  }

int ambush(cell *c, eItem what) {
  int maxdist = gamerange();
  celllister cl(c, maxdist, 1000000, NULL);
  cell *c0 = c;
  int d = 0;
  int dogs0 = 0;
  for(cell *cx: cl.lst) {
    int dh = cl.getdist(cx);
    if(dh <= 2 && cx->monst == moHunterGuard)
      cx->monst = moHunterDog, dogs0++;
    if(dh > d) c0 = cx, d = dh;
    }
  if(sphere) {
    int dogs = ambushSize(c, what);  
    for(int i = cl.lst.size()-1; i>0 && dogs; i--) 
      if(!isPlayerOn(cl.lst[i]) && !cl.lst[i]->monst)
        cl.lst[i]->monst = moHunterDog, dogs--;
    }
  vector<cell*> around;
  cell *clast = NULL;
  cell *ccur = c0;
  for(int tries=0; tries<10000; tries++) {
    cell *c2 = NULL;
    forCellEx(c1, ccur)
      if(c1 != clast && cl.listed(c1) && cl.getdist(c1) == d)
        c2 = c1;
    if(!c2) break;
    if(c2->land == laHunting && c2->wall == waNone && c2->monst == moNone)
      around.push_back(c2);
    clast = ccur; ccur = c2;
    if(c2 == c0) break;
    }
  int N = isize(around);
  int dogs = ambushSize(c, what);  
  
  int gaps = dogs;
  if(!N) return dogs0;
  ambushed = true;
  int shift = hrand(N);
  dogs = min(dogs, N);
  gaps = min(gaps, N);
  for(int i=0; i<dogs; i++) {
    int pos = (shift + (N * i) / gaps) % N;
    cell *nextdog = around[pos];
    nextdog->monst = moHunterDog;
    nextdog->stuntime = 1;
    drawFlash(nextdog);
    }
  return dogs + dogs0;
  }

bool cannotPickupItem(cell *c, bool telekinesis) {
  return itemHidden(c) && !telekinesis && !(isWatery(c) && markOrb(itOrbFish));
  }

bool canPickupItemWithMagnetism(cell *c, cell *from) {
  if(!c->item || c->item == itOrbYendor || isWall(c) || cannotPickupItem(c, false))
    return false;
  if(c->item == itCompass && from->item) 
    return false;
  return true;
  }

bool doPickupItemsWithMagnetism(cell *c) {
  cell *csaf = NULL;
  if(items[itOrbMagnetism])
    forCellEx(c3, c) if(canPickupItemWithMagnetism(c3, c)) {
      if(c3->item == itCompass) {
        if(!c->item)
          moveItem(c3, c, false);
        }
      else if(c3->item == itOrbSafety || c3->item == itBuggy || c3->item == itBuggy2)
        csaf = c3;
      else if(markOrb(itOrbMagnetism))
        collectItem(c3, false);
      }
  if(csaf)
    return collectItem(csaf, false);
  return false;
  }

void pickupMovedItems(cell *c) {
  if(!c->item) return;
  if(isPlayerOn(c)) collectItem(c, true);  
  if(items[itOrbMagnetism])
    forCellEx(c2, c)
      if(isPlayerOn(c2) && canPickupItemWithMagnetism(c, c2))
        collectItem(c, true);
  }

bool collectItem(cell *c2, bool telekinesis) {

  int pg = gold();
  bool dopickup = true;
  
  if(cannotPickupItem(c2, telekinesis))
    return false;
  
  /* if(c2->item == itHolyGrail && telekinesis)
    return false; */

  if(c2->item) {
    invismove = false;
    if(shmup::on) shmup::visibleFor(2000);
    string s0 = "";
    
    if(c2->item == itPalace && items[c2->item] == 12)
      princess::forceVizier = true;
    
    if(!cantGetGrimoire(c2, false)) collectMessage(c2, c2->item);
    if(c2->item == itDodeca && peace::on) peace::simon::extend();
    }

  if(c2->land == laHunting && c2->item && !inv::activating) {
    int dogs = ambush(c2, c2->item);
    if(dogs)
      addMessage(XLAT("You are ambushed!"));
    }
  
  if(isRevivalOrb(c2->item) && multi::revive_queue.size()) {
    multiRevival(cwt.at, c2);
    }
  else if(isShmupLifeOrb(c2->item) && shmup::on) {
    playSound(c2, "pickup-orb"); // TODO summon
    gainLife();
    }
  else if(orbcharges(c2->item)) {
    eItem it = c2->item;
    if(it == itOrbRecall) saveRecall(c2);
    if(it == itOrbFire) playSound(c2, "fire");
    else if(it == itOrbFire) playSound(c2, "fire");
    else if(it == itOrbWinter) playSound(c2, "pickup-winter");
    else if(it == itOrbSpeed) playSound(c2, "pickup-speed");
    else if(it == itRevolver) playSound(c2, "pickup-key");
    else playSound(c2, "pickup-orb");
    if(!items[it]) items[it]++;
    items[it] += orbcharges(it);
    }
  else if(c2->item == itOrbLife) {
    playSound(c2, "pickup-orb"); // TODO summon
    placeGolem(cwt.at, c2, moGolem);
    }
  else if(c2->item == itOrbFriend) {
    playSound(c2, "pickup-orb"); // TODO summon
    placeGolem(cwt.at, c2, moTameBomberbird);
    }
#if CAP_TOUR
  else if(tour::on && (c2->item == itOrbSafety || c2->item == itOrbRecall)) {
    addMessage(XLAT("This Orb is not compatible with the Tutorial."));
    return true;
    }
#endif
  else if(c2->item == itOrbSafety) {
    playSound(c2, "pickup-orb"); // TODO safety
    items[c2->item] = 7;
    if(shmup::on)
      shmup::safety = true;
    else 
      activateSafety(c2->land);
    return true;
    }
  else if(c2->item == itBabyTortoise) {
    using namespace tortoise;
    int bnew = babymap[c2];
    babymap.erase(c2);
    int bold = seekbits;
    seekbits = bnew;
    last = seekbits;
    if(seek()) {
      cell *c = passable(cwt.at, NULL, 0) ? cwt.at : c2;
      c->item = itBabyTortoise;
      if(c == c2) dopickup = false;
      babymap[c] = bold;
      }
    else items[itBabyTortoise]++;
    }
  else if(c2->item == itOrbYendor && peace::on) {
    if(!items[itDodeca]) {
      addMessage(XLAT("Collect as many Dodecahedra as you can, then return here!"));
      }
    else {
      addMessage(XLAT("Your score: %1", its(items[itDodeca])));
      peace::simon::restore();
      }
    dopickup = false;
    }
  else if(c2->item == itOrbYendor && yendor::state(c2) != yendor::ysUnlocked) {
    dopickup = false;
    }
  else if(c2->item == itOrbYendor) 
    yendor::collected(c2);    
  else if(c2->item == itHolyGrail) {
    playSound(c2, "tada");
    int v = newRoundTableRadius() + 12;
    items[itOrbTeleport] += v;
    items[itOrbSpeed] += v;
    items[itHolyGrail]++;
    addMessage(XLAT("Congratulations! You have found the Holy Grail!"));
    if(!eubinary) c2->master->alt->emeraldval |= GRAIL_FOUND;
    achievement_collection(c2->item, pg, gold());
    }
  else if(c2->item == itKey) {
    playSound(c2, "pickup-key");
    for(int i=0; i<isize(yendor::yi); i++) if(yendor::yi[i].path[YDIST-1] == c2)
      yendor::yi[i].found = true;
    items[itKey]++;
    }
  else if(!telekinesis && cantGetGrimoire(c2)) {
    // telekinesis checks the condition earlier
    dopickup = false;
    }
  else if(c2->item == itCompass) {
    dopickup = false;
    }
  else if(c2->item == itBuggy || c2->item == itBuggy2) {
    items[itOrbSafety] += 7;
    if(shmup::on)
      shmup::safety = true;
    else {
      buggyGeneration = false;
      activateSafety(laCrossroads);
      }
    return true;
    }
  else if(c2->item == itTreat) {
    playSound(c2, "pickup-scroll");
    halloween::getTreat(c2);
    }   
  else {
    bool lhu = hellUnlocked();
    if(c2->item == itBarrow) 
      for(int i=0; i<c2->landparam; i++) gainItem(c2->item);
    else if(c2->item) gainItem(c2->item);
    int g2 = gold();
    
    if(c2->item) {
      char ch = iinf[c2->item].glyph;
      if(ch == '*') playSound(c2, "pickup-gem");
      else if(ch == '$' || ch == 'x') playSound(c2, "pickup-gold");
      else if(ch == '%' || ch == ';') playSound(c2, "pickup-potion");
      else playSound(c2, "pickup-scroll");
      }

    if(items[itFireShard] && items[itAirShard] && items[itWaterShard] && items[itEarthShard]) {
      items[itFireShard]--;
      items[itAirShard]--;
      items[itWaterShard]--;
      items[itEarthShard]--;
      gainItem(itElemental);
      gainItem(itElemental);
      gainItem(itElemental);
      gainItem(itElemental);
      addMessage(XLAT("You construct some Elemental Gems!", c2->item) + itemcounter(items[itElemental]));
      }          

    if(c2->item == itBounty) 
      items[itRevolver] = 6;
        
    if(c2->item == itHyperstone && items[itHyperstone] == 10)
      achievement_victory(true);

    if(chaosmode && gold() >= 300 && !chaosAchieved) {
      achievement_gain("CHAOS", rg::chaos);
      chaosAchieved = true;
      }

#if ISMOBILE==1
    if(pg < lastsafety + R30*3/2 && g2 >= lastsafety + R30*3/2)
      addMessage(XLAT("The Orb of Safety from the Land of Eternal Motion might save you."));
#endif
    
#define IF(x) if(pg < (x) && g2 >= x && !peace::on)

    IF(R60/4) 
      addMessage(XLAT("Collect treasure to access more different lands..."));
    IF(R30)
      addMessage(XLAT("You feel that you have enough treasure to access new lands!"));
    IF(R30*3/2)
      addMessage(XLAT("Collect more treasures, there are still more lands waiting..."));
    IF(R60)
      addMessage(XLAT("You feel that the stars are right, and you can access R'Lyeh!"));
    IF(R30*5/2)
      addMessage(XLAT("Kill monsters and collect treasures, and you may get access to Hell..."));
    IF(R10 * 9) 
      addMessage(XLAT("To access Hell, collect %1 treasures each of 9 kinds...", its(R10)));
    if(hellUnlocked() && !lhu) {
      addMessage(XLAT("Abandon all hope, the gates of Hell are opened!"));
      addMessage(XLAT("And the Orbs of Yendor await!"));
      }
    }
  
  if(dopickup && c2->item) {
#ifdef HASLINEVIEW
    // temporary variable to avoid the "cannot bind bitfield" problem in C++11
    eItem dummy = c2->item;
    conformal::findhistory.emplace_back(c2, dummy);
#endif
    c2->item = itNone;
    }
//    if(c2->land == laHive)
//      c2->heat = 1;

  int numOrb = 0;
  for(int i=0; i<ittypes; i++)
    if(itemclass(eItem(i)) == IC_ORB && items[i])
      numOrb++;
  if(numOrb) achievement_count("ORB", numOrb, 0);

  if(princess::reviveAt && gold(NO_LOVE) >= princess::reviveAt && !inv::on) {
    princess::reviveAt = 0,
    items[itSavedPrincess] = 1;
    addMessage("You have enough treasure now to revive the Princess!");
    }

  return false;
  }

void dropGreenStone(cell *c) {
  if(items[itGreenStone] && !passable(c, NULL, P_MONSTER)) {
    // NOTE: PL/CZ translations assume that itGreenStone is dropped to avoid extra forms!
    addMessage(XLAT("Cannot drop %the1 here!", itGreenStone));
    return;
    }
  if(items[itGreenStone] && c->item == itNone) {
    items[itGreenStone]--;
    if(false) {
      c->item = itNone;
      spill(c, eWall(c->wall ^ waFloorA ^ waFloorB), 3);
      addMessage(XLAT("The slime reacts with %the1!", itGreenStone));
      }
    else {
      c->item = itGreenStone;
      addMessage(XLAT("You drop %the1.", itGreenStone));
      if(isHaunted(cwt.at->land)) survivalist = false;
      }
    }
  else {
    if(items[itGreenStone] && c->item == itGreenStone)
      addMessage(XLAT("You juggle the Dead Orbs."));
    else if(items[itGreenStone] && c->item)
      addMessage(XLAT("You give %the1 a grim look.", c->item));
    else if(items[itGreenStone]) {
      addMessage(XLAT("Cannot drop %the1 here!", itGreenStone));
      return;
      }
    else if(gold() >= 300)
      addMessage(XLAT("You feel great, like a true treasure hunter."));
    else if(gold() >= 200)
      addMessage(XLAT("Your eyes shine like gems."));
    else if(gold() >= 100)
      addMessage(XLAT("Your eyes shine as you glance at your precious treasures."));
    else if(gold() >= 50)
      addMessage(XLAT("You glance at your great treasures."));
    else if(gold() >= 10)
      addMessage(XLAT("You glance at your precious treasures."));
    else if(gold() > 0)
      addMessage(XLAT("You glance at your precious treasure."));
    else
      addMessage(XLAT("Your inventory is empty."));
    }
  }

void roundTableMessage(cell *c2) {
  if(!euclid && !cwt.at->master->alt) return;
  if(!euclid && !c2->master->alt) return;
  int dd = celldistAltRelative(c2) - celldistAltRelative(cwt.at);

  bool tooeasy = (roundTableRadius(c2) < newRoundTableRadius());
            
  if(dd>0) {
    if(grailWasFound(cwt.at)) {
      addMessage(XLAT("The Knights congratulate you on your success!"));
      knighted = roundTableRadius(cwt.at);
      }
    else if(!tooeasy)
      addMessage(XLAT("The Knights laugh at your failure!"));
    }
  else {
    if(grailWasFound(cwt.at))
      addMessage(XLAT("The Knights stare at you!"));
    else if(tooeasy) {
      if(!tactic::on)
        addMessage(XLAT("Come on, this is too easy... find a bigger castle!"));
      }
    else
      addMessage(XLAT("The Knights wish you luck!"));
    }
  }

bool in_full_game() {
  if(chaosmode) return true;
  if(geometry == gEuclid && isCrossroads(specialland)) return true;
  if(weirdhyperbolic && specialland == laCrossroads4) return true;
  if(geometry == gCrystal && isCrossroads(specialland)) return true;
  if(geometry == gNormal && !NONSTDVAR) return true;
  return false;
  }

void knightFlavorMessage(cell *c2) {

  if(!eubinary && !c2->master->alt) {
    addMessage(XLAT("\"I am lost...\""));
    return;
    }

  if(tactic::on) {
    addMessage(XLAT("\"The Knights of the Horocyclic Table salute you!\""));
    return;
    }

  bool grailfound = grailWasFound(c2);
  int rad = roundTableRadius(c2);
  bool tooeasy = (rad < newRoundTableRadius());

  static int msgid = 0;

  retry:
  if(msgid >= 32) msgid = 0;  
  
  if(msgid == 0 && grailfound) {
    addMessage(XLAT("\"I would like to congratulate you again!\""));
    }
  else if(msgid == 1 && !tooeasy) {
    addMessage(XLAT("\"Find the Holy Grail to become one of us!\""));
    }
  else if(msgid  == 2 && !tooeasy) {
    addMessage(XLAT("\"The Holy Grail is in the center of the Round Table.\""));
    }
  else if(msgid == 3 && geometry == gCrystal) {
    if(crystal::pure())
      addMessage(XLAT("\"Each piece of the Round Table is exactly %1 steps away from the Holy Grail.\"", its(roundTableRadius(c2))));
    else
      addMessage(XLAT("\"According to Merlin, the Round Table is a perfect Euclidean sphere in %1 dimensions.\"", its(ginf[gCrystal].sides/2)));
    }
  else if(msgid == 3 && !peace::on && in_full_game()) {
    addMessage(XLAT("\"I enjoy watching the hyperbug battles.\""));
    }
  else if(msgid == 4 && in_full_game()) {
    addMessage(XLAT("\"Have you visited a temple in R'Lyeh?\""));
    }
  else if(msgid == 5 && in_full_game()) {
    addMessage(XLAT("\"Nice castle, eh?\""));
    }
  else if(msgid == 6 && items[itSpice] < 10 && !peace::on && in_full_game()) {
    addMessage(XLAT("\"The Red Rock Valley is dangerous, but beautiful.\""));
    }
  else if(msgid == 7 && items[itSpice] < 10 && !peace::on && in_full_game()) {
    addMessage(XLAT("\"Train in the Desert first!\""));
    }
  else if(msgid == 8 && sizes_known() && !tactic::on) {
    string s = "";
    if(geometry == gCrystal)
      s = crystal::get_table_boundary();
    else if(!quotient)
      s = expansion.get_descendants(rad).get_str(100);
    if(s == "") { msgid++; goto retry; }
    addMessage(XLAT("\"Our Table seats %1 Knights!\"", s));
    }
  else if(msgid == 9 && sizes_known() && !tactic::on) {
    string s = "";
    if(geometry == gCrystal)
      s = crystal::get_table_volume();
    else if(!quotient)
      s = expansion.get_descendants(rad-1, expansion.diskid).get_str(100);
    if(s == "") { msgid++; goto retry; }
    addMessage(XLAT("\"There are %1 floor tiles inside our Table!\"", s));
    }
  else if(msgid == 10 && !items[itPirate] && !items[itWhirlpool] && !peace::on && in_full_game()) {
    addMessage(XLAT("\"Have you tried to take a boat and go into the Ocean? Try it!\""));
    }
  else if(msgid == 11 && !princess::saved && in_full_game()) {
    addMessage(XLAT("\"When I visited the Palace, a mouse wanted me to go somewhere.\""));
    }
  else if(msgid == 12 && !princess::saved && in_full_game()) {
    addMessage(XLAT("\"I wonder what was there...\""));
    }
  else if(msgid == 13 && !peace::on && in_full_game()) {
    addMessage(XLAT("\"Be careful in the Rose Garden! It is beautiful, but very dangerous!\""));
    }
  else if(msgid == 14) {
    addMessage(XLAT("\"There is no royal road to geometry.\""));
    }
  else if(msgid == 15) {
    addMessage(XLAT("\"There is no branch of mathematics, however abstract, "));
    addMessage(XLAT("which may not some day be applied to phenomena of the real world.\""));
    }
  else if(msgid == 16) {
    addMessage(XLAT("\"It is not possession but the act of getting there, "));
    addMessage(XLAT("which grants the greatest enjoyment.\""));
    }
  else if(msgid == 17) {
    addMessage(XLAT("\"We live in a beautiful and orderly world, "));
    addMessage(XLAT("and not in a chaos without norms.\""));
    }                                                          
  else if(msgid == 25) {
    addMessage(XLAT("\"Thank you very much for talking, and have a great rest of your day!\""));
    }
  else {
    msgid++; goto retry;
    }

  msgid++;
  }

void uncoverMines(cell *c, int lev, int dist) {
  if(c->wall == waMineUnknown)
    c->wall = waMineOpen;
  
  bool minesNearby = false;
  bool nominesNearby = false;
  bool mineopens = false;

  forCellEx(c2, c) {
    if(c2->wall == waMineMine) minesNearby = true;
    if(c2->wall == waMineOpen) mineopens = true;
    if(c2->wall == waMineUnknown && !c2->item) nominesNearby = true;
    }

  if(lev && (nominesNearby || mineopens) && !minesNearby) for(int i=0; i<c->type; i++)
    if(c->move(i) && (c->move(i)->wall == waMineUnknown || c->move(i)->wall == waMineOpen))
      uncoverMines(c->move(i), lev-1, dist+1);

  if(minesNearby && !nominesNearby && dist == 0) {
    forCellEx(c2, c) 
      if(c2->wall == waMineMine && c2->land == laMinefield)
        c2->landparam |= 1;
    }
  }

namespace orbbull {
  cell *prev[MAXPLAYER];
  eLastmovetype prevtype[MAXPLAYER];
  int count;

  bool is(cell *c1, cell *c2, cell *c3) {
    int lp = neighborId(c2, c1);
    int ln = neighborId(c2, c3);
    return lp >= 0 && ln >= 0 && anglestraight(c2, lp, ln);
    }
  
  void gainBullPowers() {
    items[itOrbShield]++; orbused[itOrbShield] = true;
    items[itOrbThorns]++; orbused[itOrbThorns] = true;
    items[itOrbHorns]++; orbused[itOrbHorns] = true;
    }

  void check() {
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
  }

// predictable or not
static constexpr bool randterra = false;

void terracotta(cell *c) {
  if(c->wall == waTerraWarrior && !c->monst) {
    bool live = false;
    if(randterra) {
      c->landparam++;
      if((c->landparam == 3 && hrand(3) == 0) ||
        (c->landparam == 4 && hrand(2) == 0) || 
        c->landparam == 5)
          live = true;
      }
    else {
      c->landparam--;
      live = !c->landparam;
      }
    if(live)
      c->monst = moTerraWarrior,
      c->hitpoints = 7,
      c->wall = waNone;
    }
  }

void terracottaAround(cell *c) {
  forCellEx(c2, c)
    terracotta(c2);
  }

void terracotta() {
  for(int i=0; i<numplayers(); i++)
    forCellEx(c, playerpos(i)) {
      if(shmup::on) {
        forCellEx(c2, c)
          terracotta(c2);
        }
      else
        terracotta(c);
      }
  }

eMonster passive_switch = moSwitch2;

void checkSwitch() {
  passive_switch = (gold() & 1) ? moSwitch1 : moSwitch2;
  }

void monstersTurn() {
  checkSwitch();
  mirror::breakAll();
  DEBT("bfs");
  bfs();
  DEBT("charge");
  if(elec::havecharge) elec::act();
  DEBT("mmo");
  int phase2 = (1 & items[itOrbSpeed]);
  if(!phase2) movemonsters();

  if(playerInPower() && (phase2 || !items[itOrbSpeed]) && (havewhat & HF_FAST)) 
    moveNormals(moWitchSpeed);

  if(phase2 && markOrb(itOrbEmpathy)) {
    bfs();
    movegolems(AF_FAST);
    for(int i=0; i<isize(dcal); i++) {
      if(dcal[i]->monst == moFriendlyGhost && dcal[i]->stuntime)
        dcal[i]->stuntime--;
      refreshFriend(dcal[i]);
      }
    }
  DEBT("rop");
  reduceOrbPowers();
  int phase1 = (1 & items[itOrbSpeed]);
  DEBT("lc");
  if(!phase1) livecaves();
  if(!phase1) ca::simulate();
  if(!phase1) heat::processfires();
  
  for(cell *c: crush_now) {
    playSound(NULL, "closegate");
    if(canAttack(c, moCrusher, c, c->monst, AF_GETPLAYER | AF_CRUSH)) {
      attackMonster(c, AF_MSG | AF_GETPLAYER | AF_CRUSH, moCrusher);
      }
    moveEffect(c, c, moDeadBird, -1);
    destroyBoats(c, NULL, true);
    }
  
  crush_now = move(crush_next);
  crush_next.clear();
  
  DEBT("heat");
  heat::processheat();
  // if(elec::havecharge) elec::drawcharges();

  orbbull::check();

  if(!phase1) terracotta();
  
  if(items[itOrbFreedom])
    for(int i=0; i<numplayers(); i++)
      if(multi::playerActive(i))
        checkFreedom(playerpos(i));

  DEBT("check");
  checkmove();
  if(canmove) elec::checklightningfast();


#ifdef HASLINEVIEW
  for(int i=0; i<numplayers(); i++)
    if(multi::playerActive(i))
      conformal::movehistory.push_back(playerpos(i));
#endif
  }

void pushThumper(cell *th, cell *cto) {
  eWall w = th->wall;
  cto->wparam = th->wparam;
  if(th->land == laAlchemist)
    th->wall = isAlch(cwt.at) ? cwt.at->wall : cto->wall;
  else th->wall = waNone;
  if(cto->wall == waOpenPlate || cto->wall == waClosePlate) {
    toggleGates(cto, cto->wall);
    addMessage(XLAT("%The1 destroys %the2!", waThumperOn, cto->wall));
    }
  if(cellUnstable(cto) && cto->land == laMotion) {
    addMessage(XLAT("%The1 falls!", waThumperOn));
    doesFallSound(cto);
    }
  else if(cellUnstableOrChasm(cto)) {
    addMessage(XLAT("%The1 fills the hole!", waThumperOn));
    cto->wall = waTempFloor;
    }
  else if(isWatery(cto)) {
    addMessage(XLAT("%The1 fills the hole!", waThumperOn));
    cto->wall = waTempBridge;
    }
  else 
    cto->wall = w;
  }

bool canPushThumperOn(cell *tgt, cell *thumper, cell *player) {
  if(tgt->wall == waBoat || tgt->wall == waStrandedBoat) return false;
  if(isReptile(tgt->wall)) return false;
  if(isWatery(tgt) && !tgt->monst)
    return true;
  if(tgt->wall == waChasm && !tgt->monst)
    return true;
  return
    passable(tgt, thumper, P_MIRROR) &&
    passable(tgt, player, P_MIRROR) &&
    !tgt->item;
  }

void activateActiv(cell *c, bool msg) {
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

bool scentResistant() {
  return markOrb(itOrbBeauty) || markOrb(itOrbAether) || markOrb(itOrbShield);
  }

void wouldkill(const char *msg) {
  if(who_kills_me == moWarning)
    addMessage(XLAT("This move appears dangerous -- are you sure?"));
  else if(who_kills_me == moFireball) 
    addMessage(XLAT("Cannot move into the current location of another player!"));
  else if(who_kills_me == moAirball) 
    addMessage(XLAT("Players cannot get that far away!"));
  else
    addMessage(XLAT(msg, who_kills_me));
  }

bool havePushConflict(cell *pushto, bool checkonly) {
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

cell *global_pushto;

void killFriendlyIvy() {
  forCellEx(c2, cwt.at) if(c2->monst == moFriendlyIvy) 
    killMonster(c2, moPlayer, 0);
  }

bool monsterPushable(cell *c2) {
  return (c2->monst != moFatGuard && !(isMetalBeast(c2->monst) && c2->stuntime < 2) && c2->monst != moTortoise && c2->monst != moTerraWarrior && c2->monst != moVizier);
  }  

bool got_survivalist;

bool should_switchplace(cell *c1, cell *c2) {
  if(isPrincess(c2->monst) || among(c2->monst, moGolem, moIllusion, moMouse, moFriendlyGhost))
    return true;
  
  if(peace::on) return c2->monst && !isMultitile(c2->monst);
  return false;
  }

bool warningprotection_hit(eMonster m) {
  if(m && warningprotection(XLAT("Are you sure you want to hit %the1?", m)))
    return true;
  return false;
  }

bool switchplace_prevent(cell *c1, cell *c2, bool checkonly) {
  if(!should_switchplace(c1, c2)) return false;
  if(c1->monst && c1->monst != moFriendlyIvy) {
    if(!checkonly) addMessage(XLAT("There is no room for %the1!", c2->monst));
    return true;
    }
  if(passable(c1, c2, P_ISFRIEND | (c2->monst == moTameBomberbird ? P_FLYING : 0))) return false;
  if(warningprotection_hit(c2->monst)) return true;
  return false;
  }

void handle_switchplaces(cell *c1, cell *c2, bool& switchplaces) {
  if(should_switchplace(c1, c2)) {
    bool pswitch = false;
    if(c2->monst == moMouse)
      princess::mouseSqueak(c2);
    else if(isPrincess(c2->monst)) {
      princess::line(c2);
      princess::move(c1, c2);
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

bool movepcto(int d, int subdir, bool checkonly) {
  if(d >= 0 && !checkonly && subdir != 1 && subdir != -1) printf("subdir = %d\n", subdir);
  global_pushto = NULL;
  bool switchplaces = false;

  if(d == MD_USE_ORB) 
    return targetRangedOrb(multi::whereto[multi::cpid].tgt, roMultiGo);

  bool errormsgs = multi::players == 1 || multi::cpid == multi::players-1;
  if(hardcore && !canmove) return false;
  if(hardcore && checkonly) { return false; }
  if(checkonly && haveRangedTarget()) return true;
  if(!checkonly && d >= 0) {
    flipplayer = false;
    if(multi::players > 1) multi::flipped[multi::cpid] = false;
    }
  if(!checkonly) { DEBB(DF_TURN, (debugfile,"movepc\n")); }
  int origd = d;
  if(d >= 0) {
    cwt += d;
    mirror::act(d, mirror::SPINSINGLE);
    d = cwt.spin;
    }
  if(d != -1 && !checkonly) playermoved = true;
  if(!checkonly) invismove = false;  
  bool boatmove = false;
  
  if(multi::players > 1)
    lastmountpos[multi::cpid] = cwt.at;
  else
    lastmountpos[0] = cwt.at;
  
  if(againstRose(cwt.at, NULL) && d<0 && !scentResistant()) {
    if(checkonly) return false;
    addMessage("You just cannot stand in place, those roses smell too nicely.");
    return false;
    }

  if(d >= 0) {
    cell *c2 = cwt.at->move(d);
    bool goodTortoise = c2->monst == moTortoise && tortoise::seek() && !tortoise::diff(tortoise::getb(c2)) && !c2->item;

    if(againstRose(cwt.at, c2) && !scentResistant()) {
      if(checkonly) return false;
      addMessage("Those roses smell too nicely. You have to come towards them.");
      return false;
      }
    
    if(items[itOrbDomination] > ORBBASE && isMountable(c2->monst) && !monstersnear2()) {
      if(checkonly) return true;
      if(!isMountable(cwt.at->monst)) dragon::target = NULL;
      movecost(cwt.at, c2, 3);
      
      flipplayer = true; if(multi::players > 1) multi::flipped[multi::cpid] = true;
      invismove = (turncount >= noiseuntil) && items[itOrbInvis] > 0;
      killFriendlyIvy();
      goto mountjump;
      }
    
    if(!passable(c2, cwt.at, P_ISPLAYER | P_MIRROR | P_USEBOAT | P_FRIENDSWAP) && items[itOrbFlash]) {
      if(checkonly) return true;
      if(orbProtection(itOrbFlash)) return true;
      activateFlash();
      bfs();
      if(multi::players > 1) { multi::whereto[multi::cpid].d = MD_UNDECIDED; return false; }
      checkmove();
      return true;
      }

    if(!passable(c2, cwt.at, P_ISPLAYER | P_MIRROR | P_USEBOAT | P_FRIENDSWAP) && items[itOrbLightning]) {
      if(checkonly) return true;
      if(orbProtection(itOrbLightning)) return true;
      activateLightning();
      keepLightning = true;
      bfs();
      keepLightning = false;
      if(multi::players > 1) { multi::whereto[multi::cpid].d = MD_UNDECIDED; return false; }
      checkmove();
      return true;
      }

    if(isActivable(c2)) {
      if(checkonly) return true;
      activateActiv(c2, true);
      bfs();
      if(multi::players > 1) { multi::whereto[multi::cpid].d = MD_UNDECIDED; return false; }
      checkmove();
      return true;
      }

    if(c2->wall == waThumperOn && !c2->monst && !nonAdjacentPlayer(c2, cwt.at)) {
      int pushdir;
      cell *c3 = determinePush(cwt, c2, subdir, [c2] (cell *c) { return canPushThumperOn(c, c2, cwt.at); }, pushdir);
      if(c3 == c2) {
        if(checkonly) return false;
        addMessage(XLAT("No room to push %the1.", c2->wall));
        return false;
        }
      if(monstersnear(c2, NULL, moPlayer, NULL, cwt.at)) {
        if(!checkonly && errormsgs) wouldkill("%The1 would kill you there!");
        return false;
        }
      global_pushto = c3;
      if(checkonly) return true;
      addMessage(XLAT("You push %the1.", c2->wall));
      lastmovetype = lmPush; lastmove = cwt.at;
      pushThumper(c2, c3);
      }

    if(c2->item == itHolyGrail && roundTableRadius(c2) < newRoundTableRadius()) {
      if(checkonly) return false;
      addMessage(XLAT("That was not a challenge. Find a larger castle!"));
      return false;
      }

    if(c2->item == itOrbYendor && !checkonly && !peace::on && !itemHiddenFromSight(c2) && yendor::check(c2)) {
      return false;
      }

    if(isWatery(c2) && !nonAdjacentPlayer(cwt.at,c2) && !c2->monst && cwt.at->wall == waBoat) {

      if(havePushConflict(cwt.at, checkonly)) return false;

      if(againstWind(c2, cwt.at)) {
        if(!checkonly) 
          addMessage(XLAT(airdist(c2) < 3 ? "The Air Elemental blows you away!" : "You cannot go against the wind!"));
        return false;
        }

      if(againstCurrent(c2, cwt.at) && !markOrb(itOrbWater)) {
        if(markOrb(itOrbFish) || markOrb(itOrbAether)) goto escape;
        if(!checkonly)
          addMessage(XLAT("You cannot go against the current!"));
        return false;
        }

      if(monstersnear(c2, NULL, moPlayer, NULL, cwt.at)) {
        if(!checkonly && errormsgs) wouldkill("%The1 would kill you there!");
        return false;
        }

      if(checkonly) return true;
      moveBoat(c2, cwt.at, d);
      boatmove = true;
      goto boatjump;
      }
    
    if(!c2->monst && cwt.at->wall == waBoat && boatGoesThrough(c2) && markOrb(itOrbWater) && !nonAdjacentPlayer(c2, cwt.at)) {

      if(havePushConflict(cwt.at, checkonly)) return false;
      if(monstersnear(c2,NULL,moPlayer,NULL,cwt.at)) {
        if(!checkonly && errormsgs) wouldkill("%The1 would kill you there!");
        return false;
        }
      
      if(checkonly) return true;
      if(c2->item && !cwt.at->item) moveItem(c2, cwt.at, false), boatmove = true;
      placeWater(c2, cwt.at);
      moveBoat(c2, cwt.at, d);
      c2->mondir = revhint(cwt.at, d);
      if(c2->item) boatmove = !boatmove;
      goto boatjump;
      }

    escape:
    if(c2->wall == waBigStatue && !c2->monst && !nonAdjacentPlayer(c2, cwt.at)) {
      if(!canPushStatueOn(cwt.at)) {
        if(checkonly) return false;
        if(isFire(cwt.at))
          addMessage(XLAT("You have to escape first!"));
        else
          addMessage(XLAT("There is not enough space!"));
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
          
      if(checkonly) { c2->wall = save_c2; cwt.at->wall = save_cw; return true; }
      addMessage(XLAT("You push %the1 behind you!", waBigStatue));
      animateMovement(c2, cwt.at, LAYER_BOAT, cwt.at->c.spin(d));
      goto statuejump;
      }

    bool attackable;
    attackable = 
      c2->wall == waBigTree ||
      c2->wall == waSmallTree ||
      c2->wall == waMirrorWall;
    attackable = attackable && (!c2->monst || isFriendly(c2));
    if(attackable && markOrb(itOrbAether) && c2->wall != waMirrorWall)
      attackable = false;
    attackable = attackable && !nonAdjacentPlayer(cwt.at,c2);
      
    if(attackable) {
      if(checkNeedMove(checkonly, true)) return false;
      if(monstersnear(cwt.at,c2,moPlayer,NULL,cwt.at)) {
        if(!checkonly && errormsgs) wouldkill("%The1 would get you!");
        return false;
        }
      if(checkonly) return true;
      if(c2->wall == waSmallTree) {
        drawParticles(c2, winf[c2->wall].color, 4);
        addMessage(XLAT("You chop down the tree."));
        playSound(c2, "hit-axe" + pick123());
        c2->wall = waNone;
        sideAttack(cwt.at, d, moPlayer, 0);
        animateAttack(cwt.at, c2, LAYER_SMALL, d);
        }
      else if(c2->wall == waBigTree) {
        drawParticles(c2, winf[c2->wall].color, 8);
        addMessage(XLAT("You start chopping down the tree."));
        playSound(c2, "hit-axe" + pick123());
        c2->wall = waSmallTree;
        sideAttack(cwt.at, d, moPlayer, 0);
        animateAttack(cwt.at, c2, LAYER_SMALL, d);
        }
      else {
        if(!peace::on) {
          addMessage(XLAT("You swing your sword at the mirror."));
          sideAttack(cwt.at, d, moPlayer, 0);
          animateAttack(cwt.at, c2, LAYER_SMALL, d);
          }
        }
      if(survivalist && isHaunted(c2->land))
        survivalist = false;
      mirror::act(origd, mirror::SPINMULTI | mirror::ATTACK);
      lastmovetype = lmTree; lastmove = c2;
      swordAttackStatic();
      }
    else if(c2->monst == moKnight) {
      if(checkonly) return false;
      knightFlavorMessage(c2);
      return false;
      }
    else if(c2->monst && (!isFriendly(c2) || c2->monst == moTameBomberbird || isMountable(c2->monst))
      && !(peace::on && !isMultitile(c2->monst) && !goodTortoise)) {
    
      flagtype attackflags = AF_NORMAL;
      if(items[itOrbSpeed]&1) attackflags |= AF_FAST;
      if(items[itOrbSlaying]) attackflags |= AF_CRUSH;
      
      if(!canAttack(cwt.at, moPlayer, c2, c2->monst, attackflags)) {
        if(checkonly) return false;
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
        return false;
        }
      
      // pushto=c2 means that the monster is not killed and thus
      // still counts for lightning in monstersnear
      cell *pushto = NULL;
      int pushdir = 0;
      if(isStunnable(c2->monst) && c2->hitpoints > 1) {
        if(monsterPushable(c2))
          pushto = determinePush(cwt, c2, subdir, [c2] (cell *c) { return passable(c, c2, P_BLOW); }, pushdir);
        else          
          pushto = c2;
        }
      if(c2->monst == moTroll || c2->monst == moFjordTroll || 
         c2->monst == moForestTroll || c2->monst == moStormTroll || c2->monst == moVineSpirit)
        pushto = c2;
      
      global_pushto = pushto;
      
      if(havePushConflict(pushto, checkonly)) return false;
      
      if(!(isWatery(cwt.at) && c2->monst == moWaterElemental) && checkNeedMove(checkonly, true))
        return false;
      
      if(monstersnear(cwt.at, c2, moPlayer, pushto, cwt.at)) {
        if(errormsgs && !checkonly)
          wouldkill("You would be killed by %the1!");          
        return false;
        }

      if(c2->monst == moTameBomberbird && warningprotection_hit(moTameBomberbird)) return false;
      
      if(checkonly) return true;

      /* if(c2->monst == moTortoise) {
        printf("seek=%d get=%d <%x/%x> item=%d\n", 
          tortoise::seeking, 
          !tortoise::diff(tortoise::getb(c2)), 
          tortoise::getb(c2) & tortoise::mask,
          tortoise::seekbits & tortoise::mask,
          !c2->item);
        } */
      
      mirror::act(origd, mirror::SPINMULTI | mirror::ATTACK);
      
      int tk = tkills();

      if(goodTortoise) {
        items[itBabyTortoise] += 4;
        updateHi(itBabyTortoise, items[itBabyTortoise]);
        c2->item = itBabyTortoise;
        tortoise::babymap[c2] = tortoise::seekbits;
        playSound(c2, playergender() ? "heal-princess" : "heal-prince");
        addMessage(XLAT(playergender() == GEN_F ? "You are now a tortoise heroine!" : "You are now a tortoise hero!"));
        c2->stuntime = 2;
        achievement_collection(itBabyTortoise, 0, 0);
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
          if(c2->monst == moSalamander && (pushto == c2 || !pushto)) c2->stuntime = 10;
          if(!c2->monst) produceGhost(c2, m, moPlayer);
          if(pushto && pushto != c2) pushMonster(pushto, c2, pushdir);
          animateAttack(cwt.at, c2, LAYER_SMALL, d);
          }
        }
      
      sideAttack(cwt.at, d, moPlayer, tkills() - tk);
      lastmovetype = lmAttack; lastmove = c2;
      swordAttackStatic();
      }
    else if(!passable(c2, cwt.at, P_USEBOAT | P_ISPLAYER | P_MIRROR | P_MONSTER)) {
      if(checkonly) return false;
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
      else if(cwt.at->wall == waRed3 && snakelevel(c2) == 0)
        addMessage(XLAT("You would get hurt!", c2->wall));
      else if(cwt.at->wall == waTower && snakelevel(c2) == 0)
        addMessage(XLAT("You would get hurt!", c2->wall));
      else if(cellEdgeUnstable(cwt.at) && cellEdgeUnstable(c2))
        addMessage(XLAT("Gravity does not allow this!"));
      else if(c2->wall == waChasm && c2->land == laDual)
        addMessage(XLAT("You cannot move there!"));
      else {
        addMessage(XLAT("You cannot move through %the1!", c2->wall));
        }
      return false;
      }
    else {
      if(mineMarked(c2) && !minesafe() && !checkonly && warningprotection(XLAT("Are you sure you want to step there?")))
        return false;
      if(monstersnear(c2, NULL, moPlayer, NULL, cwt.at)) {
        if(checkonly) return false;

        if(items[itOrbFlash]) {
          if(checkonly) return true;
          if(orbProtection(itOrbFlash)) return true;
          activateFlash();
          checkmove();
          return true;
          }

        if(items[itOrbLightning]) {
          if(checkonly) return true;
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
      
      if(checkonly) return true;
      boatjump:
      statuejump:
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
        roundTableMessage(c2);
      
      invismove = (turncount >= noiseuntil) && items[itOrbInvis] > 0;
      
      if(items[itOrbFire]) {
        invismove = false;
        if(makeflame(cwt.at, 10, false)) markOrb(itOrbFire);
        }

      if(1) {
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

      if(items[itOrbDigging]) {
        invismove = false;
        if(earthMove(cwt.at, d)) markOrb(itOrbDigging);
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
            
      mountjump:
      lastmovetype = lmMove; lastmove = cwt.at;

      stabbingAttack(cwt.at, c2, moPlayer);
      cell *c1 = cwt.at;
      int d = cwt.spin;
      cwt += wstep;
      if(switchplaces)
        animateReplacement(c1, cwt.at, LAYER_SMALL, d, cwt.spin);
      else
        animateMovement(c1, cwt.at, LAYER_SMALL, d);
      
      mirror::act(origd, mirror::SPINMULTI | mirror::ATTACK | mirror::GO);

      playerMoveEffects(c1, c2);

      if(c2->monst == moFriendlyIvy) c2->monst = moNone;

      countLocalTreasure();
      landvisited[cwt.at->land] = true;
      afterplayermoved();
      }
    }
  else {
    lastmovetype = lmSkip; lastmove = NULL;
    if(checkNeedMove(checkonly, false))
      return false;
    if(monstersnear(cwt.at, NULL, moPlayer, NULL, cwt.at)) {
      if(errormsgs && !checkonly) 
        wouldkill("%The1 would get you!");
      return false;
      }
    if(checkonly) return true;
    swordAttackStatic();
    if(d == -2) 
      dropGreenStone(cwt.at);
    if(cellUnstable(cwt.at) && !markOrb(itOrbAether))
      doesFallSound(cwt.at);
    }

  invisfish = false;
  if(items[itOrbFish]) {
     invisfish = true;
     for(int i=0; i<numplayers(); i++) 
       if(multi::playerActive(i) && !isWatery(playerpos(i)))
         invisfish = false;
     if(d < 0) invisfish = false; // no invisibility if staying still
     if(invisfish) invismove = true, markOrb(itOrbFish);
     }
  
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
  
  DEBT("done");
  return true;
  }

/* bool isPsiTarget(cell *dst) {
  return 
    dst->cpdist > 1 &&
    dst->monst && 
    !(isWorm(dst) || dst->monst == moShadow);
  } */                                                      

void moveItem1(cell *from, cell *to, bool activateYendor) {
  if(from->item == itOrbYendor) {
    bool xnew = true;
    for(int i=0; i<isize(yendor::yi); i++) 
      if(yendor::yi[i].path[0] == from) xnew = false;
    if(xnew && activateYendor) yendor::check(from);
    for(int i=0; i<isize(yendor::yi); i++) 
      if(yendor::yi[i].path[0] == from) 
        yendor::yi[i].path[0] = to;
    }

  if(from->item == itKey) {
    for(int i=0; i<isize(yendor::yi); i++) if(yendor::yi[i].path[YDIST-1] == from)
      yendor::yi[i].path[YDIST-1] = to;
    }
  
  if(from->item == itBabyTortoise) {
    tortoise::babymap[to] = tortoise::babymap[from];
    tortoise::babymap.erase(from);
    }  

  eItem i = to->item;
  to->item = from->item;
  from->item = i;
  }

void moveItem (cell *from, cell *to, bool activateYendor) {
  static cell dummy;
  dummy.item = itNone;
  moveItem1(from, &dummy, activateYendor);
  moveItem1(to, from, activateYendor);
  moveItem1(&dummy, to, activateYendor);
  }

void fixWormBug(cell *c) {
  if(conformal::includeHistory) return;
  printf("worm bug!\n");
  if(c->monst == moWormtail) c->monst = moWormwait;
  if(c->monst == moTentacletail || c->monst == moTentacleGhost) c->monst = moTentacle;
  if(c->monst == moHexSnakeTail) c->monst = moHexSnake;
  }
  
cell *wormhead(cell *c) {
  // cell *cor = c;
  findhead:  
  if(c->monst == moTentacle || c->monst == moWorm || c->monst == moHexSnake ||
    c->monst == moWormwait || c->monst == moTentacleEscaping || c->monst == moTentaclewait ||
    c->monst == moDragonHead) return c;
  for(int i=0; i<c->type; i++)
    if(c->move(i) && isWorm(c->move(i)->monst) && c->move(i)->mondir == c->c.spin(i)) {
      c = c->move(i); goto findhead;
      }
  fixWormBug(c);
  return c;
  }
  
int wormpos(cell *c) {
  // cell *cor = c;
  int cnt = 0;
  findhead:  
  if(c->monst == moTentacle || c->monst == moWorm || c->monst == moHexSnake ||
    c->monst == moWormwait || c->monst == moTentacleEscaping || c->monst == moTentaclewait ||
    c->monst == moDragonHead) return cnt;
  for(int i=0; i<c->type; i++)
    if(c->move(i) && isWorm(c->move(i)->monst) && c->move(i)->mondir == c->c.spin(i)) {
      c = c->move(i); cnt++; goto findhead;
      }
  fixWormBug(c);
  return cnt;
  }
  
// currently works for worms only
bool sameMonster(cell *c1, cell *c2) {
  if(!c1 || !c2) return false;
  if(c1 == c2) return true;
  if(isWorm(c1->monst) && isWorm(c2->monst))
    return wormhead(c1) == wormhead(c2);
  if(isKraken(c1->monst) && isKraken(c2->monst))
    return kraken::head(c1) == kraken::head(c2);
  return false;
  }

eMonster haveMount() {
  for(int i=0; i<numplayers(); i++) if(multi::playerActive(i)) {
    eMonster m = playerpos(i)->monst;
    if(m) return m;
    }
  return moNone;
  }

bool mightBeMine(cell *c) {
  return c->wall == waMineUnknown || c->wall == waMineMine;
  }

hookset<bool(cell*)> *hooks_mark;

void performMarkCommand(cell *c) {
  if(!c) return;
  if(callhandlers(false, hooks_mark, c)) return;
  if(c->land == laCA && c->wall == waNone) 
    c->wall = waFloorA;
  else if(c->land == laCA && c->wall == waFloorA)
    c->wall = waNone; 
  if(c->land != laMinefield) return;
  if(c->item) return;
  if(!mightBeMine(c)) return;
  bool adj = false;
  forCellEx(c2, c) if(c2->wall == waMineOpen) adj = true;
  if(adj) c->landparam ^= 1;
  }

bool mineMarked(cell *c) {
  if(!mightBeMine(c)) return false;
  if(c->item) return false;
  if(c->land != laMinefield) return true;
  return c->landparam & 1;
  }

bool mineMarkedSafe(cell *c) {
  if(!mightBeMine(c)) return false;
  if(c->item) return true;
  if(c->land != laMinefield) return false;
  return c->landparam & 2;
  }

bool minesafe() {
  return items[itOrbAether];
  }

bool warningprotection(const string& s) {
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

}
