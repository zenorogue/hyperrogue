// Hyperbolic Rogue -- main header file
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file hyper.h
 *  \brief The main header file of HyperRogue
 *
 *  Contains general utility macros, various value macros, using clauses for standard library functions,
 *  implementation of the basic connection_table, walker, cell and heptagon classes,
 *  and general routines which did not fit elsewhere
 */

#ifndef _HYPER_H_
#define _HYPER_H_

// version numbers
#define VER "11.2o"
#define VERNUM_HEX 0xA80F

#include "sysconfig.h"
#include "classes.h"

#include <stdarg.h>
#include "hyper_function.h"

namespace hr {

struct always_false {
  operator bool() const { return false; };
  bool operator = (bool b) const { return b; };
  };

template<class T>
void ignore(T&&) {
  // placate GCC's overzealous -Wunused-result
  }

template<class T, class V, class... U> bool among(T x, V y) { return x == y; }
template<class T, class V, class... U> bool among(T x, V y, U... u) { return x==y || among(x,u...); }

// functions and types used from the standard library
using std::vector;
using std::map;
using std::array;
using std::unordered_map;
using std::sort;
using std::multimap;
using std::set;
using std::string;
using std::pair;
using std::tuple;
using std::shared_ptr;
using std::make_shared;
using std::min;
using std::max;
using std::make_pair;
using std::tie;
using std::queue;
using std::swap;
using std::complex;
using std::reverse;
using std::real;
using std::imag;
using std::stable_sort;
using std::out_of_range;
using std::get;
using std::move;
using std::make_tuple;
using std::unique_ptr;

using std::abs;
using std::isfinite;
using std::isnan;
using std::isinf;
using std::log;
using std::exp;
using std::sin;
using std::cos;
using std::sinh;
using std::asin;
using std::acos;
using std::tan;
using std::atan;
using std::atan2;
using std::tanh;
using std::sqrt;
using std::pow;
using std::floor;
using std::ceil;
#ifndef NO_STD_HYPOT
using std::hypot;
using std::asinh;
using std::acosh;
#endif

struct hr_exception: std::exception { hr_exception() {} };
struct hr_shortest_path_exception: hr_exception { };

// genus (in grammar)
#define GEN_M 0
#define GEN_F 1
#define GEN_N 2
#define GEN_O 3

// Add a message to the GUI.
// If multiple messages appear with the same spamtype != 0, the older ones disappear quickly
void addMessage(string s, char spamtype = 0);

// geometry-dependent constants

#define cginf ginf[geometry]

#define ALPHA (M_PI*2/S7)
#define S7 cginf.sides
#define S3 cginf.vertex
#define hyperbolic_37 (S7 == 7 && S3 == 3 && !binarytiling && !archimedean)
#define hyperbolic_not37 ((S7 > 7 || S3 > 3 || binarytiling || archimedean) && hyperbolic)
#define weirdhyperbolic ((S7 > 7 || S3 > 3 || !STDVAR || binarytiling || archimedean) && hyperbolic)
#define stdhyperbolic (S7 == 7 && S3 == 3 && STDVAR && !binarytiling && !archimedean)

#define cgflags cginf.flags 

#define binarytiling (cgflags & qBINARY)
#define archimedean (cgflags & qARCHI)
#define cryst (cgflags & qCRYSTAL)
#define penrose (cgflags & qPENROSE)

/** convenience flag for geometries with major aspects missing */
#define experimental (cgflags & qEXPERIMENTAL)

// these geometries do not feature alternate structures for horocycles
#define eubinary (euclid || binarytiling || cryst || nil)

#define cgclass (cginf.cclass)
#define euclid (cgclass == gcEuclid)
#define sphere (cgclass == gcSphere)
#define solnih (cgclass == gcSolNIH)
#define sol (cgflags & qSOL)
#define nih (cgflags & qNIH)
#define nil (cgclass == gcNil)
#define sl2 (cgclass == gcSL2)
#define prod (cgclass == gcProduct)
#define hybri (cgflags & qHYBRID)
#define hyperbolic (cgclass == gcHyperbolic)
#define nonisotropic (sol || nil || sl2 || nih)
#define translatable (euclid || nonisotropic)
#define nonorientable (cgflags & qNONORIENTABLE)
#define elliptic (cgflags & qELLIPTIC)
#define quotient (cgflags & qANYQ)
#define smallbounded (cgflags & qSMALL)
#define bounded (cgflags & qBOUNDED)

// Dry Forest burning, heat transfer, etc. are performed on the whole universe
#define doall (bounded)

#define sphere_narcm (sphere && !archimedean)

#define a4 (S3 == 4)
#define a45 (S3 == 4 && S7 == 5)
#define a46 (S3 == 4 && S7 == 6)
#define a47 (S3 == 4 && S7 == 7)
#define a457 (S3 == 4 && S7 != 6)
#define a467 (S3 == 4 && S7 >= 6)
#define a38 (S3 == 3 && S7 == 8)
#define sphere4 (sphere && S7 == 4)
#define stdeuc (geometry == gNormal || geometry == gEuclid || geometry == gEuclidSquare)
#define smallsphere (sphere_narcm && S7 < 5)
#define bigsphere (sphere_narcm && S7 == 5)

#define S6 (S3*2)
#define MAX_S3 4

#define SG6 (S3==3?6:4)
#define SG3 (S3==3?3:2)
#define SG2 (S3==3?2:1)

#define GOLDBERG (variation == eVariation::goldberg)
#define IRREGULAR (variation == eVariation::irregular)
#define PURE (variation == eVariation::pure)
#define BITRUNCATED (variation == eVariation::bitruncated)
#define DUAL (variation == eVariation::dual)
#define DUALMUL (DUAL ? 2 : 1)

#define CHANGED_VARIATION (variation != cginf.default_variation)

#define STDVAR (PURE || BITRUNCATED)
#define NONSTDVAR (!STDVAR)

#if CAP_ARCM
#define VALENCE (BITRUNCATED ? 3 : archimedean ? arcm::valence() : S3)
#else
#define VALENCE (BITRUNCATED ? 3 : S3)
#endif

#define NUMWITCH 7

// achievements

#define LB_YENDOR_CHALLENGE 40
#define LB_PURE_TACTICS 41
#define NUMLEADER 82
#define LB_PURE_TACTICS_SHMUP 49
#define LB_PURE_TACTICS_COOP 50
#define LB_RACING 81

#if ISMOBILE || ISWEB || ISPANDORA || 1
typedef double ld;
#define LDF "%lf"
#define PLDF "lf"
#else
typedef long double ld;
#define LDF "%Lf"
#define PLDF "Lf"
#endif

typedef complex<ld> cld;

struct charstyle {
  int charid;
  color_t skincolor, haircolor, dresscolor, swordcolor, dresscolor2, uicolor, eyecolor;
  bool lefthanded;
  };

enum eStereo { sOFF, sAnaglyph, sLR, sODS };

struct videopar {
  ld scale, alpha, sspeed, mspeed, yshift, camera_angle;
  ld ballangle, ballproj, euclid_to_sphere, twopoint_param, fisheye_param, stretch, binary_width, fixed_facing_dir;
  int mobilecompasssize;
  int radarsize; // radar for 3D geometries
  ld radarrange;
  int aurastr, aurasmoothen;
  bool fixed_facing;
  bool fixed_yz;
  bool use_wall_radar;
  
