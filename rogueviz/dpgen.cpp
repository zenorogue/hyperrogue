// Hyperbolic Rogue -- dual-geometry puzzle generator
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file dpgen.cpp
 *  \brief dual geometry puzzle generator
 */

#include "rogueviz.h"

namespace hr {

EX namespace dpgen {

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
int last_seed = 0;

void check();

void show_menu();

const struct puzzle *current_puzzle;

int orig_cheat;

void launch(int seed, int elimit, int hlimit) {

  /* setup */
  dual::disable();
  dual::enable();
  stop_game();
  dual::switch_to(0);
  enable_canvas(); ccolor::set_random(0);
  canvas_default_wall = waSea;
  pconf.scale = .5;
  dual::switch_to(1);
  enable_canvas(); ccolor::set_random(0);
  shrand(seed);
  start_game();
  
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
  orig_cheat = cheater;

  rv_change(gen_wandering, false);
  rv_hook(hooks_o_key, 91, [] (o_funcs& v) {
    v.push_back(named_dialog(XLAT("select a puzzle"), show_menu));
    });
  current_puzzle = nullptr;
  }

struct puzzle {
  string name;
  int seed;
  int el, hl;
  string achievement;
  };

bool restricted;
void puzzle_restrict();

void launch_puzzle(const puzzle& p) {
  launch(last_seed = p.seed, last_elimit = p.el, last_hlimit = p.hl);
  popScreenAll();
  if(restricted) pushScreen(puzzle_restrict);
  clearMessages();
  addMessage("Welcome to Dual Geometry Puzzle!");
  current_puzzle = &p;
  }

vector<puzzle> puzzles = {
  {"easy 1", 1, 3, 2, "DP-EASY"},
  {"easy 2", 2, 3, 2, "DP_EASY"},
  {"easy 3", 5, 3, 2, "DP_EASY"},
  {"medium 1", 7, 3, 3, "DP-MEDIUM"},
  {"medium 2", 11, 3, 3, "DP-MEDIUM"},
  {"hard 1", 1, 4, 3, "DP-HARD"},
  {"hard 2", 1, 3, 4, "DP_HARD"},
  {"hard 3", 1, 3, 5, "DP_HARD"},
  };

void launch_sample_puzzle() {
  launch_puzzle(puzzles[0]);
  }

EX void check() {
  int k = dual::currently_loaded;
  dual::switch_to(1-k);
  bool ok = cwt.at->wall == waOpenPlate;
  dual::switch_to(k);
  ok = ok && cwt.at->wall == waOpenPlate;
  if(ok) {
    addMessage("You won!");
    #if RVCOL
    if(cheater == orig_cheat) rv_achievement(current_puzzle->achievement);
    #endif
    }
  }

bool hide_random = false;

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
      launch_puzzle(p);
      });
    }
  dialog::addBreak(50);
  if(last_elimit && !hide_random) {
    dialog::addItem(XLAT("randomize"), 'r');
    dialog::add_action([] { 
      last_seed = rand() % 1000000;
      launch(last_seed, last_elimit, last_hlimit); 
      });

    dialog::addItem(XLAT("enter seed"), 's');
    dialog::add_action([] { 
      auto& di = dialog::editNumber(last_seed, 0, 1000000, 1, last_seed, XLAT("seed"), "");
      di.reaction_final = [] {
        launch(last_seed, last_elimit, last_hlimit); 
        };
      di.extra_options = [] {
        dialog::addSelItem("Euclidean size", its(last_elimit), 'E');
        dialog::add_action([] { popScreen(); dialog::editNumber(last_elimit, 2, 10, 1, 3, XLAT("Euclidean size"), ""); });
        dialog::addSelItem("hyperbolic size", its(last_hlimit), 'H');
        dialog::add_action([] { popScreen(); dialog::editNumber(last_hlimit, 2, 10, 1, 2, XLAT("hyperbolic size"), ""); });
        };
      });
    }
  dialog::addBreak(50);
  dialog::addBack();

  if(restricted) {
    dialog::addItem(XLAT("RogueViz settings"), 'S');
    dialog::add_action([] { pushScreen(showSettings); });

    dialog::addItem(XLAT("back to RogueViz menu"), 'm');
    dialog::add_action([] { quitmainloop = true; });
    }

  dialog::display();
  }

EX void puzzle_restrict() {
  cmode = sm::NORMAL | sm::CENTER;
  gamescreen();

  keyhandler = [] (int sym, int uni) {
    if(DEFAULTNOR(sym)) handlePanning(sym, uni);
    handle_movement(sym, uni);

    if(sym == 'v' && DEFAULTNOR(sym))
      pushScreen(show_menu);

    if(sym == PSEUDOKEY_MENU || sym == SDLK_ESCAPE)
      pushScreen(show_menu);
    };
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