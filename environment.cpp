// Hyperbolic Rogue - environment
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file environment.cpp
 *  \brief game environment: routines related to the game that affect all the map. Monsters to move are detected here, but their moves are implemented in monstermove.cpp
 */

#include "hyper.h"

namespace hr {

#if HDR
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
#define HF_WESTWALL   Flag(31)
#define HF_JUMP       Flag(32)
#define HF_DICE       Flag(33)
#endif

EX flagtype havewhat, hadwhat;

/** monsters of specific types to move */
EX vector<cell*> worms, ivies, ghosts, golems, hexsnakes;

/** temporary changes during bfs */
vector<pair<cell*, eMonster>> tempmonsters;

/** The position of the first cell in dcal in distance 7. New wandering monsters can be generated in dcal[first7..]. */
EX int first7;           

/** the list of all nearby cells, according to cpdist */
EX vector<cell*> dcal;

/** the list of all nearby cells, according to current pathdist */
EX vector<cellwalker> pathq;

/** the number of big statues -- they increase monster generation */
EX int statuecount;

/** the number of slimes in Wetland -- they create ghosts */
EX int wetslime;

/** list of monsters to move (pathq restriced to monsters) */
EX vector<cell*> pathqm;

/** which hex snakes are there */
EX set<int> snaketypes;

EX int gamerange_bonus = 0;
EX int gamerange() { return getDistLimit() + gamerange_bonus; }

// pathdist begin
EX cell *pd_from;
EX int pd_range;

#if HDR
/** The pathdata is used to keep a list of visited cells. It is used as follows:
 *  1) create pathdata object: pathdata pd(identifier)
 *  2) use one of the following methods to mark cells as visited:
 *  2a) onpath_with_dir or onpath_random_dir, to mark a cell together with its distance and the direction we came from (used by computePathdist to make pathfinding not sensitive to direction indexing)
 *  2b) onpath, to mark a cell at its distance (used when ordering is irrelevant: compute_graphical_distance and in shmup)
 *  2c) onpatk_mark, to just mark a cell (used in groupmove2)
 *  3) All the visited cells are listed in pathq, and they have 'pathdist' set to their recorded distance (0 in case of onpath_mark).
 *  4) When the pathdata object is deleted, all the pathdist values are cleared back to PINFD.
 *  The variable 'pathlock' ensures that we do not use two pathdata objects at once.
 **/

struct pathdata {
  void checklock();
  ~pathdata();
  pathdata(eMonster m, bool include_allies IS(true));
  pathdata(int i);
  };
#endif

/** using pathdata, record a cell (together with direction) as visited */
EX void onpath_with_dir(cellwalker cw, int d) {
  if(!pathlock) {
    println(hlog, "onpath(", cw, ", ", d, ") without pathlock");
    }
  cw.at->pathdist = d;
  pathq.push_back(cw);
  }

/** using pathdata, record a cell as visited, with random direction */
EX void onpath_random_dir(cell *c, int d) {
  onpath_with_dir(cellwalker(c, hrand(c->type), hrand(2)), d);
  }

EX void onpath(cell *c, int d) {
  onpath_with_dir(cellwalker(c, 0, 0), d);
  }

EX void onpath_mark(cell *c) {
  onpath_with_dir(cellwalker(c, 0, 0), 0);
  }

EX void clear_pathdata() {
  for(auto c: pathq) c.at->pathdist = PINFD;
  pathq.clear(); 
  pathqm.clear();
  }

/** This ensures that we do not use two pathdata objects at once */
EX int pathlock = 0;

/** compute_graphical_distance determines the distance of every cell
 *  from the current FOV center. It uses the pathq structures but
 *  does not lock them */

EX void compute_graphical_distance() {
  if(pathlock) { printf("path error: compute_graphical_distance\n"); }
  cell *c1 = centerover ? centerover : pd_from ? pd_from : cwt.at;
  int sr = get_sightrange_ambush();
  if(pd_from == c1 && pd_range == sr) return;
  clear_pathdata();
  
  pathlock++;
  pd_from = c1;
  pd_range = sr;
  onpath(c1, 0);

  for(int qb=0; qb<isize(pathq); qb++) {
    cell *c = pathq[qb].at;
    if(c->pathdist == pd_range) break;
    if(qb == 0) forCellCM(c1, c) ;
    forCellEx(c1, c)
      if(c1->pathdist == PINFD)
        onpath(c1, c->pathdist + 1);
    }

  pathlock--;
  }

const int max_radius = 16;

struct visit_set {
  set<cell*> visited;
  queue<cell*> q;
  void visit(cell *c) {
    if(visited.count(c)) return;
    visited.insert(c);
    q.push(c);
    }
  };

struct princess_ai {
  array<visit_set, max_radius+1> info;
  void visit_gate(cell *g) { info[0].visit(g); }
  void run();
  };

void princess_ai::run() {
  int radius = toggle_radius(waOpenPlate);
  if(pathq.empty()) return;
  int d = pathq.back().at->pathdist;
  if(d == PINFD - 1) return;
  d++;
  if(d < 5) d = 5; /* the Princess AI avoids plates when too close to the player */
  hassert(radius <= max_radius);

  for(int k=0; k<=radius; k++) while(!info[k].q.empty()) {
    cell *c = info[k].q.front();
    info[k].q.pop();
    if(k < radius) forCellEx(c1, c) {
      info[k+1].visit(c1);
      if(k == 0 && c1->wall == waClosedGate)
        info[0].visit(c1);
      }
    if(k == radius && c->wall == waOpenPlate && c->pathdist == PINFD)
      onpath_random_dir(c, d);
    }
  }

EX void computePathdist(eMonster param, bool include_allies IS(true)) {
  
  for(cell *c: targets)
    if(include_allies || isPlayerOn(c))
      onpath_random_dir(c, isPlayerOn(c) ? 0 : 1);

  int qtarg = isize(targets);
  
  int limit = gamerange();
  
  int qb = 0;

  bool princess = isPrincess(param);
  princess_ai gd;
  princess_retry:
  
  for(; qb < isize(pathq); qb++) {
    cellwalker cw = pathq[qb];
    /* The opposite cell will be added to the queue first, which helps the AI. */
    cw += cw.at->type/2;
    cell*& c = cw.at;
    if(c->monst && !isBug(c) && !(isFriendly(c) && !c->stuntime)) {
      pathqm.push_back(c); 
      continue; // no paths going through monsters
      }
    if(isMounted(c) && !isPlayerOn(c)) {
      // don't treat the Worm you are riding as passable
      pathqm.push_back(c); 
      continue; 
      }
    if(c->cpdist > limit && !(c->land == laTrollheim && turncount < c->landparam) && c->wall != waThumperOn) continue;
    int d = c->pathdist;
    if(d == PINFD - 1) continue;
    for(int j=0; j<c->type; j++) {
      cellwalker cw1 = cw + j;
      // printf("i=%d cd=%d\n", i, c->move(i)->cpdist);
      cell *c2 = cw1.peek();
      
      flagtype f = P_MONSTER;
      if(param == moTameBomberbird) f |= P_FLYING | P_ISFRIEND;
      if(isPrincess(param)) f |= P_ISFRIEND | P_USEBOAT | P_CHAIN;
      if(param == moGolem) f |= P_ISFRIEND;
      bool pass = c2 && c2->pathdist == PINFD;
      if(pass && qb < qtarg && !nonAdjacent(c, c2) && !thruVine(c,c2)) pass = passable(c2, NULL, f);
      else pass = pass && passable(c, c2, f);

      if(pass) {
        
        if(qb >= qtarg) {
          if(param == moTortoise && nogoSlow(c, c2)) continue;
          if(param == moIvyRoot  && strictlyAgainstGravity(c, c2, false, MF_IVY)) continue;
          if(param == moWorm && (cellUnstable(c) || cellEdgeUnstable(c) || prairie::no_worms(c))) continue;
          if(!isFriendly(param) && items[itOrbLava] && c2->cpdist <= 5 && pseudohept(c) && makeflame(c2, 1, true))
            continue;
          }

        onpath_with_dir(cw1 + wstep, d+1);
        }
      
      else if(c2 && c2->wall == waClosedGate && princess)
        gd.visit_gate(c2);
      }
    }
  
  if(princess) {
    gd.run(); 
    if(qb < isize(pathq)) goto princess_retry;
    }
  }

pathdata::~pathdata() {
  pathlock--;
  clear_pathdata();
  }

void pathdata::checklock() {
  if(pd_from) pd_from = NULL, clear_pathdata();
  if(pathlock) printf("path error\n");
  pathlock++;
  }

pathdata::pathdata(int i) { checklock(); }

pathdata::pathdata(eMonster m, bool include_allies IS(true)) {
  checklock();
  if(isize(pathq))
    println(hlog, "! we got tiles on pathq: ", isize(pathq));

  computePathdist(m, include_allies);
  }

// pathdist end

/** additional direction information for BFS algorithms.
 *  It remembers from where we have got to this location
 *  the opposite cell will be added to the queue first,
 *  which helps the AI. Used in bfs().
 **/
EX vector<int> bfs_reachedfrom;

/** calculate cpdist, 'have' flags, and do general fixings */
EX void bfs() {

  calcTidalPhase(); 
    
  yendor::onpath();
  
  int dcs = isize(dcal);
  for(int i=0; i<dcs; i++) dcal[i]->cpdist = INFD;
  worms.clear(); ivies.clear(); ghosts.clear(); golems.clear(); 
  tempmonsters.clear(); targets.clear(); 
  statuecount = 0;
  wetslime = 0;
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
  
  dcal.clear(); bfs_reachedfrom.clear();

  recalcTide = false;
  
  for(cell *c: player_positions()) {
    if(c->cpdist == 0) continue;
    c->cpdist = 0;
    checkTide(c);
    dcal.push_back(c);
    bfs_reachedfrom.push_back(hrand(c->type));
    if(!invismove) targets.push_back(c);
    }
  
  int distlimit = gamerange();

  for(cell *c: player_positions()) {
    if(items[itOrbDomination])
    if(c->monst == moTentacle || c->monst == moTentaclewait || c->monst == moTentacleEscaping)
      worms.push_back(c);
    }
  
  int qb = 0;
  first7 = 0;
  while(true) {
    if(qb == isize(dcal)) break;
    int i, fd = bfs_reachedfrom[qb] + dcal[qb]->type/2;
    cell *c = dcal[qb++];
    
    int d = c->cpdist;
    
    if(WDIM == 2 && d == distlimit) { first7 = qb; break; }

    for(int j=0; j<c->type; j++) if(i = (fd+j) % c->type, c->move(i)) {
      // printf("i=%d cd=%d\n", i, c->move(i)->cpdist);
      cell *c2 = c->move(i);
      if(!c2) continue;
      
      if(isWarpedType(c2->land)) havewhat |= HF_WARP;
      if(c2->land == laMirror) havewhat |= HF_MIRROR;
      
      if((c->wall == waBoat || c->wall == waSea) &&
        (c2->wall == waSulphur || c2->wall == waSulphurC))
        c2->wall = waSea;
      
      if(c2 && signed(c2->cpdist) > d+1) {
        if(WDIM == 3 && !gmatrix.count(c2)) {
          if(!first7) first7 = qb;
          continue;
          }
        c2->cpdist = d+1;
        
        // remove treasures
        if(!peace::on && c2->item && c2->cpdist == distlimit && itemclass(c2->item) == IC_TREASURE &&
          c2->item != itBabyTortoise && WDIM != 3 &&
          (items[c2->item] >= (ls::any_chaos()?10:20) + currentLocalTreasure || getGhostcount() >= 2)) {
            c2->item = itNone;
            if(c2->land == laMinefield) { c2->landparam &= ~3; }
            }
            
        if(c2->item == itBombEgg && c2->cpdist == distlimit && items[itBombEgg] >= c2->landparam) {
          c2->item = itNone;
          c2->landparam |= 2;
          c2->landparam &= ~1;
          if(!c2->monst) c2->monst = moBomberbird, c2->stuntime = 0;
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

        if(c2->item == itWest && c2->cpdist == distlimit && items[itWest] >= c2->landparam + 4)
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
        bfs_reachedfrom.push_back(c->c.spin(i));
        
        checkTide(c2);
                
        if(c2->wall == waBigStatue && c2->land != laTemple) 
          statuecount++;
        
        if(isAlch(c2->wall) && c2->land == laWet)
          wetslime++;
          
        if(cellHalfvine(c2) && isWarped(c2)) {
          addMessage(XLAT("%The1 is destroyed!", c2->wall));
          destroyHalfvine(c2);
          }
        
        if(c2->wall == waCharged) elec::havecharge = true;
        if(isElectricLand(c2)) elec::haveelec = true;
        
        if(c2->land == laWhirlpool) havewhat |= HF_WHIRLPOOL;
        if(c2->land == laWhirlwind) havewhat |= HF_WHIRLWIND;
        if(c2->land == laWestWall) havewhat |= HF_WESTWALL;
        if(c2->land == laPrairie) havewhat |= HF_RIVER;
        if(c2->land == laClearing) havewhat |= HF_MUTANT;

        if(c2->wall == waRose) havewhat |= HF_ROSE;
        
        if((hadwhat & HF_ROSE) && (rosemap[c2] & 3)) havewhat |= HF_ROSE;
        
        if(c2->monst) {
          if(isHaunted(c2->land) && 
            c2->monst != moGhost && c2->monst != moZombie && c2->monst != moNecromancer)
            fail_survivalist();
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
          else if(among(c2->monst, moFrog, moVaulter, moPhaser))
            havewhat |= HF_JUMP;
          else if(c2->monst == moReptile) havewhat |= HF_REPTILE;
          else if(isLeader(c2->monst)) havewhat |= HF_LEADER;
          else if(c2->monst == moEarthElemental) havewhat |= HF_EARTH;
          else if(c2->monst == moWaterElemental) havewhat |= HF_WATER;
          else if(c2->monst == moVoidBeast) havewhat |= HF_VOID;
          else if(c2->monst == moHunterDog) havewhat |= HF_HUNTER;
          else if(isMagneticPole(c2->monst)) havewhat |= HF_MAGNET;
          else if(c2->monst == moAltDemon) havewhat |= HF_ALT;
          else if(c2->monst == moHexDemon) havewhat |= HF_HEXD;
          else if(among(c2->monst, moAnimatedDie, moAngryDie)) havewhat |= HF_DICE;
          else if(c2->monst == moMonk) havewhat |= HF_MONK;
          else if(c2->monst == moShark || c2->monst == moCShark || among(c2->monst, moRusalka, moPike)) havewhat |= HF_SHARK;
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

  for(int i=first7; i<isize(dcal); i++)
    forCellEx(c2, dcal[i])
      if(c2->wall == waThumperOn) {
        targets.push_back(c2);
        }

  while(recalcTide) {
    recalcTide = false;
    for(int i=0; i<isize(dcal); i++) checkTide(dcal[i]);
    }    
  
  for(auto& t: tempmonsters) t.first->monst = t.second;
  
  buildAirmap();
  }

EX void moverefresh(bool turn IS(true)) {
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
      if(turn) c->stuntime--;
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
      
      if(c->monst && !survivesChasm(c->monst) && c->monst != moReptile && normal_gravity_at(c)) {
        if(c->monst != moRunDog && c->land == laMotion) 
          achievement_gain_once("FALLDEATH1");
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
          vector<int> gooddirs;
          // in the peace mode, a reptile will
          // prefer to walk on the ground, rather than the chasm
          for(int i=0; i<c->type; i++) {
            int i0 = (i+3) % c->type;
            int i1 = (i+c->type-3) % c->type;
            if(c->move(i0) && passable(c->move(i0), c, 0)) 
            if(c->move(i1) && passable(c->move(i1), c, 0)) 
              gooddirs.push_back(i);
            }
          c->mondir = hrand_elt(gooddirs, c->mondir);
          playSound(c, "click");
          }
        }
      }

    else if(isFire(c)) {
      if(c->monst == moSalamander) c->stuntime = max<int>(c->stuntime, 1);
      else if(c->monst == moVaulter && c->mondir == JUMP)
        c->mondir = NODIR;
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
      if(c->monst && !survivesWater(c->monst) && normal_gravity_at(c)) {
        playSound(c, "splash"+pick12());
        if(isNonliving(c->monst))
          addMessage(XLAT("%The1 sinks!", c->monst));
        else 
          addMessage(XLAT("%The1 drowns!", c->monst));
        if(isBull(c->monst)) {
          addMessage(XLAT("%The1 is filled!", c->wall));
          c->wall = waShallow;
          }
        fallMonster(c, AF_FALL);
        }
      }
    else if(c->wall == waSulphur || c->wall == waSulphurC || c->wall == waMercury) {
      if(c->monst && !survivesPoison(c->monst, c->wall) && normal_gravity_at(c)) {
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
      else if(c->monst && !survivesPoison(c->monst, c->wall) && normal_gravity_at(c)) {
        if(isNonliving(c->monst))
          addMessage(XLAT("%The1 is destroyed by lava!", c->monst));
        else 
          addMessage(XLAT("%The1 is killed by lava!", c->monst));
        playSound(c, "steamhiss", 70);
        fallMonster(c, AF_FALL);
        }
      }
    else if(!isWateryOrBoat(c) && c->wall != waShallow) {
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

// find worms and ivies
EX void settemp(cell *c) {
  tempmonsters.emplace_back(c, (eMonster) c->monst);
  c->monst = moNone;
  }

EX void findWormIvy(cell *c) {
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
  
EX void monstersTurn() {
  reset_spill();
  checkSwitch();
  mirror::breakAll();
  DEBB(DF_TURN, ("bfs"));
  bfs();
  DEBB(DF_TURN, ("charge"));
  if(elec::havecharge) elec::act();
  DEBB(DF_TURN, ("mmo"));
  int phase2 = (1 & items[itOrbSpeed]);
  if(!phase2) movemonsters();

  for(cell *pc: player_positions()) if(pc->item == itOrbSafety)  {
    collectItem(pc, pc, true);
    return;
    }

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
  DEBB(DF_TURN, ("rop"));
  if(!dual::state) reduceOrbPowers();
  int phase1 = (1 & items[itOrbSpeed]);
  if(dual::state && items[itOrbSpeed]) phase1 = !phase1;
  DEBB(DF_TURN, ("lc"));
  if(!phase1) livecaves();
  if(!phase1) ca::simulate();
  if(!phase1) heat::processfires();
  
  for(cell *c: crush_now) {
    changes.ccell(c);
    playSound(NULL, "closegate");
    if(canAttack(c, moCrusher, c, c->monst, AF_GETPLAYER | AF_CRUSH)) {
      attackMonster(c, AF_MSG | AF_GETPLAYER | AF_CRUSH, moCrusher);
      }
    moveEffect(movei(c, FALL), moDeadBird);
    destroyBoats(c, NULL, true);
    explodeBarrel(c);
    }
  
  changes.value_keep(crush_now);
  changes.value_keep(crush_next);
  crush_now = std::move(crush_next);
  crush_next.clear();
  
  DEBB(DF_TURN, ("heat"));
  heat::processheat();
  // if(elec::havecharge) elec::drawcharges();

  orbbull::check();

  #if CAP_COMPLEX2
  if(!phase1) terracotta::check();
  #endif
  
  if(items[itOrbFreedom])
    for(cell *pc: player_positions())
      checkFreedom(pc);

  DEBB(DF_TURN, ("check"));
  checkmove();
  if(canmove) elec::checklightningfast();


#if CAP_HISTORY
  for(cell *pc: player_positions())
    history::movehistory.push_back(pc);
#endif
  }

/** check if whirlline is looped, if yes, remove the repeat; may not detect loops immediately */
EX bool looped(vector<cell*>& whirlline) {
  if(isize(whirlline) == 1)
    return false;
  if(whirlline.back() == whirlline.front()) {
    whirlline.pop_back();
    return true;
    }
  int pos = isize(whirlline)/2;
  if(isize(whirlline) > 2 && whirlline.back() == whirlline[pos]) {
    while(pos && whirlline.back() == whirlline[pos])
      whirlline.pop_back();
    /* something weird must have happened... */
    static bool once = true;
    if(once) addMessage("warning: a looped line");
    once = false;
    return true;
    }
  return false;
  }

}
