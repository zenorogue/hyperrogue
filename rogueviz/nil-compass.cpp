#include "rogueviz.h"

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
 *  Freezes for tessellations with ideal vertices
 *
 * 
 *
 **/

namespace rogueviz {

namespace nilcompass {

hyperpoint to_rot(hyperpoint h) {
  if(nil) h[2] -= h[0] * h[1] / 2;
  return h;
  }

hyperpoint to_heis(hyperpoint h) {
  if(nil) h[2] += h[0] * h[1] / 2;
  if(sphere || hyperbolic) h = normalize(h);
  if(sphere || hyperbolic) h[0] /= 2, h[1] /= 2, h[2] /= 2;
  return h;
  }

struct shape {
  color_t col;
  int i;
  int is;
  hpcshape sh;
  };

vector<shape> shapes;

bool known;

int zeroticks;

void reset() {
  known = false;
  shapes.clear();
  }

bool draw_compass(cell *c, const shiftmatrix& V) {

  if(!known) {
    known = true;

    for(int i=0; i<3; i++) for(int is=-1; is<2; is+=2) 
    for(int js=-1; js<2; js+=2) 
    for(int ks=-1; ks<2; ks+=2) 
    {      
      int j = (i+1) % 3;
      int k = (j+1) % 3;
      color_t col = 0xFFFFFFFF;
      part(col, i+1) = 0xA0;
      if(js*ks==1) {
        part(col, i+1) = 0x30; // col = gradient(col, 0xFF, 0, 0.8, 1);
        }
      if(is == -1) part(col, j+1) = part(col, i+1);
      
      shapes.emplace_back(shape{col, i, is, hpcshape()});
      
      auto& sh = shapes.back().sh;
      
      cgi.bshape(sh, PPR::LINE);
  
      hyperpoint p1 = C0 + ctangent(i, is * .4);
      hyperpoint p2 = C0 + ctangent(j, js * .15);
      hyperpoint p3 = C0 + ctangent(k, ks * .15);
      
      for(int i=0; i<10; i++) cgi.hpcpush(to_heis(lerp(p1, p2, i/10.)));
      for(int i=0; i<10; i++) cgi.hpcpush(to_heis(lerp(p2, p3, i/10.)));
      for(int i=0; i<10; i++) cgi.hpcpush(to_heis(lerp(p3, p1, i/10.)));
      
      cgi.hpcpush(to_heis(p1));
      
      cgi.finishshape();
      }
    cgi.extra_vertices();
    }

  poly_outline = 0;
  for(const auto& s: shapes) {

    ld t = 36 + (ticks - zeroticks) / 1000.;
    
    auto remap = [&] (int _i, int _is) {
      auto col = s.col;
      if(s.i == _i && s.is == _is) return col;
      int c = part(col, 1) + part(col, 2) + part(col, 3);
      c += 1; c /= 12;
      color_t col1 = s.col;
      part(col1, 1) = part(col1, 2) = part(col1, 3) = c;
      return gradient(s.col, col1, 0, 0.9, 1);
      };
    
    vector<pair<ld, color_t>> clist = {
      {36, s.col},
      {42.5, s.col},
      {42.7, remap(1, -1)},
      {43.9, remap(1, -1)},
      {44.1, remap(0, +1)},
      {44.9, remap(0, +1)},
      {45.1, remap(1, +1)},
      {46.0, remap(1, +1)},
      {46.2, remap(0, -1)},
      {47.2, remap(0, -1)},
      {47.4, remap(2, -1)},
      {48.1, remap(2, -1)},
      {48.3, remap(2, +1)},
      {49.1, remap(2, +1)},
      {49.3, s.col},
      {99, s.col}
      };
    
    int step = 0;
    while(t > clist[step+1].first) step++;
    auto smoothen = [&] (ld x) { return x * x * (3 - 2*x); };
    auto t1 = ilerp(clist[step].first, clist[step+1].first, t);
    auto col1 = gradient(clist[step].second, clist[step+1].second, 0, smoothen(t1), 1);
    
    queuepoly(V, s.sh, col1);
    }

  return false;
  }

auto hchook = arg::add3("-nil-compass", [] { rv_hook(hooks_drawcell, 100, draw_compass); });

}
}
// 36.00 -> START
// 42.6  -> NORTH
// 44.0  -> EAST
// 45.0  -> SOUTH
// 46.1  -> WEST
// 47.3  -> UP
// 48.2  -> DOWN
// 49.2  -> ...
// 53.00 -> END

