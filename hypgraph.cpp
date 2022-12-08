// Hyperbolic Rogue -- hyperbolic graphics
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file hypgraph.cpp
 *  \brief mapping hyperpoints to the screen, and related functions
 */

#include "hyper.h"
namespace hr {

hyperpoint ghxy, ghgxy;
shiftpoint ghpm = shiftless(C02);

EX ld flip_limit = 1.1;

EX bool flip_sphere() { return GDIM == 2 && sphere && pconf.alpha > flip_limit; }

EX bool sphere_flipped;

void ghcheck(hyperpoint &ret, const shiftpoint &H) {
  if(hypot_d(2, ret-ghxy) < hypot_d(2, ghgxy-ghxy)) {
    ghpm = H; ghgxy = ret;
    }
  }

EX void camrotate(ld& hx, ld& hy) {
  ld cam = pconf.camera_angle * degree;
  GLfloat cc = cos(cam);
  GLfloat ss = sin(cam);
  ld ux = hx, uy = hy * cc + ss, uz = cc - ss * hy;
  hx = ux / uz, hy = uy / uz;
  }

EX bool non_spatial_model() {
  if(among(pmodel, mdRotatedHyperboles, mdJoukowsky, mdJoukowskyInverted, mdPolygonal, mdPolynomial))
    return true;
  if(pmodel == mdSpiral && euclid)
    return true;
  #if CAP_GL
  return pmodel && vid.consider_shader_projection && (get_shader_flags() & SF_DIRECT);
  #else
  return false;
  #endif
  }

EX hyperpoint perspective_to_space(hyperpoint h, ld alpha IS(pconf.alpha), eGeometryClass gc IS(ginf[geometry].cclass)) {
  ld hx = h[0], hy = h[1];
  
  if(gc == gcEuclid)
    return hpxy(hx * (1 + alpha), hy * (1 + alpha));
    
  ld hr = hx*hx+hy*hy;
  if(LDIM == 3) hr += h[2]*h[2];
  
  if(hr > .9999 && gc == gcHyperbolic) return Hypc;
  
  ld A, B, C;
  
  ld curv = gc == gcSphere ? 1 : -1;
  
  A = 1+curv*hr;
  B = 2*hr*alpha*-curv;
  C = 1 - curv*hr*alpha*alpha;
  
  B /= A; C /= A;
  
  ld rootsign = 1;
  if(gc == gcSphere && pconf.alpha > 1) rootsign = -1;
  
  ld hz = B / 2 + rootsign * sqrt(C + B*B/4);
  
  hyperpoint H;
  H[0] = hx * (hz+alpha);
  H[1] = hy * (hz+alpha);
  if(LDIM == 3) H[2] = h[2] * (hz + alpha);
  H[LDIM] = hz;
  
  return H;  
  }

EX hyperpoint space_to_perspective(hyperpoint z, ld alpha IS(pconf.alpha)) {
  ld s = 1 / (alpha + z[LDIM]);
  z[0] *= s;
  z[1] *= s;
  if(GDIM == 3) {
    z[2] *= s;
    z[3] = 0;
    }
  else
    z[2] = 0;
  return z;
  }

EX hyperpoint pointable() {
  return WDIM == 2 && GDIM == 3 ? zpush0(cgi.FLOOR) : C0;
  }

/** find a shiftpoint which minimizes value -- we represent points by matrices to make things a bit simpler */

EX shiftmatrix minimize_point_value(shiftmatrix T, function<ld(const shiftmatrix&)> value) {
  
  ld best = value(T);

  for(int it=0; it<50; it++) 
    for(int s=0; s<2*WDIM; s++) {
      shiftmatrix T1 = T * cpush(s/2, (s&1?1:-1) * pow(1.2, -it));
      ld dist = value(T1);
      if(dist < best) best = dist, T = T1;
      if(mdBandAny()) {
        T1.shift += TAU;
        dist = value(T1);
        if(dist < best) best = dist, T = T1;
        T1.shift -= 720._deg;
        dist = value(T1);
        if(dist < best) best = dist, T = T1;
        T1.shift += TAU;
        }
      }
  
  return T;
  }

EX shiftpoint find_on_screen(hyperpoint hxy, const shiftmatrix& T) {
  hyperpoint rel = pointable();
  auto distance_at = [&] (const shiftmatrix& T1) {
    hyperpoint h1;
    applymodel(T1*rel, h1);
    return sqhypot_d(2, hxy - h1);
    };
  return minimize_point_value(T, distance_at) * rel;
  }

EX shiftpoint gethyper(ld x, ld y) {

  ld hx = (x - current_display->xcenter) / current_display->radius;
  ld hy = (y - current_display->ycenter) / current_display->radius / pconf.stretch;
  hyperpoint hxy = point3(hx, hy, 0);
  
  if(WDIM == 2 && GDIM == 3) {
    return mouseover ? find_on_screen(hxy, ggmatrix(mouseover)): shiftless(Hypc);
    }

  if(pmodel) {
    ghxy = hxy;
    return find_on_screen(hxy, rgpushxto0(ghpm));
    }
  
  if(pconf.camera_angle) camrotate(hx, hy);
  
  return shiftless(perspective_to_space(hpxyz(hx, hy, 0)));
  }

void ballmodel(hyperpoint& ret, double alpha, double d, double zl) {
  hyperpoint H = ypush(vid.camera) * xpush(d) * ypush(zl) * C0;
  ld tzh = pconf.ballproj + H[LDIM];
  ld ax = H[0] / tzh;
  ld ay = H[1] / tzh;
  
  ld ca = cos(alpha), sa = sin(alpha);

  ret[0] = ax * ca;
  ret[1] = ay;
  ret[2] = ax * sa;
  
  models::apply_ball(ret[2], ret[1]);
  }

bool use_z_coordinate() {
  #if CAP_VR
  if(vrhr::rendering()) return true;
  #endif
  return current_display->stereo_active();
  }

void apply_depth(hyperpoint &f, ld z) {
  if(vid.usingGL)
    f[2] = z * pconf.depth_scaling;
  else {
    z = z * current_display->radius * pconf.depth_scaling;
    ld mul = current_display->radius / (current_display->radius + z);
    f[0] = f[0] * mul;
    f[1] = f[1] * mul;
    f[2] = vid.xres * current_display->eyewidth() / 2 / current_display->radius + vid.ipd * mul / 2;
    }
  }

bool hypot_zlev(ld zlev, ld& d, ld& df, ld& zf) {
  if(zlev == 1) {
    df = 1; zf = 0;
    return false;
    }
  else {
    // (0,0,1) -> (0, sin z, cos z) -> (sin d cos z, sin z, cos d cos z)
    ld z = geom3::factor_to_lev(zlev);
    
    ld tz = sin_auto(z);
    ld td = sin_auto(abs(d)) * cos_auto(z);
    ld h = hypot(td, tz);
    zf = tz / h, df = td / h;

    if(d > 0)
      d = hypot_auto(d, z);
    else {
      d = -hypot_auto(-d, z);
      zf = -zf;
      }
    return true;
    }
  }

int twopoint_sphere_flips;
bool twopoint_do_flips;

ld find_zlev(hyperpoint& H) {

  if(spatial_graphics) {
    ld zlev = zlevel(H);
    if(zlev > 1-1e-9 && zlev < 1+1e-9) return 1;
    H /= zlev;
    return zlev;
    }  
  
  return 1;
  }

ld get_tz(hyperpoint H) {
  ld tz = pconf.alpha+H[LDIM];
  if(tz < BEHIND_LIMIT && tz > -BEHIND_LIMIT) tz = BEHIND_LIMIT;
  return tz;
  }

EX ld atan2(hyperpoint h) {
  return atan2(h[1], h[0]);
  }

pair<ld, ld> move_z_to_y(hyperpoint& H) {
  if(GDIM == 2) return make_pair(0, 0);
  ld R = hypot(H[1], H[2]);
  pair<ld, ld> res = { H[1] / R, H[2] / R };
  H[1] = R; H[2] = 0;
  return res;
  }

void move_y_to_z(hyperpoint& H, pair<ld, ld> coef) {
  if(GDIM == 3) {
    H[2] = H[1] * coef.second;
    H[1] = H[1] * coef.first;
    #if MAXMDIM >= 4
    H[3] = 1;
    #endif
    }
  }

template<class T> void makeband(shiftpoint H, hyperpoint& ret, const T& f) {
  ld zlev = find_zlev(H.h);
  models::apply_orientation_yz(H[1], H[2]);
  models::apply_orientation(H[0], H[1]);
  auto r = move_z_to_y(H.h);
  
  ld x, y, yf, zf=0;
  y = asin_auto(H[1]);
  x = asin_auto_clamp(H[0] / cos_auto(y));
  if(sphere) {
    if(H[LDIM] < 0 && x > 0) x = M_PI - x;
    else if(H[LDIM] < 0 && x <= 0) x = -M_PI - x;
    }
  x += H.shift;
  hypot_zlev(zlev, y, yf, zf);
  
  f(x, y);
  
  ld yzf = y * zf; y *= yf;
  ret = hpxyz(x / M_PI, y / M_PI, 0);
  move_y_to_z(ret, r);
  models::apply_orientation(ret[1], ret[0]);
  models::apply_orientation_yz(ret[2], ret[1]);
  if(zlev != 1 && use_z_coordinate()) 
    apply_depth(ret, yzf / M_PI);
  return;
  }

EX void makeband_f(shiftpoint H, hyperpoint& ret, const hr::function<void(ld&,ld&)>& f) {
  makeband(H, ret, f);
  }

void band_conformal(ld& x, ld& y) {
  switch(cgclass) {
    case gcSphere:
      y = atanh(sin(y));
      x *= 2; y *= 2;
      break;
    case gcHyperbolic:
      y = 2 * atan(tanh(y/2));
      x *= 2; y *= 2;
      break;
    case gcEuclid:
    default:
      // y = y;
      y *= 2; x *= 2;
      break;
    }
  }

void make_twopoint(ld& x, ld& y) {
  auto p = pconf.twopoint_param;
  ld dleft = hypot_auto(x-p, y);
  ld dright = hypot_auto(x+p, y);
  if(sphere) {
    int tss = twopoint_sphere_flips;
    if(tss&1) { tss--; 
      dleft = TAU - 2*p - dleft;
      dright = TAU - 2*p - dright;
      swap(dleft, dright);
      y = -y;
      }
    while(tss) { tss -= 2;
      dleft = TAU - 4*p + dleft;
      dright = TAU - 4*p + dright;
      }
    }
  x = (dright*dright-dleft*dleft) / 4 / p;
  y = (y>0?1:-1) * sqrt(dleft * dleft - (x-p)*(x-p) + 1e-9);
  }

hyperpoint mobius(hyperpoint h, ld angle, ld scale = 1) {
  h = perspective_to_space(h * scale, 1, gcSphere);
  h = cspin(1, 2, angle * degree) * h;
  return space_to_perspective(h, 1) / scale;
  }

hyperpoint compute_hybrid(hyperpoint H, int rootid) {
  auto& t = pconf.twopoint_param;
  hyperpoint Hl = xpush(+t) * H;
  hyperpoint Hr = xpush(-t) * H;
  ld g = (Hl[0] + 1e-7) / (Hl[1] + 1e-8);
  ld d = hdist0(Hr);
  
  hyperpoint spinned = spintox(Hl) * xpush0(2*t);
  if(Hl[0] < 0) spinned = pispin * spinned;

  ld y = asin_auto(spinned[1]);
  ld x = asin_auto_clamp(spinned[0] / cos_auto(y));

  int sign = (Hl[0] > 0 ? 1 : -1) * hdist0(Hl) < x ? -1 : 1;
  
  switch(rootid & 3) {
    case 1: sign = -sign; break;
    case 2: sign = 1; break;
    case 3: sign = -1; break;
    }

  // (x + t) / g = y
  // yy + (x-t)(x-t) = dd
  // (x+t)*(x+t)/g*g + x*x + t*t - 2*x*t = dd
  
  // x*x*(1+1/g*g) + t*t*(1+1/g*g) + 2xt (1/gg-1) = dd
  // xx + 2xt (1/gg-1) / (1+1/gg) = dd / (1+1/gg) - tt
  
  ld b = t*(1/g/g - 1) / (1+1/g/g);
  ld c = d*d / (1+1/g/g) - t*t;
  
  // xx + 2bx = c
  // xx + 2bx + bb = c + bb
  // (x+b)^2 = c+bb
  // x = +/- sqrt(c+bb) - b
  
  ld a = c+b*b;
  
  hyperpoint ret;
  ret[0] = (a > 0 ? sign * sqrt(a) : 0) - b;
  ret[1] = (ret[0] + t) / g;
  ret[2] = 0;
  
  return ret;
  }

EX ld signed_sqrt(ld x) { return x > 0 ? sqrt(x) : -sqrt(-x); }

EX int axial_x, axial_y;

EX void apply_perspective(const hyperpoint& H, hyperpoint& ret) {
  if(H[2] == 0) { ret[0] = 1e6; ret[1] = 1e6; ret[2] = 1; return; }
  ld ratio = vid.xres / current_display->tanfov / current_display->radius / 2;
  ret[0] = H[0]/H[2] * ratio;
  ret[1] = H[1]/H[2] * ratio;
  ret[2] = 1;
  ret[3] = 1;
  }

EX void apply_nil_rotation(hyperpoint& H) {
  if(nil) {
    H[2] -= H[0] * H[1] / 2;
    models::apply_orientation(H[0], H[1]);
    H[2] += H[0] * H[1] / 2 * pconf.rotational_nil;
    models::apply_orientation(H[1], H[0]);        
    }
  }

EX void applymodel(shiftpoint H_orig, hyperpoint& ret) {
  apply_other_model(H_orig, ret, pmodel);
  }

EX void vr_sphere(hyperpoint& ret, hyperpoint& H, eModel md) {
  ret = H;
  int flip = 1;
  if(md == mdHalfplane) flip = -flip;
  if(pconf.alpha < 1) flip = -flip;  
  ret *= pow(sqhypot_d(3, H), (flip * pconf.depth_scaling-1) / 2);
  ret[2] += pconf.alpha;
  if(md == mdHalfplane) {
    ld d = sqhypot_d(3, ret);
    ret /= abs(d);
    }
  }

void vr_disk(hyperpoint& ret, hyperpoint& H) {
  if(euclid) {
    ret = H;
    ret[2] = vid.depth * (1 - (ret[2] - 1) * pconf.depth_scaling) + pconf.alpha + vid.camera;    
    }
  else if(sphere) {
    vr_sphere(ret, H, mdDisk);
    return;
    }    
  else {
    ld zlev = find_zlev(H);
    ld zl = vid.depth-geom3::factor_to_lev(zlev) * pconf.depth_scaling;
  
    ld d = hdist0(H);
    ld dd = hypot_d(2, H);
  
    hyperpoint H1 = ypush(vid.camera) * xpush(d) * ypush0(zl);
    ld tzh = pconf.alpha + H1[2];
    ld ax = H1[0] / tzh;
    ld ay = H1[1] / tzh;
  
    ret[0] = ax * H[0] / dd;
    ret[1] = ax * H[1] / dd;
    ret[2] = ay;
    }
  }

#if MAXMDIM >= 4
/** Compute the three-point projection. Currently only works in isotropic 3D spaces. */
EX void threepoint_projection(const hyperpoint& H, hyperpoint& ret) {

  hyperpoint H1 = H;
  find_zlev(H1);
  if(true) {
    models::apply_orientation_yz(H1[1], H1[2]);
    models::apply_orientation(H1[0], H1[1]);
    }

  auto p = pconf.twopoint_param;

  ld dist[3];
  for(int i=0; i<3; i++) {
    hyperpoint h1 = xspinpush0(TAU*i/3, p);
    dist[i] = geo_dist(h1, H1);
    }
  
  /* we are looking for the points (x,y,z) such that:
     (x-xi)^2 + (y-yi)^2 + z^2 = di^2

     which is equivalent to:
     x^2+y^2+z^2 -2xxi -2yyi = di^2-xi^2-yi^2
     
     After setting s = x^2+y^2+z^2, we get a system of linear equations for (x,y,s)
  */
    
  dynamicval<eGeometry> g(geometry, gEuclid);
  
  transmatrix T = Id;
  hyperpoint v = C0;
  for(int i=0; i<3; i++) {
    hyperpoint pp = xspinpush0(TAU*i/3, p);
    v[i] = dist[i]*dist[i] - p*p;
    T[i][0] = -2 * pp[0];
    T[i][1] = -2 * pp[1];
    T[i][2] = 1;
    }
  
  transmatrix U = inverse3(T);
  hyperpoint sxy = U * v;
  
  // compute the actual z based on s
  sxy[2] = sxy[2] - sqhypot_d(2, sxy);
  sxy[2] = sxy[2] > 0 ? sqrt(sxy[2]) : 0;

  if(H1[2] < 0) sxy[2] *= -1;
  
  sxy[3] = 1;
  
  geometry = gCubeTiling;
  
  ret = sxy;
  models::apply_orientation(ret[1], ret[0]);
  models::apply_orientation_yz(ret[2], ret[1]);
  }
#endif

EX vector<hr::function<void(shiftpoint& H_orig, hyperpoint& H, hyperpoint& ret)>> extra_projections;

EX void apply_other_model(shiftpoint H_orig, hyperpoint& ret, eModel md) {

  hyperpoint H = H_orig.h;
  
  if(models::product_model(md)) {
    ld zlev = zlevel(H_orig.h);
    H_orig.h /= exp(zlev);
    hybrid::in_underlying_geometry([&] { applymodel(H_orig, ret); });
    ret[2] = zlev * pconf.product_z_scale;
    ret = NLP * ret;
    return;    
    }
  
  switch(md) {
    case mdPerspective: {
      if(gproduct) H = product::inverse_exp(H);
      apply_nil_rotation(H);
      H = lp_apply(H);
      apply_perspective(H, ret);
      return;      
      }

    case mdGeodesic: {
      auto S = lp_apply(inverse_exp(H_orig, pNORMAL | pfNO_DISTANCE));
      apply_perspective(S, ret);
      return;
      }
      
    case mdLiePerspective: {
      if(false) {
        hyperpoint h = point31(0, 0, 1);
        hyperpoint a = point31(0, 0, 0);
        hyperpoint b = point31(0.1, 0, 0);
        println(hlog, rgpushxto0(h) * a);
        println(hlog, rgpushxto0(h) * b);
        exit(1);
        /* x wanes as z grows! */
        }
      if(hyperbolic) {
        models::apply_orientation_yz(H[1], H[2]);
        models::apply_orientation(H[0], H[1]);
        }
      auto S = lie_log(H); S[3] = 1;
      S = lp_apply(S);
      if(hyperbolic) {
        models::apply_orientation(ret[1], ret[0]);
        models::apply_orientation_yz(ret[2], ret[1]);
        }
      apply_perspective(S, ret);
      return;
      }

    case mdRelPerspective: {
      auto S = rel_log(H_orig); S[3] = 1;
      S = lp_apply(S);
      apply_perspective(S, ret);
      return;
      }

    case mdPixel:
      ret = H / current_display->radius;
      return; 
    
    case mdBall: {
      if(vrhr::rendering()) {
        vr_disk(ret, H);
        return;
        }
      ld zlev = find_zlev(H);
      
      ld zl = vid.depth-geom3::factor_to_lev(zlev) * pconf.depth_scaling;
  
      ballmodel(ret, atan2(H), hdist0(H), zl);
      break;      
      }
    
    case mdDisk: {
      if(nonisotropic) {
        ret = lp_apply(inverse_exp(H_orig, pNORMAL | pfNO_DISTANCE));
        ld w;
        if(sn::in()) {
          // w = 1 / sqrt(1 - sqhypot_d(3, ret));
          // w = w / (pconf.alpha + w);
          w = 1 / (sqrt(1 - sqhypot_d(3, ret)) * pconf.alpha + 1);
          }
        else {
          w = hypot_d(3, ret);
          w = sinh(w) / ((pconf.alpha + cosh(w)) * w);
          }
        for(int i=0; i<3; i++) ret[i] *= w;
        ret[3] = 1; 
        break;
        }
      if(vrhr::rendering() && WDIM == 2) {
        vr_disk(ret, H);
        return;
        }
      ld tz = get_tz(H);
      if(!pconf.camera_angle) {
        ret[0] = H[0] / tz;
        ret[1] = H[1] / tz;
        if(GDIM == 3) ret[2] = H[2] / tz;
        else ret[2] = vid.xres * current_display->eyewidth() / 2 / current_display->radius - vid.ipd / tz / 2;
        if(MAXMDIM == 4) ret[3] = 1;
        }
      else {
        ld tx = H[0];
        ld ty = H[1];
        ld cam = pconf.camera_angle * degree;
        GLfloat cc = cos(cam);
        GLfloat ss = sin(cam);
        ld ux = tx, uy = ty * cc - ss * tz, uz = tz * cc + ss * ty;
        ret[0] = ux / uz;
        ret[1] = uy / uz;
        ret[2] = vid.xres * current_display->eyewidth() / 2 / current_display->radius - vid.ipd / uz / 2;
        }
      return;
      }
    
    case mdCentralInversion: {
      ld tz = get_tz(H);
      for(int d=0; d<GDIM; d++) ret[d] = H[d] / tz;
      for(int d=GDIM; d<MAXMDIM; d++) ret[d] = 1;
      ld r = 0;
      for(int d=0; d<GDIM; d++) r += ret[d]*ret[d];
      for(int d=0; d<GDIM; d++) ret[d] /= r;
      return;
      }
    
    case mdHalfplane: {
      if(sphere && vrhr::rendering()) {
        vr_sphere(ret, H, md);
        return;
        }
      // Poincare to half-plane
      
      ld zlev = find_zlev(H);
      H = space_to_perspective(H);
      
      models::apply_orientation_yz(H[1], H[2]);
      models::apply_orientation(H[0], H[1]);
  
      H[1] += 1;
      double rad = sqhypot_d(GDIM, H);
      H /= -rad;
      H[1] += .5;
      
      if(GDIM == 3) {
        // a bit simpler when we do not care about 3D
        H *= pconf.halfplane_scale;
        ret[0] = -H[0];
        ret[1] = 1 + H[1];
        ret[2] = H[2];
        ret[3] = 1;
        models::apply_orientation(ret[1], ret[0]);
        models::apply_orientation_yz(ret[2], ret[1]);
        break;
        }
      
      models::apply_orientation(H[0], H[1]);
      
      H *= pconf.halfplane_scale;
      
      ret[0] = -models::osin - H[0];
      ld height = 0;
      if(zlev != 1) {
        if(abs(models::ocos) > 1e-9)
          height += H[1] * (pow(zlev, models::ocos) - 1);
        if(abs(models::ocos) > 1e-9 && models::osin)
          height += H[0] * models::osin * (pow(zlev, models::ocos) - 1) / models::ocos;
        else if(models::osin)
          height += H[0] * models::osin * log(zlev);
        }
      ret[1] = models::ocos + H[1];
      ret[2] = GDIM == 3 ? H[2] : 0;
      if(MAXMDIM == 4) ret[3] = 1;
       if(zlev != 1 && use_z_coordinate())
        apply_depth(ret, height);
      else 
        ret[1] += height * pconf.depth_scaling;
      break;
      }
    
    case mdAxial: {
      models::apply_orientation_yz(H[1], H[2]);
      models::apply_orientation(H[0], H[1]);
      
      ld& mt = pconf.model_transition;
      
      ld z = H[LDIM];
      if(mt != 1) z += (1-mt) * pconf.alpha;

      ret[0] = H[0] / z;
      ret[1] = H[1] / z;
      if(GDIM == 3) ret[2] = H[2] / z;
      else ret[2] = 0;
      ret[3] = 1;
      
      if(mt) for(int i=0; i<LDIM; i++)  {
        if(mt < 1) 
          ret[i] *= mt;
        ret[i] = atan_auto(ret[i]);
        if(mt < 1) 
          ret[i] /= mt;
        }
      
      if(sphere) ret[0] += axial_x * M_PI, ret[1] += axial_y * M_PI;

      models::apply_orientation(ret[1], ret[0]);
      models::apply_orientation_yz(ret[2], ret[1]);
      break;
      }
    
    case mdAntiAxial: {
      models::apply_orientation_yz(H[1], H[2]);
      models::apply_orientation(H[0], H[1]);
      
      ret[0] = asin_auto(H[0]);
      ret[1] = asin_auto(H[1]);

      ret[2] = 0; ret[3] = 1;

      models::apply_orientation(ret[1], ret[0]);
      models::apply_orientation_yz(ret[2], ret[1]);
      break;
      }
    
    case mdQuadrant: {
      H = space_to_perspective(H);
      models::apply_orientation_yz(H[1], H[2]);
      models::apply_orientation(H[0], H[1]);
      
      tie(H[0], H[1]) = make_pair((H[0] + H[1]) / sqrt(2), (H[1] - H[0]) / sqrt(2));

      H[1] += 1;
      double rad = sqhypot_d(GDIM, H);
      H /= -rad;
      H[1] += .5;
      
      H *= 2;
      
      ld x = exp(-H[0]/2);
      ret[0] = -H[1] * x - 1;
      ret[1] = H[1] / x + 1;

      models::apply_orientation(ret[1], ret[0]);
      models::apply_orientation_yz(ret[2], ret[1]);
      break;
      }
    
    case mdHorocyclic: {

      if(sl2) {
        ret = unshift(H_orig);
        ret *= .5;
        ret[LDIM] = 1;
        ret = lp_apply(ret);
        break;
        }
      find_zlev(H);

      apply_nil_rotation(H);
      
      if(hyperbolic) {
        models::apply_orientation_yz(H[1], H[2]);
        models::apply_orientation(H[0], H[1]);
        }
      
      ret = hyperbolic ? deparabolic13(H) : H;
      ret *= .5;
      ret[LDIM] = 1;
      
      if(hyperbolic) {  
        models::apply_orientation(ret[1], ret[0]);
        models::apply_orientation_yz(ret[2], ret[1]);
        }
      
      if(nonisotropic && !vrhr::rendering()) ret = lp_apply(ret);

      break;
      }

    case mdLieOrthogonal: {
      find_zlev(H);

      if(hyperbolic) {
        models::apply_orientation_yz(H[1], H[2]);
        models::apply_orientation(H[0], H[1]);
        }

      ret = lie_log(H);
      ret *= .5;
      ret[LDIM] = 1;

      if(hyperbolic) {
        models::apply_orientation(ret[1], ret[0]);
        models::apply_orientation_yz(ret[2], ret[1]);
        }

      if(nonisotropic && !vrhr::rendering()) ret = lp_apply(ret);

      break;
      }

    case mdRelOrthogonal: {

      ret = rel_log(H_orig);
      ret *= .5;
      ret[LDIM] = 1;

      if(nonisotropic && !vrhr::rendering()) ret = lp_apply(ret);
      break;
      }

    case mdHemisphere: {
  
      #if CAP_VR
      ld dir = vrhr::rendering() ? -1:1;
      #else
      constexpr ld dir = 1;
      #endif          

      switch(cgclass) {
        case gcHyperbolic: {
          ld zl = zlevel(H);
          ret = H / H[2];
          ret[2] = sqrt(1 - sqhypot_d(2, ret));
          // need to reverse in VR
          ret = ret * (1 + (zl - 1) * ret[2] * pconf.depth_scaling * dir);
          break;
          }
          
        case gcEuclid: default: {
          // stereographic projection to a sphere
          auto hd = hdist0(H) / pconf.euclid_to_sphere;
          if(hd == 0) ret = hpxyz(0, 0, -1);
          else {
            ld x = 2 * hd / (1 + hd * hd);
            ld y = x / hd;
            ret = H * x / hd / pconf.euclid_to_sphere;
            ret[2] = (1 - y);
            ret[2] *= dir;
            ret = ret * (1 + (H[2]-1) * y * pconf.depth_scaling * dir / pconf.euclid_to_sphere);
            }
          break;
          }
        
        case gcSphere: {
          if(vrhr::rendering()) { vr_sphere(ret, H, md); return; }
          ld z = sqhypot_d(3, H);
          int s = H[2] > 0 ? 1 : -1;
          ret = H;
          ret /= ret[2];
          ret[2] = sqrt(1 + ret[0]*ret[0] + ret[1]*ret[1]) * s;
          ret *= z;
          ld& topz = pconf.top_z;
          if(abs(ret[2]) > topz || (hemi_side && s != hemi_side)) {
            ld scale = sqrt(topz*topz-1) / hypot_d(2, ret);
            ret *= scale;
            ret[2] = topz * s;
            }
          if(pconf.depth_scaling != 1) {
            ld v = intval(H, Hypc);
            ret *= pow(v, (dir * pconf.depth_scaling-1) / 2);
            }
          ret /= 3;
          break;
          }
        }

      if(vrhr::rendering()) return;
      
      swap(ret[1], ret[2]);
      
      models::apply_ball(ret[2], ret[1]);
      
      break;
      }
    
    case mdHyperboloidFlat: 
    case mdHyperboloid: {

      if(nonisotropic) {
        // if(nisot::local_perspective_used()) H = NLP * H;
        ret = lp_apply(H);
        break;
        }    
      if(gproduct) {
        ret = H;
        break;
        }

      #if CAP_VR
      if(vrhr::rendering()) {
        if(sphere) { vr_sphere(ret, H, md); return; }
        ret[0] = H[0] * pconf.hyperboloid_scaling;
        ret[1] = H[1] * pconf.hyperboloid_scaling;
        ret[2] = (pconf.alpha + H[2]);
        if(pconf.depth_scaling != 1) {
          ld v = intval(H, Hypc);
          ret *= pow(v, (pconf.depth_scaling-1) / 2);
          }
        return;
        }
      #endif
      
      ret = H;

      if(sphere && pmodel == mdHyperboloidFlat) {
        int s = H[2] > 0 ? 1 : -1;
        ret /= ret[2];
        ret[2] = sqrt(1 + ret[0]*ret[0] + ret[1]*ret[1]) * s;
        }

      if(pconf.depth_scaling != 1) {
        ld v = intval(ret, Hypc);
        ret *= pow(v, (pconf.depth_scaling-1) / 2);
        }

      if(pmodel == mdHyperboloid) {
        ld& topz = pconf.top_z;
        if(ret[2] > topz) {
          ld scale = sqrt(topz*topz-1) / hypot_d(2, ret);
          ret *= scale;
          ret[2] = topz;
          }
        }
      else {
        ret = space_to_perspective(ret, pconf.alpha);
        ret[2] = 1 - pconf.alpha;
        if(sphere) ret[2] = -ret[2];
        }
  
      ret[0] = ret[0] / 3;
      tie(ret[1], ret[2]) = make_pair(((sphere?0:1) - ret[2]) / 3, ret[1] / 3);
      
      models::apply_ball(ret[2], ret[1]);
      break;
      }
    
    case mdFisheye: {
      ld zlev;
      if(nonisotropic) {
        H = lp_apply(inverse_exp(H_orig));
        zlev = 1;
        }
      else {
        zlev = find_zlev(H);
        H = space_to_perspective(H);
        }
      H /= pconf.fisheye_param;
      H[LDIM] = zlev;
      ret = H / sqrt(1 + sqhypot_d(GDIM+1, H));
      if(GDIM == 3) ret[LDIM] = zlev;
      break;
      }
    
    case mdSimulatedPerspective: {
      models::apply_orientation_yz(H[1], H[2]);
      models::apply_orientation(H[0], H[1]);
      auto yz = move_z_to_y(H);
      hyperpoint Hl = xpush(-pconf.twopoint_param) * H;
      hyperpoint Hr = xpush(+pconf.twopoint_param) * H;
      ld lyx = (Hl[1] + 1e-7) / (Hl[0] + 1e-8);
      ld ryx = (Hr[1] + 1e-7) / (Hr[0] + 1e-8);
      // (r.x + t) * lyx = (r.x - t) * ryx = r.y
      // r.x * lyx + t * lyx = r.x * ryx - t * ryx
      // r.x * (lyx-ryx) = - t * (ryx + lyx)
      // r.x = -t * (ryx+lyx) / (lyx-ryx)
      // r.x = - 2 * t * lyx * ryx / lyx / ryx
      
      ret[0] = -pconf.twopoint_param * (ryx + lyx) / (lyx - ryx);
      ret[1] = (ret[0] + pconf.twopoint_param) * lyx;
      ret[2] = 0;

      ret[0] = -ret[0]; ret[1] = -ret[1];

      move_y_to_z(ret, yz);      
      models::apply_orientation(ret[1], ret[0]);
      models::apply_orientation_yz(ret[2], ret[1]);
      break;
      }
    
    case mdTwoHybrid: {
      models::apply_orientation_yz(H[1], H[2]);
      models::apply_orientation(H[0], H[1]);
      auto yz = move_z_to_y(H);
      
      ret = compute_hybrid(H, whateveri[0]); 
      
      move_y_to_z(ret, yz);      
      models::apply_orientation(ret[1], ret[0]);
      models::apply_orientation_yz(ret[2], ret[1]);
      break;
      }
    
    case mdJoukowsky: 
    case mdJoukowskyInverted: {
      models::apply_orientation_yz(H[1], H[2]);
      models::apply_orientation(H[0], H[1]);
      // with equal speed skiprope: models::apply_orientation(H[1], H[0]);
  
      if(pconf.skiprope) {
        static ld last_skiprope = 0;
        static transmatrix lastmatrix;
        if(pconf.skiprope != last_skiprope) {
          ret = mobius(C0, -pconf.skiprope, 2);
          const cld c1(1, 0);
          const cld c2(2, 0);
          const cld c4(4, 0);
          cld w(ret[0], ret[1]);
          cld z = sqrt(c4*w*w-c1) + c2*w;
          if(abs(z) > 1) z = c1 / z;
          hyperpoint zr = hpxyz(real(z), imag(z), 0);
          
          hyperpoint inhyp = perspective_to_space(zr, 1, gcHyperbolic);
          last_skiprope = pconf.skiprope;
          lastmatrix = rgpushxto0(inhyp);
          }
        H = lastmatrix * H;
        }
  
      H = space_to_perspective(H);
      auto yz = move_z_to_y(H);
      ld r = hypot_d(2, H);
      ld c = H[0] / r;
      ld s = H[1] / r;
      ld& mt = pconf.model_transition;
      ld a = 1 - .5 * mt, b = .5 * mt;
      swap(a, b);
      
      ret[0] = (a * r + b/r) * c / 2;
      ret[1] = (a * r - b/r) * s / 2;
      ret[2] = 0;

      if(pconf.skiprope) 
        ret = mobius(ret, pconf.skiprope, 2);
        
      if(pmodel == mdJoukowskyInverted) {
        ld r2 = sqhypot_d(2, ret);
        if(pconf.dualfocus_autoscale)
          ret *= (1-pconf.model_transition) / 2;

        ret[0] = ret[0] / r2;
        ret[1] = -ret[1] / r2;
        move_y_to_z(ret, yz);      
        models::apply_orientation(ret[1], ret[0]);
        
        /*
  
        ret[0] += 1;
        ld alpha = atan2(ret[1], ret[0]);
        ld mod = hypot(ret[0], ret[1]);
        // ret[0] = cos(alpha/2) * sqrt(mod);
        // ret[1] = sin(alpha/2) * sqrt(mod);
        ret[0] = alpha;
        ret[1] = log(mod); */
        }
      else {
        move_y_to_z(ret, yz);      
        models::apply_orientation(ret[0], ret[1]);
        }
      models::apply_orientation_yz(ret[2], ret[1]);
  
      break;
      }
    
    case mdPolygonal: case mdPolynomial: {
    
      H = space_to_perspective(H);

      models::apply_orientation(H[0], H[1]);
  
      pair<long double, long double> p = polygonal::compute(H[0], H[1]);
  
      models::apply_orientation(p.second, p.first);
      ret[0] = p.first;
      ret[1] = p.second;
      ret[2] = 0;
      break;
      }  
      
    case mdBand: 
      if(pconf.model_transition != 1) {
        ld& mt = pconf.model_transition;
        
        H = space_to_perspective(H);
        
        models::apply_orientation(H[0], H[1]);
    
        H[0] += 1;
        double rad = H[0]*H[0] + H[1]*H[1];
        H[1] /= rad;
        H[0] /= rad;
        H[0] -= .5;
        
        ld phi = atan2(H);
        ld r = hypot_d(2, H);
        
        r = pow(r, 1 - mt);
        phi *= (1 - mt);
        ret[0] = r * cos(phi);
        ret[1] = r * sin(phi);
        ret[2] = 0;
        
        ret[0] -= pow(0.5, 1-mt);
        ret[0] /= -(1-mt) * 90._deg;
        ret[1] /= (1-mt) * 90._deg;
        
        models::apply_orientation(ret[1], ret[0]);
        }
      else 
        makeband(H_orig, ret, band_conformal);
      break;
    
    case mdMiller:
      makeband(H_orig, ret, [] (ld& x, ld& y) {
        y *= pconf.miller_parameter;
        band_conformal(x, y);
        y /= pconf.miller_parameter;
        });        
      break;
      
    case mdLoximuthal:
      makeband(H_orig, ret, [] (ld&x, ld &y) {
        ld orig_y = y;
        band_conformal(x, y);
        ld x0 = 0, y0 = pconf.loximuthal_parameter; band_conformal(x0, y0);
        y -= y0;
        
        orig_y -= pconf.loximuthal_parameter;
        
        if(y) x = x * orig_y / y;
        y = orig_y;
        });
      break;
      
    case mdTwoPoint: 
      makeband(H_orig, ret, make_twopoint);
      break;
    
    case mdThreePoint: 
    #if MAXMDIM >= 4
      threepoint_projection(H, ret);
    #else
      throw hr_exception();
    #endif
      break;
    
    case mdMollweide: 
      makeband(H_orig, ret, [] (ld& x, ld& y) { 
        ld theta = 
          hyperbolic ? min(y / 2 + 0.572365, y * 0.78509) :
          euclid ? y :
          y > 0 ? max(y * 0.012/0.015, 90._deg - (90._deg-y) * 0.066262/0.015708) :
          min(y * 0.012/0.015, -90._deg + (90._deg+y) * 0.066262/0.015708);
          
        if(sphere && abs(theta) >= 90._deg - 1e-6) ;
        else {
          for(int it=0; it<4; it++) {
            auto a = (sin_auto(2*theta) +2*theta - M_PI * sin_auto(y));
            auto b = (2 + 2 * cos_auto(2*theta));
            theta = theta - a / b;
          } }
        y = 90._deg * sin_auto(theta);
        x = x * cos_auto(theta); 
        });
      break;
    
    case mdCentralCyl: 
      makeband(H_orig, ret, [] (ld& x, ld& y) { y = tan_auto(y); ld top = vid.yres * M_PI / current_display->radius; if(y>top) y=top; if(y<-top) y=-top; });
      break;

    case mdGallStereographic: 
      makeband(H_orig, ret, [] (ld& x, ld& y) { 
        y = 2 * sin_auto(y) / (1 + cos_auto(y));
        ld top = vid.yres * M_PI / current_display->radius; if(y>top) y=top; if(y<-top) y=-top;
        });
      break;
    
    case mdAitoff: case mdHammer: case mdWinkelTripel:
      makeband(H_orig, ret, [&] (ld& x, ld& y) {
        ld ox = x, oy = y;
        x *= pconf.aitoff_parameter;

        ld x0 = sin_auto(x) * cos_auto(y);
        ld y0 = cos_auto(x) * cos_auto(y);
        ld z0 = sin_auto(y);
        
        ld d = acos_auto(y0);
        ld d0 = hypot(x0, z0);
        
        if(md == mdAitoff || md == mdWinkelTripel) ;
        else if(sphere) d = sqrt(2*(1 - cos(d))) * 90._deg;
        else d = sqrt(2*(cosh(d) - 1)) / 1.5;

        x = x0 * d / d0 / pconf.aitoff_parameter, y = z0 * d / d0;
        if(md == mdWinkelTripel) 
          x = lerp(x, ox, pconf.winkel_parameter),
          y = lerp(y, oy, pconf.winkel_parameter);
          
        });
      break;
    
    case mdWerner: {

      models::apply_orientation_yz(H[1], H[2]);
      models::apply_orientation(H[0], H[1]);

      find_zlev(H); // ignored for now
      
      ld r = hdist0(H);
      if(r == 0) { ret = H; return; }
      ld angle = atan2(H[0], H[1]);
      angle *= sin_auto(r) / r;
      
      ret[0] = sin(angle) * r;
      ret[1] = cos(angle) * r;
      ret[2] = 0;
      ret[3] = 1;

      models::apply_orientation(ret[1], ret[0]);
      models::apply_orientation_yz(ret[2], ret[1]);
      break;
      }

    case mdCollignon: 
      find_zlev(H_orig.h);
      makeband(H_orig, ret, [] (ld& x, ld& y) { 
        ld sgn = 1;
        if(pconf.collignon_reflected && y > 0) y = -y, sgn = -1;
        y = signed_sqrt(sin_auto(y) + pconf.collignon_parameter);
        x *= y / 1.2;
        y -= signed_sqrt(pconf.collignon_parameter);
        y *= sgn;
        y *= M_PI;
        });
      break;
    
    case mdBandEquiarea: 
      makeband(H_orig, ret, [] (ld& x, ld& y) { y = sin_auto(y); });
      break;
    
    case mdBandEquidistant:
      makeband(H_orig, ret, [] (ld& x, ld& y) { });
      break;
    
    case mdSinusoidal: 
      makeband(H_orig, ret, [] (ld& x, ld& y) { x *= cos_auto(y); });
      break;
    
    case mdEquidistant: case mdEquiarea: case mdEquivolume: {
      if(vrhr::rendering() && GDIM == 3 && pmodel == mdEquidistant) {
        ret = inverse_exp(H_orig);
        ret[3] = 1;
        return;
        }

      if(nonisotropic || gproduct) {
        ret = lp_apply(inverse_exp(H_orig));
        ret[3] = 1;
        break;
        }
      ld zlev = find_zlev(H);

      ld rad = hypot_d(GDIM, H);
      if(rad == 0) rad = 1;
      ld d = hdist0(H);
      ld df, zf;
      hypot_zlev(zlev, d, df, zf);
      
      if(md == mdEquivolume)
        d = pow(volume_auto(d), 1/3.) * pow(90._deg, 1/3.);
      else if(md == mdEquiarea && sphere) {
        d = sqrt(2*(1 - cos(d))) * 90._deg;
        }
      else if(pmodel == mdEquiarea && hyperbolic)
        d = sqrt(2*(cosh(d) - 1)) / 1.5;
      
      ld factor = d * df / rad;
      if(!vrhr::rendering()) factor /= M_PI;

      ret = H * factor;
      if(GDIM == 2) ret[2] = 0; 
      if(MAXMDIM == 4) ret[3] = 1;
      if(zlev != 1 && use_z_coordinate()) 
        apply_depth(ret, d * zf / M_PI);
      
      break;
      }
    
    case mdRotatedHyperboles: {
      // ld zlev =  <- not implemented
      find_zlev(H); // + vid.depth;
      models::apply_orientation(H[0], H[1]);
      
      ld y = asin_auto(H[1]);
      ld x = asin_auto_clamp(H[0] / cos_auto(y));
      // ld z = zlev == 1 ? 0 : geom3::factor_to_lev(zlev);
      
      ld factor = geom3::lev_to_factor(y + vid.depth);
      
      ret[0] = sinh(x) * factor;
      ret[1] = cosh(x) * factor;
      ret[2] = 0;
      
      if(pconf.use_atan) {
        ret[0] = atan(ret[0]);
        ret[1] = atan(ret[1]);
        }

      break;
      }
    
    case mdFormula: {
      dynamicval<eModel> m(pmodel, pconf.basic_model);
      applymodel(H_orig, ret);
      exp_parser ep;
      ep.extra_params["z"] = cld(ret[0], ret[1]);
      ep.extra_params["cx"] = ret[0];
      ep.extra_params["cy"] = ret[1];
      ep.extra_params["cz"] = ret[2];
      ep.extra_params["ux"] = H[0];
      ep.extra_params["uy"] = H[1];
      ep.extra_params["uz"] = H[2];
      ep.s = pconf.formula;
      cld res;
      try {
        res = ep.parse();
        }
      catch(hr_parse_exception&) {
        res = 0;
        }
      ret[0] = real(res);
      ret[1] = imag(res);
      ret[2] = 0;
      break;
      }
    
    case mdSpiral: {
      cld z;
      if(hyperbolic || sphere) makeband(H_orig, ret, band_conformal);
      else ret = H;
      z = cld(ret[0], ret[1]) * models::spiral_multiplier;
      
      if(pconf.spiral_cone < 360) {
        ld alpha = imag(z) * 360 / pconf.spiral_cone;
        ld r = real(z);
        r = exp(r);
        
        ret[0] = -sin(alpha) * r;
        ret[1] = cos(alpha) * r;
        if(euclid) ret = models::euclidean_spin * ret;
        ret[2] = (r-1) * sqrt( pow(360/pconf.spiral_cone, 2) - 1);
        
        models::apply_ball(ret[2], ret[1]);
        }
      else {      
        z = exp(z);
        ret[0] = real(z);
        ret[1] = imag(z);
        if(euclid) ret = models::euclidean_spin * ret;

        if(pconf.skiprope) 
          ret = mobius(ret, pconf.skiprope, 1);
        }
      break;
      }
    
    case mdRetroCraig: {
      makeband(H_orig, ret, [] (ld& x, ld& y) { 
        if(x)
          y = x / sin_auto(x) * (sin_auto(y) * cos_auto(x) - tan_auto(pconf.loximuthal_parameter) * cos_auto(y));
        else
          y = sin_auto(y) - tan_auto(pconf.loximuthal_parameter) * cos_auto(y);
        });
      break;
      }

    case mdRetroLittrow: {
      makeband(H_orig, ret, [] (ld& x, ld& y) { 
        tie(x, y) = make_pair(
          sin_auto(x) / cos_auto(y),
          cos_auto(x) * tan_auto(y)
          );
        });
      break;
      }
    
    case mdRetroHammer: {
      ld d = hdist(H, ypush0(pconf.loximuthal_parameter));
      makeband(H_orig, ret, [d,H] (ld& x, ld& y) { 
        if(x == 0 && y == 0) return;

        if(x)
          y = x / sin_auto(x) * (sin_auto(y) * cos_auto(x) - tan_auto(pconf.loximuthal_parameter) * cos_auto(y));
        else
          y = sin_auto(y) - tan_auto(pconf.loximuthal_parameter) * cos_auto(y);
          
        ld scale = d / hypot(x, y);
        if(H[2] < 0) scale = -scale;
        x *= scale;
        y *= scale;
        });
      break;
      }
    
    case mdPanini: {
      find_zlev(H);
      models::apply_orientation_yz(H[1], H[2]);
      models::apply_orientation(H[0], H[1]);

      ld proh = sqrt(H[2]*H[2] + curvature() * H[0] * H[0]);
      H /= proh;
      H /= (H[2] + pconf.alpha);
      ret = H;
      ret[2] = 0; ret[3] = 1;

      models::apply_orientation(ret[1], ret[0]);
      models::apply_orientation_yz(ret[2], ret[1]);
      break;
      }
    
    case mdPoorMan: {
      find_zlev(H);
      H = space_to_perspective(H);
      
      models::apply_orientation_yz(H[1], H[2]);
      models::apply_orientation(H[0], H[1]);
      
      ld u = H[0], v = H[1];
      if(abs(u) > 1e-3 && abs(v) > 1e-3) {
        ld r2 = u*u+v*v;
        ld scale = sqrt((-r2+sqrt(r2*(r2+4*u*u*v*v*(r2-2))))/(2*(r2-2))) / u / v;
        if(u*v<0) scale = -scale;
        H = scale * H;
        }
      ret = H;
      ret[2] = 0;
      ret[3] = 1;
  
      models::apply_orientation(ret[1], ret[0]);
      models::apply_orientation_yz(ret[2], ret[1]);
      break;
      }
    
    case mdGUARD: case mdManual: break;

    default:
      if(md < isize(extra_projections) && extra_projections[md])
        extra_projections[md](H_orig, H, ret);
      break;
    }

  ghcheck(ret,H_orig);
  }

// game-related graphics

EX transmatrix sphereflip; // on the sphere, flip
EX bool playerfound; // has player been found in the last drawing?

EX bool outofmap(hyperpoint h) {
  if(GDIM == 3)
    return false;
  else if(euclid) 
    return h[2] < .5; // false; // h[0] * h[0] + h[1] * h[1] > 15 * cgi.crossf;
  else if(sphere)
    return h[2] < .1 && h[2] > -.1 && h[1] > -.1 && h[1] < .1 && h[0] > -.1 && h[0] < .1;
  else
    return h[2] < .5;
  }

EX hyperpoint mirrorif(const hyperpoint& V, bool b) {
  if(b) return Mirror*V;
  else return V;
  }

EX shiftmatrix mirrorif(const shiftmatrix& V, bool b) {
  if(b) return V*Mirror;
  else return V;
  }

// -1 if away, 0 if not away
EX int away(const transmatrix& V2) {
  return (intval(C0, V2 * xpush0(.1)) > intval(C0, tC0(V2))) ? -1 : 0;
  }

/* double zgrad(double f1, double f2, int nom, int den) {
  using namespace geom3;
  ld fo1 = factor_to_lev(f1);
  ld fo2 = factor_to_lev(f2);
  return lev_to_factor(fo1 + (fo2-fo1) * nom / den);
  } */

EX double zgrad0(double l1, double l2, int nom, int den) {
  using namespace geom3;
  return lev_to_factor(l1 + (l2-l1) * nom / den);
  }

EX bool behindsphere(const hyperpoint& h) {
  if(!sphere) return false;

  if(mdBandAny()) return false;

  if(pconf.alpha > 1) {
     if(h[LDIM] > -1/pconf.alpha) return true;
     }  
  
  if(pconf.alpha <= 1) {
    if(h[LDIM] < .2-pconf.alpha) return true;
    }
  
  return false;
  }

ld to01(ld a0, ld a1, ld x) {
  if(x < a0) return 0;
  if(x > a1) return 1;
  return (x-a0) / (a1-a0);
  }

EX ld spherity(const hyperpoint& h) {
  if(!sphere) return 1;
  
  if(pconf.alpha > 1) {
    return to01(1/pconf.alpha, 1, abs(h[2]));
    }  
  
  if(pconf.alpha <= 1) {
    return to01(-1.5, 1, h[2]);
    }
      
  return 1;
  }

EX bool behindsphere(const transmatrix& V) {
  return behindsphere(tC0(V));
  }

EX bool behindsphere(const shiftmatrix& V) {
  return behindsphere(tC0(V.T));
  }

EX ld spherity(const transmatrix& V) {
  return spherity(tC0(V));
  }

EX bool confusingGeometry() {
  #if MAXMDIM >= 4
  if(reg3::ultra_mirror_in()) return true;
  #endif
  return quotient || elliptic || (fake::in() && fake::multiple);
  }

EX ld master_to_c7_angle() {
  if(mhybrid) return hybrid::in_underlying_geometry(master_to_c7_angle);
  if(WDIM == 3) return 0;
  ld alpha = 0;
  #if CAP_GP
  if(cgi.gpdata) alpha = cgi.gpdata->alpha;
  #endif  
  return (!BITRUNCATED && !bt::in() && !arcm::in()) ? M_PI + alpha : 0;
  }

EX transmatrix actualV(const heptspin& hs, const transmatrix& V) {
  if(gproduct) return PIU(actualV(hs, V));
  if(WDIM == 3) return V;
  #if CAP_IRR
  if(IRREGULAR)
    return V * spin(M_PI + TAU / S7 * (hs.spin + irr::periodmap[hs.at].base.spin));
  #endif
  #if CAP_ARCM
  if(arcm::in()) return V * spin(-arcm::current.triangles[arcm::id_of(hs.at)][hs.spin].first);
  #endif
  #if CAP_BT
  if(bt::in()) return V;
  #endif
  if(kite::in()) return V;
  return (hs.spin || !BITRUNCATED) ? V * spin(hs.spin*TAU/hs.at->type + master_to_c7_angle()) : V;
  }

EX shiftmatrix actualV(const heptspin& hs, const shiftmatrix& V) {
  return shiftless(actualV(hs, V.T), V.shift);
  }

EX bool point_behind(const shiftpoint h) {
  if(sphere) return false;
  if(!in_perspective()) return false;
  hyperpoint h1;
  if(pmodel == mdGeodesic) h1 = inverse_exp(h, pQUICK);
  if(pmodel == mdPerspective && gproduct) h1 = product::inverse_exp(h.h);
  h1 = lp_apply(h1);
  return h1[2] < 1e-8;
  }

void raise_error() {
  println(hlog, "something wrong");
  }
  
EX bool invalid_matrix(const transmatrix T) {
  for(int i=0; i<GDIM; i++) for(int j=0; j<GDIM; j++)
    if(std::isnan(T[i][j]) || T[i][j] > 1e8 || T[i][j] < -1e8 || std::isinf(T[i][j]))
      return true;
  if(gproduct || (cgflags & qAFFINE)) {
    for(int i=0; i<GDIM; i++) for(int j=0; j<GDIM; j++) if(abs(T[i][j]) > 1e-60) return false;
    }
  else 
    for(int i=0; i<GDIM; i++) for(int j=0; j<GDIM; j++) if(T[i][j] > .5 || T[i][j] < -.5) return false;
  return true;
  }
  
EX bool invalid_point(const hyperpoint h) {
  return std::isnan(h[LDIM]) || h[LDIM] > 1e8 || std::isinf(h[LDIM]);
  }

EX bool invalid_point(const shiftpoint h) { return invalid_point(h.h); }

EX bool in_smart_range(const shiftmatrix& T) {
  shiftpoint h = tC0(T);
  if(invalid_point(h)) return false;
  if(nil || nih) return true;
  #if CAP_SOLV
  if(pmodel == mdGeodesic) return nisot::in_table_range(h.h);
  #endif
  hyperpoint h1;
  applymodel(h, h1);
  if(invalid_point(h1)) return false;
  ld x = current_display->xcenter + current_display->radius * h1[0];
  ld y = current_display->ycenter + current_display->radius * h1[1] * pconf.stretch;

  bool culling = !geom3::euc_in_hyp();

  if(culling) {
    if(x > current_display->xtop + current_display->xsize * 2) return false;
    if(x < current_display->xtop - current_display->xsize * 1) return false;
    if(y > current_display->ytop + current_display->ysize * 2) return false;
    if(y < current_display->ytop - current_display->ysize * 1) return false;
    if(GDIM == 3) {
      if(-h1[2] < pconf.clip_min * 2 - pconf.clip_max) return false;
      if(-h1[2] > pconf.clip_max * 2 - pconf.clip_min) return false;
      }
    }

  ld epsilon = 0.01;
  
  transmatrix ar;
  
  ld dx = 0, dy = 0, dz = 0, dh[MAXMDIM];
  for(int i=0; i<GDIM; i++) {    
    hyperpoint h2;
    applymodel(T * cpush0(i, epsilon), h2);
    ld x1 = current_display->radius * abs(h2[0] - h1[0]) / epsilon;
    ld y1 = current_display->radius * abs(h2[1] - h1[1]) * pconf.stretch / epsilon;
    
    for(int j=0; j<GDIM; j++) ar[i][j] = current_display->radius * (h2[j]-h1[j]) / epsilon;
    
    dx = max(dx, x1); dy = max(dy, y1);
    if(GDIM == 3) dz = max(dz, abs(h2[2] - h1[2]));
    dh[i] = hypot(x1, y1);
    }
  
  if(GDIM == 2 && vid.smart_area_based) {
    ld area = det2(ar);
    ld scale = sqrt(area) * cgi.scalefactor * hcrossf7;
    if(scale <= vid.smart_range_detail) return false;
    }
  
  else if(GDIM == 3) { 
    if(-h1[2] + 2 * dz < pconf.clip_min || -h1[2] - 2 * dz > pconf.clip_max) return false;
    sort(dh, dh+GDIM); 
    ld scale = sqrt(dh[1] * dh[2]) * cgi.scalefactor * hcrossf7;
    if(scale <= (WDIM == 2 ? vid.smart_range_detail : vid.smart_range_detail_3)) return false;
    }
  else {
    ld scale = sqrt(dh[0] * dh[1]) * cgi.scalefactor * hcrossf7;
    if(scale <= vid.smart_range_detail) return false;
    }

  if(!culling) return true;
  
  return 
    x - 2 * dx < current_display->xtop + current_display->xsize && 
    x + 2 * dx > current_display->xtop &&
    y - 2 * dy < current_display->ytop + current_display->ysize &&
    y + 2 * dy > current_display->ytop;
  }

#if CAP_GP
namespace gp {

/*
void drawrec(cell *c, const transmatrix& V) {
  if(dodrawcell(c))
    drawcell(c, V, 0, false);
  for(int i=0; i<c->type; i++) {
    cell *c2 = c->move(i);
    if(!c2) continue;
    if(c2->move(0) != c) continue;
    if(c2 == c2->master->c7) continue;
    transmatrix V1 = V * ddspin(c, i) * xpush(cgi.crossf) * iddspin(c2, 0) * spin(M_PI);
    drawrec(c2, V1);
    }
  } */
  
