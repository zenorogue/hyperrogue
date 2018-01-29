// Hyperbolic Rogue
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

// implementation of the Hypersian Rug mode


#if CAP_RUG

#define TEXTURESIZE (texturesize)
#define HTEXTURESIZE (texturesize/2)

#if !CAP_GLEW
#if ISLINUX
extern "C" {
GLAPI void APIENTRY glGenFramebuffers (GLsizei n, GLuint *framebuffers);
GLAPI void APIENTRY glBindFramebuffer (GLenum target, GLuint framebuffer);
GLAPI void APIENTRY glFramebufferTexture (GLenum target, GLenum attachment, GLuint texture, GLint level);
GLAPI GLenum APIENTRY glCheckFramebufferStatus (GLenum target);
GLAPI void APIENTRY glDrawBuffers (GLsizei n, const GLenum *bufs);
GLAPI void APIENTRY glGenRenderbuffers (GLsizei n, GLuint *renderbuffers);
GLAPI void APIENTRY glBindRenderbuffer (GLenum target, GLuint renderbuffer);
GLAPI void APIENTRY glRenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
GLAPI void APIENTRY glFramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
GLAPI void APIENTRY glDeleteRenderbuffers (GLsizei n, const GLuint *renderbuffers);
GLAPI void APIENTRY glDeleteFramebuffers (GLsizei n, const GLuint *framebuffers);
}
#endif

#if ISMAC
#define glFramebufferTexture glFramebufferTextureEXT 
#endif
#endif

namespace rug {

struct rug_exception { };

bool fast_euclidean = true;
bool good_shape;

ld modelscale = 1;
ld model_distance = 2;

ld fov = 90;

eGeometry gwhere = gEuclid;

#define USING_NATIVE_GEOMETRY dynamicval<eGeometry> gw(geometry, gwhere == gElliptic ? gSphere : gwhere)

// hypersian rug datatypes and globals
//-------------------------------------

bool rugged = false;
bool genrug = false;
bool glew   = false;

int vertex_limit = 20000;

bool renderonce  = false;
bool rendernogl  = false;
int  texturesize = 1024;
ld scale = 1;

ld err_zero = 1e-3, err_zero_current, current_total_error;

int queueiter, qvalid, dt;

struct edge {
  struct rugpoint *target;
  double len;
  };

struct rugpoint {
  double x1, y1;
  bool valid;
  bool inqueue;
  double dist;
  hyperpoint h;    // point in the represented space
  hyperpoint flat; // point in the native space, in azeq
  vector<edge> edges;
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
  };

rugpoint *finger_center;
ld finger_range = .1;
ld finger_force = 1;

struct triangle {
  rugpoint *m[3];
  triangle(rugpoint *m1, rugpoint *m2, rugpoint *m3) {
    m[0] = m1; m[1] = m2; m[2] = m3;
    }
  };

vector<rugpoint*> points;
vector<triangle> triangles;

bool rug_perspective = false;

// extra geometry functions
//--------------------------

// returns a matrix M
// such that inverse(M) * h1 = ( |h1|, 0, 0) and inverse(M) * h2 = ( .., .., 0)

transmatrix orthonormalize(hyperpoint h1, hyperpoint h2) {
  using namespace hyperpoint_vec;

  hyperpoint vec[3] = {h1, h2, h1 ^ h2};
  
  for(int i=0; i<3; i++) {
    for(int j=0; j<i; j++) vec[i] -= vec[j] * (vec[i] | vec[j]);
    if(zero3(vec[i])) {
      // 'random' direction
      vec[i] = hpxyz(1.12, 1.512+i, 1.12904+i);
      for(int j=0; j<i; j++) vec[i] -= vec[j] * (vec[i] | vec[j]);
      }
    vec[i] /= hypot3(vec[i]);
    }

  transmatrix M;
  for(int i=0; i<3; i++) for(int j=0; j<3; j++)
    M[i][j] = vec[j][i];
  
  return M;
  }

hyperpoint azeq_to_hyperboloid(hyperpoint h) {
  if(abs(h[2])>1e-4) printf("Error: h[2] = %lf\n", h[2]);
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
    ld d2 = hypot2(h);
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
    // if(zero3(h)) { h[0] = 1e-9; h[1] = 1e-10; h[2] = 1e-11; }
    normalizer n(hpxyz(coord==0,coord==1,coord==2), h);
    hyperpoint f = n(h);
    h = n[xpush(val) * f];
    }
  }
  
void push_all_points(int coord, ld val) {
  if(!val) return;
  else for(int i=0; i<size(points); i++)
    push_point(points[i]->flat, coord, val);
  }

// construct the graph
//---------------------

int hyprand;

rugpoint *addRugpoint(hyperpoint h, double dist) {
  rugpoint *m = new rugpoint;
  m->h = h;
  
  /*
  ld tz = vid.alphax+h[2];
  m->x1 = (1 + h[0] / tz) / 2;
  m->y1 = (1 + h[1] / tz) / 2;
  */

  hyperpoint onscreen;
  applymodel(m->h, onscreen);
  m->x1 = (1 + onscreen[0] * vid.scale) / 2;
  m->y1 = (1 - onscreen[1] * vid.scale) / 2;
  m->valid = false;

  using namespace hyperpoint_vec;

  if(sphere) {
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
        scale = M_PI / 2 - 1e-3, good_shape = false;
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
    ld d0 = hypot2(h); if(!d0) d0 = 1;
    
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
      hpoint = xpush(r) * ypush(d) * xpush(-r) * C0;
      hpoint[0] = -hpoint[0];
      }

    ld hpdist = hdist0(hpoint);
    ld z = hypot2(hpoint);
    if(z==0) z = 1;
    m->flat = hpxyz(hpdist * h[0]/d0 * hpoint[1] / z, hpdist * h[1]/d0 * hpoint[1] / z, -hpdist * hpoint[0] / z);    
    }
  
  else m->flat = // hpxyz(h[0], h[1], sin(atan2(h[0], h[1]) * 3 + hyprand) * (h[2]-1) / 1000);
    hpxyz(h[0], h[1], (h[2] - .99) * (rand() % 1000 - rand() % 1000) / 1000);

  if(rug_perspective)
    push_point(m->flat, 2, -model_distance);
  
  // if(rug_perspective && gwhere == gEuclid) m->flat[2] -= 3;
  m->inqueue = false;
  m->dist = dist;
  points.push_back(m);
  return m;
  }

