namespace hr {

namespace ads_game {

color_t shipcolor = 0x2020FFFF;

cross_result findflat(shiftpoint h) {
  return cross0(current * rgpushxto0(h));
  }

struct cell_to_draw {
  cross_result center;
  ld d;
  cell *c;
  ads_matrix V;
  bool operator < (const cell_to_draw& c2) const { return d > c2.d; }
  };

void draw_game_cell(const cell_to_draw& cd) {

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
  gen_terrain(c, ci);
  gen_rocks(c, ci, 0);

  auto& t = ci.type;

  if(t == wtGate) {
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
    }
  else {
    color_t col = 
      t == wtSolid ? 0x603000FF :
      t == wtDestructible ? 0x301800FF :
      0x181818FF;

    for(auto h: hlist) curvepoint(h.h);
    addaura(shiftless(cd.center.h), col >> 8, 0);
    queuecurve(shiftless(Id), 0x101010FF, col, PPR::WALL);
    }

  if(view_proper_times) {
    string str = format(tformat, cd.center.shift / ads_time_unit);
    queuestr(shiftless(rgpushxto0(cd.center.h)), .1, str, 0xFF4040, 8);
    }

  for(auto& r: ci.rocks) {
    auto& rock = *r;

    if(!paused) {
      if(rock.type == oRock && rock.expire < pdata.score) { rock.resource = rtNone; rock.col = rock_color[rtNone]; rock.expire = 999999; }
      if(rock.type == oResource && rock.expire < pdata.score) { rock.resource = rtNone; rock.col = rsrc_color[rtNone]; rock.shape = rsrc_shape[rtNone]; rock.expire = 999999; }
      }
    
    hybrid::in_actual([&]{
      dynamicval<eGeometry> b(geometry, gRotSpace);
      auto h = V * rock.at;
      rock.pt_main = cross0(current * h);
      });
    
    if(rock.pt_main.shift < rock.life_start || rock.pt_main.shift > rock.life_end) continue;
    displayed.push_back(&rock);

    rock.pts.clear();
    auto& shape = *rock.shape;
    for(int i=0; i<isize(shape); i += 2) {
      hybrid::in_actual([&]{
        auto h = V * rock.at * rots::uxpush(shape[i] * ads_scale) * rots::uypush(shape[i+1] * ads_scale);
        cross_result f = cross0(current * h);
        rock.pts.push_back(f);
        });
      }

    for(auto h: rock.pts) curvepoint(h.h);
    curvepoint(rock.pts[0].h);
    queuecurve(shiftless(Id), 
      rock.type == oMissile ? missile_color : 
      rock.type == oParticle ? rock.col :
      0x000000FF, rock.col, obj_prio[rock.type]);

    if(view_proper_times && rock.type != oParticle) {
      string str = format(tformat, rock.pt_main.shift / ads_time_unit);
      queuestr(shiftless(rgpushxto0(rock.pt_main.h)), .1, str, 0xFFFFFF, 8);
      }
    }
  
  /* todo: binary search */
  if(paused) for(auto& rock: ci.shipstates) {
    cross_result cr;
    hybrid::in_actual([&]{
      dynamicval<eGeometry> b(geometry, gRotSpace);
      auto h = V * rock.at;
      cr = cross0(current * h);
      });
        
    if(cr.shift < -1e-6 || cr.shift > rock.duration + 1e-6) continue;
    vector<hyperpoint> pts;

    auto& shape = shape_ship;
    for(int i=0; i<isize(shape); i += 2) {
      hybrid::in_actual([&]{
        auto h = V * rock.at * rgpushxto0(normalize(hyperpoint(shape[i] * ads_scale, shape[i+1] * ads_scale, 1, 0)));
        pts.push_back(cross0(current * h).h);
        });
      }

    for(auto h: pts) curvepoint(h);
    curvepoint(pts[0]);
    queuecurve(shiftless(Id), 0xFF, shipcolor, PPR::MONSTER_FOOT);

    if(view_proper_times) {
      string str = format(tformat, (cr.shift + rock.start) / ads_time_unit);
      queuestr(shiftless(rgpushxto0(cr.h)), .1, str, 0xC0C0C0, 8);
      }
    }
  
  if(paused && c == vctr_ship && !game_over && !inHighQual) {
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
      
      transmatrix T = rgpushxto0( spin(12*degree*u) * xpush0(0.5) );
  
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
    while(!dq.empty()) {

      i++; if(i > draw_per_frame) break;
      auto& cd = dq.top();
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

    if(!game_over && !paused) {
      poly_outline = 0xFF;
      if(ship_pt < invincibility_pt) {
        ld u = (invincibility_pt-ship_pt) / ads_how_much_invincibility;
        poly_outline = gradient(shipcolor, rsrc_color[rtHull], 0, 0.5 + cos(5*u*TAU), 1);
        }
      queuepolyat(shiftless(spin(ang*degree) * Id), make_shape(), shipcolor, PPR::MONSTER_HAIR);
      poly_outline = 0xFF;

      if(view_proper_times) {
        string str = format(tformat, ship_pt / ads_time_unit);
        queuestr(shiftless(Id), .1, str, 0xFFFFFF, 8);
        }
      }
    
    if(paused && view_proper_times) {
      string str = format(tformat, view_pt / ads_time_unit);
      queuestr(shiftless(Id), .1, str, 0xFFFF00, 8);
      }
    }

  copyright_shown = "";
  view_footer();
  }

}}
