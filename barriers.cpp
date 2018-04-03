// Hyperbolic Rogue -- Barriers
// This file implements routines related to barriers (Great Walls and similar).
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

bool checkBarriersFront(cellwalker bb, int q, bool cross) {

  if(!ctof(bb.c)) 
    return false;

  if(bb.c->mpdist < BARLEV) return false;
  if(bb.c->mpdist == BUGLEV) return false;
  if(bb.c->bardir != NODIR) return false;
  if(bb.spin == (nonbitrunc ? 3 : 0)) {q--; if(!q) return true; }

  if(!cross) for(int i=0; i<7; i++) {
    cellwalker bb2 = bb + i + wstep;
    if(bb2.c->bardir != NODIR) return false; 
    if(!nonbitrunc) { 
      bb2 = bb2 + 4 + wstep;
      if(bb2.c->bardir != NODIR) return false;
      }
    }

  bb += wstep;
  if(!nonbitrunc) { bb = bb + 3 + wstep + 3 + wstep; }
  return checkBarriersBack(bb, q);
  }

bool hasbardir(cell *c) {
  return c->bardir != NODIR && c->bardir != NOBARRIERS;
  }

void preventbarriers(cell *c) {
  if(c && c->bardir == NODIR) c->bardir = NOBARRIERS;
  }

bool checkBarriersBack(cellwalker bb, int q, bool cross) {
  // printf("back, %p, s%d\n", bb.c, bb.spin);

  // if(mark) { printf("mpdist = %d [%d] bardir = %d spin=%d q=%d cross=%d\n", bb.c->mpdist, BARLEV, bb.c->bardir, bb.spin, q, cross); }
  
  if(bb.c->mpdist < BARLEV) return false;
  if(bb.c->mpdist == BUGLEV) return false;
  if(bb.c->bardir != NODIR) return false;

  // if(bb.spin == 0 && bb.c->mpdist == INFD) return true;
  
  if(!cross) for(int i=0; i<7; i++) {
    cellwalker bb2 = bb + i + wstep;
    if(bb2.c->bardir != NODIR) return false;
    if(!nonbitrunc) {
      bb2 = bb2 + 4 + wstep;
      if(bb2.c->bardir != NODIR) return false;
      }
    }

  bb = bb + 3 + wstep + (nonbitrunc ? 5 : 4) + wstep + 3;
  return checkBarriersFront(bb, q);
  }

bool checkBarriersNowall(cellwalker bb, int q, int dir, eLand l1=laNone, eLand l2=laNone) {
  if(bb.c->mpdist < BARLEV && l1 == laNone) return false;
  if(bb.c->bardir != NODIR && l1 == laNone) return false;
  // if(bb.c->mov[dir] && bb.c->mov[dir]->bardir != NODIR && l1 == laNone) return false;
  // if(bb.c->mov[dir] && bb.c->mov[dir]->mpdist < BARLEV && l1 == laNone) return false;
  
  if(l1 != laNone) { 
    bb.c->bardir = bb.spin; bb.c->barright = l2; bb.c->barleft = NOWALLSEP; 
    setland(bb.c, l1);
    }
  if(q > 10) return true;
  
  if(l1 == laNone) for(int i=0; i<bb.c->type; i++) {
    cell *c1 = bb.c->mov[i];
    if(!c1) continue;
    for(int j=0; j<c1->type; j++) {
      cell *c2 = c1->mov[j];
      if(!c2) continue;
      if(c2 && c2->bardir == NOBARRIERS) 
        return false;
      if(c2 && c2->bardir != NODIR && c2->barleft != NOWALLSEP) 
        return false;
      // note: "far crashes" between NOWALL lines are allowed
      }
    }
  
  if(nonbitrunc && S3==4) {
    bb = bb + dir + wstep + dir;
    }
  else if(nonbitrunc) {
    bb = bb + (dir>0?3:4) + wstep - (dir>0?3:4);
    }
  else {
    bb = bb + wstep + (2*dir) + wstep + dir;
    }
  return checkBarriersNowall(bb, q+1, -dir, l2, l1);
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
    bool setbar = ctof(c);
    if(c->barleft == laKraken || c->barright == laKraken)
    if(c->barleft != laWarpSea && c->barright != laWarpSea)
      setbar = !setbar;
    c->wall = setbar ? waBarrier : waSea;
    c->land = laOceanWall;
    }
  else if(isElemental(c->barleft) && isElemental(c->barright)) {
    c->land = laElementalWall;
    c->wall = getElementalWall(c->barleft);
    }
  else if(c->barleft == laHaunted || c->barright == laHaunted) {
    c->land = laHauntedWall;
    }
  else if(c->barleft == laMirrored2 || c->barright == laMirrored2)
    c->land = laMirrorWall2;
  else if(c->barleft == laMirrored || c->barright == laMirrored)
    c->land = laMirrorWall;
  else if(c->barleft == laTerracotta && c->barright == laTerracotta) {
    c->land = laMercuryRiver;
    c->wall = waMercury;
    }
  else {
    c->wall = waBarrier;
    c->land = laBarrier;
    }
  }

