// Hyperbolic Rogue
// This file contains hyperbolic points and matrices.
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

eGeometry geometry;
eVariation variation;

// hyperbolic points and matrices

// basic functions and types
//===========================

#ifdef SINHCOSH
// ld sinh(ld alpha) { return (exp(alpha) - exp(-alpha)) / 2; }
// ld cosh(ld alpha) { return (exp(alpha) + exp(-alpha)) / 2; }

/* ld inverse_sinh(ld z) {
  return log(z+sqrt(1+z*z));
  }
 
double inverse_cos(double c) {
  double s = sqrt(1-c*c);
  double r = atan(s/c);
  if(r < 0) r = -r;
  return r;
  }

// ld tanh(ld x) { return sinh(x) / cosh(x); }
ld inverse_tanh(ld x) { return log((1+x)/(1-x)) / 2; } */

#endif
#ifndef M_PI
#define M_PI 3.14159265358979
#endif

ld squar(ld x) { return x*x; }

int sig(int z) { return (sphere || z<2)?1:-1; }

int curvature() {
  switch(cgclass) {
    case gcEuclid: return 0;
    case gcHyperbolic: return -1;
    case gcSphere: return 1;
    default: return 0;
    }
  }

ld sin_auto(ld x) {
  switch(cgclass) {
    case gcEuclid: return x;
    case gcHyperbolic: return sinh(x);
    case gcSphere: return sin(x);
    default: return x;
    }
  }

ld asin_auto(ld x) {
  switch(cgclass) {
    case gcEuclid: return x;
    case gcHyperbolic: return asinh(x);
    case gcSphere: return asin(x);
    default: return x;
    }
  }

ld asin_auto_clamp(ld x) {
  switch(cgclass) {
    case gcEuclid: return x;
    case gcHyperbolic: return asinh(x);
    case gcSphere: return x>1 ? M_PI/2 : x<-1 ? -M_PI/2 : std::isnan(x) ? 0 : asin(x);
    default: return x;
    }
  }

ld cos_auto(ld x) {
  switch(cgclass) {
    case gcEuclid: return 1;
    case gcHyperbolic: return cosh(x);
    case gcSphere: return cos(x);
    default: return 1;
    }
  }

ld tan_auto(ld x) {
  switch(cgclass) {
    case gcEuclid: return x;
    case gcHyperbolic: return tanh(x);
    case gcSphere: return tan(x);
    default: return 1;
    }
  }

ld atan_auto(ld x) {
  switch(cgclass) {
    case gcEuclid: return x;
    case gcHyperbolic: return atanh(x);
    case gcSphere: return atan(x);
    default: return 1;
    }
  }

ld atan2_auto(ld y, ld x) {
  switch(cgclass) {
    case gcEuclid: return y/x;
    case gcHyperbolic: return atanh(y/x);
    case gcSphere: return atan2(y, x);
    default: return 1;
    }
  }

// hyperbolic point:
//===================

// we represent the points on the hyperbolic plane
// by points in 3D space (Minkowski space) such that x^2+y^2-z^2 == -1, z > 0
// (this is analogous to representing a sphere with points such that x^2+y^2+z^2 == 1)

hyperpoint hpxy(ld x, ld y) { 
  return hpxyz(x,y, euclid ? 1 : sphere ? sqrt(1-x*x-y*y) : sqrt(1+x*x+y*y));
  }

// center of the pseudosphere
const hyperpoint Hypc(0,0,0);

// origin of the hyperbolic plane
const hyperpoint C0(0,0,1);

// a point (I hope this number needs no comments ;) )
const hyperpoint Cx1(1,0,1.41421356237);

// this function returns approximate square of distance between two points
// (in the spherical analogy, this would be the distance in the 3D space,
// through the interior, not on the surface)
// also used to verify whether a point h1 is on the hyperbolic plane by using Hypc for h2

bool zero2(hyperpoint h) { return h[0] == 0 && h[1] == 0; }

bool zero3(hyperpoint h) { return h[0] == 0 && h[1] == 0 && h[2] == 0; }

