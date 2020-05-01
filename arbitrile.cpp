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
  vector<pair<int, int> > sublines;
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

struct hr_polygon_error : hr_exception {
  vector<transmatrix> v;
  eGeometryClass c;
  int id;
  map<string, cld> params;
  hr_polygon_error(const vector<transmatrix>& _v, int _id) : v(_v), c(cgclass), id(_id) {}
  ~hr_polygon_error() noexcept(true) {}
  };

void start_debugger(hr_polygon_error& err) {
  stop_game();
  if(err.c != cgclass) {
    if(err.c == gcEuclid) set_geometry(gEuclid);
    if(err.c == gcHyperbolic) set_geometry(gNormal);
    if(err.c == gcSphere) set_geometry(gSphere);
    }

  if(specialland != laCanvas) {   
    canvas_default_wall = waInvisibleFloor;
    patterns::whichCanvas = 'g';
    patterns::canvasback = 0xFFFFFF;
    firstland = specialland = laCanvas;
    }

  start_game();
  drawthemap();

  mapeditor::drawing_tool = true;
  pushScreen(mapeditor::showDrawEditor);
  mapeditor::initdraw(cwt.at);
  
  int n = isize(err.v);
  
  mapeditor::dtcolor = 0xFF0000FF;
  mapeditor::dtwidth = 0.02;
  for(int i=0; i<n-1; i++)
    mapeditor::dt_add_line(tC0(err.v[i]), tC0(err.v[i+1]), 0);
  
  mapeditor::dtcolor = 0xFFFFFFFF;
  for(int i=0; i<n; i++)
    mapeditor::dt_add_text(tC0(err.v[i]), 0.5, its(i));
  }

void shape::build_from_angles_edges() {
  transmatrix at = Id;
  vertices.clear();
  int n = isize(angles);
  hyperpoint ctr = Hypc;
  vector<transmatrix> matrices;
  for(int i=0; i<n; i++) {
    matrices.push_back(at);
    println(hlog, "at = ", at);
    vertices.push_back(tC0(at));
    ctr += tC0(at);
    at = at * xpush(edges[i]) * spin(angles[i]);
    }
  matrices.push_back(at);
  if(!eqmatrix(at, Id)) throw hr_polygon_error(matrices, id);
  if(sqhypot_d(3, ctr) < 1e-2) {
    // this may happen for some spherical tilings
    // try to move towards the center
    println(hlog, "special case encountered");
    for(int i=0; i<n; i++) {
      ctr += at * xpush(edges[i]) * spin((angles[i]+M_PI)/2) * xpush0(.01);
      at = at * xpush(edges[i]) * spin(angles[i]);
      }
    println(hlog, "ctr = ", ctr);
    }
  ctr = normalize(ctr);
  for(auto& v: vertices) v = gpushxto0(ctr) * v;
  }

bool correct_index(int index, int size) { return index >= 0 && index < size; }
template<class T> bool correct_index(int index, const T& v) { return correct_index(index, isize(v)); }

template<class T> void verify_index(int index, const T& v, exp_parser& ep) { if(!correct_index(index, v)) throw hr_parse_exception("bad index: " + its(index) + " at " + ep.where()); }

string unnamed = "unnamed";

EX void load_tile(exp_parser& ep, bool unit) {
  current.shapes.emplace_back();
  auto& cc = current.shapes.back();
  cc.id = isize(current.shapes) - 1;
  cc.flags = 0;
  while(ep.next() != ')') {
    ld dist = 1;
    if(!unit) {
      dist = ep.rparse(0);
      ep.force_eat(",");
      }
    ld angle = ep.rparse(0);
    cc.edges.push_back(real(dist * ep.extra_params["distunit"]));
    cc.angles.push_back(real(angle * ep.extra_params["angleunit"] + ep.extra_params["angleofs"]));
    if(ep.eat(",")) continue;
    else if(ep.eat(")")) break;
    else throw hr_parse_exception("expecting , or )");
    }
  try {
    cc.build_from_angles_edges();
    }
  catch(hr_parse_exception& ex) {
    throw hr_parse_exception(ex.s + ep.where());
    }
  catch(hr_polygon_error& poly) {
    poly.params = ep.extra_params;
    throw;
    }
  cc.connections.resize(cc.size());
  for(int i=0; i<isize(cc.connections); i++)
    cc.connections[i] = make_tuple(cc.id, i, false);
  }

