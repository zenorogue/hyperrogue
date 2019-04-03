// Hyperbolic Rogue -- hyperbolic graphics
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

ld ghx, ghy, ghgx, ghgy;
hyperpoint ghpm = C0;

void ghcheck(hyperpoint &ret, const hyperpoint &H) {
  if(hypot(ret[0]-ghx, ret[1]-ghy) < hypot(ghgx-ghx, ghgy-ghy)) {
    ghpm = H; ghgx = ret[0]; ghgy = ret[1];
    }
  }

void camrotate(ld& hx, ld& hy) {
  ld cam = vid.camera_angle * degree;
  GLfloat cc = cos(cam);
  GLfloat ss = sin(cam);
  ld ux = hx, uy = hy * cc + ss, uz = cc - ss * hy;
  hx = ux / uz, hy = uy / uz;
  }

hyperpoint perspective_to_space(hyperpoint h, ld alpha = vid.alpha, eGeometryClass geo = ginf[geometry].cclass);

bool non_spatial_model() {
  if(among(pmodel, mdRotatedHyperboles, mdJoukowsky, mdJoukowskyInverted, mdPolygonal, mdPolynomial))
    return true;
  if(pmodel == mdSpiral && euclid)
    return true;
  return vid.consider_shader_projection && shaderside_projection && pmodel;  
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
  if(gc == gcSphere && vid.alpha > 1) rootsign = -1;
  
  ld hz = B / 2 + rootsign * sqrt(C + B*B/4);
  
  hyperpoint H;
  H[0] = hx * (hz+vid.alpha);
  H[1] = hy * (hz+vid.alpha);
  H[DIM] = hz;
  
  return H;  
  }

hyperpoint space_to_perspective(hyperpoint z, ld alpha = vid.alpha);

hyperpoint space_to_perspective(hyperpoint z, ld alpha) {
  ld s = 1 / (alpha + z[DIM]);
  z[0] *= s;
  z[1] *= s;
  if(DIM == 3) {
    z[2] *= s;
    z[3] = 0;
    }
  else
    z[2] = 0;
  return z;
  }

hyperpoint gethyper(ld x, ld y) {

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
  hyperpoint H = ypush(geom3::camera) * xpush(d) * ypush(zl) * C0;
  ld tzh = vid.ballproj + H[DIM];
  ld ax = H[0] / tzh;
  ld ay = H[1] / tzh;
  
  ld ca = cos(alpha), sa = sin(alpha);

  ret[0] = ax * ca;
  ret[1] = ay;
  ret[2] = ax * sa;
  
  conformal::apply_ball(ret[2], ret[1]);
  }

void apply_depth(hyperpoint &f, ld z) {
  if(vid.usingGL)
    f[2] = z;
  else {
    z = z * current_display->radius;
    ld mul = current_display->scrdist / (current_display->scrdist + z);
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
    using namespace hyperpoint_vec;
    if(zlev > 1-1e-6 && zlev < 1+1e-6) return 1;
    H /= zlev;
    return zlev;
    }  
  
  return 1;
  }

ld get_tz(hyperpoint H) {
  ld tz = euclid ? (1+vid.alpha) : vid.alpha+H[DIM];
  if(tz < BEHIND_LIMIT && tz > -BEHIND_LIMIT) tz = BEHIND_LIMIT;
  return tz;
  }

ld atan2(hyperpoint h) {
  return atan2(h[1], h[0]);
  }

pair<ld, ld> move_z_to_y(hyperpoint& H) {
  if(DIM == 2) return make_pair(0, 0);
  ld R = hypot(H[1], H[2]);
  pair<ld, ld> res = { H[1] / R, H[2] / R };
  H[1] = R; H[2] = 0;
  return res;
  }

void move_y_to_z(hyperpoint& H, pair<ld, ld> coef) {
  if(DIM == 3) {
    H[2] = H[1] * coef.second;
    H[1] = H[1] * coef.first;
    H[3] = 1;
    }
  }

template<class T> void makeband(hyperpoint H, hyperpoint& ret, const T& f) {
  ld zlev = find_zlev(H);
  conformal::apply_orientation_yz(H[1], H[2]);
  conformal::apply_orientation(H[0], H[1]);
  auto r = move_z_to_y(H);
  
  ld x, y, yf, zf=0;
  y = asin_auto(H[1]);
  x = asin_auto_clamp(H[0] / cos_auto(y)) + band_shift;
  if(sphere) {
    if(H[DIM] < 0 && x > 0) x = M_PI - x;
    else if(H[DIM] < 0 && x <= 0) x = -M_PI - x;
    }
  hypot_zlev(zlev, y, yf, zf);
  
  f(x, y);
  
  ld yzf = y * zf; y *= yf;
  ret = hpxyz(x / M_PI, y / M_PI, 0);
  move_y_to_z(ret, r);
  conformal::apply_orientation(ret[1], ret[0]);
  conformal::apply_orientation_yz(ret[2], ret[1]);
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
  using namespace hyperpoint_vec;
  h = perspective_to_space(h * scale, 1, gcSphere);
  h = rotmatrix(angle * degree, 1, 2) * h;
  return space_to_perspective(h, 1) / scale;
  }

