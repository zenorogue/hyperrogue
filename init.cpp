#define VER "9.4g"
#define VERNUM 9407
#define VERNUM_HEX 0x9407

#define GEN_M 0
#define GEN_F 1
#define GEN_N 2
#define GEN_O 3

#ifdef MOBILE
#define MOBWEB
#endif

#ifdef WEB
#define MOBWEB
#define ONEGRAPH
#endif

#ifdef IOS
#define ONEGRAPH
#endif

#ifdef MOBWEB
#define NORUG
#define NOEDIT
#define NOMODEL
#endif

#ifdef MINI
#define NORUG
#define NOEDIT
#define NOMODEL
#define NOSAVE
#define NOCONFIG
#define NOTRANS
#endif


#ifdef MOBILE
#define EXTRALICENSE "\n\nHyperRogue soundtrack by Shawn Parrotte (http://www.shawnparrotte.com), under the Creative Commons BY-SA 3.0 license, http://creativecommons.org/licenses/by-sa/3.0/"
#undef XEXTRALICENSE

#ifndef FAKEMOBILE
#define SDLK_F1  (123001)
#define SDLK_F2  (123002)
#define SDLK_F3  (123003)
#define SDLK_F4  (123004)
#define SDLK_F5  (123005)
#define SDLK_F6  (123006)
#define SDLK_F7  (123007)
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
#define SDLK_DELETE (123052)
#define SDLK_DELETE (123052)
#endif

int fontscale = 100;
bool buttonclicked;
void gdpush(int t);
#endif

// desktop

#include <stdio.h>

#ifdef USE_SDL
#include <SDL/SDL.h>

#ifndef MAC
#undef main
#endif

#include <SDL/SDL_ttf.h>
#endif

#include <cmath>
#include <time.h>
#include <vector>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>

#ifdef USE_UNORDERED_MAP
#include <unordered_map>
#else
#define unordered_map map
#endif

using namespace std;

string s0;
void addMessage(string s, char spamtype = 0);

#ifdef ANDROID
FILE *debfile;
#endif

FILE *debugfile;
int debugflags;

#ifdef USE_COMMANDLINE
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
#include "landgen.cpp"
#include "orbs.cpp"
#include "system.cpp"
#include "geometry.cpp"
#include "polygons.cpp"
#include "mapeditor.cpp"
#ifndef MOBILE
#include "netgen.cpp"
#endif
#include "graph.cpp"
#include "sound.cpp"
#include "achievement.cpp"
#ifdef TOUR
#include "tour.cpp"
#endif
#ifndef MOBILE
#include <unistd.h>
#endif

bool fixseed = false;

void initAll() {
  ca::init();
  arg::read(1);
  srand(time(NULL));
  shrand(fixseed ? 0 : time(NULL));

  achievement_init(); // not in ANDROID

  eLand f = firstland;
  
  // initlanguage();
  initgraph();
#ifndef NOSAVE
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
  
  firstland = f;
  }

void finishAll() {
  achievement_final(!items[itOrbSafety]);
  
#ifndef NOSAVE
  saveStats();
#endif
  offscreen.clear();
  clearMemory();
#ifndef MOBILE
  cleargraph();
#endif
  
  achievement_close();  
  }

