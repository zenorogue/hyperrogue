#include "nilrider.h"
#include "statues.cpp"
#include "timestamp.cpp"
#include "levels.cpp"
#include "level.cpp"
#include "planning.cpp"
#include "solver.cpp"

namespace nilrider {

/** is the game paused? */
bool paused = false;

bool planning_mode = false;
bool view_simulation = false;
int simulation_start_tick;

ld aimspeed_key_x = 1, aimspeed_key_y = 1, aimspeed_mouse_x = 1, aimspeed_mouse_y = 1;

vector<string> move_names = { "camera down", "move left", "camera up", "move right", "fine control", "pause", "reverse time", "view simulation", "menu" };

int reversals = 0;

void frame() {
  if(planning_mode && !view_simulation) return;

  shiftmatrix V = ggmatrix(cwt.at);  
  
  curlev->draw_level(V);

  curlev->current.draw_unilcycle(V);  
  }

bool turn(int delta) {
  if(planning_mode && !view_simulation) return false;

  multi::handleInput(0);
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

  if(!paused && !view_simulation && !backing) for(int i=0; i<delta; i++) {
    curlev->history.push_back(curlev->current);
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

void run() {
  cmode = sm::MAP;
  clearMessages();
  dialog::init();
  if(view_simulation && !paused) {
    int ttick = gmod(ticks - simulation_start_tick, isize(curlev->history));
    timer = ttick * 1. / tps;
    curlev->current = curlev->history[ttick];  
    curlev->current.centerview(curlev);
    }
  if(planning_mode && !view_simulation)
    cmode |= sm::SHOWCURSOR;
  if(aimspeed_mouse_x == 0 && aimspeed_mouse_y == 0)
    cmode |= sm::SHOWCURSOR;
  gamescreen(0);
  if(planning_mode && !view_simulation) {
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
  
  if(planning_mode && !view_simulation) {
    for(auto& b: buttons) show_button(b.first, b.second, planmode == b.first ? 0xFFD500 : dialog::dialogcolor);
    show_button(PSEUDOKEY_SIM, "simulation");
    }
  
  bool pause_av = false;
  if(planning_mode && view_simulation) {
    show_button(PSEUDOKEY_SIM, "return");
    pause_av = true;
    show_button(PSEUDOKEY_PAUSE, "pause", paused ? 0xFF0000 : dialog::dialogcolor);
    }
  
  if(!planning_mode) {
    pause_av = true;
    show_button(PSEUDOKEY_PAUSE, "pause", paused ? 0xFF0000 : dialog::dialogcolor);
    }
  
  show_button(PSEUDOKEY_MENU, "menu");

  dialog::add_key_action(PSEUDOKEY_MENU, [] {
    paused = true;
    pushScreen(main_menu);
    });
  if(pause_av) dialog::add_key_action(PSEUDOKEY_PAUSE, [] {
    paused = !paused;
    if(view_simulation && !paused) 
      simulation_start_tick = ticks - timer * tps;
    });
  dialog::add_key_action('-', [] {
    paused = false;
    });
  if(planning_mode) dialog::add_key_action(PSEUDOKEY_SIM, [] {
    view_simulation = !view_simulation;
    paused = false;
    simulation_start_tick = ticks;
    });
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
    if(planning_mode && !view_simulation && curlev->handle_planning(sym, uni)) return;
    dialog::handleNavigation(sym, uni);
    };
  }

int speedlimit = 0;
vector<string> speedlimit_names = {"none", "yellow", "green", "full"};

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
  add_edit(speedlimit);
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
    dialog::add_action([] {
      });
  
    dialog::addItem("save the replay", 'e');
    dialog::add_action([] {
      });
    }
  else {
    dialog::addItem("save this plan", 's');
    dialog::add_action([] {
      });
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

  nilrider_keys();

  check_cgi();
  cgi.prepare_shapes();
  
  init_statues();
  
  curlev->init();

  param_enum(planning_mode, "nil_planning", "nil_planning", false)
    -> editable({{"manual", "control the unicycle manually"}, {"planning", "try to plan the optimal route!"}}, "game mode", 'p');

  param_enum(speedlimit, "nil_speedlimit", "nil_speedlimit", 0)
    -> editable({
      {"no limit", "reach the goals as fast as you wan"}, 
      {"yellow", "your speed must be in the yellow zone to collect"},
      {"green", "your speed must be in the green zone to collect"},
      {"full", "you must fully stop to collect"}
      }, "speed limit", 's');

  rv_hook(hooks_frame, 100, frame);
  rv_hook(shmup::hooks_turn, 100, turn);
  on = true;
  on_cleanup_or_next([] { on = false; });
  pushScreen(run);
  }

auto celldemo = arg::add3("-unilcycle", initialize) + arg::add3("-unilplan", [] { planning_mode = true; }) + arg::add3("-viewsim", [] { view_simulation = true; })
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
