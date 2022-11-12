#include "rogueviz.h"

// Impossible Ring visualization

// used in: 
// https://youtu.be/3WejR74o6II
// https://youtu.be/ztodGQDK810

// usage: -cylon

namespace rogueviz {

namespace cylon {

ld cscale = .2;

ld cylball;

struct iring {

  geometry_information *icgi;

  static const int frames = 32;
  
  static const int cols = 256;

  static const int steps = 2048;    

  array<array<hpcshape, cols>, frames> ptriangle[2];

  vector<color_t> huess[2];
  
  vector<transmatrix> path;
  
  hpcshape ball;
  
  void init() {
  
    icgi = &cgi;
  
    unsigned difh = 256;
    int mh = 255;
    color_t base = 0xFF;
    
    auto& hues = huess[0];

    for(unsigned y=0; y<difh; y++)
      hues.push_back(base + 0x1000000*mh + 0x10000 * y);
    for(unsigned y=0; y<difh; y++)
      hues.push_back(base + 0x1010000*mh - 0x1000000 * y);
    for(unsigned y=0; y<difh; y++)
      hues.push_back(base + 0x0010000*mh + 0x100 * y);
    for(unsigned y=0; y<difh; y++)
      hues.push_back(base + 0x0010100*mh - 0x10000 * y);
    for(unsigned y=0; y<difh; y++)
      hues.push_back(base + 0x0000100*mh + 0x1000000 * y);
    for(unsigned y=0; y<difh; y++)
      hues.push_back(base + 0x1000100*mh - 0x100 * y);      
      
    for(color_t h: hues) huess[1].push_back(((h & 0xFEFEFE00) >> 1) | 0xFF);
  
    ld delta = 0.004;
    
    transmatrix T = cspin(1, 2, 45._deg);
    
    int switchat = nil ? 1024 : 2048;
    
    auto step = [&] (int id) {
      ld alpha = id * 1. / steps * TAU;
      if(id < switchat)
        return T * point3(cos(alpha) * delta, sin(alpha) * delta, 0);
      else
        return T * point3(cos(alpha) * delta, 0, sin(alpha) * delta);
      };
    
    ld dmin = 0, dmax = 1;
    
    auto shift = [&] (ld d) {
      delta = d;

      hyperpoint start = C0;
      
      for(int a=0; a<steps; a++) {
        hyperpoint s = step(a);
        start = rgpushxto0(start) * (C0+s);
        }
      
      println(hlog, "start[", delta, "] = ", kz(start));
      
      return start[2];
      };
    
    println(hlog, shift(0.0001), shift(1));          
    
    if(nil) for(int it=0; it<50; it++) {
      delta = (dmin + dmax) / 2;
      
      ld val = shift(delta);

      if(val > 0) dmax = delta;
      else dmin = delta;
      }
    
    println(hlog, "delta = ", delta);
    
    vector<array<hyperpoint, 4> > pipe;
    
    hyperpoint start = C0;
    
    path.resize(2 * steps);
    
    for(int a=0; a<steps; a++) {

      hyperpoint uds[3];  

      ld alpha = a * 1. / steps * TAU;
      if(a < switchat) {
        uds[0] = T * point31(sin(alpha) * cscale, -cos(alpha) * cscale, 0) - C0;
        uds[1] = T * point31(0, 0, cscale) - C0;
        uds[2] = T * point31(-cos(alpha) * cscale, -sin(alpha) * cscale, 0) - C0;
        }
      else {
        uds[0] = T * point31(0, cscale, 0) - C0;
        uds[1] = T * point31(sin(alpha) * cscale, 0, -cos(alpha) * cscale) - C0;
        uds[2] = T * point31(-cos(alpha) * cscale, 0, -sin(alpha) * cscale) - C0;
        }
  
      // compute cube vertices
      
      array<hyperpoint, 4> verts;      
    
      for(int a=0; a<4; a++) {
        verts[a] = C0;
        for(int k=0; k<2; k++) 
          verts[a] += (a&(1<<k)) ? uds[k] : -uds[k];
        verts[a] = nisot::translate(start) * verts[a];
        }
      
      pipe.push_back(verts);

      path[a] = inverse(build_matrix(uds[0], uds[1], uds[2], C0)) * inverse(rgpushxto0(start));
      path[a+steps] = inverse(build_matrix(-uds[0], -uds[1], uds[2], C0)) * inverse(rgpushxto0(start));
      
      // println(hlog, "gs = ", gpushxto0(start));

      println(hlog, "start @ ", inverse(rgpushxto0(start)) * start);

      hyperpoint s = step(a);
      start = rgpushxto0(start) * (C0 + s);      
      
       // * );
      }
    
    pipe.resize(steps + frames);
    for(int i=0; i<frames; i++)
    for(int j=0; j<4; j++)
      pipe[i+steps][j] = pipe[i][j^3];

    for(int fr=0; fr<frames; fr++)
    for(int sa=0; sa<2; sa++)
    for(int si=0; si<cols; si++) {
      auto textured_square = [&] (auto f) {
        texture_order([&] (ld ix, ld iy) { f(.5 + ix/2 + iy/2, .5 + ix/2 - iy/2); });
        texture_order([&] (ld ix, ld iy) { f(.5 - ix/2 - iy/2, .5 - ix/2 + iy/2); });
        texture_order([&] (ld ix, ld iy) { f(.5 + ix/2 - iy/2, .5 - ix/2 - iy/2); });
        texture_order([&] (ld ix, ld iy) { f(.5 - ix/2 + iy/2, .5 + ix/2 + iy/2); });
        };
  
      auto pipesquare = [&] (hyperpoint a00, hyperpoint a01, hyperpoint a10, hyperpoint a11) {
        textured_square( [&] (ld ix, ld iy) {
          hyperpoint shf = lerp(lerp(a00, a01, ix), lerp(a10, a11, ix), iy);
          // if(cscale) shf = shf * cscale - C0 * (cscale-1);
          cgi.hpcpush(shf);
          });
        };
      
      cgi.bshape(ptriangle[sa][fr][si], PPR::WALL);

      for(int i=fr; i<steps; i+=frames) {
        auto& pi = pipe[i];
        auto& pj = pipe[i+frames];
        
        int val = i * cols / steps / 2;
        
        if(si == val && sa == 0) pipesquare(pi[0], pi[2], pj[0], pj[2]);
        if(si == (val+cols/2)%cols && sa == 0) pipesquare(pi[1], pi[3], pj[1], pj[3]);
        if(si == (val+cols/4)%cols && sa == 1) pipesquare(pi[0], pi[1], pj[0], pj[1]);
        if(si == (val+cols*3/4)%cols && sa == 1) pipesquare(pi[2], pi[3], pj[2], pj[3]);
        }
      
      cgi.last->flags |= POLY_TRIANGLES;
      cgi.last->tinf = &floor_texture_vertices[0];
      cgi.last->texture_offset = 0;
      cgi.finishshape();
      cgi.extra_vertices();
      }
    
    if(cylball) {
      ball.prio = PPR::WALL;
      cgi.make_ball(ball, cylball, 2);
      cgi.finishshape();
      cgi.extra_vertices();
      }
    }
    
  };

iring *ir;

void reset() {
  if(ir) delete ir;
  ir = nullptr;
  }

bool draw_ptriangle(cell *c, const shiftmatrix& V) {

  if(ir && ir->icgi != &cgi) reset();
   
  if(!ir) { ir = new iring; ir->init(); 
    // growthrate();
    }
  
  if(c == cwt.at) {
    int frid = (ticks % 1000) * ir->frames / 1000;
    for(int sa: {0, 1})
    for(int side=0; side<ir->cols; side++) {
      auto &s = queuepoly(V, ir->ptriangle[sa][frid][side], ir->huess[sa][256*6/ir->cols * side]);
      ensure_vertex_number(*s.tinf, s.cnt);

      /* auto& s1 = queuepoly(V * nisot::translate(td.at), ir->pcube[side], gradient(tcolors[td.tcolor], magiccolors[side], 0, .2, 1));
      ensure_vertex_number(*s1.tinf, s1.cnt); */
      }
    
    if(cylball) {
      long long isp = isize(ir->path);
      transmatrix T = ir->path[isp-1 - (ticks * isp / int(anims::period)) % isp];
      T = nisot::parallel_transport(inverse(T), -point3(0, cscale + cylball, 0));
      queuepoly(V * T, ir->ball, 0xFFFFFFFF);
      }
    }

  return false;
  }

bool cylanim = false;

void o_key(o_funcs& v) {
  v.push_back(named_functionality("ring size", [] { 
    dialog::editNumber(cscale, 0, 1, .01, .1, "", "");
    dialog::reaction = reset;
    }));
  }

void enable() {
  rogueviz::rv_hook(hooks_drawcell, 100, draw_ptriangle);
  rogueviz::rv_hook(anims::hooks_anim, 100, [] {
    if(!ir || !cylanim) return;
    centerover = currentmap->gamestart();
    long long isp = isize(ir->path);
    View = ir->path[isp-1 - (ticks * isp / int(anims::period)) % isp];
    shift_view(point3(0, 0.3, 0));
    anims::moved();
    });
  rogueviz::rv_hook(hooks_o_key, 80, o_key);
  }

auto hchook = 

