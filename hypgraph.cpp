// Hyperbolic Rogue -- hyperbolic graphics
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file hypgraph.cpp
 *  \brief mapping hyperpoints to the screen, and related functions
 */

#include "hyper.h"
namespace hr {

ld ghx, ghy, ghgx, ghgy;
hyperpoint ghpm = C0;

#if HDR
inline bool sphereflipped() { return sphere && vid.alpha > 1.1 && GDIM == 3; }
#endif

void ghcheck(hyperpoint &ret, const hyperpoint &H) {
  if(hypot(ret[0]-ghx, ret[1]-ghy) < hypot(ghgx-ghx, ghgy-ghy)) {
    ghpm = H; ghgx = ret[0]; ghgy = ret[1];
    }
  }

EX void camrotate(ld& hx, ld& hy) {
  ld cam = vid.camera_angle * degree;
  GLfloat cc = cos(cam);
  GLfloat ss = sin(cam);
  ld ux = hx, uy = hy * cc + ss, uz = cc - ss * hy;
  hx = ux / uz, hy = uy / uz;
  }

hyperpoint perspective_to_space(hyperpoint h, ld alpha = vid.alpha, eGeometryClass geo = ginf[geometry].cclass);

EX bool non_spatial_model() {
  if(among(pmodel, mdRotatedHyperboles, mdJoukowsky, mdJoukowskyInverted, mdPolygonal, mdPolynomial))
    return true;
  if(pmodel == mdSpiral && euclid)
    return true;
  return pmodel && vid.consider_shader_projection && (get_shader_flags() & SF_DIRECT);
  }

hyperpoint perspective_to_space(hyperpoint h, ld alpha, eGeometryClass gc) {
  ld hx = h[0], hy = h[1];
  
  if(gc == gcEuclid)
    return hpxy(hx * (1 + alpha), hy * (1 + alpha));
    
  ld hr = hx*hx+hy*hy;
  
  if(hr > .9999 && gc == gcHyperbolic) return Hypc;
  
  ld A, B, C;
  
  ld curv = gc == gcSphere ? 1 : -1;
  
  A = 1+curv*hr;
  B = 2*hr*vid.alpha*-curv;
  C = 1 - curv*hr*vid.alpha*vid.alpha;
  
  B /= A; C /= A;
  
  ld rootsign = 1;
  // if(gc == gcSphere && vid.alpha > 1) rootsign = -1;
  
  ld hz = B / 2 + rootsign * sqrt(C + B*B/4);
  
  hyperpoint H;
  H[0] = hx * (hz+vid.alpha);
  H[1] = hy * (hz+vid.alpha);
  H[LDIM] = hz;
  
  return H;  
  }

EX hyperpoint space_to_perspective(hyperpoint z, ld alpha IS(vid.alpha)) {
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

EX hyperpoint gethyper(ld x, ld y) {

  ld hx = (x - current_display->xcenter) / current_display->radius;
  ld hy = (y - current_display->ycenter) / current_display->radius / vid.stretch;
  
  if(pmodel) {
    ghx = hx, ghy = hy;
    return ghpm;
    }
  
  if(vid.camera_angle) camrotate(hx, hy);
  
  return perspective_to_space(hpxyz(hx, hy, 0));
  }

void ballmodel(hyperpoint& ret, double alpha, double d, double zl) {
  hyperpoint H = ypush(vid.camera) * xpush(d) * ypush(zl) * C0;
  ld tzh = vid.ballproj + H[LDIM];
  ld ax = H[0] / tzh;
  ld ay = H[1] / tzh;
  
  ld ca = cos(alpha), sa = sin(alpha);

  ret[0] = ax * ca;
  ret[1] = ay;
  ret[2] = ax * sa;
  
  models::apply_ball(ret[2], ret[1]);
  }

void apply_depth(hyperpoint &f, ld z) {
  if(vid.usingGL)
    f[2] = z;
  else {
    z = z * current_display->radius;
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
    else
      d = -hypot_auto(d, z);
    return true;
    }
  }

int twopoint_sphere_flips;
bool twopoint_do_flips;

ld find_zlev(hyperpoint& H) {

  if(spatial_graphics) {
    ld zlev = zlevel(H);
    if(zlev > 1-1e-6 && zlev < 1+1e-6) return 1;
    H /= zlev;
    return zlev;
    }  
  
  return 1;
  }

ld get_tz(hyperpoint H) {
  ld tz = euclid ? (1+vid.alpha) : vid.alpha+H[LDIM];
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
    H[3] = 1;
    }
  }

template<class T> void makeband(hyperpoint H, hyperpoint& ret, const T& f) {
  ld zlev = find_zlev(H);
  models::apply_orientation_yz(H[1], H[2]);
  models::apply_orientation(H[0], H[1]);
  auto r = move_z_to_y(H);
  
  ld x, y, yf, zf=0;
  y = asin_auto(H[1]);
  x = asin_auto_clamp(H[0] / cos_auto(y));
  if(sphere) {
    if(H[LDIM] < 0 && x > 0) x = M_PI - x;
    else if(H[LDIM] < 0 && x <= 0) x = -M_PI - x;
    }
  x += band_shift;
  hypot_zlev(zlev, y, yf, zf);
  
  f(x, y);
  
  ld yzf = y * zf; y *= yf;
  ret = hpxyz(x / M_PI, y / M_PI, 0);
  move_y_to_z(ret, r);
  models::apply_orientation(ret[1], ret[0]);
  models::apply_orientation_yz(ret[2], ret[1]);
  if(zlev != 1 && current_display->stereo_active()) 
    apply_depth(ret, yzf / M_PI);
  return;
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
  auto p = vid.twopoint_param;
  ld dleft = hypot_auto(x-p, y);
  ld dright = hypot_auto(x+p, y);
  if(sphere) {
    int tss = twopoint_sphere_flips;
    if(tss&1) { tss--; 
      dleft = 2*M_PI - 2*p - dleft;
      dright = 2*M_PI - 2*p - dright;
      swap(dleft, dright);
      y = -y;
      }
    while(tss) { tss -= 2;
      dleft = 2*M_PI - 4*p + dleft;
      dright = 2*M_PI - 4*p + dright;
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
  auto& t = vid.twopoint_param;
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

EX ld signed_sqrt(ld x) { return x > 0 ? sqrt(x) : -sqrt(-x); };

EX void applymodel(hyperpoint H, hyperpoint& ret) {

  hyperpoint H_orig = H;
  
  if(models::product_model()) {
    ld zlev = zlevel(H);
    H /= exp(zlev);
    hybrid::in_underlying_geometry([&] { applymodel(H, ret); });
    ret[2] = zlev * models::product_z_scale;
    ret = NLP * ret;
    return;    
    }
  
  switch(pmodel) {
    case mdPerspective: {
      ld ratio = vid.xres / current_display->tanfov / current_display->radius / 2;
      if(prod) H = product::inverse_exp(H);
      H = lp_apply(H);
      if(H[2] == 0) { ret[0] = 1e6; ret[1] = 1e6; ret[2] = 1; return; }
      ret[0] = H[0]/H[2] * ratio;
      ret[1] = H[1]/H[2] * ratio;
      ret[2] = 1;
      return;
      }

    case mdGeodesic: {
      auto S = lp_apply(inverse_exp(H, iTable));
      ld ratio = vid.xres / current_display->tanfov / current_display->radius / 2;
      ret[0] = S[0]/S[2] * ratio;
      ret[1] = S[1]/S[2] * ratio;
      ret[2] = 1;
      return;
      }
      
    case mdPixel:
      ret = H / current_display->radius;
      return; 
    
    case mdBall: {
      ld zlev = find_zlev(H);
      
      ld zl = vid.depth-geom3::factor_to_lev(zlev);
  
      ballmodel(ret, atan2(H), hdist0(H), zl);
      break;      
      }
    
    case mdDisk: {
      if(nonisotropic) {
        ret = lp_apply(inverse_exp(H, iTable, true));
        ld w;
        if(solnih) {
          // w = 1 / sqrt(1 - sqhypot_d(3, ret));
          // w = w / (vid.alpha + w);
          w = 1 / (sqrt(1 - sqhypot_d(3, ret)) * vid.alpha + 1);
          }
        else {
          w = hypot_d(3, ret);
          w = sinh(w) / ((vid.alpha + cosh(w)) * w);
          }
        for(int i=0; i<3; i++) ret[i] *= w;
        ret[3] = 1; 
        break;
        }
      ld tz = get_tz(H);
      if(!vid.camera_angle) {
        ret[0] = H[0] / tz;
        ret[1] = H[1] / tz;
        if(GDIM == 3) ret[2] = H[2] / tz;
        else ret[2] = vid.xres * current_display->eyewidth() / 2 / current_display->radius - vid.ipd / tz / 2;
        if(MAXMDIM == 4) ret[3] = 1;
        }
      else {
        ld tx = H[0];
        ld ty = H[1];
        ld cam = vid.camera_angle * degree;
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
      ld r = 0;
      for(int d=0; d<GDIM; d++) r += ret[d]*ret[d];
      for(int d=0; d<GDIM; d++) ret[d] /= r;
      return;
      }
    
    case mdHalfplane: {
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
        H *= models::halfplane_scale;
        ret[0] = -H[0];
        ret[1] = 1 + H[1];
        ret[2] = H[2];
        ret[3] = 1;
        models::apply_orientation(ret[1], ret[0]);
        models::apply_orientation_yz(ret[2], ret[1]);
        break;
        }
      
      models::apply_orientation(H[0], H[1]);
      
      H *= models::halfplane_scale;
      
      ret[0] = -models::osin - H[0];
      if(zlev != 1) {
        if(abs(models::ocos) > 1e-5)
          H[1] = H[1] * pow(zlev, models::ocos);
        if(abs(models::ocos) > 1e-5 && models::osin)
          H[1] += H[0] * models::osin * (pow(zlev, models::ocos) - 1) / models::ocos;
        else if(models::osin)
          H[1] += H[0] * models::osin * log(zlev);
        }
      ret[1] = models::ocos + H[1];
      ret[2] = GDIM == 3 ? H[2] : 0;
      if(MAXMDIM == 4) ret[3] = 1;
      if(zlev != 1 && current_display->stereo_active()) 
        apply_depth(ret, -H[1] * geom3::factor_to_lev(zlev));
      break;
      }
    
    case mdHorocyclic: {

      find_zlev(H);
      
      models::apply_orientation_yz(H[1], H[2]);
      models::apply_orientation(H[0], H[1]);
      
      ret = hyperbolic ? deparabolic10(H) : H;
      ret *= .5;
      ret[LDIM] = 1;
  
      models::apply_orientation(ret[1], ret[0]);
      models::apply_orientation_yz(ret[2], ret[1]);
      
      if(nonisotropic) ret = lp_apply(ret);
      
      break;
      }
    
    case mdHemisphere: {
  
      switch(cgclass) {
        case gcHyperbolic: {
          ld zl = zlevel(H);
          ret = H / H[2];
          ret[2] = sqrt(1 - sqhypot_d(2, ret));
          ret = ret * (1 + (zl - 1) * ret[2]);
          break;
          }
          
        case gcEuclid: default: {
          // stereographic projection to a sphere
          auto hd = hdist0(H) / vid.euclid_to_sphere;
          if(hd == 0) ret = hpxyz(0, 0, -1);
          else {
            ld x = 2 * hd / (1 + hd * hd);
            ld y = x / hd;
            ret = H * x / hd / vid.euclid_to_sphere;
            ret[2] = (1 - y);
            ret = ret * (1 + (H[2]-1) * y / vid.euclid_to_sphere);
            }
          break;
          }
        
        case gcSphere: {
          ret = H;
          break;
          }
        }
      
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
      if(prod) {
        ret = H;
        break;
        }
      if(pmodel == mdHyperboloid) {
        ld& topz = models::top_z;
        if(H[2] > topz) {
          ld scale = sqrt(topz*topz-1) / hypot_d(2, H);
          H *= scale;
          H[2] = topz;
          }
        }
      else {
        H = space_to_perspective(H, vid.alpha);
        H[2] = 1 - vid.alpha;
        }
  
      ret[0] = H[0] / 3;
      ret[1] = (1 - H[2]) / 3;
      ret[2] = H[1] / 3;
      
      models::apply_ball(ret[2], ret[1]);
      break;
      }
    
    case mdFisheye: {
      ld zlev;
      if(nonisotropic) {
        H = lp_apply(inverse_exp(H, iTable, false));
        zlev = 1;
        }
      else {
        zlev = find_zlev(H);
        H = space_to_perspective(H);
        }
      H /= vid.fisheye_param;
      H[LDIM] = zlev;
      ret = H / sqrt(1 + sqhypot_d(GDIM+1, H));
      if(GDIM == 3) ret[LDIM] = zlev;
      break;
      }
    
    case mdSimulatedPerspective: {
      models::apply_orientation_yz(H[1], H[2]);
      models::apply_orientation(H[0], H[1]);
      auto yz = move_z_to_y(H);
      hyperpoint Hl = xpush(-vid.twopoint_param) * H;
      hyperpoint Hr = xpush(+vid.twopoint_param) * H;
      ld lyx = (Hl[1] + 1e-7) / (Hl[0] + 1e-8);
      ld ryx = (Hr[1] + 1e-7) / (Hr[0] + 1e-8);
      // (r.x + t) * lyx = (r.x - t) * ryx = r.y
      // r.x * lyx + t * lyx = r.x * ryx - t * ryx
      // r.x * (lyx-ryx) = - t * (ryx + lyx)
      // r.x = -t * (ryx+lyx) / (lyx-ryx)
      // r.x = - 2 * t * lyx * ryx / lyx / ryx
      
      ret[0] = -vid.twopoint_param * (ryx + lyx) / (lyx - ryx);
      ret[1] = (ret[0] + vid.twopoint_param) * lyx;
      ret[2] = 0;

      move_y_to_z(ret, yz);      
      models::apply_orientation(ret[0], ret[1]);
      models::apply_orientation_yz(ret[2], ret[1]);
      break;
      }
    
    case mdTwoHybrid: {
      models::apply_orientation_yz(H[1], H[2]);
      models::apply_orientation(H[0], H[1]);
      auto yz = move_z_to_y(H);
      
      ret = compute_hybrid(H, whateveri[0]); 
      
      move_y_to_z(ret, yz);      
      models::apply_orientation(ret[0], ret[1]);
      models::apply_orientation_yz(ret[2], ret[1]);
      break;
      }
    
    case mdJoukowsky: 
    case mdJoukowskyInverted: {
      models::apply_orientation_yz(H[1], H[2]);
      models::apply_orientation(H[0], H[1]);
      // with equal speed skiprope: models::apply_orientation(H[1], H[0]);
  
      if(vid.skiprope) {
        static ld last_skiprope = 0;
        static transmatrix lastmatrix;
        if(vid.skiprope != last_skiprope) {
          ret = mobius(C0, -vid.skiprope, 2);
          const cld c1(1, 0);
          const cld c2(2, 0);
          const cld c4(4, 0);
          cld w(ret[0], ret[1]);
          cld z = sqrt(c4*w*w-c1) + c2*w;
          if(abs(z) > 1) z = c1 / z;
          hyperpoint zr = hpxyz(real(z), imag(z), 0);
          
          hyperpoint inhyp = perspective_to_space(zr, 1, gcHyperbolic);
          last_skiprope = vid.skiprope;
          lastmatrix = rgpushxto0(inhyp);
          }
        H = lastmatrix * H;
        }
  
      H = space_to_perspective(H);
      auto yz = move_z_to_y(H);
      ld r = hypot_d(2, H);
      ld c = H[0] / r;
      ld s = H[1] / r;
      ld& mt = models::model_transition;
      ld a = 1 - .5 * mt, b = .5 * mt;
      swap(a, b);
      
      ret[0] = (a * r + b/r) * c / 2;
      ret[1] = (a * r - b/r) * s / 2;
      ret[2] = 0;

      if(vid.skiprope) 
        ret = mobius(ret, vid.skiprope, 2);
        
      if(pmodel == mdJoukowskyInverted) {
        ld r2 = sqhypot_d(2, ret);
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
      if(models::model_transition != 1) {
        ld& mt = models::model_transition;
        
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
        ret[0] /= -(1-mt) * M_PI / 2;
        ret[1] /= (1-mt) * M_PI / 2;
        
        models::apply_orientation(ret[1], ret[0]);
        }
      else 
        makeband(H, ret, band_conformal);
      break;
      
    case mdTwoPoint: 
      makeband(H, ret, make_twopoint);
      break;
    
    case mdMollweide: 
      makeband(H, ret, [] (ld& x, ld& y) { 
        ld theta = 
          hyperbolic ? min(y / 2 + 0.572365, y * 0.78509) :
          euclid ? y :
          y > 0 ? max(y * 0.012/0.015, M_PI/2 - (M_PI/2-y) * 0.066262/0.015708) :
          min(y * 0.012/0.015, -M_PI/2 + (M_PI/2+y) * 0.066262/0.015708);
          
        if(sphere && abs(theta) >= M_PI/2 - 1e-6) ;
        else {
          for(int it=0; it<4; it++) {
            auto a = (sin_auto(2*theta) +2*theta - M_PI * sin_auto(y));
            auto b = (2 + 2 * cos_auto(2*theta));
            theta = theta - a / b;
          } }
        y = M_PI * sin_auto(theta) / 2;
        x = x * cos_auto(theta); 
        });
      break;
    
    case mdCentralCyl: 
      makeband(H, ret, [] (ld& x, ld& y) { y = tan_auto(y); ld top = vid.yres * M_PI / current_display->radius; if(y>top) y=top; if(y<-top) y=-top; });
      break;

    case mdCollignon: 
      find_zlev(H);
      makeband(H, ret, [] (ld& x, ld& y) { 
        ld sgn = 1;
        if(vid.collignon_reflected && y > 0) y = -y, sgn = -1;
        y = signed_sqrt(sin_auto(y) + vid.collignon_parameter);
        x *= y / 1.2;
        y -= signed_sqrt(vid.collignon_parameter);
        y *= sgn;
        y *= M_PI;
        });
      break;
    
    case mdBandEquiarea: 
      makeband(H, ret, [] (ld& x, ld& y) { y = sin_auto(y); });
      break;
    
    case mdBandEquidistant:
      makeband(H, ret, [] (ld& x, ld& y) { });
      break;
    
    case mdSinusoidal: 
      makeband(H, ret, [] (ld& x, ld& y) { x *= cos_auto(y); });
      break;
    
    case mdEquidistant: case mdEquiarea: case mdEquivolume: {
      if(nonisotropic || prod) {
        ret = lp_apply(inverse_exp(H, iTable, false));
        ret[3] = 1;
        break;
        }
      ld zlev = find_zlev(H);

      ld rad = hypot_d(GDIM, H);
      if(rad == 0) rad = 1;
      ld d = hdist0(H);
      ld df, zf;
      hypot_zlev(zlev, d, df, zf);
      
      // 4 pi / 2pi = M_PI 
      
      if(pmodel == mdEquivolume)
        d = pow(volume_auto(d), 1/3.) * pow(M_PI / 2, 1/3.);
      else if(pmodel == mdEquiarea && sphere)
        d = sqrt(2*(1 - cos(d))) * M_PI / 2;
      else if(pmodel == mdEquiarea && hyperbolic)
        d = sqrt(2*(cosh(d) - 1)) / 1.5;

      ret = H * (d * df / rad / M_PI);
      if(GDIM == 2) ret[2] = 0; 
      if(MAXMDIM == 4) ret[3] = 1;
      if(zlev != 1 && current_display->stereo_active()) 
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
      
      if(models::use_atan) {
        ret[0] = atan(ret[0]);
        ret[1] = atan(ret[1]);
        }

      break;
      }
    
    case mdFormula: {
      dynamicval<eModel> m(pmodel, models::basic_model);
      applymodel(H, ret);
      exp_parser ep;
      ep.extra_params["z"] = cld(ret[0], ret[1]);
      ep.extra_params["cx"] = ret[0];
      ep.extra_params["cy"] = ret[1];
      ep.extra_params["cz"] = ret[2];
      ep.extra_params["ux"] = H[0];
      ep.extra_params["uy"] = H[1];
      ep.extra_params["uz"] = H[2];
      ep.s = models::formula;
      cld res = ep.parse();
      ret[0] = real(res);
      ret[1] = imag(res);
      ret[2] = 0;
      break;
      }
    
    case mdSpiral: {
      cld z;
      if(hyperbolic || sphere) makeband(H, ret, band_conformal);
      else ret = H;
      z = cld(ret[0], ret[1]) * models::spiral_multiplier;
      
      if(models::spiral_cone < 360) {
        ld alpha = imag(z) * 360 / models::spiral_cone;
        ld r = real(z);
        r = exp(r);
        
        ret[0] = -sin(alpha) * r;
        ret[1] = cos(alpha) * r;
        ret[2] = (r-1) * sqrt( pow(360/models::spiral_cone, 2) - 1);
        
        models::apply_ball(ret[2], ret[1]);
        }
      else {      
        z = exp(z);
        ret[0] = real(z);
        ret[1] = imag(z);

        if(vid.skiprope) 
          ret = mobius(ret, vid.skiprope, 1);
        }
      }
    
    case mdGUARD: case mdManual: break;
    }

  ghcheck(ret,H_orig);
  }

// game-related graphics

EX transmatrix sphereflip; // on the sphere, flip
EX bool playerfound; // has player been found in the last drawing?

double q3 = sqrt(double(3));

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

EX transmatrix mirrorif(const transmatrix& V, bool b) {
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

  if(vid.alpha > 1) {
     if(h[LDIM] > -1/vid.alpha) return true;
     }  
  
  if(vid.alpha <= 1) {
    if(h[LDIM] < .2-vid.alpha) return true;
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
  
  if(vid.alpha > 1) {
    return to01(1/vid.alpha, 1, abs(h[2]));
    }  
  
  if(vid.alpha <= 1) {
    return to01(-1.5, 1, h[2]);
    }
      
  return 1;
  }

EX bool behindsphere(const transmatrix& V) {
  return behindsphere(tC0(V));
  }

EX ld spherity(const transmatrix& V) {
  return spherity(tC0(V));
  }

EX bool confusingGeometry() {
  return quotient;
  }

EX ld master_to_c7_angle() {
  if(hybri) return hybrid::in_underlying_geometry(master_to_c7_angle);
  ld alpha = 0;
  #if CAP_GP
  if(cgi.gpdata) alpha = cgi.gpdata->alpha;
  #endif  
  return (!BITRUNCATED && !binarytiling && !archimedean) ? M_PI + alpha : 0;
  }

EX transmatrix actualV(const heptspin& hs, const transmatrix& V) {
  if(prod) return PIU(actualV(hs, V));
  if(WDIM == 3) return V;
  #if CAP_IRR
  if(IRREGULAR)
    return V * spin(M_PI + 2 * M_PI / S7 * (hs.spin + irr::periodmap[hs.at].base.spin));
  #endif
  #if CAP_ARCM
  if(archimedean) return V * spin(-arcm::current.triangles[arcm::id_of(hs.at)][hs.spin].first);
  #endif
  #if CAP_BT
  if(binarytiling) return V;
  #endif
  if(penrose) return V;
  return (hs.spin || !BITRUNCATED) ? V * spin(hs.spin*2*M_PI/S7 + master_to_c7_angle()) : V;
  }

EX bool point_behind(hyperpoint h) {
  if(sphere) return false;
  if(!in_perspective()) return false;
  if(pmodel == mdGeodesic) h = inverse_exp(h, iLazy);
  if(pmodel == mdPerspective && prod) h = product::inverse_exp(h);
  h = lp_apply(h);
  return h[2] < 1e-8;
  }

void raise_error() {
  println(hlog, "something wrong");
  }
  
EX bool invalid_matrix(const transmatrix T) {
  for(int i=0; i<GDIM; i++) for(int j=0; j<GDIM; j++)
    if(std::isnan(T[i][j]) || T[i][j] > 1e8 || T[i][j] < -1e8 || std::isinf(T[i][j]))
      return true;
  if(prod) {
    for(int i=0; i<GDIM; i++) for(int j=0; j<GDIM; j++) if(abs(T[i][j]) > 1e-6) return false;
    }
  else 
    for(int i=0; i<GDIM; i++) for(int j=0; j<GDIM; j++) if(T[i][j] > .5 || T[i][j] < -.5) return false;
  return true;
  }
  
EX bool invalid_point(const hyperpoint h) {
  return std::isnan(h[LDIM]) || h[LDIM] > 1e8 || std::isinf(h[LDIM]);
  }

EX bool in_smart_range(const transmatrix& T) {
  hyperpoint h = tC0(T);
  if(invalid_point(h)) return false;
  if(nil || nih) return true;
  #if CAP_SOLV
  if(pmodel == mdGeodesic) return solnihv::in_table_range(h);
  #endif
  hyperpoint h1;
  applymodel(h, h1);
  if(invalid_point(h1)) return false;
  ld x = current_display->xcenter + current_display->radius * h1[0];
  ld y = current_display->ycenter + current_display->radius * h1[1] * vid.stretch;

  if(x > current_display->xtop + current_display->xsize * 2) return false;
  if(x < current_display->xtop - current_display->xsize * 1) return false;
  if(y > current_display->ytop + current_display->ysize * 2) return false;
  if(y < current_display->ytop - current_display->ysize * 1) return false;
  if(GDIM == 3) {
    if(-h1[2] < models::clip_min * 2 - models::clip_max) return false;
    if(-h1[2] > models::clip_max * 2 - models::clip_min) return false;
    }

  ld epsilon = 0.01;
  
  ld dx = 0, dy = 0, dz = 0, dh[MAXMDIM];
  for(int i=0; i<GDIM; i++) {    
    hyperpoint h2;
    applymodel(T * cpush0(i, epsilon), h2);
    ld x1 = current_display->radius * abs(h2[0] - h1[0]) / epsilon;
    ld y1 = current_display->radius * abs(h2[1] - h1[1]) * vid.stretch / epsilon;
    dx = max(dx, x1); dy = max(dy, y1);
    if(GDIM == 3) dz = max(dz, abs(h2[2] - h1[2]));
    dh[i] = hypot(x1, y1);
    }
  
  if(GDIM == 3) { 
    if(-h1[2] + 2 * dz < models::clip_min || -h1[2] - 2 * dz > models::clip_max) return false;
    sort(dh, dh+GDIM); 
    ld scale = sqrt(dh[1] * dh[2]) * cgi.scalefactor * hcrossf7;
    if(scale <= (WDIM == 2 ? vid.smart_range_detail : vid.smart_range_detail_3)) return false;
    }
  else {
    ld scale = sqrt(dh[0] * dh[1]) * cgi.scalefactor * hcrossf7;
    if(scale <= vid.smart_range_detail) return false;
    }
  
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
  
  bool drawrec(cell *c, const transmatrix& V, gp::loc at, int dir, int maindir) { 
    bool res = false;
    transmatrix V1 = V * cgi.gpdata->Tf[draw_li.last_dir][at.first&31][at.second&31][fixg6(dir)];
    if(do_draw(c, V1)) {
      /* auto li = get_local_info(c);
      if((dir - li.total_dir) % S6) printf("totaldir %d/%d\n", dir, li.total_dir);
      if(at != li.relative) printf("at %s/%s\n", disp(at), disp(li.relative));
      if(maindir != li.last_dir) printf("ld %d/%d\n", maindir, li.last_dir); */
      draw_li.relative = at;
      draw_li.total_dir = fixg6(dir);
      drawcell(c, V1);
      res = true;
      }
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->move(i);
      if(!c2) continue;
      if(c2->move(0) != c) continue;
      if(c2 == c2->master->c7) continue;
      res |= drawrec(c2, V, at + eudir(dir+i), dir + i + SG3, maindir);
      }
    return res;
    }
  
  bool drawrec(cell *c, const transmatrix& V) {
    draw_li.relative = loc(0,0);
    draw_li.total_dir = 0;
    draw_li.last_dir = -1;
    bool res = false;
    if(do_draw(c, V))
      drawcell(c, V), res = true;
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->move(i);
      if(!c2) continue;
      if(c2->move(0) != c) continue;
      if(c2 == c2->master->c7) continue;
      draw_li.last_dir = i;
      res |= drawrec(c2, V, gp::loc(1,0), SG3, i);
      }
    return res;
    }
  }
#endif

vector<tuple<heptspin, hstate, transmatrix, ld> > drawn_cells;

bool in_multi = false;

void hrmap_standard::draw() {
  if(sphere && pmodel == mdSpiral && !in_multi) {
    in_multi = true;
    if(models::ring_not_spiral) {
      int qty = ceil(1. / models::sphere_spiral_multiplier);
      if(qty > 100) qty = 100;
      for(int i=-qty; i < qty; i++) {
        band_shift = 2 * M_PI * i;
        draw();
        }
      }
    else {
      draw();
      if(vid.use_smart_range) for(int i=1;; i++) {
        int drawn = cells_drawn;
        band_shift = 2 * M_PI * i;
        draw();
        band_shift = -2 * M_PI * i;
        draw();
        if(drawn == cells_drawn) break;
        }
      }
    in_multi = false;
    return;
    }
  drawn_cells.clear();
  drawn_cells.emplace_back(centerover->master, hsOrigin, cview() * master_relative(centerover, true), band_shift);
  for(int i=0; i<isize(drawn_cells); i++) {    
    // prevent reallocation due to insertion
    if(drawn_cells.capacity() < drawn_cells.size() + 16)
      drawn_cells.reserve(max<size_t>(2 * drawn_cells.size(), 128));

    const auto& dc = drawn_cells[i];
    auto& hs = get<0>(dc);
    auto& s = get<1>(dc);
    auto& V = get<2>(dc);
    dynamicval<ld> bs(band_shift, get<3>(dc));

    cell *c = hs.at->c7;
    
    transmatrix V10;
    const transmatrix& V1 = hs.mirrored ? (V10 = V * Mirror) : V;
    
    bool draw = false;
    
    if(0) ;
    
    #if CAP_GP    
    else if(GOLDBERG) {
      draw = gp::drawrec(c, actualV(hs, V1));
      }
    #endif
    
    #if CAP_IRR    
    else if(IRREGULAR) {
      auto& hi = irr::periodmap[hs.at];
      transmatrix V0 = actualV(hs, V1);
      auto& vc = irr::cells_of_heptagon[hi.base.at];
      for(int i=0; i<isize(vc); i++) {
        cell *c = hi.subcells[i];
        transmatrix V1 = V0 * irr::cells[vc[i]].pusher;
        if(do_draw(c, V1))
          draw = true,
          drawcell(hi.subcells[i], V0 * irr::cells[vc[i]].pusher);
        }
      }
    #endif
    
    else {
      if(do_draw(c, V1)) {
        transmatrix V2 = actualV(hs, V1);
        drawcell(cellwalker(c, 0, hs.mirrored), V2);
        draw = true;
        }
  
      if(BITRUNCATED) for(int d=0; d<S7; d++) {
        int ds = hs.at->c.fix(hs.spin + d);
        // createMov(c, ds);
        if(c->move(ds) && c->c.spin(ds) == 0) {
          transmatrix V2 = V1 * cgi.hexmove[d];
          if(do_draw(c->move(ds), V2))
            draw = true,
            drawcell(cellwalker(c->move(ds), 0, hs.mirrored ^ c->c.mirror(ds)), V2);
          }
        }
      }
    
    if(sphere) draw = true;
  
    if(draw) for(int d=0; d<S7; d++) {
      hstate s2 = transition(s, d);
      if(s2 == hsError) continue;
      heptspin hs2 = hs + d + wstep;
      transmatrix Vd = V * cgi.heptmove[d];
      bandfixer bf(Vd);
      drawn_cells.emplace_back(hs2, s2, Vd, band_shift);
      }
    }
  }

int mindx=-7, mindy=-7, maxdx=7, maxdy=7;
  
EX transmatrix eumove(ld x, ld y) {
  transmatrix Mat = Id;
  Mat[LDIM][LDIM] = 1;
  
  if(a4) {
    Mat[0][LDIM] += x * cgi.crossf;
    Mat[1][LDIM] += y * cgi.crossf;
    }
  else {
    Mat[0][LDIM] += (x + y * .5) * cgi.crossf;
    // Mat[LDIM][0] += (x + y * .5) * cgi.crossf;
    Mat[1][LDIM] += y * q3 /2 * cgi.crossf;
    // Mat[LDIM][1] += y * q3 /2 * cgi.crossf;
    }
  
  ld v = a4 ? 1 : q3;

  while(Mat[0][LDIM] <= -16384 * cgi.crossf) Mat[0][LDIM] += 32768 * cgi.crossf;
  while(Mat[0][LDIM] >= 16384 * cgi.crossf) Mat[0][LDIM] -= 32768 * cgi.crossf;
  while(Mat[1][LDIM] <= -16384 * v * cgi.crossf) Mat[1][LDIM] += 32768 * v * cgi.crossf;
  while(Mat[1][LDIM] >= 16384 * v * cgi.crossf) Mat[1][LDIM] -= 32768 * v * cgi.crossf;
  return Mat;
  }

EX transmatrix eumove(int vec) {
  int x, y;
  tie(x,y) = vec_to_pair(vec);
  return eumove(x, y);
  }

EX transmatrix eumovedir(int d) {
  if(a4) {
    d = d & 3;
    switch(d) {
      case 0: return eumove(1,0);
      case 1: return eumove(0,1);
      case 2: return eumove(-1,0);
      case 3: return eumove(0,-1);
      }
    }
  else {
    d = gmod(d, S6);
    switch(d) {
      case 0: return eumove(1,0);
      case 1: return eumove(0,1);
      case 2: return eumove(-1,1);
      case 3: return eumove(-1,0);
      case 4: return eumove(0,-1);
      case 5: return eumove(1,-1);
      }
    }
  return eumove(0,0);
  }

EX void spinEdge(ld aspd) { 
  ld downspin = 0;
  if(dual::state == 2 && dual::currently_loaded != dual::main_side) {
    transmatrix our   = dual::get_orientation();
    transmatrix their = dual::player_orientation[dual::main_side];
    fixmatrix(our);
    fixmatrix(their);
    if(GDIM == 2) {
      transmatrix T = their * inverse(our);
      hyperpoint H = T * xpush0(1);
      downspin = -atan2(H[1], H[0]);
      }
    else rotate_view(their * inverse(our));
    }
  else if(playerfound && vid.fixed_facing) {
    hyperpoint H = gpushxto0(playerV * C0) * playerV * xpush0(5);
    downspin = atan2(H[1], H[0]);
    downspin += vid.fixed_facing_dir * degree;
    if(flipplayer) downspin += M_PI;
    while(downspin < -M_PI) downspin += 2*M_PI;
    while(downspin > +M_PI) downspin -= 2*M_PI;
    aspd = (1 + 2 * abs(downspin)) * aspd;
    }
  else if((WDIM == 2 || prod) && GDIM == 3 && vid.fixed_yz && !CAP_ORIENTATION) {
    aspd = 999999;
    auto& vo = get_view_orientation();
    // does not work well (also need change auto& to auto)
    // if(hybri && !prod) vo = vo * inverse(nisot::translate(tC0(vo)));
    if(straightDownSeek) {
      auto sdp = straightDownPoint;
      if(prod) sdp = vo * product::inverse_exp(sdp);
      if(sdp[0])
        downspin = models::rotation * degree - atan2(sdp[0], sdp[1]);
      }
    else {
      if(vo[0][2]) 
        downspin = -atan2(vo[0][2], vo[1][2]);
      }
    }
  else if(straightDownSeek) {
    downspin = atan2(straightDownPoint[1], straightDownPoint[0]);
    downspin -= M_PI/2;
    downspin += models::rotation * degree;
    while(downspin < -M_PI) downspin += 2*M_PI;
    while(downspin > +M_PI) downspin -= 2*M_PI;
    downspin = downspin * min(straightDownSpeed, (double)1);
    }
  if(downspin >  aspd) downspin =  aspd;
  if(downspin < -aspd) downspin = -aspd;
  rotate_view(spin(downspin));
  }

EX void centerpc(ld aspd) { 
  
  if(subscreens::split([=] () {centerpc(aspd);})) return;
  if(dual::split([=] () { centerpc(aspd); })) return;

  #if CAP_CRYSTAL
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
    int sl = snakelevel(cwt.at);
    if(sl && WDIM == 2) T = T * zpush(cgi.SLEV[sl] - cgi.FLOOR);
    View = inverse(T);
    if(prod) NLP = inverse(pc->ori);
    if(WDIM == 2) rotate_view( cspin(0, 1, M_PI) * cspin(2, 1, M_PI/2 + shmup::playerturny[id]) * spin(-M_PI/2) );
    return;
    }
  #endif
  
  if(ors::mode == 2 && vid.sspeed < 5) return;
  if(vid.sspeed >= 4.99) aspd = 1000;
  DEBBI(DF_GRAPH, ("center pc"));

  auto& W = current_display->which_copy;                                                            
  ors::unrotate(W); ors::unrotate(View); ors::unrotate(cwtV);

  /* what should we center? */
  transmatrix T;
  if(multi::players > 1) 
    T = cwtV; /* do not even try */
  else {
    T = W;
    if(shmup::on)
      T = T * shmup::pc[0]->at;
    }
  
  if(invalid_matrix(T)) return;
  
  #if MAXMDIM >= 4
  if(GDIM == 3 && WDIM == 2) {
    geom3::do_auto_eye();
    int sl = snakelevel(cwt.at);
    if(isWorm(cwt.at->monst) && sl < 3) sl++;
    if(sl || vid.eye) T = T * zpush(cgi.SLEV[sl] - cgi.FLOOR + vid.eye);
    }
  #endif
  hyperpoint H = inverse(actual_view_transform) * tC0(T);
  ld R = (zero_d(GDIM, H) && !prod) ? 0 : hdist0(H);
  if(R < 1e-9) {
    // either already centered or direction unknown
    /* if(playerfoundL && playerfoundR) {
      
      } */
    spinEdge(aspd);
    fixmatrix(View);
    current_display->which_copy = gmatrix[cwt.at];
    }
  
  else {
    aspd *= euclid ? (2+3*R*R) : (1+R+(shmup::on?1:0));
    
    if(R < aspd) 
      shift_view_to(H);
    else 
      shift_view_towards(H, aspd);
      
    fixmatrix(View);
    fixmatrix(current_display->which_copy);
    spinEdge(aspd);
    }

  ors::rerotate(W); ors::rerotate(cwtV); ors::rerotate(View);
  }

EX void optimizeview() {

  if(subscreens::split(optimizeview)) return;
  if(dual::split(optimizeview)) return;
  
  transmatrix iView = inverse(View);
  virtualRebase(centerover, iView);
  View = inverse(iView);
  fixmatrix(View);

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
    addball(i * M_PI/30, 10, 0);
  for(double d=10; d>=-10; d-=.2)
    addball(0, d, 0);
  for(double d=-10; d<=10; d+=.2)
    addball(0, d, vid.depth);
  addball(0, 0, -vid.camera);
  addball(0, 0, vid.depth);
  addball(0, 0, -vid.camera);
  addball(0, -10, 0);
  addball(0, 0, -vid.camera);
  queuecurve(darkena(0xFF, 0, 0x80), 0, PPR::CIRCLE);
  queuereset(pmodel, PPR::CIRCLE);
  }

EX void resetview() {
  DEBBI(DF_GRAPH, ("reset view"));
  // EUCLIDEAN
  NLP = Id;
  if(cwt.at) {
    centerover = cwt.at;
    View = iddspin(cwt.at, cwt.spin);
    if(!flipplayer) View = pispin * View;
    if(cwt.mirrored) View = Mirror * View;
    if(GDIM == 2) View = spin(M_PI + vid.fixed_facing_dir * degree) * View;
    if(GDIM == 3 && !prod) View = cspin(0, 2, M_PI/2) * View;
    if(prod) NLP = cspin(0, 2, M_PI/2);
    }
  else if(currentmap) {
    centerover = currentmap->gamestart();
    View = Id;
    }
  cwtV = View;
  current_display->which_copy = 
    nonisotropic ? gpushxto0(tC0(inverse(View))) :
    View;
  // SDL_LockSurface(s);
  // SDL_UnlockSurface(s);
  }


EX void panning(hyperpoint hf, hyperpoint ht) {
  View = 
    rgpushxto0(hf) * rgpushxto0(gpushxto0(hf) * ht) * gpushxto0(hf) * View;
  playermoved = false;
  }

EX int cells_drawn, cells_generated;

EX void fullcenter() {
  history::path_for_lineanimation.clear();
  if(playerfound && false) centerpc(INF);
  else {
    bfs();
    resetview();
    drawthemap();
    centerpc(INF);
    centerover = cwt.at;
    }
  playermoved = true;
  }

transmatrix screenpos(ld x, ld y) {
  transmatrix V = Id;
  V[0][2] += (x - current_display->xcenter) / current_display->radius * (1+vid.alpha);
  V[1][2] += (y - current_display->ycenter) / current_display->radius * (1+vid.alpha);
  return V;
  }

/** 
  In 3D, we use the standard translation matrices to place stuff on the screen.
  In 2D, this does not work (as HyperRogue reduces matrices to 3x3) so we use the native disk projection
*/

EX eModel flat_model() { return MDIM == 4 ? mdPixel : mdDisk; }

EX transmatrix atscreenpos(ld x, ld y, ld size) {
  transmatrix V = Id;
  
  if(pmodel == mdPixel) {
    V[0][3] += (x - current_display->xcenter);
    V[1][3] += (y - current_display->ycenter);
    V[0][0] = size * 2 * cgi.hcrossf / cgi.crossf;
    V[1][1] = size * 2 * cgi.hcrossf / cgi.crossf;
    if(WDIM == 3) V[2][2] = -1;
    }
  else { 
    V[0][2] += (x - current_display->xcenter);
    V[1][2] += (y - current_display->ycenter);
    V[0][0] = size * 2 * cgi.hcrossf / cgi.crossf;
    V[1][1] = size * 2 * cgi.hcrossf / cgi.crossf;
    V[2][2] = current_display->radius;
    if(S3 == OINF) V[0][0] /= 5, V[1][1] /= 5;
    }

  return V;
  }

void circle_around_center(ld radius, color_t linecol, color_t fillcol, PPR prio) {
  #if CAP_QUEUE
  if(among(pmodel, mdDisk, mdEquiarea, mdEquidistant, mdFisheye) && !(pmodel == mdDisk && hyperbolic && vid.alpha <= -1) && vid.camera_angle == 0) {
    hyperpoint ret;
    applymodel(xpush0(radius), ret);
    ld r = hypot_d(2, ret);
    queuecircle(current_display->xcenter, current_display->ycenter, r * current_display->radius, linecol, prio, fillcol);
    return;
    }  
  #endif
  #if CAP_QUEUE
  for(int i=0; i<=360; i++) curvepoint(xspinpush0(i * degree, 10));
  auto& c = queuecurve(linecol, fillcol, prio);
  if(pmodel == mdDisk && hyperbolic && vid.alpha <= -1)
    c.flags |= POLY_FORCE_INVERTED;
  if(pmodel == mdJoukowsky)
    c.flags |= POLY_FORCE_INVERTED;
  c.flags |= POLY_ALWAYS_IN;
  #endif
  }

EX color_t periodcolor = 0x00FF0080;
EX color_t ringcolor = darkena(0xFF, 0, 0xFF);
EX color_t modelcolor = 0;

#if CAP_QUEUE
EX void draw_model_elements() {

  dynamicval<ld> lw(vid.linewidth, vid.linewidth * vid.multiplier_ring);
  switch(pmodel) {
  
    case mdRotatedHyperboles: {
      queuechr(current_display->xcenter, current_display->ycenter + current_display->radius * vid.alpha, 0, vid.fsize, 'X', ringcolor, 1, 8);
      return;
      }
    
    case mdTwoHybrid: {
      queuereset(mdPixel, PPR::CIRCLE);
      
      for(int mode=0; mode<4; mode++) {
        for(int s=-200; s<=200; s ++) {
          ld p = tanh(s / 40.);
          ld a = vid.twopoint_param * (1+p);
          ld b = vid.twopoint_param * (1-p);
          ld h = ((mode & 2) ? -1 : 1) * sqrt(asin_auto(tan_auto(a) * tan_auto(b)));
          
          hyperpoint H = xpush(p * vid.twopoint_param) * ypush0(h);
          
          hyperpoint res = compute_hybrid(H, 2 | mode);
          models::apply_orientation(res[0], res[1]);
          models::apply_orientation_yz(res[2], res[1]);
          curvepoint(res * current_display->radius);
          }
        queuecurve(ringcolor, 0, PPR::CIRCLE);
        }

      queuereset(pmodel, PPR::CIRCLE);
      /* fallthrough */
      }

    case mdTwoPoint: case mdSimulatedPerspective: {
      ld a = -models::model_orientation * degree;
      queuechr(xspinpush0(a, +vid.twopoint_param), vid.xres / 100, 'X', ringcolor >> 8);
      queuechr(xspinpush0(a, -vid.twopoint_param), vid.xres / 100, 'X', ringcolor >> 8);
      return;
      }
    
    case mdBall: {
      queuecircle(current_display->xcenter, current_display->ycenter, current_display->radius, ringcolor, PPR::OUTCIRCLE, modelcolor);
      ballgeometry();
      return;
      }
    
    case mdHyperboloid: {
      if(hyperbolic) {
#if CAP_QUEUE
        curvepoint(point3(0,0,1));
        curvepoint(point3(0,0,-vid.alpha));
        queuecurve(ringcolor, 0, PPR::CIRCLE);
        
        ld& tz = models::top_z;
        ld z = acosh(tz);
    
        hyperpoint a = xpush0(z);
        ld cb = models::cos_ball;
        ld sb = models::sin_ball;
        
        a[1] = sb * a[2] / -cb;
        a[0] = sqrt(-1 + a[2] * a[2] - a[1] * a[1]);
    
        curvepoint(point3(0,0,-vid.alpha));
        curvepoint(a);
        curvepoint(point3(0,0,0));
        a[0] = -a[0];
        curvepoint(a);
        curvepoint(point3(0,0,-vid.alpha));
        queuecurve(ringcolor, 0, PPR::CIRCLE);
    
        curvepoint(point3(-1,0,0));
        curvepoint(point3(1,0,0));
        queuecurve(ringcolor, 0, PPR::CIRCLE);
    
        a[1] = sb * tz / -cb;
        a[0] = sqrt(tz * tz - a[1] * a[1]);
        a[2] = tz - vid.alpha;
    
        curvepoint(a);
        curvepoint(point3(0,0,-vid.alpha));
        a[0] = -a[0];
        curvepoint(a);
        queuecurve(ringcolor, 0, PPR::CIRCLE);
#endif
        }
      return;
      }

    default: break;
    }
  }
 
void queuestraight(hyperpoint X, int style, color_t lc, color_t fc, PPR p) {

  hyperpoint H0, H1;
  applymodel(X, H0);
  H0 *= current_display->radius;
  ld mul0 = hypot(vid.xres, vid.yres) / hypot_d(2, H0);
  
  if(style == 1) {
    H1 = H0 * -mul0;
    }
  else {
    applymodel(pispin * X, H1);
    H1 *= current_display->radius;
    }

  ld mul1 = hypot(vid.xres, vid.yres) / hypot_d(2, H1);
  
  queuereset(mdPixel, p);
  curvepoint(H0 + spin(M_PI/2) * H0 * mul0);
  curvepoint(H0 - spin(M_PI/2) * H0 * mul0);
  curvepoint(H1 + spin(M_PI/2) * H1 * mul1);
  curvepoint(H1 - spin(M_PI/2) * H1 * mul1);
  curvepoint(H0 + spin(M_PI/2) * H0 * mul0);
  
  queuecurve(lc, fc, p).flags |= POLY_ALWAYS_IN | POLY_FORCEWIDE;
  queuereset(pmodel, p);
  /*
  for(int i=0; i<1; i++) {
    hyperpoint h = spin(i * 45 * degree) * X;
    hyperpoint res;
    applymodel(h, res);
    if(hypot2(res) < 1000 && !std::isnan(res[0]) && !std::isnan(res[1]))
      queuechr(h, 16, 'X', 0xFF0000 + i * 0x20);
    } */
  }

EX void draw_boundary(int w) {

  if(w == 1) return;
  if(nonisotropic || euclid || prod) return;

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
    circle_around_center(M_PI/2, periodcolor, 0, PPR::CIRCLE);
  
  switch(pmodel) {
  
    case mdTwoPoint: {
      if(twopoint_do_flips || current_display->stereo_active() || !sphere) return;
      queuereset(mdPixel, p);
  
      for(int b=-1; b<=1; b+=2)
      for(ld a=-90; a<=90+1e-6; a+=pow(.5, vid.linequality)) {
        ld x = sin(a * vid.twopoint_param * b / 90);
        ld y = 0;
        ld z = -sqrt(1 - x*x);
        models::apply_orientation(y, x);
        hyperpoint h1;
        applymodel(hpxyz(x,y,z), h1);
        
        models::apply_orientation(h1[0], h1[1]);      
        h1[1] = abs(h1[1]) * b;
        models::apply_orientation(h1[1], h1[0]);
        curvepoint(h1);
        }
  
      queuecurve(lc, fc, p).flags |= POLY_FORCEWIDE;
      queuereset(pmodel, p);
      return;
      }
    
    case mdBand: case mdBandEquidistant: case mdBandEquiarea: case mdSinusoidal: case mdMollweide: case mdCentralCyl: case mdCollignon: {
      if(GDIM == 3) return;
      if(pmodel == mdBand && models::model_transition != 1) return;
      bool bndband = (among(pmodel, mdBand, mdCentralCyl) ? hyperbolic : sphere);
      transmatrix T = spin(-models::model_orientation * degree);
      ld right = M_PI/2 - 1e-5;
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
        queuecurve(periodcolor, 0, PPR::CIRCLE).flags |= POLY_FORCEWIDE;
        }
      return;
      }
    
    case mdHalfplane: 
      if(hyperbolic && GDIM == 2) {
        queuestraight(xspinpush0(-models::model_orientation * degree - M_PI/2, fakeinf), 1, lc, fc, p);
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
        queuecurve(lc, fc, p);
        queuereset(pmodel, p);
        p = PPR::CIRCLE; fc = 0;
        queuereset(mdPixel, p);
  
        for(int i=0; i<=360; i++) {
          ld s = sin(i * degree);
          curvepoint(point3(current_display->radius * cos(i * degree), current_display->radius * s * models::sin_ball, 0));
          }
        queuecurve(lc, fc, p);
        queuereset(pmodel, p);
        }
      if(euclid || sphere) {
        queuereset(mdPixel, p);  
        for(int i=0; i<=360; i++) {
          curvepoint(point3(current_display->radius * cos(i * degree), current_display->radius * sin(i * degree), 0));
          }
        queuecurve(lc, fc, p);
        queuereset(pmodel, p);
        }
      return;
      }
    
    case mdHyperboloid: {
      if(hyperbolic) {
        ld& tz = models::top_z;
        ld mz = acosh(tz);
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
              curvepoint(xspinpush0(-M_PI/2 - t * alpha, mz));
            }
          else {
            ld alpha = - atan2(a[0], -a[1]);
            
            for(ld t=-1; t<=1; t += step)
              curvepoint(xspinpush0(+M_PI/2 - t * alpha, mz));
            }
          
          queuecurve(lc, fc, p);
          fc = 0; p = PPR::CIRCLE;
          }

        for(ld t=0; t<=360; t ++)
          curvepoint(xspinpush0(t * degree, mz));

        queuecurve(lc, fc, p);
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
          ret = mobius(ret, vid.skiprope, 1);
          ret *= current_display->radius;
          curvepoint(ret);
          }
        queuecurve(ringcolor, 0, p).flags |= POLY_ALWAYS_IN;
        queuereset(pmodel, p);
        }
      return;
      }

