/** representation based on the halfplane model; assumes Dim=3 */

namespace reps {

template<class F> struct sl2 : public array<F, 4> {

  sl2(F a, F b, F c, F d) { self[0] = a; self[1] = b; self[2] = c; self[3] = d; }
  
  sl2 operator * (const sl2& sec) const {
    return sl2(
      self[0] * sec[0] + self[1] * sec[2], 
      self[0] * sec[1] + self[1] * sec[3], 
      self[2] * sec[0] + self[3] * sec[2], 
      self[2] * sec[1] + self[3] * sec[3]
      );
    }

  std::string print() {
    return hr::lalign(0, "[", self[0], ",", self[1], ";", self[2], ",", self[3], "]");
    }

  };

TD sl2<typename D::Number> split_quaternion_to_sl2(const multivector<D>& h) {
  auto h3 = h[0], h2 = h[1 | 2], h1 = h[1 | 4], h0 = h[2 | 4];
  return sl2(h3 - h1, h2 + h0, -h2 + h0, h3 + h1);
  }

TD multivector<D> sl2_to_split_quaternion(const sl2<typename D::Number>& e) {
  auto h0 = (e[1] + e[2]) / 2;
  auto h3 = (e[0] + e[3]) / 2;
  auto h1 = (e[3] - e[0]) / 2;
  auto h2 = (e[1] - e[2]) / 2;
  auto res = zero_vector<multivector_data<D>>();
  res[0] = h3; res[1 | 2] = h2; res[1 | 4] = h1; res[2 | 4] = h0;
  return res;
  }

template<class N> using sl2c = sl2<std::complex<N>>;

TD sl2c<typename D::Number> split_biquaternion_to_sl2c(const multivector<D>& h) {
  using cn = std::complex<typename D::Number>;
  return sl2(cn(h[0]-h[9], h[15]-h[6]), cn(h[3]+h[10], -h[5]-h[12]), cn(h[10]-h[3], h[12]-h[5]), cn(h[0]+h[9], h[6]+h[15]));
  }

TD multivector<D> sl2c_to_split_biquaternion(const sl2c<typename D::Number>& e) {
  auto res = zero_vector<multivector_data<D>>();
  res[0]  = +(real(e[0]) + real(e[3])) / 2;
  res[3]  = +(real(e[1]) - real(e[2])) / 2;
  res[5]  = -(imag(e[1]) + imag(e[2])) / 2;
  res[6]  = +(imag(e[3]) - imag(e[0])) / 2;
  res[9]  = +(real(e[3]) - real(e[0])) / 2;
  res[10] = +(real(e[1]) + real(e[2])) / 2;
  res[12] = +(imag(e[2]) - imag(e[1])) / 2;
  res[15] = +(imag(e[0]) + imag(e[3])) / 2;
  return res;
  }

TD struct rep_halfplane {

  using data = D;
  using N = typename D::Number;
  using point = std::complex<N>;
  using isometry = sl2<N>;

  static isometry cspin(int i, int j, N alpha) {
    // return split_quaternion_to_sl2( rep_clifford<D>::cspin(i, j, alpha) );
    if(i>j) std::swap(i, j), alpha = -alpha; alpha /= 2;
    auto ca = cos(alpha), sa = sin(alpha);
    return isometry(ca, -sa, sa, ca);
    }
  static isometry cspin90(int i, int j, N alpha) {
    // return split_quaternion_to_sl2( rep_clifford<D>::cspin(i, j, alpha) );
    auto ca = sqrt(N(2)), sa = sqrt(N(2));
    if(i>j) std::swap(i, j), sa = -sa;
    return isometry(ca, -sa, sa, ca);
    }
  static isometry lorentz(int i, int j, N alpha) {
    // return split_quaternion_to_sl2( rep_clifford<D>::lorentz(i, j, alpha) );
    if(i>j) std::swap(i, j); alpha /= 2;
    if(i == 0) return isometry(exp(-alpha), N(0), N(0), exp(alpha));
    if(i == 1) {
      auto ca = cosh(alpha), sa = sinh(alpha);
      return isometry(ca, sa, sa, ca);
      }
    throw hr::hr_exception("bad lorentz");
    }
  static isometry id() { return isometry(N(1),N(0),N(0),N(1)); };
  static point center() { return point(N(0), N(1)); };
  static point apply(const isometry& T, const point& x) { 
    return (T[0] * x + T[1] * 1) / (T[2] * x + T[3] * 1);
    };
  static isometry apply(const isometry& T, const isometry& U) { return T * U; };

  static typename rep_clifford<D>::point to_poincare(const point& x) {
    auto a = real(x), b = imag(x);

    auto tmp = isometry(sqrt(b), a/sqrt(b), N(0), N(1)/sqrt(b));
    auto sq = sl2_to_split_quaternion<D>(tmp);

    // sq[0] = (sqrt(b) + 1/sqrt(b)) / 2;; sq[1 | 2] = a/sqrt(b)/2; sq[1 | 4] = (1/sqrt(b) - sqrt(b)) / 2; sq[2 | 4] = a/sqrt(b)/2;
   
    sq = despin(sq);
    return typename rep_clifford<D>::point({{sq}});
    }

  static isometry inverse(isometry T) { return isometry(T[3], -T[1], -T[2], T[0]); }
  static isometry push(const point& p) { return split_quaternion_to_sl2<D>(to_poincare(p)[0]); }

  static N dist0(const point& x) { return rep_clifford<D>::dist0(to_poincare(x)); } 
  static N angle(const point& x) { return rep_clifford<D>::angle(to_poincare(x)); }
  static N get_coord(const point& x, int i) { return rep_clifford<D>::get_coord(to_poincare(x), i); }

  // imag may be very small and still important, so do not use the default complex print
  static std::string print(const point& x) { return hr::lalign(0, "{real:", real(x), " imag:", imag(x), "}"); }
  static std::string print(const isometry& x) { return x.print(); }
  };

TD struct rep_halfspace {

  using data = D;
  using N = typename D::Number;
  struct point { std::complex<N> xy; N z; };
  using isometry = sl2c<N>;

  static isometry cspin(int i, int j, N alpha) {
    return split_biquaternion_to_sl2c( rep_clifford<D>::cspin(i, j, alpha) );
    }
  static isometry cspin90(int i, int j) {
    return split_biquaternion_to_sl2c( rep_clifford<D>::cspin90(i, j) );
    }
  static isometry lorentz(int i, int j, N alpha) {
    return split_biquaternion_to_sl2c( rep_clifford<D>::lorentz(i, j, alpha) );
    }
  static isometry id() { return isometry(N(1),N(0),N(0),N(1)); }
  static point center() { return point{ .xy = N(0), .z = N(1) }; }
  static point apply(const isometry& T, const point& x) { 
    auto nom = T[0] * x.xy + T[1] * N(1);
    auto nomz= T[0] * x.z;
    auto den = T[2] * x.xy + T[3] * N(1);
    auto denz= T[2] * x.z;
    // D = den + denz * j
    auto dnorm = std::norm(den) + std::norm(denz);
    using std::conj;
    // conj(D) = conj(den) - denz * j
    // N / D = (nom + nomz * j) / (den + denz * j) = 
    // = (nom + nomz * j) * (conj(den) - denz * j) / dnorm
    
    // auto rxy  = (nom * conj(den) - nomz * j * denz * j);
    // auto rz*j = (-nom * denz * j + nomz * j * conj(den))

    // apply the formula: j * a = conj(a) * j

    auto rxy = (nom * conj(den) + nomz * conj(denz));
    auto rz  = (nomz * den - nom * denz); // todo only real part
    // println(hlog, "imag of rz = ", imag(rz));
    return point { .xy = rxy / dnorm, .z = real(rz) / dnorm };
    };
  static isometry apply(const isometry& T, const isometry& U) { return T * U; };

  static typename rep_clifford<D>::point to_poincare(const point& x) {
    auto tmp = isometry(sqrt(x.z), x.xy/sqrt(x.z), N(0), N(1)/sqrt(x.z));
    auto sq = sl2c_to_split_biquaternion<D>(tmp);
    sq = despin(sq);
    return typename rep_clifford<D>::point({{sq}});
    }

  static isometry inverse(isometry T) { return isometry(T[3], -T[1], -T[2], T[0]); }
  static isometry push(const point& p) { return split_biquaternion_to_sl2c<D>(to_poincare(p)[0]); }

  static N dist0(const point& x) { return rep_clifford<D>::dist0(to_poincare(x)); } 
  static N angle(const point& x) { return rep_clifford<D>::angle(to_poincare(x)); }
  static N get_coord(const point& x, int i) { return rep_clifford<D>::get_coord(to_poincare(x), i); }

  // imag may be very small and still important, so do not use the default complex print
  static std::string print(const point& x) { return hr::lalign(0, "{x:", real(x.xy), " y:", imag(x.xy), " z:", x.z, "}"); }
  static std::string print(const isometry& x) { return x.print(); }
  };

template<class D> using rep_half = typename std::conditional<D::Dim==3, rep_halfplane<D>, rep_halfspace<D>>::type;

}
