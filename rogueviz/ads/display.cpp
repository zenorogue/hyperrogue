namespace hr {

namespace ads_game {

color_t shipcolor = 0x2020FFFF;

cross_result findflat(shiftpoint h) {
  return cross0(current * rgpushxto0(h));
  }

void draw_game_cell(cell *cs, ads_matrix V, ld plev) {  
  auto g = PIA( hybrid::get_where(cs) );
  adjust_to_zero(V, g, plev);
  auto c = g.first;
  
  cross_result center;
  vector<cross_result> hlist;

  hybrid::in_actual([&]{    
    for(int i=0; i<=c->type; i++) {
      hyperpoint ha = hybrid::get_corner(c, i, 2, 0);
      hlist.push_back(findflat(V * ha));
      }
    center = findflat(V * C0);
    });
  
  if(1) {
    ld d = hdist0(center.h);
    if(d < vctr_dist) vctr_dist = d, new_vctr = PIA( hybrid::get_at(c, 0) ), new_vctrV = V;
    }

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
      queuecurve(shiftless(Id), 0xFFFFFFFF, col, PPR::LINE);
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
    addaura(shiftless(center.h), col >> 8, 0);
    queuecurve(shiftless(Id), 0x101010FF, col, PPR::WALL);
    }

  if(view_proper_times) {
    string str = format(tformat, center.shift / time_unit);
    queuestr(shiftless(rgpushxto0(center.h)), .1, str, 0xFF4040, 8);
    }

  for(auto& r: ci.rocks) {
    auto& rock = *r;
    
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
        auto h = V * rock.at * rots::uxpush(shape[i] * scale) * rots::uypush(shape[i+1] * scale);
        cross_result f = cross0(current * h);
        rock.pts.push_back(f);
        });
      }

    for(auto h: rock.pts) curvepoint(h.h);
    curvepoint(rock.pts[0].h);
    queuecurve(shiftless(Id), 
      rock.type == oMissile ? missile_color : 
      rock.type == oParticle ? rock.col :
      0x000000FF, rock.col, PPR::LINE);

    if(view_proper_times && rock.type != oParticle) {
      string str = format(tformat, rock.pt_main.shift / time_unit);
      queuestr(shiftless(rgpushxto0(rock.pt_main.h)), .1, str, 0xFFFFFF, 8);
      }
    }
  
  /* todo: binary search */
  if(paused) for(auto& rock: ci.shipstates) {
    ld t;
    hybrid::in_actual([&]{
      dynamicval<eGeometry> b(geometry, gRotSpace);
      auto h = V * rock.at;
      t = cross0(current * h).shift;
      });
    
    if(t < -1e-6 || t > rock.duration + 1e-6) continue;
    vector<hyperpoint> pts;

    auto& shape = shape_ship;
    for(int i=0; i<isize(shape); i += 2) {
      hybrid::in_actual([&]{
        auto h = V * rock.at * rgpushxto0(normalize(hyperpoint(shape[i] * scale, shape[i+1] * scale, 1, 0)));
        pts.push_back(cross0(current * h).h);
        });
      }

    for(auto h: pts) curvepoint(h);
    curvepoint(pts[0]);
    queuecurve(shiftless(Id), 0xFF, shipcolor, PPR::LINE);
    }
  
  if(paused && c == vctr_ship_base && !game_over) {
    vector<hyperpoint> pts;
    vector<ld> times;
    int ok = 0, bad = 0;
    for(int i=0; i<=360; i++) {
      hybrid::in_actual([&]{
        auto h = ads_inverse(current_ship * vctrV_ship) * spin(i*degree) * lorentz(0, 2, 6);
        auto cr = cross0(current * V * h);
        pts.push_back(cr.h);
        times.push_back(cr.shift);
        if(cr.shift > 0 && cr.shift < 90*degree) ok++; else bad++;
        });
      }
    if(bad == 0) {
      for(auto h: pts) curvepoint(h);
      queuecurve(shiftless(Id), 0x000000C0, 0x00000060, PPR::SUPERLINE);
      }
    }
  }

void view_ads_game() {
  ld plev;
  displayed.clear();
  
  hybrid::in_actual([&] {
    plev = cgi.plevel; /* we are in another CGI so we have no access to that... */
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
      
      transmatrix T = spin(12*degree*u) * xpush(0.5);
  
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
  // current = current * gpushxto0(p);
  // vctrV = rgpushxto0(p) * vctrV;

  if(1) {
    make_shape();

    set<cell*> visited;
    using key = tuple<ld, cell*, ads_matrix>;
    auto cmp = [] (const key& a1, const key& a2) { return get<0>(a1) < get<0>(a2); };
    std::priority_queue<key, vector<key>, decltype(cmp)> dq(cmp);
    auto visit = [&] (ld t, cell *c, const ads_matrix& V) {
      auto w = hybrid::get_where(c);
      if(visited.count(w.first)) return;
      visited.insert(w.first);
      dq.emplace(t, c, V);
      };
    
    hybrid::in_actual([&] {
      dynamicval<eGeometry> b(geometry, gRotSpace);
      visit(0, vctr, vctrV);
      vctr_dist = HUGE_VAL;
      });
    
    int i = 0;
    while(!dq.empty()) {

      i++; if(i > draw_per_frame) break;
      auto& p = dq.top();
      cell *c = get<1>(p);
      ads_matrix V = get<2>(p);
      dq.pop();

      draw_game_cell(c, V, plev);
      
      hybrid::in_actual([&] {
        for(int i=0; i<c->type-2; i++) {
          cell *c2 = c->cmove(i);
          auto V1 = V * currentmap->adj(c, i);
          optimize_shift(V1);

          auto g = hybrid::get_where(c2);
          adjust_to_zero(V1, g, plev);
          c2 = hybrid::get_at(g.first, 0);
          auto center = findflat(V1 * C0);

          visit(-hdist0(center.h), c2, V1);
          }
        });
      }

    if(!game_over && !paused) {
      poly_outline = 0xFF;
      if(ship_pt < invincibility_pt) {
        ld u = (invincibility_pt-ship_pt) / how_much_invincibility;
        poly_outline = gradient(shipcolor, rsrc_color[rtHull], 0, 0.5 + cos(5*u*TAU), 1);
        }
      queuepolyat(shiftless(spin(ang*degree) * Id), shShip, shipcolor, PPR::LINE);
      poly_outline = 0xFF;

      if(view_proper_times) {
        string str = format(tformat, ship_pt / time_unit);
        queuestr(shiftless(Id), .1, str, 0xFFFFFF, 8);
        }
      }
    
    if(paused && view_proper_times) {
      string str = format(tformat, view_pt / time_unit);
      queuestr(shiftless(Id), .1, str, 0xFFFF00, 8);
      }
    }
  }

}}
