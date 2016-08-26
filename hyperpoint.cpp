// Hyperbolic Rogue
// Copyright (C) 2011-2012 Zeno Rogue, see 'hyper.cpp' for details

// basic utility functions

#ifdef MOBILE
typedef double ld;
#else
typedef long double ld;
#endif

template<class T> int size(const T& x) {return int(x.size()); }
string its(int i) { char buf[64]; sprintf(buf, "%d", i); return buf; }
string fts(float x) { char buf[64]; sprintf(buf, "%4.2f", x); return buf; }
string fts4(float x) { char buf[64]; sprintf(buf, "%6.4f", x); return buf; }
string cts(char c) { char buf[8]; buf[0] = c; buf[1] = 0; return buf; }
string llts(long long i) {
    // sprintf does not work on Windows IIRC
    if(i < 0) return "-" + llts(-i);
    if(i < 10) return its((int) i);
    return llts(i/10) + its(i%10);
}
string itsh(int i) {static char buf[16]; sprintf(buf, "%03X", i); return buf; }

// for the Euclidean mode...
bool euclid = false;

// for the pure heptagonal grid
bool purehepta = false;

// hyperbolic points and matrices

// basic functions and types
//===========================

#ifdef SINHCOSH
ld sinh(ld alpha) { return (exp(alpha) - exp(-alpha)) / 2; }
ld cosh(ld alpha) { return (exp(alpha) + exp(-alpha)) / 2; }
#endif

ld squar(ld x) { return x*x; }

int sig(int z) { return z<2?1:-1; }

// hyperbolic point:
//===================

// we represent the points on the hyperbolic plane
// by points in 3D space (Minkowski space) such that x^2+y^2-z^2 == -1, z > 0
// (this is analogous to representing a sphere with points such that x^2+y^2+z^2 == 1)

struct hyperpoint {
  ld tab[3];
  ld& operator [] (int i) { return tab[i]; }
  const ld& operator [] (int i) const { return tab[i]; }
  };

hyperpoint hpxyz(ld x, ld y, ld z) { 
  // EUCLIDEAN
  hyperpoint r; r[0] = x; r[1] = y; r[2] = z; return r; 
  }

hyperpoint hpxy(ld x, ld y) { 
  // EUCLIDEAN
  return hpxyz(x,y, euclid ? 1 : sqrt(1+x*x+y*y));
  }

// center of the pseudosphere
hyperpoint Hypc = { {0,0,0} };

// origin of the hyperbolic plane
hyperpoint C0 = { {0,0,1} };

// a point (I hope this number needs no comments ;) )
hyperpoint Cx1 = { {1,0,1.41421356237} };

// this function returns approximate square of distance between two points
// (in the spherical analogy, this would be the distance in the 3D space,
// through the interior, not on the surface)
// also used to verify whether a point h1 is on the hyperbolic plane by using Hypc for h2

ld intval(const hyperpoint &h1, const hyperpoint &h2) {
  return squar(h1[0]-h2[0]) + squar(h1[1]-h2[1]) - squar(h1[2]-h2[2]);
  }

// display a hyperbolic point
char *display(const hyperpoint& H) { 
  static char buf[100];
  sprintf(buf, "%8.4f:%8.4f:%8.4f", double(H[0]), double(H[1]), double(H[2]));
  return buf;
  }

// get the center of the line segment from H1 to H2
hyperpoint mid(const hyperpoint& H1, const hyperpoint& H2) {

  hyperpoint H3;
  H3[0] = H1[0] + H2[0];
  H3[1] = H1[1] + H2[1];
  H3[2] = H1[2] + H2[2];
  
  ld Z = 2;
  
  if(!euclid) { 
    Z = intval(H3, Hypc); 
    Z = sqrt(-Z);
    }
  
  for(int c=0; c<3; c++) H3[c] /= Z;
  
  return H3;
  }

