#if CAP_COMMANDLINE
const char *scorefile = "hyperrogue.log";
const char *conffile = "hyperrogue.ini";
string levelfile = "hyperrogue.lev";
string picfile = "hyperrogue.pic";
const char *musicfile = "";
const char *loadlevel = NULL;

eLand readland(const char *s) {
  string ss = s;
  if(ss == "II") return laCrossroads2;
  if(ss == "III") return laCrossroads3;
  if(ss == "IV") return laCrossroads4;
  if(ss == "V") return laCrossroads5;
  for(int l=0; l<landtypes; l++) if(strstr(linf[l].name, s) != NULL) {
    return eLand(l);
    break;
    }
  return laNone;
  }

eItem readItem(const char *s) {
  string ss = s;
  for(int i=0; i<ittypes; i++) if(strstr(iinf[i].name, s) != NULL) {
    return eItem(i);
    break;
    }
  return itNone;
  }

eMonster readMonster(const char *s) {
  string ss = s;
  for(int i=0; i<motypes; i++) if(strstr(minf[i].name, s) != NULL) {
    return eMonster(i);
    break;
    }
  return moNone;
  }

void initializeCLI() {
  printf("HyperRogue by Zeno Rogue <zeno@attnam.com>, version " VER "\n");

#ifndef NOLICENSE
  printf("released under GNU General Public License version 2 and thus\n");
  printf("comes with absolutely no warranty; see COPYING for details\n");
#endif

  #ifdef FHS
  static string sbuf, cbuf;
  if(getenv("HOME")) {
    sbuf = getenv("HOME"); sbuf += "/."; sbuf += scorefile;
    cbuf = getenv("HOME"); cbuf += "/."; cbuf += conffile;
    scorefile = sbuf.c_str();
    conffile = cbuf.c_str();
    }
  #endif
  }

int arg::readCommon() {
  if(argis("-c")) { PHASE(1); shift(); conffile = args(); }
  else if(argis("-s")) { PHASE(1); shift(); scorefile = args(); }
  else if(argis("-m")) { PHASE(1); shift(); musicfile = args(); }
#if CAP_SDLAUDIO
  else if(argis("-se")) { PHASE(1); shift(); wheresounds = args(); }
#endif
#if CAP_EDIT
  else if(argis("-lev")) { shift(); levelfile = args(); }
  else if(argis("-pic")) { shift(); picfile = args(); }
  else if(argis("-load")) { PHASE(3); shift(); mapstream::loadMap(args()); }
  else if(argis("-picload")) { PHASE(3); shift(); mapeditor::loadPicFile(args()); }
#endif
  else if(argis("-canvas")) {
    firstland = specialland = laCanvas;
    shift();
    if(args()[1] == 0) patterns::whichCanvas = args()[0];
    else patterns::canvasback = strtol(args(), NULL, 16);
    }
  else if(argis("-noplayer")) 
    mapeditor::drawplayer = !mapeditor::drawplayer;
  else if(argis("-pattern")) {
    PHASE(3);
    shift();
    char *c = args();
    using namespace patterns;
    subpattern_flags = 0;
    while(*c) { 
      if(*c >= '0' && *c <= '9') subpattern_flags ^= 1 << (*c - '0'); 
      else whichPattern = *c;
      c++; 
      }
    }
  else if(argis("-nogui")) {
    noGUI = true;
    }
  else if(argis("-back")) {
    shift(); backcolor = strtol(args(), NULL, 16);
    }
  else if(argis("-borders")) {
    shift(); bordcolor = strtol(args(), NULL, 16);
    }
  else if(argis("-fore")) {
    shift(); forecolor = strtol(args(), NULL, 16);
    }
  else if(argis("-W2")) {
    shift(); cheatdest = readland(args()); autocheat = true;
    showstartmenu = false;
    }
  else if(argis("-W")) {
    shift(); 
    firstland0 = firstland = specialland = readland(args()); autocheat = true;
    showstartmenu = false;
    }
  else if(argis("-I")) {
    PHASE(3) cheater++; timerghost = false;
    shift(); eItem i = readItem(args());
    shift(); items[i] = argi(); 
    }
  else if(argis("-IP")) {
    PHASE(3) cheater++; timerghost = false;
    shift(); eItem i = readItem(args());
    shift(); int q = argi();
    placeItems(q, i);
    }
  else if(argis("-ambush")) {
    // make all ambushes use the given number of dogs
    // example: hyper -W Hunt -IP Shield 1 -ambush 60
    PHASE(3) cheater++; timerghost = false;
    shift(); ambushval = argi();
    }
  else if(argis("-M")) {
    PHASE(3) cheater++; timerghost = false;
    shift(); eMonster m = readMonster(args());
    shift(); int q = argi();
    printf("m = %s q = %d\n", dnameof(m), q);
    restoreGolems(q, m, 7);
    }
  else if(argis("-L")) {
    printf("Treasures:\n");
    for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_TREASURE)
        printf("    %s\n", iinf[i].name);
    printf("\n");
    printf("Orbs:\n");
    for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_ORB)
        printf("    %s\n", iinf[i].name);
    printf("\n");
    printf("Other items:\n");
    for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_OTHER)
        printf("    %s\n", iinf[i].name);
    printf("\n");
    printf("Monsters:\n");
    for(int i=1; i<motypes; i++) 
      printf("    %s\n", minf[i].name);
    printf("\n");
    printf("Lands:\n");
    for(int i=1; i<landtypes; i++) 
      printf("    %s\n", linf[i].name);
    printf("\n");
    printf("Walls:\n");
    for(int i=0; i<walltypes; i++) 
      printf("    %s\n", winf[i].name);
    printf("\n");
    exit(0);
    }

  else if(argis("-aa")) { PHASEFROM(2); shift(); vid.antialias = argi(); }
  else if(argis("-lw")) { PHASEFROM(2); shift(); vid.linewidth = argf(); }
  else if(argis("-wm")) { PHASEFROM(2); shift(); vid.wallmode = argi(); }
  else if(argis("-mm")) { PHASEFROM(2); shift(); vid.monmode = argi(); }

