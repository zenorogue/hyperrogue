/* some extra routines for debugging and testing the rulegen algorithm */

#include "../hyper.h"
#include <fstream>
#include <chrono>
#include <semaphore.h>

// extra ruleflags:
// 30: do not clear memory

namespace hr {
namespace rulegen {

pair<int,int> longest_shortcut();

EX flagtype sub_rulegen_flags;

string testroot = "devmods/rulegen-tests/";
string testdir = testroot;

struct hrmap_testproto : hrmap {
  map<heptagon*, tcell*> counterpart;
  map<tcell*, heptagon*> counterpart2;

  heptagon* clone(tcell *o) {
    auto& h = counterpart2[o];
    if(!h) {
      h = tailored_alloc<heptagon> (o->type);
      counterpart[h] = o;
      h->zebraval = 0;
      h->emeraldval = 0;
      h->distance = 0;
      h->cdata = nullptr;
      h->c7 = newCell(o->type, h);
      h->c7->land = laCanvas;
      }
    return h;
    }

  ~hrmap_testproto() { 
    println(hlog, "clearing ", isize(counterpart), " heptagons from testproto");
    for(auto p: counterpart) {
      auto& at = p.first;
      if(at->cdata) delete at->cdata;
      destroy_cell(at->c7);      
      tailored_delete(at);
      }
    }

  heptagon *getOrigin() override { 
    return clone(t_origin[0].at);
    }
  
  heptagon *create_step(heptagon *h, int d) override {
    auto ch = counterpart[h];
    if(!ch->move(d)) {
      h->c.connect(d, &oob, 0, false);
      h->c7->c.connect(d, &out_of_bounds, 0, false);
      out_of_bounds.master = &oob;
      oob.c7 = &out_of_bounds;
      return &oob;
      }
    auto ch1 = ch->cmove(d);
    auto d1 = ch->c.spin(d);
    auto h1 = clone(ch1);
    if(ch == ch1)
      throw rulegen_failure("connected to self??");
    if(h == h1) 
      throw rulegen_failure("connected to self on hept level??");
    h->c.connect(d, h1, d1, false);
    return h1;
    }  
  
  void find_cell_connection(cell *c, int d) {
    heptagon *h2 = createStep(c->master, d);
    if(h2 == &oob) return;
    c->c.connect(d, h2->c7,c->master->c.spin(d), c->master->c.mirror(d));
    }

  transmatrix adj(heptagon *h, int dir) override {
    if(h->move(dir) == &oob || h == &oob) return Id;
    return arb::get_adj(arb::current_or_slided(), shvid(h->c7), dir, -1, h->c.move(dir) ? h->c.spin(dir) : -1);
    }

  hyperpoint get_corner(cell *c, int cid, ld cf) override {
    auto h = c->master;
    if(!counterpart.count(h)) {
      auto& sh = arb::current_or_slided().shapes[c->master->zebraval];
      cid = gmod(cid, sh.size());
      return normalize(C0 + (sh.vertices[cid] - C0) * 3 / cf);
      }
    int s = counterpart[h]->id;
    auto& sh = arb::current_or_slided().shapes[s];

    return normalize(C0 + (sh.vertices[cid] - C0) * 3 / cf);
    }

  int shvid(cell *c) override {
    if(!counterpart.count(c->master)) {
      /* may happen while handling floorshapes */
      return arb::id_of(c->master);
      }
    auto cc = counterpart[c->master];
    return cc->id;
    }
  
  bool strict_tree_rules() { return false; }
  };

reaction_t clear_debug = [] {};

map<tcell*,int> sprawl_shown;

vector<twalker> old_sprawl, cur_sprawl;
int old_sprawl_id;

int total_analyzers();

void cleanup_protomap() {
  auto m = dynamic_cast<hrmap_testproto*> (currentmap);
  auto *c = first_tcell;
  while(c) {
    auto wh = m->clone(c);
    for(int i=0; i<wh->type; i++) if(wh->move(i) == &oob) wh->move(i) = nullptr;
    for(int i=0; i<wh->c7->type; i++) if(wh->c7->move(i) == &out_of_bounds) wh->c7->move(i) = nullptr;
    c = c->next;
    }
  }

void iterate(int qty) {
  dynamicval<int> set_timeout(rulegen_timeout, 999999);
  for(int i=0; i<qty; i++) {
    try {
      rules_iteration();
      try_count--;
      break;
      }
    catch(rulegen_retry& f) {
      println(hlog, "retry on: ", f.what());
      }
    catch(rulegen_surrender& f) {
      println(hlog, "surrender: ", f.what());
      }
    catch(rulegen_failure& f) {
      println(hlog, "failure: ", f.what());
      }
    }
  println(hlog, "try_count = ", try_count, " states = ", isize(treestates), " imp = ", isize(important), " analyzers = ", total_analyzers(), " cell = ", tcellcount, " shortcuts = ", longest_shortcut());
  cleanup_protomap();
  }

void print_rules();

void irradiate() {
  dynamicval<int> set_timeout(rulegen_timeout, 999999);
  try{
    vector<tcell*> last;
    auto *c = first_tcell;
    while(c) {
      last.push_back(c);
      c = c->next;
      }
    for(tcell* l: last) {
      ufindc(l);
      for(int i=0; i<l->type; i++) l->cmove(i);
      }
    handle_distance_errors();
    }
  catch(rulegen_failure& r) {
    println(hlog, "error in irradiate: ", r.what());
    }
  cleanup_protomap();
  }

void move_to(cellwalker cw) {  
  cwt = cw;
  centerover = cwt.at;
  View = Id;
  }

void move_to(twalker dw) {
  auto m = dynamic_cast<hrmap_testproto*> (currentmap);
  ufind(dw);
  move_to(cellwalker(m->clone(dw.at)->c7, dw.spin, dw.mirrored));
  }

vector<twalker> old_givers;

void try_sprawling(tcell *c) {
  twalker cw = c;
  cw = get_parent_dir(cw);
  ufind(cw);
  auto aid = get_aid(cw);
  auto a_ptr = &(analyzers[aid.first][aid.second]);
  vector<twalker> sprawl = { cw };
  vector<analyzer_state*> states = { nullptr };

  while(true) {
    auto& a = *a_ptr;
    if(!a) {
      println(hlog, "analyzer not allocated");
      return;
      }
    states.push_back(a);
    if(isize(sprawl) <= cw.at->type) {
      a->id = 0, a->dir = isize(sprawl)-1;
      }
    if(a->id == MYSTERY) {
      println(hlog, "reached codeid ", a->analyzer_id, " which is state ", a->dir);
      return;
      }
    auto t = sprawl[a->id];
    twalker tw = t + a->dir;
    ufind(tw);
    tw.cpeek();
    ufind(tw);
    int mc = move_code(tw + wstep);
    sprawl.push_back(tw + wstep);
    println(hlog, "codeid ", a->analyzer_id, ": going from ", tw, " in direction ", a->dir, " reaching ", sprawl.back(), " of movecode ", mc);
    a_ptr = &(a->substates[mc]);
    }
  }

void debug_menu() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  auto m = dynamic_cast<hrmap_testproto*> (currentmap);
  dialog::init("debug menu");
  
