#include "rogueviz.h"
#include <iostream>
#include <thread>

namespace rogueviz {

namespace rwalk {

struct line {
  hyperpoint a;
  hyperpoint b;
  color_t col;
  int timestamp;
  };

struct rwalker {
  cell *at;
  transmatrix ori;
  transmatrix T;
  color_t col;
  int simulated;
  };

map<cell*, vector<line> > drawn;

vector<rwalker> walkers;

ld total_time;

bool draw_rwalk(cell *c, const shiftmatrix& V) {

  vid.linewidth *= 3;
  for(auto p: drawn[c]) if(p.timestamp <= total_time)
    queueline(V * p.a, V * p.b, p.col, 0);
  vid.linewidth /= 3;

  return false;
  }

bool in_video;

ld sim_speed = 5;

int branch_each = 50000;

ld step_size = 0.02;

bool advance_walkers(int delta) {
  if(walkers.empty()) {
    walkers.emplace_back(rwalker{currentmap->gamestart(), Id, Id, 0xFFFFFFFF, 0});
    }
  
  if(in_video) {
    ld t = history::phase / (isize(history::v) - 1);
    total_time = walkers[0].simulated * t;
    }
  else {
    total_time += delta * sim_speed;
    }

  for(int i=0; i<isize(walkers); i++) {
    if(heptdistance(walkers[i].at, centerover) > 7)
      continue;
    while(walkers[i].simulated < total_time) {
      walkers[i].simulated++;
      if(branch_each && rand() % branch_each == 0) {
        walkers.push_back(walkers[i]);
        walkers.back().col = hrandpos() | 0x808080FF;
        walkers[i].col = hrandpos() | 0x808080FF;
        }
      auto& w = walkers[i];
      hyperpoint h = tC0(w.T);
      if(WDIM == 2) {
        w.T = w.T * xspinpush(randd() * TAU, step_size);
        }
      else {
        hyperpoint dir = random_spin() * xtangent(step_size);
        apply_shift_object(w.T, w.ori, dir);        
        }
      fixmatrix(w.T);
      hyperpoint h1 = tC0(w.T);
      drawn[w.at].emplace_back(line{h, h1, w.col, w.simulated});
      virtualRebase(w.at, w.T);
      w.simulated++;
      }
    }
  // centerover = walkers[0].at;
  // View = inverse(walkers[0].T);
  // setdist(centerover, 0, nullptr);
  return false;
  }

void enable();

int args() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-rwalk")) {
    enable();
    }
  else if(argis("-rwparam")) {
    shift_arg_formula(sim_speed);
    shift_arg_formula(step_size);
    shift(); branch_each = argi();
    }

  else return 1;
  return 0;
  }

void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("random walk"), 0xFFFFFFFF, 150, 0);

  dialog::addSelItem("step size", fts(step_size), 'd');
  dialog::add_action([]() {
    dialog::editNumber(step_size, 1e-3, 10, 0.1, 1e-2, "step size", "");
    dialog::scaleLog();
    });

  dialog::addSelItem("steps per millisecond", fts(sim_speed), 'v');
  dialog::add_action([]() {
    dialog::editNumber(sim_speed, 0, 10, 0.1, 5, "steps per millisecond", "");
    });

  dialog::addSelItem("steps per branch", its(branch_each), 'b');
  dialog::add_action([]() {
    dialog::editNumber(branch_each, 100, 1000000, 0.1, 50000, "steps per branch", "");
    dialog::scaleLog();
    });

  dialog::addBoolItem("create an animation", in_video, 'a');
  dialog::add_action([]() {
    in_video = !in_video;
    if(!in_video) {
      total_time = walkers[0].simulated;
      history::on = false;
      }
    if(in_video) {
      history::create(currentmap->gamestart(), walkers[0].at, walkers[0].T);
      models::rotation = rand() % 360;
      }
    });

  dialog::addBack();
  dialog::display();    
  }

void o_key(o_funcs& v) {
  v.push_back(named_dialog("random walk", show));
  }

string cap = "non-Euclidean random walk/";

void rw_slide(vector<tour::slide>& v, string title, string desc, reaction_t t) {
  using namespace tour;
  v.push_back(
    tour::slide{cap + title, 18, LEGAL::NONE | QUICKGEO, desc, 
   
  [t] (presmode mode) {
    setCanvas(mode, '0');

    if(mode == pmStart) {
      tour::slide_backup(mapeditor::drawplayer, false);
      stop_game();
      t();
      start_game();
      enable();
      }

    if(mode == pmKey) {
      drawn.clear();
      walkers.clear();
      total_time = 0;
      }
    }}
    );
  }

void enable() {
  rv_hook(hooks_drawcell, 100, draw_rwalk);
  rv_hook(shmup::hooks_turn, 100, advance_walkers);
  rv_hook(hooks_o_key, 80, o_key);
  rv_hook(hooks_clearmemory, 40, [] () {
    drawn.clear();
    walkers.clear();
    total_time = 0;
    });
  }

auto msc = 
  addHook(hooks_args, 100, args)
+ addHook_rvslides(180, [] (string s, vector<tour::slide>& v) {
  if(s != "mixed") return;
  v.push_back(tour::slide{
    cap+"random walk visualization", 10, tour::LEGAL::NONE | tour::QUICKSKIP,
    "Here we see random walk in various geometries.\n"
    "Press '5' to reset.\n"
    ,
    [] (tour::presmode mode) {
      slide_url(mode, 'y', "YouTube link", "https://www.youtube.com/watch?v=sXNI_i6QZZY");
      }
    });
  rw_slide(v, "Euclidean plane", "In Euclidean plane, the random walk always returns to the neighborhood of the starting point with probability 1.", [] {
    set_geometry(gEuclid);
    set_variation(eVariation::pure);
    sim_speed = 5;
    branch_each = 0;
    step_size = 0.02;
    });
  rw_slide(v, "Euclidean 3-space", 
    "However, in Euclidean 3-space, it does not return.", [] {
    set_geometry(gCubeTiling);
    set_variation(eVariation::pure);
    sim_speed = 5;
    branch_each = 0;
    step_size = 0.02;
    });
  rw_slide(v, "Hyperbolic geometry", "In H2, it does not return, even if we branch from time to time.", [] {
    set_geometry(gNormal);
    set_variation(eVariation::bitruncated);
    sim_speed = 5;
    branch_each = 50000;
    step_size = 0.02;
    });
  /* it works in other geometries too -- exercise left for the reader */
  });

// {4,5} : 10 6 works

}

}