    default: break;
    }

  if(sphere && pmodel == mdDisk && vid.alpha > 1) {
    double rad = current_display->radius / sqrt(vid.alpha*vid.alpha - 1);
    queuecircle(current_display->xcenter, current_display->ycenter, rad, lc, p, fc);
    return;
    }

  if(sphere && !among(pmodel, mdEquidistant, mdEquiarea)) return;
  circle_around_center(fakeinf, lc, fc, p);
  }
#endif

EX ld band_shift = 0;
EX void fix_the_band(transmatrix& T) {
  if(((mdinf[pmodel].flags & mf::uses_bandshift) && T[LDIM][LDIM] > 1e6) || (sphere && pmodel == mdSpiral)) {
    hyperpoint H = tC0(T);
    find_zlev(H);
    models::apply_orientation(H[0], H[1]);
    
    ld y = asin_auto(H[1]);
    ld x = asin_auto_clamp(H[0] / cos_auto(y));
    if(sphere) {
      if(H[LDIM] < 0 && x > 0) x = M_PI - x;
      else if(H[LDIM] < 0 && x <= 0) x = -M_PI - x;
      }
    band_shift += x;
    T = xpush(-x) * T;
    fixmatrix(T);
    // todo orientation
    }
  }

#if HDR
struct bandfixer {
  dynamicval<ld> bw;
  bandfixer(transmatrix& T) : bw(band_shift, band_shift) { fix_the_band(T); }
  };
