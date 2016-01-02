// definitions

// disable this if you have no access to SDL_gfx
#ifndef STEAM
#define GFX
#endif

#define GL

#ifdef NOGFX
#undef GFX
#endif

// scale the Euclidean
#define EUCSCALE 2.3

// disable this if you have no access to SDL_mixer
#ifndef MOBILE
#define AUDIO
#endif

#define NUMWITCH 7

// achievements

// initialize the achievement system.
void achievement_init();

// close the achievement system.
void achievement_close();

// gain the achievement with the given name.
// Only awarded if euclid equals euclideanAchievement.
void achievement_gain(const char*, bool euclideanAchievement = false, bool shmupAchievement = false);

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

// achievements received this game
vector<string> achievementsReceived;

// game forward declarations

bool isCrossroads(eLand l);
enum orbAction { roMouse, roKeyboard, roCheck, roMouseForce };
void moveItem (cell *from, cell *to, bool activateYendor);
void uncoverMines(cell *c, int lev);
bool survivesMine(eMonster m);
void killMonster(cell *c);
void toggleGates(cell *ct, eWall type, int rad);
bool destroyHalfvine(cell *c, eWall newwall = waNone, int tval = 6);
void buildCrossroads2(cell *c);
void createBugArmy(cell *c);
heptagon *createAlternateMap(cell *c, int rad, hstate firststate, int special=0);
void generateAlts(heptagon *h);
void whirlGenerate(cell *wto);
void setdist(cell *c, int d, cell *from);
void checkOnYendorPath();
void killThePlayerAt(eMonster m, cell *c);
bool collectItem(cell *c2, bool telekinesis = false);
void castLightningBolt(struct cellwalker lig);
bool movepcto(int d, int subdir = 1, bool checkonly = false);
void stabbingAttack(cell *mf, cell *mt, eMonster who = moNone);
bool earthMove(cell *from, int dir);
void messageKill(eMonster killer, eMonster victim);
void moveMonster(cell *ct, cell *cf);
int palaceHP();

void createMirrors(cell *c, int dir, eMonster type);
void createMirages(cell *c, int dir, eMonster type);

int neighborId(cell *c1, cell *c2);

struct movedir { int d; int subdir; };
inline bool movepcto(const movedir& md) { return movepcto(md.d, md.subdir); }

void activateActiv(cell *c, bool msg);

// shmup

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
  }

// graph

void showMissionScreen();

void restartGraph();
void resetmusic();

void cleargraphmemory();

void drawFlash(cell* c);

void drawLightning();
void drawSafety();
void restartGraph();
void movepckeydir(int);

void centerpc(ld aspd);

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
  bool goteyes;
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
  
  bool female;
  bool samegender; // same gender for the Princess?
  int language;
  
  int skincolor, haircolor, dresscolor, swordcolor;
  int killreduction;
  
  shmup::config scfg;
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
  emOverview
  };
 
extern emtype cmode, lastmode;

extern transmatrix View; // current rotation, relative to viewctr
extern transmatrix cwtV; // player-relative view

extern cell *mouseover, *mouseover2;

extern struct SDL_Surface *s;

namespace mapeditor { 
  extern bool drawplayer; 
  extern char whichPattern;
  int generateCanvas(cell *c);
  void clearModelCells();
  void applyModelcell(cell *c);
  int realpattern(cell *c);
  int patterndir(cell *c, char w = whichPattern);
  }

void selectEyeGL(int ed);
void selectEyeMask(int ed);
