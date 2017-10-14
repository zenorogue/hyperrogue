#define VER "10.1b"
#define VERNUM 10102
#define VERNUM_HEX 0xA082

#define GEN_M 0
#define GEN_F 1
#define GEN_N 2
#define GEN_O 3

// OS settings

#ifndef ISMAC
#define ISMAC 0
#endif

#ifndef ISLINUX
#define ISLINUX 0
#endif

#ifndef ISWINDOWS
#define ISWINDOWS 0
#endif

#ifndef ISPANDORA
#define ISPANDORA 0
#endif

#ifndef ISIOS
#define ISIOS 0
#endif

#ifndef ISANDROID
#define ISANDROID 0
#endif

#ifndef ISWEB
#define ISWEB 0
#endif

#ifndef ISFAKEMOBILE
#define ISFAKEMOBILE 0
#endif

#define ISMOBILE (ISIOS || ISANDROID || ISFAKEMOBILE)
#define ISMOBWEB (ISMOBILE || ISWEB)

#ifndef ISMINI
#define ISMINI 0
#endif

#ifndef CAP_XGD
#define CAP_XGD (ISANDROID || ISFAKEMOBILE)
#endif

#define CAP_FRAMELIMIT (!ISMOBWEB)

#if ISMOBILE==1
#define EXTERNALFONT
#endif

#ifndef CAP_INV
#define CAP_INV 1
#endif

#ifndef CAP_ANDROIDSHARE
#define CAP_ANDROIDSHARE (ISANDROID)
#endif

#ifndef CAP_SDL
#define CAP_SDL (!ISMOBILE)
#endif

#ifndef CAP_SDLGFX
#define CAP_SDLGFX (CAP_SDL && !ISWEB)
#endif

#ifndef CAP_GL
#define CAP_GL (ISMOBILE || CAP_SDL)
#endif

#ifndef CAP_AUDIO
#define CAP_AUDIO ((ISMOBILE || CAP_SDL) && !ISWEB && !ISMINI)
#endif

#define CAP_GLORNOT (CAP_GL && !ISWEB && !ISIOS) 

#ifndef CAP_CERTIFY
#define CAP_CERTIFY 0
#endif

#ifndef CAP_RUG
#define CAP_RUG (!ISMOBWEB && !ISMINI && CAP_SDL && CAP_GL)
#endif

#ifndef CAP_EDIT
#define CAP_EDIT (!ISWEB && !ISMINI)
#endif

#ifndef CAP_MODEL
#define CAP_MODEL (!ISMOBWEB && !ISMINI && CAP_SDLGFX)
#endif

#ifndef CAP_SAVE
#define CAP_SAVE (!ISWEB && !ISMINI)
#endif

#ifndef CAP_CONFIG
#define CAP_CONFIG (!ISWEB && !ISMINI)
#endif

#ifndef CAP_TRANS
#define CAP_TRANS (!ISWEB && !ISMINI)
#endif

#ifndef CAP_TOUR
#define CAP_TOUR (!ISWEB && !ISMINI)
#endif

#ifndef CAP_ROGUEVIZ
#define CAP_ROGUEVIZ 0
#endif

#ifndef CAP_PROFILING
#define CAP_PROFILING 0
#endif

#define PSEUDOKEY_WHEELDOWN 2501
#define PSEUDOKEY_WHEELUP 2502

#ifndef CAP_PNG
#define CAP_PNG (!ISMOBWEB && !ISMAC && CAP_SDLGFX)
#endif

#ifndef CAP_COMMANDLINE
#define CAP_COMMANDLINE (!ISMOBILE)
#endif

#define CAP_SDLAUDIO (CAP_SDL && CAP_AUDIO)

#ifndef CAP_SVG
#define CAP_SVG (!ISMOBILE)
#endif

#ifndef CAP_SDLJOY
#define CAP_SDLJOY (CAP_SDL && !ISWEB)
#endif

#ifndef CAP_SDLTTF
#define CAP_SDLTTF (CAP_SDL && !ISMOBILE && !ISWEB)
#endif

#define CAP_GLFONT (CAP_GL && !ISMOBILE)

#ifndef CAP_TABFONT
#define CAP_TABFONT (ISWEB)
#endif

#ifndef CAP_CREATEFONT
#define CAP_CREATEFONT 0
#endif

