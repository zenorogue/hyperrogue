// This is the main header file of HyperRogue. Mostly everything is dumped here.
// It is quite chaotic.

// version numbers
#define VER "11.1i"
#define VERNUM_HEX 0xA709

#include <stdarg.h>
#include "hyper_function.h"

namespace hr {

struct always_false {
  operator bool() const { return false; };
  void operator = (bool b) const {};
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

#define ALPHA (M_PI*2/S7)
#define S7 ginf[geometry].sides
#define S3 ginf[geometry].vertex
#define hyperbolic_37 (S7 == 7 && S3 == 3 && !binarytiling && !archimedean)
#define hyperbolic_not37 ((S7 > 7 || S3 > 3 || binarytiling || archimedean) && hyperbolic)
#define weirdhyperbolic ((S7 > 7 || S3 > 3 || !STDVAR || binarytiling || archimedean) && hyperbolic)
#define stdhyperbolic (S7 == 7 && S3 == 3 && STDVAR && !binarytiling && !archimedean)

#define binarytiling (ginf[geometry].flags & qBINARY)
#define archimedean (geometry == gArchimedean)
#define penrose (ginf[geometry].flags & qPENROSE)

// these geometries do not feature alternate structures for horocycles
#define eubinary (euclid || binarytiling || geometry == gCrystal || nil)

#define cgclass (ginf[geometry].cclass)
#define euclid (cgclass == gcEuclid)
#define sphere (cgclass == gcSphere)
#define sol (cgclass == gcSol)
#define nil (cgclass == gcNil)
#define hyperbolic (cgclass == gcHyperbolic)
#define nonisotropic (sol || nil)
#define translatable (euclid || nonisotropic)
#define nonorientable (ginf[geometry].flags & qNONORIENTABLE)
#define elliptic (ginf[geometry].flags & qELLIPTIC)
#define quotient (ginf[geometry].flags & qANYQ)
#define euwrap (quotient && euclid)
#define fulltorus (bounded && euclid)
#define smallbounded (ginf[geometry].flags & qSMALL)
#define bounded (ginf[geometry].flags & qBOUNDED)

// Dry Forest burning, heat transfer, etc. are performed on the whole universe
#define doall (bounded)

// These geometries are generated without the heptagon structure. 
// 'master' holds the coordinates
#define masterless among(geometry, gEuclid, gEuclidSquare, gTorus)

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
#define euclid4 (masterless && a4)
#define euclid6 (masterless && !a4)

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

#define CHANGED_VARIATION (variation != ginf[geometry].default_variation)

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

static const int MAXPLAYER = 7;
static const int MAXJOY = 8;
static const int MAXBUTTON = 64;
static const int MAXAXE = 16;
static const int MAXHAT = 4;

namespace multi {

  struct config {
    char keyaction[512];
    char joyaction[MAXJOY][MAXBUTTON];
    char axeaction[MAXJOY][MAXAXE];
    char hataction[MAXJOY][MAXHAT][4];
    int  deadzoneval[MAXJOY][MAXAXE];
    };

  extern config scfg;  
  }

enum eStereo { sOFF, sAnaglyph, sLR, sODS };

struct videopar {
  ld scale, alpha, sspeed, mspeed, yshift, camera_angle;
  ld ballangle, ballproj, euclid_to_sphere, twopoint_param, stretch, binary_width, fixed_facing_dir;
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
  };

extern videopar vid;

#if MAXMDIM == 3
#define WDIM 2
#else
#define WDIM ((geometry >= gBinary3 && geometry != gBinary4 && geometry != gKiteDart2 PROD(&& geometry != gProduct)) ? 3 : 2)
#endif
#define GDIM (vid.always3 ? 3 : WDIM)
#define DIM GDIM
#define MDIM (DIM+1)

#define self (*this)

struct hyperpoint : array<ld, MAXMDIM> {
  hyperpoint() {}
  
  hyperpoint(ld x, ld y, ld z, ld w) { 
    self[0] = x; self[1] = y; self[2] = z; 
    if(MAXMDIM == 4) self[3] = w;
    }

  inline hyperpoint& operator *= (ld d) {
    for(int i=0; i<MDIM; i++) self[i] *= d;
    return self;
    }
  
  inline hyperpoint& operator /= (ld d) { 
    for(int i=0; i<MDIM; i++) self[i] /= d;
    return self;
    }
  
  inline hyperpoint& operator += (const hyperpoint h2) { 
    for(int i=0; i<MDIM; i++) self[i] += h2[i];
    return self;
    }

  inline hyperpoint& operator -= (const hyperpoint h2) { 
    for(int i=0; i<MDIM; i++) self[i] -= h2[i];
    return self;
    }

  inline friend hyperpoint operator * (ld d, hyperpoint h) { return h *= d; }  
  inline friend hyperpoint operator * (hyperpoint h, ld d) { return h *= d; }  
  inline friend hyperpoint operator / (hyperpoint h, ld d) { return h /= d; }  
  inline friend hyperpoint operator + (hyperpoint h, hyperpoint h2) { return h += h2; }
  inline friend hyperpoint operator - (hyperpoint h, hyperpoint h2) { return h -= h2; }

  // cross product  
  inline friend hyperpoint operator ^ (hyperpoint h1, hyperpoint h2) {
    return hyperpoint(
      h1[1] * h2[2] - h1[2] * h2[1],
      h1[2] * h2[0] - h1[0] * h2[2],
      h1[0] * h2[1] - h1[1] * h2[0],
      0
      );
    }

  // inner product
  inline friend ld operator | (hyperpoint h1, hyperpoint h2) {
    ld sum = 0;
    for(int i=0; i<MDIM; i++) sum += h1[i] * h2[i];
    return sum;
    }    
  };

struct transmatrix {
  ld tab[MAXMDIM][MAXMDIM];
  hyperpoint& operator [] (int i) { return (hyperpoint&)tab[i][0]; }
  const ld * operator [] (int i) const { return tab[i]; }
  
  inline friend hyperpoint operator * (const transmatrix& T, const hyperpoint& H) {
    hyperpoint z;
    for(int i=0; i<MDIM; i++) {
      z[i] = 0;
      for(int j=0; j<MDIM; j++) z[i] += T[i][j] * H[j];
      }
    return z;
    }

  inline friend transmatrix operator * (const transmatrix& T, const transmatrix& U) {
    transmatrix R;
    for(int i=0; i<MDIM; i++) for(int j=0; j<MDIM; j++) {
      R[i][j] = 0;
      for(int k=0; k<MDIM; k++)
        R[i][j] += T[i][k] * U[k][j];
      }
    return R;
    }  
  };


constexpr transmatrix diag(ld a, ld b, ld c, ld d) {
  #if MAXMDIM==3
  return transmatrix{{{a,0,0}, {0,b,0}, {0,0,c}}};
  #else
  return transmatrix{{{a,0,0,0}, {0,b,0,0}, {0,0,c,0}, {0,0,0,d}}};
  #endif
  }

const static hyperpoint Hypc = hyperpoint(0, 0, 0, 0);

// identity matrix
const static transmatrix Id = diag(1,1,1,1);

// zero matrix
const static transmatrix Zero = diag(0,0,0,0);

// mirror image
const static transmatrix Mirror = diag(1,-1,1,1);
const static transmatrix MirrorY = diag(1,-1,1,1);

// mirror image
const static transmatrix MirrorX = diag(-1,1,1,1);

// mirror image
const static transmatrix MirrorZ = diag(1,1,-1,1);

// rotate by PI
const static transmatrix pispin = diag(-1,-1,1,1);

// central symmetry
const static transmatrix centralsym = diag(-1,-1,-1,-1);

inline hyperpoint hpxyz(ld x, ld y, ld z) { return DIM == 2 ? hyperpoint(x,y,z,0) : hyperpoint(x,y,0,z); }
inline hyperpoint hpxyz3(ld x, ld y, ld z, ld w) { return DIM == 2 ? hyperpoint(x,y,w,0) : hyperpoint(x,y,z,w); }
inline hyperpoint point3(ld x, ld y, ld z) { return hyperpoint(x,y,z,0); }
inline hyperpoint point31(ld x, ld y, ld z) { return hyperpoint(x,y,z,1); }
inline hyperpoint point2(ld x, ld y) { return hyperpoint(x,y,0,0); }

extern int cellcount, heptacount;
extern color_t forecolor;
extern ld band_shift;
// cell information for the game

struct gcell {

#if CAP_BITFIELD
  // main fields
  eLand land : 8;
  eWall wall : 8;
  eMonster monst : 8;
  eItem item : 8;

  // if this is a barrier, what lands on are on the sides?
  eLand barleft : 8, barright : 8; 

  unsigned ligon : 1;    // is it sparkling with lightning?

  signed 
    mpdist : 7,
    pathdist : 8,       // player distance wrt usual movement
    cpdist : 8;         // current/minimum player distance

  unsigned 
    mondir : 8,         // monster direction, for multi-tile monsters and graphics
    bardir : 8,         // barrier direction
    stuntime : 8,       // stun time left (for Palace Guards and Skeletons)
    hitpoints : 7,      // hitpoints left (for Palace Guards, also reused as cpid for mirrors)
    monmirror : 1;      // monster mirroring state for nonorientable geometries
  
  unsigned landflags : 8;      // extra flags for land
#else
  eLand land;
  eWall wall;
  eMonster monst;
  eItem item;
  eLand barleft, barright;
  bool ligon, monmirror;
  signed char pathdist, cpdist, mpdist;
  