rugpoint *findRugpoint(hyperpoint h) {
  for(int i=0; i<size(points); i++) 
    if(intvalxyz(points[i]->h, h) < 1e-5) return points[i];
  return NULL;
  }

rugpoint *findOrAddRugpoint(hyperpoint h, double dist) {
  rugpoint *r = findRugpoint(h);
  return r ? r : addRugpoint(h, dist);
  }

void addNewEdge(rugpoint *e1, rugpoint *e2, ld len = 1) {
  edge e; e.len = len;
  e.target = e2; e1->edges.push_back(e);
  e.target = e1; e2->edges.push_back(e);
  }

void addEdge(rugpoint *e1, rugpoint *e2, ld len = 1) {
  for(int i=0; i<size(e1->edges); i++) 
    if(e1->edges[i].target == e2) return;
  addNewEdge(e1, e2, len);
  }

void addTriangle(rugpoint *t1, rugpoint *t2, rugpoint *t3, ld len = 1) {
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

void calcLengths() {
  for(int i=0; i<size(points); i++) for(int j=0; j<size(points[i]->edges); j++) {
    ld d = hdist(points[i]->h, points[i]->edges[j].target->h);
    if(elliptic && d > M_PI/2) d = M_PI - d;
    points[i]->edges[j].len = d * modelscale;
    }
  }

void setVidParam() {
  vid.xres = vid.yres = TEXTURESIZE;
  vid.scrsize = HTEXTURESIZE;
  vid.radius = vid.scrsize * vid.scale; vid.xcenter = HTEXTURESIZE; vid.ycenter = HTEXTURESIZE;
  vid.beta = 2; vid.alphax = 1; vid.eye = 0; vid.goteyes = false;
  }

void buildTorusRug() {
  using namespace torusconfig;

  setVidParam();

  struct toruspoint {
    int x,y;
    toruspoint() { x=y=getqty(); }
    toruspoint(int _x, int _y) : x(_x), y(_y) {}
    int d2() { 
      return x*x+(euclid6?x*y:0)+y*y;
      }
    };
  
  vector<toruspoint> zeropoints;
  vector<toruspoint> tps(qty);
  
  auto& mode = tmodes[torus_mode];
  bool single = mode.flags & TF_SINGLE;
  bool klein = mode.flags & TF_KLEIN;
  
  pair<toruspoint, toruspoint> solution;
  
  if(single) {
    for(int ax=-qty; ax<qty; ax++) 
    for(int ay=-qty; ay<qty; ay++) {
      int v = (ax*dx + ay*dy) % qty;
      if(v<0) v += qty;
      toruspoint tp(ax, ay);
      if(tps[v].d2() > tp.d2()) tps[v] = tp;
      if(v == 0) 
        zeropoints.emplace_back(ax, ay);
      }
    
    ld bestsol = 1e12;
    
    for(auto p1: zeropoints)
    for(auto p2: zeropoints) {
      int det = p1.x * p2.y - p2.x * p1.y;
      if(det < 0) continue;
      if(det != qty && det != -qty) continue;
      ld quality = ld(p1.d2()) * p2.d2();
      if(quality < bestsol * 3)
      if(quality < bestsol)
        bestsol = quality, solution.first = p1, solution.second = p2;
      }
    
    if(solution.first.d2() > solution.second.d2())
      swap(solution.first, solution.second);
    }
  else {
    if(klein)
      solution.first = toruspoint(2*sdx, 0);
    else
      solution.first = toruspoint(sdx, 0);
    if(mode.flags & TF_WEIRD)
      solution.second = toruspoint(sdy/2, sdy);
    else
      solution.second = toruspoint(0, sdy);

    if(solution.first.d2() > solution.second.d2())
      swap(solution.first, solution.second);
    }
    
  ld factor = sqrt(ld(solution.second.d2()) / solution.first.d2());
  
  printf("factor = %lf\n", factor);
  if(factor <= 2.05) factor = 2.2;
  factor -= 1;
        
  // 22,1
  // 7,-17
  
  // transmatrix z1 = {{{22,7,0}, {1,-17,0}, {0,0,1}}};
  transmatrix z1 = {{{(ld)solution.first.x,(ld)solution.second.x,0}, {(ld)solution.first.y,(ld)solution.second.y,0}, {0,0,1}}};
  transmatrix z2 = inverse(z1);
  
  map<pair<int, int>, rugpoint*> glues;
  
  auto addToruspoint = [&] (ld x, ld y) {
    auto r = addRugpoint(C0, 0);
    hyperpoint onscreen;
    applymodel(tC0(eumove(x, y)), onscreen);
    // take point (1,0)
    // apply eumove(1,0)
    // divide by EUCSCALE
    // multiply by vid.radius (= HTEXTURESIZE * rugzoom)
    // add 1, divide by texturesize
    r->x1 = onscreen[0];
    r->y1 = onscreen[1];
    // r->y1 = (1 + onscreen[1] * rugzoom / EUCSCALE)/2;
    hyperpoint h1 = hpxyz(x, y, 0);
    hyperpoint h2 = z2 * h1;
    double alpha = -h2[0] * 2 * M_PI;
    double beta = -h2[1] * 2 * M_PI;
    // r->flat = {alpha, beta, 0}; 
    double sc = (factor+1)/4;
    
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
      addTriangle(rugarr[yy][xx], rugarr[yy][xx+1], rugarr[yy+1][xx+1], modelscale/rugmax);
    }
  
  double maxz = 0;
  
  for(auto p: points)
    maxz = max(maxz, max(abs(p->x1), abs(p->y1)));
  
  // maxz * rugzoom * vid.radius == vid.radius
  
  vid.scale = 1 / maxz;
  
  for(auto p: points)
    p->x1 = (vid.xcenter + vid.radius * vid.scale * p->x1)/ vid.xres,
    p->y1 = (vid.ycenter - vid.radius * vid.scale * p->y1)/ vid.yres;
  
  qvalid = 0;
  for(auto p: points) if(!p->glue) qvalid++;
  printf("qvalid = %d\n", qvalid);

  if(rug_perspective)
    push_all_points(2, -model_distance);  
  
  return;
  }

void verify() {
  vector<ld> ratios;
  for(auto m: points)
    for(auto& e: m->edges) {
      auto m2 = e.target;
      ld l = e.len;
      
      normalizer n(m->flat, m2->flat);
      hyperpoint h1 = n(m->flat);
      hyperpoint h2 = n(m2->flat);
      ld l0 = hdist(h1, h2);
      ratios.push_back(l0 / l);
      }
    
  printf("Length verification:\n");
  sort(ratios.begin(), ratios.end());
  for(int i=0; i<size(ratios); i += size(ratios) / 10)
    printf("%lf\n", ratios[i]);
  printf("\n");
  }

void comp(cell*& minimum, cell *next) {
  int nc = next->cpdist, mc = minimum->cpdist;
  if(tie(nc, next) < tie(mc, minimum))
    minimum = next;
  }
  
void buildRug() {

  if(torus) {
    good_shape = true;
    buildTorusRug();
    return;
    }
  
  celllister cl(centerover.c ? centerover.c : cwt.c, sightrange, vertex_limit, NULL);

  map<cell*, rugpoint *> vptr;
  
  for(int i=0; i<size(cl.lst); i++)
    vptr[cl.lst[i]] = addRugpoint(shmup::ggmatrix(cl.lst[i])*C0, cl.dists[i]);

  for(auto& p: vptr) {
    cell *c = p.first;
    rugpoint *v = p.second;
    for(int j=0; j<c->type; j++) try {
      cell *c2 = c->mov[j];
      rugpoint *w = vptr.at(c2);
      // if(v<w) addEdge(v, w);
      
      cell *c3 = c->mov[(j+1) % c->type];
      rugpoint *w2 = vptr.at(c3);
      
      if(a4) {
        cellwalker cw(c, j);
        cwstep(cw); cwspin(cw, -1); cwstep(cw);
        cell *c4 = cw.c;
        cell *cm = c; comp(cm, c); comp(cm, c2); comp(cm, c3); comp(cm, c4);
        if(cm == c || cm == c4)
          addTriangle(v, w, w2);
        }
      else if(v > w && v > w2)
        addTriangle(v, w, w2);
      }
    catch(out_of_range) {}
    }

  printf("vertices = %d triangles=  %d\n", size(points), size(triangles));

  calcLengths();
  sort(points.begin(), points.end(), psort);
  
  verify();
  }

// rug physics

queue<rugpoint*> pqueue;
void enqueue(rugpoint *m) {
  if(m->inqueue) return;
  pqueue.push(m);
  m->inqueue = true;
  }

bool force_euclidean(rugpoint& m1, rugpoint& m2, double rd, double d1=1, double d2=1) {
  if(!m1.valid || !m2.valid) return false;
  // double rd = hdist(m1.h, m2.h) * xd;
  // if(rd > rdz +1e-6 || rd< rdz-1e-6) printf("%lf %lf\n", rd, rdz);
  double t = 0;
  for(int i=0; i<3; i++) t += (m1.flat[i] - m2.flat[i]) * (m1.flat[i] - m2.flat[i]);
  t = sqrt(t);
  /* printf("%s ", display(m1.flat));
  printf("%s ", display(m2.flat));
  printf("%lf/%lf\n", t, rd); */
  current_total_error += (t-rd) * (t-rd);
  bool nonzero = abs(t-rd) > err_zero_current;
  double force = (t - rd) / t / 2; // 20.0;
  for(int i=0; i<3; i++) {
    double di = (m2.flat[i] - m1.flat[i]) * force;
    m1.flat[i] += di * d1;
    m2.flat[i] -= di * d2;
    if(nonzero && d2>0) enqueue(&m2);
    }  
  return nonzero;
  }

bool force(rugpoint& m1, rugpoint& m2, double rd, double d1=1, double d2=1) {
  if(!m1.valid || !m2.valid) return false;
  if(gwhere == gEuclid && fast_euclidean) {
    return force_euclidean(m1, m2, rd, d1, d2);
    }
  // double rd = hdist(m1.h, m2.h) * xd;
  // if(rd > rdz +1e-6 || rd< rdz-1e-6) printf("%lf %lf\n", rd, rdz);
  using namespace hyperpoint_vec;
  normalizer n(m1.flat, m2.flat);
  hyperpoint f1 = n(m1.flat);
  hyperpoint f2 = n(m2.flat);
  
  ld t = hdist(f1, f2);
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

/*  
  printf("%p %p\n", &m1, &m2);

  printf("m1      = %s\n", display(m1.flat));
  printf("m2      = %s\n", display(m2.flat));
  printf("Mi * m1 = %s\n", display(Mi*m1.flat));
  printf("Mi * m2 = %s\n", display(Mi*m2.flat));

  printf("     f1 = %s\n", display(f1));
  printf(" T * f1 = %s\n", display(T * f1));
  printf("T1 * f1 = %s\n", display(T1 * f1));
  printf("     f2 = %s\n", display(f2));
  printf(" T * f2 = %s\n", display(T * f2));
  printf("T1 * f2 = %s\n", display(T1 * f2));
  printf("iT1     = %s\n", display(iT1 * C0));
  printf("iT1 + t = %s\n", display(iT1 * xpush(t) * C0));
*/

  f1 = iT1 * xpush(forcev) * C0;
  f2 = iT1 * xpush(t-forcev) * C0;

  m1.flat = n[f1];
  m2.flat = n[f2];
  
  if(nonzero && d2>0) enqueue(&m2);
  return nonzero;
  }

vector<pair<ld, rugpoint*> > preset_points;

void preset(rugpoint *m) {
  int q = 0;
  hyperpoint h;
  for(int i=0; i<3; i++) h[i] = 0;
  using namespace hyperpoint_vec;
  
  preset_points.clear();
  
  for(int j=0; j<size(m->edges); j++)
  for(int k=0; k<j; k++) {
    rugpoint *a = m->edges[j].target;
    rugpoint *b = m->edges[k].target;
    if(!a->valid) continue;
    if(!b->valid) continue;
    double blen = -1;
    for(int j2=0; j2<size(a->edges); j2++) 
      if(a->edges[j2].target == b) blen = a->edges[j2].len;
    if(blen <= 0) continue;
    for(int j2=0; j2<size(a->edges); j2++) 
    for(int k2=0; k2<size(b->edges); k2++) 
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
      
      // printf("A %lf %lf %lf %lf C %lf %lf %lf %lf\n", a1, a2, az, ah, c1, c2, cz, ch);
      }
    }
    
  if(q>0) m->flat = h/q;
  // printf("preset (%d) -> %s\n", q, display(m->flat));
  if(std::isnan(m->flat[0]) || std::isnan(m->flat[1]) || std::isnan(m->flat[2]))
    throw rug_exception();
  }

