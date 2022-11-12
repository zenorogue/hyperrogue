#include "../hyper.h"

// a very simple example: render a triangle in S3 using HyperRogue engine

// compile with: mymake rogueviz/sample.cpp

// run HyperRogue with: ./hyper -geo 120c -canvas 0 -noplayer
// press End to move forward (initially the camera is in the center of the triangle so you don't see it)

// (120c sets spherical geometry (120-cell) and -canvas 0 makes the world empty)

namespace hr {

bool draw_simple_scene() {

  shiftmatrix Where = ggmatrix(currentmap->gamestart());

  curvepoint(xpush0(0.2));
  curvepoint(spin(120._deg) * xpush0(0.2));
  curvepoint(spin(240._deg) * xpush0(0.2));
  curvepoint(xpush0(0.2));
  
  queuecurve(Where, 0xFF0000FF, 0x00FF00FF, PPR::LINE);
  
  return false;
  }

auto plyhook = addHook(hooks_frame, 100, draw_simple_scene);

}
