// Hyperbolic Rogue -- basic computations in non-Euclidean geometry
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file hyperpoint.cpp
 *  \brief basic computations in non-Euclidean geometry
 *
 *  This implements hyperpoint (a point in non-Euclidean space), transmatrix (a transformation matrix),
 *  and various basic routines related to them: rotations, translations, inverses and determinants, etc.
 *  For nonisotropic geometries, it rather refers to nonisotropic.cpp.
 */

namespace hr {

#if HDR
static const ld degree = M_PI / 180;
#endif

eGeometry geometry;
eVariation variation;


// hyperbolic points and matrices

#if HDR
struct hyperpoint : array<ld, MAXMDIM> {
  hyperpoint() {}
  
  hyperpoint(ld x, ld y, ld z, ld w) { 
    self[0] = x; self[1] = y; self[2] = z; 
    if(MAXMDIM == 4) self[3] = w;
    }

  inline hyperpoint& operator *= (ld d) {
    for(int i=0; i<MDIM; i++) self[i] *= d;
    return self;
    }
  
  inline hyperpoint& operator /= (ld d) { 
    for(int i=0; i<MDIM; i++) self[i] /= d;
    return self;
    }
  
  inline hyperpoint& operator += (const hyperpoint h2) { 
    for(int i=0; i<MDIM; i++) self[i] += h2[i];
    return self;
    }

  inline hyperpoint& operator -= (const hyperpoint h2) { 
    for(int i=0; i<MDIM; i++) self[i] -= h2[i];
    return self;
    }

  inline friend hyperpoint operator * (ld d, hyperpoint h) { return h *= d; }  
  inline friend hyperpoint operator * (hyperpoint h, ld d) { return h *= d; }  
  inline friend hyperpoint operator / (hyperpoint h, ld d) { return h /= d; }  
  inline friend hyperpoint operator + (hyperpoint h, hyperpoint h2) { return h += h2; }
  inline friend hyperpoint operator - (hyperpoint h, hyperpoint h2) { return h -= h2; }

  // cross product  
  inline friend hyperpoint operator ^ (hyperpoint h1, hyperpoint h2) {
    return hyperpoint(
      h1[1] * h2[2] - h1[2] * h2[1],
      h1[2] * h2[0] - h1[0] * h2[2],
      h1[0] * h2[1] - h1[1] * h2[0],
      0
      );
    }

  // inner product
  inline friend ld operator | (hyperpoint h1, hyperpoint h2) {
    ld sum = 0;
    for(int i=0; i<MDIM; i++) sum += h1[i] * h2[i];
    return sum;
    }    
  };

struct transmatrix {
  ld tab[MAXMDIM][MAXMDIM];
  hyperpoint& operator [] (int i) { return (hyperpoint&)tab[i][0]; }
  const ld * operator [] (int i) const { return tab[i]; }
  
  inline friend hyperpoint operator * (const transmatrix& T, const hyperpoint& H) {
    hyperpoint z;
    for(int i=0; i<MDIM; i++) {
      z[i] = 0;
      for(int j=0; j<MDIM; j++) z[i] += T[i][j] * H[j];
      }
    return z;
    }

