#include "rogueviz.h"

/** A physics visualization of balls in a shell.
 *  
 *  Compile with HyperRogue, enable a 3D geometry (e.g. Nil), and watch.
 *  This is not configurable yet... you may need to manually change the gravity direction, or the number of balls
 *  (it is not optimized, and it does not work in real time with the default number of balls).
 */

namespace rogueviz {

namespace balls {

bool on = true;

struct ball {
  hyperpoint at;
  hyperpoint vel;
  };

vector<ball> balls;

ld r_small_ball = .1;
ld r_big_ball = 1;

hpcshape shSmallBall, shBigBall, shShell;

void initialize(int max_ball) {
  on = true;
  
  cgi.make_ball(shSmallBall, r_small_ball, 2);
  cgi.make_ball(shBigBall, r_big_ball, 4);
  
  cgi.bshape(shShell, PPR::WALL);
  shShell.flags |= POLY_TRIANGLES;

  auto pt = [] (int i, int j) {
    cgi.hpcpush(direct_exp(/* cspin(0, 2, -30._deg) **/ cspin90(2, 1) * cspin(0, 1, j * degree) * cspin(0, 2, i * 90._deg / 16) * ztangent(r_big_ball)));
    };

  for(int i=0; i<16; i++)
  for(int j=0; j<360; j++) {
    pt(i, j);
    pt(i, j+1);
    pt(i+1, j);
    pt(i, j+1);
    pt(i+1, j);
    pt(i+1, j+1);
    }
  cgi.finishshape();
  cgi.extra_vertices();
  
  for(int a=-max_ball; a<=max_ball; a++) 
  for(int b=-max_ball; b<=max_ball; b++) 
  for(int c=-max_ball; c<=max_ball; c++)
  {
    hyperpoint h = point3(0.21*a + 1e-2, 0.21*b, 0.21*c);
    
    if(hypot_d(3, h) > r_big_ball - r_small_ball) continue;
    
    transmatrix T = rgpushxto0(direct_exp(h));
        
    balls.emplace_back(ball{T*C0, T*ztangent(1e-3)});
    }
    
  }

bool draw_balls(cell *c, const shiftmatrix& V) {
  if(!on) return false;
  
  if(c == currentmap->gamestart()) {
    for(auto& b: balls)
      queuepoly(V * rgpushxto0(b.at), shSmallBall, 0xFFFFFFFF);
    queuepoly(V, shShell, 0x0000F0FF);
    }
  
  return false;
  }

ld inner(hyperpoint a, hyperpoint b) {
  ld s = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
  if(hyperbolic) return s - a[3] * b[3];
  if(sphere) return s + a[3] * b[3];
  return s;
  }

void geodesic_steps(hyperpoint& at, hyperpoint& vel, int qty) {
  if(nonisotropic) {
    vel /= qty;
    for(int i=0; i<qty; i++)
      nisot::geodesic_step(at, vel);
    vel *= qty;
    }
  else {
    ld d = sqrt(inner(vel, vel));
    tie(at, vel) = make_pair(
      at * cos_auto(d) + vel * sin_auto(d)/d,
      vel * cos_auto(d) - at * sin_auto(d) * sig(3) * d
      );
    }
  }

ld elastic_in = .2;
ld elastic_out = .2;

ld gravity = 1;

bool turn(int delta) {
  if(!on) return false;
  for(int i=0; i<delta; i++) {
    for(auto& b: balls) {
      /* gravity direction: z */
      b.vel += ctangent(2, 1e-6) * gravity;

      geodesic_steps(b.at, b.vel, 1);
      
      if(!nonisotropic && !euclid) {
        ld e = sqrt(abs(inner(b.at, b.at)));
        b.at /= e;
        ld e2 = inner(b.at, b.vel) * sig(3);
        b.vel -= b.at * e2;
        }
      
      hyperpoint v = inverse_exp(shiftless(b.at));
      ld d = hypot_d(3, v);
      ld rbs = r_big_ball - r_small_ball;
      if(d > rbs) {
        hyperpoint c = C0, ve = v * rbs / d;
        geodesic_steps(c, ve, 20);
        hyperpoint ort = ve / d;
        transmatrix T = gpushxto0(b.at);
        b.vel -= inner(T*b.vel, T*ort) * ort * (1 + elastic_out);
        
        b.at = c;
        if(!nonisotropic && !euclid) {
          ld e2 = inner(b.at, b.vel) * sig(3);          
          b.vel -= b.at * e2;
          }
        }
      }

    /* This is not optimized. It should use a partition of the space, 
     * to tell which balls have a chance to touch each other. */

    for(auto& b1: balls) 
    for(auto& b2: balls) {
      if(&b2 == &b1) break;
      hyperpoint dif = inverse_exp(shiftless(gpushxto0(b1.at) * b2.at));
      ld d = hypot_d(3, dif);
      if(d < r_small_ball * 2) {
        hyperpoint ort1 = (dif / d);
        ld vel1 = +inner(gpushxto0(b1.at) * b1.vel, ort1);
        hyperpoint ort2 = inverse_exp(shiftless(gpushxto0(b2.at) * b1.at)) / d;
        ld vel2 = +inner(gpushxto0(b2.at) * b2.vel, ort2);
        ld vels = vel1 + vel2;
        if(vels < 0) continue;
        
        vels *= (1 + elastic_in) / 2;
        
        b1.vel -= rgpushxto0(b1.at) * (vels * ort1);
        b2.vel -= rgpushxto0(b2.at) * (vels * ort2);
        }
      }
    }
  return false;
  }

int args() {
  using namespace arg;
           
  if(0) ;

  else if(argis("-ball-physics")) {
    start_game();
    check_cgi();
    cgi.require_shapes();
    shift();
    initialize(argi());
    View = cspin90(1, 2);
    }

  else return 1;
  return 0;
  }


auto celldemo = addHook(hooks_drawcell, 100, draw_balls) +
    addHook(shmup::hooks_turn, 100, turn) +
    addHook(hooks_args, 100, args) +
    addHook(hooks_clearmemory, 40, [] () {
      balls.clear();
      on = false;      
      });

}
}