void setland(cell *c, eLand l) {
  if(c->land != l)  {
    c->landparam = 0;  
    }
  if(l == laNone) {
    printf("setland\n"); // NONEDEBUG
    }
  c->land = l;
  }

void extendcheck(cell *c) {
  return;
  if(!nonbitrunc && c->landparam == 0 && c->barleft != NOWALLSEP) {
    raiseBuggyGeneration(c, "extend error");
    }
  }

bool mirrorwall(cell *c) {
  return c->barleft == laMirrored || c->barright == laMirrored;
  }
  
void extendBarrierFront(cell *c) {
  limitgen("extend front %p\n", c); 
  if(buggyGeneration) return;
  int ht = c->landparam;
  extendcheck(c);

  cellwalker bb(c, c->bardir); setbarrier(bb.c);
  bb += wstep;
  
  if(!nonbitrunc) {
    bb.c->barleft = c->barleft;
    bb.c->barright = c->barright;
    setbarrier(bb.c);
    if(!mirrorwall(bb.c)) 
      bb.c->landparam = (ht-4);
  //printf("[A heat %d]\n", ht-4);
  
    setland((bb + 2 + wstep).c, c->barleft);
    setland((bb + 4 + wstep).c, c->barright);
    
    bb = bb + 3 + wstep; 
    bb.c->barleft = c->barright;
    bb.c->barright = c->barleft;
    setbarrier(bb.c);
    if(!mirrorwall(bb.c)) 
      bb.c->landparam = (ht-4)^2;
  //printf("[B heat %d]\n", (ht-4)^2);
    
    bb = bb + 3 + wstep;
    
    bb.c->barleft = c->barleft;
    bb.c->barright = c->barright;
    if(!mirrorwall(bb.c)) 
      bb.c->landparam = ht ^ 2;
    }

//printf("[C heat %d]\n", (ht)^2);
  bb.c->bardir = bb.spin;
  bb.c->barleft = c->barright;
  bb.c->barright = c->barleft;
  // printf("#1\n");
  extendcheck(bb.c);
  extendBarrier(bb.c);
  
  for(int a=-3; a<=3; a++) if(a) {
    bb.c = c; bb.spin = c->bardir; bb += (nonbitrunc?-a:a); bb += wstep; 
    setland(bb.c, a > 0 ? c->barright : c->barleft);
    }
  }

void extendBarrierBack(cell *c) {
  limitgen("extend back %p\n", c); 
  if(buggyGeneration) return;
  int ht = c->landparam;
  extendcheck(c);

  cellwalker bb(c, c->bardir); setbarrier(bb.c);
  bb = bb + 3 + wstep + (nonbitrunc?5:4); 
  setland(bb.c, nonbitrunc ? c->barleft : c->barright); 
  bb = bb + wstep + 3;
  bb.c->bardir = bb.spin;
  bb.c->barleft = c->barright;
  bb.c->barright = c->barleft;
  if(!mirrorwall(bb.c)) 
    bb.c->landparam = ht ^ 11;
  extendcheck(bb.c);
//printf("[D heat %d]\n", (ht^11));

  // needed for CR2 to work
  if(!nonbitrunc) { 
    auto bb2 = bb + wstep;
    bb2.c->barleft = c->barright;
    bb2.c->barright = c->barleft;
    if(!mirrorwall(bb2.c)) 
      bb2.c->landparam = (ht^11)-4;
    }
//printf("[E heat %d]\n", (ht^11));

  // printf("#2\n");
  extendBarrier(bb.c);
  }

