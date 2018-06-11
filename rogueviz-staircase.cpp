// Hyperbolic Rogue: staircase simulation in RogueViz
// Copyright (C) 2011-2018 Zeno and Tehora Rogue, see 'hyper.cpp' for details

// Kohonen's self-organizing networks. 
// This is a part of RogueViz, not a part of HyperRogue.

namespace rogueviz { namespace staircase {

using namespace hyperpoint_vec;

ld scurvature = 0;
ld acurvature = 0;

ld global_r;

ld szoom = 5;

ld progress = 0;

ld strafex, strafey;

hyperpoint spcoord(hyperpoint h) {
  ld phi = h[0], y = h[1], z = h[2], r = global_r;
  dynamicval<eGeometry> gw(geometry, rug::gwhere == gElliptic ? gSphere : rug::gwhere);
  hyperpoint inh = xpush(-acurvature*(y + r - frac(progress))/szoom) * spin(M_PI/2) * xpush(acurvature*z) * C0;
  hyperpoint i = inh * (hdist0(inh) / hypot2(inh));
  ld aphi = (r+phi + floor(progress))*M_PI/6;
  return hpxyz(i[1] * sin(aphi), i[1] * cos(aphi), i[0]);
  }

rug::rugpoint *pt(hyperpoint h, hyperpoint c) {
  auto r = rug::addRugpoint(C0, -1);
  r->flat = spcoord(h);
  r->x1 = c[0];
  r->y1 = c[1];
  r->valid = true;
  return r;
  }

void addRect(hyperpoint h, hyperpoint hx, hyperpoint hy, hyperpoint v, hyperpoint vx, hyperpoint vy, int ix, int iy) {
  using namespace hyperpoint_vec;
  vector<vector<rug::rugpoint*> > rps(iy+1, vector<rug::rugpoint*> (ix+1));
  for(int y=0; y<=iy; y++)
  for(int x=0; x<=ix; x++) {
    rps[y][x] = pt(h+hx*x/ix+hy*y/iy, v+vx*x/ix+vy*y/iy);
    }
  for(int y=0; y<iy; y++)
  for(int x=0; x<ix; x++) 
    rug::addTriangle(rps[y][x], rps[y+1][x], rps[y][x+1]),
    rug::addTriangle(rps[y+1][x+1], rps[y][x+1], rps[y+1][x]);
  }

void addTri(hyperpoint h, hyperpoint hx, hyperpoint hy, hyperpoint v, hyperpoint vx, hyperpoint vy, int i) {
  using namespace hyperpoint_vec;
  vector<vector<rug::rugpoint*> > rps(i+1, vector<rug::rugpoint*> (i+1));
  for(int y=0; y<=i; y++)
  for(int x=0; x<=i; x++) {
    if(x+y <= i)
      rps[y][x] = pt(h+hx*x/i+hy*y/i, v+vx*x/i+vy*y/i);
    }
  for(int y=0; y<i; y++)
  for(int x=0; x<i; x++) {
    if(x+y < i)
      rug::addTriangle(rps[y][x], rps[y+1][x], rps[y][x+1]);
    if(x+y+2 <= i)
      rug::addTriangle(rps[y+1][x+1], rps[y][x+1], rps[y+1][x]);
    }
  }

bool need_texture = true;

#if CAP_TEXTURE
texture::texture_data tdata; // = texture::config.data;
#endif

void make_texture() {
#if CAP_TEXTURE
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
    pix(5,x,y) = 0xFF000000 + gradient(0, 0x804000, -1, sin(2*M_PI*8*y/fw), 1);
    pix(7,x,y) = 0xFF000000 + gradient(0, 0x808080, 0, x*ll(fw-x)*y*(fw-y), ll(fw/2)*(fw/2)*(fw-fw/2)*(fw-fw/2));
    }

