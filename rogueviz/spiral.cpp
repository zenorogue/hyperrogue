// Hyperbolic Rogue
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

// example commandline:
// -spiral 2,10000 -spiraledge 0,2 -spiraledge 1,1 -lab -spiralcolor 2 FF4040FF

#include "rogueviz.h"

namespace rogueviz {

namespace spiral {

  ld mul;
  
  transmatrix at(double d) {
    return spin(log(d) * TAU / log(mul)) * xpush(log(d));
    }

  void place(int N, ld _mul) {
    mul = _mul;
    init(RV_GRAPH | RV_HAVE_WEIGHT | RV_INVERSE_WEIGHT);
    rv_hook(hooks_alt_edges, 100, spiral_altedge);
    rv_hook(hooks_frame, 0, drawExtra);
    weight_label = "extent";
    vdata.resize(N);
  
    for(int i=0; i<N; i++) {
      vertexdata& vd = vdata[i];
      
      double d = i + 1;
      
      transmatrix h = at(d);
  
      createViz(i, cwt.at, h);
      vd.name = its(i+1);
      virtualRebase(vd.m);
  
      vd.cp = dftcolor;
      }
  
    storeall();
    }
  
  void edge(ld shift, ld mul) {
    int N = isize(vdata);
    auto t = add_edgetype(fts(shift)+" " + fts(mul));
    t->visible_from = 1. / (N+.5);
    for(int i=0; i<N; i++) {
      int i0 = i+1;
      int j0 = int(i0 * mul + shift) - 1;
      if(j0 >= 0 && j0 < N) addedge(i, j0, 1/(i+1), false, t);
      }
    }
  
  void color(ld start, ld period, colorpair c) {
    int N = isize(vdata);
    int maxw = N;
    while(start >= 0 && start < N) {
      int i = int(start);
      vdata[i].cp = c;
      start += period;
      maxw--; if(maxw <= 0) break;
      }
    }
  }

bool spiral_altedge(edgeinfo* ei, bool store) {
  bool onspiral = abs(ei->i - ei->j) == 1;

  if(onspiral) {
    const int prec = 20; 
    if(store) ei->orig = currentmap->gamestart();
    transmatrix T = ggmatrix(currentmap->gamestart());
    hyperpoint l1 = tC0(spiral::at(1+ei->i));
    if(!store) l1 = T * l1;
    for(int z=1; z<=prec; z++) {
      hyperpoint l2 = tC0(spiral::at(1+ei->i+(ei->j-ei->i) * z / (prec+.0)));
      if(!store)
        queueline(l1, T*l2, col, vid.linequality).prio = PPR::STRUCT0;
      else
        storeline(ei->prec, l1, l2);
      l1 = l2;
      }
    return true;
    }
  
  return false;
  }

auto hooks =
  addHook(hooks_args, 100, [] {
    using namespace arg;

    if(argis("-spiral")) {
      PHASE(3); 
      ld mul = 2;
      int N = 1000;
      shift(); sscanf(argcs(), LDF ",%d", &mul, &N);
      spiral::place(N, mul);
      return 0;
      }
  
    else if(argis("-spiraledge")) {
      PHASE(3); 
      ld shft = 1;
      ld mul = 1;
      shift(); sscanf(argcs(), LDF "," LDF, &shft, &mul);
      spiral::edge(shft, mul);
      return 0;
      }
  
    else if(argis("-spiralcolor")) {
      PHASE(3); 
      ld period = 1;
      ld start = 1;
      shift(); sscanf(argcs(), LDF "," LDF, &period, &start);
      start--;
      shift();
      spiral::color(start, period, parse(args()));
      return 0;
      }

    return 1;
    });

}}