ld sse(hyperpoint h) {
  ld sse = 0;
  for(auto& p: preset_points) {
    ld l = p.first;
    normalizer n(h, p.second->flat);
    hyperpoint h1 = n(h);
    hyperpoint h2 = n(p.second->flat);
    ld l0 = hdist(h1, h2);
    sse += (l0-l) * (l0-l);
    }
  
  return sse;
  }

void optimize(rugpoint *m, bool do_preset) {

  if(do_preset) {
    preset(m);
    // int ed0 = size(preset_points);
    for(auto& e: m->edges) if(e.target->valid) 
      preset_points.emplace_back(e.len, e.target);
    if(gwhere >= gSphere) {
      ld cur = sse(m->flat);
      for(int it=0; it<500; it++) {
        ld ex = exp(-it/60);
        again:
        hyperpoint last = m->flat;
        switch(it%6) {
          case 0: m->flat[0] += ex; break;
          case 1: m->flat[0] -= ex; break;
          case 2: m->flat[1] += ex; break;
          case 3: m->flat[1] -= ex; break;
          case 4: m->flat[2] += ex; break;
          case 5: m->flat[2] -= ex; break;
          }
        ld now = sse(m->flat);
        if(now < cur) { cur = now; ex *= 1.2; goto again; }
        else m->flat = last;
        }
      // printf("edges = [%d] %d sse = %lf\n",ed0, size(preset_points), cur);
      }
    }
  for(int it=0; it<50; it++) 
    for(int j=0; j<size(m->edges); j++)
      force(*m, *m->edges[j].target, m->edges[j].len, 1, 0);
  }

