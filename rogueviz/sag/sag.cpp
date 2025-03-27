// RogueViz -- SAG embedder: main file
// Copyright (C) 2011-24 Zeno Rogue, see 'hyper.cpp' for details

#include "sag.h"
#include "cells.cpp"
#include "data.cpp"
#include "functions.cpp"
#include "annealing.cpp"

namespace rogueviz {

namespace sag {

flagtype state;

// std::mt19937 los;

bool auto_repeat;
extern void output_stats();

int sag_ittime = 100;
int ipturn = 100;
bool auto_visualize = true;

void iterate() {
  if(!sagmode) return;
  int t1 = SDL_GetTicks();
  #if CAP_SDL && !CAP_SDL2
  int last = -1;
  #endif
  for(int i=0; i<ipturn; i++) {
    numiter++;
    sag::saiter();
    #if CAP_SDL && !CAP_SDL2
    int q = i * sag_ittime / ipturn;
    if(q > last) { last = 1; SDL_PumpEvents(); }
    #endif
    }
  int t2 = SDL_GetTicks();
  int t = t2 - t1;
  if(t < (sag_ittime+1) / 2) ipturn *= 2;
  else if(t > sag_ittime * 2) ipturn /= 2;
  else ipturn = ipturn * sag_ittime / t;
  print(hlog, format("it %12lld temp %6.4f [2:%8.6f,10:%8.6f,50:%8.6f] cost = %f\n",
    numiter, double(sag::temperature), 
    (double) exp(-2 * exp(-sag::temperature)),
    (double) exp(-10 * exp(-sag::temperature)),
    (double) exp(-50 * exp(-sag::temperature)),
    (double) sag::cost));

  if(auto_visualize) create_viz();
  }
  
bool turn(int delta) {
  if(vizsa_start) {
    if(vizsa_start == -1) vizsa_start = ticks;
    auto t = ticks;
    double d = (t-vizsa_start) / (1000. * vizsa_len);
    if(d > 1 && auto_repeat) {
      optimize_sag_loglik_auto();
      output_stats();
      vizsa_start = -1;
      }
    if(d > 1) sagmode = sagOff;
    else {
      temperature = hightemp - (d*(hightemp-lowtemp));
      sagmode = sagSA;
      }
    }

  iterate();

  return false;
  // shmup::pc[0]->rebase();
  }

void set_inverse() {
  if(method == smMatch) vizflags |= RV_INVERSE_WEIGHT;
  else vizflags &=~ RV_INVERSE_WEIGHT;
  }

void menu() {
  cmode |= sm::MAYDARK | sm::SIDE;
  gamescreen();
  dialog::init("SAG settings");

  dialog::addSelItem(XLAT("temperature"), fts(sag::temperature), 't');
  dialog::add_action([] {
    dialog::editNumber(sag::temperature, sag::lowtemp, sag::hightemp, 1, 0, XLAT("temperature"), "");
    });
  dialog::addSelItem(XLAT("SAG mode"), sag::sagmodes[sag::sagmode], 'm');      
  dialog::add_action([] { sag::sagmode = sag::eSagmode( (1+sag::sagmode) % 3 ); });

  dialog::addSelItem(XLAT("min temperature"), fts(sag::lowtemp), 'i');
  dialog::add_action([] {
    dialog::editNumber(sag::lowtemp, -20, 20, 1, 0, XLAT("min temperature"), "");
    });

  dialog::addSelItem(XLAT("max temperature"), fts(sag::hightemp), 'i');
  dialog::add_action([] {
    dialog::editNumber(sag::hightemp, -20, 20, 1, 0, XLAT("high temperature"), "");
    });

  dialog::addSelItem(XLAT("automatic cycle"), fts(sag::vizsa_len), 'c');
  dialog::add_action([] {
    dialog::editNumber(sag::vizsa_len, 5, 1800, 1, 0, XLAT("automatic cycle"), "");
    });
        
  dialog::addBoolItem(XLAT("automatic"), sag::vizsa_start, 'a');
  dialog::add_action([] { 
    sag::vizsa_start = sag::vizsa_start ? 0 : SDL_GetTicks();
    sag::sagmode = sagOff;
    });

  dialog::addBoolItem_action(XLAT("auto-repeat"), auto_repeat, 'r');

  dialog::addSelItem(XLAT("smoothness"), its(sag_ittime), 's');
  dialog::add_action([] {
    dialog::editNumber(sag_ittime, 0, 1000, 10, 100, XLAT("smoothness"),
      XLAT("How much milliseconds to compute before re-rendering the screen when optimizing in the background. Low values look nicer, but may cause less time to be spent on iterations.")
      );
    });

  dialog::addBoolItem_action(XLAT("auto-visualize"), sag::auto_visualize, 'b');

  dialog::addSelItem(XLAT("SAG method"), method_names[method], 'm');
  dialog::add_action([] {
    method = eSagMethod((method + 1) % method_count);
    prepare_method();
    compute_cost();
    });

  if(method == smMatch) {
    dialog::addSelItem(XLAT("match parameter A"), fts(match_a), 'A');
    dialog::add_action([] {
      dialog::editNumber(match_a, 0, 10, 1, 1, XLAT("match parameter A"), "").reaction = prepare_graph;
      });
    dialog::addSelItem(XLAT("match parameter B"), fts(match_b), 'B');
    dialog::add_action([] {
      dialog::editNumber(match_b, 0, 10, 1, 1, XLAT("match parameter B"), "").reaction = prepare_graph;
      });
    }

  if(method == smLogistic) {
    dialog::addSelItem(XLAT("logistic parameter R"), fts(lgsag.R), 'A');
    dialog::add_action([] {
      dialog::editNumber(match_a, 0, 10, 1, 1, XLAT("logistic parameter R"), "").reaction = prepare_graph;
      });
    dialog::addSelItem(XLAT("logistic parameter T"), fts(lgsag.T), 'B');
    dialog::add_action([] {
      dialog::editNumber(match_b, 0, 10, 1, 1, XLAT("logistic parameter T"), "").reaction = prepare_graph;
      });
    dialog::addItem(XLAT("optimize logistic parameters"), 'O');
    dialog::add_action(optimize_sag_loglik_logistic);
    }

  dialog::addSelItem(XLAT("cost value"), fts(cost), 'X');
  dialog::add_action([] {
    optimize_sag_loglik_auto();
    });

  dialog::addBoolItem_action(XLAT("visualize (sub)cells"), visualize_subcells_on, 'v');

  dialog::display();
  }

void auto_orth(bool set_colors) {
  hyperpoint pmin = C0, pmax = C0;
  shiftmatrix M = ggmatrix(centerover);
  ld maxsize = 0;
  println(hlog, "cellpoints = ", isize(cellpoint), " sagsubcell_point = ", isize(sagsubcell_point), " M = ", M);
  println(hlog, "NLP = ", NLP);

  vector<int> on_cell(isize(sagcells), 0);
  for(auto v: sagid) on_cell[v]++;

  int id = 0;
  for(auto p: sagsubcell_point) {
    hyperpoint ret;
    applymodel(M*p, ret);
    println(hlog, kz(M*p), " -> ", kz(ret));
    for(int i=0; i<3; i++) {
      if(ret[i] < pmin[i]) pmin[i] = ret[i];
      if(ret[i] > pmax[i]) pmax[i] = ret[i];
      }
    ld size = hypot(hypot(ret[0], ret[1]), ret[2]);
    if(size > maxsize && on_cell[id]) maxsize = size;
    id++;
    }
  println(hlog, "pmin = ", pmin, " pmax = ", pmax, " maxsize = ", maxsize);
  pconf.scale = 0.95 / maxsize;
  pconf.clip_min = -maxsize * 3 - 0.1;
  pconf.clip_max = maxsize + 0.1;
  println(hlog, isize(vdata), " vs ", isize(sagsubcell_point));
  if(set_colors) for(int i=0; i<isize(vdata); i++) {
    auto p = sagsubcell_point[sagid[i]];
    hyperpoint ret;
    applymodel(M*p, ret);
    auto& col = vdata[i].cp.color1;
    for(int j=0; j<3; j++) {
      println(hlog, "coloring ", tie(i,j), ret[j], " -> ", ilerp(pmin[j], pmax[j], ret[j]), " -> ", lerp(0, 255, ilerp(pmin[j], pmax[j], ret[j])));
      part(col, j+1) = lerp(0, 255, ilerp(pmin[j], pmax[j], ret[j]));
      }
    vdata[i].cp.color2 = col;
    }
  }

bool use_cells_to_draw;
set<cell*> cells_to_draw;

void autoviz() {
  no_find_player = true;
  smooth_scrolling = true;
  bobbing = false;
  View = Id; NLP = Id; vid.fixed_yz = false;
  models::configure();
  make_actual_view();
  drawthemap();
  if(GDIM == 3) game_keys_scroll = true;
  if(sphere && GDIM == 3) {
    pmodel = mdEquidistant;
    pconf.scale = 0.95;
    }
  if(hyperbolic && GDIM == 2) {
    pmodel = mdDisk;
    pconf.alpha = 1;
    pconf.scale = 0.95;
    }
  if(hyperbolic && GDIM == 3) {
    pmodel = mdDisk;
    pconf.alpha = 1;
    pconf.scale = 0.95;
    }
  if(sol || nil) {
    pmodel = mdLieOrthogonal;
    nisot::geodesic_movement = false;
    auto_orth(false);
    }
  if(sl2) {
    pmodel = mdHorocyclic;
    auto_orth(false);
    }
  if(gproduct) {
    pmodel = mdEquidistant;
    NLP = cspin(1, 2, 75._deg);
    auto_orth(false);
    }
  if(euclid) {
    pmodel = mdDisk;
    pconf.alpha = 1;
    auto_orth(false);
    }
  if(quotient) {
    if(GDIM == 3) {
      pmodel = mdPerspective;
      if(hyperbolic) pmodel = mdDisk;
      frustum_culling = false;
      if(euclid) camera_speed *= 10;
      }
    return;
    }
  cells_to_draw.clear();
  for(auto p: sagcells) cells_to_draw.insert(p.first);
  rv_hook(hooks_do_draw, 100, [] (cell *c, const shiftmatrix& V) { if(!use_cells_to_draw) return 0; return cells_to_draw.count(c) ? 1 : -1; });
  rv_hook(hooks_o_key, 80, [] (o_funcs& v) { v.push_back(named_functionality("switch use_cells_to_draw", [] { use_cells_to_draw = !use_cells_to_draw; })); });
  }

void viz_longpath() {
  int DN = isize(sagid);
  int maxdist = -1;
  int get_i = 0, get_j = 0, count = 0;
  for(int i=0; i<DN; i++)
  for(int j=0; j<DN; j++) {
    auto d = sagdist[sagid[i]][sagid[j]];
    if(S3 >= OINF && sagdist[sagid[i]][0] >= max_sag_dist / 4) continue;
    if(S3 >= OINF && sagdist[sagid[j]][0] >= max_sag_dist / 4) continue;
    if(d > maxdist) {
      maxdist = d;
      count = 0;
      }
    if(d == maxdist) {
      count++; if(hrand(count) == 0) get_i = i, get_j = j;
      }
    }
  println(hlog, "max distance between nodes is ", maxdist, " at ", tie(get_i, get_j), " appearing ", count, " times");
  history::create(sagcells[sagid[get_i]].first, sagcells[sagid[get_j]].first, Id);
  }

void unoptimize() {
  use_cells_to_draw = true;
  drawthemap();
  for(int i=0; i<isize(vdata); i++) {
    vdata[i].m->at = inverse_shift(ggmatrix(cwt.at), ggmatrix(vdata[i].m->base)) * vdata[i].m->at;
    vdata[i].m->base = cwt.at;
    }
  use_cells_to_draw = false;
  shmup::fixStorage();
  rogueviz::rv_change(dont_optimize, true);
  rogueviz::rv_change(frustum_culling, false);
  }

int readArgs() {
#if CAP_COMMANDLINE
  using namespace arg;

  if(0) ;

  else if(argis("-sag-autoviz")) {
    autoviz();
    }
  else if(argis("-sag-longpath")) {
    viz_longpath();
    }
  else if(argis("-sagaviz")) {
    PHASE(3); 
    shift(); sag::auto_visualize = argi();
    }
  else if(argis("-sagviz")) {
    sag::vizsa_start = SDL_GetTicks();
    shift(); sag::vizsa_len = argi();
    }
  else if(argis("-sagsmooth")) {
    shift(); sag::sag_ittime = argi();
    }
  else if(argis("-sag-unoptimize")) {
    unoptimize();
    }

  else return 1;
#endif
  return 0;
  }

void init() {

  if(state & SS_GENERAL) return;
  state |= SS_GENERAL;

  rogueviz::init(RV_GRAPH | RV_WHICHWEIGHT | RV_AUTO_MAXWEIGHT | RV_HAVE_WEIGHT);

  rv_hook(hooks_clearmemory, 100, clear);
  rv_hook(shmup::hooks_turn, 100, turn);
  rv_hook(hooks_drawcell, 100, visualize_subcells);
  rv_hook(rogueviz::hooks_rvmenu, 100, [] {
    dialog::addItem("SAG settings", 's');
    dialog::add_action_push(menu);
    });

  weight_label = "min weight";
  temperature = 0; sagmode = sagOff;
  sag_edge = add_edgetype("SAG edge");
  }

void clear() {
  sagedges.clear();
  visualization_active = false;
  neighbors.clear();
  sagcells.clear();
  ids.clear();
  sagsubcell_point.clear();
  sagsubcell_inv.clear();
  cell_matrix.clear();
  cellpoint.clear();
  sagdist.clear();
  sag_edge = nullptr;
  state = 0;
  }

string cname() {
  if(euclid) return "coord-6.txt";
  if(PURE) return "coord-7.txt";
  return "coord-67.txt";
  }

int ah = addHook(hooks_args, 100, readArgs) + addHook(hooks_clearmemory, 100, clear)
  + addHook_rvslides(120, [] (string s, vector<tour::slide>& v) {
    if(s != "data") return;
    using namespace pres;
    string sagf = "SAG/";
    v.push_back(
      slide{sagf+"Roguelikes", 63, LEGAL::UNLIMITED | QUICKGEO,
        "A visualization of roguelikes, based on discussion on /r/reddit. "
        "See: http://www.roguetemple.com/z/hyper/reddit.php",
        roguevizslide('0', [] () {
          rogueviz::dftcolor = 0x282828FF;

          rogueviz::showlabels = true;
          part(rogueviz::default_edgetype.color, 0) = 181;
          rogueviz::sag::edgepower = 1;
          rogueviz::sag::edgemul = 1;

          gmatrix.clear();
          drawthemap();
          gmatrix0 = gmatrix;

          slide_backup(rogueviz::sag::legacy, true);
          rogueviz::sag::read_weighted(RVPATH "sag/roguelikes/edges.csv");
          rogueviz::readcolor(RVPATH "sag/roguelikes/color.csv");
          rogueviz::sag::load_sag_solution(RVPATH "sag/roguelikes/" + cname());
          })
        }
      );
    v.push_back(slide  {sagf+"Programming languages of GitHub", 64, LEGAL::UNLIMITED | QUICKGEO,
    "A visualization of programming languages.",
    roguevizslide('0', [] () {
      rogueviz::dftcolor = 0x282828FF;

      rogueviz::showlabels = true;
      part(rogueviz::default_edgetype.color, 0) = 128;
      rogueviz::sag::edgepower = .4;
      rogueviz::sag::edgemul = .02;

      gmatrix.clear();
      drawthemap();
      gmatrix0 = gmatrix;

      slide_backup(rogueviz::sag::legacy, true);
      rogueviz::sag::read_weighted(RVPATH "sag/lang/edges.csv");
      rogueviz::readcolor(RVPATH "sag/lang/color.csv");
      rogueviz::sag::load_sag_solution(RVPATH "sag/lang/" + cname());
      if(euclid) rogueviz::legend.clear();
      })
    });

    v.push_back(slide {sagf+"Boardgames", 62, LEGAL::UNLIMITED | QUICKGEO,
        "A visualization of board games, based on discussions on Reddit.",
    roguevizslide('0', [] () {
      rogueviz::dftcolor = 0x282828FF;

      rogueviz::showlabels = true;
      part(rogueviz::default_edgetype.color, 0) = 157;
      rogueviz::sag::edgepower = 1;
      rogueviz::sag::edgemul = 1;

      gmatrix.clear();
      drawthemap();
      gmatrix0 = gmatrix;

      slide_backup(rogueviz::sag::legacy, true);
      rogueviz::sag::read_weighted(RVPATH "sag/boardgames/edges.csv");
      rogueviz::readcolor(RVPATH "sag/boardgames/color.csv");
      rogueviz::sag::load_sag_solution(RVPATH "sag/boardgames/" + cname());
      })
        });

    });

EX }

}

#include "experiments.cpp"
