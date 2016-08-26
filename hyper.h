// definitions

// disable this if you have no access to SDL_gfx
#ifndef STEAM
#define GFX
#endif

#define GL

#ifdef NOGFX
#undef GFX
#endif

#ifdef MAC
#define NOPNG
#endif

// scale the Euclidean
#define EUCSCALE 2.3

// disable this if you have no access to SDL_mixer
#ifndef MOBILE
#define AUDIO
#endif

#define NUMWITCH 7

// achievements

#define LB_YENDOR_CHALLENGE 40
#define LB_PURE_TACTICS 41
#define NUMLEADER 57
#define LB_PURE_TACTICS_SHMUP 49
#define LB_PURE_TACTICS_COOP 50

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
vector<string> achievementsReceived;

// game forward declarations

bool mirrorkill(cell *c);
bool isNeighbor(cell *c1, cell *c2);
void checkTide(cell *c);
namespace anticheat { extern bool tampered; }
int numplayers();
void removeIvy(cell *c);
bool cellEdgeUnstable(cell *c);
int coastvalEdge(cell *c);
typedef int cellfunction(cell*);
int towerval(cell *c, cellfunction* cf = &coastvalEdge);
#define HRANDMAX 0x7FFFFFFF
int hrandpos(); // 0 to HRANDMAX
void restartGame(char switchWhat = 0);
int landMultiplier(eLand l);
eItem treasureType(eLand l);
void buildBarrier(cell *c, int d, eLand l = laNone);
void extendBarrier(cell *c);
bool buildBarrier4(cell *c, int d, int mode, eLand ll, eLand lr);
void makeEmpty(cell *c);
bool isCrossroads(eLand l);
enum orbAction { roMouse, roKeyboard, roCheck, roMouseForce };
void moveItem (cell *from, cell *to, bool activateYendor);
void uncoverMines(cell *c, int lev);
bool survivesMine(eMonster m);
void killMonster(cell *c);
void toggleGates(cell *ct, eWall type, int rad);
bool destroyHalfvine(cell *c, eWall newwall = waNone, int tval = 6);
void buildCrossroads2(cell *c);
bool isHaunted(eLand l);
heptagon *createAlternateMap(cell *c, int rad, hstate firststate, int special=0);
void generateAlts(heptagon *h);
void setdist(cell *c, int d, cell *from);
void checkOnYendorPath();
void killThePlayerAt(eMonster m, cell *c, int flags);
bool notDippingFor(eItem i);
bool collectItem(cell *c2, bool telekinesis = false);
void castLightningBolt(struct cellwalker lig);
bool movepcto(int d, int subdir = 1, bool checkonly = false);
void stabbingAttack(cell *mf, cell *mt, eMonster who);
bool earthMove(cell *from, int dir);
void messageKill(eMonster killer, eMonster victim);
void moveMonster(cell *ct, cell *cf);
int palaceHP();
void placeLocalOrbs(cell *c);
int elementalKills();
bool elementalUnlocked();
bool isMultitile(eMonster m);
void checkFreedom(cell *cf);
int rosedist(cell *c);
bool canPushStatueOn(cell *c);

namespace hive { void createBugArmy(cell *c); }
namespace whirlpool { void generate(cell *wto); }
namespace whirlwind { void generate(cell *wto); }
namespace mirror { 
  void createMirrors(cell *c, int dir, eMonster type);
  void createMirages(cell *c, int dir, eMonster type);
  }

int neighborId(cell *c1, cell *c2);

struct movedir { int d; int subdir; };
inline bool movepcto(const movedir& md) { return movepcto(md.d, md.subdir); }

void activateActiv(cell *c, bool msg);

// shmup

struct charstyle {
  int charid, skincolor, haircolor, dresscolor, swordcolor, dresscolor2;
  };

string csname(charstyle& cs);
void initcs(charstyle& cs);
void savecs(FILE *f, charstyle& cs);
void loadcs(FILE *f, charstyle& cs);

namespace shmup {
  extern bool on;
  extern bool safety;
  extern int curtime;
  extern int players, cpid;
  void clearMemory();
  void init();
  void teleported();
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
  
#define MAXBUTTON 64
#define MAXAXE 16
#define MAXHAT 4

  struct config {
    int players;
    int subconfig;
    int setwhat;
    char keyaction[512];
    char joyaction[8][MAXBUTTON];
    char axeaction[8][MAXAXE];
    char hataction[8][MAXHAT][4];
    };
  
