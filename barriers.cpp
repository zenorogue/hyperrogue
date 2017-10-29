// This file implements routines related to barriers (Great Walls and similar).

bool checkBarriersFront(cellwalker bb, int q, bool cross) {

  if(!ctof(bb.c)) 
    return false;

  if(bb.c->mpdist < BARLEV) return false;
  if(bb.c->mpdist == BUGLEV) return false;
  if(bb.c->bardir != NODIR) return false;
  if(bb.spin == (purehepta ? 3 : 0)) {q--; if(!q) return true; }

  if(!cross) for(int i=0; i<7; i++) {
    cellwalker bb2 = bb;
    cwspin(bb2, i); cwstep(bb2); 
    if(bb2.c->bardir != NODIR) return false; 
    if(!purehepta) { 
      cwspin(bb2, 4); cwstep(bb2);
      if(bb2.c->bardir != NODIR) return false;
      }
    }

  cwstep(bb); 
  if(!purehepta) { cwspin(bb, 3); cwstep(bb); cwspin(bb, 3); cwstep(bb); }
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
    cellwalker bb2 = bb;
    cwspin(bb2, i); cwstep(bb2); 
    if(bb2.c->bardir != NODIR) return false;
    if(!purehepta) {
      cwspin(bb2, 4); cwstep(bb2);
      if(bb2.c->bardir != NODIR) return false;
      }
    }

  cwspin(bb, 3); cwstep(bb); cwspin(bb, purehepta ? 5 : 4); 
  // bool create = cwstepcreates(bb);
  cwstep(bb); cwspin(bb, 3); 
  // if(create && bb.spin == 0) return true;
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
  
  if(purehepta && S3==4) {
    cwspin(bb, dir);
    cwstep(bb);
    cwspin(bb, dir);
    }
  else if(purehepta) {
    cwspin(bb, 3*dir);
    cwstep(bb);
    cwspin(bb, -3*dir);
    }
  else {
    cwstep(bb);
    cwspin(bb, 2*dir);
    cwstep(bb);
    cwspin(bb, dir);
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
/*if(isHive(c->barleft) && isHive(c->barright))
    c->wall = waWaxWall, c->land = c->barleft; */
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
  if(!purehepta && c->landparam == 0 && c->barleft != NOWALLSEP) {
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
  cwstep(bb); 
  
  if(!purehepta) {
    bb.c->barleft = c->barleft;
    bb.c->barright = c->barright;
    setbarrier(bb.c);
    if(!mirrorwall(bb.c)) 
      bb.c->landparam = (ht-4);
  //printf("[A heat %d]\n", ht-4);

    cwspin(bb, 2); cwstep(bb); setland(bb.c, c->barleft); cwstep(bb);
    cwspin(bb, 2); cwstep(bb); setland(bb.c, c->barright); cwstep(bb);
    cwspin(bb, 2); 
    
    cwspin(bb, 3); cwstep(bb); 
    bb.c->barleft = c->barright;
    bb.c->barright = c->barleft;
    setbarrier(bb.c);
    if(!mirrorwall(bb.c)) 
      bb.c->landparam = (ht-4)^2;
  //printf("[B heat %d]\n", (ht-4)^2);
    cwspin(bb, 3); cwstep(bb);
    
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
    bb.c = c; bb.spin = c->bardir; cwspin(bb, purehepta?-a:a); cwstep(bb); 
    setland(bb.c, a > 0 ? c->barright : c->barleft);
    }
  }

void extendBarrierBack(cell *c) {
  limitgen("extend back %p\n", c); 
  if(buggyGeneration) return;
  int ht = c->landparam;
  extendcheck(c);

  cellwalker bb(c, c->bardir); setbarrier(bb.c);
  cwspin(bb, 3); cwstep(bb); cwspin(bb, purehepta?5:4); 
  setland(bb.c, purehepta ? c->barleft : c->barright); 
  cwstep(bb); cwspin(bb, 3);
  bb.c->bardir = bb.spin;
  bb.c->barleft = c->barright;
  bb.c->barright = c->barleft;
  if(!mirrorwall(bb.c)) 
    bb.c->landparam = ht ^ 11;
  extendcheck(bb.c);
//printf("[D heat %d]\n", (ht^11));

  // needed for CR2 to work
  if(!purehepta) { 
    cwstep(bb); 
    bb.c->barleft = c->barright;
    bb.c->barright = c->barleft;
    if(!mirrorwall(bb.c)) 
      bb.c->landparam = (ht^11)-4;
    cwstep(bb);
    }
//printf("[E heat %d]\n", (ht^11));

  // printf("#2\n");
  extendBarrier(bb.c);
  }

void extendNowall(cell *c) {

  c->barleft = NOWALLSEP_USED;
  cellwalker cw(c, c->bardir);
  
  if(!purehepta) {
    cwstep(cw);
    setland(cw.c, c->barright);
    }
  
  if(purehepta && S3 == 4) {
    cwstep(cw);
    setland(cw.c, c->barright);
    cw.c->barleft = NOWALLSEP_USED;
    cw.c->barright = c->land;
    cw.c->bardir = cw.spin;
    cwstep(cw);
    }
  
  for(int i=-1; i<2; i+=2) {  
    if(purehepta && S3==4) {
      cwspin(cw, i);
      cwstep(cw);
      cwspin(cw, i);
      }
    else if(purehepta) {
      cwspin(cw, 3*i);
      cwstep(cw);
      cwspin(cw, -3*i);
      }
    else {
      cwspin(cw, 2*i);
      cwstep(cw);
      }
    if(cw.c->barleft != NOWALLSEP_USED) {
      cw.c->barleft = NOWALLSEP;
      if(S3 == 4 && purehepta) {
        cw.c->barright = c->barright;
        cw.c->bardir = cw.spin;
        setland(cw.c, c->land);
        }
      else {
        setland(cw.c, c->barright);
        cw.c->barright = c->land;
        if(c->barright == laNone) { 
          printf("barright\n"); 
          }// NONEDEBUG
        setland(cw.c, c->barright);
        if(!purehepta) cwspin(cw, i);
        cw.c->bardir = cw.spin;
        if(!purehepta) cwspin(cw, -i);
        }
      extendcheck(cw.c);
      extendBarrier(cw.c);
      }
    if(purehepta && S3==4) {
      cwspin(cw, -i);
      cwstep(cw);
      cwspin(cw, -i);
      }
    else if(purehepta) {
      cwspin(cw, 3*i);
      cwstep(cw);
      cwspin(cw, -3*i);
      }
    else {
      cwstep(cw);
      cwspin(cw, -2*i);
      }
    }
  }

bool gotit = false;

void extendCR5(cell *c) {
  if(purehepta) return;
// if(c->barright == laCrossroads5) extendCR5(c);
  eLand forbidden = c->barleft;
  eLand forbidden2 = laNone;
  cellwalker cw(c, c->bardir);
  for(int u=0; u<2; u++) {
    // if(gotit) break;
    cwspin(cw, 2);
    cwstep(cw);
    cwspin(cw, 2);
    cwstep(cw);
    cwspin(cw, 5);
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
    if(!purehepta) cwstep(cw);
    if(cw.c->land != laMirrorWall)
      if(buildBarrier6(cw, 1)) return;
    }
    
  if(firstmirror && (purehepta?c->barleft == laMirror : c->barright == laMirror) && hrand(100) < 60) {
    cellwalker cw(c, c->bardir);
    if(purehepta) {
      cwspin(cw, -3); cwstep(cw); cwspin(cw, -3);
//    cwspin(cw, 3); cwstep(cw); cwspin(cw, -2); cwstep(cw); cwspin(cw, 3);
      }
    else {
      cwstep(cw); cwspin(cw, 3); cwstep(cw); cwspin(cw, -1); // check this
      }
    if(buildBarrier6(cw, 2)) return;    
    }
    
  if(((c->barleft == laCrossroads3 || c->barright == laCrossroads3) && hrand(100) < 66) ||
    (isElemental(c->barleft) && isElemental(c->barright) && hrand(100) < 75) 
    || (firstmirror && hrand(100) < 60)
    ) {
    
    cellwalker cw(c, c->bardir);
    if(purehepta) {
      cwstep(cw); 
      if(isbar4(cw.c)) {
        cwstep(cw); cwspin(cw, 3); cwstep(cw); cwspin(cw, -1); cwstep(cw);
        bool b = buildBarrier4(cw.c, cw.spin, 2, oppositeElement(c->barleft, c->barright), c->barright);
        if(b) return;
        }
      else {
        bool b = buildBarrier4(c, c->bardir, 1, c->barleft, c->barright);
        if(b) return;
        }
      }
    else {
      cwspin(cw, 3); cwstep(cw); 
      cell *cp = cwpeek(cw, 4);
      if(!isbar4(cp)) {
        cwspin(cw, 2);  cwstep(cw);
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
  for(int i=0; i<4; i++) b[i] = cw;
  
  if(buggyGeneration) return true;

  if(!purehepta) {
    cwstep(b[0]);
    cwspin(b[1], 1); cwstep(b[1]); cwspin(b[1], 3); cwstep(b[1]); 
    cwspin(b[2], 4); cwstep(b[2]);
    cwspin(b[3], 3); cwstep(b[3]); cwspin(b[3], 3); cwstep(b[3]);
    }
  else {
    cwspin(b[1], 3); cwstep(b[1]); cwspin(b[1], 3);
    cwspin(b[2], -2); cwstep(b[2]); cwspin(b[2], -3);
    cwspin(b[3], -3); cwstep(b[3]); cwspin(b[3], 2); cwstep(b[3]); cwspin(b[3],-3);
    if(type == 1 && b[3].c->land != laMirrorWall) return false;
    if(type == 2 && cwpeek(b[1], 0)->land != laMirrorWall) return false;
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
    if(!(purehepta?checkBarriersFront:checkBarriersBack)(b[1], 6, true)) return false;
    if(!(purehepta?checkBarriersFront:checkBarriersBack)(b[2], 6, true)) return false;
    }
  else {
    if(!(purehepta?checkBarriersFront:checkBarriersBack)(b[0], 6, true)) return false;
    if(!(purehepta?checkBarriersFront:checkBarriersBack)(b[3], 6, true)) return false;
    }
  
  for(int d=0; d<4; d++) {
    b[d].c->bardir = b[d].spin;
    
    if(purehepta) {
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
  
    (purehepta?extendBarrierFront:extendBarrierBack)(b[d].c);
    }  

  if(purehepta && false) {
    for(int z=0; z<4; z++)
      b[z].c->item = eItem(1+z+4*type);
    for(int a=0; a<4; a++) 
      extendBarrierBack(cwpeek(b[a],0));
    }

  if(!purehepta) {
    setland(cwpeek(cw, 1), laMirrorWall);
    setland(cwpeek(cw, 2), laMirrored);
    setland(cwpeek(cw, 3), laMirrorWall2);
    setland(cwpeek(cw, 4), laMirrorWall2);
    setland(cwpeek(cw, 5), laMirrored);
    setland(cwpeek(cw, 0), laMirrorWall);
    setland(cwpeek(b[0], 2), laMirrored);
    setland(cwpeek(b[3], 6), laMirrored2);
    setland(cwpeek(b[3], 5), laMirrored2);
    setland(cwpeek(b[1], -1), laMirrored);
    setland(cwpeek(b[2], -2), laMirrored);
    setland(cwpeek(b[1], -2), laMirrored);
    setland(cwpeek(b[0], -2), laMirror);
    cw.c->land = laMirrorWall;
    cw.c->wall = waMirrorWall;
    cw.c->landparam = 1;
    }
  else {
    setland(cw.c, laMirrorWall2);
    setland(cwpeek(cw, 0), laMirrorWall2);
    setland(cwpeek(cw, 1), laMirrored);
    setland(cwpeek(cw, 2), laMirrored);
    setland(cwpeek(cw, 3), laMirrorWall);
    setland(cwpeek(cw, 4), laMirrored);
    setland(cwpeek(cw, 5), laMirrorWall2);
    setland(cwpeek(cw, 6), laMirrored2);

    setland(cwpeek(b[1], 0), laMirrorWall);
    setland(cwpeek(b[1], 1), laMirror);
    setland(cwpeek(b[1], 2), laMirrorWall);
    setland(cwpeek(b[1], 6), laMirrored);

    cellwalker cf = b[0];
    cwstep(cf);
    setland(cwpeek(cf, -2), laMirrored);

    cf = b[3];
    cwstep(cf);
    setland(cwpeek(cf, -2), laMirrored);
    }

  return true;
  }
  
  

bool buildBarrier4(cell *c, int d, int mode, eLand ll, eLand lr) {
  limitgen("build4 %p\n", c); 
  if(buggyGeneration) return true;
  d %= 7;
  cellwalker b1(c, d);

  cellwalker b2(c, d);
  if(purehepta) cwstep(b2);
  else { cwstep(b2); cwspin(b2, 3); cwstep(b2); cwspin(b2, 3); cwstep(b2); }
  
  cellwalker b3(c, d);
  if(purehepta) {
    cwspin(b3, -1); cwstep(b3); cwspin(b3, 3);
    }
  else {
    cwstep(b3); cwspin(b3, 4); cwstep(b3); cwspin(b3, 4);
    }

  cellwalker b4(c, d);
  if(purehepta) {
    cwspin(b4, 1); cwstep(b4); cwspin(b4, -3);
    }
  else {
    cwstep(b4); cwspin(b4, -4); cwstep(b4); cwspin(b4, -4);
    }
  
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
  
  if(!purehepta) for(int a=-3; a<=3; a++) if(a) {
    setland(cwpeek(b1, a), a > 0 ? lr : ll);
    setland(cwpeek(b2, a), a > 0 ? xr : xl);
    setland(cwpeek(b3, a), a > 0 ? lr : xl);
    setland(cwpeek(b4, a), a > 0 ? xr : ll);
    }
  
  if(purehepta) setbarrier(b1.c), setbarrier(b2.c), setbarrier(b3.c), setbarrier(b4.c);

  if(!purehepta) {
    cell *cp;
    cp = cwpeek(b1, 0);
    cp->barleft = ll; cp->barright = lr; setbarrier(cp);
    cp = cwpeek(b2, 0);
    cp->barleft = xl; cp->barright = xr; setbarrier(cp);
    }
  
  return true;
  }    

void buildBarrierStrong(cell *c, int d, bool oldleft) {
  d %= 7;
  cellwalker bb(c, d);

  c->bardir = d;
  eLand oldland = c->land;
  eLand newland = getNewLand(oldland);
  landcount[newland]++;

  if(oldleft) c->barleft = oldland, c->barright = newland;
  else c->barleft = newland, c->barright = oldland;
  extendcheck(bb.c);
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
    int d = (S3>3 && purehepta) ? (2+(i&1)) : dtab[i];
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

