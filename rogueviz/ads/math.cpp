#include "../rogueviz.h"

namespace hr {

namespace ads_game {

static constexpr auto TAU = 2*M_PI;

/** hyperpoint represents a point in the SL(2,R)-like AdS, while ads_point represents a point in the universal cover */
struct ads_point : shiftpoint {
  ads_point(hyperpoint _h = C0, ld _s = 0) { h = _h; shift = _s; }
  ads_point(shiftpoint _s) : shiftpoint(_s) {}
  };

/** similarly, ads_matrix represents a transformation of the universal cover space */
struct ads_matrix : shiftmatrix {
  ads_matrix(transmatrix _h = Id, ld _s = 0) { T = _h; shift = _s; }
  ads_point operator* (ads_point) const;
  ads_matrix operator* (ads_matrix) const;
  ads_point operator* (const hyperpoint& h) const { return ads_point(T*h, shift); }
  ads_matrix operator* (const transmatrix& h) const { return ads_matrix(T*h, shift); }
  ads_matrix(shiftmatrix _s) : shiftmatrix(_s) {}
  };

ads_point kz(ads_point x) { x.h = hr::kz(x.h); x.shift = hr::kz(x.shift); return x; }
ads_matrix kz(ads_matrix x) { x.T = hr::kz(x.T); x.shift = hr::kz(x.shift); return x; }

/** Lorentz boost. */
transmatrix lorentz(int a, int b, ld v) {
  transmatrix T = Id;
  T[a][a] = T[b][b] = cosh(v);
  T[a][b] = T[b][a] = sinh(v);
  return T;
  }

void fixmatrix_ads(transmatrix& T) {
  for(int x=0; x<4; x++) for(int y=x; y>=0; y--) {
    ld dp = 0;
    for(int z=0; z<4; z++) dp += T[z][x] * T[z][y] * sig(z);
    
    if(y == x) dp = 1 - sqrt(sig(x)/dp);
    else dp *= sig(y);

    for(int z=0; z<4; z++) T[z][x] -= dp * T[z][y];
    }
  }

/* get_at(g) is at V; adjust g.second==0 and V accordingly */
void adjust_to_zero(ads_matrix& V, pair<cell*, int>& g, ld plev) {
  V.shift -= plev * g.second;
  g.second = 0;
  }

/** by how many cycles should we shift */
ld get_shift_cycles(ld shift) {
  return floor(shift / TAU + .5) * TAU;
  }

/** this is uzpush(-x) */
transmatrix chg_shift(ld x) {
  return cspin(2, 3, x) * cspin(0, 1, x);
  }

ads_point ads_matrix::operator*(ads_point h) const {
  auto& T = *this;
  optimize_shift(h);
  ld sh = get_shift_cycles(h.shift);
  h.shift -= sh;
  auto res0 = T;
  optimize_shift(res0);
  auto res1 = res0 * chg_shift(h.shift);
  optimize_shift(res1);
  res1.shift += get_shift_cycles(res0.shift - res1.shift);
  auto res2 = res1 * h.h;
  optimize_shift(res2);
  res2.shift += get_shift_cycles(res1.shift - res2.shift);
  res2.shift += sh;  
  return res2;
  }

ads_matrix ads_matrix::operator*(ads_matrix h) const {
  auto& T = *this;
  optimize_shift(h);
  ld sh = get_shift_cycles(h.shift);
  h.shift -= sh;

  auto res0 = T;
  optimize_shift(res0);
  auto res1 = res0 * chg_shift(h.shift);
  optimize_shift(res1);
  res1.shift += get_shift_cycles(res0.shift - res1.shift);
  auto res2 = res1 * h.T;
  optimize_shift(res2);
  res2.shift += get_shift_cycles(res1.shift - res2.shift);
  res2.shift += sh;
  return res2;
  }

ads_matrix ads_inverse(const ads_matrix& T) {
  ads_matrix res(inverse(unshift(T)), 0);
  ads_matrix m = res * T;
  optimize_shift(m);
  res.shift -= m.shift;
  return res;
  }

struct cross_result {
  hyperpoint h;
  ld shift;
  };

extern ads_matrix current;

/** T represents a worldline of some object; find when does this worldline cross the time=0 slice.
 *  shift is T's proper time at the point of crossing, and h=(x,y,z) is the Minkowski hyperboloid point where it crosses.
 **/

cross_result cross0(ads_matrix hz) {
  
  transmatrix deg90 = chg_shift(90*degree);
  hyperpoint uhz = unshift(hz * C0);
  hyperpoint uhz1 = unshift(hz * deg90 * C0);

  ld cost, sint, tant;
  ld t;
  
  if(uhz1[2]) {
    tant = - uhz[2] / uhz1[2];
    cost = 1 / sqrt(1 + tant * tant);
    sint = tant * cost;
    t = atan2(sint, cost);
    }
  else {
    cost = 0;
    sint = 1;
    t = 90*degree;
    }
  
  hyperpoint uhzt = unshift(hz * chg_shift(t) * C0);
  if(uhzt[3] < 0) { t += 180*degree; uhzt = -uhzt; }

  tie(uhzt[2], uhzt[3]) = make_pair(uhzt[3], -uhzt[2]);
  t += get_shift_cycles(-hz.shift-t);
  
  return cross_result{uhzt, t};
  }

/** Similar as cross0_light but for light-like wordlines.
 *  The point returned by cross0_light(T) is the same as the limit of cross0(T * lorentz(0, 2, v)).
 **/

cross_result cross0_light(ads_matrix hz) {
  transmatrix uhz = unshift(hz);
  ld t = uhz[2][3] / -(uhz[2][2] + uhz[2][0]);
  hyperpoint uhzt = uhz * hyperpoint(t, 0, t, 1);
  tie(uhzt[2], uhzt[3]) = make_pair(uhzt[3], -uhzt[2]);
  if(uhzt[2] < 0) uhzt = -uhzt;
  return cross_result{uhzt, t};
  }

/** sample from Poisson distribution */
int rpoisson(ld lambda) {
  ld prob = randd();
  ld poisson = exp(-lambda);
  int cnt = 0;
  while(cnt < 2*lambda+100) {
    if(prob < poisson) break;
    prob -= poisson;
    cnt++;
    poisson *= lambda / cnt;
    }
  return cnt;
  }

}
}