EX void load(const string& fname) {
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
    verify_index(ai, c.shapes, ep); 
    c.shapes[ai].flags |= f;
    ep.force_eat(")");
    };
  while(true) {

    ep.extra_params["distunit"] = distunit;
    ep.extra_params["angleunit"] = angleunit;
    ep.extra_params["angleofs"] = angleofs;

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
      ginf[gArbitrary].sides = 7;
      set_flag(ginf[gArbitrary].flags, qBOUNDED, false);
      }
    else if(ep.eat("h2.")) {
      ginf[gArbitrary].g = giHyperb2;
      ginf[gArbitrary].sides = 7;
      set_flag(ginf[gArbitrary].flags, qBOUNDED, false);
      }
    else if(ep.eat("s2.")) {
      ginf[gArbitrary].g = giSphere2;
      ginf[gArbitrary].sides = 5;
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
      if(debugflags & DF_GEOM)
        println(hlog, "let ", tok, " = ", real(ep.extra_params[tok]));
      }
    else if(ep.eat("unittile(")) load_tile(ep, true);
    else if(ep.eat("tile(")) load_tile(ep, false);
    else if(ep.eat("conway(\"")) {
      string s = "";
      while(true) {
        int m = 0;
        if(ep.eat("(")) m = 0;
        else if(ep.eat("[")) m = 1;
        else if(ep.eat("\"")) break;
        else throw hr_parse_exception("cannot parse Conway notation, " + ep.where());

        int ai = 0;
        int as = ep.iparse();
        while(ep.eat("'")) ai++;
        if(ep.eat("@")) ai = ep.iparse();
        int bi = 0, bs = 0;
        if(ep.eat(")") || ep.eat("]")) bs = as, bi = ai;
        else {
          bs = ep.iparse();
          while(ep.eat("'")) bi++;
          if(ep.eat("@")) bi = ep.iparse();
          }          
        if(ep.eat(")") || ep.eat("]")) {}
        c.shapes[ai].connections[as] = make_tuple(bi, bs, m);
        c.shapes[bi].connections[bs] = make_tuple(ai, as, m);
        }
      ep.force_eat(")");
      }
    else if(ep.eat("c(")) {
      int ai = ep.iparse(); verify_index(ai, c.shapes, ep); ep.force_eat(",");
      int as = ep.iparse(); verify_index(as, c.shapes[ai], ep); ep.force_eat(",");
      int bi = ep.iparse(); verify_index(bi, c.shapes, ep); ep.force_eat(",");
      int bs = ep.iparse(); verify_index(bs, c.shapes[bi], ep); ep.force_eat(",");
      int m = ep.iparse(); ep.force_eat(")");
      c.shapes[ai].connections[as] = make_tuple(bi, bs, m);
      c.shapes[bi].connections[bs] = make_tuple(ai, as, m);
      }
    else if(ep.eat("subline(")) {
      int ai = ep.iparse(); verify_index(ai, c.shapes, ep); ep.force_eat(",");
      int as = ep.iparse(); verify_index(as, c.shapes[ai], ep); ep.force_eat(",");
      int bs = ep.iparse(); verify_index(bs, c.shapes[ai], ep); ep.force_eat(")");
      c.shapes[ai].sublines.emplace_back(as, bs);
      }
    else if(ep.eat("sublines(")) {
      ld d = ep.rparse() * distunit; 
      ld eps = 1e-4;
      if(ep.eat(",")) eps = ep.rparse() * distunit;
      ep.force_eat(")");
      for(auto& sh: c.shapes) {
        for(int i=0; i<isize(sh.vertices); i++)
        for(int j=0; j<isize(sh.vertices); j++)
          if(j != i+1 && i != j+1 && !(i==0 && j == isize(sh.vertices)-1) && !(j==0 && i == isize(sh.vertices)-1) && i != j) {
            ld dist = hdist(sh.vertices[i], sh.vertices[j]);
            if(abs(dist - d) < eps) {
              sh.sublines.emplace_back(i, j);
              println(hlog, "add subline ", i, "-", j);
              }
            }
        }
      }
    else throw hr_parse_exception("expecting command, " + ep.where());
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
    int m = get<2>(co);

    if(h->c.move(dl)) xdl = h->c.spin(dl);

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
    
    transmatrix Res = rgpushxto0(vm) * rspintox(rm*vr);
    if(m) Res = Res * MirrorX;
    Res = Res * spintox(xrm*xvl) * xrm;
    
    if(m) swap(vl, vr);
    
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
    auto& xsh = current.shapes[xt];
    
    transmatrix T = p.second * adj(h, d);
    
    if(hyperbolic) {
      dynamicval<eGeometry> g(geometry, gNormal); 
      dynamicval<hrmap*> cm(currentmap, current_altmap);
      // transmatrix U = T;
      current_altmap->virtualRebase(alt, T);
      // U = U * inverse(T);
      }
    fixmatrix(T);
    
    if(euclid) {
      /* hash the rough coordinates as heptagon* alt */
      size_t s = size_t(T[0][LDIM]+.261) * 124101 + size_t(T[1][LDIM]+.261) * 82143;
      alt = (heptagon*) s;
      }

    for(auto& p2: altmap[alt]) if(id_of(p2.first) == xt && hdist(tC0(p2.second), tC0(T)) < 1e-2) {
      for(int oth=0; oth < p2.first->type; oth++) {
        ld err = hdist(p2.second * xsh.vertices[oth], T * xsh.vertices[e]);
        if(err < 1e-2) {
          static ld max_err = 0;
          if(err > max_err) {
            println(hlog, "err = ", err);
            max_err = err;
            }
          h->c.connect(d, p2.first, oth%p2.first->type, m);
          return p2.first;
          }
        }
      }

    auto h1 = tailored_alloc<heptagon> (current.shapes[xt].size());
    h1->distance = h->distance + 1;
    h1->zebraval = xt;
    h1->c7 = newCell(h1->type, h1);
    h1->alt = nullptr;
    h1->cdata = nullptr;
    h1->emeraldval = h->emeraldval ^ m;
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
      else continue;
  
      for(int i=0; i<h->type; i++) {
        transmatrix V1 = V * adj(h, i);
        bandfixer bf(V1);
        dq::enqueue(h->move(i), V1);
        }
      }
    }

  transmatrix relative_matrix(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
    return relative_matrix_recursive(h2, h1);
    }

  transmatrix adj(cell *c, int dir) override { return adj(c->master, dir); }
  
  ld spin_angle(cell *c, int d) override { return SPIN_NOT_AVAILABLE; }
  };

