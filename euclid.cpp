// Hyperbolic Rogue -- Euclidean geometry
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file euclid.cpp
 *  \brief Euclidean geometry, including 2D, 3D, and quotient spaces
 */

#include "hyper.h"
namespace hr {

EX namespace euc {

  #if HDR
  struct coord : array<int, 3> {
    coord() {}
    coord(int x, int y, int z) { self[0] = x; self[1] = y; self[2] = z; }
    coord& operator += (coord b) { for(int i: {0,1,2}) self[i] += b[i]; return self; }
    coord& operator -= (coord b) { for(int i: {0,1,2}) self[i] -= b[i]; return self; }
    coord operator + (coord b) const { coord a = self; return a += b; }
    coord operator - (coord b) const { coord a = self; return a -= b; }
    coord operator -() const { return coord(-self[0], -self[1], -self[2]); }
    coord& operator +() { return self; }
    const coord& operator +() const { return self; }
    coord operator *(int x) const { return coord(x*self[0], x*self[1], x*self[2]); }
    friend coord operator *(int x, const coord& y) { return coord(x*y[0], x*y[1], x*y[2]); }
    };
  
  typedef array<coord, 3> intmatrix;
  #endif

  EX const coord euzero = coord(0,0,0);
  EX const coord eutester = coord(3,7,0);
  EX intmatrix euzeroall = make_array<coord>(euzero, euzero, euzero);

  static const intmatrix main_axes = make_array<coord>(coord(1,0,0), coord(0,1,0), coord(0,0,1));
  
  EX vector<coord> get_shifttable() {
    static const coord D0 = main_axes[0];
    static const coord D1 = main_axes[1];
    static const coord D2 = main_axes[2];
    vector<coord> shifttable;
        
    /* for portal spaces... */
    auto g = geometry;
    if(S7 == 6 && WDIM == 3) g = gCubeTiling;

    switch(g) {
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
        printf("euc::get_shifttable() called in geometry that is not euclid3");
        exit(1);
      }
    
    // reverse everything
    int s = isize(shifttable);
    for(int i=0; i<s; i++) shifttable.push_back(-shifttable[i]);
    return shifttable;
    }
  
  EX coord basic_canonicalize(coord x);
  
  #if HDR
  struct torus_config {
    /** periods entered by the user */
    intmatrix user_axes;
    /** OR'ed flags: 1 -- flip X in 3D, 2 -- flip Y in 3D, 4 -- flip X/Y in 3D, 8 -- Klein bottle in 2D, 16 -- third turn in 3D, 32 -- Hantzsche-Wendt in 3D */
    int twisted;

    torus_config() {}
    torus_config(intmatrix user_axes, int twisted) : user_axes(user_axes), twisted(twisted) {}
    };
  
  struct torus_config_full : torus_config {
    /** optimal representation of the periods */
    intmatrix optimal_axes;
    /** regular axes (?) */
    intmatrix regular_axes;
    /** in 2D: the period vector which is reflected */
    gp::loc twisted_vec;
    /** in 2D: a vector orthogonal to twisted_vec */
    gp::loc ortho_vec;
    /** determinant */
    int det;
    /** the number of infinite dimensions */
    int infinite_dims;
    /** ? */  
    intmatrix inverse_axes;
    /** for canonicalization on tori */
    map<coord, int> hash;
    vector<coord> seq;
    int index;

    void reset() { index = 0; hash.clear(); seq.clear(); }
    
    /** add to the tori canonicalization list */
    void add(coord val);
    
    /** get the representative on the tori canonicalization list */
    coord get(coord x);

    /** find the equivalence class of coo */
    coord compute_cat(coord coo);

    /** canonicalize coord x; in case of twisting, adjust d, M, and mirr accordingly */
    void canonicalize(coord& x, coord& d, transmatrix& M, bool& mirr);
    };
  #endif
  EX torus_config eu_input, eu_edit;
  EX torus_config_full eu;

  struct hrmap_euclidean : hrmap_standard {
    vector<coord> shifttable;
    vector<transmatrix> tmatrix;
    map<coord, heptagon*> spacemap;
    map<heptagon*, coord> ispacemap;
    cell *camelot_center;

    map<gp::loc, struct cdata> eucdata;
    
    void compute_tmatrix() {
      cgi.prepare_basics();
      shifttable = get_shifttable();
      tmatrix.resize(S7);
      for(int i=0; i<S7; i++) tmatrix[i] = eumove(shifttable[i]);
      }
    
    void on_dim_change() override {
      compute_tmatrix();
      }

    vector<cell*> toruscells;  
    vector<cell*>& allcells() override { 
      if(closed_manifold && !disksize) {
        if(isize(toruscells) == 0) {
          celllister cl(getOrigin()->c7, 1000, 1000000, NULL);
          toruscells = cl.lst;
          }
        return toruscells;
        }
      return hrmap::allcells();
      }

    hrmap_euclidean() {
      compute_tmatrix();
      camelot_center = NULL;
      build_torus3(geometry);    
      #if CAP_IRR
      if(!valid_irr_torus()) { 
        addMessage(XLAT("Error: period mismatch"));
        eu_input = irr::base_config; 
        build_torus3(geometry); 
        }
      #endif
      }

    heptagon *getOrigin() override {
      return get_at(euzero);
      }

    heptagon *get_at(coord at) {
      if(spacemap.count(at)) 
        return spacemap[at];
      else {
        auto h = init_heptagon(S7);
        if(!IRREGULAR) 
          h->c7 = newCell(S7, h);
        #if CAP_IRR
        else {
          coord m0 = shifttable[0];
          transmatrix dummy;
          bool mirr;
          auto ati = at;
          irr::base_config.canonicalize(ati, m0, dummy, mirr);
          indenter id(2);
          for(int spin=0; spin<S7; spin++) if(shifttable[spin] == m0) {
            irr::link_to_base(h, heptspin(((hrmap_euclidean*)irr::base)->get_at(ati), spin, mirr));
            break;
            }
          }
        #endif
        if(S7 != 14)
          h->zebraval = gmod(at[0] + at[1] * 2 + at[2] * 4, 5);
        else 
          h->zebraval = at[0] & 1;
        spacemap[at] = h;
        ispacemap[h] = at;

        return h;
        }
      }
    
    heptagon *create_step(heptagon *parent, int d) override {
      int d1 = (d+S7/2)%S7;
      bool mirr = false;
      transmatrix I;
      auto v = ispacemap[parent] + shifttable[d];
      auto st = shifttable[d1];
      eu.canonicalize(v, st, I, mirr);
      if(eu.twisted)
        for(int i=0; i<S7; i++) if(shifttable[i] == st) d1 = i;

      heptagon *h = get_at(v);
      h->c.connect(d1, parent, d, mirr);
      return h;
      }  

    transmatrix adj(heptagon *h, int i) override {
      if(!eu.twisted) return tmatrix[i];
      transmatrix res = tmatrix[i];
      coord id = ispacemap[h];
      id += shifttable[i];
      auto dummy = euzero;
      bool dm = false;
      eu.canonicalize(id, dummy, res, dm);
      
      return res;
      }

    transmatrix adj(cell *c, int i) override {
      if(dont_inverse()) return adj(c->master, i);
      if(WDIM == 3) return adj(c->master, i);
      else return hrmap_standard::adj(c, i);
      }

    void draw_at(cell *at, const shiftmatrix& where) override {
      dq::clear_all();
      dq::enqueue_by_matrix(at->master, where * master_relative(centerover, true));
      
      while(!dq::drawqueue.empty()) {
        auto& p = dq::drawqueue.front();
        heptagon *h = p.first;
        shiftmatrix V = p.second;      
        dq::drawqueue.pop();
        
        cell *c = h->c7;

        bool draw = drawcell_subs(c, V * spin(master_to_c7_angle()));
        if(in_wallopt() && isWall3(c) && isize(dq::drawqueue) > 1000 && !hybrid::pmap) continue;
  
        if(draw) for(int i=0; i<S7; i++)
          dq::enqueue_by_matrix(h->move(i), optimized_shift(V * adj(h, i)));
        }
      }
    
    transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
      if(eu.twisted) {
        if(h1 == h2) return Id;
        for(int s=0; s<S7; s++) if(h2 == h1->move(s)) return adj(h1, s);
        coord c1 = ispacemap[h1];
        coord c2 = ispacemap[h2];
        transmatrix T = eumove(c2 - c1);

        transmatrix I = Id;
        coord cs = c1;
        for(int s=0; s<4; s++) {
          for(int a=-1; a<=1; a++)
          for(int b=-1; b<=1; b++) {
            if(b && WDIM == 2) continue;
            transmatrix T1 = I * eumove((c2 - cs) + a*eu.user_axes[0] + b*eu.user_axes[1]);
            if(hdist(tC0(T1), hint) < hdist(tC0(T), hint))
              T = T1;
            }
          auto co = eu.user_axes[WDIM-1];
          cs += co;
          I = I * eumove(co);
          auto dummy = euzero;
          bool dm = false;
          eu.canonicalize(cs, dummy, I, dm);
          }
        return T;
        }
      auto d = ispacemap[h2] - ispacemap[h1];
      d = basic_canonicalize(d);
      return eumove(d);
      }
    
    subcellshape& get_cellshape(cell* c) override {
      return *cgi.heptshape;
      }
    };
  