int divides = 0;
bool stop = false;

bool subdivide_further() {
  if(torus) return false;
  return size(points) * 4 < vertex_limit;
  }

void subdivide() {
  int N = size(points);
  // if(euclid && gwhere == gEuclid) return;
  if(!subdivide_further()) {
    if(euclid && !bounded && gwhere == gEuclid) {
      printf("Euclidean -- full precision\n");
      stop = true; 
      }
    else {
      err_zero_current /= 2;
      printf("increasing precision to %lg\n", err_zero_current);
      for(auto p: points) enqueue(p);
      }
    return; 
    }
  printf("subdivide (%d,%d)\n", N, size(triangles));
  divides++;
  vector<triangle> otriangles = triangles;
  triangles.clear();
  
  halves.clear();

  // subdivide edges
  for(int i=0; i<N; i++) {
    rugpoint *m = points[i];
    for(int j=0; j<size(m->edges); j++) {
      rugpoint *m2 = m->edges[j].target;
      if(m2 < m) continue;
      rugpoint *mm = addRugpoint(mid(m->h, m2->h), (m->dist+m2->dist)/2);
      halves[make_pair(m, m2)] = mm;
      if(!good_shape) {
        using namespace hyperpoint_vec;
        normalizer n(m->flat, m2->flat);
        hyperpoint h1 = n(m->flat);
        hyperpoint h2 = n(m2->flat);
        mm->flat = n[mid(h1, h2)];
        }
      mm->valid = true; qvalid++;
      mm->inqueue = false; enqueue(mm);
      }
     m->edges.clear();
     }
    
  for(int i=0; i<size(otriangles); i++)
    addTriangle1(otriangles[i].m[0], otriangles[i].m[1], otriangles[i].m[2]);
    
  calcLengths();

  printf("result (%d,%d)\n", size(points), size(triangles));

  }

void addNewPoints() {

  if(torus || qvalid == size(points)) {
    subdivide();
    return;
    }
  
  double dist = hdist0(points[qvalid]->h) + .1e-6;
  
  int oqvalid = qvalid;

  for(int i=0; i<size(points); i++) {
    rugpoint& m = *points[i];
    bool wasvalid = m.valid;
    m.valid = wasvalid || sphere || hdist0(m.h) <= dist;
    if(m.valid && !wasvalid) {
      qvalid++;
      
      if(!good_shape) optimize(&m, i > 7);

      enqueue(&m);
      }
    }
  if(qvalid != oqvalid) { printf("adding new points %4d %4d %4d %.9lf %9d %9d\n", oqvalid, qvalid, size(points), dist, dt, queueiter); }
  }