  int linequality;

  bool full;
  
  int graphglyph; // graphical glyphs
  bool darkhepta;
  int shifttarget;
  
  int xres, yres, framelimit;
  
  int xscr, yscr;
  
  ld xposition, yposition;
  
  bool grid;
  int particles;
  
  int fsize;
  int flashtime;
  
  int wallmode, monmode, axes;
  bool axes3;
  bool revcontrol;
  
  int msgleft, msglimit;

  bool usingGL;
  int antialias;
  #define AA_NOGL      1
  #define AA_VERSION   2
  #define AA_LINES     4
  #define AA_POLY      8
  #define AA_LINEWIDTH 16
  #define AA_FONT      32
  #define AA_MULTI     64
  #define AA_MULTI16   128 // not configurable
  ld linewidth;
  
  ld multiplier_grid, multiplier_ring;

  int joyvalue, joyvalue2, joypanthreshold;
  ld joypanspeed;
  
  charstyle cs;
  
  bool samegender; // same gender for the Princess?
  int language;
  
  bool backeffects; // background particle effects
  
  int killreduction, itemreduction, portreduction;
  
  int steamscore;
  bool drawmousecircle; // draw the circle around the mouse
  bool skipstart;       // skip the start menu
  bool quickmouse;      // quick mouse on the map
  bool sloppy_3d;       // make 3D faster but ugly
  int timeformat;       // time format used in the message log
  
