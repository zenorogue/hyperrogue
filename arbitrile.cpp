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
  int flags;
  vector<hyperpoint> vertices;
  vector<ld> angles;
  vector<ld> edges;
  vector<tuple<int, int, int>> connections;
  int size() const { return isize(vertices); }
  void build_from_angles_edges();
  };

struct arbi_tiling {

  int order;
  bool have_line, have_ph;

  vector<shape> shapes;
  string name;
  string comment;

  geometryinfo1& get_geometry();
  eGeometryClass get_class() { return get_geometry().kind; }

  ld scale();
  };
#endif

EX arbi_tiling current;


/** id of vertex in the arbitrary tiling */

EX short& id_of(heptagon *h) { return h->zebraval; }

void shape::build_from_angles_edges() {
  transmatrix at = Id;
  vertices.clear();
  int n = isize(angles);
  hyperpoint ctr = Hypc;
  for(int i=0; i<n; i++) {
    println(hlog, "at = ", at);
    vertices.push_back(tC0(at));
    ctr += tC0(at);
    at = at * xpush(edges[i]) * spin(angles[i]);
    }
  if(!eqmatrix(at, Id)) throw hr_parse_exception("polygon error");
  ctr = normalize(ctr);
  for(auto& v: vertices) v = gpushxto0(ctr) * v;
  }

bool correct_index(int index, int size) { return index >= 0 && index < size; }
template<class T> bool correct_index(int index, const T& v) { return correct_index(index, isize(v)); }

template<class T> void verify_index(int index, const T& v) { if(!correct_index(index, v)) throw hr_parse_exception("bad index"); }

string unnamed = "unnamed";

void load(const string& fname) {
  fhstream f(fname, "rt");
  string s;
  while(true) {
    int c = fgetc(f.f);
    if(c < 0) break;
    s += c;
    }
  auto& c = current;
  c.order++;
  c.shapes.clear();
  c.name = unnamed;
  c.comment = "";
  exp_parser ep;
  ep.s = s;
  ld angleunit = 1, distunit = 1, angleofs = 0;
  auto addflag = [&] (int f) {
    int ai;
    if(ep.next() == ')') ai = isize(c.shapes)-1;
    else ai = ep.iparse();
    verify_index(ai, c.shapes); 
    c.shapes[ai].flags |= f;
    ep.force_eat(")");
    };
  while(true) {
    ep.skip_white();
    if(ep.next() == 0) break;
    if(ep.eat("#")) {
      bool doubled = ep.eat("#");
      while(ep.eat(" ")) ;
      string s = "";
      while(ep.next() >= 32) s += ep.next(), ep.at++;
      if(doubled) {
        if(c.name == unnamed) c.name = s;
        else {
          c.comment += s; 
          c.comment += "\n";
          }
        }
      }
    else if(ep.eat("e2.")) {
      ginf[gArbitrary].g = giEuclid2;
      set_flag(ginf[gArbitrary].flags, qBOUNDED, false);
      }
    else if(ep.eat("h2.")) {
      ginf[gArbitrary].g = giHyperb2;
      set_flag(ginf[gArbitrary].flags, qBOUNDED, false);
      }
    else if(ep.eat("s2.")) {
      ginf[gArbitrary].g = giSphere2;
      set_flag(ginf[gArbitrary].flags, qBOUNDED, false);
      }
    else if(ep.eat("angleunit(")) angleunit = real(ep.parsepar());
    else if(ep.eat("angleofs(")) angleofs = real(ep.parsepar());
    else if(ep.eat("distunit(")) distunit = real(ep.parsepar());
    else if(ep.eat("line(")) {
      addflag(arcm::sfLINE);
      c.have_line = true;
      }
    else if(ep.eat("grave(")) {
      addflag(arcm::sfPH);
      c.have_ph = true;
      }
    else if(ep.eat("let(")) {
      string tok = ep.next_token();
      ep.force_eat("=");
      ep.extra_params[tok] =ep.parsepar();
      }
    else if(ep.eat("tile(")) {
      c.shapes.emplace_back();
      auto& cc = c.shapes.back();
      cc.id = isize(c.shapes) - 1;
      cc.flags = 0;
      while(ep.next() != ')') {
        ld dist = ep.rparse(0);
        ep.force_eat(",");
        ld angle = ep.rparse(0);
        cc.edges.push_back(dist * distunit);
        cc.angles.push_back(angle * angleunit + angleofs);
        if(ep.eat(",")) continue;
        else if(ep.eat(")")) break;
        else throw hr_parse_exception("expecting , or )");
        }
      cc.build_from_angles_edges();
      cc.connections.resize(cc.size());
      }
    else if(ep.eat("c(")) {
      int ai = ep.iparse(); verify_index(ai, c.shapes); ep.force_eat(",");
      int as = ep.iparse(); verify_index(as, c.shapes[ai]); ep.force_eat(",");
      int bi = ep.iparse(); verify_index(bi, c.shapes); ep.force_eat(",");
      int bs = ep.iparse(); verify_index(bs, c.shapes[bi]); ep.force_eat(",");
      int m = ep.iparse(); ep.force_eat(")");
      c.shapes[ai].connections[as] = make_tuple(bi, bs, m);
      c.shapes[bi].connections[bs] = make_tuple(ai, as, m);
      }
    else throw hr_parse_exception("expecting command");
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
    h1->cdata = nullptr;
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
    try {
      load(args());
      }
    catch(hr_parse_exception& ex) {
      println(hlog, "failed: ", ex.s);
      exit(3);
      }
    ginf[gArbitrary].tiling_name = current.name;
    }
  else return 1;
  return 0;
  }

auto hook = addHook(hooks_args, 100, readArgs);
#endif

EX bool in() { return geometry == gArbitrary; }

EX string tes = "tessellations/marjorie-rice.tes";

EX bool linespattern(cell *c) {
  return current.shapes[id_of(c->master)].flags & arcm::sfLINE;
  }

EX bool pseudohept(cell *c) {
  return current.shapes[id_of(c->master)].flags & arcm::sfPH;
  }

EX void choose() {
  dialog::openFileDialog(tes, XLAT("open a tiling"), ".tes", 
  [] () {
    stop_game();
    set_geometry(gArbitrary);
    try {
      load(tes);
      ginf[gArbitrary].tiling_name = current.name;
      }
    catch(hr_parse_exception& ex) {
      println(hlog, "failed: ", ex.s);
      set_geometry(gNormal);
      }
    start_game();
    return true;
    });
  }

EX }
}