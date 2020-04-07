// non-Euclidean sunflower spirals (aka golden spirals or Fibonacci spirals)
// Copyright (C) 2018 Zeno and Tehora Rogue, see 'hyper.cpp' for details

// use: commandline parameter -sunflower <quantity> <density>
// e.g.: hyper -sunflower 10000 0.01
// e.g.: hyper -sunflower 5 0.01

#include "rogueviz.h"

namespace rogueviz {

namespace sunflower {

bool on;

int qty = 100;
ld density = 1, zdensity;
ld range;

/* which property to infer from the other two: 'd'ensity, 'q'ty or 'r'ange */
char infer;

vector<hyperpoint> ps;

hyperpoint p(int i) {
  ld step = M_PI * (3 - sqrt(5));
  return spin(i * step) * xpush(sphere ? (i+.5) * density : euclid ? sqrt((i+.5) * density) : acosh(1 + (i+.5) * density)) * C0;
  }

vector<int> inext, inext2;

vector<int> fibs = {1, 2};
  
bool sunflower_cell(cell *c, transmatrix V) {
  if(!on) return false;
  density = zdensity / 100;
  
  ld qd;
  
  if(sphere) {
    if(infer == 'r') 
      range = qty * density;
    else qd = range;
    }
  else if(euclid) {
    if(infer == 'r')
      range = sqrt(qty * density);
    else qd = range * range;
    }
  else {
    if(infer == 'r')
      range = acosh(1 + qty * density);
    else 
      qd = (cosh(range) - 1);
    }
  
  if(infer == 'q') qty = qd / density;
  if(infer == 'd') density = qd / qty;
  
  ps.resize(qty);
  inext.resize(qty);
  inext2.resize(qty);
  while(fibs.back() < qty) {
    auto add = fibs.back() + *(fibs.end()-2);
    fibs.push_back(add);
    }
  
  if(c == cwt.at) {
    for(int i=0; i<qty; i++) ps[i] = V * p(i);

    for(int i=0; i<qty; i++) {
      ld ba = 99;
      ld bb = 99;
      int bi = 0, bj = 0;
      for(int a: fibs) {
        if(a>i) break;
        if(hdist(ps[i], ps[i-a]) < ba)
          bb = ba, bj = bi, ba = hdist(ps[i], ps[i-a]), bi = i-a;
        else if(hdist(ps[i], ps[i-a]) < bb)
          bb = hdist(ps[i], ps[i-a]), bj = i-a;
        }
      inext[i] = bi;
      inext2[i] = bj;
      }

    for(int i=0; i<qty; i++) {    
      if(inext[inext[i]] == inext2[i] || inext2[inext[i]] == inext2[i] || inext[inext2[i]] == inext[i] || inext2[inext2[i]] == inext[i]) {
        curvepoint(ps[i]);
        curvepoint(ps[inext[i]]);
        curvepoint(ps[inext2[i]]);
        queuecurve(0xFFFFFFFF, 0x00C000FF, PPR::LINE);
        }      
      else {
        curvepoint(ps[i]);
        curvepoint(ps[inext[i]]);
        curvepoint(ps[inext[i] + inext2[i] - i]);
        curvepoint(ps[inext2[i]]);
        queuecurve(0xFFFFFFFF, 0x0000C0FF, PPR::LINE);
        }
      }
    }
  return true;
  }

int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-sunflower")) {
    on = true;
    infer = 'r';
    shift(); qty = argi();    
    shift(); zdensity = argf() * 100;
    patterns::whichShape = '9';
    nohud = true;
    }
  else if(argis("-sunflower-qr")) {
    on = true;
    infer = 'd';
    shift(); qty = argi();    
    shift(); range = argf();
    patterns::whichShape = '9';
    nohud = true;
    }
  else if(argis("-sunflower-dr")) {
    on = true;
    infer = 'q';
    shift(); density = argi();    
    shift(); range = argf();
    patterns::whichShape = '9';
    nohud = true;
    }
  else return 1;
  return 0;
  }

ld distance_per_rug;

bool adjust_rug;

named_functionality o_key() {
  if(on) return named_functionality("sunflower density", [] { 
    dialog::editNumber(zdensity, 0, 1, .1, 1, "density", "density");
    dialog::reaction = [] {
      if(adjust_rug) 
        rug::model_distance = sqrt(zdensity) * distance_per_rug;
      else
        distance_per_rug = rug::model_distance / sqrt(zdensity);
      };
    distance_per_rug = rug::model_distance / sqrt(zdensity);
    dialog::extra_options = [] {
      dialog::addBoolItem_action("auto-adjust the Rug", adjust_rug, 'A');
      };
    });
  return named_functionality();
  }

auto hook = 0
#if CAP_COMMANDLINE
+ addHook(hooks_args, 100, readArgs)
#endif
+ addHook(hooks_o_key, 80, o_key)
+ addHook(hooks_drawcell, 100, sunflower_cell)
+ addHook(rvtour::hooks_build_rvtour, 144, [] (vector<tour::slide>& v) {
  using namespace tour;
  v.push_back(
    tour::slide{"unsorted/sunflower spirals", 18, LEGAL::ANY | QUICKGEO, 
      "A sunflower sends out its n-th seed at angle 180° (3-sqrt(5)). "
      "As new seeds are created, older seeds are pushed out. Therefore. "
      "the distance d(n) of the n-th seed from the center will be such that "
      "the area of a circle of radius d(n) changes linearly with n.\n\n"
      "In the Euclidean plane, this process creates an interesting "
      "phenomenon: if we try to compute the number of spirals at a given "
      "distance from the center, we usually obtain a Fibonacci number. "
      "The further from the start we are, the larger Fibonacci number we "
      "get.\n\n"
      "Because of the exponential growth in the hyperbolic plane, we "
      "get to larger Fibonacci numbers faster.\n\n"
      
      "Press 123 to change the geometry, 5 to see this in the Hypersian Rug model. "
      "Press o to change the density.",
   
  [] (presmode mode) {
    setCanvas(mode, '0');
    
    if((mode == pmStop || mode == pmGeometry) && rug::rugged) rug::close();
    
    if(mode == pmKey) {
      if(rug::rugged) rug::close();
      else rug::init();
      }
    
    if(mode == pmStart) {
      stop_game();
      
      tour::slide_backup(on, true);
      tour::slide_backup(range, sphere ? 2 * M_PI : euclid ? 10 : 6);
      tour::slide_backup<ld>(zdensity, 1);
      tour::slide_backup(infer, 'q');

      start_game();
      }
    }}
    );
  });
 
}

}