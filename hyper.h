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
#define VER "12.1d"
#define VERNUM_HEX 0xA924

#include "sysconfig.h"

#include <stdarg.h>
#include "hyper_function.h"

/** \brief the main namespace of HyperRogue */
namespace hr {

/** \brief A helper structure that acts as a boolean which is always false. Helpful when disabling stuff with compiler flags. */
struct always_false {
  operator bool() const { return false; }
  bool operator = (bool b) const { return b; }
  };

/** \brief placate GCC's overzealous -Wunused-result */
template<class T>
void ignore(T&&) {
  }

/** \brief a simple static_cast<void*> for use with printf("%p") */
inline const void *voidp(const void *p) {
  return p;
  }

/** \brief Is the value of first parameter equal to one of the remaining parameters? */
template<class T, class V, class... U> bool among(T x, V y) { return x == y; }
template<class T, class V, class... U> bool among(T x, V y, U... u) { return x==y || among(x,u...); }

// functions and types used from the standard library
using std::vector;
using std::map;
using std::array;
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

struct hr_exception : std::runtime_error {
    explicit hr_exception() : std::runtime_error("hr_exception") {}
    explicit hr_exception(const std::string& s) : std::runtime_error(s.c_str()) {}
};

struct hr_shortest_path_exception { };

// genus (in grammar)
#define GEN_M 0
#define GEN_F 1
#define GEN_N 2
#define GEN_O 3
#define GEN_BASE_MASK   7

#define GENF_ELISION    (1 << 3)
#define GENF_PLURALONLY (1 << 4)
#define GENF_PROPER     (1 << 5)

// Add a message to the GUI.
// If multiple messages appear with the same spamtype != 0, the older ones disappear quickly
void addMessage(string s, char spamtype = 0);

// geometry-dependent constants

#define cginf ginf[geometry]

#define S7 cginf.sides
#define S3 cginf.vertex
#define hyperbolic_37 (S7 == 7 && S3 == 3 && !bt::in() && !arcm::in())
#define hyperbolic_not37 ((S7 > 7 || S3 > 3 || bt::in() || arcm::in()) && hyperbolic)
#define weirdhyperbolic ((S7 > 7 || S3 > 3 || !STDVAR || bt::in() || arcm::in() || arb::in()) && hyperbolic)
#define stdhyperbolic (S7 == 7 && S3 == 3 && STDVAR && !bt::in() && !arcm::in() && !arb::in())

#define cgflags cginf.flags 

#define cryst (cgflags & qCRYSTAL)

/** convenience flag for geometries with major aspects missing */
#define experimental (cgflags & qEXPERIMENTAL)

// these geometries do not feature alternate structures for horocycles
#define eubinary (euclid || bt::in() || cryst || nil)

#define cgclass (cginf.cclass)
#define euclid (cgclass == gcEuclid)
#define sphere (cgclass == gcSphere)
#define sol (cgflags & qSOL)
#define nih (cgflags & qNIH)
#define nil (cgclass == gcNil)
#define sl2 (cgclass == gcSL2)
#define prod (cgclass == gcProduct)
#define hybri (cgflags & qHYBRID)
#define rotspace (geometry == gRotSpace)
#define hyperbolic (cgclass == gcHyperbolic)
#define nonisotropic (among(cgclass, gcSolNIH, gcNil, gcSL2))
#define translatable (euclid || nonisotropic)
#define nonorientable (cgflags & qNONORIENTABLE)
#define elliptic (cgflags & qELLIPTIC)
#define quotient (cgflags & qANYQ)
#define smallbounded ((cgflags & qSMALL) || disksize)
#define closed_manifold (cgflags & qCLOSED)
#define closed_or_bounded (closed_manifold || disksize)

// Dry Forest burning, heat transfer, etc. are performed on the whole universe
#define doall (closed_or_bounded)

#define sphere_narcm (sphere && !arcm::in())

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

#define GOLDBERG_INV (GOLDBERG || INVERSE)

#define INVERSE among(variation, eVariation::unrectified, eVariation::warped, eVariation::untruncated )

#define UNRECTIFIED (variation == eVariation::unrectified)
#define WARPED (variation == eVariation::warped)
#define UNTRUNCATED (variation == eVariation::untruncated)

#define GOLDBERG (variation == eVariation::goldberg)
#define IRREGULAR (variation == eVariation::irregular)
#define PURE (variation == eVariation::pure)
#define BITRUNCATED (variation == eVariation::bitruncated)
#define DUAL (variation == eVariation::dual)
#define DUALMUL (DUAL ? 2 : 1)

#define CHANGED_VARIATION (variation != cginf.default_variation)

#define STDVAR (PURE || BITRUNCATED)
#define NONSTDVAR (!STDVAR)

#define VALENCE current_valence()

#define NUMWITCH 7

// achievements

#define LB_YENDOR_CHALLENGE 40
#define LB_PURE_TACTICS 41
#define NUMLEADER 87
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

typedef unsigned color_t;

struct charstyle {
  int charid;
  color_t skincolor, haircolor, dresscolor, swordcolor, dresscolor2, uicolor, eyecolor;
  bool lefthanded;
  };

enum eStereo { sOFF, sAnaglyph, sLR, sODS };

enum eModel : int;

/** configuration of the projection */
struct projection_configuration {
  eModel model;            /**< which projection, see classes.cpp */
  ld xposition, yposition; /**< move the center to another position */
  ld scale, alpha, camera_angle, fisheye_param, twopoint_param, stretch, ballangle, ballproj, euclid_to_sphere;
  ld clip_min, clip_max;
  ld model_orientation, halfplane_scale, model_orientation_yz;  
  ld collignon_parameter; 
  ld aitoff_parameter, miller_parameter, loximuthal_parameter, winkel_parameter;
  bool show_hyperboloid_flat;
  bool collignon_reflected;
  string formula;
  eModel basic_model;
  ld top_z;
  ld model_transition;
  ld spiral_angle;
  ld spiral_x;
  ld spiral_y;
  bool use_atan;
  ld right_spiral_multiplier;
  ld any_spiral_multiplier;
  ld sphere_spiral_multiplier;
  ld spiral_cone;
  ld skiprope;
  ld product_z_scale;
  ld rotational_nil;
  