  tdata.loadTextureGL();
#endif
  }

int savetex;

int prec = 4;

int maxr = 1000;

bool on;

void make_staircase() {

  // stereo::mode = stereo::sODS;
  // stereo::set_viewport(0);
  rug::no_fog = true;

  printf("scurvature = %lf progress = %lf strafe=%lf,%lf\n", scurvature, progress, strafex, strafey);
  rug::renderonce = true;
  rug::rug_perspective = true;
  if(scurvature > -1e-6 && scurvature < 1e-6) { 
    rug::gwhere = gEuclid;
    acurvature = 1;
    }
  else if(scurvature < 0) {
    rug::gwhere = gNormal;
    acurvature = -scurvature;
    }
  else {
    rug::gwhere = gSphere;
    acurvature = scurvature;
    }
  rug::ruggospeed = acurvature;
  stereo::ipd = 0.15 * acurvature;
  if(!rug::rugged || !staircase::on) {
    staircase::on = true;
    rug::reopen();
    }
  if(need_texture) {
    make_texture();
#if CAP_TEXTURE
    rug::alternate_texture = tdata.textureid;
#endif
    }
  rug::clear_model(); 
  printf("compute\n");
  for(int r=-maxr; r<maxr; r++) {
    if(scurvature < -1e-6 && abs(r * acurvature) > 7*12) continue;
    if(scurvature > 1e-6 && abs(acurvature*r/szoom) > M_PI) continue;
    global_r = r;
    // step
    addRect(hpxyz(0,0,1), hpxyz(0,0,1), hpxyz(1,0,0), hpxy(0,0), hpxy(.25,0), hpxy(0,.25), prec, prec);
    // step connection
    addRect(hpxyz(1,0,1), hpxyz(0,0,1), hpxyz(0,1,0), hpxy(0.75,0.25), hpxy(.25,0), hpxy(0,.25), prec, prec);
    // triangle inner side
    addTri(hpxyz(1,0,1), hpxyz(0,1,0), hpxyz(-1,0,0), hpxy(.5,0), hpxy(.125,.125), hpxy(0,.125), prec);
    // rectangle under triangle
    addRect(hpxyz(0,0,1), hpxyz(1,1,0), hpxyz(0,1,0), hpxy(.5,.125), hpxy(.125,0), hpxy(0,.125), prec, prec);
    // barrier post
    addRect(hpxyz(.45,0,1.1), hpxyz(.1,.1,0), hpxyz(0,-3,0), hpxy(0,.5), hpxy(.25,0), hpxy(0,.25), 2, prec);
    // barrier
    addRect(hpxyz(.45,-3,1), hpxyz(0,0,.2), hpxyz(1,1,0), hpxy(.5,.5), hpxy(.25,0), hpxy(0,.25), 1, prec);
    // outer wall
    addRect(hpxyz(0,0,2), hpxyz(1,1,0), hpxyz(0,12,0), hpxy(.25,.25), hpxy(.25,0), hpxy(0,.25), prec, prec);
    // lower wall
    addRect(hpxyz(0,1,1), hpxyz(1,1,0), hpxyz(0,0,1), hpxy(.5,0), hpxy(.25,0), hpxy(0,.25), prec, prec);
    }    
  printf("push\n");
  rug::push_all_points(0, strafex * acurvature);
  rug::push_all_points(1, strafey * acurvature);
  for(auto p: rug::points) p->valid = true;
  rug::good_shape = true;  
  printf("done (%d points)\n", size(rug::points));
  rug::lowrug = 1e-2 * acurvature;
  rug::hirug = 1e3 * acurvature;
  }

// -0.50 .. 0.16

int ctick;

void check() {
  if(ctick && ctick < ticks) {
    calcparam();
    make_staircase();
    ctick = 0;
    }
  if(on && !rug::rugged) {
    on = false;
    rug::alternate_texture = 0;
    rug::no_fog = false;
    rug::ruggospeed = 1;
    staircase::on = false;
    }
  }

void showMenu() {
  cmode = sm::SIDE;
  gamescreen(0);
  dialog::init(XLAT("Spiral Staircase"), iinf[itPalace].color, 150, 0);

  dialog::addSelItem("     " + XLAT("X"), fts(strafex), 'x');
  dialog::addSelItem("     " + XLAT("Y"), fts(strafey), 'y');
  dialog::addSelItem("     " + XLAT("curvature"), fts(scurvature), 'c');

  dialog::addBreak(100);
  dialog::addItem(XLAT("disable menu"), SDLK_ESCAPE);
  dialog::addBoolItem(XLAT("low quality"), prec == 1, '1');
  dialog::addBoolItem(XLAT("medium quality"), prec == 2, '2');
  dialog::addBoolItem(XLAT("high quality"), prec == 4, '3');
  dialog::addItem(XLAT("take me back"), 'q');

  dialog::display();
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    
    if(uni == 'x') {
      dialog::editNumber(strafex, -1, 1, .05, 1, XLAT("X"),
        XLAT("Also changed with keys A/D")
        );
      dialog::reaction = [] () { ctick = ticks + 500; };
      }
    else if(uni == 'y') {
      dialog::editNumber(strafey, -1, 1, .05, 1, XLAT("Y"),
        XLAT("Also changed with keys W/S")
        );
      dialog::reaction = [] () { ctick = ticks + 500; };
      }
    else if(uni == 'c') {
      dialog::editNumber(scurvature, -1, 1, .05, 1, XLAT("curvature"),
        XLAT("Also changed with keys K/L. Press G for the golden spiral")
        );
      dialog::reaction = [] () { ctick = ticks + 500; };
      }
    else if(uni == 'w') {
      strafey += .1;
      ctick = ticks + 200;
      }
    else if(uni == 's') {
      strafey -= .1;
      ctick = ticks + 200;
      }
    else if(uni == 'd') {
      strafex += .1;
      ctick = ticks + 200;
      }
    else if(uni == 'a') {
      strafex -= .1;
      ctick = ticks + 200;
      }
    else if(uni == 'k') {
      scurvature += .02;
      ctick = ticks + 200;
      }
    else if(uni == 'l') {
      scurvature -= .02;
      ctick = ticks + 200;
      }
    else if(uni == 'p') {
      progress += .1;
      ctick = ticks + 200;
      }
    else if(uni == 'g') {
      scurvature = -4 * log((sqrt(5)+1)/2) / 2.4;
      ctick = ticks + 200;
      }
    else if(uni == '1') {
      prec = 1, maxr = 100;
      make_staircase();
      }
    else if(uni == '2') {
      prec = 2, maxr = 300;
      make_staircase();
      }
    else if(uni == '3') {
      prec = 4, maxr = 1000;
      make_staircase();
      }
    else if(uni == 'q') {
      ctick = 0;
      rug::close();
      popScreen();
      }
    else if(doexiton(sym, uni)) popScreen();
    };
  }

#if CAP_COMMANDLINE
int readArgs() {
  using namespace arg;

  // #1: load the samples
  
  if(argis("-stair")) {
    showstartmenu = false;
    ctick = ticks + 2000;
    pushScreen(showMenu);
    }

  else return 1;
  return 0;
  }
#endif

int phooks = addHook(hooks_args, 100, readArgs)
  + addHook(hooks_fixticks, 100, check);

}}
