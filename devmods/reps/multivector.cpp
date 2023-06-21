namespace reps {

TD struct mvector {
  array<typename D::Number, D::Dim> values;
  typename D::Number& operator [] (int i) { return values[i]; }
  const typename D::Number& operator [] (int i) const { return values[i]; }
  mvector operator + (const mvector& M) const {
    mvector result;
    for(int i=0; i<D::Dim; i++) result[i] = self[i] + M[i];
    return result;
    }
  mvector operator - (const mvector& M) const {
    mvector result;
    for(int i=0; i<D::Dim; i++) result[i] = self[i] - M[i];
    return result;
    }
  mvector operator * (const typename D::Number& x) const {
    mvector result;
    for(int i=0; i<D::Dim; i++) result[i] = self[i] * x;
    return result;
    }
  mvector operator / (const typename D::Number& x) const {
    mvector result;
    for(int i=0; i<D::Dim; i++) result[i] = self[i] / x;
    return result;
    }
  mvector operator * (int x) const {
    mvector result;
    for(int i=0; i<D::Dim; i++) result[i] = self[i] * x;
    return result;
    }
  mvector operator / (int x) const {
    mvector result;
    for(int i=0; i<D::Dim; i++) result[i] = self[i] / x;
    return result;
    }
  };

TD struct matrix {
  array<array<typename D::Number, D::Dim>, D::Dim> values;
  array<typename D::Number, D::Dim>& operator [] (int i) { return values[i]; }
  const array<typename D::Number, D::Dim>& operator [] (int i) const { return values[i]; }
  matrix operator * (const matrix& M) const {
    matrix result;
    for(int i=0; i<D::Dim; i++)
    for(int k=0; k<D::Dim; k++) {
      result[i][k] = typename D::Number(0);
      for(int j=0; j<D::Dim; j++) result[i][k] += self[i][j] * M[j][k];
      }
    return result;
    }
  mvector<D> operator * (const mvector<D>& V) const {
    mvector<D> result;
    for(int i=0; i<D::Dim; i++) {
      result[i] = typename D::Number(0);
      for(int j=0; j<D::Dim; j++) result[i] += self[i][j] * V[j];
      }
    return result;
    }
  matrix operator * (const typename D::Number& x) const {
    matrix result;
    for(int i=0; i<D::Dim; i++) for(int j=0; j<D::Dim; j++) result[i][j] = self[i][j] * x;
    return result;
    }
  matrix operator / (const typename D::Number& x) const {
    matrix result;
    for(int i=0; i<D::Dim; i++) for(int j=0; j<D::Dim; j++) result[i][j] = self[i][j] / x;
    return result;
    }
  matrix operator * (int x) const {
    matrix result;
    for(int i=0; i<D::Dim; i++) for(int j=0; j<D::Dim; j++) result[i][j] = self[i][j] * x;
    return result;
    }
  matrix operator / (int x) const {
    matrix result;
    for(int i=0; i<D::Dim; i++) for(int j=0; j<D::Dim; j++) result[i][j] = self[i][j] / x;
    return result;
    }
  };

TD constexpr mvector<D> zero_vector() {
  mvector<D> result;
  for(int i=0; i<D::Dim; i++) result[i] = typename D::Number(0);
  return result;
  }

TD constexpr mvector<D> unit_vector(int id) {
  mvector<D> result;
  for(int i=0; i<D::Dim; i++) result[i] = typename D::Number(0);
  result[id] = typename D::Number(1);
  return result;
  }

TD struct multivector_data {
  using Number = typename D::Number;
  static constexpr int Dim = 1<<D::Dim;
  static constexpr int Flipped = -1;
  };

TD using multivector = mvector<multivector_data<D>>;

TD std::string nz(const multivector<D>& a) {
  constexpr int mdim = 1<<D::Dim;
  using Number = typename D::Number;  
  hr::shstream str;
  for(int i=0; i<mdim; i++) if(abs(a[i]) > Number(1e-10)) {
    if(str.s != "") print(str, " ");
    if(a[i] > Number(0)) print(str, "+");
    print(str, a[i]);
    for(int u=0; u<D::Dim; u++) if(i & (1<<u)) print(str, hr::s0 + char('A'+u));
    }
  if(str.s == "") return "0";
  return str.s;
  }

TD constexpr multivector<D> unit(const typename D::Number& a) {
  auto res = zero_vector<multivector_data<D>>();
  res[0] = a;
  return res;
  }

TD constexpr multivector<D> embed(const mvector<D>& a) {
  auto res = zero_vector<multivector_data<D>>();
  for(int i=0; i<D::Dim; i++) res[1<<i] = a[i];
  return res;
  }

TD constexpr mvector<D> unembed(const multivector<D>& a) {
  mvector<D> res;
  for(int i=0; i<D::Dim; i++) res[i] = a[1<<i];
  return res;
  }

/* for clarity */
using mvindex = int;
using signtype = int;
/* mvindex decimal 10 (binary 1010) corresponds to unit_vector(1) * unit_vector(3) */

TD constexpr signtype conj_sign(mvindex mvid) {
  int b = __builtin_popcount(mvid);
  b = b * (b+1) / 2;
  return (b&1) ? -1 : 1;
  }

TD constexpr signtype tra_sign(mvindex mvid) {
  int b = __builtin_popcount(mvid);
  b = b * (b-1) / 2;
  return (b&1) ? -1 : 1;
  }

TD constexpr signtype mul_sign(mvindex a, mvindex b) {
  int flips = 0;
  for(int i=0; i<D::Dim; i++) if(b & (1<<i)) {
    // we will need to swap it with that many 1-bits of a
    flips += __builtin_popcount(a & ((1<<i)-1));
    if((i == D::Flipped) && (a & (1<<i))) flips++;
    }
  return (flips&1) ? -1 : 1;
  }

TD struct all {
  static constexpr bool check(mvindex a) { return true; }
  static constexpr bool isflat(mvindex a) { return false; }
  };

TD struct even {
  static constexpr bool check(mvindex a) { return __builtin_popcount(a) % 2 == 0; }
  static constexpr bool isflat(mvindex a) { return false; }
  };

TD struct flat_even {
  static constexpr bool check(mvindex a) { return __builtin_popcount(a) % 2 == 0; }
  static constexpr bool isflat(mvindex a) { return nm == nmFlatten && a == 0; }
  };

TD struct odd {
  static constexpr bool check(mvindex a) { return __builtin_popcount(a) % 2 == 1; }
  static constexpr bool isflat(mvindex a) { return false; }
  };

TD struct units {
  static constexpr bool check(mvindex a) { return a == 0; }
  static constexpr bool isflat(mvindex a) { return false; }
  };

TD struct rotational {
  static constexpr bool check(mvindex a) { return __builtin_popcount(a) % 2 == 0 && a < (1<<(D::Dim-1)); }
  static constexpr bool isflat(mvindex a) { return false; }
  };

TD struct underling {
  static constexpr bool check(mvindex a) { return __builtin_popcount(a) == 1; }
  static constexpr bool isflat(mvindex a) { return false; }
  };

TD struct flat_underling {
  static constexpr bool check(mvindex a) { return __builtin_popcount(a) == 1; }
  static constexpr bool isflat(mvindex a) { return nm == nmFlatten && a == 1<<(D::Dim-1); }
  };

TD struct poincare {
  static constexpr bool check(mvindex a) { return __builtin_popcount(a ^ (1<<(D::Dim-1))) == 1; }
  static constexpr bool isflat(mvindex a) { return false; }
  };

TD multivector<D> multimul(const multivector<D>& a, const multivector<D>& b) {
  constexpr int mdim = 1<<D::Dim;
  auto res = zero_vector<multivector_data<D>>();
  for(mvindex i=0; i<mdim; i++)
  for(mvindex j=0; j<mdim; j++) {
    res[i^j] += a[i] * b[j] * mul_sign<D>(i, j);
    }
  return res;
  }

template<class A, class B, class C, class D>
multivector<D> chkmul(const multivector<D>& a, const multivector<D>& b) {
  constexpr int mdim = 1<<D::Dim;
  auto res = zero_vector<multivector_data<D>>();

  /* we initialize with 0s and then add stuff, so one add per component is not necessary */
  for(mvindex i=0; i<mdim; i++) if(C::check(i)) cbc[cbcAdd]--;

  for(mvindex i=0; i<mdim; i++) if(A::check(i))
  for(mvindex j=0; j<mdim; j++) if(B::check(j) && C::check(i^j)) {
    if(A::isflat(i))
      res[i^j] += b[j] * mul_sign<D>(i, j);
    else if(B::isflat(j))
      res[i^j] += a[i] * mul_sign<D>(i, j);
    else
      res[i^j] += a[i] * b[j] * mul_sign<D>(i, j);
    }
  return res;
  }

TD multivector<D> conjugate(const multivector<D>& a) {
  constexpr int mdim = 1<<D::Dim;
  auto res = a;
  for(int i=0; i<mdim; i++) res[i] *= conj_sign<D>(i);
  return res;
  }

TD multivector<D> transpose(const multivector<D>& a) {
  constexpr int mdim = 1<<D::Dim;
  auto res = a;
  for(int i=0; i<mdim; i++) res[i] *= tra_sign<D>(i);
  return res;
  }

template<class C, class D> multivector<D> apply_nm(multivector<D> a);

TD using poincare_rotation = std::pair<multivector<D>, multivector<D>>;

/** decompose o into the poincare part and the rotational component */
TD poincare_rotation<D> despin2(const multivector<D>& a) {
  auto p = a;
  for(int i=(1<<(D::Dim-1)); i<(1<<(D::Dim)); i++) p[i] = typename D::Number(0);
  p = p * pow(chkmul<rotational<D>,rotational<D>,units<D>>(p, conjugate(p))[0], -0.5);
  auto p1 = chkmul<even<D>,rotational<D>,poincare<D>>(a, conjugate(p));
  return {apply_nm<poincare<D>, D>(p1), p};
  }

/** remove the rotational component of a, leaving only the poincare part */
TD multivector<D> despin(const multivector<D>& a) {
  auto p = a;
  for(int i=(1<<(D::Dim-1)); i<(1<<(D::Dim)); i++) p[i] = typename D::Number(0);
  auto p1 = chkmul<even<D>,rotational<D>,poincare<D>>(a, conjugate(p));
  if(nm == nmInvariant) return p1 * pow(chkmul<rotational<D>,rotational<D>,units<D>>(p, conjugate(p))[0], -0.5);
  return apply_nm<poincare<D>, D>(p1);
  }

TD std::string nzv(const mvector<D>& a) { return "vector(" + nz(embed(a)) + ")"; }
TD std::string nzv(const matrix<D>& a) { return "<matrix>"; }

template<class C, class D>
typename D::Number sqnorm(multivector<D> a) {
  using N = typename D::Number;
  auto res = chkmul<C, C, units<D>>(a, conjugate(a))[0];
  if(res <= N(0) || isinf(res) || isnan(res)) res = N(1);
  return res;
  }

TD typename D::Number sqnorm(mvector<D> a) { 
  using N = typename D::Number;
  N res(0);
  for(int i=0; i<D::Dim; i++) res += a[i] * a[i] * (i == D::Flipped ? -1:1);
  if(D::Flipped != -1) res = -res;
  if(nm ==nmWeak && (res <= N(0) || isinf(res) || isnan(res))) res = N(1);
  return res; 
  }

/** if nm is set to nmFlatten or nmForced or nmBinary, apply the requested operation */

template<class C, class D> multivector<D> flatten(multivector<D> a) {
  using N = typename D::Number;
  auto divby = a[0]; a[0] = N(1);
  for(int i=1; i<(1<<D::Dim); i++) if(C::check(i)) a[i] /= divby;
  return a;
  }

template<class C, class D>
multivector<D> apply_nm(multivector<D> a) {
  if(nm == nmFlatten) return flatten<C>(a);
  if(nm == nmForced || nm == nmWeak) return a * pow(sqnorm<C,D>(a), -0.5);
  if(nm == nmBinary) { while(a[0] >= 2) { a = a / 2; } while(a[0] > 0 && a[0] < 0.5) { a = a * 2; } }
  return a;
  }

TD mvector<D> apply_nm(mvector<D> a) {
  if(nm == nmFlatten) { cbc[cbcDiv]--; return a / a[D::Dim-1]; }
  if(nm == nmForced || nm == nmWeak) return a * pow(sqnorm<D>(a), -0.5);
  if(nm == nmBinary) { while(a[D::Dim-1] >= 2) { a = a / 2; } while(a[D::Dim-1] > 0 && a[D::Dim-1] < 0.5) { a = a * 2; } }
  return a;
  }

/** get b which is a coordinate of a, but in normalized form. That is, if a is normalized simply return b, otherwise, multiply b appropriately */

template<class C, class D, class E> E get_normalized(multivector<D> a, E b) {
  if(nm != nmInvariant && nm != nmForced) return b * pow(sqnorm<C,D>(a), -0.5);
  return b;
  }

template<class D, class E> E get_normalized(mvector<D> a, E b) {
  if(nm != nmInvariant && nm != nmForced) return b * pow(sqnorm<D>(a), -0.5);
  return b;
  }

}

