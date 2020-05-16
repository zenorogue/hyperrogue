#include "hyper.h"

// Fake non-Euclidean

namespace hr {

EX namespace fake {

  EX ld scale;
  
  EX bool multiple;
  
  EX eGeometry underlying;
  EX geometry_information *underlying_cgip;
  EX hrmap *pmap;
  EX geometry_information *pcgip;
  EX eGeometry actual_geometry;
  
  EX bool in() { return geometry == gFake; }

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

    hrmap_fake() {
      in_underlying([this] { initcells(); underlying_map = currentmap; });
      for(hrmap*& m: allmaps) if(m == underlying_map) m = NULL;
      }
    
    heptagon *create_step(heptagon *parent, int d) override {
      parent->c.connect(d, parent, d, false);
      return parent;
      }

    transmatrix adj(cell *c, int d) override {
      transmatrix S1, S2;
      ld dist;
      in_underlying([c, d, &S1, &S2, &dist] {
        transmatrix T = currentmap->adj(c, d);
        S1 = rspintox(tC0(T));
        transmatrix T1 = spintox(tC0(T)) * T;
        dist = hdist0(tC0(T1));
        S2 = xpush(-dist) * T1;
        });
      
      if(WDIM == 2) {
        
        hyperpoint a1, a2, b1, b2;

        in_underlying([c, d, &a1, &a2, &b1, &b2] {
          a1 = get_corner_position(c, d);
          a2 = get_corner_position(c, (d+1) % c->type);
          
          auto c1 = c->move(d);
          auto d1 = c->c.spin(d);
          b1 = get_corner_position(c1, d1);
          b2 = get_corner_position(c1, (d1+1) % c1->type);
          });
        
        cgi.adjcheck = hdist0(mid(befake(a1), befake(a2))) + hdist0(mid(befake(b1), befake(b2)));
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
      if(h1 == h2) return Id;
  
      for(int a=0; a<h1->type; a++) if(h1->move(a) == h2)
        return adj(h1, a);

      return Id;
      }

    transmatrix relative_matrix(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
      return relative_matrix(h2->c7, h1->c7, hint);
      }

    void draw() override {
      sphereflip = Id;
      
      // for(int i=0; i<S6; i++) queuepoly(ggmatrix(cwt.at), shWall3D[i], 0xFF0000FF);
      
      if(pmodel == mdDisk && WDIM == 2) {
        draw_recursive(centerover, cview(), -1, -1, nullptr, 0);
        return;
        }
      
      dq::visited_c.clear();
      dq::visited_by_matrix.clear();
      auto enqueue = (multiple ? dq::enqueue_by_matrix_c : dq::enqueue_c);
      enqueue(centerover, cview());
      
      while(!dq::drawqueue_c.empty()) {
        auto& p = dq::drawqueue_c.front();
        cell *c = get<0>(p);
        transmatrix V = get<1>(p);
        dynamicval<ld> b(band_shift, get<2>(p));
        bandfixer bf(V);
        dq::drawqueue_c.pop();

        if(!do_draw(c, V)) continue;
        drawcell(c, V);
        if(in_wallopt() && isWall3(c) && isize(dq::drawqueue_c) > 1000) continue;
    
        for(int i=0; i<S7; i++) if(c->move(i)) {
          enqueue(c->move(i), V * adj(c, i));
          }
        }
      }
    };
  
  EX hrmap* new_map() { return new hrmap_fake; };

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
  
  println(hlog, "h = ", hdist0(h), " ucgi = ", format("%p", &ucgi), " @ ", hyperbolic, " / ", sphere, " h2 = ", h2);
  
  ld x = hypot(h2[1], h2[2]);
  ld y = h2[0];
  return 360 / (90 + atan(y/x) / degree);
  }

EX void generate() {
  println(hlog, "Generating fake");
  FPIU( cgi.require_basics() );
  auto &ucgi = *underlying_cgip;
  
  cgi.loop = ucgi.loop;
  cgi.face = ucgi.face;

  for(int a=0; a<16; a++)
  for(int b=0; b<16; b++) {
    cgi.dirs_adjacent[a][b] = ucgi.dirs_adjacent[a][b];
    cgi.next_dir[a][b] = ucgi.next_dir[a][b];
    }

  for(int b=0; b<12; b++)
    cgi.spins[b] = ucgi.spins[b];
  
  compute_around(true);
  }

int get_middle() {
  if(S7 == 20) return 5;
  if(S7 == 8) return 4;
  return 3;
  }

EX ld around;

EX void compute_scale() {

  int middle = get_middle();
  
  // the value of 'around' which makes the tiling Euclidean
  ld good = M_PI / asin(cos(M_PI/middle) / sin(M_PI/underlying_cgip->face));
  
  println(hlog, "good = ", good);
  
  if(abs(good - around) < 1e-6) good = around;

  multiple = false;
  for(int k=1; k<10; k++)
    if(abs(around - underlying_cgip->loop) < 1e-6)
      multiple = true;

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
  println(hlog, "around_ideal = ", around_ideal);
  
  if(euclid) scale = 1;
  else if(abs(around_ideal - around) < 1e-6) {
    hyperpoint h0 = underlying_cgip->cellshape[0];
    auto s = kleinize(h0);
    ld d = hypot_d(LDIM, s);
    scale = 1/d;

    hyperpoint h = h0;
    auto h1 = h / h[WDIM] * scale;
    h1[WDIM] = 1;
    
    println(hlog, "material = ", material(h1));
    }
  else {
    ld minscale = 0, maxscale = 10;
    for(int it=0; it<100; it++) {
      scale = (minscale + maxscale) /  2;
      ld ar = compute_around(false);
      println(hlog, "scale = ", scale, " ar = ", ar);
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
  sightranges[gFake] = sightranges[underlying] * scale;
  }

void set_gfake(int c, ld _around) {
  stop_game();
  cgi.require_basics();
  fake::scale = scale;
  underlying = geometry;
  underlying_cgip = cgip;  
  ginf[gFake] = ginf[underlying];
  
  set_geometry(gFake);
  
  around = _around;
  
  compute_scale();
  check_cgi();

  compute_scale();
  check_cgi();
  }

EX void change_around() {
  if(around > 2) {
    ld t = sightranges[gFake] / (sightranges[underlying] * scale);
    compute_scale();
    ray::reset_raycaster();
    sightranges[gFake] *= t;
    }
  };
  
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-gfake")) {
    if(fake::in()) shift_arg_formula(around, change_around);
    else {
      shift(); int c = argi();
      ld around;
      shift_arg_formula(around);
      set_gfake(c, around);
      }
    }
  else return 1;
  return 0;
  }

auto fundamentalhook = addHook(hooks_args, 100, readArgs);

EX }

}

