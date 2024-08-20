#include "../rogueviz.h"

namespace nilrider {

using namespace rogueviz;

struct level;

/** ticks per second */
inline const ld tps = 1000;

struct colorscheme {
  color_t wheel1, wheel2, seat, seatpost;
  colorscheme(int i) {
    if(i == 1) { wheel1 = 0xFFFF40FF; wheel2 = 0xFF4040FF; seat = 0x303030FF; seatpost = 0x303030FF; }
    if(i == 2) {
      wheel1 = 0xFF | ((rand() % 0x1000000) << 8);
      wheel2 = 0xFF | ((rand() % 0x1000000) << 8);
      seat = 0xFF | ((rand() % 0x1000000) << 8);
      seatpost = 0xFF | ((rand() % 0x1000000) << 8);
      }
    }
  };

colorscheme my_scheme(1);

struct timestamp {
  hyperpoint where; /**< the current position of the unicycle */
  ld heading_angle; /**< the current heading angle */
  ld vel;           /**< the current velocity in units per second */
  ld circpos;       /**< controls the wheel graphics */
  ld slope;         /**< the current slope, as angle */
  ld chg_slope;     /**< slope used at surface state change */
  ld gfx_slope;     /**< current slope used by graphics */
  ld t;             /**< planning spline parameter */
  ld timer = 0;     /**< the timer, in seconds */
  level *on_surface;/**< pointer to the sub-level if we are currently on the surface, nullptr otherwise */

  ld sstime;        /**< when did we leave or enter surface? for smoothing gfx_slope */
  hyperpoint flyvel;/**< velocity vector if we are not on any surface */
  ld circvel;       /**< how fast the wheel is rotating if we are not on any surface, per second */

  ld last_draw;     /**< when was gfx_slope computed */
  ld last_tramp;    /**< time of last trampoline */
  ld tramp_head;    /**< heading_angle at the moment of last trampoline */


  flagtype collected_triangles; /**< a bitset which shows which triangles are collected */
  flagtype goals;               /**< a bitset which shows which goals are complete */
  flagtype failed;              /**< a bitset which shows which goals are failed */

  bool tick(level*, ld timeleft = 1. / tps);/**< one tick of the simulation -- returns false if the unicycle has stopped or crashed */
  void centerview(level*);
  void draw_unilcycle(const shiftmatrix&, const colorscheme& cs);
  void draw_instruments(level*);
  ld energy_in_squares();
  bool collect(level*);
  bool out_of_surface(level*);
  void be_consistent();

  bool check_crashes_rec(level*, hyperpoint owhere, hyperpoint oflyvel, ld timeleft);
  bool check_crashes(level*, hyperpoint owhere, hyperpoint oflyvel, ld timeleft);
  };

struct planpoint {
  hyperpoint at;
  hyperpoint vel;
  planpoint(hyperpoint a, hyperpoint v): at(a), vel(v) {};
  };

using plan_t = vector<planpoint>;

constexpr flagtype nrlPolar = Flag(1);
constexpr flagtype nrlOrder = Flag(2);
constexpr flagtype nrlSwapTriangleOrder = Flag(3);
constexpr flagtype nrlUserCreated = Flag(4);

struct statue {
  transmatrix T;
  hpcshape *shape;
  color_t color;
  };

struct triangledata {
  int x, y;
  hyperpoint where;
  level *which;
  array<color_t, 7> colors;
  };

struct manual_replay {
  string name;
  colorscheme cs;
  vector<pair<int, int>> headings;
  };

struct plan_replay {
  string name;
  colorscheme cs;
  plan_t plan;
  };

using xy_float = pair<ld, ld>;
using xy_int = pair<int, int>;
inline xy_int pfloor(xy_float p) { return {floor(p.first), floor(p.second)}; }

enum eGoalResult { grNone, grSuccess, grFailed };

struct checkerparam {
  timestamp *t;
  level *l;
  int rev;
  };

using goalchecker = std::function<eGoalResult(checkerparam)>;

struct goal {
  color_t color;
  string desc;
  goalchecker check;
  };

using surface_fun = std::function<ld(hyperpoint h)>;

struct ghost {
  colorscheme cs;
  vector<timestamp> history;
  };

struct level {
  string name;
  string filename;
  char hotkey;
  string longdesc;
  flagtype flags;
  ld minx, miny, maxx, maxy;
  vector<string> map_tiles;
  ld startx, starty;
  vector<level*> sublevels;
  ld scale;
  surface_fun surface;
  
  bool initialized;
  
