// Hyperbolic Rogue -- advanced geometry
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file geometry2.cpp
 *  \brief Matrices to transform between coordinates of various cells, coordinates of cell corners, etc.
 */

#include "hyper.h"
namespace hr {

shiftmatrix &ggmatrix(cell *c);

EX void fixelliptic(transmatrix& at) {
  if(elliptic && at[LDIM][LDIM] < 0) {
    for(int i=0; i<MXDIM; i++) for(int j=0; j<MXDIM; j++)
      at[i][j] = -at[i][j];
    }
  }

EX void fixelliptic(hyperpoint& h) {
  if(elliptic && h[LDIM] < 0)
    for(int i=0; i<MXDIM; i++) h[i] = -h[i];
  }

/** find relative_matrix via recursing the tree structure */
EX transmatrix relative_matrix_recursive(heptagon *h2, heptagon *h1) {
  if(gmatrix0.count(h2->c7) && gmatrix0.count(h1->c7))
    return inverse_shift(gmatrix0[h1->c7], gmatrix0[h2->c7]);
  transmatrix gm = Id, where = Id;
  while(h1 != h2) {
    for(int i=0; i<h1->type; i++) {
      if(h1->move(i) == h2) {
        return gm * currentmap->adj(h1, i) * where;
        }
      }
    if(h1->distance > h2->distance) {
      for(int i=0; i<h1->type; i++) if(h1->move(i) && h1->move(i)->distance < h1->distance) {
        gm = gm * currentmap->adj(h1, i);
        h1 = h1->move(i);
        goto again;
        }
      }
    else {
      for(int i=0; i<h2->type; i++) if(h2->move(i) && h2->move(i)->distance < h2->distance) {
        where = currentmap->iadj(h2, 0) * where;
        h2 = h2->move(i);
        goto again;
        }
      }
    again: ;
    }
  return gm * where;
  }

transmatrix hrmap_standard::master_relative(cell *c, bool get_inverse) {
  if(0) ;
  #if CAP_IRR  
  else if(IRREGULAR) {
    int id = irr::cellindex[c];
    ld alpha = TAU / S7 * irr::periodmap[c->master].base.spin;
    return get_inverse ? irr::cells[id].rpusher * spin(-alpha-master_to_c7_angle()): spin(alpha + master_to_c7_angle()) * irr::cells[id].pusher;
    }
  #endif
  #if CAP_GP
  else if(GOLDBERG) {
    if(c == c->master->c7) {
      return spin((get_inverse?-1:1) * master_to_c7_angle());
      }
    else {
      auto li = gp::get_local_info(c);
      transmatrix T = spin(master_to_c7_angle()) * cgi.gpdata->Tf[li.last_dir][li.relative.first&GOLDBERG_MASK][li.relative.second&GOLDBERG_MASK][gp::fixg6(li.total_dir)];
      if(get_inverse) T = iso_inverse(T);
      return T;
      }
    }
  #endif
  else if(BITRUNCATED) {
    if(c == c->master->c7)
      return Id;
    return (get_inverse?cgi.invhexmove:cgi.hexmove)[c->c.spin(0)];
    }
  else if(WDIM == 3)
    return Id;
  else
    return pispin * Id;
  }

EX transmatrix calc_relative_matrix(cell *c2, cell *c1, const hyperpoint& hint) {
  return currentmap->relative_matrix(c2, c1, hint);
  }

// target, source, direction from source to target

#if CAP_GP
namespace gp { extern gp::local_info draw_li; }
#endif

transmatrix hrmap_standard::adj(heptagon *h, int d) {
  if(inforder::mixed()) {
    int t0 = h->type;
    int t1 = h->cmove(d)->type;
    int sp = h->c.spin(d);
    return spin(-d * TAU / t0) * xpush(spacedist(h->c7, d)) * spin(M_PI + TAU * sp / t1);
    }
  transmatrix T = cgi.heptmove[d];
  if(h->c.mirror(d)) T = T * Mirror;
  int sp = h->c.spin(d);
  if(sp) T = T * spin(TAU*sp/S7);
  return T;
  }

EX transmatrix relative_matrix_via_masters(cell *c2, cell *c1, const hyperpoint& hint) {
  heptagon *h1 = c1->master;
  transmatrix gm = currentmap->master_relative(c1, true);
  heptagon *h2 = c2->master;
  transmatrix where = currentmap->master_relative(c2);
  
  transmatrix U = currentmap->relative_matrix(h2, h1, hint);
  
  return gm * U * where;
  }

transmatrix hrmap_standard::relative_matrixc(cell *c2, cell *c1, const hyperpoint& hint) {
  return relative_matrix_via_masters(c2, c1, hint);
  }

transmatrix hrmap_standard::relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) {

  transmatrix gm = Id, where = Id;
  // always add to last!
//bool hsol = false;
//transmatrix sol;

  set<heptagon*> visited;
  map<ld, vector<pair<heptagon*, transmatrix>>> hbdist;

  int steps = 0;
  while(h1 != h2) {
    steps++; if(steps > 10000) {
      println(hlog, "not found"); return Id; 
      }
    if(closed_manifold) {
      transmatrix T;
      ld bestdist = 1e9;
      for(int d=0; d<S7; d++) {
        auto hm = h1->move(d);
        if(!hm) continue;
        transmatrix S = adj(h1, d);
        if(hm == h2) {
          transmatrix T1 = gm * S * where;
          auto curdist = hdist(tC0(T1), hint);
          if(curdist < bestdist) T = T1, bestdist = curdist;
          }
        if(geometry != gMinimal) for(int e=0; e<S7; e++) if(hm->move(e) == h2) {
          transmatrix T1 = gm * S * adj(hm, e) * where;
          auto curdist = hdist(tC0(T1), hint);
          if(curdist < bestdist) T = T1, bestdist = curdist;
          }
        }
      if(bestdist < 1e8) return T;
      }
    for(int d=0; d<h1->type; d++) if(h1->move(d) == h2) {
      return gm * adj(h1, d) * where;
      }
    if(among(geometry, gFieldQuotient, gBring, gMacbeath)) {
      int bestdist = 1000000, bestd = 0;
      for(int d=0; d<S7; d++) {
        int dist = celldistance(h1->cmove(d)->c7, h2->c7);
        if(dist < bestdist) bestdist = dist, bestd = d;
        }
      gm = gm * adj(h1, bestd);
      h1 = h1->move(bestd);
      }
    #if CAP_CRYSTAL
    else if(cryst) {
      for(int d3=0; d3<S7; d3++) {
        auto hm = h1->cmove(d3);
        if(visited.count(hm)) continue;
        visited.insert(hm);
        ld dist = crystal::space_distance(hm->c7, h2->c7);
        hbdist[dist].emplace_back(hm, gm * adj(h1, d3));
        }
      auto &bestv = hbdist.begin()->second;
      tie(h1, gm) = bestv.back();
      bestv.pop_back();
      if(bestv.empty()) hbdist.erase(hbdist.begin());
      }
    #endif
    else if(h1->distance < h2->distance) {
      where = iadj(h2, 0) * where;
      h2 = h2->move(0);
      }
    else {
      gm = gm * adj(h1, 0);
      h1 = h1->move(0);
      }
    }
  return gm * where;
  }