#ifdef ANDROID
string buildScoreDescription() {
  string s;
  time_t timer;
  timer = time(NULL);
  char buf[128]; strftime(buf, 128, "%c", localtime(&timer));
  char buf2[128];
  
  s += XLAT("HyperRogue for Android");
  s += " ("VER"), http://www.roguetemple.com/z/hyper/\n";
  s += XLAT("Date: %1 time: %2 s ", buf, its(savetime + time(NULL) - timerstart));
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

#ifdef MOBILE

bool lclicked = false, clicked = false;
string lmouseovers;
bool inmenu = false;

bool longclick;

void handleScoreClick();
void openURL();
void displayTexts();
void controlMusic(int ticks);

void showHelp(MOBPAR_FORMAL, string nhelp) {
  help = nhelp;
  // helptext = help;
  lastmode = cmode;
  cmode = emHelp;
  }

bool useRangedOrb;

void handleclick(MOBPAR_FORMAL) {

    if(!shmup::on && andmode == 0 && cmode == emNormal && canmove && !useRangedOrb && vid.mobilecompasssize > 0) {
      using namespace shmupballs;
      int dx = mousex - xmove;
      int dy = mousey - yb;
      int h = hypot(dx, dy);
      if(h < rad) {
        if(h < rad*SKIPFAC) movepcto(MD_WAIT);
        else {
          double d = revcontrol ? -1 : 1;
          mouseh = hpxy(dx * d / rad, dy * d / rad);
          mousemovement();
          }
        getcstat = 0;
        return;
        }
      }

    if(buttonclicked || outofmap(mouseh)) {
      
      if(andmode == 0 && getcstat == 'g' && !shmup::on && (cmode == emNormal || cmode == emQuit)) {
        movepcto(MD_DROP);
        getcstat = 0;
        }
      
      else if(getcstat != SDLK_F1) {
        int px = mousex < vid.xcenter ? 0 : 1;
        int py = mousey < vid.ycenter ? 0 : 1;
        
        if(cmode == (canmove ? emNormal : emQuit)) {
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
    
    if(andmode == 0 && cmode == (canmove ? emNormal : emQuit) && !outofmap(mouseh)) {

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

  if(andmode == 3) cmode = emMenu, andmode = 0;
  }

int touchedAt;

int getticks();

#ifdef ANDROIDSHARE
void shareScore(MOBPAR_FORMAL);
#endif

void mobile_draw(MOBPAR_FORMAL) {

  optimizeview();

  int lastt = ticks; ticks = getticks();
  if(lastt > ticks) lastt = ticks;
  int tdiff = ticks - lastt;

  if(playermoved && vid.sspeed > -4.99)
    centerpc(tdiff / 1000.0 * exp(vid.sspeed));

  if(shmup::on && (andmode == 0 || andmode == 10) && cmode == emNormal) 
    shmup::turn(tdiff);
    
  safety = false;
  vid.fsize = (min(vid.xres, vid.yres) * fontscale + 50) / 3200;
  
  hyperpoint mouseoh = mouseh;
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
  
  if(cmode == emNormal) {
    lmouseover = (gtouched && lclicked) ? mouseover : NULL;
    if(!shmup::on && !useRangedOrb && vid.mobilecompasssize) {
      using namespace shmupballs;
      int dx = mousex - xmove;
      int dy = mousey - yb;
      int h = hypot(dx, dy);
      if(h < rad) {
        if(h < rad*SKIPFAC) { lmouseover = cwt.c; mousedest.d = -1; }
        else {
          double d = revcontrol ? -1 : 1;
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

  graphdata.clear();
  getcstat = 0; shiftmul = 1; getcshift = 1;
  drawscreen();
  shiftmul = getcshift;
  calcMousedest();

  if(lclicked && !clicked && !inmenu) handleclick(MOBPAR_ACTUAL);

  if(inmenu && !clicked && !lclicked) inmenu = false;
  
  bool keyreact = lclicked && !clicked;
  
  if(cmode == emOverview || cmode == emTactic) {
    using namespace dialog::zoom;
    if(zoomoff || (cmode != emOverview && cmode != emTactic)) {
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

#ifdef ANDROIDSHARE
  if(getcstat == 's'-96 && keyreact) {
    cmode = canmove ? emQuit : emNormal;
    shareScore(MOBPAR_ACTUAL);
    cmode = emNormal;
    }
#endif

  if(andmode == 2 && cmode != emNormal) andmode = 12;

  if((cmode == emQuit && !canmove && keyreact && lclicked && !clicked) && !buttonclicked) {
    cmode = emNormal; printf("back to quit\n");
    }
  else if(cmode == emScores) handleScoreKeys(0);
  else if(getcstat && keyreact) {

    if(cmode == (canmove ? emQuit : emNormal)) 
      handleQuit(getcstat, getcstat);

    else {
      if(cmode != emNormal && cmode != emQuit) inmenu = true;
      if(cmode == emMenu && getcstat == 'q') openURL();
      else { extra ex; handlekey(getcstat, getcstat, ex); }
      }

    }

  #ifdef IOS
  displayTexts();
  #endif

  if((cmode != emVisual1 && cmode != emScores)) {
     
    if(clicked && lclicked && andmode == 1 && !inmenu) {
      if(!outofmap(mouseoh) && !outofmap(mouseh) && mouseoh[2] < 50 && mouseh[2] < 50) {
        panning(mouseoh, mouseh);
        }
      }

    if(andmode == 1 && lclicked && !clicked && !inmenu && mouseover)
      performMarkCommand(mouseover);

    if(clicked && andmode == 2 && (mouseover != lmouseover || mouseovers != lmouseovers) && cmode == emNormal) {
      addMessage(mouseovers);
      lmouseovers = mouseovers;
      }
  
    if(andmode == 10 && clicked != lclicked) andmode = 0;
    if(andmode == 20 && clicked != lclicked) andmode = 10;

    if(andmode == 2 && lclicked && !clicked) { 
      if(cmode == emNormal)
        showHelp(MOBPAR_ACTUAL, help);
      else if(cmode != emScores && cmode != emPickScores)
        cmode = emNormal;
      }

  else if(andmode == 4) {
    achievement_final(false);
    }
    
  if(clicked && andmode == 2 && (mouseover != lmouseover || mouseovers != lmouseovers)) {
    addMessage(mouseovers);
    showHelp(MOBPAR_ACTUAL, help);
    lmouseovers = mouseovers;
    }

    }

  if(clicked != lclicked)
    flashMessages();

  // END
  lclicked = clicked;  

#ifdef IOS
  controlMusic(ticks - lastt);
  #endif
  }

#endif