#ifndef CAP_FIXEDSIZE
#define CAP_FIXEDSIZE (CAP_CREATEFONT || CAP_TABFONT ? 36 : 0)
#endif

#ifndef CAP_SHMUP
#define CAP_SHMUP 1
#endif

#ifdef ISSTEAM
#define CAP_ACHIEVE 1
#endif

#ifndef CAP_ACHIEVE
#define CAP_ACHIEVE 0
#endif

#ifndef CAP_SHMUP_GOOD
#define CAP_SHMUP_GOOD (!ISMOBWEB)
#endif

#if ISMOBILE
#define EXTRALICENSE "\n\nHyperRogue soundtrack by Shawn Parrotte (http://www.shawnparrotte.com), under the Creative Commons BY-SA 3.0 license, http://creativecommons.org/licenses/by-sa/3.0/"
#undef XEXTRALICENSE

int fontscale = 100;
bool buttonclicked;
void gdpush(int t);
#endif

#ifndef HYPERPATH
#define HYPERPATH ""
#endif

#include <stdio.h>

#if CAP_SDL
#include <SDL/SDL.h>

#if !ISMAC
#undef main
#endif

#if CAP_SDLAUDIO
#include <SDL/SDL_mixer.h>
#endif

#if CAP_SDLTTF
#include <SDL/SDL_ttf.h>
#endif

#if CAP_SDLGFX
#include <SDL/SDL_gfxPrimitives.h>
#endif

#elif !ISFAKEMOBILE
#define SDLK_F1  (123001)
#define SDLK_F2  (123002)
#define SDLK_F3  (123003)
#define SDLK_F4  (123004)
#define SDLK_F5  (123005)
#define SDLK_F6  (123006)
#define SDLK_F7  (123007)
#define SDLK_F8  (123008)
#define SDLK_F9  (123009)
#define SDLK_F10 (123010)
#define SDLK_ESCAPE (123099)
#define SDLK_F12 (123012)
#define SDLK_HOME (123013)
#define SDLK_LEFT (123014)
#define SDLK_RIGHT (123015)
#define MIX_MAX_VOLUME 128
#define SDLK_UP (123021)
#define SDLK_DOWN (123022)
#define SDLK_PAGEUP (123023)
#define SDLK_PAGEDOWN (123024)
#define SDLK_RETURN (123025)
#define SDLK_KP1 (123031)
#define SDLK_KP2 (123032)
#define SDLK_KP3 (123033)
#define SDLK_KP4 (123034)
#define SDLK_KP5 (123035)
#define SDLK_KP6 (123036)
#define SDLK_KP7 (123037)
#define SDLK_KP8 (123038)
#define SDLK_KP9 (123039)
#define SDLK_KP_PERIOD (123051)
#define SDLK_KP_MINUS  (123053)
#define SDLK_DELETE (123052)
#define SDLK_DELETE (123052)
#define SDLK_KP_ENTER (123054)
#define SDLK_BACKSPACE (123055)
typedef int SDL_Event;
#endif

#if ISWEB
inline Uint8 *SDL_GetKeyState(void *v) { static Uint8 tab[1024]; return tab; }
#endif

#ifndef CAP_GLEW
#define CAP_GLEW (CAP_GL && !ISMOBILE && !ISMAC && !ISLINUX)
#endif

#if CAP_GLEW
  #include <GL/glew.h>
#else
  #define GL_GLEXT_PROTOTYPES 1
  #if ISMAC
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <OpenGL/glext.h>
  #elif ISIOS
    // already included
  #elif ISANDROID
    #include <GLES/gl.h>
    #include <GLES/glext.h>
    #include <GLES2/gl2.h>
  #else
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glext.h>
    #endif
  #endif

#include <functional>
#include <memory>
#include <cmath>
#include <time.h>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>
#include <queue>
#include <stdexcept>
#include <array>

#ifdef USE_UNORDERED_MAP
#include <unordered_map>
#else
#define unordered_map map
#endif

using namespace std;

string s0;
void addMessage(string s, char spamtype = 0);

#if ISANDROID
FILE *debfile;
#endif

FILE *debugfile;
int debugflags;

#if CAP_COMMANDLINE
const char *scorefile = "hyperrogue.log";
const char *conffile = "hyperrogue.ini";
string levelfile = "hyperrogue.lev";
string picfile = "hyperrogue.pic";
const char *musicfile = "";
const char *loadlevel = NULL;
#endif

