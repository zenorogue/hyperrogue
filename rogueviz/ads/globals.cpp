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

ld time_scale = .5;

color_t missile_color = 0xFF0000FF;

bool game_over;

constexpr int score_types = 3;

struct player_data {
  int hitpoints;
  int ammo;
  ld fuel;
  ld oxygen;
  int score[score_types];
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
int draw_per_frame = 200;

bool simple_ship = false;

/* for DS */

ads_object *main_rock;

extern string copyright_shown;

extern local_parameter_set lps_relhell, lps_relhell_space, lps_relhell_ads_spacetime, lps_relhell_ds_spacetime_pers, lps_relhell_ds_spacetime_klein;

int XSCALE = 48;
int YSCALE = 48;
int talpha = 32;
void init_textures();
void pick_textures();
void draw_textures();
void reset_textures();
void run_size_hooks();

void ds_restart();
void run_ads_game_std();
void add_ads_cleanup();
void run_ds_game();
void run_ds_game_std();
void add_ds_cleanup();

/** in the replay mode */
bool rev_replay;
bool in_replay;
void switch_replay();
void switch_pause();
void replay_animation();

bool in_spacetime();
void switch_spacetime();
void switch_spacetime_to(bool);
bool ads_draw_cell(cell *c, const shiftmatrix& V);
extern transmatrix Duality;
extern int use_duality;

ld spacetime_step = 0.1;
int spacetime_qty = 30;

color_t ghost_color = 0x800080FF;

/* types */

enum eObjType { oRock, oMissile, oParticle, oResource, oMainRock, oTurret, oTurretMissile };
enum eResourceType { rtNone, rtHull, rtAmmo, rtFuel, rtOxygen, rtGoldRocks, rtGoldGate, rtGoldTurret, rtGUARD };
enum eWalltype { wtNone, wtDestructible, wtSolid, wtGate, wtBarrier };

PPR obj_prio[7] = { PPR::MONSTER_BODY, PPR::ITEMa, PPR::ITEM_BELOW, PPR::ITEM, PPR::MONSTER_HEAD, PPR::MONSTER_BODY, PPR::ITEMa };

struct cell_to_draw {
  cross_result center;
  ld d;
  cell *c;
  ads_matrix V;
  bool operator < (const cell_to_draw& c2) const { return d > c2.d; }
  };

/** all cell_to_draw drawn currently */
std::unordered_map<cell*, cell_to_draw> cds, cds_last;

struct turret_state {
  ld angle, dist;
  int index;
  ld err;
  };

struct expiry_data {
  int score;
  int score_id;
  };

struct ads_object {
  eObjType type;
  eResourceType resource;
  cell *owner;
  ads_matrix at;
  color_t col;
  expiry_data expire;
  vector<ld>* shape;
  ld last_shot;
  int hlast;

  map<ld, turret_state> turret_states;

  ld life_start, life_end;
  cross_result pt_main;
  vector<cross_result> pts;
  int subtype;

  ads_object(eObjType t, cell *_owner, const ads_matrix& T, color_t _col) : type(t), owner(_owner), at(T), col(_col) {
    life_start = -HUGE_VAL;
    life_end = HUGE_VAL;
    subtype = 0;
    }
  };

struct shipstate {
  ads_matrix at;
  ads_matrix current;
  ld start;
  ld duration;
  ld ang;
  ads_matrix vctrV;
  cell *vctr;
  };

struct cellinfo {
  int mpd_terrain; /* 0 = fully generated terrain */
  int rock_dist; /* rocks generated in this radius */
  vector<std::unique_ptr<ads_object>> rocks;
  vector<shipstate> shipstates;
  eWalltype type;
  cellinfo() {
    mpd_terrain = 4;
    rock_dist = -1;
    type = wtNone;
    }
  };

struct gamedata {
  int gameid;
  string myname;
  string timerstart, timerend;
  string variant;
  string deathreason;
  ld scores[8];
  int seconds;
  int turrets_hit, rocks_hit, rsrc_collected;
  };

extern gamedata cur;

void init_gamedata();
void game_over_with_message(const string& reason);
void save_to_hiscores();
void hiscore_menu();
void load_hiscores();

string get_main_help();
int generate_mouseovers();

bool all_params_default();
bool no_param_change;
bool params_changed();

void start_relhell_tour();

void ads_restart();
void run_ads_game();

}}
