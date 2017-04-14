// Hyperbolic Rogue
// main game routines: movement etc.

// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

int lastsafety;
int mutantphase;
int turncount;
int rosewave, rosephase;
int avengers;

cell *lastmove;
enum eLastmovetype {lmSkip, lmMove, lmAttack, lmSpecial, lmPush, lmTree};
eLastmovetype lastmovetype;

int sval = 100000000;

bool hauntedWarning;
bool survivalist;

bool hardcore = false;
int hardcoreAt;

flagtype havewhat, hadwhat;

#define HF_BUG        (1<<0)
#define HF_EARTH      (1<<1)
#define HF_BIRD       (1<<2)
#define HF_LEADER     (1<<3)
#define HF_HEX        (1<<4)
#define HF_WHIRLPOOL  (1<<5)
#define HF_WATER      (1<<6)
#define HF_AIR        (1<<7)
#define HF_MUTANT     (1<<8)
#define HF_OUTLAW     (1<<9)
#define HF_WHIRLWIND  (1<<10)
#define HF_ROSE       (1<<11)
#define HF_DRAGON     (1<<12)
#define HF_KRAKEN     (1<<13)
#define HF_SHARK      (1<<14)
#define HF_BATS       (1<<15)
#define HF_REPTILE    (1<<16)
#define HF_EAGLES     (1<<17)
#define HF_SLOW       (1<<18)
#define HF_FAST       (1<<19)
#define HF_WARP       (1<<20)
#define HF_MOUSE      (1<<21)
#define HF_RIVER      (1<<22)

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
int items[ittypes], hiitems[MODECODES][ittypes], kills[motypes], explore[10], exploreland[10][landtypes], landcount[landtypes];
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
vector<cell*> worms, ivies, ghosts, golems, mirrors, mirrors2, hexsnakes;

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

inline cell*& singlepos() { return cwt.c; }
inline bool singleused() { return !(shmup::on || multi::players > 1); }

#include "mtrand.cpp"

MTRand_int32 r;

void shrand(int i) {
  r.seed(i);
  }

int hrandpos() { return r() & HRANDMAX; }

int hrand(int i) { 
  return r() % i;
  }