  unsigned char mondir, bardir, stuntime, hitpoints;
  unsigned char landflags;
#endif
  
  // 'landparam' is used for: 
  // heat in Icy/Cocytus; 
  // heat in Dry (0..10); 
  // CR2 structure; 
  // hive Weird Rock color / pheromones;
  // Ocean/coast depth;
  // Bomberbird Egg hatch time / mine marking;
  // number of Ancient Jewelry;
  // improved tracking in Trollheim
  union { 
    int32_t landpar; 
    unsigned int landpar_color;
    float heat; 
    char bytes[4]; 
    struct fieldinfo { 
      uint16_t fieldval;
      unsigned rval : 4;
      unsigned flowerdist : 4;
      unsigned walldist : 4;
      unsigned walldist2 : 4;
      } fi;
  
  } LHU;
  
  #ifdef CELLID
  int cellid;
  #endif
  
  gcell() { cellcount++; 
    #ifdef CELLID
    cellid = cellcount;  
    #endif
    }
  ~gcell() { cellcount--; }
  };

#define landparam LHU.landpar
#define landparam_color LHU.landpar_color

#define fval LHU.fi.fieldval

#define NODIR 126
#define NOBARRIERS 127
#define MODFIXER (2*10090080*17)

#define MAX_EDGE 18

template<class T> struct walker;

template<class T> struct connection_table {

  // Assumption: class T has a field c of type connection_table<T>.

  // NOTE: since aconnection_table may be allocated with
  // less than MAX_EDGE neighbors (see tailored_alloc),
  // the order of fields matters.

  T* move_table[MAX_EDGE + (MAX_EDGE + sizeof(char*) - 1) / sizeof(char*)];
  
  unsigned char *spintable() { return (unsigned char*) (&move_table[full()->degree()]); }

  T* full() { T* x = (T*) this; return (T*)((char*)this - ((char*)(&(x->c)) - (char*)x)); }
  void setspin(int d, int spin, bool mirror) { 
    unsigned char& c = spintable() [d];
    c = spin;
    if(mirror) c |= 128;
    }
  // we are spin(i)-th neighbor of move[i]
  int spin(int d) { return spintable() [d] & 127; }
  bool mirror(int d) { return spintable() [d] & 128; }  
  int fix(int d) { return (d + MODFIXER) % full()->degree(); }
  T*& modmove(int i) { return move(fix(i)); }
  T*& move(int i) { return move_table[i]; }
  unsigned char modspin(int i) { return spin(fix(i)); }
  void fullclear() { 
    for(int i=0; i<full()->degree(); i++) move_table[i] = NULL;
    }
  void connect(int d0, T* c1, int d1, bool m) {
    move(d0) = c1;
    c1->move(d1) = full();
    setspin(d0, d1, m);
    c1->c.setspin(d1, d0, m);    
    }
  void connect(int d0, walker<T> hs) {
    connect(d0, hs.at, hs.spin, hs.mirrored);
    }
  };

// Allocate a class T with a connection_table, but 
// with only `degree` connections. Also set yet
// unknown connections to NULL.

// Generating the hyperbolic world consumes lots of
// RAM, so we really need to be careful on low memory devices. 

template<class T> T* tailored_alloc(int degree) {
  const T* sample = (T*) &degree;
  T* result;
#ifndef NO_TAILORED_ALLOC
  int b = (char*)&sample->c.move_table[degree] + degree - (char*) sample;
  result = (T*) new char[b];
  new (result) T();
#else
  result = new T;
#endif
  result->type = degree;
  for(int i=0; i<degree; i++) result->c.move_table[i] = NULL;
  return result;
  }

template<class T> void tailored_delete(T* x) {
  x->~T();  
  delete[] ((char*) (x));
  }

static const struct wstep_t { wstep_t() {} } wstep;
static const struct wmirror_t { wmirror_t() {}} wmirror;
static const struct rev_t { rev_t() {} } rev;
static const struct revstep_t { revstep_t() {}} revstep;

int hrand(int x);

// automatically adjust monster generation for 3D geometries
int hrand_monster(int x);

vector<int> reverse_directions(struct cell *c, int i);

// unused for heptagons
inline vector<int> reverse_directions(struct heptagon *c, int i) { return {i}; }

template<class T> struct walker {
  T *at;
  int spin;
  bool mirrored;
  walker<T> (T *at = NULL, int s = 0, bool m = false) : at(at), spin(s), mirrored(m) { if(at) s = at->c.fix(s); }
  walker<T>& operator += (int i) {
    spin = at->c.fix(spin+(mirrored?-i:i));
    return (*this);
    }
  walker<T>& operator -= (int i) {
    spin = at->c.fix(spin-(mirrored?-i:i));
    return (*this);
    }
  walker<T>& operator += (wmirror_t) {
    mirrored = !mirrored;
    return (*this);
    }
  walker<T>& operator += (wstep_t) {
    at->cmove(spin);
    int nspin = at->c.spin(spin);
    if(at->c.mirror(spin)) mirrored = !mirrored;
    at = at->move(spin);
    spin = nspin;
    return (*this);
    }
  walker<T>& operator += (rev_t) {
    auto rd = reverse_directions(at, spin);
    if(rd.size() == 1) spin = rd[0];
    else spin = rd[hrand(rd.size())];
    return (*this);
    }
  walker<T>& operator += (revstep_t) {
    (*this) += rev; return (*this) += wstep; 
    }
  bool operator != (const walker<T>& x) const {
    return at != x.at || spin != x.spin || mirrored != x.mirrored;
    }
  bool operator == (const walker<T>& x) const {
    return at == x.at && spin == x.spin && mirrored == x.mirrored;
    }

  bool operator < (const walker<T>& cw2) const {
    return tie(at, spin, mirrored) < tie(cw2.at, cw2.spin, cw2.mirrored);
    }

  walker<T>& operator ++ (int) { return (*this) += 1; }
  walker<T>& operator -- (int) { return (*this) -= 1; }
  template<class U> walker operator + (U t) const { walker<T> w = *this; w += t; return w; }
  template<class U> walker operator - (U t) const { walker<T> w = *this; w += (-t); return w; }
  T*& peek() { return at->move(spin); }
  T* cpeek() { return at->cmove(spin); }
  bool creates() { return !peek(); }
  walker<T> mirrorat(int d) { return walker<T> (at, at->c.fix(d+d - spin), !mirrored); }
  };

struct cell;

// automaton state
enum hstate { hsOrigin, hsA, hsB, hsError, hsA0, hsA1, hsB0, hsB1, hsC };

struct cell *createMov(struct cell *c, int d);
struct heptagon *createStep(struct heptagon *c, int d);

struct cdata {
  int val[4];
  int bits;
  };

// in bitruncated/irregular/Goldberg geometries, heptagons form the 
// underlying regular tiling (not necessarily heptagonal); in pure
// geometries, they correspond 1-1 to tiles; in 'masterless' geometries
// heptagons are unused

struct heptagon {
  // automaton state
  hstate s : 6;
  unsigned int dm4: 2;
  // distance from the origin
  short distance;
  // note: all the 'val' values may have different meaning in other geometries
  // emerald/wineyard generator
  short emeraldval;
  // fifty generator
  short fiftyval;
  // zebra generator (1B actually)
  short zebraval;
  // field id
  int fieldval : 24;
  // degree
  unsigned char type : 8;
  // data for fractal landscapes
  short rval0, rval1;
  // for alternate structures, cdata contains the pointer to the original
  // for the main map, it contains the fractal landscape data
  struct cdata *cdata;
  // central cell of this underlying tiling
  cell *c7;
  // associated generator of alternate structure, for Camelot and horocycles
  heptagon *alt;
  // connection table
  connection_table<heptagon> c;
  heptagon*& move(int d) { return c.move(d); }
  heptagon*& modmove(int d) { return c.modmove(d); }
  // functions
  heptagon () { heptacount++; }
  ~heptagon () { heptacount--; }
  heptagon *cmove(int d) { return createStep(this, d); }
  heptagon *cmodmove(int d) { return createStep(this, c.fix(d)); }
  inline int degree() { return type; }

  // prevent accidental copying
  heptagon(const heptagon&) = delete;
  heptagon& operator=(const heptagon&) = delete;
  // do not add any fields after connection_table (see tailored_alloc)
  };

struct cell : gcell {
  char type; int degree() { return type; }

  // wall parameter, used for remaining power of Bonfires and Thumpers
  char wparam;

  // used by celllister
  int listindex;

  // heptagon who owns us; for 'masterless' tilings it contains coordinates instead
  heptagon *master;

  connection_table<cell> c;
  cell*& move(int d) { return c.move(d); }
  cell*& modmove(int d) { return c.modmove(d); }
  cell* cmove(int d) { return createMov(this, d); }
  cell* cmodmove(int d) { return createMov(this, c.fix(d)); }
  cell() {}

