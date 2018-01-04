// This is the main header file of HyperRogue. Mostly everything is dumped here.
// It is quite chaotic.

// scale the Euclidean
#define EUCSCALE 2.3

#define NUMWITCH 7

// achievements

#define LB_YENDOR_CHALLENGE 40
#define LB_PURE_TACTICS 41
#define NUMLEADER 76
#define LB_PURE_TACTICS_SHMUP 49
#define LB_PURE_TACTICS_COOP 50

#if ISMOBILE || ISWEB || ISPANDORA || 1
typedef double ld;
#define LDF "%lf"
#define PLDF "lf"
#define ASINH asinh
#else
typedef long double ld;
#define LDF "%Lf"
#define PLDF "Lf"
#define ASINH asinhl
#endif

#define DEBMEM(x) // { x fflush(stdout); }

struct hyperpoint {
  ld tab[3];
  ld& operator [] (int i) { return tab[i]; }
  const ld& operator [] (int i) const { return tab[i]; }
  };

struct transmatrix {
  ld tab[3][3];
  ld * operator [] (int i) { return tab[i]; }
  const ld * operator [] (int i) const { return tab[i]; }
  };

inline hyperpoint operator * (const transmatrix& T, const hyperpoint& H) {
  hyperpoint z;
  for(int i=0; i<3; i++) {
    z[i] = 0;
    for(int j=0; j<3; j++) z[i] += T[i][j] * H[j];
    }
  return z;
  }

inline transmatrix operator * (const transmatrix& T, const transmatrix& U) {
  transmatrix R;
  // for(int i=0; i<3; i++) for(int j=0; j<3; j++) R[i][j] = 0;
  for(int i=0; i<3; i++) for(int j=0; j<3; j++) // for(int k=0; k<3; k++)
    R[i][j] = T[i][0] * U[0][j] + T[i][1] * U[1][j] + T[i][2] * U[2][j];
  return R;
  }

extern int cellcount, heptacount;

// cell information for the game

struct gcell {

  // main fields
  eLand land : 8;
  eWall wall : 8;
  eMonster monst : 8;
  eItem item : 8;

  // if this is a barrier, what lands on are on the sides?
  eLand barleft : 8, barright : 8; 

  unsigned ligon : 1;    // is it sparkling with lightning?

  unsigned 
    pathdist : 7,       // player distance wrt usual movement
    cpdist : 8, mpdist : 8; // current/minimum player distance

  unsigned 
    mondir : 4,         // monster direction, for multi-tile monsters and graphics
    bardir : 4,         // barrier direction
    stuntime : 4,       // stun time left (for Palace Guards and Skeletons)
    hitpoints : 4;      // hitpoints left (for Palace Guards, also reused as cpid for mirrors)
  
  unsigned landflags : 8;      // extra flags for land
  
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
  
  gcell() { cellcount++; }
  ~gcell() { cellcount--; }
  };

#define landparam LHU.landpar

#define fval LHU.fi.fieldval

#define NODIR 8
#define NOBARRIERS 9
#define MODFIXER 23520

inline void tsetspin(uint32_t& t, int d, int spin) { t &= ~(15 << (d<<2)); t |= spin << (d<<2); }
inline int tspin(uint32_t& t, int d) { return (t >> (d<<2)) & 7; }
inline int tmirror(uint32_t& t, int d) { return (t >> ((d<<2)+3)) & 1; }

inline int fixrot(int a) { return (a+MODFIXER)% S7; }
inline int fix42(int a) { return (a+MODFIXER)% S42; }

struct cell;

// automaton state
enum hstate { hsOrigin, hsA, hsB, hsError, hsA0, hsA1, hsB0, hsB1, hsC };

struct heptagon {
  // automaton state
  hstate s : 6;
  int dm4: 2;
  // we are spin[i]-th neighbor of move[i]
  uint32_t spintable;
  int spin(int d) { return tspin(spintable, d); }
  int mirror(int d) { return tmirror(spintable, d); }
  void setspin(int d, int sp) { tsetspin(spintable, d, sp); }
  // neighbors; move[0] always goes towards origin,
  // and then we go clockwise
  heptagon* move[MAX_EDGE];
  // distance from the origin
  short distance;
  // emerald/wineyard generator
  short emeraldval;
  // fifty generator
  short fiftyval;
  // zebra generator (1B actually)
  short zebraval;
  // field id
  int fieldval;
  // evolution data
  short rval0, rval1;
  struct cdata *cdata;
  // central cell
  cell *c7;
  // associated generator of alternate structure, for Camelot and horocycles
  heptagon *alt;
  // functions
  heptagon*& modmove(int i) { return move[fixrot(i)]; }
  unsigned char gspin(int i) { return spin(fixrot(i)); }
  heptagon () { heptacount++; }
  ~heptagon () { heptacount--; }
  };

struct heptspin {
  heptagon *h;
  int spin;
  bool mirrored;
  heptspin() { mirrored = false; }
  };

struct cell : gcell {
  char type; // 6 for hexagons, 7 for heptagons

  // wall parameter, used for remaining power of Bonfires and Thumpers
  char wparam;

  // 'tmp' is used for:
  // pathfinding algorithm used by monsters with atypical movement (which do not use pathdist)
  // bugs' pathfinding algorithm
  short aitmp;

  uint32_t spintable;
  int spin(int d) { return tspin(spintable, d); }
  int spn(int d) { return tspin(spintable, d); }
  int mirror(int d) { return tmirror(spintable, d); }

  heptagon *master;
  cell *mov[MAX_EDGE]; // meaning very similar to heptagon::move
  };

// similar to heptspin from heptagon.cpp
struct cellwalker {
  cell *c;
  int spin;
  bool mirrored;
  cellwalker(cell *c, int spin, bool m=false) : c(c), spin(spin), mirrored(m) { }
  cellwalker() { mirrored = false; }
  };

#define BUGCOLORS 3

// land completion for shared unlocking
#define U5 (inv::on ? 10 : 5)
// land completion for advanced unlocking
#define U10 (inv::on ? 25 : 10)

// land completion
#define R10 (inv::on ? 50 : 10)
// intermediate lands
#define R30 (inv::on ? 100 : 30)
// advanced lands
#define R60 (inv::on ? 200 : 60)
// advanced lands II
#define R90 (inv::on ? 300 : 90)
// Crossroads IV
#define R200 (inv::on ? 800 : 200)
// Crossroads V
#define R300 (inv::on ? 1200 : 300)
// kill types for Dragon Chasms
#define R20 (inv::on ? 30 : 20)
// kill count for Graveyard/Hive
#define R100 (inv::on ? 500 : 100)

string XLAT(string x);
string cts(char c);
string its(int i);
int hrand(int i);

#ifndef STDSIZE
template<class T> int size(const T& x) {return int(x.size()); }
#endif

extern int currentscore[NUMLEADER];
extern int syncstate;

// initialize the achievement system.
void achievement_init();

// close the achievement system.
void achievement_close();

// gain the achievement with the given name.
// flags: 'e' - for Euclidean, 's' - for Shmup, '7' - for heptagonal
// Only awarded if special modes are matched exactly.
void achievement_gain(const char*, char flags = 0);

// gain the achievement for collecting a number of 'it'.
void achievement_collection(eItem it, int prevgold, int newgold);

// this is used for 'counting' achievements, such as kill 10
// monsters at the same time.
void achievement_count(const string& s, int current, int prev);

// scores for special challenges
void achievement_score(int cat, int score);

// gain the victory achievements. Set 'hyper' to true for
// the Hyperstone victory, and false for the Orb of Yendor victory.
void achievement_victory(bool hyper);

// gain the final achievements. Called with really=false whenever the user
// looks at their score, and really=true when the game really ends.
void achievement_final(bool really);

// display the last achievement gained.
void achievement_display();

// call the achievement callbacks
void achievement_pump();

// achievements received this game
extern vector<string> achievementsReceived;

// game forward declarations
typedef unsigned long long flagtype;
#define Flag(i) (flagtype(1ull<<i))