ld intval(const hyperpoint &h1, const hyperpoint &h2) {
  if(elliptic) {
    double d1 = squar(h1[0]-h2[0]) + squar(h1[1]-h2[1]) + squar(h1[2]-h2[2]);
    double d2 = squar(h1[0]+h2[0]) + squar(h1[1]+h2[1]) + squar(h1[2]+h2[2]);
    return min(d1, d2);
    }
  return squar(h1[0]-h2[0]) + squar(h1[1]-h2[1]) + (sphere?1:euclid?0:-1) * squar(h1[2]-h2[2]);
  }

ld intvalxy(const hyperpoint &h1, const hyperpoint &h2) {
  return squar(h1[0]-h2[0]) + squar(h1[1]-h2[1]);
  }

ld intvalxyz(const hyperpoint &h1, const hyperpoint &h2) {
  return squar(h1[0]-h2[0]) + squar(h1[1]-h2[1]) + squar(h1[2]-h2[2]);
  }

ld hypot2(const hyperpoint& h) {
  return sqrt(h[0]*h[0]+h[1]*h[1]);
  }
  
ld hypot3(const hyperpoint& h) {
  return sqrt(h[0]*h[0]+h[1]*h[1]+h[2]*h[2]);
  }
  
ld sqhypot2(const hyperpoint& h) {
  return h[0]*h[0]+h[1]*h[1];
  }
  
ld sqhypot3(const hyperpoint& h) {
  return h[0]*h[0]+h[1]*h[1]+h[2]*h[2];
  }
  
ld zlevel(const hyperpoint &h) {
  if(euclid) return h[2];
  else if(sphere) return sqrt(intval(h, Hypc));
  else return sqrt(-intval(h, Hypc));
  }

// display a hyperbolic point
char *display(const hyperpoint& H) { 
  static char buf[100];
  sprintf(buf, "%8.4f:%8.4f:%8.4f", double(H[0]), double(H[1]), double(H[2]));
  return buf;
  }

ld hypot_auto(ld x, ld y) {
  switch(cgclass) {
    case gcEuclid:
      return hypot(x, y);
    case gcHyperbolic:
      return acosh(cosh(x) * cosh(y));
    case gcSphere:
      return acos(cos(x) * cos(y));
    default:
      return hypot(x, y);
    }
  }

// move H back to the sphere/hyperboloid/plane
hyperpoint normalize(hyperpoint H) {
  ld Z = zlevel(H);
  for(int c=0; c<3; c++) H[c] /= Z;
  return H;
  }

// get the center of the line segment from H1 to H2
hyperpoint mid(const hyperpoint& H1, const hyperpoint& H2) {
  using namespace hyperpoint_vec;
  return normalize(H1 + H2);
  }

// like mid, but take 3D into account
hyperpoint midz(const hyperpoint& H1, const hyperpoint& H2) {
  using namespace hyperpoint_vec;
  hyperpoint H3 = H1 + H2;
  
  ld Z = 2;
  
  if(!euclid) Z = zlevel(H3) * 2 / (zlevel(H1) + zlevel(H2));
  for(int c=0; c<3; c++) H3[c] /= Z;
  
  return H3;
  }

// matrices
//==========

// matrices represent isometries of the hyperbolic plane
// (just like isometries of the sphere are represented by rotation matrices)

// identity matrix
const transmatrix Id = {{{1,0,0}, {0,1,0}, {0,0,1}}};

// mirror image
const transmatrix Mirror = {{{1,0,0}, {0,-1,0}, {0,0,1}}};

// mirror image
const transmatrix MirrorX = {{{-1,0,0}, {0,1,0}, {0,0,1}}};

// mirror image
const transmatrix MirrorZ = {{{1,0,0}, {0,1,0}, {0,0,-1}}};

// rotate by PI
const transmatrix pispin = {{{-1,0,0}, {0,-1,0}, {0,0,1}}};

// central symmetry
const transmatrix centralsym = {{{-1,0,0}, {0,-1,0}, {0,0,-1}}};

// rotate by alpha degrees
transmatrix spin(ld alpha) {
  transmatrix T = Id;
  T[0][0] = +cos(alpha); T[0][1] = +sin(alpha);
  T[1][0] = -sin(alpha); T[1][1] = +cos(alpha);
  T[2][2] = 1;
  return T;
  }