hyperpoint mid3(const hyperpoint& H1, const hyperpoint& H2, const hyperpoint& H3) {

  hyperpoint Hx;
  Hx[0] = H1[0] + H2[0] + H3[0];
  Hx[1] = H1[1] + H2[1] + H3[1];
  Hx[2] = H1[2] + H2[2] + H3[2];
  
  ld Z = 2;
  
  if(!euclid) { 
    Z = intval(Hx, Hypc); 
    Z = sqrt(-Z);
    }
  
  for(int c=0; c<3; c++) Hx[c] /= Z;
  
  return Hx;
  }

hyperpoint mid4(const hyperpoint& H1, const hyperpoint& H2, const hyperpoint& H3, const hyperpoint& H4) {

  hyperpoint Hx;
  Hx[0] = H1[0] + H2[0] + H3[0] + H4[0];
  Hx[1] = H1[1] + H2[1] + H3[1] + H4[1];
  Hx[2] = H1[2] + H2[2] + H3[2] + H4[2];
  
  ld Z = 2;
  
  if(!euclid) { 
    Z = intval(Hx, Hypc); 
    Z = sqrt(-Z);
    }
  
  for(int c=0; c<3; c++) Hx[c] /= Z;
  
  return Hx;
  }

// matrices
//==========

// matrices represent isometries of the hyperbolic plane
// (just like isometries of the sphere are represented by rotation matrices)

struct transmatrix {
  ld tab[3][3];
  ld * operator [] (int i) { return tab[i]; }
  const ld * operator [] (int i) const { return tab[i]; }
  };

// identity matrix
transmatrix Id = {{{1,0,0}, {0,1,0}, {0,0,1}}};

transmatrix Mirror = {{{1,0,0}, {0,-1,0}, {0,0,1}}};

hyperpoint operator * (const transmatrix& T, const hyperpoint& H) {
  hyperpoint z;
  for(int i=0; i<3; i++) {
    z[i] = 0;
    for(int j=0; j<3; j++) z[i] += T[i][j] * H[j];
    }
  return z;
  }

transmatrix operator * (const transmatrix& T, const transmatrix& U) {
  transmatrix R;
  for(int i=0; i<3; i++) for(int j=0; j<3; j++) R[i][j] = 0;
  for(int i=0; i<3; i++) for(int j=0; j<3; j++) for(int k=0; k<3; k++)
    R[i][j] += T[i][k] * U[k][j];
  return R;
  }

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

// push alpha units to the right
transmatrix xpush(ld alpha) {
  if(euclid) return eupush(alpha, 0);
  transmatrix T = Id;
  T[0][0] = +cosh(alpha); T[0][2] = +sinh(alpha);
  T[2][0] = +sinh(alpha); T[2][2] = +cosh(alpha);
  return T;
  }

double inverse_sinh(ld z) {
  return log(z+sqrt(1+z*z));
  }
 
// push alpha units vertically
transmatrix ypush(ld alpha) {
  if(euclid) return eupush(0, alpha);
  transmatrix T = Id;
  T[1][1] = +cosh(alpha); T[1][2] = +sinh(alpha);
  T[2][1] = +sinh(alpha); T[2][2] = +cosh(alpha);
  return T;
  }

// rotate the hyperplane around C0 such that H[1] == 0 and H[0] >= 0
transmatrix spintox(hyperpoint H) {
  transmatrix T = Id;
  ld R = sqrt(H[0] * H[0] + H[1] * H[1]);
  if(R >= 1e-12) {
    T[0][0] = +H[0]/R; T[0][1] = +H[1]/R;
    T[1][0] = -H[1]/R; T[1][1] = +H[0]/R;
    }
  return T;
  }

// reverse of spintox(H)
transmatrix rspintox(hyperpoint H) {
  transmatrix T = Id;
  ld R = sqrt(H[0] * H[0] + H[1] * H[1]);
  if(R >= 1e-12) {
    T[0][0] = +H[0]/R; T[0][1] = -H[1]/R;
    T[1][0] = +H[1]/R; T[1][1] = +H[0]/R;
    }
  return T;
  }