  bool drawrec(cell *c, const shiftmatrix& V, gp::loc at, int dir, int maindir, local_info li) { 
    bool res = false;
    shiftmatrix V1 = V * cgi.gpdata->Tf[li.last_dir][at.first&GOLDBERG_MASK][at.second&GOLDBERG_MASK][fixg6(dir)];
    if(do_draw(c, V1)) {
      /* auto li = get_local_info(c);
      if((dir - li.total_dir) % S6) printf("totaldir %d/%d\n", dir, li.total_dir);
      if(at != li.relative) printf("at %s/%s\n", disp(at), disp(li.relative));
      if(maindir != li.last_dir) printf("ld %d/%d\n", maindir, li.last_dir); */
      li.relative = at;
      li.total_dir = fixg6(dir);
      current_li = li;
      li_for = c;
      drawcell(c, V1);
      res = true;
      }
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->move(i);
      if(!c2) continue;
      if(c2->move(0) != c) continue;
      if(c2 == c2->master->c7) continue;
      res |= drawrec(c2, V, at + eudir(dir+i), dir + i + SG3, maindir, li);
      }
    return res;
    }
  
  bool drawrec(cell *c, const shiftmatrix& V) {
    local_info li;    
    li.relative = loc(0,0);
    li.total_dir = 0;
    li.last_dir = -1;
    li.first_dir = -1;
    li_for = c;
    current_li = li;
    bool res = false;
    if(do_draw(c, V))
      drawcell(c, V), res = true;
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->move(i);
      if(!c2) continue;
      if(c2->move(0) != c) continue;
      if(c2 == c2->master->c7) continue;
      li.last_dir = i;
      res |= drawrec(c2, V, gp::loc(1,0), SG3, i, li);
      }
    return res;
    }
  }
