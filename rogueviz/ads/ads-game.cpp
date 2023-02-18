/* Main file of Relative Hell. */
/* Compile with mymake -O3 -rv rogueviz/ads/ads-game */
/* Best run with -ads-menu; more detailed options are available too */

#define VER_RH "0.2"

#ifdef RELHELL

#define CUSTOM_CAPTION "Relative Hell " VER_RH
#define MAXMDIM 4
#define CAP_INV 0
#define CAP_COMPLEX2 0
#define CAP_EDIT 0
#define CAP_BT 0
#define CAP_SOLV 0
#define CAP_THREAD 0
#define CAP_FILES 1
#define CAP_CONFIG 1
#define CAP_CRYSTAL 0
#define CAP_ARCM 0
#define CAP_HISTORY 0
#define CAP_STARTANIM 0
#include "../hyper.cpp"

namespace rogueviz { std::vector<hr::reaction_t> cleanup; }

#endif

#include "../rogueviz.h"

#include "math.cpp"
#include "globals.cpp"
#include "shapes.cpp"
#include "resources.cpp"
#include "map.cpp"
#include "control.cpp"
#include "display.cpp"
#include "menu.cpp"
#include "ds-game.cpp"
#include "ds-texture.cpp"
#include "views.cpp"

namespace hr {

namespace ads_game {

void change_default_key(int key, int val) {
  char* t = multi::scfg.keyaction;
  t[key] = val;
  #if CAP_CONFIG
  set_saver_default(t[key]);
  #endif
  }

void set_default_keys() {
  change_default_key('s', 16 + 0);
  change_default_key('a', 16 + 1);
  change_default_key('w', 16 + 2);
  change_default_key('d', 16 + 3);
  change_default_key('f', 16 + 4);
  change_default_key('p', 16 + 5);
  change_default_key('t', 16 + 6);
  change_default_key('o', 16 + 7);
  change_default_key('m', 16 + 8);
  change_default_key('i', 16 + 9);
  change_default_key('k', 16 + 10);
  change_default_key('l', 16 + 11);
  }

void restart() {

  if(in_spacetime()) {
    switch_spacetime();
    restart();
    switch_spacetime();
    return;
    }

  hybrid::in_actual([&] {
    vctr = new_vctr = starting_point;
    vctrV = new_vctrV = current = ads_matrix(Id, 0);
    init_rsrc();
    });

  ci_at.clear();

  gen_terrain(vctr, ci_at[vctr], -2);
  forCellEx(c1, vctr) ci_at[c1].type = wtNone;
  ci_at[vctr].type = wtNone;
  invincibility_pt = ads_how_much_invincibility;

  paused = false;
  ship_pt = 0;
  }

void run_ads_game() {

  if(!sl2) set_geometry(gRotSpace);
  if(hybrid::csteps) {
    stop_game();
    hybrid::csteps = 0;
    hybrid::reconfigure();
    }
  start_game();

  starting_point = hybrid::get_where(cwt.at).first;
  
  rogueviz::rv_hook(hooks_frame, 100, view_ads_game);
  rogueviz::rv_hook(hooks_prestats, 100, display_rsrc);
  rogueviz::rv_hook(hooks_handleKey, 0, handleKey);
  rogueviz::rv_hook(hooks_drawcell, 0, ads_draw_cell);
  rogueviz::rv_hook(shmup::hooks_turn, 0, ads_turn);
  rogueviz::rv_hook(anims::hooks_anim, 100, replay_animation);
  
  cgi.use_count++;
  hybrid::in_underlying_geometry([] {
    cgi.use_count++;
    });

  auto umap = hybrid::get_umap();
  hybrid::actual_geometry = geometry;
  geometry = hybrid::underlying;
  hybrid::underlying_cgip->single_step = cgi.single_step;
  hybrid::underlying_cgip->psl_steps = cgi.psl_steps;
  cgip = hybrid::underlying_cgip;
  hybrid::pmap = currentmap;
  currentmap = umap;  
  pmodel = mdDisk;
  cwt.at = centerover = currentmap->gamestart();

  restart();
  }

void set_config() {
  set_default_keys();

  nohelp = true;
  nomenukey = true;
  no_find_player = true;
  showstartmenu = false;
  mapeditor::drawplayer = false;
  vid.drawmousecircle = false;
  draw_centerover = false;
  vid.axes3 = false;
  enable_canvas();
  patterns::whichCanvas = 'r';
  patterns::rwalls = 0;
  }

void run_ads_game_std() {
  set_geometry(gNormal);
  set_variation(eVariation::pure);
  run_ads_game();
  showstartmenu = false;
  }

void change_scale(ld s) {
  ads_scale *= s;
  rock_density /= (s * s);
  rock_max_rapidity *= s;
  ads_simspeed *= s;
  pconf.scale /= s;
  ads_how_much_invincibility *= s;
  ads_max_pdata.oxygen *= s;
  ads_tank_pdata.oxygen *= s;
  crash_particle_life *= s;
  fuel_particle_life *= s;
  }

auto shot_hooks = 
  arg::add3("-ads-game1", run_ads_game)
+ arg::add3("-ads-game", run_ads_game_std)
+ arg::add3("-ads-zero", set_config)
+ arg::add3("-ads-menu", [] { set_config(); pushScreen(pick_the_game); })
+ arg::add3("-ads-scale", [] { arg::shift(); ld s = arg::argf(); change_scale(s); })
+ arg::add3("-ads-restart", restart)
+ addHook(hooks_configfile, 100, [] {
    param_f(ads_how_much_invincibility, "ads_invinc")
    -> editable(0, TAU, TAU/4, "AdS invincibility time", "How long does the period of invincibility after crashing last, in absolute units.", 'i');
    param_f(ds_how_much_invincibility, "ads_invinc")
    -> editable(0, TAU, TAU/4, "dS invincibility time", "How long does the period of invincibility after crashing last, in absolute units.", 'i');
    param_b(auto_angle, "ads_auto_angle")
    -> editable("automatically rotate the projection", 'a');
    param_f(ads_simspeed, "ads_game_simspeed")
    -> editable(0, 2*TAU, TAU/4, "AdS game speed", "Controls the speed of the game, in absolute units per second.", 's');
    param_f(ds_simspeed, "ds_game_simspeed")
    -> editable(0, 2*TAU, TAU/4, "dS game speed", "Controls the speed of the game, in absolute units per second.", 's');
    param_f(ads_scale, "ads_game_scale")
    -> editable(0, 2, 0.1, "AdS game scale", "Controls the scaling of game objects.", 'c');
    param_f(ds_scale, "ds_game_scale")
    -> editable(0, 2, 0.1, "dS game scale", "Controls the scaling of game objects.", 'c');
    param_f(ads_accel, "ads_game_accel")
    -> editable(0, 30, 1, "AdS acceleration", "Controls your ship's acceleration, in absolute units per second squared.", 'a');
    param_f(ds_accel, "ds_game_accel")
    -> editable(0, 30, 1, "dS acceleration", "Controls your ship's acceleration, in absolute units per second squared.", 'a');
    param_f(ads_time_unit, "ads_time_unit")
    -> editable(0, 2*TAU, 1, "AdS time unit",
      "Controls the unit used when the 'display the proper times' option is on.\n\n"
      "It takes tau(=2π) units to go the full circle, so the default time unit is tau. You can also use the absolute units (1).\n\n"
      "Times in settings are always specified in absolute units."
      , 'a');
    param_f(ds_time_unit, "ds_time_unit")
    -> editable(0, 2*TAU, 1, "dS time unit",
      "Controls the unit used when the 'display the proper times' option is on.\n\n"
      "It takes tau(=2π) units to go the full circle, so the default time unit is tau. You can also use the absolute units (1).\n\n"
      "Times in settings are always specified in absolute units."
      , 'a');
    param_f(pause_speed, "ads_pause_speed")
    -> editable(0, 30, 1, "movement speed while paused", "Controls the speed of camera movement while paused and holding the 'move switch' key. Absolute units per second.", 'v');
    param_f(rock_density, "ads_rock_density")
    -> editable(0, 5, 0.05, "rock density", "how many rocks to generate", 'd');
    param_f(rock_max_rapidity, "ads_rock_rapidity")
    -> editable(0, 5, 0.05, "rock rapidity", "how fast should the rocks be relative to the map", 'w');
    param_f(ads_missile_rapidity, "ads_missile_rapidity")
    -> editable(0, 5, 0.05, "AdS missile rapidity", "how fast should the missiles go relative to the ship", 'm');
    param_f(ds_missile_rapidity, "ds_missile_rapidity")
    -> editable(0, 5, 0.05, "dS missile rapidity", "how fast should the missiles go relative to the ship", 'm');
    param_b(auto_rotate, "ads_auto_rotate")
    -> editable("automatically rotate the screen", 'r');
    param_b(view_proper_times, "ads_display")
    -> editable("display the proper times", 't');

    param_f(crash_particle_rapidity, "ads_crash_rapidity")
    -> editable(0, 5, 0.1, "crash particle rapidity", "how fast should the crash particles be", 'r');
    param_f(crash_particle_qty, "ads_crash_qty")
    -> editable(0, 5, 0.1, "crash particle quantity", "how many crash particles", 'q');
    param_f(crash_particle_life, "ads_crash_life")
    -> editable(0, 5, 0.1, "crash particle lifetime", "how long should the crash particles live", 'l');
    param_f(fuel_particle_rapidity, "ads_fuel_rapidity")
    -> editable(0, 5, 0.1, "fuel particle rapidity", "how fast should the fuel particles be", 'R');
    param_f(fuel_particle_qty, "ads_fuel_qty")
    -> editable(0, 5, 0.1, "fuel particle quantity", "how many fuel particles", 'Q');
    param_f(fuel_particle_life, "ads_fuel_life")
    -> editable(0, 5, 0.1, "fuel particle lifetime", "how long should the fuel particles live", 'L');

    param_i(max_gen_per_frame, "ads_gen_per_frame")
    -> editable(0, 100, 1, "tiles to generate per frame", "reduce if the framerate is low", 'G');
    param_i(draw_per_frame, "ads_draw_per_frame")
    -> editable(0, 3000, 0.1, "tiles to draw per frame", "reduce if the framerate is low", 'D');

    param_i(XSCALE, "ds_xscale")
    -> editable(4, 512, 8, "x precision of Earth-de Sitter", "", 'x');
    param_i(YSCALE, "ds_yscale")
    -> editable(4, 512, 8, "y precision of Earth-de Sitter", "", 'y');
    param_i(talpha, "ds_talpha")
    -> editable(0, 255, 16, "dS texture intensity", "", 't');

    param_f(spacetime_step, "ads_spacetime_step")
    -> editable(0, 1, 0.05, "step size in the spacetime display", "", 's');

    param_i(spacetime_qty, "ads_spacetime_qty")
    -> editable(0, 100, 5, "step quantity in the spacetime display", "", 'q');

    addsaver(ghost_color, "color:ghost");

    rsrc_config();
    });

#ifdef RELHELL
auto hook1=
    addHook(hooks_config, 100, [] {
      set_config();
      if(arg::curphase == 1)
        conffile = "relhell.ini";
      if(arg::curphase == 3) pushScreen(pick_the_game);
      });
#endif

}
}