#define TOGGLE(x, param, act) \
else if(args()[0] == '-' && args()[1] == x && !args()[2]) { showstartmenu = false; if(curphase == 3) {act;} else {PHASE(2); param = !param;} } \
else if(args()[0] == '-' && args()[1] == x && args()[2] == '1') { showstartmenu = false; if(curphase == 3 && !param) {act;} else {PHASE(2); param = true;} } \
else if(args()[0] == '-' && args()[1] == x && args()[2] == '0') { showstartmenu = false; if(curphase == 3 && param) {act;} else {PHASE(2); param = false;} }

  TOGGLE('o', vid.usingGL, switchGL())
  TOGGLE('C', chaosmode, restartGame('C'))
  TOGGLE('7', nontruncated, restartGame('7'))
  TOGGLE('f', vid.full, switchFullscreen())
  TOGGLE('T', tactic::on, restartGame('t'))
  TOGGLE('S', shmup::on, restartGame('s'))
  TOGGLE('H', hardcore, switchHardcore())
  TOGGLE('R', randomPatternsMode, restartGame('r'))
  TOGGLE('i', inv::on, restartGame('i'))
  
  else if(argis("-peace")) {
    peace::otherpuzzles = true;
    if(curphase == 3) restartGame('P');
    else peace::on = true;
    }
  else if(argis("-pmem")) {
    peace::otherpuzzles = false;
    if(curphase == 3) restartGame('P');
    else peace::on = true;
    }
  else if(argis("-geo")) { 
    if(curphase == 3) {
      shift(); targetgeometry = (eGeometry) argi();
      restartGame('g');
      }
    else {      
      PHASE(2); shift(); geometry = targetgeometry = (eGeometry) argi();
      }
    }
  else if(argis("-qs")) {
    autocheat = true;
    shift(); currfp.qpaths.push_back(args());
    }
  else if(argis("-fix")) {
    fixseed = true; autocheat = true;
    }
  else if(argis("-fixx")) {
    fixseed = true; autocheat = true;
    shift(); startseed = argi();
    }
  else if(argis("-steplimit")) {
    fixseed = true; autocheat = true;
    shift(); steplimit = argi();
    }
  else if(argis("-qpar")) { 
    int p;
    shift(); sscanf(args(), "%d,%d,%d", 
      &p, &quotientspace::rvadd, &quotientspace::rvdir
      );
    autocheat = true;
    currfp.init(p); 
    }
  else if(argis("-tpar")) { 
    shift(); sscanf(args(), "%d,%d,%d", 
      &torusconfig::qty, 
      &torusconfig::dx,
      &torusconfig::dy
      );
    }
  else if(argis("-cs")) {
    shift(); 
    fieldpattern::matrix M = currfp.strtomatrix(args());
    fieldpattern::subpathid = currfp.matcode[M];
    fieldpattern::subpathorder = currfp.order(M);
    autocheat = true;
    }
  else if(argis("-csp")) {
    autocheat = true;
    currfp.findsubpath();
    }
  else if(argis("-fi")) {
    fieldpattern::info();
    exit(0);
    } 
  else if(argis("-quantum")) {
    quantum = true;
    autocheat = true;
    }
  else if(argis("-P")) { 
    PHASE(2); shift(); 
    vid.scfg.players = argi(); 
    }
  else if(argis("-PM")) { 
    PHASEFROM(2); shift(); pmodel = eModel(argi());
    }
  else if(argis("-offline")) offlineMode = true;
  else if(argis("-debugf")) {
    debugfile = fopen("hyperrogue-debug.txt", "w");
    shift(); debugflags = argi();
    }
  else if(argis("-debuge")) {
    debugfile = stderr;
    shift(); debugflags = argi();
    }
  else if(argis("-each")) {
    shift(); int q = argi(); autocheat = true;
    for(int i=0; i<ittypes; i++)
      if(itemclass(eItem(i)) == IC_TREASURE)
        items[i] = q;
    }
  else if(argis("-ch")) { autocheat = true; }
  else if(argis("-zoom")) { 
    PHASEFROM(2); shift(); vid.scale = argf();
    }
  else if(argis("-alpha")) { 
    PHASEFROM(2); shift(); vid.alpha = argf();
    }
  else if(argis("-Y")) { 
    yendor::on = true;
    shift(); yendor::challenge = argi();
    }
  else if(argis("-r")) { 
    PHASEFROM(2);
    shift(); 
    int clWidth=0, clHeight=0, clFont=0;
    sscanf(args(), "%dx%dx%d", &clWidth, &clHeight, &clFont);
    if(clWidth) vid.xres = clWidth;
    if(clHeight) vid.yres = clHeight;
    if(clFont) vid.fsize = clFont;
    }    
  else if(argis("--version") || argis("-v")) {
    printf("HyperRogue version " VER "\n");
    exit(0);
    }
  else if(argis("--run")) {
    PHASE(3); mainloop(); quitmainloop = false;
    }
