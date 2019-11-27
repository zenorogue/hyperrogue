// Hyperbolic Rogue - Hypersian Rug mode
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

/** \file rug.cpp
 *  \brief Hypersian Rug mode
 *
 *  See also surface.cpp for constant curvature surfaces.
 */

#include "hyper.h"
namespace hr {

#if CAP_RUG

#define TEXTURESIZE (texturesize)
#define HTEXTURESIZE (texturesize/2)

EX bool rug_failure = false;

EX namespace rug {

EX ld lwidth = 2;

EX bool in_crystal() { return surface::sh == surface::dsCrystal; }

bool computed = false;

#if HDR
  struct edge {
    struct rugpoint *target;
    ld len;
    };
  
  struct dexp_data {
    hyperpoint params;
    hyperpoint cont;
    ld remaining_distance;
    };

  struct rugpoint {
    double x1, y1;
    bool valid;
    bool inqueue;
    double dist;
    hyperpoint h;    // point in the represented space
    hyperpoint flat; // point in the native space, in azeq
    hyperpoint precompute;
    vector<edge> edges;
    vector<edge> anticusp_edges;
    // Find-Union algorithm
    rugpoint *glue;
    rugpoint *getglue() {
      return glue ? (glue = glue->getglue()) : this;
      }
    hyperpoint& glueflat() {
      return glue->flat;
      }
    rugpoint() { glue = NULL; }
    void glueto(rugpoint *x) {
      x = x->getglue();
      auto y = getglue();
      if(x != y) y->glue = x;
      }
    int dexp_id;
    dexp_data surface_point;
    };

  struct triangle {
    rugpoint *m[3];
    triangle(rugpoint *m1, rugpoint *m2, rugpoint *m3) {
      m[0] = m1; m[1] = m2; m[2] = m3;
      }
    };
#endif

EX vector<rugpoint*> points;
EX vector<triangle> triangles;

int when_enabled;

struct rug_exception { };

EX bool fast_euclidean = true;
EX bool good_shape;
EX bool subdivide_first = false;
EX bool spatial_rug = false;

EX bool subdivide_further();
EX void subdivide();

EX ld modelscale = 1;
EX ld model_distance = 4;

EX eGeometry gwhere = gEuclid;

#define USING_NATIVE_GEOMETRY dynamicval<eGeometry> gw(geometry, gwhere == gElliptic ? gSphere : gwhere)

// hypersian rug datatypes and globals
//-------------------------------------

EX bool rugged = false;
bool genrug = false;

EX int vertex_limit = 20000;

EX bool renderonce  = false;
EX int renderlate  = 0;
EX bool rendernogl  = false;
EX int  texturesize = 1024;
EX ld scale = 1;
EX ld ruggo = 0;

EX ld anticusp_factor = 1;
EX ld anticusp_dist;

EX ld err_zero = 1e-3, err_zero_current, current_total_error;

EX int queueiter, qvalid, dt;

EX rugpoint *finger_center;
EX ld finger_range = .1;
EX ld finger_force = 1;

EX int rugdim;

EX bool rug_perspective = ISANDROID;

// extra geometry functions
//--------------------------

// returns a matrix M
// such that inverse(M) * h1 = ( |h1|, 0, 0) and inverse(M) * h2 = ( .., .., 0)

transmatrix orthonormalize(hyperpoint h1, hyperpoint h2) {

  hyperpoint vec[3] = {h1, h2, h1 ^ h2};
  
  for(int i=0; i<3; i++) {
    for(int j=0; j<i; j++) vec[i] -= vec[j] * (vec[i] | vec[j]);
    if(zero_d(3, vec[i])) {
      // 'random' direction
      vec[i] = hpxyz(1.12, 1.512+i, 1.12904+i);
      for(int j=0; j<i; j++) vec[i] -= vec[j] * (vec[i] | vec[j]);
      }
    vec[i] /= hypot_d(3, vec[i]);
    }

  transmatrix M;
  for(int i=0; i<3; i++) for(int j=0; j<3; j++)
    M[i][j] = vec[j][i];
  
  return M;
  }

hyperpoint azeq_to_hyperboloid(hyperpoint h) {
  if(abs(h[2])>1e-4) {
    println(hlog, "Error: h[2] = ", h[2]);
    rug_failure = true;
    }
  if(euclid) {
    h[2] = 1;
    return h;
    }
  ld d = hypot(h[0], h[1]);
  if(d == 0) {
    h[2] = 1;
    return h;
    }
  if(sphere) {
    ld d0 = d ? d : 1;
    h[0] = sin(d) * h[0]/d0;
    h[1] = sin(d) * h[1]/d0;
    h[2] = cos(d);
    }
  else {
    ld d0 = d ? d : 1;
    h[0] = sinh(d) * h[0]/d0;
    h[1] = sinh(d) * h[1]/d0;
    h[2] = cosh(d);
    }
  return h;
  }

hyperpoint hyperboloid_to_azeq(hyperpoint h) {
  if(euclid) {
    h[2] = 0;
    return h;
    }
  else {
    ld d = hdist0(h);
    if(d == 0) { h[2] = 0; return h; }
    ld d2 = hypot_d(2, h);
    if(d2 == 0) { h[2] = 0; return h; }
    h[0] = d * h[0] / d2;
    h[1] = d * h[1] / d2;
    h[2] = 0;
    return h;
    }
  }

struct normalizer {

  transmatrix M, Mi;
  
  dynamicval<eGeometry> gw;
 
  normalizer (hyperpoint h1, hyperpoint h2) : gw(geometry, gwhere == gElliptic ? gSphere : gwhere) {
    M = orthonormalize(h1, h2);
    Mi = inverse(M);
    }
  
  hyperpoint operator() (hyperpoint h) { return azeq_to_hyperboloid(Mi*h); }
  hyperpoint operator[] (hyperpoint h) { return M*hyperboloid_to_azeq(h); }
  };

void push_point(hyperpoint& h, int coord, ld val) {
  if(fast_euclidean && gwhere == gEuclid) 
    h[coord] += val;
  else if(!val) return;
  else {
    // if(zero_d(3, h)) { h[0] = 1e-9; h[1] = 1e-10; h[2] = 1e-11; }
    normalizer n(hpxyz(coord==0,coord==1,coord==2), h);
    hyperpoint f = n(h);
    h = n[xpush(val) * f];
    }
  }
  
EX void push_all_points(int coord, ld val) {
  if(!val) return;
  else for(int i=0; i<isize(points); i++)
    push_point(points[i]->flat, coord, val);
  }

// construct the graph
//---------------------

int hyprand;

EX rugpoint *addRugpoint(hyperpoint h, double dist) {
  rugpoint *m = new rugpoint;
  m->h = h;
  
  /*
  ld tz = vid.alpha+h[2];
  m->x1 = (1 + h[0] / tz) / 2;
  m->y1 = (1 + h[1] / tz) / 2;
  */

  hyperpoint onscreen;
  applymodel(m->h, onscreen);
  m->x1 = (1 + onscreen[0] * vid.scale) / 2;
  m->y1 = (1 - onscreen[1] * vid.scale) / 2;
  m->valid = false;

  if(euclid && quotient && !bounded) {
    hyperpoint h1 = eumove(euclid3::ascoord(euclid3::T0[1])) * C0;
    h1 /= sqhypot_d(2, h1);
    if(nonorientable) h1 /= 2;
    m->valid = good_shape = true;
    ld d = h1[0] * h[1] - h1[1] * h[0]; 
    ld a = h[0] * h1[0] + h[1] * h1[1];

    // m->flat = modelscale * hpxyz(d * 2 * M_PI, sin(a * 2 * M_PI), cos(a * 2 * M_PI));

    USING_NATIVE_GEOMETRY;
    hyperpoint hpoint = ypush(modelscale) * xpush0(modelscale * d * 2 * M_PI);
    ld hpdist = hdist0(hpoint);
    ld z = hypot_d(2, hpoint);
    if(z==0) z = 1;
    hpoint = hpoint * hpdist / z;
    
    m->flat = hpxyz(hpoint[0], hpoint[1] * sin(a*2*M_PI), hpoint[1]*cos(a*2*M_PI));
    }
  else if(sphere) {
    m->valid = good_shape = true;
    ld scale;
    if(gwhere == gEuclid) {
      scale = modelscale;
      }
    else if(gwhere == gNormal) {
      // sinh(scale) = modelscale
      scale = asinh(modelscale);
      }
    else /* sphere/elliptic*/ {
      if(modelscale >= 1) 
        // do as good as we can...
        scale = M_PI / 2 - 1e-3, good_shape = false, m->valid = false;
      else scale = asin(modelscale);
      }
    m->flat = h * scale;
    }

  else if(euclid && gwhere == gEuclid) {
    m->flat = h * modelscale;
    m->valid = good_shape = true;
    }
  
  else if(gwhere == gNormal && (euclid || (hyperbolic && modelscale >= 1))) {
    m->valid = good_shape = true;

    ld d = hdist0(h);
    ld d0 = hypot_d(2, h); if(!d0) d0 = 1;
    
    hyperpoint hpoint;
    bool orig_euclid = euclid;
    USING_NATIVE_GEOMETRY;
    
    if(orig_euclid) {
      d *= modelscale;
      // point on a horocycle going through C0, in distance d along the horocycle
      hpoint = hpxy(d*d/2, d);
      }
    else {
      // radius of the equidistant
      ld r = acosh(modelscale);
      // point on an equdistant going through C0 in distance d along the guiding line
      // hpoint = hpxy(cosh(r) * sinh(r) * (cosh(d) - 1), sinh(d) * cosh(r));
      hpoint = xpush(r) * ypush(d) * xpush0(-r);
      hpoint[0] = -hpoint[0];
      }

    ld hpdist = hdist0(hpoint);
    ld z = hypot_d(2, hpoint);
    if(z==0) z = 1;
    m->flat = hpxyz(hpdist * h[0]/d0 * hpoint[1] / z, hpdist * h[1]/d0 * hpoint[1] / z, -hpdist * hpoint[0] / z);    
    }
  
  else {
    m->flat = h;
    ld hd = h[LDIM];
    for(int d=GDIM; d<rugdim; d++)
      m->flat[d] = (hd - .99) * (rand() % 1000 - rand() % 1000) / 1000;
    }

  if(rug_perspective)
    push_point(m->flat, 2, -model_distance);
  
  // if(rug_perspective && gwhere == gEuclid) m->flat[2] -= 3;
  m->inqueue = false;
  m->dist = dist;
  points.push_back(m);
  return m;
  }

EX rugpoint *findRugpoint(hyperpoint h) {
  for(int i=0; i<isize(points); i++) 
    if(sqhypot_d(rugdim, points[i]->h - h) < 1e-5) return points[i];
  return NULL;
  }

EX rugpoint *findOrAddRugpoint(hyperpoint h, double dist) {
  rugpoint *r = findRugpoint(h);
  return r ? r : addRugpoint(h, dist);
  }

void addNewEdge(rugpoint *e1, rugpoint *e2, ld len = 1) {
  edge e; e.len = len;
  e.target = e2; e1->edges.push_back(e);
  e.target = e1; e2->edges.push_back(e);
  }

EX bool edge_exists(rugpoint *e1, rugpoint *e2) {
  for(auto& e: e1->edges)
    if(e.target == e2)
      return true;
  return false;
  }

void addEdge(rugpoint *e1, rugpoint *e2, ld len = 1) {
  if(!edge_exists(e1, e2))
    addNewEdge(e1, e2, len);
  }

void add_anticusp_edge(rugpoint *e1, rugpoint *e2, ld len = 1) {
  for(auto& e: e1->anticusp_edges)
    if(e.target == e2) return;
  edge e; e.len = len;
  e.target = e2; e1->anticusp_edges.push_back(e);
  e.target = e1; e2->anticusp_edges.push_back(e);
  }

EX void addTriangle(rugpoint *t1, rugpoint *t2, rugpoint *t3, ld len IS(1)) {
  addEdge(t1->getglue(), t2->getglue(), len); 
  addEdge(t2->getglue(), t3->getglue(), len); 
  addEdge(t3->getglue(), t1->getglue(), len);
  triangles.push_back(triangle(t1,t2,t3));
  }

map<pair<rugpoint*, rugpoint*>, rugpoint*> halves;

rugpoint* findhalf(rugpoint *r1, rugpoint *r2) {
  if(r1 > r2) swap(r1, r2);
  return halves[make_pair(r1,r2)];
  }

void addTriangle1(rugpoint *t1, rugpoint *t2, rugpoint *t3) {
  rugpoint *t12 = findhalf(t1, t2);
  rugpoint *t23 = findhalf(t2, t3);
  rugpoint *t31 = findhalf(t3, t1);
  addTriangle(t1,  t12, t31);
  addTriangle(t12, t2,  t23);
  addTriangle(t23, t3,  t31);
  addTriangle(t23, t31, t12);
  }

bool psort(rugpoint *a, rugpoint *b) {
  return hdist0(a->h) < hdist0(b->h);
  }

EX void sort_rug_points() {
  sort(points.begin(), points.end(), psort);
  }

void calcLengths() {
  for(auto p: points) 
    for(auto& edge: p->edges) 
      edge.len = hdist(p->h, edge.target->h) * modelscale;
  }

void calcparam_rug() {
  auto cd = current_display;

  cd->xtop = cd->ytop = 0;
  cd->xsize = cd->ysize = TEXTURESIZE;
  cd->xcenter = cd->ycenter = cd->scrsize = HTEXTURESIZE;
  
  cd->radius = cd->scrsize * vid.scale;
  }

EX void buildTorusRug() {

   /* todo
  calcparam_rug();
  models::configure();

  vector<toruspoint> tps(qty);
  
  pair<gp::loc, gp::loc> periods;
  
  // todo take the periods 
  
  ld factor = sqrt(ld(dsquare(periods.second)) / dsquare(periods.first));
  
  ld xfactor = 0, yfactor = 0;
  
  println(hlog, "factor = ", factor);
  if(factor <= 2.05) factor = 2.2;
  factor -= 1;
        
  // 22,1
  // 7,-17
  
  transmatrix z1 = matrix3(
    periods.first.first, periods.second.first, 0, 
    periods.first.second, periods.second.second, 0,
    0, 0, 1);
  // transmatrix z1 = {{{22,7,0}, {1,-17,0}, {0,0,1}}};
  transmatrix z2 = inverse(z1);
  
  if(gwhere == gSphere) {
    hyperpoint xh = z2 * hpxyz(1, 0, 0);
    hyperpoint yh = z2 * hpxyz(0, 1, 0);
    // hypot(xh[0], factor * xh[1]) == hypot(yh[0], factor * yh[1])
    // xh[0]*xh[0] - yh[0] * yh[0] = factor * factor * (yh[1] * yh[1] - (xh[1] * xh[1])
    
    ld factor2 = (xh[0]*xh[0] - yh[0] * yh[0]) / (yh[1] * yh[1] - xh[1] * xh[1]);
    ld factor = sqrt(factor2);
    xfactor = sqrt(1/(1+factor2));
    yfactor = xfactor * factor;
                                    
    ld xscale = hypot(xfactor * xh[0] * 2 * M_PI, yfactor * xh[1] * 2 * M_PI);
    ld yscale = hypot(xfactor * yh[0] * 2 * M_PI, yfactor * yh[1] * 2 * M_PI);
    println(hlog, "xh = ", xh);
    println(hlog, "yh = ", yh);
    println(hlog, "factor = ", make_tuple(xfactor, yfactor, factor));
    println(hlog, "scales = ", make_tuple(xscale, yscale));
    
    modelscale = xscale / cgi.crossf;
    }
  
  map<pair<int, int>, rugpoint*> glues;
  
  auto addToruspoint = [&] (ld x, ld y) {
    auto r = addRugpoint(C0, 0);
    hyperpoint onscreen;
    applymodel(tC0(eumove(x, y)), onscreen);
    // take point (1,0)
    // apply eumove(1,0)
    // multiply by current_display->radius (= HTEXTURESIZE * rugzoom)
    // add 1, divide by texturesize
    r->x1 = onscreen[0];
    r->y1 = onscreen[1];
    hyperpoint h1 = hpxyz(x, y, 0);
    hyperpoint h2 = z2 * h1;
    double alpha = -h2[0] * 2 * M_PI;
    double beta = -h2[1] * 2 * M_PI;
    // r->flat = {alpha, beta, 0}; 
    double sc = (factor+1)/4;
    
    if(gwhere == gSphere) {
      ld ax = alpha + 1.124651, bx = beta + 1.214893;
      ld x = xfactor * sin(ax), y = xfactor * cos(ax), z = yfactor * sin(bx), t = yfactor * cos(bx);
      ld d = acos(t) / sqrt(x*x+y*y+z*z);
      r->flat = r->h = hpxyz(x * d, y * d, z * d);
      }
    else 
      r->flat = r->h = hpxyz((factor+cos(alpha)) * cos(beta) * sc, (factor+cos(alpha)) * sin(beta) * sc, -sin(alpha) * sc);
    r->valid = true;
    
    static const int X = 100003; // a prime
    auto gluefun = [] (ld z) { return int(frac(z + .5/X) * X); };
    auto p = make_pair(gluefun(h2[0]), gluefun(h2[1]));
    auto& r2 = glues[p];
    if(r2) r->glueto(r2); else r2 = r;
    return r;
    };
    
  int rugmax = (int) sqrt(vertex_limit / qty);
  if(rugmax < 1) rugmax = 1;
  if(rugmax > 16) rugmax = 16;
  
  ld rmd = rugmax;

  for(int leaf=0; leaf<(klein ? 2 : 1); leaf++)
  for(int i=0; i<getqty(); i++) {
    int x, y;
    
    if(single) {
      x = tps[i].x;
      y = tps[i].y;
      }
    else {
      x = i % sdx;
      y = i / sdx;
      if(x > sdx/2) x -= sdx;
      if(y > sdy/2) y -= sdy;
      
      if(leaf) { 
        x += sdx;
        if(x > sdx) x -= 2 * sdx;
        }
      }
    
    rugpoint *rugarr[32][32];
    for(int yy=0; yy<=rugmax; yy++)
    for(int xx=0; xx<=rugmax; xx++)
      rugarr[yy][xx] = addToruspoint(x+xx/rmd, y+(yy-xx)/rmd);
    
    for(int yy=0; yy<rugmax; yy++)
    for(int xx=0; xx<rugmax; xx++)
      addTriangle(rugarr[yy][xx], rugarr[yy+1][xx], rugarr[yy+1][xx+1], modelscale/rugmax),
      addTriangle(rugarr[yy][xx+1], rugarr[yy][xx], rugarr[yy+1][xx+1], modelscale/rugmax);
    }
  
  double maxz = 0;
  
  for(auto p: points)
    maxz = max(maxz, max(abs(p->x1), abs(p->y1)));
  
  // maxz * rugzoom * current_display->radius == current_display->radius
  
  vid.scale = 1 / maxz;
  
  for(auto p: points)
    p->x1 = (current_display->xcenter + current_display->radius * vid.scale * p->x1)/ vid.xres,
    p->y1 = (current_display->ycenter - current_display->radius * vid.scale * p->y1)/ vid.yres;
  
  qvalid = 0;
  for(auto p: points) if(!p->glue) qvalid++;
  println(hlog, "qvalid = ", qvalid);

  if(rug_perspective)
    push_all_points(2, -model_distance);  
  */
  
  return;
  }

EX void verify() {
  vector<ld> ratios;
  for(auto m: points)
    for(auto& e: m->edges) {
      auto m2 = e.target;
      ld l = e.len;
      
      ld l0;
      if(fast_euclidean) l0 = sqhypot_d(rugdim, m->flat - m2->flat);
      else {      
        normalizer n(m->flat, m2->flat);
        hyperpoint h1 = n(m->flat);
        hyperpoint h2 = n(m2->flat);
        l0 = hdist(h1, h2);
        }

      ratios.push_back(l0 / l);
      }
    
  println(hlog, "Length verification:");
  sort(ratios.begin(), ratios.end());
  for(int i=0; i<isize(ratios); i += isize(ratios) / 10)
    println(hlog, ratios[i]);
  println(hlog);
  }

void comp(cell*& minimum, cell *next) {
  int nc = next->cpdist, mc = minimum->cpdist;
  if(tie(nc, next) < tie(mc, minimum))
    minimum = next;
  }

EX void buildRug() {

  need_mouseh = true;
  good_shape = false;
  if(euclid && bounded) {
    good_shape = true;
    buildTorusRug();
    return;
    }
  
  celllister cl(centerover ? centerover : cwt.at, get_sightrange(), vertex_limit, NULL);

  map<cell*, rugpoint *> vptr;
  
  for(int i=0; i<isize(cl.lst); i++)
    vptr[cl.lst[i]] = addRugpoint(ggmatrix(cl.lst[i])*C0, cl.dists[i]);

  for(auto& p: vptr) {
    cell *c = p.first;
    rugpoint *v = p.second;
    
    if(archimedean || (euclid && quotient)) {
      rugpoint *p[MAX_EDGE+1];
      for(int j=0; j<c->type; j++) p[j] = findOrAddRugpoint(ggmatrix(c) * get_corner_position(c, j), v->dist);
      for(int j=0; j<c->type; j++) addTriangle(v, p[j], p[(j+1) % c->type]);
      
      if((euclid && quotient) && nonorientable) {
        transmatrix T = ggmatrix(c) * eumove(euclid3::ascoord(euclid3::T0[1]));
        rugpoint *Tv = addRugpoint(T * C0, 0);
        for(int j=0; j<c->type; j++) p[j] = findOrAddRugpoint(T * get_corner_position(c, j), v->dist);
        for(int j=0; j<c->type; j++) addTriangle(Tv, p[j], p[(j+1) % c->type]);
        }
      }
    
    else for(int j=0; j<c->type; j++) try {
      cell *c2 = c->move(j);
      rugpoint *w = vptr.at(c2);
      // if(v<w) addEdge(v, w);
      
      cell *c3 = c->modmove(j+1);
      rugpoint *w2 = vptr.at(c3);
      
      if(a4) {
        cell *c4 = (cellwalker(c,j) + wstep - 1).cpeek();
        cell *cm = c; comp(cm, c); comp(cm, c2); comp(cm, c3); comp(cm, c4);
        if(cm == c || cm == c4)
          addTriangle(v, w, w2);
        }
      else if(v > w && v > w2)
        addTriangle(v, w, w2);
      }
    catch(out_of_range&) {}
    }

  println(hlog, "vertices = ", isize(points), " triangles= ", isize(triangles));

  if(subdivide_first) 
    for(int i=0; i<20 && subdivide_further(); i++)
      subdivide();
  
  sort_rug_points();
  
  calcLengths();  

  verify();
  
  for(auto p: points) if(p->valid) qvalid++;
  }

// rug physics

queue<rugpoint*> pqueue;

EX void enqueue(rugpoint *m) {
  if(m->inqueue) return;
  pqueue.push(m);
  m->inqueue = true;
  }

bool force_euclidean(rugpoint& m1, rugpoint& m2, double rd, bool is_anticusp = false, double d1=1, double d2=1) {
  if(!m1.valid || !m2.valid) return false;
  // double rd = hdist(m1.h, m2.h) * xd;
  double t = sqhypot_d(rugdim, m1.flat - m2.flat);
  if(is_anticusp && t > rd*rd) return false;
  t = sqrt(t);
  current_total_error += (t-rd) * (t-rd);
  bool nonzero = abs(t-rd) > err_zero_current;
  double force = (t - rd) / t / 2; // 20.0;
  for(int i=0; i<rugdim; i++) {
    double di = (m2.flat[i] - m1.flat[i]) * force;
    m1.flat[i] += di * d1;
    m2.flat[i] -= di * d2;
    if(nonzero && d2>0) enqueue(&m2);
    }  
  return nonzero;
  }

bool force(rugpoint& m1, rugpoint& m2, double rd, bool is_anticusp=false, double d1=1, double d2=1) {
  if(!m1.valid || !m2.valid) return false;
  if(gwhere == gEuclid && fast_euclidean) {
    return force_euclidean(m1, m2, rd, is_anticusp, d1, d2);
    }
  normalizer n(m1.flat, m2.flat);
  hyperpoint f1 = n(m1.flat);
  hyperpoint f2 = n(m2.flat);
  
  ld t = hdist(f1, f2);
  if(is_anticusp && t > rd) return false;
  current_total_error += (t-rd) * (t-rd);
  bool nonzero = abs(t-rd) > err_zero_current;
  double forcev = (t - rd) / 2; // 20.0;
  
  transmatrix T = gpushxto0(f1);
  transmatrix T1 = spintox(T * f2) * T;

  transmatrix iT1 = inverse(T1);
  
  for(int i=0; i<3; i++) if(std::isnan(m1.flat[i])) { 
    addMessage("Failed!");
    throw rug_exception();
    }

  f1 = iT1 * xpush0(d1*forcev);
  f2 = iT1 * xpush0(t-d2*forcev);

  m1.flat = n[f1];
  m2.flat = n[f2];
  
  if(nonzero && d2>0) enqueue(&m2);
  return nonzero;
  }

vector<pair<ld, rugpoint*> > preset_points;

EX void preset(rugpoint *m) {
  if(GDIM == 3) return;
  int q = 0;
  hyperpoint h;
  for(int i=0; i<3; i++) h[i] = 0;
  
  preset_points.clear();
  
  for(int j=0; j<isize(m->edges); j++)
  for(int k=0; k<j; k++) {
    rugpoint *a = m->edges[j].target;
    rugpoint *b = m->edges[k].target;
    if(!a->valid) continue;
    if(!b->valid) continue;
    double blen = -1;
    for(int j2=0; j2<isize(a->edges); j2++) 
      if(a->edges[j2].target == b) blen = a->edges[j2].len;
    if(blen <= 0) continue;
    for(int j2=0; j2<isize(a->edges); j2++) 
    for(int k2=0; k2<isize(b->edges); k2++) 
    if(a->edges[j2].target == b->edges[k2].target && a->edges[j2].target != m) {    
      rugpoint *c = a->edges[j2].target;
      if(!c->valid) continue;

      double a1 = m->edges[j].len/blen;
      double a2 = m->edges[k].len/blen;
      double c1 = a->edges[j2].len/blen;
      double c2 = b->edges[k2].len/blen;
      
      double cz = (c1*c1-c2*c2+1) / 2;
      double ch = sqrt(c1*c1 - cz*cz + 1e-10);

      double az = (a1*a1-a2*a2+1) / 2;
      double ah = sqrt(a1*a1 - az*az + 1e-10);
      
      // c->h = a->h + (b->h-a->h) * cz + ch * ort
      hyperpoint ort = (c->flat - a->flat - cz * (b->flat-a->flat)) / ch;

      // m->h = a->h + (b->h-a->h) * az - ah * ort
      hyperpoint res = a->flat + (b->flat-a->flat) * az - ah * ort;
      
      h += res;
      
      preset_points.emplace_back(hypot(blen * (ah+ch), blen * (az-cz)), c);
      q++;
      }
    }
    
  if(q>0) m->flat = h/q;
  // printf("preset (%d) -> %s\n", q, display(m->flat));
  if(std::isnan(m->flat[0]) || std::isnan(m->flat[1]) || std::isnan(m->flat[2]))
    throw rug_exception();
  }

ld sse(const hyperpoint& h) {
  ld sse = 0;
  for(auto& p: preset_points) {
    ld l = p.first;
    ld l0;
    if(fast_euclidean) 
      l0 = hypot_d(rugdim, h - p.second->flat);
    else {
      normalizer n(h, p.second->flat);
      hyperpoint h1 = n(h);
      hyperpoint h2 = n(p.second->flat);
      l0 = hdist(h1, h2);
      }
    sse += (l0-l) * (l0-l);
    }
  
  return sse;
  }

EX void optimize(rugpoint *m, bool do_preset) {

  if(do_preset) {
    preset(m);
    // int ed0 = isize(preset_points);
    for(auto& e: m->edges) if(e.target->valid) 
      preset_points.emplace_back(e.len, e.target);
    if(gwhere >= gSphere || GDIM == 3) {
      ld cur = sse(m->flat);
      for(int it=0; it<500; it++) {
        ld ex = exp(-it/60);
        again:
        hyperpoint last = m->flat;
        m->flat[(it/2)%rugdim] += ((it&1)?1:-1) * ex;
        ld now = sse(m->flat);
        if(now < cur) { cur = now; ex *= 1.2; goto again; }
        else m->flat = last;
        }
      }
    }
  for(int it=0; it<50; it++) 
    for(int j=0; j<isize(m->edges); j++)
      force(*m, *m->edges[j].target, m->edges[j].len, false, 1, 0);
  }

int divides = 0;
bool stop = false;

EX bool subdivide_further() {
  if(euclid && bounded) return false;
  if(GDIM == 3) return false;
  return isize(points) * 4 < vertex_limit;
  }

EX void subdivide() {
  int N = isize(points);
  // if(euclid && gwhere == gEuclid) return;
  if(!subdivide_further()) {
    if(euclid && !bounded && gwhere == gEuclid) {
      println(hlog, "Euclidean -- full precision");
      stop = true; 
      }
    else {
      err_zero_current /= 2;
      println(hlog, "increasing precision to ", err_zero_current);
      for(auto p: points) enqueue(p);
      }
    return; 
    }
  println(hlog, "subdivide ", make_pair(N, isize(triangles)));
  need_mouseh = true;  
  divides++;
  vector<triangle> otriangles = triangles;
  triangles.clear();
  
  halves.clear();

  // subdivide edges
  for(int i=0; i<N; i++) {
    rugpoint *m = points[i];
    for(int j=0; j<isize(m->edges); j++) {
      rugpoint *m2 = m->edges[j].target;
      if(m2 < m) continue;
      rugpoint *mm = addRugpoint(mid(m->h, m2->h), (m->dist+m2->dist)/2);
      halves[make_pair(m, m2)] = mm;
      if(!good_shape) {
        normalizer n(m->flat, m2->flat);
        hyperpoint h1 = n(m->flat);
        hyperpoint h2 = n(m2->flat);
        mm->flat = n[mid(h1, h2)];
        }
      mm->valid = m->valid && m2->valid; 
      if(mm->valid) qvalid++;
      mm->inqueue = false; enqueue(mm);
      }
     m->edges.clear();
     }
    
  for(int i=0; i<isize(otriangles); i++)
    addTriangle1(otriangles[i].m[0], otriangles[i].m[1], otriangles[i].m[2]);
    
  calcLengths();

  println(hlog, "result ", make_tuple(isize(points), isize(triangles)));
  }

EX ld slow_modeldist(const hyperpoint& h1, const hyperpoint& h2) {
  normalizer n(h1, h2);
  hyperpoint f1 = n(h1);
  hyperpoint f2 = n(h2);
  return hdist(f1, f2);
  }

typedef array<ld, 4> hyperpoint4;

hyperpoint4 azeq_to_4(const hyperpoint& h) {
  array<ld, 4> res;
  ld rad = hypot_d(3, h);
  res[3] = cos(rad);
  ld sr = sin(rad) / rad;
  for(int j=0; j<3; j++) res[j] = h[j] * sr;
  return res;
  }

EX ld modeldist(const hyperpoint& h1, const hyperpoint& h2) {
  if(gwhere == gSphere) {
    hyperpoint4 coord[2] = { azeq_to_4(h1), azeq_to_4(h2) };
    ld edist = 0;
    for(int j=0; j<4; j++) edist += sqr(coord[0][j] - coord[1][j]);
    return 2 * asin(sqrt(edist) / 2);
    }
    
  return slow_modeldist(h1, h2);
  }

typedef long long bincode;
const bincode sY = (1<<16);
const bincode sZ = sY * sY;
const bincode sT = sY * sY * sY;

bincode acd_bin(ld x) {
  return (int) floor(x / anticusp_dist + .5);
  }

bincode get_bincode(hyperpoint h) {
  switch(ginf[gwhere].cclass) {
    case gcEuclid: case gcSolNIH: case gcNil: case gcProduct: case gcSL2:
      return acd_bin(h[0]) + acd_bin(h[1]) * sY + acd_bin(h[2]) * sZ;
    case gcHyperbolic:
      return acd_bin(hypot_d(3, h));
    case gcSphere: {
      auto p = azeq_to_4(h);
      return acd_bin(p[0]) + acd_bin(p[1]) * sY + acd_bin(p[2]) * sZ + acd_bin(p[3]) * sT;
      }
    }
  return 0;
  }

void generate_deltas(vector<bincode>& target, int dim, bincode offset) {
  if(dim == 0) {
    if(offset > 0) target.push_back(offset);
    }
  else {
    generate_deltas(target, dim-1, offset * sY);
    generate_deltas(target, dim-1, offset * sY + 1);
    generate_deltas(target, dim-1, offset * sY - 1);
    }
  }

int detect_cusp_at(rugpoint *p, rugpoint *q) {
  if(hdist(p->h, q->h) * modelscale <= anticusp_dist)
    return 0;
  else if(modeldist(p->flat, q->flat) > anticusp_dist - err_zero_current)
    return 1;
  else {
    add_anticusp_edge(p, q);
    enqueue(p);
    enqueue(q);
    return 2;
    }
  }

int detect_cusps() {
  ld max_edge_length = 0;
  for(auto p: points) 
  for(auto e: p->edges)
    max_edge_length = max(max_edge_length, e.len);
  anticusp_dist = anticusp_factor * max_edge_length;
  
  array<int, 3> stats = {{0,0,0}};

  map<bincode, vector<rugpoint*> > code_to_point;
  for(auto p: points) if(p->valid)
    code_to_point[get_bincode(p->flat)].push_back(p);
  
  vector<bincode> deltas;
  generate_deltas(deltas, gwhere == gEuclid ? 3 : gwhere == gNormal ? 1 : 4, 0);
  
  for(auto b: code_to_point) {
    bincode at = b.first;
    for(auto p: b.second) 
    for(auto q: b.second) 
      if(p < q) stats[detect_cusp_at(p, q)]++;
    for(bincode bc: deltas)
      if(code_to_point.count(at + bc))
      for(auto p: b.second)
      for(auto q: code_to_point[at+bc])
        stats[detect_cusp_at(p, q)]++;
    }
  
  /* printf("testing\n");
  int stats2[3] = {0,0,0};
  for(auto p: points) if(p->valid)
  for(auto q: points) if(q->valid) if(p<q) {
    stats2[detect_cusp_at(p, q)]++;
    }

  printf("cusp stats: %d/%d/%d | %d/%d/%d\n", stats[0], stats[1], stats[2], stats2[0], stats2[1], stats2[2]); */

  println(hlog, "cusp stats: ", stats);
  return stats[2];
  }
  
EX void addNewPoints() {

  if(anticusp_factor && detect_cusps())
    return;

  if((euclid && quotient) || qvalid == isize(points)) {
    subdivide();
    return;
    }
  
  ld dist = hdist0(points[qvalid]->h) + .1e-6;
  
  int oqvalid = qvalid;

  for(int i=0; i<isize(points); i++) {
    rugpoint& m = *points[i];
    bool wasvalid = m.valid;
    m.valid = wasvalid || sphere || hdist0(m.h) <= dist;
    if(m.valid && !wasvalid) {
      qvalid++;
      need_mouseh = true;  
      
      if(!good_shape) optimize(&m, i > 7);

      enqueue(&m);
      }
    }
  if(qvalid != oqvalid) { println(hlog, "adding new points ", make_tuple(oqvalid, qvalid, isize(points), dist, dt, queueiter)); }
  }

EX void physics() {

  #if CAP_CRYSTAL
  if(in_crystal()) {
    crystal::build_rugdata();
    return;
    }
  #endif

  if(good_shape) return;

  auto t = SDL_GetTicks();
  
  current_total_error = 0;
  
  while(SDL_GetTicks() < t + 5 && !stop)
  for(int it=0; it<50 && !stop; it++)
    if(pqueue.empty()) addNewPoints();
    else {
      queueiter++;
      rugpoint *m = pqueue.front();
      pqueue.pop();
      m->inqueue = false;
      bool moved = false;
      
      for(auto& e: m->edges)
        moved = force(*m, *e.target, e.len) || moved;
      
      for(auto& e: m->anticusp_edges)
        moved = force(*m, *e.target, anticusp_dist, true) || moved;
      
      if(moved) enqueue(m), need_mouseh = true;
      }    

  }

// drawing the Rug
//-----------------

bool use_precompute;

void getco(rugpoint *m, hyperpoint& h, int &spherepoints) {
  h = use_precompute ? m->getglue()->precompute : m->getglue()->flat;
  if(rug_perspective && gwhere >= gSphere) {
    if(h[2] > 0) {
      ld rad = hypot_d(3, h);
      // turn M_PI to -M_PI
      // the only difference between sphere and elliptic is here:
      // in elliptic, we subtract PI from the distance
      ld rad_to = (gwhere == gSphere ? M_PI + M_PI : M_PI) - rad;
      ld r = -rad_to / rad;
      h *= r;
      spherepoints++;
      }
    }
  }

extern int besti;

#if CAP_ODS
/* these functions are for the ODS projection, used in VR videos */

bool project_ods(hyperpoint azeq, hyperpoint& h1, hyperpoint& h2, bool eye) {
  USING_NATIVE_GEOMETRY;
  
  ld d = hypot_d(3, azeq);
  ld sindbd = sin_auto(d)/d, cosd = cos_auto(d);
  
  return ods::project(hyperpoint(azeq[0] * sindbd, azeq[1] * sindbd, azeq[2] * sindbd, cosd), h1, h2, eye);
  
//  printf("%10.5lf %10.5lf %10.5lf ", azeq[0], azeq[1], azeq[2]);
//  printf(" => %10.5lf %10.5lf %10.5lf %10.5lf", x, y, z, t);
  
  
//  printf("\n");
  return true;
  }
#endif

vector<glhr::ct_vertex> ct_array;

vector<glhr::ct_vertex> cp_array;

void drawTriangle(triangle& t) {
  int num = t.m[2] ? 3 : 2;
  for(int i=0; i<num; i++) {
    if(!t.m[i]->valid) return;
    // if(t.m[i]->dist >= get_sightrange()+.51) return;
    }
  dt++;

#if CAP_ODS
  if(vid.stereo_mode == sODS) {
    hyperpoint pts[3];

    // not implemented
    if(num == 2) return; 

    for(int i=0; i<num; i++)
      pts[i] = t.m[i]->getglue()->flat;    
      
    hyperpoint hc = (pts[1] - pts[0]) ^ (pts[2] - pts[0]);  
    double hch = hypot_d(3, hc);
    
    ld col = (2 + hc[0]/hch) / 3;
    
    bool natsph = among(gwhere, gSphere, gElliptic);
    
    bool ok = true;
    array<hyperpoint, 6> h;
    for(int eye=0; eye<2; eye++) {
      if(true) {
        for(int i=0; i<3; i++)
          ok = ok && project_ods(pts[i], h[i], h[i+3], eye);
        if(!ok) return;
        for(int i=0; i<6; i++) {
          // let Delta be from 0 to 2PI
          if(h[i][2]<0) h[i][2] += 2 * M_PI;
          // Theta is from -PI/2 to PI/2. Let it be from 0 to PI
          h[i][1] += (eye?-1:1) * M_PI/2;
          }
        }
      else {
        for(int i=0; i<6; i++) 
          h[i][0] = -h[i][0],
          h[i][1] = -h[i][1],
          h[i][2] = 2*M_PI-h[i][2];
        }
      if(natsph) {
        if(raddif(h[4][0], h[0][0]) < raddif(h[1][0], h[0][0]))
          swap(h[1], h[4]);
        if(raddif(h[5][0], h[0][0]) < raddif(h[2][0], h[0][0]))
          swap(h[5], h[2]);
        }
      else {
        if(h[0][2] < 0) swap(h[0], h[3]);
        if(h[1][2] < 0) swap(h[1], h[4]);
        if(h[2][2] < 0) swap(h[2], h[5]);
        }
      if(abs(h[1][1] - h[0][1]) > M_PI/2) return;
      if(abs(h[2][1] - h[0][1]) > M_PI/2) return;
      cyclefix(h[1][0], h[0][0]);
      cyclefix(h[2][0], h[0][0]);
      cyclefix(h[4][0], h[3][0]);
      cyclefix(h[5][0], h[3][0]);
      for(int s: {0, 3}) {
        int fst = 0, lst = 0;
        if(h[s+1][0] < -M_PI || h[s+2][0] < -M_PI) lst++;
        if(h[s+1][0] > +M_PI || h[s+2][0] > +M_PI) fst--;
        for(int x=fst; x<=lst; x++) for(int i=0; i<3; i++) {
          ct_array.emplace_back(
            hpxyz(h[s+i][0] + 2*M_PI*x, h[s+i][1], h[s+i][2]),
            t.m[i]->x1, t.m[i]->y1,
            col);
          }
        if(!natsph) break;
        }
      }
    return;
    }
#endif  

  int spherepoints = 0;
  array<hyperpoint,3> h;
  for(int i=0; i<num; i++) getco(t.m[i], h[i], spherepoints);
  if(spherepoints == 1 || spherepoints == 2) return;
  
  ld col = 1;
  if(num == 3) {
    hyperpoint hc = (h[1] - h[0]) ^ (h[2] - h[0]);  
    double hch = hypot_d(3, hc);  
    col = (2 + hc[0]/hch) / 3;
    }
  
  for(int i=0; i<num; i++) 
    (num==3?ct_array:cp_array).emplace_back(h[i], t.m[i]->x1, t.m[i]->y1, col);
  }

EX struct renderbuffer *glbuf;

EX void prepareTexture() {
  ensure_glbuf();
  if(!glbuf) { rug::close(); return; }
  resetbuffer rb;
  
  dynamicval<eStereo> d(vid.stereo_mode, sOFF);
  dynamicval<ld> dl(levellines, 0);
  calcparam_rug();
  models::configure();
  
  glbuf->enable();
  glbuf->clear(0);

  ptds.clear();

  #if CAP_QUEUE
  draw_boundary(0);
  draw_boundary(1);
  
  draw_model_elements();
  #endif

  drawthemap();
  if(mousing && !renderonce) {
    for(int i=0; i<numplayers(); i++) if(multi::playerActive(i))
      queueline(tC0(ggmatrix(playerpos(i))), mouseh, 0xFF00FF, 8 + vid.linequality);
    }
  if(finger_center) {
    transmatrix V = rgpushxto0(finger_center->h);
    queuechr(V, 0.5, 'X', 0xFFFFFFFF, 2);
    for(int i=0; i<72; i++)
      queueline(V * xspinpush0(i*M_PI/32, finger_range), V * xspinpush0((i+1)*M_PI/32, finger_range), 0xFFFFFFFF, vid.linequality);
    }
  drawqueue();
  calcparam();
  rb.reset();
  }

double xview, yview;

EX bool no_fog;

EX ld lowrug = 1e-2;
EX ld hirug = 1e3;

EX GLuint alternate_texture;

EX bool invert_depth;

EX void drawRugScene() {
  glbuf->use_as_texture();
  if(alternate_texture)
    glBindTexture( GL_TEXTURE_2D, alternate_texture);

  if(backcolor == 0) 
    glClearColor(0.05f,0.05f,0.05f,1.0f);
  else
    glhr::colorClear(backcolor << 8 | 0xFF);
#ifdef GLES_ONLY
  glClearDepthf(invert_depth ? -1.0f : 1.0f);
#else
  glClearDepth(invert_depth ? -1.0f : 1.0f);
#endif
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glDisable(GL_BLEND);
  current_display->next_shader_flags = GF_LIGHTFOG | GF_TEXTURE;
  glhr::set_depthtest(true);
  glhr::set_depthwrite(true);
  glDepthFunc(invert_depth ? GL_GREATER : GL_LESS);
  
  for(int ed=current_display->stereo_active() && vid.stereo_mode != sODS ? -1 : 0; ed < 2; ed += 2) {
    use_precompute = false;
    ct_array.clear();
    cp_array.clear();
    if(ed == 1 && vid.stereo_mode == sAnaglyph)
      glClear(GL_DEPTH_BUFFER_BIT);
    
    dynamicval<eModel> p(pmodel, mdManual);
    current_display->set_all(ed);
    eyewidth_translate(ed);

    if(glhr::current_glprogram->uLevelLines != -1)
      glUniform1f(glhr::current_glprogram->uLevelLines, levellines);

    if(vid.stereo_mode == sODS) {
      glhr::projection_multiply(glhr::ortho(M_PI, M_PI, 100)); // 2*M_PI));
      }
    else if(rug_perspective || current_display->stereo_active()) {

      xview = current_display->tanfov;
      yview = current_display->tanfov * vid.yres / vid.xres;
      
      glhr::projection_multiply(glhr::frustum(xview, yview, lowrug, hirug));
      xview = -xview; yview = -yview;

      if(!rug_perspective) 
        glhr::projection_multiply(glhr::translate(0, 0, -model_distance));
      if(ed) {
        if(gwhere == gEuclid)
          glhr::projection_multiply(glhr::translate(vid.ipd*ed/2, 0, 0));
        else {
          use_precompute = true;
          for(auto p: points) {
            p->precompute = p->flat;
            push_point(p->precompute, 0, vid.ipd*ed/2);
            }
          }
        }
      }
    else {
      xview = current_display->tanfov * model_distance;
      yview = current_display->tanfov * model_distance * vid.yres / vid.xres;
      // glOrtho(-xview, xview, yview, -yview, -1000, 1000);

      glhr::projection_multiply(glhr::ortho(xview, yview, -1000));
      }
    glhr::color2(0xFFFFFFFF);
    
    glhr::fog_max(
      no_fog ? 1000 :
      gwhere == gSphere && rug_perspective ? 10 : 
      gwhere == gElliptic && rug_perspective ? 4 :
      100,
      darkena(backcolor, 0, 0xFF)
      );
    GLERR("fog_max");
      
    for(int t=0; t<isize(triangles); t++)
      drawTriangle(triangles[t]);
      
    glhr::id_modelview();
    
    if(isize(ct_array) > 0) {
      glhr::prepare(ct_array);
      glDrawArrays(GL_TRIANGLES, 0, isize(ct_array));
      }

    GLERR("rugz");
    if(isize(cp_array) > 0) {
      glhr::prepare(cp_array);
      glLineWidth(lwidth);
      glDrawArrays(GL_LINES, 0, isize(cp_array));
      }
    GLERR("rugt");

    current_display->set_mask(0);
    GLERR("afterrug");
    }

  glEnable(GL_BLEND);

  if(rug_failure) {
    rug::close();
    rug::clear_model();
    rug::init();
    }
  }

// organization
//--------------

EX transmatrix currentrot;

EX void close_glbuf() {
  delete glbuf;
  glbuf = nullptr;
  }

EX void ensure_glbuf() {
  if(glbuf) return;
  glbuf = new renderbuffer(TEXTURESIZE, TEXTURESIZE, vid.usingGL && !rendernogl);
  if(!glbuf->valid) {
    addMessage(XLAT("Failed to enable"));
    close_glbuf();
    return;
    }
  }

EX void reopen() {
  if(rugged) return;
  rugdim = 2 * GDIM - 1;
  when_enabled = 0;
  GLERR("before init");
  ensure_glbuf();
  if(!glbuf) { rugged = false; return; }
  rugged = true;
  if(renderonce) prepareTexture();
  if(!rugged) return;
  }

EX bool display_warning = true;

EX void init_model() {
  clear_model();
  genrug = true;
  drawthemap();
  genrug = false;
  
  qvalid = 0; dt = 0; queueiter = 0;
  err_zero_current = err_zero;
  
  #if CAP_CRYSTAL
  if(cryst && surface::sh == surface::dsNone) {
    surface::sh = surface::dsCrystal;
    crystal::init_rotation();
    good_shape = true;
    return;
    }
  #endif
  
  try {
    buildRug();
    while(good_shape && subdivide_further()) subdivide();
  
    currentrot = Id;
    
    bool valid = true;
    for(rugpoint *r: points)
      if(r->x1<0 || r->x1>1 || r->y1<0 || r->y1 > 1)
        valid = false;
    
    if(sphere && pmodel == mdDisk && vid.alpha > 1)
      valid = false;
    
    if(display_warning && !valid)
      gotoHelp(
        "Note: this mode is based on what you see on the screen -- but re-rendered in another way. "
        "If not everything is shown on the screen (e.g., too zoomed in), the results will be incorrect "
        "(though possibly interesting). "
        "Use a different projection to fix this."
        );
    }
  catch(rug_exception) {
    close();
    clear_model();
    }
  }

EX void init() {
  if(dual::state) return;
  reopen();
  if(rugged) init_model();
  }

EX void clear_model() {
  triangles.clear();
  for(int i=0; i<isize(points); i++) delete points[i];
  points.clear();
  pqueue = queue<rugpoint*> ();
  }
  
EX void close() {
  if(!rugged) return;
  rugged = false;
  close_glbuf();
  finger_center = NULL;
  }

int lastticks;

ld protractor = 0;

EX void apply_rotation(const transmatrix& t) {
  if(!rug_perspective) currentrot = t * currentrot;
  #if CAP_CRYSTAL
  if(in_crystal()) crystal::apply_rotation(t);
  else
  #endif
  for(auto p: points) p->flat = t * p->flat;
  }

EX void move_forward(ld distance) {
  if(rug_perspective) push_all_points(2, distance);
  else model_distance /= exp(distance);
  }

#define CAP_HOLDKEYS (CAP_SDL && !ISWEB)

EX bool handlekeys(int sym, int uni) {
  if(NUMBERKEY == '1') {
    ld bdist = 1e12;
    if(finger_center) 
      finger_center = NULL;
    else {
      for(auto p: points) {
        ld cdist = hdist(p->getglue()->h, mouseh);
        if(cdist < bdist)
          bdist = cdist, finger_center = p->getglue();
        }
      }
    if(renderonce) renderlate+=10;
    return true;
    }
  else if(NUMBERKEY == '2') {
    #if CAP_CRYSTAL
    if(in_crystal())
      crystal::switch_z_coordinate();
    else
    #endif
      apply_rotation(cspin(0, 2, M_PI));
    return true;
    }
  else if(NUMBERKEY == '3') {
    #if CAP_CRYSTAL
    if(in_crystal())
      crystal::flip_z();
    else
    #endif
      apply_rotation(cspin(0, 2, M_PI/2));
    return true;
    }
  #if CAP_CRYSTAL
  else if(sym == SDLK_HOME && in_crystal()) {
    crystal::next_home_orientation();
    return true;
    }
  #endif
#if !CAP_HOLDKEYS
  else if(sym == SDLK_PAGEUP || uni == '[') {
    move_forward(.1);
    return true;
    }
  else if(sym == SDLK_PAGEDOWN || uni == ']') {
    move_forward(-.1);
    return true;
    }
  else if(sym == SDLK_HOME)  { apply_rotation(cspin(0, 1, .1)); return true; }
  else if(sym == SDLK_END)   { apply_rotation(cspin(1, 0, .1)); return true; }
  else if(sym == SDLK_DOWN)  { apply_rotation(cspin(2, 1, .1)); return true; }
  else if(sym == SDLK_UP)    { apply_rotation(cspin(1, 2, .1)); return true; }
  else if(sym == SDLK_LEFT)  { apply_rotation(cspin(2, 0, .1)); return true; }
  else if(sym == SDLK_RIGHT) { apply_rotation(cspin(0, 2, .1)); return true; }
#endif
  else return false;
  }

EX void finger_on(int coord, ld val) {
  for(auto p: points) {
    ld d = hdist(finger_center->h, p->getglue()->h);
    push_point(p->flat, coord, val * finger_force * exp( - sqr(d / finger_range)));
    }
  enqueue(finger_center), good_shape = false;
  }

transmatrix last_orientation;

EX ld ruggospeed = 1;

EX void actDraw() {
  try {

  if(!renderonce) prepareTexture();
  else if(renderlate) {
    renderlate--;
    prepareTexture();
    }
  // do not display button
  else playerfound = true;
  current_display->set_viewport(0);
  physics();
  drawRugScene();
  
  #if CAP_ORIENTATION
  if(!when_enabled) ticks = when_enabled;
  if(ticks < when_enabled + 500)
    last_orientation = getOrientation();
  else {
    transmatrix next_orientation = getOrientation();
    apply_rotation(inverse(next_orientation) * last_orientation);
    last_orientation = next_orientation;
    }        
  #endif
  
  int qm = 0;
  double alpha = (ticks - lastticks) / 1000.0;
  lastticks = ticks;

  if(ruggo) move_forward(ruggo * alpha);

  #if CAP_HOLDKEYS
  Uint8 *keystate = SDL_GetKeyState(NULL);
  if(keystate[SDLK_LALT]) alpha /= 10;

  transmatrix t = Id;
  
  auto perform_finger = [=] () {
    if(keystate[SDLK_HOME]) finger_range /= exp(alpha);
    if(keystate[SDLK_END]) finger_range *= exp(alpha);
    if(keystate[SDLK_LEFT]) finger_on(0, -alpha);
    if(keystate[SDLK_RIGHT]) finger_on(0, alpha);
    if(keystate[SDLK_UP]) finger_on(1, alpha);
    if(keystate[SDLK_DOWN]) finger_on(1, -alpha);
    if(keystate[SDLK_PAGEDOWN]) finger_on(2, -alpha);
    if(keystate[SDLK_PAGEUP]) finger_on(2, +alpha);
    };

  if(cmode & sm::NUMBER) {
    }
  else if(rug_perspective) {
    
    ld strafex = 0, strafey = 0, push = 0;

    if(finger_center) 
      perform_finger();
    else {
      if(keystate[SDLK_HOME]) qm++, t = t * cspin(0, 1, alpha), protractor += alpha;
      if(keystate[SDLK_END]) qm++, t = t * cspin(1, 0, alpha), protractor -= alpha;
      if(!keystate[SDLK_LSHIFT]) {
        if(keystate[SDLK_DOWN]) qm++, t = t * cspin(2, 1, alpha), protractor += alpha;
        if(keystate[SDLK_UP]) qm++, t =  t * cspin(1, 2, alpha), protractor -= alpha;
        if(keystate[SDLK_LEFT]) qm++, t = t * cspin(2, 0, alpha), protractor += alpha;
        if(keystate[SDLK_RIGHT]) qm++, t =  t * cspin(0, 2, alpha), protractor -= alpha;
        }
      if(keystate[SDLK_PAGEDOWN]) push -= alpha;
      if(keystate[SDLK_PAGEUP]) push += alpha;
      
      if(keystate[SDLK_LSHIFT]) {    
        if(keystate[SDLK_LEFT]) strafex += alpha;
        if(keystate[SDLK_RIGHT]) strafex -= alpha;
        if(keystate[SDLK_UP]) strafey -= alpha;
        if(keystate[SDLK_DOWN]) strafey += alpha;
        }
      }

    if(qm) {
      if(keystate[SDLK_LCTRL]) 
        push_all_points(2, +model_distance);
      apply_rotation(t);
      if(keystate[SDLK_LCTRL]) 
        push_all_points(2, -model_distance);
      }
    
    model_distance -= push;
    push_all_points(2, push * ruggospeed);
    push_all_points(0, strafex * ruggospeed);
    push_all_points(1, strafey * ruggospeed);
    }
  else {
    if(finger_center)
      perform_finger();
    else {
      if(keystate[SDLK_HOME] && !in_crystal()) qm++, t = inverse(currentrot);
      if(keystate[SDLK_END]) {
        qm++;
        if(in_crystal()) t = t * cspin(0, 1, alpha);
        else t = currentrot * cspin(0, 1, alpha) * inverse(currentrot);
        }
      if(keystate[SDLK_DOWN]) qm++, t = t * cspin(1, 2, alpha);
      if(keystate[SDLK_UP]) qm++, t =  t * cspin(2, 1, alpha);
      if(keystate[SDLK_LEFT]) qm++, t = t * cspin(0, 2, alpha);
      if(keystate[SDLK_RIGHT]) qm++, t =  t * cspin(2, 0, alpha);
      if(keystate[SDLK_PAGEUP]) model_distance /= exp(alpha * ruggospeed);
      if(keystate[SDLK_PAGEDOWN]) model_distance *= exp(alpha * ruggospeed);
      }
  
    if(qm) {
      apply_rotation(t);
      }
    }
  #endif
    }
  catch(rug_exception) {
    rug::close();
    }
  }

int besti;

void getco_pers(rugpoint *r, hyperpoint& p, int& spherepoints, bool& error) {
  getco(r, p, spherepoints);
  if(rug_perspective) {
    if(p[2] >= 0)
      error = true;
    else {
      p[0] /= p[2];
      p[1] /= p[2];
      }
    }
  }

static const ld RADAR_INF = 1e12;
ld radar_distance = RADAR_INF;

EX hyperpoint gethyper(ld x, ld y) {
  double mx = (x - current_display->xcenter)/vid.xres * 2 * xview;
  double my = (current_display->ycenter - y)/vid.yres * 2 * yview;
  radar_distance = RADAR_INF;
  
  double rx1=0, ry1=0;
  
  bool found = false;
  
  for(int i=0; i<isize(triangles); i++) {
    auto r0 = triangles[i].m[0];
    auto r1 = triangles[i].m[1];
    auto r2 = triangles[i].m[2];
    if(!r2) continue;
    if(!r0->valid || !r1->valid || !r2->valid) continue;
    hyperpoint p0, p1, p2;
    bool error = false;
    int spherepoints = 0;
    getco_pers(r0, p0, spherepoints, error);
    getco_pers(r1, p1, spherepoints, error);
    getco_pers(r2, p2, spherepoints, error);
    if(error || spherepoints == 1 || spherepoints == 2) continue;
    double dx1 = p1[0] - p0[0];
    double dy1 = p1[1] - p0[1];
    double dx2 = p2[0] - p0[0];
    double dy2 = p2[1] - p0[1];
    double dxm = mx - p0[0];
    double dym = my - p0[1];
    // A (dx1,dy1) = (1,0)
    // B (dx2,dy2) = (0,1)
    double det = dx1*dy2 - dy1*dx2;
    double tx = dxm * dy2 - dym * dx2;
    double ty = -(dxm * dy1 - dym * dx1);
    tx /= det; ty /= det;
    if(tx >= 0 && ty >= 0 && tx+ty <= 1) {
      double rz1 = p0[2] * (1-tx-ty) + p1[2] * tx + p2[2] * ty;
      rz1 = -rz1; if(!rug_perspective) rz1 += model_distance;
      if(rz1 < radar_distance) {
        radar_distance = rz1;
        rx1 = r0->x1 + (r1->x1 - r0->x1) * tx + (r2->x1 - r0->x1) * ty;
        ry1 = r0->y1 + (r1->y1 - r0->y1) * tx + (r2->y1 - r0->y1) * ty;
        }
      found = true;
      }
    }
  
  if(!found) return Hypc;
  
  double px = rx1 * TEXTURESIZE, py = (1-ry1) * TEXTURESIZE;

  calcparam_rug();
  models::configure();
  hyperpoint h = hr::gethyper(px, py);
  calcparam();

  return h;
  }

EX string makehelp() {
  return
    XLAT(
    "In this mode, HyperRogue is played on a 3D model of a part of the hyperbolic plane, "
    "similar to one you get from the 'paper model creator' or by hyperbolic crocheting.\n\n")
    /*
    "This requires some OpenGL extensions and may crash or not work correctly -- enabling "
    "the 'render texture without OpenGL' options may be helpful in this case. Also the 'render once' option "
    "will make the rendering faster, but the surface will be rendered only once, so "
    "you won't be able to play a game on it.\n\n" */
    #if !ISMOBILE
  + XLAT("Use arrow keys to rotate, Page Up/Down to zoom.")
    + "\n\n" + 
    XLAT("In the perspective projection, you can use arrows to rotate the camera, Page Up/Down to go forward/backward, Shift+arrows to strafe, and Ctrl+arrows to rotate the model.")
    #endif
     ;
  }

EX string geometry_name(eGeometry g) {
  switch(g) {
    case gNormal: return XLAT("hyperbolic");
    case gEuclid: return XLAT("Euclidean");
    case gSphere: return XLAT("spherical");
    case gElliptic: return XLAT("elliptic");
    default: return XLAT("unknown");
    }
  }

void change_texturesize() {
  if(rugged) {
    close();
    reopen();
    }
  }

ld old_distance;

EX void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);
  dialog::init(XLAT("hypersian rug mode"), iinf[itPalace].color, 150, 100);
  