#endif

vector<tuple<heptspin, hstate, shiftmatrix> > drawn_cells;

EX bool drawcell_subs(cell *c, const shiftmatrix& V) {
  
  #if CAP_GP    
  if(GOLDBERG) {
    return gp::drawrec(c, V);
    }
  #endif
  
  bool draw = false;

  #if CAP_IRR    
  if(IRREGULAR) {
    auto& hi = irr::periodmap[c->master];
    auto& vc = irr::cells_of_heptagon[hi.base.at];
    for(int i=0; i<isize(vc); i++) {
      cell *c = hi.subcells[i];
      shiftmatrix V1 = V * irr::cells[vc[i]].pusher;
      if(do_draw(c, V1))
        draw = true,
        drawcell(hi.subcells[i], V * irr::cells[vc[i]].pusher);
      }
    return draw;
    }
  #endif
  
  if(do_draw(c, V)) {
    draw = true;
    drawcell(c, V);
    }

  if(BITRUNCATED) forCellIdEx(c1, d, c) {
    if(c->c.spin(d) == 0) {
      shiftmatrix V2 = V * currentmap->adj(c, d);
      if(do_draw(c1, V2))
        draw = true,
        drawcell(c1, V2);
      }
    }

  return draw;
  }

void hrmap::draw_all() {
  if(sphere && pmodel == mdSpiral) {
    if(models::ring_not_spiral) {
      int qty = ceil(1. / pconf.sphere_spiral_multiplier);
      if(qty > 100) qty = 100;
      for(int i=-qty; i < qty; i++)
        draw_at(centerover, cview(TAU * i));
      }
    else {
      draw_at(centerover, cview());
      if(vid.use_smart_range) for(int i=1;; i++) {
        int drawn = cells_drawn;
        draw_at(centerover, cview(TAU * i));
        draw_at(centerover, cview(-TAU * i));
        if(drawn == cells_drawn) break;
        }
      }
    }
  else
    draw_at(centerover, cview());
  }

