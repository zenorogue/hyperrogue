// Hyperbolic Rogue
// This is the main file when the online version of HyperRogue is compiled with Emscripten.
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

#ifndef ISWEB
#define ISWEB 1
#define ISMINI 0
#define CAP_INV 0
#define CAP_URL 1
#define GLES_ONLY
#define SCU
#endif

#if CAP_ROGUEVIZ || defined(MAXMDIM)
#define MAXMDIM 4
#else
#define MAXMDIM 3
#endif

// we want newconformist, but we don't want CAP_GD there
#define CAP_NCONF 1
#define CAP_GD 0

#ifndef EMSCRIPTEN
#define EMSCRIPTEN
#endif

#ifdef FAKEWEB
namespace hr { void mainloopiter(); }
template<class A, class B, class C> void emscripten_set_main_loop(A a, B b, C c) { while(true) mainloopiter(); }
#else
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <string>

namespace hr { 
  void initweb();
  void emscripten_get_commandline();

  void loadCompressedChar(int &otwidth, int &otheight, int *tpix);
  void get_canvas_size();

  const char *wheresounds;

  std::string get_value(std::string name);

  void offer_download(std::string sfilename, std::string smimetype);
  }

#ifdef SCU
#include "hyper.cpp"
#else
#include "hyper.h"
#endif

namespace hr {

string get_value(string name) {
  char *str = (char*)EM_ASM_INT({
    var name = UTF8ToString($0, $1);
    var value = document.getElementById(name).value;
    var lengthBytes = lengthBytesUTF8(value)+1;
    var stringOnWasmHeap = _malloc(lengthBytes);
    stringToUTF8(value, stringOnWasmHeap, lengthBytes);
    return stringOnWasmHeap;
    }, name.c_str(), int(name.size())
    );
  string res = str;
  free(str);
  return res;
  }

EX void offer_download(string sfilename, string smimetype) {

  EM_ASM({
    var name = UTF8ToString($0, $1);
    var mime = UTF8ToString($2, $3);

    let content = Module.FS.readFile(name);
    console.log(`Offering download of "${name}", with ${content.length} bytes...`);
  
    var a = document.createElement('a');
    a.download = name;
    a.href = URL.createObjectURL(new Blob([content], {type: mime}));
    a.style.display = 'none';

    document.body.appendChild(a);
    a.click();
    setTimeout(() => {
      document.body.removeChild(a);
      URL.revokeObjectURL(a.href);
    }, 2000);
    }, sfilename.c_str(), isize(sfilename), smimetype.c_str(), isize(smimetype)
    );

  }

reaction_t on_use_file = [] {};

extern "C" {
  void use_file() { 
    on_use_file();
    }
  }

EX void offer_choose_file(reaction_t r) {
  on_use_file = r;
  EM_ASM({
    fileElem.click();
    });
  }

//    window.open(Pointer_stringify($0));
bool demoanim;

void toggleanim(bool v) {
  demoanim = v;
  if(v) {
    sightrange_bonus = -3;
    vid.wallmode = 5;
    vid.particles = true;
    vid.sspeed = -1;
    vid.mspeed = -1;
    vid.highdetail = vid.middetail = 5;
    }
  else {
    sightrange_bonus = 0;
    vid.sspeed = 5;
    vid.mspeed = 5;
    vid.particles = false;
    vid.wallmode = 3;
    vid.highdetail = vid.middetail = -1;
    }
  }

void showDemo() {
  cmode = sm::DARKEN;
  gamescreen();

  getcstat = ' ';
  
  dialog::init(XLAT("HyperRogue %1: online demo", VER), 0xC00000, 200, 100);
  dialog::addBreak(50);

  dialog::addItem(XLAT("play the game"), 'f');
  dialog::addItem(XLAT("learn about hyperbolic geometry"), 'T');
  dialog::addHelp(); 
  // dialog::addItem(XLAT("toggle high detail"), 'a');
  dialog::addBreak(100);

  dialog::addTitle("highlights", 0xC00000, 120);
  dialog::addItem(XLAT("Temple of Cthulhu"), 't');
  dialog::addItem(XLAT("Land of Storms"), 'l');
  dialog::addItem(XLAT("Burial Grounds"), 'b');

  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    if(sym == SDLK_F1 || uni == 'h') gotoHelp(help);
    else if(uni == 'a') {
      toggleanim(!demoanim);
      popScreen();
      }
    else if(uni == 'f') {
      firstland = laIce;
      restart_game(tactic::on ? rg::tactic : rg::nothing);
      }
#if CAP_TOUR
    else if(uni == 'T') {
      firstland = laIce;
      if(!tour::on) tour::start();
      }
#endif
    else if(uni == 't') {
      firstland = laTemple;
      restart_game(tactic::on ? rg::tactic : rg::nothing);
      }
    else if(uni == 'l') {
      firstland = laStorms;
      restart_game(tactic::on ? rg::tactic : rg::nothing);
      }
    else if(uni == 'b') {
      firstland = laBurial;
      restart_game(tactic::on ? rg::tactic : rg::nothing);
      items[itOrbSword] = 60;
      }
    };
  }

