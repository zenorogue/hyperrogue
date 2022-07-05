// Hyperbolic Rogue -- Arnold's cat map
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file asonov.cpp 
 *  \brief Arnold's cat map
 */


#include "hyper.h"

//#include <cstdio>
//#include <cmath>

namespace hr {

EX namespace asonov {

#if !CAP_SOLV
#if HDR
inline bool in() { return false; }
#endif
#endif

EX int period_xy = 8;
EX int period_z = 8;

#if CAP_SOLV

EX bool in() { return cgflags & qCAT; }

EX hyperpoint tx, ty, tz;
EX transmatrix straighten;

#if HDR
struct coord: public array<int,3> {
  coord() {}
  coord(int x, int y, int z) : array<int,3>(make_array(zgmod(x, period_xy), zgmod(y, period_xy), zgmod(z, period_z))) {}
  coord shift(int x, int y, int z=0) { return coord(self[0]+x, self[1]+y, self[2]+z); }
  coord up() { return coord(self[0]*2-self[1], self[1]-self[0], self[2]+1); }
  coord down() { return coord(self[0]+self[1], self[0]+self[1]*2, self[2]-1); }  
  coord addmove(int d);
  coord operator - (coord b);
  };
#endif

coord coord::addmove(int d) {
  switch(d) {
    case 0: return up().shift(0, 0);
    case 1: return up().shift(1, -1);
    case 2: return up().shift(-1, 0);
    case 3: return up().shift(0, -1);
    case 4: return shift(1, 0);
    case 5: return shift(0, 1);
    case 6: return down().shift(0, 0);
    case 7: return down().shift(0, 1);
    case 8: return down().shift(1, 1);
    case 9: return down().shift(1, 2);
    case 10: return shift(-1, 0);
    case 11: return shift(0, -1);
    default: throw hr_exception("error");
    }
  }

EX void prepare() {
  using namespace hr;
  transmatrix A = Id;
  A[0][0] = 1;
  A[0][1] = 1;
  A[1][0] = 1;
  A[1][1] = 2;
  
  double det = hr::det(A);
  if(det != 1) { printf("wrong det\n"); return; }
  
  // (a00-x)(a11-x) - a01*a10 = 0
  
  // x^2 - (a00+a11) x + 1 = 0
  
  double b = (A[0][0] + A[1][1]) / 2;

  // x^2 - 2b x + b^2 = b^2-1
  
  // if(b*b <= 1) { printf("imaginary eigenvalues\n"); return 0; }
  
  // x = b + sqrt(b^2-1)
  
  hyperpoint lambda;
  lambda[0] = b + sqrt(b*b-1);
  lambda[1] = b - sqrt(b*b-1);
  
  DEBB(DF_GEOM, ("b = ", b, " lambda = ", lambda));
  
  transmatrix eigen = Id;
  
  for(int i: {0,1}) {    
    eigen[0][i] = 1;
    eigen[1][i] = (lambda[i] - A[0][0]) / A[0][1];
    }
  
  transmatrix ieigen = inverse(eigen);
  
  tx = point3(ieigen[0][0], ieigen[1][0], 0) * vid.binary_width;
  ty = point3(ieigen[0][1], ieigen[1][1], 0) * vid.binary_width;
  tz = -point3(0, 0, log(lambda[0]));
  
  DEBB(DF_GEOM, ("tx = ", tx, " ty = ", ty, " tz = ", tz));
  
  straighten = inverse(build_matrix(asonov::tx/2, asonov::ty/2, asonov::tz/2, C0));
  }

EX void prepare_walls() {

  auto& hsh = get_hsh();
  auto& cs = hsh.faces;
  cs.clear();
  
  auto pt = [&] (int x, int y, int z) { return asonov::tx*x/2 + asonov::ty*y/2 + asonov::tz*z/2 + C0; };
  
  cs.push_back({pt(-1,-1,+1), pt(00,+1,+1), pt(+1,+1,+1)});
  cs.push_back({pt(00,-1,+1), pt(+1,+1,+1), pt(+1,-1,+1)});
  cs.push_back({pt(-1,+1,+1), pt(00,+1,+1), pt(-1,-1,+1)});
  cs.push_back({pt(-1,-1,+1), pt(+1,+1,+1), pt(00,-1,+1)});
  cs.push_back({pt(+1,-1,-1), pt(+1,00,-1), pt(+1,+1,-1), pt(+1,+1,+1), pt(+1,-1,+1)});
  cs.push_back({pt(-1,+1,-1), pt(-1,+1,+1), pt(00,+1,+1), pt(+1,+1,+1), pt(+1,+1,-1)});
  cs.push_back({pt(-1,-1,-1), pt(-1,00,-1), pt(+1,-1,-1)});
  cs.push_back({pt(-1,00,-1), pt(-1,+1,-1), pt(+1,-1,-1)});
  cs.push_back({pt(-1,+1,-1), pt(+1,00,-1), pt(+1,-1,-1)});
  cs.push_back({pt(-1,+1,-1), pt(+1,+1,-1), pt(+1,00,-1)});
  cs.push_back({pt(-1,+1,-1), pt(-1,00,-1), pt(-1,-1,-1), pt(-1,-1,+1), pt(-1,+1,+1)});
  cs.push_back({pt(+1,-1,-1), pt(+1,-1,+1), pt(00,-1,+1), pt(-1,-1,+1), pt(-1,-1,-1)});
  
  hsh.compute_hept();
  }

transmatrix coord_to_matrix(coord c, coord zero) {
  transmatrix T = Id;

  while(zero[2] != c[2]) {
    int z = szgmod(c[2] - zero[2], period_z);
    if(z > 0) zero = zero.up(), T = eupush(tz) * eupush(ty/2) * T;
    else zero = zero.down(), T = eupush(-ty/2) * eupush(-tz) * T;
    }
  return T * eupush(tx * szgmod(c[0]-zero[0], period_xy) + ty * szgmod(c[1]-zero[1], period_xy));
  }

coord coord::operator - (coord b) {
  auto c = self;
  while(b[2]) {
    int z = szgmod(b[2], period_z);
    if(z > 0) b = b.down(), c = c.down();
    else if(z < 0) b = b.up(), c = c.up();
    }
  c[0] = zgmod(c[0]-b[0], period_xy); b[0] = 0;
  c[1] = zgmod(c[1]-b[1], period_xy); b[1] = 0;
  return c;
  }

EX transmatrix adjmatrix(int i) {
  dynamicval<int> pxy(period_xy, 64);
  dynamicval<int> pz(period_z, 64);
  coord zero(0,0,0);
  coord c = zero.addmove(i);
  return coord_to_matrix(c, zero);
  }
  
struct hrmap_asonov : hrmap {
  map<coord, heptagon*> at;
    map<heptagon*, coord> coords;
    
  heptagon *getOrigin() override { return get_at(coord(0,0,0)); }
    
  hrmap_asonov() { prepare(); }
  
  ~hrmap_asonov() {
    for(auto& p: at) clear_heptagon(p.second);
    }

  heptagon *get_at(coord c) {
    auto& h = at[c];
    if(h) return h;
    h = init_heptagon(S7);
    h->c7 = newCell(S7, h);
    coords[h] = c;
    h->distance = c[2];
    h->zebraval = c[0];
    h->emeraldval = c[1];
    return h;      
    }
  
  heptagon *create_step(heptagon *parent, int d) override {
    auto p = coords[parent];
    auto q = p.addmove(d);
    auto child = get_at(q);
    parent->c.connect(d, child, (d + 6) % 12, false);
    return child;
    }
  
  transmatrix adj(heptagon *h, int i) override { return adjmatrix(i); }
  
  transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override { 
    for(int a=0; a<S7; a++) if(h2 == h1->move(a)) return adjmatrix(a);
    return coord_to_matrix(coords[h2], coords[h1]);
    }
  };

EX hrmap *new_map() { return new hrmap_asonov; }

EX coord get_coord(heptagon *h) { return ((hrmap_asonov*)currentmap)->coords[h]; }

EX heptagon *get_at(coord where) { return ((hrmap_asonov*)currentmap)->at[where]; }

EX int period_xy_edit, period_z_edit;

EX void set_flags() {
  auto& flag = ginf[gArnoldCat].flags;
  set_flag(flag, qANYQ, period_xy || period_z);
  set_flag(flag, qCLOSED, period_xy && period_z);
  set_flag(flag, qSMALL, period_xy && period_z && (period_xy * period_xy * period_z <= 4096));
  set_flag(flag, qHUGE_BOUNDED, period_xy * period_xy * period_z > 16384);
  }

EX void prepare_config() {
  period_xy_edit = period_xy;
  period_z_edit = period_z;
  }
  
EX void show_config() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("Solv quotient spaces"));

  dialog::addSelItem(XLAT("%1 period", "X/Y"), its(period_xy_edit), 'x');
  dialog::add_action([=] { 
    dialog::editNumber(period_xy_edit, 0, 64, 1, 0, XLAT("%1 period", "X/Y"), 
      XLAT("Note: the value 0 functions effectively as the size of int (2^32).")
      ); 
    dialog::bound_low(0);
    });      

  dialog::addSelItem(XLAT("%1 period", "Z"), its(period_z_edit), 'z');
  dialog::add_action([=] { 
    dialog::editNumber(period_z_edit, 0, 64, 1, 0, XLAT("%1 period", "Z"), 
      XLAT("Set to 0 to make it non-periodic.")
      ); 
    dialog::bound_low(0);
    });      

  dialog::addBreak(50);

  dialog::addItem(XLAT("activate"), 'a');
  dialog::add_action([] {
    stop_game();
    period_xy = period_xy_edit;
    period_z = period_z_edit;
    set_flags();
    geometry = gArnoldCat;
    start_game();
    });
    
  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  }

#endif

}
}
