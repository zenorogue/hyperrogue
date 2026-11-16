#include "../hyper.h"

namespace hr {
namespace dandelin {

int dandelin = 1;
ld dandelin_h = 1.2;
ld dandelin_angle = 0;

int dandelin_parts = 7;

eModel mdDandelin;

void add_projection() {
  int q = isize(mdinf);
  mdDandelin = eModel(q);
  eModel p = pmodel;
  auto mo = mdinf[mdHemisphere];
  mo.name_hyperbolic = "Dandelin spheres";
  mo.name_euclidean = "not implemented";
  mo.name_spherical = "not implemented";
  mo.flags |= mf::hyper_only;
  mdinf.push_back(mo);
  while(isize(extra_projections) < q) extra_projections.emplace_back();
  extra_projections.emplace_back([p] (shiftpoint& H_orig, hyperpoint& H, hyperpoint& ret) {
    if(pconf.small_hyperboloid) H = mid(C0, H);
    ld zl = zlevel(H);
    ret = H / H[2];
    ret[2] = sqrt(1 - sqhypot_d(2, ret));
    ret = ret * (1 + (zl - 1) * ret[2] * pconf.depth_scaling);
    if(among(dandelin, 2, 4)) {
      ret[2] -= dandelin_h;
      }
    if(dandelin == 3) {
      ret[2] = - (spin(dandelin_angle) * ret)[0] * sqrt(pow(dandelin_h, 2) - 1);
      }
    if(among(dandelin, 4, 5)) {
      ret[2] *= -1;
      }
    swap(ret[1], ret[2]);      
    ret = pconf.ball() * ret;
    });
  pmodel = mdDandelin;
  }

int get_side(const hyperpoint& H) {
  if(pmodel != mdDandelin) return 0;
  if(dandelin == 3) return 1;
  hyperpoint res;
  if(dandelin == 2) {
    dynamicval<int> d(dandelin, 0);
    applymodel(shiftless(H), res);
    return res[2] < 0 ? 1 : -1;
    }
  if(dandelin == 4) {
    dynamicval<int> d(dandelin, 5);
    applymodel(shiftless(H), res);
    return res[2] < 0 ? 1 : -1;
    }
  return 0;
  }

bool draw_main() {
  if(pmodel != mdDandelin) return false;
  if(dandelin != 1) return false;
  indenter ind(2);
  dandelin = 4;
  if(dandelin_parts & 1) hr::draw_main();
  dandelin = 3;
  if(dandelin_parts & 2) hr::draw_main();
  dandelin = 2;
  if(dandelin_parts & 4) hr::draw_main();
  dandelin = 1;
  return true;
  }

bool draw_boundary(int w) {
  if(pmodel != mdDandelin) return false;
  if(w == 1) return true;
  dynamicval<ld> lw(vid.linewidth, vid.linewidth * vid.multiplier_ring * (svg::in ? svg::divby : 1));
  color_t lc = ringcolor;
  color_t fc = modelcolor;
  
  auto d = deconstruct_ball();

  auto p = PPR::CIRCLE;
  for(int s=0; s<5; s++) {
    queuereset(mdPixel, p);
    for(int i=0; i<=360; i++) {
      ld c1 = cos(i * degree);
      ld s1 = sin(i * degree);
      if(s == 0)
        curvepoint(point3(current_display->radius * c1, current_display->radius * (s1 * d.sin_beta - (c1 * cos(dandelin_angle) - s1 * sin(dandelin_angle)) * d.cos_beta * sqrt(pow(dandelin_h,2)-1)), 0));
      if(s == 1)
        curvepoint(point3(current_display->radius * c1, current_display->radius * (s1 * d.sin_beta - dandelin_h * d.cos_beta), 0));
      if(s == 2)
        curvepoint(point3(current_display->radius * c1, current_display->radius * (s1 * d.sin_beta + dandelin_h * d.cos_beta), 0));
      auto z = d.cos_beta * s1 >= 0 - 1e-6 ? 1 : abs(d.sin_beta);
      if(s == 3)
        curvepoint(point3(current_display->radius * c1, current_display->radius * (-dandelin_h * d.cos_beta + s1 * z), 0));
      if(s == 4)
        curvepoint(point3(current_display->radius * c1, current_display->radius * (+dandelin_h * d.cos_beta - s1 * z), 0));
      /* if(s == 1)
        curvepoint(point3(current_display->radius * c1, current_display->radius * s1 * (d.cos_beta * s1 >= 0 - 1e-6 ? 1 : abs(d.sin_beta)), 0)); */
      }
    queuecurve(shiftless(d.talpha), lc, fc, p);
    if(s == 4) {
      curvepoint(point3(current_display->radius * 1, +current_display->radius * dandelin_h * d.cos_beta, 0));
      curvepoint(point3(current_display->radius * 1, -current_display->radius * dandelin_h * d.cos_beta, 0));
      queuecurve(shiftless(d.talpha), lc, fc, p);
      curvepoint(point3(current_display->radius * -1, +current_display->radius * dandelin_h * d.cos_beta, 0));
      curvepoint(point3(current_display->radius * -1, -current_display->radius * dandelin_h * d.cos_beta, 0));
      queuecurve(shiftless(d.talpha), lc, fc, p);
      }
    queuereset(pmodel, p);
    }

  return true;
  }

EX int ad = arg::add3("-dandelin", [] {
  dandelin = 1;
  add_projection();
  addHook(hooks_get_side, 100, get_side);
  addHook(hooks_draw_main, 100, draw_main);
  addHook(hooks_draw_boundary, 100, draw_boundary);
  addHook(hooks_two_sided_model, 100, [] { if(pmodel == mdDandelin) return 1; return 0; });
  pmodel = mdDandelin;
  param_f(dandelin_h, "dandelin_h");
  param_f(dandelin_angle, "dandelin_angle");
  param_i(dandelin_parts, "dandelin_parts");
  });

}
}