  charstyle scs[4];
  }

// graph

extern int msgscroll;

void showMissionScreen();

void restartGraph();
void resetmusic();

void cleargraphmemory();

void drawFlash(cell* c);
void drawBigFlash(cell* c);

void drawLightning();
void drawSafety();
void restartGraph();
void movepckeydir(int);

void centerpc(ld aspd);

void displayStatHelp(int y, string name);
void displayStat(int y, const string& name, const string& val, char mkey);
void displayButton(int x, int y, const string& name, int key, int align, int rad = 0);
void displayColorButton(int x, int y, const string& name, int key, int align, int rad, int color, int color2 = 0);
inline string ONOFF(bool b) { return XLAT(b ? "ON" : "OFF"); }
int darkened(int c);
extern int getcstat;
bool displaychr(int x, int y, int shift, int size, char chr, int col);
bool displayfr(int x, int y, int b, int size, const string &s, int color, int align);
void saveHighQualityShot();

bool outofmap(hyperpoint h);
void getcoord(const hyperpoint& H, int& x, int& y, int &shift);
void drawline(const hyperpoint& H1, int x1, int y1, int s1, const hyperpoint& H2, int x2, int y2, int col);
void drawline(const hyperpoint& H1, const hyperpoint& H2, int col);
void drawCircle(int x, int y, int size, int color);
void fixcolor(int& col);
int displaydir(cell *c, int d);
hyperpoint gethyper(ld x, ld y);
void resetview(); extern cell *lcenterover; extern heptspin viewctr;
#ifndef MOBILE
int& qpixel(SDL_Surface *surf, int x, int y);
#endif
void drawthemap();
void drawfullmap();
bool displaystr(int x, int y, int shift, int size, const char *str, int color, int align);

extern int darken;
void setvideomode();
void calcparam();

string ifMousing(string key, string s);

void saveConfig();

extern hyperpoint mouseh;

extern int webdisplay;

extern bool GL_initialized;
extern hyperpoint ccenter;
extern ld crad;

extern bool mousepressed, anyshiftclick;
extern string help;

extern int lalpha;

struct videopar {
  ld scale, eye, alpha, aspeed;
  bool full;
  bool goteyes;  // for rendering
  bool goteyes2; // for choosing colors
  bool quick;
  bool darkhepta;
  bool shifttarget;
  
  int xres, yres, framelimit;
  
  int xscr, yscr;
  
  // paramaters calculated from the above
  int xcenter, ycenter;
  int radius;
  ld alphax, beta;
  
  int fsize;
  int flashtime;
  
  int wallmode, monmode, axes;

  // for OpenGL
  float scrdist;
  
  bool usingGL;
  bool usingAA;

  int joyvalue, joyvalue2, joypanthreshold;
  float joypanspeed;
  
  charstyle cs;
  
  bool samegender; // same gender for the Princess?
  int language;
  
  int killreduction, itemreduction, portreduction;
  
  shmup::config scfg;
  
  bool steamscore;
  };

extern videopar vid;

enum emtype {emNormal, emHelp, 
  emMenu,
  emVisual1, emVisual2, 
  emChangeMode, emCustomizeChar,
  emQuit, emDraw, emScores, emPickEuclidean, 
  emPickScores, 
  emShmupConfig,
  emMapEditor,
  emPatternPicker,
  emOverview,
  emNetgen,
  emYendor, emTactic, emRugConfig,
  emConformal,
  emProgress,
  emCheatMenu
  };
 
extern emtype cmode, lastmode;

extern transmatrix View; // current rotation, relative to viewctr
extern transmatrix cwtV; // player-relative view

extern cell *mouseover, *mouseover2;
extern string mouseovers;

extern struct SDL_Surface *s;

namespace mapeditor { 
  extern bool drawplayer; 
  extern char whichPattern, whichShape;
  int generateCanvas(cell *c);
  void clearModelCells();
  void applyModelcell(cell *c);
  int realpattern(cell *c);
  int patterndir(cell *c, char w = whichPattern);
  extern cell *drawcell;
  }

namespace rug {
  extern bool rugged;
  void init();
  void close();
  void actDraw();
  void buildVertexInfo(cell *c, transmatrix V);
  }

#define HASLINEVIEW
namespace conformal {
  extern bool on;
  extern vector<pair<cell*, eMonster> > killhistory;
  extern vector<pair<cell*, eItem> > findhistory;  
  extern vector<cell*> movehistory;
  extern bool includeHistory;
  
