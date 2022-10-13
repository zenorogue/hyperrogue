namespace hr {

namespace ads_game {

enum eObjType { oRock, oMissile, oParticle, oResource, oMainRock };

PPR obj_prio[5] = { PPR::MONSTER_BODY, PPR::ITEMa, PPR::ITEM_BELOW, PPR::ITEM, PPR::MONSTER_HEAD };

struct ads_object {
  eObjType type;
  eResourceType resource;
  cell *owner;
  ads_matrix at;
  color_t col;
  int expire;
  vector<ld>* shape;
  
  ld life_start, life_end;
  cross_result pt_main;
  vector<cross_result> pts;
  
  ads_object(eObjType t, cell *_owner, const ads_matrix& T, color_t _col) : type(t), owner(_owner), at(T), col(_col) { 
    life_start = -HUGE_VAL;
    life_end = HUGE_VAL;
    }
  };

enum eWalltype { wtNone, wtDestructible, wtSolid, wtGate };

int gen_expire() {
  return 20 / randd() - 15;
  }

struct shipstate {
  ads_matrix at;
  ads_matrix current;
  ld start;
  ld duration;
  ld ang;
  ads_matrix vctrV;
  cell *vctr;
  };

vector<shipstate> history;

struct cellinfo {
  int mpd_terrain; /* 0 = fully generated terrain */
  int rock_dist; /* rocks generated in this radius */
  vector<std::unique_ptr<ads_object>> rocks;
  vector<shipstate> shipstates;
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
      }, 10);
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
  if(!hyperbolic) { println(hlog, "wrong geometry detected in gen_terrain!");  exit(1); }
  if(ci.mpd_terrain > level + 1) gen_terrain(c, ci, level+1);
  forCellCM(c1, c) gen_terrain(c1, ci_at[c1], level+1);
  genstats[level]++;
  
  if(level == 2) {
    int r = hrand(100);
    if(r < 3) {
      forCellCM(c1, c) if(hrand(100) < 50)
        forCellCM(c2, c1)  if(hrand(100) < 50)
          if(ci_at[c2].type == wtNone) ci_at[c2].type = wtDestructible;
      }
    else if(r < 6) {
      forCellCM(c1, c) if(hrand(100) < 50)
        forCellCM(c2, c1)  if(hrand(100) < 50)
          if(ci_at[c2].type < wtSolid)
            ci_at[c2].type = wtSolid;
      }
    else if(r < 8)
      ci_at[c].type = wtGate;
    }
  ci.mpd_terrain = level;
  }

void add_rock(cell *c, cellinfo& ci, const ads_matrix& T) {
  eResourceType rt = eResourceType(rand() % 6);
  auto r = std::make_unique<ads_object> (oRock, c, T, rock_color[rt]);
  r->resource = rt;
  r->expire = gen_expire();
  r->shape = &(rand() % 2 ? shape_rock2 : shape_rock);
  if(geometry != gRotSpace) { println(hlog, "wrong geometry detected in gen_rocks 2!");  exit(1); }
  int q = 0;

  auto cleanup = [&] (cell *c, ld t) {
    auto& ci = ci_at[c];
    hybrid::in_underlying_geometry([&] { gen_terrain(c, ci); });
    ci.type = wtNone;
    q++;
    return false;
    };

  if(q == 0) ci.type = wtNone;
  compute_life(hybrid::get_at(c, 0), unshift(r->at), cleanup);

  /* for(int i=0; i<isize(r->shape[0]); i += 2) { // exact check is too slow here
    hyperpoint h;
    h[0] = r->shape[0][i];
    h[1] = r->shape[0][i+1];
    h[2] = 0;
    h[3] = 1; */
  if(0) for(int i=0; i<4; i++) {
    hyperpoint h = spin(90*degree*i) * rots::uxpush(0.15) * C0;
    compute_life(hybrid::get_at(c, 0), unshift(r->at) * rgpushxto0(h), cleanup);
    }
  ci.rocks.emplace_back(std::move(r));
  }

void gen_rocks(cell *c, cellinfo& ci, int radius) {
  if(radius <= ci.rock_dist) return;
  if(ci.rock_dist < radius - 1) gen_rocks(c, ci, radius-1);
  forCellCM(c1, c) gen_rocks(c1, ci_at[c1], radius-1);
  if(!hyperbolic) { println(hlog, "wrong geometry detected in gen_rocks 1!");  exit(1); }

  if(radius == 0) {
    int q = rpoisson(rock_density);
    for(int i=0; i<q; i++) {

      /* any point inside the cell equally likely */
      ld maxr = cgi.rhexf;
      cell *c1 = nullptr;
      ld r, alpha;
      while(c1 != c) {
        ld vol = randd() * wvolarea_auto(maxr);
        r = binsearch(0, maxr, [vol] (ld r) { return wvolarea_auto(r) > vol; });
        alpha = randd() * TAU;
        hyperpoint h = spin(alpha) * xpush0(r);
        c1 = c;
        virtualRebase(c1, h);
        }

      hybrid::in_actual([&] {
        add_rock(c, ci, ads_matrix(spin(alpha) * rots::uxpush(r/2) * chg_shift(randd() * TAU) * spin(randd() * TAU) * lorentz(0, 3, randd() * rock_max_rapidity)));
        });
      }
    }
  ci.rock_dist = radius;
  }

