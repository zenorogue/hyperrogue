#if NILRIDER
#define CUSTOM_CAPTION "Nil Rider 1.0"
#define MAXMDIM 4
#define CAP_INV 0
#define CAP_COMPLEX2 0
#define CAP_EDIT 0
#define CAP_TEXTURE 1
#define CAP_BT 0
#define CAP_SOLV 0
#define CAP_THREAD 0
#define CAP_RUG 0
#define CAP_SVG 0
#define CAP_TOUR 0
#define CAP_IRR 0
#define CAP_CRYSTAL 0
#define CAP_ARCM 0
#define CAP_HISTORY 0
#define CAP_STARTANIM 0
#define CAP_SAVE 0
#define CAP_TRANS 0

#ifdef BWEB
#include "../../hyperweb.cpp"
#else
#include "../../hyper.cpp"
#endif
#include "../simple-impossible.cpp"
#include "../rogueviz.cpp"
#endif

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
bool loaded_or_planned = false;

void frame() {
  if(planning_mode && !view_replay) return;

  shiftmatrix V = ggmatrix(cwt.at);  
  
  curlev->draw_level(V);

  curlev->current.draw_unilcycle(V);  
  }

bool crash_sound = true;
bool running;
bool backing;

/* land for music */

eLand music_nilrider = eLand(400);
eLand music_nilrider_back = eLand(401);
eLand music_nilrider_paused = eLand(402);
eLand music_nilrider_planning = eLand(403);
eLand music_nilrider_nonrunning = eLand(404);

void sync_music(eLand l) {
  musicpos[music_nilrider] = zgmod(curlev->current.timer * 1000, musiclength[music_nilrider]);
  musicpos[music_nilrider_back] = zgmod(-curlev->current.timer * 1000, musiclength[music_nilrider_back]);
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

  if(min_gfx_slope < -90._deg) min_gfx_slope = -90._deg;
  if(min_gfx_slope > +90._deg) min_gfx_slope = +90._deg;
  
  backing = false;

  if(a[16+6]) {
    if(!la[16+6]) reversals++;
    if(planning_mode)
      simulation_start_tick += 2*delta;
    else for(int i=0; i<delta; i++) {
      if(isize(curlev->history) > 1) {
        backing = true;
        curlev->history.pop_back();
        curlev->current = curlev->history.back();
        crash_sound = true;
        }
      else {
        reversals = 0;
        loaded_or_planned = false;
        crash_sound = true;
        }
      }
    }

  if(!paused && !view_replay && !backing) {

    auto t = curlev->current.collected_triangles;
    bool fail = false;

    for(int i=0; i<delta; i++) {
      curlev->history.push_back(curlev->current);
      curlev->current.be_consistent();
      bool b = curlev->current.tick(curlev);
      running = b;
      if(!b) {
        curlev->history.pop_back();
        fail = true;
        break;
        }
      }

    if(t != curlev->current.collected_triangles)
      playSound(cwt.at, "pickup-gold");

    if(fail && crash_sound) {
      char ch = curlev->mapchar(curlev->current.where);
      if(ch == 'r') {
        playSound(cwt.at, "closegate");
        crash_sound = false;
        }
      if(ch == '!') {
        playSound(cwt.at, "seen-air");
        crash_sound = false;
        }
      }
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
    }
  }

void run() {
  cmode = sm::PANNING | sm::NORMAL;
  clearMessages();
  dialog::init();
  if(view_replay && !paused) {
    int ttick = gmod(ticks - simulation_start_tick, isize(curlev->history));
    curlev->current = curlev->history[ttick];  
    curlev->current.centerview(curlev);
    }
  if(planning_mode && !view_replay)
    cmode |= sm::SHOWCURSOR;
  if(aimspeed_mouse_x == 0 && aimspeed_mouse_y == 0)
    cmode |= sm::SHOWCURSOR;
  gamescreen();
  if(planning_mode && !view_replay) {
    curlev->draw_planning_screen();
    if(!holdmouse) {
      auto t0 = SDL_GetTicks();
      while(SDL_GetTicks() < t0 + 100) {
        if(!curlev->simulate()) break;
        }  
      }
    }
  curlev->current.draw_instruments(curlev);
  
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
    if(curlev->current.timer) paused = true;
    game_keys_scroll = true;
    pushScreen(main_menu);
    });
  if(pause_av) dialog::add_key_action(PSEUDOKEY_PAUSE, [] {
    paused = !paused;
    game_keys_scroll = true;
    if(view_replay && !paused)
      simulation_start_tick = ticks - curlev->current.timer * tps;
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

void clear_path(level *l) {
  l->history.clear();
  l->current = l->start;
  l->history.push_back(l->start);
  paused = false;
  reversals = 0;
  loaded_or_planned = false;
  crash_sound = true;
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
      clear_path(l);
      popScreen();
      });
    }
  dialog::addBreak(100);
  dialog::addBack();
  dialog::display();
  }

