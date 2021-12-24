/* some extra routines for debugging and testing the rulegen algorithm */

#include "../hyper.h"
#include <fstream>
#include <chrono>

// extra ruleflags:
// 30: do not clear memory

namespace hr {
namespace rulegen {

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
  println(hlog, "try_count = ", try_count, " states = ", isize(treestates), " imp = ", isize(important), " analyzers = ", total_analyzers(), " cell = ", tcellcount);
  cleanup_protomap();
  }

void print_rules();

void irradiate() {
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

void sprawl(twalker c) {
  auto [d, id] = get_code(c);
  twalker cw(c.at, d);
  cur_sprawl = spread(get_analyzer(cw), cw);
  println(hlog, "sprawl result = ", cur_sprawl);
  println(hlog, "code = ", treestates[id].code);
  sprawl_shown.clear();
  for(int i=0; i<isize(cur_sprawl); i++) sprawl_shown[cur_sprawl[i].at] = i;
  if(isize(cur_sprawl) == isize(old_sprawl) && old_sprawl_id < isize(treestates)) {
    auto& oldcode = treestates[old_sprawl_id].code.second;
    auto& newcode = treestates[id].code.second;
    int q = isize(cur_sprawl);
    debuglist = {};
    for(int i=0; i<q; i++)
      if(oldcode[i] != newcode[i]) {
        println(hlog, "index: ", i, " old: ", oldcode[i], " new: ", newcode[i], " at ", old_sprawl[i], " vs ", cur_sprawl[i]);
        debuglist.push_back(old_sprawl[i]);
        debuglist.push_back(cur_sprawl[i]);
        }
    }
  old_sprawl = cur_sprawl;
  old_sprawl_id = id;
  }

void debug_menu() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);
  auto m = dynamic_cast<hrmap_testproto*> (currentmap);
  dialog::init("debug menu");
  
  dialog::addItem("sprawl", 's');
  dialog::add_action([m] {
    sprawl(twalker(m->counterpart[centerover->master], 0));
    });

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
    parent_debug = true;
    try {
      twalker cw(c, 0);
      get_parent_dir(cw);
      }
    catch(rulegen_failure& f) {
      println(hlog, "catched: ", f.what());
      }
    parent_debug = false;
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
    gamescreen(0);
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

  dialog::addItem("clean data and parents", 'C');
  dialog::add_action(clean_parents);

  dialog::addItem("irradiate", 'i');
  dialog::add_action(irradiate);

  dialog::addItem("irradiate x10", 'I');
  dialog::add_action([] { for(int i=0; i<10; i++) irradiate(); });

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
        string code = (tc->code == MYSTERY ? "?" : its(tc->code));
        if(show_dist) label = label + "/" + code;
        else label = code;
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

void list_sequence() {  

  int N = isize(treestates);
  if(!N) {
    println(hlog, "no states generated");
    return;
    }

  vector<bignum> howmany(N);
  howmany[0] = 1;
  
  vector<string> seq;
  
  for(int iter=0; iter<30; iter++) {
    bignum total;
    for(auto& h: howmany) total += h;
    seq.push_back(total.get_str(100));
    vector<bignum> next(N);
    for(int id=0; id<N; id++)
      for(int s: treestates[id].rules) if(s >= 0)
        next[s] += howmany[id];
    howmany = std::move(next);
    }
  
  println(hlog, "sequence: ", seq);
  }

void print_rules();

string rule_name(int r) {
  if(r == DIR_UNKNOWN) return "??";
  else if(r == DIR_LEFT) return "L";
  else if(r == DIR_RIGHT) return "R";
  else if(r == DIR_PARENT) return "P";
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
  for(auto& p: shortcuts) for(auto& v: p.second) {
    res = max<int>(res, isize(v->pre));
    qty++;
    }
  return {qty, res};
  }

int longest_analyzer() {
  int res = 0;
  for(auto& a: analyzers) res = max(res, isize(a.second.spread));
  return res;
  }

int total_analyzers() {
  int res = 0;
  for(auto& a: analyzers) res += isize(a.second.spread);
  return res;
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
  if(!arb::in()) try {
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

  again:
  print(hlog, "CSV");

  // easier parsing
  for(auto& ch: message) if(ch == ' ') ch = '_';

  #define Out(title,value) if(add_header) print(hlog, ";", title); else if(add_labels) print(hlog, " ", title, "=", value);  else print(hlog, ";", value); break;

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
    case 'f': Out("file", tesname);
    case 'l': Out("shortcut", longest_shortcut());
    case '3': Out("shqty", longest_shortcut().first);
    case '4': Out("shlong", longest_shortcut().second);
    case 'A': Out("analyzer", longest_analyzer());
    case 'H': Out("hard", hard_parents);
    case '1': Out("single", single_live_branches);
    case '2': Out("double", double_live_branches);
    case 'p': Out("premini", states_premini);
    case 'K': Out("movecount", format("%ld", rulegen::movecount));
    }
  println(hlog);
  fflush(stdout);
  if(add_header) { add_header = false; goto again; }

  // for(auto& sh: shortcuts) println(hlog, sh.first, " : ", isize(sh.second), " shortcuts (CSV)");
  
  /*if(status == "ACC")*/ print_rules();
  if(status != "ACC") treestates = alt_treestates;
  /* for(auto& a: analyzers)
    println(hlog, "analyzer ", a.first, " size is ", isize(a.second.spread)); */
  fflush(stdout);
  list_sequence();

  fflush(stdout);
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
  
  for(const string& s: filenames) {  
    println(hlog, "loading ", s, "... ", id++, "/", isize(filenames));
    if(trv) { trv--; id++; continue; }
    if(set_general(s))
      test_current(s);
    }
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
  else if(argis("-rulecat")) {
    PHASEFROM(3);
    shift(); 
    rulecat(args());
    }
  else if(argis("-trv")) {
    shift(); test_rotate_val = argi();
    }
  else if(argis("-ruleflag")) {
    shift();
    rulegen::flags ^= Flag(argi());
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

  else if(argis("-tesgen")) {
    shift(); string s = args();
    set_general(s);
    if(!arb::in()) try {
      arb::convert::convert();
      arb::convert::activate();
      }
    catch(hr_exception& e) {
      println(hlog, "failed to convert ", s);
      }
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
    
  else return 1;
  return 0;
  }

auto testhooks = 
    addHook(hooks_args, 100, testargs);

} }
