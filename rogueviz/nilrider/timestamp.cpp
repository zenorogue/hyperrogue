namespace nilrider {

ld timestamp::energy_in_squares() { return vel * vel / (2 * gravity); }

EX ld sym_to_heis_bonus(const hyperpoint& H) {
  return H[0] * H[1] / 2;
  }

/** convert rotationally symmetric to Heisenberg model */
EX hyperpoint sym_to_heis(hyperpoint H) {
  if(nil) {
    H[2] += sym_to_heis_bonus(H);
    }
  return H;
  }

void timestamp::draw_unilcycle(const shiftmatrix& V) {
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
      queuecurve(V * rgpushxto0(where), 0xFFFFFFFF, 0xFFFF40FF, PPR::WALL);
    else
      queuecurve(V * rgpushxto0(where), 0xFFFFFFFF, 0xFF4040FF, PPR::WALL);
    }
  
  if(1) {
    curvepoint(base + Ta * point3(hublen, 0, whrad+hublen));
    curvepoint(base + Ta * point3(-hublen, -hublen, whrad+hublen));
    curvepoint(base + Ta * point3(-hublen, +hublen, whrad+hublen));
    curvepoint(base + Ta * point3(hublen, 0, whrad+hublen));
    queuecurve(V * rgpushxto0(where), 0xFF, 0x303030FF, PPR::WALL);
    
    for(auto& y: {hublen, -hublen}) {
      curvepoint(base + Ta * point3(hublen * .1, -y, 0));
      curvepoint(base + Ta * point3(hublen * -.1, -y, 0));
      curvepoint(base + Ta * point3(hublen * -.1, 0, whrad + hublen / 2));
      curvepoint(base + Ta * point3(hublen * .1, 0, whrad + hublen / 2));
      curvepoint(base + Ta * point3(hublen * .1, -y, 0));
      queuecurve(V * rgpushxto0(where), 0xFF, 0x303030FF, PPR::WALL);

      curvepoint(base + Ta * point3(hublen * -.1, 0, whrad + hublen / 2));
      curvepoint(base + Ta * point3(hublen * .1, 0, whrad + hublen / 2));
      curvepoint(base + Ta * point3(hublen * .1, 0, whrad + hublen));
      curvepoint(base + Ta * point3(hublen * -.1, 0, whrad + hublen));
      curvepoint(base + Ta * point3(hublen * -.1, 0, whrad + hublen / 2));
      queuecurve(V * rgpushxto0(where), 0xFF, 0x303030FF, PPR::WALL);
      }
    }
  }

bool tick_debug = false;