void pick_game() {
  clearMessages();
  dialog::init();
  poly_outline = 0xFF;
  dialog::addBigItem(curlev->name, 't');
  dialog::addBreak(50);
  dialog::addHelp(curlev->longdesc);

  int gid = 0;
  for(auto& g: curlev->goals) {
    dialog::addBreak(50);
    auto man = curlev->records[0][gid];
    auto plan = curlev->records[1][gid];
    if(man && plan)
      dialog::addInfo("manual: " + format_timer(man) + " planning: " + format_timer(plan), g.color);
    else if(man)
      dialog::addInfo("manual: " + format_timer(man), g.color);
    else if(plan)
      dialog::addInfo("planning: " + format_timer(plan), g.color);
    else
      dialog::addInfo("goal not obtained:", g.color);
    dialog::addBreak(50);
    dialog::addHelp(g.desc);
    gid++;
    }

  dialog::addBreak(100);
  dialog::addItem("change the track", 't');
  dialog::add_action_push(pick_level);
  dialog::addBreak(100);
  add_edit(planning_mode);
  dialog::addBack();
  dialog::display();
  }

void nil_set_geodesic() {
  pmodel = mdGeodesic;
  nisot::geodesic_movement = true;
  popScreen();
  }

void nil_set_perspective() {
  pmodel = mdPerspective;
  nisot::geodesic_movement = false;
  pconf.rotational_nil = 0;
  }