void extendNowall(cell *c) {

  c->barleft = NOWALLSEP_USED;
  cellwalker cw(c, c->bardir);
  
  if(!nonbitrunc) {
    cw += wstep;
    setland(cw.c, c->barright);
    }
  
  if(nonbitrunc && S3 == 4) {
    auto cw2 = cw + wstep;
    setland(cw2.c, c->barright);
    cw2.c->barleft = NOWALLSEP_USED;
    cw2.c->barright = c->land;
    cw2.c->bardir = cw2.spin;
    }
  
  for(int i=-1; i<2; i+=2) {  
    cellwalker cw0;
    if(nonbitrunc && S3==4) {
      cw0 = cw + i + wstep + i; 
      }
    else if(nonbitrunc) {
      cw0 = cw + (i>0?3:4) + wstep - (i>0?3:4);
      //cw0 = cw + (3*i) + wstep - (3*i);
      }
    else {
      cw0 = cw + (2*i) + wstep;
      }
    if(cw0.c->barleft != NOWALLSEP_USED) {
      cw0.c->barleft = NOWALLSEP;
      if(S3 == 4 && nonbitrunc) {
        cw0.c->barright = c->barright;
        cw0.c->bardir = cw0.spin;
        setland(cw0.c, c->land);
        }
      else {
        setland(cw0.c, c->barright);
        cw0.c->barright = c->land;
        if(c->barright == laNone) { 
          printf("barright\n"); 
          }// NONEDEBUG
        setland(cw0.c, c->barright);
        if(!nonbitrunc) cw0 += i;
        cw0.c->bardir = cw0.spin;
        if(!nonbitrunc) cw0 -= i;
        }
      extendcheck(cw0.c);
      extendBarrier(cw0.c);
      }
    }
  }

bool gotit = false;

void extendCR5(cell *c) {
  if(nonbitrunc) return;
// if(c->barright == laCrossroads5) extendCR5(c);
  eLand forbidden = c->barleft;
  eLand forbidden2 = laNone;
  cellwalker cw(c, c->bardir);
  for(int u=0; u<2; u++) {
    // if(gotit) break;
    cw = cw + 2 + wstep + 2 + wstep + 5;
    if(cw.c->bardir == NODIR) {
      cw.c->landparam = 40;
      cw.c->bardir = cw.spin;
      cw.c->barright = laCrossroads5;
      eLand nland = forbidden;
      for(int i=0; i<10 && (nland == forbidden || nland == forbidden2); i++)
        nland = getNewLand(laCrossroads5);
      cw.c->barleft = forbidden2 = nland;
      landcount[nland]++;
      extendBarrier(cw.c);
      gotit = true;
      }
    else forbidden2 = cw.c->barleft;
    }
  }

bool isbar4(cell *c) {
  return
    c->wall == waBarrier || c->land == laElementalWall || 
    c->land == laMirrorWall || c->land == laMirrorWall2 ||
    c->land == laMercuryRiver;
  }  

void extendBarrier(cell *c) {
  limitgen("extend barrier %p\n", c); 
  if(buggyGeneration) return;
  
  if(c->barleft == NOWALLSEP_USED) return;

  extendcheck(c);
  
  // printf("build barrier at %p", c);
  if(c->land == laBarrier || c->land == laElementalWall || c->land == laHauntedWall || c->land == laOceanWall || 
    c->land == laMirrorWall || c->land == laMirrorWall2 || c->land == laMercuryRiver) { 
    // printf("-> ready\n");
    return;
    }
//  if(c->wall == waWaxWall) return;
  if(c->mpdist > BARLEV) {
    // printf("-> too far\n");
    return; // == INFD) return;
    }

  if(c->barleft == NOWALLSEP) {
    extendNowall(c);
    return;
    }
  
  bool firstmirror = 
    (c->barleft == laMirrored || c->barright == laMirrored) && 
      c->barleft != laMirrored2 && c->barright != laMirrored2;
  
  if(firstmirror && c->barleft == laMirror && hrand(100) < 60) {
    cellwalker cw(c, c->bardir);
    if(!nonbitrunc) cw += wstep;
    if(cw.c->land != laMirrorWall)
      if(buildBarrier6(cw, 1)) return;
    }
    
  if(firstmirror && (nonbitrunc?c->barleft == laMirror : c->barright == laMirror) && hrand(100) < 60) {
    cellwalker cw(c, c->bardir);
    if(nonbitrunc) {
      cw = cw - 3 + wstep - 3;
      }
    else {
      cw = cw + wstep + 3 + wstep - 1; // check this
      }
    if(buildBarrier6(cw, 2)) return;    
    }
    
  if(((c->barleft == laCrossroads3 || c->barright == laCrossroads3) && hrand(100) < 66) ||
    (isElemental(c->barleft) && isElemental(c->barright) && hrand(100) < 75) 
    || (firstmirror && hrand(100) < 60)
    ) {
    
    cellwalker cw(c, c->bardir);
    if(nonbitrunc) {
      cw += wstep;
      if(isbar4(cw.c)) {
        cw = cw + wstep + 3 + wstep - 1 + wstep;
        bool b = buildBarrier4(cw.c, cw.spin, 2, oppositeElement(c->barleft, c->barright), c->barright);
        if(b) return;
        }
      else {
        bool b = buildBarrier4(c, c->bardir, 1, c->barleft, c->barright);
        if(b) return;
        }
      }
    else {
      cw = cw + 3 + wstep;
      cell *cp = (cw + 4 + wstep).c;
      if(!isbar4(cp)) {
        cw = cw + 2 + wstep;
        bool b = buildBarrier4(cw.c, cw.spin, 2, oppositeElement(c->barleft, c->barright), c->barright);
        if(b) return;
        }
      else {
        bool b = buildBarrier4(c, c->bardir, 1, c->barleft, c->barright);
        if(b) return;
        }
      }
    }
  
  extendBarrierFront(c);  
  extendBarrierBack(c);
  
  if(c->barright == laCrossroads5) extendCR5(c);
  }