  // prevent accidental copying
  cell(const cell&) = delete;
  heptagon& operator=(const cell&) = delete;
  // do not add any fields after connection_table (see tailored_alloc)
  };

/*
namespace arcm { int degree(heptagon *h); int valence(); }

int heptagon::degree() { 
  #if CAP_ARCM
  if(archimedean) return arcm::degree(this); else 
  #endif
  return S7; 
  } */

typedef walker<heptagon> heptspin;
typedef walker<cell> cellwalker;

static const struct cth_t { cth_t() {}} cth;
inline heptspin operator+ (cellwalker cw, cth_t) { return heptspin(cw.at->master, cw.spin * DUALMUL, cw.mirrored); }
inline cellwalker operator+ (heptspin hs, cth_t) { return cellwalker(hs.at->c7, hs.spin / DUALMUL, hs.mirrored); }

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

string XLAT(string x);    // translate the sentence x
string XLATN(string x);   // translate the sentence x
string cts(char c);       // character to string
string its(int i);        // int to string
string itsh8(int i);      // int to string (8 hex digits)
string itsh(int i);       // int to string

// size casted to int, to prevent warnings and actual errors caused by the unsignedness of x.size()
template<class T> int isize(const T& x) {return x.size(); }

// game forward declarations

namespace anticheat { extern bool tampered; }
#define HRANDMAX 0x7FFFFFFF

namespace rg {
  // possible parameters e.g. for restart_game and wrongmode
  static const char nothing = 0;
  static const char peace = 'P';
  static const char inv = 'i';
  static const char chaos = 'C';
  static const char tactic = 't';
  static const char tour = 'T';
  static const char yendor = 'y';
  static const char shmup = 's';
  static const char randpattern = 'r';
  static const char princess = 'p';
  static const char daily = 'd';
  static const char daily_off = 'D';
  static const char racing = 'R';
  static const char dualmode = 'U';
  
  // wrongmode only -- marks 'global' achievements not related to the current mode
  static const char global = 'x'; 
  // wrongmode only -- change vid.scfg.players then restart_game(rg::nothing) instead
  static const char multi = 'm';
  // wrongmode only -- mark achievements for special geometries/variations
  static const char special_geometry = 'g';
  }

enum orbAction { roMouse, roKeyboard, roCheck, roMouseForce, roMultiCheck, roMultiGo };

namespace hive { void createBugArmy(cell *c); }
namespace whirlpool { void generate(cell *wto); }
namespace whirlwind { void generate(cell *wto); }
namespace mirror { 
  static const int SPINSINGLE = 1;
  static const int SPINMULTI = 2;
  static const int GO = 4;
  static const int ATTACK = 8;
    
  void act(int dir, int flags);
  }

int neighborId(cell *c1, cell *c2);

struct movedir { 
  int d; 
  // non-negative numbers denote 'rotate +d steps and act in this direction
  // negative numbers have the following meanings (warning: not used consistently):
  #define MD_WAIT (-1)
  #define MD_DROP (-2)
  #define MD_UNDECIDED (-3)
  #define MD_USE_ORB (-4)
  int subdir; // for normal movement (0+): turn left or right
  cell *tgt;  // for MD_USE_ORB: target cell
  };

void activateActiv(cell *c, bool msg);

// shmup

string csname(charstyle& cs);
void initcs(charstyle& cs);

extern bool flipplayer;

template<class T> class hookset : public map<int, function<T>> {};
typedef hookset<void()> *purehookset;

static const int NOHINT = -1;

typedef function<void()> reaction_t;
typedef function<bool()> bool_reaction_t;

#define HELPFUN(x) (help_delegate = x, "HELPFUN")

extern vector< function<void()> > screens;

template<class T> void pushScreen(const T& x) { screens.push_back(x); } 
inline void popScreen() { if(isize(screens)>1) screens.pop_back(); }
inline void popScreenAll() { while(isize(screens)>1) popScreen(); }

struct display_data {
  transmatrix view_matrix; // current rotation, relative to viewctr
  transmatrix player_matrix; // player-relative view
  heptspin view_center;
  cellwalker precise_center;
  unordered_map<cell*, transmatrix> cellmatrices, old_cellmatrices;
  ld xmin, ymin, xmax, ymax; // relative
  ld xtop, ytop, xsize, ysize; // in pixels
  display_data() { xmin = ymin = 0; xmax = ymax = 1; }

  // paramaters calculated from the above
  int xcenter, ycenter;
  ld radius;
  int scrsize;  
  bool sidescreen;

  ld tanfov;

  GLfloat scrdist, scrdist_text;

  ld eyewidth();
  bool stereo_active();
  bool in_anaglyph();

  void set_viewport(int ed);
  void set_projection(int ed);
  void set_mask(int ed);

  void set_all(int ed);
  };

extern display_data default_display;
extern display_data *current_display;

#define View (current_display->view_matrix)
#define cwtV (current_display->player_matrix)
#define viewctr (current_display->view_center)
#define centerover (current_display->precise_center)
#define gmatrix (current_display->cellmatrices)
#define gmatrix0 (current_display->old_cellmatrices)

typedef function<int(cell*)> cellfunction;

namespace mapeditor { 
#if CAP_EDIT
  extern map<int, cell*> modelcell;
#endif

  extern bool drawplayer; 
  void applyModelcell(cell *c);
  
  extern cell *drawcell;
  void initdraw(cell *c); 
  #if CAP_EDIT
  void showMapEditor();
  void showDrawEditor();
  #endif
  
  enum eShapegroup { sgPlayer, sgMonster, sgItem, sgFloor, sgWall };
  static const int USERSHAPEGROUPS = 5;

  bool haveUserShape(eShapegroup group, int id);  
  void draw_texture_ghosts(cell *c, const transmatrix& V);  
  void map_settings();
  }

struct renderbuffer;

namespace rug {
  extern bool display_warning;
  extern bool rugged;
  extern bool spatial_rug;
  extern bool computed;
  extern bool renderonce;
  extern bool rendernogl;
  extern int  texturesize;
  extern ld   model_distance;
  extern ld   modelscale;
  extern transmatrix currentrot;
#if CAP_RUG
  void show();
  // initialize both the texture and the model
  void init();
  // initialize only the texture (assume model already initialized)
  void reopen();
  // close the rug mode, remove the texture
  void close();
  // clear the model
  void clear_model();
  void actDraw();
  void select();
  void buildVertexInfo(cell *c, transmatrix V);
  void drawRugScene();
  void prepareTexture();
  void drawRugScene();
  void push_all_points(int coord, ld val);
  void apply_rotation(const transmatrix& t);
  string makehelp();

  struct edge {
    struct rugpoint *target;
    ld len;
    };
  
  struct dexp_data {
    hyperpoint params;
    hyperpoint cont;
    ld remaining_distance;
    };

  struct rugpoint {
    double x1, y1;
    bool valid;
    bool inqueue;
    double dist;
    hyperpoint h;    // point in the represented space
    hyperpoint flat; // point in the native space, in azeq
    hyperpoint precompute;
    vector<edge> edges;
    vector<edge> anticusp_edges;
    // Find-Union algorithm
    rugpoint *glue;
    rugpoint *getglue() {
      return glue ? (glue = glue->getglue()) : this;
      }
    hyperpoint& glueflat() {
      return glue->flat;
      }
    rugpoint() { glue = NULL; }
    void glueto(rugpoint *x) {
      x = x->getglue();
      auto y = getglue();
      if(x != y) y->glue = x;
      }
    int dexp_id;
    dexp_data surface_point;
    };

  struct triangle {
    rugpoint *m[3];
    triangle(rugpoint *m1, rugpoint *m2, rugpoint *m3) {
      m[0] = m1; m[1] = m2; m[2] = m3;
      }
    };

  extern vector<rugpoint*> points;
  extern vector<triangle> triangles;

  extern int qvalid;
  extern bool subdivide_further();
  extern void subdivide();
  extern bool good_shape;
  extern int vertex_limit;
  extern void enqueue(rugpoint *p);
  void sort_rug_points();
  extern bool rug_perspective;
  
  bool handlekeys(int sym, int uni);

  void addTriangle(rugpoint *t1, rugpoint *t2, rugpoint *t3, ld len = 1);
  rugpoint *addRugpoint(hyperpoint h, double dist);
  void buildRug();

  bool in_crystal();
  void physics();

extern renderbuffer *glbuf; 
extern eGeometry gwhere;
extern bool no_fog;
extern ld lowrug, hirug, ruggospeed;
extern GLuint alternate_texture;
extern bool invert_depth;
extern ld ruggo;
#endif
  }

#define HASLINEVIEW

namespace shmup { struct monster; }

namespace conformal {
  extern bool on;
  extern vector<pair<cell*, eMonster> > killhistory;
  extern vector<pair<cell*, eItem> > findhistory;  
  extern vector<cell*> movehistory;
  extern set<cell*> inmovehistory, inkillhistory, infindhistory;
  extern bool includeHistory;
  extern bool use_atan;
  extern ld rotation;
  extern ld rotation_xz, rotation_xy2;
  transmatrix rotmatrix();
  extern int do_rotate;
  extern ld model_orientation, model_orientation_yz;
  extern ld halfplane_scale;
  extern ld ocos, osin, ocos_yz, osin_yz;
  extern ld cos_ball, sin_ball;
  extern bool model_straight, model_straight_yz;
  extern ld model_transition;
  extern ld top_z;
  extern ld spiral_angle, spiral_x, spiral_y;
  extern ld spiral_cone;
  extern ld clip_min, clip_max;
  
  // screen coordinates to logical coordinates: apply_orientation(x,y)
  // logical coordinates back to screen coordinates: apply_orientation(y,x)
  template<class A>
  void apply_orientation(A& x, A& y) { if(!model_straight) tie(x,y) = make_pair(x*ocos + y*osin, y*ocos - x*osin); }
  template<class A>
  void apply_orientation_yz(A& x, A& y) { if(!model_straight_yz) tie(x,y) = make_pair(x*ocos_yz + y*osin_yz, y*ocos_yz - x*osin_yz); }
  template<class A>
  void apply_ball(A& x, A& y) { tie(x,y) = make_pair(x*cos_ball + y*sin_ball, y*cos_ball - x*sin_ball); }
  
