#include "nilrider.h"
#include "statues.cpp"
#include "timestamp.cpp"
#include "levels.cpp"
#include "level.cpp"
#include "planning.cpp"

namespace nilrider {

/** is the game paused? */
bool paused = false;

bool planning_mode = false;
bool view_simulation = false;
int simulation_start_tick;

void frame() {
  if(planning_mode && !view_simulation) return;

  shiftmatrix V = ggmatrix(cwt.at);  
  
  curlev->draw_level(V);

  curlev->current.draw_unilcycle(V);  
  }

bool turn(int delta) {
  if(planning_mode && !view_simulation) return false;
  Uint8 *keystate = SDL_GetKeyState(NULL);
  if(keystate[SDLK_RIGHT] && !paused) curlev->current.heading_angle -= delta / 1000.;
  if(keystate[SDLK_LEFT] && !paused) curlev->current.heading_angle += delta / 1000.;
  
  if(keystate[SDLK_UP] && !paused) min_gfx_slope -= delta / 1000.;
  if(keystate[SDLK_DOWN] && !paused) min_gfx_slope += delta / 1000.;

  curlev->current.heading_angle += mouseaim_x;
  min_gfx_slope += mouseaim_y;

  #if CAP_VR
  if(vrhr::active()) {
    curlev->current.heading_angle += vrhr::vraim_x * delta / 400;
    min_gfx_slope -= vrhr::vraim_y * delta / 400;
    }
  #endif

  if(min_gfx_slope < -90*degree) min_gfx_slope = -90*degree;
  if(min_gfx_slope > +90*degree) min_gfx_slope = +90*degree;

  if(!paused && !view_simulation) for(int i=0; i<delta; i++) {    
    curlev->history.push_back(curlev->current);
    bool b = curlev->current.tick(curlev);
    if(b) timer += 1. / tps;
    else curlev->history.pop_back();
    }
 
  if(!paused) curlev->current.centerview(curlev);
  return false;
  }

void main_menu();

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
  auto show_button = [&] (char c, string s, color_t col = dialog::dialogcolor) {
    if(displayButtonS(x, vid.yres - vid.fsize, s, col, 0, vid.fsize))
      getcstat = c;
    x += textwidth(vid.fsize, s) + vid.fsize;
    };
  
  if(planning_mode && !view_simulation) {
    for(auto& b: buttons) show_button(b.first, b.second, planmode == b.first ? 0xFFD500 : dialog::dialogcolor);
    show_button('s', "simulation");
    }
  
  if(planning_mode && view_simulation) {
    show_button('s', "return");
    show_button('p', "pause", paused ? 0xFF0000 : dialog::dialogcolor);
    }
  
  if(!planning_mode) {
    show_button('p', "pause", paused ? 0xFF0000 : dialog::dialogcolor);
    }
  
  show_button('v', "menu");  

  dialog::add_key_action('v', [] { 
    paused = true;
    pushScreen(main_menu);
    });
  dialog::add_key_action('p', [] {
    paused = !paused;
    if(view_simulation && !paused) 
      simulation_start_tick = ticks - timer * tps;
    });
  dialog::add_key_action('-', [] {
    paused = false;
    });
  dialog::add_key_action('b', [] {
    if(planning_mode)
      simulation_start_tick += 500;
    else {
      for(int i=0; i<500; i++) if(!curlev->history.empty()) curlev->history.pop_back();
      curlev->current = curlev->history.back();
      timer = isize(curlev->history) * 1. / tps;
      }
    });
  if(planning_mode) dialog::add_key_action('s', [] {
    view_simulation = !view_simulation;
    paused = false;
    simulation_start_tick = ticks;
    });
  dialog::display();
  
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

void initialize() {

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
