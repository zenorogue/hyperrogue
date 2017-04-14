// HyperRogue, shapes used for the vector graphics
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

#ifdef GFX
#ifndef MOBILE
#include <SDL/SDL_gfxPrimitives.h>
#endif
#endif

#ifdef WINDOWS
#include <GL/glew.h>
#else

#define GL_GLEXT_PROTOTYPES 1
#ifndef MOBILE
#ifdef GL
#ifdef MAC
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#ifdef MAC
#include <OpenGL/glext.h>
#else
#include <GL/glext.h>
#endif
#endif
#endif

#ifdef ANDROID
#ifndef FAKE
#ifdef GL
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES2/gl2.h>
#endif
#endif
#endif
#endif

#define QHPC 32000

int qhpc, prehpc;

hyperpoint hpc[QHPC];

bool first;

bool fatborder;

#define PSHIFT 0
// #define STLSORT

#define NEWSHAPE (-13.5)
#define WOLF (-15.5)

extern long double polydata[];

struct hpcshape {
  int s, e, prio;
  };

hpcshape *last = NULL;

struct qpoly {
      transmatrix V;
      GLfloat *tab;
      int curveindex;
      int cnt;
      int outline;
      };

struct qline {
      hyperpoint H1, H2;
      int prf;
      };
      
#define MAXQCHR 16

struct qchr {
      char str[MAXQCHR];
      int x, y, shift, size, frame;
      int align;
      };
      
struct qcir {
      int x, y, size;
      };

enum eKind { pkPoly, pkLine, pkString, pkCircle, pkShape, pkResetModel };

struct polytodraw {
  eKind kind;
  int prio, col;
  union {
    qpoly  poly;
    qline  line;
    qchr   chr;
    qcir   cir;
    } u;
#ifdef ROGUEVIZ
  string* info;
  polytodraw() { info = NULL; }
#endif
  };

vector<polytodraw> ptds;

polytodraw& lastptd() { return ptds[size(ptds)-1]; }

polytodraw& nextptd() { ptds.resize(size(ptds)+1); return lastptd(); }

bool ptdsort(const polytodraw& p1, const polytodraw& p2) {
  return p1.prio < p2.prio;
  }

void hpcpush(hyperpoint h) { 
  if(sphere) h = mid(h,h);
  if(vid.usingGL && !first && intval(hpc[qhpc-1], h) > (sphere ? .01 : 0.25)) {
    hyperpoint md = mid(hpc[qhpc-1], h);
    hpcpush(md);
    }
  first = false;
  hpc[qhpc++] = h;
  }

#define SIDE_SLEV 0
#define SIDE_WALL 3
#define SIDE_LAKE 4
#define SIDE_LTOB 5
#define SIDE_BTOI 6
#define SIDE_WTS3 7
#define SIDEPARS 8

bool validsidepar[SIDEPARS];

void chasmifyPoly(double fac, double fac2, int k) {
  for(int i=qhpc-1; i >= last->s; i--) {
    hyperpoint H;
    for(int j=0; j<3; j++) {
      H[j] = hpc[i][j] * fac;
      hpc[i][j] *= fac2;
      }
    hpc[qhpc++] = H;
    }
  hpc[qhpc++] = hpc[last->s];
  }

void shift(hpcshape& sh, double dx, double dy, double dz) {
  hyperpoint H = hpxyz(dx, dy, dz);
  transmatrix m = rgpushxto0(H);
  for(int i=sh.s; i<sh.e; i++) 
    hpc[i] = m * hpc[i];
  }

#ifndef MOBILE
SDL_Surface *aux;
#endif

vector<polytodraw*> ptds2;

#ifdef GL
#define USEPOLY

GLuint shapebuffer;

#define POLYMAX 60000
GLfloat glcoords[POLYMAX][3];
int qglcoords;

extern void glcolor(int color);
#endif

GLfloat *currentvertices;

void activateVertexArray(GLfloat *f, int qty) {
  currentvertices = f;
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, f);
  }


GLfloat *ourshape = NULL;

void initPolyForGL() {
  
  if(ourshape) delete[] ourshape;
  ourshape = new GLfloat[3 * qhpc];
  
  // GLfloat ourshape[3*qhpc];
  
  int id = 0;
  for(int i=0; i<qhpc; i++) {
    ourshape[id++] = hpc[i][0];
    ourshape[id++] = hpc[i][1];
    ourshape[id++] = hpc[i][2];
    }
  
  currentvertices = NULL;
  }

void activateShapes() {
  if(currentvertices != ourshape) {
    activateVertexArray(ourshape, qhpc);
    }
  }

void activateGlcoords() {
  activateVertexArray(glcoords[0], qglcoords);
  }


#ifdef GFX
#define POLYMAX 60000
#define USEPOLY
#endif

int polyi;

int polyx[POLYMAX], polyxr[POLYMAX], polyy[POLYMAX];

hyperpoint gltopoint(GLfloat t[3]) {
  hyperpoint h;
  h[0] = t[0]; h[1] = t[1]; h[2] = t[2];
  return h;
  }

void addpoint(const hyperpoint& H) {
  if(vid.usingGL) {
    if(polyi >= POLYMAX) return;
    if(pmodel) {
      hyperpoint Hscr;
      applymodel(H, Hscr);
      glcoords[qglcoords][0] = Hscr[0] * vid.radius;
      glcoords[qglcoords][1] = Hscr[1] * vid.radius;
      glcoords[qglcoords][2] = Hscr[2] * vid.radius;
      }
    else if(euclid) {
      for(int i=0; i<3; i++) glcoords[qglcoords][i] = H[i];
  
      glcoords[qglcoords][2] *= EUCSCALE;
      glcoords[qglcoords][2] += vid.alpha;
      // glcoords[qglcoords][2] = 1; // EUCSCALE;
      }
    else {
      for(int i=0; i<3; i++) glcoords[qglcoords][i] = H[i];
      glcoords[qglcoords][2] += vid.alpha;
      }
    qglcoords++;
    }
  else {
    if(polyi >= POLYMAX) return;
    hyperpoint Hscr;
    applymodel(H, Hscr);
    ld x = vid.xcenter + Hscr[0] * vid.radius;
    ld y = vid.ycenter + Hscr[1] * vid.radius;

    if(x < -vid.xres) x = -vid.xres;
    if(y < -vid.yres) y = -vid.yres;
    if(x > 2*vid.xres) x = 2*vid.xres;
    if(y > 2*vid.yres) y = 2*vid.yres;

    ld xe = Hscr[2] * vid.radius * vid.eye;
    polyx[polyi] = x-xe;
    polyxr[polyi] = x+xe;
    polyy[polyi] = y;
    polyi++;
    }
  }

void addpoly(const transmatrix& V, GLfloat *tab, int cnt) {
  while(cnt--) {
    addpoint(V*gltopoint(tab));
    tab += 3;
    }
  }

#ifdef GFX
#ifndef MOBILE
void aapolylineColor(SDL_Surface *s, int*x, int *y, int polyi, int col) {
  for(int i=1; i<polyi; i++)
    aalineColor(s, x[i-1], y[i-1], x[i], y[i], col);
  }

void polylineColor(SDL_Surface *s, int *x, int *y, int polyi, int col) {
  for(int i=1; i<polyi; i++)
    lineColor(s, x[i-1], y[i-1], x[i], y[i], col);
  }

void filledPolygonColorI(SDL_Surface *s, int* polyx, int *polyy, int polyi, int col) {
  Sint16 spolyx[polyi], spolyy[polyi];
  for(int i=0; i<polyi; i++) spolyx[i] = polyx[i], spolyy[i] = polyy[i];
  filledPolygonColor(s, spolyx, spolyy, polyi, col);
  }

#endif
#endif

void glcolor2(int color) {
  unsigned char *c = (unsigned char*) (&color);
  glColor4f(c[3] / 255.0, c[2] / 255.0, c[1]/255.0, c[0] / 255.0);
  }

void glapplymatrix(const transmatrix& V) {
  GLfloat mat[16];
  int id = 0;
  
  for(int y=0; y<3; y++) {
    for(int x=0; x<3; x++) mat[id++] = V[x][y];
    mat[id++] = 0;
    }
  {for(int x=0; x<3; x++) mat[id++] = 0;} mat[id++] = 1;
  
  if(euclid) {
    /* mat[2] = 0; mat[6] = 0; mat[10] = -zz; 
    mat[12] = mat[8]; mat[13] = mat[9]; mat[14] = EUCSCALE+zz; */
    // mat[8] = mat[9] = 0;
    // mat[11] = -EUCSCALE;
    for(int i=2; i<16; i+=4) mat[i] *= EUCSCALE;
    }
  
  mat[14] = GLfloat(vid.alpha);

  if(mat[10] + vid.alpha < 0 && !sphere && !euclid) {
    for(int i=0; i<3; i++)
    for(int j=0; j<16; j+=4)
      mat[j+i] = -mat[j+i];
    }
  
  glMultMatrixf(mat);
  }

void gldraw(int useV, const transmatrix& V, int ps, int pq, int col, int outline) {
  for(int ed = vid.goteyes ? -1 : 0; ed<2; ed+=2) {
    if(ed) selectEyeGL(ed);
    
    if(useV == 1) {
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glapplymatrix(V);
      }
      
    if(useV == 2) {
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      GLfloat mat[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
        };
      mat[8] += ed * vid.eye;
      glMultMatrixf(mat);
      }
      
    if(col) {
      glEnable(GL_STENCIL_TEST);
 
      glColorMask( GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE );
      glStencilOp( GL_INVERT, GL_INVERT, GL_INVERT);
      glStencilFunc( GL_ALWAYS, 0x1, 0x1 );
      glColor4f(1,1,1,1);
      glDrawArrays(GL_TRIANGLE_FAN, ps, pq);
 
      selectEyeMask(ed);
 
      glcolor2(col);
      glStencilOp( GL_ZERO, GL_ZERO, GL_ZERO);
      glStencilFunc( GL_EQUAL, 1, 1);
      glDrawArrays(GL_TRIANGLE_FAN, ps, pq);
      glDisable(GL_STENCIL_TEST);
      }
    
    if(outline) {
      glcolor2(outline);
      glDrawArrays(GL_LINE_STRIP, ps, pq);
      }
 
    if(useV) glPopMatrix();
    }
  }

void drawpolyline(const transmatrix& V, GLfloat* tab, int cnt, int col, int outline) {
#ifdef GL
  if(vid.usingGL) {
    if(pmodel == mdDisk) {    
      const int pq = cnt;
      if(currentvertices != tab)
        activateVertexArray(tab, pq);
      const int ps=0;
      gldraw(1, V, ps, pq, col, outline);    
      }
    else {
      qglcoords = 0;
      addpoly(V, tab, cnt);
      activateGlcoords();    
      gldraw(2, Id, 0, qglcoords, col, outline);
      }
    return;
    }
#endif
  
  polyi = 0;
  addpoly(V, tab, cnt);

#ifdef MOBILE

#ifdef ANDROID
#define ANDROIDGD
#endif

#ifdef FAKEMOBILE
#define ANDROIDGD
#endif

#ifdef ANDROIDGD
  gdpush(1); gdpush(col); gdpush(outline); gdpush(polyi);
  for(int i=0; i<polyi; i++) gdpush(polyx[i]), gdpush(polyy[i]);
#endif

#else

#ifdef GFX
  filledPolygonColorI(s, polyx, polyy, polyi, col);
#ifndef MOBILE
  if(svg::in) svg::polygon(polyx, polyy, polyi, col, outline);
#endif
  if(vid.goteyes) filledPolygonColorI(aux, polyxr, polyy, polyi, col);
  
  (vid.usingAA?aapolylineColor:polylineColor)(s, polyx, polyy, polyi, outline);
  if(vid.goteyes) aapolylineColor(aux, polyxr, polyy, polyi, outline);
  
  if(vid.xres >= 2000 || fatborder) {
    int xmi = 3000, xma = -3000;
    for(int t=0; t<polyi; t++) xmi = min(xmi, polyx[t]), xma = max(xma, polyx[t]);
    
    if(xma > xmi + 20) for(int x=-1; x<2; x++) for(int y=-1; y<=2; y++) if(x*x+y*y == 1) {
      for(int t=0; t<polyi; t++) polyx[t] += x, polyy[t] += y;
      aapolylineColor(s, polyx, polyy, polyi, outline);
      for(int t=0; t<polyi; t++) polyx[t] -= x, polyy[t] -= y;
      }
    }
#endif
#endif
  }

vector<float> prettylinepoints;

void prettypoint(const hyperpoint& h) {
  for(int i=0; i<3; i++) prettylinepoints.push_back(h[i]);
  }

void prettylinesub(const hyperpoint& h1, const hyperpoint& h2, int lev) {
  if(lev) {
    hyperpoint h3 = midz(h1, h2);
    prettylinesub(h1, h3, lev-1);
    prettylinesub(h3, h2, lev-1);
    }
  else prettypoint(h2);
  }

void prettyline(hyperpoint h1, hyperpoint h2, int col, int lev) {
  prettylinepoints.clear();
  prettypoint(h1);
  prettylinesub(h1, h2, lev);
  drawpolyline(Id, &prettylinepoints[0], size(prettylinepoints)/3, 0, col);
  }

vector<GLfloat> curvedata;
int curvestart = 0;

void drawqueue() {
#ifdef USEPOLY

  int siz = size(ptds);

  setcameraangle(true);

#ifdef GL
  if(vid.usingGL) 
    glClear(GL_STENCIL_BUFFER_BIT);
#endif
  
  profile_start(3);
#ifdef STLSORT
  sort(ptds.begin(), ptds.end(), ptdsort);
  
#else
  
  int qp[PPR_MAX];
  for(int a=0; a<PPR_MAX; a++) qp[a] = 0;
  
  for(int i = 0; i<siz; i++) {
    if(ptds[i].prio < 0 || ptds[i].prio >= PPR_MAX) {
      printf("Illegal priority %d of kind %d\n", ptds[i].prio, ptds[i].kind);
      ptds[i].prio = rand() % PPR_MAX;
      }
    qp[ptds[i].prio]++;
    }
  
  int total = 0;
  for(int a=0; a<PPR_MAX; a++) {
    int b = qp[a];
    qp[a] = total; total += b;
    }
  
  ptds2.resize(siz);
  
  for(int i = 0; i<siz; i++) ptds2[qp[ptds[i].prio]++] = &ptds[i];

#endif
  profile_stop(3);

#ifndef MOBILE
  if(vid.goteyes && !vid.usingGL) {

    if(aux && (aux->w != s->w || aux->h != s->h))
      SDL_FreeSurface(aux);
  
    if(!aux) {
      aux = SDL_CreateRGBSurface(SDL_SWSURFACE,s->w,s->h,32,0,0,0,0);
      }

    // SDL_LockSurface(aux);
    // memset(aux->pixels, 0, vid.xres * vid.yres * 4);
    // SDL_UnlockSurface(aux);
    SDL_BlitSurface(s, NULL, aux, NULL);
    }
#endif
  
  for(int i=0; i<siz; i++) {

#ifdef STLSORT
    polytodraw& ptd (ptds[i]);
#else
    polytodraw& ptd (*ptds2[i]);
#endif

#ifdef ROGUEVIZ
    svg::info = ptd.info;
#endif

    // if(ptd.prio == 46) printf("eye size %d\n", polyi);
    
    if(ptd.kind == pkResetModel) {
      pmodel = eModel(ptd.col);
      continue;
      }

    if(ptd.kind == pkPoly) {
      if(ptd.u.poly.curveindex >= 0)
        ptd.u.poly.tab = &curvedata[ptd.u.poly.curveindex];
      drawpolyline(ptd.u.poly.V, ptd.u.poly.tab, ptd.u.poly.cnt, ptd.col, ptd.u.poly.outline);
      }
    else if(ptd.kind == pkLine) {
      prettyline(ptd.u.line.H1, ptd.u.line.H2, ptd.col, ptd.u.line.prf);
      }
    else if(ptd.kind == pkString) {
      qchr& q(ptd.u.chr);
#ifndef MOBILE
      if(svg::in) 
        svg::text(q.x, q.y, q.size, q.str, q.frame, ptd.col, q.align);
      else {
        if(q.frame) {
          displaystr(q.x-q.frame, q.y, q.shift, q.size, q.str, 0, q.align);
          displaystr(q.x+q.frame, q.y, q.shift, q.size, q.str, 0, q.align);
          displaystr(q.x, q.y-q.frame, q.shift, q.size, q.str, 0, q.align);
          displaystr(q.x, q.y+q.frame, q.shift, q.size, q.str, 0, q.align);
          }
        displaystr(q.x, q.y, q.shift, q.size, q.str, ptd.col, q.align);
        }
#else
      displayfr(q.x, q.y, q.frame, q.size, q.str, ptd.col, q.align);
#endif
      }
    else if(ptd.kind == pkCircle) {
#ifndef MOBILE
      if(svg::in) 
        svg::circle(ptd.u.cir.x, ptd.u.cir.y, ptd.u.cir.size, ptd.col);
      else
#endif
      drawCircle(ptd.u.cir.x, ptd.u.cir.y, ptd.u.cir.size, ptd.col);
      }
    }

#ifndef MOBILE  
  if(vid.goteyes && !vid.usingGL) {
    int qty = s->w * s->h;
    int *a = (int*) s->pixels;
    int *b = (int*) aux->pixels;
    SDL_LockSurface(aux);
    while(qty) {
      *a = ((*a) & 0xFF0000) | ((*b) & 0x00FFFF);
      a++; b++; qty--;
      }
    SDL_UnlockSurface(aux);
    }

  if(vid.goteyes && vid.usingGL) selectEyeGL(0), selectEyeMask(0);

#endif
#endif

  setcameraangle(false);
  curvedata.clear(); curvestart = 0;
  }

hpcshape 
  shFloorSide[SIDEPARS][2], shSemiFloorSide[SIDEPARS], shTriheptaSide[SIDEPARS][2], shMFloorSide[SIDEPARS][2],
  shFloor[2], shBFloor[2], shMFloor2[2], shMFloor3[2], shMFloor4[2],
  shCircleFloor,
  shFloorShadow[2], shTriheptaFloorShadow[2], shTriheptaEucShadow[3],
  shWall[2], shMineMark[2], shFan,
  shStarFloor[2], shCloudFloor[2], shTriFloor[2], shZebra[5],
  shButterflyFloor[2],
  shTower[11],
  shTurtleFloor[3], shDragonFloor[3], shRoseFloor[3],
  shChargedFloor[4], shSStarFloor[2], shOverFloor[3],
  shEmeraldFloor[6],
  shFeatherFloor[3], shDemonFloor[2], shCrossFloor[2], shMFloor[2], shCaveFloor[3],
  shSemiFeatherFloor[2], shPowerFloor[3],
  shSemiFloor[2], shSemiBFloor[2], shSemiFloorShadow,
  shDesertFloor[2], shRedRockFloor[3][2],
  shPalaceFloor[2], shNewFloor[2], shTrollFloor[2],
  shLeafFloor[2],
  shBarrowFloor[3],
  shTriheptaFloor[11], shTriheptaFloor2[2], shTriheptaEuc[3],
  shCross, shGiantStar[2], shLake, shMirror,
  shGem[2], shStar, shDisk, shDiskT, shDiskS, shDiskM, shDiskSq, shRing,   
  shEgg,
  shSpikedRing, shTargetRing, shSawRing, shGearRing, shPeaceRing, shHeptaRing,
  shSpearRing,
  shDaisy, shTriangle, shNecro, shStatue, shKey,
  shGun,
  shFigurine, shTreat,
  shElementalShard,
  // shBranch, 
  shIBranch, shTentacle, shTentacleX, shILeaf[2], 
  shMovestar,
  shWolf, shYeti, shDemon, shGDemon, shEagle, shGargoyleWings, shGargoyleBody,
  shDogBody, shDogHead, shDogFrontLeg, shDogRearLeg, shDogFrontPaw, shDogRearPaw,
  shDogTorso,
  shHawk,
  shCatBody, shCatLegs, shCatHead, shFamiliarHead, shFamiliarEye,
  shWolf1, shWolf2, shWolf3,
  shDogStripes,
  shPBody, shPSword, shPKnife,
  shHumanFoot, shHumanLeg, shHumanGroin, shHumanNeck, shSkeletalFoot, shYetiFoot,
  shMagicSword, shSeaTentacle, shKrakenHead, shKrakenEye, shKrakenEye2,
  shArrow,
  shPHead, shPFace, shGolemhead, shHood, shArmor, 
  shAztecHead, shAztecCap,
  shSabre, shTurban1, shTurban2, shVikingHelmet,
  shWestHat1, shWestHat2, shGunInHand,
  shKnightArmor, shKnightCloak, shWightCloak,
  shGhost, shEyes, shSlime, shJoint, shWormHead, shTentHead, shShark,
  shHedgehogBlade, shHedgehogBladePlayer,
  shWolfBody, shWolfHead, shWolfLegs, shWolfEyes,
  shWolfFrontLeg, shWolfRearLeg, shWolfFrontPaw, shWolfRearPaw,
  shBigHepta, shBigTriangle, shBigHex, shBigHexTriangle, shBigHexTriangleRev,
  shFemaleBody, shFemaleHair, shFemaleDress, shWitchDress,
  shWitchHair, shBeautyHair, shFlowerHair, shFlowerHand, shSuspenders,
  shBugBody, shBugArmor, shBugLeg, shBugAntenna,
  shPickAxe, shPike, shFlailBall, shFlailTrunk, shFlailChain,
  shBook, shBookCover, shGrail,
  shBoatOuter, shBoatInner, shCompass1, shCompass2, shCompass3,
  shKnife, shTongue, shFlailMissile,
  shPirateHook, shPirateHood, shEyepatch, shPirateX,
  // shScratch, 
  shHeptaMarker,
  shSkeletonBody, shSkull, shSkullEyes, shFatBody, shWaterElemental,
  shPalaceGate, shFishTail,
  shMouse, shMouseLegs, shMouseEyes,
  shPrincessDress, shPrinceDress,
  shWizardCape1, shWizardCape2,
  shBigCarpet1, shBigCarpet2, shBigCarpet3,
  shGoatHead, shRose, shThorns,
  shRatHead, shRatTail, shRatEyes, shRatCape1, shRatCape2,
  shWizardHat1, shWizardHat2,
  shTortoise[13][6],
  shDragonLegs, shDragonTail, shDragonHead, shDragonSegment, shDragonNostril, 
  shDragonWings, 
  shSolidBranch, shWeakBranch, shBead0, shBead1,
  shBatWings, shBatBody, shBatMouth, shBatFang, shBatEye,
  shParticle[16],
  shReptile[5][4],
  shReptileBody, shReptileHead, shReptileFrontFoot, shReptileRearFoot,
  shReptileFrontLeg, shReptileRearLeg, shReptileTail, shReptileEye,

  shTrylobite, shTrylobiteHead, shTrylobiteBody,
  shTrylobiteFrontLeg, shTrylobiteRearLeg, shTrylobiteFrontClaw, shTrylobiteRearClaw,
  
  shBullBody, shBullHead, shBullHorn, shBullRearHoof, shBullFrontHoof,
  
  shButterflyBody, shButterflyWing, shGadflyBody, shGadflyWing, shGadflyEye,

  shDodeca;

#define USERLAYERS 8
#define USERSHAPEGROUPS 8
#define USERSHAPEIDS 256

struct usershapelayer {
  vector<hyperpoint> list;
  bool sym;
  int rots;
  int color;
  hyperpoint shift, spin;
  hpcshape sh;
  };

struct usershape {
  usershapelayer d[USERLAYERS];
  };

usershape *usershapes[USERSHAPEGROUPS][USERSHAPEIDS];

void drawTentacle(hpcshape &h, ld rad, ld var, ld divby) {
  for(int i=0; i<=20; i++)
    hpcpush(ddi(S21, rad + var * sin(i * M_PI/divby)) * ddi(0, crossf * i/20.) * C0);
  for(int i=20; i>=0; i--)
    hpcpush(ddi(S21*3, rad - var * sin(i * M_PI/divby)) * ddi(0, crossf * i/20.) * C0);
  }

hyperpoint hpxd(ld d, ld x, ld y, ld z) {
  hyperpoint H = hpxyz(d*x, d*y, z);
  H = mid(H, H);
  return H;
  }

double scalef;

hyperpoint hpxyzsc(double x, double y, double z) {
  if(purehepta) return hpxd(scalef, x, y, z);
  else return hpxyz(x,y,z);
  }

hyperpoint turtlevertex(int u, double x, double y, double z) {
  ld scale = purehepta ? scalef : 1;
  if(u) scale /= 2;
  return hpxd(scale, x, y, z);
  }


void bshape(hpcshape& sh, int p) {
  if(last) last->e = qhpc;
  last = &sh;
  last->s = qhpc, last->prio = p;
  first = true; 
  }

hyperpoint spfix(int rots, hyperpoint h) {
  if(!sphere) return h;
  if(rots != 7) return h;
  double d = atan2(h[0], h[1]);
  return spin(M_PI + M_PI * 4/35 * d) * h;
  }

void bshape(hpcshape& sh, int p, double shzoom, int shapeid) {
  bshape(sh, p);
  int whereis = 0;
  while(polydata[whereis] != NEWSHAPE || polydata[whereis+1] != shapeid) whereis++;
  int rots = polydata[whereis+2]; int sym = polydata[whereis+3];
  whereis += 4;
  int qty = 0;
  while(polydata[whereis + 2*qty] != NEWSHAPE) qty++;
  double shzoomx = shzoom;
  double shzoomy = shzoom;
  if(shzoom == WOLF) shzoomx = 1.5 * (purehepta ? crossf / hcrossf : 1), shzoomy = 1.6 * (purehepta ? crossf / hcrossf : 1);
  int rots2 = rots;
  if(rots == 7) rots2 = S7;
  for(int r=0; r<rots2; r++) {
    for(int i=0; i<qty; i++)
      hpcpush(spin(2*M_PI*r/rots2) * spfix(rots, hpxy(polydata[whereis+2*i] * shzoomx, polydata[whereis+2*i+1] * shzoomy)));
    if(sym == 2)
    for(int i=qty-1; i>=0; i--)
      hpcpush(spin(2*M_PI*r/rots2) * spfix(rots, hpxy(polydata[whereis+2*i] * shzoomx, -polydata[whereis+2*i+1] * shzoomy)));
    }
  hpcpush(spfix(rots, hpxy(polydata[whereis] * shzoomx, polydata[whereis+1] * shzoomy)));
  }

void copyshape(hpcshape& sh, hpcshape& orig, int p) {
  if(last) last->e = qhpc;
  sh = orig; sh.prio = p;
  }

void zoomShape(hpcshape& old, hpcshape& newsh, double factor, int lev) {

  bshape(newsh, lev);
  for(int i=old.s; i<old.e; i++) {
    hyperpoint H = hpc[i];
    H = hpxyz(factor*H[0], factor*H[1], H[2]);
    H = mid(H,H);
    hpcpush(H);
    }
  }

void bshapeend() {
  if(last) last->e = qhpc;
  last = NULL;
  }

transmatrix shadowmulmatrix;

void pushShape(const usershapelayer& ds) {

  if(ds.list.empty()) return;

  transmatrix T = rgpushxto0(ds.shift) * rspintox(ds.spin);
  
  for(int r=0; r<ds.rots; r++) {
    for(int i=0; i<size(ds.list); i++)
      hpcpush(T * spin(2*M_PI*r/ds.rots) * ds.list[i]);

    if(ds.sym) {
  
      transmatrix mirrortrans = Id; mirrortrans[1][1] = -1;
      for(int i=size(ds.list)-1; i>=0; i--)
        hpcpush(T * spin(2*M_PI*r/ds.rots) * mirrortrans * ds.list[i]);
      }
    }
  hpcpush(T * ds.list[0]);
  }

void buildpolys() {

  geom3::compute();
  DEBB(DF_INIT, (debugfile,"buildpolys\n"));

  // printf("crossf = %f euclid = %d sphere = %d\n", float(crossf), euclid, sphere);
  qhpc = 0;

  bshape(shMovestar, PPR_MOVESTAR);
  for(int i=0; i<8; i++) {
    hpcpush(spin(M_PI * i/4) * xpush(crossf) * spin(M_PI * i/4) * C0);
    hpcpush(spin(M_PI * i/4 + M_PI/8) * xpush(crossf/4) * spin(M_PI * i/4 + M_PI/8) * C0);
    }
  
  // scales
  scalef = purehepta ? crossf / hcrossf : 1;
  double scalef2 = purehepta ? crossf / hcrossf * .88 : 1;
  
  double spzoom = sphere ? 1.4375 : 1;
  
  double spzoom6 = sphere ? 1.2375 : 1;
  double spzoom7 = sphere ? .8 : 1;
  double spzoomd7 = (purehepta && sphere) ? 1 : spzoom7;

#define SHADMUL 1.3
  
  // procedural floors
  double shexf = purehepta ? crossf* .55 : hexf;    
  
  bshape(shTriheptaFloor[0], PPR_FLOOR);
  for(int t=0; t<=3; t++) hpcpush(ddi(t*S28, scalef*spzoom6*.2776) * C0);

  bshape(shTriheptaFloor[1], PPR_FLOOR);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*12, sphere ? .54 : scalef*spzoom6*.5273) * C0);

  bshape(shTriheptaFloorShadow[0], PPR_FLOOR);
  for(int t=0; t<=3; t++) hpcpush(ddi(t*S28, scalef*spzoom6*.2776*SHADMUL) * C0);

  bshape(shTriheptaFloorShadow[1], PPR_FLOOR);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*12, sphere ? .54 : scalef*spzoom6*.5273*SHADMUL) * C0);

  bshape(shFloor[0], PPR_FLOOR);
  for(int t=0; t<=6; t++) hpcpush(ddi(S7 + t*S14, shexf*.8*spzoom) * C0);

  bshape(shCircleFloor, PPR_FLOOR);
  for(int t=0; t<=84; t+=2) hpcpush(ddi(t, shexf*.7*spzoom) * C0);

  bshape(shFloor[1], PPR_FLOOR);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*12, shexf*.94) * C0);

  for(int i=0; i<3; i++) for(int j=0; j<3; j++) shadowmulmatrix[i][j] =
    i==2&&j==2 ? 1:
    i==j ? SHADMUL:
    0;
  
  bshape(shFloorShadow[0], PPR_FLOOR);
  for(int t=0; t<=6; t++) hpcpush(ddi(S7 + t*S14, shexf*.8*spzoom*SHADMUL) * C0);

  bshape(shFloorShadow[1], PPR_FLOOR);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*12, shexf*.94*SHADMUL) * C0);
  
  // sidewalls for the 3D mode
  for(int k=0; k<SIDEPARS; k++) {
    double dlow=1, dhi=1;
    if(k==SIDE_WALL) dhi = geom3::WALL;
    else if(k==SIDE_LAKE) dlow = geom3::LAKE;
    else if(k==SIDE_LTOB) dlow = geom3::BOTTOM, dhi = geom3::LAKE;
    else if(k==SIDE_BTOI) dlow = geom3::INFDEEP, dhi = geom3::BOTTOM;
    else if(k==SIDE_WTS3) dlow = geom3::SLEV[3], dhi = geom3::WALL;
    else dlow = geom3::SLEV[k-SIDE_SLEV], dhi = geom3::SLEV[k-SIDE_SLEV+1];
    
    validsidepar[k] = (dlow > 0 && dhi > 0) || (dlow < 0 && dhi < 0);

    bshape(shFloorSide[k][0], PPR_LAKEWALL);
    for(int t=0; t<=1; t++) hpcpush(ddi(t*S14-S7, shexf*.8*spzoom) * C0);
    chasmifyPoly(dlow, dhi, k);
    
    bshape(shFloorSide[k][1], PPR_LAKEWALL);
    for(int t=0; t<=1; t++) hpcpush(ddi(t*12-6, shexf*.94) * C0);
    chasmifyPoly(dlow, dhi, k);

    bshape(shSemiFloorSide[k], PPR_LAKEWALL);
    for(int t=0; t<=3; t+=3) hpcpush(ddi(S7 + (3+t)*S14, shexf*.8*spzoom) * C0);
    chasmifyPoly(dlow, dhi, k);

    bshape(shTriheptaSide[k][0], PPR_LAKEWALL);
    for(int t=0; t<=1; t++) hpcpush(ddi(t*S28-S14, scalef*spzoom6*.2776) * C0);
    chasmifyPoly(dlow, dhi, k);
  
    bshape(shTriheptaSide[k][1], PPR_LAKEWALL);
    for(int t=0; t<=1; t++) hpcpush(ddi(t*12-6, sphere ?.54 : scalef*spzoom6*.5273) * C0);
    chasmifyPoly(dlow, dhi, k);

    bshape(shMFloorSide[k][0], PPR_LAKEWALL);
    for(int t=0; t<=1; t++) hpcpush(ddi(t*S14-S7, shexf*.7*spzoom) * C0);
    chasmifyPoly(dlow, dhi, k);

    bshape(shMFloorSide[k][1], PPR_LAKEWALL);
    for(int t=0; t<=1; t++) hpcpush(ddi(t*12-6, shexf*.8) * C0);
    chasmifyPoly(dlow, dhi, k);
    }

  for(int d=0; d<2; d++) {
    bshape(shSemiFloor[d], PPR_FLOOR);
    for(int t=0; t<=4; t++) hpcpush(ddi(S7 + (3+3*d+t%4)*S14, shexf*.8*spzoom) * C0);
    }

  bshape(shMFloor[0], PPR_FLOORa);
  for(int t=0; t<=6; t++) hpcpush(ddi(S7 + t*S14, shexf*.7*spzoom) * C0);

  bshape(shMFloor[1], PPR_FLOORa);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*12, shexf*.8) * C0);

  bshape(shMFloor2[0], PPR_FLOORb);
  for(int t=0; t<=6; t++) hpcpush(ddi(S7 + t*S14, shexf*.6*spzoom) * C0);

  bshape(shMFloor2[1], PPR_FLOORb);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*12, shexf*.7) * C0);
  
  bshape(shMFloor3[0], PPR_FLOORc);
  for(int t=0; t<=6; t++) hpcpush(ddi(S7 + t*S14, shexf*.5*spzoom) * C0);

  bshape(shMFloor3[1], PPR_FLOORc);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*12, shexf*.6) * C0);
  
  bshape(shMFloor4[0], PPR_FLOORd);
  for(int t=0; t<=6; t++) hpcpush(ddi(S7 + t*S14, shexf*.4*spzoom) * C0);

  bshape(shMFloor4[1], PPR_FLOORd);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*12, shexf*.5) * C0);

  bshape(shBigCarpet1, PPR_GFLOORa);
