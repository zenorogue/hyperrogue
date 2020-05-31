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
  
  EX bool in() { return geometry == gFake; }
  
  EX bool available() {
    if(in()) return true;
    if(GDIM == 2 && standard_tiling() && (PURE || BITRUNCATED)) return true;
    if(arcm::in() && PURE) return true;
    if(GDIM == 2) return false;
    if(among(geometry, gRhombic3, gBitrunc3)) return false;
    return euc::in() || reg3::in();
    }

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
      parent->c.connect(d, parent, d, false);
      return parent;
      }

    transmatrix adj(cell *c, int d) override {
      transmatrix S1, S2;
      ld dist;
      in_underlying([c, d, &S1, &S2, &dist] {
        dynamicval<bool> u(arcm::use_gmatrix, false);
        transmatrix T = currentmap->adj(c, d);
        S1 = rspintox(tC0(T));
        transmatrix T1 = spintox(tC0(T)) * T;
        dist = hdist0(tC0(T1));
        S2 = xpush(-dist) * T1;
        });
      
      if(arcm::in()) {
        int t = arcm::id_of(c->master);
        int t2 = arcm::id_of(c->move(d)->master);
        auto& cof = arcm::current_or_fake();
        cgi.adjcheck = cof.inradius[t/2] + cof.inradius[t2/2];
        }
      
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
      
      return S1 * xpush(cgi.adjcheck) * S2;
      }
    
    void draw_recursive(cell *c, const transmatrix& V, ld a0, ld a1, cell *parent, int depth) {
      band_shift = 0;
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
        queuecurve(0xFF0000FF, 0, PPR::LINE);
        }


      indenter id(2);
      for(int i=0; i<c->type; i++) if(c->move(i) && c->move(i) != parent) {
        auto h0 = V * befake(FPIU(get_corner_position(c, i)));
        auto h1 = V * befake(FPIU(get_corner_position(c, (i+1) % c->type)));
        ld b0 = atan2(h0);
        ld b1 = atan2(h1);
        while(b1 < b0) b1 += 2 * M_PI;
        if(a0 == -1) {
          draw_recursive(c->move(i), V * adj(c, i), b0, b1, c, depth+1);
          }
        else {
          if(b1 - b0 > M_PI) continue;
          
          if(b0 < a0 - M_PI) b0 += 2 * M_PI;
          if(b0 > a0 + M_PI) b0 -= 2 * M_PI;
          if(b0 < a0) b0 = a0;
          
          if(b1 > a1 + M_PI) b1 -= 2 * M_PI;
          if(b1 < a1 - M_PI) b1 += 2 * M_PI;
          if(b1 > a1) b1 = a1;
          
          if(b0 > b1) continue;
          
          draw_recursive(c->move(i), V * adj(c, i), b0, b1, c, depth+1);
          }
        }
      }

    transmatrix relative_matrix(cell *h2, cell *h1, const hyperpoint& hint) override {
      if(arcm::in()) return underlying_map->relative_matrix(h2, h1, hint);
      if(h1 == h2) return Id;
  
      for(int a=0; a<h1->type; a++) if(h1->move(a) == h2)
        return adj(h1, a);

      return Id;
      }

    transmatrix relative_matrix(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
      if(arcm::in()) return underlying_map->relative_matrix(h2, h1, hint);
      return relative_matrix(h2->c7, h1->c7, hint);
      }

    void draw() override {
      sphereflip = Id;
      
      // for(int i=0; i<S6; i++) queuepoly(ggmatrix(cwt.at), shWall3D[i], 0xFF0000FF);
      
      if(pmodel == mdDisk && WDIM == 2 && recursive_draw) {
        draw_recursive(centerover, cview(), -1, -1, nullptr, 0);
        return;
        }
      
      dq::visited_c.clear();
      dq::visited_by_matrix.clear();
      auto enqueue = (multiple && multiple_special_draw ? dq::enqueue_by_matrix_c : dq::enqueue_c);
      enqueue(centerover, cview());
      
      int id = 0;
      int limit = 100 * pow(1.2, sightrange_bonus);
      if(WDIM == 3 || vid.use_smart_range) 
        limit = INT_MAX;
      
      while(!dq::drawqueue_c.empty()) {
        auto& p = dq::drawqueue_c.front();
        id++;
        cell *c = get<0>(p);
        transmatrix V = get<1>(p);
        dynamicval<ld> b(band_shift, get<2>(p));
        bandfixer bf(V);
        dq::drawqueue_c.pop();

        if(!do_draw(c, V)) continue;
        drawcell(c, V);
        if(in_wallopt() && isWall3(c) && isize(dq::drawqueue_c) > 1000) continue;
        
        if(id > limit) continue;
    
        for(int i=0; i<c->type; i++) if(c->move(i)) {
          enqueue(c->move(i), V * adj(c, i));
          }
        }
      }

    ld spin_angle(cell *c, int d) override {
      return underlying_map->spin_angle(c,d);
      }
    };
  
  EX hrmap* new_map() { return new hrmap_fake; }

  EX hrmap* get_umap() { if(!dynamic_cast<hrmap_fake*>(currentmap)) return nullptr; else return ((hrmap_fake*)currentmap)->underlying_map; }

  #if HDR
  template<class T> auto in_underlying_geometry(const T& f) -> decltype(f()) {
    if(!fake::in()) return f();
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
  auto h1 = h / h[WDIM] * scale;
  h1[WDIM] = 1;
  if(material(h1) > 1e-3)
    h1 = normalize(h1);
  return h1;
  }