  dialog::addBoolItem(XLAT("enable the Hypersian Rug mode"), rug::rugged, 'u');
  
  dialog::addBoolItem(XLAT("render the texture only once"), (renderonce), 'o');
  #if CAP_SDL
  dialog::addBoolItem(XLAT("render texture without OpenGL"), (rendernogl), 'g');
  #else
  rendernogl = false;
  #endif
  dialog::addSelItem(XLAT("texture size"), its(texturesize)+"x"+its(texturesize), 's');

  dialog::addSelItem(XLAT("vertex limit"), its(vertex_limit), 'v');
  if(rug::rugged)
    dialog::lastItem().value += " (" + its(qvalid) + ")";
  
  dialog::addSelItem(XLAT("model distance"), fts(model_distance), 'd');
  dialog::addBoolItem(XLAT("projection"), rug_perspective, 'p');
  dialog::lastItem().value = XLAT(rug_perspective ? "perspective" : 
    gwhere == gEuclid ? "orthogonal" : "azimuthal equidistant");
  if(!rug::rugged)
    dialog::addSelItem(XLAT("native geometry"), geometry_name(gwhere), 'n');
  else
    dialog::addSelItem(XLAT("radar"), radar_distance == RADAR_INF ? "∞" : fts(radar_distance, 4), 'r');
  dialog::addSelItem(XLAT("model scale factor"), fts(modelscale), 'm');
  if(rug::rugged)
    dialog::addSelItem(XLAT("model iterations"), its(queueiter), 0);
  dialog::addItem(XLAT("stereo vision config"), 'f');
  // dialog::addSelItem(XLAT("protractor"), fts(protractor * 180 / M_PI) + "°", 'f');
  if(!good_shape) {
    dialog::addSelItem(XLAT("maximum error"), fts(err_zero), 'e');
    if(rug::rugged)
      dialog::lastItem().value += " (" + fts(err_zero_current) + ")";
    }
  dialog::addSelItem(XLAT("automatic move speed"), fts(ruggo), 'G');
  dialog::addSelItem(XLAT("anti-crossing"), fts(anticusp_factor), 'A');
  dialog::addBoolItem(XLAT("3D monsters/walls on the surface"), spatial_rug, 'S');
  dialog::add_action([] () { spatial_rug = !spatial_rug; });
  edit_levellines('L');

#if CAP_SURFACE  
  if(hyperbolic) {
    if(gwhere == gEuclid)
      dialog::addItem(XLAT("smooth surfaces"), 'c');
    else dialog::addBreak(100);
    }
#endif

