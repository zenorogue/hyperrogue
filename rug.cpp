// Hyperbolic Rogue
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

// implementation of the Hypersian Rug mode

#ifndef NORUG

#define TEXTURESIZE (texturesize)
#define HTEXTURESIZE (texturesize/2)

#ifdef LINUX
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

#ifdef MAC
#define glFramebufferTexture glFramebufferTextureEXT 
#endif

namespace rug {

// hypersian rug datatypes and globals
//-------------------------------------

bool rugged = false;
bool genrug = false;
bool glew   = false;

bool renderonce  = false;
bool rendernogl  = false;
int  texturesize = 1024;
double scale = 1;

int queueiter, qvalid, dt;
double err;

struct edge {
  struct rugpoint *target;
  double len;
  };

struct rugpoint {
  double x1, y1;
  bool valid;
  bool inqueue;
  double dist;
  hyperpoint h;
  hyperpoint flat;
  vector<edge> edges;
  };

struct triangle {
  rugpoint *m[3];
  triangle(rugpoint *m1, rugpoint *m2, rugpoint *m3) {
    m[0] = m1; m[1] = m2; m[2] = m3;
    }
  };

vector<rugpoint*> points;
vector<triangle> triangles;

// construct the graph
//---------------------

int hyprand;

rugpoint *addRugpoint(hyperpoint h, double dist) {
  rugpoint *m = new rugpoint;
  m->h = h;
  
  ld tz = vid.alphax+h[2];
  m->x1 = (1 + h[0] / tz) / 2;
  m->y1 = (1 + h[1] / tz) / 2;
  m->flat = // hpxyz(h[0], h[1], sin(atan2(h[0], h[1]) * 3 + hyprand) * (h[2]-1) / 1000);
    hpxyz(h[0], h[1], (h[2]-1) * (rand() % 1000 - rand() % 1000) / 1000);
  m->valid = false;
  m->inqueue = false;
  m->dist = dist;
  points.push_back(m);
  return m;
  }

rugpoint *findRugpoint(hyperpoint h, double dist) {
  for(int i=0; i<size(points); i++) 
    if(intval(points[i]->h, h) < 1e-5) return points[i];
  return addRugpoint(h, dist);
  }

void addNewEdge(rugpoint *e1, rugpoint *e2) {
  edge e; e.target = e2; e1->edges.push_back(e);
  e.target = e1; e2->edges.push_back(e);
  }

void addEdge(rugpoint *e1, rugpoint *e2) {
  for(int i=0; i<size(e1->edges); i++) 
    if(e1->edges[i].target == e2) return;
  addNewEdge(e1, e2);
  }

void addTriangle(rugpoint *t1, rugpoint *t2, rugpoint *t3) {
  addEdge(t1, t2); addEdge(t2, t3); addEdge(t3, t1);
  triangles.push_back(triangle(t1,t2,t3));
  }

void addTriangle1(rugpoint *t1, rugpoint *t2, rugpoint *t3) {
  rugpoint *t12 = findRugpoint(mid(t1->h, t2->h), (t1->dist+ t2->dist)/2);
  rugpoint *t23 = findRugpoint(mid(t2->h, t3->h), (t1->dist+ t2->dist)/2);
  rugpoint *t31 = findRugpoint(mid(t3->h, t1->h), (t1->dist+ t2->dist)/2);
  addTriangle(t1,  t12, t31);
  addTriangle(t12, t2,  t23);
  addTriangle(t23, t3,  t31);
  addTriangle(t23, t31, t12);
  }

bool psort(rugpoint *a, rugpoint *b) {
  return a->h[2] < b->h[2];
  }

void calcLengths() {
  for(int i=0; i<size(points); i++) for(int j=0; j<size(points[i]->edges); j++)
    points[i]->edges[j].len = hdist(points[i]->h, points[i]->edges[j].target->h);
  }

void buildRug() {

  map<cell*, rugpoint *> vptr;
 
  for(int i=0; i<size(dcal); i++)
    if(gmatrix.count(dcal[i])) 
      vptr[dcal[i]] = addRugpoint(gmatrix[dcal[i]]*C0, dcal[i]->cpdist);

  for(int i=0; i<size(dcal); i++) {
    cell *c = dcal[i];
    rugpoint *v = vptr[c];
    if(!v) continue;
    for(int j=0; j<c->type; j++) {
      cell *c2 = c->mov[j];
      rugpoint *w = vptr[c2];
      if(!w) continue;
      // if(v<w) addEdge(v, w);
      
      cell *c3 = c->mov[(j+1) % c->type];
      rugpoint *w2 = vptr[c3];
      if(!w2) continue;
      if(c->type == 7) addTriangle(v, w, w2);
      }
    }

  printf("vertices = %d triangles=  %d\n", size(points), size(triangles));

  calcLengths();
  sort(points.begin(), points.end(), psort);
  }

// rug physics

queue<rugpoint*> pqueue;
void enqueue(rugpoint *m) {
  if(m->inqueue) return;
  pqueue.push(m);
  m->inqueue = true;
  }

void force(rugpoint& m1, rugpoint& m2, double rd, double d1=1, double d2=1) {
  if(!m1.valid || !m2.valid) return;
  // double rd = hdist(m1.h, m2.h) * xd;
  // if(rd > rdz +1e-6 || rd< rdz-1e-6) printf("%lf %lf\n", rd, rdz);
  double t = 0;
  for(int i=0; i<3; i++) t += (m1.flat[i] - m2.flat[i]) * (m1.flat[i] - m2.flat[i]);
  t = sqrt(t);
  // printf("%lf %lf\n", t, rd);
  err += (t-rd) * (t-rd);
  bool nonzero = t < rd-1e-9 || t > rd+1e-9;
  double force = (t - rd) / t / 2; // 20.0;
  for(int i=0; i<3; i++) {
    double di = (m2.flat[i] - m1.flat[i]) * force;
    m1.flat[i] += di * d1;
    m2.flat[i] -= di * d2;
    if(nonzero && d2>0) enqueue(&m2);
    }  
  }

void preset(rugpoint *m) {
  int q = 0;
  hyperpoint h;
  for(int i=0; i<3; i++) h[i] = 0;
  using namespace hyperpoint_vec;
  
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
      double ch = sqrt(c2*c2 - cz*cz);

      double az = (a1*a1-a2*a2+1) / 2;
      double ah = sqrt(a2*a2 - az*az);
      
      // c->h = a->h + (b->h-a->h) * cz + ch * ort
      hyperpoint ort = (c->flat - a->flat - cz * (b->flat-a->flat)) / ch;

      // m->h = a->h + (b->h-a->h) * az - ah * ort
      hyperpoint res = a->flat + (b->flat-a->flat) * az - ah * ort;
      
      for(int i=0; i<3; i++) h[i] += res[i];
      
      q++;
      }
    }
    
  if(q>0) for(int i=0; i<3; i++) m->flat[i] = h[i]/q;
  }

