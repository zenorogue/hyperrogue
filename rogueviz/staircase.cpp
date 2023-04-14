// Hyperbolic Rogue: staircase simulation in RogueViz
// Copyright (C) 2011-2018 Zeno and Tehora Rogue, see 'hyper.cpp' for details
// see: https://www.youtube.com/watch?v=HZNRo6mr5pk

#include "rogueviz.h"

namespace rogueviz { namespace staircase {

ld scurvature = 0;
ld acurvature = 0;

ld global_r;

ld szoom = 5;

ld progress = 0;

basic_textureinfo tinf;

hyperpoint spcoord(hyperpoint h) {
  ld phi = h[0], y = h[1], z = h[2], r = global_r;
  ld aphi = (r+phi + floor(progress))*30._deg;
  return zpush(acurvature*(y + r - frac(progress))/szoom) * xspinpush0(aphi, acurvature * z);
  }

using tv = glhr::textured_vertex;

tv pt(hyperpoint h, hyperpoint c) {
  tv result;
  h = spcoord(h);
  result.coords[0] = h[0];
  result.coords[1] = h[1];
  result.coords[2] = h[2];
  result.coords[3] = h[3];
  result.texture[0] = c[0];
  result.texture[1] = c[1];
  result.texture[2] = c[2];
  return result;
  }

vector<glvertex> vertices;

void add_pt(tv tv) {
  vertices.push_back(tv.coords);
  glvertex t;
  for(int i=0; i<3; i++) t[i] = tv.texture[i];
  tinf.tvertices.push_back(t);
  }

void tri(tv a, tv b, tv c) { add_pt(a); add_pt(b); add_pt(c); }

void addRect(hyperpoint h, hyperpoint hx, hyperpoint hy, hyperpoint v, hyperpoint vx, hyperpoint vy, int ix, int iy) {
  vector<vector<tv> > rps(iy+1, vector<tv> (ix+1));
  for(int y=0; y<=iy; y++)
  for(int x=0; x<=ix; x++) {
    rps[y][x] = pt(h+hx*x/ix+hy*y/iy, v+vx*x/ix+vy*y/iy);
    }
  for(int y=0; y<iy; y++)
  for(int x=0; x<ix; x++) {
    tri(rps[y][x], rps[y+1][x], rps[y][x+1]),
    tri(rps[y+1][x+1], rps[y][x+1], rps[y+1][x]);
    }
  }

void addTri(hyperpoint h, hyperpoint hx, hyperpoint hy, hyperpoint v, hyperpoint vx, hyperpoint vy, int i) {
  vector<vector<tv> > rps(i+1, vector<tv> (i+1));
  for(int y=0; y<=i; y++)
  for(int x=0; x<=i; x++) {
    if(x+y <= i)
      rps[y][x] = pt(h+hx*x/i+hy*y/i, v+vx*x/i+vy*y/i);
    }
  for(int y=0; y<i; y++)
  for(int x=0; x<i; x++) {
    if(x+y < i)
      tri(rps[y][x], rps[y+1][x], rps[y][x+1]);
    if(x+y+2 <= i)
      tri(rps[y+1][x+1], rps[y][x+1], rps[y+1][x]);
    }
  }

bool need_texture = true;

#if CAP_TEXTURE
texture::texture_data tdata; // = texture::config.data;
#endif

void make_texture() {
#if CAP_TEXTURE
  if(!need_texture) return;
  printf("make texture\n");
  need_texture = false;
  tdata.whitetexture();
  int tw = tdata.twidth;
  printf("tw = %d\n", tw);
  int fw = tw / 4;
  auto pix = [&] (int k, int x, int y) -> unsigned& {
    return tdata.texture_pixels[y * tw + x + (k&3) * fw + (k>>2) * fw * tw];
    };
  for(int y=0; y<tw; y++)
  for(int x=0; x<tw; x++) 
    pix(0,x,y) = rand();
  for(int y=0; y<=fw; y++)
  for(int x=0; x<=fw; x++) {
    typedef long long ll;
    pix(0,x,y) = 0xFF000000 + 0x10101 * ((x*ll(fw-x)*y*(fw-y)*255)/ll(fw/2)/(fw/2)/(fw-fw/2)/(fw-fw/2));
    pix(2,x,y) = 0xFF400000 + 0x10000 * (y * 63 / fw);
    pix(8,x,y) = 0xFF101010;
    pix(10,x,y) = 0xFF000000 + gradient(0, 0xFFD500, 0, x*(fw-x), fw*fw/4);
    pix(5,x,y) = 0xFF000000 + gradient(0, 0x804000, -1, sin(TAU*8*y/fw), 1);
    pix(7,x,y) = 0xFF000000 + gradient(0, 0x808080, 0, x*ll(fw-x)*y*(fw-y), ll(fw/2)*(fw/2)*(fw-fw/2)*(fw-fw/2));
    }

  tdata.loadTextureGL();
#endif
  }

int savetex;

#if ISWEB
int prec = 1;
int maxr = 100;
#else
int prec = 4;
int maxr = 1000;
#endif

bool on;

void enable_hooks();

void reset_geom(eGeometry g) {
  set_geometry(g);
  start_game();
  View = Id;
  enable_hooks();
  }

void make_staircase() {

  // vid.stereo_mode = current_display->sODS;

  println(hlog, "scurvature = ", scurvature, " progress = ", progress);
  if(scurvature > -1e-6 && scurvature < 1e-6) { 
    if(WDIM == 2 || !euclid) reset_geom(gCubeTiling);
    acurvature = 1;
    }
  else if(scurvature < 0) {
    if(WDIM == 2 || !hyperbolic) reset_geom(gSpace534);
    acurvature = -scurvature;
    }
  else {
    if(WDIM == 2 || !sphere) reset_geom(gCell120);
    acurvature = scurvature;
    }

  // rug::ruggospeed = acurvature;
  vid.ipd = 0.15 * acurvature;
  staircase::on = true;
  if(need_texture) {
    make_texture();
    }
  vertices.clear();
  tinf.tvertices.clear();
  printf("compute\n");
  for(int r=-maxr; r<maxr; r++) {
    if(scurvature < -1e-6 && abs(r * acurvature) > 7*12) continue;
    if(scurvature > 1e-6 && abs(acurvature*r/szoom) > M_PI) continue;
    global_r = r;
    // step
    addRect(point31(0,0,1), point30(0,0,1), point30(1,0,0), hpxy(0,0), hpxy(.25,0), hpxy(0,.25), prec, prec);
    // step connection
    addRect(point31(1,0,1), point30(0,0,1), point30(0,1,0), hpxy(0.75,0.25), hpxy(.25,0), hpxy(0,.25), prec, prec);
    // triangle inner side
    addTri(point31(1,0,1), point30(0,1,0), point30(-1,0,0), hpxy(.5,0), hpxy(.125,.125), hpxy(0,.125), prec);
    // rectangle under triangle
    addRect(point31(0,0,1), point30(1,1,0), point30(0,1,0), hpxy(.5,.125), hpxy(.125,0), hpxy(0,.125), prec, prec);
    // barrier post
    addRect(point31(.45,0,1.1), point30(.1,.1,0), point30(0,-3,0), hpxy(0,.5), hpxy(.25,0), hpxy(0,.25), 2, prec);
    // barrier
    addRect(point31(.45,-3,1), point30(0,0,.2), point30(1,1,0), hpxy(.5,.5), hpxy(.25,0), hpxy(0,.25), 1, prec);
    // outer wall
    addRect(point31(0,0,2), point30(1,1,0), point30(0,12,0), hpxy(.25,.25), hpxy(.25,0), hpxy(0,.25), prec, prec);
    // lower wall
    addRect(point31(0,1,1), point30(1,1,0), point30(0,0,1), hpxy(.5,0), hpxy(.25,0), hpxy(0,.25), prec, prec);
    }    
  printf("push\n");
  printf("done (%d points)\n", isize(tinf.tvertices));
  }

// -0.50 .. 0.16

void showMenu() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("Spiral Staircase"), iinf[itPalace].color, 150, 0);
  