EX hrmap *new_map() { return new hrmap_arbi; }

void run(string fname) {
  stop_game();
  eGeometry g = geometry;
  arbi_tiling t = current;
  auto v = variation;
  set_geometry(gArbitrary);
  try {
     load(fname);
     ginf[gArbitrary].tiling_name = current.name;
     }
   catch(hr_polygon_error& poly) {
     set_geometry(g);
     set_variation(v);
     current = t;
     start_debugger(poly);
     string help = XLAT("Polygon number %1 did not close correctly. Here is the picture to help you understand the issue.\n\n", its(poly.id));
     showstartmenu = false;
     for(auto& p: poly.params)
       help += lalign(-1, p.first, " = ", p.second, "\n");
     gotoHelp(help);
     }
   catch(hr_parse_exception& ex) {
     println(hlog, "failed: ", ex.s);
     set_geometry(g);
     current = t;
     start_game();
     addMessage("failed: " + ex.s);
     }
  start_game();
  }

#if CAP_COMMANDLINE  
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-arbi")) {
    PHASEFROM(2);
    shift(); 
    run(args());
    }
  else return 1;
  return 0;
  }

auto hook = addHook(hooks_args, 100, readArgs);
#endif

EX bool in() { return geometry == gArbitrary; }

EX string tes = "tessellations/sample/marjorie-rice.tes";

EX bool linespattern(cell *c) {
  return current.shapes[id_of(c->master)].flags & arcm::sfLINE;
  }

EX bool pseudohept(cell *c) {
  return current.shapes[id_of(c->master)].flags & arcm::sfPH;
  }

EX void choose() {
  dialog::openFileDialog(tes, XLAT("open a tiling"), ".tes", 
  [] () {
    run(tes);
    return true;
    });
  }

EX }
}