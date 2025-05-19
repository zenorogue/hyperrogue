namespace nilrider {

using hr::lerp;

ld timestamp::energy_in_squares() { return vel * vel / (2 * gravity); }

/** convert rotationally symmetric to internal model */
EX hyperpoint sym_to_used(hyperpoint H) {
  if(nil) nilv::convert_ref(H, nilv::nmSym, nilv::model_used);
  return H;
  }

void timestamp::draw_unilcycle(const shiftmatrix& V, const colorscheme& cs) {
  const int points = 60 / (1 + reduce_quality);
  const int spoke_each = 5;
  hyperpoint whpoint[points+1];
  
  transmatrix Ta = cspin(0, 1, -heading_angle);
  transmatrix Tb = cspin(0, 2, -slope);
  
  hyperpoint base = Ta * Tb * point31(0, 0, whrad);
  
  for(int a=0; a<points; a++) {
    ld beta = TAU * a / points + circpos;
    whpoint[a] = base + Ta * point3(whrad*sin(beta),0,whrad*cos(beta));
    }
  whpoint[points] = whpoint[0];
  
  hyperpoint hub[2];
  const ld hublen = whrad / 2;
  for(int a=0; a<2; a++) {
    hub[a] = base + Ta * point3(0, hublen*(a?1:-1), 0);
    }
  
  for(int a=0; a<points; a+=spoke_each) for(int b=0; b<2; b++) {
    curvepoint(hub[b]);
    for(int b=0; b<=spoke_each; b++)
      curvepoint(whpoint[a+b]);
    curvepoint(hub[b]);
    if(a&1)
      queuecurve(V * rgpushxto0(where), 0xFFFFFFFF, cs.wheel1, PPR::WALL);
    else
      queuecurve(V * rgpushxto0(where), 0xFFFFFFFF, cs.wheel2, PPR::WALL);
    }
  
  if(1) {
    curvepoint(base + Ta * point3(hublen, 0, whrad+hublen));
    curvepoint(base + Ta * point3(-hublen, -hublen, whrad+hublen));
    curvepoint(base + Ta * point3(-hublen, +hublen, whrad+hublen));
    curvepoint(base + Ta * point3(hublen, 0, whrad+hublen));
    queuecurve(V * rgpushxto0(where), 0xFF, cs.seat, PPR::WALL);
    
    for(auto& y: {hublen, -hublen}) {
      curvepoint(base + Ta * point3(hublen * .1, -y, 0));
      curvepoint(base + Ta * point3(hublen * -.1, -y, 0));
      curvepoint(base + Ta * point3(hublen * -.1, 0, whrad + hublen / 2));
      curvepoint(base + Ta * point3(hublen * .1, 0, whrad + hublen / 2));
      curvepoint(base + Ta * point3(hublen * .1, -y, 0));
      queuecurve(V * rgpushxto0(where), 0xFF, cs.seatpost, PPR::WALL);

      curvepoint(base + Ta * point3(hublen * -.1, 0, whrad + hublen / 2));
      curvepoint(base + Ta * point3(hublen * .1, 0, whrad + hublen / 2));
      curvepoint(base + Ta * point3(hublen * .1, 0, whrad + hublen));
      curvepoint(base + Ta * point3(hublen * -.1, 0, whrad + hublen));
      curvepoint(base + Ta * point3(hublen * -.1, 0, whrad + hublen / 2));
      queuecurve(V * rgpushxto0(where), 0xFF, cs.seatpost, PPR::WALL);
      }
    }
  }

bool tick_debug = false;

bool timestamp::out_of_surface(level *lev) {
  auto xy = lev->get_xy_i(where);
  char ch = lev->mapchar(xy);
  return ch == '!';
  }

bool timestamp::collect(level *lev) {
  auto xy = on_surface->get_xy_i(where);
  char ch = on_surface->mapchar(xy);
  if(ch == 'r') return false;
  if(ch == '*') {
    for(int i=0; i<isize(lev->triangles); i++) {
      auto& t = lev->triangles[i];
      if(t.which != on_surface) continue;
      if(t.x == xy.first && t.y == xy.second) collected_triangles |= (1<<i);
      }
    }

  int gid = 0;
  for(auto& g: lev->goals) {
    bool gfailed = failed & Flag(gid);
    bool gsuccess = goals & Flag(gid);
    if(gfailed || gsuccess) { gid++; continue; }
    checkerparam cp {this, lev, reversals};
    auto res = g.check(cp);
    if(res == grFailed) {
      failed |= Flag(gid);
      }
    else if(res == grSuccess) {
      goals |= Flag(gid);
      auto& score = lev->current_score[gid];
      score = timer;
      if(lev->flags & nrlJumping) score = -where[0];
      if(planning_mode || !loaded_or_planned) {
        auto &res = lev->records[planning_mode][gid];
        if(res == 0 || score < res) {
          res = score;
          save();
          }
        }
      }
    gid++;
    }

  return true;
  }

/* convert heading to integral units, to make saved replays consistent */

constexpr ld h_units = 360 * 60 * 60;
constexpr ld h_mul = h_units / TAU;

/* set to flyvel[2] in the case of crash from below */
constexpr ld CRASHED_FROM_BELOW = -147;

int heading_to_int(ld a) {
  a = a * h_mul;
  int ai = floor(a + .5);
  ai = gmod(ai, h_units);
  return ai;
  }

ld int_to_heading(ld a) {
  return a / h_mul;
  }

void timestamp::be_consistent() {
  heading_angle = int_to_heading(heading_to_int(heading_angle));
  }

bool timestamp::tick(level *lev, ld time_left) {
  
  if(flyvel[2] == CRASHED_FROM_BELOW) return false;
  if(on_surface && !collect(lev)) return false;
  const ld eps = slope_eps;
  
  if(on_surface) {
    hyperpoint wnext = where;
    wnext[0] += cos(heading_angle) * eps;
    wnext[1] += sin(heading_angle) * eps;
    wnext[2] = on_surface->surface(wnext);

    wnext = gpushxto0(where) * wnext;
    slope = atan(wnext[2] / eps);

    if(out_of_surface(lev)) {
      on_surface = nullptr;
      sstime = timer; chg_slope = gfx_slope;
      flyvel = wnext * vel / hypot_d(3, wnext);
      flyvel[3] = 0;
      }
    }

  timer += time_left;

  if(on_surface) {
    auto ovel = vel;

    vel -= sin(slope) * gravity * time_left;
    if(vel < 0) {
      vel = 0;
      if(ovel == 0) return false;
      }

    auto mvel = (vel + ovel) / 2;
    where[0] += cos(heading_angle) * mvel * cos(slope) * time_left;
    where[1] += sin(heading_angle) * mvel * cos(slope) * time_left;
    where[2] = on_surface->surface(where);
    circvel = mvel / whrad;
    }

  else {
    auto owhere = where;
    auto oflyvel = flyvel;
    flyvel = rgpushxto0(where) * flyvel;
    flyvel[2] -= gravity * time_left / 2;

    // todo rewrite geodesic_step to take gravity into account into RK4 correctly
    flyvel *= time_left;
    nisot::geodesic_step(where, flyvel);
    flyvel /= time_left;

    flyvel[2] -= gravity * time_left / 2;
    auto mflyvel = (flyvel + oflyvel) / 2;
    auto new_heading_angle = atan2(mflyvel[1], mflyvel[0]);
    if(timer >= last_tramp + 0.5) heading_angle = new_heading_angle;
    else {
      while(new_heading_angle < heading_angle - M_PI) new_heading_angle += TAU;
      while(new_heading_angle > heading_angle + M_PI) new_heading_angle -= TAU;
      heading_angle = lerp(heading_angle, new_heading_angle, ilerp(timer - time_left, last_tramp + 0.5, timer));
      }

    flyvel = gpushxto0(where) * flyvel;
    mflyvel = gpushxto0(where) * mflyvel;
    slope = atan(mflyvel[2] / hypot_d(2, mflyvel));

    vel = hypot_d(3, flyvel);

    if(check_crashes_rec(lev, owhere, oflyvel, time_left)) return false;
    }

  circpos += circvel * time_left;

  return true;
  }

bool timestamp::check_crashes(level* lev, hyperpoint owhere, hyperpoint oflyvel, ld time_left) {
  ld oz = lev->surface(owhere);
  ld z = lev->surface(where);

  if(owhere[2] < oz && where[2] >= z) {
    auto xy = lev->get_xy_i(where);
    char ch = lev->mapchar(xy);
    if(ch != '!') { flyvel[2] = CRASHED_FROM_BELOW; return true; }
    }

  if(owhere[2] > oz && where[2] <= z) {

    auto xy = lev->get_xy_i(where);
    char ch = lev->mapchar(xy);
    if(ch == '!') return false;

    string s0 = ""; s0 += ch;

    ld part = binsearch(0, 1, [&] (ld p) {
      hyperpoint h = lerp(owhere, where, p);
      return h[2] < lev->surface(h);
      });

    timer -= time_left * (1 - part);

    where = lerp(owhere, where, part);
    flyvel = lerp(oflyvel, flyvel, part);

    /* tangent vectors */
    hyperpoint dx = gpushxto0(where) * lev->surface_point(rgpushxto0(where) * point31(slope_eps, 0, 0));
    hyperpoint dy = gpushxto0(where) * lev->surface_point(rgpushxto0(where) * point31(0, slope_eps, 0));
    hyperpoint dz = point30(0, 0, slope_eps);

    /* orthonormalize */
    dx = dx / hypot_d(3, dx);
    dy = dy - dot_d(3, dx, dy) * dy;
    dy = dy / hypot_d(3, dy);
    dz = dz - dot_d(3, dx, dz) * dx;
    dz = dz - dot_d(3, dy, dz) * dy;
    dz = dz / hypot_d(3, dz); dz[3] = 0;

    if(ch == 'T') {
      /* reflect off the trampoline */
      flyvel = flyvel - dot_d(3, flyvel, dz) * dz * 2;
      where[2] = lev->surface(where) + 1e-4;
      last_tramp = timer;
      tramp_head = heading_angle;
      }

    else if(ch == 'V') {
      /* convert velocity on velocity converter */
      vel = hypot_d(3, flyvel);
      on_surface = lev;
      }

    else {
      /* waste some energy */
      flyvel = flyvel - dot_d(3, flyvel, dz) * dz;
      vel = hypot_d(3, flyvel);
      on_surface = lev;
      }

    if(part == 1) return false;
    return !tick(lev, time_left * (1 - part));
    }
  return false;
  }

bool timestamp::check_crashes_rec(level* l, hyperpoint owhere, hyperpoint oflyvel, ld time_left) {
  if(check_crashes(l, owhere, oflyvel, time_left)) return true;
  for(auto s: l->sublevels) if(check_crashes(s, owhere, oflyvel, time_left)) return true;
  return false;
  }

void timestamp::centerview(level *lev) {
  // static bool once = false; if(once) return; once = true;

  if(vrhr::active()) {
    transmatrix Ta = cspin(0, 1, -heading_angle);
    transmatrix Tb = cspin(0, 2, -slope);

    hyperpoint base = Ta * Tb * point31(0, 0, whrad);
    hyperpoint refpoint = rgpushxto0(where) * rgpushxto0(base) * point31(0, 0, whrad*3);
    centerover = cwt.at; playermoved = false;
    View = cspin(0, 2, heading_angle-90*degree) * cspin(1, 2, -90*degree) * gpushxto0(refpoint);
    return;
    }

  auto w = where;
  w[2] += 0.2 * lev->scale;
  hyperpoint front = rgpushxto0(w) * sym_to_used(hyperpoint(1e-3 * cos(heading_angle), 1e-3*sin(heading_angle), 0, 1));
  hyperpoint up = w; up[2] += 1e-3;

  set_view(w, front, up);
  
  transmatrix T = View;

  if(last_draw <= sstime) min_gfx_slope = gfx_slope;
  
  gfx_slope = min_gfx_slope;
  if(on_surface) gfx_slope = binsearch(-90*degree, min(slope, min_gfx_slope), [&] (ld slope) {
    View = T;
    rotate_view(cspin(1, 2, slope));
    for(int i=0; i<8; i++) {
      shift_view(ztangent(whdist * lev->scale / 8.));
      hyperpoint p = inverse(View) * C0;
      ld room = p[2] - on_surface->surface(p);
      if(room < .1 * lev->scale) return true;
      for(hyperpoint h: {point3(0,0,0), point3(.001,0,0), point3(-.001,0,0), point3(0,-0.001,0), point3(0,0.001,0)})
        if(lev->mapchar(p+h) == 'r') return true;
      }
    return false;
    }, 10);

  if(timer < sstime + 1) {
    ld t = timer - sstime;
    gfx_slope = lerp(chg_slope, gfx_slope, t * t * (3 - 2*t));
    }

  last_draw = timer;

  View = T;
  rotate_view(cspin(1, 2, gfx_slope));
  shift_view(ztangent(whdist * lev->scale));
  centerover = cwt.at;
  playermoved = false;   
  }

string format_timer(ld t) {
  return hr::format("%d:%02d.%02d", int(t / 60), int(t) % 60, int(frac(t) * 100));
  }

string format_timer_goal(ld t, const goal& g, bool plan) {
  int stars = g.sa(t) * (plan ? 1 : 2);
  string s;
  if(t > 0) s = format_timer(t);
  else s = hr::format("%.02fm", -t);
  return s + " (" + its(stars) + " stars)";
  }

void timestamp::draw_instruments(level* l) {
  dynamicval<eGeometry> g(geometry, gEuclid);
  dynamicval<eModel> pm(pmodel, mdDisk);
  dynamicval<bool> ga(vid.always3, false);
  dynamicval<color_t> ou(poly_outline);
  dynamicval<geometryinfo1> gi(ginf[gEuclid].g, giEuclid2);
  initquickqueue();
  check_cgi(); cgi.require_shapes();
  
  ld rad = 40;

  ld cx = rad * 2;
  ld cy = rad * 2;
  
  // clinometer
  
  cx += rad * 1.2;

  auto ASP = atscreenpos(0, 0);

  for(int i=-90; i<=90; i++)
    curvepoint(eupoint(cx+cos(i * degree)*rad, cy-sin(i*degree)*rad));

  curvepoint(eupoint(cx, cy+rad));
  queuecurve(ASP, 0x000000FF, 0xFFFFFF80, PPR::ZERO);
  
  curvepoint(eupoint(0, 0));
  curvepoint(eupoint(rad, 0));
  queuecurve(atscreenpos(cx, cy) * spin(min_gfx_slope), 0x40, 0x40, PPR::ZERO);

  curvepoint(eupoint(rad/4, 0));
  curvepoint(eupoint(0, rad));
  curvepoint(eupoint(-rad/4, 0));
  curvepoint(eupoint(rad/4, 0));
  queuecurve(atscreenpos(cx, cy) * spin(90._deg + slope), 0xFF, 0x40C040FF, PPR::ZERO);

  // compass
  
  cx -= rad * 1.2;
  
  for(int i=0; i<360; i++)
    curvepoint(eupoint(cx-cos(i * degree)*rad, cy-sin(i*degree)*rad));
  
  queuecurve(ASP, 0x000000FF, 0xFFFFFF80, PPR::ZERO);

  for(int d: {1}) {
  
    // d == +1: direction arrow
    // d == -1: compass
  
    curvepoint(eupoint(rad/4, 0));
    curvepoint(eupoint(0, rad));
    curvepoint(eupoint(-rad/4, 0));
    
    queuecurve(atscreenpos(cx, cy) * spin(d * (90*degree + heading_angle)), 0xFF, d > 0 ? 0x0000FFFF : 0xFF0000FF, PPR::ZERO);
  
    curvepoint(eupoint(rad/4, 0));
    curvepoint(eupoint(0, -rad));
    curvepoint(eupoint(-rad/4, 0));
    curvepoint(eupoint(rad/4, 0));
    queuecurve(atscreenpos(cx, cy) * spin(d * (90*degree + heading_angle)), 0xFF, 0xFFFFFFFF, PPR::ZERO);
    }
  
  // speed meter

  cx += rad * 3.4;
  
  for(int i=0; i<360; i++)
    curvepoint(eupoint(cx-cos(i * degree)*rad, cy-sin(i*degree)*rad));
  queuecurve(ASP, 0x000000FF, 0xFFFFFF80, PPR::ZERO);
  
  auto e_to_angle = [] (ld energy) {
    return 135*degree - 3 * atan(energy/10);
    };
  
  vector<ld> short_lines = {2, 3, 4, 6, 7, 8, 9, 30, 40, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  
  for(auto h: short_lines) {
    auto a = e_to_angle(h);
    curvepoint(eupoint(-sin(a)*rad*.95, -cos(a)*rad*.95));
    curvepoint(eupoint(-sin(a)*rad*.85, -cos(a)*rad*.85));
    queuecurve(atscreenpos(cx, cy), 0xFF, 0, PPR::ZERO);
    }

  vector<ld> long_lines = {0, 1, 5, 10, 20, 50};
  
  for(auto h: long_lines) {
    auto a = e_to_angle(h);
    curvepoint(eupoint(-sin(a)*rad*.95, -cos(a)*rad*.95));
    curvepoint(eupoint(-sin(a)*rad*.75, -cos(a)*rad*.75));
    queuecurve(atscreenpos(cx, cy), 0xFF, 0, PPR::ZERO);
    displaystr(cx -sin(a)*rad*.65, cy -cos(a)*rad*.65, 0, 8, its(h), 0, 8);
    }

  curvepoint(eupoint(rad/4, 0));
  curvepoint(eupoint(0, -rad));
  curvepoint(eupoint(-rad/4, 0));
  curvepoint(eupoint(rad/4, 0));
  queuecurve(atscreenpos(cx, cy) * spin(e_to_angle(energy_in_squares())), 0xFF, 0xFF8080FF, PPR::ZERO);
  
  cx += rad;

  int tid = 0;

  for(int i=0; i<isize(l->triangles); i++) {
    bool have = collected_triangles & Flag(i);
    color_t f = l->triangles[i].colors[6];
    if(have) {
      poly_outline = 0xFF;
      }
    else {
      poly_outline = f;
      f = 0x40;
      }

    queuepoly(atscreenpos(cx+rad/2, cy+(tid&1?1:-1)*rad/3) * euscalexx(rad * 1.2) * spin(90*degree), cgi.shTriangle, f);
    tid++;
    if(tid == 2) { tid = 0; cx += rad/1.4; }
    }
  if(tid) cx += rad/1.4;
  cx += 5;

  int gid = 0;
  for(auto& g: l->goals) {
    bool gfailed = failed & Flag(gid);
    bool gsuccess = goals & Flag(gid);
    shiftmatrix T = atscreenpos(cx+rad/2, cy+(gid-1)*rad/1.2) * euscalexx(rad * 1.2);
    poly_outline = 0xFF; color_t f = darkena(g.color, 0, 0xFF);
    if(gsuccess) {
      queuepoly(T * spin(90*degree), cgi.shGrail, f);
      displaystr(cx+rad, cy+(gid-1)*rad/1.2, 0, vid.fsize*.75, format_timer_goal(l->current_score[gid], g, planning_mode), 0, 0);
      }
    else {
      poly_outline = f; f = 0x40;
      queuepoly(T * spin(90*degree), cgi.shGrail, f);
      if(gfailed) { poly_outline = 0xFF; queuepoly(T, cgi.shPirateX, 0xC00000FF); }
      }
    gid++;
    }

  quickqueue();
  glflush();
  
  displaystr(vid.xres - vid.fsize, vid.fsize*2, 0, vid.fsize * 2, format_timer(timer), 0, 16);

  string s;
  if(loaded_or_planned) s = "R";
  else if(reversals) s = hr::format("+%d", reversals);
  else return;
  displaystr(vid.xres - vid.fsize, vid.fsize*4, 0, vid.fsize, s, 0, 16);
  }

}
