// Hyperbolic Rogue -- Blizzard
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file blizzard.cpp 
 *  \brief special graphical effects, such as the Blizzard and arrow traps
 */

#include "hyper.h"
namespace hr {

struct snowball {
  transmatrix T;
  shiftmatrix global;
  snowball *prev;
  snowball *next;
  double phase;
  snowball(int t) { T = rgpushxto0(randomPointIn(t)); phase = randd(); }
  };                                                                

#if HDR
struct blizzardcell;
#endif

struct blizzardcell {
  cell *c;
  int frame;
  int tmp;
  shiftmatrix *gm;
  char wmap;
  int inward, outward, ward;
  vector<int> qty;
  vector<snowball*> inorder, outorder;
  int inid, outid;
  ~blizzardcell() { for(auto i: inorder) delete i; }
  };

EX map<cell*, blizzardcell> blizzardcells;

EX void set_blizzard_frame(cell *c, int frameid) {
  blizzardcells[c].frame = frameid;
  }

EX vector<blizzardcell*> bcells;

int blizzard_N;

EX blizzardcell* getbcell(cell *c) {
  int i = c->listindex;
  if(i<0 || i >= blizzard_N) return NULL;
  if(bcells[i]->c != c) return NULL;
  return bcells[i];
  }

EX void drawBlizzards() {
  #if CAP_SHAPES && CAP_FIELD
  poly_outline = OUTLINE_NONE;
  auto it = blizzardcells.begin();
  bcells.clear();
  while(it != blizzardcells.end()) 
    if(it->second.frame != frameid || !gmatrix.count(it->first))
      it = blizzardcells.erase(it);
      else {
        it->second.c = it->first;
        bcells.push_back(&it->second);
        it++;
        }
  blizzard_N = isize(bcells);
  for(int i=0; i<blizzard_N; i++) {
    auto& bc = *bcells[i];
    bc.tmp = bc.c->listindex,
    bc.c->listindex = i;
    bc.gm = &gmatrix[bc.c];
    bc.wmap = windmap::at(bc.c);
    }

  for(int i=0; i<blizzard_N; i++) {
    auto& bc = *bcells[i];
    cell *c = bc.c;
    bc.inward = bc.outward = 0;
    bc.qty.resize(c->type);
    for(int i=0; i<c->type; i++) {
      int& qty = bc.qty[i];
      qty = 0;
      cell *c2 = c->move(i);
      if(!c2) continue;
      auto bc2 = getbcell(c2);
      if(!bc2) continue;
      int z = (bc2->wmap - bc.wmap) & 255;
      if(z >= windmap::NOWINDBELOW && z < windmap::NOWINDFROM)
        bc.outward += qty = z / 8;
      z = (-z) & 255;
      if(z >= windmap::NOWINDBELOW && z < windmap::NOWINDFROM)
        bc.inward += z / 8, qty = -z/8;
      }
    bc.ward = max(bc.inward, bc.outward);
    while(isize(bc.inorder) < bc.ward) {
      auto sb = new snowball(c->type);
      bc.inorder.push_back(sb);
      bc.outorder.push_back(sb);
      }
    for(auto& sb: bc.inorder) sb->prev = sb->next = NULL;
    bc.inid = 0;
    }
  
  double at = fractick(40);

  for(int i=0; i<blizzard_N; i++) {
    auto& bc = *bcells[i];
    for(auto sb: bc.inorder)
      sb->global = (*bc.gm) * sb->T;
    }

  for(int i=0; i<blizzard_N; i++) {
    auto& bc = *bcells[i];
    cell *c = bc.c;
    
    bc.outid = 0;
    
    for(int d=0; d<c->type; d++) for(int k=0; k<bc.qty[d]; k++) {
      auto& bc2 = *getbcell(c->move(d));
      auto& sball = *bc.outorder[bc.outid++];
      auto& sball2 = *bc2.inorder[bc2.inid++];
      sball.next = &sball2;
      sball2.prev = &sball;
      
      hyperpoint t = inverse_shift(sball.global, tC0(sball2.global));
      double at0 = at + sball.phase;
      if(at0>1) at0 -= 1;
      shiftmatrix tpartial = sball.global * rspintox(t) * xpush(hdist0(t) * at0);
      
      if(wmascii || wmblack)
        queuestr(tpartial, .2, ".", 0xFFFFFF);
      else
        queuepoly(tpartial, cgi.shSnowball, 0xFFFFFF80);
      }
    }

  for(int ii=0; ii<blizzard_N; ii++) {
    auto& bc = *bcells[ii];

    /* if(isNeighbor(bc.c, mouseover)) {
      if(againstWind(mouseover, bc.c))
        queuepoly(*bc.gm, cgi.shHeptaMarker, 0x00C00040);
      if(againstWind(bc.c, mouseover))
        queuepoly(*bc.gm, cgi.shHeptaMarker, 0xC0000040);
      } */

    forCellIdEx(c2, i, bc.c) if(bc.c == mouseover || c2 == mouseover) {
      color_t col = 0x00C00080;
      if(c2 == mouseover)
        col ^= 0xC0C00000;
      if(isPlayerOn(c2))
        col ^= 0x00000040;
      if(isPlayerOn(bc.c))
        col ^= 0x00000040;
      if(againstWind(bc.c, c2))
        queuepoly(*bc.gm * ddspin(bc.c, i) * xpush(cellgfxdist(bc.c, i)/2), cgi.shWindArrow, col);
      }

    int B = isize(bc.outorder);
    if(B<2) continue;
    int i = rand() % B;
    int j = rand() % (B-1);
    if(i==j) j++;
    
    if(1) {
      auto& sb1 = *bc.outorder[i];
      auto& sb2 = *bc.outorder[j];
      
      double swapcost = 0;
      if(sb1.next) swapcost -= hdist(tC0(sb1.global), tC0(sb1.next->global));
      if(sb2.next) swapcost -= hdist(tC0(sb2.global), tC0(sb2.next->global));
      if(sb1.next) swapcost += hdist(tC0(sb2.global), tC0(sb1.next->global));
      if(sb2.next) swapcost += hdist(tC0(sb1.global), tC0(sb2.next->global));
      if(swapcost < 0) {
        swap(bc.outorder[i], bc.outorder[j]);
        swap(sb1.next, sb2.next);
        if(sb1.next) sb1.next->prev = &sb1;
        if(sb2.next) sb2.next->prev = &sb2;
        }
      }
    
    if(1) {
      auto& sb1 = *bc.inorder[i];
      auto& sb2 = *bc.inorder[j];
      
      double swapcost = 0;
      if(sb1.prev) swapcost -= hdist(tC0(sb1.global), tC0(sb1.prev->global));
      if(sb2.prev) swapcost -= hdist(tC0(sb2.global), tC0(sb2.prev->global));
      if(sb1.prev) swapcost += hdist(tC0(sb2.global), tC0(sb1.prev->global));
      if(sb2.prev) swapcost += hdist(tC0(sb1.global), tC0(sb2.prev->global));
      if(swapcost < 0) {
        swap(bc.inorder[i], bc.inorder[j]);
        swap(sb1.prev, sb2.prev);
        if(sb1.prev) sb1.prev->next = &sb1;
        if(sb2.prev) sb2.prev->next = &sb2;
        }
      }
    
    auto& sbp = *bc.inorder[i];
    if(sbp.next && sbp.prev) {
      double p1 = sbp.next->phase;
      double p2 = sbp.prev->phase;
      double d = p2-p1;
      if(d<=.5) d+=1;
      if(d>=.5) d-=1;
      sbp.phase = p1 + d/2;
      if(sbp.phase >= 1) sbp.phase -= 1;
      if(sbp.phase < 0) sbp.phase += 1;
      }
    }

  for(auto bc: bcells)
    bc->c->listindex = bc->tmp;
  #endif
  }
       
EX vector<cell*> arrowtraps;

EX void drawArrowTraps() {
  for(cell *c: arrowtraps) {
    auto r = traplimits(c);
    
    try {
      shiftmatrix& t0 = gmatrix.at(r[0]);
      shiftmatrix& t1 = gmatrix.at(r[4]);
      ignore(t0);
      ignore(t1);

      #if CAP_QUEUE
      queueline(tC0(t0), tC0(t1), 0xFF0000FF, 4 + vid.linequality, PPR::ITEM);
      #endif
      #if CAP_SHAPES
      if((c->wparam & 7) == 3 && !shmup::on) {
//        queueline(t0 * randomPointIn(r[0]->type), t1 * randomPointIn(r[1]->type), 0xFFFFFFFF, 4, PPR::ITEM);
        int tt = int(fractick(64) * 401);
        
        for(int u=0; u<2; u++) {
          shiftmatrix& tu = u ? t0 : t1;
          shiftmatrix& tv = u ? t1 : t0;
          hyperpoint trel = inverse_shift(tu, tC0(tv));
          shiftmatrix tpartial = tu * rspintox(trel) * xpush(hdist0(trel) * tt / 401.0);
          tpartial = tpartial * ypush(.05);
          if(GDIM == 3) tpartial = tpartial * cspin90(1, 2);
          queuepoly(tpartial, cgi.shTrapArrow, 0xFFFFFFFF);
          }
        }
      #endif
      }
    catch(out_of_range&) {}
    }
  }

auto ccm_blizzard = addHook(hooks_clearmemory, 0, [] () {
  arrowtraps.clear();
  blizzardcells.clear();
  bcells.clear();
  }) + 
+ addHook(hooks_gamedata, 0, [] (gamedata* gd) {
  gd->store(arrowtraps);
  gd->store(blizzardcells);
  gd->store(bcells);
  gd->store(blizzard_N);
  })
+ addHook(hooks_removecells, 0, [] () {
  eliminate_if(arrowtraps, is_cell_removed);
  });

}