void applymodel(hyperpoint H, hyperpoint& ret) {

  using namespace hyperpoint_vec;
  
  hyperpoint H_orig = H;
  
  switch(pmodel) {
    case mdPerspective: {
      ld ratio = vid.xres / current_display->tanfov / current_display->radius / 2;
      ret[0] = H[0]/H[2] * ratio;
      ret[1] = H[1]/H[2] * ratio;
      ret[2] = 1;
      return;
      }
      
    case mdUnchanged:
      ret = H / current_display->radius;
      return; 
    
    case mdBall: {
      ld zlev = find_zlev(H);
      
      ld zl = geom3::depth-geom3::factor_to_lev(zlev);
  
      ballmodel(ret, atan2(H), hdist0(H), zl);
      break;      
      }
    
    case mdDisk: {
      ld tz = get_tz(H);
      if(!vid.camera_angle) {
        ret[0] = H[0] / tz;
        ret[1] = H[1] / tz;
        if(DIM == 3) ret[2] = H[2] / tz;
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
    
    case mdHalfplane: {
      // Poincare to half-plane
      
      ld zlev = find_zlev(H);
      H = space_to_perspective(H);
      
      conformal::apply_orientation_yz(H[1], H[2]);
      conformal::apply_orientation(H[0], H[1]);
  
      H[1] += 1;
      double rad = sqhypot_d(DIM, H);
      H /= -rad;
      H[1] += .5;
      
      if(DIM == 3) {
        // a bit simpler when we do not care about 3D
        H *= conformal::halfplane_scale;
        ret[0] = -H[0];
        ret[1] = 1 + H[1];
        ret[2] = H[2];
        ret[3] = 1;
        conformal::apply_orientation(ret[1], ret[0]);
        conformal::apply_orientation_yz(ret[2], ret[1]);
        break;
        }
      
      conformal::apply_orientation(H[0], H[1]);
      
      H *= conformal::halfplane_scale;
      
      ret[0] = -conformal::osin - H[0];
      if(zlev != 1) {
        if(abs(conformal::ocos) > 1e-5)
          H[1] = H[1] * pow(zlev, conformal::ocos);
        if(abs(conformal::ocos) > 1e-5 && conformal::osin)
          H[1] += H[0] * conformal::osin * (pow(zlev, conformal::ocos) - 1) / conformal::ocos;
        else if(conformal::osin)
          H[1] += H[0] * conformal::osin * log(zlev);
        }
      ret[1] = conformal::ocos + H[1];
      ret[2] = DIM == 3 ? H[2] : 0;
      if(MAXMDIM == 4) ret[3] = 1;
      if(zlev != 1 && current_display->stereo_active()) 
        apply_depth(ret, -H[1] * geom3::factor_to_lev(zlev));
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
          
        case gcEuclid: {
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
      
      conformal::apply_ball(ret[2], ret[1]);
      
      break;
      }
    
    case mdHyperboloidFlat: 
    case mdHyperboloid: {
    
      if(pmodel == mdHyperboloid) {
        ld& topz = conformal::top_z;
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
      
      conformal::apply_ball(ret[2], ret[1]);
      break;
      }
    
    case mdFisheye: {
      ld zlev = find_zlev(H);
      H = space_to_perspective(H);
      H[DIM] = zlev;
      ret = H / sqrt(1 + sqhypot_d(DIM+1, H));
      if(DIM == 3) ret[DIM] = zlev;
      break;
      }
    
    case mdJoukowsky: 
    case mdJoukowskyInverted: {
      conformal::apply_orientation_yz(H[1], H[2]);
      conformal::apply_orientation(H[0], H[1]);
      // with equal speed skiprope: conformal::apply_orientation(H[1], H[0]);
  
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
      ld& mt = conformal::model_transition;
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
        conformal::apply_orientation(ret[1], ret[0]);
        
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
        conformal::apply_orientation(ret[0], ret[1]);
        }
      conformal::apply_orientation_yz(ret[2], ret[1]);
  
      break;
      }
    
    case mdPolygonal: case mdPolynomial: {
    
      H = space_to_perspective(H);

      conformal::apply_orientation(H[0], H[1]);
  
      pair<long double, long double> p = polygonal::compute(H[0], H[1]);
  
      conformal::apply_orientation(p.second, p.first);
      ret[0] = p.first;
      ret[1] = p.second;
      ret[2] = 0;
      break;
      }  
      
    case mdBand: 
      if(conformal::model_transition != 1) {
        ld& mt = conformal::model_transition;
        
        H = space_to_perspective(H);
        
        conformal::apply_orientation(H[0], H[1]);
    
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
        
        conformal::apply_orientation(ret[1], ret[0]);
        }
      else 
        makeband(H, ret, band_conformal);
      break;
      
    case mdTwoPoint: 
      makeband(H, ret, make_twopoint);
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
      ld zlev = find_zlev(H);

      ld rad = hypot_d(DIM, H);
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
      if(DIM == 2) ret[2] = 0; 
      if(MAXMDIM == 4) ret[3] = 1;
      if(zlev != 1 && current_display->stereo_active()) 
        apply_depth(ret, d * zf / M_PI);
      
      break;
      }
    
    case mdRotatedHyperboles: {
      // ld zlev =  <- not implemented
      find_zlev(H); // + geom3::depth;
      conformal::apply_orientation(H[0], H[1]);
      
      ld y = asin_auto(H[1]);
      ld x = asin_auto_clamp(H[0] / cos_auto(y));
      // ld z = zlev == 1 ? 0 : geom3::factor_to_lev(zlev);
      
      ld factor = geom3::lev_to_factor(y + geom3::depth);
      
      ret[0] = sinh(x) * factor;
      ret[1] = cosh(x) * factor;
      ret[2] = 0;
      
      if(conformal::use_atan) {
        ret[0] = atan(ret[0]);
        ret[1] = atan(ret[1]);
        }

      break;
      }
    
    case mdFormula: {
      dynamicval<eModel> m(pmodel, conformal::basic_model);
      applymodel(H, ret);
      exp_parser ep;
      ep.extra_params["z"] = cld(ret[0], ret[1]);
      ep.extra_params["cx"] = ret[0];
      ep.extra_params["cy"] = ret[1];
      ep.extra_params["cz"] = ret[2];
      ep.extra_params["ux"] = H[0];
      ep.extra_params["uy"] = H[1];
      ep.extra_params["uz"] = H[2];
      ep.s = conformal::formula;
      cld res = ep.parse();
      ret[0] = real(res);
      ret[1] = imag(res);
      ret[2] = 0;
      break;
      }
    
    case mdSpiral: {
      cld z;
      if(hyperbolic) makeband(H, ret, band_conformal);
      else ret = H;
      z = cld(ret[0], ret[1]) * conformal::spiral_multiplier;
      
      if(conformal::spiral_cone < 360) {
        ld alpha = imag(z) * 360 / conformal::spiral_cone;
        ld r = real(z);
        r = exp(r);
        
        ret[0] = -sin(alpha) * r;
        ret[1] = cos(alpha) * r;
        ret[2] = (r-1) * sqrt( pow(360/conformal::spiral_cone, 2) - 1);
        
        conformal::apply_ball(ret[2], ret[1]);
        }
      else {      
        z = exp(z);
        ret[0] = real(z);
        ret[1] = imag(z);

        if(vid.skiprope) 
          ret = mobius(ret, vid.skiprope, 1);
        }
      }
    
    case mdGUARD: break;
    }

  ghcheck(ret,H_orig);
  }

// game-related graphics

transmatrix sphereflip; // on the sphere, flip
bool playerfound; // has player been found in the last drawing?

double q3 = sqrt(double(3));

bool outofmap(hyperpoint h) {
  if(euclid) 
    return h[2] < .5; // false; // h[0] * h[0] + h[1] * h[1] > 15 * eurad;
  else if(sphere)
    return h[2] < .1 && h[2] > -.1 && h[1] > -.1 && h[1] < .1 && h[0] > -.1 && h[0] < .1;
  else
    return h[2] < .5;
  }

hyperpoint mirrorif(const hyperpoint& V, bool b) {
  if(b) return Mirror*V;
  else return V;
  }

transmatrix mirrorif(const transmatrix& V, bool b) {
  if(b) return V*Mirror;
  else return V;
  }

// -1 if away, 0 if not away
int away(const transmatrix& V2) {
  return (intval(C0, V2 * xpush0(.1)) > intval(C0, tC0(V2))) ? -1 : 0;
  }

/* double zgrad(double f1, double f2, int nom, int den) {
  using namespace geom3;
  ld fo1 = factor_to_lev(f1);
  ld fo2 = factor_to_lev(f2);
  return lev_to_factor(fo1 + (fo2-fo1) * nom / den);
  } */

double zgrad0(double l1, double l2, int nom, int den) {
  using namespace geom3;
  return lev_to_factor(l1 + (l2-l1) * nom / den);
  }

bool behindsphere(const hyperpoint& h) {
  if(!sphere) return false;

  if(mdBandAny()) return false;

  if(vid.alpha > 1) {
     if(h[2] > -1/vid.alpha) return true;
     }  
  
  if(vid.alpha <= 1) {
    if(h[2] < .2-vid.alpha) return true;
    }
  
  return false;
  }

ld to01(ld a0, ld a1, ld x) {
  if(x < a0) return 0;
  if(x > a1) return 1;
  return (x-a0) / (a1-a0);
  }

ld spherity(const hyperpoint& h) {
  if(!sphere) return 1;
  
  if(vid.alpha > 1) {
    return to01(1/vid.alpha, 1, -h[2]);
    }  
  
  if(vid.alpha <= 1) {
    return to01(-1.5, 1, h[2]);
    }
      
  return 1;
  }

bool behindsphere(const transmatrix& V) {
  return behindsphere(tC0(V));
  }

ld spherity(const transmatrix& V) {
  return spherity(tC0(V));
  }

bool confusingGeometry() {
  return quotient;
  }

ld master_to_c7_angle() {
  #if CAP_GP
  auto alpha = gp::alpha;
  #else
  auto alpha = 0;
  #endif  
  return (!BITRUNCATED && !binarytiling && !archimedean) ? M_PI + alpha : 0;
  }

transmatrix actualV(const heptspin& hs, const transmatrix& V) {
  if(DIM == 3) return V;
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
  return (hs.spin || !BITRUNCATED) ? V * spin(hs.spin*2*M_PI/S7 + master_to_c7_angle()) : V;
  }

transmatrix applyspin(const heptspin& hs, const transmatrix& V) {
  #if CAP_BT
  if(binarytiling) return V;
  #endif
  #if CAP_ARCM
  if(archimedean) return V * spin(arcm::current.triangles[arcm::id_of(hs.at)][hs.spin].first);
  #endif
  return hs.spin ? V * spin(hs.spin*2*M_PI/S7) : V;
  }

bool invis_point(const hyperpoint h) {
  if(DIM == 2 || sphere) return false;
  return h[2] < 0;
  }

bool invalid_point(const hyperpoint h) {
  return std::isnan(h[DIM]) || h[DIM] > 1e8 || std::isinf(h[DIM]);
  }

bool invalid_point(const transmatrix T) {
  return std::isnan(T[DIM][DIM]) || T[DIM][DIM] > 1e8 || std::isinf(T[DIM][DIM]);
  }

bool in_smart_range(const transmatrix& T) {
  if(invalid_point(T)) return false;
  hyperpoint h1;
  applymodel(tC0(T), h1);
  for(int i=0; i<DIM; i++) 
    if(std::isnan(h1[i]) || std::isinf(h1[i])) return false;
  ld x = current_display->xcenter + current_display->radius * h1[0];
  ld y = current_display->ycenter + current_display->radius * h1[1] * vid.stretch;

  if(x > current_display->xtop + current_display->xsize * 2) return false;
  if(x < current_display->xtop - current_display->xsize * 1) return false;
  if(y > current_display->ytop + current_display->ysize * 2) return false;
  if(y < current_display->ytop - current_display->ysize * 1) return false;
  if(DIM == 3) {
    if(-h1[2] < conformal::clip_min * 2 - conformal::clip_max) return false;
    if(-h1[2] > conformal::clip_max * 2 - conformal::clip_min) return false;
    }

  ld epsilon = 0.01;
  
  ld dx = 0, dy = 0, dz = 0, dh[MAXMDIM];
  for(int i=0; i<DIM; i++) {    
    hyperpoint h2;
    applymodel(T * cpush0(i, epsilon), h2);
    ld x1 = current_display->radius * abs(h2[0] - h1[0]) / epsilon;
    ld y1 = current_display->radius * abs(h2[1] - h1[1]) * vid.stretch / epsilon;
    dx = max(dx, x1); dy = max(dy, y1);
    if(DIM == 3) dz = max(dz, abs(h2[2] - h1[2]));
    dh[i] = hypot(x1, y1);
    }
  
  if(DIM == 3) { 
    if(-h1[2] + 2 * dz < conformal::clip_min || -h1[2] - 2 * dz > conformal::clip_max) return false;
    sort(dh, dh+DIM); 
    ld scale = sqrt(dh[1] * dh[2]) * scalefactor * hcrossf7;
    if(scale <= vid.smart_range_detail_3) return false;
    }
  else {
    ld scale = sqrt(dh[0] * dh[1]) * scalefactor * hcrossf7;
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
    transmatrix V1 = V * ddspin(c, i) * xpush(crossf) * iddspin(c2, 0) * spin(M_PI);
    drawrec(c2, V1);
    }
  } */
  
  gp::local_info draw_li;

  bool drawrec(cell *c, const transmatrix& V, gp::loc at, int dir, int maindir) { 
    bool res = false;
    transmatrix V1 = V * Tf[draw_li.last_dir][at.first&31][at.second&31][fixg6(dir)];
    if(do_draw(c, V1)) {
      /* auto li = get_local_info(c);
      if(fix6(dir) != fix6(li.total_dir)) printf("totaldir %d/%d\n", dir, li.total_dir);
      if(at != li.relative) printf("at %s/%s\n", disp(at), disp(li.relative));
      if(maindir != li.last_dir) printf("ld %d/%d\n", maindir, li.last_dir); */
      draw_li.relative = at;
      draw_li.total_dir = fixg6(dir);
      drawcell(c, V1, 0, false);
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
      drawcell(c, V, 0, false), res = true;
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

void hrmap_standard::draw() {
  drawn_cells.clear();
  drawn_cells.emplace_back(viewctr, hsOrigin, cview(), band_shift);
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
          drawcell(hi.subcells[i], V0 * irr::cells[vc[i]].pusher, 0, false);
        }
      }
    #endif
    
    else {
      if(do_draw(c, V1)) {
        transmatrix V2 = actualV(hs, V1);
        drawcell(c, V2, 0, hs.mirrored);
        draw = true;
        }
  
      if(BITRUNCATED) for(int d=0; d<S7; d++) {
        int ds = hs.at->c.fix(hs.spin + d);
        // createMov(c, ds);
        if(c->move(ds) && c->c.spin(ds) == 0) {
          transmatrix V2 = V1 * hexmove[d];
          if(do_draw(c->move(ds), V2))
            draw = true,
            drawcell(c->move(ds), V2, 0, hs.mirrored ^ c->c.mirror(ds));
          }
        }
      }
  
    if(draw) for(int d=0; d<S7; d++) {
      hstate s2 = transition(s, d);
      if(s2 == hsError) continue;
      heptspin hs2 = hs + d + wstep;
      transmatrix Vd = V * heptmove[d];
      bandfixer bf(Vd);
      drawn_cells.emplace_back(hs2, s2, Vd, band_shift);
      }
    }
  }

int mindx=-7, mindy=-7, maxdx=7, maxdy=7;
  
transmatrix eumove(ld x, ld y) {
  transmatrix Mat = Id;
  Mat[2][2] = 1;
  
  if(a4) {
    Mat[0][2] += x * eurad;
    Mat[1][2] += y * eurad;
    }
  else {
    Mat[0][2] += (x + y * .5) * eurad;
    // Mat[2][0] += (x + y * .5) * eurad;
    Mat[1][2] += y * q3 /2 * eurad;
    // Mat[2][1] += y * q3 /2 * eurad;
    }
  
  ld v = a4 ? 1 : q3;

  while(Mat[0][2] <= -16384 * eurad) Mat[0][2] += 32768 * eurad;
  while(Mat[0][2] >= 16384 * eurad) Mat[0][2] -= 32768 * eurad;
  while(Mat[1][2] <= -16384 * v * eurad) Mat[1][2] += 32768 * v * eurad;
  while(Mat[1][2] >= 16384 * v * eurad) Mat[1][2] -= 32768 * v * eurad;
  return Mat;
  }

transmatrix eumove(int vec) {
  int x, y;
  tie(x,y) = vec_to_pair(vec);
  return eumove(x, y);
  }

transmatrix eumovedir(int d) {
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
    d = fix6(d);
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

void spinEdge(ld aspd) { 

  if(playerfound && vid.fixed_facing) {
    hyperpoint H = gpushxto0(playerV * C0) * playerV * xpush0(5);
    downspin = atan2(H[1], H[0]);
    downspin += vid.fixed_facing_dir * degree;
    if(flipplayer) downspin += M_PI;
    while(downspin < -M_PI) downspin += 2*M_PI;
    while(downspin > +M_PI) downspin -= 2*M_PI;
    aspd = (1 + 2 * abs(downspin)) * aspd;
    }
  if(downspin >  aspd) downspin =  aspd;
  if(downspin < -aspd) downspin = -aspd;
  View = spin(downspin) * View;
  }

void centerpc(ld aspd) { 
  
  if(subscreens::split([=] () {centerpc(aspd);})) return;

  #if CAP_CRYSTAL
  if(geometry == gCrystal)
    crystal::centerrug(aspd);
  #endif

  if(shmup::on && DIM == 3 && vid.sspeed > -5) {
    int id = subscreens::in ? subscreens::current_player : 0;
    if(false) { // gmatrix.count(shmup::pc[id]->base)) {
      transmatrix at = ggmatrix(shmup::pc[id]->base) * shmup::pc[id]->at * cpush(2, -vid.yshift);  
      View = inverse(at) * View;
      }
    else {
      viewctr = shmup::pc[id]->base->master;
      View = inverse(shmup::pc[id]->at * cpush(2, -vid.yshift));
      }
    #if CAP_RACING
    if(racing::on) racing::set_view();
    #endif
    return;
    }
  
  #if CAP_RACING
  if(racing::on && !racing::standard_centering) {
    racing::set_view();   
    return;
    }
  #endif
  
  if(ors::mode == 2 && vid.sspeed < 5) return;
  if(vid.sspeed >= 4.99) aspd = 1000;
  DEBB(DF_GRAPH, (debugfile,"center pc\n"));
  
  ors::unrotate(cwtV); ors::unrotate(View);
  
  hyperpoint H = tC0(cwtV);
  if(DIM == 2) H = ypush(-vid.yshift) * sphereflip * H;
  if(DIM == 3 && !shmup::on && vid.yshift) H = cpush(2, -vid.yshift) * H;
  ld R = zero_d(DIM, H) ? 0 : hdist0(H);
  if(R < 1e-9) {
    // either already centered or direction unknown
    /* if(playerfoundL && playerfoundR) {
      
      } */
    spinEdge(aspd);
    fixmatrix(View);
    ors::rerotate(cwtV); ors::rerotate(View);
    return;
    }
  
  if(euclid) {
    // Euclidean
    aspd *= (2+3*R*R);
    if(aspd > R) aspd = R;
    
    for(int i=0; i<DIM; i++)
      View[i][DIM] -= H[i] * aspd / R;
        
    }
  
  else {
    aspd *= (1+R+(shmup::on?1:0));

    if(R < aspd) {
      View = gpushxto0(H) * View;
      }
    else 
      View = rspintox(H) * xpush(-aspd) * spintox(H) * View;
      
    fixmatrix(View);
    spinEdge(aspd);
    }

  ors::rerotate(cwtV); ors::rerotate(View);
  }

void optimizeview() {

  if(subscreens::split(optimizeview)) return;
  
  #if CAP_ANIMATIONS
  if(centerover.at && inmirror(centerover.at)) {
    anims::reflect_view();
    }
  #endif
  
  DEBB(DF_GRAPH, (debugfile,"optimize view\n"));
  int turn = 0;
  ld best = INF;
  
  transmatrix TB = Id;
  
  if(0) ;

  #if CAP_BT || CAP_ARCM || MAXMDIM == 4
  else if(binarytiling || archimedean || DIM == 3) {
    turn = -1, best = hdist0(tC0(View));
    for(int i=0; i<viewctr.at->c7->type; i++) {
      int i1 = i * DUALMUL;
      heptagon *h2 = createStep(viewctr.at, i1);
      transmatrix T = currentmap->relative_matrix(h2, viewctr.at);
      hyperpoint H = View * tC0(T);
      ld quality = hdist0(H);
      if(quality < best) best = quality, turn = i1, TB = T;
      }
    if(turn >= 0) {
      View = View * TB;
      fixmatrix(View);
      viewctr.at = createStep(viewctr.at, turn);
      }
    }
  #endif
  
  else {
  
    for(int i=-1; i<S7; i++) {
  
      ld trot = -i * M_PI * 2 / (S7+.0);
      transmatrix T = i < 0 ? Id : spin(trot) * xpush(tessf) * pispin;
      hyperpoint H = View * tC0(T);
      if(H[DIM] < best) best = H[DIM], turn = i, TB = T;
      }
  
    if(turn >= 0) {
      View = View * TB;
      fixmatrix(View);
      viewctr = viewctr + turn + wstep;
      }
    }
  }

void addball(ld a, ld b, ld c) {
  hyperpoint h;
  ballmodel(h, a, b, c);
  for(int i=0; i<3; i++) h[i] *= current_display->radius;
  curvepoint(h);
  }

void ballgeometry() {
  queuereset(vid.usingGL ? mdDisk : mdUnchanged, PPR::CIRCLE);
  for(int i=0; i<60; i++)
    addball(i * M_PI/30, 10, 0);
  for(double d=10; d>=-10; d-=.2)
    addball(0, d, 0);
  for(double d=-10; d<=10; d+=.2)
    addball(0, d, geom3::depth);
  addball(0, 0, -geom3::camera);
  addball(0, 0, geom3::depth);
  addball(0, 0, -geom3::camera);
  addball(0, -10, 0);
  addball(0, 0, -geom3::camera);
  queuecurve(darkena(0xFF, 0, 0x80), 0, PPR::CIRCLE);
  queuereset(pmodel, PPR::CIRCLE);
  }

void resetview() {
  DEBB(DF_GRAPH, (debugfile,"reset view\n"));
  View = Id;
  // EUCLIDEAN
  if(!masterless) 
    viewctr.at = cwt.at->master,
    viewctr.spin = cwt.spin;
  else centerover = cwt;
  cwtV = Id;
  // SDL_LockSurface(s);
  // SDL_UnlockSurface(s);
  }


void panning(hyperpoint hf, hyperpoint ht) {
  View = 
    rgpushxto0(hf) * rgpushxto0(gpushxto0(hf) * ht) * gpushxto0(hf) * View;
  playermoved = false;
  }

int cells_drawn;

void fullcenter() {
  conformal::path_for_lineanimation.clear();
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

transmatrix atscreenpos(ld x, ld y, ld size) {
  transmatrix V = Id;

  V[0][2] += (x - current_display->xcenter);
  V[1][2] += (y - current_display->ycenter);
  V[0][0] = size * 2 * hcrossf / crossf;
  V[1][1] = size * 2 * hcrossf / crossf;
  V[2][2] = current_display->scrdist;

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

color_t periodcolor = 0x00FF0080;
color_t ringcolor = darkena(0xFF, 0, 0xFF);
color_t modelcolor = 0;

#if CAP_QUEUE
void draw_model_elements() {

  switch(pmodel) {
  
    case mdRotatedHyperboles: {
      queuechr(current_display->xcenter, current_display->ycenter + current_display->radius * vid.alpha, 0, vid.fsize, 'X', ringcolor, 1, 8);
      return;
      }

    case mdTwoPoint: {
      ld a = -conformal::model_orientation * degree;
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
        
        ld& tz = conformal::top_z;
        ld z = acosh(tz);
    
        hyperpoint a = xpush0(z);
        ld cb = conformal::cos_ball;
        ld sb = conformal::sin_ball;
        
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

  using namespace hyperpoint_vec;
  hyperpoint H;
  applymodel(X, H);
  H *= current_display->radius;
  ld mul = hypot(vid.xres, vid.yres) / hypot_d(2, H);
  ld m = style == 1 ? -mul : -1;
  
  queuereset(mdUnchanged, p);
  curvepoint(H + spin(M_PI/2) * H * mul);
  curvepoint(H - spin(M_PI/2) * H * mul);
  curvepoint(m * H - spin(M_PI/2) * H * mul);
  curvepoint(m * H + spin(M_PI/2) * H * mul);
  curvepoint(H + spin(M_PI/2) * H * mul);

  queuecurve(lc, fc, p).flags |= POLY_ALWAYS_IN;
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

void draw_boundary(int w) {

  if(w == 1) return;

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

  if(elliptic && !among(pmodel, mdBand, mdBandEquidistant, mdBandEquiarea, mdSinusoidal))
    circle_around_center(M_PI/2, periodcolor, 0, PPR::CIRCLE);
  
  switch(pmodel) {
  
    case mdTwoPoint: {
      if(twopoint_do_flips || current_display->stereo_active() || !sphere) return;
      queuereset(vid.usingGL ? mdDisk : mdUnchanged, p);
  
      for(int b=-1; b<=1; b+=2)
      for(ld a=-90; a<=90+1e-6; a+=pow(.5, vid.linequality)) {
        using namespace hyperpoint_vec;
        ld x = sin(a * vid.twopoint_param * b / 90);
        ld y = 0;
        ld z = -sqrt(1 - x*x);
        conformal::apply_orientation(y, x);
        hyperpoint h1;
        applymodel(hpxyz(x,y,z), h1);
        
        conformal::apply_orientation(h1[0], h1[1]);      
        h1[1] = abs(h1[1]) * b;
        conformal::apply_orientation(h1[1], h1[0]);
        curvepoint(h1);
        }
  
      queuecurve(lc, fc, p);
      queuereset(pmodel, p);
      return;
      }
    
    case mdBand: case mdBandEquidistant: case mdBandEquiarea: case mdSinusoidal: {
      if(pmodel == mdBand && conformal::model_transition != 1) return;
      bool bndband = ((pmodel == mdBand) ? hyperbolic : sphere);
      transmatrix T = spin(-conformal::model_orientation * degree);
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
        queuecurve(periodcolor, 0, PPR::CIRCLE);
        }
      return;
      }
    
    case mdHalfplane: 
      if(hyperbolic) {
        queuestraight(xspinpush0(-conformal::model_orientation * degree - M_PI/2, fakeinf), 1, lc, fc, p);
        return;
        }
      break;
    
    case mdHemisphere: {
      if(hyperbolic) {
        queuereset(mdUnchanged, p);
        for(int i=0; i<=360; i++) {
          ld s = sin(i * degree);
          curvepoint(point3(current_display->radius * cos(i * degree), current_display->radius * s * (conformal::cos_ball * s >= 0 - 1e-6 ? 1 : abs(conformal::sin_ball)), 0));
          }
        queuecurve(lc, fc, p);
        queuereset(pmodel, p);
        p = PPR::CIRCLE; fc = 0;
        queuereset(mdUnchanged, p);
  
        for(int i=0; i<=360; i++) {
          ld s = sin(i * degree);
          curvepoint(point3(current_display->radius * cos(i * degree), current_display->radius * s * conformal::sin_ball, 0));
          }
        queuecurve(lc, fc, p);
        queuereset(pmodel, p);
        }
      if(euclid || sphere) {
        queuereset(mdUnchanged, p);  
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
        ld& tz = conformal::top_z;
        ld mz = acosh(tz);
        ld cb = conformal::cos_ball;
        ld sb = conformal::sin_ball;
        
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
      using namespace hyperpoint_vec;
      if(euclid) return;
      // if(p == PPR::CIRCLE) p = PPR::OUTCIRCLE;
      auto& sm = conformal::spiral_multiplier;
      ld u = hypot(1, imag(sm) / real(sm));
      if(real(sm)) {
        queuereset(mdUnchanged, p);
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

ld band_shift = 0;
void fix_the_band(transmatrix& T) {
  if((models[pmodel].flags & mf::quasiband) && T[2][2] > 1e6) {
    hyperpoint H = tC0(T);
    find_zlev(H);
    conformal::apply_orientation(H[0], H[1]);
    
    ld y = asin_auto(H[1]);
    ld x = asin_auto_clamp(H[0] / cos_auto(y));
    band_shift += x;
    // printf("fixing with shift = %lf\n", x);
    T = xpush(-x) * T;
    fixmatrix(T);
    // todo orientation
    }
  }

namespace dq {
  queue<tuple<heptagon*, transmatrix, ld>> drawqueue;

  set<heptagon*> visited;
  void enqueue(heptagon *h, const transmatrix& T) {
    if(!h || visited.count(h)) { return; }
    visited.insert(h);
    drawqueue.emplace(h, T, band_shift);
    }  

  set<int> visited_by_matrix;
  void enqueue_by_matrix(heptagon *h, const transmatrix& T) {
    if(!h) return;
    int b = reg3::bucketer(tC0(T));
    if(visited_by_matrix.count(b)) { return; }
    visited_by_matrix.insert(b);
    drawqueue.emplace(h, T, band_shift);
    }
  }

bool do_draw(cell *c) {
  // do not display out of range cells, unless on torus
  if(c->pathdist == PINFD && geometry != gTorus && vid.use_smart_range == 0)
    return false;
  // do not display not fully generated cells, unless changing range allowed
  if(c->mpdist > 7 && !allowChangeRange()) return false;
  // in the Yendor Challenge, scrolling back is forbidden
  if(c->cpdist > 7 && yendor::on && !cheater && !autocheat) return false;

  return true;
  }  

ld extra_generation_distance = 99;

bool do_draw(cell *c, const transmatrix& T) {
  if(DIM == 3) {
    if(cells_drawn > vid.cells_drawn_limit) return false;
    if(vid.use_smart_range) {
      if(cells_drawn >= 50 && !in_smart_range(T)) return false;
      setdist(c, 7, c);
      }
    else {
      ld dist = hdist0(tC0(T));
      if(dist > sightranges[geometry]) return false;
      if(dist <= extra_generation_distance) setdist(c, 7, c);
      }
    return true;
    }

  if(just_gmatrix && sphere) return true;
  if(!do_draw(c)) return false;
  if(euclid && pmodel == mdSpiral) {
    hyperpoint h = tC0(T);
    cld z(h[0], h[1]);
    z = z * conformal::spiral_multiplier;
    ld iz = imag(z) + 1.14279e-2; // make it never fall exactly on PI
    if(iz < -M_PI || iz >= M_PI) return false;
    }
  if(hyperbolic && pmodel == mdSpiral && conformal::ring_not_spiral) {
    cld z;
    hyperpoint H = tC0(T);
    hyperpoint ret;
    makeband(H, ret, band_conformal);
    z = cld(ret[0], ret[1]) * conformal::spiral_multiplier;
    if(imag(z) < -conformal::spiral_cone_rad/2-1e-5 || imag(z) >= conformal::spiral_cone_rad/2-1e-5) return false;
    }
  if(cells_drawn > vid.cells_drawn_limit) return false;
  bool usr = vid.use_smart_range || quotient || euwrap;
  if(usr && cells_drawn >= 50 && !in_smart_range(T)) return false;
  if(vid.use_smart_range == 2) setdist(c, 7, c);
  return true; 
  }

int cone_side(const hyperpoint H) {
  hyperpoint ret;
  if(hyperbolic) makeband(H, ret, band_conformal);
  else ret = H;
  cld z = cld(ret[0], ret[1]) * conformal::spiral_multiplier;
  
  auto zth = [&] (cld z) {
    ld alpha = imag(z) * 360 / conformal::spiral_cone;
    ld r = real(z);
    r = exp(r);
    
    hyperpoint ret;
      
    ret[0] = -sin(alpha) * r;
    ret[1] = cos(alpha) * r;
    ret[2] = (r-1) * sqrt( pow(360/conformal::spiral_cone, 2) - 1);
    
    conformal::apply_ball(ret[2], ret[1]);
    return ret;
    };
  
  hyperpoint ret0 = zth(z);
  hyperpoint ret1 = zth(z + cld(1e-3, 0));
  hyperpoint ret2 = zth(z + cld(0, 1e-3));

  return (ret1[1] - ret0[1]) * (ret2[0] - ret0[0]) < (ret2[1] - ret0[1]) * (ret1[0] - ret0[0]) ? 1 : -1;
  }

}
