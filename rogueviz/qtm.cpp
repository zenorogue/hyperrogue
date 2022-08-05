#include "rogueviz.h"

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

int qmode;

color_t rainbow_color_at(hyperpoint h) {
  ld sat = 1 - 1 / h[2];
  ld hue = atan2(h[0], h[1]) / (2 * M_PI);
  return rainbow_color(sat, hue);
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
    if(qmode == 1) {
      if(hybrid::get_where(c).second == 0)
        c->landparam = 0xFFFFFF;
      }
    if(qmode == 2) {
      if(hybrid::get_where(c).second != 0)
        c->wall = waNone;
      }
    }
  else {
    if(c->land == laHive) return;
    color_t col;
    if(hyperbolic) {
      hyperpoint h = calc_relative_matrix(c, currentmap->gamestart(), C0) * C0;
      col = rainbow_color_at(h);
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

bool may_set_cell(cell *c, int d, cell *from) {
  set_cell(c);
  return false;
  }

void enable() {
  rogueviz::rv_hook(hooks_cellgen, 100, may_set_cell);
  }

int args() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-qtm-stripe")) {
    qmode = 1;
    }
  else if(argis("-qtm-no-stripe")) {
    qmode = 0;
    }
  else if(argis("-qtm-stripe-only")) {
    qmode = 2;
    }
  else if(argis("-qtm")) {
    PHASEFROM(2);
    enable();
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

  else if(argis("-one-line")) {
    start_game();
    cell *c = cwt.at;
    int i = 0;
    do {
      c = c->cmove(0);
      i++;
      }
    while(c != cwt.at);
    for(int j=0; j<i; j++) {
      c->wall = waWaxWall;
      c->landparam = rainbow_color(1, j * 1. / i);
      c = c->cmove(0);
      }
    }
  else if(argis("-two-line")) {
    start_game();
    cell *c = cwt.at;
    int i = 0;
    do {
      c = c->cmove(0);
      i++;
      }
    while(c != cwt.at);
    for(int j=0; j<i; j++) {
      c->wall = waWaxWall;
      c->landparam = rainbow_color(1, j * 1. / i);
      c = c->cmove(0);
      }
    vector<cell*> a;
    for(cell *x: currentmap->allcells()) {
      bool good = true;
      forCellCM(y, x) if(y->wall == waWaxWall) good = false;
      if(good) a.push_back(x);
      }
    for(cell *x: a) {
      x->wall = waWaxWall;
      x->landparam = 0xFFFFFF;
      }
    vector<cell*> b;
    for(cell *x: a) {
      bool good = false;
      forCellCM(y, x) if(y->wall != waWaxWall) good = true;
      if(good) b.push_back(x);
      }
    for(cell *x: b) x->wall = waNone;
    }

  else return 1;
  return 0;
  }

auto hooks = 
    addHook(hooks_args, 100, args)
  + addHook_rvslides(180, [] (string s, vector<tour::slide>& v) {
      if(s != "mixed") return;
      using namespace tour;
      for(int m: {1,2}) {
        string ex = m == 1 ? " (A)" : " (B)";
        string bonus = m == 2 ? "\n\nTo make stretching more interesting, only the 'stripes' are filled now." : "";
        v.push_back(slide{
          "rotations/isometries of 2-sphere"+ex, 10, LEGAL::NONE | QUICKGEO,
          "3D engines often represent rotations as unit quaternions. These form a 3D elliptic space. What if we could fly through this space of rotations?\n\n"
          "Rotation matching the current camera position shown in the corner. Beams in the rotation space correspond to bumps on the sphere.\n\n"
          "You can also obtain a different geometry (Berger sphere) by stretching along the fibers. Press 5 to stretch."+bonus
          ,
          [m] (presmode mode) {
            setCanvas(mode, '0');
            slide_url(mode, 't', "Twitter link", "https://twitter.com/ZenoRogue/status/1166723332840001536");
            slide_url(mode, 's', "stretched Twitter link", "https://twitter.com/ZenoRogue/status/1258035231996682244");
            if(mode == pmStart) {
              set_geometry(gSphere);
              set_variation(eVariation::bitruncated);
              set_geometry(gRotSpace);
              slide_backup(rots::underlying_scale, .25);
              slide_backup(qmode, m);
              #if CAP_RAY
              slide_backup(ray::max_cells, 32768);
              slide_backup(ray::fixed_map, true);
              #endif
              slide_backup(camera_speed, .1);
              enable();
              start_game();
              }
            rogueviz::pres::non_game_slide_scroll(mode);
            if(mode == pmKey)
              edit_stretch();
            }});
        v.push_back(slide{
          "rotations/isometries of the hyperbolic plane"+ex, 10, LEGAL::NONE | QUICKGEO,
          "The hyperbolic analog of the previous slide. The space is PSL(2,R) now, which has a non-isotropic geometry. It can be represented using split quaternions.\n\n"
          "Again, press 5 to stretch."+bonus
          ,
          [m] (presmode mode) {
            setCanvas(mode, '0');
            slide_url(mode, 's', "stretched Twitter link", "https://twitter.com/ZenoRogue/status/1259143275115687936");
            if(mode == pmStart) {
              set_geometry(gKleinQuartic);
              set_variation(eVariation::bitruncated);
              set_geometry(gRotSpace);
              slide_backup(rots::underlying_scale, .25);
              slide_backup(qmode, m);
              #if CAP_RAY
              slide_backup(ray::max_cells, 32768);
              slide_backup(ray::fixed_map, true);
              slide_backup(ray::want_use, 2);
              #endif
              slide_backup(camera_speed, .1);
              enable();
              start_game();
              }
            rogueviz::pres::non_game_slide_scroll(mode);
            if(mode == pmKey)
              edit_stretch();
            }});
        }
      });

}

}
