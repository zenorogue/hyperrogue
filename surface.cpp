// Hyperbolic Rogue -- surfaces of constant negative curvature
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file surface.cpp
 *  \brief surfaces of constant negative curvature
 *
 *  See http://webmath2.unito.it/paginepersonali/sergio.console/CurveSuperfici/AG15.pdf for a nice reference
 */

#include "hyper.h"
namespace hr { 

#if CAP_SURFACE
EX namespace surface {

ld sech(ld d) { return 1 / cosh(d); }

#if HDR
enum eShape { dsHypersian, dsTorus, dsTractricoid, dsDini, dsKuen, dsHyperlike, dsHyperboloid, dsHemisphere, dsCrystal, dsHilbert };
#endif

string shape_name[] = { "hypersian rug", "conformal torus", "tractricoid", "Dini's surface", "Kuen surface", "concave barrel",
  "hyperboloid", "hemisphere", "crystal", "Hilbert rug" };
  
EX eShape sh;

hyperpoint unit_vector[3] = {point3(1,0,0), point3(0,1,0), point3(0,0,1)};

ld last_int_of = 0, last_int = 0;

EX ld dini_b = .15;

EX ld hyper_b = 1;

ld f(ld x) {
  return sqrt(1 - pow(hyper_b * sinh(x), 2));
  }

int kuen_branch(ld v, ld u);

ld integral(ld x) {
  if(x == 0) {
    last_int = last_int_of = 0;
    }
  else {
    last_int += (x - last_int) * f((x + last_int)/2);
    last_int_of = x;
    }
  return last_int;
  }

hyperpoint coord(hyperpoint h) {
  // return { cos(u)*sin(v), cos(u)*cos(v), sin(u) };
  
  /*
  ld t = h[0];
  ld v = h[1];
  ld r = 1 / cosh(t);
  ld x = t - tanh(t);
  return { x + v * .1, r * sin(v), r * cos(v) };
  */

  switch(sh) {
    case dsTractricoid: {
      ld t = h[0];
      ld v = h[1];
      ld r = 1 / cosh(t);
      ld x = t - tanh(t);

      return point31( r * sin(v), r * cos(v), x );
      break;
      }
    
    case dsDini: {
      ld t = h[0];
      ld v = h[1];
      
      ld a = sqrt(1-dini_b*dini_b);
      
      return point31( a * sin(v) * sin(t), a * cos(v) * sin(t),  a * (cos(t) + log(tan(t/2))) + dini_b * v );
      break;
      }
    
    case dsKuen: {
      ld v = h[0];
      ld u = h[1];
      
      ld deno = 1 / (1 + u * u * sin(v) * sin(v));
      
      return point31(
        2 * (cos(u) + u * sin(u)) * sin(v) * deno,
        2 * (sin(u) - u * cos(u)) * sin(v) * deno,
        log(tan(v/2)) + 2 * cos(v) * deno
        );
      }
    
    case dsHyperlike: {
      ld u = h[0];
      ld v = h[1];

      ld phi = hyper_b * cosh(v);
      ld psi = integral(v);
      
      return point31( phi * cos(u), phi * sin(u), psi );
      }
    
    default:
      return h;
    }
  }

ld det(hyperpoint h1, hyperpoint h2, hyperpoint h3) {
  return det(build_matrix(h1, h2, h3,C03));
  }

ld epsd = 1e-5;

hyperpoint coord_derivative(hyperpoint h, int cc) {
  switch(sh) {
    case dsHyperlike: {
      ld u = h[0];
      ld v = h[1];
      if(cc == 0) {
        ld phi = hyper_b * cosh(v);
        return point3( phi * -sin(u), phi * cos(u), 0 );
        }
      else {
        return point3( hyper_b * sinh(v) * cos(u), hyper_b * sinh(v) * sin(u), f(v) );
        }
      }
    case dsKuen: {
      ld v = h[0];
      ld u = h[1];      
      ld denom = pow(sin(v),2)*(u*u)+1;
      ld denom2 = denom * denom;
      if(cc == 1) 
        return point3(
          2*sin(v)/denom*u*cos(u)+-4*(sin(u)*u+cos(u))*pow(sin(v),3)/denom2*u,
          -4*pow(sin(v),3)*(sin(u)-u*cos(u))/denom2*u+2*sin(u)*sin(v)/denom*u,
          -4*pow(sin(v),2)/denom2*u*cos(v)
          );
      else return point3(
          2*(sin(u)*u+cos(u))/denom*cos(v)+-4*(sin(u)*u+cos(u))*pow(sin(v),2)/denom2*(u*u)*cos(v),
          2*(sin(u)-u*cos(u))/denom*cos(v)+-4*pow(sin(v),2)*(sin(u)-u*cos(u))/denom2*(u*u)*cos(v),
         -4*sin(v)/denom2*(u*u)*pow(cos(v),2)+1/tan(v/2)*(pow(tan(v/2),2)+1)/2+-2*sin(v)/denom
         );
      break;
      }
    default:
      // too lazy do differentiate
      return (coord(h + unit_vector[cc] * epsd) - coord(h)) / epsd;
    }
  }

ld compute_curvature(hyperpoint at) {
  hyperpoint xu = coord_derivative(at, 0);
  hyperpoint xv = coord_derivative(at, 1);
  hyperpoint xuu = (coord_derivative(at + unit_vector[0] * epsd, 0) - xu) / epsd;
  hyperpoint xuv = (coord_derivative(at + unit_vector[1] * epsd, 0) - xu) / epsd;
  hyperpoint xvv = (coord_derivative(at + unit_vector[1] * epsd, 1) - xv) / epsd;
  return 
    (det(xuu, xu, xv) * det(xvv, xu, xv) - pow(det(xuv, xu, xv), 2)) /
    pow((xu|xu) * (xv|xv) - pow((xu|xv), 2), 2);
  }

hyperpoint shape_origin() {
  switch(sh) {
    case dsDini:
      return point31(A_PI * .82, 0, 0);
    case dsTractricoid:
      return point31(1, 0, 0);
    case dsKuen:
      return point31(90._deg * 1.000001, M_PI, 0);
    case dsHyperlike:
      return point31(0,0,0);
    default:
      return Hypc;
    }
  }

ld hyperlike_bound() { return asinh(1 / hyper_b); }

bool flag_clamp_min(ld& coord, ld minv) {
  if(coord < minv) { coord = minv; return false; }
  return true;
  }

bool flag_clamp_max(ld& coord, ld maxv) {
  if(coord > maxv) { coord = maxv; return false; }
  return true;
  }

bool flag_clamp(ld& coord, ld minv, ld maxv) {
  return flag_clamp_min(coord, minv) && flag_clamp_max(coord, maxv);
  }

bool flag_clamp_sym(ld& coord, ld v) {
  return flag_clamp(coord, -v, v);
  }

int surface_branch(hyperpoint p) {
  if(sh == dsKuen) return kuen_branch(p[0], p[1]);
  return 0;
  }

bool inbound(ld& x, ld& y) {
  switch(sh) {
    case dsDini:
      return flag_clamp(x, 90._deg, M_PI);
    
    case dsTractricoid:
      return flag_clamp_min(x, 0) && flag_clamp_sym(y, M_PI);
    
    case dsKuen: 
      return flag_clamp(x, 0, M_PI) && flag_clamp(y, 0, TAU);
    
    case dsHyperlike:
      return flag_clamp_sym(x, M_PI) && flag_clamp_sym(y, hyperlike_bound());
    
    default: 
      return true;
    }
  }

bool is_inbound(hyperpoint h) {
  return inbound(h[0], h[1]);
  }

int precision = 100;

using rug::dexp_data;

struct dexp_origin {
  transmatrix H;     // isometry of H2 moving zero to C0
  transmatrix M;     // local coordinates on H2 to local coordinates on surface
  hyperpoint zero;   // parameters of the zero point
  };

dexp_data dexp(hyperpoint p, hyperpoint t) {
  ld eps = 1. / precision;
  int b = surface_branch(p);
  
  for(ld u=0; u<1; u += eps) {
    
    transmatrix T = build_matrix(coord_derivative(p, 0), coord_derivative(p, 1), Hypc, C03);
    
    p += t * eps;

    if(!is_inbound(p) || surface_branch(p) != b)
      return { p - t * eps, t, hypot_d(3, t) * (1-u) / precision };
      
    auto v0 = coord_derivative(p, 0);
    auto v1 = coord_derivative(p, 1);

    transmatrix T2 = build_matrix(v0, v1, v0 ^ v1, C03);
      
    t = inverse(T2) * T * t;
    t[2] = 0;
    }
  return { p, t, 0 };
  }

dexp_data map_to_surface(hyperpoint p, const dexp_origin& dor) {
  hyperpoint h = dor.H * p;
  ld rad = hypot_d(2, h);
  if(rad == 0) rad = 1;
  ld d = hdist0(h);

  hyperpoint direction;
  direction[0] = d * h[0] / rad;
  direction[1] = d * h[1] / rad;
  direction[2] = 0;
  #if MAXMDIM == 4
  direction[3] = 0;
  #endif

  return dexp(dor.zero, dor.M * direction);
  }

transmatrix create_M_matrix(hyperpoint zero, hyperpoint v1) {
  hyperpoint Te0 = coord_derivative(zero, 0);
  hyperpoint Te1 = coord_derivative(zero, 1);

  transmatrix T = build_matrix(Te0, Te1, Hypc, C03);

  v1 = v1 / hypot_d(3, T*v1);
  hyperpoint v2 = point3(1e-3, 1e-4, 0);
  v2 = v2 - v1 * ((T*v1) | (T*v2)) / hypot_d(3, T*v1);
  v2 = v2 / hypot_d(3, T*v2);
  
  if((((T*v1) ^ (T*v2)) | ((T*unit_vector[0]) ^ (T*unit_vector[1]))) < 0)
    v2 = v2 * -1;
  
  transmatrix M = build_matrix(v1, v2, Hypc, C03);
  
  println(hlog, M);

  println(hlog, "M matrix test: ", 
    make_tuple(hypot_d(3, T*M*unit_vector[0]), hypot_d(3, T*M*unit_vector[1]), hypot_d(3, T*M*(unit_vector[0]+unit_vector[1])),
    ((T*M*unit_vector[0]) | (T*M*unit_vector[1]))));
    
  return M;
  }

dexp_origin at_zero(hyperpoint zero, transmatrix start) {

  println(hlog, "zero = ", zero);

  println(hlog, "curvature at zero = ", compute_curvature(zero));
  println(hlog, "curvature at X1 = ", compute_curvature(zero + point3(.3, 0, 0)));
  println(hlog, "curvature at X2 = ", compute_curvature(zero + point3(0, .3, 0)));
  println(hlog, "curvature at X3 = ", compute_curvature(zero + point3(.4, .3, 0)));
  
  return {start, create_M_matrix(zero, unit_vector[0]), zero};
  }

dexp_origin at_other(dexp_origin& o1, hyperpoint h) {
  println(hlog, "\n\nmapping ", h, "...");
  println(hlog, o1.H, o1.M);
  auto dd = map_to_surface(h, o1);
  
  hyperpoint newzero = dd.params;
  println(hlog, "error = ", dd.remaining_distance);

  transmatrix Spin = spintox(o1.H * h);
  transmatrix T = pushxto0(Spin * o1.H * h) * Spin;
  
  println(hlog, "h is = ", h);
  println(hlog, "T*c0 is = ", T * C0);
  println(hlog, "T*h is = ", T * o1.H * h);
  
  return {T * o1.H, create_M_matrix(newzero, dd.cont), newzero};
  }

void addTriangleV(rug::rugpoint *t1, rug::rugpoint *t2, rug::rugpoint *t3, ld len = 1) {
  if(t1 && t2 && t3)
    rug::addTriangle(t1, t2, t3, len);
  }

hyperpoint kuen_cross(ld v, ld u) {
  auto du = coord_derivative(point3(v,u,0), 0);
  auto dv = coord_derivative(point3(v,u,0), 1);
  return du^dv;
  }

ld kuen_hypot(ld v, ld u) {
  auto du = coord_derivative(point3(v,u,0), 0);
  auto dv = coord_derivative(point3(v,u,0), 1);
  auto n = hypot_d(3, du^dv);
  return n;
  }

int kuen_branch(ld v, ld u) {
  if(v > 90._deg)
  return kuen_cross(v, u)[2] > 0 ? 1 : 2;
  else
  return kuen_cross(v, u)[2] < 0 ? 1 : 2;
  }

int dexp_colors[16] = { 
  0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00 };

int dexp_comb_colors[16] = { 
  0x000000, 0x0000FF, 0x00FF00, 0x00FFFF,
  0xFF0000, 0xFF00FF, 0xFFFF00, 0xFFFFFF,
  0xFFD500, 
  0x123456, 0x123456, 0x123456, 0x123456, 0x123456, 0x123456, 0x123456
   };

EX int coverage_style;
EX bool approx_quality;
EX vector<pair<hyperpoint, int> > coverage;

#if CAP_KUEN_MAP
void draw_kuen_map() {
  SDL_Surface *kuen_map = SDL_CreateRGBSurface(SDL_SWSURFACE,512,512,32,0,0,0,0);
  
  ld nmax = 0;

  for(int i=0; i<2; i++) {
    for(int r=0; r<512; r++)
    for(int h=0; h<512; h++) {
      ld v = M_PI * (r+.5) / 512;
      ld u = TAU * (h+.5) / 512;
      auto du = coord_derivative(point3(v,u,0), 0);
      auto dv = coord_derivative(point3(v,u,0), 1);
      auto n = hypot_d(3, du^dv);

      if(n > nmax) nmax = n;
      if(i == 1) {
        auto vv = kuen_cross(v, u);
        auto& px = qpixel(kuen_map, r, h);
        px |= 0xFF000000;
        for(int k=0; k<3; k++)
          part(px, k) = (vv[k] > 0 ? 0xFF : 0);
        px = 0xFF000000 + (((int)(n*255/nmax)) * (kuen_branch(v,u) == 1 ? 0x10101 : 0x10001));
        }
      }
    println(hlog, "nmax = ", nmax);
    }

  for(auto p: rug::points) {
    auto hp = p->surface_point.params;
    int x = int(512 * hp[0] / M_PI);
    int y = int(512 * hp[1] / TAU);
    qpixel(kuen_map, x, y) = 0xFF000000 | dexp_colors[p->dexp_id];
    }

  IMAGESAVE(kuen_map, "kuen.png");    
  }
#endif

EX void full_mesh() {
  rug::clear_model();
  rug::buildRug();
  rug::qvalid = 0;
  for(auto p: rug::points) p->valid = true, rug::qvalid++;
  while(rug::subdivide_further()) rug::subdivide();
  rug::sort_rug_points();
  for(auto p: rug::points) p->valid = false;
  rug::good_shape = true;
  }

char rchar(int id) {
  return 33 + id % 94;
  }
  
void run_hyperlike() {
  if(!rug::rugged) rug::reopen();
  rug::clear_model();
  
  int lim = (int) sqrt(rug::vertex_limit);
  for(int r=0; r<lim; r++)
  for(int h=0; h<lim; h++) 
    rug::addRugpoint(shiftless(xpush(TAU * hyper_b * (2*r-lim) / lim) * ypush(hyperlike_bound() * (2*h-lim) / lim) * C0), -1);
  for(int r=0; r<lim-1; r++)
  for(int h=0; h<lim-1; h++) {
    addTriangle(rug::points[lim*r+h], rug::points[lim*r+h+1], rug::points[lim*r+h+lim]);
    addTriangle(rug::points[lim*r+h+lim+1], rug::points[lim*r+h+lim], rug::points[lim*r+h+1]);
    }

  vector<ld> integral_table;
  for(int i=0; i<=precision; i++)
    integral_table.push_back(integral(hyperlike_bound() * i / precision));

  int id = 0;
  for(auto p: rug::points) {
    auto h = unshift(p->h);
    coverage.emplace_back(h, rchar(id++) + 7 * 256);
    
    ld y = asinh(h[1]);
    ld x = asinh(h[0] / cosh(y)) / hyper_b;
    p->surface_point.remaining_distance = !inbound(x, y);
    p->surface_point.params = hpxyz(x, y, 0);

    int sgn = y > 0 ? 1 : -1;
    ld phi = hyper_b * cosh(y);
    int pt = y * precision * sgn / hyperlike_bound();
    USING_NATIVE_GEOMETRY;
    p->native = point31(phi * cos(x), phi * sin(x), sgn * integral_table[pt]);
    p->valid = true;
    }
  }

void run_kuen() {
  full_mesh();

  auto H = Id;
  auto Hi = inverse(H);
  
  auto frontal_map = at_zero(hpxyz(90._deg * 1.000001, M_PI * 1, 0), Id);
  auto back0 = at_zero(hpxyz(90._deg * 1.000001, .67, 0), H);
  auto back1 = at_other(back0, Hi * spin(-90._deg) * hpxy(0.511, -0.5323));
  auto back2 = at_other(back0, Hi * spin(-90._deg) * hpxy(0.511, 0.5323));
  
  frontal_map.H = frontal_map.H * ypush(2.6);
  back0.H = back0.H * ypush(.4);
  back1.H = back1.H * ypush(.4);
  back2.H = back2.H * ypush(.4);
  
  int it = 0;
  for(auto p: rug::points) p->dexp_id = it++;
  
  vector<rug::rugpoint*> mesh = std::move(rug::points);
  vector<rug::triangle> old_triangles = std::move(rug::triangles);
  
  rug::clear_model();
  
  int part = 0;
  
  vector<int> coverages(isize(mesh), 0);
  
  for(auto& m: { frontal_map, back0, back1, back2 } ) {
    part++;
    int pid[5] = {0, 8, 1, 2, 4};
    string captions[5] = {"", "the upper component", "the lower center", "the lower left", "the lower right"};
    
    vector<rug::rugpoint*> newmesh(isize(mesh), nullptr);
    for(auto p: mesh) {
      auto px = map_to_surface(unshift(p->h), m);
      p->surface_point = px;
      history::progress(XLAT("solving the geodesics on: %1, %2/%3", XLAT(captions[part]), its(p->dexp_id), its(isize(mesh))));
      }
    for(auto p: mesh) {
      // make it a bit nicer by including the edges where only one endpoint is valid

      auto& px = p->surface_point;
      p->valid = px.remaining_distance == 0;
      for(auto e: p->edges) if(e.target->surface_point.remaining_distance == 0)
        p->valid = true;
      if(p->valid) {
        rug::rugpoint *np = new rug::rugpoint;
        newmesh[p->dexp_id] = np;
        rug::points.push_back(np);
        np->x1 = p->x1;
        np->y1 = p->y1;
        np->valid = true;
        np->inqueue = false;
        np->dist = 0;
        np->h = p->h;
        USING_NATIVE_GEOMETRY;
        np->native = coord(px.params);
        np->surface_point = px;
        np->dexp_id = p->dexp_id;
        coverages[p->dexp_id] |= pid[part];
        }
      }
    for(auto& t: old_triangles) {
      rug::rugpoint* r[3];
      for(int i=0; i<3; i++)
        r[i] = newmesh[t.m[i]->dexp_id];
      bool looks_good = true;
      for(int i=0; i<3; i++)
        if(!r[i]) looks_good = false;
      if(!looks_good) continue;
      for(int i=0; i<3; i++) {
        USING_NATIVE_GEOMETRY;
        if(hypot_d(3, r[i]->native - r[(i+1)%3]->native) > .2)
          looks_good = false;
        }
      if(looks_good)
        addTriangleV(r[0], r[1], r[2]);
      }
    }
  
  for(auto t: mesh) {
    int c = coverages[t->dexp_id];
    coverage.emplace_back(unshift(t->h), rchar(t->dexp_id) + 256 * c);
    }

  // delete the old mesh
  for(auto t: mesh) delete t;

#if CAP_KUEN_MAP
  draw_kuen_map();
#endif
  }

template<class T> void run_function(T f) {
  full_mesh();
  for(auto p: rug::points) {
    USING_NATIVE_GEOMETRY;
    p->native = f(unshift(p->h)),
    p->valid = true;
    }
  }

void run_other() {
  full_mesh();
  auto dp = at_zero(shape_origin(),  spin(90._deg));

  int it = 0;
  for(auto p: rug::points) {
    it++;
    auto h = unshift(p->h);

    p->surface_point = map_to_surface(h, dp);
    if(1) {
      USING_NATIVE_GEOMETRY;    
      p->native = coord(p->surface_point.params);
      }
    history::progress(XLAT("solving the geodesics on: %1, %2/%3", XLAT(shape_name[sh]), its(it), its(isize(rug::points))));
    if(p->surface_point.remaining_distance == 0)
      coverage.emplace_back(h, rchar(it) + 256 * 7);
    }
  
  clearMessages();

  for(auto p: rug::points) {
    // make it a bit nicer by including the edges where only one endpoint is valid
    p->valid = p->surface_point.remaining_distance == 0;
    if(sh != dsKuen) {
      for(auto e: p->edges) if(e.target->surface_point.remaining_distance == 0)
        p->valid = true;
      }
    }
  }

EX void run_shape(eShape s) {
  if(s == sh && rug::rugged) { rug::close(); return; }
  coverage.clear();
  need_mouseh = true;
  sh = s;
  
  if(rug::rugged) rug::close();
  if(!among(s, dsHypersian, dsTorus, dsCrystal)) rug::gwhere = rug::rgEuclid;

  rug::init();
  // if(!rug::rugged) rug::reopen();
  
  pushScreen(history::progress_screen);
  if(sh != dsHypersian) rug::good_shape = true;

  switch(sh) {
    case dsHypersian:
    case dsTorus:
    case dsCrystal:
      break;
      
    case dsKuen:
      run_kuen();
      break;
    
    case dsHyperlike:
      run_hyperlike();
      break;
    
    default:
      run_other();
      break;

    case dsHyperboloid:
      run_function([] (hyperpoint h) { return h; });
      break;

    case dsHemisphere:
      run_function([] (hyperpoint h) { h = h / h[2]; h[2] = sqrt(1 - sqhypot_d(2, h)); return h; });
      break;

    case dsHilbert:
      run_hilbert();
    }

  if(sh != dsHypersian) rug::good_shape = true;
  
  rug::qvalid = 0;
  
  popScreen();
  
  if(sh != dsHyperboloid && sh != dsHemisphere && sh != dsHypersian) {

    ld minx = 1e9, maxx = -1e9;
  
    for(auto p: rug::points) if(p->valid) {
      minx = min(p->native[2], minx);
      maxx = max(p->native[2], maxx);
      rug::qvalid++;
      }
    
    println(hlog, "minx = ", minx, " maxx = ", maxx);
  
    ld shift = -(minx + maxx) / 2;
    for(auto p: rug::points) if(p->valid)
      p->native[2] += shift;
    }
  }

void cancel_shape() {
  if(sh) {
    sh = dsHypersian;
    rug::good_shape = false;
    rug::qvalid = 0;
    for(auto p: rug::points) 
      p->valid = p->surface_point.remaining_distance == 0;
    for(auto p: rug::points) 
      if(p->valid) 
        rug::qvalid++, rug::enqueue(p);
    }
  }

cell *coverage_center;
transmatrix coverage_matrix;

// approximations

hyperpoint hilbert_at(hyperpoint s, hyperpoint e, hyperpoint d, hyperpoint as, hyperpoint ae, ld steps, ld t) {
  hyperpoint ctr = (e+d)/2;
  hyperpoint f = e+(d-s);
  if(steps == 0) return as * (1-t) * (1-t) + ae * t * t + ctr * (1 - t*t - (1-t) * (1-t));
  if(steps < 1) return lerp(hilbert_at(s, e, d, as, ae, 0, t), hilbert_at(s, e, d, as, ae, 1, t), steps);
  t *= 4;
  int ti = int(t);
  if(ti == 4) ti = 3;
  t -= ti;
  if(ti == 0) return hilbert_at(s, (s+d)/2, (s+e)/2, (s+as)/2, (s+d+e+d)/4, steps-1, t);
  if(ti == 1) return hilbert_at((s+d)/2, ctr, d, (s+d+e+d)/4, (e+d+d+f)/4, steps-1, t);
  if(ti == 2) return hilbert_at(ctr, (s+d)/2 + (e-s), d + (e-s)/2, (e+d+d+f)/4, (e+d+f+e)/4, steps-1, t);
  if(ti == 3) return hilbert_at((s+d)/2 + (e-s), e, ctr, (e+d+f+e)/4, (e+ae)/2, steps-1, t);
  throw hr_exception("hilbert_at error");
  }

ld approx_length_at(ld s) {
  if(s < 0.5) s -= (s - 0.5) / 2;
  return exp(0.5) * pow(2, s);;
  }

ld approx_length_to_s(ld len) {
  ld s = log(len / exp(0.5)) / log(2);
  if(s < 0.5) s += (s - 0.5);
  return s;
  }

bool circular = false;
int qty = 7;
ld hscale = 1;

EX void run_hilbert() {
  full_mesh();
  rug::good_shape = true;
  rug::qvalid = 0;

  for(auto p: rug::points) {
    hyperpoint& pt = p->native;
    hyperpoint h = p->h.h;

    if(circular) {
      ld r = hdist0(h);
      ld phi = atan2(h);

      ld length_factor = r == 0 ? 1 : sinh(r) / r;
      ld s = approx_length_to_s((approx_length_at(0.0) * length_factor));

      ld t = phi * qty / TAU;
      ld tz = floor(t);
      ld x0 = -1 + 2 * tz;
      ld x1 = +1 + 2 * tz;
      hyperpoint h = hilbert_at(point3(x0,-1,1), point3(x1,-1,1), point3(x0,1,1), point3(x0,0,1), point3(x1,0,1), s, t-tz);
      h *= TAU / (2 * qty);
      pt = cspin(0, 1, h[0]) * cspin(1, 2, h[1]) * point3(0, 1, 0) * r;
      }
    else {
      ld y = asin_auto(h[1]);
      ld x = asin_auto_clamp(h[0] / cos_auto(y));

      ld length_factor = cos_auto(y);
      ld s = approx_length_to_s((approx_length_at(0.5) * length_factor));

      ld t = x / approx_length_at(0.5) / hscale + .5;
      ld tz = floor(t);
      ld x0 = -1 + 2 * tz;
      ld x1 = +1 + 2 * tz;
      pt = hilbert_at(point3(x0,-1,1), point3(x1,-1,1), point3(x0,1,1), point3(x0,0,1), point3(x1,0,1), s, t-tz);

      pt[0] *= hscale;
      pt[1] *= hscale;
      pt[2] = y;
      }

    pt *= rug::modelscale;

    pt[3] = 1;

    p->valid = true;
    }
  }

// menu

EX void shape_option(eShape s, char c, bool named IS(true)) {
  bool b = rug::rugged || coverage_style || approx_quality;
  dialog::addBoolItem(named ? XLAT(shape_name[s]) : XLAT("enable"), b && sh == s, c);
  dialog::add_action([s] { run_shape(s); });
  }

EX void show_smooth() {
  rug::init_rug_page(XLAT("constant curvature surfaces"));

  shape_option(dsTractricoid, 't');

  shape_option(dsHyperlike, 'b');
  dialog::addSelItem("     " + XLAT("parameter"), fts(hyper_b), 'B');
  dialog::add_action([] {
    dialog::editNumber(hyper_b, -1, 1, .05, 1, XLAT("parameter"),
      XLAT("Controls the inner radius.")
      );
    dialog::get_ne().reaction = [] () {
      if(sh == dsHyperlike) run_shape(sh);
      };
    });

  shape_option(dsDini, 'd');
  dialog::addSelItem("     " + XLAT("parameter") + " ", fts(dini_b), 'D');
  dialog::add_action([] {
    dialog::editNumber(dini_b, -1, 1, .05, .15, XLAT("parameter"),
      XLAT("The larger the number, the more twisted it is.")
      );
    });

  shape_option(dsKuen, 'k');

  dialog::addSelItem(XLAT("precision"), its(precision), 'p');
  dialog::add_action([] {
    dialog::editNumber(precision, 1, 10000, 0, 100, XLAT("precision"),
      XLAT("Computing these models involves integrals and differential equations, which are currently solved numerically. This controls the precision.")
      );
    dialog::get_ne().step = .1;
    dialog::scaleLog();
    });

  string cstyles[4] = { "OFF", "on surface", "on H² (static)", "on H² (dynamic)" };
  if((rug::rugged && sh && sh != dsHyperboloid && sh != dsHemisphere) || coverage_style) {
    dialog::addSelItem(XLAT("display coverage"), cstyles[coverage_style], 'c');
    dialog::add_action([] {
      coverage_style = (1 + coverage_style) % 4;
      if(coverage_style == 0) {
        rug::reopen();
        }
      if(coverage_style == 2) {
        if(rug::rugged) rug::close();
        }
      coverage_matrix = inverse(unshift(ggmatrix(coverage_center = cwt.at)));
      });
    }
  else dialog::addBreak(100);

  dialog::addHelp();
  dialog::add_action([] {
    gotoHelp(XLAT(
      "In this menu you can choose from several known smooth surfaces of constant negative curvature. "
      "Since the curvature of hyperbolic plane is also constant negative, these surfaces "
      "are great to draw hyperbolic tesselations on. While they look great, only a small part "
      "of the hyperbolic plane can be visibly represented in this way, so they are not "
      "good for playing HyperRogue; however, the coverage extends far away in specific directions, "
      "so first increasing sight range in graphics config and texture size in hypersian rug settings may improve the effect."
      ));
    });

  rug::close_rug_page();
  }

EX void display_approx() {
  dialog::addBoolItem_action(XLAT("approximation quality"), approx_quality, 'A');
  dialog::addCustom(50, [] {
    if(approx_quality) {
      int mid = (dialog::top + dialog::tothei) / 2;
      int siz = vid.fsize / 2;
      for(int s=0; s<=10; s++) {
        ld r = exp(lerp(log(v_approx_min), log(v_approx_max), s/10.));
        color_t col = rainbow_color(1, s / 10.);
        displaystr(dialog::dcenter + (dialog::dialogwidth - 3 * vid.fsize) * lerp(-0.5, 0.5, s/10.), mid, 0, siz, format("%.3f", r), col, 8);
        }
      }
    });
  if(approx_quality) {
    dialog::addSelItem(XLAT("set scale to"), format("%lf..%lf", approx_min, approx_max), 'S');
    dialog::add_action([] { v_approx_max = approx_max; v_approx_min = approx_min; });
    }
  else dialog::addBreak(100);
  }

EX void show_hilbert() {
  rug::init_rug_page(XLAT("Hilbert Rug"));

  shape_option(dsHilbert, 'u', false);

  dialog::addBoolItem(XLAT("circular mode"), circular, 'C');
  dialog::add_action([] { circular = !circular; if(sh == dsHilbert) run_shape(dsHilbert); });

  if(circular) {
    dialog::addSelItem(XLAT("scale"), its(qty), 'Q');
    dialog::add_action([] {
      dialog::editNumber(qty, 1, 100, 1, 7, XLAT("scale"),
        XLAT("The smaller the number, the flatter it is.")
        );
      dialog::get_ne().reaction = [] { if(sh == dsHilbert) run_shape(dsHilbert); };
      });
    }
  else {
    dialog::addSelItem(XLAT("scale"), fts(hscale), 'S');
    dialog::add_action([] {
      dialog::editNumber(hscale, 1e-3, 1e3, .05, 1, XLAT("scale"),
        XLAT("The smaller the number, the flatter it is.")
        );
      dialog::get_ne().reaction = [] { if(sh == dsHilbert) run_shape(dsHilbert); };
      });
    }

  rug::close_rug_page();
  }

string finger_help =
  "Press 1 while mouse is over the surface to enable the finger force. In this mode, "
  "pressing arrow keys affects the shape of the surface (simulated pulling).";

EX void show_other() {
  rug::init_rug_page(XLAT("other features"));

  dialog::addTitle(XLAT("other 3D models"), iinf[itPalace].color, 150);

  shape_option(dsHyperboloid, 'h');
  shape_option(dsHemisphere, 's');

  dialog::addBreak(100);

  #if CAP_FILES
  if(rug::rugged)
    dialog::addItem(XLAT("save the current embedding"), 's');
  else
    dialog::addItem(XLAT("load a saved embedding"), 's');
  dialog::add_action([] {
    static string rugname = "saved.rug";
    if(rug::rugged)
      dialog::openFileDialog(rugname, XLAT("save embedding to:"), ".rug", [] () { rug::rug_save(rugname); return true; });
    else
      dialog::openFileDialog(rugname, XLAT("load embedding from:"), ".rug", [] () { rug::init(); rug::rug_load(rugname); return true; });
    });
  #endif

  if(rug::rugged) {
    dialog::addSelItem(XLAT("radar"), rug::radar_distance == rug::RADAR_INF ? "∞" : fts(rug::radar_distance, 4), 'r');
    dialog::add_action([] {
      addMessage(XLAT("This just shows the 'z' coordinate of the selected point."));
      });
    }
  else dialog::addBreak(100);

  dialog::addItem(XLAT("surface_point to native"), 'x');
  dialog::add_action([] {
   for(auto p: rug::points)
     p->native = p->surface_point.params;
    });

  display_approx();

  dialog::add_key_action('1', rug::activate_finger);

  dialog::addSelItem(XLAT("finger range"), fts(rug::finger_range), 'R');
  dialog::add_action([] {
    dialog::editNumber(rug::finger_range, 0, 1, .01, .1, XLAT("finger range"),
      XLAT(finger_help)
      );
    });
  dialog::addSelItem(XLAT("finger force"), fts(rug::finger_force), 'F');
  dialog::add_action([] {
    dialog::editNumber(rug::finger_force, 0, 1, .01, .1, XLAT("finger force"),
      XLAT(finger_help)
      );
    });

  rug::close_rug_page();
  }

#if CAP_COMMANDLINE
int surface_args() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-kuen")) {
    PHASE(3);
    calcparam();
    run_shape(dsKuen);
    }

