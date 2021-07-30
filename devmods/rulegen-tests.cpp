/* some extra routines for debugging and testing the rulegen algorithm */

#include "../hyper.h"
#include <fstream>

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
    return arb::get_adj(arb::current_or_slided(), shvid(h->c7), dir, -1, h->c.move(dir) ? h->c.spin(dir) : -1);
    }

  int shvid(cell *c) override {
    if(!counterpart.count(c->master)) {
      /* may happen while handling floorshapes */
      return arb::id_of(c->master);
      }
    auto cc = counterpart[c->master];
    return cc->id;
    }
  
  bool strict_tree_rules() { return true; }
  };

reaction_t clear_debug = [] {};

map<tcell*,int> sprawl_shown;

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

      return false;
      });
    
    vector<int> dh;

    dh.push_back(addHook(hooks_o_key, 15, [m] (o_funcs& v) {  
      v.push_back(named_functionality("sprawl", [m] {
        tcell *c = m->counterpart[centerover->master];
        auto [d, id] = get_code(c);
        twalker cw(c, d);
        auto res = spread(get_analyzer(cw), cw);
        println(hlog, "sprawl result = ", res);
        sprawl_shown.clear();
        for(int i=0; i<isize(res); i++) sprawl_shown[res[i].at] = i;
        }));
      }));
    
    for(auto dw: debuglist)
      dh.push_back(addHook(hooks_o_key, 10, [m,dw] (o_funcs& v) {  
        v.push_back(named_functionality(lalign(0, "go to ", dw), [dw,m] {
          cwt = cellwalker(m->clone(dw.at)->c7, dw.spin, dw.mirrored);
          centerover = cwt.at;
          View = Id;
          }));
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
  else if(r == DIR_MULTI_GO_LEFT) return "ML";
  else if(r == DIR_MULTI_GO_RIGHT) return "MR";
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
    
    println(hlog, " rg:", ts.giver, ts.is_live ? " [live]" : " [dead]");
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

void test_current() {
  stop_game();
  pointer_indices.clear();
  string s = arb::in() ? arb::current.filename : full_geometry_name();
  // if(s.find("884-211-045") == string::npos) return;
  start_game();

  string t = testdir;
  for(char c: s) 
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

  println(hlog, "TESTING: ", s, " AS: ", t);
  indenter ind(2);
  
  if(draw_which & 1) {
    View = Id; shot::take(t+"-old.png");
    }
  
  string status;
  bool ok = false;

  int tstart = SDL_GetTicks();
  try {
    generate_rules();
    status = "ACC;OK";
    ok = true;
    }
  catch(rulegen_surrender& e) {
    println(hlog, "surrender: ** ", e.what());
    status = s0 + "SUR;\"" + e.what() + "\"";
    }
  catch(rulegen_retry& e) {
    println(hlog, "try exceeded: ** ", e.what());
    status = s0 + "TRY;\"" + e.what() + "\"";
    }
  catch(rulegen_failure& e) {
    println(hlog, "error: ** ", e.what());
    status = s0 + "ERR;\"" + e.what() + "\"";
    }

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
  
  println(hlog, "after rulegen: tcellcount = ", tcellcount, "-", tunified, " qsolid = ", qsolid, " qdist = ", qdist, " qcode = ", qcode, " radius = ", prepare_around_radius, " try_count = ", try_count, " ticks = ", tstart);

  println(hlog, "CSV;", euclid ? "E" : hyperbolic ? "H" : "?", ";", status, ";", tcellcount, ";", tunified, ";", qsolid, ";", qdist, ";", qcode, ";", prepare_around_radius, ";", try_count, ";", tstart, ";", isize(treestates), ";", arb::current.filename);
  fflush(stdout);

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

void test_from_file(string list) {

  set_dir("devmods/rulegen-tests/" + list);

  vector<string> filenames;
  std::ifstream is("devmods/rulegen-tests/" + list + ".lst");
  string s;
  while(getline(is, s)) {
    if(s != "") filenames.push_back(s);
    }
  int trv = test_rotate_val;
  
  int id = 0;
  
  for(const string& s: filenames) {  
    if(trv) { trv--; id++; continue; }
    stop_game();
    set_geometry(gArbitrary);
    try {
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
    test_current();
    }
  }

void test_all_geoms(int i) {
  if(i&1) {
    set_dir("regular/");
    test_all_regular({gNormal, gOctagon, g45, g46, g47, gEuclid, gEuclidSquare});
    }
  }

int testargs() {
  using namespace arg;
           
  if(0) ;

  else if(argis("-testproto")) {
    restart_game_on(new hrmap_testproto);
    println(hlog, "creatad a testproto map with ", tcellcount, " cells");
    }
  else if(argis("-test-this")) {
    PHASEFROM(3);
    try {
      test_current();
      }
    catch(rulegen_failure& e) {
      }
    }
  else if(argis("-test-list")) {
    PHASEFROM(3);
    shift(); 
    test_from_file(args());
    }
  else if(argis("-test-all")) {
    PHASEFROM(3);
    shift(); 
    test_all_geoms(argi());
    start_game();
    }
  else if(argis("-trv")) {
    shift(); test_rotate_val = argi();
    }
  else if(argis("-view-debug"))
    view_debug();
  else if(argis("-clear-debug"))
    clear_debug();

  else return 1;
  return 0;
  }

auto testhooks = 
    addHook(hooks_args, 100, testargs);

} }