#if CAP_TOUR
  else if(argis("--tour")) {
    PHASE(3); tour::start();
    }
  else if(argis("--presentation")) {
    PHASE(3); tour::texts = false;
    tour::start();
    }
#endif
  else if(argis("--draw")) {
    PHASE(3); drawscreen();
    }
  else if(argis("--exit")) {
    PHASE(3); printf("Success.\n");
    exit(0);
    }
  else if(argis("-gencells")) {
    PHASE(3); shift(); 
    printf("Generating %d cells...\n", argi());
    celllister cl(cwt.c, 50, argi(), NULL);
    printf("Cells generated: %d\n", size(cl.lst));
    for(int i=0; i<size(cl.lst); i++)
      setdist(cl.lst[i], 7, NULL);
    }
  else if(argis("-sr")) {    
    PHASEFROM(2);
    shift(); sightrange = argi();
    }
  else if(argis("-srx")) {    
    PHASEFROM(2);
    shift(); sightrange = argi(); overgenerate = true; autocheat = true;
    }
  else if(argis("-we")) {    
    PHASEFROM(2);
    shift(); whatever = argf(); resetGeometry();
    }
  else if(argis("-rch")) {    
    PHASEFROM(2);
    reptilecheat = true; autocheat = true; firstland = laReptile;
    }
#if CAP_SDL
  else if(argis("-pngshot")) {
    PHASE(3); shift(); 
    printf("saving PNG screenshot to %s\n", args());
    saveHighQualityShot(args());
    }
