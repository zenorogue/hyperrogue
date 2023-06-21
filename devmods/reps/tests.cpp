namespace reps {

constexpr int test_dim = 3;
constexpr bool in_hyperbolic = true;

int edges, valence;

void prepare_tests() {
  hr::start_game();
  if(MDIM != test_dim) throw hr::hr_exception("fix your dimension");
  if(!(in_hyperbolic ? hyperbolic : sphere)) throw hr::hr_exception("fix your geometry");
  if(hr::variation != hr::eVariation::pure) throw hr::hr_exception("fix your variation"); 
  if(quotient) throw hr::hr_exception("fix your quotient");
  if(test_dim == 4) {
    if(cginf.tiling_name != "{4,3,5}") throw hr::hr_exception("only {4,3,5} implemented in 3D");
    edges = 4;
    valence = 5;
    }
  else {
    edges = hr::cwt.at->type;
    bool ok;
    valence = hr::get_valence(hr::cwt.at, 1, ok);
    }
  }

struct data {
  using Number = hr::ld;
  static constexpr int Dim = test_dim;
  static constexpr int Flipped = in_hyperbolic ? test_dim-1 : -1;
  };

struct countdata {
  using Number = countfloat;
  static constexpr int Dim = data::Dim;
  static constexpr int Flipped = data::Flipped;
  };

struct bigdata {
  using Number = big;
  static constexpr int Dim = data::Dim;
  static constexpr int Flipped = data::Flipped;
  };

using good = rep_linear_nn<bigdata>;

int debug; // 0 -- never, 1 -- only errors, 2 -- always

vector<cell*> randomwalk(std::mt19937& gen, cell *from, int dist) {
  vector<cell*> res = { from };
  while(celldistance(from, res.back()) < dist) {
    int i = gen() % res.back()->type;
    res.push_back(res.back()->cmove(i));
    }
  return res;
  }

template<class N> N rand01(std::mt19937& gen) { return N(((gen() & HRANDMAX) | 1) / (HRANDMAX+1.)); }

vector<cell*> random_return(std::mt19937& gen, cell *from, cell *to, ld peq, ld pbad) {
  vector<cell*> res = { from };
  ld d = celldistance(to, from);
  while(to != res.back()) {
    int i = gen() % res.back()->type;
    cell *r1 = res.back()->cmove(i);
    ld d1 = celldistance(to, r1);
    bool ok = d1 < d ? true : d1 == d ? rand01<ld>(gen) < peq : rand01<ld>(gen) < pbad;
    if(ok) { res.push_back(r1); d = d1; }
    }
  return res;
  }

vector<cell*> vrev(vector<cell*> a) { reverse(a.begin(), a.end()); return a; }
vector<cell*> vcon(vector<cell*> a, vector<cell*> b) { for(auto bi: b) a.push_back(bi); return a; }

template<class N> N edge_of_triangle_with_angles(N alpha, N beta, N gamma) {
  N of = (cos(alpha) + cos(beta) * cos(gamma)) / (sin(beta) * sin(gamma));
  if(hyperbolic) return acosh(of);
  return acos(of);
  }

template<class N> N get_edgelen() {
  N beta = get_deg<N>(360)/valence;
  return edge_of_triangle_with_angles<N> (beta, get_deg<N>(180)/edges, get_deg<N>(180)/edges);
  }

template<class T> typename T::isometry cpush(int c, typename T::data::Number distance) {
  return T::lorentz(c, T::data::Dim-1, distance);
  }

template<class T> struct cube_rotation_data_t {
  std::vector<std::pair<hr::transmatrix, typename T::isometry>> mapping;
  };

template<class T> cube_rotation_data_t<T> cube_rotation_data;

template<class T> cube_rotation_data_t<T>& build_cube_rotation() {
  auto& crd = cube_rotation_data<T>;
  auto& crdm = crd.mapping;
  // using N = typename T::data::Number;
  if(crdm.empty()) {
    crdm.emplace_back(hr::Id, T::id());
    for(int i=0; i<isize(crdm); i++) 
    for(int j=0; j<3; j++) for(int k=0; k<3; k++) if(j != k) {
      hr::transmatrix U = crdm[i].first * hr::cspin90(j, k);
      bool is_new = true;
      for(int i0=0; i0<isize(crdm); i0++) if(hr::eqmatrix(U, crdm[i0].first)) is_new = false;
      // if(is_new) crdm.emplace_back(U, T::apply(crdm[i].second, T::cspin(j, k, get_deg<N>(90))));
      if(is_new) crdm.emplace_back(U, T::apply(crdm[i].second, T::cspin90(j, k)));
      }
    if(isize(crdm) != 24) {
      println(hlog, "the number of rotations found: ", isize(crdm));
      throw hr::hr_exception("wrong number of rotations");
      }
    }
  return crd;
  }

template<class T, class U> U apply_move(cell *a, cell *b, U to) {
  if(a == b) return to;
  using N = typename T::data::Number;
  if constexpr(test_dim == 4) {
    auto& crdm = build_cube_rotation<T>().mapping;
    int ida = neighborId(a, b);
    auto M = hr::currentmap->adj(a, ida);
    for(int i0=0; i0<isize(crdm); i0++)
    for(int i1=0; i1<isize(crdm); i1++)
      if(hr::eqmatrix(M, crdm[i0].first * hr::xpush(1.06128) * crdm[i1].first)) {
        to = T::apply(crdm[i1].second, to);
        to = T::apply(cpush<T>(0, get_edgelen<N>()), to);
        to = T::apply(crdm[i0].second, to);
        return to;
        }
    println(hlog, "tessf = ", hr::cgip->tessf);
    println(hlog, "len = ", get_edgelen<N>());
    throw hr::hr_exception("rotation not found");
    }
  int ida = neighborId(a, b);
  int idb = neighborId(b, a);
  auto P1 = T::cspin(0, 1, idb * get_deg<N>(360) / edges);
  to = T::apply(P1, to);
  auto P2 = cpush<T>(0, get_edgelen<N>());
  to = T::apply(P2, to);
  auto P3 = T::cspin(1, 0, get_deg<N>(180) + ida * get_deg<N>(360) / edges);
  to = T::apply(P3, to);
  return to;
  }

template<class T, class U> U apply_path(vector<cell*> path, U to) {
  for(int i=hr::isize(path)-2; i>=0; i--)
    to = apply_move<T, U> (path[i], path[i+1], to);
  return to;
  }

template<class T> double test_sanity(int i) {
  hr::indenter in(2);

  ld d1 = 1.25, d2 = 1.5;

  typename good::point gp = good::center();
  gp = good::apply(cpush<good>(0, d1), gp);
  gp = good::apply(cpush<good>(1, d2), gp);
  gp = good::apply(good::cspin(0, 1, get_deg<big>(72)), gp);

  typename T::point p = T::center();
  p = T::apply(cpush<T>(0, d1), p);
  p = T::apply(cpush<T>(1, d2), p);
  p = T::apply(T::cspin(0, 1, get_deg<typename T::data::Number>(72)), p);

  double res = 0;
  #define ADD(x, y) if(debug) println(hlog, "VS ", x, ",", y); res += pow( double(x-y), 2);
  #define ADDA(x, y) if(debug) println(hlog, "VS ", x, ",", y); res += pow( cyclefix_on(double(x-y)), 2);

  if(debug) println(hlog, "p=", T::print(p));

  ADD(T::get_coord(p, 0), good::get_coord(gp, 0));
  ADD(T::get_coord(p, 1), good::get_coord(gp, 1));
  ADD(T::get_coord(p, 2), good::get_coord(gp, 2));
  ADD(T::dist0(p), good::dist0(gp));
  ADDA(T::angle(p), good::angle(gp));

  return res;
  }

template<class T> double test_consistency(int i) {
  double res = 0;
  using D = typename T::data;

  auto a = cpush<T>(0, 1);
  auto b = cpush<T>(1, 1);
  auto c = cpush<T>(0, 1);

  auto s   = T::apply(T::apply(a, b), c);
  auto sp  = T::apply(s, T::center());
  auto s1  = T::apply(a, T::apply(b, c));
  auto sp1 = T::apply(s1, T::center());
  auto sp2 = T::apply(a, T::apply(b, T::apply(c, T::center())));
  ADD(T::dist0(sp), T::dist0(sp1));
  ADD(T::dist0(sp), T::dist0(sp2));
  for(int i=0; i<D::Dim; i++) { ADD(T::get_coord(sp, i), T::get_coord(sp1, i)); }
  for(int i=0; i<D::Dim; i++) { ADD(T::get_coord(sp, i), T::get_coord(sp2, i)); }
  if(test_dim == 3) {
    ADDA(T::angle(sp), T::angle(sp1));
    ADDA(T::angle(sp), T::angle(sp2));
    }

  return res;
  }

template<class T> double test_tba(int id) {
  std::mt19937 testr; testr.seed(id);
  for(int i=0; i<hr::cwt.at->type; i++) {
    vector<cell*> p = {hr::cwt.at, hr::cwt.at->cmove(i), hr::cwt.at};
    auto h = apply_path<T>(p, T::center());
    if(debug == 2) println(hlog, "i=", hr::lalign(3, i), " h = ", T::print(h), " distance =", T::dist0(h));
    if(T::dist0(h) >= 0 && T::dist0(h) < 0.1) continue;
    exit(1);
    }
  return 999;
  }

template<class T> double test_loop_point(int id) {
  std::mt19937 testr; testr.seed(id);
  for(int i=0; i<100; i++) {
    auto p1 = randomwalk(testr, hr::cwt.at, i);
    auto p2 = random_return(testr, p1.back(), hr::cwt.at, 1/16., 1/32.);
    auto p = vcon(p1, p2);
    if(debug == 2) println(hlog, "path = ", p);
    auto h = apply_path<T>(vrev(p), T::center());
    if(debug == 2) println(hlog, "i=", hr::lalign(3, i), " h = ", T::print(h), " distance =", T::dist0(h));
    if(T::dist0(h) >= 0 && T::dist0(h) < 0.1) continue;
    return i;
    }
  return 999;
  }

template<class T> double test_loop_iso(int id) {
  std::mt19937 testr; testr.seed(id);
  for(int i=0; i<100; i++) {
    auto p1 = randomwalk(testr, hr::cwt.at, i);
    auto p2 = random_return(testr, p1.back(), hr::cwt.at, 1/16., 1/32.);
    auto p = vcon(p1, p2);
    if(debug == 2) println(hlog, "path = ", p);
    auto h = apply_path<T>(vrev(p), T::id());
    auto hr = T::apply(h, T::center());
    // println(hlog, "i=", hr::lalign(3, i), " h=", hr);
    if(debug == 2) println(hlog, "i=", hr::lalign(3, i), " hr = ", T::print(hr), " distance = ", T::dist0(hr));
    if(T::dist0(hr) >= 0 && T::dist0(hr) < 0.1) continue;
    if(debug == 1) println(hlog, "i=", hr::lalign(3, i), " hr = ", T::print(hr), " distance = ", T::dist0(hr));
    return i;
    }
  return 999;
  }

template<class T, class F> vector<T> repeat_test(const F& f, int qty) {
  vector<T> res;
  for(int i=0; i<qty; i++) res.push_back(f(i));
  return res;
  }

template<class T> typename T::isometry random_rotation(std::mt19937& testr) {
  using D = typename T::data;
  using N = typename D::Number;

  if(D::Dim == 3) {
    auto alpha = rand01<N>(testr) * get_deg<N>(360);
    return T::cspin(0, 1, alpha);
    }

  auto x = T::id();
  for(int i=0; i<100; i++) {
    int c0 = testr() % (D::Dim-1);
    int c1 = testr() % (D::Dim-1);
    if(c0 == c1) continue;
    auto len = rand01<N>(testr) * get_deg<N>(360);
    x = T::apply(T::cspin(c0, c1, len), x);
    }
  return x;
  }

template<class T> typename T::isometry random_iso(std::mt19937& testr) {
  auto x = T::id();
  using D = typename T::data;
  using N = typename D::Number;
  for(int i=0; i<100; i++) {
    int c0 = testr() % D::Dim;
    int c1 = testr() % D::Dim;
    if(c0 == c1) continue;
    if(c0 == D::Flipped) std::swap(c0, c1);
    N len = c1 < D::Flipped ? rand01<N>(testr) * get_deg<N>(360) : (rand01<N>(testr)-N(0.5)) * N(0.25);
    if(c1 == D::Flipped) x = T::apply(T::lorentz(c0, c1, len), x);
    else x = T::apply(T::cspin(c0, c1, len), x);
    }
  return x;
  }


template<class T> std::string test_count(int id) {
  std::mt19937 testr; testr.seed(id);
  hr::shstream out;
  auto A = random_iso<T>(testr);
  auto B = random_iso<T>(testr);
  auto C = random_iso<T>(testr);
  auto P = T::apply(C, T::center());
  for(int i=0; i<9; i++) {
    counts.clear();
    for(auto& i: cbc) i = 0;
    std::string s;
    switch(i) {
      case 0: s = "spin"; T::cspin(0, 1, countfloat(.5)); break;
      case 1: s = "L0"; T::lorentz(0, T::data::Dim-1, countfloat(.5)); break;
      case 2: s = "L1"; T::lorentz(1, T::data::Dim-1, countfloat(.5)); break;
      case 3: s = "ip"; T::apply(A, P); break;
      case 4: s = "ii"; T::apply(A, B); break;
      case 5: s = "d0"; T::dist0(P); break;
      case 6: s = "angle"; T::angle(P); break;
      case 7: s = "inverse"; T::inverse(A); break;
      case 8: s = "push"; T::push(P); break;
      }
    if(i) print(out, " ");
    if(1) {
      print(out, s, "(");
      bool nsp = false;
      for(int i=1; i<5; i++) if(cbc[i]) {
        if(nsp) print(out, " ");
        print(out, cbc[i], hr::s0+".AMDF"[i]);
        nsp = true;
        }
      print(out, ")");
      }
    }
  return out.s;
  }

template<class A, class B> bool closeto(A a, B b) { return abs(a-b) < 0.1; }

template<class A, class B> bool closeto_angle(A a, B b) { return abs(cyclefix_on(double(a-b))) < 0.1; }

template<class T> double test_angledist(int id) {
  std::mt19937 testr; testr.seed(id);
  for(int i=1; i<1000; i += (1 + i/5)) {

    auto p = randomwalk(testr, hr::cwt.at, i);
    auto h = apply_path<T>(vrev(p), T::center());

    auto gh = apply_path<good>(vrev(p), good::center());

    if(debug == 2) {
      println(hlog, "good: ", good::print(gh), " dist = ", good::dist0(gh), " angle = ", good::angle(gh));
      println(hlog, "test: ", T::print(h), " dist = ", T::dist0(h), " angle = ", T::angle(h), " [i=", i, "]");
      }

    if(closeto(good::dist0(gh), T::dist0(h)) && closeto_angle(good::angle(gh), T::angle(h))) continue;

    if(debug == 1) {
      println(hlog, "good: ", good::print(gh), " dist = ", good::dist0(gh), " angle = ", good::angle(gh));
      println(hlog, "test: ", T::print(h), " dist = ", T::dist0(h), " angle = ", T::angle(h), " [i=", i, "]");
      }

    return i;
    }
  return 999;
  }

#define TEST_VARIANTS(x,D,q,t,rn, r) \
  nm = nmInvariant; println(hlog, rn, "invariant: ", repeat_test<t>(x<r<D>>, q));  \
  nm = nmForced;    println(hlog, rn, "forced   : ", repeat_test<t>(x<r<D>>, q));   \
  nm = nmWeak;      println(hlog, rn, "weak     : ", repeat_test<t>(x<r<D>>, q));    \
  nm = nmFlatten;   println(hlog, rn, "flatten  : ", repeat_test<t>(x<r<D>>, q));     \
  nm = nmCareless;  println(hlog, rn, "careless : ", repeat_test<t>(x<r<D>>, q));      \
  nm = nmBinary;    println(hlog, rn, "binary   : ", repeat_test<t>(x<r<D>>, q));

/*
#define TEST_ALL(x,D,q,t) \
  println(hlog, "HyperRogue: ", repeat_test<t>(x<rep_hr<D>>, q));                                             \
  polar_mod = polar_choose = false;  println(hlog, "high polar: ", repeat_test<t>(x<rep_high_polar<D>>, q));   \
  if(test_dim == 3) { polar_mod = polar_choose = false;  println(hlog, "low polar : ", repeat_test<t>(x<rep_polar2<D>>, q)); }
*/

//                    println(hlog, "HyperRogue         : ", repeat_test<t>(x<rep_hr<D>>, q));                     

#define TEST_ALL(x,D,q,t) \
  fix_matrices = true;  TEST_VARIANTS(x,D,q,t,"linear+F  ", rep_linear)                                           \
  fix_matrices = false; TEST_VARIANTS(x,D,q,t,"linear-F  ", rep_linear)                                            \
  TEST_VARIANTS(x,D,q,t,"mixed     ", rep_mixed)                                                                    \
  TEST_VARIANTS(x,D,q,t,"Clifford  ", rep_clifford)                                                                  \
  nm = nmFlatten; println(hlog, "Clifford  gyro     : ", repeat_test<t>(x<rep_half<D>>, q));                          \
  nm = nmInvariant; println(hlog, "halfplane invariant: ", repeat_test<t>(x<rep_half<D>>, q));                         \
  polar_choose = false;  println(hlog, "polar     basic    : ", repeat_test<t>(x<rep_high_polar<D>>, q));               \
  polar_choose = true;   println(hlog, "polar     improved : ", repeat_test<t>(x<rep_high_polar<D>>, q));                \
  if(test_dim == 3) {                                                                                                     \
    polar_mod = false; polar_choose = false;  println(hlog, "polar     F/F      : ", repeat_test<t>(x<rep_polar2<D>>, q)); \
    polar_mod = false; polar_choose = true;   println(hlog, "polar     F/T      : ", repeat_test<t>(x<rep_polar2<D>>, q));  \
    polar_mod = true;  polar_choose = false;  println(hlog, "polar     T/F      : ", repeat_test<t>(x<rep_polar2<D>>, q));   \
    polar_mod = true;  polar_choose = true;   println(hlog, "polar     T/T      : ", repeat_test<t>(x<rep_polar2<D>>, q));    \
    }

template<class T> double test_distances(int id, int a) {
  std::mt19937 testr; testr.seed(id);
  using N = typename T::data::Number;

  for(int i=1; i<1000; i ++) {

    auto R = random_rotation<T>(testr);
    auto dif = exp(N(-1) * i) + get_deg<N>(a);

    auto p1 = T::apply(T::apply(R, cpush<T>(0, N(i))), T::center());
    auto p2 = T::apply(T::apply(R, T::apply(T::cspin(0, 1, dif), cpush<T>(0, N(i)))), T::center());
    auto pd = T::apply(T::inverse(T::push(p1)), p2);
    auto d = T::dist0(pd);

    // for good we do not need R actually
    auto gp1 = good::apply(cpush<good>(0, N(i)), good::center());
    auto gp2 = good::apply(good::apply(good::cspin(0, 1, dif), cpush<good>(0, N(i))), good::center());
    auto gd = good::dist0(good::apply(good::inverse(good::push(gp1)), gp2));

    if(debug == 2) println(hlog, T::print(p1), " ... ", T::print(p2), " = ", T::print(pd), " d=", d, " [i=", i, " dif=", dif, "]");

    if(closeto(d, gd)) continue;

    return i;
    }
  return 999;
  }

template<class T> double test_similarity(int id) { return test_distances<T>(id, 0); }
template<class T> double test_dissimilarity(int id) { return test_distances<T>(id, 180); }
template<class T> double test_other(int id) { return test_distances<T>(id, 1); }

template<class T> double test_walk(int id) {
  std::mt19937 testr; testr.seed(id);

  ld step = 1/16.;
  // mover-relative to cell-relative
  auto R0 = random_rotation<T>(testr);
  cell *c0 = hr::cwt.at;
  auto R1 = T::apply(R0, cpush<T>(0, step/2));
  cell *c1 = hr::cwt.at;

  int i = 0;
  int lastchange = 0;
  while(i< lastchange + 1000 && i < 10000 && celldistance(c0, c1) < 3) {
    // println(hlog, "iteration ", i, " in ", c0, " vs ", c1);
    auto rebase = [&] (typename T::isometry& R, cell*& c, int id) {
      ld d = T::dist0(T::apply(R, T::center()));
      for(int dir=0; dir<c->type; dir++) {
        cell *altc = c->cmove(dir);
        auto altR = apply_move<T>(altc, c, R);
        ld altd = T::dist0(T::apply(altR, T::center()));
        if(altd < d + 1/256.) {
          R = altR; c = altc; lastchange = i; return;
          }
        }
      };
    R0 = T::apply(R0, cpush<T>(0, step)); rebase(R0, c0, 0);
    R1 = T::apply(R1, cpush<T>(0, step)); rebase(R1, c1, 1);
    i++;
    }

  return i;
  }

template<class T> double test_close(int id) {
  std::mt19937 testr; testr.seed(id);

  cell *c = hr::cwt.at;
  int phase = 0;
  auto p0 = T::apply(cpush<T>(0, 1/8.), T::center());
  auto p = p0;

  int steps = 0;
  const int maxdist = id + 1;
  int errors = 0; 

  while(steps < 10000) {
    int d = testr() % c->type;
    cell *c1 = c->cmove(d);

    bool do_move = false;

    switch(phase) {
      case 0:
        /* always move */
        do_move = true;
        if(celldistance(c1, hr::cwt.at) == maxdist) phase = 1;
        break;

      case 1:
        /* move only towards the center */
        int d0 = celldistance(c, hr::cwt.at);
        int d1 = celldistance(c1, hr::cwt.at);
        do_move = d1 < d0;
        if(d1 == 0) phase = 0;
        break;
      }

    if(do_move) {
      p = apply_move<T>(c1, c, p); c = c1; steps++;
      if(debug == 2) println(hlog, "dist = ", celldistance(c, hr::cwt.at), " dist = ", T::dist0(p));
      if(c == hr::cwt.at) {
        auto d = T::dist0(p);
        auto a = T::angle(p);
        if(!closeto(d, 1/8.) || !closeto_angle(a, 0)) {
          errors++; phase = 0; c = hr::cwt.at; p = p0;
          }
        }
      }
    }

  return errors;
  }

void run_all_tests() {
  prepare_tests();

  // println(hlog, "test_sanity"); TEST_ALL(test_sanity, data, 1, ld);

  // println(hlog, "test_consistency"); TEST_ALL(test_consistency, data, 1, ld);

  println(hlog, "test_loop_iso"); TEST_ALL(test_loop_iso, data, 20, int);

  println(hlog, "test_loop_point"); TEST_ALL(test_loop_point, data, 20, int);

  println(hlog, "test_angledist"); TEST_ALL(test_angledist, data, 3, int);

  println(hlog, "test_similarity"); TEST_ALL(test_similarity, data, 20, int);

  println(hlog, "test_dissimilarity"); TEST_ALL(test_dissimilarity, data, 20, int);

  println(hlog, "test_other"); TEST_ALL(test_other, data, 20, int);

  println(hlog, "test_walk"); TEST_ALL(test_walk, data, 20, int);

  println(hlog, "test_close"); TEST_ALL(test_close, data, 20, int);

  println(hlog, "test_count"); TEST_ALL(test_count, countdata, 1, std::string);
  }

}
