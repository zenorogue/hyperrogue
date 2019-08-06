// Hyperbolic Rogue
// Copyright (C) 2011-2018 Zeno Rogue

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

#include "init.cpp"

#if CU_HYPER

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

hr::hookset<bool(int argc, char** argv)> *hr::hooks_main;

int hyper_main(int argc, char **argv) {
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

#ifndef NOMAIN
int main(int argc, char **argv) {
  return hyper_main(argc, argv);
  }
#endif 
#endif
