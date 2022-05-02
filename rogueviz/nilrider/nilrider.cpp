#include "nilrider.h"
#include "statues.cpp"
#include "timestamp.cpp"
#include "levels.cpp"
#include "level.cpp"
#include "planning.cpp"
#include "solver.cpp"
#include "save.cpp"

namespace nilrider {

/** is the game paused? */
bool paused = false;

bool planning_mode = false;
bool view_replay = false;
int simulation_start_tick;

ld aimspeed_key_x = 1, aimspeed_key_y = 1, aimspeed_mouse_x = 1, aimspeed_mouse_y = 1;

vector<string> move_names = { "camera down", "move left", "camera up", "move right", "fine control", "pause", "reverse time", "view simulation", "menu" };

int reversals = 0;

void frame() {
  if(planning_mode && !view_replay) return;

  shiftmatrix V = ggmatrix(cwt.at);  
  
  curlev->draw_level(V);

  curlev->current.draw_unilcycle(V);  
  }

bool turn(int delta) {
  if(planning_mode && !view_replay) return false;

  multi::get_actions();
  auto& a = multi::actionspressed;
  auto& la = multi::lactionpressed;

  ld mul = 1;
  if(a[16+4]) mul /= 5;
  if(a[16+3] && !paused) curlev->current.heading_angle -= aimspeed_key_x * mul * delta / 1000.;
  if(a[16+1] && !paused) curlev->current.heading_angle += aimspeed_key_x * mul * delta / 1000.;
  if(a[16+2] && !paused) min_gfx_slope -= aimspeed_key_y * mul * delta / 1000.;
  if(a[16+0] && !paused) min_gfx_slope += aimspeed_key_y * mul * delta / 1000.;

  curlev->current.heading_angle -= aimspeed_mouse_x * mouseaim_x * mul;
  min_gfx_slope += aimspeed_mouse_y * mouseaim_y * mul;

  #if CAP_VR
  if(vrhr::active()) {
    curlev->current.heading_angle -= aimspeed_mouse_x * vrhr::vraim_x * mul * delta / 400;
    min_gfx_slope -= aimspeed_mouse_y * vrhr::vraim_y * mul * delta / 400;
    }
  #endif

  if(min_gfx_slope < -90*degree) min_gfx_slope = -90*degree;
  if(min_gfx_slope > +90*degree) min_gfx_slope = +90*degree;

  bool backing = false;

  if(a[16+6]) {
    if(!la[16+6]) reversals++;
    if(planning_mode)
      simulation_start_tick += 2*delta;
    else for(int i=0; i<delta; i++) {
      if(isize(curlev->history) > 1) {
        backing = true;
        curlev->history.pop_back();
        curlev->current = curlev->history.back();
        timer = isize(curlev->history) * 1. / tps;
        }
      else {
        reversals = 0;
        timer = 0;
        }
      }
    }

  if(!paused && !view_replay && !backing) for(int i=0; i<delta; i++) {
    curlev->history.push_back(curlev->current);
    curlev->current.be_consistent();
    bool b = curlev->current.tick(curlev);
    if(b) timer += 1. / tps;
    else curlev->history.pop_back();
    }
 
  if(!paused) curlev->current.centerview(curlev);
  return false;
  }

void main_menu();

#define PSEUDOKEY_PAUSE 2511
#define PSEUDOKEY_SIM 2512

void toggle_replay() {
  view_replay = !view_replay;
  paused = false;
  simulation_start_tick = ticks;
  if(!view_replay && !planning_mode) {
    paused = true;
    curlev->current = curlev->history.back();
    timer = isize(curlev->history) * 1. / tps;
    }
  }

void run() {
  cmode = sm::MAP;
  clearMessages();
  dialog::init();
  if(view_replay && !paused) {
    int ttick = gmod(ticks - simulation_start_tick, isize(curlev->history));
    timer = ttick * 1. / tps;
    curlev->current = curlev->history[ttick];  
    curlev->current.centerview(curlev);
    }
  if(planning_mode && !view_replay)
    cmode |= sm::SHOWCURSOR;
  if(aimspeed_mouse_x == 0 && aimspeed_mouse_y == 0)
    cmode |= sm::SHOWCURSOR;
  gamescreen(0);
  if(planning_mode && !view_replay) {
    curlev->draw_planning_screen();
    if(!holdmouse) {
      auto t0 = SDL_GetTicks();
      while(SDL_GetTicks() < t0 + 100) {
        if(!curlev->simulate()) break;
        }  
      }
    }
  curlev->current.draw_instruments(timer);
  
  if(paused && !planning_mode) {
    displayButton(current_display->xcenter, current_display->ycenter, mousing ? XLAT("paused -- click to unpause") : XLAT("paused -- press p to continue"), 'p', 8);
    }
  
  int x = vid.fsize;
  auto show_button = [&] (int c, string s, color_t col = dialog::dialogcolor) {
    if(displayButtonS(x, vid.yres - vid.fsize, s, col, 0, vid.fsize))
      getcstat = c;
    x += textwidth(vid.fsize, s) + vid.fsize;
    };
  
  if(planning_mode && !view_replay) {
    for(auto& b: buttons) show_button(b.first, b.second, planmode == b.first ? 0xFFD500 : dialog::dialogcolor);
    show_button(PSEUDOKEY_SIM, "simulation");
    }
  
  bool pause_av = view_replay || !planning_mode;
  if(pause_av) {
    show_button(PSEUDOKEY_SIM, planning_mode ? "return" : "replay", (view_replay  && !planning_mode) ? 0xFF0000 : dialog::dialogcolor);
    show_button(PSEUDOKEY_PAUSE, "pause", paused ? 0xFF0000 : dialog::dialogcolor);
    }
  
  show_button(PSEUDOKEY_MENU, "menu");

  dialog::add_key_action(PSEUDOKEY_MENU, [] {
    paused = true;
    pushScreen(main_menu);
    });
  if(pause_av) dialog::add_key_action(PSEUDOKEY_PAUSE, [] {
    paused = !paused;
    if(view_replay && !paused)
      simulation_start_tick = ticks - timer * tps;
    });
  dialog::add_key_action('-', [] {
    paused = false;
    });
  dialog::add_key_action(PSEUDOKEY_SIM, toggle_replay);
  dialog::display();

  char* t = multi::scfg.keyaction;
  for(int i=1; i<512; i++) {
    auto& ka = dialog::key_actions;
    if(t[i] == 16+5) ka[i] = ka[PSEUDOKEY_PAUSE];
    if(t[i] == 16+7) ka[i] = ka[PSEUDOKEY_SIM];
    if(t[i] == 16+8) ka[i] = ka[PSEUDOKEY_MENU];
    }
  
  keyhandler = [] (int sym, int uni) {
    if(paused) handlePanning(sym, uni);
    if(planning_mode && !view_replay && curlev->handle_planning(sym, uni)) return;
    dialog::handleNavigation(sym, uni);
    };
  }

void pick_level() {
  clearMessages();
  dialog::init(XLAT("select the track"), 0xC0C0FFFF, 150, 100);
  for(auto l: all_levels) {
    dialog::addItem(l->name, l->hotkey);
    dialog::add_action([l] {
      curlev = l;
      recompute_plan_transform = true;
      l->init();
      popScreen();
      });
    }
  dialog::addBreak(100);
  dialog::addBack();
  dialog::display();
  }

void pick_game() {
  clearMessages();
  dialog::init(XLAT("how do you want to play?"), 0xC0C0FFFF, 150, 100);
  dialog::addSelItem("selected track", curlev->name, 't');
  dialog::add_action_push(pick_level);
  dialog::addInfo(curlev->longdesc);
  dialog::addBreak(100);
  add_edit(planning_mode);
  dialog::addBreak(100);
  dialog::addBack();
  dialog::display();
  }

void settings() {
  dialog::init(XLAT("settings"), 0xC0C0FFFF, 150, 100);
  add_edit(aimspeed_key_x);
  add_edit(aimspeed_key_y);
  add_edit(aimspeed_mouse_x);
  add_edit(aimspeed_mouse_y);
  add_edit(whrad);
  add_edit(whdist);
  add_edit(min_gfx_slope);
  dialog::addItem("configure keys", 'k');
  dialog::add_action_push(multi::get_key_configurer(1, move_names, "Nilrider keys"));
  dialog::addItem("RogueViz settings", 'r');
  dialog::add_key_action('r', [] {
    pushScreen(showSettings);
    });
  dialog::addBreak(100);
  dialog::display();
  dialog::addBack();
  }

bool deleting = false;

template<class T, class U> void replays_of_type(vector<T>& v, const U& loader) {
  int i = 0;
  for(auto& r: v) {
    dialog::addItem(r.name, 'a');
    dialog::add_action([&v, i, loader] {
      if(deleting) {
        dialog::push_confirm_dialog(
          [&, i] { v.erase(v.begin() + i); save(); },
          "Are you sure you want to delete '" + v[i].name + "'?"
          );
        }
      else loader(v[i]);
      });
    i++;
    }
  }

void replays() {
  dialog::init(XLAT(planning_mode ? "saved plans" : "replays"), 0xC0C0FFFF, 150, 100);
  if(!planning_mode) replays_of_type(curlev->manual_replays, [] (manual_replay& r) {
    view_replay = false;
    curlev->history.clear();
    auto& current = curlev->current;
    current = curlev->start;
    for(auto h: r.headings) {
      current.heading_angle = int_to_heading(h);
      curlev->history.push_back(current);
      if(!current.tick(curlev)) break;
      }
    toggle_replay();
    popScreen();
    });
  if(planning_mode) replays_of_type(curlev->plan_replays, [] (plan_replay& r) {
    view_replay = false;
    curlev->history.clear();
    curlev->plan = r.plan;
    popScreen();
    });
  dialog::addBoolItem_action("delete", deleting, 'X');
  dialog::addBack();
  dialog::display();
  }

void pop_and_push_replays() {
  deleting = false;
  popScreen();
  pushScreen(replays);
  }

reaction_t on_quit = [] { exit(0); }; 

void main_menu() {
  clearMessages();
  dialog::init(XLAT("Nil Rider"), 0xC0C0FFFF, 150, 100);

  dialog::addItem("continue", 'c');
  dialog::add_action(popScreen);

  if(!planning_mode) {
    dialog::addItem("restart", 'r');
    dialog::add_action([] {
      curlev->current = curlev->start;
      timer = 0;
      paused = false;
      popScreen();
      });
  
    dialog::addItem("view the replay", 'v');
    dialog::add_action(toggle_replay);
  
    dialog::addItem("save the replay", 's');
    dialog::add_action([] {
      vector<int> ang;
      for(auto& h: curlev->history) ang.push_back(heading_to_int(h.heading_angle));
      curlev->manual_replays.emplace_back(manual_replay{new_replay_name(), std::move(ang)});
      save();
      });

    dialog::addItem("load a replay", 'l');
    dialog::add_action(pop_and_push_replays);
    }
  else {
    dialog::addItem("save this plan", 's');
    dialog::add_action([] {
      curlev->plan_replays.emplace_back(plan_replay{new_replay_name(), curlev->plan});
      save();
      });

    dialog::addItem("load a plan", 'l');
    dialog::add_action(pop_and_push_replays);
    }

  dialog::addItem("change track or game settings", 't');
  dialog::add_action_push(pick_game);

  dialog::addItem("change other settings", 'o');
  dialog::add_action_push(settings);

  dialog::addItem("quit", 'q');
  dialog::add_action([] { 
    on_quit();
    });
  
  dialog::display();
  }

bool on;

void change_default_key(int key, int val) {
  char* t = multi::scfg.keyaction;
  t[key] = val;
  set_saver_default(t[key]);
  }

void nilrider_keys() {
  for(int i=0; i<512; i++)
    if(multi::scfg.keyaction[i] >= 16 && multi::scfg.keyaction[i] < 32)
      change_default_key(i, 0);
  change_default_key('s', 16 + 0);
  change_default_key('a', 16 + 1);
  change_default_key('w', 16 + 2);
  change_default_key('d', 16 + 3);
  change_default_key(SDLK_LCTRL, 16 + 4);
  change_default_key('p', 16 + 5);
  change_default_key('b', 16 + 6);
  change_default_key('r', 16 + 7);
  change_default_key('v', 16 + 8);
  }

void initialize() {
  load();
  nilrider_keys();

  check_cgi();
  cgi.prepare_shapes();
  
  init_statues();
  
  curlev->init();

  param_enum(planning_mode, "nil_planning", "nil_planning", false)
    -> editable({{"manual", "control the unicycle manually"}, {"planning", "try to plan the optimal route!"}}, "game mode", 'p');

  rv_hook(hooks_frame, 100, frame);
  rv_hook(shmup::hooks_turn, 100, turn);
  on = true;
  on_cleanup_or_next([] { on = false; });
  pushScreen(run);
  }

auto celldemo = arg::add3("-unilcycle", initialize) + arg::add3("-unilplan", [] { planning_mode = true; }) + arg::add3("-viewsim", [] { view_replay = true; })
  + arg::add3("-oqc", [] { on_quit = popScreenAll; })
  + arg::add3("-nilsolve", [] { curlev->solve(); })
  + addHook(hooks_configfile, 100, [] {
    param_f(aimspeed_key_x, "nilrider_key_x")
    ->editable(-5, 5, 0.1, "navigation sensitivity (keyboard)", "press Left/Right to navigate (lCtrl to fine-tune)", 'n');
    param_f(aimspeed_key_y, "nilrider_key_y")
    ->editable(-5, 5, 0.1, "camera sensitivity (keyboard)", "press Up/Down to set the camera angle (lCtrl to fine-tune)", 'c');
    param_f(aimspeed_mouse_x, "nilrider_mouse_x")
    ->editable(-5, 5, 0.1, "navigation sensitivity (mouse/vr)", "move mouse Left/Right to navigate (lCtrl to fine-tune)", 'N');
    param_f(aimspeed_mouse_y, "nilrider_mouse_y")
    ->editable(-5, 5, 0.1, "camera sensitivity (mouse/vr)", "move mouse Up/Down to set the camera angle (lCtrl to fine-tune)", 'C');
    param_f(whrad, "nilrider_radius")
    ->editable(0, 0.5, 0.01, "wheel radius", "note: this parameter is just visual, it does not affect the physics in any way", 'w');
    param_f(whdist, "nilrider_dist")
    ->editable(0, 5, 0.05, "camera distance", "how far is the unicycle from the camera", 'd');
    param_f(min_gfx_slope, "min_gfx_slope")
    ->editable(-90*degree, 90*degree, degree, "min camera slope", "affected by up/down", 'm');
    })
  + arg::add3("-fullsim", [] {
    /* for animations */
    popScreenAll();
    rv_hook(anims::hooks_anim, 100, [] {
      int ttick = ticks % isize(curlev->history);
      timer = ttick * 1. / tps;
      curlev->current = curlev->history[ttick];  
      curlev->current.centerview(curlev);
      anims::moved();
      });
    }) + arg::add3("-unillevel", [] {
      arg::shift();
      for(auto l: all_levels) if(appears(l->name, arg::args())) curlev = l;
      if(on) curlev->init();
      })
    + arg::add3("-simplemodel", [] {
      nisot::geodesic_movement = false;
      pmodel = mdPerspective;
      pconf.rotational_nil = 0;
      });


}