void hrmap::draw_at(cell *at, const shiftmatrix& where) {
  dq::clear_all();
  auto& enq = confusingGeometry() ? dq::enqueue_by_matrix_c : dq::enqueue_c;
  
  enq(at, where);
      
  while(!dq::drawqueue_c.empty()) {
    auto& p = dq::drawqueue_c.front();
    cell *c = p.first;
    shiftmatrix V = p.second;
    dq::drawqueue_c.pop();
    
    if(!do_draw(c, V)) continue;
    drawcell(c, V);
    if(in_wallopt() && isWall3(c) && isize(dq::drawqueue) > 1000) continue;

    #if MAXMDIM >= 4
    if(reg3::ultra_mirror_in())
      for(auto& T: cgi.ultra_mirrors) 
        enq(c, optimized_shift(V * T));
    #endif
    
    for(int i=0; i<c->type; i++) {
      // note: need do cmove before c.spin
      cell *c1 = c->cmove(i);      
      if(c1 == &out_of_bounds) continue;
      enq(c1, optimized_shift(V * adj(c, i)));
      }
    }
  }

void hrmap_standard::draw_at(cell *at, const shiftmatrix& where) {
  if(S3 > 4) {
    hrmap::draw_at(at, where);
    return;
    }
  drawn_cells.clear();
  drawn_cells.emplace_back(at->master, hsOrigin, where * master_relative(at, true));
  for(int i=0; i<isize(drawn_cells); i++) {    
    // prevent reallocation due to insertion
    if(drawn_cells.capacity() < drawn_cells.size() + 16)
      drawn_cells.reserve(max<size_t>(2 * drawn_cells.size(), 128));

    const auto& dc = drawn_cells[i];
    auto& hs = get<0>(dc);
    auto& s = get<1>(dc);
    auto& V = get<2>(dc);

    cell *c = hs.at->c7;
    
    const shiftmatrix& V1 = hs.mirrored ? V * Mirror : V;
    
    bool draw = drawcell_subs(c, actualV(hs, V1));
    
    if(sphere) draw = true;

    if(draw) for(int d=0; d<c->master->type; d++) {
      hstate s2 = transition(s, d);
      if(s2 == hsError) continue;
      heptspin hs2 = hs + d + wstep;
      shiftmatrix Vd;
      if(inforder::mixed()) {
        int d1 = gmod(hs.spin+d, c->type);
        Vd = V * spin(-TAU*d/c->type) * xpush(spacedist(c, d1)) * spin180();
        }
      else
        Vd = V * cgi.heptmove[d];
      optimize_shift(Vd);
      drawn_cells.emplace_back(hs2, s2, Vd);
      }
    }
  }

