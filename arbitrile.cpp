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

EX int affine_limit = 200;

#if HDR

/** a type used to specify the connections between shapes */
struct connection_t {
  /** the index of the connected shape in the 'shapes' table */
  int sid;
  /** the index of the edge in the 'shapes' table */
  int eid;
  /** 1 if this connection mirrored, 0 otherwise. do_unmirror() removes all mirrors by doubling shapes */
  int mirror;
  bool operator == (const arb::connection_t& b) const { return tie(sid, eid, mirror) == tie(b.sid, b.eid, b.mirror); }
  bool operator < (const arb::connection_t& b) const { return tie(sid, eid, mirror) < tie(b.sid, b.eid, b.mirror); }
  };

inline void print(hstream& hs, const connection_t& conn) { print(hs, tie(conn.sid, conn.eid, conn.mirror)); }

/** \brief each shape of the arb tessellation
 *  note: the usual HyperRogue convention is: vertex 0, edge 0, vertex 1, edge 1, ... 
 *  note: the tesfile convention is: edge 0, vertex 0, edge 1, vertex 1, ...
 */

/** edge with infinite end on the left */
constexpr ld INFINITE_LEFT = -1;
/** edge with infinite end on the right */
constexpr ld INFINITE_RIGHT = -2;
/** edge with two infinite ends */
constexpr ld INFINITE_BOTH = -3;

struct shape {
  /** index in the arbi_tiling::shapes */
  int id;
  /** index in the original file */
  int orig_id;
  /** flags such as sfLINE and sfPH */
  int flags;
  /** list of vertices in the usual convention */
  vector<hyperpoint> vertices;
  /** list of angles in the tesfile convention */
  vector<ld> angles;
  /** list of edge lengths */
  vector<ld> edges;
  /** list of input edges */
  vector<ld> in_edges;
  /** list of input angles */
  vector<ld> in_angles;
  /** (ultra)ideal markers */
  vector<bool> ideal_markers;
  /** list of edge connections */
  vector<connection_t> connections;
  int size() const { return isize(vertices); }
  void build_from_angles_edges(bool is_comb);
  vector<pair<int, int> > sublines;
  vector<pair<ld, ld>> stretch_shear;
  /** '*inf' was applied to represent an apeirogon/pseudogon */
  bool apeirogonal;
  /** connections repeat `repeat_value` times */
  int repeat_value;
  /** 0 if the no mirror symmetries are declared; otherwise, edge i is the mirror of edge gmod(symmetric_value-i, size()). Make sure symmetric_value != 0, e.g., by adding size() */
  int symmetric_value;
  /** if a tile/edge combination may be connected to edges j1 and j2 of this, j1-j2 must be divisible by cycle_length */
  int cycle_length;
  /** list of valences of vertices in the tesfile convention */
  vector<int> vertex_valence;
  /** list of periods of vertices in the tesfile convention */
  vector<int> vertex_period;
  /** list of angles at vertices in the tesfile convention */
  vector<vector<ld>> vertex_angles;
  /** football types */
  int football_type;
  /** is it a mirrored version of an original tile */
  bool is_mirrored;
  /** auxiliary function for symmetric_value: is the edge index reflectable? */
  bool reflectable(int id) {
    if(!symmetric_value) return false;
    if(apeirogonal && gmod(id, size()) >= size() - 2) return false;
    return true;
    }
  /** reflect a reflectable reflect index */
  int reflect(int id) {
    return gmod(symmetric_value - id, size() - (apeirogonal ? 2 : 0));
    }
  };

struct slider {
  string name;
  ld zero;
  ld current;
  ld min;
  ld max;
  };

struct intslider {
  string name;
  int zero;
  int current;
  int min;
  int max;
  };

struct arbi_tiling {

  int order;
  /* line flags have been marked for tiles */
  bool have_line;
  /* pseudohept flags have been marked for tiles (1), or the tiling is football-colorable (2), or neither (0) */
  int have_ph;
  /* is the tree structure given in the tes file */
  bool have_tree;
  /* is the valence data reliable */
  bool have_valence;
  /* use "star." if the tessellation includs star polygons */
  bool is_star;
  /* use "combinatorial." for combinatorial tessellations; vertex valences computed based on their angles. Currently only rulegen works for combinatorial tessellations */
  bool is_combinatorial;
  /* reserved for future flags */
  bool res0, res1, res2, res3;

  int yendor_backsteps;

  vector<shape> shapes;
  string name;
  string comment;
  
  vector<slider> sliders;
  vector<intslider> intsliders;
  
  ld cscale;
  int range;
  ld floor_scale;
  ld boundary_ratio;
  string filename;
  int mirror_rules;
  
  vector<string> options;

  int min_valence, max_valence;
  bool is_football_colorable;
  bool was_unmirrored;
  bool was_split_for_football;

  geometryinfo1& get_geometry();
  eGeometryClass get_class() { return get_geometry().kind; }

  ld scale();
  };
#endif

/** currently loaded tiling */
EX arbi_tiling current;

/** is the currently displayed map current or slided */
EX bool using_slided;

/** for real-valued sliders, current is the tiling used by the map, while slided is the tiling used for the display */
EX arbi_tiling slided;

EX bool in_slided() { return in() && using_slided; }

EX arbi_tiling& current_or_slided() {
  return using_slided ? slided : current;
  }

/** id of vertex in the arbitrary tiling */

EX short& id_of(heptagon *h) { return h->zebraval; }

#if HDR
struct hr_polygon_error : hr_exception {
  vector<transmatrix> v;
  eGeometryClass c;
  int id;
  transmatrix end;
  map<string, cld> params;
  hr_polygon_error(const vector<transmatrix>& _v, int _id, transmatrix _e) : v(_v), c(cgclass), id(_id), end(_e) {}
  ~hr_polygon_error() noexcept(true) {}
  string generate_error();
  };
#endif

string hr_polygon_error::generate_error() {
  cld dist = (hdist0(tC0(end)) / params["distunit"]);
  bool angle = abs(dist) < 1e-9;
  if(angle) dist = (atan2(end * lxpush0(1)) / params["angleunit"]);
  return
    XLAT("Polygon number %1 did not close correctly (%2 %3). Here is the picture to help you understand the issue.\n\n", its(id), 
      angle ? "angle" : "distance",
      lalign(0, dist)
      );
  }

struct connection_debug_request : hr_exception {
  int id;
  eGeometryClass c;
  connection_debug_request(int i): id(i), c(cgclass) {}
  };

void ensure_geometry(eGeometryClass c) {
  stop_game();
  if(c != cgclass) {
    if(c == gcEuclid) set_geometry(gEuclid);
    if(c == gcHyperbolic) set_geometry(gNormal);
    if(c == gcSphere) set_geometry(gSphere);
    }

  if(specialland != laCanvas) {   
    canvas_default_wall = waInvisibleFloor;
    patterns::whichCanvas = 'g';
    patterns::canvasback = 0xFFFFFF;
    enable_canvas();
    }
  start_game();
  }

void start_poly_debugger(hr_polygon_error& err) {
  #if CAP_EDIT
  ensure_geometry(err.c);

  drawthemap();

  mapeditor::drawing_tool = true;
  pushScreen(mapeditor::showDrawEditor);
  mapeditor::initdraw(cwt.at);
  
  int n = isize(err.v);
  
  mapeditor::dtcolor = 0xFF0000FF;
  mapeditor::dtwidth = 0.02;
  for(int i=0; i<n-1; i++)
    mapeditor::dt_add_line(shiftless(tC0(err.v[i])), shiftless(tC0(err.v[i+1])), 0);
  
  mapeditor::dtcolor = 0xFFFFFFFF;
  for(int i=0; i<n; i++)
    mapeditor::dt_add_text(shiftless(tC0(err.v[i])), 0.5, its(i));
  #endif
  }

