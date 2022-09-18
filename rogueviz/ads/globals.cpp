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

/** hyperbolic cell closest to the current view/ship */
cell *vctr, *new_vctr, *vctr_ship;

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
void change_scale(ld s);

/** all the missiles and objects currently displayed */
vector<struct ads_object*> displayed;

/** how much should be the objects scaled */
ld scale = 1;

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

ld rock_density = 0.25;
ld rock_max_rapidity = 1.5;

ld missile_rapidity = 3; // speed is tanh(3) = about 0.95c

ld crash_particle_rapidity = 1;
ld crash_particle_qty = 8;
ld crash_particle_life = .5;

ld fuel_particle_rapidity = 1;
ld fuel_particle_qty = 20;
ld fuel_particle_life = .15;

cell *starting_point;

int max_gen_per_frame = 3;
int draw_per_frame = 1000;

}}