  void create();
  void clear();
  void handleKey();
  void show();
  void apply();
  void renderAutoband();
  }
  
namespace polygonal {
  extern int SI;
  extern double STAR;
  void solve();
  typedef long double ld;
  pair<ld, ld> compute(ld x, ld y);
  }

void selectEyeGL(int ed);
void selectEyeMask(int ed);
extern int ticks;
void setGLProjection();

#ifdef LOCAL
extern void process_local_stats();
bool localDescribe();
void localDrawMap();
extern bool localKill(shmup::monster *m);
#endif

// passable flags

#define P_MONSTER    (1<<0)  // can move through monsters
#define P_MIRROR     (1<<1)  // can move through mirrors
#define P_REVDIR     (1<<2)  // reverse direction movement
#define P_WIND       (1<<3)  // can move against the wind
#define P_GRAVITY    (1<<4)  // can move against the gravity
#define P_ISPLAYER   (1<<5)  // player-only moves (like the Round Table jump)
#define P_ONPLAYER   (1<<6)  // always can step on the player
#define P_FLYING     (1<<7)  // is flying
#define P_BULLET     (1<<8)  // bullet can fly through more things
#define P_JUMP1      (1<<10) // first part of a jump
#define P_JUMP2      (1<<11) // second part of a jump
#define P_TELE       (1<<12) // teleport onto
#define P_BLOW       (1<<13) // Orb of Air -- blow, or push
#define P_AETHER     (1<<14) // aethereal
#define P_FISH       (1<<15) // swimming
#define P_WINTER     (1<<16) // fire resistant
#define P_USEBOAT    (1<<17) // can use boat
#define P_NOAETHER   (1<<18) // disable AETHER
#define P_FRIENDSWAP (1<<19) // can move on friends (to swap with tem)
#define P_ISFRIEND   (1<<20) // is a friend (can use Empathy + Winter/Aether/Fish combo)
#define P_LEADER     (1<<21) // can push statues and use boats
#define P_MARKWATER  (1<<22) // mark Orb of Water as used
#define P_EARTHELEM  (1<<23) // Earth Elemental
#define P_WATERELEM  (1<<24) // Water Elemental
#define P_IGNORE37   (1<<25) // ignore the triheptagonal board
#define P_CHAIN      (1<<26) // for chaining moves with boats
#define P_DEADLY     (1<<27) // suicide moves allowed
#define P_ROSE       (1<<28) // rose smell
#define P_CLIMBUP    (1<<29) // allow climbing up
#define P_CLIMBDOWN  (1<<30) // allow climbing down

bool passable(cell *w, cell *from, int flags);

bool isElemental(eLand l);
int coastval(cell *c, eLand base);
int getHauntedDepth(cell *c);
eLand randomElementalLand();
extern eLand euland[65536];
bool notDippingForExtra(eItem i, eItem x);
void placePrizeOrb(cell *c);
int hivehard();
eMonster randomHyperbug();
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
#define PT(x, y) (tactic::on ? (y) : (x))
#define ROCKSNAKELENGTH 50
#define PUREHARDCORE_LEVEL 10
#define PRIZEMUL 7

#define INF  9999
#define INFD 20
#define BARLEV ((ISANDROID||ISIOS||purehepta)?9:10)
#define BUGLEV 15
// #define BARLEV 9

bool isKillable(cell *c);
bool isKillableSomehow(cell *c);

extern vector<cell*> mirrors, mirrors2;
bool isAlchAny(eWall w);
bool isAlchAny(cell *c);

#define YDIST 101
#define MODECODES 38
  
extern cellwalker cwt; // player character position
extern int sval;

extern int items[ittypes], hiitems[MODECODES][ittypes], kills[motypes], explore[10], exploreland[10][landtypes], landcount[landtypes];

extern eLand firstland, euclidland;
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

int gold();
int tkills();
bool hellUnlocked();

bool markOrb(eItem it); // mark the orb as 'used', return true if exists
bool markEmpathy(eItem it); // mark both the given orb and Empathy as 'used', return true if exists

bool isMimic(eMonster m);
bool isMimic(cell *c);
void killWithMessage(cell *c, bool orStun = false, eMonster killer = moNone);

