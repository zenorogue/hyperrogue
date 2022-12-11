#include "rogueviz.h"

namespace rogueviz {

namespace magic { void magic(int i); }

namespace crystal_sokoban { void run_sb(); }

namespace colorpicker {

int current_step;
color_t current_color;
cell *current_center;

color_t get_color_at(cell *c) {
  crystal::coord oldc = crystal::get_coord(current_center->master);
  crystal::coord newc = crystal::get_coord(c->master);
  color_t res;
  for(int i=0; i<3; i++) {
    int val = part(current_color, 2-i) + current_step * ((newc[i] - oldc[i]) / 2);
    if(val < 0) val = 0;
    if(val > 255) val = 255;
    part(res, 2-i) = val;
    }
  return res;
  }

bool color_markers(cell *c, const shiftmatrix& V) {
  if(!centerover) return false;
  if(centerover != current_center) {
    current_color = get_color_at(centerover);
    current_center = centerover;
    }
  c->landparam = get_color_at(c);
  return false;
  }

bool color_key(int sym, int uni) {
  if((cmode & sm::NORMAL) && (uni >= '0' && uni <= '6')) {
    current_step = (1 << (uni - '0'));
    return true;
    }
  if((cmode & sm::NORMAL) && uni == '[') {
    current_step = (1 + current_step) / 2;
    return true;
    }
  if((cmode & sm::NORMAL) && uni == ']') {
    current_step = 2 * current_step;
    return true;
    }
  if((cmode & sm::NORMAL) && uni >= 1000 && uni < 1010) {
    current_step = 1 << (uni - 1000);
    return true;
    }
  return false;
  }

bool color_prestats() {
  nohelp = true;
  for(int k= 0; k <= 6; k++) {
    int v = 1<< k;
    if(displayButtonS(10 + k * vid.fsize * 2, 10 + vid.fsize, its(k), v == current_step ? 0xFF2020 : 0xC0C0C0, 0, vid.fsize)) 
      getcstat = 1000 + k;
    }
  if(mouseover) {
    displayButtonS(10 + 7 * vid.fsize * 2, 10 + vid.fsize, itsh(mouseover->landparam & 0xFFFFFF), mouseover->landparam, 0, vid.fsize); 
    }
  return true;
  }

void run_cpick() {
  crystal::compass_probability = 0;
  stop_game();
  crystal::set_crystal(6);
  set_variation(eVariation::pure);
  enable_canvas();
  patterns::whichCanvas = 'g';
  patterns::canvasback = 0;
  check_cgi();
  start_game();
  current_center = currentmap->gamestart();
  current_color = 0x808080;    
  current_step = 32;
  mapeditor::drawplayer = false;
  vid.smart_range_detail = 1;
  vid.use_smart_range = 2;
  rv_hook(hooks_drawcell, 100, color_markers);
  rv_hook(hooks_handleKey, 50, color_key);
  rv_hook(hooks_prestats, 150, color_prestats);
  }

auto cphook = addHook(hooks_args, 100, [] {
  using namespace arg;
           
  if(0) ;
  else if(argis("-cpick")) {
    PHASEFROM(2);
    run_cpick();
    }
  else return 1;
  return 0;
  });
  }

namespace sokoban {

bool on;

bool created;

hpcshape sokowall[10][7];

hyperpoint tpoint[6];

void push3(hyperpoint h) {
  cgi.first = false;
  cgi.hpc.push_back(h);
  }

void create_sokowalls(cell *c) {
  for(int a=0; a<6; a++) tpoint[a] = currentmap->adj(c, a) * C0;
  const int qfr = 8;
  auto v = [&] (int a, int b, int fr) {
    hyperpoint h0 = get_corner_position(c, b);
    hyperpoint h1 = get_corner_position(c, b+1);
    hyperpoint h2 = normalize(h0 * (qfr-fr) + h1 * fr);
    return orthogonal_move_fol(h2, 1 / (1 - a / 6.1));
    };
  
  for(int a=0; a<9; a++) 
  for(int b=0; b<6; b++) {
    cgi.bshape(sokowall[a][b], PPR::FLOOR + 2 * a + 1);
    for(int f=0; f<=qfr; f++)
      push3(v(a, b, f));
    for(int f=0; f<=qfr; f++)
      push3(v(a+1, b, qfr-f));
    push3(v(a, b, 0));
    }

  for(int a=0; a<9; a++) {
    cgi.bshape(sokowall[a][6], PPR::FLOOR + 2 * a);
    for(int b=0; b<6; b++)
      for(int f=0; f<qfr; f++)
        push3(v(a, b, f));
    push3(v(a, 0, 0));
    }
  
  cgi.finishshape();
  cgi.extra_vertices();
  }

bool sokomap(cell *c, const shiftmatrix& V) {
  if(!created) {
    created = true;
    create_sokowalls(c);
    }

  crystal::coord v = crystal::get_coord(c->master);
  bool high = (v[0] ^ v[1] ^ v[2]) & 2;
  

  color_t col = 0x00C000FF;
  
  poly_outline = 0xFF;
  
  int lev = 0;
  
  int phase = ((v[0] ^ v[2] ^ v[4]) & 2) >> 1;
  
  // high = v[0] >= 0 && v[0] <= 8 && v[1] >= 0 && v[1] <= 8 && v[2] == 0 && !(v[0] > 0 && v[0] < 8 && v[1] > 0 && v[1] < 8);
  // if(high) col = 0xFFFF00FF, lev = 3;
  
  high = true;
  lev = ((v[0] + v[1] + v[2])/2+1) & 3;
  if(lev == 0) col = 0x008000FF;
  if(lev == 1) col = 0x00C000FF;
  if(lev == 2) col = 0x40C000FF;
  if(lev == 3) col = 0x80FF00FF;
  
  // if(v[0] == 4 && v[1] == 4 && v[2] == 0) col = 0xFFFFFFFF;
  
  c->landparam = lev;

  if(high) {
    ld d = hdist0(tC0(V));
    color_t dark1 = col - ((col & 0xFCFCFC00) >> 1);
    color_t dark2 = col - ((col & 0xFCFCFC00) >> 2);
    for(int b=0; b<6; b++) 
      for(int a=c->move(b)->landparam; a<lev; a++) if(hdist0(V * tpoint[b]) < d) {
        /* auto& p = */
        queuepoly(V, sokowall[a][b], (((b+phase)&1) ? dark1 : dark2));
        // p.flags |= POLY_PRECISE_WIDE;
        // p.linewidth = 2;
        }
    queuepoly(V, sokowall[lev][6], col);
    }
  else {
    queuepoly(V, sokowall[0][6], col);
    }

  c->wall = waInvisibleFloor;
  return false;
  }

void run_sb() {
  crystal::compass_probability = 0;
  stop_game();
  crystal::set_crystal(6);
  set_variation(eVariation::pure);
  enable_canvas();
  patterns::whichCanvas = 'g';
  patterns::canvasback = 0;
  check_cgi();
  rv_hook(hooks_drawcell, 100, sokomap);
  start_game();
  mapeditor::drawplayer = false;
  vid.smart_range_detail = 1;
  vid.use_smart_range = 2;
  }
  
auto sbhook = addHook(hooks_args, 100, [] {
  using namespace arg;
           
  if(0) ;
  else if(argis("-sb")) {
    PHASEFROM(2);
    sokoban::run_sb();
    }
  else return 1;
  return 0;
  });


}

const flagtype VC = 1;
const flagtype NO_VC = 2;
const flagtype PLAYER = 4;

void sync(int mode, flagtype flags) {
  using namespace tour;
  if(mode == pmStart) {
    crystal::compass_probability = 0;
    crystal::crystal_period = 0;
    enable_canvas();
    mapeditor::drawplayer = (flags & PLAYER);
    vid.smart_range_detail = 1;
    vid.use_smart_range = 2;
    crystal::view_coordinates = (flags & VC);
    smooth_scrolling = true;
    }
  if(mode == pmKey && !(flags & NO_VC))
    crystal::view_coordinates = !crystal::view_coordinates;
  if(cwt.at != centerover && !playermoved && !(flags & PLAYER)) {
    cwt.at = centerover; 
    current_display->which_copy = gmatrix[cwt.at].T;
    }
  }

vector<tour::slide> high_slides;

int shapeid;

int mycanvas(cell *c) {

  int dim = crystal::get_dim();

  auto d = crystal::get_coord(c->master);
  for(int i=0; i<dim; i++) d[i] >>= 1;
  
  color_t col = 0;
  
  d[0] ++;
  
  int ones = 0;
  for(int i=0; i<dim; i++) if((d[i] & 1) == 1) ones++;
  
  switch(shapeid) {
  
    case 0: {
      auto dx = d; dx[0] -= 2;
      bool grid = false;
      for(int i=1; i<dim; i++) if((dx[i] & 3) == 2) grid = true;
  
      for(int i=0; i<3; i++) part(col, i) = 0xFF + 0x30 * (d[i]-2);
      if(grid) col |= 0x1000000;
      if(dx == crystal::c0) col = 0x1FFD500;
      if(dx[0] == 2 && dx[1] == 0 && dx[2] == 0 && dx[3] == 0 && dx[4] == 0 && dx[5] == 0) col = 0;
      if(dx[0] == 6 && dx[1] == 0 && dx[2] == 0 && dx[3] == 0 && dx[4] == 0 && dx[5] == 0) col = 0;
      return col;
      }
    
    case 1: {
      if(d[1] == 0 && d[2] == 0 && d[3] == 0 && d[4] == 0 && d[5] == 0) ;
      else col = gradient(0x1FF0000, 0x1FFFF00, 9, d[0], 16);
      part(col, 0) = 0x80 + d[1] * 0x70;
      return col;
      }
    
    case 2: {
      col = gradient(0xFFFF00, 0x00FF00, 17, d[0], 24);
      for(int i=0; i<1; i++) part(col, i) = 0xFF + 0x30 * (d[i+1]-2);
      c->landparam = col;
      if(ones == dim-1) col |= 0x1000000;
      return col;
      }
      
    case 3: {
      if(d[3] == 1) col = (ones & 1) ? 0x1C0FFC0 : 0x180FF80;
      if(d[3] == -1) col = (ones & 1) ? 0x18080FF : 0x14040FF;
      if(d[2] == 1) col = (ones & 1) ? 0x1FFCC00 : 0x1FF8080;
      if(d[2] == -1) col = (ones & 1) ? 0x180FFFF : 0x1408080;

      if(d[4] == 1) col = 0x1FFFFFF;
      if(d[4] == -1) col = 0x1FFFFFF;
      if(d[5] == 1) col = 0x1FFFFFF;
      if(d[5] == -1) col = 0x1FFFFFF;
      return col;
      }
    
    case 4: {
      if(d[3] == 1) col = (ones & 1) ? 0x1C0FFC0 : 0x180FF80;
      if(d[3] == -1) col = (ones & 1) ? 0x18080FF : 0x14040FF;

      if(d[4] == 1) col = 0x1FFFFFF;
      if(d[4] == -1) col = 0x1FFFFFF;
      return col;
      }
      
    case 5: {
      if(d[3] == 1) col = (ones & 1) ? 0x1C0FFC0 : 0x180FF80;
      if(d[3] == -2) col = (ones & 1) ? 0x180FFFF : 0x140FFFF;
      return col;
      }

    case 6: {
      if(d[3] == 1) col = (ones & 1) ? 0x1FF8080 : 0x1FF6060;
      if(d[2] == -1) col = (ones & 1) ? 0x1FFFF70 : 0x1E0E060;
      return col;
      }
    
    case 7: {
      if(d[1] == 0 || d[2] == 0) ;
      else if(d[1] > 0 && d[2] > 0) col = 0x1FF0000;
      else if(d[1] > 0 && d[2] < 0) col = 0x1FFFF00;
      else if(d[1] < 0 && d[2] < 0) col = 0x100FFFF;
      else if(d[1] < 0 && d[2] > 0) col = 0x10000FF;
      return col;
      }
    
    case 8: {
      int s = d[1] + d[2] + d[3] + d[4] + d[5];
      if(s > 0) col = 0x1FFD500;
      else if (s < -1) col = 0x17851a9;
      return col;
      }
    
    case 9: {
      int s = d[1] + d[2] + d[3] + d[4] + d[5] + d[0];
      if(s > 0) col = 0x1FF20FF;
      else if (s < -1) col = 0x1C0C0C0;
      return col;
      }
    
    case 10: /* house */ {
      d[0]--;
      int is0 = 0, is1 = 0, is2 = 0, ismore = 0;
      for(int a=0; a<dim; a++) {
        int v = abs(d[a]);
        if(v == 0) is0++;
        else if(v == 1) is1++;
        else if(v == 2) is2++;
        else if(v > 2) ismore++;
        }
      if(d[dim-1]) return 0x101010;
      else if(ismore) return 0x101010;
      else if(is2) return 0x1800000;
      else if(is1) return 0xFFFF00;
      else return 0x1FFFFFF;
      }

    case 11: /* orthoplex */ {
      int s = abs(d[0] - 4) + abs(d[1] - 3) + abs(d[2] - 2) + abs(d[3] - 3);
      if(s == 0) return 0x1FFFFFF;
      else if(s == 12) return 0x1FF0000;
      else if(s < 12) return 0x800000;
      // else if(s == 13) return 0x1FF8000;
      else return 0x202020;
      }
    
    default:
      return -1;
    }
  }

void enable() {
  rv_hook(patterns::hooks_generate_canvas, 100, mycanvas);
  }

auto explore_structure(int _shapeid) {
  using namespace tour;
  return [=] (presmode mode) {
    sync(mode, NO_VC);
    if(mode == pmStart) {
      tour::slide_backup(mapeditor::drawplayer, false);
      tour::slide_backup(smooth_scrolling, true);
      stop_game();
      set_geometry(geometry == gCrystal534 ? gCrystal534 : gCrystal344);
      enable_canvas();
      patterns::whichCanvas = ' ';
      shapeid = _shapeid;
      enable();
      crystal::crystal_period = 4;
      start_game();
      #if CAP_RAY
      ray::max_cells = 4096;
      #endif
      }
    if(mode == pmKey || mode == pmGeometrySpecial) {
      stop_game();
      set_geometry(geometry == gCrystal534 ? gCrystal344 : gCrystal534);
      enable();
      start_game();
      }
    };
  }

void add_explore_structure(vector<tour::slide>& v, int id, string nshort, string nlong) {
  string hds = "high-dimensional shapes/";
  v.emplace_back(
    tour::slide{hds+nshort, 999, tour::LEGAL::SPECIAL, 
      nlong + "\n\n"
      "In these slides, press 5 to switch between 4-dimensional and 6-dimensional space."
      "Press End/Home to move forward/backward.",
      explore_structure(id)
      });
  }

void house(int sides, int shape = 10) {
  stop_game();
  if(sides < 0)
    set_geometry(gCrystal344);
  else
    crystal::set_crystal(sides);
  set_variation(eVariation::pure);
  enable_canvas();
  patterns::whichCanvas = ' ';
  shapeid = shape;
  check_cgi();
  enable();
  start_game();
  }

#if CAP_RVSLIDES
tour::slide *gen_high_demo() {
  high_slides.clear();
  using namespace tour;
  auto& v = high_slides;
  v.emplace_back(
    slide{"Introduction/Three-dimensional space", 999, LEGAL::NONE, 
      "This is our 2D visualization of 3-dimensional space.\n\n"
      "In most slides you can press '5' to enable or disable the coordinate display. "
      "You can move the focus with numpad, arrowkeys, or clicking cells with mouse."
      ,
      [] (presmode mode) {        
        sync(mode, VC);
        if(mode == pmStart) {        
          crystal::set_crystal(6);
          patterns::whichCanvas = 'K';
          start_game();
          }
        }
      });

  v.emplace_back(
    slide{"Introduction/Four-dimensional space", 999, LEGAL::NONE, 
      "This is our 2D visualization of 4-dimensional space.",
      [] (presmode mode) {
        sync(mode, VC);
        if(mode == pmStart) {
          crystal::set_crystal(8);
          patterns::whichCanvas = 'K';
          start_game();
          }
        }
      });

  v.emplace_back(
    slide{"Introduction/Color picker", 999, LEGAL::NONE, 
      "Color picker. You can press '0' to '6' to adjust how fast the colors change.",
      [] (presmode mode) {
        sync(mode, 0);
        if(mode == pmStart)
          colorpicker::run_cpick();
        }
      });

  v.emplace_back(
    slide{"games and puzzles/house 3D", 999, LEGAL::NONE, 
      "A house in three dimensions. This is a 5x5 square, its center is white, rest of its interior is yellow, and its perimeter is red. "
      "By using the third dimension, you can leave the square, or enter it back.",
      [] (presmode mode) {
        sync(mode, 0);
        if(mode == pmStart) {
          house(6);
          }
        }
      });

  v.emplace_back(
    slide{"games and puzzles/house 4D", 999, LEGAL::NONE, 
      "A house in four dimensions. This is a 5x5x5 cube."
      ,
      [] (presmode mode) {
        sync(mode, 0);
        if(mode == pmStart) {
          house(8);
          }
        }
      });

  v.emplace_back(
    slide{"games and puzzles/house, 3D visualization", 999, LEGAL::NONE, 
      "A house in four dimensions, using the 3D version of our visualization. This visualization is harder to understand. Press End/Home to move forward/backward.",
      [] (presmode mode) {
        sync(mode, 0);
        if(mode == pmStart) {
          house(-1);
          }
        }
      });

  v.emplace_back(
    slide{"games and puzzles/4D orthoplex using 2D", 999, LEGAL::NONE, 
      "Try to find the center of the orthoplex in four dimensions. This is a 4D analog of the octahedron.\n\n"
      "The faces of the orthoplex are bright red, the outside is dark gray, and the center is white."
      ,
      [] (presmode mode) {
        sync(mode, 0);
        if(mode == pmStart) {
          house(8, 11);
          }
        }
      });

  v.emplace_back(
    slide{"games and puzzles/4D orthoplex using 3D", 999, LEGAL::NONE, 
      "The same visualization in 3D.",
      [] (presmode mode) {
        sync(mode, 0);
        if(mode == pmStart) {
          house(8, 11);
          }
        }
      });

  v.emplace_back(
    slide{"games and puzzles/basic roguelike", 999, LEGAL::NONE, 
      "This is a basic roguelike in three dimensions. Even though it appears that it should be possible to move in such a way that "
      "one of the enemies is no longer adjacent to us, it turns out that one of its other 'copies' will always manage "
      "to chase us. This is clear when we imagine the actual higher-dimensional space.",
      [] (presmode mode) {
        sync(mode, NO_VC | PLAYER);
        if(mode == pmStart) {
          crystal::set_crystal(6);
          patterns::whichCanvas = 'c';
          colortables['c'][0] = 0x208020;
          colortables['c'][1] = 0x105010;
          patterns::canvasback = 0x101010;
          start_game();
          auto & us = vid.cs;
          us.charid = 4;
          /* us.skincolor = 0xB55239FF;
          us.haircolor = 0xB55239FF;
          us.dresscolor = 0xB55239FF; */
          us.skincolor = 0x202020FF;
          us.haircolor = 0x202020FF;
          us.dresscolor = 0x202020FF;
          us.eyecolor = 0xC000FF;
          cwt.at->move(0)->monst = moRedFox;
          cwt.at->move(1)->monst = moLavaWolf;
          minf[moLavaWolf].color = 0x909090;
          minf[moLavaWolf].name = "Wolf";
          minf[moRedFox].name = "Fox";
          }
       }
      });

  v.emplace_back(
    slide{"games and puzzles/gravity mockup", 999, LEGAL::NONE, 
      "A mockup of a 4D game with gravity. We use the 3D version of our visualization, while the gravity dimension is shown using perspective.",
      [] (presmode mode) {
        sync(mode, 0);
        if(mode == pmStart) {
          sokoban::run_sb();
          }
       }
      });
    
  add_explore_structure(v, 0, "Cage", "In this series of slides we show various structures in four-dimensional space, using the three-dimensional variant of our visualization.\n\nWe start with a 4x4x4x4 cage with a golden point in the center.");
  add_explore_structure(v, 1, "Tunnel", "One-dimensional tunnel");
  add_explore_structure(v, 2, "Skeleton", "The 1-skeleton of the tessellation of Z^4 with cubes of edge 2");
  add_explore_structure(v, 3, "Tunnel2", "Two-dimensional tunnel");
  add_explore_structure(v, 4, "Hyperplanes", "Two hyperplanes in distance 2, i.e., three-dimensional tunnel.");
  add_explore_structure(v, 5, "Far hyperplanes", "Two hyperplanes in distance 3.");
  add_explore_structure(v, 6, "Orthogonal", "Two orthogonal hyperplanes.");
  add_explore_structure(v, 7, "Quarterspaces", "Four quarterspaces.");
  add_explore_structure(v, 8, "Diagonal", "Diagonal tunnel in all coordinates except one.");
  add_explore_structure(v, 9, "Diagonal", "Diagonal tunnel in all coordinates.");

  v.emplace_back(
    slide{"Magic cube/Standard magic cube", 999, LEGAL::NONE, 
      "Magic Cube (aka Rubik's Cube) using two dimensions. This is an example of a visualization which is difficult for our method, because we are moving complex objects in the 3D space."
      "Press 'r' to rotate the face (while the mouse pointer is on its center -- two coodinates must be 0 and one must be non-zero), Shift+R to reset.",
      [] (presmode mode) {
        sync(mode, 0);
        if(mode == pmStart) {
          magic::magic(6);
          }
        }
      });

  v.emplace_back(
    slide{"Magic cube/Four-dimensional magic cube", 999, LEGAL::NONE, 
      "Magic Cube (4D) using two dimensions. Keys are the same as in the previous slide.\n\n"
      "Use 'r' to rotate the 2D face under the mouse pointer (two coodinates must be 0 and two must be non-zero)."
      ,
      [] (presmode mode) {
        sync(mode, 0);
        if(mode == pmStart) {
          magic::magic(8);
          }
        }
      });

  v.emplace_back(
    slide{"Magic cube/Four-dimensional magic cube, 3D visualization", 999, LEGAL::NONE, 
      "Magic Cube (4D) using three dimensions.",
      [] (presmode mode) {
        sync(mode, 0);
        if(mode == pmStart) {
          magic::magic(-1);
          }
        }
      });

  v.emplace_back(
    slide{"3D Sokoban", 999, LEGAL::NONE,
      "A three-dimensional Sokoban puzzle visualized using H2.\n\n"
      "The puzzle is designed so that all three dimensions matter.\n\n"
      "Press 'r' or Backspace to undo moves.",
      [] (presmode mode) {
        sync(mode, 0);
        if(mode == pmStart) {
          crystal_sokoban::run_sb();
          }
        }
      });

  callhooks(pres::hooks_build_rvtour, "highdim", high_slides);
  pres::add_end(v);
  return &high_slides[0];
  }
#endif

auto highdim_hooks  = 
    addHook_slideshows(120, [] (tour::ss::slideshow_callback cb) {
  
    if(high_slides.empty()) gen_high_demo();

    cb(XLAT("visualizing higher-dimensional spaces"), &high_slides[0], 'h');
    });

}