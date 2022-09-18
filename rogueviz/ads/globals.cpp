namespace hr {

namespace ads_game {

/** simulation speed */
ld simspeed = TAU;

/** by how much do WAS keys accelerate */
ld accel = 6;

/** cursor movement speed while paused */
ld pause_speed = 5;

/** time unit for time display */
ld time_unit = TAU;

/** transform world coordinates to current view coordinates */
ads_matrix current;

/** transform world coordinates to ship coordinates (used when paused) */
ads_matrix current_ship;

/** SL cell closest to the current view/ship */
cell *vctr, *new_vctr, *vctr_ship;

/** hyperbolic cell closest to the current view/ship */
cell *vctr_ship_base;

/** world coordinates of vctr -- technically, this is a shiftmatrix */
ads_matrix vctrV, new_vctrV, vctrV_ship;

/** how far is vctr from the ship */
ld vctr_dist;

/** how is the ship shape rotated */
ld ang = 0;

/** ship's current proper time */
ld ship_pt;

/** paused camera's current proper time */
ld view_pt;

/** until when is the ship invincible */
ld invincibility_pt;

/** is the game paused */
bool paused;

/** auto-rotate the screen */
bool auto_rotate = false;

/** should we display the proper times of all objects */
bool view_proper_times = false;

/** format for displaying time */
const char *tformat = "%.2f";

void game_menu();
void restart();

/** all the missiles and objects currently displayed */
vector<struct ads_object*> displayed;

color_t missile_color = 0xFF0000FF;

bool game_over;

struct player_data {
  int hitpoints;
  int score;
  int ammo;
  ld fuel;
  ld oxygen;
  };

ld how_much_invincibility = TAU / 4;

player_data pdata, max_pdata, tank_pdata;

bool auto_angle = true;

cell *starting_point;

}}
