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
EX hookset<bool(int argc, char** argv)> *hooks_main;

EX int hyper_main(int argc, char **argv) {
  using namespace hr; 
#if ISWEB
  emscripten_get_commandline();
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
#if !ISWEB
  if(showstartmenu && !vid.skipstart) {
    #if CAP_STARTANIM
    startanims::pick();
    #endif
    pushScreen(showStartMenu);
    }
#endif
  mainloop();
  finishAll();  
  profile_info();
  return 0;
  }
}

#ifndef NOMAIN
int main(int argc, char **argv) {
  return hr::hyper_main(argc, argv);
  }
#endif 
