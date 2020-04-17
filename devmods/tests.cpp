#include "../hyper.h"
#include <iostream>
#include <thread>

namespace hr {

namespace tests {

string test_eq(hyperpoint h1, hyperpoint h2, ld err = 1e-6) {
  if(sqhypot_d(MDIM, h1 -h2) < err)
    return "OK";
  else
    return "ERROR";
  }

string test_eq(transmatrix T1, transmatrix T2, ld err = 1e-6) {
  if(eqmatrix(T1, T2, err))
    return "OK";
  else
    return "ERROR";
  }

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
      ld shrunk_x = geo_dist(oxy(0,0,-1), oxy(.01,0,-1));
      ld shrunk_y = geo_dist(oxy(0,0,-1), oxy(0,.01,-1));
      ld expand_x = geo_dist(oxy(0,0,+1), oxy(.01,0,+1));
      ld expand_y = geo_dist(oxy(0,0,+1), oxy(0,.01,+1));
      if(WDIM == 2) shrunk_y = expand_y = 1;
      println(hlog, "should be 1: ", lalign(10, (shrunk_x * shrunk_y * bt::area_expansion_rate()) / (expand_x * expand_y)), " : ", tie(shrunk_x, shrunk_y, expand_x, expand_y, aer));
      if(geometry == gArnoldCat)
        println(hlog, "(but not in Arnold's cat)");
      }
    }
  else if(argis("-test-push")) {
    PHASEFROM(3);
    for(eGeometry g: {gSol, gNil, gCubeTiling, gSpace534, gCell120}) {
      stop_game();
      set_geometry(g);
      println(hlog, "testing geometry: ", geometry_name());
      hyperpoint h = hyperpoint(.1, .2, .3, 1);
      h = normalize(h);
      println(hlog, "h = ", h);
      println(hlog, "test rgpushxto0: ", test_eq(rgpushxto0(h) * C0, h));
      println(hlog, "test gpushxto0: ", test_eq(gpushxto0(h) * h, C0));
      println(hlog, "test inverses: ", test_eq(inverse(rgpushxto0(h)), gpushxto0(h)));
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
