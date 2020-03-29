// Hyperbolic Rogue
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

// Hyperbolic geometry is a good tool to visualize data,
// especially trees and scale-free networks. This file
// uses HyperRogue to browse such vizualizations.

// Since it is not of use for general HyperRogue players, 
// it is disabled by default -- compile with the ROGUEVIZ flag to enable this.

// How to use:

// hyper -embed <start of filename> -- visualize a social network
//   embedded into hyperbolic plane, see:
//   https://bitbucket.org/HaiZhung/hyperbolic-embedder/overview
//   (it uses the same format)

// hyper -tess <parameter file> -- visualize a horocyclic tesselation,

#include "rogueviz.h"

namespace rogueviz {

namespace fullnet {

int fullnet_id;

void drawExtra() {
  
  if(vizid == &fullnet_id) {
    for(map<cell*, transmatrix>::iterator it = gmatrix.begin(); it != gmatrix.end(); it++) {
      cell *c = it->first;
      c->wall = waChasm;
      }
    int index = 0;

    for(map<cell*, transmatrix>::iterator it = gmatrix.begin(); it != gmatrix.end(); it++) {
      cell *c = it->first;
      bool draw = true;
      for(int i=0; i<isize(named); i++) if(named[i] == c) draw = false;
      if(draw && gmatrix.count(c))
        queuedisk(it->second, dftcolor, false, NULL, index++);
        // queuepolyat(it->second, shDisk, dftcolor., PPR::LINE);
      }
    
    for(int i=0; i<isize(named); i++) if(gmatrix.count(named[i])) {
      string s = ""; s += 'A'+i;
      queuestr(gmatrix[named[i]], 1, s, forecolor, 1);
      }
    
    canmove = true; items[itOrbAether] = true;
    }
  }

auto hooks =
  addHook(hooks_frame, 0, drawExtra) +
  addHook(hooks_args, 100, [] {
    using namespace arg;
    if(argis("-net")) {
      PHASE(3);
      init(&fullnet_id, 0);
      linepatterns::patTriTree.color = 0x30;
      linepatterns::patTriOther.color = 0x10;
      linepatterns::patTriRings.color = 0xFF;
      return 0;
      }
    return 1;
    });

}}