  void configure();
  
  extern bool autoband;
  extern bool autobandhistory;
  extern bool dospiral;
  extern ld lvspeed;
  extern int bandsegment;
  extern int bandhalf;  
  extern ld extra_line_steps;
  
  void create(cell *start, cell *target);
  void create_playerpath();
  void create_recenter_to_view(bool precise);
  void clear();
  void model_menu();
  void history_menu();
  string get_model_name(eModel);
  void apply();
  void movetophase();
  void renderAutoband();
  
  extern vector<shmup::monster*> v;
  extern double phase;
  void applyIB();

  void progress_screen();
  void progress(string str);
  bool model_has_orientation();

  extern string formula;
  extern eModel basic_model;
  }
  
namespace polygonal {
  static const int MSI = 120;
  extern int SI;
  extern ld STAR;
  extern int deg;
  extern ld coefr[MSI], coefi[MSI];
  extern int maxcoef, coefid;
  void solve();
  pair<ld, ld> compute(ld x, ld y);
  }

// passable flags

#define SAGEMELT .1
#define TEMPLE_EACH (among(geometry, gHoroRec, gHoroHex, gKiteDart3) ? 3 : (sol && binarytiling) ? 6 : (WDIM == 3 && binarytiling) ? 2 : geometry == gSpace435 ? 4 : (WDIM == 3 && hyperbolic) ? 3 : 6)
#define PT(x, y) ((tactic::on || quotient == 2 || daily::on) ? (y) : inv::on ? min(2*(y),x) : (x))
#define ROCKSNAKELENGTH 50
#define WORMLENGTH 15
#define PUREHARDCORE_LEVEL 10
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
  
typedef flagtype modecode_t;

#define GUNRANGE 3

// 0 = basic treasure, 1 = other item, 2 = power orb, 3 = not an item
#define IC_TREASURE 0
#define IC_OTHER 1
#define IC_ORB 2
#define IC_NAI 3

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

struct stalemate1 {
  eMonster who;
  cell *moveto;
  cell *killed;
  cell *pushto;
  cell *comefrom;
  cell *swordlast[2], *swordtransit[2], *swordnext[2];
  bool isKilled(cell *c);
  stalemate1(eMonster w, cell *mt, cell *ki, cell *pt, cell *cf) : who(w), moveto(mt), killed(ki), pushto(pt), comefrom(cf) {}
  };

namespace stalemate {
  extern vector<stalemate1> moves;
  extern bool  nextturn;
  
  bool isKilled(cell *c);

  bool isMoveto(cell *c);
  bool isKilledDirectlyAt(cell *c);
  bool isPushto(cell *c);
  };

namespace tortoise {
  extern int seekbits;
  int getRandomBits();
  }

namespace sword {

  struct sworddir {
    int angle;
    transmatrix T;
    };
  
  extern array<sworddir, MAXPLAYER> dir;

  cell *pos(cell *c, const sworddir& sd, bool rev);
  cell *pos(int id);
  bool at(cell *where, bool noplayer = false);
  sworddir shift(cell *c1, cell *c2, sworddir);
  }

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

namespace shot {
  #if CAP_SHOT
  extern int shotx, shoty, shotformat;
  extern bool make_svg;
  extern ld gamma, fade;
  extern string caption;
  extern bool transparent;
  void menu();
  void default_screenshot_content();
  void take(string fname, const function<void()>& what = default_screenshot_content);
  #endif
  }

#if CAP_SVG
namespace svg {
  void circle(int x, int y, int size, color_t col, color_t fillcolor, double linewidth);
  void polygon(int *polyx, int *polyy, int polyi, color_t col, color_t outline, double linewidth);
  void text(int x, int y, int size, const string& str, bool frame, color_t col, int align);
  extern bool in;
  extern string link;
  #if CAP_SHOT
  void render(const string& fname, const function<void()>& what = shot::default_screenshot_content);
  #endif
  }
#else
namespace svg {
  static const always_false in;
  }
#endif

namespace halloween {
  void getTreat(cell *where);
  }

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

namespace mapeditor {
  bool drawUserShape(const transmatrix& V, eShapegroup group, int id, color_t color, cell *c, PPR prio = PPR::DEFAULT);
  }

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

inline string pick123() { return cts('1' + rand() % 3); }
inline string pick12() { return cts('1' + rand() % 2); }

extern int detaillevel;
extern bool quitmainloop;

enum eGravity { gsNormal, gsLevitation, gsAnti };
extern eGravity gravity_state, last_gravity_state;

#define IFM(x) (mousing?"":x)

namespace quotientspace {
  void build();
  void clear();
  extern vector<int> connections;
  }

void killFriendlyIvy();

#if CAP_SHAPES
void pushdown(cell *c, int& q, const transmatrix &V, double down, bool rezoom, bool repriority);
#endif

extern bool viewdists;

void preventbarriers(cell *c);

bool passable_for(eMonster m, cell *w, cell *from, flagtype extra);

void beastcrash(cell *c, cell *beast);

int angledist(int t, int d1, int d2);
int angledist(cell *c, int d1, int d2);

void setcameraangle(bool b);

#define MODELCOUNT ((int) mdGUARD)

void drawShape(pair<ld,ld>* coords, int qty, color_t color);

#define pmodel (vid.vpmodel)
string current_proj_name();

inline bool mdAzimuthalEqui() { return among(pmodel, mdEquidistant, mdEquiarea, mdEquivolume); }

inline bool mdBandAny() { return among(pmodel, mdBand, mdBandEquidistant, mdBandEquiarea, mdSinusoidal); }

color_t darkena(color_t c, int lev, int a);

#define SHSIZE 16

namespace anims { void animate_parameter(ld &x, string f, const reaction_t& r); }

extern bool timerghost;
extern bool autocheat;
extern int cheater;

namespace arg {
#if CAP_COMMANDLINE
  
  void lshift();
  void unshift();

  void shift();

  const string& args();
  const char* argcs();
  int argi();
  ld argf();
  bool argis(const string& s);
  bool nomore();
  unsigned arghex();

  inline void shift_arg_formula(ld& x, const reaction_t& r = reaction_t()) { shift(); x = argf(); 
    #if CAP_ANIMATIONS
    anims::animate_parameter(x, args(), r); 
    #endif
    }
  
  void init(int _argc, char **_argv);
  
  void launch_dialog(const reaction_t& r = reaction_t());
  
  extern int curphase;
  
  void phaseerror(int x);
  
  // returned values: 0 = ok, 1 = not recognized, 2 = shift phase
  int readCommon();
  int readLocal();

// an useful macro
#define PHASE(x) { if(arg::curphase > x) arg::phaseerror(x); else if(arg::curphase < x) return 2; }
#define PHASEFROM(x) { if(arg::curphase < x) return 2; }

  inline void cheat() { autocheat = true; cheater++; timerghost = false; }

#define TOGGLE(x, param, act) \
else if(args()[0] == '-' && args()[1] == x && !args()[2]) { PHASEFROM(2); showstartmenu = false; act; } \
else if(args()[0] == '-' && args()[1] == x && args()[2] == '1') { PHASEFROM(2); showstartmenu = false; if(!param) act; } \
else if(args()[0] == '-' && args()[1] == x && args()[2] == '0') { PHASEFROM(2); showstartmenu = false; if(param) act; }

  
  void read(int phase);

  eLand readland(const string& ss);
  eItem readItem(const string& ss);
  eMonster readMonster(const string& ss);
    
#endif
  }

#if CAP_TOUR
namespace tour {
  extern bool on;
  extern string tourhelp;
  extern string slidecommand;
  extern int currentslide;
  
  enum presmode { 
    pmStartAll = 0,
    pmStart = 1, pmFrame = 2, pmStop = 3, pmKey = 4, pmRestart = 5,
    pmAfterFrame = 6,
    pmGeometry = 11, pmGeometryReset = 13, pmGeometryStart = 15
    };

  void setCanvas(presmode mode, char canv);

  void presentation(presmode mode);
  void checkGoodLand(eLand l);
  int getid();
  
  extern function<eLand(eLand)> getNext;
  extern function<bool(eLand)> quickfind;
  extern function<bool(eLand)> showland;

  void start();

  struct slide { 
    const char *name; int unused_id; int flags; const char *help; 
    function<void(presmode mode)> action;
    } ;
  
  extern slide *slides;
  extern slide default_slides[];

  static const int LEGAL_NONE=0;
  static const int LEGAL_UNLIMITED=1;
  static const int LEGAL_HYPERBOLIC=2;
  static const int LEGAL_ANY=3;
  static const int LEGAL_NONEUC=4;
  static const int QUICKSKIP=8;
  static const int FINALSLIDE=16;
  static const int QUICKGEO=32;
  static const int SIDESCREEN = 64;
  static const int USE_SLIDE_NAME = 128;
  
  extern slide slideHypersian;
  extern slide slideExpansion;

  namespace ss {
    void showMenu();
    void list(slide*);
    }

  extern hookset<void(int)> *hooks_slide;
  };
#else
namespace tour {
  static const always_false on;
  }
#endif

namespace sm {
  static const int NORMAL = 1;
  static const int MISSION = 2;
  static const int HELP = 4;
  static const int MAP = 8;
  static const int DRAW = 16;
  static const int NUMBER = 32;
  static const int SHMUPCONFIG = 64;
  static const int OVERVIEW = 128;
  static const int SIDE = 256;
  static const int DOTOUR = 512;
  static const int CENTER = 1024;
  static const int ZOOMABLE = 4096;
  static const int TORUSCONFIG = 8192;
  static const int MAYDARK = 16384;
  static const int DIALOG_STRICT_X = 32768; // do not interpret dialog clicks outside of the X region
  static const int EXPANSION = (1<<16);
  static const int HEXEDIT = (1<<17);
  };

namespace linepatterns {

