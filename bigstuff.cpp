// This file implements:
// * routines related to (horo)cycles
// * routines related to equidistants
// * 'setland' routines for other geometries
// * the buildBigStuff function which calls equidistant/(horo)cycle/barrier generators.
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// horocycles

int newRoundTableRadius() {
  return 28 + 2 * items[itHolyGrail];
  }

int getAnthraxData(cell *c, bool b) {
  int d = celldistAlt(c);
  int rad = 28 + 3 * anthraxBonus;
  while(d < -rad) { 
    d += rad + 12;
    rad += 3;
    }
  while(d >= 12) {
    if(rad > 5) rad -= 3;
    else if(rad) rad--;
    d -= rad + 12;
    }
  if(b) return rad;
  return d;
  }

int roundTableRadius(cell *c) {
  if(euclid) return 28;
  if(tactic::on) return getAnthraxData(c, true);
  return c->master->alt->alt->emeraldval & GRAIL_RADIUS_MASK;
  }

int celldistAltRelative(cell *c) {
  if(sphere || quotient) {
    return celldist(c) - 3;
    }
  if(tactic::on) return getAnthraxData(c, false);
  return celldistAlt(c) - roundTableRadius(c);
  }

int euclidAlt(short x, short y) {
  if(specialland == laTemple || specialland == laClearing) {
    if(euclid6)
      return max(int(x), x+y);
    else if(nonbitrunc)
      return x + abs(y);
    else
      return max(x, y);
    }
  else if(specialland == laCaribbean || specialland == laWhirlpool || specialland == laMountain) {
    if(euclid6)
      return 
        min(
          min(max(int(-x), -x-y) + 3,
          max(int(x+y), int(y)) + 3),
          max(int(x), int(-y)) + 3
          );
    else if(nonbitrunc)
      return 3 - min(abs(x-y), abs(x+y));
    else
      return 3 - min(abs(x), abs(y));
    }
  else if(specialland == laPrincessQuest)
    return eudist(x-EPX, y-EPY);
  else return eudist(x-(a4 ? 21 : 20), y-10);
  }

const int NOCOMPASS = 1000000;

int compassDist(cell *c) {
  if(sphere || quotient) return 0;
  if(euclid || c->master->alt) return celldistAlt(c);
  if(isHaunted(c->land) || c->land == laGraveyard) return getHauntedDepth(c);
  return NOCOMPASS;
  }

cell *findcompass(cell *c) {
  int d = compassDist(c);
  if(d == NOCOMPASS) return NULL;
  
  while(inscreenrange(c)) {
    if(!euclid && !sphere && !quotient)
      generateAlts(c->master);
    forCellEx(c2, c) if(compassDist(c2) < d) {
      c = c2;
      d = compassDist(c2);
      goto nextk;
      }
    break;
    nextk: ;
    }
  
  return c;
  }

bool grailWasFound(cell *c) {
  if(euclid || quotient || sphere) return items[itHolyGrail];
  return c->master->alt->alt->emeraldval & GRAIL_FOUND;
  }

void generateAlts(heptagon *h, int levs, bool link_cdata) {
  if(!h->alt) return;
  preventbarriers(h->c7);
  for(int i=0; i<S7; i++) preventbarriers(h->c7->mov[i]);
  if(gp::on)
    for(int i=0; i<S7; i++) preventbarriers(createStep(h, i)->c7);
  for(int i=0; i<S7; i++) 
    createStep(h->alt, i)->alt = h->alt->alt;
  int relspin = -4; // for horocycles it must go the other way
  if(quotient) relspin = 0;
  else {
  for(int j=0; j<S7; j++) for(int i=0; i<S7; i++) {
    createStep(h, i);
    if(h->move[i]->alt == h->alt->move[j]) {
      relspin = (i-j+S7) % S7;
      break;
      }
    }
  if(relspin == -4 && quotient != 2) {
    if(h->alt != h->alt->alt) {
      printf("relspin {%p:%p}\n", h->alt, h->alt->alt);
      {for(int i=0; i<S7; i++) printf("%p ", h->alt->move[i]);} printf(" ALT\n");
      {for(int i=0; i<S7; i++) printf("%p ", h->move[i]);} printf(" REAL\n");
      {for(int i=0; i<S7; i++) printf("%p ", h->move[i]->alt);} printf(" REAL ALT\n");
      }
    relspin = 3;
    } }
  // h[relspin] matches alt[0]
//printf("{%d~%d}\n", h->distance, h->alt->distance);
  for(int i=0; i<S7; i++) {
    int ir = (S7+i-relspin)%S7;
    heptagon *hm = h->alt->move[ir];
    heptagon *ho = createStep(h, i);
//  printf("[%p:%d ~ %p:%d] %p ~ %p\n", 
//    h, i, h->alt, ir, 
//    ho, hm);
    if(ho->alt && ho->alt != hm) {
      if(ho->alt->alt == hm->alt && !quotient) {
        printf("ERROR: alt cross! [%d -> %d]\n", ho->alt->distance, hm->distance);
        // exit(1);
        }
      continue;
      }
    ho->alt = hm;
    if(link_cdata) hm->cdata = (cdata*) ho;
    if(levs) generateAlts(ho, levs-1, link_cdata);
    }
  }