  inline friend transmatrix operator * (const transmatrix& T, const transmatrix& U) {
    transmatrix R;
    for(int i=0; i<MDIM; i++) for(int j=0; j<MDIM; j++) {
      R[i][j] = 0;
      for(int k=0; k<MDIM; k++)
        R[i][j] += T[i][k] * U[k][j];
      }
    return R;
    }  
  };


constexpr transmatrix diag(ld a, ld b, ld c, ld d) {
  #if MAXMDIM==3
  return transmatrix{{{a,0,0}, {0,b,0}, {0,0,c}}};
  #else
  return transmatrix{{{a,0,0,0}, {0,b,0,0}, {0,0,c,0}, {0,0,0,d}}};
  #endif
  }

const static hyperpoint Hypc = hyperpoint(0, 0, 0, 0);

// identity matrix
const static transmatrix Id = diag(1,1,1,1);

// zero matrix
const static transmatrix Zero = diag(0,0,0,0);

// mirror image
const static transmatrix Mirror = diag(1,-1,1,1);
const static transmatrix MirrorY = diag(1,-1,1,1);

// mirror image
const static transmatrix MirrorX = diag(-1,1,1,1);

// mirror image
const static transmatrix MirrorZ = diag(1,1,-1,1);

// rotate by PI
const static transmatrix pispin = diag(-1,-1,1,1);

// central symmetry
const static transmatrix centralsym = diag(-1,-1,-1,-1);

inline hyperpoint hpxyz(ld x, ld y, ld z) { return MDIM == 3 ? hyperpoint(x,y,z,0) : hyperpoint(x,y,0,z); }
inline hyperpoint hpxyz3(ld x, ld y, ld z, ld w) { return MDIM == 3 ? hyperpoint(x,y,w,0) : hyperpoint(x,y,z,w); }
inline hyperpoint point3(ld x, ld y, ld z) { return hyperpoint(x,y,z,0); }
inline hyperpoint point31(ld x, ld y, ld z) { return hyperpoint(x,y,z,1); }
inline hyperpoint point2(ld x, ld y) { return hyperpoint(x,y,0,0); }

extern const hyperpoint C02, C03;

#define C0 (MDIM == 3 ? C02 : C03)
#endif

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

EX ld squar(ld x) { return x*x; }

EX int sig(int z) { return prod ? PIU(sig(z)) : (sphere || sol || z<GDIM)?1:-1; }

EX int curvature() {
  switch(cgclass) {
    case gcEuclid: return 0;
    case gcHyperbolic: return -1;
    case gcSphere: return 1;
    case gcProduct: return PIU(curvature());
    default: return 0;
    }
  }

EX ld sin_auto(ld x) {
  switch(cgclass) {
    case gcEuclid: return x;
    case gcHyperbolic: return sinh(x);
    case gcSphere: return sin(x);
    case gcProduct: return PIU(sin_auto(x));
    default: return x;
    }
  }

EX ld asin_auto(ld x) {
  switch(cgclass) {
    case gcEuclid: return x;
    case gcHyperbolic: return asinh(x);
    case gcSphere: return asin(x);
    case gcProduct: return PIU(asin_auto(x));
    default: return x;
    }
  }

EX ld acos_auto(ld x) {
  switch(cgclass) {
    case gcHyperbolic: return acosh(x);
    case gcSphere: return acos(x);
    case gcProduct: return PIU(acos_auto(x));
    default: return x;
    }
  }

EX ld volume_auto(ld r) {
  switch(cgclass) {
    case gcEuclid: return 4 * r * r * r / 3 * M_PI;
    case gcHyperbolic: return M_PI * (sinh(2*r) - 2 * r);
    case gcSphere: return M_PI * (2 * r - sin(2*r));
    default: return 0;
    }
  }

EX ld asin_auto_clamp(ld x) {
  switch(cgclass) {
    case gcEuclid: return x;
    case gcHyperbolic: return asinh(x);
    case gcSphere: return x>1 ? M_PI/2 : x<-1 ? -M_PI/2 : std::isnan(x) ? 0 : asin(x);
    default: return x;
    }
  }

EX ld acos_auto_clamp(ld x) {
  switch(cgclass) {
    case gcHyperbolic: return x < 1 ? 0 : acosh(x);
    case gcSphere: return x > 1 ? 0 : x < -1 ? M_PI : acos(x);
    case gcProduct: return PIU(acos_auto_clamp(x));
    default: return x;
    }
  }

EX ld cos_auto(ld x) {
  switch(cgclass) {
    case gcEuclid: return 1;
    case gcHyperbolic: return cosh(x);
    case gcSphere: return cos(x);
    case gcProduct: return PIU(cos_auto(x));
    default: return 1;
    }
  }

EX ld tan_auto(ld x) {
  switch(cgclass) {
    case gcEuclid: return x;
    case gcHyperbolic: return tanh(x);
    case gcSphere: return tan(x);
    case gcProduct: return PIU(tan_auto(x));
    default: return 1;
    }
  }

EX ld atan_auto(ld x) {
  switch(cgclass) {
    case gcEuclid: return x;
    case gcHyperbolic: return atanh(x);
    case gcSphere: return atan(x);
    case gcProduct: return PIU(atan_auto(x));
    default: return x;
    }
  }

EX ld atan2_auto(ld y, ld x) {
  switch(cgclass) {
    case gcEuclid: return y/x;
    case gcHyperbolic: return atanh(y/x);
    case gcSphere: return atan2(y, x);
    case gcProduct: return PIU(atan2_auto(y, x));
    default: return y/x;
    }
  }

// cosine rule -- edge opposite alpha
EX ld edge_of_triangle_with_angles(ld alpha, ld beta, ld gamma) {
  return acos_auto((cos(alpha) + cos(beta) * cos(gamma)) / (sin(beta) * sin(gamma)));
  }

// hyperbolic point:
//===================

// we represent the points on the hyperbolic plane
// by points in 3D space (Minkowski space) such that x^2+y^2-z^2 == -1, z > 0
// (this is analogous to representing a sphere with points such that x^2+y^2+z^2 == 1)

hyperpoint hpxy(ld x, ld y) { 
  return hpxyz(x,y, translatable ? 1 : sphere ? sqrt(1-x*x-y*y) : sqrt(1+x*x+y*y));
  }

hyperpoint hpxy3(ld x, ld y, ld z) { 
  return hpxyz3(x,y,z, translatable ? 1 : sphere ? sqrt(1-x*x-y*y-z*z) : sqrt(1+x*x+y*y+z*z));
  }

// origin of the hyperbolic plane
const hyperpoint C02 = hyperpoint(0,0,1,0);
const hyperpoint C03 = hyperpoint(0,0,0,1);

// a point (I hope this number needs no comments ;) )
const hyperpoint Cx12 = hyperpoint(1,0,1.41421356237,0);
const hyperpoint Cx13 = hyperpoint(1,0,0,1.41421356237);

#define Cx1 (GDIM==2?Cx12:Cx13)

EX bool zero_d(int d, hyperpoint h) { 
  for(int i=0; i<d; i++) if(h[i]) return false;
  return true;
  }

// this function returns approximate square of distance between two points
// (in the spherical analogy, this would be the distance in the 3D space,
// through the interior, not on the surface)
// also used to verify whether a point h1 is on the hyperbolic plane by using Hypc for h2

EX ld intval(const hyperpoint &h1, const hyperpoint &h2) {
  ld res = 0;
  for(int i=0; i<MDIM; i++) res += squar(h1[i] - h2[i]) * sig(i);
  if(elliptic) {
    ld res2 = 0;
    for(int i=0; i<MDIM; i++) res2 += squar(h1[i] + h2[i]) * sig(i);
    return min(res, res2);
    }
  return res;
  }

EX ld quickdist(const hyperpoint &h1, const hyperpoint &h2) {
  if(prod) return hdist(h1, h2);
  return intval(h1, h2);
  }

EX ld sqhypot_d(int d, const hyperpoint& h) {
  ld sum = 0;
  for(int i=0; i<d; i++) sum += h[i]*h[i];
  return sum;
  }
  
EX ld hypot_d(int d, const hyperpoint& h) {
  return sqrt(sqhypot_d(d, h));
  }
  
EX ld zlevel(const hyperpoint &h) {
  if(translatable) return h[LDIM];
  else if(sphere) return sqrt(intval(h, Hypc));
  else if(prod) return log(sqrt(abs(intval(h, Hypc)))); /* abs works with both underlying spherical and hyperbolic */
  else return (h[LDIM] < 0 ? -1 : 1) * sqrt(-intval(h, Hypc));
  }

EX ld hypot_auto(ld x, ld y) {
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
EX hyperpoint normalize(hyperpoint H) {
  if(prod) return H;
  ld Z = zlevel(H);
  for(int c=0; c<MDIM; c++) H[c] /= Z;
  return H;
  }

// get the center of the line segment from H1 to H2
hyperpoint mid(const hyperpoint& H1, const hyperpoint& H2) {
  if(prod) {
    auto d1 = product_decompose(H1);
    auto d2 = product_decompose(H2);
    return zshift(PIU( mid(d1.second, d2.second) ), (d1.first + d2.first) / 2);
    }
  return normalize(H1 + H2);
  }

// like mid, but take 3D into account
EX hyperpoint midz(const hyperpoint& H1, const hyperpoint& H2) {
  if(prod) return mid(H1, H2);
  hyperpoint H3 = H1 + H2;
  
  ld Z = 2;
  
  if(!euclid) Z = zlevel(H3) * 2 / (zlevel(H1) + zlevel(H2));
  for(int c=0; c<MDIM; c++) H3[c] /= Z;
  
  return H3;
  }

// matrices
//==========

// matrices represent isometries of the hyperbolic plane
// (just like isometries of the sphere are represented by rotation matrices)

// rotate by alpha degrees
EX transmatrix cspin(int a, int b, ld alpha) {
  transmatrix T = Id;
  T[a][a] = +cos(alpha); T[a][b] = +sin(alpha);
  T[b][a] = -sin(alpha); T[b][b] = +cos(alpha);
  return T;
  }

EX transmatrix spin(ld alpha) { return cspin(0, 1, alpha); }

EX transmatrix random_spin() {
  if(WDIM == 2) return spin(randd() * 2 * M_PI);
  else {
    ld alpha2 = acos(randd() * 2 - 1);
    ld alpha = randd() * 2 * M_PI;
    ld alpha3 = randd() * 2 * M_PI;
    return cspin(0, 2, alpha2) * cspin(0, 1, alpha) * cspin(1, 2, alpha3);
    }
  }

EX transmatrix eupush(ld x, ld y) {
  transmatrix T = Id;
  T[0][LDIM] = x;
  T[1][LDIM] = y;
  return T;
  }

EX transmatrix eupush(hyperpoint h) {
  if(nonisotropic) return nisot::translate(h);
  transmatrix T = Id;
  for(int i=0; i<GDIM; i++) T[i][LDIM] = h[i];
  return T;
  }

EX transmatrix eupush3(ld x, ld y, ld z) {
  return eupush(point3(x, y, z));
  }

EX transmatrix euscalezoom(hyperpoint h) {
  transmatrix T = Id;
  T[0][0] = h[0];
  T[0][1] = -h[1];
  T[1][0] = h[1];
  T[1][1] = h[0];
  return T;
  }

EX transmatrix euaffine(hyperpoint h) {
  transmatrix T = Id;
  T[0][1] = h[0];
  T[1][1] = exp(h[1]);
  return T;
  }

EX transmatrix cpush(int cid, ld alpha) {
  transmatrix T = Id;
  if(prod && cid == 2)
    return mscale(Id, alpha);
  if(nonisotropic) 
    return eupush3(cid == 0 ? alpha : 0, cid == 1 ? alpha : 0, cid == 2 ? alpha : 0);
  T[LDIM][LDIM] = T[cid][cid] = cos_auto(alpha);
  T[cid][LDIM] = sin_auto(alpha);
  T[LDIM][cid] = -curvature() * sin_auto(alpha);
  return T;
  }

// push alpha units to the right
EX transmatrix xpush(ld alpha) { return cpush(0, alpha); }

EX bool eqmatrix(transmatrix A, transmatrix B, ld eps IS(.01)) {
  for(int i=0; i<MDIM; i++)
  for(int j=0; j<MDIM; j++)
    if(std::abs(A[i][j] - B[i][j]) > eps)
      return false;
  return true;
  }

#if MAXMDIM >= 4
// in the 3D space, move the point h orthogonally to the (x,y) plane by z units
EX hyperpoint orthogonal_move(const hyperpoint& h, ld z) {
  if(prod) return zshift(h, z);
  if(!hyperbolic) return rgpushxto0(h) * cpush(2, z) * C0;
  if(nil) return nisot::translate(h) * cpush0(2, z);
  if(translatable) return hpxy3(h[0], h[1], h[2] + z);
  ld u = 1;
  if(h[2]) z += asin_auto(h[2]), u /= acos_auto(z);
  u *= cos_auto(z);
  return hpxy3(h[0] * u, h[1] * u, sinh(z));
  }
#endif

// push alpha units vertically
EX transmatrix ypush(ld alpha) { return cpush(1, alpha); }

EX transmatrix zpush(ld z) { return cpush(2, z); }

EX transmatrix matrix3(ld a, ld b, ld c, ld d, ld e, ld f, ld g, ld h, ld i) {
  #if MAXMDIM==3
  return transmatrix {{{a,b,c},{d,e,f},{g,h,i}}};
  #else
  if(GDIM == 2)
    return transmatrix {{{a,b,c,0},{d,e,f,0},{g,h,i,0},{0,0,0,1}}};
  else
    return transmatrix {{{a,b,0,c},{d,e,0,f},{0,0,1,0},{g,h,0,i}}};
  #endif
  }

EX transmatrix matrix4(ld a, ld b, ld c, ld d, ld e, ld f, ld g, ld h, ld i, ld j, ld k, ld l, ld m, ld n, ld o, ld p) {
  #if MAXMDIM==3
  return transmatrix {{{a,b,d},{e,f,h},{m,n,p}}};
  #else
  return transmatrix {{{a,b,c,d},{e,f,g,h},{i,j,k,l},{m,n,o,p}}};
  #endif
  }

#if MAXMDIM >= 4
EX void swapmatrix(transmatrix& T) {
  for(int i=0; i<4; i++) swap(T[i][2], T[i][3]);
  for(int i=0; i<4; i++) swap(T[2][i], T[3][i]);
  if(GDIM == 3) {
    for(int i=0; i<4; i++) T[i][2] = T[2][i] = 0;
    T[2][2] = 1;
    }
  fixmatrix(T);
  for(int i=0; i<4; i++) for(int j=0; j<4; j++) if(isnan(T[i][j])) T = Id;
  }

EX void swapmatrix(hyperpoint& h) {
  swap(h[2], h[3]);
  }
#endif

EX transmatrix parabolic1(ld u) {
  if(euclid)
    return ypush(u);
  else {
    ld diag = u*u/2;
    return matrix3(
      -diag+1, u, diag,
      -u, 1, u,
      -diag, u, diag+1
      );
    }
  }

EX transmatrix parabolic13(ld u, ld v) {
  if(euclid)
    return ypush(u);
  else {
    ld diag = (u*u+v*v)/2;
    return matrix4(
      -diag+1, u, v, diag,
      -u, 1, 0, u,
      -v, 0, 1, v,
      -diag, u, v, diag+1
      );
    }
  }

EX transmatrix spintoc(const hyperpoint& H, int t, int f) {
  transmatrix T = Id;
  ld R = hypot(H[f], H[t]);
  if(R >= 1e-12) {
    T[t][t] = +H[t]/R; T[t][f] = +H[f]/R;
    T[f][t] = -H[f]/R; T[f][f] = +H[t]/R;
    }
  return T;
  }

EX transmatrix rspintoc(const hyperpoint& H, int t, int f) {
  transmatrix T = Id;
  ld R = hypot(H[f], H[t]);
  if(R >= 1e-12) {
    T[t][t] = +H[t]/R; T[t][f] = -H[f]/R;
    T[f][t] = +H[f]/R; T[f][f] = +H[t]/R;
    }
  return T;
  }

// rotate the hyperbolic plane around C0 such that H[1] == 0 and H[0] >= 0
EX transmatrix spintox(const hyperpoint& H) {
  if(GDIM == 2 || prod) return spintoc(H, 0, 1); 
  transmatrix T1 = spintoc(H, 0, 1);
  return spintoc(T1*H, 0, 2) * T1;
  }

EX void set_column(transmatrix& T, int i, const hyperpoint& H) {
  for(int j=0; j<MDIM; j++)
    T[j][i] = H[j];
  }

EX transmatrix build_matrix(hyperpoint h1, hyperpoint h2, hyperpoint h3, hyperpoint h4) {
  transmatrix T;
  for(int i=0; i<MDIM; i++)
    T[i][0] = h1[i],
    T[i][1] = h2[i],
    T[i][2] = h3[i];
  if(MAXMDIM == 4) for(int i=0; i<MDIM; i++) T[i][3] = h4[i];
  return T;
  }

// reverse of spintox(H)
EX transmatrix rspintox(const hyperpoint& H) {
  if(GDIM == 2 || prod) return rspintoc(H, 0, 1); 
  transmatrix T1 = spintoc(H, 0, 1);
  return rspintoc(H, 0, 1) * rspintoc(T1*H, 0, 2);
  }

// for H such that H[1] == 0, this matrix pushes H to C0
EX transmatrix pushxto0(const hyperpoint& H) {
  transmatrix T = Id;
  T[0][0] = +H[LDIM]; T[0][LDIM] = -H[0];
  T[LDIM][0] = curvature() * H[0]; T[LDIM][LDIM] = +H[LDIM];
  return T;
  }

// reverse of pushxto0(H)
EX transmatrix rpushxto0(const hyperpoint& H) {
  transmatrix T = Id;
  T[0][0] = +H[LDIM]; T[0][LDIM] = H[0];
  T[LDIM][0] = -curvature() * H[0]; T[LDIM][LDIM] = +H[LDIM];
  return T;
  }

EX transmatrix ggpushxto0(const hyperpoint& H, ld co) {
  if(translatable) {
    return eupush(co * H);
    }
  if(prod) {
    auto d = product_decompose(H);
    return mscale(PIU(ggpushxto0(d.second, co)), d.first * co);
    }
  transmatrix res = Id;
  if(sqhypot_d(GDIM, H) < 1e-12) return res;
  ld fac = (H[LDIM]-1) / sqhypot_d(GDIM, H);
  for(int i=0; i<GDIM; i++)
  for(int j=0; j<GDIM; j++)
    res[i][j] += H[i] * H[j] * fac;
    
  for(int d=0; d<GDIM; d++)
    res[d][LDIM] = co * H[d],
    res[LDIM][d] = -curvature() * co * H[d];
  res[LDIM][LDIM] = H[LDIM];
  
  return res;
  }

// generalization: H[1] can be non-zero
EX transmatrix gpushxto0(const hyperpoint& H) {
  return ggpushxto0(H, -1);
  }

EX transmatrix rgpushxto0(const hyperpoint& H) {
  return ggpushxto0(H, 1);
  }

// fix the matrix T so that it is indeed an isometry
// (without using this, imprecision could accumulate)

EX void fixmatrix(transmatrix& T) {
  if(nonisotropic) ; // T may be inverse... do not do that
  else if(prod) {
    auto z = zlevel(tC0(T));
    T = mscale(T, -z);
    PIU(fixmatrix(T));
    T = mscale(T, +z);
    }
  else if(euclid) {
    for(int x=0; x<GDIM; x++) for(int y=0; y<=x; y++) {
      ld dp = 0;
      for(int z=0; z<GDIM; z++) dp += T[z][x] * T[z][y];
      
      if(y == x) dp = 1 - sqrt(1/dp);
      
      for(int z=0; z<GDIM; z++) T[z][x] -= dp * T[z][y];
      }
    for(int x=0; x<GDIM; x++) T[LDIM][x] = 0;
    T[LDIM][LDIM] = 1;
    }
  else for(int x=0; x<MDIM; x++) for(int y=0; y<=x; y++) {
    ld dp = 0;
    for(int z=0; z<MDIM; z++) dp += T[z][x] * T[z][y] * sig(z);
    
    if(y == x) dp = 1 - sqrt(sig(x)/dp);
    
    for(int z=0; z<MDIM; z++) T[z][x] -= dp * T[z][y];
    }
  }

// show the matrix on screen

EX ld det(const transmatrix& T) {
  if(GDIM == 2) {
    ld det = 0;
    for(int i=0; i<3; i++) 
      det += T[0][i] * T[1][(i+1)%3] * T[2][(i+2)%3];
    for(int i=0; i<3; i++) 
      det -= T[0][i] * T[1][(i+2)%3] * T[2][(i+1)%3];
    return det;
    }
  else {
    ld det = 1;
    transmatrix M = T;
    for(int a=0; a<MDIM; a++) {
      for(int b=a; b<=GDIM; b++)
        if(M[b][a]) {
          if(b != a)
            for(int c=a; c<MDIM; c++) tie(M[b][c], M[a][c]) = make_pair(-M[a][c], M[b][c]);
          break;
          }
      if(!M[a][a]) return 0;
      for(int b=a+1; b<=GDIM; b++) {
        ld co = -M[b][a] / M[a][a];
        for(int c=a; c<MDIM; c++) M[b][c] += M[a][c] * co;
        }
      det *= M[a][a];
      }
    return det;
    }
  }

void inverse_error(const transmatrix& T) {
  println(hlog, "Warning: inverting a singular matrix: ", T);
  }

EX transmatrix inverse(const transmatrix& T) {
  if(GDIM == 2) {
    ld d = det(T);
    transmatrix T2;
    if(d == 0) {
      inverse_error(T); 
      return Id;
      }
    
    for(int i=0; i<3; i++) 
    for(int j=0; j<3; j++) 
      T2[j][i] = (T[(i+1)%3][(j+1)%3] * T[(i+2)%3][(j+2)%3] - T[(i+1)%3][(j+2)%3] * T[(i+2)%3][(j+1)%3]) / d;
    return T2;
    }
  else {
    transmatrix T1 = T;
    transmatrix T2 = Id;
  
    for(int a=0; a<MDIM; a++) {
      int best = a;
      
      for(int b=a+1; b<MDIM; b++)
        if(abs(T1[b][a]) > abs(T1[best][a]))
          best = b;

      int b = best;

      if(b != a)
        for(int c=0; c<MDIM; c++) 
          swap(T1[b][c], T1[a][c]), swap(T2[b][c], T2[a][c]);

      if(!T1[a][a]) { inverse_error(T); return Id; }
      for(int b=a+1; b<=GDIM; b++) {
        ld co = -T1[b][a] / T1[a][a];
        for(int c=0; c<MDIM; c++) T1[b][c] += T1[a][c] * co, T2[b][c] += T2[a][c] * co;
        }
      }
    
    for(int a=MDIM-1; a>=0; a--) {
      for(int b=0; b<a; b++) {
        ld co = -T1[b][a] / T1[a][a];
        for(int c=0; c<MDIM; c++) T1[b][c] += T1[a][c] * co, T2[b][c] += T2[a][c] * co;
        }
      ld co = 1 / T1[a][a];
      for(int c=0; c<MDIM; c++) T1[a][c] *= co, T2[a][c] *= co;
      }
    return T2;
    }
  }

EX pair<ld, hyperpoint> product_decompose(hyperpoint h) {
  ld z = zlevel(h);
  return make_pair(z, mscale(h, -z));
  }

// distance between mh and 0
EX ld hdist0(const hyperpoint& mh) {
  switch(cgclass) {
    case gcHyperbolic:
      if(mh[LDIM] < 1) return 0;
      return acosh(mh[LDIM]);
    case gcEuclid: {
      return hypot_d(GDIM, mh);
      }
    case gcSphere: {
      ld res = mh[LDIM] >= 1 ? 0 : mh[LDIM] <= -1 ? M_PI : acos(mh[LDIM]);
      if(elliptic && res > M_PI/2) res = M_PI-res;
      return res;
      }
    case gcProduct: {
      auto d1 = product_decompose(mh);
      return hypot(PIU(hdist0(d1.second)), d1.first);
      }
    default:
      return hypot_d(GDIM, mh);
    }
  }

EX ld circlelength(ld r) {
  switch(cgclass) {
    case gcEuclid:
      return 2 * M_PI * r;
    case gcHyperbolic:
      return 2 * M_PI * sinh(r);
    case gcSphere:
      return 2 * M_PI * sin(r);
    default:
      return 2 * M_PI * r;
    }
  }

// distance between two points
EX ld hdist(const hyperpoint& h1, const hyperpoint& h2) {
  // return hdist0(gpushxto0(h1) * h2);
  ld iv = intval(h1, h2);
  switch(cgclass) {
    case gcEuclid:
      if(iv < 0) return 0;
      return sqrt(iv);
    case gcHyperbolic:
      if(iv < 0) return 0;
      return 2 * asinh(sqrt(iv) / 2);
    case gcSphere:
      return 2 * asin_auto_clamp(sqrt(iv) / 2);
    case gcProduct: {
      auto d1 = product_decompose(h1);
      auto d2 = product_decompose(h2);
      return hypot(PIU(hdist(d1.second, d2.second)), d1.first - d2.first);
      }
    default:
      if(iv < 0) return 0;
      return sqrt(iv);
    }
  }

EX hyperpoint mscale(const hyperpoint& t, double fac) {
  if(GDIM == 3 && !prod) return cpush(2, fac) * t;
  if(prod) fac = exp(fac);
  hyperpoint res;
  for(int i=0; i<MDIM; i++) 
    res[i] = t[i] * fac;
  return res;
  }

EX transmatrix mscale(const transmatrix& t, double fac) {
  if(GDIM == 3 && !prod) {
    // if(pmodel == mdFlatten) { transmatrix u = t; u[2][LDIM] -= fac; return u; }
    return t * cpush(2, fac);
    }
  if(prod) fac = exp(fac);
  transmatrix res;
  for(int i=0; i<MDIM; i++) for(int j=0; j<MDIM; j++)
    res[i][j] = t[i][j] * fac;
  return res;
  }

EX transmatrix xyscale(const transmatrix& t, double fac) {
  transmatrix res;
  for(int i=0; i<MDIM; i++) for(int j=0; j<GDIM; j++)
    res[i][j] = t[i][j] * fac;
  return res;
  }

EX transmatrix xyzscale(const transmatrix& t, double fac, double facz) {
  transmatrix res;
  for(int i=0; i<MDIM; i++) for(int j=0; j<GDIM; j++)
    res[i][j] = t[i][j] * fac;
  for(int i=0; i<MDIM; i++) 
    res[i][LDIM] = t[i][LDIM] * facz;
  return res;
  }

// double downspin_zivory;

EX transmatrix mzscale(const transmatrix& t, double fac) {
  if(GDIM == 3) return t * cpush(2, fac);
  // take only the spin
  transmatrix tcentered = gpushxto0(tC0(t)) * t;
  // tcentered = tcentered * spin(downspin_zivory);
  fac -= 1;
  transmatrix res = t * inverse(tcentered) * ypush(-fac) * tcentered;
  fac *= .2;
  fac += 1;
  for(int i=0; i<MDIM; i++) for(int j=0; j<MDIM; j++)
    res[i][j] = res[i][j] * fac;
  return res;
  }

EX transmatrix pushone() { return xpush(sphere?.5 : 1); }

// rotation matrix in R^3

EX hyperpoint mid3(hyperpoint h1, hyperpoint h2, hyperpoint h3) {
  return mid(h1+h2+h3, h1+h2+h3);
  }

EX hyperpoint mid_at(hyperpoint h1, hyperpoint h2, ld v) {
  hyperpoint h = h1 * (1-v) + h2 * v;
  return mid(h, h);
  }  

EX hyperpoint mid_at_actual(hyperpoint h, ld v) {
  return rspintox(h) * xpush0(hdist0(h) * v);
  }

// in 3D, an orthogonal projection of C0 on the given triangle
EX hyperpoint orthogonal_of_C0(hyperpoint h0, hyperpoint h1, hyperpoint h2) {
  h0 /= h0[3];
  h1 /= h1[3];
  h2 /= h2[3];
  hyperpoint w = h0;
  hyperpoint d1 = h1 - h0;
  hyperpoint d2 = h2 - h0;
  ld denom = (d1|d1) * (d2|d2) - (d1|d2) * (d1|d2);
  ld a1 = (d2|w) * (d1|d2) - (d1|w) * (d2|d2);
  ld a2 = (d1|w) * (d1|d2) - (d2|w) * (d1|d1);
  hyperpoint h = w * denom + d1 * a1 + d2 * a2;
  return normalize(h);
  }

EX hyperpoint zshift(hyperpoint x, ld z) {
  if(GDIM == 3 && WDIM == 2) return rgpushxto0(x) * cpush0(2, z);
  else if(prod) return mscale(x, z);
  else return mscale(x, z);
  }

EX hyperpoint hpxd(ld d, ld x, ld y, ld z) {
  hyperpoint H = hpxyz(d*x, d*y, z);
  H = mid(H, H);
  return H;
  }

EX ld signum(ld x) { return x<0?-1:x>0?1:0; }

bool asign(ld y1, ld y2) { return signum(y1) != signum(y2); }

ld xcross(ld x1, ld y1, ld x2, ld y2) { return x1 + (x2 - x1) * y1 / (y1 - y2); }

EX transmatrix solmul(const transmatrix T, const transmatrix LPe, const transmatrix V) {
  if(nonisotropic || prod) return nisot::transport_view(T, LPe, V);
  else return T * V;
  }

EX transmatrix solmul_pt(const transmatrix Position, const transmatrix T) {
  if(nonisotropic) return nisot::parallel_transport(Position, Id, T);
  else return Position * T;
  }

EX transmatrix solmul_pt(const transmatrix Position, const transmatrix LPe, const transmatrix T) {
  if(nonisotropic || prod) return nisot::parallel_transport(Position, LPe, T);
  else return Position * T;
  }

EX transmatrix spin_towards(const transmatrix Position, const hyperpoint goal, int dir, int back) {
  transmatrix T = 
    nonisotropic ? nisot::spin_towards(Position, goal) : 
    rspintox(inverse(Position) * goal);
  if(back < 0) T = T * spin(M_PI);
  if(dir) T = T * cspin(dir, 0, -M_PI/2);
  T = Position * T;
  return T;
  }

EX ld ortho_error(transmatrix T) {

  ld err = 0;
  
  for(int x=0; x<3; x++) for(int y=0; y<3; y++) {
      ld s = 0;
      for(int z=0; z<3; z++) s += T[z][x] * T[z][y];
      
      s -= (x==y);
      err += s*s;
      }

  return err;      
  }

EX transmatrix transpose(transmatrix T) {
  transmatrix result;
  for(int i=0; i<MDIM; i++)
    for(int j=0; j<MDIM; j++)
      result[j][i] = T[i][j];
  return result;
  }

#if HDR
inline hyperpoint cpush0(int c, ld x) { 
  hyperpoint h = Hypc;
  if(c == 2 && prod) {
    h[2] = exp(x);
    return h;
    }
  h[LDIM] = cos_auto(x);
  h[c] = sin_auto(x);
  return h;
  }

inline hyperpoint xspinpush0(ld alpha, ld x) { 
  hyperpoint h = Hypc;
  h[LDIM] = cos_auto(x);
  h[0] = sin_auto(x) * cos(alpha);
  h[1] = sin_auto(x) * -sin(alpha);
  return h;
  }

inline hyperpoint xpush0(ld x) { return cpush0(0, x); }
inline hyperpoint ypush0(ld x) { return cpush0(1, x); }
inline hyperpoint zpush0(ld x) { return cpush0(2, x); }

// T * C0, optimized
inline hyperpoint tC0(const transmatrix &T) {
  hyperpoint z;
  for(int i=0; i<MDIM; i++) z[i] = T[i][LDIM];
  return z;
  }
#endif

}
