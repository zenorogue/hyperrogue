// Hyperbolic Rogue -- basic computations in non-Euclidean geometry
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file hyperpoint.cpp
 *  \brief basic computations in non-Euclidean geometry
 *
 *  This implements hyperpoint (a point in non-Euclidean space), transmatrix (a transformation matrix),
 *  and various basic routines related to them: rotations, translations, inverses and determinants, etc.
 *  For nonisotropic geometries, it rather refers to nonisotropic.cpp.
 */

#include "hyper.h"
namespace hr {

#if HDR
static const ld degree = M_PI / 180;
#endif

eGeometry geometry;
eVariation variation;


#if HDR
/** \brief A point in our continuous space
 *  Originally used for representing points in the hyperbolic plane.
 *  Currently used for all kinds of supported spaces, as well as
 *  for all vector spaces (up to 4 dimensions). We are using
 *  the normalized homogeneous coordinates, which allows us to work with most
 *  geometries in HyperRogue in a uniform way.

 *  In the hyperbolic plane, this is the Minkowski hyperboloid model:
 *  (x,y,z) such that x*x+y*y-z*z == -1 and z > 0.
 *
 *  In spherical geometry, we have x*x+y*y+z*z == 1.
 *
 *  In Euclidean geometry, we have z = 1.
 *
 *  In isotropic 3D geometries an extra coordinate is added.
 *
 *  In nonisotropic coordinates h[3] == 1.
 *
 *  In product geometries the 'z' coordinate is modelled by multiplying all 
 *  three coordinates with exp(z).
 *
 */

struct hyperpoint : array<ld, MAXMDIM> {
  hyperpoint() {}
  
  #if ISGCC46
  #if MAXMDIM == 4
  // aaa
  constexpr hyperpoint(ld x, ld y, ld z, ld w) : array<ld, MAXMDIM> { (array<ld, MAXMDIM>) {{x, y, z, w}}} {}
  #else
  constexpr hyperpoint(ld x, ld y, ld z, ld w) : array<ld, MAXMDIM> { (array<ld, MAXMDIM>) {{x, y, z}}} {}
  #endif

  #else
  #if MAXMDIM == 4
  constexpr hyperpoint(ld x, ld y, ld z, ld w) : array<ld, MAXMDIM> {{x,y,z,w}} {}
  #else
  constexpr hyperpoint(ld x, ld y, ld z, ld w) : array<ld, MAXMDIM> {{x,y,z}} {}
  #endif
  #endif

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

