// Hyperbolic Rogue -- rule generator
// Copyright (C) 2011-2021 Zeno Rogue, see 'hyper.cpp' for details

/** \file rulegen3.cpp 
 *  \brief An algorithm to create strict tree rules for arb tessellations -- 3D parts
 */

#include "hyper.h"

namespace hr {

EX namespace rulegen {

struct road_shortcut_trie_vertex {
  set<vector<int>> backpaths;
  map<int, shared_ptr<struct road_shortcut_trie_vertex>> children;
  };

EX map<int, shared_ptr<struct road_shortcut_trie_vertex>> road_shortcuts;

int qroad;

map<int, int> qroad_for;
map<tcell*, int> qroad_memo;

EX void add_road_shortcut(tcell *s, tcell *t) {
  shared_ptr<road_shortcut_trie_vertex> u;
  vector<int> tpath;
  if(!road_shortcuts.count(s->id)) road_shortcuts[s->id] = make_shared<road_shortcut_trie_vertex>();
  u = road_shortcuts[s->id];
  while(true) {
    // println(hlog, s, " dist=", s->dist, " parent = ", s->parent_dir, " vs ", t, " dist=", t->dist, " parent = ", t->parent_dir);
    if(s == t) {
      reverse(tpath.begin(), tpath.end());
      auto& ba = u->backpaths;
      if(!ba.count(tpath)) qroad++, qroad_for[s->id]++;
      ba.insert(tpath);
      return;
      }
    if(s->dist >= t->dist) {
      twalker sw = s;
      get_parent_dir(sw);
      if(s->parent_dir == MYSTERY) throw hr_exception("unknown parent_dir (s) in add_road_shortcut");
      if(!u->children.count(s->parent_dir)) u->children[s->parent_dir] = make_shared<road_shortcut_trie_vertex>();
      u = u->children[s->parent_dir];
      s = s->move(s->parent_dir);
      }
    if(t->dist > s->dist) {
      twalker tw = t;
      get_parent_dir(tw);
      if(t->parent_dir == MYSTERY) throw hr_exception("unknown parent_dir (t) in add_road_shortcut");
      tpath.push_back(t->c.spin(t->parent_dir));
      t = t->move(t->parent_dir);
      }
    }
  }

EX int newcon;

EX void apply_road_shortcut(tcell *s) {
  auto& mem = qroad_memo[s];
  if(mem == qroad_for[s->id]) return;
  mem = qroad_for[s->id];
  shared_ptr<road_shortcut_trie_vertex> u;
  if(!road_shortcuts.count(s->id)) return;
  u = road_shortcuts[s->id];
  int q = tcellcount;
  while(true) {
    for(auto& v: u->backpaths) {
      auto s1 = s;
      for(auto x: v) {
        s1 = s1->cmove(x);
        be_solid(s1);
        }
      }
    twalker s0 = s; get_parent_dir(s0);
    if(!u->children.count(s->parent_dir)) break;
    u = u->children[s->parent_dir];
    s = s->move(s->parent_dir);
    }
  static int qmax = 0;
  newcon += tcellcount - q;
  if(tcellcount > q + qmax) println(hlog, "road shortcuts created ", qmax = tcellcount-q, " new connections");
  }

/** next roadsign ID -- they start at -100 and go downwards */
int next_roadsign_id = -100;

/** get the ID of a roadsign path */
EX map<vector<int>, int> roadsign_id;

EX int get_roadsign(twalker what) {
  int dlimit = what.at->dist - 1;
  tcell *s = what.at, *t = what.peek();
  apply_road_shortcut(s);
  vector<int> result;
  while(s->dist > dlimit) { 
    twalker s0 = s;
    get_parent_dir(s0);
    if(s->parent_dir == MYSTERY) throw hr_exception("parent_dir unknown");
    result.push_back(s->parent_dir); s = s->move(s->parent_dir);
    result.push_back(s->dist - dlimit);
    }
  vector<int> tail;
  while(t->dist > dlimit) {
    twalker t0 = t;
    get_parent_dir(t0);
    if(t->parent_dir == MYSTERY) throw hr_exception("parent_dir unknown");
    tail.push_back(t->dist - dlimit);
    tail.push_back(t->c.spin(t->parent_dir));
    t = t->move(t->parent_dir);
    }
  map<tcell*, int> visited;
  queue<tcell*> vqueue;
  auto visit = [&] (tcell *c, int dir) {
    if(visited.count(c)) return;
    visited[c] = dir;
    vqueue.push(c);
    };
  visit(s, MYSTERY);
  while(true) {
    if(vqueue.empty()) throw hr_exception("vqueue empty");
    tcell *c = vqueue.front();
    if(c == t) break;
    vqueue.pop();
    for(int i=0; i<c->type; i++)
      if(c->move(i) && c->move(i)->dist <= dlimit)
        visit(c->move(i), c->c.spin(i));
    }
  while(t != s) {
    add_road_shortcut(s, t);
    int d = visited.at(t);
    tail.push_back(t->dist - dlimit);
    tail.push_back(t->c.spin(d));
    t = t->move(d);
    }
  reverse(tail.begin(), tail.end());
  for(auto t: tail) result.push_back(t);
  if(roadsign_id.count(result)) return roadsign_id[result];
  return roadsign_id[result] = next_roadsign_id--;
  }

map<pair<int, int>, vector<pair<int, int>> > all_edges;

EX vector<pair<int, int>>& check_all_edges(twalker cw, analyzer_state* a, int id) {
  auto& ae = all_edges[{cw.at->id, cw.spin}];
  if(ae.empty()) {
    set<tcell*> seen;
    vector<pair<twalker, transmatrix> > visited;
    vector<pair<int, int>> ae1;
    auto visit = [&] (twalker tw, const transmatrix& T, int id, int dir) {
      if(seen.count(tw.at)) return;
      seen.insert(tw.at);
      auto& sh0 = currentmap->get_cellshape(tcell_to_cell[cw.at]);
      auto& sh1 = currentmap->get_cellshape(tcell_to_cell[tw.at]);
      int common = 0;
      vector<hyperpoint> kleinized;
      vector<hyperpoint> rotated;
      for(auto v: sh0.vertices_only) kleinized.push_back(kleinize(sh0.from_cellcenter * v));
      for(auto w: sh1.vertices_only) rotated.push_back(kleinize(T*sh1.from_cellcenter * w));
      
      for(auto v: kleinized)
      for(auto w: rotated)
        if(sqhypot_d(MDIM, v-w) < 1e-6)
          common++;
      if(flags & w_vertex_edges) {
        if(common < 1) { ae1.emplace_back(id, dir); return; }
        }
      else {
        if(common < 2) { ae1.emplace_back(id, dir); return; }
        }
      visited.emplace_back(tw, T);
      ae.emplace_back(id, dir);
      };
    visit(cw, Id, -1, -1);
    for(int i=0; i<isize(visited); i++) {
      auto tw = visited[i].first;      
      for(int j=0; j<tw.at->type; j++) {
        visit(tw + j + wstep, visited[i].second * currentmap->adj(tcell_to_cell[tw.at], (tw+j).spin), i, j);
        }
      }
    if(flags & w_ae_extra_step) for(auto p: ae1) ae.push_back(p);
    println(hlog, "for ", tie(cw.at->id, cw.spin), " generated all_edges structure: ", ae, " of size ", isize(ae));
    }
  return ae;
  }

EX void cleanup3() {
  all_edges.clear();
  roadsign_id.clear();
  next_roadsign_id = -100;
  }

int last_qroad;

EX void check_road_shortcuts() {
  println(hlog, "road shortcuts = ", qroad, " treestates = ", isize(treestates), " roadsigns = ", next_roadsign_id);
  if(qroad > last_qroad) {
    println(hlog, "qroad_for = ", qroad_for);
    println(hlog, "newcon = ", newcon, " tcellcount = ", tcellcount); newcon = 0;
    clear_codes();
    last_qroad = qroad;
    roadsign_id.clear();
    next_roadsign_id = -100;
    throw rulegen_retry("new road shortcuts");
    }
  }

void genhoneycomb(string fname) {
  if(WDIM != 3) throw hr_exception("genhoneycomb not in honeycomb");

  int qc = isize(t_origin);

  vector<short> data;
  string side_data;

  map<int, vector<int>> rev_roadsign_id;
  for(auto& rs: roadsign_id) rev_roadsign_id[rs.second] = rs.first;

  int N = isize(treestates);
  using classdata = pair<vector<int>, int>;
  vector<classdata> nclassify(N);
  for(int i=0; i<N; i++) nclassify[i] = {{0}, i};

  int numclass = 1;
  while(true) {
    println(hlog, "N = ", N, " numclass = ", numclass);
    for(int i=0; i<N; i++) {
      auto& ts = treestates[i];
      for(int j=0; j<isize(ts.rules); j++) {
        int j1 = gmod(j - ts.giver.spin, isize(ts.rules));
        auto r = ts.rules[j1];
        if(r < 0) nclassify[i].first.push_back(r);
        else nclassify[i].first.push_back(nclassify[r].first[0]);
        }
      }
    sort(nclassify.begin(), nclassify.end());
    vector<int> last = {}; int newclass = 0;
    for(int i=0; i<N; i++) {
      if(nclassify[i].first != last) {
        newclass++;
        last = nclassify[i].first;
        }
      nclassify[i].first = {newclass-1};
      }
    sort(nclassify.begin(), nclassify.end(), [] (const classdata& a, const classdata& b) { return a.second < b.second; });
    if(numclass == newclass) break;
    numclass = newclass;
    }
  vector<int> representative(numclass);
  for(int i=0; i<isize(treestates); i++) representative[nclassify[i].first[0]] = i;

  println(hlog, "Minimized rules (", numclass, " states):");
  for(int i=0; i<numclass; i++) {
    auto& ts = treestates[representative[i]];
    print(hlog, lalign(4, i), ":");
    for(int j=0; j<isize(ts.rules); j++) {
      int j1 = gmod(j - ts.giver.spin, isize(ts.rules));
      auto r =ts.rules[j1];
      if(r == DIR_PARENT) print(hlog, " P");
      else if(r >= 0) print(hlog, " ", nclassify[r].first[0]);
      else print(hlog, " S", r);
      }
    println(hlog);
    }
  println(hlog);

  vector<int> childpos;

  for(int i=0; i<numclass; i++) {
    childpos.push_back(isize(data));
    auto& ts = treestates[representative[i]];
    for(int j=0; j<isize(ts.rules); j++) {
      int j1 = gmod(j - ts.giver.spin, isize(ts.rules));
      auto r =ts.rules[j1];
      if(r == DIR_PARENT) {
        data.push_back(-1);
        side_data += ('A' + j);
        side_data += ",";
        }
      else if(r >= 0) {
        data.push_back(nclassify[r].first[0]);
        }
      else {
        data.push_back(-1);
        auto& str = rev_roadsign_id[r];
        bool next = true;
        for(auto ch: str) {
          if(next) side_data += ('a' + ch);
          next = !next;
          }
        side_data += ",";
        }
      }
    }
  childpos.push_back(isize(data));

  shstream ss;

  ss.write(ss.get_vernum());
  mapstream::save_geometry(ss);
  ss.write(fieldpattern::use_rule_fp);
  ss.write(fieldpattern::use_quotient_fp);
  ss.write(reg3::minimize_quotient_maps);

  auto& fp = currfp;
  hwrite_fpattern(ss, fp);

  vector<int> root(qc, 0);
  for(int i=0; i<qc; i++) root[i] = nclassify[get_treestate_id(t_origin[i]).second].first[0];
  println(hlog, "root = ", root);
  hwrite(ss, root);

  println(hlog, "data = ", data);
  hwrite(ss, data);
  println(hlog, "side_data = ", side_data);
  hwrite(ss, side_data);
  println(hlog, "childpos = ", childpos);
  hwrite(ss, childpos);

  println(hlog, "compress_string");
  string s = compress_string(ss.s);

  fhstream of(fname, "wb");
  print(of, s);
  }

#if CAP_COMMANDLINE
int readRuleArgs3() {
  using namespace arg;
  if(0) ;
  else if(argis("-gen-honeycomb")) {
    shift(); genhoneycomb(args());
    }

  else if(argis("-urq")) {
    // -urq 7 to prepare honeycomb generation
    stop_game();
    shift(); int i = argi();
    reg3::reg3_rule_available = (i & 8) ? 0 : 1;
    fieldpattern::use_rule_fp = (i & 1) ? 1 : 0;
    fieldpattern::use_quotient_fp = (i & 2) ? 1 : 0;
    reg3::minimize_quotient_maps = (i & 4) ? 1 : 0;
    }

  else if(argis("-subrule")) {    
    stop_game();
    shift(); reg3::other_rule = args();
    shstream ins(decompress_string(read_file_as_string(args())));
    ins.read(ins.vernum);
    mapstream::load_geometry(ins);
    reg3::subrule = true;
    }

  else return 1;
  return 0;
  }

auto hook3 = addHook(hooks_args, 100, readRuleArgs3);
#endif

}

}