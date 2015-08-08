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

int sagephase = 0;

// number of Grails collected, to show you as a knight
int knighted = 0;

bool showoff = false;
bool safety = false;
int showid = 0;

bool lifebrought = false; // was Life brought to the Dead Caves?
bool invismove = false; // last move was invisible

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
int items[ittypes], kills[motypes], explore[10], exploreland[10][landtypes], landcount[landtypes];
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

vector<cell*> pathqm; // list of monsters to move (pathq restriced to monsters)

vector<cell*> targets; // list of monster targets

// monsters of specific types to move
vector<cell*> worms, ivies, ghosts, golems, mirrors, mirrors2; 

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

bool isIcyLand(cell *c) {
  return c->land == laIce || c->land == laCocytus;
  }

void initcell(cell *c) {
  c->mpdist = INFD;   // minimum distance from the player, ever
  c->cpdist = INFD;   // current distance from the player
  c->pathdist = INFD; // current distance from the player, along paths (used by yetis)
  c->heat = 0;
  c->wall  = waNone;
  c->item  = itNone;
  c->monst = moNone;
  c->bardir = NODIR;
  c->land = laNone;
  c->tmp = -1;
  c->ligon = 0;
  lastexplore = turncount;
  }

// 0 = basic treasure, 1 = something else, 2 = power orb
#define IC_TREASURE 0
#define IC_OTHER 1
#define IC_ORB 2
int itemclass(eItem i) {
  if(i == 0) return -1;
  if(i < itKey || i == itFernFlower || 
    i == itWine || i == itSilver || i == itEmerald || i == itRoyalJelly || i == itPower ||
    i == itGrimoire)
    return IC_TREASURE;
  if(i == itKey || i == itOrbYendor || i == itGreenStone || i == itHolyGrail) 
    return IC_OTHER;
  return IC_ORB;
  }
  
int puregold() {
  int i = items[itOrbYendor] * 50 + items[itHolyGrail] * 10;
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
    
    case laNone: return itNone;
    case laBarrier: return itNone;
    case laGameBoard: return itNone;
    
    case laFjord: return itEmerald;
    case laWineyard: return itWine;
    case laHive: return itRoyalJelly;
    case laDeadCaves: return itSilver;
    case laPower: return itPower;
    case laCamelot: return itHolyGrail;
    case laTemple: return itGrimoire;
    }
  return itNone;
  }

eItem localTreasureType() {
  lastland = cwt.c->land;
  return treasureType(lastland);
  }

void countLocalTreasure() {
  eItem i = localTreasureType();
  currentLocalTreasure = i ? items[i] : 0;
  }

bool hellUnlocked() {
  int i = 0;
  for(int t=0; t<ittypes; t++) 
    if(itemclass(eItem(t)) == IC_TREASURE && items[t] >= 10)
      i++;
  return i >= 9;
  }

bool hyperstonesUnlocked() {
  for(int t=1; t<ittypes; t++) 
    if(t != itHyperstone && itemclass(eItem(t)) == IC_TREASURE && items[t] < 10) {
      return false;
      }
  return true;
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
    kills[moCultistLeader];
  }

bool passable(cell *w, cell *from, bool monster_passable, bool mirror_passable) {
  if(w->wall == waFloorA && from && from->wall == waFloorB && !w->item && !from->item)
    return false;
  if(w->wall == waFloorB && from && from->wall == waFloorA && !w->item && !from->item)
    return false;
  if(w->wall == waMirror || w->wall == waCloud) return mirror_passable;
  if((w->wall == waVineHalfA || w->wall == waVineHalfB) && from && from->wall == w->wall)
    return false;
  if(w->wall == waNone || w->wall == waFloorA || w->wall == waFloorB || 
    w->wall == waCavefloor || w->wall == waFrozenLake || w->wall == waVineHalfA ||
    w->wall == waVineHalfB || w->wall == waDeadfloor || w->wall == waDeadfloor2) {
    if(w->monst) return monster_passable;
    return true;
    }
  return false;
  }

bool cellUnstable(cell *c) {
  return c->land == laMotion && c->wall == waNone;
  }

bool cellUnstableOrChasm(cell *c) {
  return c->land == laMotion && (c->wall == waNone || c->wall == waChasm);
  }

bool cellHalfvine(cell *c) {
  return c->wall == waVineHalfA || c->wall == waVineHalfB;
  }

bool thruVine(cell *c, cell *c2) {
  return cellHalfvine(c) && c2->wall == c->wall;
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

bool isFire(cell *w) {
  return w->wall == waBonfire || w->wall == waPartialFire;
  }

// eagles can go through lakes, chasms, and slime
// todo vines?
bool eaglepassable(cell *w) {
  if(w->monst) return false;
  return
    w->wall == waNone || w->wall == waFloorA || w->wall == waFloorB || 
    w->wall == waCavefloor || w->wall == waFrozenLake || w->wall == waLake ||
    w->wall == waDeadfloor || w->wall == waDeadfloor2 || w->wall == waCamelotMoat ||
    w->wall == waSulphur || w->wall == waSulphurC || w->wall == waChasm;
  }

bool isActiv(cell *c) {
  return c->wall == waThumper || c->wall == waBonfire || c->wall == waPartialFire;
  }

bool isMimic(eMonster m) {
  return m == moMirror || m == moMirage;
  }

bool isMimic(cell *c) { 
  return isMimic(c->monst);
  }

bool isGolemOrKnight(cell *c) {
  return 
    c->monst == moGolem || c->monst == moGolemMoved || 
    c->monst == moKnight ||c->monst == moKnightMoved;
  }

bool isFriendly(cell *c) {
  return isMimic(c->monst) || isGolemOrKnight(c) || c->monst == moIllusion;
  }

bool isBug(eMonster m) {
  return m >= moBug0 && m < moBug0+BUGCOLORS;
  }

bool isBug(cell *c) {
  return isBug(c->monst);
  }

bool isFriendlyOrBug(cell *c) {
  return isFriendly(c) || isBug(c);
  }

bool isIvy(cell *c) {
  return c->monst == moIvyRoot || c->monst == moIvyHead || c->monst == moIvyBranch || c->monst == moIvyWait ||
    c->monst == moIvyNext || c->monst == moIvyDead;
  }

bool isDemon(cell *c) {
  return c->monst == moLesser || c->monst == moLesserM || 
    c->monst == moGreater || c->monst == moGreaterM;
  }

bool isWorm(cell *c) {
  return c->monst == moWorm || c->monst == moWormtail || c->monst == moWormwait || 
    c->monst == moTentacle || c->monst == moTentacletail || c->monst == moTentaclewait ||
    c->monst == moTentacleEscaping;
  }

void useup(cell *c) {
  c->tmp--;
  if(c->tmp == 0) c->wall = waNone;
  }

bool isInactiveEnemy(cell *w) {
  if(w->monst == moWormtail || w->monst == moWormwait || w->monst == moTentacletail || w->monst == moTentaclewait)
    return true;
  if(w->monst == moLesserM || w->monst == moGreaterM)
    return true;
  if(w->monst == moIvyRoot || w->monst == moIvyWait || w->monst == moIvyNext || w->monst == moIvyDead)
    return true;
  return false;
  }

bool isActiveEnemy(cell *w, cell *killed) {
  if(w->monst == moNone || w == killed) return false;
  if(isFriendly(w)) return false;
  if(isInactiveEnemy(w)) return false;
  if(w->monst == moIvyHead || w->monst == moIvyBranch) {
    while(w != killed && w->mondir != NODIR) w = w->mov[w->mondir];
    return w != killed;
    }
  return true;
  }

bool isArmedEnemy(cell *w, cell *killed) {
  return w->monst != moCrystalSage && isActiveEnemy(w, killed);
  }

bool player_passable(cell *w, cell *from, bool mon) {
  if(w->monst && !isFriendly(w)) return false;
  if(isFire(w) && items[itOrbWinter]) return true;
  if(w->wall == waRoundTable && from->wall != waRoundTable)
    return true;
  if(items[itOrbGhost] > 1) return true;
  return passable(w, from, mon, true);
  }

bool isHive(eLand l) {
  return l == laHive;
  }
  
bool isKillable(cell *c) {
  return c->monst != moShadow && !isWorm(c) && c->monst != moGreater && c->monst != moGreaterM
    && c->monst != moHedge && c->monst != moFlailer;
    // && !isBug(c->monst);
  }

bool isKillableSomehow(cell *c) {
  return isKillable(c) 
    || c->monst == moHedge || c->monst == moLancer || c->monst == moFlailer;
  }

bool isNeighbor(cell *c1, cell *c2) {
  for(int i=0; i<c1->type; i++) if(c1->mov[i] == c2) return true;
  return false;
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
        bool b = isFriendly(c3) || isBug(c3);
        if(friendly ? b: !b) return true;
        }
      }
    }
  return false;
  }

int monstersnear(cell *c, cell *nocount = NULL, bool shielded = true) {
  int res = 0;
  bool fast = false;
  if(shielded) {
    if(items[itOrbShield] > 1) return 0;
    fast = (items[itOrbSpeed]  && !(items[itOrbSpeed] & 1));
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
        if(!passable(c2, c3, !isArmedEnemy(c2, nocount), false)) continue;
        if(c2 == cwt.c && items[itOrbFire]) continue;
        }
      // flashwitches cannot attack if it would kill another enemy
      if(c3->monst == moWitchFlash && flashWouldKill(c3, false)) continue;
      if(nocount && mirrorkill(c3)) continue;
      res++, which = c3->monst;
      } 

    // consider normal monsters
    if(c2 && isArmedEnemy(c2, nocount) && c2->monst != moLancer) {
      if(fast && c2->monst != moWitchSpeed) continue;
      // they cannot attack through vines
      if(attackingForbidden(c, c2))
        continue;
      // do not count if it would be killed by a mimic
      if(nocount && mirrorkill(c2)) continue;
      // do not include stabbed enemies
      if(
        (c2->monst == moHedge || (isKillable(c2) && items[itOrbThorns]))
        && c2->cpdist == 1 && c != cwt.c)
        continue;
      res++, which = c2->monst;
      }
    }

  return res;
  }
        
// reduce c->mpdist to d; also generate the landscape

bool checkBarriersBack(cellwalker& bb, int q=5);

bool checkBarriersFront(cellwalker& bb, int q=5) {
  if(bb.c->mpdist < BARLEV) return false;
  if(bb.c->mpdist == BUGLEV) return false;
  if(bb.c->bardir != NODIR) return false;
  if(bb.spin == 0) {q--; if(!q) return true; }

  if(1) for(int i=0; i<7; i++) {
    cellwalker bb2 = bb;
    cwspin(bb2, i); cwstep(bb2); 
    if(bb2.c->bardir != NODIR) return false;
    cwspin(bb2, 4); cwstep(bb2);
    if(bb2.c->bardir != NODIR) return false;
    }

  cwstep(bb); cwspin(bb, 3); cwstep(bb); cwspin(bb, 3); cwstep(bb);
  return checkBarriersBack(bb, q);
  }