int divides = 0;
bool stop = false;

void subdivide() {
  int N = size(points);
  if(divides > 4) {stop = true; return; }
  printf("subdivide (%d,%d)\n", N, size(triangles));
  divides++;
  vector<triangle> otriangles = triangles;
  triangles.clear();
  
  // subdivide edges
  for(int i=0; i<N; i++) {
    rugpoint *m = points[i];
    for(int j=0; j<size(m->edges); j++) {
      rugpoint *m2 = m->edges[j].target;
      rugpoint *mm = addRugpoint(mid(m->h, m2->h), (m->dist+m2->dist)/2);
      using namespace hyperpoint_vec;
      mm->flat = (m->flat + m2->flat) / 2;
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

  if(qvalid == size(points)) {
    subdivide();
    return;
    }
  
  double dist = points[qvalid]->h[2] + .1e-6;
  
  int oqvalid = qvalid;

  for(int i=0; i<size(points); i++) {
    rugpoint& m = *points[i];
    bool wasvalid = m.valid;
    m.valid = wasvalid || (m.h[2] >= .5 && m.h[2] < dist);
    if(m.valid && !wasvalid) {
      qvalid++;
      if(i > 7) preset(&m);
      
      for(int it=0; it<50; it++) 
      for(int j=0; j<size(m.edges); j++)
        force(m, *m.edges[j].target, m.edges[j].len, 1, 0);

      enqueue(&m);
      }
    }
  if(qvalid != oqvalid) { printf("%4d %4d %4d %.9lf %9d %9d\n", oqvalid, qvalid, size(points), dist, dt, queueiter); }
  }

void physics() {

  for(int it=0; it<10000 && !stop; it++) 
    if(pqueue.empty()) addNewPoints();
    else {
      queueiter++;
      rugpoint *m = pqueue.front();
      pqueue.pop();
      m->inqueue = false;
      for(int j=0; j<size(m->edges); j++)
        force(*m, *m->edges[j].target, m->edges[j].len);      
      }
  }

// drawing the Rug
//-----------------

int eyemod;

void getco(rugpoint& m, double& x, double& y, double& z) {
  x = m.flat[0];
  y = m.flat[1];
  z = m.flat[2];
  if(eyemod) x += eyemod * z * vid.eye;
  }

extern int besti;

void drawTriangle(triangle& t) {
  rugpoint& m1 = *t.m[0];
  rugpoint& m2 = *t.m[1];
  rugpoint& m3 = *t.m[2];
  if(!m1.valid || !m2.valid || !m3.valid) return;
  if(m1.dist >= sightrange+.51 || m2.dist >= sightrange+.51 || m3.dist >= sightrange+.51)
    return;
  dt++;
  double x1, y1, z1;
  double x2, y2, z2;
  double x3, y3, z3;
  getco(m1,x1,y1,z1);
  getco(m2,x2,y2,z2);
  getco(m3,x3,y3,z3);
  
  double xa = x2-x1, ya = y2-y1, za = z2-z1;
  double xb = x3-x1, yb = y3-y1, zb = z3-z1;
  
  double xn = ya * zb - za * yb;
  double yn = za * xb - xa * zb;
  double zn = xa * yb - ya * xb;
  double h = sqrt(xn*xn+yn*yn+zn*zn);
  
  glNormal3f(xn/h,yn/h,zn/h);
  
  glTexCoord2f(m1.x1, m1.y1); 
  glVertex3f(x1, y1, z1);
  glTexCoord2f(m2.x1, m2.y1); 
  glVertex3f(x2, y2, z2);
  glTexCoord2f(m3.x1, m3.y1); 
  glVertex3f(x3, y3, z3);
  }

void setVidParam() {
  vid.xres = vid.yres = TEXTURESIZE; vid.scale = 1; 
  vid.radius = HTEXTURESIZE; vid.xcenter = HTEXTURESIZE; vid.ycenter = HTEXTURESIZE;
  vid.beta = 2; vid.alphax = 1; vid.eye = 0; vid.goteyes = false;
  }

GLuint FramebufferName = 0;
GLuint renderedTexture = 0;
GLuint depth_stencil_rb = 0;

SDL_Surface *texture;
Uint32 *expanded_data;

void initTexture() {

  if(!rendernogl) {
#ifndef PANDORA
    FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, TEXTURESIZE, TEXTURESIZE, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);  
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
#ifndef PANDORA
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glViewport(0,0,TEXTURESIZE,TEXTURESIZE);
  
    setGLProjection();
    ptds.clear();
    drawthemap();
    if(!renderonce) queueline(C0, mouseh, 0xFF00FF);
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
#ifndef PANDORA
    glDeleteTextures(1, &renderedTexture);
    glDeleteRenderbuffers(1, &depth_stencil_rb);
    glDeleteFramebuffers(1, &FramebufferName);
#endif
    }
  }

double xview, yview;

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

  glClearColor(0.05,0.05,0.05,1);
  glClearDepth(1.0f); 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glDisable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  xview = vid.xres/(vid.radius*scale);
  yview = vid.yres/(vid.radius*scale);

  glOrtho(-xview, xview, -yview, yview, -1000, 1000);

  glColor4f(1,1,1,1);
  
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

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  selectEyeGL(0);
  }

