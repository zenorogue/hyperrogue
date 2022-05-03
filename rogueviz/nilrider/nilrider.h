#include "../rogueviz.h"

namespace nilrider {

using namespace rogueviz;

struct level;

struct timestamp {
  hyperpoint where; /**< the current position of the unicycle */
  ld heading_angle; /**< the current heading angle */
  ld vel;           /**< the current velocity in units per second */
  ld circpos;       /**< controls the wheel graphics */
  ld slope;         /**< the current slope */
  ld t;             /**< planning spline parameter */

  flagtype collected_triangles; /**< a bitset which shows which triangles are collected */
  flagtype achievements;        /**< a bitset which shows which achievements are complete */
  flagtype achflags;            /**< a bitset which marks failed conducts, etc. */

  bool tick(level*);/**< one tick of the simulation -- returns false if the unicycle has stopped or crashed */
  void centerview(level*);
  void draw_unilcycle(const shiftmatrix&);
  void draw_instruments(ld t);
  ld energy_in_squares();
  bool collect(level*);
  void be_consistent();
  };

struct planpoint {
  hyperpoint at;
  hyperpoint vel;
  planpoint(hyperpoint a, hyperpoint v): at(a), vel(v) {};
  };

using plan_t = vector<planpoint>;

constexpr flagtype nrlPolar = Flag(1);
constexpr flagtype nrlOrder = Flag(2);

struct statue {
  transmatrix T;
  hpcshape *shape;
  color_t color;
  };

struct triangledata {
  int x, y;
  hyperpoint where;
  array<color_t, 7> colors;
  };

struct manual_replay {
  string name;
  vector<int> headings;
  };

struct plan_replay {
  string name;
  plan_t plan;
  };

using xy_float = pair<ld, ld>;
using xy_int = pair<int, int>;
inline xy_int pfloor(xy_int p) { return {floor(p.first), floor(p.second)}; }

struct level {
  string name;
  char hotkey;
  string longdesc;
  flagtype flags;
  ld minx, miny, maxx, maxy;
  vector<string> map_tiles;
  ld startx, starty;
  ld scale;
  std::function<ld(hyperpoint h)> surface;
  
  bool initialized;
  
  level(string name, char hotkey, flagtype flags, string longdesc, ld minx, ld miny, ld maxx, ld maxy, const vector<string>& mt, ld sx, ld sy, const std::function<ld(hyperpoint h)>& surf) :
    name(name), hotkey(hotkey), longdesc(longdesc), flags(flags), minx(minx), miny(miny), maxx(maxx), maxy(maxy), map_tiles(mt), startx(sx), starty(sy), surface(surf) { initialized = false; }
  
  ld real_minx, real_miny, real_maxx, real_maxy;

  /* data */
  hpcshape shFloor;  /**< the 3D model of floor */
  hpcshape shPlanFloor;  /**< the 3D model of floor for planning */
  hpcshape shField;  /**< the 3D model of the 'field' */
  hpcshape shCastle; /**< the 3D model of the 'castle' */
  
  vector<statue> statues;
  vector<triangledata> triangles;

  /** the texture data used for the ground */
  texture::texture_data *unil_texture;

  /** the texture used for the ground */
  basic_textureinfo uniltinf;

  /** the texture used for the ground */
  basic_textureinfo castle_tinf;
  
  /** starting timestamp */
  timestamp start;

  /** current timestamp */
  timestamp current;
  
  /** initialize textures and start */
  void init();

  vector<timestamp> history;
  
  vector<manual_replay> manual_replays;
  vector<plan_replay> plan_replays;

  /** plan for the planning mode */
  plan_t plan;
  void init_plan();
  bool simulate();
  void draw_planning_screen();
  void draw_level(const shiftmatrix& V);
  shiftmatrix plan_transform;

  hyperpoint get_spline(ld t);
  hyperpoint mappt(ld x, ld y, int s);
  ld safe_alt(hyperpoint h, ld mul = 1, ld mulx = 1);
  void compute_plan_transform();
  bool handle_planning(int sym, int uni);
  void solve();

  xy_float get_xy_f(hyperpoint h);
  xy_int get_xy_i(hyperpoint h) { return pfloor(get_xy_f(h)); }
  char mapchar(xy_int p);
  char mapchar(xy_float p) { return mapchar(pfloor(p)); }
  char mapchar(hyperpoint h) { return mapchar(pfloor(get_xy_f(h))); }
  };

/** ticks per second */
inline const ld tps = 1000;

/** wheel radius */
inline ld whrad = 0.05;

/** epsilon used to measure slope */
inline ld slope_eps = 0.01;

/** gravity acceleration constant, in units per second squared */
inline ld gravity = 1 / 16.;

/** the distance of camera from the wheel */
inline ld whdist = 0.5;

/** minimum slope for rendering */
inline ld min_gfx_slope = +M_PI/2;

/** current slope for rendering */
inline ld gfx_slope = 0;

/** the timer */
inline ld timer = 0;

/** default block unit */
inline double dft_block = 1;

extern map<char, color_t> bcols;
extern map<char, array<string, 16> > submaps;

hyperpoint sym_to_heis(hyperpoint H);

}
