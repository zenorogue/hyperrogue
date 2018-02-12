// Hyperbolic Rogue
// This is the main file when the online version of HyperRogue is compiled with Emscripten.
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

#define ISWEB 1
#define ISMINI 1
#define CAP_AUDIO 0
#define CAP_SDLGFX 0
#define CAP_PNG 0
#define CAP_TOUR 1
#define CAP_SDLTTF 0
#define CAP_SHMUP 0
#define CAP_RUG 1
#define CAP_ORIENTATION 1
#define CAP_INV 0
#define GLES_ONLY

#ifdef FAKEWEB
void mainloopiter();
template<class A, class B, class C> void emscripten_set_main_loop(A a, B b, C c) { while(true) mainloopiter(); }
#else
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

void initweb();
void emscripten_get_commandline(int& argc, const char **& argv);

void loadCompressedChar(int &otwidth, int &otheight, int *tpix);

const char *wheresounds;

#include "hyper.cpp"

// -- demo --

bool demoanim;

void toggleanim(bool v) {
  demoanim = v;
  if(v) {
    sightrange = 4;
    vid.wallmode = 5;
    vid.particles = true;
    vid.sspeed = -1;
    vid.mspeed = -1;
    geom3::highdetail = geom3::middetail = 5;
    }
  else {
    sightrange = 7;
    vid.sspeed = 5;
    vid.mspeed = 5;
    vid.particles = false;
    vid.wallmode = 3;
    geom3::highdetail = geom3::middetail = -1;
    }
  }

void showDemo() {
  gamescreen(2);

  getcstat = ' ';
  
  dialog::init(XLAT("HyperRogue %1: online demo", VER), 0xC00000, 200, 100);
  dialog::addBreak(50);

  dialog::addItem(XLAT("play"), 'f');
  dialog::addItem(XLAT("tutorial"), 'T');
  dialog::addItem(XLAT("help"), 'h'); dialog::lastItem().keycaption += " / F1";
  // dialog::addItem(XLAT("toggle high detail"), 'a');
  dialog::addBreak(100);

  dialog::addTitle("highlights", 0xC00000, 120);
  dialog::addItem(XLAT("Temple of Cthulhu"), 't');
  dialog::addItem(XLAT("Land of Storms"), 'l');
  dialog::addItem(XLAT("Burial Grounds"), 'b');

  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    if(sym == SDLK_F1 || sym == 'h') gotoHelp(help);
    else if(sym == 'a') {
      toggleanim(!demoanim);
      popScreen();
      }
    else if(sym == 'f') {
      firstland = laIce;
      if(tactic::on) restartGame('t');
      else restartGame();
      }
#if CAP_TOUR
    else if(sym == 'T') {
      firstland = laIce;
      if(!tour::on) tour::start();
      }
#endif
    else if(sym == 't') {
      firstland = laTemple;
      if(!tactic::on) restartGame('t');
      else restartGame();
      }
    else if(sym == 'l') {
      firstland = laStorms;
      if(!tactic::on) restartGame('t');
      else restartGame();
      }
    else if(sym == 'b') {
      firstland = laBurial;
      if(!tactic::on) restartGame('t');
      else restartGame();
      items[itOrbSword] = 60;
      }
    };
  }

EM_BOOL fsc_callback(int eventType, const EmscriptenFullscreenChangeEvent *fullscreenChangeEvent, void *userData) {
  if(fullscreenChangeEvent->isFullscreen) {
    vid.xres = vid.xscr = fullscreenChangeEvent->screenWidth;
    vid.yres = vid.yscr = fullscreenChangeEvent->screenHeight;
    vid.full = true;
    printf("set to %d x %d\n", vid.xres, vid.yres);
    setvideomode();
    }
  else {
    vid.xres = vid.xres = 800;
    vid.yres = vid.yres = 600;
    vid.full = true;
    printf("reset to %d x %d\n", vid.xres, vid.yres);
    setvideomode();
    }
  return true;
  }

void initweb() {
  rug::renderonce = true;
  // toggleanim(false);
  emscripten_set_fullscreenchange_callback(0, NULL, false, fsc_callback);
  pushScreen(showDemo);
  }

transmatrix getOrientation() {
  ld alpha, beta, gamma;
  alpha = EM_ASM_DOUBLE({ return rotation_alpha; });
  beta = EM_ASM_DOUBLE({ return rotation_beta; });
  gamma = EM_ASM_DOUBLE({ return rotation_gamma; });
  return 
    rotmatrix(0, 1, alpha * M_PI / 180) *
    rotmatrix(1, 2, beta * M_PI / 180) *
    rotmatrix(0, 2, gamma * M_PI / 180);
  }

vector<string> emscripten_args;
vector<const char*> emscripten_args_c;

void emscripten_get_commandline(int& argc, const char **& argv) {
  char *str = (char*)EM_ASM_INT({
    var jsString = document.location.href;
    var lengthBytes = lengthBytesUTF8(jsString)+1;
    var stringOnWasmHeap = _malloc(lengthBytes);
    stringToUTF8(jsString, stringOnWasmHeap, lengthBytes+1);
    return stringOnWasmHeap;
    });
  string s = str;
  s += "+xxxxxx";
  for(int i=0; i<size(s); i++) if(s[i] == '?') {
    printf("Received command line arguments: %s\n", str);
    emscripten_args.push_back("hyperweb");
    string next = ""; i += 3;
    for(; i<size(s); i++)  {
      if(s[i] == '+') {
        emscripten_args.push_back(next);
        next = "";
        }
      else if(s[i] == '%') {
        string s2 = "";
        s2 += s[i+1];
        s2 += s[i+2];
        i += 2;
        next += strtol(s2.c_str(), NULL, 16);
        }
      else next += s[i];
      }
    argc = size(emscripten_args);
    for(string& s: emscripten_args) emscripten_args_c.push_back(s.c_str());
    argv = &(emscripten_args_c[0]);
    }
  free(str);
  }