#define S7 (sphere?5:7)
#define S42 (S7*6)
#define S14 (S7*2)
#define S21 (S7*3)
#define S28 (S7*4)
#define S84 (S7*12)

#include "util.cpp"
#include "hyperpoint.cpp"
#include "patterns.cpp"
#include "classes.cpp"
#include "fieldpattern.cpp"
#include "heptagon.cpp"
#include "language.cpp"
#include "hyper.h"
#include "cell.cpp"
#include "flags.cpp"
#include "yendor.cpp"
#include "complex.cpp"
#include "game.cpp"
#include "orbgen.cpp"
#include "monstergen.cpp"
#include "barriers.cpp"
#include "bigstuff.cpp"
#include "landlock.cpp"
#include "landgen.cpp"
#include "orbs.cpp"
#if CAP_INV
#include "inventory.cpp"
#else
bool inv::on;
#endif
#include "system.cpp"
#include "debug.cpp"
#include "geometry.cpp"
#include "polygons.cpp"
#include "mapeditor.cpp"
#if CAP_MODEL
#include "netgen.cpp"
#endif
#if CAP_TABFONT || CAP_CREATEFONT
#include "nofont.cpp"
#endif
#include "basegraph.cpp"
#include "help.cpp"
#include "config.cpp"
#include "scores.cpp"
#include "menus.cpp"
#include "quit.cpp"
#include "shmup.cpp"
#if CAP_ROGUEVIZ
#include "rogueviz.cpp"
#endif
#include "conformal.cpp"
#include "rug.cpp"
#include "control.cpp"
#include "hud.cpp"
#include "hypgraph.cpp"
#include "graph.cpp"
#include "sound.cpp"
#include "achievement.cpp"
#if CAP_TOUR
#include "tour.cpp"
#endif
#if ISMOBILE==0
#include <unistd.h>
#endif

bool fixseed = false;
int startseed = 0;

eLand firstland0;

void initAll() {
  showstartmenu = true;
  ca::init();
  arg::read(1);
  srand(time(NULL));
  shrand(fixseed ? startseed : time(NULL));

  achievement_init(); // not in ANDROID

  firstland0 = firstland;
  
  // initlanguage();
  initgraph();
#if CAP_SAVE
  loadsave();
#endif
  resetGeometry();
  initcells();
  
  shmup::safety = safety;
  initgame();
  restartGraph();

  if(!shmup::on) {
    restoreGolems(items[itOrbLife], moGolem); items[itOrbLife] = 0;
    restoreGolems(items[itOrbFriend], moTameBomberbird); items[itOrbFriend] = 0;
    restoreGolems(kills[moPrincessMoved], moPrincess, princess::saveHP); kills[moPrincessMoved] = 0;
    restoreGolems(kills[moPrincessArmedMoved], moPrincessArmed, princess::saveArmedHP); kills[moPrincessArmedMoved] = 0;
    }
  
  firstland = firstland0;
  }

void finishAll() {
  achievement_final(!items[itOrbSafety]);
  
#if CAP_SAVE
  saveStats();
#endif
  offscreen.clear();
  clearMemory();
#if ISMOBILE==0
  cleargraph();
#endif
  
  achievement_close();  
  }

#if ISANDROID
string buildScoreDescription() {
  string s;
  time_t timer;
  timer = time(NULL);
  char buf[128]; strftime(buf, 128, "%c", localtime(&timer));
  char buf2[128];
  
  s += XLAT("HyperRogue for Android");
  s += " ( " VER "), http://www.roguetemple.com/z/hyper/\n";
  s += XLAT("Date: %1 time: %2 s ", buf, getgametime());
  s += XLAT("distance: %1\n", its(celldist(cwt.c)));
  // s += buf2;
  if(cheater) s += XLAT("Cheats: ") + its(cheater) + "\n";
  s += XLAT("Score: ") + its(gold());

  for(int i=0; i<ittypes; i++) if(items[i]) {
    string t = XLATN(iinf[i].name);
    sprintf(buf2, " %s (%d)", t.c_str(), items[i]);
    s += buf2;
    }
  s += "\n";
  s += XLAT("Kills: ") + its(tkills());
  for(int i=1; i<motypes; i++) if(kills[i]) {
    string t = XLATN(minf[i].name);
    sprintf(buf2, " %s (%d)", t.c_str(), kills[i]);
    s += buf2;
    }
  s += "\n";

  for(int i=0; i<gamelog.size(); i++) if(gamelog[i].msg != "") s += gamelog[i].msg + "\n";
  
  return s;
  }