  int use_smart_range;  // 0 = distance-based, 1 = model-based, 2 = model-based and generate
  ld smart_range_detail;// minimum visible cell for modes 1 and 2
  ld smart_range_detail_3;// minimum visible cell in 3D (for mode 2, there is no mode 1)
  int cells_drawn_limit;
  int cells_generated_limit; // limit on cells generated per frame
  
  ld skiprope;

  eStereo stereo_mode;
  ld ipd;
  ld lr_eyewidth, anaglyph_eyewidth;
  ld fov;
  bool consider_shader_projection;
  int desaturate;
  int texture_step;

  
  bool always3;  // always use the 3D engine
  ld depth;      // world level below the plane
  ld camera;     // camera level above the plane
  ld wall_height, creature_scale, height_width;
  eModel vpmodel;
  ld lake_top, lake_bottom;
  ld rock_wall_ratio;
  ld human_wall_ratio;

  int tc_alpha, tc_depth, tc_camera;
  ld highdetail, middetail;
  bool gp_autoscale_heights;
  
  ld eye;
  bool auto_eye;

  ld collignon_parameter; bool collignon_reflected;
  ld plevel_factor;
  };

extern videopar vid;

#define WDIM cginf.g.gameplay_dimension
#define GDIM cginf.g.graphical_dimension
#define MDIM cginf.g.homogeneous_dimension
#define LDIM (MDIM-1)
#define cclass g.kind

#define self (*this)

#define MODFIXER (2*10090080*17)

#define BUGCOLORS 3

#define big_unlock (inv::on && !chaosmode)

// land completion for shared unlocking
#define U5 (big_unlock ? 10 : 5)
// land completion for advanced unlocking
#define U10 (big_unlock ? 25 : 10)

// land completion
#define R10 (big_unlock ? 50 : 10)
// intermediate lands
#define R30 (big_unlock ? 100 : 30)
// advanced lands
#define R60 (big_unlock ? 200 : 60)
// advanced lands II
#define R90 (big_unlock ? 300 : 90)
// Crossroads IV
#define R200 (big_unlock ? 800 : 200)
// Crossroads V
#define R300 (big_unlock ? 1200 : 300)
// kill types for Dragon Chasms
#define R20 (big_unlock ? 30 : 20)
// kill count for Graveyard/Hive
#define R100 (big_unlock ? 500 : 100)

// size casted to int, to prevent warnings and actual errors caused by the unsignedness of x.size()
template<class T> int isize(const T& x) {return x.size(); }

// game forward declarations

namespace anticheat { extern bool tampered; }
#define HRANDMAX 0x7FFFFFFF

struct movedir { 
  int d; 
  // non-negative numbers denote 'rotate +d steps and act in this direction
  // negative numbers have the following meanings (warning: not used consistently):
  #define MD_WAIT (-1)
  #define MD_DROP (-2)
  #define MD_UNDECIDED (-3)
  #define MD_USE_ORB (-4)
  int subdir; // for normal movement (0+): turn left or right
  struct cell *tgt;  // for MD_USE_ORB: target cell
  };

// shmup

template<class T> class hookset : public map<int, function<T>> {};
typedef hookset<void()> *purehookset;

static const int NOHINT = -1;

typedef function<void()> reaction_t;
typedef function<bool()> bool_reaction_t;

#define HELPFUN(x) (help_delegate = x, "HELPFUN")

typedef function<int(struct cell*)> cellfunction;

// passable flags

#define SAGEMELT .1
#define TEMPLE_EACH (among(geometry, gHoroRec, gHoroHex, gKiteDart3) ? 3 : sol ? 6 : (WDIM == 3 && binarytiling) ? 2 : geometry == gSpace435 ? 4 : (WDIM == 3 && hyperbolic) ? 3 : (S3 == OINF) ? 4 : 6)
#define PT(x, y) ((tactic::on || quotient == 2 || daily::on) ? (y) : inv::on ? min(2*(y),x) : (x))
#define ROCKSNAKELENGTH 50
#define WORMLENGTH 15
#define PRIZEMUL 7

#define INF  9999
#define INFD 60
#define PINFD 125
#ifndef BARLEV
#define BARLEV ((ISANDROID||ISIOS||ISFAKEMOBILE||getDistLimit()<7)?(getDistLimit()<4?8:9):10)
#endif
#define BUGLEV 15
// #define BARLEV 9

#define YDIST 101
#define MODECODES (1ll<<61)
  
#define GUNRANGE 3

// loops

#define fakecellloop(ct) for(cell *ct = (cell*)1; ct; ct=NULL)

#define forCellIdAll(ct, i, cf) fakecellloop(ct) for(int i=0; i<(cf)->type && (ct=(cf)->move(i),true); i++) 
#define forCellIdCM(ct, i, cf)  fakecellloop(ct) for(int i=0; i<(cf)->type && (ct=createMov((cf),i),true); i++) 
#define forCellIdEx(ct, i, cf)  forCellIdAll(ct,i,cf) if(ct)

#define forCellEx(ct, cf) forCellIdEx(ct,forCellEx ## __LINE__,cf)
#define forCellCM(ct, cf) forCellIdCM(ct,forCellCM ## __LINE__,cf)
#define forCellAll(ct, cf) forCellIdCM(ct,forCellAll ## __LINE__,cf)

// canAttack/moveval flags

#define AF_NORMAL            0          // nothing special about this attack

#define AF_TOUGH             Flag(0)    // tough attacks: Hyperbugs
#define AF_MAGIC             Flag(1)    // magical attacks: Flash
#define AF_STAB              Flag(2)    // stabbing attacks (usually ignored except Hedgehogs)
#define AF_LANCE             Flag(3)    // lance attacks (used by Lancers)
#define AF_ONLY_ENEMY        Flag(4)    // only say YES if it is an enemy
#define AF_ONLY_FRIEND       Flag(5)    // only say YES if it is a friend
#define AF_ONLY_FBUG         Flag(6)    // only say YES if it is a bug_or friend
#define AF_BACK              Flag(7)    // backward attacks (ignored except Viziers and Flailers)
#define AF_APPROACH          Flag(8)    // approach attacks (ignored except Lancers)
#define AF_IGNORE_UNARMED    Flag(9)    // ignore the UNARMED flag
#define AF_NOSHIELD          Flag(10)   // ignore the shielded status
#define AF_GETPLAYER         Flag(11)   // check for player (replace m2 with moPlayer for player position)
#define AF_GUN               Flag(12)   // revolver attack
#define AF_FAST              Flag(13)   // fast attack
#define AF_EAT               Flag(17)   // eating attacks from Worm-likes

#define MF_NOATTACKS         Flag(14)   // don't do any attacks
#define MF_PATHDIST          Flag(15)   // consider pathdist for moveval
#define MF_ONLYEAGLE         Flag(16)   // do this only for Eagles
#define MF_MOUNT             Flag(18)   // don't do 
#define MF_NOFRIEND          Flag(19)   // don't do it for friends

#define AF_SWORD             Flag(20)   // big sword
#define AF_SWORD_INTO        Flag(21)   // moving into big sword
#define AF_MSG               Flag(22)   // produce a message
#define AF_MUSTKILL          Flag(23)   // when TRUE, stunning attacks are not accepted by canAttack
#define AF_NEXTTURN          Flag(24)   // next turn -- don't count shield at power 1
#define AF_FALL              Flag(25)   // death by falling
#define MF_STUNNED           Flag(26)   // edgeunstable: ignore ladders (as stunned monsters do)
#define MF_IVY               Flag(27)   // edgeunstable: ignore ivy (ivy cannot climb ivy)
#define AF_HORNS             Flag(28)   // spear attack (always has APPROACH too)
#define AF_BULL              Flag(29)   // bull attack
#define AF_SIDE              Flag(30)   // side attack
#define AF_CRUSH             Flag(31)   // Crusher's delayed attack

#if CAP_SDL

#if CAP_PNG
#include "savepng.h"
#define IMAGEEXT ".png"
void IMAGESAVE(SDL_Surface *s, const char *fname);
#else
#define IMAGEEXT ".bmp"
#define IMAGESAVE SDL_SaveBMP
#endif

#endif

// for some reason I need this to compile under OSX

#if ISMAC
extern "C" { void *_Unwind_Resume = 0; }
#endif

template<class T> struct dynamicval {
  T& where;
  T backup;
  dynamicval(T& wh, T val) : where(wh) { backup = wh; wh = val; }
  dynamicval(T& wh) : where(wh) { backup = wh; }
  ~dynamicval() { where = backup; }
  };

static const int MAXPLAYER = 7;

#define DEFAULTCONTROL (multi::players == 1 && !shmup::on && !multi::alwaysuse && !(rug::rugged && rug::renderonce))
#define DEFAULTNOR(sym) (DEFAULTCONTROL || multi::notremapped(sym))

#define CAP_MENUSCALING (ISPANDORA || ISMOBILE)

#if CAP_MENUSCALING
#define displayfrZ dialog::zoom::displayfr
#define displayfrZH dialog::zoom::displayfr_highlight
#else
#define displayfrZ displayfr
#define displayfrZH dialog::zoom::displayfr_highlight
#endif

// just in case if I change my mind about when Orbs lose their power
#define ORBBASE 0

#define mmscale(V, x) (mmspatial ? (ivoryz ? mzscale(V,x) : mscale(V, x)) : (V))

#define SHADOW_WALL 0x60
#define SHADOW_SL   0x18
#define SHADOW_MON  0x30

// ranks:
enum class PPR {
  ZERO, EUCLIDEAN_SKY, OUTCIRCLE, MOVESTAR,
  MINUSINF,
  BELOWBOTTOMm,
  BELOWBOTTOM,
  BELOWBOTTOMp,
  BELOWBOTTOM_FALLANIM,
  LAKEBOTTOM, HELLSPIKE,
  INLAKEWALLm, INLAKEWALL, INLAKEWALLp,
  INLAKEWALL_FALLANIM,
  SUBLAKELEV, LAKELEV, BOATLEV, BOATLEV2, BOATLEV3,
  LAKEWALLm, LAKEWALL, LAKEWALLp,
  LAKEWALL_FALLANIM,
  FLOOR_TOWER,
  FLOOR,
  FLOOR_DRAGON,
  FLOORa, FLOORb, FLOORc, FLOORd,
  LIZEYE,
  BFLOOR,
  GFLOORa, GFLOORb, GFLOORc,
  WALLSHADOW,
  STRUCT0, STRUCT1, STRUCT2, STRUCT3,
  THORNS, WALL,
  REDWALLm, REDWALLs, REDWALLp, REDWALL,
  REDWALLm2, REDWALLs2, REDWALLp2, REDWALLt2,
  REDWALLm3, REDWALLs3, REDWALLp3, REDWALLt3,
  HEPTAMARK,
  ITEM_BELOW,
  ITEM, ITEMa, ITEMb,
  BIGSTATUE,