EX bool keep_vertical() {
  if((WDIM == 2 || gproduct) && GDIM == 3 && vid.fixed_yz) return !CAP_ORIENTATION;
  if(downseek.qty) return true;
  return false;
  }

EX hyperpoint vertical_vector() {
  auto& ds = downseek;
  if(msphere && !sphere && !gproduct && vid.fixed_yz) {
    hyperpoint h = get_view_orientation() * C0;
    if(vid.wall_height > 0) h = -h;
    return h;
    }
  if(meuclid && hyperbolic && vid.fixed_yz) {
    hyperpoint h = iso_inverse(View) * C0;
    return View * (orthogonal_move(h, vid.wall_height) - h);
    }
  if((WDIM == 2 || gproduct) && GDIM == 3 && vid.fixed_yz)
    return get_view_orientation() * lztangent(1);
  else if(ds.qty && gproduct)
    return get_view_orientation() * product::inverse_exp(ds.point);
  else if(ds.qty)
    return ds.point;
  return C0;
  }

EX bool down_is_forward;

EX void spinEdge(ld aspd) { 

  #if CAP_VR
  if(vrhr::active() && keep_vertical() && !vrhr::first) {
    transmatrix T = vrhr::hmd_ref_at;
    T = vrhr::sm * inverse(T);
    vrhr::be_33(T);
    
    transmatrix V = T * get_view_orientation();
    
    hyperpoint h = inverse(V) * C0;
    if(!gproduct) {
      V = V * rgpushxto0(h);
      }
    
    int dir = down_is_forward ? 0 : 1;

    V = cspin90(2, dir) * V;

    if(1) {
      dynamicval<eGeometry> g(geometry, gSphere);
      bool b = vid.always3;
      vid.always3 = false;
      geom3::apply_always3();
      V = gpushxto0(V*C0) * V;
      fixmatrix(V);
      if(b) {
        vid.always3 = b;
        geom3::apply_always3();
        }
      }
    
    vrhr::be_33(V);

    V = cspin90(dir, 2) * V;
    V = inverse(T) * V;
    if(!gproduct) V = V * gpushxto0(h);
    get_view_orientation() = V;
    return;
    }
  #endif

  ld downspin = 0;
  auto& ds = downseek;
  if(dual::state == 2 && (dual::one_euclidean ? !euclid : dual::currently_loaded != dual::main_side)) {
    transmatrix our   = dual::get_orientation();
    transmatrix their = dual::player_orientation[dual::main_side];
    fixmatrix(our);
    fixmatrix(their);
    if(GDIM == 2) {
      transmatrix T = their * iso_inverse(our);
      hyperpoint H = T * xpush0(1);
      downspin = -atan2(H[1], H[0]);
      }
    else rotate_view(their * iso_inverse(our));
    }
  else if(playerfound && vid.fixed_facing) {
    hyperpoint H = gpushxto0(unshift(playerV) * C0) * unshift(playerV) * xpush0(5);
    downspin = atan2(H[1], H[0]);
    downspin += vid.fixed_facing_dir * degree;
    if(flipplayer) downspin += M_PI;
    cyclefix(downspin, 0);
    aspd = (1 + 2 * abs(downspin)) * aspd;
    }
  else if(keep_vertical()) {
    hyperpoint h = vertical_vector();
    downspin = -atan2(h[0], h[1]);
    if(ds.qty && GDIM == 2) {
      downspin += models::rotation * degree;
      }
    if(ds.qty) {
      cyclefix(downspin, 0);      
      downspin = downspin * min(ds.speed, (double)1);
      }
    else aspd = 999999;
    }
  if(downspin >  aspd) downspin =  aspd;
  if(downspin < -aspd) downspin = -aspd;
  rotate_view(spin(downspin));
  }

/** \brief convert a shiftmatrix to the coordinate system of View 
 *  usually used to set which_copy
 */
EX transmatrix back_to_view(const shiftmatrix& V) {
  // ortho_inverse does not work in 2.5D, iso_inverse does not work in Nil. 
  // just use inverse
  return inverse(actual_view_transform) * unshift(V);
  }

EX void fix_whichcopy(cell *c) {
  if(!gmatrix.count(cwt.at)) return;
  current_display->which_copy = back_to_view(gmatrix[c]);
  }    

void fix_whichcopy_if_near() {
  if(!gmatrix.count(cwt.at)) return;
  transmatrix T = back_to_view(gmatrix[cwt.at]);
  if(!eqmatrix(T, current_display->which_copy)) return;
  current_display->which_copy = T;
  }

EX void adjust_eye(transmatrix& T, cell *c) {
  if(!embedded_plane) return;
  geom3::do_auto_eye();
  int sl = snakelevel(c);
  if(isWorm(c->monst) && sl < 3) sl++;
  int i = (msphere && !sphere) ? 1 : 0;
  if(sl || vid.eye || i)
    T = T * lzpush(cgi.SLEV[sl] - cgi.FLOOR + vid.eye + i);
  }

EX void centerpc(ld aspd) {

  if(subscreens::split([=] () {centerpc(aspd);})) return;
  if(dual::split([=] () { centerpc(aspd); })) return;

  #if CAP_CRYSTAL && CAP_RUG
  if(cryst)
    crystal::centerrug(aspd);
  #endif
  
  #if CAP_RACING
  if(racing::on && racing::set_view()) return;
  #endif

  #if MAXMDIM >= 4
  if(shmup::on && vid.sspeed > -5 && GDIM == 3) {
    int id = subscreens::in ? subscreens::current_player : 0;
    auto& pc = shmup::pc[id];
    centerover = pc->base;
    transmatrix T = pc->at;
    adjust_eye(T, cwt.at);
    /* in nonisotropic geometries, T is isometry * rotation, so iso_inverse does not work */
    if(nonisotropic)
      View = inverse(T);
    else
      View = iso_inverse(T);
    if(gproduct) NLP = ortho_inverse(pc->ori);
    if(WDIM == 2) rotate_view( cspin180(0, 1) * cspin(2, 1, 90._deg + shmup::playerturny[id]) * spin270() );
    return;
    }
  #endif
  
  if(ors::mode == 2 && vid.sspeed < 5) return;
  if(vid.sspeed >= 4.99) aspd = 1000;
  DEBBI(DF_GRAPH, ("center pc"));

  auto& W = current_display->which_copy;
  ors::unrotate(W); ors::unrotate(View); ors::unrotate(cwtV.T);

  /* what should we center? */
  transmatrix T;
  if(multi::players > 1) 
    T = unshift(cwtV); /* do not even try */
  else {
    T = W;
    if(shmup::on)
      T = T * shmup::pc[0]->at;
    }
  
  if(invalid_matrix(T)) return;
  
  adjust_eye(T, cwt.at);

  hyperpoint H = tC0(T);

  ld R = (zero_d(GDIM, H) && !gproduct) ? 0 : hdist0(H);
  if(R < 1e-9) {
    // either already centered or direction unknown
    /* if(playerfoundL && playerfoundR) {
      
      } */

    spinEdge(aspd);
    fixmatrix(View);
    fix_whichcopy(cwt.at);
    fixmatrix(current_display->which_copy);
    }
  
  else {
    aspd *= euclid ? (2+3*R*R) : (1+R+(shmup::on?1:0));

    if(R < aspd) fix_whichcopy_if_near();
    
    if(R < aspd) 
      shift_view_to(shiftless(H));
    else 
      shift_view_towards(shiftless(H), aspd);
      
    fixmatrix(View);
    fixmatrix(current_display->which_copy);
    spinEdge(aspd);
    }

  if(set_multi && multi::two_focus) {
    pconf.model_orientation = atan2(multi_point) / degree;
    auto& d = pconf.twopoint_param;
    d = hdist0(multi_point);
    if(among(pmodel, mdJoukowsky, mdJoukowskyInverted)) {
      pconf.model_orientation += 90;
      pconf.model_transition = sinh(d) / (1 + cosh(d));
      pconf.dualfocus_autoscale = true;
      }
    }

  ors::rerotate(W); ors::rerotate(cwtV.T); ors::rerotate(View);
  }

EX transmatrix oView;

EX purehookset hooks_preoptimize, hooks_postoptimize;

EX void optimizeview() {

  if(subscreens::split(optimizeview)) return;
  if(dual::split(optimizeview)) return;
  
  cell *c = centerover;
  transmatrix iView = view_inverse(View);
  callhooks(hooks_preoptimize);
  virtualRebase(centerover, iView);
  if(c != centerover && (sphere || sl2)) {
    transmatrix T = currentmap->relative_matrix(centerover, c, C0);
    T = stretch::itranslate(tC0(T)) * T;
    stretch::mstretch_matrix = T * stretch::mstretch_matrix;
    }
    
  View = iview_inverse(iView);
  fixmatrix(View);
  callhooks(hooks_postoptimize);
  
  walking::handle();

  if(is_boundary(centerover))
    centerover = c, View = oView;
  else
    oView = View;

  #if CAP_ANIMATIONS
  if(centerover && inmirror(centerover)) {
    anims::reflect_view();
    }
  #endif
  }

void addball(ld a, ld b, ld c) {
  hyperpoint h;
  ballmodel(h, a, b, c);
  for(int i=0; i<3; i++) h[i] *= current_display->radius;
  curvepoint(h);
  }

