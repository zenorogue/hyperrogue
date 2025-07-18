namespace hr {

namespace ads_game {

color_t shipcolor = 0x2020FFFF;

cross_result findflat(shiftpoint h) {
  return cross0(current * rgpushxto0(h));
  }

void apply_duality(shiftmatrix& S) {
  if(use_duality == 1) {
    S.T = unshift(S);
    S.shift = 0; // get_shift_cycles(S.shift);
    S.T = Duality * S.T * Duality;
    };
  if(use_duality == 2) {
    S = ads_matrix(Id, -90._deg) * S * ads_matrix(Id, +90._deg);
    S.T = spin(90._deg) * S.T;
    }
  }

vector<ads_object*> under_mouse;
hyperpoint mousetester;

void view_time(const shiftmatrix& S, ld t, color_t col) {
  if(!view_proper_times) return;
  auto S1 = S;
  if(time_shift) S1.T = rgpushxto0(S1.T * C0) * ypush(time_shift);
  string str = hr::format(tformat, t / (main_rock ? ds_time_unit : ads_time_unit));
  queuestr(S1, time_scale, str, col, 8);
  }

void draw_game_cell(const cell_to_draw& cd) {
  bool hv = mtwisted;
  using cellptr = cell*;
  const cellptr& c = cd.c;
  const ads_matrix& V = cd.V;

  vector<cross_result> hlist;

  hybrid::in_actual([&]{    
    for(int i=0; i<=c->type; i++) {
      hyperpoint ha = hybrid::get_corner(c, i, 2, 0);
      hlist.push_back(findflat(V * ha));
      }
    });
  
  auto& ci = ci_at[c];
  if(ci.mpd_terrain > 0) {
    if(!gen_budget) return;
    gen_budget--;
    }
  PIU({
    gen_terrain(c, ci);
    gen_rocks(c, ci, 0);
    });

  auto& t = ci.type;

  color_t col = 0;

  if(hv) ;
  else if(t == wtGate) {
    ld minv = hlist[0].shift;
    ld maxv = hlist[0].shift;
    for(auto& h: hlist) {
      ld v = h.shift;
      if(v < minv) minv = v;
      if(v > maxv) maxv = v;
      }
  
    auto draw_slice = [&] (ld a, ld b, color_t col) {
      vector<hyperpoint> slice;
      for(int i=0; i<c->type; i++) {
        if(hlist[i].shift >= a && hlist[i].shift <= b)
          slice.push_back(hlist[i].h);
        if((hlist[i].shift < a) ^ (hlist[i+1].shift < a)) {
          ld p = ilerp(hlist[i].shift, hlist[i+1].shift, a);
          hyperpoint h1 = lerp(hlist[i].h, hlist[i+1].h, p);
          slice.push_back(h1);
          }
        if((hlist[i].shift > b) ^ (hlist[i+1].shift > b)) {
          ld p = ilerp(hlist[i].shift, hlist[i+1].shift, b);
          hyperpoint h1 = lerp(hlist[i].h, hlist[i+1].h, p);
          slice.push_back(h1);
          }
        if(hlist[i+1].shift < a && hlist[i].shift > b)
          swap((&slice.back())[-1], slice.back()); 
        }
      if(isize(slice) < 3) return;
      for(auto e: slice) curvepoint(e);
      curvepoint(slice[0]);
      queuecurve(shiftless(Id), 0xFFFFFFFF, col, PPR::WALL);
      };
    
    for(int v=floor(minv); v<maxv+1; v++) {
      draw_slice(v, v+1, (v & 3) ? 0x080828FF : 0xA04020FF);
      }

    col = 1;
    }
  else {
    col = 
      t == wtSolid ? 0x603000FF :
      t == wtDestructible ? 0x301800FF :
      t == wtBarrier ? 0xC0C0C0FF :
      empty_color(c);

    color_t out = t == wtNone ? empty_outline(c) : 0xFF;

    for(auto h: hlist) curvepoint(h.h);
    if(col >> 8)
      addaura(shiftless(cd.center.h), col >> 8, 0);
    queuecurve(shiftless(Id), out, col, PPR::WALL);
    }

  ld ads_scale = get_scale();

  if(col >> 8) view_time(shiftless(rgpushxto0(cd.center.h)), cd.center.shift, 0xFF4040);

  // need i-loop because new rocks can be created in handle_turret

  for(int i=0; i<isize(ci.rocks); i++) {
    auto& rock = *ci.rocks[i];

    if(!paused) {
      if(rock.type == oRock && expired(rock.expire, pdata)) { rock.resource = rtNone; rock.col = rock_color[rtNone]; rock.expire.score = 999999; }
      if(rock.type == oResource && expired(rock.expire, pdata)) { rock.resource = rtNone; rock.col = rsrc_color[rtNone]; rock.shape = rsrc_shape[rtNone]; rock.expire.score = 999999; }
      }
    
    ld ang = 0;

    ads_matrix M;

    hybrid::in_actual([&]{
      dynamicval<eGeometry> b(geometry, gTwistedProduct);
      M = V * rock.at;
      rock.pt_main = cross0(current * M);
      if(rock.type == oTurret) handle_turret(&rock, ang);
      if(ang) M = M * spin(ang);
      });
    
    if(rock.pt_main.shift < rock.life_start || rock.pt_main.shift > rock.life_end) continue;
    displayed.push_back(&rock);

    rock.pts.clear();
    auto& shape = *rock.shape;
    for(int i=0; i<isize(shape); i += 2) {
      hybrid::in_actual([&]{
        auto h = M * twist::uxpush(shape[i] * ads_scale) * twist::uypush(shape[i+1] * ads_scale);
        cross_result f = cross0(current * h);
        rock.pts.push_back(f);
        });
      }

    if(rock.type != oParticle && pointcrash(mousetester, rock.pts)) under_mouse.push_back(&rock);

    if(hv) {
      ld t = rock.life_start;
      if(t < -100) t = 0;
      ld shift = floor((rock.pt_main.shift - t) / spacetime_step) * spacetime_step + t;
      ads_point M = current * (V * rock.at) * ads_matrix(Id, shift) * C0;
      optimize_shift(M);
      for(int z0=-spacetime_qty; z0<=spacetime_qty; z0++) {
        ld z = z0 * spacetime_step;
        if((shift+z) < rock.life_start) continue;
        if((shift+z) > rock.life_end) continue;
        for(int i=0; i<isize(shape); i += 2) {
          auto h = twist::uxpush(shape[i] * ads_scale) * twist::uypush(shape[i+1] * ads_scale) * C0;
          curvepoint(h);
          }
        curvepoint_first();
        ads_matrix S = current * V * rock.at * ads_matrix(Id, rock.pt_main.shift+z);
        apply_duality(S);
        queuecurve(S, rock.col, 0, PPR::LINE);
        }
      }

    else {
      for(auto h: rock.pts) curvepoint(h.h);
      curvepoint(rock.pts[0].h);
      queuecurve(shiftless(Id),
        rock.type == oMissile ? missile_color :
        rock.type == oTurretMissile ? 0xFF8000FF :
        rock.type == oParticle ? rock.col :
        0x000000FF, rock.col, obj_prio[rock.type]);
      }

    if(rock.type != oParticle) view_time(shiftless(rgpushxto0(rock.pt_main.h)), rock.pt_main.shift, 0xFFFFFF);
    }

  ld last_shown = -HUGE_VAL;
  if(paused || which_cross) if(hv) for(auto& rock: ci.shipstates) {

    if(rock.start < last_shown + ship_history_period) continue;
    last_shown = rock.start;

    render_ship_parts([&] (const hpcshape& sh, color_t col, int sym) {
      int dx = sym ? -1 : 1;
      for(int i=sh.s; i<sh.e; i++) {
        auto h = twist::uxpush(cgi.hpc[i][0]) * twist::uypush(cgi.hpc[i][1] * dx) * C0;
        curvepoint(h);
        }
      ads_matrix S = current * V * rock.at;
      S = S * spin(-(rock.ang+90)*degree);
      apply_duality(S);
      S = S * spin(+(rock.ang+90)*degree);
      queuecurve(S, col, 0, PPR::LINE);
      });

    }

  if(paused || which_cross) if(!hv && !ci.shipstates.empty()) {
    cross_result cr;

    int lo = 0, hi = isize(ci.shipstates)-1;
    while(lo < hi) {
      auto med = (lo + hi + 1) / 2;
      hybrid::in_actual([&]{
        dynamicval<eGeometry> b(geometry, gTwistedProduct);
        auto h = V * ci.shipstates[med].at;
        cr = cross0(current * h);
        });
      if(cr.shift < -1e-6) hi = med - 1;
      else lo = med;
      }

    auto& rock = ci.shipstates[lo];

    hybrid::in_actual([&]{
      dynamicval<eGeometry> b(geometry, gTwistedProduct);
      auto h = V * rock.at;
      cr = cross0(current * h);
      });

    bool ok = cr.shift >= -1e-6 && cr.shift < rock.duration + 1e-6;

    if(ok) render_ship_parts([&] (const hpcshape& sh, color_t col, int sym) {
      int dx = sym ? -1 : 1;
      vector<hyperpoint> pts;
      for(int i=sh.s; i<sh.e; i++) {
        auto& ac = cgi.hpc;
        hybrid::in_actual([&]{
          auto h = V * rock.at * rgpushxto0(normalize(hyperpoint(ac[i][0], ac[i][1] * dx, 1, 0)));
          pts.push_back(cross0(current * h).h);
          });
        }

      for(auto h: pts) curvepoint(h);
      queuecurve(shiftless(Id), 0xFF, col, PPR::MONSTER_FOOT);
      });

    if(ok) view_time(shiftless(rgpushxto0(cr.h)), cr.shift + rock.start, 0xC0C0C0);
    }

  if(paused && c == vctr_ship && !game_over && !in_replay && !hv) {
    cross_result cr;
    hybrid::in_actual([&]{
      auto h = ads_inverse(current_ship * vctrV_ship);
      cr = cross0(current * V * h);
      // println(hlog, current * V * h);
      });
    if(cr.shift < M_PI && cr.shift < M_PI) {
      vector<hyperpoint> pts;
      for(int i=0; i<=360; i++) {
        hybrid::in_actual([&]{
          auto h = ads_inverse(current_ship * vctrV_ship) * spin(i*degree);
          auto cr = cross0_light(current * V * h);
          pts.push_back(cr.h);
          });
        }
      for(auto h: pts) curvepoint(h);
      queuecurve(shiftless(Id), 0xFF0000C0, cr.shift > 0 ? 0x00000060 : 0xFFFFFF10, PPR::SUPERLINE);
      }
    }
  }

void view_footer() {
  if(inHighQual) return;
  string str = "Relative Hell ";
  str += VER_RH;
  if(main_rock) str += " dS";
  else str += " AdS";
  if(paused) str += " (paused)";
  if(!nofps) str += XLAT(" fps: ") + its(calcfps());
  str += " " + copyright_shown;

  queuestr(10, vid.yres-vid.fsize, 0, vid.fsize/2, str, 0x202020, 0, 0);
  }

void view_ads_game() {
  displayed.clear();
  cds_last = std::move(cds); cds.clear();
  mousetester = kleinize(unshift(mouseh));
  under_mouse.clear();
  
  bool hv = mhybrid;

  hybrid::in_actual([&] {
    gen_budget = max_gen_per_frame;
    
    vctr = new_vctr;
    vctrV = new_vctrV;
    cross_result base;

    // todo rebase
    base = findflat(ads_point(C0, 0));
    // println(hlog, base.h);
    ld ebase = 0;
    hybrid::in_underlying_geometry([&]{
      ebase = hdist0(base.h);
      // println(hlog, "dist base pre = ", hdist0(base.h), " at ", base.shift);
      });
    // println(hlog, base.h[2]*base.h[2] - base.h[1]*base.h[1] - base.h[0] * base.h[0]);
    
    for(int u=0; u<30; u++) {
      auto bcurrent = current;
      
      transmatrix T = rgpushxto0( spin(TAU*u/30) * xpush0(0.5) );
  
      current.T = current.T * T;
      auto base1 = findflat(ads_point(C0, 0));
      ld ebase1 = 0;
      hybrid::in_underlying_geometry([&] {
        ebase1 = hdist0(base1.h);
        });
      if(ebase1 < ebase) { vctrV.T = inverse(T) * vctrV.T; }
      else { current = bcurrent; }
      }
    }
    );

  if(1) {
    set<cell*> visited;
    std::priority_queue<cell_to_draw> dq;
    auto visit = [&] (cell *c, const ads_matrix& V) {
      if(visited.count(c)) return;
      visited.insert(c);
      
      cell_to_draw cd;
      cd.c = c;
      cd.V = V;
      cd.center = findflat(V * C0);
      cd.d = hdist0(cd.center.h);
      if(cd.d < vctr_dist) vctr_dist = cd.d, new_vctr = c, new_vctrV = V;
      
      dq.emplace(cd);
      };
    
    hybrid::in_actual([&] {
      vctr_dist = HUGE_VAL;
      visit(vctr, vctrV);
      });
    
    int i = 0;
    ld lastd = -10;

    while(!dq.empty()) {

      i++;
      auto& cd = dq.top();
      if(i > draw_per_frame && cd.d > lastd + 1e-4) break;
      lastd = cd.d;
      cds[cd.c] = cd;
      draw_game_cell(cd);

      cell *c = cd.c;
      ads_matrix V = cd.V;
      dq.pop();
      
      hybrid::in_actual([&] {
        auto csl = hybrid::get_at(c, 0);
        for(int i=0; i<c->type; i++) {
          cell *csl2 = csl->cmove(i);
          auto V1 = V * currentmap->adj(csl, i);
          optimize_shift(V1);

          auto g = hybrid::get_where(csl2);
          adjust_to_zero(V1, g, cgi.plevel);

          visit(g.first, V1);
          }
        });
      }

    if(!game_over && !paused && !in_replay && !hv && !which_cross) {
      poly_outline = 0xFF;
      if(ship_pt < invincibility_pt && invincibility_pt < HUGE_VAL) {
        ld u = (invincibility_pt-ship_pt) / ads_how_much_invincibility;
        poly_outline = gradient(shipcolor, rsrc_color[rtHull], 1, cos(5*u*TAU), -1);
        }
      render_ship_parts([&] (const hpcshape& sh, color_t col, int sym) {
        shiftmatrix M = shiftless(spin(ang*degree) * Id);
        if(sym) M = M * MirrorY;
        queuepolyat(M, sh, col, PPR::MONSTER_HAIR);
        });
      poly_outline = 0xFF;

      view_time(shiftless(Id), ship_pt, 0xFFFFFF);
      }    
    }

  copyright_shown = "";
  view_footer();
  }

}}
