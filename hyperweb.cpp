#define WEB
#define MOBWEB
#define MINI
#define NOAUDIO
#define NOGFX
#define NOPNG
#define DEMO
#define TOUR
#define NOTTF

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

void playSound(cell *c, const string& fname, int vol) { }

void initweb() {
  toggleanim(false);
  cmode = emMenu;
  }