  enum ePattern {
    patPalacelike,
    patPalace,
    patZebraTriangles,
    patZebraLines,
    patTriTree,
    patTriRings,
    patHepta,
    patRhomb,
    patTree,
    patAltTree,
    patVine,
    patPower,
    patNormal,
    patTrihepta,
    patBigTriangles,
    patBigRings,
    patHorocycles,
    patTriOther,
    patDual,
    patMeridians,
    patParallels,
    patCircles,
    patRadii
    };
  
  void clearAll();
  void setColor(ePattern id, color_t col);
  void drawAll();
  void showMenu();
  void switchAlpha(ePattern id, color_t col);

  struct linepattern {
    int id;
    const char *lpname;
    color_t color;
    ld multiplier;
    };

  extern vector<linepattern> patterns;
  extern ld width;
  };

static const int DISTANCE_UNKNOWN = 127;

#define SETMOUSEKEY 5000
extern char mousekey;
extern char newmousekey;
void displaymm(char c, int x, int y, int rad, int size, const string& title, int align);

template<class T, class... U> T pick(T x, U... u) { std::initializer_list<T> i = {x,u...}; return *(i.begin() + hrand(1+sizeof...(u))); }

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

struct msginfo {
  int stamp;
  time_t rtstamp;
  int gtstamp;
  int turnstamp;
  char flashout;
  char spamtype;
  int quantity;
  string msg;
  };

int watercolor(int phase);
bool doHighlight();
void buildHelpText();
void buildCredits();
void setAppropriateOverview();
bool quitsaves();

extern const char* COLORBAR;

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

namespace mirror {
  cellwalker reflect(const cellwalker& cw);
  }

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

#if CAP_FIELD
namespace windmap {
  void create();

  static const int NOWINDBELOW = 8;
  static const int NOWINDFROM = 120;

  int getId(cell *c);
  int at(cell *c);
  }
#endif

int getgametime();
string getgametime_s(int timespent = getgametime());
extern int stampbase;

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

struct texture_triangle {
  array<hyperpoint, 3> v;
  array<hyperpoint, 3> tv;
  texture_triangle(array<hyperpoint, 3> _v, array<hyperpoint, 3> _tv) : v(_v), tv(_tv) {}
  };

struct basic_textureinfo {
  int texture_id;
  vector<glvertex> tvertices; 
  };
  
struct textureinfo : basic_textureinfo {
  transmatrix M;
  vector<texture_triangle> triangles;
  vector<glvertex> vertices;
  cell *c;
  vector<transmatrix> matrices;
  
  // these are required to adjust to geometry changes
  int current_type, symmetries;
  };

struct drawqueueitem {
  PPR prio;
  color_t color;
  int subprio;
  virtual void draw() = 0;
  virtual void draw_back() {}
  virtual void draw_pre() {}
  virtual ~drawqueueitem() {}
  void draw_darker();
  virtual color_t outline_group() = 0;
  };

struct dqi_poly : drawqueueitem {
  ld band_shift;
  transmatrix V;
  const vector<glvertex> *tab;
  int offset, cnt, offset_texture;
  color_t outline;
  double linewidth;
  int flags;
  basic_textureinfo *tinf;
  hyperpoint intester;
  void draw();
  void gldraw();
  void draw_back();
  virtual color_t outline_group() { return outline; }
  };

struct dqi_line : drawqueueitem {
  ld band_shift;
  hyperpoint H1, H2;
  int prf;
  double width;
  void draw();
  void draw_back();
  virtual color_t outline_group() { return color; }
  };
      
struct dqi_string : drawqueueitem {
  string str;
  int x, y, shift, size, frame;
  int align;
  void draw();
  virtual color_t outline_group() { return 1; }
  };

struct dqi_circle : drawqueueitem {
  int x, y, size, fillcolor;
  double linewidth;
  void draw();
  virtual color_t outline_group() { return 2; }
  };

struct dqi_action : drawqueueitem {
  reaction_t action;
  dqi_action(const reaction_t& a) : action(a) {}
  void draw() { action(); }
  virtual color_t outline_group() { return 2; }
  };

struct sky_item {
  cell *c;
  transmatrix T;
  color_t color;
  sky_item(cell *_c, const transmatrix _T, color_t _color) : c(_c), T(_T), color(_color) {}
  };

extern struct dqi_sky *sky;

struct dqi_sky : drawqueueitem {
  vector<sky_item> sky;
  void draw();
  virtual color_t outline_group() { return 3; }
  // singleton
  dqi_sky() { hr::sky = this; }
  ~dqi_sky() { hr::sky = NULL; }
  };
  
extern int emeraldtable[100][7];

// extern cell *cwpeek(cellwalker cw, int dir);

const eLand NOWALLSEP = laNone;
const eLand NOWALLSEP_USED = laWhirlpool;

#define HAUNTED_RADIUS getDistLimit()
#define UNKNOWN 65535

namespace tactic {
  extern bool on;
  extern bool trailer;
  }

namespace yendor {
  extern bool on;
  extern bool generating;
  extern eLand nexttostart;
  
  #define YF_DEAD 1
  #define YF_WALLS 2
  #define YF_END 4
  #define YF_DEAD5 8

  #define YF_NEAR_IVY   16
  #define YF_NEAR_ELEM  32
  #define YF_NEAR_OVER  64
  #define YF_NEAR_RED   128
  #define YF_REPEAT     512
  #define YF_NEAR_TENT  1024

  #define YF_START_AL   2048
  #define YF_START_CR   4096
  #define YF_CHAOS      8192
  #define YF_RECALL     16384
  #define YF_NEAR_FJORD 32768
  
  #define YF_START_ANY  (YF_START_AL|YF_START_CR)  

  struct yendorlevel {
    eLand l;
    int flags;
    };
  
  yendorlevel& clev();
  }

namespace clearing {

  struct clearingdata {
    cell *root;
    int dist;
    };
  
  extern bool buggyplant;
  
  extern std::map<heptagon*, clearingdata> bpdata;
  }

namespace peace {
  extern bool on;
  }

namespace princess {
#define EPX 39
#define EPY 21

#define OUT_OF_PRISON 200
#define OUT_OF_PALACE 250
#define PRADIUS0 (141)
#define PRADIUS1 (150)

  extern bool generating;
  extern bool gotoPrincess;
  extern bool forceMouse;
  extern bool challenge;
  extern bool squeaked;
  extern bool saved;
  extern bool nodungeon;
  extern int reviveAt;
  extern bool forceVizier;

  struct info {
    int id;         // id of this info
    cell *prison;   // where was the Princess locked
    heptagon *alt;  // alt of the prison
    int bestdist;   // best dist achieved
    int bestnear;   // best dist achieved, by the player
    int value;      // number of Rugs at 120
    cell *princess; // where is the Princess currently
    };
    
  int newInfo(cell *c);
  }

#define GRAIL_FOUND 0x4000
#define GRAIL_RADIUS_MASK 0x3FFF

extern vector<cell*> dcal;

struct hrmap {
  virtual heptagon *getOrigin() { return NULL; }
  virtual cell *gamestart() { return getOrigin()->c7; }
  virtual ~hrmap() { };
  virtual vector<cell*>& allcells() { return dcal; }
  virtual void verify() { }
  virtual void link_alt(const cellwalker& hs) { }
  virtual void generateAlts(heptagon *h, int levs = IRREGULAR ? 1 : S3-3, bool link_cdata = true);
  heptagon *may_create_step(heptagon *h, int direction) {
    if(h->move(direction)) return h->move(direction);
    return create_step(h, direction);
    }
  virtual heptagon *create_step(heptagon *h, int direction) {
    printf("create_step called unexpectedly\n"); exit(1);
    return NULL;
    }
  virtual transmatrix relative_matrix(heptagon *h2, heptagon *h1) {
    printf("relative_matrix called unexpectedly\n"); 
    return Id;
    }
  virtual transmatrix relative_matrix(cell *c2, cell *c1, const hyperpoint& point_hint) {
    return relative_matrix(c2->master, c1->master);
    }
  virtual void draw() {
    printf("undrawable\n");
    }
  virtual vector<hyperpoint> get_vertices(cell*);
  };

// hrmaps which are based on regular non-Euclidean 2D tilings, possibly quotient 
struct hrmap_standard : hrmap {
  void draw() override;
  transmatrix relative_matrix(cell *c2, cell *c1, const hyperpoint& point_hint) override;
  heptagon *create_step(heptagon *h, int direction) override;
  };

void clearfrom(heptagon*);
void verifycells(heptagon*);

struct hrmap_hyperbolic : hrmap_standard {
  heptagon *origin;
  eVariation mvar;
  hrmap_hyperbolic();
  hrmap_hyperbolic(heptagon *origin);
  heptagon *getOrigin() override { return origin; }
  ~hrmap_hyperbolic() {
    // verifycells(origin);
    // printf("Deleting hyperbolic map: %p\n", this);
    dynamicval<eVariation> ph(variation, mvar);
    clearfrom(origin);
    }
  void verify() override { verifycells(origin); }
  };

// list all cells in distance at most maxdist, or until when maxcount cells are reached

struct manual_celllister {
  vector<cell*> lst;
  vector<int> tmps;