  else if(argis("-dini")) {
    PHASE(3);
    calcparam();
    shift();
    dini_b = argf();
    run_shape(dsDini);
    }

  else if(argis("-barrel")) {
    PHASE(3);
    calcparam();
    shift();
    hyper_b = argf();
    run_shape(dsHyperlike);
    }

  else if(argis("-tractricoid")) {
    PHASE(3);
    calcparam();
    run_shape(dsTractricoid);
    }

  else if(argis("-hemi")) {
    PHASE(3);
    calcparam();
    run_shape(dsHemisphere);
    }

  else if(argis("-hyperb")) {
    PHASE(3);
    calcparam();
    run_shape(dsHyperboloid);
    }

  else if(argis("-d:surface")) {
    launch_dialog(rug::show);
    launch_dialog(show_smooth);
    }

  else return 1;
  return 0;
  }
#endif

#if CAP_COMMANDLINE
auto surface_hook = addHook(hooks_args, 100, surface_args);
#endif

EX ld approx_min;
EX ld approx_max;
EX ld v_approx_min = 0.5;
EX ld v_approx_max = 1.5;

void display_coverage_and_approx_quality() {

  shiftmatrix M = 
    coverage_style == 3 ? ggmatrix(coverage_center) * coverage_matrix
    : shiftless(Id);

  if(coverage_style)
    for(auto p : coverage)
      queuestr(M * p.first, 10, s0+char(p.second), dexp_comb_colors[(p.second >> 8) & 15]);
/*      if(p->valid && p->surface_point.remaining_distance == 0)
        queuestr(p->h, 10, "x", dexp_colors[p->dexp_id]); */

  if(approx_quality) {
    vid.linewidth *= 4;
    approx_min = INF, approx_max = 0;
    for(auto p: rug::points) {
      for(auto& e: p->edges) {
        if(e.os > e.ot) continue;
        if(!e.os->valid || !e.ot->valid) continue;
        ld d;
        if(1) {
          USING_NATIVE_GEOMETRY;
          d = hdist(e.ot->native, e.os->native);
          }
        ld ratio = d / e.len;
        if(ratio < approx_min) approx_min = ratio;
        if(ratio > approx_max) approx_max = ratio;
        ld z = log(ratio) / log(v_approx_max / v_approx_min);
        color_t col = (rainbow_color(1, z) << 8) | 0xFF;
        queueline(e.ot->h, e.os->h, col, 0);
        }
      }
    vid.linewidth /= 4;
    }
  }

auto surface_hook2 = addHook(hooks_frame, 0, display_coverage_and_approx_quality);

EX }
#endif
}