transmatrix eupush(ld x, ld y) {
  transmatrix T = Id;
  T[0][2] = x;
  T[1][2] = y;
  return T;
  }

transmatrix eupush(hyperpoint h) {
  transmatrix T = Id;
  T[0][2] = h[0];
  T[1][2] = h[1];
  return T;
  }

transmatrix euscalezoom(hyperpoint h) {
  transmatrix T = Id;
  T[0][0] = h[0];
  T[0][1] = -h[1];
  T[1][0] = h[1];
  T[1][1] = h[0];
  return T;
  }

transmatrix euaffine(hyperpoint h) {
  transmatrix T = Id;
  T[0][1] = h[0];
  T[1][1] = exp(h[1]);
  return T;
  }

// push alpha units to the right
transmatrix xpush(ld alpha) {
  if(euclid) return eupush(alpha, 0);
  transmatrix T = Id;
  if(sphere) {
    T[0][0] = +cos(alpha); T[0][2] = +sin(alpha);
    T[2][0] = -sin(alpha); T[2][2] = +cos(alpha);
    }
  else {
    T[0][0] = +cosh(alpha); T[0][2] = +sinh(alpha);
    T[2][0] = +sinh(alpha); T[2][2] = +cosh(alpha);
    }
  return T;
  }

inline hyperpoint xpush0(ld x) { 
  hyperpoint h;
  if(euclid) return hpxy(x, 0);
  else if(sphere) h[0] = sin(x), h[1] = 0, h[2] = cos(x);
  else h[0] = sinh(x), h[1] = 0, h[2] = cosh(x);
  return h;
  }

inline hyperpoint xspinpush0(ld alpha, ld x) { 
  // return spin(alpha)*xpush0(x);
  ld s;
  hyperpoint h;
  if(euclid) return hpxy(x*cos(alpha), -x*sin(alpha));
  else if(sphere) s=sin(x), h[0] = s*cos(alpha), h[1] = -s*sin(alpha), h[2] = cos(x);
  else s=sinh(x), h[0] = s*cos(alpha), h[1] = -s*sin(alpha), h[2] = cosh(x);
  return h;
  }

// push alpha units vertically
transmatrix ypush(ld alpha) {
  if(euclid) return eupush(0, alpha);
  transmatrix T = Id;
  if(sphere) {
    T[1][1] = +cos(alpha); T[1][2] = +sin(alpha);
    T[2][1] = -sin(alpha); T[2][2] = +cos(alpha);
    }
  else {
    T[1][1] = +cosh(alpha); T[1][2] = +sinh(alpha);
    T[2][1] = +sinh(alpha); T[2][2] = +cosh(alpha);
    }
  return T;
  }

// rotate the hyperbolic plane around C0 such that H[1] == 0 and H[0] >= 0
transmatrix spintox(const hyperpoint& H) {
  transmatrix T = Id;
  ld R = sqrt(H[0] * H[0] + H[1] * H[1]);
  if(R >= 1e-12) {
    T[0][0] = +H[0]/R; T[0][1] = +H[1]/R;
    T[1][0] = -H[1]/R; T[1][1] = +H[0]/R;
    }
  return T;
  }

void set_column(transmatrix& T, int i, const hyperpoint& H) {
  for(int j=0; j<3; j++)
    T[j][i] = H[j];
  }

transmatrix build_matrix(hyperpoint h1, hyperpoint h2, hyperpoint h3) {
  transmatrix T;
  for(int i=0; i<3; i++)
    T[i][0] = h1[i],
    T[i][1] = h2[i],
    T[i][2] = h3[i];
  return T;
  }

// reverse of spintox(H)
transmatrix rspintox(const hyperpoint& H) {
  transmatrix T = Id;
  ld R = sqrt(H[0] * H[0] + H[1] * H[1]);
  if(R >= 1e-12) {
    T[0][0] = +H[0]/R; T[0][1] = -H[1]/R;
    T[1][0] = +H[1]/R; T[1][1] = +H[0]/R;
    }
  return T;
  }

