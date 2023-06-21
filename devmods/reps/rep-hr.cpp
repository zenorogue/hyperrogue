namespace reps {

/* pull the HyperRogue representation; assumes HyperRogue geometry is set correctly, Number = ld, and Dim=3 or 4 */
TD struct rep_hr {

  using data = D;
  using N = typename D::Number;
  using point = hr::hyperpoint;
  using isometry = hr::transmatrix;

  static constexpr isometry cspin(int i, int j, N alpha) { return hr::cspin(i, j, ld(alpha)); }
  static constexpr isometry cspin90(int i, int j) { return hr::cspin90(i, j); }
  static constexpr isometry lorentz(int i, int j, N alpha) { return hr::lorentz(i, j, ld(alpha)); }
  static isometry id() { return hr::Id; };
  static point center() { return D::Dim == 4 ? hr::C03 : hr::C02; };
  static point apply(const isometry& T, const point& x) { return T * x; };
  static isometry apply(const isometry& T, const isometry& U) { return T * U; };
  static ld dist0(const point& x) { return hdist0(x); } 
  static ld angle(const point& x) { return atan2(x[1], x[0]); } 
  static ld get_coord(const point& x, int i) { return x[i]; }
  static isometry inverse(const isometry& T) { return iso_inverse(T); }
  static isometry push(const point& p) { return rgpushxto0(p); }

  static std::string print(point p) { return hr::lalign(0, p); }
  static std::string print(isometry p) { return hr::lalign(0, p); }
  };
}
