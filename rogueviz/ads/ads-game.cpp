#include "../rogueviz.h"

#include "math.cpp"
#include "globals.cpp"
#include "shapes.cpp"
#include "resources.cpp"
#include "map.cpp"
#include "control.cpp"
#include "display.cpp"
#include "menu.cpp"

namespace hr {

namespace ads_game {

void change_default_key(int key, int val) {
  char* t = multi::scfg.keyaction;
  t[key] = val;
  #if CAP_CONFIG
  set_saver_default(t[key]);
  #endif
  }

void restart() {

  hybrid::in_actual([&] {
    vctr = new_vctr = starting_point;
    vctrV = new_vctrV = ads_matrix(Id, 0);
    init_rsrc();
    });

  ci_at.clear();

  gen_terrain(vctr, ci_at[vctr], -2);
  forCellEx(c1, vctr) ci_at[c1].type = wtNone;
  ci_at[vctr].type = wtNone;
  invincibility_pt = how_much_invincibility;
  }

void run_ads_game() {

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
  rogueviz::rv_hook(shmup::hooks_turn, 0, ads_turn);
  
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

  rogueviz::rv_change(nohelp, true);
  rogueviz::rv_change(nomenukey, true);
  rogueviz::rv_change(nomap, true);
  rogueviz::rv_change(no_find_player, true);

  restart();
  }

void run_ads_game_std() {
  set_geometry(gNormal);
  set_variation(eVariation::pure);
  run_ads_game();
  showstartmenu = false;
  }

void change_scale(ld s) {
  scale *= s;
  rock_density /= (s * s);
  rock_max_rapidity *= s;
  simspeed *= s;
  pconf.scale /= s;
  how_much_invincibility *= s;
  max_pdata.oxygen *= s;
  tank_pdata.oxygen *= s;
  crash_particle_life *= s;
  fuel_particle_life *= s;
  }

auto shot_hooks = 
  arg::add3("-ads-game1", run_ads_game)
+ arg::add3("-ads-game", run_ads_game_std)
+ arg::add3("-ads-scale", [] { arg::shift(); ld s = arg::argf(); change_scale(s); })
+ arg::add3("-ads-restart", restart)
+ addHook(hooks_configfile, 100, [] {
    param_f(how_much_invincibility, "ads_invinc")
    -> editable(0, TAU, TAU/4, "invincibility time", "How long does the period of invincibility after crashing last, in absolute units.", 'i');
    param_b(auto_angle, "ads_auto_angle")
    -> editable("automatically rotate the projection", 'a');
    param_f(simspeed, "ads_game_simspeed")
    -> editable(0, 2*TAU, TAU/4, "game speed", "Controls the speed of the game.", 's');
    param_f(scale, "ads_game_scale")
    -> editable(0, 2, 0.1, "game scale", "Controls the scaling of game objects.", 'c');
    param_f(accel, "ads_game_accel")
    -> editable(0, 30, 1, "acceleration", "Controls the speed of your ship's acceleration.", 'a');
    param_f(time_unit, "ads_time_unit")
    -> editable(0, 2*TAU, 1, "time unit",
      "Controls the unit used when the 'display the proper times' option is on.\n\n"
      "It takes tau(=2π) units to go the full circle, so the default time unit is tau. You can also use the absolute units (1).\n\n"
      "Times in settings are always specified in absolute units."
      , 'a');
    param_f(pause_speed, "ads_pause_speed")
    -> editable(0, 30, 1, "movement speed while paused", "Controls the speed of camera movement while paused and holding the 'move switch' key.", 'v');
    param_f(rock_density, "ads_rock_density")
    -> editable(0, 5, 0.05, "rock density", "how many rocks to generate", 'd');
    param_f(rock_max_rapidity, "ads_rock_rapidity")
    -> editable(0, 5, 0.05, "rock rapidity", "how fast should the rocks be relative to the map", 'w');
    param_f(missile_rapidity, "ads_missile_rapidity")
    -> editable(0, 5, 0.05, "missile rapidity", "how fast should the missiles go relative to the ship", 'm');
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

    rsrc_config();
    });

}
}
