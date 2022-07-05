#include "rogueviz.h"
#include <unordered_map>

// heat equation simulation
// https://twitter.com/ZenoRogue/status/1208409387733307392

// run with e.g.
// -geo 1 -canvas 0 -smart 1 -smartlimit 999999 -heatx
// -tes tessellations/sample/marjorie-rice.tes heat_scale=0.02 -canvas 0 -smart 1 -smartlimit 999999 -heatx

namespace hr {

namespace heatx {

const int NOT_STARTED = 999999;
const int OFF = 999998;

int last_steps = NOT_STARTED;

std::unordered_map<cell*, double> m1, m2, m3;

ld delta = 0.01;

int mode = 1;

int qsteps = 2000;

ld frac_per_frame = .001;

ld frac;

ld scale = 0.04;

int simulation_range = 20000;

void advance_heat_wave() {

  if(euclid && GDIM == 2)
    pconf.scale = scale / max(frac, .15);

  int steps = mode == 2 ? (frac * qsteps) : (frac * frac * qsteps);

  if(steps != last_steps || mode == 3) {
    celllister cl(cwt.at, 999999, simulation_range, nullptr);
    if(steps < last_steps) {
      last_steps = 0;
      m1.clear();
      m2.clear();
      for(cell *c: cl.lst) m1[c] = 0;
      m2 = m1;
      m1[cwt.at] = 1;
      }
    while(last_steps < steps) {
      switch(mode) {
        case 0:
          // heat: average of adjacent
          for(cell *c: cl.lst) {
            ld v = m1[c];
            forCellEx(c2, c) {
              if(m1.count(c2)) v += m1[c2]; else v += m1[c];
              }
            v /= (1 + c->type);
            m2[c] = v;
            }
          swap(m1, m2);
          break;
        case 1:
          // heat: transfer to adjacent
          for(auto& p: m2) p.second = 0;
          for(cell *c: cl.lst) {
            ld v = m1[c] / (1 + c->type);
            m2[c] += v;
            forCellEx(c2, c) {
              if(m1.count(c2)) m2[c2] += v; else m2[c] += v;
              }
            }
          swap(m1, m2);
          break;
        case 2:
          // wave
          for(cell *c: cl.lst) {
            m3[c] = 0;
            forCellEx(c2, c) {
              if(m1.count(c2)) m3[c] += (m1[c2] - m1[c]);
              }
            }
          for(cell *c: cl.lst) {
            m1[c] += m2[c] * delta + m3[c] * delta * delta / 2;
            m2[c] += m3[c] * delta;
            }
          break;
        }
      last_steps++;
      }

    if(mode == 3) {
      ld fsteps = qsteps * frac;
      for(cell *c: cl.lst) {
        ld d = hdist0(tC0(ggmatrix(c)));
        m1[c] = m2[c] = m3[c] = exp(-d*d/(fsteps+1e-3));
        }
      }

    ld maxv = 0;
    for(auto p: m1) maxv = max(maxv, abs(p.second));
    for(cell *c: cl.lst) {
      ld x = m1[c] / maxv;
      if(mode == 2) {
        if(x < 0) c->landparam = gradient(0x001010, 0x1010FF, -1, x, 0);
        else c->landparam = gradient(0x1010FF, 0xFFFFFF, 0, x, 1);
        }
      else {
        if(x < 1/2.) c->landparam = gradient(0x001010, 0xFF1010, 0, x, 1/2.);
        else c->landparam = gradient(0xFF1010, 0xFFFF10, 1/2., x, 1.);
        if(x > .2 && x < .3) c->landparam |= 0x4040;
        }
      }
    }
  
  // return false;
  }

void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("heat transfer simulation"), 0xFFFFFFFF, 150, 0);
  add_edit(delta);
  add_edit(qsteps);
  add_edit(frac_per_frame);
  add_edit(scale);
  add_edit(simulation_range);
  dialog::addBack();
  dialog::display();
  }

