#include "rogueviz.h"

#define PSHIFT 0

#define RVPATH HYPERPATH "rogueviz/"

namespace rogueviz {

#ifndef RV_ALL
namespace cylon {
  extern void enable();
  extern bool cylanim;
  }

namespace nilcompass {
  bool draw_compass(cell *c, const shiftmatrix& V);
  extern int zeroticks;
  }

namespace balls {
  struct ball {
    hyperpoint at;
    hyperpoint vel;
    };
  extern vector<ball> balls;
  extern void initialize(int);
  }
}

namespace hr { 

namespace bricks {
  extern int animation;
  void enable();
  extern void build(bool in_pair);
  extern void build_stair();

  struct brick {
    euc::coord co;
    color_t col;
    int walls;
    hyperpoint location;
    hpcshape shRotWall[6];
    };
  
  extern vector<brick> bricks;
  }

namespace pentaroll {
  extern void create_pentaroll(bool);
  }

namespace ply {
  extern bool animated;
  void enable();
  extern rogueviz::objmodels::model staircase;
  }
#endif
}

namespace hr {

namespace dmv {

transmatrix xyzscale(ld x) {
  transmatrix T = Id;
  T[0][0] = T[1][1] = T[2][2] = x;
  return T;
  }

using namespace rogueviz::pres;
using namespace hr::tour;

struct dmv_grapher : grapher {