bool checkBarriersBack(cellwalker& bb, int q) {
  // printf("back, %p, s%d\n", bb.c, bb.spin);
  if(bb.c->mpdist < BARLEV) return false;
  if(bb.c->mpdist == BUGLEV) return false;
  if(bb.c->bardir != NODIR) return false;
  // if(bb.spin == 0 && bb.c->mpdist == INFD) return true;
  
  if(1) for(int i=0; i<7; i++) {
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

void setbarrier(cell *c) {
  c->wall = waBarrier;
  c->land = laBarrier;
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
    if(passable(c->mov[i], c, false, false)) {
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

bool destroyHalfvine(cell *c, eWall newwall = waNone, int tval = 6);

void killMonster(cell *c);

void prespill(cell* c, eWall t, int rad) {
  // these monsters block spilling
  if(c->monst == moSeep || c->monst == moVineSpirit || c->monst == moShark ||
    c->monst == moGreaterShark)
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
    c->wall == waDune || c->wall == waAncientGrave || c->wall == waThumper ||
    c->wall == waFreshGrave || c->wall == waColumn || c->wall == waPartialFire ||
    c->wall == waDeadwall || c->wall == waWaxWall || c->wall == waCamelot || c->wall == waRoundTable ||
    c->wall == waBigStatue)
    return;
  // these walls block further spilling
  if(c->wall == waCavewall || cellUnstable(c) || c->wall == waSulphur ||
    c->wall == waSulphurC || c->wall == waLake || c->wall == waChasm ||
    c->wall == waDryTree || c->wall == waWetTree || c->wall == waTemporary ||
    c->wall == waVinePlant || c->wall == waBonfire || c->wall == waVineHalfA || c->wall == waVineHalfB ||
    c->wall == waCamelotMoat)
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
    }  
  }

void ivynext(cell *c);

void earthFloor(cell *c) {
  if(c->monst) return;
  if(c->wall == waDeadwall) c->wall = waDeadfloor;
  if(c->wall == waDune) c->wall = waNone;
  if(c->wall == waAncientGrave || c->wall == waFreshGrave) c->wall = waNone;
  }

void earthWall(cell *c) {
  if(c->wall == waDeadfloor || c->wall == waDeadfloor2 || c->wall == waEarthD) {
    c->item = itNone;
    c->wall = waDeadwall;
    }
  if(c->wall == waNone && c->land == laDesert) {
    c->item = itNone;
    c->wall = waDune;
    }
  }

bool isWitch(eMonster m) {
  // evil golems don't count
  return m >= moWitch && m < moWitch+NUMWITCH-1;
  }

void killMonster(cell *c) {
  DEB("killmonster");
  eMonster m = c->monst;
  
  if(!m) return;
  if(isWorm(c)) return;
  if(m == moShadow) return;
  if(m == moGolemMoved) m = moGolem;
  if(m == moKnightMoved) m = moKnight;
  if(m == moSlimeNextTurn) m = moSlime;
  if(m == moLesserM) m = moLesser;
  if(m == moGreater) m = moLesser;
  if(m == moGreaterM) m = moLesser;
  kills[m]++;
  
  if(m == moTroll) {
    destroyHalfvine(c);
    c->wall = cellUnstableOrChasm(c) ? waChasm : waDeadTroll;
    c->item = itNone;
    for(int i=0; i<c->type; i++) if(c->mov[i]) {
      c->mov[i]->item = itNone;
      if(c->mov[i]->wall == waDeadwall || c->mov[i]->wall == waDeadfloor2) c->mov[i]->wall = waCavewall;
      if(c->mov[i]->wall == waDeadfloor) c->mov[i]->wall = waCavefloor;
      }
    }
  if(m == moMiner) {
    destroyHalfvine(c);
    c->wall = cellUnstableOrChasm(c) ? waChasm : waNone;
    for(int i=0; i<c->type; i++) if(passable(c->mov[i], c, true, true)) {
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
  if(m == moVineSpirit) {
    destroyHalfvine(c);
    c->wall = waNone;
    }
  if(isWitch(m) && (c->wall == waBonfire || passable(c, NULL, true, false)) && !c->item) {
    if(m == moWitchFire) c->item = itOrbFire;
    if(m == moWitchFlash) c->item = itOrbFlash;
    if(m == moWitchGhost) c->item = itOrbGhost;
    if(m == moWitchWinter) c->item = itOrbWinter;
    if(m == moWitchSpeed) c->item = itOrbSpeed;
    if(c->wall == waBonfire && itemBurns(c->item))
      c->item = itNone;
    }
  if(m == moFireFairy) {
    c->wall = cellUnstableOrChasm(c) ? waChasm : waBonfire;
    c->item = itNone, c->tmp = 50;
    }
  if(m == moPyroCultist && c->item == itNone && c->wall != waChasm) {
    // a reward for killing him before he shoots!
    c->item = itOrbDragon;
    }
  if(m == moSlime) { c->monst = moNone; spill(c, c->wall, 2); }
  // if(c->monst == moShark) c->heat += 1;
  // if(c->monst == moGreaterShark) c->heat += 10;
  if(isIcyLand(c)) {
    if(m == moCultist) c->heat += 3;
    if(m == moPyroCultist) c->heat += 6;
    if(m == moLesser) c->heat += 10;
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
  else c->monst = moNone;

  if(m == moEarthElemental) earthWall(c);
  }

void burnMonstersAndItems(cell *c, int val, eWall firetype = waBonfire) {
  if(itemBurns(c->item)) 
    addMessage(XLAT("%The1 burns!", c->item)), c->item = itNone;
  if(c->monst && c->monst != moGhost && !isWorm(c) && c->monst != moShadow) {
    addMessage(XLAT("%The1 burns!", c->monst));
    killMonster(c);
    }
  c->tmp = val;
  c->wall = firetype;
  }

void flameHalfvine(cell *c, int val) {
  burnMonstersAndItems(c, val);
  c->wall = waPartialFire;
  }

bool destroyHalfvine(cell *c, eWall newwall, int tval) {
  if(cellHalfvine(c)) {
    for(int t=0; t<c->type; t++) if(c->mov[t]->wall == c->wall) {
      if(newwall == waPartialFire) burnMonstersAndItems(c->mov[t], tval, newwall);
      else c->mov[t]->wall = newwall;
      }
    if(newwall == waPartialFire) burnMonstersAndItems(c, tval, newwall);
    else c->wall = newwall;
    return true;
    }
  return false;
  }

bool orbChance(cell *c, eLand usual, int chthere, int chcross) {
  if(c->land == usual) return chthere && rand() % chthere == 0;
  if(chcross && c->land == laCrossroads) {
    chcross = (chcross / 50) * (50 + items[itHyperstone]);
    return rand() % chcross == 0;
    }
  return false;
  }

void buildBarrier(cell *c) {
  // printf("build barrier at %p", c);
  if(c->wall == waBarrier) { 
    // printf("-> ready\n");
    return;
    }
//  if(c->wall == waWaxWall) return;
  if(c->mpdist > BARLEV) {
    // printf("-> too far\n");
    return; // == INFD) return;
    }

  cellwalker bb(c, c->bardir); setbarrier(bb.c);
  cwstep(bb); 
  bb.c->barleft = c->barright;
  bb.c->barright = c->barleft;
  setbarrier(bb.c);
  cwspin(bb, 2); cwstep(bb); bb.c->land = c->barleft; cwstep(bb);
  cwspin(bb, 2); cwstep(bb); bb.c->land = c->barright; cwstep(bb);
  cwspin(bb, 2); 
  
  cwspin(bb, 3); cwstep(bb); 
  bb.c->barleft = c->barright;
  bb.c->barright = c->barleft;
  setbarrier(bb.c);
  cwspin(bb, 3); cwstep(bb);
  
  bb.c->bardir = bb.spin;
  bb.c->barleft = c->barright;
  bb.c->barright = c->barleft;
  // printf("#1\n");
  buildBarrier(bb.c);
  
  for(int a=-3; a<=3; a++) if(a) {
    bb.c = c; bb.spin = c->bardir; cwspin(bb, a); cwstep(bb); 
    bb.c->land = a > 0 ? c->barright : c->barleft;
    }

  bb.c = c; bb.spin = c->bardir;
  cwspin(bb, 3); cwstep(bb); cwspin(bb, 4); bb.c->land = c->barright; cwstep(bb); cwspin(bb, 3);
  bb.c->bardir = bb.spin;
  bb.c->barleft = c->barright;
  bb.c->barright = c->barleft;
  // printf("#2\n");
  buildBarrier(bb.c);
  }

void chasmify(cell *c) {
  c->wall = waChasm; c->item = itNone;
  int q = 0;
  cell *c2[10];
  for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->mpdist > c->mpdist && cellUnstable(c->mov[i]))
    c2[q++] = c->mov[i];
  if(q) {
    cell *c3 = c2[rand() % q];
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
    int d = d2[rand() % q];
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

eLand getNewLand(eLand old) {
  eLand tab[32];
  int cnt = 0;
  
/*   if(isHive(old) && rand() % 100 < 90) {
    eLand n = old;
    while(n == old) n = eLand(laHive0 + rand() % 3);
    return n;
    } */
  
  // return (rand() % 2) ? laMotion : laJungle;

  // the basic lands, always available
  tab[cnt++] = laCrossroads;
  tab[cnt++] = laIce;
  tab[cnt++] = laDesert;
  tab[cnt++] = laJungle;
  if(old != laDeadCaves) tab[cnt++] = laCaves;
  
  // the advanced lands
  if(gold() >= 30) {
    tab[cnt++] = laCrossroads;
    tab[cnt++] = laAlchemist;
    tab[cnt++] = laMirror;
    tab[cnt++] = laMotion;
    }
  
  if(gold() >= 60) {
    tab[cnt++] = laCrossroads;
    tab[cnt++] = laRlyeh;
    if(old != laDeadCaves && items[itGold] >= 5 && items[itFernFlower] >= 5) tab[cnt++] = laFjord;
    if(old != laPower) tab[cnt++] = laDryForest;
    if(old != laPower) tab[cnt++] = laWineyard;
    if(old != laCaves && old != laFjord && items[itGold] >= 10) tab[cnt++] = laDeadCaves;
    }
  
  if(tkills() >= 100) {
    tab[cnt++] = laGraveyard;
    if(gold() >= 60) tab[cnt++] = laHive;
    }
  
  if(hellUnlocked()) {
    tab[cnt++] = laCrossroads;
    tab[cnt++] = laHell;
    }
  
  if(items[itHell] >= 10) {
    tab[cnt++] = laCocytus;
    if(old != laWineyard && old != laDryForest) tab[cnt++] = laPower;
    }
  
  eLand n = old;
  while(n == old) n = tab[rand() % cnt];
  
  return n;  
  }

bool notDippingFor(eItem i) {
  int v = items[i] - currentLocalTreasure;
  if(v <= 10) return true;
  if(v >= 20) return false;
  return v >= rand() % 10 + 10;
  }

bool notDippingForExtra(eItem i, eItem x) {
  int v = items[i] - min(items[x], currentLocalTreasure);
  if(v <= 10) return true;
  if(v >= 20) return false;
  return v >= rand() % 10 + 10;
  }

eLand euland[65536];

eLand switchable(eLand nearland, eLand farland, eucoord c) {
  if(nearland == laCrossroads) {
    if(rand() % 4 == 0 && (short(c)%3==0))
      return laBarrier;
    return laCrossroads;
    }
  else if(nearland == laBarrier) {
    return getNewLand(farland);
    }
  else {
    if(rand() % 20 == 0 && (short(c)%3==0))
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

void createBugArmy(cell *c);

int newRoundTableRadius() {
  return 28 + 2 * items[itHolyGrail];
  }

int roundTableRadius(cell *c) {
  if(euclid) return 28;
  return c->master->alt->alt->fjordval & GRAIL_RADIUS_MASK;
  }

bool grailWasFound(cell *c) {
  if(euclid) return items[itHolyGrail];
  return c->master->alt->alt->fjordval & GRAIL_FOUND;
  }

int euclidAlt(short x, short y) {
  if(euclidland == laTemple) {
    return max(int(x), x+y);
    }
  else return eudist(x-20, y-10);
  }

int celldistAltRelative(cell *c) {
  return celldistAlt(c) - roundTableRadius(c);
  }

heptagon *createAlternateMap(cell *c, int rad, hstate firststate);

void generateAlts(heptagon *h);

void buildCamelotWall(cell *c) {
  c->wall = waCamelot;
  for(int i=0; i<c->type; i++) {
    cell *c2 = createMov(c, i);
    if(c2->wall == waNone && celldistAlt(c2) > celldistAlt(c) && c2->monst == moNone)
      c2->wall = waCamelotMoat;
    }
  }

// This function generates all lands. Warning: it's very long!
void setdist(cell *c, int d, cell *from) {
  DEB("setdist");
  if(signed(c->mpdist) <= d) return;
  c->mpdist = d;
  
  if(d >= BARLEV) {
    if(!c->land) c->land = from->land;
    if(c->land == laTemple) c->land = laRlyeh;
    if(c->land == laCamelot) c->land = laCrossroads;

    if(euclid) {
      c->land = euclidland;
      if(euclidland == laCrossroads) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        c->land = getEuclidLand(y+2*x);
        }
      }
    }
  
  if(d == BARLEV && !euclid) {
    
    if(c->type == 7 && rand() % 10000 < (
      showoff ? (cwt.c->mpdist > 7 ? 0 : 10000) : 
      c->land == laCrossroads ? 5000 : 50) &&
      c->land != laGameBoard) {
    
      int bd = 2 + (rand() % 2) * 3;
      
      cellwalker bb(c, bd);
      cellwalker bb2 = bb;

      if(checkBarriersFront(bb) && checkBarriersBack(bb2)) {
        c->bardir = bd;
        eLand oldland = c->land;
        eLand newland = getNewLand(oldland);
        if(showoff) newland = showlist[(showid++) % 10];
        landcount[newland]++;

        if(bd == 5) c->barleft = oldland, c->barright = newland;
        else c->barleft = newland, c->barright = oldland;
        }
      }
        
    if(c->land == laCrossroads && c->type == 7 && rand() % 2000 < 200 && items[itEmerald] >= 5) {
      int rtr = newRoundTableRadius();
      heptagon *alt = createAlternateMap(c, rtr+14, hsOrigin);
      if(alt) {
        alt->fjordval = rtr;
        }
      }

    if(c->land == laRlyeh && c->type == 7 && rand() % 2000 < 100 && items[itStatue] >= 5)
      createAlternateMap(c, 2, hsA);

    if(c->bardir != NODIR && c->bardir != NOBARRIERS) 
      buildBarrier(c);
    }
  
  if(d < 10) {
    explore[d]++;
    exploreland[d][c->land]++;
    
    if(d < BARLEV)
    for(int i=0; i<c->type; i++) {
      createMov(c, i);
      setdist(c->mov[i], d+1, c);
      }
    
    if(d==8 && c->land == laFjord) {
      if(euclid) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        if(((y-2)&7) < 4) c->wall = waCavewall;
        else c->wall = waCavefloor;
        }
      else {
        int v = fjordval(c);
        if((v&3) >= 2) 
          c->wall = waCavewall;
        else c->wall = waCavefloor;
        }
      }
    
    if(d==8 && c->land == laPower) {
      int v;
      if(euclid) {
        eucoord x, y;
        decodeMaster(c->master, x, y);
        int y0 = ((short)y) % 6;
        if(y0<0) y0+=6;
        if(y0 == 3 || y0 == 4) v=24; else v=0;
        }
      else v = fjordval(c);
      v &= ~3;
      if((v == 24 || v == 32 || v == 56))
        c->wall = waBonfire, c->tmp = 9;
      else if(rand() % 100 < 10) {
        c->wall = waGlass;
        eItem protectedItems[17] = {
          itPower, itPower, itPower, itPower, itPower, itPower,
          itOrbLightning, itOrbLightning, itOrbThorns, itOrbThorns,
          itOrbInvis, itOrbInvis,
          itOrbShield, itOrbTeleport, itOrbPsi,
          itOrbDragon, itOrbIllusion
          };
        c->item = protectedItems[rand() % 17];
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
        int v = fjordval(c);
        int w = v / 4;
        if(w == 9 || w == 10 || w == 7 || w == 8) {
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
        
      if(c->land == laIce) if(rand() % 100 < 5 && c->wall != waBarrier) {
        c->wall = waIcewall;
        for(int i=0; i<c->type; i++) if(rand() % 100 < 50) {
          createMov(c, i);
          setdist(c->mov[i], d+1, c);
          cell *c2 = c->mov[i];
          if(c2->wall == waBarrier || c2->land != laIce) continue;
          c2->wall = waIcewall;
          for(int j=0; j<c2->type; j++) if(rand() % 100 < 20) {
            createMov(c2, j);
            setdist(c->mov[i], d+2, c);
            cell *c3 = c2->mov[j];
            if(c3->wall == waBarrier || c3->land != laIce) continue;
            c3->wall = waIcewall;
            }
          }
        }

      if(c->land == laIce || c->land == laCocytus) if(c->wall == waIcewall && items[itDiamond] >= 5 && rand() % 200 == 1)
        c->wall = waBonfire, c->tmp = -1;;

      if(c->land == laCaves) 
        c->wall = rand() % 100 < 55 ? waCavewall : waCavefloor;
      
      if(c->land == laDeadCaves) {
        int i = rand() % 100;
        if(i<50) c->wall = waDeadwall;
        else if(i<55) c->wall = waDeadfloor2;
        else c->wall = waDeadfloor;
        }
      
      if(c->land == laAlchemist) 
        c->wall = rand() % 2 ? waFloorA : waFloorB;
      
      if(c->land == laDryForest) 
        c->wall = rand() % 100 < 50 ? (rand() % 100 < 50 ? waDryTree : waWetTree) : 
          waNone;
      
      if(c->land == laGraveyard && ishept(c)) 
        c->wall = rand() % 5 ? waAncientGrave : waFreshGrave;
      
      if(c->land == laRlyeh)  {
        if(rand() % 500 < 5) {
          for(int i=0; i<c->type; i++) {
            createMov(c, i);
            setdist(c->mov[i], d+1, c);
            if(c->mov[i] && c->mov[i]->land == laRlyeh)
              c->mov[i]->wall = waColumn;
            }
          
          for(int j=0; j<2; j++) {
            int i = rand() % c->type;
            if(c->mov[i] && c->mov[i]->land == laRlyeh)
              c->mov[i]->wall = waNone;
            }
          }
        if(ishept(c) && rand() % 2) c->wall = waColumn;
        }
      
      if(c->land == laHell) {
        if(rand() % 100 < 4) {
          for(int i=0; i<c->type; i++) {
            createMov(c, i);
            setdist(c->mov[i], d+1, c);
            if(c->mov[i] && c->mov[i]->land == laHell)
              if(c->mov[i]->wall != waSulphurC)
                c->mov[i]->wall = waSulphur;
            }
          
          c->wall = waSulphurC;
          }
        }
      
      if(c->land == laCocytus)  {
        if(c->wall == waNone) c->wall = waFrozenLake;
        if(rand() % 100 < 5) {
          for(int i=0; i<c->type; i++) {
            createMov(c, i);
            setdist(c->mov[i], d+1, c);
            if(c->mov[i] && c->mov[i]->land == laCocytus)
              c->mov[i]->wall = waLake;
            }
          
          c->wall = waLake;

          if(rand() % 500 < 100 + 2 * (items[itSapphire]))
            c->monst = moShark;
          }
        }

      if(isHive(c->land) && rand() % 2000 < 2) 
        createBugArmy(c);
      
      if((c->land == laCrossroads  && !euclid) || c->land == laCamelot) 
      if(euclid || c->master->alt) {
        int d = celldistAltRelative(c);
        {
          eucoord x,y;
          decodeMaster(c->master, x, y);
          }
        if(d <= 14) {
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
          if(d < 0 && rand() % 7000 <= 10 + items[itHolyGrail] * 5) {
            eMonster m[3] = { moHedge, moLancer, moFlailer };
            c->monst = m[rand() % 3];
            }
          if(d == 1) {
            // roughly as many knights as table cells
            if(rand() % 1720 < 1000) 
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
      
      if(isHive(c->land) && rand() % 2000 < 100 && !c->wall && !c->item && !c->monst) {
        int nww = 0;
        for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->wall == waWaxWall)
          nww++;
        if(nww == 0) {
          c->wall = waWaxWall;
          c->monst = moNone;
          c->heat = rand() & 0xFFFFFF;
          }
        /* for(int i=0; i<c->type; i++) {
          if(rand() % 6 < 5) {
            createMov(c,i);
            cell *c2 = c->mov[i];
            c2->wall = waWaxWall;
            c2->monst = moNone;
            }
          } */
        }
      
      if(c->land == laDesert) {
        if(rand() % 100 < 5) {
          for(int i=0; i<c->type; i++) {
            createMov(c, i);
            setdist(c->mov[i], d+1, c);
            if(c->mov[i] && c->mov[i]->land == laDesert)
              c->mov[i]->wall = waDune;
            }
          
          for(int j=0; j<2; j++) {
            int i = rand() % c->type;
            if(c->mov[i] && c->mov[i]->land == laDesert)
              c->mov[i]->wall = waNone;
            }
          }

        if(rand() % 300 == 1 && items[itSpice] >= 5) 
          c->wall = waThumper, c->tmp = -1;
        }
      }
    
    if(d == 7 && c->land == laCaves && c->wall == waCavewall && rand() % 5000 < items[itGold])
      c->monst = moSeep;
    
    if(d == 7 && c->land == laFjord && c->wall == waCavewall && rand() % 5000 < items[itEmerald])
      c->monst = moSeep;
    
    if(d == 7 && c->land == laDeadCaves && c->wall == waDeadwall && rand() % 1000 < items[itSilver])
      c->monst = moSeep;
    
    if(d == 7 && c->wall == waVinePlant && rand() % 100 < 10)
      c->monst = moVineSpirit;
    
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
    
    if(d == 7 && c->wall == waIcewall && c->land != laIce && c->land != laCocytus)
      c->wall = waNone;
    
    if(d == 7 && c->wall == waChasmD) {
      chasmify(c);
      }
      
    if(d == 7 && c->wall == waEarthD) {
      chasmifyEarth(c);
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
        if(c2->wall != waBonfire && c2->wall != waGlass) {
          if(c->wall == waBonfire) c->monst = moWitchWinter;
          else if(c->wall == waGlass) c->monst = moWitchGhost;
          else c->monst = moEvilGolem;
          }
        }
      }

    if(d == 7 && passable(c, NULL, false, false)) {
      if(c->land == laBarrier) c->wall = waBarrier;
      }

    if(d == 7 && passable(c, NULL, false, false) && !safety) {
      int hard = items[itOrbYendor] * 5;
      
      if(c->land == laIce) {
        if(rand() % 5000 < 100 + 2 * (kills[moYeti] + kills[moWolf]) && notDippingFor(itDiamond))
          c->item = itDiamond;
        if(rand() % 8000 < 2 * (items[itDiamond] + hard))
          c->monst = rand() % 2 ? moYeti : moWolf;
        }
      if(c->land == laCaves) {
        if(rand() % 5000 < 100 + 2 * (kills[moTroll] + kills[moGoblin]) && notDippingFor(itGold))
          c->item = itGold;
        if(rand() % 8000 < 10 + 2 * (items[itGold] + hard))
          c->monst = rand() % 2 ? moTroll : moGoblin;
        }
      if(c->land == laDeadCaves) {
        if(rand() % 5000 < 100 + 2 * (kills[moDarkTroll] + kills[moEarthElemental]) && notDippingFor(itSilver))
          c->item = itSilver;
        if(rand() % 16000 < (items[itSilver] + hard)) {
          c->monst = moEarthElemental;
          for(int i=0; i<c->type; i++) {
            cell *c2 = c->mov[i];
            earthFloor(c2);
            }
          for(int i=0; i<c->type; i++) if(c->mov[i]->mpdist < c->mpdist) c->mondir = i;
          chasmifyEarth(c); c->wall = waDeadfloor2;
          }
        else if(rand() % 8000 < 60 + 8 * (items[itSilver] + hard)) {
          if(rand() % 100 < 25) {
            }
          else c->monst = rand() % 2 ? moDarkTroll : moGoblin;
          }
        }
      if(c->land == laDesert) {
        if(rand() % 5000 < 100 + 2 * (kills[moWorm] + kills[moDesertman]) && notDippingFor(itSpice))
          c->item = itSpice;
        if(rand() % 8000 < 10 + 2 * (items[itSpice] + hard))
          c->monst = rand() % 2 ? moWorm : moDesertman,
          c->mondir = NODIR;
        }
      if(c->land == laWineyard) {
        if(rand() % 5000 < 100 + 2 * (kills[moVineBeast] + kills[moVineSpirit]) && notDippingFor(itWine))
          c->item = itWine;
        if(rand() % 8000 < 12 * (items[itWine] + hard))
          c->monst = moVineBeast;
        }
      if(c->land == laFjord) {
        if(rand() % 1000 < 100 + 2 * (kills[moMiner] + kills[moLancer] + kills[moFlailer]) && notDippingFor(itEmerald)) {
          // do not destroy walls!
          bool ok = true;
          for(int i=0; i<c->type; i++) if(c->mov[i]->wall == waCavewall) ok = false;
          if(ok) c->item = itEmerald;
          }
        if(rand() % 8000 < 50 + 10 * (items[itEmerald] + hard)) {
          static eMonster fjordmonsters[4] = { moHedge, moLancer, moFlailer, moMiner };
          c->monst = fjordmonsters[rand() % 4];
          }
        }
      if(c->land == laJungle) {
        if(rand() % 5000 < 25 + 2 * (kills[moIvyRoot] + kills[moMonkey]) && notDippingFor(itRuby))
          c->item = itRuby;
        if(rand() % 15000 < 5 + 1 * (items[itRuby] + hard))
          c->monst = moMonkey;
        else if(rand() % 80000 < 5 + items[itRuby] + hard)
          c->monst = moEagle;
        else if(ishept(c) && rand() % 4000 < 300 + items[itRuby]) {
          bool hard = rand() % 100 < 25;
          if(hard ? buildIvy(c, 1, 9) : buildIvy(c, 0, c->type))
            c->item = itRuby;
          }
        }
      if(c->land == laAlchemist) {
        if(rand() % 5000 < 25 + min(kills[moSlime], 200) && notDippingFor(itElixir))
          c->item = itElixir;
        else if(rand() % 3500 < 10 + items[itElixir] + hard)
          c->monst = moSlime;
        }
      if(c->land == laPower) {
        if(rand() % (5000+50*items[itPower]) < 1200) {
          eItem powerorbs[6] = {
            itOrbFlash, itOrbSpeed, itOrbFire, itOrbWinter, itOrbGhost, itOrbLife};
          c->item = powerorbs[rand() % 6];
          }
        else if(c->type == 6 && rand() % 5000 < 10) 
          c->wall = rand() % 2 ? waMirror : waCloud;
        else if(rand() % 1000 < 10 + (items[itPower] ? 10:0) + (items[itPower] + hard))
          c->monst = eMonster(moWitch + rand() % NUMWITCH);
        }
      if(c->land == laCrossroads) {
        if(c->type == 6 && rand() % 8000 < 120 && items[itShard] >= 10)
          c->wall = rand() % 2 ? waMirror : waCloud;
        else if(hyperstonesUnlocked() && rand() % 5000 < tkills() && notDippingFor(itHyperstone))
          c->item = itHyperstone;
        else if(rand() % 4000 < items[itHyperstone] + 2 * items[itHolyGrail]) {
          // only interesting monsters here!
          static eMonster m[14] = {
            moWorm, moTroll, moEagle,
            moLesser, moGreater, moPyroCultist, moGhost,
            moFireFairy, moIvyRoot, moTentacle, moHedge,
            moLancer, moFlailer, moVineBeast
            };
          eMonster cm = m[rand() % 14];
          if(cm == moIvyRoot) buildIvy(c, 0, c->type);
          else c->monst = cm;
          if(cm == moWorm || cm == moTentacle)
            c->mondir = NODIR;
          }
        }
      if(c->land == laMirror) {
        if(c->type == 6 && rand() % 5000 < 120 && notDippingFor(itShard))
          c->wall = rand() % 2 ? waMirror : waCloud;
        if(rand() % 12000 < 8 + items[itShard] + hard)
          c->monst = moRanger;
        else if(rand() % 60000 < 8 + items[itShard] + hard)
          c->monst = moEagle;
        }
      if(c->land == laGraveyard) {
        if(rand() % 5000 < 30 + 2 * (kills[moZombie] + kills[moGhost] + kills[moNecromancer]) && notDippingFor(itBone))
          c->item = itBone;
        if(rand() % 20000 < 10 + items[itBone] + hard + (kills[moZombie] + kills[moGhost] + kills[moNecromancer])/60) {
          eMonster grm[6] = { moZombie, moZombie, moZombie, moGhost, moGhost, moNecromancer};
          c->monst = grm[rand() % 6];
          }
        }
      if(c->land == laRlyeh) {
        if(rand() % 5000 < 30 + 2 * (kills[moCultist] + kills[moTentacle] + kills[moPyroCultist]) && notDippingFor(itStatue))
          c->item = itStatue;
        if(rand() % 8000 < 5 + items[itStatue] + hard)
          c->monst = moTentacle, c->item = itStatue, c->mondir = NODIR;
        else if(rand() % 12000 < 5 + items[itStatue] + hard)
          c->monst = rand() % 3 ? ((rand() % 40 < items[itStatue]-25) ? moCultistLeader : moCultist) : moPyroCultist;
        else if(rand() % 8000 < 5 + items[itStatue] + hard && c->type == 6) {
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
        int d = celldistAlt(c);
        // remember: d is negative
        if(d % TEMPLE_EACH == 0) {
          if(rand() % 5000 < 20 - 2*d)
            c->monst = moTentacle, c->mondir = NODIR;
          }
        else {
          int d0 = d % TEMPLE_EACH;
          if(d0<0) d0=-d0;
          if(rand() % 100 < 30) //  && d0 != 1 && d0 != TEMPLE_EACH-1)
            c->wall = waBigStatue;
          else if(rand() % 20000 < -d)
            c->monst = rand() % 3 ? moCultist : moPyroCultist;
          else if(rand() % 100000 < -d)
            c->monst = moCultistLeader;
          else if(rand() % 5000 < 250)
            c->item = itGrimoire;
          else if(rand() % 5000 < 10 && -d > TEMPLE_EACH * 10)
            c->item = itOrbDragon;
          }
        }
      if(c->land == laDryForest) {
        if(rand() % 5000 < 100 + 2 * (kills[moFireFairy]*2 + kills[moHedge]) && notDippingFor(itFernFlower))
          c->item = itFernFlower;
        if(rand() % 4000 < 40 + items[itFernFlower] + hard)
          c->monst = moHedge;
        else if(rand() % 8000 < 2 * items[itFernFlower] + hard)
          c->monst = moFireFairy;
        }
      if(c->land == laHell) {
        if(rand() % 1500 < 30 + (kills[moCultist] + kills[moTentacle]) && notDippingFor(itHell))
          c->item = itHell;
        if(rand() % 8000 < 40 + items[itHell] + hard)
          c->monst = moLesser;
        else if(rand() % 24000 < 40 + items[itHell] + hard)
          c->monst = moGreater;
        }
      if(c->land == laCocytus) {
        if(rand() % 5000 < 100 + 2 * (kills[moShark] + kills[moGreaterShark] + kills[moCrystalSage]) && notDippingFor(itSapphire))
          c->item = itSapphire;
        if(rand() % 5000 < 2 * (items[itSapphire] + hard)) {
          eMonster ms[3] = { moYeti, moGreaterShark, moCrystalSage };
          c->monst = ms[rand() % 3];
          if(c->monst == moGreaterShark) c->wall = waLake;
          }
        }
      if(c->land == laMotion) {
        if(rand() % 1500 < 30 + (kills[moRunDog]) && notDippingFor(itFeather))
          c->item = itFeather;
        if(rand() % 20000 < 25 + items[itFeather] + hard) {
          c->monst = moRunDog;
          // preset the movement direction
          // this will make the dog go in the direction of the center,
          // if the player is unreachable/invisible
          for(int d=0; d<c->type; d++) if(c->mov[d] == from) {
            c->mondir = (d+3) % c->type;
            }
          chasmify(c);
          }
        }
      if(c->land == laHive) {
        if(isHive(c->land) && rand() % 6000 < (items[itRoyalJelly] + hard-15)) 
          c->monst = eMonster(moBug0 + rand() % BUGCOLORS);

        /* if(rand() % 1500 < 30 + (kills[moBug0] + kills[moBug1] + kills[moBug2]) && notDippingFor(itRoyalJelly))
          c->item = itRoyalJelly; */
        /* if(rand() % 2000 < 2)
          c->monst = eMonster(moBug0 + rand() % 3); */
        }
      if(!c->item && c->wall != waCloud && c->wall != waMirror) {
        if(orbChance(c, laJungle, 1200, 1500) && items[itRuby] >= 10)
          c->item = itOrbLightning;
        if(orbChance(c, laIce, 2000, 1500) && items[itDiamond] >= 10)
          c->item = itOrbFlash;
        if(orbChance(c, laCaves, 1800, 2000) && items[itGold] >= 10)
          c->item = itOrbLife;
        if(orbChance(c, laAlchemist, 600, 800) && items[itElixir] >= 10)
          c->item = itOrbSpeed;
        if(orbChance(c, laGraveyard, 200, 200) && items[itBone] >= 10)
          c->item = itGreenStone;
        if(orbChance(c, laDesert, 2500, 600) && items[itSpice] >= 10)
          c->item = itOrbShield;
        if(orbChance(c, laHell, 2000, 1000) && items[itHell] >= 10)
          c->item = itOrbYendor;
        if(orbChance(c, laRlyeh, 1500, 1000) && items[itStatue] >= 10)
          c->item = itOrbTeleport;
        if(orbChance(c, laMotion, 2000, 700) && items[itFeather] >= 10) {
          c->item = itOrbSafety;
          }

        if(orbChance(c, laIce, 1500, 0) && items[itDiamond] >= 10)
          c->item = itOrbWinter;
        if(orbChance(c, laDryForest, 2500, 0) && items[itFernFlower] >= 10)
          c->item = itOrbWinter;
        if(orbChance(c, laCocytus, 1500, 1500) && items[itSapphire] >= 10)
          c->item = itOrbWinter;
        if(orbChance(c, laCaves, 1200, 0) && items[itGold] >= 10)
          c->item = itOrbDigging;
        if(orbChance(c, laDryForest, 500, 2000) && items[itFernFlower] >= 10)
          c->item = itOrbThorns;

        if(orbChance(c, laDeadCaves, 1800, 0) && items[itSilver] >= 10)
          c->item = itGreenStone;
        if(orbChance(c, laDeadCaves, 1800, 1000) && items[itSilver] >= 10)
          c->item = itOrbDigging;
        if(orbChance(c, laFjord, 1500, 2500) && items[itEmerald] >= 10)
          c->item = itOrbPsi;
        if(orbChance(c, laWineyard, 900, 1200) && items[itWine] >= 10)
          c->item = itOrbGhost;
        if(orbChance(c, laHive, 800, 1200) && items[itRoyalJelly] >= 10)
          c->item = itOrbInvis;
        if(orbChance(c, laPower, 0, 1500) && items[itPower] >= 10)
          c->item = itOrbFire;
        if(orbChance(c, laTemple, 0, 2500) && items[itGrimoire] >= 10)
          c->item = itOrbDragon;
        if(orbChance(c, laCamelot, 1000, 1500) && items[itHolyGrail])
          c->item = itOrbIllusion;
        }
      }
    }
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
    else if(c->monst == moWormtail) {
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

bool isGhostMover(eMonster m) {
  return m == moGhost || m == moGreaterShark || 
    (cwt.c->land == laPower && (m == moWitchGhost || m == moWitchWinter));
  }

bool havebugs, haveearth, haveeagles, haveleader;

bool bugsfighting;

// calculate cpdist and pathdist
void bfs() {
  
  int dcs = size(dcal);
  for(int i=0; i<dcs; i++) dcal[i]->cpdist = INFD;
  worms.clear(); ivies.clear(); ghosts.clear(); golems.clear(); mirrors.clear();
  temps.clear(); tempval.clear(); targets.clear(); havebugs = false;
  haveearth = false; haveeagles = false; haveleader = false;
  
  dcal.clear(); dcal.push_back(cwt.c);
  reachedfrom.clear(); reachedfrom.push_back(rand() % cwt.c->type);
  
  int pqs = size(pathq);
  for(int i=0; i<pqs; i++) pathq[i]->pathdist = INFD;
  pathq.clear(); if(!invismove) targets.push_back(cwt.c);

  cwt.c->cpdist = 0;
  int qb = 0;
  while(true) {
    int i, fd = reachedfrom[qb] + 3;
    cell *c = dcal[qb++];
    int d = c->cpdist;
    if(d == 7) { first7 = qb; break; }
    for(int j=0; j<c->type; j++) if(i = (fd+j) % c->type, c->mov[i]) {
      // printf("i=%d cd=%d\n", i, c->mov[i]->cpdist);
      cell *c2 = c->mov[i];
      if(c2 && signed(c2->cpdist) > d+1) {
        c2->cpdist = d+1;
        
        // remove treasures
        if(c2->item && c2->cpdist == 7 && itemclass(c2->item) == IC_TREASURE &&
          items[c2->item] >= 20 + currentLocalTreasure)
            c2->item = itNone;
        
        c2->ligon = 0;
        dcal.push_back(c2);
        reachedfrom.push_back(c->spn[i]);
        if(c2->monst) {
          if(isWorm(c2) || isIvy(c2)) findWormIvy(c2);
          else if(isGhostMover(c2->monst))
            ghosts.push_back(c2);
          else if(isBug(c2)) {
            havebugs = true;
            targets.push_back(c2);
            }
          else if(isFriendly(c2)) {
            targets.push_back(c2);
            if(c2->monst == moGolem) golems.push_back(c2);
            if(c2->monst == moKnight) golems.push_back(c2);
            if(c2->monst == moIllusion) {
              if(items[itOrbIllusion]) items[itOrbIllusion]--;
              else c2->monst = moNone;
              }
            if(isMimic(c2)) mirrors.push_back(c2);
            }
          else if(c2->monst == moEagle) haveeagles = true;
          else if(c2->monst == moEarthElemental) haveearth = true;
          else if(c2->monst == moCultistLeader) haveleader = true;
          }
        // pheromones!
        if(c2->land == laHive && c2->heat >= 50 && c2->wall != waWaxWall) 
          havebugs = true;
        if(c2->wall == waThumper && c2->tmp > 0) {
          useup(c2);
          targets.push_back(c2);
          }
        }
      }
    }

  for(int i=0; i<size(targets); i++) {
    targets[i]->pathdist = (targets[i] == cwt.c) ? 0 : 1;
    pathq.push_back(targets[i]);
    }

  int qtemp = size(temps);
  for(int i=0; i<qtemp; i++) temps[i]->monst = tempval[i];
  
  pathqm.clear();  
  reachedfrom.clear(); reachedfrom.push_back(rand() % cwt.c->type);

  qb = 0;
  for(qb=0; qb < size(pathq); qb++) {
    int fd = reachedfrom[qb] + 3;
    cell *c = pathq[qb];
    eucoord x, y;
    decodeMaster(c->master, x, y);
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
        (c->mov[i]->monst || passable(c->mov[i], (d==0)?NULL:c, true, false))) {
        c->mov[i]->pathdist = d+1;
        pathq.push_back(c->mov[i]); reachedfrom.push_back(c->spn[i]);
        }
      }
    }
  }

extern void cleargraphmemory();

// initialize the game
void initgame() {
  cwt.c = origin.c7; cwt.spin = 0;
  cwt.c->land = euclid ? euclidland : firstland;
  createMov(cwt.c, 0);

  for(int i=0; i<65536; i++) euland[i] = laNone;

  // extern int sightrange; sightrange = 9;
  // cwt.c->land = laHell; items[itHell] = 10;
  for(int i=9; i>=0; i--) {
    setdist(cwt.c, i, NULL);
    verifycells(&origin);
    }
  if(cwt.c->land == laCocytus)
    cwt.c->wall = waFrozenLake;
  else if(cwt.c->land == laAlchemist || cwt.c->land == laGameBoard)
    ;
  else if(cwt.c->land == laCaves || cwt.c->land == laFjord)
    cwt.c->wall = waCavefloor;
  else if(cwt.c->land == laDeadCaves)
    cwt.c->wall = waDeadfloor2;
  else
    cwt.c->wall = waNone;
  cwt.c->item = itNone;
  cwt.c->monst = moNone;

  cleargraphmemory();
  
  if(!safety) {
    timerstart = time(NULL); turncount = 0; sagephase = 0;
    timerstopped = false;
    savecount = 0; savetime = 0;
    cheater = 0;
    if(firstland != laIce) cheater++;
    }
  else safety = false;
  
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

void firetrail(cell *c) {
  if(!passable(c, NULL, true, true)) return;
  destroyHalfvine(c);
  c->item = itNone;
  if(c->wall == waFrozenLake)
    c->wall = waLake;
  if(cellUnstableOrChasm(c))
    return;
  c->wall = waBonfire, c->tmp = 10;
  }

void moveNormal(cell *c) {
  bool repeat = true;
  eMonster m = c->monst;
  
  again:

  for(int j=0; j<c->type; j++) 
    if(c->mov[j] && isFriendlyOrBug(c->mov[j]) && repeat && !attackingForbidden(c->mov[j], c)) {
      // XLATC enemy destroys a friend
      addMessage(XLAT("%The1 destroys %the2!", m, c->mov[j]->monst));
      c->mov[j]->monst = moNone;
      return;
      }

  int posdir[10], nc = 0;
  
  for(int j=0; j<c->type; j++) {
    cell *c2 = c->mov[j];
    if(!c2) continue;
    if(c2->pathdist >= c->pathdist) continue;
    if(!passable(c2, c, false, false)) continue;
    // crystal sages can't stand out of Cocytus
    if(m == moCrystalSage && (c2->land != laCocytus || c2->heat > SAGEMELT))
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

  if(nc == 0 && !passable(c, NULL, true, true)) {
    // running dogs run away!
    // they prefer a straight direction
    int sgn = rand() % 2 ? 1 : -1;
    for(int b=3; b>=0; b--) for(int s=-1; s<=1; s+=2) if(!nc) {
      int d = (c->mondir + b*s*sgn) % c->type;
      d += c->type; d %= c->type;
      cell *c2 = c->mov[d];
      if(passable(c2, c, false, false))
        posdir[nc++] = d;
      }
    }
      
/*
  // Eagles can fly through chasms/slime if needed
  if(m == moEagle && nc == 0) {
    printf("no eagle no fly\n");
    for(int j=0; j<c->type; j++) printf("%d from %d\n", c->mov[j]->cpdist, c->cpdist);
    for(int j=0; j<c->type; j++)
      if(c->mov[j] && c->mov[j]->cpdist < c->cpdist && 
        !c->mov[j]->monst && 
        (c->mov[j]->land == laMotion || c->mov[j]->land == laAlchemist) && c->wall != waDeadTroll)
        if(c->mov[j]->cpdist > 0)
          posdest[nc++] = c->mov[j];
    printf("nc=%d\n", nc);
    } */

  if(!nc) return;  
  nc = rand() % nc;
  cell *c2 = c->mov[posdir[nc]];
  c2->monst = m, c->monst = moNone;
  c2->mondir = c->spn[posdir[nc]];
  if(m == moEagle && repeat && c->pathdist > 1) { repeat = false; c = c2; goto again; }
  
  // lancers pierce our friends :(
  if(m == moLancer) { 
    // printf("lancer stab?\n");
    for(int u=2; u<=c2->type-2; u++) {
      cell *c3 = c2->mov[(c->spn[posdir[nc]]+u)%c2->type]; 
      if(c3->monst && isKillableSomehow(c3)) {
        addMessage(XLAT("%The1 pierces %the2!", m, c3->monst));
        killMonster(c3);
        }
      }
    }
  
  if(m == moWitchFire) firetrail(c);
  if(isWitch(m) && c2->item == itOrbLife && passable(c, NULL, true, true)) {
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
      if(isIcyLand(c2)) c2->heat += 0.5;
      if((c2->wall == waDune || c2->wall == waIcewall ||
        c2->wall == waAncientGrave || c2->wall == waFreshGrave || 
        c2->wall == waColumn || c2->wall == waThumper || c2->wall == waBonfire ||
        c2->wall == waDryTree || c2->wall == waWetTree ||
        c2->wall == waVinePlant || c2->wall == waVineHalfA || c2->wall == waVineHalfB)) {
        destroyHalfvine(c2); c2->wall = waNone;
        }
      if(c2->wall == waCavewall || c2->wall == waDeadTroll) c2->wall = waCavefloor;
      if(c2->wall == waDeadfloor2) c2->wall = waDeadfloor;
      if(c2->wall == waDeadwall) c2->wall = waDeadfloor2;
      if(c2->wall == waBigStatue) c2->wall = waNone;
      if(c2->wall == waFloorA || c2->wall == waFloorB)
      if(c->wall == waFloorA || c->wall == waFloorB)
        c2->wall = c->wall;
      }
    }
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
    if(c->mov[j] && isFriendlyOrBug(c->mov[j]) && !attackingForbidden(c->mov[j], c)) {
      addMessage(XLAT("%The1 eats %the2!", c->monst, c->mov[j]->monst));
      ncg = 1; gmov[0] = c->mov[j];
      break;
      }
    if(c->mov[j] && passable(c->mov[j], c, false, false) && !cellUnstable(c->mov[j]) && c->mov[j] != cwt.c) {
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
      addMessage(XLAT("The sandworm explodes in a cloud of Spice!"));
      kills[moWorm]++;
      spices = 3;
      }
    eItem loc = treasureType(c->land);
    while(c->monst == moWorm || c->monst == moWormtail || c->monst == moTentacle || c->monst == moTentacletail) {
      // if(!id) 
      explodeAround(c);
      if(spices > 0 && c->land == laDesert) {
        if(notDippingForExtra(itSpice, loc)) 
          c->item = itSpice;
        spices--;
        }
      c->monst = moNone;
      if(c->mondir != NODIR) c = c->mov[c->mondir];
      }
    return;
    }
  
  cell* goal;
  if(ncg) goal = gmov[rand() % ncg];
  else goal = bmov[rand() % ncb];
  
  for(int j=0; j<c->type; j++) if(c->mov[j] == goal) {
    goal->monst = eMonster(moWormwait + id);
      
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
        if(c->mov[j] && isFriendly(c->mov[j])) {
          addMessage(XLAT("The ivy destroys %the1!", c->mov[j]->monst));
          c->mov[j]->monst = moNone;
          continue;
          }
        if(c->mov[j] && signed(c->mov[j]->pathdist) < pd && passable(c->mov[j], c, false, false))
          mto = c->mov[j], pd = mto->pathdist, sp = c->spn[j];
        }
      c = c->mov[c->mondir];
      }

    if(mto && mto->cpdist) {        
      mto->monst = moIvyWait, mto->mondir = sp;
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

// move slimes, and also seeps

int sval = 1;
vector<cell*> slimedfs;

bool isSlimeMover(cell *c) {
  return
  c->monst == moSlime || c->monst == moSeep || c->monst == moShark ||
    c->monst == moVineSpirit;
  }

void slimevisit(cell *c, cell *from, int u) {
  if(!c) return;
  if(eq(c->tmp, sval)) return;
  if(c->wall != waCavewall && c->wall != waLake && c->wall != waDeadwall &&
    c->wall != waFloorA && c->wall != waFloorB && c->wall != waVinePlant &&
    c->wall != waVineHalfB && c->wall != waVineHalfA
    ) return;
  bool hv = false;
  // cavewalls/deadwalls are the same for seeps
  if(c->wall == waCavewall && from->wall == waDeadwall) hv++;
  if(c->wall == waDeadwall && from->wall == waCavewall) hv++;
  // only travel to halfvines correctly
  if(cellHalfvine(c) && from != cwt.c) {
    int i=0;
    for(int t=0; t<c->type; t++) if(c->mov[t] && c->mov[t]->wall == c->wall) i=t;
    int z = i-u; if(z<0) z=-z; z%=6;
    if(z>1) return;
    hv=true;
    }
  // only travel from halfvines correctly
  if(cellHalfvine(from) && from != cwt.c) {
    int i=0;
    for(int t=0; t<from->type; t++) if(from->mov[t] && from->mov[t]->wall == from->wall) i=t;
    int z = i-c->spn[u]; if(z<0) z=-z; z%=6;
    if(z>1) return;
    hv=true;
    }
  if(c->wall != from->wall && from != cwt.c && !hv) return;
  if(c->item) return;
  // if(c->wall == waThumper || c->wall == waBonfire) return;
  c->tmp = sval;
  if(size(slimedfs) < 1000) slimedfs.push_back(c), reachedfrom.push_back(u);
  if(!isWorm(c) && !isIvy(c) && !isMimic(c))
    for(int i=0; i<c->type; i++) if(c->mov[i] == from)
      c->mondir = i;
  if(isSlimeMover(c)) {

    for(int j=0; j<c->type; j++) 
      if(c->mov[j] && isFriendlyOrBug(c->mov[j])) {
        // XLATC slime/seep/shark
        addMessage(XLAT("%The1 eats %the2!", c->monst, c->mov[j]->monst));
        c->mov[j]->monst = moNone;
        return;
        }
    
    if(from->cpdist == 0 || from->monst) return;

    from->monst = c->monst, c->monst = moNone;
    }
  }

void moveslimes() {
  sval++;
  slimedfs.clear(); reachedfrom.clear(); 
  for(int i=0; i<size(targets); i++) {  
    slimedfs.push_back(targets[i]);
    reachedfrom.push_back(rand() % targets[i]->type);
    }
  for(int i=0; i<size(slimedfs); i++) {
    cell *c = slimedfs[i];
    int j = reachedfrom[i];
    for(int t=0; t<c->type; t++) {
      int u = (j+t) % c->type;
      slimevisit(c->mov[u], c, c->spn[u]);
      }
    }
  }

// move eagles

vector<cell*> eagledfs;

void eaglevisit(cell *c, cell *from, int id) {
  if(!c) return;
  if(eq(c->tmp, sval)) return;
  if(c->monst == moEagle) {
    if(id == 1) for(int j=0; j<c->type; j++) 
      if(c->mov[j] && isFriendlyOrBug(c->mov[j]) && !attackingForbidden(c->mov[j], c)) {
        // XLATC eagle
        addMessage(XLAT("%The1 claws %the2!", c->monst, c->mov[j]->monst));
        c->mov[j]->monst = moNone;
        return;
        }
    
    if(from->cpdist == 0 || from->monst) return;

    from->monst = c->monst, c->monst = moNone;
    }
  if(c->wall == waThumper || isFire(c)) return;
  c->tmp = sval;
  if(!eaglepassable(c)) return;
  if(size(eagledfs) < 1000) eagledfs.push_back(c);
  }

void moveeagles(int id) {
  if(invismove) return;
  sval++;
  eagledfs.clear();
  for(int i=0; i<size(targets); i++)
    eagledfs.push_back(targets[i]);
  //eagledfs.push_back(cwt.c);
  for(int i=0; i<size(eagledfs); i++) {
    cell *c = eagledfs[i];
    for(int t=0; t<c->type; t++)
      eaglevisit(c->mov[t], c, id);
    }
  }

vector<cell*> earthdfs;

bool earthpassable(cell *c, cell *from) {
  // cannot go through Living Caves...
  if(c->wall == waCavefloor) return false;
  // but can dig through...
  if(c->wall == waDeadwall || c->wall == waDune)
    return true;
  return passable(c, from, true, false);
  }

void earthMove(cell *from, int dir) {
  cell *c2 = from->mov[dir];
  int d = from->spn[dir];
  earthWall(from);
  if(c2) for(int u=2; u<=c2->type-2; u++) {
    cell *c3 = c2->mov[(d + u)% c2->type];
    earthFloor(c3);
    }
  }

void earthvisit(cell *c, cell *from, int d) {
  if(!c) return;
  if(eq(c->tmp, sval)) return;

  if(!earthpassable(c, from)) return;

  if(c->monst == moEarthElemental) {
    // note: move from 'c' to 'from'!
    for(int j=0; j<c->type; j++) 
      if(c->mov[j] && isFriendlyOrBug(c->mov[j]) && !attackingForbidden(c->mov[j], c)) {
        // XLATC eagle
        addMessage(XLAT("%The1 punches %the2!", c->monst, c->mov[j]->monst));
        c->mov[j]->monst = moNone;
        return;
        }
    
    if(!passable(from, c, false, false)) {
      earthFloor(from);
      return;
      }
    if(from->cpdist == 0 || from->monst) return;

    earthMove(c, from->spn[d]);
    
    from->monst = c->monst, c->monst = moNone;
    }
  if(c->wall == waThumper || isFire(c)) return;
  c->tmp = sval;
  if(size(earthdfs) < 1000) earthdfs.push_back(c);
  }

void moveearth() {
  if(invismove) return;
  sval++;
  earthdfs.clear();
  for(int i=0; i<size(targets); i++)
    earthdfs.push_back(targets[i]);
  for(int i=0; i<size(earthdfs); i++) {
    cell *c = earthdfs[i];
    for(int t=0; t<c->type; t++)
      earthvisit(c->mov[t], c, t);
    }
  }

vector<cell*> leaderdfs;

bool canPushStatueOn(cell *c) {
  return passable(c, NULL, true, false);
  }

// note: move from 'c' to 'from'!
void leadervisit(cell *c, cell *from, int d) {
  if(!c) return;
  if(eq(c->tmp, sval)) return;

  if(c->monst == moCultistLeader) {
    // he cannot push big statues into bonfires, just like the player
    if(c->wall == waBonfire && from->wall == waBigStatue)
      return;
    // note: move from 'c' to 'from'!
    for(int j=0; j<c->type; j++) 
      if(c->mov[j] && isFriendlyOrBug(c->mov[j]) && !attackingForbidden(c->mov[j], c)) {
        // XLATC eagle
        addMessage(XLAT("%The1 punches %the2!", c->monst, c->mov[j]->monst));
        c->mov[j]->monst = moNone;
        return;
        }
    
    if(from->wall != waBigStatue && !passable(from, c, false, false)) {
      return;
      }
    if(from->cpdist == 0 || from->monst) return;

    if(from->wall == waBigStatue) {
      from->wall = c->wall;
      c->wall = waBigStatue;
      }
    // earthMove(c, from->spn[d]);
    
    from->monst = c->monst, c->monst = moNone;
    }

  else if(c->wall == waBigStatue ? !canPushStatueOn(from) : !passable(c, from, false, false)) 
    return;

  if(c->wall == waThumper || isFire(c)) return;
  c->tmp = sval;

  if(size(leaderdfs) < 1000) leaderdfs.push_back(c);
  }

void moveleader() {
  if(invismove) return;
  sval++;
  leaderdfs.clear();
  for(int i=0; i<size(targets); i++)
    leaderdfs.push_back(targets[i]);
  //leaderdfs.push_back(cwt.c);
  for(int i=0; i<size(leaderdfs); i++) {
    cell *c = leaderdfs[i];
    for(int t=0; t<c->type; t++)
      leadervisit(c->mov[t], c, t);
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

// from-to
bool ghostmove(cell* c1, cell* c2) {
  if(c1->monst == moGhost || c1->monst == moWitchGhost) return true;
  if(c1->monst == moGreaterShark) return c2->wall == waLake;
  if(c1->monst == moWitchWinter) 
    return c2->wall == waBonfire || passable(c2, c1, false, false);
  return false;
  }

void moveghosts() {

  if(invismove) return;
  for(int d=0; d<8; d++) movesofgood[d].clear();  

  for(int i=0; i<size(ghosts); i++) {
    cell *c = ghosts[i];
    if(isGhostMover(c->monst) && c->cpdist > 1) {
      int goodmoves = 0;

      for(int k=0; k<c->type; k++) if(c->mov[k] && !c->mov[k]->monst && c->mov[k]->cpdist < c->cpdist)
        if(ghostmove(c, c->mov[k]))
          goodmoves++;
      
      movesofgood[goodmoves].push_back(c);
      }
    }
  
  for(int d=0; d<8; d++) for(int i=0; i<size(movesofgood[d]); i++) {
    cell *c = movesofgood[d][i];
    
    if(isGhostMover(c->monst) && c->cpdist > 1) {
      
      int mdir[7];

      for(int j=0; j<c->type; j++) 
        if(c->mov[j] && isFriendlyOrBug(c->mov[j])) {
          // XLATC ghost/greater shark
          addMessage(XLAT("%The1 scares %the2!", c->monst, c->mov[j]->monst));
          c->mov[j]->monst = moNone;
          return;
          }
    
      int qmpos = 0;
      for(int k=0; k<c->type; k++) if(c->mov[k] && !c->mov[k]->monst && c->mov[k]->cpdist < c->cpdist)
        if(ghostmove(c, c->mov[k]))
          mdir[qmpos++] = k;
      if(!qmpos) continue;
      int d = mdir[rand() % qmpos];
      cell *c2 = c->mov[d];
      c2->monst = c->monst; c->monst = moNone;
      }
    }
  }

int lastdouble = -3;

void stabbingAttack(cell *mf, cell *mt, eMonster who = moNone) {
  int numsh = 0, numflail = 0, numlance = 0;
  for(int t=0; t<mf->type; t++) {
    cell *c = mf->mov[t];
    if(c->monst == moHedge || (!who && items[itOrbThorns] && c->monst && isKillable(c))) {
      for(int u=0; u<c->type; u++) {
        if(c->mov[u] == mt) {
          if(who)
            addMessage(XLAT("%The1 stabs %the2.", who, c->monst));
          else
            addMessage(XLAT("You stab %the1.", c->monst));
          int k = tkills();
          killMonster(c);
          if(tkills() > k) numsh++;
          }
        }
      }
    if(c->monst == moFlailer) {
      bool away = true;
      for(int u=0; u<c->type; u++) if(c->mov[u] == mt) away = false;
      if(away) {
        if(who)
          addMessage(XLAT("%The1 tricks %the2.", who, c->monst));
        else
          addMessage(XLAT("You trick %the1.", c->monst));
        int k = tkills();
        killMonster(c);
        if(tkills() > k) numflail++;
        }
      }
    }

  for(int t=0; t<mt->type; t++) {
    cell *c = mt->mov[t];
    if(c->monst == moLancer) {
      if(who)
        addMessage(XLAT("%The1 tricks %the2.", who, c->monst));
      else
        addMessage(XLAT("You trick %the1.", c->monst));
      int k = tkills();
      killMonster(c);
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
    if(m == moGolem || m == moKnight) {
      if(m == moGolem) qg++;
      int bestv = 100, bq = 0, bdirs[7];
      for(int k=0; k<c->type; k++) if(c->mov[k]) {
        int val;
        if(c->mov[k] == cwt.c) val = 0;
        else if(isActiveEnemy(c->mov[k], NULL) && isKillable(c->mov[k]) &&
          !attackingForbidden(c->mov[k], c))
          val = 12000;
        else if(isInactiveEnemy(c->mov[k]) && isKillable(c->mov[k]) && 
          !attackingForbidden(c->mov[k], c))
          val = 10000;
        else if(isIvy(c->mov[k]) && !attackingForbidden(c->mov[k], c)) val = 8000;
        else if(monstersnear(c->mov[k], NULL, false)) val = 0;
        else if(passable(c->mov[k], c, false, false)) val = 4000;
        else val = 0;
        if(c->monst == moGolem)
          val -= c->mov[k]->cpdist;
        if(c->monst == moKnight && c->mov[k]->master->alt)
          val -= celldistAlt(c->mov[k]);
        if(val > bestv) bestv = val, bq = 0;
        if(val == bestv) bdirs[bq++] = k;
        }
      if(bestv <= 100) continue;
      int dir = bdirs[rand() % bq];
      cell *c2 = c->mov[dir];
      if(c2->monst) {
        addMessage(XLAT("%The1 destroys %the2!", c->monst, c2->monst));
        killMonster(c2);
        }
      else {
        stabbingAttack(c, c2, m);
        c2->monst = m == moGolem ? moGolemMoved : moKnightMoved;
        c2->mondir = c->spn[dir];
        c->monst = moNone;
        }
      }
    }
  achievement_count("GOLEM", qg, 0);
  }

bool wchance(int a, int of) {
  of *= 10; 
  a += items[itOrbYendor] * 5 + items[itHolyGrail] + 1;
  if(cwt.c->land == laCrossroads) a+= items[itHyperstone] * 10;
  for(int i=0; i<ittypes; i++) if(itemclass(eItem(i)) == IC_TREASURE)
    a = max(a, (items[i]-10) / 10);
  return rand() % (a+of) < a;
  }

void wandering() {
  int t = turncount - lastexplore;
  int seepcount = 0;
  if(t > 40) seepcount = (t-40 + rand() % 20) / 20;
  int ghostcount = 0;
  if(t > 80) ghostcount = (t-80 + rand() % 20) / 20;
  
  while(first7 < size(dcal)) {
    int i = first7 + rand() % (size(dcal) - first7);
    cell *c = dcal[i];
    
    // wandering seeps & ghosts
    if(seepcount && c->wall == waCavewall && !c->monst && eq(c->tmp, sval)) {
      c->monst = moSeep;
      seepcount--;
      continue;
      }
    
    if(ghostcount && !c->monst && cwt.c->type != laCaves) {
      c->monst = moGhost;
      ghostcount--;
      continue;
      }
    
    if((c->wall == waCavewall || c->wall == waDeadwall) && !c->monst && eq(c->tmp, sval) &&
      wchance(items[treasureType(c->land)], 10)) {
      c->monst = moSeep;
      continue;
      }
    
    else if(c->monst || c->pathdist == INFD) break;

    else if(c->land == laIce && wchance(items[itDiamond], 10))
      c->monst = rand() % 2 ? moWolf : moYeti;

    else if(c->land == laDesert && wchance(items[itSpice], 10))
      c->monst = rand() % 10 ? moDesertman : moWorm;

    else if(c->land == laCaves && wchance(items[itGold], 5))
      c->monst = rand() % 3 ? moTroll : moGoblin;

    else if(c->land == laDeadCaves && wchance(items[itSilver], 5))
      c->monst = rand() % 20 ? (rand() % 3 ? moDarkTroll : moGoblin) : moEarthElemental;

    else if(c->land == laJungle && wchance(items[itRuby], 40))
      c->monst = rand() % 10 ? moMonkey : moEagle;

    else if(c->land == laMirror && wchance(items[itShard], 15))
      c->monst = rand() % 10 ? moRanger : moEagle;

    else if(c->land == laHell && wchance(items[itHell], 20))
      c->monst = rand() % 3 ? moLesser : moGreater;

    else if(c->land == laRlyeh && wchance(items[itStatue], 15))
      c->monst = rand() % 3 ? moPyroCultist : 
        (rand() % 40 < items[itStatue]-25) ? moCultistLeader : moCultist;

    else if(c->land == laGraveyard && wchance(items[itBone], 15))
      c->monst = rand() % 5 ? moGhost : moNecromancer;
      
    else if(c->land == laDryForest && wchance(items[itFernFlower], 5))
      c->monst = rand() % 5 ? moHedge : moFireFairy;
      
    else if(c->land == laCocytus && wchance(items[itSapphire], 45))
      c->monst = moCrystalSage;
      
    else if(c->land == laAlchemist && wchance(items[itElixir], 3) && eq(c->tmp, sval) && c->item == itNone)
      c->monst = moSlime;
    
    else if(c->land == laFjord && wchance(items[itEmerald], 5)) {
      static eMonster m[4] = {moHedge, moLancer, moMiner, moFlailer};      
      c->monst = m[rand() % 4];
      }
    
    else if(c->land == laWineyard && wchance(items[itWine], 10)) {
      c->monst = moVineBeast;
      }
    
    else if(c->land == laPower && wchance(items[itPower], 10)) {
      c->monst = eMonster(moWitch + rand() % NUMWITCH);
      }
    
    else if(c->land == laCamelot && rand() % 30 == 0 && celldistAltRelative(c) < 0) {
      eMonster m[3] = { moHedge, moLancer, moFlailer };
      c->monst = m[rand() % 3];
      }
    
    else if(c->land == laCrossroads && items[itHyperstone] && wchance(items[itHyperstone], 20)) {
      // only interesting monsters here!
      static eMonster m[12] = {
        moWorm, moTroll, moEagle,
        moLesser, moGreater, moPyroCultist, moGhost,
        moFireFairy, moHedge,
        moLancer, moFlailer, moVineBeast
        };
      c->monst = m[rand() % 9];
      }
    
    else break;
      
    if(c->monst == moWorm) c->mondir = NODIR;
      
    // laMotion -> no respawn!
    }
  }

void sageheat(cell *c, double v) {
  c->heat += v;
  if(c->wall == waFrozenLake && c->heat > .6) c->wall = waLake;
  }

bool normalMover(eMonster m) {
  return
    m == moYeti || m == moRanger || m == moGoblin || m == moTroll || m == moDesertman || 
    m == moMonkey || m == moZombie || m == moNecromancer || m == moCultist || 
    m == moLesser || m == moGreater || m == moRunDog || m == moPyroCultist || 
    m == moFireFairy || m == moCrystalSage || m == moHedge ||
    m == moVineBeast || m == moLancer || m == moFlailer ||
    m == moMiner || m == moDarkTroll ||
    (cwt.c->land == laPower && (
      (isWitch(m) && m != moWitchGhost && m != moWitchWinter) || m == moEvilGolem
      ));
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
  if(isActiv(c) || !passable(c, NULL, true, false))
    return;
  short& i(c->tmp);
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
  if(c->land == laHive && c->heat > 1) {
    c->heat -= 1;
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
    if(c2->tmp < 0 || c2->tmp >= size(buginfo)) continue;
    if(!passable(c, c2, true, false)) continue;
    int j = c2->tmp;
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
        if(!passable(c2, c, false, false)) continue;
        if(c2->tmp < 0 || c2->tmp >= size(buginfo)) continue;
        if(buginfo[c2->tmp].where != c2) continue;
        if(buginfo[c2->tmp].dist[k] < b.dist[k])
          qual = 1;
        else if(buginfo[c2->tmp].dist[k] == b.dist[k])
          qual = 0;
        }
      // printf("%d->#%d %d: %d\n", i, dir, c2->tmp, qual);
      if(qual > bqual) bqual = qual, q=0;
      if(qual == bqual) gmoves[q++] = dir;
      }
    
    if(!q) { c->heat += 3; continue; }
    int d = gmoves[rand() % q];
    cell *c2 = c->mov[d];
    if(c2->monst) {
      eMonster killed = c2->monst;
      if(isBug(killed)) battlecount++;
      else addMessage(XLAT("%The1 fights with %the2!", c->monst, c2->monst));
      killMonster(c2);
      // killMonster(c);
      if(isBug(killed)) {
        c2->monst = moDeadBug, deadbug.push_back(c2);
        bugcount[killed - moBug0]--;
        }
//    c->monst = moDeadBug, deadbug.push_back(c);
      }
    else {
      c2->monst = c->monst;
      c2->mondir = c->spn[d];
      c->monst = moNone;
      // pheromones!
      if(c->heat < 90) c->heat += 5;
      if(c2->heat < 90) c2->heat += 5;
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
  if(isActiv(c) || !passable(c, NULL, true, false)) return;
  short& i = c->tmp;
  if(i >= 0 && i < size(buginfo) && buginfo[i].where == c)
    return;
  i = size(buginfo);
  buginfo_t b;
  b.where = c;
  b.dist[0] = d;
  buginfo.push_back(b);
  }

void createBugArmy(cell *c) {
  int k = rand() % BUGCOLORS;
  int minbugs = 50, maxbugs = 50;
  int var = 5 + items[itRoyalJelly];
  if(var>25) var=25;
  // minbugs += 100; maxbugs += 100;
  minbugs -= var; maxbugs += var;
  maxbugs += items[itRoyalJelly];
  int numbugs = minbugs + rand() % (maxbugs - minbugs + 1);
  
  /* int i = items[itRoyalJelly];
  int chance = 20 + 25 * i + 9000;
  // i=0: 16%
  // i=10: 73%
  // i=50: 1270 vs 6000
  eMonster m = eMonster(moBug0 + rand() % BUGCOLORS);
  if(c->wall) return;
  for(int i=0; i<c->type; i++) {
    cell *c2 = createMov(c,i);
    if(rand() % (100+chance) < chance) {
      if(!c2->wall) c2->monst = m;
      for(int j=2; j<=c2->type-2; j++) {
        int jj = (j+c->spn[i]) % c2->type;
        cell *c3 = createMov(c2, jj);
        if(rand() % (6000+chance) < chance && !c3->wall)
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
      cwspin(bf, 3 + rand() % 2);
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
    if(c->heat >= 100) return;
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
    
      if(c->pathdist == 1 && c->monst != moGhost) {
        if(items[itOrbShield]) continue;
        addMessage(XLAT("%The1 is confused!", c->monst));
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
  
  if(isActiveEnemy(c, NULL)) {
  
    if(c->pathdist == 1 && c->monst != moGhost) {
      // c->iswall = true; c->ismon = false;
      if(items[itOrbShield] || c->monst == moCrystalSage) return;

      addMessage(XLAT("%The1 is confused!", m));
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
        if(passable(c->mov[j], c, false, false) && c->mov[j]->pathdist < c->pathdist)
          ztab[zombienum++] = c->mov[j];
        }
      if(gravenum && zombienum) {
        cell *gr = gtab[rand() % gravenum];
        gr->wall = waAncientGrave;
        gr->monst = moGhost;
        ztab[rand() % zombienum]->monst = moZombie;
        addMessage(XLAT("%The1 raises some undead!", c->monst));
        return;
        }
      }
    
    if(c->monst == moWolf) {
      int bhd = NODIR;
      ld besth = c->heat;
      for(int j=0; j<c->type; j++) if(c->mov[j]->heat > besth && passable(c->mov[j], c, false, false))
        besth = c->mov[j]->heat, bhd = j;
      if(bhd != NODIR) {
        // printf("wolf moved from %Lf (%p) to %Lf (%p)\n", c->heat, c, besth, c->mov[bhd]);
        c->mov[bhd]->monst = moWolfMoved, c->monst = moNone;
        }
      }
    
    else if(c->monst == moPyroCultist && c->cpdist <= 4 && cwt.c->wall == waNone && !cellUnstable(cwt.c)) {
      addMessage(XLAT("%The1 throws fire at you!", c->monst));
      if(itemBurns(cwt.c->item)) 
        addMessage(XLAT("%The1 burns!", cwt.c->item)), cwt.c->item = itNone;
      cwt.c->wall = waBonfire;
      cwt.c->tmp = 20;
      c->monst = moCultist;
      }

    else if(c->monst == moWitchFlash && flashWouldKill(c, true) && !flashWouldKill(c, false)) {
      addMessage(XLAT("%The1 activates her Flash spell!", c->monst));
      c->monst = moWitch;
      activateFlashFrom(c);
      }

    else if(c->monst == moCrystalSage && c->cpdist <= 4 && isIcyLand(cwt.c)) {
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
  int wrm = size(worms);
  for(int i=0; i<wrm; i++) {
    moveWorm(worms[i]);
    }

  DEBT("ivy");
  moveivy();
  DEBT("slimes");
  moveslimes();
  DEBT("eagles");
  if(haveeagles) moveeagles(1);
  if(haveeagles) moveeagles(2);
  DEBT("earth");
  if(haveearth) moveearth();
  DEBT("leader");
  if(haveleader) moveleader();
  DEBT("bugs");
  if(havebugs) movebugs();

  DEBT("golems");
  movegolems();
  
  DEBT("fresh");
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    
    if(c->monst == moWolfMoved) c->monst = moWolf;
    if(c->monst == moIvyNext) {
      c->monst = moIvyHead; ivynext(c);
      }
    if(c->monst == moIvyDead) 
      removeIvy(c);
    if(c->monst == moGolemMoved) c->monst = moGolem;
    if(c->monst == moKnightMoved) c->monst = moKnight;
    if(c->monst == moSlimeNextTurn) c->monst = moSlime;
    if(c->monst == moLesser) c->monst = moLesserM;
    else if(c->monst == moLesserM) c->monst = moLesser;
    if(c->monst == moGreater) c->monst = moGreaterM;
    else if(c->monst == moGreaterM) c->monst = moGreater;
    
    if(c->wall == waChasm) {
      c->item = itNone;
      if(c->monst && c->monst != moGhost && c->monst != moEagle) {
        if(c->monst != moRunDog) achievement_gain("FALLDEATH1");
        killMonster(c);
        }
      }

    if(c->wall == waBonfire) {
      if(c->monst && c->monst != moGhost && c->monst != moWitchWinter && c->monst != moWitchGhost) {
        addMessage(XLAT("%The1 burns!", c->monst));
        killMonster(c);
        }
      }

    if(c->wall == waLake) {
      c->item = itNone;
      if(c->monst == moLesser || c->monst == moLesserM || c->monst == moGreater || c->monst == moGreaterM)
        c->monst = moGreaterShark;
      if(c->monst && c->monst != moShark && c->monst != moGreaterShark &&
        c->monst != moGhost && c->monst != moEagle) killMonster(c);
      }

    if(c->monst && cellUnstable(c) && c->monst != moGhost && c->monst != moEagle) {
      c->wall = waChasm;
      }
    }

  DEBT("shadow");
  moveshadow();
  
  DEBT("wandering");
  wandering();
  }

// move heat

vector<cell*> vinefires;

void heat() {
  double rate = items[itOrbSpeed] ? .5 : 1;
  int oldmelt = kills[0];

  /* if(cwt.c->heat > .5)  cwt.c->heat += .3;
  if(cwt.c->heat > 1.)  cwt.c->heat += .3;
  if(cwt.c->heat > 1.4) cwt.c->heat += .5; */
  if(isIcyLand(cwt.c))
    cwt.c->heat += (items[itOrbWinter] ? -1.2 : 1.2) * rate;
  
  vinefires.clear();
  
  int dcs = size(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->cpdist > 8) break;
    if(isFire(c) && c->tmp > 0) {
      if(c->land != laPower) useup(c);
      if(c->wall == waBonfire) for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if(c2 && c2->wall == waVinePlant)
          vinefires.push_back(c2);
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
    if(!isIcyLand(c)) continue;
    if(c->monst == moRanger) c->heat += 3 * rate;
    if(c->monst == moDesertman) c->heat += 4 * rate;
    if(c->monst == moMonkey) c->heat += rate;
    if(c->wall == waDeadTroll) c->heat -= 2 * rate;
    if(c->wall == waBigStatue) c->heat -= 3 * rate;
    if(c->monst == moLesser || c->monst == moLesserM || c->monst == moGreater || c->monst == moGreaterM)
      c->heat += (c->land == laCocytus ? 1.5 : 10) * rate;
    if(c->monst == moGreaterShark)
      c->heat += 2 * rate;
    if(c->monst == moCultist) c->heat += 3 * rate;
    if(c->monst == moCultistLeader) c->heat += 4 * rate;
    if(c->monst == moPyroCultist) c->heat += 6 * rate;
    if(c->monst == moGhost) c->heat -= rate;
    if(c->wall == waBonfire && c->tmp > 0) c->heat += 4 * rate;
    
    ld hmod = 0;
    
    for(int j=0; j<c->type; j++) if(c->mov[j]) {
      if(!isIcyLand(c->mov[j])) {
        // make sure that we can still enter Cocytus,
        // it won't heat up right away even without Orb of Winter or Orb of Speed
        if(c->mov[j] == cwt.c && (c->land == laIce || items[itOrbWinter])) 
          hmod += rate * (items[itOrbWinter] ? -1.2 : 1.2) / 4;
        continue;
        }
      ld hdiff = c->mov[j]->heat - c->heat;
      hdiff /= 10;
      if(c->mov[j]->cpdist <= 8)
        c->mov[j]->heat -= hdiff;
      else
        hdiff = -c->heat / 250;
      hmod += hdiff;
      }
    
    c->heat += hmod * rate;
    if(c->monst == moCrystalSage && c->heat >= SAGEMELT) {
      addMessage(XLAT("%The1 melts away!", c->monst));
      killMonster(c);
      }
    }
  
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->wall == waIcewall && c->heat > .4) c->wall = waNone, kills[0]++;
    if(c->wall == waFrozenLake && c->heat > .6) c->wall = waLake, kills[0]++;
    if(c->wall == waLake && c->heat < -.4 && c->monst != moGreaterShark) {
      c->wall = waFrozenLake;
      if(c->monst == moShark) {
        addMessage(XLAT("%The1 is frozen!", c->monst));
        killMonster(c);
        }
      }
    }

  for(int i=0; i<size(vinefires); i++) {
    cell* c = vinefires[i];
    if(c->wall == waVinePlant) {
      if(c->monst == moVineBeast) killMonster(c);
      c->wall = waBonfire, c->tmp = 6;
      }
    else if(cellHalfvine(c)) destroyHalfvine(c, waPartialFire);
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
    
    if(c->wall != waCavefloor && c->wall != waCavewall) continue;
    
    // if(c->wall == waThumper || c->wall == waBonfire) continue;
    c->tmp = 0;
    if(c->monst == moDarkTroll) c->monst = moTroll;
    if(c->item || c->monst || c->cpdist == 0) continue;
    for(int j=0; j<c->type; j++) if(c->mov[j]) {
      if(c->mov[j]->wall == waDeadfloor) c->tmp++, bringlife.push_back(c->mov[j]);
      else if(c->mov[j]->wall == waDeadwall || c->mov[j]->wall == waDeadfloor2) c->tmp--, bringlife.push_back(c->mov[j]);
      else if(c->mov[j]->wall == waCavefloor) c->tmp++;
      else if(c->mov[j]->wall == waCavewall) c->tmp--;
      else if(c->mov[j]->wall == waDeadTroll) c->tmp -= 5;
      else if(c->mov[j]->wall == waVinePlant) c->tmp--;
      else if(c->mov[j]->wall != waBarrier) c->tmp += 5;      
      if(c->mov[j]->cpdist == 0 && items[itOrbDigging]) c->tmp+=100;
      if(c->mov[j]->wall == waThumper && c->mov[j]->tmp > 0) c->tmp+=100;
      if(c->mov[j]->wall == waBonfire) c->tmp+=100;
      if(c->mov[j]->wall == waBigStatue) c->tmp-=100;
      if(c->mov[j]->item) c->tmp+=2;
      if(c->mov[j]->monst == moZombie) c->tmp += 10;
      if(c->mov[j]->monst == moGhost) c->tmp += 10;
      if(c->mov[j]->monst == moNecromancer) c->tmp += 10;
      if(c->mov[j]->monst == moWormtail) c->tmp++;
      if(c->mov[j]->monst == moTentacletail) c->tmp-=2;
      if(isIvy(c->mov[j])) c->tmp--;
      if(isDemon(c->mov[j])) c->tmp-=3;
      // if(c->mov[j]->monst) c->tmp++;
      // if(c->mov[j]->monst == moTroll) c->tmp -= 3;
      }
    }

  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->cpdist > 8) break;
    if(c->wall != waCavefloor && c->wall != waCavewall) continue;
//  if(c->land != laCaves) continue;
//  if(c->wall == waThumper || c->wall == waBonfire) continue;
    
    if(c->tmp > 0) c->wall = waCavefloor;
    if(c->tmp < 0) {
      c->wall = waCavewall;
      if(c->land != laCaves && c->land != laDeadCaves && c->land != laFjord && !gardener) {
        gardener = true;
        achievement_gain("GARDENER");
        }
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
      killMonster(c);
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
    if(c->wall == waThumper) continue;
    if(c->wall == waBonfire)
      c->heat = 0;
    
    for(int j=0; j<c->type; j++) if(c->mov[j]) {
      if(c->mov[j]->wall == waBonfire) c->heat++;
      }
    }

  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->cpdist > 8) break;
    if(c->land != laDryForest) continue;
    if(c->wall == waNone && c->heat >= 10)
      burnMonstersAndItems(c, 50);
    if((c->wall == waDryTree || c->wall == waWetTree || c->wall == waBonfire) && c->heat >= 1)
      c->wall = waBonfire, c->tmp = 50;
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
    c->wall == waNone || c->wall == waCavefloor || c->wall == waFloorA || c->wall == waFloorB ||
    c->wall == waFrozenLake || c->wall == waDeadfloor || c->wall == waDeadfloor2;
  }

void castLightningBolt(cellwalker lig);

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
        addMessage(XLAT("%The1 destroys %the2!", m, c2->monst));
        killMonster(c2);
        }
      if(!go) continue;
      c->monst = moNone;
      if(!c2) continue;
      stabbingAttack(c, c2, m);
      if(!passable(c2, c, true, true))  continue;
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
      c2->monst = m;
      c2->mondir = c->spn[c->mondir];
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

void reduceOrbPowers() {
  if(items[itOrbLightning]) items[itOrbLightning]--;
  if(items[itOrbSpeed]) items[itOrbSpeed]--;
  if(items[itOrbFlash]) items[itOrbFlash]--;
  if(items[itOrbShield]) items[itOrbShield]--;
  if(items[itOrbWinter]) items[itOrbWinter]--;
  if(items[itOrbFire]) items[itOrbFire]--;
  if(items[itOrbIllusion]) items[itOrbIllusion]--;
  if(items[itOrbDragon]) items[itOrbDragon]--;
  if(items[itOrbPsi]) items[itOrbPsi]--;
  if(items[itOrbInvis]) items[itOrbInvis]--;
  if(items[itOrbGhost]) items[itOrbGhost]--;
  if(items[itOrbDigging]) items[itOrbDigging]--;
  if(items[itOrbTeleport]) items[itOrbTeleport]--;
  if(items[itOrbSafety]) items[itOrbSafety]--;
  if(items[itOrbThorns]) items[itOrbThorns]--;
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
  killMonster(c);
  if(isIcyLand(c))
    c->heat += 2;
  if(c->land == laDryForest)
    c->heat += 2;
  if(c->wall == waCavewall)  c->wall = waCavefloor;
  if(c->wall == waDeadTroll) c->wall = waCavefloor;
  if(c->wall == waDeadfloor2)  c->wall = waDeadfloor;
  if(c->wall == waDeadwall)  c->wall = waDeadfloor2;
  if(c->wall == waMirror)    c->wall = waNone;
  if(c->wall == waCloud)     c->wall = waNone;
  if(c->wall == waDune)      c->wall = waNone;
  if(c->wall == waAncientGrave) c->wall = waNone;
  if(c->wall == waFreshGrave) c->wall = waNone;
  if(c->wall == waColumn)    c->wall = waNone;
  if(c->wall == waGlass)     c->wall = waNone;
  if(c->wall == waDryTree || c->wall == waWetTree)    c->wall = waNone;
  if(c->wall == waBigStatue) c->wall = waNone;
  if(isActiv(c))             c->tmp = 77;
  }

extern void drawFlash(cell* c);

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
  if(c.c->mov[d]->wall == waBarrier) return true;
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
    killMonster(c);
    if(isIcyLand(c) || c->land == laDryForest) c->heat += 2;
    c->ligon = 1;
    
    bool brk = false, spin = false;

    if(c->wall == waCavewall)  c->wall = waCavefloor, brk = true;
    if(c->wall == waDeadTroll) c->wall = waCavefloor, brk = true;
    if(c->wall == waDeadfloor2)c->wall = waDeadfloor;
    if(c->wall == waDeadwall)  c->wall = waDeadfloor2, brk = true;
    if(c->wall == waGlass)     c->wall = waNone, spin = true;
    if(c->wall == waDune)      c->wall = waNone, brk = true;
    if(c->wall == waIcewall)   c->wall = waNone, brk = true;
    if(c->wall == waAncientGrave) c->wall = waNone, spin = true;
    if(c->wall == waFreshGrave) c->wall = waNone, spin = true;
    if(c->wall == waBigStatue) c->wall = waNone, spin = true;
    if(c->wall == waColumn)    c->wall = waNone, spin = true;
    if(isActiv(c))             c->tmp = 77;
    if(c->wall == waDryTree || c->wall == waWetTree || c->wall == waVinePlant)    {
      burnMonstersAndItems(c, 4);
      brk = true;
      }
    if(cellHalfvine(c) && c->mov[lig.spin] && c->wall == c->mov[lig.spin]->wall) {
      destroyHalfvine(c, waPartialFire, 4);
      brk = true;
      }

    if(c == cwt.c)             {bnc++; if(bnc > 10) break; }
    if(spin) cwspin(lig, rand() % lig.c->type);
    
    if(brk) break;
    
    if(c->wall == waBarrier || c->wall == waCamelot)   {
      int left = -1;
      int right = 1;
      while(barrierAt(lig, left)) left--;
      while(barrierAt(lig, right)) right++;
      cwspin(lig, -(right + left));
      bnc++; if(bnc > 10) break;
      }
    else {
      cwspin(lig, 3);
      if(c->type == 7) cwspin(lig, rand() % 2);
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
  extern void drawLightning();
  drawLightning();
  addMessage(XLAT("You activate the Lightning spell!"));
  items[itOrbLightning] = 0;
  for(int i=0; i<cwt.c->type; i++) 
    castLightningBolt(cellwalker(cwt.c, i));
  achievement_count("LIGHTNING", tkills(), tk);
  }

// move the PC in direction d (or stay in place for d == -1)

bool canmove = true;

bool checkNeedMove(bool checkonly) {
  if(items[itOrbGhost] > 1) return false;
  if(cwt.c->wall == waRoundTable) {
    if(checkonly) return true;
    addMessage(XLAT("It would be impolite to land on the table!"));
    }
  else if(cwt.c->wall == waLake) {
    if(checkonly) return true;
    addMessage(XLAT("Ice below you is melting! RUN!"));
    }
  else if(isFire(cwt.c) && items[itOrbShield] < 2 && !items[itOrbWinter]) {
    if(checkonly) return true;
    addMessage(XLAT("This spot will be burning soon! RUN!"));
    }
  else if(items[itOrbGhost] == 1 && !player_passable(cwt.c, NULL, false)) {
    if(checkonly) return true;
    addMessage(XLAT("Your Aether power has expired! RUN!"));
    }
  else if(cwt.c->wall == waChasm) {
    if(checkonly) return true;
    addMessage(XLAT("The floor has collapsed! RUN!"));
    }
  else return false;
  return true;
  }

#define YDIST 101

struct yendorinfo {
  cell *path[YDIST];
  bool found;
  };

vector<yendorinfo> yi;

int yii = 0;

bool checkYendor(cell *yendor, bool checkonly) {
  int byi = size(yi);
  for(int i=0; i<size(yi); i++) if(yi[i].path[0] == yendor) byi = i;
  if(byi < size(yi) && yi[byi].found) return true;
  if(checkonly) return false;
  if(byi == size(yi)) {
    yendorinfo nyi;
    nyi.path[0] = yendor;

    cellwalker lig(yendor, rand() % yendor->type);

    cell *prev = yendor;
    
    for(int i=0; i<YDIST-1; i++) {
      nyi.path[i] = lig.c;
      
      prev = lig.c;
      cwstep(lig);
      cwspin(lig, 3);
      if(lig.c->type == 7) cwspin(lig, rand() % 2);
            
      setdist(lig.c, 10, prev);
      setdist(lig.c, 9, prev);
      }

    nyi.path[YDIST-1] = lig.c;
    nyi.found = false;

    cell *key = lig.c;

    for(int b=10; b>=7; b--) setdist(key, b, prev);
    
    for(int i=-1; i<key->type; i++) {
      cell *c2 = i >= 0 ? key->mov[i] : key;
      c2->monst = moNone; c2->item = itNone;
      if(!passable(c2, NULL, true, true)) {
        if(c2->wall == waCavewall) c2->wall = waCavefloor;
        else if(c2->wall == waDeadwall) c2->wall = waDeadfloor2;
        else if(c2->wall == waLake) c2->wall = waFrozenLake;
        else c2->wall = waNone;
        }
      }
    key->item = itKey;

    yi.push_back(nyi);
    }
  yii = byi;
  addMessage(XLAT("You need to find the right Key to unlock this Orb of Yendor!"));
  achievement_gain("YENDOR1");
  return false;
  }

int countMyGolems() {
  int g=0, dcs = size(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->monst == moGolem) g++;
    }
  return g;
  }

void restoreGolems(int qty) {
  int dcs = size(dcal);
  for(int i=1; qty && i<dcs; i++) {
    cell *c = dcal[i];
    if(passable(c, NULL, false, false))
      c->monst = moGolem, qty--;
    }
  }

void activateSafety(eLand l) {
  extern void drawSafety();
  int g = countMyGolems();
  drawSafety();
  addMessage(XLAT("You fall into a wormhole!"));
  eLand f = firstland;
  if(l == laTemple) l = laRlyeh;
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
  restoreGolems(g); 
  extern void restartGraph();
  restartGraph();  
  }

bool hasSafeOrb(cell *c) {
  return 
    c->item == itOrbSafety ||
    c->item == itOrbShield ||
    c->item == itOrbYendor;
  }


bool movepcto(int d, bool checkonly = false);

void checkmove() {
  canmove = false;
  if(movepcto(-1, true)) canmove = true;
  for(int i=0; i<cwt.c->type; i++) 
    if(movepcto(1, true)) canmove = true;
  if(!canmove)
    achievement_final(true);
  if(canmove && timerstopped) {
    timerstart = time(NULL);
    timerstopped = false;
    }
  }

// move the PC. Warning: a very long function! todo: refactor

void placeGolem(cell *on, cell *moveto, eMonster m) {
  if(passable(on, moveto, false, false))
    cwt.c->monst = m;
  else if(on->wall == waBonfire)
    addMessage(XLAT("%The1 burns!", m));
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

enum orbAction { roMouse, roKeyboard, roCheck, roMouseForce };

// roCheck: return orb type if successful, 0 otherwise
// roMouse/roKeyboard: 
//    return orb type if successful, eItem(-1) if do nothing, 0 otherwise
eItem targetRangedOrb(cell *c, orbAction a);

bool haveRangedOrb() {
  return 
    items[itOrbPsi] || items[itOrbDragon] || items[itOrbTeleport] ||
    items[itOrbIllusion];
  }

bool isRangedOrb(eItem i) {
  return i == itOrbPsi || i == itOrbDragon || i == itOrbTeleport || i == itOrbIllusion;
  }

bool haveRangedTarget() {
  if(!haveRangedOrb())
    return false;
  for(int i=0; i<size(dcal); i++) {
    cell *c = dcal[i];
    if(targetRangedOrb(c, roCheck)) return true;
    }
  return false;
  }

bool movepcto(int d, bool checkonly) {
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

    if(isActiv(c2) && c2->tmp == -1) {
      if(checkonly) return true;
      addMessage(XLAT("You activate %the1.", c2->wall));
      c2->tmp = 100;
      checkmove();
      return true;
      }

    if((c2->wall == waThumper/* || (c2->wall == waBigStatue && c2->type == 6)*/) && !monstersnear(c2) && !c2->monst) {
      eWall w = c2->wall;
      cellwalker push = cwt;
      cwstep(push);
      cwspin(push, 3);
      cwstep(push);
/*    if(w == waBigStatue && push.c->type == 7) {
        if(checkonly) return false;
        addMessage(XLAT("%The1 is too heavy to put it back on the pedestal.", c2->wall));
        return false;
        } */
      if((!passable(push.c, c2, false, true) || !passable(push.c, cwt.c, false, true) || push.c->item) && c2->type == 7) {
        cwstep(push);
        cwspin(push, 1);
        cwstep(push);
        }
      if(!passable(push.c, c2, false, true) || !passable(push.c, cwt.c, false, true) || push.c->item) {
        if(checkonly) return false;
        addMessage(XLAT("No room to push %the1.", c2->wall));
        return false;
        }
      if(checkonly) return true;
      addMessage(XLAT("You push %the1.", c2->wall));
      push.c->tmp = c2->tmp;
      if(c2->land == laAlchemist)
        c2->wall = (cwt.c->wall == waFloorB || cwt.c->wall == waFloorA) ? cwt.c->wall : push.c->wall;
      else c2->wall = waNone;
      push.c->wall = w;
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

    if(c2->wall == waBigStatue && !monstersnear(c2) && !c2->monst) {
      if(!canPushStatueOn(cwt.c)) {
        if(checkonly) return false;
        if(cwt.c->wall == waBonfire)
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

    if(c2->wall == waDryTree && !monstersnear(cwt.c)) {
      if(checkonly) return true;
      addMessage(XLAT("You start cutting down the tree."));
      c2->wall = waWetTree;
      }
    else if(c2->wall == waWetTree && !monstersnear(cwt.c)) {
      if(checkonly) return true;
      addMessage(XLAT("You cut down the tree."));
      c2->wall = waNone;
      }
    else if(c2->monst == moKnight) {
      if(checkonly) return false;
      
      bool tooeasy = (roundTableRadius(c2) < newRoundTableRadius());
      if(grailWasFound(cwt.c) && rand() % 5 == 0) {
        addMessage(XLAT("\"I would like to congratulate you again!\""));
        }
      else if(rand() % 5 != 0 && !tooeasy) {
        static int i;
        i++;
        if(i%2) 
          addMessage(XLAT("\"Find the Holy Grail to become one of us!\""));
        else
          addMessage(XLAT("\"The Holy Grail is in the center of the Round Table.\""));
        }
      else {
        int i = rand() % 3;
        if(i == 0)
          addMessage(XLAT("\"I enjoy watching the hyperbug battles.\""));
        if(i == 1)
          addMessage(XLAT("\"Have you visited a temple in R'Lyeh?\""));
        if(i == 2)
          addMessage(XLAT("\"Nice castle, eh?\""));
        }
      return false;
      }
    else if(c2->monst && !isFriendly(c2)) {
      if(c2->monst == moWorm || c2->monst == moWormtail || c2->monst == moWormwait) {
        if(checkonly) return false;
        addMessage(XLAT("You cannot attack Sandworms directly!"));
        return false;
        }
      
      if(attackingForbidden(c2, cwt.c)) {
        if(checkonly) return false;
        addMessage(XLAT("You cannot attack through the Vine!"));
        return false;
        }
      
      if(c2->monst == moTentacle || c2->monst == moTentacletail || c2->monst == moTentaclewait) {
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
      if(c2->monst == moHedge && !items[itOrbThorns]) {
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

      if(checkNeedMove(checkonly))
        return false;
      
      if(checkonly) return true;
      addMessage(XLAT("You kill %the1.", c2->monst));

      int mt = c2->monst;
      int tk = tkills();
      killMonster(c2);
      
      int ntk = tkills();

      if(tk == 0 && ntk > 0)
        addMessage(XLAT("That was easy, but groups could be dangerous."));
        
      if(tk < 10 && ntk >= 10)
        addMessage(XLAT("Good to know that your fighting skills serve you well in this strange world."));

      if(tk < 50 && ntk >= 50)
        addMessage(XLAT("You wonder where all these monsters go, after their death..."));

      if(tk < 100 && ntk >= 100)
        addMessage(XLAT("You feel that the souls of slain enemies pull you to the Graveyard..."));
        
      if(mt == moIvyRoot && ntk>tk)
        achievement_gain("IVYSLAYER");
      
      gomirrors(0);
      }
    else if(!player_passable(c2, cwt.c, true)) {
      if(checkonly) return false;
      if(c2->wall == waFloorA || c2->wall == waFloorB)
        addMessage(XLAT("Wrong color!"));
      else if(c2->wall == waRoundTable)
        addMessage(XLAT("It would be impolite to land on the table!"));
      else
        addMessage(XLAT("You cannot move through %the1!", c2->wall));
      return false;
      }
    else if(c2->land == laGameBoard) {
      // do not pick up!
      if(checkonly) return true;
      flipplayer = true;
      cwstep(cwt);      
      setdist(cwt.c, 0, NULL);
      bfs();
      checkmove();
      return true;
      }
    else {
      if(c2->item == itOrbYendor && !checkYendor(c2, checkonly)) {
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
      
      if(cwt.c->wall == waRoundTable) {
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
      
      int pg = gold();
      bool dopickup = true;

      invismove = items[itOrbInvis] > 0;
      if(items[itOrbFire]) {
        invismove = false;
        firetrail(cwt.c);
        }

      if(items[itOrbDigging]) {
        invismove = false;
        earthMove(cwt.c, d);
        }

      if(c2->item) {
        invismove = false;
        string s0 = "";
        if(0) ;
        if(gold() == 0)
          addMessage(XLAT("Wow! %1! This trip should be worth it!", c2->item));
        else if(gold() == 1)
          addMessage(XLAT("For now, collect as much treasure as possible..."));
        else if(gold() == 2)
          addMessage(XLAT("Prove yourself here, then find new lands, with new quests..."));
        else if(!items[c2->item] && itemclass(c2->item) == IC_TREASURE)
          addMessage(XLAT("You collect your first %1!", c2->item));
        else if(c2->item == itKey)
          addMessage(XLAT("You have found the Key! Now unlock this Orb of Yendor!"));
        else if(c2->item == itGreenStone && !items[itGreenStone])
          addMessage(XLAT("This orb is dead..."));
        else if(c2->item == itGreenStone)
          addMessage(XLAT("Another Dead Orb."));
        else if(itemclass(c2->item) != IC_TREASURE)
          addMessage(XLAT("You have found %the1!", c2->item));
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
        if(items[c2->item] > 67) items[c2->item] = 67;
        }
      else if(c2->item == itOrbLife) {
        placeGolem(cwt.c, c2, moGolem);
        }
      else if(c2->item == itOrbSafety) {
        items[c2->item] += 7;
        activateSafety(c2->land);
        return true;
        }
      else if(c2->item == itOrbLightning) {
        items[c2->item] += 78;
        if(items[c2->item] > 777) items[c2->item] = 777;
        }
      else if(c2->item == itOrbThorns) {
        items[c2->item] += 78;
        if(items[c2->item] > 151) items[c2->item] = 151;
        }
      else if(c2->item == itOrbFlash) {
        items[c2->item] += 78;
        if(items[c2->item] > 777) items[c2->item] = 777;
        }
      else if(c2->item == itOrbShield) {
        items[c2->item] += 16;
        if(items[c2->item] > 77) items[c2->item] = 77;
        }
      else if(c2->item == itOrbWinter) {
        items[c2->item] += 31;
        if(items[c2->item] > 77) items[c2->item] = 77;
        }
      else if(c2->item == itOrbFire) {
        items[c2->item] += 31;
        if(items[c2->item] > 77) items[c2->item] = 77;
        }
      else if(c2->item == itOrbDragon) {
        items[c2->item] += 78;
        if(items[c2->item] > 111) items[c2->item] = 111;
        }
      else if(c2->item == itOrbIllusion) {
        items[c2->item] += 78;
        if(items[c2->item] > 111) items[c2->item] = 111;
        }
      else if(c2->item == itOrbPsi) {
        items[c2->item] += 78;
        if(items[c2->item] > 111) items[c2->item] = 111;
        }
      else if(c2->item == itOrbInvis) {
        items[c2->item] += 31;
        if(items[c2->item] > 77) items[c2->item] = 77;
        }
      else if(c2->item == itOrbGhost) {
        items[c2->item] += 31;
        if(items[c2->item] > 77) items[c2->item] = 77;
        }
      else if(c2->item == itOrbDigging) {
        items[c2->item] += 78;
        if(items[c2->item] > 101) items[c2->item] = 101;
        }
      else if(c2->item == itOrbTeleport) {
        items[c2->item] += 78;
        if(items[c2->item] > 201) items[c2->item] = 201;
        }
      else if(c2->item == itOrbYendor) {
        for(int i=0; i<4; i++) switch(rand() % 12) {
          case 0: items[itOrbSpeed] += 31; break;
          case 1: items[itOrbLightning] += 78; break;
          case 2: items[itOrbFlash] += 78; break;
          case 3: items[itOrbShield] += 31; break;
          case 4: items[itOrbWinter] += 151; break;
          case 5: items[itOrbDigging] += 151; break;
          case 6: items[itOrbTeleport] += 151; break;
          case 7: items[itOrbThorns] += 151; break;
          case 8: items[itOrbInvis] += 151; break;
          case 9: items[itOrbPsi] += 151; break;
          case 10: items[itOrbGhost] += 151; break;
          case 11: items[itOrbFire] += 151; break;
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
        if(!euclid) c2->master->alt->fjordval |= GRAIL_FOUND;
        achievement_collection(c2->item, pg, gold());
        }
      else if(c2->item == itKey) {
        for(int i=0; i<size(yi); i++) if(yi[i].path[YDIST-1] == c2)
          yi[i].found = true;
        items[itKey]++;
        }
      else if(c2->item == itGrimoire && items[itGrimoire] > celldistAlt(c2)/-TEMPLE_EACH) {
        addMessage(XLAT("You already have this Grimoire! Seek new tomes in the inner circles."));
        dopickup = false;
        }
      else {
        bool lhu = hellUnlocked();
        if(c2->item) items[c2->item]++;
        int g2 = gold();
        
        if(c2->item == itHyperstone && items[itHyperstone] == 10)
          achievement_victory(true);
        
        achievement_collection(c2->item, pg, g2);

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
      
      if(items[itOrbWinter] && isIcyLand(cwt.c)) {
        invismove = false;
        if(cwt.c->wall == waNone) 
          cwt.c->wall = waIcewall;
        }
      
      movecost(cwt.c, c2);

      if(c2->monst == moGolem || c2->monst == moIllusion) {
        addMessage(XLAT("You switch places with %the1.", c2->monst));
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

      if(c2->wall == waMirror) {
        invismove = false;
        addMessage(XLAT("The mirror shatters!"));
        if(c2->land == laMirror) {
          int g = gold();
          items[itShard]++;
          achievement_collection(itShard, g+1, g);
          }
        c2->wall = waNone;
        createMirrors(cwt.c, cwt.spin, moMirage);
        }

      if(c2->wall == waGlass && items[itOrbGhost] > 2) {
        addMessage(XLAT("Your Aether powers are drained by %the1!", c2->wall));
        items[itOrbGhost] = 2;
        }
        
      if(c2->wall == waCloud) {
        invismove = false;
        addMessage(XLAT("The cloud turns into a bunch of images!"));
        if(c2->land == laMirror) {
          int g = gold();
          items[itShard]++;
          achievement_collection(itShard, g+1, g);
          }
        c2->wall = waNone;
        createMirages(cwt.c, cwt.spin, moMirage);
        }
      
      if(cellUnstable(c2) && !items[itOrbGhost])
        c2->wall = waChasm;

      countLocalTreasure();
      landvisited[cwt.c->land] = true;
      setdist(cwt.c, 0, NULL);
      }
    }
  else {
    if(checkNeedMove(checkonly))
      return false;
    if(monstersnear(cwt.c)) {
      if(checkonly) return false;
      addMessage(XLAT("%The1 would get you!", which));
      return false;
      }
    if(checkonly) return true;
    if(d == -2 && items[itGreenStone] && cwt.c->item == itNone) {
      items[itGreenStone]--;
      if(false) {
        cwt.c->item = itNone;
        spill(cwt.c, eWall(cwt.c->wall ^ waFloorA ^ waFloorB), 3);
        addMessage(XLAT("The slime reacts with %the1!", itGreenStone));
        }
      else {
        cwt.c->item = itGreenStone;
        addMessage(XLAT("You drop %the1.", itGreenStone));
        }
      }
    else if(d == -2) {
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
  DEBT("bfs");
  bfs();
  destroyStrayMirrors();
  DEBT("heat");
  heat();
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
  cwt.c = dest; cwt.spin = rand() % dest->type; flipplayer = !!(rand() % 2);
  items[itOrbTeleport] = 0;

  addMessage(XLAT("You teleport to a new location!"));
  destroyMirrors();

  for(int i=9; i>=0; i--)
    setdist(cwt.c, i, NULL);

  bfs();
  checkmove();
  }

void psi_attack(cell *dest) {
  addMessage(XLAT("You kill %the1 with a mental blast!", dest->monst));
  killMonster(dest);
  items[itOrbPsi] -= 30;
  if(items[itOrbPsi]<0) items[itOrbPsi] = 0;
  checkmove();
  }

bool flammable(cell *c) {
  return 
    c->wall == waNone || c->wall == waFloorA || c->wall == waFloorB ||
    c->wall == waCavefloor || c->wall == waDeadfloor ||
    c->wall == waDryTree || c->wall == waWetTree ||
    c->wall == waVinePlant || c->wall == waVineHalfA || c->wall == waVineHalfB ||
    c->wall == waIcewall;
  }

void placeDragonfire(cell *c) {
  if(cellUnstable(c))
    c->wall = waChasm;
  else if(c->wall == waNone && c->land == laCocytus)
    c->wall = waLake, c->heat += 20;
  else if(cellHalfvine(c))
    flameHalfvine(c, 20);
  else {
    c->wall = waBonfire;
    c->tmp = 20;
    }
  if(itemBurns(c->item)) 
    addMessage(XLAT("%The1 burns!", c->item)), c->item = itNone;
  addMessage(XLAT("You throw fire!"));
  items[itOrbDragon] -= 5;
  if(items[itOrbDragon]<0) items[itOrbDragon] = 0;
  checkmove();
  }

void placeIllusion(cell *c) {
  c->monst = moIllusion;
  items[itOrbIllusion] -= 5;
  if(items[itOrbIllusion]<0) items[itOrbIllusion] = 0;
  addMessage(XLAT("You create an Illusion!"));
  checkmove();
  }

eItem targetRangedOrb(cell *c, orbAction a) {
  if(!haveRangedOrb()) return itNone;
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
  
  // (1) switch with an illusion
  if(items[itOrbTeleport] && c->monst == moIllusion) {
    if(a != roCheck) teleportTo(c);
    return itOrbTeleport;
    }

  // (2) place illusion
  if(items[itOrbIllusion] && c->monst == moNone && c->item == itNone && passable(c, NULL, false, true)) {
    if(a != roCheck) placeIllusion(c);
    return itOrbIllusion;
    }
  
  // (3) teleport
  if(items[itOrbTeleport] && c->monst == moNone && c->item == itNone && passable(c, NULL, false, true)) {
    if(a != roCheck) teleportTo(c);
    return itOrbTeleport;
    }
    
  // (4) remove an illusion
  if(items[itOrbIllusion] && c->monst == moIllusion) {
    if(a != roCheck) {
      addMessage(XLAT("You take the Illusion away."));
      items[itOrbIllusion] += 4;
      c->monst = moNone;
      }
    return itOrbIllusion;
    }
  
  // (5) psi blast
  if(items[itOrbPsi] && c->monst && !isWorm(c) && c->monst != moShadow) {
    if(a != roCheck) psi_attack(c);
    return itOrbPsi;
    }
  
  // (6) place fire
  if(items[itOrbDragon] && flammable(c)) {
    if(a != roCheck) placeDragonfire(c);
    return itOrbDragon;
    }
  
  if(a == roCheck) return itNone;

  if(items[itOrbPsi] && c->monst) {
    addMessage(XLAT("%The1 is immune to mental blasts!", c->monst));
    }  
  else if(items[itOrbTeleport] && c->monst) {
    addMessage(XLAT("Cannot teleport on a monster!"));
    }
  else if(items[itOrbIllusion] && c->item)
    addMessage(XLAT("Cannot cast illusion on an item!"));
  else if(items[itOrbIllusion] && c->monst)
    addMessage(XLAT("Cannot cast illusion on a monster!"));
  else if(items[itOrbIllusion] && !passable(c, NULL, false, true))
    addMessage(XLAT("Cannot cast illusion here!"));
  else if(items[itOrbTeleport] && c->item) {
    addMessage(XLAT("Cannot teleport on an item!"));
    }
  else if(items[itOrbTeleport] && !passable(c, NULL, false, true)) {
    addMessage(XLAT("Cannot teleport here!"));
    }
  else if(items[itOrbDragon] && !flammable(c)) {
    addMessage(XLAT("Cannot throw fire there!"));
    }
  else return eItem(0);
  
  return eItem(-1);
  }

#define MAXBOX 120
#define POSSCORE 107 // update this when new boxes are added!

struct score {
  string ver;
  int box[MAXBOX];
  };

int savebox[MAXBOX], boxid;
bool saving;

string boxname[MAXBOX];
bool fakebox[MAXBOX];

void applyBox(int& t) {
  if(saving) savebox[boxid++] = t;
  else t = savebox[boxid++];
  }

void applyBoxNum(int& i, string name = "") {
  fakebox[boxid] = (name == "");
  boxname[boxid] = name;
  applyBox(i);
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
  applyBox(items[it]);
  }

void applyBoxM(eMonster m, bool f = false) {
  fakebox[boxid] = f;
  boxname[boxid] = minf[m].name;
  applyBox(kills[m]);
  }

void killbox(eMonster m, int& val) {
  if(saving) kills[m] = val;
  else { val = kills[m]; kills[m] = 0; }
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
    kills[i] = 0;
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
  else savetime = applyBoxLoad();
  
  savecount++; applyBoxNum(savecount, "number of saves"); savecount--;
  applyBoxNum(cheater);

  if(saving) applyBoxSave(cwt.c->land);
  else firstland = eLand(applyBoxLoad());
  
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
  }

void saveBox() {
  boxid = 0; saving = true; applyBoxes();
  }

void loadBox() {
  // have boxid
  boxid = 0; saving = false; applyBoxes();
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
  if(euclid) return;
#ifndef ANDROID

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
    saveBox();
    
    for(int i=0; i<boxid; i++) fprintf(f, " %d", savebox[i]);
    saveCertificate(f);

    fprintf(f, "\n");
    }
  fprintf(f, "Played on: %s - %s (%d turns)\n", sbuf, buf, turncount);
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
          break;
          }
        }
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

void restartGame() {
  DEB("savestats");
  achievement_final(true);
  saveStats();
  DEB("clear");
  for(int i=0; i<ittypes; i++) items[i] = 0;
  for(int i=0; i<motypes; i++) kills[i] = 0;
  for(int i=0; i<10; i++) explore[i] = 0;
  tampered = false; achievementsReceived.clear();
  knighted = 0;
  // items[itGreenStone] = 100;
  cellcount = 0;
  DEB("clearmem");
  clearMemory();
  DEB("initc");
  initcells();
  DEB("initg");
  initgame();
  canmove = true;
  DEB("restg");
  extern void restartGraph();
  restartGraph();
  extern void resetmusic();
  resetmusic();
  }

void restartGameSwitchEuclid() {
  DEB("savestats");
  achievement_final(true);
  saveStats();
  DEB("clear");
  for(int i=0; i<ittypes; i++) items[i] = 0;
  for(int i=0; i<motypes; i++) kills[i] = 0;
  for(int i=0; i<10; i++) explore[i] = 0;
  // items[itGreenStone] = 100;
  cellcount = 0;
  DEB("clearmem");
  clearMemory();
  euclid = !euclid;
  DEB("initc");
  initcells();
  DEB("initg");
  initgame();
  canmove = true;
  DEB("restg");
  extern void restartGraph();
  restartGraph();
  extern void resetmusic();
  resetmusic();
  }

void clearGameMemory() {
  pathq.clear();
  dcal.clear();
  yii = 0; yi.clear();
  clearshadow();
  }

bool fjordwalled[64];

#ifndef ANDROID

void applyGameBoard(char u, int sym, cell *c, char boardmode) {
  if(!c) c = cwt.c;
  if(sym == SDLK_RETURN) {
    items[c->item]++;
    c->item = itNone;
    }
  if(u == ' ')
    c->item = itNone;

  extern void movepckeydir(int);
  if(sym == 'd' || sym == SDLK_KP6) movepckeydir(0);
  if(              sym == SDLK_KP3) movepckeydir(1);
  if(sym == 'x' || sym == SDLK_KP2) movepckeydir(2);
  if(              sym == SDLK_KP1) movepckeydir(3);
  if(sym == 'a' || sym == SDLK_KP4) movepckeydir(4);
  if(              sym == SDLK_KP7) movepckeydir(5);
  if(sym == 'w' || sym == SDLK_KP8) movepckeydir(6);
  if(              sym == SDLK_KP9) movepckeydir(7);

  if(u == 'g')
    c->item = itGreenStone;
  if(u == 'r')
    c->item = itOrbSpeed;
  if(u == 't')
    c->item = itOrbFlash;
  if(u == 'o')
    c->item = itOrbShield;
  if(u == 'i')
    c->item = itOrbTeleport;
  if(u == 'p')
    c->item = itOrbLightning;
  if(u == 'h')
    c->item = itDiamond;
  if(u == 'j')
    c->item = itHell;
  if(u == 'k')
    c->item = itFernFlower;
  if(u == 'l')
    c->item = itSapphire;
  
  if(boardmode == 'f') {
    int fv = fjordval(c);
    if(u == '0' || u == '1' || u == '2' || u == '3' || u == '4') fjordwalled[fv] = !fjordwalled[fv];
    if(u == '1' || u == '4') fjordwalled[fv^1] = !fjordwalled[fv^1];
    if(u == '2' || u == '4') fjordwalled[fv^2] = !fjordwalled[fv^2];
    if(u == '3' || u == '4') fjordwalled[fv^3] = !fjordwalled[fv^3];
    return;
    }
  
  if(u == '0')
    c->wall = waNone;
  if(u == '1')
    c->wall = waFloorA;
  if(u == '2')
    c->wall = waFloorB;
  if(u == '3')
    c->wall = waFrozenLake;
  if(u == '4')
    c->wall = waCavefloor;
  if(u == '5')
    c->wall = waLake;
  if(u == '6')
    c->wall = waCavewall;
  if(u == '7')
    c->wall = waIcewall;
  if(u == '8')
    c->wall = waDryTree;
  if(u == '9')
    c->wall = waColumn;
  }
#endif

static int orbid = 0;

eItem nextOrb() {
  orbid++;
  eItem i = eItem(orbid % ittypes);
  if(itemclass(i) == IC_ORB) return i;
  else return nextOrb();
  }

eItem randomTreasure() {
  eItem i = eItem(rand() % ittypes);
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
    if(q == bq) { cq++; if(rand() % cq == 0) best = i; }
    }
  return best;
  }

extern int webdisplay;

void applyCheat(char u, cell *c = NULL) {
  if(u == 'M' && cwt.c->type == 6) {
    addMessage(XLAT("You summon some Mirages!"));
    cheater++;
    createMirrors(cwt.c, cwt.spin, moMirage),
    createMirages(cwt.c, cwt.spin, moMirage);
    }
  if(u == 'G') {
    addMessage(XLAT("You summon a golem!"));
    cheater++;
    int i = cwt.spin;
    if(passable(cwt.c->mov[i], NULL, false, false)) 
      cwt.c->mov[i]->monst = moGolem;
    }
  if(u == 'L') {
    do {
      firstland = eLand(firstland+1);
      if(firstland == landtypes) firstland = eLand(2);
      }
    while(firstland == laGameBoard || firstland == laCamelot || firstland == laTemple);
    euclidland = firstland;
    cheater++; addMessage(XLAT("You will now start your games in %1", firstland));
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
    }
  if(u == 'P') {
    for(int i=0; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_ORB) 
        items[i] = 0;
    cheater++; addMessage(XLAT("Orb power depleted!"));
    }
  if(u == 'O') {
    cheater++; addMessage(XLAT("Orbs summoned!"));
    for(int i=0; i<cwt.c->type; i++) 
      if(passable(cwt.c->mov[i], NULL, false, false)) {
        eItem it = nextOrb();
        cwt.c->mov[i]->item = it;
        }
    }
  if(u == 'F') {
    items[itOrbFlash] += 1;
    items[itOrbTeleport] += 1;
    items[itOrbLightning] += 1;
    items[itOrbSpeed] += 1;
    items[itOrbShield] += 1;
    cheater++; addMessage(XLAT("Orb power gained!"));
    }
  if(u == 'D') {
    items[itGreenStone] += 10;
    cheater++; addMessage(XLAT("Dead orbs gained!"));
    }
  if(u == 'Y') {
    items[itOrbYendor] ++;
    cheater++; addMessage(XLAT("Orb of Yendor gained!"));
    }
  if(u == 'T') {
    items[randomTreasure2(10)] += 10;
    cheater++; addMessage(XLAT("Treasure gained!"));
    }
  if(u == 'T'-64) {
    items[randomTreasure2(100)] += 100;
    cheater++; addMessage(XLAT("Lots of treasure gained!"));
    }
  if(u == 'W') {
    addMessage(XLAT("You summon a sandworm!"));
    cheater++;
    int i = cwt.spin;
    if(passable(cwt.c->mov[i], NULL, false, false))
      cwt.c->mov[i]->monst = moWorm,
      cwt.c->mov[i]->mondir = NODIR;
    }
  if(u == 'I') {
    addMessage(XLAT("You summon an Ivy!"));
    cheater++;
    int i = cwt.spin;
    int j = cwt.c->spn[i];
    cell* c = cwt.c->mov[i]->mov[(j+3)%cwt.c->mov[i]->type];
    if(passable(c, NULL, false, false)) buildIvy(c, 0, 1);
    }
  if(u == 'E') {
    addMessage(XLAT("You summon a monster!"));
    cheater++;
    int i = cwt.spin;
    if(cwt.c->mov[i]->wall == waChasm)
      cwt.c->mov[i]->wall = waNone;
    if(passable(cwt.c->mov[i], NULL, true, false)) {
      eMonster mo[7] = { moEagle, moPyroCultist, moGhost, moTroll, moKnight, moMiner, moVineBeast };
      cwt.c->mov[i]->monst = mo[rand() % 7];
      }
    }
  if(u == 'H') {
    addMessage(XLAT("You summon some Thumpers!"));
    cheater++;
    for(int i=0; i<cwt.c->type; i++) 
      if(passable(cwt.c->mov[i], NULL, false, false))
        cwt.c->mov[i]->wall = rand() % 2 ? waThumper : waBigStatue, cwt.c->mov[i]->tmp = -1;
    }
  if(u == 'B') {
    addMessage(XLAT("You summon a bonfire!"));
    cheater++;
    int i = cwt.spin;
    if(passable(cwt.c->mov[i], NULL, false, false)) 
      cwt.c->mov[i]->wall = waBonfire, cwt.c->mov[i]->tmp = -1;
    }
  if(u == 'Z') {
    cwt.spin++; flipplayer = false;
    cwt.spin %= cwt.c->type;
    }
  if(u == 'J') {
    if(items[localTreasureType()] > 0)
      items[localTreasureType()] = 0;
    else for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_TREASURE) 
        items[i] = 0;
    cheater++; addMessage(XLAT("Treasure lost!"));
    }
  if(u == 'K') {
    for(int i=0; i<motypes; i++) kills[i] += 10;
    cheater++; addMessage(XLAT("Kills gained!"));
    }
  if(u == 'S') {
    activateSafety(cwt.c->land);
    items[itOrbSafety] += 3;
    cheater++; addMessage(XLAT("Activated Orb of Safety!"));
    }
  if(u == 'U') {
    activateSafety(firstland);
    cheater++; addMessage(XLAT("Teleported to %1!", firstland));
    }
  if(u == 'W'-64) {
    webdisplay++;
    cheater++; addMessage(XLAT("Cheat-changed the display.", firstland));
    }
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

heptagon *createAlternateMap(cell *c, int rad, hstate firststate) {

  // check for direction
  int gdir = -1;
  for(int i=0; i<c->type; i++) {
    if(c->mov[i] && c->mov[i]->mpdist < c->mpdist) gdir = i;
    }
  if(!gdir) return NULL;
  
  // check for non-crossing
  int bd = 2;
  cellwalker bb(c, bd);
  cellwalker bb2 = bb;
  if(!(checkBarriersFront(bb) && checkBarriersBack(bb2))) {
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
      cwspin(bf, 3 + rand() % 2);
    cwstep(bf);
    }
  cx[rad] = bf.c;
  heptagon *h = bf.c->master;
  
  heptagon *alt = new heptagon;
  allAlts.push_back(alt);
//printf("new alt {%p}\n", alt);
  alt->s = firststate;
  alt->fjordval = 0;
  for(int i=0; i<7; i++) alt->move[i] = NULL;
  alt->distance = 0;
  alt->c7 = NULL;
  alt->alt = alt;
  h->alt = alt;
  
  for(int d=rad; d>=0; d--) {
    generateAlts(cx[d]->master);  
    cx[d]->bardir = NOBARRIERS;
    }

  return alt;
//for(int d=rad; d>=0; d--) printf("%3d. %p {%d}\n", d, cx[d]->master, cx[d]->master->alt->distance);
  }