  hrmap_euclidean* cubemap() {
    if(fake::in()) return FPIU(cubemap());
    return ((hrmap_euclidean*) currentmap);
    }

  hrmap_euclidean* eucmap() { 
    return cubemap(); 
    }

  EX vector<coord>& get_current_shifttable() { return cubemap()->shifttable; }
  EX map<coord, heptagon*>& get_spacemap() { return cubemap()->spacemap; }
  EX map<heptagon*, coord>& get_ispacemap() { return cubemap()->ispacemap; }
  EX cell *& get_camelot_center() { return cubemap()->camelot_center; }

  EX heptagon* get_at(coord co) { return cubemap()->get_at(co); }

  EX hrmap* new_map() {
    return new hrmap_euclidean;
    }

  EX transmatrix move_matrix(heptagon *h, int i) { 
    return cubemap()->adj(h, i);
    }
  
  EX bool pseudohept(cell *c) {
    if(cgflags & qPORTALSPACE) return false;
    coord co = cubemap()->ispacemap[c->master];
    if(S7 == 12) {
      for(int i=0; i<3; i++) if((co[i] & 1)) return false;
      }
    else {
      for(int i=0; i<3; i++) if(!(co[i] & 1)) return false;
      }
    return true;
    }

  EX int dist_alt(cell *c) {
    if(WDIM == 2) {
      auto v = full_coords2(c);
      return euclidAlt(v.first, v.second);
      }
    if(specialland == laCamelot) return dist_relative(c) + roundTableRadius(c);
    auto v = cubemap()->ispacemap[c->master];
    if(S7 == 6) return v[2];
    else if(S7 == 12) return (v[0] + v[1] + v[2]) / 2;
    else return v[2]/2;
    }

