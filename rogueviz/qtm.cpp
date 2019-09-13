#include "../hyper.h"

// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/*

3D engines often represent rotations as unit quaternions. These form a three-dimensional elliptic space. What if we could fly through it?

Lower left corner shows the rotation matching the current camera position. Beams in the rotation space correspond to bumps on the sphere.

Run like:
[hyper] -geo 2 -rotspace -rot_uscale 0.25 -qtm (space of rotations of the sphere)
[hyper] -rotspace -rot_uscale 0.25 -qtm (space of rotations of the hyperbolic plane, slow)
[hyper] -geo Nil -qtm (Nil, which is not a space of rotations, but similar; rendered in a similar way)

*/

namespace hr {

namespace hybrid { extern hrmap *pmap; }

color_t rcolor() {
  color_t res;
  part(res, 0) = hrand(0x80);
  part(res, 1) = hrand(256);
  part(res, 2) = hrand(0x80) + 128;
  swap(part(res, 1), part(res, rand() % 2));
  swap(part(res, 2), part(res, rand() % 3));
  return res;
  }
  
void set_cell(cell *c) {
  if(hybri) {
    cell *c1 = hybrid::get_where(c).first;
    if(c1->land != laHive) hybrid::in_underlying_map([&] { set_cell(c1); });
    c->land = c1->land;
    c->wall = c1->wall;
    c->landparam = c1->landparam;
    c->item = itNone;
    c->monst = moNone;
    }
  else {
    if(c->land == laHive) return;
    color_t col;
    if(hyperbolic)
      col = rcolor();
    else if(nil) {
      part(col, 0) = 128 + c->master->zebraval * 50;
      part(col, 1) = 128 + c->master->emeraldval * 50;
      part(col, 2) = 255;
      }
    else {
      hyperpoint h = calc_relative_matrix(c, currentmap->gamestart(), C0) * C0;
      part(col, 0) = (h[0] * 127.5 + 127.5);
      part(col, 1) = (h[1] * 127.5 + 127.5);
      part(col, 2) = (h[2] * 127.5 + 127.5);
      }
    c->landparam = col;
    c->land = laHive;
    c->wall = (nil ? (c->master->zebraval & c->master->emeraldval & 1) : pseudohept(c)) ? waWaxWall : waNone;
    c->item = itNone;
    c->monst = moNone;
    }
  }

bool qtm_on;

bool may_set_cell(cell *c, const transmatrix& T) {
  if(qtm_on) set_cell(c);
  return false;
  }

int args() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-qtm")) {
    PHASEFROM(2);
    qtm_on = true;
    }

  else return 1;
  return 0;
  }



auto magichook = addHook(hooks_drawcell, 100, may_set_cell)
  + addHook(hooks_args, 100, args);

}