  dialog::addBreak(50);
  dialog::addHelp();
  dialog::addBack();

  dialog::display();
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);

    if(uni == 'h' || uni == SDLK_F1) gotoHelp(makehelp());
    else if(uni == 'u') {
      if(rug::rugged) rug::close();
      else { 
        #if CAP_SURFACE
        surface::sh = surface::dsNone; 
        #endif
        rug::init(); 
        }
      }
    else if(uni == 'R')
      dialog::editNumber(finger_range, 0, 1, .01, .1, XLAT("finger range"),
        XLAT("Press 1 to enable the finger mode.")
        );
    else if(uni == 'F')
      dialog::editNumber(finger_force, 0, 1, .01, .1, XLAT("finger force"),
        XLAT("Press 1 to enable the finger force.")
        );
    else if(uni == 'o')
      renderonce = !renderonce;
    else if(uni == 'G') {
      dialog::editNumber(ruggo, -1, 1, .1, 0, XLAT("automatic move speed"),
        XLAT("Move automatically without pressing any keys.")
        );
      }
    else if(uni == 'A') {
      dialog::editNumber(anticusp_factor, 0, 1.5, .1, 0, XLAT("anti-crossing"),
        XLAT("The anti-crossing algorithm prevents the model from crossing itself, "
          "by preventing points which should not be close from being close. "
          "The bigger number, the more sensitive it is, but the embedding is slower. Set 0 to disable.")
        );
      }
    else if(uni == 'v') {
      dialog::editNumber(vertex_limit, 0, 50000, 500, 3000, ("vertex limit"), 
        XLAT("The more vertices, the more accurate the Hypersian Rug model is. "
        "However, a number too high might make the model slow to compute and render.")
        );
      dialog::reaction = [] () { err_zero_current = err_zero; };
      }
    else if(uni == 'r') 
      addMessage(XLAT("This just shows the 'z' coordinate of the selected point."));
    else if(uni == 'm') {
      dialog::editNumber(modelscale, 0.1, 10, rugged ? .01 : .1, 1, XLAT("model scale factor"), 
        XLAT("This is relevant when the native geometry is not Euclidean. "
        "For example, if the native geometry is spherical, and scale < 1, a 2d sphere will be rendered as a subsphere; "
        "if the native geometry is hyperbolic, and scale > 1, a hyperbolic plane will be rendered as an equidistant surface. ")
        );
      dialog::scaleLog();
      if(rug::rugged) {
        static bool adjust_points = true;
        static bool camera_center = false;
        static bool adjust_edges = true;
        static bool adjust_distance = true;
        static ld last;
        last = modelscale;
        dialog::extra_options = [] () {
          dialog::addBoolItem_action(XLAT("adjust points"), adjust_points, 'P');
          if(adjust_points)
            dialog::addBoolItem_action(XLAT("center on camera"), camera_center, 'C');
          else
            dialog::addBreak(100);
          dialog::addBoolItem_action(XLAT("adjust edges"), adjust_edges, 'E');
          dialog::addBoolItem_action(XLAT("adjust distance"), adjust_distance, 'D');
          };
        dialog::reaction = [] () {
          if(!last || !modelscale) return;
          if(!camera_center) push_all_points(2, model_distance);
          for(auto p:points) {
            if(adjust_edges) for(auto& e: p->edges) e.len *= modelscale / last;
            if(adjust_points) p->flat *= modelscale / last;
            enqueue(p);
            }
          if(adjust_distance) model_distance = model_distance * modelscale / last;
          last = modelscale;
          good_shape = false;
          if(!camera_center) push_all_points(2, -model_distance);
          };
        }
      }
    else if(uni == 'p') {
      rug_perspective = !rug_perspective;
      if(rugged) {
        if(rug_perspective) 
          push_all_points(2, -model_distance);
        else
          push_all_points(2, +model_distance);
        }
      }
    else if(uni == 'd') {
      dialog::editNumber(model_distance, -10, 10, .1, 1, XLAT("model distance"), 
        XLAT("In the perspective projection, this sets the distance from the camera to the center of the model. "
        "In the orthogonal projection this just controls the scale.")
        );
      old_distance = model_distance;
      dialog::reaction = [] () { 
        if(rug::rugged && rug_perspective) {
          push_all_points(2, old_distance - model_distance);
          }
        old_distance = model_distance;
        };
      }
    else if(uni == 'e') {
      dialog::editNumber(err_zero, 1e-9, 1, .1, 1e-3, XLAT("maximum error"), 
        XLAT("New points are added when the current error in the model is smaller than this value.")
        );
      dialog::scaleLog();
      dialog::reaction = [] () { err_zero_current = err_zero; };
      }
    else if(uni == 'f') 
      pushScreen(showStereo);
    else if(uni == 'n' && !rug::rugged)
      gwhere = eGeometry((gwhere+1) % 4);
    else if(uni == 'g' && !rug::rugged && CAP_SDL)
      rendernogl = !rendernogl;
    else if(uni == 's') {
      texturesize *= 2;
      if(texturesize == 8192) texturesize = 64;
      change_texturesize();
      }
