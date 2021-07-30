// Hyperbolic Rogue -- Barriers
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file barriers.cpp 
 *  \brief This file implements routines related to barriers (Great Walls and similar).
 */

#include "hyper.h"
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

/** return true if the cell c is not allowed to generate barriers because of other large things already existing nearby. */
EX bool hasbardir(cell *c) {
  return c->bardir != NODIR && c->bardir != NOBARRIERS;
  }

EX void preventbarriers(cell *c) {
  if(hybri) c = hybrid::get_where(c).first;
  if(c && c->bardir == NODIR) c->bardir = NOBARRIERS;
  }

EX bool checkBarriersBack(cellwalker bb, int q IS(5), bool cross IS(false)) {
  // printf("back, %p, s%d\n", hr::voidp(bb.at), bb.spin);

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
  return (has_nice_dual() && (l1 == laWarpCoast || l1 == laWarpSea || l2 == laWarpSea || l2 == laWarpCoast));
  }

EX int get_valence(cellwalker bb, int dir, bool& ok) {
  int steps = 0;
  cellwalker bb1 = bb;
  while(bb1 != bb || !steps) {
    bb1 += dir;
    bb1 += wstep;
    steps++;
    }
  return steps;
  }

EX void set_and_wall(cell *c, eLand l) {
  setland(c, l);
  if(c->bardir == NODIR) {
    c->barleft = NOWALLSEP_USED;
    c->bardir = NOBARRIERS;
    }
  }

EX void surround_by(bool setit, cellwalker bb, int dir, int a, int b, eLand which, bool swapped, bool& ok) {
  for(int i=0; i<a; i++) {
    bb += dir;
    bb += wstep;
    }
  for(int i=a; i<b; i++) {
    if(setit) set_and_wall(bb.at, which);
    else {
      if(bb.at->bardir != NODIR) ok = false;
      if(swapped && bb.at->mpdist < BARLEV) ok = false;
      }
    bb += dir;
    bb += wstep;
    }
  }

EX void spin_around_by(cellwalker& bb, int dir, int q) {
  for(int i=0; i<q; i++) {
    bb += dir;
    bb += wstep;
    }
  }

EX bool advance_nowall(cellwalker& bb, int& dir, eLand& l1, eLand& l2, eLand& ws, bool setit) {
  bool ok = true;
  if(ws == NOWALLSEP_WALL) {
  

    /*
    if(setit) bb.at->monst = moBug0;
    if(setit) bb.cpeek()->monst = moBug2;
    */

    int steps = get_valence(bb, dir, ok);
    if(steps % 2 == 0) goto again;

    int s = (steps - 1) / 2;
    surround_by(setit, bb, dir, 1, s, l1, false, ok);
    surround_by(setit, bb, dir, s+1, steps-1, l2, true, ok);
    spin_around_by(bb, dir, s);

    bool at_corner = true;
    
    int qty = 0;
    tile:
    qty++; if(qty == 100) return true;

    if(bb.at->bardir != NODIR) ok = false;
    if(bb.at->mpdist < BARLEV) ok = false;

    if(setit) {
      setland(bb.at, laBarrier);
      bb.at->barleft = NOWALLSEP_USED;
      bb.at->wall = waBarrier;
      }
    
    // if at_corner: bb is facing the tile 1 before the first inside
    int t = bb.at->type;

    int q = at_corner ? t/2 : (t-1) / 2;
    
    if(1) {
      auto bb1 = bb;
      if(at_corner) bb1 += dir;
      bb1 -= dir;
      for(int i=1; i<q; i++) {
        int d = get_valence(bb1, -dir, ok);
        surround_by(setit, bb1, -dir, 2, d, l1, false, ok);
        bb1-=dir;
        }
      }
    
    bb += dir;
    for(int i=1; i<q; i++) {
      int d = get_valence(bb, dir, ok);
      surround_by(setit, bb, dir, 2, d, l2, true, ok);
      bb+=dir;
      }

    // bb is now facing the last neighbor inside 

    if(t % 2 == (at_corner ? 1 : 0)) {
      int d = get_valence(bb, dir, ok);
      surround_by(setit, bb, dir, 2, d, l2, true, ok);

      bb += dir;
      bb += wstep;

      d = get_valence(bb, -dir, ok);
      surround_by(setit, bb, -dir, 2, d-1, l1, false, ok);

      at_corner = false;
      goto tile;
      }
    
    int steps1 = get_valence(bb, dir, ok);
    if(steps1 % 2 == 0) {
      int s1 = steps1 / 2;
      surround_by(setit, bb, dir, 1, s1, l1, false, ok);
      surround_by(setit, bb, dir, s1+1, steps1-1, l2, true, ok);
      spin_around_by(bb, dir, s1);
      at_corner = true;
      goto tile;
      }
    int s1 = (steps - 1) / 2;
    surround_by(setit, bb, dir, 1, s1, l1, false, ok);
    surround_by(setit, bb, dir, s1+2, steps1-1, l2, true, ok);
    spin_around_by(bb, dir, s1);
    bb += dir;    
    }
  else if(warped_version(l1, l2)) {
    bb = bb + wstep + (2*dir) + wstep + dir;
    dir = -dir;
    swap(l1, l2);
    }
  else {    
    again:
    cellwalker bb1 = bb;
    int steps = get_valence(bb, dir, ok);
    int s = 2;
    if(ws == NOWALLSEP_SWAP) s = 5 - s;
    if(dir == -1) s = 5 - s;
    s = (1 + steps - s) / 2;
    surround_by(setit, bb, dir, 1, s, l1, false, ok);
    surround_by(setit, bb, dir, s+2, steps-1, l2, true, ok);
    spin_around_by(bb, dir, s);
    bb += dir;
    if(steps & 1) ws = (ws == NOWALLSEP ? NOWALLSEP_SWAP : NOWALLSEP);
    if(bb.at == bb1.at) goto again;
    }
  return ok;
  }

