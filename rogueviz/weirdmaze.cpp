#include "rogueviz.h"

// a quick generator for the map used here: https://twitter.com/ZenoRogue/status/1423663341944377345

// options used:

// https://twitter.com/ZenoRogue/status/1423663341944377345
// -fix -ch -noplayer -smart 1 -symbol 3^7 -7 -canvas-random 0 -weirdmaze 2 -ray-reflect .1 -switch-fpp -shot-fhd -ray-do -ray-cells 16384 -gfake 6 depth=.25 wall_height=.5 -smoothcam

// https://twitter.com/ZenoRogue/status/1423663397401501698
// -fix -ch -noplayer -smart 1 -symbol 3^7 -7 -canvas-random 0 -weirdmaze 2 -ray-reflect .1 -switch-fpp -shot-fhd -ray-do -ray-cells 16384 -gfake 6 depth=1 wall_height=1 -smoothcam -ray-dont -gfake 7 -smoothcam "fixed_YZ=0" "draw_sky=0" -tstep 16 -noshadow -shot-1000 -run wall_height=2 -run wall_height=1..2 -animvideo 600 animated-fpp.mp4

// https://twitter.com/ZenoRogue/status/1423663473377038341
// -fix -ch -noplayer -smart 1 -symbol 3^7 -7 -canvas-random 0 -weirdmaze 2 -ray-reflect .1 -switch-fpp -ray-do -ray-cells 16384 -gfake 6.9 depth=.25 wall_height=.5 -run -gfake 6.1 -run -gfake "7../0..6.01../0..|6.01../0..7../0" "depth=fake_edgelength" "wall_height=2*fake_edgelength" -shot-fhd -fov 110 -run -animvideo 600 animated-curvature.mp4

// most other scenes change the "weirdmaze" parameter (the number of filles tiles at every corner)

// (includes manual movement)


namespace rogueviz {

namespace weirdmaze {

int atleast = 1;

set<cell*> visited;

bool restrict(cell *c, const shiftmatrix& V) {
  if(visited.count(c)) return false;
  
  visited.insert(c);
  
  for(int i=0; i<c->type; i++) {
    cellwalker cw(c, i);
    cellwalker cw0 = cw;
    
    int q = 0;
    int walls = 0;
    vector<cellwalker> unknown;
    do {
      q++;
      cw += wstep;
      cw++;
      if(cw.at->wall == waWaxWall) walls++;
      if(!visited.count(cw.at)) unknown.push_back(cw);
      }
    while(cw != cw0);
    
    if(walls == 0 && isize(unknown) == atleast) {
      if(atleast == 2) {
        cw = unknown[0];
        cw0 = cw;
        do {
          visited.insert(cw.at);
          cw.at->wall = waWaxWall;
          cw += wstep;
          cw--;
          }
        while(cw != cw0);
        }
      if(atleast > 2) {
        cw = unknown[0];
        vector<cellwalker> lst;
        for(int i=0; i<200; i++) {
          lst.push_back(cw);
          if(gmod(i, atleast-1) == (atleast-2)) {
            cw += wstep;
            cw--;
            }
          else {
            cw += wstep;
            cw++;
            }
          }
        cw = unknown[0];
        for(int i=-1; i>=-200; i--) {
          if(gmod(i, atleast-1) == (atleast-2)) {
            cw++;
            cw += wstep;
            }
          else {
            cw--;
            cw += wstep;
            }
          lst.push_back(cw);
          }
        int fil = 0;
        for(auto w: lst) if(visited.count(w.at)) fil++;
        if(fil == 0) {
          for(auto cw: lst) {
            visited.insert(cw.at);
            for(int i=0; i<3; i++)
              visited.insert((cw+i).cpeek());
            cw.at->wall = waWaxWall;
            }
          }
        }
      if(atleast == 1) {
        unknown[0].at->wall = waWaxWall;
        println(hlog, "added at ", unknown[0].at, " from ", c);
        visited.insert(unknown[0].at);
        }
      }    
    }
  
  return false;
  }

void enable() {
  using namespace arg;
  shift();
  atleast = argi();
  visited.clear();
  rv_hook(hooks_drawcell, 100, restrict);
  }
  
auto plyhook = 
  arg::add3("-weirdmaze", enable);



}
}