void shape::build_from_angles_edges(bool is_comb) {
  transmatrix at = Id;
  int n = isize(in_angles);
  hyperpoint ctr = Hypc;
  vector<transmatrix> matrices;
  for(int i=0; i<n; i++) {
    matrices.push_back(at);
    if(debugflags & DF_GEOM) println(hlog, "at = ", at);
    ctr += tC0(at);
    at = at * lxpush(in_edges[i]) * spin(in_angles[i]+M_PI);
    }
  matrices.push_back(at);
  if(is_comb) return;
  if(!eqmatrix(at, Id) && !apeirogonal) {
    throw hr_polygon_error(matrices, id, at);
    }
  if(sqhypot_d(3, ctr) < 1e-2) {
    // this may happen for some spherical tilings
    // try to move towards the center
    if(debugflags & DF_GEOM) println(hlog, "special case encountered");
    for(int i=0; i<n; i++) {
      ctr += at * lxpush(in_edges[i]) * spin((in_angles[i]+M_PI)/2) * lxpush0(.01);
      at = at * lxpush(in_edges[i]) * spin(in_angles[i]);
      }
    if(debugflags & DF_GEOM) println(hlog, "ctr = ", ctr);
    }
  hyperpoint inf_point;
  if(apeirogonal) {
    transmatrix U = at;
    for(int i=0; i<3; i++) for(int j=0; j<3; j++) U[i][j] -= Id[i][j];
    hyperpoint v;
    ld det = U[0][1] * U[1][0] - U[1][1] * U[0][0];
    v[1] = (U[1][2] * U[0][0] - U[0][2] * U[1][0]) / det;
    v[0] = (U[0][2] * U[1][1] - U[1][2] * U[0][1]) / det;
    v[2] = 1;
    inf_point = v;
    ctr = mid(C0, tC0(at));
    ctr = towards_inf(ctr, inf_point);
    }
  ctr = normalize(ctr);
  vertices.clear();
  angles.clear();
  for(int i=0; i<n; i++) {
    edges.push_back(in_edges[i]);
    if(!ideal_markers[i]) {
      vertices.push_back(tC0(gpushxto0(ctr) * matrices[i]));
      angles.push_back(in_angles[i]);
      }
    else {
      angles.push_back(0);
      hyperpoint a1 = tC0(matrices[i]);
      hyperpoint t1 = get_column(matrices[i], 0);
      hyperpoint a2 = tC0(matrices[i+2]);
      hyperpoint t2 = get_column(matrices[i+2], 0);

      a1 /= a1[2];
      a2 /= a2[2];

      t1 -= a1 * t1[2];
      t2 -= a2 * t2[2];

      ld c1 = a2[0] - a1[0], c2 = a2[1] - a1[1];
      ld v1 = t1[0], v2 = t1[1];
      ld u1 = t2[0], u2 = t2[1];

      ld r = (u2 * c1 - c2 * u1) / (v1 * u2 - v2 * u1);
      // ld s = (v2 * c1 - c2 * v1) / (v1 * u2 - v2 * u1);

      hyperpoint v = a1 + r * t1;
      // also v == a2 + s * t2;
      v[2] = 1;
      v = gpushxto0(ctr) * v;
      v /= v[2];
      vertices.push_back(v);
      i++;
      }
    }
  if(apeirogonal) {
    vertices.push_back(gpushxto0(ctr) * tC0(at));
    hyperpoint v = gpushxto0(ctr) * inf_point;
    v /= v[2];
    vertices.push_back(v);
    auto b = angles.back() / 2;
    angles.back() = b;
    angles.push_back(0);
    angles.push_back(b);
    edges.push_back(0);
    edges.push_back(0);
    }
  n = isize(angles);
  for(int i=0; i<n; i++) {
    bool left = angles[i] == 0;
    bool right = angles[gmod(i-1, isize(vertices))] == 0;
    if(left && right) edges[i] = INFINITE_BOTH;
    else if(left) edges[i] = INFINITE_LEFT;
    else if(right) edges[i] = INFINITE_RIGHT;
    }
  }

EX bool correct_index(int index, int size) { return index >= 0 && index < size; }
template<class T> bool correct_index(int index, const T& v) { return correct_index(index, isize(v)); }

template<class T> void verify_index(int index, const T& v, exp_parser& ep) { if(!correct_index(index, v)) throw hr_parse_exception("bad index: " + its(index) + " at " + ep.where()); }

string unnamed = "unnamed";

EX void load_tile(exp_parser& ep, arbi_tiling& c, bool unit) {
  c.shapes.emplace_back();
  auto& cc = c.shapes.back();
  cc.id = isize(c.shapes) - 1;
  cc.orig_id = cc.id;
  cc.is_mirrored = false;
  cc.symmetric_value = 0;
  cc.flags = 0;
  cc.repeat_value = 1;
  bool is_symmetric = false;
  while(ep.next() != ')') {
    cld dist = 1;
    ep.skip_white();
    if(ep.eat("|")) {
      cc.symmetric_value = ep.iparse();
      is_symmetric = true;
      ep.force_eat(")");
      break;
      }
    if(ep.eat("*")) {
      ld frep = ep.rparse(0);
      if(isinf(frep)) {
        cc.apeirogonal = true;
        set_flag(ginf[gArbitrary].flags, qIDEAL, true);
        if(ep.eat(",") && ep.eat("|")) {
          is_symmetric = true;
          if(isize(cc.in_edges) == 1 && ep.eat(")")) break;
          cc.symmetric_value = ep.iparse();
          }
        ep.force_eat(")");
        break;
        }
      int rep = int(frep+.5);
      int repeat_from = 0;
      int repeat_to = cc.in_edges.size();
      if(rep == 0) {
        cc.in_edges.resize(repeat_from);
        cc.in_angles.resize(repeat_from);
        cc.ideal_markers.resize(repeat_from);
        }
      else if(rep < 0) throw hr_parse_exception("don't know how to use a negative repeat in tile definition");
      for(int i=1; i<rep; i++)
      for(int j=repeat_from; j<repeat_to; j++) {
        cc.in_edges.push_back(cc.in_edges[j]);
        cc.in_angles.push_back(cc.in_angles[j]);
        cc.ideal_markers.push_back(cc.ideal_markers[j]);
        }
      ep.skip_white();
      if(ep.eat(",")) {
        ep.force_eat("|");
        is_symmetric = true;
        if(repeat_to == 1 && ep.eat(")")) goto skip;
        cc.symmetric_value = ep.iparse();
        }
      if(ep.eat(")")) {
        skip:
        if(repeat_from == 0) cc.repeat_value = rep;
        break;
        }
      else throw hr_parse_exception("expecting ) after repeat");
      }
    if(!unit) {
      dist = ep.parse(0);
      ep.force_eat(",");
      }
    cld angle;
    ep.skip_white();
    if(ep.eat("[")) {
      cc.in_edges.push_back(ep.validate_real(dist * ep.extra_params["distunit"]));
      angle = ep.parse(0); ep.force_eat(",");
      cc.in_angles.push_back(ep.validate_real(angle * ep.extra_params["angleunit"]));
      cc.ideal_markers.push_back(true);
      dist = ep.parse(0); ep.force_eat(",");
      angle = ep.parse(0); ep.force_eat("]");
      set_flag(ginf[gArbitrary].flags, qIDEAL, true);
      }
    else
      angle = ep.parse(0);
    cc.in_edges.push_back(ep.validate_real(dist * ep.extra_params["distunit"]));
    cc.in_angles.push_back(ep.validate_real(angle * ep.extra_params["angleunit"]));
    cc.ideal_markers.push_back(false);
    if(ep.eat(",")) continue;
    else if(ep.eat(")")) break;
    else throw hr_parse_exception("expecting , or )");
    }
  try {
    cc.build_from_angles_edges(c.is_combinatorial);
    }
  catch(hr_parse_exception& ex) {
    throw hr_parse_exception(ex.s + ep.where());
    }
  catch(hr_polygon_error& poly) {
    poly.params = ep.extra_params;
    throw;
    }
  int n = cc.size();
  if(is_symmetric && !cc.symmetric_value) cc.symmetric_value += n - (cc.apeirogonal ? 2 : 0);
  cc.connections.resize(n);
  for(int i=0; i<isize(cc.connections); i++)
    cc.connections[i] = connection_t{cc.id, i, false};
  if(cc.apeirogonal) {
    cc.connections[n-2].eid = n-1;
    cc.connections[n-1].eid = n-2;
    }
  cc.stretch_shear.resize(n, make_pair(1, 0));
  }

EX bool do_unmirror = true;

template<class T> void cycle(vector<T>& t) {
  std::rotate(t.begin(), t.begin() + 2, t.end());
  }

/** \brief for tessellations which contain mirror rules, remove them by taking the orientable double cover */
EX void unmirror(arbi_tiling& c) {
  if(cgflags & qAFFINE) return;
  auto& mirror_rules = c.mirror_rules;
  mirror_rules = 0;
  for(auto& s: c.shapes)
    for(auto& t: s.connections)
      if(t.mirror)
        mirror_rules++;
  if(!mirror_rules) return;
  auto& sh = c.shapes;
  int s = isize(sh);
  vector<int> mirrored_id(s, -1);
  for(int i=0; i<s; i++)
    if(!sh[i].symmetric_value) {
      mirrored_id[i] = isize(sh);
      sh.push_back(sh[i]);
      }
  int ss = isize(sh);
  for(int i=0; i<ss; i++) {
    sh[i].id = i;
    if(i >= s) sh[i].is_mirrored = true;
    }
  for(int i=s; i<ss; i++) {
    for(auto& v: sh[i].vertices) 
      v[1] = -v[1];
    reverse(sh[i].edges.begin(), sh[i].edges.end());
    for(auto& e: sh[i].edges) {
      if(e == INFINITE_LEFT) e = INFINITE_RIGHT;
      else if(e == INFINITE_RIGHT) e = INFINITE_LEFT;
      }
    reverse(sh[i].vertices.begin()+1, sh[i].vertices.end());
    reverse(sh[i].angles.begin(), sh[i].angles.end()-1);
    reverse(sh[i].connections.begin(), sh[i].connections.end());
    if(sh[i].apeirogonal) {
      cycle(sh[i].edges);
      cycle(sh[i].vertices);
      if(debugflags & DF_GEOM) println(hlog, "angles before = ", sh[i].angles);
      cycle(sh[i].angles);
      if(debugflags & DF_GEOM) println(hlog, "angles now = ", sh[i].angles);
      cycle(sh[i].connections);
      }
    }

  if(true) for(int i=0; i<ss; i++) {
    for(auto& co: sh[i].connections) {
      bool mirr = co.mirror ^ (i >= s);
      co.mirror = false;
      if(mirr && mirrored_id[co.sid] == -1) {
        if(sh[co.sid].reflectable(co.eid)) {
          co.eid = sh[co.sid].reflect(co.eid);
          }
        }
      else if(mirr) {
        co.sid = mirrored_id[co.sid];
        co.eid = isize(sh[co.sid].angles) - 1 - co.eid;
        if(sh[co.sid].apeirogonal)
          co.eid = gmod(co.eid - 2, isize(sh[co.sid].angles));
        }
      }
    }

  c.was_unmirrored = true;
  }

