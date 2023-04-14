#include "rogueviz.h"

namespace hr {

#if CAP_MODELS
namespace ply {

using namespace rogueviz::objmodels;

bool animated;

hyperpoint A = point31(9.7, -38.1, 10.116);
hyperpoint B = point31(17.3, -38.1, 10.95);
hyperpoint C = point31(17.3, -46.1, 11.75);
hyperpoint D = point31(12.95, -46.08, 12.28);
hyperpoint E = point31(12.86, -44.53, 12.4);

ld radius = (12.86 - 11.11) / 2;

transmatrix nilform;

map<hyperpoint, pair<int, hyperpoint> > cache;

bool debugnil = false;
bool usecache = true;

pair<int, hyperpoint> nilize(hyperpoint h) {
  
  if(euclid) return {0, h - A + C0};

  hyperpoint hc = h;
  for(int i=0; i<4; i++) hc[i] = floor(h[i] * 1000 + .5);
  if(usecache && cache.count(hc)) return cache[hc];
  
  hyperpoint at = A;
  
  hyperpoint result = C0;
  
  auto move_coord_full = [&] (hyperpoint tgt) {
    for(int i=0; i<100; i++) {
      result = nisot::translate(result) * nilform * ((tgt - at) / 100 + C0);
      }
    at = tgt;
    };
  
  auto move_coord = [&] (int c, hyperpoint upto) {
    ld part = (h[c] - at[c]) / (upto[c] - at[c]);
    if(part > 1) part = 1;
    move_coord_full(at + part * (upto-at));
    };
  
  int cat = 0;
  
  if(h[1] > -39) {
    move_coord(0, B);
    cat = 1;
    goto finish;
    }
  move_coord_full(B);
  if(h[0] > 17.2) {
    move_coord(1, C);
    cat = 2;
    goto finish;    
    }
  move_coord_full(C);
  if(h[1] < -45.5) {
    move_coord(0, D);
    cat = 3;
    goto finish;
    }
  cat = 4;
  move_coord_full(D);
  move_coord(1, E);
  finish:
  
  move_coord_full(h);
  return cache[hc] = {cat, result};
  }

ld vperiod;

pair<hyperpoint, hyperpoint> trace_path(ld v) {

  ld vorig = v;
  
  hyperpoint lctr = A;
  ld angle = 0;
  
  ld arclen = radius * 90._deg;
  
  auto change_angle = [&] (ld x) {
    if(v == 0) return;
    else if(v >= arclen) v -= arclen, angle += 1;
    else angle += v / arclen, v = 0;
    };

  auto shift_to = [&] (hyperpoint h, bool last = false) {
    ld seglen = hypot_d(3, h - lctr);
    if(v == 0) return;
    else if(v >= seglen && !last) v -= seglen, lctr = h;
    else lctr = lerp(lctr, h, v / seglen), v = 0;
    };
  
  change_angle(1);
  shift_to(B);
  change_angle(2);
  shift_to(C);
  change_angle(3);
  shift_to(D);
  change_angle(4);
  shift_to(E, true);
  
  angle *= 90._deg;
  
  if(v > 0) vperiod = vorig - v;
  
  return { lctr + point3(radius * -cos(angle), radius * sin(angle), 0), point3(-sin(angle), -cos(angle), 0) };  
  }

transmatrix scaleby(ld x) {
  transmatrix res = Id;
  for(int i=0; i<LDIM; i++)
    res[i][i] = x;
  return res;
  }

vector<hyperpoint> route(1000), forwards(1000), overroute(1000);

hyperpoint interpolate_at(vector<hyperpoint>& v, ld x) {
  while(x > 1000) x -= 1000;
  return lerp(v[x], v[(int(x)+1) % 1000], frac(x));
  }

ld advance = 2;
ld over = 1.5;
ld over2 = 1;

void make_routes() {
  for(int i=0; i<1000; i++) {
    ld v = vperiod * i / 1000;
    route[i] = nilize(trace_path(v).first + point3(0,0,over)).second;
    ld vb = v + advance;
    if(vb > vperiod) vb -= vperiod;
    forwards[i] = nilize(trace_path(vb).first).second;
    overroute[i] = nilize(trace_path(v).first + point3(0,0,over+over2)).second;
    }
  
  auto smoothen = [&] (vector<hyperpoint>& v) {
    for(int it=0; it<100; it++) {
      vector<hyperpoint> smoother;
      for(int i=0; i<1000; i++) {
        hyperpoint& a = v[i];
        hyperpoint& b = v[(i+1) % 1000];
        hyperpoint& c = v[(i+2) % 1000];
        smoother.push_back((a + b + c) / 3);
        }
      v = smoother;
      }
    };
  
  smoothen(route);
  smoothen(forwards);
  smoothen(overroute);
  }

bool prepared;

void prepare_nilform() {
  if(prepared) return;
  prepared = true;
  hyperpoint axis = (E - A);
  transmatrix rotator = Id;
  rotator[2] = axis;
  println(hlog, " = ", (rotator[2] | rotator[2]));
  rotator[2] /= sqrt(rotator[2] | rotator[2]);
  rotator[1] -= (rotator[1] | rotator[2]) * rotator[2];
  rotator[1] /= sqrt(rotator[1] | rotator[1]);
  rotator[0] -= (rotator[0] | rotator[2]) * rotator[2];
  rotator[0] -= (rotator[0] | rotator[1]) * rotator[1];
  rotator[0] /= sqrt(rotator[0] | rotator[0]);
  println(hlog, "rotator = ", kz(rotator));
  rotator = inverse(transpose(rotator));
  println(hlog, "rotator = ", kz(rotator));    
  
  ld minscale = 0.5; // positive
  ld maxscale = 1.5; // negative
  ld scale;
  
  for(int it=0; it<100; it++) {  
    scale = (minscale + maxscale) / 2;
    nilform = rotator * scaleby(scale);
    cache.clear();
    hyperpoint nE = nilize(E).second;
    if(nE[2] < 0) maxscale = scale;
    else minscale = scale;
    }
  
  println(hlog, "scale = ", scale);
  println(hlog, nilize(E).second);  
  
  vperiod = radius * TAU + hypot_d(3, B-A) + hypot_d(3, C-B) + hypot_d(3, D-C) + hypot_d(3, E-D);
  println(hlog, "vperiod = ", vperiod);
  
  make_routes();
  }

struct nilmodel : model {
  hyperpoint transform(hyperpoint h) override { return nilize(h).second; }
  void process_triangle(vector<hyperpoint>& hys, vector<hyperpoint>& tot, bool textured, object *co) override {
    auto n0 = nilize(hys[0]).first;
    auto n1 = nilize(hys[1]).first;
    auto n2 = nilize(hys[2]).first;
    auto mi = min(n0, min(n1, n2));
    auto ma = max(n0, max(n1, n2));
    if(ma - mi > 1) return;
    model::process_triangle(hys, tot, textured, co);
    }
  };

nilmodel staircase;

bool draw_ply() {

  if(nil) prepare_nilform();

  staircase.render(ggmatrix(currentmap->gamestart()));
  
  return false;
  }

void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("Ascending & Descending"), 0xFFFFFFFF, 150, 0);