  ld depth_scaling;
  ld hyperboloid_scaling;
  ld vr_angle, vr_zshift, vr_scale_factor;
  bool dualfocus_autoscale;

  int back_and_front; /* 0 = do not, 1 = do, 2 = only back */

  projection_configuration() { 
    formula = "z^2"; top_z = 5; model_transition = 1; spiral_angle = 70; spiral_x = 10; spiral_y = 7; 
    rotational_nil = 1;
    right_spiral_multiplier = 1;
    any_spiral_multiplier = 1;
    sphere_spiral_multiplier = 2;
    spiral_cone = 360;
    use_atan = false;
    product_z_scale = 1;
    aitoff_parameter = .5;
    miller_parameter = .8;
    loximuthal_parameter = 0;
    winkel_parameter = .5;
    show_hyperboloid_flat = true;
    depth_scaling = 1;
    vr_angle = 0;
    hyperboloid_scaling = 1;
    vr_zshift = 0;
    vr_scale_factor = 1;
    back_and_front = 0;
    dualfocus_autoscale = false;
    }
  };

enum eThreatLevel { tlNoThreat, tlSpam, tlNormal, tlHighThreat };

struct videopar {
  projection_configuration projection_config, rug_config;
  ld yshift;
  ld sspeed, mspeed;
  ld binary_width, fixed_facing_dir;
  int mobilecompasssize;
  int radarsize; // radar for 3D geometries
  ld radarrange;
  int aurastr, aurasmoothen;
  bool fixed_facing;
  bool fixed_yz;
  bool use_wall_radar;
  
  int linequality;

  bool want_fullscreen;
  bool full;
  bool change_fullscr;
  bool relative_window_size;
  bool want_vsync;
  bool current_vsync;
  
  int graphglyph; // graphical glyphs
  bool darkhepta;
  int shifttarget;
  
  int xres, yres, framelimit;
  
  int xscr, yscr;
  
  int fullscreen_x, fullscreen_y;
  int window_x, window_y;
  ld window_rel_x, window_rel_y;
  
