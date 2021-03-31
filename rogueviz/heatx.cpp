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

ld scale = 0.05;

void advance_heat_wave() {

  if(euclid && GDIM == 2)
    pconf.scale = scale / max(frac, .15);

  int steps = mode == 2 ? (frac * qsteps) : (frac * frac * qsteps);

  if(steps != last_steps) {
    celllister cl(cwt.at, 400, 20000, nullptr);
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
  rogueviz::cleanup.push_back([] { m1.clear(); m2.clear(); m3.clear(); last_steps = OFF; });
  last_steps = NOT_STARTED; frac = 0;
  }

auto heathook = arg::add3("-heatx", enable)
  + addHook(hooks_configfile, 100, [] {
    param_f(delta, "heat_delta");
    param_i(qsteps, "heat_qsteps");
    param_f(frac_per_frame, "heat_pf");
    param_f(scale, "heat_scale");
    });

}
}
