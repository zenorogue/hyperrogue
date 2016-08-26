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

#ifdef LOCAL
#define CDATA
#endif

#define VER "8.3j"
#define VERNUM 8310
#define VERNUM_HEX 0x8310

#define ISANDROID 0
#define ISMOBILE 0
#define ISIOS 0

#include <stdio.h>

#include <SDL/SDL.h>

#ifndef MAC
#undef main
#endif

#include <SDL/SDL_ttf.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

FILE *debugfile;
int debugflags;

const char *scorefile = "hyperrogue.log";
const char *conffile = "hyperrogue.ini";

string levelfile = "hyperrogue.lev";
string picfile = "hyperrogue.pic";

const char *loadlevel = NULL;
const char *musicfile = "";

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

string s0;
void addMessage(string s, char spamtype = 0);

int clWidth, clHeight, clFont;
string commandline;

#include "hyperpoint.cpp"
#include "patterns.cpp"
#include "heptagon.cpp"
#include "classes.cpp"
#include "language.cpp"

#ifdef STEAM
#define NOLICENSE
#endif

#include "hyper.h"

#include "cell.cpp"

#include "flags.cpp"
#include "yendor.cpp"
#include "complex.cpp"
#include "game.cpp"
#include "landgen.cpp"
#include "orbs.cpp"
#include "system.cpp"

// #include "patterngen.cpp"

#include "geometry.cpp"

#include "polygons.cpp"

#ifndef MOBILE
#include "mapeditor.cpp"
#endif

#include "netgen.cpp"

#include "graph.cpp"

#include "achievement.cpp"

#include <unistd.h>

