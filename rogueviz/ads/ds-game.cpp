namespace hr {

namespace ads_game {

void set_default_keys();

transmatrix dscurrent, dscurrent_ship;

vector<shipstate> ds_states;

bool ds_turn(int idelta) {
  multi::handleInput(idelta);
  ld delta = idelta / anims::period;
  
  if(!(cmode & sm::NORMAL)) return false;
  
  // todo handle_crashes();

  auto& a = multi::actionspressed;
  auto& la = multi::lactionpressed;
  
  // todo if(a[16+4] && !la[16+4] && !paused) fire();

  if(a[16+5] && !la[16+5]) {
    paused = !paused;
    if(paused) {
      dscurrent_ship = dscurrent;
      view_pt = 0;
      }
    else {
      dscurrent = dscurrent_ship;
      }
    }

  if(a[16+6] && !la[16+6]) view_proper_times = !view_proper_times;
  if(a[16+7] && !la[16+7]) auto_rotate = !auto_rotate;
  if(a[16+8] && !la[16+8]) pushScreen(game_menu);

  if(true) {
    dynamicval<eGeometry> g(geometry, gSpace435);
    
    ld pt = delta * simspeed;
    ld mul = read_movement();

    if(paused && a[16+11]) {
      dscurrent = spin(ang*degree) * cspin(0, 2, mul*delta*-pause_speed) * spin(-ang*degree) * dscurrent;
      }
    else {
      dscurrent = spin(ang*degree) * lorentz(0, 3, -delta*accel*mul) * spin(-ang*degree) * dscurrent;
      }
    
    if(!paused) {
      pdata.fuel -= delta*accel*mul;
      // todo gen_particles(rpoisson(delta*accel*mul*fuel_particle_qty), vctr, ads_inverse(current * vctrV) * spin(ang*degree+M_PI) * rots::uxpush(0.06 * scale), rsrc_color[rtFuel], fuel_particle_rapidity, fuel_particle_life, 0.02);
      }

    ld tc = 0;
    if(!paused) tc = pt;
    else if(a[16+9]) tc = pt;
    else if(a[16+10]) tc = -pt;

    if(!paused && !game_over) {
      auto& v = ds_states;
      v.emplace_back();
      v.back().at.T = inverse(dscurrent) * spin(ang*degree);
      v.back().start = ship_pt;
      v.back().duration = pt;
      }
    
    dscurrent = lorentz(3, 2, -tc) * dscurrent;
    
    if(!paused) {
      ship_pt += pt;
      pdata.oxygen -= pt;
      if(pdata.oxygen < 0) {
        pdata.oxygen = 0;
        game_over = true;
        }
      }
    else view_pt += tc;
    }

  return true;
  }

vector<transmatrix> opoints;

hyperpoint pov = point30(0, 0, 1);

cross_result ds_cross0(transmatrix T) {
  // h = T * (0 0 cosh(t) sinh(t))
  // h[3] == 0
  // T[3][2] * cosh(t) + T[3][3] * sinh(t) = 0
  // T[3][2] + T[3][3] * tanh(t) = 0
  ld tt = - T[3][2] / T[3][3];
  if(tt < -1 || tt > 1) return cross_result{ Hypc, 0 };
  cross_result res;
  ld t = atanh(tt);
  res.shift = t;
  res.h = T * hyperpoint(0, 0, cosh(t), sinh(t));
  return res;
  }

void view_ds_game() {
  displayed.clear();
  
  if(opoints.empty()) {
    for(int i=0; i<500; i++) {
      hyperpoint h = random_spin3() * C0;
      println(hlog, "h = ", h);

      transmatrix T = gpushxto0(h);
      dynamicval<eGeometry> g(geometry, gSpace435);
      for(int i=0; i<4; i++) T[i][3] = T[3][i] = i == 3;
      transmatrix worldline = inverse(T);
      worldline = worldline * spin(randd() * TAU);
      worldline = worldline * lorentz(0, 3, randd());
      
      opoints.push_back(worldline);
      }
    }
  
  if(1) {
    make_shape();
    
    for(auto& worldline: opoints) {
      poly_outline = 0xFF;
      cross_result cr;
      transmatrix at;
      
      vector<hyperpoint> circle;
      
      if(1) {
        dynamicval<eGeometry> g(geometry, gSpace435);
        cr = ds_cross0(dscurrent * worldline);

        at = dscurrent * worldline * lorentz(2, 3, cr.shift);

        for(int d=0; d<=360; d += 5) {
          transmatrix T = at * spin(d*degree) * cspin(0, 2, 0.1);
          auto cr1 = ds_cross0(T);
          circle.push_back(cr1.h);
          }
        }
      
      vector<hyperpoint> circle_flat;
      for(auto c: circle) circle_flat.push_back(c / (1 + c[2]));
      
      ld area = 0;
      for(int i=0; i<isize(circle)-1; i++)
        area += (circle_flat[i] ^ circle_flat[i+1]) [2];
      
      if(area > 0) continue;

      // queuepolyat(shiftless(rgpushxto0(cr.h)), cgi.shGem[0], 0xFFFFFFF, PPR::LINE);
      for(auto p: circle) curvepoint(p);
      queuecurve(shiftless(Id), 0xFFFFFFFF, 0xFFFFFFFF, PPR::LINE);

      if(view_proper_times) {
        string str = format(tformat, cr.shift / time_unit);
        queuestr(shiftless(rgpushxto0(cr.h)), .1, str, 0xFFFF00, 8);
        }
      }

    ld delta = paused ? 1e-4 : -1e-4;
    for(auto& ss: ds_states) {
      dynamicval<eGeometry> g(geometry, gSpace435);
      cross_result cr = ds_cross0(dscurrent * ss.at.T);
      if(cr.shift < delta) continue;
      if(cr.shift > ss.duration + delta) continue;
      transmatrix at = dscurrent * ss.at.T * lorentz(2, 3, cr.shift);
      
      vector<hyperpoint> pts;
      
      auto& shape = shape_ship;
      for(int i=0; i<isize(shape); i += 2) {
        transmatrix at1 = at * cspin(0, 2, shape[i] * scale) * cspin(1, 2, shape[i+1] * scale);
        pts.push_back(ds_cross0(at1).h);
        }
      
      geometry = g.backup;
      for(auto pt: pts) curvepoint(pt);
      queuecurve(shiftless(Id), 0xFF, shipcolor, PPR::LINE);

      if(view_proper_times) {
        string str = format(tformat, (cr.shift + ss.start) / time_unit);
        queuestr(shiftless(rgpushxto0(cr.h)), .1, str, 0xC0C0C0, 8);
        }
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

void run_ds_game() {

  set_default_keys();

  rogueviz::rv_hook(hooks_frame, 100, view_ds_game);
  rogueviz::rv_hook(shmup::hooks_turn, 0, ds_turn);
  rogueviz::rv_hook(hooks_prestats, 100, display_rsrc);
  
  if(true) {
    dynamicval<eGeometry> g(geometry, gSpace435);
    dscurrent = Id;
    }

  ship_pt = 0;
  init_rsrc();
  
  rogueviz::rv_change(nohelp, true);
  rogueviz::rv_change(nomenukey, true);
  rogueviz::rv_change(nomap, true);
  rogueviz::rv_change(no_find_player, true);
  rogueviz::rv_change(showstartmenu, false);
  rogueviz::rv_hook(hooks_handleKey, 0, handleKey);
  }

auto ds_hooks = 
  arg::add3("-ds-game", run_ds_game)
;

}
}
