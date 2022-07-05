// Hyperbolic Rogue -- dual-geometry puzzle generator
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file dpgen.cpp
 *  \brief dual geometry puzzle generator
 */

#include "rogueviz.h"

namespace hr {

EX namespace dpgen {

EX bool in;

typedef tuple<cell*, cell*, int> cpos;

map<cpos, int> visited;

vector<cpos> all;
vector<int> last;

void enqueue(cpos p, int d, int li) {
  if(visited.count(p)) return;
  visited[p] = d;
  all.push_back(p);
  last.push_back(li);
  }

void solve(cpos at) {
  visited.clear();
  all.clear();
  last.clear();
  enqueue(at, 0, -1);
  for(int i=0; i<isize(all); i++) {
    auto next = all[i];
    
    auto c0 = get<0>(next);
    auto c1 = get<1>(next);
    auto d  = get<2>(next);
    
    int dist = visited[next];
    
    for(int k=0; k<4; k++) {
      cell *ca0 = c0->move(k);
      if(!ca0) continue;
      if(ca0->wall != waNone) continue;
      
      cell *ca1 = c1->modmove(d+k);
      if(!ca1) continue;
      if(ca1->wall != waNone) continue;
      
      int s = (c1->c.spin((d+k)%4) - c0->c.spin(k)) & 3;
      enqueue(make_tuple(ca0, ca1, s), dist+1, i);
      }
    }
  }

int last_elimit, last_hlimit;

void check();

void launch(int seed, int elimit, int hlimit) {

  /* setup */
  dual::enable();
  stop_game();
  dual::switch_to(0);
  enable_canvas();
  canvas_default_wall = waSea;
  pconf.scale = .5;
  dual::switch_to(1);
  enable_canvas();
  shrand(seed);
  start_game();
  in = true;
  
  cell *c0, *c1;
  dual::switch_to(0);
  vector<cell*> cl0, cl1;
  if(1) {
    c0 = cwt.at;
    celllister cl(cwt.at, elimit, 999, nullptr);
    cl0 = cl.lst;
    for(cell *c: cl.lst) {
      c->wall = waNone, c->land = laCanvas;
      c->landparam = cl.getdist(c) % 2 ? 0x80C080 : 0x409040;
      }
    println(hlog, "c0 size = ", isize(cl.lst));
    }
  dual::switch_to(1);
  if(1) {
    c1 = cwt.at;
    celllister cl(cwt.at, hlimit, 999, nullptr);
    cl1 = cl.lst;
    for(cell *c: cl.lst) {
      c->wall = waNone, c->land = laCanvas;
      #if CAP_ARCM
      int id = arcm::current.tilegroup[arcm::id_of(c->master)];
      color_t yellows[5] = { 0x80C080, 0x80C0C0, 0x8080C0, 0xC080C0, 0xC0C080 };
      c->landparam = yellows[id];
      #endif
      }
    println(hlog, "c1 size = ", isize(cl.lst));
    }
  cpos start = make_tuple(c0, c1, 0);
  solve(start);
  println(hlog, "queue size = ", isize(all));
  
  vector<cell*> clboth;

  pair<cell*, cell*> worst;
  if(1) {
    int wdist = -1, wdcount;
    for(cell* x0: cl0) for(cell *x1: cl1) {
      int x = 9999;
      for(int d=0; d<4; d++) 
        if(visited.count(make_tuple(x0, x1, d))) 
          x = min(x, visited[make_tuple(x0, x1, d)]);
      if(x == 9999) continue;
      if(x > wdist) wdist = x, wdcount = 0;
      if(wdist == x) { wdcount++; if(hrand(wdcount) == 0) worst = {x0, x1}; }
      }
    // println(hlog, "wdist = ", wdist, " x ", wdcount);
    }
  
  clboth = cl0; for(cell *c: cl1) clboth.push_back(c);
  
  while(true) {
    int wdist = -1, wdcount = 0;
    cell *worst_block;
    for(cell *c: clboth) if(c->wall == waNone && c != c0 && c != c1) {
      c->wall = waSea;
      solve(start);
      c->wall = waNone;
      int x = 9999;
      for(int d=0; d<4; d++) 
        if(visited.count(make_tuple(worst.first, worst.second, d))) 
          x = min(x, visited[make_tuple(worst.first, worst.second, d)]);
      if(x == 9999) continue;
      if(x > wdist) wdist = x, wdcount = 0;
      if(wdist == x) { wdcount++; if(hrand(wdcount) == 0) worst_block = c; }
      }
    println(hlog, "wdist = ", wdist, " x ", wdcount);
    if(wdist == -1) break;
    worst_block->wall = waSea;
    }
  
  solve(start);
  
  println(hlog, "worst = ", worst);
  for(int i=0; i<isize(all); i++) if(get<0>(all[i]) == worst.first && get<1>(all[i]) == worst.second) {
    int at = i;
    while(at != -1) {
      // get<0>(all[at])->item = itDiamond;
      // get<1>(all[at])->item = itDiamond;
      at = last[at];
      }
    break;
    }

  worst.first->wall = waOpenPlate;
  worst.second->wall = waOpenPlate;
  rogueviz::rv_hook(dual::hooks_after_move, 100, dpgen::check);
  bool b = gen_wandering;
  rogueviz::on_cleanup_or_next([b] { gen_wandering = b; });
  gen_wandering = false;
  }

struct puzzle {
  string name;
  int seed;
  int el, hl;
  };

vector<puzzle> puzzles = {
  {"easy 1", 1, 3, 2},
  {"easy 2", 2, 3, 2},
  {"easy 3", 5, 3, 2},
  {"medium 1", 7, 3, 3},
  {"medium 2", 11, 3, 3},
  {"hard 1", 1, 4, 3},
  {"hard 2", 1, 3, 4},
  {"hard 3", 1, 3, 5},
  };

EX void check() {
  if(in) {
    int k = dual::currently_loaded;
    dual::switch_to(1-k);
    bool ok = cwt.at->wall == waOpenPlate;
    dual::switch_to(k);
    ok = ok && cwt.at->wall == waOpenPlate;
    if(ok) addMessage("You won!");
    }
  }

bool hide_random = false;
int last_seed = 0;
  
EX void show_menu() {
  cmode = sm::DARKEN;
  gamescreen();
  dialog::init(XLAT("dual geometry puzzles"));
  dialog::addHelp(XLAT("move both characters to marked squares at once!"));
  dialog::addBreak(100);
  char ch = 'a';
  for(auto& p: puzzles) {
    dialog::addItem(p.name, ch++);
    dialog::add_action([p] { 
      launch(last_seed = p.seed, last_elimit = p.el, last_hlimit = p.hl); 
      popScreenAll(); 
      });
    }
  dialog::addBreak(50);
  if(last_elimit && !hide_random) {
    dialog::addItem(XLAT("randomize"), 'r');
    dialog::add_action([] { 
      last_seed = rand() % 1000000;
      launch(last_seed, last_elimit, last_hlimit); 
      popScreenAll();
      });

    dialog::addItem(XLAT("enter seed"), 's');
    dialog::add_action([] { 
      dialog::editNumber(last_seed, 0, 1000000, 1, last_seed, XLAT("seed"), "");
      dialog::reaction_final = [] {
        launch(last_seed, last_elimit, last_hlimit); 
        popScreenAll();
        };
      dialog::extra_options = [] {
        dialog::addSelItem("Euclidean size", its(last_elimit), 'E');
        dialog::add_action([] { popScreen(); dialog::editNumber(last_elimit, 2, 10, 1, 3, XLAT("Euclidean size"), ""); });
        dialog::addSelItem("hyperbolic size", its(last_hlimit), 'H');
        dialog::add_action([] { popScreen(); dialog::editNumber(last_hlimit, 2, 10, 1, 2, XLAT("hyperbolic size"), ""); });
        };
      });
    }
  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  }

#if CAP_COMMANDLINE
auto sbhook = addHook(hooks_args, 100, [] {
  using namespace arg;
           
  if(0) ;
  else if(argis("-dpgen")) {
    shift(); last_seed = argi();
    shift(); last_elimit = argi();
    shift(); last_hlimit = argi();
    launch(last_seed, last_elimit, last_hlimit);
    }
  else if(argis("-d:dpgen")) {
    pushScreen(show_menu);
    }
  else if(argis("-dph")) {
    last_seed = 1;
    last_elimit = 3;
    last_hlimit = 2;
    launch(1, 3, 2);
    hide_random = true;
    pushScreen(show_menu);
    }
  else return 1;
  return 0;
  }) + addHook(hooks_o_key, 91, [] (o_funcs& v) {
    if(in) v.push_back(named_dialog(XLAT("select a puzzle"), show_menu));
    })
  + addHook_rvslides(205, [] (string s, vector<tour::slide>& v) {
      if(s != "mixed") return;
      v.push_back(tour::slide{
        "dual geometry puzzle", 10, tour::LEGAL::NONE | tour::QUICKSKIP | tour::QUICKGEO,
        "Move both characters to marked squares at once!\n"
        ,
        [] (tour::presmode mode) {
          slide_action(mode, 'r', "launch the dual geometry puzzle", [] {
            pushScreen(show_menu);
            });
          }
        });
      })
    ;
#endif

EX }
}