// non-Euclidean sunflower spirals (aka golden spirals or Fibonacci spirals)
// Copyright (C) 2018 Zeno and Tehora Rogue, see 'hyper.cpp' for details

// use: commandline parameter -sunflower <quantity> <density>
// e.g.: hyper -sunflower-qd 10000 1
// e.g.: hyper -sunflower-dr 1 4.5

// Commandlines for https://twitter.com/ZenoRogue/status/1247900522905886723 :

// Part 1:
// -geo 1 -sunflower-node 1 -sunflower-qd "1..10..20..60..100..140..180..220..|1000..1040..|2000..2040..|5000..5040..|10000..10040" 1 -zoom "sqrt(1000/(100+sunq))" -animperiod 20000 -shott 0 -back A0E0A0 -lw 16 -shotxy 1000 1000 -shotaa 2

// Part 2:
// -rugtsize 8192 -rugon -run -rugv 4000000 -run -sunflower-dr "0.001..0.002..0.005..0.01..0.02..0.04..0.06..0.07..0.08..0.09" 3..4.1..4.5..4.5..4.5..4.5..4.5..4.5..4.5 -lw 4 -sunflower-out 1 -shott 0 -back A0E0A0 -shotxy 1000 1000 -shotaa 2 -sunflower-adj 16 -animrec 600 curv%04d.png
// rotate the rug; press F10; wait until rug has millions of vertices; press F10; animation will be recorded

// Part 3:
// -rugtsize 8192 -rugon -rugv 1000000 -sunflower-dr .5 4.5 -lw 16 -shott 0 -back A0E0A0 -shotxy 1000 1000 -shotaa 2 -sunflower-adj 6
// (rotation animation set manually)

// Part 4:
// -geo 2 -sunflower-dr .1 pi -shott 0 -back A0E0A0 -shotxy 1000 1000 -shotaa 2 -animmove "2*pi" 0 0

#include "rogueviz.h"

namespace rogueviz {

namespace sunflower {

bool nodes;

ld qty = 100;
ld density = 1, zdensity;
ld range;

ld yshift;

ld distance_per_rug;

bool adjust_rug;

/* which property to infer from the other two: 'd'ensity, 'q'ty or 'r'ange */
char infer;

vector<hyperpoint> ps;

int iqty;

ld qfrac;

bool outward = false;

ld step_angle = M_PI * (3 - sqrt(5));

hyperpoint p(int i) {
  ld step = step_angle;
  return spin((outward ? i : i-iqty) * step) * xpush(sphere ? (acos(1 - (i+.5+qfrac) * density)) : euclid ? sqrt((i+.5+qfrac) * density) : acosh(1 + (i+.5+qfrac) * density)) * C0;
  }

vector<int> inext, inext2;

vector<int> fibs;

color_t sunflower1 = 0xC04000FF;
color_t sunflower2 = 0xFFD500FF;
color_t sunflower3 = 0x000000FF;

bool overlay = false;

bool sunflower_cell(cell *c, shiftmatrix V) {
  density = zdensity / 100;
  
  ld qd;
  
  if(sphere) {
    if(infer == 'r') 
      range = qty * density * M_PI/2;
    else qd = range * 2/M_PI;
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

  if(adjust_rug) {
    using namespace rug;

    model_distance = sqrt(zdensity) * distance_per_rug;

    }
  
  iqty = qty;
  qfrac = qty - iqty;
  if(outward) qfrac = 0;
  if(iqty < 0 || iqty > 2000000) return false;
  
  if(fibs.empty()) {
    ld best_error = 1;
    vector<int> sgns;
    for(int i=1; i<iqty; i++) {
      ld v = i * step_angle / (2*M_PI);
      v = frac(v);
      auto sgn = v > .5;
      if(sgn) v = 1-v;
      if(v < best_error) fibs.push_back(i), sgns.push_back(sgn), best_error = v;
      }
    println(hlog, "fibs = ", fibs);
    println(hlog, "sgns = ", sgns);
    }

  ps.resize(iqty);
  inext.resize(iqty);
  inext2.resize(iqty);
  while(fibs.back() < iqty) {
    /* to do: might not work correctly if step_angle is changed */
    auto add = fibs.back() + *(fibs.end()-2);
    fibs.push_back(add);
    }
  
  if(c == cwt.at) {
    for(int i=0; i<iqty; i++) ps[i] = p(i);

    for(int i=0; i<iqty; i++) {
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

    if(sunflower1 || sunflower2 || sunflower3) for(int i=0; i<iqty; i++) {
      if(inext[inext[i]] == inext2[i] || inext2[inext[i]] == inext2[i] || inext[inext2[i]] == inext[i] || inext2[inext2[i]] == inext[i]) {
        curvepoint(ps[i]);
        curvepoint(ps[inext2[i]]);
        curvepoint(ps[inext[i]]);
        // queuecurve(0xFFFFFFFF, 0x00C000FF, PPR::LINE);
        queuecurve(V * ypush(yshift), sunflower3, sunflower1, PPR::LINE);
        }      
      else {
        curvepoint(ps[i]);
        curvepoint(ps[inext[i]]);
        curvepoint(ps[inext[i] + inext2[i] - i]);
        curvepoint(ps[inext2[i]]);
        queuecurve(V * ypush(yshift), sunflower3, sunflower2, PPR::LINE);
        }
      if(nodes) queuepolyat(V * ypush(yshift) * rgpushxto0(ps[i]), cgi.shSnowball, 0xFF, PPR::SUPERLINE);
      }
    }

  return !overlay;
  }

void insert_param() {  
  param_f(zdensity, "sund");
  param_f(qty, "sunq");
  param_f(range, "sunr");
  param_f(distance_per_rug, "sunf");
  param_f(yshift, "sunyshift");
  }

void show();

void enable() {
  rv_hook(hooks_o_key, 80, [] (o_funcs& v) { v.push_back(named_dialog("sunflowers", show)); });
  rv_hook(hooks_drawcell, 100, sunflower_cell);
  }
  
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-sunflower-qd")) {
    enable();
    infer = 'r';
    shift_arg_formula(qty);
    shift_arg_formula(zdensity);
    patterns::whichShape = '9';
    insert_param();
    nohud = true;
    }
  else if(argis("-sunflower-qr")) {
    enable();
    infer = 'd';
    shift_arg_formula(qty);
    shift_arg_formula(range);
    patterns::whichShape = '9';
    insert_param();
    nohud = true;
    }
  else if(argis("-sunflower-dr")) {
    infer = 'q';
    shift_arg_formula(zdensity);
    shift_arg_formula(range);
    enable();
    /*
    patterns::whichShape = '9';
    */
    insert_param();
    nohud = true;
    }
  else if(argis("-sunflower-node")) {
    shift(); nodes = argi();
    }
  else if(argis("-sunflower-out")) {
    shift(); outward = argi();
    }
  else if(argis("-sunflower-angle")) {
    shift_arg_formula(step_angle, [] { fibs.clear(); });
    }
  else if(argis("-sunflower-adj")) {
    adjust_rug = true;
    shift_arg_formula(distance_per_rug);
    }
  else if(argis("-sunflower-colors")) {
    PHASEFROM(2);
    shift(); sunflower1 = argcolor(32);
    shift(); sunflower2 = argcolor(32);
    shift(); sunflower3 = argcolor(32);
    }
  else if(argis("-sunflower-overlay")) {
    PHASEFROM(2);
    shift(); overlay = argi();
    }
  else return 1;
  return 0;
  }

void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("sunflower spirals"), 0xFFFFFFFF, 150, 0);
  
