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

EX bool mouse_control_rug = false;

EX transmatrix rugView;

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
    shiftpoint h;      // point in the represented space
    hyperpoint native; // point in the native space
    hyperpoint precompute;
    vector<edge> edges;
    vector<edge> anticusp_edges;
    // Find-Union algorithm
    rugpoint *glue;
    rugpoint *getglue() {
      return glue ? (glue = glue->getglue()) : this;
      }
    hyperpoint& gluenative() {
      return glue->native;
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
    triangle() { m[0] = m[1] = m[2] = nullptr; }
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

#if HDR
constexpr eGeometry rgHyperbolic = gSpace534;
constexpr eGeometry rgEuclid = gCubeTiling;
constexpr eGeometry rgSphere = gCell120;
constexpr eGeometry rgElliptic = gECell120;
#endif

EX eGeometry gwhere = rgEuclid;

#if HDR
#define USING_NATIVE_GEOMETRY_IN_RUG dynamicval<eGeometry> gw(geometry, rug::rugged ? hr::rug::gwhere : geometry)
#define USING_NATIVE_GEOMETRY dynamicval<eGeometry> gw(geometry, hr::rug::gwhere)
#endif

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

EX ld anticusp_factor = 1;
EX ld anticusp_dist;

EX ld err_zero = 1e-3, err_zero_current, current_total_error;

EX int queueiter, qvalid, dt;

EX rugpoint *finger_center;
EX ld finger_range = .1;
EX ld finger_force = 1;

#define rconf (vid.rug_config)

EX bool perspective() { return models::is_perspective(rconf.model); }

void push_point(hyperpoint& h, int coord, ld val) {
  USING_NATIVE_GEOMETRY;
  if(fast_euclidean && euclid) 
    h[coord] += val;
  else if(!val) return;
  else {
    h = cpush(coord, val) * h;
    }
  }
  
EX void push_all_points(int coord, ld val) {
  if(!val) return;
  else for(int i=0; i<isize(points); i++)
    push_point(points[i]->native, coord, val);
  }

// construct the graph
//---------------------

int hyprand;

bool rug_euclid() { USING_NATIVE_GEOMETRY; return euclid; }
bool rug_hyperbolic() { USING_NATIVE_GEOMETRY; return hyperbolic; }
bool rug_sphere() { USING_NATIVE_GEOMETRY; return sphere; }
bool rug_elliptic() { USING_NATIVE_GEOMETRY; return elliptic; }

EX map<cell*, rugpoint*> rug_map;

EX rugpoint *addRugpoint(shiftpoint h, double dist) {
  rugpoint *m = new rugpoint;
  m->h = h;
  
  /*
  ld tz = pconf.alpha+h[2];
  m->x1 = (1 + h[0] / tz) / 2;
  m->y1 = (1 + h[1] / tz) / 2;
  */

  hyperpoint onscreen;
  applymodel(m->h, onscreen);
  m->x1 = (1 + onscreen[0] * pconf.scale) / 2;
  m->y1 = (1 - onscreen[1] * pconf.scale) / 2;
  m->valid = false;

  if(euclid && quotient && !closed_manifold) {
    hyperpoint h1 = iso_inverse(models::euclidean_spin) * eumove(euc::eu.user_axes[1]) * C0;
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
    
    m->native = point31(hpoint[0], hpoint[1] * sin(a*2*M_PI), hpoint[1]*cos(a*2*M_PI));
    }
  else if(sphere) {
    m->valid = good_shape = true;
    ld scale;
    USING_NATIVE_GEOMETRY;
    if(euclid) {
      scale = modelscale;
      }
    else if(hyperbolic) {
      // sinh(scale) = modelscale
      scale = asinh(modelscale);
      }
    else if(sphere) {
      if(modelscale >= 1) 
        // do as good as we can...
        scale = M_PI / 2 - 1e-3, good_shape = false, m->valid = false;
      else scale = asin(modelscale);
      }
    else
      scale = 1;
    m->native = unshift(h) * scale;
    m->native = hpxy3(m->native[0], m->native[1], m->native[2]);
    }

  else if(euclid && rug_euclid()) {
    m->native = unshift(h) * modelscale;
    m->native[2] = 0;
    #if MAXMDIM >= 4
    m->native[3] = 1;
    #endif
    m->valid = good_shape = true;
    }
  
  else if(rug_hyperbolic() && euclid) {
    m->valid = good_shape = true;
    USING_NATIVE_GEOMETRY;
    m->native = tC0(parabolic13(h[0]*modelscale, h[1]*modelscale));
    }

  else if(rug_hyperbolic() && hyperbolic && modelscale >= 1) {
    m->valid = good_shape = true;

    // radius of the equidistant
    ld r = acosh(modelscale);
    
    h[3] = h[2]; h[2] = 0;
    
    USING_NATIVE_GEOMETRY;
    
    m->native = rgpushxto0(unshift(h)) * cpush0(2, r);
    }
  
  else {
    m->native = unshift(h);
    ld hd = h[LDIM];
    for(int d=GDIM; d<MAXMDIM-1; d++) {
      m->native[d] = (hd - .99) * (rand() % 1000 - rand() % 1000) / 1000;
      }
    USING_NATIVE_GEOMETRY;
    #if MAXMDIM >= 4
    m->native[3] = 1;
    m->native = normalize(m->native);
    #endif
    }

  m->inqueue = false;
  m->dist = dist;
  points.push_back(m);
  return m;
  }

EX rugpoint *findRugpoint(shiftpoint h) {
  USING_NATIVE_GEOMETRY;
  for(int i=0; i<isize(points); i++) 
    if(geo_dist_q(points[i]->h.h, unshift(h, points[i]->h.shift)) < 1e-5) return points[i];
  return NULL;
  }

EX rugpoint *findOrAddRugpoint(shiftpoint h, double dist) {
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
      edge.len = geo_dist_q(p->h.h, unshift(edge.target->h, p->h.shift)) * modelscale;
  }

EX void calcparam_rug() {
  auto cd = current_display;

  cd->xtop = cd->ytop = 0;
  cd->xsize = cd->ysize = TEXTURESIZE;
  cd->xcenter = cd->ycenter = cd->scrsize = HTEXTURESIZE;
  
  cd->radius = cd->scrsize * pconf.scale;
  }

#if HDR
struct clifford_torus {
  transmatrix T;
  transmatrix iT;
  ld xfactor, yfactor;
  bool flipped;
  hyperpoint xh, yh;
  hyperpoint torus_to_s4(hyperpoint t);  
  hyperpoint actual_to_torus(const hyperpoint& a) {
    return iT * a;
    }
  hyperpoint torus_to_actual(const hyperpoint& t) {
    return T * t;
    }
  clifford_torus();
  ld get_modelscale() {
    return hypot_d(2, xh) * xfactor * 2 * M_PI;
    }
  ld compute_mx();  
  };
#endif

struct hyperpoint clifford_torus::torus_to_s4(hyperpoint t) {
  double alpha = -t[0] * 2 * M_PI;
  double beta = t[1] * 2 * M_PI;
  
  ld ax = alpha + 1.124651, bx = beta + 1.214893;
  return hyperpoint(
    xfactor * sin(ax), 
    xfactor * cos(ax), 
    yfactor * sin(bx),
    yfactor * cos(bx)
    );
  }

clifford_torus::clifford_torus() {
  auto p1 = to_loc(euc::eu.user_axes[0]);
  auto p2 = to_loc(euc::eu.user_axes[1]);

  xh = euc::eumove(p1)*C0-C0;
  yh = euc::eumove(p2)*C0-C0;
  if(nonorientable) yh *= 2;
  
  flipped = false; // sqhypot_d(2, xh) < sqhypot_d(2, yh);
  
  if(flipped) swap(xh, yh);
  
  ld factor2 = sqhypot_d(2, xh) / sqhypot_d(2, yh);
  ld factor = sqrt(factor2);

  yfactor = sqrt(1/(1+factor2));
  xfactor = factor * yfactor;
                                    
  T = build_matrix(xh, yh, C0, C03);  
  iT = inverse(T);
  }

ld clifford_torus::compute_mx() {
  ld mx = 0;
  for(int i=0; i<1000; i++)
    mx = max(mx, hypot(xfactor, yfactor * sin(i)) / (1-yfactor * cos(i)));    
  println(hlog, "mx = ", mx);
  return mx;
  }

#if MAXMDIM >= 4
EX void buildTorusRug() {

  calcparam_rug();
  models::configure();
  
  clifford_torus ct;

  if(gwhere == gSphere) 
    modelscale = ct.get_modelscale();
  
  cell *gs = currentmap->gamestart();  
  map<pair<int, int>, rugpoint*> glues;
  
  ld mx = ct.compute_mx();
    
  auto addToruspoint = [&] (hyperpoint h) {
    auto r = addRugpoint(shiftless(C0), 0);
    hyperpoint onscreen;
    shiftpoint h1 = gmatrix[gs] * ct.torus_to_actual(h);
    applymodel(h1, onscreen);
    r->x1 = onscreen[0];
    r->y1 = onscreen[1];
    
    if(1) {
      USING_NATIVE_GEOMETRY;
      hyperpoint hp = ct.torus_to_s4(h);
      
      /* spherical coordinates are already good, otherwise... */
      
      if(!sphere) {
        /* stereographic projection to get Euclidean conformal torus */
        hp /= (hp[3]+1);
        hp /= mx;
        #if MAXMDIM >= 4
        hp[3] = 1;
        #endif
        }
      
      /* ... in H^3, use inverse Poincare to get hyperbolic conformal torus */

      if(hyperbolic) 
        hp = perspective_to_space(hp, 1, gcHyperbolic);
      
      r->native = hp;
      }
        
    r->valid = true;
    
    static const int X = 100003; // a prime
    auto gluefun = [] (ld z) { return int(frac(z + .5/X) * X); };
    auto p = make_pair(gluefun(h[0]), gluefun(h[1]));
    auto& r2 = glues[p];
    if(r2) r->glueto(r2); else r2 = r;
    return r;
    };
    
  int rugmax = (int) sqrt(vertex_limit / isize(currentmap->allcells()));
  if(rugmax < 1) rugmax = 1;
  if(rugmax > 16) rugmax = 16;
  
  ld rmd = rugmax;
  
  hyperpoint sx = ct.iT * (currentmap->adj(gs, 0)*C0-C0)/rmd;
  hyperpoint sy = ct.iT * (currentmap->adj(gs, 1)*C0-C0)/rmd;
  
  for(int leaf=0; leaf<(nonorientable ? 2 : 1); leaf++)
  for(cell *c: currentmap->allcells()) {
        
    hyperpoint h = ct.iT * calc_relative_matrix(c, gs, C0) * C0;
    
    if(leaf) h[ct.flipped ? 0 : 1] += .5;
    
    rugpoint *rugarr[32][32];
    for(int yy=0; yy<=rugmax; yy++)
    for(int xx=0; xx<=rugmax; xx++)
      rugarr[yy][xx] = addToruspoint(h+xx*sx+yy*sy);
    
    for(int yy=0; yy<rugmax; yy++)
    for(int xx=0; xx<rugmax; xx++)
      addTriangle(rugarr[yy][xx], rugarr[yy+1][xx], rugarr[yy+1][xx+1], modelscale/rugmax),
      addTriangle(rugarr[yy][xx+1], rugarr[yy][xx], rugarr[yy+1][xx+1], modelscale/rugmax);
    }
  
  double maxz = 0;
  
  for(auto p: points)
    maxz = max(maxz, max(abs(p->x1), abs(p->y1)));
  
  if(1) pconf.scale = 1 / maxz;

  if(1) for(auto p: points)
    p->x1 = (1 + pconf.scale * p->x1)/2,
    p->y1 = (1 - pconf.scale * p->y1)/2;
  
  qvalid = 0;
  for(auto p: points) if(!p->glue) qvalid++;
  println(hlog, "qvalid = ", qvalid);

  return;
  }
#endif

EX void verify() {
  vector<ld> ratios;
  for(auto m: points)
    for(auto& e: m->edges) {
      auto m2 = e.target;
      ld l = e.len;
      
      USING_NATIVE_GEOMETRY;
      ld l0 = geo_dist_q(m->native, m2->native);

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
  #if MAXMDIM >= 4
  if(euclid && closed_manifold) {
    good_shape = true;
    buildTorusRug();
    return;
    }
  #endif
  
  celllister cl(centerover ? centerover : cwt.at, get_sightrange(), vertex_limit, NULL);

  for(int i=0; i<isize(cl.lst); i++)
    rug_map[cl.lst[i]] = addRugpoint(ggmatrix(cl.lst[i])*C0, cl.dists[i]);

  for(auto& p: rug_map) {
    cell *c = p.first;
    rugpoint *v = p.second;
    
    if(arcm::in() || arb::in() || (euclid && quotient)) {
      vector<rugpoint*> p(c->type+1);
      for(int j=0; j<c->type; j++) p[j] = findOrAddRugpoint(ggmatrix(c) * get_corner_position(c, j), v->dist);
      for(int j=0; j<c->type; j++) addTriangle(v, p[j], p[(j+1) % c->type]);
      
      if((euclid && quotient) && nonorientable) {
        shiftmatrix T = ggmatrix(c) * eumove(euc::eu.user_axes[1]);
        rugpoint *Tv = addRugpoint(T * C0, 0);
        for(int j=0; j<c->type; j++) p[j] = findOrAddRugpoint(T * get_corner_position(c, j), v->dist);
        for(int j=0; j<c->type; j++) addTriangle(Tv, p[j], p[(j+1) % c->type]);
        }
      }
    
    else for(int j=0; j<c->type; j++) try {
      cell *c2 = c->move(j);
      rugpoint *w = rug_map.at(c2);
      // if(v<w) addEdge(v, w);
      
      cell *c3 = c->modmove(j+1);
      rugpoint *w2 = rug_map.at(c3);
      
      cell *c4 = (cellwalker(c,j) + wstep - 1).cpeek();      
      
      if(c4 != c3) {
        cell *cm = c; comp(cm, c); comp(cm, c2); comp(cm, c3); comp(cm, c4);
        if(cm == c || cm == c4)
          addTriangle(v, w, w2);
        }
      else if(v > w && v > w2)
        addTriangle(v, w, w2);
      }
    catch(const std::out_of_range&) {}
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
  USING_NATIVE_GEOMETRY;
  // double rd = geo_dist_q(m1.h, m2.h) * xd;
  double t = sqhypot_d(3, m1.native - m2.native);
  if(is_anticusp && t > rd*rd) return false;
  t = sqrt(t);
  current_total_error += (t-rd) * (t-rd);
  bool nonzero = abs(t-rd) > err_zero_current;
  double force = (t - rd) / t / 2; // 20.0;
  for(int i=0; i<3; i++) {
    double di = (m2.native[i] - m1.native[i]) * force;
    m1.native[i] += di * d1;
    m2.native[i] -= di * d2;
    if(nonzero && d2>0) enqueue(&m2);
    }  
  return nonzero;
  }

bool force(rugpoint& m1, rugpoint& m2, double rd, bool is_anticusp=false, double d1=1, double d2=1) {
  if(!m1.valid || !m2.valid) return false;
  if(rug_euclid() && fast_euclidean) {
    return force_euclidean(m1, m2, rd, is_anticusp, d1, d2);
    }
  USING_NATIVE_GEOMETRY;
  
  ld t = geo_dist_q(m1.native, m2.native);
  if(is_anticusp && t > rd) return false;
  current_total_error += (t-rd) * (t-rd);
  bool nonzero = abs(t-rd) > err_zero_current;
  double forcev = (t - rd) / 2; // 20.0;
  
  transmatrix T = iso_inverse(rgpushxto0(m1.native));
  hyperpoint ie = inverse_exp(shiftless(T * m2.native));

  transmatrix iT = rgpushxto0(m1.native);
  
  for(int i=0; i<MXDIM; i++) if(std::isnan(m1.native[i])) { 
    addMessage("Failed!");
    println(hlog, "m1 = ", m1.native);
    throw rug_exception();
    }

  m1.native = iT * direct_exp(ie * (d1*forcev/t));
  m2.native = iT * direct_exp(ie * ((t-d2*forcev)/t));

  if(nonzero && d2>0) enqueue(&m2);
  return nonzero;
  }

vector<pair<ld, rugpoint*> > preset_points;

EX void preset(rugpoint *m) {
  if(GDIM == 3) return;
  int q = 0;
  USING_NATIVE_GEOMETRY;
  hyperpoint h = Hypc;
  
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
      hyperpoint ort = (c->native - a->native - cz * (b->native-a->native)) / ch;

      // m->h = a->h + (b->h-a->h) * az - ah * ort
      hyperpoint res = a->native + (b->native-a->native) * az - ah * ort;
      
      h += res;
      
      preset_points.emplace_back(hypot(blen * (ah+ch), blen * (az-cz)), c);
      q++;
      }
    }
    
  #if MAXMDIM >= 4
  if(q>0) m->native = normalize(h);
  #else
  if(q>0) m->native = h / q;
  #endif

  if(std::isnan(m->native[0]) || std::isnan(m->native[1]) || std::isnan(m->native[2]))
    throw rug_exception();
  }

ld sse(const hyperpoint& h) {
  ld sse = 0;
  for(auto& p: preset_points) {
    ld l = p.first;
    USING_NATIVE_GEOMETRY;
    ld l0 = geo_dist_q(h, p.second->native);
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
    if(!rug_euclid()) {
      ld cur = sse(m->native);
      for(int it=0; it<500; it++) {
        ld ex = exp(-it/60);
        again:
        hyperpoint last = m->native;
        USING_NATIVE_GEOMETRY;
        m->native = rgpushxto0(m->native) * cpush0((it/2)%3, (it&1)?ex:-ex);
        ld now = sse(m->native);
        if(now < cur) { cur = now; ex *= 1.2; goto again; }
        else m->native = last;
        }
      }
    }
  for(int it=0; it<50; it++) 
    for(int j=0; j<isize(m->edges); j++)
      force(*m, *m->edges[j].target, m->edges[j].len, false, 1, 0);
  }

EX int divides;
EX int precision_increases;
bool stop = false;

EX bool subdivide_further() {
  if(euclid && closed_manifold) return false;
  if(GDIM == 3) return false;
  return isize(points) * 4 < vertex_limit;
  }

EX void subdivide() {
  int N = isize(points);
  // if(euclid && gwhere == gEuclid) return;
  if(!subdivide_further()) {
    if(euclid && !closed_manifold && gwhere == gEuclid) {
      println(hlog, "Euclidean -- full precision");
      stop = true; 
      }
    else {
      precision_increases++;
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
        #if MAXMDIM >= 4
        USING_NATIVE_GEOMETRY;
        mm->native = mid(m->native, m2->native);
        #else
        mm->native = (m->native + m2->native) / 2;
        #endif
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

EX ld modeldist(const hyperpoint& h1, const hyperpoint& h2) {
  USING_NATIVE_GEOMETRY;
  return geo_dist_q(h1, h2);
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
      return acd_bin(h[0]) + acd_bin(h[1]) * sY + acd_bin(h[2]) * sZ + acd_bin(h[3]) * sT;
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
  else if(modeldist(p->native, q->native) > anticusp_dist - err_zero_current)
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
    code_to_point[get_bincode(p->native)].push_back(p);
  
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

  #if CAP_CRYSTAL && MAXMDIM >= 4
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

extern int besti;

#if CAP_ODS
/* these functions are for the ODS projection, used in VR videos */

bool project_ods(hyperpoint h, hyperpoint& h1, hyperpoint& h2, bool eye) {
  USING_NATIVE_GEOMETRY;
  
  return ods::project(h, h1, h2, eye);

//  printf("%10.5lf %10.5lf %10.5lf ", azeq[0], azeq[1], azeq[2]);
//  printf(" => %10.5lf %10.5lf %10.5lf %10.5lf", x, y, z, t);
  
  
//  printf("\n");
  return true;
  }
#endif

vector<glhr::ct_vertex> ct_array;

vector<glhr::ct_vertex> cp_array;

EX basic_textureinfo tinf;

void drawTriangle(triangle& t) {
  for(int i=0; i<3; i++) {
    if(!t.m[i]) return;
    if(!t.m[i]->valid) return;
    }
  
  ld col = 1;
  if(true) {
    hyperpoint hc = (t.m[1]->native - t.m[0]->native) ^ (t.m[2]->native - t.m[0]->native);  
    double hch = hypot_d(3, hc);  
    col = (2 + hc[0]/hch) / 3;
    if(nonisotropic) col = (9+col) / 10;
    }

  for(int i=0; i<3; i++)  {
    curvepoint(t.m[i]->native);
    tinf.tvertices.push_back(glhr::pointtogl(point3(t.m[i]->x1, t.m[i]->y1, col)));
    }
  }

EX struct renderbuffer *glbuf;

EX void prepareTexture() {
  ensure_glbuf();
  if(!glbuf) { rug::close(); return; }
  resetbuffer rb;
  
  dynamicval<eStereo> d(vid.stereo_mode, sOFF);
  dynamicval<ld> dl(levellines, 0);
  #if CAP_VR
  dynamicval<int> i(vrhr::state, 0);
  #endif
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
  if(finger_center) {
    shiftmatrix V = rgpushxto0(finger_center->h);
    queuestr(V, 0.5, "X", 0xFFFFFFFF, 2);
    for(int i=0; i<72; i++)
      queueline(V * xspinpush0(i*M_PI/32, finger_range), V * xspinpush0((i+1)*M_PI/32, finger_range), 0xFFFFFFFF, vid.linequality);
    }
  drawqueue();
  calcparam();
  rb.reset();
  }

EX bool no_fog;

EX ld lowrug = 1e-2;
EX ld hirug = 1e3;

EX GLuint alternate_texture;

EX bool rug_control() { return rug::rugged; }

#if HDR

struct using_rugview {
  using_rugview() { if(rug_control()) swap(View, rugView), swap(geometry, gwhere); }
  ~using_rugview() { if(rug_control()) swap(View, rugView), swap(geometry, gwhere); }
  };

#endif

EX purehookset hooks_rugframe;

EX void drawRugScene() {
  callhooks(hooks_rugframe);
  USING_NATIVE_GEOMETRY;
  tinf.texture_id = alternate_texture ? alternate_texture : glbuf->renderedTexture;
  tinf.tvertices.clear();
  
  ptds.clear();
  
  for(auto t: triangles) drawTriangle(t);
  
  auto& rug = queuecurve(shiftless(Id), 0, 0xFFFFFFFF, PPR::LINE);

  dynamicval<transmatrix> tV(View, View);
  View = Id; /* needed for vr */

  if(nonisotropic) {
    transmatrix T2 = eupush( tC0(view_inverse(rugView)) );
    NLP = rugView * T2;  
    rug.V = shiftless(ortho_inverse(NLP) * rugView);
    }
  else {
    rug.V = shiftless(rugView);
    }
   
  rug.offset_texture = 0;
  rug.tinf = &tinf;
  if(levellines && disable_texture) rug.tinf = nullptr;
  rug.flags = POLY_TRIANGLES | POLY_FAT | POLY_PRINTABLE | POLY_ALWAYS_IN | POLY_ISSIDE | POLY_SHADE_TEXTURE;

  dynamicval<projection_configuration> p(pconf, rconf);
  calcparam();
  
  drawqueue();
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
  when_enabled = 0;
  GLERR("before init");
  ensure_glbuf();
  if(!glbuf) { rugged = false; return; }
  rugged = true;
  if(renderonce && !disable_texture) prepareTexture();
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
  divides = 0;
  precision_increases = 0;
  
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
    
    if(sphere && pmodel == mdDisk && pconf.alpha > 1)
      valid = false;
    
    if(display_warning && !valid)
      gotoHelp(
        "Note: this mode is based on what you see on the screen -- but re-rendered in another way. "
        "If not everything is shown on the screen (e.g., too zoomed in), the results will be incorrect "
        "(though possibly interesting). "
        "Use a different projection to fix this."
        );
    }
  catch(const rug_exception&) {
    close();
    clear_model();
    }
  }

EX void reset_view() {
  rugView = Id;
  if(perspective()) {
    using_rugview urv;
    shift_view(ztangent(model_distance));
    }
  }

EX void init() {
  if(dual::state) return;
  reopen();
  if(rugged) init_model();
  reset_view();
  }

EX void clear_model() {
  triangles.clear();
  for(int i=0; i<isize(points); i++) delete points[i];
  rug_map.clear();
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

#define CAP_HOLDKEYS (CAP_SDL && !ISWEB)

EX bool handlekeys(int sym, int uni) {
  USING_NATIVE_GEOMETRY;
  if(NUMBERKEY == '2') {
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
  else if(NUMBERKEY == '3') {
    #if CAP_CRYSTAL
    if(in_crystal())
      crystal::switch_z_coordinate();
    else
    #endif
      rotate_view(cspin(0, 2, M_PI));
    return true;
    }
  else if(NUMBERKEY == '4') {
    #if CAP_CRYSTAL
    if(in_crystal())
      crystal::flip_z();
    else
    #endif
      rotate_view(cspin(0, 2, M_PI/2));
    return true;
    }
  #if CAP_CRYSTAL
  else if(sym == SDLK_HOME && in_crystal()) {
    crystal::next_home_orientation();
    return true;
    }
  #endif
  else return false;
  }

EX void finger_on(int coord, ld val) {
  for(auto p: points) {
    ld d = hdist(finger_center->h, p->getglue()->h);
    push_point(p->native, coord, val * finger_force * exp( - sqr(d / finger_range)));
    }
  enqueue(finger_center), good_shape = false;
  }

transmatrix last_orientation;

EX ld move_on_touch = 1;

EX void actDraw() {
  try {

  if(!renderonce && !(levellines && disable_texture)) prepareTexture();
  else if(renderlate) {
    renderlate--;
    prepareTexture();
    }
  // do not display button
  else playerfound = true;
  current_display->set_viewport(0);
  physics();
  drawRugScene();
  
  double alpha = (ticks - lastticks) / 1000.0;
  lastticks = ticks;

  #if CAP_HOLDKEYS
  const Uint8 *keystate = SDL12_GetKeyState(NULL);
  if(keystate[SDL12(SDLK_LALT, SDL_SCANCODE_LALT)]) alpha /= 10;
  #endif

  #if CAP_HOLDKEYS
  auto perform_finger = [=] () {
    if(keystate[SDL12(SDLK_HOME, SDL_SCANCODE_HOME)]) finger_range /= exp(alpha);
    if(keystate[SDL12(SDLK_END, SDL_SCANCODE_END)]) finger_range *= exp(alpha);
    if(keystate[SDL12(SDLK_LEFT, SDL_SCANCODE_LEFT)]) finger_on(0, -alpha);
    if(keystate[SDL12(SDLK_RIGHT, SDL_SCANCODE_RIGHT)]) finger_on(0, alpha);
    if(keystate[SDL12(SDLK_UP, SDL_SCANCODE_UP)]) finger_on(1, alpha);
    if(keystate[SDL12(SDLK_DOWN, SDL_SCANCODE_DOWN)]) finger_on(1, -alpha);
    if(keystate[SDL12(SDLK_PAGEDOWN, SDL_SCANCODE_PAGEDOWN)]) finger_on(2, -alpha);
    if(keystate[SDL12(SDLK_PAGEUP, SDL_SCANCODE_PAGEUP)]) finger_on(2, +alpha);
    };

  if(finger_center)
    perform_finger();    
  #endif
    }
  catch(const rug_exception&) {
    rug::close();
    }
  }

int besti;

static const ld RADAR_INF = 1e12;
ld radar_distance = RADAR_INF;

EX shiftpoint gethyper(ld x, ld y) {
  
  projection_configuration bak = pconf;
  pconf = rconf;
  calcparam(); 

  double mx = (x - current_display->xcenter)/current_display->radius;
  double my = (y - current_display->ycenter)/current_display->radius/pconf.stretch;

  bool vr = vrhr::active() && which_pointer;
  transmatrix U;

  if(1) {
    USING_NATIVE_GEOMETRY;
    U = ortho_inverse(NLP) * rugView;
    }
  
  #if CAP_VR
  transmatrix T = Id;
  if(vr) {
    mx = my = 0;
    E4;
    vrhr::gen_mv();
    T = vrhr::model_to_controller(which_pointer);
    }
  #endif
  
  calcparam();
  
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
    if(1) {
      USING_NATIVE_GEOMETRY;
      // USING_RUG_PMODEL;
      // dynamicval<eModel> m(pmodel, mdEquidistant);

      if(elliptic && pmodel == mdDisk) {
        int sp = 
          (r0->native[3] < 0) + (r1->native[3] < 0) + (r2->native[3] < 0);
        if(sp == 1 || sp == 2) continue;
        }
      

      auto find = [&] (const hyperpoint &native, hyperpoint& res) {
        if(!vr) {
          applymodel(shiftless(U * native), res);
          }
        #if CAP_VR
        else {
          dynamicval<int> vi(vrhr::state, 2);
          bool bad;
          res = vrhr::model_location(shiftless(U * native), bad);
          if(bad) error = true;
          E4; res[3] = 1; res = T * res;
          }
        #endif
        };

      find(r0->native, p0);
      find(r1->native, p1);
      find(r2->native, p2);
      
      }

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
      rz1 = -rz1;
      if(vr && rz1 < 0) { /* behind the controller, ignore */ }
      else if(rz1 < radar_distance) {
        radar_distance = rz1;
        rx1 = r0->x1 + (r1->x1 - r0->x1) * tx + (r2->x1 - r0->x1) * ty;
        ry1 = r0->y1 + (r1->y1 - r0->y1) * tx + (r2->y1 - r0->y1) * ty;
        #if CAP_VR
        if(vr) vrhr::pointer_distance = radar_distance;
        #endif
        }
      found = true;
      }
    }
  
  pconf = bak;
  if(!found) return shiftless(Hypc);
  
  double px = rx1 * TEXTURESIZE, py = (1-ry1) * TEXTURESIZE;

  calcparam_rug();
  models::configure();
  shiftpoint h = hr::gethyper(px, py);
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
     ;
  }

void change_texturesize() {
  if(rugged) {
    close();
    reopen();
    }
  }

ld old_distance;

EX void rug_geometry_choice() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("hypersian rug mode"), iinf[itPalace].color, 150, 100);
  
  USING_NATIVE_GEOMETRY; 
  
  dialog::addBoolItem("Euclidean", euclid, 'a');
  dialog::add_action([] { gwhere = rgEuclid; popScreen(); });

  dialog::addBoolItem("hyperbolic", hyperbolic, 'b');
  dialog::add_action([] { gwhere = rgHyperbolic; popScreen(); });

  dialog::addBoolItem("spherical", sphere && !elliptic, 'c');
  dialog::add_action([] { gwhere = rgSphere; popScreen(); });

  dialog::addBoolItem("elliptic", elliptic, 'd');
  dialog::add_action([] { gwhere = rgElliptic; popScreen(); });

  dialog::addBoolItem("Nil", nil, 'e');
  dialog::add_action([] { gwhere = gNil; popScreen(); });

  dialog::addBoolItem("Solv", sol, 'e');
  dialog::add_action([] { gwhere = gSol; popScreen(); });

  dialog::addBack();
  dialog::display();
  }

EX void show() {
  cmode = sm::SIDE | sm::MAYDARK | sm::PANNING;
  gamescreen();
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
  if(rug::rugged) {
    dialog::addSelItem(XLAT("projection"), models::get_model_name(rconf.model), 'p'); 
    }
  else dialog::addBreak(100);
  if(!rug::rugged) {
    dynamicval<eGeometry> g(geometry, gwhere);
    dialog::addSelItem(XLAT("native geometry"), geometry_name(), 'n');
    if(gwhere == rgElliptic) dialog::lastItem().value += " (e)";
    }
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
  #if ISMOBILE
  dialog::addSelItem(XLAT("move on touch"), fts(move_on_touch), 'G');
  #endif
  dialog::addSelItem(XLAT("anti-crossing"), fts(anticusp_factor), 'A');
  dialog::addBoolItem(XLAT("3D monsters/walls on the surface"), spatial_rug, 'S');
  dialog::add_action([] () { spatial_rug = !spatial_rug; });
  edit_levellines('L');

#if CAP_SURFACE  
  if(hyperbolic)
    dialog::addItem(XLAT("smooth surfaces"), 'c');
  else
    dialog::addBreak(100);
#endif

  dialog::addBreak(50);
  dialog::addHelp();
  dialog::addBack();

  dialog::display();
  keyhandler = [] (int sym, int uni) {
    handlePanning(sym, uni);
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
      dialog::editNumber(move_on_touch, -1, 1, .1, 0, XLAT("move on touch"), "");
      dialog::extra_options = anims::rug_angle_options;
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
            if(adjust_points) p->native *= modelscale / last;
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
      pushScreen(models::model_menu);
      }
    else if(uni == 'd') {
      dialog::editNumber(model_distance, -10, 10, .1, 1, XLAT("model distance"), 
        XLAT("In the perspective projection, this sets the distance from the camera to the center of the model. "
        "In the orthogonal projection this just controls the scale.")
        );
      old_distance = model_distance;
      dialog::reaction = [] () { 
        if(rug::rugged && perspective()) {
          using_rugview rv;
          shift_view(ztangent(old_distance - model_distance));
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
    #if MAXMDIM >= 4
    else if(uni == 'n' && !rug::rugged) 
      pushScreen(rug_geometry_choice);
    #endif
#if CAP_SDL
    else if(uni == 'g' && !rug::rugged)
      rendernogl = !rendernogl;
#endif
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

EX void rug_save(string fname) {
  fhstream f(fname, "wb");
  if(!f.f) {
    addMessage(XLAT("Failed to save embedding to %1", fname));
    return;
    }
  f.write(f.vernum);
  f.write(gwhere);
  USING_NATIVE_GEOMETRY;
  int N = isize(points);
  f.write(N);
  map<rugpoint*, int> ids;
  for(int i=0; i<isize(points); i++) ids[points[i]] = i;
  f.write(surface::sh);
  for(int i=0; i<4; i++) for(int j=0; j<4; j++) 
    f.write(rugView[i][j]);
  auto get_id = [&] (rugpoint *r) {
    if(!r) return int(-1);
    return ids[r];
    };
  for(auto p: points) {
    f.write(p->valid);
    f.write(p->x1);
    f.write(p->y1);
    f.write(p->native);
    f.write(get_id(p->glue));
    }
  int M = isize(triangles);
  f.write(M);
  for(auto t: triangles) {
    f.write(get_id(t.m[0]));
    f.write(get_id(t.m[1]));
    f.write(get_id(t.m[2]));
    }

  int cp = isize(surface::coverage);
  f.write(cp);
  for(auto p: surface::coverage) f.write(p.first), f.write(p.second);
  }

EX void rug_load(string fname) {
  clear_model();
  fhstream f(fname, "rb");
  if(!f.f) {
    addMessage(XLAT("Failed to load embedding from %1", fname));
    return;
    }
  f.read(f.vernum);
  f.read(gwhere);
  USING_NATIVE_GEOMETRY;
  int N = f.get<int>();
  println(hlog, "N = ", N);
  points.resize(N);
  for(int i=0; i<N; i++) 
    points[i] = new rugpoint;
  f.read(surface::sh);
  for(int i=0; i<4; i++) for(int j=0; j<4; j++) 
    f.read(rugView[i][j]);
  auto by_id = [&] (rugpoint *& p) {
    int i = f.get<int>();
    if(i == -1) p = nullptr;
    else p = points[i];
    };
  for(auto p: points) {
    f.read(p->valid);
    f.read(p->x1);
    f.read(p->y1);
    f.read(p->native);
    by_id(p->glue);
    }
  triangles.resize(f.get<int>());
  for(auto& t: triangles) {
    by_id(t.m[0]);
    by_id(t.m[1]);
    by_id(t.m[2]);
    }

  surface::coverage.resize(f.get<int>());
  for(auto p: surface::coverage) f.read(p.first), f.read(p.second);
  good_shape = true;
  }

#if CAP_COMMANDLINE
int rugArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-rugmodelscale")) {
    shift_arg_formula(modelscale);
    }

  else if(argis("-ruggeo")) {
    shift(); gwhere = readGeo(args());
    if(gwhere == gEuclid) gwhere = rgEuclid;
    if(gwhere == gSphere) gwhere = rgSphere;
    if(gwhere == gNormal) gwhere = rgHyperbolic;
    if(gwhere == gElliptic) gwhere = rgElliptic;
    }

  else if(argis("-rugpers")) {
    USING_NATIVE_GEOMETRY;
    rconf.model = nonisotropic ? mdGeodesic : mdPerspective;
    }

  else if(argis("-rugonce")) {
    renderonce = true;
    }

  else if(argis("-rugsave")) {
    shift(); rug_save(args());
    }

  else if(argis("-rugload")) {
    PHASE(3); 
    start_game();
    calcparam();
    rug::init();
    shift(); rug_load(args());
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

  else if(argis("-rugorth")) {
    rconf.model = mdEquidistant;
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
    err_zero_current = err_zero;
    }

  else if(argis("-rugon")) {
    PHASE(3); 
    start_game();
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

auto clear_rug_map = addHook(hooks_clearmemory, 40, [] () { rug_map.clear(); });

EX }

#endif

#if !CAP_RUG

// fake for mobile
EX namespace rug {
    EX bool rugged = false;
    EX bool renderonce = false;
    EX bool rendernogl = true;
    EX bool mouse_control_rug = false;
    EX int texturesize = 512;
    EX ld scale = 1.0f;
    EX bool rug_control() { return false; }
    EX bool in_crystal() { return false; }
    EX void reset_view() { }
    EX void close() { }
#if HDR
  struct using_rugview {};
#endif
EX }

#endif
}