#if CAP_SURFACE
    else if(uni == 'c') 
      pushScreen(surface::show_surfaces);
#endif
    else if(handlekeys(sym, uni)) ;
    else if(doexiton(sym, uni)) popScreen();
    };
  }

EX void select() {
  if(dual::state) return;
  pushScreen(rug::show);
  }

#if CAP_COMMANDLINE
int rugArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-rugmodelscale")) {
    shift_arg_formula(modelscale);
    }

  else if(argis("-ruggeo")) {
    shift(); gwhere = (eGeometry) argi();
    }

  else if(argis("-rugpers")) {
    rug_perspective = true;
    }

  else if(argis("-rugonce")) {
    renderonce = true;
    }

  else if(argis("-rugdist")) {
    shift_arg_formula(model_distance);
    }

  else if(argis("-ruglate")) {
    renderonce = true;
    renderlate += 10;
    }

  else if(argis("-rugmany")) {
    renderonce = false;
    }

  else if(argis("-ruglwidth")) {
    shift_arg_formula(lwidth);
    }

  else if(argis("-rugauto")) {
    shift_arg_formula(ruggo);
    }

  else if(argis("-rugorth")) {
    rug_perspective = false;
    }

  else if(argis("-rugerr")) {
    shift_arg_formula(err_zero);
    }

  else if(argis("-rugtsize")) {
    shift(); rug::texturesize = argi();
    change_texturesize();
    }

  else if(argis("-rugv")) {
    shift(); vertex_limit = argi();
    }

  else if(argis("-rugon")) {
    PHASE(3); 
    calcparam();
    rug::init();
    }

  else if(argis("-sdfoff")) {
    subdivide_first = false;
    }

  else if(argis("-sdfon")) {
    subdivide_first = true;
    }

  else if(argis("-anticusp")) {
    shift_arg_formula(anticusp_factor);
    }

  else if(argis("-d:rug")) 
    launch_dialog(show);

  else return 1;
  return 0;
  }

auto rug_hook = 
  addHook(hooks_args, 100, rugArgs);
#endif

}

#else

// fake for mobile
namespace rug {
    bool rugged = false;
    bool renderonce = false;
    bool rendernogl = true;
    int texturesize = 512;
    ld scale = 1.0f;
}

#endif
}
