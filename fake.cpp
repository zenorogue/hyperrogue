#include "hyper.h"

// Fake non-Euclidean

namespace hr {

EX namespace fake {

  EX ld scale;
  
  EX bool multiple;
  
  EX bool multiple_special_draw = true;
  EX bool recursive_draw = false;
  
  EX eGeometry underlying;
  EX geometry_information *underlying_cgip;
  EX hrmap *pmap;
  EX geometry_information *pcgip;
  EX eGeometry actual_geometry;

  EX int ordered_mode = 0;
  
  EX bool in() { return geometry == gFake; }

  EX void on_dim_change() { pmap->on_dim_change(); }
  
  /** like in() but takes slided arb into account */
  EX bool split() { return in() || arb::in_slided(); }
  
  EX bool available() {
    if(in()) return true;
    if(WDIM == 2 && standard_tiling() && (PURE || BITRUNCATED)) return true;
    if(arcm::in() && PURE) return true;
    if(WDIM == 2) return false;
    if(among(geometry, gBitrunc3)) return false;
    #if MAXMDIM >= 4
    if(reg3::in() && !among(variation, eVariation::pure, eVariation::subcubes, eVariation::coxeter, eVariation::bch_oct)) return false;
    return euc::in() || reg3::in();
    #else
    return euc::in();
    #endif
    }
  
  map<cell*, ld> random_order;

  // a dummy map that does nothing
  struct hrmap_fake : hrmap {
    hrmap *underlying_map;
    
    template<class T> auto in_underlying(const T& t) -> decltype(t()) {
      pcgip = cgip; 
      dynamicval<hrmap*> gpm(pmap, this);
      dynamicval<eGeometry> gag(actual_geometry, geometry);
      dynamicval<eGeometry> g(geometry, underlying);
      dynamicval<geometry_information*> gc(cgip, underlying_cgip);
      dynamicval<hrmap*> gu(currentmap, underlying_map);
      return t();
      }

    heptagon *getOrigin() override { return in_underlying([this] { return underlying_map->getOrigin(); }); }
    
    cell* gamestart() override { return in_underlying([this] { return underlying_map->gamestart(); }); }

    hrmap_fake(hrmap *u) {
      underlying_map = u;
      for(hrmap*& m: allmaps) if(m == underlying_map) m = this;
      if(currentmap == u) currentmap = this;
      }

    void find_cell_connection(cell *c, int d) override { 
      FPIU(createMov(c, d));
      }
  
    hrmap_fake() {
      in_underlying([this] { initcells(); underlying_map = currentmap; });
      for(hrmap*& m: allmaps) if(m == underlying_map) m = NULL;
      }
    
    ~hrmap_fake() { 
      in_underlying([this] {
        delete underlying_map; 
        });
      }
    
    heptagon *create_step(heptagon *parent, int d) override {
      return FPIU(currentmap->create_step(parent, d));
      }

    hyperpoint get_corner(cell *c, int cid, ld cf=3) override { 

      if(arcm::in() || hat::in()) {
        return underlying_map->get_corner(c, cid, cf);
        }

      hyperpoint h;
      h = FPIU(currentmap->get_corner(c, cid, cf));
      return befake(h);
      }

    transmatrix adj(cell *c, int d) override {
      if(hat::in()) return underlying_map->adj(c, d);
      if(variation == eVariation::coxeter) {
        array<int, 3> which;
        in_underlying([&which, c, d] {
          auto T = currentmap->adj(c, d);
          auto& f1 = currentmap->get_cellshape(c).faces_local[d];
          auto& f2 = currentmap->get_cellshape(c->move(d)).faces_local[c->c.spin(d)];
          for(int i=0; i<3; i++) {
            which[i] = -1;
            for(int j=0; j<isize(f2); j++)
              if(hdist(T * f2[j], f1[i]) < 1e-6)
                which[i] = j;
            }
          });
        auto& f1 = get_cellshape(c).faces_local[d];
        auto& f2 = get_cellshape(c->move(d)).faces_local[c->c.spin(d)];
        vector<ld> d1;
        for(auto& v: f1) d1.push_back(hdist0(normalize(v)));
        vector<hyperpoint> cf2(3);
        for(int i=0; i<3; i++)
          cf2[i] = f2[which[i]];
        transmatrix F2, F1;
        for(int i=0; i<3; i++) set_column(F2, i, cf2[i]);
        for(int i=0; i<3; i++) set_column(F1, i, f1[i]);

        auto dtang = [] (vector<hyperpoint> v) {
          if(euclid) return (v[1] - v[0]) ^ (v[2] - v[0]);
          transmatrix T = gpushxto0(normalize(v[0]));
          hyperpoint h = iso_inverse(T) * ((T*v[1]) ^ (T*v[2]));
          return h;
          };

        set_column(F2, 3, dtang(cf2));
        set_column(F1, 3, dtang(f1));
        transmatrix T = F1 * inverse(F2);
        return T;
        }
      transmatrix S1, S2;
      ld dist;
      #if MAXMDIM >= 4
      bool impure = reg3::in() && !PURE;
      #else
      bool impure = !PURE;
      #endif
      vector<int> mseq;
      if(impure) {
        mseq = FPIU ( currentmap->get_move_seq(c, d) );
        if(mseq.empty()) {
          auto& s1 = get_cellshape(c);
          auto& s2 = get_cellshape(c->move(d));
          return s1.from_cellcenter * s2.to_cellcenter;
          }
        if(isize(mseq) > 1)
          throw hr_exception("fake adj not implemented for isize(mseq) > 1");
        }
      in_underlying([c, d, &S1, &S2, &dist, &impure, &mseq] {
        #if CAP_ARCM
        dynamicval<bool> u(arcm::use_gmatrix, false);
        #endif
        transmatrix T;
        if(impure) {
          T = currentmap->adj(c->master, mseq[0]);
          }
        else {
          T = currentmap->adj(c, d);
          }
        S1 = rspintox(tC0(T));
        transmatrix T1 = spintox(tC0(T)) * T;
        dist = hdist0(tC0(T1));
        S2 = xpush(-dist) * T1;
        });
      
      if(impure) {
        auto& s1 = get_cellshape(c);
        auto& s2 = get_cellshape(c->move(d));
        S1 = s1.from_cellcenter * S1;
        S2 = S2 * s2.to_cellcenter;
        }

      #if CAP_ARCM
      if(arcm::in()) {
        int t = arcm::id_of(c->master);
        int t2 = arcm::id_of(c->move(d)->master);
        auto& cof = arcm::current_or_fake();
        cgi.adjcheck = cof.inradius[t/2] + cof.inradius[t2/2];
        }
      #else
      if(0) ;
      #endif
      
      else if(WDIM == 2) {
      
        ld dist;
        in_underlying([c, d, &dist] {
          dist = currentmap->spacedist(c, d);
          });
        
        auto& u = *underlying_cgip;
        if(dist == u.tessf) cgi.adjcheck = cgi.tessf;
        else if(dist == u.crossf) cgi.adjcheck = cgi.crossf;
        else if(dist == u.hexhexdist) cgi.adjcheck = cgi.hexhexdist;
        else cgi.adjcheck = dist * scale;        
        }
      
      else if(underlying == gBitrunc3) {
        ld x = (d % 7 < 3) ? 1 : sqrt(3)/2;
        x *= scale;
        cgi.adjcheck = 2 * atanh(x);
        }
      
      return S1 * xpush(cgi.adjcheck) * S2;
      }
    
    void draw_recursive(cell *c, const shiftmatrix& V, ld a0, ld a1, cell *parent, int depth) {
      if(!do_draw(c, V)) return;
      drawcell(c, V);
      
      if(depth >= 15) return;
      
      // queuestr(V, .2, fts(a0)+":"+fts(a1), 0xFFFFFFFF, 1);
      
      ld d = hdist0(tC0(V));

      if(false) {
        curvepoint(spin(-a0) * xpush0(d));
        curvepoint(spin(-a0) * xpush0(d+.2));
        curvepoint(spin(-a1) * xpush0(d+.2));
        curvepoint(spin(-a1) * xpush0(d));
        curvepoint(spin(-a0) * xpush0(d));
        queuecurve(shiftless(Id), 0xFF0000FF, 0, PPR::LINE);
        }


      indenter id(2);
      for(int i=0; i<c->type; i++) if(c->move(i) && c->move(i) != parent) {
        auto h0 = V * befake(FPIU(get_corner_position(c, i)));
        auto h1 = V * befake(FPIU(get_corner_position(c, (i+1) % c->type)));
        ld b0 = atan2(unshift(h0));
        ld b1 = atan2(unshift(h1));
        while(b1 < b0) b1 += TAU;
        if(a0 == -1) {
          draw_recursive(c->move(i), optimized_shift(V * adj(c, i)), b0, b1, c, depth+1);
          }
        else {
          if(b1 - b0 > M_PI) continue;
          
          cyclefix(b0, a0);
          if(b0 < a0) b0 = a0;
          
          cyclefix(b1, a1);
          if(b1 > a1) b1 = a1;
          
          if(b0 > b1) continue;
          
          draw_recursive(c->move(i), optimized_shift(V * adj(c, i)), b0, b1, c, depth+1);
          }
        }
      }

    transmatrix relative_matrixc(cell *h2, cell *h1, const hyperpoint& hint) override {
      if(arcm::in()) return underlying_map->relative_matrix(h2, h1, hint);
      if(h1 == h2) return Id;
  
      for(int a=0; a<h1->type; a++) if(h1->move(a) == h2)
        return adj(h1, a);

      return Id;
      }

    transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
      if(arcm::in()) return underlying_map->relative_matrix(h2, h1, hint);
      return relative_matrix(h2->c7, h1->c7, hint);
      }

    void draw_at(cell *at, const shiftmatrix& where) override {
      sphereflip = Id;
      
      // for(int i=0; i<S6; i++) queuepoly(ggmatrix(cwt.at), shWall3D[i], 0xFF0000FF);
      
      if(pmodel == mdDisk && WDIM == 2 && recursive_draw) {
        draw_recursive(at, where, -1, -1, nullptr, 0);
        return;
        }
      
      dq::clear_all();
      
      int id = 0;
      int limit = 100 * pow(1.2, sightrange_bonus);
      if(WDIM == 3 || vid.use_smart_range) 
        limit = INT_MAX;
        
      if(ordered_mode && !(multiple && multiple_special_draw)) {
        using pct = pair<cell*, shiftmatrix>;
        auto comparer = [] (pct& a1, pct& a2) { 
          if(ordered_mode > 2) {
            auto val = [] (pct& a) {
              if(!random_order.count(a.first))
                random_order[a.first] = randd() * 2;
              return random_order[a.first] + hdist0(tC0(a.second));
              };
            return val(a1) > val(a2);
            }
          return a1.second[LDIM][LDIM] > a2.second[LDIM][LDIM]; 
          };
        std::priority_queue<pct, std::vector<pct>, decltype(comparer)> myqueue(comparer);
        
        auto enq = [&] (cell *c, const shiftmatrix& V) {
          if(!c) return;
          if(ordered_mode == 1 || ordered_mode == 3) {
            if(dq::visited_c.count(c)) return;
            dq::visited_c.insert(c);
            }
          myqueue.emplace(c, V);          
          };

        enq(centerover, cview());

        while(!myqueue.empty()) {
          auto& p = myqueue.top();
          id++;
          cell *c = p.first;
          shiftmatrix V = p.second;
          myqueue.pop();
          
          if(ordered_mode == 2 || ordered_mode == 4) {
            if(dq::visited_c.count(c)) continue;
            dq::visited_c.insert(c);
            }

          if(!do_draw(c, V)) continue;
          drawcell(c, V);

          if(in_wallopt() && isWall3(c) && isize(dq::drawqueue_c) > 1000) continue;

          if(id > limit) continue;
    
          for(int i=0; i<c->type; i++) if(c->move(i)) {
            enq(c->move(i), optimized_shift(V * adj(c, i)));
            }
          }
        
        return;
        }

      auto enqueue = (multiple && multiple_special_draw ? dq::enqueue_by_matrix_c : dq::enqueue_c);
      enqueue(at, where);
      
      while(!dq::drawqueue_c.empty()) {
        auto& p = dq::drawqueue_c.front();
        id++;
        cell *c = p.first;
        shiftmatrix V = p.second;
        dq::drawqueue_c.pop();

        if(!do_draw(c, V)) continue;
        drawcell(c, V);
        if(in_wallopt() && isWall3(c) && isize(dq::drawqueue_c) > 1000) continue;
        
        if(id > limit) continue;
    
        for(int i=0; i<c->type; i++) if(c->move(i)) {
          enqueue(c->move(i), optimized_shift(V * adj(c, i)));
          }
        }
      }

    ld spin_angle(cell *c, int d) override {
      return underlying_map->spin_angle(c,d);
      }

    int shvid(cell *c) override {
      return FPIU( currentmap->shvid(c) );
      }

    subcellshape& get_cellshape(cell *c) override {
      return *FPIU( (cgip = pcgip, &(currentmap->get_cellshape(c))) );
      }

    transmatrix ray_iadj(cell *c, int i) override {
      if(WDIM == 2)
        return to_other_side(get_corner(c, i), get_corner(c, i+1));
      #if MAXMDIM >= 4
      if(PURE) return iadj(c, i);      
      auto& v = get_cellshape(c).faces_local[i];
      hyperpoint h = 
        project_on_triangle(v[0], v[1], v[2]);
      transmatrix T = rspintox(h);
      return T * xpush(-2*hdist0(h)) * spintox(h);
      #else
      return Id;
      #endif
      }
    };
  
