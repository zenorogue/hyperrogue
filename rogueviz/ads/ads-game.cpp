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

  nomap = true;
  no_find_player = true;
  vctr = new_vctr = cwt.at;
  cell *c = hybrid::get_where(vctr).first;
  hybrid::in_underlying_geometry([&] {
    gen_terrain(c, ci_at[c], -2);
    forCellEx(c1, c) ci_at[c1].type = wtNone;
    ci_at[c].type = wtNone;
    ci_at[c].rocks.clear();
    });
  vctrV = new_vctrV = ads_matrix(Id, 0);
  rogueviz::rv_hook(hooks_prestats, 100, view_ads_game);
  rogueviz::rv_hook(hooks_handleKey, 0, handleKey);
  rogueviz::rv_hook(shmup::hooks_turn, 0, ads_turn);
  init_rsrc();
  }

auto shot_hooks = 
  arg::add3("-ads-game", run_ads_game)
+ addHook(hooks_configfile, 100, [] {
    param_f(simspeed, "ads_game_simspeed")
    -> editable(0, 2*TAU, TAU/4, "game speed", "Controls the speed of the game.", 's');
    param_f(accel, "ads_game_accel")
    -> editable(0, 30, 1, "acceleration", "Controls the speed of your ship's acceleration.", 'a');
    param_b(auto_rotate, "ads_auto_rotate")
    -> editable("automatically rotate the screen", 'r');
    param_b(view_proper_times, "ads_display")
    -> editable("display the proper times", 't');
    });

}
}