  dialog::addSelItem("advance", fts(advance), 'a');
  dialog::add_action([]() {
    dialog::editNumber(advance, 0, 100, 1, 1, "advance", "");
    dialog::reaction = make_routes;
    });

  dialog::addSelItem("over", fts(over), 'o');
  dialog::add_action([]() {
    dialog::editNumber(over, 0, 100, 1, 1, "over", "");
    dialog::reaction = make_routes;
    });

  dialog::addSelItem("over2", fts(over2), 'p');
  dialog::add_action([]() {
    dialog::editNumber(over2, 0, 100, 1, 1, "over2", "");
    dialog::reaction = make_routes;
    });

  dialog::addBoolItem_action("animated", animated, 'a');

  add_edit(prec);

  dialog::addBack();
  dialog::display();    
  }

void o_key(o_funcs& v) {
  v.push_back(named_dialog("Ascending & Descending", show));
  }

void enable() {
  rogueviz::rv_hook(hooks_frame, 100, draw_ply);
  rogueviz::rv_hook(hooks_o_key, 80, o_key);
  rogueviz::rv_hook(anims::hooks_anim, 100, [] { 
    if(!animated) return;
    usecache = false;
    ld t = ticks * 1. / anims::period;
    t = t - floor(t);
    t *= 1000;
    
    centerover = currentmap->gamestart();

    #if CAP_VR
    if(vrhr::active())
      View = gpushxto0(interpolate_at(route, t));
    else
    #endif
    set_view(
      interpolate_at(route, t),
      interpolate_at(forwards, t),
      interpolate_at(overroute, t)
      );
    
    anims::moved();
    });
  }

auto plyhook = 
  arg::add3("-asd", enable) + 
  addHook(hooks_configfile, 100, [] {
    param_b(animated, "ad_animated");
    });

}
#endif
}
