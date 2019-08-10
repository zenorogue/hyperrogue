// Hyperbolic Rogue -- Barriers
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file barriers.cpp 
 *  \brief This file implements routines related to barriers (Great Walls and similar).
 */

namespace hr {

EX bool checkBarriersFront(cellwalker bb, int q IS(5), bool cross IS(false)) {

  if(!ctof(bb.at)) 
    return false;

  if(bb.at->mpdist < BARLEV) return false;
  if(bb.at->mpdist == BUGLEV) return false;
  if(bb.at->bardir != NODIR) return false;
  if(bb.spin == (PURE ? 3 : 0)) {q--; if(!q) return true; }

  if(!cross) for(int i=0; i<7; i++) {
    cellwalker bb2 = bb + i + wstep;
    if(bb2.at->bardir != NODIR) return false; 
    if(!PURE) { 
      bb2 = bb2 + 4 + wstep;
      if(bb2.at->bardir != NODIR) return false;
      }
    }

  bb += wstep;
  if(!PURE) { bb = bb + 3 + wstep + 3 + wstep; }
  return checkBarriersBack(bb, q);
  }

EX bool hasbardir(cell *c) {
  return c->bardir != NODIR && c->bardir != NOBARRIERS;
  }

/** return true if the cell c is not allowed to generate barriers because of other large things already existing nearby. */
EX void preventbarriers(cell *c) {
  if(c && c->bardir == NODIR) c->bardir = NOBARRIERS;
  }

EX bool checkBarriersBack(cellwalker bb, int q IS(5), bool cross IS(false)) {
  // printf("back, %p, s%d\n", bb.at, bb.spin);

  // if(mark) { printf("mpdist = %d [%d] bardir = %d spin=%d q=%d cross=%d\n", bb.at->mpdist, BARLEV, bb.at->bardir, bb.spin, q, cross); }
  
  if(bb.at->mpdist < BARLEV) return false;
  if(bb.at->mpdist == BUGLEV) return false;
  if(bb.at->bardir != NODIR) return false;

  // if(bb.spin == 0 && bb.at->mpdist == INFD) return true;
  
  if(!cross) for(int i=0; i<7; i++) {
    cellwalker bb2 = bb + i + wstep;
    if(bb2.at->bardir != NODIR) return false;
    if(!PURE) {
      bb2 = bb2 + 4 + wstep;
      if(bb2.at->bardir != NODIR) return false;
      }
    }

  bb = bb + 3 + wstep + (PURE ? 5 : 4) + wstep + 3;
  return checkBarriersFront(bb, q);
  }

/** warp coasts use a different algorithm for nowall barriers when has_nice_dual() is on. Check whether we should use this different algorithm when the lands are l1 and l2 */
EX bool warped_version(eLand l1, eLand l2) {
  return (has_nice_dual() && (l1 == laWarpCoast || l1 == laWarpSea || l2 == laWarpSea || l2 == laWarpCoast)) || (VALENCE == 3);
  }

EX bool checkBarriersNowall(cellwalker bb, int q, int dir, eLand l1 IS(laNone), eLand l2 IS(laNone)) {
  if(bb.at->mpdist < BARLEV && l1 == l2) return false;
  if(bb.cpeek()->bardir != NODIR && l1 == l2) return false;
  if(bb.at->bardir != NODIR && l1 == l2) return false;
  // if(bb.at->move(dir) && bb.at->move(dir)->bardir != NODIR && l1 == laNone) return false;
  // if(bb.at->move(dir) && bb.at->move(dir)->mpdist < BARLEV && l1 == laNone) return false;
  
  if(l1 != l2) { 
    bb.at->bardir = bb.spin; bb.at->barright = l2; bb.at->barleft = NOWALLSEP; 
    setland(bb.at, l1);
    }
  if(q > 20) return true;
  
  if(l1 == laNone) for(int i=0; i<bb.at->type; i++) {
    cell *c1 = bb.at->move(i);
    if(!c1) continue;
    for(int j=0; j<c1->type; j++) {
      cell *c2 = c1->move(j);
      if(!c2) continue;
      if(c2 && c2->bardir == NOBARRIERS) 
        return false;
      if(c2 && c2->bardir != NODIR && c2->barleft != NOWALLSEP) 
        return false;
      // note: "far crashes" between NOWALL lines are allowed
      }
    }
  
  if(warped_version(l1, l2)) {
    bb = bb + wstep + (2*dir) + wstep + dir;
    }
  else if(VALENCE > 3) {
    bb = bb + dir + wstep + dir;
    dir = -dir;
    swap(l1, l2);
    }
  else {
    bb = bb + (dir>0?3:4) + wstep - (dir>0?3:4);
    }
  return checkBarriersNowall(bb, q+1, -dir, l2, l1);
  }

EX eWall getElementalWall(eLand l) {
  if(l == laEAir) return waChasm;
  if(l == laEEarth) return waStone;
  if(l == laEFire) return waEternalFire;
  if(l == laEWater) return waSea;
  return waNone;
  }

EX void setbarrier(cell *c) {
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

EX void setland(cell *c, eLand l) {
  if(c->land != l)  {
    c->landparam = 0;  
    }
  if(l == laNone) {
    printf("setland\n"); // NONEDEBUG
    }
  c->land = l;
  }

EX void extendcheck(cell *c) {
  return;
  if(BITRUNCATED && c->landparam == 0 && c->barleft != NOWALLSEP) {
    raiseBuggyGeneration(c, "extend error");
    }
  }

EX bool mirrorwall(cell *c) {
  return c->barleft == laMirrored || c->barright == laMirrored;
  }
  
EX void extendBarrierFront(cell *c) {
  limitgen("extend front %p\n", c); 
  if(buggyGeneration) return;
  int ht = c->landparam;
  extendcheck(c);

  cellwalker bb(c, c->bardir); setbarrier(bb.at);
  bb += wstep;
  
  if(BITRUNCATED) {
    bb.at->barleft = c->barleft;
    bb.at->barright = c->barright;
    setbarrier(bb.at);
    if(!mirrorwall(bb.at)) 
      bb.at->landparam = (ht-4);
  //printf("[A heat %d]\n", ht-4);
  
    setland((bb + 2).cpeek(), c->barleft);
    setland((bb + 4).cpeek(), c->barright);
    
    bb = bb + 3 + wstep; 
    bb.at->barleft = c->barright;
    bb.at->barright = c->barleft;
    setbarrier(bb.at);
    if(!mirrorwall(bb.at)) 
      bb.at->landparam = (ht-4)^2;
  //printf("[B heat %d]\n", (ht-4)^2);
    
    bb = bb + 3 + wstep;
    
    bb.at->barleft = c->barleft;
    bb.at->barright = c->barright;
    if(!mirrorwall(bb.at)) 
      bb.at->landparam = ht ^ 2;
    }

//printf("[C heat %d]\n", (ht)^2);
  bb.at->bardir = bb.spin;
  bb.at->barleft = c->barright;
  bb.at->barright = c->barleft;
  // printf("#1\n");
  extendcheck(bb.at);
  extendBarrier(bb.at);
  
  for(int a=-3; a<=3; a++) if(a) {
    bb.at = c; bb.spin = c->bardir; bb += (PURE?-a:a); bb += wstep; 
    setland(bb.at, a > 0 ? c->barright : c->barleft);
    }
  }

EX void extendBarrierBack(cell *c) {
  limitgen("extend back %p\n", c); 
  if(buggyGeneration) return;
  int ht = c->landparam;
  extendcheck(c);

  cellwalker bb(c, c->bardir); setbarrier(bb.at);
  bb = bb + 3 + wstep + (PURE?5:4); 
  setland(bb.at, PURE ? c->barleft : c->barright); 
  bb = bb + wstep + 3;
  bb.at->bardir = bb.spin;
  bb.at->barleft = c->barright;
  bb.at->barright = c->barleft;
  if(!mirrorwall(bb.at)) 
    bb.at->landparam = ht ^ 11;
  extendcheck(bb.at);
//printf("[D heat %d]\n", (ht^11));

  // needed for CR2 to work
  if(BITRUNCATED) { 
    auto bb2 = bb + wstep;
    bb2.at->barleft = c->barright;
    bb2.at->barright = c->barleft;
    if(!mirrorwall(bb2.at)) 
      bb2.at->landparam = (ht^11)-4;
    }
//printf("[E heat %d]\n", (ht^11));

  // printf("#2\n");
  extendBarrier(bb.at);
  }

EX void extendNowall(cell *c) {

  c->barleft = NOWALLSEP_USED;
  cellwalker cw(c, c->bardir);
  
  bool warpv = warped_version(c->land, c->barright);
  
  if(warpv) {
    cw += wstep;
    setland(cw.at, c->barright);
    }
  else if(VALENCE > 3) {
    auto cw2 = cw + wstep;
    setland(cw2.at, c->barright);
    cw2.at->barleft = NOWALLSEP_USED;
    cw2.at->barright = c->land;
    cw2.at->bardir = cw2.spin;
    }
  
  for(int i=-1; i<2; i+=2) {  
    cellwalker cw0;
    if(warpv) {
      cw0 = cw + (2*i) + wstep;
      }
    else if(VALENCE > 3) {
      cw0 = cw + i + wstep + i; 
      }
    else {
      cw0 = cw + (i>0?3:4) + wstep - (i>0?3:4);
      //cw0 = cw + (3*i) + wstep - (3*i);
      }
    if(cw0.at->barleft != NOWALLSEP_USED) {
      cw0.at->barleft = NOWALLSEP;
      if(VALENCE > 3) {
        cw0.at->barright = c->barright;
        cw0.at->bardir = cw0.spin;
        setland(cw0.at, c->land);
        }
      else {
        setland(cw0.at, c->barright);
        cw0.at->barright = c->land;
        if(c->barright == laNone) { 
          printf("barright\n"); 
          }// NONEDEBUG
        setland(cw0.at, c->barright);
        if(warpv) cw0 += i;
        cw0.at->bardir = cw0.spin;
        if(warpv) cw0 -= i;
        }
      extendcheck(cw0.at);
      extendBarrier(cw0.at);
      }
    }
  }

bool gotit = false;

EX void extendCR5(cell *c) {
  if(!BITRUNCATED) return;
// if(c->barright == laCrossroads5) extendCR5(c);
  eLand forbidden = c->barleft;
  eLand forbidden2 = laNone;
  cellwalker cw(c, c->bardir);
  for(int u=0; u<2; u++) {
    // if(gotit) break;
    cw = cw + 2 + wstep + 2 + wstep + 5;
    if(cw.at->bardir == NODIR) {
      cw.at->landparam = 40;
      cw.at->bardir = cw.spin;
      cw.at->barright = laCrossroads5;
      eLand nland = forbidden;
      for(int i=0; i<10 && (nland == forbidden || nland == forbidden2); i++)
        nland = getNewLand(laCrossroads5);
      cw.at->barleft = forbidden2 = nland;
      landcount[nland]++;
      extendBarrier(cw.at);
      gotit = true;
      }
    else forbidden2 = cw.at->barleft;
    }
  }

EX bool isbar4(cell *c) {
  return
    c->wall == waBarrier || c->land == laElementalWall || 
    c->land == laMirrorWall || c->land == laMirrorWall2 ||
    c->land == laMercuryRiver;
  }  

EX void extendBarrier(cell *c) {
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
    if(WDIM == 3) extend3D(c);
    else extendNowall(c);
    return;
    }
  
  bool firstmirror = 
    (c->barleft == laMirrored || c->barright == laMirrored) && 
      c->barleft != laMirrored2 && c->barright != laMirrored2;
  
  if(firstmirror && c->barleft == laMirror && hrand(100) < 60) {
    cellwalker cw(c, c->bardir);
    if(BITRUNCATED) cw += wstep;
    if(cw.at->land != laMirrorWall)
      if(buildBarrier6(cw, 1)) return;
    }
    
  if(firstmirror && (PURE?c->barleft == laMirror : c->barright == laMirror) && hrand(100) < 60) {
    cellwalker cw(c, c->bardir);
    if(PURE) {
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
    if(PURE) {
      cw += wstep;
      if(isbar4(cw.at)) {
        cw = cw + wstep + 3 + wstep - 1 + wstep;
        bool b = buildBarrier4(cw.at, cw.spin, 2, oppositeElement(c->barleft, c->barright), c->barright);
        if(b) return;
        }
      else {
        bool b = buildBarrier4(c, c->bardir, 1, c->barleft, c->barright);
        if(b) return;
        }
      }
    else {
      cw = cw + 3 + wstep;
      cell *cp = (cw + 4 + wstep).at;
      if(!isbar4(cp)) {
        cw = cw + 2 + wstep;
        bool b = buildBarrier4(cw.at, cw.spin, 2, oppositeElement(c->barleft, c->barright), c->barright);
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

EX void buildBarrierForce(cell *c, int d, eLand l) {
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

EX void buildBarrier(cell *c, int d, eLand l IS(laNone)) {
  d %= 7;
  cellwalker bb(c, d);
  
  if(checkBarriersFront(bb) && checkBarriersBack(bb)) 
    buildBarrierForce(c, d, l);
  }

EX bool buildBarrier6(cellwalker cw, int type) {
  limitgen("build6 %p/%d (%d)\n", cw.at, cw.spin, type); 
  
  cellwalker b[4];
  
  if(buggyGeneration) return true;

  if(BITRUNCATED) {
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

    if(type == 1 && b[3].at->land != laMirrorWall) return false;
    if(type == 2 && (b[1] + wstep).at->land != laMirrorWall) return false;
    // if(type == 2 && b[2].at->land != laMirrorWall) return false;
    }
  
  if(false) {
    for(int z=0; z<4; z++) {
      printf("%p/%d\n", b[z].at, b[z].spin);
      b[z].at->wall = waStrandedBoat; b[z].at->land = laAlchemist;
      b[z].at->mondir = b[z].spin;
      b[z].at->mpdist = 7;
      b[z].at->item = eItem(1+z);
      buggyGeneration = true;
      }
    return true;
    }
   
  if(type == 1) {  
    if(!(PURE?checkBarriersFront:checkBarriersBack)(b[1], 6, true)) return false;
    if(!(PURE?checkBarriersFront:checkBarriersBack)(b[2], 6, true)) return false;
    }
  else {
    if(!(PURE?checkBarriersFront:checkBarriersBack)(b[0], 6, true)) return false;
    if(!(PURE?checkBarriersFront:checkBarriersBack)(b[3], 6, true)) return false;
    }
  
  for(int d=0; d<4; d++) {
    b[d].at->bardir = b[d].spin;
    
    if(PURE) {
      b[0].at->barleft = laMirrored, b[0].at->barright = laMirrored2;
      b[1].at->barleft = laMirror, b[1].at->barright = laMirrored;
      b[2].at->barleft = laMirrored2, b[2].at->barright = laMirrored;
      b[3].at->barleft = laMirrored, b[3].at->barright = laMirror;
      }
    else {    
      b[0].at->barleft = laMirror, b[0].at->barright = laMirrored;
      b[1].at->barleft = laMirrored, b[1].at->barright = laMirror;
      b[2].at->barleft = laMirrored, b[2].at->barright = laMirrored2;
      b[3].at->barleft = laMirrored2, b[3].at->barright = laMirrored;
      }
  
    (PURE?extendBarrierFront:extendBarrierBack)(b[d].at);
    }  

  if(PURE && false) {
    for(int z=0; z<4; z++)
      b[z].at->item = eItem(1+z+4*type);
    for(int a=0; a<4; a++) 
      extendBarrierBack((b[a]+wstep).at);
    }

  if(BITRUNCATED) {
    setland((cw+1).cpeek(), laMirrorWall);
    setland((cw+2).cpeek(), laMirrored);
    setland((cw+3).cpeek(), laMirrorWall2);
    setland((cw+4).cpeek(), laMirrorWall2);
    setland((cw+5).cpeek(), laMirrored);
    setland((cw+0).cpeek(), laMirrorWall);
    setland((b[0]+2).cpeek(), laMirrored);
    setland((b[3]+6).cpeek(), laMirrored2);
    setland((b[3]+5).cpeek(), laMirrored2);
    setland((b[1]-1).cpeek(), laMirrored);
    setland((b[2]-2).cpeek(), laMirrored);
    setland((b[1]-2).cpeek(), laMirrored);
    setland((b[0]-2).cpeek(), laMirror);
    cw.at->land = laMirrorWall;
    cw.at->wall = waMirrorWall;
    cw.at->landparam = 1;
    }
  else {
    setland(cw.at, laMirrorWall2);
    setland((cw+0).cpeek(), laMirrorWall2);
    setland((cw+1).cpeek(), laMirrored);
    setland((cw+2).cpeek(), laMirrored);
    setland((cw+3).cpeek(), laMirrorWall);
    setland((cw+4).cpeek(), laMirrored);
    setland((cw+5).cpeek(), laMirrorWall2);
    setland((cw+6).cpeek(), laMirrored2);

    setland((b[1]).cpeek(), laMirrorWall);
    setland((b[1]+1).cpeek(), laMirror);
    setland((b[1]+2).cpeek(), laMirrorWall);
    setland((b[1]+6).cpeek(), laMirrored);

    setland((b[0] + wstep - 2).cpeek(), laMirrored);
    setland((b[3] + wstep - 2).cpeek(), laMirrored);
    }

  return true;
  }
  
EX bool buildBarrier4(cell *c, int d, int mode, eLand ll, eLand lr) {
  limitgen("build4 %p\n", c); 
  if(buggyGeneration) return true;
  d %= 7;
  
  cellwalker cd(c, d);
  
  cellwalker b1 = cd;
  cellwalker b2 = PURE ? cd + wstep : cd + wstep + 3 + wstep + 3 + wstep;  
  cellwalker b3 = PURE ? cd - 1 + wstep + 3 : cd + wstep + 4 + wstep + 4;
  cellwalker b4 = PURE ? cd + 1 + wstep - 3 : cd + wstep - 4 + wstep - 4;
  
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
  
  c= b2.at; d=b2.spin;
  c->bardir = d, c->barleft = xl, c->barright = xr; extendBarrierBack(c);

  c= b3.at; d=b3.spin;
  c->bardir = d, c->barleft = xl, c->barright = lr; extendBarrierFront(c);

  c= b4.at; d=b4.spin;
  c->bardir = d, c->barleft = ll, c->barright = xr; extendBarrierFront(c);
  
  if(BITRUNCATED) for(int a=-3; a<=3; a++) if(a) {
    setland((b1+a).cpeek(), a > 0 ? lr : ll);
    setland((b2+a).cpeek(), a > 0 ? xr : xl);
    setland((b3+a).cpeek(), a > 0 ? lr : xl);
    setland((b4+a).cpeek(), a > 0 ? xr : ll);
    }
  
  if(PURE) setbarrier(b1.at), setbarrier(b2.at), setbarrier(b3.at), setbarrier(b4.at);

  if(BITRUNCATED) {
    cell *cp;
    cp = (b1+wstep).at;
    cp->barleft = ll; cp->barright = lr; setbarrier(cp);
    cp = (b2+wstep).at;
    cp->barleft = xl; cp->barright = xr; setbarrier(cp);
    }
  
  return true;
  }    

EX void buildBarrierStrong(cell *c, int d, bool oldleft, eLand newland) {
  d %= 7;
  cellwalker bb(c, d);

  c->bardir = d;
  eLand oldland = c->land;
  landcount[newland]++;

  if(oldleft) c->barleft = oldland, c->barright = newland;
  else c->barleft = newland, c->barright = oldland;
  extendcheck(bb.at);
  }

EX void buildBarrierStrong(cell *c, int d, bool oldleft) {
  buildBarrierStrong(c, d, oldleft, getNewLand(c->land));
  }

EX void buildCrossroads2(cell *c) {

  if(buggyGeneration) return;

  if(!c) return;
  
  for(int i=0; i<c->type; i++)
    if(c->move(i) && !c->move(i)->landparam && c->move(i)->mpdist < c->mpdist)
      buildCrossroads2(c->move(i));
  
  if(hasbardir(c))
    extendBarrier(c);

  if(c->land != laCrossroads2) return;

  if(!c->landparam) {
    for(int i=0; i<c->type; i++) { 
      cell *c2 = createMov(c, i);
      if(c2 && c2->landparam && (c2->land == laCrossroads2 || c2->land == laBarrier)) {
        for(int j=0; j<c2->type; j++) {
          createMov(c2, j);
          cell *c3 = c2->move(j);
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
                int nh = emeraldtable[h2][(42+d + c->c.spin(i) - j) % c2->type];
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
//      cell *c2 = c->move(i);
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
      if(c->move(i) && c->move(i)->landparam == h-4) {
        bool oldleft = true;
        for(int j=1; j<=3; j++) 
          if(c->modmove(i+j) && c->modmove(i+j)->mpdist < c->mpdist)
            oldleft = false;
            
        c->landparam = h;
        buildBarrierStrong(c, i, oldleft);
        c->landparam = h;
        extendBarrier(c);
        }
      }
    }
  }

#if MAXMDIM >= 4
EX void extend3D(cell *c) {
  eLand l1 = c->land;
  c->barleft = NOWALLSEP_USED;
  
  cellwalker cw(c, c->bardir);
  
  if(S3 == 5) {
    cw += wstep; cw += rev;
    cw.at->bardir = NOBARRIERS;
    setland(cw.at, laBarrier);
    }
  
  auto cw1 = cw + wstep;
  setland(cw1.at, c->barright);
  if(cw1.at->bardir == NODIR) {
    cw1.at->barleft = NOWALLSEP_USED;
    cw1.at->barright = l1;
    cw1.at->bardir = cw1.spin;
    }

  for(int j=0; j<S7; j++) if(reg3::dirs_adjacent[cw.spin][j]) {
    cellwalker bb2 = reg3::strafe(cw, j);
    if(S3 == 5) { bb2 += rev; bb2 += wstep; }

    if(bb2.at->bardir == NODIR) {
      bb2.at->bardir = bb2.spin;
      bb2.at->barleft = NOWALLSEP;
      bb2.at->barright = c->barright;
      bb2.at->land = l1;
      // bb2.at->item = itGold;
      extendBarrier(bb2.at);
      }
    }
  }

bool built = false;

EX bool buildBarrier3D(cell *c, eLand l2, int forced_dir) {
  if(forced_dir == NODIR) {
    for(int t=0; t<S7; t++) if((!c->move(t) || c->move(t)->mpdist > c->mpdist) && buildBarrier3D(c, l2, t)) return true;
    return false;
    }
  cellwalker cw(c, forced_dir);
  if(S3 == 5) { cw += wstep; cw += rev; }
  set<cell*> listed_cells = { cw.at };
  vector<cellwalker> to_test { cw };  
  for(int i=0; i<isize(to_test); i++) {
    auto bb = to_test[i];
    if(bb.at->mpdist < BARLEV) return false;
    if(bb.cpeek()->mpdist < BARLEV) return false;
    if(bb.cpeek()->bardir != NODIR) return false;
    if(S3 == 5 && (bb+rev).cpeek()->mpdist < BARLEV) return false;
    if(S3 == 5 && (bb+rev).cpeek()->bardir != NODIR) return false;
    if(bb.at->bardir != NODIR) return false;
    for(int j=0; j<S7; j++) {
      if(S3 == 5 && i <= 5) bb.at->cmove(j);
      if(reg3::dirs_adjacent[bb.spin][j] && bb.at->move(j)) {
        cellwalker bb2 = reg3::strafe(bb, j);
        if(listed_cells.count(bb2.at)) continue;
        listed_cells.insert(bb2.at);
        to_test.push_back(bb2);
        }
      }
    }

  for(int i=0; i<isize(to_test); i++) {
    auto bb = to_test[i];
    if(S3 == 5) { bb.at->bardir = NOBARRIERS; setland(bb.at, laBarrier); bb += rev; bb += wstep; }
    bb.at->land = c->land;
    bb.at->bardir = bb.spin;
    bb.at->barleft = NOWALLSEP;
    bb.at->barright = l2;
    extendBarrier(bb.at);
    }

  built = true;
  return true;
  }
#endif

EX bool buildBarrierNowall(cell *c, eLand l2, int forced_dir IS(NODIR)) {

  if(geometry == gBinary4) return false;
  #if MAXMDIM >= 4
  // 3D binary tilings create walls using their own methods
  if(WDIM == 3 && binarytiling) return false;

  if(WDIM == 3 && hyperbolic) return buildBarrier3D(c, l2, forced_dir);
  #endif

  if(c->land == laNone) {
    printf("barrier nowall! [%p]\n", c);
    raiseBuggyGeneration(c, "barrier nowall!");
    return false;
    }
  
  bool warpv = warped_version(c->land, l2);
  if(warpv && !archimedean && !pseudohept(c)) return false;
  
  int ds[MAX_EDGE];
  for(int i=0; i<c->type; i++) ds[i] = i;
  for(int j=0; j<c->type; j++) swap(ds[j], ds[hrand(j+1)]);

  for(int i=0; i<c->type; i++) {
    int d = forced_dir != NODIR ? forced_dir : (VALENCE>3) ? (2+(i&1)) : ds[i];
/*    if(warpv && GOLDBERG) {
      d = hrand(c->type); */
    if(warpv && c->move(d) && c->move(d)->mpdist < c->mpdist) continue;
    if(GOLDBERG && a4 && c->move(d) && c->move(d)->mpdist <= c->mpdist) continue;
/*      }
    else 
      d = (S3>3 && !warpv) ? (2+(i&1)) : dtab[i]; */
    cellwalker cw(c, d);
    
    eLand ws = warpv ? laWarpCoast : laNone;
    
    if(forced_dir != NODIR || (checkBarriersNowall(cw, 0, -1, ws, ws) && checkBarriersNowall(cw, 0, 1, ws, ws))) {
      eLand l1 = c->land;
      checkBarriersNowall(cw, 0, -1, l1, l2);
      checkBarriersNowall(cw, 0, 1, l1, l2);
      extendBarrier(c);
      return true;
      }
    }
  
  return false;
  }

}