  bool listed(cell *c) {
    return c->listindex >= 0 && c->listindex < isize(lst) && lst[c->listindex] == c;
    }
  
  bool add(cell *c) {
    if(listed(c)) return false;
    tmps.push_back(c->listindex);
    c->listindex = isize(lst);
    lst.push_back(c);
    return true;
    }

  ~manual_celllister() {     
    for(int i=0; i<isize(lst); i++) lst[i]->listindex = tmps[i];
    }  
  };
  

struct celllister : manual_celllister {
  vector<int> dists;
  
  void add_at(cell *c, int d) {
    if(add(c)) dists.push_back(d);
    }
  
  celllister(cell *orig, int maxdist, int maxcount, cell *breakon) {
    add_at(orig, 0);
    cell *last = orig;
    for(int i=0; i<isize(lst); i++) {
      cell *c = lst[i];
      if(maxdist) forCellCM(c2, c) {
        add_at(c2, dists[i]+1);
        if(c2 == breakon) return;
        }
      if(c == last) {
        if(isize(lst) >= maxcount || dists[i]+1 == maxdist) break;
        last = lst[isize(lst)-1];
        }
      }
    }

  int getdist(cell *c) { return dists[c->listindex]; }
  };

hrmap *newAltMap(heptagon *o);

#if CAP_FIELD
#define currfp fieldpattern::getcurrfp()
namespace fieldpattern {
  struct fpattern& getcurrfp();
  }

int currfp_gmul(int a, int b);
int currfp_inverses(int i);
int currfp_distwall(int i);

int currfp_n();
int currfp_get_P();
int currfp_get_R();
int currfp_get_X();
#endif

void runGeometryExperiments();

// z to close to this limit => do not draw

#define BEHIND_LIMIT 1e-6

namespace lv {
  static const flagtype appears_in_geom_exp = 1;
  static const flagtype display_error_message = 2;
  static const flagtype appears_in_full = 4;
  static const flagtype appears_in_ptm = 8;
  static const flagtype display_in_help = 16;
  static const flagtype one_and_half = 32;
  };

struct land_validity_t {
  int quality_level; // 0 (dont show), 1 (1/2), 2 (ok), 3(1!)
  flagtype flags;
  string msg;
  };

extern const hyperpoint C02, C03;

#define C0 (DIM == 2 ? C02 : C03)

namespace fieldpattern {
  pair<int, bool> fieldval(cell *c);
  }

int emeraldval(cell *c);

int inpair(cell *c, int colorpair);
int snake_pair(cell *c);

extern colortable nestcolors;

unsigned char& part(color_t& col, int i);

int pattern_threecolor(cell *c);
int fiftyval200(cell *c);

bool isWall3(cell *c, color_t& wcol);
extern transmatrix actual_view_transform, radar_transform;
ld wall_radar(cell *c, transmatrix T);

extern string bitruncnames[5];
extern bool need_mouseh;

void clear_euland(eLand first);

extern eMonster passive_switch;

bool cannotPickupItem(cell *c, bool telekinesis);
bool canPickupItemWithMagnetism(cell *c, cell *from);
void pickupMovedItems(cell *c);
eMonster genRuinMonster(cell *c);

template<class T> void hrandom_shuffle(T* x, int n) {
  for(int k=1; k<n; k++) swap(x[k], x[hrand(k+1)]);
  }

void resetModes(char leave = 'c');
void activateSafety(eLand l);
void showMainMenu();
extern bool nomenukey;
void resetConfig();
void welcomeMessage();
void jumpTo(cell *dest, eItem byWhat, int bonuskill = 0, eMonster dashmon = moNone);
extern bool canmove;

void activateSafety(eLand l);
extern bool childbug;

void fullcenter();
void mainloop();
void clearAnimations();

void destroycellcontents(cell *c);
extern heptagon *last_cleared;

template<class T, class U> void eliminate_if(vector<T>& data, U pred) {
  for(int i=0; i<isize(data); i++)
    if(pred(data[i]))
      data[i] = data.back(), data.pop_back(), i--;
  }

bool is_cell_removed(cell *c);
void set_if_removed(cell*& c, cell *val);

#if CAP_ORIENTATION
transmatrix getOrientation();
#endif

namespace elec { extern int lightningfast; }

#define DF_INIT              1 // always display these
#define DF_MSG               2 // always display these
#define DF_WARN              4 // always display these
#define DF_ERROR             8 // always display these
#define DF_STEAM            16
#define DF_GRAPH            32
#define DF_TURN             64
#define DF_FIELD           128
#define DF_GEOM            256
#define DF_MEMORY          512
#define DF_TIME           1024 // a flag to display timestamps
#define DF_GP             2048
#define DF_POLY           4096
#define DF_LOG            8192
#define DF_KEYS "imwesxufgbtopl"

#if ISANDROID
#define DEBB(r,x)
#define DEBB0(r,x)
#define DEBBI(r,x)
#else
#define DEBB(r,x) { if(debugflags & (r)) { println_log x; } }
#define DEBB0(r,x) { if(debugflags & (r)) { print_log x; } }
#define DEBBI(r,x) { if(debugflags & (r)) { println_log x; } } indenter_finish _debbi(debugflags & (r));
#endif

template<class T> array<T, 4> make_array(T a, T b, T c, T d) { array<T,4> x; x[0] = a; x[1] = b; x[2] = c; x[3] = d; return x; }
template<class T> array<T, 3> make_array(T a, T b, T c) { array<T,3> x; x[0] = a; x[1] = b; x[2] = c; return x; }
template<class T> array<T, 2> make_array(T a, T b) { array<T,2> x; x[0] = a; x[1] = b; return x; }

extern const hyperpoint Hypc;

struct supersaver {
  string name;
  virtual string save() = 0;
  virtual void load(const string& s) = 0;
  virtual bool dosave() = 0;
  virtual void reset() = 0;
  virtual ~supersaver() {};
  };

typedef vector<shared_ptr<supersaver>> saverlist;

extern saverlist savers;

#if CAP_CONFIG

template<class T> struct dsaver : supersaver {
  T& val;
  T dft;
  bool dosave() { return val != dft; }
  void reset() { val = dft; }
  dsaver(T& val) : val(val) { }
  };

template<class T> struct saver : dsaver<T> {};

template<class T, class U, class V> void addsaver(T& i, U name, V dft) {
  auto s = make_shared<saver<T>> (i);
  s->dft = dft;
  s->name = name;
  savers.push_back(s);
  }

template<class T> void addsaver(T& i, string name) {
  addsaver(i, name, i);
  }

template<class T> struct saverenum : supersaver {
  T& val;
  T dft;
  bool dosave() { return val != dft; }
  void reset() { val = dft; }
  saverenum<T>(T& v) : val(v) { }
  string save() { return its(int(val)); }
  void load(const string& s) { val = (T) atoi(s.c_str()); }
  };

template<class T, class U> void addsaverenum(T& i, U name, T dft) {
  auto s = make_shared<saverenum<T>> (i);
  s->dft = dft;
  s->name = name;
  savers.push_back(s);
  }

template<class T, class U> void addsaverenum(T& i, U name) {
  addsaverenum(i, name, i);
  }

template<> struct saver<int> : dsaver<int> {
  saver<int>(int& val) : dsaver<int>(val) { }
  string save() { return its(val); }
  void load(const string& s) { val = atoi(s.c_str()); }
  };

template<> struct saver<char> : dsaver<char> {
  saver<char>(char& val) : dsaver<char>(val) { }
  string save() { return its(val); }
  void load(const string& s) { val = atoi(s.c_str()); }
  };

template<> struct saver<bool> : dsaver<bool> {
  saver<bool>(bool& val) : dsaver<bool>(val) { }
  string save() { return val ? "yes" : "no"; }
  void load(const string& s) { val = isize(s) && s[0] == 'y'; }
  };

template<> struct saver<unsigned> : dsaver<unsigned> {
  saver<unsigned>(unsigned& val) : dsaver<unsigned>(val) { }
  string save() { return itsh(val); }
  void load(const string& s) { val = (unsigned) strtoll(s.c_str(), NULL, 16); }
  };

template<> struct saver<string> : dsaver<string> {
  saver<string>(string& val) : dsaver<string>(val) { }
  string save() { return val; }
  void load(const string& s) { val = s; }
  };

string fts(ld val, int prec);
string itsh(int x);

template<> struct saver<ld> : dsaver<ld> {
  saver<ld>(ld& val) : dsaver<ld>(val) { }
  string save() { return fts(val, 10); }
  void load(const string& s) { 
    if(s == "0.0000000000e+000") ; // ignore!
    else val = atof(s.c_str()); 
    }
  };

#endif

#if CAP_SHAPES
struct floorshape;

struct hpcshape;

struct qfloorinfo {
  transmatrix spin;
  const hpcshape *shape;
  floorshape *fshape;
  textureinfo *tinf;
  int usershape;
  };

extern qfloorinfo qfi;

struct hpcshape {
  int s, e;
  PPR prio;
  int flags;
  hyperpoint intester;
  basic_textureinfo *tinf;
  int texture_offset;
  int shs, she;
  void clear() { s = e = shs = she = texture_offset = 0; prio = PPR::ZERO; tinf = NULL; flags = 0; }
  };

extern vector<hpcshape> shPlainWall3D, shWireframe3D, shWall3D, shMiniWall3D;
#endif

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

namespace torusconfig {
  extern int sdx, sdy;
  
