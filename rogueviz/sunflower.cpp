// non-Euclidean sunflower spirals (aka golden spirals or Fibonacci spirals)
// Copyright (C) 2018 Zeno and Tehora Rogue, see 'hyper.cpp' for details

// use: commandline parameter -sunflower <quantity> <density>
// e.g.: hyper -sunflower 10000 0.01

// for spherical geometry, density is set automatically to cover the whole sphere

namespace hr {

namespace sunflower {

int qty = 100;
ld density = 1, zdensity;

static const int maxfib = 300000;

hyperpoint ps[maxfib];

hyperpoint p(int i) {
  ld step = M_PI * (3 - sqrt(5));
  return spin(i * step) * xpush(sphere ? (i+.5) * M_PI / qty : euclid ? sqrt((i+.5) * density) : acosh(1 + (i+.5) * density)) * C0;
  }

int inext[maxfib], inext2[maxfib];

const vector<int> fibs = 
  {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 46368, 75025, 121393, 196418, 317811};
  
bool sunflower_cell(cell *c, transmatrix V) {
  density = zdensity / 100;
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
    shift(); qty = argi();
    shift(); zdensity = argf() * 100;
    patterns::whichShape = '9';
    nohud = true;
    addHook(hooks_drawcell, 100, sunflower_cell);
    addHook(dialog::hooks_display_dialog, 100, [] () {
      if(current_screen_cfunction() == showMainMenu) {
        dialog::addItem("sunflower", 1001); 
        dialog::add_action([] () { dialog::editNumber(zdensity, 0, 1, .1, 1, "density", "density"); });
        }
      });
    }
  else return 1;
  return 0;
  }

auto hook = addHook(hooks_args, 100, readArgs);
 
}

}