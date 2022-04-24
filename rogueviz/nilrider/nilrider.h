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
  bool tick(level*);/**< one tick of the simulation -- returns false if the unicycle has stopped or crashed */
  void centerview(level*);
  void draw_unilcycle(const shiftmatrix&);
  void draw_instruments(ld t);
  ld energy_in_squares();
  };

struct planpoint {
  hyperpoint at;
  hyperpoint vel;
  planpoint(hyperpoint a, hyperpoint v): at(a), vel(v) {};
  };

constexpr flagtype nrlPolar = Flag(1);

struct statue {
  transmatrix T;
  hpcshape *shape;
  color_t color;
  };

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
  
  /** plan for the planning mode */
  vector<planpoint> plan;
  void init_plan();
  bool simulate();
  void draw_planning_screen();
  void draw_level(const shiftmatrix& V);
  shiftmatrix plan_transform;

  hyperpoint get_spline(ld t);
  hyperpoint mappt(ld x, ld y, int s);
  ld safe_alt(hyperpoint h, ld mul = 1);
  void compute_plan_transform();
  bool handle_planning(int sym, int uni);
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