bool mirrorkill(cell *c);
bool isNeighbor(cell *c1, cell *c2);
void checkTide(cell *c);
namespace anticheat { extern bool tampered; }
int numplayers();
void removeIvy(cell *c);
bool cellEdgeUnstable(cell *c, flagtype flags = 0);
int coastvalEdge(cell *c);
typedef int cellfunction(cell*);
int towerval(cell *c, cellfunction* cf = &coastvalEdge);
#define HRANDMAX 0x7FFFFFFF
int hrandpos(); // 0 to HRANDMAX
void restartGame(char switchWhat = 0, bool push = false, bool keep_screens = true);
int landMultiplier(eLand l);
eItem treasureType(eLand l);
void buildBarrier(cell *c, int d, eLand l = laNone);
void extendBarrier(cell *c);
bool buildBarrier4(cell *c, int d, int mode, eLand ll, eLand lr);
bool buildBarrier6(struct cellwalker cw, int type);
bool makeEmpty(cell *c);
bool isCrossroads(eLand l);
enum orbAction { roMouse, roKeyboard, roCheck, roMouseForce, roMultiCheck, roMultiGo };
void moveItem (cell *from, cell *to, bool activateYendor);
void uncoverMines(cell *c, int lev, int dist);
bool survivesMine(eMonster m);
void killMonster(cell *c, eMonster who_killed, flagtype flags = 0);
void toggleGates(cell *ct, eWall type, int rad);
bool destroyHalfvine(cell *c, eWall newwall = waNone, int tval = 6);
void buildCrossroads2(cell *c);
bool isHaunted(eLand l);
heptagon *createAlternateMap(cell *c, int rad, hstate firststate, int special=0);
void generateAlts(heptagon *h, int levs = S3-3);
void setdist(cell *c, int d, cell *from);
void checkOnYendorPath();
void killThePlayerAt(eMonster m, cell *c, flagtype flags);
bool notDippingFor(eItem i);
bool collectItem(cell *c2, bool telekinesis = false);
void castLightningBolt(struct cellwalker lig);
bool movepcto(int d, int subdir = 1, bool checkonly = false);
void stabbingAttack(cell *mf, cell *mt, eMonster who, int bonuskill = 0);
bool earthMove(cell *from, int dir);
void messageKill(eMonster killer, eMonster victim);
void moveMonster(cell *ct, cell *cf);
int palaceHP();
void placeLocalOrbs(cell *c);
int elementalKills();
bool elementalUnlocked();
bool trollUnlocked();
bool isMultitile(eMonster m);
void checkFreedom(cell *cf);
int rosedist(cell *c);
bool canPushStatueOn(cell *c);

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
  int d; // 0 to 6, or one of the following -- warning: not used consistently
  #define MD_WAIT (-1)
  #define MD_DROP (-2)
  #define MD_UNDECIDED (-3)
  #define MD_USE_ORB (-4)
  int subdir; // for normal movement (0-6): turn left or right
  cell *tgt;  // for MD_USE_ORB: target cell
  };

inline bool movepcto(const movedir& md) { return movepcto(md.d, md.subdir); }

void activateActiv(cell *c, bool msg);

// shmup

struct charstyle {
  int charid;
  unsigned skincolor, haircolor, dresscolor, swordcolor, dresscolor2, uicolor;
  };

string csname(charstyle& cs);
void initcs(charstyle& cs);
void savecs(FILE *f, charstyle& cs, int vernum);
void loadcs(FILE *f, charstyle& cs, int vernum);

#define MAXPLAYER 7
#define MAXJOY 8

#define MAXBUTTON 64
#define MAXAXE 16
#define MAXHAT 4

namespace multi {

  extern bool shmupcfg;
  extern bool alwaysuse;
  void recall();
  extern cell *origpos[MAXPLAYER], *origtarget[MAXPLAYER];
  extern int players;
  extern cellwalker player[MAXPLAYER];
  extern bool flipped[MAXPLAYER];
  cell *mplayerpos(int i);
  
  extern vector<int> revive_queue; // queue for revival

  extern movedir whereto[MAXPLAYER]; // player's target cell

  extern int cpid; // player id -- an extra parameter for player-related functions
  extern int cpid_edit; // cpid currently being edited

  // treasure collection, kill, and death statistics
  extern int treasures[MAXPLAYER], kills[MAXPLAYER], deaths[MAXPLAYER];

  struct config {
    int players;
    int subconfig;
    int setwhat;
    char keyaction[512];
    char joyaction[MAXJOY][MAXBUTTON];
    char axeaction[MAXJOY][MAXAXE];
    char hataction[MAXJOY][MAXHAT][4];
    int  deadzoneval[MAXJOY][MAXAXE];
    };

  void saveConfig(FILE *f);
  void loadConfig(FILE *f);
  void initConfig();
   
  extern charstyle scs[MAXPLAYER];

  bool playerActive(int p);
  int activePlayers();
  cell *multiPlayerTarget(int i);
  void checklastmove();
  void leaveGame(int i);
  void showShmupConfig();
  }

namespace shmup {
  using namespace multi;
  void recall();
  extern bool on;
  extern bool safety;
  extern int curtime;
  void clearMonsters();
  void clearMemory();
  void init();
  void teleported();
  
  struct monster {
    eMonster type;
    cell *base;
    cell *torigin; 
      // tortoises: origin
      // butterflies: last position
    transmatrix at;
    transmatrix pat;
    eMonster stk;
    bool dead;
    bool notpushed;
    bool inBoat;
    monster *parent; // who shot this missile
    eMonster parenttype; // type of the parent
    int nextshot;    // when will it be able to shot (players/flailers)
    int pid;         // player ID
    char hitpoints;
    int stunoff;
    int blowoff;
    double swordangle; // sword angle wrt at
    double vel;        // velocity, for flail balls
    double footphase;
    bool isVirtual;  // off the screen: gmatrix is unknown, and pat equals at
    
    monster() { 
      dead = false; inBoat = false; parent = NULL; nextshot = 0; 
      stunoff = 0; blowoff = 0; footphase = 0;
      }
  
    void store();
      
    void findpat();
  
    cell *findbase(const transmatrix& T);
  
    void rebasePat(const transmatrix& new_pat);
  
    };

  extern struct monster* mousetarget;
  extern eItem targetRangedOrb(orbAction a);
  void degradeDemons();
  void killThePlayer(eMonster m);
  void killThePlayer(eMonster m, int i);
  void visibleFor(int t);
  bool verifyTeleport();
  bool dragonbreath(cell *dragon);

  void shmupDrownPlayers(cell *c);

  cell *playerpos(int i);

  bool playerInBoat(int i);
  void destroyBoats(cell *c);
  bool boatAt(cell *c);
  
  void virtualRebase(cell*& base, transmatrix& at, bool tohex);
  void virtualRebase(shmup::monster *m, bool tohex);
  transmatrix calc_relative_matrix(cell *c, heptagon *h1);
  void fixStorage();
  void addShmupHelp(string& out);
  void activateArrow(cell *c);
  transmatrix& ggmatrix(cell *c);
  }

// graph

void showMissionScreen();

void restartGraph();
void resetmusic();

void drawFlash(cell* c);
void drawBigFlash(cell* c);
void drawParticle(cell *c, int col, int maxspeed = 100);
void drawParticles(cell *c, int col, int qty, int maxspeed = 100);
void drawFireParticles(cell *c, int qty, int maxspeed = 100);
int firecolor(int phase);

void drawLightning();
void drawSafety();
void restartGraph();
void movepckeydir(int);

void centerpc(ld aspd);

void displayButton(int x, int y, const string& name, int key, int align, int rad = 0);
void displayColorButton(int x, int y, const string& name, int key, int align, int rad, int color, int color2 = 0);
inline string ONOFF(bool b) { return XLAT(b ? "ON" : "OFF"); }
int darkened(int c);
extern int getcstat;
bool displaychr(int x, int y, int shift, int size, char chr, int col);
bool displayfr(int x, int y, int b, int size, const string &s, int color, int align);
bool displayfrSP(int x, int y, int sh, int b, int size, const string &s, int color, int align, int p);

