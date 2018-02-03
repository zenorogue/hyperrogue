#define ISWEB 1
#define ISMINI 1
#define CAP_AUDIO 0
#define CAP_SDLGFX 0
#define CAP_PNG 0
#define CAP_TOUR 1
#define CAP_SDLTTF 0
#define CAP_SHMUP 0
#define CAP_RUG 1

#ifdef FAKEWEB
void mainloopiter();
template<class A, class B, class C> void emscripten_set_main_loop(A a, B b, C c) { while(true) mainloopiter(); }
#else
#include <emscripten.h>
#endif

void initweb();

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
  dialog::addItem(XLAT("toggle high detail"), 'a');
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

void initweb() {
  toggleanim(false);
  // pushScreen(showDemo);
  }