void physics() {

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
      for(int j=0; j<size(m->edges); j++)
        moved = force(*m, *m->edges[j].target, m->edges[j].len) || moved;
      
      if(moved) enqueue(m);
      }    

  }

// drawing the Rug
//-----------------

int eyemod;

void getco(rugpoint *m, hyperpoint& h, int &spherepoints) {
  using namespace hyperpoint_vec;
  h = m->getglue()->flat;
  if(rug_perspective && gwhere >= gSphere) {
    if(h[2] > 0) {
      ld rad = hypot3(h);
      // turn M_PI to -M_PI
      // the only difference between sphere and elliptic is here:
      // in elliptic, we subtract PI from the distance
      ld rad_to = (gwhere == gSphere ? M_PI + M_PI : M_PI) - rad;
      ld r = -rad_to / rad;
      h *= r;
      spherepoints++;
      }
    }
  if(eyemod) h[0] += eyemod * h[2] * vid.eye;
  }

extern int besti;

/* these functions are for the ODS projection, used in VR videos */

void cyclefix(ld& a, ld b) {
  if(a > b + M_PI) a -= 2 * M_PI;
  if(a < b - M_PI) a += 2 * M_PI;
  }

ld raddif(ld a, ld b) {
  ld d = a-b;
  if(d < 0) d = -d;
  if(d > 2*M_PI) d -= 2*M_PI;
  if(d > M_PI) d = 2 * M_PI-d;
  return d;
  }

bool ods = false;
ld ipd = 0.05;

bool project_ods(hyperpoint azeq, hyperpoint& h1, hyperpoint& h2, bool eye) {
  ld tanalpha = tan(ipd/2);
  if(eye) tanalpha = -tanalpha;
  
  using namespace hyperpoint_vec;  
  ld d = hypot3(azeq);
  ld sindbd = sin(d)/d, cosd = cos(d);
  
  ld x = azeq[0] * sindbd;
  ld y = azeq[2] * sindbd;
  ld z = azeq[1] * sindbd;
  ld t = cosd;
  
//  printf("%10.5lf %10.5lf %10.5lf ", azeq[0], azeq[1], azeq[2]);
//  printf(" => %10.5lf %10.5lf %10.5lf %10.5lf", x, y, z, t);
  
  ld cottheta2 = (x*x + y*y - tanalpha*tanalpha*t*t) / (z*z);
//  if(cottheta2 < 0) printf(" BAD\n");
  if(cottheta2 < 0) return false;
  ld theta = atan(sqrt(1 / cottheta2));
  
  for(int i=0; i<2; i++) {
    hyperpoint& h = (i ? h1 : h2);
    if(i == 1) theta = -theta;
      
    ld x0 = t * tanalpha;
    ld y0 = -z / tan(theta);
    
    ld phi = atan2(y, x) - atan2(y0, x0);
      
    ld delta = atan2(z / sin(theta), t / cos(ipd/2));
    
    h[0] = phi;
    h[1] = theta;
    h[2] = delta;
//    printf(" => %10.5lf %10.5lf %10.5lf", phi, theta, delta);
    }
  
//  printf("\n");
  return true;
  }

void drawTriangle(triangle& t) {
  using namespace hyperpoint_vec;  
  for(int i: {0,1,2}) {
    if(!t.m[i]->valid) return;
    if(t.m[i]->dist >= sightrange+.51) return;
    }
  dt++;

  if(ods) {
    hyperpoint pts[3];
    for(int i=0; i<3; i++)
      pts[i] = t.m[i]->getglue()->flat;    

    hyperpoint hc = (pts[1] - pts[0]) ^ (pts[2] - pts[0]);  
    double hch = hypot3(hc);
    glNormal3f(hc[0]/hch,hc[1]/hch,hc[2]/hch);

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
      if(raddif(h[4][0], h[0][0]) < raddif(h[1][0], h[0][0]))
        swap(h[1], h[4]);
      if(abs(h[1][1] - h[0][1]) > M_PI/2) return;
      if(raddif(h[5][0], h[0][0]) < raddif(h[2][0], h[0][0]))
        swap(h[5], h[2]);
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
          glTexCoord2f(t.m[i]->x1, t.m[i]->y1);
          glVertex3f(h[s+i][0] + 2*M_PI*x, h[s+i][1], h[s+i][2]);
          }
        }
      }
    return;
    }
  
  int spherepoints = 0;
  array<hyperpoint,3> h;
  for(int i: {0,1,2}) getco(t.m[i], h[i], spherepoints);
  if(spherepoints == 1 || spherepoints == 2) return;
  
  hyperpoint hc = (h[1] - h[0]) ^ (h[2] - h[0]);  
  double hch = hypot3(hc);
  
  glNormal3f(hc[0]/hch,hc[1]/hch,hc[2]/hch);
  
  for(int i: {0,1,2}) {
    glTexCoord2f(t.m[i]->x1, t.m[i]->y1);
    glVertex3f(h[i][0], h[i][1], h[i][2]);
    }
  }

GLuint FramebufferName = 0;
GLuint renderedTexture = 0;
GLuint depth_stencil_rb = 0;

SDL_Surface *texture;
Uint32 *expanded_data;

void initTexture() {

  if(!rendernogl) {
#if !ISPANDORA
    FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, TEXTURESIZE, TEXTURESIZE, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
#ifdef TEX
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);  
#else
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);  
#endif
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    
    glGenRenderbuffers(1, &depth_stencil_rb);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, TEXTURESIZE, TEXTURESIZE);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_rb);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_rb);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      addMessage("Failed to initialize the framebuffer");
      rugged = false;
      }  