bool outofmap(hyperpoint h);
void applymodel(hyperpoint H, hyperpoint& Hscr);
void drawCircle(int x, int y, int size, int color);
void fixcolor(int& col);
int displaydir(cell *c, int d);
hyperpoint gethyper(ld x, ld y);
void resetview(); extern heptspin viewctr; extern cellwalker centerover;
void drawthemap();
void drawfullmap();
bool displaystr(int x, int y, int shift, int size, const char *str, int color, int align);
bool displaystr(int x, int y, int shift, int size, const string& str, int color, int align);

extern int darken, inmirrorcount;
void calcparam();

#if CAP_SDL
int& qpixel(SDL_Surface *surf, int x, int y);
void setvideomode();
void saveHighQualityShot(const char *fname = NULL, const char *caption = NULL, int fade = 255);
#endif

#if CAP_CONFIG
void saveConfig();
#endif

extern hyperpoint mouseh;

extern hyperpoint ccenter;
extern ld crad;

extern bool mousepressed, anyshiftclick;
extern string help;

typedef function<void()> reaction_t;
typedef function<bool()> bool_reaction_t;

extern reaction_t help_delegate;  

#define HELPFUN(x) (help_delegate = x, "HELPFUN")

struct videopar {
  ld scale, eye, alpha, sspeed, mspeed, yshift, camera_angle;
  ld ballangle, ballproj;
  int mobilecompasssize;
  int aurastr, aurasmoothen;

  bool full;
  bool goteyes;  // for rendering
  bool goteyes2; // for choosing colors
  int graphglyph; // graphical glyphs
  bool darkhepta;
  int shifttarget;
  
  int xres, yres, framelimit;
  
  int xscr, yscr;
  
  ld xposition, yposition;
  
  // paramaters calculated from the above
  int xcenter, ycenter;
  int radius;
  int scrsize;
  ld alphax, beta;
  
  bool grid;
  int particles;
  
  int fsize;
  int flashtime;
  
  int wallmode, monmode, axes;
  bool revcontrol;
  
  int msgleft, msglimit;

  // for OpenGL
  float scrdist;
  
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

  int joyvalue, joyvalue2, joypanthreshold;
  ld joypanspeed;
  
  charstyle cs;
  
  bool samegender; // same gender for the Princess?
  int language;
  
  bool backeffects; // background particle effects
  
  int killreduction, itemreduction, portreduction;
  
  multi::config scfg;
  
  int steamscore;
  bool drawmousecircle; // draw the circle around the mouse
  bool skipstart;       // skip the start menu
  int quickmouse;       // quick mouse on the map
  int timeformat;       // time format used in the message log
  };

extern videopar vid;

extern vector< function<void()> > screens;

template<class T> void pushScreen(T& x) { screens.push_back(x); } 
inline void popScreen() { screens.pop_back(); }
inline void popScreenAll() { while(size(screens)>1) popScreen(); }

extern transmatrix View; // current rotation, relative to viewctr
extern transmatrix cwtV; // player-relative view

extern cell *mouseover, *mouseover2, *lmouseover;
extern string mouseovers;

extern struct SDL_Surface *s;

namespace patterns {
  extern char whichShape;

  extern char whichPattern;
  
  static const char PAT_WARP = 0;
  static const char PAT_ZEBRA = 'z';
  static const char PAT_EMERALD = 'f';
  static const char PAT_PALACE = 'p';
  static const char PAT_FIELD = 'F';
  static const char PAT_DOWN = 'H';
  static const char PAT_COLORING = 'C';
  static const char PAT_SIBLING = 'S';
  static const char PAT_CHESS = 'c';

  extern int subpattern_flags;
  
  static const int SPF_ROT = 1;
  static const int SPF_SYM01 = 2;
  static const int SPF_SYM02 = 4;
  static const int SPF_SYM03 = 8;
  static const int SPF_CHANGEROT = 16;
  static const int SPF_TWOCOL = 32;
  static const int SPF_EXTRASYM = 64;
  static const int SPF_ALTERNATE = 128;
  static const int SPF_FOOTBALL = 256;
  static const int SPF_FULLSYM = 512;
  static const int SPF_DOCKS = 1024;

  static const int SPF_SYM0123 = SPF_SYM01 | SPF_SYM02 | SPF_SYM03;
  
  extern char whichCanvas;

  extern bool displaycodes;

  int generateCanvas(cell *c);

  struct patterninfo {
    int id;
    int dir;
    bool reflect;
    int symmetries;
    };
    
  patterninfo getpatterninfo(cell *c, char pat, int sub);

  inline patterninfo getpatterninfo0(cell *c) {
    return getpatterninfo(c, whichPattern, subpattern_flags);
    }
  }

namespace mapeditor { 
#if CAP_EDIT
  extern map<int, cell*> modelcell;
#endif

  extern bool drawplayer; 
  void applyModelcell(cell *c);
  
  extern cell *drawcell;
  void initdraw(cell *c); 
  void showMapEditor();
  void showDrawEditor();
  }

namespace rug {
  extern bool rugged;
  extern bool renderonce;
  extern bool rendernogl;
  extern int  texturesize;
  extern ld   model_distance;
#if CAP_RUG
  void show();
  void init();
  void close();
  void actDraw();
  void select();
  void buildVertexInfo(cell *c, transmatrix V);
#endif
  }

#define HASLINEVIEW

namespace conformal {
  extern bool on;
  extern vector<pair<cell*, eMonster> > killhistory;
  extern vector<pair<cell*, eItem> > findhistory;  
  extern vector<cell*> movehistory;
  extern bool includeHistory;
  extern int rotation;
  extern bool autoband;
  extern bool autobandhistory;
  extern bool dospiral;
  extern ld lvspeed;
  extern int bandsegment;
  extern int bandhalf;  
  
  void create();
  void clear();
  void show();
  void apply();
  void movetophase();
  void renderAutoband();