// for H such that H[1] == 0, this matrix pushes H to C0
transmatrix pushxto0(const hyperpoint& H) {
  if(euclid) return eupush(-H[0], -H[1]);
  transmatrix T = Id;
  if(sphere) {
    T[0][0] = +H[2]; T[0][2] = -H[0];
    T[2][0] = +H[0]; T[2][2] = +H[2];
    }
  else {
    T[0][0] = +H[2]; T[0][2] = -H[0];
    T[2][0] = -H[0]; T[2][2] = +H[2];
    }
  return T;
  }

// reverse of pushxto0(H)
transmatrix rpushxto0(const hyperpoint& H) {
  if(euclid) return eupush(H[0], H[1]);
  transmatrix T = Id;
  if(sphere) {
    T[0][0] = +H[2]; T[0][2] = +H[0];
    T[2][0] = -H[0]; T[2][2] = +H[2];
    }
  else {
    T[0][0] = +H[2]; T[0][2] = +H[0];
    T[2][0] = +H[0]; T[2][2] = +H[2];
    }
  return T;
  }

// generalization: H[1] can be non-zero
transmatrix gpushxto0(const hyperpoint& H) {
  if(euclid) return eupush(-H[0], -H[1]);
  hyperpoint H2 = spintox(H) * H;
  return rspintox(H) * pushxto0(H2) * spintox(H);
  }

transmatrix rgpushxto0(const hyperpoint& H) {
  if(euclid) return eupush(H[0], H[1]);
  hyperpoint H2 = spintox(H) * H;
  return rspintox(H) * rpushxto0(H2) * spintox(H);
  }


// fix the matrix T so that it is indeed an isometry
// (without using this, imprecision could accumulate)

void fixmatrix(transmatrix& T) {
  if(euclid) {
    for(int x=0; x<2; x++) for(int y=0; y<=x; y++) {
      ld dp = 0;
      for(int z=0; z<2; z++) dp += T[z][x] * T[z][y];
      
      if(y == x) dp = 1 - sqrt(1/dp);
      
      for(int z=0; z<2; z++) T[z][x] -= dp * T[z][y];
      }
    for(int x=0; x<2; x++) T[2][x] = 0;
    T[2][2] = 1;
    }
  else for(int x=0; x<3; x++) for(int y=0; y<=x; y++) {
    ld dp = 0;
    for(int z=0; z<3; z++) dp += T[z][x] * T[z][y] * sig(z);
    
    if(y == x) dp = 1 - sqrt(sig(x)/dp);
    
    for(int z=0; z<3; z++) T[z][x] -= dp * T[z][y];
    }
  }

// show the matrix on screen

void display(const transmatrix& T) {
  for(int y=0; y<3; y++) {
    for(int x=0; x<3; x++) printf("%10.7f", double(T[y][x]));
    printf(" -> %10.7f\n", double(squar(T[y][0]) + squar(T[y][1]) + sig(2) * squar(T[y][2])));
    // printf("\n");
    }
  
  for(int x=0; x<3; x++) printf("%10.7f", double(squar(T[0][x]) + squar(T[1][x]) + sig(2) * squar(T[2][x])));
  printf("\n");
  
  for(int x=0; x<3; x++) {
    int y = (x+1) % 3;
    printf("%10.7f", double(T[0][x]*T[0][y] + T[1][x]*T[1][y] + sig(2) * T[2][x]*T[2][y]));
    }
  printf("\n\n");
  }

ld det(const transmatrix& T) {
  ld det = 0;
  for(int i=0; i<3; i++) 
    det += T[0][i] * T[1][(i+1)%3] * T[2][(i+2)%3];
  for(int i=0; i<3; i++) 
    det -= T[0][i] * T[1][(i+2)%3] * T[2][(i+1)%3];
  return det;
  }

void inverse_error(const transmatrix& T) {
  printf("Warning: inverting a singular matrix\n");
  display(T);
  }

transmatrix inverse(const transmatrix& T) {
  profile_start(7);
  
  ld d = det(T);
  transmatrix T2;
  if(d == 0) {
    inverse_error(T); 
    return Id;
    }
  
  for(int i=0; i<3; i++) 
  for(int j=0; j<3; j++) 
    T2[j][i] = (T[(i+1)%3][(j+1)%3] * T[(i+2)%3][(j+2)%3] - T[(i+1)%3][(j+2)%3] * T[(i+2)%3][(j+1)%3]) / d;

  profile_stop(7);
  return T2;
  }