//for(int t=0; t<=7; t++) hpcpush(ddi(t*12, -shexf*3.5) * C0);  
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*12, -shexf*2.1) * C0);  

  bshape(shBigCarpet2, PPR_GFLOORb);
//for(int t=0; t<=7; t++) hpcpush(ddi(t*12, -shexf*3.4) * C0);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*12, -shexf*1.9) * C0);

  bshape(shBigCarpet3, PPR_GFLOORc);
//for(int t=0; t<=7; t++) hpcpush(ddi(t*12, -shexf*3.4) * C0);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*12*3, -shexf*1.7) * C0);

  bshape(shBFloor[0], PPR_BFLOOR);
  for(int t=0; t<=6; t++) hpcpush(ddi(S7 + t*S14, shexf*.1) * C0);

  bshape(shBFloor[1], PPR_BFLOOR);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*12, shexf*.1) * C0);
  
  bshape(shMineMark[0], PPR_MINEMARK);
  for(int t=0; t<=6; t++) hpcpush(ddi(S7 + t*S14, shexf*.1) * C0);

  bshape(shMineMark[1], PPR_MINEMARK);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*12, shexf*.1) * C0);
  
  for(int d=0; d<2; d++) {
    bshape(shSemiBFloor[d], PPR_BFLOOR);
    for(int t=0; t<=4; t++) hpcpush(ddi(S7 + (3+3*d+t%4)*S14, shexf*.1) * C0);
    }

  // walls etc
  
  bshape(shGiantStar[1], PPR_GFLOORa);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*36, -shexf*2.4) * C0);

  bshape(shGiantStar[0], PPR_GFLOORa);
  for(int t=0; t<=6; t++) {
    hpcpush(ddi(t*S14, -shexf*2.4) * C0);
    hpcpush(ddi(t*S14+S7, shexf*1.5) * C0);
    }
  hpcpush(ddi(0, -shexf*2.4) * C0);
  
  bshape(shMirror, PPR_WALL);
  if(purehepta) {
    for(int t=0; t<=S7; t++) hpcpush(ddi(t*12, shexf*.8) * C0);
    }
  else {
    for(int t=0; t<=6; t++) hpcpush(ddi(S7 + t*S14, shexf*.7) * C0);
    }

  bshape(shWall[0], PPR_WALL);
  for(int t=0; t<=6; t++) {
    hpcpush(ddi(S7 + t*S14, shexf*.8) * C0);
    hpcpush(ddi(S14 + t*S14, shexf*.2) * C0);
    }
  
  bshape(shWall[1], PPR_WALL);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*36, shexf*.94) * C0);

  bshape(shCross, PPR_WALL);
  for(int i=0; i<=S84; i+=S7)
    hpcpush(ddi(i, shexf * (i%3 ? 0.8 : 0.3)) * C0);

// items
  
  bshape(shGem[0], PPR_ITEM);
  for(int t=0; t<=6; t++) {
    hpcpush(ddi(S7 + t*S14, shexf*.4) * C0);
    hpcpush(ddi(S14 + t*S14, shexf*.1) * C0);
    }
  
  bshape(shGem[1], PPR_ITEM);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*36, shexf*.5) * C0);

  bshape(shStar, PPR_ITEM);
  for(int t=0; t<S84; t+=6) {
    hpcpush(ddi(t,   shexf*.2) * C0);
    hpcpush(ddi(t+3,   shexf*.6) * C0);
    }

  bshape(shDaisy, PPR_ITEM);
  for(int t=0; t<=6; t++) {
    hpcpush(ddi(t*S14, shexf*.8*3/4) * C0);
    hpcpush(ddi(t*S14+S7, shexf*-.5*3/4) * C0);
    }
  hpcpush(ddi(0, shexf*.6) * C0);

  bshape(shTriangle, PPR_ITEM);
  for(int t=0; t<=4; t++) {
    hpcpush(ddi(t*S28, shexf*.5) * C0);
    }

  bshape(shDisk, PPR_ITEM);
  for(int i=0; i<=S84; i+=3)
    hpcpush(ddi(i, crossf * .2) * C0);

  bshape(shDiskT, PPR_ITEM);
  for(int i=0; i<=S84; i+=S28)
    hpcpush(ddi(i, crossf * .2) * C0);

  bshape(shDiskS, PPR_ITEM);
  for(int i=0; i<=S84; i+=S21) {
    hpcpush(ddi(i, crossf * .2) * C0);
    hpcpush(ddi(i+S21/3, crossf * .1) * C0);
    hpcpush(ddi(i+S21-S21/3, crossf * .1) * C0);
    }

  bshape(shDiskM, PPR_ITEM);
  for(int i=0; i<=S84; i+=3) {
    hpcpush(ddi(i, crossf * .1) * C0);
    }

  bshape(shDiskSq, PPR_ITEM);
  for(int i=0; i<=S84; i+=21) {
    hpcpush(ddi(i, crossf * .15) * C0);
    }

  bshape(shEgg, PPR_ITEM);
  for(int i=0; i<=S84; i+=3)
    hpcpush(hpxy(sin(i*2*M_PI/S84)*.15, cos(i*2*M_PI/S84)*.11));
  
  bshape(shRing, PPR_ITEM);
  for(int i=0; i<=S84; i+=3)
    hpcpush(ddi(i, crossf * .25) * C0);
  for(int i=S84; i>=0; i--)
    hpcpush(ddi(i, crossf * .30) * C0);
  hpcpush(ddi(0, crossf * .25) * C0);
  
  bshape(shSpikedRing, PPR_ITEM);
  for(int i=0; i<=S84; i+=3)
    hpcpush(ddi(i, crossf * .25) * C0);
  for(int i=S84; i>=0; i--)
    hpcpush(ddi(i, crossf * (i&1?.35:.30)) * C0);
  hpcpush(ddi(0, crossf * .25) * C0);
  
  bshape(shTargetRing, PPR_ITEM);
  for(int i=0; i<=S84; i+=3)
    hpcpush(ddi(i, crossf * .25) * C0);
  for(int i=S84; i>=0; i--)
    hpcpush(ddi(i, crossf * (i >= S42-6 && i <= S42+6 ?.36:.30)) * C0);
  hpcpush(ddi(0, crossf * .25) * C0);
  
  bshape(shSpearRing, PPR_ITEM);
  for(int i=0; i<=S84; i+=3)
    hpcpush(ddi(i, crossf * .25) * C0);
  for(int i=S84; i>=0; i--) {
    int d = i - S42;
    if(d<0) d = -d;
    d = 8 - 2 * d;
    if(d<0) d = 0;
    hpcpush(ddi(i, crossf * (.3 + .04 * d)) * C0);
    }
  hpcpush(ddi(0, crossf * .25) * C0);
  
  bshape(shSawRing, PPR_ITEM);
  for(int i=0; i<=S84; i+=3)
    hpcpush(ddi(i, crossf * .25) * C0);
  for(int i=S84; i>=0; i--)
    hpcpush(ddi(i, crossf * (.3 + (i&3) * .02)) * C0);
  hpcpush(ddi(0, crossf * .25) * C0);
  
  bshape(shGearRing, PPR_ITEM);
  for(int i=0; i<=S84; i+=3)
    hpcpush(ddi(i, crossf * .25) * C0);
  for(int i=S84; i>=0; i--)
    hpcpush(ddi(i, crossf * ((i%6<3)?.3:.36)) * C0);
  hpcpush(ddi(0, crossf * .25) * C0);
  
  bshape(shPeaceRing, PPR_ITEM);
  for(int i=0; i<=S84; i+=3)
    hpcpush(ddi(i, crossf * .25) * C0);
  for(int i=S84; i>=0; i--)
    hpcpush(ddi(i, crossf * (i%28 < 7?.36 : .3)) * C0);
  hpcpush(ddi(0, crossf * .25) * C0);
  
  bshape(shHeptaRing, PPR_ITEM);
  for(int i=0; i<=S84; i+=3)
    hpcpush(ddi(i, crossf * .25) * C0);
  for(int i=S84; i>=0; i--)
    hpcpush(ddi(i, crossf * (i%12 < 3?.4 : .27)) * C0);
  hpcpush(ddi(0, crossf * .25) * C0);
  
  bshape(shCompass1, PPR_ITEM);
  for(int i=0; i<=S84; i+=3)
    hpcpush(ddi(i, crossf * .35) * C0);
  
  bshape(shCompass2, PPR_ITEMa);
  for(int i=0; i<=S84; i+=3)
    hpcpush(ddi(i, crossf * .3) * C0);
  
  bshape(shCompass3, PPR_ITEMb);
  hpcpush(ddi(0, crossf * .29) * C0);
  hpcpush(ddi(S21, crossf * .04) * C0);
  hpcpush(ddi(-S21, crossf * .04) * C0);
  
  /* bshape(shBranch, 32);
  hpcpush(ddi(21, crossf/5) * C0);
  hpcpush(ddi(21, -crossf/5) * C0);
  hpcpush(ddi(21, -crossf/5) * ddi(0, crossf) * C0);
  hpcpush(ddi(21, crossf/5) * ddi(0, crossf) * C0); */
  
  bshape(shILeaf[0], PPR_ONTENTACLE);
  for(int t=0; t<=6; t++) {
    hpcpush(ddi(S7 + t*S14, shexf*.7) * C0);
    hpcpush(ddi(S14 + t*S14, shexf*.15) * C0);
    }

  bshape(shILeaf[1], PPR_ONTENTACLE);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*36, shexf*.8) * C0);

  bshape(shSlime, 33);
  for(int i=0; i<=S84; i++)
    hpcpush(ddi(i, crossf * (0.7 + .2 * sin(i * M_PI * 2 / S84 * 9))) * C0);

  bshape(shHeptaMarker, PPR_HEPTAMARK);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*12, shexf*.2) * C0);
  
  bshape(shBigHepta, PPR_FLOOR);
  for(int t=0; t<=S7; t++) hpcpush(ddi(t*12, -shexf*1.5) * C0);
  
  bshape(shBigHex, PPR_FLOOR);
  for(int t=0; t<=6; t++) hpcpush(ddi(t*S14, -shexf*1.3) * C0);
  
  bshape(shBigTriangle, PPR_FLOOR);
  for(int t=0; t<=3; t++) hpcpush(ddi(t*S28, -shexf*1.5) * C0);
  
  bshape(shBigHexTriangleRev, PPR_FLOOR);
  for(int t=0; t<=3; t++) hpcpush(ddi(t*S28, -shexf*1.3) * C0);
  
  bshape(shBigHexTriangle, PPR_FLOOR);
  for(int t=0; t<=3; t++) hpcpush(ddi(S14+t*S28, -shexf*1.3) * C0);
  
  bshape(shRose, PPR_ITEM);
  for(int t=0; t<S84; t++) 
    hpcpush(spin(M_PI * t / (S42+.0)) * xpush(crossf * (0.2 + .15 * sin(M_PI * t / (S42+.0) * 3))) * C0);

  bshape(shThorns, PPR_THORNS);
  for(int t=0; t<60; t++) 
    hpcpush(spin(M_PI * t / 30.0) * xpush(crossf * ((t&1) ? 0.3 : 0.6)) * C0);
    
  for(int i=0; i<16; i++) {
    bshape(shParticle[i], PPR_PARTICLE);
    for(int t=0; t<6; t++) 
      hpcpush(spin(M_PI * t * 2 / 6 + M_PI * 2/6 * hrand(100) / 150.) * xpush(0.03 + hrand(100) * 0.0003) * C0);
    hpc[qhpc++] = hpc[last->s];
    }
  
  // hand-drawn shapes

  // floors:
  bshape(shStarFloor[0], PPR_FLOOR, scalef2*spzoom6, 1);
  bshape(shStarFloor[1], PPR_FLOOR, scalef2*spzoomd7, 2);
  bshape(shCloudFloor[0], PPR_FLOOR, scalef2*spzoom6, 3);
  bshape(shCloudFloor[1], PPR_FLOOR, scalef2*spzoomd7, 4);
  
  bshape(shCrossFloor[0], PPR_FLOOR, scalef*spzoom6, 5);
  bshape(shCrossFloor[1], PPR_FLOOR, scalef*spzoomd7, 6);
  bshape(shChargedFloor[0], PPR_FLOOR, scalef*spzoom6, 8);
  bshape(shChargedFloor[1], PPR_FLOOR, scalef*spzoomd7, 9);
  bshape(shChargedFloor[2], PPR_FLOOR, scalef*spzoom6, 7);
  bshape(shChargedFloor[3], 12, spzoomd7 * (sphere&&purehepta?.9:1), 10); // purehepta variant
  bshape(shSStarFloor[0], PPR_FLOOR, scalef*spzoom6, 11);
  bshape(shSStarFloor[1], PPR_FLOOR, scalef*spzoomd7, 12);
  bshape(shOverFloor[0], PPR_FLOOR, scalef*spzoom, 13);
  if(purehepta) bshape(shOverFloor[1], PPR_FLOOR, sphere ? .83 : 1, 14);
  else bshape(shOverFloor[1], PPR_FLOOR, scalef*spzoom7, 15);
  bshape(shOverFloor[2], PPR_FLOOR, 1*spzoom7, 16);
  bshape(shTriFloor[0], PPR_FLOOR, scalef*spzoom6, 17);
  bshape(shTriFloor[1], PPR_FLOOR, scalef*spzoomd7, 18);
  bshape(shFeatherFloor[0], PPR_FLOOR, scalef*spzoom6, 19);
  if(purehepta) bshape(shFeatherFloor[1], PPR_FLOOR, sphere ? .83 : 1, 20);
  else bshape(shFeatherFloor[1], PPR_FLOOR, scalef*spzoom7, 21);
  bshape(shFeatherFloor[2], PPR_FLOOR, 1, 22);  // Euclidean variant
  bshape(shBarrowFloor[0], PPR_FLOOR, spzoom6, 23);
  bshape(shBarrowFloor[1], PPR_FLOOR, (sphere&&purehepta?.9:1) * spzoomd7, 24);
  bshape(shBarrowFloor[2], PPR_FLOOR, sphere?.9:1, 25);
  bshape(shNewFloor[0], PPR_FLOOR, scalef*spzoom6, 26);
  bshape(shNewFloor[1], PPR_FLOOR, scalef*spzoomd7, 27);
  bshape(shTrollFloor[0], PPR_FLOOR, 1*spzoom6, 28);
  bshape(shTrollFloor[1], PPR_FLOOR, 1*spzoomd7, 29);

  bshape(shButterflyFloor[0], PPR_FLOOR, scalef*spzoom6, 325);
  bshape(shButterflyFloor[1], PPR_FLOOR, scalef*spzoomd7, 326);

  bshape(shLeafFloor[0], PPR_FLOOR_DRAGON, 1*spzoom6, 313);
  bshape(shLeafFloor[1], PPR_FLOOR_DRAGON, 1*spzoomd7, 314);
  
  bshape(shTriheptaFloor[2], PPR_FLOOR,  scalef, 32);
  bshape(shTriheptaFloor[3], PPR_FLOOR,  scalef, 33);
  bshape(shTriheptaFloor[4], PPR_FLOOR,  scalef, 34);
  bshape(shTriheptaFloor[5], PPR_FLOOR,  scalef, 35);
  bshape(shTriheptaFloor[6], PPR_FLOOR,  scalef, 36);
  bshape(shTriheptaFloor[7], PPR_FLOOR,  scalef, 37);
  bshape(shTriheptaFloor[9], PPR_FLOOR,  scalef, 38);
  bshape(shTriheptaFloor[10], PPR_FLOOR,  scalef, 39);
  bshape(shTriheptaFloor2[0], PPR_FLOOR,  scalef, 40);
  bshape(shTriheptaFloor2[1], PPR_FLOOR,  scalef, 41);
  bshape(shSemiFloorShadow, PPR_FLOOR, scalef, 263);
  bshape(shTriheptaEuc[0], PPR_FLOOR,  scalef, 42);
  bshape(shTriheptaEuc[1], PPR_FLOOR,  scalef, 43);
  bshape(shTriheptaEuc[2], PPR_FLOOR,  scalef, 44);
  bshape(shTriheptaEucShadow[0], PPR_FLOOR,  scalef*SHADMUL, 42);
  bshape(shTriheptaEucShadow[1], PPR_FLOOR,  scalef*SHADMUL, 43);
  bshape(shTriheptaEucShadow[2], PPR_FLOOR,  scalef*SHADMUL, 44);
  bshape(shPalaceFloor[0], PPR_FLOOR,  scalef*spzoom6, 45);
  bshape(shPalaceFloor[1], PPR_FLOOR,  scalef*spzoomd7, 46);
  bshape(shPalaceGate, PPR_STRUCT1, scalef, 47);
  bshape(shSemiFeatherFloor[0], PPR_FLOOR,  scalef*spzoom6, 48);
  bshape(shSemiFeatherFloor[1], PPR_FLOOR,  scalef*spzoom6, 49);
  bshape(shDemonFloor[1], PPR_FLOOR,  scalef*spzoomd7, 50);
  bshape(shDemonFloor[0], PPR_FLOOR,  scalef*spzoom6, 51);
  bshape(shCaveFloor[0], PPR_FLOOR,  scalef*spzoom6, 52);
  bshape(shCaveFloor[1], PPR_FLOOR,  scalef*spzoomd7, 53);
  bshape(shCaveFloor[2], PPR_FLOOR,  1, 54);  // Euclidean variant
  bshape(shDesertFloor[0], PPR_FLOOR,  scalef*spzoom6, 55);
  bshape(shDesertFloor[1], PPR_FLOOR,  scalef*spzoomd7, 56);
  for(int i=1; i<=3; i++) for(int j=0; j<2; j++) 
    zoomShape(shDesertFloor[j], shRedRockFloor[i-1][j], 1 - .1 * i, PPR_FLOORa+i);
  bshape(shPowerFloor[0], PPR_FLOOR_DRAGON, scalef*spzoom6, 57);
  bshape(shPowerFloor[1], PPR_FLOOR_DRAGON, scalef*spzoomd7, 58);
  bshape(shRoseFloor[2], PPR_FLOOR,  1, 173); // purehepta
  bshape(shRoseFloor[0], PPR_FLOOR,  1, 174);
  bshape(shRoseFloor[1], PPR_FLOOR,  scalef, 175);
  bshape(shTurtleFloor[0], PPR_FLOOR,  1, 176);
  bshape(shTurtleFloor[1], PPR_FLOOR,  scalef, 177);
  bshape(shTurtleFloor[2], PPR_FLOOR,  sphere && purehepta ? .9 : 1, 178); // purehepta
  bshape(shDragonFloor[0], PPR_FLOOR_DRAGON, 1, 181);
  bshape(shDragonFloor[1], PPR_FLOOR_DRAGON, scalef, 182);
  bshape(shDragonFloor[2], PPR_FLOOR,  1, 183);
  bshape(shZebra[0], PPR_FLOOR,  scalef, 162);
  bshape(shZebra[1], PPR_FLOOR,  scalef, 163);
  bshape(shZebra[2], PPR_FLOOR,  scalef, 164);
  bshape(shZebra[3], PPR_FLOOR,  scalef, 165);
  bshape(shZebra[4], PPR_FLOOR,  1, 166); // for purehepta
  bshape(shEmeraldFloor[0], PPR_FLOOR,  scalef, 167); // 4
  bshape(shEmeraldFloor[1], PPR_FLOOR,  scalef, 168); // 12
  bshape(shEmeraldFloor[2], PPR_FLOOR,  scalef, 169); // 16
  bshape(shEmeraldFloor[3], PPR_FLOOR,  scalef, 170); // 20
  bshape(shEmeraldFloor[4], PPR_FLOOR,  scalef, 171); // 28
  bshape(shEmeraldFloor[5], PPR_FLOOR,  scalef, 172); // 36
  bshape(shTower[0], PPR_FLOOR_TOWER,  scalef, 196); // 4
  bshape(shTower[1], PPR_FLOOR_TOWER,  scalef, 197); // 5
  bshape(shTower[2], PPR_FLOOR_TOWER,  scalef, 198); // 6
  bshape(shTower[3], PPR_FLOOR_TOWER,  scalef, 199); // 8
  bshape(shTower[4], PPR_FLOOR_TOWER,  scalef, 200); // 9
  bshape(shTower[5], PPR_FLOOR_TOWER,  scalef, 201); // 10
  bshape(shTower[6], PPR_FLOOR_TOWER,  scalef, 202); // 10
  bshape(shTower[7], PPR_FLOOR_TOWER,  1, 203); // purehepta 7
  bshape(shTower[8], PPR_FLOOR_TOWER,  1, 204); // purehepta 11
  bshape(shTower[9], PPR_FLOOR_TOWER,  1, 205); // purehepta 15
  bshape(shTower[10], PPR_FLOOR_TOWER,  1, 206);  // Euclidean
  
  // structures & walls
  bshape(shBoatOuter, PPR_STRUCT0, scalef, 154);
  bshape(shBoatInner, PPR_STRUCT1, scalef, 155);
  bshape(shSolidBranch, PPR_STRUCT0, scalef, 235);
  bshape(shWeakBranch, PPR_STRUCT0, scalef, 236);
  bshape(shFan, PPR_WALL, scalef, 59);

  // items:
  bshape(shElementalShard, PPR_ITEM, scalef, 60);
  bshape(shNecro, PPR_ITEM, scalef, 61);
  bshape(shFigurine, PPR_ITEM, scalef, 62);
  bshape(shStatue, PPR_ITEM, scalef, 63);
  bshape(shBook, PPR_ITEM, scalef, 64);
  bshape(shBookCover, PPR_ITEMa, scalef, 65);
  bshape(shGrail, PPR_ITEM, scalef, 66);
  bshape(shGun, PPR_ITEM, scalef, 67);
  bshape(shKey, PPR_ITEM, scalef, 68);
  bshape(shPirateX, PPR_ITEM, scalef, 124);
  bshape(shTreat, PPR_ITEM, scalef, 253);

  // first layer monsters
  bshape(shTentacleX, PPR_TENTACLE0);
  drawTentacle(shTentacleX, crossf * .25, crossf * .1, 10);
  bshape(shIBranch, PPR_TENTACLE1);
  drawTentacle(shIBranch, crossf * .1, crossf * .2, 5);
  bshape(shTentacle, PPR_TENTACLE1);
  drawTentacle(shTentacle, crossf * .2, crossf * .1, 10);
  copyshape(shJoint, shDisk, PPR_ONTENTACLE);
  bshape(shTentHead, PPR_ONTENTACLE, scalef, 79);
  bshape(shWormHead, PPR_ONTENTACLE, scalef, 80);
  if(purehepta) bshape(shDragonSegment, PPR_TENTACLE1, 1, 233);
  else bshape(shDragonSegment, PPR_TENTACLE1, scalef, 234);
  bshape(shDragonWings, PPR_ONTENTACLE, scalef, 237);
  bshape(shDragonLegs, PPR_TENTACLE0, scalef, 238);
  if(purehepta) bshape(shDragonTail, PPR_TENTACLE1, 1, 239);
  else bshape(shDragonTail, PPR_TENTACLE1, scalef, 240);
  bshape(shDragonNostril, PPR_ONTENTACLE_EYES, scalef, 241);
  bshape(shDragonHead, PPR_ONTENTACLE, scalef, 242);
  if(purehepta) bshape(shSeaTentacle, PPR_TENTACLE1, 1, 245);
  else bshape(shSeaTentacle, PPR_TENTACLE1, 1, 246);  
  ld ksc = purehepta ? 1.8 : 1.5;  
  bshape(shKrakenHead, PPR_ONTENTACLE, ksc, 247);
  bshape(shKrakenEye, PPR_ONTENTACLE_EYES, ksc, 248);
  bshape(shKrakenEye2, PPR_ONTENTACLE_EYES2, ksc, 249);

  // monsters
  bshape(shGhost, PPR_MONSTER_BODY, scalef, 69);
  bshape(shGargoyleWings, PPR_MONSTER_CLOAK, scalef, 70);
  bshape(shGargoyleBody, PPR_MONSTER_BODY, scalef, 71);
  bshape(shDogStripes, PPR_MONSTER_ARMOR1, scalef, 72);
  bshape(shWolf, PPR_MONSTER_BODY, scalef, 73);
  bshape(shWolf1, PPR_MONSTER_EYE0, scalef, 74);
  bshape(shWolf2, PPR_MONSTER_EYE0, scalef, 75);
  bshape(shWolf3, PPR_MONSTER_EYE0, scalef, 76);
  bshape(shHawk, PPR_MONSTER_BODY, scalef, 77);
  bshape(shEagle, PPR_MONSTER_BODY, scalef, 78);
  bshape(shWaterElemental, PPR_MONSTER_BODY, scalef, 81);
  bshape(shMouse, PPR_MONSTER_BODY, scalef, 82);
  bshape(shMouseLegs, PPR_MONSTER_LEG, scalef, 83);
  bshape(shMouseEyes, PPR_MONSTER_EYE0, scalef, 84);

  bshape(shHumanFoot, PPR_MONSTER_FOOT, scalef, 259);
  bshape(shHumanLeg, PPR_MONSTER_LEG, scalef, 260);
  bshape(shHumanGroin, PPR_MONSTER_GROIN, scalef, 261);
  bshape(shHumanNeck, PPR_MONSTER_NECK, scalef, 262);
  bshape(shSkeletalFoot, PPR_MONSTER_FOOT, scalef, 264);
  bshape(shYetiFoot, PPR_MONSTER_FOOT, scalef, 276);

  for(int i=0; i<5; i++)
    for(int j=0; j<4; j++)
      bshape(shReptile[i][j], j >= 2 ? PPR_LIZEYE : PPR_FLOOR_DRAGON, scalef, 277+i*4+j);
    
  shift(shReptile[1][2], 0.316534, -0.136547, 1.057752);
  shift(shReptile[1][3], 0.340722, -0.059946, 1.058152);

  shift(shReptile[2][2], -0.124134, -0.286631, 1.047648);
  shift(shReptile[2][3], -0.054108, -0.298966, 1.045136);

  shift(shReptile[3][2], -0.229759, 0.191918, 1.043849);
  shift(shReptile[3][3], -0.173583, 0.236900, 1.042234);

  shift(shReptile[4][2], 0.05, 0, 1.00124921972503929);
  shift(shReptile[4][3], -0.05, 0, 1.00124921972503929);

  bshape(shReptileBody, PPR_MONSTER_BODY, scalef, 297);
  bshape(shReptileHead, PPR_MONSTER_HEAD, scalef, 298);
  bshape(shReptileFrontFoot, PPR_MONSTER_FOOT, scalef, 299);
  bshape(shReptileRearFoot, PPR_MONSTER_FOOT, scalef, 300);
  bshape(shReptileFrontLeg, PPR_MONSTER_LEG, scalef, 301);
  bshape(shReptileRearLeg, PPR_MONSTER_LEG, scalef, 302);
  bshape(shReptileTail, PPR_MONSTER_BODY, scalef, 303);
  bshape(shReptileEye, PPR_MONSTER_EYE0, scalef, 304);  
  bshape(shDodeca, PPR_ITEM, scalef, 305);  

  bshape(shPBody, PPR_MONSTER_BODY, scalef, 85);
  bshape(shYeti, PPR_MONSTER_BODY, scalef, 86);
  bshape(shPSword, PPR_MONSTER_WPN, scalef, 90);
  bshape(shPKnife, PPR_MONSTER_WPN, scalef, 91);
  bshape(shPirateHook, PPR_MONSTER_WPN, scalef, 92);
  bshape(shSabre, PPR_MONSTER_WPN, scalef, 93);
  bshape(shHedgehogBlade, PPR_MONSTER_WPN, scalef, 94);
  bshape(shHedgehogBladePlayer, PPR_MONSTER_WPN, scalef, 95);
  bshape(shFemaleBody, PPR_MONSTER_BODY, scalef, 96);
  bshape(shFemaleDress, PPR_MONSTER_ARMOR0, scalef, 97);
  bshape(shDemon, PPR_MONSTER_HAIR, scalef, 98);

  bshape(shTrylobite, PPR_MONSTER_BODY, scalef, 99);
  bshape(shTrylobiteHead, PPR_MONSTER_HEAD, scalef, 100);
  bshape(shTrylobiteBody, PPR_MONSTER_BODY, scalef, 308);
  bshape(shTrylobiteFrontClaw, PPR_MONSTER_FOOT, scalef, 309);
  bshape(shTrylobiteRearClaw, PPR_MONSTER_FOOT, scalef, 310);
  bshape(shTrylobiteFrontLeg, PPR_MONSTER_LEG, scalef, 311);
  bshape(shTrylobiteRearLeg, PPR_MONSTER_LEG, scalef, 312);

  bshape(shBullBody, PPR_MONSTER_BODY, scalef, 315);
  bshape(shBullHorn, PPR_MONSTER_HEAD, scalef, 316);
  bshape(shBullRearHoof, PPR_MONSTER_FOOT, scalef, 317);
  bshape(shBullFrontHoof, PPR_MONSTER_FOOT, scalef, 318);
  bshape(shBullHead, PPR_MONSTER_HEAD, scalef, 319);

  bshape(shButterflyBody, PPR_MONSTER_BODY, scalef, 320);
  bshape(shButterflyWing, PPR_MONSTER_BODY, scalef, 321);

  bshape(shGadflyBody, PPR_MONSTER_BODY, scalef * 1.5, 322);
  bshape(shGadflyWing, PPR_MONSTER_BODY, scalef * 1.5, 323);
  bshape(shGadflyEye, PPR_MONSTER_BODY, scalef * 1.5, 324);

  bshape(shGoatHead, PPR_MONSTER_HAIR, scalef, 101);
  bshape(shRatHead, PPR_MONSTER_HEAD, scalef, 102);
  bshape(shRatEyes, PPR_MONSTER_EYE0, scalef, 103);  
  bshape(shRatTail, PPR_MONSTER_LEG, scalef, 104);
  bshape(shRatCape1, PPR_MONSTER_HOODCLOAK2, scalef, 105);
  bshape(shRatCape2, PPR_MONSTER_HOODCLOAK1, scalef, 106);
  bshape(shKnightArmor, PPR_MONSTER_ARMOR0, scalef, 107);
  bshape(shWightCloak, PPR_MONSTER_CLOAK, scalef, 108);
  bshape(shKnightCloak, PPR_MONSTER_CLOAK, scalef, 109);
  bshape(shPrincessDress, PPR_MONSTER_ARMOR1, scalef, 110);
  bshape(shWizardCape1, PPR_MONSTER_HOODCLOAK1, 1, 111);
  bshape(shWizardCape2, PPR_MONSTER_HOODCLOAK2, 1, 112);
  bshape(shPrinceDress, PPR_MONSTER_ARMOR0, scalef, 113);
  bshape(shArmor, PPR_MONSTER_HAT0, scalef, 114);
  bshape(shTurban1, PPR_MONSTER_HAT0, scalef, 115);
  bshape(shTurban2, PPR_MONSTER_HAT1, scalef, 116);
  bshape(shWizardHat1, PPR_MONSTER_HAT0, 1, 117);
  bshape(shWizardHat2, PPR_MONSTER_HAT1, 1, 118);
  bshape(shWestHat1, PPR_MONSTER_HAT0, scalef, 119);
  bshape(shWestHat2, PPR_MONSTER_HAT1, scalef, 120);
  bshape(shGunInHand, PPR_MONSTER_WPN, scalef, 121);
  bshape(shVikingHelmet, PPR_MONSTER_HAT0, scalef, 122);
  bshape(shHood, PPR_MONSTER_HAT0, scalef, 123);
  bshape(shPirateHood, PPR_MONSTER_HAT0, scalef, 125);
  bshape(shEyepatch, PPR_MONSTER_HAT1, scalef, 126);
  bshape(shPHead, PPR_MONSTER_HEAD, scalef, 127);
  shGolemhead = shDisk; shGolemhead.prio = PPR_MONSTER_HEAD;
  bshape(shFemaleHair, PPR_MONSTER_HAIR, scalef, 128);
  bshape(shWitchHair, PPR_MONSTER_HAIR, scalef, 129);
  bshape(shBeautyHair, PPR_MONSTER_HAIR, scalef, 130);
  bshape(shFlowerHair, PPR_MONSTER_HAT0, scalef, 131);
  bshape(shSuspenders, PPR_MONSTER_ARMOR1, scalef, 132);
  bshape(shFlowerHand, PPR_MONSTER_WPN, scalef, 133);
  bshape(shPFace, PPR_MONSTER_FACE, scalef, 134);
  bshape(shEyes, PPR_MONSTER_EYE0, scalef, 135);
  bshape(shShark, PPR_MONSTER_BODY, scalef, 136);
  bshape(shBugBody, PPR_MONSTER_BODY, scalef, 137);
  bshape(shBugArmor, PPR_MONSTER_ARMOR0, scalef, 138);
  bshape(shBugLeg, PPR_MONSTER_BODY, scalef, 306);
  bshape(shBugAntenna, PPR_MONSTER_BODY, scalef, 307);
  bshape(shCatBody, PPR_MONSTER_BODY, scalef, 139);
  bshape(shCatLegs, PPR_MONSTER_LEG, scalef, 140);
  bshape(shFamiliarHead, PPR_MONSTER_HEAD, scalef, 141);
  bshape(shFamiliarEye, PPR_MONSTER_EYE1, scalef, 142);
  bshape(shCatHead, PPR_MONSTER_HEAD, scalef, 143);
  bshape(shWolfBody, PPR_MONSTER_BODY, WOLF, 144);
  bshape(shWolfHead, PPR_MONSTER_HEAD, WOLF, 145);
  bshape(shWolfLegs, PPR_MONSTER_LEG, WOLF, 146);
  bshape(shWolfEyes, PPR_MONSTER_EYE0, WOLF, 147);
  bshape(shWitchDress, PPR_MONSTER_ARMOR0, scalef, 148);
  bshape(shPickAxe, PPR_MONSTER_WPN, scalef, 149);
  bshape(shPike, PPR_MONSTER_WPN, scalef, 150);
  bshape(shFlailBall, PPR_MONSTER_WPN, scalef, 151);
  bshape(shFlailTrunk, PPR_MONSTER_WPN, scalef, 152);
  bshape(shFlailChain, PPR_MONSTER_SUBWPN, scalef, 153);
  // bshape(shScratch, 17, scalef, 156);
  bshape(shSkeletonBody, PPR_MONSTER_BODY, scalef, 157);
  bshape(shSkull, PPR_MONSTER_HEAD, scalef, 158);
  bshape(shSkullEyes, PPR_MONSTER_EYE0, scalef, 159);
  bshape(shFishTail, PPR_MONSTER_LEG, scalef, 160);
  bshape(shFatBody, PPR_MONSTER_BODY, scalef, 161);
  bshape(shAztecHead, PPR_MONSTER_HEAD, scalef, 179);
  bshape(shAztecCap, PPR_MONSTER_HAT0, scalef, 180); // 1 114 5 [000000]
  bshape(shBatWings, PPR_MONSTER_LEG, scalef, 254);
  bshape(shBatBody, PPR_MONSTER_BODY, scalef, 255);
  bshape(shBatMouth, PPR_MONSTER_EYE0, scalef, 256);
  bshape(shBatFang, PPR_MONSTER_EYE1, scalef, 257);
  bshape(shBatEye, PPR_MONSTER_EYE0, scalef, 258);
  
  bshape(shDogBody, PPR_MONSTER_BODY, scalef, 265);
  bshape(shDogHead, PPR_MONSTER_HEAD, scalef, 266);
  bshape(shDogTorso, PPR_MONSTER_BODY, scalef, 267);
  bshape(shDogFrontPaw, PPR_MONSTER_FOOT, scalef, 268);
  bshape(shDogRearPaw, PPR_MONSTER_FOOT, scalef, 269);
  bshape(shDogFrontLeg, PPR_MONSTER_LEG, scalef, 270);
  bshape(shDogRearLeg, PPR_MONSTER_LEG, scalef, 271);

  bshape(shWolfFrontPaw, PPR_MONSTER_FOOT, scalef, 272);
  bshape(shWolfRearPaw, PPR_MONSTER_FOOT, scalef, 273);
  bshape(shWolfFrontLeg, PPR_MONSTER_LEG, scalef, 274);
  bshape(shWolfRearLeg, PPR_MONSTER_LEG, scalef, 275);
  
  // missiles
  bshape(shKnife, PPR_MISSILE, scalef, 87);
  bshape(shTongue, PPR_MISSILE, scalef, 88);
  bshape(shFlailMissile, PPR_MISSILE, scalef, 89);

  for(int u=0; u<=2; u+=2) {
  
    int sh = u ? PPR_ITEM : PPR_MONSTER_LEG;
    int uz = u?2:1;
  
    bshape(shTortoise[0][0+u], sh+1, scalef/uz, 207);
    bshape(shTortoise[1][0+u], sh+2, scalef/uz, 208);
    bshape(shTortoise[2][0+u], sh+2, scalef/uz, 209);
    bshape(shTortoise[3][0+u], sh+2, scalef/uz, 210);
    bshape(shTortoise[4][0+u], sh+2, scalef/uz, 211);
    bshape(shTortoise[5][0+u], sh+2, scalef/uz, 212);
    bshape(shTortoise[6][0+u], sh+2, scalef/uz, 213);
    bshape(shTortoise[7][0+u], sh+2, scalef/uz, 214);
    bshape(shTortoise[8][0+u], sh, scalef/uz, 215);
    bshape(shTortoise[9][0+u], sh, scalef/uz, 217);
    bshape(shTortoise[10][0+u], sh, scalef/uz, 218);
    bshape(shTortoise[11][0+u], sh, scalef/uz, 219);
    bshape(shTortoise[12][0+u], sh+2, scalef/uz, 216);
    
    bshape(shTortoise[0][1+u], sh+1, scalef/uz, 220);
    bshape(shTortoise[1][1+u], sh+2, scalef/uz, 221);
    bshape(shTortoise[2][1+u], sh+2, scalef/uz, 222);
    bshape(shTortoise[3][1+u], sh+2, scalef/uz, 223);
    bshape(shTortoise[4][1+u], sh+2, scalef/uz, 224);
    bshape(shTortoise[5][1+u], sh+2, scalef/uz, 225);
    bshape(shTortoise[6][1+u], sh+2, scalef/uz, 226);
    bshape(shTortoise[7][1+u], sh+2, scalef/uz, 227);
    bshape(shTortoise[8][1+u], sh, scalef/uz, 228);
    bshape(shTortoise[9][1+u], sh, scalef/uz, 230);
    bshape(shTortoise[10][1+u], sh, scalef/uz, 231);
    bshape(shTortoise[11][1+u], sh, scalef/uz, 232);
    bshape(shTortoise[12][1+u], sh+2, scalef/uz, 229);
    }

  for(int v=0; v<13; v++) for(int z=0; z<2; z++)
    copyshape(shTortoise[v][4+z], shTortoise[v][2+z], shTortoise[v][2+z].prio + (PPR_CARRIED-PPR_ITEM));

  if(purehepta) bshape(shMagicSword, PPR_MAGICSWORD, 1, 243);
  else bshape(shMagicSword, PPR_MAGICSWORD, 1, 244);
  
  bshape(shBead0, 20, 1, 250);
  bshape(shBead1, 20, 1, 251);
  bshape(shArrow, PPR_ARROW, 1, 252);
  
  bshapeend();

  prehpc = qhpc;
  DEBB(DF_INIT, (debugfile,"hpc = %d\n", qhpc));
  
  for(int i=0; i<USERSHAPEGROUPS; i++) for(int j=0; j<USERSHAPEIDS; j++) {
    usershape *us = usershapes[i][j];
    if(!us) continue;
    for(int l=0; l<USERLAYERS; l++) {
      us->d[l].sh.s = qhpc;
      first = true;
      pushShape(us->d[l]);
      us->d[l].sh.e = qhpc;
      }
    }
  
  static int qhpc0;
  if(qhpc != qhpc0)
    printf("qhpc = %d (%d+%d)\n", qhpc0 = qhpc, prehpc, qhpc-prehpc);

  initPolyForGL();
  }