bool timestamp::collect(level *lev) {
  auto xy = lev->get_xy_i(where);
  char ch = lev->mapchar(xy);
  if(ch == 'r' || ch == '!') return false;
  if(ch == '*') {
    for(int i=0; i<isize(lev->triangles); i++) {
      auto& t = lev->triangles[i];
      if(t.x == xy.first && t.y == xy.second) collected_triangles |= (1<<i);
      }
    }

  int gid = 0;
  for(auto& g: lev->goals) {
    bool gfailed = failed & Flag(gid);
    bool gsuccess = goals & Flag(gid);
    if(gfailed || gsuccess) continue;
    checkerparam cp {this, lev, reversals};
    auto res = g.check(cp);
    if(res == grFailed) {
      failed |= Flag(gid);
      }
    else if(res == grSuccess) {
      goals |= Flag(gid);
      lev->current_score[gid] = timer;
      if(planning_mode || !loaded_or_planned) {
        auto &res = lev->records[planning_mode][gid];
        if(res == 0 || timer < res) {
          res = timer;
          println(hlog, "saved -- success on goal ", gid, " in time ", timer);
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

bool timestamp::tick(level *lev) {
  
  if(!collect(lev)) return false;
  const ld eps = slope_eps;
  
  hyperpoint wnext = where;
  wnext[0] += cos(heading_angle) * eps;
  wnext[1] += sin(heading_angle) * eps;
  wnext[2] = lev->surface(wnext);
  
  wnext = gpushxto0(where) * wnext;
  slope = atan(wnext[2] / eps);
  
  auto ovel = vel;
  
  vel -= sin(slope) * gravity / tps;
  if(vel < 0) {
    vel = 0;
    if(ovel == 0) return false;
    }
  
  auto mvel = (vel + ovel) / 2;
  where[0] += cos(heading_angle) * mvel * cos(slope) / tps;
  where[1] += sin(heading_angle) * mvel * cos(slope) / tps;
  where[2] = lev->surface(where);
  circpos += mvel / whrad / tps;
  
  timer += 1. / tps;
  return true;
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
  hyperpoint front = rgpushxto0(w) * sym_to_heis(hyperpoint(1e-3 * cos(heading_angle), 1e-3*sin(heading_angle), 0, 1));
  hyperpoint up = w; up[2] += 1e-3;

  set_view(w, front, up);
  
  transmatrix T = View;
  
  ld gfx_slope = binsearch(-90*degree, min(slope, min_gfx_slope), [&] (ld slope) {
    View = T;
    rotate_view(cspin(1, 2, slope));
    for(int i=0; i<8; i++) {
      shift_view(ztangent(whdist * lev->scale / 8.));
      hyperpoint p = inverse(View) * C0;
      ld room = p[2] - lev->surface(p);
      if(room < .1 * lev->scale) return true;
      for(hyperpoint h: {point3(0,0,0), point3(.001,0,0), point3(-.001,0,0), point3(0,-0.001,0), point3(0,0.001,0)})
        if(lev->mapchar(p+h) == 'r') return true;
      }
    return false;
    }, 10);
  
  View = T;
  rotate_view(cspin(1, 2, gfx_slope));
  shift_view(ztangent(whdist * lev->scale));
  centerover = cwt.at;
  playermoved = false;   
  }

string format_timer(ld t) {
  return format("%d:%02d.%02d", int(t / 60), int(t) % 60, int(frac(t) * 100));
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
  
  auto sId = shiftless(Id);

  ld pix = 1 / (2 * cgi.hcrossf / cgi.crossf);
  
  // clinometer
  
  cx += rad * 1.2;

  for(int i=-90; i<=90; i++)
    curvepoint(atscreenpos(cx+cos(i * degree)*rad, cy-sin(i*degree)*rad, 1) * C0);

  curvepoint(atscreenpos(cx, cy+rad, 1) * C0);
  queuecurve(sId, 0x000000FF, 0xFFFFFF80, PPR::ZERO);
  
  curvepoint(hpxy(0, 0));
  curvepoint(hpxy(rad, 0));
  /* curvepoint(hpxy(rad/4, 0));
  curvepoint(hpxy(0, rad));
  curvepoint(hpxy(-rad/4, 0));  
  curvepoint(hpxy(rad/4, 0)); */
  queuecurve(sId * atscreenpos(cx, cy, pix) * spin(min_gfx_slope), 0x40, 0x40, PPR::ZERO);

  curvepoint(hpxy(rad/4, 0));
  curvepoint(hpxy(0, rad));
  curvepoint(hpxy(-rad/4, 0));  
  curvepoint(hpxy(rad/4, 0));
  queuecurve(sId * atscreenpos(cx, cy, pix) * spin(90._deg + slope), 0xFF, 0x40C040FF, PPR::ZERO);

  // compass
  
  cx -= rad * 1.2;
  
  for(int i=0; i<360; i++)
    curvepoint(atscreenpos(cx-cos(i * degree)*rad, cy-sin(i*degree)*rad, 1) * C0);
  
  queuecurve(sId, 0x000000FF, 0xFFFFFF80, PPR::ZERO);

  for(int d: {1}) {
  
    // d == +1: direction arrow
    // d == -1: compass
  
    curvepoint(hpxy(rad/4, 0));
    curvepoint(hpxy(0, rad));
    curvepoint(hpxy(-rad/4, 0));
    
    queuecurve(sId * atscreenpos(cx, cy, pix) * spin(d * (90*degree + heading_angle)), 0xFF, d > 0 ? 0x0000FFFF : 0xFF0000FF, PPR::ZERO);
  
    curvepoint(hpxy(rad/4, 0));
    curvepoint(hpxy(0, -rad));
    curvepoint(hpxy(-rad/4, 0));
    curvepoint(hpxy(rad/4, 0));
    queuecurve(sId * atscreenpos(cx, cy, pix) * spin(d * (90*degree + heading_angle)), 0xFF, 0xFFFFFFFF, PPR::ZERO);
    }
  
  // speed meter

  cx += rad * 3.4;
  
  for(int i=0; i<360; i++)
    curvepoint(atscreenpos(cx-cos(i * degree)*rad, cy-sin(i*degree)*rad, 1) * C0);
  queuecurve(sId, 0x000000FF, 0xFFFFFF80, PPR::ZERO);  
  
  auto e_to_angle = [] (ld energy) {
    return 135*degree - 3 * atan(energy/10);
    };
  
  vector<ld> short_lines = {2, 3, 4, 6, 7, 8, 9, 30, 40, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  
  for(auto h: short_lines) {
    auto a = e_to_angle(h);
    curvepoint(hpxy(-sin(a)*rad*.95, -cos(a)*rad*.95));
    curvepoint(hpxy(-sin(a)*rad*.85, -cos(a)*rad*.85));
    queuecurve(sId * atscreenpos(cx, cy, pix), 0xFF, 0, PPR::ZERO);
    }

  vector<ld> long_lines = {0, 1, 5, 10, 20, 50};
  
  for(auto h: long_lines) {
    auto a = e_to_angle(h);
    curvepoint(hpxy(-sin(a)*rad*.95, -cos(a)*rad*.95));
    curvepoint(hpxy(-sin(a)*rad*.75, -cos(a)*rad*.75));
    queuecurve(sId * atscreenpos(cx, cy, pix), 0xFF, 0, PPR::ZERO);
    displaystr(cx -sin(a)*rad*.65, cy -cos(a)*rad*.65, 0, 8, its(h), 0, 8);
    }

  curvepoint(hpxy(rad/4, 0));
  curvepoint(hpxy(0, -rad));
  curvepoint(hpxy(-rad/4, 0));
  curvepoint(hpxy(rad/4, 0));
  queuecurve(sId * atscreenpos(cx, cy, pix) * spin(e_to_angle(energy_in_squares())), 0xFF, 0xFF8080FF, PPR::ZERO);
  
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

    queuepoly(sId * atscreenpos(cx+rad/2, cy+(tid&1?1:-1)*rad/3, pix * rad * 1.2) * spin(90*degree), cgi.shTriangle, f);
    tid++;
    if(tid == 2) { tid = 0; cx += rad/1.4; }
    }
  if(tid) cx += rad/1.4;
  cx += 5;

  int gid = 0;
  for(auto& g: l->goals) {
    bool gfailed = failed & Flag(gid);
    bool gsuccess = goals & Flag(gid);
    shiftmatrix T = sId * atscreenpos(cx+rad/2, cy+(gid-1)*rad/1.2, pix * rad * 1.2);
    poly_outline = 0xFF; color_t f = darkena(g.color, 0, 0xFF);
    if(gsuccess) {
      queuepoly(T * spin(90*degree), cgi.shGrail, f);
      displaystr(cx+rad, cy+(gid-1)*rad/1.2, 0, vid.fsize*.75, format_timer(l->current_score[gid]), 0, 0);
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
  else if(reversals) s = format("+%d", reversals);
  else return;
  displaystr(vid.xres - vid.fsize, vid.fsize*4, 0, vid.fsize, s, 0, 16);
  }

}
