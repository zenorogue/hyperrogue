// non-Euclidean falling block game, implemented using the HyperRogue engine
// Copyright (C) 2011-2021 Zeno Rogue, see 'hyper.cpp' for details

#define BRINGRIS_VER "2.0"

#ifdef BRINGRIS

#define CUSTOM_CAPTION "Bringris " BRINGRIS_VER

#define MAXMDIM 4

#define CAP_INV 0
#define CAP_COMPLEX2 0
#define CAP_EDIT 0

#ifdef BWEB
#define CAP_BT 0
#define CAP_SOLV 0
#endif

#define CAP_THREAD 0
// #define CAP_ZLIB 0
#define CAP_FILES 1
#define CAP_CONFIG 1
// #define CAP_ANIMATIONS 0

#define CAP_RUG 0
#define CAP_SHOT 0
#define CAP_SVG 0
#define CAP_PNG 0
#define CAP_WRL 0
#define CAP_TOUR 0
#define CAP_IRR 0
#define CAP_CRYSTAL 0
#define CAP_ARCM 0
#define CAP_HISTORY 0
#define CAP_STARTANIM 0
#define CAP_SAVE 0
#define CAP_TRANS 0

#ifdef BWEB
#include "../hyperweb.cpp"
#else
#include "../hyper.cpp"
#endif

#include "subquotient.cpp"
#define SUBQ
#endif

#include "rogueviz.h"

#define solnil (nil || sol)

