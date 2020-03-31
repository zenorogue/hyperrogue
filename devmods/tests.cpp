#include "../hyper.h"
#include <iostream>
#include <thread>

namespace hr {

namespace tests {

int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-test-dist")) {
    start_game();
    shift(); int d = argi();
    vector<cell*> l = currentmap->allcells();
    int errors = 0;
    int unknown = 0;
    for(cell *c1: l) if(c1->cpdist <= d)
    for(cell *c2: l) if(c2->cpdist <= d) {
      int cd = celldistance(c1, c2);
      int bcd = bounded_celldistance(c1, c2);
      if(bcd == DISTANCE_UNKNOWN)
        unknown++;
      else if(cd != bcd) {
        errors++;
        println(hlog, "distance error: ", tie(c1,c2), " cd = ", cd, " bcd = ", bcd);
        }
      }

    int q = 0;
    for(cell *c: l) if(c->cpdist <= d) q++;
    
    println(hlog, "cells checked: ", q, " errors: ", errors, " unknown: ", unknown, " in: ", full_geometry_name());
    
    if(errors) exit(1);
    }
  else if(argis("-test-bt")) {
    PHASEFROM(3);
    for(int i=0; i<gGUARD; i++) {
      eGeometry g = eGeometry(i);      
      
      set_geometry(g);
      ld aer = bt::area_expansion_rate();
      if(!aer) continue;
      
      // if(cgflags & qDEPRECATED) continue;
      // if(cgflags & qHYBRID) continue;
      // if(arb::in() || arcm::in()) continue;
      // if(!(bt::in() || nonisotropic || among(geometry, gEuclidSquare, 

      println(hlog, "testing geometry: ", ginf[g].menu_displayed_name);

      start_game();      

      int co = bt::expansion_coordinate();

      int cx = (co + 1) % WDIM;
      int cy = (co + 2) % WDIM;
      auto oxy = [&] (ld x, ld y, ld z) { hyperpoint h = Hypc; h[co] = z; h[cx] = x; if(WDIM == 3) h[cy] = y; return tC0(bt::normalized_at(h)); };
      ld shrunk_x = geo_dist(oxy(0,0,-1), oxy(.01,0,-1), iTable);
      ld shrunk_y = geo_dist(oxy(0,0,-1), oxy(0,.01,-1), iTable);
      ld expand_x = geo_dist(oxy(0,0,+1), oxy(.01,0,+1), iTable);
      ld expand_y = geo_dist(oxy(0,0,+1), oxy(0,.01,+1), iTable);
      if(WDIM == 2) shrunk_y = expand_y = 1;
      println(hlog, "should be 1: ", lalign(10, (shrunk_x * shrunk_y * bt::area_expansion_rate()) / (expand_x * expand_y)), " : ", tie(shrunk_x, shrunk_y, expand_x, expand_y, aer));
      if(geometry == gArnoldCat)
        println(hlog, "(but not in Arnold's cat)");
      }
    }
  else return 1;
  return 0;
  }

auto hooks = addHook(hooks_args, 100, readArgs);
 
// Bolza:: genus 2 => Euler characteristic -2
// octagon: -2/6
// ~> 6 octagons

}
}