  level(string name, char hotkey, flagtype flags, string longdesc, ld minx, ld miny, ld maxx, ld maxy, const vector<string>& mt, ld sx, ld sy, const vector<level*> subs, const std::function<ld(hyperpoint h)>& surf, vector<goal> g) :
    name(name), hotkey(hotkey), longdesc(longdesc), flags(flags), minx(minx), miny(miny), maxx(maxx), maxy(maxy), map_tiles(mt), startx(sx), starty(sy), sublevels(subs), surface(surf), goals(g) {
    initialized = false;
    unil_texture = nullptr;
    unil_texture_stepped = nullptr;
    unil_texture_levels = nullptr;
    }
  
  ld real_minx, real_miny, real_maxx, real_maxy;

  /* data */
  hpcshape shFloor;  /**< the 3D model of floor */
  hpcshape shPlanFloor;  /**< the 3D model of floor for planning */
  hpcshape shStepFloor;  /**< the 3D model of stepped floor */
  hpcshape shField;  /**< the 3D model of the 'field' */
  hpcshape shCastle; /**< the 3D model of the 'castle' */
  
  vector<statue> statues;
  vector<triangledata> triangles;
  vector<goal> goals;

  /** the texture data used for the ground */
  texture::texture_data *unil_texture;

  /** the texture data used for the ground in the stepped mode */
  texture::texture_data *unil_texture_stepped;

  /** the texture data used for the ground with level lines */
  texture::texture_data *unil_texture_levels;

  /** the texture used for the ground */
  basic_textureinfo uniltinf;

  /** currently used point for levellines -- levellines[3]==0 means none */
  hyperpoint levellines_for;

  /** requested point for levellines (texture redrawn in init_textures) */
  hyperpoint new_levellines_for;

  /** the texture used for the castles */
  basic_textureinfo castle_tinf;

  /** the texture used for the ground in the stepped mode*/
  basic_textureinfo uniltinf_stepped;
  
  /** starting timestamp */
  timestamp start;

  /** current timestamp */
  timestamp current;
  
  /** initialize textures */
  void init_textures();

  /** initialize shapes */
  void init_shapes();

  /** initialize all */
  void init();

  vector<timestamp> history;
  vector<ghost> ghosts;
  
  vector<manual_replay> manual_replays;
  vector<plan_replay> plan_replays;

  /* vector of records in no-planning and planning for each goal (0 = no record known) */
  vector<ld> records[2];

  /* vector of current time for each goal */
  vector<ld> current_score;

  /** plan for the planning mode */
  plan_t plan;
  void init_plan();
  bool simulate();
  void draw_planning_screen();
  void draw_level(const shiftmatrix& V);
  void draw_level_rec(const shiftmatrix& V);
  shiftmatrix plan_transform;

  hyperpoint get_spline(ld t);
  hyperpoint mappt(ld x, ld y, int s);
  ld safe_alt(hyperpoint h, ld mul = 1, ld mulx = 1);
  void compute_plan_transform();
  bool handle_planning(int sym, int uni);
  void solve();

  hyperpoint surface_point(hyperpoint h) { h[2] = surface(h); return h; }

  xy_float get_xy_f(hyperpoint h);
  xy_int get_xy_i(hyperpoint h) { return pfloor(get_xy_f(h)); }
  char mapchar(xy_int p);
  char mapchar(xy_float p) { return mapchar(pfloor(p)); }
  char mapchar(hyperpoint h) { return mapchar(pfloor(get_xy_f(h))); }

  void gen_layer_list(vector<level*>& all) {
    all.push_back(this); for(auto s: sublevels) s->gen_layer_list(all);
    }

  vector<level*> gen_layer_list() {
    vector<level*> res; gen_layer_list(res); return res;
    }

  vector<timestamp> headings_to_history(manual_replay&);
  void load_plan_as_ghost(plan_replay&);
  void load_manual_as_ghost(manual_replay&);
  void load_all_ghosts();
  };

/** wheel radius */
inline ld whrad = 0.05;

/** epsilon used to measure slope */
inline ld slope_eps = 0.01;

/** gravity acceleration constant, in units per second squared */
inline ld gravity = 1 / 16.;

/** the distance of camera from the wheel */
inline ld whdist = 0.5;

/** ghost repeat period */
inline ld ghost_repeat = 5;

/** minimum slope for rendering */
inline ld min_gfx_slope = +90._deg;

/** current slope for rendering */
inline ld gfx_slope = 0;

/** default block unit */
inline double dft_block = 1;

/** default block unit */
inline double simulation_speed = 1;

extern map<char, color_t> bcols;
extern map<char, array<string, 16> > submaps;

hyperpoint sym_to_used(hyperpoint H);

extern int reversals;
extern bool loaded_or_planned;
extern bool planning_mode;

inline int reduce_quality = 0;

void save();

extern bool on;
extern void restart();
extern bool stepped_display;
}
