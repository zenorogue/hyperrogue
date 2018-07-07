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
  ld cam = vid.camera_angle * M_PI / 180;
  GLfloat cc = cos(cam);
  GLfloat ss = sin(cam);
  ld ux = hx, uy = hy * cc + ss, uz = cc - ss * hy;
  hx = ux / uz, hy = uy / uz;
  }

hyperpoint gethyper(ld x, ld y) {

  ld hx = (x - vid.xcenter) / vid.radius;
  ld hy = (y - vid.ycenter) / vid.radius;
  
  if(pmodel) {
    ghx = hx, ghy = hy;
    return ghpm;
    }
  
  if(vid.camera_angle) camrotate(hx, hy);
  
  if(euclid)
    return hpxy(hx * (1 + vid.alpha), hy * (1 + vid.alpha));
    
  ld hr = hx*hx+hy*hy;
  
  if(hr > .9999 && !sphere) return Hypc;
  
  // hz*hz-(hx/(hz+alpha))^2 - (hy/(hz+alpha))^2 =
  
  // hz*hz-hr*(hz+alpha)^2 == 1
  // hz*hz - hr*hr*hz*Hz
  
  
  ld A, B, C;
  
  ld curv = sphere ? 1 : -1;
  
  A = 1+curv*hr;
  B = 2*hr*vid.alpha*-curv;
  C = 1 - curv*hr*vid.alpha*vid.alpha;
  
  // Az^2 - Bz = C
  B /= A; C /= A;
  
  // z^2 - Bz = C
  // z^2 - Bz + (B^2/4) = C + (B^2/4)
  // z = (B/2) + sqrt(C + B^2/4)
  
  ld rootsign = 1;
  if(sphere && vid.alpha > 1) rootsign = -1;
  
  ld hz = B / 2 + rootsign * sqrt(C + B*B/4);
  
  hyperpoint H;
  H[0] = hx * (hz+vid.alpha);
  H[1] = hy * (hz+vid.alpha);
  H[2] = hz;
  
  return H;
  }

void ballmodel(hyperpoint& ret, double alpha, double d, double zl) {
  hyperpoint H = ypush(geom3::camera) * xpush(d) * ypush(zl) * C0;
  ld tzh = vid.ballproj + H[2];
  ld ax = H[0] / tzh;
  ld ay = H[1] / tzh;
  ld ball = vid.ballangle * M_PI / 180;
  
  ld ca = cos(alpha), sa = sin(alpha);
  ld cb = cos(ball), sb = sin(ball);
  
  ret[0] = ax * ca;
  ret[1] = ay * cb + ax * sa * sb;
  ret[2] = ax * sa * cb - ay * sb;
  }

void apply_depth(hyperpoint &f, ld z) {
  if(vid.usingGL)
    f[2] = z;
  else {
    z = z * vid.radius;
    ld mul = stereo::scrdist / (stereo::scrdist + z);
    f[0] = f[0] * mul;
    f[1] = f[1] * mul;
    f[2] = vid.xres * stereo::eyewidth() / 2 / vid.radius + stereo::ipd * mul / 2;
    }
  }

bool hypot_zlev(bool zlev_used, ld& d, ld zlev, ld& df, ld& zf, ld &z) {
  if(!zlev_used) {
    df = 1; zf = 0;
    return false;
    }
  else {
    // (0,0,1) -> (0, sin z, cos z) -> (sin d cos z, sin z, cos d cos z)
    ld z = geom3::factor_to_lev(zlev);
    ld tz = sin_auto(z);
    ld td = sin_auto(abs(d)) * cos_auto(z);
    ld h = hypot(td, tz);
    if(d > 0)
      d = hypot_auto(d, z);
    else
      d = -hypot_auto(d, z);
    zf = tz / h, df = td / h;
    return true;
    }
  }

bool hypot_zlev(bool zlev_used, ld& d, ld zlev, ld& df, ld& zf) {
  ld z;
  return hypot_zlev(zlev_used, d, zlev, df, zf, z);
  }

int twopoint_sphere_flips;
bool twopoint_do_flips;

