// ./ht -rands 223 -geo Bring -7 -vizszi -canvas-random 0 -gears animperiod=500 -smart 10 -run -zoom .95 -shot-1000 -smart 1 -animvideo 30 gears.mp4 -exit

#include "../hyper.h"

namespace hr {

namespace gears {

ld px = 0.15, py = 0.22;

int teeth1 = 10, teeth2 = 25;

ld extend = 0, extendin = 0;

ld tsize = 0.1;

ld p;

ld rat = 0.2;

hyperpoint h1, h2;

int speed = 1;

struct gear_data : gi_extension {
  hpcshape gear1, gear2, circle;
  };

void genshape1(gear_data& gd) {

  cgi.bshape(gd.gear1, PPR::FLOOR);
  ld r1 = asin_auto(teeth1 * tsize);

  hyperpoint afirst, first, last;
  ld cp0 = 0 - extendin, cp1 = 1 + extend;

  for(int t=0; t<teeth1; t++) {

    for(ld param1=cp0; param1<=cp1; param1+=0.01) {
      ld alpha1 = (param1 - 0.5) * (TAU / teeth1);
      hyperpoint h = spin(-TAU * t / teeth1) * spin(-alpha1) * xpush(r1) * normalize(lerp(h1, h2, param1));
      if(param1 == cp0) first = h;
      if(param1 == cp0 && t == 0) afirst = h;
      cgi.hpcpush(h);
      }

    for(ld param1=cp1; param1>=cp0; param1-=0.01) {
      ld alpha1 = (param1 - 0.5) * (TAU / teeth1);
      hyperpoint h = spin(-TAU * t / teeth1) * spin(-M_PI/teeth1) * spin(alpha1) * xpush(r1) * normalize(MirrorY * lerp(h1, h2, param1));
      cgi.hpcpush(h);
      last = h;
      }

    cgi.hpcpush(normalize(last+C0*rat));
    cgi.hpcpush(spin(-M_PI/teeth1) * normalize(first+last+C0*rat*2));
    cgi.hpcpush(spin(-TAU /teeth1) * normalize(first+C0*rat));
    }

  cgi.hpcpush(afirst);

  cgi.finishshape();
  }

void genshape2() {
  ld r2 = asin_auto(teeth2 * tsize);

  hyperpoint afirst, first, last;
  ld cp0 = 0 - extend, cp1 = 1 + extendin;

  for(int t=0; t<teeth2; t++) {
    for(ld param1=cp1; param1>=cp0; param1-=0.01) {
      ld alpha1 = (param1 - 0.5) * (TAU / teeth2);
      hyperpoint h = spin(TAU * t / teeth2) * spin(M_PI/teeth2) * spin(-alpha1) * xpush(-r2) * normalize(MirrorY * lerp(h1, h2, param1));
      if(param1 == cp1) first = h;
      if(param1 == cp1 && t == 0) afirst = h;
      curvepoint(h);
      }

    for(ld param1=cp0; param1<=cp1; param1+=0.01) {
      ld alpha1 = (param1 - 0.5) * (TAU / teeth2);
      hyperpoint h = spin(TAU * (t+1) / teeth2) * spin(alpha1) * xpush(-r2) * normalize(lerp(h1, h2, param1));
      last = h;
      curvepoint(h);
      }

    curvepoint(normalize(last+C0*rat));
    curvepoint(spin(M_PI/teeth2) * normalize(first+last+C0*rat*2));
    curvepoint(spin(TAU/teeth2) * normalize(first+C0*rat));
    }
  curvepoint(afirst);
  }

void gencircle(gear_data& gd) {
  cgi.bshape(gd.circle, PPR::ITEM);
  for(int a=0; a<=360; a++) cgi.hpcpush(xspinpush0(a*degree, asin_auto(teeth1 * tsize) * .6));
  cgi.finishshape();
  }

void gears_frame() {

  // ld r1 = asin_auto(teeth1 * tsize);
  // ld r2 = asin_auto(teeth2 * tsize);

  // ld param = frac(ticks * speed / anims::period);

  // shiftmatrix S = ggmatrix(cwt.at) * xpush(p); hr::ignore(S);

  h1 = hpxy(-px, py);
  h2 = hpxy(px, -py);

  /*
  queueline(S * h1, S * h2, 0xFF0000FF);
  queueline(S * xpush0(-r1), S * lerp(h1, h2, param), 0x00FF0080);
  queueline(S * xpush0(r1), S * lerp(h1, h2, param), 0x00FF0080);
  */

  // ld alpha = (param - 0.5) * (TAU / teeth1);

  /* genshape1();
  queuecurve(S * xpush(-r1) * spin(alpha), 0xFF0000FF, 0xFF000080, PPR::LINE);

  alpha = (param - 0.5) * (TAU / teeth2);
  genshape2();
  queuecurve(S * xpush(r2) * spin(-alpha), 0xFF0000FF, 0xFF000080, PPR::LINE); */

  }

set<cell*> visited;

// spin = t[0..1] * TAU / teeth1


bool gears_cell(cell *c, const shiftmatrix& V) {

  h1 = hpxy(-px, py);
  h2 = hpxy(px, -py);

  auto& gd = (unique_ptr<gear_data>&) cgi.ext["gears"];
  if(!gd) {
    gd = std::make_unique<gear_data>();
    genshape1(*gd);
    gencircle(*gd);
    cgi.extra_vertices();
    println(hlog, "generated gears ", tie(gd->gear1.s, gd->gear1.e), tie(gd->circle.s, gd->circle.e));
    }

  if(!visited.count(c)) {
    hyperpoint h = (V * C0).h;
    ld d = hdist0(h);
    d = 1 / (1 + d);
    println(hlog, "d = ", d);
    int iad = int(d * 100);
    ld r = atan2(h);
    if(iad == 37) r += M_PI*0.4;
    c->landparam = rainbow_color(1 - d, r / TAU);
    visited.insert(c);
    }

  ld param = frac(ticks * speed / anims::period);

  ld alpha = (param - 0.5) * (TAU / teeth1);

  color_t col = c->landparam & 0xFFFFFF;
  poly_outline = darkena(col, 0, 0xFF);
  vid.linewidth *= 2;
  // queuecurve(V * spin((sphere ? (celldistance(c, cwt.at)&1) : (c->master->distance&1)) ? alpha : -alpha), darkena(col, 0, 0xFF), darkena(gradient(0, col, 0, 0.5, 1), 0, 0xFF), PPR::LINE);
  queuepoly(V * spin((sphere ? (celldistance(c, cwt.at)&1) : (c->master->distance&1)) ? alpha : -alpha), gd->gear1, darkena(gradient(0, col, 0, 0.5, 1), 0, 0xFF));
  vid.linewidth /= 2;

  vid.linewidth *= 2;
  queuepoly(V, gd->circle, 0x000000FF);
  vid.linewidth /= 2;

  addaura(tC0(V), col, 0);

  return true;
  }

void gears_dialog() {
  cmode = sm::NORMAL | sm::SIDE;
  gamescreen();
  dialog::init();
  dialog::init(XLAT("gear parameters"));
  dialog::addBreak(25);
  dialog::addSelItem("gear size", fts(tsize), 'g');
  dialog::add_action([] { dialog::editNumber(tsize, 0, 1, 0.1, 0.1, "gear size", ""); dialog::scaleLog(); });
  dialog::addSelItem("teeth1", its(teeth1), 'a');
  dialog::add_action([] { dialog::editNumber(teeth1, 3, 100, 1, 10, "teeth1", ""); });
  dialog::addSelItem("teeth2", its(teeth2), 'b');
  dialog::add_action([] { dialog::editNumber(teeth2, 3, 100, 1, 10, "teeth2", ""); });
  dialog::addSelItem("px", fts(px), 'c');
  dialog::add_action([] { dialog::editNumber(px, 0, 1, 0.01, 0.01, "px", ""); });
  dialog::addSelItem("py", fts(py), 'd');
  dialog::add_action([] { dialog::editNumber(py, 0, 1, 0.01, 0.01, "py", ""); });
  dialog::addSelItem("extend", fts(extend), 'e');
  dialog::add_action([] { dialog::editNumber(extend, 0, 1, 0.01, 0.01, "extend", ""); });
  dialog::addSelItem("extend-in", fts(extendin), 'f');
  dialog::add_action([] { dialog::editNumber(extendin, 0, 1, 0.01, 0.01, "extend-in", ""); });
  dialog::addBack();
  dialog::display();
  }

void enable() {
  start_game();
  // addHook(hooks_frame, 100, gears_frame);
  pushScreen(gears_dialog);
  ld r = hdist(C0, currentmap->adj(cwt.at, 0) * C0) / 2;
  teeth1 = 15; teeth2 = 15;
  // r = asin_auto(teeth1 * tsize);
  tsize = sin_auto(r) / teeth1;
  p = -r;
  px = 0.05; py = 0.1;
  println(hlog, "r=", r, " size=", tsize);
  addHook(hooks_drawcell, 100, gears_cell);
  }

auto shot_hooks = arg::add2("-gears", enable) + arg::add2("-rands", [] { arg::shift(); int q = arg::argi(); for(int i=0; i<q; i++) hrand(10); })
  + arg::add2("-gear-speed", [] { arg::shift(); speed = arg::argi(); arg::shift(); anims::rot_multiplier = arg::argi(); });

}
}