EX shiftmatrix &ggmatrix(cell *c) {
  shiftmatrix& t = gmatrix[c];
  if(t[LDIM][LDIM] == 0) {
    t.T = actual_view_transform * View * calc_relative_matrix(c, centerover, C0);
    t.shift = 0;
    }
  return t;
  }

#if HDR
struct horo_distance {
  ld a, b;
  
  void become(hyperpoint h1);
  horo_distance(hyperpoint h) { become(h); }
  horo_distance(shiftpoint h1, const shiftmatrix& T);
  bool operator < (const horo_distance z) const;
  friend void print(hstream& hs, horo_distance x) { print(hs, "[", x.a, ":", x.b, "]"); }
  };
#endif

void horo_distance::become(hyperpoint h1) {
  #if CAP_SOLV
  if(sn::in()) {
    a = abs(h1[2]);
    if(asonov::in()) h1 = asonov::straighten * h1;
    b = hypot_d(2, h1);
    }
  #else
  if(0) {}
  #endif
  #if CAP_BT
  else if(bt::in()) {
    b = intval(h1, C0);
    a = abs(bt::horo_level(h1));
    }
  #endif
  else if(hybri)
    a = 0, b = hdist(h1, C0);
  else
    a = 0, b = intval(h1, C0);
  }

horo_distance::horo_distance(shiftpoint h1, const shiftmatrix& T) {
  #if CAP_BT
  if(bt::in()) become(inverse_shift(T, h1));
  else
#endif
  if(sn::in() || hybri || nil) become(inverse_shift(T, h1));
  else
    a = 0, b = intval(h1.h, unshift(tC0(T), h1.shift));
  }

bool horo_distance::operator < (const horo_distance z) const {
  #if CAP_BT
  if(bt::in() || sn::in()) {
    if(a < z.a-1e-6) return true;
    if(a > z.a+1e-6) return false;
    }
  #endif
  return b < z.b - 1e-4;
  }