  dialog::addItem("parent_dir", 'p');
  dialog::add_action([m] {
    tcell *c = m->counterpart[centerover->master];
    auto c1 = c;
    ufindc(c1);
    if(c != c1) {
      println(hlog, "ufindc changes ", c, " to ", c1);
      for(int i=0; i<c->type; i++)
        println(hlog, twalker(c,i), twalker(c,i)+wstep, twalker(c,i)+wstep+wstep);
      }
    println(hlog, "parent_dir = ", c->parent_dir);
    c->parent_dir = MYSTERY;
    rulegen::rdebug_flags |= 16;
    try {
      twalker cw(c, 0);
      get_parent_dir(cw);
      }
    catch(rulegen_failure& f) {
      println(hlog, "catched: ", f.what());
      }
    rulegen::rdebug_flags &= ~16;
    println(hlog, "parent_dir = ", c->parent_dir);
    cleanup_protomap();
    });
  
  dialog::addItem("iterate", 'm');
  dialog::add_action([] { iterate(1); });

  dialog::addItem("iterate x100", 'M');
  dialog::add_action([] { iterate(100); });

  dialog::addItem("iterate x1000", 'T');
  dialog::add_action([] { iterate(1000); });

  dialog::addItem("debug tiles", 'd');
  dialog::add_action_push([m] { 
    cmode = sm::SIDE | sm::MAYDARK;
    gamescreen();
    dialog::init();
    for(auto dw: debuglist) {
      dialog::addItem("go to " + index_pointer(dw.at), 'a');
      dialog::add_action([dw] {
        move_to(dw);
        });
      }
    dialog::display();
    });
  
  dialog::addItem("furthest", 'f');
  dialog::add_action([m] { 
    auto *c = first_tcell;
    tcell *furthest = c;
    while(c) {
      if(c->unified_to.at == c)
        if(c->dist < MYSTERY && c->dist > furthest->dist) furthest = c;
      c = c->next;
      }
    println(hlog, "furthest in distance = ", furthest->dist);
    move_to(furthest);
    });
  
  dialog::addItem("print rules", 'P');
  dialog::add_action(print_rules);

  dialog::addItem("clean data", 'c');
  dialog::add_action(clean_data);

  dialog::addItem("clean parents", 'p');
  dialog::add_action(clean_parents);

  dialog::addItem("irradiate", 'i');
  dialog::add_action(irradiate);

  dialog::addItem("irradiate x10", 'I');
  dialog::add_action([] { for(int i=0; i<10; i++) irradiate(); });

  dialog::addItem("record givers", 'g');
  dialog::add_action([] {
    old_givers.clear();
    for(int i=0; i<isize(treestates); i++) old_givers.push_back(treestates[i].giver);
    println(hlog, "old_givers = ", old_givers);
    debuglist = old_givers;
    });

  dialog::addItem("compare givers", 'G');
  dialog::add_action([] {
    int Q = isize(old_givers);
    debuglist = {};
    for(int i=0; i<Q; i++)
    for(int j=0; j<i; j++) {
      auto c1 = get_treestate_id(old_givers[i]);
      auto c2 = get_treestate_id(old_givers[j]);
      if(c1.second == c2.second) {
        println(hlog, "old state ", i, " at ", old_givers[i], " and old state ", j, " at ", old_givers[j], " have now code ", c1);
        debuglist.push_back(old_givers[i]);
        debuglist.push_back(old_givers[j]);
        }
      }
    });

  dialog::addItem("sprawl", 's');
  dialog::add_action([m] { try_sprawling(m->counterpart[cwt.at->master]); });

  dialog::addItem("name", 'n');
  dialog::add_action([m] { println(hlog, "name = ", index_pointer(m->counterpart[cwt.at->master])); });