  extern vector<shmup::monster*> v;
  extern double phase;
  void applyIB();
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

void selectEyeGL(int ed);
void selectEyeMask(int ed);
extern int ticks;
void setGLProjection();

// passable flags

#define P_MONSTER    Flag(0)  // can move through monsters
#define P_MIRROR     Flag(1)  // can move through mirrors
#define P_REVDIR     Flag(2)  // reverse direction movement
#define P_WIND       Flag(3)  // can move against the wind
#define P_GRAVITY    Flag(4)  // can move against the gravity
#define P_ISPLAYER   Flag(5)  // player-only moves (like the Round Table jump)
#define P_ONPLAYER   Flag(6)  // always can step on the player
#define P_FLYING     Flag(7)  // is flying
#define P_BULLET     Flag(8)  // bullet can fly through more things
#define P_MIRRORWALL Flag(9)  // mirror images go through mirror walls
#define P_JUMP1      Flag(10) // first part of a jump
#define P_JUMP2      Flag(11) // second part of a jump
#define P_TELE       Flag(12) // teleport onto
#define P_BLOW       Flag(13) // Orb of Air -- blow, or push
#define P_AETHER     Flag(14) // aethereal
#define P_FISH       Flag(15) // swimming
#define P_WINTER     Flag(16) // fire resistant
#define P_USEBOAT    Flag(17) // can use boat
#define P_NOAETHER   Flag(18) // disable AETHER
#define P_FRIENDSWAP Flag(19) // can move on friends (to swap with tem)
#define P_ISFRIEND   Flag(20) // is a friend (can use Empathy + Winter/Aether/Fish combo)
#define P_LEADER     Flag(21) // can push statues and use boats
#define P_MARKWATER  Flag(22) // mark Orb of Water as used
#define P_EARTHELEM  Flag(23) // Earth Elemental
#define P_WATERELEM  Flag(24) // Water Elemental
#define P_IGNORE37   Flag(25) // ignore the triheptagonal board
#define P_CHAIN      Flag(26) // for chaining moves with boats
#define P_DEADLY     Flag(27) // suicide moves allowed
#define P_ROSE       Flag(28) // rose smell
#define P_CLIMBUP    Flag(29) // allow climbing up
#define P_CLIMBDOWN  Flag(30) // allow climbing down
#define P_REPTILE    Flag(31) // is reptile
#define P_VOID       Flag(32) // void beast
#define P_PHASE      Flag(33) // phasing movement
#define P_PULLMAGNET Flag(34) // pull the other part of the magnet

bool passable(cell *w, cell *from, flagtype flags);

bool isElemental(eLand l);
int coastval(cell *c, eLand base);
int getHauntedDepth(cell *c);
eLand randomElementalLand();
bool notDippingForExtra(eItem i, eItem x);
void placePrizeOrb(cell *c);
void wandering();
bool isSealand(eLand l);
int newRoundTableRadius();
bool grailWasFound(cell *c);
extern bool buggyGeneration;
int buildIvy(cell *c, int children, int minleaf);
int celldistAltRelative(cell *c);
int roundTableRadius(cell *c);
cell *chosenDown(cell *c, int which, int bonus, cellfunction* cf = &coastvalEdge);
eLand pickLandRPM(eLand old);
bool bearsCamelot(eLand l);

extern bool safety;

#define SAGEMELT .1
#define TEMPLE_EACH 6
#define PT(x, y) ((tactic::on || quotient == 2) ? (y) : inv::on ? min(2*(y),x) : (x))
#define ROCKSNAKELENGTH 50
#define WORMLENGTH 15
#define PUREHARDCORE_LEVEL 10
#define PRIZEMUL 7

#define INF  9999
#define INFD 20
#define PINFD 125
#define BARLEV ((ISANDROID||ISIOS||ISFAKEMOBILE||getDistLimit()<7)?9:10)
#define BUGLEV 15
// #define BARLEV 9

bool isKillable(cell *c);
bool isKillableSomehow(cell *c);

bool isAlchAny(eWall w);
bool isAlchAny(cell *c);

#define YDIST 101
#define MODECODES 255
  
extern cellwalker cwt; // player character position
extern int sval;

extern int items[ittypes], kills[motypes], explore[10], exploreland[10][landtypes], landcount[landtypes];

typedef int modecode_t;
extern map<modecode_t, array<int, ittypes> > hiitems;

extern eLand firstland, specialland;
bool pseudohept(cell *c);
bool pureHardcore();
extern int cheater;
int airdist(cell *c);
bool eq(short a, short b);
extern vector<cell*> dcal;   // queue for cpdist
bool isPlayerOn(cell *c);
bool isFriendly(eMonster m);
bool isFriendly(cell *c);
bool isChild(cell *w, cell *killed); // is w killed if killed is killed?

static const int NO_TREASURE = 1;
static const int NO_YENDOR = 2;
static const int NO_GRAIL = 4;
static const int NO_LOVE = 8;

int gold(int no = 0);
int tkills();
bool hellUnlocked();

bool markOrb(eItem it); // mark the orb as 'used', return true if exists
bool markEmpathy(eItem it); // mark both the given orb and Empathy as 'used', return true if exists
bool markEmpathy2(eItem it); // as above, but next turn

bool isMimic(eMonster m);
bool isMimic(cell *c);

void fallMonster(cell *c, flagtype flags = 0); // kill monster due to terrain

bool attackMonster(cell *c, flagtype flags, eMonster killer);

bool isWorm(eMonster m);
bool isWorm(cell *c);
void empathyMove(cell *c, cell *cto, int dir);
bool isIvy(eMonster m);
bool isIvy(cell *c);

#define GUNRANGE 3

// 0 = basic treasure, 1 = other item, 2 = power orb, 3 = not an item
#define IC_TREASURE 0
#define IC_OTHER 1
#define IC_ORB 2
#define IC_NAI 3

bool playerInPower();
void activateFlash();
void activateLightning();
bool markOrb(eItem it);
bool markOrb2(eItem it);
void drainOrb(eItem it, int target = 0);
void useupOrb(eItem it, int qty);

void initgame();
bool haveRangedTarget();
eItem targetRangedOrb(cell *c, orbAction a);
void reduceOrbPowers();
int realstuntime(cell *c);

extern bool invismove, invisfish;
bool attackingForbidden(cell *c, cell *c2);
void killOrStunMonster(cell *c2, eMonster who_killed);

void useup(cell *c); // useup thumpers/bonfires
cell *playerpos(int i);

bool makeflame(cell *c, int timeout, bool checkonly);
void bfs();
bool isPlayerInBoatOn(cell *c);
bool isPlayerInBoatOn(cell *c, int i);
void destroyBoats(cell *c, cell *cf, bool strandedToo);
extern bool showoff;
extern int lastexplore;
extern int truelotus;
extern eLand lastland;
extern time_t timerstart;
extern bool timerstopped;
bool againstRose(cell *cfrom, cell *cto);
bool withRose(cell *cfrom, cell *cto);

// loops

#define fakecellloop(ct) for(cell *ct = (cell*)1; ct; ct=NULL)

#define forCellIdAll(ct, i, cf) fakecellloop(ct) for(int i=0; i<(cf)->type && (ct=(cf)->mov[i],true); i++) 
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

bool canAttack(cell *c1, eMonster m1, cell *c2, eMonster m2, flagtype flags);

extern bool chaosmode;
extern bool chaosUnlocked;
extern bool chaosAchieved;
bool isTechnicalLand(eLand l);
int getGhostcount();

void raiseBuggyGeneration(cell *c, const char *s);
void verifyMutantAround(cell *c);

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

void drawscreen();

void buildAirmap();

// currently works for worms only
bool sameMonster(cell *c1, cell *c2);
cell *wormhead(cell *c);
eMonster getMount(int player_id);
eMonster haveMount();

bool isDragon(eMonster m);

// for some reason I need this to compile under OSX

#if ISMAC
extern "C" { void *_Unwind_Resume = 0; }
#endif

extern bool autocheat;
extern bool inHighQual;

void mountmove(cell *c, int spin, bool fp);
void mountmove(cell *c, int spin, bool fp, cell *ppos);
void mountswap(cell *c1, int spin1, bool fp1, cell *c2, int spin2, bool fp2);

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

  bool anyKilled();
  bool isMoveto(cell *c);
  bool isKilledDirectlyAt(cell *c);
  bool isPushto(cell *c);
  };

extern int turncount;

bool reduceOrbPower(eItem it, int cap);
bool checkOrb(eMonster m1, eItem orb);

movedir vectodir(const hyperpoint& P);

namespace sword {

  extern int angle[MAXPLAYER];

  cell *pos(cell *c, int s);
  cell *pos(int id);
  bool at(cell *where, bool noplayer = false);
  int shift(cell *c1, cell *c2);
  }

void killThePlayer(eMonster m, int id, flagtype flags);
bool attackJustStuns(cell *c2, flagtype flags);

bool isTargetOrAdjacent(cell *c);
bool warningprotection();
bool mineMarked(cell *c);
bool minesafe();
bool hasSafeOrb(cell *c);
void placeWater(cell *c, cell *c2);
bool againstCurrent(cell *w, cell *from);

#define DEFAULTCONTROL (multi::players == 1 && !shmup::on && !multi::alwaysuse)
#define DEFAULTNOR(sym) (DEFAULTCONTROL || multi::notremapped(sym))

extern bool timerghost;

#define CAP_MENUSCALING (ISPANDORA || ISMOBILE)

#if CAP_MENUSCALING
#define displayfrZ dialog::displayzoom
#else
#define displayfrZ displayfr
#endif

namespace dialog {

  enum tDialogItem {diTitle, diItem, diBreak, diHelp, diInfo, diSlider, diBigItem};

  struct item {
    tDialogItem type;
    string body;
    string value;
    string keycaption;
    int key;
    int color, colorv, colork, colors, colorc;
    int scale;
    double param;
    int position;
    };

  extern vector<item> items;
  
  extern reaction_t reaction;

  item& lastItem();
  extern unsigned int *palette;
  