int bak_xres, bak_yres;

EM_BOOL fsc_callback(int eventType, const EmscriptenFullscreenChangeEvent *fullscreenChangeEvent, void *userData) {
  if(fullscreenChangeEvent->isFullscreen) {
    bak_xres = vid.xres;
    bak_yres = vid.yres;
    vid.xres = vid.xscr = fullscreenChangeEvent->screenWidth;
    vid.yres = vid.yscr = fullscreenChangeEvent->screenHeight;
    vid.full = true;
    printf("set to %d x %d\n", vid.xres, vid.yres);
    setvideomode();
    }
  else {
    vid.xres = vid.xscr = bak_xres;
    vid.yres = vid.yscr = bak_yres;
    vid.full = true;
    printf("reset to %d x %d\n", vid.xres, vid.yres);
    setvideomode();
    }
  return true;
  }

EX void get_canvas_size() {
  vid.xscr = vid.xres = EM_ASM_INT({
    var d = document.getElementById("this_wide");
    if(!d) return window.innerWidth;
    return d.clientWidth;
    });
  if(vid.full) vid.yscr = vid.yres = EM_ASM_INT({
    var d = document.getElementById("this_wide");
    if(!d) return window.innerWidth;
    return d.clientHeight;
    });
  else {
    vid.xscr = vid.xres = vid.xres - 32;
    vid.yscr = vid.yres = EM_ASM_INT({
      return window.innerHeight;
      }) - 32;
    vid.yscr = vid.yres = min(vid.yscr, vid.xscr * 9 / 16);
    vid.xscr = vid.xres = min(vid.xscr, vid.yscr * 16 / 9);    
    }
  println(hlog, "X = ", vid.xscr, " Y = ", vid.yscr);
  }
  
EM_BOOL resize_callback(int eventType, const EmscriptenUiEvent *resizeEvent, void *userData) {
  if(vid.full) return true;
  get_canvas_size();
  setvideomode();
  return true;
  }

EX void initweb() {
  // toggleanim(false);
  emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, false, fsc_callback);
  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, false, resize_callback);
  printf("showstartmenu = %d\n", showstartmenu);
  if(showstartmenu) pushScreen(showDemo);
  }

#if CAP_ORIENTATION
transmatrix getOrientation() {
  ld alpha, beta, gamma;
  alpha = EM_ASM_DOUBLE({ return rotation_alpha; });
  beta = EM_ASM_DOUBLE({ return rotation_beta; });
  gamma = EM_ASM_DOUBLE({ return rotation_gamma; });
  return 
    cspin(0, 1, alpha * degree) *
    cspin(1, 2, beta * degree) *
    cspin(0, 2, gamma * degree);
  }
#endif

EX void emscripten_get_commandline() {
#ifdef EMSCRIPTEN_FIXED_ARG
  string s = EMSCRIPTEN_FIXED_ARG;
#else
  char *str = (char*)EM_ASM_INT({
    var jsString = document.location.href;
    if (typeof(default_arg) != 'undefined' && jsString.indexOf('?') == -1)
      jsString = default_arg;
    var lengthBytes = lengthBytesUTF8(jsString)+1;
    var stringOnWasmHeap = _malloc(lengthBytes);
    stringToUTF8(jsString, stringOnWasmHeap, lengthBytes+1);
    return stringOnWasmHeap;
    });
  string s = str;
#endif
  s += "+xxxxxx";
  for(int i=0; i<isize(s); i++) if(s[i] == '?') {
#ifndef EMSCRIPTEN_FIXED_ARG
    printf("HREF: %s\n", str);
#endif
    arg::argument.push_back("hyperweb"); arg::lshift();
    string next = ""; i += 3;
    for(; i<isize(s); i++)  {
      if(s[i] == '+') {
        arg::argument.push_back(next);
        next = "";
        }
      else if(s[i] == '%') {
        string s2 = "";
        s2 += s[i+1];
        s2 += s[i+2];
        i += 2;
        next += strtol(s2.c_str(), NULL, 16);
        }
      else if(s[i] == '&') { 
        arg::argument.push_back(next); break; 
        }
      else next += s[i];
      }
    printf("Arguments:"); for(string s: arg::argument) printf(" %s", s.c_str()); printf("\n");
    break;
    }

#ifndef EMSCRIPTEN_FIXED_ARG
  free(str);
#endif
  }
}