template<class T, class U> 
void virtualRebase_cell(cell*& base, T& at, const U& check) {
  horo_distance currz(check(at));
  T best_at = at;
  while(true) {
    cell *newbase = NULL;
    forCellIdCM(c2, i, base) {
      transmatrix V2 = currentmap->iadj(base, i);
      T cand_at = V2 * at;
      horo_distance newz(check(cand_at));
      if(newz < currz) {
        currz = newz;
        best_at = cand_at;
        newbase = c2;
        }
      if(arb::in()) forCellIdCM(c3, j, c2) {
        transmatrix V3 = currentmap->iadj(c2, j);
        T cand_at3 = V3 * cand_at;
        horo_distance newz3(check(cand_at3));
        if(newz3 < currz) {
          currz = newz3;
          best_at = cand_at3;
          newbase = c3;
          }
        }
      }
    if(!newbase) break;
    base = newbase;
    at = best_at;
    }
  #if MAXMDIM >= 4
  if(reg3::ultra_mirror_in()) {
    again:
    for(auto& v: cgi.ultra_mirrors) {
      T cand_at = v * at;
      horo_distance newz(check(cand_at));
      if(newz < currz) {
        currz = newz;
        at = cand_at;
        goto again;
        }
      }
    }
  #endif
  }

template<class T, class U> 
void virtualRebase(cell*& base, T& at, const U& check) {

  if(nil) {
    hyperpoint h = check(at);
    auto step = [&] (int i) {
      at = currentmap->adj(base, (i+S7/2) % S7) * at;
      base = base->cmove(i);
      h = check(at);
      };
    
    auto& nw = nilv::nilwidth;
    
    bool ss = S7 == 6;

    while(h[1] < -0.5 * nw) step(ss ? 1 : 2);
    while(h[1] >= 0.5 * nw) step(ss ? 4 : 6);
    while(h[0] < -0.5 * nw) step(0);
    while(h[0] >= 0.5 * nw) step(ss ? 3 : 4);
    while(h[2] < -0.5 * nw * nw) step(ss ? 2 : 3);
    while(h[2] >= 0.5 * nw * nw) step(ss ? 5 : 7);
    return;
    }

  if(geometry == gSol) {
    /** the general algorithm sometimes makes much more iterations than needed... try to approximate the geodesic */
    hyperpoint h = check(at);
    auto step = [&] (int i) {
      at = currentmap->iadj(base, i) * at;
      base = base->cmove(i);
      h = check(at);
      };

    auto nw = vid.binary_width * log(2);
    while(abs(h[0]) > 2) step(6);
    while(h[0] < -0.5 * nw) step(4);
    while(h[0] > +0.5 * nw) step(0);
    while(abs(h[1]) > 2) {
      step(2);
      while(h[0] < -0.5 * nw) step(4);
      while(h[0] > +0.5 * nw) step(0);
      }
    while(h[1] < -0.5 * nw) step(5);
    while(h[1] > +0.5 * nw) step(1);
    while(h[2] > 1) {
      step(2);
      while(h[0] < -0.5 * nw) step(4);
      while(h[0] > +0.5 * nw) step(0);
      while(h[1] < -0.5 * nw) step(5);
      while(h[1] > +0.5 * nw) step(1);
      }
    while(h[2] < -1) {
      step(6);
      while(h[0] < -0.5 * nw) step(4);
      while(h[0] > +0.5 * nw) step(0);
      while(h[1] < -0.5 * nw) step(5);
      while(h[1] > +0.5 * nw) step(1);
      }
    }
  /* todo variants of sol */

  if(prod) {
    auto d = product_decompose(check(at)).first;
    while(d > cgi.plevel / 2)  { 
      at = currentmap->iadj(base, base->type-1) * at; 
      base = base->cmove(base->type-1); d -= cgi.plevel;
      }
    while(d < -cgi.plevel / 2) { 
      at = currentmap->iadj(base, base->type-2) * at; 
      base = base->cmove(base->type-2); d += cgi.plevel;
      }
    auto w = hybrid::get_where(base);
    at = mscale(at, -d);
    PIU( virtualRebase(w.first, at, check) );
    at = mscale(at, +d);
    base = hybrid::get_at(w.first, w.second);
    return;
    }

  virtualRebase_cell(base, at, check);
  }

EX void virtualRebase(cell*& base, transmatrix& at) {
  virtualRebase(base, at, tC0_t);
  }

EX void virtualRebase(cell*& base, hyperpoint& h) {
  // we perform fixing in check, so that it works with larger range
  virtualRebase(base, h, [] (const hyperpoint& h) { 
    if(hyperbolic && GDIM == 2) return hpxy(h[0], h[1]);
    if(hyperbolic && GDIM == 3) return hpxy3(h[0], h[1], h[2]);
    return h; 
    });
  }