void ballgeometry() {
  queuereset(mdPixel, PPR::CIRCLE);
  for(int i=0; i<60; i++)
    addball(TAU * i / 60, 10, 0);
  for(double d=10; d>=-10; d-=.2)
    addball(0, d, 0);
  for(double d=-10; d<=10; d+=.2)
    addball(0, d, vid.depth);
  addball(0, 0, -vid.camera);
  addball(0, 0, vid.depth);
  addball(0, 0, -vid.camera);
  addball(0, -10, 0);
  addball(0, 0, -vid.camera);
  queuecurve(shiftless(Id), darkena(0xFF, 0, 0x80), 0, PPR::CIRCLE);
  queuereset(pmodel, PPR::CIRCLE);
  }

EX void resetview() {
  DEBBI(DF_GRAPH, ("reset view"));
  // EUCLIDEAN
  NLP = Id;
  stretch::mstretch_matrix = Id;
  auto lView = View;
  if(cwt.at) {
    centerover = cwt.at;
    View = iddspin(cwt.at, cwt.spin);
    adjust_eye(View, cwt.at);
    if(!flipplayer) View = pispin * View;
    if(cwt.mirrored) View = Mirror * View;

    if(centering) {
      hyperpoint vl = View * get_corner_position(cwt.at, cwt.spin);
      hyperpoint vr = View * get_corner_position(cwt.at, (cwt.spin+1) % cwt.at->type);

      hyperpoint vm = (centering == eCentering::edge) ? mid(vl, vr) : vl;
      
      transmatrix rm = gpushxto0(vm);

      View = spintox(rm*vr) * rm * View;
      }

    if(GDIM == 2) View = spin(M_PI + vid.fixed_facing_dir * degree) * View;
    if(GDIM == 3 && !gproduct) View = cspin90(0, 2) * View;
    if(gproduct) NLP = cspin90(0, 2);
    if(cheater && eqmatrix(View, lView) && !centering) {
      View = Id; adjust_eye(View, cwt.at);
      static ld cyc = 0;
      cyc += 90._deg;
      View = spin(cyc) * View;
      if(GDIM == 2) View = spin(M_PI + vid.fixed_facing_dir * degree) * View;
      if(GDIM == 3 && !gproduct) View = cspin90(0, 2) * View;
      }
    }
  else if(currentmap) {
    centerover = currentmap->gamestart();
    View = Id; adjust_eye(View, cwt.at);
    }

  cwtV = shiftless(View);
  current_display->which_copy = 
    nonisotropic ? gpushxto0(tC0(view_inverse(View))) :
    View;
  // SDL_LockSurface(s);
  // SDL_UnlockSurface(s);
  }


EX void panning(shiftpoint hf0, shiftpoint ht0) {
  hyperpoint hf = hf0.h;
  hyperpoint ht = unshift(ht0, hf0.shift);
  View = 
    rgpushxto0(hf) * rgpushxto0(gpushxto0(hf) * ht) * gpushxto0(hf) * View;
  playermoved = false;
  }

EX int cells_drawn, cells_generated;

EX void fullcenter() {
  if(history::saved_ends == 0)
    history::path_for_lineanimation.clear();
  if(playerfound && false) centerpc(INF);
  else {
    bfs();
    resetview(); View = inverse(View);
    drawthemap();
    if(!centering) centerpc(INF);
    centerover = cwt.at;
    }
  playermoved = !centering;
  }

transmatrix screenpos(ld x, ld y) {
  transmatrix V = Id;
  V[0][2] += (x - current_display->xcenter) / current_display->radius * (1+pconf.alpha);
  V[1][2] += (y - current_display->ycenter) / current_display->radius * (1+pconf.alpha);
  return V;
  }

/** 
  In 3D, we use the standard translation matrices to place stuff on the screen.
  In 2D, this does not work (as HyperRogue reduces matrices to 3x3) so we use the native disk projection
*/

EX int flat_on;
eGeometry backup_geometry;
eVariation backup_variation;
videopar backup_vid;

/** \brief enable the 'flat' model for drawing HUD. See hr::flat_model_enabler */
EX void enable_flat_model(int val) {
  if(flat_on < 1 && flat_on + val >= 1) {
    #if CAP_GL
    glClear(GL_DEPTH_BUFFER_BIT);
    #endif
    backup_geometry = geometry;
    backup_variation = variation;
    backup_vid = vid;
    geometry = gNormal;
    variation = eVariation::bitruncated;
    pmodel = mdDisk;
    pconf.alpha = 1;
    pconf.scale = 1;
    pconf.camera_angle = 0;
    pconf.stretch = 1;
    
    vid.always3 = false;
    vid.wall_height = .3;
    vid.human_wall_ratio = .7;
    vid.camera = 1;
    vid.depth = 1;
    geom3::apply_always3();
    check_cgi();
    cgi.require_shapes();
    calcparam();
    }
  if(flat_on >= 1 && flat_on + val < 1) {
    geometry = backup_geometry;
    variation = backup_variation;
    vid = backup_vid;
    geom3::apply_always3();
    calcparam();
    check_cgi();
    }
  flat_on += val;
  }

#if HDR
struct flat_model_enabler {
  flat_model_enabler() { enable_flat_model(+1); }
  ~flat_model_enabler() { enable_flat_model(-1); }
  };
#endif

EX transmatrix atscreenpos(ld x, ld y, ld size) {
  transmatrix V = Id;
  
  if(pmodel == mdPixel) {
    V[0][3] += (x - current_display->xcenter);
    V[1][3] += (y - current_display->ycenter);
    V[0][0] = size * 2 * cgi.hcrossf / cgi.crossf;
    V[1][1] = size * 2 * cgi.hcrossf / cgi.crossf;
    if(WDIM == 3) V[2][2] = -1;
    }
  else if(pmodel == mdHorocyclic) {
    V[0][3] += (x - current_display->xcenter) * 2 / current_display->radius;
    V[1][3] += (y - current_display->ycenter) * 2/ current_display->radius;
    V[0][0] = size * 2 / current_display->radius;
    V[1][1] = size * 2 / current_display->radius;
    }
  else { 
    V[0][2] += (x - current_display->xcenter);
    V[1][2] += (y - current_display->ycenter);
    V[0][0] = size * 2 * cgi.hcrossf / cgi.crossf;
    V[1][1] = size * 2 * cgi.hcrossf / cgi.crossf;
    V[2][2] = current_display->radius;
    if(S3 >= OINF) V[0][0] /= 5, V[1][1] /= 5;
    }

  return V;
  }

void circle_around_center(ld radius, color_t linecol, color_t fillcol, PPR prio) {
  #if CAP_QUEUE
  if(among(pmodel, mdDisk, mdEquiarea, mdEquidistant, mdFisheye) && !(pmodel == mdDisk && hyperbolic && pconf.alpha <= -1) && pconf.camera_angle == 0) {
    hyperpoint ret;
    applymodel(shiftless(xpush0(radius)), ret);
    ld r = hypot_d(2, ret);
    queuecircle(current_display->xcenter, current_display->ycenter, r * current_display->radius, linecol, prio, fillcol);
    return;
    }  
  #endif
  #if CAP_QUEUE
  for(int i=0; i<=360; i++) curvepoint(xspinpush0(i * degree, 10));
  auto& c = queuecurve(shiftless(Id), linecol, fillcol, prio);
  if(pmodel == mdDisk && hyperbolic && pconf.alpha <= -1)
    c.flags |= POLY_FORCE_INVERTED;
  if(pmodel == mdJoukowsky)
    c.flags |= POLY_FORCE_INVERTED;
  c.flags |= POLY_ALWAYS_IN;
  #endif
  }

EX color_t periodcolor = 0x00FF0080;
EX color_t ringcolor = 0xFFFF;
EX color_t modelcolor = 0;

#if CAP_QUEUE
EX void draw_model_elements() {

  #if CAP_VR
  if(vrhr::active() && models::is_hyperboloid(pmodel)) return;
  #endif

  dynamicval<ld> lw(vid.linewidth, vid.linewidth * vid.multiplier_ring);
  switch(pmodel) {
  
    case mdRelOrthogonal:
    case mdRelPerspective: {
      constexpr ld cc = 3;
      if(sl2) for(ld dist: {-0.1, 0.1}) {
        transmatrix Lorentz = Id;
        Lorentz[0][0] = Lorentz[2][2] = cosh(cc);
        Lorentz[0][2] = Lorentz[2][0] = sinh(cc);
        hyperpoint h = Lorentz * cspin(3, 2, dist) * C0;
        for(int s=0; s<=360; s++)
          curvepoint(spin(s*degree) * h);
        queuecurve(shiftless(Id), ringcolor, 0, PPR::CIRCLE);
        }
      if(hyperbolic) for(ld dist: {-0.1, 0.1}) {
        transmatrix Lorentz = Id;
        Lorentz[0][0] = Lorentz[3][3] = cosh(cc);
        Lorentz[0][3] = Lorentz[3][0] = sinh(cc);
        hyperpoint h = Lorentz * hyperpoint(0, 0, cosh(dist), sinh(dist));
        for(int s=0; s<=360; s++)
          curvepoint(spin(s*degree) * h);
        queuecurve(shiftless(Id), ringcolor, 0, PPR::CIRCLE);
        }
      return;
      }

    case mdRotatedHyperboles: {
      queuestr(current_display->xcenter, current_display->ycenter + current_display->radius * pconf.alpha, 0, vid.fsize, "X", ringcolor, 1, 8);
      return;
      }
    
    case mdTwoHybrid: {
      queuereset(mdPixel, PPR::CIRCLE);
      
      for(int mode=0; mode<4; mode++) {
        for(int s=-200; s<=200; s ++) {
          ld p = tanh(s / 40.);
          ld a = pconf.twopoint_param * (1+p);
          ld b = pconf.twopoint_param * (1-p);
          ld h = ((mode & 2) ? -1 : 1) * sqrt(asin_auto(tan_auto(a) * tan_auto(b)));
          
          hyperpoint H = xpush(p * pconf.twopoint_param) * ypush0(h);
          
          hyperpoint res = compute_hybrid(H, 2 | mode);
          models::apply_orientation(res[0], res[1]);
          models::apply_orientation_yz(res[2], res[1]);
          curvepoint(res * current_display->radius);
          }
        queuecurve(shiftless(Id), ringcolor, 0, PPR::CIRCLE);
        }

      queuereset(pmodel, PPR::CIRCLE);
      goto fallthrough;
      }

    case mdTwoPoint: case mdSimulatedPerspective: fallthrough: {
      if(set_multi) return; /* no need */
      ld a = -pconf.model_orientation * degree;
      queuestr(shiftless(xspinpush0(a, +pconf.twopoint_param)), vid.xres / 100, "X", ringcolor >> 8);
      queuestr(shiftless(xspinpush0(a, -pconf.twopoint_param)), vid.xres / 100, "X", ringcolor >> 8);
      return;
      }
    
    case mdThreePoint: {
      vid.linewidth *= 5;
      for(int i=0; i<=3; i++) {
        hyperpoint h = xspinpush0(120._deg*i, pconf.twopoint_param);
        models::apply_orientation(h[1], h[0]);
        models::apply_orientation_yz(h[2], h[1]);
        curvepoint(h);
        }
      
      queuecurve(shiftless(Id), ringcolor, 0, PPR::SUPERLINE);
      vid.linewidth /= 5;
      return;
      }
    
    case mdBall: {
      queuecircle(current_display->xcenter, current_display->ycenter, current_display->radius, ringcolor, PPR::OUTCIRCLE, modelcolor);
      ballgeometry();
      return;
      }
    
    case mdHyperboloid:
    case mdHemisphere: {
      if(!pconf.show_hyperboloid_flat) return;
      if(models::is_hyperboloid(pmodel)) {
#if CAP_QUEUE
        curvepoint(point3(0,0,1));
        curvepoint(point3(0,0,-pconf.alpha));
        queuecurve(shiftless(Id), ringcolor, 0, PPR::CIRCLE);
        
        ld& tz = pconf.top_z;
        ld z = acosh(tz);
    
        hyperpoint a = xpush0(z);
        ld cb = models::cos_ball;
        ld sb = models::sin_ball;
        
        a[1] = sb * a[2] / -cb;
        a[0] = sqrt(-1 + a[2] * a[2] - a[1] * a[1]);
    
        curvepoint(point3(0,0,-pconf.alpha));
        curvepoint(a);
        curvepoint(point3(0,0,0));
        a[0] = -a[0];
        curvepoint(a);
        curvepoint(point3(0,0,-pconf.alpha));
        queuecurve(shiftless(Id), ringcolor, 0, PPR::CIRCLE);
    
        curvepoint(point3(-1,0,0));
        curvepoint(point3(1,0,0));
        queuecurve(shiftless(Id), ringcolor, 0, PPR::CIRCLE);
    
        a[1] = sb * tz / -cb;
        a[0] = sqrt(tz * tz - a[1] * a[1]);
        a[2] = tz - pconf.alpha;
    
        curvepoint(a);
        curvepoint(point3(0,0,-pconf.alpha));
        a[0] = -a[0];
        curvepoint(a);
        queuecurve(shiftless(Id), ringcolor, 0, PPR::CIRCLE);
#endif
        }
      return;
      }

    default: break;
    }
  }
 
void queuestraight(hyperpoint X, int style, color_t lc, color_t fc, PPR p) {

  hyperpoint H0, H1;
  applymodel(shiftless(X), H0);
  H0 *= current_display->radius;
  ld mul0 = hypot(vid.xres, vid.yres) / hypot_d(2, H0);
  
  if(style == 1) {
    H1 = H0 * -mul0;
    }
  else {
    applymodel(shiftless(pispin * X), H1);
    H1 *= current_display->radius;
    }

  ld mul1 = hypot(vid.xres, vid.yres) / hypot_d(2, H1);
  
  queuereset(mdPixel, p);
  curvepoint(H0 + spin90() * H0 * mul0);
  curvepoint(H0 - spin90() * H0 * mul0);
  curvepoint(H1 + spin90() * H1 * mul1);
  curvepoint(H1 - spin90() * H1 * mul1);
  curvepoint(H0 + spin90() * H0 * mul0);
  
  queuecurve(shiftless(Id), lc, fc, p).flags |= POLY_ALWAYS_IN | POLY_FORCEWIDE;
  queuereset(pmodel, p);
  /*
  for(int i=0; i<1; i++) {
    hyperpoint h = spin(i * 45 * degree) * X;
    hyperpoint res;
    applymodel(h, res);
    if(hypot2(res) < 1000 && !std::isnan(res[0]) && !std::isnan(res[1]))
      queuestr(h, 16, "X", 0xFF0000 + i * 0x20);
    } */
  }