EX vector<hyperpoint> befake(const vector<hyperpoint>& v) {
  vector<hyperpoint> res;
  for(auto& h: v) res.push_back(befake(h));
  return res;
  }

EX ld compute_around(bool setup) {
  auto &ucgi = *underlying_cgip;
  
  auto fcs = befake(ucgi.cellshape);
  
  if(setup) {
    cgi.cellshape = fcs;
    cgi.vertices_only = befake(ucgi.vertices_only);
    }
  
  hyperpoint h = Hypc;
  for(int i=0; i<ucgi.face; i++) h += fcs[i];
  if(material(h) > 0)
    h = normalize(h);
  
  if(setup)
    cgi.adjcheck = 2 * hdist0(h);

  hyperpoint u = Hypc;
  u += fcs[0];
  u += fcs[1];
  
  if(material(u) <= 0) 
    return HUGE_VAL;

  u = normalize(u);
  hyperpoint h2 = rspintox(h) * xpush0(2 * hdist0(h));
  
  h2 = spintox(u) * h2;
  u = spintox(u) * u;
  
  h2 = gpushxto0(u) * h2;
  u = gpushxto0(u) * u;

  ld x = hypot(h2[1], h2[2]);
  ld y = h2[0];
  return 360 / (90 + atan(y/x) / degree);
  }

EX void generate() {
  FPIU( cgi.require_basics() );
  #if MAXMDIM >= 4
  auto &ucgi = *underlying_cgip;
  
  cgi.loop = ucgi.loop;
  cgi.face = ucgi.face;
  cgi.schmid = ucgi.schmid;

  for(int a=0; a<16; a++)
  for(int b=0; b<16; b++) {
    cgi.dirs_adjacent[a][b] = ucgi.dirs_adjacent[a][b];
    cgi.next_dir[a][b] = ucgi.next_dir[a][b];
    }

  for(int b=0; b<12; b++)
    cgi.spins[b] = ucgi.spins[b];
  
  compute_around(true);
  reg3::compute_ultra();  
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
  if(arcm::in()) return arcm::current.N * 2 / arcm::current.euclidean_angle_sum;
  if(WDIM == 2) return 4 / (S7-2.) + 2;

  int middle = get_middle();
    
  return M_PI / asin(cos(M_PI/middle) / sin(M_PI/underlying_cgip->face));
  }

EX int around_orig() {
  if(arcm::in())
    return arcm::current.N;
  if(WDIM == 2)
    return S3;
  return
    underlying_cgip->loop;
  }

EX void compute_scale() {

  ld good = compute_euclidean();
  
  if(around < 0) around = good;
  
  if(abs(good - around) < 1e-6) good = around;
  
  int s3 = around_orig();

  multiple = false;
  int mcount = int(around / s3 + .5);
  multiple = abs(around - mcount * s3) < 1e-6;

  if(around == good) {  
    ginf[gFake].g = WDIM == 3 ? giEuclid3 : giEuclid2;
    }
  
  if(around > good) {
    ginf[gFake].g = WDIM == 3 ? giHyperb3 : giHyperb2;
    }

  if(around < good) {
    ginf[gFake].g = WDIM == 3 ? giSphere3 : giSphere2;
    }

  ld around_ideal = 1/(1/2. - 1./get_middle());
  
  if(arcm::in()) {
    ginf[gFake].tiling_name = "(" + ginf[gArchimedean].tiling_name + ")^" + fts(around / around_orig());
    return;
    }
  else if(WDIM == 2) {
    ginf[gFake].tiling_name = lalign(0, "{", S7, ",", around, "}");
    return;
    }
  else if(euclid) scale = 1;
  else if(abs(around_ideal - around) < 1e-6) {
    hyperpoint h0 = underlying_cgip->cellshape[0];
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
  
  ginf[gFake].xcode = no_code;
  
  if(currentmap) new hrmap_fake(currentmap);
  }

EX void change_around() {
  if(around >= 0 && around <= 2) return;

  ld t = in() ? scale : 1;
  hyperpoint h = inverse_exp(tC0(View));
  transmatrix T = gpushxto0(tC0(View)) * View;
  
  ld range = sightranges[geometry];
  
  if(!fake::in()) {
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

  println(hlog, "scale = ", t, " -> ", scale, " range = ", range);
  t = scale / t;
//  println(hlog, "t = ", t, " h distance = ", hypot_d(3, h), " for ", h);
  h *= t;
  View = rgpushxto0(direct_exp(h)) * T;
  fixmatrix(View);
  playermoved = false;
  sightranges[gFake] = range * t;
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

    };
  }
  
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-gfake")) {
    shift_arg_formula(around, change_around);
    }
  else return 1;
  return 0;
  }

auto fundamentalhook = addHook(hooks_args, 100, readArgs);

EX }

}