void applymodel(hyperpoint H, hyperpoint& ret) {
  
  ld tz = euclid ? (1+vid.alpha) : vid.alpha+H[2];
  if(tz < BEHIND_LIMIT && tz > -BEHIND_LIMIT) tz = BEHIND_LIMIT;
  
  if(pmodel == mdUnchanged) { 
    for(int i=0; i<3; i++) ret[i] = H[i] / vid.radius;
    return; 
    }
  
  if(pmodel == mdBall) {
    ld zlev = zlevel(H);
    using namespace hyperpoint_vec;
    H = H / zlev;
    
    ld zl = geom3::depth-geom3::factor_to_lev(zlev);
    double alpha = atan2(H[1], H[0]);
    double d = hdist0(H);

    ballmodel(ret, alpha, d, zl);
    ghcheck(ret,H);
    
    return;
    }
  
  if(pmodel == mdHemisphere) {
    ld ball = vid.ballangle * M_PI / 180;
    using namespace hyperpoint_vec;
    
    switch(cgclass) {
      case gcHyperbolic: {
        ld zl = zlevel(H);
        ret = H / H[2];
        ret[2] = sqrt(1 - sqhypot2(ret));
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
    
    ret = rotmatrix(M_PI/2 + ball, 1, 2) * ret;
    
    ghcheck(ret, H);
    return;
    }

  if(pmodel == mdHyperboloid) {

    ld ball = -vid.ballangle * M_PI / 180;
    ld cb = cos(ball), sb = sin(ball);

    ret[0] = H[0] / 3;
    ret[1] = (1 - H[2]) / 3 * cb - H[1] / 3 * sb;
    ret[2] = -(-H[1] / 3 * cb - (1 - H[2]) / 3 * sb);

    ghcheck(ret,H);
    return;
    }
  
  if(pmodel == mdDisk) {
  
    if(!vid.camera_angle) {
      ret[0] = H[0] / tz;
      ret[1] = H[1] / tz;
      ret[2] = vid.xres / vid.radius * stereo::eyewidth() / 2 - stereo::ipd / tz / 2;
      }
    else {
      ld tx = H[0];
      ld ty = H[1];
      ld cam = vid.camera_angle * M_PI / 180;
      GLfloat cc = cos(cam);
      GLfloat ss = sin(cam);
      ld ux = tx, uy = ty * cc - ss * tz, uz = tz * cc + ss * ty;
      ret[0] = ux / uz;
      ret[1] = uy / uz;
      ret[2] = vid.xres / vid.radius * stereo::eyewidth() / 2 - stereo::ipd / uz / 2;
      }
    return;
    }

  if(pmodel == mdFisheye) {
    ret[0] = H[0] / tz;
    ret[1] = H[1] / tz;
    ld hypot = sqrt(1 + ret[0]*ret[0] + ret[1]*ret[1]);
    ret[0] /= hypot;
    ret[1] /= hypot;
    ghcheck(ret, H);
    return;
    }

  ld zlev = 1;
  bool zlev_used = false;

  if(wmspatial || mmspatial) {
    zlev = zlevel(H);
    using namespace hyperpoint_vec;
    zlev_used = !((zlev > 1-1e-6 && zlev < 1+1e-6));
    if(zlev_used) H /= zlev;
    }
  
  if(pmodel == mdTwoPoint || mdBandAny() || pmodel == mdSinusoidal) {
    // map to plane
    if(false) {
      auto p = vid.twopoint_param;
      ld dleft = hdist(H, xpush(-p) * C0);
      ld dright = hdist(H, xpush(p) * C0);
      ld yf = 1, zf = 0;
      if(zlev_used) {
        ld y_orig = asin_auto(H[1]);
        ld z;
        hypot_zlev(true, y_orig, zlev, yf, zf, z);
        dleft = hypot_auto(dleft, z);
        dright = hypot_auto(dright, z);
        }
      ld x = (dright*dright-dleft*dleft) / 4 / p;
      ld y = sqrt(dleft * dleft - (x-p)*(x-p) + 1e-9);
      x = -x;
      ret = hpxyz(x/M_PI, y*(H[1]<0?-1:1)*yf/M_PI, 0);
      if(zlev_used && stereo::active()) 
        apply_depth(ret, y * zf / M_PI);
      }
    else {
      ld x, y, yf, zf;
      y = asin_auto(H[1]);
      x = asin_auto_clamp(H[0] / cos_auto(y));
      if(sphere) {
        if(H[2] < 0 && x > 0) x = M_PI - x;
        else if(H[2] < 0 && x <= 0) x = -M_PI - x;
        }
      hypot_zlev(zlev_used, y, zlev, yf, zf);
      if(pmodel == mdTwoPoint) {
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
      else if(pmodel == mdBand) switch(cgclass) {
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
      else if(pmodel == mdBandEquiarea) 
        y = sin_auto(y);
      else if(pmodel == mdSinusoidal) 
        x *= cos_auto(y);
      ret = hpxyz(x / M_PI, y * yf / M_PI, 0);
      if(zlev_used && stereo::active()) 
        apply_depth(ret, y * zf / M_PI);
      }
    ghcheck(ret, H);
    return;
    }

  if(mdAzimuthalEqui()) {
    ld rad = sqrt(H[0] * H[0] + H[1] * H[1]);
    if(rad == 0) rad = 1;
    ld d = hdist0(H);
    ld yf, zf;
    hypot_zlev(zlev_used, d, zlev, yf, zf);
    
    // 4 pi / 2pi = M_PI 
    
    if(pmodel == 6 && sphere)
      d = sqrt(2*(1 - cos(d))) * M_PI / 2;
    else if(pmodel == 6 && !euclid)
      d = sqrt(2*(cosh(d) - 1)) / 1.5;
    ret[0] = d * yf * H[0] / rad / M_PI;
    ret[1] = d * yf * H[1] / rad / M_PI;
    ret[2] = 0; 
    if(zlev_used && stereo::active()) 
      apply_depth(ret, d * zf / M_PI);
    ghcheck(ret,H);

    return;
    }
  
  tz = H[2]+vid.alpha;

  if(pmodel == mdPolygonal || pmodel == mdPolynomial) {
    pair<long double, long double> p = polygonal::compute(H[0]/tz, H[1]/tz);
    ret[0] = p.first;
    ret[1] = p.second;
    ret[2] = 0;
    ghcheck(ret,H);
    return;
    }
  
  if(pmodel == mdHalfplane) {
    // Poincare to half-plane
    
    ld x0, y0;  
    x0 = H[0] / tz;
    y0 = H[1] / tz;
    if(conformal::lower_halfplane) x0 = -x0, y0 = -y0;
    y0 += 1;
    double rad = x0*x0 + y0*y0;
    y0 /= rad;
    x0 /= rad;
    y0 -= .5;
  
    if(conformal::lower_halfplane) x0 = -x0, y0 = -y0;
    
    ret[0] = x0;
    if(wmspatial || mmspatial) {
      if(conformal::lower_halfplane) y0 /= zlev;
      else y0 *= zlev;
      }
    ret[1] = (conformal::lower_halfplane?-1:1) - y0;
    ret[2] = 0;
    if(zlev != 1 && stereo::active()) 
      apply_depth(ret, -y0 * geom3::factor_to_lev(zlev));
    ghcheck(ret,H);
    return;
    }
  }

// game-related graphics

transmatrix View; // current rotation, relative to viewctr
transmatrix cwtV = Id; // player-relative view
transmatrix sphereflip; // on the sphere, flip
heptspin viewctr; // heptagon and rotation where the view is centered at
bool playerfound; // has player been found in the last drawing?

#define eurad crossf

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
  return elliptic || quotient || torus;
  }

ld master_to_c7_angle() {
  return nonbitrunc ? M_PI + gp::alpha : 0;
  }

transmatrix actualV(const heptspin& hs, const transmatrix& V) {
  return (hs.spin || nonbitrunc) ? V * spin(hs.spin*2*M_PI/S7 + master_to_c7_angle()) : V;
  }

transmatrix applyspin(const heptspin& hs, const transmatrix& V) {
  return hs.spin ? V * spin(hs.spin*2*M_PI/S7) : V;
  }

// in hyperbolic quotient geometries, relying on pathdist is not sufficient
bool in_qrange(const transmatrix& V) {
  if(!quotient || !hyperbolic) return true;
  return V[2][2] < cosh(crossf * get_sightrange_ambush());
  }

namespace gp {

/*
void drawrec(cell *c, const transmatrix& V) {
  if(dodrawcell(c))
    drawcell(c, V, 0, false);
  for(int i=0; i<c->type; i++) {
    cell *c2 = c->mov[i];
    if(!c2) continue;
    if(c2->mov[0] != c) continue;
    if(c2 == c2->master->c7) continue;
    transmatrix V1 = V * ddspin(c, i) * xpush(crossf) * iddspin(c2, 0) * spin(M_PI);
    drawrec(c2, V1);
    }
  } */
  
  gp::local_info draw_li;

  void drawrec(cell *c, const transmatrix& V, gp::loc at, int dir, int maindir) {
    if(dodrawcell(c)) {
      /* auto li = get_local_info(c);
      if(fix6(dir) != fix6(li.total_dir)) printf("totaldir %d/%d\n", dir, li.total_dir);
      if(at != li.relative) printf("at %s/%s\n", disp(at), disp(li.relative));
      if(maindir != li.last_dir) printf("ld %d/%d\n", maindir, li.last_dir); */
      draw_li.relative = at;
      draw_li.total_dir = fix6(dir);
      transmatrix V1 = V * Tf[draw_li.last_dir][at.first&31][at.second&31][fix6(dir)];
      if(in_qrange(V1))
        drawcell(c, V1, 0, false);
      }
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->mov[i];
      if(!c2) continue;
      if(c2->mov[0] != c) continue;
      if(c2 == c2->master->c7) continue;
      drawrec(c2, V, at + eudir(dir+i), dir + i + 3, maindir);
      }
    }
  
  void drawrec(cell *c, const transmatrix& V) {
    draw_li.relative = loc(0,0);
    draw_li.total_dir = 0;
    draw_li.last_dir = -1;
    if(dodrawcell(c))
      drawcell(c, V, 0, false);
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->mov[i];
      if(!c2) continue;
      if(c2->mov[0] != c) continue;
      if(c2 == c2->master->c7) continue;
      draw_li.last_dir = i;
      drawrec(c2, V, gp::loc(1,0), 3, i);
      }
    }
  }

void drawrec(const heptspin& hs, hstate s, const transmatrix& V) {

  // shmup::calc_relative_matrix(cwt.c, hs.h);
    
  cell *c = hs.h->c7;
  
  transmatrix V10;
  const transmatrix& V1 = hs.mirrored ? (V10 = V * Mirror) : V;
  
  if(gp::on) {
    gp::drawrec(c, actualV(hs, V1));
    }
  
  else {
    if(dodrawcell(c)) {
      transmatrix V2 = actualV(hs, V1);
      drawcell(c, V2, 0, hs.mirrored);
      }
    
    if(!nonbitrunc) for(int d=0; d<S7; d++) {
      int ds = fixrot(hs.spin + d);
      // createMov(c, ds);
      if(c->mov[ds] && c->spn(ds) == 0 && dodrawcell(c->mov[ds])) {
        transmatrix V2 = V1 * hexmove[d];
        if(in_qrange(V2))
        drawcell(c->mov[ds], V2, 0, boolean_xor(hs.mirrored, c->mirror(ds)));
        }
      }
    }

  if(c->pathdist < PINFD && in_qrange(V))
  for(int d=0; d<S7; d++) {
    hstate s2 = transition(s, d);
    if(s2 == hsError) continue;
    heptspin hs2 = hs + d + wstep;
    drawrec(hs2, s2, V * heptmove[d]);
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

ld matrixnorm(const transmatrix& Mat) {
  return Mat[0][2] * Mat[0][2] + Mat[1][2] * Mat[1][2];
  }
  
void drawEuclidean() {
  DEBB(DF_GRAPH, (debugfile,"drawEuclidean\n"));
  sphereflip = Id;
  if(!centerover.c) centerover = cwt;
  // printf("centerover = %p player = %p [%d,%d]-[%d,%d]\n", lcenterover, cwt.c,
  //   mindx, mindy, maxdx, maxdy);
  int pvec = cellwalker_to_vec(centerover);
    
  int minsx = mindx-1, maxsx=maxdx+1, minsy=mindy-1, maxsy=maxdy+1;
  mindx=maxdx=mindy=maxdy=0;
  
  transmatrix View0 = View;
  
  ld cellrad = vid.radius / (1 + vid.alpha);
  
  ld centerd = matrixnorm(View0);
  
  for(int dx=minsx; dx<=maxsx; dx++)
  for(int dy=minsy; dy<=maxsy; dy++) {
    torusconfig::torus_cx = dx;
    torusconfig::torus_cy = dy;
    cellwalker cw = vec_to_cellwalker(pvec + euclid_getvec(dx, dy));
    transmatrix Mat = eumove(dx,dy);
    
    if(!cw.c) continue;

    Mat = View0 * Mat;
    
    if(true) {
      ld locald = matrixnorm(Mat);
      if(locald < centerd) centerd = locald, centerover = cw, View = View0 * eumove(dx, dy);
      }
    
    // Mat[0][0] = -1;
    // Mat[1][1] = -1;
    
    // Mat[2][0] = x*x/10;
    // Mat[2][1] = y*y/10;
    // Mat = Mat * xpush(x-30) * ypush(y-30);

    int cx, cy, shift;
    getcoord0(tC0(Mat), cx, cy, shift);
    if(cx >= 0 && cy >= 0 && cx < vid.xres && cy < vid.yres) {
      if(dx < mindx) mindx = dx;
      if(dy < mindy) mindy = dy;
      if(dx > maxdx) maxdx = dx;
      if(dy > maxdy) maxdy = dy;
      }
    if(cx >= -cellrad && cy >= -cellrad && cx < vid.xres+cellrad && cy < vid.yres+cellrad)
      if(dodrawcell(cw.c)) {
        drawcell(cw.c, cw.mirrored ? Mat * Mirror : Mat, cw.spin, cw.mirrored);
        }
    }
  }

void spinEdge(ld aspd) { 
  if(downspin >  aspd) downspin =  aspd;
  if(downspin < -aspd) downspin = -aspd;
  View = spin(downspin) * View;
  }

void centerpc(ld aspd) { 
  if(vid.sspeed >= 4.99) aspd = 1000;
  DEBB(DF_GRAPH, (debugfile,"center pc\n"));
  hyperpoint H = ypush(-vid.yshift) * sphereflip * tC0(cwtV);
  ld R = H[0] == 0 && H[1] == 0 ? 0 : hdist0(H); // = sqrt(H[0] * H[0] + H[1] * H[1]);
  if(R < 1e-9) {
    // either already centered or direction unknown
    /* if(playerfoundL && playerfoundR) {
      
      } */
    spinEdge(aspd);
    fixmatrix(View);
    return;
    }
  
  if(euclid) {
    // Euclidean
    aspd *= (2+3*R*R);
    if(aspd > R) aspd = R;
    
    View[0][2] -= cwtV[0][2] * aspd / R;
    View[1][2] -= cwtV[1][2] * aspd / R;
        
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
  }

void optimizeview() {
  
  DEBB(DF_GRAPH, (debugfile,"optimize view\n"));
  int turn = 0;
  ld best = INF;
  
  transmatrix TB = Id;
  
  for(int i=-1; i<S7; i++) {

    ld trot = -i * M_PI * 2 / (S7+.0);
    transmatrix T = i < 0 ? Id : spin(trot) * xpush(tessf) * pispin;
    hyperpoint H = View * tC0(T);
    if(H[2] < best) best = H[2], turn = i, TB = T;
    }

  if(turn >= 0) {
    View = View * TB;
    fixmatrix(View);
    viewctr = viewctr + turn + wstep;
    }
  }

void addball(ld a, ld b, ld c) {
  hyperpoint h;
  ballmodel(h, a, b, c);
  for(int i=0; i<3; i++) h[i] *= vid.radius;
  curvepoint(h);
  }

void ballgeometry() {
  queuereset(vid.usingGL ? mdDisk : mdUnchanged, PPR_CIRCLE);
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
  queuecurve(darkena(0xFF, 0, 0x80), 0, PPR_CIRCLE);
  queuereset(pmodel, PPR_CIRCLE);
  }

void resetview() {
  DEBB(DF_GRAPH, (debugfile,"reset view\n"));
  View = Id;
  // EUCLIDEAN
  if(!euclid) 
    viewctr.h = cwt.c->master,
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

void fullcenter() {
  if(playerfound && false) centerpc(INF);
  else {
    bfs();
    resetview();
    drawthemap();
    centerpc(INF);
    }
  playermoved = true;
  }

transmatrix screenpos(ld x, ld y) {
  transmatrix V = Id;
  V[0][2] += (x - vid.xcenter) / vid.radius * (1+vid.alpha);
  V[1][2] += (y - vid.ycenter) / vid.radius * (1+vid.alpha);
  return V;
  }

transmatrix atscreenpos(ld x, ld y, ld size) {
  transmatrix V = Id;

  V[0][2] += (x - vid.xcenter);
  V[1][2] += (y - vid.ycenter);
  V[0][0] = size * 2 * hcrossf / crossf;
  V[1][1] = size * 2 * hcrossf / crossf;
  V[2][2] = stereo::scrdist;

  return V;
  }

}