  EX bool get_emerald(cell *c) {
    auto v = cubemap()->ispacemap[c->master];
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
  
  bool cellvalid(coord v) {
    if(S7 == 6) return true;
    if(S7 == 12) return (v[0] + v[1] + v[2]) % 2 == 0;
    if(S7 == 14) return v[0] % 2 == v[1] % 2 && v[0] % 2 == v[2] % 2;
    return false;
    }

  EX int celldistance(coord v) {
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
    if(GDIM == 2)
      return dist(full_coords2(c1), full_coords2(c2));
    return celldistance(basic_canonicalize(cm->ispacemap[c1->master] - cm->ispacemap[c2->master]));
    }

  EX void set_land(cell *c) {
    if(cgflags & qPORTALSPACE) return;
    setland(c, specialland); 
    auto m = cubemap();
    auto co = m->ispacemap[c->master];
    
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
      while(euc::celldistance(cc, start) < r + 5)
        cc = cc->cmove(hrand(cc->type));
      }
    
    return euc::celldistance(cc, c) - r;
    }
  
  /* quotient spaces */

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
  
  EX torus_config torus3(int x, int y, int z) {
    intmatrix T0 = euzeroall;
    tie(T0[0][0], T0[1][1], T0[2][2]) = make_tuple(x, y, z);    
    return {T0, 0};
    }

  EX torus_config clear_torus3() {
    return {euzeroall, 0};
    }  
  
  coord torus_config_full::compute_cat(coord coo) {
    coord cat = euzero;
    auto& T2 = inverse_axes;
    for(int i=0; i<3; i++) {
      int val = T2[0][i] * coo[0] + T2[1][i] * coo[1] + T2[2][i] * coo[2];
      if(i < WDIM - infinite_dims) val = gmod(val, det);
      cat += val * main_axes[i];
      }
    return cat;
    }
    
  EX bool valid_third_turn(const intmatrix& m) {
    if(m[0][2] != -m[0][0]-m[0][1]) return false;
    if(m[1][0] != m[0][1]) return false;
    if(m[1][1] != m[0][2]) return false;
    if(m[1][2] != m[0][0]) return false;
    if(m[2][0] != m[2][1]) return false;
    if(m[2][0] != m[2][2]) return false;
    return true;
    }

  EX torus_config make_hantzsche_wendt(int v) {
    intmatrix im;
    for(int i=0; i<3; i++)
    for(int j=0; j<3; j++) im[i][j] = 0;

    for(int i=0; i<3; i++) {
      im[i][i] = v;
      im[i][(i+1)%3] = v;
      }
    
    return {im, 32};
    }
  
  EX bool valid_hantzsche_wendt(const intmatrix& m) {
    return m[0][0] > 0 && m == make_hantzsche_wendt(m[0][0]).user_axes;
    }

  EX torus_config make_third_turn(int a, int b, int c) {
    intmatrix T0;
    T0[0][0] = a;
    T0[0][1] = b;
    T0[2][0] = c;
    T0[0][2] = -T0[0][0]-T0[0][1];
    T0[1][0] = T0[0][1];
    T0[1][1] = T0[0][2];
    T0[1][2] = T0[0][0];
    T0[2][1] = T0[2][2] = c;
    return {T0, 8};
    }

  EX torus_config make_quarter_turn(int a, int b, int c) {
    intmatrix T0 = euzeroall;
    T0[0][0] = a;
    T0[0][1] = b;
    T0[2][0] = c;
    return {T0, 5};
    }

  void torus_config_full::add(coord val) {
    auto cat = compute_cat(val); if(hash.count(cat)) return; hash[cat] = isize(seq); seq.push_back(val);
    }

  coord torus_config_full::get(coord x) {
    auto cat = compute_cat(x);
    auto& st = cubemap()->shifttable;
    while(!hash.count(cat)) {
      if(index == isize(seq)) throw hr_exception();
      auto v = seq[index++];
      for(auto s: st) add(v + s);
      }
    return seq[hash[cat]];
    }
  
  EX bool valid_irr_torus() {
    #if CAP_IRR
    if(!IRREGULAR) return true;
    if(eu.twisted) return false;
    for(int i=0; i<2; i++) {
      auto x = eu.user_axes[i];
      coord dm = eutester;
      transmatrix dummy = Id;
      bool mirr = false;
      irr::base_config.canonicalize(x, dm, dummy, mirr);
      auto x0 = eu.user_axes[i];
      auto dm0 = eutester;
      eu.canonicalize(x0, dm0, dummy, mirr);
      if(x0 != euzero || dm0 != eutester) return false;
      }
    #endif
    return true;
    }
  
