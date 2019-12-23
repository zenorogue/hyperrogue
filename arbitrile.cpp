// Hyperbolic Rogue -- Arbitrary Tilings
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file arbitrile.cpp 
 *  \brief Arbitrary tilings
 *
 *  Arbitrary tilings, defined in .tes files.
 */

#include "hyper.h"
namespace hr {

EX namespace arb {

#if HDR

struct shape {
  int id;
  vector<hyperpoint> vertices;
  vector<ld> angles;
  vector<ld> edges;
  vector<tuple<int, int, int>> connections;
  int size() { return isize(vertices); }
  void build_from_angles_edges();
  };

struct arbi_tiling {

  vector<shape> shapes;

  geometryinfo1& get_geometry();
  eGeometryClass get_class() { return get_geometry().kind; }

  ld scale();
  };
#endif

EX arbi_tiling current;


/** id of vertex in the arbitrary tiling */

EX short& id_of(heptagon *h) { return h->zebraval; }

void shape::build_from_angles_edges() {
  hyperpoint at(0, 0, 1, 0);
  hyperpoint direction(1, 0, 0, 0);
  vertices.clear();
  int n = isize(angles);
  hyperpoint ctr = Hypc;
  for(int i=0; i<n; i++) {
    vertices.push_back(at);
    ctr += at;
    at += direction * edges[i];
    direction = spin(angles[i] * degree) * direction;
    }
  ctr = normalize(ctr);
  for(auto& v: vertices) v = v + (C0 - ctr);
  }

void load(const string& fname) {
  fhstream f(fname, "rt");
  string s = scan<string>(f);
  auto& c = current;
  c.shapes.clear();
  int N = scan<int>(f);
  int tc = 0;
  for(int i=0; i<N; i++) {
    c.shapes.emplace_back();
    auto& cc = c.shapes.back();
    cc.id = i;
    int siz = scan<int>(f);
    for(int s=0; s<siz; s++) {
      cc.edges.push_back(scan<ld>(f));
      cc.angles.push_back(scan<ld>(f));
      }
    cc.build_from_angles_edges();
    cc.connections.resize(cc.size());
    tc += siz;
    }
  while(true) {
    int ai = scan<int>(f);
    if(ai < 0) break;
    int as = scan<int>(f);
    int bi = scan<int>(f);
    int bs = scan<int>(f);
    int m = scan<int>(f);
    c.shapes[ai].connections[as] = {bi, bs, m};
    c.shapes[bi].connections[bs] = {ai, as, m};
    }
  }

geometryinfo1& arbi_tiling::get_geometry() {
  return ginf[gEuclid].g;
  }

map<heptagon*, vector<pair<heptagon*, transmatrix> > > altmap;

EX map<heptagon*, pair<heptagon*, transmatrix>> arbi_matrix;

EX hrmap *current_altmap;

heptagon *build_child(heptspin p, pair<int, int> adj);

struct hrmap_arbi : hrmap {
  heptagon *origin;
  heptagon *getOrigin() override { return origin; }

  hrmap_arbi() {
    dynamicval<hrmap*> curmap(currentmap, this);
    origin = tailored_alloc<heptagon> (current.shapes[0].size());
    origin->s = hsOrigin;
    origin->emeraldval = 0;
    origin->zebraval = 0;
    origin->fiftyval = 0;
    origin->fieldval = 0;
    origin->rval0 = origin->rval1 = 0;
    origin->cdata = NULL;
    origin->alt = NULL;
    origin->c7 = newCell(origin->type, origin);
    origin->distance = 0;

    heptagon *alt = NULL;
    
    /*
    if(hyperbolic) {
      dynamicval<eGeometry> g(geometry, gNormal); 
      alt = tailored_alloc<heptagon> (S7);
      alt->s = hsOrigin;
      alt->emeraldval = 0;
      alt->zebraval = 0;
      alt->distance = 0;
      alt->c7 = NULL;
      alt->alt = alt;
      alt->cdata = NULL;
      current_altmap = newAltMap(alt); 
      }
    */
    
    transmatrix T = xpush(.01241) * spin(1.4117) * xpush(0.1241) * Id;
    arbi_matrix[origin] = make_pair(alt, T);
    altmap[alt].emplace_back(origin, T);
  
    cgi.base_distlimit = 0;
    celllister cl(origin->c7, 1000, 200, NULL);
    ginf[geometry].distlimit[0] = cgi.base_distlimit = cl.dists.back();
    if(sphere) cgi.base_distlimit = SEE_ALL;
    }

  ~hrmap_arbi() {
    /*
    if(hyperbolic) for(auto& p: arbi_matrix) if(p.second.first->cdata) {
      delete p.second.first->cdata;
      p.second.first->cdata = NULL;
      }
    */
    clearfrom(origin);
    altmap.clear();
    arbi_matrix.clear();
    if(current_altmap) {
      dynamicval<eGeometry> g(geometry, gNormal);       
      delete current_altmap;
      current_altmap = NULL;
      }
    }
  void verify() override { }

  transmatrix adj(heptagon *h, int dl) override { 
    auto& c = current;
    int t = id_of(h);
    auto& sh = c.shapes[t];
    
    int dr = gmod(dl+1, sh.size());

    auto& co = sh.connections[dl];
    int xt = get<0>(co);
    int xdl = get<1>(co);
    // int m = get<2>(co);
    auto& xsh = c.shapes[xt];
    int xdr = gmod(xdl+1, xsh.size());

    hyperpoint vl = sh.vertices[dl];
    hyperpoint vr = sh.vertices[dr];
    hyperpoint vm = mid(vl, vr);
        
    transmatrix rm = gpushxto0(vm);
    
    hyperpoint xvl = xsh.vertices[xdl];
    hyperpoint xvr = xsh.vertices[xdr];
    hyperpoint xvm = mid(xvl, xvr);
    
    transmatrix xrm = gpushxto0(xvm);
    
    transmatrix Res = rgpushxto0(vm) * rspintox(rm*vr) * spintox(xrm*xvl) * xrm;
    
    if(hdist(vl, Res*xvr) + hdist(vr, Res*xvl) > .1) {
      println(hlog, "s1 = ", kz(spintox(rm*vr)), " s2 = ", kz(rspintox(xrm*xvr)));    
      println(hlog, tie(t, dl), " = ", kz(Res));    
      println(hlog, hdist(vl, Res * xvr), " # ", hdist(vr, Res * xvl));
      exit(3);
      }
        
    return Res;
    }

  heptagon *create_step(heptagon *h, int d) override {
  
    int t = id_of(h);
  
    const auto& p = arbi_matrix[h];
    
    heptagon *alt = p.first;
    
    auto& sh = current.shapes[t];
    
    auto& co = sh.connections[d];
    
    int xt = get<0>(co);
    int e = get<1>(co);
    int m = get<2>(co);
    
    transmatrix T = p.second * adj(h, d);
    
    if(hyperbolic) {
      dynamicval<eGeometry> g(geometry, gNormal); 
      dynamicval<hrmap*> cm(currentmap, current_altmap);
      // transmatrix U = T;
      current_altmap->virtualRebase(alt, T);
      // U = U * inverse(T);
      }
    
    if(euclid) {
      /* hash the rough coordinates as heptagon* alt */
      size_t s = size_t(T[0][LDIM]+.261) * 124101 + size_t(T[1][LDIM]+.261) * 82143;
      alt = (heptagon*) s;
      }

    for(auto& p2: altmap[alt]) if(eqmatrix(p2.second, T)) {
      h->c.connect(d, p2.first, e, m);
      return p2.first;
      }

    auto h1 = tailored_alloc<heptagon> (current.shapes[xt].size());
    h1->distance = h->distance + 1;
    h1->zebraval = xt;
    h1->c7 = newCell(h1->type, h1);
    h1->alt = nullptr;
    h->c.connect(d, h1, e, m);
    
    arbi_matrix[h1] = make_pair(alt, T);
    altmap[alt].emplace_back(h1, T);    
    return h1;
    }
  
  void draw() override {
    dq::visited.clear();
    dq::enqueue(centerover->master, cview());
    
    while(!dq::drawqueue.empty()) {
      auto& p = dq::drawqueue.front();
      heptagon *h = get<0>(p);
      transmatrix V = get<1>(p);
      dynamicval<ld> b(band_shift, get<2>(p));
      dq::drawqueue.pop();
  
      if(do_draw(h->c7, V)) drawcell(h->c7, V);
  
      for(int i=0; i<h->type; i++) {
        transmatrix V1 = V * adj(h, i);
        bandfixer bf(V1);
        dq::enqueue(h->move(i), V1);
        }
      }
    }

  transmatrix adj(cell *c, int dir) override { return adj(c->master, dir); }
  
  ld spin_angle(cell *c, int d) override { return SPIN_NOT_AVAILABLE; }
  };

EX hrmap *new_map() { return new hrmap_arbi; }

#if CAP_COMMANDLINE  
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-arbi")) {
    PHASEFROM(2);
    stop_game();
    shift(); 
    set_geometry(gArbitrary);
    load(args());
    }
  else return 1;
  return 0;
  }

auto hook = addHook(hooks_args, 100, readArgs);
#endif

EX bool in() { return geometry == gArbitrary; }

EX }
}