  bool grid;
  bool particles;
  
  bool relative_font;
  int fsize, abs_fsize, fontscale;
  int flashtime;
  
  int wallmode, monmode, axes, highlightmode, orbmode;
  bool axes3;
  bool revcontrol;
  
  int msgleft, msglimit;

  bool wantGL;
  int want_antialias;
  bool fineline;
  
  bool usingGL;
  int antialias;
  #define AA_NOGL      1
  #define AA_VERSION   2
  #define AA_LINES     4
  #define AA_POLY      8
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
  bool smart_area_based;// based on area or length?
  int cells_drawn_limit;
  int cells_generated_limit; // limit on cells generated per frame
  
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
  ld lake_top, lake_bottom;
  ld rock_wall_ratio;
  ld human_wall_ratio;
  bool pseudohedral; // in 3D modes
  ld depth_bonus;   // to fiix the placement of 3D models in pseudogonal -- not working currently

  int tc_alpha, tc_depth, tc_camera;
  ld highdetail, middetail;
  bool gp_autoscale_heights;
  
  ld eye;
  bool auto_eye;

  ld plevel_factor;
  bool bubbles_special, bubbles_threshold, bubbles_all;
  int joysmooth;
  
  eThreatLevel faraway_highlight; // draw attention to monsters on the horizon
  int faraway_highlight_color; // 0 = monster color, 100 = red-green oscillation

  ld ispeed;
  bool flasheffects;
  };

extern videopar vid;

/** \brief How many dimensional is the gameplay. In the FPP mode of a 2D geometry, WDIM is 2 */
#define WDIM cginf.g.gameplay_dimension
/** \brief How many dimensional is the graphical representation. In the FPP mode of a 2D geometry, MDIM is 3 */
#define GDIM cginf.g.graphical_dimension
/** \brief How many dimensions of the matrix representation are used. It is usually 3 in 2D geometries (not FPP) and in product geometries, 4 in 3D geometries */
#define MDIM (MAXMDIM == 3 ? 3 : cginf.g.homogeneous_dimension)
/** \brief What dimension of matrices is used in loops (the 'extra' dimensions have values 0 or 1 as in Id)
 *  Even if MDIM==3, it may be faster to keep 4x4 matrices and perform computations using them (rather than having another condition due to the variable loop size). 
 *  The experiments on my computer show it to be the case, but the effect is not significant, and it may be different on another computer.
 */
#define MXDIM (CAP_MDIM_FIXED ? MAXMDIM : MDIM)
/** \brief The 'homogeneous' dimension index */
#define LDIM (MDIM-1)
#define cclass g.kind

#define self (*this)

#define BUGCOLORS 3

#define big_unlock (inv::on && !ls::any_chaos())

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

// automatically growing vector
template<class T> struct grow_vector : public vector<T> {
  T& grow(size_t index) {
    if(index >= this->size()) {
      this->resize(index + 1);
      }
    return (vector<T>::operator[]) (index);
    }
  };

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

template<class T>
class hookset {
    std::map<int, std::function<T>> *map_ = nullptr;

public:
    template<class U>
    int add(int prio, U&& hook) {
        if (map_ == nullptr) map_ = new std::map<int, std::function<T>>();
        while (map_->count(prio)) {
            prio++;
        }
        map_->emplace(prio, static_cast<U&&>(hook));
        return prio;
    }

    void del(int prio) {
        map_->erase(prio);
        }

    template<class... U>
    void callhooks(U&&... args) const {
        if (map_ == nullptr) return;
        for (const auto& p : *map_) {
            p.second(static_cast<U&&>(args)...);
        }
    }