void buildBarrierForce(cell *c, int d, eLand l) {
  c->bardir = d;
  eLand oldland = c->land;
  if(oldland == laNone) {
    raiseBuggyGeneration(c, "oldland is NONE");
    return;
    }
  eLand newland = l ? l : getNewLand(oldland);
  landcount[newland]++;
  if(d == 4 || d == 5 || d == 6) c->barleft = oldland, c->barright = newland;
  else c->barleft = newland, c->barright = oldland;
  if(!mirrorwall(c)) c->landparam = 40;
  extendcheck(c);
  }

void buildBarrier(cell *c, int d, eLand l) {
  d %= 7;
  cellwalker bb(c, d);
  
  if(checkBarriersFront(bb) && checkBarriersBack(bb)) 
    buildBarrierForce(c, d, l);
  }

bool buildBarrier6(cellwalker cw, int type) {
  limitgen("build6 %p/%d (%d)\n", cw.c, cw.spin, type); 
  
  cellwalker b[4];
  
  if(buggyGeneration) return true;

  if(!nonbitrunc) {
    b[0] = cw + wstep;
    b[1] = cw + 1 + wstep + 3 + wstep;
    b[2] = cw + 4 + wstep;
    b[3] = cw + 3 + wstep + 3 + wstep;
    }
  else {
    b[0] = cw;
    b[1] = cw + 3 + wstep + 3;
    b[2] = cw - 2 + wstep - 3;
    b[3] = cw - 3 + wstep + 2 + wstep - 3;

    if(type == 1 && b[3].c->land != laMirrorWall) return false;
    if(type == 2 && (b[1] + wstep).c->land != laMirrorWall) return false;
    // if(type == 2 && b[2].c->land != laMirrorWall) return false;
    }
  
  if(false) {
    for(int z=0; z<4; z++) {
      printf("%p/%d\n", b[z].c, b[z].spin);
      b[z].c->wall = waStrandedBoat; b[z].c->land = laAlchemist;
      b[z].c->mondir = b[z].spin;
      b[z].c->mpdist = 7;
      b[z].c->item = eItem(1+z);
      buggyGeneration = true;
      }
    return true;
    }
   
  if(type == 1) {  
    if(!(nonbitrunc?checkBarriersFront:checkBarriersBack)(b[1], 6, true)) return false;
    if(!(nonbitrunc?checkBarriersFront:checkBarriersBack)(b[2], 6, true)) return false;
    }
  else {
    if(!(nonbitrunc?checkBarriersFront:checkBarriersBack)(b[0], 6, true)) return false;
    if(!(nonbitrunc?checkBarriersFront:checkBarriersBack)(b[3], 6, true)) return false;
    }
  
  for(int d=0; d<4; d++) {
    b[d].c->bardir = b[d].spin;
    
    if(nonbitrunc) {
      b[0].c->barleft = laMirrored, b[0].c->barright = laMirrored2;
      b[1].c->barleft = laMirror, b[1].c->barright = laMirrored;
      b[2].c->barleft = laMirrored2, b[2].c->barright = laMirrored;
      b[3].c->barleft = laMirrored, b[3].c->barright = laMirror;
      }
    else {    
      b[0].c->barleft = laMirror, b[0].c->barright = laMirrored;
      b[1].c->barleft = laMirrored, b[1].c->barright = laMirror;
      b[2].c->barleft = laMirrored, b[2].c->barright = laMirrored2;
      b[3].c->barleft = laMirrored2, b[3].c->barright = laMirrored;
      }
  
    (nonbitrunc?extendBarrierFront:extendBarrierBack)(b[d].c);
    }  

  if(nonbitrunc && false) {
    for(int z=0; z<4; z++)
      b[z].c->item = eItem(1+z+4*type);
    for(int a=0; a<4; a++) 
      extendBarrierBack((b[a]+wstep).c);
    }

  if(!nonbitrunc) {
    setland((cw+1+wstep).c, laMirrorWall);
    setland((cw+2+wstep).c, laMirrored);
    setland((cw+3+wstep).c, laMirrorWall2);
    setland((cw+4+wstep).c, laMirrorWall2);
    setland((cw+5+wstep).c, laMirrored);
    setland((cw+0+wstep).c, laMirrorWall);
    setland((b[0]+2+wstep).c, laMirrored);
    setland((b[3]+6+wstep).c, laMirrored2);
    setland((b[3]+5+wstep).c, laMirrored2);
    setland((b[1]-1+wstep).c, laMirrored);
    setland((b[2]-2+wstep).c, laMirrored);
    setland((b[1]-2+wstep).c, laMirrored);
    setland((b[0]-2+wstep).c, laMirror);
    cw.c->land = laMirrorWall;
    cw.c->wall = waMirrorWall;
    cw.c->landparam = 1;
    }
  else {
    setland(cw.c, laMirrorWall2);
    setland((cw+0+wstep).c, laMirrorWall2);
    setland((cw+1+wstep).c, laMirrored);
    setland((cw+2+wstep).c, laMirrored);
    setland((cw+3+wstep).c, laMirrorWall);
    setland((cw+4+wstep).c, laMirrored);
    setland((cw+5+wstep).c, laMirrorWall2);
    setland((cw+6+wstep).c, laMirrored2);

    setland((b[1]+wstep).c, laMirrorWall);
    setland((b[1]+1+wstep).c, laMirror);
    setland((b[1]+2+wstep).c, laMirrorWall);
    setland((b[1]+6+wstep).c, laMirrored);

    setland((b[0] + wstep - 2 + wstep).c, laMirrored);
    setland((b[3] + wstep - 2 + wstep).c, laMirrored);
    }

  return true;
  }
  
  

