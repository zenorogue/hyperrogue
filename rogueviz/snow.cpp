#include "../hyper.h"

/** \brief Snowball visualization
 *
 *  This visualization puts small objects ('snowballs') randomly throughout the space. 
 *  It provides a way to visualize the geometry without any tessellation.
 *
 *  Should work for tessellations where every tile is congruent.
 *
 *  The snow_lambda parameter gives the expected number of snowballs per cell.
 *  (The number in every region has Poisson distribution with mean proportional to its area.)
 *
 *  Not implemented for: product
 *
 **/

namespace hr {

ld snow_lambda = 1;

bool snow_test = false;

/* a funny glitch */
bool snow_glitch = false;

int snow_shape = 0;

map<cell*, vector<transmatrix> > matrices_at;

hpcshape& shapeid(int i) {
  switch(i) {
    case 0: 
      return cgi.shSnowball;
    case 1:
      return cgi.shHeptaMarker;
    case 2:
      return cgi.shDisk;
    default:
      return cgi.shDisk;
    }
  }

bool draw_snow(cell *c, const transmatrix& V) {
  
  if(!matrices_at.count(c)) {
    auto& v = matrices_at[c];
    int cnt = 0;
    ld prob = randd();
    ld poisson = exp(-snow_lambda);
    while(cnt < 2*snow_lambda+100) {
      if(prob < poisson) break;
      prob -= poisson;
      cnt++;
      poisson *= snow_lambda / cnt;
      }
    if(snow_test) {
      if(c != cwt.at) 
        cnt = 0;
      else {
        c->wall = waFloorA;
        cnt = snow_lambda;
        }
      }      
    
    if(snow_glitch) {
      // in the standard tiling, this is incorrect but fun
      for(int t=0; t<cnt; t++) {
        hyperpoint h = C0;
        h[0] = randd() - .5;
        h[1] = randd() - .5;
        h[2] = randd() - .5;
        h[2] = -h[2];
        v.push_back(rgpushxto0(h));
        }
      }
    else if(nonisotropic || bt::in()) {

      int co = bt::expansion_coordinate();
      ld aer = bt::area_expansion_rate();

      for(int t=0; t<cnt; t++) {
        hyperpoint h;
        // randd() - .5;
        
        for(int a=0; a<3; a++) {
          if(a != co || aer == 1)
            h[a] = randd() * 2 - 1;
          else {
            ld r = randd();
            h[co] = log(lerp(1, aer, r)) / log(aer) * 2 - 1;
            }
          }
        h = tC0(bt::normalized_at(h));
        v.push_back(rgpushxto0(h));
        }
      }
    else {
      while(isize(v) < cnt) {
        ld maxr = WDIM == 2 ? cgi.rhexf : cgi.corner_bonus;
        ld vol = randd() * wvolarea_auto(maxr);
        ld r = binsearch(0, maxr, [vol] (ld r) { return wvolarea_auto(r) > vol; });
        transmatrix T = random_spin();
        hyperpoint h = T * xpush0(r);
        cell* c1 = c;
        virtualRebase(c1, h);
        if(c1 == c)
          v.push_back(T * xpush(r));
        }
      }
    }
  
  poly_outline = 0xFF;
  for(auto& T: matrices_at[c])
    queuepoly(V * T, shapeid(snow_shape), 0xFFFFFFFF).tinf = nullptr;

  return false;
  }

bool cylanim = false;

auto hchook = addHook(hooks_drawcell, 100, draw_snow)

+ addHook(clearmemory, 40, [] () {
    matrices_at.clear();
    })

+ addHook(hooks_args, 100, [] {
  using namespace arg;
           
  if(0) ;
  else if(argis("-snow-lambda")) {
    shift_arg_formula(snow_lambda);
    }
  else if(argis("-snow-shape")) {
    shift(); snow_shape = argi();
    }
  else if(argis("-snow-test")) {
    snow_test = true;
    }
  else if(argis("-snow-glitch")) {
    snow_test = true;
    }
  else return 1;
  return 0;
  });

}
