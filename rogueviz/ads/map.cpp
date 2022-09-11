namespace hr {

namespace ads_game {

struct rockinfo {
  int type;
  ads_matrix at;
  color_t col;
  
  ld life_start, life_end;
  
  rockinfo(int t, const ads_matrix& T, color_t _col) : type(t), at(T), col(_col) { 
    life_start = -HUGE_VAL;
    life_end = HUGE_VAL;
    }
  };

enum eWalltype { wtNone, wtDestructible, wtSolid, wtGate };

struct cellinfo {
  int mpd_terrain; /* 0 = fully generated terrain */
  int rock_dist; /* rocks generated in this radius */
  vector<rockinfo> rocks;
  eWalltype type;
  cellinfo() {
    mpd_terrain = 4;
    rock_dist = -1;
    type = wtNone;
    }
  };

std::unordered_map<cell*, cellinfo> ci_at;

using worldline_visitor = std::function<bool(cell*, ld)>;

void compute_life(cell *c, transmatrix S1, const worldline_visitor& wv) {
  ld t = 0;

  int iter = 0;
  cell *cur_c = c;
  auto cur_w = hybrid::get_where(c);
  while(t < 2 * M_PI) {
    iter++;
    auto last_w = cur_w;
    auto next_w = cur_w;
    transmatrix next_S1;
    ld next_t;
    ld last_time = t;
    cell *next_c = nullptr;
    binsearch(t, t+M_PI/2, [&] (ld t1) { 
      S1 = S1 * chg_shift(t1 - last_time);
      last_time = t1;
      virtualRebase(cur_c, S1);
      cur_w = hybrid::get_where(cur_c);
      if(cur_w.first != last_w.first) {
        next_c = cur_c;
        next_w = cur_w;
        next_S1 = S1;
        next_t = t1;
        return true;
        }
      return false;
      }, 20);
    if(!next_c) return;
    S1 = next_S1;
    cur_w = next_w;
    t = next_t;
    cur_c = next_c;
    if(iter > 1000) {
      println(hlog, "compute_life c=", cur_c, " w=", cur_w, "t=", t, " S1=", S1);
      fixmatrix_ads(S1);
      }
    if(iter > 1100) break;
    if(wv(cur_w.first, t)) break;
    }
  }

map<int, int> genstats;

int gen_budget;

void gen_terrain(cell *c, cellinfo& ci, int level = 0) {
  if(level >= ci.mpd_terrain) return;
  if(ci.mpd_terrain > level + 1) gen_terrain(c, ci, level+1);
  forCellCM(c1, c) gen_terrain(c1, ci_at[c1], level+1);
  genstats[level]++;
  
  if(level == 2) {
    int r = hrand(100);
    if(r < 5) {
      forCellCM(c1, c) if(hrand(100) < 50)
        forCellCM(c2, c1)  if(hrand(100) < 50)
          if(ci_at[c2].type == wtNone) ci_at[c2].type = wtDestructible;
      }
    else if(r < 10) {
      forCellCM(c1, c) if(hrand(100) < 50)
        forCellCM(c2, c1)  if(hrand(100) < 50)
          if(ci_at[c2].type < wtSolid)
            ci_at[c2].type = wtSolid;
      }
    else if(r < 12)
      ci_at[c].type = wtGate;
    }
  ci.mpd_terrain = level;
  }

void gen_rocks(cell *c, cellinfo& ci, int radius) {
  if(radius <= ci.rock_dist) return;
  if(ci.rock_dist < radius - 1) gen_rocks(c, ci, radius-1);
  forCellCM(c1, c) gen_rocks(c1, ci_at[c1], radius-1);
  if(geometry != gNormal) { println(hlog, "wrong geometry detected in gen_rocks 1!");  exit(1); }

  if(radius == 0) {
    hybrid::in_actual([&] {
      int q = rpoisson(.05);
      
      auto add_rock = [&] (rockinfo&& r) {
        if(geometry != gRotSpace) { println(hlog, "wrong geometry detected in gen_rocks 2!");  exit(1); }
        compute_life(hybrid::get_at(c, 0), unshift(r.at), [&] (cell *c, ld t) {
          auto& ci = ci_at[c];
          hybrid::in_underlying_geometry([&] { gen_terrain(c, ci); });
          ci.type = wtNone;
          return false;
          });
        ci.rocks.emplace_back(r);
        };
      
      for(int i=0; i<q; i++) {
        int kind = hrand(100);
        if(kind < 50) 
          add_rock(rockinfo(0, ads_matrix(rots::uxpush(randd() * .6 - .3) * rots::uypush(randd() * .6 - .3)), 0xC0C0C0FF));
        else
          add_rock(rockinfo(0, ads_matrix(rots::uypush(randd() * .6 - .3) * lorentz(0, 3, 0.5 + randd() * 1)), 0xC04040FF));
        }        
      });
    }
  ci.rock_dist = radius;
  }


}}
