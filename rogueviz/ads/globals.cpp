namespace hr {

namespace ads_game {

/** simulation speed */
ld ads_simspeed = TAU / 10;
ld ds_simspeed = M_PI / 10;

#define DS_(x) (main_rock ? ds_##x : ads_##x)

/** by how much do WAS keys accelerate */
ld ads_accel = 6 * ads_simspeed;
ld ds_accel = 15 * ds_simspeed;

/** cursor movement speed while paused */
ld pause_speed = 0.5;

/** time unit for time display */
ld ads_time_unit = TAU;
ld ds_time_unit = 1;

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
ld ads_scale = 1;
ld ds_scale = 1;

color_t missile_color = 0xFF0000FF;

bool game_over;

struct player_data {
  int hitpoints;
  int score;
  int ammo;
  ld fuel;
  ld oxygen;
  };

ld ads_how_much_invincibility = TAU / 4;
ld ds_how_much_invincibility = TAU / 4;

player_data pdata, ads_max_pdata, ads_tank_pdata, ds_max_pdata, ds_tank_pdata;

bool auto_angle = true;

ld rock_density = 0.25;
ld rock_max_rapidity = 1.5;

ld ads_missile_rapidity = 3; // speed is tanh(3) = about 0.95c
ld ds_missile_rapidity = 3; // speed is tanh(3) = about 0.95c

ld crash_particle_rapidity = 1;
ld crash_particle_qty = 8;
ld crash_particle_life = .5;

ld fuel_particle_rapidity = 1;
ld fuel_particle_qty = 20;
ld fuel_particle_life = .15;

cell *starting_point;

int max_gen_per_frame = 3;
int draw_per_frame = 1000;

/* for DS */

ads_object *main_rock;

extern string copyright_shown;

int XSCALE = 48;
int YSCALE = 48;
int talpha = 32;
void init_textures();
void pick_textures();
void draw_textures();
void reset_textures();

void ds_restart();
void run_ads_game_std();
void run_ds_game();
}}
