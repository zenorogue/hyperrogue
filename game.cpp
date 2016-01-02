// Hyperbolic Rogue

// Copyright (C) 2011-2013 Zeno Rogue, see 'hyper.cpp' for details

#define DEB(x) // printf("%s\n", x); fflush(stdout);

#define SAGEMELT .1

#define TEMPLE_EACH 6

time_t timerstart, savetime;
bool timerstopped;
int savecount;
int turncount;
int lastexplore;
eLand lastland;

bool hardcore = false;
int hardcoreAt;

bool randomPatternsMode = false;
int randompattern[landtypes];

#define RANDLANDS 14
eLand randlands[14] = {
  laIce, laDesert, laCaves, laAlchemist, laGraveyard, laPower, laLivefjord, laZebra,
  laRlyeh, laDryForest, laEmerald, laWineyard, laDeadCaves, laRedRock
  };

bool seenSevenMines = false;

#define PUREHARDCORE_LEVEL 10

bool pureHardcore() { return hardcore && hardcoreAt < PUREHARDCORE_LEVEL; }

bool canmove = true;

int sagephase = 0;

// number of Grails collected, to show you as a knight
int knighted = 0;

bool showoff = false;
bool safety = false;
int showid = 0;

bool lifebrought = false; // was Life brought to the Dead Caves?
bool escaped = false; // escaped the Whirlpool?

bool invismove = false, invisfish = false; // last move was invisible [due to Fish]

int currentLocalTreasure;

bool landvisited[landtypes];

eLand showlist[10] = {
  laHell, laRlyeh, laAlchemist, laGraveyard, laCaves, laDesert, laIce, laJungle, laMotion, laMirror
  };

eLand firstland = laIce, euclidland = laIce;

extern void DEBT(const char *buf);

#define DEBT(x) // printf("%s\n", x);

bool eq(short a, short b) { return a==b; }

// game state
int items[ittypes], hiitems[ittypes], kills[motypes], explore[10], exploreland[10][landtypes], landcount[landtypes];
bool orbused[ittypes], lastorbused[ittypes];
bool playerdead = false;  // obsolete
bool playermoved = true;  // center on the PC?
bool flipplayer = true;   // flip the player image after move, do not flip after attack
int  cheater = 0;         // did the player cheat?

#define INF  9999
#define INFD 20
#define BARLEV (ISANDROID?9:10)
#define BUGLEV 15
// #define BARLEV 9

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

cellwalker cwt; // player character position

// heat <-> Celsius calculations:

bool isIcyLand(eLand l) {
  return l == laIce || l == laCocytus;
  }

bool isIcyLand(cell *c) {
  return isIcyLand(c->land);
  }

inline float& HEAT(cell *c) {
  return c->LHU.heat;
  }

#include "mtrand.cpp"

MTRand_int32 r;

void shrand(int i) {
  r.seed(i);
  }

int hrand(int i) { 
  return r() % i;
  }

double absheat(cell *c) {
  if(c->land == laCocytus) return HEAT(c) -.6;
  if(c->land == laIce) return HEAT(c) -.4;
  return 0;
  }

double celsius(cell *c) { return absheat(c) * 60; }

// watery

bool isWatery(cell *c) {
  return c->wall == waCamelotMoat || c->wall == waSea || c->wall == waLake;
  }

bool isWateryOrBoat(cell *c) {
  return isWatery(c) || c->wall == waBoat;
  }

bool boatStrandable(cell *c) {
  return c->wall == waNone && (c->land == laLivefjord || c->land == laOcean);
  }

void initcell(cell *c) {
  c->mpdist = INFD;   // minimum distance from the player, ever
  c->cpdist = INFD;   // current distance from the player
  c->pathdist = INFD; // current distance from the player, along paths (used by yetis)
  c->landparam = 0; c->landflags = 0;
  c->wall  = waNone;
  c->item  = itNone;
  c->monst = moNone;
  c->bardir = NODIR;
  c->land = laNone;
  c->ligon = 0;
  c->stuntime = 0;
  lastexplore = shmup::on ? shmup::curtime : turncount;
  }

// 0 = basic treasure, 1 = something else, 2 = power orb
#define IC_TREASURE 0
#define IC_OTHER 1
#define IC_ORB 2

bool isElementalShard(eItem i) {
  return 
    i == itFireShard || i == itAirShard || i == itEarthShard || i == itWaterShard;
  }

eMonster elementalOf(eLand l) {
  if(l == laEFire) return moFireElemental;
  if(l == laEWater) return moWaterElemental;
  if(l == laEAir) return moAirElemental;
  if(l == laEEarth) return moEarthElemental;
  return moNone;
  }

int itemclass(eItem i) {
  if(i == 0) return -1;
  if(i < itKey || i == itFernFlower || 
    i == itWine || i == itSilver || i == itEmerald || i == itRoyalJelly || i == itPower ||
    i == itGrimoire || i == itPirate || i == itRedGem || i == itBombEgg ||
    i == itCoast || i == itWhirlpool || i == itPalace || i == itFjord ||
    i == itElemental || i == itZebra || i == itEdge)
    return IC_TREASURE;
  if(i == itKey || i == itOrbYendor || i == itGreenStone || i == itHolyGrail || i == itCompass ||
    i == itSavedPrincess || isElementalShard(i)) 
    return IC_OTHER;  
  return IC_ORB;
  }
  
bool itemHidden(cell *c) {
  return isWatery(c);
  }

bool itemHiddenFromSight(cell *c) {
  return isWatery(c) && !items[itOrbInvis] && !(items[itOrbFish] && isWatery(cwt.c));
  }

int puregold() {
  int i = items[itOrbYendor] * 50 + items[itHolyGrail] * 10;
  if(items[itOrbLove]) i += 30;
  for(int t=0; t<ittypes; t++) 
    if(itemclass(eItem(t)) == IC_TREASURE)
      i += items[t];
  return i;
  }

eItem treasureType(eLand l) {
  switch(l) {
    case laIce: return itDiamond;
    case laJungle: return itRuby;
    case laCaves: return itGold;
    case laDesert: return itSpice;

    case laAlchemist: return itElixir;
    case laMirror: return itShard;
    case laMotion: return itFeather;

    case laGraveyard: return itBone;
    case laRlyeh: return itStatue;
    case laDryForest: return itFernFlower;    

    case laHell: return itHell;
    case laCocytus: return itSapphire;
    case laCrossroads: return itHyperstone;
    case laCrossroads2: return itHyperstone;
    case laCrossroads3: return itHyperstone;
    
    case laNone: return itNone;
    case laBarrier: return itNone;
    case laOceanWall: return itNone;
    case laCanvas: return itNone;
    
    case laEmerald: return itEmerald;
    case laWineyard: return itWine;
    case laHive: return itRoyalJelly;
    case laDeadCaves: return itSilver;
    case laPower: return itPower;
    case laCamelot: return itHolyGrail;
    case laTemple: return itGrimoire;
    
    case laCaribbean: return itPirate;
    case laRedRock: return itRedGem;
    
    case laMinefield: return itBombEgg;
    case laOcean: return itCoast;
    case laWhirlpool: return itWhirlpool;
    case laPalace: return itPalace;
    case laLivefjord: return itFjord;
    
    case laEdge: return itEdge;
    case laZebra: return itZebra;

    case laEAir: case laEEarth: case laEWater: case laEFire: 
    case laElementalWall: return itElemental;
    
    case laPrincessQuest: return itSavedPrincess;
    }
  return itNone;
  }

#define ORBLINES 33

struct orbinfo {
  eLand l;
  int lchance;
  int gchance;
  eItem orb;
  };

orbinfo orbinfos[ORBLINES] = {
  {laJungle, 1200, 1500,itOrbLightning},
  {laIce, 2000, 1500,itOrbFlash},
  {laCaves, 1800, 2000,itOrbLife},
  {laAlchemist, 800, 800,itOrbSpeed},
  {laGraveyard, 200, 200,itGreenStone},
  {laDesert, 2500, 900,itOrbShield},
  {laHell, 2000, 1000,itOrbYendor},
  {laRlyeh, 1500, 1500,itOrbTeleport},
  {laMotion, 2000, 700, itOrbSafety},    
  {laIce, 1500, 0, itOrbWinter},
  {laDryForest, 2500, 0, itOrbWinter}, 
  {laCocytus, 1500, 1500, itOrbWinter}, 
  {laCaves, 1200, 0, itOrbDigging},
  {laDryForest, 500, 2000, itOrbThorns},
  {laDeadCaves, 1800, 0, itGreenStone},
  {laDeadCaves, 1800, 1500, itOrbDigging},
  {laEmerald, 1500, 3500, itOrbPsi},
  {laWineyard, 900, 1200, itOrbGhost},
  {laHive, 800, 1200, itOrbInvis},
  {laPower, 0, 3000, itOrbFire},
  {laMinefield, 0, 3500, itOrbFriend},
  {laTemple, 0, 3000, itOrbDragon},
  {laCaribbean, 0, 3500, itOrbPreserve},
  {laRedRock, 0, 2500, itOrbTelekinesis},
  {laCamelot, 1000, 1500, itOrbIllusion},
  {laOcean, 0, 3000, itOrbAir},
  {laPalace, 0, 4000, itOrbDiscord},
  {laZebra, 500, 1500, itOrbFrog},
  {laLivefjord, 0, 1800, itOrbFish},
  {laPrincessQuest, 0, 2500, itOrbLove},
  {laWhirlpool, 0, 2000, itOrbWater},
  {laEdge, 500, 4000, itOrbMatter},
  {laElementalWall, 1500, 4000, itOrbSummon},
  };

bool isElemental(eLand l);

eItem orbType(eLand l) {
  if(isElemental(l)) l = laElementalWall;
  for(int i=0; i<ORBLINES; i++) 
    if(orbinfos[i].l == l && orbinfos[i].gchance)
      return orbinfos[i].orb;
  return itNone;
  }

enum eOrbLandRelation { 
  olrForbidden, // never appears: forbidden
  olrDangerous, // never appears: would be dangerous
  olrUseless,   // never appears: useless here
  olrNoPrizes,  // no prizes in this land
  olrNoPrizeOrb,// orb not allowed as a prize
  olrPrize25,   // prize for collecting 25
  olrPrize3,    // prize for collecting 3
  olrNative,    // native orb in this land
  olrNative1,    // native orb in this land (1)
  olrGuest,     // extra orb in this land
  olrPNative,   // Land of Power: native
  olrPBasic,    // Land of Power: basic orbs
  olrPPrized,   // Land of Power: prized orbs
  olrPNever,    // Land of Power: foreign orbs
  olrHub,       // hub lands
  olrMonster    // available from a monster
  };

string olrDescriptions[] = {
  "forbidden to find in %the1",
  "too dangerous to use in %the1",
  "useless in %the1",
  "only native Orbs allowed in %the1",
  "this Orb is never unlocked globally (only hubs)",
  "collect 25 %2 to unlock it in %the1",
  "collect 3 %2 to unlock it in %the1",
  "native in %the1 (collect 10 %2)",
  "native in %the1 (collect 1 %2)",
  "secondary in %the1 (collect 10 %3, or 25 %2)",
  "the native Orb of %the1",
  "this Orb appears on floors and is used by witches",
  "a prized Orb, it appears only in cabinets",
  "this Orb never appears in %the1",
  "Hub Land: orbs appear here if unlocked in their native land",
  "kill a monster, or collect 25 %2"
  };

eOrbLandRelation getOLR(eItem it, eLand l) {

  if(l == laPower) {
    if(it == itOrbFire) return olrPNative;

    if(
      it == itOrbFlash || it == itOrbSpeed || it == itOrbWinter || it == itOrbGhost ||
      it == itOrbLife) return olrPBasic;

    if(
      it == itOrbLightning || it == itOrbThorns || it == itOrbInvis ||
      it == itOrbShield || it == itOrbTeleport || it == itOrbPsi ||
      it == itOrbDragon || it == itOrbIllusion || it == itOrbPreserve)
        return olrPPrized;
    
    return olrPNever;
    }
    
  if(it == itOrbYendor && l == laWhirlpool) return olrUseless;
  
  if(it == itOrbAir && l == laAlchemist) return olrUseless;
  // if(it == itOrbShield && l == laMotion) return olrUseless;

  if(it == itOrbIllusion && l == laCamelot) return olrNative1;
  if(it == itOrbLove) return olrNoPrizeOrb;    
  if(orbType(l) == it) return olrNative;
  if(it == itOrbWinter && (l == laIce || l == laDryForest))
    return olrGuest;
  if(it == itOrbDigging && l == laCaves)
    return olrGuest;
  if(it == itOrbFrog && (l == laPalace || l == laPrincessQuest))
    return olrGuest;
  if(it == itOrbDragon && l == laRlyeh)
    return olrMonster;
  if(it == itOrbWater && l == laLivefjord)
    return olrMonster;
  if(isCrossroads(l) || l == laOcean)
    return olrHub;
  
  if(l == laCocytus)
    if(it == itOrbDragon || it == itOrbFire || it == itOrbFlash || it == itOrbLightning)
      return olrDangerous;
  
  if(it == itOrbSafety)
    if(l == laCaves || l == laLivefjord || l == laRedRock || l == laCocytus || l == laHell ||
      l == laDesert || l == laAlchemist || l == laDeadCaves || l == laMinefield) 
      return olrDangerous;
    
  if(it == itOrbMatter)
    if(l == laCaves || l == laEmerald || l == laAlchemist || l == laCaribbean || 
      l == laMinefield || l == laCocytus) return olrUseless;

  if(l == laPrincessQuest)
    if(it == itOrbGhost || it == itOrbFlash || it == itOrbTeleport || it == itOrbSummon)
      return olrForbidden;
    
  if(l == laTemple)
    return olrNoPrizes;
  
  if(it == itOrbDigging) {
    if(l == laCaves || l == laOcean || l == laLivefjord || l == laEmerald ||
      l == laDesert || l == laDeadCaves || l == laRedRock || l == laCaribbean || l == laGraveyard)
        return olrPrize25;
    return olrUseless;
    }
  
  if(it == itShard) {
    if(l == laDesert || l == laIce || l == laJungle || l == laGraveyard ||
      l == laRlyeh || l == laHell || l == laDryForest || l == laWineyard ||
      l == laHive || l == laCamelot || l == laRedRock || l == laPalace ||
      l == laLivefjord || l == laZebra || isElemental(l) || l == laPrincessQuest)
      return olrPrize25;
    return olrForbidden;
    }

  if(it == itOrbWater) 
    if(l == laMotion || l == laZebra || l == laEdge)
      return olrUseless;
  
  if(it == itOrbWinter && l != laRlyeh && l != laTemple) 
    return olrUseless;
  
  if(it == itOrbLife && l == laMotion)
    return olrUseless;
  
  if(it == itOrbFish && l != laOcean && l != laLivefjord && l != laWhirlpool && l != laCamelot)
    return olrUseless;
  
  if(it == itOrbIllusion) return olrPrize3;
  
  return olrPrize25;
  }

int gold();
int tkills();
bool hellUnlocked();

int landMultiplier(eLand l) {
  if(l == laCamelot || l == laPrincessQuest) return 10;
  return 1;
  }

// 2 = always available, 1 = highscore required, 0 = never available
int isRandland(eLand l) {
  if(l == laIce || l == laDesert || l == laCaves)
    return 2;
  for(int i=0; i<RANDLANDS; i++) if(randlands[i] == l) return 1;
  return 0;
  }

bool landUnlocked(eLand l) {
  if(randomPatternsMode) {
    int i = isRandland(l);
    if(i == 2) return true;
    if(i == 1) return hiitems[treasureType(l)] >= 10;
    return false;
    }
  
  switch(l) {
    case laIce: case laJungle: case laCaves: case laDesert: 
    case laMotion: case laCrossroads: 
      return true;
    
    case laMirror: case laMinefield: case laAlchemist: case laPalace:
    case laOcean: case laLivefjord:
      return gold() >= 30;
    
    case laCaribbean: case laWhirlpool:
      return exploreland[0][laOcean] || items[itCoast] || items[itStatue];
    
    case laRlyeh: case laDryForest: case laWineyard: case laCrossroads2:
      return gold() >= 60;
    
    case laDeadCaves:
      return gold() >= 60 && items[itGold] >= 10;

    case laGraveyard:
      return tkills() >= 100;

    case laHive:
      return tkills() >= 100 && gold() >= 60;

    case laRedRock:
      return gold() >= 60 && items[itSpice] >= 10;
    
    case laEmerald:
      return (items[itFernFlower] >= 5 && items[itGold] >= 5) || kills[moVizier];
    
    case laCamelot:
      return items[itEmerald] >= 5;
    
    case laHell: case laCrossroads3:
      return hellUnlocked();
      
    case laCocytus: case laPower: 
      return items[itHell] >= 10;

    case laTemple:
      return items[itStatue] >= 5;

    case laEdge: return gold() >= 60;
    case laZebra: return gold() >= 30 && items[itFeather] >= 10;

    case laEAir: case laEEarth: case laEWater: case laEFire:  case laElementalWall:
      return 
        kills[moFireElemental] || kills[moWaterElemental] ||
        kills[moEarthElemental] || kills[moAirElemental];        
    
    case laBarrier: case laNone: case laOceanWall: case laCanvas:
      return false;

    case laPrincessQuest: return kills[moVizier];
    }
  return false;
  }

int numplayers() {
  if(shmup::on) return shmup::players;
  return 1;
  }

cell *playerpos(int i) {
  if(!shmup::on) return cwt.c;
  return shmup::playerpos(i);
  }

bool isPlayerOn(cell *c) {
  if(!shmup::on) return c == cwt.c;
  for(int i=0; i<numplayers(); i++) if(playerpos(i) == c) return true;
  return false;
  }

bool isPlayerInBoatOn(cell *c) {
  if(!shmup::on) return c == cwt.c && (c->wall == waBoat || c->wall == waStrandedBoat);
  for(int i=0; i<numplayers(); i++) 
    if(playerpos(i) == c && (
      c->wall == waBoat || c->wall ==  waStrandedBoat || shmup::playerInBoat(i)
      )) 
    return true;
  return false;
  }
  
bool playerInPower() {
  if(!shmup::on) return cwt.c->land == laPower;
  for(int i=0; i<numplayers(); i++) if(playerpos(i)->land == laPower)
    return true;
  return false;
  }

eItem localTreasureType() {
  lastland = cwt.c->land;
  return treasureType(lastland);
  }

void countLocalTreasure() {
  eItem i = localTreasureType();
  currentLocalTreasure = i ? items[i] : 0;
  }

int orbsUnlocked() {
  int i = 0;
  for(int t=0; t<ittypes; t++) 
    if(itemclass(eItem(t)) == IC_TREASURE && items[t] >= 10)
      i++;
  return i;
  }

bool hellUnlocked() {
  return orbsUnlocked() >= 9;
  }

void countHyperstoneQuest(int& i1, int& i2) {
  i1 = 0; i2 = 0;
  for(int t=1; t<ittypes; t++) 
    if(t != itHyperstone && itemclass(eItem(t)) == IC_TREASURE) {
      i2++; if(items[t] >= 10) i1++;
      }
  }  

bool hyperstonesUnlocked() {
  int i1, i2;
  countHyperstoneQuest(i1, i2);
  return i1 == i2;
  }

bool markOrb(eItem it) {
  if(!items[it]) return false;
  orbused[it] = true;
  return true;
  }

bool markOrb2(eItem it) {
  if(!items[it]) return false;
  orbused[it] = true;
  return items[it] > 1;
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

int tkills() {
  return 
    kills[moYeti] + kills[moWolf] + 
    kills[moRanger] + kills[moTroll] + kills[moGoblin] +
    kills[moWorm] + kills[moDesertman] + kills[moIvyRoot] +
    kills[moMonkey] + kills[moEagle] + kills[moSlime] + kills[moSeep] +
    kills[moRunDog] + 
    kills[moCultist] + kills[moTentacle] + kills[moPyroCultist] + 
    kills[moLesser] + kills[moGreater] + 
    kills[moZombie] + kills[moGhost] + kills[moNecromancer] +
    kills[moHedge] + kills[moFireFairy] +
    kills[moCrystalSage] + kills[moShark] + kills[moGreaterShark] +
    kills[moMiner] + kills[moFlailer] + kills[moLancer] +
    kills[moVineSpirit] + kills[moVineBeast] +
    kills[moBug0] + kills[moBug1] + kills[moBug2] +
    kills[moDarkTroll] + kills[moEarthElemental] +
    kills[moWitch] + kills[moEvilGolem] + kills[moWitchFlash] + kills[moWitchFire] +
    kills[moWitchWinter] + kills[moWitchSpeed] +
    kills[moCultistLeader] +
    kills[moPirate] + kills[moCShark] + kills[moParrot] +
    kills[moHexSnake] + kills[moRedTroll] +
    kills[moPalace] + kills[moSkeleton] + kills[moFatGuard] + kills[moVizier] +
    kills[moViking] + kills[moFjordTroll] + kills[moWaterElemental] +
    kills[moAlbatross] +
    kills[moAirElemental] + kills[moFireElemental] +
    kills[moGargoyle] + kills[moEdgeMonkey] + kills[moOrangeDog]
    ;
  }

// monster/wall types

bool isFire(cell *w) {
  return w->wall == waFire || w->wall == waPartialFire || w->wall == waEternalFire;
  }

bool isThumper(eWall w) {
  return w == waThumperOff || w == waThumperOn;
  }

bool isThumper(cell *c) {
  return isThumper(c->wall);
  }

bool isActivable(cell *c) {
  return c->wall == waThumperOff || c->wall == waBonfireOff;
  }

bool hasTimeout(cell *c) {
  return c->wall == waThumperOn || c->wall == waFire || c->wall == waPartialFire ||
    c->wall == waTempWall || c->wall == waTempFloor || c->wall == waTempBridge;
  }

bool isMimic(eMonster m) {
  return m == moMirror || m == moMirage;
  }

bool isMimic(cell *c) { 
  return isMimic(c->monst);
  }

bool isPrincess(eMonster m) {
  return 
    m == moPrincess || m == moPrincessMoved ||
    m == moPrincessArmed || m == moPrincessArmedMoved;
  }

bool isGolemOrKnight(eMonster m) {
  return 
    m == moGolem  || m == moGolemMoved || 
    m == moKnight || m == moKnightMoved ||
    m == moTameBomberbird  || m == moTameBomberbirdMoved ||
    m == moMouse || m == moMouseMoved ||
    isPrincess(m);
  }

bool isGolemOrKnight(cell *c) { return isGolemOrKnight(c->monst); }

bool isNonliving(eMonster m) {
  return 
    m == moMirage || m == moMirror || m == moGolem || m == moGolemMoved ||
    m == moZombie || m == moGhost || m == moShadow || m == moSkeleton ||
    m == moEvilGolem || m == moIllusion || m == moEarthElemental || 
    m == moWaterElemental;
  }

bool isStunnable(eMonster m) {
  return m == moPalace || m == moFatGuard || m == moSkeleton || isPrincess(m);
  }

bool hasHitpoints(eMonster m) {
  return m == moPalace || m == moFatGuard || m == moVizier || isPrincess(m);
  }

bool isFriendly(eMonster m) {
  return isMimic(m) || isGolemOrKnight(m) || m == moIllusion;
  }

bool isFriendly(cell *c) { return isFriendly(c->monst); }

bool isBug(eMonster m) {
  return m >= moBug0 && m < moBug0+BUGCOLORS;
  }

bool isBug(cell *c) {
  return isBug(c->monst);
  }

bool isKillable(cell *c);
bool isKillableSomehow(cell *c);

bool isFriendlyOrBug(cell *c) { // or killable discord!
  // do not attack the stunned Princess
  if(isPrincess(c->monst) && c->stuntime) return false;
  return isFriendly(c) || isBug(c) || (c->monst && markOrb(itOrbDiscord) && isKillable(c) && !c->stuntime);
  }

bool isFriendlyOrBugS(cell *c) { // or killable discord!
  // do not attack the stunned Princess
  if(isPrincess(c->monst) && c->stuntime) return false;
  return isFriendly(c) || isBug(c) || (c->monst && markOrb(itOrbDiscord) && isKillableSomehow(c) && !c->stuntime);
  }

bool isIvy(eMonster m) {
  return m == moIvyRoot || m == moIvyHead || m == moIvyBranch || m == moIvyWait ||
    m == moIvyNext || m == moIvyDead;
  }

bool isIvy(cell *c) {
  return isIvy(c->monst);
  }

bool isDemon(eMonster m) {
  return m == moLesser || m == moLesserM || m == moGreater || m == moGreaterM;
  }

bool isDemon(cell *c) {
  return isDemon(c->monst);
  }

bool isWorm(eMonster m) {
  return m == moWorm || m == moWormtail || m == moWormwait || 
    m == moTentacle || m == moTentacletail || m == moTentaclewait ||
    m == moTentacleEscaping || m == moTentacleGhost ||
    m == moHexSnake || m == moHexSnakeTail;
  }

bool isWorm(cell *c) {
  return isWorm(c->monst);
  }

bool isWitch(eMonster m) {
  // evil golems don't count
  return m >= moWitch && m < moWitch+NUMWITCH-1;
  }

bool isOnCIsland(cell *c) {
  return (c->wall == waCIsland || c->wall == waCTree || c->wall == waCIsland2);
  }

bool cellUnstable(cell *c) {
  return (c->land == laMotion && c->wall == waNone) || c->wall == waTrapdoor;
  }

bool ignoresPlates(eMonster m) {
  return m == moMouse;
  }

bool cellUnstableOrChasm(cell *c) {
  return 
    (c->land == laMotion && c->wall == waNone) || 
    c->wall == waChasm || c->wall == waTrapdoor;
  }

bool cellHalfvine(cell *c) {
  return c->wall == waVineHalfA || c->wall == waVineHalfB;
  }

bool thruVine(cell *c, cell *c2) {
  return cellHalfvine(c) && c2->wall == c->wall && c2 != c;
  }

// === MOVEMENT FUNCTIONS ===

bool againstWind(cell *c2, cell *c1);

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
  if(c->land == laEdge) return false;
  return
    (c->wall == waNone && c->land != laMotion && c->land != laZebra) || 
    c->wall == waFloorA || c->wall == waFloorB ||
    c->wall == waCavefloor || c->wall == waFrozenLake ||
    c->wall == waDeadfloor || c->wall == waCIsland || c->wall == waCIsland2 ||
    c->wall == waMineUnknown || c->wall == waMineMine || c->wall == waMineOpen ||
    c->wall == waBonfireOff || c->wall == waFire || c->wall == waPartialFire;
  }

void placeWater(cell *c, cell *c2) {
  if(isWatery(c)) ;
  else if(c2 && (c2->wall == waFloorA || c2->wall == waFloorB))
    c->wall = c2->wall;
  else if(isIcyLand(c))
    c->wall = waLake;
  else
    c->wall = waSea;
  }

bool realred(eWall w) {
  return w == waRed1 || w == waRed2 || w == waRed3;
  }

int snakelevel(eWall w) {
  if(w == waRed1 || w == waDeadfloor2 || w == waRubble || w == waGargoyleFloor || 
    w == waGargoyleBridge || w == waTempFloor || w == waTempBridge) 
    return 1;
  if(w == waRed2) return 2;
  if(w == waRed3) return 3;
  return 0;
  }

int snakelevel(cell *c) { return snakelevel(c->wall); }

int incline(cell *cfrom, cell *cto) {
  return snakelevel(cto) - snakelevel(cfrom);
  }

bool inclineInRange(cell *cfrom, cell *cto) {
  int i = incline(cfrom, cto);
  return i <= 1 && i >= -2;
  }

bool cellEdgeUnstable(cell *c);
int coastval(cell *c);

bool passable(cell *w, cell *from, bool monster_passable, bool mirror_passable, bool revdir) {

  if(from && !(revdir ? inclineInRange(w, from) : inclineInRange(from, w))) return false;

  if(from && cellEdgeUnstable(w) && cellEdgeUnstable(from) && 
    coastval(w) != coastval(from) + (revdir?1:-1)) return false;
  
  if(from && (revdir ? againstWind(from,w) : againstWind(w, from))) return false;
  
  if(revdir && from && w->monst && passable(from, w, false, mirror_passable, false))
    return true;
  
  if(w->wall == waFloorA && from && from->wall == waFloorB && !w->item && !from->item)
    return false;
  if(w->wall == waFloorB && from && from->wall == waFloorA && !w->item && !from->item)
    return false;
  if(w->wall == waMirror || w->wall == waCloud) return mirror_passable;

  if(from && thruVine(from, w)) return false;
  if(w->wall == waNone || w->wall == waFloorA || w->wall == waFloorB || 
    w->wall == waCavefloor || w->wall == waFrozenLake || w->wall == waVineHalfA ||
    w->wall == waVineHalfB || w->wall == waDeadfloor || w->wall == waDeadfloor2 ||
    w->wall == waRubble || w->wall == waGargoyleFloor || w->wall == waGargoyleBridge ||
    w->wall == waTempFloor || w->wall == waTempBridge ||
    w->wall == waBoat || w->wall == waCIsland || w->wall == waCIsland2 || 
    w->wall == waRed1 || w->wall == waRed2 || w->wall == waRed3 ||
    w->wall == waMineUnknown || w->wall == waMineMine || w->wall == waMineOpen ||
    w->wall == waStrandedBoat || w->wall == waOpenGate || w->wall == waClosePlate ||
    w->wall == waOpenPlate || w->wall == waTrapdoor || w->wall == waGiantRug ||
    w->wall == waLadder) {
    if(w->monst) return monster_passable;
    return true;
    }
  return false;
  }

bool blowable0(cell *c2, cell *c) {
  return 
    passable(c2, NULL, false, false, false) || 
    c2->wall == waCamelotMoat || isFire(c2) || c2->wall == waChasm ||
    c2->wall == waSea || c2->wall == waLake || c2->wall == waPartialFire ||
    c2->wall == waTrapdoor || c2->wall == waSulphur || c2->wall == waSulphurC;
  }

bool blowable(cell *c2, cell *c) {
  if(c2->monst) return false;
  return blowable0(c2, c);
  }

bool jumpable(cell *c2, cell *c) {
  if(c2->monst && c2->monst != moMouse) return false;
  return blowable0(c2, c);
  }

bool player_passable(cell *w, cell *from, bool mon);

bool jumpable2(cell *c, cell *c2) {
  if(!c->monst && c->wall == waNone && c->land == laEdge && c2->wall == waNone)
    return true;
  return player_passable(c, c2, false);
  }

extern bool haveair;
int airdir;

int airdist(cell *c) {
  if(!haveair) return 3;
  if(!c) return 3;
  if(c->monst == moAirElemental) 
    return 0;
  if(!blowable(c, c)) return 3;
  for(int i=0; i<c->type; i++) {
    cell *c2 = c->mov[i];
    if(c2 && c2->monst == moAirElemental) {
      airdir = c->spn[i] * 42 / c2->type;
      return 1;
      }
    }
  for(int i=0; i<c->type; i++) {
    cell *c2 = c->mov[i];
    if(!c2) continue;
    if(!blowable(c2, c)) continue;
    if(!blowable(c, c2)) continue;
    for(int i=0; i<c2->type; i++) {
      cell *c3 = c2->mov[i];
      if(c3 && c3->monst == moAirElemental) {
        airdir = c2->spn[i] * 42 / c3->type;
        return 2;
        }
      }
    }
  return 3;
  }

bool againstWind(cell *c2, cell *c1) {
  if(!c1 || !c2) return false;
  return airdist(c2) < airdist(c1);
  }

bool player_passable(cell *w, cell *from, bool mon) {
  if(w->monst && !isFriendly(w)) return false;
  if(isFire(w) && markOrb(itOrbWinter)) return true;
  if(isWatery(w) && !(from && isWatery(from) && againstCurrent(w, from)) && markOrb(itOrbFish)) return true;
  if(isWatery(w) && from && from->wall == waBoat) {
    if(againstCurrent(w, from))
      return !items[itOrbWater];
    return true;
    }
  if(w->wall == waRoundTable && from && from->wall != waRoundTable)
    return true;
  if(passable(w, from, mon, true, false)) return true;
  return markOrb(itOrbGhost);
  return false;
  }

bool isAngryBird(eMonster m) {
  return m == moEagle || m == moAlbatross || m == moBomberbird || m == moGargoyle ||
    m == moAirElemental;
  }

bool isBird(eMonster m) {
  return isAngryBird(m) || m == moTameBomberbird || m == moTameBomberbirdMoved;    
  }

bool normalMover(eMonster m) {
  return
    m == moYeti || m == moRanger || m == moGoblin || m == moTroll || m == moDesertman || 
    m == moMonkey || m == moZombie || m == moNecromancer || m == moCultist || 
    m == moLesser || m == moGreater || m == moRunDog || m == moPyroCultist || 
    m == moFireFairy || m == moCrystalSage || m == moHedge ||
    m == moVineBeast || m == moLancer || m == moFlailer ||
    m == moMiner || m == moDarkTroll || 
    (playerInPower() && (
      (isWitch(m) && m != moWitchGhost && m != moWitchWinter) || m == moEvilGolem
      )) ||
    m == moRedTroll || 
    m == moPalace || m == moFatGuard || m == moSkeleton || m == moVizier ||
    m == moFjordTroll || m == moEdgeMonkey ||
    m == moFireElemental || m == moOrangeDog;
  }

// eagles can go through lakes, chasms, and slime
// todo vines?
bool eaglepassable(cell *w, cell *from) {
  if(w->monst) return false;
  if(isPlayerOn(w)) return true;
  if(againstWind(w, from)) return false;
  return
    w->wall == waNone || w->wall == waFloorA || w->wall == waFloorB || 
    w->wall == waCavefloor || w->wall == waFrozenLake || w->wall == waLake ||
    w->wall == waDeadfloor || w->wall == waDeadfloor2 || w->wall == waCamelotMoat ||
    w->wall == waRubble || w->wall == waGargoyleFloor || w->wall == waGargoyleBridge ||
    w->wall == waTempFloor || w->wall == waTempBridge ||
    w->wall == waSulphur || w->wall == waSulphurC || w->wall == waChasm ||
    w->wall == waRed1 || w->wall == waRed2 || w->wall == waRed3 ||
    w->wall == waSea || w->wall == waCIsland || w->wall == waCIsland2 ||
    w->wall == waMineMine || w->wall == waMineOpen || w->wall == waMineUnknown ||
    w->wall == waClosePlate || w->wall == waOpenPlate || w->wall == waOpenGate ||
    w->wall == waTrapdoor || w->wall == waGiantRug || w->wall == waLadder;
  }

bool earthpassable(cell *c, cell *from) {
  // cannot go through Living Caves...
  if(c->wall == waCavefloor) return false;
  if(isPlayerOn(c)) return true;
  // but can dig through...
  if(c->wall == waDeadwall || c->wall == waDune || c->wall == waStone)
    return true;
  if(c->wall == waSea && c->land == laLivefjord)
    return true;
  return passable(c, from, true, false, false);
  }

// from-to

bool isGhost(eMonster m) {
  return m == moGhost || m == moTentacleGhost;
  }

bool ghostmove(eMonster m, cell* to, cell* from) {
  if(to->monst && !(to->monst == moTentacletail && isGhost(m)))
    return false;
  if((m == moWitchGhost || m == moWitchWinter) && to->land != laPower)
    return false;
  if(isGhost(m))
    for(int i=0; i<to->type; i++) 
      if(to->mov[i] && to->mov[i] != from && isGhost(to->mov[i]->monst))
        return false;
  if(isGhost(m) || m == moWitchGhost) return true;
  if(m == moGreaterShark) return isWatery(to);
  if(m == moWitchWinter) return isFire(to) || passable(to, from, false, false, false);
  if(isPlayerOn(to)) return true;
  return false;
  }

bool isGhostMover(eMonster m) {
  return m == moGhost || m == moGreaterShark || m == moTentacleGhost ||
    (playerInPower() && (m == moWitchGhost || m == moWitchWinter));
  }

bool isSlimeMover(eMonster m) {
  return
    m == moSlime || m == moSeep || m == moShark ||
    m == moVineSpirit || m == moCShark || m == moParrot;
  }

bool isBlowableMonster(eMonster m) {
  return m && !(
    isWorm(m) || isIvy(m) || isSlimeMover(m) || m == moGhost || m == moGreaterShark ||
    m == moWaterElemental || m == moWitchGhost || isMimic(m)
    );
  }
  
bool isSlimeMover(cell *c) {
  return isSlimeMover(c->monst);
  }
  
int slimegroup(cell *c) {
  if(c->wall == waCavewall || c->wall == waDeadwall)
    return 1;
  if(isWatery(c))
    return 2;
  if(c->wall == waFloorA)
    return 3;
  if(c->wall == waFloorB)
    return 4;
  if(c->wall == waVinePlant || cellHalfvine(c))
    return 5;
  if(c->wall == waCTree)
    return 6;
  return 0;
  }

bool slimepassable(cell *w, cell *c) {
  int u = dirfromto(c, w);
  if(w == c) return true;
  if(isPlayerOn(w)) return true;
  int group = slimegroup(c);
  if(!group) return false;
  int ogroup = slimegroup(w);
  if(!ogroup) return false;
  bool hv = (group == ogroup);
  
  // don't go against the current
  if(isWateryOrBoat(w) && isWateryOrBoat(c)) 
    return !againstCurrent(w, c);

  if(w->item) return false;

  // only travel to halfvines correctly
  if(cellHalfvine(c)) {
    int i=0;
    for(int t=0; t<c->type; t++) if(c->mov[t] && c->mov[t]->wall == c->wall) i=t;
    int z = i-u; if(z<0) z=-z; z%=6;
    if(z>1) return false;
    hv=true;
    }
  // only travel from halfvines correctly
  if(cellHalfvine(w)) {
    int i=0;
    for(int t=0; t<w->type; t++) if(w->mov[t] && w->mov[t]->wall == w->wall) i=t;
    int z = i-c->spn[u]; if(z<0) z=-z; z%=6;
    if(z>1) return false;
    hv=true;
    }
  if(!hv) return false;
  return true;
  }

bool canPushStatueOn(cell *c) {
  return passable(c, NULL, true, false, false) && c->wall != waBoat && !snakelevel(c);
  }

bool boatpassable(cell *c, cell *from) {
  return
    from->wall == waBoat && isWatery(c) && !againstCurrent(c, from);
  }

void moveBoat(cell *to, cell *from) {
  eWall x = to->wall; to->wall = from->wall; from->wall = x;
  to->mondir = neighborId(to, from);
  moveItem(from, to, false);
  }

void moveBoatIfUsingOne(cell *to, cell *from) {
  if(from->wall == waBoat && isWatery(to)) moveBoat(to, from);
  }

bool leaderpassable(cell *c, cell *from) {
  if(isPlayerOn(c)) return true;
  if(c->monst) return false;
  if(isWateryOrBoat(c) && isWateryOrBoat(from) && (c->wall != waBoat || from->wall != waBoat))
    return !againstCurrent(c, from);
  if(c->wall == waBigStatue) return canPushStatueOn(from);
  return passable(c, from, false, false, false);
  }

bool isLeader(eMonster m) {
  return m == moPirate || m == moCultistLeader || m == moViking;
  }

bool waterpassable(cell *w, cell *from) {
  return isWatery(w) || boatGoesThrough(w) || w->wall == waDeadTroll || isPlayerOn(w);
  }

bool passable_for(eMonster m, cell *w, cell *from, bool monster_passable) {
  if(w->monst && !monster_passable) 
    return false;
  if(m == moWolf) {
    return isIcyLand(w) && (isPlayerOn(w) || passable(w, from, monster_passable, false, false));
    }
  if(normalMover(m) || isBug(m) || isDemon(m)) {
    if((isWitch(m) || m == moEvilGolem) && w->land != laPower)
      return false;
    return isPlayerOn(w) || passable(w, from, monster_passable, false, false);
    }
  if(isSlimeMover(m))
    return slimepassable(w, from);
  if(m == moEarthElemental)
    return earthpassable(w, from);
  if(m == moWaterElemental) 
    return waterpassable(w, from);
  if(m == moGreaterShark)
    return isWatery(w) || w->wall == waBoat || w->wall == waFrozenLake;
  if(isGhostMover(m))
    return ghostmove(m, w, from);
    // for the purpose of Shmup this is correct
  if(isBird(m))
    return eaglepassable(w, from);
  if(isLeader(m))
    return leaderpassable(w, from);
  if(isPrincess(m))
    return passable(w, from, monster_passable, false, false) || boatpassable(w, from);
  if(isGolemOrKnight(m))
    return passable(w, from, monster_passable, false, false);
  return false;
  }

eMonster movegroup(eMonster m) {
  if(isWitch(m) || m == moEvilGolem) {
    if(m == moWitchGhost) return moWitchGhost;
    if(m == moWitchWinter) return moWitchWinter;
    return moWitch;
    }
  if(normalMover(m)) return moYeti;
  if(isSlimeMover(m)) return moSlime;
  if(m == moEarthElemental) return moEarthElemental;
  if(isLeader(m)) return moPirate;
  if(isAngryBird(m)) return moEagle;
  if(isBird(m)) return moTameBomberbird;
  if(m == moGhost) return moGhost;
  if(m == moGreaterShark) return moGreaterShark;
  if(m == moWolf) return moWolf;
  if(isDemon(m)) return moLesser;
  if(isBug(m)) return m;
  if(m == moWaterElemental) return moWaterElemental;
  return moNone;
  }

bool itemBurns(eItem it) {
  return it && it != itOrbDragon && it != itOrbFire;
  }

bool attackThruVine(eMonster m) {
  return m == moGhost || m == moVineSpirit;
  }

// target, source
bool attackingForbidden(cell *c, cell *c2) {
  return thruVine(c, c2) && !attackThruVine(c2->monst) && !attackThruVine(c->monst);
  }

void useup(cell *c) {
  c->wparam--;
  if(c->wparam == 0) {
    if(c->wall == waTempFloor)
      c->wall = waChasm;
    else if(c->wall == waTempBridge)
      placeWater(c, c);
    else 
      c->wall = c->land == laCaribbean ? waCIsland2 : waNone;
    }
  }

bool isInactiveEnemy(cell *w, bool forpc) {
  if(w->monst == moWormtail || w->monst == moWormwait || w->monst == moTentacletail || w->monst == moTentaclewait || w->monst == moHexSnakeTail)
    return true;
  if(w->monst == moLesserM || w->monst == moGreaterM)
    return true;
  if(w->monst == moIvyRoot || w->monst == moIvyWait || w->monst == moIvyNext || w->monst == moIvyDead)
    return true;
  if((forpc ? w->stuntime : w->stuntime > 1) && !isFriendly(w))
    return true;
  return false;
  }

// forpc = true (for PC), false (for golems)
bool isActiveEnemy(cell *w, cell *killed, bool forpc) {
  if((forpc ? w->stuntime : w->stuntime > 1))
    return false;
  if(w->monst == moNone || w == killed) return false;
  if(isFriendly(w)) return false;
  if(isInactiveEnemy(w, forpc)) return false;
  if(w->monst == moIvyHead || w->monst == moIvyBranch) {
    while(w != killed && w->mondir != NODIR) w = w->mov[w->mondir];
    return w != killed;
    }
  return true;
  }

bool isUnarmed(eMonster m) {
  return 
    m == moMouse || m == moMouseMoved || m == moPrincess || m == moPrincessMoved ||
    m == moCrystalSage;   
  }

bool isArmedEnemy(cell *w, cell *killed, bool forpc) {
  return w->monst != moCrystalSage && isActiveEnemy(w, killed, forpc);
  }

bool isHive(eLand l) {
  return l == laHive;
  }
  
bool isKillable(cell *c) {
  return c->monst != moShadow && (!isWorm(c) || c->monst == moTentacleGhost) && c->monst != moGreater && c->monst != moGreaterM
    && c->monst != moHedge && c->monst != moFlailer && !(c->monst == moVizier && c->hitpoints > 1);
    // && !isBug(c->monst);
  }

bool isKillableSomehow(cell *c) {
  return isKillable(c) 
    || c->monst == moHedge || c->monst == moLancer || c->monst == moFlailer ||
    c->monst == moVizier;
  }

bool isNeighbor(cell *c1, cell *c2) {
  for(int i=0; i<c1->type; i++) if(c1->mov[i] == c2) return true;
  return false;
  }

int neighborId(cell *c1, cell *c2) {
  for(int i=0; i<c1->type; i++) if(c1->mov[i] == c2) return i;
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

// friendly==false: would the flash kill enemy monsters (i.e., allied with the witch)?
// friendly==true: would the flash kill friendly monsters (or bugs)?
bool flashWouldKill(cell *c, bool friendly) {
  for(int t=0; t<c->type; t++) {
    cell *c2 = c->mov[t];
    for(int u=0; u<c2->type; u++) {
      cell *c3 = c2->mov[u];
      if(isWorm(c3)) continue; // immune to Flash
      if(c3->monst == moEvilGolem) continue; // evil golems don't count
      if(c3 != c && c3->monst) {
        bool b = isFriendlyOrBugS(c3);
        if(friendly ? b: !b) return true;
        }
      }
    }
  return false;
  }

int monstersnear(cell *c, cell *nocount = NULL, eMonster who = moNone) {
  if(hardcore && !who) return 0;
  int res = 0;
  bool fast = false;

  if(!who) {
    fast = (items[itOrbSpeed] && !(items[itOrbSpeed] & 1));
    }
  
  for(int t=0; t<c->type; t++) {
    cell *c2 = c->mov[t];

    // consider monsters who attack from distance 2
    if(c2)
    if(!thruVine(c, c2)) for(int u=2; u<=c2->type-2; u++) {
      cell *c3 = c2->mov[(c->spn[t]+u) % c2->type];
      if(!c3) continue;
      // only these monsters can attack from two spots...
      if(c3->monst != moLancer && c3->monst != moWitchSpeed && c3->monst != moWitchFlash) continue;
      // speedwitches can only attack not-fastened monsters,
      // others can only attack if the move is not fastened
      if(c3->monst == moWitchSpeed && items[itOrbSpeed]) continue;
      if(c3->monst != moWitchSpeed && fast) continue;
      // cannot attack if the immediate cell is impassable (except flashwitches)
      if(c3->monst != moWitchFlash) {
        if(!passable(c2, c3, !isArmedEnemy(c2, nocount, true), false, false)) continue;
        if(isPlayerOn(c2) && items[itOrbFire]) continue;
        }
      // flashwitches cannot attack if it would kill another enemy
      if(c3->monst == moWitchFlash && flashWouldKill(c3, false)) continue;
      if(nocount && mirrorkill(c3)) continue;
      res++, which = c3->monst;
      } 

    // consider normal monsters
    if(c2 && isArmedEnemy(c2, nocount, !who) && (c2->monst != moLancer || isUnarmed(who))) {
      if(fast && c2->monst != moWitchSpeed) continue;
      // they cannot attack through vines
      if(attackingForbidden(c, c2))
        continue;
      // do not count if it would be killed by a mimic
      if(nocount && mirrorkill(c2)) continue;
      // do not include stabbed enemies
      if(
        (c2->monst == moHedge || (isKillable(c2) && items[itOrbThorns]))
        && c2->cpdist == 1 && !isPlayerOn(c))
        continue;
      res++, which = c2->monst;
      }
    }

  if(!who && res && markOrb2(itOrbShield)) 
    res = 0;

  return res;
  }
        
// reduce c->mpdist to d; also generate the landscape

bool buggyGeneration = false;


bool checkBarriersBack(cellwalker bb, int q=5, bool cross = false);

bool checkBarriersFront(cellwalker bb, int q=5, bool cross = false) {

  if(bb.c->mpdist < BARLEV) return false;
  if(bb.c->mpdist == BUGLEV) return false;
  if(bb.c->bardir != NODIR) return false;
  if(bb.spin == 0) {q--; if(!q) return true; }

  if(!cross) for(int i=0; i<7; i++) {
    cellwalker bb2 = bb;
    cwspin(bb2, i); cwstep(bb2); 
    if(bb2.c->bardir != NODIR) return false;
    cwspin(bb2, 4); cwstep(bb2);
    if(bb2.c->bardir != NODIR) return false;
    }

  cwstep(bb); cwspin(bb, 3); cwstep(bb); cwspin(bb, 3); cwstep(bb);
  return checkBarriersBack(bb, q);
  }

bool checkBarriersBack(cellwalker bb, int q, bool cross) {
  // printf("back, %p, s%d\n", bb.c, bb.spin);
  
  if(bb.c->mpdist < BARLEV) return false;
  if(bb.c->mpdist == BUGLEV) return false;
  if(bb.c->bardir != NODIR) return false;
  
  // if(bb.spin == 0 && bb.c->mpdist == INFD) return true;
  
  if(!cross) for(int i=0; i<7; i++) {
    cellwalker bb2 = bb;
    cwspin(bb2, i); cwstep(bb2); 
    if(bb2.c->bardir != NODIR) return false;
    cwspin(bb2, 4); cwstep(bb2);
    if(bb2.c->bardir != NODIR) return false;
    }

  cwspin(bb, 3); cwstep(bb); cwspin(bb, 4); 
  // bool create = cwstepcreates(bb);
  cwstep(bb); cwspin(bb, 3); 
  // if(create && bb.spin == 0) return true;
  return checkBarriersFront(bb, q);
  }

bool isSealand(eLand l) {
  return l == laOcean || l == laCaribbean || l == laWhirlpool || l == laLivefjord ||
    l == laOceanWall;
  }

bool isElemental(eLand l) {
  return l == laEAir || l == laEWater || l == laEEarth || l == laEFire ||
    l == laElementalWall;
  }

eWall getElementalWall(eLand l) {
  if(l == laEAir) return waChasm;
  if(l == laEEarth) return waStone;
  if(l == laEFire) return waEternalFire;
  if(l == laEWater) return waSea;
  return waNone;
  }

void setbarrier(cell *c) {
  if(isSealand(c->barleft) && isSealand(c->barright)) {
    c->wall = c->type == 7 ? waBarrier : waSea; 
    c->land = laOceanWall;
    }
  else if(isElemental(c->barleft) && isElemental(c->barright)) {
    c->land = laElementalWall;
    c->wall = getElementalWall(c->barleft);
    }
  else {
    c->wall = waBarrier;
    c->land = laBarrier;
    }
/*if(isHive(c->barleft) && isHive(c->barright))
    c->wall = waWaxWall, c->land = c->barleft; */
  }

void killIvy(cell *c) {
  if(c->monst == moIvyDead) return;
  for(int i=0; i<c->type; i++) if(c->mov[i])
    if(isIvy(c->mov[i]) && c->mov[i]->mondir == c->spn[i])
      killIvy(c->mov[i]);
  c->monst = moIvyDead;
  }

int buildIvy(cell *c, int children, int minleaf) {
  c->mondir = NODIR;
  c->monst = moIvyRoot;
  
  cell *child = NULL;

  int leaf = 0;
  int leafchild = 0;
  for(int i=0; i<c->type; i++) {
    createMov(c, i);
    if(passable(c->mov[i], c, false, false, false)) {
      if(children && !child) 
        child = c->mov[i], leafchild = buildIvy(c->mov[i], children-1, 5);
      else 
        c->mov[i]->monst = (leaf++) ? moIvyWait : moIvyHead,
        c->mov[i]->mondir = c->spn[i];
      }
    }
  
  leaf += leafchild;
  if(leaf < minleaf) {
    if(child) killIvy(child);
    killIvy(c);
    return 0;
    }
  else return leaf;
  }

bool isIcyWall(cell *c) {
  return c->wall == waNone || c->wall == waIcewall || c->wall == waFrozenLake || c->wall == waLake;
  }

void prespill(cell* c, eWall t, int rad) {
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
    c->wall = waNone; killMonster(c);
    }
  // these walls block spilling completely
  if(c->wall == waIcewall || c->wall == waBarrier || c->wall == waDeadTroll ||
    c->wall == waDune || c->wall == waAncientGrave || 
    c->wall == waThumperOff || c->wall == waThumperOn ||
    c->wall == waFreshGrave || c->wall == waColumn || c->wall == waPartialFire ||
    c->wall == waDeadwall || c->wall == waWaxWall || c->wall == waCamelot || c->wall == waRoundTable ||
    c->wall == waBigStatue || c->wall == waRed1 || c->wall == waRed2 || c->wall == waRed3 ||
    c->wall == waPalace || c->wall == waOpenGate || c->wall == waClosedGate ||
    c->wall == waPlatform || c->wall == waStone || c->wall == waTempWall ||
    c->wall == waTempFloor || c->wall == waTempBridge)
    return;
  // these walls block further spilling
  if(c->wall == waCavewall || cellUnstable(c) || c->wall == waSulphur ||
    c->wall == waSulphurC || c->wall == waLake || c->wall == waChasm ||
    c->wall == waDryTree || c->wall == waWetTree || c->wall == waTemporary ||
    c->wall == waVinePlant || isFire(c) || c->wall == waBonfireOff || c->wall == waVineHalfA || c->wall == waVineHalfB ||
    c->wall == waCamelotMoat || c->wall == waSea || c->wall == waCTree ||
    c->wall == waRubble || c->wall == waGargoyleFloor || c->wall == waGargoyle)
      t = waTemporary;

  if(c->wall == waSulphur) {
    // remove the center as it would not look good
    for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->wall == waSulphurC)
      c->mov[i]->wall = waSulphur;
    }

  destroyHalfvine(c);
  c->wall = t;
  // destroy items...
  c->item = itNone;
  // block spill
  if(t == waTemporary) return;
  // cwt.c->item = itNone;
  if(rad) for(int i=0; i<c->type; i++) if(c->mov[i])
    prespill(c->mov[i], t, rad-1);
  }

void spillfix(cell* c, eWall t, int rad) {
  if(c->wall == waTemporary) c->wall = t;
  if(rad) for(int i=0; i<c->type; i++) if(c->mov[i])
    spillfix(c->mov[i], t, rad-1);
  }

void spill(cell* c, eWall t, int rad) {
  prespill(c,t,rad); spillfix(c,t,rad);
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
  return false;
  }

bool earthWall(cell *c) {
  if(c->wall == waDeadfloor || c->wall == waDeadfloor2 || c->wall == waEarthD) {
    c->item = itNone;
    c->wall = waDeadwall;
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
  if(c->wall == waCIsland || c->wall == waCIsland2 || (c->wall == waNone && c->land == laOcean)) {
    c->item = itNone;
    c->wall = waSea;
    if(c->land == laOcean) c->landparam = 40;
    return true;
    }
  return false;
  }

void snakepile(cell *c) {
  if(c->wall == waRed1 || c->wall == waOpenGate) c->wall = waRed2;
  else if(c->wall == waRed2) c->wall = waRed3;
  else if(c->wall == waNone || c->wall == waFloorA || c->wall == waFloorB ||
    c->wall == waCIsland || c->wall == waCIsland2 ||
    (c->wall == waSea && c->land == laOcean) ||
    (c->wall == waSea && c->land == laLivefjord) ||
    c->wall == waOpenPlate || c->wall == waClosePlate ||
    c->wall == waMineUnknown || c->wall == waMineOpen)
    c->wall = waRed1;
  else if(c->wall == waDeadfloor)
    c->wall = waDeadfloor2; 
  else if(c->wall == waDeadfloor2)
    c->wall = waDeadwall; 
  else if(c->wall == waRubble || c->wall == waGargoyleFloor || c->wall == waGargoyleBridge ||
    c->wall == waTempFloor || c->wall == waTempBridge)
    c->wall = waRed2; 
  else if(c->wall == waCavefloor) c->wall = waCavewall;
  else if(c->wall == waSea) c->wall = waCIsland;
  else if(isWateryOrBoat(c) || c->wall == waFrozenLake) c->wall = waNone;
  else if(isWateryOrBoat(c) || c->wall == waFrozenLake) c->wall = waNone;
  else if(cellHalfvine(c)) {
    destroyHalfvine(c, waRed1);
    }
  }

bool eternalFire(cell *c) {
  return c->land == laDryForest || c->land == laPower || c->land == laMinefield ||
    c->land == laEFire || c->land == laElementalWall;
  }
  
void explodeMine(cell *c);

bool makeflame(cell *c, int timeout, bool checkonly)  {
  if(itemBurns(c->item)) {
    if(checkonly) return true;
    addMessage(XLAT("%The1 burns!", c->item)), c->item = itNone;
    }
  if(cellUnstable(c)) {
    if(checkonly) return true;
    c->wall = waChasm;
    }
  else if(c->wall == waChasm || c->wall == waOpenGate || c->wall == waRed2 || c->wall == waRed3)
    return false;
  else if(c->wall == waBoat) {
    if(checkonly) return true;
    placeWater(c, c);
    addMessage(XLAT("%The1 burns!", winf[c->wall].name));
    if(isIcyLand(c)) HEAT(c) += 1;
    }
  else if(c->wall == waNone && c->land == laCocytus) {
    if(checkonly) return true;
    c->wall = waLake, HEAT(c) += 1;
    }
  else if(c->wall == waFrozenLake && c->land == laCocytus) {
    if(checkonly) return true;
    c->wall = waLake, HEAT(c) += 1;
    }
  else if(c->wall == waIcewall) {
    if(checkonly) return true;
    c->wall = waNone;
    }
  else if(c->wall == waMineMine) {
    if(checkonly) return true;
    explodeMine(c);
    }
  else if(c->wall != waCTree && c->wall != waDryTree && c->wall != waWetTree && 
    c->wall != waVinePlant && !passable(c, NULL, true, true, false)) return false;
  else {
    eWall w = eternalFire(c) ? waEternalFire : waFire;
    if(w == c->wall) return false;
    if(checkonly) return true;
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
  
  c->wall = waMineOpen;
  makeflame(c, 20, false);
  
  for(int i=0; i<c->type; i++) if(c->mov[i]) {
    cell *c2 = c->mov[i];
    if(c2->wall == waRed2 || c2->wall == waRed3)
      c2->wall = waRed1;
    else if(c2->wall == waDeadTroll || c2->wall == waGargoyle) {
      c2->wall = waNone;
      makeflame(c2, 10, false);
      }
    else if(c2->wall == waPalace || c2->wall == waOpenGate || c2->wall == waClosedGate) {
      c2->wall = waNone;
      makeflame(c2, 10, false);
      }
    else makeflame(c2, 20, false);
    }
  }

void stunMonster(cell *c2) {
  if(c2->monst != moSkeleton)
    c2->hitpoints--;
  c2->stuntime = (
    c2->monst == moFatGuard ? 2 : 
    c2->monst == moSkeleton && c2->land != laPalace ? 7 :
    3);
  }

bool attackJustStuns(cell *c2) {
  return isStunnable(c2->monst) && c2->hitpoints > 1;
  }
  
void killOrStunMonster(cell *c2) {
  if(attackJustStuns(c2))
    stunMonster(c2);
  else
    killMonster(c2);
  }

void moveEffect(cell *ct, cell *cf, eMonster m);
  
namespace princess {

#define EPX 39
#define EPY 21

#define OUT_OF_PRISON 200
#define OUT_OF_PALACE 250

  bool generating = false;
  bool challenge = false;
  bool saved = false;
  bool everSaved = false;
  bool everGotYendorVictory = false;
  
  bool forceVizier = false;
  bool forceMouse = false;

  int saveHP = 0, saveArmedHP = 0;
  
  struct info {
    int id;         // id of this info
    cell *prison;   // where was the Princess locked
    heptagon *alt;  // alt of the prison
    int bestdist;   // best dist achieved
    int bestnear;   // best dist achieved, by the player
    int value;      // number of Rugs at 120
    cell *princess; // where is the Princess currently
    };
    
  vector<info*> infos;
  
  void assign(info *i) {
    if(i->alt) i->alt->emeraldval = i->id;
    }
  
  void newInfo(cell *c) {
    info *i = new info;
    i->prison = c;
    i->princess = c;
    i->alt = c->master->alt;
    i->id = size(infos);
    i->bestdist = 0;
    i->bestnear = OUT_OF_PRISON;
    infos.push_back(i);
    assign(i);
    }
  
  void newFakeInfo(cell *c) {
    info *i = new info;
    i->prison = NULL;
    i->princess = c;
    i->alt = NULL;
    i->id = size(infos);
    i->bestdist = OUT_OF_PALACE;
    i->bestnear = 0;
    infos.push_back(i);
    assign(i);
    }
  
  info *getPrisonInfo(cell *c) {
    if(euclid) return NULL;
    if(c->land != laPalace) return NULL;
    if(!c->master->alt) return NULL;
    return infos[c->master->alt->emeraldval];
    }
  
  info *getPrincessInfo(cell *c) {
    for(int i=0; i<size(infos); i++) if(infos[i]->princess == c) {
      while(i) {
        infos[i]->id = i-1; assign(infos[i]);
        infos[i-1]->id = i; assign(infos[i-1]);
        i--;
        }
      return infos[i];
      }
    return NULL;
    }

  int dist(cell *c) {
    if(c->land != laPalace) return OUT_OF_PALACE;
    else if(euclid) return celldistAlt(c);
    else if(!c->master->alt) return OUT_OF_PRISON;
    else return celldistAlt(c);
    }
  
  void clear() {
    for(int i=0; i<size(infos); i++) delete infos[i];
    infos.clear();
    }
 
  bool bringBackAt(cell *c) {
    if(!c) return false;
    if(!passable(c, NULL, false, false, false)) return false;
    c->monst = moPrincessArmed;
    c->stuntime = 0;
    c->hitpoints = palaceHP();
    drawFlash(c);

    info *inf = NULL;
    for(int i=0; i<size(infos); i++) 
      if(infos[i]->princess && infos[i]->bestdist == OUT_OF_PALACE)
        inf = infos[i];

    if(inf) { inf->princess->monst = moNone; inf->princess = c; }
    else newFakeInfo(c);
    return true;
    }

  void bringBack() {
    if(bringBackAt(cwt.c->mov[cwt.spin])) return;
    for(int i=1; i<size(dcal); i++)
      if(bringBackAt(dcal[i])) return;
    }

  void move(cell *ct, cell *cf) {
    if(euclid) return;
    princess::info *i = princess::getPrincessInfo(cf);
    if(!i) {
      static bool warn = true;
      // note: OK if mapediting or loading
      if(warn) printf("Warning: unknown princess\n"); 
      warn = false;
      }
    else {
      i->princess = ct;
      int newdist = dist(ct);
      // printf("newdist = %d (vs %d)\n", newdist, i->bestdist);
      if(newdist < ALTDIST_ERROR && newdist > i->bestdist) {
        i->bestdist = newdist;
//      printf("Improved dist to %d\n", newdist);
        if(newdist == OUT_OF_PALACE) {
          if(!princess::saved)
            achievement_gain("PRINCESS1");
          princess::saved = true;
          princess::everSaved = false;
          items[itSavedPrincess]++;
          }
        if(newdist == OUT_OF_PRISON && princess::challenge) {
          addMessage(XLAT("Congratulations! Your score is %1.", its(i->value)));
          achievement_gain("PRINCESS2");
          if(!cheater) achievement_score(36, i->value);
          showMissionScreen();
          }
        }
      }
    }

  void mouseSqueak(cell *c) {
    eMonster m = c->monst;
    info *i = getPrisonInfo(c);
    int d = dist(c);

    if(!i)
       addMessage(XLAT("%The1 squeaks in a confused way.", m));
    else if(i->bestdist >= 6)
       addMessage(XLAT("%The1 squeaks gratefully!", m));
    else if(!i->princess)
       addMessage(XLAT("%The1 squeaks hopelessly.", m));
    else if(d > 120)
       addMessage(XLAT("%The1 squeaks in despair.", m));
    else if(d > 90)
       addMessage(XLAT("%The1 squeaks sadly.", m));
    else if(d > 60)
       addMessage(XLAT("%The1 squeaks with hope!", m));
    else if(d > 30)
       addMessage(XLAT("%The1 squeaks happily!", m));
    else
       addMessage(XLAT("%The1 squeaks excitedly!", m));
    }
  
  void line(cell *c) {
    int d = (euclid || c->master->alt) ? celldistAlt(c) : 200;
    eMonster m = c->monst;
  
    static int msgid = 0;
  
    retry:
    if(msgid >= 32) msgid = 0;  
    
    if(msgid == 0 && d < 20 && c->land == laPalace) {
      addMessage(XLAT("%The1 kisses you, and begs you to bring %him1 away from here.", m));
      }
    else if(msgid == 1 && d >= 20 && c->land == laPalace) {
      if(m == moPrincess)
        addMessage(XLAT("\"I want my revenge. Stun a guard and leave him for me!\"", m));
      else
        addMessage(XLAT("\"That felt great. Thanks!\"", m));
      }
    else if(msgid  == 2 && d >= 70 && c->land == laPalace) {
      addMessage(XLAT("\"Bring me out of here please!\"", m));
      }
    else if(msgid == 3 && c->land != laPalace) {
      addMessage(XLAT("%The1 kisses you, and thanks you for saving %him1.", m));
      }
    else if(msgid == 4 && c->land != laPalace && m == moPrincess) {
      addMessage(XLAT("\"I have been trained to fight with a Hypersian scimitar, you know?\"", m));
      }
    else if(msgid == 5 && c->land != laPalace) {
      addMessage(XLAT("\"I would love to come to your world with you!\"", m));
      }
    else if(msgid == 6 && c->land != laPalace) {
      addMessage(XLAT("\"Straight lines stay close to each other forever, this is so romantic!\"", m));
      }
    else if(msgid == 7 && c->land != laPalace) {
      addMessage(XLAT("\"Maps... Just like the world, but smaller... how is that even possible?!\"", m));
      }    
    else {
      msgid++; goto retry;
      }
  
    msgid++;
    }

  void playernear(cell *c) {
    info *i = getPrisonInfo(c);
    int d = dist(c);
    // if(i) printf("d=%d bn=%d\n", d, i->bestnear);
    if(i && d < i->bestnear) {
      if(i->bestnear > 100 && d <= 100) {
        i->value = items[itPalace];
        if(princess::challenge) 
          addMessage(XLAT("Hardness frozen at %1.", its(i->value)));
        }
      i->bestnear = d;
      }
    }
    
  }

bool grailWasFound(cell *c) {
  if(euclid) return items[itHolyGrail];
  return c->master->alt->alt->emeraldval & GRAIL_FOUND;
  }

int euclidAlt(short x, short y) {
  if(euclidland == laTemple) {
    return max(int(x), x+y);
    }
  else if(euclidland == laCaribbean || euclidland == laWhirlpool) {
    return 
      min(
        min(max(int(-x), -x-y) + 3,
        max(int(x+y), int(y)) + 3),
        max(int(x), int(-y)) + 3
        );
    }
  else if(euclidland == laPrincessQuest)
    return eudist(x-EPX, y-EPY);
  else return eudist(x-20, y-10);
  }

void flameHalfvine(cell *c, int val) {
  if(itemBurns(c->item)) {
    c->item = itNone;
    addMessage(XLAT("%The1 burns!", c->item)), c->item = itNone;
    }
  c->wall = waPartialFire;
  c->wparam = val;
  }

void killMonster(cell *c) {
  DEB("killmonster");
  eMonster m = c->monst;
  
  if(!m) return;
  if(isWorm(c) && m != moTentacleGhost) return;
  if(m == moShadow) return;
  if(m == moGolemMoved) m = moGolem;
  if(m == moKnightMoved) m = moKnight;
  if(m == moSlimeNextTurn) m = moSlime;
  if(m == moLesserM) m = moLesser;
  if(m == moGreater) m = moLesser;
  if(m == moGreaterM) m = moLesser;
  if(isPrincess(m)) m = moPrincess;
  if(m == moTentacleGhost) m = moGhost;
  kills[m]++;
  
  if(m == moPrincess) {
    princess::info *i = princess::getPrincessInfo(c);
    if(i) {
      i->princess = NULL;
      if(i->bestdist == OUT_OF_PALACE) items[itSavedPrincess]--;
      if(princess::challenge) showMissionScreen();
      }
    }

  if(m == moGargoyle && c->wall != waMineMine) {
    bool connected = false;
    
    if(c->land == laEdge) {
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if(c2->wall == waPlatform || c2->wall == waGargoyle || c2->wall == waBarrier ||
          c2->wall == waDeadTroll)
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
      destroyHalfvine(c);
      if(cellUnstableOrChasm(c)) c->wall = waGargoyleFloor;
      else if(isWatery(c)) c->wall = waGargoyleBridge;
      else c->wall = waGargoyle;
      c->item = itNone;
      }
    }
    
  if(m == moTroll || m == moFjordTroll) {
    destroyHalfvine(c);
    c->wall = cellUnstableOrChasm(c) ? waChasm : waDeadTroll;
    c->item = itNone;
    for(int i=0; i<c->type; i++) if(c->mov[i]) {
      if(m == moTroll) c->mov[i]->item = itNone;
      if(c->mov[i]->wall == waDeadwall || c->mov[i]->wall == waDeadfloor2) c->mov[i]->wall = waCavewall;
      if(c->mov[i]->wall == waDeadfloor) c->mov[i]->wall = waCavefloor;
      }
    }
  if(m == moMiner) {
    destroyHalfvine(c);
    c->wall = cellUnstableOrChasm(c) ? waChasm : waNone;
    for(int i=0; i<c->type; i++) if(passable(c->mov[i], c, true, true, false)) {
      destroyHalfvine(c->mov[i]);
      c->mov[i]->wall = cellUnstableOrChasm(c) ? waChasm : waNone;
      if(c->mov[i]->monst == moSlime || c->mov[i]->monst == moSlimeNextTurn)
        killMonster(c->mov[i]);
      }
    }
  if(m == moVineBeast) {
    destroyHalfvine(c);
    c->wall = cellUnstableOrChasm(c) ? waChasm : waVinePlant;
    c->item = itNone;
    }
  if(isBird(m)) moveEffect(c, c, moDeadBird);
  if(m == moBomberbird || m == moTameBomberbird) {
    if(c->wall == waNone || c->wall == waMineUnknown || c->wall == waMineOpen ||
      c->wall == waCavefloor || c->wall == waDeadfloor || c->wall == waDeadfloor2 ||
      c->wall == waRubble || c->wall == waGargoyleFloor || 
      c->wall == waCIsland || c->wall == waCIsland2 || 
      c->wall == waStrandedBoat || c->wall == waRed1 || c->wall == waGiantRug)
      c->wall = waMineMine;
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
    if(isFire(c)) {
      c->wall = waMineMine;
      explodeMine(c);
      }
    }
  if(m == moVineSpirit) {
    destroyHalfvine(c);
    if(!isFire(c)) c->wall = waNone;
    }
  if(m == moRedTroll) snakepile(c);
  if(isWitch(m) && (isFire(c) || passable(c, NULL, true, false, false)) && !c->item) {
    if(m == moWitchFire) c->item = itOrbFire;
    if(m == moWitchFlash) c->item = itOrbFlash;
    if(m == moWitchGhost) c->item = itOrbGhost;
    if(m == moWitchWinter) c->item = itOrbWinter;
    if(m == moWitchSpeed) c->item = itOrbSpeed;
    if(isFire(c) && itemBurns(c->item))
      c->item = itNone;
    }
  if(m == moFireFairy) 
    makeflame(c, 50, false);
  if(m == moPyroCultist && c->item == itNone && c->wall != waChasm) {
    // a reward for killing him before he shoots!
    c->item = itOrbDragon;
    }
  // note: an Orb appears underwater!
  if(m == moWaterElemental && c->item == itNone)
    c->item = itOrbWater;

  if(m == moPirate && isOnCIsland(c) && c->item == itNone && (euclid||c->master->alt) && celldistAlt(c) <= -5) {
    bool toomany = false;
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->mov[i];
      if(c2) for(int j=0; j<c2->type; j++)
        if(c2->mov[j] && c2->mov[j]->item == itCompass)
          toomany = true;
      }
    if(!toomany) c->item = itCompass;
    }
  if(m == moSlime) { c->monst = moNone; spill(c, c->wall, 2); }
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
    eMonster m = c->monst;
    /*if((m == moIvyBranch || m == moIvyHead) && c->mov[c->mondir]->monst == moIvyRoot)
      ivynext(c, moIvyNext); */
    killIvy(c);
    if(m == moIvyBranch || m == moIvyHead || m == moIvyNext) {
      int qty = 0;
      cell *c2 = c->mov[c->mondir];
      for(int i=0; i<c2->type; i++)
        if(c2->mov[i]->monst == moIvyWait && c2->mov[i]->mondir == c2->spn[i])
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
  }

void killWithMessage(cell *c, bool orStun = false) {
  eMonster m = c->monst;
  int tk = tkills();
  if(orStun) 
    killOrStunMonster(c);
  else
    killMonster(c);
  int ntk = tkills();

  if(tk == 0 && ntk > 0)
    addMessage(XLAT("That was easy, but groups could be dangerous."));
    
  if(tk < 10 && ntk >= 10)
    addMessage(XLAT("Good to know that your fighting skills serve you well in this strange world."));

  if(tk < 50 && ntk >= 50)
    addMessage(XLAT("You wonder where all these monsters go, after their death..."));

  if(tk < 100 && ntk >= 100)
    addMessage(XLAT("You feel that the souls of slain enemies pull you to the Graveyard..."));
  
  if(m == moVizier && c->monst != moVizier && kills[moVizier] == 1) {
    addMessage(XLAT("Hmm, he has been training in the Emerald Mine. Interesting..."));
    princess::forceMouse = true;
    }
  
  if(m == moIvyRoot && ntk>tk)
    achievement_gain("IVYSLAYER");
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

bool isCrossroads(eLand l) {
  return l == laCrossroads || l == laCrossroads2 || l == laCrossroads3;
  }

/*
bool orbChance(cell *c, eLand usual, int chthere, int chcross) {
  if(usual == laElementalWall && isElementalLand(c->land))
    usual = c->land;
  if(c->land == usual) return chthere && hrand(chthere) == 0;
  if(chcross && isCrossroads(c->land)) {
    chcross = (chcross / 50) * (50 + items[itHyperstone]);
    return hrand(chcross) == 0;
    }
  if(chcross && c->land == laOcean && c->wall == waBoat && c->landparam > 25) {
    return hrand(chcross) < 20;
    }
  return false;
  }

void placeOrbs(cell *c) {

  if(c->land == laZebra && c->wall == waTrapdoor) return;
  if(c->land == laEdge && cellEdgeUnstable(c)) return;
  
  for(int i=0; i<ORBLINES; i++) {
    orbinfo& oi(orbinfos[i]);
    if(orbChance(c, oi.l, oi.lchance, oi.gchance) && items[treasureType(oi.l)] * landMultiplier(oi.l) >= 10)
      c->item = oi.orb;
    if(oi.orb == itOrbWater && c->land != laOcean) c->wall = waStrandedBoat;
    }

  }
*/

void buildPrizeMirror(cell *c) {
  if(c->type == 7) return;
  if(items[itShard] < 25) return;
  c->wall = hrand(2) ? waCloud : waMirror;
  }

void placePrizeOrb(cell *c) {
  eLand l = c->land;
  if(isElemental(l)) l = laElementalWall;

  // these two lands would have too much orbs according to normal rules
  if(l == laPalace && hrand(100) >= 20) return;
  if(l == laGraveyard && hrand(100) >= 15) return;
  if(l == laLivefjord && hrand(100) >= 35) return;
  if(l == laMinefield && hrand(100) >= 25) return;
  if(l == laElementalWall && hrand(100) >= 25) return;

  if(l == laPalace && princess::dist(c) < OUT_OF_PRISON)
    l = laPrincessQuest;
  for(int i=0; i<ORBLINES; i++) {
    orbinfo& oi(orbinfos[i]);
    eOrbLandRelation olr = getOLR(oi.orb, l);
    if(olr != olrPrize25 && olr != olrPrize3) continue;
    if(olr == olrPrize25 || olr == olrGuest || olr == olrMonster) { 
      if(items[treasureType(oi.l)] < 25)  continue;
      } 
    else if(olr == olrPrize3) { if(items[treasureType(oi.l)] < 3)  continue; }
    else continue;

    if(!oi.gchance) continue;
    if(hrand(oi.gchance) >= 60) continue;
    if(oi.orb == itOrbWater && c->land != laOcean) {
      if(cellHalfvine(c)) continue;
      c->wall = waStrandedBoat;
      return;
      }
    c->item = oi.orb;
    }
  
  // printf("land: %s orb: %s\n", dnameof(l), dnameof(c->item));
  }

#define PRIZEMUL 7

void placeLocalOrbs(cell *c) {
  eLand l = c->land;
  if(l == laZebra && c->wall == waTrapdoor) return;
  if(l == laEdge && cellEdgeUnstable(c)) return;
  if(isElemental(l)) l = laElementalWall;
  
  for(int i=0; i<ORBLINES; i++) {
    orbinfo& oi(orbinfos[i]);
    if(oi.l != l) continue;
    if(!oi.lchance) continue;
    int ch = hrand(oi.lchance);
    if(ch == 0 && items[treasureType(oi.l)] * landMultiplier(oi.l) >= 10) {
      // printf("local orb\n");
      c->item = oi.orb;
      if(oi.orb == itOrbWater && c->land != laOcean) c->wall = waStrandedBoat;
      return;
      }
    else if(oi.gchance && ch == 1 && getOLR(itShard, l) == olrPrize25 && l != laRedRock)
      buildPrizeMirror(c);
    else if(oi.gchance && (ch >= 2 && ch < 2+PRIZEMUL)) 
      placePrizeOrb(c);
    }
  }

void placeCrossroadOrbs(cell *c) {  
  for(int i=0; i<ORBLINES; i++) {
    orbinfo& oi(orbinfos[i]);
    if(!oi.gchance) continue;
    if(items[treasureType(oi.l)] * landMultiplier(oi.l) < 10) continue;
    if(hrand(oi.gchance)) continue;
    if(hrand(50+items[itHyperstone]) >= 50) continue;
    c->item = oi.orb;
    if(oi.orb == itOrbWater && c->land != laOcean) c->wall = waStrandedBoat;
    }
  }

void placeOceanOrbs(cell *c) {  
  for(int i=0; i<ORBLINES; i++) {
    orbinfo& oi(orbinfos[i]);
    if(items[treasureType(oi.l)] * landMultiplier(oi.l) < 10) continue;
    if(!oi.gchance) continue;
    if(hrand(oi.gchance) >= 20) continue;
    c->item = oi.orb;
    }
  }

bool errorReported = false;

void describeCell(cell *c) {
  if(!c) { printf("NULL\n"); return; }
  printf("describe %p: ", c);
  printf("%-15s", linf[c->land].name);
  printf("%-15s", winf[c->wall].name);
  printf("%-15s", iinf[c->item].name);
  printf("%-15s", iinf[c->monst].name);
  printf("%3d", c->landparam);
  printf("%3d", c->mpdist);
  printf("\n");
  }

void raiseBuggyGeneration(cell *c, const char *s) {

  printf("procgen error in: %s\n", s);
  
  if(!errorReported) {
    addMessage(string("something strange happened in: ") + s);
    errorReported = true;
    }
  // return;
  
  describeCell(c);
  for(int i=0; i<c->type; i++) describeCell(c->mov[i]);

  z:
  c->item = itPirate;
  for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->mpdist < c->mpdist) {
    c = c->mov[i];
    goto z;
    }
    
  buggyGeneration = true; return;
  }

void setland(cell *c, eLand l) {
  if(c->land != l) 
    c->landparam = 0;
  c->land = l;
  }

void buildBarrier(cell *c);

bool buildBarrier4(cell *c, int d, int mode, eLand ll, eLand lr);

void buildBarrierFront(cell *c) {
  int ht = c->landparam;

  cellwalker bb(c, c->bardir); setbarrier(bb.c);
  cwstep(bb); 
  bb.c->barleft = c->barleft;
  bb.c->barright = c->barright;
  bb.c->landparam = (ht-4);
//printf("[A heat %d]\n", ht-4);
  setbarrier(bb.c);
  cwspin(bb, 2); cwstep(bb); setland(bb.c, c->barleft); cwstep(bb);
  cwspin(bb, 2); cwstep(bb); setland(bb.c, c->barright); cwstep(bb);
  cwspin(bb, 2); 
  
  cwspin(bb, 3); cwstep(bb); 
  bb.c->landparam = (ht-4)^2;
//printf("[B heat %d]\n", (ht-4)^2);
  bb.c->barleft = c->barright;
  bb.c->barright = c->barleft;
  setbarrier(bb.c);
  cwspin(bb, 3); cwstep(bb);
  
  bb.c->landparam = ht ^ 2;
//printf("[C heat %d]\n", (ht)^2);
  bb.c->bardir = bb.spin;
  bb.c->barleft = c->barright;
  bb.c->barright = c->barleft;
  // printf("#1\n");
  buildBarrier(bb.c);
  
  for(int a=-3; a<=3; a++) if(a) {
    bb.c = c; bb.spin = c->bardir; cwspin(bb, a); cwstep(bb); 
    setland(bb.c, a > 0 ? c->barright : c->barleft);
    }
  }

void buildBarrierBack(cell *c) {
  int ht = c->landparam;

  cellwalker bb(c, c->bardir); setbarrier(bb.c);
  cwspin(bb, 3); cwstep(bb); cwspin(bb, 4); setland(bb.c, c->barright); cwstep(bb); cwspin(bb, 3);
  bb.c->bardir = bb.spin;
  bb.c->barleft = c->barright;
  bb.c->barright = c->barleft;
  bb.c->landparam = ht ^ 11;
//printf("[D heat %d]\n", (ht^11));

  // needed for CR2 to work
  cwstep(bb); 
  bb.c->barleft = c->barright;
  bb.c->barright = c->barleft;
  bb.c->landparam = (ht^11)-4;
  cwstep(bb);
//printf("[E heat %d]\n", (ht^11));

  // printf("#2\n");
  buildBarrier(bb.c);
  }

eLand oppositeElement(eLand l) {
  if(l == laEFire) return laEWater;
  if(l == laEWater) return laEFire;
  if(l == laEAir) return laEEarth;
  if(l == laEEarth) return laEAir;
  return l;
  }

void buildBarrier(cell *c) {
  if(buggyGeneration) return;
  
  // printf("build barrier at %p", c);
  if(c->wall == waBarrier || c->land == laElementalWall) { 
    // printf("-> ready\n");
    return;
    }
//  if(c->wall == waWaxWall) return;
  if(c->mpdist > BARLEV) {
    // printf("-> too far\n");
    return; // == INFD) return;
    }
  
  if(((c->barleft == laCrossroads3 || c->barright == laCrossroads3) && hrand(100) < 66) ||
    (isElemental(c->barleft) && isElemental(c->barright) && hrand(100) < 25)) {
    cellwalker cw(c, c->bardir);
    cwspin(cw, 3); cwstep(cw); 
    cell *cp = cwpeek(cw, 4);
    if(cp->wall != waBarrier && cp->land != laElementalWall) {
      cwspin(cw, 2);  cwstep(cw);
      bool b = buildBarrier4(cw.c, cw.spin, 2, oppositeElement(c->barleft), c->barright);
      if(b) return;
      }
    else {
      bool b = buildBarrier4(c, c->bardir, 1, c->barleft, c->barright);
      if(b) return;
      }
    }
  
  buildBarrierFront(c);  
  buildBarrierBack(c);
  }

void chasmify(cell *c) {
  c->wall = waChasm; c->item = itNone;
  int q = 0;
  cell *c2[10];
  for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->mpdist > c->mpdist && cellUnstable(c->mov[i]))
    c2[q++] = c->mov[i];
  if(q) {
    cell *c3 = c2[hrand(q)];
    c3->wall = waChasmD;
    }
  }

void chasmifyEarth(cell *c) {
  int q = 0;
  int d2[10];
  for(int i=2; i<=c->type-2; i++) {
    int j = (i+c->mondir)%c->type;
    cell *c2 = c->mov[j];
    if(c2 && c2->mpdist > c->mpdist && (
      c2->wall == waDeadfloor || c2->wall == waDeadwall ||
      c2->wall == waDeadfloor2)) 
      d2[q++] = j;
    }
  if(!q) printf("no further move!\n");
  if(q) {
    int d = d2[hrand(q)];
    cell *c3 = c->mov[d];
    c3->wall = waEarthD;
    for(int i=0; i<c3->type; i++) {
      cell *c4 = createMov(c3, i);
      earthFloor(c4);
      }
    c3->mondir = c->spn[d];
    }
  earthWall(c); c->item = itNone;
  }

void chasmifyElemental(cell *c) {
  int q = 0;
  int d2[10];
  for(int i=2; i<=c->type-2; i++) {
    int j = (i+c->mondir)%c->type;
    cell *c2 = c->mov[j];
    if(c2 && c2->mpdist > c->mpdist && c2->land == c->land)
      d2[q++] = j;
    }
  if(q) {
    int d = d2[hrand(q)];
    cell *c3 = c->mov[d];
    c3->wall = waElementalD;
    for(int i=0; i<c3->type; i++) {
      cell *c4 = createMov(c3, i);
      c4->wall = waNone;
      }
    c3->mondir = c->spn[d];
    }
  c->wall = getElementalWall(c->land);
  c->wparam = 100; c->item = itNone;
  }

bool incompatible1(eLand l1, eLand l2) {
  if(isCrossroads(l1) && isCrossroads(l2)) return true;
  if(l1 == laJungle && l2 == laMotion) return true;
  if(l1 == laMirror && l2 == laMotion) return true;
  if(l1 == laPower && l2 == laWineyard) return true;
  if(l1 == laPower && l2 == laDryForest) return true;
  if(l1 == laEFire && l2 == laWineyard) return true;
  if(l1 == laEFire && l2 == laDryForest) return true;
  if(l1 == laGraveyard && l2 == laDryForest) return true;
  if(l1 == laGraveyard && l2 == laRedRock) return true;
  if(l1 == laGraveyard && l2 == laEmerald) return true;
  if(l1 == laDeadCaves && l2 == laEmerald) return true;
  if(l1 == laDeadCaves && l2 == laCaves) return true;
  if(isElemental(l1) && isElemental(l2)) return true;
  return false;
  }

eLand randomElementalLand() {
  int i = hrand(4);
  eLand t[4] = { laEFire, laEWater, laEAir, laEEarth };
  return t[i];
  }

bool incompatible(eLand nw, eLand old) {
  return (nw == old) || incompatible1(nw, old) || incompatible1(old, nw);
  }

extern bool generatingEquidistant;

bool rlyehComplete() {
  return items[itStatue] >= 10 || items[itGrimoire] >= 10;
  }

bool lchance() { return hrand(100) < 90; }

eLand pickLandRPM(eLand old) {
  while(true) {
    eLand n = randlands[hrand(RANDLANDS)];
    if(incompatible(n, old)) continue;
    if(n == laIce || n == laCaves || n == laDesert)
      return n;
    if(hiitems[treasureType(n)] < 10)
      continue;
    return n;
    }
  }

eLand getNewLand(eLand old) {

  if(randomPatternsMode) return pickLandRPM(old);
  
  if(old == laCaribbean)
    return laOcean;
    
  if(old == laEEarth && lchance()) return hrand(2) ? laEWater : laEFire;
  if(old == laEAir   && lchance()) return hrand(2) ? laEWater : laEFire;
  if(old == laEWater && lchance()) return hrand(2) ? laEEarth : laEAir;
  if(old == laEFire  && lchance()) return hrand(2) ? laEEarth : laEAir;
  
  if(old == laLivefjord && hrand(2))
    return laOcean;
  
  if(old == laOcean && !generatingEquidistant)
    return hrand(2) ? laCaribbean : laLivefjord;

  if(old == laOcean && gold() >= 60 && hrand(100) < 75 && !rlyehComplete()) 
    return laRlyeh;
    
  if(old == laRlyeh && !rlyehComplete())
    return laOcean;
    
  eLand tab[256];
  int cnt = 0;
  
/*   if(isHive(old) && hrand(100) < 90) {
    eLand n = old;
    while(n == old) n = eLand(laHive0 + hrand(3));
    return n;
    } */
  
  // return (hrand(2)) ? laMotion : laJungle;

  // the basic lands, always available
  tab[cnt++] = laCrossroads;
  tab[cnt++] = laIce;
  tab[cnt++] = laDesert;
  tab[cnt++] = laJungle;
  tab[cnt++] = laMotion;
  if(old != laDeadCaves) tab[cnt++] = laCaves;
  
  // the intermediate lands
  if(gold() >= 30) {
    tab[cnt++] = laCrossroads;
    tab[cnt++] = laAlchemist;
    tab[cnt++] = laMirror;
    tab[cnt++] = laOcean;
    tab[cnt++] = laLivefjord;
    tab[cnt++] = laMinefield;
    tab[cnt++] = laPalace;
    if(kills[moVizier]) tab[cnt++] = laEmerald;
    if(items[itFeather] >= 10) tab[cnt++] = laZebra;
    }
  
  // the advanced lands
  if(gold() >= 60) {
    // REMOVETHIS
    tab[cnt++] = laEdge;
    tab[cnt++] = laCrossroads2;
    if(rlyehComplete()) tab[cnt++] = laRlyeh;
    if(old == laCrossroads || old == laCrossroads2) tab[cnt++] = laOcean;
    if(old == laOcean) tab[cnt++] = laCrossroads;
    if(items[itGold] >= 5 && items[itFernFlower] >= 5 && !kills[moVizier]) 
      tab[cnt++] = laEmerald;
    tab[cnt++] = laDryForest;
    tab[cnt++] = laWineyard;
    if(items[itGold] >= 10) tab[cnt++] = laDeadCaves;
    // tab[cnt++] = laCaribbean;
    if(items[itSpice] >= 10) {
      tab[cnt++] = laRedRock;
      if(old == laDesert) tab[cnt++] = laRedRock;
      }
    if(old == laRedRock) tab[cnt++] = laDesert;
    }
  
  if(tkills() >= 100) {
    tab[cnt++] = laGraveyard;
    if(gold() >= 60) tab[cnt++] = laHive;
    }
  
  if(kills[moWaterElemental] + kills[moEarthElemental] + kills[moFireElemental] + kills[moAirElemental])
    tab[cnt++] = randomElementalLand();
  
  if(hellUnlocked()) {
    tab[cnt++] = laCrossroads3;
    tab[cnt++] = laHell;
    }
  
  if(items[itHell] >= 10) {
    tab[cnt++] = laCocytus;
    if(old == laHell || old == laIce) tab[cnt++ ] = laCocytus;
    if(old == laCocytus) { tab[cnt++] = laIce; tab[cnt++] = laHell; }
    tab[cnt++] = laPower;
    if(old == laCrossroads || old == laCrossroads2) tab[cnt++] = laOcean;
    if(old == laOcean) tab[cnt++] = laCrossroads2;
    }
  
  // for(int i=0; i<20; i++) tab[cnt++] = laRedRock;
  // for(int i=0; i<20; i++) tab[cnt++] = laCaribbean;
  // for(int i=0; i<20; i++) tab[cnt++] = laCocytus;
  
  // for(int i=0; i<20; i++) tab[cnt++] = laCrossroads;

  eLand n = old;
  while(incompatible(n, old)) n = tab[hrand(cnt)];
  
  return n;  
  }

bool notDippingFor(eItem i) {
  int v = items[i] - currentLocalTreasure;
  if(v <= 10) return true;
  if(v >= 20) return false;
  return v >= hrand(10) + 10;
  }

bool notDippingForExtra(eItem i, eItem x) {
  int v = items[i] - min(items[x], currentLocalTreasure);
  if(v <= 10) return true;
  if(v >= 20) return false;
  return v >= hrand(10) + 10;
  }

eLand euland[65536];

eLand switchable(eLand nearland, eLand farland, eucoord c) {
  if(nearland == laCrossroads) {
    if(hrand(4) == 0 && (short(c)%3==0))
      return laBarrier;
    return laCrossroads;
    }
  else if(nearland == laBarrier) {
    return getNewLand(farland);
    }
  else {
    if(hrand(20) == 0 && (short(c)%3==0))
      return laBarrier;
    return nearland;
    }
  }

eLand getEuclidLand(eucoord c) {
  if(euland[c]) return euland[c];
  if(c == 0 || c == eucoord(-1) || c == 1)
    return euland[c] = laCrossroads;
  if(euland[eucoord(c-2)] && ! euland[eucoord(c-1)]) getEuclidLand(c-1);
  if(euland[eucoord(c+2)] && ! euland[eucoord(c+1)]) getEuclidLand(c+1);
  if(euland[eucoord(c-1)]) return 
    euland[c] = switchable(euland[c-1], euland[eucoord(c-2)], c);
  if(euland[eucoord(c+1)]) return 
    euland[c] = switchable(euland[c+1], euland[eucoord(c+2)], c);
  return euland[c] = laCrossroads;
  }

int newRoundTableRadius() {
  return 28 + 2 * items[itHolyGrail];
  }

int roundTableRadius(cell *c) {
  if(euclid) return 28;
  return c->master->alt->alt->emeraldval & GRAIL_RADIUS_MASK;
  }

int celldistAltRelative(cell *c) {
  return celldistAlt(c) - roundTableRadius(c);
  }

void buildCamelotWall(cell *c) {
  c->wall = waCamelot;
  for(int i=0; i<c->type; i++) {
    cell *c2 = createMov(c, i);
    if(c2->wall == waNone && (euclid || (c2->master->alt && c->master->alt)) && celldistAlt(c2) > celldistAlt(c) && c2->monst == moNone)
      c2->wall = waCamelotMoat;
    }
  }

// This function generates all lands. Warning: it's very long!

void buildBarrier(cell *c, int d) {
  d %= 7;
  cellwalker bb(c, d);
  
  if(checkBarriersFront(bb) && checkBarriersBack(bb)) {
    c->bardir = d;
    eLand oldland = c->land;
    eLand newland = getNewLand(oldland);
    if(showoff) newland = showlist[(showid++) % 10];
    landcount[newland]++;

    if(d == 5) c->barleft = oldland, c->barright = newland;
    else c->barleft = newland, c->barright = oldland;
    c->landparam = 40;
    }
  }

bool buildBarrier4(cell *c, int d, int mode, eLand ll, eLand lr) {
  d %= 7;
  cellwalker b1(c, d);

  cellwalker b2(c, d);
  cwstep(b2); cwspin(b2, 3); cwstep(b2); cwspin(b2, 3); cwstep(b2);
  
  cellwalker b3(c, d);
  cwstep(b3); cwspin(b3, 4); cwstep(b3); cwspin(b3, 4);

  cellwalker b4(c, d);
  cwstep(b4); cwspin(b4, -4); cwstep(b4); cwspin(b4, -4);
  
  if(mode == 0) {
    if(!((checkBarriersBack(b1) && checkBarriersBack(b2)))) return false;
    if(!((checkBarriersFront(b3) && checkBarriersFront(b4)))) return false;
    }

  if(mode == 1) {
    if(!(checkBarriersFront(b3, 5, true) && checkBarriersFront(b4, 5, true))) 
      return false;
    }
    
  if(mode == 2) {
    if(!((checkBarriersBack(b1, 5, true) && checkBarriersBack(b2, 5, true)))) 
      return false;
    }
  
  eLand xl = oppositeElement(ll);
  eLand xr = oppositeElement(lr);
  
  c->bardir = d, c->barleft = ll, c->barright = lr; buildBarrierBack(c);
  
  c= b2.c; d=b2.spin;
  c->bardir = d, c->barleft = xl, c->barright = xr; buildBarrierBack(c);

  c= b3.c; d=b3.spin;
  c->bardir = d, c->barleft = xl, c->barright = lr; buildBarrierFront(c);

  c= b4.c; d=b4.spin;
  c->bardir = d, c->barleft = ll, c->barright = xr; buildBarrierFront(c);
  
  for(int a=-3; a<=3; a++) if(a) {
    setland(cwpeek(b1, a), a > 0 ? lr : ll);
    setland(cwpeek(b2, a), a > 0 ? xr : xl);
    setland(cwpeek(b3, a), a > 0 ? lr : xl);
    setland(cwpeek(b4, a), a > 0 ? xr : ll);
    }

  cell *cp;
  cp = cwpeek(b1, 0);
  cp->barleft = ll; cp->barright = lr; setbarrier(cp);
  cp = cwpeek(b2, 0);
  cp->barleft = xl; cp->barright = xr; setbarrier(cp);
  
  return true;
  }    

void buildBarrierStrong(cell *c, int d, bool oldleft) {
  d %= 7;
  cellwalker bb(c, d);

  c->bardir = d;
  eLand oldland = c->land;
  eLand newland = getNewLand(oldland);
  if(showoff) newland = showlist[(showid++) % 10];
  landcount[newland]++;

  if(oldleft) c->barleft = oldland, c->barright = newland;
  else c->barleft = newland, c->barright = oldland;
  }

void buildCrossroads2(cell *c) {

  if(buggyGeneration) return;

  if(!c) return;
  
  for(int i=0; i<c->type; i++)
    if(c->mov[i] && !c->mov[i]->landparam && c->mov[i]->mpdist < c->mpdist)
      buildCrossroads2(c->mov[i]);
  
  if(c->bardir != NODIR && c->bardir != NOBARRIERS)
    buildBarrier(c);

  if(c->land != laCrossroads2) return;

  if(!c->landparam) {
    for(int i=0; i<c->type; i++) { 
      cell *c2 = createMov(c, i);
      if(c2 && c2->landparam) {
        for(int j=0; j<c2->type; j++) {
          createMov(c2, j);
          cell *c3 = c2->mov[j];
          if(c3 && c3->landparam) {
            int h2 = c2->landparam;
            int h3 = c3->landparam;
            
            // ambiguous
            if(h2/4 == 1 && h3/4 == 3) continue;
            if(h2/4 == 3 && h3/4 == 1) continue;
            
            for(int d=0; d<c2->type; d++)
              if(emeraldtable[h2][d] == h3) {
                int nh = emeraldtable[h2][(42+d + c->spn[i] - j) % c2->type];
                if(c->landparam>0 && c->landparam != nh) printf("CONFLICT\n");
                c->landparam = nh;
                }
    
            if(c->landparam == 0) 
              printf("H2 = %d H3=%d\n", c2->landparam, c3->landparam);
            }
          }
        if(c->landparam == 0) {
          printf("H2 = %d\n", c2->landparam);
          describeCell(c2);
          for(int i=0; i<c2->type; i++) describeCell(c2->mov[i]);
//        halted = true;
//        c->heat = -1;
          c2->item = itCompass;
          return;
          }
        }
      }
    
    if(c->landparam) {
//    for(int i=0; i<c->type; i++) { 
//      cell *c2 = c->mov[i];
//      buildCrossroads2(c2);
//      }
      }
    else {
      raiseBuggyGeneration(c, "buildCrossroads2");
      return;
      }
    }

  int h = c->landparam;
  
  if(h/4 >= 8 && h/4 <= 11) {
    for(int i=0; i<c->type; i++) { 
      createMov(c, i)->land = laCrossroads2;
      if(!c->mov[i]->landparam) buildCrossroads2(c->mov[i]);
      }
    if(h/4 == 8 || h/4 == 10)
    for(int i=0; i<c->type; i++) { 
      if(c->mov[i] && c->mov[i]->landparam == h-4) {
        bool oldleft = true;
        for(int j=1; j<=3; j++) 
          if(c->mov[(i+j)%7] && c->mov[(i+j)%7]->mpdist < c->mpdist)
            oldleft = false;
            
        buildBarrierStrong(c, i, oldleft); 
        c->landparam = h;
        buildBarrier(c);
        }
      }
    }
  }

extern bool bugtrack;

bool generatingEquidistant = false;

void buildAnotherEquidistant(cell *c) {
  // printf("building another coast\n");

  int gdir = -1;
  for(int i=0; i<c->type; i++) {
    if(c->mov[i] && c->mov[i]->mpdist < c->mpdist) gdir = i;
    }
  if(!gdir) return;

  generatingEquidistant = true;

  cellwalker cw(c, (gdir+3) % c->type);
  vector<cell*> coastpath;
  while(coastpath.size() < 30 || cw.c->type != 7) {
    coastpath.push_back(cw.c);
    cwstep(cw); cwspin(cw, 3); 
    if(cw.c->type == 7 && hrand(2) == 0) cwspin(cw, 1);
    }
  coastpath.push_back(cw.c);
  // printf("setdists\n");
  for(int i=1; i<size(coastpath) - 1; i++) {
    setdist(coastpath[i], BARLEV, coastpath[i-1]);
    setdist(coastpath[i], BARLEV-1, coastpath[i-1]);
    }
  // printf("building barrier\n");
  cell *c2 = coastpath[coastpath.size() - 1];
  int bd = 2 + (hrand(2)) * 3;
  buildBarrier(c2, bd);
  // printf("building barrier II\n");
  if(c2->bardir != NODIR && c2->bardir != NOBARRIERS)
    buildBarrier(c2);
  // printf("setdisting\n");
  for(int i=size(coastpath)-2; i>=0; i--) {
    for(int j=BARLEV; j>=6; j--)
      setdist(coastpath[i], j, NULL);
    // buildEquidistant(coastpath[i]);
    // printf("i=%d heat=%f\n", i, coastpath[i]->heat);
    // coastpath[i]->item = itCoast;
    }
  generatingEquidistant = false;
  }

bool bugtrack = false, bugfound = false;

#define UNKNOWN 65535

int coastval(cell *c) {
  if(!c) return UNKNOWN;
  if(c->land == laNone) return UNKNOWN;
  if(c->land == laOceanWall || c->land == laCaribbean || c->land == laWhirlpool ||
    c->land == laLivefjord)
    return 30;
  if(c->land  != laOcean && c->land != laEdge) {
    return 0;
    }
  if(!c->landparam) return UNKNOWN;
  return c->landparam;
  }

bool cellEdgeUnstable(cell *c) {
  if(c->land != laEdge || c->wall != waNone) return false;
  int d = coastval(c);
  for(int i=0; i<c->type; i++)
    if(coastval(c->mov[i]) == d-1) {
      if(againstWind(c->mov[i], c)) return false;
      if(!passable(c->mov[i], NULL, true, false, false) &&
        !isFire(c->mov[i]))
        return false;
      if(isWorm(c->mov[i]))
        return false;
      }
  return true;
  }

eMonster crossroadsMonster() {

  static eMonster weak[9] = {
    moYeti, moGoblin, moRanger, moOrangeDog, moRunDog, moMonkey, moZombie,
    moDesertman, moCultist
    };
  
  if(hrand(10) == 0) return weak[hrand(9)];

  static eMonster m[24] = {
    moWorm, moTentacle, 
    moTroll, moEagle,
    moLesser, moGreater, moPyroCultist, moGhost,
    moFireFairy, moIvyRoot, moHedge,
    moLancer, moFlailer, moVineBeast,
    moBomberbird, moAlbatross, moRedTroll,
    moWaterElemental, moAirElemental, moFireElemental,
    moFatGuard, moMiner, moPalace, moVizier
    };
  return m[hrand(24)];
  }

eMonster wanderingCrossroadsMonster() {
  while(true) {
    eMonster m = crossroadsMonster();
    if(!isIvy(m) && m != moTentacle) return m;
    }
  }

void buildEquidistant(cell *c) {
  if(!c) return;
  if(c->landparam) return;
  int mcv = UNKNOWN;

  // find the lowest coastval
  for(int i=0; i<c->type; i++) {
    int cv = coastval(c->mov[i]);
    if(cv < mcv) mcv = cv;
    }
  
  int mcv2 = 0;
  
  if(bugfound) { mcv = 1; c->landparam = 10; }
   
  else if(mcv == 0) {
    // if(generatingEquidistant) printf("mcv=0\n");
    c->landparam = 1;
    }
  else {
    // if it appears twice, increase it
    int qcv = 0;
    int sid = 0;
    for(int i=0; i<c->type; i++) 
      if(coastval(c->mov[i]) == mcv)
        qcv++, sid = i;
    
    // if(generatingEquidistant) printf("qcv=%d mcv=%d\n", qcv, mcv);
    if(qcv >= 2) c->landparam = mcv+1;
    else {
      // if(qcv != 1) { printf("qcv = %d\n", qcv);  exit(1); }
      cell *c2 = c->mov[sid];
      int bsid = c->spn[sid];
      for(int j=0; j<7; j++) {
        int q = (bsid+j+42) % c2->type;
        cell *c3 = c2->mov[q];
        if(coastval(c3) < mcv) {
          cell *c4 = createMovR(c2, bsid+1);
          buildEquidistant(c4);
          mcv2 = coastval(c4);
          break;
          }
        q = (bsid-j+42) % c2->type;
        c3 = c2->mov[q];
        if(coastval(c3) < mcv) {
          cell *c4 = createMovR(c2, bsid-1);
          buildEquidistant(c4);
          mcv2 = coastval(c4);
          break;
          }
        }
      if(bugfound) c->item = itSapphire;
      if(mcv2 > mcv) mcv2 = mcv;
      if(mcv2 == 0) mcv2 = mcv;
      c->landparam = mcv2+1;

      /* if(c->heat < 3)
        raiseBuggyGeneration(c, "low heat"); */

      }
    }
  
  if(!c->landparam) {
    // int z = int(c->heat);
    if(c->item || c->monst) 
      printf("building coast over %s/%s, mpdist = %d\n", iinf[c->item].name, minf[c->monst].name,
        c->mpdist);
    if(c->land == laOcean) c->wall = waSea;
    }
  
  if(c->landparam > 30 && c->land == laOcean && !generatingEquidistant && hrand(10) < 5) 
    buildAnotherEquidistant(c);
  }

void beCIsland(cell *c) {
  int i = hrand(3);
  if(i == 0) c->wall = waCIsland;
  if(i == 1) c->wall = waCIsland2;
  if(i == 2) c->wall = waCTree;
  return;
  }  

void generateTreasureIsland(cell *c) {
  if(!euclid) generateAlts(c->master);
  if(isOnCIsland(c)) return;
  
  bool src = hrand(100) < 10;
  if(src) {
    beCIsland(c);
    if(c->wall == waCTree) return;
    }
  cell* ctab[7];
  int qc = 0, qlo, qhi;
  for(int i=0; i<c->type; i++) {
    cell *c2 = createMov(c, i);
    if((euclid || (c->master->alt && c2->master->alt)) && celldistAlt(c2) < celldistAlt(c)) {
      ctab[qc++] = c2;
      qlo = i; qhi = i;
      while(true) {
        qlo--;
        c2 = createMovR(c, qlo);
        if(!c2->master->alt) break;
        if(celldistAlt(c2) >= celldistAlt(c)) break;
        ctab[qc++] = c2;
        }
      while(true) {
        qhi++;
        c2 = createMovR(c, qhi);
        if(!c2->master->alt) break;
        if(celldistAlt(c2) >= celldistAlt(c)) break;
        ctab[qc++] = c2;
        }
      break;
      }
    }
  if(!qc) { printf("NO QC\n"); c->wall = waSea; return; }
  cell* c2 = createMovR(c, (qlo+qhi)/2);
  generateTreasureIsland(c2);
  if(!src) {
    c->wall = c2->wall;
    if(c->wall != waCTree && hrand(100) < 15)
      c->wall = (c->wall == waCIsland ? waCIsland2 : waCIsland);
    }
  if(src && c2->wall == waCTree && (euclid||c->master->alt) && celldistAlt(c) <= -10) {
    bool end = true;
    for(int i=0; i<qc; i++) {
      generateTreasureIsland(ctab[i]);
      if(ctab[i]->wall != waCTree)
        end = false;
      }
    // printf("%p: end=%d, qc=%d, dist=%d\n", c, end, qc, celldistAlt(c));
    if(end) c->item = itPirate;
    else c->item = itCompass;
    }
  }

#define ROCKSNAKELENGTH 50

void buildRedWall(cell *c, int gemchance) {
  if(c->monst) return;
  int ki = kills[moHexSnake] + kills[moRedTroll];
  c->wall = waRed3;
  if(hrand(100+ki) < gemchance + ki)
    c->item = itRedGem;
  if(items[itRedGem] >= 10 && hrand(18000) < gemchance)
    c->item = itOrbTelekinesis;
  }

bool generatingYendor = false;
bool yendorPath = false;

int palaceHP() {
  if(items[itPalace] < 3) // 1+2
    return 2;
  else if(items[itPalace] < 10) // 1+2+3+4
    return 3;
  else if(items[itPalace] < 21) // 1+2+3+4+5+6
    return 4;
  else if(items[itPalace] < 35)
    return 5;
  else if(items[itPalace] < 50)
    return 6;
  else return 7;
  }

bool havemouse;

cell *randomDown(cell *c) {
  cell *tab[7];
  int q=0;
  for(int i=0; i<c->type; i++) 
    if(c->mov[i] && coastval(c->mov[i]) < coastval(c))
      tab[q++] = c->mov[i];
  if(!q) return NULL;
  if(q==1) return tab[0];
  return tab[hrand(q)];
  }

// which=1 => right, which=-1 => left
// set which=1,bonus=1 to get right neighbor on level

cell *chosenDown(cell *c, int which, int bonus) {
  int d = coastval(c)-1;
  for(int i=0; i<c->type; i++) {
    if(!c->mov[i]) createMov(c, i);
    if(c->mov[i]->mpdist > BARLEV) setdist(c->mov[i], BARLEV, c);
    
    if(coastval(c->mov[i]) == d) {
      int i2 = (i+which+42)%c->type;
      if(coastval(c->mov[i2]) == d)
        return createMovR(c, i2+bonus);
      else return createMovR(c, i+bonus);
      }
    }
  // printf("nothing down here\n");
  return NULL;
  }

int edgeDepth(cell *c) {
  if(c->land == laEdge) return coastval(c);
  else if(c->land != laBarrier) {
    for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->land == laBarrier)
      return -20+c->cpdist;
    }
  return 0;
  }

int towerval(cell *c) {
  if(c->land != laEdge) return 0;
  cell *cp1 = chosenDown(c, 1, 1);
  if(!cp1) return 0;
  /* cell *cp2 = chosenDown(cp1, 1, 1);
  if(!cp2) return 0;
  cell *cm1 = chosenDown(c, -1, -1);
  if(!cm1) return 0;
  cell *cm2 = chosenDown(cm1, -1, -1);
  if(!cm2) return 0; */

  /* return 
    (c->type-6) 
    + 2*(cp1->type-6) + 4*(cp2->type-6) 
    + 8*(cm1->type-6) +16*(cm2->type-6); */
  
  int under = 0;
  for(int i=0; i<c->type; i++) if(c->mov[i]->landparam < c->landparam)
    under++;
  return (c->type-6) + 2*(cp1->type-6) + 4*under;
  }

int hivehard() {
  return items[itRoyalJelly] + 5 * items[itOrbYendor] * (items[itOrbYendor] * 3 - 2);
  // 0, 5, 40, 135
  }

eMonster randomHyperbug() {
  int h = hivehard();
  if(hrand(200) < h)
    return moBug2;
  return eMonster(moBug0 + hrand(BUGCOLORS));
  // 50: 25/25/50
  // 100: 
  }

#define RANDPATC(c) (randpattern(c,randompattern[c->land]))
#define RANDPAT (randpattern(c,randompattern[c->land]))
#define RANDPAT3(i) (randpatternMajority(c,i,RANDITER))
#define RANDPATV(x) (randpattern(c,randompattern[x]))

void setdist(cell *c, int d, cell *from) {
  DEB("setdist");
  if(buggyGeneration) return;
  if(signed(c->mpdist) <= d) return;
  c->mpdist = d;
  
  if(d >= BARLEV) {
    if(!c->land && from->land != laElementalWall) c->land = from->land;
    if(c->land == laTemple) c->land = laRlyeh;
    if(c->land == laWhirlpool) c->land = laOcean;
    if(c->land == laCamelot) c->land = laCrossroads;

    if(euclid) {
      c->land = euclidland;
      if(euclidland == laCrossroads) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        c->land = getEuclidLand(y+2*x);
        }
      if(euclidland == laWhirlpool) {
        c->land = laOcean;
        c->landparam = 99;
        }
      if(euclidland == laPrincessQuest)
        c->land = laPalace;
      if(euclidland == laOcean) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        int y0 = y; if(y>50000) y0 -= 65536; y0 += 10;
        if(y0 == 0) 
          {c->land = laBarrier; if(ishept(c)) c->land = laRlyeh; }
        else if(y0<0) c->land = laRlyeh;
        else c->landparam = y0;
        }
      if(euclidland == laEdge) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        int y0 = y; if(y>50000) y0 -= 65536; y0 = -y0; y0 -= 5;
        if(y0 == 0) 
          {c->land = laBarrier; if(ishept(c)) c->land = laAlchemist; }
        else if(y0<0) c->land = laAlchemist;
        else {
          c->landparam = y0;
          }
        }
      if(euclidland == laElementalWall) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        int y0 = y; if(y>32768) y0 -= 65536;
        int x0 = x +y/2;
        
        int id = 0;
        if(y0&16) id += 2;
        if(x0&16) id += 1;
        
        x0 += 8; y0 += 8;

        y0--; x0++;
        int id2 = 0;
        if(y0&16) id2 += 2;
        if(x0&16) id2 += 1;
        
        c->land = eLand(laEFire + id);
        
        if(((y0&15) == 15 && (x0&1)) || ((x0&15) == 0 && ((y0+1)&1))) {
          if(c->land == laEFire) c->wall = waEternalFire;
          if(c->land == laEWater) c->wall = waSea;
          if(c->land == laEAir) c->wall = waChasm;
          if(c->land == laEEarth) c->wall = waStone;
          c->barright = c->land;
          c->barleft = eLand(laEFire+id2);
          c->land = laElementalWall;
          }
        }
      if(euclidland == laCrossroads3) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        int y0 = y; if(y>32768) y0 -= 65536;
        int x0 = x +y/2;
        
        x0 += 24; y0 += 8;
        
        int id = 0;
        if(y0&16) id ^= 1;
        if(x0&16) id ^= 1;
        
        c->land = id ? laCrossroads3 : laDesert;
        
        if(((y0&15) == 15 && (x0&1)) || ((x0&15) == 0 && ((y0+1)&1))) {
          c->wall = waBarrier;
          c->land = laBarrier;
          }
        }
      }
    }
  
#ifndef MOBILE
  if(d == BARLEV && c->land == laCanvas) 
      c->landparam = mapeditor::generateCanvas(c);
#endif

  if(d == BARLEV && !euclid && c != cwt.c) {
  
    bool deepOcean = false;
          
    if(c->land == laOcean) {
      if(!from) deepOcean = true;
      else for(int i=0; i<from->type; i++) {
        cell *c2 = from->mov[i];
        if(c2 && c2->landparam > 30)
          deepOcean = true;
        }
      }
    
    if(generatingEquidistant) deepOcean = false;
    
    if(c->land == laCrossroads2)
      buildCrossroads2(c);

    else if(c->type == 7 && hrand(10000) < (
      showoff ? (cwt.c->mpdist > 7 ? 0 : 10000) : 
      princess::challenge ? 0 :
      c->land == laCrossroads3 ? 10000 : 
      isElemental(c->land) ? 4000 : 
      c->land == laCrossroads ? 5000 : 
      c->land == laCaribbean ? 500 :
      c->land == laCanvas ? 0 :
      c->land == laOcean ? (deepOcean ? 1000 : 0) :
      c->land == laEdge ? 0 :
      50))
    {
      
      int bd = 2 + hrand(2) * 3;
      
      buildBarrier(c, bd); 

      /* int bd = 2;      
      buildBarrier4(c, bd, 0, getNewLand(c->land), c->land); */
      }
        
    if(c->land == laCrossroads && c->type == 7 && hrand(2000) < 200 && items[itEmerald] >= 5) {
      int rtr = newRoundTableRadius();
      heptagon *alt = createAlternateMap(c, rtr+14, hsOrigin);
      if(alt) {
        alt->emeraldval = rtr;
        }
      }

    if(true) {
      if(c->land == laRlyeh && c->type == 7 && hrand(2000) < 100 && items[itStatue] >= 5 && !randomPatternsMode)
        createAlternateMap(c, 2, hsA);

      if(c->land == laOcean && c->type == 7 && hrand(2000) < 500 && deepOcean)
        createAlternateMap(c, 2, hsA);

      if(c->land == laCaribbean && c->type == 7)
        createAlternateMap(c, 2, hsA);

      if(c->land == laPalace && c->type == 7 && !princess::generating && !shmup::on &&
        (princess::forceMouse ? (from && from->pathdist != INF) : (hrand(2000) < 20)) && 
        !c->master->alt && 
        (princess::challenge || kills[moVizier]))
        createAlternateMap(c, 141, hsOrigin, waPalace);

      if(c->bardir != NODIR && c->bardir != NOBARRIERS) 
        buildBarrier(c);
      }
    }
  
  if(d < 10) {
    explore[d]++;
    exploreland[d][c->land]++;
    
    if(d == BARLEV-2 && c->land == laOcean) {
      buildEquidistant(c);
      }

    if(d < BARLEV) for(int i=0; i<c->type; i++) {
      setdist(createMov(c, i), d+1, c);
      }
    
    if(d == 8 && c->land == laEdge) {
      buildEquidistant(c);
      }

    if(d == 8 && c->land == laEdge && !euclid) {

      if(hrand(1000) < 75 && // chosenDown(c, 1, 0)->landflags != 3 && chosenDown(c,-1,0)->landflags != 3 && 
        (c->landparam&1) ) {
        c->landflags = 3;
        }
      else c->landflags = 0;
      }

    if(d == 7 && c->land == laEdge) {
      /* if(int(c->landparam) % 5 == 0) 
        c->wall = waCamelot;
        */
      
      if(euclid) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        string tab[] = {
          ".####...",
          "L...L...",
          ".L..L...",
          "L...L...",
          "........",
          "........"
          };
        int y0 = y; if(y>32768) y0 -= 65536;
        
        y0 += 5; y0 %= 12; if(y0<0) y0+=12;
        
        if(y0 >= 6) { y0 -= 6; x += 4; }
        
        char ch = tab[y0][(x+(y+1)/2)&7];
        
        if(ch == '#')
          c->wall = waPlatform;
        else if(ch == 'L')
          c->wall = waLadder;
        }
      
      else if(true) {
        
        cell *c2 = c;
        cell *c3 = c;
        
        bool rdepths[5];
        for(int i=0; i<5; i++) {
          if(coastval(c2) == 0) { 
            rdepths[i++] = false;
            }
          else {
            cell *c4 = c2;
            if(c2 != c3 && !isNeighbor(c2, c3)) {
              for(int i=0; i<c2->type; i++) if(c2->mov[i] && isNeighbor(c2->mov[i], c3))
                c4 = c2->mov[i];
              }
            rdepths[i] = c2 && c3 && c4 && (c2->landflags == 3 || c3->landflags == 3 || c4->landflags == 3);
            c2 = chosenDown(c2, 1, 0);
            c3 = chosenDown(c3, -1, 0);
            }
          }
        
        if(rdepths[3]) {
          c->wall = waPlatform;
//        if(!c4->item) c4->item = itPalace;
          }
        else if(!rdepths[2] && !rdepths[4] && !rdepths[1]) {
          c2 = c;
          c3 = c;
          cell *c4 = chosenDown(c, 1, 1);
          cell *c5 = chosenDown(c, -1, -1);
          for(int i=0; i<3; i++) {
            if(coastval(c2) == 0) break;
            if(c2 && c4 && c4->landflags == 3 && c2->landflags != 3 && c4 == chosenDown(c2, 1, 1)) 
              c->wall = waLadder;
            if(c3 && c5 && c5->landflags == 3 && c3->landflags != 3 && c5 == chosenDown(c3, -1, -1)) 
              c->wall = waLadder;
            buildEquidistant(c4); buildEquidistant(c5);
            if(c2) c2 = chosenDown(c2,  1, 0);
            if(c3) c3 = chosenDown(c3, -1, 0);
            if(c4) c4 = chosenDown(c4,  1, 0);
            if(c5) c5 = chosenDown(c5, -1, 0);
            }
          }
        }
      else c->wall = waCIsland;
      }
    
    if(d == 9 && c->land == laPalace) {

/*    ELEMENTAL:
      items[itOrbGhost] = 999;
      int h = fiftyval(c);
      eWall colorwalls[8] = {
        waNone, waSea, waCIsland, waFloorA, waColumn, waDryTree, waChasm, waMirror
        };
      c->wall = colorwalls[h&7];
*/

      if(cdist50(c) == 3 && polarb50(c) == 1)
        c->wall = waPalace;
      }
  
    if(d == 8 && c->land == laPalace) {
    
      // note: Princess Challenge brings back the normal Palace generation
      bool lookingForPrincess = !euclid && c->master->alt && !princess::challenge;

      if(cdist50(c) == 3 && polarb50(c) == 1) {
        int q = 0, s = 0;
        if(!ishept(c)) for(int i=0; i<6; i++)
          if(cdist50(c->mov[i]) == 3 && polarb50(c->mov[i]) == 1 && !ishept(c->mov[i]))
            q++, s += i;
        if(q == 1 && c->mov[s]->land == laPalace) {
          switch(princess::generating ? 0 : hrand(2)) {
            case 0: 
              c->wall = waClosedGate;
              c->mov[s]->wall = waClosedGate;
              break;
            case 1:
              c->wall = waOpenGate;
              c->mov[s]->wall = waOpenGate;
              break;
            }
          }
        }
      else if((hrand(100) < (lookingForPrincess ? 7 : 5) && cdist50(c)) ||
        (cdist50(c) == 0 && polarb50(c) && hrand(100) < 60)) {
        c->wall = hrand(100) < (lookingForPrincess ? 30:50) ? waClosePlate : waOpenPlate;
        }
      else if(hrand(100) < (lookingForPrincess ? 3 : 5))
        c->wall = waTrapdoor;
      }
  
    if(d==8 && c->land == laEmerald) {
      if(randomPatternsMode)
        c->wall = RANDPAT3(0) ? waCavewall : waCavefloor;
      else if(euclid) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        if(((y-2)&7) < 4) c->wall = waCavewall;
        else c->wall = waCavefloor;
        }
      else {
        int v = emeraldval(c);
        if((v&3) >= 2) 
          c->wall = waCavewall;
        else c->wall = waCavefloor;
        }
      }
    
    if(d == 8 && isIcyLand(c)) c->landparam = 0;
    if(d == 8 && c->land == laDryForest) c->landparam = 0;

    if(d==8 && c->land == laPower) {
      int v;
      if(randomPatternsMode)
        v = RANDPAT ? 24 : 0;
      else if(euclid) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        int y0 = ((short)y) % 6;
        if(y0<0) y0+=6;
        if(y0 == 3 || y0 == 4) v=24; else v=0;
        }
      else v = emeraldval(c);
      v &= ~3;
      if((v == 24 || v == 32 || v == 56))
        c->wall = waEternalFire;
      else if(hrand(100) < 10) {
        c->wall = waGlass;
        eItem protectedItems[18] = {
          itPower, itPower, itPower, itPower, itPower, itPower,
          itOrbLightning, itOrbLightning, itOrbThorns, itOrbThorns,
          itOrbInvis, itOrbInvis,
          itOrbShield, itOrbTeleport, itOrbPsi,
          itOrbDragon, itOrbIllusion, itOrbPreserve
          };
        c->item = protectedItems[hrand(18)];
        }
      }

    if(d==8 && c->land == laZebra) {
      if(euclid) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        if(y&1) c->wall = waTrapdoor;
        else c->wall = waNone;
        }
      else
      c->wall = (randomPatternsMode ? RANDPAT : (zebra40(c)&2)) ? waTrapdoor : waNone;
      }
    
    if(d==8 && isElemental(c->land)) {
      if(hrand(c->land == laEAir ? 6 : 25) == 0) {
        if(c->land == laEFire) c->wall = waEternalFire;
        else if(c->land == laEWater) c->wall = waSea;
        else if(c->land == laEAir) c->wall = waChasm;
        else if(c->land == laEEarth) c->wall = waStone;
        }
      }
    
    if(d==8 && c->land == laWineyard) {
      if(euclid) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        int dy = ((short)y)%3; if(dy<0) dy += 3;
        if(dy == 1) c->wall = waVinePlant;
        }
      else {
        int v = emeraldval(c);
        int w = v / 4;
        if(randomPatternsMode) c->wall = RANDPAT ? waVinePlant : waNone;
        else if(w == 9 || w == 10 || w == 7 || w == 8) {
          c->wall = waVinePlant;
          }
        else if(v == 24 || v == 58 || v == 26 || v == 56)
          c->wall = waVineHalfA;
        else if(v == 25 || v == 59 || v == 27 || v == 57)
          c->wall = waVineHalfB;
        else c->wall = waNone;
        }
      }
    
    if(d == 7 && cellHalfvine(c)) {
      int i = -1;
      for(int k=0; k<c->type; k++) if(c->mov[k] && c->mov[k]->wall == c->wall)
        i = 0;
      if(i == -1) c->wall = waNone;
      }

    // 24-58
    // 26-56
    if(d == 9) {
    
      // if(c->land == laIce && ((celldist(c) % 10) + 10) % 10 == 5)
      //   c->wall = waColumn;
        
      if(c->land == laIce) {
        if(randomPatternsMode) c->wall = RANDPAT ? waIcewall : waNone;
        else if(hrand(100) < 5 && c->wall != waBarrier) {
          c->wall = waIcewall;
          for(int i=0; i<c->type; i++) if(hrand(100) < 50) {
            cell *c2 = createMov(c, i);
            setdist(c2, d+1, c);
            if(c2->wall == waBarrier || c2->land != laIce) continue;
            c2->wall = waIcewall;
            for(int j=0; j<c2->type; j++) if(hrand(100) < 20) {
              cell *c3 = createMov(c2, j);
              setdist(c->mov[i], d+2, c);
              if(c3->wall == waBarrier || c3->land != laIce) continue;
              c3->wall = waIcewall;
              }
            }
          }
        }

      if(c->land == laIce || c->land == laCocytus) if(c->wall == waIcewall && items[itDiamond] >= 5 && hrand(200) == 1)
        c->wall = waBonfireOff;

      if(c->land == laCaves) 
        c->wall = (randomPatternsMode ? RANDPAT3(1) : hrand(100) < 55) ? waCavewall : waCavefloor;
      
      if(c->land == laLivefjord) { 
        int die = (randomPatternsMode ? (RANDPAT3(2)?100:0) : hrand(100));
        if(die < 50)
          c->wall = waSea;
        else
          c->wall = waNone;
        }
      
      if(c->land == laDeadCaves) {
        int die = (randomPatternsMode ? (RANDPAT?100:0) : hrand(100));
        if(die<50) c->wall = waDeadwall;
        else if(die<55) c->wall = waDeadfloor2;
        else c->wall = waDeadfloor;
        }
      
      if(c->land == laAlchemist) 
        c->wall = (randomPatternsMode ? RANDPAT : hrand(2)) ? waFloorA : waFloorB;
      
      if(c->land == laDryForest) {
        if(randomPatternsMode)
          c->wall = RANDPAT ? waNone : RANDPATV(laHell) ? waDryTree : waWetTree;
        else
          c->wall = 
            (hrand(100) < 50) ? (hrand(100) < 50 ? waDryTree : waWetTree) : waNone;
        }
      
      if(c->land == laGraveyard) {
        if(randomPatternsMode)
          c->wall = RANDPAT ? ((RANDPATV(laCrossroads) || RANDPATV(laCrossroads2)) ? waAncientGrave : waFreshGrave) : waNone;
        else if(ishept(c)) 
          c->wall = hrand(5) ? waAncientGrave : waFreshGrave;
        }
      
      if(c->land == laRlyeh)  {
        if(randomPatternsMode) {
          c->wall = RANDPAT ? waColumn : waNone;
          }
        else {
          if(hrand(500) < 5) {
            for(int i=0; i<c->type; i++) {
              cell *c2 = createMov(c, i);
              setdist(c2, d+1, c);
              if(c2 && c2->land == laRlyeh)
                c2->wall = waColumn;
              }
            
            for(int j=0; j<2; j++) {
              int i = hrand(c->type);
              if(c->mov[i] && c->mov[i]->land == laRlyeh)
                c->mov[i]->wall = waNone;
              }
            }
          if(ishept(c) && hrand(2)) c->wall = waColumn;
          }
        }
      
      if(c->land == laHell) {
        if(hrand(1000) < 36) {
          for(int i=0; i<c->type; i++) {
            cell *c2 = createMov(c, i);
            setdist(c2, d+1, c);
            if(c2 && c2->land == laHell)
              if(c2->wall != waSulphurC)
                c2->wall = waSulphur;
            }
          
          c->wall = waSulphurC;
          }
        }
      
      if(c->land == laCocytus)  {
        if(c->wall == waNone) c->wall = waFrozenLake;
        if(hrand(100) < 5) {
          for(int i=0; i<c->type; i++) {
            cell *c2 = createMov(c, i);
            setdist(c2, d+1, c);
            if(c2 && c2->land == laCocytus)
              c2->wall = waLake;
            }
          
          c->wall = waLake;

          if(hrand(500) < 100 + 2 * (items[itSapphire]))
            c->monst = moShark;
          }
        }

      if(isHive(c->land) && hrand(2000) < 2) 
        createBugArmy(c);
      
      if(c->land == laPalace && !euclid && c->master->alt) {
        int d = celldistAlt(c);
        if(d <= 150) generateAlts(c->master);
        }
      
      if((c->land == laCrossroads  && !euclid) || c->land == laCamelot) 
      if(euclid || c->master->alt) {
        int d = celldistAltRelative(c);
        {
          eucoord x,y;
          decodeMaster(c->master, x, y);
          }
        if(d <= 14 && roundTableRadius(c) > 20) {
          if(!euclid) generateAlts(c->master);
          c->bardir = NOBARRIERS;
          int d = celldistAltRelative(c);
          if(d == 10) {
            if(ishept(c)) buildCamelotWall(c);
            else {
              if(!euclid) for(int i=0; i<7; i++) generateAlts(c->master->move[i]);
              int q = 0;
              for(int t=0; t<6; t++) {
                createMov(c, t);
                if(celldistAltRelative(c->mov[t]) == 10 && !ishept(c->mov[t])) q++;
                }
              if(q == 1) buildCamelotWall(c);
              // towers of Camelot
              if(q == 0) {
                c->monst = moKnight;
                for(int i=0; i<6; i++) 
                  buildCamelotWall(c->mov[i]);
                for(int i=0; i<c->type; i++) if(celldistAltRelative(c->mov[i]) < d)
                  c->mondir = i;
                }
              }
            }
          if(d == 0) c->wall = waRoundTable;
          if(celldistAlt(c) == 0) c->item = itHolyGrail;
          if(d < 0 && hrand(7000) <= 10 + items[itHolyGrail] * 5) {
            eMonster m[3] = { moHedge, moLancer, moFlailer };
            c->monst = m[hrand(3)];
            }
          if(d == 1) {
            // roughly as many knights as table cells
            if(hrand(1720) < 1000) 
              c->monst = moKnight;
            if(!euclid) for(int i=0; i<7; i++) generateAlts(c->master->move[i]);
            for(int i=0; i<c->type; i++) 
              if(c->mov[i] && celldistAltRelative(c->mov[i]) < d)
                c->mondir = (i+3) % 6;
            }
          if(d <= 10) c->land = laCamelot;
          if(d > 10 && !euclid) c->land = laCrossroads;
          }
        }
      
      if((c->land == laRlyeh && !euclid) || c->land == laTemple) {
        if(euclid || (c->master->alt && c->master->alt->distance <= 2)) {
          if(!euclid) generateAlts(c->master);
          c->bardir = NOBARRIERS;
          int d = celldistAlt(c);
          if(d <= 0) {
            c->land = laTemple, c->wall = waNone, c->monst = moNone, c->item = itNone;
            }
          if(d % TEMPLE_EACH==0) {
            if(ishept(c)) 
              c->wall = waColumn;
            else {
              if(!euclid) for(int i=0; i<7; i++) generateAlts(c->master->move[i]);
              int q = 0;
              for(int t=0; t<6; t++) {
                createMov(c, t);
                if(celldistAlt(c->mov[t]) % TEMPLE_EACH == 0 && !ishept(c->mov[t])) q++;
                }
              if(q == 2) c->wall = waColumn;
              }
            }
          }
        }

      if(c->land == laOcean || c->land == laWhirlpool) {
        if(euclid || (c->master->alt && c->master->alt->distance <= 2)) {
          if(!euclid) generateAlts(c->master);
          c->bardir = NOBARRIERS;
          int dd = celldistAlt(c);
          if(dd <= 0) {
            c->land = laWhirlpool, c->wall = waSea, c->monst = moNone, c->item = itNone;
            }
          }
        }
      
      if(c->land == laCaribbean) {
        if(!euclid) {
          if(c->master->alt && c->master->alt->distance <= 2) {
            if(!euclid) generateAlts(c->master);
            c->bardir = NOBARRIERS;
            int d = celldistAlt(c);
            if(d <= 0) 
              // c->wall = waChasm;
              generateTreasureIsland(c);
            else
              c->wall = waSea;
            }
          else c->wall = waSea;
          }
        else {
          int d = celldistAlt(c);
          if(d <= 0) 
            generateTreasureIsland(c);
          else
            c->wall = waSea;
          }
        }
      
      if(isHive(c->land) && hrand(2000) < 100 && !c->wall && !c->item && !c->monst) {
        int nww = 0;
        for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->wall == waWaxWall)
          nww++;
        if(nww == 0) {
          c->wall = waWaxWall;
          c->monst = moNone;
          c->landparam = rand() & 0xFFFFFF;
          }
        /* for(int i=0; i<c->type; i++) {
          if(hrand(6) < 5) {
            createMov(c,i);
            cell *c2 = c->mov[i];
            c2->wall = waWaxWall;
            c2->monst = moNone;
            }
          } */
        }
      
      if(c->land == laDesert) {
        if(randomPatternsMode)
          c->wall = RANDPAT ? waDune : waNone;
        else {
          if(hrand(100) < 5) {
            for(int i=0; i<c->type; i++) {
              cell *c2 = createMov(c, i);
              setdist(c2, d+1, c);
              if(c2 && c2->land == laDesert)
                c2->wall = waDune;
              }
            
            for(int j=0; j<2; j++) {
              int i = hrand(c->type);
              if(c->mov[i] && c->mov[i]->land == laDesert)
                c->mov[i]->wall = waNone;
              }
            }
          }
  
        if(hrand(300) == 1 && items[itSpice] >= 5) c->wall = waThumperOff;
        }

      if(c->land == laRedRock) {
        if(randomPatternsMode) {
          c->wall = waNone;
          if(!ishept(c)) { if(RANDPAT) buildRedWall(c, 20); }
          else {
            int k = 0;
            for(int i=0; i<20; i++) 
              if(RANDPATC(c->mov[i%7]) && !RANDPATC(c->mov[(i+1)%7]))
                k++;
            if(k>=4) buildRedWall(c, 20);
            }
          }
        else if(ishept(c) && hrand(100) < 8 && !c->monst) {
          buildRedWall(c, 80);
          int i = hrand(7);
          buildRedWall(createMovR(c, i), 33);
          if(hrand(2) == 0) 
            buildRedWall(createMovR(createMovR(c, i), c->spn[i]+(hrand(2)?2:4)), 20);
          i += 3 + hrand(2);
          if(hrand(6) < 4)
            buildRedWall(createMovR(c, i), 33);
          if(hrand(2) == 0) 
            buildRedWall(createMovR(createMovR(c, i), c->spn[i]+(hrand(2)?2:4)), 20);
          }
        }
      }
    
    if(d == 8 && c->land == laCaribbean && !euclid) {
      int mindist  = 9;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if((euclid || c2->master->alt) && celldistAlt(c2) < mindist)
          mindist = celldistAlt(c2);
        }
      if(mindist == 0) beCIsland(c);
      }
    
    if(d == 7 && c->land == laRedRock && c->wall == waNone && hrand(1000) == 0)
      buildPrizeMirror(c);
    
    if(d == 7 && c->land == laCaribbean && c->wall == waSea) {
      bool coast = false;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if(passable(c2, c, false, false, false) && c2->wall != waBoat) coast = true;
        }
      if(coast && hrand(10) < 5) {
        c->wall = waBoat;
        if(items[itPirate] >= 10 && hrand(100) < 2)
          c->item = itOrbPreserve;
        else if(hrand(100) < 2*PRIZEMUL)
          placePrizeOrb(c);
        }
      }

    if(d == 7 && c->land == laCaves && c->wall == waCavewall && hrand(5000) < items[itGold])
      c->monst = moSeep;
    
    if(d == 7 && c->land == laLivefjord && c->wall == waSea && hrand(5000) < 15 + items[itFjord]) {
      if(items[itFjord] >= 5 && hrand(100) < 20)
        c->monst = moWaterElemental;
      else {
        c->monst = moViking;
        c->wall = waBoat;
        }
      }
    
    if(d == 7 && c->land == laLivefjord && hrand(2000) < 50 + kills[moViking] && notDippingFor(itFjord)) {
      c->item = itFjord;
      }
    
    if(d == 7 && c->land == laLivefjord && items[itFjord] >= 10 && hrand(2000) < 2)
      c->item = itOrbFish;
    
    if(d == 7 && c->land == laLivefjord && hrand(2000) < 2*PRIZEMUL)
      placePrizeOrb(c);
    
    if(d == 7 && c->land == laLivefjord && hrand(2000) < 2)
      buildPrizeMirror(c);
    
    if(d == 7 && c->land == laEmerald && c->wall == waCavewall && hrand(5000) < items[itEmerald])
      c->monst = moSeep;
    
    if(d == 7 && c->land == laDeadCaves && c->wall == waDeadwall && hrand(1000) < items[itSilver])
      c->monst = moSeep;
    
    if(d == 7 && c->wall == waVinePlant && hrand(100) < (randomPatternsMode ? 2 : 10))
      c->monst = moVineSpirit;

    if(d == 7 && c->land == laOcean && !safety) {
      if(c->landparam >= 1 && c->landparam <= 25) {
        if(hrand(1000) < 5)
          c->wall = waBoat;
        if(hrand(1000) < 50 + kills[moAlbatross]/2)
          c->item = itCoast;
        if(hrand(15000) < 10 + 2 * items[itCoast])
          c->monst = moAlbatross;
        if(items[itCoast] >= 10 && hrand(10000) < 5)
          c->item = itOrbAir;
        if(hrand(10000) < 5*PRIZEMUL)
          placePrizeOrb(c);
        if(hrand(10000) < 5)
          buildPrizeMirror(c);
        }
      else if(c->landparam > 25) {
        if(hrand(30000) < 10) {
          c->wall = waBoat;
          c->monst = moPirate;
          // orbs are possible!
          placeOceanOrbs(c);
          }
        else if(hrand(30000) < 10)
          c->monst = moCShark;
        }
      }

    if(d == 7 && c->land == laCaribbean && c->wall == waSea && hrand(10000) < 20 + items[itPirate])
      c->monst = moCShark;
    
    if(d == 7 && c->wall == waCTree && hrand(5000) < 100 + items[itPirate])
      c->monst = moParrot;
    
    // repair the buggy walls flowing in from another land, like ice walls flowing into the Caves
    if(d == 7 && c->land == laCaves && c->wall != waCavewall && c->wall != waCavefloor)
      c->wall = waCavefloor;
    
    if(d == 7 && c->land == laDeadCaves && c->wall != waDeadwall && 
      c->wall != waDeadfloor && c->wall != waDeadfloor2 && c->wall != waEarthD)
      c->wall = waDeadfloor2;
    
    if(d == 7 && c->land == laCocytus && c->wall != waFrozenLake && c->wall != waLake && c->wall != waIcewall)
      c->wall = waFrozenLake;
    
    if(d == 7 && c->land == laAlchemist && c->wall != waFloorA && c->wall != waFloorB)
      c->wall = waFloorA;
    
    if(d == 3 && c->land == laMinefield && safety && c->wall == waMineMine)
      c->wall = waMineOpen;
    
    if(d == 7 && c->land == laMinefield) {
      c->wall = waMineUnknown;
      // 250: rare mines
      // 1250: at 25
      int minefreq = 0;
      int treas = items[itBombEgg];
      if(treas <= 10) minefreq = 250 + 30 * treas;
      if(treas <= 110) minefreq = 550 + 10 * (treas-10);
      else minefreq = 1550 + (treas - 110);
      
      // experimentation says that 600 is slightly too hard to find the Orb of Yendor
      if(generatingYendor || yendorPath)
        if(minefreq < 550)
          minefreq = 550;
      
      int tfreq = 
        treas < 10 ? 50 + 5 * treas :
        treas < 25 ? 100  + (treas-10) * 2:
        treas < 50 ? 150 + (treas-25) :
        175;

      if(hrand(5000) < minefreq)
        c->wall = waMineMine;
      else if(hrand(5000) < tfreq && !safety) {
        c->item = itBombEgg;
        c->landparam = items[itBombEgg] + 5 + hrand(11);
        }
      else if(hrand(5000) < treas - 20 + items[itOrbYendor] * 5 && !safety)
        c->monst = moBomberbird;
      else if(treas >= 10 && hrand(5000) < 10 && !safety)
        c->item = itOrbFriend;
      else if(hrand(5000) < 10*PRIZEMUL && !safety)
        placePrizeOrb(c);
      }

    if(d == 7 && c->wall == waIcewall && c->land != laIce && c->land != laCocytus)
      c->wall = waNone;
    
    if(d == 7 && c->wall == waChasmD) {
      chasmify(c);
      }
      
    if(d == 7 && c->wall == waEarthD) {
      chasmifyEarth(c);
      }
    
    if(d == 7 && c->wall == waElementalD) {
      chasmifyElemental(c);
      }
    
    // seal entrances to the Land of Power.
    if(d == 7 && c->land == laPower && c->type == 7) {
      bool onwall = false;
      for(int i=0; i<7; i++) if(c->mov[i] && c->mov[i]->land != laPower)
        onwall = true;
      if(!onwall) for(int i=0; i<7; i++) {
        cell *c2 = c->mov[i];
        cell *c3 = c2->mov[(c->spn[i] + 3) % 6];
        if(c3->land != laPower && c3->land != laBarrier)
        if(c2->wall != waFire && c2->wall != waGlass) {
          if(c->wall == waFire) c->monst = moWitchWinter;
          else if(c->wall == waGlass) c->monst = moWitchGhost;
          else c->monst = moEvilGolem;
          }
        }
      }

    if(d == 7 && passable(c, NULL, false, false, false)) {
      if(c->land == laBarrier) c->wall = waBarrier;
      if(c->land == laOceanWall) c->wall = c->type == 7 ? waBarrier : waSea;
      }

    if(d == 7 && c->land == laWhirlpool) 
      whirlGenerate(c);
    
    if(c->land == laPalace && princess::generating) {
      // no Opening Plates nearby
      if(d <= 7 && c->wall == waOpenPlate)
        c->wall = waNone;
      // no monsters nearby
      if(d>0) c->monst = moNone;
      // no Plates or Trapdoors in the Princess cell
      if(d < 3 && (c->wall == waClosePlate || c->wall == waOpenPlate || c->wall == waTrapdoor))
        c->wall = waNone;
      if(d > 1) c->item = itNone;
      // the Princess herself
      if(d == 0) {
        c->monst = moPrincess;
        c->hitpoints = palaceHP();
        c->wall = waGiantRug;
        cell *c2 = NULL;
        for(int i=0; i<c->type; i++) {
          cellwalker cw(c, i);
          cwstep(cw);
          cwspin(cw, 4);
          cwstep(cw);
          cwspin(cw, 2);
          cwstep(cw);
          cwspin(cw, 4);
          cwstep(cw);
          cwspin(cw, 2 + hrand(3));
          cwstep(cw);
          if(!c2) c2 = cw.c;
          else if(celldist(cw.c) > celldist(c2)) c2 = cw.c;
          cw.c->monst = moMouse;
          }
        c2->wall = waOpenPlate;
        }
          
      }
    
    if(d == 7 && passable(c, NULL, false, false, false) && !safety) {
      int hard = items[itOrbYendor] * 5;
      
      if(c->land == laEdge) {
        if(hrand(20000) < 20 + items[itEdge]) {
          if(cellEdgeUnstable(c))
            c->monst = moGargoyle;
          else
            c->monst = moEdgeMonkey;
          }
        else if(c->landparam >= 14 && hrand(2000) < 50+kills[moGargoyle]+kills[moEdgeMonkey] && !cellEdgeUnstable(c) ) {
          c->item = itEdge;
          }
        }
    
      if(c->land == laIce) {
        if(hrand(5000) < 100 + 2 * (kills[moYeti] + kills[moWolf]) && notDippingFor(itDiamond))
          c->item = itDiamond;
        if(hrand(8000) < 2 * (items[itDiamond] + hard))
          c->monst = hrand(2) ? moYeti : moWolf;
        }
      if(c->land == laPalace) {
        bool lookingForPrincess0 = !euclid && c->master->alt;
        bool lookingForPrincess = lookingForPrincess0 && !princess::challenge;
        int hardness = lookingForPrincess ? 5 : items[itPalace] + hard;
        
        if(hrand(5000) < 100 + 2 * (kills[moPalace] + kills[moFatGuard] + kills[moVizier] + kills[moSkeleton]) && notDippingFor(itPalace) &&
          c->wall != waOpenGate && !lookingForPrincess0)
          c->item = itPalace;
        if(items[itPalace] >= 10 && hrand(5000) < 16 && c->wall != waOpenGate)
          c->item = hrand(100) < 80 ? itOrbFrog : itOrbDiscord;
        if(hrand(5000) < 20*PRIZEMUL && c->wall != waOpenGate)
          placePrizeOrb(c);
        if(hrand(5000) < 20 && c->wall == waNone)
          buildPrizeMirror(c);
        if(c->land == laPalace && (euclid || c->master->alt) && celldistAlt(c) <= 150 && !havemouse && !princess::generating &&
          (euclid || (princess::getPrisonInfo(c)->bestdist < 6 && princess::getPrisonInfo(c)->princess))) {
          c->monst = moMouse;
          addMessage(XLAT("You hear a distant squeak!"));
          drawFlash(c);
/*          {
  cell *c2= c;          
  z:
  c2->item = itPirate;
  printf("AT %p\n", c2);
  for(int i=0; i<c2->type; i++) if(c2->mov[i] && c2->mov[i]->mpdist < c2->mpdist) {
    c2 = c2->mov[i];
    goto z;
    } 
          } */
          havemouse = true;
          }
        else if(hrand(15000) < 10 + hardness) {
          c->monst = moPalace;
          c->hitpoints = palaceHP();
          if(hrand(10 + items[itPalace]) >= 14 && !lookingForPrincess)
            c->monst = moSkeleton;
          }
        else if(hrand(20000) < hardness) {
          c->monst = moFatGuard;
          c->hitpoints = palaceHP();
          }
        else if(hrand(20000) < hardness - 7) {
          c->monst = moVizier;
          c->hitpoints = palaceHP();
          }
        else if(princess::forceVizier && from->pathdist != INF) {
          c->monst = moVizier;
          c->hitpoints = palaceHP();
          princess::forceVizier = false;
          }
        }
      if(c->land == laCaves) {
        if(hrand(5000) < 100 + 2 * min(kills[moTroll] + kills[moGoblin], 150) && notDippingFor(itGold))
          c->item = itGold;
        if(hrand(8000) < 10 + 2 * (items[itGold] + hard))
          c->monst = hrand(2) ? moTroll : moGoblin;
        }
      if(c->land == laLivefjord) {
        if(hrand(16000) < 10 + 2 * (items[itFjord] + hard))
          c->monst = moFjordTroll;
        }
      if(c->land == laDeadCaves) {
        if(hrand(5000) < 100 + 2 * (kills[moDarkTroll] + kills[moEarthElemental]) && notDippingFor(itSilver))
          c->item = itSilver;
        if(hrand(16000) < (items[itSilver] + hard)) {
          c->monst = moEarthElemental;
          for(int i=0; i<c->type; i++) {
            cell *c2 = c->mov[i];
            earthFloor(c2);
            }
          for(int i=0; i<c->type; i++) if(c->mov[i]->mpdist < c->mpdist) c->mondir = i;
          chasmifyEarth(c); c->wall = waDeadfloor2;
          }
        else if(hrand(8000) < 60 + 8 * (items[itSilver] + hard)) {
          if(hrand(100) < 25) {
            }
          else c->monst = hrand(2) ? moDarkTroll : moGoblin;
          }
        }
      if(c->land == laDesert) {
        if(hrand(5000) < 100 + 2 * (kills[moWorm] + kills[moDesertman]) && notDippingFor(itSpice))
          c->item = itSpice;
        if(hrand(8000) < 10 + 2 * (items[itSpice] + hard))
          c->monst = hrand(2) ? moWorm : moDesertman,
          c->mondir = NODIR;
        }
      if(c->land == laRedRock) {
        if(hrand(16000) < 30+items[itRedGem] && !ishept(c)) {
          int i = -1;
          for(int t=0; t<6; t++) if(c->mov[t]->mpdist > c->mpdist && !ishept(c->mov[t]))
            i = t;
          if(i != -1) {
            c->monst = moHexSnake;
            c->bardir = NOBARRIERS;
            int len = ROCKSNAKELENGTH;
            cell *c2 = c;
            while(--len) {
              c2->bardir = NOBARRIERS;
              c2->mondir = i;
              createMov(c2, i);
              int j = c2->spn[i];
              c2 = c2->mov[i];
              c2->monst = moHexSnakeTail;
              if(c2->bardir != NODIR) return;
              i = (j + (len%2 ? 2 : 4)) % 6;
              }
            c2->mondir = NODIR;
            }
          }
        else if(hrand(16000) < 50+items[itRedGem] && !ishept(c))
          c->monst = moRedTroll,
          c->mondir = NODIR;
        }
      if(c->land == laWineyard) {
        if(hrand(5000) < 100 + 2 * (kills[moVineBeast] + kills[moVineSpirit]) && notDippingFor(itWine))
          c->item = itWine;
        if(hrand(8000) < 12 * (items[itWine] + hard))
          c->monst = moVineBeast;
        }
      if(c->land == laZebra) {
        if(c->wall == waNone && hrand(2500) < 100 + 2 * (kills[moOrangeDog]) && notDippingFor(itZebra))
          c->item = itZebra;
        if(hrand(10000) < 40 + 2*(items[itZebra] + hard))
          c->monst = moOrangeDog;
        }
      if(isElemental(c->land) && c->land != laElementalWall) {
        eItem localshard = eItem(itFireShard + (c->land - laEFire));
        int danger = 5 * items[localshard] * items[localshard];
        eMonster elof = elementalOf(c->land);
        int elkills = kills[elof];
        
        if(hrand(8000) < 12 + (items[itElemental] + danger + hard)) {
          c->monst = elof;
          if(c->land != laEAir) chasmifyElemental(c);
          c->wall = waNone;
          if(c->land == laEWater) c->wall = waSea;
          }
        else if(hrand(5000) < 100 + elkills*3 && notDippingFor(itElemental))
          c->item = localshard;
        else if(hrand(5000) < 10 && items[itElemental] >= 10)
          c->item = itOrbSummon;
        else if(hrand(5000) < 10*PRIZEMUL)
          placePrizeOrb(c);
        }
      if(c->land == laEmerald) {
        if(hrand(1000) < 100 + 2 * (kills[moMiner] + kills[moLancer] + kills[moFlailer]) && notDippingFor(itEmerald)) {
          // do not destroy walls!
          bool ok = true;
          for(int i=0; i<c->type; i++) if(c->mov[i]->wall == waCavewall) ok = false;
          if(ok) c->item = itEmerald;
          }
        if(hrand(8000) < 50 + 10 * (items[itEmerald] + hard)) {
          static eMonster emeraldmonsters[4] = { moHedge, moLancer, moFlailer, moMiner };
          c->monst = emeraldmonsters[hrand(4)];
          }
        }
      if(c->land == laJungle) {
        if(hrand(5000) < 25 + 2 * (kills[moIvyRoot] + kills[moMonkey]) && notDippingFor(itRuby))
          c->item = itRuby;
        if(hrand(15000) < 5 + 1 * (items[itRuby] + hard))
          c->monst = moMonkey;
        else if(hrand(80000) < 5 + items[itRuby] + hard)
          c->monst = moEagle;
        else if(ishept(c) && hrand(4000) < 300 + items[itRuby]) {
          bool hard = hrand(100) < 25;
          if(hard ? buildIvy(c, 1, 9) : buildIvy(c, 0, c->type))
            c->item = itRuby;
          }
        }
      if(c->land == laAlchemist) {
        if(hrand(5000) < 25 + min(kills[moSlime], 200) && notDippingFor(itElixir))
          c->item = itElixir;
        else if(hrand(3500) < 10 + items[itElixir] + hard)
          c->monst = moSlime;
        }
      if(c->land == laPower) {
        if(hrand(5000+50*items[itPower]) < 1200) {
          eItem powerorbs[6] = {
            itOrbFlash, itOrbSpeed, itOrbFire, itOrbWinter, itOrbGhost, itOrbLife};
          c->item = powerorbs[hrand(6)];
          }
        else if(c->type == 6 && hrand(5000) < 10) 
          c->wall = hrand(2) ? waMirror : waCloud;
        else if(hrand(1000) < 10 + (items[itPower] ? 10:0) + (items[itPower] + hard))
          c->monst = eMonster(moWitch + hrand(NUMWITCH));
        }
      if(isCrossroads(c->land)) {
        if(c->type == 6 && hrand(8000) < 120 && items[itShard] >= 10)
          c->wall = hrand(2) ? waMirror : waCloud;
        else {
          if(hyperstonesUnlocked() && hrand(25000) < tkills() && notDippingFor(itHyperstone))
            c->item = itHyperstone;
          if(hrand(4000) < items[itHyperstone] + 2 * items[itHolyGrail]) {
            // only interesting monsters here!
            eMonster cm = crossroadsMonster();
            if(cm == moIvyRoot) buildIvy(c, 0, c->type);
            else c->monst = cm;
            if(cm == moWorm || cm == moTentacle)
              c->mondir = NODIR;
            }
          }
        }
      if(c->land == laMirror) {
        if(c->type == 6 && hrand(5000) < 120 && notDippingFor(itShard))
          c->wall = hrand(2) ? waMirror : waCloud;
        else if(c->type == 7 && hrand(5000) < 10 * PRIZEMUL)
          placePrizeOrb(c);
        else if(hrand(12000) < 8 + items[itShard] + hard)
          c->monst = moRanger;
        else if(hrand(60000) < 8 + items[itShard] + hard)
          c->monst = moEagle;
        }
      if(c->land == laGraveyard) {
        if(hrand(5000) < 30 + 2 * (kills[moZombie] + kills[moGhost] + kills[moNecromancer]) && notDippingFor(itBone))
          c->item = itBone;
        if(hrand(20000) < 10 + items[itBone] + hard + (kills[moZombie] + kills[moGhost] + kills[moNecromancer])/60) {
          eMonster grm[6] = { moZombie, moZombie, moZombie, moGhost, moGhost, moNecromancer};
          c->monst = grm[hrand(6)];
          }
        }
      if(c->land == laRlyeh) {
        if(hrand(5000) < 30 + 2 * (kills[moCultist] + kills[moTentacle] + kills[moPyroCultist]) && notDippingFor(itStatue))
          c->item = itStatue;
        if(hrand(8000) < 5 + items[itStatue] + hard)
          c->monst = moTentacle, c->item = itStatue, c->mondir = NODIR;
        else if(hrand(12000) < 5 + items[itStatue] + hard)
          c->monst = hrand(3) ? ((hrand(40) < items[itStatue]-25) ? moCultistLeader : moCultist) : moPyroCultist;
        else if(hrand(8000) < 5 + items[itStatue] + hard && c->type == 6) {
          for(int t=0; t<c->type; t++) {
            if(c->mov[t] && c->mov[t]->monst == moNone && (c->wall == waNone || c->wall == waColumn))
              c->mov[t]->wall = ishept(c->mov[t]) ? waColumn : waNone;
            if(c->mov[t]->wall == waColumn)
              c->mov[t]->item = itNone;
            }
          if(buildIvy(c, 0, 3)) c->item = itStatue;
          }
        }
      if(c->land == laTemple) {
        // depth!
        int d = (euclid || c->master->alt) ? celldistAlt(c) : 10;
        // remember: d is negative
        if(d % TEMPLE_EACH == 0) {
          if(hrand(5000) < 20 - 2*d)
            c->monst = moTentacle, c->mondir = NODIR;
          }
        else {
          // int d0 = d % TEMPLE_EACH;
          // if(d0<0) d0=-d0;
          if(hrand(100) < 30) //  && d0 != 1 && d0 != TEMPLE_EACH-1)
            c->wall = waBigStatue;
          else if(hrand(20000) < -d)
            c->monst = hrand(3) ? moCultist : moPyroCultist;
          else if(hrand(100000) < -d)
            c->monst = moCultistLeader;
          else if(hrand(5000) < 250)
            c->item = itGrimoire;
          else if(hrand(5000) < 10 && -d > TEMPLE_EACH * 10)
            c->item = itOrbDragon;
          }
        }
      if(c->land == laDryForest) {
        if(hrand(5000) < 100 + 2 * (kills[moFireFairy]*2 + kills[moHedge]) && notDippingFor(itFernFlower))
          c->item = itFernFlower;
        if(hrand(4000) < 40 + items[itFernFlower] + hard)
          c->monst = moHedge;
        else if(hrand(8000) < 2 * items[itFernFlower] + hard)
          c->monst = moFireFairy;
        }
      if(c->land == laHell) {
        if(hrand(6000) < 120 + (kills[moLesser]) && notDippingFor(itHell))
          c->item = itHell;
        if(hrand(8000) < 40 + items[itHell] + hard)
          c->monst = moLesser;
        else if(hrand(24000) < 40 + items[itHell] + hard)
          c->monst = moGreater;
        }
      if(c->land == laCocytus) {
        if(hrand(5000) < 100 + 2 * (kills[moShark] + kills[moGreaterShark] + kills[moCrystalSage]) && notDippingFor(itSapphire))
          c->item = itSapphire;
        if(hrand(5000) < 2 * (items[itSapphire] + hard)) {
          eMonster ms[3] = { moYeti, moGreaterShark, moCrystalSage };
          c->monst = ms[hrand(3)];
          if(c->monst == moGreaterShark) c->wall = waLake;
          }
        }
      if(c->land == laMotion) {
        if(hrand(1500) < 30 + (kills[moRunDog]) && notDippingFor(itFeather))
          c->item = itFeather;
        if(hrand(20000) < 25 + items[itFeather] + hard) {
          c->monst = moRunDog;
          // preset the movement direction
          // this will make the dog go in the direction of the center,
          // if the player is unreachable/invisible
          for(int d=0; d<c->type; d++) if(c->mov[d] == from) {
            c->mondir = (d+3) % c->type;
            }
          chasmify(c);
          c->wall = shmup::on ? waNone : waChasm;
          }
        }
      if(c->land == laHive) {
        if(isHive(c->land) && hrand(6000) < (hivehard() - 15)) 
          c->monst = randomHyperbug();

        /* if(hrand(1500) < 30 + (kills[moBug0] + kills[moBug1] + kills[moBug2]) && notDippingFor(itRoyalJelly))
          c->item = itRoyalJelly; */
        /* if(hrand(2000) < 2)
          c->monst = eMonster(moBug0 + hrand(3)); */
        }
      if(c->land == laCaribbean) {
//      if(hrand(1500) < 60 && celldistAlt(c) <= -5)
//        c->item = itCompass;
        if(hrand(16000) < 40 + (items[itPirate] + hard))
          c->monst = moPirate;
        }
      if(!c->item && c->wall != waCloud && c->wall != waMirror) {
        if(isCrossroads(c->land))
          placeCrossroadOrbs(c);
        else
          placeLocalOrbs(c);
        }
      }
    }

#ifndef MOBILE
  if(d >= 7 && mapeditor::whichPattern)
    mapeditor::applyModelcell(c);
#endif
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

bool havebugs, haveearth, haveeagles, haveleader, havehex, havewhirlpool, havewater,
  haveair;

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

void checkTide(cell *c) {
  if(c->land == laOcean) {
    if(c->wall == waStrandedBoat || c->wall == waBoat)
      c->wall = c->landparam >= tidalphase ? waBoat : waStrandedBoat;
    if(c->wall == waSea || c->wall == waNone)
      c->wall = c->landparam >= tidalphase ? waSea : waNone;
    if(isFire(c) && c->landparam >= tidalphase)
      c->wall = waSea;
    }
  }

// calculate cpdist and pathdist
void bfs(bool tick = true) {

  calcTidalPhase(); 
    
  checkOnYendorPath();
  
  int dcs = size(dcal);
  for(int i=0; i<dcs; i++) dcal[i]->cpdist = INFD;
  worms.clear(); ivies.clear(); ghosts.clear(); golems.clear(); mirrors.clear();
  temps.clear(); tempval.clear(); targets.clear(); 
  statuecount = 0;
  hexsnakes.clear();
  havebugs = false;
  haveearth = false; haveeagles = false; haveleader = false; havehex = false;
  havemouse = false;
  havewater = false;
  havewhirlpool = false;
  haveair = false;
  
  dcal.clear(); reachedfrom.clear(); 

  int pqs = size(pathq);
  for(int i=0; i<pqs; i++) pathq[i]->pathdist = INFD;
  pathq.clear(); 

  for(int i=0; i<numplayers(); i++) {
    cell *c = playerpos(i);
    if(c->cpdist == 0) continue;
    c->cpdist = 0;
    checkTide(c);
    dcal.push_back(c);
    reachedfrom.push_back(hrand(c->type));
    if(!invismove) targets.push_back(c);
    }  

  int qb = 0;
  while(true) {
    int i, fd = reachedfrom[qb] + 3;
    cell *c = dcal[qb++];
    int d = c->cpdist;
    if(d == 7) { first7 = qb; break; }
    for(int j=0; j<c->type; j++) if(i = (fd+j) % c->type, c->mov[i]) {
      // printf("i=%d cd=%d\n", i, c->mov[i]->cpdist);
      cell *c2 = c->mov[i];
      
      if((c->wall == waBoat || c->wall == waSea) &&
        (c2->wall == waSulphur || c2->wall == waSulphurC))
        c2->wall = waSea;
      
      if(c2 && signed(c2->cpdist) > d+1) {
        c2->cpdist = d+1;
        
        // remove treasures
        if(c2->item && c2->cpdist == 7 && itemclass(c2->item) == IC_TREASURE &&
          items[c2->item] >= 20 + currentLocalTreasure)
            c2->item = itNone;

        if(c2->item == itBombEgg && c2->cpdist == 7 && items[itBombEgg] >= c2->landparam) {
          c2->item = itNone;
          if(!c2->monst) c2->monst = moBomberbird;
          }
      
        if(c2->item == itEdge && c2->cpdist == 7 && items[itEdge] >= c2->landparam) {
          c2->item = itNone;
          }
        
        if(!keepLightning) c2->ligon = 0;
        dcal.push_back(c2);
        reachedfrom.push_back(c->spn[i]);
        
        checkTide(c2);
                
        if(c2->wall == waBigStatue && c2->land != laTemple) 
          statuecount++;
        
        if(c2->land == laWhirlpool) havewhirlpool = true;
        
        if(c2->monst) {
          if(c2->monst == moHexSnake || c2->monst == moHexSnakeTail) {
            havehex = true;
            if(c2->monst == moHexSnake) hexsnakes.push_back(c2);
            else findWormIvy(c2);
            }
          else if(isGhostMover(c2->monst))
            ghosts.push_back(c2);
          else if(isWorm(c2) || isIvy(c2)) findWormIvy(c2);
          else if(isBug(c2)) {
            havebugs = true;
            targets.push_back(c2);
            }
          else if(isFriendly(c2)) {
            if(c2->monst != moMouse) targets.push_back(c2);
            if(c2->monst == moGolem) golems.push_back(c2);
            if(c2->monst == moKnight) golems.push_back(c2);
            if(c2->monst == moTameBomberbird) golems.push_back(c2);
            if(c2->monst == moMouse) { golems.push_back(c2); havemouse = true; }
            if(c2->monst == moPrincess || c2->monst == moPrincessArmed) golems.push_back(c2);
            if(c2->monst == moIllusion) {
              if(items[itOrbIllusion]) items[itOrbIllusion]--;
              else c2->monst = moNone;
              }
            if(isMimic(c2)) mirrors.push_back(c2);
            }
          else if(isAngryBird(c2->monst)) haveeagles = true;
          else if(isLeader(c2->monst)) haveleader = true;
          else if(c2->monst == moEarthElemental) haveearth = true;
          else if(c2->monst == moWaterElemental) havewater = true;
          if(c2->monst == moAirElemental) haveair = true;
          }
        // pheromones!
        if(c2->land == laHive && c2->landparam >= 50 && c2->wall != waWaxWall) 
          havebugs = true;
        if(c2->wall == waThumperOn)
          targets.push_back(c2);
        }
      }
    }
  
  reachedfrom.clear(); 
  for(int i=0; i<size(targets); i++) {
    targets[i]->pathdist = isPlayerOn(targets[i]) ? 0 : 1;
    pathq.push_back(targets[i]);
    reachedfrom.push_back(hrand(targets[i]->type));
    }

  int qtemp = size(temps);
  for(int i=0; i<qtemp; i++) temps[i]->monst = tempval[i];
  
  pathqm.clear();  

  qb = 0;
  for(qb=0; qb < size(pathq); qb++) {
    int fd = reachedfrom[qb] + 3;
    cell *c = pathq[qb];
    if(c->monst && !isBug(c) && !isFriendly(c)) { 
      pathqm.push_back(c); 
      continue; // no paths going through monsters
      }
    if(c->cpdist > 7) continue;
    int d = c->pathdist;
    for(int j=0; j<c->type; j++) {
      int i = (fd+j) % c->type; 
      // printf("i=%d cd=%d\n", i, c->mov[i]->cpdist);
      if(c->mov[i] && c->mov[i]->pathdist == INFD && !thruVine(c, c->mov[i]) &&
        passable(c->mov[i], d==0?NULL:c, true, false, true)) {
        c->mov[i]->pathdist = d+1;
        pathq.push_back(c->mov[i]); reachedfrom.push_back(c->spn[i]);
        }
      }
    }
  }

// initialize the game
void initgame() {

  if(firstland == laNone || firstland == laBarrier)
    firstland = laCrossroads;

  if(firstland == laOceanWall) firstland = laOcean; 
  if(firstland == laEdge) firstland = laCrossroads;
  if(firstland == laElementalWall) firstland = randomElementalLand();
    
  cwt.c = origin.c7; cwt.spin = 0;
  cwt.c->land = euclid ? euclidland : firstland;
  createMov(cwt.c, 0);

  for(int i=0; i<65536; i++) euland[i] = laNone;
  
  if(euclid && euclidland == laPrincessQuest) {
    cell *c = euclideanAtCreate(EPX, EPY);
    princess::generating = true;
    c->land = laPalace;
    for(int j=BARLEV; j>=0; j--) setdist(c, j, NULL);
    princess::generating = false;
    }

  if(cwt.c->land == laCrossroads2) {
    cwt.c->landparam = 12;
    createMov(cwt.c, 0)->landparam = 44;
    }

  // extern int sightrange; sightrange = 9;
  // cwt.c->land = laHell; items[itHell] = 10;
  for(int i=BARLEV; i>=0; i--) {
    setdist(cwt.c, i, NULL);
    verifycells(&origin);
    }
  if(cwt.c->land == laCocytus)
    cwt.c->wall = waFrozenLake;
  else if(cwt.c->land == laAlchemist || cwt.c->land == laCanvas)
    ;
  else if(cwt.c->land == laCaves || cwt.c->land == laEmerald)
    cwt.c->wall = waCavefloor;
  else if(cwt.c->land == laDeadCaves)
    cwt.c->wall = waDeadfloor2;
  else if(cwt.c->land == laCaribbean || cwt.c->land == laOcean || cwt.c->land == laWhirlpool || cwt.c->land == laLivefjord)
    cwt.c->wall = waBoat; // , cwt.c->item = itOrbYendor;
  else if(cwt.c->land == laMinefield)
    cwt.c->wall = waMineOpen;
  else
    cwt.c->wall = waNone;
  cwt.c->item = itNone;
  cwt.c->monst = moNone;
  
  if(shmup::on) shmup::init();
  
  if(!safety) {
    timerstart = time(NULL); turncount = 0; 
    sagephase = 0; hardcoreAt = 0;
    timerstopped = false;
    savecount = 0; savetime = 0;
    cheater = 0;
    if(!randomPatternsMode) {
      if(firstland != (princess::challenge ? laPalace : laIce)) cheater++;
      }
    if(princess::challenge) {
      kills[moVizier] = 1;
      princess::forceMouse = true;
      if(princess::everGotYendorVictory)
        items[itGreenStone] = 99;
      addMessage(XLAT("Welcome to %the1 Challenge!", moPrincess));
      addMessage(XLAT("The more Hypersian Rugs you collect, the harder it is.", moPrincess));
      }
    }
  else {
    safety = false;
    }
  
  // items[itGreenStone] = 100;
  // items[itOrbTeleport] = 100;
  
  /* items[itGold] = 10;
  items[itDiamond] = 10;
  items[itSpice] = 10;
  items[itElixir] = 10;
  items[itRuby] = 10;
  items[itShard] = 10;
  items[itSilver] = 10;
  items[itWine] = 10;
  items[itRoyalJelly] = 10;
  items[itEmerald] = 10;
  items[itFeather] = 10;
  kills[moYeti] = 90; */
  
  /*
  items[itGold] = 20;
  items[itDiamond] = 20;
  items[itSpice] = 20;
  items[itRuby] = 20;
  items[itElixir] = 20;
  */
  
  /*
  items[itOrbShield]    = 100;
  items[itOrbSpeed]     = 100;
  items[itOrbWinter]    = 100;
  items[itOrbLightning] = 100;
  */
  
  // items[itOrbLightning]    = 100;
  // items[itRuby]      = 100;
  
  // items[itOrbWinter]    = 1000;
  
  bfs();
  }

void toggleGates(cell *ct, eWall type, int rad) {
  if(!ct) return;
  if(ct->wall == waOpenGate && type == waClosePlate) {
    bool onWorm = false;
    if(isWorm(ct)) onWorm = true;
    for(int i=0; i<ct->type; i++) 
      if(ct->mov[i] && ct->mov[i]->wall == waOpenGate && isWorm(ct->mov[i])) onWorm = true;
    if(!onWorm) {
      ct->wall = waClosedGate, rad = 1;
      if(ct->item) {
        addMessage(XLAT("%The1 is crushed!", ct->item));
        ct->item = itNone;
        }
      }
    }
  if(ct->wall == waClosedGate && type == waOpenPlate)
    ct->wall = waOpenGate, rad = 1;
  if(rad) for(int i=0; i<ct->type; i++)
    toggleGates(ct->mov[i], type, rad-1);
  }

// effect of moving monster m from cf to ct
// this is called from moveMonster, or separately from moveIvy/moveWorm,
// or when a dead bird falls (then m == moDeadBird)

void moveEffect(cell *ct, cell *cf, eMonster m) {

  if(!survivesMine(m))
    explodeMine(ct);

  if(!ignoresPlates(m) && !survivesMine(m) && (ct->wall == waClosePlate || ct->wall == waOpenPlate))
    toggleGates(ct, ct->wall, 3);
    
  if(isPrincess(m)) princess::move(ct, cf);
  }

void updateHi(eItem it, int v) {
  if(v > hiitems[it]) hiitems[it] = v;
  }

void gainItem(eItem it) {
  int g = gold();
  items[it]++; updateHi(it, items[it]);
  achievement_collection(it, gold(), g);
  }

void playerMoveEffects(cell *c2) {    
  bool nomine = (c2->wall == waMineMine || c2->wall == waMineUnknown)  && markOrb(itOrbGhost);
  
  if(!nomine) {
    uncoverMines(c2,
      items[itBombEgg] < 10 && hiitems[itBombEgg] < 25 ? 0 :
      items[itBombEgg] < 20 ? 1 :
      items[itBombEgg] < 30 ? 2 :
      3
      );
    explodeMine(c2);
    }
  
  if((c2->wall == waClosePlate || c2->wall == waOpenPlate) && !markOrb(itOrbGhost))
    toggleGates(c2, c2->wall, 3);

  princess::playernear(c2);

  if(c2->wall == waGlass && items[itOrbGhost] > 2) {
    addMessage(XLAT("Your Aether powers are drained by %the1!", c2->wall));
    items[itOrbGhost] = 2;
    }
    
  if(c2->wall == waMirror && !markOrb(itOrbGhost)) {
    invismove = false;
    addMessage(XLAT("The mirror shatters!"));
    if(c2->land == laMirror) gainItem(itShard);
    c2->wall = waNone;
    createMirrors(cwt.c, cwt.spin, moMirage);
    }

  if(c2->wall == waCloud && !markOrb(itOrbGhost)) {
    invismove = false;
    addMessage(XLAT("The cloud turns into a bunch of images!"));
    if(c2->land == laMirror) gainItem(itShard);
    c2->wall = waNone;
    createMirages(cwt.c, cwt.spin, moMirage);
    }
  
  if(cellUnstable(c2) && !markOrb(itOrbGhost))
    c2->wall = waChasm;

  if(c2->wall == waStrandedBoat && markOrb(itOrbWater))
    c2->wall = waBoat;

  if(c2->land == laOcean && c2->wall == waBoat && c2->landparam < 30 && markOrb(itOrbWater))
    c2->landparam = 40;
  }

void moveMonster(cell *ct, cell *cf) {
  eMonster m = cf->monst;
  moveEffect(ct, cf, m);
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
    if(ct->monst != moTentacleGhost)
      ct->mondir = neighborId(ct, cf);
    }
  ct->hitpoints = cf->hitpoints;
  ct->stuntime = cf->stuntime;

  if(isFriendly(m) || isBug(m) || items[itOrbDiscord]) stabbingAttack(cf, ct, m);

  if(isLeader(m)) {
    if(ct->wall == waBigStatue) {
      ct->wall = cf->wall;
      cf->wall = waBigStatue;
      }

    moveBoatIfUsingOne(ct, cf);
    }
  
  if(m == moEarthElemental)
    earthMove(cf, neighborId(cf, ct));

  if(m == moWaterElemental) {
    placeWater(ct, cf);
    for(int i=0; i<ct->type; i++) {
      cell *c2 = ct->mov[i];
      if(!c2) continue;
      if(c2->wall == waBoat && !(c2 == cwt.c && markOrb(itOrbWater))) {
        addMessage(XLAT("%The1 is washed away!", c2->wall, moWaterElemental));
        placeWater(c2, ct);
        }
      else if(c2->wall == waStrandedBoat) {
        addMessage(XLAT("%The1 is washed away!", c2->wall, moWaterElemental));
        c2->wall = waNone;
        }
      else if(c2->wall == waDeadTroll) {
        addMessage(XLAT("%The1 is washed away!", c2->wall, moWaterElemental));
        c2->wall = waNone;
        }
      else if(isFire(c2) && c2->wall != waEternalFire) {
        addMessage(XLAT("%The1 is extinguished!", c2->wall, moWaterElemental));
        c2->wall = waNone;
        }
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
      if(c3->monst && isKillableSomehow(c3)) {
        addMessage(XLAT("%The1 pierces %the2!", m, c3->monst));
        killWithMessage(c3, true);
        }
      killThePlayerAt(m, c3);
      }
    }
  
  if(m == moWitchFire) makeflame(cf, 10, false);
  if(m == moFireElemental) makeflame(cf, 20, false);
  }

void moveNormal(cell *c) {
  bool repeat = true;
  eMonster m = c->monst;
  
  if(c->stuntime) return;

  for(int j=0; j<c->type; j++) 
    if(c->mov[j] && isFriendlyOrBug(c->mov[j]) && repeat && !attackingForbidden(c->mov[j], c)) {
      // XLATC enemy destroys a friend
      if(attackJustStuns(c->mov[j]))
        addMessage(XLAT("%The1 attacks %the2!", m, c->mov[j]->monst));
      else messageKill(m, c->mov[j]->monst);
      killWithMessage(c->mov[j], true);
      return;
      }

  int posdir[10], nc = 0;
  
  for(int j=0; j<c->type; j++) {
    cell *c2 = c->mov[j];
    if(!c2) continue;
    if(c2->pathdist >= c->pathdist) continue;
    if(!passable(c2, c, false, false, false)) continue;
    // crystal sages can't stand out of Cocytus
    if(m == moCrystalSage && (c2->land != laCocytus || HEAT(c2) > SAGEMELT || cwt.c->wall == waBoat))
      continue;
    // lancers don't move next to other monsters
    if(c->monst == moLancer) { 
      bool lancerok = true;
      for(int u=2; u<=c2->type-2; u++) {
        cell *c3 = c2->mov[(c->spn[j]+u)%c2->type]; 
        if(c3->monst && !isFriendlyOrBug(c3) && isKillableSomehow(c3))
          lancerok = false;
        }
      if(!lancerok) continue;
      }


//    if(m == moNecromancer ? c2->land == laGraveyard : true)
//    it is more fun when demons step into the Land of Eternal Motion, IMO
//    if((m == moLesser || m == moGreater) ? c2->land != laMotion : true)
    if(c2->cpdist > 0) {
      posdir[nc] = j;
      nc++;
      }
    }

  if(nc == 0 && !passable(c, NULL, true, true, false)) {
    // running dogs run away!
    // they prefer a straight direction
    int sgn = hrand(2) ? 1 : -1;
    for(int b=3; b>=0; b--) for(int s=-1; s<=1; s+=2) if(!nc) {
      int d = (c->mondir + b*s*sgn) % c->type;
      d += c->type; d %= c->type;
      cell *c2 = c->mov[d];
      if(passable(c2, c, false, false, false))
        posdir[nc++] = d;
      }
    }
      
  if(!nc) return;  
  nc = hrand(nc);
  cell *c2 = c->mov[posdir[nc]];  
  moveMonster(c2, c);
  
  if(isWitch(m) && c2->item == itOrbLife && passable(c, NULL, true, true, false)) {
    // note that Fire Witches don't pick up Orbs of Life,
    addMessage(XLAT("%The1 picks up %the2!", moWitch, c2->item));
    c->monst = moEvilGolem; c2->item = itNone;
    }
  else if(m == moWitch) {
    bool pickup = false;
    if(c2->item == itOrbFlash)
      pickup = true, m = moWitchFlash;
    if(c2->item == itOrbWinter)
      pickup = true, m = moWitchWinter;
    if(c2->item == itOrbGhost)
      pickup = true, m = moWitchGhost;
    if(c2->item == itOrbFire)
      pickup = true, m = moWitchFire;
    // Orbs of Speed are a special case here, because we don't want
    // the witch to move immediately
    if(c2->item == itOrbLife)
      pickup = true, c->monst = moEvilGolem;
    if(pickup) {
      addMessage(XLAT("%The1 picks up %the2!", moWitch, c2->item));
      c2->monst = m; c2->item = itNone;
      }
    }
  }

void explodeAround(cell *c) {
  for(int j=0; j<c->type; j++) {
    cell* c2 = c->mov[j];
    if(c2) {
      if(isIcyLand(c2)) HEAT(c2) += 0.5;
      if((c2->wall == waDune || c2->wall == waIcewall ||
        c2->wall == waAncientGrave || c2->wall == waFreshGrave || 
        c2->wall == waColumn || c2->wall == waThumperOff || c2->wall == waThumperOn ||
        (isFire(c2) && !eternalFire(c2)) ||
        c2->wall == waDryTree || c2->wall == waWetTree ||
        c2->wall == waVinePlant || c2->wall == waVineHalfA || c2->wall == waVineHalfB)) {
        destroyHalfvine(c2); 
        c2->wall = waNone;
        }
      if(c2->wall == waCavewall || c2->wall == waDeadTroll) c2->wall = waCavefloor;
      if(c2->wall == waDeadfloor2) c2->wall = waDeadfloor;
      if(c2->wall == waGargoyleFloor) c2->wall = waChasm;
      if(c2->wall == waGargoyleBridge) placeWater(c2, c2);
      if(c2->wall == waRubble) c2->wall = waNone;
      if(c2->wall == waPlatform) c2->wall = waNone;
      if(c2->wall == waStone) c2->wall = waNone;
      if(c2->wall == waLadder) c2->wall = waNone;
      if(c2->wall == waGargoyle) c2->wall = waNone;
      if(c2->wall == waDeadwall) c2->wall = waDeadfloor2;
      if(c2->wall == waBigStatue) c2->wall = waNone;
      if(c2->wall == waPalace || c2->wall == waOpenGate || c2->wall == waClosedGate) 
        c2->wall = waNone;
      if(c2->wall == waFloorA || c2->wall == waFloorB)
      if(c->wall == waFloorA || c->wall == waFloorB)
        c2->wall = c->wall;
      }
    }
  }

void killThePlayer(eMonster m, int id) {
  if(markOrb(itOrbShield)) return;
  if(shmup::on) {
    shmup::cpid = id;
    shmup::killThePlayer(m);
    }
  else if(hardcore) {
    addMessage(XLAT("You are killed by %the1!", m));
    canmove = false;
    }
  else
    addMessage(XLAT("%The1 is confused!", m));
  }

void killThePlayerAt(eMonster m, cell *c) {
  for(int i=0; i<numplayers(); i++) 
    if(playerpos(i) == c) 
      killThePlayer(m, i);
  }

void moveWorm(cell *c) {

  if(c->monst == moWormwait) { c->monst = moWorm; return; }
  else if(c->monst == moTentaclewait) { c->monst = moTentacle; return; }
  else if(c->monst == moTentacleEscaping) {    
    // explodeAround(c);
    c->monst = moNone;
    if(c->mondir != NODIR) c->mov[c->mondir]->monst = moTentacleEscaping;
    return;
    }
  else if(c->monst != moWorm && c->monst != moTentacle) return;

  int ncg = 0, ncb = 0;
  cell *gmov[7], *bmov[7];
  
  int id = c->monst - moWorm;
  
  for(int j=0; j<c->type; j++) {
    if(c->mov[j] && isFriendlyOrBugS(c->mov[j]) && !attackingForbidden(c->mov[j], c)) {
      addMessage(XLAT("%The1 eats %the2!", c->monst, c->mov[j]->monst));
      killWithMessage(c->mov[j], false);
      ncg = 1; gmov[0] = c->mov[j];
      break;
      }
    if(isPlayerOn(c->mov[j]) && !attackingForbidden(c->mov[j], c)) killThePlayerAt(c->monst, c->mov[j]);
    if(c->mov[j] && passable(c->mov[j], c, false, false, false) && !cellUnstable(c->mov[j]) && (id || !cellEdgeUnstable(c->mov[j])) && c->mov[j] != cwt.c) {
      if(c->mov[j]->pathdist < c->pathdist) gmov[ncg++] = c->mov[j]; else bmov[ncb++] = c->mov[j];
      }
    }

  if(ncg == 0 && ncb == 0) {
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
      }
    return;
    }
  
  cell* goal;
  if(ncg) goal = gmov[hrand(ncg)];
  else goal = bmov[hrand(ncb)];
  
  for(int j=0; j<c->type; j++) if(c->mov[j] == goal) {
    goal->monst = eMonster(moWormwait + id);
    moveEffect(goal, NULL, eMonster(moWormwait + id));
      
    c->monst = eMonster(moWormtail + id);
    goal->mondir = c->spn[j];
    
    if(id) break;
    
    cell *c2 = c, *c3 = c2;
    for(int a=0; a<15; a++)
      if(c2->monst == moWormtail) {
        if(c2->mondir == NODIR) return;
        c3 = c2, c2 = c3->mov[c2->mondir];
        }
    
    if(c2->monst == moWormtail) c2->monst = moNone, c3->mondir = NODIR;
    }

  }

void ivynext(cell *c) {
  cellwalker cw(c, c->mondir);
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
  c->monst = moNone;
  for(int i=0; i<c->type; i++)
  // note that semi-vines don't count
    if(c->mov[i]->wall == waVinePlant) {
      destroyHalfvine(c);
      c->wall = waVinePlant;
      }
  }

void moveivy() {
  for(int i=0; i<size(ivies); i++) {
    cell *c = ivies[i];
    cell *co = c;
    if(c->monst != moIvyHead) continue;
    ivynext(c);

    cell *mto = NULL;
    int pd = c->pathdist;
    int sp = 0;
    
    while(c->monst != moIvyRoot) {
      for(int j=0; j<c->type; j++) {
        if(c->mov[j] && isFriendly(c->mov[j]) && !attackingForbidden(c->mov[j], c)) {
          if(isNonliving(c->mov[j]->monst))
            addMessage(XLAT("The ivy destroys %the1!", c->mov[j]->monst));
          else
            addMessage(XLAT("The ivy kills %the1!", c->mov[j]->monst));
          killMonster(c->mov[j]);
          continue;
          }
        if(isPlayerOn(c->mov[j]) && !attackingForbidden(c->mov[j], c))
          killThePlayerAt(c->monst, c->mov[j]);
        if(c->mov[j] && signed(c->mov[j]->pathdist) < pd && passable(c->mov[j], c, false, false, false))
          mto = c->mov[j], pd = mto->pathdist, sp = c->spn[j];
        }
      c = c->mov[c->mondir];
      }

    if(mto && mto->cpdist) {        
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
  int d = from->spn[dir];
  b |= earthWall(from);
  if(c2) for(int u=2; u<=c2->type-2; u++) {
    cell *c3 = c2->mov[(d + u)% c2->type];
    b |= earthFloor(c3);
    }
  return b;
  }

int sval; vector<cell*> gendfs;

void monsterfight(cell *attacker, cell *victim) {
  if(isBird(attacker->monst))
    addMessage(XLAT("%The1 claws %the2!", attacker->monst, victim->monst));
  else if(isSlimeMover(attacker)) {
    if(attackJustStuns(victim))
      addMessage(XLAT("%The1 attacks %the2!", attacker->monst, victim->monst));
    else
      addMessage(XLAT("%The1 eats %the2!", attacker->monst, victim->monst));
    }
  else
    addMessage(XLAT("%The1 punches %the2!", attacker->monst, victim->monst));
  killWithMessage(victim, true);
  }

void groupmove2(cell *c, cell *from, int d, eMonster movtype, bool noattacks) {
  if(!c) return;
  if(eq(c->aitmp, sval)) return;
  if(!passable_for(movtype, from, c, false)) return;

  if(movegroup(c->monst) == movtype) {
    if(movtype == moEagle && noattacks && c->monst != moEagle) return;

    if(c->stuntime) return;
    
    // note: move from 'c' to 'from'!
    if(!noattacks) for(int j=0; j<c->type; j++) 
      if(c->mov[j] && isFriendlyOrBug(c->mov[j]) && !attackingForbidden(c->mov[j], c)) {
        monsterfight(c, c->mov[j]);
        c->aitmp = sval;
        // XLATC eagle
        return;
        }
    
    if(movtype == moEarthElemental && !passable(from, c, false, false, false)) {
      earthFloor(from);
      return;
      }

    if(from->cpdist == 0 || from->monst) { c->aitmp = sval; return; }
    moveMonster(from, c);
    }
  c->aitmp = sval;
  if(size(gendfs) < 1000) gendfs.push_back(c);
  }

#define ONLY_ONE_PLAYER_POSSIBLE 0

void groupmove(eMonster movtype, bool noattacks = false) {
  if(!noattacks) for(int i=0; i<cwt.c->type; i++) {
    eMonster m = cwt.c->mov[i]->monst;
    if(movegroup(m) == movtype && !attackingForbidden(cwt.c, cwt.c->mov[i]))
      killThePlayer(m, ONLY_ONE_PLAYER_POSSIBLE);
    }
  sval++;
  gendfs.clear();
  for(int i=0; i<size(targets); i++)
    gendfs.push_back(targets[i]);

  if(invisfish && movtype == moSlime) for(int i=0; i<numplayers(); i++) {
    cell *c = playerpos(i);
    gendfs.push_back(c);
    }
  
  for(int i=0; i<size(gendfs); i++) {
    cell *c = gendfs[i];
    for(int t=0; t<c->type; t++)
      groupmove2(c->mov[t], c, t, movtype, noattacks);
    }
  }

// Hex monsters

vector<cell*> hexdfs;

// note: move from 'c' to 'from'!
void moveHexSnake(cell *from, cell *c, int d) {
  if(from->wall == waBoat) from->wall = waSea;
  moveEffect(from, c, c->monst);
  from->monst = c->monst; from->mondir = d;
  c->monst = moHexSnakeTail;
  
  cell *c2 = c, *c3=c2;
  for(int a=0;; a++) if(c2->monst == moHexSnakeTail) {
    if(a == ROCKSNAKELENGTH) { c2->monst = moNone, c3->mondir = NODIR; break; }
    if(c2->mondir == NODIR) break;
    c3 = c2, c2 = c3->mov[c2->mondir];
    }
    else break;
  }

void snakeAttack(cell *c) {
  for(int j=0; j<c->type; j++) 
    if(c->mov[j] && isFriendlyOrBug(c->mov[j]) && !attackingForbidden(c->mov[j], c) 
        /* &&
        (passable(c->mov[j], c, true, false, false) && !ishept(c->mov[j])) */
        ) {
        
        if(attackJustStuns(c->mov[j]))
          addMessage(XLAT("%The1 attacks %the2!", c->monst, c->mov[j]->monst));
        else
          addMessage(XLAT("%The1 kills %the2!", c->monst, c->mov[j]->monst));
        killWithMessage(c->mov[j], true);
        }
  }

// note: move from 'c' to 'from'!
void hexvisit(cell *c, cell *from, int d) {
  if(!c) return;
  if(cellUnstable(c) || cellEdgeUnstable(c)) return;
  if(eq(c->aitmp, sval)) return;
  
  if(cellUnstableOrChasm(c) || cellUnstableOrChasm(from)) return;
  
  /* if(c->monst == moHexSnake)
    printf("%p:%p %s %d\n", from, c, dnameof(from->monst), passable(from, c, true, false, false)); */

  if(from->cpdist && (!isWatery(from) && !passable(from, c, true, false, false))) return;
  
  if(c->monst == moHexSnake) {
    // printf("got snake\n");
    
    if(ishept(from)) return;

    if(isFriendlyOrBug(from) && !attackingForbidden(from, c)) {
      addMessage(XLAT("%The1 eats %the2!", c->monst, from->monst));
      killWithMessage(from, false);
      }
    
    if(from->cpdist == 0 || from->monst) return;

    snakeAttack(c);
    if(c->monst == moRedTroll) 
      moveMonster(from, c);
    else moveHexSnake(from, c, d);
    }

  c->aitmp = sval;
  if(c->bardir == 0) c->bardir = NOBARRIERS;

  if(size(hexdfs) < 2000) hexdfs.push_back(c);
  }

void movehex() {
  for(int p=0; p<numplayers(); p++) {
    shmup::cpid = p;
    cell *c = playerpos(p);
    for(int i=0; i<c->type; i++) 
      if(c->mov[i]->monst == moHexSnake && !attackingForbidden(cwt.c, cwt.c->mov[i]))
        killThePlayer(moHexSnake, p);
    }
  sval++;
  hexdfs.clear();
  for(int i=0; i<size(targets); i++) {
    hexdfs.push_back(targets[i]);
    targets[i]->aitmp = sval;
    }
  //hexdfs.push_back(cwt.c);

  for(int i=0; i<size(hexdfs); i++) {
    cell *c = hexdfs[i];
    for(int t=0; t<c->type; t++) if(c->mov[t] && !ishept(c->mov[t]))
      hexvisit(c->mov[t], c, t);
    }
  for(int i=0; i<size(hexsnakes); i++) {
    cell *c = hexsnakes[i];
    if(c->monst == moHexSnake) {
      int t[2];
      t[0] = (c->mondir+2) % 6;
      t[1] = (c->mondir+4) % 6;
      if(hrand(2) == 0) swap(t[0], t[1]);
      for(int u=0; u<2; u++) {
        createMov(c, t[u]);
        if(!ishept(c->mov[t[u]]))
          hexvisit(c, c->mov[t[u]], c->spn[t[u]]);
        }
      }
    if(c->monst == moHexSnake) {
      snakeAttack(c);
      kills[moHexSnake]++;
      cell *c2 = c;
      while(c2->monst == moHexSnakeTail || c2->monst == moHexSnake) {
        if(c2->monst != moHexSnake && c2->mondir != NODIR)
          snakepile(c2);
        snakepile(c2);
        c2->monst = moNone; 
        if(c2->mondir == NODIR) break;
        c2 = c2->mov[c2->mondir];
        }
      }
    }
  }

// next == +1 -> next
// next == -1 -> prev
cell *getWhirlpool(cell *c, int next) {
  int i = 0;
  if(!euclid && !c->master->alt) return NULL;
  int d = celldistAlt(c);
  int d2;
  while(true) {
    if(i == c->type) return NULL;
    if(c->mov[i] && (d2 = celldistAlt(c->mov[i])) != d)
      break;
    i++;
    }
  if(i == c->type) return NULL;
  if(d>d2) next = -next;
  for(int j=1; j<c->type; j++) {
    cell *c2 = c->mov[(i+42+next*j) % c->type];
    if(celldistAlt(c2) == d) return c2;
    }
  return NULL;
  }

void buildWhirlline(vector<cell*>& whirlline, int d) {
  again: 
  cell *at = whirlline[size(whirlline)-1];
  cell *prev = whirlline[size(whirlline)-2];
  for(int i=0; i<at->type; i++) 
    if(at->mov[i] && (euclid || at->mov[i]->master->alt) && celldistAlt(at->mov[i]) == d && at->mov[i] != prev) {
      whirlline.push_back(at->mov[i]);
      goto again;
      }
  }

void whirlGenerate(cell *wto) {
  if(wto->wall == waBoat || wto->monst)
    return;
  
  if(hrand(35000) < 40 + items[itWhirlpool])
    wto->monst = moCShark;
  else if(hrand(5000) < 500)
    wto->wall = waBoat;
  
  if(wto->wall == waBoat && (euclid || wto->master->alt)) { 
    int d = celldistAlt(wto);
    // 250 : hard
    if(hrand(5000) < 60 + 3 * items[itWhirlpool])
      wto->monst = moPirate;
    if(hrand(5000) < 20 && d < -20)
      wto->item = itOrbSafety;
    if(hrand(5000) < 20*PRIZEMUL && d < -20)
      placePrizeOrb(wto);
    if(items[itWhirlpool] >= 10 && hrand(5000) < 20 && d < -15)
      wto->item = itOrbWater;
    else if(d<-10 && hrand(5000) < 1000-d)
      wto->item = itWhirlpool;
    }
  }

void whirlMove(cell *wto, cell *wfrom) {
  // monsters don't move
  if(wfrom && (wfrom == cwt.c || wfrom->monst))
    return;
  // disappear
  if(!wto) { wfrom->wall = waSea; wfrom->item = itNone; }
  
  if(wfrom && wto && wfrom->wall == waBoat && wto->wall == waSea && !wto->monst) {
    wfrom->wall = waSea; wto->wall = waBoat;
    }
  
  if(wfrom && wto && wfrom->item && !wto->item && wfrom->wall != waBoat) {
    moveItem(wfrom, wto, false);
    }
  
  if(wto && !wfrom) 
    whirlGenerate(wto);
  }

void movewhirlpool() {
  sval++;
  for(int i=0; i<size(dcal); i++) {
    cell *c = dcal[i];
    if(c->land == laWhirlpool && c->aitmp != sval && (euclid || c->master->alt)) {
      cell *c2 = getWhirlpool(c, 1);
      if(!c2) continue;
      int d = celldistAlt(c);
      vector<cell*> whirlline;
      whirlline.push_back(c);
      whirlline.push_back(c2);
      buildWhirlline(whirlline, d);
      reverse(whirlline.begin(), whirlline.end());
      buildWhirlline(whirlline, d);
      int z = size(whirlline);
      
      for(int i=0; i<z; i++) 
        whirlline[i]->aitmp = sval;

      whirlMove(NULL, whirlline[0]);
      
      for(int i=0; i<z-1; i++) 
        whirlMove(whirlline[i], whirlline[i+1]);
      
      whirlMove(whirlline[z-1], NULL);
      }
    }
  }



#define SHSIZE 16

cell *shpos[SHSIZE];
int cshpos = 0;

void clearshadow() {
  for(int i=0; i<SHSIZE; i++) shpos[i] = NULL;
  }

void moveshadow() {
  if(shpos[cshpos] && shpos[cshpos]->monst == moShadow)
    shpos[cshpos]->monst = moNone;
  shpos[cshpos] = cwt.c;
  cshpos = (cshpos+1) % SHSIZE;
  if(shpos[cshpos] && shpos[cshpos]->monst == moNone && shpos[cshpos]->cpdist && shpos[cshpos]->land == laGraveyard)
    shpos[cshpos]->monst = moShadow;
  }

void moveghosts() {

  if(invismove) return;
  for(int d=0; d<8; d++) movesofgood[d].clear();  

  for(int i=0; i<size(ghosts); i++) {
    cell *c = ghosts[i];
    
    if(c->stuntime) return;

    if(isGhostMover(c->monst) && c->cpdist == 1) 
      killThePlayer(c->monst, ONLY_ONE_PLAYER_POSSIBLE);

    if(isGhostMover(c->monst) && c->cpdist > 1) {
      int goodmoves = 0;

      for(int k=0; k<c->type; k++) if(c->mov[k] && c->mov[k]->cpdist < c->cpdist)
        if(ghostmove(c->monst, c->mov[k], c))
          goodmoves++;
      
      movesofgood[goodmoves].push_back(c);
      }
    }
  
  for(int d=0; d<8; d++) for(int i=0; i<size(movesofgood[d]); i++) {
    cell *c = movesofgood[d][i];
    if(c->stuntime) return;
    
    if(isGhostMover(c->monst) && c->cpdist > 1) {
      
      int mdir[7];

      for(int j=0; j<c->type; j++) 
        if(c->mov[j] && isFriendlyOrBug(c->mov[j])) {
          // XLATC ghost/greater shark
          addMessage(XLAT("%The1 scares %the2!", c->monst, c->mov[j]->monst));
          killWithMessage(c->mov[j], true);
          return;
          }
    
      int qmpos = 0;
      for(int k=0; k<c->type; k++) if(c->mov[k] && c->mov[k]->cpdist < c->cpdist)
        if(ghostmove(c->monst, c->mov[k], c))
          mdir[qmpos++] = k;
      if(!qmpos) continue;
      int d = mdir[hrand(qmpos)];
      cell *c2 = c->mov[d];
      moveMonster(c2, c);
      }
    }
  }

int lastdouble = -3;

void messageKill(eMonster killer, eMonster victim) {
  if(isNonliving(victim)) {
    if(killer)
      addMessage(XLAT("%The1 destroys %the2!", killer, victim));
    else
      addMessage(XLAT("You destroy %the1.", victim));
    }
  else {
    if(killer)
      addMessage(XLAT("%The1 kills %the2!", killer, victim));
    else
      addMessage(XLAT("You kill %the1.", victim));
    }
  }

void stabbingAttack(cell *mf, cell *mt, eMonster who) {
  int numsh = 0, numflail = 0, numlance = 0;
  for(int t=0; t<mf->type; t++) {
    cell *c = mf->mov[t];
    if(!isUnarmed(who))
    if(c->monst == moHedge || (!who && c->monst && isKillable(c) && markOrb(itOrbThorns))) {
      for(int u=0; u<c->type; u++) {
        if(c->mov[u] == mt) {
          if(who)
            addMessage(XLAT("%The1 stabs %the2.", who, c->monst));
          else
            addMessage(XLAT("You stab %the1.", c->monst));
          int k = tkills();
          killWithMessage(c, true);
          if(tkills() > k) numsh++;
          }
        }
      }
    if(c->monst == moFlailer || (c->monst == moVizier && !isUnarmed(who))) {
      bool away = true;
      if(c == mt) away = false;
      for(int u=0; u<c->type; u++) if(c->mov[u] == mt) away = false;
      if(away) {
        if(c->monst == moVizier && c->hitpoints > 1) {
          if(who)
            addMessage(XLAT("%The1 hits %the2.", who, c->monst));
          else
            addMessage(XLAT("You hit %the1.", c->monst));
          c->hitpoints--;
          // c->stuntime = 1;
          }
        else {
          if(c->monst != moFlailer) {
            messageKill(who, c->monst);
            }
          else {
            if(who)
              addMessage(XLAT("%The1 tricks %the2.", who, c->monst));
            else
              addMessage(XLAT("You trick %the1.", c->monst));
            }
          if(c->monst == moFlailer && isPrincess(who) && isUnarmed(who))
            achievement_gain("PRINCESS_PACIFIST");

          int k = tkills();
          killWithMessage(c);
          if(tkills() > k) numflail++;
          }
        }
      }
    }

  if(!isUnarmed(who)) for(int t=0; t<mt->type; t++) {
    cell *c = mt->mov[t];
    if(c->monst == moLancer) {
      if(who)
        addMessage(XLAT("%The1 tricks %the2.", who, c->monst));
      else
        addMessage(XLAT("You trick %the1.", c->monst));
      int k = tkills();
      killWithMessage(c);
      if(tkills() > k) numlance++;
      }
    }

  if(numsh) achievement_count("STAB", numsh, 0);
  
  if(numlance && numflail && numsh) achievement_gain("MELEE3");

  if(numlance + numflail + numsh >= 5) achievement_gain("MELEE5");

  if(numsh == 2) {
    if(lastdouble == turncount-1) achievement_count("STAB", 4, 0);
    lastdouble = turncount;
    }
  }

void movegolems() {
  int qg = 0;
  for(int i=0; i<size(golems); i++) {
    cell *c = golems[i];
    eMonster m =  c->monst;
    if(isPrincess(m) && c->stuntime) continue;
    if(m == moGolem || m == moKnight || m == moTameBomberbird || m == moPrincess ||
      m == moPrincessArmed || m == moMouse) {
      if(m == moGolem) qg++;
      int bestv = 100, bq = 0, bdirs[7];
      for(int k=0; k<c->type; k++) if(c->mov[k]) {
        int val;
        cell *c2 = c->mov[k];
        if(c2 == cwt.c) val = 0;
        else if(m == moPrincess && isStunnable(c2->monst) && c2->stuntime && c2->monst != moSkeleton &&
          !cellUnstableOrChasm(c) && !attackingForbidden(c2, c) && !isUnarmed(c2->monst)) {
          val = 15000;
          }
        else if(m == moPrincessArmed && isPrincess(c2->monst) && !attackingForbidden(c2, c))
          val = 14000; // jealousy!
        else if(isActiveEnemy(c2, NULL, false) && isKillable(c2) &&
          !attackingForbidden(c2, c) && !isUnarmed(m))
          val = 12000;
        else if(isInactiveEnemy(c2, false) && isKillable(c2) && 
          !attackingForbidden(c2, c) && !isUnarmed(m) && c2->monst != moSkeleton)
          val = 10000;
        else if(isIvy(c2) && !attackingForbidden(c2, c) && !isUnarmed(m)) val = 8000;
        else if(monstersnear(c2, NULL, m)) val = 50; // linked with mouse suicide!
        else if(passable_for(m, c2, c, false)) val = 4000;
        else val = 0;
        if(c->monst == moGolem )
          val -= c2->cpdist;
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
        if(!euclid && c->monst == moKnight && c2->master->alt)
          val -= celldistAlt(c2);
        if(val > bestv) bestv = val, bq = 0;
        if(val == bestv) bdirs[bq++] = k;
        }
      if(bestv <= 100) continue;
      int dir = bdirs[hrand(bq)];
      cell *c2 = c->mov[dir];
      if(c2->monst) {
        if(m == moPrincess) {
          addMessage(XLAT("%The1 takes %his1 revenge on %the2!", m, c2->monst));
          killWithMessage(c2, false);
          c->monst = m = moPrincessArmed;
          }
        else {
          if(attackJustStuns(c2))
            addMessage(XLAT("%The1 attacks %the2!", m, c2->monst));
          else {
            messageKill(c->monst, c2->monst);
            if(isPrincess(c->monst) && isPrincess(c2->monst)) 
              addMessage("\"That should teach you to take me seriously!\"");
            }
          killWithMessage(c2, true);
          }
        }
      else {
        moveMonster(c2, c);
        if(m != moTameBomberbird) 
          moveBoatIfUsingOne(c2, c);
          
        if(m == moGolem) c2->monst = moGolemMoved;
        if(m == moMouse) c2->monst = moMouseMoved;
        if(m == moPrincess) c2->monst = moPrincessMoved;
        if(m == moPrincessArmed) c2->monst = moPrincessArmedMoved;
        if(m == moTameBomberbird) c2->monst = moTameBomberbirdMoved;
        if(m == moKnight) c2->monst = moKnightMoved;
        }
      }
    }
  achievement_count("GOLEM", qg, 0);
  }

bool wchance(int a, int of) {
  of *= 10; 
  a += items[itOrbYendor] * 5 + items[itHolyGrail] + 1;
  if(isCrossroads(cwt.c->land)) 
    a+= items[itHyperstone] * 10;
  for(int i=0; i<ittypes; i++) if(itemclass(eItem(i)) == IC_TREASURE)
    a = max(a, (items[i]-10) / 10);
  return hrand(a+of) < a;
  }

void wanderingZebra(cell *start) {
  cell *c = start, *c2 = start;
  for(int it=0; it<100; it++) {
    if(c->cpdist == 7) {
      c->monst = moOrangeDog;
      return;
      }
    int q = 0;
    cell *ctab[8];
    for(int i=0; i<c->type; i++) {
      cell *c3 = c->mov[i];
      if(c3 && c3 != c2 && c3->land == laZebra && c3->wall == waNone)
        ctab[q++] = c3;
      }
    if(!q) break;
    c2 = c; c = ctab[hrand(q)];
    }
  }

void wandering() {
  int t = shmup::on ? (shmup::curtime - lastexplore) / 350 : turncount - lastexplore;
  int seepcount = 0;
  if(t > 40) seepcount = (t-40 + hrand(20)) / 20;
  int ghostcount = 0;
  if(t > 80) ghostcount = (t-80 + hrand(20)) / 20;

  if(cwt.c->land == laZebra && cwt.c->wall == waNone && wchance(items[itZebra], 20))
    wanderingZebra(cwt.c);
  
  while(first7 < size(dcal)) {
    int i = first7 + hrand(size(dcal) - first7);
    cell *c = dcal[i];
    
    // wandering seeps & ghosts
    if(seepcount && c->wall == waCavewall && !c->monst && eq(c->aitmp, sval)) {
      c->monst = moSeep;
      seepcount--;
      continue;
      }
    
    if(ghostcount && !c->monst && cwt.c->type != laCaves) {
      c->monst = moGhost;
      ghostcount--;
      continue;
      }
    
    if((c->wall == waCavewall || c->wall == waDeadwall) && !c->monst && eq(c->aitmp, sval) &&
      wchance(items[treasureType(c->land)], 10)) {
      c->monst = moSeep;
      continue;
      }
    
    else if(c->wall == waCTree && !c->monst && eq(c->aitmp, sval) && wchance(items[itPirate], 15)) {
      c->monst = moParrot;
      continue;
      }
    
    else if(c->wall == waSea && !c->monst && eq(c->aitmp, sval)) {
      if(c->land == laCaribbean && wchance(items[itPirate], 15)) {
        c->monst = moCShark;
        continue;
        }
      if(c->land == laOcean && cwt.c->landparam < 25 && c->landparam < 25 && wchance(items[itCoast], 25)) {
        c->monst = moAlbatross;
        continue;
        }
      if(c->land == laLivefjord && wchance(items[itFjord], 80) && items[itFjord] >= 10) {
        c->monst = moWaterElemental;
        continue;
        }
      break;
      }
    
    else if(c->monst || c->pathdist == INFD) break;

    else if(hrand(50) < statuecount * statuecount)
      c->monst = moCultistLeader;

    else if(c->land == laIce && wchance(items[itDiamond], 10))
      c->monst = hrand(2) ? moWolf : moYeti;

    else if(c->land == laDesert && wchance(items[itSpice], 10))
      c->monst = hrand(10) ? moDesertman : moWorm;

    else if(c->land == laRedRock && wchance(items[itRedGem], 10))
      c->monst = hrand(10) ? moRedTroll : moHexSnake;

    else if(c->land == laCaves && wchance(items[itGold], 5))
      c->monst = hrand(3) ? moTroll : moGoblin;

    else if(c->land == laHive && wchance(hivehard(), 25))
      c->monst = randomHyperbug();

    else if(c->land == laDeadCaves && wchance(items[itSilver], 5))
      c->monst = hrand(20) ? (hrand(3) ? moDarkTroll : moGoblin) : moEarthElemental;

    else if(c->land == laJungle && wchance(items[itRuby], 40))
      c->monst = hrand(10) ? moMonkey : moEagle;

    else if(c->land == laMirror && wchance(items[itShard], 15))
      c->monst = hrand(10) ? moRanger : moEagle;

    else if(c->land == laHell && wchance(items[itHell], 20))
      c->monst = hrand(3) ? moLesser : moGreater;

    else if(c->land == laCaribbean && wchance(items[itPirate], 30))
      c->monst = moPirate;

    else if(c->land == laRlyeh && wchance(items[itStatue], 15))
      c->monst = hrand(3) ? moPyroCultist : 
        (hrand(40) < items[itStatue]-25) ? moCultistLeader : moCultist;

    else if(c->land == laGraveyard && wchance(items[itBone], 15))
      c->monst = hrand(5) ? moGhost : moNecromancer;
      
    else if(c->land == laDryForest && wchance(items[itFernFlower], 5))
      c->monst = hrand(5) ? moHedge : moFireFairy;
      
    else if(c->land == laCocytus && wchance(items[itSapphire], 45))
      c->monst = moCrystalSage;
      
    else if(c->land == laAlchemist && wchance(items[itElixir], 3) && eq(c->aitmp, sval) && c->item == itNone)
      c->monst = moSlime;
    
    else if(isElemental(c->land) && wchance(items[itElemental], 20))
      c->monst = elementalOf(c->land);
    
    else if(c->land == laEdge && wchance(items[itEdge], 20))
      c->monst = moGargoyle;
    
    else if(c->land == laMinefield && wchance(items[itBombEgg]-20, 400))
      c->monst = moBomberbird;
    
    else if(c->land == laEmerald && wchance(items[itEmerald], 5)) {
      static eMonster m[4] = {moHedge, moLancer, moMiner, moFlailer};      
      c->monst = m[hrand(4)];
      }
    
    else if(c->land == laWineyard && wchance(items[itWine], 10)) {
      c->monst = moVineBeast;
      }
    
    else if(c->land == laPalace && wchance(items[itPalace], 50)) {
      if(princess::dist(c) < OUT_OF_PRISON && !princess::challenge) break;
      
      if(items[itPalace] >= 15 && hrand(100) < 10)
        c->monst = moVizier;
      else if(items[itPalace] >= 5 && hrand(100) < 50)
        c->monst = hrand(2) ? moFatGuard : moSkeleton;
      else c->monst = moPalace;
      c->hitpoints = palaceHP();
      }
    
    else if(c->land == laLivefjord && wchance(items[itFjord], 10)) {
      c->monst = moViking;
      }
    
    else if(c->land == laOcean && wchance(items[itCoast], 100)) {
      c->monst = moAlbatross;
      }
    
    else if(c->land == laPower && wchance(items[itPower], 10)) {
      c->monst = eMonster(moWitch + hrand(NUMWITCH));
      }
    
    else if(c->land == laCamelot && hrand(30) == 0 && (euclid || c->master->alt) && celldistAltRelative(c) < 0) {
      eMonster m[3] = { moHedge, moLancer, moFlailer };
      c->monst = m[hrand(3)];
      }
    
    else if(isCrossroads(c->land) && items[itHyperstone] && wchance(items[itHyperstone], 20)) {
      c->monst = wanderingCrossroadsMonster();
      }
    
    else break;
      
    if(c->monst == moWorm || c->monst == moHexSnake) c->mondir = NODIR;
      
    // laMotion -> no respawn!
    }
  }

void sageheat(cell *c, double v) {
  HEAT(c) += v;
  if(c->wall == waFrozenLake && HEAT(c) > .6) c->wall = waLake;
  }

struct buginfo_t {
  cell *where;
  short dist[BUGCOLORS];
  };

vector<buginfo_t> buginfo;

vector<int> bugqueue[BUGCOLORS];
vector<int> bugqueue4[BUGCOLORS];

struct bugtomove_t {
  int dist, moves, index;
  bugtomove_t(int d, int m, int i) { dist=d; moves=m; index=i; }
  };

bool operator < (const bugtomove_t& m1, const bugtomove_t& m2) {
  if(m1.dist != m2.dist) return m1.dist < m2.dist;
  if(m1.moves != m2.moves) return m1.moves < m2.moves;
  return false;
  }

vector<bugtomove_t> bugtomove;
vector<cell*> deadbug;
vector<cell*> bugcellq;

int bugcount[BUGCOLORS];

bool isBugEnemy(cell *c, int k) {
  if(c == cwt.c && !invismove) return true;
  if(!c->monst) return false;
  if(c->monst == moBug0+k) return false;
  if(isIvy(c)) return false;
  return (isBug(c) || isKillableSomehow(c));
  }

// list bugs and targets for each color
#define BUGINF 29999

void bugQueueInsert(int k, int i, int d) {
  if(buginfo[i].dist[k] > d) {
    if(buginfo[i].dist[k] != BUGINF) {
      printf("%d -> %d\n", buginfo[i].dist[k], d);
      }
    buginfo[i].dist[k] = d;
    bugqueue[k].push_back(i);
    }
  }

void bugcell(cell *c) {
  short& i(c->aitmp);
  if(i >= 0 && i < size(buginfo) && buginfo[i].where == c)
    return;
  i = size(buginfo);
  buginfo.resize(i+1);
  buginfo_t& b(buginfo[i]);
  b.where = c;
  for(int k=0; k<BUGCOLORS; k++) {
    b.dist[k] = BUGINF;
    bool havebug = false, haveother = false;
    for(int dir=0; dir<c->type; dir++) {
      cell *c2 = c->mov[dir];
      if(c2 && isBugEnemy(c2,k) && !attackingForbidden(c2, c)) {
        if(isBug(c2)) havebug = true;
        else haveother = true;
        }
      }
    if(havebug) bugQueueInsert(k, i, 0);
    else if(haveother) bugqueue4[k].push_back(i);
    }
/*// bugs communicate if the distance is at most 2
  // also all nearby cells are inserted to the buginfo structure
  if(size(buginfo) < 30000) {
    for(int dir=0; dir<c->type; dir++) {
      cell *c2 = c->mov[dir];
      if(c2) {
        // if(isBug(c)) bugcellq.push_back(c2); => does not help...
        for(int t=0; t<c2->type; t++)
          if(c2->mov[t] && isBug(c2->mov[t]))
            bugcellq.push_back(c2), 
            bugcellq.push_back(c2->mov[t]);
        }
      }
    }*/
  
  // use pheromones!
  if(c->land == laHive && c->landparam > 1) {
    c->landparam --;
    for(int dir=0; dir<c->type; dir++) {
      cell *c2 = c->mov[dir];
      if(c2) {
        for(int t=0; t<c2->type; t++)
          if(c2->mov[t])
            bugcellq.push_back(c2), 
            bugcellq.push_back(c2->mov[t]);
        }
      }
    }
  }

int last_d = -1;

void handleBugQueue(int k, int t) {
  int i = bugqueue[k][t];
  buginfo_t& b(buginfo[i]);
  cell *c = b.where;
  int d = b.dist[k];
  last_d = d;
  int goodmoves = 0;
  for(int dir=0; dir<c->type; dir++) {
    cell *c2 = c->mov[dir];
    if(!c2) continue;
    if(c2->aitmp < 0 || c2->aitmp >= size(buginfo)) continue;
    if(!passable(c, c2, true, false, false)) continue;
    int j = c2->aitmp;
    if(buginfo[j].where != c2) continue;
    if(buginfo[j].dist[k] < d) goodmoves++;
    bugQueueInsert(k, j, d+1);
    }
  if(isBug(c) && c->monst == moBug0+k) {
    bugcount[c->monst - moBug0]++;
    bugtomove.push_back(bugtomove_t(d,goodmoves,i));
    }
  }

#include <set>

void movebugs() {
  buginfo.clear();
  for(int k=0; k<BUGCOLORS; k++) bugqueue[k].clear();
  for(int k=0; k<BUGCOLORS; k++) bugqueue4[k].clear();
  for(int k=0; k<BUGCOLORS; k++) bugcount[k] = 0;
  bugtomove.clear();
  deadbug.clear();
  
  int xdcs = size(dcal); for(int i=0; i<xdcs; i++) bugcell(dcal[i]);
  for(int i=0; i<size(bugcellq); i++) bugcell(bugcellq[i]);
  bugcellq.clear();
  
  // printf("buginfo = %d\n", size(buginfo));
  
  for(int k=0; k<BUGCOLORS; k++) {
    int t = 0;
    last_d = -1;
    int invadist = 4 - (items[itRoyalJelly]+10) / 20;
    if(invadist<0) invadist = 0;
    for(; t<size(bugqueue[k]) && last_d < invadist-1; t++) handleBugQueue(k, t);
    for(int u=0; u<size(bugqueue4[k]); u++)
      bugQueueInsert(k, bugqueue4[k][u], invadist);
    bugqueue4[k].clear();
    for(; t<size(bugqueue[k]); t++) handleBugQueue(k, t);
    }
  
  for(int k=0; k<BUGCOLORS; k++) {
    set<int> check;
    for(int t=0; t<size(bugqueue[k]); t++) {
      if(check.count(bugqueue[k][t])) {
        printf("REPETITION! [%d]\n", t);
        }
      check.insert(bugqueue[k][t]);
      }
    }
  
  random_shuffle(bugtomove.begin(), bugtomove.end());
  sort(bugtomove.begin(), bugtomove.end());
  
  int battlecount = 0;
  for(int t=0; t<size(bugtomove); t++) {
    bugtomove_t& bm(bugtomove[t]);
    int i = bm.index;
      
    buginfo_t& b(buginfo[i]);
    cell *c = b.where;
    if(!isBug(c)) continue;
    if(c->stuntime) continue;
    eMonster m = c->monst;
    int k = (m - moBug0) % BUGCOLORS;
    int gmoves[8], q=0, bqual = -1;

    for(int dir=0; dir<c->type; dir++) {
      cell *c2 = c->mov[dir];
      int qual = -10;
      if(!c2) continue;
      else if(isBugEnemy(c2, k) && c2->monst != moDeadBug)
        qual = c2 != cwt.c ? 2 : -5;
      else {
        if(c2->monst) continue;
        if(!passable(c2, c, false, false, false)) continue;
        if(c2->aitmp < 0 || c2->aitmp >= size(buginfo)) continue;
        if(buginfo[c2->aitmp].where != c2) continue;
        if(buginfo[c2->aitmp].dist[k] < b.dist[k])
          qual = 1;
        else if(buginfo[c2->aitmp].dist[k] == b.dist[k])
          qual = 0;
        }
      // printf("%d->#%d %d: %d\n", i, dir, c2->tmp, qual);
      if(qual > bqual) bqual = qual, q=0;
      if(qual == bqual) gmoves[q++] = dir;
      }
    
    if(!q) { if(c->land == laHive) c->landparam += 3; continue; }
    int d = gmoves[hrand(q)];
    cell *c2 = c->mov[d];
    if(c2->monst) {
      eMonster killed = c2->monst;
      if(isBug(killed)) battlecount++;
      else addMessage(XLAT("%The1 fights with %the2!", c->monst, c2->monst));
      killOrStunMonster(c2);
      // killMonster(c);
      if(isBug(killed)) {
        c2->monst = moDeadBug, deadbug.push_back(c2);
        bugcount[killed - moBug0]--;
        }
//    c->monst = moDeadBug, deadbug.push_back(c);
      }
    else {
      moveMonster(c2, c);
      // pheromones!
      if(c->land == laHive && c->landparam < 90) c->landparam += 5;
      if(c2->land == laHive && c2->landparam < 90) c2->landparam += 5;
      // if(isHive(c2->land)) c2->land = eLand(laHive0+k);
/*      if(c2->item == itRoyalJelly && !isQueen(m)) {
        // advance!
        c2->monst = eMonster(m+BUGCOLORS);
        c2->item = itNone;
        } */
      }
    }
  
  // cleanup
  for(int i=0; i<size(deadbug); i++) deadbug[i]->monst = moNone;
  if(battlecount)
    addMessage(XLAT("The Hyperbugs are fighting!"));
    
  int maxbug = 0;
  for(int k=0; k<BUGCOLORS; k++) if(bugcount[k] > maxbug) maxbug = bugcount[k];
  
  achievement_count("BUG", maxbug, 0);
  }

void bugcitycell(cell *c, int d) {
  short& i = c->aitmp;
  if(i >= 0 && i < size(buginfo) && buginfo[i].where == c)
    return;
  i = size(buginfo);
  buginfo_t b;
  b.where = c;
  b.dist[0] = d;
  buginfo.push_back(b);
  }

void createBugArmy(cell *c) {
  int k = randomHyperbug() - moBug0;
  int minbugs = 50, maxbugs = 50;
  int var = 5 + items[itRoyalJelly];
  if(var>25) var=25;
  // minbugs += 100; maxbugs += 100;
  minbugs -= var; maxbugs += var;
  maxbugs += items[itRoyalJelly];
  int numbugs = minbugs + hrand(maxbugs - minbugs + 1);
  
  /* int i = items[itRoyalJelly];
  int chance = 20 + 25 * i + 9000;
  // i=0: 16%
  // i=10: 73%
  // i=50: 1270 vs 6000
  eMonster m = eMonster(moBug0 + hrand(BUGCOLORS));
  if(c->wall) return;
  for(int i=0; i<c->type; i++) {
    cell *c2 = createMov(c,i);
    if(hrand(100+chance) < chance) {
      if(!c2->wall) c2->monst = m;
      for(int j=2; j<=c2->type-2; j++) {
        int jj = (j+c->spn[i]) % c2->type;
        cell *c3 = createMov(c2, jj);
        if(hrand(6000+chance) < chance && !c3->wall)
          c3->monst = m;
        }
      }
    }
  c->monst = eMonster(m + BUGCOLORS); */

  int gdir = -1;
  for(int i=0; i<c->type; i++) {
    if(c->mov[i] && c->mov[i]->mpdist < c->mpdist) gdir = i;
    }
  if(!gdir) return;
  cellwalker bf(c, gdir);
  for(int i=0; i<7; i++) {
    if(bf.c->type == 6)
      cwspin(bf, 3);
    else
      cwspin(bf, 3 + hrand(2));
    cwstep(bf);
    }
  cell *citycenter = bf.c;
  buginfo.clear();
  
  // mark the area with BFS
  bugcitycell(citycenter, 0);
  for(int i=0; i<size(buginfo); i++) {
    buginfo_t& b(buginfo[i]);
    cell *c = b.where;
    int d = b.dist[0];
    // ERRORS!
    if(c->land != laHive && c->land != laNone) return;
    if(c->bardir != NODIR) return;
    if(c->land == laHive && c->landparam >= 100) return;
    // bfs
    if(d < 9) for(int t=0; t<c->type; t++)
      bugcitycell(createMov(c,t), d+1);
    }

  // place everything
  for(int i=0; i<size(buginfo); i++) {
    buginfo_t& b(buginfo[i]);
    cell *c = b.where;
    int d = b.dist[0];
    if(d <= 1 && c->wall == waNone)
      c->item = itRoyalJelly;
    c->bardir = NOBARRIERS;
    if(d == 9 || d == 6 || d == 3)
      c->barleft = eLand(d/3),
      c->barright = eLand(k);
    else
      c->barleft = laNone;
    if(numbugs && c->wall == waNone)
      c->monst = eMonster(moBug0 + k), numbugs--;
    c->land = laHive;
    // prevent barriers
    if(c->mpdist == INFD) c->mpdist = BUGLEV;
    }
  }

void moveFastMonsters() {
  for(int i=0; i<size(pathqm); i++) {
    cell *c = pathqm[i];
    
    if(c->monst == moWitchSpeed) {
    
      if(c->cpdist == 1 && c->monst != moGhost) {
        killThePlayer(c->monst, ONLY_ONE_PLAYER_POSSIBLE);
        break;
        }
      
      int goodmoves = 0;
      for(int t=0; t<c->type; t++) {
        cell *c2 = c->mov[t];
        if(c2 && c2->pathdist < c->pathdist)
          goodmoves++;
        }
      movesofgood[goodmoves].push_back(c);
      }
    }

  for(int d=0; d<8; d++) for(int i=0; i<size(movesofgood[d]); i++) {
    cell *c = movesofgood[d][i];
    if(c->monst != moNone)
      moveNormal(c);
    }
  }

void activateFlashFrom(cell *cf);

vector<cell*> nonmovers;

bool sagefresh = true;

void considerEnemyMove(cell *c) {
  eMonster m = c->monst;
  
  if(isActiveEnemy(c, NULL, true)) {
  
    if(c->cpdist == 1 && c->monst != moGhost && isNeighbor(c, cwt.c) && !attackingForbidden(cwt.c, c)) {
      // c->iswall = true; c->ismon = false;
      if(c->monst == moCrystalSage) return;
      
      killThePlayer(m, ONLY_ONE_PLAYER_POSSIBLE);
      // playerdead = true;
      return;
      }
    
    if(c->monst == moWitch && c->item == itOrbSpeed) {
      addMessage(XLAT("%The1 picks up %the2!", moWitch, c->item));
      c->monst = moWitchSpeed; c->item = itNone;
      }

    if(c->monst == moNecromancer) {
      int gravenum = 0, zombienum = 0;
      cell *gtab[8], *ztab[8];
      for(int j=0; j<c->type; j++) if(c->mov[j]) {
        if(c->mov[j]->wall == waFreshGrave) gtab[gravenum++] = c->mov[j];
        if(passable(c->mov[j], c, false, false, false) && c->mov[j]->pathdist < c->pathdist)
          ztab[zombienum++] = c->mov[j];
        }
      if(gravenum && zombienum) {
        cell *gr = gtab[hrand(gravenum)];
        gr->wall = waAncientGrave;
        gr->monst = moGhost;
        ztab[hrand(zombienum)]->monst = moZombie;
        addMessage(XLAT("%The1 raises some undead!", c->monst));
        return;
        }
      }
    
    if(c->monst == moWolf) {
      int bhd = NODIR;
      ld besth = absheat(c);
      for(int j=0; j<c->type; j++) 
        if(isIcyLand(c->mov[j]))
        if(absheat(c->mov[j]) > besth && passable(c->mov[j], c, false, false, false))
          besth = absheat(c->mov[j]), bhd = j;
      if(bhd != NODIR) {
        // printf("wolf moved from %Lf (%p) to %Lf (%p)\n", c->heat, c, besth, c->mov[bhd]);
        moveMonster(c->mov[bhd], c);
        c->mov[bhd]->monst = moWolfMoved;
        }
      }
    
    else if(c->monst == moPyroCultist && c->cpdist <= 4 && makeflame(cwt.c, 20, true)) {
      addMessage(XLAT("%The1 throws fire at you!", c->monst));
      makeflame(cwt.c, 20, false);
      c->monst = moCultist;
      }

    else if(c->monst == moPyroCultist && c->cpdist <= 3) {
      // just wait until the player moves
      }

    else if(c->monst == moWitchFlash && flashWouldKill(c, true) && !flashWouldKill(c, false)) {
      addMessage(XLAT("%The1 activates her Flash spell!", c->monst));
      c->monst = moWitch;
      activateFlashFrom(c);
      }

    else if(c->monst == moCrystalSage && c->cpdist <= 4 && isIcyLand(cwt.c) && cwt.c->wall != waBoat) {
      // only one sage attacks
      if(sagefresh) {
        sagefresh = false;
        if(sagephase == 0) {
          addMessage(XLAT("%The1 shows you two fingers.", c->monst));
          addMessage(XLAT("You wonder what does it mean?"));
          }
        else if(sagephase == 1) {
          addMessage(XLAT("%The1 shows you a finger.", c->monst));
          addMessage(XLAT("You think about possible meanings."));
          }
        else {
          addMessage(XLAT("%The1 moves his finger downwards.", c->monst));
          addMessage(XLAT("Your brain is steaming."));
          }
        sagephase++;
        sageheat(cwt.c, .0);
        for(int i=0; i<cwt.c->type; i++)
          sageheat(cwt.c->mov[i], .3);
        }
      }
    
    else if(normalMover(m)) {
      int goodmoves = 0;
      for(int t=0; t<c->type; t++) {
        cell *c2 = c->mov[t];
        if(c2 && c2->pathdist < c->pathdist)
          goodmoves++;
        }
      movesofgood[goodmoves].push_back(c);
      }
    }
  }

void moveworms() {
  int wrm = size(worms);
  for(int i=0; i<wrm; i++) {
    moveWorm(worms[i]);
    }
  }

bool survivesWater(eMonster m) {
  return 
    m == moShark || m == moGreaterShark || m == moCShark || 
    m == moGhost || m == moWitchGhost || m == moTentacleGhost ||
    isBird(m) || m == moWaterElemental ||
    isWorm(m) || isIvy(m);
  }

bool survivesFire(eMonster m) {
  return
    m == moGhost || m == moWitchWinter || m == moWitchGhost ||
    m == moBomberbird || m == moTameBomberbird || m == moTameBomberbirdMoved ||
    isWorm(m);
  }

bool survivesMine(eMonster m) {
  return 
    isBird(m) || m == moGhost;
  }

bool survivesWall(eMonster m) {
  return m == moGhost;
  }

bool survivesChasm(eMonster m) {
  return 
    m == moGhost || isBird(m);
  }

void moverefresh() {
  int dcs = size(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    
    if(c->monst == moWolfMoved) c->monst = moWolf;
    if(c->monst == moIvyNext) {
      c->monst = moIvyHead; ivynext(c);
      }
    if(c->monst == moIvyDead) 
      removeIvy(c);
    if(c->monst == moGolemMoved) c->monst = moGolem;
    if(c->monst == moMouseMoved) c->monst = moMouse;
    if(c->monst == moPrincessMoved) c->monst = moPrincess;
    if(c->monst == moPrincessArmedMoved) c->monst = moPrincessArmed;
    if(c->monst == moKnightMoved) c->monst = moKnight;
    if(c->monst == moTameBomberbirdMoved) c->monst = moTameBomberbird;
    if(c->monst == moSlimeNextTurn) c->monst = moSlime;
    if(c->monst == moLesser) c->monst = moLesserM;
    else if(c->monst == moLesserM) c->monst = moLesser;
    if(c->monst == moGreater) c->monst = moGreaterM;
    else if(c->monst == moGreaterM) c->monst = moGreater;
    
    if(c->stuntime) c->stuntime--;

    if(c->wall == waChasm) {
      c->item = itNone;
      if(c->monst && !survivesChasm(c->monst)) {
        if(c->monst != moRunDog && c->land == laMotion) 
          achievement_gain("FALLDEATH1");
        addMessage(XLAT("%The1 falls!", c->monst));
        killWithMessage(c, false);
        }
      }

    if(isFire(c)) {
      if(c->monst && !survivesFire(c->monst)) {
        addMessage(XLAT("%The1 burns!", c->monst));
        killWithMessage(c, false);
        }
      }

    if(c->wall == waMineMine && c->monst && !survivesMine(c->monst))
      explodeMine(c);
    
    if(isWatery(c)) {
      if(c->monst == moLesser || c->monst == moLesserM || c->monst == moGreater || c->monst == moGreaterM)
        c->monst = moGreaterShark;
      if(c->monst && !survivesWater(c->monst)) {
        if(isNonliving(c->monst))
          addMessage(XLAT("%The1 sinks!", c->monst));
        else 
          addMessage(XLAT("%The1 drowns!", c->monst));
        killWithMessage(c, false);
        }
      }
    
    if(c->monst && c->wall == waClosedGate && !survivesWall(c->monst)) {
      addMessage(XLAT("%The1 is crushed!", c->monst));
      killWithMessage(c, false);
      }

    if(c->monst && cellUnstable(c) && !survivesChasm(c->monst) && !ignoresPlates(c->monst) && !shmup::on) {
      c->wall = waChasm;
      }
    }
  }

void movemonsters() {

  sagefresh = true;
  turncount++;

  DEBT("ghosts");
  moveghosts();
  
  DEBT("normal");
  
  for(int d=0; d<8; d++) movesofgood[d].clear();
  
  for(int i=0; i<size(pathqm); i++) {
    cell *c = pathqm[i];
    considerEnemyMove(c);
    }

  int dcs = size(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->pathdist == INFD)
      considerEnemyMove(c);
    }

  for(int d=0; d<8; d++) for(int i=0; i<size(movesofgood[d]); i++) {
    cell *c = movesofgood[d][i];
    if(normalMover(c->monst))
      moveNormal(c);
    }

  if(sagefresh) sagephase = 0;
  
  DEBT("worm");
  moveworms();
  DEBT("ivy");
  moveivy();
  DEBT("slimes");
  groupmove(moSlime, false);
  DEBT("eagles");
  if(haveeagles) groupmove(moEagle, false), groupmove(moEagle, true);
  DEBT("earth");
  if(haveearth) groupmove(moEarthElemental, false);
  DEBT("water");
  if(havewater) groupmove(moWaterElemental, false);
  DEBT("leader");
  if(haveleader) groupmove(moPirate, false);
  DEBT("hex");
  if(havehex) movehex();
  DEBT("bugs");
  if(havebugs) movebugs();
  DEBT("whirlpool");
  if(havewhirlpool) movewhirlpool();

  DEBT("golems");
  movegolems();
  
  DEBT("fresh");
  moverefresh();

  DEBT("shadow");
  moveshadow();
  
  DEBT("wandering");
  wandering();
  }

// move heat

vector<cell*> vinefires;

void processheat(double rate = 1, bool tick = true) {
  if(markOrb(itOrbSpeed)) rate /= 2;
  int oldmelt = kills[0];
  
  vector<cell*> offscreen2;
  
  for(int i=0; i<size(offscreen); i++) {
    cell *c = offscreen[i];
    if(c->cpdist > 7) {
      bool readd = false;
      if(isIcyLand(c)) {
        if(HEAT(c) < .01 && HEAT(c) > -.01)
          HEAT(c) = 0;
        else {
          HEAT(c) *= 1 - rate/10;
          readd = true;
          }
        }
      if(hasTimeout(c)) {
        useup(c);
        if(hasTimeout(c)) readd = true;
        }
      if(readd) offscreen2.push_back(c);
      }
    }
  
  offscreen.clear(); swap(offscreen, offscreen2);

  /* if(cwt.c->heat > .5)  cwt.c->heat += .3;
  if(cwt.c->heat > 1.)  cwt.c->heat += .3;
  if(cwt.c->heat > 1.4) cwt.c->heat += .5; */
  
  for(int i=0; i<numplayers(); i++) {
    cell *c = playerpos(i);
    double xrate = (c->land == laCocytus && shmup::on) ? rate/3 : rate;
    if(isIcyLand(c))
      HEAT(c) += (markOrb(itOrbWinter) ? -1.2 : 1.2) * xrate;
    }
  
  vinefires.clear();
  
  int dcs = size(dcal);
  for(int i=0; i<dcs; i++) {
    bool readd = false;
    cell *c = dcal[i];
    double xrate = (c->land == laCocytus && shmup::on) ? rate/3 : rate;
    if(c->cpdist > 8) break;

    if(hasTimeout(c)) {
      if(tick) useup(c);
      readd = true;
      }
    
    if(isFire(c) && tick) {
      if(c->wall != waPartialFire) for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if(c2 && c2->wall == waVinePlant)
          vinefires.push_back(c2);
        if(c2 && c2->wall == waBonfireOff) activateActiv(c2, false);
        // both halfvines have to be near fire at once
        if(c2 && cellHalfvine(c2) && c->mov[(i+1)%c->type]->wall == c2->wall)
          vinefires.push_back(c2);
        }
      
      // two semifires are required to spread
      if(c->wall == waPartialFire) for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if(c2 && (c2->wall == waVinePlant)) {
          for(int j=0; j<c2->type; j++) if(c2->mov[j] && c2->mov[j]->wall == waPartialFire &&
            c2->mov[j] != c)
            vinefires.push_back(c2);
          }
        }
      }
    if(isIcyLand(c)) {
      if(c->monst == moRanger) HEAT(c) += 3 * xrate;
      if(c->monst == moDesertman) HEAT(c) += 4 * xrate;
      if(c->monst == moMonkey) HEAT(c) += xrate;
      if(c->wall == waDeadTroll) HEAT(c) -= 2 * xrate;
      if(c->wall == waBigStatue) HEAT(c) -= .5 * xrate;
      if(c->monst == moLesser || c->monst == moLesserM || c->monst == moGreater || c->monst == moGreaterM)
        HEAT(c) += (c->land == laCocytus ? 1.5 : 10) * xrate;
      if(c->monst == moGreaterShark)
        HEAT(c) += 2 * xrate;
      if(c->monst == moCultist) HEAT(c) += 3 * xrate;
      if(c->monst == moCultistLeader) HEAT(c) += 4 * xrate;
      if(c->monst == moPyroCultist) HEAT(c) += 6 * xrate;
      if(c->monst == moFireFairy) HEAT(c) += 6 * xrate;
      if(c->monst == moFireElemental) HEAT(c) += 8 * xrate;
      if(c->monst == moGhost) HEAT(c) -= xrate;
      if(c->monst == moWaterElemental) HEAT(c) -= xrate;
      if(isFire(c)) HEAT(c) += 4 * xrate;
      if(isPrincess(c->monst)) HEAT(c) += 1.2 * xrate;
      
      ld hmod = 0;
      
      for(int j=0; j<c->type; j++) if(c->mov[j]) {
        if(!isIcyLand(c->mov[j])) {
          // make sure that we can still enter Cocytus,
          // it won't heat up right away even without Orb of Winter or Orb of Speed
          if(c->mov[j] == cwt.c && (c->land == laIce || markOrb(itOrbWinter))) 
            hmod += (markOrb(itOrbWinter) ? -1.2 : 1.2) / 4;
          continue;
          }
        ld hdiff = absheat(c->mov[j]) - absheat(c);
        hdiff /= 10;
        if(shmup::on && (c->land == laCocytus || c->mov[j]->land == laCocytus))
          hdiff /= 3;
        if(c->mov[j]->cpdist <= 7)
          HEAT(c->mov[j]) -= hdiff * rate;
        else
          hdiff = -HEAT(c) / 30;
        hmod += hdiff;
        }
      
      HEAT(c) += hmod * rate;
      if(c->monst == moCrystalSage && HEAT(c) >= SAGEMELT) {
        addMessage(XLAT("%The1 melts away!", c->monst));
        killWithMessage(c, false);
        }
      }
    
    if(readd || HEAT(c)) 
      offscreen.push_back(c);
    }
  
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->wall == waIcewall && HEAT(c) > .4) c->wall = waNone, kills[0]++;
    if(c->wall == waFrozenLake && HEAT(c) > (c->land == laCocytus ? .6 : .4)) c->wall = waLake, kills[0]++;

    if(c->wall == waLake && HEAT(c) < (c->land == laCocytus ? -.4 : .4) && c->monst != moGreaterShark) {
      c->wall = waFrozenLake;
      if(c->monst == moShark || c->monst == moCShark) {
        addMessage(XLAT("%The1 is frozen!", c->monst));
        killWithMessage(c, false);
        }
      }
    }

  if(tick) for(int i=0; i<size(vinefires); i++) {
    cell* c = vinefires[i];
    if(c->wall == waVinePlant)
      makeflame(c, 6, false);
    else if(cellHalfvine(c)) destroyHalfvine(c, waPartialFire, 6);
    }
  
  if(kills[0] != oldmelt) bfs();
  }

bool gardener = false;

void livecaves() {
  int dcs = size(dcal);
  
  vector<cell*> bringlife;
  
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->cpdist > 8) break;
    
    if(c->wall == waCavefloor || c->wall == waCavewall) {
      c->aitmp = 0;
      if(c->monst == moDarkTroll) c->monst = moTroll;
      if(c->item || c->monst || c->cpdist == 0) continue;
      for(int j=0; j<c->type; j++) if(c->mov[j]) {
        if(c->mov[j]->wall == waDeadfloor) c->aitmp++, bringlife.push_back(c->mov[j]);
        else if(c->mov[j]->wall == waDeadwall || c->mov[j]->wall == waDeadfloor2) c->aitmp--, bringlife.push_back(c->mov[j]);
        else if(c->mov[j]->wall == waCavefloor) c->aitmp++;
        else if(c->mov[j]->wall == waCavewall) c->aitmp--;
        else if(c->mov[j]->wall == waRubble) c->aitmp--;
        else if(c->mov[j]->wall == waGargoyle) c->aitmp--;
        else if(c->mov[j]->wall == waGargoyleFloor) c->aitmp--;
        else if(c->mov[j]->wall == waGargoyleBridge) c->aitmp--;
        else if(c->mov[j]->wall == waDeadTroll) c->aitmp -= 5;
        else if(c->mov[j]->wall == waVinePlant) c->aitmp--;
        else if(c->mov[j]->wall != waBarrier) c->aitmp += 5;      
        if(c->mov[j]->cpdist == 0 && markOrb(itOrbDigging)) c->aitmp+=100;
        if(c->mov[j]->wall == waThumperOn) c->aitmp+=100;
        if(c->mov[j]->wall == waFire) c->aitmp+=100;
        if(c->mov[j]->wall == waBigStatue) c->aitmp-=100;
        if(c->mov[j]->item) c->aitmp+=2;
        if(c->mov[j]->monst == moZombie) c->aitmp += 10;
        if(c->mov[j]->monst == moGhost) c->aitmp += 10;
        if(c->mov[j]->monst == moGargoyle) c->aitmp--;
        if(c->mov[j]->monst == moNecromancer) c->aitmp += 10;
        if(c->mov[j]->monst == moWormtail) c->aitmp++;
        if(c->mov[j]->monst == moTentacletail) c->aitmp-=2;
        if(isIvy(c->mov[j])) c->aitmp--;
        if(isDemon(c->mov[j])) c->aitmp-=3;
        // if(c->mov[j]->monst) c->tmp++;
        // if(c->mov[j]->monst == moTroll) c->tmp -= 3;
        }
      }
    else if(c->land == laLivefjord) {
      c->aitmp = 0;
      if(c->monst == moWaterElemental)
        c->aitmp += 1000;
      if(isPlayerInBoatOn(c) && markOrb(itOrbWater))
        c->aitmp += 1000;
      if(c->monst == moEarthElemental)
        c->aitmp -= 1000;
      if(isPlayerOn(c) && markOrb(itOrbDigging))
        c->aitmp -= 1000;
      for(int j=0; j<c->type; j++) if(c->mov[j]) {
        cell *c2 = c->mov[j];
        if(c2->wall == waNone || c2->wall == waStrandedBoat)
          c->aitmp -= (c2->land == laLivefjord ? 1 : 100);
        if(c2->wall == waTempFloor || c2->wall == waTempBridge)
          ;
        else if(c2->wall == waDeadTroll || c2->wall == waThumperOn || isFire(c2) || snakelevel(c2))
          c->aitmp -= 10;
        if(c2->wall == waBigStatue)
          c->aitmp -= 10;
        if(c2->wall == waSea || c2->wall == waBoat)
          c->aitmp += (c2->land == laLivefjord ? 1 : 100);
        if(c2->monst == moWaterElemental)
          c->aitmp += 1000;
        if(c2 == cwt.c && c2->wall == waBoat && markOrb(itOrbWater))
          c->aitmp += 1000;
        if(c2->monst == moEarthElemental)
          c->aitmp -= 1000;
        if(c2 == cwt.c && markOrb(itOrbDigging))
          c->aitmp -= 1000;
        if(c2->wall == waBarrier) {
          bool landbar = false;
          for(int k=0; k<c2->type; k++)
            if(c2->mov[k]) {
              cell *c3 = c2->mov[k];
              if(!isSealand(c3->land))
                landbar = true;
              }
          if(landbar) c->aitmp -= 5;
          else c->aitmp += 5;
          }
        }
      }
    }

  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->cpdist > 8) break;
    if(c->wall == waCavefloor || c->wall == waCavewall) {
  //  if(c->land != laCaves) continue;
  //  if(c->wall == waThumper || c->wall == waBonfire) continue;
      
      if(c->aitmp > 0) c->wall = waCavefloor;
      if(c->aitmp < 0) {
        c->wall = waCavewall;
        if(c->land != laCaves && c->land != laDeadCaves && c->land != laEmerald && !gardener) {
          gardener = true;
          achievement_gain("GARDENER");
          }
        }
      }
    else if(c->land == laLivefjord) {
      if(c->aitmp > 0 && c->wall == waStrandedBoat) c->wall = waBoat;
      if(c->aitmp > 0 && c->wall == waNone) {
        if(c->item && c->cpdist == 1 && markOrb(itOrbWater))
          collectItem(c);
        c->wall = waSea;
        }
      if(c->aitmp < 0 && c->wall == waBoat) c->wall = waStrandedBoat;
      if(c->aitmp < 0 && c->wall == waSea) c->wall = waNone;
      }     
    }
  
  for(int i=0; i<size(bringlife); i++) {
    cell *c = bringlife[i];
    if(!lifebrought) { lifebrought = true;
      achievement_gain("LIFEBRINGER");
      }
    if(c->wall == waDeadfloor) c->wall = waCavefloor;
    if(c->wall == waDeadfloor2) c->wall = waCavewall;
    if(c->wall == waDeadwall) c->wall = waCavewall;
    if(c->wall == waCavewall && c->item) c->wall = waCavefloor;
    if(c->land == laDeadCaves) c->land = laCaves;
    if(c->item == itSilver) c->item = itGold;
    if(c->item == itGreenStone) c->item = itOrbLife;
    if(c->monst == moEarthElemental) {
      addMessage(XLAT("%The1 is destroyed by the forces of Life!", c->monst));
      killWithMessage(c, false);
      c->item = itOrbDigging;
      }
    }
  }

void dryforest() {
  int dcs = size(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->cpdist > 8) break;
    if(c->land != laDryForest) continue;
    
    for(int j=0; j<c->type; j++) if(c->mov[j]) {
      if(isFire(c->mov[j])) c->landparam++;
      }

    if(c->landparam >= 10) makeflame(c, 10, false), c->landparam = 0;
    }

  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->cpdist > 8) break;
    if(c->land != laDryForest) continue;
    if((c->wall == waDryTree || c->wall == waWetTree || isFire(c)) && c->landparam >= 1)
      c->wall = waEternalFire;
    }

/*
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    c->tmp = 0;
    c->heat = 0;
    if(c->cpdist > 8) break;
    if(c->land != laDryForest) continue;
    
    for(int j=0; j<c->type; j++) if(c->mov[j]) {
      if(c->mov[j]->wall == waWetTree)
        c->tmp++;
      if(c->mov[j]->wall == waDryTree)
        c->heat++;
      }
    }

  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    int a = c->type - c->tmp - int(c->heat);
    if(c->tmp > a && c->tmp > c->heat)
      c->wall = waWetTree;
    else if(c->heat > a && c->heat > c->tmp)
      c->wall = waDryTree;
    else if(a > c->heat && a > c->tmp)
      c->wall = waNone;
    } */
  }

// mirror management

bool cellMirrorable(cell *c) {
  return 
    c->wall == waNone || c->wall == waCavefloor || c->wall == waFloorA || 
    c->wall == waFloorB ||
    c->wall == waFrozenLake || c->wall == waDeadfloor || c->wall == waDeadfloor2 ||
    c->wall == waGiantRug || c->wall == waCIsland || c->wall == waCIsland2 ||
    c->wall == waGargoyleFloor || c->wall == waRubble ||
    c->wall == waGargoyleBridge || c->wall == waTempFloor || c->wall == waTempBridge;
  }

void createMM(cellwalker& cw, eMonster type) {
  if(type == moLightningBolt)
    castLightningBolt(cw);
  else if(cw.c->monst == moNone && cellMirrorable(cw.c) && cw.c != cwt.c)  {
    cw.c->monst = type;
    cw.c->mondir = cw.spin;
    }
  }

void createMirrors(cell *c, int dir, eMonster type) {
  cellwalker C(c, dir);
  
  if(type == moMirror) type = moMirage;
  else if(type == moMirage) type = moMirror;

  for(int i=0; i<6; i++) {
    cwstep(C);
    if(C.c->type == 6) {
      cwspin(C, i);
      createMM(C, type);
      cwspin(C, -i);
      }
    cwstep(C);
    cwspin(C, 1);
    }
  }

void createMirages(cell *c, int dir, eMonster type) {
  cellwalker C(c, dir);
  for(int i=0; i<6; i++) {
    cwstep(C);
    if(C.c->type == 6) {
      cwspin(C, 2);
      cwstep(C);
      cwspin(C, 4-i);
      createMM(C, type);
      cwspin(C, 6-4+i);
      cwstep(C);
      cwspin(C, 2);
      cwstep(C);
      cwspin(C, 2-i);
      createMM(C, type);
      cwspin(C, 6-2+i);
      cwstep(C);
      cwspin(C, 2);
      }
    cwstep(C);
    cwspin(C, 1);
    }
  }

void spinmirrors(int d) {

  for(int i=0; i<size(mirrors); i++) {
    cell *c = mirrors[i];
    if(c->monst == moMirror) 
      mirrors[i]->mondir = (mirrors[i]->mondir - d + 42) % mirrors[i]->type;
    if(c->monst == moMirage)
      mirrors[i]->mondir = (mirrors[i]->mondir + d + 42) % mirrors[i]->type;
    }

  }

void destroyMirrors() {
  for(int i=0; i<size(mirrors); i++) {
    cell *c = mirrors[i];
    eMonster m = c->monst;
    if(isMimic(m)) c->monst = moNone;
    }
  mirrors.clear();
  }

void destroyStrayMirrors() {
  for(int i=0; i<size(mirrors2); i++) {
    cell *c = mirrors2[i];
    if(c->cpdist > 7 && isMimic(c)) {
      c->monst = moNone;
      }
    }
  }

void gomirrors(bool go) {
  int tk = tkills();
  int nummirage = 0;
  mirrors2.clear();
  for(int i=0; i<size(mirrors); i++) {
    cell *c = mirrors[i];
    eMonster m = c->monst;
    if(isMimic(m)) {
      if(m == moMirage) nummirage++;
      cell *c2 = c->mov[c->mondir];
      if(c2 && c2->monst != moNone && !isMimic(c2) && isKillable(c2)) {
        if(!attackJustStuns(c2))
          messageKill(m, c2->monst);
        killOrStunMonster(c2);
        }
      if(c2->wall == waDryTree)
        c2->wall = waWetTree;
      else if(c2->wall == waWetTree)
        c2->wall = waNone;
      if(!go) continue;
      c->monst = moNone;
      if(!c2) continue;
      if(!passable(c2, c, true, true, false))  continue;
      if(isWorm(c2)) continue;
      if(c2->monst == moGreater) {
        c2->monst = moLesser; continue;
        }
      if(c2->monst == moGreaterM) {
        c2->monst = moLesserM; continue;
        }
      if(c2 == cwt.c) {
        addMessage(XLAT("You join %the1.", m));
        continue;
        }
      if(isMimic(c2)) {
        addMessage(XLAT("Two of your images crash and disappear!"));
        c2->monst = moNone;
        continue;
        }
      if(isIvy(c2)) {
        // killIvy(c2);
        continue;
        }
      c->monst = m; moveMonster(c2, c);
      mirrors2.push_back(c2);      
      }
    }
  for(int i=0; i<size(mirrors2); i++) {
    cell *c = mirrors2[i];
    eMonster m = c->monst;
    if(c->wall == waMirror) {
      addMessage(XLAT("%The1 breaks the mirror!", m));
      createMirrors(c, c->mondir, m);
      c->wall = waNone;
      }
    if(c->wall == waCloud) {
      addMessage(XLAT("%The1 disperses the cloud!", m));
      createMirages(c, c->mondir, m);
      c->wall = waNone;
      }
    }
  achievement_count("MIRRORKILL", tkills(), tk);
  achievement_count("MIRAGE", nummirage, 0);
  }

bool reduceOrbPower(eItem it, int cap) {
  if(items[it] && (lastorbused[it] || !markOrb(itOrbPreserve) || (it == itOrbShield && items[it]>3))) {
    items[it] -= numplayers();
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
  for(int i=0; i<ittypes; i++) 
    lastorbused[i] = orbused[i], orbused[i] = false;
  if(items[itOrbShield]) orbused[itOrbShield] = lastorbused[itOrbShield];
  reduceOrbPower(itOrbPreserve, cwt.c->land == laCaribbean ? 777 : 150);
  if(invismove && !invisfish) markOrb(itOrbInvis);
  reduceOrbPower(itOrbLightning, 777);
  reduceOrbPower(itOrbSpeed, 67);
  reduceOrbPower(itOrbShield, 77);
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
  }

void flashAlchemist(cell *c) {
  if(c->wall == waFloorA || c->wall == waFloorB) {
    if(cwt.c->wall == waFloorA || cwt.c->wall == waFloorB)
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
  if(c->wall == waAncientGrave) c->wall = waNone;
  if(c->wall == waFreshGrave) c->wall = waNone;
  if(c->wall == waColumn)    c->wall = waNone;
  if(c->wall == waGlass)     c->wall = waNone;
  if(c->wall == waDryTree || c->wall == waWetTree)    c->wall = waNone;
  if(c->wall == waBigStatue) c->wall = waNone;
  if(c->wall == waCTree)     c->wall = waCIsland2;
  if(c->wall == waPalace)    c->wall = waRubble;
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

void activateFlash() {
  int tk = tkills();
  drawFlash(cwt.c);
  addMessage(XLAT("You activate the Flash spell!"));
  items[itOrbFlash] = 0;
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
    c.c->mov[d]->land == laElementalWall) return true;
  return false;
  }

void castLightningBolt(cellwalker lig) {
  int bnc = 0;
  while(true) {
    // printf("at: %p i=%d d=%d\n", lig.c, i, lig.spin);
    if(lig.c->mov[lig.spin] == 0) break;
    cwstep(lig);
    
    cell *c = lig.c;

    flashAlchemist(c);
    killWithMessage(c, false);
    if(isIcyLand(c)) HEAT(c) += 2;
    if(c->land == laDryForest) c->landparam += 2;
    c->ligon = 1;
    
    bool brk = false, spin = false;

    if(c->wall == waGargoyle)  brk = true;
    if(c->wall == waCavewall)  c->wall = waCavefloor, brk = true;
    if(c->wall == waDeadTroll) c->wall = waCavefloor, brk = true;
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
    
    if(c->wall == waBoat)    c->wall = waSea, spin = true;
    if(c->wall == waStrandedBoat)    c->wall = waNone, spin = true;

    if((c->wall == waNone || c->wall == waSea) && c->land == laLivefjord)
      c->wall = eWall(c->wall ^ waSea ^ waNone);
    
    if(c->wall == waRed1)      c->wall = waNone;
    if(c->wall == waRed2)      c->wall = waRed1;
    if(c->wall == waRed3)      c->wall = waRed2, brk = true;
    
    if(isActivable(c))         activateActiv(c, false);
    if(c->wall == waDryTree || c->wall == waWetTree || c->wall == waVinePlant)    {
      makeflame(c, 4, false);
      brk = true;
      }
    if(c->wall == waCTree) makeflame(c, 12, false);
    if(cellHalfvine(c) && c->mov[lig.spin] && c->wall == c->mov[lig.spin]->wall) {
      destroyHalfvine(c, waPartialFire, 4);
      brk = true;
      }

    if(c == cwt.c)             {bnc++; if(bnc > 10) break; }
    if(spin) cwspin(lig, hrand(lig.c->type));
    
    if(brk) break;
    
    if(c->wall == waBarrier || c->wall == waCamelot || c->wall == waPalace || c->wall == waPlatform ||
      c->wall == waTempWall)   {
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
      createMirages(c, lig.spin, moLightningBolt);
      }

    if(c->wall == waMirror) {
      c->wall = waNone;
      createMirrors(c, lig.spin, moLightningBolt);
      break;
      }
    }
  }

void activateLightning() {
  int tk = tkills();
  drawLightning();
  addMessage(XLAT("You activate the Lightning spell!"));
  items[itOrbLightning] = 0;
  for(int i=0; i<cwt.c->type; i++) 
    castLightningBolt(cellwalker(cwt.c, i));
  achievement_count("LIGHTNING", tkills(), tk);
  }

// move the PC in direction d (or stay in place for d == -1)

bool checkNeedMove(bool checkonly, bool attacking) {
  if(cwt.c->wall == waRoundTable) {
    if(markOrb2(itOrbGhost)) return false;
    if(checkonly) return true;
    addMessage(XLAT("It would be impolite to land on the table!"));
    }
  else if(cwt.c->wall == waLake) {
    if(markOrb2(itOrbGhost)) return false;
    if(checkonly) return true;
    addMessage(XLAT("Ice below you is melting! RUN!"));
    }
  else if(!attacking && cellEdgeUnstable(cwt.c)) {
    if(markOrb2(itOrbGhost)) return false;
    if(checkonly) return true;
    addMessage(XLAT("Nothing to stand on here!"));
    }
  else if(cwt.c->wall == waSea || cwt.c->wall == waCamelotMoat) {
    if(markOrb(itOrbFish)) return false;
    if(markOrb2(itOrbGhost)) return false;
    if(checkonly) return true;
    addMessage(XLAT("You have to run away from the water!"));
    }
  else if(cwt.c->wall == waClosedGate) {
    if(markOrb2(itOrbGhost)) return false;
    if(checkonly) return true;
    addMessage(XLAT("The gate is closing right on you! RUN!"));
    }
  else if(isFire(cwt.c) && !markOrb(itOrbWinter) && !markOrb2(itOrbShield)) {
    if(markOrb2(itOrbGhost)) return false;
    if(checkonly) return true;
    addMessage(XLAT("This spot will be burning soon! RUN!"));
    }
  else if(items[itOrbGhost] == 1 && !player_passable(cwt.c, NULL, false)) {
    if(markOrb2(itOrbGhost)) return false;
    if(checkonly) return true;
    addMessage(XLAT("Your Aether power has expired! RUN!"));
    }
  else if(cwt.c->wall == waChasm) {
    if(markOrb2(itOrbGhost)) return false;
    if(checkonly) return true;
    addMessage(XLAT("The floor has collapsed! RUN!"));
    }
  else return false;
  if(hardcore) { 
    canmove = false;
    return false;
    }
  return true;
  }

#define YDIST 101

struct yendorinfo {
  cell *path[YDIST];
  bool found;
  };

vector<yendorinfo> yi;

int yii = 0;

enum eYendorState { ysUntouched, ysLocked, ysUnlocked };

eYendorState yendorState(cell *yendor) {
  for(int i=0; i<size(yi); i++) if(yi[i].path[0] == yendor)
    return yi[i].found ? ysUnlocked : ysLocked;
  return ysUntouched;
  }

bool checkYendor(cell *yendor, bool checkonly) {
  int byi = size(yi);
  for(int i=0; i<size(yi); i++) if(yi[i].path[0] == yendor) byi = i;
  if(byi < size(yi) && yi[byi].found) return true;
  if(checkonly) return false;
  if(byi == size(yi)) {
    yendorinfo nyi;
    nyi.path[0] = yendor;

    cellwalker lig(yendor, hrand(yendor->type));

    cell *prev = yendor;
    
    generatingYendor = true;
    
    for(int i=0; i<YDIST-1; i++) {
      nyi.path[i] = lig.c;
      
      prev = lig.c;
      cwstep(lig);
      cwspin(lig, 3);
      if(lig.c->type == 7) cwspin(lig, hrand(2));
            
      setdist(lig.c, 10, prev);
      setdist(lig.c, 9, prev);
      setdist(lig.c, 8, prev);
      setdist(lig.c, 7, prev);
      }

    nyi.path[YDIST-1] = lig.c;
    nyi.found = false;

    cell *key = lig.c;

    generatingYendor = false;

    for(int b=10; b>=7; b--) setdist(key, b, prev);
    
    for(int i=-1; i<key->type; i++) {
      cell *c2 = i >= 0 ? key->mov[i] : key;
      c2->monst = moNone; c2->item = itNone;
      if(!passable(c2, NULL, true, true, false)) {
        if(c2->wall == waCavewall) c2->wall = waCavefloor;
        else if(c2->wall == waDeadwall) c2->wall = waDeadfloor2;
        else if(c2->wall == waLake) c2->wall = waFrozenLake;
        else if(c2->land == laCaribbean) c2->wall = waCIsland;
        else if(c2->land == laOcean) c2->wall = waCIsland;
        else if(c2->land == laRedRock) c2->wall = waRed3;
        else if(c2->land == laWhirlpool) 
          c2->wall = waBoat, c2->monst = moPirate, c2->item = itOrbWater;
        else c2->wall = waNone;
        }
      if(c2->land == laEdge && key->land == laEdge &&
        c2->landparam < key->landparam)
          c2->wall = waPlatform;
      }
    key->item = itKey;

    yi.push_back(nyi);
    }
  yii = byi;
  addMessage(XLAT("You need to find the right Key to unlock this Orb of Yendor!"));
  achievement_gain("YENDOR1");
  return false;
  }

void checkOnYendorPath() {
  yendorPath = false;
  if(yii < size(yi)) {
    for(int i=0; i<YDIST; i++) if(yi[yii].path[i]->cpdist <= 7) {
      yendorPath = true;
      }
    }
  }

int countMyGolems(eMonster m) {
  int g=0, dcs = size(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->monst == m) g++;
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
        (c->mpdist >= 3 && eaglepassable(c, NULL)) : 
        passable(c, NULL, false, false, false)) {
      c->hitpoints = hp / qty;
      c->monst = m, qty--, hp -= c->hitpoints;
      if(m == moPrincess || m == moPrincessArmed)
        princess::newFakeInfo(c);
      }
    }
  }

void activateSafety(eLand l) {
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
  if(l == laWhirlpool) l = laOcean;
  if(l == laCamelot) l = laCrossroads;
  firstland = l;
  for(int i=0; i<65536; i++) euland[i] = laNone;
  euland[0] = euland[1] = firstland;
  safety = true;
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

bool hasSafeOrb(cell *c) {
  return 
    c->item == itOrbSafety ||
    c->item == itOrbShield ||
    c->item == itOrbYendor;
  }

void checkmove() {
  if(hardcore) return;
  bool orbusedbak[ittypes];
  
  // do not activate orbs!
  for(int i=0; i<ittypes; i++) orbusedbak[i] = orbused[i];

  canmove = false;
  if(movepcto(-1, 0, true)) canmove = true;
  if(!canmove)
    for(int i=0; i<cwt.c->type; i++) 
      if(movepcto(1, -1, true)) canmove = true;
  if(!canmove)
    for(int i=0; i<cwt.c->type; i++) 
      if(movepcto(1, 1, true)) canmove = true;
  if(!canmove)
    achievement_final(true);
  if(canmove && timerstopped) {
    timerstart = time(NULL);
    timerstopped = false;
    }

  for(int i=0; i<ittypes; i++) orbused[i] = orbusedbak[i];
  }

// move the PC. Warning: a very long function! todo: refactor

void placeGolem(cell *on, cell *moveto, eMonster m) {
  if(passable(on, moveto, false, false, false) || m == moTameBomberbird)
    cwt.c->monst = m;
  else {
    cwt.c->monst = m;
    killWithMessage(cwt.c);
    if(isFire(on))
      addMessage(XLAT("%The1 burns!", m));
    else if(on->wall == waChasm)
      addMessage(XLAT("%The1 falls!", m));
    else if(isWatery(on) && isNonliving(m))
      addMessage(XLAT("%The1 sinks!", m));
    else if(isWatery(on))
      addMessage(XLAT("%The1 drowns!", m));
    else if(on->wall == waClosedGate)
      addMessage(XLAT("%The1 is crushed!", m));
    }
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
  }      

// roCheck: return orb type if successful, 0 otherwise
// roMouse/roKeyboard: 
//    return orb type if successful, eItem(-1) if do nothing, 0 otherwise
eItem targetRangedOrb(cell *c, orbAction a);

bool haveRangedOrb() {
  return 
    items[itOrbPsi] || items[itOrbDragon] || items[itOrbTeleport] ||
    items[itOrbIllusion] || items[itOrbTelekinesis] || items[itOrbAir] ||
    items[itOrbFrog] || items[itOrbSummon] || items[itOrbMatter];
  }

bool isRangedOrb(eItem i) {
  return i == itOrbPsi || i == itOrbDragon || i == itOrbTeleport || i == itOrbIllusion ||
    i == itOrbTelekinesis || i == itOrbAir || i == itOrbFrog;
  }

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

bool cantGetGrimoire(cell *c2, bool verbose = true) {
  if(!euclid && !c2->master->alt) return false;
  if(c2->item == itGrimoire && items[itGrimoire] > celldistAlt(c2)/-TEMPLE_EACH) {
    if(verbose)
      addMessage(XLAT("You already have this Grimoire! Seek new tomes in the inner circles."));
    return true;
    }
  return false;
  }

bool collectItem(cell *c2, bool telekinesis) {

  int pg = gold();
  bool dopickup = true;
  
  if(itemHidden(c2) && !telekinesis && !(isWatery(c2) && markOrb(itOrbFish)))
    return false;
  
  if(c2->item == itOrbYendor && telekinesis && yendorState(c2) != ysUnlocked)
    return false;

  /* if(c2->item == itHolyGrail && telekinesis)
    return false; */

  if(c2->item) {
    invismove = false;
    if(shmup::on) shmup::visibleFor(2000);
    string s0 = "";
    
    if(c2->item == itPalace && items[c2->item] == 12)
      princess::forceVizier = true;
    
    if(isElementalShard(c2->item)) {
      int tsh = 
        items[itFireShard] + items[itAirShard] + items[itWaterShard] + items[itEarthShard] +
        items[itElemental];
      if(tsh == 0) {
        addMessage(XLAT("Collect four different Elemental Shards!"));
        addMessage(XLAT("Unbalanced shards in your inventory are dangerous."));
        }
      else {
        string t = XLAT("You collect %the1.", c2->item);
        addMessage(t);
        }
      }
    else if(c2->item == itKey)
      addMessage(XLAT("You have found the Key! Now unlock this Orb of Yendor!"));
    else if(c2->item == itGreenStone && !items[itGreenStone])
      addMessage(XLAT("This orb is dead..."));
    else if(c2->item == itGreenStone)
      addMessage(XLAT("Another Dead Orb."));
    else if(itemclass(c2->item) != IC_TREASURE)
      addMessage(XLAT("You have found %the1!", c2->item));
    else if(gold() == 0)
      addMessage(XLAT("Wow! %1! This trip should be worth it!", c2->item));
    else if(gold() == 1)
      addMessage(XLAT("For now, collect as much treasure as possible..."));
    else if(gold() == 2)
      addMessage(XLAT("Prove yourself here, then find new lands, with new quests..."));
    else if(!items[c2->item] && itemclass(c2->item) == IC_TREASURE)
      addMessage(XLAT("You collect your first %1!", c2->item));
    else if(items[c2->item] == 4 && maxgold() == 4) {
      addMessage(XLAT("You feel that %the2 become%s2 more dangerous.", c2->item, c2->land));
      addMessage(XLAT("With each %1 you collect...", c2->item, c2->land));
      }
    else if(items[c2->item] == 9 && maxgold() == 9)
      addMessage(XLAT("Are there any magical orbs in %the1?...", c2->land));
    else if(items[c2->item] == 10 && maxgold() == 10) {
      addMessage(XLAT("You feel that %the1 slowly become%s1 dangerous...", c2->land));
      addMessage(XLAT("Better find some other place."));
      }
    else if(c2->item == itSpice && items[itSpice] == 7)
      addMessage(XLAT("You have a vision of the future, fighting demons in Hell..."));
    else if(c2->item == itSpice && items[itSpice] == 10)
      addMessage(XLAT("You will be fighting red rock snakes, too..."));
//  else if(c2->item == itFeather && items[itFeather] == 10)
//    addMessage(XLAT("There should be a Palace somewhere nearby..."));
    else if(c2->item == itElixir && items[itElixir] == 4)
      addMessage(XLAT("With this Elixir, your life should be long and prosperous..."));
    else if(c2->item == itBone && items[itBone] == 6)
      addMessage(XLAT("The Necromancer's Totem contains hellish incantations..."));
    else if(c2->item == itStatue && items[itStatue] == 6)
      addMessage(XLAT("The inscriptions on the Statue of Cthulhu point you toward your destiny..."));
    else if(c2->item == itStatue && items[itStatue] == 4)
      addMessage(XLAT("There must be some temples of Cthulhu in R'Lyeh..."));
    else if(c2->item == itDiamond && items[itDiamond] == 8)
      addMessage(XLAT("Still, even greater treasures lie ahead..."));
    else if(c2->item == itFernFlower && items[itFernFlower] == 4)
      addMessage(XLAT("You overheard Hedgehog Warriors talking about emeralds..."));
    else if(c2->item == itEmerald && items[itEmerald] == 4)
      addMessage(XLAT("You overhear miners talking about a castle..."));
    else if(c2->item == itEmerald && items[itEmerald] == 5)
      addMessage(XLAT("A castle in the Crossroads..."));
    else {
      string t = XLAT("You collect %the1.", c2->item);
      addMessage(t);
      }
    }
  
  if(c2->item == itOrbSpeed) {
    items[c2->item] += 31;
    }
  else if(c2->item == itOrbLife) {
    if(shmup::on) {
      items[c2->item] ++;
      if(items[c2->item] > 5) items[c2->item] = 5;
      }
    else placeGolem(cwt.c, c2, moGolem);
    }
  else if(c2->item == itOrbFriend) {
    if(shmup::on) {
      items[itOrbLife] ++;
      if(items[itOrbLife] > 5) items[itOrbLife] = 5;
      }
    else placeGolem(cwt.c, c2, moTameBomberbird);
    }
  else if(c2->item == itOrbSafety) {
    items[c2->item] += 7;
    if(shmup::on)
      shmup::safety = true;
    else 
      activateSafety(c2->land);
    return true;
    }
  else if(c2->item == itOrbLightning) {
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbPreserve) {
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbLove) {
    items[c2->item] += 31;
    }
  else if(c2->item == itOrbTelekinesis) {
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbThorns) {
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbFlash) {
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbShield) {
    items[c2->item] += 16;
    orbused[itOrbShield] = false;
    }
  else if(c2->item == itOrbWater) {
    items[c2->item] += 31;
    }
  else if(c2->item == itOrbAir) {
    items[c2->item] += 67;
    }
  else if(c2->item == itOrbFrog) {
    items[c2->item] += 45;
    }
  else if(c2->item == itOrbDiscord) {
    // make it seem to be 23
    if(!items[c2->item]) items[c2->item]++;
    items[c2->item] += 23;
    }
  else if(c2->item == itOrbSummon) {
    items[c2->item] += 121;
    }
  else if(c2->item == itOrbMatter) {
    items[c2->item] += 67;
    }
  else if(c2->item == itOrbFish) {
    items[c2->item] += 31;
    }
  else if(c2->item == itOrbWinter) {
    items[c2->item] += 31;
    }
  else if(c2->item == itOrbFire) {
    items[c2->item] += 31;
    }
  else if(c2->item == itOrbDragon) {
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbIllusion) {
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbPsi) {
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbInvis) {
    items[c2->item] += 31;
    }
  else if(c2->item == itOrbGhost) {
    items[c2->item] += 31;
    }
  else if(c2->item == itOrbDigging) {
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbTeleport) {
    items[c2->item] += 78;
    }
  else if(c2->item == itOrbYendor) {
    items[itOrbShield] += 31;
    // Shielding always, so that we know that it protects!
    for(int i=0; i<4; i++) switch(hrand(13)) {
      case 0: items[itOrbSpeed] += 31; break;
      case 1: items[itOrbLightning] += 78; break;
      case 2: items[itOrbFlash] += 78; break;
      case 3: items[itOrbPreserve] += 78; break;
      case 4: items[itOrbWinter] += 151; break;
      case 5: items[itOrbDigging] += 151; break;
      case 6: items[itOrbTeleport] += 151; break;
      case 7: items[itOrbThorns] += 151; break;
      case 8: items[itOrbInvis] += 151; break;
      case 9: items[itOrbPsi] += 151; break;
      case 10: items[itOrbGhost] += 151; break;
      case 11: items[itOrbFire] += 151; break;
      case 12: items[itOrbTelekinesis] += 78; break;
      }
    items[itOrbYendor]++;
    items[itKey]--;
    addMessage(XLAT("CONGRATULATIONS!"));
    achievement_collection(itOrbYendor, pg, gold());
    achievement_victory(false);
    }
  else if(c2->item == itHolyGrail) {
    int v = newRoundTableRadius() + 12;
    items[itOrbTeleport] += v;
    items[itOrbSpeed] += v;
    items[itHolyGrail]++;
    addMessage(XLAT("Congratulations! You have found the Holy Grail!"));
    if(!euclid) c2->master->alt->emeraldval |= GRAIL_FOUND;
    achievement_collection(c2->item, pg, gold());
    }
  else if(c2->item == itKey) {
    for(int i=0; i<size(yi); i++) if(yi[i].path[YDIST-1] == c2)
      yi[i].found = true;
    items[itKey]++;
    }
  else if(!telekinesis && cantGetGrimoire(c2)) {
    // telekinesis checks the condition earlier
    dopickup = false;
    }
  else if(c2->item == itCompass) {
    dopickup = false;
    }
  else {
    bool lhu = hellUnlocked();
    if(c2->item) gainItem(c2->item);
    int g2 = gold();

    if(items[itFireShard] && items[itAirShard] && items[itWaterShard] && items[itEarthShard]) {
      items[itFireShard]--;
      items[itAirShard]--;
      items[itWaterShard]--;
      items[itEarthShard]--;
      gainItem(itElemental);
      gainItem(itElemental);
      gainItem(itElemental);
      gainItem(itElemental);
      addMessage(XLAT("You construct some Elemental Gems!", c2->item));
      }
        
    if(c2->item == itHyperstone && items[itHyperstone] == 10)
      achievement_victory(true);
    
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
  
  if(dopickup) c2->item = itNone;
//    if(c2->land == laHive)
//      c2->heat = 1;

  int numOrb = 0;
  for(int i=0; i<ittypes; i++)
    if(itemclass(eItem(i)) == IC_ORB && items[i])
      numOrb++;
  if(numOrb) achievement_count("ORB", numOrb, 0);

  return false;
  }

void dropGreenStone(cell *c) {
  if(!passable(c, NULL, true, false, false)) {
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
    else if(tooeasy)
      addMessage(XLAT("Come on, this is too easy... find a bigger castle!"));
    else
      addMessage(XLAT("The Knights wish you luck!"));
    }
  }

void knightFlavorMessage(cell *c2) {

  long long tab[100], sum[100];
  tab[0] = 1;
  tab[1] = 1*7;
  tab[2] = 2*7;
  tab[3] = 4*7;
  tab[4] = 7*7;
  for(int i=5; i<100; i++) 
    tab[i] = tab[i-1] + tab[i-2] + tab[i-3] - tab[i-4];
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
  else {
    msgid++; goto retry;
    }

  msgid++;
  }

void uncoverMines(cell *c, int lev) {
  if(c->wall == waMineUnknown)
    c->wall = waMineOpen;
  if(!lev) return;
  
  if(c->wall == waMineOpen) {
    bool minesNearby = false;
    for(int i=0; i<c->type; i++)
      if(c->mov[i] && c->mov[i]->wall == waMineMine)
        minesNearby = true;
    if(!minesNearby) for(int i=0; i<c->type; i++)
      if(c->mov[i] && (c->mov[i]->wall == waMineUnknown || c->mov[i]->wall == waMineOpen))
        uncoverMines(c->mov[i], lev-1);
    }
  }

void monstersTurn() {
  DEBT("bfs");
  bfs();
  destroyStrayMirrors();
  DEBT("heat");
  processheat();
  DEBT("rop");
  int phase1 = (1 & items[itOrbSpeed]);
  reduceOrbPowers();
  DEBT("mmo");
  int phase2 = (1 & items[itOrbSpeed]);
  if(!phase2) movemonsters();
  if(cwt.c->land == laPower && !phase1) { bfs(); moveFastMonsters(); }
  DEBT("lc");
  if(!phase1) livecaves();
  if(!phase1) dryforest();
  DEBT("check");
  checkmove();
  }

void pushThumper(cell *th, cell *cto) {
  eWall w = th->wall;
  cto->wparam = th->wparam;
  if(th->land == laAlchemist)
    th->wall = (cwt.c->wall == waFloorB || cwt.c->wall == waFloorA) ? cwt.c->wall : cto->wall;
  else th->wall = waNone;
  if(cto->wall == waOpenPlate || cto->wall == waClosePlate) {
    toggleGates(cto, cto->wall, 3);
    addMessage(XLAT("%The1 destroys %the2!", waThumperOn, cto->wall));
    }
  if(cellUnstable(cto) && cto->land == laMotion) {
    addMessage(XLAT("%The1 falls!", waThumperOn));
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
  if(isWatery(tgt) && !tgt->monst)
    return true;
  if(tgt->wall == waChasm && !tgt->monst)
    return true;
  return
    passable(tgt, thumper, false, true, false) &&
    passable(tgt, player, false, true, false) &&
    !tgt->item;
  }

void activateActiv(cell *c, bool msg) {
  if(msg) addMessage(XLAT("You activate %the1.", c->wall));
  if(c->wall == waThumperOff) c->wall = waThumperOn;
  if(c->wall == waBonfireOff) c->wall = waFire;
  c->wparam = 100;
  }

bool movepcto(int d, int subdir, bool checkonly) {
  if(hardcore && !canmove) return false;
  if(hardcore && checkonly) { printf("hardcore check\n"); return false; }
  if(checkonly && haveRangedTarget()) return true;
  if(!checkonly) flipplayer = false;
  if(!checkonly) DEB("movepc");
  if(d >= 0) {
    cwspin(cwt, d);
    spinmirrors(d);
    d = cwt.spin;
    }
  if(d != -1 && !checkonly) playermoved = true;
  if(!checkonly) invismove = false;  
  bool boatmove = false;
  if(d >= 0) {
    cell *c2 = cwt.c->mov[d];
    
    if(!player_passable(c2, cwt.c, false) && items[itOrbFlash]) {
      if(checkonly) return true;
      activateFlash();
      checkmove();
      return true;
      }

    if(!player_passable(c2, cwt.c, false) && items[itOrbLightning]) {
      if(checkonly) return true;
      activateLightning();
      checkmove();
      return true;
      }

    if(isActivable(c2)) {
      if(checkonly) return true;
      activateActiv(c2, true);
      checkmove();
      return true;
      }

    if(c2->wall == waThumperOn && !monstersnear(c2) && !c2->monst) {
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
      if(checkonly) return true;
      addMessage(XLAT("You push %the1.", c2->wall));
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

    if(isWatery(c2) && !monstersnear(c2) && !c2->monst && cwt.c->wall == waBoat) {

      if(againstWind(c2, cwt.c)) {
        if(!checkonly) 
          addMessage(XLAT("The Air Elemental blows you away!"));
        return false;
        }

      if(againstCurrent(c2, cwt.c) && !markOrb(itOrbWater)) {
        if(markOrb(itOrbFish)) goto escape;
        if(!checkonly)
          addMessage(XLAT("You cannot go against the current!"));
        return false;
        }
      if(checkonly) return true;
      moveBoat(c2, cwt.c);
      boatmove = true;
      }
    
    if(!monstersnear(c2) && !c2->monst && cwt.c->wall == waBoat && boatGoesThrough(c2) && markOrb(itOrbWater)) {
      if(checkonly) return true;
      placeWater(cwt.c, c2);
      c2->wall = waBoat;
      if(cwt.c->item) moveItem(cwt.c, c2, false), boatmove = true;
      }

    escape:
    if(c2->wall == waBigStatue && !monstersnear(c2) && !c2->monst) {
      if(!canPushStatueOn(cwt.c)) {
        if(checkonly) return false;
        if(isFire(cwt.c))
          addMessage(XLAT("You have to escape first!"));
        else
          addMessage(XLAT("There is not enough space!"));
        return false;
        }
          
      if(checkonly) return true;
      addMessage(XLAT("You push %the1 behind you!", c2->wall));
      c2->wall = cwt.c->wall;
      if(cellUnstable(cwt.c))
        cwt.c->wall = waChasm;
      else
        cwt.c->wall = waBigStatue;
      }

    if(c2->wall == waDryTree && !markOrb(itOrbGhost) && !monstersnear(cwt.c)) {
      if(checkNeedMove(checkonly, true)) return false;
      if(checkonly) return true;
      addMessage(XLAT("You start cutting down the tree."));
      gomirrors(0);
      c2->wall = waWetTree;
      }
    else if(c2->wall == waWetTree && !markOrb(itOrbGhost) && !monstersnear(cwt.c)) {
      if(checkNeedMove(checkonly, true)) return false;
      if(checkonly) return true;
      addMessage(XLAT("You cut down the tree."));
      gomirrors(0);
      c2->wall = waNone;
      }
    else if(c2->monst == moKnight) {
      if(checkonly) return false;
      if(!euclid && !c2->master->alt) {
        addMessage(XLAT("\"I am lost...\""));
        return false;
        }
      knightFlavorMessage(c2);
      return false;
      }
    else if(c2->monst && (!isFriendly(c2) || c2->monst == moTameBomberbird)) {
      if(c2->monst == moWorm || c2->monst == moWormtail || c2->monst == moWormwait) {
        if(checkonly) return false;
        addMessage(XLAT("You cannot attack Sandworms directly!"));
        return false;
        }
      
      if(c2->monst == moHexSnake || c2->monst == moHexSnakeTail) {
        if(checkonly) return false;
        addMessage(XLAT("You cannot attack Rock Snakes directly!"));
        return false;
        }
      
      if(attackingForbidden(c2, cwt.c)) {
        if(checkonly) return false;
        addMessage(XLAT("You cannot attack through the Vine!"));
        return false;
        }
      
      if(c2->monst == moTentacle || c2->monst == moTentacletail || c2->monst == moTentaclewait || c2->monst == moTentacleEscaping) {
        if(checkonly) return false;
        addMessage(XLAT("You cannot attack Tentacles directly!"));
        return false;
        }
      
/*    if(isBug(c2)) {
        if(checkonly) return false;
        addMessage(XLAT("You cannot win with %the1!", c2->monst));
        return false;
        }
*/
      if(c2->monst == moHedge && !markOrb(itOrbThorns)) {
        if(checkonly) return false;
        addMessage(XLAT("You cannot attack %the1 directly!", c2->monst));
        addMessage(XLAT("Stab them by walking around them."));
        return false;
        }
      
      if(c2->monst == moFlailer) {
        if(checkonly) return false;
        addMessage(XLAT("You cannot attack %the1 directly!", c2->monst));
        addMessage(XLAT("Make him hit himself by walking away from him."));
        return false;
        }
      
      if(c2->monst == moVizier && c2->hitpoints > 1) {
        if(checkonly) return false;
        addMessage(XLAT("You cannot attack %the1 directly!", c2->monst));
        addMessage(XLAT("Hit him by walking away from him."));
        return false;
        }
      
      if(c2->monst == moShadow) {
        if(checkonly) return false;
        addMessage(XLAT("You cannot defeat the Shadow!"));
        return false;
        }
      
      if(c2->monst == moGreater || c2->monst == moGreaterM) {
        if(checkonly) return false;
        addMessage(XLAT("You cannot defeat the Greater Demon yet!"));
        return false;
        }
      
      if(monstersnear(cwt.c, c2)) {
        if(checkonly) return false;
        addMessage(XLAT("You would be killed by %the1!", which));
        return false;
        }

      if(!(isWatery(cwt.c) && c2->monst == moWaterElemental) && checkNeedMove(checkonly, true))
        return false;
      
      if(checkonly) return true;
        
      if(isStunnable(c2->monst) && c2->hitpoints > 1) {
        addMessage(XLAT("You stun %the1.", c2->monst));
        stunMonster(c2);
        if(c2->monst != moFatGuard) {
          cellwalker push = cwt;
          cwstep(push);
          cwspin(push, 3 * -subdir);
          cwstep(push);
          if(c2->type == 7 && !blowable(push.c, c2)) {
            cwstep(push);
            cwspin(push, 1 * -subdir);
            cwstep(push);
            }
          if(blowable(push.c, c2))
            pushMonster(push.c, c2);
          }
        }
      else {
        messageKill(moNone, c2->monst);
        killWithMessage(c2);
        }
      
      gomirrors(0);
      }
    else if(!player_passable(c2, cwt.c, true)) {
      if(checkonly) return false;
      if(c2->wall == waFloorA || c2->wall == waFloorB)
        addMessage(XLAT("Wrong color!"));
      else if(c2->wall == waRoundTable)
        addMessage(XLAT("It would be impolite to land on the table!"));
      else if(cwt.c->wall == waRed3 && snakelevel(c2) == 0)
        addMessage(XLAT("You would get hurt!", c2->wall));
      else if(cellEdgeUnstable(cwt.c) && cellEdgeUnstable(c2))
        addMessage(XLAT("Gravity does not allow this!"));
      else if(againstWind(c2, cwt.c))
        addMessage(XLAT("The Air Elemental blows you away!"));
      else {
        addMessage(XLAT("You cannot move through %the1!", c2->wall));
        }
      return false;
      }
    else {
      if(c2->item == itOrbYendor && !boatmove && !checkYendor(c2, checkonly)) {
        return false;
        }
      if(c2->item == itHolyGrail) {
        if(roundTableRadius(c2) < newRoundTableRadius()) {
          if(checkonly) return false;
          addMessage(XLAT("That was not a challenge. Find a larger castle!"));
          return false;
          }
        }
      if(!hasSafeOrb(c2) && monstersnear(c2)) {
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

        addMessage(XLAT("%The1 would kill you there!", which));
        return false;
        }
      if(checkonly) return true;
      flipplayer = true;
      if(c2->item && c2->land == laAlchemist) c2->wall = cwt.c->wall;
      if(c2->wall == waRoundTable) {
        addMessage(XLAT("You jump over the table!"));
        }
      
      if(cwt.c->wall == waRoundTable) 
        roundTableMessage(c2);
      
      invismove = items[itOrbInvis] > 0;
      if(items[itOrbFire]) {
        invismove = false;
        if(makeflame(cwt.c, 10, false)) markOrb(itOrbFire);
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

      if(c2->monst == moGolem || c2->monst == moIllusion || isPrincess(c2->monst) || c2->monst == moMouse) {
        if(c2->monst == moMouse)
          princess::mouseSqueak(c2);
        else if(isPrincess(c2->monst)) {
          princess::line(c2);
          princess::move(cwt.c, c2);
          }
        else  
          addMessage(XLAT("You switch places with %the1.", c2->monst));
        cwt.c->hitpoints = c2->hitpoints;
        cwt.c->stuntime = c2->stuntime;
        placeGolem(cwt.c, c2, c2->monst);
        c2->monst = moNone;
        }
      else if(c2->monst) {
        addMessage(XLAT("You rejoin %the1.", c2->monst));
        killMonster(c2);
        }

      stabbingAttack(cwt.c, c2);
      cwstep(cwt);
      
      gomirrors(1);

      playerMoveEffects(c2);

      countLocalTreasure();
      landvisited[cwt.c->land] = true;
      setdist(cwt.c, 0, NULL);
      }
    }
  else {
    if(checkNeedMove(checkonly, false))
      return false;
    if(monstersnear(cwt.c)) {
      if(checkonly) return false;
      addMessage(XLAT("%The1 would get you!", which));
      return false;
      }
    if(checkonly) return true;
    if(d == -2) 
      dropGreenStone(cwt.c);
    }

  invisfish = false;
  if(items[itOrbFish]) {
     invisfish = true;
     for(int i=0; i<numplayers(); i++) 
       if(!isWatery(playerpos(i)))
         invisfish = false;
     if(invisfish) invismove = true, markOrb(itOrbFish);
     }
  monstersTurn();

  if(items[itWhirlpool] && cwt.c->land != laWhirlpool && !escaped) {
    escaped = true;
    achievement_gain("WHIRL1");
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

void teleportTo(cell *dest) {
  cwt.c->monst = dest->monst;
  dest->monst = moNone;
  movecost(cwt.c, dest);
  playerMoveEffects(dest);
  cwt.c = dest; cwt.spin = hrand(dest->type); flipplayer = !!(hrand(2));
  items[itOrbTeleport] = 0;

  addMessage(XLAT("You teleport to a new location!"));
  destroyMirrors();

  for(int i=9; i>=0; i--)
    setdist(cwt.c, i, NULL);

  bfs();
  
  if(shmup::on)
    shmup::teleported();
  else
    checkmove();
  }

void jumpTo(cell *dest) {
  movecost(cwt.c, dest);

  cwt.c = dest; 

  playerMoveEffects(dest);
  if(cwt.c->item != itOrbYendor && cwt.c->item != itHolyGrail)
    collectItem(cwt.c, true);

  items[itOrbFrog] -= 5;
  if(items[itOrbFrog] < 0) items[itOrbFrog] = 0;

  addMessage(XLAT("You jump!"));
  destroyMirrors();

  for(int i=9; i>=0; i--)
    setdist(cwt.c, i, NULL);

  if(shmup::on)
    shmup::teleported();
  else
    monstersTurn();
  }

void moveItem1(cell *from, cell *to, bool activateYendor) {
  if(from->item == itOrbYendor) {
    bool xnew = true;
    for(int i=0; i<size(yi); i++) 
      if(yi[i].path[0] == from) xnew = false;
    if(xnew && activateYendor) checkYendor(from, false);
    for(int i=0; i<size(yi); i++) 
      if(yi[i].path[0] == from) 
        yi[i].path[0] = to;
    }

  if(from->item == itKey) {
    for(int i=0; i<size(yi); i++) if(yi[i].path[YDIST-1] == from)
      yi[i].path[YDIST-1] = to;
    }    
  }

void moveItem (cell *from, cell *to, bool activateYendor) {
  moveItem1(from, to, activateYendor);
  moveItem1(to, from, activateYendor);
  eItem i = to->item;
  to->item = from->item;
  from->item = i;
  }

void telekinesis(cell *dest) {
    
  int cost = dest->cpdist * dest->cpdist;
  
  if(dest->land == laAlchemist && 
    (dest->wall == waFloorA || dest->wall == waFloorB) &&
    (cwt.c->wall == waFloorA || cwt.c->wall == waFloorB))
      dest->wall = cwt.c->wall;
    
  if(dest->land == laPower && cwt.c->land != laPower && dest->item != itOrbFire && dest->item != itOrbLife) {
    items[dest->item] += 2;
    dest->item = itNone;
    addMessage(XLAT("The Orb loses its power as it leaves the Land of Power!"));
    }

  moveItem(dest, cwt.c, true);
  collectItem(cwt.c, true);
  items[itOrbTelekinesis] -= cost;

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
    if(dest->land == laEdge) return moAirElemental;
    if(dest->land == laEAir) return moAirElemental;
    if(dest->land == laEWater) return moWaterElemental;
    if(dest->land == laEEarth) return moEarthElemental;
    if(dest->land == laEFire) return moFireElemental;
    if(dest->land == laMotion) return moRunDog;
    }
  return moNone;
  }

void summonAt(cell *dest) {
  dest->monst = summonedAt(dest);
  dest->stuntime = 3;
  if(dest->monst == moPirate || dest->monst == moViking)
    dest->wall = waBoat;
  if(dest->wall == waStrandedBoat)
    dest->wall = waBoat;
  else if(dest->monst == moWaterElemental)
    placeWater(dest, dest);
  if(dest->wall == waStone)
    dest->wall = waNone;
  if(dest->monst == moFireElemental && isFire(dest))
    dest->wall = waNone;
  addMessage(XLAT("You summon %the1!", dest->monst));
  moveEffect(dest, dest, dest->monst);

  if(hasHitpoints(dest->monst))
    dest->hitpoints = palaceHP();

  items[itOrbSummon] -= 20;
  if(items[itOrbSummon]<0) items[itOrbSummon] = 0;
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
  dest->wparam = (items[itOrbMatter]+1) / 2;
  items[itOrbMatter] /= 2;
  dest->item = itNone; // underwater items are destroyed by this
  checkmove();
  }

void psi_attack(cell *dest) {
  if(isNonliving(dest->monst))
    addMessage(XLAT("You destroy %the1 with a mental blast!", dest->monst));
  else
    addMessage(XLAT("You kill %the1 with a mental blast!", dest->monst));
  killWithMessage(dest, false);
  items[itOrbPsi] -= 30;
  if(items[itOrbPsi]<0) items[itOrbPsi] = 0;
  checkmove();
  }

void placeIllusion(cell *c) {
  c->monst = moIllusion;
  items[itOrbIllusion] -= 5;
  if(items[itOrbIllusion]<0) items[itOrbIllusion] = 0;
  addMessage(XLAT("You create an Illusion!"));
  checkmove();
  }

void blowoff(cell *cf, cell *ct) {
  addMessage(XLAT("You blow %the1 away!", cf->monst));
  pushMonster(ct, cf);
  items[itOrbAir]--;
  checkmove();
  }

void useOrbOfDragon(cell *c) {
  makeflame(c, 20, false);
  addMessage(XLAT("You throw fire!"));
  items[itOrbDragon] -= 5;
  if(items[itOrbDragon]<0) items[itOrbDragon] = 0;
  checkmove();
  }


eItem targetRangedOrb(cell *c, orbAction a) {

  if(false) if(a == roMouse || a == roMouseForce) {
    c->item = itShard;
    cell *c2 = c;
    cell *c3 = c;
    cell *c4 = chosenDown(c, 1, 1);
    cell *c5 = chosenDown(c, -1, -1);
    for(int i=0; i<3; i++) {
      printf("i=%d c2=%p c3=%p c4=%p c5=%p\n", i, c2, c3, c4, c5);
      c2->item = itDiamond;
      c3->item = itRuby;
      c4->item = itFjord;
      c5->item = itSapphire;
      buildEquidistant(c4); buildEquidistant(c5);
      c2 = chosenDown(c2,  1, 0);
      c3 = chosenDown(c3, -1, 0);
      c4 = chosenDown(c4,  1, 0);
      c5 = chosenDown(c5, -1, 0);
      if(c4->landflags == 3 && c4 == chosenDown(c2, 1, 1)) c->wall = waCIsland;
      if(c5->landflags == 3 && c5 == chosenDown(c3, -1, -1)) c->wall = waCIsland;
      }
    return itOrbDiscord;
    }

  if(!haveRangedOrb()) return itNone;

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
  
  // (0) telekinesis
  if(c->item && !itemHidden(c) && !cwt.c->item && items[itOrbTelekinesis] >= c->cpdist * c->cpdist && !cantGetGrimoire(c, a != roCheck)) {
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
      if(c2 && c2->cpdist > c->cpdist && blowable(c2, c)) {
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
    bool b = markOrb(itOrbGhost);
    if(i) items[itOrbGhost] = i-1;
    for(int i=0; i<cwt.c->type; i++) {
      cell *c2 = cwt.c->mov[i];
      if(isNeighbor(c2, c)) {
        jumpthru = c2;
        if((b || jumpable(c2, c)) && !thruVine(c2,cwt.c)) {
          jumpstate = 2;
          if(jumpable2(c, c2) && !thruVine(c2,c)) {
            jumpstate = 3;
            break;
            }
          }
        }
      }
    items[itOrbGhost] = i;
    if(jumpstate == 3 && !monstersnear(c)) {
      jumpstate = 4;
      if(a != roCheck) jumpTo(c);
      return itOrbFrog;
      }
    }
  
  // (1) switch with an illusion
  if(items[itOrbTeleport] && c->monst == moIllusion) {
    if(a != roCheck) teleportTo(c);
    return itOrbTeleport;
    }

  // (2) place illusion
  if(!shmup::on && items[itOrbIllusion] && c->monst == moNone && c->item == itNone && passable(c, NULL, false, true, false)) {
    if(a != roCheck) placeIllusion(c);
    return itOrbIllusion;
    }
  
  // (3) teleport
  if(items[itOrbTeleport] && c->monst == moNone && (c->item == itNone || itemHidden(c)) && 
    player_passable(c, NULL, false)) {
    if(a != roCheck) teleportTo(c);
    return itOrbTeleport;
    }
    
  // (4) remove an illusion
  if(!shmup::on && items[itOrbIllusion] && c->monst == moIllusion) {
    if(a != roCheck) {
      addMessage(XLAT("You take the Illusion away."));
      items[itOrbIllusion] += 4;
      c->monst = moNone;
      }
    return itOrbIllusion;
    }
  
  // (5) psi blast (non-shmup variant)
  if(!shmup::on && items[itOrbPsi] && c->monst && !isWorm(c) && c->monst != moShadow) {
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
  else if(items[itOrbIllusion] && !passable(c, NULL, false, true, false))
    addMessage(XLAT("Cannot cast illusion here!"));
  else if(items[itOrbTeleport] && !player_passable(c, NULL, false)) {
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

#define MAXBOX 180
#define POSSCORE 161 // update this when new boxes are added!

struct score {
  string ver;
  int box[MAXBOX];
  };

int savebox[MAXBOX], boxid;
bool saving, loading, loadingHi;

string boxname[MAXBOX];
bool fakebox[MAXBOX];

void applyBox(int& t) {
  if(saving) savebox[boxid++] = t;
  else if(loading) t = savebox[boxid++];
  else boxid++;
  }

void applyBoxNum(int& i, string name = "") {
  fakebox[boxid] = (name == "");
  boxname[boxid] = name;
  applyBox(i);
  }

void applyBoxBool(bool& b, string name = "") {
  int i = b;
  applyBoxNum(i, name);
  b = i;
  }

// just skips the value when loading
void applyBoxSave(int i, string name = "") {
  fakebox[boxid] = (name == "");
  boxname[boxid] = name;
  applyBox(i);
  }

int applyBoxLoad(string name = "") {
  fakebox[boxid] = (name == "");
  boxname[boxid] = name;
  int i=0; applyBox(i);
  return i;
  }

void applyBoxI(eItem it, bool f = false) {
  boxname[boxid] = iinf[it].name;
  fakebox[boxid] = f;
  if(loadingHi) { 
    updateHi(it, savebox[boxid++]); 
    }
  else applyBox(items[it]);
  }

void applyBoxM(eMonster m, bool f = false) {
  fakebox[boxid] = f;
  boxname[boxid] = minf[m].name;
  applyBox(kills[m]);
  }

void applyBoxes() {

  applyBoxSave(timerstart, "time elapsed");
  time_t timer = time(NULL);
  applyBoxSave(timer, "date");
  applyBoxSave(gold(), "treasure collected");
  applyBoxSave(tkills(), "total kills");
  applyBoxNum(turncount, "turn count");
  applyBoxNum(cellcount, "cells generated");

  if(!saving) timerstart = time(NULL);
  
  for(int i=0; i<itOrbLightning; i++) 
    if(i == 0) items[i] = 0, applyBoxI(itFernFlower);
    else applyBoxI(eItem(i));
  
  for(int i=0; i<43; i++) {
    if(loading) kills[i] = 0;
    bool fake = 
      i == moLesserM || i == moNone || i == moWolfMoved || i == moTentacletail;
    if(i == moWormtail) applyBoxM(moCrystalSage);
    else if(i == moWormwait) applyBoxM(moFireFairy);
    else if(i == moTentacleEscaping) applyBoxM(moMiner);
    else if(i == moGolemMoved) applyBoxM(moIllusion);
    else if(i == moTentaclewait) applyBoxI(itOrbThorns, true);
    else if(i == moGreater) applyBoxI(itOrbDragon, true);
    else if(i == moGreaterM) applyBoxI(itOrbIllusion, true);
    else applyBoxM(eMonster(i), fake);
    }
    
  if(saving) {
    applyBoxSave(savetime + timer - timerstart);
    }
  else if(loading) savetime = applyBoxLoad();
  else boxid++;
  
  savecount++; applyBoxNum(savecount, "number of saves"); savecount--;
  applyBoxNum(cheater);

  if(saving) applyBoxSave(cwt.c->land);
  else if(loading) firstland = eLand(applyBoxLoad());
  else boxid++;
  
  for(int i=itOrbLightning; i<25; i++) applyBoxI(eItem(i), true);
  
  applyBoxI(itRoyalJelly);
  applyBoxI(itWine);
  applyBoxI(itSilver);
  applyBoxI(itEmerald);
  applyBoxI(itPower);
  applyBoxI(itOrbFire, true);
  applyBoxI(itOrbInvis, true);
  applyBoxI(itOrbGhost, true);
  applyBoxI(itOrbPsi, true);
  applyBoxM(moBug0);
  applyBoxM(moBug1);
  applyBoxM(moBug2);
  applyBoxM(moVineBeast);
  applyBoxM(moVineSpirit);
  applyBoxM(moLancer);
  applyBoxM(moFlailer);
  applyBoxM(moEarthElemental);
  applyBoxM(moDarkTroll);
  applyBoxM(moWitch);
  applyBoxM(moWitchFire);
  applyBoxM(moWitchFlash);
  applyBoxM(moWitchGhost);
  applyBoxM(moWitchSpeed);
  applyBoxM(moEvilGolem);
  applyBoxM(moWitchWinter);
  applyBoxI(itHolyGrail);
  applyBoxI(itGrimoire);
  applyBoxM(moKnight);
  applyBoxM(moCultistLeader);
  
  applyBoxM(moPirate);
  applyBoxM(moCShark);
  applyBoxM(moParrot);
  applyBoxI(itPirate);
  applyBoxI(itOrbPreserve, true);
  
  applyBoxM(moHexSnake);
  applyBoxM(moRedTroll);
  applyBoxI(itRedGem);
  applyBoxI(itOrbTelekinesis, true);
  
  applyBoxBool(euclid);
  applyBoxBool(hardcore);
  applyBoxNum(hardcoreAt);
  applyBoxBool(shmup::on);
  if(saving) applyBoxSave(euclidland);
  else if(loading) euclidland = eLand(applyBoxLoad());
  else boxid++;
  
  applyBoxI(itCoast);
  applyBoxI(itWhirlpool);
  applyBoxI(itBombEgg);
  applyBoxM(moBomberbird);
  applyBoxM(moTameBomberbird);
  applyBoxM(moAlbatross);
  applyBoxI(itOrbFriend, true);
  applyBoxI(itOrbAir, true);
  applyBoxI(itOrbWater, true);
  
  applyBoxI(itPalace);
  applyBoxI(itFjord);
  applyBoxI(itOrbFrog);
  applyBoxI(itOrbDiscord);
  applyBoxM(moPalace);
  applyBoxM(moFatGuard);
  applyBoxM(moSkeleton);
  applyBoxM(moVizier);
  applyBoxM(moViking);
  applyBoxM(moFjordTroll);
  applyBoxM(moWaterElemental);
  
  applyBoxI(itSavedPrincess);
  applyBoxI(itOrbLove);
  applyBoxM(moPrincess);
  applyBoxM(moPrincessMoved); // live Princess for Safety
  applyBoxM(moPrincessArmedMoved); // live Princess for Safety
  applyBoxM(moMouse);
  applyBoxNum(princess::saveArmedHP);
  applyBoxNum(princess::saveHP);
  
  applyBoxI(itEdge);
  applyBoxI(itElemental);
  applyBoxI(itZebra);
  applyBoxI(itFireShard);
  applyBoxI(itWaterShard);
  applyBoxI(itAirShard);
  applyBoxI(itEarthShard);
  
  applyBoxM(moAirElemental);
  applyBoxM(moFireElemental);
  applyBoxM(moEdgeMonkey);
  applyBoxM(moGargoyle);
  applyBoxM(moOrangeDog);
  applyBoxI(itOrbSummon);
  applyBoxI(itOrbMatter);
  }

void saveBox() {
  boxid = 0; saving = true; applyBoxes(); saving = false;
  }

void loadBox() {
  // have boxid
  boxid = 0; loading = true; applyBoxes(); loading = false;
  }

void loadBoxHigh() {
  // have boxid
  boxid = 0; loadingHi = true; applyBoxes(); loadingHi = false;
  }

// certify that saves and achievements were received
// in an official version of HyperRogue
#ifdef CERTIFY
#include "certify.cpp"
#else
bool tampered;

void saveCertificate(FILE *f) {}
bool loadCertificate(FILE *f, score& sc) {return true; }

int achievement_certify(const char *s, int a, int b, int c) { return 0; }

#endif

void saveStats() {
#ifndef ANDROID

  if(randomPatternsMode) return;

  FILE *f = fopen(scorefile, "at");
  if(!f) {
    printf("Could not open the score file '%s'!\n", scorefile);
    addMessage(XLAT("Could not open the score file: ", scorefile));
    return;
    }

  if(showoff) return;
  
  time_t timer;
  timer = time(NULL);
  char sbuf[128]; strftime(sbuf, 128, "%c", localtime(&timerstart));
  char buf[128]; strftime(buf, 128, "%c", localtime(&timer));
  
  fprintf(f, "HyperRogue: game statistics (version "VER")\n");
  if(cheater)
    fprintf(f, "CHEATER! (cheated %d times)\n", cheater);
  if(true) {

    fprintf(f, VER);
    if(!shmup::on) items[itOrbLife] = countMyGolems(moGolem); 
    if(!shmup::on) items[itOrbFriend] = countMyGolems(moTameBomberbird); 
    if(!shmup::on) kills[moPrincessMoved] = countMyGolems(moPrincess); 
    if(!shmup::on) kills[moPrincessArmedMoved] = countMyGolems(moPrincessArmed); 
    if(!shmup::on) princess::saveHP = countMyGolemsHP(moPrincess); 
    if(!shmup::on) princess::saveArmedHP = countMyGolemsHP(moPrincessArmed); 
    saveBox();
    
    for(int i=0; i<boxid; i++) fprintf(f, " %d", savebox[i]);
    saveCertificate(f);

    fprintf(f, "\n");
    }
  fprintf(f, "Played on: %s - %s (%d %s)\n", sbuf, buf, turncount, 
    shmup::on ? "knives" : "turns");
  fprintf(f, "Total wealth: %d\n", gold());
  fprintf(f, "Total enemies killed: %d\n", tkills());
  fprintf(f, "cells generated: %d\n", cellcount);
  fprintf(f, "Number of cells explored, by distance from the player:\n"); 
  for(int i=0; i<10; i++) fprintf(f, " %d", explore[i]); fprintf(f, "\n");
/*for(int j=0; j<landtypes; j++) {
    bool haveland = false;
    for(int i=0; i<10; i++) 
      if(exploreland[i][j]) 
        haveland = true;
    if(haveland)
      for(int i=0; i<10; i++) 
        fprintf(f, " %d", exploreland[i][j]);
    fprintf(f, " %s\n", linf[j].name);
    } */
  if(kills[0]) fprintf(f, "walls melted: %d\n", kills[0]);
  fprintf(f, "cells travelled: %d\n", celldist(cwt.c));
  
  fprintf(f, "\n");

  for(int i=0; i<ittypes; i++) if(items[i])  
    fprintf(f, "%4dx %s\n", items[i], iinf[i].name);
    
  fprintf(f, "\n");
  
  for(int i=1; i<motypes; i++) if(kills[i])  
    fprintf(f, "%4dx %s <%d>\n", kills[i], minf[i].name, i);
  
  fprintf(f, "\n\n\n");
  
  printf("Game statistics saved to %s\n", scorefile);
  addMessage(XLAT("Game statistics saved to %1", scorefile));
  fclose(f);
#endif
  }

bool havesave = true;

#ifndef ANDROID
// load the save
void loadsave() {

  printf("Trying to load a save.\n");
  FILE *f = fopen(scorefile, "rt");
  havesave = f;
  if(!f) return;
  score sc;
  bool ok = false;
  bool tamper = false;
  while(!feof(f)) {
    char buf[120];
    if(fgets(buf, 120, f) == NULL) break;
    if(buf[0] == 'H' && buf[1] == 'y') {
      if(fscanf(f, "%s", buf) <= 0) break; sc.ver = buf;
      if(sc.ver < "4.4" || sc.ver == "CHEATER!") continue;
      ok = true;
      for(int i=0; i<MAXBOX; i++) {
        if(fscanf(f, "%d", &sc.box[i]) <= 0) {
          boxid = i;
          
          tamper = loadCertificate(f, sc);

          for(int i=0; i<boxid; i++) savebox[i] = sc.box[i];
          for(int i=boxid; i<MAXBOX; i++) savebox[i] = 0;

          // Euclidean and Shmup highscores do not count for hiitems
          if(savebox[116] || savebox[119]) break;
          
          loadBoxHigh();

          break;
          }
        }
      }
    if(buf[0] == 'A' && buf[1] == 'C' && buf[2] == 'H') {
      char buf1[80], buf2[80];
      sscanf(buf, "%70s%70s", buf1, buf2);
      if(buf2 == string("PRINCESS1")) princess::everSaved = true;
      if(buf2 == string("YENDOR2")) princess::everGotYendorVictory = true;
      }
    }
  fclose(f);
  if(ok && sc.box[65 + 4 + itOrbSafety - itOrbLightning]) {
    tampered = tamper;
//  printf("box = %d (%d)\n", sc.box[65 + 4 + itOrbSafety - itOrbLightning], boxid);
    for(int i=0; i<boxid; i++) savebox[i] = sc.box[i];
    for(int i=boxid; i<MAXBOX; i++) savebox[i] = 0;
    loadBox();
    if(items[itHolyGrail]) {
      items[itHolyGrail]--;
      knighted = newRoundTableRadius();
      items[itHolyGrail]++;
      }
    else knighted = 0;
    safety = true;
    addMessage(XLAT("Game loaded."));
    }
  }
#endif

void restartGame(char switchWhat = 0) {
  DEB("savestats");
  achievement_final(true);
  saveStats();
  DEB("clear");
  for(int i=0; i<ittypes; i++) items[i] = 0;
  for(int i=0; i<motypes; i++) kills[i] = 0;
  for(int i=0; i<10; i++) explore[i] = 0;
  for(int i=0; i<10; i++) for(int l=0; l<landtypes; l++)
    exploreland[i][l] = 0;
  tampered = false; achievementsReceived.clear();
  princess::saved = false;
  princess::forceVizier = false;
  princess::forceMouse = false;
  knighted = 0;
  // items[itGreenStone] = 100;
  cellcount = 0;
  DEB("clearmem");
  clearMemory();
  if(switchWhat == 'e') euclid = !euclid;
  if(switchWhat == 's') shmup::on = !shmup::on;
  if(switchWhat == 'p') {
    princess::challenge = !princess::challenge;
    firstland = princess::challenge ? laPalace : laIce;
    if(shmup::on) shmup::on = false;
    }
  DEB("initc");
  initcells();
  DEB("initg");

  if(randomPatternsMode) {
    for(int i=0; i<landtypes; i++)
      randompattern[i] = rand();
    if(randomPatternsMode) firstland = pickLandRPM(laNone);
    clearMemoRPM();
    }

  initgame();
  canmove = true;
  DEB("restg");
  restartGraph();
  resetmusic();
  resetmusic();
  }

void clearGameMemory() {
  pathq.clear();
  dcal.clear();
  yii = 0; yi.clear();
  clearshadow();
  offscreen.clear();  
  princess::clear();
  seenSevenMines = false;
  }

bool emeraldwalled[64];

static int orbid = 0;

eItem nextOrb() {
  orbid++;
  eItem i = eItem(orbid % ittypes);
  if(itemclass(i) == IC_ORB) return i;
  else return nextOrb();
  }

eItem randomTreasure() {
  eItem i = eItem(hrand(ittypes));
  if(itemclass(i) == IC_TREASURE) return i;
  else return randomTreasure();
  }

eItem randomTreasure2(int cv) {
  int bq = 60000, cq = 0;
  eItem best = itDiamond;
  eItem lt = localTreasureType();
  for(int a=1; a<ittypes; a++) {
    eItem i = eItem(a);
    if(itemclass(i) != IC_TREASURE) continue;
    int q = 2*items[i];
    if(a == lt) q -= (2*cv-1);
    if(a == itEmerald && cwt.c->land == laCrossroads) q -= 5;
    if(q < bq) bq = q, cq = 0;
    if(q == bq) { cq++; if(hrand(cq) == 0) best = i; }
    }
  return best;
  }

bool applyCheat(char u, cell *c = NULL) {
  if(u == 'M' && cwt.c->type == 6) {
    addMessage(XLAT("You summon some Mirages!"));
    cheater++;
    createMirrors(cwt.c, cwt.spin, moMirage),
    createMirages(cwt.c, cwt.spin, moMirage);
    return true;
    }
  if(u == 'G') {
    addMessage(XLAT("You summon a golem!"));
    cheater++;
    int i = cwt.spin;
    if(passable(cwt.c->mov[i], NULL, false, false, false)) 
      cwt.c->mov[i]->monst = hrand(2) ? moGolem : moTameBomberbird;
    return true;
    }
  if(u == 'L') {
    do {
      if(firstland == eLand(landtypes-1))
        firstland = eLand(2);
      else
        firstland = eLand(firstland+1);
      }
    while(firstland == laCanvas || firstland == laCamelot || firstland == laTemple ||
      firstland == laWhirlpool || firstland == laOceanWall);
    euclidland = firstland;
    cheater++; addMessage(XLAT("You will now start your games in %1", firstland));
    return true;
    }
  if(u == 'C') {
    cheater++; 
    activateSafety(laCrossroads);
    addMessage(XLAT("Activated the Hyperstone Quest!"));
    for(int i=0; i<itHyperstone; i++) items[i] = 10;
    items[itFernFlower] = 10;
    items[itHyperstone] = 0;
    items[itOrbShield] = 10;
    kills[moYeti] = 20;
    kills[moDesertman] = 20;
    kills[moRunDog] = 20;
    kills[moZombie] = 20;
    kills[moMonkey] = 20;
    kills[moCultist] = 20;
    kills[moTroll] = 20;
    return true;
    }
  if(u == 'P') {
    for(int i=0; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_ORB) 
        items[i] = 0;
    cheater++; addMessage(XLAT("Orb power depleted!"));
    return true;
    }
  if(u == 'O') {
    cheater++; addMessage(XLAT("Orbs summoned!"));
    for(int i=0; i<cwt.c->type; i++) 
      if(passable(cwt.c->mov[i], NULL, false, false, false)) {
        eItem it = nextOrb();
        cwt.c->mov[i]->item = it;
        }
    return true;
    }
  if(u == 'F') {
    items[itOrbFlash] += 1;
    items[itOrbTeleport] += 1;
    items[itOrbLightning] += 1;
    items[itOrbSpeed] += 1;
    items[itOrbShield] += 1;
    cheater++; addMessage(XLAT("Orb power gained!"));
    return true;
    }
  if(u == 'D') {
    items[itGreenStone] += 10;
    cheater++; addMessage(XLAT("Dead orbs gained!"));
    return true;
    }
  if(u == 'D'-64) {
    mapeditor::drawplayer = !mapeditor::drawplayer;
    return true;
    }
  if(u == 'A') {
    cmode = emMapEditor;
    return true;
    }
  if(u == 'A'-64) {
    cmode = emDraw;
    return true;
    }
  if(u == 'Y') {
    items[itOrbYendor] ++;
    cheater++; addMessage(XLAT("Orb of Yendor gained!"));
    return true;
    }
  if(u == 'T') {
    items[randomTreasure2(10)] += 10;
    cheater++; addMessage(XLAT("Treasure gained!"));
    return true;
    }
  if(u == 'T'-64) {
    items[randomTreasure2(100)] += 100;
    cheater++; addMessage(XLAT("Lots of treasure gained!"));
    return true;
    }
  if(u == 'I'-64) {
    items[randomTreasure2(10)] += 25;
    cheater++; addMessage(XLAT("Treasure gained!"));
    return true;
    }
  if(u == 'U'-64) {
    items[randomTreasure2(10)] += 50;
    cheater++; addMessage(XLAT("Treasure gained!"));
    return true;
    }
  if(u == 'W') {
    addMessage(XLAT("You summon a sandworm!"));
    cheater++;
    int i = cwt.spin;
    if(passable(cwt.c->mov[i], NULL, false, false, false))
      cwt.c->mov[i]->monst = moWorm,
      cwt.c->mov[i]->mondir = NODIR;
    return true;
    }
  if(u == 'I') {
    addMessage(XLAT("You summon an Ivy!"));
    cheater++;
    int i = cwt.spin;
    int j = cwt.c->spn[i];
    cell* c = cwt.c->mov[i]->mov[(j+3)%cwt.c->mov[i]->type];
    if(passable(c, NULL, false, false, false)) buildIvy(c, 0, 1);
    return true;
    }
  if(u == 'E') {
    addMessage(XLAT("You summon a monster!"));
    cheater++;
    int i = cwt.spin;
    if(cwt.c->mov[i]->wall == waChasm)
      cwt.c->mov[i]->wall = waNone;
    if(passable(cwt.c->mov[i], NULL, true, false, false)) {
      eMonster mo[9] = { moEagle, moPyroCultist, moGhost, moTroll, moMiner, moVineBeast, moBug0,
        moBomberbird, moSkeleton };
      cwt.c->mov[i]->monst = mo[hrand(9)];
      cwt.c->mov[i]->stuntime = 3;
      cwt.c->mov[i]->hitpoints = 3;
      }
    return true;
    }
  if(u == 'E'-64) {
    addMessage(XLAT("You summon many monsters!"));
    cheater++;
    for(int i=0; i<cwt.c->type; i++) {
      cell *c2 = cwt.c->mov[i];
      if(passable(c2, NULL, true, false, false)) {
        eMonster mo[2] = { moRedTroll, moDarkTroll };
        c2->monst = mo[hrand(2)];
        }
      }
    return true;
    }
  if(u == 'H') {
    addMessage(XLAT("You summon some Thumpers!"));
    cheater++;
    for(int i=0; i<cwt.c->type; i++) 
      if(passable(cwt.c->mov[i], NULL, false, false, false)) {
        eWall ws[3] = { waThumperOff, waBigStatue, waBoat };
        cwt.c->mov[i]->wall = ws[hrand(3)];
        }
    return true;
    }
  if(u == 'B') {
    addMessage(XLAT("You summon a bonfire!"));
    cheater++;
    int i = cwt.spin;
    if(passable(cwt.c->mov[i], NULL, false, false, false)) 
      cwt.c->mov[i]->wall = waBonfireOff;
    return true;
    }
  if(u == 'Z') {
    cwt.spin++; flipplayer = false;
    cwt.spin %= cwt.c->type;
    return true;
    }
  if(u == 'J') {
    if(items[localTreasureType()] > 0)
      items[localTreasureType()] = 0;
    else for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_TREASURE) 
        items[i] = 0;
    cheater++; addMessage(XLAT("Treasure lost!"));
    return true;
    }
  if(u == 'K') {
    for(int i=0; i<motypes; i++) kills[i] += 10;
    cheater++; addMessage(XLAT("Kills gained!"));
    return true;
    }
  if(u == 'S') {
    activateSafety(cwt.c->land);
    items[itOrbSafety] += 3;
    cheater++; addMessage(XLAT("Activated Orb of Safety!"));
    return true;
    }
  if(u == 'U') {
    activateSafety(firstland);
    cheater++; addMessage(XLAT("Teleported to %1!", firstland));
    return true;
    }
  if(u == 'W'-64) {
    webdisplay++;
    cheater++; addMessage(XLAT("Cheat-changed the display.", firstland));
    return true;
    }
  if(u == 'F'-64) {
    items[itOrbShield] += 30;
    return true;
    }
  if(u == 'Y'-64) {
    int i = cwt.spin;
    cwt.c->mov[i]->item = itOrbYendor;
    return true;
    }
  return false;
  }

void generateAlts(heptagon *h) {
  if(!h->alt) return;
  h->c7->bardir = NOBARRIERS;
  for(int i=0; i<7; i++) if(h->c7->mov[i])
    h->c7->mov[i]->bardir = NOBARRIERS;
  for(int i=0; i<7; i++) 
    createStep(h->alt, i)->alt = h->alt->alt;
  int relspin = -4; // for horocycles it must go the other way
  for(int i=0; i<7; i++) for(int j=0; j<7; j++) {
    createStep(h, i);
    if(h->move[i]->alt == h->alt->move[j])
      relspin = (i-j+7) % 7;
    }
  if(relspin == -4) {
    if(h->alt != h->alt->alt) {
      printf("relspin {%p:%p}\n", h->alt, h->alt->alt);
      for(int i=0; i<7; i++) printf("%p ", h->alt->move[i]); printf(" ALT\n");
      for(int i=0; i<7; i++) printf("%p ", h->move[i]); printf(" REAL\n");
      for(int i=0; i<7; i++) printf("%p ", h->move[i]->alt); printf(" REAL ALT\n");
      }
    relspin = 3;
    }
  // h[relspin] matches alt[0]
//printf("{%d~%d}\n", h->distance, h->alt->distance);
  for(int i=0; i<7; i++) {
    int ir = (7+i-relspin)%7;
    heptagon *hm = h->alt->move[ir];
    heptagon *ho = createStep(h, i);
//  printf("[%p:%d ~ %p:%d] %p ~ %p\n", 
//    h, i, h->alt, ir, 
//    ho, hm);
    if(ho->alt && ho->alt != hm) {
      if(ho->alt->alt == hm->alt) {
        printf("ERROR: alt cross! [%d -> %d]\n", ho->alt->distance, hm->distance);
        // exit(1);
        }
      continue;
      }
    ho->alt = hm;
    }
  }

heptagon *createAlternateMap(cell *c, int rad, hstate firststate, int special) {

  // check for direction
  int gdir = -1;
  for(int i=0; i<c->type; i++) {
    if(c->mov[i] && c->mov[i]->mpdist < c->mpdist) gdir = i;
    }
  if(gdir < 0) return NULL;
  
  // check for non-crossing
  int bd = 2;
  cellwalker bb(c, bd);
  if(!(checkBarriersFront(bb) && checkBarriersBack(bb))) {
    return NULL;
    }

  // okay, let's go then!
  cellwalker bf(c, gdir);
  cell *cx[rad+1];
  for(int i=0; i<rad; i++) {
    cx[i] = bf.c;
    if(bf.c->type == 6)
      cwspin(bf, 3);
    else
      cwspin(bf, 3 + hrand(2));
    cwstep(bf);
    }
  cx[rad] = bf.c;
  heptagon *h = bf.c->master;
  
  if(special == waPalace) {  
  
    // type 7 is ensured
    cell *c = bf.c;
    
    if(cdist50(c) != 0) return NULL;
    if(polarb50(c) != 1) return NULL;
    }
  
  heptagon *alt = new heptagon;
  allAlts.push_back(alt);
//printf("new alt {%p}\n", alt);
  alt->s = firststate;
  alt->emeraldval = 0;
  alt->zebraval = 0;
  for(int i=0; i<7; i++) alt->move[i] = NULL;
  alt->distance = 0;
  alt->c7 = NULL;
  alt->alt = alt;
  h->alt = alt;
  
  for(int d=rad; d>=0; d--) {
    generateAlts(cx[d]->master);  
    cx[d]->bardir = NOBARRIERS;
    }

  if(special == waPalace) {

    cell *c = bf.c;

    princess::generating = true;
    c->land = laPalace;
    for(int j=BARLEV; j>=0; j--)
      setdist(c, j, NULL);
    princess::generating = false;
    
    princess::newInfo(c);
    princess::forceMouse = false;  

    // static bool debug=true; if(debug) debug=false, cwt.c = c;
    }
  
  return alt;
//for(int d=rad; d>=0; d--) printf("%3d. %p {%d}\n", d, cx[d]->master, cx[d]->master->alt->distance);
  }