void initShape(int sg, int id) {

  if(!usershapes[sg][id]) {
    usershape *us = new usershape;
    usershapes[sg][id] = us;

    for(int i=0; i<USERLAYERS; i++) {
      us->d[i].sh.prio = (sg >= 3 ? 1:50) + i;

      us->d[i].rots = 1;
      us->d[i].sym = 0;
      us->d[i].shift = C0;
      us->d[i].spin = Cx1;
      us->d[i].color = 0;
      }
    }
  }

int poly_outline;

unsigned char& part(int& col, int i) {
  unsigned char* c = (unsigned char*) &col; return c[i];
  }

void queuepolyat(const transmatrix& V, const hpcshape& h, int col, int prio) {
  polytodraw& ptd = nextptd();
  ptd.kind = pkPoly;
  ptd.u.poly.V = V;
  ptd.u.poly.cnt = h.e-h.s;
  ptd.u.poly.tab = &ourshape[3*h.s];
  ptd.u.poly.curveindex = -1;
  if(cblind) {
    // protanopia
    /* int r = (56 * part(col,3) + 43 * part(col,2)) / 100;
    int g = (58 * part(col,3) + 42 * part(col,2)) / 100;
    int b = (24 * part(col,2) + 75 * part(col,1)) / 100; */
    // deuteranopia
    /* int r = (625 * part(col,3) + 375 * part(col,2)) / 1000;
    int g = (700 * part(col,3) + 300 * part(col,2)) / 1000;
    int b = (300 * part(col,2) + 700 * part(col,1)) / 1000; 
    part(col,3) = r;
    part(col,2) = g;
    part(col,1) = b; */
    part(col,2) = part(col,3) = (part(col,2) * 2 + part(col,3) + 1)/3;
    }
  ptd.col = (darkened(col >> 8) << 8) + (col & 0xFF);
  ptd.prio = prio << PSHIFT;
  ptd.u.poly.outline = poly_outline;
  }

void addfloats(vector<GLfloat>& v, hyperpoint h) {
  for(int i=0; i<3; i++) v.push_back(h[i]);
  }

void queuereset(eModel md, int prio) {
  polytodraw& ptd = nextptd();
  ptd.kind = pkResetModel;
  ptd.col = md;
  ptd.prio = prio << PSHIFT;
  }

void queuetable(const transmatrix& V, GLfloat *f, int cnt, int linecol, int fillcol, int prio) {
  polytodraw& ptd = nextptd();
  ptd.kind = pkPoly;
  ptd.u.poly.V = V;
  ptd.u.poly.cnt = cnt;
  ptd.u.poly.tab = f;
  ptd.u.poly.curveindex = -1;
  ptd.col = fillcol;
  ptd.prio = prio << PSHIFT;
  ptd.u.poly.outline = linecol;
  }

void queuepoly(const transmatrix& V, const hpcshape& h, int col) {
  queuepolyat(V,h,col,h.prio);
  }

void queuepolyb(const transmatrix& V, const hpcshape& h, int col, int b) {
  queuepolyat(V,h,col,h.prio+b);
  }

struct qfloorinfo {
  bool special;
  transmatrix spin;
  const hpcshape *shape;
  };

qfloorinfo qfi;

int chasmg;

bool isSpecial(const hpcshape &h) {
  return
    &h != &shFloor[0] && 
    &h != &shFloor[1] && 
    &h != &shTriheptaFloor[0] && 
    &h != &shTriheptaFloor[1] && 
    &h != &shTriheptaEuc[0] && 
    &h != &shTriheptaEuc[1];
  }

void qfloor0(cell *c, const transmatrix& V, const hpcshape& h, int col) {
  extern bool wmspatial;
  if(chasmg == 2) ;
  else if(chasmg && wmspatial) {
    if(detaillevel == 0) return;
    queuepolyat(V,h, c->land == laCocytus ? 0x080808FF : 0x101010FF, PPR_LAKEBOTTOM);
    }
  else 
    queuepoly(V,h,col);
  }
  
void qfloor(cell *c, const transmatrix& V, const hpcshape& h, int col) {
  qfloor0(c, V, h, col);  
  qfi.special = isSpecial(h);
  qfi.shape = &h, qfi.spin = Id; 
  }

void qfloor(cell *c, const transmatrix& V, const transmatrix& Vspin, const hpcshape& h, int col) {
  qfloor0(c, V*Vspin, h, col);  
  qfi.special = isSpecial(h);
  qfi.shape = &h, qfi.spin = Vspin;
  }

void curvepoint(const hyperpoint& H1) {
  curvedata.push_back(H1[0]);
  curvedata.push_back(H1[1]);
  curvedata.push_back(H1[2]);
  }

void queuecurve(int linecol, int fillcol, int prio) {
  queuetable(Id, &curvedata[curvestart], (size(curvedata)-curvestart)/3, linecol, fillcol, prio);
  lastptd().u.poly.curveindex = curvestart;
  curvestart = size(curvedata);
  }

void queueline(const hyperpoint& H1, const hyperpoint& H2, int col, int prf = 0, int prio = PPR_LINE) {
  polytodraw& ptd = nextptd();
  ptd.kind = pkLine;
  ptd.u.line.H1 = H1;
  ptd.u.line.H2 = H2;
  ptd.u.line.prf = prf;
  ptd.col = (darkened(col >> 8) << 8) + (col & 0xFF);
  ptd.prio = prio << PSHIFT;
  }

void queuestr(int x, int y, int shift, int size, string str, int col, int frame = 0, int align = 8) {
  polytodraw& ptd = nextptd();
  ptd.kind = pkString;
  ptd.u.chr.x = x;
  ptd.u.chr.y = y;
  int ss = (int) str.size(); if(ss>=MAXQCHR) ss=MAXQCHR-1;
  {for(int i=0; i<ss; i++) ptd.u.chr.str[i] = str[i];} ptd.u.chr.str[ss] = 0;
  ptd.u.chr.align = align;
  ptd.u.chr.shift = shift;
  ptd.u.chr.size = size;
  ptd.col = darkened(col);
  ptd.u.chr.frame = frame;
  ptd.prio = PPR_TEXT << PSHIFT;
  }

void queuechr(int x, int y, int shift, int size, char chr, int col, int frame = 0, int align = 8) {
  polytodraw& ptd = nextptd();
  ptd.kind = pkString;
  ptd.u.chr.x = x;
  ptd.u.chr.y = y;
  ptd.u.chr.str[0] = chr;
  ptd.u.chr.str[1] = 0;
  ptd.u.chr.shift = shift;
  ptd.u.chr.size = size;
  ptd.u.chr.align = align;
  ptd.col = col;
  ptd.u.chr.frame = frame;
  ptd.prio = PPR_TEXT << PSHIFT;
  }

void queuecircle(int x, int y, int size, int color, int prio = PPR_CIRCLE) {
  polytodraw& ptd = nextptd();
  ptd.kind = pkCircle;
  ptd.u.cir.x = x;
  ptd.u.cir.y = y;
  ptd.u.cir.size = size;
  ptd.col = color;
  ptd.prio = prio << PSHIFT;
  }

#ifdef MOBILE
namespace svg {
  bool in = false;
  }
#endif

#ifndef MOBILE
// svg renderer
namespace svg {
  FILE *f;
  bool in = false;
  
  ld cta(int col) {
    // col >>= 24;
    col &= 0xFF;
    return col / 255.0;
    }
  
  bool invisible(int col) { return (col & 0xFF) == 0; }
  
  ld gamma = .5;
  
  void fixgamma(unsigned int& color) {
    unsigned char *c = (unsigned char*) (&color);
    for(int i=1; i<4; i++) c[i] = 255 * pow(float(c[i] / 255.0), float(gamma));
    }
  
  int svgsize;
  int divby = 10;
  
  const char* coord(int val) {
    static char buf[10][20];
    static int id;
    id++; id %= 10;
    if(divby == 1) {
      sprintf(buf[id], "%d", val); return buf[id];
      }
    else if(divby <= 10) {
      sprintf(buf[id], "%.1f", val*1./divby); return buf[id];
      }
    else {
      sprintf(buf[id], "%.2f", val*1./divby); return buf[id];
      }
    }
  
  char* stylestr(unsigned int fill, unsigned int stroke, ld width=1) {
    fixgamma(fill);
    fixgamma(stroke);
    static char buf[600];
    // printf("fill = %08X stroke = %08x\n", fill, stroke);
  
    if(stroke == 0xFF00FF) {
      stroke = 0x000000FF;
      
      if(fill == 0x332a22ff) fill = 0x000000FF;
      else if(fill == 0x686868FF) fill = 0x000000FF;
      else if(fill == 0xd0d0d0FF) fill = 0x000000FF;
      else fill = 0xFFFFFFFF;
      }
    
    sprintf(buf, "style=\"stroke:#%06x;stroke-opacity:%.3" PLDF ";stroke-width:%" PLDF "px;fill:#%06x;fill-opacity:%.3" PLDF "\"",
      (stroke>>8) & 0xFFFFFF, cta(stroke),
      width/divby,
      (fill>>8) & 0xFFFFFF, cta(fill)
      );
    return buf;
    }
  
  void circle(int x, int y, int size, int col) {
    int ba = (backcolor << 8) + 0xFF;
    if(!invisible(col))
    fprintf(f, "<circle cx='%s' cy='%s' r='%s' %s/>\n",
      coord(x), coord(y), coord(size), stylestr(ba, col));
    }
  
  string *info;
  
  void startstring() {
    if(info) fprintf(f, "<a xlink:href=\"%s\" xlink:show=\"replace\">", info->c_str());
    }

  void stopstring() {
    if(info) fprintf(f, "</a>");
    }

  void text(int x, int y, int size, const string& str, bool frame, int col, int align) {

    double dfc = (x - vid.xcenter) * (x - vid.xcenter) + 
      (y - vid.ycenter) * (y - vid.ycenter);
    dfc /= vid.radius;
    dfc /= vid.radius;
    // 0 = center, 1 = edge
    dfc = 1 - dfc;
    
    col = 0xFF + (col << 8);

    if(!invisible(col)) {
      startstring();
      fprintf(f, "<text x='%s' y='%s' font-family='Times' text-anchor='%s' font-size='%s' %s>%s</text>",
        coord(x), coord(y+size/2), 
        align == 8 ? "middle" :
        align < 8 ? "start" :
        "end",
        coord(size), stylestr(col, frame ? 0x0000000FF : 0, (1<<sightrange)*dfc/40), str.c_str());
      stopstring();
      fprintf(f, "\n");
      }
    }
  
  void polygon(int *polyx, int *polyy, int polyi, int col, int outline) {
  
    if(invisible(col) && invisible(outline)) return;
    double dfc = 0.8;
    if(!pmodel && !euclid) {
      int avgx = 0, avgy = 0;
      for(int i=0; i<polyi; i++) 
        avgx += polyx[i],
        avgy += polyy[i];
      avgx /= polyi;
      avgy /= polyi;
      dfc = (avgx - vid.xcenter) * (avgx - vid.xcenter) + 
        (avgy - vid.ycenter) * (avgy - vid.ycenter);
      dfc /= vid.radius;
      dfc /= vid.radius;
      // 0 = center, 1 = edge
      dfc = 1 - dfc;
      
      if(dfc < 0) dfc = 1;
      }
    
    startstring();
    for(int i=0; i<polyi; i++) {
      if(i == 0)
        fprintf(f, "<path d=\"M ");
      else
        fprintf(f, " L ");
      fprintf(f, "%s %s", coord(polyx[i]), coord(polyy[i]));
      }
    
    fprintf(f, "\" %s/>", stylestr(col, outline, vid.radius*dfc/256));
    stopstring();
    fprintf(f, "\n");
    }
  
  void render(const char *fname) {

    if(cheater) doOvergenerate();

    dynamicval<videopar> v(vid, vid);
    dynamicval<bool> v2(in, true);
    dynamicval<int> v4(cheater, 0);
    
    vid.usingGL = false;
    vid.xres = vid.yres = svgsize ? svgsize : min(1 << (sightrange+7), 16384);
    calcparam();
#ifdef ROGUEVIZ
    rogueviz::fixparam();
#endif
    inHighQual = true; 
    darken = 0;
    
    time_t timer;
    timer = time(NULL);

    char buf[128]; strftime(buf, 128, "svgshot-%y%m%d-%H%M%S.svg", localtime(&timer));
    if(!fname) fname = buf;

    f = fopen(fname, "wt");
    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"%s\" height=\"%s\">\n", coord(vid.xres), coord(vid.yres));
    drawfullmap();
    fprintf(f, "</svg>\n");
    fclose(f);
    addMessage(XLAT("Saved the SVG shot to %1 (sightrange %2)", fname, its(sightrange)));
    }
  }
#endif

void getcoord0(const hyperpoint& h, int& xc, int &yc, int &sc) {
  hyperpoint hscr;
  applymodel(h, hscr);
  xc = vid.xcenter + vid.radius * hscr[0];
  yc = vid.ycenter + vid.radius * hscr[1];
  sc = vid.eye * vid.radius * hscr[2];
  }

void queuechr(const hyperpoint& h, int size, char chr, int col, int frame = 0) {
  int xc, yc, sc; getcoord0(h, xc, yc, sc);
  queuechr(xc, yc, sc, size, chr, col, frame);
  }
  
void queuechr(const transmatrix& V, double size, char chr, int col, int frame = 0) {
  int xc, yc, sc; getcoord0(tC0(V), xc, yc, sc);
  int xs, ys, ss; getcoord0(V * xpush0(.5), xs, ys, ss);
  queuechr(xc, yc, sc, int(sqrt(squar(xc-xs)+squar(yc-ys)) * scalef * size), chr, col, frame);
  }
  
void queuestr(const hyperpoint& h, int size, const string& chr, int col, int frame = 0) {
  int xc, yc, sc; getcoord0(h, xc, yc, sc);
  queuestr(xc, yc, sc, size, chr, col, frame);
  }
  
void queuestr(const transmatrix& V, double size, const string& chr, int col, int frame = 0, int align = 8) {
  int xc, yc, sc; getcoord0(tC0(V), xc, yc, sc);
  int xs, ys, ss;  getcoord0(V * xpush0(.5), xs, ys, ss); 
  queuestr(xc, yc, sc, int(sqrt(squar(xc-xs)+squar(yc-ys)) * size), chr, col, frame, align);
  }
  
void queuecircle(const transmatrix& V, double size, int col) {
  int xc, yc, sc; getcoord0(tC0(V), xc, yc, sc);
  int xs, ys, ss; getcoord0(V * xpush0(.5), xs, ys, ss);  
  queuecircle(xc, yc, int(sqrt(squar(xc-xs)+squar(yc-ys)) * size), col);
  }

