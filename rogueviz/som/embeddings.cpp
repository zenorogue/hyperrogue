// here the embeddings used in our experiments are implemented
// Copyright (C) 2011-2022 Tehora and Zeno Rogue, see 'hyper.cpp' for details

#include "kohonen.h"

namespace rogueviz {

namespace embeddings {

embedding_type etype = eNatural;

/** landscape embedding */

map<cell*, kohvec> landscape_at;

map<cellwalker, kohvec> delta_at;
map<cellwalker, int> delta_id;

int qdelta;

void init_landscape(int dimensions) {
  etype = eLandscape;
  landscape_at.clear();
  delta_at.clear();
  delta_id.clear();
  qdelta = 0;
  landscape_at[currentmap->gamestart()].resize(dimensions, 0);
  println(hlog, "initialized for ", currentmap->gamestart());
  }

kohvec& get_landscape_at(cell *h);

void init_landscape_det(const vector<cell*>& ac) {
  etype = eLandscape;
  landscape_at.clear();
  delta_at.clear();
  delta_id.clear();
  qdelta = 0;
  landscape_at[currentmap->gamestart()].resize(0, 0);
  for(cell *c: ac) get_landscape_at(c);
  int dimensions = isize(delta_at);
  landscape_at.clear();
  landscape_at[currentmap->gamestart()].resize(dimensions, 0);
  println(hlog, "qdelta = ", qdelta, " size of delta_at = ", isize(delta_at));
  for(auto& d: delta_at) {
    d.second.resize(dimensions, 0);
    // d.second[id++] = 1;
    d.second[delta_id[d.first]] = 1;
    }
  
  println(hlog, "initialized for ", currentmap->gamestart(), ", dimensions = ", dimensions);
  }

void normalize(cellwalker& cw) {
  int d = celldist(cw.at);
  back:
  if(GDIM == 3) {
    auto& da = currentmap->dirdist(cw.at);
    for(int j=0; j<S7; j++) if(da[j] == 1) {
      cellwalker str = currentmap->strafe(cw, j);
      int d1 = celldist(str.at);
      if(d1 == d+1) continue;
      else if(d1 == d-1) { d = d1; cw = str; goto back; }
      else println(hlog, tie(d, d1));
      }
    }
  else if(S3 == OINF) return;
  else if(S3 == 4) for(int s: {1, -1}) {
    cellwalker str = (cw + s) + wstep + s;
    int d1 = celldist(str.at);
    if(d1 < d) { d = d1; cw = str; goto back; }
    }
  else {
    while(true) {
      cellwalker str = (cw + 1) + wstep + 2;
      int d1 = celldist(str.at);
      if(d1 > d) break;
      d = d1; cw = str;
      }    
    while(true) {
      cellwalker str = (cw - 2) + wstep - 1;
      int d1 = celldist(str.at);
      if(d1 > d) break;
      d = d1; cw = str;
      }    
    }
  }

ld hrandd() {
  return ((hrngen() & HRANDMAX) + .5) / HRANDMAX;
  }

ld gaussian_random() {
  ld u1 = hrandd();
  ld u2 = hrandd();
  return sqrt(-2*log(u1)) * cos(2*M_PI*u2);
  }

void apply_delta(cellwalker cw, kohvec& v) {
  normalize(cw);

  auto& da = delta_at[cw];
  if(!delta_id.count(cw)) {
    delta_id[cw] = qdelta++;
    da.resize(isize(v));
    for(int i=0; i<min(200, isize(da)); i++)
      if(i < isize(da)) da[i] = gaussian_random();
    }

  for(int i=0; i<isize(v); i++) v[i] += da[i];
  }

kohvec& get_landscape_at(cell *h) {
  if(landscape_at.count(h)) return landscape_at[h];

  int hd = celldist(h);
  // if(hd > 2) exit(1);
  for(int i=0; i<h->type; i++) {
    cell *h1 = h->cmove(i);
    auto hd1 = celldist(h1);
    if(hd1 < hd) {
      cellwalker cw(h, i);
      auto& res = landscape_at[h];
      res = get_landscape_at(h1);
      if(S3 == 3) {
        apply_delta(cw, res);
        apply_delta(cw+1, res);
        }
      else
        apply_delta(cw, res);
      break;
      }
    }
  
  return landscape_at[h];
  }

/** signposts embedding */

vector<cell*> signposts;

void mark_signposts(bool full, const vector<cell*>& ac) {
  etype = eSignpost;
  println(hlog, "marking signposts");  
  signposts.clear();
  int maxd = 0;
  if(!closed_manifold)
    for(cell *c: ac) maxd = max(celldist(c), maxd);
  for(cell *c: ac)
    if(full || c->type != 6)
    if(closed_manifold || celldist(c) == maxd)
      signposts.push_back(c);
  }

/** special signposts */

void mark_signposts_subg(int a, int b, const vector<cell*>& ac) {
  etype = eSignpost;
  println(hlog, "marking bitrunc signposts");
  signposts.clear();
  int maxd = 0;
  if(!closed_manifold)
    for(cell *c: ac) maxd = max(celldist(c), maxd);
  for(cell *c: ac) {
    auto li = gp::get_local_info(c);
    auto rel = li.relative * gp::loc(a, b);
    auto rel2 = rel * gp::param.conj();
    rel2 = rel2 / (gp::param * gp::param.conj()).first;
    if(rel2 * gp::param == rel) 
      signposts.push_back(c);
    }
  }

/** rug embedding */

map<cell*, hyperpoint> rug_coordinates;

void generate_rug(int i, bool close) {
  etype = eHypersian;
  rug::init();
  while(rug::precision_increases < i) rug::physics();
  if(close) rug::close();
  for(auto p: rug::rug_map)
    rug_coordinates[p.first] = p.second->native;
  }

/** main function */

void get_coordinates(kohvec& v, cell *c, cell *c0) {

  switch(etype) {
    case eLandscape: {      
      v = get_landscape_at(c);
      columns = isize(v);
      break;
      }
    
    case eSignpost:
      columns = isize(signposts);
      alloc(v);
      for(int i=0; i<isize(signposts); i++) 
        v[i] = celldistance(signposts[i], c);
      break;
    
    case eHypersian: {
      columns = 3;
      alloc(v);
      auto h = rug_coordinates.at(c);
      for(int i=0; i<3; i++) v[i] = h[i];
      break;
      }    
    
    case eNatural: {
      hyperpoint h = calc_relative_matrix(c, c0, C0) * C0;  

      using namespace euc;
      auto& T0 = eu_input.user_axes;
      if(sphere) {      
        columns = MDIM;
        alloc(v);
        for(int i=0; i<MDIM; i++)
          v[i] = h[i];
        }
      else if(euclid && closed_manifold && S3 == 3 && WDIM == 2 && T0[0][1] == 0 && T0[1][0] == 0 && T0[0][0] == T0[1][1]) {
        columns = 6;
        alloc(v);
        int s = T0[0][0];
        for(int i=0; i<3; i++) {
          hyperpoint h1 = spin(120*degree*i) * h;
          ld x = h1[1];
          ld alpha = 2 * M_PI * x / s / (sqrt(3) / 2);
          // println(hlog, kz(x), " -> ", kz(alpha));
          v[2*i] = cos(alpha);
          v[2*i+1] = sin(alpha);
          }
        // println(hlog, kz(h), " -> ", v);
        }
      else if(euclid && closed_manifold && WDIM == 2) {
        columns = 4;
        alloc(v);
        rug::clifford_torus ct;
        h = ct.torus_to_s4(ct.actual_to_torus(h));
        for(int i=0; i<4; i++)
          v[i] = h[i];
        }
      else if(euclid && closed_manifold && WDIM == 3) {
        columns = 6;
        alloc(v);
        using namespace euc;
        auto& T0 = eu_input.user_axes;
        for(int i=0; i<3; i++) {
          int s = T0[i][i];
          ld alpha = 2 * M_PI * h[i] / s;
          v[2*i] = cos(alpha) * s;
          v[2*i+1] = sin(alpha) * s;
          }
        }
      else if(euclid && !quotient) {
        columns = WDIM;
        alloc(v);
        for(int i=0; i<WDIM; i++)
          v[i] = h[i];
        }
      else {
        println(hlog, "error: unknown geometry to get coordinates from");
        exit(1);
        }
      break;
      }
    
    case eProjection: {
      hyperpoint h = calc_relative_matrix(c, c0, C0) * C0;  
      hyperpoint res;
      applymodel(shiftless(h), res);
      columns = WDIM;
      if(models::is_3d(pconf)) columns = 3;
      alloc(v);
      for(int i=0; i<columns; i++) v[i] = res[i];
      }
    }
  }

}
}
