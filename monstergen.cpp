// Hyperbolic Rogue

// Copyright (C) 2011-2017 Zeno Rogue, see 'hyper.cpp' for details

// Routines concentrating on monster generation.

bool timerghost = true;

int buildIvy(cell *c, int children, int minleaf) {
  if(c->monst) return 0;
  c->mondir = NODIR;
  c->monst = moIvyRoot;
  
  cell *child = NULL;

  int leaf = 0;
  int leafchild = 0;
  for(int i=0; i<c->type; i++) {
    createMov(c, i);
    if(passable(c->mov[i], c, 0) && c->mov[i]->land == c->land) {
      if(children && !child) 
        child = c->mov[i], leafchild = buildIvy(c->mov[i], children-1, 5);
      else 
        c->mov[i]->monst = (leaf++ || peace::on) ? moIvyWait : moIvyHead,
        c->mov[i]->mondir = c->spn(i);
      }
    }
  
  leaf += leafchild;
  if(leaf < minleaf) {
    if(child) killIvy(child, moNone);
    killIvy(c, moNone);
    return 0;
    }
  else return leaf;
  }

// the 'chasmify' functions create a simulation of the path the monster came by

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
    c3->mondir = c->spn(d);
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
    if(!c3->monst) {
      c3->wall = waElementalD;
      for(int i=0; i<c3->type; i++) {
        cell *c4 = createMov(c3, i);
        if(c4->wall != waBarrier) c4->wall = waNone;
        }
      c3->mondir = c->spn(d);
      }
    }
  c->wall = getElementalWall(c->land);
  c->wparam = 100; c->item = itNone;
  }

// an appropriate monster for the Crossroads

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
  eMonster mo = m[hrand(24)];
  if(peace::on && mo == moWaterElemental) return crossroadsMonster();
  if(peace::on && mo == moFireFairy) return crossroadsMonster();
  if(peace::on && isMultitile(mo)) return crossroadsMonster();
  return mo;
  }

eMonster wanderingCrossroadsMonster() {
  while(true) {
    eMonster m = crossroadsMonster();
    if(!isIvy(m) && m != moTentacle) return m;
    }
  }

int palaceHP() {
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

int hardness_empty() {
  return yendor::hardness() * (yendor::hardness() * 3/5 - 2);
  }

bool redtrolls(cell *c) {
  return false; /*
  int cd = getCdata(c, 2);
  cd &= 63;
  return cd < 32; */
  }

eMonster pickTroll(cell *c) { 
  if(redtrolls(c))
    return pick(moTroll,moDarkTroll,moRedTroll);
  else
    return pick(moForestTroll, moStormTroll, moFjordTroll);
  }

void dieTroll(cell *c, eMonster m) {
  if(m == moTroll) c->wall = waDeadTroll;
  else if(m == moDarkTroll) c->wall = waDeadfloor2;
  else if(m == moRedTroll) c->wall = waRed1;
  else c->wall = waDeadTroll2, c->wparam = m;
  }

int reptilemax() {
  int i = items[itDodeca] + yendor::hardness();
  if(i >= 245) return 5;
  int r = (250 - i);
  if(shmup::on) r = (r+2) / 3;
  return r;
  }

bool wchance(int a, int of, int reduction = 0) {
  of *= 10; 
  a += yendor::hardness() + 1;
  if(isCrossroads(cwt.c->land)) 
    a+= items[itHyperstone] * 10;

//if(cwt.c->land == laWhirlwind && !nowhirl) a += items[itWindstone] * 3;

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
      cell *c3 = c->mov[i];
      if(c3 && c3 != c2 && c3->land == laZebra && c3->wall == waNone)
        ctab[q++] = c3;
      }
    if(!q) break;
    c2 = c; c = ctab[hrand(q)];
    }
  }

int getGhostTimer() {
  return shmup::on ? (shmup::curtime - lastexplore) / 350 : turncount - lastexplore;
  }