EX bool checkBarriersNowall(cellwalker bb, int q, int dir, eLand ws, eLand l1 IS(laNone), eLand l2 IS(laNone)) {

  if(l1 == l2) {
    if(bb.at->mpdist < BARLEV || bb.cpeek()->mpdist < BARLEV || bb.cpeek()->bardir != NODIR || bb.at->bardir != NODIR)
      return false;
    for(int i=0; i<bb.at->type; i++) {
      cell *c1 = bb.at->move(i);
      if(!c1) continue;
      if(c1->bardir != NODIR) return false;
      }
    }
  
  if(l1 != l2 && bb.at->barleft != NOWALLSEP_USED) { 
    bb.at->bardir = bb.spin; bb.at->barright = l2; bb.at->barleft = ws; 
    setland(bb.at, l1);
    }
  if(q > 20) return true;
  
  bool b = advance_nowall(bb, dir, l1, l2, ws, l1 != l2);
  if(l1 == l2 && !b) return false;
  return checkBarriersNowall(bb, q+1, dir, ws, l1, l2);
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
  limitgen("extend front %p\n", hr::voidp(c)); 
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
  limitgen("extend back %p\n", hr::voidp(c)); 
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

  eLand ws = c->barleft;
  c->barleft = NOWALLSEP_USED;
  if(c->bardir == NODIR) {
    println(hlog, "error: NODIR barrier at ", c);
    return;
    }
  cellwalker cw(c, c->bardir);
  
  eLand l1 = c->land;
  eLand l2 = c->barright;

  setland(cw.cpeek(), l2);
  cw.cpeek()->barleft = NOWALLSEP_USED;
  
  checkBarriersNowall(cw, 0, 1, ws, l1, l2);
  checkBarriersNowall(cw, 0, -1, ws, l1, l2);
  
  for(int i: {-1, 1}) {  

    cellwalker cw0 = cw;
    eLand xl1 = l1, xl2 = l2;
    eLand ws1 = ws;
    int i1 = i;
    advance_nowall(cw0, i1, xl1, xl2, ws1, true);
    if(cw0.at->barleft != NOWALLSEP_USED) {
      setland(cw0.at, xl1);
      cw0.at->barleft = ws1;
      cw0.at->barright = xl2;
      cw0.at->bardir = cw0.spin;
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
      if(ls::single() && specialland == laCrossroads5)
        nland = laCrossroads5;
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
  limitgen("extend barrier %p\n", hr::voidp(c)); 
  if(buggyGeneration) return;
  
  if(c->barleft == NOWALLSEP_USED) return;

  extendcheck(c);
  
  // printf("build barrier at %p", hr::voidp(c));
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

  if(c->barleft == NOWALLSEP || c->barleft == NOWALLSEP_SWAP || c->barleft == NOWALLSEP_WALL) {
    #if MAXMDIM >= 4
    if(WDIM == 3) extend3D(c);
    else 
    #endif
    extendNowall(c);
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
  
  if(!old_nice_walls()) {
    buildBarrierX(NOWALLSEP_WALL, c, l ? l : getNewLand(l), NODIR);
    return;
    }

  d %= 7;
  cellwalker bb(c, d);
  
  if(checkBarriersFront(bb) && checkBarriersBack(bb)) 
    buildBarrierForce(c, d, l);
  }

EX bool buildBarrier6(cellwalker cw, int type) {
  limitgen("build6 %p/%d (%d)\n", hr::voidp(cw.at), cw.spin, type); 
  
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
      printf("%p/%d\n", hr::voidp(b[z].at), b[z].spin);
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
  limitgen("build4 %p\n", hr::voidp(c)); 
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
        if(ls::single())
          buildBarrierStrong(c, i, oldleft, specialland);
        else
          buildBarrierStrong(c, i, oldleft);
        c->landparam = h;
        extendBarrier(c);
        }
      }
    }
  }

#if MAXMDIM >= 4
EX bool bufferzone() { return PURE && S7 == 6; }
EX int basic_tests() { return 50; }

