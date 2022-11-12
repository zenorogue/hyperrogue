#include "rogueviz.h"

// implementation of this demo: https://twitter.com/ZenoRogue/status/1304130700870901761

// compile with: ./mymake rogueviz/pentaroll.cpp

// ./hyper -canvas-random 10 -noplayer -geo 534h -to-fq 72414D0C -pentaroll 1 -sight3 6 -animperiod 30000 -shot-1000

// or -pentaroll 0 if you want to explore yourself

namespace hr {

namespace pentaroll {

void animate();

void create_pentaroll(bool animated) {
  start_game();
  /* get the list of all close cells */
  cell *c0 = currentmap->gamestart();
  celllister cl(c0, 4, 1000, nullptr);
  
  /* construct the relative matrices for them */
  map<cell*, transmatrix> rel;
  rel[c0] = Id;
  for(cell *c: cl.lst)
    for(int i=0; i<c->type; i++)
      if(rel.count(c->move(i)))
        rel[c] = rel[c->move(i)] * currentmap->iadj(c, i);

  /* the construction */
  for(cell *c: cl.lst) {
    int common = 0;
    for(auto& v: currentmap->get_cellshape(c).vertices_only_local)
    for(auto& w: currentmap->get_cellshape(c).vertices_only_local)
      if(hdist(v, rel[c] * w) < 1e-6)
        common++;
      
    setdist(c, 7, nullptr);
    
    if(c == c0) c->wall = waPalace;
    else if(common > 0) c->wall = waNone;
    }      
  
  c0->wall = waNone;
  for(int i=1; i<=cgi.face; i++) {
    int i0 = i;
    if(cgi.face == 4 && i0 >= 3) i0++;
    cellwalker cw(c0, i0);
    cw.peek()->wall = waPlatform;      
    if(cgi.face == 5)  {
      cellwalker cw1 = currentmap->strafe(cw, (i==1?cgi.face:i-1));
      cw1.peek()->wall = waWaxWall;
      cw1.peek()->landparam = hrand(0x1000000) | 0x808080;
      }
    }
  if(animated) 
    rogueviz::rv_hook(anims::hooks_anim, 100, animate);
  }

/* currently not configurable */

/* if <1, stay closer to the center */
ld how_far = 1;
ld orig_distance = 1;
ld far_distance = 1;

void animate() {

  centerover = currentmap->gamestart();
  
  ld t = ticks * 20. / anims::period;
  
  int tb = int(t) % cgi.face;
  
  hyperpoint m;
  for(int i=0; i<cgi.face; i++) m += cgi.heptshape->vertices_only[i];
  m /= cgi.face;
  
  auto normm = [&] (hyperpoint h) {
    return normalize(lerp(m, h, how_far));
    };
  
  hyperpoint h1 = normm(cgi.heptshape->vertices_only[tb]);
  hyperpoint h2 = normm(cgi.heptshape->vertices_only[(tb+1) % cgi.face]);
  hyperpoint h3 = normm(cgi.heptshape->vertices_only[(tb+2) % cgi.face]);
  
  hyperpoint a = gpushxto0(h2) * h1;
  hyperpoint b = gpushxto0(h2) * h3;
  b = spintox(a) * b;
  a = spintox(a) * a;
  b[3] = 0;
  println(hlog, "a = ", a);
  println(hlog, "b = ", b);

  b /= hypot_d(3, b);
  ld angle = acos(b[0]);

  println(hlog, "b = ", b);
  println(hlog, "angle = ", angle / degree, " deg");
  
  ld tf = t - floor(t);
  
  /* make it more smooth */
  tf = tf * tf * (3-2*tf);
  
  hyperpoint h = lerp(h1, h2, tf);

  h = normalize(h);
  
  set_view(h, h2, h3);  
  
  /* set_view does not orient correctly, so we rotate it */
  
  View = cspin90(2, 0) * View;
  
  /* we move the camera backward */

  View = zpush(lerp(orig_distance, far_distance, frac(ticks * 1. / anims::period))) * View;
  
  /* we also need to rotate the view to make it smooth */
  
  View = spin((angle-M_PI) * int(t)) * View;
  
  anims::moved();
  }

auto hooks = arg::add3("-pentaroll", [] { create_pentaroll(arg::shift_argi()); });

}

}