#endif

#if ISMOBILE==1

bool lclicked = false, clicked = false;
string lmouseovers;
bool inmenu = false;

bool longclick;

void handleScoreClick();
void openURL();
void displayTexts();
void controlMusic(int ticks);

void showHelp(MOBPAR_FORMAL, string nhelp) {
  gotoHelp(nhelp);
  }

bool useRangedOrb;

void handleclick(MOBPAR_FORMAL) {

    if(!shmup::on && andmode == 0 && size(screens) == 1 && canmove && !useRangedOrb && vid.mobilecompasssize > 0) {
      using namespace shmupballs;
      int dx = mousex - xmove;
      int dy = mousey - yb;
      int h = hypot(dx, dy);
      if(h < rad) {
        if(h < rad*SKIPFAC) movepcto(MD_WAIT);
        else {
          double d = vid.revcontrol ? -1 : 1;
          mouseh = hpxy(dx * d / rad, dy * d / rad);
          mousemovement();
          }
        getcstat = 0;
        return;
        }
      }

    if(buttonclicked || mouseout()) {
      
      if(andmode == 0 && getcstat == 'g' && !shmup::on && size(screens) == 1) {
        movepcto(MD_DROP);
        getcstat = 0;
        }
      
      else if(getcstat != SDLK_F1) {
        int px = mousex < vid.xcenter ? 0 : 1;
        int py = mousey < vid.ycenter ? 0 : 1;
        
        if(size(screens) == 1) {
          if(px == 0 && py == 1) {
            if(andmode == 0 && shmup::on) ; 
            else andmode = 10;
            }
          if(px == 1 && py == 1) {
            if(andmode == 0 && shmup::on) ; // just fire, do not change modes
            else {
              if(andmode == 1) {
                centerpc(INF);
                View = Id;
                viewctr.h = cwt.c->master;
                }
              andmode = 11;
              }
            }
          if(px == 0 && py == 0) andmode = 22;
          if(px == 1 && py == 0) andmode = 13;
          }
        }
      
      else {
        if(andmode == 0 && help != "@") {
          addMessage(mouseovers);
          showHelp(MOBPAR_ACTUAL, help);
	  andmode = 10;
          getcstat = 0;
          return;
          }
        }
      }
    
    if(andmode == 0 && size(screens) == 1 && !mouseout()) {

      bool forcetarget = longclick;
      
      if(mouseover && targetclick && targetRangedOrb(mouseover, forcetarget ? roMouseForce : roMouse)) {
        ;
        }
      else if(!forcetarget) movepcto(mousedest);
      }

    if(andmode == 10) {
      if(!playerfound) {
        centerpc(INF);
        View = Id;
        viewctr.h = cwt.c->master;
        }
      playermoved = true;
      }
      
    if(andmode >= 10) andmode -= 10;

  if(andmode == 3) {
    if(tour::on)
      showMissionScreen();
    else
      pushScreen(showMainMenu);
    andmode = 0;
    }
  }

int touchedAt;

int getticks();

#if CAP_ANDROIDSHARE
void shareScore(MOBPAR_FORMAL);
#endif