static void reduce_gcd(int& a, int b) {
  a = abs(gcd(a, b));
  }

EX void mirror_connection(arb::arbi_tiling& ac, connection_t& co) {
  if(co.mirror && ac.shapes[co.sid].reflectable(co.eid)) {
    co.eid = ac.shapes[co.sid].reflect(co.eid);
    co.mirror = !co.mirror;
    }
  }

EX void compute_vertex_valence_prepare(arb::arbi_tiling& ac) {

  int tcl = -1;

  while(true) {

    for(auto& sh: ac.shapes) {
      int i = sh.id;
      int n = isize(sh.vertices);
      
      for(int k=sh.cycle_length; k<n; k++) {
        auto co = sh.connections[k];
        auto co1 = sh.connections[k-sh.cycle_length];
        if(co.sid != co1.sid) {
          println(hlog, "ik = ", tie(i,k), " co=", co, " co1=", co1, " cl=", sh.cycle_length);
          throw hr_parse_exception("connection error #2 in compute_vertex_valence");
          }
        mirror_connection(ac, co);
        mirror_connection(ac, co1);
        reduce_gcd(ac.shapes[co.sid].cycle_length, co.eid - co1.eid);
        }

      for(int k=0; k<n; k++) {
        auto co = sh.connections[k];
        auto co0 = co;
        co = ac.shapes[co.sid].connections[co.eid];
        if(co.sid != i) throw hr_parse_exception("connection error in compute_vertex_valence");
        co.mirror ^= co0.mirror;
        mirror_connection(ac, co);
        reduce_gcd(sh.cycle_length, k-co.eid);
        }
      if(debugflags & DF_GEOM) 
        println(hlog, "tile ", i, " cycle_length = ", sh.cycle_length, " / ", n);
      }
    
    int new_tcl = 0;
    for(auto& sh: ac.shapes) {
      auto& len = sh.cycle_length;
      if(len < 0) len = -len;
      new_tcl += len;
      }

    if(new_tcl == tcl) break;
    tcl = new_tcl;
    }
  }

/** returns true if we need to recompute */
EX bool compute_vertex_valence_flat(arb::arbi_tiling& ac) {
  for(auto& sh: ac.shapes) {
    int n = sh.size();
    int i = sh.id;
    sh.vertex_valence.resize(n);
    sh.vertex_period.resize(n);
    sh.vertex_angles.resize(n);
    for(int k=0; k<n; k++) {
      ld total = 0;
      int qty = 0, pqty = 0;
      connection_t at = {i, k, false};
      connection_t at1 = at;
      vector<ld> anglelist;
      do {
        if(at.sid == at1.sid && (at.eid-at1.eid) % ac.shapes[at.sid].cycle_length == 0) pqty = 0;
        if(qty && pqty == 0 && !total) break;
        ld a = ac.shapes[at.sid].angles[at.eid];
        while(a < 0) a += TAU;
        while(a > TAU) a -= TAU;
        total += a;
        anglelist.push_back(a);
        qty++;
        pqty++;

        at.eid++;
        if(at.eid == isize(ac.shapes[at.sid].angles)) at.eid = 0;

        at = ac.shapes[at.sid].connections[at.eid];
        }
      while(total < TAU - 1e-6);
      if(total == 0) qty = OINF;
      if(total > TAU + 1e-6) throw hr_parse_exception("improper total in compute_stats");
      if(at.sid != i) throw hr_parse_exception("ended at wrong type determining vertex_valence");
      if((at.eid - k) % ac.shapes[i].cycle_length) {
        reduce_gcd(ac.shapes[i].cycle_length, at.eid - k);
        return true;
        }
      sh.vertex_valence[k] = qty;
      sh.vertex_period[k] = pqty;
      sh.vertex_angles[k] = std::move(anglelist);
      }
    if(debugflags & DF_GEOM) 
      println(hlog, "computed vertex_valence of ", i, " as ", ac.shapes[i].vertex_valence);
    }
  return false;
  }

/** returns true if we need to recompute */
EX bool compute_vertex_valence_generic(arb::arbi_tiling& ac) {
  for(auto& sh: ac.shapes) {
    int n = sh.size();
    int i = sh.id;
    sh.vertex_valence.resize(n);
    for(int k=0; k<n; k++) {
      connection_t at = {i, k, false};
      transmatrix T = Id;
      int qty = 0;
      do {
        if(qty && at.sid == i) {
          auto co1 = at;
          bool found = find_connection(T, Id, co1);
          if(found) {
            mirror_connection(ac, co1);
            if((co1.eid - k) % ac.shapes[i].cycle_length) {
              reduce_gcd(ac.shapes[i].cycle_length, co1.eid - k);
              return true;
              }
            break;
            }
          }

        if(at.mirror) {
          if(at.eid == 0) at.eid = isize(ac.shapes[at.sid].angles);
          at.eid--;
          }
        else {
          at.eid++;
          if(at.eid == isize(ac.shapes[at.sid].angles)) at.eid = 0;
          }

        auto at0 = at;
        at = ac.shapes[at.sid].connections[at.eid];
        T = T * get_adj(ac, at0.sid, at0.eid, at.sid, at.eid, at.mirror);
        at.mirror ^= at0.mirror;
        qty++;
        }
      while(qty < OINF);
      sh.vertex_valence[k] = qty;
      }
    if(debugflags & DF_GEOM)
      println(hlog, "computed vertex_valence of ", i, " as ", ac.shapes[i].vertex_valence);
    }
  return false;
  }

EX void compute_vertex_valence(arb::arbi_tiling& ac) {

  for(auto& sh: ac.shapes)
    sh.cycle_length = isize(sh.vertices) / sh.repeat_value;

  bool generic = false;
  
  if(!ac.was_unmirrored) for(auto& sh: ac.shapes) if(sh.symmetric_value) generic = true;
  for(auto& sh: ac.shapes) for(auto& co: sh.connections) if(co.mirror) generic = true;

  if(cgflags & qAFFINE) generic = true;
  if(ac.is_star) generic = true;

  recompute:
  compute_vertex_valence_prepare(ac);

  if(generic ? compute_vertex_valence_generic(ac) : compute_vertex_valence_flat(ac)) goto recompute;
  ac.have_valence = true;

  ac.min_valence = UNKNOWN; ac.max_valence = 0;
  for(auto& sh: ac.shapes) 
    for(auto& val: sh.vertex_valence) {
      if(val < ac.min_valence) ac.min_valence = val;
      if(val > ac.max_valence) ac.max_valence = val;
      }      
  }

EX bool extended_football = true;