void gen_particles(int qty, cell *c, shiftmatrix from, color_t col, ld spd, ld t, ld spread = 1) {
  auto& ro = ci_at[c].rocks;
  for(int i=0; i<qty; i++) {
    auto r = std::make_unique<ads_object>(oParticle, c, from * spin(randd() * TAU * spread) * lorentz(0, 2, (.5 + randd() * .5) * spd), col );
    r->shape = &shape_particle;
    r->life_end = randd() * t;
    r->life_start = 0;
    ro.emplace_back(std::move(r));
    }
  }

void gen_resource(cell *c, shiftmatrix from, eResourceType rsrc, int expire) {
  if(!rsrc) return;
  auto r = std::make_unique<ads_object>(oResource, c, from, rsrc_color[rsrc]);
  r->shape = rsrc_shape[rsrc];
  r->life_end = HUGE_VAL;
  r->life_start = 0;
  r->resource = rsrc;
  r->expire = expire;
  ci_at[c].rocks.emplace_back(std::move(r));
  }

bool pointcrash(hyperpoint h, const vector<cross_result>& vf) {
  int winding = 0;
  vector<hyperpoint> kleins;
  for(auto& p: vf) kleins.push_back(kleinize(p.h) - h);
  auto take = [&] (hyperpoint& a, hyperpoint& b) {
    if(asign(a[1], b[1]) && xcross(b[0], b[1], a[0], a[1]) < 1e-6)
      winding++;
    };
  for(int i=1; i<isize(kleins); i++) take(kleins[i-1], kleins[i]);
  take(kleins.back(), kleins[0]);
  return winding & 1;
  }

void common_crash_ship() {
  invincibility_pt = ship_pt + DS_(how_much_invincibility);
  pdata.hitpoints--;
  if(pdata.hitpoints <= 0) game_over = true;
  playSound(nullptr, "explosion");
  }
  
void ads_crash_ship() {
  if(ship_pt < invincibility_pt) return;
  common_crash_ship();
  hybrid::in_actual([&] {
    gen_particles(rpoisson(crash_particle_qty * 2), vctr, ads_inverse(current * vctrV) * spin(ang*degree), rsrc_color[rtHull], crash_particle_rapidity, crash_particle_life);
    });
  }

void handle_crashes() {
  if(paused) return;
  if(rotspace) {
    if(!currentmap) { println(hlog, "no currentmap!"); return; }
    PIU({ handle_crashes(); });
    return;
    }
  vector<ads_object*> missiles;
  vector<ads_object*> rocks;
  vector<ads_object*> resources;
  for(auto m: displayed) {
    if(m->type == oMissile)
      missiles.push_back(m);
    if(m->type == oRock)
      rocks.push_back(m);
    if(m->type == oResource)
      resources.push_back(m);
    }
  hybrid::in_underlying_geometry([&] {
    for(auto m: missiles) {
      hyperpoint h = kleinize(m->pt_main.h);
      for(auto r: rocks) {
        if(pointcrash(h, r->pts)) {
          m->life_end = m->pt_main.shift;
          r->life_end = r->pt_main.shift;
          hybrid::in_actual([&] {
            gen_particles(rpoisson(crash_particle_qty), m->owner, m->at * ads_matrix(Id, m->life_end), missile_color, crash_particle_rapidity, crash_particle_life);
            gen_particles(rpoisson(crash_particle_qty), r->owner, r->at * ads_matrix(Id, r->life_end), r->col, crash_particle_rapidity, crash_particle_life);
            gen_resource(r->owner, r->at * ads_matrix(Id, r->life_end), r->resource, r->expire);
            playSound(nullptr, "hit-crush3");
            });
          }
        }
      }
    if(!game_over) for(int i=0; i<isize(shape_ship); i+=2) {
      hyperpoint h = spin(ang*degree) * hpxyz(shape_ship[i] * ads_scale, shape_ship[i+1] * ads_scale, 1);
      for(auto r: rocks) {
        if(pointcrash(h, r->pts)) ads_crash_ship();
        }
      for(auto r: resources) {
        if(pointcrash(h, r->pts)) {
          r->life_end = r->pt_main.shift;
          gain_resource(r->resource);
          }
        }

      hyperpoint h1 = normalize(h);
      bool crashed = false;
      hybrid::in_actual([&] {
        swap(h1[2], h1[3]);
        ads_point rel = ads_inverse(current * vctrV) * ads_point(h1, 0);
        cell *c = hybrid::get_at(vctr, 0);
        virtualRebase(c, rel.h);
        optimize_shift(rel);
        auto w = hybrid::get_where(c);
        auto& ci = ci_at[w.first];
        ld t = rel.shift + w.second * cgi.plevel;
        if(ci.type == wtDestructible || ci.type == wtSolid || (ci.type == wtGate && (int(floor(t)) & 3) == 0)) {
          if(!crashed && ship_pt > invincibility_pt) println(hlog, "crashed at t = ", t / TAU, " shift = ", rel.shift/TAU, " sec = ", w.second*cgi.plevel/TAU);
          crashed = true;
          }
        });

      if(crashed) ads_crash_ship();
      }
    });
  }

}}