  void addSelItem(string body, string value, int key);
  void addBoolItem(string body, bool value, int key);
  void addColorItem(string body, int value, int key);
  void openColorDialog(unsigned int& col, unsigned int *pal = palette);
  void addHelp(string body);
  void addInfo(string body, int color = 0xC0C0C0);
  void addItem(string body, int key);
  int addBreak(int val);  
  void addTitle(string body, int color, int scale);
  
  void init();
  void init(string title, int color = 0xE8E8E8, int scale = 150, int brk = 60);
  void display();

  void editNumber(ld& x, ld vmin, ld vmax, ld step, ld dft, string title, string help);
  void editNumber(int& x, int vmin, int vmax, int step, int dft, string title, string help);
  void scaleLog();
  void scaleSinh();
  void handleNavigation(int &sym, int &uni);
  bool displayzoom(int x, int y, int b, int size, const string &s, int color, int align);
  bool editingDetail();

  int handlePage(int& nl, int& nlm, int perpage);
  void displayPageButtons(int i, bool pages);
  bool handlePageButtons(int uni);
  extern bool sidedialog;
  extern int dialogflags;
  extern int dcenter;
  int displaycolor(unsigned col);

  void openFileDialog(string& filename, string fcap, string ext, bool_reaction_t action);
  
  extern string infix;
  bool hasInfix(const string &s);
  bool editInfix(int uni);

  void vpush(int i, const char *name);
  extern vector<pair<string, int> > v;  
  }

void checkStunKill(cell *dest);

void clearMessages();

void resetGeometry();
extern bool nonchamfered;

namespace svg {
  void circle(int x, int y, int size, int col);
  void polygon(int *polyx, int *polyy, int polyi, int col, int outline, double minwidth);
  void text(int x, int y, int size, const string& str, bool frame, int col, int align);
  extern bool in;
  extern string *info;
  void render(const char *fname = NULL);
  }

extern int sightrange;

namespace halloween {
  void getTreat(cell *where);
  }

// just in case if I change my mind about when Orbs lose their power
#define ORBBASE 0

transmatrix mscale(const transmatrix& t, double fac);
transmatrix mzscale(const transmatrix& t, double fac);
extern bool ivoryz;
#define mmscale(V, x) (mmspatial ? (ivoryz ? mzscale(V,x) : mscale(V, x)) : (V))

#define SHADOW_WALL 0x60
#define SHADOW_SL   0x18
#define SHADOW_MON  0x30

bool drawMonsterType(eMonster m, cell *where, const transmatrix& V, int col, double footphase);
void drawPlayerEffects(const transmatrix& V, cell *c, bool onPlayer);

// monster movement animations

struct animation {
  int ltick;
  double footphase;
  transmatrix wherenow;
  };

// we need separate animation layers for Orb of Domination and Tentacle+Ghost,
// and also to mark Boats
#define ANIMLAYERS 3
#define LAYER_BIG   0 // for worms and krakens
#define LAYER_SMALL 1 // for others
#define LAYER_BOAT  2 // mark that a boat has moved

extern map<cell*, animation> animations[ANIMLAYERS];
extern unordered_map<cell*, transmatrix> gmatrix, gmatrix0;

void animateMovement(cell *src, cell *tgt, int layer);

// for animations which might use the same locations,
// such as replacements or multi-tile monsters
void indAnimateMovement(cell *src, cell *tgt, int layer);
void commitAnimations(int layer);

void animateReplacement(cell *a, cell *b, int layer);
void fallingFloorAnimation(cell *c, eWall w = waNone, eMonster m = moNone);
void fallingMonsterAnimation(cell *c, eMonster m, int id = multi::cpid);

// ranks:
enum PPR {
  PPR_ZERO, PPR_OUTCIRCLE, PPR_MOVESTAR,
  PPR_MINUSINF,
  PPR_BELOWBOTTOMm,
  PPR_BELOWBOTTOM,
  PPR_BELOWBOTTOMp,
  PPR_BELOWBOTTOM_FALLANIM,
  PPR_LAKEBOTTOM, PPR_HELLSPIKE,
  PPR_INLAKEWALLm, PPR_INLAKEWALL, PPR_INLAKEWALLp,
  PPR_INLAKEWALL_FALLANIM,
  PPR_SUBLAKELEV, PPR_LAKELEV, PPR_BOATLEV, PPR_BOATLEV2, PPR_BOATLEV3,
  PPR_LAKEWALLm, PPR_LAKEWALL, PPR_LAKEWALLp,
  PPR_LAKEWALL_FALLANIM,
  PPR_FLOOR_TOWER,
  PPR_FLOOR,
  PPR_FLOOR_DRAGON,
  PPR_FLOORa, PPR_FLOORb, PPR_FLOORc, PPR_FLOORd,
  PPR_LIZEYE,
  PPR_BFLOOR,
  PPR_GFLOORa, PPR_GFLOORb, PPR_GFLOORc,
  PPR_WALLSHADOW,
  PPR_STRUCT0, PPR_STRUCT1, PPR_STRUCT2, PPR_STRUCT3,
  PPR_THORNS, PPR_WALL,
  PPR_REDWALLm, PPR_REDWALLs, PPR_REDWALLp, PPR_REDWALL,
  PPR_REDWALLm2, PPR_REDWALLs2, PPR_REDWALLp2, PPR_REDWALLt2,
  PPR_REDWALLm3, PPR_REDWALLs3, PPR_REDWALLp3, PPR_REDWALLt3,
  PPR_HEPTAMARK,
  PPR_ITEM_BELOW,
  PPR_ITEM, PPR_ITEMa, PPR_ITEMb,
  PPR_BIGSTATUE,

  PPR_WALL3m, PPR_WALL3s, PPR_WALL3p, PPR_WALL3, PPR_WALL3A,

// PPR_WALL3m, PPR_WALL3s, PPR_WALL3p, PPR_WALL3, PPR_WALL3A,
  PPR_HIDDEN, PPR_GIANTSHADOW,
  PPR_TENTACLE0, PPR_TENTACLE1,
  PPR_ONTENTACLE, PPR_ONTENTACLE_EYES, PPR_ONTENTACLE_EYES2,
  PPR_MONSTER_SHADOW,
  PPR_MONSTER_FOOT, PPR_MONSTER_LEG, PPR_MONSTER_GROIN,
  PPR_MONSTER_SUBWPN, PPR_MONSTER_WPN,
  PPR_MONSTER_BODY, PPR_MONSTER_ARMOR0, PPR_MONSTER_ARMOR1,
  PPR_MONSTER_CLOAK, PPR_MONSTER_NECK,
  PPR_MONSTER_HEAD, PPR_MONSTER_FACE, PPR_MONSTER_EYE0, PPR_MONSTER_EYE1,
  PPR_MONSTER_HAIR, PPR_MONSTER_HAT0, PPR_MONSTER_HAT1,
  PPR_MONSTER_HOODCLOAK1, PPR_MONSTER_HOODCLOAK2,
  PPR_STUNSTARS,
  PPR_CARRIED, PPR_CARRIEDa, PPR_CARRIEDb,
  PPR_PARTICLE, PPR_SWORDMARK, PPR_MAGICSWORD, PPR_MISSILE, 
  PPR_MINEMARK, PPR_ARROW,
  PPR_MOBILE_ARROW,
  PPR_LINE, PPR_TEXT, PPR_CIRCLE,
  PPR_MAX
  };
  
void ShadowV(const transmatrix& V, const struct hpcshape& bp, int prio = PPR_MONSTER_SHADOW);

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

extern bool audio;
extern string musiclicense;
extern string musfname[landtypes];
extern int musicvolume, effvolume;
void initAudio();
bool loadMusicInfo();
void handlemusic();
void playSeenSound(cell *c);
void playSound(cell *c, const string& fname, int vol = 100);

inline string pick123() { return cts('1' + rand() % 3); }
inline string pick12() { return cts('1' + rand() % 2); }

bool playerInBoat(int i);