namespace hr {

namespace subquotient {
  #ifndef SUBQ
  void create_subquotient(int qty = -1, int id = 0);
  #endif
  extern eGeometry gSubquotient;
  }

namespace bringris {

local_parameter_set lps_bringris("bringris:");
local_parameter_set lps_bringris_explore("bringris:explore:", &lps_bringris);
local_parameter_set lps_bringris_play("bringris:play:", &lps_bringris);

struct bgeometry {
  string name;
  string cap;
  flagtype flags;
  int stars, stars_needed, default_max_piece;
  reaction_t create;
  };

bool stars_enabled = true;

enum eBringrisMove { bmUp=4, bmRight=5, bmDown=6, bmLeft=7, bmFullDrop=8, bmTurnLeft=9, bmTurnRight=10, bmDrop=11, bmPause=12, bmNothing=13, bmLast=14 };

vector<string> move_names = { "", "", "", "", "move up", "move right", "move down", "move left", "full drop", "turn left", "turn right", "drop by one", "pause", "do nothing" };

int camera_level;

ld ang = 0, cur_ang = 0;

int lti;

int bgeom = 0;

bool rotate_allowed = false;

bool in_bringris;

bool use_raycaster = true;

bool flashes = true;

int last_adjust, when_t;
transmatrix tView, pView;
cell* ncenter;

cell *well_center;

vector<cell*> level;

vector<cell*> out_level;

map<cell*, int> center_distance;

int camera = 3;

int facing_mod = 0;

int draw_per_level = 2000;

int shape_id, next_shape_id;

cellwalker at;

int move_started;
int move_at;

bool paused;
bool explore;

struct gamedata {
  string bgeom_name;
  string myname;
  string timerstart, timerend;
  int max_piece;
  bool pro_game;
  ld score;
  int bricks, completed, cubes, well_size, levelsize, seconds;
  vector<string> lmap;
  auto sorter() { static double err = -1; return tie(pro_game ? score : err, completed, bricks, cubes, seconds); }
  };

gamedata cur;
vector<gamedata> allsaves;

enum eState {
  tsPreGame, tsFalling, tsBetween, tsCollect, tsGameover
  };

eState state = tsPreGame;

constexpr flagtype HYPERBOLIC = 1;
constexpr flagtype ORBIFOLD = 2;
constexpr flagtype SECRET = 4;
constexpr flagtype NONORIENTABLE = 8;
constexpr flagtype SPHERICAL = 16;
constexpr flagtype EUCLIDEAN = 32;
constexpr flagtype SUBQUOTIENT = 64;
constexpr flagtype HDUAL = 128;
constexpr flagtype BOUNDED_WELL = 256;
constexpr flagtype ASYMMETRIC_ONLY = 512;
constexpr flagtype FLAT_ONLY = 1024;

cell *get_center();
cell *shift_block_target(int dir);
void shift_block(int dir, bool camera_only = false);
void rotate_block(int dir, bool camera_only = false);

void start_new_game();
void clear_map();
void save();
void load();

vector<bgeometry> bgeoms = {
  {"Bring surface", "the original Bringris geometry", HYPERBOLIC, 0, 0, 4, [] {
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
    rotate_allowed = false;
    }},

  {"torus", "Euclidean level geometry", EUCLIDEAN, 0, 0, 4, [] {
    auto& T0 = euc::eu_input.user_axes;
    T0[0][0] = 5;
    T0[0][1] = 0;
    T0[1][0] = 0;
    T0[1][1] = 5;
    euc::eu_input.twisted = 0;
    set_geometry(gEuclidSquare);
    set_variation(eVariation::pure);
    set_geometry(gProduct);
    rotate_allowed = true;
    }},

  {"Cube", "spherical level geometry", SPHERICAL, 0, 0, 3, [] {
    set_geometry(gSmallSphere);
    set_variation(eVariation::pure);
    set_geometry(gProduct);
    rotate_allowed = false;
    }},

  {"Klein bottle", "non-orientable manifold", EUCLIDEAN | NONORIENTABLE, 0, 1000, 4, [] {
    auto& T0 = euc::eu_input.user_axes;
    T0[0][0] = 5;
    T0[0][1] = 0;
    T0[1][0] = 0;
    T0[1][1] = 5;
    euc::eu_input.twisted = 8;
    set_geometry(gEuclidSquare);
    set_variation(eVariation::pure);
    set_geometry(gProduct);
    rotate_allowed = true;
    }},

  {"pentagons", "different tiles on the Bring surface", HYPERBOLIC | HDUAL, 0, 1500, 4, [] {
    using namespace fieldpattern;
    current_extra = 2;
    auto& gxcur = fgeomextras[current_extra];
    while(isize(gxcur.primes) < 1) nextPrime(gxcur);
    fgeomextras[current_extra].current_prime_id = 0;
    enableFieldChange();
    set_geometry(gFieldQuotient);
    set_variation(eVariation::pure);
    set_geometry(gProduct);
    rotate_allowed = false;
    }},

  {"bounded well", "five squares around a vertex", BOUNDED_WELL, 0, 2000, 4, [] {
    set_geometry(g45);
    gp::param = gp::loc(1, 1);
    set_variation(eVariation::unrectified);
    set_geometry(gProduct);

    rotate_allowed = false;
    }},

  {"double cube", "six squares around a vertex", HYPERBOLIC, 0, 2500, 3, [] {
    using namespace fieldpattern;
    current_extra = 3;
    auto& gxcur = fgeomextras[current_extra];
    while(isize(gxcur.primes) < 1) nextPrime(gxcur);
    fgeomextras[current_extra].current_prime_id = 0;
    enableFieldChange();
    set_geometry(gFieldQuotient);
    
    gp::param = gp::loc(1, 1);
    set_variation(eVariation::unrectified);

    set_geometry(gProduct);
    rotate_allowed = true;
    }},

  {"30/6", "six squares around a vertex", HYPERBOLIC, 0, 3000, 4, [] {
    using namespace fieldpattern;
    current_extra = 3;
    auto& gxcur = fgeomextras[current_extra];
    while(isize(gxcur.primes) < 1) nextPrime(gxcur);
    fgeomextras[current_extra].current_prime_id = 2;
    enableFieldChange();
    set_geometry(gFieldQuotient);
    
    gp::param = gp::loc(1, 1);
    set_variation(eVariation::unrectified);

    set_geometry(gProduct);
    rotate_allowed = true;
    }},

  {"42", "seven squares around a vertex", HYPERBOLIC, 0, 4000, 4, [] {
    using namespace fieldpattern;
    current_extra = 4;
    auto& gxcur = fgeomextras[current_extra];
    while(isize(gxcur.primes) < 1) nextPrime(gxcur);
    fgeomextras[current_extra].current_prime_id = 0;
    enableFieldChange();
    set_geometry(gFieldQuotient);
    
    gp::param = gp::loc(1, 1);
    set_variation(eVariation::unrectified);

    set_geometry(gProduct);
    rotate_allowed = false;
    }},

  {"mirrored Bring", "hyperbolic and non-orientable", HYPERBOLIC | NONORIENTABLE | ASYMMETRIC_ONLY, 0, 5000, 4, [] {
    set_geometry(gBring);
    gp::param = gp::loc(1, 1);
    set_variation(eVariation::unrectified);
    start_game();
    subquotient::create_subquotient(2);
    set_geometry(gProduct);
    rotate_allowed = false;
    }},

  {"giant", "like mirrored Bring but much larger", HYPERBOLIC | NONORIENTABLE, 0, 6000, 5, [] {
    using namespace fieldpattern;
    current_extra = 2;
    auto& gxcur = fgeomextras[current_extra];
    while(isize(gxcur.primes) < 1) nextPrime(gxcur);
    fgeomextras[current_extra].current_prime_id = 1;
    enableFieldChange();
    set_geometry(gFieldQuotient);
    
    gp::param = gp::loc(1, 1);
    set_variation(eVariation::unrectified);
    subquotient::create_subquotient(2);

    set_geometry(gProduct);
    rotate_allowed = false;
    cur.well_size = 6;
    }},

  {"orbifold", "one fifth of the giant", HYPERBOLIC | NONORIENTABLE | ORBIFOLD, 0, 7000, 4, [] {
    using namespace fieldpattern;
    current_extra = 2;
    auto& gxcur = fgeomextras[current_extra];
    while(isize(gxcur.primes) < 1) nextPrime(gxcur);
    fgeomextras[current_extra].current_prime_id = 1;
    enableFieldChange();
    set_geometry(gFieldQuotient);
    
    gp::param = gp::loc(1, 1);
    set_variation(eVariation::unrectified);
    subquotient::create_subquotient(10);

    set_geometry(gProduct);
    rotate_allowed = false;
    }},

  {"torus: shear", "Nil geometry: are you sure you want this?", 0, 0, 10000, 4, [] {
    nilv::nilperiod = make_array(5, 0, 5);
    // nilv::set_flags();
    set_geometry(gNil);
    rotate_allowed = false;
    }},

#if CAP_SOLV
  {"torus: Arnold's Cat", "Solv geometry: flat shapes are crazy enough", FLAT_ONLY, 0, 12000, 2, [] {
    asonov::period_xy = 5;
    asonov::period_z = 0;
    asonov::set_flags();
    set_geometry(gArnoldCat);
    rotate_allowed = false;
    }},
#endif
  };

void create_game();

void enable_bgeom() {
  stop_game_and_switch_mode(rg::nothing);
  cur.well_size = 10;
  bgeoms[bgeom].create();
  cur.max_piece = bgeoms[bgeom].default_max_piece;
  start_game();
  create_game();
  state = tsPreGame;
  }

void enable_bgeom(int b) {
  bgeom = b;
  enable_bgeom();
  }

flagtype bflags() { return bgeoms[bgeom].flags; }
  
using code_t = vector<pair<int, int>>;

struct piecedata {
  code_t code;
  int multi;
  int penalty;
  int symmetries;
  int count;
  };

vector<piecedata> piecelist;

map<set<cell*>, int> seen_blocks;

bool listed(const vector<cellwalker>& v, cell* c) {
  for(auto cw: v) if(cw.at == c) return true;
  return false;
  }

int down_dir() { return nil ? 4 : sol ? 0 : cwt.at->type-1; }
int up_dir() { return nil ? 1 : sol ? 6 : cwt.at->type-2; }

cell *get_at(cell *lev, int z) {
  if(mproduct)
    return hybrid::get_at(lev, z);
  else {
    // auto co = nilv::get_coord(lev->master);
    // co[2] += z * co[0];
    // co[1] = z;
    // return nilv::get_heptagon_at(co)->c7;
    while(z<0) z++, lev=lev->cmove(up_dir());
    while(z>0) z--, lev=lev->cmove(down_dir());
    return lev;
    }
  }

int get_z(cell* c) {
  if(mproduct)
    return hybrid::get_where(c).second;
  else if(nil)
    return nilv::get_coord(c->master)[1];
  else if(sol)
    return asonov::get_coord(c->master)[2];
  else
    exit(1);
  }

pair<cell*, int> get_where(cell *what) {
  if(mproduct)
    return hybrid::get_where(what);
  else {
    int z = get_z(what);
    int zm = z;
    while(zm>0) what = what->cmove(up_dir()), zm--;
    while(zm<0) what = what->cmove(down_dir()), zm++;
    return {what, z};
    // co[2] -= co[1] * co[0];
    // co[1] = 0;
    // cell *c = nilv::get_heptagon_at(co)->c7;
    // return {c, z};
    }
  }

set<cell*> as_set(const vector<cellwalker>& shape) {
  set<cell*> res;
  for(auto sh: shape) res.insert(sh.at);
  return res;
  }

bool same(const vector<cellwalker>& shape, const vector<cellwalker>& shape2) {
  set<cell*> allcells;
  if(isize(shape) != isize(shape2)) return false;
  for(auto sh: shape) allcells.insert(sh.at);
  for(auto sh: shape2) allcells.erase(sh.at);
  return allcells.empty();
  }

cellwalker flatspin(cellwalker cw, int i) {
  if(solnil)
    cw.spin = i;
  else
    cw.spin = gmod(cw.spin + (cw.mirrored ? -i : i), cw.at->type - (mhybrid ? 2 : 0));
  return cw;
  }

int add_dir(cellwalker orig, int i, int sym = 0) {
  if(mproduct) {
    if(i >= orig.at->type-2) {
      if(sym&2) i ^= (orig.at->type-1) ^ (orig.at->type-2);
      return i;
      }
    else {
      if(sym&1) i = -i;
      return flatspin(orig, i).spin;
      }
    }
  return i;
  }

cellwalker add(cellwalker orig, int i, int sym = 0) {
  if(mproduct) {
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

vector<transmatrix> build_shape_matrices(const code_t& code, cellwalker start, int sym = 0) {
  vector<cellwalker> all = {start};
  vector<transmatrix> allm = {Id};
  for(auto c: code) {
    all.push_back(add(all[c.first], c.second, sym));
    allm.push_back(allm[c.first] * currentmap->adj(all[c.first].at, add_dir(all[c.first], c.second, sym)));
    }
  return allm;
  }

int penalty(const vector<cellwalker>& shape, const code_t& code) {
  int p = 0;
  if(mproduct) {
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
  if(solnil)
    for(auto s: shape) 
      if(get_z(s.at) > get_z(shape[0].at))
        p += 10000;
  if(bflags() & HDUAL) {

    for(auto s: shape) 
      if(get_z(s.at) > get_z(shape[0].at))
        p += 20000;
    /* if(!dists.count(shape[0].at->move(2)))
      p += 40000;
    if(!dists.count(shape[0].at->move(3)))
      p += 20000; */
    }
  return p;
  }

bool builds(const vector<cellwalker>& shape, const code_t& code, int sym = 0, int eliminate = -1) {
  if(isize(shape) != isize(code)+1) return false;
  int ori = (solnil) ? 1 : mproduct ? shape[0].at->type-2 : shape[0].at->type;
  for(auto sh: shape) for(int i=0; i<ori; i++) {
    vector<cellwalker> shape2 = build_from(code, cellwalker(sh.at, i), sym);
    if(eliminate != -1) seen_blocks.emplace(as_set(shape2), eliminate);
    if(same(shape, shape2)) 
      return true;
    }
  return false;
  }

void generate_shapes_rec(vector<cellwalker>& sofar, code_t& code, int cnt) {
  if(isize(sofar) == cnt) {
    int p = penalty(sofar, code);
    auto ass = as_set(sofar);
    
    if(seen_blocks.count(ass)) {
      auto& pc = piecelist[seen_blocks[ass]];
      if(p < pc.penalty) pc.penalty = p, pc.code = code;
      pc.multi++;
      return;
      }

    int id = 0;
    for(auto& pc: piecelist) {
      if(builds(sofar, pc.code, 0, id)) {
        if(p < pc.penalty) pc.penalty = p, pc.code = code;
        pc.multi++;
        return;
        }
      id++;
      }
    int syms = 0;
    bool invalid = false;
    for(int i: {0,1,2,3}) if(builds(sofar, code, i)) { 
      syms++; 
      if((bflags() & ASYMMETRIC_ONLY) && i == 1 && cnt >= 4)
        invalid = true;
      }
    if(invalid) return;
    bool vertical = true;
    for(auto c: code) if(c.second < 4) vertical = false;
    if(vertical) syms *= 2;
    if(isize(code) == 0) syms = 1;
    piecelist.push_back(piecedata{code, 1, p, syms, 0});
    return;
    }
  for(int i=0; i<isize(sofar); i++)
  for(int t=0; t<sofar[i].at->type; t++) {
    if(sol && !among(t, 4, 5, 10, 11)) continue; 
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
  vector<cellwalker> cws = { get_at(get_center(), -cur.well_size - 1) };
  code_t co = {};
  generate_shapes_rec(cws, co, cnt);
  }

void list_all() {
  println(hlog, "number of shapes = ", isize(piecelist));
  // for(auto sh: piecelist) println(hlog, "multi=", sh.multi, " penalty=", sh.penalty, " syms=", sh.symmetries, " => ", sh.code);
  }

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
  if(cur.well_size <= 5) y *= 2;
  return hipso[gmod(y, 13)];
  }

void draw_shape() {
  auto shape = build_from(piecelist[shape_id].code, at);
  for(auto c: shape) {
    int y = -get_z(c.at);
    c.at->wall = waWaxWall, c.at->landparam = get_hipso(y);
    }
  ray::reset_raycaster_map();
  }

bool shape_drawn() {
  auto shape = build_from(piecelist[shape_id].code, at);
  for(auto c: shape)
    return c.at->wall == waWaxWall;
  return false;
  }
  
void remove_shape() {
  auto shape = build_from(piecelist[shape_id].code, at);
  for(auto c: shape)
    c.at->wall = waNone;
  ray::reset_raycaster_map();
  }

bool shape_conflict(cellwalker cw) {  
  auto shape = build_from(piecelist[shape_id].code, cw);

  /* self-conflict possible in the orbifold */
  for(int i=0; i<isize(shape); i++) for(int j=0; j<i; j++)
    if(shape[i].at == shape[j].at)
      return true;

  for(auto c: shape)
    if(c.at->wall)
      return true;
  return false;
  }

bool check_bshift(cellwalker c0, cellwalker c1) {
  if(c0.at->type != 6) return false;
  auto shape0 = build_from(piecelist[shape_id].code, c0);
  auto shape1 = build_from(piecelist[shape_id].code, c1);
  for(int i=0; i<isize(shape0); i++) {
    if(isNeighbor(shape0[i].at, c0.at))
    if(isNeighbor(shape1[i].at, c1.at))
    if(shape0[i].at != shape1[i].at)
    if(!isNeighbor(shape0[i].at, shape1[i].at)) {
      cell *cfound = nullptr;
      forCellEx(c, shape0[i].at) if(isNeighbor(c, shape1[i].at)) {
        cfound = c;
        }
      if(!cfound) continue;
      if(!cfound->wall) continue;
      if(shape1[i].at->cmove(up_dir())->wall) return true;
      }
    }
  return false;
  }

ld current_move_time_limit() {
  // return 50000 * pow(.9, cur.completed) + 10000. / (1 + cur.completed);
  return 3500 * pow(.995, cur.completed * isize(level));
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

void reset_view();
void set_tview(transmatrix T);

void new_piece() {
  if(well_center && true) {
    again:
    if(get_where(at.at).first != well_center) {
      at.at = get_at(get_where(at.at).first, -cur.well_size - 1);
      int d = center_distance[get_where(at.at).first];
      for(int i=0; i<4; i++) {
        auto mov = get_where(shift_block_target(i)).first;
        if(center_distance.count(mov) && center_distance[mov] < d) {
          shift_block(i, true);
          goto again;
          }
        }
      println(hlog, "failed to recenter");
      }
    while(at.spin) rotate_block(1, true);
    }  
  at.at = get_at(get_where(at.at).first, -cur.well_size - 1);
  shape_id = next_shape_id;
  next_shape_id = choose_piece();
  if(shape_conflict(at)) {
    playSound(cwt.at, "die-bomberbird");
    state = tsGameover;
    #if RVCOL
    if(cur.pro_game && cur.max_piece == bgeoms[bgeom].default_max_piece)
      rogueviz::rv_leaderboard("Bringris: " + bgeoms[bgeom].name, cur.score, 1, rvlc::num, lalign(0, cur.bricks, " ", cur.cubes, " ", cur.completed));
    #endif
    save();
    }
  else {
    draw_shape();
    state = tsFalling;
    move_started = ticks;
    move_at = ticks + current_move_time_limit();
    }    
  }

vector<int> by_level;

void find_lines() {
  by_level.clear();
  // println(hlog, "Removing levels");
  
  for(int z=1; z<=cur.well_size; z++) {
    int ct = 0;
    for(auto lev: level) {
      cell *c = get_at(lev, -z);
      ct += (c->wall ? 1 : 0);
      }
    by_level.push_back(ct);
    }
  // println(hlog, by_level, " / ", isize(level));
  
  int points = 0;
  
  if(true) {
    for(int z=1; z<=cur.well_size; z++) if(by_level[z-1] >= isize(level)) {
      points++;
      for(auto lev: level) {
        cell *c = get_at(lev, -z);
        to_disappear.insert(c);
        }
      }
    }
  else {
    // int lines_found = 0;
    for(int z=1; z<=cur.well_size; z++) {
      for(auto lev: level) for(int d=0; d<lev->type; d++) {
        cellwalker cw(get_at(lev, -z), d);
        cellwalker cw0 = cw;
        bool filled = true;
        do {
          if(!cw.at->wall) filled = false;
          cw += wstep;
          cw = flatspin(cw, lev->type/2);
          }
        while(cw != cw0);
        if(filled) {
          // lines_found = true;
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
    cur.score += 100000. * points * (points+1.) / current_move_time_limit();
    cur.completed += points;
    playSound(cwt.at, points == 1 ? "pickup-gold" : "orb-mind");
    #if RVCOL
    if(points == 4 && cur.pro_game && cur.max_piece == 4 && bgeoms[bgeom].default_max_piece == 4) rogueviz::rv_achievement("BRINGRISFOUR");
    #endif
    }
  }

void disappear_lines() {
  for(auto lev: level) {
    int nz = 1;
    for(int z=1; z<=cur.well_size; z++) {
      cell *c1 = get_at(lev, -z);
      if(!to_disappear.count(c1)) {
        cell *c0 = get_at(lev, -nz);
        c0->wall = c1->wall;
        c0->landparam = get_hipso(nz);
        nz++;
        }
      }
    while(nz <= camera_level) {
      cell *c0 = get_at(lev, -nz);
      c0->wall = waNone;
      nz++;
      }
    }
  to_disappear.clear();
  state = tsBetween;
  ray::reset_raycaster_map();
  }

void state_loop() {
  find_lines();
  if(to_disappear.empty()) new_piece();
  }
    
void fallen() {
  draw_shape();
  cur.bricks++;
  cur.cubes += isize(piecelist[shape_id].code)+1;
  state = tsBetween;
  playSound(cwt.at, "closegate");
  cur.score += 200000. / (current_move_time_limit() * 3 + ticks - move_started);
  }

void drop() {
  remove_shape();
  cellwalker fall = at;
  fall.at = fall.at->cmove(down_dir());
  if(shape_conflict(fall))
    fallen();
  else {
    at = fall;
    draw_shape();
    }
  if(solnil) {
    pView = pView * currentmap->adj(cwt.at, down_dir());
    when_t = ticks + turn_animation;
    }
  }

void auto_drop() {
  drop();
  move_at = ticks + current_move_time_limit();
  }

void fulldrop() {
  remove_shape();
  cellwalker fall = at;
  int no = 0;
  auto last = fall;
  while(true) {
    last = fall;
    fall.at = fall.at->cmove(down_dir());
    if(shape_conflict(fall)) break;
    no++;
    }
  playSound(cwt.at, "hit-crush2");
  // println(hlog, "dropped by ", no);
  fall = last;
  at = fall;
  move_at = ticks + current_move_time_limit();
  draw_shape();
  if(!no) fallen();
  }

void verify_matrix(transmatrix T) {
  vector<ld> ver;
  for(int i=0; i<3; i++)
  for(int j=0; j<3; j++) {
    ld a = T[0][i] * T[0][j] + T[1][i]*T[1][j] - T[2][i] * T[2][j];
    ver.push_back(kz(a));
    }
  println(hlog, ver);
  }

ld move_dist;

void set_view() {
  move_dist = hdist0(currentmap->adj(cwt.at, 0) * C0);
  
  if(in_h2xe() && PURE && S3 == 4) {
    ld dist = PIU(hdist0(get_corner_position(currentmap->gamestart(), 0)));
    dist -= 1e-4;
    move_dist = PIU(hdist(get_corner_position(currentmap->gamestart(), 0), get_corner_position(currentmap->gamestart(), 1)));
    tView = xpush(-dist) * tView;
    tView = spin(135._deg) * tView;
    }  
  if(in_h2xe() && UNRECTIFIED)
    tView = spin(135._deg) * tView;
  if(in_h2xe() && S7 == 4)
    tView = spin90() * tView;
  if(in_s2xe())
    tView = spin90() * tView;
  if(in_e2xe())
    tView = spin90() * tView;
  }

void set_tview(transmatrix T) {

  View = T * tView;

  transmatrix rel = pView * inverse(View);
  fixmatrix(rel);

  centerover = ncenter;
  optimizeview();
  ncenter = centerover;
  // tView = View;
  if(bgeom == 4)
    tView = spin(72._deg*at.spin);
  else
    tView = spin(90._deg*at.spin);
  if(at.mirrored)
    tView = MirrorY * tView;
  // tView = spin(90._deg*at.spin);
  set_view();  

  pView = rel * tView;
  when_t = ticks + turn_animation;
  }

void rotate_block(int d, bool camera_only) {
  if(!rotate_allowed && !camera_only) {
    playSound(cwt.at, "hit-crush3");
    return;
    }
  if(!camera_only) remove_shape();
  cellwalker at1 = flatspin(at, d);
  if(camera_only || !shape_conflict(at1)) {
    at = at1;
    set_tview(spin(d*90._deg));
    }
  else playSound(cwt.at, "hit-crush3");
  if(!camera_only) draw_shape();
  }    

int nilmap(int dir) {
  if(nil) {
    int nm[4] = {3, 2, 0, 5};
    return nm[dir];
    }
  if(sol) {
    int nm[4] = {11, 10, 5, 4};
    return nm[dir];
    }
  exit(1);
  }

cell *shift_block_target(int dir) {
  return flatspin(at, dir).cpeek();
  }

void shift_block(int dir, bool camera_only) {
  int t = currentmap->gamestart()->type;
  if(mproduct) t -= 2;

  if(!camera_only) remove_shape();
  
  cellwalker at1;
  
  if(solnil) {
    at1.at = at.at->cmove(nilmap(dir));
    }
  else if(t&1) {
    if(dir == 3)
      at1 = flatspin(flatspin(at, 4) + wstep, 3);
    if(dir == 1)
      at1 = flatspin(flatspin(at, 2) + wstep, 1);
    if(dir == 2)
      at1 = flatspin(flatspin(at, 3) + wstep, 4);
    if(dir == 0)
      at1 = flatspin(flatspin(at, 1) + wstep, 2);
    }
  else {
    int kspin = (t/2) - dir;
    at1 = flatspin(at, dir);  
    at1 = flatspin(at1 + wstep, kspin);
    }

  ld angle = dir * 90 * degree;
  
  if(camera_only || !shape_conflict(at1)) {
    // playSound(cwt.at, "hit-crush1");
    #if RVCOL
    if(check_bshift(at, at1)) rogueviz::rv_achievement("BSHIFT");
    #endif
    at = at1;
    if(solnil) {
      pView = pView * currentmap->adj(cwt.at, nilmap(dir));
      when_t = ticks + turn_animation;
      }
    else
      set_tview(spin(-angle) * ypush(-move_dist) * spin(angle));
    if(!camera_only) draw_shape();
    }
  else playSound(cwt.at, "hit-crush3");
  }

void bringris_action(int k) {
  if(k >= 4 && k < 8) shift_block((k-4)^2);
  if(k == bmTurnLeft) rotate_block(1);
  if(k == bmTurnRight) rotate_block(-1);
  if(k == bmDrop) drop();
  if(k == bmFullDrop) fulldrop();
  if(k == bmPause) paused = true;
  }

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
    if(id < draw_per_level) {
      auto go = [&] (int i) {
        cell *c1 = c->cmove(i);
        dq::enqueue_by_matrix_c(c1, optimized_shift(V * currentmap->adj(c, i)));       
        };
      if(mproduct) {
        for(int i=0; i<c->type-2; i++) go(i);
        }
      else if(sol) {
        go(4); go(5); go(10); go(11);
        }
      else if(nil) {
        go(0); go(2); go(3); go(5);
        }
      }      
    dq::drawqueue_c.pop();
    id++;
    }  
  }

transmatrix smooth;

void change_depth(shiftmatrix& V, int newlevel, int zlev) {
  if(solnil) {
    while(newlevel > zlev) zlev++, V = V * currentmap->adj(cwt.at, down_dir());
    while(newlevel < zlev) zlev--, V = V * currentmap->adj(cwt.at, up_dir());
    }
  else if(in_h2xe())
    V = shiftless(V.T, cgi.plevel * (newlevel - zlev));
  else
    V = shiftless(V.T * zpush(cgi.plevel * (newlevel - zlev)));
  }

void draw_wirecube_at(cell *c, const transmatrix& rel, int zlev, color_t col) {

  auto where_c = get_where(c);
  auto c_camera = get_at(where_c.first, zlev);
  
  for(shiftmatrix V: current_display->all_drawn_copies[c_camera]) {
    change_depth(V, where_c.second, zlev);
    for(int i=0; i<c->type; i++)
      queuepolyat(V * rel, cgi.shWireframe3D[i], 0, PPR::SUPERLINE).outline = col;
    }
  }

void draw_piece(int zlev, int id) {
  auto shape = build_from(piecelist[id].code, at);
  auto matrices = build_shape_matrices(piecelist[id].code, at);

  auto where_at = get_where(at.at);
  
  vid.linewidth *= 3;

  int mid = 0;

  for(auto c: shape) {    
    auto where_c = get_where(c.at);
    color_t levels[5] = {color_t(0xFFFFFFFFF), color_t(0xFFFF00FF), color_t(0xFF8000FF), color_t(0xFF0000FF), color_t(0xC000C0FF) };
    draw_wirecube_at(at.at, smooth * matrices[mid++], zlev, levels[where_at.second-where_c.second]);
    }  
  
  vid.linewidth /= 3;
  }

void draw_holes(int zlev) {
  bool d = state == tsFalling && shape_drawn();
  if(d) remove_shape();
  for(auto lev: level) {
    bool covered = false;
    for(int z=cur.well_size; z>=1; z--) {
      cell *c1 = get_at(lev, -z);
      if(c1->wall) covered = true;
      else if(covered) {
        vid.linewidth *= 4;
        draw_wirecube_at(c1, Id, zlev, (get_hipso(z) << 8) | 0xFF);
        vid.linewidth /= 4;
        }
      }
    }
  if(d) draw_shape();
  }

void draw_all_noray(int zlev) {
  for(auto lev: level) {
    for(int z=0; z<=camera_level+1; z++) {
      cell *c1 = get_at(lev, -z);
      
      if(c1->wall) {
      
        auto c_camera = get_at(lev, zlev);
        
        for(shiftmatrix V: current_display->all_drawn_copies[c_camera]) {
          change_depth(V, -z, zlev);
          
          color_t wcol = c1->landparam;
          if(c1->wall == waBarrier) wcol = winf[waBarrier].color;

          int d = (wcol & 0xF0F0F0) >> 4;

          forCellIdCM(c2, i, c1)
            if(!c2->wall) {
              color_t col = darkena(wcol - d * get_darkval(c1, i), 0, 0xFF);
              auto &q = queuepolyat(V, cgi.shWall3D[i], col, PPR::WALL);
              q.tinf = &floor_texture_vertices[cgi.shFloor.id];
              ensure_vertex_number(*q.tinf, q.cnt);
              }
          }
        }
      }
    }
  }

bool use_equidistant;

void bringris_frame() {
  if(!in_bringris) return;
  ray::want_use = use_raycaster ? 2 : 0;
  #if CAP_VR
  vrhr::hsm = explore ? vrhr::eHeadset::holonomy : vrhr::eHeadset::reference;
  vrhr::eyes = use_equidistant ? vrhr::eEyes::equidistant : vrhr::eEyes::truesim;
  #endif
  
  int zlev = get_z(centerover);

  if(state == tsCollect) for(cell *c: to_disappear) c->landparam = flashes ? rand() & 0xFFFFFF : 0x101010;
  
  // just_gmatrix = true;

  create_matrices();

  if(!use_raycaster) 
    draw_all_noray(zlev);

  if(anyshiftclick) draw_holes(zlev);
  
  if(state == tsFalling && !explore && !cur_ang) draw_piece(zlev, shape_id);
  }

renderbuffer *next_buffer;

void draw_screen(int xstart, bool show_next) {
  int steps = camera_level - (-get_z(at.at));
  if(state != tsFalling) steps = camera_level - (cur.well_size + 1);

  #if CAP_VR
  if(!explore) {
    E4;
    vrhr::hmd_at_ui = vrhr::hmd_ref_at * cspin(0, 2, 30._deg);
    }
  #endif
    
  
  dynamicval<display_data> ccd(*current_display);
  current_display->xmax = xstart * 1. / vid.xres;
  ray::max_cells = (isize(level) + isize(out_level)) * (camera_level+2);
  
  if(explore) {
    gamescreen();
    lps_enable(&lps_bringris_explore);
    }
  else {  
    lps_enable(&lps_bringris_play);
    NLP = Id;
    View = pView;
    if(nil) {
      centerover = at.at;
      rotate_view(cspin90(2, 1));
      shift_view(ztangent(3 * nilv::nilwidth));
      rotate_view(cspin90(1, 0));
      anims::moved();
      }
    else if(sol) {
      centerover = at.at;
      rotate_view(cspin180(1, 2));
      shift_view(ztangent(1));
      rotate_view(cspin90(1, 0));
      anims::moved();
      }
    else {
      ld lv = -cgi.plevel * steps;
      shift_view(ztangent(lv));
      rotate_view(cspin(1, 2, cur_ang));
      shift_view(ztangent(cgi.plevel * (2 + cur.max_piece)));
      centerover = ncenter;
      anims::moved();
      }

    // make_actual_view();
    // anims::moved();
    
    sightranges[geometry] *= 100;
    if(state == tsFalling && !explore && !cur_ang && !lctrlclick) remove_shape();    
    gamescreen();
    if(state == tsFalling && !explore && !cur_ang) draw_shape();

    extern void render_next(int xstart);
    render_next(xstart);

    sightranges[geometry] /= 100;

    if(state == tsBetween) state_loop();
    
    if(state == tsCollect && ticks >= move_at) 
      disappear_lines();
    
    if(ticks >= move_at && state == tsFalling && cur.pro_game) {
      auto_drop();
      }

    View = pView;
    centerover = ncenter;      
    NLP = Id;
    }
  }

void create_game();

void geometry_menu() {
  clearMessages();
  dialog::init("Bringris geometries");
  dialog::addBreak(100);
  int total_stars = 0;
  for(int i=0; i<isize(bgeoms); i++) total_stars += bgeoms[i].stars;
  for(int i=0; i<isize(bgeoms); i++) {
    if(total_stars >= bgeoms[i].stars_needed || !stars_enabled) {
      dialog::addTitle(bgeoms[i].name, i == bgeom ? 0xFF00 : 0xFF0000, 150);
      dialog::items.back().key = 'a' + i;
      dialog::add_action([i] {
        enable_bgeom(i);
        });
      dialog::addInfo(bgeoms[i].cap);
      dialog::items.back().key = 'a' + i;
      if(!stars_enabled) ;
      else if(bgeoms[i].stars) dialog::addInfo("stars: " + its(bgeoms[i].stars));
      else dialog::addBreak(100);
      dialog::addBreak(50);
      if(i == bgeom) bgeoms[i].flags &= ~SECRET;
      if(bgeoms[i].flags & SECRET) {
        dialog::items.pop_back();
        dialog::items.pop_back();
        dialog::items.pop_back();
        }
      }
    else {
      dialog::addTitle("locked", 0x404040, 150);
      if(stars_enabled) dialog::addInfo("stars needed: " + its(bgeoms[i].stars_needed));
      dialog::addBreak(50);
      }
    }
  dialog::addBreak(100);
  dialog::addSelItem("max piece", its(cur.max_piece), 'M');
  dialog::add_action([] {
    cur.max_piece++;
    if(cur.max_piece == 6) cur.max_piece = 2;
    create_game();
    state = tsPreGame;
    });
  if(bflags() & ASYMMETRIC_ONLY)
    dialog::addInfo("(only asymmetric large pieces)");
  else if(bflags() & FLAT_ONLY)
    dialog::addInfo("(only flat pieces)");
  else
    dialog::addBreak(100);

  dialog::addBreak(100);
  if(stars_enabled) {
    if(total_stars < 6000)
    dialog::addHelp("Collect stars to unlock more spaces!\n\n"
      "training mode: 1 block removed = 1 star\n\n"
      "expert mode: 1 block removed = 5 stars\n\n"
      "only best score per space counts");
    dialog::addInfo("currently " + its(total_stars) + " stars");
    }
  dialog::addBack();
  dialog::display();
  }

void visual_menu() {
  cmode = sm::MAYDARK;
  gamescreen();
  dialog::init("Bringris visuals");

  dialog::addBoolItem_action("flashes on level completed", flashes, 'f');

  dialog::addBoolItem_action("use raycasting", use_raycaster, 'r');

  dialog::addSelItem(XLAT("iterations in raycasting"), its(ray::max_iter_current()), 's');
  dialog::add_action([&] {
    auto& di = dialog::editNumber(ray::max_iter_current(), 0, 600, 1, 60, XLAT("iterations in raycasting"), "");
    di.reaction = ray::reset_raycaster;
    });

  dialog::addSelItem(XLAT("reflective walls in raycasting"), fts(ray::reflect_val), 'R');
  dialog::add_action([&] {
    auto& di = dialog::editNumber(ray::reflect_val, 0, 1, 0.1, 0, XLAT("reflective walls"), "");
    di.reaction = ray::reset_raycaster;
    });

  dialog::addSelItem(XLAT("cells to draw per level"), its(draw_per_level), 'R');
  dialog::add_action([&] {
    dialog::editNumber(draw_per_level, 0, 1000, 500, 50, XLAT("cells to draw"), 
      "If the level size is 30, 600 cells to draw means that every cell is drawn 20 times on average. "
      "Used when raycasting is off, and to draw the wireframes");
    });
  
  dialog::addBreak(200);
  
  #if CAP_VR  
  dialog::addBoolItem_action(XLAT("VR enabled"), vrhr::enabled, 'o');
  if(!vrhr::enabled)
    dialog::addBreak(100);
  else if(vrhr::failed)
    dialog::addInfo(XLAT("error: ") + vrhr::error_msg, 0xC00000);
  else
    dialog::addInfo(XLAT("VR initialized correctly"), 0x00C000);
  
  if(vrhr::active()) {
    dialog::addBoolItem_action(XLAT("equidistant VR"), use_equidistant, 'e');
    if(use_equidistant)
      dialog::addInfo("(distances are seen correctly)");
    else
      dialog::addInfo("(simulate non-Euclidean binocular vision)");
    add_edit(vrhr::cscr);
    }
  else
    dialog::addBreak(300);
  #endif
    
  dialog::addBreak(100);
  dialog::addBack();  

  dialog::display();
  }
  
void settings_menu() {
  emptyscreen();
  dialog::init("Bringris settings");
  dialog::addItem("alternative geometry", 'g');
  dialog::add_action_push(geometry_menu);
  dialog::addItem("visuals & Virtual Reality", 'v');
  dialog::add_action_push(visual_menu);
  dialog::addItem("configure keys", 'k');
  dialog::add_action_push(multi::get_key_configurer(1, move_names, "Bringris keys", multi::scfg_default));

  #if CAP_AUDIO
  add_edit(effvolume);
  if(music_available) add_edit(musicvolume);
  #endif

  dialog::addBreak(100);

  #if CAP_FILES && !ISWEB
  dialog::addItem("save the current config", 's');
  dialog::add_action([] {
    dynamicval<eGeometry> g(geometry, gNormal);
    saveConfig();
    });
  #endif
  
  dialog::addBack();
  dialog::display();
  }

bool hi_pro;

void hiscore_menu() {
  emptyscreen();
  dialog::init("High scores");
  string s = bgeoms[bgeom].name;
  if(cur.max_piece != bgeoms[bgeom].default_max_piece) s = s + " (block " + its(cur.max_piece) + ")";
  dialog::addInfo(s);
  dialog::addItem(hi_pro ? "expert mode" : "training mode", 'm');
  dialog::add_action([] { hi_pro = !hi_pro; });
  vector<gamedata*> v;
  for(auto& ad: allsaves)
    if(ad.bgeom_name == bgeoms[bgeom].name && ad.max_piece == cur.max_piece && ad.pro_game == hi_pro)
      v.push_back(&ad);
  sort(v.begin(), v.end(), [] (gamedata* g1, gamedata* g2) { return g1->sorter() > g2->sorter(); });
  dialog::start_list(900, 900, '1');
  for(auto ad: v) {
    dialog::addSelItem(ad->myname, hi_pro ? fts(ad->score) : its(ad->completed), dialog::list_fake_key++);
    dialog::add_action_push([ad] {
      emptyscreen();
      dialog::init();
      if(hi_pro) dialog::addSelItem("score", fts(ad->score), 's');
      dialog::addSelItem("levels", fts(ad->completed), 'l');
      dialog::addSelItem("bricks", fts(ad->bricks), 'b');
      dialog::addSelItem("cubes", fts(ad->cubes), 'c');
      dialog::addItem("explore", 'e');
      dialog::add_action([ad] {

        clear_map();

        for(int z=0; z<=ad->well_size; z++) {
          println(hlog, "z = ", z);
          string s = ad->lmap[z];
          println(hlog, "s = ", s);
          int index = 0;
          for(auto lev: level) {
            cell *c = get_at(lev, -z);
            char key = s[index++];
            if(key == '.')  c->wall = waNone;
            else c->wall = waWaxWall, c->landparam = get_hipso(z);
            }
          cur.lmap.push_back(s);
          }

        state = tsGameover;  explore = true;
        ray::reset_raycaster_map();
        popScreen();
        popScreen();
        });
      dialog::addBack();
      dialog::display();
      });
    }
  dialog::end_list();
  dialog::addBack();
  dialog::display();
  }

void adjust_animation(ld part) {
  if(solnil) {
    hyperpoint sh = pView * C0;
    sh = lerp(C0, sh, 1-part);
    pView = eupush(sh);
    smooth = inverse(pView);
    }
  else {
    transmatrix T = pView * inverse(tView);
    hyperpoint vec = inverse_exp(shiftless(tC0(T)));
    transmatrix Tspin = gpushxto0(tC0(T)) * T;
    ld alpha = atan2(Tspin*xpush0(1));
    // println(hlog, "vec=", vec, " part = ", part);
    pView = spin(alpha * part) * gpushxto0(direct_exp(vec*part)) * pView;
    fixmatrix(pView);
    View = tView;
    smooth = inverse(pView) * cview().T;
    }
  }

bool next_fail = false;

int BRINGRIS_TEXTURESIZE = 256;

int nxmin, nxmax, nymin, nymax;

auto ah = addHook(hooks_resetGL, 500, [] {
  println(hlog, "hooks_resetGL called");
  if(next_buffer) { delete next_buffer; next_buffer = nullptr; }
  });

void render_next(int xstart) {
  if(!next_buffer && !next_fail) {
    next_buffer = new renderbuffer(BRINGRIS_TEXTURESIZE, BRINGRIS_TEXTURESIZE, true);
    if(!next_buffer->valid) {
      next_fail = true;
      delete next_buffer;
      next_buffer = nullptr;
      println(hlog, "failed to create next_buffer");
      }
    else println(hlog, "valid next_buffer created");
    }
  
  nxmin = (xstart + vid.fsize);  
  nxmax = (vid.xres - vid.fsize);
  nymin = vid.yres - (vid.fsize * (18+8));  
  nymax = vid.yres - (vid.fsize * 18);

  int zlev = get_z(centerover);

  if(!next_buffer) {
    if(1) {
      dynamicval<display_data> ccd(*current_display);
      current_display->xmin = nxmin * 1. / vid.xres;
      current_display->xmax = nxmax * 1. / vid.xres;
      current_display->ymin = 1 - nymax * 1. / vid.yres;
      current_display->ymax = 1 - nymin * 1. / vid.yres;
      calcparam();
      initquickqueue();
      draw_piece(zlev, next_shape_id);
      quickqueue();
      }

    calcparam();
    return;
    }
  
  if(1) {
    resetbuffer rb;
    next_buffer->enable();
    dynamicval<int> dx(vid.xres, BRINGRIS_TEXTURESIZE);
    dynamicval<int> dy(vid.yres, BRINGRIS_TEXTURESIZE);
    dynamicval<ld> dxmi(current_display->xmin, 0);
    dynamicval<ld> dxma(current_display->xmax, 1);
    dynamicval<ld> dymi(current_display->ymin, 0);
    dynamicval<ld> dyma(current_display->ymax, 1);
    #if CAP_VR
    dynamicval<int> dvr(vrhr::state, 0);
    #endif
    calcparam();
    current_display->set_viewport(0);
    setGLProjection();
    
    // gamescreen(1);
    initquickqueue();
    draw_piece(zlev, next_shape_id);
    quickqueue();
    rb.reset();
    }
  }

EX void reset_vr_ref() {
  #if CAP_VR
  vrhr::hmd_ref_at = vrhr::hmd_at_ui = vrhr::hmd_at;
  #endif
  }

EX void display_next(int xstart) {

  if(next_buffer) {
    current_display->next_shader_flags = GF_TEXTURE;
    dynamicval<eModel> m(pmodel, mdPixel);
    current_display->set_all(0, 0);
    glBindTexture(GL_TEXTURE_2D, next_buffer->renderedTexture);
    glhr::id_modelview();
    glhr::set_depthtest(false);
    glhr::color2(0xFFFFFFFF);
    vector<glhr::textured_vertex> tvx;
    for(int a=0; a<6; a++) {
      int dx[6] = {0, 1, 1, 0, 0, 1};
      int dy[6] = {0, 0, 1, 0, 1, 1};
      glhr::textured_vertex tx;
      tx.coords[2] = 0;
      tx.coords[3] = 1;
      tx.coords[0] = (dx[a] ? nxmax : nxmin) - current_display->xcenter;
      tx.coords[1] = (dy[a] ? nymax : nymin) - current_display->ycenter;
      tx.texture[0] = dx[a];
      tx.texture[1] = dy[a];
      tvx.push_back(tx);
      }
    glhr::prepare(tvx);
    glDrawArrays(GL_TRIANGLES, 0, 6);    
    }
  }

purehookset bringris_extensions;

void run() {

  clearMessages();
  dialog::init();

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
  else adjust_animation((ticks - last_adjust) * 1. / (when_t - last_adjust));

  last_adjust = ticks;
  
  ray::want_use = use_raycaster ? 2 : 0;
  sightranges[geometry] = 50;
  if(!solnil) vid.cells_drawn_limit = 1;
  else vid.cells_drawn_limit = 2000;

  cmode = sm::NORMAL | sm::CENTER;
  if(!explore) cmode |= sm::VR_MENU;

  int xstart = vid.xres;
  if(!nohud) xstart -= vid.fsize * 10;
  
  getcstat = '-';
  
  bool show_next = state != tsGameover && state != tsPreGame && !paused;

  #if CAP_VR  
  dynamicval<reaction_t> d(vrhr::change_ui_bounds, [show_next, xstart] {
    vrhr::ui_xmin = xstart;
    vrhr::ui_xmax = vid.xres;
    if(show_next && !explore)
      vrhr::ui_ymin = vid.yres - vid.fsize * 28;
    });
  #endif

  draw_screen(xstart, show_next);
  
  calcparam();
  
  bool in_menu = !show_next && !explore;
  
  if(nohud) {
    describeMouseover();
    }
  else {
  for(int i=0; i<isize(by_level); i++) {
    displaystr(xstart + vid.fsize, vid.yres - vid.fsize * (i+2), 0, vid.fsize, its(by_level[i]), get_hipso(i+1), 0);
    }
  displaystr(xstart + vid.fsize, vid.yres - vid.fsize * 1, 0, vid.fsize, its(isize(level)), winf[waBarrier].color, 0);

  if(state != tsPreGame) {
    displaystr(xstart + vid.fsize, vid.yres - vid.fsize * 16, 0, vid.fsize, "LEVELS " + its(cur.completed), winf[waBarrier].color, 0);
    displaystr(xstart + vid.fsize, vid.yres - vid.fsize * 15, 0, vid.fsize, "BRICKS " + its(cur.bricks), winf[waBarrier].color, 0);
    displaystr(xstart + vid.fsize, vid.yres - vid.fsize * 14, 0, vid.fsize, "CUBES " + its(cur.cubes), winf[waBarrier].color, 0);
    if(cur.pro_game)
      displaystr(xstart + vid.fsize, vid.yres - vid.fsize * 13, 0, vid.fsize, "SCORE " + fts(int(cur.score)), winf[waBarrier].color, 0);
    }
  
  if(show_next) {
    displaystr(xstart + vid.fsize, vid.yres - vid.fsize * 27, 0, vid.fsize, "NEXT:", winf[waBarrier].color, 0);
    #if CAP_VR
    if(vrhr::active())
      vrhr::in_vr_ui([xstart] {
        display_next(xstart);
        });
    else
    #endif
      display_next(xstart);
    }
      
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
    if(displayButtonS(xx, vid.fsize * 4, "TRAINING", 0xFFFFFFFF, 8, vid.fsize)) getcstat = 't';
    if(displayButtonS(xx, vid.fsize * 6, "EXPERT GAME", 0xFFFFFFFF, 8, vid.fsize)) getcstat = 'x';
    if(displayButtonS(xx, vid.fsize * 8, "SETTINGS", 0xFFFFFFFF, 8, vid.fsize)) getcstat = 's';
    if(state != tsPreGame)
      if(displayButtonS(xx, vid.fsize * 10, "EXPLORE", 0xFFFFFFFF, 8, vid.fsize)) getcstat = 'e';
    if(displayButtonS(xx, vid.fsize * 12, "HI SCORES", 0xFFFFFFFF, 8, vid.fsize)) getcstat = 'h';
    if(!ISWEB) {
      if(displayButtonS(xx, vid.fsize * 14, "QUIT", 0xFFFFFFFF, 8, vid.fsize)) getcstat = 'q';
      }
    /* else if(state == tsGameover)
      if(displayButtonS(xx, vid.fsize * 14, "TWEET", 0xFFFFFFFF, 8, vid.fsize)) getcstat = 'T'; */
    
    if(vrhr::active())
      if(displayButtonS(xx, vid.fsize * 16, "RESET VR", 0xFFFFFFFF, 8, vid.fsize)) getcstat = 'V';
    }
  }
  
  keyhandler = [xstart, in_menu] (int sym, int uni) {
    if(explore) handlePanning(sym, uni);
    if(explore && sym == SDLK_BACKSPACE) 
      explore = false;

    auto& act = multi::action_states[1];

    if(state == tsFalling && !paused) {
      multi::handleInput(0, multi::scfg_default);
      bool consumed = false;
      for(int i=0; i<bmLast; i++)
        if(act[i].pressed()) {
          bringris_action(i);
          consumed = true;
          }
      if(consumed) return;
      }

    dialog::handleNavigation(sym, uni);
    if(in_menu && sym == 'q' && !ISWEB) {
      in_bringris = false;
      quitmainloop = true;
      }
    if(sym == '-') {
      if(!which_pointer) {
        int ax = mousex * 3 / xstart;
        if(ax > 3) ax = 3;
        int ay = mousey * 3 / vid.yres;
        if(ay > 2) ay = 2;
        int id = ay * 4 + ax;
        eBringrisMove moves[12] = {
          bmTurnLeft, bmUp, bmTurnRight, bmPause,
          bmLeft, bmDrop, bmRight, bmFullDrop,
          bmNothing, bmDown, bmNothing, bmFullDrop
          };
        eBringrisMove mov = moves[id];
        if((state == tsFalling && !paused) || mov == bmPause)  
          bringris_action(mov);
        }
#if CAP_VR
      else {
        if(explore)
          explore = !explore;
        else if(
          mousex >= vrhr::ui_xmin && mousex <= vrhr::ui_xmax && 
          mousey >= vrhr::ui_ymin && mousey <= vrhr::ui_ymax)
            paused = !paused;
        else {
          vrhr::compute_vr_direction(which_pointer);
          ld r = 1 / sqrt(2);        
          auto& dir = vrhr::vr_direction;
          vector<pair<eBringrisMove, hyperpoint> > choices = {
            {bmLeft, hyperpoint(-1, 0, 0, 0)},
            {bmRight, hyperpoint(1, 0, 0, 0)},
            {bmUp, hyperpoint(0, -1, 0, 0)},
            {bmDown, hyperpoint(0, 1, 0, 0)},
            {bmTurnLeft, hyperpoint(-r, -r, 0, 0)},
            {bmTurnRight, hyperpoint(r, -r, 0, 0)},
            {bmDrop, hyperpoint(-r, r, 0, 0)},
            {bmFullDrop, hyperpoint(r, r, 0, 0)}
            };
          
          eBringrisMove mov = bmNothing;
          ld best = 0;
                    
          for(auto& b: choices) {
            ld dot = (dir | b.second);
            if(dot > best) best = dot, mov = b.first;
            }

          bringris_action(mov);          
          }
        println(hlog, vrhr::vr_direction);
        }
#endif
      return;
      }
    
    // if(sym == 'k') ang = 0;
    // if(sym == 'l') ang = 45 * degree;    
    if(sym == 'p' || sym == 'c' || (sym == SDLK_ESCAPE && !ISWEB)) {
      if(!paused) move_at = move_at - ticks;
      paused = !paused;
      if(!paused) move_at = move_at - ticks;
      explore = false;
      }
    if(sym == 'T' && state == tsGameover) {
      const vector<const char*> emoji =
        {"😀","😎","👽","🤖","😺","🎩","🎓","👑","💍","🐯","🦁","🐮","🐷","🐽","🐸","🐙","🐵","🐦","🐧","🐔","🐒","🙉","🙈","🐣","🐥","🐺","🐗","🐴","🦄","🐝","🐛","🐢","🦀","🦂","🕷","🐜","🐞","🐌","🐠","🐟","🐡","🐬","🐋","🐊","🐆","🐘","🐫","🐪","🐄","🐂","🐃","🐏","🐑","🐀","🐁","🐓","🦃","🐉","🐾","🐿","🐇","🐈","🐩","🐕","🐲","🌵","🍁","🌻","🌎","⭐️","⚡️","🔥","❄️","☔️","☂️","💧","🍏","🍎","🍐","🍋","🍌","🍉","🍇","🌶","🍅","🍍","🍑","🍈","🍓","🌽","🍠","🍯","🍞","🍗","🧀","🍖","🍤","🌯","🌮","🍝","🍕","🌭","🍟","🍔","⚽️","🎱","🏆","🎪","🎲","🎳","🚗","🚕","🚙","🏎","⛺️","⛩","🕹","💾","☎️","⏱","🔦","💡","💰","💎","🔨","💣","🔑","❤️","🔔"};

      string out;
      if(cur.pro_game) {
        out = "Got " + its(cur.score) + " points for completing " + its(cur.completed) + " levels in #Bringris!";
        }
      else if(cur.completed) {
        out = "Used " + its(cur.bricks) + " blocks to complete " + its(cur.completed) + " levels in #Bringris!";
        }
      else {
        out = "Dropped " + its(cur.bricks) + " blocks in #Bringris!";
        }
      if(bgeom || cur.max_piece != 4) out += " (" + bgeoms[bgeom].name + "/" + its(cur.max_piece) + ")";
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
    if(sym == 'V') {
      reset_vr_ref();
      }
    if(in_menu && sym == 't') {
      start_new_game();
      paused = false;
      explore = false;
      cur.pro_game = false;
      playSound(cwt.at, "elementalgem");
      }
    if(in_menu && sym == 's') {
      pushScreen(settings_menu);
      }
    if(in_menu && sym == 'h') {
      pushScreen(hiscore_menu);
      }
    if(in_menu && sym == 'x') {
      start_new_game();
      paused = false;
      explore = false;
      cur.pro_game = true;
      playSound(cwt.at, "elementalgem");
      }
    #if CAP_SHOT
    if(sym == ']') {
      static int id = 0;
      perfect_linewidth = 0;
      shot::shot_aa = 2;
      vid.linewidth *= 2;
      shot::take(hr::format("bringris-%04d.png", id++), [] { draw_screen(vid.xres, false); });
      vid.linewidth /= 2;
      }
    #endif
    #ifdef MAKE_VIDEO
    if(sym == '[') {
      perfect_linewidth = 0;
      shot::shot_aa = 4;
      vid.linewidth *= 5;
      int dir = 3;
      int fpm = in_s2xe() ? 60 : 30;
      int steps = 0;
      auto att = at;
      bool boundedw = bflags() & BOUNDED_WELL;
      if(boundedw) steps = 20;
      else do {
        steps++;
        shift_block(dir);
        }
      while(at != att);
      println(hlog, steps, " steps of ", fpm, " frames each");
      anims::noframes = steps * fpm;
      anims::period = steps * fpm;
      int lt = -1;
      int p = addHook(anims::hooks_anim, 200, [&] {
        int t = ticks;
        if(t == lt) {
          println(hlog, "ignore");
          return;
          }
        lt = t;
        println(hlog, "t = ", t);
        if(t % fpm == 0) {
          pView = tView; 
          View = pView;
          smooth = Id;
          shift_block(dir);
          if(boundedw) dir = (1+dir) % 4;
          }
        if(boundedw) {
          auto f = [fpm] (ld k) { k /= fpm; return k * k * (3 - 2 * k); };
          ld a = f(t % fpm);
          ld b = f(t % fpm + 1);
          adjust_animation((b-a) / (1-a));
          }
        else
          adjust_animation(1. / (fpm - t % fpm));
        });
      anims::record_video("bringris.mp4", [] { return anims::record_animation_of( [] { println(hlog, "called"); draw_screen(vid.xres, false); } ); });
      delHook(anims::hooks_anim, p); 
      vid.linewidth /= 5;
      }
    #endif
    };

  callhooks(bringris_extensions);
  }

cell *get_center() {
  return well_center ? well_center : level[0];
  }

void reset_view() {
  centerover = get_at(get_center(), -camera_level);
  cwt.at = centerover;
  ncenter = get_at(get_center(), -camera_level);
  
  NLP = Id;
  tView = Id;
  
  set_view();
  pView = tView;
  }

void clear_map() {
  for(auto lev: level) for(int z=0; z<=camera_level+1; z++) {
    cell *c = get_at(lev, -z);
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

  for(auto lev: out_level) for(int z=1; z<=camera_level; z++) {
    cell *c = get_at(lev, -z);
    c->item = itNone;
    c->land = laCanvas;
    c->wall = waWaxWall;
    c->landparam = (get_hipso(z) & 0xFCFCFC) >> 2;
    }  
  }
  
void start_new_game() {

  timerstart = time(NULL);

  for(auto& p: piecelist) p.count = 0;

  clear_map();

  at = get_at(get_center(), -cur.well_size - 1);
  next_shape_id = choose_piece();
  
  state = tsBetween;
  
  reset_view();
  ray::reset_raycaster_map();

  // reset_view();

  cur.completed = 0;
  cur.bricks = 0;
  cur.cubes = 0;
  cur.score = 0;
  }

void get_level() {
  if(bflags() & BOUNDED_WELL) {
    set<cell*> all;
    well_center = currentmap->gamestart();
    all.insert(well_center);
    for(int i=0; i<4; i++)
    for(int l: {-1, 0, 1})
    for(int j=0; j<4; j++)
    for(int k: {-1, 1}) {
      cellwalker cw(well_center, i);
      cw += wstep;
      all.insert(cw.at);
      if(l) {
        cw += l;
        cw += wstep;
        all.insert(cw.at);
        }
      cw += j;
      cw += wstep;
      all.insert(cw.at);
      cw += k;
      cw += wstep;
      all.insert(cw.at);
      }
    set<cell*> all_ext;
    for(cell *c: all)
      forCellCM(d, c)
        if(!all.count(d))
          all_ext.insert(d);
          
    level.clear();
    for(auto c: all) 
      level.push_back(c);
    for(auto c: all_ext) 
      out_level.push_back(c);
    }
  else {
    level = currentmap->allcells();
    if(bflags() & ORBIFOLD) {
      vector<cell*> clist;
      set<cell*> visited;
      auto visit = [&] (cell *c) { 
        if(!visited.count(c))
          visited.insert(c),
          clist.push_back(c);
        };
      for(auto c: level) if(isNeighbor(c, c)) visit(c);
      for(int i=0; i<isize(clist); i++)
        for(int j=0; j<clist[i]->type; j++)
          visit(clist[i]->cmove(j));
      well_center = clist.back();
      }
    }
  if(well_center) {
    vector<cell*> visited;
    set<cell*> all;
    for(auto l: level) all.insert(l);
    auto visit = [&] (cell *c, int d) { 
      if(all.count(c) && !center_distance.count(c))
        center_distance[c] = d,
        visited.push_back(c);
      };
    visit(well_center, 0);
    for(int i=0; i<isize(visited); i++)
      for(int j=0; j<visited[i]->type; j++)
        visit(visited[i]->move(j), center_distance[visited[i]] + 1);
    }
  }
  
void create_game() {
  level.clear();
  out_level.clear();
  well_center = nullptr;

  if(!mproduct && !solnil) {
    println(hlog, "need product or Solnil geometry");
    exit(1);
    }
  if(nil) {
    for(int x=0; x<5; x++)
    for(int y=0; y<5; y++)
     level.push_back(nilv::get_heptagon_at(nilv::mvec(x, 0, y))->c7);
    }
  else if(sol) {
    level.clear();
    for(int x=0; x<5; x++)
    for(int y=0; y<5; y++)
     level.push_back(asonov::get_at(asonov::coord(x, y, 0))->c7);
    }
  else 
    PIU(get_level());  
  piecelist.clear();
  piecelist.reserve(2000);
  seen_blocks.clear();
  for(int ps=1; ps<=cur.max_piece; ps++)
    generate_shapes(ps);
  list_all();
  // println(hlog, "level size = ", isize(level));
  
  camera_level = cur.well_size + cur.max_piece + camera;
  
  playermoved = false;
  ray::max_iter_current() = solnil ? 600 : 200;

  start_new_game();
  state = tsPreGame;
  }

void init_all() {
  lps_enable(&lps_bringris);
  enable_bgeom();
  vid.texture_step = 8;
  showstartmenu = false;
  pushScreen(run);  
  in_bringris = true;
  load();
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

  else if(argis("-bringris0")) {
    PHASEFROM(3);
    start_game();
    create_game();
    }

  else if(argis("-bgeo")) {
    PHASEFROM(2);
    shift();
    enable_bgeom(argi());
    }
    
  else if(argis("-bringris")) {
    PHASEFROM(2);
    init_all();
    }

  else if(argis("-bringris-drop")) {
    PHASEFROM(2);
    start_new_game();
    paused = false;
    explore = false;
    cur.pro_game = false;
    new_piece(); fulldrop(); fallen(); new_piece(); fulldrop();
    }

  else if(argis("-bringris-explore")) {
    explore = true;
    }

  else if(argis("-ray-off"))
    use_raycaster = false;

  else if(argis("-ray-on"))
    use_raycaster = true;

  else if(argis("-dpl")) {
    shift(); draw_per_level = argi();
    }

  else return 1;
  return 0;
  }

void default_config() {
  multi::change_default_key(lps_bringris, SDL12('\r', SDL_SCANCODE_RETURN), 16 + 8);
  multi::change_default_key(lps_bringris, SDL12('q', SDL_SCANCODE_Q), 16 + 9);
  multi::change_default_key(lps_bringris, SDL12('e', SDL_SCANCODE_E), 16 + 10);
  multi::change_default_key(lps_bringris, SDL12(' ', SDL_SCANCODE_SPACE), 16 + 11);
  multi::change_default_key(lps_bringris, SDL12('p', SDL_SCANCODE_P), 16 + 12);

  param_i(bgeom, "bringris-geometry", 0);
  param_b(stars_enabled, "bringris_stars", true);
  lps_add(lps_bringris, ray::want_use, ray::want_use);
  #if CAP_VR
  lps_add(lps_bringris, vrhr::hsm, vrhr::hsm);
  lps_add(lps_bringris, vrhr::eyes, vrhr::eyes);
  #endif

  lps_add(lps_bringris, ray::exp_decay_poly, 200);
  lps_add(lps_bringris, ray::fixed_map, true);
  lps_add(lps_bringris, mapeditor::drawplayer, false);

  lps_add(lps_bringris, vid.fov, 90);
  lps_add(lps_bringris, vid.plevel_factor, 0.5);
  lps_add(lps_bringris, vid.axes3, false);

  lps_add(lps_bringris, vid.cells_drawn_limit);
  for(auto& g: sightranges) lps_add(lps_bringris, g);

  lps_add(lps_bringris_explore, mouseaim_sensitivity, 0.01);
  lps_add(lps_bringris_explore, camera_speed, 2);
  lps_add(lps_bringris_explore, smooth_scrolling, true);

  lps_add(lps_bringris_play, mouseaim_sensitivity, 0);

  param_b(use_raycaster, "bringris-ray");
  param_i(draw_per_level, "draw-per-level");
  param_b(use_equidistant, "bringris-equidistant");
  param_b(flashes, "bringris-flashes");
  }

auto hooks = 
    addHook(hooks_args, 100, args)
  + addHook(hooks_frame, 100, bringris_frame)
  + addHook(hooks_configfile, 300, default_config)
  + addHook(dialog::hooks_display_dialog, 100, [] () {
      if(dialog::items.size() && dialog::items[0].body == "Bringris keys") {
        dialog::addBreak(200);
        if(!rotate_allowed)
          dialog::addHelp("note: rotation keys only available when necessary");
        dialog::addHelp("press SHIFT to highlight the holes");
        dialog::addHelp("mouse control by pressing parts of the game screen");
        }
      }); 

#ifdef BRINGRIS
auto hook1=
    addHook(hooks_config, 100, [] {
      if(arg::curphase == 1) 
        conffile = "bringris.ini";
      if(arg::curphase == 2) init_all();      
      });
#endif

void fill_gamedata() {
  cur.bgeom_name = bgeoms[bgeom].name;
  time_t timer;
  timer = time(NULL);
  char buf[128];
  strftime(buf, 128, "%c", localtime(&timerstart)); cur.timerstart = buf;
  strftime(buf, 128, "%c", localtime(&timer)); cur.timerend = buf;
  cur.myname = cur.timerstart;
  cur.levelsize = isize(level);
  cur.seconds = int(timer - timerstart);
  cur.lmap.clear();
  for(int z=0; z<=cur.well_size; z++) {
    string s;
    for(auto lev: level) {
      cell *c = get_at(lev, -z);
      s += (c->wall ? '#' : '.');
      }
    cur.lmap.push_back(s);
    }
  }

void save(const gamedata& sd) {
  #if CAP_SAVE
  fhstream f("bringris.save", "at");
  println(f, "Bringris ", BRINGRIS_VER);
  println(f, sd.bgeom_name);
  println(f, "unnamed");
  println(f, sd.timerstart);
  println(f, sd.timerend);
  println(f, sd.max_piece, " ", sd.pro_game ? sd.score : -1, " ", sd.bricks, " ", sd.completed, " ", sd.cubes, " ", sd.well_size, " ", sd.levelsize, " ", sd.seconds);
  for(auto& s: sd.lmap) println(f, s.c_str());
  #endif
  }

void save() {
  fill_gamedata();
  save(cur);
  allsaves.push_back(cur);
  }

void load() {
  allsaves.clear();
  fhstream f("bringris.save", "rt");
  if(!f.f) return;
  string s;
  while(!feof(f.f)) {
    s = scanline_noblank(f);
    if(s == "Bringris 2.0") {
      gamedata gd;
      gd.bgeom_name = scanline_noblank(f);
      gd.myname = scanline_noblank(f);
      gd.timerstart = scanline_noblank(f);
      gd.timerend = scanline_noblank(f);
      sscanf(scanline_noblank(f).c_str(), "%d%lf%d%d%d%d%d%d",
        &gd.max_piece, &gd.score, &gd.bricks, &gd.completed, &gd.cubes, &gd.well_size, &gd.levelsize, &gd.seconds);
      gd.pro_game = gd.score >= 0;
      for(int i=0; i<=gd.well_size; i++) gd.lmap.push_back(scanline_noblank(f));
      allsaves.push_back(gd);
      for(auto& g: bgeoms) if(g.name == gd.bgeom_name && g.default_max_piece == gd.max_piece)
        g.stars = max(g.stars, (gd.pro_game ? 5 : 1) * gd.completed * gd.levelsize);
      }
    }
  }

}
}
