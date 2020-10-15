// non-Euclidean falling block game, implemented using the HyperRogue engine
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

#ifdef BRINGRIS

#define CUSTOM_CAPTION "Bringris"

#define MAXMDIM 4

#define CAP_INV 0
#define CAP_COMPLEX2 0
#define CAP_EDIT 0
#define CAP_BT 0
#define CAP_SOLV 0
#define CAP_THREAD 0
// #define CAP_ZLIB 0
#define CAP_FILES 0
// #define CAP_ANIMATIONS 0

#define CAP_RUG 0
#define CAP_SHOT 0
#define CAP_SVG 0
#define CAP_PNG 0
#define CAP_TOUR 0
#define CAP_IRR 0
#define CAP_CRYSTAL 0
#define CAP_ARCM 0
#define CAP_HISTORY 0
#define CAP_STARTANIM 0
#define CAP_CONFIG 0
#define CAP_SAVE 0
#define CAP_TRANS 0

#ifdef BWEB
#include "../hyperweb.cpp"
#else
#include "../hyper.cpp"
#endif
#endif

#include "../hyper.h"

namespace hr {

namespace bringris {

using code_t = vector<pair<int, int>>;

struct piecedata {
  code_t code;
  int multi;
  int penalty;
  int symmetries;
  int count;
  };

vector<piecedata> piecelist;

bool listed(const vector<cellwalker>& v, cell* c) {
  for(auto cw: v) if(cw.at == c) return true;
  return false;
  }

bool same(const vector<cellwalker>& shape, const vector<cellwalker>& shape2) {
  set<cell*> allcells;
  if(isize(shape) != isize(shape2)) return false;
  for(auto sh: shape) allcells.insert(sh.at);
  for(auto sh: shape2) allcells.erase(sh.at);
  return allcells.empty();
  }

cellwalker flatspin(cellwalker cw, int i) {
  cw.spin = gmod(cw.spin + i, cw.at->type - (hybri ? 2 : 0));
  return cw;
  }

cellwalker add(cellwalker orig, int i, int sym = 0) {
  if(prod) {
    if(i >= orig.at->type-2) {
      if(sym&2) i ^= (orig.at->type-1) ^ (orig.at->type-2);
      orig.at = orig.at->cmove(i);
      return orig;
      }
    else {
      if(sym&1) i = -i;
      return flatspin(orig, i) + wstep;
      }
    }
  return orig + i + wstep;
  }

vector<cellwalker> build_from(const code_t& code, cellwalker start, int sym = 0) {
  vector<cellwalker> all = {start};
  for(auto c: code)
    all.push_back(add(all[c.first], c.second, sym));
  return all;
  }

int penalty(const vector<cellwalker>& shape, const code_t& code) {
  int p = 0;
  if(prod) {
    int bad = shape[0].at->type-1;
    for(auto co: code) if(co.second == bad)
      p += 1000;
    }
  map<cell*, int> dists;
  for(auto cw: shape) dists[cw.at] = 99;
  dists[shape[0].at] = 0;
  for(int i=0; i<isize(shape); i++)
    for(auto c1: shape)
      for(auto c2: shape)
        if(neighborId(c1.at, c2.at) != -1)
          dists[c2.at] = min(dists[c2.at], dists[c1.at] + 1);
  for(auto d: dists) p += d.second * 10;
  for(auto c: code) if(c.second == 0 || c.second == 2) p++;
  return p;
  }

bool builds(const vector<cellwalker>& shape, const code_t& code, int sym = 0) {
  for(auto sh: shape) for(int i=0; i<sh.at->type; i++) {
    vector<cellwalker> shape2 = build_from(code, cellwalker(sh.at, i), sym);
    if(same(shape, shape2)) 
      return true;
    }
  return false;
  }

void generate_shapes_rec(vector<cellwalker>& sofar, code_t& code, int cnt) {
  if(isize(sofar) == cnt) {
    int p = penalty(sofar, code);
    for(auto& pc: piecelist) 
      if(builds(sofar, pc.code)) {
        if(p < pc.penalty) pc.penalty = p, pc.code = code;
        pc.multi++;
        return;
        }
    int syms = 0;
    for(int i: {0,1,2,3}) if(builds(sofar, code, i)) syms++;
    bool vertical = true;
    for(auto c: code) if(c.second < 4) vertical = false;
    if(vertical) syms *= 2;
    if(isize(code) == 0) syms = 1;
    piecelist.push_back(piecedata{code, 1, p, syms, 0});
    return;
    }
  for(int i=0; i<isize(sofar); i++)
  for(int t=0; t<sofar[i].at->type; t++) {
    cellwalker ncw = add(sofar[i], t);
    if(listed(sofar, ncw.at)) continue;
    code.emplace_back(i, t);
    sofar.push_back(ncw);
    generate_shapes_rec(sofar, code, cnt);
    sofar.pop_back();
    code.pop_back();
    }
  }

void generate_shapes(int cnt) {
  vector<cellwalker> cws = { cwt };
  code_t co = {};
  generate_shapes_rec(cws, co, cnt);
  }

void list_all() {
  println(hlog, "number of shapes = ", isize(piecelist));
  // for(auto sh: piecelist) println(hlog, "multi=", sh.multi, " penalty=", sh.penalty, " syms=", sh.symmetries, " => ", sh.code);
  }

vector<cell*> level;

bool pro_game;

int well_size = 10;
int camera = 3;

int facing_mod = 0;

int shape_id, next_shape_id;

cellwalker at;

int move_started;
int move_at;

int completed;
int bricks, cubes;

ld score;

enum eState {
  tsPreGame, tsFalling, tsBetween, tsCollect, tsGameover
  };

bool paused;
bool explore;

eState state = tsPreGame;

void water_shape() {
  auto shape = build_from(piecelist[shape_id].code, at);
  for(auto c: shape)
    c.at->wall = waSea; 
  }

set<cell*> to_disappear;

color_t hipso[] = {
  0x3030C0, 
  0x6060FF,
  0x9090FF,
  0xFFFFFF,
  0x008000,
  0x40FF00,
  0xFFFF00,
  0xFF8000,
  0xFF0000,
  0x800000,
  0x400040,
  0x500050,
  0x600060
  };

color_t get_hipso(ld y) {
  y += 12;
  return hipso[gmod(y, 13)];
  }

void draw_shape() {
  auto shape = build_from(piecelist[shape_id].code, at);
  for(auto c: shape) {
    int y = -hybrid::get_where(c.at).second;
    c.at->wall = waWaxWall, c.at->landparam = get_hipso(y);
    }
  }

void remove_shape() {
  auto shape = build_from(piecelist[shape_id].code, at);
  for(auto c: shape)
    c.at->wall = waNone;
  }

bool shape_conflict(cellwalker cw) {
  auto shape = build_from(piecelist[shape_id].code, cw);
  for(auto c: shape)
    if(c.at->wall)
      return true;
  return false;
  }

ld current_move_time_limit() {
  return 50000 * pow(.9, completed) + 10000. / (1 + completed);
  }

int turn_animation = 500;
int collect_animation = 300;

int choose_piece() {
  ld total = 0;
  int sel = 0;
  ld lowest = 100000;
  for(auto &p: piecelist) 
    lowest = min(lowest, p.count * 1. / p.symmetries);
    
  vector<tuple<int,int,ld>> probs;
    
  for(int i=0; i<isize(piecelist); i++) {
    auto& p = piecelist[i];
    double cnt = p.symmetries * exp(log(.5) * (p.count * 1. / p.symmetries - lowest));
    probs.emplace_back(p.symmetries, p.count, cnt);
    total += cnt;
    if(randd() < cnt / total) 
      sel = i;
    }
  
  // println(hlog, "probs = ", probs);
  piecelist[sel].count++;
  return sel;
  }

void new_piece() {
  at.at = hybrid::get_at(hybrid::get_where(at.at).first, -well_size - 1);  
  shape_id = next_shape_id;
  next_shape_id = choose_piece();
  if(shape_conflict(at)) {
    playSound(cwt.at, "die-bomberbird");
    state = tsGameover;
    }
  else {
    draw_shape();
    state = tsFalling;
    move_started = ticks;
    move_at = ticks + current_move_time_limit();
    }    
  }

vector<int> by_level;

bool expert = true;

void find_lines() {
  by_level.clear();
  // println(hlog, "Removing levels");
  
  for(int z=1; z<=well_size; z++) {
    int ct = 0;
    for(auto lev: level) {
      cell *c = hybrid::get_at(lev, -z);
      ct += (c->wall ? 1 : 0);
      }
    by_level.push_back(ct);
    }
  // println(hlog, by_level, " / ", isize(level));
  
  int points = 0;
  
  if(expert) {
    for(int z=1; z<=well_size; z++) if(by_level[z-1] >= isize(level)) {
      points++;
      for(auto lev: level) {
        cell *c = hybrid::get_at(lev, -z);
        to_disappear.insert(c);
        }
      }
    }
  else {
    int lines_found = 0;
    for(int z=1; z<=well_size; z++) {
      for(auto lev: level) for(int d=0; d<lev->type; d++) {
        cellwalker cw(hybrid::get_at(lev, -z), d);
        cellwalker cw0 = cw;
        bool filled = true;
        do {
          if(!cw.at->wall) filled = false;
          cw += wstep;
          cw = flatspin(cw, lev->type/2);
          }
        while(cw != cw0);
        if(filled) {
          lines_found = true;
          do {
            to_disappear.insert(cw.at);
            cw += wstep;
            cw = flatspin(cw, lev->type/2);
            }
          while(cw != cw0);
          }
        }
      }
    // println(hlog, "lines found = ", lines_found);
    }
  if(!to_disappear.empty()) {
    move_at = ticks + collect_animation;
    state = tsCollect;
    score += 10000000. * points * (points+1.) / current_move_time_limit();
    completed += points;
    playSound(cwt.at, points == 1 ? "pickup-gold" : "orb-mind");
    }
  }

void disappear_lines() {
  for(auto lev: level) {
    int nz = 1;
    for(int z=1; z<=well_size; z++) {
      cell *c1 = hybrid::get_at(lev, -z);
      if(!to_disappear.count(c1)) {
        cell *c0 = hybrid::get_at(lev, -nz);
        c0->wall = c1->wall;
        c0->landparam = get_hipso(nz);
        nz++;
        }
      }
    while(nz < well_size + camera_level) {
      cell *c0 = hybrid::get_at(lev, -nz);
      c0->wall = waNone;
      nz++;
      }
    }
  to_disappear.clear();
  state = tsBetween;
  }

void state_loop() {
  find_lines();
  if(to_disappear.empty()) new_piece();
  }
    
void fallen() {
  draw_shape();
  bricks++;
  cubes += isize(piecelist[shape_id].code)+1;
  state = tsBetween;
  playSound(cwt.at, "closegate");
  score += 20000000. / (current_move_time_limit() * 3 + move_started - ticks);
  }

void drop() {
  remove_shape();
  cellwalker fall = at;
  fall.at = fall.at->cmove(fall.at->type-1);
  if(shape_conflict(fall))
    fallen();
  else {
    at = fall;
    draw_shape();
    }
  move_at = ticks + current_move_time_limit();
  }

void fulldrop() {
  remove_shape();
  cellwalker fall = at;
  int no = 0;
  while(true) {
    fall.at = fall.at->cmove(fall.at->type-1);
    if(shape_conflict(fall)) break;
    no++;
    }
  playSound(cwt.at, "hit-crush2");
  // println(hlog, "dropped by ", no);
  fall.at = fall.at->cmove(fall.at->type-2);
  at = fall;
  draw_shape();
  if(!no) fallen();
  }

int last_adjust, when_t;
transmatrix tView, pView;
cell* ncenter;

void verify_matrix(transmatrix T) {
  vector<ld> ver;
  for(int i=0; i<3; i++)
  for(int j=0; j<3; j++) {
    ld a = T[0][i] * T[0][j] + T[1][i]*T[1][j] - T[2][i] * T[2][j];
    ver.push_back(kz(a));
    }
  println(hlog, ver);
  }

void set_tview(transmatrix T) {

  View = T * tView;

  transmatrix rel = pView * inverse(View);
  fixmatrix(rel);

  centerover = ncenter;
  optimizeview();
  ncenter = centerover;
  tView = View;
  tView = spin(135*degree - 90*degree*-(at.spin+facing_mod));

  pView = rel * tView;
  when_t = ticks + turn_animation;
  }

void rotate_block(int d) {
  remove_shape();
  cellwalker at1 = flatspin(at, d);
  if(!shape_conflict(at1)) {
    at = at1;
    facing_mod -= (currentmap->gamestart()->type & 1 ? 2 : 1)*d;
    // set_tview(spin(d*72*degree));
    }
  draw_shape();
  }    

void shift_block(int k) {
  int t = currentmap->gamestart()->type;
  if(prod) t -= 2;
  if(t&1) {
    k = gmod(k+facing_mod, 10);
    int dir = k / 2;
    int kspin = (k&1?3:2) - dir;
    remove_shape();
    cellwalker at1 = flatspin(flatspin(at, dir) + wstep, kspin);
    ld angle = (dir*2-facing_mod) * 36 * degree;
    
    if(!shape_conflict(at1)) {
      at = at1;
      set_tview(spin(-angle) * ypush(-cgi.tessf) * spin(angle));
      facing_mod += (k&1?-1:1);
      }
    draw_shape();
    }
  else {
    k = gmod(k+facing_mod, t);
    int dir = k;
    int kspin = (t/2) - dir;
    remove_shape();
    
    cellwalker at1 = flatspin(at, dir);
    ld dist = hdist0(currentmap->adj(at1.at, at1.spin) * C0);
    
    at1 = flatspin(at1 + wstep, kspin);
    ld angle = (dir-facing_mod) * 90 * degree;
    
    if(!shape_conflict(at1)) {
      // playSound(cwt.at, "hit-crush1");
      at = at1;
      set_tview(spin(-angle) * ypush(-dist) * spin(angle));
      }
    else playSound(cwt.at, "hit-crush3");
    draw_shape();
    }
  }

int camera_level;

ld ang = 0, cur_ang = 0;

int lti;

void create_matrices() {
  dq::clear_all();
  make_actual_view();
  dq::enqueue_by_matrix_c(centerover, cview());
  current_display->all_drawn_copies.clear();
  int id = 0;
  while(!dq::drawqueue_c.empty()) {
    auto& p = dq::drawqueue_c.front();
    cell*& c = p.first;
    shiftmatrix& V = p.second;
    current_display->all_drawn_copies[c].push_back(V);
    gmatrix[p.first] = p.second;
    if(id < (cur_ang ? 100 : 500)) {
      for(int i=0; i<c->type-2; i++) {
        cell *c1 = c->cmove(i);
        dq::enqueue_by_matrix_c(c1, optimized_shift(V * currentmap->adj(c, i)));
        }
      }
    dq::drawqueue_c.pop();
    id++;
    }  
  }

transmatrix smooth;

void draw_wirecube_at(const transmatrix& smoo, cell *c, int zlev, color_t col) {

  auto where_c = hybrid::get_where(c);
  
  auto c_camera = hybrid::get_at(where_c.first, -camera_level);
  
  for(const shiftmatrix& V: current_display->all_drawn_copies[c_camera]) {
    shiftmatrix VA = shiftless(smoo * V.T, cgi.plevel * (where_c.second - zlev));
    for(int i=0; i<c->type; i++)
      queuepolyat(VA, cgi.shWireframe3D[i], 0, PPR::SUPERLINE).outline = col;
    }
  }

void draw_piece(int zlev, int id) {
  sightranges[geometry] *= 100;
  initquickqueue();
  auto shape = build_from(piecelist[id].code, at);

  auto where_at = hybrid::get_where(at.at);
  
  vid.linewidth *= 3;

  for(auto c: shape) {    
    auto where_c = hybrid::get_where(c.at);      
    color_t levels[4] = {color_t(0xFFFFFFFFF), color_t(0xFFFF00FF), color_t(0xFF8000FF), color_t(0xFF0000FF)};      
    draw_wirecube_at(smooth, c.at, zlev, levels[where_at.second-where_c.second]);
    }

  vid.linewidth /= 3;

  quickqueue();
  glflush();
  sightranges[geometry] /= 100;
  }

void draw_holes(int zlev) {
  sightranges[geometry] *= 100;
  initquickqueue();
  if(state == tsFalling) remove_shape();
  for(auto lev: level) {
    bool covered = false;
    for(int z=well_size; z>=1; z--) {
      cell *c1 = hybrid::get_at(lev, -z);
      if(c1->wall) covered = true;
      else if(covered) {
        vid.linewidth *= 4;
        draw_wirecube_at(Id, c1, zlev, (get_hipso(z) << 8) | 0xFF);
        vid.linewidth /= 4;
        }
      }
    }
  if(state == tsFalling) draw_shape();
  quickqueue();
  sightranges[geometry] /= 100;
  }

void start_new_game();

void draw_screen(int xstart, bool show_next) {
  int steps = camera_level - (-hybrid::get_where(at.at).second);
  if(state != tsFalling) steps = camera_level - (well_size + 1);
  
  dynamicval<display_data> ccd(*current_display);
  current_display->xmax = xstart * 1. / vid.xres;
  
  if(explore) {
    gamescreen(0);
    mouseaim_sensitivity = 0.01;
    camera_speed = 2;
    smooth_scrolling = true;
    }
  else {  
    mouseaim_sensitivity = 0;
    NLP = Id;
    View = pView;
    ld lv = -cgi.plevel * steps;
    shift_view(ztangent(lv));
    rotate_view(cspin(1, 2, cur_ang));
    shift_view(ztangent(cgi.plevel * 6));
    centerover = ncenter;
    int zlev = hybrid::get_where(centerover).second;  
    make_actual_view();
    create_matrices();
    // anims::moved();
    
    auto adc = std::move(current_display->all_drawn_copies);
    
    if(state == tsCollect) for(cell *c: to_disappear) c->landparam = rand() & 0xFFFFFF;
    if(state == tsFalling && !explore && !cur_ang) remove_shape();
    gamescreen(0);
    if(state == tsFalling && !explore && !cur_ang) draw_shape();

    current_display->all_drawn_copies = std::move(adc);
        
    if(anyshiftclick) draw_holes(zlev);
    
    if(state == tsFalling && !explore && !cur_ang) draw_piece(zlev, shape_id);

    if(show_next) {
      dynamicval<display_data> ccd(*current_display);
      current_display->xmin = (xstart + vid.fsize) * 1. / vid.xres;
      current_display->xmax = (vid.xres - vid.fsize) * 1. / vid.xres;
      current_display->ymin = (vid.fsize * 18) * 1. / vid.yres;
      current_display->ymax = (vid.fsize * (18+8)) * 1. / vid.yres;
      calcparam();
      draw_piece(zlev, next_shape_id);
      }      
  
    if(state == tsBetween) state_loop();
    
    if(state == tsCollect && ticks >= move_at) 
      disappear_lines();
    
    if(ticks >= move_at && state == tsFalling && pro_game) {
      drop();
      }

    View = pView;
    centerover = ncenter;      
    NLP = Id;
    }
  }

void run() {

  clearMessages();

  if(ang < cur_ang) {
    cur_ang -= (ticks - lti) / 1000.;
    if(cur_ang < ang) cur_ang = ang;
    }
  
  if(ang > cur_ang) {
    cur_ang += (ticks - lti) / 1000.;
    if(cur_ang > ang) cur_ang = ang;
    }
  
  lti = ticks;
  
  if(explore) ;
  else if(ticks > when_t) {
    pView = tView; 
    View = pView;
    smooth = Id;
    }
  else {
    ld part = (ticks - last_adjust) * 1. / (when_t - last_adjust);
    transmatrix T = pView * inverse(tView);
    hyperpoint vec = inverse_exp(shiftless(tC0(T)));
    transmatrix Tspin = gpushxto0(tC0(T)) * T;
    ld alpha = atan2(Tspin*xpush0(1));
    pView = spin(alpha * part) * gpushxto0(direct_exp(vec*part)) * pView;
    fixmatrix(pView);
    smooth = tView * inverse(pView);
    }
  last_adjust = ticks;
  
  ray::want_use = 2;
  sightranges[geometry] = 50;
  vid.cells_drawn_limit = 1;

  cmode = sm::NORMAL | sm::CENTER;

  int xstart = vid.xres - vid.fsize * 10;
  
  getcstat = '-';
  
  bool show_next = state != tsGameover && state != tsPreGame && !paused;

  draw_screen(xstart, show_next);
  
  calcparam();
  for(int i=0; i<isize(by_level); i++) {
    displaystr(xstart + vid.fsize, vid.yres - vid.fsize * (i+2), 0, vid.fsize, its(by_level[i]), get_hipso(i+1), 0);
    }
  displaystr(xstart + vid.fsize, vid.yres - vid.fsize * 1, 0, vid.fsize, its(isize(level)), winf[waBarrier].color, 0);

  if(state != tsPreGame) {
    displaystr(xstart + vid.fsize, vid.yres - vid.fsize * 16, 0, vid.fsize, "LEVELS " + its(completed), winf[waBarrier].color, 0);
    displaystr(xstart + vid.fsize, vid.yres - vid.fsize * 15, 0, vid.fsize, "BRICKS " + its(bricks), winf[waBarrier].color, 0);
    displaystr(xstart + vid.fsize, vid.yres - vid.fsize * 14, 0, vid.fsize, "CUBES " + its(cubes), winf[waBarrier].color, 0);
    if(pro_game)
      displaystr(xstart + vid.fsize, vid.yres - vid.fsize * 13, 0, vid.fsize, "SCORE " + fts(int(score)), winf[waBarrier].color, 0);
    }
  
  if(show_next)
    displaystr(xstart + vid.fsize, vid.yres - vid.fsize * 27, 0, vid.fsize, "NEXT:", winf[waBarrier].color, 0);
      
  bool in_menu = !show_next && !explore;
  
  if(explore) {
    int xx = (xstart + vid.xres) / 2;
    if(displayButtonS(xx, vid.fsize * 2, "backspace", 0xFFFFFFFF, 8, vid.fsize)) getcstat = SDLK_BACKSPACE;
    if(displayButtonS(xx, vid.fsize * 3, "to stop", 0xFFFFFFFF, 8, vid.fsize)) getcstat = SDLK_BACKSPACE;
    if(displayButtonS(xx, vid.fsize * 4, "exploring", 0xFFFFFFFF, 8, vid.fsize)) getcstat = SDLK_BACKSPACE;
    }
  
  if(in_menu) {
    int xx = (xstart + vid.xres) / 2;
    if(paused) {
      if(displayButtonS(xx, vid.fsize * 2, "CONTINUE", 0xFFFFFFFF, 8, vid.fsize)) getcstat = 'p';
      }
    else if(state == tsGameover) {
      displayButtonS(xx, vid.fsize * 2, "game over", 0xFFFFFFFF, 8, vid.fsize);
      }
    if(displayButtonS(xx, vid.fsize * 4, "NEW GAME", 0xFFFFFFFF, 8, vid.fsize)) getcstat = 'n';
    if(displayButtonS(xx, vid.fsize * 6, "EXPERT GAME", 0xFFFFFFFF, 8, vid.fsize)) getcstat = 'x';
    if(state != tsPreGame)
      if(displayButtonS(xx, vid.fsize * 8, "EXPLORE", 0xFFFFFFFF, 8, vid.fsize)) getcstat = 'e';
    if(!ISWEB) {
      if(displayButtonS(xx, vid.fsize * 10, "QUIT", 0xFFFFFFFF, 8, vid.fsize)) getcstat = 'q';
      }
    else if(state == tsGameover)
      if(displayButtonS(xx, vid.fsize * 10, "TWEET", 0xFFFFFFFF, 8, vid.fsize)) getcstat = 't';
    }
  
  keyhandler = [xstart, in_menu] (int sym, int uni) {
    if(explore) handlePanning(sym, uni);
    if(explore && sym == SDLK_BACKSPACE) 
      explore = false;
    if(sym != 13)
      dialog::handleNavigation(sym, uni);
    if(in_menu && sym == 'q' && !ISWEB) exit(0);
    if(sym == '-') {
      int ax = mousex * 3 / xstart;
      if(ax > 3) ax = 3;
      int ay = mousey * 3 / vid.yres;
      if(ay > 2) ay = 2;
      sym = uni = "-w-\ra d\r-s-\r" [ax+4*ay];
      }
    if(state == tsFalling && !paused) {
      int t = currentmap->gamestart()->type;
      if(t & 1) {
        if(sym == 'd') rotate_block(1);
        if(sym == 's') rotate_block(-1);
        for(int k=0; k<10; k++)
          if(sym == "cxzaqwerfv"[k])
            shift_block(k);
        }
      else {
        /*
        if(sym == 'q') rotate_block(1);
        if(sym == 'e') rotate_block(-1);
        */
        for(int k=0; k<4; k++)
          if(sym == "sawd"[k])
            shift_block(k);
        }
      if(sym == ' ') drop();
      if(sym == 13) fulldrop();
      }
    // if(sym == 'k') ang = 0;
    // if(sym == 'l') ang = 45 * degree;    
    if(sym == 'p') {
      if(!paused) move_at = move_at - ticks;
      paused = !paused;
      if(!paused) move_at = move_at - ticks;
      explore = false;
      }
    if(sym == 't' && state == tsGameover) {
      const vector<const char*> emoji =
        {"😀","😎","👽","🤖","😺","🎩","🎓","👑","💍","🐯","🦁","🐮","🐷","🐽","🐸","🐙","🐵","🐦","🐧","🐔","🐒","🙉","🙈","🐣","🐥","🐺","🐗","🐴","🦄","🐝","🐛","🐢","🦀","🦂","🕷","🐜","🐞","🐌","🐠","🐟","🐡","🐬","🐋","🐊","🐆","🐘","🐫","🐪","🐄","🐂","🐃","🐏","🐑","🐀","🐁","🐓","🦃","🐉","🐾","🐿","🐇","🐈","🐩","🐕","🐲","🌵","🍁","🌻","🌎","⭐️","⚡️","🔥","❄️","☔️","☂️","💧","🍏","🍎","🍐","🍋","🍌","🍉","🍇","🌶","🍅","🍍","🍑","🍈","🍓","🌽","🍠","🍯","🍞","🍗","🧀","🍖","🍤","🌯","🌮","🍝","🍕","🌭","🍟","🍔","⚽️","🎱","🏆","🎪","🎲","🎳","🚗","🚕","🚙","🏎","⛺️","⛩","🕹","💾","☎️","⏱","🔦","💡","💰","💎","🔨","💣","🔑","❤️","🔔"};

      string out;
      if(pro_game) {
        out = "Got " + its(score) + " points for completing " + its(completed) + " levels in #Bringris!";
        }
      else if(completed) {
        out = "Used " + its(bricks) + " blocks to complete " + its(completed) + " levels in #Bringris!";
        }
      else {
        out = "Dropped " + its(bricks) + " blocks in #Bringris!";
        }
      unsigned hash = time(NULL) / 600;
      for(char c: out) hash = 171 * hash + c;
      std::mt19937 invr;
      invr.seed(hash);
      out += " ";
      for(int i=0; i<4; i++)
        out += emoji[invr() % isize(emoji)];            
      // println(hlog, out);
      #if ISWEB
      EM_ASM({
        var tweetbegin = 'https://twitter.com/intent/tweet?text=';
        var tweettxt = UTF8ToString($0, $1);
        var finaltweet = tweetbegin +encodeURIComponent(tweettxt) + "&url=https://zenorogue.itch.io/bringris";
        window.open(finaltweet,'_blank');
        }, out.c_str(), isize(out));
      #endif
      }
    if(in_menu && sym == 'e') {
      explore = !explore;
      }
    if(in_menu && sym == 'n') {
      start_new_game();
      paused = false;
      explore = false;
      pro_game = false;
      playSound(cwt.at, "elementalgem");
      }
    if(in_menu && sym == 'x') {
      start_new_game();
      paused = false;
      explore = false;
      pro_game = true;
      playSound(cwt.at, "elementalgem");
      }
    #if CAP_SHOT
    if(sym == ']') {
      static int id = 0;
      perfect_linewidth = 0;
      shot::shot_aa = 2;
      vid.linewidth *= 2;
      shot::take(format("bringris-%04d.png", id++), [] { draw_screen(vid.xres, false); });
      vid.linewidth /= 2;
      }
    #endif
    };
  }

void start_new_game() {
  
  for(auto& p: piecelist) p.count = 0;

  for(auto lev: level) for(int z=0; z<=camera_level+1; z++) {
    cell *c = hybrid::get_at(lev, -z);
    setdist(c, 7, nullptr);
    c->item = itNone;
    c->land = laCanvas;
    if(z == 0)
      c->wall = waBarrier, c->land = laBarrier;
    else if(z <= camera_level)
      c->wall = waNone;
    else 
      c->wall = waWaxWall, c->land = laCanvas, c->landparam = 0xC000C0;
    }
  
  centerover = hybrid::get_at(level[0], -camera_level);
  cwt.at = centerover;
  ncenter = hybrid::get_at(level[0], -camera_level);
  
  at = hybrid::get_at(level[0], -well_size - 1);
  next_shape_id = choose_piece();
  
  state = tsBetween;

  NLP = Id;
  tView = pView = in_h2xe() ? spin(135*degree) : spin(90*degree);

  completed = 0;
  bricks = 0;
  cubes = 0;
  score = 0;
  }
  
void create(int i) {
  if(!prod) {
    println(hlog, "need product geometry");
    exit(1);
    }
  level = PIU(currentmap->allcells());
  generate_shapes(1);
  generate_shapes(2);
  generate_shapes(3);
  generate_shapes(4);
  list_all();
  // println(hlog, "level size = ", isize(level));
  
  camera_level = well_size + i + camera;
  
  playermoved = false;
  ray::want_use = 2;
  ray::exp_decay_poly = 200;
  mapeditor::drawplayer = false;
  // sightranges[geometry] = 1;
  
  
  pushScreen(run);
  
  vid.fov = 90;
  vid.plevel_factor = 0.5;
  // vid.grid = true;

  mouseaim_sensitivity = 0;  
  
  showstartmenu = false;
  
  start_new_game();
  state = tsPreGame;
  }

int args() {
  using namespace arg;
           
  if(0) ;

  else if(argis("-list")) {
    PHASEFROM(3);
    start_game();
    shift(); int i = argi();
    generate_shapes(i);
    list_all();
    }

  else if(argis("-bringris")) {
    PHASEFROM(3);
    start_game();
    shift(); int i = argi();
    create(i);
    }

  else return 1;
  return 0;
  }

auto hooks = 
    addHook(hooks_args, 100, args);

#ifdef BRINGRIS
auto hook1=
    addHook(hooks_config, 100, [] {
      if(arg::curphase == 2) {
        stop_game_and_switch_mode(rg::nothing);
        using namespace fieldpattern;
        current_extra = 2;
        auto& gxcur = fgeomextras[current_extra];
        while(isize(gxcur.primes) < 1) nextPrime(gxcur);
        fgeomextras[current_extra].current_prime_id = 0;
        enableFieldChange();
        set_geometry(gFieldQuotient);
        
        gp::param = gp::loc(1, 1);
        set_variation(eVariation::unrectified);

        set_geometry(gProduct);
        start_game();
        create(4);
        vid.texture_step = 8;
        }
      });
#endif

}
}