void enable() {
  using rogueviz::rv_hook;
  rv_hook(hooks_frame, 100, advance_heat_wave);
  rv_hook(anims::hooks_anim, 100, [] {
     if(inHighQual) {
       frac = std::fmod(ticks, anims::period) * 1. / anims::period;
       }
     else {
       frac += frac_per_frame;
       if(frac > 1) frac--;
       }
     });
  rv_hook(shot::hooks_take, 100, [] { 
     advance_heat_wave(); calcparam(); models::configure(); 
     });
  rv_hook(hooks_drawcell, 100, [] (cell *c, const shiftmatrix& V) {
    if(WDIM == 3) 
      queuepoly(face_the_player(V), cgi.shRing, darkena(c->landparam_color, 0, 0xFF));
    return false;
    });
  rv_hook(hooks_o_key, 80, [] (o_funcs& v) { v.push_back(named_dialog("heat", show)); });
  rv_hook(hooks_post_initgame, 100, [] { last_steps = NOT_STARTED; frac = 0; });
  rogueviz::cleanup.push_back([] { m1.clear(); m2.clear(); m3.clear(); last_steps = OFF; });
  last_steps = NOT_STARTED; frac = 0;
  }

string cap = "heat transfer/";

void heat_slide(vector<tour::slide>& v, string title, string desc, reaction_t t) {
  using namespace tour;
  v.push_back(
    tour::slide{cap + title, 18, LEGAL::NONE | QUICKGEO, desc, 
   
  [t] (presmode mode) {
    setCanvas(mode, '0');
    slide_backup(vid.use_smart_range, 2);
    slide_backup(vid.smart_range_detail, 1);
    slide_backup(vid.cells_drawn_limit, 100000);
    slide_backup(vid.cells_generated_limit, 10000);
    if(mode == pmStart) {      
      t();
      start_game();
      enable();
      }
    }}
    );
  }

auto heathook = arg::add3("-heatx", enable)
  + addHook(hooks_configfile, 100, [] {
    param_f(delta, "heat_delta")
    ->editable(0, 1, 0.01, "delta", "how fast is the heat transfer", 't');
    param_i(qsteps, "heat_qsteps")
    ->editable(0, 10000, 100, "steps to simulate", "", 's');
    param_f(frac_per_frame, "heat_pf")
    ->editable(0, 0.01, 0.0001, "speed", "speed of simulation: fraction per frame", 'v');
    param_f(scale, "heat_scale")
    ->editable(0, 1, 0.001, "scale", "scaling factor", 'f');
    param_i(simulation_range, "heat_range")
    ->editable(0, 100000, 1000, "heat simulation range", "number of cells to consider", 'r');
    param_i(mode, "heat_mode");
    })
  + addHook_rvslides(180, [] (string s, vector<tour::slide>& v) {
      if(s != "mixed") return;
      heat_slide(v, "squares", 
        "A simple heat simulation. In each turn, the temperature changes towards the average of temperatures of adjacent cells.\n\n"
        "Here we do this simulation on a square grid. Note that, despite the natural taxicab metric, spread heats in perfect circles.",
        [] {
          set_geometry(gEuclidSquare); set_variation(eVariation::pure);
          });
      heat_slide(v, "Marjorie Rice tiling", "Heat simulation on a tiling discovered by Marjorie Rice. Despite the more complex tiling, the heat spreads in perfect circles!", [] {
        arb::run("tessellations/sample/marjorie-rice.tes");
        tour::slide_backup(scale, 0.02);
        });
      heat_slide(v, "elongated triangular", "It is not always perfect circles -- in a periodic tessellation, it could also be ellipses. Here the ellipses are very close to perfect circles.", [] {
        set_variation(eVariation::pure);
        set_geometry(gArchimedean);
        arcm::current.parse("(4,4,3L,3L,3L) [3,4]");
        });
      heat_slide(v, "kite-and-dart tiling", "But even in the kite-and-dart tiling we seem to get perfect circles.", [] {
        set_geometry(gKiteDart2);
        });
      heat_slide(v, "hyperbolic tiling", 
          "We used Euclidean tessellations so far. In each Euclidean tessellation, the tessellations behaved in roughly the same, Euclidean way.\n\n"
          "In hyperbolic geometry it is different -- not only it is less circular, but the radius of the hot area (at least 30% of the heat of the central tile) will not grow to infinity!", [] {
        set_geometry(gNormal);
        gp::param.first = 4;
        gp::param.second = 0;
        set_variation(eVariation::goldberg);
        });
      });

}
}
