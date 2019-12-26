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
#endif

EX flagtype havewhat, hadwhat;

/** monsters of specific types to move */
EX vector<cell*> worms, ivies, ghosts, golems, hexsnakes;

/** temporary changes during bfs */
vector<pair<cell*, eMonster>> tempmonsters;

/** additional direction information for BFS algorithms.
 *  It remembers from where we have got to this location
 *  the opposite cell will be added to the queue first,
 *  which helps the AI.
 **/
EX vector<int> reachedfrom;

/** The position of the first cell in dcal in distance 7. New wandering monsters can be generated in dcal[first7..]. */
EX int first7;           

/** the list of all nearby cells, according to cpdist */
EX vector<cell*> dcal;
/** the list of all nearby cells, according to current pathdist */
EX vector<cell*> pathq;

/** the number of big statues -- they increase monster generation */
EX int statuecount;

/** list of monsters to move (pathq restriced to monsters) */
EX vector<cell*> pathqm;

/** which hex snakes are there */
EX set<int> snaketypes;

EX int gamerange_bonus = 0;
EX int gamerange() { return getDistLimit() + gamerange_bonus; }

// pathdist begin
EX cell *pd_from;
EX int pd_range;

EX void onpath(cell *c, int d) {
  c->pathdist = d;
  pathq.push_back(c);
  }

EX void onpath(cell *c, int d, int sp) {
  c->pathdist = d;
  pathq.push_back(c);
  reachedfrom.push_back(sp);
  }

EX void clear_pathdata() {
  for(auto c: pathq) c->pathdist = PINFD;
  pathq.clear(); 
  pathqm.clear();
  reachedfrom.clear(); 
  }

EX int pathlock = 0;

EX void compute_graphical_distance() {
  if(pathlock) { printf("path error: compute_graphical_distance\n"); }
  cell *c1 = centerover ? centerover : pd_from ? pd_from : cwt.at;
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

EX void computePathdist(eMonster param) {
  
  for(cell *c: targets)
    onpath(c, isPlayerOn(c) ? 0 : 1, hrand(c->type));

  int qtarg = isize(targets);
  
  int limit = gamerange();

  for(int qb=0; qb < isize(pathq); qb++) {
    cell *c = pathq[qb];
    int fd = reachedfrom[qb] + c->type/2;
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

#if HDR
struct pathdata {
  void checklock() { 
    if(pd_from) pd_from = NULL, clear_pathdata();
    if(pathlock) printf("path error\n"); 
    pathlock++; 
    }
  ~pathdata() {
    pathlock--;
    clear_pathdata();
    }
  pathdata(eMonster m) { 
    checklock();
    computePathdist(m); 
    }
  pathdata(int i) { 
    checklock();
    }
  };
#endif
// pathdist end

/** calculate cpdist, 'have' flags, and do general fixings */
EX void bfs() {

  calcTidalPhase(); 
    
  yendor::onpath();
  
  int dcs = isize(dcal);
  for(int i=0; i<dcs; i++) dcal[i]->cpdist = INFD;
  worms.clear(); ivies.clear(); ghosts.clear(); golems.clear(); 
  tempmonsters.clear(); targets.clear(); 
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
  first7 = 0;
  while(true) {
    if(qb == isize(dcal)) break;
    int i, fd = reachedfrom[qb] + 3;
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
        if(c2->land == laWestWall) havewhat |= HF_WESTWALL;
        if(c2->land == laPrairie) havewhat |= HF_RIVER;
        if(c2->land == laClearing) havewhat |= HF_MUTANT;

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
      
      if(c->monst && !survivesChasm(c->monst) && c->monst != moReptile && normal_gravity_at(c)) {
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
      if(c->monst && !survivesWater(c->monst) && normal_gravity_at(c)) {
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
  checkSwitch();
  mirror::breakAll();
  DEBB(DF_TURN, ("bfs"));
  bfs();
  DEBB(DF_TURN, ("charge"));
  if(elec::havecharge) elec::act();
  DEBB(DF_TURN, ("mmo"));
  int phase2 = (1 & items[itOrbSpeed]);
  if(!phase2) movemonsters();
  for(int i=0; i<numplayers(); i++) if(playerpos(i)->item == itOrbSafety) {
    collectItem(playerpos(i), true);
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
    playSound(NULL, "closegate");
    if(canAttack(c, moCrusher, c, c->monst, AF_GETPLAYER | AF_CRUSH)) {
      attackMonster(c, AF_MSG | AF_GETPLAYER | AF_CRUSH, moCrusher);
      }
    moveEffect(movei(c, FALL), moDeadBird);
    destroyBoats(c, NULL, true);
    explodeBarrel(c);
    }
  
  crush_now = move(crush_next);
  crush_next.clear();
  
  DEBB(DF_TURN, ("heat"));
  heat::processheat();
  // if(elec::havecharge) elec::drawcharges();

  orbbull::check();

  if(!phase1) terracotta::check();
  
  if(items[itOrbFreedom])
    for(int i=0; i<numplayers(); i++)
      if(multi::playerActive(i))
        checkFreedom(playerpos(i));

  DEBB(DF_TURN, ("check"));
  checkmove();
  if(canmove) elec::checklightningfast();


#if CAP_HISTORY
  for(int i=0; i<numplayers(); i++)
    if(multi::playerActive(i))
      history::movehistory.push_back(playerpos(i));
#endif
  }

}
