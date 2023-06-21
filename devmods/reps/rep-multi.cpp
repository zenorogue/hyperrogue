namespace reps {

TD typename D::Number acos_auto(typename D::Number x) {
  using N = typename D::Number;
  if(hyperbolic) {
    if(x < N(1)) return N(0);
    return acosh(x);
    }
  if(sphere) {
    if(x > N(1)) return N(0);
    return acos(x);
    }
  throw hr::hr_exception("error");
  }

/* use the linear representation, as in HyperRogue, but DO NOT apply nm, for comparison */
TD struct rep_linear_nn {

  using data = D;
  using point = mvector<data>;
  using isometry = matrix<data>;
  using N = typename D::Number;

  static constexpr isometry id() { 
    matrix<D> result;
    for(int i=0; i<D::Dim; i++)
    for(int j=0; j<D::Dim; j++)
      result[i][j] = N(i == j);
    return result;
    };

  static constexpr isometry cspin(int i, int j, typename D::Number angle) {
    auto res = id();
    auto ca = cos(angle), sa = sin(angle);
    res[i][i] = ca;
    res[j][j] = ca;
    res[i][j] = sa;
    res[j][i] = -sa;
    return res;
    };

  static constexpr isometry cspin90(int i, int j) {
    auto res = id();
    res[i][i] = 0;
    res[j][j] = 0;
    res[i][j] = 1;
    res[j][i] = -1;
    return res;
    };

  static constexpr isometry lorentz(int i, int j, typename D::Number angle) {
    auto res = id();
    auto ca = cosh(angle), sa = sinh(angle);
    res[i][i] = ca;
    res[j][j] = ca;
    res[i][j] = sa;
    res[j][i] = sa;
    return res;
    }

  static constexpr point center() { return unit_vector<data>(D::Dim-1); }
  static point apply(const isometry& T, const point& x) { return T * x; };
  static isometry apply(const isometry& T, const isometry& U) { return T * U; };
  static typename D::Number dist0(point x) { 
    return acos_auto<D> (x[D::Dim-1]);
    } 
  static typename D::Number angle(const point& x) { return atan2(x[1], x[0]); } 
  static typename D::Number get_coord(point x, int i) { return x[i]; }

  static isometry inverse(isometry T) { 
    for(int i=0; i<D::Dim; i++)
    for(int j=0; j<i; j++) std::swap(T[i][j], T[j][i]);
    if constexpr(D::Flipped != -1) {
      for(int i=0; i<D::Dim-1; i++) T[i][D::Dim-1] = -T[i][D::Dim-1];
      for(int i=0; i<D::Dim-1; i++) T[D::Dim-1][i] = -T[D::Dim-1][i];
      }
    return T;
    }
  static isometry push(const point& p) {
    auto res = id();
    // to do: for spherical!
    N fac = N(1)/(p[D::Dim-1]+N(1));
    for(int i=0; i<D::Dim-1; i++)
    for(int j=0; j<D::Dim-1; j++)
      res[i][j] += p[i] * p[j] * fac;
      
    for(int d=0; d<D::Dim-1; d++)
      res[d][D::Dim-1] = p[d],
      res[D::Dim-1][d] = p[d];
    res[D::Dim-1][D::Dim-1] = p[D::Dim-1];
    
    return res;
    }

  static std::string print(point p) { return nzv(p); }
  static std::string print(isometry p) { return nzv(p); }
  };

TD mvector<D> get_column(matrix<D> a, int id) {
  mvector<D> tmp;
  for(int i=0; i<D::Dim; i++) tmp[i] = a[i][id];
  return tmp;
  }

TD typename D::Number inner(mvector<D> a, mvector<D> b) {
  using N = typename D::Number;
  N res(0);
  for(int i=0; i<D::Dim; i++) res += a[i] * b[i] * (i==D::Flipped?-1:1);
  if(isnan(res) || isinf(res)) return N(0);
  return res;
  }

TD void set_column(matrix<D>& a, int id, mvector<D> v) {
  for(int i=0; i<D::Dim; i++) a[i][id] = v[i];
  }

TD typename D::Number sqnorm(matrix<D> a) { return sqnorm<D>(get_column<D>(a, D::Dim-1)); }

bool fix_matrices;

TD matrix<D> apply_nm(matrix<D> a) {
  using N = typename D::Number;
  // normalize first
  auto& lead = a[D::Dim-1][D::Dim-1];
  if(nm == nmFlatten) a = a / lead, cbc[cbcDiv]--;
  if(nm == nmForced || nm == nmWeak) a = a * pow(sqnorm<D>(a), -0.5);
  if(nm == nmBinary) { 
    while(lead >= 2 && !isinf(lead)) { a = a / 2; } while(lead > 0 && lead < 0.5) { a = a * 2; } 
    }
  // fixmatrix later
  if(!fix_matrices) return a;
  auto divby = (nm == nmBinary || nm == nmWeak || nm == nmCareless || nm == nmFlatten) ? sqnorm<D>(a) : N(1);
  for(int i=D::Dim-2; i>=0; i--) {
    auto ro = get_column(a, i);
    auto last = get_column(a, D::Dim-1);
    ro = ro + last * inner(ro, last) / divby;
    for(int j=i+1; j<D::Dim-1; j++) {
      auto next = get_column(a, j);
      ro = ro - next * inner(ro, next) / divby;
      }
    auto in = inner(ro, ro);
    if(in > N(0)) ro = ro * (pow(in*in, -.5) * divby);
    set_column(a, i, ro);
    }
  return a;
  }

/* use the linear representation, as in HyperRogue */
TD struct rep_linear {

  using data = D;
  using point = mvector<data>;
  using isometry = matrix<data>;
  using N = typename D::Number;

  static constexpr isometry cspin(int i, int j, typename D::Number angle) {
    return apply_nm<D>( rep_linear_nn<D>::cspin(i, j, angle) );
    }
  static constexpr isometry cspin90(int i, int j) {
    return rep_linear_nn<D>::cspin90(i, j);
    }
  static constexpr isometry lorentz(int i, int j, typename D::Number angle) {
    return apply_nm<D>( rep_linear_nn<D>::lorentz(i, j, angle) );
    }
  static isometry id() { return rep_linear_nn<D>::id(); };
  static constexpr point center() { return unit_vector<data>(D::Dim-1); }
  static point apply(const isometry& T, const point& x) { return apply_nm(T * x); };
  static isometry apply(const isometry& T, const isometry& U) { return apply_nm(T * U); };
  static typename D::Number dist0(point x) { 
    return acos_auto<D> (get_normalized(x, x[D::Dim-1]));
    } 
  static typename D::Number angle(const point& x) { return atan2(x[1], x[0]); } 
  static typename D::Number get_coord(point x, int i) { 
    return get_normalized(x, x[i]); }

  static isometry inverse(isometry T) {
    return rep_linear_nn<D>::inverse(T);
    }
  static isometry push(const point& p) {
    return apply_nm( rep_linear_nn<D>::push(get_normalized(p, p)) );
    }

  static std::string print(point p) { return nzv(p); }
  static std::string print(isometry p) { return nzv(p); }
  };

/* use the linear representation of points and the multivector representation of isometries */

TD struct rep_mixed {

  using data = D;
  using N = typename D::Number;

  using point = mvector<data>;
  using isometry = multivector<data>;

  static isometry cspin(int i, int j, typename data::Number alpha, bool noflat = false) {
/*    auto u = unit_vector<multivector_data<data>> (0);
    auto ui = unit_vector<data> (i);
    auto uj = unit_vector<data> (j);
    return u * cos(alpha/2) + multimul(embed(ui), embed(uj)) * sin(alpha/2); */
    auto res = zero_vector<multivector_data<data>> ();
    if(nm == nmFlatten && !noflat) {
      res[0] = N(1);
      res[(1<<i) | (1<<j)] = tan(alpha/2) * (i > j ? 1 : -1); 
      return res;
      }
    res[0] = cos(alpha/2);
    res[(1<<i) | (1<<j)] = sin(alpha/2) * (i > j ? 1 : -1); 
    return res;
    }

  static isometry cspin90(int i, int j, bool noflat = false) {
    auto res = zero_vector<multivector_data<data>> ();
    if(nm == nmFlatten && !noflat) {
      res[0] = N(1);
      res[(1<<i) | (1<<j)] = N(i > j ? 1 : -1); 
      return res;
      }
    res[0] = sqrt(N(.5));
    res[(1<<i) | (1<<j)] = sqrt(N(.5)) * (i > j ? 1 : -1); 
    return res;
    }

  static isometry lorentz(int i, int j, typename data::Number alpha) {
/*  // j must be time coordinate
    auto u = unit_vector<multivector_data<data>> (0);
    auto ui = unit_vector<data> (i);
    auto uj = unit_vector<data> (j);
    return u * cosh(alpha/2) + multimul(embed(uj), embed(ui)) * sinh(alpha/2); */
    auto res = zero_vector<multivector_data<data>> ();
    if(nm == nmFlatten) {
      res[0] = N(1);
      res[(1<<i) | (1<<j)] = tanh(alpha/2);
      return res; 
      }
    res[0] = cosh(alpha/2);
    res[(1<<i) | (1<<j)] = sinh(alpha/2);
    return res; 
    }
  static isometry id() { return unit_vector<multivector_data<data>> (0); };
  static constexpr point center() { return unit_vector<data>(D::Dim-1); }
  static point apply(const isometry& T, const point& x) {
    // return unembed(multimul(multimul(T, embed(x)), conjugate(T)));
    return apply_nm(unembed(chkmul<odd<D>,flat_even<D>,underling<D>>(chkmul<flat_even<D>,flat_underling<D>,odd<D>>(T, embed(x)), conjugate(T))));
    };
  static isometry apply(const isometry& T, const isometry& U) { 
    auto res = apply_nm<even<D>, D>(chkmul<flat_even<D>,flat_even<D>,even<D>>(T, U));
    return res;
    }

  static isometry inverse(isometry T) { return conjugate(T); }
  static isometry push(const point& p) {
    auto pm = get_normalized(p, p);
    pm[D::Dim-1] = pm[D::Dim-1] + N(1);
    // since p was normalized, sqnorm of pm is 2 * pm[D::Dim-1]
    pm = pm * pow(2 * pm[D::Dim-1], -0.5);
    multivector<data> v1 = embed(pm);
    multivector<data> v2 = unit_vector<multivector_data<data>>(1<<(D::Dim-1));
    multivector<data> v3 = chkmul<underling<D>,underling<D>,poincare<D>>(v1, v2);
    v3 = apply_nm<poincare<D>, D>(v3);
    return v3;
    }

  static typename D::Number dist0(point x) { return acos_auto<D> (get_normalized(x, x[D::Dim-1])); } 
  static typename D::Number angle(const point& x) { return atan2(x[1], x[0]); } 
  static typename D::Number get_coord(point x, int i) { return get_normalized(x, x[i]); }

  static std::string print(point p) { return nzv(p); }
  static std::string print(isometry p) { return nz(p); }
  };

/* use the hyperboloid-Poincare representation of points and the multivector representation of isometries */

TD struct rep_clifford {

  using data = D;
  using N = typename D::Number;

  using point = array< multivector<data>, 1>;
  using isometry = multivector<data>;

  static isometry cspin(int i, int j, typename data::Number alpha) { return rep_mixed<D>::cspin(i, j, alpha); }
  static isometry cspin90(int i, int j) { return rep_mixed<D>::cspin90(i, j); }
  // j must be the neg coordinate!
  static isometry lorentz(int i, int j, N alpha) { return rep_mixed<D>::lorentz(i, j, alpha); }
  static isometry id() { return rep_mixed<D>::id(); }
  static constexpr point center() { return point{{ id() }}; }
  static point apply(const isometry& T, const point& x) { return point{{ despin(chkmul<even<D>,poincare<D>,even<D>>(T, x[0])) }}; }
  static isometry apply(const isometry& T, const isometry& U) { return apply_nm<even<D>,D>( chkmul<even<D>,even<D>,even<D>>(T, U) ); }

  static isometry inverse(isometry T) { return conjugate(T); }
  static isometry push(const point& p) { return p[0]; }

  static typename D::Number dist0(const point& ax) { 
    return acos_auto<D>(get_normalized<poincare<D>, D, N>(ax[0], ax[0][0]))*2;
    } 
  static constexpr int mvlast = 1<<(D::Dim-1);
  static typename D::Number angle(const point& x) { 
     return atan2(x[0][2 | mvlast], x[0][1 | mvlast]); 
     } 
  static typename D::Number get_coord(const point& x, int i) {
    auto x1 = multimul(multimul(x[0], unit_vector<multivector_data<data>> (mvlast)), conjugate(x[0])); 
    auto x2 = unembed(x1);
    return get_normalized(x2, x2[i]);
    }

  static std::string print(point p) { return nz(p[0]); }
  static std::string print(isometry p) { return nz(p); }
  };

/* split isometries into the poincare and rotational part */

TD struct rep_gyro {

  using data = D;
  using N = typename D::Number;

  using point = multivector<data>;
  using isometry = poincare_rotation<data>;

  static isometry cspin(int i, int j, typename data::Number alpha) { return { rep_mixed<D>::id(), rep_mixed<D>::cspin(i, j, alpha, true) }; }
  static isometry cspin90(int i, int j, typename data::Number alpha) { return { rep_mixed<D>::id(), rep_mixed<D>::cspin90(i, j, alpha, true) }; }
  static isometry lorentz(int i, int j, typename data::Number alpha) { return {rep_mixed<D>::lorentz(i, j, alpha), rep_mixed<D>::id() }; }
  static isometry id() { return { rep_mixed<D>::id(), rep_mixed<D>::id() }; }
  static constexpr point center() { return rep_mixed<D>::id(); }
  static point apply(const isometry& T, const point& x) { return despin(chkmul<poincare<D>,poincare<D>,even<D>>(T.first, chkmul<rotational<D>,poincare<D>,poincare<D>>(T.second, x))); }
  static isometry apply(const isometry& T, const isometry& U) {
    auto R1 = apply_nm<rotational<D>, poincare<D>, poincare<D>> (T.second, U.first);
    auto R2 = apply_nm<poincare<D>, poincare<D>, even<D>> (T.first, R1);
    auto R3 = despin2(R2);
    return { R3.first, apply_nm<rotational<D>, rotational<D>, rotational<D>> (R3.second, U.second) };
    }

  static isometry inverse(isometry T) { return { conjugate(T.first), conjugate(T.second) }; }
  static isometry push(const point& p) { return { p, rep_mixed<D>::id() }; }

  static typename D::Number dist0(const point& ax) { 
    return acos_auto<D>(get_normalized<poincare<D>, D, N>(ax, ax[0]))*2;
    } 
  static constexpr int mvlast = 1<<(D::Dim-1);
  static typename D::Number angle(const point& x) { 
     return atan2(x[0][2 | mvlast], x[0][1 | mvlast]); 
     } 
  static typename D::Number get_coord(const point& x, int i) {
    auto x1 = multimul(multimul(x[0], unit_vector<multivector_data<data>> (mvlast)), conjugate(x[0])); 
    auto x2 = unembed(x1);
    return get_normalized(x2, x2[i]);
    }

  static std::string print(point p) { return nz(p[0]); }
  static std::string print(isometry p) { return "["+nz(p.first)+","+nz(p.second)+"]"; }
  };

}