bool buildBarrier4(cell *c, int d, int mode, eLand ll, eLand lr) {
  limitgen("build4 %p\n", c); 
  if(buggyGeneration) return true;
  d %= 7;
  
  cellwalker cd(c, d);
  
  cellwalker b1 = cd;
  cellwalker b2 = nonbitrunc ? cd + wstep : cd + wstep + 3 + wstep + 3 + wstep;  
  cellwalker b3 = nonbitrunc ? cd - 1 + wstep + 3 : cd + wstep + 4 + wstep + 4;
  cellwalker b4 = nonbitrunc ? cd + 1 + wstep - 3 : cd + wstep - 4 + wstep - 4;
  
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
  
  eLand xl = oppositeElement(ll, lr);
  eLand xr = oppositeElement(lr, ll);

  c->bardir = d, c->barleft = ll, c->barright = lr; extendBarrierBack(c);
  
  c= b2.c; d=b2.spin;
  c->bardir = d, c->barleft = xl, c->barright = xr; extendBarrierBack(c);

  c= b3.c; d=b3.spin;
  c->bardir = d, c->barleft = xl, c->barright = lr; extendBarrierFront(c);

  c= b4.c; d=b4.spin;
  c->bardir = d, c->barleft = ll, c->barright = xr; extendBarrierFront(c);
  
  if(!nonbitrunc) for(int a=-3; a<=3; a++) if(a) {
    setland((b1+a+wstep).c, a > 0 ? lr : ll);
    setland((b2+a+wstep).c, a > 0 ? xr : xl);
    setland((b3+a+wstep).c, a > 0 ? lr : xl);
    setland((b4+a+wstep).c, a > 0 ? xr : ll);
    }
  
  if(nonbitrunc) setbarrier(b1.c), setbarrier(b2.c), setbarrier(b3.c), setbarrier(b4.c);

  if(!nonbitrunc) {
    cell *cp;
    cp = (b1+wstep).c;
    cp->barleft = ll; cp->barright = lr; setbarrier(cp);
    cp = (b2+wstep).c;
    cp->barleft = xl; cp->barright = xr; setbarrier(cp);
    }
  
  return true;
  }    