#endif
  else if(argis("-svgsize")) {
    shift(); sscanf(args(), "%d/%d", &svg::svgsize, &svg::divby);
    }
  else if(argis("-svgfont")) {
    shift(); svg::font = args();
    // note: use '-svgfont latex' to produce text output as: \myfont{size}{text}
    // (this is helpful with Inkscape's PDF+TeX output feature; define \myfont yourself)
    }
  else if(argis("-pngsize")) {
    shift(); sscanf(args(), "%d", &pngres);
    }
  else if(argis("-pngformat")) {
    shift(); sscanf(args(), "%d", &pngformat);
    }
  else if(argis("-svggamma")) {
    shift(); svg::gamma = argf();
    }
  else if(argis("-svgshot")) {
    PHASE(3); shift(); 
    printf("saving SVG screenshot to %s\n", args());
    svg::render(args());
    }
  else if(argis("--help") || argis("-h")) {
    printf("Press F1 while playing to get ingame options.\n\n");
    printf("HyperRogue accepts the following command line options:\n");
    printf("  -c FILE        - use the specified configuration file\n");
    printf("  -s FILE        - use the specified highscore file\n");
    printf("  -m FILE        - use the specified soundtrack (music)\n");
    printf("  -se DIR        - the directory containing sound effects\n");
    printf("  -lev FILE      - use the specified filename for the map editor (without loading)\n");
    printf("  -load FILE     - use the specified filename for the map editor\n");
    printf("  -canvas COLOR  - set background color or pattern code for the canvas\n");
    printf("  --version, -v  - show the version number\n");
    printf("  --help, -h     - show the commandline options\n");
    printf("  -f*            - toggle fullscreen mode\n");
    printf("  -wm n, -mm n   - start in the given wallmode or monmode\n");
    printf("  -r WxHxF       - use the given resolution and font size\n");
    printf("  -o*            - toggle the OpenGL mode\n");
    printf("  -W LAND        - start in the given land (cheat)\n");
    printf("  -W2 LAND       - make the given land easy to find (also turns on autocheat)\n");
    printf("  -ch            - auto-enable cheat mode\n");
    printf("  -geo n         - switch geometry (1=Euclidean, 2=spherical, 3=elliptic, 4/5=quotient)\n");
    printf("  -qs <desc>     - fieldpattern: quotient by the given <desc> (must be followed by qpar)\n");
    printf("  -qpar <prime>  - fieldpattern: use the given prime instead of 43\n");
    printf("  -cs <desc>     - fieldpattern: set subpath to the given <desc> (cannot be followed by qpar)\n");
    printf("  -csp           - fieldpattern: find the subpath of order <prime> (cannot be followed by qpar)\n");
    printf("  -S*            - toggle Shmup\n");
    printf("  -P n           - switch Shmup number of players (n=1..7)\n");
    printf("  -PM            - switch the model index\n");
    printf("  -H*            - toggle Hardcore\n");
    printf("  -T*            - toggle Tactical\n");
    printf("  -7*            - toggle heptagonal mode\n");
    printf("  -C*            - toggle Chaos mode\n");
    printf("  -R*            - toggle Random Pattern\n");
    printf("  -Y id          - enable Yendor, level id\n");
    printf("  -D             - disable all the special game modes\n");
    printf("  -L             - list of features\n");
    printf("  -debugf 7      - output debugging information to hyperrogue-debug.txt\n");
    printf("  -debuge 7      - output debugging information to stderr\n");
    printf("  -offline       - don't connect to Steam (for Steam versions)\n");
    printf("  -I ITEM n      - start with n of ITEM (activates cheat and disables ghosts)\n");
    printf("  -fix           - fix the seed\n");
    printf("Toggles: -o0 disables, -o1 enables, -o switches");
    printf("Not all options are documented, see hyper.cpp");
    exit(0);
    }
  else if(ca::readArg()) ;
  else return 1;
  return 0;
  }

purehookset hooks_config;

hookset<int()> *hooks_args;

namespace arg {
  int argc; char **argv;
  int curphase;

  auto ah = addHook(hooks_args, 0, readCommon);
  
  void read(int phase) { 
    curphase = phase;
    callhooks(hooks_config);
    while(argc) {
      for(auto& h: *hooks_args) {
        int r = h.second(); if(r == 2) return; if(r == 0) { lshift(); goto cont; }
        }
      printf("Unknown option: %s\n", args());
      exit(3);
      cont: ;
      }
    }
  }
#endif