EX void check_football_colorability(arbi_tiling& c) {
  if(!c.have_valence) return;
  for(auto&sh: c.shapes) for(auto v: sh.vertex_valence)
    if(v % 3) return;

  for(int i=0; i<3; i++) {
    for(auto&sh: c.shapes) sh.football_type = 3;

    vector<int> aqueue;

    c.shapes[0].football_type = i;
    aqueue = {0};
    bool bad = false;
    for(int qi=0; qi<isize(aqueue); qi++) {
      int sid = aqueue[qi];

      auto& sh = c.shapes[sid];

      for(int j=0; j<sh.size(); j++) {
        auto &co = sh.connections[j];
        auto t = sh.football_type;
        if(c.have_ph && ((sh.flags & arcm::sfPH) != (t==2))) bad = true;
        if(sh.apeirogonal && t < 2 && (isize(sh) & 1)) bad = true;

        auto assign = [&] (int tt) {
          auto& t1 = c.shapes[co.sid].football_type;
          if(t1 == 3) {
            t1 = tt;
            aqueue.push_back(co.sid);
            }
          else {
            if(t1 != tt) bad = true;
            }
          };

        if(t < 2) {
          if((j & 1) == t) assign(2); else assign((co.eid & 1) ? 0 : 1);
          }
        else {
          assign((co.eid & 1) ? 1 : 0);
          }
        }
      }
    if(!bad) {
      c.have_ph = 2;
      for(auto& sh: c.shapes) if(sh.football_type == 2) sh.flags |= arcm::sfPH;
      return;
      }
    }

  if(extended_football && !c.have_tree) {
    for(auto&sh: c.shapes)
      sh.football_type = 0;

    for(int i=0; i<3*isize(c.shapes); i++) {
      for(auto&sh: c.shapes) {
        int &res = sh.football_type;
        int siz = sh.size();
        if(sh.apeirogonal) siz -= 2;
        else if(siz & 1) res |= 3;
        if((sh.cycle_length & 1) && !sh.apeirogonal) {
          if(res & 3) res |= 3;
          }
        if(sh.apeirogonal && (siz & 1)) {
          if(res & 3) res |= 3;
          }
        if(sh.flags & arcm::sfPH) res |= 3;
        for(int i=0; i<sh.size(); i++) {
          auto co = sh.connections[i];
          co.eid %= c.shapes[co.sid].cycle_length;
          if(res & 1) {
            if(i&1) {
              if(co.eid & 1)
                c.shapes[co.sid].football_type |= 1;
              else
                c.shapes[co.sid].football_type |= 2;
              }
            else
              c.shapes[co.sid].football_type |= 4;
            }
          if(res & 2) {
            if(!(i&1)) {
              if(co.eid & 1)
                c.shapes[co.sid].football_type |= 1;
              else
                c.shapes[co.sid].football_type |= 2;
              }
            else
              c.shapes[co.sid].football_type |= 4;
            }
          if(res & 4) {
            if(co.eid & 1)
              c.shapes[co.sid].football_type |= 2;
            else
              c.shapes[co.sid].football_type |= 1;
            }
          }
        }
      }

    c.is_football_colorable = true;
    c.was_split_for_football = true;
    for(auto&sh: c.shapes)
     if(sh.football_type == 7)
       c.is_football_colorable = false;

    if(c.is_football_colorable) {
      vector<array<int, 3> > new_indices(isize(c.shapes), make_array(-1, -1, -1));
      auto oldshapes = c.shapes;
      c.shapes.clear();
      for(int i=0; i<isize(oldshapes); i++)
        for(int t=0; t<3; t++)
          if(!(oldshapes[i].football_type & (1<<t))) {
            if(t == 1 && (oldshapes[i].cycle_length & 1) && !oldshapes[i].apeirogonal) continue;
            new_indices[i][t] = isize(c.shapes);
            c.shapes.push_back(oldshapes[i]);
            c.shapes.back().football_type = t;
            if(t == 2) c.shapes.back().flags |= arcm::sfPH;
            }

      for(int i=0; i<isize(oldshapes); i++)
        for(int t=0; t<3; t++) {
          int ni = new_indices[i][t];
          if(ni == -1) continue;
          auto& sh = c.shapes[ni];
          sh.id = ni;
          for(int j=0; j<isize(sh); j++) {
            auto &co = sh.connections[j];
            auto assign = [&] (int tt) {
              auto ni1 = new_indices[co.sid][tt];
              if(ni1 == -1 && tt == 1) {
                ni1 = new_indices[co.sid][0];
                co.eid += oldshapes[co.sid].cycle_length;
                co.eid %= isize(oldshapes[co.sid]);
                }
              co.sid = ni1;
              };

            if(sh.apeirogonal && j >= isize(sh)-2) {
              co.sid = ni;
              if(t < 2 && (isize(sh) & 1)) co.sid = new_indices[i][t^1];
              continue;
              }

            co.eid %= oldshapes[co.sid].cycle_length;
            if(t < 2) {
              if((j & 1) == t) assign(2); else assign((co.eid & 1) ? 0 : 1);
              }
            else {
              assign((co.eid & 1) ? 1 : 0);
              }
            }

          if((sh.cycle_length&1) && (t < 2) && !sh.apeirogonal) sh.cycle_length *= 2;
          if(debugflags & DF_GEOM)
            println(hlog, tie(i,t), " becomes ", ni, " with connections ", sh.connections, " and cycle length = ", sh.cycle_length);
          }

      c.have_ph = 2;
      return;
      }
    }

  for(auto&sh: c.shapes) sh.football_type = 3;
  }

EX void add_connection_sub(arbi_tiling& c, int ai, int as, int bi, int bs, int m) {
  int as0 = as, bs0 = bs;
  auto& ash = c.shapes[ai];
  auto& bsh = c.shapes[bi];
  do {
    ash.connections[as] = connection_t{bi, bs, m};
    as = gmod(as + ash.size() / ash.repeat_value, ash.size());
    }
  while(as != as0);
  do {
    c.shapes[bi].connections[bs] = connection_t{ai, as, m};
    bs = gmod(bs + bsh.size() / bsh.repeat_value, bsh.size());
    }
  while(bs != bs0);
  }

EX void add_connection(arbi_tiling& c, int ai, int as, int bi, int bs, int m) {
  auto& ash = c.shapes[ai];
  auto& bsh = c.shapes[bi];
  add_connection_sub(c, ai, as, bi, bs, m);
  int as1, bs1;
  if(ash.symmetric_value) {
    as1 = ash.reflect(as);
    add_connection_sub(c, ai, as1, bi, bs, !m);
    }
  if(bsh.symmetric_value) {
    bs1 = bsh.reflect(bs);
    add_connection_sub(c, ai, as, bi, bs1, !m);
    }
  if(ash.symmetric_value && bsh.symmetric_value)
    add_connection_sub(c, ai, as1, bi, bs1, m);
  }

EX void set_defaults(arb::arbi_tiling& c, bool keep_sliders, string fname) {
  c.order++;
  c.name = unnamed;
  c.comment = "";
  c.filename = fname;
  c.cscale = 1;
  c.range = 0;
  c.boundary_ratio = 1;
  c.floor_scale = .5;
  c.have_ph = 0;
  c.have_line = false;
  c.is_football_colorable = false;
  c.have_tree = false;
  c.have_valence = false;
  c.yendor_backsteps = 0;
  c.is_star = false;
  c.is_combinatorial = false;
  c.was_unmirrored = false;
  c.was_split_for_football = false;
  c.shapes.clear();
  if(!keep_sliders) {
    c.sliders.clear();
    c.intsliders.clear();
    }
  }

