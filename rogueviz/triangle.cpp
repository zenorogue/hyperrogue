#include "../hyper.h"

// Impossible Triangle visualization

// used in: https://www.youtube.com/watch?v=YmFDd49WsrY

// settings:
// ./mymake -O3 rogueviz/triangle
// ./hyper -geo Nil -canvas x -tstep 8 -nilperiod 3 3 3

// also used in: https://youtu.be/RPL4-Ydviug

// ./hyper -geo Nil -nilwidth .9 -canvas x -tstep 1 -nilperiod 1 10 1 -triset 32 31 992

namespace hr {

// each color group (i.e., each face direction) is a different hpcshape
hpcshape ptriangle[6];

EX hyperpoint lerp(hyperpoint a0, hyperpoint a1, ld x) {
  return a0 + (a1-a0) * x;
  }

hyperpoint operator+(hyperpoint x) { return x; }

// do not change this
int shape = 1;

// how many cubes to subdivide edges to
int how = 4;

// how many cubes to draw (should be smaller than how because they are not really cubes and thus they get into each other)
int how1 = how - 1;

// precision: number of substeps to simulate (best if divisible by how and how1)
int isteps = 4 * 1024;

/* make the impossible triangle shape */
void make_shape() {

  static bool done = false;
  if(done) return;
  done = true;
  
  // four main axes of the regular tetrahedron, rotated so that ds[3] points to (0,0,1)
  
  ld rest = sqrt(8/9.);
  ld rex = sqrt(1 - 1/9. - pow(rest/2., 2));

  hyperpoint ds[4];
  
  ds[0] = point3(rex, -rest/2, -1/3.);
  ds[1] = point3(0, rest, -1/3.);
  ds[2] = point3(-rex, -rest/2, -1/3.);
  ds[3] = point3(0, 0, +1);
  
  hyperpoint start = point31(0, 0, 0);
    
  double lastz;
  
  double lasta;
  
  double ca;
  
  // compute how to scale this in Nil so that everything fits
  
  for(ld a = 1e-5;; a+=1e-5) {
    hyperpoint at = start;
    for(int d=0; d<3; d++) {
      for(int i=0; i<isteps; i++) {
        at = nisot::translate(at) * (start + ds[d] * a);
        }
      }
    
    println(hlog, "at = ", at, " for a = ", a, " sq = ", at[2] / a / a);
    if(at[2] > 0) {
      ld z = at[2];
      ca = lerp(lasta, a, ilerp(lastz, z, 0));
      break;
      }
    lastz = at[2]; lasta  =a;
    }
  
  println(hlog, "ca = ", ca);
  
  ld scale = .2;
  
  // compute the shift between the cubes

  array<hyperpoint, 4> uds;
  for(int d=0; d<3; d++) {
    hyperpoint at = start;
    for(int i=0; i<isteps/how; i++) {
      at = nisot::translate(at) * (start + ds[d] * ca);
      }
    uds[d] = (at - start) / 2.;
    }
  
  println(hlog, "uds = ", uds);

  for(int a=0; a<3; a++) println(hlog, sqhypot_d(3, inverse_exp(start + ds[a] * ca, iTable, false)));

  for(int a=0; a<3; a++) println(hlog, sqhypot_d(3, inverse_exp(uds[a], iTable, false)));
  
  // compute cube vertices

  hyperpoint verts[8];
  for(int a=0; a<8; a++) {
    verts[a] = start;
    for(int k=0; k<3; k++) 
      verts[a] += (a&(1<<k)) ? uds[k] : -uds[k];
    }
  
  // since Nil does not really have cubes, we need to move the vertices a bit so that it looks nicer
  
  // ugliness of the current vertices
  
  auto errf = [&] {
    ld res = 0;
    for(int s=0; s<8; s++)
    for(int t=0; t<3; t++) {
      if((s & (1<<t)) == 0) {
        int s1 = s | (1<<t);
        ld dix = sqhypot_d(3, inverse(nisot::translate(nisot::translate(start + 2*uds[t]) * verts[s])) * verts[s1]);
        // println(hlog, tie(s, t), "di = ", dix);
        res += dix * dix;
        }
      }
    return res;
    };
  
  // minimize the ugliness

  ld curerr = errf();
  println(hlog, "curerr = ", curerr);
  int att = 0;
  if(1) while(true) {
    int id = rand() % 8;
    int j = rand() % 3;
    ld eps = (rand() % 100 - rand() % 100) / 100000.;
    verts[id][j] += eps;
    ld nerr = errf();
    if(nerr < curerr) {
      curerr = nerr;
      println(hlog, "curerr = ", curerr, " # ", att);
      att = 0;
      }
    else {
      verts[id][j] -= eps;
      }
    att++;
    if(att > 50000) break;
    }

    for(int s=0; s<8; s++)
    for(int t=0; t<3; t++) {
      if((s & (1<<t)) == 0) {
        int s1 = s | (1<<t);
        ld dix = sqhypot_d(3, inverse(nisot::translate(nisot::translate(start + 2*uds[t]) * verts[s])) * verts[s1]);
        println(hlog, tie(s, t), "di = ", dix);
        }
      }
  
  scale = 1.;
  
  // build all the faces
  
  for(int si=0; si<6; si++) {
    cgi.bshape(ptriangle[si], PPR::WALL);
    hyperpoint at = start;
    
    for(int d=0; d<3; d++) {
      
      int d1 = (d+1) % 3;
      int d2 = (d+2) % 3;

      hyperpoint path[isteps+1];
      for(int i=0; i<isteps; i++) {
        path[i] = at;
        at = nisot::translate(at) * (start + ds[d] * ca);
        }
      path[isteps] = at;
      
      auto &u = uds[d];
      auto &v = uds[d1];
      auto &w = uds[d2];
      
      auto textured_square = [&] (auto f) {
        texture_order([&] (ld ix, ld iy) { f(.5 + ix/2 + iy/2, .5 + ix/2 - iy/2); });
        texture_order([&] (ld ix, ld iy) { f(.5 - ix/2 - iy/2, .5 - ix/2 + iy/2); });
        texture_order([&] (ld ix, ld iy) { f(.5 + ix/2 - iy/2, .5 - ix/2 - iy/2); });
        texture_order([&] (ld ix, ld iy) { f(.5 - ix/2 + iy/2, .5 + ix/2 + iy/2); });
        };
  
      auto sidewall = [&] (hyperpoint wide, hyperpoint shift) {
        textured_square( [&] (ld ix, ld iy) {
          hyperpoint online = path[int(ix * isteps + .1)];
          hyperpoint shf = lerp(u, -u, ix) + lerp(-wide, wide, iy) + shift;
          shf *= scale;
          cgi.hpcpush(nisot::translate(online) * (start + shf));
          });
        };
  
      auto sidesquare = [&] (hyperpoint wx, hyperpoint wy, hyperpoint shift, ld p) {
        textured_square( [&] (ld ix, ld iy) {
          hyperpoint online = path[int(p * isteps + .1)];
          hyperpoint shf = lerp(wx, -wx, ix) + lerp(wy, -wy, iy) + shift;
          shf *= scale;
          cgi.hpcpush(nisot::translate(online) * (start + shf));
          });
        };
      
      auto sidesquare1 = [&] (hyperpoint a00, hyperpoint a01, hyperpoint a10, hyperpoint a11, ld p) {
        hyperpoint online = path[int(p * isteps + .1)];
        textured_square( [&] (ld ix, ld iy) {
          hyperpoint shf = lerp(lerp(a00, a01, ix), lerp(a10, a11, ix), iy);
          shf *= scale;
          cgi.hpcpush(nisot::translate(online) * (shf));
          });
        };
      
      if(shape == 0) {
        if(si == d2*2) sidewall(v, w);
        if(si == d1*2) sidewall(w, v);
        if(si == d2*2+1) sidewall(v, -w);
        if(si == d1*2+1) sidewall(w, -v);
        
        if(si == d2*2) sidesquare(u, v, w, 0);
        if(si == d1*2) sidesquare(w, u, v, 0);
        if(si == d1*2+1) sidesquare(u, w, -v, 0);
        if(si == d*2+1) sidesquare(w, v, -u, 0);
        }
      
      if(shape == 1) for(int a=0; a<how1; a++) {
        ld c = a * 1. / how1;

/*
        if(si == d*2) sidesquare(v, w, u, c);
        if(si == d*2+1) sidesquare(w, v, -u, c);
        if(si == d1*2) sidesquare(w, u, v, c);
        if(si == d1*2+1) sidesquare(u, w, -v, c);
        if(si == d2*2) sidesquare(u, v, w, c);
        if(si == d2*2+1) sidesquare(v, u, -w, c);
*/

        if(si == 0) sidesquare1(verts[0], verts[2], verts[4], verts[6], c);
        if(si == 1) sidesquare1(verts[1], verts[3], verts[5], verts[7], c);
        if(si == 2) sidesquare1(verts[0], verts[1], verts[4], verts[5], c);
        if(si == 3) sidesquare1(verts[2], verts[3], verts[6], verts[7], c);
        if(si == 4) sidesquare1(verts[0], verts[1], verts[2], verts[3], c);
        if(si == 5) sidesquare1(verts[4], verts[5], verts[6], verts[7], c);
        
        }
      }
  
    cgi.last->flags |= POLY_TRIANGLES;
    cgi.last->tinf = &floor_texture_vertices[0];
    cgi.last->texture_offset = 0;
    cgi.finishshape();
    cgi.extra_vertices();
    }
  }

// Magic Cube (aka Rubik Cube) colors

color_t magiccolors[6] = { 0xFFFF00FF, 0xFFFFFFFF, 0x0000FFFF, 0x00FF00FF, 0xFF0000FF, 0xFF8000FF};

bool draw_ptriangle(cell *c, const transmatrix& V) {
  make_shape();
  
  if(c == cwt.at) {
    for(int side=0; side<6; side++) {
      auto &s = queuepoly(V, ptriangle[side], magiccolors[side]);
      ensure_vertex_number(*s.tinf, s.cnt);
      }
    }    

  return false;
  }


auto hchook = addHook(hooks_drawcell, 100, draw_ptriangle)

+ addHook(hooks_args, 100, [] {
  using namespace arg;
           
  if(0) ;
  else if(argis("-triset")) {
    shift(); how = argi();
    shift(); how1 = argi();
    shift(); isteps = argi();
    }
  else return 1;
  return 0;
  });



}