  inline friend hyperpoint operator - (hyperpoint h) { return h * -1; }

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

/** \brief A matrix acting on hr::hyperpoint 
 *  Since we are using homogeneous coordinates for hr::hyperpoint,
 *  rotations and translations can be represented
 *  as matrix multiplications. Other applications of matrices in HyperRogue 
 *  (in dimension up to 4) are also implemented using transmatrix.
 */
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

/** returns a diagonal matrix */
constexpr transmatrix diag(ld a, ld b, ld c, ld d) {
  #if MAXMDIM==3
  return transmatrix{{{a,0,0}, {0,b,0}, {0,0,c}}};
  #else
  return transmatrix{{{a,0,0,0}, {0,b,0,0}, {0,0,c,0}, {0,0,0,d}}};
  #endif
  }

constexpr hyperpoint Hypc = hyperpoint(0, 0, 0, 0);

/** identity matrix */
constexpr transmatrix Id = diag(1,1,1,1);

/** zero matrix */
constexpr transmatrix Zero = diag(0,0,0,0);

/** mirror image */
constexpr transmatrix Mirror = diag(1,-1,1,1);

/** mirror image: flip in the Y coordinate */
constexpr transmatrix MirrorY = diag(1,-1,1,1);

/** mirror image: flip in the X coordinate */
constexpr transmatrix MirrorX = diag(-1,1,1,1);

/** mirror image: flip in the Z coordinate */
constexpr transmatrix MirrorZ = diag(1,1,-1,1);

/** rotate by PI in the XY plane */
constexpr transmatrix pispin = diag(-1,-1,1,1);

/** central symmetry matrix */
constexpr transmatrix centralsym = diag(-1,-1,-1,-1);

inline hyperpoint hpxyz(ld x, ld y, ld z) { return MDIM == 3 ? hyperpoint(x,y,z,0) : hyperpoint(x,y,0,z); }
inline hyperpoint hpxyz3(ld x, ld y, ld z, ld w) { return MDIM == 3 ? hyperpoint(x,y,w,0) : hyperpoint(x,y,z,w); }
constexpr hyperpoint point3(ld x, ld y, ld z) { return hyperpoint(x,y,z,0); }
constexpr hyperpoint point31(ld x, ld y, ld z) { return hyperpoint(x,y,z,1); }
constexpr hyperpoint point2(ld x, ld y) { return hyperpoint(x,y,0,0); }

constexpr hyperpoint C02 = hyperpoint(0,0,1,0);
constexpr hyperpoint C03 = hyperpoint(0,0,0,1);

/** C0 is the origin in our space */
#define C0 (MDIM == 3 ? C02 : C03)
#endif

// basic functions and types
//===========================

#ifndef M_PI
#define M_PI 3.14159265358979
#endif

EX ld squar(ld x) { return x*x; }

EX int sig(int z) { return ginf[geometry].g.sig[z]; }

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

EX ld asin_clamp(ld x) { return x>1 ? M_PI/2 : x<-1 ? -M_PI/2 : std::isnan(x) ? 0 : asin(x); }

EX ld asin_auto_clamp(ld x) {
  switch(cgclass) {
    case gcEuclid: return x;
    case gcHyperbolic: return asinh(x);
    case gcSphere: return asin_clamp(x);
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

/** This function returns the length of the edge opposite the angle alpha in 
 *  a triangle with angles alpha, beta, gamma. This is called the cosine rule,
 *  and of course works only in non-Euclidean geometry. */ 
EX ld edge_of_triangle_with_angles(ld alpha, ld beta, ld gamma) {
  return acos_auto((cos(alpha) + cos(beta) * cos(gamma)) / (sin(beta) * sin(gamma)));
  }

EX hyperpoint hpxy(ld x, ld y) { 
  return hpxyz(x,y, sl2 ? sqrt(1+x*x+y*y) : translatable ? 1 : sphere ? sqrt(1-x*x-y*y) : sqrt(1+x*x+y*y));
  }

EX hyperpoint hpxy3(ld x, ld y, ld z) { 
  return hpxyz3(x,y,z, sl2 ? sqrt(1+x*x+y*y-z*z) :translatable ? 1 : sphere ? sqrt(1-x*x-y*y-z*z) : sqrt(1+x*x+y*y+z*z));
  }

#if HDR
// a point (I hope this number needs no comments ;) )
constexpr hyperpoint Cx12 = hyperpoint(1,0,1.41421356237,0);
constexpr hyperpoint Cx13 = hyperpoint(1,0,0,1.41421356237);

#define Cx1 (GDIM==2?Cx12:Cx13)
#endif

EX bool zero_d(int d, hyperpoint h) { 
  for(int i=0; i<d; i++) if(h[i]) return false;
  return true;
  }

/** this function returns approximate square of distance between two points
 *  (in the spherical analogy, this would be the distance in the 3D space,
 *  through the interior, not on the surface)
 *  also used to verify whether a point h1 is on the hyperbolic plane by using Hypc for h2
 */

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

/** square Euclidean hypotenuse in the first d dimensions */
EX ld sqhypot_d(int d, const hyperpoint& h) {
  ld sum = 0;
  for(int i=0; i<d; i++) sum += h[i]*h[i];
  return sum;
  }
  
/** Euclidean hypotenuse in the first d dimensions */
EX ld hypot_d(int d, const hyperpoint& h) {
  return sqrt(sqhypot_d(d, h));
  }
  
EX ld zlevel(const hyperpoint &h) {
  if(sl2) return sqrt(-intval(h, Hypc));
  else if(translatable) return h[LDIM];
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

/** normalize the homogeneous coordinates */
EX hyperpoint normalize(hyperpoint H) {
  if(prod) return H;
  ld Z = zlevel(H);
  for(int c=0; c<MDIM; c++) H[c] /= Z;
  return H;
  }

/** get the center of the line segment from H1 to H2 */
EX hyperpoint mid(const hyperpoint& H1, const hyperpoint& H2) {
  if(prod) {
    auto d1 = product_decompose(H1);
    auto d2 = product_decompose(H2);
    return zshift(PIU( mid(d1.second, d2.second) ), (d1.first + d2.first) / 2);
    }
  return normalize(H1 + H2);
  }

/** like mid, but take 3D into account */
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

/** rotate by alpha degrees in the coordinates a, b */
EX transmatrix cspin(int a, int b, ld alpha) {
  transmatrix T = Id;
  T[a][a] = +cos(alpha); T[a][b] = +sin(alpha);
  T[b][a] = -sin(alpha); T[b][b] = +cos(alpha);
  return T;
  }

/** rotate by alpha degrees in the XY plane */
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
  if(sl2) return slr::translate(slr::xyz_point(x, y, z));
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
  if(sl2) return slr::translate(h) * cpush0(2, z);
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

EX hyperpoint parabolic10(hyperpoint h) {
  if(euclid) { h[MDIM] = 1; return h; }
  else if(MDIM == 4) return hyperpoint(sinh(h[0]), h[1]/exp(h[0]), h[2]/exp(h[0]), cosh(h[0]));
  else return hyperpoint(sinh(h[0]), h[1]/exp(h[0]), cosh(h[0]), 0);
  }

EX hyperpoint deparabolic10(const hyperpoint h) {
  if(euclid) return h;
  ld x = -log(h[LDIM] - h[0]);
  if(MDIM == 3) return hyperpoint(x, h[1] * exp(x), 1, 0);
  return point31(x, h[1] * exp(x), h[2] * exp(x));
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

/** an Euclidean rotation in the axes (t,f) which rotates
 *  the point H to the positive 't' axis 
 */

EX transmatrix rspintoc(const hyperpoint& H, int t, int f) {
  transmatrix T = Id;
  ld R = hypot(H[f], H[t]);
  if(R >= 1e-12) {
    T[t][t] = +H[t]/R; T[t][f] = -H[f]/R;
    T[f][t] = +H[f]/R; T[f][f] = +H[t]/R;
    }
  return T;
  }

/** an isometry which takes the point H to the positive X axis 
 *  \see rspintox 
 */
EX transmatrix spintox(const hyperpoint& H) {
  if(GDIM == 2 || prod) return spintoc(H, 0, 1); 
  transmatrix T1 = spintoc(H, 0, 1);
  return spintoc(T1*H, 0, 2) * T1;
  }

/** inverse of hr::spintox 
 */
EX transmatrix rspintox(const hyperpoint& H) {
  if(GDIM == 2 || prod) return rspintoc(H, 0, 1); 
  transmatrix T1 = spintoc(H, 0, 1);
  return rspintoc(H, 0, 1) * rspintoc(T1*H, 0, 2);
  }

/** for H on the X axis, this matrix pushes H to C0 
 *  \see gpushxto0
 */

EX transmatrix pushxto0(const hyperpoint& H) {
  transmatrix T = Id;
  T[0][0] = +H[LDIM]; T[0][LDIM] = -H[0];
  T[LDIM][0] = curvature() * H[0]; T[LDIM][LDIM] = +H[LDIM];
  return T;
  }

/** set the i-th column of T to H */
EX void set_column(transmatrix& T, int i, const hyperpoint& H) {
  for(int j=0; j<MDIM; j++)
    T[j][i] = H[j];
  }

/** build a matrix using the given vectors as columns */
EX transmatrix build_matrix(hyperpoint h1, hyperpoint h2, hyperpoint h3, hyperpoint h4) {
  transmatrix T;
  for(int i=0; i<MDIM; i++)
    T[i][0] = h1[i],
    T[i][1] = h2[i],
    T[i][2] = h3[i];
  if(MAXMDIM == 4) for(int i=0; i<MDIM; i++) T[i][3] = h4[i];
  return T;
  }

/** for H on the X axis, this matrix pushes C0 to H
 *  \see rgpushxto0
 */

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

/** a translation matrix which takes H to 0 */
EX transmatrix gpushxto0(const hyperpoint& H) {
  return ggpushxto0(H, -1);
  }

/** a translation matrix which takes 0 to H */
EX transmatrix rgpushxto0(const hyperpoint& H) {
  return ggpushxto0(H, 1);
  }

/** \brief Fix the numerical inaccuracies in the isometry T
 *  The nature of hyperbolic geometry makes the computations numerically unstable.
 *  The numerical errors tend to accumulate, eventually destroying the projection.
 *  This function fixes this problem by replacing T with a 'correct' isometry.
 */

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

/** determinant */
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

/** warning about incorrect inverse */
void inverse_error(const transmatrix& T) {
  println(hlog, "Warning: inverting a singular matrix: ", T);
  }

/** inverse */
EX transmatrix inverse(const transmatrix& T) {
  if(MDIM == 3) {
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

/** distance from mh and 0 */
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
    case gcSL2: {
      auto cosh_r = hypot(mh[2], mh[3]);
      auto phi = atan2(mh[2], mh[3]);
      return hypot(cosh_r < 1 ? 0 : acosh(cosh_r), phi);
      }
    case gcNil: {
      ld bz = mh[0] * mh[1] / 2;
      return hypot(mh[0], mh[1]) + abs(mh[2] - bz);
      }
    default:
      return hypot_d(GDIM, mh);
    }
  }

/** length of a circle of radius r */
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

/* distance between h1 and h2 */
EX ld hdist(const hyperpoint& h1, const hyperpoint& h2) {
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
    case gcSL2: 
      return hdist0(inverse(slr::translate(h1)) * h2);
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

/** in 3D, an orthogonal projection of C0 on the given triangle */
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
  else if(sl2) return mscale(x, z);
  else if(prod) return mscale(x, z);
  else return mscale(x, z);
  }

EX hyperpoint hpxd(ld d, ld x, ld y, ld z) {
  hyperpoint H = hpxyz(d*x, d*y, z);
  H = mid(H, H);
  return H;
  }

EX ld signum(ld x) { return x<0?-1:x>0?1:0; }

EX bool asign(ld y1, ld y2) { return signum(y1) != signum(y2); }

EX ld xcross(ld x1, ld y1, ld x2, ld y2) { return x1 + (x2 - x1) * y1 / (y1 - y2); }

EX transmatrix parallel_transport(const transmatrix Position, const transmatrix& ori, const hyperpoint direction, int precision IS(100)) {
  if(nonisotropic) return nisot::parallel_transport(Position, direction);
  else if(prod) {
    hyperpoint h = product::direct_exp(ori * direction);
    return Position * rgpushxto0(h);
    }
  else return Position * rgpushxto0(direct_exp(direction, precision));
  }

EX void apply_parallel_transport(transmatrix& Position, const transmatrix orientation, const hyperpoint direction) {
  Position = parallel_transport(Position, orientation, direction);
  }

EX void rotate_object(transmatrix& Position, transmatrix& orientation, transmatrix R) {
  if(prod) orientation = orientation * R;
  else Position = Position * R;
  }

EX transmatrix spin_towards(const transmatrix Position, transmatrix& ori, const hyperpoint goal, int dir, int back) {
  transmatrix T;
  ld alpha = 0;
  if(nonisotropic && nisot::geodesic_movement)
    T = nisot::spin_towards(Position, goal);
  else { 
    hyperpoint U = inverse(Position) * goal;
    if(prod) {
      hyperpoint h = product::inverse_exp(U);
      alpha = asin_clamp(h[2] / hypot_d(3, h));
      U = product_decompose(U).second;
      }
    T = rspintox(U);
    }
  if(back < 0) T = T * spin(M_PI), alpha = -alpha;
  if(prod) {
    if(dir == 0) ori = cspin(2, 0, alpha);
    if(dir == 2) ori = cspin(2, 0, alpha - M_PI/2), dir = 0;
    }
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
namespace slr { 
  hyperpoint xyz_point(ld x, ld y, ld z); 
  hyperpoint polar(ld r, ld theta, ld phi); 
  }

inline hyperpoint cpush0(int c, ld x) { 
  hyperpoint h = Hypc;
  if(sl2) return slr::xyz_point(c==0?x:0, c==1?x:0, c==2?x:0);
  if(c == 2 && prod) {
    h[2] = exp(x);
    return h;
    }
  h[LDIM] = cos_auto(x);
  h[c] = sin_auto(x);
  return h;
  }

inline hyperpoint xspinpush0(ld alpha, ld x) { 
  if(sl2) return slr::polar(x, -alpha, 0);
  hyperpoint h = Hypc;
  h[LDIM] = cos_auto(x);
  h[0] = sin_auto(x) * cos(alpha);
  h[1] = sin_auto(x) * -sin(alpha);
  return h;
  }

inline hyperpoint xpush0(ld x) { return cpush0(0, x); }
inline hyperpoint ypush0(ld x) { return cpush0(1, x); }
inline hyperpoint zpush0(ld x) { return cpush0(2, x); }

/** T * C0, optimized */
inline hyperpoint tC0(const transmatrix &T) {
  hyperpoint z;
  for(int i=0; i<MDIM; i++) z[i] = T[i][LDIM];
  return z;
  }
#endif

/** tangent vector in the given direction */
EX hyperpoint ctangent(int c, ld x) { return point3(c==0?x:0, c==1?x:0, c==2?x:0); }

/** tangent vector in direction X */
EX hyperpoint xtangent(ld x) { return ctangent(0, x); }

/** tangent vector in direction Y */
EX hyperpoint ztangent(ld z) { return ctangent(2, z); }

/** change the length of the targent vector */
EX hyperpoint tangent_length(hyperpoint dir, ld length) {
  ld r = hypot_d(GDIM, dir);
  if(!r) return dir;
  return dir * (length / r);
  }

/** exponential function: follow the geodesic given by v */
EX hyperpoint direct_exp(hyperpoint v, int steps) {
  if(solnih) return nisot::numerical_exp(v, steps);
  if(nil) return nilv::formula_exp(v);
  if(sl2) return slr::formula_exp(v);
  if(prod) return product::direct_exp(v);
  ld d = hypot_d(GDIM, v);
  if(d > 0) for(int i=0; i<GDIM; i++) v[i] = v[i] * sin_auto(d) / d;
  v[LDIM] = cos_auto(d);
  return v;
  }

#if HDR
enum iePrecision { iLazy, iTable };
#endif
  
/** inverse exponential function \see hr::direct_exp */
EX hyperpoint inverse_exp(const hyperpoint h, iePrecision p, bool just_direction IS(true)) {
  #if CAP_SOLV
  if(solnih) {
    if(nih) 
      return solnihv::get_inverse_exp_nsym(h, p == iLazy, just_direction);
    else
      return solnihv::get_inverse_exp_symsol(h, p == iLazy, just_direction);
    }
  #endif
  if(nil) return nilv::get_inverse_exp(h, p == iLazy ? 5 : 20);
  if(sl2) return slr::get_inverse_exp(h);
  if(prod) return product::inverse_exp(h);
  ld d = acos_auto_clamp(h[GDIM]);
  hyperpoint v;
  if(d && sin_auto(d)) for(int i=0; i<GDIM; i++) v[i] = h[i] * d / sin_auto(d);
  v[3] = 0;
  return v;
  }

EX ld geo_dist(const hyperpoint h1, const hyperpoint h2, iePrecision p) {
  if(!nonisotropic) return hdist(h1, h2);
  return hypot_d(3, inverse_exp(inverse(nisot::translate(h1)) * h2, p, false));
  }

EX hyperpoint lp_iapply(const hyperpoint h) {
  return nisot::local_perspective_used() ? inverse(NLP) * h : h;
  }

EX hyperpoint lp_apply(const hyperpoint h) {
  return nisot::local_perspective_used() ? NLP * h : h;
  }

EX hyperpoint smalltangent() { return xtangent(.1); }

EX void cyclefix(ld& a, ld b) {
  while(a > b + M_PI) a -= 2 * M_PI;
  while(a < b - M_PI) a += 2 * M_PI;
  }

EX ld raddif(ld a, ld b) {
  ld d = a-b;
  if(d < 0) d = -d;
  if(d > 2*M_PI) d -= 2*M_PI;
  if(d > M_PI) d = 2 * M_PI-d;
  return d;
  }

EX int bucketer(ld x) {
  return int(x * 10 + 100000.5) - 100000;
  }

EX int bucketer(hyperpoint h) {
  return bucketer(h[0]) + 1000 * bucketer(h[1]) + 1000000 * bucketer(h[2]);
  }  

}
