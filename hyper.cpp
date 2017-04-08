// Hyperbolic Rogue
// Copyright (C) 2011 Zeno Rogue

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#define ISANDROID 0
#define ISMOBILE 0
#define ISIOS 0
#define USE_SDL
#define USE_COMMANDLINE

#ifdef STEAM
#define NOLICENSE
#endif

#include "init.cpp"

#ifdef LINUX
#include <sys/resource.h>

void moreStack() {
  const rlim_t kStackSize = 1 << 28; // 28;
  struct rlimit rl;
  int result;

  result = getrlimit(RLIMIT_STACK, &rl);
  if(result == 0) {
    if(rl.rlim_cur < kStackSize) {
      // rl.rlim_cur = 1 << 19; // kStackSize;
      result = setrlimit(RLIMIT_STACK, &rl);
      if (result != 0) {
        fprintf(stderr, "setrlimit returned result = %d\n", result);
        }
      }
    }
  }
#endif

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
  else if(argis("-se")) { PHASE(1); shift(); wheresounds = args(); }
#ifndef NOEDIT
  else if(argis("-lev")) { shift(); levelfile = args(); }
  else if(argis("-pic")) { shift(); picfile = args(); }
  else if(argis("-load")) { PHASE(3); shift(); mapstream::loadMap(loadlevel); }
#endif
  else if(argis("-canvas")) {
    firstland = euclidland = laCanvas;
    shift();
    if(args()[1] == 0) mapeditor::whichCanvas = args()[0];
    else mapeditor::canvasback = strtol(args(), NULL, 16);
    }
  else if(argis("-back")) {
    shift(); backcolor = strtol(args(), NULL, 16);
    }
  else if(argis("-W2")) {
    shift(); cheatdest = readland(args()); autocheat = true;
    }
  else if(argis("-W")) {
    shift(); firstland = euclidland = readland(args());
    }
  else if(argis("-I")) {
    PHASE(3) cheater++; timerghost = false;
    shift(); eItem i = readItem(args());
    shift(); items[i] = argi(); 
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

  else if(argis("-wm")) { PHASE(2); vid.wallmode = argi(); }
  else if(argis("-mm")) { PHASE(2); vid.monmode = argi(); }

#define TOGGLE(x, param) \
else if(args()[0] == '-' && args()[1] == x && !args()[2]) { PHASE(2); param = !param; } \
else if(args()[0] == '-' && args()[1] == x && args()[2] == '1') { PHASE(2); param = true; } \
else if(args()[0] == '-' && args()[1] == x && args()[2] == '0') { PHASE(2); param = false; }

  TOGGLE('o', vid.usingGL)
  TOGGLE('C', chaosmode)
  TOGGLE('7', purehepta)
  TOGGLE('f', vid.full)
  TOGGLE('T', tactic::on)
  TOGGLE('S', shmup::on)
  TOGGLE('H', hardcore)
  TOGGLE('R', randomPatternsMode)
  
  else if(argis("-geo")) { PHASE(2); shift(); geometry = targetgeometry = (eGeometry) argi(); }
  else if(argis("-qs")) {
    autocheat = true;
    shift(); fp43.qpaths.push_back(args());
    }
  else if(argis("-fix")) {
    fixseed = true; autocheat = true;
    }
  else if(argis("-qpar")) { 
    int p;
    shift(); sscanf(args(), "%d,%d,%d", 
      &p, &quotientspace::rvadd, &quotientspace::rvdir
      );
    autocheat = true;
    fp43.init(p); 
    }
  else if(argis("-cs")) {
    shift(); 
    fieldpattern::matrix M = fp43.strtomatrix(args());
    fieldpattern::subpathid = fp43.matcode[M];
    fieldpattern::subpathorder = fp43.order(M);
    autocheat = true;
    }
  else if(argis("-csp")) {
    autocheat = true;
    fp43.findsubpath();
    }
  else if(argis("-fi")) {
    fieldpattern::info();
    exit(0);
    } 
  else if(argis("-P")) { 
    PHASE(2); shift(); 
    vid.scfg.players = argi(); 
    }
  else if(argis("-PM")) { 
    PHASE(2); shift(); pmodel = eModel(argi());
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
  else if(argis("-ch")) { autocheat = true; }
  else if(argis("-zoom") && curphase == 2) { 
    PHASE(2); shift(); vid.scale = argf();
    }
  else if(argis("-zoom") && curphase == 3) { 
    PHASE(3); shift(); vid.scale = argf();
    }
  else if(argis("-Y")) { 
    yendor::on = true;
    shift(); yendor::challenge = argi();
    }
  else if(argis("-r")) { 
    PHASE(2);
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
#ifdef TOUR
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
    if(curphase == 1) PHASE(2);
    shift(); sightrange = argi();
    }
  else if(argis("-pngshot")) {
    PHASE(3); shift(); 
    printf("saving PNG screenshot to %s\n", args());
    saveHighQualityShot(args());
    }
  else if(argis("-svgsize")) {
    shift(); sscanf(args(), "%d/%d", &svg::svgsize, &svg::divby);
    }
  else if(argis("-pngsize")) {
    shift(); sscanf(args(), "%d", &pngres);
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

int main(int argc, char **argv) {
#ifndef WEB
  #ifdef LINUX
    moreStack();
  #endif
  arg::init(argc, argv);
  initializeCLI();
#endif
  initAll();
  arg::read(3);
  mainloop();
  finishAll();  
  profile_info();
  return 0;
  }

#ifdef USE_COMMANDLINE
namespace arg {
  int argc; char **argv;
  
  void read(int phase) { 
    curphase = phase;
    while(argc) {
      int r;
      r = readCommon(); if(r == 2) return; if(r == 0) { lshift(); continue; }
#ifdef LOCAL
      r = readLocal(); if(r == 2) return; if(r == 0) { lshift(); continue; }
#endif
#ifdef ROGUEVIZ
      r = rogueviz::readArgs(); if(r == 2) return; if(r == 0) { lshift(); continue; }
#endif
      printf("Unknown option: %s\n", args());
      exit(3);
      }
    }
  }
#endif
