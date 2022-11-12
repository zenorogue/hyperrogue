// used in: https://twitter.com/ZenoRogue/status/1449021648137502721
// other animations in this thread use the TES system (tessellations/sample/star-*.tes)
// but hexagrams and horospheres are not supported in tes, so we do them manually
// commandline: -canvas 0 -geo 44onh -PM 0 -horo63 -tstep 16 -noplayer -back ffffff
// add to make a animation: -shot-1000 -animrot 1 90 -animvideo 300 name.mp4

#include "rogueviz.h"

namespace hr {

namespace horo63 {

map<pair<int, int>, color_t> mco;
map<pair<int, int>, bool> sco;

bool draw_horo63(cell *c, const shiftmatrix& V) {
  if(c != currentmap->gamestart()) return false;
  
  int mx = 30;
  
  for(int x=-mx; x<=mx; x++)
  for(int y=-mx; y<=mx; y++) if(x*x+y*y <= mx*mx) {
    array<hyperpoint, 6> c;
    hyperpoint ctr;
    for(int i=0; i<6; i++) {
      hyperpoint h = spin(TAU*i/6) * point31(1,0,0);
      h[0] += x;
      h[0] += y/2.;
      h[1] += sqrt(3)/2. * y;
      //println(hlog, tie(x,i), h);
      c[i] = parabolic13(h[0]*.5, h[1]*.5) * xpush0(-2);
      ctr += c[i];
      }
    ctr = normalize(ctr);
    color_t& col = mco[{y,x}];
    if(col == 0) col = (hrand(0x1000000)<<8) | 0x808080FF; 
    
    for(int a=0; a<8; a++)
    texture_order([&] (ld i, ld j) {
      curvepoint(normalize(ctr*(1-j-i) + c[a]*i + c[(a+2)%6]*j));
      });
   
    auto& cu = queuecurve(V, 0, col, PPR::WALL);
    cu.flags |= POLY_TRIANGLES;
    cu.tinf = &floor_texture_vertices[cgi.shStarFloor.id];
    ensure_vertex_number(*cu.tinf, cu.cnt);
    }
  println(hlog, "drawn");
  return false;
  }

void enable63() { 
  rogueviz::rv_hook(hooks_drawcell, 100, draw_horo63);
  }

bool draw_horo44(cell *c, const shiftmatrix& V) {
  if(c != currentmap->gamestart()) return false;
  
  int mx = 10;
  
  for(int x=-mx; x<=mx; x++)
  for(int y=-mx; y<=mx; y++) if(x*x+y*y <= mx*mx) {
    if(x < -4 || y < -4 || x > 3 || y > 3) continue;
    array<hyperpoint, 8> c;
    hyperpoint ctr;
    array<double, 8> dx = {1, 3, 3, 1, -1, -3, -3, -1};
    array<double, 8> dy = {3, 1, -1, -3, -3, -1, 1, 3};
    for(int i=0; i<8; i++) {
      ld ax = x + dx[i]/2. + .5;
      ld ay = y + dy[i] / 2. + .5;
      ax /= 2;
      ay /= 2;
      c[i] = parabolic13(ax, ay) * xpush0(-1);
      ctr += c[i];
      }
    ctr = normalize(ctr);
    color_t& col = mco[{y,x}];
    
    auto mod = [] () {
      color_t col = 0;
      for(int i=0; i<3; i++) part(col, i) = hrand(64);
      return col << 8;
      };

    bool& special = sco[{y,x}];
    
    if(col == 0) {
     
      if(x == 1 && y == 0)
        col = 0x7F1010FF, special = true;
      else if(x == -1 && y == 1)
        col = 0x40FF40FF, special = true;
      else if(x == -2 && y == -1)
        col = 0x00007FFF, special = true;
      else if(x == 0 && y == -2)
        col = 0xFFD540FF, special = true;
      else if((x+y)&1) {
        col = mod() ^ 0xFF; 
        // part(col, 1+hrand(3)) ^= 0x20;
        }
      else {
        col = mod() ^ 0xFFFFFFFF;
        // part(col, 1+hrand(3)) ^= 0x20;
        }
      }
    
    for(int a=0; a<8; a++)
    texture_order([&] (ld i, ld j) {
      curvepoint(normalize(ctr*(1-j-i) + c[a]*i + c[(a+2)%8]*j));
      });
   
    if(false)
    for(auto v1: c) 
    for(auto v2: c) 
    for(int i=0; i<5; i++)
      hr::addaura(V*normalize(v1*i+v2*(4-i)), col >> 8, 0);

    auto& cu = queuecurve(V, 0, col, PPR::WALL);
    cu.flags |= POLY_TRIANGLES;
    cu.tinf = &floor_texture_vertices[cgi.shStarFloor.id];
    ensure_vertex_number(*cu.tinf, cu.cnt);
    }
  return false;
  }

void enable44() { 
  rogueviz::rv_hook(hooks_drawcell, 100, draw_horo44);
  }

auto frac_ah = arg::add3("-horo63", enable63) + arg::add3("-horo44", enable44);

}}

// 0 0
// 1 2