long double polydata[] = {
// shStarFloor[0] (6x1)
NEWSHAPE,   1,6,1, 0.267355,0.153145, 0.158858,0.062321, 0.357493,-0.060252,
// shStarFloor[1] (7x1)
NEWSHAPE,   2,7,1, -0.012640,0.255336, 0.152259,0.386185, 0.223982,0.275978,
// shCloudFloor[0] (6x1)
NEWSHAPE,   3,6,1, -0.249278,0.146483, -0.242058,0.146003, -0.209868,0.146655, -0.180387,0.151575, -0.148230,0.165669, -0.131141,0.187345, -0.127578,0.219788, -0.130923,0.255428, -0.116050,0.294689, -0.093271,0.316597, -0.050041,0.330532, -0.007791,0.309060, 0.017835,0.271341, 0.017835,0.271341,
// shCloudFloor[1] (7x1)
NEWSHAPE,   4,7,1, -0.216129,0.195545, -0.209066,0.211647, -0.195698,0.234578, -0.193583,0.272597, -0.202572,0.304529, -0.198965,0.336605, -0.167757,0.354608, -0.132750,0.357613, -0.104720,0.351753, -0.079702,0.340062, -0.064640,0.331114, -0.040288,0.308007, -0.023142,0.289269,
// shCrossFloor[0] (3x1)
NEWSHAPE,   5,3,1, -0.283927,0.089050, -0.363031,0.095818, -0.386170,0.136539, -0.370773,0.209082, -0.207773,0.157096, -0.213111,0.255207, -0.045690,0.195109, -0.053263,0.305287, 0.090089,0.277990,
// shCrossFloor[1] (7x2)
NEWSHAPE,   6,7,2, -0.254099,-0.080041, -0.326144,-0.061810,
// shChargedFloor[2] (3x1)
NEWSHAPE,   7,3,1, 0.259904,0.175997, 0.266043,0.049579, 0.315948,0.019620, 0.201609,-0.012441, 0.286297,-0.066513, 0.279241,-0.164471,
// shChargedFloor[0] (3x1)
NEWSHAPE,   8,3,1, 0.000420,-0.165342, 0.154528,-0.281262, -0.003359,-0.311544, -0.120635,-0.166812, -0.177022,-0.273052, -0.276778,-0.195332,
// shChargedFloor[1] (7x1)
NEWSHAPE,   9,7,1, 0.374086,-0.003058, 0.231873,-0.155490, 0.424384,-0.155026,
// shChargedFloor[3] (7x1)
NEWSHAPE,  10,7,1, -0.560276,0.275068, -0.432169,0.356327, -0.232687,0.234741, -0.397991,0.569704, -0.256078,0.505042,
// shSStarFloor[0] (6x1)
NEWSHAPE,  11,6,1, -0.261229,0.272221, -0.023065,0.198020,
// shSStarFloor[1] (7x1)
NEWSHAPE,  12,7,1, -0.236325,0.100575, -0.439825,0.136330, -0.334057,0.279698,
// shOverFloor[0] (3x1)
NEWSHAPE,  13,3,1, 0.260902,0.180699, 0.278422,0.058159, 0.257517,-0.080799, 0.198743,-0.215550, 0.062950,-0.080317, 0.110388,-0.278019, -0.043099,-0.182658,
// shOverFloor[1] (7x1)
NEWSHAPE,  14,7,1, -0.586767,0.283785, -0.323522,0.218875, -0.576300,0.603001, -0.137153,0.295524, -0.383052,0.684221,
// shOverFloor[1] (7x1)
NEWSHAPE,  15,7,1, -0.437062,0.198802, -0.261731,0.160454, -0.401949,0.300851, -0.208814,0.204608, -0.296258,0.393467, -0.159132,0.277743,
// shOverFloor[2] (6x1)
NEWSHAPE,  16,6,1, 0.008833,-0.299167, -0.020500,-0.148834, -0.121333,-0.290000, -0.119500,-0.128667, -0.236833,-0.279000,
// shTriFloor[0] (3x1)
NEWSHAPE,  17,3,1, -0.353692,0.244649,
// shTriFloor[1] (7x1)
NEWSHAPE,  18,7,1, -0.352357,0.009788,
// shFeatherFloor[0] (3x1)
NEWSHAPE,  19,3,1, 0.227689,0.169556, 0.333673,0.157023, 0.251378,0.091850, 0.316741,0.073094, 0.260919,0.038655, 0.306256,0.038890, 0.231096,-0.033702, 0.255955,-0.043464, 0.226427,-0.077082, 0.266494,-0.101752, 0.217364,-0.144909, 0.223207,-0.194093, 0.158549,-0.144135, 0.179022,-0.208053, 0.105573,-0.172756, 0.120828,-0.236822, 0.033598,-0.201587, 0.067872,-0.281182,
// shFeatherFloor[1] (7x1)
NEWSHAPE,  20,7,1, -0.130677,0.632450, -0.089355,0.521394, -0.043946,0.681315, 0.018619,0.527506, 0.055053,0.679188, 0.097948,0.501131, 0.149721,0.584970, 0.203217,0.443040, 0.242077,0.549815, 0.310522,0.431044, 0.352912,0.566221,
// shFeatherFloor[1] (7x1)
NEWSHAPE,  21,7,1, -0.097861,0.337622, -0.034295,0.357645, -0.014623,0.326578, 0.024557,0.373262, 0.063328,0.316638, 0.088328,0.358219, 0.111793,0.286773, 0.146987,0.328271, 0.170806,0.287930, 0.191056,0.303731,
// shFeatherFloor[2] (6x1)
NEWSHAPE,  22,6,1, 0.004667,-0.288667, -0.019167,-0.217167, -0.088833,-0.303333, -0.090667,-0.187833, -0.153000,-0.235500, -0.171333,-0.132833, -0.233667,-0.211667,
// shBarrowFloor[0] (6x2)
NEWSHAPE,  23,6,2, 0.272054,0.153801, 0.270633,0.093140, 0.239314,0.098622, 0.185341,0.089779, 0.198868,0.083706, 0.239635,0.081872, 0.256105,0.076570, 0.277257,0.058659,
// shBarrowFloor[1] (7x2)
NEWSHAPE,  24,7,2, 0.298659,0.140526, 0.320688,0.102373, 0.327535,0.088055, 0.293375,0.066852, 0.270939,0.063345, 0.242988,0.065298, 0.250251,0.054365, 0.285707,0.056115, 0.321178,0.061438, 0.343446,0.057906, 0.382513,0.007427,
// shBarrowFloor[2] (7x2)
NEWSHAPE,  25,7,2, 0.506507,0.238008, 0.558886,0.149881, 0.443577,0.020402, 0.572763,0.108813, 0.630577,0.009183,
// shNewFloor[0] (3x1)
NEWSHAPE,  26,3,1, -0.243151,-0.202057, -0.249456,-0.026167, -0.208097,0.124704,
// shNewFloor[1] (7x1)
NEWSHAPE,  27,7,1, -0.343473,0.068811, -0.371524,0.213003, -0.243649,0.253621,
// shTrollFloor[0] (3x1)
NEWSHAPE,  28,3,1, -0.262252,-0.145851, -0.264695,-0.107665, -0.284349,-0.038417, -0.297250,0.003588, -0.274408,0.048950, -0.266188,0.105083, -0.261804,0.148400, -0.241599,0.174935, -0.209702,0.203376, -0.168334,0.233368, -0.124537,0.231186, -0.074296,0.239577, -0.051790,0.275001, -0.021701,0.311779,
// shTrollFloor[1] (7x2)
NEWSHAPE,  29,7,2, -0.319274,-0.152751, -0.312576,-0.088525, -0.288942,-0.033872,
// shTriheptaFloor[0] (3x1)
NEWSHAPE,  30,3,1, -0.139445,0.241954,
// shTriheptaFloor[1] (7x1)
NEWSHAPE,  31,7,1, 0.555765,-0.002168,
// shTriheptaFloor[2] (1x2)
NEWSHAPE,  32,1,2, 0.276938,-0.000093, -0.141904,0.242007, -0.293711,0.146334,
// shTriheptaFloor[3] (1x2)
NEWSHAPE,  33,1,2, -0.351825,0.160017, -0.123908,0.541824, 0.350858,0.435495, 0.559842,-0.000857,
// shTriheptaFloor[4] (1x1)
NEWSHAPE,  34,1,1, 0.267620,0.003973, -0.007632,0.337777, -0.271967,0.165630, -0.292806,-0.156142, -0.141920,-0.243873,
// shTriheptaFloor[5] (1x2)
NEWSHAPE,  35,1,2, 0.376029,0.016747, 0.233909,-0.274206, -0.115849,-0.540138, -0.486044,-0.233574,
// shTriheptaFloor[6] (1x2)
NEWSHAPE,  36,1,2, 0.136841,-0.241418, -0.007877,-0.337310, -0.282188,-0.183806,
// shTriheptaFloor[7] (1x2)
NEWSHAPE,  37,1,2, 0.562869,0.005892, 0.346218,0.424587, -0.098836,0.362115, -0.344149,0.171831,
// shTriheptaFloor[9] (1x2)
NEWSHAPE,  38,1,2, 0.384569,0.008580, 0.234427,0.298447, -0.070055,0.365304, -0.492813,0.235557,
// shTriheptaFloor[10] (1x2)
NEWSHAPE,  39,1,2, 0.286495,0.001984, -0.014828,0.337687, -0.282562,0.156802,
// shTriheptaFloor2[0] (3x1)
NEWSHAPE,  40,3,1, -0.134870,0.238185, -0.014606,0.184459, 0.080296,0.256902, 0.180115,0.200011, 0.166151,0.081439, 0.273667,0.005344,
// shTriheptaFloor2[1] (7x2)
NEWSHAPE,  41,7,2, -0.499701,-0.228679, -0.472331,-0.116951, -0.361356,-0.069085,
// shTriheptaEuc[0] (3x1)
NEWSHAPE,  42,3,1, -0.232333,-0.000167,
// shTriheptaEuc[1] (6x1)
NEWSHAPE,  43,6,1, 0.002000,-0.439500,
// shTriheptaEuc[2] (3x1)
NEWSHAPE,  44,3,1, -0.121167,-0.214667,
// shPalaceFloor[0] (3x2)
NEWSHAPE,  45,3,2, 0.118361,0.205152, 0.273318,0.190450,
// shPalaceFloor[1] (7x1)
NEWSHAPE,  46,7,1, -0.225907,0.287688,
// shPalaceGate (1x2)
NEWSHAPE,  47,1,2, -0.219482,-0.025012, 0.135153,-0.049012,
// shSemiFeatherFloor[0] (1x1)
NEWSHAPE,  48,1,1, -0.213723,0.142000, -0.217805,0.186740, -0.168303,0.178633, -0.165430,0.203978, -0.105630,0.181755, -0.121309,0.236968, -0.041402,0.211323, -0.057879,0.276895, -0.017811,0.253866, 0.017667,0.368934, 0.047389,0.274022, 0.080505,0.307761, 0.077701,0.240665, 0.118416,0.268671, 0.146146,0.160920, 0.177400,0.199591, 0.182840,0.137845, 0.210035,0.188620, 0.224749,0.113626, 0.281767,0.097057, 0.212978,0.071571, 0.267866,0.042710, 0.209124,0.016525, 0.255362,-0.017839, 0.214280,-0.046220, 0.270979,-0.083301, 0.265581,-0.128638, 0.025000,0.006441,
// shSemiFeatherFloor[1] (1x1)
NEWSHAPE,  49,1,1, -0.260056,0.104281, 0.016285,-0.045687, 0.294213,-0.200108, 0.203762,-0.171240, 0.218578,-0.231670, 0.162724,-0.207729, 0.184841,-0.241991, 0.080474,-0.212574, 0.082238,-0.239152, 0.040630,-0.227735, 0.050962,-0.289632, -0.023848,-0.250638, -0.047106,-0.286037, -0.055913,-0.224791, -0.096323,-0.258790, -0.110786,-0.189815, -0.154769,-0.220600, -0.166545,-0.156525, -0.220248,-0.191486, -0.226246,-0.155963, -0.331380,-0.170431, -0.264008,-0.097302, -0.308875,-0.070140, -0.282501,-0.040835, -0.303566,-0.023046, -0.264959,0.005751, -0.231085,0.042569, -0.257805,0.048589, -0.232591,0.068009,
// shDemonFloor[1] (7x2)
NEWSHAPE,  50,7,2, -0.226625,-0.098204, -0.248360,-0.079830, -0.282817,-0.064277, -0.267165,-0.033078, -0.265427,-0.008890, -0.192052,-0.006156, -0.183253,-0.047966, -0.143185,-0.036403, -0.143048,-0.019396, -0.166019,-0.021973,
// shDemonFloor[0] (3x2)
NEWSHAPE,  51,3,2, -0.301493,-0.143505, -0.286951,-0.089187, -0.265835,-0.083073, -0.254617,-0.091662, -0.235025,-0.098559, -0.212515,-0.090006, -0.195614,-0.068094, -0.191005,-0.038201, -0.190797,-0.019695, -0.209527,-0.022317, -0.209825,-0.043455, -0.217026,-0.061116, -0.229748,-0.071561, -0.245216,-0.072048, -0.259160,-0.060979, -0.268975,-0.048441, -0.279355,-0.049976, -0.293619,-0.069848, -0.299119,-0.088435, -0.309915,-0.111622, -0.326351,-0.118070, -0.344537,-0.119314, -0.371612,-0.115703, -0.386986,-0.108411, -0.395533,-0.101984, -0.372081,-0.100489, -0.352370,-0.089767, -0.339559,-0.070299, -0.338745,-0.051606, -0.343230,-0.037106, -0.346305,-0.026537, -0.349398,-0.006643, -0.344437,0.011923,
// shCaveFloor[0] (3x1)
NEWSHAPE,  52,3,1, -0.234236,0.156157, -0.104249,0.246637, -0.046397,0.291271, 0.016871,0.307576, 0.102393,0.312431, 0.175295,0.270792, 0.252411,0.200098,
// shCaveFloor[1] (7x1)
NEWSHAPE,  53,7,1, -0.275790,0.020429, -0.270081,0.108032, -0.241466,0.175612,
// shCaveFloor[2] (6x2)
NEWSHAPE,  54,6,2, -0.240110,-0.132397, -0.245549,-0.126958, -0.248269,-0.113359, -0.250989,-0.099760, -0.253709,-0.081628, -0.253709,-0.073469, -0.253709,-0.058057, -0.253709,-0.039018,
// shDesertFloor[0] (3x1)
NEWSHAPE,  55,3,1, -0.122282,0.287492, -0.111151,0.302069, -0.092261,0.322915, -0.071518,0.368383, -0.014386,0.321713, 0.014126,0.287661, 0.052673,0.283918, 0.144166,0.292263, 0.201841,0.240906, 0.236802,0.171166, 0.187940,0.142834, 0.179062,0.090148, 0.306355,-0.036347,
// shDesertFloor[1] (7x2)
NEWSHAPE,  56,7,2, -0.336141,-0.153280, -0.336530,-0.140779, -0.323000,-0.116492, -0.295897,-0.085654, -0.271537,-0.031871,
// shPowerFloor[0] (3x1)
NEWSHAPE,  57,3,1, 0.188696,0.349290, 0.246724,0.011920,
// shPowerFloor[1] (7x1)
NEWSHAPE,  58,7,1, 0.563851,-0.000804, 0.151110,-0.073482,
// shFan (6x1)
NEWSHAPE,  59,6,1, 0.107525,0.021206, 0.174486,0.023794, 0.248649,0.075227, 0.288526,0.125253, 0.316075,0.194021, 0.330682,0.132382, 0.308763,0.072356, 0.265689,0.011475, 0.200080,-0.044306, 0.135111,-0.076768,
// shElementalShard (3x1)
NEWSHAPE,  60,3,1, -0.345978,0.004270, -0.054981,0.027419, -0.063949,0.034031, -0.078659,0.057318, -0.103108,0.086449, -0.130880,0.100059, -0.157829,0.107491, -0.173912,0.106860, -0.191566,0.100397, -0.200633,0.095930, -0.216136,0.090593, -0.200642,0.111259, -0.173354,0.128719, -0.141916,0.144399, -0.106497,0.138938, -0.079427,0.124373, -0.053552,0.106563, -0.030995,0.078798, -0.024668,0.068596, -0.027285,0.085742, -0.033561,0.111171, -0.036750,0.137167, -0.035531,0.165577, -0.023882,0.199023, -0.002982,0.230118, 0.021321,0.244715, 0.012442,0.234213, -0.002073,0.208126, -0.005433,0.188040, -0.004352,0.170363, 0.001158,0.155057, 0.002408,0.141554, 0.008798,0.125672, 0.018292,0.110314, 0.026174,0.100337,
// shNecro (1x2)
NEWSHAPE,  61,1,2, -0.280120,0.002558, -0.252806,0.037921, -0.063204,0.010733, 0.000000,0.061995, 0.057203,0.013109, 0.155766,0.023121, 0.174234,0.055215, 0.256152,0.057064, 0.286380,0.030821, 0.251170,0.025243, 0.227431,0.033740, 0.210837,0.017363, 0.227179,0.007489, 0.245429,0.020138, 0.286335,0.028248, 0.289108,0.001285, 0.189404,0.003690, 0.186892,0.025821, 0.176349,0.017145, 0.184160,0.011050, 0.177605,0.006124, 0.184138,0.001228,
// shFigurine (1x2)
NEWSHAPE,  62,1,2, -0.105008,0.082180, -0.018191,0.040930, -0.011407,0.123195, 0.041081,0.123243, 0.040941,0.038667, 0.070555,0.027312, 0.086601,0.059253, 0.134876,0.073153, 0.190524,0.064273, 0.211472,0.036778,
// shStatue (8x1)
NEWSHAPE,  63,8,1, -0.047663,0.032172, -0.047670,0.034561, -0.054881,0.039371, -0.080231,0.043109, -0.103440,0.045706, -0.148323,0.042552, -0.188278,0.031995, -0.239693,0.002510, -0.257087,-0.036727, -0.283314,-0.026921, -0.290985,0.034764, -0.262852,0.081663, -0.198209,0.104714, -0.140842,0.107775, -0.088218,0.099094, -0.057645,0.088869, -0.035877,0.075342, -0.023854,0.060828, -0.022642,0.053625, -0.011901,0.044034, -0.007144,0.050008,
// shBook (2x1)
NEWSHAPE,  64,2,1, -0.123296,-0.203475, 0.123296,-0.203475,
// shBookCover (3x1)
NEWSHAPE,  65,3,1, 0.018255,-0.016768, 0.010865,-0.065298, 0.044639,-0.075918, 0.027526,-0.106818, -0.029540,-0.088203,
// shGrail (1x2)
NEWSHAPE,  66,1,2, -0.154537,0.122394, -0.142973,0.123252, -0.138296,0.095461, -0.123926,0.072898, -0.107625,0.065300, -0.067188,0.057589, -0.040655,0.052612, -0.037133,0.069475, -0.021657,0.101065, 0.003631,0.129510, 0.040272,0.153766, 0.087340,0.161149, 0.127500,0.154732, 0.161853,0.141932, 0.184896,0.152194, 0.189249,0.157707,
// shGun (1x1)
NEWSHAPE,  67,1,1, -0.139431,-0.044738, -0.115996,-0.007279, -0.057715,-0.031033, -0.045084,-0.014605, -0.066569,-0.008222, -0.070986,0.003205, -0.070744,0.009636, -0.066988,0.002251, -0.058525,-0.008510, -0.045683,-0.008971, -0.043249,-0.008257, -0.031506,0.005726, -0.020012,0.013281, 0.001173,0.003679, 0.012717,-0.007112, 0.093390,0.118437, 0.127945,0.108856, -0.004226,-0.087963, -0.020241,-0.089138, -0.029416,-0.102578, -0.027650,-0.127629, -0.035592,-0.124229, -0.041465,-0.103099, -0.041059,-0.088601,
// shKey (1x1)
NEWSHAPE,  68,1,1, -0.280212,-0.017913, -0.279685,0.060299, -0.258790,0.115302, -0.228319,0.104699, -0.239075,0.056623, -0.213944,0.046023, -0.200310,0.089579, -0.168273,0.078609, -0.179201,0.040504, -0.150719,0.029171, -0.146064,0.060860, -0.115859,0.053102, -0.119288,0.019279, -0.087371,0.011969, -0.080285,0.050328, -0.068304,0.065908, -0.052728,0.079091, -0.033525,0.083811, -0.007181,0.087363, 0.025157,0.089847, 0.044329,0.082667, 0.061077,0.068263, 0.074262,0.055098, 0.088682,0.034754, 0.093450,0.008387, 0.092228,-0.005989, 0.092278,-0.023968, 0.085107,-0.045550, 0.070661,-0.058685, 0.045297,-0.040529, 0.057205,-0.014301, 0.057207,0.015494, 0.041704,0.039321, 0.023823,0.048837, 0.003575,0.058394, -0.021441,0.050029, -0.047692,0.040538, -0.054813,0.017874, -0.054802,-0.010722, -0.036924,-0.039306, -0.020254,-0.052421, 0.011915,-0.056002, 0.034561,-0.047670, 0.045297,-0.040529, 0.070661,-0.058685, 0.055051,-0.068215, 0.033505,-0.080172, 0.011971,-0.088587, -0.010772,-0.087369, -0.035904,-0.080185, -0.061039,-0.063432, -0.071750,-0.041854, -0.082525,-0.015548, -0.280212,-0.017913,
// shGhost (1x2)
NEWSHAPE,  69,1,2, -0.312167,0.001301, -0.185499,0.015970, -0.273136,0.038290, -0.184484,0.043046, -0.296331,0.093578, -0.174561,0.070070, -0.222751,0.104454, -0.162371,0.098407, -0.272077,0.156217, -0.140182,0.126656, -0.161729,0.148045, -0.119271,0.146323, -0.231927,0.213686, -0.001244,0.218928, 0.016184,0.220354, 0.031094,0.216414, 0.045998,0.212585, 0.064748,0.211675, 0.079530,0.201310, 0.094597,0.199151, 0.110770,0.190424, 0.123174,0.181651, 0.132996,0.171528, 0.149284,0.160480, 0.156499,0.147805, 0.171645,0.135575, 0.180157,0.119276, 0.188931,0.106895, 0.197759,0.093283, 0.201351,0.080789, 0.209030,0.067188, 0.212718,0.052246, 0.215145,0.037308, 0.217714,0.026126, 0.217612,0.014922,
// shGargoyleWings (1x2)
NEWSHAPE,  70,1,2, 0.042241,0.002223, 0.041129,-0.007781, 0.045593,-0.034473, 0.066799,-0.059006, 0.116090,-0.071440, 0.169203,-0.075077, 0.154517,-0.085096, 0.124220,-0.116387, 0.104146,-0.144461, 0.082994,-0.176082, 0.049446,-0.209023, 0.004505,-0.235397, -0.039494,-0.248247, -0.071220,-0.256618, -0.116784,-0.264180, -0.160337,-0.264954, -0.177592,-0.262973, -0.159002,-0.255539, -0.137018,-0.243463, -0.124173,-0.222383, -0.128481,-0.203992, -0.160238,-0.194091, -0.184305,-0.194482, -0.160187,-0.190645, -0.117943,-0.174106, -0.108751,-0.156961, -0.107466,-0.136571, -0.120870,-0.120870, -0.143330,-0.104138, -0.156871,-0.097484, -0.143266,-0.095138, -0.119536,-0.087138, -0.103741,-0.071392, -0.091360,-0.051251, -0.083500,-0.032287, -0.077900,-0.015580,
// shGargoyleBody (1x2)
NEWSHAPE,  71,1,2, 0.128302,-0.002231, 0.128305,-0.010041, 0.136196,-0.021211, 0.146366,-0.031284, 0.164504,-0.042525, 0.161107,-0.043633, 0.143009,-0.042456, 0.127212,-0.033477, 0.114839,-0.025644, 0.108151,-0.046828, 0.094666,-0.021161, 0.081255,-0.022262, 0.068979,-0.022251, 0.057838,-0.028919, 0.082541,-0.093696, 0.108462,-0.117407, 0.080377,-0.111634, 0.074909,-0.139757, 0.066894,-0.095881, 0.056798,-0.077958, 0.043402,-0.066772, 0.031144,-0.056726, 0.011119,-0.053373, -0.024478,-0.070096, -0.007782,-0.046693, -0.062361,-0.071270, -0.041154,-0.050052, -0.097005,-0.068015, -0.066772,-0.043402, -0.120555,-0.063626, -0.092454,-0.038987, -0.144240,-0.068206, -0.151438,-0.125638, -0.186951,-0.141902, -0.209987,-0.145636, -0.230971,-0.155113, -0.216796,-0.137756, -0.243372,-0.131308, -0.209646,-0.120603, -0.188968,-0.119230, -0.167228,-0.113356, -0.164543,-0.052609, -0.121573,-0.023422, -0.142949,-0.011168, -0.187169,-0.007845, -0.243507,-0.003382,
// shDogStripes (1x2)
NEWSHAPE,  72,1,2, -0.147497,-0.001105, -0.159242,0.009400, -0.153105,0.023214, -0.135232,0.011039, -0.110779,0.007165, -0.119793,0.067901, -0.099799,0.063959, -0.090296,0.011012, -0.047281,0.012095, -0.058915,0.070478, -0.041289,0.078174, -0.021982,0.013189, 0.011539,0.012638, -0.001651,0.079245, 0.011562,0.089191, 0.034627,0.013741, 0.043979,0.012094, 0.052798,0.032449, 0.058855,0.029703, 0.077053,0.029170, 0.066006,0.005501,
// shWolf (1x2)
NEWSHAPE,  73,1,2, -0.310601,0.000000, -0.158251,0.009739, -0.149626,0.045009, -0.173168,0.066320, -0.250414,0.056912, -0.242122,0.064314, -0.253563,0.065926, -0.241104,0.075740, -0.249897,0.082453, -0.237118,0.081982, -0.246310,0.097762, -0.230366,0.088118, -0.210870,0.082352, -0.194727,0.083100, -0.180298,0.087679, -0.162032,0.087154, -0.135091,0.081542, -0.116083,0.068924, -0.106144,0.066340, -0.084057,0.063643, -0.061108,0.071892, -0.044295,0.077815, -0.021516,0.077698, 0.000000,0.078872, 0.025203,0.099613, 0.040964,0.113252, 0.067885,0.127285, 0.086481,0.135203, 0.104129,0.144556, 0.097579,0.132951, 0.112604,0.134635, 0.098603,0.124167, 0.118341,0.115901, 0.094688,0.115325, 0.079806,0.108826, 0.065011,0.097516, 0.053964,0.082746, 0.049028,0.066966, 0.045353,0.053708, 0.046494,0.040534, 0.051260,0.033378, 0.059646,0.029823, 0.069275,0.029860, 0.077732,0.029897, 0.081360,0.028715, 0.066249,0.039649, 0.106662,0.046229, 0.146020,0.025919, 0.159666,0.028110, 0.172131,0.020186, 0.172077,0.013438, 0.168613,0.007837,
// shWolf1 (1x1)
NEWSHAPE,  74,1,1, 0.121226,-0.019639, 0.119731,-0.021190, 0.117841,-0.022233, 0.115725,-0.022676, 0.113570,-0.022480, 0.111568,-0.021661, 0.109897,-0.020293, 0.108705,-0.018496, 0.108099,-0.016432, 0.108131,-0.014283, 0.108800,-0.012240, 0.110046,-0.010484, 0.111757,-0.009172, 0.113783,-0.008421, 0.115942,-0.008296, 0.118044,-0.008810, 0.119902,-0.009915, 0.121349,-0.011516, 0.122258,-0.013468, 0.122549,-0.015598, 0.122194,-0.017718,
// shWolf2 (1x1)
NEWSHAPE,  75,1,1, 0.110896,0.008835, 0.109780,0.010595, 0.109236,0.012605, 0.109313,0.014686, 0.110004,0.016653, 0.111247,0.018331, 0.112933,0.019572, 0.114911,0.020264, 0.117005,0.020346, 0.119030,0.019812, 0.120806,0.018708, 0.122174,0.017133, 0.123014,0.015226, 0.123250,0.013158, 0.122862,0.011111, 0.121884,0.009268, 0.120403,0.007792, 0.118550,0.006815, 0.116491,0.006423, 0.114408,0.006652, 0.112486,0.007480,
// shWolf3 (1x1)
NEWSHAPE,  76,1,1, 0.157399,-0.008998, 0.154869,-0.009703, 0.152240,-0.009639, 0.149747,-0.008812, 0.147612,-0.007296, 0.146023,-0.005225, 0.145123,-0.002783, 0.144991,-0.000188, 0.145638,0.002331, 0.147008,0.004549, 0.148979,0.006269, 0.151375,0.007339, 0.153983,0.007663, 0.156573,0.007212, 0.158913,0.006027, 0.160796,0.004213, 0.162055,0.001931, 0.162577,-0.000616, 0.162316,-0.003201, 0.161296,-0.005596, 0.159607,-0.007588,
// shHawk (1x2)
NEWSHAPE,  77,1,2, 0.142456,-0.001557, 0.116236,0.010199, 0.104323,0.033324, 0.091361,0.040882, 0.082569,0.043693, 0.074592,0.041969, 0.061035,0.040602, 0.050686,0.036782, 0.044666,0.030442, 0.036067,0.018664, 0.027492,0.024803, 0.023952,0.039607, 0.029039,0.130828, 0.010152,0.198985, -0.016340,0.244502, -0.044325,0.274285, -0.071191,0.291142, -0.095838,0.290953, -0.163452,0.314668, -0.126557,0.290164, -0.115484,0.276976, -0.097565,0.262090, -0.139857,0.285325, -0.169577,0.286959, -0.204455,0.287946, -0.165758,0.267094, -0.137259,0.245199, -0.122713,0.231981, -0.105482,0.206850, -0.103989,0.192625, -0.136578,0.214149, -0.178626,0.222509, -0.216644,0.228276, -0.178075,0.209345, -0.154479,0.186094, -0.140331,0.166218, -0.129767,0.146790, -0.125089,0.126774, -0.125855,0.112996, -0.151531,0.130982, -0.179485,0.133004, -0.192671,0.128132, -0.161665,0.115486, -0.138366,0.095450, -0.128650,0.083339, -0.121392,0.069153, -0.120756,0.059987, -0.121451,0.047500, -0.122143,0.035028, -0.141470,0.038442, -0.175470,0.051892, -0.204042,0.065126, -0.234854,0.089092, -0.277604,0.102438, -0.297591,0.091976, -0.278633,0.079023, -0.292616,0.065802, -0.251306,0.044753, -0.294009,0.035431, -0.266622,0.025835, -0.301490,0.006576,
// shEagle (1x2)
NEWSHAPE,  78,1,2, -0.153132,0.000000, -0.151960,0.025529, -0.192201,0.028337, -0.204119,0.019793, -0.201539,0.029674, -0.208386,0.038452, -0.200321,0.038333, -0.199208,0.050730, -0.192349,0.039456, -0.149495,0.034031, -0.148542,0.057225, -0.089847,0.025157, -0.048881,0.038151, 0.002416,0.130440, -0.028450,0.202864, -0.044701,0.209848, -0.044823,0.216644, -0.039848,0.217916, -0.028602,0.216381, -0.018604,0.210847, -0.016122,0.210828, -0.014922,0.217612, -0.025023,0.231467, -0.035205,0.241403, -0.041644,0.248604, -0.041754,0.254318, -0.034211,0.258480, -0.021481,0.253982, -0.011317,0.243946, 0.001251,0.232708, 0.008728,0.224428, 0.009969,0.223059, 0.011289,0.238321, 0.003802,0.261058, -0.005116,0.280126, -0.007740,0.296717, 0.001286,0.290619, 0.012793,0.280166, 0.022854,0.264095, 0.028906,0.241303, 0.031328,0.234335, 0.032584,0.234354, 0.034119,0.252735, 0.032108,0.286405, 0.030917,0.292424, 0.039807,0.285067, 0.050896,0.264659, 0.058073,0.246178, 0.063739,0.221211, 0.083520,0.165812, 0.083035,0.146532, 0.075285,0.129928, 0.066581,0.122266, 0.055484,0.112174, 0.044365,0.087531, 0.034631,0.065680, 0.034631,0.065680, 0.026221,0.053634, 0.022616,0.041661, 0.021411,0.033306, 0.022591,0.024969, 0.026158,0.021402, 0.028538,0.020215, 0.034494,0.017842, 0.040463,0.020231, 0.042853,0.021427, 0.044051,0.022621, 0.051232,0.023829, 0.058426,0.023847, 0.064425,0.020282, 0.068025,0.015514, 0.071634,0.011939, 0.089791,0.002394,
// shTentHead (1x2)
NEWSHAPE,  79,1,2, -0.153133,-0.001215, -0.153144,0.008508, -0.148132,0.023070, -0.142123,0.052233, -0.146204,0.068228, -0.177590,0.085095, -0.224851,0.089187, -0.262047,0.120747, -0.255264,0.167153, -0.229967,0.174723, -0.184192,0.147855, -0.138732,0.121544, -0.108401,0.118145, -0.102300,0.123003, -0.091928,0.154439, -0.090798,0.199010, -0.056384,0.229293, -0.011331,0.246774, 0.024027,0.255446, 0.073005,0.235380, 0.151935,0.187094, 0.146867,0.164292, 0.094648,0.162253, 0.044142,0.175341, -0.003641,0.148054, -0.002399,0.100774, 0.047859,0.071788, 0.098991,0.082090, 0.151038,0.106832, 0.199477,0.131730, 0.255339,0.129594, 0.264313,0.081720, 0.226731,0.063886, 0.168813,0.041592, 0.128034,0.026573, 0.108135,0.002403,
// shWormHead (1x2)
NEWSHAPE,  80,1,2, -0.212131,-0.004962, -0.213579,0.021110, -0.208766,0.057162, -0.193784,0.094408, -0.171245,0.126572, -0.149103,0.156559, -0.121949,0.182923, -0.099674,0.199349, -0.073594,0.213299, -0.039892,0.220654, 0.000000,0.225787, 0.051224,0.223636, 0.099986,0.207472, 0.126101,0.189776, 0.144643,0.172075, 0.260938,0.181352, 0.170506,0.139392, 0.241779,0.141997, 0.188983,0.108168, 0.239385,0.101327, 0.202657,0.079571, 0.238058,0.069276, 0.208736,0.055911, 0.237283,0.040175, 0.212303,0.028555, 0.235548,0.015035, 0.216200,0.001243,
// shWaterElemental (1x2)
NEWSHAPE,  81,1,2, 0.033277,-0.002494, 0.030600,0.009657, 0.028387,0.043359, 0.023906,0.062967, 0.019706,0.086354, 0.012812,0.106509, 0.005882,0.110737, -0.011019,0.128245, -0.022186,0.143031, -0.030554,0.165535, -0.031448,0.191850, -0.019105,0.210887, -0.011140,0.218374, 0.003294,0.228201, 0.022226,0.229526, 0.030754,0.226143, 0.045164,0.218293, 0.048550,0.213967, 0.049437,0.209816, 0.066512,0.191298, 0.088299,0.195587, 0.104559,0.227968, 0.106012,0.240682, 0.089373,0.265934, 0.046213,0.273931, 0.013854,0.270257, -0.022699,0.280947, -0.082207,0.292512, -0.094232,0.286583, -0.060514,0.273212, -0.030455,0.255305, -0.028453,0.245093, -0.035130,0.238686, -0.050479,0.236064, -0.095235,0.239928, -0.156131,0.242848, -0.180731,0.231002, -0.161568,0.228039, -0.140105,0.226302, -0.114110,0.211717, -0.104006,0.203643, -0.096764,0.190676, -0.097207,0.178784, -0.104028,0.166435, -0.109337,0.160521, -0.119778,0.153916, -0.145551,0.154306, -0.196422,0.144903, -0.238648,0.130576, -0.251825,0.121707, -0.243461,0.108932, -0.227615,0.116160, -0.199823,0.119359, -0.155889,0.119514, -0.126863,0.107638, -0.121879,0.082303, -0.130612,0.062191, -0.149053,0.051540, -0.176729,0.044071, -0.213072,0.040955, -0.257325,0.030738, -0.282810,0.025058, -0.296088,0.010521,
// shMouse (1x2)
NEWSHAPE,  82,1,2, -0.180666,0.000000, -0.063349,-0.003586, -0.052534,-0.013134, -0.040561,-0.021474, -0.020260,-0.026219, -0.001191,-0.028595, 0.025033,-0.026225, 0.045342,-0.015512, 0.056152,-0.020310, 0.077865,-0.014375, 0.094893,-0.002402, 0.101018,-0.000000,
// shMouseLegs (1x2)
NEWSHAPE,  83,1,2, -0.033372,-0.005959, -0.047793,-0.037040, -0.003572,-0.011907, 0.033422,-0.039390, 0.026209,-0.004765,
// shMouseEyes (1x2)
NEWSHAPE,  84,1,2, 0.073016,-0.011970, 0.065770,-0.013154, 0.065764,-0.008370, 0.071797,-0.003590,
// shPBody (1x2)
NEWSHAPE,  85,1,2, -0.127943,0.000000, -0.121732,0.008437, -0.120752,0.047093, -0.114785,0.065246, -0.096531,0.082051, -0.079664,0.100183, -0.087015,0.156872, -0.090442,0.188317, -0.085023,0.215058, -0.078296,0.241201, -0.070101,0.263835, -0.062700,0.273833, -0.053763,0.276497, -0.030638,0.274461, -0.015319,0.275737, 0.001277,0.277150, 0.020384,0.271369, 0.038101,0.262896, 0.045596,0.255842, 0.062388,0.263558, 0.085371,0.258660, 0.084235,0.228817, 0.071073,0.213220, 0.048603,0.218088, 0.042541,0.228972, 0.028749,0.228742, 0.011222,0.224439, -0.012498,0.229969, -0.026261,0.230095, -0.024880,0.217700, -0.022225,0.198787, -0.020850,0.180288, -0.021870,0.150662, -0.022997,0.136774, -0.004819,0.120485, 0.007204,0.104455, 0.016748,0.083741, 0.026225,0.054833, 0.033323,0.030943, 0.034483,0.001189, 0.034483,-0.001189,
// shYeti (1x2)
NEWSHAPE,  86,1,2, -0.146785,0.001213, -0.119261,0.012047, -0.134264,0.022982, -0.116850,0.026502, -0.128124,0.037470, -0.114489,0.040975, -0.134526,0.049690, -0.108483,0.056652, -0.124869,0.072739, -0.100038,0.069906, -0.116319,0.082393, -0.086776,0.085571, -0.105410,0.095717, -0.078461,0.101396, -0.084756,0.111394, -0.078850,0.123734, -0.094972,0.127846, -0.081624,0.138883, -0.102733,0.139423, -0.083221,0.154204, -0.115566,0.148760, -0.081064,0.167041, -0.093630,0.170012, -0.081530,0.184060, -0.104404,0.190164, -0.078114,0.195904, -0.088688,0.211101, -0.074815,0.211977, -0.081589,0.225940, -0.067678,0.226847, -0.091419,0.248863, -0.059306,0.244796, -0.060896,0.256271, -0.051997,0.257446, -0.058919,0.276663, -0.042072,0.270280, -0.042449,0.288141, -0.020355,0.268437, -0.007716,0.290637, 0.001272,0.268323, 0.017995,0.289206, 0.021598,0.265534, 0.038374,0.277573, 0.037997,0.257112, 0.048725,0.280810, 0.050653,0.254534, 0.069009,0.269645, 0.085977,0.273328, 0.075050,0.258222, 0.097049,0.259223, 0.103016,0.248003, 0.079575,0.241251, 0.091636,0.222186, 0.070376,0.232494, 0.072102,0.205119, 0.060388,0.237778, 0.058115,0.195365, 0.045217,0.237388, 0.037611,0.234439, 0.034796,0.213746, 0.023715,0.225917, 0.027226,0.204191, 0.012420,0.214873, 0.020917,0.189485, 0.003700,0.197357, 0.009817,0.182844, -0.011065,0.188106, 0.003663,0.168520, -0.023305,0.180308, -0.004879,0.164654, -0.034247,0.170012, -0.004861,0.153136, -0.027964,0.151980, -0.003639,0.146787, -0.015710,0.131722, 0.010889,0.136717, -0.003615,0.120484, 0.027708,0.116858, 0.007202,0.103229, 0.034761,0.089899, 0.015548,0.082525, 0.044270,0.074183, 0.021490,0.069245, 0.051357,0.054940, 0.027405,0.050043, 0.053662,0.034582, 0.034521,0.033331, 0.060848,0.029827, 0.041653,0.017851, 0.065617,0.015510, 0.051204,0.004763,
// shKnife (1x2)
NEWSHAPE,  87,1,2, -0.086373,0.015595, -0.063364,0.015542, -0.061008,0.033495, -0.041796,0.037020, -0.038168,0.021470, 0.085206,0.028802, 0.158634,0.003661,
// shTongue (1x2)
NEWSHAPE,  88,1,2, -0.036955,0.001192, -0.036957,-0.007153, -0.048967,-0.028663, -0.061094,-0.050313, -0.064717,-0.050336, -0.049075,-0.055059, -0.022708,-0.058563, 0.011941,-0.053734, 0.029846,-0.044173, 0.046581,-0.033443, 0.062159,-0.015540, 0.066968,-0.005979,
// shFlailMissile (21x1)
NEWSHAPE,  89,21,1, -0.064558,0.008369, -0.038156,0.011924,
// shPSword (1x1)
NEWSHAPE,  90,1,1, 0.093822,0.244697, 0.105758,0.251015, 0.110908,0.249862, 0.110690,0.245554, 0.113376,0.247134, 0.117228,0.245924, 0.127263,0.237981, 0.131886,0.226997, 0.116494,0.231721, 0.106117,0.231182, 0.105927,0.226986, 0.263283,-0.174653, 0.086104,0.209645, 0.079571,0.202657, 0.074206,0.190462, 0.068951,0.179766, 0.065727,0.200902, 0.068444,0.209067, 0.077641,0.221653, 0.086737,0.227526, 0.086260,0.248631, 0.086431,0.252937,
// shPKnife (1x1)
NEWSHAPE,  91,1,1, 0.070235,0.261061, 0.085110,0.243297, 0.100384,0.253764, 0.115592,0.241462, 0.106046,0.228043, 0.193021,0.140235, 0.223042,0.069158, 0.217457,0.064184, 0.149083,0.101106, 0.073293,0.198875, 0.059119,0.191168, 0.049288,0.208259, 0.061401,0.222183, 0.046445,0.239874,
// shPirateHook (1x1)
NEWSHAPE,  92,1,1, 0.025637,0.290334, 0.015893,0.212083, 0.070681,0.208634, 0.078764,0.213716, 0.092820,0.214478, 0.128742,0.099394, 0.114620,0.096368, 0.104002,0.089501, 0.096284,0.072719, 0.093155,0.050966, 0.099066,0.033609, 0.110446,0.021946, 0.127291,0.014728, 0.145718,0.008586, 0.167517,0.008308, 0.156477,0.015183, 0.140613,0.023565, 0.132241,0.028989, 0.118961,0.034720, 0.108990,0.048739, 0.107378,0.058592, 0.115339,0.077749, 0.131027,0.084323, 0.151797,0.088283, 0.113111,0.234773, 0.104917,0.253698, 0.105424,0.258064, 0.096993,0.264198, 0.089213,0.264464, 0.087585,0.273426, 0.080433,0.279698, 0.072806,0.281526,
// shSabre (1x1)
NEWSHAPE,  93,1,1, 0.052478,0.211644, 0.042242,0.185536, 0.057979,0.181601, 0.068086,0.204991, 0.085295,0.201073, 0.100829,0.193318, 0.113718,0.184489, 0.125735,0.178476, 0.134891,0.170005, 0.144264,0.162907, 0.152391,0.155912, 0.160192,0.146346, 0.166733,0.136894, 0.180659,0.123243, 0.189268,0.108634, 0.197644,0.091550, 0.204992,0.075866, 0.207394,0.063057, 0.213308,0.045015, 0.215741,0.031014, 0.216495,0.013402, 0.217512,-0.002957, 0.216708,-0.025412, 0.215602,-0.037833, 0.209033,-0.050943, 0.249210,-0.008237, 0.247115,0.012233, 0.246190,0.028812, 0.239192,0.052194, 0.237023,0.067663, 0.231339,0.087245, 0.221702,0.107109, 0.215996,0.124316, 0.206323,0.141806, 0.194941,0.164879, 0.173125,0.180454, 0.158905,0.191858, 0.143293,0.202073, 0.128179,0.215193, 0.111272,0.224403, 0.090433,0.232613, 0.079164,0.234740, 0.083027,0.256039, 0.071081,0.264147, 0.060044,0.234679,
// shHedgehogBlade (1x2)
NEWSHAPE,  94,1,2, 0.117178,0.032617, 0.102699,0.066452, 0.056807,0.109987, 0.052506,0.272774, 0.079931,0.279758, 0.082589,0.170109, 0.139258,0.126935, 0.240653,0.136967, 0.177567,0.067821, 0.273978,0.042249, 0.187242,-0.000000,
// shHedgehogBladePlayer (1x2)
NEWSHAPE,  95,1,2, 0.117178,0.032617, 0.102699,0.066452, 0.056807,0.109987, 0.052506,0.272774, 0.079931,0.279758, 0.082589,0.170109, 0.173109,0.220554, 0.139258,0.126935, 0.240653,0.136967, 0.177567,0.067821, 0.273978,0.042249, 0.187242,-0.000000,
// shFemaleBody (1x2)
NEWSHAPE,  96,1,2, -0.091723,0.073620, -0.089581,0.094424, -0.082621,0.117856, -0.070099,0.258731, -0.058825,0.268551, -0.003834,0.273475, 0.040760,0.263668, 0.077891,0.260490, 0.085895,0.233685, 0.064882,0.210867, 0.037427,0.217075, 0.011258,0.226410, -0.037448,0.218444, -0.026537,0.112181, -0.015609,0.091250, 0.007196,0.086357, 0.019189,0.083950, 0.027568,0.077909, 0.033526,0.068249, 0.034689,0.059808, 0.037035,0.046593, 0.029813,0.028620, 0.017865,0.010719, 0.010716,0.003572,
// shFemaleDress (1x2)
NEWSHAPE,  97,1,2, -0.094893,0.000000, -0.098831,0.051826, -0.095833,0.099473, -0.087712,0.125476, -0.020535,0.119586, -0.012011,0.093684, 0.001200,0.088788, 0.021589,0.083958, 0.033549,0.073088, 0.041871,0.056227, 0.039397,0.035815, 0.030996,0.021459, 0.019057,0.011911, 0.010716,-0.001191,
// shDemon (1x2)
NEWSHAPE,  98,1,2, 0.098330,0.005996, 0.098350,0.015592, 0.097179,0.027594, 0.088731,0.041967, 0.082695,0.047939, 0.070682,0.061098, 0.075191,0.124914, 0.083954,0.132624, 0.097611,0.134215, 0.112530,0.132100, 0.126233,0.127459, 0.148804,0.116829, 0.170177,0.098653, 0.175203,0.092537, 0.167872,0.107389, 0.150410,0.125752, 0.132974,0.139130, 0.093435,0.163511, 0.064916,0.165353, 0.042697,0.159810, 0.026717,0.148159, 0.018123,0.130482, 0.016784,0.095911, 0.020324,0.078904, 0.017909,0.070441, 0.013123,0.065613, 0.000000,0.063197, -0.015494,0.057207, -0.032146,0.038099, -0.063226,0.021473, -0.122974,0.008439, -0.197385,0.012337, -0.283101,0.000000,
// shTrylobite (1x2)
NEWSHAPE,  99,1,2, -0.145203,-0.004468, -0.144076,0.007818, -0.163334,0.030206, -0.213613,0.048344, -0.244836,0.053029, -0.230982,0.060844, -0.188498,0.063955, -0.146444,0.055895, -0.100275,0.031197, -0.083512,0.040086, -0.073567,0.085828, -0.071617,0.153304, -0.088919,0.211605, -0.114353,0.253615, -0.085940,0.254429, -0.060908,0.240247, -0.032564,0.204365, -0.014542,0.164439, -0.005571,0.106972, 0.014454,0.048921, 0.090242,0.051248, 0.093652,0.070239, 0.090579,0.133073, 0.075196,0.187429, 0.049574,0.233224, 0.018120,0.279734, 0.039746,0.297527, 0.094039,0.267387, 0.134354,0.217902, 0.162119,0.165496, 0.180861,0.108966, 0.190803,0.067342, 0.197519,0.039279,
// shTrylobiteHead (1x2)
NEWSHAPE, 100,1,2, 0.163308,-0.016778, 0.168997,-0.026860, 0.195236,-0.040394, 0.207912,-0.056192, 0.226552,-0.084534, 0.231600,-0.120884, 0.228729,-0.159657, 0.218914,-0.192826, 0.203437,-0.228441, 0.166197,-0.269785, 0.139999,-0.283413, 0.152680,-0.284851, 0.201083,-0.275346, 0.227316,-0.252447, 0.248761,-0.221374, 0.263269,-0.190328, 0.275789,-0.170944, 0.294151,-0.143655, 0.304263,-0.111677, 0.315665,-0.074073, 0.322577,-0.045594, 0.278615,-0.033977, 0.269353,-0.055455, 0.251793,-0.049681, 0.255151,-0.016935, 0.277445,-0.035105, 0.322577,-0.045594,
// shGoatHead (1x2)
NEWSHAPE, 101,1,2, 0.098234,-0.008161, 0.097458,-0.016878, 0.085163,-0.036297, 0.074171,-0.049357, 0.064423,-0.056062, 0.071545,-0.060919, 0.090299,-0.086141, 0.099873,-0.119956, 0.098555,-0.155296, 0.084548,-0.179003, 0.066861,-0.191576, 0.016947,-0.207877, -0.036121,-0.216389, -0.071628,-0.210733, -0.053640,-0.210686, -0.029915,-0.199910, -0.013982,-0.189736, 0.018469,-0.168500, 0.033113,-0.153873, 0.045027,-0.132252, 0.044982,-0.113169, 0.039048,-0.095429, 0.028215,-0.083777, 0.018818,-0.079369, 0.005692,-0.074475, -0.012112,-0.070532, -0.028520,-0.067591, -0.047548,-0.063966, -0.060278,-0.061572, -0.079475,-0.058008, -0.093451,-0.049712, -0.106876,-0.031455, -0.109804,-0.016063, -0.108918,-0.001180,
// shRatHead (1x2)
NEWSHAPE, 102,1,2, -0.090184,-0.008907, -0.129484,-0.042417, -0.081266,-0.032284, -0.111639,-0.081497, -0.058974,-0.047847, -0.078109,-0.104889, -0.040051,-0.058964, -0.041269,-0.117115, -0.011126,-0.071203, -0.008915,-0.108094, 0.030035,-0.061182, 0.022272,-0.092430, 0.060095,-0.052305, 0.060130,-0.071265, 0.098049,-0.038997, 0.091380,-0.059063, 0.122724,-0.037933, 0.121640,-0.052450, 0.144106,-0.030162, 0.151996,-0.021235, 0.154245,-0.011177, 0.156503,-0.004472, 0.171370,-0.056003, 0.162163,-0.001118,
// shRatEyes (1x2)
NEWSHAPE, 103,1,2, 0.080137,0.021147, 0.090197,0.025612, 0.090206,0.032296, 0.082388,0.035627, 0.075685,0.033390, 0.073440,0.021142, 0.077903,0.018919, 0.079019,0.018920,
// shRatTail (1x1)
NEWSHAPE, 104,1,1, 0.004445,-0.016668, -0.024450,-0.015559, -0.035572,-0.023344, -0.050044,-0.032251, -0.054504,-0.037819, -0.066777,-0.046744, -0.074613,-0.059022, -0.079088,-0.062380, -0.092563,-0.079180, -0.103821,-0.090424, -0.120815,-0.112985, -0.144731,-0.135755, -0.195993,-0.131788, -0.233493,-0.084599, -0.240035,-0.011269, -0.210229,0.059584, -0.186362,0.085322, -0.185130,0.072930, -0.197514,0.038156, -0.204332,-0.020209, -0.188533,-0.069578, -0.161255,-0.075029, -0.129508,-0.050240, -0.108106,-0.023404, -0.076781,0.007789, -0.031123,0.024454, -0.003334,0.018891, 0.008889,0.012223, 0.014445,0.002222,
// shRatCape1 (1x2)
NEWSHAPE, 105,1,2, 0.111734,-0.044549, 0.085077,-0.064302, 0.076126,-0.069355, 0.063009,-0.072447, 0.060184,-0.075301, -0.016803,-0.045427, -0.084897,-0.004966,
// shRatCape2 (1x2)
NEWSHAPE, 106,1,2, 0.064357,-0.017052, 0.062169,-0.036861, 0.052288,-0.064948, 0.041296,-0.082593, 0.029743,-0.094738, 0.018182,-0.103582, 0.004409,-0.109669, -0.009371,-0.114663, -0.023159,-0.116896, -0.035856,-0.120805, -0.044697,-0.123606, -0.052994,-0.126412, -0.066282,-0.129803, -0.068502,-0.130927, -0.077943,-0.135432, -0.089059,-0.138844, -0.103549,-0.143972, -0.118107,-0.151931, -0.142204,-0.155536, -0.155133,-0.156245, -0.189079,-0.160076, -0.172777,-0.122777, -0.163406,-0.078103, -0.158135,-0.020458,
// shKnightArmor (1x2)
NEWSHAPE, 107,1,2, -0.129512,-0.004842, -0.129934,0.057074, -0.089756,0.104311, -0.099124,0.177184, -0.086054,0.237913, -0.019952,0.218220, -0.020812,0.167721, -0.024311,0.143434, 0.006041,0.122026, 0.026372,0.079117, 0.032239,0.044179, 0.034575,0.016691, 0.034565,0.003576,
// shWightCloak (1x2)
NEWSHAPE, 108,1,2, 0.033830,0.001771, 0.026266,-0.078450, -0.008137,-0.128166, -0.032077,-0.140784, -0.061416,-0.142096, -0.077813,-0.152253, -0.102686,-0.173285, -0.116436,-0.179982, -0.137144,-0.175455, -0.139446,-0.169272, -0.128291,-0.167777, -0.112415,-0.158387, -0.104516,-0.150085, -0.097005,-0.138640, -0.104605,-0.128377, -0.121858,-0.124995, -0.135777,-0.130025, -0.150982,-0.138470, -0.176251,-0.152351, -0.204651,-0.170039, -0.236443,-0.182657, -0.260761,-0.182008, -0.289038,-0.169599, -0.291713,-0.160123, -0.281530,-0.160333, -0.264828,-0.166999, -0.240959,-0.155502, -0.221286,-0.138182, -0.212055,-0.124802, -0.216004,-0.111599, -0.222140,-0.093865, -0.234242,-0.103510, -0.254816,-0.119994, -0.273521,-0.124945, -0.297868,-0.118456, -0.319582,-0.099338, -0.332300,-0.085506, -0.342656,-0.077867, -0.331725,-0.075650, -0.311338,-0.089299, -0.296662,-0.091450, -0.273246,-0.082737, -0.257713,-0.072665, -0.235809,-0.054595, -0.224729,-0.037162, -0.184572,-0.005059,
// shKnightCloak (1x2)
NEWSHAPE, 109,1,2, -0.000015,0.003765, 0.007931,-0.006370, 0.020079,-0.026681, 0.029245,-0.044814, 0.027073,-0.059250, 0.023423,-0.078357, 0.002361,-0.116363, -0.019415,-0.136638, -0.058154,-0.153287, -0.094935,-0.166174, -0.136465,-0.178800, -0.161844,-0.181507, -0.184577,-0.179578, -0.162984,-0.162667, -0.150295,-0.135739, -0.152621,-0.120097, -0.161098,-0.102831, -0.167882,-0.095516, -0.184530,-0.081393, -0.163316,-0.053335, -0.149316,-0.029081, -0.143262,-0.011700, -0.140925,0.006746,
// shPrincessDress (1x1)
NEWSHAPE, 110,1,1, 0.041877,-0.057431, 0.046812,-0.078020, 0.028859,-0.096196, -0.001205,-0.110866, -0.053233,-0.116144, -0.110022,-0.122246, -0.181271,-0.133766, -0.221411,-0.145062, -0.252155,-0.156465, -0.285822,-0.165961, -0.259590,-0.116944, -0.258096,-0.089439, -0.246236,-0.031569, -0.247436,0.011362, -0.253890,0.054586, -0.268619,0.095109, -0.277680,0.121972, -0.286489,0.138666, -0.295278,0.151594, -0.248719,0.149489, -0.200432,0.139924, -0.139171,0.124392, -0.102260,0.110782, -0.077429,0.101626,
// shWizardCape1 (1x2)
NEWSHAPE, 111,1,2, -0.091286,0.091286, -0.084762,0.158069, -0.029729,0.155508, -0.011369,0.043203,
// shWizardCape2 (1x2)
NEWSHAPE, 112,1,2, -0.006827,0.072824, -0.050157,0.100313, -0.075330,0.109571, -0.119046,0.123624, -0.167718,0.126363, -0.212240,0.126882, -0.252517,0.122784,
// shPrinceDress (1x2)
NEWSHAPE, 113,1,2, -0.123399,0.032664, -0.122325,0.049657, -0.121280,0.064278, -0.114003,0.076406, -0.098124,0.089645, -0.084789,0.101747, -0.081167,0.105396, -0.080113,0.115314, -0.099259,0.181148, -0.053980,0.166846, -0.018484,0.187307, -0.015849,0.154831, -0.013334,0.134548, 0.006043,0.123271, 0.021636,0.096161, 0.031121,0.067030, 0.038206,0.038206, 0.038163,0.017889,
// shArmor (1x2)
NEWSHAPE, 114,1,2, -0.131705,0.010875, -0.133453,0.061874, -0.093134,0.099182, -0.097643,0.135480, -0.010956,0.158255, 0.016844,0.113094, 0.022740,0.084974, -0.062459,0.086481, -0.061152,0.076740, 0.063569,0.076763, 0.074360,0.065964, 0.083920,0.049153, 0.088667,0.032352, 0.092226,0.002395, 0.057196,0.007150, 0.054891,0.041765, 0.039313,0.039313,
// shTurban1 (1x2)
NEWSHAPE, 115,1,2, -0.072384,-0.002801, -0.075078,0.020302, -0.067673,0.048512, -0.046761,0.065782, -0.011565,0.081749, 0.024684,0.095470, 0.076082,0.087352, 0.102859,0.061877, 0.113555,0.034171,
// shTurban2 (1x2)
NEWSHAPE, 116,1,2, -0.041779,0.001538, -0.029927,0.025572, -0.001577,0.046885, 0.030017,0.051171, 0.051757,0.039557, 0.066202,0.014187,
// shWizardHat1 (1x2)
NEWSHAPE, 117,1,2, -0.040934,0.027289, -0.043234,0.054612, -0.034198,0.107155, -0.011421,0.141617, 0.029596,0.077405, 0.031831,0.025010, 0.036377,0.011368,
// shWizardHat2 (1x2)
NEWSHAPE, 118,1,2, 0.018184,0.011365, 0.009094,0.038652, -0.006823,0.052309, -0.063734,0.045524, -0.102563,0.029629, -0.134711,0.020549, -0.169382,0.006867,
// shWestHat1 (1x2)
NEWSHAPE, 119,1,2, -0.070542,-0.002276, -0.070545,0.013654, -0.066000,0.034138, -0.045518,0.059173, -0.034143,0.070563, -0.006832,0.091098, 0.029616,0.093405, 0.047849,0.088862, 0.077446,0.054668, 0.086570,0.045563, 0.100276,0.031906, 0.111711,0.004560,
// shWestHat2 (1x2)
NEWSHAPE, 120,1,2, -0.018184,0.011365, -0.018185,0.020458, -0.002273,0.031826, 0.022743,0.047761, 0.054614,0.045512, 0.065995,0.029584, 0.081957,0.009106,
// shGunInHand (1x1)
NEWSHAPE, 121,1,1, 0.048306,0.216227, 0.048215,0.197453, 0.165920,0.170529, 0.173217,0.189384, 0.103628,0.207255, 0.108418,0.221449, 0.089975,0.230705, 0.073629,0.211684,
// shVikingHelmet (1x2)
NEWSHAPE, 122,1,2, -0.068191,-0.016749, -0.064642,-0.037110, -0.055059,-0.049075, -0.040676,-0.057425, -0.027503,-0.060986, -0.030377,-0.140951, -0.026834,-0.154904, -0.003670,-0.166357, 0.018370,-0.168999, 0.055380,-0.175984, 0.082350,-0.161012, 0.068527,-0.152961, 0.031640,-0.146029, 0.008493,-0.138308, -0.002413,-0.117054, -0.004785,-0.069381, 0.017935,-0.063369, 0.049033,-0.046641, 0.032200,-0.027430,
// shHood (1x2)
NEWSHAPE, 123,1,2, -0.289108,0.001285, -0.253872,0.006315, -0.239738,0.013807, -0.225917,0.023715, -0.220610,0.037392, -0.239040,0.055357, -0.278480,0.068016, -0.290572,0.069737, -0.249897,0.082453, -0.220651,0.087759, -0.204652,0.097335, -0.243692,0.127587, -0.294677,0.136206, -0.303482,0.130064, -0.255520,0.152279, -0.227045,0.154340, -0.176957,0.161897, -0.131265,0.160986, -0.098249,0.157199, -0.054857,0.153598, -0.031595,0.149471, -0.001208,0.130439, 0.016853,0.115564, 0.028853,0.106995, 0.043228,0.096063, 0.053964,0.082746, 0.056228,0.064603, 0.063348,0.049005, 0.054927,0.048957, 0.014325,0.070433, -0.046699,0.077832, -0.088908,0.061274, -0.081520,0.052748, -0.044233,0.068142, 0.011920,0.059602, 0.086263,0.037141,
// shPirateX (4x2)
NEWSHAPE, 124,4,2, -0.217493,-0.115996, -0.119624,-0.071774, -0.076312,-0.023848,
// shPirateHood (1x2)
NEWSHAPE, 125,1,2, 0.028515,-0.002137, 0.030041,0.018069, 0.041515,0.042413, 0.018125,0.065806, -0.016208,0.072033, -0.045904,0.078062, -0.082076,0.083600, -0.135647,0.084840, -0.187346,0.092498, -0.230428,0.116494, -0.151031,0.111393, -0.111628,0.086283, -0.087869,0.069439, -0.097756,0.014558,
// shEyepatch (1x1)
NEWSHAPE, 126,1,1, 0.029860,-0.048970, 0.063361,0.014346, 0.059794,0.031093, 0.017892,-0.040556,
// shPHead (1x2)
NEWSHAPE, 127,1,2, 0.060794,0.001192, 0.058426,0.023847, 0.050054,0.030986, 0.042896,0.038130, 0.044109,0.042917, 0.032180,0.050058, 0.017884,0.059612, 0.005963,0.064401, -0.009546,0.068015, -0.022689,0.070455, -0.044247,0.070556, -0.053847,0.068206, -0.047819,0.065752, -0.040573,0.056087, -0.040563,0.053686, -0.053753,0.053753, -0.067016,0.056246, -0.090053,0.054032, -0.097365,0.051688, -0.072989,0.046665, -0.065710,0.040621, -0.063272,0.034621, -0.064461,0.031037, -0.074098,0.028683, -0.094756,0.031185, -0.113149,0.027685, -0.114376,0.026487, -0.088611,0.020357, -0.087387,0.017956, -0.103257,0.018010, -0.127970,0.014487, -0.164656,0.006098, -0.174996,0.001224,
// shFemaleHair (1x2)
NEWSHAPE, 128,1,2, -0.185924,0.002463, -0.273576,0.019176, -0.186050,0.025874, -0.268509,0.057538, -0.183840,0.054288, -0.258480,0.097250, -0.174976,0.069005, -0.246145,0.133329, 0.029891,0.058585, 0.045415,0.043025, 0.051358,0.025082,
// shWitchHair (1x2)
NEWSHAPE, 129,1,2, 0.042269,0.051167, 0.042269,0.051167, 0.034481,0.055615, 0.030032,0.057839, 0.018910,0.063403, 0.007787,0.066742, 0.003337,0.068971, 0.001112,0.068971, -0.012240,0.074551, -0.021146,0.079021, -0.037870,0.091333, -0.055771,0.111541, -0.067005,0.126193, -0.069311,0.140857, -0.077214,0.151072, -0.082864,0.156769, -0.088523,0.162478, -0.102090,0.169403, -0.110020,0.172889, -0.119120,0.178681, -0.129367,0.183363, -0.140780,0.188082, -0.152182,0.189382, -0.126986,0.173060, -0.121256,0.166166, -0.116681,0.160437, -0.116640,0.155895, -0.116591,0.150223, -0.121038,0.142332, -0.128936,0.141269, -0.136844,0.140209, -0.150448,0.140343, -0.156128,0.140403, -0.165232,0.140504, -0.168638,0.139407, -0.173187,0.138325, -0.178884,0.137257, -0.182300,0.136163, -0.184589,0.136190, -0.171969,0.131506, -0.159432,0.130240, -0.143544,0.130087, -0.127691,0.127691, -0.106186,0.112892, -0.102795,0.109500, -0.096004,0.098237, -0.092611,0.091496, -0.086977,0.082517, -0.085842,0.078039, -0.084699,0.070211, -0.084690,0.066860, -0.088036,0.063519, -0.092499,0.059066, -0.096971,0.056845, -0.107054,0.055757, -0.120527,0.055799, -0.137414,0.055859, -0.158884,0.054826, -0.171350,0.051517, -0.163386,0.047001, -0.144148,0.045815, -0.132862,0.042427, -0.121600,0.037930, -0.112608,0.033448, -0.103624,0.023399, -0.098017,0.014480, -0.093538,0.007795,
// shBeautyHair (1x2)
NEWSHAPE, 130,1,2, 0.058945,0.017795, 0.051157,0.031139, 0.040039,0.047824, 0.025588,0.066752, 0.003338,0.077896, -0.020043,0.090192, -0.047936,0.104790, -0.074808,0.118352, -0.100655,0.127497, -0.120982,0.135545, -0.144880,0.150496, -0.177054,0.171415, -0.211757,0.182315, -0.246928,0.194584, -0.271704,0.197499, -0.287220,0.201397, -0.306370,0.203099, -0.328100,0.204919, -0.355070,0.210497, -0.372314,0.211094, -0.378552,0.212500, -0.378552,0.212500, -0.320256,0.184807, -0.386076,0.179705, -0.320758,0.158088, -0.391464,0.152879, -0.322452,0.125779, -0.394332,0.119109, -0.330437,0.101761, -0.401200,0.089027, -0.345683,0.069823, -0.404521,0.060100, -0.352724,0.045808, -0.410490,0.028908, -0.357430,0.017184, -0.411658,0.001156,
// shFlowerHair (1x2)
NEWSHAPE, 131,1,2, 0.020097,0.000230, 0.021808,-0.003455, 0.020624,-0.006537, 0.019055,-0.009617, 0.015358,-0.012872, 0.012065,-0.013813, 0.008195,-0.014555, 0.003932,-0.016068, -0.000938,-0.021248, -0.002329,-0.026069, 0.002252,-0.033451, 0.008397,-0.038528, 0.015533,-0.040713, 0.022307,-0.039801, 0.025990,-0.038478, 0.033367,-0.034670, 0.040526,-0.034537, 0.044375,-0.037666, 0.044527,-0.043667, 0.042949,-0.047912, 0.039234,-0.053495, 0.029716,-0.058642, 0.021226,-0.055470, 0.017392,-0.050791, 0.019179,-0.044419, 0.021725,-0.039990, 0.023868,-0.037880, 0.027754,-0.035204, 0.031261,-0.031751, 0.033036,-0.026932, 0.029413,-0.020135, 0.024802,-0.016233, 0.015732,-0.014421, 0.010520,-0.013800, 0.005700,-0.012410, -0.002576,-0.008288, -0.006404,-0.003621, -0.005985,0.000626,
// shSuspenders (1x2)
NEWSHAPE, 132,1,2, 0.036850,0.051150, 0.021480,0.095281, -0.084982,0.100985, -0.108740,0.081693, 0.027517,0.075948, 0.036300,0.051700,
// shFlowerHand (1x1)
NEWSHAPE, 133,1,1, 0.061138,-0.276481, 0.048889,-0.207664, 0.042675,-0.208468, 0.039268,-0.207720, 0.038317,-0.207150, 0.033960,-0.205835, 0.033484,-0.205550, 0.030231,-0.202307, 0.026503,-0.198784, 0.023404,-0.193050, 0.022285,-0.187206, 0.021521,-0.179639, 0.020958,-0.172845, 0.023051,-0.166988, 0.024435,-0.164584, 0.024074,-0.158566, 0.023202,-0.156760, 0.022528,-0.155715, 0.021101,-0.154869, 0.019476,-0.153262, 0.034413,-0.155043, 0.037623,-0.155021, 0.041825,-0.151082, 0.047419,-0.144756, 0.046987,-0.139992, 0.047408,-0.139933, 0.048335,-0.144628, 0.055494,-0.149173, 0.060644,-0.151807, 0.063735,-0.150949, 0.077598,-0.145147, 0.076489,-0.147136, 0.075355,-0.148340, 0.075002,-0.149527, 0.074673,-0.151502, 0.076027,-0.157381, 0.078039,-0.159310, 0.081710,-0.164363, 0.083094,-0.171043, 0.084506,-0.178519, 0.085090,-0.184437, 0.083740,-0.190792, 0.081158,-0.195197, 0.078954,-0.199202, 0.078578,-0.199606, 0.074763,-0.202061, 0.074012,-0.202870, 0.070951,-0.204520, 0.064751,-0.205449, 0.072719,-0.274864,
// shPFace (1x2)
NEWSHAPE, 134,1,2, 0.011878,-0.000000, 0.011880,0.010692, 0.014262,0.022581, 0.015455,0.027343, 0.017842,0.034494, 0.021425,0.041659, 0.025005,0.044056, 0.029785,0.047656, 0.032176,0.048860, 0.036964,0.051273, 0.044133,0.048905, 0.046519,0.046519, 0.051292,0.041749, 0.056061,0.034591, 0.060844,0.028632, 0.064425,0.020282, 0.064417,0.016701, 0.069220,0.009548, 0.075246,0.003583, 0.077662,0.001195, 0.080082,-0.000000,
// shEyes (1x2)
NEWSHAPE, 135,1,2, 0.070467,0.026276, 0.054837,0.027418, 0.040494,0.034539, 0.034561,0.047670, 0.032259,0.070492, 0.044434,0.096073, 0.065124,0.106127, 0.089628,0.109007, 0.110425,0.098291, 0.121434,0.088647, 0.124892,0.073965, 0.123474,0.064158, 0.117069,0.050690, 0.113313,0.047013, 0.098463,0.037224, 0.081366,0.029914, 0.070467,0.026276,
// shShark (1x2)
NEWSHAPE, 136,1,2, -0.254548,0.002533, -0.295445,0.115041, -0.214304,0.034887, -0.113418,0.026545, -0.034699,0.062219, -0.010782,0.079071, 0.009598,0.087578, 0.037297,0.096250, -0.001207,0.119535, -0.032645,0.120908, -0.002431,0.144615, 0.027983,0.145997, 0.058411,0.137510, 0.096163,0.115639, 0.130542,0.089062, 0.175135,0.075234, 0.227410,0.066590, 0.215688,0.036156, 0.187577,0.041958, 0.184662,0.017235, 0.208627,0.008693, 0.214217,0.028645, 0.229902,0.056533, 0.249359,0.045568, 0.259206,0.035577, 0.276638,0.026895, 0.302288,0.020758, 0.330687,0.013175, 0.348759,-0.003993,
// shBugBody (1x2)
NEWSHAPE, 137,1,2, -0.036973,0.000272, -0.037088,0.049282, -0.103861,0.110700, -0.187497,0.070862, -0.199435,0.092353, -0.102619,0.135884, -0.010856,0.052276, 0.027094,0.127840, -0.010117,0.161004, -0.122621,0.155285, -0.129838,0.187041, -0.003559,0.192507, 0.048774,0.146275, 0.045625,0.106117, 0.010970,0.038441, 0.089547,0.116575, 0.094553,0.161297, 0.079585,0.174794, 0.010872,0.213071, 0.019129,0.245278, 0.122810,0.186751, 0.134973,0.146945, 0.133411,0.118806, 0.060081,0.038600, 0.122451,0.041792, 0.137147,0.087747, 0.150029,0.114701, 0.172516,0.135274, 0.194165,0.144658, 0.206227,0.137750, 0.191047,0.128830, 0.173558,0.119946, 0.156400,0.103721, 0.146061,0.085696, 0.131320,0.034801, 0.128188,0.004402,
// shBugArmor (1x2)
NEWSHAPE, 138,1,2, -0.257478,-0.001337, -0.204917,0.009783, -0.203815,0.017248, -0.113586,0.008028, -0.187093,0.033579, -0.182177,0.042285, -0.170953,0.054731, -0.148063,0.061036, -0.114175,0.062548, -0.089190,0.053147, -0.055805,0.032092, -0.046611,0.046680, -0.034899,0.056544, -0.026686,0.062743, -0.011200,0.065507, 0.015073,0.067380, 0.038084,0.055978, 0.058951,0.034968, 0.080396,0.053686, 0.091257,0.062541, 0.107216,0.066788, 0.136013,0.061816, 0.150105,0.048890, 0.154042,0.040462, 0.173238,0.060925, 0.190355,0.071679, 0.214645,0.070389, 0.221506,0.066951, 0.203766,0.058557, 0.189101,0.049201, 0.181298,0.037761, 0.177552,0.025312, 0.177715,0.016710, 0.176915,-0.004189,
// shCatBody (1x2)
NEWSHAPE, 139,1,2, -0.308099,0.007958, -0.308099,0.007958, -0.300981,0.013629, -0.295063,0.015888, -0.298642,0.024981, -0.279731,0.016988, -0.280962,0.032854, -0.262156,0.019210, -0.263366,0.031649, -0.244692,0.020297, -0.245893,0.032711, -0.229645,0.022514, -0.228517,0.031520, -0.206622,0.019090, -0.211256,0.034835, -0.195179,0.021313, -0.200928,0.032553, -0.182636,0.024650, -0.187230,0.036998, -0.166723,0.024617, -0.172450,0.042553, -0.157684,0.035786, -0.164538,0.051488, -0.148670,0.046949, -0.155517,0.061536, -0.140794,0.054753, -0.141981,0.068196, -0.125033,0.058051, -0.125087,0.071478, -0.112675,0.059126, -0.113871,0.078147, -0.099238,0.065787, -0.099290,0.080325, -0.089161,0.065756, -0.086969,0.080279, -0.080217,0.066848, -0.073560,0.083591, -0.063470,0.067924, -0.055703,0.086897, -0.048978,0.070128, -0.045663,0.085757, -0.036721,0.067879, -0.028944,0.082379, -0.024473,0.063406, -0.016698,0.085718, -0.012236,0.066743, -0.001113,0.080128, 0.005561,0.062283, 0.014465,0.073436, 0.017795,0.058945, 0.026707,0.073445, 0.030032,0.057839, 0.042293,0.070117, 0.044501,0.055626, 0.053427,0.064557, 0.054515,0.046727, 0.072375,0.056786, 0.070112,0.038951, 0.082394,0.040084, 0.070104,0.032270, 0.077909,0.026712, 0.075671,0.020031, 0.089069,0.014474, 0.075666,0.011127, 0.085713,0.005566, 0.077896,-0.001113,
// shCatLegs (1x2)
NEWSHAPE, 140,1,2, -0.109115,0.005511, -0.150780,0.114748, -0.148046,0.122540, -0.141908,0.125278, -0.131832,0.124077, -0.123409,0.117322, -0.088085,0.011561, 0.010990,0.019783, 0.040802,0.111378, 0.045231,0.116940, 0.050761,0.119177, 0.057933,0.115867, 0.060111,0.105884, 0.025279,0.006595,
// shFamiliarHead (1x2)
NEWSHAPE, 141,1,2, 0.257493,0.002498, 0.199698,0.024419, 0.214773,0.038961, 0.175084,0.036275, 0.196857,0.052339, 0.156515,0.045663, 0.174175,0.068554, 0.133862,0.047313, 0.150572,0.078274, 0.111906,0.056441, 0.131266,0.089711, 0.081753,0.056166, 0.087456,0.089841, 0.065143,0.061409, -0.058696,0.149969, 0.038088,0.015520, 0.004420,0.018600, 0.032174,0.003509,
// shFamiliarEye (1x1)
NEWSHAPE, 142,1,1, 0.155427,0.038018, 0.200883,0.012345, 0.144081,0.014520,
// shCatHead (1x2)
NEWSHAPE, 143,1,2, 0.073175,-0.000000, 0.058843,0.007699, 0.074280,0.010454, 0.062705,0.019251, 0.077045,0.019811, 0.068229,0.032464, 0.084236,0.031933, 0.077073,0.043491, 0.091431,0.037454, 0.083718,0.051773, 0.098645,0.047394, 0.091478,0.058965, 0.106417,0.052933, 0.101443,0.057888, 0.102568,0.063416, 0.104266,0.073373, 0.106515,0.080576, 0.110984,0.086689, 0.121624,0.100063, 0.122097,0.085081, 0.122586,0.070680, 0.128687,0.065724, 0.135350,0.060217, 0.142646,0.069664, 0.138114,0.054141, 0.151534,0.058070, 0.145904,0.047529, 0.161578,0.049801, 0.155386,0.040920, 0.167717,0.041514, 0.160393,0.029313, 0.171614,0.030448, 0.164858,0.020469, 0.171594,0.021588, 0.176845,0.069851, 0.174963,0.020486, 0.183057,0.072113, 0.178894,0.017169, 0.191443,0.058820, 0.181703,0.013295, 0.183949,0.005541,
// shWolfBody (1x2)
NEWSHAPE, 144,1,2, -0.311492,0.001303, -0.165080,0.009783, -0.195342,0.027200, -0.153616,0.025603, -0.173302,0.051622, -0.128440,0.037563, -0.152961,0.068527, -0.107392,0.045853, -0.121764,0.090105, -0.079263,0.050440, -0.084555,0.086971, -0.043029,0.046615, -0.049440,0.102497, -0.020293,0.047747, -0.012028,0.101033, 0.009546,0.047732, 0.027630,0.091298, 0.043025,0.045415, 0.071007,0.077024, 0.071856,0.028742, 0.113578,0.045915, 0.090012,0.007201,
// shWolfHead (1x2)
NEWSHAPE, 145,1,2, 0.183292,0.002460, 0.178071,0.011053, 0.185996,0.019708, 0.170281,0.015926, 0.170341,0.024510, 0.158693,0.019531, 0.158807,0.032983, 0.147214,0.020683, 0.142274,0.036481, 0.135869,0.025475, 0.129693,0.037575, 0.118394,0.028994, 0.114793,0.043501, 0.102362,0.033719, 0.100073,0.053051, 0.087692,0.037239, 0.080602,0.063760, 0.065828,0.032315, 0.008362,0.058537, 0.046539,0.014320, 0.028597,0.008341, 0.051325,-0.000000,
// shWolfLegs (1x2)
NEWSHAPE, 146,1,2, -0.139559,-0.001214, -0.139599,0.016995, -0.228445,0.053973, -0.209280,0.056057, -0.216331,0.065024, -0.204121,0.065966, -0.204581,0.081084, -0.194817,0.069489, -0.113487,0.036219, -0.008338,0.022632, 0.076062,0.091757, 0.081210,0.107875, 0.084610,0.090653, 0.099450,0.094598, 0.089336,0.078471, 0.105258,0.072592, 0.083028,0.062572, 0.025016,0.001191,
// shWolfEyes (1x2)
NEWSHAPE, 147,1,2, 0.172835,0.002452, 0.156071,0.002439, 0.154815,0.012190, 0.138338,0.016989, 0.135790,0.008487, 0.147155,0.006081, 0.153518,0.004874, 0.156070,-0.000000,
// shWitchDress (1x2)
NEWSHAPE, 148,1,2, 0.039326,-0.001751, 0.041168,0.053278, 0.035516,0.073044, 0.024685,0.088439, 0.009671,0.096889, -0.002313,0.097795, -0.008219,0.099482, -0.016102,0.107500, -0.016016,0.108729, -0.022078,0.163499, -0.052858,0.141939, -0.084069,0.161675, -0.098088,0.129874, -0.108494,0.122098, -0.117272,0.122997, -0.128424,0.129237, -0.140905,0.138269, -0.152175,0.151195, -0.161208,0.157511, -0.178346,0.167608, -0.195945,0.176675, -0.219689,0.185527, -0.234056,0.190460, -0.219683,0.178715, -0.211330,0.162725, -0.206261,0.134376, -0.201753,0.102919, -0.205795,0.072925, -0.213892,0.049798, -0.218429,0.022623, -0.219265,0.011429,
// shPickAxe (1x1)
NEWSHAPE, 149,1,1, 0.064364,0.237609, 0.183353,-0.046466, 0.171819,-0.050455, 0.142623,-0.056105, 0.122591,-0.057865, 0.106467,-0.058329, 0.118180,-0.067773, 0.135948,-0.071025, 0.161536,-0.071733, 0.188994,-0.071278, 0.212623,-0.063402, 0.239189,-0.047822, 0.248564,-0.040034, 0.265990,-0.019250, 0.272276,0.000110, 0.269338,0.000001, 0.247284,-0.019782, 0.224346,-0.034255, 0.208443,-0.042147, 0.086124,0.253692,
// shPike (1x1)
NEWSHAPE, 150,1,1, 0.096293,0.278465, 0.083339,-0.050087, 0.079276,-0.383984, 0.095192,-0.378226, 0.073482,-0.534658, 0.070202,-0.534586, 0.045864,-0.377144, 0.061795,-0.383600, 0.062148,-0.049623, 0.069133,0.279061,
// shFlailBall (1x1)
NEWSHAPE, 151,1,1, -0.152664,-0.216202, -0.169658,-0.186257, -0.175291,-0.180580, -0.179213,-0.204880, -0.180636,-0.179891, -0.208327,-0.205555, -0.195199,-0.172755, -0.194159,-0.164682, -0.215525,-0.177426, -0.196845,-0.160074, -0.233600,-0.155277, -0.200041,-0.144144, -0.193092,-0.139738, -0.215936,-0.131441, -0.191101,-0.134685, -0.209451,-0.103230, -0.180537,-0.121970, -0.172894,-0.124532, -0.180135,-0.101552, -0.167729,-0.122842, -0.154066,-0.088606, -0.151375,-0.122930, -0.148774,-0.130514, -0.135083,-0.110267, -0.144329,-0.133463, -0.109150,-0.122417, -0.134514,-0.146301, -0.138895,-0.153179, -0.114703,-0.151023, -0.138521,-0.158550, -0.108526,-0.179203, -0.142650,-0.174484, -0.150697,-0.175461, -0.134343,-0.193130, -0.154679,-0.179212,
// shFlailTrunk (1x1)
NEWSHAPE, 152,1,1, 0.051293,0.275700, 0.057876,0.280376, 0.067059,0.283710, 0.073576,0.283978, 0.077320,0.279640, 0.147958,-0.151688, 0.141932,-0.161853, 0.138281,-0.166934, 0.129252,-0.166536, 0.118813,-0.160892, 0.112195,-0.152881,
// shFlailChain (1x1)
NEWSHAPE, 153,1,1, 0.124731,-0.149526, 0.117490,-0.160203, 0.106751,-0.149132, 0.095911,-0.162180, 0.085170,-0.151131, 0.074321,-0.161824, 0.062353,-0.149567, 0.052646,-0.162759, 0.041822,-0.150522, 0.032058,-0.161359, 0.023596,-0.147919, 0.013752,-0.158813, 0.005196,-0.149008, -0.004760,-0.161206, -0.014646,-0.148927, -0.027247,-0.159995, -0.039846,-0.148910, -0.052728,-0.162603, -0.066903,-0.150243, -0.077482,-0.162849, -0.093362,-0.150445, -0.104284,-0.165774, -0.117892,-0.153274, -0.131957,-0.166335, -0.144633,-0.153714, -0.156334,-0.168243, -0.173918,-0.153010, -0.173894,-0.147770, -0.156165,-0.131894, -0.144597,-0.145973, -0.131802,-0.132992, -0.117856,-0.145631, -0.104131,-0.132849, -0.093350,-0.147924, -0.077354,-0.135296, -0.066892,-0.147748, -0.052601,-0.135298, -0.039846,-0.148910, -0.027144,-0.137836, -0.014646,-0.148927, -0.004647,-0.136736, 0.005196,-0.149008, 0.013843,-0.139323, 0.023585,-0.150350, 0.032171,-0.137084, 0.041833,-0.148099, 0.052770,-0.136144, 0.062353,-0.149567, 0.074434,-0.137691, 0.085181,-0.148721, 0.096024,-0.138086, 0.106739,-0.151540, 0.117580,-0.140943, 0.124720,-0.151933,
// shBoatOuter (1x2)
NEWSHAPE, 154,1,2, -0.254359,0.147192, -0.147845,0.149087, 0.030409,0.144749, 0.120225,0.127586, 0.172686,0.119265, 0.208240,0.105375, 0.245303,0.092783, 0.286782,0.068466, 0.309035,0.045638, 0.335671,0.021145, 0.352147,0.009337, 0.357212,0.006689,
// shBoatInner (1x2)
NEWSHAPE, 155,1,2, -0.229820,0.121893, 0.022940,0.117115, 0.169024,0.090064, 0.256702,0.052103, 0.299070,-0.000000,
// shScratch (1x2)
NEWSHAPE, 156,1,2, -0.028983,0.241948,
// shSkeletonBody (1x2)
NEWSHAPE, 157,1,2, 0.001190,0.001190, 0.001193,-0.042936, -0.004786,-0.070589, -0.016803,-0.088814, -0.036099,-0.097467, -0.038444,-0.087700, -0.028752,-0.074277, -0.022672,-0.042958, -0.022648,-0.027416, -0.044152,-0.020286, -0.046994,-0.100012, -0.031489,-0.240575, 0.044046,-0.236589, 0.052529,-0.220120, 0.068888,-0.219190, 0.079398,-0.230631, 0.083186,-0.263635, 0.062603,-0.265745, 0.048239,-0.255158, -0.048471,-0.265315, -0.068926,-0.105203, -0.070637,-0.026339, -0.085171,-0.020393,
// shSkull (1x2)
NEWSHAPE, 158,1,2, -0.120823,0.019332, -0.089124,0.061424, -0.035904,0.063430, 0.019074,0.032188, 0.022631,0.004764,
// shSkullEyes (1x2)
NEWSHAPE, 159,1,2, -0.001191,0.010716, -0.016673,0.009527, -0.016682,0.025023, -0.002382,0.022631, 0.001191,0.023823,
// shFishTail (1x2)
NEWSHAPE, 160,1,2, -0.003571,0.001190, -0.007145,0.015481, -0.015500,0.035769, -0.031072,0.053778, -0.047927,0.064702, -0.062478,0.073291, -0.083262,0.082055, -0.098001,0.082272, -0.111474,0.073912, -0.123761,0.063094, -0.129971,0.059519, -0.138754,0.057206, -0.148852,0.053684, -0.164257,0.053935, -0.177269,0.054165, -0.191786,0.054443, -0.214728,0.056179, -0.227189,0.058994, -0.237162,0.065598, -0.240110,0.069505, -0.248932,0.077474, -0.262350,0.087023, -0.271446,0.092626, -0.278978,0.094284, -0.283499,0.094500, -0.277139,0.087653, -0.269286,0.078221, -0.253927,0.055864, -0.247700,0.034122, -0.243240,0.018905, -0.241745,0.005036,
// shFatBody (1x2)
NEWSHAPE, 161,1,2, -0.158280,0.010638, -0.153491,0.063014, -0.143004,0.090522, -0.130133,0.119287, -0.120336,0.139833, -0.108707,0.144976, -0.103459,0.149545, -0.088463,0.147968, -0.090694,0.183338, -0.069574,0.274320, -0.026928,0.273931, 0.000335,0.278791, 0.049634,0.254674, 0.064323,0.261140, 0.085998,0.258714, 0.084852,0.235757, 0.075972,0.211105, 0.051782,0.222035, -0.009808,0.219631, -0.037620,0.215354, -0.020675,0.185577, -0.021056,0.141669, 0.007264,0.129328, 0.036658,0.102338, 0.051136,0.057412, 0.056061,-0.003004,
// shZebra[0] (6x1)
NEWSHAPE, 162,6,1, -0.307697,-0.214329,
// shZebra[1] (1x1)
NEWSHAPE, 163,1,1, -0.192992,-0.168975, -0.238865,0.204661, -0.080551,0.254199, 0.347359,0.222111, 0.392920,-0.180638, 0.019691,-0.156962,
// shZebra[2] (1x1)
NEWSHAPE, 164,1,1, -0.255158,0.223637, -0.114892,0.277358, 0.260873,0.219611, 0.444210,0.107854, 0.255129,-0.352141, -0.058563,-0.195290, -0.205857,-0.164479,
// shZebra[3] (1x1)
NEWSHAPE, 165,1,1, 0.248469,0.308508, 0.257329,0.012990, 0.288207,-0.281417, 0.038093,-0.236115, -0.209655,-0.205481, -0.224279,0.047025, -0.253191,0.302526, -0.002290,0.296336,
// shZebra[4] (1x1)
NEWSHAPE, 166,1,1, 0.623738,-0.009789, 0.462499,-0.298584, 0.351221,-0.619633, -0.099397,-0.442426, -0.571648,-0.361506, -0.537598,0.000000, -0.571648,0.361506, -0.099397,0.442426, 0.351221,0.619633, 0.462499,0.298584, 0.623738,0.009789,
// shEmeraldFloor[0] (1x1)
NEWSHAPE, 167,1,1, -0.056700,0.289325, 0.003114,0.299067, 0.041221,0.283536, 0.125459,0.234982, 0.181143,0.183624, 0.228595,0.105917, 0.268750,0.012281, 0.313793,-0.081056, 0.306997,-0.188603, 0.262425,-0.286297, 0.218519,-0.355547, 0.153244,-0.376219, 0.046875,-0.364611, -0.019398,-0.349785, -0.083546,-0.322881, -0.141981,-0.272036, -0.195959,-0.227784, -0.221863,-0.171718, -0.239605,-0.111801, -0.260405,-0.063040, -0.276108,0.051160, -0.250004,0.138005, -0.184725,0.213503, -0.125567,0.259839,
// shEmeraldFloor[1] (1x1)
NEWSHAPE, 168,1,1, -0.012456,-0.282668, 0.093943,-0.261125, 0.176033,-0.224423, 0.251304,-0.129592, 0.253408,0.003018, 0.205115,0.151740, 0.136504,0.257503, 0.050478,0.321171, -0.024338,0.351747, -0.113671,0.338793, -0.199507,0.312663, -0.289397,0.244537, -0.318919,0.189505, -0.296780,0.103663, -0.266492,-0.004074, -0.225121,-0.113729, -0.162566,-0.195216, -0.093623,-0.256862, -0.073239,-0.271190,
// shEmeraldFloor[2] (1x1)
NEWSHAPE, 169,1,1, 0.142342,0.315002, 0.192633,0.259152, 0.232117,0.228558, 0.263994,0.150040, 0.283920,0.084560, 0.289892,-0.010808, 0.297148,-0.086089, 0.297551,-0.165417, 0.290991,-0.200376, 0.258510,-0.274435, 0.200576,-0.292802, 0.137570,-0.326255, 0.057449,-0.363515, 0.007219,-0.381780, -0.042563,-0.362309, -0.099799,-0.296581, -0.126618,-0.259997, -0.164471,-0.210125, -0.206762,-0.179726, -0.255672,-0.132984, -0.278657,-0.105484, -0.292210,-0.086272, -0.317777,-0.034598, -0.332027,0.039956, -0.311197,0.107127, -0.293985,0.148441, -0.261872,0.181378, -0.161843,0.220098, -0.080956,0.253464, -0.028510,0.284753, 0.013578,0.313129, 0.055797,0.341483,
// shEmeraldFloor[3] (1x1)
NEWSHAPE, 170,1,1, -0.163082,-0.220437, -0.210355,-0.178152, -0.270116,-0.134375, -0.286296,-0.068967, -0.322640,0.023933, -0.303410,0.080931, -0.256143,0.157047, -0.191383,0.193946, -0.112200,0.248267, -0.006180,0.297906, 0.045011,0.314576, 0.123367,0.290642, 0.173268,0.251704, 0.226704,0.200367, 0.274509,0.112459, 0.272693,0.041554, 0.266634,-0.039227, 0.245934,-0.087769, 0.206654,-0.145971, 0.168911,-0.181347, 0.113778,-0.241571, 0.060166,-0.287358, -0.030419,-0.300995, -0.106762,-0.252814,
// shEmeraldFloor[4] (1x1)
NEWSHAPE, 171,1,1, -0.121227,-0.290032, -0.211853,-0.226567, -0.271304,-0.142026, -0.291031,-0.001756, -0.289025,0.081565, -0.274404,0.151774, -0.270636,0.184903, -0.177964,0.228844, -0.109553,0.258706, -0.046057,0.288157, 0.012352,0.334753, 0.093790,0.294439, 0.113901,0.258432, 0.169445,0.181108, 0.201022,0.133994, 0.217873,0.089759, 0.229954,0.028756, 0.219894,-0.023561, 0.191854,-0.102714, 0.155541,-0.176809, 0.124174,-0.224046, 0.061461,-0.284240, 0.004090,-0.327127,
// shEmeraldFloor[5] (1x1)
NEWSHAPE, 172,1,1, -0.123950,0.204444, -0.062627,0.216559, 0.004307,0.251676, 0.053230,0.277691, 0.104981,0.268275, 0.179487,0.211716, 0.219611,0.155886, 0.242732,0.098595, 0.253441,0.041064, 0.251167,-0.002564, 0.258569,-0.050639, 0.241512,-0.095904, 0.224178,-0.142437, 0.200723,-0.160460, 0.160438,-0.177252, 0.098857,-0.201735, 0.042704,-0.222826, -0.046440,-0.220643, -0.121819,-0.210954, -0.193674,-0.181500, -0.240226,-0.152286, -0.288784,-0.106825, -0.312611,-0.056858, -0.322108,0.009276, -0.323420,0.074633, -0.320770,0.110549, -0.238954,0.181091,
// shRoseFloor[2] (14x2)
NEWSHAPE, 173,14,2, -0.548465,-0.132422, -0.551824,-0.102571, -0.528982,-0.047757, -0.442542,-0.018200, -0.315538,-0.004582,
// shRoseFloor[0] (3x1)
NEWSHAPE, 174,3,1, 0.264832,-0.019358, 0.272178,-0.037378, 0.276373,-0.078165, 0.266115,-0.117875, 0.242680,-0.150883, 0.209916,-0.171773, 0.181695,-0.180720, 0.145686,-0.180823, 0.099010,-0.173709, 0.080081,-0.167628, 0.071976,-0.220677, 0.055614,-0.256642, 0.042043,-0.264568, 0.010906,-0.279995, -0.019644,-0.286339, -0.054597,-0.283522, -0.091229,-0.258700, -0.116444,-0.237228, -0.130501,-0.207658, -0.140731,-0.154944, -0.136597,-0.109083, -0.128765,-0.083983, -0.118905,-0.052948, -0.101590,-0.027009, -0.093459,-0.015558, -0.103528,-0.017996, -0.124791,-0.033412, -0.146258,-0.066451, -0.157241,-0.103135, -0.159819,-0.147005,
// shRoseFloor[1] (7x2)
NEWSHAPE, 175,7,2, 0.372919,0.175334, 0.377019,0.163872, 0.375818,0.147479, 0.369194,0.127784, 0.350154,0.106817, 0.321073,0.087366, 0.290798,0.067704, 0.264404,0.056694, 0.272461,0.048502, 0.305905,0.057526, 0.332293,0.060486, 0.365740,0.053423, 0.392326,0.029151, 0.394325,0.004706,
// shTurtleFloor[0] (6x2)
NEWSHAPE, 176,6,2, 0.112369,0.059775, 0.111167,0.006757, 0.119117,0.006652, 0.119333,0.067428, 0.214640,0.118987, 0.262563,0.053738,
// shTurtleFloor[1] (7x2)
NEWSHAPE, 177,7,2, 0.299631,0.136858, 0.313974,0.087924, 0.223968,0.016894, 0.122331,0.014981, 0.111796,0.050597, 0.106729,0.040319, 0.112168,0.004682, 0.235569,0.008771, 0.315941,0.071298, 0.418977,0.047086, 0.473946,0.006115,
// shTurtleFloor[2] (7x2)
NEWSHAPE, 178,7,2, -0.558425,-0.248734, -0.539322,-0.019784, -0.279489,-0.005033, -0.252876,-0.100862, -0.234059,-0.088782, -0.261386,0.010296,
// shAztecHead (1x2)
NEWSHAPE, 179,1,2, 0.074692,0.000429, 0.066283,-0.019417, 0.046672,-0.015897, 0.034348,-0.045854, 0.010851,-0.055008, -0.020151,-0.062581, -0.045128,-0.057532, -0.068539,-0.044938, -0.083393,-0.018981, -0.086999,-0.006818,
// shAztecCap (1x2)
NEWSHAPE, 180,1,2, 0.001102,0.007720, 0.005712,-0.021311, -0.006877,-0.046748, -0.027647,-0.060816, -0.056423,-0.068486, -0.093017,-0.074757, -0.122525,-0.080547, -0.133671,-0.117082, -0.101438,-0.158050, -0.077029,-0.154904, -0.062688,-0.128399, -0.047439,-0.156234, -0.091646,-0.199265, -0.164679,-0.177117, -0.182172,-0.095454, -0.158692,-0.051866, -0.134446,-0.013804, -0.129636,-0.010802, -0.117269,-0.007091,
// shDragonFloor[0] (3x2)
NEWSHAPE, 181,3,2, -0.137150,-0.234461, -0.227787,-0.028743, -0.260021,-0.029555, -0.301991,-0.109956, -0.386143,-0.013264,
// shDragonFloor[1] (7x2)
NEWSHAPE, 182,7,2, 0.205492,0.081657, 0.333919,0.017825, 0.361617,0.020628, 0.363346,0.130144, 0.529009,0.017851,
// shDragonFloor[2] (6x1)
NEWSHAPE, 183,6,1, -0.255833,0.146834, -0.186167,0.179834, -0.195333,0.112000, -0.079833,0.097334, -0.072500,0.183500, -0.178833,0.280667, -0.166000,0.339334, -0.089000,0.326500, -0.079833,0.258667,
// shTower[0] (1x1)
NEWSHAPE, 184,1,1, -0.286251,0.206460, -0.228388,0.237443, -0.159130,0.310692, -0.101028,0.374054, 0.075166,0.301721, 0.296628,0.282561, 0.392862,0.273630, 0.406414,0.155729, 0.351704,0.039501, 0.238677,-0.090994, 0.117245,-0.194511, 0.021470,-0.256598, -0.009932,-0.269112, -0.142800,-0.030207,
// shTower[1] (1x1)
NEWSHAPE, 185,1,1, 0.250672,0.215566, -0.010561,0.166674, -0.188122,0.172750, -0.277968,0.195647, -0.273175,-0.294725, -0.058073,-0.278517, 0.114633,-0.264898, 0.214865,-0.223092, 0.318979,-0.166948, 0.348056,-0.154771,
// shTower[2] (1x1)
NEWSHAPE, 186,1,1, -0.305085,0.182715, -0.141572,-0.249919, -0.082532,-0.256373, 0.011916,-0.200378, 0.107643,-0.130975, 0.197879,-0.052950, 0.275493,0.054106, 0.346502,0.177608, 0.365290,0.224553, -0.042684,0.405864,
// shTower[3] (1x1)
NEWSHAPE, 187,1,1, -0.244566,0.213804, -0.121406,0.179228, 0.102781,0.233054, 0.137859,0.248290, 0.219219,0.271805, 0.439787,-0.128376, 0.327884,-0.165668, 0.191605,-0.198467, -0.015788,-0.263051, -0.162942,-0.274063, -0.284113,-0.264520, -0.360780,-0.248815,
// shTower[4] (1x1)
NEWSHAPE, 188,1,1, 0.571958,-0.279170, 0.393753,-0.334598, 0.230386,-0.380872, 0.040909,-0.353244, -0.132206,-0.325608, -0.193426,-0.301627, -0.209589,-0.285475, -0.184256,0.133876, -0.122720,0.103535, 0.028404,0.096630, 0.131561,0.118840, 0.206961,0.143553, 0.285398,0.171884, 0.311736,0.188934, 0.427540,-0.043657,
// shTower[5] (1x1)
NEWSHAPE, 189,1,1, -0.311898,0.273433, -0.241832,0.284721, -0.093134,0.385666, -0.048259,0.410019, -0.034956,0.418726, 0.323554,0.088209, 0.281449,-0.050570, 0.166974,-0.175799, 0.053476,-0.232832, -0.046497,-0.270575, -0.167112,-0.296403, -0.182433,-0.298027, -0.237210,-0.011802,
// shTower[6] (1x1)
NEWSHAPE, 190,1,1, 0.591895,-0.274213, 0.451640,-0.046334, 0.339584,0.178653, 0.075724,0.130436, -0.048271,0.097633, -0.223763,-0.157117, -0.073558,-0.240165, 0.043434,-0.280964, 0.298734,-0.305291,
// shTower[7] (1x1)
NEWSHAPE, 191,1,1, 0.078002,0.370221, 0.413248,0.133206, 0.355187,0.048021, 0.239656,-0.028483, 0.091569,-0.106563, -0.053014,-0.170046, -0.148171,-0.179787, -0.226021,0.273779,
// shTower[8] (1x1)
NEWSHAPE, 192,1,1, 0.533180,-0.117639, 0.378296,-0.199392, 0.209814,-0.236744, 0.054537,-0.276614, -0.034481,-0.273513, -0.160263,-0.038320, -0.297354,0.194169, -0.193697,0.211849, -0.063165,0.262989, 0.036923,0.306670, 0.097986,0.358635, 0.135238,0.379300, 0.318600,0.124720,
// shTower[9] (1x1)
NEWSHAPE, 193,1,1, -0.217261,0.132486, 0.024523,0.407718, 0.145333,0.449681, 0.484226,0.101907, 0.321456,-0.134096, -0.036797,-0.295091, -0.180549,-0.344274, -0.259938,-0.172378,
// shTower[10] (1x1)
NEWSHAPE, 194,1,1, -0.174062,-0.167910, -0.295951,0.239183, -0.212854,0.268510, -0.079667,0.325104, -0.024447,0.357384, 0.004880,0.379437, 0.047240,0.411450, 0.404628,0.134398, 0.279121,0.016611, 0.142934,-0.078661, 0.031621,-0.149237, -0.095567,-0.210063,
// shTower[11] (1x1)
NEWSHAPE, 195,1,1, -0.261640,0.232003, -0.114066,0.222344, 0.070087,0.311006, 0.146439,0.385502, 0.156417,0.393458, 0.321239,0.151530, 0.513393,-0.077506, 0.367820,-0.144703, 0.183543,-0.172959, 0.022726,-0.215721, -0.158572,-0.235502, -0.259609,-0.221068, -0.283069,-0.211523,
// shTower[0] (1x1)
NEWSHAPE, 196,1,1, -0.343678,0.205736, -0.313502,-0.269598, 0.053446,-0.276534, 0.316036,-0.088255, 0.415175,0.117882, -0.013091,0.329953,
// shTower[1] (1x1)
NEWSHAPE, 197,1,1, -0.311517,0.177757, -0.277989,0.198280, -0.192835,0.277382, -0.118766,0.352616, 0.340632,0.220226, 0.346703,0.158836, 0.324069,0.046483, 0.255673,-0.074052, 0.185700,-0.149283, 0.033206,-0.246367, -0.096570,-0.283752, -0.194307,-0.258126, -0.265575,-0.226804, -0.285480,-0.219017,
// shTower[2] (1x1)
NEWSHAPE, 198,1,1, -0.287245,0.124770, -0.224066,0.141558, -0.131929,0.201218, -0.078630,0.261221, -0.020026,0.337507, 0.318283,0.112649, 0.303068,-0.062313, 0.276375,-0.100467, 0.222744,-0.146642, 0.008616,-0.304232, -0.058987,-0.274278, -0.112858,-0.303563, -0.233080,-0.305264, -0.372515,-0.347162,
// shTower[3] (1x1)
NEWSHAPE, 199,1,1, -0.276471,0.193549, -0.248272,0.167366, -0.131672,0.161485, -0.029928,0.144574, 0.145677,0.141539, 0.244238,0.160944, 0.278308,0.181096, 0.290850,-0.178199, 0.244356,-0.239966, 0.158564,-0.296188, 0.052883,-0.325301, -0.059036,-0.324976, -0.156078,-0.299394, -0.222738,-0.269837, -0.251922,-0.245775,
// shTower[4] (1x1)
NEWSHAPE, 200,1,1, -0.311686,0.135147, -0.107931,0.170082, -0.054960,0.232687, 0.094837,0.235522, 0.206298,0.281848, 0.350261,-0.169080, 0.258249,-0.214408, 0.149756,-0.245358, 0.010018,-0.258374, -0.164035,-0.248262, -0.260903,-0.226907, -0.329503,-0.207534,
// shTower[5] (1x1)
NEWSHAPE, 201,1,1, -0.278494,0.161752, -0.132974,0.168104, -0.045035,0.201619, 0.004416,0.226141, 0.097798,0.196263, 0.208310,0.187256, 0.213133,0.158213, 0.255407,-0.229136, 0.150044,-0.303075, 0.021265,-0.309104, -0.124340,-0.302692, -0.340803,-0.287456, -0.338423,-0.172501,
// shTower[6] (1x1)
NEWSHAPE, 202,1,1, -0.033122,-0.384901, 0.122565,-0.232523, 0.222346,-0.147072, 0.293004,-0.141494, 0.228575,0.266628, 0.142937,0.283066, 0.030023,0.299757, -0.078299,0.271316, -0.144933,0.245692, -0.207818,0.168012, -0.262782,0.117599, -0.313780,0.032325, -0.338217,-0.056491, -0.334152,-0.102325,
// shTower[7] (1x1)
NEWSHAPE, 203,1,1, -0.547921,0.434077, -0.432932,0.444800, -0.319705,0.505720, -0.203488,0.623669, -0.096990,0.678123, 0.227072,0.612087, 0.575897,0.612804, 0.593166,0.304792, 0.552524,0.120670, 0.446203,-0.067415, 0.274469,-0.190444, 0.132895,-0.256501, -0.012450,-0.301315, -0.125769,-0.381194, -0.269514,-0.401566, -0.392905,-0.383183, -0.443645,-0.375389, -0.432392,-0.062656, -0.461638,0.244209,
// shTower[8] (1x1)
NEWSHAPE, 204,1,1, -0.471133,0.305753, -0.159124,0.299060, 0.208647,0.436673, 0.398417,0.636059, 0.544645,0.255027, 0.777805,-0.085301, 0.517790,-0.221942, 0.294745,-0.374429, 0.158715,-0.473212, -0.080020,-0.462822, -0.384068,-0.511651, -0.395566,-0.095237,
// shTower[9] (1x1)
NEWSHAPE, 205,1,1, 0.449766,-0.690111, -0.024136,-0.595991, -0.057829,-0.383939, -0.467634,-0.413702, -0.438668,-0.055072, -0.463281,0.296832, -0.494437,0.335413, -0.124821,0.618109, 0.139228,0.529058, 0.413839,0.480139, 0.501877,0.230248, 0.624273,-0.003880, 0.739892,-0.031980, 0.559897,-0.339843,
// shTower[10] (2x1)
NEWSHAPE, 206,2,1, 0.250000,-0.210000, -0.250000,-0.210000,
// shTortoise[0][0+u] (1x2)
NEWSHAPE, 207,1,2, 0.130604,0.040186, 0.114944,0.065842, 0.093777,0.101592, 0.054693,0.123896, 0.014512,0.136188, -0.039114,0.147516, -0.099597,0.137646, -0.136732,0.127766, -0.168324,0.108850, -0.193220,0.085376, -0.210135,0.041577,
// shTortoise[1][0+u] (1x2)
NEWSHAPE, 208,1,2, 0.115992,0.041266, 0.089260,0.093723, 0.044488,0.044488,
// shTortoise[2][0+u] (1x2)
NEWSHAPE, 209,1,2, 0.034470,0.042254, -0.004448,0.060054, -0.037810,0.044482,
// shTortoise[3][0+u] (1x2)
NEWSHAPE, 210,1,2, -0.048945,0.046720, -0.085793,0.061280, -0.121603,0.039047,
// shTortoise[4][0+u] (1x2)
NEWSHAPE, 211,1,2, -0.131730,0.040189, -0.184061,0.083052, -0.200961,0.041539,
// shTortoise[5][0+u] (1x1)
NEWSHAPE, 212,1,1, 0.080316,0.097049, 0.035588,0.048934, 0.000000,0.067856, 0.004461,0.123805, 0.041274,0.119361,
// shTortoise[6][0+u] (1x1)
NEWSHAPE, 213,1,1, -0.046728,0.055629, -0.011124,0.067858, -0.010038,0.122683, -0.034616,0.137349, -0.079314,0.123998, -0.077994,0.071309,
// shTortoise[7][0+u] (1x1)
NEWSHAPE, 214,1,1, -0.090293,0.070228, -0.091619,0.118434, -0.129870,0.117555, -0.171553,0.086337, -0.128389,0.052472,
// shTortoise[8][0+u] (1x2)
NEWSHAPE, 215,1,2, 0.117447,0.019850, 0.124690,0.033104, 0.143100,0.043648, 0.156518,0.044798, 0.172226,0.045964, 0.185156,0.042131, 0.198679,0.033298, 0.209415,0.022219, 0.213376,0.015559,
// shTortoise[12][0+u] (1x1)
NEWSHAPE, 216,1,1, 0.189028,0.011641, 0.192981,0.013309, 0.193557,0.019966, 0.189620,0.027168, 0.183986,0.028817, 0.175528,0.022149, 0.174396,0.017163, 0.180574,0.010524,
// shTortoise[9][0+u] (1x1)
NEWSHAPE, 217,1,1, 0.091033,0.091033, 0.098877,0.108268, 0.113462,0.128959, 0.125832,0.141353, 0.137077,0.147622, 0.124211,0.147500, 0.127683,0.159881, 0.114214,0.153579, 0.114888,0.166505, 0.101432,0.158522, 0.098764,0.173114, 0.089226,0.164043, 0.085437,0.178641, 0.079119,0.147726, 0.066181,0.103132,
// shTortoise[10][0+u] (1x1)
NEWSHAPE, 218,1,1, -0.127294,0.114564, -0.165450,0.169906, -0.170219,0.186962, -0.172820,0.171706, -0.183575,0.171299, -0.183400,0.159430, -0.197560,0.157378, -0.192844,0.145469, -0.208144,0.141739, -0.185331,0.134128, -0.168270,0.122731, -0.149026,0.105814,
// shTortoise[11][0+u] (1x2)
NEWSHAPE, 219,1,2, -0.198773,0.054968, -0.210619,0.042791, -0.215140,0.036691, -0.225922,0.025041, -0.241923,0.019510, -0.258017,0.015079, -0.278275,0.012318, -0.288758,0.012335, -0.296937,0.009542, -0.302208,0.005056, -0.305142,0.000562,
// shTortoise[0][1+u] (1x2)
NEWSHAPE, 220,1,2, -0.215020,0.032895, -0.196018,0.086615, -0.144388,0.137555, -0.039214,0.155679, 0.055338,0.132276, 0.091306,0.105859, 0.110754,0.069544, 0.130081,0.057985, 0.148913,0.053201, 0.153797,0.043848,
// shTortoise[1][1+u] (1x2)
NEWSHAPE, 221,1,2, 0.054487,0.011120, 0.041149,0.044485, 0.072368,0.053441, 0.083644,0.089220, 0.117129,0.046852,
// shTortoise[2][1+u] (1x2)
NEWSHAPE, 222,1,2, 0.040018,0.014451, 0.031130,0.038913, -0.001112,0.058940, -0.035584,0.044480, -0.020005,0.023339,
// shTortoise[3][1+u] (1x2)
NEWSHAPE, 223,1,2, -0.040020,0.018898, -0.050063,0.050063, -0.077977,0.064609, -0.119364,0.042391, -0.106982,0.020059,
// shTortoise[4][1+u] (1x2)
NEWSHAPE, 224,1,2, -0.122687,0.014499, -0.129484,0.042417, -0.152149,0.067124, -0.178269,0.069514, -0.197514,0.038156, -0.184908,0.021292,
// shTortoise[5][1+u] (1x1)
NEWSHAPE, 225,1,1, 0.076947,0.093675, 0.060121,0.066801, 0.032253,0.052271, -0.001112,0.070086, 0.006682,0.095775, 0.003346,0.123804, 0.040155,0.118235,
// shTortoise[6][1+u] (1x1)
NEWSHAPE, 226,1,1, 0.000000,0.100251, -0.006693,0.126054, -0.027911,0.136208, -0.074828,0.122851, -0.057958,0.095854, -0.076880,0.072423, -0.042276,0.057852, -0.013350,0.070089,
// shTortoise[7][1+u] (1x1)
NEWSHAPE, 227,1,1, -0.079176,0.091443, -0.091663,0.126316, -0.123093,0.116379, -0.172636,0.078471, -0.145392,0.072696, -0.120531,0.056917, -0.090297,0.071346,
// shTortoise[8][1+u] (1x2)
NEWSHAPE, 228,1,2, 0.122997,0.015995, 0.146947,0.015468, 0.167654,0.014386, 0.182833,0.016067, 0.199210,0.019976, 0.209990,0.025554, 0.220813,0.031704, 0.229392,0.037861, 0.242002,0.041263, 0.255227,0.041328, 0.264456,0.039696, 0.275450,0.036391, 0.283564,0.030262, 0.289940,0.019068, 0.293428,0.010660,
// shTortoise[12][1+u] (1x1)
NEWSHAPE, 229,1,1, 0.268412,0.011184, 0.273055,0.015108, 0.274235,0.022946, 0.265563,0.028513, 0.260944,0.029615, 0.255733,0.024010, 0.255715,0.017308, 0.255713,0.016191,
// shTortoise[9][1+u] (1x1)
NEWSHAPE, 230,1,1, 0.057351,0.106430, 0.077464,0.149395, 0.093278,0.183779, 0.107707,0.228249, 0.112041,0.215164, 0.126140,0.219908, 0.127627,0.204537, 0.142352,0.211574, 0.109929,0.173777, 0.072799,0.098720,
// shTortoise[10][1+u] (1x1)
NEWSHAPE, 231,1,1, -0.136932,0.091981, -0.259079,0.148132, -0.268509,0.159166, -0.266397,0.150469, -0.274082,0.151224, -0.268499,0.144503, -0.277008,0.143964, -0.270075,0.137129, -0.278972,0.133484, -0.265628,0.133017, -0.150766,0.083180,
// shTortoise[11][1+u] (1x2)
NEWSHAPE, 232,1,2, -0.188793,-0.018459, -0.214469,-0.012848, -0.236108,-0.035186, -0.268017,-0.009275, -0.280049,-0.007050,
// shDragonSegment (1x2)
NEWSHAPE, 233,1,2, -0.203174,0.011881, -0.186616,-0.109701, -0.119110,-0.176468, 0.041364,-0.257437, 0.024528,-0.167332, 0.168002,-0.237034, 0.122445,-0.154843, 0.293732,-0.215965, 0.243967,-0.121295, 0.379618,-0.208934, 0.352319,-0.102369, 0.499801,-0.217583, 0.458633,-0.130186, 0.629571,-0.201212, 0.600616,-0.112655, 0.808599,-0.206589, 0.759781,-0.121862, 0.966786,-0.214338, 0.904584,-0.144299, 1.120205,-0.210597, 1.072776,-0.124515, 1.265050,-0.194962, 1.187624,-0.110483, 1.448051,-0.152371, 1.331387,-0.062841, 1.531624,-0.093190, 1.375131,-0.019085,
// shDragonSegment (1x2)
NEWSHAPE, 234,1,2, -0.007346,-0.123992, 0.122987,-0.173919, 0.061557,-0.116488, 0.202542,-0.163574, 0.147987,-0.118203, 0.282832,-0.158128, 0.213251,-0.117597, 0.363492,-0.156273, 0.302775,-0.112860, 0.441499,-0.158901, 0.388517,-0.109014, 0.509440,-0.155192, 0.465857,-0.105710, 0.586378,-0.160120, 0.536831,-0.108756, 0.666714,-0.160638, 0.614597,-0.110994, 0.707340,-0.118674, 0.642298,-0.079128, 0.729313,-0.077848, 0.661282,-0.043253, 0.733677,-0.031495, 0.672818,-0.004657,
// shSolidBranch (3x2)
NEWSHAPE, 235,3,2, 0.128555,0.209748, 0.199690,0.215574, 0.254193,0.204038, 0.210912,0.198439, 0.140478,0.162954, 0.065662,0.046742, 0.052259,0.011119,
// shWeakBranch (3x2)
NEWSHAPE, 236,3,2, 0.022229,0.026675, 0.044472,0.022236, 0.075677,0.026710, 0.114862,0.037915, 0.161146,0.053715, 0.167896,0.039176, 0.135065,0.017860, 0.127177,0.003347,
// shDragonWings (1x2)
NEWSHAPE, 237,1,2, 0.122700,0.018517, 0.114272,0.053018, 0.089169,0.100403, 0.057758,0.136154, -0.006949,0.158795, -0.095746,0.184719, -0.062179,0.148185, -0.123227,0.161622, -0.083331,0.114547, -0.148338,0.120521, -0.091130,0.073903, -0.172062,0.072792, -0.108240,0.034096, -0.179205,0.026589, -0.114815,0.000267,
// shDragonLegs (1x2)
NEWSHAPE, 238,1,2, 0.134917,-0.000905, 0.124854,0.043901, 0.099656,0.088037, 0.061927,0.126507, -0.022131,0.207886, -0.080657,0.255286, 0.215933,0.322901, 0.307187,0.312641, 0.266293,0.353495, 0.332413,0.379096, 0.261603,0.390908, 0.296236,0.449480, 0.224166,0.406153, 0.235023,0.466366, 0.183625,0.408492, -0.160855,0.327637, -0.192176,0.275601, -0.152760,0.194158, -0.049616,0.086549, -0.073488,0.043239, -0.075888,0.011802,
// shDragonTail (1x2)
NEWSHAPE, 239,1,2, 0.363554,-0.008229, 0.128365,-0.123955, -0.098304,-0.247909, -0.020731,-0.079043, -0.237275,-0.091634, -0.207909,-0.172106, -0.358930,-0.064586, -0.336385,-0.155676, -0.534253,-0.055873, -0.485229,-0.140288, -0.721828,-0.084122, -0.668232,-0.161876, -0.941472,-0.144105, -0.866272,-0.216238, -1.272595,-0.235195, -1.430879,-0.234454, -1.594270,-0.145883, -1.654478,-0.053066,
// shDragonTail (1x2)
NEWSHAPE, 240,1,2, -0.778183,-0.033498, -0.773667,-0.113471, -0.718901,-0.170803, -0.626330,-0.194636, -0.662726,-0.150620, -0.561857,-0.166840, -0.594130,-0.120798, -0.492551,-0.149697, -0.515916,-0.118685, -0.385534,-0.144280, -0.422856,-0.102151, -0.286384,-0.135043, -0.334390,-0.084182, -0.207305,-0.108259, -0.268575,-0.064828, -0.167373,-0.087126, -0.211574,-0.057493, -0.167164,-0.050378, -0.104898,-0.050168, -0.043224,-0.045499, 0.038675,-0.050050, 0.061460,-0.050078, -0.018255,-0.125501, 0.206740,-0.025268, 0.268300,-0.004626,
// shDragonNostril (1x1)
NEWSHAPE, 241,1,1, -0.196345,-0.030293, -0.173587,-0.042557, -0.146359,-0.027931, -0.175798,-0.019035,
// shDragonHead (1x2)
NEWSHAPE, 242,1,2, -0.234288,-0.029286, -0.210215,-0.057331, -0.171514,-0.080712, -0.173633,-0.053770, -0.129710,-0.093928, -0.135211,-0.068164, -0.091567,-0.108317, -0.094829,-0.085904, -0.053575,-0.123893, -0.056851,-0.099211, -0.016761,-0.149729, -0.015612,-0.119318, 0.010056,-0.149722, 0.034683,-0.163345, 0.042545,-0.170178, 0.097987,-0.213995, 0.128990,-0.241008, 0.156615,-0.250811, 0.192347,-0.250392, 0.215391,-0.242742, 0.241399,-0.208378, 0.244430,-0.187586, 0.234221,-0.201248, 0.205478,-0.215695, 0.188163,-0.216501, 0.172921,-0.200046, 0.159959,-0.174604, 0.152696,-0.138100, 0.161616,-0.121212, 0.207372,-0.122845, 0.171669,-0.100982, 0.218526,-0.090114, 0.180599,-0.077400, 0.232188,-0.067628, 0.196472,-0.059503, 0.244802,-0.047381, 0.210112,-0.035955, 0.244719,-0.029321, 0.215818,-0.023605, 0.241204,-0.016907, 0.212345,-0.008988, 0.244667,-0.001127,
// shMagicSword (1x2)
NEWSHAPE, 243,1,2, 0.460244,0.025013, 0.556616,0.025533, 0.591538,0.202368, 0.654732,0.232324, 0.666558,0.078727, 0.994985,0.063137, 1.388046,0.051648, 1.514842,0.013406,
// shMagicSword (1x2)
NEWSHAPE, 244,1,2, 0.273115,0.018516, 0.287601,0.032471, 0.306938,0.020928, 0.314379,0.046575, 0.307461,0.086182, 0.300388,0.102458, 0.332095,0.095887, 0.346420,0.056176, 0.351178,0.028094, 0.752221,0.023027, 0.794417,0.005175,
// shSeaTentacle (1x1)
NEWSHAPE, 245,1,1, -2.153013,0.033944, -1.647254,0.158902, -1.268813,0.296141, -1.133858,0.242342, -1.041458,0.212975, -0.932914,0.195867, -0.909023,0.199004, -0.754650,0.193789, -0.634570,0.185783, -0.489278,0.166282, -0.343249,0.107130, -0.220848,0.062181, -0.137036,0.039277, 0.019954,0.022781, 0.115439,-0.000192, -0.000098,-0.050493, -0.084985,-0.057626, -0.203684,-0.064431, -0.328103,-0.033983, -0.463800,-0.046175, -0.786647,-0.048919, -1.118381,-0.076511, -1.227766,-0.140168, -1.234647,-0.210899, -1.233924,-0.265574, -1.626076,-0.134736, -2.146445,-0.014522,
// shSeaTentacle (1x1)
NEWSHAPE, 246,1,1, -0.581517,0.000237, -0.270972,0.171843, -0.213352,0.119030, -0.161150,0.092951, -0.114611,0.074683, -0.047157,0.071482, 0.019130,0.078297, 0.104805,0.092171, 0.193609,0.071644, 0.240320,0.035980, 0.258904,-0.018841, 0.253114,-0.070738, 0.241935,-0.086882, 0.221553,-0.081501, 0.224073,-0.073361, 0.208946,-0.042488, 0.169897,-0.033591, 0.117246,-0.046730, 0.027793,-0.098385, -0.054463,-0.112917, -0.147724,-0.107183, -0.203144,-0.111772, -0.254451,-0.143591, -0.272922,-0.167873,
// shKrakenHead (1x2)
NEWSHAPE, 247,1,2, -0.532637,0.022779, -0.550265,0.061628, -0.524692,0.130147, -0.455350,0.206917, -0.377664,0.268147, -0.273227,0.272552, -0.185554,0.237075, -0.136140,0.194766, -0.126552,0.184923, -0.116668,0.169553, -0.101930,0.146766, -0.075415,0.127121, -0.041274,0.120227, 0.113990,0.136127,
// shKrakenEye (1x1)
NEWSHAPE, 248,1,1, 0.130451,0.002988, 0.151783,0.013236, 0.171439,0.034011, 0.184575,0.063992, 0.184241,0.085007, 0.167638,0.109355, 0.149908,0.128805, 0.112562,0.135825, 0.088512,0.122949, 0.067899,0.104186, 0.055509,0.067503, 0.062374,0.041655, 0.071027,0.024842, 0.081064,0.012305, 0.095177,0.004604, 0.116385,0.000336, 0.124753,0.002566,
// shKrakenEye2 (1x1)
NEWSHAPE, 249,1,1, 0.172608,0.075020, 0.173285,0.064902, 0.164089,0.049576, 0.147378,0.049339, 0.135325,0.053681, 0.128076,0.066823, 0.128055,0.080938, 0.137094,0.092105, 0.146798,0.094361, 0.154590,0.091353, 0.165226,0.082730,
// shBead0 (1x2)
NEWSHAPE, 250,1,2, 0.340267,-0.226747, 0.208689,-0.202368,
// shBead1 (1x1)
NEWSHAPE, 251,1,1, -0.186883,0.074525, -0.165331,0.041449, -0.154901,0.003708, -0.156520,-0.035345, -0.170044,-0.072239, -0.194272,-0.103696, -0.227049,-0.126921, -0.265465,-0.139850, -0.306106,-0.141335, -0.345360,-0.131244, -0.379739,-0.110473, -0.406190,-0.080868, -0.422362,-0.045059, -0.426817,-0.006228, -0.419160,0.032174, -0.400071,0.066735, -0.371247,0.094385, -0.335248,0.112667, -0.295273,0.119955, -0.254875,0.115604, -0.217642,0.099998,
// shArrow (1x2)
NEWSHAPE, 252,1,2, -0.053383,0.030028, 0.040021,0.022234, 0.033369,0.056728, 0.101370,0.001114,
// shTreat
NEWSHAPE, 253,3,2, 0.220188,0.075636, 0.089347,0.084922, 0.109762,0.055626, 0.120125,0.037135, 0.127589,0.018676, 0.128473,0.010613, 
// shBatWings
NEWSHAPE, 254,1,2, 0.063731,-0.043246, 0.124072,-0.170024, 0.084056,-0.324550, -0.103512,-0.366996, -0.074400,-0.297599, -0.108529,-0.230913, -0.086972,-0.144190, -0.118951,-0.109801, -0.072887,-0.059221, -0.167080,-0.022888, 
// shBatBody
NEWSHAPE, 255,1, 2, -0.061429,-0.022752, -0.059165,-0.038685, -0.141985,-0.103054, -0.043229,-0.050055, 0.029576,-0.056876, 0.080137,-0.153405, -0.107472,-0.114332, 0.084857,-0.172008, -0.094609,-0.230753, 0.085016,-0.193009, 0.060443,-0.299888, -0.096263,-0.356877, 0.086368,-0.322128, 0.128679,-0.167743, 0.068287,-0.038696, 0.148735,-0.073224, 0.174190,-0.061883, 0.183380,-0.032092, 0.157798,-0.016009, 0.118603,-0.015966, 
// shBatMouth
NEWSHAPE, 256, 1, 2, 0.065982,0.006826, 0.081971,0.027324, 0.091097,0.002277, 
// shBatFang
NEWSHAPE, 257, 1, 1, 0.084220,0.016514, 0.054588,0.011372, 0.088636,0.006606, 
// shBatEye
NEWSHAPE, 258, 1, 1, 0.100828,0.030855, 0.108021,0.024250, 0.106903,0.014878, 0.099146,0.009915, 

// shHumanFoot
NEWSHAPE, 259, 1, 1, -0.091073,0.081161, -0.071926,0.092823, -0.046931,0.094885, -0.019968,0.097943, -0.001122,0.092149, 0.011212,0.084470, 0.010501,0.070502, -0.001155,0.056218, -0.018631,0.051502, -0.070956,0.049847, -0.086365,0.056093, -0.094267,0.064719, 
// shHumanLeg
NEWSHAPE, 260, 1, 1, -0.085549,0.082560, -0.066987,0.087728, -0.050005,0.088432, -0.036738,0.073722, -0.033485,0.056714, -0.050113,0.042534, -0.076986,0.046956, -0.086334,0.058587, 
// shHumanGroin
NEWSHAPE, 261, 1, 2, -0.092531,0.043227, -0.077988,0.087426, -0.048692,0.113964, -0.022425,0.100486, 0.002705,0.048193, 
// shHumanNeck
NEWSHAPE, 262, 1, 2, -0.035947,0.023404, -0.011281,0.043922, 0.017582,0.045486, 0.031600,0,
// shSemiFloorShadow
NEWSHAPE, 263, 1, 2, -0.317691,0.208329, 0.065767,0.397781, 
// shSkeletalFoot
NEWSHAPE, 264, 1, 1, -0.082697,0.087468, -0.071752,0.102240, -0.048756,0.105169, 0.005645,0.101864, 0.009206,0.097754, 0.005659,0.094999, -0.043535,0.093347, -0.043804,0.090603, 0.007314,0.087046, 0.010873,0.084038, 0.005962,0.079364, -0.043239,0.079083, -0.045150,0.075798, 0.004608,0.072508, 0.010903,0.067865, 0.006269,0.060184, -0.065401,0.062704, -0.079938,0.072088, 
// shDogBody
NEWSHAPE, 265, 1, 2, -0.310601,0.000000, -0.158251,0.009739, -0.149626,0.045009, -0.173168,0.066320, -0.250414,0.056912, -0.242122,0.064314, -0.253563,0.065926, -0.241104,0.075740, -0.249897,0.082453, -0.237118,0.081982, -0.246310,0.097762, -0.230366,0.088118, -0.210870,0.082352, -0.194727,0.083100, -0.180298,0.087679, -0.162032,0.087154, -0.135091,0.081542, -0.116083,0.068924, -0.106144,0.066340, -0.084057,0.063643, -0.061108,0.071892, -0.044295,0.077815, -0.021516,0.077698, 0.000000,0.078872, 0.025203,0.099613, 0.040964,0.113252, 0.067885,0.127285, 0.086481,0.135203, 0.104129,0.144556, 0.097579,0.132951, 0.112604,0.134635, 0.098603,0.124167, 0.118341,0.115901, 0.094688,0.115325, 0.079806,0.108826, 0.065011,0.097516, 0.053964,0.082746, 0.049028,0.066966, 0.045353,0.053708, 0.046494,0.040534, 0.051260,0.033378, 0.074561,0.026708, 0.111469,0.022294, 
// shDogHead
NEWSHAPE, 266, 1, 2, 0.063398,0.010010, 0.064517,0.018910, 0.069275,0.029860, 0.077732,0.029897, 0.081360,0.028715, 0.066249,0.039649, 0.106662,0.046229, 0.146020,0.025919, 0.159666,0.028110, 0.172131,0.020186, 0.172077,0.013438, 0.168613,0.007837, 
// shDogTorso
NEWSHAPE, 267, 1, 2, -0.310601,0.000000, -0.158251,0.009739, -0.149626,0.045009, -0.116083,0.068924, -0.106144,0.066340, -0.084057,0.063643, -0.061108,0.071892, -0.044295,0.077815, -0.021516,0.077698, 0.000000,0.078872, 0.036721,0.067879, 0.045353,0.053708, 0.046494,0.040534, 0.051260,0.033378, 0.074561,0.026708, 0.111469,0.022294, 
// shDogFrontPaw
NEWSHAPE, 268, 1, 1, -0.021458,0.071528, 0.056770,0.096454, 0.086625,0.096557, 0.094338,0.086062, 0.091509,0.069458, 0.029143,0.047289, -0.007695,0.037376, -0.024745,0.050039, 
// shDogRearPaw
NEWSHAPE, 269, 1, 1, -0.120244,0.033095, -0.181823,0.053216, -0.199420,0.068325, -0.189994,0.093330, -0.169179,0.099289, -0.138237,0.080730, -0.088688,0.049026, -0.094739,0.030294, -0.102488,0.029755, 
// shDogFrontLeg
NEWSHAPE, 270, 1, 1, 0.050083,0.064552, 0.044492,0.047829, 0.018900,0.044470, 0.002224,0.057826, 0.003338,0.075664, 0.020042,0.089074, 0.034520,0.086856, 0.040087,0.084629, 0.052335,0.076832, 
// shDogRearLeg
NEWSHAPE, 271, 1, 1, -0.147691,0.079440, -0.120617,0.078177, -0.102586,0.061329, -0.100284,0.036771, -0.112610,0.034564, -0.133966,0.033491, -0.153171,0.040249, -0.157810,0.067153, 
// shWolfFrontPaw
NEWSHAPE, 272, 1, 1, 0.044056,0.084808, 0.114167,0.147973, 0.121635,0.170511, 0.126416,0.144158, 0.148897,0.151675, 0.134077,0.125212, 0.157522,0.115923, 0.123842,0.098410, 0.074868,0.045141, 0.056665,0.043462, 0.046213,0.055016, 0.039626,0.070996, 
// shWolfRearPaw
NEWSHAPE, 273, 1, 1, -0.228714,0.033559, -0.341382,0.086496, -0.314606,0.087990, -0.326169,0.109775, -0.303428,0.105707, -0.309485,0.135882, -0.287964,0.110057, -0.198121,0.069332, -0.187804,0.046735, -0.200044,0.035736, -0.213491,0.031344, 
// shWolfFrontLeg
NEWSHAPE, 274, 1, 1, 0.043124,0.086241, 0.073285,0.082608, 0.078919,0.072022, 0.084577,0.049131, 0.058349,0.030045, 0.038327,0.023872, 0.019412,0.043794, 0.027622,0.072180, 
// shWolfRearLeg
NEWSHAPE, 275, 1, 1, -0.209981,0.074314, -0.168422,0.064352, -0.158696,0.031877, -0.173682,0.018683, -0.192122,0.019786, -0.221918,0.028600, -0.240782,0.055614, -0.225734,0.072687, 
// shYetiFoot
NEWSHAPE, 276, 1, 1, -0.099231,0.063552, -0.100435,0.085928, -0.045726,0.113758, -0.046823,0.105908, 0.034547,0.103641, -0.006678,0.083479, 0.042290,0.067886, -0.004448,0.057826, 0.046713,0.042264, -0.033354,0.037802, -0.081272,0.036739, 

// inspired by "Reptiles" by M. C. Escher
// shReptile[0][0]
NEWSHAPE, 277, 1, 1, 0.090249,0.427619, 0.102783,0.400499, 0.097688,0.364242, 0.106494,0.395874, 0.118188,0.369310, 0.102003,0.349554, 0.121343,0.363844, 0.129595,0.344141, 0.050755,0.267910, 0.107545,0.210120, 0.150912,0.140416, 0.255007,0.145856, 0.355391,0.087396, 0.387934,-0.018561, 0.265842,0.036925, 0.166743,0.002095, 0.169740,-0.118507, 0.146633,-0.140697, 0.157984,-0.178391, 0.257462,-0.153233, 0.319288,-0.224253, 0.313778,-0.230037, 0.281806,-0.195398, 0.314350,-0.232830, 0.311017,-0.237877, 0.273849,-0.204157, 0.310236,-0.238328, 0.307024,-0.245388, 0.281006,-0.229176, 0.167306,-0.280766, 0.112967,-0.233534, 0.089715,-0.085063, -0.011303,-0.178784, -0.006646,-0.301030, -0.074700,-0.338239, -0.065767,-0.372645, -0.089284,-0.428388, -0.106500,-0.417504, -0.093817,-0.389881, -0.108182,-0.416394, -0.135701,-0.402089, -0.116982,-0.374103, -0.139952,-0.401941, -0.154418,-0.393114, -0.130692,-0.334127, -0.065484,-0.273054, -0.031231,-0.116890, -0.096759,-0.068310, -0.263995,-0.141107, -0.398301,-0.030205, -0.393023,0.021064, -0.260404,0.062755, -0.134384,0.029318, -0.088701,0.042159, -0.093602,0.150729, -0.164204,0.145885, -0.192717,0.119533, -0.206386,0.125176, -0.192110,0.146432, -0.210532,0.126947, -0.226982,0.141915, -0.205723,0.164601, -0.231744,0.143851, -0.246721,0.154465, -0.207322,0.234994, -0.133376,0.235522, -0.064416,0.257637, -0.025934,0.154919, 0.085583,0.151110, 0.022816,0.207531, 0.001449,0.284211, 0.083710,0.351486,
NEWSHAPE, 278, 1, 1, -0.339796,-0.018091, -0.324230,-0.054118, -0.300845,-0.078303, -0.253695,-0.085627, -0.202022,-0.073590, -0.135132,-0.044392, -0.056765,-0.035787, -0.007858,-0.040002, 0.072987,-0.013217, 0.141559,0.023233, 0.223733,0.073816, 0.295586,0.074474, 0.330901,0.047755, 0.298595,0.086679, 0.251202,0.098051, 0.201763,0.089397, 0.132153,0.064910, 0.068866,0.028372, 0.002275,0.007722, -0.055689,0.000427, -0.161069,0.003444, -0.241590,0.005109, -0.274421,0.003951,
NEWSHAPE, 279, 1, 1, -0.252522,-0.071341, -0.245933,-0.086380, -0.256407,-0.108129, -0.279152,-0.101371, -0.287975,-0.089716, -0.287044,-0.071382, -0.264154,-0.060260,
NEWSHAPE, 280, 1, 1, -0.258740,0.011957, -0.254596,-0.009727, -0.273495,-0.033201, -0.294516,-0.034869, -0.307872,-0.015362, -0.306068,0.008710, -0.288916,0.020706,
// shReptile[1][0]
NEWSHAPE, 281, 1, 1,  0.267913,-0.363961, 0.103423,-0.407902, 0.029926,-0.285688, 0.004562,-0.182498, -0.089218,-0.185966, -0.096986,-0.367985, -0.248327,-0.455994, -0.256139,-0.447327, -0.237682,-0.433175, -0.257547,-0.445407, -0.265780,-0.439947, -0.250475,-0.422667, -0.268312,-0.437951, -0.275164,-0.436243, -0.203450,-0.358525, -0.187185,-0.110088, -0.484119,-0.068248, -0.424486,0.012521, -0.323946,-0.003100, -0.200798,0.033484, -0.244734,0.222076, -0.357719,0.170021, -0.365522,0.192816, -0.333464,0.208694, -0.367623,0.196994, -0.371956,0.213819, -0.339807,0.232001, -0.373008,0.216200, -0.376113,0.227855, -0.287818,0.311052, -0.201927,0.336476, -0.162665,0.301719, -0.123143,0.195801, -0.085354,0.085987, 0.106772,0.103301, 0.235043,0.288663, 0.398887,0.202650, 0.392371,0.193980, 0.362197,0.205391, 0.393392,0.192794, 0.388672,0.184045, 0.354985,0.191178, 0.388962,0.181776, 0.383857,0.173107, 0.273977,0.186438, 0.201147,0.007597, 0.385514,-0.014104, 0.452378,-0.139100, 0.441679,-0.165374, 0.286863,-0.196581, 0.154996,-0.121531, 0.112780,-0.178884, 0.130676,-0.244138, 0.233120,-0.294200, 0.242235,-0.311655, 0.187156,-0.319959, 0.242817,-0.315142, 0.253181,-0.338344, 0.193625,-0.348671, 0.254919,-0.341938,
NEWSHAPE, 282, 1, 1, 0.326447,-0.044290, 0.268772,-0.053597, 0.235899,-0.055845, 0.171515,-0.048557, 0.088801,-0.018246, 0.037774,0.004993, -0.027541,0.016777, -0.088937,0.009667, -0.172537,-0.011240, -0.219668,-0.017765, -0.282928,-0.032376, -0.337206,-0.032470, -0.385877,-0.034804, -0.440028,-0.044474, -0.371779,-0.058999, -0.311902,-0.066325, -0.248815,-0.064253, -0.177727,-0.063011, -0.059856,-0.073519, 0.027306,-0.080984, 0.112403,-0.081788, 0.185321,-0.082903, 0.246089,-0.091503, 0.280118,-0.131876, 0.327222,-0.147933, 0.374578,-0.123044, 0.364318,-0.067249, 
NEWSHAPE, 283, 7, 1, 0.027415,-0.009216, 
NEWSHAPE, 284, 7, 1, 0.023426,0.005270, 
// shReptile[2][0]
NEWSHAPE, 285, 1, 1, 0.264174,-0.393902, 0.220100,-0.344820, 0.196314,-0.232131, 0.044844,-0.186655, 0.015265,-0.205081, -0.003936,-0.333213, -0.136120,-0.408084, -0.149930,-0.405620, -0.177207,-0.243366, -0.085946,-0.199911, -0.087381,-0.147154, -0.302464,-0.169877, -0.302092,-0.297549, -0.327187,-0.293024, -0.330841,-0.249219, -0.329911,-0.292299, -0.353906,-0.290880, -0.354914,-0.247858, -0.357304,-0.291341, -0.377715,-0.287105, -0.387134,-0.104888, -0.106049,-0.041562, -0.095545,0.040016, -0.302051,0.163655, -0.332611,0.319950, -0.310541,0.349402, -0.289857,0.281329, -0.303842,0.352541, -0.278525,0.372081, -0.261882,0.313955, -0.275092,0.376585, -0.247719,0.398573, -0.239799,0.336833, -0.214496,0.215823, -0.168074,0.170579, -0.067435,0.196926, 0.028714,0.266822, 0.015830,0.400766, 0.131125,0.319617, 0.148574,0.162459, 0.283651,0.088527, 0.295994,0.169290, 0.331188,0.179878, 0.329752,0.145196, 0.335770,0.181014, 0.363641,0.191082, 0.365077,0.157473, 0.367064,0.192650, 0.395018,0.204633, 0.405110,0.041551, 0.311991,0.015994, 0.195242,0.016634, 0.122740,-0.085748, 0.261020,-0.098708, 0.286694,-0.182139, 0.332188,-0.307144, 0.306926,-0.340004, 0.279202,-0.289607, 0.302360,-0.343229, 0.284083,-0.365340, 0.252717,-0.314768, 0.279532,-0.371945,
NEWSHAPE, 286, 1, 1, -0.127096,-0.255815, -0.091894,-0.241092, -0.073203,-0.219108, -0.050918,-0.160430, -0.046653,-0.121421, -0.028140,-0.044121, -0.017641,0.026545, 0.022513,0.133022, 0.050663,0.193120, 0.058157,0.270962, 0.054214,0.311270, 0.046360,0.353789, 0.068493,0.316069, 0.093492,0.242457, 0.080840,0.108339, 0.056390,0.006556, 0.031831,-0.062813, -0.001836,-0.143721, -0.034334,-0.239758, -0.041121,-0.297513, -0.071677,-0.333221, -0.108488,-0.343266, -0.135448,-0.293017, 
NEWSHAPE, 287, 7, 1, -0.021896,-0.012396, 
NEWSHAPE, 288, 7, 1, -0.023484,-0.012828, 
// shReptile[3][0]
NEWSHAPE, 289, 1, 1, 0.276554,-0.041210, 0.310457,0.039428, 0.287105,0.072645, 0.169859,-0.000837, 0.151648,-0.089695, 0.191477,-0.149470, 0.280888,-0.183226, 0.341099,-0.338656, 0.273683,-0.366029, 0.209539,-0.273223, 0.105852,-0.172588, -0.061113,-0.279714, -0.013642,-0.342451, -0.058157,-0.375558, -0.090346,-0.316684, -0.061781,-0.378853, -0.101453,-0.409200, -0.128968,-0.341003, -0.106321,-0.412460, -0.147327,-0.447396, -0.196876,-0.316767, -0.070150,-0.182145, -0.133594,-0.036417, -0.294753,-0.151579, -0.409075,-0.148250, -0.444408,-0.105385, -0.374677,-0.113506, -0.443332,-0.098946, -0.434173,-0.078381, -0.367706,-0.088453, -0.432558,-0.074716, -0.428663,-0.053306, -0.293683,-0.069123, -0.141662,0.058415, -0.281560,0.174905, -0.249829,0.284696, -0.226324,0.297042, -0.119112,0.263418, -0.075663,0.144858, -0.021279,0.097533, 0.064463,0.189563, 0.011865,0.335469, -0.005820,0.393456, 0.044789,0.390508, 0.065571,0.321367, 0.047251,0.390455, 0.090068,0.386431, 0.103507,0.317601, 0.093275,0.386149, 0.130199,0.386756, 0.148881,0.163862, 0.089483,0.079942, 0.135587,0.047323, 0.293067,0.168426, 0.408382,0.092603, 0.384168,0.007719, 0.315700,-0.045433, 0.346149,0.015698, 0.313006,-0.045307, 0.295254,-0.041824, 0.326732,0.017229, 0.292941,-0.042281, 
NEWSHAPE, 290, 1, 1, -0.130824,0.101694, -0.057151,-0.007670, 0.115177,-0.160650, 0.211980,-0.236858, 0.296693,-0.311866, 0.232978,-0.219581, 0.138912,-0.105664, -0.002855,0.058899, -0.100445,0.121752, -0.136992,0.183934, -0.144345,0.222791, -0.168225,0.256064, -0.232048,0.242113, -0.236767,0.213008, -0.234529,0.179695, -0.198210,0.146185, -0.167694,0.143175, 
NEWSHAPE, 291, 7, 1, -0.019211,0.011243, 
NEWSHAPE, 292, 7, 1, -0.017430,0.009904, 
// shReptile[4][0]
NEWSHAPE, 293, 3, 1, 0.000458,0.307912, 0.204532,0.422468, 0.260617,0.384080, 0.177355,0.114658, 0.351515,0.031931, 0.411778,0.098785, 0.502998,0.070318, 0.433450,0.021025, 0.502176,0.057577, 0.563080,-0.085829, 0.478445,-0.055947, 0.558143,-0.094808, 0.534739,-0.111613, 0.420992,-0.057813,
NEWSHAPE, 294, 3, 1, -0.169183,-0.225979, -0.187272,-0.078673, -0.281255,0.003711, -0.095795,0.040777, 
NEWSHAPE, 295, 7, 1, -0.019211,0.011243, 
NEWSHAPE, 296, 7, 1, -0.017430,0.009904, 

// shReptileBody
NEWSHAPE, 297, 1, 1, 0.207893,0.052816, 0.154587,0.095216, 0.118624,0.121981, 0.077111,0.131872, 0.011141,0.103613, -0.040061,0.066768, -0.052300,0.056751, -0.041318,0.136239, -0.097277,0.126349, -0.146652,0.094036, -0.214827,0.059612, -0.280943,0.028321, -0.353759,-0.002290, -0.410588,-0.042794, -0.446484,-0.083934, -0.379856,-0.069065, -0.301215,-0.057970, -0.243697,-0.056411, -0.176013,-0.072872, -0.133073,-0.090579, -0.099458,-0.115103, -0.065944,-0.139713, -0.052291,-0.050065, 0.014488,-0.109218, 0.077073,-0.123987, 0.130941,-0.109677, 0.162455,-0.085149, 0.218159,-0.035985,
// shReptileHead
NEWSHAPE, 298, 1, 2, 0.471485,-0.015757, 0.288467,-0.098421, 0.170941,-0.033237, 
// shReptileFrontFoot
NEWSHAPE, 299, 1, 1, 0.250278,0.167235, 0.238951,0.195456, 0.195709,0.200188, 0.240500,0.197580, 0.232498,0.224361, 0.186427,0.225955, 0.227016,0.226262, 0.227409,0.260408, 0.102772,0.258192, 0.080173,0.252350, 0.059570,0.224171, 0.051410,0.202351, 0.074841,0.180175, 0.114709,0.161348, 
// shReptileRearFoot
NEWSHAPE, 300, 1, 1, -0.067876,0.206615, -0.114088,0.181539, -0.272616,0.185109, -0.253628,0.222368, -0.216050,0.219537, -0.249625,0.225089, -0.240714,0.244505, -0.205182,0.244381, -0.232919,0.248821, -0.217457,0.272915, -0.099816,0.264976, -0.065058,0.238612, 
// shReptileFrontLeg
NEWSHAPE, 301, 1, 1, 0.047213,0.212458, 0.077946,0.248522, 0.130365,0.256196, 0.141554,0.241207, 0.148626,0.179026, 0.127519,0.104028, 0.091374,0.056830, 0.045625,0.063430, 0.028944,0.082379, 0.036887,0.150900,
// shReptileRearLeg
NEWSHAPE, 302, 1, 1, -0.098128,0.262832, -0.068071,0.236831, -0.053685,0.166798, -0.046188,0.128123, -0.056603,0.055942, -0.108486,0.050024, -0.124961,0.087637, -0.134873,0.130635, -0.145563,0.191132, -0.140577,0.221299, -0.120741,0.249057,
// shReptileTail
NEWSHAPE, 303, 1, 1, -0.419630,-0.067234, -0.351432,-0.034342, -0.278537,-0.003397, -0.200906,0.024692, -0.083503,0.034515, 0.041138,0.030019, 0.138446,0.017864, 0.197445,0.006731, 0.094656,-0.005568, 0.007778,0.003333, -0.085713,-0.003339, -0.197452,-0.013463, -0.280927,-0.023788,
// shReptileEye
NEWSHAPE, 304, 1, 1, 0.288102,0.032160, 0.268177,0.043655, 0.265884,0.058115, 0.276356,0.070881, 0.290293,0.073602, 0.310905,0.064370, 0.308359,0.045081, 

// shDodeca,
NEWSHAPE, 305, 5, 2, 0.123140,0.087570, 0.151044,0.006085, 0.141348,0.003141, 0.091602,0.003971, 0.064937,0.045676, 0.057217,0.044013, 0.086006,0.002395, 0.086006,-0.002395,
// shBugLeg
NEWSHAPE, 306, 1, 1, -0.188132,0.071590, -0.106107,0.109975, -0.020337,0.032429, -0.010997,-0.052235, 0.027591,-0.128573, -0.009955,-0.162603, -0.123175,-0.156466, -0.129658,-0.186975, -0.003881,-0.192409, 0.048636,-0.145908, 0.045758,-0.106953, 0.012643,-0.040678, 0.031326,0.003847, 0.131788,0.118499, 0.134832,0.147039, 0.121802,0.186319, 0.018400,0.244215, 0.011672,0.216213, 0.079288,0.174654, 0.095323,0.160720, 0.088932,0.115998, 0.010444,0.037927, -0.102392,0.137814, 
// shBugAntenna
NEWSHAPE, 307, 1, 2, -0.037388,-0.035738, 0.121931,-0.041931, 0.148538,-0.114729, 0.171226,-0.135647, 0.193388,-0.143787, 0.206926,-0.136091, 0.172174,-0.118855, 0.163075,-0.108717, 0.147190,-0.083002, 0.129680,-0.024281, 

// shTrylobiteBody
NEWSHAPE, 308, 1, 2, 0.196119,0.032415, 0.090349,0.051694, 0.014858,0.048341, -0.084517,0.042642, -0.099309,0.031132, -0.146522,0.055533, -0.190296,0.065896, -0.234005,0.061772, -0.245550,0.053775, -0.212120,0.048191, -0.163612,0.030837, -0.143014,0.007540, 
// shTrylobiteFrontClaw
NEWSHAPE, 309, 1, 1, 0.162924,0.164217, 0.134529,0.219357, 0.096861,0.266620, 0.040792,0.297621, 0.018375,0.280440, 0.073270,0.189478, 0.089713,0.132980, 0.111928,0.119234, 0.144100,0.124454, 
// shTrylobiteRearClaw
NEWSHAPE, 310, 1, 1, -0.014785,0.165073, -0.030220,0.198964, -0.060610,0.240281, -0.086102,0.254842, -0.112355,0.254248, -0.091511,0.215066, -0.070034,0.152230, -0.057129,0.134268, -0.041113,0.127236, -0.022776,0.135081, 
// shTrylobiteFrontLeg
NEWSHAPE, 311, 1, 1, 0.196620,0.034345, 0.161944,0.165955, 0.131681,0.185106, 0.105471,0.183927, 0.090859,0.133459, 0.094842,0.063710, 0.082033,0.026880, 0.114740,0.000635, 0.168794,0.000330, 
// shTrylobiteRearLeg
NEWSHAPE, 312, 1, 1, 0.014403,0.049247, -0.005676,0.101370, -0.014283,0.164470, -0.036662,0.174292, -0.054954,0.173745, -0.070446,0.153946, -0.074749,0.083574, -0.082124,0.043654, -0.052716,0.016562, -0.009043,0.013419, 
// shLeafFloor,
NEWSHAPE, 313, 3, 1, -0.124709,0.213831, -0.127071,0.274094, -0.099723,0.369490, -0.023215,0.462261, 0.072785,0.531016, 0.152171,0.570046, 0.244072,0.583021, 0.278329,0.581281, 0.289459,0.567347, 0.241111,0.530198, 0.189086,0.449607, 0.153336,0.354939, 0.133803,0.281218, 0.127619,0.205626, 0.145868,0.133361, 0.140560,0.107526, 0.106341,0.086253, 0.061487,0.046396, 0.029920,0.018377, 0.013295,0.007733, 0.050569,0.013990, 0.166612,0.102510, 
// shLeafFloor,
NEWSHAPE, 314, 1, 1, 0, 0,

// shBullBody
NEWSHAPE, 315, 1, 2, -0.399002,0.000618, -0.412389,-0.011435, -0.383217,-0.004139, -0.396241,-0.020642, -0.365704,-0.007876, -0.335517,-0.003463, -0.306649,-0.005631, -0.286501,-0.014485, -0.270113,-0.034709, -0.263139,-0.079331, -0.257114,-0.110215, -0.247630,-0.125790, -0.231094,-0.131733, -0.215499,-0.134849, -0.198771,-0.130780, -0.184961,-0.120425, -0.176890,-0.110874, -0.170135,-0.107553, -0.128764,-0.126896, -0.138805,-0.126406, -0.089067,-0.151289, -0.099830,-0.154113, -0.066698,-0.158059, -0.043468,-0.159303, -0.027802,-0.156801, -0.033201,-0.163796, -0.009201,-0.151129, 0.003865,-0.145169, 0.002259,-0.154029, 0.022833,-0.142371, 0.035183,-0.139200, 0.057625,-0.133866, 0.085702,-0.138411, 0.106606,-0.143670, 0.123715,-0.148158, 0.139656,-0.143049, 0.152591,-0.131020, 0.170000,-0.108676, 0.194767,-0.076789, 0.218502,-0.057643, 
// shBullHorn
NEWSHAPE, 316, 1, 1, 0.321702,-0.078714, 0.329603,-0.122456, 0.342058,-0.140037, 0.360561,-0.149754, 0.379070,-0.152553, 0.395131,-0.150636, 0.423718,-0.142016, 0.448897,-0.133266, 0.441750,-0.150756, 0.426899,-0.163295, 0.403192,-0.169643, 0.371044,-0.173385, 0.345193,-0.167994, 0.313418,-0.152134, 0.303510,-0.135781, 0.297242,-0.118441, 0.292244,-0.102342, 0.308799,-0.096856, 
// shBullRearHoof
NEWSHAPE, 317, 1, 1, -0.218107,-0.148042, -0.243390,-0.132449, -0.246496,-0.105156, -0.238188,-0.090308, -0.226562,-0.085665, -0.207006,-0.088878, -0.189916,-0.100015, -0.189059,-0.127164, -0.196081,-0.138609, 
// shBullFrontHoof
NEWSHAPE, 318, 1, 1, 0.110547,-0.088215, 0.135310,-0.087225, 0.144428,-0.099644, 0.144636,-0.125576, 0.135691,-0.137934, 0.121038,-0.142332, 0.104105,-0.138807, 0.088308,-0.128550, 0.088231,-0.113919, 0.096026,-0.102725,
// shBullHead
NEWSHAPE, 319, 1, 2, 0.334622,-0.050250, 0.307607,-0.096839, 0.281739,-0.111332, 0.258180,-0.106443, 0.236906,-0.077841, 0.214842,-0.061865, 0.184962,-0.040355, 0.198626,-0.028054, 0.195166,-0.013460, 0.181470,-0.002240, 

// shButterflyBody
NEWSHAPE, 320, 1, 2, 0.176732,-0.004396, 0.195200,-0.015935, 0.201979,-0.028577, 0.202087,-0.043427, 0.198784,-0.050031, 0.198704,-0.041226, 0.196945,-0.030227, 0.190734,-0.019233, 0.170044,-0.008793, 0.163946,-0.020339, 0.151703,-0.020894, 0.140009,-0.012649, 0.122852,-0.026418, 0.108465,-0.031392, 0.017314,-0.028016, 0.000470,-0.019079, -0.088765,-0.013614, -0.095608,-0.005678, 
// shButterflyWing
NEWSHAPE, 321, 1, 2, 0.101344,-0.004988, 0.178342,-0.088519, 0.202439,-0.133112, 0.222376,-0.199882, 0.236304,-0.245909, 0.228546,-0.251735, 0.203290,-0.253007, 0.178432,-0.236979, 0.151432,-0.218854, 0.124491,-0.196352, 0.092144,-0.173026, 0.066111,-0.165612, 0.055720,-0.012369, 0.055445,-0.104987, 0.048098,-0.123101, 0.031857,-0.142619, 0.017719,-0.157675, -0.015575,-0.171093, -0.037946,-0.176348, -0.059503,-0.171340, -0.086100,-0.144824, -0.102753,-0.105546, -0.111188,-0.079719, -0.110326,-0.064989, -0.025725,-0.012627, -0.023585,-0.006998, 

// shGadflyBody
NEWSHAPE, 322, 1, 2, 0.170044,-0.008793, 0.163946,-0.020339, 0.151703,-0.020894, 0.140009,-0.012649, 0.122852,-0.026418, 0.108465,-0.031392, 0.017314,-0.028016, 0.000470,-0.019079, 
// shGadflyWing
NEWSHAPE, 323, 1, 2, 0.130571,-0.024552, -0.001114,-0.108092, -0.042319,-0.085751, -0.047826,-0.042265, 0.091301,-0.002227, 0.124928,-0.003346, 
// shGadflyEye
NEWSHAPE, 324, 1, 1, 0.168968,-0.004476, 0.175788,-0.012316, 0.172390,-0.020150, 0.165580,-0.020138, 0.161038,-0.013420, 0.161033,-0.007828, 

// inspired by "Seven Butterflies Pattern" by Doug Dunham (in turn inspired by M. C. Escher)
// http://www.bridgesmathart.org/art-exhibits/bridges2001/dunham1/index.html
// shButterflyFloor[0]
NEWSHAPE, 325, 3, 1, 0.003906,-0.017741, -0.005665,-0.023874, -0.001296,-0.059255, 0.008110,-0.088485, 0.021638,-0.119244, 0.001998,-0.123781, -0.025603,-0.125790, -0.042514,-0.132101, -0.062642,-0.134604, -0.080079,-0.114436, -0.119123,-0.090010, -0.141865,-0.078372, -0.171380,-0.068573, -0.219062,-0.058323, -0.143742,-0.087861, -0.091657,-0.119016, -0.064293,-0.149293, -0.035036,-0.174552, 0.008068,-0.188158, 0.033975,-0.202674, 0.014209,-0.230302, -0.012607,-0.273132, -0.029424,-0.294809, -0.225216,-0.161997, -0.242761,-0.128783, -0.243022,-0.100285, -0.242528,-0.076469, -0.253522,-0.113827, -0.276566,-0.145725, -0.242545,-0.170535, -0.250219,-0.181472, -0.286326,-0.161348, -0.293850,-0.153836, -0.307260,-0.144896, -0.340292,-0.119818, -0.408251,-0.088506, -0.397993,-0.073727, -0.374925,-0.086159, -0.356519,-0.086714, -0.343301,-0.057086, -0.319019,-0.041256, -0.278705,-0.010797, -0.342214,0.129479, -0.259524,0.175390, -0.228995,0.117575, -0.204395,0.063454, -0.257242,0.022435, -0.263564,0.003431, -0.193668,0.062052, -0.104991,0.119589, -0.107623,0.096041, -0.111440,0.076555, -0.126283,0.044518, -0.143519,0.037706, -0.175989,0.003239, -0.213542,-0.048387, -0.188075,-0.038009, -0.156904,-0.002863, -0.139409,0.021202, -0.129500,0.034230, 
// shButterflyFloor[1]
NEWSHAPE, 326, 7, 1, -0.199281,-0.117040, -0.202870,-0.110023, -0.247957,-0.128116, -0.298501,0.006170, -0.226086,0.045756, -0.061553,0.006677, -0.059070,0.020733, -0.217691,0.072727, 

NEWSHAPE
};