  enum eTorusMode : char { 
    tmSingleHex, 
    tmSingle, 
    tmSlantedHex, 
    tmStraight, 
    tmStraightHex,
    tmKlein,
    tmKleinHex,
    tmCylinder,
    tmCylinderHex,
    tmMobius,
    tmMobiusHex,
    };
  
  extern eTorusMode torus_mode;
  extern void activate();

  struct torusmode_info {
    string name;
    flagtype flags;
    };
  
  extern vector<torusmode_info> tmodes;

  enum : flagtype {
    TF_SINGLE = 1,
    TF_SIMPLE = 2,
    TF_WEIRD  = 4,
    TF_HEX    = 16,
    TF_SQUARE = 32,
    TF_CYL    = 64,
    TF_KLEIN = 256,
    };

  flagtype tmflags();
  }

struct plainshape;
void clear_plainshape(plainshape& gsh);

namespace gp {
  void clear_plainshapes();
  plainshape& get_plainshape();
  }

extern bool just_gmatrix;

bool haveLeaderboard(int id);
int get_currentscore(int id);
void set_priority_board(int id);
int get_sync_status();
bool score_loaded(int id);
int score_default(int id);
void handle_event(SDL_Event& ev);

void generate_floorshapes();

#define SIDE_SLEV 0
#define SIDE_WTS3 3
#define SIDE_WALL 4
#define SIDE_LAKE 5
#define SIDE_LTOB 6
#define SIDE_BTOI 7
#define SIDE_SKY  8
#define SIDE_HIGH 9
#define SIDE_HIGH2 10
#define SIDEPARS  11

#if CAP_SHAPES
struct floorshape {
  bool is_plain;
  int shapeid;
  int id;
  int pstrength; // pattern strength in 3D
  int fstrength; // frame strength in 3D
  PPR prio;
  vector<hpcshape> b, shadow, side[SIDEPARS], gpside[SIDEPARS][MAX_EDGE], levels[SIDEPARS], cone[2];
  floorshape() { prio = PPR::FLOOR; pstrength = fstrength = 10; }
  };

struct plain_floorshape : floorshape {
  ld rad0, rad1;
  void configure(ld r0, ld r1) { rad0 = r0; rad1 = r1; }
  };

// noftype: 0 (shapeid2 is heptagonal or just use shapeid1), 1 (shapeid2 is pure heptagonal), 2 (shapeid2 is Euclidean), 3 (shapeid2 is hexagonal)
struct escher_floorshape : floorshape {
  int shapeid0, shapeid1, noftype, shapeid2;
  ld scale;
  };
#endif

struct usershapelayer {
  vector<hyperpoint> list;
  bool sym;
  int rots;
  color_t color;
  hyperpoint shift, spin;
  ld zlevel;
  int texture_offset;
  PPR prio;
  };

static const int USERLAYERS = 32;

struct usershape { usershapelayer d[USERLAYERS]; };

extern array<map<int, usershape*>, mapeditor::USERSHAPEGROUPS> usershapes;
void initShape(int sg, int id);

extern int usershape_changes;

#define BADMODEL 0

static const int WINGS = (BADMODEL ? 1 : 4);

typedef array<hpcshape, WINGS+1> hpcshape_animated;

extern vector<ld> equal_weights;

struct geometry_information {

  /* basic geometry parameters */

  // tessf: distance from heptagon center to another heptagon center
  // hexf: distance from heptagon center to small heptagon vertex
  // hcrossf: distance from heptagon center to big heptagon vertex
  // crossf: distance from heptagon center to adjacent cell center (either hcrossf or tessf)
  // hexhexdist: distance between adjacent hexagon vertices
  // hexvdist: distance between hexagon vertex and hexagon center
  // hepvdist: distance between heptagon vertex and hexagon center (either hcrossf or something else)
  // rhexf: distance from heptagon center to heptagon vertex (either hexf or hcrossf)
  ld tessf, crossf, hexf, hcrossf, hexhexdist, hexvdist, hepvdist, rhexf;

  transmatrix heptmove[MAX_EDGE], hexmove[MAX_EDGE];
  transmatrix invheptmove[MAX_EDGE], invhexmove[MAX_EDGE];

  int base_distlimit;

  /* shape parameters */
  ld sword_size;
  ld scalefactor, orbsize, floorrad0, floorrad1, zhexf;
  ld corner_bonus;
  ld hexshift;
  ld asteroid_size[8];
  ld wormscale;
  ld tentacle_length;
  
  /* 3D parameters */
  ld INFDEEP, BOTTOM, HELLSPIKE, LAKE, WALL, FLOOR, STUFF,
    SLEV[4], FLATEYE,
    LEG0, LEG1, LEG, LEG3, GROIN, GROIN1, GHOST,
    BODY, BODY1, BODY2, BODY3,
    NECK1, NECK, NECK3, HEAD, HEAD1, HEAD2, HEAD3,
    ALEG0, ALEG, ABODY, AHEAD, BIRD, LOWSKY, SKY, HIGH, HIGH2;
  ld human_height, slev;

  ld eyelevel_familiar, eyelevel_human, eyelevel_dog;

#if CAP_SHAPES
hpcshape 
  shSemiFloorSide[SIDEPARS],
  shBFloor[2],
  shWave[8][2],  
  shCircleFloor,
  shBarrel,
  shWall[2], shMineMark[2], shBigMineMark[2], shFan,
  shZebra[5],
  shSwitchDisk,
  shTower[11],
  shEmeraldFloor[6],
  shSemiFeatherFloor[2], 
  shSemiFloor[2], shSemiBFloor[2], shSemiFloorShadow,
  shMercuryBridge[2],
  shTriheptaSpecial[14], 
  shCross, shGiantStar[2], shLake, shMirror,
  shHalfFloor[6], shHalfMirror[3],
  shGem[2], shStar, shDisk, shDiskT, shDiskS, shDiskM, shDiskSq, shRing,   
  shTinyBird, shTinyShark,
  shEgg,
  shSpikedRing, shTargetRing, shSawRing, shGearRing, shPeaceRing, shHeptaRing,
  shSpearRing, shLoveRing,
  shDaisy, shTriangle, shNecro, shStatue, shKey, shWindArrow,
  shGun,
  shFigurine, shTreat,
  shElementalShard,
  // shBranch, 
  shIBranch, shTentacle, shTentacleX, shILeaf[2], 
  shMovestar,
  shWolf, shYeti, shDemon, shGDemon, shEagle, shGargoyleWings, shGargoyleBody,
  shFoxTail1, shFoxTail2,
  shDogBody, shDogHead, shDogFrontLeg, shDogRearLeg, shDogFrontPaw, shDogRearPaw,
  shDogTorso,
  shHawk,
  shCatBody, shCatLegs, shCatHead, shFamiliarHead, shFamiliarEye,
  shWolf1, shWolf2, shWolf3,
  shRatEye1, shRatEye2, shRatEye3,
  shDogStripes,
  shPBody, shPSword, shPKnife,
  shFerocityM, shFerocityF, 
  shHumanFoot, shHumanLeg, shHumanGroin, shHumanNeck, shSkeletalFoot, shYetiFoot,
  shMagicSword, shMagicShovel, shSeaTentacle, shKrakenHead, shKrakenEye, shKrakenEye2,
  shArrow,
  shPHead, shPFace, shGolemhead, shHood, shArmor, 
  shAztecHead, shAztecCap,
  shSabre, shTurban1, shTurban2, shVikingHelmet, shRaiderHelmet, shRaiderArmor, shRaiderBody, shRaiderShirt,
  shWestHat1, shWestHat2, shGunInHand,
  shKnightArmor, shKnightCloak, shWightCloak,
  shGhost, shEyes, shSlime, shJelly, shJoint, shWormHead, shTentHead, shShark, shWormSegment, shSmallWormSegment, shWormTail, shSmallWormTail,
  shSlimeEyes, shDragonEyes, shWormEyes, shGhostEyes,
  shMiniGhost, shMiniEyes,
  shHedgehogBlade, shHedgehogBladePlayer,
  shWolfBody, shWolfHead, shWolfLegs, shWolfEyes,
  shWolfFrontLeg, shWolfRearLeg, shWolfFrontPaw, shWolfRearPaw,
  shFemaleBody, shFemaleHair, shFemaleDress, shWitchDress,
  shWitchHair, shBeautyHair, shFlowerHair, shFlowerHand, shSuspenders, shTrophy,
  shBugBody, shBugArmor, shBugLeg, shBugAntenna,
  shPickAxe, shPike, shFlailBall, shFlailTrunk, shFlailChain, shHammerHead,
  shBook, shBookCover, shGrail,
  shBoatOuter, shBoatInner, shCompass1, shCompass2, shCompass3,
  shKnife, shTongue, shFlailMissile, shTrapArrow,
  shPirateHook, shPirateHood, shEyepatch, shPirateX,
  // shScratch, 
  shHeptaMarker, shSnowball, shSun, shNightStar, shEuclideanSky,
  shSkeletonBody, shSkull, shSkullEyes, shFatBody, shWaterElemental,
  shPalaceGate, shFishTail,
  shMouse, shMouseLegs, shMouseEyes,
  shPrincessDress, shPrinceDress,
  shWizardCape1, shWizardCape2,
  shBigCarpet1, shBigCarpet2, shBigCarpet3,
  shGoatHead, shRose, shRoseItem, shThorns,
  shRatHead, shRatTail, shRatEyes, shRatCape1, shRatCape2,
  shWizardHat1, shWizardHat2,
  shTortoise[13][6],
  shDragonLegs, shDragonTail, shDragonHead, shDragonSegment, shDragonNostril, 
  shDragonWings, 
  shSolidBranch, shWeakBranch, shBead0, shBead1,
  shBatWings, shBatBody, shBatMouth, shBatFang, shBatEye,
  shParticle[16], shAsteroid[8],
  shReptile[5][4],
  shReptileBody, shReptileHead, shReptileFrontFoot, shReptileRearFoot,
  shReptileFrontLeg, shReptileRearLeg, shReptileTail, shReptileEye,

