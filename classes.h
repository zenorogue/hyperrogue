// Hyperbolic Rogue -- items, monsters, walls, lands, descriptions, etc.
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file classes.h
 *  \brief header file for content
 */

namespace hr {

typedef unsigned color_t;

typedef unsigned long long flagtype;
#define Flag(i) (flagtype(1ull<<i))

static inline void set_flag(flagtype& f, flagtype which, bool b) {
  if(b) f |= which;
  else f &= ~which;
  }

// we use CF not MF to avoid confusion with MF_ movement flags
static const flagtype CF_NOGHOST = Flag(0);
static const flagtype CF_RAIDER = Flag(1);
static const flagtype CF_PRINCESS = Flag(2);
static const flagtype CF_MIMIC = Flag(3);
static const flagtype CF_GOK = Flag(4);
static const flagtype CF_NONLIVING = Flag(5);
static const flagtype CF_METAL = Flag(6);
static const flagtype CF_STUNNABLE = Flag(7);
static const flagtype CF_HP = Flag(8);
static const flagtype CF_MOUNTABLE = Flag(9);
static const flagtype CF_FRIENDLY = Flag(10);
static const flagtype CF_PLAYER = Flag(11);
static const flagtype CF_BUG = Flag(12);
static const flagtype CF_IVY = Flag(13);
static const flagtype CF_PART = Flag(14);
static const flagtype CF_MUTANTIVY = Flag(15);
static const flagtype CF_ANYIVY = Flag(16);
static const flagtype CF_BULLET = Flag(17);
static const flagtype CF_DEMON = Flag(18);
static const flagtype CF_WORM = Flag(19);
static const flagtype CF_WITCH = Flag(20);
static const flagtype CF_BIRD = Flag(21);
static const flagtype CF_SLOWMOVER = Flag(22);
static const flagtype CF_MAGNETIC = Flag(23);
static const flagtype CF_SWITCH = Flag(24);
static const flagtype CF_GHOST = Flag(25);
static const flagtype CF_SHARK = Flag(26);
static const flagtype CF_SLIME = Flag(27);
static const flagtype CF_DRAGON = Flag(28);
static const flagtype CF_KRAKEN = Flag(29);
static const flagtype CF_NOBLOW = Flag(30);
static const flagtype CF_MULTITILE = Flag(31);
static const flagtype CF_LEADER = Flag(32);
static const flagtype CF_FLYING = Flag(33);
static const flagtype CF_ATTACK_THRU_VINE = Flag(34);
static const flagtype CF_ATTACK_NONADJACENT = Flag(35);
static const flagtype CF_NOHIGHLIGHT = Flag(36);
static const flagtype CF_INACTIVE = Flag(37);
static const flagtype CF_UNARMED = Flag(38);
static const flagtype CF_IGNORE_PLATE = Flag(39);
static const flagtype CF_BULL = Flag(40);
static const flagtype CF_TROLL = Flag(41);
static const flagtype CF_IGNORE_SMELL = Flag(42);
static const flagtype CF_RATLING = Flag(43);
static const flagtype CF_POWER = Flag(44);
static const flagtype CF_GHOSTMOVER = Flag(45);
static const flagtype CF_TECHNICAL = Flag(46);
static const flagtype CF_MOVED = Flag(47);
static const flagtype CF_FACING = Flag(48);
static const flagtype CF_FACE_UP = Flag(49);
static const flagtype CF_FACE_SIDE = Flag(50);

enum eMonster {
  #define MONSTER(a,b,c,d,e,f,g,h) d,
  #include "content.cpp"
  motypes
  };

struct monstertype {
  char  glyph;
  color_t color;
  const char *name;
  flagtype flags;
  enum eMonster mgroup;
  const char *help;
  };

struct genderswitch_t {
  int gender;
  eMonster m;
  const char *name;
  const char *desc;
  };

#define NUM_GS 6

enum eOrbshape { osNone, osLove, osRanged, osOffensive, osFriend, osUtility, osDirectional, osWarping };

static const flagtype ZERO = 0;

static const flagtype IF_SHARD = Flag(0);
static const flagtype IF_FIREPROOF = Flag(1);
static const flagtype IF_PROTECTION = Flag(2);
static const flagtype IF_EMPATHY = Flag(3);
static const flagtype IF_RANGED = Flag(4);
static const flagtype IF_SHMUPLIFE = Flag(5);
static const flagtype IF_REVIVAL = Flag(6);

// 0 = basic treasure, 1 = other item, 2 = power orb, 3 = not an item
#define IC_TREASURE 0
#define IC_OTHER 1
#define IC_ORB 2
#define IC_NAI 3

struct itemtype {
  char  glyph;
  color_t color;
  const char *name;
  int itemclass;
  flagtype flags;
  eOrbshape orbshape;
  const char *help;
  };

enum eItem { 
  #define ITEM(a,b,c,d,e,f,g,h,i) d,
  #include "content.cpp"
  ittypes
  };

enum eSlimegroup { sgNone, sgCave, sgWater, sgFloorA, sgFloorB, sgVine, sgTree };

static const flagtype WF_WATER = Flag(0);
static const flagtype WF_BOAT = Flag(1);
static const flagtype WF_CHASM = Flag(2);
static const flagtype WF_NOFLIGHT = Flag(3);
static const flagtype WF_FIRE = Flag(4);
static const flagtype WF_THUMPER = Flag(5);
static const flagtype WF_ACTIVABLE = Flag(6);
static const flagtype WF_ALCHEMY = Flag(7);
static const flagtype WF_RED = Flag(8);
static const flagtype WF_WALL = Flag(9);
static const flagtype WF_PUSHABLE = Flag(10);
static const flagtype WF_CONE = Flag(11);
static const flagtype WF_STDTREE = Flag(12);
static const flagtype WF_GRAVE = Flag(13);
static const flagtype WF_REPTILE = Flag(14);
static const flagtype WF_HEATCOLOR = Flag(15);
static const flagtype WF_HIGHWALL = Flag(16);
static const flagtype WF_THORNY = Flag(17);
static const flagtype WF_TIMEOUT = Flag(18);
static const flagtype WF_CISLAND = Flag(19);
static const flagtype WF_SULPHURIC = Flag(20);
static const flagtype WF_HALFVINE = Flag(21);

struct walltype {
  char  glyph;
  color_t color;
  const char *name;
  flagtype flags;
  int snakelevel;
  eSlimegroup sg;
  const char *help;
  };

enum eWall { 
  #define WALL(a,b,c,d,e,f,g,h,i) d,
  #include "content.cpp"
  walltypes
  };

static const flagtype LF_GENERATE_ALL = Flag(0);
static const flagtype LF_ICY = Flag(1);
static const flagtype LF_GRAVITY = Flag(2);
static const flagtype LF_EQUI = Flag(3);
static const flagtype LF_WARPED = Flag(4);
static const flagtype LF_CYCLIC = Flag(5);
static const flagtype LF_TECHNICAL = Flag(6);
static const flagtype LF_MIRROR = Flag(7);
static const flagtype LF_SEA = Flag(8);
static const flagtype LF_COASTAL = Flag(9);
static const flagtype LF_PURESEA = Flag(10);
static const flagtype LF_ELEMENTAL = Flag(11);
static const flagtype LF_HAUNTED = Flag(12);
static const flagtype LF_TROLL = Flag(13);
static const flagtype LF_INMIRROR = Flag(14);
static const flagtype LF_INMIRRORORWALL = Flag(15);

struct landtype {
  color_t color;
  const char *name;
  flagtype flags;
  eItem treasure;
  const char *help;
  };

enum eLand {
  #define LAND(a,b,c,d,e,f,g) c,
  #include "content.cpp"
  landtypes
  };

enum eGeometry {
  gNormal, gEuclid, gSphere, gElliptic, gZebraQuotient, gFieldQuotient, gTorus, gOctagon, g45, g46, g47, gSmallSphere, gTinySphere, gEuclidSquare, gSmallElliptic, 
  gKleinQuartic, gBolza, gBolza2, gMinimal, gBinaryTiling, gArchimedean, 
  gMacbeath, gBring, gSchmutzM2, gSchmutzM3, gCrystal, gOctahedron, 
  gBinary3, gCubeTiling, gCell120, gECell120, gRhombic3, gBitrunc3, 
  gSpace534, gSpace435, 
  gCell5, 
  gCell8, gECell8,
  gCell16, gECell16,
  gCell24, gECell24,
  gCell600, gECell600,
  gHoroTris, gHoroRec, gHoroHex,
  gField435, gField534,
  gBinary4, gSol,
  gKiteDart2, gKiteDart3, gNil, gProduct, gRotSpace,
  gTernary, gNIH, gSolN, gInfOrder, gSpace336, gSpace344, gCrystal344,
  gArnoldCat,
  gGUARD};

enum eGeometryClass { gcHyperbolic, gcEuclid, gcSphere, gcSolNIH, gcNil, gcProduct, gcSL2 };

enum class eVariation { bitruncated, pure, goldberg, irregular, dual };  

typedef flagtype modecode_t;

/** only the actual geometry */
struct geometryinfo1 {
  /** geometry class */
  eGeometryClass kind;
  /** dimension of the gameplay (2 for crystal) */
  int gameplay_dimension;
  /** dimension of the graphics, may be greater than gameplay_dimension with vid.always3 on */
  int graphical_dimension;
  /** dimension of the homogeneous vector space used, usually graphical_dimension+1, but 3 in product */
  int homogeneous_dimension;
  /** signature of the scalar product used */
  int sig[4];
  };

struct geometryinfo {
  std::string tiling_name;
  std::string quotient_name;
  const char* menu_displayed_name;
  const char* shortname;
  int sides;
  int vertex;
  flagtype flags;
  geometryinfo1 g;
  modecode_t xcode;
  std::array<int,2> distlimit; // bitrunc, non-bitrunc
  eVariation default_variation;
  };

static const flagtype qBOUNDED         = 1;
static const flagtype qANYQ            = 2;
static const flagtype qNONORIENTABLE   = 4;
static const flagtype qSMALL           = 8;

static const flagtype qFIELD           = 16;
static const flagtype qDOCKS           = 32;
static const flagtype qZEBRA           = 64;

static const flagtype qELLIPTIC        = 128;

static const flagtype qBINARY          = 256;
static const flagtype qPENROSE         = 512;

static const flagtype qREGULAR         = 1024; /* not set! */
static const flagtype qARCHI           = 2048;
static const flagtype qHYBRID          = 4096;
static const flagtype qCRYSTAL         = 8192;
static const flagtype qSOL             = 16384;
static const flagtype qEXPERIMENTAL    = 32768;
static const flagtype qNIH             = 65536;

static const flagtype qIDEAL           = 131072;

// note: dnext assumes that x&7 equals 7
static const int SEE_ALL = 50;
static const int FORBIDDEN = -1;

extern eGeometry geometry;
extern eVariation variation;

extern std::vector<geometryinfo> ginf;

extern monstertype minf[motypes];
extern itemtype iinf[ittypes];

extern const landtype linf[landtypes];
extern color_t floorcolors[landtypes];
extern walltype winf[walltypes];

enum cpatterntype { 
  cpFootball, cpThree, cpChess, cpSingle, cpSingleSym, cpOddEven, cpLarge, cpZebra, cpUnknown
  };
  
struct landtacinfo { eLand l; int tries, multiplier; };

enum eModel {
  mdDisk, mdHalfplane, mdBand, mdPolygonal, mdFormula,
  // 5..8.
  mdEquidistant, mdEquiarea, mdBall, mdHyperboloid, 
  // 9..13
  mdHemisphere, mdBandEquidistant, mdBandEquiarea, mdSinusoidal, mdTwoPoint, 
  // 14..16
  mdFisheye, mdJoukowsky, mdJoukowskyInverted,
  // 17..19
  mdRotatedHyperboles, mdSpiral, mdPerspective,
  // 20..24
  mdEquivolume, mdCentralInversion, mdSimulatedPerspective, mdTwoHybrid, mdGeodesic,
  // 25
  mdMollweide, mdCentralCyl, mdCollignon, mdHorocyclic,
  // 29..
  mdGUARD, mdPixel, mdHyperboloidFlat, mdPolynomial, mdManual
  };

typedef unsigned long long flagtype;

namespace mf {
  static const flagtype azimuthal = 1;
  static const flagtype cylindrical = 2;
  static const flagtype equiarea = 4;
  static const flagtype equidistant = 8;
  static const flagtype conformal = 16;
  static const flagtype euc_boring = 32;
  static const flagtype space = 64;
  static const flagtype hyper_only = 128;
  static const flagtype hyper_or_torus = 256;
  static const flagtype pseudocylindrical = 512; /* includes cylindrical */
  static const flagtype equivolume = 1024;
  static const flagtype twopoint = 2048;
  static const flagtype uses_bandshift = 4096;
  
  static const flagtype band = (cylindrical | pseudocylindrical | uses_bandshift);
  static const flagtype pseudoband = (pseudocylindrical | uses_bandshift);
  };
  
struct modelinfo {
  const char *name_hyperbolic;
  const char *name_euclidean;
  const char *name_spherical;  
  
  flagtype flags;

  int is_azimuthal;
  int is_band;
  int is_equiarea;
  int is_equidistant;
  int is_conformal;
  const char* name;
  };

extern std::vector<modelinfo> mdinf;

static const int OINF = 100;

}
