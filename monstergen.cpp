// Hyperbolic Rogue -- monster generation
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file monstergen.cpp
 *  \brief monster generation
 */

#include "hyper.h"
namespace hr {

EX int avengers, mirrorspirits, wandering_jiangshi, jiangshi_on_screen;

EX bool timerghost = true;
EX bool gen_wandering = true;

EX int buildIvy(cell *c, int children, int minleaf) {
  if(c->monst) return 0;
  c->mondir = NODIR;
  c->monst = moIvyRoot;
  c->monmirror = nonorientable && hrand(2);
  
  cell *child = NULL;

  int leaf = 0;
  int leafchild = 0;
  for(int i=0; i<c->type; i++) {
    createMov(c, i);
    if(passable(c->move(i), c, 0) && c->move(i)->land == c->land) {
      if(children && !child) 
        child = c->move(i), leafchild = buildIvy(c->move(i), children-1, 5);
      else 
        c->move(i)->monst = (leaf++ || peace::on) ? moIvyWait : moIvyHead,
        c->move(i)->mondir = c->c.spin(i),
        c->move(i)->monmirror = c->monmirror;
      }
    }
  
  leaf += leafchild;
  if(leaf < minleaf) {
    if(child) killIvy(child, moNone);
    dynamicval<int> k(kills[moIvyDead]);
    killIvy(c, moNone);
    return 0;
    }
  else return leaf;
  }

/** the 'chasmify' functions create a simulation of the path the monster came by */
EX void chasmify(cell *c) {
  c->wall = waChasm; c->item = itNone;
  int q = 0;
  cell *c2[10];
  for(int i=0; i<c->type; i++) if(c->move(i) && c->move(i)->mpdist > c->mpdist && cellUnstable(c->move(i)))
    c2[q++] = c->move(i);
  if(q) {
    cell *c3 = c2[hrand(q)];
    c3->wall = waChasmD;
    }
  }

EX void chasmifyEarth(cell *c) {
  int q = 0;
  int d2[10];
  for(int i=2; i<=c->type-2; i++) {
    int j = (i+c->mondir)%c->type;
    cell *c2 = c->move(j);
    if(c2 && c2->mpdist > c->mpdist && (
      c2->wall == waDeadfloor || c2->wall == waDeadwall ||
      c2->wall == waDeadfloor2)) 
      d2[q++] = j;
    }
  if(!q) printf("no further move!\n");
  if(q) {
    int d = d2[hrand(q)];
    cell *c3 = c->move(d);
    c3->wall = waEarthD;
    for(int i=0; i<c3->type; i++) {
      cell *c4 = createMov(c3, i);
      earthFloor(c4);
      }
    c3->mondir = c->c.spin(d);
    }
  earthWall(c); c->item = itNone;
  }

EX void chasmifyElemental(cell *c) {
  int q = 0;
  int d2[10];
  for(int i=2; i<=c->type-2; i++) {
    int j = (i+c->mondir)%c->type;
    cell *c2 = c->move(j);
    if(c2 && c2->mpdist > c->mpdist && c2->land == c->land)
      d2[q++] = j;
    }
  if(q) {
    int d = d2[hrand(q)];
    cell *c3 = c->move(d);
    if(!c3->monst) {
      c3->wall = waElementalD;
      for(int i=0; i<c3->type; i++) {
        cell *c4 = createMov(c3, i);
        if(c4->wall != waBarrier) c4->wall = waNone;
        }
      c3->mondir = c->c.spin(d);
      }
    }
  c->wall = getElementalWall(c->land);
  c->wparam = 100; c->item = itNone;
  }

/** generate a monster appropriate for the Crossroads */

EX eMonster crossroadsMonster() {

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
  eMonster mo = m[hrand(24)];
  if(peace::on && mo == moWaterElemental) return crossroadsMonster();
  if(peace::on && mo == moFireFairy) return crossroadsMonster();
  if(peace::on && isMultitile(mo)) return crossroadsMonster();
  return mo;
  }

EX eMonster wanderingCrossroadsMonster() {
  while(true) {
    eMonster m = crossroadsMonster();
    if(!isIvy(m) && m != moTentacle) return m;
    }
  }

EX int palaceHP() {
  if(tactic::on && isCrossroads(firstland))
    return 4;
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

EX int hardness_empty() {
  return yendor::hardness() * (yendor::hardness() * 3/5 - 2);
  }

EX bool redtrolls(cell *c) {
  return false; /*
  int cd = getCdata(c, 2);
  cd &= 63;
  return cd < 32; */
  }

EX eMonster pickTroll(cell *c) { 
  if(redtrolls(c))
    return pick(moTroll,moDarkTroll,moRedTroll);
  else
    return pick(moForestTroll, moStormTroll, moFjordTroll);
  }

EX void dieTroll(cell *c, eMonster m) {
  if(m == moTroll) c->wall = waDeadTroll;
  else if(m == moDarkTroll) c->wall = waDeadfloor2;
  else if(m == moRedTroll) c->wall = waRed1;
  else c->wall = waDeadTroll2, c->wparam = m;
  }

EX int reptilemax() {
  int i = items[itDodeca] + yendor::hardness();
  if(i >= 245) return 5;
  int r = (250 - i);
  if(shmup::on) r = (r+2) / 3;
  return r;
  }

bool wchance(int a, int of, int reduction = 0) {
  of *= 10; 
  a += yendor::hardness() + 1;
  if(isCrossroads(cwt.at->land)) 
    a+= items[itHyperstone] * 10;

//if(cwt.at->land == laWhirlwind && !nowhirl) a += items[itWindstone] * 3;

  for(int i=0; i<ittypes; i++) if(itemclass(eItem(i)) == IC_TREASURE)
    a = max(a, (items[i]-R10) / 10);
  
  a -= reduction;
  if(a < 0) return false;

  return hrand(a+of) < a;
  }

void wanderingZebra(cell *start) {
  cell *c = start, *c2 = start;
  for(int it=0; it<100; it++) {
    if(c->cpdist == getDistLimit()) {
      c->monst = moOrangeDog;
      c->stuntime = 0;
      return;
      }
    int q = 0;
    cell *ctab[8];
    for(int i=0; i<c->type; i++) {
      cell *c3 = c->move(i);
      if(c3 && c3 != c2 && c3->land == laZebra && c3->wall == waNone)
        ctab[q++] = c3;
      }
    if(!q) break;
    c2 = c; c = ctab[hrand(q)];
    }
  }

EX int getGhostTimer() {
  return shmup::on ? (shmup::curtime - lastexplore) / 350 : turncount - lastexplore;
  }

EX int getGhostcount() {
  if(peace::on) return 0;
  int t = getGhostTimer();
  int ghostcount = 0;
  if(t > 80) ghostcount = (t-80 + hrand(20)) / 20;
  return ghostcount;
  }

int getSeepcount() {
  int t = getGhostTimer();
  int seepcount = 0;
  if(t > 40) seepcount = (t-40 + hrand(20)) / 20;
  return seepcount;
  }

EX bool canReachPlayer(cell *cf, eMonster m) {
  manual_celllister cl;
  cl.add(cf);
  for(int i=0; i<isize(cl.lst) && i < 10000; i++) {
    cell *c = cl.lst[i];
    bool found = false;
    
    auto test = [&] (cell *c2) {
      if(cl.listed(c2)) return;
      if(!passable_for(m, c2, c, P_MONSTER | P_ONPLAYER | P_CHAIN)) return;
      if(isPlayerOn(c2)) found = true;
      cl.add(c2);
      };

    forCellEx(c2, c) {
      if(frog_power(m)) forCellEx(c3, c2) test(c3);
      test(c2);
      }
    
    if(found) return true;
    }
  return false;
  }

EX bool haveOrbPower() {
  for(int i=0; i<ittypes; i++) if(itemclass(eItem(i)) == IC_ORB && items[i]) return true;
  if(quotient) for(int i=0; i<isize(dcal); i++) {
    cell *c = dcal[i];
    if(itemclass(c->item) == IC_ORB) return true;
    }
  else if(sphere_narcm && WDIM == 2 && !INVERSE) for(int i=0; i<spherecells(); i++) {
    cell *c = getDodecahedron(i)->c7;
    if(itemclass(c->item) == IC_ORB) return true;
    forCellEx(c2, c) if(itemclass(c2->item) == IC_ORB) return true;
    }
  return false;
  }

EX bool haveKraken() {
  for(int i=0; i<spherecells(); i++) {
    cell *c = getDodecahedron(i)->c7;
    if(c->monst == moKrakenH || c->monst == moKrakenT) return true;
    }
  return false;
  }

EX eItem wanderingTreasure(cell *c) {
  eLand l = c->land;
  #if CAP_DAILY
  if(daily::on && daily::prevent_spawn_treasure_on(c)) return itNone;
  #endif
  if(l == laEFire) return itFireShard;
  if(l == laEWater) return itWaterShard;
  if(l == laEAir) return itAirShard;
  if(l == laEEarth) return itEarthShard;
  if(l == laElementalWall) return itNone;
  if(l == laAsteroids) return itNone;
  if(l == laMirror && c->type != 6) return itNone;
  if(l == laMirrorOld && c->type != 6) return itNone;
  if(l == laEmerald) {
    forCellEx(c2, c) if(c2->wall == waCavewall) return itNone;
    }
  if(l == laMinefield && c->wall == waMineMine) return itNone;
  if(l == laBurial && hrand(2)) return itOrbSword;
  if(l == laKraken) return itOrbFish;
  return treasureType(l);
  }

/** generate the wandering monsters */
EX void wandering() {
  #if CAP_COMPLEX2
  if(mine::in_minesweeper()) {
    mine::count_status();
    return;
    }
  #endif
  if(!canmove) return;
  if(!gen_wandering) return;
  if(racing::on) return;
  if(items[itOrbSafety]) return;
  pathdata pd(moYeti);
  int seepcount = getSeepcount();
  int ghostcount = getGhostcount();
  if(cwt.at->land == laCA) ghostcount = 0;
  bool genturn = hrand(100) < 30;
  
  if(closed_or_bounded && specialland == laClearing)
    clearing::new_root();

  if(cwt.at->land == laZebra && cwt.at->wall == waNone && wchance(items[itZebra], 20))
    wanderingZebra(cwt.at);

  bool smallbounded_generation = smallbounded || (closed_manifold && specialland == laClearing);

  auto valid = [] (cell *c) { if(disksize && !is_in_disk(c)) return false; if(inmirror(c)) return false; return true; };

  if(smallbounded_generation) {
    int maxdist = 0;
    for(int i=0; i<isize(dcal); i++) if(valid(dcal[i])) if(dcal[i]->cpdist > maxdist) maxdist = dcal[i]->cpdist;
    for(int i=0; i<isize(dcal); i++) if(valid(dcal[i])) if(dcal[i]->cpdist >= maxdist-1) { first7 = i; break; }
    
    if(hrand(5) == 0) {
      // spawn treasure
      }
    
    if(smallbounded && hrand(100) < 2) {
      auto& ac = currentmap->allcells();
      cell *c1 = ac[hrand(isize(ac))];
      if(c1->wall == waVinePlant && !c1->monst) {
        c1->monst = moVineSpirit;
        c1->stuntime = 3;
        }
      }
    }
  
  int iter = 0;
  while(first7 < isize(dcal)) {
    iter++; if(iter > 1000) break;
    int i = first7 + hrand(isize(dcal) - first7);
    cell *c = dcal[i];
    if(!valid(c)) continue;
    if(isPlayerOn(c)) break;
    
    if(specialland == laStorms) {
      // place the sandstone wall completely randomly (but not on the player)
      vector<cell*>& ac = currentmap->allcells();
      c = ac[hrand(isize(ac))];
      if(isPlayerOn(c)) continue;
      }
    
    if(smallbounded_generation && !c->item && hrand(5) == 0 && c->land != laHalloween) {
      if(passable(c, NULL, 0) || specialland == laKraken) {
        if(c->land != laGraveyard && !in_lovasz() && !haveOrbPower() && specialland != laHell) for(int it=0; it<1000 && !c->item; it++)
          placeLocalOrbs(c);
        if(!c->item) c->item = wanderingTreasure(c);
        if(c->item == itShard) {
          c->item = itNone;
          mirror::build(c);
          }
        if(c->item == itFulgurite) {
          c->item = itNone, c->wall = waSandstone;
          }
        if(c->item == itBarrow) 
          c->landparam = 2 + hrand(2),
          c->wall = waBarrowDig;
        }
      }

    if(!c->monst) c->stuntime = 0;

    if(timerghost && !smallbounded) {
      // wandering seeps & ghosts
      if(seepcount && c->wall == waCavewall && !c->monst && canReachPlayer(c, moSlime)) {
        c->monst = moSeep;
        playSeenSound(c);
        seepcount--;
        continue;
        }
      
      if(ghostcount && !c->monst && !inmirror(c)) {
        c->monst = moGhost;
        playSeenSound(c);
        ghostcount--;
        continue;
        }
      }
    
    if(c->land == laWet && !smallbounded && wetslime >= 25 && !c->monst && hrand(100) <= wetslime-25) {
      static bool angry = false;
      if(!angry) { angry = true; addMessage("You seem to have really pissed off the water spirits!"); }
      c->monst = moGhost;
      playSeenSound(c);
      continue;
      }
        
    else if((c->wall == waCavewall || c->wall == waDeadwall) && !c->monst &&
      wchance(items[treasureType(c->land)], 10) && canReachPlayer(c, moSlime)) {
      c->monst = moSeep;
      playSeenSound(c);
      continue;
      }
    
    else if(smallbounded && c->wall == waVinePlant && !c->monst && wchance(items[treasureType(c->land)], 10) && canReachPlayer(c, moSlime)) {
      c->monst = moVineSpirit;
      playSeenSound(c);
      continue;
      }
    
    else if(c->land == laOcean && cwt.at->land == laOcean && cwt.at->landparam > 25 && c->landparam > 25 && !tactic::on && !yendor::on && hrand(100) < 2) {
      c->monst = moPirate; c->wall = waBoat; c->item = itOrbSafety; 
      continue;
      }

    else if(c->wall == waCTree && !c->monst && wchance(items[itPirate], 15) && canReachPlayer(c, moSlime)) {
      c->monst = moParrot;
      playSeenSound(c);
      continue;
      }

    else if(c->land == laEndorian && c->wall == waNone && wchance(items[itApple], 50)) {
      c->monst = moSparrowhawk;
      playSeenSound(c);
      continue;
      }
    
    #if CAP_COMPLEX2
    else if(c->land == laBrownian && wchance(items[itBrownian], 75)) {
      c->monst = moAcidBird;
      continue;
      }
    
    else if(c->land == laVariant && wchance(items[itVarTreasure], 50)) {
      int i = hrand(21);
      if(getBits(c) & (1>>i)) {
        eMonster m = variant::features[i].wanderer;
        if(m) c->monst = m, c->hitpoints = 3;
        }
      continue;
      }
    #endif

    else if(c->land == laFrog && !c->monst && wchance(items[itFrog], 25)) {
      eMonster m = pick(moFrog, moPhaser, moVaulter);
      if(canReachPlayer(c, m)) {
        c->monst = m;
        playSeenSound(c);
        continue;
        }
      }
    
    else if(c->land == laWet && among(c->wall, waDeepWater, waShallow) && !c->monst && wchance(items[itWet], 15) && canReachPlayer(c, moShark)) {
        c->monst = hrand(100) < 10 ? moRusalka : moPike;
        playSeenSound(c);
        continue;
        }

    else if(c->wall == waSea && !c->monst) {
      if(c->land == laCaribbean && wchance(items[itPirate], 15) && canReachPlayer(c, moPirate)) {
        c->monst = moCShark;
        playSeenSound(c);
        continue;
        }
      if(c->land == laWarpSea && avengers && canReachPlayer(c, moPirate)) {
        c->monst = moRatlingAvenger;
        c->wall = waBoat;
        avengers--;
        if(cheater) printf("avenger comes\n");
        playSeenSound(c);
        continue;
        }
      if(c->land == laWarpSea && wchance(items[itCoral], 25) && canReachPlayer(c, moPirate)) {
        c->monst = moRatling;
        c->wall = waBoat;
        playSeenSound(c);
        continue;
        }
      if(c->land == laOcean && (items[itCoast] > 50 || ((cwt.at->land != laOcean || cwt.at->landparam < 25) && c->landparam < 25)) && wchance(items[itCoast], 25) && canReachPlayer(c, moEagle)) {
        c->monst = moAlbatross;
        playSeenSound(c);
        continue;
        }
      if(c->land == laDocks && wchance(items[itDock], 25) && canReachPlayer(c, moEagle)) {
        c->monst = moAlbatross;
        playSeenSound(c);
        continue;
        }
      if(!peace::on && c->land == laLivefjord && wchance(items[itFjord], 80) && items[itFjord] >= 10 && canReachPlayer(c, moWaterElemental)) {
        c->monst = moWaterElemental;
        playSeenSound(c);
        continue;
        }
      if(!peace::on && c->land == laKraken && ((sphere && !hrand(15)) || wchance(items[itKraken], 240)) && !kraken_pseudohept(c)) {
        bool b = sphere || canReachPlayer(c, moKrakenH);
        if(sphere_narcm && WDIM == 2 && (haveKraken() || !items[itOrbFish])) { 
          c->monst = moViking; c->wall = waBoat; c->item = itOrbFish; 
          playSeenSound(c);
          continue;
          }        
        if(b) forCellEx(c2, c) if((sphere || c2->cpdist > gamerange()) && !kraken_pseudohept(c2)) {
          forCellCM(c3, c2) if(c3->monst || c3->wall != waSea) 
            goto notfound;
          c2->monst = moKrakenH;
          c2->stuntime = 0;
          playSeenSound(c2);
          for(int i=0; i<c2->type; i++) {
            c2->move(i)->monst = moKrakenT;
            c2->move(i)->hitpoints = 1;
            c2->move(i)->stuntime = 0;
            c2->move(i)->mondir = c2->c.spin(i);
            }
          goto found;
          }
        goto notfound;
        found:
        continue;
        }
      notfound:
      break;
      }
    
    else if(smallbounded && c->land == laPower && !c->monst) {
      if(wchance(items[itPower], 10))
        c->monst = eMonster(moWitch + hrand(NUMWITCH));
      }
    
    else if(c->monst || c->pathdist == PINFD) break;
    
    else if(c->land == laAsteroids) {
      int gen = 0;
      if(asteroids_generated * 12 <= items[itAsteroid]) gen = 2;
      if(gen == 0) {
        int qty = 0;
        for(cell *c: currentmap->allcells()) if(c->monst == moAsteroid) qty++;
        if(!qty) gen = 1;
        }
      if(gen) c->monst = moAsteroid, c->hitpoints = 4;
      if(gen == 2) 
        asteroids_generated++;
      
      if(items[itAsteroid] > (asteroid_orbs_generated+2) * (asteroid_orbs_generated+3) && !c->item) {
        c->item = pick(itOrbThorns, itOrbSide1, itOrbSide2, itOrbSide3, itOrbShield, itOrbLife);
        asteroid_orbs_generated++;
        }

      break;
      }
    
    else if(c->land == laClearing && wchance(items[itMutant2], 150) && items[itMutant2] >= 15 && !c->monst && c->type == 7) 
      c->monst = moRedFox;

    else if(c->land == laDual && wchance(items[itGlowCrystal], 40)) {
      c->monst = moRatling;
      playSeenSound(c);
      }

    else if(hrand(50) < statuecount * statuecount) 
      c->monst = moCultistLeader;

    else if(c->land == laIce && wchance(items[itDiamond], 10))
      c->monst = hrand(2) ? moWolf : moYeti;

    else if(c->land == laHunting && wchance(items[itHunting], 50, 26))
      c->monst = moHunterDog;

    else if(c->land == laDesert && wchance(items[itSpice], 10))
      c->monst = (hrand(10) || peace::on) ? moDesertman : moWorm;

    else if(c->land == laDragon && (items[itDragon] >= 8 || items[itOrbYendor]) && wchance(items[itDragon], 20))
      c->monst = moFireElemental;

    else if(c->land == laRedRock && wchance(items[itRedGem], 10)) {
      if (hrand(10) || peace::on)
        c->monst = moRedTroll;
      else if (!pseudohept(c))
        c->monst = moHexSnake, c->hitpoints = 1;
      }

    else if(c->land == laCaves && wchance(items[itGold], 5))
      c->monst = hrand(3) ? moTroll : moGoblin;

    else if(c->land == laBull && wchance(items[itBull], 40))
      c->monst = moGadfly;

    else if(items[itBull] >= 50 && isize(butterflies) && wchance(items[itBull]-49, 25))
      c->monst = moGadfly;

    else if(c->land == laPrairie && cwt.at->LHU.fi.flowerdist > 3 && wchance(items[itGreenGrass], prairie::isriver(cwt.at) ? 150 : 40))
      c->monst = moGadfly;

    else if(c->land == laHive && wchance(hive::hivehard(), 25))
      c->monst = hive::randomHyperbug();

    else if(c->land == laDeadCaves && wchance(items[itSilver], 5))
      c->monst = hrand(20) ? (hrand(3) ? moDarkTroll : moGoblin) : moEarthElemental;

    else if(c->land == laJungle && wchance(items[itRuby], 40))
      c->monst = hrand(10) ? moMonkey : moEagle;

    else if(c->land == laCursed && wchance(items[itCursed], 40))
      c->monst = moHexer;

    else if(c->land == laMirrorOld && wchance(items[itShard], 15))
      c->monst = hrand(10) ? moRanger : moEagle;

    else if(c->land == laMirror && mirrorspirits) {
      mirrorspirits--;
      c->monst = moMirrorSpirit;
      }
    
    else if(c->land == laMirror && wchance(items[itShard], 120))
      c->monst = moNarciss;

    else if(c->land == laWarpCoast && wchance(items[itCoral], 40))
      c->monst = moRatling;

    else if(c->land == laBurial && wchance(items[itBarrow], 250))
      c->monst = moDraugr;

    else if(c->land == laBlizzard && wchance(items[itBlizzard], 120))
      c->monst = hrand(5) ? moVoidBeast : moIceGolem;

    else if(c->land == laVolcano && wchance(items[itLavaLily], 120)) {
      c->monst = hrand(5) ? moLavaWolf : moSalamander;
      c->hitpoints = 3;
      }

    else if(c->land == laTrollheim && wchance(items[itTrollEgg], 150))
      c->monst = pickTroll(c);

    else if(c->land == laRose && wchance(items[itRose], 25))
      c->monst = moFalsePrincess;

    else if(c->land == laHell && wchance(items[itHell], 20))
      c->monst = hrand(3) ? moLesser : moGreater;

    else if(c->land == laStorms && wchance(items[itFulgurite], 30)) {
      c->monst = hrand(3) ? moMetalBeast : moStormTroll;
      c->hitpoints = 3;
      }

    else if(c->land == laWhirlwind && wchance(items[itWindstone], 30)) 
      c->monst = hrand(5) ? moWindCrow : moAirElemental;

    else if(c->land == laWildWest && wchance(items[itBounty], 30)) 
      c->monst = moOutlaw;

    else if(c->land == laEndorian && c->wall == waTrunk && wchance(items[itApple], 30)) 
      c->monst = moResearcher;

    else if(c->land == laOvergrown && wchance(items[itMutant], 50)) 
      c->monst = moForestTroll;

    else if(c->land == laTerracotta && wchance(items[itTerra], 40)) 
      c->monst = moJiangshi;

    else if(c->land == laTerracotta && wandering_jiangshi && genturn) 
      wandering_jiangshi--, c->monst = moJiangshi;

    else if(c->land == laSwitch && wchance(items[itSwitch], 80)) 
      c->monst = active_switch();

    else if(c->land == laRuins && wchance(items[itRuins], 80)) {
      c->monst = genRuinMonster(c);
      c->hitpoints = 3;
      }

    else if(c->land == laEclectic && wchance(items[itEclectic], 20)) {
      gen_eclectic_monster(c);
      }

    else if(c->land == laCaribbean && wchance(items[itPirate], 30))
      c->monst = moPirate;

    else if(c->land == laRlyeh && wchance(items[itStatue], 15))
      c->monst = hrand(3) ? moPyroCultist : 
        (hrand(40) < items[itStatue]-25) ? moCultistLeader : moCultist;

    else if(c->land == laGraveyard && wchance(items[itBone], 15))
      c->monst = hrand(5) ? moGhost : moNecromancer;
      
    else if(isHaunted(c->land) && wchance(items[itLotus], 15))
      c->monst = moGhost;
      
    else if(c->land == laDryForest && wchance(items[itFernFlower], 5))
      c->monst = hrand(5) ? moHedge : moFireFairy;
      
    else if(c->land == laCocytus && wchance(items[itSapphire], 45))
      c->monst = moCrystalSage;
      
    else if(c->land == laAlchemist && wchance(items[itElixir], 3) && canReachPlayer(c, moSlime) && c->item == itNone)
      c->monst = moSlime; // ?
    
    else if(isElemental(c->land) && wchance(items[itElemental], 20) && !peace::on)
      c->monst = elementalOf(c->land);
    
    else if(c->land == laIvoryTower && wchance(items[itIvory], 20))
      c->monst = cellEdgeUnstable(c) ? moGargoyle : moFamiliar;
    
    else if(c->land == laMinefield && wchance(items[itBombEgg]-20, 400))
      c->monst = moBomberbird;
    
    else if(c->land == laEmerald && wchance(items[itEmerald], 5))
      c->monst = emerald_monster();
    
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
      c->monst = sphere ? pick(moViking, moWaterElemental, moFjordTroll) : moViking;
      }
    
    else if(c->land == laOcean && wchance(items[itCoast], 100)) {
      c->monst = moAlbatross;
      }
    
    else if(c->land == laPower && wchance(items[itPower], 10)) {
      c->monst = eMonster(moWitch + hrand(NUMWITCH));
      }
    
    else if(c->land == laCamelot && hrand(30) == 0 && (euclid || c->master->alt) && celldistAltRelative(c) < 0)
      c->monst = camelot_monster();
    
    else if(isCrossroads(c->land) && items[itHyperstone] && wchance(items[itHyperstone], 20)) {
      c->monst = wanderingCrossroadsMonster();
      c->hitpoints = palaceHP();
      }
    

    else break;
      
    playSeenSound(c);
    if(c->monst == moWorm || c->monst == moHexSnake) c->mondir = NODIR;
      
    // laMotion -> no respawn!
    }
  }

EX void generateSnake(cell *c, int i, int snakecolor) {
  c->monst = moHexSnake;
  c->hitpoints = snakecolor;
  int cpair = (1<<pattern_threecolor(c)) | (1<<pattern_threecolor(c->move(i)));
  preventbarriers(c);
  int len = BITRUNCATED ? ROCKSNAKELENGTH : 2;
  cell *c2 = c;
  vector<cell*> rocksnake;
  while(--len) {
    rocksnake.push_back(c2);
    preventbarriers(c2);
    c2->mondir = i;
    createMov(c2, i);
    int j = c2->c.spin(i);
    cell *c3 = c2->move(i);
    if(c3->monst || c3->bardir != NODIR || c3->wall) break;
    c2 = c3;
    c2->monst = moHexSnakeTail; c2->hitpoints = snakecolor;
    int t = c2->type;
    if(mhybrid) t -= 2;
    i = (j + (t%4 == 0 ? t/2 : (len%2 ? 2 : t - 2))) % t;
    createMov(c2, i);
    if(!inpair(c2->move(i), cpair)) {
      vector<int> goodsteps;
      {for(int i=0; i<t; i++)
        if(inpair(c2->cmove(i), cpair))
        goodsteps.push_back(i);}
      if(!isize(goodsteps)) break;
      i = goodsteps[hrand(isize(goodsteps))];
      }
    }
  if(isize(rocksnake) < ROCKSNAKELENGTH/2 && BITRUNCATED) {
    for(int i=0; i<isize(rocksnake); i++) 
      rocksnake[i]->monst = moNone;
    }
  else c2->mondir = NODIR;
  }
}
