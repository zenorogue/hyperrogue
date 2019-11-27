// Hyperbolic Rogue -- Euclidean geometry
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file euclid.cpp
 *  \brief Euclidean geometry, including 2D, 3D, and quotient spaces
 */

#include "hyper.h"
namespace hr {

// 3D Euclidean space

#if MAXMDIM == 4

EX namespace euclid3 {

  #if HDR
  typedef long long coord; 
  constexpr long long COORDMAX = (1<<16);
  #endif
  typedef array<coord, 3> axes;  
  typedef array<array<int, 3>, 3> intmatrix;  


  static const axes main_axes = make_array<coord>(1, COORDMAX, COORDMAX * COORDMAX );
  
  EX array<int, 3> getcoord(coord x) {
    array<int, 3> res;
    for(int k=0; k<3; k++) {
      int next = x % COORDMAX;
      if(next>COORDMAX/2) next -= COORDMAX; 
      if(next<-COORDMAX/2) next += COORDMAX; 
      res[k] = next;
      x -= next;
      x /= COORDMAX;
      }
    return res; 
    }
  
  EX coord ascoord(array<int, 3> x) {
    return x[0] * main_axes[0] + x[1] * main_axes[1] + x[2] * main_axes[2];
    }

  EX vector<coord> get_shifttable() {
    static const coord D0 = main_axes[0];
    static const coord D1 = main_axes[1];
    static const coord D2 = main_axes[2];
    vector<coord> shifttable;
    switch(geometry) {
      case gCubeTiling:
        shifttable = { +D0, +D1, +D2 };
        break;
      
      case gRhombic3:
        shifttable = { D0+D1, D0+D2, D1+D2, D1-D2, D0-D2, D0-D1 };
        break;
      
      case gBitrunc3:
        shifttable = { 2*D0, 2*D1, 2*D2, D0+D1+D2, D0+D1-D2, D0-D1-D2, D0-D1+D2 };
        break;
      
      case gEuclid:
        shifttable = { D0, D1, D1-D0, -D0, -D1, D0-D1 };
        break;

      case gEuclidSquare:
        shifttable = { D0, D1, -D0, -D1 };
        break;
      
      default:
        printf("euclid3::get_shifttable() called in geometry that is not euclid3");
        exit(1);
      }
    
    // reverse everything
    int s = isize(shifttable);
    for(int i=0; i<s; i++) shifttable.push_back(-shifttable[i]);
    return shifttable;
    }
  
  EX coord canonicalize(coord x);
  EX int twisted;
  intmatrix T0;
  EX gp::loc twisted_vec, ortho_vec;
  
  struct hrmap_euclid3 : hrmap_standard {
    vector<coord> shifttable;
    vector<transmatrix> tmatrix;
    map<coord, heptagon*> spacemap;
    map<heptagon*, coord> ispacemap;
    cell *camelot_center;

    map<gp::loc, struct cdata> eucdata;

    vector<cell*> toruscells;  
    vector<cell*>& allcells() override { 
      if(bounded) {
        if(isize(toruscells) == 0) {
          celllister cl(getOrigin()->c7, 1000, 1000000, NULL);
          toruscells = cl.lst;
          }
        return toruscells;
        }
      return hrmap::allcells();
      }

    hrmap_euclid3() {
      shifttable = get_shifttable();
      tmatrix.resize(S7);
      for(int i=0; i<S7; i++) 
        tmatrix[i] = eumove(shifttable[i]);
      camelot_center = NULL;
      build_torus3(geometry);
      }

    heptagon *getOrigin() override {
      return get_at(0);
      }

    heptagon *get_at(coord at) {
      if(spacemap.count(at)) 
        return spacemap[at];
      else {
        auto h = tailored_alloc<heptagon> (S7);
        if(!IRREGULAR) 
          h->c7 = newCell(S7, h);
        else 
          irr::link_to_base(h, ((hrmap_euclid3*)irr::base)->get_at(at));
        h->distance = 0;
        h->cdata = NULL;
        h->alt = NULL;
        auto co = getcoord(at);
        if(S7 != 14)
          h->zebraval = gmod(co[0] + co[1] * 2 + co[2] * 4, 5);
        else 
          h->zebraval = co[0] & 1;
        spacemap[at] = h;
        ispacemap[h] = at;
        return h;
        }
      }
    
    heptagon *build(heptagon *parent, int d, coord at) {
      auto h = get_at(at);
      int d1 = (d+S7/2)%S7;
      bool mirr = false;
      if(twisted) {
        transmatrix I;
        auto st = getcoord(shifttable[d1]);
        twist(ispacemap[parent] + shifttable[d], st, I, mirr);
        for(int i=0; i<S7; i++) if(getcoord(shifttable[i]) == st) d1 = i;
        }
      h->c.connect(d1, parent, d, mirr);
      return h;
      }
  
    heptagon *create_step(heptagon *parent, int d) override {
      return build(parent, d, canonicalize(ispacemap[parent] + shifttable[d]));
      }  

    transmatrix adj(heptagon *h, int i) override {
      if(!twisted) return tmatrix[i];
      transmatrix res = tmatrix[i];
      coord id = ispacemap[h];
      id += shifttable[i];
      auto dummy = getcoord(0);
      bool dm = false;
      twist(id, dummy, res, dm);
      
      return res;
      }

    void draw() {
      dq::visited_by_matrix.clear();
      dq::enqueue_by_matrix(centerover->master, cview() * master_relative(centerover, true));
      
      while(!dq::drawqueue.empty()) {
        auto& p = dq::drawqueue.front();
        heptagon *h = get<0>(p);
        transmatrix V = get<1>(p);      
        dynamicval<ld> b(band_shift, get<2>(p));
        bandfixer bf(V);
        dq::drawqueue.pop();
        
        cell *c = h->c7;

        bool draw = drawcell_subs(c, V * spin(master_to_c7_angle()));
        if(wallopt && isWall3(c) && isize(dq::drawqueue) > 1000) continue;
  
        if(draw) for(int i=0; i<S7; i++)
          dq::enqueue_by_matrix(h->move(i), V * adj(h, i));
        }
      }
    
    transmatrix warppush(coord dif) {
      auto v = getcoord(dif);
      for(int i: {0, 1})
        if(T0[i][i])
          v[i] = gmod(v[i] + T0[i][i] / 2, T0[i][i]) - T0[i][i] / 2;
      return eupush3(v[0], v[1], v[2]);
      }
    
    transmatrix relative_matrix(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
      if(twisted) {
        if(h1 == h2) return Id;
        for(int s=0; s<S7; s++) if(h2 == h1->move(s)) return adj(h1, s);
        coord c1 = ispacemap[h1];
        coord c2 = ispacemap[h2];
        transmatrix T = warppush(c2 - c1);
        for(int d: {-1, 1}) {
          transmatrix I = Id;
          coord cs = c1;
          for(int s=0; s<3; s++) {
            cs += d * T0[2][2] * main_axes[2];
            I = I * eupush3(0, 0, d * T0[2][2]);
            auto dummy = getcoord(0);
            bool dm = false;
            cs = twist(cs, dummy, I, dm);
            transmatrix T1 = I * warppush(c2 - cs);
            if(hdist0(tC0(T1)) < hdist0(tC0(T)))
              T = T1;
            }
          }
        return T;
        }
      auto d = ispacemap[h2] - ispacemap[h1];
      d = canonicalize(d);
      return eumove(d);
      }
    
    vector<hyperpoint> get_vertices(cell* c) override {
      vector<hyperpoint> res;
      if(S7 < 14)
        for(ld a: {-.5,.5}) for(ld b: {-.5,.5}) for(ld c: {-.5, .5}) res.push_back(hpxy3(a,b,c));
      if(S7 == 12) {
        res.push_back(hpxy3(1,0,0));
        res.push_back(hpxy3(-1,0,0));
        res.push_back(hpxy3(0,1,0));
        res.push_back(hpxy3(0,-1,0));
        res.push_back(hpxy3(0,0,1));
        res.push_back(hpxy3(0,0,-1));
        }
      if(S7 == 14) {
        for(ld a: {-1.,-.5,0.,.5,1.})
        for(ld b: {-1.,-.5,0.,.5,1.})
        for(ld c: {-1.,-.5,0.,.5,1.})
          if(a == 0 || b == 0 || c == 0)
          if(a == .5 || a == -.5 || b == .5 || b == -.5 || c == .5 || c == -.5)
          if(a == 1 || a == -1 || b == 1 || b == -1 || c == 1 || c == -1)
            res.push_back(hpxy3(a,b,c));
        }
      return res;
      }
    };
  
  hrmap_euclid3* cubemap() {
    return ((hrmap_euclid3*) currentmap);
    }

  hrmap_euclid3* eucmap() { return cubemap(); }

  EX vector<coord>& get_current_shifttable() { return cubemap()->shifttable; }
  EX map<coord, heptagon*>& get_spacemap() { return cubemap()->spacemap; }
  EX map<heptagon*, coord>& get_ispacemap() { return cubemap()->ispacemap; }
  EX cell *& get_camelot_center() { return cubemap()->camelot_center; }

  EX hrmap* new_map() {
    return new hrmap_euclid3;
    }

  EX transmatrix move_matrix(heptagon *h, int i) { 
    return cubemap()->adj(h, i);
    }
  
  EX bool pseudohept(cell *c) {
    coord co = cubemap()->ispacemap[c->master];
    auto v = getcoord(co);
    if(S7 == 12) {
      for(int i=0; i<3; i++) if((v[i] & 1)) return false;
      }
    else {
      for(int i=0; i<3; i++) if(!(v[i] & 1)) return false;
      }
    return true;
    }

  EX int dist_alt(cell *c) {
    if(specialland == laCamelot) return dist_relative(c) + roundTableRadius(c);
    coord co = cubemap()->ispacemap[c->master];
    auto v = getcoord(co);
    if(S7 == 6) return v[2];
    else if(S7 == 12) return (v[0] + v[1] + v[2]) / 2;
    else return v[2]/2;
    }

  EX bool get_emerald(cell *c) {
    auto v = getcoord(cubemap()->ispacemap[c->master]);
    int s0 = 0, s1 = 0;
    for(int i=0; i<3; i++) {
      v[i] = gmod(v[i], 6);
      int d = min(v[i], 6-v[i]);;
      s0 += min(v[i], 6-v[i]);
      s1 += 3-d;
      }
    if(s0 == s1) println(hlog, "equality");
    return s0 > s1;
    }
  
  bool cellvalid(coord co) {
    auto v = getcoord(co);
    if(S7 == 6) return true;
    if(S7 == 12) return (v[0] + v[1] + v[2]) % 2 == 0;
    if(S7 == 14) return v[0] % 2 == v[1] % 2 && v[0] % 2 == v[2] % 2;
    return false;
    }

  EX int celldistance(coord co) {
    auto v = getcoord(co);
    if(S7 == 6) 
      return abs(v[0]) + abs(v[1]) + abs(v[2]);
    else {
      for(int i=0; i<3; i++) v[i] = abs(v[i]);
      sort(v.begin(), v.end());
      int dist = 0;
      if(S7 == 12) {
        int d = v[1] - v[0]; v[1] -= d; v[2] -= d;
        dist += d;
        int m = min((v[2] - v[0]), v[0]);
        dist += 2 * m;
        v[0] -= m; v[1] -= m; v[2] -= m * 2;
        if(v[0])
          dist += (v[0] + v[1] + v[2]) / 2;
        else
          dist += v[2];
        }
      else {
        dist = v[0] + (v[1] - v[0]) / 2 + (v[2] - v[0]) / 2;
        }
      return dist;
      }
    }

  EX int celldistance(cell *c1, cell *c2) {
    auto cm = cubemap();
    return celldistance(canonicalize(cm->ispacemap[c1->master] - cm->ispacemap[c2->master]));
    }

  EX void set_land(cell *c) {
    setland(c, specialland); 
    auto m = cubemap();
    auto co = getcoord(m->ispacemap[c->master]);
    
    int dv = 1;
    if(geometry != gCubeTiling) dv = 2;
  
    int hash = 0;
    for(int a=0; a<3; a++) hash = 1317 * hash + co[a] / 4;
  
    set_euland3(c, co[0]*120, co[1]*120, (co[1]+co[2]) / dv, hash);
    }
  
  EX int dist_relative(cell *c) {
    auto m = cubemap();
    auto& cc = m->camelot_center;
    int r = roundTableRadius(NULL);
    cell *start = m->gamestart();
    if(!cc) {
      cc = start;
      while(euclid3::celldistance(cc, start) < r + 5)
        cc = cc->cmove(hrand(cc->type));
      }
    
    return euclid3::celldistance(cc, c) - r;
    }
  
  /* quotient spaces */

  intmatrix make_intmatrix(axes a) {
    intmatrix T;
    T[0] = getcoord(a[0]);
    T[1] = getcoord(a[1]);
    T[2] = getcoord(a[2]);
    return T;
    }
  
  int determinant(const intmatrix T) {
    int det = 0;
    for(int i=0; i<3; i++) 
      det += T[0][i] * T[1][(i+1)%3] * T[2][(i+2)%3];
    for(int i=0; i<3; i++) 
      det -= T[0][i] * T[1][(i+2)%3] * T[2][(i+1)%3];
    return det;
    }
  
  intmatrix scaled_inverse(const intmatrix T) {
    intmatrix T2;
    for(int i=0; i<3; i++) 
    for(int j=0; j<3; j++) 
      T2[j][i] = (T[(i+1)%3][(j+1)%3] * T[(i+2)%3][(j+2)%3] - T[(i+1)%3][(j+2)%3] * T[(i+2)%3][(j+1)%3]);
    return T2;
    }
  
  axes user_axes;
  axes optimal_axes;
  axes regular_axes;
  
  intmatrix T, T2, T_edit;
  EX int det;
  int infinite_dims;
  int twisted0, twisted_edit;
  
  EX void set_torus3(int x, int y, int z) {
    for(int i=0; i<3; i++) for(int j=0; j<3; j++) T0[i][j] = 0;
    tie(T0[0][0], T0[1][1], T0[2][2]) = make_tuple(x, y, z);    
    twisted = 0;
    }

  EX void clear_torus3() {
    set_torus3(0, 0, 0);
    }  
  
  unordered_map<coord, int> canonical_hash;
  vector<coord> canonical_seq;
  int canonical_index;
  
  coord compute_cat(coord co) {
    auto coo = getcoord(co);
    coord cat = 0;
    for(int i=0; i<3; i++) {
      int val = T2[0][i] * coo[0] + T2[1][i] * coo[1] + T2[2][i] * coo[2];
      if(i < WDIM - infinite_dims) val = gmod(val, det);
      cat += val * main_axes[i];
      }
    return cat;
    };
  
  
  void add_canonical(coord val) {
    auto cat = compute_cat(val);
    if(canonical_hash.count(cat)) return;
    canonical_hash[cat] = isize(canonical_seq);
    canonical_seq.push_back(val);
    }
  
  EX void build_torus3(eGeometry g) {
  
    int dim = ginf[g].g.gameplay_dimension;
  
    for(int i=0; i<dim; i++) {
      user_axes[i] = 0;
      for(int j=0; j<dim; j++) user_axes[i] += main_axes[j] * T0[i][j];
      }
    if(dim == 2) user_axes[2] = 0;
  
    optimal_axes = user_axes;
    
    again:
    for(int i=0; i<dim; i++) if(optimal_axes[i] < 0) optimal_axes[i] = -optimal_axes[i];
    if(optimal_axes[0] < optimal_axes[1]) swap(optimal_axes[0], optimal_axes[1]);
    if(optimal_axes[1] < optimal_axes[dim]) swap(optimal_axes[1], optimal_axes[dim]);
    if(optimal_axes[0] < optimal_axes[1]) swap(optimal_axes[0], optimal_axes[1]);
    for(int i=0; i<3; i++) {
      int i1 = (i+1) % 3;
      int i2 = (i+2) % 3;
      for(int a=-10; a<=10; a++)
      for(int b=-10; b<=10; b++) {
        coord cand = optimal_axes[i] + optimal_axes[i1] * a + optimal_axes[i2] * b;
        if(celldistance(cand) < celldistance(optimal_axes[i])) {
          optimal_axes[i] = cand;
          goto again; 
          }
        }
      }
  
    regular_axes = optimal_axes;
    infinite_dims = dim;
    for(int i=0; i<dim; i++) if(optimal_axes[i]) infinite_dims--;
    
    int attempt = 0;
    next_attempt:
    for(int i=dim-infinite_dims; i<3; i++)
      regular_axes[i] = main_axes[(attempt+i)%3];

    T = make_intmatrix(regular_axes);
    det = determinant(T);
    if(det == 0) { 
      attempt++;
      if(attempt == 3) {
        println(hlog, "weird singular!\n");
        exit(1);
        }
      goto next_attempt;
      }
      
    if(det < 0) det = -det;
    
    T2 = scaled_inverse(T);
    canonical_hash.clear();
    canonical_seq.clear();  
    canonical_index = 0;  
    add_canonical(0);
    
    twisted = twisted0;
    if(dim == 3) {
      twisted &= 7;
      if(g != gCubeTiling && ((T0[0][0]+T0[2][2]) & 1)) twisted &=~ 1;
      if(g != gCubeTiling && ((T0[1][1]+T0[2][2]) & 1)) twisted &=~ 2;
      for(int i=0; i<3; i++) for(int j=0; j<3; j++)
        if(i != j && T0[i][j]) twisted = 0;
      if(T0[2][2] == 0) twisted = 0;
      if(T0[0][0] != T0[1][1]) twisted &= 3;
     }
    else {
      twisted &= 8;
      twisted_vec = as_gp(T0[1]);
      ortho_vec = as_gp(T0[0]);
      if(twisted_vec == gp::loc{0,0}) twisted = 0;
      if(chiral(twisted_vec)) twisted = 0;
      if(dscalar(twisted_vec, ortho_vec))
        twisted = 0;
      }
    
    set_flag(ginf[g].flags, qANYQ, infinite_dims < dim);
    set_flag(ginf[g].flags, qBOUNDED, infinite_dims == 0);
    bool nonori = false;
    if(twisted&1) nonori = !nonori;
    if(twisted&2) nonori = !nonori;
    if(twisted&4) nonori = !nonori;
    set_flag(ginf[g].flags, qNONORIENTABLE, nonori);      
    }

  void build_torus3() {
    for(eGeometry g: { gEuclid, gEuclidSquare, gCubeTiling, gRhombic3, gBitrunc3}) 
      build_torus3(g);
    }
  
  void swap01(transmatrix& M) {
    for(int i=0; i<4; i++) swap(M[i][0], M[i][1]);
    }

  EX coord twist(coord x, array<int, 3>& d, transmatrix& M, bool& mirr) {
    if(WDIM == 3) {
      auto coo = getcoord(x);
      while(coo[2] >= T0[2][2]) {
        coo[2] -= T0[2][2];
        if(twisted & 1) coo[0] *= -1, d[0] *= -1, M = M * MirrorX;
        if(twisted & 2) coo[1] *= -1, d[1] *= -1, M = M * MirrorY;
        if(twisted & 4) swap(coo[0], coo[1]), swap01(M), swap(d[0], d[1]);
        }
      while(coo[2] < 0) {
        coo[2] += T0[2][2];
        if(twisted & 4) swap(coo[0], coo[1]), swap(d[0], d[1]), swap01(M);
        if(twisted & 1) coo[0] *= -1, d[0] *= -1, M = M * MirrorX;
        if(twisted & 2) coo[1] *= -1, d[1] *= -1, M = M * MirrorY;
        }
      for(int i: {0,1})
        if(T0[i][i]) coo[i] = gmod(coo[i], T0[i][i]);
      return coo[0] * main_axes[0] + coo[1] * main_axes[1] + coo[2] * main_axes[2];
      }
    else {
      auto crd = getcoord(x);
      gp::loc coo = gp::loc(crd[0], crd[1]);
      gp::loc ort = (S3 == 3 ? gp::loc(1, -2) : gp::loc(0, 1)) * twisted_vec;
      int dsc = dscalar(twisted_vec, twisted_vec);
      gp::loc d0 (d[0], d[1]);
      hyperpoint h = eumove(as_coord(twisted_vec)) * C0;
      while(true) {
        int dsx = dscalar(coo, twisted_vec);
        if(dsx >= dsc) coo = coo - twisted_vec;
        else if (dsx < 0) coo = coo + twisted_vec;
        else break;
        M = M * spintox(h) * MirrorY * rspintox(h);
        auto s = ort * dscalar(d0, ort) * 2;
        auto v = dscalar(ort, ort);
        s.first /= v;
        s.second /= v;
        d0 = d0 - s;
        s = ort * dscalar(coo, ort) * 2;
        s.first /= v;
        s.second /= v;
        coo = coo - s;
        mirr = !mirr;
        }
      if(ortho_vec != gp::loc{0,0}) {
        int osc = dscalar(ortho_vec, ortho_vec);
        while(true) {
          int dsx = dscalar(coo, ortho_vec);
          if(dsx >= osc) coo = coo - ortho_vec;
          else if(dsx < 0) coo = coo + ortho_vec;
          else break;
          }
        }
      d[0] = d0.first; d[1] = d0.second;
      return coo.first * main_axes[0] + coo.second * main_axes[1];
      }
    }

  coord canonicalize(coord x) {
    if(twisted) {
      transmatrix M = Id;
      auto dummy = getcoord(0);
      bool dm = false;
      return twist(x, dummy, M, dm);
      }
    if(infinite_dims == WDIM) return x;
    if(infinite_dims == WDIM-1) {
      while(celldistance(x + optimal_axes[0]) <= celldistance(x)) x += optimal_axes[0];
      while(celldistance(x - optimal_axes[0]) <  celldistance(x)) x -= optimal_axes[0];
      return x;
      }
    auto cat = compute_cat(x);
    auto& st = cubemap()->shifttable;
    while(!canonical_hash.count(cat)) {
      if(canonical_index == isize(canonical_seq)) throw hr_exception();
      auto v = canonical_seq[canonical_index++];
      for(auto s: st) add_canonical(v + s);
      }
    return canonical_seq[canonical_hash[cat]];
    }

  EX void prepare_torus3() {
    T_edit = T0;
    twisted_edit = twisted0;
    }
  
  EX void show_fundamental() {
    initquickqueue();
    transmatrix M = ggmatrix(cwt.at);
    hyperpoint h0 = M*C0;
    hyperpoint ha = M*(eumove(ascoord(T_edit[0])) * C0 - C0) / 2;
    hyperpoint hb = M*(eumove(ascoord(T_edit[1])) * C0 - C0) / 2;
    if(WDIM == 3) {
      hyperpoint hc = M*(eumove(ascoord(T_edit[2])) * C0 - C0) / 2;
      for(int d:{-1,1}) for(int e:{-1,1}) {
        queueline(h0+d*ha+e*hb-hc, h0+d*ha+e*hb+hc, 0xFFFFFFFF);
        queueline(h0+d*hb+e*hc-ha, h0+d*hb+e*hc+ha, 0xFFFFFFFF);
        queueline(h0+d*hc+e*ha-hb, h0+d*hc+e*ha+hb, 0xFFFFFFFF);
        }
      }
    else {
      queueline(h0+ha+hb, h0+ha-hb, 0xFFFFFFFF);
      queueline(h0-ha+hb, h0-ha-hb, 0xFFFFFFFF);
      queueline(h0+ha+hb, h0-ha+hb, 0xFFFFFFFF);
      queueline(h0+ha-hb, h0-ha-hb, 0xFFFFFFFF);
      }
    quickqueue();
    }

  EX void show_torus3() {
    int dim = WDIM;
    cmode = sm::SIDE | sm::MAYDARK | sm::TORUSCONFIG;
    gamescreen(1);  
    dialog::init(XLAT("Euclidean quotient spaces"));
    
    for(int y=0; y<dim+1; y++)
      dialog::addBreak(100);
    
    dialog::addInfo(XLAT("columns specify periods"));
    dialog::addInfo(XLAT("(vectors you need to take to get back to start)"));
    
    dialog::addBreak(50);

    show_fundamental();
    if(dim == 3) {
      bool nondiag = false;
      for(int i=0; i<dim; i++) 
        for(int j=0; j<dim; j++) 
          if(T_edit[i][j] && i != j) nondiag = true;
        
      if(nondiag) {
        dialog::addInfo(XLAT("twisting implemented only for diagonal matrices"));
        dialog::addBreak(200);
        }
      else if(T_edit[dim-1][dim-1] == 0) {
        dialog::addInfo(XLAT("nothing to twist"));
        dialog::addInfo(XLAT("change the bottom left corner"));
        dialog::addBreak(100);
        }
      else {
        auto g = geometry;
        if(g == gCubeTiling || (T_edit[0][0]+T_edit[2][2]) % 2 == 0)
          dialog::addBoolItem(XLAT("flip X coordinate"), twisted_edit & 1, 'x');
        else
          dialog::addBoolItem(XLAT("flipping X impossible"), twisted_edit & 1, 'x');
        dialog::add_action([] { twisted_edit ^= 1; });
  
        if(g == gCubeTiling || (T_edit[1][1]+T_edit[2][2]) % 2 == 0)
          dialog::addBoolItem(XLAT("flip Y coordinate"), twisted_edit & 2, 'y');
        else
          dialog::addBoolItem(XLAT("flipping Y impossible"), twisted_edit & 2, 'y');
        dialog::add_action([] { twisted_edit ^= 2; });
  
        if(T_edit[0][0] == T_edit[1][1])
          dialog::addBoolItem(XLAT("swap X and Y"), twisted_edit & 4, 'z');
        else
          dialog::addBoolItem(XLAT("swapping impossible"), twisted_edit & 4, 'z');
        dialog::add_action([] { twisted_edit ^= 4; });
        }
      }
    else {
      if(T_edit[1][0] == 0 && T_edit[1][1] == 0)
        dialog::addInfo(XLAT("change the second column for Möbius bands and Klein bottles"));
      else if(chiral(as_gp(T_edit[1])))
        dialog::addInfo(XLAT("second period is chiral -- cannot be mirrored"));
      else if(dscalar(as_gp(T_edit[1]), as_gp(T_edit[0])))
        dialog::addInfo(XLAT("periods must be orthogonal for mirroring"));
      else {
        dialog::addBoolItem(XLAT("mirror flip in the second period"), twisted_edit & 8, 'x');
        dialog::add_action([] { twisted_edit ^= 8; });
        }
      }
    
    dialog::addBreak(50);
    
    char xch = 'p';
    for(eGeometry g: {gCubeTiling, gRhombic3, gBitrunc3}) {
      if(dim == 2) g = geometry;
      dialog::addItem(XLAT(ginf[g].menu_displayed_name), xch++);
      dialog::add_action([g] {
        stop_game();
        set_geometry(g);
        T0 = T_edit;
        twisted0 = twisted_edit;
        start_game();
        });
      if(dim == 2) break;
      }

    dialog::addBreak(50);
    dialog::addBack();
    dialog::display();
    
    int i = -1;
    for(auto& v: dialog::items) if(v.type == dialog::diBreak) {
      if(i >= 0 && i < dim) {
        for(int j=0; j < dim; j++) {
          char ch = 'a' + i * 3 + j;
          if(displayfr(dialog::dcenter + dialog::dfspace * 4 * (j-(dim-1.)/2), v.position, 2, dialog::dfsize, its(T_edit[j][i]), 0xFFFFFF, 8))
            getcstat = ch;
          dialog::add_key_action(ch, [=] {
            dialog::editNumber(T_edit[j][i], -10, +10, 1, 0, "", XLAT(
              "This matrix lets you play on the quotient spaces of three-dimensional. "
              "Euclidean space. Every column specifies a translation vector which "
              "takes you back to the starting point. For example, if you put "
              "set 2, 6, 0 on the diagonal, you get back to the starting point "
              "if you move 2 steps in the X direction, 6 steps in the Y direction "
              "(the quotient space is infinite in the Z direction).\n\n"
              "You can also introduce twists for diagonal matrices: after going "
              "the given number of steps in the Z direction, the space is also "
              "mirrored or rotated. (More general 'twisted' spaces are currently "
              "not implemented.)"
              )
              );
            dialog::extra_options = show_fundamental;
            });
          }
        }
      i++;
      }
    }

  #if CAP_COMMANDLINE
  int euArgs() {
    using namespace arg;
             
    if(0) ;
    else if(argis("-t3")) {
      PHASEFROM(2);
      stop_game();
      for(int i=0; i<3; i++)
      for(int j=0; j<3; j++) {
        shift(); T0[i][j] = argi();
        }
      build_torus3();
      }
    else if(argis("-t2")) {
      PHASEFROM(2);
      stop_game();
      for(int i=0; i<2; i++)
      for(int j=0; j<2; j++) {
        shift(); T0[i][j] = argi();
        }
      shift(); twisted0 = argi();
      build_torus3();
      }
    else if(argis("-twist3")) {
      PHASEFROM(2);
      stop_game();
      for(int i=0; i<3; i++)
      for(int j=0; j<3; j++) T0[i][j] = 0;
      
      for(int i=0; i<3; i++) {
        shift(); T0[i][i] = argi();
        }
      shift(); twisted0 = argi();
      build_torus3();
      }
    else if(argis("-twisttest")) {
      start_game();
      celllister cl(cwt.at, 10000, 10000, NULL);
      for(cell *c: cl.lst) {
        heptagon *h = c->master;
        for(int i=0; i<S7; i++)
        for(int j=0; j<S7; j++)
        for(int k=0; k<S7; k++)
        for(int l=0; l<S7; l++)
          if(h->move(i) && c->move(k) && h->move(i)->move(j) == h->move(k)->move(l) && h->move(i)->move(j)) {
            transmatrix T1 = move_matrix(h, i) * move_matrix(h->move(i), j);
            transmatrix T2 = move_matrix(h, k) * move_matrix(h->move(k), l);
            if(!eqmatrix(T1, T2)) {
              println(hlog, c, " @ ", getcoord(cubemap()->ispacemap[c->master]), " : ", i, "/", j, "/", k, "/", l, " :: ", T1, " vs ", T2);
              exit(1);
              }
            }
        }
      }
  
    else return 1;
    return 0;
    }
  
  auto euhook = addHook(hooks_args, 100, euArgs);
  #endif
  EX }

#endif

EX int dscalar(gp::loc e1, gp::loc e2) {
  return 2 * (e1.first * e2.first + e1.second*e2.second) + (S3 == 3 ? e1.first*e2.second + e2.first * e1.second : 0);
  }

EX int dsquare(gp::loc e) { return dscalar(e, e)/2; }
  
EX int dcross(gp::loc e1, gp::loc e2) {
  return e1.first * e2.second - e1.second*e2.first;
  }

EX gp::loc euc2_coordinates(cell *c) { 
  auto vec = euclid3::eucmap()->ispacemap[c->master];
  auto ans = euclid3::getcoord(vec);
  if(BITRUNCATED)
    return as_gp(ans) * gp::loc(1,1) + (c == c->master->c7 ? gp::loc(0,0) : gp::eudir((c->c.spin(0)+4)%6));
  if(GOLDBERG) {
    auto li = gp::get_local_info(c);
    gp::loc shift(0,0);
    if(li.first_dir >= 0) shift = gp::eudir(li.last_dir) * li.relative;
    return as_gp(ans) * gp::param + shift;
    }
  return as_gp(ans);
  }

/** this is slow, but we use it only for small p's */
EX cell* at_euc2_coordinates(gp::loc p) {
  cellwalker cw(currentmap->gamestart());
  while(p.first--) cw += revstep;
  cw ++;
  while(p.second--) cw += revstep;
  return cw.at;
  }
 
EX euclid3::coord as_coord(gp::loc p) { return p.first + p.second * euclid3::COORDMAX; }

EX gp::loc sdxy() { return {0, 0}; }

EX pair<bool, string> coord_display(const transmatrix& V, cell *c) {
  if(c != c->master->c7) return {false, ""};
  hyperpoint hx = eumove(1) * C0;
  hyperpoint hy = eumove(euclid3::COORDMAX) * C0;
  hyperpoint hz = WDIM == 2 ? C0 : eumove(euclid3::main_axes[2]) * C0;
  hyperpoint h = kz(inverse(build_matrix(hx, hy, hz, C03)) * inverse(ggmatrix(cwt.at->master->c7)) * V * C0);

  if(WDIM == 3)  
    return {true, fts(h[0]) + "," + fts(h[1]) + "," + fts(h[2]) };
  else
    return {true, fts(h[0]) + "," + fts(h[1]) };
  }

EX gp::loc as_gp(const array<int, 3>& v) { return gp::loc(v[0], v[1]); }

EX map<gp::loc, cdata>& get_cdata() { return euclid3::eucmap()->eucdata; }
  
EX transmatrix eumove(euclid3::coord vec) {
  constexpr double q3 = sqrt(double(3));
  auto co = euclid3::getcoord(vec);
  if(WDIM == 3) {
    return eupush3(co[0], co[1], co[2]);
    }
  transmatrix Mat = Id;
  if(a4) {
    Mat[0][LDIM] += co[0] * cgi.tessf;
    Mat[1][LDIM] += co[1] * cgi.tessf;
    }
  else {
    Mat[0][LDIM] += (co[0] + co[1] * .5) * cgi.tessf;
    Mat[1][LDIM] += co[1] * q3 /2 * cgi.tessf;
    }
  return Mat;
  }

EX bool chiral(gp::loc g) {
  int x = g.first;
  int y = g.second;
  if(x == 0) return false;
  if(y == 0) return false;
  if(x+y == 0) return false;
  if(x==y) return false;
  if(S3 == 3 && y == -2*x) return false;
  if(S3 == 3 && x == -2*y) return false;
  return true;
  }

EX euclid3::coord first_period() { return 0; }

EX void twist_once(gp::loc coo) {
  coo = coo - euclid3::twisted_vec * gp::univ_param();
  if(euclid3::twisted&8) {
    gp::loc ort = (S3 == 3 ? gp::loc(1, -2) : gp::loc(0, 1)) * euclid3::twisted_vec * gp::univ_param();
    auto s = ort * dscalar(coo, ort) * 2;
    auto v = dscalar(ort, ort);
    s.first /= v;
    s.second /= v;
    coo = coo - s;
    }
  }

EX int eudist(int sx, int sy, bool reduce IS(true)) {
  int z0 = abs(sx);
  int z1 = abs(sy);
  if(a4 && BITRUNCATED)
    return (z0 == z1 && z0 > 0 && reduce) ? z0+1: max(z0, z1);
  if(a4) return z0 + z1;
  int z2 = abs(sx+sy);
  return max(max(z0,z1), z2);
  }

EX int eudist(gp::loc a, gp::loc b) {
  return eudist(a.first-b.first, a.second-b.second, (a.first ^ a.second)&1);
  }

EX int cyldist(gp::loc a, gp::loc b) {
  a = as_gp(euclid3::getcoord(euclid3::canonicalize(as_coord(a))));
  b = as_gp(euclid3::getcoord(euclid3::canonicalize(as_coord(b))));
  
  if(!quotient) return eudist(a, b);
  
  int best = 0;
  for(int sa=0; sa<16; sa++) {
    auto _a = a, _b = b;
    if(sa&1) twist_once(_a);
    if(sa&2) twist_once(_b);
    if(sa&4) _a = _a + euclid3::ortho_vec * gp::univ_param();
    if(sa&8) _b = _b + euclid3::ortho_vec * gp::univ_param();
    int val = eudist(_a, _b);
    if(sa == 0 || val < best) best = val;
    }
  
  return best;
  }

}