// organization
//--------------

transmatrix rotmatrix(double rotation, int c0, int c1) {
  transmatrix t = Id;
  t[c0][c0] = cos(rotation);
  t[c1][c1] = cos(rotation);
  t[c0][c1] = sin(rotation);
  t[c1][c0] = -sin(rotation);
  return t;
  }

transmatrix currentrot;
    
void init() {
#ifdef WINDOWS
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
  
  buildRug();
  qvalid = 0; dt = 0; queueiter = 0;
  
  currentrot = Id;
  }

void close() {
  if(!rugged) return;
  rugged = false;
  closeTexture();
  triangles.clear();
  for(int i=0; i<size(points); i++) delete points[i];
  points.clear();
  pqueue = queue<rugpoint*> ();
  }

int lastticks;

void actDraw() { 
  if(!renderonce) prepareTexture();
  physics();
  drawRugScene();
  Uint8 *keystate = SDL_GetKeyState(NULL);
  int qm = 0;
  transmatrix t = Id;
  double alpha = (ticks - lastticks) / 1000.0;
  lastticks = ticks;

  if(keystate[SDLK_HOME]) qm++, t = inverse(currentrot);
  if(keystate[SDLK_END]) qm++, t = currentrot * rotmatrix(alpha, 0, 1) * inverse(currentrot);
  if(keystate[SDLK_DOWN]) qm++, t = t * rotmatrix(alpha, 1, 2);
  if(keystate[SDLK_UP]) qm++, t =  t * rotmatrix(alpha, 2, 1);
  if(keystate[SDLK_LEFT]) qm++, t = t * rotmatrix(alpha, 0, 2);
  if(keystate[SDLK_RIGHT]) qm++, t =  t * rotmatrix(alpha, 2, 0);
  if(keystate[SDLK_PAGEUP]) scale *= exp(alpha);
  if(keystate[SDLK_PAGEDOWN]) scale /= exp(alpha);

  if(qm) {
    currentrot = t * currentrot;
    for(int i=0; i<size(points); i++) points[i]->flat = t * points[i]->flat;
    }
  }