void hrmap_hyperbolic::virtualRebase(heptagon*& base, transmatrix& at) {

  while(true) {
  
    double currz = at[LDIM][LDIM];
    
    heptagon *h = base;
    
    heptagon *newbase = NULL;
    
    transmatrix bestV {};
    
    for(int d=0; d<S7; d++) {
      heptspin hs(h, d, false);
      heptspin hs2 = hs + wstep;
      transmatrix V2 = iadj(h, d) * at;
      double newz = V2[LDIM][LDIM];
      if(newz < currz) {
        currz = newz;
        bestV = V2;
        newbase = hs2.at;
        }
      }

    if(newbase) {
      base = newbase;
      at = bestV;
      continue;
      }

    return;
    }
  }

EX bool no_easy_spin() {
  return NONSTDVAR || arcm::in() || WDIM == 3 || bt::in() || kite::in();
  }

ld hrmap_standard::spin_angle(cell *c, int d) {
  if(WDIM == 3) return SPIN_NOT_AVAILABLE;
  ld hexshift = 0;
  if(c == c->master->c7 && (S7 % 2 == 0) && BITRUNCATED) hexshift = cgi.hexshift + TAU/c->type;
  else if(cgi.hexshift && c == c->master->c7) hexshift = cgi.hexshift;
  #if CAP_IRR
  if(IRREGULAR) {
    auto id = irr::cellindex[c];
    auto& vs = irr::cells[id];
    if(d < 0 || d >= c->type) return 0;
    auto& p = vs.jpoints[vs.neid[d]];
    return -atan2(p[1], p[0]) - hexshift;
    }
  #endif
  return M_PI - d * TAU / c->type - hexshift;
  }

EX transmatrix ddspin(cell *c, int d, ld bonus IS(0)) { return currentmap->spin_to(c, d, bonus); }
EX transmatrix iddspin(cell *c, int d, ld bonus IS(0)) { return currentmap->spin_from(c, d, bonus); }
EX ld cellgfxdist(cell *c, int d) { return currentmap->spacedist(c, d); }

EX transmatrix ddspin_side(cell *c, int d, ld bonus IS(0)) { 
  if(kite::in()) {
    hyperpoint h1 = get_corner_position(c, gmod(d, c->type), 3);
    hyperpoint h2 = get_corner_position(c, gmod(d+1, c->type) , 3);
    hyperpoint hm = mid(h1, h2);
    return rspintox(hm) * spin(bonus);
    }
  return currentmap->spin_to(c, d, bonus); 
  }

EX transmatrix iddspin_side(cell *c, int d, ld bonus IS(0)) {
  if(kite::in()) {
    hyperpoint h1 = get_corner_position(c, gmod(d, c->type), 3);
    hyperpoint h2 = get_corner_position(c, gmod(d+1, c->type) , 3);
    hyperpoint hm = mid(h1, h2);
    return spintox(hm) * spin(bonus);
    }
  return currentmap->spin_from(c, d, bonus);
  }
    
double hrmap_standard::spacedist(cell *c, int i) {
  if(NONSTDVAR || WDIM == 3) return hrmap::spacedist(c, i);
  if(inforder::mixed()) {
    int t0 = c->type;
    int t1 = c->cmove(i)->type;
    auto halfmove = [] (int i) {
      if(i == 1) return 0.0;
      if(i == 2) return 0.1;
      return edge_of_triangle_with_angles(0, M_PI/i, M_PI/i);
      };
    ld tessf0 = halfmove(t0);
    ld tessf1 = halfmove(t1);
    return (tessf0 + tessf1) / 2;
    }  
  if(!BITRUNCATED) return cgi.tessf;
  if(c->type == S6 && (i&1)) return cgi.hexhexdist;
  return cgi.crossf;
  }

int neighborId(heptagon *h1, heptagon *h2) {
  for(int i=0; i<h1->type; i++) if(h1->move(i) == h2) return i;
  return -1;
  }

transmatrix hrmap_standard::adj(cell *c, int i) {
  if(GOLDBERG) {
    transmatrix T = master_relative(c, true);
    transmatrix U = master_relative(c->cmove(i), false);
    heptagon *h = c->master, *h1 = c->cmove(i)->master;
    static bool first = true;
    if(h == h1)
      return T * U;
    else if(gp::do_adjm) {
      if(gp::gp_adj.count(make_pair(c,i))) {
        return T * gp::get_adj(c,i) * U;
        }
      if(first) { first = false; println(hlog, "no gp_adj"); }
      }
    else for(int i=0; i<h->type; i++) if(h->move(i) == h1)
      return T * adj(h, i) * U;
    if(first) {
      first = false;
      println(hlog, "not adjacent");
      }
    }
  if(NONSTDVAR || WDIM == 3) {
    return calc_relative_matrix(c->cmove(i), c, C0);
    }
  double d = cellgfxdist(c, i);
  transmatrix T = ddspin(c, i) * xpush(d);
  if(c->c.mirror(i)) T = T * Mirror;
  cell *c1 = c->cmove(i);
  T = T * iddspin(c1, c->c.spin(i), M_PI);
  return T;
  }

