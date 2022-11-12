#include "rogueviz.h"

/** \brief Immersed plane
 *
 *  Euclidean plane immersed in various geometries.
 *
 *  Compile with HyperRogue, enable a 3D geometry, and press oo to view or to change the direction.
 *
 **/

namespace rogueviz {

namespace embed {

ld scale = 1;

color_t gridcol = 0xFFFFFFFF;

hyperpoint fp(ld x, ld y) {
  x *= scale;
  y *= scale;
  x *= A_PI/10 * sqrt(2)/2;
  y *= A_PI/10 * sqrt(2)/2;
  if(euclid)
    return hyperpoint(x, y, 0, 1);
  
  if(sphere) {
    ld a = sqrt(2)/2;
    x /= a;
    y /= a;
    hyperpoint h = cspin(2, 3, -45._deg) * hyperpoint(sin(x)*a, sin(y)*a, cos(x)*a, cos(y)*a);
    return h;
    }

  if(hyperbolic)
    return cspin90(2, 0) * tC0(parabolic13(x, y));
  
  if(nil)
    return hyperpoint(0, y, x, 1);
  
  if(prod)
    return zpush(y) * xpush0(x);

  if(sl2) {
    ld a = sqrt(2)/2;
    x /= a;
    y /= a;
    hyperpoint h = cspin(2, 3, -45._deg) * hyperpoint(sinh(x)*a, sinh(y)*a, cosh(x)*a, cosh(y)*a);
    return h;
    }

  if(sol || nih)
    return hyperpoint(x, y, 0, 1);
  
  return hyperpoint(x, y, 0, 1);
  }

void embed(const transmatrix& V, ld x, ld y, ld x1, ld y1) {
  queueline(V* fp(x, y), V*fp(x1, y1), gridcol, 0, PPR::LINE);
  }

void draw_grid(const transmatrix& V) {
  for(int x=-10; x<=10; x++) {
    for(int y=-10; y<10; y++) {
      for(int a=0; a<=10; a++)
        curvepoint(V * fp(x, y+a/10.));
      queuecurve(gridcol, 0, PPR::LINE).flags |= POLY_FORCEWIDE | POLY_ONE_LEVEL;

      for(int a=0; a<=10; a++)
        curvepoint(V * fp(y+a/10., x));
      queuecurve(gridcol, 0, PPR::LINE).flags |= POLY_FORCEWIDE | POLY_ONE_LEVEL;
      }
    }
  }

bool draw_embed(cell *c, const transmatrix& V) {

  if(c == cwt.at && false) 
    draw_grid(V);
  
  return false;
  }

int emba;

void relocate() {
  View = Id;
  NLP = Id;
  centerover = currentmap->gamestart();
  
  vid.fixed_yz = false;
  
  if(nil) rotate_view(cspin90(2, 0));
  if(prod) rotate_view(cspin90(1, 2));

  if(emba == 1) {  
    rotate_view(cspin(0, 1, 45._deg));
    rotate_view(cspin(1, 2, 30._deg));
    }
  
  if(emba == 2) {
    rotate_view(cspin(1, 2, 81._deg));
    }
  
  for(int a=0; a<100; a++) 
    shift_view(ztangent(scale/100));
  if(scale < 0) rotate_view(diag(-1,-1,-1,1));
  playermoved = false;
  
  }

int part = 1;

ld wider = 1.5;

void draw_all() {
  vid.linewidth *= wider;
  if(1) {
  
    int id = 0;
    
    auto draw_colored = [&] (eGeometry gg, eGeometry g2, color_t col) {
      dynamicval<eGeometry> g(geometry, gg);
      if(g2 != gNormal) {
        hybrid::configure(g2);
        geometry = g2;
        }
      // dynamicval<color_t> gc(gridcol, col);
      dynamicval<eModel> gm(pmodel, default_model());
      int idx = id % 3;
      int idy = id / 3;
      id++;
      auto& cd = current_display;
      dynamicval<ld> gx1(cd->xmin, idx / 3.);
      dynamicval<ld> gxy(cd->ymin, idy / 3.);
      dynamicval<ld> gx2(cd->xmax, (idx+1) / 3.);
      dynamicval<ld> gy2(cd->ymax, (idy+1) / 3.);
      calcparam();
      dynamicval<ld> gs(scale, scale);
      
      /* verification
      hyperpoint h1 = fp(0, 0);
      hyperpoint h2 = fp(1, 0);
      hyperpoint h3 = fp(0, 1);
      hyperpoint h4 = fp(1, 1);
      println(hlog, geometry_name(), " : ", geo_dist(h1, h2), " = ", geo_dist(h3, h4));
      println(hlog, geometry_name(), " : ", geo_dist(h1, h3), " = ", geo_dist(h2, h4));
      */
      
      if(nih) scale /= log(2);
      relocate();
      initquickqueue();
      make_actual_view();
      draw_grid(cview());
      drawqueue();
      };
            
    draw_colored(gCell120, gNormal, 0x00FF00FF);
    draw_colored(gNil, gNormal, 0xFF0000FF);
    draw_colored(gNormal, gRotSpace, 0x0000FFFF);

    draw_colored(gSphere, gProduct, 0x00FF80FF);
    draw_colored(gNIH, gNormal, 0xFF8000FF);
    draw_colored(gNormal, gProduct, 0x00FFFFFF);

    draw_colored(gSpace534, gNormal, 0xFF0000FF);      
    draw_colored(gSolN, gNormal, 0xFF80FFFF);
    draw_colored(gSol, gNormal, 0xFF00FFFF);
    
    calcparam();

    //draw_colored(gCubeTiling, gNormal, 0xFFFFFFFF);
    
    relocate();
    }
  vid.linewidth /= wider;
  }

void show() {
  dialog::editNumber(scale, -2, 2, 0.1, 1, "distance", "distance");
  dialog::extra_options = [] {
    draw_all();
    
    dialog::addItem("direction", 'A');
    dialog::add_action([] {
      emba ++;
      emba %= 3;
      });
    };
  dialog::reaction = [] { relocate(); };  
  }

void o_key(o_funcs& v) {
  v.push_back(named_functionality("plane embedding", show));
  }

auto hchook = addHook(hooks_drawcell, 100, draw_embed)

+ addHook(hooks_o_key, 80, o_key)

+ addHook(hooks_args, 100, [] {
  using namespace arg;
           
  if(0) ;
  else if(argis("-embed-angle")) {
    shift(); emba = argi();
    }
  else if(argis("-embed-video")) {
    start_game();
    shot::transparent = 0;
    shot::shotx = 500;
    shot::shoty = 500;
    shift();
    anims::record_video(args(), [] {
      for(int i=0; i<360; i++) {
        scale = sin((i+.5) * degree);
        shot::take("x", draw_all);
        }
      return true;
      });
    }
  else return 1;
  return 0;
  });


}
}