EX void draw_boundary(int w) {

  if(w == 1) return;
  if(nonisotropic || euclid || gproduct) return;
  #if CAP_VR
  if(vrhr::active() && pmodel == mdHyperboloid) return;
  #endif

  dynamicval<ld> lw(vid.linewidth, vid.linewidth * vid.multiplier_ring);

  color_t lc = ringcolor;
  color_t fc = modelcolor;
  PPR p = PPR::OUTCIRCLE;

  if(haveaura()) lc = 0;
  if(lc == 0 && fc == 0) return;
  if(pmodel == mdRotatedHyperboles) return;
  
  ld fakeinf = sphere ? M_PI-1e-5 : hyperbolic ? 10 : exp(10);
  
  #if CAP_SVG
  dynamicval<ld> dw(vid.linewidth, vid.linewidth * (svg::in ? svg::divby : 1));
  #endif

  if(elliptic && !among(pmodel, mdBand, mdBandEquidistant, mdBandEquiarea, mdSinusoidal, mdMollweide, mdCollignon))
    circle_around_center(90._deg, periodcolor, 0, PPR::CIRCLE);
  
  int broken_coord = models::get_broken_coord(pmodel);
  if(broken_coord) {
    int unbroken_coord = 3 - broken_coord;
    const ld eps = 1e-3;
    const ld rem = sqrt(1-eps*eps);
    for(int s: {-1, 1}) {
      for(int a=1; a<180; a++) {
        hyperpoint h = Hypc;
        h[broken_coord] = -sin_auto(a*degree) * rem;
        h[0] = sin_auto(a*degree) * eps * s;
        h[unbroken_coord] = cos_auto(a*degree);
        models::apply_orientation(h[1], h[0]);
        curvepoint(h);
        }
      queuecurve(shiftless(Id), periodcolor, 0, PPR::CIRCLE).flags |= POLY_FORCEWIDE;
      }
    }
  
  if(pmodel == mdWerner && hyperbolic) return;

  switch(pmodel) {
  
    case mdTwoPoint: {
      if(twopoint_do_flips || current_display->stereo_active() || !sphere) return;
      queuereset(mdPixel, p);
  
      for(int b=-1; b<=1; b+=2)
      for(ld a=-90; a<=90+1e-6; a+=pow(.5, vid.linequality)) {
        ld x = sin(a * pconf.twopoint_param * b / 90);
        ld y = 0;
        ld z = -sqrt(1 - x*x);
        models::apply_orientation(y, x);
        hyperpoint h1;
        applymodel(shiftless(hpxyz(x,y,z)), h1);
        
        models::apply_orientation(h1[0], h1[1]);      
        h1[1] = abs(h1[1]) * b;
        models::apply_orientation(h1[1], h1[0]);
        curvepoint(h1);
        }
  
      queuecurve(shiftless(Id), lc, fc, p).flags |= POLY_FORCEWIDE;
      queuereset(pmodel, p);
      return;
      }
    
    case mdBand: case mdBandEquidistant: case mdBandEquiarea: case mdSinusoidal: case mdMollweide: case mdCentralCyl: case mdCollignon: 
    case mdGallStereographic: case mdMiller:
    {
      if(GDIM == 3) return;
      if(pmodel == mdBand && pconf.model_transition != 1) return;
      bool bndband = (among(pmodel, mdBand, mdMiller, mdGallStereographic, mdCentralCyl) ? hyperbolic : sphere);
      transmatrix T = spin(-pconf.model_orientation * degree);
      ld right = 90._deg - 1e-5;
      if(bndband) 
        queuestraight(T * ypush0(hyperbolic ? 10 : right), 2, lc, fc, p);
      ld xperiod = elliptic ? fakeinf/2 : fakeinf;
      if(sphere && !bndband) {
        queuestraight(T * xpush0(xperiod), 2, periodcolor, 0, PPR::CIRCLE);
        }
      if(sphere && bndband) {
        ld adegree = degree-1e-6;
        for(ld a=-90; a<90+1e-6; a+=pow(.5, vid.linequality)) {
          curvepoint(T * xpush(xperiod) * ypush0(a * adegree));
          }
        for(ld a=-90; a<90+1e-6; a+=pow(.5, vid.linequality)) {
          curvepoint(T * xpush(-xperiod) * ypush0(-a * adegree));
          }
        curvepoint(T * xpush(xperiod) * ypush0(-90 * adegree));
        queuecurve(shiftless(Id), periodcolor, 0, PPR::CIRCLE).flags |= POLY_FORCEWIDE;
        }
      return;
      }
    
    case mdHalfplane: 
      if(hyperbolic && GDIM == 2) {
        queuestraight(xspinpush0(-pconf.model_orientation * degree - 90._deg, fakeinf), 1, lc, fc, p);
        return;
        }
      break;
    
    case mdHemisphere: {
      if(hyperbolic) {
        queuereset(mdPixel, p);
        for(int i=0; i<=360; i++) {
          ld s = sin(i * degree);
          curvepoint(point3(current_display->radius * cos(i * degree), current_display->radius * s * (models::cos_ball * s >= 0 - 1e-6 ? 1 : abs(models::sin_ball)), 0));
          }
        queuecurve(shiftless(Id), lc, fc, p);
        queuereset(pmodel, p);
        p = PPR::CIRCLE; fc = 0;
        queuereset(mdPixel, p);
  
        for(int i=0; i<=360; i++) {
          ld s = sin(i * degree);
          curvepoint(point3(current_display->radius * cos(i * degree), current_display->radius * s * models::sin_ball, 0));
          }
        queuecurve(shiftless(Id), lc, fc, p);
        queuereset(pmodel, p);
        }
      if(euclid) {
        queuereset(mdPixel, p);  
        for(int i=0; i<=360; i++) {
          curvepoint(point3(current_display->radius * cos(i * degree), current_display->radius * sin(i * degree), 0));
          }
        queuecurve(shiftless(Id), lc, fc, p);
        queuereset(pmodel, p);
        }
      if(sphere) goto as_hyperboloid;
      return;
      }
    
    case mdHyperboloid: {
      if(hyperbolic) {
        as_hyperboloid:
        ld& tz = pconf.top_z;
        ld mz = sphere ? atan(sqrt(tz*tz-1)) : acosh(tz);
        ld cb = models::cos_ball;
        ld sb = models::sin_ball;
        
        if(abs(sb) <= abs(cb) + 1e-5) {
          ld step = .01 / (1 << vid.linequality);        
    
          hyperpoint a;
  
          for(ld t=-1; t<=1; t += step) {
  
            a = xpush0(t * mz);
            
            if(t != 0) {
              a[1] = sb * a[2] / -cb;
              ld v = -1 + a[2] * a[2] - a[1] * a[1];
              if(v < 0) continue;
              a[0] = sqrt(v);
              if(t < 0) a[0] = -a[0];
              }
            
            curvepoint(a);
            }
          
          if((sb > 0) ^ (cb < 0)) {
            ld alpha = M_PI - atan2(a[0], -a[1]);
            
            for(ld t=-1; t<=1; t += step)
              curvepoint(xspinpush0(-90._deg - t * alpha, mz));
            }
          else {
            ld alpha = - atan2(a[0], -a[1]);
            
            for(ld t=-1; t<=1; t += step)
              curvepoint(xspinpush0(+90._deg - t * alpha, mz));
            }
          
          queuecurve(shiftless(Id), lc, fc, p);
          fc = 0; p = PPR::CIRCLE;
          }

        for(ld t=0; t<=360; t ++)
          curvepoint(xspinpush0(t * degree, mz));

        queuecurve(shiftless(Id), lc, fc, p);

        if(sphere) {
          for(ld t=0; t<=360; t ++)
            curvepoint(xspinpush0(t * degree, M_PI-mz));

          queuecurve(shiftless(Id), lc, fc, p);
          }
        }
      return;
      }

    case mdSpiral: {
      if(euclid) return;
      if(models::ring_not_spiral) return;
      // if(p == PPR::CIRCLE) p = PPR::OUTCIRCLE;
      auto& sm = models::spiral_multiplier;
      ld u = hypot(1, imag(sm) / real(sm));
      if(real(sm)) {
        queuereset(mdPixel, p);
        for(ld a=-10; a<=10; a+=0.01 / (1 << vid.linequality) / u) {
          cld z = exp(cld(a, a * imag(sm) / real(sm) + M_PI));
          hyperpoint ret = point2(real(z), imag(z));
          ret = mobius(ret, pconf.skiprope, 1);
          ret *= current_display->radius;
          curvepoint(ret);
          }
        queuecurve(shiftless(Id), ringcolor, 0, p).flags |= POLY_ALWAYS_IN;
        queuereset(pmodel, p);
        }
      return;
      }

    default: break;
    }

  if(sphere && pmodel == mdDisk && pconf.alpha > 1) {
    double rad = current_display->radius / sqrt(pconf.alpha*pconf.alpha - 1);
    queuecircle(current_display->xcenter, current_display->ycenter, rad, lc, p, fc);
    return;
    }

  if(sphere && !among(pmodel, mdEquidistant, mdEquiarea)) return;
  circle_around_center(fakeinf, lc, fc, p);
  }
#endif

EX void change_shift(shiftpoint& h, ld by) {
  if(!by) return;
  h.shift += by;
  if(sl2) {
    ld ca = cos(by), sa = sin(by);
    tie(h[2], h[3]) = make_pair(h[2] * ca - h[3] * sa, h[3] * ca + h[2] * sa);
    tie(h[0], h[1]) = make_pair(h[0] * ca - h[1] * sa, h[1] * ca + h[0] * sa);
    }
  else if((mdinf[pmodel].flags & mf::uses_bandshift) || (sphere && pmodel == mdSpiral)) {
    h.h = spin(pconf.model_orientation * degree) * h.h;
    h.h = xpush(-by) * h.h;
    h.h = spin(-pconf.model_orientation * degree) * h.h;
    }
  }

EX void change_shift(shiftmatrix& T, ld by) {
  if(!by) return;
  T.shift += by;
  if(sl2) {
    ld ca = cos(by), sa = sin(by);
    for(int a=0; a<4; a++) {
      tie(T[2][a], T[3][a]) = make_pair(T[2][a] * ca - T[3][a] * sa, T[3][a] * ca + T[2][a] * sa);
      tie(T[0][a], T[1][a]) = make_pair(T[0][a] * ca - T[1][a] * sa, T[1][a] * ca + T[0][a] * sa);
      }
    }
  else if((mdinf[pmodel].flags & mf::uses_bandshift) || (sphere && pmodel == mdSpiral)) {
    T.T = spin(pconf.model_orientation * degree) * T.T;
    T.T = xpush(-by) * T.T;
    fixmatrix(T.T);
    T.T = spin(-pconf.model_orientation * degree) * T.T;
    }
  }

EX transmatrix unshift(shiftmatrix T, ld to IS(0)) {
  change_shift(T, to - T.shift);
  return T.T;
  }

EX hyperpoint unshift(shiftpoint T, ld to IS(0)) {
  change_shift(T, to - T.shift);
  return T.h;
  }

EX transmatrix inverse_shift(const shiftmatrix& T1, const shiftmatrix& T2) {
  return iso_inverse(T1.T) * unshift(T2, T1.shift);
  }

EX hyperpoint inverse_shift(const shiftmatrix& T1, const shiftpoint& T2) {
  return iso_inverse(T1.T) * unshift(T2, T1.shift);
  }

EX void optimize_shift(shiftpoint& h) {
  if(sl2) {
    change_shift(h, atan2(h[2], h[3]));
    }
  }

EX void optimize_shift(shiftmatrix& T) {

  if(sl2) {
    change_shift(T, atan2(T[2][3], T[3][3]));
    if(hybrid::csteps) {
      auto period = (M_PI * hybrid::csteps) / cgi.psl_steps;
      while(T.shift > period*.4999)
        T.shift -= period;
      while(T.shift < -period*.5001)
        T.shift += period;
      }
    return;
    }

  else if(((mdinf[pmodel].flags & mf::uses_bandshift) && T[LDIM][LDIM] > 30) || (sphere && pmodel == mdSpiral)) {
    T.T = spin(pconf.model_orientation * degree) * T.T;
    hyperpoint H = tC0(T.T);
    find_zlev(H);
    
    ld y = asin_auto(H[1]);
    ld x = asin_auto_clamp(H[0] / cos_auto(y));
    if(sphere) {
      if(H[LDIM] < 0 && x > 0) x = M_PI - x;
      else if(H[LDIM] < 0 && x <= 0) x = -M_PI - x;
      }
    T.shift += x;
    T.T = xpush(-x) * T.T;
    fixmatrix(T.T);
    T.T = spin(-pconf.model_orientation * degree) * T.T;
    }
  }

EX shiftmatrix optimized_shift(const shiftmatrix& T) {
  shiftmatrix U = T; 
  optimize_shift(U);
  return U;
  }

EX namespace dq {
  EX queue<pair<heptagon*, shiftmatrix>> drawqueue;
  
  EX unsigned bucketer(const shiftpoint& T) {
    return bucketer(T.h) + unsigned(floor(T.shift*81527+.5));
    }

  EX set<heptagon*> visited;
  EX void enqueue(heptagon *h, const shiftmatrix& T) {
    if(!h || visited.count(h)) { return; }
    visited.insert(h);
    drawqueue.emplace(h, T);
    }  

  EX set<unsigned> visited_by_matrix;
  EX void enqueue_by_matrix(heptagon *h, const shiftmatrix& T) {
    if(!h) return;
    unsigned b = bucketer(tC0(T));
    if(visited_by_matrix.count(b)) { return; }
    visited_by_matrix.insert(b);
    drawqueue.emplace(h, T);
    }

  EX queue<pair<cell*, shiftmatrix>> drawqueue_c;
  EX set<cell*> visited_c;

  EX void enqueue_c(cell *c, const shiftmatrix& T) {
    if(!c || visited_c.count(c)) { return; }
    visited_c.insert(c);
    drawqueue_c.emplace(c, T);
    }

  EX void enqueue_by_matrix_c(cell *c, const shiftmatrix& T) {
    if(!c) return;
    unsigned b = bucketer(tC0(T));
    if(visited_by_matrix.count(b)) { return; }
    visited_by_matrix.insert(b);
    drawqueue_c.emplace(c, T);
    }
  
  EX void clear_all() {
    visited.clear();
    visited_by_matrix.clear();
    visited_c.clear();
    while(!drawqueue_c.empty()) drawqueue_c.pop();
    while(!drawqueue.empty()) drawqueue.pop();
    }


  EX }

