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

// disable for the Android version
#define ISANDROID 0
#define ISMOBILE 0
#define ISIOS 0
#define VER "7.4h"
#define VERNUM 7480
#define VERNUM_HEX 0x7480

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

const char *scorefile = "hyperrogue.log";
const char *conffile = "hyperrogue.ini";

string levelfile = "hyperrogue.lev";
string picfile = "hyperrogue.pic";

const char *loadlevel = NULL;
const char *musicfile = "";

typedef long double ld;

template<class T> int size(T& x) {return x.size(); }
string its(int i) { char buf[64]; sprintf(buf, "%d", i); return buf; }
string cts(char c) { char buf[8]; buf[0] = c; buf[1] = 0; return buf; }
string llts(long long i) { 
  // sprintf does not work on Windows IIRC
  if(i < 0) return "-" + llts(-i);
  if(i < 10) return its(i);
  return llts(i/10) + its(i%10);
  }
string fts(float x) { char buf[64]; sprintf(buf, "%4.2f", x); return buf; }
string fts4(float x) { char buf[64]; sprintf(buf, "%6.4f", x); return buf; }
string itsh(int i) {static char buf[16]; sprintf(buf, "%03X", i); return buf; }

#undef DEBT
void DEBT(const char *buf) {
  printf("%4d %s\n", SDL_GetTicks(), buf);
  }

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
#include "game.cpp"

// #include "patterngen.cpp"

#include "geometry.cpp"

#include "polygons.cpp"

#ifndef MOBILE
#include "mapeditor.cpp"
#endif

#include "graph.cpp"

#include "achievement.cpp"

#include <unistd.h>

int main(int argc, char **argv) {

  printf("HyperRogue by Zeno Rogue <zeno@attnam.com>, version "VER"\n");

#ifndef NOLICENSE
  printf("released under GNU General Public License version 2 and thus\n");
  printf("comes with absolutely no warranty; see COPYING for details\n");
#endif

  achievement_init();

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
    else if(strcmp(argv[i], "-o") == 0) { commandline += "o"; }
    else if(strcmp(argv[i], "-E") == 0) { commandline += "E"; }
    else if(strcmp(argv[i], "-S") == 0) { commandline += "S"; }
    else if(strcmp(argv[i], "-H") == 0) { commandline += "H"; }
    else if(strcmp(argv[i], "-P") == 0) { commandline += "P"; }
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
      printf("  -o             - switch the OpenGL mode on or off\n");
      printf("  -W LAND        - start in the given land (cheat)\n");
      printf("  -E             - switch Euclidean\n");
      printf("  -S             - switch Shmup\n");
      printf("  -P             - switch Shmup number of players\n");
      printf("  -H             - switch Hardcore\n");
      printf("  -L             - list of features\n");
      exit(0);
      }
    else {
      printf("Unknown option: %s\n", argv[i]);
      exit(3);
      }
    }
  
  /* transmatrix T;
  for(int a=0; a<3; a++) for(int b=0; b<3; b++)
    T[a][b] = (10 + a*a + b*4 + (b==1?a:0)) / 20.;
  display(T);
  transmatrix T2 = inverse(T);
  display(T2);
  display(T*T2); */
  
  eLand f = firstland;
    
  // initlanguage();
  initgraph();
  loadsave();
  initcells();
  /* for(int uu=9; uu >= 0; uu--) {
    printf("uu=%d\n", uu);
    initgame(uu);
    restartGame();
    } */
  
  #ifdef BUILDZEBRA
  firstland = laCanvas;
  shmup::on = false;
  #endif
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
  
  mainloop();

  achievement_final(!items[itOrbSafety]);
  
  saveStats();
  int msec = SDL_GetTicks() - t1;
  printf("frame : %f ms (%f fps)\n", 1.*msec/frames, 1000.*frames/msec);
  offscreen.clear();  
  clearMemory();
  cleargraph();
  
  achievement_close();
  
  return 0;
  }