  add_edit(scurvature);
  add_edit(prec);
  add_edit(maxr);

  dialog::addBreak(100);
  dialog::addBoolItem(XLAT("low quality"), prec == 1, '1');
  dialog::add_action([] { prec = 1; maxr = 100; make_staircase(); });
  dialog::addBoolItem(XLAT("medium quality"), prec == 2, '2');
#if !ISWEB
  dialog::addBoolItem(XLAT("high quality"), prec == 4, '3');
#endif
  dialog::addItem(XLAT("take me back"), 'q');

  dialog::display();
  }

void draw_staircase() {
  shiftmatrix Zero = ggmatrix(cwt.at);
  auto &p = queuepoly(Zero, cgi.shGem[0], 0xFFFFFFFF);
  p.tab = &vertices;
  p.cnt = isize(vertices);
  p.flags |= POLY_TRIANGLES;
  p.offset = 0;
  p.offset_texture = 0;
  p.tinf = &tinf;
  #if CAP_TEXTURE
  tinf.texture_id = tdata.textureid;
  #endif
  }

void enable_hooks() {
  rv_hook(hooks_o_key, 80, [] (o_funcs& v) { v.push_back(named_dialog("staircase", showMenu)); });
  rv_hook(hooks_frame, 100, draw_staircase);
  }

void enable() {
  enable_hooks();
  make_staircase();  
  make_texture();
  }

int phooks = arg::add3("-stair", enable)
  + addHook_rvslides(141, [] (string s, vector<tour::slide>& v) {
    if(s != "mixed") return;
    using namespace tour;
    v.push_back(
      tour::slide{"Spiral Staircase", 62, LEGAL::NONE | QUICKGEO,
     "Spiral Staircase Demo.\n\n"
     "In Euclidean geometry, when we look at a spiral staircase from above, "
     "we see a hyperbolic spiral -- not a logarithmic spiral, as some people claim. "
     "However, in hyperbolic geometry, we would see a (roughly) logarithmic spiral.\n\n"
     "Press '5' to change the curvature or other parameters.",
     
    [] (presmode mode) {
      setCanvas(mode, '0');
      if(mode == pmStart) staircase::make_staircase();
      slidecommand = "staircase menu";
      if(mode == pmKey) pushScreen(staircase::showMenu);
      slide_url(mode, 'y', "YouTube link", "https://www.youtube.com/watch?v=HZNRo6mr5pk");
      }}
      );
    })
  + addHook(hooks_configfile, 100, [] {
    param_f(scurvature, "stair_curvature")
    ->editable(-1, 1, .05, XLAT("curvature of the space"), "", 'c')
    ->set_reaction(make_staircase)
    ->set_extra([] {
      dialog::addItem("golden spiral", 'G');
      dialog::add_action([] {
        scurvature = -4 * log((sqrt(5)+1)/2) / 2.4;
        make_staircase();
        popScreen();
        });
      });
    param_i(prec, "stair_precision")
    ->editable(1, 8, 1, XLAT("higher value = better"), "staircase precision", 'p')
    ->set_sets([] { dialog::bound_low(1); })
    ->set_reaction(make_staircase);
    param_i(maxr, "stair_maxr")
    ->editable(1, 5000, 100, XLAT("higher value = more levels"), "staircase max", 'm')
    ->set_sets([] { dialog::bound_low(1); })
    ->set_reaction(make_staircase);
    });

}}