int getGhostcount() {
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

bool canReachPlayer(cell *cf, eMonster m) {
  vector<cell*> v;
  sval++;
  v.push_back(cf); cf->aitmp = sval;
  for(int i=0; i<size(v); i++) {
    cell *c = v[i];
    for(int j=0; j<c->type; j++) {
      cell *c2 = c->mov[j];
      if(!c2) continue;
      if(eq(c2->aitmp, sval)) continue;
      if(!passable_for(m, c2, c, P_MONSTER | P_ONPLAYER | P_CHAIN)) continue;
      if(isPlayerOn(c2)) return true;
      c2->aitmp = sval; v.push_back(c2);
      }
    }
  return false;
  }

bool haveOrbPower() {
  for(int i=0; i<ittypes; i++) if(itemclass(eItem(i)) == IC_ORB && items[i]) return true;
  if(quotient) for(int i=0; i<size(dcal); i++) {
    cell *c = dcal[i];
    if(itemclass(c->item) == IC_ORB) return true;
    }
  else if(sphere) for(int i=0; i<spherecells(); i++) {
    cell *c = getDodecahedron(i)->c7;
    if(itemclass(c->item) == IC_ORB) return true;
    forCellEx(c2, c) if(itemclass(c2->item) == IC_ORB) return true;
    }
  return false;
  }

bool haveKraken() {
  for(int i=0; i<spherecells(); i++) {
    cell *c = getDodecahedron(i)->c7;
    if(c->monst == moKrakenH || c->monst == moKrakenT) return true;
    }
  return false;
  }

eItem wanderingTreasure(cell *c) {
  eLand l = c->land;
  if(l == laEFire) return itFireShard;
  if(l == laEWater) return itWaterShard;
  if(l == laEAir) return itAirShard;
  if(l == laEEarth) return itEarthShard;
  if(l == laElementalWall) return itNone;
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

void wandering() {
  if(!canmove) return;
  int seepcount = getSeepcount();
  int ghostcount = getGhostcount();
  if(cwt.c->land == laCA) ghostcount = 0;

  if(cwt.c->land == laZebra && cwt.c->wall == waNone && wchance(items[itZebra], 20))
    wanderingZebra(cwt.c);
  
  if(smallbounded) {
    int maxdist = 0;
    for(int i=0; i<size(dcal); i++) if(dcal[i]->cpdist > maxdist) maxdist = dcal[i]->cpdist;
    for(int i=0; i<size(dcal); i++) if(dcal[i]->cpdist >= maxdist-1) { first7 = i; break; }
    
    if(hrand(5) == 0) {
      // spawn treasure
      }
    }
  
  while(first7 < size(dcal)) {
    int i = first7 + hrand(size(dcal) - first7);
    cell *c = dcal[i];
    if(inmirror(c)) continue;
    
    if(smallbounded && !c->item && hrand(5) == 0 && c->land != laHalloween) {
      if(passable(c, NULL, 0) || specialland == laKraken) {
        if(!haveOrbPower() && specialland != laHell) for(int it=0; it<1000 && !c->item; it++)
          placeLocalOrbs(c);
        if(!c->item) c->item = wanderingTreasure(c);
        if(c->item == itShard) {
          c->item = itNone, c->wall = hrand(2) ? waMirror : waCloud;
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
        
    if((c->wall == waCavewall || c->wall == waDeadwall) && !c->monst &&
      wchance(items[treasureType(c->land)], 10) && canReachPlayer(c, moSlime)) {
      c->monst = moSeep;
      playSeenSound(c);
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
      if(c->land == laOcean && (items[itCoast] > 50 || (cwt.c->landparam < 25 && c->landparam < 25)) && wchance(items[itCoast], 25) && canReachPlayer(c, moEagle)) {
        c->monst = moAlbatross;
        playSeenSound(c);
        continue;
        }
      if(!peace::on && c->land == laLivefjord && wchance(items[itFjord], 80) && items[itFjord] >= 10 && canReachPlayer(c, moWaterElemental)) {
        c->monst = moWaterElemental;
        playSeenSound(c);
        continue;
        }
      if(!peace::on && c->land == laKraken && ((sphere && !hrand(15)) || wchance(items[itKraken], 240)) && !pseudohept(c)) {
        bool b = canReachPlayer(c, moKrakenH);
        if(sphere && (haveKraken() || !items[itOrbFish])) { 
          c->monst = moViking; c->wall = waBoat; c->item = itOrbFish; 
          playSeenSound(c);
          continue;
          }        
        if(b) forCellEx(c2, c) if((sphere || c2->cpdist > 7) && !pseudohept(c2)) {
          forCellCM(c3, c2) if(c3->monst || c3->wall != waSea) 
            goto notfound;
          c2->monst = moKrakenH;
          playSeenSound(c2);
          for(int i=0; i<c2->type; i++) {
            c2->mov[i]->monst = moKrakenT;
            c2->mov[i]->hitpoints = 1;
            c2->mov[i]->mondir = c2->spn(i);
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
    
    else if(c->monst || c->pathdist == PINFD) break;
    
    else if(c->land == laClearing && wchance(items[itMutant2], 150) && items[itMutant2] >= 15 && !c->monst && c->type == 7) 
      c->monst = moRedFox;

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

    else if(c->land == laRedRock && wchance(items[itRedGem], 10))
      c->monst = (hrand(10) || peace::on) ? moRedTroll : moHexSnake;

    else if(c->land == laCaves && wchance(items[itGold], 5))
      c->monst = hrand(3) ? moTroll : moGoblin;

    else if(c->land == laBull && wchance(items[itBull], 40))
      c->monst = moGadfly;

    else if(items[itBull] >= 50 && size(butterflies) && wchance(items[itBull]-49, 25))
      c->monst = moGadfly;

    else if(c->land == laPrairie && cwt.c->LHU.fi.flowerdist > 3 && wchance(items[itGreenGrass], prairie::isriver(cwt.c) ? 150 : 40))
      c->monst = moGadfly;

    else if(c->land == laHive && wchance(hive::hivehard(), 25))
      c->monst = hive::randomHyperbug();

    else if(c->land == laDeadCaves && wchance(items[itSilver], 5))
      c->monst = hrand(20) ? (hrand(3) ? moDarkTroll : moGoblin) : moEarthElemental;

    else if(c->land == laJungle && wchance(items[itRuby], 40))
      c->monst = hrand(10) ? moMonkey : moEagle;

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

    else if(c->land == laTerracotta && wchance(items[itTerra], 35)) 
      c->monst = moJiangshi;

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
      c->monst = sphere ? pick(moViking, moWaterElemental, moFjordTroll) : moViking;
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
      c->hitpoints = palaceHP();
      }
    

    else break;
      
    playSeenSound(c);
    if(c->monst == moWorm || c->monst == moHexSnake) c->mondir = NODIR;
      
    // laMotion -> no respawn!
    }
  }