  WALL3m, WALL3s, WALL3p, WALL3, WALL3A,

// WALL3m, WALL3s, WALL3p, WALL3, WALL3A,
  HIDDEN, GIANTSHADOW,
  TENTACLE0, TENTACLE1,
  ONTENTACLE, ONTENTACLE_EYES, ONTENTACLE_EYES2,
  MONSTER_SHADOW,
  MONSTER_FOOT, MONSTER_LEG, MONSTER_GROIN,
  MONSTER_SUBWPN, MONSTER_WPN,
  MONSTER_BODY, MONSTER_ARMOR0, MONSTER_ARMOR1,
  MONSTER_CLOAK, MONSTER_NECK,
  MONSTER_HEAD, MONSTER_FACE, MONSTER_EYE0, MONSTER_EYE1,
  MONSTER_HAIR, MONSTER_HAT0, MONSTER_HAT1,
  MONSTER_HOODCLOAK1, MONSTER_HOODCLOAK2,
  STUNSTARS,
  CARRIED, CARRIEDa, CARRIEDb,
  PARTICLE, SWORDMARK, MAGICSWORD, MISSILE, SKY,
  MINEMARK, ARROW,
  MOBILE_ARROW,
  LINE, 
  // in depth tested models transparent surfaces need to be depth sorted by HyperRogue
  // and set to PPR::TRANSPARENT_* to draw them after all the opaque ones
  TRANSPARENT_LAKE, TRANSPARENT_SHADOW, TRANSPARENT_WALL,
  // no depth testing for SUPERLINE and above
  SUPERLINE, TEXT, CIRCLE,
  MAX,
  DEFAULT = -1
  };

inline PPR operator + (PPR x, int y) { return PPR(int(x) + y); }
inline PPR operator - (PPR x, int y) { return PPR(int(x) - y); }
inline int operator - (PPR x, PPR y) { return int(x) - int(y); }

#define OUTLINE_NONE     0x000000FF
#define OUTLINE_FRIEND   0x00FF00FF
#define OUTLINE_ENEMY    0xFF0000FF
#define OUTLINE_TREASURE 0xFFFF00FF
#define OUTLINE_ORB      0xFF8000FF
#define OUTLINE_OTHER    0xFFFFFFFF
#define OUTLINE_DEAD     0x800000FF
#define OUTLINE_TRANS    0
#define OUTLINE_DEFAULT  ((bordcolor << 8) + 0xFF)
#define OUTLINE_FORE     ((forecolor << 8) + 0xFF)
#define OUTLINE_BACK     ((backcolor << 8) + 0xFF)

enum orbAction { roMouse, roKeyboard, roCheck, roMouseForce, roMultiCheck, roMultiGo };

#define MODELCOUNT ((int) mdGUARD)

#define pmodel (vid.vpmodel)

color_t darkena(color_t c, int lev, int a);

static const int DISTANCE_UNKNOWN = 127;

#include <functional>

template<class T, class U> int addHook(hookset<T>*& m, int prio, const U& hook) {
  if(!m) m = new hookset<T> ();
  while(m->count(prio)) {
    prio++;
    }
  (*m)[prio] = hook;
  return 0;
  }

template<class T, class... U> void callhooks(hookset<T> *h, U... args) {
  if(h) for(auto& p: *h) p.second(args...);
  }

template<class T, class V, class... U> V callhandlers(V zero, hookset<T> *h, U&... args) {
  if(h) for(auto& p: *h) {
    auto z = p.second(args...);
    if(z != zero) return z;
    }
  return zero;
  }

string XLAT(string);

#define GLERR(call) glError(call, __FILE__, __LINE__)

struct colortable: vector<color_t> {
  color_t& operator [] (int i) { i %= size(); if(i<0) i += size(); return ((vector<color_t>&)(*this)) [i]; }
  const color_t& operator [] (int i) const { i %= size(); if(i<0) i += size(); return ((vector<color_t>&)(*this)) [i]; }
  colortable(std::initializer_list<color_t> v) : vector(v) {}
  colortable() : vector({0}) {}
  };

#define SHMUPTITLE "shoot'em up mode"

// check for a plain number key
#define NUMBERKEY (interpret_as_direction(sym, uni) ? 0 : uni)
#define DKEY (get_direction_key(sym, uni))
#define DIRECTIONKEY (interpret_as_direction(sym, uni) ? uni : 0)

namespace scores { void load(); }

#if ISMOBILE==1
namespace leader { void showMenu(); void handleKey(int sym, int uni); }
#endif

struct hint {
  time_t last;
  function<bool()> usable;
  function<void()> display;
  function<void()> action;  
  };

extern hint hints[];
int counthints();

int textwidth(int siz, const string &str);
#if CAP_GL
int gl_width(int size, const char *s);
#endif

#ifdef ISMOBILE
extern int andmode;
extern bool longclick;
extern bool useRangedOrb;
#endif

static inline bool orbProtection(eItem it) { return false; } // not implemented

#ifndef GL
typedef float GLfloat;
#endif

typedef array<GLfloat, 2> glvec2;
typedef array<GLfloat, 3> glvec3;
typedef array<GLfloat, 4> glvec4;

#if MAXMDIM == 4
#define SHDIM 4
typedef glvec4 glvertex;
#else
#define SHDIM 3
typedef glvec3 glvertex;
#endif

extern int emeraldtable[100][7];

// extern cell *cwpeek(cellwalker cw, int dir);

const eLand NOWALLSEP = laNone;
const eLand NOWALLSEP_USED = laWhirlpool;

#define HAUNTED_RADIUS getDistLimit()
#define UNKNOWN 65535

#define GRAIL_FOUND 0x4000
#define GRAIL_RADIUS_MASK 0x3FFF

extern vector<cell*> dcal;

// z to close to this limit => do not draw

#define BEHIND_LIMIT 1e-6

template<class T, class U> void eliminate_if(vector<T>& data, U pred) {
  for(int i=0; i<isize(data); i++)
    if(pred(data[i]))
      data[i] = data.back(), data.pop_back(), i--;
  }

template<class T> array<T, 4> make_array(T a, T b, T c, T d) { array<T,4> x; x[0] = a; x[1] = b; x[2] = c; x[3] = d; return x; }
template<class T> array<T, 3> make_array(T a, T b, T c) { array<T,3> x; x[0] = a; x[1] = b; x[2] = c; return x; }
template<class T> array<T, 2> make_array(T a, T b) { array<T,2> x; x[0] = a; x[1] = b; return x; }

namespace daily {
  extern bool on;
  extern int daily_id;
  void setup();
  void split();
  void gifts();
  void turnoff();
  void showMenu();
  int find_daily_lbid(int id);
  bool prevent_spawn_treasure_on(cell *c);
  void handleQuit(int sev);
  void uploadscore(bool really_final);
  }

#define RING(i) for(double i=0; i<=cgi.S84+1e-6; i+=SD3 * pow(.5, vid.linequality))
#define REVRING(i) for(double i=cgi.S84; i>=-1e-6; i-=SD3 * pow(.5, vid.linequality))
#define PRING(i) for(double i=0; i<=cgi.S84+1e-6; i+= pow(.5, vid.linequality))
#define REVPRING(i) for(double i=cgi.S84; i>=-1e-6; i-=pow(.5, vid.linequality))

#define ONEMPTY if(d == 7 && passable(c, NULL, 0) && !safety && !reptilecheat)

template <class T> void texture_order(const T& f) {
  const int STEP = vid.texture_step;
  const ld STEP2 = STEP;
  for(int y=0; y<STEP; y++)
  for(int x=0; x<STEP; x++) {
    ld x0 = x / STEP2;
    ld y0 = y / STEP2;
    ld b = 1 / STEP2;

     if(x+y < STEP) {
       f(x0, y0); f(x0+b, y0); f(x0, y0+b);
       }
     if(x+y <= STEP && x && y) {
       f(x0, y0); f(x0-b, y0); f(x0, y0-b);
       }
    }
  }

static const color_t NOCOLOR = 0;

  static const int max_vec = (1<<14);
  extern bool needConfirmationEvenIfSaved();

}

#define IS(z) = z
#include "autohdr.h"
#undef IS
#define IS(z)
#define EX

#endif