#endif

EX namespace dq {
  EX queue<tuple<heptagon*, transmatrix, ld>> drawqueue;

  EX set<heptagon*> visited;
  EX void enqueue(heptagon *h, const transmatrix& T) {
    if(!h || visited.count(h)) { return; }
    visited.insert(h);
    drawqueue.emplace(h, T, band_shift);
    }  

  EX set<int> visited_by_matrix;
  EX void enqueue_by_matrix(heptagon *h, const transmatrix& T) {
    if(!h) return;
    int b = bucketer(tC0(T));
    if(visited_by_matrix.count(b)) { return; }
    visited_by_matrix.insert(b);
    drawqueue.emplace(h, T, band_shift);
    }
  EX }

EX bool do_draw(cell *c) {
  // do not display out of range cells, unless on torus
  if(c->pathdist == PINFD && geometry != gTorus && vid.use_smart_range == 0)
    return false;
  // do not display not fully generated cells, unless changing range allowed
  if(c->mpdist > 7 && !allowChangeRange()) return false;
  // in the Yendor Challenge, scrolling back is forbidden
  if(c->cpdist > 7 && (yendor::on || isHaunted(cwt.at->land)) && !cheater && !autocheat) return false;

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

EX bool do_draw(cell *c, const transmatrix& T) {

  if(hybrid::pmap) return hybrid::do_draw(c, T);
  if(WDIM == 3) {
    // do not care about cells outside of the track
    if(GDIM == 3 && racing::on && c->land == laMemory && cells_drawn >= S7+1) return false;

    if(cells_drawn > vid.cells_drawn_limit) return false;
    if(cells_drawn < 50) { limited_generation(c); return true; }
    if(nil && pmodel == mdGeodesic) {
      ld dist = hypot_d(3, inverse_exp(tC0(T), iLazy));
      if(dist > sightranges[geometry] + (vid.sloppy_3d ? 0 : 0.9)) return false;
      if(dist <= extra_generation_distance && !limited_generation(c)) return false;
      }
    else if(pmodel == mdGeodesic && sol) {
      if(!nisot::in_table_range(tC0(T))) return false;
      if(!limited_generation(c)) return false;
      }
    else if(pmodel == mdGeodesic && nih) {
      hyperpoint h = inverse_exp(tC0(T), iLazy, false);
      ld dist = hypot_d(3, h);
      if(dist > sightranges[geometry] + (vid.sloppy_3d ? 0 : cgi.corner_bonus)) return false;
      if(dist <= extra_generation_distance && !limited_generation(c)) return false;
      }
    else if(pmodel == mdGeodesic && sl2) {
      if(hypot(tC0(T)[2], tC0(T)[3]) > cosh(slr::range_xy)) return false;
      if(!limited_generation(c)) return false;
      }
    else if(vid.use_smart_range) {
      if(cells_drawn >= 50 && !in_smart_range(T)) return false;
      if(!limited_generation(c)) return false;
      }
    else {
      ld dist = hdist0(tC0(T));
      if(dist > sightranges[geometry] + (vid.sloppy_3d ? 0 : cgi.corner_bonus)) return false;
      if(dist <= extra_generation_distance && !limited_generation(c)) return false;
      }
    return true;
    }

  if(just_gmatrix && sphere) return true;
  if(!do_draw(c)) return false;
  if(euclid && pmodel == mdSpiral) {
    hyperpoint h = tC0(T);
    cld z(h[0], h[1]);
    z = z * models::spiral_multiplier;
    ld iz = imag(z) + 1.14279e-2; // make it never fall exactly on PI
    if(iz < -M_PI || iz >= M_PI) return false;
    }
  if(pmodel == mdSpiral && models::ring_not_spiral) {
    cld z;
    hyperpoint H = tC0(T);
    hyperpoint ret;
    makeband(H, ret, band_conformal);
    z = cld(ret[0], ret[1]) * models::spiral_multiplier;
    if(imag(z) < -models::spiral_cone_rad/2-1e-5 || imag(z) >= models::spiral_cone_rad/2-1e-5) return false;
    }
  if(cells_drawn > vid.cells_drawn_limit) return false;
  bool usr = vid.use_smart_range || quotient || euwrap;
  if(usr && cells_drawn >= 50 && !in_smart_range(T) && !(WDIM == 2 && GDIM == 3 && hdist0(tC0(T)) < 2.5)) return false;
  if(vid.use_smart_range == 2 && !limited_generation(c)) return false;
  return true; 
  }

EX int cone_side(const hyperpoint H) {
  hyperpoint ret;
  if(hyperbolic) makeband(H, ret, band_conformal);
  else ret = H;
  cld z = cld(ret[0], ret[1]) * models::spiral_multiplier;
  
  auto zth = [&] (cld z) {
    ld alpha = imag(z) * 360 / models::spiral_cone;
    ld r = real(z);
    r = exp(r);
    
    hyperpoint ret;
      
    ret[0] = -sin(alpha) * r;
    ret[1] = cos(alpha) * r;
    ret[2] = (r-1) * sqrt( pow(360/models::spiral_cone, 2) - 1);
    
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
  return prod ? NLP : View;
  }

/** rotate the view using the given rotation matrix */
EX void rotate_view(transmatrix T) {
  transmatrix& which = get_view_orientation();
  which = T * which;
  if(!prod && !nonisotropic) current_display->which_copy = T * current_display->which_copy;
  }

/** shift the view according to the given tangent vector */
EX transmatrix get_shift_view_of(const hyperpoint H, const transmatrix V) {
  if(!nonisotropic) {
    return rgpushxto0(direct_exp(lp_iapply(H), 100)) * V;
    }
  else if(!nisot::geodesic_movement) {
    transmatrix IV = inverse(V);
    nisot::fixmatrix(IV);
    const transmatrix V1 = inverse(IV);
    return V1 * eupush(IV * eupush(H) * V1 * C0);
    }
  else {
    return inverse(nisot::parallel_transport(inverse(V), -H));
    }
  }

/** shift the view according to the given tangent vector */
EX void shift_view(hyperpoint H) {
  auto oView = View;
  View = get_shift_view_of(H, View);
  auto& wc = current_display->which_copy;
  if(nonisotropic) {
    transmatrix ioldv = eupush(tC0(inverse(oView)));
    transmatrix newv = inverse(eupush(tC0(inverse(View))));
    wc = newv * ioldv * wc;
    }
  else
    wc = get_shift_view_of(H, wc);
  }

void multiply_view(transmatrix T) {
  View = T * View;
  auto& wc = current_display->which_copy;
  wc = T * wc;
  }

EX void shift_view_to(hyperpoint H) {
  if(!nonisotropic) multiply_view(gpushxto0(H));
  else shift_view(-inverse_exp(H, iTable, false));
  }

EX void shift_view_towards(hyperpoint H, ld l) {
  if(!nonisotropic && !prod) 
    multiply_view(rspintox(H) * xpush(-l) * spintox(H));
  else if(nonisotropic && !nisot::geodesic_movement)
    shift_view(tangent_length(H-C0, -l));
  else {
    hyperpoint ie = inverse_exp(H, iTable, true);
    if(prod) ie = lp_apply(ie);
    shift_view(tangent_length(ie, -l));
    }
  }

}