  EX hrmap* new_map() { return new hrmap_fake; }

  EX hrmap* get_umap() { if(!dynamic_cast<hrmap_fake*>(currentmap)) return nullptr; else return ((hrmap_fake*)currentmap)->underlying_map; }

  #if HDR
  template<class T> auto in_underlying_geometry(const T& f) -> decltype(f()) {
    if(!fake::in()) return f();
    pcgip = cgip; 
    dynamicval<eGeometry> g(geometry, underlying);
    dynamicval<eGeometry> gag(actual_geometry, geometry);
    dynamicval<geometry_information*> gc(cgip, underlying_cgip);
    dynamicval<hrmap*> gpm(pmap, currentmap);
    dynamicval<hrmap*> gm(currentmap, get_umap());
    return f();
    }
  
  #define FPIU(x) hr::fake::in_underlying_geometry([&] { return (x); })
  #endif

EX hyperpoint befake(hyperpoint h) {
  auto h1 = h / h[LDIM] * scale;
  h1[LDIM] = 1;
  if(material(h1) > 1e-3)
    h1 = normalize(h1);
  return h1;
  }

EX vector<hyperpoint> befake(const vector<hyperpoint>& v) {
  vector<hyperpoint> res;
  for(auto& h: v) res.push_back(befake(h));
  return res;
  }

EX vector<vector<hyperpoint>> befake(const vector<vector<hyperpoint>>& v) {
  vector<vector<hyperpoint>> res;
  for(auto& h: v) res.push_back(befake(h));
  return res;
  }

EX ld compute_around(bool setup) {
  auto &ucgi = *underlying_cgip;
  
  auto fcs = befake(ucgi.heptshape->faces);
  
  if(setup) {
    cgi.heptshape->faces = fcs;
    cgi.heptshape->compute_hept();
    }
  
  hyperpoint h = Hypc;
  for(int i=0; i<ucgi.face; i++) h += fcs[0][i];
  if(material(h) > 0)
    h = normalize(h);
  
  if(setup)
    cgi.adjcheck = 2 * hdist0(h);

  hyperpoint h2 = rspintox(h) * xpush0(2 * hdist0(h));
  
  auto kh= kleinize(h);
  auto k0 = kleinize(fcs[0][0]);
  auto k1 = kleinize(fcs[0][1]);
  
  auto vec = k1 - k0;
  
  // u = fcs[0] + vec * z

  // (f1-u) | (vec-u) = 0
  // (f1 - f0 + vec*z) | 
  
  // (vec | h2-vec*z)  == (vec | h2) - (vec | vec*z) == 0
  
  auto z = (vec|(kh-k0)) / (vec|vec);
  
  hyperpoint u = k0 + vec * z;

  if(material(u) <= 0) 
    return HUGE_VAL;

  u = normalize(u);
  
  h2 = spintox(u) * h2;
  u = spintox(u) * u;
  
  h2 = gpushxto0(u) * h2;
  u = gpushxto0(u) * u;

  ld x = hypot(h2[1], h2[2]);
  ld y = h2[0];
  
  ld ans = 360 / (90 + atan(y/x) / degree);
  
  return ans;
  }

EX void generate() {
  FPIU( cgi.require_basics() );
  #if MAXMDIM >= 4
  auto &ucgi = *underlying_cgip;
  
  cgi.loop = ucgi.loop;
  cgi.face = ucgi.face;
  cgi.schmid = ucgi.schmid;

  auto& hsh = get_hsh();
  
  hsh = *ucgi.heptshape;

  for(int b=0; b<32; b++)
    cgi.spins[b] = ucgi.spins[b];
  
  compute_around(true);
  hsh.compute_hept();
  reg3::compute_ultra();
  
  reg3::generate_subcells();
  if(variation == eVariation::coxeter) {
    for(int i=0; i<isize(cgi.subshapes); i++) {
      auto& s = cgi.subshapes[i];
      s.faces_local = ucgi.subshapes[i].faces_local;
      for(auto &face: s.faces_local) for(auto& v: face) {
        v = kleinize(v);
        for(int i=0; i<3; i++) v[i] *= scale;
        }
      reg3::make_vertices_only(s.vertices_only, s.faces_local);
      }
    }
  #endif
  }

int get_middle() {
  if(S7 == 20) return 5;
  if(S7 == 8) return 4;
  return 3;
  }

EX ld around;

/** @brief the value of 'around' which makes the tiling Euclidean */
EX ld compute_euclidean() {
  #if CAP_ARCM
  if(arcm::in()) return arcm::current.N * 2 / arcm::current.euclidean_angle_sum;
  #endif
  if(underlying == gAperiodicHat) return 6;
  if(WDIM == 2) return 4 / (S7-2.) + 2;


  if(underlying == gRhombic3) return 3;
  if(underlying == gBitrunc3) return 2.55208;
  int middle = get_middle();

  if(!fake::in()) underlying_cgip = cgip;
    
  return M_PI / asin(cos(M_PI/middle) / sin(M_PI/underlying_cgip->face));
  }

EX ld around_orig() {
  #if CAP_ARCM
  if(arcm::in())
    return arcm::current.N;
  #endif
  if(hat::in()) return 6;
  if(WDIM == 2)
    return S3;
  if(underlying == gRhombic3)
    return 3;
  if(underlying == gBitrunc3)
    return 2.24259;
  return
    geometry == gFake ? underlying_cgip->loop : cgi.loop;
  }

EX geometryinfo1 geometry_of_curvature(ld curvature, int dim) {
  if(curvature == 0)
    return WDIM == 3 ? giEuclid3 : giEuclid2;

  if(curvature < 0)
    return WDIM == 3 ? giHyperb3 : giHyperb2;
    
  return WDIM == 3 ? giSphere3 : giSphere2;
  }

EX void compute_scale() {

  ld good = compute_euclidean();
  
  if(around < 0) around = good;
  
  if(abs(good - around) < 1e-6) good = around;
  
  int s3 = around_orig();

  multiple = false;
  int mcount = int(around / s3 + .5);
  multiple = abs(around - mcount * s3) < 1e-6;
  
  ginf[gFake].g = geometry_of_curvature(good - around, WDIM);

  geom3::apply_always3();
  ld around_ideal = 1/(1/2. - 1./get_middle());
  
  bool have_ideal = abs(around_ideal - around) < 1e-6;
  if(underlying == gRhombic3 || underlying == gBitrunc3) have_ideal = false;
  
  if(arcm::in()) {
    ginf[gFake].tiling_name = "(" + ginf[gArchimedean].tiling_name + ")^" + fts(around / around_orig());
    return;
    }
  else if(WDIM == 2) {
    ginf[gFake].tiling_name = lalign(0, "{", S7, ",", around, "}");
    return;
    }
  else if(euclid) scale = 1;
  else if(have_ideal) {
    hyperpoint h0 = underlying_cgip->heptshape->faces[0][0];
    auto s = kleinize(h0);
    ld d = hypot_d(LDIM, s);
    scale = 1/d;

    hyperpoint h = h0;
    auto h1 = h / h[WDIM] * scale;
    h1[WDIM] = 1;
    set_flag(ginf[gFake].flags, qIDEAL, true);
    set_flag(ginf[gFake].flags, qULTRA, false);
    }
  else {
    set_flag(ginf[gFake].flags, qIDEAL, false);
    set_flag(ginf[gFake].flags, qULTRA, around > around_ideal);
    ld minscale = 0, maxscale = 10;
    for(int it=0; it<100; it++) {
      scale = (minscale + maxscale) /  2;
      ld ar = compute_around(false);
      if(sphere) {
        if(ar < around) maxscale = scale;
        else minscale = scale;
        }
      else {
        if(ar > around) maxscale = scale;
        else minscale = scale;
        }
      }
    
    /* ultra a bit earlier */    
    if(underlying == gRhombic3 || underlying == gBitrunc3) {
      auto fcs = befake(underlying_cgip->heptshape->faces[0][0]);
      set_flag(ginf[gFake].flags, qULTRA, material(fcs) < 0);
      }
    }
  
  auto& u = underlying_cgip;
  ginf[gFake].tiling_name = lalign(0, "{", u->face, ",", get_middle(), ",", around, "}");
  }

void set_gfake(ld _around) {
  cgi.require_basics();
  underlying = geometry;
  underlying_cgip = cgip;  
  ginf[gFake] = ginf[underlying];
  
  geometry = gFake;
  
  around = _around;
  
  compute_scale();
  check_cgi();
  cgi.require_basics();

  if(currentmap) new hrmap_fake(currentmap);
  if(hat::in()) hat::reshape();
  }

EX void change_around() {
  if(around >= 0 && around <= 2) return;

  ld t = in() ? scale : 1;
  hyperpoint h = inverse_exp(shiftless(tC0(View)));
  transmatrix T = gpushxto0(tC0(View)) * View;
  
  ld range = sightranges[geometry];
  
  if(!fake::in()) {
    underlying = geometry;
    if(around == around_orig()) return; /* do nothing */
    set_gfake(around);
    }
  
  else {    
    compute_scale();
    ray::reset_raycaster();

    /* to compute scale */
    if(WDIM == 2)
      cgi.prepare_basics();
    }

  t = scale / t;
  h *= t;
  View = rgpushxto0(direct_exp(h)) * T;
  fixmatrix(View);
  
  sightranges[gFake] = range * t;
  #if CAP_TEXTURE
  texture::config.remap();
  #endif
  geom3::apply_always3();
  }

EX void configure() {
  if(!in()) {
    underlying_cgip = cgip;
    around = around_orig();
    }
  dialog::editNumber(around, 2.01, 10, 1, around, "fake curvature", 
    "This feature lets you construct the same tiling, but "
    "from shapes of different curvature.\n\n"
    "The number you give here is (2D) vertex degree or (3D) "
    "the number of cells around an edge.\n\n"
    );
  if(fake::in())
    dialog::reaction = change_around;
  else  
    dialog::reaction_final = change_around;
  dialog::extra_options = [] {
    ld e = compute_euclidean();
    dialog::addSelItem("Euclidean", fts(e), 'E');
    dialog::add_action([e] {
      around = e;
      popScreen();
      change_around();
      });

    dialog::addSelItem("original", fts(around_orig()), 'O');
    dialog::add_action([] {
      around = around_orig();
      popScreen();
      change_around();
      });

    dialog::addSelItem("double original", fts(2 * around_orig()), 'D');
    dialog::add_action([] {
      around = 2 * around_orig();
      popScreen();
      change_around();
      });
    
    dialog::addBoolItem_action("draw all if multiple of original", multiple_special_draw, 'M');
    dialog::addBoolItem_action("draw copies (2D only)", recursive_draw, 'C');

    dialog::addBoolItem_choice("unordered", ordered_mode, 0, 'U');
    dialog::addBoolItem_choice("pre-ordered", ordered_mode, 1, 'P');
    dialog::addBoolItem_choice("post-ordered", ordered_mode, 2, 'Q');

    };
  }
  
#if CAP_COMMANDLINE
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-gfake-euc")) {
    start_game();
    around = compute_euclidean();
    change_around();
    }
  else if(argis("-gfake")) {
    start_game();
    shift_arg_formula(around, change_around);
    }
  else if(argis("-gfake-order")) {
    shift(); ordered_mode = argi();
    }
  else return 1;
  return 0;
  }

auto fundamentalhook = addHook(hooks_args, 100, readArgs);
#endif

EX }

}

