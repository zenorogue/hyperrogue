#include "rogueviz.h"

namespace rogueviz {

namespace graph {
vector<string> formula;
bool graph_on;

color_t graphcolor;

hyperpoint err = point3(500,0,0);

bool iserror(hyperpoint h) { return sqhypot_d(2, h) > 10000 || std::isnan(h[0]) || std::isnan(h[1]) || std::isnan(h[2]) || std::isinf(h[0]) || std::isinf(h[1]) || std::isinf(h[2]); }

hyperpoint xy_to_point(ld x, ld y) {
  if(sphere && hypot(x, y) > 1)
    return err;
  return hpxy(x, y);
  }
  
hyperpoint find_point(ld t) {
  exp_parser ep;
  auto &dict = ep.extra_params;
  dict["t"] = t;
  dict["phi"] = t * 2 * M_PI;
  dict["x"] = tan(t * M_PI - M_PI/2);
  for(auto& ff: formula) {
    ep.s = ff;
    string varname = "";
    ep.at = 0;
    while(!among(ep.next(), '=', -1)) varname += ep.next(), ep.at++;
    ep.at++;
    cld x = ep.parse();
    if(!ep.ok()) return err;
    dict[varname] = x;
    }
  if(!dict.count("y") && dict.count("r"))
    return xspinpush0(real(dict["phi"]), real(dict["r"]));
  if(dict.count("z") && dict.count("x"))
    return hpxyz(real(dict["x"]), real(dict["y"]), real(dict["z"]));
  if(dict.count("z")) {
   return xy_to_point(real(dict["z"]), imag(dict["z"]));
   }
  return xy_to_point(real(dict["x"]), real(dict["y"]));
  }

hyperpoint gcurvestart = err;

void xcurvepoint(hyperpoint h) {
  curvepoint(h);
  if(iserror(gcurvestart))
    gcurvestart = h;
  else if(sphere && intval(gcurvestart, h) > .1) {
    queuecurve(cwtV, graphcolor, 0, PPR::LINE);
    curvepoint(h);
    gcurvestart = h;
    }
  }

void finish() {
  if(!iserror(gcurvestart)) {
    queuecurve(cwtV, graphcolor, 0, PPR::LINE);
    gcurvestart = err;
    }
  }

int small_limit = 6, big_limit = 20;

void draw_to(ld t0, hyperpoint h0, ld t1, hyperpoint h1, int small = 0, int big = 0) {
  if(iserror(h0) || iserror(h1) || intval(h0, h1) < .01) small++;
  else small = 0;
  if(small >= small_limit || big >= big_limit) {
    xcurvepoint(h1);
    return;
    }
  if(t1-t0 < 1e-6) {
    finish();
    return;
    }
  ld t2 = (t0 + t1) / 2;
  hyperpoint h2 = find_point(t2);
  draw_to(t0, h0, t2, h2, small, big+1);
  draw_to(t2, h2, t1, h1, small, big+1);
  }

int editwhich = -1;

void show_graph() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("graph"));
  for(int i=0; i<isize(formula); i++) {
    if(editwhich == i) {
      dialog::addItem(dialog::view_edited_string(), '1'+i);
      }
    else {
      dialog::addItem(formula[i], editwhich == -1 ? '1'+i : 0);
      dialog::add_action([i] () { editwhich = i; dialog::start_editing(formula[i]); });
      }
    }

  dialog::addBack();
  dialog::display();

  keyhandler = [] (int sym, int uni) {
    if(editwhich >= 0) {
      if(dialog::handle_edit_string(sym, uni)) ;
      else if(doexiton(sym, uni))
        editwhich = -1;
      }
    else {
      handlePanning(sym, uni);
      dialog::handleNavigation(sym, uni);
      // if(doexiton(sym, uni)) popScreen();
      }
    };
  }

void frame() { 
  if(graphcolor) {
    hyperpoint h0 = find_point(0);
    hyperpoint h1 = find_point(1);
    if(!iserror(h0)) xcurvepoint(h0);
    draw_to(0, h0, 1, h1);
    finish();
    }
  }

#if CAP_COMMANDLINE  
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-dgraph")) {
    PHASE(3);    
    showstartmenu = false;
    pushScreen(show_graph);
    shift();
    while(args().find("=") != string::npos) {
      formula.emplace_back(args());
      shift();
      }
    graphcolor = arghex();
    }
  else if(argis("-dgs")) {
    small_limit = argi();
    }
  else if(argis("-dgl")) {
    big_limit = argi();
    }
  else return 1;
  return 0;
  }
#endif


auto xhook = addHook(hooks_args, 100, readArgs)
  + addHook(hooks_frame, 0, frame);
 
}
}