  arg::add3("-cylon", enable)

+ addHook(hooks_configfile, 100, [] {
    param_f(cscale, "cyls");
    param_b(cylanim, "cylanim");
    param_f(cylball, "cylball");
    })

+ addHook_rvslides(167, [] (string s, vector<tour::slide>& v) {
  if(s != "noniso") return;
  using namespace tour;
  v.push_back(
    tour::slide{"Impossible architecture in Nil/impossible ring", 18, LEGAL::NONE | QUICKGEO, 
      "Ring with a square cross-section. Cut it in half. Rotate one of the two halves by 90°, "
      "keeping one of the two ends connected to the other half. Do this in Nil geometry, so the other pair of ends remains connected too.\n\n"
      "Move with mouse/arrows/PgUpDn. Press '5' to enable animation, 'o' to change ring size.\n\n",
   
  [] (presmode mode) {
    slide_url(mode, 'y', "YouTube link", "https://youtu.be/3WejR74o6II");
    setCanvas(mode, '0');
    
    slidecommand = "animation";
    if(mode == pmKey) {
      tour::slide_backup(cylanim, !cylanim);
      }
    
    if(mode == pmStart) {
      stop_game();
      set_geometry(gNil);
      tour::slide_backup(mapeditor::drawplayer, false);
      tour::slide_backup(smooth_scrolling, true);
      tour::on_restore(nilv::set_flags);
      tour::slide_backup(nilv::nilperiod, make_array(3, 3, 3));
      nilv::set_flags();
      start_game();
      playermoved = false;
      enable();
      }
    }}
    );
  });



}}