extern int lowfar;
extern bool wmspatial, wmescher, wmplain, wmblack, wmascii;
extern bool mmspatial, mmhigh, mmmon, mmitem;
extern int maxreclevel, reclevel;

string explain3D(ld *param);

extern int detaillevel;
extern bool quitmainloop;

enum eGlyphsortorder {
  gsoFirstTop, gsoFirstBottom,
  gsoLastTop, gsoLastBottom,
  gsoLand, gsoValue,
  gsoMAX
  };

extern eGlyphsortorder glyphsortorder;

#if CAP_ROGUEVIZ
namespace rogueviz { 
  extern bool on;
  string describe(shmup::monster *m);
  void describe(cell *c);
  void activate(shmup::monster *m);
  void drawVertex(const transmatrix &V, cell *c, shmup::monster *m);
  bool virt(shmup::monster *m);
  void turn(int delta);
  void drawExtra();
  void fixparam();
  int readArgs();
  void close();
  void mark(cell *c);
  void showMenu();
  string makehelp();
  }
#endif

void explodeMine(cell *c);
bool mayExplodeMine(cell *c, eMonster who);

int gravityLevel(cell *c);
void fullcenter();
void movecost(cell* from, cell *to);
void checkmove();

transmatrix eumove(ld x, ld y);
transmatrix eumove(int vec);
transmatrix eumovedir(int d);

int reptilemax();

extern bool mousing;
#define IFM(x) (mousing?"":x)

extern cell *recallCell;

extern eLand cheatdest;
void cheatMoveTo(eLand l);

extern unsigned backcolor, bordcolor, forecolor;

extern bool overgenerate;
void doOvergenerate();

void collectMessage(cell *c2, eItem which);

namespace quotientspace {
  void build();
  void clear();
  extern vector<int> connections;
  }

void killFriendlyIvy();

void pushdown(cell *c, int& q, const transmatrix &V, double down, bool rezoom, bool repriority);

extern bool viewdists;

void preventbarriers(cell *c);

bool passable_for(eMonster m, cell *w, cell *from, flagtype extra);

void beastcrash(cell *c, cell *beast);

int angledist(int t, int d1, int d2);
int angledist(cell *c, int d1, int d2);

void setcameraangle(bool b);

enum eModel {
  mdDisk, mdHalfplane, mdBand, mdPolygonal, mdPolynomial,
  mdEquidistant, mdEquiarea, mdBall, mdHyperboloid, mdGUARD, mdUnchanged };

#define MODELCOUNT ((int) mdGUARD)

void drawShape(pair<ld,ld>* coords, int qty, int color);

extern eModel pmodel;

inline bool mdEqui() { return pmodel == mdEquidistant || pmodel == mdEquiarea; }

int darkena(int c, int lev, int a);

#define SHSIZE 16

extern cell *shpos[MAXPLAYER][SHSIZE];
extern int cshpos;


namespace arg {
#if CAP_COMMANDLINE
  extern int argc; extern char **argv;
  
  inline void lshift() {
    argc--; argv++;
    }

  inline void shift() {
    lshift(); if(!argc) { printf("Missing parameter\n"); exit(1); }
    }

  inline char* args() { return *argv; }
  inline int argi() { return atoi(*argv); }
  inline ld argf() { return atof(*argv); }
  inline bool argis(const char *s) { return strcmp(*argv, s) == 0; }
  
  inline void init(int _argc, char **_argv) { argc=_argc-1; argv=_argv+1; }
  
  extern int curphase;
  
  inline void phaseerror(int x) {
    printf("Command line error: cannot read command '%s' from phase %d in phase %d\n", args(), x, curphase);
    exit(1);
    }
  
  // returned values: 0 = ok, 1 = not recognized, 2 = shift phase
  int readCommon();
  int readLocal();

// an useful macro
#define PHASE(x) { if(arg::curphase > x) phaseerror(x); else if(arg::curphase < x) return 2; }
#define PHASEFROM(x) { if(arg::curphase < x) return 2; }
  
  void read(int phase);
  
#endif
  }

extern bool generatingEquidistant;

void clearfrom(heptagon *at);
void clearHexes(heptagon *at);
void verifycells(heptagon *at);
int zebra40(cell *c);
cell *createMov(cell *c, int d);

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
  
  extern slide slideHypersian;
  extern slide slideExpansion;

  namespace ss {
    void showMenu();
    void list(slide*);
    }

  };
#endif

namespace rogueviz {
  extern bool rog3;
  extern bool rvwarp;
#if CAP_TOUR
  namespace rvtour {
    extern tour::slide rvslides[];
    }
#endif
  };

extern bool doCross;
void optimizeview();

extern int pngres;
extern int pngformat;

extern bool noGUI;
extern bool dronemode;

extern ld whatever;

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
  static const int A3 = 2048; // affects poly
  static const int ZOOMABLE = 4096;
  static const int TORUSCONFIG = 8192;
  static const int MAYDARK = 16384;
  static const int DIALOG_STRICT_X = 32768; // do not interpret dialog clicks outside of the X region
  };

namespace linepatterns {

  enum ePattern {
    patPalacelike,
    patPalace,
    patZebraTriangles,
    patZebraLines,
    patTriNet,
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
    patBigRings
    };
  
  void clearAll();
  void setColor(ePattern id, int col);
  void drawAll();
  void showMenu();
  };

transmatrix ddspin(cell *c, int d, int bonus = 0);
bool doexiton(int sym, int uni);
void switchFullscreen();
string turnstring(int i);
int celldistance(cell *c1, cell *c2);
bool behindsphere(const transmatrix& V);
extern hyperpoint pirateCoords;

bool mouseout();

bool againstWind(cell *c2, cell *c1); // to, from

transmatrix atscreenpos(ld x, ld y, ld size);

hyperpoint mirrorif(const hyperpoint& V, bool b);

#define SETMOUSEKEY 5000
extern char mousekey;
extern char newmousekey;
void displaymm(char c, int x, int y, int rad, int size, const string& title, int align);

bool canPushThumperOn(cell *tgt, cell *thumper, cell *player);
void pushThumper(cell *th, cell *cto);

template<class T, class... U> T pick(T x, U... u) { std::initializer_list<T> i = {x,u...}; return *(i.begin() + hrand(1+sizeof...(u))); }

template<class T, class V, class... U> bool among(T x, V y) { return x == y; }
template<class T, class V, class... U> bool among(T x, V y, U... u) { return x==y || among(x,u...); }

eLand getNewSealand(eLand old);
bool createOnSea(eLand old);

namespace inv {
  extern bool on;
  extern bool usedForbidden;
  extern int remaining[ittypes];
  void compute();
  void applyBox(eItem it);

  extern int incheck;  
  void check(int delta);
  void show();
  }

bool drawItemType(eItem it, cell *c, const transmatrix& V, int icol, int ticks, bool hidden);

void initquickqueue();
void quickqueue();
int darkenedby(int c, int lev);
extern int mousex, mousey;
string generateHelpForItem(eItem it);
bool graphglyph();
extern bool hiliteclick;
extern int antialiaslines;
extern int ringcolor;

#include <functional>

template<class T> class hookset : public map<int, function<T>> {};
typedef hookset<void()> *purehookset;

template<class T, class U> int addHook(hookset<T>*& m, int prio, const U& hook) {
  if(!m) m = new hookset<T> ();
  while(m->count(prio)) {
    prio++;
    }
  (*m)[prio] = hook;
  return 0;
  }

extern purehookset hooks_frame, hooks_stats, clearmemory, hooks_config, hooks_tests;

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

extern hookset<bool(int sym, int uni)> *hooks_handleKey;
extern hookset<void(cell *c, const transmatrix& V)> *hooks_drawcell;
extern hookset<bool(int argc, char** argv)> *hooks_main;
extern hookset<int()> *hooks_args;
extern hookset<eLand(eLand)> *hooks_nextland;

// hooks to extend HyperRogue with an external program
// start compilation from another file which defines EXTRA_..., includes
// hyper.cpp, then defines the necessary functions