bool isWorm(eMonster m);
bool isWorm(cell *c);
void empathyMove(cell *c, cell *cto, int dir);
bool isIvy(eMonster m);
bool isIvy(cell *c);

#define GUNRANGE 3

// 0 = basic treasure, 1 = something else, 2 = power orb
#define IC_TREASURE 0
#define IC_OTHER 1
#define IC_ORB 2

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
void killOrStunMonster(cell *c2);

extern vector<cell*> offscreen; // offscreen cells to take care off

void useup(cell *c); // useup thumpers/bonfires
cell *playerpos(int i);

bool makeflame(cell *c, int timeout, bool checkonly);
void bfs();
bool isPlayerInBoatOn(cell *c);
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

#define AF_TOUGH             (1<<0)    // tough attacks: Hyperbugs
#define AF_MAGIC             (1<<1)    // magical attacks: Flash
#define AF_STAB              (1<<2)    // stabbing attacks (usually ignored except Hedgehogs)
#define AF_LANCE             (1<<3)    // lance attacks (used by Lancers)
#define AF_ONLY_ENEMY        (1<<4)    // only say YES if it is an enemy
#define AF_ONLY_FRIEND       (1<<5)    // only say YES if it is a friend
#define AF_ONLY_FBUG         (1<<6)    // only say YES if it is a bug_or friend
#define AF_BACK              (1<<7)    // backward attacks (ignored except Viziers and Flailers)
#define AF_APPROACH          (1<<8)    // approach attacks (ignored except Lancers)
#define AF_IGNORE_UNARMED    (1<<9)    // ignore the UNARMED flag
#define AF_NOSHIELD          (1<<10)   // ignore the shielded status
#define AF_GETPLAYER         (1<<11)   // check for player (replace m2 with moPlayer for player position)
#define AF_GUN               (1<<12)   // revolver attack
#define AF_FAST              (1<<13)   // fast attack
#define AF_EAT               (1<<17)   // eating attacks from Worm-likes

#define MF_NOATTACKS         (1<<14)   // don't do any attacks
#define MF_PATHDIST          (1<<15)   // consider pathdist for moveval
#define MF_ONLYEAGLE         (1<<16)   // do this only for Eagles
#define MF_MOUNT             (1<<18)   // don't do 
#define MF_NOFRIEND          (1<<19)   // don't do it for friends

bool canAttack(cell *c1, eMonster m1, cell *c2, eMonster m2, int flags);

extern bool chaosmode;
extern bool chaosUnlocked;
extern bool chaosAchieved;
bool isTechnicalLand(eLand l);
int getGhostcount();

void raiseBuggyGeneration(cell *c, const char *s);
void verifyMutantAround(cell *c);

extern FILE *debugfile;
extern int debugflags;
extern bool offline;

#ifdef ANDROID
#define DEBB(r,x)
#else
#define DEBB(r,x) { if(debugfile && (!(r) || (debugflags & (r)))) { fprintf x; fflush(debugfile); } }
#endif

#define DF_INIT              0 // always display these
#define DF_MSG               0 // always display these
#define DF_STEAM             1
#define DF_GRAPH             2
#define DF_TURN              4

#ifdef MOBILE
#define NOPNG
#endif

// #define NOPNG

#ifdef NOPNG
#define IMAGEEXT ".bmp"
#define IMAGESAVE SDL_SaveBMP
#else
#include "savepng.h"
#define IMAGEEXT ".png"

void IMAGESAVE(SDL_Surface *s, const char *fname);
#endif

void drawscreen();

void buildAirmap();

// currently works for worms only
bool sameMonster(cell *c1, cell *c2);
cell *wormhead(cell *c);
eMonster getMount();

bool isDragon(eMonster m);

// for some reason I need this to compile under OSX

#ifdef MAC
extern "C" { void *_Unwind_Resume = 0; }
#endif

extern bool autocheat;
extern bool inHighQual;

void mountmove(cell *c, int spin, bool fp);

template<class T> struct dynamicval {
  T& where;
  T backup;
  dynamicval(T& wh, T val) : where(wh) { backup = wh; wh = val; }
  ~dynamicval() { where = backup; }
  };

namespace stalemate {
  eMonster who;
  cell *moveto;
  cell *killed;
  cell *pushto;
  cell *comefrom;
  bool  nextturn;
  
  bool isKilled(cell *c);
  };

extern int turncount;

bool reduceOrbPower(eItem it, int cap);
bool checkOrb(eMonster m1, eItem orb);