EX void load(const string& fname, bool load_as_slided IS(false), bool keep_sliders IS(false)) {
  fhstream f(fname, "rt");
  if(!f.f) throw hr_parse_exception("file " + fname + " does not exist");
  string s;
  while(true) {
    int c = fgetc(f.f);
    if(c < 0) break;
    s += c;
    }
  auto& c = load_as_slided ? slided : current;
  set_defaults(c, keep_sliders, fname);
  int qsliders = 0, qintsliders = 0;
  exp_parser ep;
  ep.s = s;
  ld angleunit = 1, distunit = 1;
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
    else if(ep.eat("c2(")) {
      ld curv = ep.rparse(0);
      ep.force_eat(")");
      ginf[gArbitrary].g = curv > 0 ? giSphere2 : curv < 0 ? giHyperb2 : giEuclid2;
      ginf[gArbitrary].sides = 7;
      set_flag(ginf[gArbitrary].flags, qCLOSED, curv > 0);
      set_flag(ginf[gArbitrary].flags, qAFFINE, false);
      geom3::apply_always3();
      }
    else if(ep.eat("e2.")) {
      ginf[gArbitrary].g = giEuclid2;
      ginf[gArbitrary].sides = 7;
      set_flag(ginf[gArbitrary].flags, qCLOSED, false);
      set_flag(ginf[gArbitrary].flags, qAFFINE, false);
      geom3::apply_always3();
      }
    else if(ep.eat("a2.")) {
      ginf[gArbitrary].g = giEuclid2;
      ginf[gArbitrary].sides = 7;
      set_flag(ginf[gArbitrary].flags, qCLOSED, false);
      set_flag(ginf[gArbitrary].flags, qAFFINE, true);
      affine_limit = 200;
      geom3::apply_always3();
      }
    else if(ep.eat("h2.")) {
      ginf[gArbitrary].g = giHyperb2;
      ginf[gArbitrary].sides = 7;
      set_flag(ginf[gArbitrary].flags, qCLOSED, false);
      set_flag(ginf[gArbitrary].flags, qAFFINE, false);
      geom3::apply_always3();
      }
    else if(ep.eat("s2.")) {
      ginf[gArbitrary].g = giSphere2;
      ginf[gArbitrary].sides = 5;
      set_flag(ginf[gArbitrary].flags, qCLOSED, true);
      set_flag(ginf[gArbitrary].flags, qAFFINE, false);
      geom3::apply_always3();
      }
    else if(ep.eat("star.")) {
      c.is_star = true;
      }
    else if(ep.eat("combinatorial.")) {
      c.is_combinatorial = true;
      }
    else if(ep.eat("option(\"")) {
      next:
      string s = "";
      while(ep.next() != '"') s += ep.eatchar();
      ep.force_eat("\"");
      c.options.push_back(s);
      ep.skip_white();
      if(ep.eat(",")) { ep.skip_white(); ep.force_eat("\""); goto next; }
      ep.force_eat(")");
      }
    else if(ep.eat("angleunit(")) angleunit = real(ep.parsepar());
    else if(ep.eat("distunit(")) distunit = real(ep.parsepar());
    else if(ep.eat("line(")) {
      addflag(arcm::sfLINE);
      c.have_line = true;
      }
    else if(ep.eat("grave(")) {
      addflag(arcm::sfPH);
      c.have_ph = true;
      }
    else if(ep.eat("slider(")) {
      slider sl;
      sl.name = ep.next_token();
      ep.force_eat(",");
      sl.current = sl.zero = ep.rparse();
      ep.force_eat(",");
      sl.min = ep.rparse();
      ep.force_eat(",");
      sl.max = ep.rparse();
      ep.force_eat(")");
      if(load_as_slided || !keep_sliders)
        c.sliders.push_back(sl);
      if(load_as_slided || keep_sliders)
        ep.extra_params[sl.name] = current.sliders[qsliders++].current;
      else
        ep.extra_params[sl.name] = sl.zero;
      }
    else if(ep.eat("intslider(")) {
      intslider sl;
      sl.name = ep.next_token();
      ep.force_eat(",");
      sl.current = sl.zero = ep.iparse();
      ep.force_eat(",");
      sl.min = ep.iparse();
      ep.force_eat(",");
      sl.max = ep.iparse();
      ep.force_eat(")");
      if(load_as_slided || !keep_sliders)
        c.intsliders.push_back(sl);
      if(load_as_slided || keep_sliders)
        ep.extra_params[sl.name] = current.intsliders[qintsliders++].current;
      else
        ep.extra_params[sl.name] = sl.zero;
      }
    else if(ep.eat("let(")) {
      string tok = ep.next_token();
      ep.force_eat("=");
      ep.extra_params[tok] =ep.parsepar();
      if(debugflags & DF_GEOM)
        println(hlog, "let ", tok, " = ", ep.extra_params[tok]);
      }
    else if(ep.eat("unittile(")) load_tile(ep, c, true);
    else if(ep.eat("tile(")) load_tile(ep, c, false);
    else if(ep.eat("affine_limit(")) {
      affine_limit = ep.iparse();
      ep.force_eat(")");
      }
    else if(ep.eat("cscale(")) {
      c.cscale = ep.rparse();
      ep.force_eat(")");
      }
    else if(ep.eat("treestate(")) {
      rulegen::parse_treestate(c, ep);
      }
    else if(ep.eat("first_treestate(")) {
      rulegen::rule_root = ep.iparse();
      ep.force_eat(")");
      }
    else if(ep.eat("yendor_backsteps(")) {
      c.yendor_backsteps = ep.iparse();
      ep.force_eat(")");
      }
    else if(ep.eat("range(")) {
      c.range = ep.iparse();
      ep.force_eat(")");
      }
    else if(ep.eat("floor_scale(")) {
      c.floor_scale = ep.rparse();
      ep.force_eat(")");
      }
    else if(ep.eat("boundary_ratio(")) {
      c.boundary_ratio = ep.rparse();
      ep.force_eat(")");
      }
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
        verify_index(ai, c.shapes, ep);
        verify_index(as, c.shapes[ai], ep);
        verify_index(bi, c.shapes, ep);
        verify_index(bs, c.shapes[bi], ep);
        add_connection(c, ai, as, bi, bs, m);
        }
      ep.force_eat(")");
      }
    else if(ep.eat("c(")) {
      int ai = ep.iparse(); verify_index(ai, c.shapes, ep); ep.force_eat(",");
      int as = ep.iparse(); verify_index(as, c.shapes[ai], ep); ep.force_eat(",");
      int bi = ep.iparse(); verify_index(bi, c.shapes, ep); ep.force_eat(",");
      int bs = ep.iparse(); verify_index(bs, c.shapes[bi], ep); ep.force_eat(",");
      int m = ep.iparse(); ep.force_eat(")");
      add_connection(c, ai, as, bi, bs, m);
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
        for(int j=0; j<i; j++)
          if(j != i+1 && i != j+1 && !(i==0 && j == isize(sh.vertices)-1) && !(j==0 && i == isize(sh.vertices)-1) && i != j) {
            ld dist = hdist(sh.vertices[i], sh.vertices[j]);
            if(abs(dist - d) < eps) {
              sh.sublines.emplace_back(i, j);
              if(debugflags & DF_GEOM) println(hlog, "add subline ", i, "-", j);
              }
            }
        }
      }
    else if(ep.eat("repeat(")) {
      int i = ep.iparse(0);
      verify_index(i, c.shapes, ep);
      ep.force_eat(",");
      int rep = ep.iparse(0);
      ep.force_eat(")");
      auto& sh = c.shapes[i];
      int N = isize(sh.angles);
      if(N % rep) 
        throw hr_parse_exception("repeat value should be a factor of the number of vertices, " + ep.where());
      sh.repeat_value = rep;
      
      int d = N / rep;
      for(int i=d; i<N; i++)
        sh.connections[i] = sh.connections[i-d];
      }
    else if(ep.eat("debug(")) {
      int i = ep.iparse(0);
      verify_index(i, c.shapes, ep);
      ep.force_eat(")");
      throw connection_debug_request(i);
      }
    else if(ep.eat("stretch_shear(")) {
      ld stretch = ep.rparse(0);
      ep.force_eat(",");
      ld shear = ep.rparse(0);
      ep.force_eat(",");
      int i = ep.iparse(0);
      verify_index(i, c.shapes, ep);
      ep.force_eat(",");
      int j = ep.iparse(0);
      verify_index(j, c.shapes[i], ep);
      ep.force_eat(")");
      auto& sh = c.shapes[i];
      sh.stretch_shear[j] = {stretch, shear};
      auto& co = sh.connections[j];
      auto& xsh = c.shapes[co.sid];
      ld scale = sh.edges[j] / xsh.edges[co.eid];
      println(hlog, "scale = ", scale);
      xsh.stretch_shear[co.eid] = {1/stretch, shear * (co.mirror ? 1 : -1) * stretch };
      }
    else throw hr_parse_exception("expecting command, " + ep.where());
    }
  if(!(cgflags & qAFFINE)) {
    for(int i=0; i<isize(c.shapes); i++) {
      auto& sh = c.shapes[i];
      for(int j=0; j<isize(sh.edges); j++) {
        ld d1 = sh.edges[j];
        auto con = sh.connections[j];
        auto& xsh = c.shapes[con.sid];
        ld d2 = xsh.edges[con.eid];
        if(d1 == INFINITE_LEFT) d1 = INFINITE_RIGHT;
        else if(d1 == INFINITE_RIGHT) d1 = INFINITE_LEFT;
        if(abs(d1 - d2) > 1e-6)
          throw hr_parse_exception(lalign(0, "connecting ", make_pair(i,j), " to ", con, " of different lengths only possible in a2"));
        }
      }
    }

  if(do_unmirror) {
    unmirror(c);
    }
  if(!c.have_tree) compute_vertex_valence(c);

  check_football_colorability(c);

  if(c.have_tree) rulegen::verify_parsed_treestates(c);
  
  if(!load_as_slided) slided = current;
  }

arbi_tiling debugged;
vector<pair<transmatrix, int> > debug_polys;

string primes(int i) {
  string res;
  while(i--) res += "'";
  return res;
  }

void connection_debugger() {
  cmode = sm::SIDE | sm::DIALOG_STRICT_X;
  gamescreen();
  
  auto& last = debug_polys.back();
  
  initquickqueue();
  for(auto& p: debug_polys) {
    int id = p.second;
    
    shiftmatrix V = gmatrix[cwt.at] * p.first;
    
    auto& sh = debugged.shapes[id].vertices;
    
    for(auto& v: sh)
      curvepoint(v);

    curvepoint(sh[0]);
    
    color_t col = colortables['A'][id];
    col = darkena(col, 0, 0xFF);
    
    if(&p == &last) {
      vid.linewidth *= 2;
      queuecurve(V, 0xFFFF00FF, col, PPR::LINE);
      vid.linewidth /= 2;
      for(int i=0; i<isize(sh); i++)
        queuestr(V * sh[i], vid.fsize, its(i), 0xFFFFFFFF);
      }
    else
      queuecurve(V, 0xFFFFFFFF, col, PPR::LINE);
    }
  quickqueue();

  dialog::init(XLAT("connection debugger"));
  
  dialog::addInfo(debugged.name);
  dialog::addHelp(debugged.comment);
  
  dialog::addBreak(50);

  dialog::addInfo("face index " + its(last.second));

  dialog::addBreak(50);  
  
  auto& sh = debugged.shapes[last.second];
  int N = isize(sh.edges);
  for(int k=0; k<N; k++) {
    auto con = sh.connections[k];
    string cap = its(k) + primes(last.second) + " -> " + its(con.eid) + primes(con.sid) + (con.mirror ? " (m) " : "");
    dialog::addSelItem(cap, "go", '0' + k);
    
    dialog::add_action([k, last, con] {
      if(euclid) cgflags |= qAFFINE;
      debug_polys.emplace_back(last.first * get_adj(debugged, last.second, k), con.sid);
      if(euclid) cgflags &= ~qAFFINE;
      });
    
    }    

  dialog::addItem("undo", 'u');
  dialog::add_action([] {
    if(isize(debug_polys) > 1)
      debug_polys.pop_back();
    });
  
  dialog::addBack();
  dialog::display();

  keyhandler = [] (int sym, int uni) {
    handlePanning(sym, uni);
    dialog::handleNavigation(sym, uni);
    if(doexiton(sym, uni)) popScreen();
    };
  }

