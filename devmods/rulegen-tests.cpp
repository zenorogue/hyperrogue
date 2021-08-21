/* some extra routines for debugging and testing the rulegen algorithm */

#include "../hyper.h"
#include <fstream>
#include <chrono>

namespace hr {
namespace rulegen {

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
    return clone(t_origin[0]);
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

void debug_menu() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);
  auto m = dynamic_cast<hrmap_testproto*> (currentmap);
  dialog::init("debug menu");
  
  dialog::addItem("sprawl", 's');
  dialog::add_action([m] {
    tcell *c = m->counterpart[centerover->master];
    auto [d, id] = get_code(c);
    twalker cw(c, d);
    auto res = spread(get_analyzer(cw), cw);
    println(hlog, "sprawl result = ", res);
    sprawl_shown.clear();
    for(int i=0; i<isize(res); i++) sprawl_shown[res[i].at] = i;
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
      get_parent_dir(c);
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
      dialog::add_action([dw,m] {
        cwt = cellwalker(m->clone(dw.at)->c7, dw.spin, dw.mirrored);
        centerover = cwt.at;
        View = Id;
        });
      }
    dialog::display();
    });
  
  dialog::addItem("print rules", 'P');
  dialog::add_action(print_rules);

  dialog::addItem("clean data", 'c');
  dialog::add_action(clean_data);

  dialog::addItem("clean data and parents", 'C');
  dialog::add_action(clean_parents);

  dialog::addItem("irradiate", 'i');
  dialog::add_action(irradiate);

  dialog::addItem("name", 'n');
  dialog::add_action([m] { println(hlog, "name = ", index_pointer(m->counterpart[cwt.at->master])); });

  dialog::display();
  }  

void view_debug() {
  auto m = dynamic_cast<hrmap_testproto*> (currentmap);
  if(m) {
    int ah = addHook(hooks_drawcell, 50, [m] (cell *c, const shiftmatrix& V) {
      tcell *tc = m->counterpart[c->master];
      
      string s;
      auto label = (tc->code == MYSTERY ? "?" : its(tc->code)) + "/" + (tc->dist == MYSTERY ? "?" : its(tc->dist));
      
      color_t col = 0xFFFFFF + 0x512960 * tc->code;
  
      if(pointer_indices.count(tc)) label += " " + index_pointer(tc);
      
      queuestr(V, 0.4, label, col, 1);
      if(tc->parent_dir >= 0 && tc->parent_dir < tc->type) {
        vid.linewidth *= 8;
        queueline(V * C0, V * currentmap->adj(c, tc->parent_dir) * C0, 0xFFFFFFFF);
        vid.linewidth /= 8;
        }
        
      if(sprawl_shown.count(tc))
        queuepoly(V, cgi.shDisk, 0xFF0000FF);
      
      for(int i=0; i<tc->type; i++) if(!tc->move(i)) {
        vid.linewidth *= 8;
        hyperpoint h1 = currentmap->get_corner(c, (i+1)%tc->type);
        hyperpoint h2 = currentmap->get_corner(c, i);
        queueline(V * h1, V * h2, 0xFF0000FF);
        vid.linewidth /= 8;
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
      print(hlog, " ", get_aid(ts.giver));
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
string test_stats = "gsmctTlAhf";

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

void test_current(string tesname) {
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

  int tstart = SDL_GetTicks();
  auto begin = std::chrono::high_resolution_clock::now();
  try {
    generate_rules();
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

  auto end = std::chrono::high_resolution_clock::now();

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
    ld s = 0; int i = 0;
    for(auto a: sh.angles) { while(a > 2 * M_PI) a -= 2 * M_PI; while(a<0) a += 2 * M_PI; s += a; i++; }
    areas.push_back((i-2) * M_PI - s);
    }
  sort(areas.begin(), areas.end());

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
    case 'u': Out("unis", tunified);
    case 'q': Out("solid", qsolid);
    case 'd': Out("dist", qdist);
    case 'C': Out("code", qcode);
    case 't': Out("try", try_count);
    case 'T': Out("T", tstart / 1000.);
    case 'P': Out("Tp", std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() / 1000000000.);
    case 'y': Out("tree", isize(treestates));
    case 'a': Out("amin;amax", lalign(0, areas[0], ";", areas.back()));
    case 'h': Out("shapes", isize(arb::current.shapes));
    case 'f': Out("file", tesname);
    case 'l': Out("shortcut", longest_shortcut());
    case '3': Out("shqty", longest_shortcut().first);
    case '4': Out("shlong", longest_shortcut().second);
    case 'A': Out("analyzer", longest_analyzer());
    case 'H': Out("hard", hard_parents);
    case '1': Out("single", single_live_branches);
    case '2': Out("double", double_live_branches);
    case 'p': Out("premini", states_premini);
    }
  println(hlog);
  fflush(stdout);
  if(add_header) { add_header = false; goto again; }

  // for(auto& sh: shortcuts) println(hlog, sh.first, " : ", isize(sh.second), " shortcuts (CSV)");

  print_rules();
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

void test_from_file(string list) {

  set_dir("devmods/rulegen-tests/" + list);

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
    if(trv) { trv--; id++; continue; }
    stop_game();
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
      println(hlog, "loading ", s, "... ", id++, "/", isize(filenames));
      arb::load(s);
      }
    catch(hr_parse_exception& ex) {
      println(hlog, "failed: ", ex.s);
      continue;
      }
    catch(arb::hr_polygon_error& ex) {
      println(hlog, "poly error");
      continue;
      }
    catch(hr_exception& ex) {
      println(hlog, "other exception");
      continue;
      }
    if(cgflags & qAFFINE) { 
      println(hlog, "illgeal tessellation found: affine"); continue; 
      }
    if(sphere) { 
      println(hlog, "illgeal tessellation found: spherical"); continue; 
      }
    test_current(s);
    }
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
  else if(argis("-trv")) {
    shift(); test_rotate_val = argi();
    }
  else if(argis("-ruleflag")) {
    shift();
    rulegen::flags ^= Flag(argi());
    }
  else if(argis("-view-debug"))
    view_debug();
  else if(argis("-print-rules"))
    print_rules();
  else if(argis("-clear-debug"))
    clear_debug();

  else return 1;
  return 0;
  }

auto testhooks = 
    addHook(hooks_args, 100, testargs);

} }