  dmv_grapher(transmatrix U) : grapher(-4, -4, 11, 11) {
    T = T * U;
    
    for(int x=-3; x<=10; x++) if(x) {
      line(p2(x,-3), p2(x,10), 0x8080FFFF);
      line(p2(-3,x), p2(10,x), 0x8080FFFF);
      }
    
    vid.linewidth *= 2;
    arrow(p2(0,-3), p2(0,10), .5);
    arrow(p2(-3,0), p2(10,0), .5);
    vid.linewidth /= 2;
    }
  };

void nil_screen(presmode mode, int id) {
  use_angledir(mode, id == 0);
  setCanvas(mode, '0');
  if(mode == pmStart) {
    slide_backup(pmodel);
    slide_backup(pconf.clip_min);
    slide_backup(pconf.clip_max);
    slide_backup(vid.cells_drawn_limit);
    stop_game(), pmodel = mdHorocyclic, geometry = gCubeTiling, pconf.clip_min = -10000, pconf.clip_max = +100, start_game();
    }
  add_stat(mode, [id] {
    cmode |= sm::SIDE;
    calcparam();

    vid.cells_drawn_limit = 0;
    drawthemap();

    // flat_model_enabler fme;
    initquickqueue();
    
    dmv_grapher g(MirrorZ * cspin(1, 2, .8 * angle) * spin(angle/2));
    
    vid.linewidth *= 3;
    
    ld t = 1e-3;

    if(id == 2) {
      t = inHighQual ? ticks * 4. / anims::period : ticks / 1000.;
      if(t - floor(t) > .5) t = ceil(t);
      else t = floor(t) + 2 * (t - floor(t));
      t -= floor(t/4)*4;
      
      ld t2 = 90._deg * t;
      
      curvepoint(p2(0,0));
      curvepoint(p2(5,5));
      curvepoint(p2(5 + 2 * cos(t2),5 + 2 * sin(t2)));
      curvepoint(p2(0,0));
      
      color_t col = cos(t2) > sin(t2) ? 0xFF808000 : 0x8080FF00;
      
      queuecurve(g.T, col | 0xFF, col | 0x20, PPR::LINE);  
      }
    
    if(id < 3) {
      g.arrow(p2(5,5), p2(7,5), .3);
      g.arrow(p2(5,5), p2(5,7), .3);
      g.arrow(p2(5,5), p2(3,5), .3);
      g.arrow(p2(5,5), p2(5,3), .3);
      }

    vid.linewidth /= 3;
    
    if(id < 3) {

      if(id == 2) {
        drawMonsterType(moEagle, nullptr, g.pos(5,5,1.5) * spin(-t * 90._deg) * xyzscale(1.5), 0x40C040, ticks / 1000., 0);
        }
    
      color_t dark = 0xFF;

      write_in_space(g.pos(7.5, 5, 1) * MirrorY, max_glfont_size, 1., "E", dark);
      write_in_space(g.pos(5, 7.5, 1) * MirrorY, max_glfont_size, 1., "N", dark);
      write_in_space(g.pos(2.5, 5, 1) * MirrorY, max_glfont_size, 1., "W", dark);
      write_in_space(g.pos(5, 2.5, 1) * MirrorY, max_glfont_size, 1., "S", dark);
      }
    
    if(id == 3) {
      vid.linewidth *= 3;
      t = ticks / anims::period;
      ld ti = ticks / 1000.;
      t = frac(t);
      vector<hyperpoint> loop = { p2(9, 4), p2(9, 9), p2(1, 9), p2(1, 5), p2(6, 6), p2(9,4) };
      int q = isize(loop) - 1;

      for(hyperpoint h: loop) curvepoint(h);
      queuecurve(g.T, 0x40C040FF, 0, PPR::LINE);
      ld total_length = 0;
      for(int i=0; i<q; i++) total_length += hypot_d(2, loop[i+1] - loop[i]);
      t *= total_length;
      t *= 1.2;
      curvepoint(p2(0,0));
      shiftmatrix T1 = g.pos(loop[0][0], loop[0][1], 1.5);
      shiftmatrix T2 = g.pos(loop[0][0], loop[0][1], 1.5);

      ld t1 = t;

      for(int i=0; i<=q; i++) {
        curvepoint(loop[i]);
        if(i == q) {
          T1 = g.pos(loop[i][0],loop[i][1],1.5) * rspintox(loop[i] - loop[i-1]);
          break;
          }
        ld len = hypot_d(2, loop[i+1] - loop[i]);
        if(len < t1) { t1 -= len; continue; }
        hyperpoint cur = lerp(loop[i], loop[i+1], t1 / len);
        T1 = g.pos(cur[0],cur[1],1.5) * rspintox(loop[i+1] - loop[i]);
        curvepoint(cur);
        break;
        }
      curvepoint(p2(0,0));
      color_t col = 0x8080FF00;
      queuecurve(g.T, col | 0xFF, col | 0x40, PPR::LINE);

      ld z = 0;

      ld zsca = .05;

      vector<pair<hyperpoint, hyperpoint> > vlines;

      for(int i=0; i<=q; i++) {
        vlines.emplace_back(loop[i], loop[i] + ztangent(z));
        curvepoint(loop[i] + ztangent(z));
        if(i == q) {
          T2 = g.pos(loop[i][0],loop[i][1],1.5) * cpush(2, z) * rspintox(loop[i] - loop[i-1]);
          break;
          }
        ld len = hypot_d(2, loop[i+1] - loop[i]);
        if(len < t) {
          t -= len;
          z += (loop[i+1][1] * loop[i][0] - loop[i+1][0] * loop[i][1]) * zsca;
          continue;
          }
        hyperpoint cur = lerp(loop[i], loop[i+1], t / len);
        z += (cur[1] * loop[i][0] - cur[0] * loop[i][1]) * zsca;
        T2 = g.pos(cur[0],cur[1],1.5) * cpush(2, z) * rspintox(loop[i+1] - loop[i]);
        curvepoint(cur + ztangent(z));
        break;
        }
      queuecurve(g.T, 0x40C040FF, 0, PPR::LINE);

      for(auto l: vlines) queueline(g.T*l.first, g.T*l.second, 0x40, 0, PPR::MONSTER_BODY);

      vid.linewidth /= 3;

      drawMonsterType(moEagle, nullptr, T2, 0x40C040, ti, 0);
      auto& bp = cgi.shEagle;
      if(bp.she > bp.shs && bp.she < bp.shs + 1000) {
        auto& p = queuepolyat(T1, bp, 0x80, PPR::TRANSPARENT_SHADOW);
        p.outline = 0;
        p.subprio = -100;
        p.offset = bp.shs;
        p.cnt = bp.she - bp.shs;
        p.flags &=~ POLY_TRIANGLES;
        p.tinf = NULL;
        }
      // queuepolyat(T2, cgi.shEagle, 0x40C040FF, PPR::SUPERLINE);
      }

    quickqueue();
    glflush();

    dialog::init();        
    // dialog::addTitle(id ? "Nil coordinates" : "Cartesian coordinates", forecolor, 150);
    
    poly_outline = 0xFFFFFFFF;
    
    dialog::addBreak(100);
    dialog::addBreak(50);
    auto dirbox = [] (string s) {
      return "\\makebox[5em][r]{\\textsf{" + s + "}} ";
      };
    auto cbox = [] (string s) {
      return "\\makebox[9em][l]{$" + s + "$} ";
      };
    dialog_may_latex(dirbox("start:") + cbox("(x,y,z)"), "start: (x,y,z)");
    dialog::addBreak(50);

    if(id == 0) {
      dialog_may_latex(dirbox("N:") + cbox("(x,y+d,z)"), "N: (x,y+d,z)");
      dialog_may_latex(dirbox("W:") + cbox("(x-d,y,z)"), "W: (x-d,y,z)");
      dialog_may_latex(dirbox("S:") + cbox("(x,y-d,z)"), "S: (x,y-d,z)");
      dialog_may_latex(dirbox("E:") + cbox("(x+d,y,z)"), "E: (x+d,y,z)");
      }
    else {
      dialog_may_latex(dirbox("N:") + cbox("(x,y+d,z+\\frac{xd}{2})"), "N: (x,y+d,z+xd/2)", t == 1 ? 0xFFD500 : dialog::dialogcolor);
      dialog_may_latex(dirbox("W:") + cbox("(x-d,y,z+\\frac{yd}{2})"), "W: (x-d,y,z+yd/2)", t == 2 ? 0xFFD500 : dialog::dialogcolor);
      dialog_may_latex(dirbox("S:") + cbox("(x,y-d,z-\\frac{xd}{2})"), "S: (x,y-d,z-xd/2)", t == 3 ? 0xFFD500 : dialog::dialogcolor);
      dialog_may_latex(dirbox("E:") + cbox("(x+d,y,z-\\frac{yd}{2})"), "E: (x+d,y,z-yd/2)", t == 0 ? 0xFFD500 : dialog::dialogcolor);
      }
    dialog::addBreak(50);
    dialog_may_latex(dirbox("U:") + cbox("(x,y,z-d)"), "U: (x,y,z-d)");
    dialog_may_latex(dirbox("D:") + cbox("(x,y,z+d)"), "D: (x,y,z+d)");
    dialog::display();
    
    dynamicval<eGeometry> gg(geometry, gNil);

    return false;
    });
  }

ld geo_zero;

void geodesic_screen(presmode mode, int id) {
  if(mode == pmStart) geo_zero = ticks;

  use_angledir(mode, id == 0);
  
  setCanvas(mode, '0');
  if(mode == pmStart) {
    slide_backup(pmodel);
    slide_backup(pconf.clip_min);
    slide_backup(pconf.clip_max);
    slide_backup(vid.cells_drawn_limit);
    stop_game(), pmodel = mdHorocyclic, geometry = gCubeTiling, pconf.clip_min = -10000, pconf.clip_max = +100, start_game();
    }
  
  add_stat(mode, [id] {
    cmode |= sm::SIDE;
    calcparam();
    
    vid.cells_drawn_limit = 0;
    drawthemap();

    // flat_model_enabler fme;
    initquickqueue();

    dmv_grapher g(MirrorZ * cspin(1, 2, .3 * angle / 90._deg) * spin(angle/2));
    
    ld val = 25;
    
    ld rv = sqrt(val);
    
    // pi*rad*rad == 25
    ld rad = sqrt(val / M_PI);

    ld rr = rad * sqrt(1/2.);
    
    ld radh = sqrt(val / M_PI - 2);
    ld rrh = radh * sqrt(1/2.);
    
    ld zmove = val - M_PI * radh * radh;
    ld len = hypot(TAU * radh, zmove);
    
    ld t = inHighQual ? ticks / 1000. : (ticks - geo_zero) / 500;

    auto frac_of = [&] (ld z) { return t - z * floor(t/z); };
    
    t = frac_of(val);

    auto draw_path = [&] (auto f, color_t col) {
      vid.linewidth *= 5;
      for(ld t=0; t<=25; t+=1/16.) curvepoint(f(t));
      queuecurve(g.T, col, 0, PPR::LINE);
      
      auto be_shadow = [&] (hyperpoint& h) {
        // ld part = 1 - angle / 90._deg;
        // h[0] += h[2] * part / 10;
        h[2] = 0;
        };

      for(ld t=0; t<=25; t+=1/16.) {
        hyperpoint h = f(t);
        be_shadow(h);
        curvepoint(h);
        }
      queuecurve(g.T, col & 0xFFFFFF40, 0, PPR::LINE);
      vid.linewidth /= 5;
      
      hyperpoint eaglepos = f(t);
      hyperpoint next_eaglepos = f(t + 1e-2);

      // queuepolyat(g.pos(x+z * .1,y,1.5) * spin(s), cgi.shEagle, 0x40, PPR::MONSTER_SHADOW).outline = 0;
      drawMonsterType(moEagle, nullptr, g.T * eupush(eaglepos) * rspintox(next_eaglepos - eaglepos) * xyzscale(2), col >> 8, t, 0);            
      
      be_shadow(eaglepos);
      be_shadow(next_eaglepos);
      
      auto& bp = cgi.shEagle;
      
      println(hlog, tie(bp.shs, bp.she));

      if(bp.she > bp.shs && bp.she < bp.shs + 1000) {
        auto& p = queuepolyat(g.T * eupush(eaglepos) * rspintox(next_eaglepos - eaglepos) * xyzscale(2), bp, 0x18, PPR::TRANSPARENT_SHADOW); 
        p.outline = 0;
        p.subprio = -100;
        p.offset = bp.shs;
        p.cnt = bp.she - bp.shs;
        p.flags &=~ POLY_TRIANGLES;
        p.tinf = NULL;
        return;
        }
      };
      
    color_t straight = 0x80FF80FF;
    color_t square = 0xcd7f32FF;
    color_t circle = 0xaaa9adFF;
    color_t helix  = 0xFFD500FF;

    if(id >= 0)
      draw_path([&] (ld t) { return point31(0, 0, t); }, straight);

    if(id >= 1)
      draw_path([&] (ld t) { 
        if(t < rv)
          return point31(t, 0, 0);
        else if(t < rv*2)
          return point31(rv, t-rv, rv*(t-rv)/2);
        else if(t < rv*3)
          return point31(rv-(t-rv*2), rv, rv*rv/2 + rv*(t-2*rv)/2);
        else if(t < rv*4)
          return point31(0, rv-(t-rv*3), val);
        else
          return point31(0, 0, val);
        }, square);

    if(id >= 2) 
      draw_path([&] (ld t) { 
        ld tx = min(t, TAU * rad);
        ld ta = tx / rad - 135._deg;
        ld x = rr + rad * cos(ta);
        ld y = rr + rad * sin(ta);
        ld z = rad * tx / 2 - ((rr * x) - (rr * y)) / 2;
        return point31(x, y, z);
        }, circle);
      
    if(id >= 3)
      draw_path([&] (ld t) { 
        ld tx = min(t, len);
        ld ta = tx / len * TAU - 135._deg;
        ld x = rrh + radh * cos(ta);
        ld y = rrh + radh * sin(ta);
        ld z = radh * radh * (tx/len*TAU) / 2 - ((rrh * x) - (rrh * y)) / 2 + zmove * tx / len;
        
        return point31(x, y, z);
        }, helix);
    
    auto cat = [] (PPR x) { 
      if(x == PPR::MONSTER_SHADOW) return 1;
      else if(x == PPR::MONSTER_BODY) return 2;
      else return 0;
      };
      
    for(int i=1; i<isize(ptds);)
      if(i && cat(ptds[i]->prio) < cat(ptds[i-1]->prio)) {
        swap(ptds[i], ptds[i-1]);
        i--;
        }
      else i++;

    quickqueue();

    dialog::init();
    dialog_may_latex("\\textsf{from $(0,0,0)$ to $(0,0,25)$}", "from (0,0,0) to (0,0,25)", forecolor, 150);

    dialog::addBreak(100);
    dialog_may_latex("\\textsf{straight upwards}", "straight upwards", straight >> 8);
    dialog_may_latex("$25$", "25", straight >> 8);
    
    if(id >= 1) {
      dialog::addBreak(100);
      dialog_may_latex("\\textsf{square}", "square", square >> 8);
      dialog_may_latex("$20$", "20", square >> 8);
      }
    else dialog::addBreak(300);

    if(id >= 2) {
      dialog::addBreak(100);
      dialog_may_latex("\\textsf{circle}", "circle", circle >> 8);
      dialog_may_latex("$"+fts(TAU * rad)+"$", fts(TAU * rad), circle >> 8);
      }
    else dialog::addBreak(300);

    if(id >= 3) {
      dialog::addBreak(100);
      dialog_may_latex("\\textsf{helix}", "helix", helix >> 8);
      dialog_may_latex("$"+fts(len)+"$", fts(len), helix >> 8);
      }
    else dialog::addBreak(300);
    
    dialog::display();
    
    return false;
    });
  }

// i==0 - stairs
// i==1 - triangle
// i==2 - double triangle

void brick_slide(int i, presmode mode, eGeometry geom, eModel md, int anim) {
  using namespace tour;
  setCanvas(mode, '0');
  if(mode == pmStart) {
    set_geometry(geom);
    start_game();
    cgi.require_shapes();
    if(i == 0)
      bricks::build_stair();
    if(i == 1)
      bricks::build(false);
    if(i == 2)
      bricks::build(true);
    bricks::enable();
    tour::slide_backup(pconf.clip_min, -100.);
    tour::slide_backup(pconf.clip_max, +10.);
    tour::slide_backup(pconf.scale, i ? .2 : 2.);
    tour::slide_backup(mapeditor::drawplayer, false);
    tour::slide_backup(pconf.rotational_nil, 0.);
    tour::slide_backup(vid.axes3, false);
    bricks::animation = anim;
    pmodel = md;
    View = Id;
    }
  clearMessages();
  no_other_hud(mode);
  }

void ply_slide(tour::presmode mode, eGeometry geom, eModel md, bool anim) {
  using namespace tour;
  if(!ply::staircase.available())  {
    slide_error(mode, "(model not available)");
    return;
    }
  if(mode == pmStartAll) {
    rogueviz::objmodels::prec = 10;
    dynamicval<eGeometry> g(geometry, gNil);
    dynamicval<eVariation> v(variation, eVariation::pure);
    dynamicval<int> s(vid.texture_step, 1);
    check_cgi();
    cgi.require_shapes();
    }
  setCanvas(mode, '0');
  if(mode == pmStart) {
    set_geometry(geom);
    start_game();
    ply::enable();
    tour::slide_backup(anims::period, 40000.);
    tour::slide_backup(mapeditor::drawplayer, false);
    tour::slide_backup(pconf.rotational_nil, 0.);
    tour::slide_backup(ply::animated, anim);
    tour::slide_backup(vid.axes3, false);
    tour::slide_backup(no_find_player, true);
    tour::slide_backup(vid.texture_step, 1);
    tour::slide_backup(sightranges[geom], 10.);
    tour::slide_backup(vid.cells_drawn_limit, 50);
    pmodel = md;
    View = Id;
    }
  clearMessages();
  no_other_hud(mode);
  }

void impossible_ring_slide(tour::presmode mode) {
  using namespace tour;
  setCanvas(mode, '0');
  if(mode == pmStart) {
    set_geometry(gCubeTiling);
    start_game();
    tour::slide_backup(pconf.clip_min, -100.);
    tour::slide_backup(pconf.clip_max, +10.);
    tour::slide_backup(mapeditor::drawplayer, false);
    tour::slide_backup(vid.axes3, false);
    pmodel = mdHorocyclic;
    View = Id;
    }
  clearMessages();
  no_other_hud(mode);
  
  use_angledir(mode, true);
  
  add_temporary_hook(mode, hooks_frame, 200, [] {
    for(int id=0; id<2; id++) {
      shiftmatrix T = ggmatrix(currentmap->gamestart());
      println(hlog, "angle = ", angle);
      if(id == 1) T = T * spin180() * xpush(1.5) * cspin(0, 2, angle) * xpush(-1.5);
      
      for(ld z: {+.5, -.5}) {
        for(ld d=0; d<=180; d++)
          curvepoint(C0 + spin(d*degree) * xtangent(1) + ztangent(z));
        for(ld d=180; d>=0; d--)
          curvepoint(C0 + spin(d*degree) * xtangent(2) + ztangent(z));
        curvepoint(C0 + spin(0) * xtangent(1) + ztangent(z));
        queuecurve(T, 0xFF, 0xFF8080FF, PPR::LINE);
        }
      for(ld d=0; d<180; d+=5) for(ld x: {1, 2}) {
        for(int i=0; i<=5; i++)
          curvepoint(C0 + spin((d+i)*degree) * xtangent(x) + ztangent(.5));
        for(int i=5; i>=0; i--)
          curvepoint(C0 + spin((d+i)*degree) * xtangent(x) + ztangent(-.5));
        curvepoint(C0 + spin((d+0)*degree) * xtangent(x) + ztangent(.5));
        queuecurve(T, 0xFF, 0xC06060FF, PPR::LINE);
        }
      
      for(ld sgn: {-1, 1}) {
        curvepoint(C0 + xtangent(sgn * 1) + ztangent(+.5));
        curvepoint(C0 + xtangent(sgn * 2) + ztangent(+.5));
        curvepoint(C0 + xtangent(sgn * 2) + ztangent(-.5));
        curvepoint(C0 + xtangent(sgn * 1) + ztangent(-.5));
        curvepoint(C0 + xtangent(sgn * 1) + ztangent(+.5));
        
        queuecurve(T, 0xFF, 0x804040FF, PPR::LINE);
        }
      }
    });  
  }

void enable_earth() {
  texture::texture_aura = true;
  stop_game();
  set_geometry(gSphere);
  enable_canvas();
  patterns::whichCanvas = 'F';
  start_game();        
  texture::config.configname = "textures/earth.txc";
  texture::config.load();
  pmodel = mdDisk;
  pconf.alpha = 1000; pconf.scale = 999;
  texture::config.color_alpha = 255;
  mapeditor::drawplayer = false;
  fullcenter();
  View = spin(234._deg) * View;
  }

slide dmv_slides[] = {
  {"Title Page", 123, LEGAL::ANY | QUICKSKIP | NOTITLE, "", 
    [] (presmode mode) {
      empty_screen(mode);
      show_picture(mode, "rogueviz/nil/penrose-triangle.png");
      add_stat(mode, [] {
        cmode |= sm::DARKEN;
        gamescreen();
        dialog::init();
        dialog::addBreak(400);
        dialog::addTitle("playing with impossibility", dialog::dialogcolor, 150);
        dialog::addBreak(1600);
        dialog::addTitle("a presentation about Nil geometry", 0xFFC000, 75);
        dialog::display();
        return true;
        });
      no_other_hud(mode);
      }
    },
  {"Euclidean plane", 999, LEGAL::NONE | QUICKGEO, 
    "The sum of angles of a triangle is 180 degrees.\n\n",
    [] (presmode mode) {
      if(mode == pmStartAll) enable_canvas();
      setCanvas(mode, 'F');
      if(mode == pmStart) {
        stop_game();
        slide_backup(firstland, laCanvas);        
        slide_backup(specialland, laCanvas);

        set_geometry(gArchimedean); arcm::current.parse("3^6");
        set_variation(eVariation::pure);

        slide_backup(colortables['F'][0], 0xC0FFC0);
        slide_backup(colortables['F'][1], 0x80FF80);
        slide_backup(pconf.alpha, 1); 
        slide_backup(pconf.scale, 1); 
        start_game();
        slide_backup(patterns::whichShape, '9');
        slide_backup(vid.use_smart_range, 2);
        slide_backup(mapeditor::drawplayer, false);
        }
      add_temporary_hook(mode, hooks_frame, 200, [] {
        shiftmatrix T = ggmatrix(currentmap->gamestart());
        vid.linewidth *= 4;
        shiftpoint h1 = T * xspinpush0(0, 2);
        shiftpoint h2 = T * xspinpush0(120._deg, 2);
        shiftpoint h3 = T * xspinpush0(240._deg, 2);
        queueline(h1, h2, 0xFF0000FF, 4);
        queueline(h2, h3, 0xFF0000FF, 4);
        queueline(h3, h1, 0xFF0000FF, 4);
        vid.linewidth /= 4;
        });
      no_other_hud(mode);
      }
    },
  {"spherical triangles", 999, LEGAL::NONE | QUICKGEO, 
    "The simplest non-Euclidean geometry is the geometry on the sphere.\n\n"
    "Here we see a spherical triangle with three right angles.\n\n"
    "For creatures restricted to just this surface, they are indeed striaght lines!\n\n"
    ,
    [] (presmode mode) {
      setCanvas(mode, '0');
      if(mode == pmStart) {
        tour::slide_backup(mapeditor::drawplayer, false);
        enable_earth();
      
        View = Id;
        View = spin(108._deg) * View;
        View = spin(90._deg) * View;
        View = cspin(2, 0, 45._deg) * View;
        View = cspin(1, 2, 30._deg) * View;
        playermoved = false;
        tour::slide_backup(vid.axes, 0);
        tour::slide_backup(vid.drawmousecircle, false);
        tour::slide_backup(draw_centerover, false);
        }
      add_temporary_hook(mode, hooks_frame, 200, [] {
        shiftmatrix T = ggmatrix(currentmap->gamestart()) * spin(-108._deg);
        vid.linewidth *= 4;
        shiftpoint h1 = T * C0;
        shiftpoint h2 = T * xpush0(90._deg);
        shiftpoint h3 = T * ypush0(90._deg);
        queueline(h1, h2, 0xFF0000FF, 3);
        queueline(h2, h3, 0xFF0000FF, 3);
        queueline(h3, h1, 0xFF0000FF, 3);
        vid.linewidth /= 4;
        });
      if(mode == pmStop) {
        texture::config.tstate = texture::tsOff;
        }
      no_other_hud(mode);
      }
    },
  {"hyperbolic plane", 999, LEGAL::NONE | QUICKGEO, 
    "Hyperbolic geometry works the opposite way to spherical geometry."
    "In hyperbolic geometry, the sum of angles of a triangle is less than 180 degrees.\n\n",
    [] (presmode mode) {
      if(mode == pmStartAll) enable_canvas();
      setCanvas(mode, 'F');
      if(mode == pmStart) {
        stop_game();
        slide_backup(firstland, laCanvas);        
        slide_backup(specialland, laCanvas);
        set_geometry(gNormal);
        set_variation(eVariation::bitruncated);
        slide_backup(colortables['F'][0], 0xC0FFC0);
        slide_backup(colortables['F'][1], 0x80FF80);
        slide_backup(pconf.alpha, 1); 
        slide_backup(pconf.scale, 1); 
        slide_backup(rug::mouse_control_rug, true);
        start_game();
        slide_backup(patterns::whichShape, '9');
        }
      if(mode == pmStart) {
        rug::modelscale = 1;
        // rug::rug_perspective = false;
        rug::gwhere = gCubeTiling;
        rug::texturesize = 1024;
        slide_backup(sightrange_bonus, -1);
        drawthemap();
        rug::init();
        }
      if(mode == pmStart) {
        stop_game();
        set_geometry(gArchimedean); arcm::current.parse("3^7");
        set_variation(eVariation::pure);
        start_game();
        }
      add_temporary_hook(mode, hooks_frame, 200, [] {
        shiftmatrix T = ggmatrix(currentmap->gamestart());
        vid.linewidth *= 16;
        shiftpoint h1 = T * xspinpush0(0, 2);
        shiftpoint h2 = T * xspinpush0(120._deg, 2);
        shiftpoint h3 = T * xspinpush0(240._deg, 2);
        queueline(h1, h2, 0xFF0000FF, 4);
        queueline(h2, h3, 0xFF0000FF, 4);
        queueline(h3, h1, 0xFF0000FF, 4);
        vid.linewidth /= 16;
        });
      if(mode == 3) {
        rug::close();
        }
      no_other_hud(mode);
      }
    },
  {"A right-angled pentagon", 999, LEGAL::NONE | QUICKGEO, 
    "There is also three-dimensional hyperbolic geometry.\n"
    "Here are some right-angled pentagons in three-dimensional hyperbolic space.\n"
    ,
    
    [] (presmode mode) {
      if(mode == pmStart) {
        slide_backup(patterns::rwalls, 10);
        slide_backup(vid.fov, 120);
        }

      setCanvas(mode, '0');
      
      if(mode == pmStart) {
        set_geometry(gSpace534);
        /*
        static bool solved = false;
        if(!solved) {
          stop_game();
          set_geometry(gSpace534);
          start_game();
          stop_game();
          cgi.require_basics();
          fieldpattern::field_from_current();
          set_geometry(gFieldQuotient);
          currfp.Prime = 5; currfp.force_hash = 0x72414D0C;
          currfp.solve();
          solved = true;
          }          
        set_geometry(gFieldQuotient);
        */
        start_game();
        tour::slide_backup(mapeditor::drawplayer, false);
        pentaroll::create_pentaroll(true);
        tour::slide_backup(anims::period, 30000.);
        tour::slide_backup(sightranges[geometry], 4);
        start_game();
        playermoved = false;
        }
      no_other_hud(mode);
      }
    },

  {"Penrose triangle (1958), Oscar Reutersvärd's triangle (1934), Penrose Stairs (1959)", 999, LEGAL::NONE, 
    "The Penrose Triangle, "
    "constructed by Lionel Penrose and Roger Penrose in 1958, "
    "is an example of an impossible figure. "
    "Many artists have used the Penrose Triangle to create "
    "more complex constructions, such as the \"Waterfall\" "
    "by M. C. Escher.\n\n"
    
    "While it is known as Penrose Triangle, a very similar construction "
    "has actually been discovered earlier by Oscar Reutersvärd (in 1934)!\n\n"

    "In 1959 Lionel Penrose and Roger Penrose have constructed another "
    "example of an impossible figure, called the Penrose staircase.",
    [] (presmode mode) {
      empty_screen(mode);
      show_picture(mode, "rogueviz/nil/penrose-all-small.png");
      no_other_hud(mode);
      }
    },

  {"Ascending & Descending", 999, LEGAL::NONE | QUICKGEO, 
    "It is the most well known from \"Ascending and Descending\" by M. C. Escher.\n\n"
    "This is a 3D model of Ascending and Descending by Lucian B. It is based on an optical illusion."
    ,
    [] (presmode mode) {
      slide_url(mode, 'm', "link to the original model", "https://3dwarehouse.sketchup.com/model/3e6df6c24a95f583cefabc2ae69d584c/MC-Escher-Ascending-and-Descending");
      ply_slide(mode, gCubeTiling, mdPerspective, false);
      if(!ply::staircase.available()) return;
      if(mode == pmStart) {
        tour::slide_backup(sightranges[geometry], 200);
        tour::slide_backup(vid.cells_drawn_limit, 200);
        tour::slide_backup(camera_speed, 5);
        centerover = euc::get_at(euc::coord{12,-23,8})->c7;
        playermoved = false;
        int cid = 0;
        for(ld val: {0.962503,0.254657,-0.0934754,0.000555891,0.0829357,-0.604328,-0.792408,0.0992114,-0.258282,0.754942,-0.602787,0.0957558,0.,0.,0.,1.})
          View[0][cid++] = val;

        // tour::slide_backup(vid.fov, 120);
        }
      non_game_slide_scroll(mode);
      if(mode == pmKey) {
        println(hlog, ggmatrix(currentmap->gamestart()));
        println(hlog, View);
        println(hlog, euc::get_ispacemap()[centerover->master]);
        }
      }
    },

/*
  {"Penrose triangle (1958)", 999, LEGAL::NONE, 
    "The Penrose Triangle, "
    "constructed by Lionel Penrose and Roger Penrose in 1958, "
    "is an example of an impossible figure. "
    "Many artists have used the Penrose Triangle to create "
    "more complex constructions, such as the \"Waterfall\" "
    "by M. C. Escher.",
    
    [] (presmode mode) {
      empty_screen(mode);
      show_picture(mode, "rogueviz/nil/penrose-triangle.png");
      no_other_hud(mode);
      }
    },
  {"Oscar Reutersvärd's triangle (1934)", 999, LEGAL::NONE, 
    "While it is known as Penrose Triangle, a very similar construction "
    "has actually been discovered earlier by Oscar Reutersvärd (in 1934)!",
    [] (presmode mode) {
      empty_screen(mode);
      show_picture(mode, "rogueviz/nil/reutersvard.png");
      no_other_hud(mode);
      }
    },
  {"Penrose staircase (1959)", 999, LEGAL::NONE, 
    "In 1959 Lionel Penrose and Roger Penrose have constructed another "
    "example of an impossible figure, called the Penrose staircase.\n\n"
    "It is the most well known from \"Ascending and Descending\" by M. C. Escher.\n\n",
    [] (presmode mode) {
      empty_screen(mode);
      show_picture(mode, "rogueviz/nil/penrose-stairs.png");
      no_other_hud(mode);
      }
    }, */
  {"non-Euclidean geometry so far", 123, LEGAL::ANY, 
    "People sometimes call such impossible constructions \"non-Euclidean\".\n\n"
    "These people generally use this name because they do not know the usual "
    "mathematical meaning of \"non-Euclidean\".\n\n"
    "It seems that the geometries we know so far are something completely different...",
  
    [] (presmode mode) {
      empty_screen(mode);
      add_stat(mode, [] {
        dialog::init();
        color_t d = dialog::dialogcolor;

        dialog::addTitle("Euclidean geometry", 0xC00000, 200);
        dialog::addTitle("parallel lines stay in the same distance", d, 150);
        
        dialog::addBreak(100);

        dialog::addTitle("spherical geometry", 0xC00000, 200);
        dialog::addTitle("no parallel lines -- they converge", d, 150);

        dialog::addBreak(100);

        dialog::addTitle("hyperbolic geometry", 0xC00000, 200);
        dialog::addTitle("parallel lines diverge", d, 150);

        dialog::display();
        return true;
        });
      non_game_slide_scroll(mode);
      }
    },

  {"Compasses in Nil", 123, LEGAL::ANY | QUICKGEO,
    "However, it turns out that there actually exists a non-Euclidean geometry, "
    "known as the Nil geometry, where constructions such as Penrose staircases and "
    "triangles naturally appear!\n\n"
    "Nil is a three-dimensional geometry, which gives new possibilities -- "
    "lines 'diverge in the third dimension' there. "
    "Every point has "
    "well-defined North, East, South, West, Up and Down direction.\n\n"
    "(press Home/End and arrow keys to move)",

    [] (presmode mode) {
      setCanvas(mode, '0');
      slidecommand = "highlight dimensions";
      if(mode == pmStart) {
        tour::slide_backup(pmodel, mdGeodesic);
        set_geometry(gNil);
        start_game();
        rogueviz::rv_hook(hooks_drawcell, 100, rogueviz::nilcompass::draw_compass);
        View = Id;
        shift_view(ztangent(.5));
        }
      non_game_slide_scroll(mode);
      if(mode == pmStart || mode == pmKey)
        rogueviz::nilcompass::zeroticks = ticks;
      }
    },

  {"a Puzzle about a Bear", 123, LEGAL::ANY,
    "To explain Nil geometry, we will start with a well-known puzzle.",
  
    [] (presmode mode) {
      empty_screen(mode);
      add_stat(mode, [] {
        dialog::init();
        color_t d = dialog::dialogcolor;
        dialog::addTitle("A bear walked five kilometers north, ", d, 150);
        dialog::addTitle("five kilometers east, five kilometers south, ", d, 150);
        dialog::addTitle("and returned exactly to the place it started.", d, 150);
        dialog::addBreak(50);
        dialog::addTitle("What color is the bear?", 0xC00000, 200);
        dialog::display();
        return true;
        });
      no_other_hud(mode);
      }
    },

  {"Cartesian coordinates", 999, LEGAL::NONE | QUICKGEO, 
    "The puzzle shows an important fact: every point on Earth has defined directions "
    "(North, East, South, West), and in most life situations, we can assume that these "
    "directions work the same as in the Cartesian system of coordinates."
    ,
    [] (presmode mode) {
      empty_screen(mode);
      nil_screen(mode, 0);
      no_other_hud(mode);
      }
    },
  {"Nil coordinates", 999, LEGAL::NONE | QUICKGEO,
    "However, because Earth is curved (non-Euclidean), these directions actually "
    "work different! If you are closer to the pole, moving East or West changes "
    "your longitude much more quickly.\n\n"
    "Nil is a three-dimensional geometry which is similar: while every point also has "
    "well-defined NSEWUD directions, but they affect the coordinates in a different way " 
    "than in the Euclidean space with the usual coordinate system.\n\n"

    "You may want to use the Pythagorean theorem to compute the length of these -- "
    "this is not correct, all the moves are of length d. You would need to use the Pythagorean "
    "theorem if you wanted to compute the length from (x,y,z) to (x,y-d,z).\n\n"
    ,
    [] (presmode mode) {
      empty_screen(mode);
      nil_screen(mode, 1);
      no_other_hud(mode);
      }
    },
  {"Nil coordinates (area)", 999, LEGAL::NONE | QUICKGEO,
    "The formulas look strange at a first glance, but the idea is actually simple: "
    "the change in the 'z' coordinate is the area of a triangle, as shown in the picture. "
    "The change is positive if we go counterclockwise, and negative if we go clockwise.\n\n"
    ,
    [] (presmode mode) {
      empty_screen(mode);
      nil_screen(mode, 2);
      no_other_hud(mode);
      }
    },
  {"Nil coordinates (loop)", 999, LEGAL::NONE | QUICKGEO,
    "If we make a tour in Nil moving only in the directions N, W, S, E, such that "
    "the analogous tour in Euclidean space would return us to the starting point, "
    "then the tour in Nil would return us directly above or below the starting point, "
    "with the difference in the z-coordinate proportional to the area of the loop."
    ,
    [] (presmode mode) {
      empty_screen(mode);
      nil_screen(mode, 3);
      no_other_hud(mode);
      }
    },
  {"Simple Penrose stairs", 999, LEGAL::NONE | QUICKGEO, 
    "This lets us easily make a simple realization of the Penrose staircase in Nil. "
    "Here is an attempt to create a Penrose staircase in Euclidean geometry...\n\n"
    "(you can rotate this with mouse or arrow keys)"
    ,
    [] (presmode mode) {
      brick_slide(0, mode, gCubeTiling, mdHorocyclic, 0);
      non_game_slide_scroll(mode);
      }
    },
  {"Simple Penrose stairs in Nil", 999, LEGAL::NONE | QUICKGEO, 
    "We can use the magic of the Nil geometry to recompensate the lost height.\n\n"
    "Press 5 to see how it looks when we walk around the stairs. When you rotate this slide, "
    "you will notice that the stairs change shape when far from the central point -- "
    "this is because we use the Nil rules of movement."
    ,
    [] (presmode mode) {
      brick_slide(0, mode, gNil, mdHorocyclic, 0);
      if(mode == pmKey) bricks::animation = !bricks::animation;
      non_game_slide_scroll(mode);
      }
    },
  {"Simple Penrose stairs in Nil (FPP)", 999, LEGAL::NONE | QUICKGEO, 
    "This slide shows our stairs in the first person perspective, from the inside."
    ,
    [] (presmode mode) {
      brick_slide(0, mode, gNil, mdPerspective, 3);
      if(mode == pmKey) bricks::animation ^= 1;
      }
    },
  {"Geodesics in Nil", 999, LEGAL::NONE | QUICKGEO, 
    "But, was the first person perspective in the last slide 'correct'?\n\n"
    "According to Fermat's Principle, the path taken by a light ray is "
    "always one which is the shortest. Our previous visualization assumed "
    "that light rays move in a fixed 'direction', which may be not the case.\n\n"
    "Let's think a bit about moving from (0,0,0) to (0,0,25). We can of course "
    "take the obvious path of length 25. Can we do it better?"
    ,
    [] (presmode mode) {
      empty_screen(mode);
      geodesic_screen(mode, 0);
      no_other_hud(mode);
      }
    },
  {"Geodesics: square", 999, LEGAL::NONE | QUICKGEO, 
    "Yes, we can! Here is a square of edge length 5. Since such a square has an "
    "area of 25 and perimeter of 20, it takes us to (0,0,25) in just 20 steps!"
    ,
    [] (presmode mode) {
      empty_screen(mode);
      geodesic_screen(mode, 1);
      no_other_hud(mode);
      }
    },
  {"Geodesics: circle", 999, LEGAL::NONE | QUICKGEO, 
    "We can do even better. Queen Dido already knew that among shapes with the "
    "given area, the circle has the shortest perimeter. A circle with area 25 "
    "has even shorter length."
    ,
    [] (presmode mode) {
      empty_screen(mode);
      geodesic_screen(mode, 2);
      no_other_hud(mode);
      }
    },
  {"Geodesics: helix", 999, LEGAL::NONE | QUICKGEO, 
    "But that was just the silver medal.\n\n"
    "For the gold medal, we need to combine the 'silver' and 'green' paths. "
    "We make the circle slightly smaller, and we satisfy the difference by moving "
    "slightly upwards. The length of such path can be computed using the Pythagorean "
    "theorem, and minimized by differentiation. There is an optimal radius which "
    "yields the best path.",
    
    [] (presmode mode) {
      empty_screen(mode);
      geodesic_screen(mode, 3);
      no_other_hud(mode);
      }
    },
  {"Simple Penrose stairs in Nil (geodesics)", 999, LEGAL::NONE | QUICKGEO, 
    "The light ray paths ('geodesics') in Nil are like the ones constructed in "
    "the last slide: they are helices, the steeper the helix, the smaller "
    "its radius.\n\n"
    "This slide presents the staircase in model perspective and the "
    "geodesically correct view. The geodesically correct view appears to spin."
    ,
    [] (presmode mode) {
      brick_slide(0, mode, gNil, mdGeodesic, 3);
      
      compare_projections(mode, mdPerspective, mdGeodesic);      
      }
    },
  {"Penrose triangle (illusion)", 999, LEGAL::NONE | QUICKGEO, 
    "Can we also construct the Penrose triangle? "
    "Yes, we can! In our space, we can construct an illusion "
    "which looks like the Penrose triangle (rotate the scene and press '5'). "
    "If we rotate this illusion in such a way that the 'paradox line' "
    "is vertical, we can recompensate the difference by using the Nil geometry. "
    "We need to scale our scene in such a way that the length of the white line "
    "equals the area contained in the projection of the red line."
    ,
    [] (presmode mode) {
      brick_slide(1, mode, gCubeTiling, mdHorocyclic, 0);
      static bool draw = false;
      if(mode == pmKey) draw = !draw;
      add_temporary_hook(mode, hooks_prestats, 200, [] {
        if(draw) {
          shiftmatrix Zero = ggmatrix(currentmap->gamestart());
          
          initquickqueue();
          
          // two first bricks are fake

          int id = 0;
          for(auto& b: bricks::bricks) {
            id++;
            if(id >= 2) curvepoint(b.location);
            }
          vid.linewidth *= 10;
          queuecurve(Zero, 0x0000FFFF, 0, PPR::SUPERLINE).flags |= POLY_FORCEWIDE;
          vid.linewidth /= 10;

          curvepoint(bricks::bricks[2].location);
          curvepoint(bricks::bricks.back().location);
          vid.linewidth *= 10;
          queuecurve(Zero, 0xFFFFFFFF, 0, PPR::SUPERLINE).flags |= POLY_FORCEWIDE;
          vid.linewidth /= 10;
          
          quickqueue();
          
          }
        return false;
        });
      non_game_slide_scroll(mode);
      // pmodel = (pmodel == mdGeodesic ? mdPerspective : mdGeodesic);
      }
    },
  {"Penrose triangle (Nil)", 999, LEGAL::NONE | QUICKGEO, 
    "Here we move around the Penrose triangle..."
    ,
    [] (presmode mode) {
      brick_slide(1, mode, gNil, mdHorocyclic, 1);
      // if(mode == pmKey) DRAW 
      // pmodel = (pmodel == mdGeodesic ? mdPerspective : mdGeodesic);
      non_game_slide_scroll(mode);
      }
    },
  {"Penrose triangle (FPP)", 999, LEGAL::NONE | QUICKGEO, 
    "... and see the Penrose triangle in first-person perspective. "
    "Since the Penrose triangle is larger (we need stronger Nil effects "
    "to make it work), the geodesic effects are also much stronger."
    ,
    [] (presmode mode) {
      brick_slide(1, mode, gNil, mdPerspective, 3);
      compare_projections(mode, mdPerspective, mdGeodesic);
      }
    },

  {"Improbawall by Matt Taylor (emty01)", 999, LEGAL::NONE | QUICKGEO, 
    "This impossible construction by Matt Taylor was popular in early 2020. "
    "How does it even work?\n\n"
    "(the animation is not included with RogueViz)"
    ,
    [] (presmode mode) {
      static bool pic_exists, video_exists;
      if(mode == pmStartAll || mode == pmStart) {
        pic_exists = file_exists("rogueviz/nil/emty-ring.png");
        video_exists = file_exists("rogueviz/nil/emty-ring.mp4");
        }
      slide_url(mode, 'i', "Instagram link", "https://www.instagram.com/p/B756GCynErw/");
      empty_screen(mode);
      if(video_exists)
        show_animation(mode, "rogueviz/nil/emty-ring.mp4", 720, 900, 300, 30);
      else if(pic_exists)
        show_picture(mode, "rogueviz/nil/emty-ring.png");
      else
        slide_error(mode, "(image not available)");
      no_other_hud(mode);
      }
    },
  {"how is this made", 999, LEGAL::NONE | QUICKGEO, 
    "Rotate this ring and press '5' to rotate a half of it by 90 degrees. "
    "After rotating this ring so that the endpoints agree, we get another "
    "case that can be solved in Nil geometry."
    ,
    [] (presmode mode) {
      impossible_ring_slide(mode);
      non_game_slide_scroll(mode);
      }
    },
  {"impossible ring in Nil", 18, LEGAL::NONE | QUICKGEO, 
    "Here is how it looks in Nil. Press '5' to animate.\n",
   
  [] (presmode mode) {
    setCanvas(mode, '0');
    
    slidecommand = "animation";
    if(mode == pmKey) {
      tour::slide_backup(rogueviz::cylon::cylanim, !rogueviz::cylon::cylanim);
      }
    
    if(mode == pmStart) {
      stop_game();
      set_geometry(gNil);
      rogueviz::cylon::enable();
      tour::on_restore(nilv::set_flags);
      tour::slide_backup(nilv::nilperiod, make_array(3, 3, 3));
      nilv::set_flags();
      start_game();
      }
    non_game_slide_scroll(mode);
    }},

  {"3D model (geodesic)", 999, LEGAL::NONE | QUICKGEO, 
    "What if we try to move something more complex, rather than a simple geometric shape?\n\n"
    "This slide is based on a 3D model of Ascending and Descending by Lucian B. "
    "We have used the trick mentioned before to move into the Nil space. Here are the results."
    ,
    [] (presmode mode) {
      slide_url(mode, 'y', "YouTube link", "https://www.youtube.com/watch?v=DurXAhFrmkE");
      ply_slide(mode, gNil, mdGeodesic, true);
      }
    },
  {"3D model (perspective)", 999, LEGAL::NONE | QUICKGEO, 
    "The same in the simple model."
    ,
    [] (presmode mode) {
      ply_slide(mode, gNil, mdPerspective, true);
      }
    },
  {"two Penrose triangles (Euc)", 999, LEGAL::NONE | QUICKGEO, 
    "Here are two Penrose triangles. Can we move that to Nil?"
    ,
    [] (presmode mode) {
      brick_slide(2, mode, gCubeTiling, mdHorocyclic, 0);
      non_game_slide_scroll(mode);
      }
    },
  {"two Penrose triangles (Nil)", 999, LEGAL::NONE | QUICKGEO, 
    "No, we cannot -- one of the triangles has opposite orientation!\n\n"
    "That is still impossible in Nil, so not all "
    "impossible constructions can be realized in Nil.\n\n"
    "For example, \"Waterfall\" by M. C. Escher is based on three "
    "triangles with two different orientations.",
    [] (presmode mode) {
      brick_slide(2, mode, gNil, mdHorocyclic, 0);
      non_game_slide_scroll(mode);
      }
    },

  {"Balls in Nil", 999, LEGAL::NONE | QUICKGEO | FINALSLIDE, 
    "A perpetuum mobile in Nil as the final slide. That's all for today!"
    ,
    [] (presmode mode) {
      slide_url(mode, 'y', "YouTube link", "https://www.youtube.com/watch?v=mxvUAcgN3go");
      slide_url(mode, 'n', "Nil Rider", "https://zenorogue.itch.io/nil-rider");
      setCanvas(mode, '0');
      if(mode == pmStart) {
        stop_game();
        set_geometry(gNil);
        check_cgi();
        cgi.require_shapes();
        start_game();
        rogueviz::balls::initialize(1);
        rogueviz::balls::balls.resize(3);
        pmodel = mdEquidistant;
        View = cspin90(1, 2);
        }
      non_game_slide_scroll(mode);
      }
    },

  {"final slide", 123, LEGAL::ANY | NOTITLE | QUICKSKIP | FINALSLIDE, 
    "FINAL SLIDE",
  
    [] (presmode mode) {
      empty_screen(mode);
      add_stat(mode, [] {
        dialog::init();
        color_t d = dialog::dialogcolor;

        dialog::addTitle("Thanks for your attention!", 0xC00000, 200);

        dialog::addBreak(100);

        dialog::addTitle("twitter.com/zenorogue/", d, 150);
        
        dialog::display();
        return true;
        });
      no_other_hud(mode);
      }
    }
  };
  
int phooks = 
  0 +
  addHook_slideshows(100, [] (tour::ss::slideshow_callback cb) {
    cb(XLAT("Playing with Impossibility"), &dmv_slides[0], 'i');
    });
 
}

}

// kolor zmienic dziada