  dialog::addSelItem("density", fts(zdensity), 'd');
  dialog::add_action([] {
    if(infer == 'd') infer = 'q';
    dialog::editNumber(zdensity, 0, 2, .1, 1, "density", "density");
    });
  
  dialog::addSelItem("quantity", fts(qty), 'q');
  dialog::add_action([] {
    if(infer == 'q') infer = 'r';
    dialog::editNumber(qty, 1, 100000, .1, 1000, "quantity", "quantity");
    dialog::scaleLog();
    });

  dialog::addSelItem("radius", fts(range), 'q');
  dialog::add_action([] {
    if(infer == 'r') infer = 'd';
    dialog::editNumber(range, 0, 10, .1, 2*M_PI, "range", "range");
    dialog::scaleLog();
    });
  
  dialog::addSelItem("infer", infer == 'd' ? "density" : infer == 'q' ? "quantity" : "range", 'i');
  dialog::add_action([] {
    if(infer == 'r') infer = 'd';
    else if(infer == 'd') infer = 'q';
    else infer = 'r';
    });
  
  if(rug::rugged) {
    dialog::addBoolItem("adjust Rug curvature", adjust_rug, 'a');
    dialog::add_action([] {
      adjust_rug = !adjust_rug;
      distance_per_rug = rug::model_distance / sqrt(zdensity);
      });
    if(adjust_rug) {
      dialog::addSelItem("factor", fts(distance_per_rug), 'f');
      dialog::add_action([] {
        dialog::editNumber(distance_per_rug, 0, 10, .1, 4, 
          "factor", "factor");
        });
      }
    else {
      dialog::addItem("disable the Hypersian Rug", 'f');
      dialog::add_action(rug::close);
      }
    }
  else {
    dialog::addItem("enable the Hypersian Rug", 'a');
    dialog::add_action(rug::init);
    }
  
  dialog::addBoolItem("draw the seeds", nodes, 's');

  dialog::addBoolItem("grow at the edge", outward, 'o');

  dialog::addBack();
  dialog::display();
  }

auto hook = 0
#if CAP_COMMANDLINE
+ addHook(hooks_args, 100, readArgs)
#endif
+ addHook_rvslides(144, [] (string s, vector<tour::slide>& v) {
  if(s != "mixed") return;
  using namespace tour;
  v.push_back(
    tour::slide{"sunflower spirals", 18, LEGAL::ANY | QUICKGEO, 
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
    slide_url(mode, 'y', "YouTube link", "https://www.youtube.com/watch?v=bKzibaNqEog");
    slide_url(mode, 't', "Twitter link", "https://twitter.com/ZenoRogue/status/1247900522905886723");
    setCanvas(mode, '0');
    
    if((mode == pmStop || mode == pmGeometry) && rug::rugged) rug::close();
    
    if(mode == pmKey) {
      if(rug::rugged) rug::close();
      else rug::init();
      }
    
    if(mode == pmStart) {
      stop_game();
      
      enable();
      tour::slide_backup(range, sphere ? 2 : euclid ? 10 : 4.3);
      tour::slide_backup<ld>(zdensity, 1);
      tour::slide_backup(infer, 'q');
  
      insert_param();
      start_game();
      }
    }}
    );
  });
 
}

}