// distance between mh and 0
double hdist0(const hyperpoint& mh) {
  switch(cgclass) {
    case gcHyperbolic:
      if(mh[2] < 1) return 0;
      return acosh(mh[2]);
    case gcEuclid: {
      ld d = sqrt(mh[0]*mh[0]+mh[1]*mh[1]);
      return d;
      }
    case gcSphere: {
      ld res = mh[2] >= 1 ? 0 : mh[2] <= -1 ? M_PI : acos(mh[2]);
      if(elliptic && res > M_PI/2) res = M_PI-res;
      return res;
      }
    default:
      return 0;
    }
  }

ld circlelength(ld r) {
  switch(cgclass) {
    case gcEuclid:
      return 2 * M_PI * r;
    case gcHyperbolic:
      return 2 * M_PI * sinh(r);
    case gcSphere:
      return 2 * M_PI * sin(r);
    default:
      return 0;
    }
  }

// distance between two points
double hdist(const hyperpoint& h1, const hyperpoint& h2) {
  return hdist0(gpushxto0(h1) * h2);
  ld iv = intval(h1, h2);
  switch(cgclass) {
    case gcEuclid:
      return sqrt(iv);
    case gcHyperbolic:
      return 2 * asinh(sqrt(iv) / 2);
    case gcSphere:
      return 2 * asin_auto_clamp(sqrt(iv) / 2);
    default:
      return 0;
    }
  }

hyperpoint mscale(const hyperpoint& t, double fac) {
  hyperpoint res;
  for(int i=0; i<3; i++) 
    res[i] = t[i] * fac;
  return res;
  }

transmatrix mscale(const transmatrix& t, double fac) {
  transmatrix res;
  for(int i=0; i<3; i++) for(int j=0; j<3; j++)
    res[i][j] = t[i][j] * fac;
  return res;
  }

transmatrix xyscale(const transmatrix& t, double fac) {
  transmatrix res;
  for(int i=0; i<3; i++) for(int j=0; j<2; j++)
    res[i][j] = t[i][j] * fac;
  return res;
  }

transmatrix xyzscale(const transmatrix& t, double fac, double facz) {
  transmatrix res;
  for(int i=0; i<3; i++) for(int j=0; j<2; j++)
    res[i][j] = t[i][j] * fac;
  for(int i=0; i<3; i++) 
    res[i][2] = t[i][2] * facz;
  return res;
  }

// double downspin_zivory;

transmatrix mzscale(const transmatrix& t, double fac) {
  // take only the spin
  transmatrix tcentered = gpushxto0(tC0(t)) * t;
  // tcentered = tcentered * spin(downspin_zivory);
  fac -= 1;
  transmatrix res = t * inverse(tcentered) * ypush(-fac) * tcentered;
  fac *= .2;
  fac += 1;
  for(int i=0; i<3; i++) for(int j=0; j<3; j++)
    res[i][j] = res[i][j] * fac;
  return res;
  }

transmatrix pushone() { return euclid ? eupush(1, 0) : xpush(sphere?.5 : 1); }

bool operator == (hyperpoint h1, hyperpoint h2) {
  return h1[0] == h2[0] && h1[1] == h2[1] && h1[2] == h2[2];
  }

// rotation matrix in R^3

transmatrix rotmatrix(double rotation, int c0, int c1) {
  transmatrix t = Id;
  t[c0][c0] = cos(rotation);
  t[c1][c1] = cos(rotation);
  t[c0][c1] = sin(rotation);
  t[c1][c0] = -sin(rotation);
  return t;
  }

hyperpoint mid3(hyperpoint h1, hyperpoint h2, hyperpoint h3) {
  using namespace hyperpoint_vec;
  return mid(h1+h2+h3, h1+h2+h3);
  }

hyperpoint mid_at(hyperpoint h1, hyperpoint h2, ld v) {
  using namespace hyperpoint_vec;
  hyperpoint h = h1 * (1-v) + h2 * v;
  return mid(h, h);
  }  

hyperpoint mid_at_actual(hyperpoint h, ld v) {
  using namespace hyperpoint_vec;
  return rspintox(h) * xpush0(hdist0(h) * v);
  }

}