EX bool do_draw(cell *c) {
  // do not display out of range cells, unless on torus
  if(c->pathdist == PINFD && !(euclid && quotient) && vid.use_smart_range == 0)
    return false;
  // do not display not fully generated cells, unless changing range allowed
  if(c->mpdist > 7 && !allowChangeRange()) return false;
  // in the Yendor Challenge, scrolling back is forbidden
  if(c->cpdist > get_sightrange() && (yendor::on || isHaunted(cwt.at->land)) && !cheater && !autocheat) return false;

  return true;
  }  

EX ld extra_generation_distance = 99;

// returns false if limited
bool limited_generation(cell *c) {
  if(c->mpdist <= 7) return true;
  if(cells_generated > vid.cells_generated_limit) return false;
  setdist(c, 7, c);
  cells_generated++;
  return true;
  }

EX bool do_draw(cell *c, const shiftmatrix& T) {

  if(WDIM == 3) {
    // do not care about cells outside of the track
    if(GDIM == 3 && racing::on && c->land == laMemory && cells_drawn >= S7+1) return false;

    if(cells_drawn > vid.cells_drawn_limit) return false;
    if(cells_drawn < 50) { limited_generation(c); return true; }
    #if MAXMDIM >= 4
    if(nil && models::is_perspective(pmodel)) {
      ld dist = hypot_d(3, inverse_exp(tC0(T), pQUICK));
      if(dist > sightranges[geometry] + (vid.sloppy_3d ? 0 : 0.9)) return false;
      if(dist <= extra_generation_distance && !limited_generation(c)) return false;
      }
    else if(sol && models::is_perspective(pmodel)) {
      if(!nisot::in_table_range(tC0(T.T))) return false;
      if(!limited_generation(c)) return false;
      }
    else if(nih && models::is_perspective(pmodel)) {
      hyperpoint h = inverse_exp(tC0(T), pQUICK);
      ld dist = hypot_d(3, h);
      if(dist > sightranges[geometry] + (vid.sloppy_3d ? 0 : cgi.corner_bonus)) return false;
      if(dist <= extra_generation_distance && !limited_generation(c)) return false;
      }
    else if(sl2 && models::is_perspective(pmodel)) {
      if(hypot(tC0(T.T)[2], tC0(T.T)[3]) > cosh(slr::range_xy)) return false;
      if(abs(T.shift) > (slr::range_z)) return false;
      if(abs(T.shift * stretch::not_squared()) > sightranges[geometry]) return false;
      if(!limited_generation(c)) return false;
      }
    #endif
    else if(vid.use_smart_range) {
      if(cells_drawn >= 50 && !in_smart_range(T)) return false;
      if(!limited_generation(c)) return false;
      }
    else {
      ld dist = hdist0(tC0(T.T));
      if(dist > sightranges[geometry] + (vid.sloppy_3d ? 0 : cgi.corner_bonus)) return false;
      if(dist <= extra_generation_distance && !limited_generation(c)) return false;
      }
    return true;
    }

  #if MAXMDIM >= 4
  if(rots::drawing_underlying && euclid && hdist0(tC0(T)) > 6) return false;
  #endif
  if(just_gmatrix && sphere) return true;
  if(!do_draw(c)) return false;
  if(euclid && pmodel == mdSpiral) {
    hyperpoint h = tC0(T.T);
    cld z(h[0], h[1]);
    z = z * models::spiral_multiplier;
    ld iz = imag(z) + 1.14279e-2; // make it never fall exactly on PI
    if(iz < -M_PI || iz >= M_PI) return false;
    }
  if(pmodel == mdSpiral && models::ring_not_spiral) {
    cld z;
    shiftpoint H = tC0(T);
    hyperpoint ret;
    makeband(H, ret, band_conformal);
    z = cld(ret[0], ret[1]) * models::spiral_multiplier;
    if(imag(z) < -models::spiral_cone_rad/2-1e-5 || imag(z) >= models::spiral_cone_rad/2-1e-5) return false;
    }
  if(cells_drawn > vid.cells_drawn_limit) return false;
  bool usr = vid.use_smart_range || quotient;
  if(usr && cells_drawn >= 50 && !in_smart_range(T) && !(WDIM == 2 && GDIM == 3 && hdist0(tC0(T)) < 2.5)) return false;
  if(vid.use_smart_range == 2 && !limited_generation(c)) return false;
  return true; 
  }

EX int cone_side(const shiftpoint H) {
  hyperpoint ret;
  if(hyperbolic) makeband(H, ret, band_conformal);
  else ret = unshift(H);
  cld z = cld(ret[0], ret[1]) * models::spiral_multiplier;
  
  auto zth = [&] (cld z) {
    ld alpha = imag(z) * 360 / pconf.spiral_cone;
    ld r = real(z);
    r = exp(r);
    
    hyperpoint ret;
      
    ret[0] = -sin(alpha) * r;
    ret[1] = cos(alpha) * r;
    ret[2] = (r-1) * sqrt( pow(360/pconf.spiral_cone, 2) - 1);
    
    models::apply_ball(ret[2], ret[1]);
    return ret;
    };
  
  hyperpoint ret0 = zth(z);
  hyperpoint ret1 = zth(z + cld(1e-3, 0));
  hyperpoint ret2 = zth(z + cld(0, 1e-3));

  return (ret1[1] - ret0[1]) * (ret2[0] - ret0[0]) < (ret2[1] - ret0[1]) * (ret1[0] - ret0[0]) ? 1 : -1;
  }

/** get the current orientation of the view */
EX transmatrix& get_view_orientation() {
  return gproduct ? NLP : View;
  }

EX hookset<bool(const transmatrix&)> hooks_rotate_view;
EX hookset<bool(const hyperpoint&)> hooks_shift_view;

/** rotate the view using the given rotation matrix */
EX void rotate_view(transmatrix T) {
  if(callhandlers(false, hooks_rotate_view, T)) return;
  transmatrix& which = get_view_orientation();
  which = T * which;
  if(!gproduct && !rug::rugged) current_display->which_copy = T * current_display->which_copy;
  }

EX hyperpoint lie_exp(hyperpoint h) {
  if(nil) {
    h[3] = 1;
    h[2] += h[0] * h[1] / 2;
    }
  else if(sol && !nih) {
    h[3] = 1;
    if(abs(h[2]) > 1e-6) {
      h[0] *= (exp(-h[2]) - 1) / -h[2];
      h[1] *= (exp(+h[2]) - 1) / h[2];
      }
    }
  else if(sol && nih) {
    h[3] = 1;
    if(abs(h[2]) > 1e-6) {
      ld z = h[2] * log(2);
      h[0] *= (exp(-z) - 1) / -z;
      z = h[2] * log(3);
      h[1] *= (exp(+z) - 1) / z;
      }
    }
  else if(nih) {
    h[3] = 1;
    if(abs(h[2]) > 1e-6) {
      ld z = h[2] * log(2);
      h[0] *= (exp(+z) - 1) / z;
      z = h[2] * log(3);
      h[1] *= (exp(+z) - 1) / z;
      }
    }
  else {
    /* not implemented -- approximate for now */
    const int steps = 16;
    h /= (1<<steps);
    h[3] = 1;
    normalize(h);
    transmatrix T = eupush(h);
    for(int i=0; i<16; i++) T = T * T;
    h = tC0(T);
    }
  return h;
  }

EX hyperpoint rel_log(shiftpoint h) {
  if(sl2) {
    optimize_shift(h);
    ld cycles = floor(h.shift / TAU + .5);
    hyperpoint h1 = unshift(h);
    ld choice = h1[2] * h1[2] - h1[0] * h1[0] - h1[1] * h1[1];
    ld r, z;
    if(choice > 0) {
      ld r = sqrt(choice);
      ld z = asin_clamp(r);
      if(h1[3] < 0) z = M_PI - z;
      z += cycles * TAU;
      }
    else if(cycles || h1[3] < -1 || choice == 0) {
      /* impossible, or light-like */
      r = 1; z = 0;
      }
    else {
      r = sqrt(-choice);
      z = asinh(r);
      }
    h1 = h1 * z / r;
    h1[3] = 0;
    return h1;
    }
  if(hyperbolic && GDIM == 3) {
    hyperpoint h1 = h.h;
    ld choice = h1[3] * h1[3] - h1[0] * h1[0] - h1[1] * h1[1];
    ld r, z;
    if(choice > 0) { r = sqrt(choice); z = asinh(r); }
    else { r = sqrt(-choice); z = asin_clamp(r); if(h1[2] < 0) z = M_PI - z; }
    h1 = h1 * z / r; h1[2] = h1[3]; h1[3] = 0;
    return h1;
    }
  throw hr_exception("rel_log in wrong geometry");
  }

EX hyperpoint lie_log(hyperpoint h) {
  if(nil) {
    h[3] = 0;
    h[2] -= h[0] * h[1] / 2;
    }
  else if(sol && !nih) {
    h[3] = 0;
    if(abs(h[2]) > 1e-6) {
      h[0] *= -h[2] / (exp(-h[2]) - 1);
      h[1] *= h[2] / (exp(+h[2]) - 1);
      }
    }
  else if(sol && nih) {
    h[3] = 0;
    if(abs(h[2]) > 1e-6) {
      ld z = h[2] * log(2);
      h[0] *= -z / (exp(-z) - 1);
      z = h[2] * log(3);
      h[1] *= z / (exp(+z) - 1);
      }
    }
  else if(nih) {
    h[3] = 1;
    if(abs(h[2]) > 1e-6) {
      ld z = h[2] * log(2);
      h[0] *= z / (exp(+z) - 1);
      z = h[2] * log(3);
      h[1] *= z / (exp(+z) - 1);
      }
    }
  else if(euclid) {
    h[LDIM] = 0;
    }
  else if(hyperbolic) {
    h = deparabolic13(h);
    if(abs(h[0]) > 1e-6)
      for(int i=1; i<LDIM; i++)
        h[i] *= h[0] / (exp(h[0])-1);
    }
  else {
    /* not implemented */
    }
  return h;
  }

/** shift the view according to the given tangent vector */
EX transmatrix get_shift_view_of(const hyperpoint H, const transmatrix V) {
  if(!nonisotropic && !stretch::in()) {
    return rgpushxto0(direct_exp(lp_iapply(H))) * V;
    }
  else if(!nisot::geodesic_movement) {
    transmatrix IV = view_inverse(View);
    transmatrix view_shift = eupush( tC0(IV) );
    transmatrix rot = V * view_shift;
    hyperpoint tH = lie_exp(inverse(rot) * H);
    return rot * eupush(tH) * inverse(view_shift);
    }
  else {
    return iview_inverse(nisot::parallel_transport(view_inverse(V), -H));
    }
  }

/** shift the view according to the given tangent vector */
EX void shift_view(hyperpoint H) {
  if(callhandlers(false, hooks_shift_view, H)) return;
  static bool recursive = false;
  if(!recursive && intra::in) {
    dynamicval<bool> r(recursive, true);
    #if MAXMDIM >= 4 && CAP_RAY
    intra::shift_view_portal(H);
    #endif
    return;
    }
  View = get_shift_view_of(H, View);
  auto& wc = current_display->which_copy;
  wc = get_shift_view_of(H, wc);
  }

/** works in embedded_plane and isotropic spaces */
void shift_view_isotropic(transmatrix T) {

  if(embedded_plane) {
    transmatrix IV = view_inverse(View);
    transmatrix rot = View * map_relative_push(IV * C0);
    View = T * View;
    transmatrix IV1 = view_inverse(View);
    transmatrix rot1 = View * map_relative_push(IV1 * C0);
    View = rot * inverse(rot1) * View;
    auto& wc = current_display->which_copy;
    wc = rot * inverse(rot1) * T * wc;
    return;
    }

  View = T * View;
  auto& wc = current_display->which_copy;
  wc = T * wc;
  }

/* like rgpushxto0 but keeps the map orientation correct */
EX transmatrix map_relative_push(hyperpoint h) {
  if(!embedded_plane) return rgpushxto0(h);
  if(geom3::same_in_same()) {
    ld z = -asin_auto(h[2]);
    ld u = 1 / cos_auto(z);
    auto h1 = hpxy3(h[0] * u, h[1] * u, 0);

    transmatrix T = rgpushxto0(h1) * zpush(-z);
    return T;
    }
  if(geom3::euc_in_hyp()) {
    auto h1 = deparabolic13(h);
    return parabolic13_at(h1);
    }
  if(msphere) {
    ld z = hdist0(h);
    geom3::light_flip(true);
    auto h1 = normalize(h);
    transmatrix T = rgpushxto0(h1);
    geom3::light_flip(false);
    return T * zpush(z);
    }
  return rgpushxto0(h);
  }

EX void shift_view_to(shiftpoint H) {
  if(!nonisotropic)
     shift_view_isotropic(gpushxto0(unshift(H)));
  else shift_view(-inverse_exp(H));
  }

EX void shift_view_towards(shiftpoint H, ld l) {
  if(!nonisotropic && !gproduct)
    shift_view_isotropic(rspintox(unshift(H)) * xpush(-l) * spintox(unshift(H)));
  else if(nonisotropic && !nisot::geodesic_movement)
    shift_view(tangent_length(unshift(H)-C0, -l));
  else {
    hyperpoint ie = inverse_exp(H, pNORMAL | pfNO_DISTANCE);
    if(gproduct) ie = lp_apply(ie);
    shift_view(tangent_length(ie, -l));
    }
  }

EX void set_view(hyperpoint camera, hyperpoint forward, hyperpoint upward) {
  if(GDIM == 2) {
    View = gpushxto0(camera);
    View = spin90() * spintox(View * upward) * View;
    return;
    }
  
  transmatrix V = gpushxto0(camera);
  forward = V * forward;
  upward = V * upward;
  
  if(pmodel == mdGeodesic || hyperbolic || sphere) {
    forward = inverse_exp(shiftless(forward));
    }
  else {
    // apply_nil_rotation(forward);
    forward -= C0;
    }
  
  if(hypot_d(3, forward) == 0) forward[0] = 1;
  
  forward /= hypot_d(3, forward);

  if(pmodel == mdGeodesic || hyperbolic || sphere)
    upward = inverse_exp(shiftless(upward));
  else {
    // apply_nil_rotation(upward);
    upward -= C0;
    }

  upward -= (forward|upward) * forward;
  if(hypot_d(3, upward) == 0) return;

  upward /= hypot_d(3, upward);

  hyperpoint rightward = (forward ^ upward);
  
  transmatrix rotator = Id;
  rotator[2] = forward;
  rotator[1] = -upward;
  rotator[0] = -rightward;
  
  if(det(rotator) < 0) rotator[0] = -rotator[0];
  
  View = iso_inverse(rgpushxto0(camera));
  if(gproduct)
    NLP = rotator;
  else
    View = rotator * View;
  }

}