// for H such that H[1] == 0, this matrix pushes H to C0
transmatrix pushxto0(hyperpoint H) {
  if(euclid) return eupush(-H[0], -H[1]);
  transmatrix T = Id;
  T[0][0] = +H[2]; T[0][2] = -H[0];
  T[2][0] = -H[0]; T[2][2] = +H[2];
  return T;
  }

// reverse of pushxto0(H)
transmatrix rpushxto0(hyperpoint H) {
  if(euclid) return eupush(H[0], H[1]);
  transmatrix T = Id;
  T[0][0] = +H[2]; T[0][2] = +H[0];
  T[2][0] = +H[0]; T[2][2] = +H[2];
  return T;
  }

// generalization: H[1] can be non-zero
transmatrix gpushxto0(hyperpoint H) {
  hyperpoint H2 = spintox(H) * H;
  return rspintox(H) * pushxto0(H2) * spintox(H);
  }

transmatrix rgpushxto0(hyperpoint H) {
  hyperpoint H2 = spintox(H) * H;
  return rspintox(H) * rpushxto0(H2) * spintox(H);
  }


// fix the matrix T so that it is indeed an isometry
// (without using this, imprecision could accumulate)

void display(const transmatrix& T);

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
    printf(" -> %10.7f\n", double(squar(T[y][0]) + squar(T[y][1]) - squar(T[y][2])));
    // printf("\n");
    }
  for(int x=0; x<3; x++) printf("%10.7f", double(squar(T[0][x]) + squar(T[1][x]) - squar(T[2][x]))); printf("\n");
  for(int x=0; x<3; x++) {
    int y = (x+1) % 3;
    printf("%10.7f", double(T[0][x]*T[0][y] + T[1][x]*T[1][y] - T[2][x]*T[2][y]));
    }
  printf("\n\n");
  }

transmatrix inverse(transmatrix T) {
  ld det = 0;
  for(int i=0; i<3; i++) 
    det += T[0][i] * T[1][(i+1)%3] * T[2][(i+2)%3];
  for(int i=0; i<3; i++) 
    det -= T[0][i] * T[1][(i+2)%3] * T[2][(i+1)%3];
  
  transmatrix T2;
  if(det == 0) return T2;
  
  for(int i=0; i<3; i++) 
  for(int j=0; j<3; j++) 
    T2[j][i] = (T[(i+1)%3][(j+1)%3] * T[(i+2)%3][(j+2)%3] - T[(i+1)%3][(j+2)%3] * T[(i+2)%3][(j+1)%3]) / det;

  return T2;
  }

double hdist(hyperpoint h1, hyperpoint h2) {
  hyperpoint mh = gpushxto0(h1) * h2;
  return inverse_sinh(sqrt(mh[0]*mh[0]+mh[1]*mh[1]));
  }

namespace hyperpoint_vec {

  hyperpoint operator * (double d, hyperpoint h) { 
    return hpxyz(h[0]*d, h[1]*d, h[2]*d);
    }
  
  hyperpoint operator * (hyperpoint h, double d) { 
    return hpxyz(h[0]*d, h[1]*d, h[2]*d);
    }
  
  hyperpoint operator / (hyperpoint h, double d) { 
    return hpxyz(h[0]/d, h[1]/d, h[2]/d);
    }
  
  hyperpoint operator + (hyperpoint h, hyperpoint h2) { 
    return hpxyz(h[0]+h2[0], h[1]+h2[1], h[2]+h2[2]);
    }
  
  hyperpoint operator - (hyperpoint h, hyperpoint h2) { 
    return hpxyz(h[0]-h2[0], h[1]-h2[1], h[2]-h2[2]);
    }
  
  }