geometryinfo1& arbi_tiling::get_geometry() {
  return ginf[gEuclid].g;
  }

map<heptagon*, vector<pair<heptagon*, transmatrix> > > altmap;

EX map<heptagon*, pair<heptagon*, transmatrix>> arbi_matrix;

EX hrmap *current_altmap;

heptagon *build_child(heptspin p, pair<int, int> adj);

/** get the midedge of lr; it takes infinite vertices into account */
EX hyperpoint get_midedge(ld len, const hyperpoint &l, const hyperpoint &r) {
  if(len == INFINITE_BOTH) {
    return normalize(closest_to_zero(l, r));
    }
  else if(len == INFINITE_RIGHT) {
    return towards_inf(r, l);
    }
  else if(len == INFINITE_LEFT) {
    return towards_inf(l, r);
    }
  else return mid(l, r);
  }

EX bool is_apeirogonal(cell *c) {
  if(!in()) return false;
  return current_or_slided().shapes[id_of(c->master)].apeirogonal;
  }

EX bool is_apeirogonal() {
  if(!in()) return false;
  for(auto& sh: current_or_slided().shapes)
    if(sh.apeirogonal) return true;
  return false;
  }

EX bool apeirogon_consistent_coloring = true;
EX bool apeirogon_hide_grid_edges = true;
EX bool apeirogon_simplified_display = false;

/** get the adj matrix corresponding to the connection of (t,dl) to connection_t{t1, xdl, xmirror} */
EX transmatrix get_adj(arbi_tiling& c, int t, int dl, int t1, int xdl, bool xmirror) {

  auto& sh = c.shapes[t];
  
  int dr = gmod(dl+1, sh.size());

  auto& xsh = c.shapes[t1];
  int xdr = gmod(xdl+1, xsh.size());

  hyperpoint vl = sh.vertices[dl];
  hyperpoint vr = sh.vertices[dr];
  hyperpoint xvl = xsh.vertices[xdl];
  hyperpoint xvr = xsh.vertices[xdr];

  bool emb = embedded_plane;
  if(emb) {
    vl = cgi.emb->actual_to_base(vl);
    vr = cgi.emb->actual_to_base(vr);
    xvl = cgi.emb->actual_to_base(xvl);
    xvr = cgi.emb->actual_to_base(xvr);
    geom3::light_flip(true);
    }

  hyperpoint vm = get_midedge(sh.edges[dl], vl, vr);
      
  transmatrix rm = gpushxto0(vm);
  
  hyperpoint xvm = get_midedge(xsh.edges[xdl], xvl, xvr);
  
  transmatrix xrm = gpushxto0(xvm);
  
  transmatrix Res = rgpushxto0(vm) * rspintox(rm*vr);
    
  if(cgflags & qAFFINE) {
    ld sca = hdist(vl, vr) / hdist(xvl, xvr);
    transmatrix Tsca = Id;
    Tsca[0][0] = Tsca[1][1] = sca;

    auto& ss = sh.stretch_shear[dl];
    Tsca[0][1] = ss.first * ss.second * sca;
    Tsca[1][1] *= ss.first;
    
    Res = Res * Tsca;
    }

  if(xmirror) Res = Res * MirrorX;
  Res = Res * spintox(xrm*xvl) * xrm;
  
  if(xmirror) swap(vl, vr);
  
  if(hdist(vl, Res*xvr) + hdist(vr, Res*xvl) > .1 && !c.is_combinatorial) {
    println(hlog, "s1 = ", kz(spintox(rm*vr)), " s2 = ", kz(rspintox(xrm*xvr)));    
    println(hlog, tie(t, dl), " = ", kz(Res));    
    println(hlog, hdist(vl, Res * xvr), " # ", hdist(vr, Res * xvl));
    throw hr_exception("error in arb::get_adj");
    }

  if(emb) {
    Res = cgi.emb->base_to_actual(Res);
    geom3::light_flip(false);
    }

  return Res;
  }

/** get the adj matrix corresponding to the connection of (t,dl) -- note: it may be incorrect for rotated/symmetric connections */
EX transmatrix get_adj(arbi_tiling& c, int t, int dl) {
  auto& sh = c.shapes[t];
  auto& co = sh.connections[dl];
  return get_adj(c, t, dl, co.sid, co.eid, co.mirror);
  }

/** Returns if F describes the same tile as T, taking possible symmetries into account. Paramater co is the expected edge (co.sid tells us the tile type); if yes, co may be adjusted */
EX bool find_connection(const transmatrix& T, const transmatrix& F, connection_t& co) {

  if(!same_point_may_warn(tC0(F), tC0(T))) return false;

  auto& xsh = current.shapes[co.sid];
  int n = isize(xsh.connections);
  for(int oth = 0; oth < n; oth++) {
    int oth1 = gmod(oth+1, n);
    int eid1 = gmod(co.eid+1, n);
    if(same_point_may_warn(F * xsh.vertices[oth], T * xsh.vertices[co.eid]) && same_point_may_warn(F * xsh.vertices[oth1], T * xsh.vertices[eid1])) {
      co.eid = oth;
      return true;
      }
    if(same_point_may_warn(F * xsh.vertices[oth], T * xsh.vertices[eid1]) && same_point_may_warn(F * xsh.vertices[oth1], T * xsh.vertices[co.eid])) {
      co.eid = oth; co.mirror = !co.mirror;
      return true;
      }
    }
  return false;
  }

struct hrmap_arbi : hrmap {
  heptagon *origin;
  heptagon *getOrigin() override { return origin; }

  hrmap_arbi() {
    dynamicval<hrmap*> curmap(currentmap, this);
    origin = init_heptagon(current.shapes[0].size());
    origin->s = hsOrigin;
    origin->c7 = newCell(origin->type, origin);

    heptagon *alt = NULL;
    
    if(mhyperbolic) {
      dynamicval<eGeometry> g(geometry, gNormal); 
      alt = init_heptagon(S7);
      alt->s = hsOrigin;
      alt->alt = alt;
      current_altmap = newAltMap(alt); 
      }
    
    transmatrix T = lxpush(.01241) * spin(1.4117) * lxpush(0.1241) * Id;
    arbi_matrix[origin] = make_pair(alt, T);
    altmap[alt].emplace_back(origin, T);
    
    if(!current.range)
      current.range = auto_compute_range(origin->c7);
    }

  ~hrmap_arbi() {
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
    if(h->c.move(dl))
      return get_adj(current_or_slided(), id_of(h), dl, id_of(h->c.move(dl)), h->c.spin(dl), h->c.mirror(dl));
    else
      return get_adj(current_or_slided(), id_of(h), dl);
    }

  heptagon *create_step(heptagon *h, int d) override {
  
    if(geom3::flipped) return geom3::in_not_flipped([&] { return create_step(h, d); });
    dynamicval<bool> sl(using_slided, false);
    int t = id_of(h);
  
    auto& sh = current.shapes[t];
    
    auto& co = sh.connections[d];
    
    if(cgflags & qAFFINE) {
      set<heptagon*> visited;
      
      vector<pair<heptagon*, transmatrix> > v;
      
      visited.insert(h);
      v.emplace_back(h, Id);
      
      transmatrix goal = adj(h, d);
      
      for(int i=0; i<affine_limit && i < isize(v); i++) {
        transmatrix T = v[i].second;
        heptagon *h2 = v[i].first;
        if(eqmatrix(T, goal)) {
          h->c.connect(d, h2, co.eid, co.mirror);
          return h2;
          }
        for(int i=0; i<h2->type; i++) {
          heptagon *h3 = h2->move(i);
          if(!h3) continue;
          if(visited.count(h3)) continue;
          visited.insert(h3);
          v.emplace_back(h3, T * adj(h2, i));
          }
        }
      
      auto h1 = init_heptagon(current.shapes[co.sid].size());
      h1->distance = h->distance + 1;
      h1->zebraval = co.sid;
      h1->c7 = newCell(h1->type, h1);
      h1->emeraldval = h->emeraldval ^ co.mirror;
      h->c.connect(d, h1, co.eid, co.mirror);
      
      return h1;
      }

    const auto& p = arbi_matrix[h];
    
    heptagon *alt = p.first;
    
    transmatrix T = p.second * adj(h, d);
    
    if(mhyperbolic) {
      dynamicval<eGeometry> g(geometry, gNormal); 
      dynamicval<hrmap*> cm(currentmap, current_altmap);
      // transmatrix U = T;
      current_altmap->virtualRebase(alt, T);
      // U = U * inverse(T);
      }
    fixmatrix(T);

    if(meuclid) {
      /* hash the rough coordinates as heptagon* alt */
      size_t s = size_t(T[0][LDIM]+.261) * 124101 + size_t(T[1][LDIM]+.261) * 82143;
      alt = (heptagon*) s;
      }

    for(auto& p2: altmap[alt]) if(id_of(p2.first) == co.sid) {
      connection_t co1 = co;
      if(find_connection(T, p2.second, co1)) {
        if(p2.first->move(co1.eid)) {
          throw hr_exception("already connected!");
          }
        h->c.connect(d, p2.first, co1.eid, co1.mirror);
        return p2.first;
        }
      }

    auto h1 = init_heptagon(current.shapes[co.sid].size());
    h1->distance = h->distance + 1;
    h1->zebraval = co.sid;
    h1->c7 = newCell(h1->type, h1);
    h1->emeraldval = h->emeraldval ^ co.mirror;
    h->c.connect(d, h1, co.eid, co.mirror);
    
    arbi_matrix[h1] = make_pair(alt, T);
    altmap[alt].emplace_back(h1, T);    
    return h1;
    }
  
  transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
    return relative_matrix_recursive(h2, h1);
    }

  transmatrix adj(cell *c, int dir) override { return adj(c->master, dir); }
  
  ld spin_angle(cell *c, int d) override { return SPIN_NOT_AVAILABLE; }

  int shvid(cell *c) override {
    return id_of(c->master);
    }

  hyperpoint get_corner(cell *c, int cid, ld cf) override {
    auto& sh = arb::current_or_slided().shapes[arb::id_of(c->master)];
    int id = gmod(cid, c->type);
    if(sh.angles[gmod(id-1, c->type)] <= 0)
      return sh.vertices[id];
    return normalize(C0 + (sh.vertices[id] - C0) * 3 / cf);
    }

  };

EX hrmap *new_map() { return new hrmap_arbi; }

EX void run(string fname) {
  stop_game();
  eGeometry g = geometry;
  arbi_tiling t = current;
  auto v = variation;
  set_geometry(gArbitrary);
  try {
     load(fname);
     ginf[gArbitrary].tiling_name = current.name;
     tes = fname;
     }
   catch(hr_polygon_error& poly) {
     set_geometry(g);
     set_variation(v);
     current = t;
     start_poly_debugger(poly);     
     string help = poly.generate_error();
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
  catch(connection_debug_request& cr) {
    set_geometry(g);     
    debugged = current;
    current = t;
    ensure_geometry(cr.c);
    debug_polys.clear();
    debug_polys.emplace_back(Id, cr.id);
    pushScreen(connection_debugger);
    }
  start_game();
  }

string slider_error;

EX void sliders_changed(bool need_restart, bool need_start) {
  if(need_restart) stop_game();
  auto& c = current_or_slided();
  arbi_tiling backup = c;
  try {
    load(current.filename, !need_restart, need_restart);
    using_slided = !need_restart;
    slider_error = "OK";
    #if CAP_TEXTURE
    texture::config.remap();
    #endif
    }
  catch(hr_parse_exception& ex) {
    c = backup;
    slider_error = ex.s;
    }
  catch(hr_polygon_error& poly) {
    c = backup;
    slider_error = poly.generate_error();
    }
  if(need_restart && need_start) start_game();
  }

EX void set_sliders() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("tessellation sliders"));
  dialog::addHelp(current.comment);
  char ch = 'A';
  for(auto& sl: current.sliders) {
    dialog::addSelItem(sl.name, fts(sl.current), ch++);
    dialog::add_action([&] {
      dialog::editNumber(sl.current, sl.min, sl.max, 1, sl.zero, sl.name, sl.name);
      dialog::reaction = [] { sliders_changed(false, false); };
      });
    }
  if(isize(current.intsliders))
    dialog::addInfo(XLAT("the following sliders will restart the game"));
  for(auto& sl: current.intsliders) {
    dialog::addSelItem(sl.name, its(sl.current), ch++);
    dialog::add_action([&] {
      dialog::editNumber(sl.current, sl.min, sl.max, 1, sl.zero, sl.name, sl.name);
      dialog::reaction = [] { sliders_changed(true, true); };
      });
    }
  dialog::addInfo(slider_error);
  dialog::addBack();
  dialog::display();
  }

