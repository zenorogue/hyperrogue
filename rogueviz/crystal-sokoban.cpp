#include "rogueviz.h"

/* a sample higher dimensional Sokoban puzzle */

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

bool is_solved() {
  for(cell *c: celllist) if(c->wall == waCrateCrate) return false;
  return true;
  }

void sb_hooks();
void soko_play();
bool already_solved;

void run_sb() {
  already_solved = false;
  showstartmenu = false;
  crystal::compass_probability = 0;
  stop_game();
  crystal::set_crystal(6);
  set_variation(eVariation::pure);
  enable_canvas();
  ccolor::set_plain(0x101010);
  check_cgi();
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

  sb_hooks();
  pushScreen(soko_play);
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

  if(undos.back().where != cwt.at) {
    save_undo();
    if(!already_solved && is_solved()) {
      already_solved = true;
      addMessage("Solved. Congratulations!");
      #if RVCOL
      rogueviz::rv_achievement("CRYSTALSOKOBAN");
      #endif
      }
    }

  if(1) {
    glflush();
    dynamicval<eGeometry> g(geometry, gEuclidSquare);
    check_cgi();
    if(!(cgi.state & 2)) {
      auto m = euc::new_map();
      dynamicval<hrmap*> dm(currentmap, m);
      cgi.require_shapes();
      ensure_floorshape_generated(0, currentmap->gamestart());
      ensure_floorshape_generated(1, currentmap->gamestart());
      }
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
    glflush();
    }
  
  check_cgi();
  
  return true;
  }

void soko_play() {
  getcstat = '-';
  cmode = sm::PANNING | sm::CENTER;
  gamescreen();
  // stillscreen = true;

  dialog::init();
  sokomap2();

  displayButton(vid.xres - vid.fsize, vid.yres - vid.fsize, "(v) quit", 'v', 16);
  dialog::add_key_action('v', [] { quitmainloop = true; });

  displayButton(vid.xres - vid.fsize, vid.yres - 3 * vid.fsize, "(u) undo", 'u', 16);
  dialog::add_key_action('u', [] { restore_undo(); });

  displayButton(vid.xres - vid.fsize, vid.yres - 5 * vid.fsize, "(s) settings", 's', 16);
  dialog::add_key_action('s', [] { pushScreen(showSettings); });

  displayButton(vid.xres - vid.fsize, vid.yres - 7 * vid.fsize, "(h) help", 'h', 16);
  dialog::add_key_action('h', [] {
    gotoHelp(
      "This is a three-dimensional Sokoban puzzle, visualized using a hyperbolic plane.\n\n"
      "Push all crates (red circles) onto targets (places with green borders).\n\n"
      "The puzzle is designed so that all three dimensions matter.\n\n"
      );
    });

  keyhandler = [] (int sym, int uni) {
    handlePanning(sym, uni);
    handle_movement(sym, uni);
    if(sym == '-' || sym == PSEUDOKEY_WHEELDOWN) {
      actonrelease = false;
      mousemovement();
      }
    dialog::handleNavigation(sym, uni);
    };
  }


bool soko_key(int sym, int uni) {
  if((cmode & sm::NORMAL) && (uni == SDLK_BACKSPACE || uni == 'r') && isize(undos) != 1) {
    restore_undo();
    return true;
    }
  return false;
  }

void sb_hooks() {
  rv_hook(hooks_prestats, 90, sokomap2);
  rv_hook(hooks_welcome_message, 50, [] () {
    addMessage(XLAT("Welcome to Crystal Sokoban!"));
    return true;
    });
  rv_hook(hooks_handleKey, 50, soko_key);
  on_cleanup_or_next([] {
    undos.clear();
    celllist.clear();
    });
  }

auto sbhook = arg::add2("-crystal-sokoban", run_sb);


}

}