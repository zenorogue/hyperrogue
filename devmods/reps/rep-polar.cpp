bool polar_mod = true, polar_choose = true;

namespace reps {

template<class N> static void cyclefix(N& a) {
  while(a > + get_deg<N>(180)) a -= get_deg<N>(360);
  while(a < - get_deg<N>(180)) a += get_deg<N>(360);
  }

template<class N> static N cyclefix_on(N a) { cyclefix(a); return a; }

/** the Taylor polynomial for 1-sqrt(1-y*y) */
template<class N> N ssqrt(N y) {
  return y*y/2 + y*y*y*y/8 + y*y*y*y*y*y*y/16 + y*y*y*y*y*y*y*y*y/128;
  }

TD struct rep_polar2 {

  using data = D;
  using N = typename D::Number;

  struct point { N phi, r; };
  struct isometry { N psi, phi, r; }; // spin by psi first

  static isometry cspin(int i, int j, N alpha) {
    if(i>j) std::swap(i, j), alpha = -alpha;
    return isometry{.psi = -alpha, .phi = N(0), .r = N(0) };
    }
  static isometry cspin90(int i, int j) { return cspin(i, j, get_deg<N>(90)); }
  static isometry lorentz(int i, int j, N alpha) {
    if(i>j) std::swap(i, j);
    if(i == 0) return isometry{.psi = N(0), .phi = N(0), .r = alpha};
    if(i == 1) return isometry{.psi = N(0), .phi = get_deg<N>(90), .r = alpha};
    throw hr::hr_exception("bad lorentz");
    }
  static isometry id() { return isometry{.psi = N(0), .phi = N(0), .r = N(0)}; };
  static point center() { return point{.phi = N(0), .r = N(0)}; };

  static std::string print(isometry T) {
    return hr::lalign(0, "{phi=", T.phi, " r=", T.r, " psi=", T.psi, "}");
    }

  static std::string print(point T) {
    return hr::lalign(0, "{phi=", T.phi, " r=", T.r, "}");
    }

  static isometry apply(isometry T, isometry U, bool need_psi = true) {
    if(T.r == 0) return isometry {.psi = T.psi+U.psi, .phi = T.psi+U.phi, .r = U.r};
    if(U.r == 0) return isometry {.psi = T.psi+U.psi, .phi = T.phi, .r = T.r};
    N alpha = U.phi + T.psi - T.phi;
    if(polar_mod) cyclefix(alpha);
    isometry res;
    N y1 = sinh(U.r) * sin(alpha);

    auto ca = cos(alpha);
    auto sa = sin(alpha);
    N x1, x2;

    // choose the appropriate method
    if(polar_choose && ca >= N(0.5)) {
      N u = ca >= N(.999999) ? ssqrt(sa) : N(1) - ca;
      res.r = cosh(T.r + U.r) - u * sinh(T.r) * sinh(U.r);
      x1 = sinh(T.r + U.r) - u * cosh(T.r) * sinh(U.r);
      if(need_psi) x2 = sinh(T.r + U.r) - u * cosh(U.r) * sinh(T.r);
      }
    else if(polar_choose && ca <= N(-0.5)) {
      N u = ca <= N(-.999999) ? ssqrt(-sa) : ca + N(1);
      res.r = cosh(T.r - U.r) + u * sinh(T.r) * sinh(U.r);
      x1 = sinh(T.r - U.r) + u * cosh(T.r) * sinh(U.r);
      if(need_psi) x2 = sinh(U.r - T.r) + u * cosh(U.r) * sinh(T.r);
      }
    else {
      res.r = sinh(T.r) * sinh(U.r) * ca + cosh(T.r) * cosh(U.r);
      x1 = cosh(T.r) * sinh(U.r) * ca + cosh(U.r) * sinh(T.r);
      if(need_psi) x2 = cosh(U.r) * sinh(T.r) * ca + cosh(T.r) * sinh(U.r);
      }

    if(res.r < N(1)) res.r = N(0); else res.r = acosh(res.r);

    N beta  = (y1 || x1) ? atan2(y1, x1) : N(0);
    res.phi = T.phi + beta;
    if(polar_mod) cyclefix(res.phi);

    if(need_psi) {
      N y2 = sinh(T.r) * sin(alpha);
      N gamma = (y2 || x2) ? atan2(y2, x2) : N(0);
      res.psi = T.psi + U.psi + beta + gamma - alpha;
      if(polar_mod) cyclefix(res.psi);
      }

    return res;
    };

  static point apply(const isometry& T, const point& x) { 
    isometry x1 = apply(T, push(x), false);
    return point { .phi = x1.phi, .r = x1.r};
    };

  static isometry inverse(isometry T) { return isometry{.psi = -T.psi, .phi = cyclefix_on<N>(get_deg<N>(180)+T.phi-T.psi), .r=T.r }; };
  static isometry push(const point& p) { return isometry{.psi = N(0), .phi = p.phi, .r = p.r}; }

  static N dist0(const point& x) { return x.r; } 
  static N angle(const point& x) { return x.phi; }
  static N get_coord(const point& x, int i) { 
    if(i == 0) return cos(x.phi) * sinh(x.r);
    if(i == 1) return sin(x.phi) * sinh(x.r);
    if(i == 2) return cosh(x.r);
    throw hr::hr_exception("bad get_coord");
    }
  };

TD struct rep_high_polar {

  using data = D;
  using N = typename D::Number;

  struct sphere_data {
    using Number  = N;
    static constexpr int Dim = D::Dim-1;
    static constexpr int Flipped = -1;
    };

  using subsphere = rep_linear_nn<sphere_data>;

  struct point { typename subsphere::point phi; N r; };
  struct isometry { typename subsphere::isometry psi; typename subsphere::point phi; N r; };

  static isometry cspin(int i, int j, N alpha) {
    return isometry{.psi = subsphere::cspin(i, j, alpha), .phi = subsphere::center(), .r = N(0) };
    }
  static isometry cspin90(int i, int j) {
    return isometry{.psi = subsphere::cspin90(i, j), .phi = subsphere::center(), .r = N(0) };
    }
  static isometry lorentz(int i, int j, N alpha) {
    if(i>j) std::swap(i, j);
    auto is = isometry{.psi = subsphere::id(), .phi = subsphere::center(), .r = alpha};
    is.phi[D::Dim-2] = N(0);
    is.phi[i] = N(1);
    return is;
    }
  static isometry id() { return isometry{.psi = subsphere::id(), .phi = subsphere::center(), .r = N(0)}; }
  static point center() { return point{.phi = subsphere::center(), .r = N(0)}; };

  static std::string print(isometry T) {
    return hr::lalign(0, "{phi=", subsphere::print(T.phi), " r=", T.r, " psi=", hr::kz(T.psi.values), "}");
    }

  static std::string print(point T) {
    return hr::lalign(0, "{phi=", subsphere::print(T.phi), " r=", T.r, "}");
    }

  static isometry apply(isometry T, isometry U, bool need_psi = true) {
    auto apsi = need_psi ? T.psi * U.psi : subsphere::id();
    if(T.r == 0) return isometry {.psi = apsi, .phi = T.psi*U.phi, .r = U.r};
    if(U.r == 0) return isometry {.psi = apsi, .phi = T.phi, .r = T.r};
    auto aphi = T.psi * U.phi;
    auto cos_alpha = inner<sphere_data>(aphi, T.phi);
    auto& ca = cos_alpha;

    isometry res;

    N x1, x2;

    auto orth = (aphi - T.phi * ca);
    N sin_alpha;
    if(ca > N(0.999999) || ca < N(0.999999))
      sin_alpha = pow(sqnorm<sphere_data>(orth), .5);
    else
      sin_alpha = pow(N(1) - ca * ca, .5);

    if(sin_alpha == N(0)) {
      if(ca >= N(1)) {
        return isometry{.psi = apsi, .phi = T.phi, .r = T.r + U.r };
        }
      if(ca <= N(-1)) {
        if(T.r >= U.r) {
          return isometry{.psi = apsi, .phi = T.phi, .r = T.r - U.r };
          }
        else {
          return isometry{.psi = apsi, .phi = T.phi*-1, .r = U.r - T.r };
          }
        }
      }

    orth = orth / sin_alpha;
    N y1 = sinh(U.r) * sin_alpha;

    // choose the appropriate method
    if(polar_choose && ca >= N(0.5)) {
      N u = ca >= N(.999999) ? ssqrt(sin_alpha) : N(1) - ca;
      res.r = cosh(T.r + U.r) - u * sinh(T.r) * sinh(U.r);
      x1 = sinh(T.r + U.r) - u * cosh(T.r) * sinh(U.r);
      if(need_psi) x2 = sinh(T.r + U.r) - u * cosh(U.r) * sinh(T.r);
      }
    else if(polar_choose && ca <= N(-0.5)) {
      N u = ca <= N(-.999999) ? ssqrt(sin_alpha) : ca + N(1); // ca = u - 1
      res.r = cosh(T.r - U.r) + u * sinh(T.r) * sinh(U.r);
      x1 = sinh(T.r - U.r) + u * cosh(T.r) * sinh(U.r);
      if(need_psi) x2 = sinh(U.r - T.r) + u * cosh(U.r) * sinh(T.r);
      }
    else {
      res.r = sinh(T.r) * sinh(U.r) * ca + cosh(T.r) * cosh(U.r);
      x1 = cosh(T.r) * sinh(U.r) * ca + cosh(U.r) * sinh(T.r);
      if(need_psi) x2 = cosh(U.r) * sinh(T.r) * ca + cosh(T.r) * sinh(U.r);
      }

    if(res.r < N(1)) res.r = N(0); else res.r = acosh(res.r);

    auto h1 = pow(x1*x1+y1*y1, -0.5);
    N cos_beta = x1*h1, sin_beta = y1*h1;

    res.phi = T.phi * cos_beta + orth * sin_beta;

    if(need_psi) {
      N y2 = sinh(T.r) * sin_alpha;
      auto h2 = pow(x2*x2+y2*y2, -0.5);
      N cos_gamma = x2*h2, sin_gamma = y2*h2;

      // delta = beta + gamma - alpha

      auto cos_beta_gamma = cos_beta * cos_gamma - sin_beta * sin_gamma;
      auto sin_beta_gamma = cos_beta * sin_gamma + sin_beta * cos_gamma;

      auto cos_delta = cos_beta_gamma * cos_alpha + sin_beta_gamma * sin_alpha;
      auto sin_delta = sin_beta_gamma * cos_alpha - cos_beta_gamma * sin_alpha;

      auto phi1  = T.phi  * cos_delta + orth  * sin_delta;
      auto orth1 = orth   * cos_delta - T.phi * sin_delta;

      auto phi2  = phi1  - T.phi;
      auto orth2 = orth1 - orth;
      typename subsphere::isometry spinner = subsphere::id();

      // Tv = v + <v, phi> * (phi'-phi) + <v, orth> * (orth'-orth)

      for(int i=0; i<D::Dim-1; i++)
      for(int j=0; j<D::Dim-1; j++)
        spinner[i][j] += phi2[i] * T.phi[j] + orth2[i] * orth[j];

      res.psi = spinner * apsi;
      }

    return res;
    };

  static point apply(const isometry& T, const point& x) { 
    isometry x1 = apply(T, push(x), false);
    return point { .phi = x1.phi, .r = x1.r};
    };

  static isometry inverse(isometry T) { return isometry{.psi = subsphere::inverse(T.psi), .phi = subsphere::inverse(T.psi)*T.phi*-1, .r=T.r }; };
  static isometry push(const point& p) { return isometry{.psi = subsphere::id(), .phi = p.phi, .r = p.r}; }

  static N dist0(const point& x) { return x.r; } 
  static N angle(const point& x) { return subsphere::angle(x.phi); }
  static N get_coord(const point& x, int i) { if(i == D::Dim-1) return cosh(x.r); else return x.phi[i] * sinh(x.r); }
  };

template<class D> using rep_polar = typename std::conditional<D::Dim==3, rep_polar2<D>, rep_high_polar<D>>::type;

}
