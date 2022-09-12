namespace hr {

namespace ads_game {

/** simulation speed */
ld simspeed = TAU;

/** by how much do WAS keys accelerate */
ld accel = 6;

/** transform world coordinates to ship coordinates */
ads_matrix current;

/** SL cell closest to the ship */
cell *vctr;

/** world coordinates of vctr -- technically, this is a shiftmatrix */
ads_matrix vctrV;

/** how far is vctr from the ship */
ld vctr_dist;

/** how is the ship shape rotated */
ld ang = 0;

/** ship's current proper time */
ld ship_pt;

/** is the game paused */
bool paused;

/** auto-rotate the screen */
bool auto_rotate = false;

/** should we display the proper times of all objects */
bool view_proper_times = false;

/** format for displaying time */
const char *tformat = "%.2f";

void game_menu();

/** all the missiles and objects currently displayed */
vector<struct ads_object*> displayed;

color_t missile_color = 0xFF0000FF;

struct player_data {
  int hitpoints;
  int score;
  int ammo;
  ld fuel;
  ld oxygen;
  };

player_data pdata, max_pdata, tank_pdata;

}}