heptagon *createAlternateMap(cell *c, int rad, hstate firststate, int special) {

  // check for direction
  int gdir = -1;
  for(int i=0; i<c->type; i++) {
    if(c->mov[i] && c->mov[i]->mpdist < c->mpdist) gdir = i;
    }
  if(gdir < 0) return NULL;
  
  // non-crossing in weird hyperbolic
  if(weirdhyperbolic) {
    if(c->bardir == NOBARRIERS) return NULL;
    forCellEx(c1, c) if(c1->bardir == NOBARRIERS) return NULL;
    }
  
  // check for non-crossing
  int bd = 2;
  cellwalker bb(c, bd);
  if(!weirdhyperbolic && !(checkBarriersFront(bb) && checkBarriersBack(bb))) {
    return NULL;
    }

  // okay, let's go then!
  cellwalker bf(c, gdir);
  cell *cx[rad+1];
  for(int i=0; i<rad; i++) {
    cx[i] = bf.c;
    bf += revstep;
    }
  cx[rad] = bf.c;
  heptagon *h = bf.c->master;

  if(h->alt) {
    printf("Error: creatingAlternateMap while one already exists\n");
    return NULL;
    }
  
  if(special == waPalace) {  
  
    // type 7 is ensured
    cell *c = bf.c;
    
    if(cdist50(c) != 0) return NULL;
    if(polarb50(c) != 1) return NULL;
    }
  
  heptagon *alt = new heptagon;
  allmaps.push_back(newAltMap(alt));
//printf("new alt {%p}\n", alt);
  alt->s = firststate;
  alt->emeraldval = 0;
  alt->zebraval = 0;
  for(int i=0; i<MAX_EDGE; i++) alt->move[i] = NULL;
  alt->distance = 0;
  alt->c7 = NULL;
  alt->alt = alt;
  h->alt = alt;
  alt->cdata = (cdata*) h;
  
  for(int d=rad; d>=0; d--) {
    generateAlts(cx[d]->master);  
    preventbarriers(cx[d]);
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

    if(princess::gotoPrincess && cheater) princess::gotoPrincess=false, cwt.c = c;
    }
  
  return alt;
//for(int d=rad; d>=0; d--) printf("%3d. %p {%d}\n", d, cx[d]->master, cx[d]->master->alt->distance);
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
  cell* ctab[MAX_EDGE];
  int qc = 0, qlo, qhi;
  for(int i=0; i<c->type; i++) {
    cell *c2 = createMov(c, i);
    if(!euclid) generateAlts(c2->master);
    if((euclid || (c->master->alt && c2->master->alt)) && celldistAlt(c2) < celldistAlt(c)) {
      ctab[qc++] = c2;
      qlo = i; qhi = i;
      while(true && qc < MAX_EDGE) {
        qlo--;
        c2 = createMovR(c, qlo);
        if(!euclid && !c2->master->alt) break;
        if(celldistAlt(c2) >= celldistAlt(c)) break;
        ctab[qc++] = c2;
        }
      while(true && qc < MAX_EDGE) {
        qhi++;
        c2 = createMovR(c, qhi);
        if(!euclid && !c2->master->alt) break;
        if(celldistAlt(c2) >= celldistAlt(c)) break;
        ctab[qc++] = c2;
        }
      break;
      }
    }
  if(!qc) { 
    printf("NO QC\n"); c->wall = waSea; 
    for(int i=0; i<c->type; i++) printf("%d ", celldistAlt(c->mov[i]));
    printf("vs %d\n", celldistAlt(c));
    return; 
    }
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

// equidistants 

extern bool generatingEquidistant;

bool generatingEquidistant = false;

cell *buildAnotherEquidistant(cell *c, int radius) {
  int gdir = -1;
  for(int i=0; i<c->type; i++) {
    if(c->mov[i] && c->mov[i]->mpdist < c->mpdist) gdir = i;
    }
  if(gdir == -1) return NULL;
  
  cellwalker cw(c, (gdir+3) % c->type);
  vector<cell*> coastpath;
  
  while(size(coastpath) < radius || (cw.c->type != 7 && !weirdhyperbolic)) {
    // this leads to bugs for some reason!
    if(cw.c->land == laCrossroads2) {
#ifdef AUTOPLAY
      if(doAutoplay) printf("avoiding the Crossroads II\n"); // todo
#endif
      return NULL;
      }
    if(cw.c->bardir != NODIR) return NULL;
    if(cw.c->landparam && cw.c->landparam < radius) return NULL;

    /* forCellEx(c2, cw.c) if(c2->bardir != NODIR) {
      generatingEquidistant = false;
      return;
      } */
    coastpath.push_back(cw.c);
    if(cw.c->land == laNone && cw.c->mpdist <= 7) {
      raiseBuggyGeneration(cw.c, "landNone 1");
      for(int i=0; i<size(coastpath); i++) coastpath[i]->item = itPirate;
      return NULL;
      }
    cw = cw + wstep + 3;
    if(ctof(cw.c) && hrand(2) == 0) cw++;
    }
  coastpath.push_back(cw.c);
  // printf("setdists\n");
  for(int i=1; i<size(coastpath) - 1; i++) {
    if(coastpath[i-1]->land == laNone) {
      raiseBuggyGeneration(cwt.c, "landNone 3");
      int mpd[10];
      for(int i=0; i<10; i++) mpd[i] = coastpath[i]->mpdist;
      {for(int i=0; i<10; i++) printf("%d ", mpd[i]);} printf("\n");
      for(int i=0; i<size(coastpath); i++) coastpath[i]->item = itPirate;
      return NULL;
      }
    setdist(coastpath[i], BARLEV, coastpath[i-1]);
    setdist(coastpath[i], BARLEV-1, coastpath[i-1]);
    if(i < size(coastpath) - 5) {
      coastpath[i]->bardir = NOBARRIERS;
//      forCellEx(c2, coastpath[i]) c2->bardir = NOBARRIERS;
      }
    }
  
  //printf("building barrier\n");
  cell *c2 = coastpath[coastpath.size() - 1];
  int bd = 2 + (hrand(2)) * 3;
  
  bool nowall = false;
  
  if(c2->land == laNone) {
    raiseBuggyGeneration(c2, "landNone 2");
    for(int i=0; i<size(coastpath); i++) coastpath[i]->item = itPirate;
    return NULL;
    }

  // prevent gravity anomalies
  if(c2->land != c->land) return NULL;
  
  // else if(ctof(c) && hrand(10000) < 20 && !isCrossroads(c->land) && gold() >= 200)
  if(weirdhyperbolic && specialland == laCrossroads4 && buildBarrierNowall(c2, getNewLand(laOcean))) {
    nowall = true;
    }
  else if(pseudohept(c2) && gold() >= R200 && hrand(10) < 2 && buildBarrierNowall(c2, laCrossroads4, 1)) {
    nowall = true;
    // raiseBuggyGeneration(c2, "check");
    // return;
    }
  else if(!weirdhyperbolic) buildBarrier(c2, bd);
  //printf("building barrier II\n");
  if(hasbardir(c2)) extendBarrier(c2);

  for(int i=size(coastpath)-(nowall?1:2); i>=0; i--) {
    for(int j=BARLEV; j>=6; j--)
      setdist(coastpath[i], j, NULL);
    }
    
  return c2;
  }

void buildAnotherEquidistant(cell *c) {
  //printf("building another coast\n");
  
  if(yendor::on) return;

  generatingEquidistant = true;
  
  int radius = c->land == laOcean ? 30 : HAUNTED_RADIUS + 5;

  buildAnotherEquidistant(c, radius);
  
  generatingEquidistant = false;
  }

int coastval(cell *c, eLand base) {
  if(!c) return UNKNOWN;
  if(c->land == laNone) return UNKNOWN;
  if(base == laGraveyard) {
    if(c->land == laHaunted || c->land == laHauntedWall)
      return 0;
    if(c->land != laGraveyard && c->land != laHauntedBorder) return 30;
    }
  else if(base == laMirrored) {
    if(!inmirror(c)) return 0;
    if(!c->landparam) return UNKNOWN;
    return c->landparam & 255;
    }
  else {
    if(c->land == laOceanWall || c->land == laCaribbean || c->land == laWhirlpool ||
      c->land == laLivefjord || c->land == laWarpSea || c->land == laKraken || c->land == laDocks)
      return 30;
    if(c->land  != laOcean && !isGravityLand(c->land) && c->land != laHaunted) {
      return 0;
      }
    }
  if(!c->landparam) return UNKNOWN;
  return c->landparam;
  }

bool checkInTree(cell *c, int maxv) {
  if(c->landparam <= 3) return false;
  if(!maxv) return false;
  if(c->landflags) return true;
  for(int t=0; t<c->type; t++)
    if(c->mov[t] && c->mov[t]->landparam < c->landparam && checkInTree(c->mov[t], maxv-1))
      return true;
  return false;
  }

void buildEquidistant(cell *c) {
  if(!c) return;
  if(c->landparam) return;
  /* if(weirdhyperbolic) {
    c->landparam = 50;
    return;
    } */
  if(sphere || euclid) return;
  eLand b = c->land;
  if(chaosmode && !inmirror(b)) return;
  if(!b) { 
    printf("land missing at %p\n", c); 
    describeCell(c);
    for(int i=0; i<c->type; i++) if(c->mov[i])
      describeCell(c->mov[i]);
    // buggycells.push_back(c);
    }
  if(b == laHauntedBorder) b = laGraveyard;
  if(inmirror(b)) b = laMirrored;
  int mcv = UNKNOWN;

  // find the lowest coastval
  for(int i=0; i<c->type; i++) {
    int cv = coastval(createMov(c,i), b);
    if(cv < mcv) mcv = cv;
    }
  
  int mcv2 = 0;
  
  if(mcv == 0) {
    // if(generatingEquidistant) printf("mcv=0\n");
    c->landparam = 1;
    }
  else {
    // if it appears twice, increase it
    int qcv = 0;
    int sid = 0;
    for(int i=0; i<c->type; i++) 
      if(coastval(c->mov[i], b) == mcv)
        qcv++, sid = i;
      
    // if(generatingEquidistant) printf("qcv=%d mcv=%d\n", qcv, mcv);
    if(qcv >= 2) c->landparam = mcv+1; // (mcv == UNKNOWN ? UNKNOWN : mcv+1);
    else {
      // if(qcv != 1) { printf("qcv = %d\n", qcv);  exit(1); }
      cell *c2 = c->mov[sid];
      int bsid = c->spn(sid);
      for(int j=0; j<7; j++) {
        int q = (bsid+j+42) % c2->type;
        cell *c3 = c2->mov[q];
        if(coastval(c3, b) < mcv) {
          cell *c4 = createMovR(c2, bsid+1);
          if(c4->land == laNone && c2->mpdist <= BARLEV) setdist(c4, BARLEV, c2);
          buildEquidistant(c4);
          mcv2 = coastval(c4, b);
          break;
          }
        q = (bsid-j+42) % c2->type;
        c3 = c2->mov[q];
        if(coastval(c3, b) < mcv) {
          cell *c4 = createMovR(c2, bsid-1);
          if(c4->land == laNone && c2->mpdist <= BARLEV) setdist(c4, BARLEV, c2);
          buildEquidistant(c4);
          mcv2 = coastval(c4, b);
          break;
          }
        }
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
  
  if(c->land == laEndorian) {
    if(c->landparam == 1 && ctof(c)) {
      for(int i=0; i<S7; i++) {
        int i1 = (i+1) % S7;
        if(c->mov[i] && c->mov[i]->land != laEndorian && c->mov[i]->land != laNone)
        if(c->mov[i1] && c->mov[i1]->land != laEndorian && c->mov[i1]->land != laNone) {
          c->landflags = 2;
          c->wall = waTrunk;
          }
        }
      }
    else if(c->landparam == 1 && !ctof(c)) {
      for(int i=0; i<S6; i++) {
        int i1 = (i+1) % S6;
        int i2 = (i+2) % S6;
        int i4 = (i+4) % S6;
        int i5 = (i+5) % S6;
        if(c->mov[i] && c->mov[i]->land == laBarrier && c->mov[i]->type == 7)
        if(c->mov[i1] && c->mov[i1]->land != laBarrier)
        if(c->mov[i2] && c->mov[i2]->land != laBarrier) 
        if(c->mov[i4] && c->mov[i4]->land != laBarrier)
        if(c->mov[i5] && c->mov[i5]->land != laBarrier) {
          c->landflags = 2;
          c->wall = waTrunk;
          }

        if(c->mov[i] && c->mov[i]->land != laEndorian && c->mov[i]->land != laNone && c->mov[i]->type == 7)
        if(c->mov[i1] && c->mov[i1]->land != laEndorian && c->mov[i1]->land != laNone)
        if(c->mov[i5] && c->mov[i5]->land != laEndorian && c->mov[i5]->land != laNone) {
          c->landflags = 3;
          c->wall = waTrunk;
          }
        }
      }
    else if(c->landparam > 1) {
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if(c2 && c2->landparam < c->landparam && c2->landflags) {
          bool ok = false;
          if(c2->landflags == 3)
            ok = true;
          else if(c2->landflags == 2) {
            ok = c->mov[(i+1)%c->type]->landparam != c->landparam-1
             &&  c->mov[(i+c->type-1)%c->type]->landparam != c->landparam-1;
            }
          else for(int j=0; j<c2->type; j++) {
            cell *c3 = c2->mov[j];
            if(c3 && c3->landparam < c2->landparam && c3->landflags)
              if(c->spn(i) == (j+3)%c2->type || c->spn(i) == (j+c2->type-3)%c2->type)
                ok = true;
            }
          if(ok) {
            c->wall = waTrunk;
            c->landflags = 1;
            }
          }
        if(c2 && c2->landparam < c->landparam && c2->landflags == 1 && ctof(c)) {
          cell *c3 = c->mov[(i+1)%7];
          if(c3 && c3->landparam < c->landparam && c3->landflags == 1) {
            c->wall = waTrunk;
            c->landflags = 2;
            }
          }
        }
      
      if(!c->landflags && checkInTree(c, 5)) {
        int lev = hrand(100);
        if(lev < 10) c->wall = waSolidBranch;
        else if(lev < 20) c->wall = waWeakBranch;
        else c->wall = waCanopy;
        }
      }
    }
  
  bool chance = true;
  if(weirdhyperbolic) {
    chance = false;
    if(specialland == laCrossroads4)
      chance = hrand(100) < 10;
    }
  
  if(c->landparam > 30 && b == laOcean && !generatingEquidistant && hrand(10) < 5 && chance) 
    buildAnotherEquidistant(c);

  if(c->landparam > HAUNTED_RADIUS+5 && b == laGraveyard && !generatingEquidistant && hrand(100) < (nonbitrunc?25:5) && items[itBone] >= 10 && chance) 
    buildAnotherEquidistant(c);
  }

cell *randomDown(cell *c) {
  cell *tab[MAX_EDGE];
  int q=0;
  for(int i=0; i<c->type; i++) 
    if(c->mov[i] && coastval(c->mov[i], laIvoryTower) < coastval(c, laIvoryTower))
      tab[q++] = c->mov[i];
  if(!q) return NULL;
  if(q==1) return tab[0];
  return tab[hrand(q)];
  }

// which=1 => right, which=-1 => left
// set which=1,bonus=1 to get right neighbor on level

typedef int cellfunction(cell*);

cell *chosenDown(cell *c, int which, int bonus, cellfunction* cf) {
  int d = (*cf)(c)-1;
  for(int i=0; i<c->type; i++) {
    if(!c->mov[i]) createMov(c, i);
    if(c->mov[i]->mpdist > BARLEV && cf == coastvalEdge) setdist(c->mov[i], BARLEV, c);
    
    if((*cf)(c->mov[i]) == d) {
      int i2 = (i+which+S42)%c->type;
      createMov(c, i2);
      if((*cf)(c->mov[i2]) == d)
        return createMovR(c, i2+bonus);
      else return createMovR(c, i+bonus);
      }
    }
  // printf("nothing down here\n");
  return NULL;
  }

int edgeDepth(cell *c) {
  if(c->land == laIvoryTower || c->land == laEndorian || c->land == laDungeon) 
    return coastvalEdge(c);
  else if(c->land != laBarrier) {
    for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->land == laBarrier)
      return -20+c->cpdist;
    }
  return 0;
  }

int getHauntedDepth(cell *c) {
  if((tactic::on || euclid) && c->land == laHaunted) return celldist(c);
  if(c->land == laHaunted) return c->landparam;
  if(c->land == laHauntedWall) return 0;
  if(c->land == laHauntedBorder || c->land == laGraveyard) return -c->landparam;
  return -100;
  }

int towerval(cell *c, cellfunction* cf) {
  // if(c->land != laEdge) return 0;
  cell *cp1 = chosenDown(c, 1, 1, cf);
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
  int cfc = (*cf)(c);
  for(int i=0; i<c->type; i++) {
    if(c->mov[i] && (*cf)(c->mov[i]) < cfc)
      under++;
    }
  return (c->type-6) + 2*(cp1->type-6) + 4*under;
  }

/* other geometries */

void setLandWeird(cell *c) {
  // replaced with standard CR4
  /* if(specialland == laIvoryTower || specialland == laEndorian || specialland == laDungeon || specialland == laOcean) {
    int d = celldist(c) - (getDistLimit() - 2);
    if(d <= 0) 
      c->land = laCrossroads4;
    else
      c->land = specialland, c->landparam = d;
    } */
  }

void setLandQuotient(cell *c) {
  int fv = zebra40(c);
  if(fv/4 == 4 || fv/4 == 6 || fv/4 == 5 || fv/4 == 10) fv ^= 2;
  if(specialland == laWarpCoast)
    if(fv%4==0 || fv%4 == 2) setland(c, laWarpSea);
  if(specialland == laElementalWall)
    setland(c, eLand(laEFire + (fv%4)));
  if(specialland == laClearing)
    c->land = laClearing;
  if(specialland == laIvoryTower || specialland == laEndorian || specialland == laDungeon || specialland == laOcean) {
    int d = celldist(c) - 1;
    if(d <= 0) 
      c->land = laCrossroads4;
    else
      c->land = specialland, c->landparam = d;
    }
  }

void setLandSphere(cell *c) {
  if(specialland == laWarpCoast)
    setland(c, getHemisphere(c, 0) > 0 ? laWarpCoast : laWarpSea);
  if(specialland == laClearing)
    c->land = laClearing;
  if(specialland == laElementalWall) {
    int x = getHemisphere(c, 1);
    int y = getHemisphere(c, 2);
    if(x > 0 && y > 0) setland(c, laEFire);
    else if(x > 0 && y < 0) setland(c, laEAir);
    else if(x < 0 && y < 0) setland(c, laEWater);
    else if(x < 0 && y > 0) setland(c, laEEarth);
    else if(x > 0) 
      c->land = laElementalWall, c->barleft = laEAir, c->barright = laEFire;
    else if(x < 0) 
      c->land = laElementalWall, c->barleft = laEEarth, c->barright = laEWater;
    else if(y > 0) 
      c->land = laElementalWall, c->barleft = laEEarth, c->barright = laEFire;
    else if(y < 0) 
      c->land = laElementalWall, c->barleft = laEAir, c->barright = laEWater;
    if(c->land == laElementalWall && (c->type != 6 || gp::on))
      c->wall = getElementalWall(hrand(2) ? c->barleft : c->barright);
    }
  if(!torus)
  if(specialland == laCrossroads || specialland == laCrossroads2 || specialland == laCrossroads3 || specialland == laTerracotta) {
    int x = getHemisphere(c, 1);
    if(x == 0 && specialland == laTerracotta)
      setland(c, laMercuryRiver), c->wall = waMercury;
    else if(x == 0 || (specialland == laCrossroads3 && getHemisphere(c, 2) == 0)) 
      setland(c, laBarrier), c->wall = waBarrier;
    else setland(c, specialland);
    if(specialland == laCrossroads3 && c->type != 6 && c->master->fiftyval == 1 && !gp::on)
      c->wall = waBigTree;        
    }
  if(specialland == laIvoryTower || specialland == laEndorian || specialland == laDungeon || specialland == laOcean || specialland == laMountain) {
    int d = celldist(c);
    if(d <= 0) 
      c->land = laCrossroads4;
    else
      c->land = specialland, c->landparam = d;
    }
  }

eLand euland[max_vec];

eLand& get_euland(int c) {
  return euland[c & (max_vec-1)];
  }

void clear_euland(eLand first) {
  for(int i=0; i<max_vec; i++) euland[i] = laNone;
  euland[0] = euland[1] = euland[max_vec-1] = first;
  }

eLand switchable(eLand nearland, eLand farland, int c) {
  if(specialland == laCrossroads4) {
    if(hrand(15) == 0)
      return getNewLand(nearland);
    return nearland;
    }
  else if(nearland == laCrossroads) {
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

eLand getEuclidLand(int c) {
  auto& la = get_euland(c);
  if(la) return la;
  if(get_euland(c-2) && !get_euland(c-1)) getEuclidLand(c-1);
  if(get_euland(c-1)) return 
    la = switchable(get_euland(c-1), get_euland(c-2), c);
  if(get_euland(c+2) && !get_euland(c+1)) getEuclidLand(c+1);
  if(get_euland(c+1)) return 
    la = switchable(get_euland(c+1), get_euland(c+2), c);
  return la = laCrossroads;
  }

void setLandEuclid(cell *c) {
  setland(c, specialland);
  if(specialland == laCrossroads) {
    int x, y;
    tie(x,y) = cell_to_pair(c);
    setland(c, getEuclidLand(y+2*x));
    }
  if(specialland == laTerracotta) {
    int x, y;
    tie(x,y) = cell_to_pair(c);
    if(((y+2*x) & 15) == 1) {
      setland(c, laMercuryRiver);
      c->wall = waMercury;
      }
    }
  if(specialland == laCrossroads4) {
    int x, y;
    tie(x,y) = cell_to_pair(c);
    c->land = getEuclidLand(y);
    }
  if(specialland == laWhirlpool) {
    c->land = laOcean;
    c->landparam = 99;
    }
  if(specialland == laPrincessQuest) setland(c, laPalace);
  if(specialland == laOcean) {
    int x, y;
    tie(x,y) = cell_to_pair(c);
    y += 10;
    if(y == 0) 
      { setland(c, laBarrier); if(ishept(c)) c->land = laRlyeh; }
    else if(y<0) setland(c, laRlyeh);
    else c->landparam = y;
    }
  if(specialland == laIvoryTower || specialland == laDungeon) {
    int x, y;
    tie(x,y) = cell_to_pair(c); y = -5 - y;
    if(specialland == laDungeon) y = -10 - y;
    if(y == 0) 
      {setland(c, laBarrier); if(ishept(c)) setland(c, laAlchemist); }
    else if(y<0) setland(c, laAlchemist);
    else {
      c->landparam = y;
      }
    }
  if(specialland == laElementalWall) {
    int x, y;
    tie(x,y) = cell_to_pair(c);
    int x0 = euclid4 ? x : x + (y>>1);
    int y0 = y;
    
    int id = 0;
    if(y0&16) id += 2;
    if(x0&16) id += 1;
    
    x0 += 8; y0 += 8;

    y0--; x0++;
    int id2 = 0;
    if(y0&16) id2 += 2;
    if(x0&16) id2 += 1;
    
    setland(c, eLand(laEFire + id));
    
    if(((y0&15) == 15 && (x0&1)) || ((x0&15) == 0 && ((y0+1)&1))) {
      if(c->land == laEFire) c->wall = waEternalFire;
      if(c->land == laEWater) c->wall = waSea;
      if(c->land == laEAir) c->wall = waChasm;
      if(c->land == laEEarth) c->wall = waStone;
      c->barright = c->land;
      c->barleft = eLand(laEFire+id2);
      setland(c, laElementalWall);
      }
    }
  if(specialland == laCrossroads3) {
    int x, y;
    tie(x,y) = cell_to_pair(c);
    int y0 = euclid4 ? 2 * y - x : y; 
    int x0 = euclid4 ? 2 * x + y : x + y/2;
    
    x0 += 24; y0 += 8;
    
    int id = 0;
    if(y0&16) id ^= 1;
    if(x0&16) id ^= 1;
    
    setland(c, id ? laCrossroads3 : laDesert);
    
    if(euclid4 ? (!(y0&15) || !(x0&15)) : ((y0&15) == 15 && (x0&1)) || ((x0&15) == 0 && ((y0+1)&1))) {
      setland(c, laBarrier);
      c->wall = waBarrier;
      }
    }
  if(specialland == laWarpCoast) {
    int x, y;
    tie(x,y) = cell_to_pair(c);
    
    int zz = a4 ? x : 2*x+y + 10;
    zz = gmod(zz, 30);
    if(zz >= 15)
      setland(c, laWarpSea);
    else
      setland(c, laWarpCoast);
    }
  }

// the main big stuff function

bool quickfind(eLand l) {
  if(l == cheatdest) return true;
  if(l == specialland && weirdhyperbolic) return true;
#if CAP_TOUR
  if(tour::on && tour::quickfind(l)) return true;
#endif
  return false;
  }

#define INVLUCK (items[itOrbLuck] && inv::on)
#define I2000 (INVLUCK?600:2000)
#define I10000 (INVLUCK?3000:10000)

int wallchance(cell *c, bool deepOcean) {
  eLand l = c->land;
  return
    showoff ? (cwt.c->mpdist > 7 ? 0 : 10000) : 
    inmirror(c) ? 0 :
    isGravityLand(l) ? 0 :
    generatingEquidistant ? 0 :
    l == laPrairie ? 0 :
    (yendor::on && yendor::nexttostart) ? 10000 :
    princess::challenge ? 0 :
    isElemental(l) ? 4000 : 
    (yendor::on && (yendor::generating || !(yendor::clev().flags & YF_WALLS))) ? 0 :
    l == laCrossroads3 ? 10000 : 
    l == laCrossroads ? 5000 : 
    l == laCrossroads2 ? 10000 : 
    l == laCrossroads5 ? 10000 : 
    l == laCrossroads4 ? (weirdhyperbolic ? 5000 : 0) :
    (l == laMirror && !yendor::generating) ? 6000 :
    l == laTerracotta ? 250 :
    (tactic::on && !tactic::trailer) ? 0 :
    l == laCaribbean ? 500 :
    (l == laWarpSea || l == laWarpCoast) ? 500 :
    l == laStorms ? 250 :
    l == laCanvas ? 0 :
    l == laHaunted ? 0 :
    (l == laGraveyard && !deepOcean) ? 0 :
    (l == laGraveyard && items[itBone] >= 10) ? 120 :
    l == laOcean ? (deepOcean ? (nonbitrunc ? 250 : 2000) : 0) :
    l == laDragon ? 120 :
    50;
  }

bool mouse_reachability_test(cell *c) {
  if(c->pathdist != PINFD) 
    return true;
  forCellEx(c2, c)
    if(passable(c, c2, 0) && c2->mpdist < c->mpdist)
      return mouse_reachability_test(c2);
  return false;      
  }

bool horo_ok() {
  // do the horocycles work in the current geometry?
  // (they work in ALL hyperbolic geometries currently!)
  return hyperbolic;
  }

bool gp_wall_test() {
  if(gp::on) return hrand(gp::dist_3()) == 0;
  return true;
  }
  
void buildBigStuff(cell *c, cell *from) {
  if(sphere || quotient) return;
  bool deepOcean = false;
  
  if(c->land == laOcean) {
    if(!from) deepOcean = true;
    else for(int i=0; i<from->type; i++) {
      cell *c2 = from->mov[i];
      if(c2 && c2->land == laOcean && c2->landparam > 30) {
        deepOcean = true;
        }
      if(c2) forCellEx(c3, c2) if(c3 && c3->land == laOcean && c3->landparam > 30)
        deepOcean = true;
      }
    }
  
  if(c->land == laGraveyard) {
    if(!from) deepOcean = true;
    else for(int i=0; i<from->type; i++) {
      cell *c2 = from->mov[i];
      if(c2 && c2->landparam > HAUNTED_RADIUS+5)
        deepOcean = true;
      }
    }
  
  if(generatingEquidistant) deepOcean = false;
  // if(weirdhyperbolic && c->land == laOcean) deepOcean = c->landparam >= 30;
  
  // buildgreatwalls
  
  if(celldist(c) < 3 && !gp::on) {
    if(top_land && c == cwt.c->master->move[3]->c7) {
      buildBarrierStrong(c, 6, true, top_land);
      }
    }
  
  else if(chaosmode) {
    if(pseudohept(c) && hrand(10000) < 9000 && c->land && !inmirror(c) && buildBarrierNowall(c, getNewLand(c->land))) 
      {}
    else if(ctof(c) && c->land == laMirror && hrand(10000) < 2000 && !weirdhyperbolic) {
      int bd = 2 + hrand(2) * 3;
      buildBarrier(c, bd, laMirrored); 
      }
    }
  
  else if(pseudohept(c) && isWarped(c->land) && hrand(10000) < 3000 && c->land && 
    buildBarrierNowall(c, eLand(c->land ^ laWarpSea ^ laWarpCoast))) ;
  
  else if(pseudohept(c) && c->land == laCrossroads4 && hrand(10000) < 7000 && c->land && 
    buildBarrierNowall(c, getNewLand(laCrossroads4))) ;
  
  else if(pseudohept(c) && hrand(I10000) < 20 && !generatingEquidistant && !yendor::on && !tactic::on && !isCrossroads(c->land) && 
    gold() >= R200 && !weirdhyperbolic &&
    !inmirror(c) && !isSealand(c->land) && !isHaunted(c->land) && !isGravityLand(c->land) && 
    (c->land != laRlyeh || rlyehComplete()) &&
    c->land != laTortoise && c->land != laPrairie && c->land && 
    !(c->land == laGraveyard && !deepOcean) 
    && c->land != laCanvas
    ) {
    buildBarrierNowall(c, laCrossroads4) ;
    }
  
  else if(weirdhyperbolic && specialland == laCrossroads4 && /*pseudohept(c) &&*/ hrand(I10000 /4) < wallchance(c, deepOcean) && gp_wall_test()) {
    buildBarrierNowall(c, getNewLand(c->land));
    }
  
  else if(weirdhyperbolic && specialland == laElementalWall && hrand(I10000) < 1000 && gp_wall_test()) 
    buildBarrierNowall(c, getNewLand(c->land));
  
  else if(weirdhyperbolic) ; // non-Nowall barriers not implemented yet in weird hyperbolic
  
  else if(c->land == laCrossroads2 && !nonbitrunc)
    buildCrossroads2(c);
  
  else if(c->land == laPrairie && c->LHU.fi.walldist == 0) {
    for(int bd=0; bd<7; bd++) {
      int fval2 = createStep(c->master, bd)->fieldval;
      int wd = currfp_gmul(fval2, currfp_inverses(c->fval-1));
      if(currfp_distwall(wd) == 0) {
        buildBarrier(c, bd); 
        break;
        }
      }
    }

  else if(ctof(c) && c->land && hrand(I10000) < wallchance(c, deepOcean))
  {
    
    int bd = 2 + hrand(2) * 3;
    
    buildBarrier(c, bd); 

    /* int bd = 2;      
    buildBarrier4(c, bd, 0, getNewLand(c->land), c->land); */
    }
      
  if((!chaosmode) && bearsCamelot(c->land) && ctof(c) && 
    (quickfind(laCamelot) || peace::on || (hrand(I2000) < 200 && horo_ok() && 
    items[itEmerald] >= U5 && !tactic::on))) {
    int rtr = newRoundTableRadius();
    heptagon *alt = createAlternateMap(c, rtr+14, hsOrigin);
    if(alt) {
      alt->emeraldval = rtr;
      alt->fiftyval = c->land;
      }
    }

  if(!chaosmode) {
  
    // buildbigstuff

    if(c->land == laRlyeh && ctof(c) && 
      (quickfind(laTemple) || peace::on || (hrand(I2000) < 100 && 
      items[itStatue] >= U5 && !randomPatternsMode && 
      !tactic::on && !yendor::on)))
      createAlternateMap(c, 2, hsA);

    if(c->land == laJungle && ctof(c) && 
      (quickfind(laMountain) || (hrand(I2000) < 100 && horo_ok() && 
      !randomPatternsMode && !tactic::on && !yendor::on && landUnlocked(laMountain))))
      createAlternateMap(c, 2, hsA);

    if(c->land == laOvergrown && ctof(c) && 
      (quickfind(laClearing) || (hrand(I2000) < 25 && !gp::on && 
      !randomPatternsMode && items[itMutant] >= U5 &&
      isLandIngame(laClearing) &&
      !tactic::on && !yendor::on))) {
      heptagon *h = createAlternateMap(c, 2, hsA);
      if(h) clearing::bpdata[h].root = NULL;
      }

    if(c->land == laStorms && ctof(c) && hrand(2000) < 1000 && horo_ok() && !randomPatternsMode) {
      heptagon *h = createAlternateMap(c, 2, hsA);
      if(h) h->alt->emeraldval = hrand(2);
      }

    if(c->land == laOcean && ctof(c) && deepOcean && !generatingEquidistant && !peace::on && horo_ok() && 
      (quickfind(laWhirlpool) || (
        hrand(2000) < (nonbitrunc ? 500 : 1000) && !tactic::on && !yendor::on)))
      createAlternateMap(c, 2, hsA);

    if(c->land == laCaribbean && horo_ok() && ctof(c) && !c->master->alt)
      createAlternateMap(c, 2, hsA);

    if(c->land == laPalace && ctof(c) && !princess::generating && !shmup::on && multi::players == 1 && horo_ok() && 
      (princess::forceMouse ? mouse_reachability_test(from) :
        (hrand(2000) < (peace::on ? 100 : 20))) && 
      !c->master->alt && 
      (princess::challenge || kills[moVizier] || peace::on) && !tactic::on && !yendor::on) {
      createAlternateMap(c, PRADIUS0, hsOrigin, waPalace);
      celllister cl(c, 5, 1000000, NULL);
      for(cell *c: cl.lst) if(c->master->alt) generateAlts(c->master);
      }
    }
  
  if(hasbardir(c)) extendBarrier(c);
  }

bool openplains(cell *c) {
  if(chaosmode) {
    forCellEx(c2, c) if(c2->land != laHunting) return false;
    return true;
    }
  int dlimit = getDistLimit();
  if(dlimit < 7) {
    celllister cl(c, dlimit, 1000000, NULL);
    int bad = 0;
    for(cell *c: cl.lst) { 
      while(c->mpdist > 8) setdist(c, c->mpdist-1, NULL);
      if(c->land != laHunting) {bad++; if(bad>5) return false;}
      }
    return true;
    }
  else {
    celllister cl(c, dlimit, 1000000, NULL);
    for(cell *c: cl.lst) { 
      while(c->mpdist > 8) setdist(c, c->mpdist-1, NULL);
      if(c->land != laHunting) return false;
      }
    return true;
    }    
  }

void buildCamelotWall(cell *c) {
  c->wall = waCamelot;
  for(int i=0; i<c->type; i++) {
    cell *c2 = createMov(c, i);
    if(c2->wall == waNone && (euclid || (c2->master->alt && c->master->alt)) && celldistAlt(c2) > celldistAlt(c) && c2->monst == moNone)
      c2->wall = waCamelotMoat;
    }
  }

void moreBigStuff(cell *c) {

  if(quotient) return;

  if(c->land == laPalace && !euclid && c->master->alt) {
    int d = celldistAlt(c);
    if(d <= PRADIUS1) generateAlts(c->master);
    }

  if(c->land == laStorms)
    if(!euclid && !quotient && !sphere) {
      if(c->master->alt && c->master->alt->distance <= 2) {
        generateAlts(c->master);
        preventbarriers(c);
        int d = celldistAlt(c);
        if(d <= -2) {
          c->wall = (c->master->alt->alt->emeraldval & 1) ? waCharged : waGrounded;
          c->item = itNone;
          c->monst = moNone;
          }
        else if(d <= -1)
          c->wall = (hrand(100) < 20) ? waSandstone : waNone;
        else if(d <= 0)
          c->wall = waNone;
        }
      }        

  if((bearsCamelot(c->land) && !euclid && !quotient) || c->land == laCamelot) 
  if(euclid || c->master->alt) {
    int d = celldistAltRelative(c);
    if(tactic::on || (d <= 14 && roundTableRadius(c) > 20)) {
      if(!euclid) generateAlts(c->master);
      preventbarriers(c);
      if(d == 10) {
        if(weirdhyperbolic ? hrand(100) < 50 : pseudohept(c)) buildCamelotWall(c);
        else {
          if(!euclid) for(int i=0; i<S7; i++) generateAlts(c->master->move[i]);
          int q = 0;
          if(weirdhyperbolic) {
            for(int t=0; t<c->type; t++) createMov(c, t);
            q = hrand(100);
            if(q < 10) q = 0;
            else if(q < 50) q = 1;
            }
          else {
            for(int t=0; t<c->type; t++) {
              createMov(c, t);
              if(celldistAltRelative(c->mov[t]) == 10 && !pseudohept(c->mov[t])) q++;
              }
            }
          if(q == 1) buildCamelotWall(c);
          // towers of Camelot
          if(q == 0 && !nonbitrunc) {
            c->monst = moKnight;
            c->wall = waTower;
            forCellEx(c2, c) {
              int cr = celldistAltRelative(c2);
              if(cr == 9) ;
              else {
                buildCamelotWall(c2);
                if(!ctof(c2))
                  c2->wall = waTower, c2->wparam = 1;
                }
              }
            for(int i=0; i<c->type; i++) if(celldistAltRelative(c->mov[i]) < d)
              c->mondir = i;
            }
          }
        }
      if(d == 0) c->wall = waRoundTable;
      if(celldistAlt(c) == 0 && !tactic::on) c->item = itHolyGrail;
      if(d < 0 && hrand(7000) <= 10 + items[itHolyGrail] * 5) {
        eMonster m[3] = { moHedge, moLancer, moFlailer };
        c->monst = m[hrand(3)];
        }
      if(d == 1) {
        // roughly as many knights as table cells
        if(hrand(nonbitrunc ? 2618 : 1720) < 1000) 
          c->monst = moKnight;
        if(!euclid) for(int i=0; i<S7; i++) generateAlts(c->master->move[i]);
        for(int i=0; i<c->type; i++) 
          if(c->mov[i] && celldistAltRelative(c->mov[i]) < d)
            c->mondir = (i+3) % 6;
        }
      if(tactic::on && d >= 2 && d <= 8 && hrand(1000) < 10)
        c->item = itOrbSafety;
      if(d == 5 && tactic::on)
        c->item = itGreenStone;
      if(d <= 10) c->land = laCamelot;
      if(d > 10 && !euclid && !tactic::on) {
        setland(c, eLand(c->master->alt->alt->fiftyval));
        if(c->land == laNone) printf("Camelot\n"); // NONEDEBUG
        }
      }
    }
  
  if(chaosmode && c->land == laTemple) {
    for(int i=0; i<c->type; i++)
      if(pseudohept(c) && c->mov[i] && c->mov[i]->land != laTemple)
        c->wall = waColumn;
    }
  
  else if((c->land == laRlyeh && !euclid) || c->land == laTemple) {
    if(euclid || (c->master->alt && (tactic::on || c->master->alt->distance <= 2))) {
      if(!euclid && !chaosmode) generateAlts(c->master);
      preventbarriers(c);
      int d = celldistAlt(c);
      if(d <= 0) {
        c->land = laTemple, c->wall = waNone, c->monst = moNone, c->item = itNone;
        }
      if(d % TEMPLE_EACH==0) {
        if(weirdhyperbolic && nonbitrunc) {
          if(hrand(100) < 50) c->wall = waColumn;
          }
        else if(pseudohept(c)) 
          c->wall = waColumn;
        else {
          if(!euclid) for(int i=0; i<S7; i++) generateAlts(c->master->move[i]);
          int q = 0;
          for(int t=0; t<c->type; t++) {
            createMov(c, t);
            if(celldistAlt(c->mov[t]) % TEMPLE_EACH == 0 && !ishept(c->mov[t])) q++;
            }
          if(q == 2) c->wall = waColumn;
          }
        }
      }
    }

  if((c->land == laOvergrown && !euclid) || c->land == laClearing) {
    if(euclid || (c->master->alt && (tactic::on || c->master->alt->distance <= 2))) {
      if(!euclid) generateAlts(c->master);
      preventbarriers(c);
      int d = celldistAlt(c);
      if(d <= 0) {
        c->land = laClearing, c->wall = waNone; // , c->monst = moNone, c->item = itNone;
        }
      else if(d == 1 && !tactic::on && !euclid)
        c->wall = waSmallTree, c->monst = moNone, c->item = itNone;
      }
    }

  if((c->land == laJungle && !euclid) || c->land == laMountain) {
    if(euclid || (c->master->alt && (tactic::on || c->master->alt->distance <= 2))) {
      if(!euclid) generateAlts(c->master);
      preventbarriers(c);
      int d = celldistAlt(c);
      if(d <= 0 || (firstland == laMountain && tactic::on)) {
        c->land = laMountain, c->wall = waNone; // , c->monst = moNone, c->item = itNone;
        }
      }
    }

  if(c->land == laOcean || c->land == laWhirlpool) {
    bool fullwhirlpool = false;
    if(tactic::on && specialland == laWhirlpool)
      fullwhirlpool = true;
    if(yendor::on && yendor::clev().l == laWhirlpool)
      fullwhirlpool = true;
    if(euclid || (c->master->alt && (fullwhirlpool || c->master->alt->distance <= 2))) {
      if(!euclid) generateAlts(c->master);
      preventbarriers(c);
      int dd = celldistAlt(c);
      if(dd <= 0 || fullwhirlpool) {
        c->land = laWhirlpool, c->wall = waSea, c->monst = moNone, c->item = itNone;
        }
      }
    }      
  }
