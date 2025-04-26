#include "hyper.h"

namespace hr {

EX namespace gp {

#if HDR
enum class subdivision { linear, equalarea, conformal, gnomonic_vertex, gnomonic_face, equaldist };
#endif

EX subdivision su = subdivision::conformal;

pair<ld, ld> compute_all_areas(bool recolor) {
  map<ld, int> qty_of;
  map<ld, int> qty_of_standard;
  ld totar = 0;

  map<cell*, ld> areas;

  int gt = S3 == 3 ? 6 : 4;

  for(cell *c: currentmap->allcells()) {
    vector<hyperpoint> hs;
    int t = c->type;
    for(int i=0; i<t; i++) hs.push_back(currentmap->get_corner(c, i));
    hs.push_back(hs[0]);

    ld area = abs(compute_area(hs));
    totar += area;

    qty_of[area]++;
    areas[c] = area;
    if(t == gt) qty_of_standard[area]++;
    }

  if(qty_of_standard.empty()) qty_of_standard = qty_of;
  if(qty_of.empty()) return {0, 0};

  auto e = qty_of.end(); e--;
  ld mina = qty_of.begin()->first;
  ld maxa = e->first;

  auto e1 = qty_of_standard.end(); e1--;
  ld mina1 = qty_of_standard.begin()->first;
  ld maxa1 = e1->first;

  if(recolor) for(auto& p: areas) if(c->land == laCanvas) p.firs5t->landparam = gradient(0xFF0000, 0x0000FF, mina, p.second, maxa);

  return {maxa/mina, maxa1/mina1};
  }

constexpr int qval = 8;

array<ld, qval> vals;

bool cornmul(const transmatrix& corners, const hyperpoint& c, hyperpoint& h) {

  if(su == subdivision::gnomonic_face) {
    auto co1 = kleinize(get_column(corners, 1));
    auto co2 = kleinize(get_column(corners, 2));
    h = c[0] * C0 + c[1] * co1 + c[2] * co2;
    return true;
    }

  if(among(su, subdivision::gnomonic_vertex, subdivision::equaldist)) {
    // to do: for gp_style = false */
    int sw = c[2] > c[1] ? -1 : 1;

    auto x = 1 - c[1] - c[2];
    auto y = 1 - (c[1] - c[2]) * sw;

    auto co1 = get_column(corners, 1);
    auto co2 = get_column(corners, 2);
    auto& co = sw == 1 ? co1 : co2;
    // println(hlog, tuple(intval(co1, Hypc), intval(co2, Hypc), hdist0(co1), hdist0(co2)));
    auto T = gpushxto0(co);
    auto dx = kleinize(T * mid(co1, co2));
    auto dxy = kleinize(T * get_column(corners, 0));

    if(su == subdivision::equaldist) {
      ld lx = hypot_d(2, dx), ly = hypot_d(2, dxy-dx);
      x = tan_auto(x * atan_auto(lx)) / lx;
      y = tan_auto(y * atan_auto(ly)) / ly;
      }

    hyperpoint hres = C0 + (dx-C0) * y + (dxy-dx) * x;

    h = iso_inverse(T) * hres;
    return true;
    }

  hyperpoint u;
  u[0] = c[0] * (vals[5] + vals[0] * (c[2] + c[1]) + vals[1] * c[1] * c[2]);
  u[1] = c[1] * (vals[6] + vals[2] * c[2] + vals[3] * c[0] + vals[4] * c[0] * c[2]);
  u[2] = c[2] * (vals[7] + vals[2] * c[1] + vals[3] * c[0] + vals[4] * c[0] * c[1]);
  u[3] = 0;

  h = corners * u;
  return true;
  }

hyperpoint compute(hyperpoint c, const transmatrix& T) {
  hyperpoint h;
  cornmul(T, c, h);
  return normalize(h);
  }

ld get_ratio(const transmatrix& T) {
  ld min_area = 1e6, max_area = 0;
  for(int a=0; a<=10; a++)
  for(int b=0; b<=10-a; b++) {
    hyperpoint h;
    h[1] = a/10.; h[2] = b/10.; h[0] = 1-h[1]-h[2];
    ld eps = 1e-3;
    auto r0 = compute(h, T);
    h[0] += eps; h[2] -= eps;
    auto r1 = compute(h, T);
    h[0] -= eps; h[1] += eps;
    auto r2 = compute(h, T);
    h[1] -= eps; h[2] += eps;
    if(hyperbolic) { transmatrix T = gpushxto0(r0); r0 = C0; r1 = T * r1; r2 = T * r2; }
    ld area = hypot_d(3, (r1-r0) ^ (r2-r0));
    if(a == 0 && b == 0) area *= 1;
    if(area < min_area) min_area = area;
    if(area > max_area) max_area = area;
    }
  return max_area / min_area;
  }

ld get_conf_error(const transmatrix& T) {
  ld err = 0;
  int tot = 0;
  for(int a=0; a<=10; a++)
  for(int b=0; b<=10-a; b++) if(a+b) {
    hyperpoint h;
    h[1] = a/10.; h[2] = b/10.; h[0] = 1-h[1]-h[2];
    ld eps = 1e-3;
    auto r0 = compute(h, T);
    h[1] += eps; h[0] -= eps;
    auto r1 = compute(h, T);
    h[1] -= eps; h[0] += eps;

    
    if(S3 == 4) { h[2] -= eps; h[0] += eps; }
    else { ld v = 2 / sqrt(3); h[2] += eps * v; h[1] -= eps * v/2; h[0] -= eps * v/2; }
    auto r2 = compute(h, T);

    transmatrix T = gpushxto0(r0); r0 = C0; r1 = T * r1 - r0; r2 = T * r2 - r0;

    ld ax = r1[0], ay = r1[1];
    ld bx = r2[0], by = r2[1];

    ld alpha = atan2(-(2*ax*bx+2*ay*by), (-ax*ax-ay*ay+bx*bx+by*by)) / 2;

    ld d1 = hypot(ax * cos(alpha) + bx * sin(alpha), ay * cos(alpha) + by * sin(alpha));
    ld d2 = hypot(ax * sin(alpha) - bx * cos(alpha), ay * sin(alpha) - by * cos(alpha));

    tot++;

    err += (d1/d2) + (d2/d1) - 2; // exit(1);
    }
  return err / tot;
  }

ld zerofun(const transmatrix& T) { return 0; }

void init_cornmul(const transmatrix& T) {

  for(auto& v: vals) v = 0;
  if(qval == 8) vals[5] = vals[6] = vals[7] = 1;
  else vals[0] = vals[1] = vals[2] = 1;

  auto chk = zerofun;
  if(su == subdivision::conformal) chk = get_conf_error;
  if(su == subdivision::equalarea) chk = get_ratio;

  auto cur = chk(T);

  for(int it=0; it<500; it++) {
    array<ld, qval> gra;
    ld sq = 0;
    for(int i=0; i<qval; i++) {
      vals[i] += 1e-4;
      gra[i] = cur - chk(T);
      vals[i] -= 1e-4;
      sq += gra[i] * gra[i];
      }
    if(!sq) break;
    sq = 1e-6 / sqrt(sq);

    while(true) {
      for(int i=0; i<qval; i++) vals[i] += sq * gra[i];
      auto cur1 = chk(T);
      if(cur1 < cur) { cur = cur1; sq *= 2; }
      else { for(int i=0; i<qval; i++) vals[i] -= sq * gra[i]; break; }
      }
    }
  }

void config_gpsubs() {
  addHook(gp::hooks_init_cornmul, 100, init_cornmul);
  // not sure why C++ could not deduce the template parameter
  using t = bool(const transmatrix&, const hyperpoint&, hyperpoint&);
  addHook<t,t> (gp::hooks_cornmul, 100, cornmul);

  param_enum(su, parameter_names("gp_sub", "gp_sub"), subdivision::equalarea)
    ->editable({{"linear", "straight lines remain straight"}, {"equal-area", "try to have tiles of equal area (tiles on corners will still be different)"}, {"conformal", "try to get regularly shaped tiles"}, {"gnomonic (vertex)", "regular in gnomonic projection (centered on pure vertex)"}, {"gnomonic (face)", "regular in gnomonic projection (centered on pure face)"}, {"tangent adjustment", "equal distances on the original edges"}}, "Goldberg mapping", 'S')
    ->add_extra([] {
       auto p = compute_all_areas(false);
       dialog::addSelItem(XLAT("cell area ratio (max/min)"), fts(p.first), 'X');
       dialog::add_action([] { compute_all_areas(true); });
       dialog::addSelItem(S3 == 3 ? XLAT("hex only") : XLAT("square only"), fts(p.second), 'Y');
       dialog::add_action([] { compute_all_areas(true); });
       })
    ->reaction = [] { if(game_active) init_cornmul(dir_matrix(0)); };
  }

auto hookc = addHook(hooks_configfile, 100, config_gpsubs) + arg::add3("-gbs-debug", [] {
  // should have more configuration, but no need for it at the moment
  addHook(hooks_frame, 100, [] {
    vid.linewidth *= 3;
    for(int a=0; a<S7; a++) {
      transmatrix T = dir_matrix(a);
      shiftmatrix S = ggmatrix(cwt.at);
      int z = 5;
      for(int ix=0; ix<z; ix++)
      for(int iy=0; iy<z-ix; iy++) {
        ld x = ix * 1./z, y = iy*1./z, x1 = (ix+1.)/z, y1 = (iy+1.)/z;
        queueline(S * normalize(cornmul(T, point3(1-x-y,x,y))), S * normalize(cornmul(T, point3(1-x1-y,x1,y))), 0x80FF80FF);
        queueline(S * normalize(cornmul(T, point3(1-x-y,x,y))), S * normalize(cornmul(T, point3(1-x-y1,x,y1))), 0x80FF80FF);
        queueline(S * normalize(cornmul(T, point3(1-x-y,x,y))), S * normalize(cornmul(T, point3(1-x1-y1,x1,y1))), 0xFF80FFFF);
        queueline(S * normalize(cornmul(T, point3(1-x1-y,x1,y))), S * normalize(cornmul(T, point3(1-x-y1,x,y1))), 0xFF80FFFF);

        hyperpoint c = point3(1-x-y,x,y);
        hyperpoint h = normalize(cornmul(T, c));
        
        if(a == 0) {
          int sw = c[2] > c[1] ? -1 : 1;
          auto x = 1 - c[1] - c[2];
          auto y = 1 - (c[1] - c[2]) * sw;
          queuestr(S * rgpushxto0(h), 0.4, format("%.2f %.2f", x, y), 0xFFFFFFFF, 2);
          }
        }
      }
    vid.linewidth /= 3;
    });
  }) + arg::add3("-gbs-ofc", [] {
    start_game();
    View = Id;
    rotate_view(xpush(-cgi.hcrossf));
    if(S3 == 4) rotate_view(spin(45._deg));
    playermoved = false;
    }) + arg::add3("-gbs-areas", [] { println(hlog, "areas = ", compute_all_areas(true)); });
    

}
}