EX double randd() { return (rand() + .5) / (RAND_MAX + 1.); }

EX hyperpoint randomPointIn(int t) {
  if(NONSTDVAR || arcm::in() || kite::in()) {
    // Let these geometries be less confusing.
    // Also easier to implement ;)
    return xspinpush0(TAU * randd(), asinh(randd() / 20));
    }
  while(true) {
    hyperpoint h = xspinpush0(TAU * (randd()-.5)/t, asinh(randd()));
    double d =
      PURE ? cgi.tessf : t == 6 ? cgi.hexhexdist : cgi.crossf;
    if(hdist0(h) < hdist0(xpush(-d) * h))
      return spin(TAU / t * (rand() % t)) * h;
    }
  }

/** /brief get the coordinates of the vertex of cell c indexed with cid
 *  the two vertices c and c->move(cid) share are indexed cid and gmod(cid+1, c->type)
 *  cf=3 is the vertex itself; larger values are closer to the center
 */  

EX hyperpoint get_corner_position(cell *c, int cid, ld cf IS(3)) {
  return currentmap->get_corner(c, cid, cf);
  } 

hyperpoint hrmap_standard::get_corner(cell *c, int cid, ld cf) {
  #if CAP_GP
  if(GOLDBERG) return gp::get_corner_position(c, cid, cf);
  #endif
  #if CAP_IRR
  if(IRREGULAR) {
    auto& vs = irr::cells[irr::cellindex[c]];
    return mid_at_actual(vs.vertices[cid], 3/cf);
    }
  #endif
  if(PURE) {
    return ddspin(c,cid,M_PI/S7) * xpush0(cgi.hcrossf * 3 / cf);
    }
  if(BITRUNCATED) {
    if(!ishept(c))
      return ddspin(c,cid,M_PI/S6) * xpush0(cgi.hexvdist * 3 / cf);
    else
      return ddspin(c,cid,M_PI/S7) * xpush0(cgi.rhexf * 3 / cf);
    }
  return C0;
  }

EX bool approx_nearcorner = false;

/** /brief get the coordinates of the center of c->move(i) */

EX hyperpoint nearcorner(cell *c, int i) {
  if(GOLDBERG_INV) {
    i = gmod(i, c->type);
    cellwalker cw(c, i);
    cw += wstep;
    transmatrix cwm = currentmap->adj(c, i);
    if(elliptic && cwm[2][2] < 0) cwm = centralsym * cwm;
    return cwm * C0;
    }
  #if CAP_IRR
  if(IRREGULAR) {
    auto& vs = irr::cells[irr::cellindex[c]];
    hyperpoint nc = vs.jpoints[vs.neid[i]];
    return mid_at(C0, nc, .94);
    }
  #endif
  #if CAP_ARCM
  if(arcm::in()) {
    if(PURE) { 
      auto &ac = arcm::current;
      auto& t = ac.get_triangle(c->master, i-1);
      int id = arcm::id_of(c->master);
      int id1 = ac.get_adj(ac.get_adj(c->master, i-1), -2).first;
      return xspinpush0(-t.first - M_PI / c->type, ac.inradius[id/2] + ac.inradius[id1/2] + (ac.real_faces == 0 ? TAU / (ac.N == 2 ? 2.1 : ac.N) : 0));
      }
    if(BITRUNCATED) {
      auto &ac = arcm::current;
      auto& t = ac.get_triangle(c->master, i);
      return xspinpush0(-t.first, t.second);
      }
    if(DUAL) {
      auto &ac = arcm::current;
      auto& t = ac.get_triangle(c->master, i * 2);
      return xspinpush0(-t.first, t.second);
      }
    }
  #endif
  #if CAP_BT
  if(geometry == gBinary4) {
    ld yx = log(2) / 2;
    ld yy = yx;
    hyperpoint neis[5];
    neis[0] = bt::get_horopoint(2*yy, -0.5);
    neis[1] = bt::get_horopoint(2*yy, +0.5);
    neis[2] = bt::get_horopoint(0, 1);
    neis[3] = bt::get_horopoint(-2*yy, c->master->zebraval ? -0.25 : +0.25);
    neis[4] = bt::get_horopoint(0, -1);
    return neis[i];
    }
  if(geometry == gTernary) {
    ld yx = log(3) / 2;
    ld yy = yx;
    hyperpoint neis[6];
    neis[0] = bt::get_horopoint(2*yy, -1);
    neis[1] = bt::get_horopoint(2*yy, +0);
    neis[2] = bt::get_horopoint(2*yy, +1);
    neis[3] = bt::get_horopoint(0, 1);
    neis[4] = bt::get_horopoint(-2*yy, c->master->zebraval / 3.);
    neis[5] = bt::get_horopoint(0, -1);
    return neis[i];
    }
  if(kite::in()) {
    if(approx_nearcorner)
      return currentmap->get_corner(c, i, 3) + currentmap->get_corner(c, i+1, 3) - C0;
    else
      return calc_relative_matrix(c->cmove(i), c, C0) * C0;
    }
  if(bt::in()) {
    if(WDIM == 3) {
      println(hlog, "nearcorner called");
      return Hypc;
      }
    ld yx = log(2) / 2;
    ld yy = yx;
    // ld xx = 1 / sqrt(2)/2;
    hyperpoint neis[7];
    neis[0] = bt::get_horopoint(0, 1);
    neis[1] = bt::get_horopoint(yy*2, 1);
    neis[2] = bt::get_horopoint(yy*2, 0);
    neis[3] = bt::get_horopoint(yy*2, -1);
    neis[4] = bt::get_horopoint(0, -1);
    if(c->type == 7)
      neis[5] = bt::get_horopoint(-yy*2, -.5),
      neis[6] = bt::get_horopoint(-yy*2, +.5);
    else
      neis[5] = bt::get_horopoint(-yy*2, 0);
    return neis[i];
    }
  #endif
  double d = cellgfxdist(c, i);
  return ddspin(c, i) * xpush0(d);
  }