  EX void build_torus3(eGeometry g) {
  
    int dim = ginf[g].g.gameplay_dimension;
    
    eu.user_axes = eu_input.user_axes;
    if(dim == 2) eu.user_axes[2] = euzero;

    eu.optimal_axes = eu.user_axes;
    
    again:
    for(int i=0; i<dim; i++) if(eu.optimal_axes[i] < euzero) eu.optimal_axes[i] = -eu.optimal_axes[i];
    if(eu.optimal_axes[0] < eu.optimal_axes[1]) swap(eu.optimal_axes[0], eu.optimal_axes[1]);
    if(eu.optimal_axes[1] < eu.optimal_axes[dim-1]) swap(eu.optimal_axes[1], eu.optimal_axes[dim-1]);
    if(eu.optimal_axes[0] < eu.optimal_axes[1]) swap(eu.optimal_axes[0], eu.optimal_axes[1]);
    for(int i=0; i<3; i++) {
      int i1 = (i+1) % 3;
      int i2 = (i+2) % 3;
      for(int a=-10; a<=10; a++)
      for(int b=-10; b<=10; b++) {
        coord cand = eu.optimal_axes[i] + eu.optimal_axes[i1] * a + eu.optimal_axes[i2] * b;
        if(celldistance(cand) < celldistance(eu.optimal_axes[i])) {
          eu.optimal_axes[i] = cand;
          goto again; 
          }
        }
      }
  
    eu.regular_axes = eu.optimal_axes;
    eu.infinite_dims = dim;
    for(int i=0; i<dim; i++) if(eu.optimal_axes[i] != euzero) eu.infinite_dims--;
    
    int attempt = 0;
    next_attempt:
    for(int i=dim-eu.infinite_dims; i<3; i++)
      eu.regular_axes[i] = main_axes[(attempt+i)%3];

    eu.det = determinant(eu.regular_axes);
    if(eu.det == 0) { 
      attempt++;
      if(attempt == 3) {
        println(hlog, "weird singular!\n");
        exit(1);
        }
      goto next_attempt;
      }
      
    if(eu.det < 0) eu.det = -eu.det;
    
    eu.inverse_axes = scaled_inverse(eu.regular_axes);
    eu.reset();
    eu.add(euzero);
    
    eu.twisted = eu_input.twisted;
    if(dim == 3) {
      auto &T0 = eu.user_axes;
      if(valid_third_turn(eu.user_axes)) {
        eu.twisted &= 16;
        if(g == gRhombic3 && (T0[2][2]&1)) eu.twisted = 0;
        if(g == gBitrunc3 && (T0[0][0]&1)) eu.twisted = 0;
        if(g == gBitrunc3 && (T0[1][1]&1)) eu.twisted = 0;
        }
      else if(valid_hantzsche_wendt(eu.user_axes)) {
        eu.twisted &= 32;
        if(g == gBitrunc3 && (T0[0][0]&1)) eu.twisted = 0;
        }
      else {
        eu.twisted &= 7;
        if(g != gCubeTiling && ((T0[0][0]+T0[2][2]) & 1)) eu.twisted &=~ 1;
        if(g != gCubeTiling && ((T0[1][1]+T0[2][2]) & 1)) eu.twisted &=~ 2;
        for(int i=0; i<3; i++) for(int j=0; j<3; j++)
          if(i != j && T0[i][j]) eu.twisted = 0;
        if(T0[2][2] == 0) eu.twisted = 0;
        if(T0[0][0] != T0[1][1]) eu.twisted &= 3;
        }
      }
    else {
      eu.twisted &= 8;
      eu.twisted_vec = to_loc(eu.user_axes[1]);
      eu.ortho_vec = to_loc(eu.user_axes[0]);
      if(eu.twisted_vec == gp::loc{0,0}) eu.twisted = 0;
      if(chiral(eu.twisted_vec)) eu.twisted = 0;
      if(dscalar(eu.twisted_vec, eu.ortho_vec))
        eu.twisted = 0;
      }
    
    set_flag(ginf[g].flags, qANYQ, eu.infinite_dims < dim);
    set_flag(ginf[g].flags, qCLOSED, eu.infinite_dims == 0);
    set_flag(ginf[g].flags, qSMALL, eu.infinite_dims == 0 && eu.det <= 4096);
    bool nonori = false;
    if(eu.twisted&1) nonori = !nonori;
    if(eu.twisted&2) nonori = !nonori;
    if(eu.twisted&4) nonori = !nonori;
    if(eu.twisted&8) nonori = !nonori;
    set_flag(ginf[g].flags, qNONORIENTABLE, nonori);      
    }

  EX void build_torus3() {
    for(eGeometry g: { gEuclid, gEuclidSquare, gCubeTiling, gRhombic3, gBitrunc3}) 
      build_torus3(g);
    }
  
  void swap01(transmatrix& M) {
    for(int i=0; i<4; i++) swap(M[i][0], M[i][1]);
    }
  
  gp::loc ort1() { return (S3 == 3 ? gp::loc(1, -2) : gp::loc(0, 1)); }

  int diagonal_cross(const coord& a, const coord& b) {
    return a[0]*b[1] + a[1]*b[2] + a[2]*b[0]
         - b[0]*a[1] - b[1]*a[2] - b[2]*a[0];
    }
  