  dialog::display();
  }  

color_t label_color = 1, wall_color = 0, tree_color = 0xFFFFFFFF, out_color = 0xFF0000FF;

bool show_codes = true;
bool show_dist = true;

void view_debug() {
  auto m = dynamic_cast<hrmap_testproto*> (currentmap);
  if(m) {
    int ah = addHook(hooks_drawcell, 50, [m] (cell *c, const shiftmatrix& V) {
      tcell *tc = m->counterpart[c->master];
      
      string s;
      auto label = (tc->dist == MYSTERY ? "?" : its(tc->dist));
      
      if(show_codes) {
        int code = tc->code;
        if(code != MYSTERY_LARGE) code = all_analyzers[code]->dir;
        else code = MYSTERY;
        string codestr = (code == MYSTERY ? "?" : its(code));
        if(show_dist) label = label + "/" + codestr;
        else label = codestr;
        }
      
      color_t col = label_color == 1 ? 0xFFFFFF + 0x512960 * tc->code : label_color;
  
      if(pointer_indices.count(tc)) label += " " + index_pointer(tc);
      
      queuestr(V, 0.4, label, col, 1);
      if(tree_color) {
        if(tc->parent_dir >= 0 && tc->parent_dir < tc->type) {
          vid.linewidth *= 8;
          queueline(V * C0, V * currentmap->adj(c, tc->parent_dir) * C0, tree_color);
          vid.linewidth /= 8;
          }
        }
        
      if(sprawl_shown.count(tc))
        queuepoly(V, cgi.shDisk, 0xFF0000FF);
      
      if(out_color) {
        for(int i=0; i<tc->type; i++) if(!tc->move(i)) {
          vid.linewidth *= 8;
          hyperpoint h1 = currentmap->get_corner(c, (i+1)%tc->type);
          hyperpoint h2 = currentmap->get_corner(c, i);
          queueline(V * h1, V * h2, 0xFF0000FF);
          vid.linewidth /= 8;
          }
        }

      return false;
      });
    
    vector<int> dh;

    dh.push_back(addHook(hooks_o_key, 15, [m] (o_funcs& v) {  
      /* v.push_back(named_functionality("mark", [m] {
        for(auto c: marklist)
          m->clone(c.at)->c7->item = itGold;
        })); */

      v.push_back(named_dialog("debug menu", debug_menu));
      }));
    
    clear_debug = [ah, dh] {
      delHook(hooks_drawcell, ah);
      for(auto dhk: dh) delHook(hooks_o_key, dhk);
      clear_debug = [] {};
      };
    }

  else if(currentmap->strict_tree_rules()) {
    int ah = addHook(hooks_drawcell, 50, [] (cell *c, const shiftmatrix& V) {
      string s;
      int id = get_state(c);
      auto label = its(id);
      
      color_t col = 0xFFFFFF + 0x512960 * id;
      if(pointer_indices.count(c->master)) label += " " + index_pointer(c->master);
  
      queuestr(V, 0.3, label, col, 1);
      
      vid.linewidth *= 8;
      queueline(V * C0, V * currentmap->adj(c, 0) * C0, 0xFFFFFFFF, 4);
      vid.linewidth /= 8;
      return false;
      });
    clear_debug = [ah] { delHook(hooks_drawcell, ah); };
    }
  }

int test_rotate_val = 0;

int test_count = 10000;

void test_rules() {
  cell *c = currentmap->gamestart();
  
  int N = isize(treestates);
  if(!N) {
    println(hlog, "no states generated");
    return;
    }

  vector<bignum> howmany(N);
  howmany[0] = 1;
  
  celllister cl(c, 100, test_count, nullptr);
  vector<int> bydist(64, 0);
  for(cell *cc: cl.lst) {
    int d = cl.getdist(cc);
    if(d < 64) bydist[d]++;
    }
  
  vector<int> vals;
  
  vector<string> seq;
  
  for(int iter=0; iter<30; iter++) {
    bignum total;
    for(auto& h: howmany) total += h;
    seq.push_back(total.get_str(100));
    println(hlog, iter, " : ", total.get_str(100), " vs ", bydist[iter]);
    if(bydist[iter] && bydist[iter] != total.approx_ll())
      println(hlog, "ERROR count mismatch");
    vector<bignum> next(N);
    for(int id=0; id<N; id++)
      for(int s: treestates[id].rules) if(s >= 0)
        next[s] += howmany[id];
    howmany = std::move(next);
    }
  
  println(hlog, "sequence: ", seq);
  }

hstream *seq_stream, *test_out;

void list_all_sequences(string tesname) {

  int N = isize(treestates);
  if(!N) {
    println(hlog, "no states generated");
    return;
    }

  for(int i=0; i<N; i++) if(treestates[i].is_root) {
    vector<bignum> howmany(N);
  
    howmany[i] = 1;

    vector<string> seq;

    for(int iter=0; iter<60; iter++) {
      bignum total;
      for(auto& h: howmany) total += h;
      seq.push_back(total.get_str(100));
      vector<bignum> next(N);
      for(int id=0; id<N; id++)
        for(int s: treestates[id].rules) if(s >= 0)
          next[s] += howmany[id];
      howmany = std::move(next);
      }

    println(*seq_stream, (hyperbolic ? "H " : "E "), "FILE ", tesname, ", id ");
    println(*seq_stream, seq);
    }

  seq_stream->flush();
  }

vector<int> gen_actual_seq(int max) {
  celllister cl(cwt.at, 1000, max, nullptr);
  vector<int> dlist(1000, 0);
  for(auto d: cl.dists) dlist[d]++;
  while(dlist.back() == 0) dlist.pop_back();
  return dlist;
  }

string unspace(const string& s) {
  string res;
  for(char c: s) if(c != ' ') res += c;
  return res;
  }

vector<string> seq_as_stringlist() {
  auto& e = cgi.expansion;
  vector<string> res;
  for(int i=0; i<100; i++) res.push_back(unspace(e->get_descendants(i).get_str(1000)));
  return res;
  }

void view_seq_stats() {
  start_game();
  println(hlog, "SEQ rules  ", seq_as_stringlist());
  println(hlog, "SEQ verify ", gen_actual_seq(100000));
  if(true) {
    stop_game();
    reg3::consider_rules = 0;
    start_game();
    println(hlog, "SEQ stupid ", gen_actual_seq(euclid ? 1000 : 100000));
    }
  }

void print_rules();

string rule_name(int r) {
  if(r == DIR_UNKNOWN) return "??";
  else if(r == DIR_LEFT) return "L";
  else if(r == DIR_RIGHT) return "R";
  else if(r == DIR_PARENT) return "P";
  else if(r < -100) return "S"+its(r);
  else return its(r);
  }

void print_rules() {
  for(int i=0; i<isize(treestates); i++) {
    print(hlog, i, ":");
    auto& ts = treestates[i];
    for(auto r: ts.rules)
      print(hlog, " ", rule_name(r));
    if(ts.giver.at)
      print(hlog, " ", ts.giver);
    else
      print(hlog, " (no giver)");
    
    println(hlog, " rg:", ts.giver, ts.is_root ? " [root]" : ts.is_live ? " [live]" : " [dead]");
    }
  }

int draw_which = 0;

void restart_game_on(hrmap *m) {
  stop_game();
  int a = addHook(hooks_newmap, 0, [m] { return m;});
  set_geometry(gArbitrary);
  start_game();
  delHook(hooks_newmap, a);
  }

bool add_header = false;
bool add_labels = true;
string test_stats = "gsmTPcuQthlpf"; // "gsmctTlAhf";

pair<int,int> longest_shortcut() {
  int res = 0;
  int qty = 0;
  for(auto& p: shortcuts) for(auto& v: p) {
    res = max<int>(res, isize(v->pre));
    qty++;
    }
  return {qty, res};
  }

int total_analyzers() {
  return isize(all_analyzers);
  }

int shape_edges() {
  int res = 0;
  for(auto& sh: arb::current.shapes) res += sh.size();
  return res;
  }

int max_edge() {
  int res = 0;
  for(auto& sh: arb::current.shapes) res = max(res, sh.size());
  return res;
  }

int max_valence() {
  int res = 0;
  for(auto& sh: arb::current.shapes) for(auto& va: sh.vertex_valence) res = max(res, va);
  return res;
  }

bool same_shape_at(const arb::shape& s1, const arb::shape& s2, int i) {
  auto N = s1.size();
  for(int j=0; j<N; j++) {
    if(abs(s1.edges[j] - s2.edges[(i+j)%N]) > 1e-6)
      return false;
    if(abs(s1.angles[j] - s2.angles[(i+j)%N]) > 1e-6)
      return false;
    }
  return true;
  }

bool same_shape_inv_at(const arb::shape& s1, const arb::shape& s2, int i) {
  auto N = s1.size();
  for(int j=0; j<N; j++) {
    if(abs(s1.edges[N-1-j] - s2.edges[(i+j)%N]) > 1e-6)
      return false;
    if(abs(s1.angles[(N*2-2-j) % N] - s2.angles[(i+j)%N]) > 1e-6)
      return false;
    }
  return true;
  }

bool same_shape(arb::shape& s1, arb::shape& s2, bool sym) {
  if(s1.size() != s2.size()) return false;
  for(int i=0; i<s1.size(); i++)
    if(same_shape_at(s1, s2, i))
      return true;
  if(sym) for(int i=0; i<s1.size(); i++)
    if(same_shape_inv_at(s1, s2, i))
      return true;
  return false;
  }

int count_different_shapes(bool sym) {
  vector<arb::shape*> dsi;
  for(auto& sh: arb::current.shapes) {
    for(auto& ksh: dsi) if(same_shape(sh, *ksh, sym)) goto next;
    dsi.push_back(&sh);
    next: ;
    }
  return isize(dsi);
  }

int count_vertex_orbits() {
  double t = 0;
  for(auto& sh: arb::current.shapes) {
    for(int i=0; i<sh.cycle_length; i++)
      t += 1. / sh.vertex_period[i];
    }
  int res = int(t + .5);
  if(abs(t - res) > .01) throw hr_exception("count_vertex_orbits error");
  return res;
  }

int count_edge_orbits() {
  int eo = 0;
  for(auto& sh: arb::current.shapes) 
    eo += sh.cycle_length;
  return eo;
  }

vector<ld> normalize_anglelist(vector<ld> v, bool sym) {
  for(auto& va: v) va = int(va * 1000000 + .5);
  vector<ld> res = v;
  for(int r=0; r<2; r++) {
    for(int u=0; u<isize(v); u++) {
      if(v < res) res = v;
      std::rotate(v.begin(), v.begin()+1, v.end());
      }
    if(!sym) break;
    reverse(v.begin(), v.end());
    }
  return res;
  }

int count_different_vertices(bool sym) {
  set<vector<ld>> seen;
  for(auto& sh: arb::current.shapes)
  for(auto& al: sh.vertex_angles) {
    al = normalize_anglelist(al, sym);
    seen.insert(al);
    }
  return isize(seen);
  }

EX long long get_shapelist() {
  long long res = 0;
  for(auto& sh: arb::current.shapes)
    res |= 1ll << min<int>(sh.size(), 61);
  return res;
  }

EX long long get_valence_list() {
  long long res = 0;
  for(auto& sh: arb::current.shapes)
  for(auto& vv: sh.vertex_valence)
    res |= 1ll << min<int>(vv, 61);
  return res;
  }

int count_different_edges() {
  vector<ld> seen;
  for(auto& sh: arb::current.shapes)
  for(auto& e: sh.edges)
    seen.push_back(e);
  sort(seen.begin(), seen.end());
  int res = 1;
  for(int i=1; i<isize(seen); i++)
    if(seen[i] > seen[i-1] + 1e-5) res++;
  return res;
  }

string count_uniform() {
  auto& sh = arb::current.shapes;  
  int N = sh.size();
  vector<int> starts;
  int qty = 0;
  for(auto& s: sh) { starts.push_back(qty); qty += s.cycle_length * 2; }
  // for(auto s: sh) println(hlog, "CSV;clen: ", s.cycle_length);
  // println(hlog, "CSV;size: ", starts, " N=", N);
  
  vector<int> rtile(qty), rvert(qty, -4), redge(qty), rangle(qty); 
  
  for(int i=0; i<N; i++) 
  for(int j=0; j<sh[i].cycle_length; j++) {
    auto c = sh[i].cycle_length;
    int a = starts[i]+2*j;
    int b = a+1;
    int jp = gmod(j+1, c);
    int jn = gmod(j-1, c);
    rtile[a] = starts[i] + 2*jp;
    rtile[b] = starts[i] + 2*jn + 1;
    rangle[a] = rangle[b] = int(sh[i].angles[j] * 100000 + .5);
    redge[a] = int(sh[i].edges[jp] * 100000 + .5);
    redge[b] = int(sh[i].edges[j] * 100000 + .5);
    // rvert[a]: go through the edge and through the tile
    auto co = sh[i].connections[jp];
    // println(hlog, "CSV; for ", tie(i,j), " got ", tie(co.sid, co.eid), "with jp=", jp);
    auto res = starts[co.sid] + 2 * gmod(co.eid, sh[co.sid].cycle_length);
    rvert[a] = res;
    co = sh[i].connections[j];
    res = starts[co.sid] + 2 * gmod(co.eid-1, sh[co.sid].cycle_length) + 1;
    // println(hlog, "CSV; for ", tie(i,j), " got ", tie(co.sid, co.eid), " .. ", sh[i].connections);
    rvert[b] = res;
    }
  
  /*
  println(hlog, "CSV;tile=", rtile);
  println(hlog, "CSV;vert=", rvert);
  println(hlog, "CSV;", rangle);
  println(hlog, "CSV;", redge);
  */

  std::vector<int> eq_class(qty, 0);
  int num_eq_class = 1;
  int last_num_eq_class = 0;

  while (num_eq_class > last_num_eq_class) {
    // println(hlog, "CSV;", eq_class);
    using vertex_data = std::array<int, 6>;
    std::vector<std::pair<vertex_data, int > > data(qty);
    last_num_eq_class = num_eq_class;
    for (int i = 0; i < qty; i++) {
      data[i].first[0] = eq_class[i];
      data[i].first[1] = rangle[i];
      data[i].first[2] = redge[i];
      data[i].first[3] = eq_class[rtile[i]];
      data[i].first[4] = eq_class[rvert[i]];
      data[i].first[5] = eq_class[i^1];
      data[i].second = i;
      }

      sort(data.begin(), data.end());
      eq_class[data[0].second] = 0;

      num_eq_class = 0;

      for (int i = 1; i < qty; i++) {
          if (data[i].first != data[i - 1].first) num_eq_class++;
          eq_class[data[i].second] = num_eq_class;
      }
    num_eq_class++;
    }
  std::vector<int> reps(num_eq_class, -1);
  for(int i=0; i<qty; i++) reps[eq_class[i]] = i;
  
  int num_edges = num_eq_class;
  int num_vert = 0, num_tile = 0, num_vert_sym = 0, num_tile_sym = 0, num_edges_sym = 0, num_edges_ev = 0, num_edges_et = 0, num_edges_ez = 0;

  for(int i: reps) {
    int i1 = reps[eq_class[rvert[i] ^ 1]];
    int i2 = reps[eq_class[rtile[i] ^ 1]];
    int i3 = reps[eq_class[rtile[i1] ^ 1]];
    // println(hlog, "CSV; ", i, " with ", tie(i1, i2, i3));
    if(i >= i1 && i >= i2 && i >= i3) num_edges_sym++;
    if(i >= i1) num_edges_ev++;
    if(i >= i2) num_edges_et++;
    if(i >= i3) num_edges_ez++;
    }

  for (int i: reps) {
    if(rtile[i] >= 0) {
      num_tile++;
      int maxj = i;
      int j = i; while(rtile[j] >= 0) maxj = max(maxj, j), tie(j, rtile[j]) = make_pair(reps[eq_class[rtile[j]]], -1);
      if(maxj&1) num_tile_sym++;
      // println(hlog, "CSV;found ", i);
      }
    if(rvert[i] >= 0) {
      num_vert++;
      int maxj = i;
      int j = i; while(rvert[j] >= 0) maxj = max(maxj, j), tie(j, rvert[j]) = make_pair(reps[eq_class[rvert[j]]], -1);
      if(maxj&1) num_vert_sym++;
      }
    }

  // println(hlog, "CSV;eq_class = ", eq_class);

  // println(hlog, "CSV;", lalign(0, num_tile, ";", num_vert, ";", num_tile_sym, ";", num_vert_sym, ";", num_edges), " = tv/stv/e");

  return lalign(0, num_tile, ";", num_vert, ";", num_edges, ";", num_tile_sym, ";", num_vert_sym, ";", num_edges_sym,";", num_edges_ev, ";", num_edges_et, ";", num_edges_ez);
  }

sem_t sem;
int max_children = 7;
bool forked;

void setup_fork(int m, string fname) {
  max_children = m;
  forked = true;
  sem_init(&sem, true, 1);
  test_out = new fhstream(fname, "wt");
  }

int max_dist() {
  int result = -1;
  tcell* c1 = first_tcell;
  while(c1) {
    if(c1->dist != MYSTERY && c1->dist > result) result = c1->dist;
    c1 = c1->next;
    }
  return result;
  }

void test_current(string tesname) {

  disable_bigstuff = true;
  worst_precision_error = 0;
  stop_game();
  pointer_indices.clear();
  // if(s.find("884-211-045") == string::npos) return;
  start_game();

  string t = testdir + "/";
  for(char c: tesname)
    if(isalnum(c)) t += c;
    else if(c == ',') t += "_";
    else if(c == ' ') t += "__";
    else if(c == '(') t += "op";
    else if(c == '(') t += "cp";
    else if(c == '[') t += "ob";
    else if(c == '[') t += "cb";
    else if(c == '/') t += "__";
  if(euclid) pconf.scale = .1;
  if(hyperbolic) pconf.scale = .95;
  if(sphere) pconf.scale = .5;
  shot::transparent = hyperbolic;

  println(hlog, "TESTING: ", tesname, " AS: ", t);
  indenter ind(2);
  
  if(draw_which & 1) {
    View = Id; shot::take(t+"-old.png");
    }
  
  string status, message;
  bool ok = false;

  // make print_rules() not crash in case of a conversion error
  treestates.clear();

  /* we do not want to include the conversion time */
  if(!arb::in() && WDIM != 3) try {
    arb::convert::convert();
    arb::convert::activate();
    }
  catch(hr_exception& e) {
    println(hlog, "CSV; failed to convert ", tesname);
    return;
    }
  
  if(flags & w_known_structure) {
    dynamicval<flagtype> f(rulegen::flags, sub_rulegen_flags);
    prepare_rules();
    if(!known()) return;
    alt_treestates = treestates;
    pointer_indices.clear();
    }
  
  int tstart = SDL_GetTicks();
  int attempts = 0;
  double max_time = 0, avg_time = 0, variance_time = 0;
  auto begin = clock(); // std::chrono::high_resolution_clock::now();
  auto last = begin;
  try {
    while(!attempts) { // || (clock() < begin + 0.1 * CLOCKS_PER_SEC && attempts < 1000)) {
      
      if(true) {
        rulegen::delete_tmap();
        rulegen::clear_all();
        last = clock();
        rulegen::movecount = 0;
        }
    
      generate_rules();
      auto cur = clock();
      double t = (cur - last) * 1. / CLOCKS_PER_SEC;
      last = cur;
      if(t > max_time) max_time = t;
      avg_time += t;
      variance_time += t * t;
      attempts++;
      }
    status = "ACC";
    message = "OK";
    ok = true;
    rules_known_for = arb::current.name;
    }
  catch(rulegen_surrender& e) {
    println(hlog, "surrender: ** ", e.what());
    status = "SUR";
    message = e.what();
    }
  catch(rulegen_retry& e) {
    println(hlog, "try exceeded: ** ", e.what());
    status = "TRY";
    message = e.what();
    }
  catch(rulegen_failure& e) {
    println(hlog, "error: ** ", e.what());
    status = "ERR";
    message = e.what();
    }
  catch(hr_precision_error& e) {
    println(hlog, "precision error: ** ", e.what());
    status = "PRE";
    message = e.what();
    }
  if(!attempts) { 
    auto cur = clock();
    double t = (cur - last) * 1. / CLOCKS_PER_SEC;
    avg_time += t; variance_time += t*t; max_time = t;
    attempts = 1;
    }
  
  avg_time /= attempts;
  variance_time /= attempts;
  variance_time -= avg_time * avg_time;
  if(attempts > 1) {
    variance_time *= attempts;
    variance_time /= (attempts-1);
    }

  auto end = clock(); // std::chrono::high_resolution_clock::now();

  if(t_origin.size() && (draw_which & 2)) {
    restart_game_on(new hrmap_testproto);
    view_debug();
    View = Id; shot::take(t+"-orig.png");
    clear_debug();
    }

  if(ok && (draw_which & 4)) {
    restart_game_on(new_hrmap_rulegen());
    view_debug();
    View = Id; shot::take(t+"-rule.png");  
    clear_debug();
    }

  tstart = SDL_GetTicks() - tstart;
  
  int qsolid = 0, qcode = 0, qdist = 0;
  
  auto *c = first_tcell;
  while(c) {
    if(c->is_solid) qsolid++;
    if(c->code != MYSTERY) qcode++;
    if(c->dist != MYSTERY) qdist++;
    c = c->next;
    }
  
  vector<ld> areas;
  for(auto& sh: arb::current.shapes) {
    if(hyperbolic) {
      ld s = 0; int i = 0;
      for(auto a: sh.angles) { while(a > 2 * M_PI) a -= 2 * M_PI; while(a<0) a += 2 * M_PI; s += a; i++; }
      areas.push_back((i-2) * M_PI - s);
      }
    else {
      ld s = 0;
      for(int i=0; i<sh.size(); i++) {
        auto v = sh.vertices[i];
        auto v2 = sh.vertices[(i+1)%sh.size()];
        s += (v2[0] - v[0]) * (v[1] + v2[1]);
        }
      s /= 2;
      if(s < 0) s = -s;
      areas.push_back(s);
      }
    }
  sort(areas.begin(), areas.end());

  vector<ld> edgelens;
  for(auto& sh: arb::current.shapes) {
    for(int i=0; i<sh.size(); i++) {
      auto v = sh.vertices[i];
      auto v2 = sh.vertices[(i+1)%sh.size()];
      ld len = hdist(v, v2);
      edgelens.push_back(len);
      }
    }
  sort(edgelens.begin(), edgelens.end());

  if(!test_out) test_out = &hlog;

  if(forked) sem_wait(&sem);
  again:
  print(*test_out, "CSV");

  // easier parsing
  for(auto& ch: message) if(ch == ' ') ch = '_';
  
  #define Out(title,value) if(add_header) print(*test_out, ";", title); else if(add_labels) print(*test_out, " ", title, "=", value);  else print(*test_out, ";", value); break;

  for(char c: test_stats) switch(c) {
    case 'g': Out("geom", euclid ? "E" : hyperbolic ? "H" : "?");
    case 's': Out("status", status);
    case 'm': Out("message", message);
    case 'c': Out("cells", tcellcount);
    case 'u':
      if(flags & w_numerical) {
        Out("precision", worst_precision_error);
        }
      else {
        Out("unis", tunified);
        }
    case 'q': Out("solid", qsolid);
    case 'Q': Out("solid_err", all_solid_errors);
    case 'd': Out("dist", qdist);
    case 'C': Out("code", qcode);
    case 't': Out("try", try_count);
    case 'T': Out("T", tstart / 1000.);
//  case 'P': Out("Tp", std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() / 1000000000.);
    case 'P': Out("Tp", (end-begin) * 1. / CLOCKS_PER_SEC);
    case 'N': Out("attempts", attempts);
    case 'M': Out("maxtime", max_time);
    case 'E': Out("avgtime", avg_time);
    case 'V': Out("vartime", variance_time);
    case 'y': Out("tree", isize(treestates));
    case 'a': Out("amin;amax", lalign(0, areas[0], ";", areas.back()));
    case 'j': Out("emin;emax", lalign(0, edgelens[0], ";", edgelens.back()));
    case 'h': Out("shapes", isize(arb::current.shapes));
    case 'e': Out("edges", shape_edges());
    case 'W': Out("max_valence;max_edge", lalign(0, max_valence(), ";", max_edge()));
    
    case 'D': Out("dshapes;dverts;dedges;bshapes;bverts", lalign(0, count_different_shapes(true), ";", count_different_vertices(true), ";", count_different_edges(), ";", count_different_shapes(false), ";", count_different_vertices(false)));
    case 'O': Out("overts;oedges", lalign(0, count_vertex_orbits(), ";", count_edge_orbits()));
    case 'U': Out("vshapes;vverts;vedges;ushapes;uverts;uedges;xea;xeb;xec", count_uniform());
    case 'L': Out("mirror_rules", arb::current.mirror_rules);
    case 'B': Out("listshape;listvalence", format("%lld;%lld", get_shapelist(), get_valence_list()));
    case 'F': Out("maxdist", max_dist());

    case 'f': Out("file", tesname);
    case 'l': Out("shortcut", longest_shortcut());
    case '3': Out("shqty", longest_shortcut().first);
    case '4': Out("shlong", longest_shortcut().second);
    case 'A': Out("analyzer", total_analyzers());
    case 'H': Out("hard", hard_parents);
    case '1': Out("single", single_live_branches);
    case '2': Out("double", double_live_branches);
    case 'p': Out("premini", states_premini);
    case 'K': Out("movecount", format("%ld", rulegen::movecount));
    }
  println(*test_out);
  test_out->flush();
  if(add_header) { add_header = false; goto again; }

  // for(auto& sh: shortcuts) println(hlog, sh.first, " : ", isize(sh.second), " shortcuts (CSV)");
  
  if(status == "ACC" && !forked) print_rules();
  if(status != "ACC") treestates = alt_treestates;
  /* for(auto& a: analyzers)
    println(hlog, "analyzer ", a.first, " size is ", isize(a.second.spread)); */
  fflush(stdout);
  
  if(seq_stream)
    list_all_sequences(tesname);

  test_out->flush();
  if(forked) sem_post(&sem);
  }

void out_reg() {
  println(hlog, "X ", int(geometry), " ", int(variation), " ", int(gp::param.first), " ", int(gp::param.second));
  }

void test_all_regular(vector<eGeometry> glist) {
  for(eGeometry g: glist) {

    set_geometry(g);
    set_variation(eVariation::pure);
    out_reg();
    
    set_geometry(g);
    set_variation(eVariation::bitruncated);
    out_reg();

    for(int a=1; a<5; a++)
    for(int b=0; b<=a; b++) {
      if(a==1 && b == 0) continue;
      if(a==1 && b == 1 && S3 == 3) continue;
      stop_game();
      set_geometry(g);
      gp::param = {a, b};    
      set_variation(eVariation::goldberg);
      out_reg();
      
      if(S3 == 4 && (geometry == g46 || ((a+b)%2 == 0))) {
        stop_game();
        set_variation(eVariation::unrectified);
        out_reg();
        }

      if(S3 == 3 && (geometry == gOctagon || (a-b)%3 == 0)) {
        stop_game();
        set_variation(eVariation::untruncated);
        out_reg();

        stop_game();
        set_variation(eVariation::warped);
        out_reg();
        }
      }    
    }
  }

void set_dir(string s) {
  testdir = testroot + s;
  system(("mkdir -p " + testdir).c_str());
  }

void set_arcm(eVariation v, string symbol) {
  auto& c = arcm::current;
  c.parse(symbol);
  c.compute_geometry();
  set_geometry(gArchimedean);
  set_variation(v);
  if(c.errors) {
    println(hlog, "ERROR: incorrect Archimedean symbol ", symbol);
    exit(1);
    }
  }

bool set_general(const string& s) {
  stop_game();
  arb::current.name = s;
  if(s[0] == 'X') {
    int a, b, c, d;
    sscanf(s.c_str()+2, "%d%d%d%d", &a, &b, &c, &d);
    geometry = eGeometry(a);
    variation = eVariation(b);
    gp::param = {c, d};
    println(hlog, "parsed ", s, " as ", full_geometry_name());
    }
  else if(s[0] == 'P') set_arcm(eVariation::pure, s.c_str()+2);
  else if(s[0] == 'D') set_arcm(eVariation::dual, s.c_str()+2);
  else if(s[0] == 'B') set_arcm(eVariation::bitruncated, s.c_str()+2);
  else try {
    set_geometry(gArbitrary);
    arb::load(s);
    }
  catch(hr_parse_exception& ex) {
    println(hlog, "failed: ", ex.s);
    return false;
    }
  catch(arb::hr_polygon_error& ex) {
    println(hlog, "poly error");
    return false;
    }
  catch(hr_exception& ex) {
    println(hlog, "other exception");
    return false;
    }
  if(cgflags & qAFFINE) {
    println(hlog, "illegal tessellation found: affine");
    return false;
    }
  if(arb::current.is_star) {
    println(hlog, "illegal tessellation found: star");
    return false;
    }
  if(arb::current.have_tree) {
    println(hlog, "illegal tessellation found: tree");
    return false;
    }
  if(sphere) {
    println(hlog, "illegal tessellation found: spherical");
    return false;
    }
  return true;
  }

void test_from_file(string list) {

  dynamicval<int> df(floorshapes_level);
  if(specialland == laCanvas) floorshapes_level = 1;

  set_dir(list);

  vector<string> filenames;
  std::ifstream is("devmods/rulegen-tests/" + list + ".lst");
  string s;
  while(getline(is, s)) {
    while(s != "" && s[0] == ' ')  s = s.substr(1);
    if(s != "" && s[0] != '#') filenames.push_back(s);
    }
  int trv = test_rotate_val;
  
  int id = 0;
  
  int children = 0;
  fflush(stdout);

  for(const string& s: filenames) {  

    println(hlog, "loading ", s, "... ", id++, "/", isize(filenames));
    println(hlog, "START: ", s); fflush(stdout);
    if(trv) { trv--; id++; continue; }

    if(forked && id > 1) {
      int pid;
      if(children >= max_children) { 
        wait(&pid); children--; 
        }
      if((pid = fork())) children++;
      else goto doit;
      continue;
      }

    doit:
    
    if(set_general(s))
      test_current(s);
    
    println(hlog, "DONE: ", s); fflush(stdout);

    if(forked && id > 1) exit(0);
    if(forked && id == 1) stop_game();
    }
  while(children) { int pid; wait(&pid); children--; }
  }

void rulecat(string list) {

  set_dir(list);

  vector<string> filenames;
  std::ifstream is("devmods/rulegen-tests/" + list + ".lst");
  string s;
  while(getline(is, s)) {
    while(s != "" && s[0] == ' ')  s = s.substr(1);
    if(s != "" && s[0] != '#') filenames.push_back(s);
    }
  
  for(const string& s: filenames) {
    string cat1;
    /*
    if(s[0] == 'X' && s[4] == '1')
      cat1 = "regular";
    else if(s[0] == 'X' && s[4] == '0')
      cat1 = "bitruncated";
    else if(s[0] == 'X' && s[4] == '5' && s[6] == 1 && s[7] == 0)
      cat1 = "regular"; */
    if(0) ;
    else if(s[0] == 'X')
      cat1 = "variations";
    /*
    else if(s[0] == 'P' && is_reg(s))
      cat1 = "regular";
    else if(s[0] == 'D' && is_reg(s))
      cat1 = "regular";
    else if(s[0] == 'B' && is_reg(s))    
      cat1 = "bitruncated";
    */
    else if(s[0] == 'P')
      cat1 = "archimedean";
    else if(s[0] == 'D')
      cat1 = "lavasz";
    else if(s[0] == 'B')
      cat1 = "archibi";
    else {
      int i = 0;
      while(s[i] != '/') i++;
      i++;
      int i1 = i;
      while(s[i] != '/') i++;
      cat1 = s.substr(i1, i-i1);
      if(cat1 == "multitile" && s.substr(i+1, 9) == "polyforms")
        cat1 = "polyforms";
      }
    printf("CSV;%s;%s\n", cat1.c_str(), s.c_str());
    }
  }

void label_all(int i, int mode) {
  queue<tcell*> to_label;
  auto m = dynamic_cast<hrmap_testproto*> (currentmap);  
  set<tcell*> visited;
  
  tcell *next_dist = nullptr;
  int cdist = 0;
  
  auto visit = [&] (tcell *c) {
    if(visited.count(c)) return;
    visited.insert(c);
    to_label.push(c);
    if(!next_dist) next_dist = c;
    };

  visit(m->counterpart[cwt.at->master]);
  while(true) {
    tcell *c = to_label.front();
    if(c == next_dist) { 
      println(hlog, "next distance for ", c);
      next_dist = nullptr;
      cdist++;
      if(cdist == i) break;
      }
    try {
      twalker cw(c, 0);
      if(mode & 4) index_pointer(c);
      if(mode & 1) get_parent_dir(cw);
      if(mode & 2) get_code(cw);
      }
    catch(hr_exception& ex) {    
      }
    to_label.pop();
    for(int i=0; i<c->type; i++) 
      visit(c->cmove(i));
    }
  
  cleanup_protomap();
  }

void seek_label(string s) {
  auto *c = first_tcell;
  while(c) {
    if(pointer_indices.count(c) && index_pointer(c) == s) {
      move_to(c);
      return;
      }
    c = c->next;
    }
  println(hlog, "not found");
  }

void tesgen(string s) {
  set_general(s);
  if(!arb::in()) try {
    arb::convert::convert();
    arb::convert::activate();
    }
  catch(hr_exception& e) {
    println(hlog, "failed to convert ", s);
    }
  }

std::mutex lock;
std::condition_variable cv;
shared_ptr<std::thread> anim_thread;
int state; // 0 = computing, 1 = animating, 2 = finished, 3 = post-finished

struct edgedata {
  int type;
  twalker tw;
  int gtick;
  int ftick;
  };

struct animdata {
  map<tcell*, shiftmatrix> where;
  vector<edgedata> data;
  };

animdata ad;

void seek(twalker tw, int gt) {
  for(auto& d: ad.data) if(d.tw == tw && d.ftick == -1) d.ftick = gt;
  }

void pcurvepoint(hyperpoint h) {
  hyperpoint last = glhr::gltopoint(curvedata.back());
  if(hdist(last, h) > .2) {
    pcurvepoint(mid(h, last));
    pcurvepoint(h);
    }
  else curvepoint(h);
  }

map<tcell*, color_t> marked;

void drawline(twalker tw, color_t col) {
  vid.linewidth *= 3;
  queueline(ad.where[tw.peek()] * C0, ad.where[tw.at] * C0, col, 4, PPR::FLOOR);
  vid.linewidth /= 3;

  auto tw1 = tw+wstep;
  if(tw.at->parent_dir == tw.spin || tw1.at->parent_dir == tw1.spin) {
    shiftmatrix& M1 = ad.where[tw.at];
    shiftmatrix& M2 = ad.where[tw.peek()];
    transmatrix pre = inverse_shift(M1, M2);

    auto& sh = arb::current_or_slided().shapes[tw.at->id];
    auto& sh1 = arb::current_or_slided().shapes[tw1.at->id];
    curvepoint(C0);
    pcurvepoint(normalize(5 * C0 + sh.vertices[tw.spin]));
    pcurvepoint(pre * normalize(5 * C0 + sh1.vertices[(tw1.spin+1)%tw1.at->type]));
    pcurvepoint(pre * C0);
    pcurvepoint(pre * normalize(5 * C0 + sh1.vertices[tw1.spin]));
    pcurvepoint(normalize(5 * C0 + sh.vertices[(tw.spin+1)%tw.at->type]));
    pcurvepoint(C0);
    queuecurve(M1, 0, 0xFF000080, PPR::LINE);
    }
  }

void animate_draw() {
  ad.where.clear();
  for(auto& p: ad.data) {
    if(p.type == 0) ad.where[p.tw.at] = ggmatrix(cwt.at); // shiftless(Id);
    else if(p.type == 1) {
      transmatrix T = arb::get_adj(arb::current_or_slided(), p.tw.peek()->id, (p.tw+wstep).spin, -1, p.tw.spin);
      transmatrix prespin = rspintox(tC0(T));
      T = spintox(tC0(T)) * T;
      ld length = hdist0(tC0(T));
      T = xpush(-length) * T;

      ld age = min(ticks - p.gtick, 1000);
      ld extension = lerp(3, 1.2, age / 1000.);
      color_t col = gradient(0, 0xFFFF, 0, age, 1000);

      if(p.ftick != -1) {
        age = min(ticks - p.ftick, 1000);
        extension = lerp(extension, 1, age / 1000.);
        col = gradient(col, 0xFFFFFFFF, 0, age, 1000);
        }

      ad.where[p.tw.at] = ad.where[p.tw.peek()] * prespin * xpush(length * extension) * T;
      drawline(p.tw, col);
      }
    else if(p.type == 2) {
      auto tw1 = p.tw;
      do {
        seek(tw1, p.gtick);
        seek(tw1+wstep, p.gtick);
        tw1 = tw1 + wstep - 1;
        }
      while(p.tw != tw1);

      ld age = min(ticks - p.gtick, 1000);
      color_t col = gradient(0, 0xFFFFFFFF, 0, age, 1000);
      drawline(p.tw, col);
      }
    else if(p.type == 3 || p.type == 4) {
      for(int i=0; i<isize(ad.data); i++) if(ad.data[i].tw.at == p.tw.at) {
        println(hlog, "found for i = ", i);
        break;
        }
      queuepolyat(ad.where[p.tw.at], cgi.shGem[0], 0xFFFFFFFF, PPR::MONSTER_BODY);
      }
    }
  for(auto& w: ad.where) {
    int id = w.first->id;
    color_t col = colortables['A'][id];
    if(marked.count(w.first)) col = marked[w.first];
    col <<= 8; col |= 0xFF;
    queuepolyat(w.second, cgi.shFullFloor.b[id], col, PPR::WALL);
    addaura(tC0(w.second), 0x800000, 0);
    }
  }

void wait_one_step() {
  std::unique_lock<std::mutex> lk(lock);
  state = 0;
  lk.unlock();
  cv.notify_one();
  lk.lock();
  cv.wait(lk, [] { return state == 1 || state == 2; });
  }

void animate() {
  rulegen::flags |= Flag(15);
  ad.data.clear();

  int i = addHook(hooks_gen_tcell, 100, [] (int i, twalker tw) {
    println(hlog, "hooks_gen_tcell called with i=", i);
    ad.data.emplace_back(edgedata{i, tw, ticks, -1});
    std::unique_lock<std::mutex> lk(lock);
    state = 1;
    lk.unlock();
    cv.notify_one();
    lk.lock();
    cv.wait(lk, [] { return state == 0; });
    rulegen::start_time = ticks;
    });

  state = 0;
  anim_thread = make_shared<std::thread>([]{
    try {
      test_current(arb::current.filename);
      }
    catch(rulegen_failure& e) {
      }
    std::unique_lock<std::mutex> lk(lock);
    println(hlog, "thread finished");
    state = 2;
    lk.unlock();
    cv.notify_one();
    });

  if(1) {
    std::unique_lock<std::mutex> lk(lock);
    cv.wait(lk, [] { return state == 1 || state == 2; });
    }

  int f = addHook(hooks_frame, 100, animate_draw);
  println(hlog, "f = ", f);

  vid.cells_drawn_limit = 0; mapeditor::drawplayer = false; cwt.at->wall = waChasm;
  no_find_player = true;

  int *k = new int;
  *k = addHook(hooks_handleKey, 0, [i, k, f] (int sym, int uni) {
    if(uni == 'y' && state == 2) {
      println(hlog, "on finished");
      anim_thread->join();
      anim_thread = nullptr;
      delHook(hooks_gen_tcell, i);
      println(hlog, "deleting handleKey hook at ", *k);
      int pk = *k;
      delete k;
      delHook(hooks_handleKey, pk);
      delHook(hooks_frame, f);
      println(hlog, "finished");
      return true;
      }
    if(uni == 't') {
      if(state == 2) {
        println(hlog, "finished!");
        return true;
        }
      else if(state == 3) {
        println(hlog, "wrong state");
        return true;
        }
      else {
        println(hlog, "waiting...");
        wait_one_step();
        return true;
        }
      }
    return false;
    });
  }

void animate_to(int i) {
  int steps = 0;
  while(ad.data.back().type != i) {
    if(state != 1) break;
    wait_one_step();
    steps++;
    }
  println(hlog, "steps = ", steps);
  }

void animate_steps(int i) {
  while(i--) {
    if(state != 1) break;
    wait_one_step();
    }
  println(hlog, "after ", i, " steps");
  }

int testargs() {
  using namespace arg;
           
  if(0) ;

  else if(argis("-testproto")) {
    restart_game_on(new hrmap_testproto);
    println(hlog, "creatad a testproto map with ", tcellcount, " cells");
    }
  else if(argis("-test-stats")) {
    shift(); test_stats = args();
    add_header = true;
    add_labels = false;
    }
  else if(argis("-test-stats-label")) {
    shift(); test_stats = args();
    add_header = false;
    add_labels = true;
    }
  else if(argis("-test-this")) {
    PHASEFROM(3);
    try {
      test_current(arb::current.filename);
      }
    catch(rulegen_failure& e) {
      }
    }
  else if(argis("-test-list")) {
    PHASEFROM(3);
    shift(); 
    test_from_file(args());
    }
  else if(argis("-test-fork")) {
    PHASEFROM(3);
    shift(); int i = argi();
    shift(); setup_fork(i, args());
    }
  else if(argis("-rulecat")) {
    PHASEFROM(3);
    shift(); 
    rulecat(args());
    }
  else if(argis("-trv")) {
    shift(); test_rotate_val = argi();
    }
  else if(argis("-ruleflag-sub")) {
    swap(rulegen::flags, sub_rulegen_flags);
    }
  else if(argis("-view-debug"))
    view_debug();
  else if(argis("-print-rules"))
    print_rules();
  else if(argis("-clear-debug"))
    clear_debug();
  
  /* tools for taking screenshots -- after testproto */

  else if(argis("-label-all")) {
    shift(); int q = argi();
    shift(); int mode = argi();
    label_all(q, mode);
    }

  else if(argis("-label-clean")) {
    pointer_indices.clear();
    }

  else if(argis("-label-seek")) {
    shift();
    seek_label(args());
    }

  else if(argis("-color-this")) {
    auto m = dynamic_cast<hrmap_testproto*> (currentmap);  
    shift(); 
    color_t hex = arghex();
    println(hlog, "assigning ", hex, " to ", m->counterpart[cwt.at->master]);
    setdist(cwt.at, 7, nullptr);
    cwt.at->landparam = hex;
    }

  else if(argis("-color-label")) {
    shift(); label_color = arghex();
    }

  else if(argis("-color-wall")) {
    shift(); wall_color = arghex();
    }

  else if(argis("-color-out")) {
    shift(); out_color = arghex();
    }

  else if(argis("-color-tree")) {
    shift(); tree_color = arghex();
    }

  else if(argis("-draw-which")) {
    shift(); draw_which = argi();
    }

  else if(argis("-no-codes")) {
    show_codes = false;
    }

  else if(argis("-no-dist")) {
    show_dist = false;
    }

  else if(argis("-dseek-start")) {
    shift(); move_to(t_origin[argi()]);
    }

  else if(argis("-dseek-giver")) {
    shift(); move_to(treestates[argi()].giver);
    }

  else if(argis("-origin-id")) {
    shift(); origin_id = argi();
    }

  else if(argis("-seqf")) {
    shift();
    seq_stream = new fhstream(args(), "w");
    }

  else if(argis("-tesgen")) {
    shift(); tesgen(args());
    }

  else if(argis("-tes-animate")) {
    animate();
    }

  else if(argis("-tes-animate-to")) {
    shift(); animate_to(argi());
    }

  else if(argis("-tes-animate-steps")) {
    shift(); animate_steps(argi());
    }

  else if(argis("-tes-animate-marked")) {
    shift(); int i = argi();
    shift(); color_t col = arghex();
    marked[ad.data[i].tw.at] = col;
    }

  else if(argis("-veb")) {
    rulegen::rdebug_flags |= 32;
    }
    
  else if(argis("-act-seq")) {
    start_game();
    shift();
    println(hlog, "obtained dlist = ", gen_actual_seq(argi()));
    }

  else if(argis("-seq-stats")) {
    start_game();
    view_seq_stats();
    }

  else if(argis("-dseek")) {
    shift();
    int i = argi();
    if(i >= 0 && i < isize(debuglist)) {
      auto m = dynamic_cast<hrmap_testproto*> (currentmap);  
      cwt = {m->clone(debuglist[i].at)->c7, debuglist[i].spin};
      centerover = cwt.at;
      View = Id;      
      }
    else
      println(hlog, "wrong dseek index");
    }

  else if(argis("-fields")) {
    fieldpattern::fpattern fp(0);
    start_game();
    fp.force_hash = 1;
    fieldpattern::use_quotient_fp = true;
    set<unsigned> seen_hashes;
    addHook(fieldpattern::hooks_solve3, 100, [&] {
      if(seen_hashes.count(fp.hashv)) return;
      seen_hashes.insert(fp.hashv);
      println(hlog, "FOUND p=", fp.Prime, " f=", fp.Field, " hash = ", fp.hashv);
      });
    for(int p=2; p<100; p++) {
      println(hlog, "listing hashes for p=", p);
      fp.Prime = p;
      fp.solve();
      }
    }

  else return 1;
  return 0;
  }

auto testhooks = 
    addHook(hooks_args, 100, testargs);

} }
