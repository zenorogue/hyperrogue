namespace hr {

namespace ads_game {

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
    if(d < vctr_dist) vctr_dist = d, vctr = PIA( hybrid::get_at(c, 0) ), vctrV = V;
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
    string str = format(tformat, center.shift / TAU);
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
        auto h = V * rock.at * rots::uxpush(shape[i]) * rots::uypush(shape[i+1]);
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

    if(view_proper_times) {
      string str = format(tformat, rock.pt_main.shift / TAU);
      queuestr(shiftless(rgpushxto0(rock.pt_main.h)), .1, str, 0xFFFFFF, 8);
      }
    }
  
  }

bool view_ads_game() {
  auto plev = cgi.plevel; /* we are in another CGI so we have no access to that... */
  gen_budget = 5;
  displayed.clear();
  
  cross_result base;
  if(1) {  
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
  // current = current * gpushxto0(p);
  // vctrV = rgpushxto0(p) * vctrV;

  hybrid::in_underlying_geometry([&] {
    dynamicval<eModel> p(pmodel, mdDisk);
    check_cgi();
    cgi.require_basics();
    cgi.require_shapes();
    ptds.clear();
    calcparam();
    clearaura();
    make_shape();
    
    set<cell*> visited;
    queue<pair<cell*, ads_matrix>> dq;
    auto visit = [&] (cell *c, const ads_matrix& V) {
      auto w = hybrid::get_where(c);
      if(visited.count(w.first)) return;
      visited.insert(w.first);
      dq.emplace(c, V);
      };
    
    hybrid::in_actual([&] {
      dynamicval<eGeometry> b(geometry, gRotSpace);
      visit(vctr, vctrV);
      vctr_dist = HUGE_VAL;
      });
    
    int i = 0;
    while(!dq.empty()) {
      i++; if(i > 1000) break;
      auto& p = dq.front();
      cell *c = p.first;
      ads_matrix V = p.second;
      dq.pop();

      draw_game_cell(c, V, plev);
      
      hybrid::in_actual([&] {
        for(int i=0; i<c->type-2; i++) {
          cell *c2 = c->cmove(i);
          auto V1 = V * currentmap->adj(c, i);
          optimize_shift(V1);
          visit(c2, V1);
          }
        });
      }

    if(true) {
      poly_outline = 0xFF;
      queuepolyat(shiftless(spin(ang*degree) * Id), shShip, 0x2020FFFF, PPR::LINE);

      if(view_proper_times) {
        string str = format(tformat, ship_pt / TAU);
        queuestr(shiftless(Id), .1, str, 0xFFFFFF, 8);
        }
      }

    if(false) queuepolyat(shiftless(rgpushxto0(base.h)), cgi.shGem[0], 0x2020FFFF, PPR::LINE);
    
    drawqueue();
    drawaura();
    });
  check_cgi();
  return true;
  }

}}