  shTrylobite, shTrylobiteHead, shTrylobiteBody,
  shTrylobiteFrontLeg, shTrylobiteRearLeg, shTrylobiteFrontClaw, shTrylobiteRearClaw,
  
  shBullBody, shBullHead, shBullHorn, shBullRearHoof, shBullFrontHoof,
  
  shButterflyBody, shButterflyWing, shGadflyBody, shGadflyWing, shGadflyEye,

  shTerraArmor1, shTerraArmor2, shTerraArmor3, shTerraHead, shTerraFace, 
  shJiangShi, shJiangShiDress, shJiangShiCap1, shJiangShiCap2,
  
  shAsymmetric,
  
  shPBodyOnly, shPBodyArm, shPBodyHand, shPHeadOnly,
  
  shDodeca;
  
  hpcshape_animated 
    shAnimatedEagle, shAnimatedTinyEagle, shAnimatedGadfly, shAnimatedHawk, shAnimatedButterfly, 
    shAnimatedGargoyle, shAnimatedGargoyle2, shAnimatedBat, shAnimatedBat2;  

  vector<hpcshape> shPlainWall3D, shWireframe3D, shWall3D, shMiniWall3D;

  vector<plain_floorshape*> all_plain_floorshapes;
  vector<escher_floorshape*> all_escher_floorshapes;

  plain_floorshape
    shFloor, 
    shMFloor, shMFloor2, shMFloor3, shMFloor4, shFullFloor,
    shBigTriangle, shTriheptaFloor, shBigHepta;
  
  escher_floorshape    
    shStarFloor, shCloudFloor, shCrossFloor, shChargedFloor,
    shSStarFloor, shOverFloor, shTriFloor, shFeatherFloor,
    shBarrowFloor, shNewFloor, shTrollFloor, shButterflyFloor,
    shLavaFloor, shLavaSeabed, shSeabed, shCloudSeabed,
    shCaveSeabed, shPalaceFloor, shDemonFloor, shCaveFloor,
    shDesertFloor, shPowerFloor, shRoseFloor, shSwitchFloor,
    shTurtleFloor, shRedRockFloor[3], shDragonFloor;

  ld dlow_table[SIDEPARS], dhi_table[SIDEPARS], dfloor_table[SIDEPARS];

  int prehpc;
  vector<hyperpoint> hpc;
  bool first;

  bool validsidepar[SIDEPARS];

  vector<glvertex> ourshape;
#endif  

  hpcshape shFullCross[2];
  hpcshape *last;

  int SD3, SD6, SD7, S12, S14, S21, S28, S42, S36, S84;
  
  vector<array<int, 3>> symmetriesAt;
  
  #ifndef SCALETUNER
  static constexpr
  #endif
  double bscale7 = 1, brot7 = 0, bscale6 = 1, brot6 = 0;
  
  vector<hpcshape*> allshapes;
  
  transmatrix shadowmulmatrix;
  
  map<usershapelayer*, hpcshape> ushr;
  
  void prepare_basics();
  void prepare_compute3();
  void prepare_shapes();
  void prepare_usershapes();

  void hpcpush(hyperpoint h);
  void hpcsquare(hyperpoint h1, hyperpoint h2, hyperpoint h3, hyperpoint h4);
  void chasmifyPoly(double fac, double fac2, int k);
  void shift(hpcshape& sh, double dx, double dy, double dz);
  void initPolyForGL();
  void extra_vertices();
  transmatrix ddi(int a, ld x);
  void drawTentacle(hpcshape &h, ld rad, ld var, ld divby);
  hyperpoint hpxyzsc(double x, double y, double z);
  hyperpoint turtlevertex(int u, double x, double y, double z);
  
  void bshape(hpcshape& sh, PPR prio);
  void finishshape();
  void bshape(hpcshape& sh, PPR prio, double shzoom, int shapeid, double bonus = 0, flagtype flags = 0);
  
  void copyshape(hpcshape& sh, hpcshape& orig, PPR prio);
  void zoomShape(hpcshape& old, hpcshape& newsh, double factor, PPR prio);
  void pushShape(usershapelayer& ds);
  void make_sidewalls();
  void procedural_shapes();
  void make_wall(int id, const vector<hyperpoint> vertices, vector<ld> weights = equal_weights);
  void create_wall3d();
  void configure_floorshapes();
  
  void init_floorshapes();
  void bshape2(hpcshape& sh, PPR prio, int shapeid, struct matrixlist& m);
  void bshape_regular(floorshape &fsh, int id, int sides, int shift, ld size, cell *model);
  void generate_floorshapes_for(int id, cell *c, int siid, int sidir);
  void generate_floorshapes();
  void make_floor_textures_here();

  vector<hyperpoint> get_shape(hpcshape sh);
  void add_cone(ld z0, const vector<hyperpoint>& vh, ld z1);
  void add_prism_sync(ld z0, vector<hyperpoint> vh0, ld z1, vector<hyperpoint> vh1);
  void add_prism(ld z0, vector<hyperpoint> vh0, ld z1, vector<hyperpoint> vh1);
  void shift_last(ld z);
  void shift_shape(hpcshape& sh, ld z);
  void shift_shape_orthogonally(hpcshape& sh, ld z);
  void add_texture(hpcshape& sh);
  void make_ha_3d(hpcshape& sh, bool isarmor, ld scale);
  void make_humanoid_3d(hpcshape& sh);
  void addtri(array<hyperpoint, 3> hs, int kind);
  void make_armor_3d(hpcshape& sh, int kind = 1); 
  void make_foot_3d(hpcshape& sh);
  void make_head_only();
  void make_head_3d(hpcshape& sh);
  void make_paw_3d(hpcshape& sh, hpcshape& legsh);
  void make_abody_3d(hpcshape& sh, ld tail);
  void make_ahead_3d(hpcshape& sh);
  void make_skeletal(hpcshape& sh, ld push = 0);
  void make_revolution(hpcshape& sh, int mx = 180, ld push = 0);
  void make_revolution_cut(hpcshape &sh, int each = 180, ld push = 0, ld width = 99);
  void clone_shape(hpcshape& sh, hpcshape& target);
  void animate_bird(hpcshape& orig, hpcshape_animated& animated, ld body);
  void slimetriangle(hyperpoint a, hyperpoint b, hyperpoint c, ld rad, int lev);
  void balltriangle(hyperpoint a, hyperpoint b, hyperpoint c, ld rad, int lev);
  void make_ball(hpcshape& sh, ld rad, int lev);
  void make_star(hpcshape& sh, ld rad);
  void make_euclidean_sky();
  void adjust_eye(hpcshape& eye, hpcshape head, ld shift_eye, ld shift_head, int q, ld zoom=1);
  void shift_last_straight(ld z);
  void queueball(const transmatrix& V, ld rad, color_t col, eItem what);
  void make_shadow(hpcshape& sh);
  void make_3d_models();
  
  /* Goldberg parameters */
  #if CAP_GP
  struct gpdata_t {
    transmatrix Tf[MAX_EDGE][32][32][6];
    transmatrix corners;
    ld alpha;
    int area;
    };
  shared_ptr<gpdata_t> gpdata;
  #endif
  
  int state;
  int usershape_state;
  
  geometry_information() { last = NULL; state = usershape_state = 0; gpdata = NULL; }
  
  void require_basics() { if(state & 1) return; state |= 1; prepare_basics(); }
  void require_shapes() { if(state & 2) return; state |= 2; prepare_shapes(); }
  void require_usershapes() { if(usershape_state == usershape_changes) return; usershape_state = usershape_changes; prepare_usershapes(); }
  int timestamp;
  };

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

  typedef pair<cell**, bool> euc_pointer;
  static const int max_vec = (1<<14);
  extern bool needConfirmationEvenIfSaved();

#define EX
#define EXT(z)
}

#define IS(z) = z
#include "autohdr.h"
#undef IS
#define IS(z)

namespace hr {
  inline bool movepcto(const movedir& md) { return movepcto(md.d, md.subdir); }

  inline hyperpoint cpush0(int c, ld x) { 
    hyperpoint h = Hypc;
    h[GDIM] = cos_auto(x);
    h[c] = sin_auto(x);
    return h;
    }
  
  inline hyperpoint xspinpush0(ld alpha, ld x) { 
    hyperpoint h = Hypc;
    h[GDIM] = cos_auto(x);
    h[0] = sin_auto(x) * cos(alpha);
    h[1] = sin_auto(x) * -sin(alpha);
    return h;
    }

  inline hyperpoint xpush0(ld x) { return cpush0(0, x); }
  inline hyperpoint ypush0(ld x) { return cpush0(1, x); }
  inline void reset_projection() { new_projection_needed = true; }

  // T * C0, optimized
  inline hyperpoint tC0(const transmatrix &T) {
    hyperpoint z;
    for(int i=0; i<MDIM; i++) z[i] = T[i][DIM];
    return z;
    }
  }