void buildBarrierStrong(cell *c, int d, bool oldleft, eLand newland) {
  d %= 7;
  cellwalker bb(c, d);

  c->bardir = d;
  eLand oldland = c->land;
  landcount[newland]++;

  if(oldleft) c->barleft = oldland, c->barright = newland;
  else c->barleft = newland, c->barright = oldland;
  extendcheck(bb.c);
  }

void buildBarrierStrong(cell *c, int d, bool oldleft) {
  buildBarrierStrong(c, d, oldleft, getNewLand(c->land));
  }

void buildCrossroads2(cell *c) {

  if(buggyGeneration) return;

  if(!c) return;
  
  for(int i=0; i<c->type; i++)
    if(c->mov[i] && !c->mov[i]->landparam && c->mov[i]->mpdist < c->mpdist)
      buildCrossroads2(c->mov[i]);
  
  if(hasbardir(c))
    extendBarrier(c);

  if(c->land != laCrossroads2) return;

  if(!c->landparam) {
    for(int i=0; i<c->type; i++) { 
      cell *c2 = createMov(c, i);
      if(c2 && c2->landparam && (c2->land == laCrossroads2 || c2->land == laBarrier)) {
        for(int j=0; j<c2->type; j++) {
          createMov(c2, j);
          cell *c3 = c2->mov[j];
          if(c3 && c3->landparam && (c3->land == laCrossroads2 || c3->land == laBarrier)) {
            int h2 = c2->landparam;
            int h3 = c3->landparam;
            
            if(h2 > 100) { raiseBuggyGeneration(c2, "bad c2 landparam"); return; }
            if(h3 > 100) { raiseBuggyGeneration(c3, "bad c3 landparam"); return; }
            
            // ambiguous
            if(h2/4 == 1 && h3/4 == 3) continue;
            if(h2/4 == 3 && h3/4 == 1) continue;
            
            for(int d=0; d<c2->type; d++)
              if(emeraldtable[h2][d] == h3) {
                int nh = emeraldtable[h2][(42+d + c->spn(i) - j) % c2->type];
                if(c->landparam>0 && c->landparam != nh) { 
                  printf("CONFLICT\n"); 
                  raiseBuggyGeneration(c, "CONFLICT"); 
                  }
                c->landparam = nh;
                }
    
            if(c->landparam == 0) 
              printf("H2 = %d H3=%d\n", c2->landparam, c3->landparam);
            }
          }
        if(c->landparam == 0) {
          printf("H2 = %d\n", c2->landparam);
//        halted = true;
//        c->heat = -1;
          raiseBuggyGeneration(c, "buildCrossroads2x");
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
    for(int i=0; i<c->type; i++) if(c->land != laBarrier) { 
      cell *c2 = createMov(c, i);
      if(c2->land == laBarrier) continue;
      c2->land = laCrossroads2;
      if(!c2->landparam) buildCrossroads2(c2);
      }
    if(h/4 == 8 || h/4 == 10)
    for(int i=0; i<c->type; i++) { 
      if(c->mov[i] && c->mov[i]->landparam == h-4) {
        bool oldleft = true;
        for(int j=1; j<=3; j++) 
          if(c->mov[(i+j)%7] && c->mov[(i+j)%7]->mpdist < c->mpdist)
            oldleft = false;
            
        c->landparam = h;
        buildBarrierStrong(c, i, oldleft);
        c->landparam = h;
        extendBarrier(c);
        }
      }
    }
  }

bool buildBarrierNowall(cell *c, eLand l2, bool force) {

  int dtab[3] = {0,1,6};
  
  if(c->land == laNone) {
    printf("barrier nowall! [%p]\n", c);
    raiseBuggyGeneration(c, "barrier nowall!");
    return false;
    }
  
  for(int i=0; i<3; i++) {
    int d = (S3>3 && nonbitrunc) ? (2+(i&1)) : dtab[i];
    if(force) d=1;
    cellwalker cw(c, d);
    
    if(force || (checkBarriersNowall(cw, 0, -1) && checkBarriersNowall(cw, 0, 1))) {
      eLand l1 = c->land;
      checkBarriersNowall(cw, 0, -1, l1, l2);
      checkBarriersNowall(cw, 0, 1, l1, l2);
      extendBarrier(c);
      return true;
      }
    }
  
  return false;
  }