#endif
    }
  else {
    texture = SDL_CreateRGBSurface(SDL_SWSURFACE,TEXTURESIZE,TEXTURESIZE,32,0,0,0,0);  
    glGenTextures( 1, &renderedTexture );
    glBindTexture( GL_TEXTURE_2D, renderedTexture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    expanded_data = new Uint32[TEXTURESIZE * TEXTURESIZE];
    }
  }

void prepareTexture() {
  videopar svid = vid;
  
  setVidParam();
  
  if(rendernogl) {
    vid.usingGL = false;
    SDL_Surface *sav = s;
    s = texture;
    SDL_FillRect(s, NULL, 0);

    drawfullmap();
    s = sav;
    for(int y=0; y<TEXTURESIZE; y++) for(int x=0; x<TEXTURESIZE; x++)
      expanded_data[y*TEXTURESIZE + x] = qpixel(texture, x, TEXTURESIZE-1-y) | 0xFF000000;
    glBindTexture( GL_TEXTURE_2D, renderedTexture);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, TEXTURESIZE, TEXTURESIZE, 0, GL_BGRA, GL_UNSIGNED_BYTE, expanded_data );    
    }
  else { 
#if !ISPANDORA
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glViewport(0,0,TEXTURESIZE,TEXTURESIZE);
  
    setGLProjection();
    ptds.clear();
    drawthemap();
    if(mousing && !renderonce) {
      for(int i=0; i<numplayers(); i++) if(multi::playerActive(i))
        queueline(tC0(shmup::ggmatrix(playerpos(i))), mouseh, 0xFF00FF, 8);
      }
    if(finger_center) {
      transmatrix V = rgpushxto0(finger_center->h);
      queuechr(V, 0.5, 'X', 0xFFFFFFFF, 2);
      for(int i=0; i<72; i++)
        queueline(tC0(V * spin(i*M_PI/32) * xpush(finger_range)), tC0(V * spin((i+1)*M_PI/32) * xpush(finger_range)), 0xFFFFFFFF, 0);
      }
    drawqueue();  
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
    }
  vid = svid;
  if(!rendernogl) glViewport(0,0,vid.xres,vid.yres);
  }

void closeTexture() {
  if(rendernogl) {
    SDL_FreeSurface(texture);
    glDeleteTextures(1, &renderedTexture);
    delete[] expanded_data;
    }
  else {
#if !ISPANDORA
    glDeleteTextures(1, &renderedTexture);
    glDeleteRenderbuffers(1, &depth_stencil_rb);
    glDeleteFramebuffers(1, &FramebufferName);
#endif
    }
  }

double xview, yview;

void glcolorClear(int color) {
  unsigned char *c = (unsigned char*) (&color);
  glClearColor(c[3] / 255.0, c[2] / 255.0, c[1]/255.0, c[0] / 255.0);
  }

void drawRugScene() {
  GLfloat light_ambient[] = { 3.5, 3.5, 3.5, 1.0 };
  GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  GLERR("lighting");

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glBindTexture(GL_TEXTURE_2D, renderedTexture);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if(backcolor == 0) 
    glClearColor(0.05,0.05,0.05,1);
  else
    glcolorClear(backcolor << 8 | 0xFF);
  glClearDepth(1.0f); 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glDisable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  ld tanfov = tan(fov * M_PI / 360);
  
  if(ods) {
    glOrtho(-M_PI, M_PI, -M_PI, M_PI, 0, -M_PI * 2);
    }
  else if(rug_perspective) {
    ld vnear = .001;
    ld vfar = 1000;
    ld sca = vnear * tanfov / vid.xres;
    xview = -tanfov;
    yview = -tanfov * vid.yres / vid.xres;
    glFrustum(-sca * vid.xres, sca * vid.xres, -sca * vid.yres, sca * vid.yres, vnear, vfar);
    }
  else {
    xview = tanfov * model_distance;
    yview = tanfov * model_distance * vid.yres / vid.xres;

    glOrtho(-xview, xview, -yview, yview, -1000, 1000);
    }

  glColor4f(1.f, 1.f, 1.f, 1.f);

  if(rug_perspective && gwhere >= gSphere) {
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 0);
    glFogf(GL_FOG_END, gwhere == gSphere ? 10 : 4);
    }

  if(vid.eye > .001 || vid.eye < -.001) { 
    selectEyeMask(1);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    eyemod = 1;
    for(int t=0; t<size(triangles); t++)
      drawTriangle(triangles[t]);
    glEnd();
    selectEyeMask(-1);
    eyemod = -1;
    glClear(GL_DEPTH_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    for(int t=0; t<size(triangles); t++)
      drawTriangle(triangles[t]);
    glEnd();
    selectEyeMask(0);
    }
  else {
    glBegin(GL_TRIANGLES);
    for(int t=0; t<size(triangles); t++)
      drawTriangle(triangles[t]);
    glEnd();
    }

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glDisable(GL_FOG);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  selectEyeGL(0);
  
  need_mouseh = true;
  }

// organization
//--------------

transmatrix currentrot;
    
void init() {
#if CAP_GLEW
  if(!glew) { 
    glew = true; 
    GLenum err = glewInit();
    if (GLEW_OK != err) {
      addMessage("Failed to initialize GLEW");
      return;
      }
    }
#endif
  if(rugged) return;
  rugged = true;
  if(scale < .01 || scale > 100) scale = 1;
  initTexture();
  if(renderonce) prepareTexture();
  if(!rugged) return;
  
  genrug = true;
  drawthemap();
  genrug = false;
  
  qvalid = 0; dt = 0; queueiter = 0;
  err_zero_current = err_zero;
  
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
    
    if(!valid)
      gotoHelp(
        "Note: this mode is based on what you see on the screen -- but re-rendered in another way. "
        "If not everything is shown on the screen (e.g., too zoomed in), the results will be incorrect "
        "(though possibly interesting). "
        "Use a different projection to fix this."
        );
    }
  catch(rug_exception) {
    close();
    }
  }

