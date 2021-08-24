#include "rogueviz.h"

namespace hr {

namespace recursive_house {

// log(2) / vid.plevel_factor == 1.78379
// 0.38858115616745542

// commandline for Euclidean animation:
// rogueviz -canvas 0 -geo beti -recursive-house -noplayer

// commandline for S2xE
// rogueviz -geo 2 -7 -product -prodperiod 1 -canvas 0 -lands 6 -recursive-house -noplayer plevel_factor=0.38858*2

// (warning: it is quite slow)

bool draw_recursive_house_at(cell *c, const shiftmatrix& V) {
  if(c != currentmap->gamestart()) return false;
  for(int lv=-10; lv<=10; lv++) {
    if(!euclid && lv) continue;
    dynamicval<eGeometry> g(geometry, gCubeTiling);
    check_cgi();
    cgi.require_shapes();

    auto cube = [&] (ld x, ld y, ld z, color_t wal, ld s = 1) {
      for(int i=0; i<6; i++) {
        auto& p = cgi.shWall3D[i];
        for(int j=p.s; j<p.e; j++) {
          hyperpoint h = cgi.hpc[j];
          h[0] *= s;
          h[1] *= s;
          h[2] *= s;
          h[0] += x-3.5;
          h[1] += y-3.5;
          h[2] += z-3.5;
          h *= exp(lv * log(4));
          h[3] = 1;
          curvepoint(h);
          }
        auto& c = queuecurve(V, 0, wal, PPR::WALL);
        c.flags |= POLY_TRIANGLES;
        c.tinf = &floor_texture_vertices[cgi.shStarFloor.id];
        ensure_vertex_number(*c.tinf, c.cnt);
        c.offset_texture = p.texture_offset;
        }
      };
    
    for(int x=0; x<=7; x++)
    for(int y=0; y<=7; y++)
      cube(x, y, 0, 0x103010FF);

    for(int z=1; z<=2; z++)
      cube(3.5, 3.5, z, 0xC0C0C0FF);
      
    cube(5, 1, 1, 0x202080FF);
    cube(5, 1, 2, 0x202060FF);
    cube(5, 1, 2.75, 0x202080FF, 0.5);
    cube(5, 1, 3.5, 0xC0C020FF);

    cube(2, 6, 1, 0x802080FF);
    cube(2, 6, 2, 0x602060FF);
    cube(2, 6, 2.75, 0x802080FF, 0.5);
    cube(2, 6, 3.5, 0xC0C020FF);

    for(int x: {0, 7}) for(int y: {0, 7}) 
      for(int z=1; z<=5; z++)
        cube(x, y, z, 0xFFD500FF);

    for(int x=0; x<=7; x++)
    for(int y=0; y<=7; y++)
      cube(x, y, 6 + min(min(x,7-x), min(y, 7-y)), 0x703030FF);
      
    }
  check_cgi();

  return false;
  }

void enable() { 
  rogueviz::rv_hook(hooks_drawcell, 100, draw_recursive_house_at);

  if(geometry == gCubeTiling)
    rogueviz::rv_hook(anims::hooks_anim, 100, [] { 
  
      ld t = ticks * 1. / anims::period;
      t = frac(t);
      
      hyperpoint at = point31(1, 0.5, 0.3);
      hyperpoint fwd = point31(0, 0, 0);
      hyperpoint up = point31(1, 0.5, 1);
      
      for(auto p: {&at, &fwd, &up})
      for(int i: {0,1,2})
        (*p)[i] *= exp(-t * log(4));
        
      set_view(at, fwd, up);
      
      sightranges[geometry] = exp(-t * log(4)) * 200;
      vid.cells_drawn_limit = 50;
      
      centerover = currentmap->gamestart();
      
      anims::moved();
      });        
  }

auto house_hook = arg::add3("-recursive-house", enable);

}}