extern ld shiftmul;
void initcs(charstyle &cs);
charstyle& getcs(int id = multi::cpid);

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

extern vector<msginfo> msgs;
void flashMessages();

extern int lightat, safetyat;

int watercolor(int phase);
bool doHighlight();
string buildHelpText();
string buildCredits();
void setAppropriateOverview();
bool quitsaves();
extern bool sidescreen;

extern const char* COLORBAR;

int textwidth(int siz, const string &str);
#define GLERR(call) glError(call, __FILE__, __LINE__)

extern bool gtouched, mousepressed, mousemoved, actonrelease;
extern bool inslider;

#if CAP_ROGUEVIZ
#define DOSHMUP (shmup::on || rogueviz::on)
#else
#define DOSHMUP shmup::on
#endif

extern bool outoffocus;
extern int frames;
extern transmatrix playerV;
extern bool didsomething;
extern void drawStats();
extern int calcfps();
extern int distcolors[8];

extern eItem orbToTarget;
extern eMonster monsterToSummon;

void panning(hyperpoint hf, hyperpoint ht);
extern transmatrix sphereflip;

void initConfig();
void loadConfig();

extern bool auraNOGL;

#if CAP_SDLJOY
extern void initJoysticks();
extern bool autojoy;
extern int joyx, joyy, panjoyx, panjoyy;
extern movedir joydir;
extern SDL_Joystick* sticks[8];
extern int numsticks;
void closeJoysticks();
#endif

void preparesort();

#if ISMOBILE==1
#define SHMUPTITLE "shoot'em up mode"
#else
#define SHMUPTITLE "shoot'em up and multiplayer"
#endif

bool dodrawcell(cell *c);
void drawcell(cell *c, transmatrix V, int spinv, bool mirrored);
extern double downspin;

extern int frameid;
extern bool leftclick;
void clearMemory();

extern function <void(int sym, int uni)> keyhandler;
void gmodekeys(int sym, int uni);

void switchGL();
void switchFullscreen();
extern int cmode;

namespace scores { void load(); }

void gotoHelp(const string& h);
void showCustomizeChar();
void showCheatMenu();
void showDisplayMode();
void showChangeMode();
void showEuclideanMenu();
void show3D();
void gameoverscreen();
void showJoyConfig();

void gamescreen(int darken);
void showMission();
void handleKeyQuit(int sym, int uni);
void handlePanning(int sym, int uni);

#if ISMOBILE==1
namespace leader { void showMenu(); void handleKey(int sym, int uni); }
#endif

bool needConfirmation();

namespace mirror {
  cellwalker reflect(const cellwalker& cw);
  }

bool inmirror(eLand l);
bool inmirror(cell *c);
bool inmirror(const cellwalker& cw);

void queuemarkerat(const transmatrix& V, int col);

void check_total_victory();
void applyBoxNum(int& i, string name = "");
extern int hinttoshow;

bool isShmupLifeOrb(eItem it);
int orbcharges(eItem it);

int gradient(int c0, int c1, ld v0, ld v, ld v1);

struct hint {
  time_t last;
  function<bool()> usable;
  function<void()> display;
  function<void()> action;  
  };

extern hint hints[];
int counthints();

void gainShard(cell *c2, const char *msg);

int textwidth(int siz, const string &str);
#if CAP_GL
int gl_width(int size, const char *s);
#endif

#ifdef ISMOBILE
extern int andmode;
#endif

void addaura(const hyperpoint& h, int col, int fd);
void addauraspecial(const hyperpoint& h, int col, int dir);

void drawBug(const cellwalker& cw, int col);

void mainloop();
void mainloopiter();
extern bool showstartmenu;
void selectLanguageScreen();

bool inscreenrange(cell *c);
bool allowIncreasedSight();

static inline bool orbProtection(eItem it) { return false; } // not implemented

namespace windmap {
  void create();

  static const int NOWINDBELOW = 8;
  static const int NOWINDFROM = 120;

  int getId(cell *c);
  int at(cell *c);
  }

extern int wavephase;

void buildEquidistant(cell *c);
void produceGhost(cell *c, eMonster victim, eMonster who);
void sideAttack(cell *mf, int dir, eMonster who, int bonus, eItem orb);
void sideAttack(cell *mf, int dir, eMonster who, int bonuskill);
void warpfloor(cell *c, const transmatrix& V, int col, int prio, bool warp);

void orboflava(int i);

void setland(cell *c, eLand l);

eLand getNewLand(eLand old);

extern bool randomPatternsMode;

extern int isRandland(eLand l);

extern vector<cell*> buggycells;

extern bool landUnlocked(eLand l);

extern void describeCell(cell*);
extern bool rlyehComplete();

extern int steplimit, cstep;

template<class... T>
void limitgen(T... args) {
  if(steplimit) {
    cstep++;
    printf("%6d ", cstep);
    printf(args...);
    if(cstep == steplimit) buggyGeneration = true;
    }
  }

eLand oppositeElement(eLand l, eLand l2);

extern int hardness_empty();
extern eWall getElementalWall(eLand l);

void gainItem(eItem it);

void destroyTrapsOn(cell *c);
void destroyTrapsAround(cell *c);

extern int messagelogpos;

void showMessageLog();

int getgametime();
string getgametime_s(int timespent = getgametime());
extern int stampbase;

transmatrix cellrelmatrix(cell *c, int i);

void terracottaAround(cell *c);

double cellgfxdist(cell *c, int i);

int ctof(cell *c);

void modalDebug(cell *c);
int getDistLimit();

void drawqueue();

#ifndef GL
typedef float GLfloat;
#endif

struct textureinfo {
  transmatrix M;
  int texture_id;
  vector<GLfloat> vertices;
  vector<GLfloat> tvertices; 
  cell *c;
  vector<transmatrix> matrices;
  
  // these are required to adjust to geometry changes
  int current_type, symmetries;
  };

struct qpoly {
      transmatrix V;
      GLfloat *tab;
      int curveindex;
      int cnt;
      int outline;
      double minwidth;
      int flags;
      textureinfo *tinf;
      };

struct qline {
      hyperpoint H1, H2;
      int prf;
      double width;
      };
      
#define MAXQCHR 64

struct qchr {
      char str[MAXQCHR];
      int x, y, shift, size, frame;
      int align;
      };
      
struct qcir {
      int x, y, size;
      };

enum eKind { pkPoly, pkLine, pkString, pkCircle, pkShape, pkResetModel, pkSpecial };

struct polytodraw {
  eKind kind;
  int prio, col;
  union {
    qpoly  poly;
    qline  line;
    qchr   chr;
    qcir   cir;
    double dvalue;
    } u;
#if CAP_ROGUEVIZ
  string* info;
  polytodraw() { info = NULL; }
#endif
  };

extern int emeraldtable[100][7];

extern void cwspin(cellwalker& cw, int d);
extern cell *cwpeek(cellwalker cw, int dir);
extern void cwstep(cellwalker& cw);
extern void cwrevstep(cellwalker& cw);
extern void cwrev(cellwalker& cw);

const eLand NOWALLSEP = laNone;
const eLand NOWALLSEP_USED = laWhirlpool;

bool hasbardir(cell *c);

bool buildBarrierNowall(cell *c, eLand l2, bool force = false);
bool checkBarriersBack(cellwalker bb, int q=5, bool cross = false);
bool checkBarriersFront(cellwalker bb, int q=5, bool cross = false);

bool quickfind(eLand l);
void beCIsland(cell *c);
bool isOnCIsland(cell *c);
void generateTreasureIsland(cell *c);
bool openplains(cell *c);
void buildBigStuff(cell *c, cell *from);
void setLandQuotient(cell *c);
void setLandSphere(cell *c);
void setLandWeird(cell *c);
void moreBigStuff(cell *c);
void setLandEuclid(cell *c);
bool checkInTree(cell *c, int maxv);
cell *findcompass(cell *c);
int edgeDepth(cell *c);
int compassDist(cell *c);

#define HAUNTED_RADIUS getDistLimit()
#define UNKNOWN 65535

