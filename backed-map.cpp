// Hyperbolic Rogue -- Backed Maps
// Copyright (C) 2026- Zeno Rogue, see 'hyper.cpp' for details

// utilities for maps which are "backed" by hyperbolic map

#include "hyper.h"

namespace hr {

#if HDR
struct backed_map {
  map<heptagon*, vector<pair<heptagon*, transmatrix> > > what_at;
  map<heptagon*, pair<heptagon*, transmatrix>> where;
  hrmap *current_altmap;

  void initialize(heptagon *origin);
  void assign(heptagon *actual, heptagon *backer, transmatrix T);
  void reassign(heptagon *actual, heptagon *backer, transmatrix T);
  void rebase(heptagon*& backer, transmatrix& T);
  void handle_precision_errors(heptagon *actual);

  geometry_information *alt_cgip[2];

  geometry_information *find_alt_cgip();

  backed_map() { current_altmap = nullptr; }

  void store(gamedata *gd);
  void clear();
  void swapdim();
  };
#endif

/** @brief like adj, but in pure 
 *  not used by arcm itself, but used in fake arcm
 */
geometry_information *backed_map::find_alt_cgip() {
  auto& galt_cgip = alt_cgip[embedded_plane];
  if(galt_cgip) return galt_cgip;
  check_cgi();
  cgi.require_basics();
  return galt_cgip = cgip;
  }

void backed_map::initialize(heptagon *origin) {

  heptagon *alt = nullptr;
  transmatrix T = Id;

  if(mhyperbolic && WDIM == 2) {
    bool f = geom3::flipped;
    if(f) geom3::light_flip(false);
    dynamicval<eGeometry> g(geometry, gNormal);
    dynamicval<eVariation> gv(variation, eVariation::pure);
    dynamicval<geometry_information*> gi(cgip, find_alt_cgip());
    alt = init_heptagon(S7);
    alt->s = hsOrigin;
    alt->alt = alt;
    current_altmap = newAltMap(alt);
    if(f) geom3::light_flip(true);
    }

  bool f = geom3::flipped;
  if(f) geom3::light_flip(false);
  transmatrix T = lxpush(.01241) * spin(1.4117) * lxpush(0.1241) * Id;
  if(f) geom3::light_flip(true);
  assign(origin, alt, T);
  }

void backed_map::assign(heptagon *actual, heptagon *backer, transmatrix T) {
  where[actual] = make_pair(backer, T);
  what_at[backer].emplace_back(actual, T);
  }

void backed_map::reassign(heptagon *actual, heptagon *backer, transmatrix T) {
  auto& p = where[actual];
  auto& old = what_at[p.first];
  for(auto& v: old) if(v.first == actual) { swap(v, old.back()); old.pop_back(); break; }
  p = make_pair(backer, T);
  what_at[backer].emplace_back(actual, T);
  }

void backed_map::rebase(heptagon*& backer, transmatrix& T) {
  if(mhyperbolic && WDIM == 2) {
    dynamicval<int> uc(cgip->use_count, cgip->use_count+1);
    dynamicval<eGeometry> g(geometry, gNormal);
    dynamicval<eVariation> gv(variation, eVariation::pure);
    dynamicval<geometry_information*> gi(cgip, find_alt_cgip());
    dynamicval<hrmap*> cm(currentmap, current_altmap);
    current_altmap->virtualRebase(backer, T);
    }
  #if CAP_BT
  if(mhyperbolic && WDIM == 3) {
    dynamicval<eGeometry> g(geometry, gBinary3);
    dynamicval<hrmap*> cm(currentmap, current_altmap);
    current_altmap->virtualRebase(backer, T);
    }
  #endif
  if(euclid) {
    /* hash the rough coordinates as heptagon* alt */
    size_t s = size_t(T[0][LDIM]+.261) * 124101 + size_t(T[1][LDIM]+.261) * 82143;
    backer = (heptagon*) s;
    }
  }

bool in_hpe = false;

void backed_map::handle_precision_errors(heptagon *h) {
  if(!hyperbolic) return;
  if(in_hpe) return;
  in_hpe = true;
  if(worst_precision_error > 1e-7) {
    println(hlog, "worst_precision_error = ", worst_precision_error, ", fixing the local structure");
    worst_precision_error = 0;
    set<heptagon*> visited;
    vector<heptagon*> q;
    auto enqueue = [&] (heptagon *h1) {
      if(visited.count(h1)) return false;
      visited.insert(h1);
      q.push_back(h1);
      return true;
      };
    enqueue(h);
    ld hpe_precision = 0;
    for(int i=0; i<250; i++) {
      h = q[i];
      for(int d=0; d<h->type; d++) {
        auto h2 = h->move(d);
        if(!h2) continue;
        bool first = enqueue(h2);
        if(true) {
          auto p1 = where[h];
          fixmatrix(p1.second);
          heptspin hi(h, d);
          transmatrix T = p1.second * currentmap->adj(h, d);
          auto p2 = p1;
          p2.second = T;

          rebase(p2.first, p2.second);

          if(first) reassign(h2, p2.first, p2.second);
          else hpe_precision = max(hpe_precision, hdist(where[h2].second*C0, p2.second*C0));
          }
        }
      }
    worst_precision_error = 0; println(hlog, "hpe_precision = ", hpe_precision);
    }
  in_hpe = false;
  }

void backed_map::clear() {
  what_at.clear();
  where.clear();
  if(current_altmap) {
    dynamicval<eGeometry> g(geometry, gNormal);       
    dynamicval<eVariation> gv(variation, eVariation::pure);
    dynamicval<geometry_information*> gi(cgip, find_alt_cgip());
    delete current_altmap;
    current_altmap = NULL;
    }
  }

void backed_map::store(gamedata *gd) {
  gd->store(what_at); gd->store(where); gd->store(current_altmap);
  }

void backed_map::swapdim() {
  dynamicval<eGeometry> g(geometry, gNormal);
  dynamicval<eVariation> gv(variation, eVariation::pure);

  alt_cgip[0] = nullptr;
  alt_cgip[1] = nullptr;

  dynamicval<geometry_information*> gi(cgip, find_alt_cgip());

  for(auto& p: what_at) for(auto& pp: p.second) swapmatrix(pp.second);
  for(auto& p: where) swapmatrix(p.second.second);

  alt_cgip[0] = nullptr;
  alt_cgip[1] = nullptr;
  }

bool show_map_stress;

void draw_stress_map() {
  if(!show_map_stress) return;
  auto bm = currentmap->get_backmap();
  if(!bm) return;
  int prec = grid_prec();
  vid.linewidth *= 4;
  for(auto& p: gmatrix) {
    cell *c = p.first;
    const shiftmatrix& V = p.second;
    auto h = c->master;
    auto p1 = bm->where[h];
    for(int i=0; i<c->type; i++) {
      auto c2 = c->move(i);
      if(!c2 || c2 < c || !gmatrix.count(c2)) continue;
      auto h2 = c2->master;
      auto& p2 = bm->where[h2];
      auto p3 = p1;
      for(int j=0; j<h->type; j++) if(h->move(j) == h2) { p3.second = p1.second * currentmap->adj(h, j); goto done; }
      for(int j=0; j<h->type; j++) if(h->move(j)) for(int k=0; k<h->move(j)->type; k++) if(h->move(j)->move(k) == h2) { p3.second = p1.second * currentmap->adj(h, j) * currentmap->adj(h->move(j), k); goto done; }
      continue;
      done:
      bm->rebase(p3.first, p3.second);

      ld dist = hdist(p3.second * C0, p2.second * C0); // + hdist(p3.second * lxpush0(1), p2.second * lxpush0(1));
      if(dist < 1e-20) dist = 1e-20;
      // if(c == cwt.at && c2 == cwt.at->move(0)) println(hlog, "dist' = ", dist);
      // if(c2 == cwt.at && c == cwt.at->move(0)) println(hlog, "dist'' = ", dist);
      color_t col = gradient(0x80FF80, 0xFF0000, log(1e-20), log(dist), log(1e-4));
      gridline(V, get_corner_position(c, i), get_corner_position(c, (i+1)%c->type), darkena(col, 0, 0xFF), prec);
      }
    }
  vid.linewidth /= 4;
  }

int hk = addHook(hooks_markers, 100, draw_stress_map) + addHook(hooks_configfile, 100, [] { param_b(show_map_stress, "show_map_stress"); });

}