void initcell(cell *c) {
  c->mpdist = INFD;   // minimum distance from the player, ever
  c->cpdist = INFD;   // current distance from the player
  c->pathdist = PINFD;// current distance from the player, along paths (used by yetis)
  c->landparam = 0; c->landflags = 0; c->wparam = 0;
  c->aitmp = 0;
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

int puregold() {
  int i = items[itOrbYendor] * 50 + items[itHolyGrail] * 10;
  if(items[itOrbLove]) i += 30;
  for(int t=0; t<ittypes; t++) 
    if(itemclass(eItem(t)) == IC_TREASURE)
      i += items[t];
  return i;
  }

int numplayers() {
  return multi::players;
  }

cell *playerpos(int i) {
  if(shmup::on) return shmup::playerpos(i);
  if(multi::players > 1) return multi::player[i].c;
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
  if(i != itHyperstone) for(int i=0; i<size(dcal); i++) {
    cell *c2 = dcal[i];
    if(c2->cpdist > 3) break;
    eItem i2 = treasureType(c2->land);
    if(i2 && items[i2] < currentLocalTreasure)
      currentLocalTreasure = items[i2];
    }
  }

int gold() {
  return puregold();
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
    &kills[moGadfly], &kills[moButterfly],
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
  return isWarped(c->land) || (items[itOrb37] && c->cpdist <= 4);
  }

// returns ishept in the normal tiling;
// in the 'pure heptagonal' tiling, returns true for a set of cells
// which roughly corresponds to the heptagons in the normal tiling
bool pseudohept(cell *c) {
  if(purehepta) {
    if(sphere) 
      return 
        c->master == getDodecahedron(3) ||
        c->master == getDodecahedron(5) ||
        c->master == getDodecahedron(6);
    int z = zebra40(c);
    return z == 5 || z == 8 || z == 15;
    }
  else return ishept(c);
  }

bool nonAdjacent(cell *c, cell *c2) {
  if(isWarped(c) && isWarped(c2) && !pseudohept(c) && !pseudohept(c2)) {
    /* int i = neighborId(c, c2);
    cell *c3 = c->mov[(i+1)%6], *c4 = c->mov[(i+5)%6];
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

bool againstWind(cell *c2, cell *c1); // to, from

// w = from->mov[d]
bool againstCurrent(cell *w, cell *from) {
  if(from->land != laWhirlpool) return false;
  if(againstWind(from, w)) return false; // wind is stronger than current
  if(!euclid && (!from->master->alt || !w->master->alt)) return false;
  int dfrom = celldistAlt(from);
  int dw = celldistAlt(w);
  if(dw < dfrom) return false;
  if(dfrom < dw) return true;
  for(int d=0; d<from->type; d++) 
    if(from->mov[d] == w) {
       cell *c3 = from->mov[(d+from->type-1) % from->type];
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
    c->wall == waBonfireOff || c->wall == waFire || c->wall == waPartialFire;
  }

void placeWater(cell *c, cell *c2) {
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

  if(from && from != w && nonAdjacent(from, w) && !F(P_IGNORE37 | P_BULLET)) return false;
  
  for(int i=0; i<numplayers(); i++) {
    cell *pp = playerpos(i);
    if(!pp) continue;
    if(w == pp && F(P_ONPLAYER)) return true;
    if(from == pp && F(P_ONPLAYER) && F(P_REVDIR)) return true;

    if(from && !((flags & P_ISPLAYER) && getMount(i))) {
      int i = revdir ? incline(w, from) : incline(from, w);
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

  if(from && strictlyAgainstGravity(w, from, revdir, flags)
    && !F(P_GRAVITY | P_BLOW | P_JUMP1 | P_JUMP2 | P_FLYING | P_BULLET | P_AETHER)
    ) return false;
  
  if(from && (revdir ? againstWind(from,w) : againstWind(w, from)) && !F(P_WIND | P_BLOW | P_JUMP1 | P_JUMP2 | P_BULLET | P_AETHER)) return false;
  
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
  
  if(F(P_BULLET)) {
    if(isFire(w) || w->wall == waBonfireOff || cellHalfvine(w) || 
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

  if(isFire(w)) {
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
    cell *c2 = c->mov[i];
    if(c2 && c2->monst == moAirElemental) {
      airdir = c->spn(i) * S42 / c2->type;
      return;
      }
    }
  for(int i=0; i<c->type; i++) {
    cell *c2 = c->mov[i];
    if(!c2) continue;
    if(!passable(c2, c, P_BLOW | P_MONSTER)) continue;
    if(!passable(c, c2, P_BLOW | P_MONSTER)) continue;
    for(int i=0; i<c2->type; i++) {
      cell *c3 = c2->mov[i];
      if(c3 && c3->monst == moAirElemental) {
        airdir = c2->spn(i) * S42 / c3->type;
        return;
        }
      }
    }
  return;
  }

bool againstWind(cell *cto, cell *cfrom) {
  if(!cfrom || !cto) return false;
  if(airdist(cto) < airdist(cfrom)) return true;
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
    for(int i=0; i<to->type; i++) 
      if(to->mov[i] && to->mov[i] != from && isGhost(to->mov[i]->monst) &&
        (to->mov[i]->monst == moFriendlyGhost) == (m== moFriendlyGhost))
        return false;
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
    for(int t=0; t<c->type; t++) if(c->mov[t] && c->mov[t]->wall == c->wall) i=t;
    int z = i-u; if(z<0) z=-z; z%=6;
    if(z>1) return false;
    hv=(group == ogroup);
    }
  // only travel from halfvines correctly
  if(cellHalfvine(w)) {
    int i=0;
    for(int t=0; t<w->type; t++) if(w->mov[t] && w->mov[t]->wall == w->wall) i=t;
    int z = i-c->spn(u); if(z<0) z=-z; z%=6;
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
    !isWorm(c->monst) && !isReptile(c->wall);
  }

void moveBoat(cell *to, cell *from) {
  eWall x = to->wall; to->wall = from->wall; from->wall = x;
  to->mondir = neighborId(to, from);
  moveItem(from, to, false);
  animateMovement(from, to, LAYER_BOAT);
  }

void moveBoatIfUsingOne(cell *to, cell *from) {
  if(from->wall == waBoat && isWatery(to)) moveBoat(to, from);
  else if(from->wall == waBoat && boatGoesThrough(to) && markEmpathy(itOrbWater)) {
    placeWater(to, from);
    moveBoat(to, from);
    }
  }

bool passable_for(eMonster m, cell *w, cell *from, flagtype extra) {
  if(w->monst && !(extra & P_MONSTER) && !isPlayerOn(w)) 
    return false;
  if(m == moWolf) {
    return isIcyLand(w) && (isPlayerOn(w) || passable(w, from, extra));
    }
  if(normalMover(m) || isBug(m) || isDemon(m) || m == moHerdBull) {
    if((isWitch(m) || m == moEvilGolem) && w->land != laPower && w->land != laHalloween)
      return false;
    return passable(w, from, extra);
    }
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
    if(pseudohept(w) || pseudohept(from)) return false;
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
  return false;
  }

eMonster movegroup(eMonster m) {
  if(isWitch(m) || m == moEvilGolem) {
    if(m == moWitchGhost) return moWitchGhost;
    if(m == moWitchWinter) return moWitchWinter;
    return moWitch;
    }
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
  return moNone;
  }

void useup(cell *c) {
  c->wparam--;
  if(c->wparam == 0) {
    drawParticles(c, c->wall == waFire ? 0xC00000 : winf[c->wall].color, 10, 50);
    if(c->wall == waTempFloor)
      c->wall = waChasm;
    else if(c->wall == waTempBridge)
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
      w = w->mov[w->mondir];
      // printf("w = %p mondir = %d\n", w, w->mondir);
      }
    
    }
  return w == killed;
  }

bool canAttack(cell *c1, eMonster m1, cell *c2, eMonster m2, flagtype flags) {

  // cannot eat worms
  if((flags & AF_EAT) && isWorm(m2)) return false;
  
  if((flags & AF_GETPLAYER) && isPlayerOn(c2)) m2 = moPlayer;
  
  if(!m2) return false;
  
  if((flags & AF_ONLY_FRIEND) && m2 != moPlayer && !isFriendly(c2)) return false;
  if((flags & AF_ONLY_FBUG)   && m2 != moPlayer && !isFriendlyOrBug(c2)) return false;
  if((flags & AF_ONLY_ENEMY) && (m2 == moPlayer || isFriendlyOrBug(c2))) return false;

  if(m2 == moHedge && !(flags & (AF_STAB | AF_TOUGH | AF_EAT | AF_MAGIC | AF_LANCE | AF_SWORD_INTO | AF_HORNS)))
    if(!checkOrb(m1, itOrbThorns)) return false;
  
  if(m2 == moDraugr && !(flags & (AF_SWORD | AF_MAGIC | AF_SWORD_INTO | AF_HORNS))) return false;

  // if(m2 == moHerdBull && !(flags & AF_MAGIC)) return false;
  if(isBull(m2) && !(flags & (AF_MAGIC | AF_HORNS | AF_SWORD_INTO))) return false;
  if(m2 == moButterfly && !(flags & (AF_MAGIC | AF_BULL | AF_HORNS | AF_SWORD_INTO))) return false;
  
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
    if(!(flags & (AF_MAGIC | AF_SWORD_INTO | AF_HORNS))) return false;
    
  if(!(flags & AF_GUN)) {

    if(c1 && c2 && nonAdjacent(c1,c2) && !attackNonAdjacent(m1) && !attackNonAdjacent(m2) && 
      !checkOrb(m1, itOrb37) && !checkOrb(m1, itOrbAether) && !checkOrb(m2, itOrbAether))
      return false;
    
    if(c1 && c2 && thruVine(c1,c2) && !attackThruVine(m1) && !attackThruVine(m2) &&
      !checkOrb(m1, itOrbAether) && !checkOrb(m2, itOrbAether))
      return false;
  
    if(!(flags & (AF_LANCE | AF_STAB | AF_BACK | AF_APPROACH)))
    if(c1 && c2 && againstRose(c1, c2) && !ignoresSmell(m1))
      return false;
    
    }
  
  if(m2 == moShadow && !(flags & AF_SWORD)) return false;
  if(isWorm(m2) && m2 != moTentacleGhost && !isDragon(m2)) return false;
  
  // dragon can't attack itself, or player who mounted it
  if(c1 && c2 && isWorm(c1->monst) && isWorm(c2->monst) && wormhead(c1) == wormhead(c2)
    && m1 != moTentacleGhost && m2 != moTentacleGhost)
    return false;
    
  // if(m2 == moTortoise && !(flags & AF_MAGIC)) return false;
  
  if(m2 == moRoseBeauty)
    if(!(flags & (AF_MAGIC | AF_LANCE | AF_GUN | AF_SWORD_INTO))) 
    if(!isMimic(m1))
    if(!checkOrb(m1, itOrbBeauty) && !checkOrb(m1, itOrbAether) && !checkOrb(m1, itOrbShield))
    if(!c1 || !c2 || !withRose(c1,c2))
      return false;
  
  if(m2 == moFlailer && !c2->stuntime)
    if(!(flags & (AF_MAGIC | AF_TOUGH | AF_EAT | AF_HORNS | AF_LANCE | AF_BACK | AF_SWORD_INTO))) return false;

  if(m2 == moVizier && c2->hitpoints > 1)
    if(!(flags & (AF_MAGIC | AF_TOUGH | AF_EAT | AF_HORNS | AF_LANCE | AF_BACK | AF_FAST))) return false;
                       
  return true;
  }

bool stalemate1::isKilled(cell *w) {
  if(w->monst == moNone || w == killed) return true;
  if(!moveto) return false;
  
  for(int b=0; b<2; b++) 
    if((w == swordnext[b] || w == swordtransit[b]) && canAttack(moveto, who, w, w->monst, AF_SWORD))
      return true;
  
  if(isNeighbor(w, moveto) && moveto != comefrom) {
    int wid = neighborId(moveto, w);
    int wfrom = neighborId(moveto, comefrom);
    int flag = AF_APPROACH;
    if(wid >= 0 && wfrom >= 0 && angledist(moveto, wfrom, wid) >= 3) flag |= AF_HORNS;
    if(canAttack(moveto, who, w, w->monst, flag)) return true;
    }

  if(isNeighbor(w, comefrom) && isNeighbor(w, moveto) && moveto != comefrom)
    if(canAttack(moveto, who, w, w->monst, AF_STAB))
      return true;

  if(who == moPlayer && (killed || moveto != comefrom) && mirrorkill(w)) return true;
  if(w->monst == moIvyHead || w->monst == moIvyBranch || isMutantIvy(w))
    return isChild(w, killed);

  if(isDragon(w->monst) && killed && isDragon(killed->monst) && killed->hitpoints) {
    cell *head1 = dragon::findhead(w);
    cell *head2 = dragon::findhead(killed);
    if(head1 == head2 && dragon::totalhp(head1) ==1) return true;
    }
  
  if(w->monst == moKrakenT && killed && killed->monst == moKrakenT && killed->hitpoints) {
    cell *head1 = w->mov[w->mondir];
    cell *head2 = killed->mov[killed->mondir];
    if(head1 == head2 && kraken::totalhp(head1) == 1) return true;
    }
  
  return false;
  }

bool stalemate::isKilled(cell *w) {
  for(int f=0; f<size(moves); f++)
    if(moves[f].isKilled(w)) return true;
  
  return false;
  };

bool isNeighbor(cell *c1, cell *c2) {
  for(int i=0; i<c1->type; i++) if(c1->mov[i] == c2) return true;
  return false;
  }

bool isNeighborCM(cell *c1, cell *c2) {
  for(int i=0; i<c1->type; i++) if(createMov(c1, i) == c2) return true;
  return false;
  }

int neighborId(cell *ofWhat, cell *whichOne) {
  for(int i=0; i<ofWhat->type; i++) if(ofWhat->mov[i] == whichOne) return i;
  return -1;
  }

// how many monsters are near
eMonster which;

bool mirrorkill(cell *c) {
  for(int t=0; t<c->type; t++) 
    if(c->mov[t] && isMimic(c->mov[t]) && c->mov[t]->mov[c->mov[t]->mondir] == c)
      return true;
  return false;
  }

bool flashWouldKill(cell *c, flagtype extra) {
  for(int t=0; t<c->type; t++) {
    cell *c2 = c->mov[t];
    for(int u=0; u<c2->type; u++) {
      cell *c3 = c2->mov[u];
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

// this is not completely correct, but it works in the Wild West
bool outlawNearby(cell *c, int dist) {
  if(dist == 0) return false;
  for(int i=0; i<c->type; i++) {
    cell *c2 = c->mov[i];
    if(!c2) continue;
    if(c2->monst == moOutlaw && !stalemate::isKilled(c2)) 
      return true;
    if((passable(c2, NULL, 0) || (c2->monst && stalemate::isKilled(c2))) && 
       outlawNearby(c2, dist-1))
      return true;
    }
  return false;
  }

// int monstersnear(cell *c, cell *nocount = NULL, eMonster who = moPlayer, cell *pushto = NULL) {

namespace stalemate {
  bool anyKilled() {
    for(int i=0; i<size(moves); i++) if(moves[i].killed) return true;
    return false;
    }

  bool isMoveto(cell *c) {
    for(int i=0; i<size(moves); i++) if(moves[i].moveto == c) return true;
    return false;
    }

  bool isKilledDirectlyAt(cell *c) {
    for(int i=0; i<size(moves); i++) if(moves[i].killed == c) return true;
    return false;
    }
  
  bool isPushto(cell *c) {
    for(int i=0; i<size(moves); i++) if(moves[i].pushto == c) return true;
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
  
int monstersnear(stalemate1& sm) {

  cell *c = sm.moveto;
  bool eaten = false;

  if(hardcore && sm.who == moPlayer) return 0;
  int res = 0;
  bool fast = false;

  elec::builder b;
  if(elec::affected(c)) { which = moLightningBolt; res++; }

  if(sm.who == moPlayer || items[itOrbEmpathy]) {
    fast = (items[itOrbSpeed] && (items[itOrbSpeed] & 1));
    }
  
  if(havewhat&HF_OUTLAW)
    if(outlawNearby(c, 3)) {
      res++; which = moOutlaw;
      }
  
  for(int t=0; t<c->type; t++) {
    cell *c2 = c->mov[t];

    // consider monsters who attack from distance 2
    if(c2) for(int u=2; u<=c2->type-2; u++) {
      cell *c3 = c2->mov[(c->spn(t)+u) % c2->type];
      if(!c3) continue;
      if(c3->monst != moWitchFlash)
        if(nonAdjacent(c, c2) || nonAdjacent(c2,c3) || thruVine(c,c2) || thruVine(c2,c3))
          continue;
      // only these monsters can attack from two spots...
      if(c3->monst != moLancer && c3->monst != moWitchSpeed && c3->monst != moWitchFlash) 
        continue;
      if(elec::affected(c3)) continue;
      if(c3->stuntime) continue;
      // speedwitches can only attack not-fastened monsters,
      // others can only attack if the move is not fastened
      if(c3->monst == moWitchSpeed && items[itOrbSpeed]) continue;
      if(c3->monst != moWitchSpeed && fast) continue;
      // cannot attack if the immediate cell is impassable (except flashwitches)
      if(c3->monst != moWitchFlash) {
        if(!passable(c2, c3, stalemate::isKilled(c2)?0:P_MONSTER)) continue;
        if(isPlayerOn(c2) && items[itOrbFire]) continue;
        }
      // flashwitches cannot attack if it would kill another enemy
      if(c3->monst == moWitchFlash && flashWouldKill(c3, 0)) continue;
      if(stalemate::anyKilled() && mirrorkill(c3)) continue;
      res++, which = c3->monst;
      } 

    // consider normal monsters
    if(c2 && 
      isArmedEnemy(c2, sm.who) && 
      !stalemate::isKilled(c2) &&
      (c2->monst != moLancer || isUnarmed(sm.who))) {
      eMonster m = c2->monst;
      if(elec::affected(c2)) continue;
      if(fast && c2->monst != moWitchSpeed) continue;
      // Krakens just destroy boats
      if(c2->monst == moKrakenT && onboat(sm)) {
        if(krakensafe(c)) continue;
        else if(warningprotection() && res == 0) m = moWarning;
        else continue;
        }
      // they cannot attack through vines
      if(!canAttack(c2, c2->monst, c, sm.who, AF_NEXTTURN)) continue;
      if(c2->monst == moWorm || c2->monst == moTentacle || c2->monst == moHexSnake) {
        if(passable_for(c2->monst, c, c2, 0))
          eaten = true;
        else if(c2->monst != moHexSnake) continue;
        }
      res++, which = m;
      }
    }

  if(sm.who == moPlayer && res && (markOrb2(itOrbShield) || markOrb2(itOrbShell)) && !eaten)
    res = 0;

  if(sm.who == moPlayer && res && markOrb2(itOrbDomination) && c->monst)
    res = 0;

  return res;
  }

namespace multi { bool aftermove; }

int monstersnear2();

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

int monstersnear2() {
  multi::cpid++;
  int b = 0;
  bool recorduse[ittypes];
  for(int i=0; i<ittypes; i++) recorduse[i] = orbused[i];
  if(multi::cpid == multi::players || multi::players == 1 || multi::checkonly) {
  
    // check for safe orbs first
    for(int i=0; i<size(stalemate::moves); i++)
      if(hasSafeOrb(stalemate::moves[i].moveto)) {
        multi::cpid--; return 0;
        }
    
    for(int i=0; i<size(stalemate::moves); i++)
    for(int j=0; j<size(stalemate::moves); j++) if(i != j) {
      if(swordConflict(stalemate::moves[i], stalemate::moves[j])) {
          b = 1;
          which = moEnergySword;
          }
      if(multi::player[i].c == multi::player[j].c) 
        { b = 1; which = moFireball; }
      if(celldistance(multi::player[i].c, multi::player[j].c) > 8) 
        { b = 1; which = moAirball; }
      }

    for(int i=0; !b && i<size(stalemate::moves); i++)
      b = monstersnear(stalemate::moves[i]);
    }
  else b = !multimove();
  multi::cpid--;
  for(int i=0; i<ittypes; i++) orbused[i] = recorduse[i];
  return b;
  }

int monstersnear(cell *c, cell *nocount, eMonster who, cell *pushto, cell *comefrom) {
  
  stalemate1 sm(who, c, nocount, pushto, comefrom);
  
  if(who == moPlayer) for(int b=0; b<2; b++) sm.swordlast[b] = sword::pos(multi::cpid, b);
  
  cell *none;
  cell **wcw = &cwt.c;
  if(who != moPlayer) wcw = &none;
  else if(multi::players > 1) wcw = &multi::player[multi::cpid].c;
  
  dynamicval<cell*> x5(*wcw, c);
  dynamicval<bool> x6(stalemate::nextturn, true);
  dynamicval<int> x7(sword::angle[multi::cpid], 
    who == moPlayer ? sword::shift(comefrom, c, sword::angle[multi::cpid]) :
    sword::angle[multi::cpid]);
  
  if(who == moPlayer) for(int b=0; b<2; b++) {
    sm.swordnext[b] = sword::pos(multi::cpid, b);
    sm.swordtransit[b] = NULL;
    if(sm.swordnext[b] && sm.swordnext[b] != sm.swordlast[b] && !isNeighbor(sm.swordlast[b], sm.swordnext[b]))
      forCellEx(c2, sm.swordnext[b])
        if(c2 != c && c2 != comefrom && isNeighbor(c2, sm.swordlast[b]))
          sm.swordtransit[b] = c2;
    }

  stalemate::moves.push_back(sm);
  
  // dynamicval<eMonster> x7(stalemate::who, who);
  
  int b;
  if(who == moPlayer && c->wall == waBigStatue) {
    eWall w = comefrom->wall;
    c->wall = waNone;
    if(doesnotFall(comefrom)) comefrom->wall = waBigStatue;
    b = monstersnear2();
    comefrom->wall = w;
    c->wall = waBigStatue;
    }
  if(who == moPlayer && c->wall == waThumperOn) {
    c->wall = waNone;
    b = monstersnear2();
    c->wall = waThumperOn;
    }
  b = monstersnear2();
  stalemate::moves.pop_back();
  return b;
  }
        
void killIvy(cell *c, eMonster who) {
  if(c->monst == moIvyDead) return;
  if(checkOrb(who, itOrbStone)) c->wall = waPetrified, c->wparam = c->monst;
  c->monst = moIvyDead; // NEWYEARFIX
  for(int i=0; i<c->type; i++) if(c->mov[i])
    if(isIvy(c->mov[i]) && c->mov[i]->mondir == c->spn(i))
      killIvy(c->mov[i], who);
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
  // these walls block spilling completely
  if(c->wall == waIcewall || c->wall == waBarrier ||  c->wall == waWarpGate ||
    c->wall == waDeadTroll || c->wall == waDeadTroll2 || 
    c->wall == waDune || c->wall == waAncientGrave || 
    c->wall == waThumperOff || c->wall == waThumperOn ||
    c->wall == waFreshGrave || c->wall == waColumn || c->wall == waPartialFire ||
    c->wall == waDeadwall || c->wall == waWaxWall || c->wall == waCamelot || c->wall == waRoundTable ||
    c->wall == waBigStatue || c->wall == waRed1 || c->wall == waRed2 || c->wall == waRed3 ||
    c->wall == waTower ||
    c->wall == waPalace || c->wall == waOpenGate || c->wall == waClosedGate ||
    c->wall == waPlatform || c->wall == waStone || c->wall == waTempWall ||
    c->wall == waTempFloor || c->wall == waTempBridge ||
    c->wall == waSandstone || c->wall == waCharged || c->wall == waGrounded ||
    c->wall == waMetal || c->wall == waSaloon || c->wall == waFan ||
    c->wall == waBarrowDig || c->wall == waBarrowWall)
    return;
  // these walls block further spilling
  if(c->wall == waCavewall || cellUnstable(c) || c->wall == waSulphur ||
    c->wall == waSulphurC || c->wall == waLake || c->wall == waChasm ||
    c->wall == waBigTree || c->wall == waSmallTree || c->wall == waTemporary ||
    c->wall == waVinePlant || isFire(c) || c->wall == waBonfireOff || c->wall == waVineHalfA || c->wall == waVineHalfB ||
    c->wall == waCamelotMoat || c->wall == waSea || c->wall == waCTree ||
    c->wall == waRubble || c->wall == waGargoyleFloor || c->wall == waGargoyle ||
    c->wall == waRose || c->wall == waPetrified)
      t = waTemporary;

  if(c->wall == waSulphur) {
    // remove the center as it would not look good
    for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->wall == waSulphurC)
      c->mov[i]->wall = waSulphur;
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
  // cwt.c->item = itNone;
  if(rad) for(int i=0; i<c->type; i++) if(c->mov[i])
    prespill(c->mov[i], t, rad-1, c);
  }

void spillfix(cell* c, eWall t, int rad) {
  if(c->wall == waTemporary) c->wall = t;
  if(rad) for(int i=0; i<c->type; i++) if(c->mov[i])
    spillfix(c->mov[i], t, rad-1);
  }

void spill(cell* c, eWall t, int rad) {
  prespill(c,t,rad, c); spillfix(c,t,rad);
  }

void degradeDemons() {
  addMessage(XLAT("You feel more experienced in demon fighting!"));
  int dcs = size(dcal);
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
  if(c->wall == waStone) { c->wall = waNone; return true; }
  if(c->wall == waAncientGrave || c->wall == waFreshGrave) {
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
  if((c->wall == waBarrowDig || c->wall == waBarrowWall) && c->land == laBurial) {
    c->item = itNone;
    c->wall = waNone;
    return true;
    }
  if(c->wall == waPlatform && c->land == laMountain) {
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
  if(c->wall == waNone && c->land == laBurial) {
    c->item = itNone;
    c->wall = waBarrowDig;
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
    c->wall == waTempFloor || c->wall == waTempBridge) {
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
  if(itemBurns(c->item)) {
    if(checkonly) return true;
    addMessage(XLAT("%The1 burns!", c->item)), c->item = itNone;
    }
  if(cellUnstable(c)) {
    if(checkonly) return true;
    doesFall(c);
    }
  else if(c->wall == waChasm || c->wall == waOpenGate || c->wall == waRed2 || c->wall == waRed3 ||
    c->wall == waTower)
    return false;
  else if(c->wall == waBoat) {
    if(checkonly) return true;
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
  
  for(int i=0; i<c->type; i++) if(c->mov[i]) {
    cell *c2 = c->mov[i];
    if(c2->wall == waRed2 || c2->wall == waRed3)
      c2->wall = waRed1;
    else if(c2->wall == waDeadTroll || c2->wall == waDeadTroll2 || c2->wall == waPetrified || c2->wall == waGargoyle) {
      c2->wall = waNone;
      makeflame(c2, 10, false);
      }
    else if(c2->wall == waPalace || c2->wall == waOpenGate || c2->wall == waClosedGate ||
      c2->wall == waSandstone || c2->wall == waMetal || c2->wall == waSaloon) {
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
  if(c2->monst != moSkeleton && !isMetalBeast(c2->monst) && c2->monst != moTortoise &&
    c2->monst != moReptile) {
    c2->hitpoints--;
    if(c2->monst == moPrincess)
      playSound(c2, princessgender() ? "hit-princess" : "hit-prince");
    }
  c2->stuntime = (
    c2->monst == moFatGuard ? 2 : 
    c2->monst == moSkeleton && c2->land != laPalace && c2->land != laHalloween ? 7 :
    isMetalBeast(c2->monst) ? 7 :
    c2->monst == moTortoise ? 7 :
    c2->monst == moReptile ? 7 :
    isPrincess(c2->monst) ? 6 :
    // spear stunning
    isBull(c2->monst) ? 3 :
    (c2->monst == moGreater || c2->monst == moGreaterM) ? 5 :
    c2->monst == moButterfly ? 2 :
    c2->monst == moDraugr ? 1 :
    c2->monst == moVizier ? 1 :
    c2->monst == moHedge ? 1 :
    c2->monst == moFlailer ? 1 :
    3);
  if(isBull(c2->monst)) c2->mondir = NODIR;
  checkStunKill(c2);
  }

bool attackJustStuns(cell *c2) {
  return isStunnable(c2->monst) && c2->hitpoints > 1;
  }
  
void moveEffect(cell *ct, cell *cf, eMonster m);
  
void flameHalfvine(cell *c, int val) {
  if(itemBurns(c->item)) {
    c->item = itNone;
    addMessage(XLAT("%The1 burns!", c->item)), c->item = itNone;
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
  else if(c->wall == waTempBridge) placeWater(c, NULL);
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
  if(checkOrb(who, itOrbStone)) c->wall = waPetrified, c->wparam = moMutant;
  removeIvy(c);
  for(int i=0; i<c->type; i++)
    if(c->mov[i]->mondir == c->spn(i) && (isMutantIvy(c->mov[i]) || c->mov[i]->monst == moFriendlyIvy))
      killMutantIvy(c->mov[i], who);
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
  
  int pcount = (deathflags & AF_FALL) ? 0 : 16; 
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
  if(!isBulletType(m)) kills[m]++;

  if(!c->item) if(m == moButterfly && (deathflags & AF_BULL))
    c->item = itBull;
  
  if(isRatling(m) && c->wall == waBoat) {
    bool avenge = false;
    for(int i=0; i<c->type; i++) if(!isWarped(c->mov[i]->land))
      avenge = true;
    if(avenge) { avengers += 2; }
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
        if(items[itSavedPrincess] == 0) {
          items[itOrbLove] = 0;
          princess::reviveAt = gold() + 20;
          }
        }
      if(princess::challenge) showMissionScreen();
      }
    }

  if(m == moGargoyle && c->wall != waMineMine) {
    bool connected = false;
    
    if(isGravityLand(c->land)) {
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if(c2->wall == waPlatform || c2->wall == waGargoyle || c2->wall == waBarrier ||
          c2->wall == waDeadTroll || c2->wall == waDeadTroll2 || c2->wall == waTrunk ||
          c2->wall == waPetrified || isAlchAny(c2->wall))
          connected = true;
        }
      }
    else {
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if(!cellUnstableOrChasm(c2) && !isWatery(c2)) connected = true;
        }
      }
    
    if(connected) {
      pcount = 0;
      playSound(c, "die-troll");
      destroyHalfvine(c);
      if(cellUnstableOrChasm(c)) c->wall = waGargoyleFloor;
      else if(isWatery(c)) c->wall = waGargoyleBridge;
      else c->wall = waGargoyle;
      c->item = itNone;
      }
    }
    
  if(m == moTroll) {
    destroyHalfvine(c);
    if(doesnotFall(c)) {
      pcount = 0;
      playSound(c, "die-troll");
      if(isReptile(c->wall)) kills[moReptile]++;
      c->wall = waDeadTroll;
      }
    else fallingFloorAnimation(c, waDeadTroll, m), pcount = 0;
    c->item = itNone;
    for(int i=0; i<c->type; i++) if(c->mov[i]) {
      c->mov[i]->item = itNone;
      if(c->mov[i]->wall == waDeadwall || c->mov[i]->wall == waDeadfloor2) c->mov[i]->wall = waCavewall;
      if(c->mov[i]->wall == waDeadfloor) c->mov[i]->wall = waCavefloor;
      }
    }
  if(m == moFjordTroll || m == moForestTroll || m == moStormTroll) {
    destroyHalfvine(c);
    if(doesnotFall(c)) {
      if(isReptile(c->wall)) kills[moReptile]++;
      c->wall = waDeadTroll2;
      pcount = 0;
      playSound(c, "die-troll");
      }
    else fallingFloorAnimation(c, waDeadTroll2, m), pcount = 0;
    c->wparam = m;
    c->item = itNone;
    }
  if(m == moMiner) {
    pcount = 32;
    playSound(c, "splash" + pick12());
    destroyHalfvine(c);
    minerEffect(c);
    for(int i=0; i<c->type; i++) if(passable(c->mov[i], c, P_MONSTER | P_MIRROR | P_CLIMBUP | P_CLIMBDOWN)) {
      destroyHalfvine(c->mov[i]);
      minerEffect(c->mov[i]);
      if(c->mov[i]->monst == moSlime || c->mov[i]->monst == moSlimeNextTurn)
        killMonster(c->mov[i], who);
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
  if(m == moVineBeast) {
    destroyHalfvine(c);
    if(doesnotFall(c)) {
      if(isReptile(c->wall)) kills[moReptile]++;
      c->wall = waVinePlant;
      pcount = 0;
      playSound(c, "die-vinebeast");
      }
    else fallingFloorAnimation(c, waVinePlant, m), pcount = 0;
    c->item = itNone;
    }
  if(isBird(m)) moveEffect(c, c, moDeadBird);
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
      else if(c->land == laMinefield) c->landparam |= 1;
      }
    else if(c->wall == waFrozenLake)
      c->wall = waLake;
    else if(c->wall == waGargoyleBridge)
      placeWater(c, c);
    else if(c->wall == waRed3 || c->wall == waRed2) {
      c->wall = waRed1;
      for(int i=0; i<c->type; i++) if(c->mov[i]->wall == waRed3)
        c->mov[i]->wall = waRed2;
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
  if(checkOrb(who, itOrbStone)) {
    destroyHalfvine(c);
    if(doesnotFall(c)) {
      if(isReptile(c->wall)) kills[moReptile]++;
      c->wall = waPetrified;
      pcount = 0;
      playSound(c, "die-troll");
      }
    else fallingFloorAnimation(c, waPetrified, m), pcount = 0;
    c->wparam = m;
    c->item = itNone;
    }
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
      euclid ||
      (c->master->alt && celldistAlt(c) <= (purehepta ? -3 : -5)) ||
      isHaunted(c->land))) {
    bool toomany = false;
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->mov[i];
      if(c2 && c2->item == itCompass) toomany = true;
      if(c2 && !purehepta) for(int j=0; j<c2->type; j++)
        if(c2->mov[j] && c2->mov[j]->item == itCompass)
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
    /*if((m == moIvyBranch || m == moIvyHead) && c->mov[c->mondir]->monst == moIvyRoot)
      ivynext(c, moIvyNext); */
    killIvy(c, who);
    if(m == moIvyBranch || m == moIvyHead || m == moIvyNext) {
      int qty = 0;
      cell *c2 = c->mov[c->mondir];
      for(int i=0; i<c2->type; i++)
        if(c2->mov[i]->monst == moIvyWait && c2->mov[i]->mondir == c2->spn(i))
          qty++;
      if(c->mov[c->mondir]->monst == moIvyRoot || qty) {
        c->monst = moIvyNext;
        /* c->monst = moIvyHead;
        ivynext(c);
        if(c->monst == moIvyHead) c->monst = moIvyNext;
        else c->monst = moNone; */
        }
      else {
        c->mov[c->mondir]->monst = moIvyHead;
        }
      }
    }
  else if(c->monst == moTentacleGhost)
    c->monst = moTentacletail;
  else c->monst = moNone;

  if(m == moEarthElemental) earthWall(c);
  if(m == moAlbatross && items[itOrbLuck]) 
    useupOrb(itOrbLuck, items[itOrbLuck] / 2);
  
  if(m == moAirElemental) {
    airmap.clear();
    for(int i=0; i<size(dcal); i++)
      if(dcal[i]->monst == moAirElemental)
        airmap.push_back(make_pair(dcal[i],0));
    buildAirmap();
    }
  drawParticles(c, minf[m].color, pcount);
  if(deathflags & AF_FALL)
    fallingMonsterAnimation(c, m);
  }

void fightmessage(eMonster victim, eMonster attacker, bool stun, int flags) {

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
    else {
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
  
  bool dostun = (flags & AF_ORSTUN) && attackJustStuns(c);
  
  if((flags & AF_HORNS) && hornStuns(c)) dostun = true;
  
  if(c->monst == moSkeleton && (flags & AF_SWORD)) dostun = false;

  bool eu = elementalUnlocked();
  bool tu = trollUnlocked();
  
  if(flags & AF_MSG) fightmessage(m, killer, dostun, flags);
  if(dostun) 
    stunMonster(c);
  else 
    killMonster(c, killer, flags);

  int ntk = tkills();
  int ntkt = killtypes();
    
  if(tkt < 20 && ntkt >= 20) {
    addMessage(XLAT("You hear a distant roar!"));
    playSound(NULL, "message-roar");
    }

  if(tk == 0 && ntk > 0 && !tactic::on && !euclid && !sphere) {
    if(notthateasy(m))
      addMessage(XLAT("Quite tough, for your first fight."));
    else 
      addMessage(XLAT("That was easy, but groups could be dangerous."));
    }
    
  if(tk < 10 && ntk >= 10 && !tactic::on && !euclid && !sphere)
    addMessage(XLAT("Good to know that your fighting skills serve you well in this strange world."));

  if(tk < 50 && ntk >= 50 && !euclid && !sphere)
    addMessage(XLAT("You wonder where all these monsters go, after their death..."));

  if(tk < 100 && ntk >= 100 && !euclid && !sphere)
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

void pushMonster(cell *ct, cell *cf) {
  moveMonster(ct, cf);
  }

bool destroyHalfvine(cell *c, eWall newwall, int tval) {
  if(cellHalfvine(c)) {
    for(int t=0; t<c->type; t++) if(c->mov[t]->wall == c->wall) {
      if(newwall == waPartialFire) flameHalfvine(c->mov[t], tval);
      else if(newwall == waRed1) c->mov[t]->wall = waVinePlant;
      else c->mov[t]->wall = newwall;
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
  for(int i=0; i<c->type; i++) if(c->mov[i]) {
    if(isAnyIvy(c->mov[i]->monst) && 
      c->land == laMountain && !(flags & MF_IVY)) return false;
    if(gravityLevel(c->mov[i]) == d-1) {
      if(againstWind(c->mov[i], c)) return false;
      if(!passable(c->mov[i], NULL, P_MONSTER | P_DEADLY))
        return false;
      if(isWorm(c->mov[i]))
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
        cell* c2 = c->mov[i];
        if(c2 && isWorm(c2) && c2->mov[c2->mondir] == c) {
          settemp(c);
          c = c2;
          bug = false;
          }
        }
      if(bug) break;
      }
    else if(c->monst == moIvyWait) {
      cell* c2 = c->mov[c->mondir];
      settemp(c); c=c2;
      }
    else if(c->monst == moIvyHead) {
      ivies.push_back(c); settemp(c);
      break;
      }
    else if(c->monst == moIvyBranch || c->monst == moIvyRoot) {
      bool bug = true;
      for(int i=0; i<c->type; i++) {
        cell* c2 = c->mov[i];
        if(c2 && (c2->monst == moIvyHead || c2->monst == moIvyBranch) && c2->mov[c2->mondir] == c) {
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
  }

int tidespeed() {
  return tide[(turncount+6) % tidalsize] - tidalphase;
  }

bool recalcTide;

#define SEADIST LHU.bytes[0]
#define LANDDIST LHU.bytes[1]
#define CHAOSPARAM LHU.bytes[2]

void checkTide(cell *c) {
  if(c->land == laOcean) {
    int t = c->landparam;
    
    if(chaosmode) {
      char& csd(c->SEADIST); if(csd == 0) csd = 7;
      char& cld(c->LANDDIST); if(cld == 0) cld = 7;
      int seadist=csd, landdist=cld;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
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
  }

void buildAirmap() {
  for(int k=0; k<size(airmap); k++) {
    int d = airmap[k].second;
    if(d == 2) break;
    cell *c = airmap[k].first;
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->mov[i];
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
    for(int k=0; k<size(dcal); k++) {
      cell *c = dcal[k];
      if(c->wall == waRose && c->cpdist <= 5) 
        rosemap[c] = rosewave * 8 + 2;
      }
    }
  
  for(map<cell*, int>::iterator it = rosemap.begin(); it != rosemap.end(); it++) {
    cell *c = it->first;
    int r = it->second;
    if(r < (rosewave) * 8) continue;
    if((r&7) == 2) if(c->wall == waRose || !isWall(c)) for(int i=0; i<c->type; i++) {
      cell *c2 = c->mov[i];
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
    }

  }

int getDistLimit() { return purehepta?5:7; }

bool nogoSlow(cell *to, cell *from) {
  if(cellEdgeUnstable(to) && gravityLevel(to) >= gravityLevel(from)) return true;
  if(cellUnstable(to)) return true;
  return false;
  }

// pathdist begin
void computePathdist(eMonster param) {
  int pqs = size(pathq);
  for(int i=0; i<pqs; i++) pathq[i]->pathdist = PINFD;
  pathq.clear(); 
  pathqm.clear();
  reachedfrom.clear(); 
  
  for(int i=0; i<size(targets); i++) {
    pathq.push_back(targets[i]);
    targets[i]->pathdist = isPlayerOn(targets[i]) ? 0 : 1;
    reachedfrom.push_back(hrand(targets[i]->type));
    }

  int qtarg = size(targets);

  for(int qb=0; qb < size(pathq); qb++) {
    int fd = reachedfrom[qb] + 3;
    cell *c = pathq[qb];
    if(c->monst && !isBug(c) && !(isFriendly(c) && !c->stuntime)) {
      pathqm.push_back(c); 
      continue; // no paths going through monsters
      }
    for(int i=0; i<numplayers(); i++) {
      cell *pc = playerpos(i);
      if(!pc) continue;
      if(c->monst && c != pc && getMount(i) && !isDragon(c->monst) && sameMonster(c, pc)) {
        // don't treat the Worm you are riding as passable
        pathqm.push_back(c); 
        continue; 
        }
      }
    if(c->cpdist > 7 && !(c->land == laTrollheim && turncount < c->landparam)) continue;
    int d = c->pathdist;
    if(d == PINFD - 1) continue;
    for(int j=0; j<c->type; j++) {
      int i = (fd+j) % c->type; 
      // printf("i=%d cd=%d\n", i, c->mov[i]->cpdist);

      if(c->mov[i] && c->mov[i]->pathdist == PINFD &&
        passable(c->mov[i], (qb<qtarg) && !nonAdjacent(c,c->mov[i]) && !thruVine(c,c->mov[i]) ?NULL:c, P_MONSTER | P_REVDIR)) {
        
        if(qb >= qtarg) {
          if(param == moTortoise && nogoSlow(c, c->mov[i])) continue;
          if(param == moIvyRoot  && strictlyAgainstGravity(c, c->mov[i], false, MF_IVY)) continue;
          if(param == moWorm && (cellUnstable(c) || cellEdgeUnstable(c))) continue;
          }

        c->mov[i]->pathdist = d+1;
        pathq.push_back(c->mov[i]); reachedfrom.push_back(c->spn(i));
        }
      }
    }
  }
// pathdist end

vector<pair<cell*, int> > butterflies;

void addButterfly(cell *c) {
  for(int i=0; i<size(butterflies); i++)
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
  
  int dcs = size(dcal);
  for(int i=0; i<dcs; i++) dcal[i]->cpdist = INFD;
  worms.clear(); ivies.clear(); ghosts.clear(); golems.clear(); mirrors.clear();
  temps.clear(); tempval.clear(); targets.clear(); 
  statuecount = 0;
  hexsnakes.clear(); 

  hadwhat = havewhat;
  havewhat = 0;  
  if(!(hadwhat & HF_WARP)) { avengers = 0; }

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
  
  int distlimit = getDistLimit();

  for(int i=0; i<numplayers(); i++) {
    cell *c = playerpos(i);
    if(!c) continue;
    if(getMount(i) && (c->monst == moTentacle || c->monst == moTentaclewait || c->monst == moTentacleEscaping))
      worms.push_back(c);
    }
  
  int qb = 0;
  while(true) {
    if(qb == size(dcal)) break;
    int i, fd = reachedfrom[qb] + 3;
    cell *c = dcal[qb++];
    
    int d = c->cpdist;
    if(d == distlimit) { first7 = qb; break; }
    for(int j=0; j<c->type; j++) if(i = (fd+j) % c->type, c->mov[i]) {
      // printf("i=%d cd=%d\n", i, c->mov[i]->cpdist);
      cell *c2 = c->mov[i];
      if(!c2) continue;
      
      if(isWarped(c2->land)) havewhat |= HF_WARP;
      
      if((c->wall == waBoat || c->wall == waSea) &&
        (c2->wall == waSulphur || c2->wall == waSulphurC))
        c2->wall = waSea;
      
      if(c2 && signed(c2->cpdist) > d+1) {
        c2->cpdist = d+1;
        
        // remove treasures
        if(c2->item && c2->cpdist == distlimit && itemclass(c2->item) == IC_TREASURE &&
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
            if(c2->mov[i] && c2->mov[i]->monst == moMutant)
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
        reachedfrom.push_back(c->spn(i));
        
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
            if(c2->monst != moMouse && !markEmpathy(itOrbInvis) &&
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
            if(isMimic(c2)) mirrors.push_back(c2);
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
    for(int i=0; i<size(dcal); i++) checkTide(dcal[i]);
    }    
  
  int qtemp = size(temps);
  for(int i=0; i<qtemp; i++) temps[i]->monst = tempval[i];
  
  buildAirmap();
  
  if(overgenerate) doOvergenerate();
  }

bool makeEmpty(cell *c) {
  if(c->monst != moPrincess) {
    if(isAnyIvy(c->monst)) killMonster(c, moPlayer, 0);
    else if(isWorm(c->monst)) {
      if(!items[itOrbDomination]) return false;
      }
    else c->monst = moNone;
    }

  if(c->land == laCocytus)
    c->wall = waFrozenLake;
  else if(c->land == laAlchemist || c->land == laCanvas)
    ;
  else if(c->land == laCaves || c->land == laEmerald)
    c->wall = waCavefloor;
  else if(c->land == laDeadCaves)
    c->wall = waDeadfloor2;
  else if(c->land == laCaribbean || c->land == laOcean || c->land == laWhirlpool || c->land == laLivefjord || c->land == laWarpSea || c->land == laKraken)
    c->wall = waBoat; // , c->item = itOrbYendor;
  else if(c->land == laMinefield)
    c->wall = waMineOpen;
  else if(c->wall == waFan && sphere)
    ;
  else if(c->wall == waOpenPlate && sphere)
    ;
  else if(c->wall == waGiantRug)
    ;
  else if(c->wall == waFreshGrave && (sphere || quotient))
    ;
  else if(isReptile(c->wall))
    c->wparam = reptilemax();
  else if(c->wall == waAncientGrave && sphere)
    ;
  else
    c->wall = waNone;
  c->item = itNone;
  
  if(c->land == laWildWest) {
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->mov[i];
      if(c2) for(int j=0; j<c->type; j++) {
        cell *c3 = c2->mov[j];
        if(c3) c3->wall = waNone;
        }
      }
    }
  
  return true;
  }

int numgates = 0;

void toggleGates(cell *ct, eWall type, int rad) {
  if(!ct) return;
  if(ct->wall == waOpenGate && type == waClosePlate) {
    bool onWorm = false;
    if(isWorm(ct)) onWorm = true;
    for(int i=0; i<ct->type; i++) 
      if(ct->mov[i] && ct->mov[i]->wall == waOpenGate && isWorm(ct->mov[i])) onWorm = true;
    if(!onWorm) {
      ct->wall = waClosedGate, numgates++;
      if(rad<1) rad=1;
      if(ct->item) {
        playSound(ct, "hit-crush"+pick123());
        addMessage(XLAT("%The1 is crushed!", ct->item));
        ct->item = itNone;
        }
      }
    }
  if(ct->wall == waClosedGate && type == waOpenPlate)
    ct->wall = waOpenGate, rad = 1, numgates++;
  if(rad) for(int i=0; i<ct->type; i++)
    toggleGates(ct->mov[i], type, rad-1);
  }

void toggleGates(cell *ct, eWall type) {
  playSound(ct, "click");
  numgates = 0;
  if(type == waClosePlate && purehepta)
    toggleGates(ct, type, 2);
  else
    toggleGates(ct, type, 3);
  if(numgates && type == waClosePlate)
    playSound(ct, "closegate");
  if(numgates && type == waOpenPlate)
    playSound(ct, "opengate");
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

// effect of moving monster m from cf to ct
// this is called from moveMonster, or separately from moveIvy/moveWorm,
// or when a dead bird falls (then m == moDeadBird)

void moveEffect(cell *ct, cell *cf, eMonster m) {

  if(cf) destroyWeakBranch(cf, ct, m);

  mayExplodeMine(ct, m);
 
  if(ct->wall == waMineUnknown && !ct->item && !ignoresPlates(m)) 
    ct->landparam |= 2; // mark as safe

  if((ct->wall == waClosePlate || ct->wall == waOpenPlate) && !ignoresPlates(m))
    toggleGates(ct, ct->wall);
    
  if(cf && isPrincess(m)) princess::move(ct, cf);
  
  if(cf && m == moTortoise) {
    tortoise::emap[ct] = tortoise::get(cf);
    tortoise::emap.erase(cf);
    }
  
  if(cf && ct->item == itBabyTortoise && !cf->item) {
    cf->item = itBabyTortoise;
    ct->item = itNone;
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
  achievement_collection(it, gold(), g);
  multi::treasures[multi::cpid]++;
  }

string itemcounter(int qty) {
  string s = ""; s += " ("; s += its(qty); s += ")"; return s;
  }

void gainShard(cell *c2, const char *msg) {
  invismove = false;
  string s = XLAT(msg);
  if(c2->land == laMirror) {
     gainItem(itShard);
     s += itemcounter(items[itShard]);
     collectMessage(c2, itShard);
     }
  addMessage(s);
  c2->wall = waNone;
  invismove = false;
  }

void playerMoveEffects(cell *c1, cell *c2) {    

  sword::angle[multi::cpid] = sword::shift(c1, c2, sword::angle[multi::cpid]);
  
  destroyWeakBranch(c1, c2, moPlayer);

  bool nomine = (c2->wall == waMineMine || c2->wall == waMineUnknown)  && markOrb(itOrbAether);
  
  if(!nomine) {
    uncoverMines(c2,
      items[itBombEgg] < 10 && hiitemsMax(itBombEgg) < 25 && !shmup::on ? 0 :
      items[itBombEgg] < 20 ? 1 :
      items[itBombEgg] < 30 ? 2 :
      3, 0
      );
    mayExplodeMine(c2, moPlayer);
    }
  
  if((c2->wall == waClosePlate || c2->wall == waOpenPlate) && !markOrb(itOrbAether))
    toggleGates(c2, c2->wall);

  princess::playernear(c2);

  if(c2->wall == waGlass && items[itOrbAether] > ORBBASE+1) {
    addMessage(XLAT("Your Aether powers are drained by %the1!", c2->wall));
    drainOrb(itOrbAether, 2);
    }
    
  if(c2->wall == waMirror && !markOrb(itOrbAether)) {
    drawParticles(c2, winf[c2->wall].color, 16);
    gainShard(c2, "The mirror shatters!");
    playSound(c2, "pickup-mirror");
    cell *pc = multi::player[multi::cpid].c;
    multi::player[multi::cpid].c = c2;
    mirror::createMirrors(cwt.c, cwt.spin, moMirage);
    multi::player[multi::cpid].c = pc;
    }

  if(c2->wall == waCloud && !markOrb(itOrbAether)) {
    drawParticles(c2, winf[c2->wall].color, 16);
    gainShard(c2, "The cloud turns into a bunch of images!");  
    playSound(c2, "pickup-mirror");
    mirror::createMirages(cwt.c, cwt.spin, moMirage);
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
    waSmallTree || c->wall == waGlass || c->wall == waClosedGate || c->wall == waStone) {
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
  else if(isBull(c->monst)) {
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
      bool opposite = angledist(c, d, c->mondir) >= 3;
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

void moveMonster(cell *ct, cell *cf) {
  eMonster m = cf->monst;                                 
  bool fri = isFriendly(cf);
  if(isDragon(m)) {
    printf("called for Dragon\n");
    return;
    }
  animateMovement(cf, ct, LAYER_SMALL);
  // the following line is necessary because otherwise plates disappear only inside the sight range
  if(cellUnstable(cf) && !ignoresPlates(m)) cf->wall = waChasm;
  moveEffect(ct, cf, m);
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
    if(ct->monst != moTentacleGhost)
      ct->mondir = neighborId(ct, cf);
    }
  ct->hitpoints = cf->hitpoints;
  ct->stuntime = cf->stuntime;
  
  if(fri || isBug(m) || items[itOrbDiscord]) stabbingAttack(cf, ct, m);

  if(isLeader(m)) {
    if(ct->wall == waBigStatue) {
      ct->wall = cf->wall;
      cf->wall = waBigStatue;
      animateMovement(ct, cf, LAYER_BOAT);
      }

    moveBoatIfUsingOne(ct, cf);
    }
  
  if(isTroll(m)) { makeTrollFootprints(ct); makeTrollFootprints(cf); }
    
  if(m == moEarthElemental) {
    if(!passable(ct, cf, 0)) earthFloor(ct);
    earthMove(cf, neighborId(cf, ct));
    }

  if(m == moWaterElemental) {
    placeWater(ct, cf);
    for(int i=0; i<ct->type; i++) {
      cell *c2 = ct->mov[i];
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
        c2->wall = waNone;
        }
      if(shmup::on && isWatery(c2)) shmup::destroyBoats(c2);
      }
    }

  if(m == moGreaterShark) for(int i=0; i<ct->type; i++) {
    cell *c3 = ct->mov[i];
    if(c3 && c3->wall == waBoat)
      makeflame(c3, 5, false);
    }

  // lancers pierce our friends :(
  if(m == moLancer) { 
    // printf("lancer stab?\n");
    for(int u=2; u<=ct->type-2; u++) {
      cell *c3 = ct->mov[(ct->mondir+u)%ct->type]; 
      if(canAttack(ct, moLancer, c3, c3->monst, AF_LANCE | AF_GETPLAYER)) {
        attackMonster(c3, AF_LANCE | AF_ORSTUN | AF_MSG | AF_GETPLAYER, m);
        }
      }
    }
  
  if(m == moWitchFire) makeflame(cf, 10, false);
  if(m == moFireElemental) { makeflame(cf, 20, false); if(cf->wparam < 20) cf->wparam = 20; }
  
  bool adj = false;  
  if(ct->cpdist == 1 && (items[itOrb37] || !nonAdjacent(cf,ct)) && markOrb(itOrbBeauty))
    adj = true;
    
  if(!adj && items[itOrbEmpathy] && items[itOrbBeauty]) {
    for(int i=0; i<ct->type; i++) if(ct->mov[i] && isFriendly(ct->mov[i]))
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

  int inc = incline(cf, ct);
  if(inc == -3 && ct->monst == moReptile)
    ct->stuntime =3;
  else if(inc == 2 && ct->monst == moReptile)
    ct->stuntime = 2;
  else if(inc == 3 && ct->monst == moReptile)
    ct->stuntime = 3;
  else if(inc == -3 && !survivesFall(ct->monst)) {
    addMessage(XLAT("%The1 falls!", ct->monst));
    fallMonster(ct);
    }
  if(isThorny(ct->wall) && !survivesThorns(ct->monst)) {
    addMessage(XLAT("%The1 is killed by thorns!", ct->monst));
    playSound(ct, "hit-rose");
    if(isBull(ct->monst)) ct->wall = waNone;
    fallMonster(ct);
    }
  if(sword::at(ct) && canAttack(NULL, moPlayer, ct, m, AF_SWORD_INTO)) {
    attackMonster(ct, AF_SWORD_INTO | AF_ORSTUN | AF_MSG, moPlayer);
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
  for(int i=-1; i<size(targets); i++)
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

// move value
int moveval(cell *c1, cell *c2, int d, int mf) {
  if(!c2) return -5000;
  
  eMonster m = c1->monst;

  // Angry Beasts can only go forward
  if(m == moRagingBull && c1->mondir != NODIR && angledist(c1, c1->mondir, d) < 3) return -1700;

  // never move against a rose
  if(againstRose(c1, c2) && !ignoresSmell(m)) return -1600;

  // worms cannot attack if they cannot move
  if(isWorm(m) && !passable_for(c1->monst, c2, c1, P_MONSTER)) return -1700;
  
  if(canAttack(c1, m, c2, c2->monst, AF_GETPLAYER | mf) && !(mf & MF_NOATTACKS)) {
    if(mf & MF_MOUNT) {
      if(c2 == dragon::target) return 3000;
      else if(isFriendlyOrBug(c2)) return 500;
      else return 2000;
      }
    if(isPlayerOn(c2)) return 2500;
    else if(isFriendlyOrBug(c2)) return 2000;
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
    for(int u=2; u<=c2->type-2; u++) {
      cell *c3 = c2->mov[(c1->spn(d)+u)%c2->type]; 
      if(c3 && canAttack(c2, moLancer, c3, c3->monst, AF_LANCE | AF_ONLY_ENEMY))
        lancerok = false;
      }
    if(!lancerok) return 750;
    }

  if(m == moWolf) {
    int val = 1500;
    if(heat::absheat(c2) <= heat::absheat(c1))
      return 900;
    for(int i=0; i<c1->type; i++) {
      cell *c3 = c1->mov[i];
      if(heat::absheat(c3) > heat::absheat(c2))
        val--;
      }
    return val;
    }
  
  if((mf & MF_MOUNT) && dragon::target) 
    return 1500 + celldistance(c1, dragon::target) - celldistance(c2, dragon::target);

  // Goblins avoid getting near the Sword
  if(m == moGoblin && sword::near(c2)) return 790;
  if(m == moBat && batsAfraid(c2)) return 790;
  
  if(m == moButterfly)
    return 1500 + angledistButterfly(c1->type, c1->mondir, d);
  
  if(m == moRagingBull && c1->mondir != NODIR)
    return 1500 - bulldist(c2);
  
  if((mf & MF_PATHDIST) && c2->pathdist < c1->pathdist) return 1500; // good move
  
  // prefer straight direction when wandering
  int dd = angledist(c1, c1->mondir, d);
    
  // goblins blocked by anglophobia prefer to move around than to stay
  if(m == moGoblin) {
    bool swn = false;
    forCellEx(c3, c1) if(sword::near(c3)) swn = true;
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
  if(c->monst == moGoblin && sword::near(c)) return 780;
  // Vikings move in a roughly straight line even if they cannot detect you
  if(c->monst == moViking && c->wall == waBoat)
    return 750;
  if(isWorm(c->monst)) return 550;
  if(c->monst == moRagingBull) return -1690; // worse than to stay in place
  if(c->monst == moBat && batsAfraid(c)) return 575;
  return 1000;
  }

int pickMoveDirection(cell *c, flagtype mf) {
  int posdir[10], nc = 0, bestval = stayval(c, mf);

  // printf("stayval [%p, %s]: %d\n", c, dnameof(c->monst), bestval);
  for(int d=0; d<c->type; d++) {
    cell *c2 = c->mov[d];
    int val = moveval(c, c2, d, mf);
    // printf("[%d] %p: val=%5d pass=%d\n", d, c2, val, passable(c2,c,0));
    if(val > bestval) nc = 0, bestval = val;
    if(val == bestval) posdir[nc++] = d;
    }
  
  if(c->monst == moRagingBull) {
    // determinize the Angry Beast movement:
    // use the previous PC's positions as the tiebreaker
    for(int k=0; k<SHSIZE && nc>1; k++) {
      int pts[10];
      for(int d=0; d<nc; d++) pts[d] = 0;
      for(int d=0; d<nc; d++)
      for(int p=0; p<numplayers(); p++) {
        cell *c2  = shpos[p][(cshpos+SHSIZE-k-1)%SHSIZE];
        if(c2) pts[d] += bulldistance(c->mov[posdir[d]], c2);
        }
      int bestpts = 1000;
      for(int d=0; d<nc; d++)
        if(pts[d] < bestpts) bestpts = pts[d];
      int nc0 = 0;
      for(int d=0; d<nc; d++) if(pts[d] == bestpts) posdir[nc0++] = posdir[d];
      nc = nc0;
      }
    }

  if(!nc) return -1;
  nc = hrand(nc);
  return posdir[nc];
  }

int pickDownDirection(cell *c, flagtype mf) {
  int downs[10], qdowns = 0;
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

// Angry Beast attack
// note: this is done both before and after movement
void beastAttack(cell *c, bool player) {
  if(c->mondir == NODIR) return;
  forCellIdEx(c2, d, c) {
    bool opposite = angledist(c, d, c->mondir) >= 3;
    int flags = AF_BULL | AF_ORSTUN;
    if(player) flags |= AF_GETPLAYER;
    if(!opposite) flags |= AF_ONLY_FBUG;
    if(canAttack(c, moRagingBull, c2, c2->monst, flags))
      attackMonster(c2, flags | AF_MSG, moRagingBull);
    }
  }

cell *moveNormal(cell *c, flagtype mf) {
  eMonster m = c->monst;
  
  int d;
  
  if(c->stuntime) {
    if(cellEdgeUnstable(c, MF_STUNNED)) d = pickDownDirection(c, mf);
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
  cell *c2 = c->mov[d];

  if(isPlayerOn(c2)) {
    killThePlayerAt(m, c2, 0);
    return c2;
    }

  eMonster m2 = c2->monst;
  if(m2) {
    attackMonster(c2, AF_ORSTUN | AF_MSG, m);
    if(m == moFlailer && m2 == moIllusion) 
      attackMonster(c, 0, m2);
    return c2;
    }
  
  moveMonster(c2, c);
  if(m == moRagingBull) beastAttack(c2, false);
  return c2;
  }

void explodeAround(cell *c) {
  for(int j=0; j<c->type; j++) {
    cell* c2 = c->mov[j];
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
      if(c2->wall == waGargoyleBridge) placeWater(c2, c2);
      if(c2->wall == waRubble) c2->wall = waNone;
      if(c2->wall == waPlatform) c2->wall = waNone;
      if(c2->wall == waStone) c2->wall = waNone;
      if(c2->wall == waRose) c2->wall = waNone;
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

void killThePlayer(eMonster m, int id, flagtype flags) {
  if(markOrb(itOrbShield)) return;
  if(shmup::on) {
    multi::cpid = id;
    shmup::killThePlayer(m);
    }
  else if(items[itOrbDomination] && playerpos(id)->monst) {
    addMessage(XLAT("%The1 tries to dismount you!", m));
    attackMonster(playerpos(id), AF_ORSTUN, m);
    useupOrb(itOrbDomination, items[itOrbDomination]/2);
    }
  else if(items[itOrbShell] && !(flags & AF_EAT)) {
    addMessage(XLAT("%The1 attacks your shell!", m));
    useupOrb(itOrbShell, 10);
    if(items[itOrbShell] < 1) items[itOrbShell] = 1, orbused[itOrbShell] = true;
    }
  else if(hardcore) {
    addMessage(XLAT("You are killed by %the1!", m));
    if(multi::players > 1 && multi::activePlayers() > 1) 
      multi::leaveGame(id);
    else {
      canmove = false;
      achievement_final(true);
      msgscroll = 0;
      }
    }
  else if(m == moLightningBolt && lastmovetype == lmAttack && isAlchAny(playerpos(id))) {
    addMessage(XLAT("You are killed by %the1!", m));
    addMessage(XLAT("Don't play with slime and electricity next time, okay?"));
    kills[moPlayer]++;
    items[itOrbSafety] = 0;
    }
  else {
    printf("confused!\n");
    addMessage(XLAT("%The1 is confused!", m));
    }
  }

void killThePlayerAt(eMonster m, cell *c, flagtype flags) {
  for(int i=0; i<numplayers(); i++) 
    if(playerpos(i) == c) 
      killThePlayer(m, i, flags);
  }

void afterplayermoved() {
  setdist(cwt.c, 0, NULL);
  prairie::treasures();
  if(generatingEquidistant) {
    printf("Warning: generatingEquidistant set to true\n");
    generatingEquidistant = false;
    }
  }

void mountmove(cell *c, int spin, bool fp, int id) {
  if(multi::players > 1) {
    multi::player[id].c = c;
    multi::player[id].spin = spin;
    multi::flipped[id] = fp;
    }
  else {
    cwt.c = c;
    cwt.spin = spin;
    flipplayer = fp;
    }
  afterplayermoved();
  }

void mountmove(cell *c, int spin, bool fp, cell *ppos) {
  for(int i=0; i<numplayers(); i++) if(playerpos(i) == ppos) {
    animateMovement(ppos, c, LAYER_SMALL);
    mountmove(c, spin, fp, i);
    }
  }

void moveWorm(cell *c) {

  bool mounted = false;
  for(int i=0; i<numplayers(); i++)
    if(multi::playerActive(i) && getMount(i) && sameMonster(c, playerpos(i))) 
      mounted = true;
  
  if(c->monst == moWormwait) { c->monst = moWorm; return; }
  else if(c->monst == moTentaclewait) { c->monst = moTentacle; return; }
  else if(c->monst == moTentacleEscaping) {    
    // explodeAround(c);
    forCellEx(c2, c)
      if(canAttack(c, c->monst, c2, c2->monst, mounted ? AF_ONLY_ENEMY : (AF_GETPLAYER | AF_ONLY_FBUG))) {
        attackMonster(c2, AF_ORSTUN | AF_MSG | AF_GETPLAYER, c->monst);
        }
    cell *c2 = c;
    vector<cell*> allcells;
    while(c2->mondir != NODIR) {
      allcells.push_back(c2);
      c2 = c2->mov[c2->mondir];
      }
    allcells.push_back(c2);
    for(int i=size(allcells)-2; i>=0; i--) {
      cell *cmt = allcells[i+1];
      cell *cft = allcells[i];
      if(cft->monst != moTentacleGhost && cmt->monst != moTentacleGhost)
        mountmove(cmt, cft->spn(cft->mondir), false, cft);
      animateMovement(cft, cmt, LAYER_BIG);
      }
    c->monst = moNone;
    if(c->mondir != NODIR) c->mov[c->mondir]->monst = moTentacleEscaping;
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
      if(c->mondir != NODIR) c = c->mov[c->mondir];
      }
    if(!id) {
      if(spiceSeen)
        addMessage(XLAT("The sandworm explodes in a cloud of Spice!"));
      else
        addMessage(XLAT("The sandworm explodes!"));
      playSound(NULL, "explosion");
      achievement_gain("ZEBRAWORM", 'q');
      }
    return;
    }
  
  cell* goal = c->mov[dir];

  if(isPlayerOn(goal) || goal->monst) 
    attackMonster(goal, AF_EAT | AF_MSG | AF_GETPLAYER, c->monst);
  
  for(int j=0; j<c->type; j++) if(c->mov[j] == goal) {
    goal->monst = eMonster(moWormwait + id);
    moveEffect(goal, NULL, eMonster(moWormwait + id));
      
    animateMovement(c, goal, LAYER_BIG);
    c->monst = eMonster(moWormtail + id);
    goal->mondir = c->spn(j);
  
    mountmove(goal, goal->mondir, true, c);
    
    if(id) {
      cell *c2 = c, *c3 = c2;
      while(c2->monst == moTentacletail || c2->monst == moTentacleGhost) {
        if(c2->mondir == NODIR) {
          drawParticles(c2, (linf[c2->land].color & 0xF0F0F0), 16, 50);
          return;
          }
        c3 = c2, c2 = c3->mov[c2->mondir];
        if(c3->monst != moTentacleGhost && c2->monst != moTentacleGhost) 
          mountmove(c3, c3->mondir, true, c2);
        animateMovement(c2, c3, LAYER_BIG);
        }
      }
    
    cell *c2 = c, *c3 = c2;
    for(int a=0; a<WORMLENGTH; a++) {
      if(c2->monst == moWormtail) {
        if(c2->mondir == NODIR) {
          drawParticles(c2, (linf[c2->land].color & 0xF0F0F0), 16);
          return;
          }
        c3 = c2, c2 = c3->mov[c2->mondir];
        mountmove(c3, c3->mondir, true, c2);
        animateMovement(c2, c3, LAYER_BIG);
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
    if(c2->mirror(c2->mondir)) cw.mirrored = !cw.mirrored;
    c2 = c2->mov[c2->mondir];
    }
  
  cw.c->monst = moIvyWait;
  bool findleaf = false;
  while(true) {
    cwspin(cw, 1);
    if(cw.spin == signed(cw.c->mondir)) {
      if(findleaf) { 
        cw.c->monst = moIvyHead; break;
        }
      cw.c->monst = moIvyWait;
      cwstep(cw);
      continue;
      }
    cwstep(cw);
    if(cw.c->monst == moIvyWait && signed(cw.c->mondir) == cw.spin) {
      cw.c->monst = moIvyBranch;
      findleaf = true; continue;
      }
    cwstep(cw);
    }
  }

// this removes Ivy, but also potentially causes Vines to grow
void removeIvy(cell *c) {
  eMonster m = c->monst;
  c->monst = moNone; // NEWYEARFIX
  for(int i=0; i<c->type; i++)
  // note that semi-vines don't count
    if(c->mov[i]->wall == waVinePlant) {
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
  if(size(ivies) == 0) return;
  computePathdist(moIvyRoot);
  for(int i=0; i<size(ivies); i++) {
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
        if(c->mov[j] && canAttack(c, c->monst, c->mov[j], c->mov[j]->monst, AF_ONLY_FRIEND | AF_GETPLAYER)) {
          if(isPlayerOn(c->mov[j]))
            killThePlayerAt(c->monst, c->mov[j], 0);
          else {
            if(attackJustStuns(c->mov[j]))
              addMessage(XLAT("The ivy attacks %the1!", c->mov[j]->monst));
            else if(isNonliving(c->mov[j]->monst))
              addMessage(XLAT("The ivy destroys %the1!", c->mov[j]->monst));
            else
              addMessage(XLAT("The ivy kills %the1!", c->mov[j]->monst));
            attackMonster(c->mov[j], AF_ORSTUN, c->monst);
            }
          continue;
          }
        if(c->mov[j] && signed(c->mov[j]->pathdist) < pd && passable(c->mov[j], c, 0)
          && !strictlyAgainstGravity(c->mov[j], c, false, MF_IVY))
          mto = c->mov[j], pd = mto->pathdist, sp = c->spn(j);
        }
      c = c->mov[c->mondir];
      }

    if(mto && mto->cpdist) {
      animateMovement(mto->mov[sp], mto, LAYER_BIG);
      mto->monst = moIvyWait, mto->mondir = sp;
      moveEffect(mto, NULL, moIvyWait);
      // if this is the only branch, we want to move the head immediately to mto instead
      if(mto->mov[mto->mondir]->monst == moIvyHead) {
        mto->monst = moIvyHead; co->monst = moIvyBranch;
        }
      }
    else if(co->mov[co->mondir]->monst != moIvyRoot) {
      // shrink useless branches, but do not remove them completely (at the root)
      if(co->monst == moIvyHead) co->mov[co->mondir]->monst = moIvyHead;
      removeIvy(co);
      }
    }
  }

bool earthMove(cell *from, int dir) {
  bool b = false;
  cell *c2 = from->mov[dir];
  int d = from->spn(dir);
  b |= earthWall(from);
  if(c2) for(int u=2; u<=c2->type-2; u++) {
    cell *c3 = c2->mov[(d + u)% c2->type];
    if(c3) b |= earthFloor(c3);
    }
  return b;
  }

vector<cell*> gendfs;

bool isMounted(cell *c) {
  for(int i=0; i<numplayers(); i++) 
    if(multi::playerActive(i) && sameMonster(c, playerpos(i))) 
      return true;
  return false;
  }

int targetcount;

bool isTargetOrAdjacent(cell *c) {
  for(int i=0; i<targetcount; i++)
    if(gendfs[i] == c || isNeighbor(gendfs[i], c)) 
      return true;
  return false;
  }

void groupmove2(cell *c, cell *from, int d, eMonster movtype, flagtype mf) {
  if(!c) return;

  if(eq(c->aitmp, sval)) return;  

  if(movtype == moKrakenH && isTargetOrAdjacent(from)) ;
/*  else if(passable_for(movtype, from, c, P_ONPLAYER | P_CHAIN | P_MONSTER)) ;
  else if(canAttack(c, movtype, from, from->monst, AF_GETPLAYER)) ; */
  else if(passable_for(movtype, from, c, P_CHAIN | P_MONSTER)) ;
  else if(canAttack(c, movtype, from, from->monst, AF_GETPLAYER)) ;
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
    if((mf & MF_ONLYEAGLE) && c->monst != moEagle && c->monst != moBat) return;
    // in the gravity lands, eagles cannot ascend in their second move
    if((mf & MF_ONLYEAGLE) && gravityLevel(c) < gravityLevel(from)) {
      c->aitmp = sval; return;
      }
    if((mf & MF_NOFRIEND) && isFriendly(c)) return;
    if((mf & MF_MOUNT) && !isMounted(c)) return;
    if(isRatling(c->monst) && lastmovetype == lmSkip) return;

    if(c->stuntime) return;
    if(c->monst == moBat && batsAfraid(from)) return;
    
    // note: move from 'c' to 'from'!
    if(!(mf & MF_NOATTACKS)) for(int j=0; j<c->type; j++) 
      if(c->mov[j] && canAttack(c, c->monst, c->mov[j], c->mov[j]->monst, af)) {
        attackMonster(c->mov[j], AF_ORSTUN | AF_GETPLAYER | AF_MSG, c->monst);
        c->aitmp = sval;
        // XLATC eagle
        return;
        }
    
    if(from->cpdist == 0 || from->monst) { c->aitmp = sval; return; }
    
    if(movtype == moDragonHead) {
      dragon::move(from, c);
      return;
      }
    
    moveMonster(from, c);
    from->aitmp = sval;
    }
  c->aitmp = sval;
  // MAXGCELL
  if(size(gendfs) < 1000 || c->cpdist <= 6) gendfs.push_back(c);
  }

void groupmove(eMonster movtype, flagtype mf) {
  sval++;
  gendfs.clear();
  
  if(mf & MF_MOUNT) {
    if(dragon::target) gendfs.push_back(dragon::target);
    if(movtype == moDragonHead) {
      for(int i=0; i<size(dcal); i++) {
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
    for(int i=0; i<size(targets); i++) gendfs.push_back(targets[i]);
  
    if(invisfish && (movtype == moSlime || movtype == moShark || movtype == moKrakenH)) for(int i=0; i<numplayers(); i++) {
      cell *c = playerpos(i);
      if(!c) continue;
      gendfs.push_back(c);
      }
    }
  
  targetcount = size(gendfs);
  
  for(int i=0; i<size(gendfs); i++) {
    cell *c = gendfs[i];
    int dirtable[10], qdirtable=0;
    
    forCellIdAll(c2,t,c) dirtable[qdirtable++] = t;
    random_shuffle(dirtable, dirtable + qdirtable);
    while(qdirtable--) {
      int t = dirtable[qdirtable];
      groupmove2(c->mov[t],c,t,movtype,mf);
      }
      
    if(movtype == moEagle && c->monst == moNone && !isPlayerOn(c)) {
      cell *c2 = whirlwind::jumpFromWhereTo(c, false);
      groupmove2(c2, c, NODIR, movtype, mf);
      }
    }

  if(movtype != moDragonHead) for(int i=0; i<size(dcal); i++) {
    cell *c = dcal[i];
    if((mf & MF_ONLYEAGLE) && c->monst != moEagle && c->monst != moBat) return;
    if(movegroup(c->monst) == movtype && !eq(c->aitmp, sval)) {
      cell *c2 = moveNormal(c, mf);
      if(c2) c2->aitmp = sval;
      }
    }
  }

// Hex monsters

vector<cell*> hexdfs;

// note: move from 'c' to 'from'!
void moveHexSnake(cell *from, cell *c, int d, bool mounted) {
  if(from->wall == waBoat) from->wall = waSea;
  moveEffect(from, c, c->monst);
  from->monst = c->monst; from->mondir = d;
  c->monst = moHexSnakeTail;
  preventbarriers(from);
  
  animateMovement(c, from, LAYER_BIG);
  mountmove(from, from->mondir, true, c);
  
  cell *c2 = c, *c3=c2;
  for(int a=0;; a++) if(c2->monst == moHexSnakeTail) {
    if(a == ROCKSNAKELENGTH) { c2->monst = moNone, c3->mondir = NODIR; break; }
    if(c2->mondir == NODIR) break;
    mountmove(c2, c2->mondir, true, c2->mov[c2->mondir]);
    animateMovement(c2->mov[c2->mondir], c2, LAYER_BIG);
    c3 = c2, c2 = c3->mov[c2->mondir];
    }
    else break;
  }

void snakeAttack(cell *c, bool mounted) {
  for(int j=0; j<c->type; j++) 
    if(c->mov[j] && canAttack(c, moHexSnake, c->mov[j], c->mov[j]->monst, 
      mounted ? AF_ONLY_ENEMY : (AF_ONLY_FBUG | AF_GETPLAYER))) {
        
        attackMonster(c->mov[j], AF_ORSTUN | AF_GETPLAYER | AF_MSG, moHexSnake);
        }
  }

bool goodmount(cell *c, bool mounted) {
  if(mounted) return isMounted(c);
  else return !isMounted(c);
  }

// note: move from 'c' to 'from'!
void hexvisit(cell *c, cell *from, int d, bool mounted) {
  if(!c) return;
  if(cellUnstable(c) || cellEdgeUnstable(c)) return;
  if(eq(c->aitmp, sval)) return;
  
  if(cellUnstableOrChasm(c) || cellUnstableOrChasm(from)) return;
  
  /* if(c->monst == moHexSnake)
    printf("%p:%p %s %d\n", from, c, dnameof(from->monst), passable(from, c, true, false, false)); */

  if(from->cpdist && (!passable(from, c, P_MONSTER|P_WIND|P_FISH))) return;
  
  if(c->monst == moHexSnake) {
    // printf("got snake\n");
    
    if(pseudohept(from)) return;
    if(!goodmount(c, mounted)) return;
    
    if(canAttack(c, moHexSnake, from, from->monst, AF_EAT | (mounted ? AF_ONLY_ENEMY : AF_ONLY_FBUG | AF_GETPLAYER))) {
      attackMonster(from, AF_MSG | AF_EAT | AF_GETPLAYER, c->monst);
      }
    
    if(from->cpdist == 0 || from->monst) return;

    snakeAttack(c, mounted);
    moveHexSnake(from, c, d, mounted);
    }

  c->aitmp = sval;

  // MAXGCELL
  if(size(hexdfs) < 2000 || c->cpdist <= 6) 
    hexdfs.push_back(c);
  }

void movehex(bool mounted) {
  sval++;
  hexdfs.clear();
  
  if(mounted) { 
    if(dragon::target && dragon::target->monst != moHexSnake) {
      hexdfs.push_back(dragon::target); 
      dragon::target->aitmp = sval;
      }
    }
  else for(int i=0; i<size(targets); i++) {
    hexdfs.push_back(targets[i]);
    targets[i]->aitmp = sval;
    }
  //hexdfs.push_back(cwt.c);
  
  for(int i=0; i<size(hexdfs); i++) {
    cell *c = hexdfs[i];
    int dirtable[10], qdirtable=0;
    for(int t=0; t<c->type; t++) if(c->mov[t] && !pseudohept(c->mov[t]))
      dirtable[qdirtable++] = t;
      
    random_shuffle(dirtable, dirtable+qdirtable);
    while(qdirtable--) {
      int t = dirtable[qdirtable];
      hexvisit(c->mov[t], c, t, mounted);
      }
    }
  for(int i=0; i<size(hexsnakes); i++) {
    cell *c = hexsnakes[i];
    if(c->monst == moHexSnake) {
      if(!goodmount(c, mounted)) continue;
      int t[7];
      for(int i=0; i<c->type; i++) t[i] = i;
      for(int j=1; j<c->type; j++) swap(t[j], t[hrand(j+1)]);
      for(int u=0; u<c->type; u++) {
        createMov(c, t[u]);
        if(!pseudohept(c->mov[t[u]]))
          hexvisit(c, c->mov[t[u]], c->spn(t[u]), mounted);
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
        c2 = c2->mov[c2->mondir];
        }
      }
    }
  }

void movemutant() {
  vector<cell*> young;
  for(int i=0; i<size(dcal); i++) 
    if(dcal[i]->monst == moMutant && dcal[i]->stuntime == mutantphase)
      young.push_back(dcal[i]);
  
  for(int j=1; j<size(young); j++)
    swap(young[j], young[hrand(j+1)]);

  mutantphase++;
  mutantphase &= 15;
  
  for(int i=0; i<size(young); i++) {
    cell *c = young[i];
    if(clearing::buggyplant) {  if(c->monst == moMutant) c->monst=moNone; continue; }
    for(int j=0; j<c->type; j++) {
      cell *c2 = c->mov[j];
      if(!c2) continue;

      if(c2->monst != moMutant && canAttack(c, moMutant, c2, c2->monst, AF_ONLY_FBUG | AF_GETPLAYER)) {
        attackMonster(c2, AF_ORSTUN | AF_MSG | AF_GETPLAYER, moMutant);
        continue;
        }
      
      if(isPlayerOn(c2)) continue;

      if((c2->land == laOvergrown || !pseudohept(c2)) && passable(c2, c, 0)) {
        if(c2->land == laClearing && c2->mpdist > 7) continue;
        c2->monst = moMutant;
        c2->mondir = c->spn(j);
        c2->stuntime = mutantphase;
        animateMovement(c, c2, LAYER_BIG);
        }
      }
    }  
  }

cell *shpos[MAXPLAYER][SHSIZE];
int cshpos = 0;

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
        if(c->mov[j] && canAttack(c, moShadow, c->mov[j], c->mov[j]->monst, AF_ONLY_FBUG | AF_GETPLAYER))
          attackMonster(c->mov[j], AF_ORSTUN | AF_MSG | AF_GETPLAYER, c->monst);
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
      if(shfrom) animateMovement(shfrom, where, LAYER_SMALL);
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

  for(int i=0; i<size(ghosts); i++) {
    cell *c = ghosts[i];
    
    if(c->stuntime) return;

    if(isGhostMover(c->monst)) {
      int goodmoves = 0;

      for(int k=0; k<c->type; k++) if(c->mov[k] && c->mov[k]->cpdist < c->cpdist)
        if(ghostmove(c->monst, c->mov[k], c) && !isPlayerOn(c->mov[k]))
          goodmoves++;
      
      movesofgood[goodmoves].push_back(c);
      }
    }
  
  for(int d=0; d<8; d++) for(int i=0; i<size(movesofgood[d]); i++) {
    cell *c = movesofgood[d][i];
    if(c->stuntime) return;
    
    if(isGhostMover(c->monst) && c->cpdist >= 1) {
      
      int mdir[7];

      for(int j=0; j<c->type; j++) 
        if(c->mov[j] && canAttack(c, c->monst, c->mov[j], c->mov[j]->monst, AF_GETPLAYER | AF_ONLY_FBUG)) {
          // XLATC ghost/greater shark
          
          attackMonster(c->mov[j], AF_ORSTUN | AF_MSG | AF_GETPLAYER, c->monst);
          goto nextghost;
          }
    
      int qmpos = 0;
      for(int k=0; k<c->type; k++) if(c->mov[k] && c->mov[k]->cpdist < c->cpdist)
        if(ghostmove(c->monst, c->mov[k], c))
          mdir[qmpos++] = k;
      if(!qmpos) continue;
      int d = mdir[hrand(qmpos)];
      cell *c2 = c->mov[d];
      if(c2->monst == moTortoise && c2->stuntime > 1) {
        addMessage(XLAT("%The1 scares %the2 a bit!", c->monst, c2->monst));
        c2->stuntime = 1;
        }
      else moveMonster(c2, c);
      
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
    thruVine(mt, c)) {
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
  if(canAttack(mt, who, c, m, AF_SWORD)) {
    markOrb(bb ? itOrbSword2: itOrbSword);
    int k = tkills();
    attackMonster(c, AF_ORSTUN | AF_MSG | AF_SWORD, who);
    if(c->monst == moShadow) c->monst = moNone;
    produceGhost(c, m, who);
    if(tkills() > k) return true;
    }
  return false;
  }

void swordAttackStatic() {
  for(int bb = 0; bb < 2; bb++) 
    if(sword::orbcount(bb))
      swordAttack(cwt.c, moPlayer, sword::pos(multi::cpid, bb), bb);
  }

void stabbingAttack(cell *mf, cell *mt, eMonster who, int bonuskill) {
  int numsh = 0, numflail = 0, numlance = 0, numslash = 0;
  
  int backdir = neighborId(mt, mf);
  
  for(int bb=0; bb<2; bb++) if(who == moPlayer && sword::orbcount(bb)) {
    int numbb = 0;
    cell *sf = sword::pos(mf, sword::angle[multi::cpid] + (bb?S21:0));
    cell *st = sword::pos(mt, sword::shift(mf, mt, sword::angle[multi::cpid]) + (bb?S21:0));
    if(swordAttack(mt, who, st, bb)) numbb++;
    if(sf != st && !isNeighbor(sf,st)) {
      // also attack the in-transit cell
      forCellEx(sb, sf) if(isNeighbor(sb, st) && sb != mf && sb != mt)
        if(swordAttack(mt, who, sb, bb)) numbb++;
      }
    achievement_count("SLASH", numbb, 0);
    numslash += numbb;
    }
  
  for(int t=0; t<mf->type; t++) {
    cell *c = mf->mov[t];
    if(!c) continue;
    
    bool stabthere = false, away = true;
    for(int u=0; u<c->type; u++) if(c->mov[u] == mt) stabthere = true, away = false;
    
    if(stabthere && canAttack(mt,who,c,c->monst,AF_STAB)) {
      if(c->monst != moHedge) { 
        markOrb(itOrbThorns); if(who != moPlayer) markOrb(itOrbEmpathy);
        }
      eMonster m = c->monst;
      int k = tkills();
      if(attackMonster(c, AF_ORSTUN | AF_STAB | AF_MSG, who)) 
        produceGhost(c, m, who);
      if(tkills() > k) numsh++;
      }

    if(away && c != mt && canAttack(mt,who,c,c->monst,AF_BACK)) {
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

  if(!isUnarmed(who)) for(int t=0; t<mt->type; t++) {
    cell *c = mt->mov[t];
    if(!c) continue;
    eMonster mm = c->monst;
    int flag = AF_APPROACH;
    if(angledist(mt, backdir, t) >= 3) flag |= AF_HORNS;
    if(canAttack(mt,who,c,c->monst, flag)) {
      if(attackMonster(c, flag | AF_MSG, who)) numlance++;
      produceGhost(c, mm, who);
      }
    }

  if(numsh) achievement_count("STAB", numsh, 0);
  
  if(numlance && numflail && numsh) achievement_gain("MELEE3");

  if(numlance + numflail + numsh + numslash + bonuskill >= 5) achievement_gain("MELEE5");

  if(numsh == 2) {
    if(lastdouble == turncount-1) achievement_count("STAB", 4, 0);
    lastdouble = turncount;
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
     c2->monst == moReptile || c2->monst == moTortoise) ? -400 :
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
  if(!euclid && c->monst == moKnight && c2->master->alt) {
    val -= celldistAlt(c2);
    // don't go to external towers
    if(c2->wall == waTower && c2->wparam == 1 && !c2->monst)
      return 60;
    }
  return val;
  }

#define STRONGWIND 99

void movegolems(flagtype flags) {
  computePathdist(moMouse);
  int qg = 0;
  for(int i=0; i<size(golems); i++) {
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
      int bestv = stayvalue(m, c), bq = 0, bdirs[7];

      DEBT("moveval");
      for(int k=0; k<c->type; k++) if(c->mov[k]) {
        cell *c2 = c->mov[k];
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
      cell *c2 = dir != STRONGWIND ? c->mov[dir] : whirlwind::jumpDestination(c);
      if(c2->monst) {
        bool revenge = (m == moPrincess);
        bool jealous = (isPrincess(c->monst) && isPrincess(c2->monst));
        eMonster m2 = c2->monst;
        if(revenge) {
          playSound(c2, princessgender() ? "dzia-princess" : "dzia-prince");
          addMessage(XLAT("%The1 takes %his1 revenge on %the2!", m, c2->monst));
          }
        attackMonster(c2, ((revenge||jealous)?0:AF_ORSTUN) | AF_MSG, m);
        produceGhost(c2, m2, m);
        if(revenge) c->monst = m = moPrincessArmed;
        if(jealous) {
          playSound(c2, princessgender() ? "dzia-princess" : "dzia-prince");
          addMessage("\"That should teach you to take me seriously!\"");
          }
        }
      else {
        passable_for(m, c2, c, P_DEADLY);
        DEBT("move");
        moveMonster(c2, c);
        if(m != moTameBomberbird && m != moFriendlyGhost) 
          moveBoatIfUsingOne(c2, c);
          
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
  for(int i=0; i<size(butterflies); i++) {
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
  for(int i=0; i<size(dcal); i++) {
    cell *c = dcal[i];
    
    if(c->stuntime) continue;
    
    eMonster m = c->monst;
    
    if(m == moSleepBull) {
      bool wakeup = false;
      forCellEx(c2, c) if(c2->monst == moGadfly) {
        addMessage(XLAT("%The1 wakes up %the2.", c2->monst, m));
        wakeup = true;
        }
      for(int i=0; i<size(targets); i++) {
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
      computePathdist(moNecromancer);
      int gravenum = 0, zombienum = 0;
      cell *gtab[8], *ztab[8];
      for(int j=0; j<c->type; j++) if(c->mov[j]) {
        if(c->mov[j]->wall == waFreshGrave) gtab[gravenum++] = c->mov[j];
        if(passable(c->mov[j], c, 0) && c->mov[j]->pathdist < c->pathdist)
          ztab[zombienum++] = c->mov[j];
        }
      if(gravenum && zombienum) {
        cell *gr = gtab[hrand(gravenum)];
        gr->wall = waAncientGrave;
        gr->monst = moGhost;
        ztab[hrand(zombienum)]->monst = moZombie;
        addMessage(XLAT("%The1 raises some undead!", m));
        playSound(c, "necromancy");
        }
      }
    
    else if(m == moOutlaw && c->pathdist <= GUNRANGE) {
      killThePlayer(m, nearestPathPlayer(c), 0);
      c->stuntime = 1;
      }

    else if(m == moWitchFlash && flashWouldKill(c, AF_GETPLAYER | AF_ONLY_FBUG) && !flashWouldKill(c, false)) {
      addMessage(XLAT("%The1 activates her Flash spell!", m));
      m = moWitch;
      activateFlashFrom(c, moWitchFlash, AF_MAGIC | AF_GETPLAYER | AF_MSG);
      c->stuntime = 1;
      }

    else if(m == moCrystalSage && c->cpdist <= 4 && isIcyLand(cwt.c) && cwt.c->wall != waBoat) {
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
        for(int i=0; i<size(targets); i++) {
          cell *t = targets[i];
          if(celldistance(c, t) > 4) continue;
          sageheat(t, .0);
          for(int i=0; i<t->type; i++)
            sageheat(t->mov[i], .3);
          }
        }
      c->stuntime = 1;
      }
    
    else if(m == moPyroCultist) {
      bool shot = false;
      bool dont_approach = false;
      // smaller range on the sphere
      int firerange = sphere ? 2 : 4;
      for(int i=0; i<size(targets); i++) {
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
      for(int i=0; i<size(targets); i++) {
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
  if(!size(worms)) return;
  computePathdist(moWorm);
  int wrm = size(worms);
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

void moverefresh(bool turn = true) {
  int dcs = size(dcal);
  
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
    
    if(c->stuntime && !isMutantIvy(c)) {
      c->stuntime--;
      int breathrange = sphere ? 2 : 3;
      if(c->stuntime == 0 && c->monst == moDragonHead)  {
        if(!c->hitpoints) c->hitpoints = 1;
        else if(shmup::on && dragon::totalhp(c) > 2 && shmup::dragonbreath(c)) {
          c->hitpoints = 0;
          }
        else if(dragon::totalhp(c) <= 2) ;
        else if(items[itOrbDomination] && isMounted(c)) {
          if(dragon::target && celldistance(c, dragon::target) <= breathrange && makeflame(dragon::target, 5, true)) {
            addMessage(XLAT("%The1 breathes fire!", c->monst));
            makeflame(dragon::target, 5, false);
            playSound(dragon::target, "fire");
            c->hitpoints = 0;
            }
          }
        else {
          for(int i=0; i<size(targets); i++) {
            cell *t = targets[i];
            if(celldistance(c, t) <= breathrange && makeflame(t, 5, true)) {
              if(isPlayerOn(t)) addMessage(XLAT("%The1 breathes fire at you!", c->monst));
              else if(t->monst)
                addMessage(XLAT("%The1 breathes fire at %the2!", c->monst, t->monst));
              else
                addMessage(XLAT("%The1 breathes fire!", c->monst));
              makeflame(t, 5, false);
              playSound(t, "fire");
              c->hitpoints = 0;
              }
            }
          }
        }
      }
    
    // tortoises who have found their children no longer move
    if(c->monst == moTortoise && c->item == itBabyTortoise &&
      !((tortoise::getb(c) ^ tortoise::babymap[c]) & tortoise::mask))
        c->stuntime = 2;

    if(c->monst == moReptile) {
      if(c->wall == waChasm || cellUnstable(c)) {
        c->monst = moNone;
        c->wall = waReptile;
        c->wparam = reptilemax();
        playSound(c, "click");
        }
      else if(isChasmy(c) || isWatery(c)) {
        c->wall = waReptileBridge;
        c->item = itNone;
        c->wparam = reptilemax();
        c->monst = moNone;
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
          playSound(c, "click");
          }
        }
      }

    else if(isFire(c)) {
      if(c->monst && !survivesFire(c->monst)) {
        addMessage(XLAT("%The1 burns!", c->monst));
        if(isBull(c->monst)) {
          addMessage(XLAT("Fire is extinguished!"));
          c->wall = waNone;
          }
        fallMonster(c);
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
    else if(!isWateryOrBoat(c)) {
      if(c->monst == moGreaterShark)
        c->monst = moGreaterM;
      else if(c->monst == moShark || c->monst == moCShark) {
        addMessage(XLAT("%The1 suffocates!", c->monst));
        fallMonster(c);
        }
      else if(c->monst == moKrakenH) {
        addMessage(XLAT("%The1 suffocates!", c->monst));
        kraken::kill(c, moNone);
        }
      }
    
    if(c->monst == moVineSpirit && !cellHalfvine(c) && c->wall != waVinePlant) {
      addMessage(XLAT("%The1 is destroyed!", c->monst));
      fallMonster(c);
      }
    
    if(c->monst) mayExplodeMine(c, c->monst);
    
    if(c->monst && c->wall == waClosedGate && !survivesWall(c->monst)) {
      playSound(c, "hit-crush"+pick123());
      addMessage(XLAT("%The1 is crushed!", c->monst));
      fallMonster(c);
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
      cell *c2 = c->mov[t];
      if(c2 && c2->pathdist < c->pathdist)
        goodmoves++;
      }
    movesofgood[goodmoves].push_back(c);
    }
  else 
    movesofgood[0].push_back(c);
  }

void moveNormals(eMonster param) {
  computePathdist(param);

  for(int d=0; d<8; d++) movesofgood[d].clear();

  for(int i=0; i<size(pathqm); i++) 
    consMove(pathqm[i], param);
  
  int dcs = size(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->pathdist == PINFD) consMove(c, param);
    }

  for(int d=0; d<8; d++) for(int i=0; i<size(movesofgood[d]); i++) {
    cell *c = movesofgood[d][i];
    if(normalMover(c->monst)) {
      moveNormal(c, MF_PATHDIST);
      }
    }
  }

void movemonsters() {

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
  DEBT("leader");
  if(havewhat & HF_LEADER) groupmove(moPirate, 0);
  DEBT("mutant");
  if(havewhat & HF_MUTANT) movemutant();
  DEBT("bugs");
  if(havewhat & HF_BUG) hive::movebugs();
  DEBT("whirlpool");
  if(havewhat & HF_WHIRLPOOL) whirlpool::move();
  DEBT("whirlwind");
  if(havewhat & HF_WHIRLWIND) whirlwind::move();
  DEBT("river");
  if(havewhat & HF_RIVER) prairie::move();

  DEBT("worm");
  cell *savepos[MAXPLAYER];
  
  for(int i=0; i<numplayers(); i++)
    savepos[i] = playerpos(i);

  moveworms();
  if(havewhat & HF_HEX) {
    movehex(false);
    if(haveMount()) movehex(true);
    }

  if(havewhat & HF_KRAKEN) kraken::attacks(), groupmove(moKrakenH, 0);
  if(havewhat & HF_DRAGON) groupmove(moDragonHead, MF_NOFRIEND);
  if(haveMount()) groupmove(moDragonHead, MF_MOUNT);

  DEBT("golems");
  movegolems(0);
  
  DEBT("fresh");
  moverefresh();

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
  if(items[itOrbDomination] > ORBBASE && cwt.c->monst) 
    return false;
  if(cwt.c->monst) {
    if(checkonly) return true;
    if(isMountable(cwt.c->monst))
      addMessage(XLAT("You need to dismount %the1!", cwt.c->monst));
    else
      addMessage(XLAT("You need to move to give space to %the1!", cwt.c->monst));
    }
  else if(cwt.c->wall == waRoundTable) {
    if(markOrb2(itOrbAether)) return false;
    if(checkonly) return true;
    addMessage(XLAT("It would be impolite to land on the table!"));
    }
  else if(cwt.c->wall == waLake) {
    if(markOrb2(itOrbAether)) return false;
    if(markOrb2(itOrbFish)) return false;
    if(checkonly) return true;
    addMessage(XLAT("Ice below you is melting! RUN!"));
    }
  else if(!attacking && cellEdgeUnstable(cwt.c)) {
    if(markOrb2(itOrbAether)) return false;
    if(checkonly) return true;
    addMessage(XLAT("Nothing to stand on here!"));
    }
  else if(cwt.c->wall == waSea || cwt.c->wall == waCamelotMoat) {
    if(markOrb(itOrbFish)) return false;
    if(markOrb2(itOrbAether)) return false;
    if(checkonly) return true;
    addMessage(XLAT("You have to run away from the water!"));
    }
  else if(cwt.c->wall == waClosedGate) {
    if(markOrb2(itOrbAether)) return false;
    if(checkonly) return true;
    addMessage(XLAT("The gate is closing right on you! RUN!"));
    }
  else if(isFire(cwt.c) && !markOrb(itOrbWinter) && !markOrb2(itOrbShield)) {
    if(markOrb2(itOrbAether)) return false;
    if(checkonly) return true;
    addMessage(XLAT("This spot will be burning soon! RUN!"));
    }
  else if(cwt.c->wall == waChasm) {
    if(markOrb2(itOrbAether)) return false;
    if(checkonly) return true;
    addMessage(XLAT("The floor has collapsed! RUN!"));
    }
  else if(items[itOrbAether] > ORBBASE && !passable(cwt.c, NULL, P_ISPLAYER | P_NOAETHER)) {
    if(markOrb2(itOrbAether)) return false;
    return true;
    }
  else if(!passable(cwt.c, NULL, P_ISPLAYER)) {
    if(isFire(cwt.c)) return false; // already checked: have Shield
    if(markOrb2(itOrbAether)) return false;
    if(checkonly) return true;
    addMessage(XLAT("Your Aether power has expired! RUN!"));
    }
  else return false;
  if(hardcore) { 
    canmove = false;
    achievement_final(true);
    msgscroll = 0;
    return false;
    }
  return true;
  }

int countMyGolems(eMonster m) {
  int g=0, dcs = size(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->monst == m) g++;
    }
  return g;
  }

int savePrincesses() {
  int g=0, dcs = size(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(isPrincess(c->monst)) princess::save(c);
    }
  return g;
  }

int countMyGolemsHP(eMonster m) {
  int g=0, dcs = size(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->monst == m) g += c->hitpoints;
    }
  return g;
  }

void restoreGolems(int qty, eMonster m, int hp = 0) {
  int dcs = size(dcal);
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
  movecost(cwt.c, recallCell);
  playerMoveEffects(cwt.c, recallCell);
  mirror::destroy();

  items[itOrbSword] = 0;
  items[itOrbSword2] = 0;

  cwt.c = recallCell; recallCell = NULL;
  cwt.spin = hrand(cwt.c->type); flipplayer = !!(hrand(2));
  fullcenter(); 
  makeEmpty(cwt.c);
  forCellEx(c2, cwt.c) 
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
  if(l == laCamelot && !(tactic::on && tactic::lasttactic == laCamelot)) 
    l = laCrossroads;
  firstland = l;
  safetyland = l;
  safetyseed = time(NULL);
  for(int i=0; i<65536; i++) euland[i] = laNone;
  euland[0] = euland[1] = firstland;
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

bool legalmoves[8];

bool hasSafeOrb(cell *c) {
  return 
    c->item == itOrbSafety ||
    c->item == itOrbShield ||
    c->item == itOrbShell  ||
    c->item == itOrbYendor;
  }

void checkmove() {
  if(multi::players > 1 && !multi::checkonly) return;
  if(hardcore) return;
  msgscroll = 0;
  bool orbusedbak[ittypes];
  
  // do not activate orbs!
  for(int i=0; i<ittypes; i++) orbusedbak[i] = orbused[i];

  for(int i=0; i<8; i++) legalmoves[i] = false;

  canmove = false;
  items[itWarning]+=2;
  if(movepcto(-1, 0, true)) canmove = legalmoves[7] = true;
  
  if(ISMOBILE || !canmove)
    for(int i=0; i<cwt.c->type; i++) 
      if(movepcto(1, -1, true)) 
        canmove = legalmoves[cwt.spin] = true;
  if(ISMOBILE || !canmove)
    for(int i=0; i<cwt.c->type; i++) 
      if(movepcto(1, 1, true)) 
        canmove = legalmoves[cwt.spin] = true;
  if(kills[moPlayer]) canmove = false;
  if(!canmove)
    achievement_final(true);
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
  if(passable(on, moveto, m == moTameBomberbird ? P_FLYING : 0))
    on->monst = m;
  else {
    on->monst = m;
    if(isFire(on))
      addMessage(XLAT("%The1 burns!", m));
    else if(on->wall == waChasm)
      addMessage(XLAT("%The1 falls!", m));
    else if(isWatery(on) && isNonliving(m))
      addMessage(XLAT("%The1 sinks!", m));
    else if(isWatery(on))
      addMessage(XLAT("%The1 drowns!", m));
    else if(isWall(on))
      addMessage(XLAT("%The1 is crushed!", m));
    else if(m == moTameBomberbird && cwt.c->wall == waBoat)
      return;
    else 
      addMessage(XLAT("%The1 is destroyed!", m));
    
    printf("mondir = %d\n", on->mondir);
    fallMonster(cwt.c);
    }
  }

bool multiRevival(cell *on, cell *moveto) {
  int fl = 0;
  if(items[itOrbAether]) fl |= P_AETHER;
  if(items[itOrbFish]) fl |= P_FISH;
  if(items[itOrbWinter]) fl |= P_WINTER;
  if(passable(on, moveto, fl)) {
    int id = multi::revive_queue[0];
    for(int i=1; i<size(multi::revive_queue); i++)
      multi::revive_queue[i-1] = multi::revive_queue[i];
    multi::revive_queue.pop_back();
    multi::player[id].c = on;
    multi::player[id].spin = neighborId(moveto, on);
    if(multi::player[id].spin < 0) multi::player[id].spin = 0;
    multi::flipped[id] = true;
    multi::whereto[id].d = MD_UNDECIDED;
    items[itOrbSword] = items[itOrbSword2] = 0;
    return true;
    }
  return false;
  }

void movecost(cell* from, cell *to) {
  if(from->land == laPower && to->land != laPower) {
    int n=0;
    for(int i=0; i<ittypes; i++)
      if(itemclass(eItem(i)) == IC_ORB && items[i] >= 2 && i != itOrbFire)
        items[i] = 2, n++;
    if(n) 
      addMessage(XLAT("As you leave, your powers are drained!"));
    }
  
  if(from->land != to->land && tour::on)
    tour::checkGoodLand(to->land);
  
  if(to->land ==laCrossroads4 && !chaosUnlocked) {
    achievement_gain("CR4");
    chaosUnlocked = true;
    }

  if(isHaunted(from->land) && !isHaunted(to->land)) {
    updateHi(itLotus, truelotus = items[itLotus]);
    if(items[itLotus] >= 1) achievement_gain("LOTUS1");
    if(items[itLotus] >= 10) achievement_gain("LOTUS2");
    if(items[itLotus] >= 25) achievement_gain("LOTUS3");
    if(items[itLotus] >= 50) achievement_gain("LOTUS4");
    achievement_final(false);
    }
  
  if(celldist(to) == 0 && !usedSafety && gold() >= 100)
    achievement_gain("COMEBACK");
  
  bool tortoiseOK = 
    to->land == from->land || to->land == laTortoise ||
    (to->land == laDragon && from->land != laTortoise) || 
    chaosmode;
  
  if(tortoise::seek() && !from->item && !tortoiseOK && passable(from, NULL, 0)) {
    from->item = itBabyTortoise;
    tortoise::babymap[from] = tortoise::seekbits;
    addMessage(XLAT("You leave %the1.", itBabyTortoise));
    items[itBabyTortoise]--;
    }
  }

bool cantGetGrimoire(cell *c2, bool verbose = true) {
  if(chaosmode) return false;
  if(!euclid && !c2->master->alt) return false;
  if(c2->item == itGrimoire && items[itGrimoire] > celldistAlt(c2)/-TEMPLE_EACH) {
    if(verbose)
      addMessage(XLAT("You already have this Grimoire! Seek new tomes in the inner circles."));
    return true;
    }
  return false;
  }

void gainLife() {
  items[itOrbLife] ++;
  if(items[itOrbLife] > 5) items[itOrbLife] = 5;
  }

void collectMessage(cell *c2, eItem which) {
  bool specialmode = 
    yendor::on || tactic::on || princess::challenge || euclid || sphere;
    
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
  else if(which == itKey)
    addMessage(XLAT("You have found the Key! Now unlock this Orb of Yendor!"));
  else if(which == itGreenStone && !items[itGreenStone])
    addMessage(XLAT("This orb is dead..."));
  else if(which == itGreenStone)
    addMessage(XLAT("Another Dead Orb."));
  else if(itemclass(which) != IC_TREASURE)
    addMessage(XLAT("You have found %the1!", which));
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
  else if(items[which] == 4 && maxgold() == 4 && !specialmode) {
    addMessage(XLAT("You feel that %the2 become%s2 more dangerous.", which, c2->land));
    addMessage(XLAT("With each %1 you collect...", which, c2->land));
    }
  else if(items[which] == 9 && maxgold() == 9 && !specialmode)
    addMessage(XLAT("Are there any magical orbs in %the1?...", c2->land));
  else if(items[which] == 10 && maxgold() == 10 && !specialmode) {
    addMessage(XLAT("You feel that %the1 slowly become%s1 dangerous...", c2->land));
    addMessage(XLAT("Better find some other place."));
    }
  else if(which == itSpice && items[itSpice] == 7 && !specialmode)
    addMessage(XLAT("You have a vision of the future, fighting demons in Hell..."));
  else if(which == itSpice && items[itSpice] == 9 && !specialmode)
    addMessage(XLAT("You will be fighting red rock snakes, too..."));
  else if(which == itKraken && items[itKraken] == 9 && !specialmode)
    addMessage(XLAT("You feel that a magical weapon is waiting for you..."));
//  else if(which == itFeather && items[itFeather] == 10)
//    addMessage(XLAT("There should be a Palace somewhere nearby..."));
  else if(which == itElixir && items[itElixir] == 4 && !specialmode)
    addMessage(XLAT("With this Elixir, your life should be long and prosperous..."));
  else if(which == itRuby && items[itRuby] == 4 && !specialmode) {
    addMessage(XLAT("You feel something strange about gravity here..."));
    }
  else if(which == itPalace && items[itPalace] == 4 && !specialmode) {
    addMessage(XLAT("The rug depicts a man in a deep dungeon, unable to leave."));
    }
  else if(which == itIvory && items[itIvory] == 4 && !specialmode) {
    addMessage(XLAT("You feel attuned to gravity, ready to face mountains and dungeons."));
    }
  else if(which == itBone && items[itBone] == 6 && !specialmode)
    addMessage(XLAT("The Necromancer's Totem contains hellish incantations..."));
  else if(which == itStatue && items[itStatue] == 6 && !specialmode)
    addMessage(XLAT("The inscriptions on the Statue of Cthulhu point you toward your destiny..."));
  else if(which == itStatue && items[itStatue] == 4 && !specialmode)
    addMessage(XLAT("There must be some temples of Cthulhu in R'Lyeh..."));
  else if(which == itDiamond && items[itDiamond] == 8 && !specialmode)
    addMessage(XLAT("Still, even greater treasures lie ahead..."));
  else if(which == itFernFlower && items[itFernFlower] == 4 && !specialmode)
    addMessage(XLAT("You overheard Hedgehog Warriors talking about emeralds..."));
  else if(which == itEmerald && items[itEmerald] == 4 && !specialmode && !chaosmode)
    addMessage(XLAT("You overhear miners talking about a castle..."));
  else if(which == itEmerald && items[itEmerald] == 5 && !specialmode && !chaosmode)
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

bool collectItem(cell *c2, bool telekinesis) {

  int pg = gold();
  bool dopickup = true;
  
  if(itemHidden(c2) && !telekinesis && !(isWatery(c2) && markOrb(itOrbFish)))
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
    }
  
  if(isRevivalOrb(c2->item) && multi::revive_queue.size()) {
    multiRevival(cwt.c, c2);
    }
  else if(c2->item == itOrbSpeed) {
    items[c2->item] += 31;
    playSound(c2, "pickup-speed");
    }
  else if(c2->item == itOrbLife) {
    playSound(c2, "pickup-orb"); // TODO summon
    if(shmup::on) gainLife();
    else placeGolem(cwt.c, c2, moGolem);
    }
  else if(c2->item == itOrbFriend) {
    playSound(c2, "pickup-orb"); // TODO summon
    if(shmup::on) gainLife();
    else placeGolem(cwt.c, c2, moTameBomberbird);
    }
  else if(tour::on && (c2->item == itOrbSafety || c2->item == itOrbRecall)) {
    addMessage(XLAT("This Orb is not compatible with the Tutorial."));
    return true;
    }
  else if(c2->item == itOrbSafety) {
    playSound(c2, "pickup-orb"); // TODO safety
    items[c2->item] += 7;
    if(shmup::on)
      shmup::safety = true;
    else 
      activateSafety(c2->land);
    return true;
    }
  else if(c2->item == itOrbLightning) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbStone) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 61;
    }
  else if(c2->item == itOrbNature) {
    playSound(c2, "pickup-orb");
    if(shmup::on) gainLife();
    else items[c2->item] += 61;
    }
  else if(c2->item == itOrbRecall) {
    saveRecall(c2);
    playSound(c2, "pickup-orb");
    items[c2->item] += 61;
    }
  else if(c2->item == itOrbTime) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbLove) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 31;
    }
  else if(c2->item == itOrbSpace) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbThorns) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbSword) {
    playSound(c2, "pickup-orb");
    if(!items[itOrbSword] && !items[itOrbSword2]) 
      sword::shuffle();
    items[c2->item] += 61 + 30 * multi::activePlayers();
    }
  else if(c2->item == itOrbSword2) {
    playSound(c2, "pickup-orb");
    if(!items[itOrbSword] && !items[itOrbSword2]) 
      sword::shuffle();
    items[c2->item] += 41 + 20 * multi::activePlayers();
    }
  else if(c2->item == itOrbFlash) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbShield) {
    playSound(c2, "pickup-orb"); // TODO Shield
    items[c2->item] += 16;
    orbused[itOrbShield] = false;
    }
  else if(c2->item == itOrbWater) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 31;
    }
  else if(c2->item == itOrbAir) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 67;
    }
  else if(c2->item == itOrbFrog) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 45;
    }
  else if(c2->item == itOrbDash) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 45;
    }
  else if(c2->item == itOrbDiscord) {
    playSound(c2, "pickup-orb");
    // make it seem to be 23
    if(!items[c2->item]) items[c2->item]++;
    items[c2->item] += 23;
    }
  else if(c2->item == itOrbSummon) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 121;
    }
  else if(c2->item == itOrbMatter) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 67;
    }
  else if(c2->item == itOrbHorns) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 67;
    }
  else if(c2->item == itOrbBull) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 67;
    }
  else if(c2->item == itOrbFish) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 21 + 10 * multi::activePlayers();
    }
  else if(c2->item == itOrbWinter) {
    playSound(c2, "pickup-winter");
    items[c2->item] += 31;
    }
  else if(c2->item == itRevolver) {
    playSound(c2, "pickup-key");
    items[c2->item] = 6;
    }
  else if(c2->item == itOrbStunning) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 61;
    }
  else if(c2->item == itOrbLuck) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 61;
    }
  else if(c2->item == itOrbUndeath) {
    playSound(c2, "pickup-orb");
    if(shmup::on) gainLife();
    else items[c2->item] += 31;
    }
  else if(c2->item == itOrbFreedom) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 31;
    }
  else if(c2->item == itOrb37) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 51;
    }
  else if(c2->item == itOrbEnergy) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 51;
    }
  else if(c2->item == itOrbDomination) {
    playSound(c2, "pickup-orb");
    if(shmup::on) gainLife();
    else items[c2->item] += 91;
    }
  else if(c2->item == itOrbShell) {
    playSound(c2, "pickup-orb"); // TOOD shield
    items[c2->item] += 67;
    }
  else if(c2->item == itBabyTortoise) {
    using namespace tortoise;
    int bnew = babymap[c2];
    babymap.erase(c2);
    int bold = seekbits;
    seekbits = bnew;
    if(seek()) {
      cell *c = passable(cwt.c, NULL, 0) ? cwt.c : c2;
      c->item = itBabyTortoise;
      if(c == c2) dopickup = false;
      babymap[c] = bold;
      }
    else items[itBabyTortoise]++;
    }
  else if(c2->item == itOrbBeauty) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 41;
    }
  else if(c2->item == itOrbEmpathy) {
    playSound(c2, "pickup-orb");
    if(shmup::on) gainLife();
    else items[c2->item] += 41;
    }
  else if(c2->item == itOrbFire) {
    playSound(c2, "fire");
    items[c2->item] += (sphere ? 3 : 31);
    }
  else if(c2->item == itOrbDragon) {
    playSound(c2, "pickup-orb");
    items[c2->item] += sphere ? 10 : 78;
    }
  else if(c2->item == itOrbIllusion) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbPsi) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbInvis) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 31;
    }
  else if(c2->item == itOrbAether) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 31;
    }
  else if(c2->item == itOrbDigging) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbTeleport) {
    playSound(c2, "pickup-orb");
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbYendor && yendor::state(c2) != yendor::ysUnlocked) {
    dopickup = false;
    }
  else if(c2->item == itOrbYendor) {
    playSound(c2, "tada");
    items[itOrbShield] += 31;
    for(int i=0; i<size(yendor::yi); i++)
      if(yendor::yi[i].path[0] == c2) 
        yendor::yi[i].foundOrb = true;
    // Shielding always, so that we know that it protects!
    for(int i=0; i<4; i++) switch(hrand(13)) {
      case 0: items[itOrbSpeed] += 31; break;
      case 1: items[itOrbLightning] += 78; break;
      case 2: items[itOrbFlash] += 78; break;
      case 3: items[itOrbTime] += 78; break;
      case 4: items[itOrbWinter] += 151; break;
      case 5: items[itOrbDigging] += 151; break;
      case 6: items[itOrbTeleport] += 151; break;
      case 7: items[itOrbThorns] += 151; break;
      case 8: items[itOrbInvis] += 151; break;
      case 9: items[itOrbPsi] += 151; break;
      case 10: items[itOrbAether] += 151; break;
      case 11: items[itOrbFire] += 151; break;
      case 12: items[itOrbSpace] += 78; break;
      }
    items[itOrbYendor]++; 
    items[itKey]--;
    yendor::everwon = true;
    if(yendor::on) {
      yendor::won = true;
      if(!cheater) {
        dynamicval<bool> c(chaosmode, false);
        yendor::bestscore[modecode()][yendor::challenge] = 
          max(yendor::bestscore[modecode()][yendor::challenge], items[itOrbYendor]);
        yendor::uploadScore();        
        }
      }
    addMessage(XLAT("CONGRATULATIONS!"));
    achievement_collection(itOrbYendor, pg, gold());
    achievement_victory(false);
    }
  else if(c2->item == itHolyGrail) {
    playSound(c2, "tada");
    int v = newRoundTableRadius() + 12;
    items[itOrbTeleport] += v;
    items[itOrbSpeed] += v;
    items[itHolyGrail]++;
    addMessage(XLAT("Congratulations! You have found the Holy Grail!"));
    if(!euclid) c2->master->alt->emeraldval |= GRAIL_FOUND;
    achievement_collection(c2->item, pg, gold());
    }
  else if(c2->item == itKey) {
    playSound(c2, "pickup-key");
    for(int i=0; i<size(yendor::yi); i++) if(yendor::yi[i].path[YDIST-1] == c2)
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
      achievement_gain("CHAOS", 'C');
      chaosAchieved = true;
      }

#ifdef MOBILE
    if(pg < lastsafety + 45 && g2 >= lastsafety + 45)
      addMessage(XLAT("The Orb of Safety from the Land of Eternal Motion might save you."));
#endif
    
    if(pg < 15 && g2 >= 15) 
      addMessage(XLAT("Collect treasure to access more different lands..."));
    if(pg < 30 && g2 >= 30)
      addMessage(XLAT("You feel that you have enough treasure to access new lands!"));
    if(pg < 45 && g2 >= 45)
      addMessage(XLAT("Collect more treasures, there are still more lands waiting..."));
    if(pg < 60 && g2 >= 60)
      addMessage(XLAT("You feel that the stars are right, and you can access R'Lyeh!"));
    if(pg < 75 && g2 >= 75)
      addMessage(XLAT("Kill monsters and collect treasures, and you may get access to Hell..."));
    if(pg < 90 && g2 >= 90) 
      addMessage(XLAT("To access Hell, collect 10 treasures each of 9 kinds..."));
    if(hellUnlocked() && !lhu) {
      addMessage(XLAT("Abandon all hope, the gates of Hell are opened!"));
      addMessage(XLAT("And the Orbs of Yendor await!"));
      }
    }
  
  if(dopickup && c2->item) {
#ifdef HASLINEVIEW
    // (eItem) to avoid the "cannot bind bitfield" problem in C++11
    conformal::findhistory.push_back(make_pair(c2, (eItem) c2->item));
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

  if(princess::reviveAt && gold() >= princess::reviveAt) {
    princess::reviveAt = 0,
    items[itSavedPrincess] = 1;
    addMessage("You have enough treasure now to revive the Princess!");
    }

  return false;
  }

void dropGreenStone(cell *c) {
  if(!passable(c, NULL, P_MONSTER)) {
    // NOTE: PL/CZ translations assume that itGreenStone is dropped to avoid extra forms!
    addMessage("Cannot drop %the1 here!", itGreenStone);
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
      if(isHaunted(cwt.c->land)) survivalist = false;
      }
    }
  else {
    if(gold() >= 300)
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
  if(!euclid && !cwt.c->master->alt) return;
  if(!euclid && !c2->master->alt) return;
  int dd = celldistAltRelative(c2) - celldistAltRelative(cwt.c);

  bool tooeasy = (roundTableRadius(c2) < newRoundTableRadius());
            
  if(dd>0) {
    if(grailWasFound(cwt.c)) {
      addMessage(XLAT("The Knights congratulate you on your success!"));
      knighted = roundTableRadius(cwt.c);
      }
    else if(!tooeasy)
      addMessage(XLAT("The Knights laugh at your failure!"));
    }
  else {
    if(grailWasFound(cwt.c))
      addMessage(XLAT("The Knights stare at you!"));
    else if(tooeasy) {
      if(!tactic::on)
        addMessage(XLAT("Come on, this is too easy... find a bigger castle!"));
      }
    else
      addMessage(XLAT("The Knights wish you luck!"));
    }
  }

void knightFlavorMessage(cell *c2) {

  if(!euclid && !c2->master->alt) {
    addMessage(XLAT("\"I am lost...\""));
    return;
    }

  if(tactic::on) {
    addMessage(XLAT("\"The Knights of the Horocyclic Table salute you!\""));
    return;
    }

  long long tab[100], sum[100];
  tab[0] = 1;
  
  if(!purehepta) {
    tab[1] = 1*7;
    tab[2] = 2*7;
    tab[3] = 4*7;
    tab[4] = 7*7;
    for(int i=5; i<100; i++) 
      tab[i] = tab[i-1] + tab[i-2] + tab[i-3] - tab[i-4];
    }
  else {
    // actually these are each second Fibonacci number
    tab[1] = 1*7;
    tab[2] = 3*7;
    for(int i=3; i<100; i++) 
      tab[i] = 3*tab[i-1] - tab[i-2];
    }
    
  sum[0] = 0;
  for(int i=1; i<100; i++) 
    sum[i] = sum[i-1] + tab[i-1];

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
  else if(msgid == 3) {
    addMessage(XLAT("\"I enjoy watching the hyperbug battles.\""));
    }
  else if(msgid == 4) {
    addMessage(XLAT("\"Have you visited a temple in R'Lyeh?\""));
    }
  else if(msgid == 5) {
    addMessage(XLAT("\"Nice castle, eh?\""));
    }
  else if(msgid == 6 && items[itSpice] < 10) {
    addMessage(XLAT("\"The Red Rock Valley is dangerous, but beautiful.\""));
    }
  else if(msgid == 7 && items[itSpice] < 10) {
    addMessage(XLAT("\"Train in the Desert first!\""));
    }
  else if(msgid == 8) {
    if(rad <= 76)
      addMessage(XLAT("\"Our Table seats %1 Knights!\"", llts(tab[rad])));
    else
      addMessage(XLAT("\"By now, you should have your own formula, you know?\""));
    }
  else if(msgid == 9 && rad <= 76) {
      addMessage(XLAT("\"There are %1 floor tiles inside our Table!\"", llts(sum[rad])));
    }
  else if(msgid == 10 && !items[itPirate] && !items[itWhirlpool]) {
    addMessage(XLAT("\"Have you tried to take a boat and go into the Ocean? Try it!\""));
    }
  else if(msgid == 11 && !princess::saved) {
    addMessage(XLAT("\"When I visited the Palace, a mouse wanted me to go somewhere.\""));
    }
  else if(msgid == 12 && !princess::saved) {
    addMessage(XLAT("\"I wonder what was there...\""));
    }
  else if(msgid == 13) {
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
    if(c->mov[i] && (c->mov[i]->wall == waMineUnknown || c->mov[i]->wall == waMineOpen))
      uncoverMines(c->mov[i], lev-1, dist+1);

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
    return lp >= 0 && ln >= 0 && angledist(c2, lp, ln) == 3;
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
      seq = is(prev[cp], lastmove, cwt.c);
      
    if(prev[cp] && prevtype[cp] == lmMove && lastmovetype == lmAttack)
      seq = is(prev[cp], cwt.c, lastmove);

    if(prev[cp] && prevtype[cp] == lmAttack && lastmovetype == lmAttack && count)
      seq = lastmove == prev[cp];

    if(prev[cp] && prevtype[cp] == lmAttack && lastmovetype == lmMove && count)
      seq = cwt.c == prev[cp];
    
    prev[cp] = lastmove; prevtype[cp] = lastmovetype;
    
    if(seq) {
      if(lastmovetype == lmMove) count++;
      gainBullPowers();
      }
    else count = 0;
    }
  }

void monstersTurn() {
  DEBT("bfs");
  bfs();
  DEBT("charge");
  if(elec::havecharge) elec::act();
  mirror::destroyStray();
  DEBT("mmo");
  int phase2 = (1 & items[itOrbSpeed]);
  if(!phase2) movemonsters();

  if(playerInPower() && (phase2 || !items[itOrbSpeed]) && (havewhat & HF_FAST)) 
    moveNormals(moWitchSpeed);

  if(phase2 && markOrb(itOrbEmpathy)) {
    bfs();
    movegolems(AF_FAST);
    for(int i=0; i<size(dcal); i++) {
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
  if(!phase1) heat::dryforest();
  DEBT("heat");
  heat::processheat();
  // if(elec::havecharge) elec::drawcharges();

  if(items[itOrbFreedom])
    for(int i=0; i<numplayers(); i++)
      if(multi::playerActive(i))
        checkFreedom(playerpos(i));

  orbbull::check();


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
    th->wall = isAlch(cwt.c) ? cwt.c->wall : cto->wall;
  else th->wall = waNone;
  if(cto->wall == waOpenPlate || cto->wall == waClosePlate) {
    toggleGates(cto, cto->wall);
    addMessage(XLAT("%The1 destroys %the2!", waThumperOn, cto->wall));
    }
  if(cellUnstable(cto) && cto->land == laMotion) {
    addMessage(XLAT("%The1 falls!", waThumperOn));
    doesFallSound(cto);
    }
  else if(cellUnstable(cto)) {
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
  if(tgt->wall == waBoat) return false;
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
  if(which == moWarning)
    addMessage(XLAT("This move appears dangerous -- are you sure?"));
  else if(which == moFireball) 
    addMessage(XLAT("Cannot move into the current location of another player!"));
  else if(which == moAirball) 
    addMessage(XLAT("Players cannot get that far away!"));
  else
    addMessage(XLAT(msg, which));
  }

bool havePushConflict(cell *pushto, bool checkonly) {
  if(pushto && multi::activePlayers() > 1) {
    for(int i=0; i<multi::players; i++)  if(i != multi::cpid && multi::playerActive(i))
      if(multi::origpos[i] == pushto || multi::origtarget[i] == pushto) {
        if(!checkonly) 
          addMessage(XLAT("Cannot push into another player!"));
        return true;
        }
    for(int i=0; i<size(stalemate::moves); i++)  {
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
  forCellEx(c2, cwt.c) if(c2->monst == moFriendlyIvy) 
    killMonster(c2, moPlayer, 0);
  }

bool movepcto(int d, int subdir, bool checkonly) {
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
    cwspin(cwt, d);
    if(multi::players == 1) mirror::spin(d);
    d = cwt.spin;
    }
  if(d != -1 && !checkonly) playermoved = true;
  if(!checkonly) invismove = false;  
  bool boatmove = false;
  
  if(againstRose(cwt.c, NULL) && d<0 && !scentResistant()) {
    if(checkonly) return false;
    addMessage("You just cannot stand in place, those roses smell too nicely.");
    return false;
    }

  if(d >= 0) {
    cell *c2 = cwt.c->mov[d];

    if(againstRose(cwt.c, c2) && !scentResistant()) {
      if(checkonly) return false;
      addMessage("Those roses smell too nicely. You have to come towards them.");
      return false;
      }
    
    if(items[itOrbDomination] > ORBBASE && isMountable(c2->monst) && !monstersnear2()) {
      if(checkonly) return true;
      if(!isMountable(cwt.c->monst)) dragon::target = NULL;
      movecost(cwt.c, c2);
      
      flipplayer = true; if(multi::players > 1) multi::flipped[multi::cpid] = true;
      invismove = (turncount >= noiseuntil) && items[itOrbInvis] > 0;
      killFriendlyIvy();
      goto mountjump;
      }
    
    if(!passable(c2, cwt.c, P_ISPLAYER | P_MIRROR | P_USEBOAT | P_FRIENDSWAP) && items[itOrbFlash]) {
      if(checkonly) return true;
      activateFlash();
      bfs();
      if(multi::players > 1) { multi::whereto[multi::cpid].d = MD_UNDECIDED; return false; }
      checkmove();
      return true;
      }

    if(!passable(c2, cwt.c, P_ISPLAYER | P_MIRROR | P_USEBOAT | P_FRIENDSWAP) && items[itOrbLightning]) {
      if(checkonly) return true;
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

    if(c2->wall == waThumperOn && !c2->monst && !nonAdjacentPlayer(c2, cwt.c)) {
      cellwalker push = cwt;
      cwstep(push);
      cwspin(push, 3 * -subdir);
      cwstep(push);
/*    if(w == waBigStatue && push.c->type == 7) {
        if(checkonly) return false;
        addMessage(XLAT("%The1 is too heavy to put it back on the pedestal.", c2->wall));
        return false;
        } */
      if((!canPushThumperOn(push.c, c2, cwt.c) && c2->type == 7)) {
        cwstep(push);
        cwspin(push, 1 * -subdir);
        cwstep(push);
        }
      if(!canPushThumperOn(push.c, c2, cwt.c)) {
        if(checkonly) return false;
        addMessage(XLAT("No room to push %the1.", c2->wall));
        return false;
        }
      if(monstersnear(c2, NULL, moPlayer, NULL, cwt.c)) {
        if(!checkonly && errormsgs) wouldkill("%The1 would kill you there!");
        return false;
        }
      global_pushto = push.c;
      if(checkonly) return true;
      addMessage(XLAT("You push %the1.", c2->wall));
      lastmovetype = lmPush; lastmove = cwt.c;
      pushThumper(c2, push.c);
      }

/*  if((c2->wall == waBigStatue) && c2->type == 7 && !monstersnear(c2)) {
      int q = 0;
      for(int i=3; i<=4; i++) {
        cellwalker push = cwt;
        cwstep(push);
        cwspin(push, i);
        cwstep(push);
        if(passable(push.c, c2, false, true)) q++;
        }
      if(!q) {
        if(checkonly) return false;
        addMessage(XLAT("No room to push %the1.", c2->wall));
        return false;
        }
      if(checkonly) return true;
      addMessage(XLAT("You push %the1.", c2->wall));
      c2->wall = waNone;
      for(int i=3; i<=4; i++) {
        cellwalker push = cwt;
        cwstep(push);
        cwspin(push, i);
        cwstep(push);
        if(passable(push.c, c2, false, true)) 
          push.c->wall = waBigStatue;
        }
      } */

    if(c2->item == itHolyGrail && roundTableRadius(c2) < newRoundTableRadius()) {
      if(checkonly) return false;
      addMessage(XLAT("That was not a challenge. Find a larger castle!"));
      return false;
      }

    if(isWatery(c2) && !nonAdjacentPlayer(cwt.c,c2) && !c2->monst && cwt.c->wall == waBoat) {

      if(havePushConflict(cwt.c, checkonly)) return false;

      if(againstWind(c2, cwt.c)) {
        if(!checkonly) 
          addMessage(XLAT(airdist(c2) < 3 ? "The Air Elemental blows you away!" : "You cannot go against the wind!"));
        return false;
        }

      if(againstCurrent(c2, cwt.c) && !markOrb(itOrbWater)) {
        if(markOrb(itOrbFish)) goto escape;
        if(!checkonly)
          addMessage(XLAT("You cannot go against the current!"));
        return false;
        }

      if(monstersnear(c2, NULL, moPlayer, NULL, cwt.c)) {
        if(!checkonly && errormsgs) wouldkill("%The1 would kill you there!");
        return false;
        }

      if(checkonly) return true;
      moveBoat(c2, cwt.c);
      boatmove = true;
      goto boatjump;
      }
    
    if(!c2->monst && cwt.c->wall == waBoat && boatGoesThrough(c2) && markOrb(itOrbWater)) {

      if(havePushConflict(cwt.c, checkonly)) return false;
      if(monstersnear(c2,NULL,moPlayer,NULL,cwt.c)) {
        if(!checkonly && errormsgs) wouldkill("%The1 would kill you there!");
        return false;
        }
      
      if(checkonly) return true;
      placeWater(c2, cwt.c);
      moveBoat(c2, cwt.c);
      c2->mondir = neighborId(c2, cwt.c);
      if(cwt.c->item) moveItem(cwt.c, c2, false), boatmove = true;
      goto boatjump;
      }

    escape:
    if(c2->wall == waBigStatue && !c2->monst && !nonAdjacentPlayer(c2, cwt.c)) {
      if(!canPushStatueOn(cwt.c)) {
        if(checkonly) return false;
        if(isFire(cwt.c))
          addMessage(XLAT("You have to escape first!"));
        else
          addMessage(XLAT("There is not enough space!"));
        return false;
        }
      
      if(havePushConflict(cwt.c, checkonly)) return false;
      
      eWall save_c2 = c2->wall;
      eWall save_cw = cwt.c->wall;
      c2->wall = cwt.c->wall;
      if(doesnotFall(cwt.c))
        cwt.c->wall = waBigStatue;
      
      if(monstersnear(c2,NULL,moPlayer,NULL,cwt.c)) {
        if(!checkonly && errormsgs) wouldkill("%The1 would kill you there!");
        c2->wall = save_c2; cwt.c->wall = save_cw;
        return false;
        }        
          
      if(checkonly) { c2->wall = save_c2; cwt.c->wall = save_cw; return true; }
      addMessage(XLAT("You push %the1 behind you!", waBigStatue));
      animateMovement(c2, cwt.c, LAYER_BOAT);
      goto statuejump;
      }

    if(c2->wall == waBigTree && !c2->monst && !markOrb(itOrbAether) && !nonAdjacentPlayer(cwt.c,c2)) {
      if(checkNeedMove(checkonly, true)) return false;
      if(monstersnear(cwt.c,NULL,moPlayer,NULL,cwt.c)) {
        if(!checkonly && errormsgs) wouldkill("%The1 would get you!");
        return false;
        }
      if(checkonly) return true;
      drawParticles(c2, winf[c2->wall].color, 4);
      addMessage(XLAT("You start chopping down the tree."));
      playSound(c2, "hit-axe" + pick123());
      if(survivalist && isHaunted(c2->land))
        survivalist = false;
      mirror::spingo(origd, 0);
      c2->wall = waSmallTree;
      lastmovetype = lmTree; lastmove = c2;
      swordAttackStatic();
      }
    else if(c2->wall == waSmallTree && !c2->monst && !markOrb(itOrbAether) && !nonAdjacentPlayer(cwt.c,c2)) {
      if(checkNeedMove(checkonly, true)) return false;
      if(monstersnear(cwt.c,NULL,moPlayer,NULL,cwt.c)) {
        if(!checkonly && errormsgs) wouldkill("%The1 would get you!");
        return false;
        }
      if(checkonly) return true;
      drawParticles(c2, winf[c2->wall].color, 8);
      addMessage(XLAT("You chop down the tree."));
      playSound(c2, "hit-axe" + pick123());
      if(survivalist && isHaunted(c2->land))
        survivalist = false;
      mirror::spingo(origd, 0);
      lastmovetype = lmTree; lastmove = c2;
      c2->wall = waNone;
      swordAttackStatic();
      }
    else if(c2->monst == moKnight) {
      if(checkonly) return false;
      knightFlavorMessage(c2);
      return false;
      }
    else if(c2->monst && (!isFriendly(c2) || c2->monst == moTameBomberbird || isMountable(c2->monst))) {
    
      bool fast = !((!items[itOrbSpeed]) || (items[itOrbSpeed]&1));
      
      if(!canAttack(cwt.c, moPlayer, c2, c2->monst, fast ? AF_FAST : 0)) {
        if(checkonly) return false;
        if(c2->monst == moWorm || c2->monst == moWormtail || c2->monst == moWormwait) 
          addMessage(XLAT("You cannot attack Sandworms directly!"));
        else if(c2->monst == moHexSnake || c2->monst == moHexSnakeTail)
          addMessage(XLAT("You cannot attack Rock Snakes directly!"));
        else if(nonAdjacent(c2, cwt.c))
          addMessage(XLAT("You cannot attack diagonally!"));
        else if(thruVine(c2, cwt.c))
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
        else if(c2->monst == moVizier && c2->hitpoints > 1 && !fast) {
          addMessage(XLAT("You cannot attack %the1 directly!", c2->monst));
          addMessage(XLAT("Hit him by walking away from him."));
          }
        else if(c2->monst == moShadow)
          addMessage(XLAT("You cannot defeat the Shadow!"));
        else if(c2->monst == moGreater || c2->monst == moGreaterM)
          addMessage(XLAT("You cannot defeat the Greater Demon yet!"));
        else if(c2->monst == moDraugr)
          addMessage(XLAT("Your mundane weapon cannot hurt %the1!", c2->monst));
        else
          addMessage(XLAT("For some reason... cannot attack!"));
        return false;
        }
      
      cell *pushto = NULL;
      if(isStunnable(c2->monst) && c2->hitpoints > 1) {
        // pushto=c2 means that the monster is not killed and thus
        // still counts for lightning in monstersnear
        pushto = c2;
        if(c2->monst != moFatGuard && !(isMetalBeast(c2->monst) && c2->stuntime < 2) && c2->monst != moTortoise) {
          cellwalker push = cwt;
          cwstep(push);
          cwspin(push, 3 * -subdir);
          cwstep(push);
          if(c2->type == 7 && !passable(push.c, c2, P_BLOW)) {
            cwstep(push);
            cwspin(push, 1 * -subdir);
            cwstep(push);
            }
          if(!passable(push.c, c2, P_BLOW) && gravityLevel(push.c) < gravityLevel(c2)) {
            cwstep(push); cwspin(push, 1); cwstep(push);
            if(gravityLevel(push.c) < gravityLevel(c2)) {
              cwstep(push); cwspin(push, -2); cwstep(push);
              }
            if(gravityLevel(push.c) < gravityLevel(c2)) {
              cwstep(push); cwspin(push, 1); cwstep(push);
              }
            }
          if(passable(push.c, c2, P_BLOW))
            pushto = push.c;
          }
        }
      if(c2->monst == moTroll || c2->monst == moFjordTroll || 
         c2->monst == moForestTroll || c2->monst == moStormTroll || c2->monst == moVineSpirit)
        pushto = c2;
      
      global_pushto = pushto;
      
      if(havePushConflict(pushto, checkonly)) return false;
      
      if(!(isWatery(cwt.c) && c2->monst == moWaterElemental) && checkNeedMove(checkonly, true))
        return false;
      
      if(monstersnear(cwt.c, c2, moPlayer, pushto, cwt.c)) {
        if(errormsgs && !checkonly)
          wouldkill("You would be killed by %the1!");          
        return false;
        }

      if(checkonly) return true;
      
      
      /* if(c2->monst == moTortoise) {
        printf("seek=%d get=%d <%x/%x> item=%d\n", 
          tortoise::seeking, 
          !tortoise::diff(tortoise::getb(c2)), 
          tortoise::getb(c2) & tortoise::mask,
          tortoise::seekbits & tortoise::mask,
          !c2->item);
        } */
      
      if(c2->monst == moTortoise && tortoise::seek() && !tortoise::diff(tortoise::getb(c2)) && !c2->item) {
        items[itBabyTortoise] += 4;
        updateHi(itBabyTortoise, items[itBabyTortoise]);
        c2->item = itBabyTortoise;
        tortoise::babymap[c2] = tortoise::seekbits;
        playSound(c2, playergender() ? "heal-princess" : "heal-prince");
        addMessage(XLAT(playergender() == GEN_F ? "You are now a tortoise heroine!" : "You are now a tortoise hero!"));
        c2->stuntime = 2;
        achievement_collection(itBabyTortoise, 0, 0);
        }
      else if(isStunnable(c2->monst) && c2->hitpoints > 1) {
        attackMonster(c2, AF_ORSTUN | AF_MSG, moPlayer);
        if(pushto && pushto != c2) pushMonster(pushto, c2);
        }
      else if(c2->monst == moVizier && c2->hitpoints > 1) {
        fightmessage(c2->monst, moPlayer, true, 0);
        c2->hitpoints--;
        }
      else if(isDragon(c2->monst) || isKraken(c2->monst)) {
        attackMonster(c2, AF_ORSTUN | AF_MSG, moPlayer);
        }
      else {
        eMonster m = c2->monst;
        attackMonster(c2, AF_MSG, moPlayer);
        produceGhost(c2, m, moPlayer);
        }
      
      mirror::spingo(origd, 0);
      lastmovetype = lmAttack; lastmove = c2;
      swordAttackStatic();
      }
    else if(!passable(c2, cwt.c, P_USEBOAT | P_ISPLAYER | P_MIRROR | P_MONSTER)) {
      if(checkonly) return false;
      if(nonAdjacent(cwt.c,c2))
        addMessage(XLAT(
          purehepta ? 
          "You cannot move between the cells without dots here!" :
          "You cannot move between the triangular cells here!"
          ));
      else if(isAlch(c2))
        addMessage(XLAT("Wrong color!"));
      else if(c2->wall == waRoundTable)
        addMessage(XLAT("It would be impolite to land on the table!"));
      else if(cwt.c->wall == waRed3 && snakelevel(c2) == 0)
        addMessage(XLAT("You would get hurt!", c2->wall));
      else if(cwt.c->wall == waTower && snakelevel(c2) == 0)
        addMessage(XLAT("You would get hurt!", c2->wall));
      else if(cellEdgeUnstable(cwt.c) && cellEdgeUnstable(c2))
        addMessage(XLAT("Gravity does not allow this!"));
      else if(againstWind(c2, cwt.c))
        addMessage(XLAT(airdist(c2) < 3 ? "The Air Elemental blows you away!" : "You cannot go against the wind!"));
      else {
        addMessage(XLAT("You cannot move through %the1!", c2->wall));
        }
      return false;
      }
    else {
      if(mineMarked(c2) && !minesafe() && !checkonly && warningprotection()) {
        addMessage("Are you sure you want to step there?");
        return false;
        }
      if(c2->item == itOrbYendor && !boatmove && !checkonly && yendor::check(c2)) {
        return false;
        }
      if(monstersnear(c2, NULL, moPlayer, NULL, cwt.c)) {
        if(checkonly) return false;

        if(items[itOrbFlash]) {
          if(checkonly) return true;
          activateFlash();
          checkmove();
          return true;
          }

        if(items[itOrbLightning]) {
          if(checkonly) return true;
          activateLightning();
          checkmove();
          return true;
          }

        if(which == moOutlaw && items[itRevolver]) {
          for(int i=0; i<c2->type; i++) {
            cell *c3 = c2->mov[i];
            if(c3) for(int i=0; i<c3->type; i++) {
              cell *c4 = c3->mov[i];
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
      if(checkonly) return true;
      boatjump:
      statuejump:
      flipplayer = true; if(multi::players > 1) multi::flipped[multi::cpid] = true;
      if(c2->item && c2->land == laAlchemist) c2->wall = cwt.c->wall;
      if(c2->wall == waRoundTable) {
        addMessage(XLAT("You jump over the table!"));
        }
      
      if(cwt.c->wall == waRoundTable) 
        roundTableMessage(c2);
      
      invismove = (turncount >= noiseuntil) && items[itOrbInvis] > 0;
      
      if(items[itOrbFire]) {
        invismove = false;
        if(makeflame(cwt.c, 10, false)) markOrb(itOrbFire);
        }

      {
      bool haveIvy = false;
      forCellEx(c3, cwt.c) if(c3->monst == moFriendlyIvy) haveIvy = true;
        
      bool killIvy = haveIvy;
        
      if(items[itOrbNature]) {
        if(c2->monst != moFriendlyIvy && strictlyAgainstGravity(c2, cwt.c, false, MF_IVY)) {
          invismove = false;
          }
        else if(cwt.c->monst) invismove = false;
        else if(haveIvy || !cellEdgeUnstable(cwt.c, MF_IVY)) {
          cwt.c->monst  = moFriendlyIvy;
          cwt.c->mondir = neighborId(cwt.c, c2);
          invismove = false;
          markOrb(itOrbNature);
          killIvy = false;
          }
        }
      
      if(killIvy) killFriendlyIvy();
      }

      if(items[itOrbDigging]) {
        invismove = false;
        if(earthMove(cwt.c, d)) markOrb(itOrbDigging);
        }

      if(!boatmove && collectItem(c2)) return true;

      if(isIcyLand(cwt.c) && cwt.c->wall == waNone && markOrb(itOrbWinter)) {
        invismove = false;
        cwt.c->wall = waIcewall;
        }
      
      movecost(cwt.c, c2);

      if(c2->monst == moGolem || c2->monst == moIllusion || isPrincess(c2->monst) || c2->monst == moMouse ||
        c2->monst == moFriendlyGhost) {
        bool pswitch = false;
        if(c2->monst == moMouse)
          princess::mouseSqueak(c2);
        else if(isPrincess(c2->monst)) {
          princess::line(c2);
          princess::move(cwt.c, c2);
          }
        else  
          pswitch = true;
        cwt.c->hitpoints = c2->hitpoints;
        cwt.c->stuntime = c2->stuntime;
        placeGolem(cwt.c, c2, c2->monst);
        if(cwt.c->monst != moNone && pswitch)
          addMessage(XLAT("You switch places with %the1.", c2->monst));
        c2->monst = moNone;
        switchplaces = true;
        }
      else if(isMimic(c2->monst)) {
        addMessage(XLAT("You rejoin %the1.", c2->monst));
        playSound(c2, "click");
        killMonster(c2, moNone);
        }
      
      mountjump:
      lastmovetype = lmMove; lastmove = cwt.c;

      stabbingAttack(cwt.c, c2, moPlayer);
      cell *c1 = cwt.c;
      cwstep(cwt);
      if(switchplaces)
        animateReplacement(c1, cwt.c, LAYER_SMALL);
      else
        animateMovement(c1, cwt.c, LAYER_SMALL);
      
      mirror::spingo(origd, 1);

      playerMoveEffects(c1, c2);

      if(c2->monst == moFriendlyIvy) c2->monst = moNone;

      countLocalTreasure();
      landvisited[cwt.c->land] = true;
      afterplayermoved();
      }
    }
  else {
    lastmovetype = lmSkip; lastmove = NULL;
    if(checkNeedMove(checkonly, false))
      return false;
    if(monstersnear(cwt.c, NULL, moPlayer, NULL, cwt.c)) {
      if(errormsgs && !checkonly) 
        wouldkill("%The1 would get you!");
      return false;
      }
    if(checkonly) return true;
    swordAttackStatic();
    if(d == -2) 
      dropGreenStone(cwt.c);
    if(cellUnstable(cwt.c) && !markOrb(itOrbAether))
      doesFallSound(cwt.c);
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

  if(items[itWhirlpool] && cwt.c->land != laWhirlpool && !whirlpool::escaped) {
    whirlpool::escaped = true;
    achievement_gain("WHIRL1");
    }

  if(items[itLotus] >= 25 && !isHaunted(cwt.c->land) && survivalist) {
    survivalist = false;
    achievement_gain("SURVIVAL");
    }

  if(seenSevenMines && cwt.c->land != laMinefield) {
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
    for(int i=0; i<size(yendor::yi); i++) 
      if(yendor::yi[i].path[0] == from) xnew = false;
    if(xnew && activateYendor) yendor::check(from);
    for(int i=0; i<size(yendor::yi); i++) 
      if(yendor::yi[i].path[0] == from) 
        yendor::yi[i].path[0] = to;
    }

  if(from->item == itKey) {
    for(int i=0; i<size(yendor::yi); i++) if(yendor::yi[i].path[YDIST-1] == from)
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
    if(c->mov[i] && isWorm(c->mov[i]->monst) && c->mov[i]->mondir == c->spn(i)) {
      c = c->mov[i]; goto findhead;
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
    if(c->mov[i] && isWorm(c->mov[i]->monst) && c->mov[i]->mondir == c->spn(i)) {
      c = c->mov[i]; cnt++; goto findhead;
      }
  fixWormBug(c);
  return cnt;
  }
  
// currently works for worms only
bool sameMonster(cell *c1, cell *c2) {
  if(c1 == c2) return true;
  if(isWorm(c1->monst) && isWorm(c2->monst))
    return wormhead(c1) == wormhead(c2);
  if(isKraken(c1->monst) && isKraken(c2->monst))
    return kraken::head(c1) == kraken::head(c2);
  return false;
  }

eMonster getMount(int i) {
  if(!items[itOrbDomination]) return moNone;
  return playerpos(i)->monst;
  }

eMonster haveMount() {
  for(int i=0; i<numplayers(); i++) if(multi::playerActive(i)) {
    eMonster m = getMount(i);
    if(m) return m;
    }
  return moNone;
  }

bool mightBeMine(cell *c) {
  return c->wall == waMineUnknown || c->wall == waMineMine;
  }

void performMarkCommand(cell *c) {
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

bool warningprotection() {
  if(hardcore) return false;
  if(multi::activePlayers() > 1) return false;
  if(items[itWarning]) return false;
  items[itWarning] = 1;
  return true;
  }

