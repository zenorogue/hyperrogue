// Hyperbolic Rogue -- advanced geometry
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file geometry2.cpp
 *  \brief Matrices to transform between coordinates of various cells, coordinates of cell corners, etc.
 */

#include "hyper.h"
namespace hr {

transmatrix &ggmatrix(cell *c);

EX void fixelliptic(transmatrix& at) {
  if(elliptic && at[LDIM][LDIM] < 0) {
    for(int i=0; i<MDIM; i++) for(int j=0; j<MDIM; j++)
      at[i][j] = -at[i][j];
    }
  }

EX void fixelliptic(hyperpoint& h) {
  if(elliptic && h[LDIM] < 0)
    for(int i=0; i<MDIM; i++) h[i] = -h[i];
  }

EX transmatrix master_relative(cell *c, bool get_inverse IS(false)) {
  if(0) ;
  #if CAP_IRR  
  else if(IRREGULAR) {
    int id = irr::cellindex[c];
    ld alpha = 2 * M_PI / S7 * irr::periodmap[c->master].base.spin;
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
      transmatrix T = spin(master_to_c7_angle()) * cgi.gpdata->Tf[li.last_dir][li.relative.first&31][li.relative.second&31][gp::fixg6(li.total_dir)];
      if(get_inverse) T = inverse(T);
      return T;
      }
    }
  #endif
  else if(BITRUNCATED && !euclid) {
    for(int d=0; d<S7; d++) if(c->master->c7->move(d) == c)
      return (get_inverse?cgi.invhexmove:cgi.hexmove)[d];
    return Id;
    }
  else if(WDIM == 3 || euclid)
    return Id;
  else
    return pispin * Id;
  }

EX transmatrix calc_relative_matrix(cell *c2, cell *c1, int direction_hint) {
  return calc_relative_matrix(c2, c1, ddspin(c1, direction_hint) * xpush0(1e-2));
  }

EX transmatrix calc_relative_matrix(cell *c2, cell *c1, const hyperpoint& point_hint) {
  return currentmap->relative_matrix(c2, c1, point_hint);
  }

// target, source, direction from source to target

#if CAP_GP
namespace gp { extern gp::local_info draw_li; }
#endif

transmatrix hrmap_standard::adj(heptagon *h, int d) {
  transmatrix T = cgi.heptmove[d];
  if(h->c.mirror(d)) T = T * Mirror;
  int sp = h->c.spin(d);
  if(sp) T = T * spin(2*M_PI*sp/S7);
  return T;
  }