/** /brief get the coordinates of the another vertex of c->move(i)
 *  this is useful for tessellation remapping TODO COMMENT
 */

EX hyperpoint farcorner(cell *c, int i, int which) {
  #if CAP_GP
  if(GOLDBERG_INV) {
    cellwalker cw(c, i);
    cw += wstep;
    if(!cw.mirrored) cw += (which?-1:2);
    else cw += (which?2:-1);
    transmatrix cwm = currentmap->adj(c, i);
    if(gp::variation_for(gp::param) == eVariation::goldberg) {
      auto li1 = gp::get_local_info(cw.at);
      return cwm * get_corner_position(li1, cw.spin);
      }
    else {
      return cwm * get_corner_position(cw.at, cw.spin, 3);
      }
    }
  #endif
  #if CAP_IRR
  if(IRREGULAR) {
    auto& vs = irr::cells[irr::cellindex[c]];
    int neid = vs.neid[i];
    int spin = vs.spin[i];
    auto &vs2 = irr::cells[neid];
    int cor2 = isize(vs2.vertices);
    transmatrix rel = vs.rpusher * vs.relmatrices[vs2.owner] * vs2.pusher;

    if(which == 0) return rel * vs2.vertices[(spin+2)%cor2];
    if(which == 1) return rel * vs2.vertices[(spin+cor2-1)%cor2];
    }
  #endif
  #if CAP_BT
  if(bt::in() || kite::in())
    return nearcorner(c, (i+which) % c->type); // lazy
  #endif
  #if CAP_ARCM
  if(arcm::in()) {
    if(PURE) {
      auto &ac = arcm::current;
      auto& t = ac.get_triangle(c->master, i-1);
      int id = arcm::id_of(c->master);
      auto id1 = ac.get_adj(ac.get_adj(c->master, i-1), -2).first;
      int n1 = isize(ac.adjacent[id1]);
      return spin(-t.first - M_PI / c->type) * xpush(ac.inradius[id/2] + ac.inradius[id1/2]) * xspinpush0(M_PI + M_PI/n1*(which?3:-3), ac.circumradius[id1/2]);
      }
    if(BITRUNCATED || DUAL) {
      int mul = DUALMUL;
      auto &ac = arcm::current;
      auto adj = ac.get_adj(c->master, i * mul);
      heptagon h; cell cx; cx.master = &h;
      arcm::id_of(&h) = adj.first;
      arcm::parent_index_of(&h) = adj.second;

      auto& t1 = arcm::current.get_triangle(c->master, i);
    
      auto& t2 = arcm::current.get_triangle(adj);
      
      return spin(-t1.first) * xpush(t1.second) * spin(M_PI + t2.first) * get_corner_position(&cx, which ? -mul : 2*mul);
      }
    }
  #endif
  
  cellwalker cw(c, i);
  cw += wstep;
  if(!cw.mirrored) cw.spin += (which?-1:2);
  else cw.spin += (which?2:-1);
  return currentmap->adj(c, i) * get_corner_position(c->move(i), cw.spin);
  }

EX hyperpoint midcorner(cell *c, int i, ld v) {
  auto hcor = farcorner(c, i, 0);
  auto tcor = get_corner_position(c, i, 3);
  return mid_at(tcor, hcor, v);
  }