#if CAP_COMMANDLINE
extern const char *scorefile;
extern string levelfile;
extern string picfile;
extern const char *conffile;
extern const char *musicfile;
#endif

extern string s0;
extern int anthraxBonus;
int celldistAlt(cell *c);
int celldist(cell *c);
int getHemisphere(cell *c, int which);

namespace tactic {
  extern bool on;
  extern bool trailer;
  extern eLand lasttactic;
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

int eudist(short sx, short sy);

heptagon *createStep(heptagon *h, int d);

cell *createMovR(cell *c, int d);

bool ishept(cell *c);
int cdist50(cell *c);
int polarb50(cell *c);

bool isGravityLand(eLand l);
bool isWarped(eLand l);
bool isWarped(cell *c);

struct hrmap {
  virtual heptagon *getOrigin() { return NULL; }
  virtual cell *gamestart() { return getOrigin()->c7; }
  virtual ~hrmap() { };
  virtual vector<cell*>& allcells() { return dcal; }
  virtual void verify() { }
  };

struct hrmap_hyperbolic : hrmap {
  heptagon *origin;
  bool isnonchamfered;
  hrmap_hyperbolic();
  heptagon *getOrigin() { return origin; }
  ~hrmap_hyperbolic() {
    DEBMEM ( verifycells(origin); )
    // printf("Deleting hyperbolic map: %p\n", this);
    dynamicval<bool> ph(nonchamfered, isnonchamfered);
    clearfrom(origin);
    }
  void verify() { verifycells(origin); }
  };

extern hrmap *currentmap;
extern vector<hrmap*> allmaps;

// list all cells in distance at most maxdist, or until when maxcount cells are reached

struct celllister {
  vector<cell*> lst;
  vector<int> tmps;
  vector<int> dists;
  
  void add(cell *c, int d) {
    if(eq(c->aitmp, sval)) return;
    c->aitmp = sval;
    tmps.push_back(c->aitmp);
    lst.push_back(c);
    dists.push_back(d);
    }
  
  ~celllister() {
    for(int i=0; i<size(lst); i++) lst[i]->aitmp = tmps[i];
    }
  
  celllister(cell *orig, int maxdist, int maxcount, cell *breakon) {
    lst.clear();
    tmps.clear();
    dists.clear();
    sval++;
    add(orig, 0);
    cell *last = orig;
    for(int i=0; i<size(lst); i++) {
      cell *c = lst[i];
      if(maxdist) forCellCM(c2, c) {
        add(c2, dists[i]+1);
        if(c2 == breakon) return;
        }
      if(c == last) {
        if(size(lst) >= maxcount || dists[i]+1 == maxdist) break;
        last = lst[size(lst)-1];
        }
      }
    }

  void prepare() {
    for(int i=0; i<size(lst); i++) lst[i]->aitmp = i;
    }
  
  int getdist(cell *c) { return dists[c->aitmp]; }
  
  bool listed(cell *c) {
    return c->aitmp >= 0 && c->aitmp < size(lst) && lst[c->aitmp] == c;
    }
  
  };

hrmap *newAltMap(heptagon *o);

#define currfp fieldpattern::getcurrfp()
namespace fieldpattern {
  struct fpattern& getcurrfp();
  }

int currfp_gmul(int a, int b);
int currfp_inverses(int i);
int currfp_distwall(int i);

const char *dnameof(eMonster m);
const char *dnameof(eLand l);
const char *dnameof(eWall w);
const char *dnameof(eItem i);

void runGeometryExperiments();

// z to close to this limit => do not draw

#define BEHIND_LIMIT 1e-6

extern vector<eLand> landlist;
template<class T> void generateLandList(T t);
int isLandValid(eLand l);
bool isLandValid2(eLand l);

bool inmirrororwall(eLand l);
extern bool holdmouse;

// what part of the compass does 'skip turn'
static const auto SKIPFAC = .4;

bool haveMobileCompass();
bool handleCompass();

inline bool sphereflipped() { return sphere && vid.alpha > 1.1; }
int cellcolor(cell *c);
transmatrix screenpos(ld x, ld y);
extern ld backbrightness;

void initcells();
void precalc();
extern const hyperpoint C0;
extern const transmatrix Id;

extern long long circlesize[100], disksize[100];
extern ld circlesizeD[10000];
void computeSizes();

#if CAP_FILES
extern const char *scorefile;
extern const char *conffile;
extern string levelfile;
extern string picfile;
extern const char *musicfile;
extern const char *loadlevel;
#endif

transmatrix spin(ld alpha);
transmatrix xpush(ld alpha);
transmatrix inverse(const transmatrix&);
ld hdist(const hyperpoint& h1, const hyperpoint& h2);

extern bool fixseed;
extern eLand firstland0;
extern int startseed;


extern transmatrix heptmove[MAX_EDGE], hexmove[MAX_EDGE];
extern transmatrix invheptmove[MAX_EDGE], invhexmove[MAX_EDGE];

heptspin hsstep(const heptspin &hs, int spin);

extern void fixmatrix(transmatrix&);
void display(const transmatrix& T);
transmatrix rgpushxto0(const hyperpoint& H);
char *display(const hyperpoint& H);

string its(int i);

double hdist0(const hyperpoint& mh);

extern bool fading;
extern ld fadeout;
int itemclass(eItem i);

extern purehookset hooks_drawmap;
extern hookset<bool(eLand&)> *hooks_music;
extern hookset<bool()> *hooks_prestats;
extern purehookset hooks_fixticks;
void activateVertexArray(GLfloat *f, int qty);

ld realradius();

void sdltogl(SDL_Surface *txt, struct glfont_t& f, int ch);
void glcolor2(int color);

void showStartMenu();

bool polara50(int x);
int polara50(cell *c);
int fiftyval049(cell *c);

namespace fieldpattern {
  pair<int, bool> fieldval(cell *c);
  }

int emeraldval(cell *c);

int inpair(cell *c, int colorpair);
int snake_pair(cell *c);

extern const unsigned int nestcolors[8];

#if CAP_TEXTURE
namespace texture {
  enum eTextureState {
    tsOff, tsAdjusting, tsActive
    };
  
  extern eTextureState tstate, tstate_max;
  extern unsigned paint_color;
  extern ld penwidth;
  extern transmatrix itt;  
  extern int twidth;
  extern bool saving;
  
  extern vector<unsigned> texture_pixels;
  
  void showMenu();
  
  void update();
  void drawPixel(cell *c, hyperpoint h, int col);

  extern cell *where;
  // compute 'c' automatically, based on the hint in 'where'
  void drawPixel(hyperpoint h, int col);
  void drawLine(hyperpoint h1, hyperpoint h2, int col, int steps = 10);
  
  void remap(eTextureState old_tstate, eTextureState old_tstate_max);
  
  void perform_mapping();
  
  void undoLock();
  void undo();
  extern bool texturesym;
  

  extern cpatterntype cgroup;
  }
#endif

void queueline(const hyperpoint& H1, const hyperpoint& H2, int col, int prf = 0, int prio = PPR_LINE);

hyperpoint ddi0(ld dir, ld dist);
extern ld tessf, crossf, hexf, hcrossf, hexhexdist, hexvdist, hepvdist, rhexf;
unsigned char& part(int& col, int i);

transmatrix applyPatterndir(cell *c, const patterns::patterninfo& si);

int pattern_threecolor(cell *c);
int fiftyval200(cell *c);

// T * C0, optimized
inline hyperpoint tC0(const transmatrix &T) {
  hyperpoint z;
  z[0] = T[0][2]; z[1] = T[1][2]; z[2] = T[2][2];
  return z;
  }

transmatrix actualV(const heptspin& hs, const transmatrix& V);
transmatrix cview();

extern string chamfernames[2];
extern bool need_mouseh;

extern int whateveri, whateveri2;

void clear_euland(eLand first);

extern eMonster passive_switch;

bool cannotPickupItem(cell *c, bool telekinesis);
bool canPickupItemWithMagnetism(cell *c, cell *from);
void pickupMovedItems(cell *c);
