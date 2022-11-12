#include "rogueviz.h"

namespace hr {

namespace extra {

template<class T> void makeband_complex(shiftpoint H, hyperpoint& ret, const T& f) {
  makeband_f(H, ret, [&] (ld& x, ld& y) {
    if(euclid) return;        
    if(isnan(x)) return;

    // auto orx = x, ory = y;
                
    cld i = hyperbolic ? cld(0,1) : cld(1, 0);        
    cld cx = x*i;
    cld cy = y*i;
    
    f(cx, cy);
    
    x = real(cx/i) + (anyshiftclick ? 1 : 0) * imag(cx/i);
    y = real(cy/i) + (anyshiftclick ? 1 : 0) * imag(cy/i);
    });
  }

template<class T> void add_complex(const char *name, flagtype flag, const T& f) {
  int q = isize(mdinf);
  mdinf.emplace_back(modelinfo{name, name, name, mf::euc_boring | mf::broken | flag, 0, 0, 0, 0, 0, nullptr});
  while(isize(extra_projections) < q) extra_projections.emplace_back();
  extra_projections.emplace_back([f] (shiftpoint& H_orig, hyperpoint& H, hyperpoint& ret) {
    makeband_complex(H_orig, ret, f);
    });
  }

template<class T> void add_band(const char *name, flagtype flag, const T& f) {
  int q = isize(mdinf);
  mdinf.emplace_back(modelinfo{name, name, name, mf::euc_boring | mf::broken | flag, 0, 0, 0, 0, 0, nullptr});
  while(isize(extra_projections) < q) extra_projections.emplace_back();
  extra_projections.emplace_back([f] (shiftpoint& H_orig, hyperpoint& H, hyperpoint& ret) {
    makeband_f(H_orig, ret, f);
    });
  }

template<class T1, class T2> cld newton_inverse(const T1& f, const T2& fp, cld yf, cld x0) {
  cld x = x0;
  for(int it=0;; it++) {
    cld y = f(x);
    cld yp = fp(x);
    x = x + (yf - y) / yp;
    if(abs(y-yf) < 1e-9) return x;
    if(it == 20) {
      println(hlog, "failed for: ", yf, " x=", x, " y=", y);
      return x;
      }
    }
  }

void add_extra_projections() {
  // does not work in H3...
  add_complex("van der Grinten", 0, [] (cld& x, cld& y) {
      
     if(abs(y) < 1e-4) return;
     
     bool ox = abs(x) < 1e-4;
             
     if(x == 0.) x = 1e-6;
     
     cld sx = real(x)+imag(x) > 0 ? 1 : -1;
     cld sy = real(y)+imag(y) > 0 ? 1 : -1;
     x /= sx;
     y /= sy;
     
     auto pi = M_PI;
     
     cld sin_theta = 2. * y / pi;
     cld cos_theta2 = 1. - sin_theta * sin_theta;
     cld A = (1/2.) * (pi / x - x / pi);
     
     cld G = sqrt(cos_theta2) / (sin_theta + sqrt(cos_theta2) - 1.);
     cld P = G * (2./sin_theta - 1.);
     cld Q = A*A + G;
     
     cld diag = A*A+P*P;
     cld s1 = A*A*(G-P*P)*(G-P*P) - diag*(G*G-P*P);
     cld s2 = (A*A+1.)*diag - Q*Q;

     if(ox) { 
       x = 0;
       cld theta = asin(sin_theta);
       y = sy * M_PI * tan(theta/2.);
       }
     
     else {
       x = sx * M_PI * (A * (G-P*P) + sqrt(s1)) / diag;
       y = sy * M_PI * (P*Q - (hyperbolic?-1.:1.) * A*sqrt(s2)) / diag;
       }
     });

  // https://en.wikipedia.org/wiki/Eckert_II_projection
  add_band("Eckert II", mf::pseudoband | mf::equiarea, [] (ld& x, ld& y) {
    ld sy = y > 0 ? 1 : -1;
    y /= sy;
    ld z = 4. - 3. * (hyperbolic ? -sinh(y) : sin(y));
    x = 2. * x * sqrt(z / 1080._deg);
    y = sy * sqrt(120._deg) * (2. - sqrt(z));
    });
  
  // https://en.wikipedia.org/wiki/Eckert_IV_projection
  add_complex("Eckert IV", mf::pseudoband | mf::equiarea, [] (cld& x, cld& y) {
    cld theta = newton_inverse(
      [] (cld th) { return th + sin(th) * cos(th) + 2. * sin(th); }, 
      [] (cld th) { return 1. + cos(th) * cos(th) - sin(th) * sin(th) + 2. * cos(th); }, 
      (2+90._deg) * sin(y), y);
    static ld cox = 2 / sqrt(4*M_PI+M_PI*M_PI);
    static ld coy = 2 * sqrt(M_PI/(4+M_PI));
    x = cox * x * (1. + cos(theta));
    y = coy * sin(theta);
    });

  // does not work in H3...
  add_complex("Ortelius", 0, [] (cld& x, cld& y) {
    cld sx = (real(x)+imag(x)) > 0 ? 1 : -1;
    x /= sx;
    if(abs(real(x)) < 90._deg) {
      cld F = M_PI*M_PI / 8. / x + x / 2.;
      x = (x - F + sqrt(F*F-y*y));
      }
    else {
      x = sqrt(M_PI*M_PI/4 - y*y) + x - 90._deg;
      }
    x *= sx;
    });
  
  // https://en.wikipedia.org/wiki/Equal_Earth_projection
  
  add_complex("Equal Earth", mf::equiarea | mf::pseudoband, [] (cld& x, cld& y) {
    static cld M = sqrt(3)/2;
    auto theta = asin(M * sin(y));
    ld A1 = 1.340624;
    ld A2 = -0.081106;
    ld A3 = 0.000893;
    ld A4 = 0.003796;
    cld pows[10];
    pows[1] = theta; for(int i=2; i<10; i++) pows[i] = pows[i-1] * theta;
    x = x*cos(theta) / M / (9*A4*pows[8] + 7*A3*pows[6] + 3*A2*pows[2] + A1);
    y = A4 * pows[9] + A3 * pows[7] + A2 * pows[3] + A1 * pows[1];
    });

  // https://en.wikipedia.org/wiki/Natural_Earth_projection
  
  add_complex("Natural Earth", mf::pseudoband, [] (cld& x, cld& y) {
    cld pows[13];
    pows[1] = y; for(int i=2; i<13; i++) pows[i] = pows[i-1] * y;
    cld l = 0.870700 - 0.131979 * pows[2] - 0.013791 * pows[4] + 0.003971 * pows[10] - 0.001529 * pows[12];
    y = y * (1.007226 + 0.015085 * pows[2] - 0.044475 * pows[6] + 0.028874 * pows[8] - 0.005916 * pows[10]);
    x = x * l;
    });      

  // https://en.wikipedia.org/wiki/Wagner_VI_projection
  add_complex("Wagner VI", mf::equiarea | mf::pseudoband, [] (cld& x, cld& y) {
    x = x * sqrt(1. - 3. * pow(y/M_PI, 2));
    });      

  /* does the Poincare model work in spherical? -- hint: it does not, as expected */
  if(0) add_complex("alt poincare", mf::equiarea | mf::pseudoband, [] (cld& x, cld& y) {
    cld i(0, 1);
    x /= i;
    y /= i;
    cld c1(1, 0);
    auto ax = cosh(y) * sinh(x);
    auto ay = sinh(y);
    auto az = cosh(x) * cosh(y);
    ax /= (az+c1);
    ay /= (az+c1);
    ay += c1;
    cld z = ax*ax + ay*ay;
    ax /= z;
    ay /= z;
    ay -= c1;
    ax *= i;
    ay *= i;
    x = ax;
    y = ay;
    });
  }

void gen_dual() {
  int q = isize(mdinf);
  eModel p = pmodel;
  auto& mo=  mdinf[p];
  mdinf.push_back(mo);
  auto& m = mdinf.back();
  m.name_hyperbolic = strdup((string("dual to ") + mo.name_spherical).c_str());
  m.name_euclidean = strdup((string("dual to ") + mo.name_euclidean).c_str());
  m.name_spherical = strdup((string("dual to ") + mo.name_hyperbolic).c_str());
  while(isize(extra_projections) < q) extra_projections.emplace_back();
  extra_projections.emplace_back([p] (shiftpoint& H_orig, hyperpoint& H, hyperpoint& ret) {
    if(hyperbolic) {
      auto Hdual = H_orig;
      auto& H1 = Hdual.h;
      H1 /= H1[2];
      H1[2] = sqrt(1 - H1[0] * H1[0] - H1[1] * H1[1]);
      dynamicval<eGeometry> g(geometry, gSphere);
      apply_other_model(Hdual, ret, p);
      }
    else if(sphere) {
      auto Hdual = H_orig;
      auto& H1 = Hdual.h;
      H1 /= H1[2];
      H1[2] = sqrt(1 + H1[0] * H1[0] + H1[1] * H1[1]);
      dynamicval<eGeometry> g(geometry, gNormal);
      apply_other_model(Hdual, ret, p);
      }
    else
      apply_other_model(H_orig, ret, p);
    });
  pmodel = eModel(q);
  }

int ar = addHook(hooks_initialize, 100, add_extra_projections)
  + arg::add3("-gen-dual", gen_dual);

}
}