/** convert a tessellation (e.g. Archimedean, regular, etc.) to the arb::current internal representation */
EX namespace convert {

EX eGeometry base_geometry;
EX eVariation base_variation;

struct id_record {
  int target;   /* master of this id type */
  int shift;    /* sample direction 0 == our direction shift */
  int modval;   /* this master type is the same as itself rotated by modval */
  cell *sample; /* sample of the master type */
  };

inline void print(hstream& hs, const id_record& i) { print(hs, "[", i.target, " shift=", i.shift, " mod=", i.modval, "]"); }

map<int, id_record> identification;

id_record& get_identification(int s, cell *c) {  
  if(!identification.count(s)) {
    auto &id = identification[s];
    id.target = s;
    id.shift = 0;
    id.modval = c->type;
    id.sample = c;
    }
  return identification[s];
  }

id_record& get_identification(cell *c) {  
  auto id = currentmap->full_shvid(c);
  return get_identification(id, c);
  }

int changes;

void be_identified(cellwalker cw1, cellwalker cw2) {
  auto& id1 = get_identification(cw1.at);
  auto& id2 = get_identification(cw2.at);
  
  indenter ind(2);
  
  int t = cw2.at->type;

  if(cw1.at->type != t) {
    println(hlog, cw1.at->type, " vs ", t);
    throw hr_exception("numbers disagree");
    }
    
  int d2 = gmod(-cw2.to_spin(id2.shift), id2.modval);
  int d1 = gmod(-cw1.to_spin(id1.shift), id1.modval);

  indenter ind1(2);

  if(id2.target != id1.target) {
    auto oid2 = id2;
    id1.modval = gcd(id1.modval, id2.modval);
    for(auto& p: identification) {
      auto& idr = p.second;
      if(idr.target == oid2.target) {
        idr.target = id1.target;
        idr.modval = id1.modval;
        idr.shift = gmod(idr.shift + (d2-d1), idr.modval);        
        idr.sample = id1.sample;
        }
      }
    changes++;
    println(hlog, identification);
    return;
    }
  if(d2 != d1) {
    auto oid2 = id2;
    id2.modval = gcd(id2.modval, abs(d2-d1));
    for(auto& p: identification) 
      if(p.second.target == oid2.target) p.second.modval = id2.modval;
    changes++;
    println(hlog, identification);
    return;
    }
  }

EX bool reverse_order;
EX bool minimize_on_convert;

EX void convert_max() {
  identification.clear(); changes = 0;

  manual_celllister cl;
  cl.add(currentmap->gamestart());
  
  int more_tests = 1000;
  pointer_indices.clear();

  int chg = -1;
  while(changes > chg) {
    changes = chg;
    
    set<int> masters_analyzed;

    for(int i=0; i<isize(cl.lst); i++) {
      auto c = cl.lst[i];
      auto& id = get_identification(c);
      
      if(masters_analyzed.count(id.target)) {
        more_tests--;
        if(more_tests < 0) continue;
        }
      masters_analyzed.insert(id.target);
      
      cellwalker cw0(c, id.shift);
      cellwalker cws(id.sample, 0);
      
      for(int i=0; i<c->type; i++) {
        if(1) {
          indenter ind(2);
          be_identified(cw0 + i + wstep, cws + i + wstep);
          be_identified(cw0 + i + wstep, cw0 + i + id.modval + wstep);
          }
        
        if(1) {
          indenter ind(2);
          auto cwx = cw0 + i + wstep;
          
          auto idx = get_identification(cwx.at);
          cellwalker xsample(idx.sample, cwx.spin);
          xsample -= idx.shift;
          
          be_identified(cwx + wstep, xsample + wstep);
          
          cl.add((cw0 + i).cpeek());
          }
        }
      }
    }
  }

EX void convert_minimize(int N, vector<int>& old_shvids, map<int, int>& old_to_new) {
  vector<pair<int, int>> address;
  vector<int> next;
  for(int i=0; i<N; i++) {
    int q = identification[old_shvids[i]].modval;
    int c = isize(address);
    for(int j=0; j<q; j++) {
      address.emplace_back(i, j);
      next.emplace_back(j == q-1 ? c : c+j+1);
      }
    }

  int K = isize(address);  
  vector<array<ld, 3> > dists(K);
  for(int i=0; i<K; i++) {
    auto pi = address[i];
    auto si = identification[old_shvids[pi.first]];
    pi.second += si.shift;
    array<hyperpoint, 3> pcorner;
    array<ld, 3> pdists;

    for(int j=0; j<3; j++)
      pcorner[j] = currentmap->get_corner(si.sample, gmod(pi.second+j, si.sample->type));

    for(int j=0; j<3; j++)
      pdists[j] = hdist(pcorner[j], pcorner[(j+1)%3]);

    dists[i] = pdists;
    }

  // this is O(K^3) and also possibly could get confused on convex/concave,
  // but should be good enough, hopefully
  
  vector<vector<int>> equal(K);
  for(int i=0; i<K; i++) equal[i].resize(K, 0);
  for(int i=0; i<K; i++)
  for(int j=0; j<K; j++) {

    equal[i][j] = true;
    for(int s=0; s<3; s++)
      equal[i][j] = equal[i][j] && abs(dists[i][s] - dists[j][s]) < 1e-6;
    }
  
  int chg = 1;
  while(chg) {
    for(auto& eq: equal) println(hlog, eq);
    chg = 0;
    for(int i=0; i<K; i++)
    for(int j=0; j<K; j++)
      if(equal[i][j] && !equal[next[i]][next[j]]) {
        equal[i][j] = false;
        chg++;
        }
    }

  for(int i=0; i<K; i++)
  for(int j=0; j<K; j++) if(i!=j && equal[i][j]) {
    auto pi = address[i];
    auto si = identification[old_shvids[pi.first]];
    cellwalker cwi(si.sample, si.shift + pi.second);

    auto pj = address[j];
    auto sj = identification[old_shvids[pj.first]];
    cellwalker cwj(sj.sample, sj.shift + pj.second);

    be_identified(cwi, cwj);
    }
  }

EX void convert() {
  start_game();
  convert_max();
  bool minimize = minimize_on_convert;
  reidentify:
  vector<int> old_shvids;
  map<int, int> old_to_new;
  for(auto id: identification)
    if(id.first == id.second.target) {
      old_to_new[id.first] = isize(old_shvids);
      old_shvids.push_back(id.first);
      }
  
  int N = isize(old_shvids);
  println(hlog, "N = ", N);
  if(minimize) {
    convert_minimize(N, old_shvids, old_to_new);
    minimize = false;
    goto reidentify;
    }

  if(reverse_order) {
    reverse(old_shvids.begin(), old_shvids.end());
    for(int i=0; i<isize(old_shvids); i++)
      old_to_new[old_shvids[i]] = i;
    }

  auto& ac = arb::current;
  ac.order++; 
  ac.comment = ac.filename = "converted from: " + full_geometry_name();
  ac.cscale = cgi.scalefactor;
  ac.boundary_ratio = 1;
  ac.floor_scale = cgi.hexvdist / cgi.scalefactor;
  ac.range = cgi.base_distlimit;
  ac.shapes.clear();
  ac.shapes.resize(N);

  ginf[gArbitrary].g = cginf.g;
  ginf[gArbitrary].flags = cgflags & qCLOSED;
  
  for(int i=0; i<N; i++) {
    auto id = identification[old_shvids[i]];
    cell *s = id.sample;
    auto& sh = ac.shapes[i];
    sh.id = i;
    int t = s->type;
    sh.vertices.clear();
    sh.connections.clear();
    sh.cycle_length = id.modval;
    sh.repeat_value = t / id.modval;
    sh.flags = hr::pseudohept(s) ? arcm::sfPH : 0;
    #if CAP_ARCM
    if(arcm::in() && arcm::linespattern(s)) { sh.flags |= arcm::sfLINE; ac.have_line = true; }
    #endif
    for(int j=0; j<t; j++) {
      auto co = currentmap->get_corner(s, j);
      sh.vertices.push_back(co);
      cellwalker cw(s, j);
      cw += wstep;
      auto idx = get_identification(cw.at);
      cellwalker xsample(idx.sample, cw.spin);
      xsample -= idx.shift;
      sh.connections.emplace_back(arb::connection_t{old_to_new.at(idx.target), xsample.spin, false});
      }
    sh.stretch_shear.resize(t, make_pair(1, 0));    
    sh.edges.clear();
    for(int j=0; j<t-1; j++)
      sh.edges.push_back(hdist(sh.vertices[j], sh.vertices[j+1]));
    sh.edges.push_back(hdist(sh.vertices[t-1], sh.vertices[0]));

    sh.angles.clear();
    for(int j=0; j<t; j++) {
      hyperpoint v0 = sh.vertices[j];
      hyperpoint v1 = sh.vertices[(j+1) % t];
      hyperpoint v2 = sh.vertices[(j+2) % t];
      transmatrix T = gpushxto0(v1);
      v0 = T * v0;
      v2 = T * v2;
      ld alpha = atan2(v0) - atan2(v2);
      cyclefix(alpha, 0);
      sh.angles.push_back(alpha);
      }
    if(debugflags & DF_GEOM) {
      println(hlog, "shape ", i, ":");
      indenter indp(2);
      println(hlog, "vertices=", sh.vertices);
      println(hlog, "connections=", sh.connections);
      println(hlog, "edges=", sh.edges);
      println(hlog, "angles=", sh.angles);
      }
    }
  
  arb::compute_vertex_valence(ac);

  ac.have_ph = geosupport_football() ? 1 : 0;
  arb::check_football_colorability(ac);
  }

EX bool in() {
  return arb::in() && base_geometry != gArbitrary;
  }

/** activate the converted tessellation */
EX void activate() {
  if(geometry != gArbitrary) {
    base_geometry = geometry;
    base_variation = variation;
    stop_game();
    geometry = gArbitrary;
    variation = eVariation::pure;
    }
  }

EX }

#if CAP_COMMANDLINE  
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-tes") || argis("-arbi")) {
    PHASEFROM(2);
    shift(); 
    run(args());
    }
  else if(argis("-tes-opt")) {
     arg::run_arguments(current.options);
    }
  else if(argis("-arb-convert")) {
    try {
      convert::convert();
      set_geometry(gArbitrary);      
      }
    catch(hr_exception& e) {
      println(hlog, "failed to convert: ", e.what());
      }
    }
  else if(argis("-arb-unmirror")) {
    shift(); do_unmirror = argi();
    }
  else if(argis("-arb-football")) {
    shift(); extended_football = argi();
    }
  else if(argis("-arb-slider")) {
    PHASEFROM(2);
    shift();
    string slider = args();
    bool found = true;
    for(auto& sl: current.sliders)
      if(sl.name == slider) {
        shift_arg_formula(sl.current, [] { sliders_changed(false, false); });
        found = true;
        }
    for(auto& sl: current.intsliders)
      if(sl.name == slider) {
        shift(); sl.current = argi();
        stop_game();
        sliders_changed(true, false);
        found = true;
        }
    if(!found) {
      println(hlog, "warning: no slider named ", slider, " found");
      shift();
      }
    }
  else return 1;
  return 0;
  }

auto hook = addHook(hooks_args, 100, readArgs);
#endif

EX bool in() { return geometry == gArbitrary; }

EX string tes = find_file("tessellations/sample/marjorie-rice.tes");

EX bool linespattern(cell *c) {
  return current.shapes[id_of(c->master)].flags & arcm::sfLINE;
  }

EX bool pseudohept(cell *c) {
  if(!current.have_ph) return true;
  return current.shapes[id_of(c->master)].flags & arcm::sfPH;
  }

EX void choose() {
  dialog::openFileDialog(tes, XLAT("open a tiling"), ".tes", 
  [] () {
    run(tes);
    #if CAP_COMMANDLINE
    if(!current.options.empty())
      dialog::push_confirm_dialog([] { arg::run_arguments(current.options); start_game(); }, "load the settings defined in this file?");
    #endif
    return true;
    });
  }

EX pair<ld, ld> rep_ideal(ld e, ld u IS(1)) {
  ld alpha = TAU / e;
  hyperpoint h1 = point3(cos(alpha)*u, -sin(alpha)*u, 1);
  hyperpoint h2 = point3(u, 0, 1);
  hyperpoint h3 = point3(cos(alpha)*u, sin(alpha)*u, 1);
  hyperpoint h12 = mid(h1, h2);
  hyperpoint h23 = mid(h2, h3);
  ld len = hdist(h12, h23);
  transmatrix T = gpushxto0(h12);
  auto T0 = T * C0;
  auto Th23 = T * h23;
  ld beta = atan2(T0);
  ld gamma = atan2(Th23);
  return {len, 90._deg - (gamma - beta)};
  }

EX void swap_vertices() {
  for(auto& p: {&current, &slided}) 
    for(auto& s: p->shapes)
      for(auto& v: s.vertices)
        swappoint(v);
  }

#if MAXMDIM >= 4
auto hooksw = addHook(hooks_swapdim, 100, [] {
  swap_vertices();
  for(auto& p: altmap) for(auto& pp: p.second) swapmatrix(pp.second);
  for(auto& p: arbi_matrix) swapmatrix(p.second.second);
  });
#endif

EX }
}