  void torus_config_full::canonicalize(coord& x, coord& d, transmatrix& M, bool& mirr) {
    if(!twisted) {
      if(infinite_dims == WDIM) return;
      if(infinite_dims == WDIM-1) {
        auto& o = optimal_axes;
        while(celldistance(x + o[0]) <= celldistance(x)) x += o[0];
        while(celldistance(x - o[0]) <  celldistance(x)) x -= o[0];
        return;
        }
      x = get(x);
      return;
      }
    auto& T0 = user_axes;
    if(twisted & 32) {    
      int period = T0[0][0];
      auto& coo = x;
      
      while(true) {
        restart:
        /* These coordinates cause the algorithm below to go in circles. We simply break if they are detected */
        if(coo[0] >= 0 && coo[1] == period - coo[0] && coo[2] == -coo[1] && coo[0]*2 > period && coo[0] < period) return;
        if(coo[0]*2 <= -period && coo[0] >= -period && coo[2] == period+coo[0] && coo[2] == -coo[1]) return;

        /* apply periods */
        for(int i=0; i<3; i++) {
          int j = (i+1) % 3;
          int k = (i+2) % 3;
          int v1 = coo[i] + coo[j];
          int v2 = coo[i] - coo[j];
          if(v1 >= period) {
            coo[i] -= period; coo[j] -= period;
            }
          else if(v1 < -period) {
            coo[i] += period; coo[j] += period;            
            }
          else if(v2 >= period) {
            coo[i] -= period; coo[j] += period;
            }
          else if(v2 < -period) {
            coo[i] += period; coo[j] -= period;
            }
          else continue;
          d[j] = -d[j]; d[k] = -d[k];
          coo[j] = -coo[j]; coo[k] = -coo[k];
          transmatrix S = Id;
          S[j][j] = -1; S[k][k] = -1;
          M = M * S;
          goto restart;
          }
        return;
        }
      }
    #if MAXMDIM >= 4
    if(twisted & 16) {
      int period = T0[2][2];
      transmatrix RotYZX = Zero;
      RotYZX[1][0] = 1;
      RotYZX[2][1] = 1;
      RotYZX[0][2] = 1;
      RotYZX[3][3] = 1;
      auto& coo = x;
      while(true) {
        auto coosum = coo[0] + coo[1] + coo[2];
        if(coosum >= 3 * period) {
          coo[0] -= period, coo[1] -= period, coo[2] -= period;
          tie(d[0], d[1], d[2]) = make_tuple(d[1], d[2], d[0]);
          tie(coo[0], coo[1], coo[2]) = make_tuple(coo[1], coo[2], coo[0]);
          M = M * RotYZX;
          }
        else if(coosum < 0) {
          coo[0] += period, coo[1] += period, coo[2] += period;
          tie(d[0], d[1], d[2]) = make_tuple(d[2], d[0], d[1]);
          tie(coo[0], coo[1], coo[2]) = make_tuple(coo[2], coo[0], coo[1]);
          M = M * RotYZX * RotYZX;
          }
        else break;
        }
      if(T0[0] != euzero) {
        while(diagonal_cross(coo, T0[1]) < 0) coo -= T0[0];
        while(diagonal_cross(coo, T0[1]) > 0) coo += T0[0];
        while(diagonal_cross(coo, T0[0]) > 0) coo -= T0[1];
        while(diagonal_cross(coo, T0[0]) < 0) coo += T0[1];
        }
      return;
      }
    #endif
    if(WDIM == 3) {
      #if MAXMDIM >= 4
      auto& coo = x;
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
      return;
      #endif
      }
    else {
      gp::loc coo = to_loc(x);
      gp::loc ort = ort1() * twisted_vec;
      int dsc = dscalar(twisted_vec, twisted_vec);
      gp::loc d0 (d[0], d[1]);
      hyperpoint h = eumove(to_coord(twisted_vec)) * C0;
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
      x = to_coord(coo);
      return;
      }
    }

  coord basic_canonicalize(coord x) {
    transmatrix M = Id;
    auto dummy = euzero;
    bool dm = false;
    eu.canonicalize(x, dummy, M, dm);
    return x;
    }

  EX void prepare_torus3() {
    eu_edit = eu_input;
    }
  