void mobile_draw(MOBPAR_FORMAL) {

  optimizeview();

  int lastt = ticks; ticks = getticks();
  if(lastt > ticks) lastt = ticks;
  int tdiff = ticks - lastt;

  if(playermoved && vid.sspeed > -4.99)
    centerpc(tdiff / 1000.0 * exp(vid.sspeed));

  if(shmup::on && (andmode == 0 || andmode == 10) && size(screens) == 1) 
    shmup::turn(tdiff);
    
  safety = false;
  vid.fsize = (min(vid.xres, vid.yres) * fontscale + 50) / 3200;
  
  mouseoh = mouseh;
  gtouched = mousepressed = clicked;

  longclick = lclicked && ticks > touchedAt + 500;
  useRangedOrb = 
    longclick || (!(vid.shifttarget & 2) && haveRangedOrb() && lmouseover && lmouseover->cpdist > 1);

  targetclick = ((vid.shifttarget & 2) && !shmup::on) ? longclick : true;

  if(shmup::on) {
    using namespace shmupballs;
    if(hypot(mousex - xfire, mousey - yb) < rad) targetclick = false;
    if(hypot(mousex - xmove, mousey - yb) < rad) targetclick = false;
    }
  
  if(cmode & sm::NORMAL) {
    lmouseover = (gtouched && lclicked) ? mouseover : NULL;
    if(!shmup::on && !useRangedOrb && vid.mobilecompasssize) {
      using namespace shmupballs;
      int dx = mousex - xmove;
      int dy = mousey - yb;
      int h = hypot(dx, dy);
      if(h < rad) {
        if(h < rad*SKIPFAC) { lmouseover = cwt.c; mousedest.d = -1; }
        else {
          double d = vid.revcontrol ? -1 : 1;
          mouseh = hpxy(dx * d / rad, dy * d / rad);
          calcMousedest();          
          }
        }
      }
    if(andmode == 0 && !useRangedOrb && gtouched && lclicked) {
      lmouseover = mousedest.d >= 0 ? cwt.c->mov[(cwt.spin + mousedest.d) % cwt.c->type] : cwt.c;
      }
    }
  mouseh = gethyper(mousex, mousey);

//  if(debfile) fprintf(debfile, "d1\n"), fflush(debfile);
  frames++;
  if(conformal::on) conformal::apply();

  if(ticks > lastt) tortoise::updateVals(ticks - lastt);

  if(clicked && !lclicked) touchedAt = ticks;  

#if CAP_XGD
  graphdata.clear();
#endif
  getcstat = 0; shiftmul = 1; getcshift = 1;
  drawscreen();
  shiftmul = getcshift;
  calcMousedest();

  inmenu = size(screens) > 1;

  if(lclicked && !clicked && !inmenu) handleclick(MOBPAR_ACTUAL);

  if(inmenu && !clicked && !lclicked) inmenu = false;
  
  bool keyreact = lclicked && !clicked;
  
  if(cmode & sm::ZOOMABLE) {
    using namespace dialog::zoom;
    if(zoomoff) {
      zoomf = 1; shiftx = shifty = 0; zoomoff = false; return; 
      }
    if(clicked && !lclicked) {
      zoomf = 3;
      }
    if(zoomf == 3) { 
      shiftx = -2*mousex;
      shifty = -2*mousey;
      }
    if(!clicked && zoomf > 1) { zoomoff = true; }
    }    
  
  if(inslider) keyreact = true;

#if CAP_ANDROIDSHARE
  if(getcstat == 's'-96 && keyreact) {
    popScreenAll();
    shareScore(MOBPAR_ACTUAL);
    }
#endif

  if(andmode == 2 && size(screens) != 1) andmode = 12;

  if((cmode & sm::NORMAL) && getcstat == '-')
    getcstat = 0;

  if(keyreact) {
    handlekey(getcstat, getcstat);
    }

  #if ISIOS
  displayTexts();
  #endif

  if(clicked && lclicked && andmode == 1 && (cmode & sm::NORMAL)) {
    if(!mouseout2() && mouseoh[2] < 50 && mouseh[2] < 50) {
      panning(mouseoh, mouseh);
      }
    }

  if(andmode == 1 && lclicked && !clicked && !inmenu && mouseover)
    performMarkCommand(mouseover);

  if(clicked && andmode == 2 && (mouseover != lmouseover || mouseovers != lmouseovers) && !inmenu) {
    addMessage(mouseovers);
    lmouseovers = mouseovers;
    }

  if(andmode == 10 && clicked != lclicked) andmode = 0;
  if(andmode == 20 && clicked != lclicked) andmode = 10;

  if(andmode == 2 && lclicked && !clicked) { 
    showHelp(MOBPAR_ACTUAL, help);
    }

  else if(andmode == 4) {
    achievement_final(false);
    }
    
  if(clicked && andmode == 12 && (mouseover != lmouseover || mouseovers != lmouseovers)) {
    addMessage(mouseovers);
    lmouseovers = mouseovers;
    }

  if(clicked != lclicked)
    flashMessages();

  // END
  lclicked = clicked;  

#if ISIOS
  controlMusic(ticks - lastt);
  #endif
  }

#endif

#if !CAP_AUDIO
void playSound(cell*, const string &s, int vol) { printf("play sound: %s vol %d\n", s.c_str(), vol); }
#endif