void close() {
  if(!rugged) return;
  rugged = false;
  closeTexture();
  triangles.clear();
  for(int i=0; i<size(points); i++) delete points[i];
  points.clear();
  pqueue = queue<rugpoint*> ();
  finger_center = NULL;
  }

int lastticks;

ld protractor = 0;

void apply_rotation(const transmatrix& t) {
  if(!rug_perspective) currentrot = t * currentrot;
  for(auto p: points) p->flat = t * p->flat;
  }

bool handlekeys(int sym, int uni) {
  if(uni == '4') {
    ods = !ods;
    return true;
    }
  else if(uni == '1') {
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
    return true;
    }
  else if(uni == '2') {
    apply_rotation(rotmatrix(M_PI, 0, 2));
    return true;
    }
  else if(uni == '3') {
    apply_rotation(rotmatrix(M_PI/2, 0, 2));
    return true;
    }
  else return false;
  }

void finger_on(int coord, ld val) {
  for(auto p: points) {
    ld d = hdist(finger_center->h, p->getglue()->h);
    push_point(p->flat, coord, val * finger_force * exp( - sqr(d / finger_range)));
    }
  enqueue(finger_center), good_shape = false;
  }

void actDraw() { 
  try {
  if(!renderonce) prepareTexture();
  physics();
  drawRugScene();
  Uint8 *keystate = SDL_GetKeyState(NULL);
  int qm = 0;
  double alpha = (ticks - lastticks) / 1000.0;
  lastticks = ticks;

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
      if(keystate[SDLK_HOME]) qm++, t = t * rotmatrix(alpha, 0, 1), protractor += alpha;
      if(keystate[SDLK_END]) qm++, t = t * rotmatrix(alpha, 1, 0), protractor -= alpha;
      if(!keystate[SDLK_LSHIFT]) {
        if(keystate[SDLK_DOWN]) qm++, t = t * rotmatrix(alpha, 2, 1), protractor += alpha;
        if(keystate[SDLK_UP]) qm++, t =  t * rotmatrix(alpha, 1, 2), protractor -= alpha;
        if(keystate[SDLK_LEFT]) qm++, t = t * rotmatrix(alpha, 2, 0), protractor += alpha;
        if(keystate[SDLK_RIGHT]) qm++, t =  t * rotmatrix(alpha, 0, 2), protractor -= alpha;
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
    push_all_points(2, push);
    push_all_points(0, strafex);
    push_all_points(1, strafey);
    }
  else {
    if(finger_center)
      perform_finger();
    else {
      if(keystate[SDLK_HOME]) qm++, t = inverse(currentrot);
      if(keystate[SDLK_END]) qm++, t = currentrot * rotmatrix(alpha, 0, 1) * inverse(currentrot);
      if(keystate[SDLK_DOWN]) qm++, t = t * rotmatrix(alpha, 1, 2);
      if(keystate[SDLK_UP]) qm++, t =  t * rotmatrix(alpha, 2, 1);
      if(keystate[SDLK_LEFT]) qm++, t = t * rotmatrix(alpha, 0, 2);
      if(keystate[SDLK_RIGHT]) qm++, t =  t * rotmatrix(alpha, 2, 0);
      if(keystate[SDLK_PAGEUP]) model_distance /= exp(alpha);
      if(keystate[SDLK_PAGEDOWN]) model_distance *= exp(alpha);
      }
  
    if(qm) apply_rotation(t);
    }
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

hyperpoint gethyper(ld x, ld y) {
  double mx = ((x*2 / vid.xres)-1) * xview;
  double my = (1-(y*2 / vid.yres)) * yview;
  radar_distance = RADAR_INF;
  
  double rx1=0, ry1=0;
  
  bool found = false;
  
  for(int i=0; i<size(triangles); i++) {
    auto r0 = triangles[i].m[0];
    auto r1 = triangles[i].m[1];
    auto r2 = triangles[i].m[2];
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

  videopar svid = vid;
  setVidParam();
  hyperpoint h = ::gethyper(px, py);
  vid = svid;

  return h;
  }

void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);
  dialog::init(XLAT("hypersian rug mode"), iinf[itPalace].color, 150, 100);
  
  dialog::addItem(XLAT("what's this?"), 'h');
  dialog::addItem(XLAT("take me back"), 'q');

  dialog::addBoolItem(XLAT("enable the Hypersian Rug mode"), rug::rugged, 'u');
  
  dialog::addBoolItem(XLAT("render the texture only once"), (renderonce), 'o');
  dialog::addBoolItem(XLAT("render texture without OpenGL"), (rendernogl), 'g');
  dialog::addSelItem(XLAT("texture size"), its(texturesize)+"x"+its(texturesize), 's');

  dialog::addSelItem(XLAT("vertex limit"), its(vertex_limit), 'v');
  if(rug::rugged)
    dialog::lastItem().value += " (" + its(qvalid) + ")";
  
  dialog::addSelItem(XLAT("model distance"), fts(model_distance), 'd');
  dialog::addBoolItem(XLAT("projection"), rug_perspective, 'p');
  dialog::lastItem().value = XLAT(rug_perspective ? "perspective" : 
    gwhere == gEuclid ? "orthogonal" : "azimuthal equidistant");
  if(!rug::rugged)
    dialog::addSelItem(XLAT("native geometry"), XLAT(gwhere ? ginf[gwhere].name : "hyperbolic"), 'n');
  else
    dialog::addSelItem(XLAT("radar"), radar_distance == RADAR_INF ? "∞" : fts4(radar_distance), 'r');
  dialog::addSelItem(XLAT("model scale factor"), fts(modelscale), 'm');
  if(rug::rugged)
    dialog::addSelItem(XLAT("model iterations"), its(queueiter), 0);
  dialog::addSelItem(XLAT("field of view"), fts(fov) + "°", 'f');
  // dialog::addSelItem(XLAT("protractor"), fts(protractor * 180 / M_PI) + "°", 'f');
  if(!good_shape) {
    dialog::addSelItem(XLAT("maximum error"), ftsg(err_zero), 'e');
    if(rug::rugged)
      dialog::lastItem().value += " (" + ftsg(err_zero_current) + ")";
    }

  dialog::display();
  keyhandler = [] (int sym, int uni) {
  #if ISPANDORA
    rendernogl = true;
  #endif
    dialog::handleNavigation(sym, uni);

    if(uni == 'h') gotoHelp(
      XLAT(
      "In this mode, HyperRogue is played on a 3D model of a part of the hyperbolic plane, "
      "similar to one you get from the 'paper model creator' or by hyperbolic crocheting.\n\n"
      "This requires some OpenGL extensions and may crash or not work correctly -- enabling "
      "the 'render texture without OpenGL' options may be helpful in this case. Also the 'render once' option "
      "will make the rendering faster, but the surface will be rendered only once, so "
      "you won't be able to play a game on it.\n\n"
      "Use arrow keys to rotate, Page Up/Down to zoom.")
      + "\n\n" + 
      XLAT("In the perspective projection, you can use arrows to rotate the camera, Page Up/Down to go forward/backward, Shift+arrows to strafe, and Ctrl+arrows to rotate the model.")
      );
    else if(uni == 'u') {
      if(rug::rugged) rug::close();
      else rug::init();
      }
    else if(uni == 'I')
      dialog::editNumber(ipd, 0, 1, .002, .05, "interpupilar distance",
        "Used in the ODS projection."
        );
    else if(uni == 'R')
      dialog::editNumber(finger_range, 0, 1, .01, .1, "finger range",
        "Press 1 to enable the finger mode."
        );
    else if(uni == 'F')
      dialog::editNumber(finger_force, 0, 1, .01, .1, "finger force",
        "Press 1 to enable the finger force."
        );
    else if(uni == 'o' && !rug::rugged)
      renderonce = !renderonce;
    else if(uni == 'v') {
      dialog::editNumber(vertex_limit, 0, 50000, 500, 3000, "vertex limit", 
        "The more vertices, the more accurate the Hypersian Rug model is. "
        "However, a number too high might make the model slow to compute and render."
        );
      dialog::reaction = [] () { err_zero_current = err_zero; };
      }
    else if(uni == 'r') 
      addMessage(XLAT("This just shows the 'z' coordinate of the selected point."));
    else if(uni == 'm') {
      dialog::editNumber(modelscale, 0.1, 10, rugged ? .001 : .1, 1, "model scale factor", 
        "This is relevant when the native geometry is not Euclidean. "
        "For example, if the native geometry is spherical, and scale < 1, a 2d sphere will be rendered as a subsphere; "
        "if the native geometry is hyperbolic, and scale > 1, a hyperbolic plane will be rendered as an equidistant surface. "
        );
      dialog::scaleLog();
      if(rug::rugged) {
        static ld last;
        last = modelscale;
        dialog::reaction = [] () {
          for(auto p:points) {
            for(auto& e: p->edge) e.len *= modelscale / last;
            enqueue(p);
            }
          last = modelscale;
          good_shape = false;
          }
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
    else if(uni == 'd')
      dialog::editNumber(model_distance, -10, 10, .1, 1, "model distance", 
        "In the perspective projection, this sets the distance from the camera to the center of the model. "
        "In the orthogonal projection this just controls the scale."
        );
    else if(uni == 'e') {
      dialog::editNumber(err_zero, 1e-9, 1, .1, 1e-3, "maximum error", 
        "New points are added when the current error in the model is smaller than this value."
        );
      dialog::scaleLog();
      dialog::reaction = [] () { err_zero_current = err_zero; };
      }
    else if(uni == 'f') {
      dialog::editNumber(fov, 1, 170, 1, 45, "field of view", 
        "Horizontal field of view, in the perspective projection. "
        "In the orthogonal projection this just controls the scale."
        );
      }
    else if(uni == 'n' && !rug::rugged)
      gwhere = eGeometry((gwhere+1) % 4);
  #if !ISPANDORA
    else if(uni == 'g' && !rug::rugged)
      rendernogl = !rendernogl;
  #endif
    else if(uni == 's' && !rug::rugged) {
      texturesize *= 2;
      if(texturesize == 8192) texturesize = 128;
      }
    else if(handlekeys(sym, uni)) ;
    else if(doexiton(sym, uni)) popScreen();
    };
  }

void select() {
  pushScreen(rug::show);
  }

int rugArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-rugmodelscale")) {
    shift(); modelscale = argf();
    }

  else if(argis("-ruggeo")) {
    shift(); gwhere = (eGeometry) argi();
    }

  else if(argis("-rugpers")) {
    rug_perspective = true;
    }

  else if(argis("-rugorth")) {
    rug_perspective = false;
    }

  else if(argis("-rugerr")) {
    shift(); err_zero = argf();
    }

  else if(argis("-rugtsize")) {
    shift(); rug::texturesize = argi();
    }

  else if(argis("-rugv")) {
    shift(); vertex_limit = argi();
    }

  else if(argis("-ods")) {
    ods = true;
    }

  else if(argis("-ipd")) {
    shift(); ipd = argf();
    }

  else return 1;
  return 0;
  }

auto rug_hook = 
  addHook(hooks_args, 100, rugArgs);
  
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