transmatrix hrmap_standard::relative_matrix(cell *c2, cell *c1, const hyperpoint& point_hint) {

  heptagon *h1 = c1->master;
  transmatrix gm = master_relative(c1, true);
  heptagon *h2 = c2->master;
  transmatrix where = master_relative(c2);

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
    if(bounded) {
      transmatrix T;
      ld bestdist = 1e9;
      for(int d=0; d<S7; d++) {
        auto hm = h1->move(d);
        if(!hm) continue;
        transmatrix S = adj(h1, d);
        if(hm == h2) {
          transmatrix T1 = gm * S * where;
          auto curdist = hdist(tC0(T1), point_hint);
          if(curdist < bestdist) T = T1, bestdist = curdist;
          }
        if(geometry != gMinimal) for(int e=0; e<S7; e++) if(hm->move(e) == h2) {
          transmatrix T1 = gm * S * adj(hm, e) * where;
          auto curdist = hdist(tC0(T1), point_hint);
          if(curdist < bestdist) T = T1, bestdist = curdist;
          }
        }
      if(bestdist < 1e8) return T;
      }
    for(int d=0; d<S7; d++) if(h1->move(d) == h2) {
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
        ld dist = crystal::space_distance(hm->c7, c2);
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

EX transmatrix &ggmatrix(cell *c) {
  transmatrix& t = gmatrix[c];
  if(t[LDIM][LDIM] == 0) {
    if(euwrap && centerover && masterless) 
      t = calc_relative_matrix(c, centerover, C0);
    else if(masterless && WDIM == 2) {
      if(!centerover) centerover = cwt.at;
      t = actual_view_transform * View * eumove(cell_to_vec(c) - cellwalker_to_vec(cellwalker(centerover)));
      }
    else 
      t = actual_view_transform * View * calc_relative_matrix(c, centerover, C0);
    }
  return t;
  }

#if HDR
struct horo_distance {
  ld a, b;
  
  void become(hyperpoint h1);
  horo_distance(hyperpoint h) { become(h); }
  horo_distance(hyperpoint h1, const transmatrix& T);
  bool operator < (const horo_distance z) const;
  friend void print(hstream& hs, horo_distance x) { print(hs, "[", x.a, ":", x.b, "]"); }
  };
#endif

void horo_distance::become(hyperpoint h1) {
  if(solnih) {
    a = abs(h1[2]);
    if(asonov::in()) h1 = asonov::straighten * h1;
    b = hypot_d(2, h1);
    }
  #if CAP_BT
  else if(binarytiling) {
    b = intval(h1, C0);
    a = abs(binary::horo_level(h1));
    }
  #endif
  else if(hybri)
    a = 0, b = hdist(h1, C0);
  else
    a = 0, b = intval(h1, C0);
  }

horo_distance::horo_distance(hyperpoint h1, const transmatrix& T) {
  #if CAP_BT
  if(binarytiling) become(inverse(T) * h1);
  else
#endif
  if(solnih || hybri || nil) become(inverse(T) * h1);
  else
    a = 0, b = intval(h1, tC0(T));
  }

bool horo_distance::operator < (const horo_distance z) const {
  #if CAP_BT
  if(binarytiling || solnih) {
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
      }
    if(!newbase) break;
    base = newbase;
    at = best_at;
    }
  }

template<class T, class U> 
void virtualRebase(cell*& base, T& at, const U& check) {

  if(prod) {
    auto w = hybrid::get_where(base);
    auto d = product_decompose(check(at)).first;
    at = mscale(at, -d);
    hybrid::in_underlying_map([&] { virtualRebase(w.first, at, check); });
    if(d > cgi.plevel / 2) { w.second++; d -= cgi.plevel; }
    if(d < -cgi.plevel / 2) { w.second--; d += cgi.plevel; }
    at = mscale(at, +d);
    base = hybrid::get_at(w.first, w.second);
    return;
    }

  virtualRebase_cell(base, at, check);
  }

EX void virtualRebase(cell*& base, transmatrix& at) {
  virtualRebase(base, at, tC0);
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
  return NONSTDVAR || archimedean || WDIM == 3 || binarytiling || penrose;
  }

ld hrmap_standard::spin_angle(cell *c, int d) {
  ld hexshift = 0;
  if(ctof(c) && (S7 % 2 == 0) && BITRUNCATED && !euclid) hexshift = cgi.hexshift + 2*M_PI/S7;
  else if(cgi.hexshift && ctof(c)) hexshift = cgi.hexshift;
  if(IRREGULAR) {
    auto id = irr::cellindex[c];
    auto& vs = irr::cells[id];
    if(d < 0 || d >= c->type) return 0;
    auto& p = vs.jpoints[vs.neid[d]];
    return -atan2(p[1], p[0]) - hexshift;
    }
  else if(masterless)
    return - d * 2 * M_PI / c->type - hexshift;
  else
    return M_PI - d * 2 * M_PI / c->type - hexshift;
  }

EX transmatrix ddspin(cell *c, int d, ld bonus IS(0)) { return currentmap->spin_to(c, d, bonus); }
EX transmatrix iddspin(cell *c, int d, ld bonus IS(0)) { return currentmap->spin_from(c, d, bonus); }
EX ld cellgfxdist(cell *c, int d) { return currentmap->spacedist(c, d); }
    
double hrmap_standard::spacedist(cell *c, int i) {
  if(NONSTDVAR) return hrmap::spacedist(c, i);
  if(!BITRUNCATED) return cgi.tessf;
  if(c->type == S6 && (i&1)) return cgi.hexhexdist;
  return cgi.crossf;
  }

transmatrix hrmap_standard::adj(cell *c, int i) {
  if(NONSTDVAR) return calc_relative_matrix(c->cmove(i), c, i);
  double d = cellgfxdist(c, i);
  transmatrix T = ddspin(c, i) * xpush(d);
  if(c->c.mirror(i)) T = T * Mirror;
  cell *c1 = c->cmove(i);
  T = T * iddspin(c1, c->c.spin(i), M_PI);
  return T;
  }

EX double randd() { return (rand() + .5) / (RAND_MAX + 1.); }

EX hyperpoint randomPointIn(int t) {
  if(NONSTDVAR || archimedean || penrose) {
    // Let these geometries be less confusing.
    // Also easier to implement ;)
    return xspinpush0(2 * M_PI * randd(), asinh(randd() / 20));
    }
  while(true) {
    hyperpoint h = xspinpush0(2*M_PI*(randd()-.5)/t, asinh(randd()));
    double d =
      PURE ? cgi.tessf : t == 6 ? cgi.hexhexdist : cgi.crossf;
    if(hdist0(h) < hdist0(xpush(-d) * h))
      return spin(2*M_PI/t * (rand() % t)) * h;
    }
  }

EX hyperpoint get_corner_position(cell *c, int cid, ld cf IS(3)) {
  #if CAP_GP
  if(GOLDBERG) return gp::get_corner_position(c, cid, cf);
  #endif
  #if CAP_IRR
  if(IRREGULAR) {
    auto& vs = irr::cells[irr::cellindex[c]];
    return mid_at_actual(vs.vertices[cid], 3/cf);
    }
  #endif
  #if CAP_BT
  if(penrose) return kite::get_corner(c, cid, cf);
  if(binarytiling) {
    if(WDIM == 3) {
      println(hlog, "get_corner_position called");
      return C0;
      }
    return mid_at_actual(binary::get_horopoint(binary::get_corner_horo_coordinates(c, cid)), 3/cf);
    }
  #endif
  #if CAP_ARCM
  if(archimedean) {
    auto &ac = arcm::current;
    if(PURE) {
      if(arcm::id_of(c->master) >= ac.N*2) return C0;
      auto& t = ac.get_triangle(c->master, cid-1);
      return xspinpush0(-t.first, t.second * 3 / cf * (ac.real_faces == 0 ? 0.999 : 1));
      }
    if(BITRUNCATED) {
      auto& t0 = ac.get_triangle(c->master, cid-1);
      auto& t1 = ac.get_triangle(c->master, cid);
      hyperpoint h0 = xspinpush0(-t0.first, t0.second * 3 / cf * (ac.real_faces == 0 ? 0.999 : 1));
      hyperpoint h1 = xspinpush0(-t1.first, t1.second * 3 / cf * (ac.real_faces == 0 ? 0.999 : 1));
      return mid3(C0, h0, h1);
      }
    if(DUAL) {
      auto& t0 = ac.get_triangle(c->master, 2*cid-1);
      return xspinpush0(-t0.first, t0.second * 3 / cf * (ac.real_faces == 0 ? 0.999 : 1));
      }
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

EX hyperpoint nearcorner(cell *c, int i) {
  if(GOLDBERG) {
    cellwalker cw(c, i);
    cw += wstep;
    transmatrix cwm = calc_relative_matrix(cw.at, c, i);
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
  if(archimedean) {
    if(PURE) { 
      auto &ac = arcm::current;
      auto& t = ac.get_triangle(c->master, i-1);
      int id = arcm::id_of(c->master);
      int id1 = ac.get_adj(ac.get_adj(c->master, i-1), -2).first;
      return xspinpush0(-t.first - M_PI / c->type, ac.inradius[id/2] + ac.inradius[id1/2] + (ac.real_faces == 0 ? 2 * M_PI / (ac.N == 2 ? 2.1 : ac.N) : 0));
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
    neis[0] = binary::get_horopoint(2*yy, -0.5);
    neis[1] = binary::get_horopoint(2*yy, +0.5);
    neis[2] = binary::get_horopoint(0, 1);
    neis[3] = binary::get_horopoint(-2*yy, c->master->zebraval ? -0.25 : +0.25);
    neis[4] = binary::get_horopoint(0, -1);
    return neis[i];
    }
  if(geometry == gTernary) {
    ld yx = log(3) / 2;
    ld yy = yx;
    hyperpoint neis[6];
    neis[0] = binary::get_horopoint(2*yy, -1);
    neis[1] = binary::get_horopoint(2*yy, +0);
    neis[2] = binary::get_horopoint(2*yy, +1);
    neis[3] = binary::get_horopoint(0, 1);
    neis[4] = binary::get_horopoint(-2*yy, c->master->zebraval / 3.);
    neis[5] = binary::get_horopoint(0, -1);
    return neis[i];
    }
  if(penrose) {
    if(approx_nearcorner)
      return kite::get_corner(c, i, 3) + kite::get_corner(c, i+1, 3) - C0;
    else
      return calc_relative_matrix(c->cmove(i), c, C0) * C0;
    }
  if(binarytiling) {
    if(WDIM == 3) {
      println(hlog, "nearcorner called");
      return Hypc;
      }
    ld yx = log(2) / 2;
    ld yy = yx;
    // ld xx = 1 / sqrt(2)/2;
    hyperpoint neis[7];
    neis[0] = binary::get_horopoint(0, 1);
    neis[1] = binary::get_horopoint(yy*2, 1);
    neis[2] = binary::get_horopoint(yy*2, 0);
    neis[3] = binary::get_horopoint(yy*2, -1);
    neis[4] = binary::get_horopoint(0, -1);
    if(c->type == 7)
      neis[5] = binary::get_horopoint(-yy*2, -.5),
      neis[6] = binary::get_horopoint(-yy*2, +.5);
    else
      neis[5] = binary::get_horopoint(-yy*2, 0);
    return neis[i];
    }
  #endif
  double d = cellgfxdist(c, i);
  return ddspin(c, i) * xpush0(d);
  }

EX hyperpoint farcorner(cell *c, int i, int which) {
  #if CAP_GP
  if(GOLDBERG) {
    cellwalker cw(c, i);
    int hint = cw.spin;
    cw += wstep;
    transmatrix cwm = calc_relative_matrix(cw.at, c, hint);
    if(elliptic && cwm[2][2] < 0) cwm = centralsym * cwm;
    // hyperpoint nfar = cwm*C0;
    auto li1 = gp::get_local_info(cw.at);
    if(which == 0)
      return cwm * get_corner_position(li1, (cw+2).spin);
    if(which == 1)
      return cwm * get_corner_position(li1, (cw-1).spin);          
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
  if(binarytiling || penrose)
    return nearcorner(c, (i+which) % c->type); // lazy
  #endif
  #if CAP_ARCM
  if(archimedean) {
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
  
  return currentmap->adj(c, i) * get_corner_position(c->move(i), (cellwalker(c, i) + wstep + (which?-1:2)).spin);
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
  if(IRREGULAR || archimedean) return midcorner(c, cid, .5);
  #endif
  return ddspin(c,cid,M_PI/S7) * xpush0(cgi.tessf/2);
  }

vector<hyperpoint> hrmap::get_vertices(cell* c) {
  vector<hyperpoint> res;
  for(int i=0; i<c->type; i++) res.push_back(get_corner_position(c, i, 3));
  return res;
  }

  }