EX hyperpoint get_warp_corner(cell *c, int cid) {
  // midcorner(c, cid, .5) but sometimes easier versions exist
  #if CAP_GP
  if(GOLDBERG) return gp::get_corner_position(c, cid, 2);
  #endif
  #if CAP_IRR || CAP_ARCM
  if(IRREGULAR || arcm::in()) return midcorner(c, cid, .5);
  #endif
  return ddspin(c,cid,M_PI/S7) * xpush0(cgi.tessf/2);
  }

EX map<cell*, map<cell*, vector<transmatrix>>> brm_structure;

EX void generate_brm(cell *c1) {
  set<unsigned> visited_by_matrix;
  queue<pair<cell*, transmatrix>> q;
  map<cell*, ld> cutoff;
  auto& res = brm_structure[c1];
  
  auto enqueue = [&] (cell *c, const transmatrix& T) {
    auto b = bucketer(tC0(T));
    if(visited_by_matrix.count(b)) return;
    visited_by_matrix.insert(b);
    q.emplace(c, T);
    };
  
  enqueue(c1, Id);
  while(!q.empty()) {
    cell *c2;
    transmatrix T;
    tie(c2,T) = q.front();
    q.pop();
    
    ld mindist = HUGE_VAL, maxdist = 0;
    
    if(WDIM == 2) {
      for(int i=0; i<c1->type; i++)
      for(int j=0; j<c2->type; j++) {
        ld d = hdist(get_corner_position(c1, i), T * get_corner_position(c2, j));
        if(d < mindist) mindist = d;
        if(d > maxdist) maxdist = d;
        }
      }
    else {
      auto& ss1 = currentmap->get_cellshape(c1);
      auto& ss2 = currentmap->get_cellshape(c2);
      for(auto v: ss1.vertices_only)
      for(auto w: ss2.vertices_only) {
        ld d = hdist(v, T*w);
        if(d < mindist) mindist = d;
        if(d > maxdist) maxdist = d;
        }
      }
      
    auto& cu = cutoff[c2];
    if(cu == 0 || cu > maxdist) 
      cu = maxdist;
    
    if(mindist >= cu) continue;
    res[c2].push_back(T);
    
    forCellIdCM(c3, i, c2) enqueue(c3, T * currentmap->adj(c2, i));
    }
  
  vector<int> cts;
  for(auto& p: res) cts.push_back(isize(p.second));
  }

/** this function exhaustively finds the best transmatrix from (c1,h1) to (c2,h2) */
EX const transmatrix& brm_get(cell *c1, hyperpoint h1, cell *c2, hyperpoint h2) {
  if(!brm_structure.count(c1))
    generate_brm(c1);
  transmatrix *result = nullptr;
  ld best = HUGE_VAL;
  for(auto& t: brm_structure[c1][c2]) {
    ld d = hdist(h1, t * h2);
    if(d < best) best = d, result = &t;
    }
  return *result;
  }

int brm_hook = addHook(hooks_clearmemory, 0, []() {
  brm_structure.clear();
  });

EX bool exhaustive_distance_appropriate() {
  if(euclid && (kite::in() || arcm::in() || arb::in() || quotient)) return true;
  #if MAXMDIM >= 4
  if(nil && quotient) return true;
  #endif
  #if CAP_SOLV
  if(asonov::in() && asonov::period_xy && asonov::period_xy <= 256) return true;
  #endif

  if(closed_manifold) return true;

  return false;
  }

#if HDR
struct pathgen {
  cellwalker start;
  cellwalker last;
  vector<cell*> path;
  bignum full_id_0;
  int last_id;
  };
#endif

EX pathgen generate_random_path_randomdir(cellwalker start, int length, bool for_yendor) {
  start.spin = hrand(start.at->type);
  return generate_random_path(start, length, for_yendor, false);
  }