int besti;

hyperpoint gethyper(ld x, ld y) {
  double mx = ((x*2 / vid.xres)-1) * xview;
  double my = (1-(y*2 / vid.yres)) * yview;
  double bdist = 1e12;
  besti = 0;
  
  for(int i=0; i<size(points); i++) {
    rugpoint& m = *points[i];
    double dist = hypot(m.flat[0]-mx, m.flat[1]-my);
    if(dist < bdist) bdist = dist, besti = i;
    }
  
  double px = points[besti]->x1 * TEXTURESIZE, py = (1-points[besti]->y1) * TEXTURESIZE;

  videopar svid = vid;
  setVidParam();
  hyperpoint h = ::gethyper(px, py);
  vid = svid;

  return h;
  }

void show() {
  dialog::init(XLAT("hypersian rug mode"), iinf[itPalace].color, 150, 100);

  dialog::addItem(XLAT("what's this?"), 'h');
  dialog::addItem(XLAT("take me back"), 'q');
  dialog::addItem(XLAT("enable the Hypersian Rug mode"), 'u');
  dialog::addBoolItem(XLAT("render the texture only once"), (renderonce), 'o');
  dialog::addBoolItem(XLAT("render texture without OpenGL"), (rendernogl), 'g');  
  dialog::addSelItem(XLAT("texture size"), its(texturesize)+"x"+its(texturesize), 's');
  dialog::display();
  }

void handleKey(int sym, int uni) {
#ifdef PANDORA
  rendernogl = true;
#endif
  dialog::handleNavigation(sym, uni);

  if(uni == 'h') {
    lastmode = cmode;
    cmode = emHelp;
    help = 
      "In this mode, HyperRogue is played on a 3D model of a part of the hyperbolic plane, "
      "similar to one you get from the 'paper model creator' or by hyperbolic crocheting.\n\n"
      "This requires some OpenGL extensions and may crash or not work correctly -- enabling "
      "the 'render texture without OpenGL' options may be helpful in this case. Also the 'render once' option "
      "will make the rendering faster, but the surface will be rendered only once, so "
      "you won't be able to play a game on it.\n\n"
      "Use arrow keys to rotate, Page Up/Down to zoom.";
    }
  else if(uni == 'u') {
    if(sphere) restartGame('E');
    if(euclid) restartGame('e');
    rug::init();
    cmode = emNormal;
    }
  else if(uni == 'o')
    renderonce = !renderonce;
#ifndef PANDORA
  else if(uni == 'g')
    rendernogl = !rendernogl;
#endif
  else if(uni == 's') {
    texturesize *= 2;
    if(texturesize == 8192) texturesize = 128;
    dialog::scaleLog();
    }
  else if(uni || sym == SDLK_F10)
    cmode = emChangeMode;
  }

void select() {
  if(rug::rugged) rug::close();
  else cmode = emRugConfig;
  }
  
}

#else

// fake for mobile
namespace rug {
    bool rugged = false;
    bool renderonce = false;
    bool rendernogl = true;
    int texturesize = 512;
    double scale = 1.0f;
}

#endif