void nil_projection() {
  dialog::init(XLAT("projection of Nil"), 0xC0C0FFFF, 150, 100);
  dialog::addBoolItem("geodesics", pmodel == mdGeodesic, 'g');
  dialog::add_action([] { popScreen(); nil_set_geodesic(); });
  dialog::addInfo("In this mode, the light is assumed to travel along the geodesics (the shortest paths in Nil).");
  dialog::addBreak(100);
  dialog::addBoolItem("constant direction", pmodel == mdPerspective, 'c');
  dialog::add_action([] { popScreen(); nil_set_perspective(); });
  dialog::addInfo("In this mode, the light is assumed to travel along the lines of constant direction.");
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
  add_edit(stepped_display);
  dialog::addItem("projection", 'P');
  dialog::add_action_push(nil_projection);
  dialog::addItem("configure keys", 'k');
  dialog::add_action_push(multi::get_key_configurer(1, move_names, "Nilrider keys"));

  #if CAP_AUDIO
  add_edit(effvolume);
  add_edit(musicvolume);
  #endif

  #if CAP_VR
  vrhr::enable_button();
  vrhr::reference_button();
  #endif

  dialog::addItem("RogueViz settings", 'r');
  dialog::add_key_action('r', [] {
    pushScreen(showSettings);
    });

  #if CAP_FILES && !ISWEB
  dialog::addItem("save the current config", 's');
  dialog::add_action([] {
    dynamicval<eGeometry> g(geometry, gNormal);
    saveConfig();
    });
  #endif

  dialog::addBreak(100);
  dialog::addBack();
  dialog::display();
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

#if CAP_SAVE
void replays() {
  dialog::init(XLAT(planning_mode ? "saved plans" : "replays"), 0xC0C0FFFF, 150, 100);
  if(!planning_mode) replays_of_type(curlev->manual_replays, [] (manual_replay& r) {
    view_replay = false;
    curlev->history.clear();
    auto& current = curlev->current;
    current = curlev->start;
    loaded_or_planned = true;
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
#endif

reaction_t on_quit = [] { exit(0); }; 

void restart() {
  clear_path(curlev);
  }

void main_menu() {
  clearMessages();
  poly_outline = 0xFF;
  dialog::init(XLAT("Nil Rider"), 0xC0C0FFFF, 150, 100);

  dialog::addItem("continue", 'c');
  dialog::add_action(popScreen);

  if(!planning_mode) {
    dialog::addItem("restart", 'r');
    dialog::add_action([] {
      clear_path(curlev);
      popScreen();
      });
  
    dialog::addItem("view the replay", 'v');
    dialog::add_action(toggle_replay);
  
    #if CAP_SAVE
    dialog::addItem("save the replay", 's');
    dialog::add_action([] {
      vector<int> ang;
      for(auto& h: curlev->history) ang.push_back(heading_to_int(h.heading_angle));
      curlev->manual_replays.emplace_back(manual_replay{new_replay_name(), std::move(ang)});
      save();
      });

    dialog::addItem("load a replay", 'l');
    dialog::add_action(pop_and_push_replays);
    #endif
    }
  else {
    #if CAP_SAVE
    dialog::addItem("save this plan", 's');
    dialog::add_action([] {
      curlev->plan_replays.emplace_back(plan_replay{new_replay_name(), curlev->plan});
      save();
      });

    dialog::addItem("load a plan", 'l');
    dialog::add_action(pop_and_push_replays);
    #endif
    }

  dialog::addItem("track / mode / goals", 't');
  dialog::add_action_push(pick_game);

  dialog::addItem("change settings", 'o');
  dialog::add_action_push(settings);

  #if CAP_VIDEO
  dialog::addItem("record video", 'v');
  dialog::add_action([] {
    dialog::openFileDialog(anims::videofile, XLAT("record to video file"),
      ".mp4", [] {
        anims::period = isize(curlev->history);
        anims::noframes = anims::period * 60 / 1000;
        int a = addHook(anims::hooks_anim, 100, [] {
          int ttick = ticks % isize(curlev->history);
          curlev->current = curlev->history[ttick];
          curlev->current.centerview(curlev);
          anims::moved();
          });
        int af = addHook(hooks_frame, 100, [] {
          int ttick = ticks % isize(curlev->history);
          curlev->current = curlev->history[ttick];
          if(planning_mode && !view_replay) curlev->draw_planning_screen();
          });
        bool b = anims::record_video_std();
        delHook(anims::hooks_anim, a);
        delHook(hooks_frame, af);
        return b;
        });
    });
  #endif

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
  #if CAP_CONFIG
  set_saver_default(t[key]);
  #endif
  }

void nilrider_keys() {
  for(int i=0; i<512; i++)
    if(multi::scfg.keyaction[i] >= 16 && multi::scfg.keyaction[i] < 32)
      change_default_key(i, 0);
  change_default_key('s', 16 + 0);
  change_default_key('a', 16 + 1);
  change_default_key('w', 16 + 2);
  change_default_key('d', 16 + 3);
  #if CAP_SDL2
  change_default_key(SDL_SCANCODE_LCTRL, 16 + 4);
  #else
  change_default_key(SDLK_LCTRL, 16 + 4);
  #endif
  change_default_key('p', 16 + 5);
  change_default_key('b', 16 + 6);
  change_default_key('r', 16 + 7);
  change_default_key('v', 16 + 8);
  }

bool nilrider_music(eLand& l) {
  if(planning_mode && !view_replay)
    l = music_nilrider_planning;
  else if(paused)
    l = music_nilrider_paused;
  else if(!running)
    l = music_nilrider_nonrunning;
  else if(backing)
    l = music_nilrider_back;
  else l = music_nilrider;
  return false;
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

  param_enum(stepped_display, "stepped_display", "stepped_display", false)
    -> editable({{"smooth", "ride on a smooth surface"}, {"blocky", "makes slopes more visible -- actual physics are not affected"}}, "game mode", 's');

  param_i(nilrider_tempo, "nilrider_tempo");
  param_i(nilrider_shift, "nilrider_shift");

  rv_hook(hooks_frame, 100, frame);
  rv_hook(shmup::hooks_turn, 100, turn);
  rv_hook(hooks_resetGL, 100, cleanup_textures);
  rv_hook(hooks_music, 100, nilrider_music);
  rv_hook(hooks_sync_music, 100, sync_music);
  on = true;
  on_cleanup_or_next([] { on = false; });
  pushScreen(run);
  }

void initialize_all() {
  showstartmenu = false;
  stop_game();
  geometry = gNil;
  variation = eVariation::pure;
  nil_set_geodesic();
  enable_canvas();
  patterns::canvasback = 0;
  vid.cells_drawn_limit = 1;
  smooth_scrolling = true;
  mapeditor::drawplayer = false;
  backcolor = 0xC0C0FFFF;
  logfog = 1;
  initialize();
  poly_outline = 0xFF;
  pushScreen(pick_game);
  #if CAP_VR
  vrhr::hsm = vrhr::eHeadset::reference;
  vrhr::eyes = vrhr::eEyes::equidistant;
  vrhr::absolute_unit_in_meters = 6;
  #endif
  }

auto celldemo = arg::add3("-unilcycle", initialize) + arg::add3("-unilplan", [] { planning_mode = true; }) + arg::add3("-viewsim", [] { view_replay = true; })
  + arg::add3("-oqc", [] { on_quit = popScreenAll; })
  + arg::add3("-nilsolve-set", [] {
    arg::shift(); solver_unit = arg::argf();
    arg::shift(); nospeed = arg::argi();
    arg::shift(); goal_id = arg::argi();
    curlev->solve(); })
  + arg::add3("-nilsolve", [] { curlev->solve(); })
  + arg::add3("-nilgeo", nil_set_geodesic)
  + arg::add3("-nilper", nil_set_perspective)
  + arg::add3("-nilrider", initialize_all)
  + arg::add3("-nilrider-q", [] { arg::shift(); reduce_quality = arg::argi(); })
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
    ->editable(0, 5, 0.05, "camera distance", "how far is the unicycle from the camera", 'd')
    ->set_reaction([] { curlev->current.centerview(curlev); });
    param_f(min_gfx_slope, "min_gfx_slope")
    ->editable(-90._deg, 90._deg, degree, "min camera slope", "affected by up/down", 'm');
    })
  + arg::add3("-fullsim", [] {
    /* for animations */
    popScreenAll();
    rv_hook(anims::hooks_anim, 100, [] {
      int ttick = ticks % isize(curlev->history);
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

#ifdef NILRIDER
auto hook1=
    addHook(hooks_config, 100, [] {
      if(arg::curphase == 1)
        conffile = "nilrider.ini";
      if(arg::curphase == 2) initialize_all();
      });
#endif

}