EX pathgen generate_random_path(cellwalker start, int length, bool for_yendor, bool randomdir) {
  pathgen p;
  p.start = start;
  p.path.resize(length+1);
  p.path[0] = start.at;
  p.last_id = 0;

  int turns = 0;

  if(exhaustive_distance_appropriate()) {
    permanent_long_distances(start.at);
    int dist = max_saved_distance(start.at);
    dist = min(dist, length);
    auto at = random_in_distance(start.at, dist);
    permanent_long_distances(at);
    for(int a=length-1; a>=0; a--) {
      p.path[a+1] = at;
      vector<cell*> prev;
      forCellCM(c2, at) if(celldistance(start.at, c2) == a) prev.push_back(c2);
      if(isize(prev))  at = prev[hrand(isize(prev))];
      }
    p.path[0] = start.at;
    p.last = p.path.back();
    }

  else if(hybri) {
    /* I am lazy */
    for(int i=1; i<=length; i++) p.path[i] = p.path[i-1]->cmove(p.path[i-1]->type-1);
    p.last = p.path.back();
    }

  else {
    int t = -1;
    bignum full_id;
    bool onlychild = true;
    bool launched = false;

    cellwalker ycw = start;
    if(for_yendor) setdist(p.path[0], 7, NULL);
    auto& expansion = get_expansion();

    for(int i=0; i<length; i++) {

      if(for_yendor && yendor::control(p, i, ycw)) { }

      else if(bt::in()) {
        // make it challenging
        vector<int> ds;
        for(int d=0; d<ycw.at->type; d++) {
          bool increase;
          if(sol)
            increase = i < YDIST / 4 || i > 3 * YDIST / 4;
          else
            increase = i < YDIST/2;
          if(increase) {
            if(celldistAlt((ycw+d).cpeek()) < celldistAlt(ycw.at))
              ds.push_back(d);
            }
          else {
            if(celldistAlt((ycw+d).cpeek()) > celldistAlt(ycw.at) && (ycw+d).cpeek() != p.path[i-1])
              ds.push_back(d);
            }
          }
        if(isize(ds)) ycw += ds[hrand(isize(ds))];
        }

      else if(currentmap->strict_tree_rules()) {
        if(for_yendor && i < arb::current.yendor_backsteps) {
          println(hlog, i, " < ", arb::current.yendor_backsteps);
          ycw.spin = 0;
          }

        else {
          if(!launched) {
            t = ycw.at->master->fieldval;
            bignum b = expansion.get_descendants(length-i, t);
            if(!full_id.approx_int()) goto stupid;
            p.full_id_0 = full_id = hrand(b);
            /* it may happen that the subtree dies out */
            launched = true;
            }

          ycw.spin = 0;

          auto& r = rulegen::treestates[t];
          for(int ri=0; ri<isize(r.rules); ri++) {
            int tch = r.rules[ri];
            if(tch < 0) continue;
            auto& sub_id = expansion.get_descendants(length-1-i, tch);
            if(full_id < sub_id) {
              t = tch; ycw += ri; break;
              }
            full_id.addmul(sub_id, -1);
            }
          }
        }

      else if(trees_known() && WDIM == 2) {
        auto sdist = [start] (cell *c) { return celldistance(start.at, c); };
        if(i == 0) {
          t = type_in(expansion, randomdir ? start.at : start.cpeek(), sdist);
          ycw--;
          if(valence() == 3) ycw--;
          bignum b = get_expansion().get_descendants(randomdir ? length : length-1, t);
          p.full_id_0 = full_id = hrand(b);
          }

        #if DEBUG_YENDORGEN
        printf("#%3d t%d %s / %s\n", i, t, full_id.get_str(100).c_str(), expansion.get_descendants(length-i, t).get_str(100).c_str());
        for(int tch: expansion.children[t]) {
          printf("     t%d %s\n", tch, expansion.get_descendants(length-i-1, t).get_str(100).c_str());
          }
        #endif

        if(i == 1)
          onlychild = true;
        if(!onlychild) ycw++;
        if(valence() == 3) ycw++;

        onlychild = false;

        for(int tch: expansion.children[t]) {
          ycw++;
          if(i < 2) tch = type_in(expansion, ycw.cpeek(), sdist);
          auto& sub_id = expansion.get_descendants(length-1-i, tch);
          if(full_id < sub_id) { t = tch; break; }

          full_id.addmul(sub_id, -1);
          onlychild = true;
          }
        }

      else if(WDIM == 3) {
        cell *prev = p.path[max(i-3, 0)];
        int d = celldistance(prev, ycw.at);
        vector<int> next;
        forCellIdCM(c, i, ycw.at) if(celldistance(prev, c) > d) next.push_back(i);
        if(!isize(next)) {
          println(hlog, "error: no more cells for i=", i);
          ycw.spin = hrand(ycw.at->type);
          }
        else {
          ycw.spin = hrand_elt(next);
          }
        }

      else {
        stupid:
        // stupid
        ycw += rev;
        // well, make it a bit more clever on bitruncated a4 grids
        if(a4 && BITRUNCATED && S7 <= 5) {
          if(ycw.at->type == 8 && ycw.cpeek()->type != 8)
            ycw++;
          if(hrand(100) < 10) {
            if(euclid ? (turns&1) : (hrand(100) < 50))
              ycw+=2;
            else
              ycw-=2;
            turns++;
            }
          }
        }

      if(for_yendor) while(p.last_id < i && (p.path[p.last_id]->land == laMirror || inmirror(p.path[p.last_id]))) {
        p.last_id++;
        setdist(p.path[p.last_id], 7, nullptr);
        }

      if(for_yendor && inmirror(ycw.at)) ycw = mirror::reflect(ycw);
      ycw += wstep;
      p.path[i+1] = ycw.at;
      }
    p.last = ycw + rev;
    }
  return p;
  }

  }
