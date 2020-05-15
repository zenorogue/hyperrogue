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

namespace qtm {

int mode;

color_t rcolor() {
  color_t res;
  part(res, 0) = hrand(0x80);
  part(res, 1) = hrand(256);
  part(res, 2) = hrand(0x80) + 128;
  swap(part(res, 1), part(res, rand() % 2));
  swap(part(res, 2), part(res, rand() % 3));
  return res;
  }

color_t rainbow_color(hyperpoint h) {
  ld sat = 1 - 1 / h[2];
  ld hue = atan2(h[0], h[1]) / (2 * M_PI);
  
  hue = frac(hue);
  
  if(hue < 0) hue++;
  
  hue *= 6;
  
  color_t res;
  
  if(hue<1) res = gradient(0xFF0000, 0xFFFF00, 0, hue, 1);
  else if(hue<2) res = gradient(0x00FF00, 0xFFFF00, 2, hue, 1);
  else if(hue<3) res = gradient(0x00FF00, 0x00FFFF, 2, hue, 3);
  else if(hue<4) res = gradient(0x0000FF, 0x00FFFF, 4, hue, 3);
  else if(hue<5) res = gradient(0x0000FF, 0xFF00FF, 4, hue, 5);
  else if(hue<6) res = gradient(0xFF0000, 0xFF00FF, 6, hue, 5);
  
  println(hlog, "sat = ", sat, " hue = ", hue);

  return gradient(0xFFFFFF, res, 0, sat, 1);
  }
  
void set_cell(cell *c) {
  if(hybri) {
    cell *c1 = hybrid::get_where(c).first;
    if(c1->land != laHive) hybrid::in_underlying_geometry([&] { set_cell(c1); });
    c->land = c1->land;
    c->wall = c1->wall;
    c->landparam = c1->landparam;
    c->item = itNone;
    c->monst = moNone;
    if(mode == 1) {
      if(hybrid::get_where(c).second == 0)
        c->landparam = 0xFFFFFF;
      }
    if(mode == 2) {
      if(hybrid::get_where(c).second != 0)
        c->wall = waNone;
      }
    }
  else {
    if(c->land == laHive) return;
    color_t col;
    if(hyperbolic) {
      hyperpoint h = calc_relative_matrix(c, currentmap->gamestart(), C0) * C0;
      col = rainbow_color(h);
      }
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
  else if(argis("-qtm-stripe")) {
    mode = 1;
    }
  else if(argis("-qtm-no-stripe")) {
    mode = 0;
    }
  else if(argis("-qtm-stripe-only")) {
    mode = 2;
    }
  else if(argis("-qtm")) {
    PHASEFROM(2);
    qtm_on = true;
    }

  else if(argis("-spheredemo")) {
    start_game();
    auto c = currentmap->allcells();
    for(cell* cx: c) cx->wall = waNone, cx->item = itNone, cx->land = laCanvas, cx->landparam = 0;
    c[1]->wall = waPalace;
    c[1]->land = laPalace;
    int N = isize(c);
    
    int i = 1+N/4;
    int j = 1+2*N/4 + 4;
    int k = 1+3*N/4;
    j %= N;

    c[i]->wall = waIcewall;
    c[i]->land = laIce;

    c[j]->wall = waBigTree;
    c[j]->land = laDryForest;

    c[k]->wall = waWaxWall;
    c[k]->landparam = 0xFF0000;
    }

  else if(argis("-two-way")) {
    start_game();
    cwt.at->move(0)->wall = waWaxWall;
    cwt.at->move(0)->landparam = 0xFF0000;

    cwt.at->move(6)->wall = waWaxWall;
    cwt.at->move(6)->landparam = 0xFFFF40;
    }

  else if(argis("-one-center")) {
    start_game();
    cwt.at->wall = waWaxWall;
    cwt.at->landparam = 0xFFD500;
    }

  else return 1;
  return 0;
  }



auto hooks = addHook(hooks_drawcell, 100, may_set_cell)
  + addHook(hooks_args, 100, args);

}

}