int main(int argc, char **argv) {

#ifdef LINUX
  moreStack();
#endif

  printf("HyperRogue by Zeno Rogue <zeno@attnam.com>, version " VER "\n");

#ifndef NOLICENSE
  printf("released under GNU General Public License version 2 and thus\n");
  printf("comes with absolutely no warranty; see COPYING for details\n");
#endif

  // printf("cell size = %d\n", int(sizeof(cell)));
  srand(time(NULL));
  shrand(time(NULL));
  
  #ifdef FHS
  char sbuf[640], cbuf[640];
  if(getenv("HOME")) {
    snprintf(sbuf, 640, "%s/.%s", getenv("HOME"), scorefile); scorefile = sbuf;
    snprintf(cbuf, 640, "%s/.%s", getenv("HOME"), conffile); conffile = cbuf;
    }
  #endif

  for(int i=1; i<argc; i++) {
    if(strcmp(argv[i], "-c") == 0 && i != argc-1) {conffile = argv[i+1]; i++;}
    else if(strcmp(argv[i], "-s") == 0 && i != argc-1) {scorefile = argv[i+1]; i++;}
    else if(strcmp(argv[i], "-m") == 0 && i != argc-1) {musicfile = argv[i+1]; i++;}
    else if(strcmp(argv[i], "-lev") == 0 && i != argc-1) {levelfile = argv[i+1]; i++;}
    else if(strcmp(argv[i], "-pic") == 0 && i != argc-1) {picfile = argv[i+1]; i++;}
    else if(strcmp(argv[i], "-load") == 0 && i != argc-1) {loadlevel = argv[i+1]; i++;}
//  else if(strcmp(argv[i], "-P") == 0 && i != argc-1) {par = atoi(argv[i+1]); i++;}
    else if(strcmp(argv[i], "-W") == 0 && i != argc-1) {
      for(int l=0; l<landtypes; l++) if(strstr(linf[l].name, argv[i+1]) != NULL) {
        firstland = euclidland = eLand(l);
        break;
        }
      i++;
      }
    else if(strcmp(argv[i], "-L") == 0) {
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
    else if(strcmp(argv[i], "-f") == 0) { commandline += "f"; }
    else if(strcmp(argv[i], "-w") == 0) { commandline += "w"; }
    else if(strcmp(argv[i], "-e") == 0) { commandline += "e"; }
    else if(strcmp(argv[i], "-a") == 0) { commandline += "a"; }
    else if(strcmp(argv[i], "-p") == 0) { commandline += "p"; }
    else if(strcmp(argv[i], "-7") == 0) { commandline += "7"; }
    else if(strcmp(argv[i], "-C") == 0) { commandline += "C"; }
    else if(strcmp(argv[i], "-o") == 0) { commandline += "o"; }
    else if(strcmp(argv[i], "-o0") == 0) { commandline += char(200); }
    else if(strcmp(argv[i], "-o1") == 0) { commandline += char(201); }
    else if(strcmp(argv[i], "-E") == 0) { commandline += "E"; }
    else if(strcmp(argv[i], "-S") == 0) { commandline += "S"; }
    else if(strcmp(argv[i], "-H") == 0) { commandline += "H"; }
    else if(strcmp(argv[i], "-P1") == 0) { commandline += "P1"; }
    else if(strcmp(argv[i], "-P2") == 0) { commandline += "P2"; }
    else if(strcmp(argv[i], "-P3") == 0) { commandline += "P3"; }
    else if(strcmp(argv[i], "-P4") == 0) { commandline += "P4"; }
    else if(strcmp(argv[i], "-T") == 0) { commandline += "T"; }
    else if(strcmp(argv[i], "-R") == 0) { commandline += "R"; }
    else if(strcmp(argv[i], "-D") == 0) { commandline += "D"; }
    else if(strcmp(argv[i], "-PM1") == 0) { pmodel = 1; }
    else if(strcmp(argv[i], "-PM2") == 0) { pmodel = 2; }
    else if(strcmp(argv[i], "-offline") == 0) offlineMode = true;
    else if(strcmp(argv[i], "-debugf") == 0) {
      debugfile = fopen("hyperrogue-debug.txt", "w");
      debugflags = atoi(argv[i+1]);
      i++;
      }
    else if(strcmp(argv[i], "-debuge") == 0) {
      debugfile = stderr;
      debugflags = atoi(argv[i+1]);
      i++;
      }
#ifdef LOCAL
    else if(strcmp(argv[i], "-auto") == 0) doAutoplay = true;
#endif
    else if(strcmp(argv[i], "-ch") == 0) { autocheat = true; }
    else if(strcmp(argv[i], "-Y") == 0) { 
      yendor::on = true;
      yendor::challenge = atoi(argv[i+1]);
      i++;
      }
    else if(strcmp(argv[i], "-r") == 0) { 
      i++; 
      sscanf(argv[i], "%dx%dx%d", &clWidth, &clHeight, &clFont);
      }    
    else if(strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
      printf("HyperRogue version " VER "\n");
      exit(0);
      }
    else if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      printf("Press F1 while playing to get ingame options.\n\n");
      printf("HyperRogue accepts the following command line options:\n");
      printf("  -c FILE        - use the specified configuration file\n");
      printf("  -s FILE        - use the specified highscore file\n");
      printf("  -m FILE        - use the specified soundtrack (music)\n");
      printf("  -lev FILE      - use the specified filename for the map editor (without loading)\n");
      printf("  -load FILE     - use the specified filename for the map editor\n");
      printf("  --version, -v  - show the version number\n");
      printf("  --help, -h     - show the commandline options\n");
      printf("  -f, -w         - start in the fullscreen or windowed mode\n");
      printf("  -e, -a, -p     - start in the Escher, ASCII, or Plain mode\n");
      printf("  -r WxHxF       - use the given resolution and font size\n");
      printf("  -o             - switch the OpenGL mode\n");
      printf("  -o0            - switch the OpenGL mode off\n");
      printf("  -o1            - switch the OpenGL mode on\n");
      printf("  -W LAND        - start in the given land (cheat)\n");
      printf("  -ch            - auto-enable cheat mode\n");
      printf("  -E             - switch Euclidean\n");
      printf("  -S             - switch Shmup\n");
      printf("  -Pn            - switch Shmup number of players (n=1..4)\n");
      printf("  -H             - switch Hardcore\n");
      printf("  -T             - switch Tactical\n");
      printf("  -7             - switch heptagonal mode\n");
      printf("  -C             - switch Chaos mode\n");
      printf("  -R             - switch Random Pattern\n");
      printf("  -Y id          - switch Yendor, level id\n");
      printf("  -D             - disable all the special game modes\n");
      printf("  -L             - list of features\n");
      printf("  -debugf 7      - output debugging information to hyperrogue-debug.txt\n");
      printf("  -debuge 7      - output debugging information to stderr\n");
      printf("  -offline       - don't connect to Steam (for Steam versions)\n");
      exit(0);
      }
    else {
      printf("Unknown option: %s\n", argv[i]);
      exit(3);
      }
    }
  
  achievement_init();

  eLand f = firstland;
    
  // initlanguage();
  initgraph();
  loadsave();
  precalc(); 
  resetGL();
  initcells();
  
  #ifdef BUILDZEBRA
  firstland = laCanvas;
  shmup::on = false; 
  #endif
  shmup::safety = safety;
  initgame();
  #ifdef BUILDZEBRA
  zebraPattern();
  #endif
  
  if(!shmup::on) {
    restoreGolems(items[itOrbLife], moGolem); items[itOrbLife] = 0;
    restoreGolems(items[itOrbFriend], moTameBomberbird); items[itOrbFriend] = 0;
    restoreGolems(kills[moPrincessMoved], moPrincess, princess::saveHP); kills[moPrincessMoved] = 0;
    restoreGolems(kills[moPrincessArmedMoved], moPrincessArmed, princess::saveArmedHP); kills[moPrincessArmedMoved] = 0;
    }
  
  firstland = f;
  
  // verifyHell();
  // exit(1);
  
  int t1 = SDL_GetTicks();
  
  // if(switchEuclid) restartGame('e');
  
  if(loadlevel) mapstream::loadMap(loadlevel);

#ifdef LOCAL  
  // river();
  autoplay();
#endif
  mainloop();
  
  achievement_final(!items[itOrbSafety]);
  
  saveStats();
  int msec = SDL_GetTicks() - t1;
  DEBB(DF_INIT, (debugfile, "frame : %f ms (%f fps)\n", 1.*msec/frames, 1000.*frames/msec));
  offscreen.clear();  
  clearMemory();
  cleargraph();
  
  achievement_close();
  
  return 0;
  }
