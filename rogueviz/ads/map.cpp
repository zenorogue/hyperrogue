namespace hr {

namespace ads_game {

expiry_data gen_expire(cell *c) {
  expiry_data ed;
  ed.score = 20 / randd() - 15;
  ed.score_id = treasure_id(treasure_of(c));
  return ed;
  }

bool expired(const expiry_data& ed, const player_data& pdata) {
  return pdata.score[ed.score_id] > ed.score;
  }

vector<shipstate> history;

std::unordered_map<cell*, cellinfo> ci_at;

using worldline_visitor = std::function<bool(cell*, ld)>;

void compute_life(cell *c, transmatrix S1, const worldline_visitor& wv) {
  ld t = 0;

  int iter = 0;
  cell *cur_c = c;
  auto cur_w = hybrid::get_where(c);
  while(t < TAU) {
    iter++;
    auto last_w = cur_w;
    auto next_w = cur_w;
    transmatrix next_S1;
    ld next_t;
    ld last_time = t;
    cell *next_c = nullptr;
    binsearch(t, t+90._deg, [&] (ld t1) {
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
  if(level == 0) setdist(c, 7, nullptr);
  if(level >= ci.mpd_terrain) return;
  if(!hyperbolic) { println(hlog, "wrong geometry detected in gen_terrain!");  exit(1); }

  if(c->land == laCaves) {
    if(ci.mpd_terrain > level + 1) gen_terrain(c, ci, level+1);
    forCellCM(c1, c) gen_terrain(c1, ci_at[c1], level+1);
    }
  genstats[level]++;
  
  if(level == 2) {
    int r = hrand(100);
    if(r < wall_frequency(c)) {
      int t = hrand(2);
      if(t == 0)
        forCellCM(c1, c) if(hrand(100) < 50) if(c1->land == c->land)
          forCellCM(c2, c1)  if(hrand(100) < 50) if(c2->land == c->land)
            if(ci_at[c2].type == wtNone) ci_at[c2].type = wtDestructible;
      if(t == 1)
        forCellCM(c1, c) if(hrand(100) < 50) if(c1->land == c->land)
          forCellCM(c2, c1)  if(hrand(100) < 50) if(c1->land == c->land)
            if(ci_at[c2].type < wtSolid)
              ci_at[c2].type = wtSolid;
      }
    r = hrand(100);
    if(r < gate_frequency(c))
      ci_at[c].type = wtGate;
    }
  ci.mpd_terrain = level;

  if(c->land == laBarrier)
    ci_at[c].type = wtBarrier;
  }

void add_rock(cell *c, cellinfo& ci, const ads_matrix& T) {

  bool fail = false;
  compute_life(hybrid::get_at(c, 0), unshift(T), [&] (cell *c, ld t) {
    hybrid::in_underlying_geometry([c] { setdist(c, 7, nullptr); });
    if(c->land == laBarrier) fail = true;
    return false;
    });
  if(fail) return;

  eResourceType rt = eResourceType(rand() % 6);
  auto r = std::make_unique<ads_object> (oRock, c, T, rock_color[rt]);
  r->resource = rt;
  r->expire = gen_expire(c);
  r->shape = &(rand() % 2 ? shape_rock2 : shape_rock);
  if(geometry != gTwistedProduct) { println(hlog, "wrong geometry detected in gen_rocks 2!");  exit(1); }
  int q = 0;

  auto cleanup = [&] (cell *c, ld t) {
    auto& ci = ci_at[c];
    hybrid::in_underlying_geometry([&] { gen_terrain(c, ci); });
    ci.type = wtNone;
    q++;
    return false;
    };

  if(q == 0) ci.type = wtNone;
  compute_life(hybrid::get_at(c, 0), unshift(T), cleanup);

  /* for(int i=0; i<isize(r->shape[0]); i += 2) { // exact check is too slow here
    hyperpoint h;
    h[0] = r->shape[0][i];
    h[1] = r->shape[0][i+1];
    h[2] = 0;
    h[3] = 1; */
  if(0) for(int i=0; i<4; i++) {
    hyperpoint h = spin(90*degree*i) * twist::uxpush(0.15) * C0;
    compute_life(hybrid::get_at(c, 0), unshift(r->at) * rgpushxto0(h), cleanup);
    }
  ci.rocks.emplace_back(std::move(r));
  }

void add_turret(cell *c, cellinfo& ci, const ads_matrix& T) {
  auto r = std::make_unique<ads_object> (oTurret, c, T, 0xC0C060FF);
  r->expire = gen_expire(c);
  r->shape = &shape_turret;
  r->last_shot = -1;
  r->hlast = 0;
  if(geometry != gTwistedProduct) { println(hlog, "wrong geometry detected in gen_turret!");  exit(1); }
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

  ci.rocks.emplace_back(std::move(r));
  }

void gen_resource(cell *c, shiftmatrix from, eResourceType rsrc, const expiry_data& expire);

void add_rsrc(cell *c, cellinfo& ci, const ads_matrix& T) {
  eResourceType rt = eResourceType(rand() % 6);
  if(rt == rtGoldRocks && c->land == laJungle) rt = rtGoldGate;
  if(rt == rtGoldRocks && c->land == laHunting) rt = rtGoldTurret;
  gen_resource(c, T, rt, gen_expire(c));
  }

int turrets;

struct placement {
  ld alpha;
  ld r;
  ld shift;
  ld spinshift;
  ld rapidity;
  ads_matrix get(const ads_matrix& M = Id) {
    return ads_matrix(Id) * spin(alpha) * twist::uxpush(r/2) * M * chg_shift(shift) * spin(spinshift) * lorentz(0, 3, rapidity);
    };
  };

/* if maxr equals cgi.rhexf, any point inside the cell equally likely */
placement get_placement(cell *c, ld maxr, ld max_rapidity) {
  cell *c1 = nullptr;
  placement p;
  while(c1 != c) {
    ld vol = randd() * wvolarea_auto(maxr);
    p.r = binsearch(0, maxr, [vol] (ld r) { return wvolarea_auto(r) > vol; });
    p.alpha = randd() * TAU;
    hyperpoint h = spin(p.alpha) * xpush0(p.r);
    c1 = c;
    virtualRebase(c1, h);
    }
  p.shift = randd() * TAU;
  p.spinshift = randd() * TAU;
  p.rapidity = randd() * max_rapidity;
  return p;
  }

void gen_rocks(cell *c, cellinfo& ci, int radius) {
  if(radius <= ci.rock_dist) return;
  if(ci.rock_dist < radius - 1) gen_rocks(c, ci, radius-1);
  forCellCM(c1, c) gen_rocks(c1, ci_at[c1], radius-1);
  if(!hyperbolic) { println(hlog, "wrong geometry detected in gen_rocks 1!");  exit(1); }

  if(radius == 0) {
    int q = rpoisson(rock_density * rock_frequency(c));
    for(int i=0; i<q; i++) {
      auto p = get_placement(c, cgi.rhexf, rock_max_rapidity);
      hybrid::in_actual([&] {
        add_rock(c, ci, p.get());
        });
      }

    if(ci.type == wtGate && hrand(100) < 20) {
      auto p = get_placement(c, cgi.rhexf / 2, rock_max_rapidity / 100);
      hybrid::in_actual([&] {
        add_rsrc(c, ci, p.get());
        });
      }

    q = rpoisson(rock_density * turret_frequency(c));
    // if(celldist(c) == 2) q += rpoisson(0.1);
    for(int i=0; i<q; i++) {
      auto p = get_placement(c, cgi.rhexf, rock_max_rapidity / 10);
      hybrid::in_actual([&] {
        add_turret(c, ci, p.get());
        for(int r=0; r<6; r++)
          add_rsrc(c, ci, p.get(spin(r * 60._deg) * twist::uxpush(turret_dist * ads_scale)));
        turrets++;
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

void gen_resource(cell *c, shiftmatrix from, eResourceType rsrc, const expiry_data& expire) {
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

// -1 : T1 is in the past of T2
// =0 : T1 is elsewhere from T2
// +1 : T1 is in the future of T2

hyperpoint hcopy;

int spacetime_relation(const ads_matrix& T1, const ads_matrix& T2) {
  auto h = ads_inverse(T1) * (T2 * C0);
  if(h.shift > 90._deg) return 1;
  if(h.shift < -90._deg) return -1;
  auto h1 = unshift(h);
  hcopy = h1;
  if(h1[0] * h1[0] + h1[1] * h1[1] > h1[2] * h1[2]) return 0;
  return h1[2] > 0 ? 1 : -1;
  }

bool bad_turret = false;

void handle_turret(ads_object *t, ld& angle_at_time) {
  ld ctime = t->pt_main.shift;

  if(t->last_shot >= t->life_end) return;

  auto p = at_or_null(cds_last, t->owner);
  if(!p) return;
  auto t1 = p->V * t->at * ads_matrix(Id, ctime);

  auto& ts = t->turret_states;
  auto it1 = ts.lower_bound(ctime);

  if(it1->first == ctime) {
    if(it1->second.err < 0.01) angle_at_time = it1->second.angle;
    return;
    }

  auto it0 = it1; if(it0 != ts.begin()) it0--;

  int tv0 = (it1 == ts.begin()) ? 0 : it0->second.index;
  int tv1 = (it1 == ts.end()) ? isize(history) : it1->second.index;

  while(tv0 < tv1) {
    int tvm = (tv0 + tv1) / 2;
    auto& hi = history[tvm];
    auto p1 = at_or_null(cds_last, hi.vctr);
    if(!p1) { tv0 = tvm+1; continue; }
    ads_matrix at1 = p1->V * hi.at;
    auto rel = spacetime_relation(t1, at1);
    if(rel == -1) tv0 = tvm+1;
    else tv1 = tvm;
    }

  // println(hlog, "tv0 search returns ", tv0, "/", isize(history), " for ctime = ", ctime);
  if(tv0 == 0 || tv0 == isize(history)) { return; }

  auto& hi = history[tv0];
  auto p1 = at_or_null(cds_last, hi.vctr);
  if(!p1) return;
  ads_matrix at1 = p1->V * hi.at;

  turret_state nts;
  if(bad_turret) {
    auto h = ads_inverse(t1) * (at1 * C0);
    auto h1 = unshift(h);
    nts.angle = -atan2(h1[1], h1[0]);
    nts.dist = acosh(h1[3]) - turret_length;
    nts.err = 0;
    }
  else {

    auto hitpoint = [&] (ld alph, ld dist) {
      return ads_inverse(at1) * t1 * spin(alph) * twist::uxpush(turret_length) * lorentz(0, 2, ads_missile_rapidity) * ads_point(C0, dist);
      };

    auto opt_hitpoint = [&] (ld alph, ld dist) { return unshift(hitpoint(alph, dist)); };

    if(it1 == ts.begin() || it0->first < ctime - 0.1 || it0->second.err > 0.01) {
      ld best_err = HUGE_VAL;
      for(int av=0; av<24; av++) for(ld dist=0.01; dist < 2; dist += 0.01) {
        ld alph = av * TAU / 24;
        ld err = sqhypot_d(2, hitpoint(alph, dist).h);
        if(err < best_err) { best_err = err; nts.angle = alph; nts.dist = dist; }
        }
      // println(hlog, "the closest hit at alpha = ", nts.angle, " and dist = ", nts.dist, " (err = ", best_err, ")");
      }
    else {
      nts.angle = it0->second.angle;
      nts.dist = it0->second.dist;
      }

    /* Newton method */
    for(int it=0; it<3; it++) {
      ld eps = 1e-4;
      hyperpoint h0 = opt_hitpoint(nts.angle, nts.dist);
      hyperpoint hx = opt_hitpoint(nts.angle + eps, nts.dist);
      hyperpoint hy = opt_hitpoint(nts.angle, nts.dist + eps);
      // println(hlog, tie(nts.angle, nts.dist), " : ", h0);
      transmatrix T = Id;
      set_column(T, 0, hx-h0);
      set_column(T, 1, hy-h0);
      transmatrix T2 = inverse2(T);
      // f(x) = h0 + T * (x-x0) / eps = 0
      // -h0 * eps = T * (x-x0)
      // T2 * (-h0 * eps) = x - x0
      hyperpoint x = T2 * (-h0 * eps);
      nts.angle += x[0];
      nts.dist += x[1];
      }

    nts.err = sqhypot_d(2, hitpoint(nts.angle, nts.dist).h);
    }

  // println(hlog, "nts values are: ", tie(nts.angle, nts.dist, nts.err));

  if(nts.err < 0.01 && ctime > t->last_shot + 1 && it0->second.err < 0.01) {
    t->last_shot = t->last_shot + floor(ctime - t->last_shot);
    if(t->last_shot >= t->life_end) return;
    ld angle = lerp(it0->second.angle, nts.angle, ilerp(it0->first, ctime, t->last_shot));
    // println(hlog, "shooting at angle ", angle, " at time ", t->last_shot);
    ads_matrix S0 = ads_inverse(p->V) * t1 * spin(angle) * twist::uxpush(turret_length * ads_scale) * lorentz(0, 2, ads_missile_rapidity);
    auto r = std::make_unique<ads_object> (oTurretMissile, t->owner, S0, rsrc_color[rtAmmo]);
    r->shape = &shape_missile;
    r->life_start = 0; r->life_end = M_PI;
    ci_at[t->owner].rocks.emplace_back(std::move(r));
    // println(hlog, "OK");
    }

  nts.index = tv0;
  t->turret_states[ctime] = nts;
  if(nts.err < 0.01) angle_at_time = nts.angle;
  }

void handle_crashes() {
  if(paused) return;
  if(mtwisted) {
    if(!currentmap) { println(hlog, "no currentmap!"); return; }
    PIU({ handle_crashes(); });
    return;
    }
  vector<ads_object*> missiles;
  vector<ads_object*> rocks;
  vector<ads_object*> resources;
  vector<ads_object*> turrets;
  vector<ads_object*> enemy_missiles;
  for(auto m: displayed) {
    if(m->type == oMissile)
      missiles.push_back(m);
    if(m->type == oTurretMissile)
      enemy_missiles.push_back(m);
    if(m->type == oRock || m->type == oTurret)
      rocks.push_back(m);
    if(m->type == oTurret)
      turrets.push_back(m);
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
            if(r->type != oTurret) gen_resource(r->owner, r->at * ads_matrix(Id, r->life_end), r->resource, r->expire);
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
      for(auto r: enemy_missiles) {
        if(pointcrash(h, r->pts)) {
          r->life_end = r->pt_main.shift;
          ads_crash_ship();
          }
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
        h1[3] = h1[2]; h1[2] = 0;
        ads_point rel = ads_inverse(current * vctrV) * ads_point(h1, 0);
        cell *c = hybrid::get_at(vctr, 0);
        virtualRebase(c, rel.h);
        optimize_shift(rel);
        auto w = hybrid::get_where(c);
        auto& ci = ci_at[w.first];
        ld t = rel.shift + w.second * cgi.plevel;
        if(ci.type == wtDestructible || ci.type == wtSolid || (ci.type == wtGate && (int(floor(t)) & 3) == 0) || ci.type == wtBarrier) {
          if(!crashed && ship_pt > invincibility_pt) println(hlog, "crashed at t = ", t / TAU, " shift = ", rel.shift/TAU, " sec = ", w.second*cgi.plevel/TAU);
          crashed = true;
          }
        });

      if(crashed) ads_crash_ship();
      }
    });
  }

}}