  EX void show_fundamental() {
    initquickqueue();
    shiftmatrix M = ggmatrix(cwt.at);
    shiftpoint h0 = M*C0;
    auto& T_edit = eu_edit.user_axes;
    hyperpoint ha = M.T*(eumove(T_edit[0]) * C0 - C0) / 2;
    hyperpoint hb = M.T*(eumove(T_edit[1]) * C0 - C0) / 2;
    if(WDIM == 3) {
      hyperpoint hc = M.T*(eumove(T_edit[2]) * C0 - C0) / 2;
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

  intmatrix on_periods(gp::loc a, gp::loc b) {
    intmatrix res;
    for(int i=0; i<3; i++) for(int j=0; j<3; j++) res[i][j] = 0;
    res[0][0] = a.first;
    res[0][1] = a.second;
    res[1][0] = b.first;
    res[1][1] = b.second;
    res[2][2] = 1;
    return res;
    }
  
  torus_config single_row_torus(int qty, int dy) { 
    return { on_periods(gp::loc{dy, -1}, gp::loc{qty, 0}), 0 };
    }
  
  torus_config regular_torus(gp::loc p) { 
    return { on_periods(p, gp::loc(0,1) * p), 0 };
    }
  
  EX torus_config rectangular_torus(int x, int y, bool klein) { 
    if(S3 == 3) y /= 2;
    return { on_periods(ort1() * gp::loc(y,0), gp::loc(x,0)), klein?8:0 };
    }
  
  void torus_config_option(string name, char key, torus_config tc) {
    dialog::addBoolItem(name, eu_edit.user_axes == tc.user_axes && eu_edit.twisted == tc.twisted && PURE, key);
    dialog::add_action([tc] {
      stop_game();
      eu_input = eu_edit = tc;
      set_variation(eVariation::pure);
      start_game();
      });
    }
  
  EX int quotient_size = 2;
  
  EX void show_torus3() {
    int dim = WDIM;
    auto& T_edit = eu_edit.user_axes;
    auto& twisted_edit = eu_edit.twisted;
    cmode = sm::SIDE | sm::MAYDARK | sm::TORUSCONFIG;
    gamescreen();
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
        
      if(valid_third_turn(T_edit)) {
        auto g = geometry;
        if(g == gCubeTiling ||
          (g == gRhombic3 && T_edit[2][2] % 2 == 0) ||
          (g == gBitrunc3 && T_edit[0][0] % 2 == 0 && T_edit[1][1] % 2 == 0))
          dialog::addBoolItem(XLAT("third-turn space"), twisted_edit & 16, 'x');
        else
          dialog::addBoolItem(XLAT("make it even"), twisted_edit & 16, 'x');
        dialog::add_action([] { eu_edit.twisted ^= 16; });
        }
      
      if(valid_hantzsche_wendt(T_edit)) {
        auto g = geometry;
        if(g == gCubeTiling || g == gRhombic3 || (g == gBitrunc3 && T_edit[0][0] % 2 == 0))
          dialog::addBoolItem(XLAT("Hantzsche-Wendt space"), twisted_edit & 32, 'x');
        else
          dialog::addBoolItem(XLAT("make it even"), twisted_edit & 32, 'x');
        dialog::add_action([] { eu_edit.twisted ^= 32; });
        }
      
      if(nondiag) {
        dialog::addInfo(XLAT("twisting implemented only for diagonal matrices"));
        dialog::addInfo(XLAT("or for columns : (A,B,C), (B,C,A), (D,D,D) where A+B+C=0"));
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
        dialog::add_action([] { eu_edit.twisted ^= 1; });
  
        if(g == gCubeTiling || (T_edit[1][1]+T_edit[2][2]) % 2 == 0)
          dialog::addBoolItem(XLAT("flip Y coordinate"), twisted_edit & 2, 'y');
        else
          dialog::addBoolItem(XLAT("flipping Y impossible"), twisted_edit & 2, 'y');
        dialog::add_action([] { eu_edit.twisted ^= 2; });
  
        if(T_edit[0][0] == T_edit[1][1])
          dialog::addBoolItem(XLAT("swap X and Y"), twisted_edit & 4, 'z');
        else
          dialog::addBoolItem(XLAT("swapping impossible"), twisted_edit & 4, 'z');
        dialog::add_action([] { eu_edit.twisted ^= 4; });
        }
      dialog::addBreak(50);
      dialog::addItem("special manifolds", 'S');
      dialog::add_action([] {
        dialog::editNumber(quotient_size, 1, 12, 1, 2, "special manifold size", "");
        dialog::extra_options = [] {
          auto q = quotient_size;
          torus_config_option(XLAT("third-turn space"), 'A', make_third_turn(q,0,q));
          torus_config_option(XLAT("quarter-turn space"), 'B', make_quarter_turn(q,0,q));
          torus_config_option(XLAT("Hantzsche-Wendt space"), 'C', make_hantzsche_wendt(q));
          };
        });
      }
    else {
      if(T_edit[1][0] == 0 && T_edit[1][1] == 0)
        dialog::addInfo(XLAT("change the second column for Möbius bands and Klein bottles"));
      else if(chiral(to_loc(T_edit[1])))
        dialog::addInfo(XLAT("second period is chiral -- cannot be mirrored"));
      else if(dscalar(to_loc(T_edit[1]), to_loc(T_edit[0])))
        dialog::addInfo(XLAT("periods must be orthogonal for mirroring"));
      else {
        dialog::addBoolItem(XLAT("mirror flip in the second period"), twisted_edit & 8, 'x');
        dialog::add_action([] { eu_edit.twisted ^= 8; });
        }
      
      dialog::addBreak(50);
      torus_config_option(XLAT("single-cell torus"), 'A', regular_torus(gp::loc{1,0}));
      torus_config_option(XLAT("large regular torus"), 'B', regular_torus(gp::loc{12, 0}));
      torus_config_option(XLAT("Klein bottle"), 'C', rectangular_torus(12, 6, true));
      torus_config_option(XLAT("cylinder"), 'D', rectangular_torus(6, 0, false));
      torus_config_option(XLAT("Möbius band"), 'E', rectangular_torus(6, 0, true));
      if(S3 == 3) torus_config_option(XLAT("seven-colorable torus"), 'F', regular_torus(gp::loc{1,2}));
      if(S3 == 3) torus_config_option(XLAT("HyperRogue classic torus"), 'G', single_row_torus(381, -22));
      torus_config_option(XLAT("no quotient"), 'H', rectangular_torus(0, 0, false));
      }
      
    dialog::addBreak(50);
    dialog::addBoolItem(XLAT("standard rotation"), eqmatrix(models::euclidean_spin, Id), 's');
    dialog::add_action([] { rotate_view(models::euclidean_spin); });

#if CAP_RUG
    if(GDIM == 2) {
      dialog::addBoolItem(XLAT("hypersian rug mode"), (rug::rugged), 'u');
      dialog::add_action(rug::select);
      }
#endif

    dialog::addBreak(50);
    
    char xch = 'p';
    for(eGeometry g: {gCubeTiling, gRhombic3, gBitrunc3}) {
      if(dim == 2) g = geometry;
      dialog::addItem(XLAT(ginf[g].menu_displayed_name), xch++);
      dialog::add_action([g] {
        stop_game();
        set_geometry(g);
        eu_input = eu_edit;
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
          dialog::add_key_action(ch, [i, j] {
            auto& T_edit = eu_edit.user_axes;
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
      auto& T0 = eu_input.user_axes;
      for(int i=0; i<3; i++)
      for(int j=0; j<3; j++) {
        shift(); T0[i][j] = argi();
        }
      build_torus3();
      }
    else if(argis("-t2")) {
      PHASEFROM(2);
      stop_game();
      auto& T0 = eu_input.user_axes;
      for(int i=0; i<2; i++)
      for(int j=0; j<2; j++) {
        shift(); T0[i][j] = argi();
        }
      shift(); eu_input.twisted = argi();
      build_torus3();
      }
    else if(argis("-twistthird")) {
      PHASEFROM(2);
      stop_game();
      shift(); int a = argi();
      shift(); int b = argi();
      shift(); int c = argi();
      eu_input = make_third_turn(a, b, c);
      build_torus3();
      }
    else if(argis("-twist3")) {
      PHASEFROM(2);
      stop_game();
      auto& T0 = eu_input.user_axes;
      for(int i=0; i<3; i++)
      for(int j=0; j<3; j++) T0[i][j] = 0;
      
      for(int i=0; i<3; i++) {
        shift(); T0[i][i] = argi();
        }
      shift(); eu_input.twisted = argi();
      build_torus3();
      }
    else if(argis("-hw")) {
      PHASEFROM(2);
      stop_game();
      shift();
      eu_input = make_hantzsche_wendt(argi());
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
              println(hlog, c, " @ ", cubemap()->ispacemap[c->master], " : ", i, "/", j, "/", k, "/", l, " :: ", T1, " vs ", T2);
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

EX int dscalar(gp::loc e1, gp::loc e2) {
  return 2 * (e1.first * e2.first + e1.second*e2.second) + (S3 == 3 ? e1.first*e2.second + e2.first * e1.second : 0);
  }

EX int dsquare(gp::loc e) { return dscalar(e, e)/2; }
  
EX int dcross(gp::loc e1, gp::loc e2) {
  return e1.first * e2.second - e1.second*e2.first;
  }

EX gp::loc full_coords2(cell *c) { 
  if(INVERSE) {
    cell *c1 = gp::get_mapped(c);
    return UIU(full_coords2(c1));
    }
  auto ans = eucmap()->ispacemap[c->master];
  if(S7 == 4 && BITRUNCATED) {
    if(c == c->master->c7) return to_loc(ans) * gp::loc(1,1);
    else {
      auto res = full_coords2(c->cmove(0)) + full_coords2(c->cmove(4));
      res.first /= 2;
      res.second /= 2;
      return res;
      }
    }
  if(BITRUNCATED)
    return to_loc(ans) * gp::loc(1,1) + (c == c->master->c7 ? gp::loc(0,0) : gp::eudir((c->c.spin(0)+4)%6));
  if(GOLDBERG) {
    auto li = gp::get_local_info(c);
    gp::loc shift(0,0);
    if(li.first_dir >= 0) shift = gp::eudir(li.last_dir) * li.relative;
    return to_loc(ans) * gp::param + shift;
    }
  return to_loc(ans);
  }

/** this is slow, but we use it only for small p's */
EX cell* at(gp::loc p) {
  cellwalker cw(currentmap->gamestart());
  while(p.first--) cw += revstep;
  cw ++;
  while(p.second--) cw += revstep;
  return cw.at;
  }
 
EX coord to_coord(gp::loc p) { return coord(p.first, p.second, 0); }

EX gp::loc sdxy() { return to_loc(eu.user_axes[1]) * gp::univ_param(); }

EX pair<bool, string> coord_display(const shiftmatrix& V, cell *c) {
  if(c != c->master->c7) return {false, ""};
  hyperpoint hx = eumove(main_axes[0]) * C0;
  hyperpoint hy = eumove(main_axes[1]) * C0;
  hyperpoint hz = WDIM == 2 ? C0 : eumove(main_axes[2]) * C0;
  hyperpoint h = kz(inverse(build_matrix(hx, hy, hz, C03)) * inverse_shift(ggmatrix(cwt.at->master->c7), V) * C0);

  if(WDIM == 3)  
    return {true, fts(h[0]) + "," + fts(h[1]) + "," + fts(h[2]) };
  else
    return {true, fts(h[0]) + "," + fts(h[1]) };
  }

EX gp::loc to_loc(const coord& v) { return gp::loc(v[0], v[1]); }

EX map<gp::loc, cdata>& get_cdata() { return eucmap()->eucdata; }
  
EX transmatrix eumove(coord co) {
  const double q3 = sqrt(double(3));
  if(WDIM == 3) {
    return eupush3(co[0], co[1], co[2]);
    }
  transmatrix Mat = Id;
  if(a4) {
    Mat[0][2] += co[0] * cgi.tessf;
    Mat[1][2] += co[1] * cgi.tessf;
    }
  else {
    Mat[0][2] += (co[0] + co[1] * .5) * cgi.tessf;
    Mat[1][2] += co[1] * q3 /2 * cgi.tessf;
    }
  if(embedded_plane) Mat = cgi.emb->base_to_actual(Mat);
  return Mat;
  }

EX transmatrix eumove(gp::loc co) { return eumove(to_coord(co)); }

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

EX void twist_once(gp::loc coo) {
  coo = coo - eu.twisted_vec * gp::univ_param();
  if(eu.twisted&8) {
    gp::loc ort = ort1() * eu.twisted_vec * gp::univ_param();
    auto s = ort * dscalar(coo, ort) * 2;
    auto v = dscalar(ort, ort);
    s.first /= v;
    s.second /= v;
    coo = coo - s;
    }
  }

EX int dist(int sx, int sy, bool reduce IS(true)) {
  int z0 = abs(sx);
  int z1 = abs(sy);
  if(a4 && BITRUNCATED)
    return (z0 == z1 && z0 > 0 && !reduce) ? z0+1: max(z0, z1);
  if(a4) return z0 + z1;
  int z2 = abs(sx+sy);
  return max(max(z0,z1), z2);
  }

EX int dist(gp::loc a, gp::loc b) {
  return dist(a.first-b.first, a.second-b.second, (a.first ^ a.second)&1);
  }

EX int cyldist(gp::loc a, gp::loc b) {
  a = to_loc(basic_canonicalize(to_coord(a)));
  b = to_loc(basic_canonicalize(to_coord(b)));
  
  if(!quotient) return dist(a, b);
  
  int best = 0;
  for(int sa=0; sa<16; sa++) {
    auto _a = a, _b = b;
    if(sa&1) twist_once(_a);
    if(sa&2) twist_once(_b);
    if(sa&4) _a = _a + eu.ortho_vec * gp::univ_param();
    if(sa&8) _b = _b + eu.ortho_vec * gp::univ_param();
    int val = dist(_a, _b);
    if(sa == 0 || val < best) best = val;
    }
  
  return best;
  }

EX void generate() {

  #if MAXMDIM >= 4
  if(fake::in()) {
    fake::generate();
    return;
    }

  auto v = euc::get_shifttable();
  
  auto& hsh = get_hsh();

  auto& cs = hsh.faces;

  cgi.loop = 4;
  cgi.schmid = 3;

  cs.clear();
  cs.resize(S7);

  if(S7 == 6) {
    cgi.adjcheck = 1;
    cgi.face = 4;
    for(int w=0; w<6; w++) {
      for(int a=0; a<4; a++) {
        int t[3];
        t[0] = (w>=3) ? -1 : 1;
        t[1] = among(a, 0, 3) ? -1 : 1;
        t[2] = among(a, 2, 3) ? -1 : 1;
        int x = w%3;
        int y = (x+2)%3;
        int z = (y+2)%3;
        cs[w].push_back(hpxy3(t[x]/2., t[y]/2., t[z]/2.));
        }
      }
    }

  if(S7 == 12) {
    cgi.adjcheck = sqrt(2);
    cgi.face = 4;
    for(int w=0; w<12; w++) {
      auto co = v[w];
      vector<int> valid;
      for(int c=0; c<3; c++) if(co[c]) valid.push_back(c);
      int third = 3 - valid[1] - valid[0];
      hyperpoint v0 = cpush0(valid[0], co[valid[0]] > 0 ? 1 : -1);
      hyperpoint v1 = cpush0(valid[1], co[valid[1]] > 0 ? 1 : -1);
      cs[w].push_back(v0);
      cs[w].push_back(v0/2 + v1/2 + cpush0(third, .5) - C0);
      cs[w].push_back(v1);
      cs[w].push_back(v0/2 + v1/2 + cpush0(third, -.5) - C0);
      }
    }

  if(S7 == 14) {
    cgi.adjcheck = 2;
    cgi.face = 4; /* the first face */
    auto v = euc::get_shifttable();
    for(int w=0; w<14; w++) {
      if(w%7 < 3) {
        int z = w>=7?-1:1;
        cs[w].push_back(cpush0(w%7, z) + cpush0((w%7+1)%3, 1/2.) - C0);
        cs[w].push_back(cpush0(w%7, z) + cpush0((w%7+2)%3, 1/2.) - C0);
        cs[w].push_back(cpush0(w%7, z) + cpush0((w%7+1)%3,-1/2.) - C0);
        cs[w].push_back(cpush0(w%7, z) + cpush0((w%7+2)%3,-1/2.) - C0);
        }
      else {
        auto t = v[w];
        ld x = t[0], y = t[1], z = t[2];
        for(hyperpoint h: {
          hpxy3(x, y/2, 0), hpxy3(x/2, y, 0), hpxy3(0, y, z/2),
          hpxy3(0, y/2, z), hpxy3(x/2, 0, z), hpxy3(x, 0, z/2)
          }) cs[w].push_back(h);
        }
      }
    }
  
  hsh.compute_hept();
  #endif
  }

/** @brief returns true if the current geometry is based on this module 
 *  (For example, Archimedean, kite, or fake with underlying non-Euclidean geometry returns false)
 */
EX bool in() { 
  if(fake::in()) return FPIU(in()); 
  if(geometry == gCubeTiling && (reg3::cubes_reg3 || !PURE)) return false;
  return meuclid && standard_tiling();
  }

EX bool in(int dim) { return in() && WDIM == dim; }
EX bool in(int dim, int s7) { return in(dim) && S7 == s7; }

EX }

EX gp::loc euc2_coordinates(cell *c) { 
  if(euc::in()) return euc::full_coords2(c); 
  hyperpoint h = calc_relative_matrix(c, currentmap->gamestart(), C0) * C0;
  return gp::loc(floor(h[0]), floor(h[1]));
  }

}