    template<class V, class... U>
    V callhandlers(V zero, U&&... args) const {
        if (map_ == nullptr) return zero;
        for (const auto& p : *map_) {
            auto z = p.second(static_cast<U&&>(args)...);
            if (z != zero) return z;
        }
        return zero;
    }
};

using purehookset = hookset<void()>;

static const int NOHINT = -1;

typedef function<void()> reaction_t;
typedef function<bool()> bool_reaction_t;

void offer_choose_file(reaction_t r);

#define HELPFUN(x) (help_delegate = x, "HELPFUN")

typedef function<int(struct cell*)> cellfunction;

// passable flags

#define SAGEMELT .1
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
#define AF_PLAGUE            Flag(32)   // Orb of Plague (do not check adjacency)
#define AF_PSI               Flag(33)   // Orb of the Mind
#define AF_WEAK              Flag(34)   // Curse of Weakness

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

template<class T> struct dynamicval {
  T& where;
  T backup;
  dynamicval(T& wh, T val) : where(wh) { backup = wh; wh = val; }
  dynamicval(T& wh) : where(wh) { backup = wh; }
  ~dynamicval() { where = backup; }
  };

struct finalizer {
  reaction_t f;
  finalizer(reaction_t r) : f(r) {}
  ~finalizer() { f(); }
  };
  
static const int MAXPLAYER = 7;

#define DEFAULTCONTROL (multi::players == 1 && !shmup::on && !multi::alwaysuse)
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
  BSHALLOW, SHALLOW, ASHALLOW,
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

#define pconf vid.projection_config
#if CAP_RUG
#define vpconf (rug::rugged ? vid.rug_config : vid.projection_config)
#else
#define vpconf pconf
#endif
#define pmodel (pconf.model)

static const int DISTANCE_UNKNOWN = 127;

template<class T, class U> int addHook(hookset<T>& m, int prio, U&& hook) {
  return m.add(prio, static_cast<U&&>(hook));
  }

template<class T> void delHook(hookset<T>& m, int prio) {
  m.del(prio);
  }

template<class T, class... U> void callhooks(const hookset<T>& h, U&&... args) {
  return h.callhooks(static_cast<U&&>(args)...);
  }

template<class T, class V, class... U> V callhandlers(V zero, const hookset<T>& h, U&&... args) {
  return h.callhandlers(zero, static_cast<U&&>(args)...);
  }

string XLAT(string);

#define GLERR(call) glError(call, __FILE__, __LINE__)

#define SHMUPTITLE "shoot'em up mode"

// check for a plain number key
#define NUMBERKEY (interpret_as_direction(sym, uni) ? 0 : uni)
#define DKEY (get_direction_key(sym, uni))
#define DIRECTIONKEY (interpret_as_direction(sym, uni) ? uni : 0)

namespace scores { void load(); }

#if ISMOBILE
namespace leader { void showMenu(); void handleKey(int sym, int uni); }
#endif

int textwidth(int siz, const string &str);
#if CAP_GL
int gl_width(int size, const char *s);
#endif

#if ISMOBILE
extern int andmode;
extern bool longclick;
extern bool useRangedOrb;
#endif

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

// Find in a std::map or std::unordered_map, or return null.
template<class Map, class Key>
const typename Map::mapped_type *at_or_null(const Map& map, const Key& key) {
  auto it = map.find(key);
  return (it == map.end()) ? nullptr : &it->second;
  }

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

/** find the smallest value of x in range [dmin..dmax] such that f(x) returns true */

template<class T> ld binsearch(ld dmin, ld dmax, const T& f, int iterations = 200) {
  for(int i=0; i<iterations; i++) {
    ld d = (dmin + dmax) / 2;
    if(dmin == d || dmax == d) break;
    if(f(d)) dmax = d;
    else dmin = d;
    }
  return dmin;
  } 

  static const int max_vec = (1<<14);
  extern bool needConfirmationEvenIfSaved();

typedef unsigned long long flagtype;
#define Flag(i) (flagtype(1ull<<i))
static inline void set_flag(flagtype& f, flagtype which, bool b) {
  if(b) f |= which;
  else f &= ~which;
  }

}

/** this macro is used to delay performing the action in case if everything is rolled back */
#define LATE(x) \
  if(changes.on) { changes.at_commit([=] { x; }); return; }

// assert macro
#ifdef NDEBUG
#define hassert(condition) if(!(condition)) __builtin_unreachable()
#else
#define hassert(condition) if(!(condition)) printf("%s:%d:%s: assertion failed: %s\n", __FILE__, __LINE__, __func__, #condition)
#endif

#define IS(z) = z
#include "autohdr.h"
#undef IS
#define IS(z)
#define EX

#endif
