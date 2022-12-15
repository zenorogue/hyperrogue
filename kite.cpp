// Hyperbolic Rogue -- Kite-and-dart tiling
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file kite.cpp
 *  \brief Kite-and-dart tiling, both in R^2 and H^3
 */

#include "hyper.h"
namespace hr {

EX namespace kite {

EX bool in() { return cgflags & qKITE; }

#if CAP_BT

#if HDR
enum pshape {pDart, pKite};
#endif

transmatrix meuscale(ld z) {
  if(meuclid) {
    transmatrix T = Id;
    T[0][0] = z;
    T[1][1] = z;
    return T;
    }
  else 
    return xpush(log(z));
  }

transmatrix mspin(ld alpha) {
  if(meuclid)
    return spin(alpha);
  else
    return cspin(1, 2, alpha);
  }

const ld euscale = 0.5;

transmatrix meupush(ld x, ld y) {
  if(meuclid)
    return eupush(euscale * x, euscale * y);
  else
    return bt::parabolic3(x, y);
  }

hyperpoint mhpxy(ld x, ld y) {
  if(meuclid) return hpxy(euscale * x, euscale * y);
  else return bt::parabolic3(x, y) * C0;
  }

const ld phi = golden_phi;
const ld rphi = 1 / phi;

const ld down = 1 / tan(36._deg);
const ld up = 1 / tan(72._deg);

const ld dart_center = (down + 2 * up) / 3;
const ld kite_center = up;

EX pshape getshape(heptagon *h) { return pshape(h->s); }

EX pair<vector<vector<hyperpoint>>, vector<vector<ld>>> make_walls() {
  
  vector<vector<hyperpoint>> kv;
  vector<vector<ld>> weights;

  for(pshape sh: {pDart, pKite}) {
    bool kite = sh == pKite;
    
    int t = kite ? 1 : -1;
    
    ld shf = kite ? kite_center : dart_center;
  
    hyperpoint left    = mhpxy(-1, shf);
    hyperpoint right   = mhpxy( 1, shf);
    hyperpoint top     = mhpxy( 0, shf + t*up);
    hyperpoint bottom  = mhpxy( 0, shf-down);
    hyperpoint dleft   = meupush(-1, shf) * meuscale(rphi) * C0;
    hyperpoint dright  = meupush( 1, shf) * meuscale(rphi) * C0;
    hyperpoint dtop    = meupush( 0, shf+t*up) * meuscale(rphi) * C0;
    hyperpoint dbottom = meupush( 0, shf-down) * meuscale(rphi) * C0;
    
    hyperpoint dleftmid = (!kite) ? meupush(0, shf-down) * meuscale(rphi) * meupush(-1, down) * C0 : meupush(0, shf-down) * meuscale(rphi) * mspin(-36._deg) * meupush(0, down - up) * C0;
    hyperpoint drightmid = (!kite) ? meupush(0, shf-down) * meuscale(rphi) * meupush(1, down) * C0 : meupush(0, shf-down) * meuscale(rphi) * mspin(36._deg) * meupush(0, down - up) * C0;
    
    hyperpoint dcenter = meupush( 0, shf-up) * meuscale(rphi) * C0;
    
    auto pw = [&] (int id, const vector<hyperpoint> v, const vector<ld> w) { kv.push_back(v); weights.push_back(w); };

    pw(0, {left, bottom, dbottom, dleftmid, dleft}, {1,1,1,1,1});
    pw(1, {bottom, right, dright, drightmid, dbottom}, {1,1,1,1,1});
    pw(2, {right, top, dtop, dright}, {1,1,1,1});
    pw(3, {top, left, dleft, dtop}, {1,1,1,1});
    
    ld b = 10; // big weight

    pw(4, {left, bottom, top}, {1,b,b});
    pw(5, {right, bottom, top}, {1,b,b});

    if(kite) {
      pw(6, {dcenter, drightmid, dright}, {b,1,1});
      pw(7, {dcenter, dright, dtop}, {b,1,1});
      pw(8, {dcenter, dleft, dleftmid}, {b,1,1});
      pw(9, {dcenter, dtop, dleft}, {b,1,1});
      pw(10,{dbottom, drightmid, dcenter}, {1,1,b});
      pw(11,{dbottom, dcenter, dleftmid}, {1,b,1});
      }
    else {
      pw(6, {dbottom, dtop, dleftmid}, {1,b,1});
      pw(7, {dbottom, drightmid, dtop}, {1,1,b});
      pw(8, {dleftmid, dtop, dleft}, {b,b,1});
      pw(9, {drightmid, dright, dtop}, {b,1,b});
      }

    }
  
  return {kv, weights};
  }

inline void print(hstream& hs, pshape sh) { print(hs, sh == pKite ? "pKite" : "pDart"); }

EX bool no_adj;
  
struct hrmap_kite : hrmap {

  transmatrix pKite1, pKite2, pKite3, pDart1, pDart2, ipKite1, ipKite2, ipKite3, ipDart1, ipDart2;

  heptagon *origin;
  
  heptagon *getOrigin() override { return origin; }

  void find_cell_connection(cell *c, int d) override { 
    kite::find_cell_connection(c, d); 
    }

  hyperpoint get_corner(cell *c, int cid, ld cf) override {
    bool kite = getshape(c->master) == pKite;
    int t = kite ? 1 : -1;
    ld shf = kite ? kite_center : dart_center;
    
    ld mul = 3/cf;
    
    switch(cid & 3) {
      case 0: return mhpxy(-mul, (shf)*mul);
      case 1: return mhpxy(0, (shf-down)*mul);
      case 2: return mhpxy(+mul, shf*mul);
      case 3: return mhpxy(0, (shf + t*up)*mul);
      }
    
    return C0; /* unreachable! */
    }

  int shvid(cell *c) override {
    return kite::getshape(c->master);
    }

  heptagon *newtile(pshape s, int dist) {
    heptagon *h = init_heptagon(8);
    h->s = hstate(s);
    h->dm4 = h->distance = dist;
    if(bt::in() || dist == 0)
      h->c7 = newCell(meuclid ? 4 : s == pKite ? 12 : 10, h);
    return h;
    }
  
  heptagon *hspawn(heptagon *of, int our, int their, pshape s) {
    auto h = newtile(s, of->distance + (our ? 1 : -1));
    if(bt::in()) bt::make_binary_lands(of, h);
    of->c.connect(our, h, their, false);
    return h;
    }

  heptagon *create_step(heptagon *of, int dir) override {
    if(of->move(dir)) return of->move(dir);
    
    auto sh = getshape(of);
  
    if(sh == pKite && dir == 0) return hspawn(of, 0, 1, pKite);
    if(sh == pKite && dir == 1) return hspawn(of, 1, 0, pKite);    
    if(sh == pKite && dir == 2) return hspawn(of, 2, 0, pKite);
    if(sh == pKite && dir == 3) return hspawn(of, 3, 0, pDart);
    
    if(sh == pDart && dir == 1) return hspawn(of, 1, 0, pKite);
    if(sh == pDart && dir == 2) return hspawn(of, 2, 0, pDart);
    if(sh == pDart && dir == 3) of->c.connect(3, of, 3, false); /* illegal */
      
      /* generated by findmore */
  
    #define RULEFOR(sh0, dir0, z, dir1) if(sh == sh0 && dir == dir0) { heptagon *at = of; if(z true) of->c.connect(dir0, at, dir1, false); }
    #define GO(our, shape) (at = at->cmove(our)) && getshape(at) == shape && 
    #define GOIF(our, shape, their) at->cmove(our) && at->c.spin(our) == their && getshape(at->move(our)) == shape && (at = at->move(our), true) && 
    
    RULEFOR(pDart, 5, GOIF(0, pDart, 2) GO(4, pKite) GO(3, pDart), 4)
    RULEFOR(pDart, 5, GOIF(0, pDart, 2) GO(4, pDart) GO(6, pKite) GO(2, pKite), 5)
    RULEFOR(pDart, 5, GOIF(0, pDart, 2) GO(7, pKite) GO(6, pKite) GO(2, pKite), 5)
    RULEFOR(pDart, 5, GOIF(0, pKite, 3) GO(5, pKite) GO(3, pDart), 4)
    RULEFOR(pDart, 5, GOIF(0, pKite, 3) GO(5, pDart) GO(6, pKite) GO(2, pKite), 5)
    RULEFOR(pDart, 4, GOIF(0, pDart, 2) GO(7, pKite) GO(1, pKite), 4)
    RULEFOR(pDart, 4, GOIF(0, pKite, 3) GO(4, pDart) GO(2, pDart), 5)
    RULEFOR(pDart, 4, GOIF(0, pKite, 3) GO(4, pKite) GO(3, pDart), 5)
    RULEFOR(pDart, 6, GOIF(0, pDart, 2) GO(4, pDart) GO(1, pKite), 6)
    RULEFOR(pDart, 6, GOIF(0, pDart, 2) GO(4, pKite) GO(1, pKite), 6)
    RULEFOR(pDart, 6, GOIF(0, pKite, 3) GO(5, pDart) GO(1, pKite), 6)
    RULEFOR(pDart, 6, GOIF(0, pKite, 3) GO(5, pKite) GO(1, pKite), 6)
    RULEFOR(pDart, 7, GOIF(0, pDart, 2) GO(1, pKite), 7)
    RULEFOR(pDart, 7, GOIF(0, pKite, 3) GO(2, pKite), 7)
    RULEFOR(pKite, 5, GOIF(0, pDart, 1) GO(5, pDart) GO(1, pKite), 4)
    RULEFOR(pKite, 5, GOIF(0, pDart, 1) GO(5, pKite) GO(2, pKite), 4)
    RULEFOR(pKite, 5, GOIF(0, pKite, 1) GO(4, pDart) GO(1, pKite), 4)
    RULEFOR(pKite, 5, GOIF(0, pKite, 1) GO(4, pKite) GO(2, pKite), 4)
    RULEFOR(pKite, 5, GOIF(0, pKite, 2) GO(6, pKite) GO(1, pKite), 4)
    RULEFOR(pKite, 5, GOIF(0, pKite, 2) GO(6, pDart) GO(5, pDart) GO(2, pDart), 5)
    RULEFOR(pKite, 5, GOIF(0, pKite, 2) GO(6, pDart) GO(5, pKite) GO(3, pDart), 5)
    RULEFOR(pKite, 5, GOIF(0, pKite, 2) GO(7, pKite) GO(7, pDart) GO(2, pDart), 5)
    RULEFOR(pKite, 4, GOIF(0, pDart, 1) GO(4, pDart) GO(1, pKite), 5)
    RULEFOR(pKite, 4, GOIF(0, pDart, 1) GO(4, pKite) GO(1, pKite), 5)
    RULEFOR(pKite, 4, GOIF(0, pKite, 1) GO(7, pDart) GO(2, pDart), 4)
    RULEFOR(pKite, 4, GOIF(0, pKite, 1) GO(7, pKite) GO(2, pKite), 5)
    RULEFOR(pKite, 4, GOIF(0, pKite, 2) GO(5, pDart) GO(1, pKite), 5)
    RULEFOR(pKite, 4, GOIF(0, pKite, 2) GO(5, pKite) GO(1, pKite), 5)
    RULEFOR(pKite, 6, GOIF(0, pDart, 1) GO(5, pDart) GO(2, pDart), 6)
    RULEFOR(pKite, 6, GOIF(0, pDart, 1) GO(5, pKite) GO(3, pDart), 6)
    RULEFOR(pKite, 6, GOIF(0, pKite, 1) GO(4, pDart) GO(2, pDart), 6)
    RULEFOR(pKite, 6, GOIF(0, pKite, 1) GO(4, pKite) GO(3, pDart), 6)
    RULEFOR(pKite, 6, GOIF(0, pKite, 2) GO(1, pKite), 7)
    RULEFOR(pKite, 7, GOIF(0, pDart, 1) GO(2, pDart), 7)
    RULEFOR(pKite, 7, GOIF(0, pKite, 1) GO(2, pKite), 6)
    RULEFOR(pKite, 7, GOIF(0, pKite, 2) GO(3, pDart), 7)
    
    #undef RULEFOR
    #undef GO
    #undef GOIF
  
    return of->move(dir);
    }
  
  map<int, transmatrix> graphrules;
  
  int encode(pshape s0, int d0, pshape s1, int d1) {
    return d0 + d1 * 16 + s0 * 256 + s1 * 512;
    }
    
  void graphrule(pshape s0, int d0, pshape s1, int d1, transmatrix T) {
    graphrules[encode(s0, d0, s1, d1)] = T;
    }

  void make_graphrules() {
    bool f = geom3::flipped;
    bool emb = embedded_plane;
    if(emb) geom3::light_flip(true);

    pKite1 = meupush(-1, kite_center + 0) * mspin(108._deg) * meuscale(rphi) * meupush(0, down - kite_center);
    pKite2 = meupush(1, kite_center + 0) * mspin(-108._deg) * meuscale(rphi) * meupush(0, down - kite_center);
    pKite3 = meupush(0, kite_center - down) * mspin(36._deg) * meuscale(rphi) * meupush(0, down - dart_center);
    
    pDart1 = meupush(0, dart_center-down) * meuscale(rphi) * meupush(0, down - kite_center);
    pDart2 = meupush(-1, dart_center+0) * mspin(144._deg) * meuscale(rphi) * meupush(0, down - dart_center);
    
    ipKite1 = inverse(pKite1);
    ipKite2 = inverse(pKite2);
    ipKite3 = inverse(pKite3);
    
    ipDart1 = inverse(pDart1);
    ipDart2 = inverse(pDart2);
  
    /* generated with facelift */
    graphrule(pDart, 0, pDart, 1, ipKite3 * ipKite1 * ipKite1 * pKite2 * pKite2 * pKite3); // ipKite3 * ipKite1 * ipDart1 * pDart2 * pDart2 * pDart2); 
    graphrule(pDart, 0, pKite, 0, ipDart2 * ipDart2 * pDart1 * pKite1); 
    graphrule(pDart, 1, pDart, 0, ipDart2 * ipDart2 * ipDart2 * pDart1 * pKite1 * pKite3); 
    graphrule(pDart, 1, pKite, 1, ipDart2 * ipKite3 * pKite1 * pKite2); 
    graphrule(pDart, 2, pKite, 2, ipDart2 * ipDart2 * ipDart2 * pDart1 * pKite1 * pKite1); 
    graphrule(pDart, 3, pKite, 3, ipKite3 * pKite2); 
    graphrule(pKite, 0, pDart, 0, ipKite1 * ipDart1 * pDart2 * pDart2); 
    graphrule(pKite, 0, pKite, 1, ipKite1 * ipKite1 * pKite2 * pKite2); 
    graphrule(pKite, 1, pDart, 1, ipKite2 * ipKite1 * pKite3 * pDart2); 
    graphrule(pKite, 1, pKite, 0, ipKite2 * ipKite2 * pKite1 * pKite1); 
    graphrule(pKite, 2, pDart, 2, ipDart1 * ipDart2 * ipKite3 * pKite1 * pKite2 * pKite3); 
    graphrule(pKite, 2, pKite, 3, ipKite2 * pKite1); 
    graphrule(pKite, 3, pDart, 3, ipDart1 * pDart2); 
    graphrule(pKite, 3, pKite, 2, ipKite1 * pKite2); 
  
    graphrule(pDart, 4, pDart, 8, ipDart2);
    graphrule(pDart, 4, pKite, 10, ipKite3);
    graphrule(pDart, 5, pDart, 9, ipKite3 * ipKite2 * ipKite1 * pKite3 * pDart2);
    graphrule(pDart, 5, pKite, 11, ipDart2 * ipDart2 * ipDart2 * pDart1 * pKite1);
    graphrule(pKite, 4, pDart, 6, ipDart1);
    graphrule(pKite, 4, pKite, 6, ipKite2);
    graphrule(pKite, 4, pKite, 9, ipKite1);
    graphrule(pKite, 5, pDart, 7, ipDart1);
    graphrule(pKite, 5, pKite, 7, ipKite2);
    graphrule(pKite, 5, pKite, 8, ipKite1);
  
    graphrule(pDart, 6, pKite, 4, pDart1);
    graphrule(pDart, 7, pKite, 5, pDart1);
    graphrule(pDart, 8, pDart, 4, pDart2);
    graphrule(pDart, 9, pDart, 5, ipDart2 * ipKite3 * pKite1 * pKite2 * pKite3);
    graphrule(pKite, 6, pKite, 4, pKite2);
    graphrule(pKite, 7, pKite, 5, pKite2);
    graphrule(pKite, 8, pKite, 5, pKite1);
    graphrule(pKite, 9, pKite, 4, pKite1);
    graphrule(pKite, 10, pDart, 4, pKite3);
    graphrule(pKite, 11, pDart, 5, ipKite1 * ipDart1 * pDart2 * pDart2 * pDart2);

    if(emb) {
      geom3::light_flip(false);
      for(auto& g: graphrules) swapmatrix(g.second);

      geom3::light_flip(f);
      }
    }

  transmatrix adj(cell *c, int dir) override {
    if(no_adj) return Id;
    auto c1 = c->cmove(dir);
    auto code = encode(getshape(c->master), dir, getshape(c1->master), c->c.spin(dir));
    if(!graphrules.count(code)) {
      println(hlog, "rule missing: ", make_tuple(getshape(c->master), dir, getshape(c1->master), c->c.spin(dir)));
      throw 0;
      }
    return graphrules[code];
    }

  /* works only for dir = 0,1,2,3 */
  transmatrix get_tmatrix(heptagon *h2, int dir, bool inverted) {
    if(dir == 0) inverted = !inverted, h2->cmove(dir), tie(dir, h2) = make_pair(h2->c.spin(dir), h2->move(dir));
    if(inverted) {
      if(dir == 1) return getshape(h2) == pKite ? ipKite1 : ipDart1;
      if(dir == 2) return getshape(h2) == pKite ? ipKite2 : ipDart2;
      return ipKite3;
      }
    else {
      if(dir == 1) return getshape(h2) == pKite ? pKite1 : pDart1;
      if(dir == 2) return getshape(h2) == pKite ? pKite2 : pDart2;
      return pKite3;
      }
    }

  transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
    if(gmatrix0.count(h2->c7) && gmatrix0.count(h1->c7))
      return inverse_shift(gmatrix0[h1->c7], gmatrix0[h2->c7]);
    transmatrix gm = Id, where = Id;
    while(h1 != h2) {
      if(h1->distance <= h2->distance)
        where = get_tmatrix(h2, 0, true) * where, h2 = h2->cmove(0);
      else
        gm = gm * get_tmatrix(h1, 0, false), h1 = h1->cmove(0);
      }
    return gm * where;
    }

  int wall_offset(cell *c) override {
    if(WDIM == 3)
      return kite::getshape(c->master) == kite::pKite ? 10 : 0;
    else
      return hrmap::wall_offset(c);
    }
  
  hrmap_kite() {
    make_graphrules();
    origin = newtile(pKite, 0);
    }

  ~hrmap_kite() {
    clearfrom(origin);
    }

  };

EX hrmap *new_map() { return new hrmap_kite; }
hrmap_kite *kite_map() { return (hrmap_kite*) currentmap; }

void con(cell *c0, int d0, cell *c1, int d1) {
  c0->c.connect(d0, c1, d1, false);
  }

EX void find_cell_connection(cell *c, int d) {
  auto h0 = c->master;
  auto sh = getshape(h0);  
  auto crule = [&] (pshape s0, int d0, pshape s1, int d1, pshape sparent, int child, int sibling, int rsibling) {
    if(sh == s0 && d == d0) {
      auto h = h0->cmove(child);
      if(getshape(h) != sparent) { printf("bad sparent\n"); exit(1); }
      if(sibling != 8) h = h->cmove(sibling);
      if(getshape(h) != s1) { printf("bad s1\n"); exit(1); }
      con(c, d0, h->c7, d1);
      // c->c.connect(d0, h->c7, d1, false);
      }
    if(sh == s1 && d == d1 && sibling == 8 && getshape(h0->cmove(0)) == s0 && h0->c.spin(0) == child)
      con(c, d1, h0->cmove(0)->c7, d0);
      // c->c.connect(d1, h0->cmove(0)->c7, d0, false);
    if(sh == s1 && d == d1 && sibling != 8 && (h0->cmove(rsibling), h0->c.spin(rsibling) == sibling) && getshape(h0->cmove(rsibling)) == sparent && getshape(h0->cmove(rsibling)->cmove(0)) == s0)
      // c->c.connect(d1, h0->cmove(sibling)->cmove(0)->c7, d0, false);
      con(c, d1, h0->cmove(rsibling)->cmove(0)->c7, d0);
    };
  
  if(d < 4) {
    int dx = d;
    dx += 4;
    heptagon *h1 = h0->cmove(dx);
    dx = h0->c.spin(dx);
    dx -= 4;
    // c->c.connect(d, h1->c7, h0->c.spin(4+d)-4, false);
    con(c, d, h1->c7, dx);
    return;
    }

  crule(pDart, 6, pKite, 4, pDart, 2, 7, 7);
  crule(pDart, 6, pKite, 4, pKite, 1, 8, 8);
  crule(pDart, 7, pKite, 5, pDart, 2, 7, 7);
  crule(pDart, 7, pKite, 5, pKite, 1, 8, 8);
  crule(pDart, 8, pDart, 4, pDart, 2, 8, 8);
  crule(pDart, 8, pDart, 4, pKite, 1, 7, 7);
  crule(pDart, 9, pDart, 5, pKite, 1, 6, 6);
  crule(pKite, 10, pDart, 4, pDart, 3, 8, 8);
  crule(pKite, 10, pDart, 4, pKite, 2, 7, 7);
  crule(pKite, 11, pDart, 5, pDart, 3, 4, 5);
  crule(pKite, 11, pDart, 5, pKite, 1, 6, 6);
  crule(pKite, 6, pKite, 4, pDart, 3, 7, 7);
  crule(pKite, 6, pKite, 4, pKite, 1, 7, 6);
  crule(pKite, 6, pKite, 4, pKite, 2, 8, 8);
  crule(pKite, 7, pKite, 5, pDart, 3, 7, 7);
  crule(pKite, 7, pKite, 5, pKite, 1, 7, 6);
  crule(pKite, 7, pKite, 5, pKite, 2, 8, 8);
  crule(pKite, 8, pKite, 5, pKite, 1, 8, 8);
  crule(pKite, 8, pKite, 5, pKite, 2, 6, 7);
  crule(pKite, 9, pKite, 4, pKite, 1, 8, 8);
  crule(pKite, 9, pKite, 4, pKite, 2, 6, 7);
  if(!c->move(d)) {
    println(hlog, "connection rule missing: ", d);
    throw "connection rule missing";
    }
  }

auto hooksw = addHook(hooks_swapdim, 100, [] { if(kite::in() && currentmap) kite_map()->make_graphrules(); });
#endif
}}

   
