// compile with mymake rogueviz/hypocycloid

// for https://twitter.com/ZenoRogue/status/1358464890978127875 :
// hyper -shot-1000 -noscr -canvas i -noplayer -geo 1 circ1=1 circ2=2 show=.05 lw=20 -zoom .9 -animvideo 360 cycloid-2-euc.mp4
// for https://twitter.com/ZenoRogue/status/1358464962088337410 :
// hyper -shot-1000 -noscr -canvas i -noplayer -geo 0 "circ1=0../0..4../0..|4../0..0../0" "circ2=0../0..8../0..|8../0..0../0" show=.05 lw=3 loops=10 -zoom .9 -animvideo 3600 cycloid-2-hyp.mp4
// for https://twitter.com/ZenoRogue/status/1358465029876690948 :
// hyper -shot-1000 -noscr -canvas i -noplayer -geo 0 "circ1=0../0..8../0..|8../0..0../0" "circ2=0../0..32../0..|32../0..0../0" prec=3600 show=.05 lw=3 loops=10 -zoom .9 -animvideo 3600 cycloid-4-hyp.mp4

// for https://twitter.com/ZenoRogue/status/1358506225990828033 :
// hyper -shot-1000 -noscr -canvas i -noplayer -geo 2 "circ1=0../0...5../0..|.5../0..0../0" "circ2=0../0..1../0..|1../0..0../0" show=.05 lw=3 loops=10 -zoom .9 -animvideo 1200 cycloid-2-sph.mp4

// for https://twitter.com/ZenoRogue/status/1358505146553159682 :
// hyper -shot-1000 -noscr -canvas i -noplayer -geo 0 circ1=1 circ2=2 show=.1 lw=5 -zoom .9 "ratio=2../0..8../0..|8../0..2../0" prec=3600 -animvideo 3600 half-radius.mp4

#include "../hyper.h"

namespace hr {

ld circ1 = 0.5; /* circumference of the small circle (actually, circumference divided by 2pi) */
ld circ2 = 1;   /* circumference of the large circle (actually, circumference divided by 2pi) */

ld show = 0.02; /* radius of the blue circle which shows the moving point */

ld lw = 3;      /* linewidth multiplier */

ld loops = 1;   /* with loops=k, the circle goes around k times during the full animation */

ld ratio = 0;   /* if set to non-zero, circ1 and circ2 are assigned so that the ratio of radii is ratio */

int prec = 360; /* precision */

void circ_frame() {
  ld t = (1. * prec * ticks) / anims::period * loops;
  
  if(ratio) {
    ld min = 0;
    ld max = 10;
    for(int it=0; it<100; it++) {
      ld x = (min + max) / 2;
      ld f = sin_auto(2*x) - ratio * sin_auto(x);
      if(f > 0 ^ spherical) max = x;
      else min = x;
      }
    circ1 = sin_auto(min);
    circ2 = sin_auto(2*min);
    }
  
  ld rad1 = asin_auto(circ1);
  ld rad2 = asin_auto(circ2);
  
  vid.linewidth *= lw;
  
  shiftmatrix at = shiftless(Id); //  * xpush(rad2);
  
  ld kdegree = 2 * M_PI / prec;
  
  if(1) {
    for(int i=0; i<=prec; i++)
      curvepoint(spin(i*kdegree) * xpush(rad2) * C0);      
    queuecurve(at, 0xFF0000FF, 0x200000FF, PPR::LINE);
    }

  for(int i=0; i<=prec; i++)
    curvepoint(spin(t*kdegree) * xpush(rad2-rad1) * spin(i*kdegree) * xpush(rad1) * C0);
  queuecurve(at, 0x00FF00FF, 0x002000FF, PPR::LINE);
  
  for(int q=0; q<360; q+=36) queueline(
    at * spin(t*kdegree) * xpush(rad2-rad1) * C0, 
    at * spin(t*kdegree) * xpush(rad2-rad1) * spin(q*degree-t*kdegree*circ2/circ1) * xpush(rad1) * C0,
    0xFFD500FF, 4);
  
  for(int i=0; i<=prec; i++)
    curvepoint(spin(i*kdegree) * xpush(rad2-rad1) * spin(-i*kdegree*circ2/circ1) * xpush(rad1) * C0);

  queuecurve(at, 0xFFFFFFFF, 0, PPR::LINE);

  for(int i=0; i<=prec; i++)
    curvepoint(spin(t*kdegree) * xpush(rad2-rad1) * spin(-t*kdegree*circ2/circ1) * xpush(rad1) * spin(i*kdegree) * xpush(show) * C0);
  queuecurve(at, 0xFFFFFFFF, 0xFFFF, PPR::LINE);    

  vid.linewidth /= lw;
  }

auto shot_hooks = addHook(hooks_frame, 100, circ_frame)
  + addHook(hooks_configfile, 100, [] {
    param_f(circ1, "circ1");
    param_f(circ2, "circ2");
    param_f(show, "show");
    param_f(loops, "loops");
    param_f(ratio, "ratio");
    param_i(prec, "prec");
    param_f(lw, "lw");
    });

}
