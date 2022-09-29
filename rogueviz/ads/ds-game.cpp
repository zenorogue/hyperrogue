namespace hr {

namespace ads_game {

void init_textures();
void draw_textures();

vector<ld> shape_disk;

void set_default_keys();

/* In DS, we also use ads_matrix, but the meaning of the shift parameter is different:
 * 
 */

vector<unique_ptr<ads_object>> rocks;

struct rock_generator {
  ld cshift;

  ads_object* add(transmatrix T) {
    auto r = std::make_unique<ads_object> (oRock, nullptr, ads_matrix(T, cshift), 0xFFFFFFFF);
    r->shape = &shape_disk;
    auto res = &*r;
    rocks.emplace_back(std::move(r));
    return res;
    };

  void report(string s) {
    println(hlog, lalign(10, format(tformat, cshift/ds_time_unit)), ": ", s);
    };

  ld rand_range(ld a, ld b) { return lerp(a, b, randd()); };

  transmatrix rand_place() {
    geometry = gSphere;
    hyperpoint h = random_spin3() * C0;
    transmatrix T = gpushxto0(h);
    geometry = gSpace435;
    for(int i=0; i<4; i++) T[i][3] = T[3][i] = i == 3;
    return T;
    };

  void death_cross(int qty) {
    ld rapidity = rand_range(1, 3);
    cshift += rand_range(0.5, 1);
    ld alpha = randd() * TAU;
    report(lalign(0, "Death Cross ", qty));
    for(int a=0; a<qty; a++)
      add(spin(a * TAU / qty + alpha) * lorentz(0, 3, rapidity));
    cshift += rand_range(0.5, 1);
    }

  void static_starry_field() {
    cshift += rand_range(1, 2);
    report("Static Starry Field");
    for(int i=0; i<100; i++) {
      transmatrix T = rand_place();
      add(inverse(T));
      }
    cshift += rand_range(1, 2);
    }

  void chaotic_starry_field() {
    cshift += rand_range(2, 3);
    report("Chaotic Starry Field");
    for(int i=0; i<50; i++) {
      transmatrix T = rand_place();
      add(inverse(T) * spin(randd() * TAU) * lorentz(0, 3, rand_range(0, 3)));
      }
    cshift += rand_range(2, 3);
    }

  /* that pattern does not work */
  void death_spiral() {
    cshift += rand_range(2, 3) + 1.5;
    report("Death Spiral");
    for(int i=0; i<30; i++) {
      add(spin(i * TAU * 14 / 30) * lorentz(0, 3, exp((i-15)/5.)));
      }
    cshift += rand_range(2, 3);
    }

  transmatrix div_matrix() {
    /* we need to find the limit of this as appr -> inf */
    ld appr = 5;
    transmatrix T = lorentz(2, 3, -appr) * cspin(0, 2, exp(-appr)) * lorentz(2, 3, appr);
    /* all the entries happen to be multiples of .125 */
    for(int i=0; i<4; i++) for(int j=0; j<4; j++) {
      auto& b = T[i][j];
      b = floor(b * 8 + .5) / 8;
      }
    return T;
    }

  /* see div_matrix */
  transmatrix conv_matrix() {
    ld appr = 5;
    transmatrix T = lorentz(2, 3, appr) * cspin(0, 2, exp(-appr)) * lorentz(2, 3, -appr);
    for(int i=0; i<4; i++) for(int j=0; j<4; j++) {
      auto& b = T[i][j];
      b = floor(b * 8 + .5) / 8;
      }
    return T;
    }

  void divergent_spiral() {
    report("Divergent Spiral");
    cshift += rand_range(.3, .7);
    ld alpha = randd() * TAU;
    ld step = rand_range(0.17, 0.23);
    for(int i=0; i<45; i++) {
      cshift += step;
      add(spin(alpha + i * TAU / 30) * div_matrix());
      }
    cshift += rand_range(.3, .7);
    }

  void convergent_spiral() {
    report("Convergent Spiral");
    cshift += rand_range(.3, .7);
    ld alpha = randd() * TAU;
    ld step = rand_range(0.17, 0.23);
    for(int i=0; i<45; i++) {
      cshift += step;
      add(spin(alpha + i * TAU / 30) * conv_matrix());
      }
    cshift += rand_range(.3, .7);
    }

  void rack() {
    report("Rack");
    int qty = 3 + rand() % 4;
    ld rapidity = rand_range(1, 3);
    ld step = rand_range(.45, .75) * ds_scale;
    ld alpha = rand_range(0, TAU);
    ld spinv = rand_range(0, TAU);
    for(int i=0; i<qty; i++) {
      cshift ++;
      for(ld j=-3; j<=3; j++) {
        add(spin(alpha + i * spinv) * cspin(0, 2, j * step) * spin(90*degree) * lorentz(0, 3, rapidity));
        }
      }
    }

  void hyperboloid() {
    report("Hyperboloid");
    ld alpha = randd() * TAU;
    ld range1 = rand_range(0.15, 0.25) * ds_scale;
    ld range2 = rand_range(0.35, 0.45) * ds_scale;
    cshift += rand_range(2, 3);
    ld rapidity = rand_range(-3, 3);
    int qty = 20 + rand() % 10;
    for(int i=0; i<qty; i++)
      add(spin(alpha) * cspin(0, 2, range1) * spin(i * TAU / qty) * cspin(0, 2, range2) * lorentz(1, 3, rapidity));
    cshift += rand_range(2, 3);
    }

  void machinegun() {
    report("Machinegun");
    ld alpha = randd() * TAU;
    int qty = 10 + 1 / (.05 + randd());
    ld rapidity = rand_range(3, 6);
    ld step = rand_range(0.1, 0.15);
    for(int i=0; i<qty; i++) {
      cshift += step;
      add(spin(alpha) * lorentz(1, 3, rapidity));
      }
    }

  void add_random() {

    int r = rand() % 150;

    #define Chance(q) if(r < 0) return; r -= (q); if(r < 0)
    Chance(10) death_cross(4);
    Chance(10) death_cross(3);
    Chance(10) static_starry_field();
    Chance(10) chaotic_starry_field();
    Chance(10) divergent_spiral();
    Chance(10) convergent_spiral();
    Chance(10) rack();
    Chance(10) hyperboloid();
    Chance(10) machinegun();
    #undef Chance
    }

  void add_until(ld t) {
    while(cshift < t) add_random();
    }

  void add_rsrc_until(ld t) {
    while(cshift < t) {
      ld rapidity = rand_range(0, 3);
      ld step = rand_range(.2, .5);
      ld alpha = rand_range(0, TAU);
      cshift += rand_range(0.5, 1) * (1 + cshift / 10);
      auto r = add(spin(alpha) * cspin(0, 2, step) * spin(90*degree) * lorentz(0, 3, rapidity));
      eResourceType rt = eResourceType(2 + rand() % 4);
      if(rt == rtGold) rt = rtHull;
      r->type = oResource;
      r->resource = rt;
      r->shape = rsrc_shape[rt];
      r->col = rsrc_color[rt];
      }
    }

  };

rock_generator rockgen, rsrcgen;

auto future_shown = 5 * TAU;

void init_ds_game() {

  dynamicval<eGeometry> g(geometry, gSpace435);

  rockgen.cshift = 0;
  rsrcgen.cshift = 0;

  /* create the main rock first */
  main_rock = rockgen.add(Id);
  main_rock->col = 0xFFD500FF;
  main_rock->type = oMainRock;

  main_rock = rockgen.add(Id);
  main_rock->col = 0xFF;
  main_rock->shape = &shape_gold;
  main_rock->type = oMainRock;

  /* also create shape_disk */
  shape_disk.clear();
  for(int d=0; d<=360; d += 15) {
    shape_disk.push_back(sin(d*degree) * 0.1 * ds_scale);
    shape_disk.push_back(cos(d*degree) * 0.1 * ds_scale);
    }

  rockgen.cshift += 2;
  rockgen.add_until(future_shown);
  
  rsrcgen.cshift += 1;
  rsrcgen.add_rsrc_until(future_shown);
  }

void ds_gen_particles(int qty, transmatrix from, ld shift, color_t col, ld spd, ld t, ld spread = 1) {
  for(int i=0; i<qty; i++) {
    auto r = std::make_unique<ads_object>(oParticle, nullptr, ads_matrix(from * spin(randd() * TAU * spread) * lorentz(0, 3, (.5 + randd() * .5) * spd), shift), col );
    r->shape = &shape_particle;
    r->life_end = randd() * t;
    r->life_start = 0;
    rocks.emplace_back(std::move(r));
    }
  }

void ds_crash_ship() {
  if(ship_pt < invincibility_pt) return;
  common_crash_ship();
  dynamicval<eGeometry> g(geometry, gSpace435);
  ds_gen_particles(rpoisson(crash_particle_qty * 2), inverse(current.T) * spin(ang*degree), current.shift, rsrc_color[rtHull], crash_particle_rapidity, crash_particle_life);
  }

void ds_handle_crashes() {
  if(paused) return;
  vector<ads_object*> dmissiles;
  vector<ads_object*> drocks;
  vector<ads_object*> dresources;
  for(auto m: displayed) {
    if(m->type == oMissile)
      dmissiles.push_back(m);
    if(m->type == oRock || m->type == oMainRock)
      drocks.push_back(m);
    if(m->type == oResource)
      dresources.push_back(m);
    }

  for(auto m: dmissiles) {
    hyperpoint h = kleinize(m->pt_main.h);
    for(auto r: drocks) {
      if(pointcrash(h, r->pts)) {
        m->life_end = m->pt_main.shift;
        if(r->type != oMainRock)
          r->life_end = r->pt_main.shift;
        dynamicval<eGeometry> g(geometry, gSpace435);
        ds_gen_particles(rpoisson(crash_particle_qty), m->at.T * lorentz(2, 3, m->life_end), m->at.shift, missile_color, crash_particle_rapidity, crash_particle_life);
        if(r->type != oMainRock)
          ds_gen_particles(rpoisson(crash_particle_qty), r->at.T * lorentz(2, 3, r->life_end), r->at.shift, r->col, crash_particle_rapidity, crash_particle_life);
        playSound(nullptr, "hit-crush3");
        break;
        }
      }
    }

  if(!game_over) for(int i=0; i<isize(shape_ship); i+=2) {
    hyperpoint h = spin(ang*degree) * hpxyz(shape_ship[i] * ds_scale, shape_ship[i+1] * ds_scale, 1);
    for(auto r: drocks) {
      if(pointcrash(h, r->pts)) ds_crash_ship();
      }
    for(auto r: dresources) {
      if(pointcrash(h, r->pts)) {
        r->life_end = r->pt_main.shift;
        gain_resource(r->resource);
        }
      }
    }
  }

void ds_fire() {
  if(!pdata.ammo) return;
  pdata.ammo--;
  playSound(nullptr, "fire");
  dynamicval<eGeometry> g(geometry, gSpace435);

  transmatrix S0 = inverse(current.T) * spin(ang*degree);

  transmatrix S1 = S0 * lorentz(0, 3, ads_missile_rapidity);

  auto r = std::make_unique<ads_object> (oMissile, nullptr, ads_matrix(S1, current.shift), rsrc_color[rtAmmo]);
  r->shape = &shape_missile;
  r->life_start = 0;

  rocks.emplace_back(std::move(r));
  }

bool ds_turn(int idelta) {
  multi::handleInput(idelta);
  ld delta = idelta / 1000.;
  
  if(!(cmode & sm::NORMAL)) return false;

  ds_handle_crashes();

  auto& a = multi::actionspressed;
  auto& la = multi::lactionpressed;

  if(a[16+4] && !la[16+4] && !paused) ds_fire();
  if(a[16+5] && !la[16+5]) {
    paused = !paused;
    if(paused) {
      current_ship = current;
      view_pt = 0;
      }
    else {
      current = current_ship;
      }
    }

  if(a[16+6] && !la[16+6]) view_proper_times = !view_proper_times;
  if(a[16+7] && !la[16+7]) auto_rotate = !auto_rotate;
  if(a[16+8] && !la[16+8]) pushScreen(game_menu);
  
  if(true) {
    dynamicval<eGeometry> g(geometry, gSpace435);
    
    ld pt = delta * ds_simspeed;
    ld mul = read_movement();
    ld dv = pt * ds_accel * mul;

    if(paused && a[16+11]) {
      current.T = spin(ang*degree) * cspin(0, 2, mul*delta*-pause_speed) * spin(-ang*degree) * current.T;
      }
    else {
      current.T = spin(ang*degree) * lorentz(0, 3, -dv) * spin(-ang*degree) * current.T;
      }
    
    if(!paused) {
      pdata.fuel -= dv;
      ds_gen_particles(rpoisson(dv*fuel_particle_qty), inverse(current.T) * spin(ang*degree+M_PI) * rots::uxpush(0.06 * ds_scale), current.shift, rsrc_color[rtFuel], fuel_particle_rapidity, fuel_particle_life, 0.02);
      }

    ld tc = 0;
    if(!paused) tc = pt;
    else if(a[16+9]) tc = pt;
    else if(a[16+10]) tc = -pt;

    if(!paused && !game_over) {
      shipstate ss;
      ss.at.T = inverse(current.T) * spin(ang*degree);
      ss.at.shift = current.shift;
      ss.start = ship_pt;
      ss.current = current;
      ss.duration = pt;
      ss.ang = ang;
      history.emplace_back(ss);
      }
    
    current.T = lorentz(3, 2, -tc) * current.T;

    auto& mshift = main_rock->pt_main.shift;
    if(mshift) {
      current.shift += mshift;
      current.T = current.T * lorentz(2, 3, mshift);
      mshift = 0;
      }
    fixmatrix(current.T);
    
    if(1) {
      rockgen.add_until(current.shift + future_shown);
      rsrcgen.add_rsrc_until(current.shift + future_shown);
      }

    if(!paused) {
      ship_pt += pt;
      pdata.oxygen -= pt;
      if(pdata.oxygen < 0) {
        pdata.oxygen = 0;
        game_over = true;
        }
      }
    else view_pt += tc;

    if(a[16+4] && !la[16+4] && false) {
      if(history.size())
        history.back().duration = HUGE_VAL;
      current = random_spin3();
      }
    }

  return true;
  }

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

cross_result ds_cross0_light(transmatrix T) {
  // h = T * (t 0 1 t); h[3] == 0
  ld t = T[3][2] / -(T[3][0] + T[3][3]);
  cross_result res;
  res.shift = t;
  res.h = T * hyperpoint(t, 0, 1, t);
  return res;
  }

transmatrix tpt(ld x, ld y) {
  return cspin(0, 2, x * ds_scale) * cspin(1, 2, y * ds_scale);
  }

// sometimes the result may be incorrect due to numerical precision -- don't show that then in this case
bool invalid(cross_result& res) {
  ld val = sqhypot_d(3, res.h);
  if(abs(val-1) > 1e-3 || isnan(val) || abs(res.h[3]) > 1e-3 || isnan(res.h[3])) return true;
  return false;
  }

void view_ds_game() {
  displayed.clear();
  
  draw_textures();

  sphereflip = sphereflipped() ? MirrorZ : Id;

  if(1) {
    for(auto& r: rocks) {
      auto& rock = *r;
      poly_outline = 0xFF;
      if(rock.type == oMainRock) rock.at.shift = current.shift;
    
      if(rock.at.shift < current.shift - future_shown) continue;
      if(rock.at.shift > current.shift + future_shown) continue;

      if(1) {
        dynamicval<eGeometry> g(geometry, gSpace435);
        transmatrix at = current.T * lorentz(2, 3, rock.at.shift - current.shift) * rock.at.T;
        rock.pt_main = ds_cross0(at);
        
        if(invalid(rock.pt_main)) continue;

        if(rock.pt_main.shift < rock.life_start) continue;
        if(rock.pt_main.shift > rock.life_end) continue;

        transmatrix at1 = at * lorentz(2, 3, rock.pt_main.shift);
        rock.pts.clear();
        
        auto& sh = *rock.shape;

        bool bad = false;
        for(int i=0; i<isize(sh); i+=2) {
          transmatrix at2 = at1 * tpt(sh[i], sh[i+1]);
          auto cr1 = ds_cross0(at2);
          if(invalid(cr1)) { bad = true; continue; }
          rock.pts.push_back(cr1);
          }
        if(bad) continue;
        }
      
      vector<hyperpoint> circle_flat;
      for(auto c: rock.pts) circle_flat.push_back(c.h / (1 + c.h[2]));
      
      ld area = 0;
      for(int i=0; i<isize(circle_flat)-1; i++)
        area += (circle_flat[i] ^ circle_flat[i+1]) [2];
      area += (circle_flat.back() ^ circle_flat[0]) [2];

      if(area > 0) continue;

      for(auto p: rock.pts) curvepoint(p.h);
      queuecurve(shiftless(sphereflip), rock.col, rock.col, obj_prio[rock.type]);

      if(view_proper_times && rock.type != oParticle) {
        ld t = rock.pt_main.shift;
        if(rock.type == oMainRock) t += current.shift;
        string str = format(tformat, t / ds_time_unit);
        queuestr(shiftless(sphereflip * rgpushxto0(rock.pt_main.h)), .1, str, 0xFFFF00, 8);
        }
      
      if(rock.pt_main.h[2] > 0.1 && rock.life_end == HUGE_VAL) {
        displayed.push_back(&rock);
        }
      }      

    ld delta = paused ? 1e-4 : -1e-4;
    if(paused) for(auto& ss: history) {
      if(ss.at.shift < current.shift - 4 * TAU) continue;
      if(ss.at.shift > current.shift + 4 * TAU) continue;
      dynamicval<eGeometry> g(geometry, gSpace435);
      cross_result cr = ds_cross0(current.T * lorentz(2, 3, ss.at.shift - current.shift) * ss.at.T);
      if(cr.shift < delta) continue;
      if(cr.shift > ss.duration + delta) continue;
      transmatrix at = current.T * lorentz(2, 3, cr.shift) * ss.at.T;
      
      vector<hyperpoint> pts;
      
      auto& shape = shape_ship;
      for(int i=0; i<isize(shape); i += 2) {
        transmatrix at1 = at * tpt(shape[i], shape[i+1]);
        pts.push_back(ds_cross0(at1).h);
        }
      
      geometry = g.backup;
      for(auto pt: pts) curvepoint(pt);
      queuecurve(shiftless(sphereflip), 0xFF, shipcolor, PPR::MONSTER_FOOT);

      if(view_proper_times) {
        string str = format(tformat, (cr.shift + ss.start) / ds_time_unit);
        queuestr(shiftless(sphereflip * rgpushxto0(cr.h)), .1, str, 0xC0C0C0, 8);
        }
      }

    if(!game_over && !paused) {
      poly_outline = 0xFF;
      if(ship_pt < invincibility_pt) {
        ld u = (invincibility_pt-ship_pt) / ds_how_much_invincibility;
        poly_outline = gradient(shipcolor, rsrc_color[rtHull], 0, 0.5 + cos(5*u*TAU), 1);
        }
      queuepolyat(shiftless(sphereflip * spin(ang*degree)), make_shape(), shipcolor, PPR::MONSTER_HAIR);
      poly_outline = 0xFF;

      if(view_proper_times) {
        string str = format(tformat, ship_pt / ds_time_unit);
        queuestr(shiftless(sphereflip), .1, str, 0xFFFFFF, 8);
        }
      }
    
    if(paused && view_proper_times) {
      string str = format(tformat, view_pt / ds_time_unit);
      queuestr(shiftless(sphereflip), .1, str, 0xFFFF00, 8);
      }

    if(paused && !game_over && !inHighQual) {
      vector<hyperpoint> pts;
      int ok = 0, bad = 0;
      for(int i=0; i<=360; i++) {
        dynamicval<eGeometry> g(geometry, gSpace435);
        auto h = inverse(current_ship.T) * spin(i*degree);
        auto cr = ds_cross0_light(current.T * lorentz(2, 3, current_ship.shift - current.shift) * h);
        pts.push_back(cr.h);
        if(cr.shift > 0) ok++; else bad++;
        }
      for(auto h: pts) curvepoint(h);
      queuecurve(shiftless(sphereflip), 0xFF0000C0, bad == 0 ? 0x00000060 : 0xFFFFFF10, PPR::SUPERLINE);
      }
    }

  view_footer();
  }

void ds_restart() {

  main_rock = nullptr;

  if(true) {
    dynamicval<eGeometry> g(geometry, gSpace435);
    current = cspin(0, 2, 0.2 * ds_scale);
    invincibility_pt = ds_how_much_invincibility;
    }

  ship_pt = 0;

  rocks.clear();
  history.clear();
  init_ds_game();
  reset_textures();
  pick_textures();
  init_rsrc();
  }

void run_ds_game() {

  stop_game();
  set_geometry(gSphere);
  start_game();

  init_textures();
  pick_textures();

  ds_restart();

  rogueviz::rv_hook(hooks_frame, 100, view_ds_game);
  rogueviz::rv_hook(shmup::hooks_turn, 0, ds_turn);
  rogueviz::rv_hook(hooks_prestats, 100, display_rsrc);
  rogueviz::rv_hook(hooks_handleKey, 0, handleKey);
  }

void ds_record() {
  #if CAP_VIDEO
  ld full = 1000;
  anims::period = full * history.back().start / DS_(simspeed);
  anims::noframes = anims::period * 60 / 1000;
  dynamicval<bool> b(paused, true);
  int a = addHook(anims::hooks_anim, 100, [&] {
    view_pt = (ticks / full) * DS_(simspeed);
    for(auto& ss: history)
      if(ss.start + ss.duration > view_pt) {
        current = ss.current;
        if(sphere) {
          dynamicval<eGeometry> g(geometry, gSpace435);
          current.T = inverse(ss.at.T * spin(-(ss.ang+90)*degree));
          current.T = lorentz(3, 2, view_pt - ss.start) * current.T;
          }
        else PIA({
          vctr = new_vctr = ss.vctr;
          vctrV = new_vctrV = ss.vctrV;
          current.T = cspin(3, 2, view_pt - ss.start) * current.T;
          if(auto_rotate)
            current.T = cspin(1, 0, view_pt - ss.start) * current.T;
          });
        break;
        }
    });
  anims::record_video_std();
  delHook(anims::hooks_anim, a);
  #endif
  }

auto ds_hooks = 
  arg::add3("-ds-game", run_ds_game)
+ arg::add3("-ds-recenter", [] { current = Id; })
+ arg::add3("-ds-record", ds_record);

}
}
