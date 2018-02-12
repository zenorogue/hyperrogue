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

hookset<bool(int argc, const char** argv)> *hooks_main;

#ifndef NOMAIN
int main(int argc, const char **argv) {
#if ISWEB
  emscripten_get_commandline(argc, argv);
#endif
  if(callhandlers(false, hooks_main, argc, argv)) return 0;
#if !ISWEB
  #if ISLINUX
    moreStack();
  #endif
#endif
#if CAP_COMMANDLINE
  arg::init(argc, argv);
  initializeCLI();
#endif
  initAll();
#if CAP_COMMANDLINE
  arg::read(3);
#endif
#if !ISWEB
  if(showstartmenu)
    pushScreen(showStartMenu);
#endif
  mainloop();
  finishAll();  
  profile_info();
  return 0;
  }
#endif 
#endif