EX bool valid_dir(const vector<char>& ad, int j, cell *c) {
  bool bch = variation == eVariation::bch;
  if(!bch) return ad[j] == 1;

  if(ad[j] != 2) return false;
  auto ad1 = currentmap->get_cellshape(c).dirdist[j];
  int a = 0;
  for(auto& dd: ad1) if(dd == 1) a++;

  int a0 = 0;
  for(auto& dd: ad) if(dd == 1) a0++;
  return a < 6;
  }

EX void extend3D(cell *c) {
  eLand l1 = c->land;
  c->barleft = NOWALLSEP_USED;
  
  cellwalker cw(c, c->bardir);
  
  if(bufferzone()) {
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

  bool bch = variation == eVariation::bch;

  auto& ad = currentmap->dirdist(cw);
  for(int j=0; j<cw.at->type; j++) {
    
    if(!valid_dir(ad, j, cw.at)) {
      if(bch && ad[j] == 1) {
        cell *c1 = cw.at->cmove(j);
        c1->bardir = NOBARRIERS;
        setland(c1, c->barright);
        }
  
      if(bch && ad[j] == 2) {
        cell *c1 = cw.at->cmove(j);
        c1->bardir = NOBARRIERS;
        setland(c1, l1);
        }
  
      continue;
      }
    
    cellwalker bb2 = currentmap->strafe(cw, j);
    if(bufferzone()) { bb2 += rev; bb2 += wstep; }

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
    for(int t=0; t<c->type; t++) if((!c->move(t) || c->move(t)->mpdist > c->mpdist) && buildBarrier3D(c, l2, t)) return true;
    return false;
    }
  bool bch = variation == eVariation::bch;
  
  cellwalker cw(c, forced_dir);

  if(bch) {
    auto& ad = currentmap->dirdist(cw);
    int a = 0;
    for(auto& dd: ad) if(dd == 1) a++;
    if(a == 6) return false;
    }
  
  if(bufferzone()) { cw += wstep; cw += rev; }
  set<cell*> listed_cells = { cw.at };
  vector<cellwalker> to_test { cw };
  int tc = basic_tests();
  for(int i=0; i<isize(to_test); i++) {
    auto bb = to_test[i];
    if(bb.at->mpdist < BARLEV) return false;
    if(bb.cpeek()->mpdist < BARLEV) return false;
    if(bb.cpeek()->bardir != NODIR) return false;
    if(bufferzone() && (bb+rev).cpeek()->mpdist < BARLEV) return false;
    if(bufferzone() && (bb+rev).cpeek()->bardir != NODIR) return false;
    if(bb.at->bardir != NODIR) return false;
    auto& ad = currentmap->dirdist(bb);
    for(int j=0; j<bb.at->type; j++) {
      if(i < tc) bb.at->cmove(j);
      if(!bb.at->move(j)) continue;
      if(!valid_dir(ad, j, bb.at)) continue;
      cellwalker bb2 = currentmap->strafe(bb, j);
      if(listed_cells.count(bb2.at)) continue;
      listed_cells.insert(bb2.at);
      to_test.push_back(bb2);
      }
    }

  for(int i=0; i<isize(to_test); i++) {
    auto bb = to_test[i];
    if(bufferzone()) { bb.at->bardir = NOBARRIERS; setland(bb.at, laBarrier); bb += rev; bb += wstep; }
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
  return buildBarrierX(NOWALLSEP, c, l2, forced_dir);
  }

EX bool buildBarrierX(eLand ws, cell *c, eLand l2, int forced_dir IS(NODIR)) {

  if(S3 >= OINF) { c->land = l2; return true; }

  if(geometry == gBinary4) return false;
  #if MAXMDIM >= 4
  // 3D binary tilings create walls using their own methods
  if(WDIM == 3 && bt::in()) return false;

  if(WDIM == 3 && hyperbolic) return buildBarrier3D(c, l2, forced_dir);
  #endif

  if(c->land == laNone) {
    printf("barrier nowall! [%p]\n", hr::voidp(c));
    raiseBuggyGeneration(c, "barrier nowall!");
    return false;
    }
  
  bool warpv = warped_version(c->land, l2);
  if(warpv && !arcm::in() && !pseudohept(c)) return false;
  
  vector<int> ds = hrandom_permutation(c->type);
  
  eLand wsx = warpv ? laWarpCoast : laNone;
  eLand l1 = c->land;
  
  if(forced_dir != NODIR) {
    cellwalker cw(c, forced_dir);
    checkBarriersNowall(cw, 0, -1, ws, l1, l2);
    checkBarriersNowall(cw, 0, 1, ws, l1, l2);
    extendBarrier(c);
    return true;
    }

  for(int d: ds) {
    if(c->move(d) && c->move(d)->mpdist <= c->mpdist) continue;    
    cellwalker cw(c, d);
    if(checkBarriersNowall(cw, 0, -1, ws, wsx, wsx) && checkBarriersNowall(cw, 0, 1, ws, wsx, wsx)) {
      checkBarriersNowall(cw, 0, -1, ws, l1, l2);
      checkBarriersNowall(cw, 0, 1, ws, l1, l2);
      extendBarrier(c);
      return true;
      }
    }
  
  return false;
  }

}
