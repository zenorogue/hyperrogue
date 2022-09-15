// Hyperbolic Rogue
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file hyper.cpp
 *  \brief the hyper_main function
 */

#include "hyper.h"

#if ISLINUX
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

namespace hr {
EX hookset<bool(int argc, char** argv)> hooks_main;

EX bool initialized = false;

/** convenience function for libhyper.so: initialize libhyper with given parameters */
EX int hyper_init(int argc, char **argv) {
#if ISWEB
  emscripten_get_commandline();
#elif ISMOBILE
#else
  arg::init(argc, argv);
#endif
  if(callhandlers(false, hooks_main, argc, argv)) return 0;
#if !ISWEB
  #if ISLINUX
    moreStack();
  #endif
#endif
#if CAP_COMMANDLINE
  initializeCLI();
#endif
  initAll();
#if CAP_COMMANDLINE
  arg::read(3);
  start_game();
#endif
  initialized = true;
  return 1;
  }

EX int hyper_init() {
  return hyper_init(0, nullptr);
  }

/** \brief convenience function for libhyper.so: play the game loop 
 * sample use: 
 * ./mymake -O3 -shared
 * cling -DFHS -DLINUX -I /usr/include/SDL -l libhyper.so -include hyper.h "-DRun=using namespace hr; hyper_loop();"
 * Run
 **/

EX void hyper_loop() {
  if(!initialized) hyper_init();
  quitmainloop = false;
  mainloop();
  }

EX int hyper_main(int argc, char **argv) {
  if(!hyper_init(argc, argv)) return 0;
#if !ISWEB
  if(showstartmenu && !vid.skipstart) {
    #if CAP_STARTANIM
    startanims::pick();
    #endif
    pushScreen(showStartMenu);
    }
#endif
  progress_warning();
  mainloop();
  finishAll();  
  return 0;
  }
}

#ifndef NOMAIN
int main(int argc, char **argv) {
  return hr::hyper_main(argc, argv);
  }
#endif 
