#include "rogueviz.h"

/* a sample higher dimensional Sokoban puzzle */
#include "magiccube.cpp"

namespace rogueviz {

namespace crystal_sokoban {

vector<string> sokomap = {
  "########|########|########|########|########|########|########|########",
  "########|########|## #####|## #####|########|########|########|########",
  "########|##. ####|# $  ###|##.$####|##. ####|########|########|########",
  "########|##.#####|# $$@###|##  ####|## #####|########|########|########", 
  "########|########|## #####|## #####|########|########|########|########",
  "########|########|########|########|########|########|########|########",
  "########|########|########|########|########|########|########|########", 
  "########|########|########|########|########|########|########|########"
  };

bool on;

bool created;

vector<cell*> celllist;

struct undo_state {
  vector<eWall> board;
  cell* where;
  };

vector<undo_state> undos;

undo_state current_state() {
  undo_state u;
  u.where = cwt.at;
  for(cell *c: celllist) u.board.push_back(c->wall);
  return u;
  }

void run_sb() {
  crystal::compass_probability = 0;
  stop_game();
  crystal::set_crystal(6);
  set_variation(eVariation::pure);
  firstland = specialland = laCanvas;
  patterns::whichCanvas = 'g';
  patterns::canvasback = 0x101010;
  check_cgi();
  vizid = &on;
  start_game();  

  for(int z=-8; z<8; z++)
  for(int y=-8; y<8; y++)
  for(int x=-8; x<8; x++) {
    crystal::coord co = crystal::c0; co[0] = 2*x; co[1] = 2*y; co[2] = 2*z;
    cell *c = crystal::get_heptagon_at(co)->c7;
    setdist(c, 7, c);
    }
  
  for(int z=-8; z<8; z++)
  for(int y=-8; y<8; y++)
  for(int x=-8; x<8; x++) {
    char what;
    if(x<0 || y<0 || z<0)
      what = '#';
    else
      what = sokomap[y][x*9+z];
    
    crystal::coord co = crystal::c0; co[0] = 2*x; co[1] = 2*y; co[2] = 2*z;
    cell *c = crystal::get_heptagon_at(co)->c7;
    
    color_t col;
    for(int i=0; i<3; i++)
      part(col, i) = 0x80 + 0x20 * (co[i] - 5);
    
    c->landparam = col;
    
    if(what == '#')
      c->wall = waStone;
    else if(what == '$')
      c->wall = waCrateCrate;
    else if(what == '*')
      c->wall = waCrateOnTarget;
    else if(what == '.')
      c->wall = waCrateTarget;
    else {
      c->wall = waNone;
      if(what == '@') {
        cwt.at = c;
        centerover = c;
        }
      }
    celllist.push_back(c);
    }
  vid.smart_range_detail = ISWEB ? 20 : 1;
  vid.use_smart_range = 2;
  undos.push_back(current_state());
  peace::on = true;
  }

void save_undo() {
  undos.push_back(current_state());
  }

void restore_undo() {
  undos.pop_back();
  auto& u = undos.back();
  cwt.at = u.where;
  current_display->which_copy = Id;
  int i = 0;
  for(cell *c: celllist) c->wall = u.board[i++];

  undos.pop_back();
  }

bool sokomap2() {
  bool in = vizid == &on;
  if(!in) return false;

  if(undos.back().where != cwt.at) save_undo();

  if(1) {
    dynamicval<eGeometry> g(geometry, gEuclidSquare);
    check_cgi();
    cgi.require_shapes();
    initquickqueue();
    
    if(1) {
      auto gm = gmatrix;
      dynamicval<bool> ww(wmspatial, false);
      dynamicval<bool> wm(mmspatial, false);
      dynamicval<shiftmatrix> s1(playerV);
      dynamicval<transmatrix> s2(current_display->which_copy);
      dynamicval<shiftmatrix> s3(cwtV, cwtV);
      dynamicval<array<map<cell*, animation>, ANIMLAYERS>> an(animations);
      animations[LAYER_SMALL] = {};
      
      for(int x=0; x<4; x++)
      for(int y=0; y<4; y++)
      for(int z=0; z<4; z++) {
        crystal::coord co = crystal::c0; co[0] = 2*x+2; co[1] = 2*y+2; co[2] = 2*z+2;
        cell *c = crystal::get_heptagon_at(co)->c7;
        drawcell(c, shiftless(euscale(.12,.12) * eupush(3+(vid.xres*1./vid.yres-1)*12, -16) * eupush(z*4.2+x, y) * Id));
        }
    
      gmatrix = gm;
      }
  
    quickqueue();
    }
  
  check_cgi();
  
  return true;
  }

bool soko_key(int sym, int uni) {
  bool in = vizid == &on;
  if((cmode & sm::NORMAL) && (uni == SDLK_BACKSPACE || uni == 'r') && in && isize(undos) != 1) {
    restore_undo();
    return true;
    }
  return false;
  }


auto sbhook = addHook(hooks_args, 100, [] {
  using namespace arg;
           
  if(0) ;
  else if(argis("-crystal-sokoban")) {
    PHASEFROM(2);
    run_sb();
    }
  else return 1;
  return 0;
  }) + addHook(hooks_prestats, 100, sokomap2)
  + addHook(hooks_handleKey, 50, soko_key);


}

}