namespace rogueviz {

namespace embeddings {

int tal_limit = 0;

void analyze_mdl_symmetric(bool symmetric) {
  int dim = current->get_dimension();
  int N = isize(rogueviz::vdata);

  if(dim == 1) return;

  vector<ld> center_distances(N);
  for(int id=0; id<N; id++) center_distances[id] = current->zero_distance(id);

  build_disttable_approx();
  logistic cont;
  cont.setRT(0, 1);
  if(symmetric) fast_loglik_cont(cont, loglik_cont_approx, "lcont", 1, 1e-6);
  if(symmetric) println(hlog, "loglikelihood = ", format("%.1f", -loglik_cont_approx(cont)));
  ld N1 = N * (N-1.);
  ld M = count_directed_edges();
  if(symmetric) { N1 /= 2; M /= 2; }
  ld max_radius = 0;
  for(auto& d: center_distances) max_radius = max(max_radius, d);
  ld M1 = N1 - M;
  ld entropy = M * log(M/N1) + M1 * log(M1/N1);
  println(hlog, "N = ", N, " M = ", M, " radius = ", max_radius, " entropy = ", format("%.1f", -entropy));
  
  auto entropy_for_exponent = [&] (ld alpha) {
    // integral of exp(alpha*t) from 0 to max_radius
    ld integral = alpha ? (exp(alpha*max_radius)-1) / alpha : max_radius;
    ld entropy = 0;
    for(auto& d: center_distances) entropy += -log(exp(alpha*d) / integral);
    return entropy;
    };

  // we minimize entropy
  ld alpha_min = -3, alpha_max = +3;
  for(int it=0; it<100; it++) {
    ld a = (alpha_min * 2 + alpha_max) / 3;
    ld b = (alpha_min + 2 * alpha_max) / 3;
    if(entropy_for_exponent(a) < entropy_for_exponent(b)) alpha_max = b; else alpha_min = a;
    }

  ld alpha = alpha_min;
  println(hlog, "distance entropy for zero alpha = ", entropy_for_exponent(0));
  println(hlog, "obtained alpha = ", alpha);
  println(hlog, "distance entropy = ", entropy_for_exponent(alpha));

  vector<vector<ld>> bad;
  vector<vector<ld>> good;
  vector<logistic> logistics(N, cont);

  if(!symmetric) {
    if(tal_limit == 0) { println(hlog, "not computing TAL -- the graph is asymmetric"); return; }
    if(N >= tal_limit) { println(hlog, "too large graph to compute TAL"); return; }

    println(hlog, "computing TAL");
    bad.resize(N);
    good.resize(N);
    vector<int> last_id(N, -1);
    ld total_loglik = 0;
    for(int i=0; i<N; i++) {
      for(int j: directed_edges[i]) last_id[j] = i;
      for(int j=0; j<N; j++) {
        if(last_id[j] == i)
          good[i].push_back(current->distance(i, j));
        else
          bad[i].push_back(current->distance(i, j));
        }
      sort(good[i].begin(), good[i].end());
      sort(bad[i].begin(), bad[i].end());

      auto fun = [&] (logistic& l) {
        ld res = 0;
        for(auto& g: good[i]) res += l.lyes(g);
        for(auto& b: bad[i]) res += l.lno(b);
        return res;
        };

      if(good[i].size() && bad[i].size() && good[i].back() >= bad[i][0]) {
        fast_loglik_cont(logistics[i], fun, nullptr, 1, 1e-6);
        total_loglik += -fun(logistics[i]);
        // println(hlog, i, ": R=", logistics[i].R, " T=", logistics[i].T, " f = ", -fun(logistics[i]));
        }
      }
    println(hlog, "total asymmetric loglikelihood = ", total_loglik);
    }

  auto llcont_eps = [] (ld eps) {
    return [eps] (logistic& cont) {
      ld res = 0;
      int DS = isize(disttable_approx);
      for(int i=0; i<DS; i++) {
        ld d = (i + .5) / llcont_approx_prec;
        if(disttable_approx[i][0])
          res += (cont.lno(d - eps) + cont.lno(d + eps)) / 2 * disttable_approx[i][0];
        if(disttable_approx[i][1])
          res += (cont.lyes(d - eps) + cont.lyes(d + eps)) / 2 * disttable_approx[i][1];
        }
      return res;
      };
    };
  auto compute_for_epsilon = [&] (ld eps, bool report) {
    ld dist_entropy = 0;
    ld sphere_entropy = 0;
    ld integral = alpha ? (exp(alpha*max_radius)-1) / alpha : max_radius;
    for(auto& d: center_distances) {
      dist_entropy += alpha ? -log( (exp(alpha*(d+eps))-exp(alpha*(d-eps))) / alpha / integral) : - log((2*eps)/integral);
      if(d > eps) sphere_entropy += log(sinh(d) / eps) * (dim-1); // 4pi is O(1)
      }
    ld extra_info = 0;
    if(symmetric) extra_info = -llcont_eps(eps)(cont);
    if(!symmetric) {
      for(int i=0; i<N; i++) {
        auto last = logistics[i];
        auto fun = [&] (logistic& l) {
          ld res = 0;
          for(auto& g: good[i]) res += l.lyes(g+eps) + l.lyes(g-eps);
          for(auto& b: bad[i]) res += l.lno(b+eps) + l.lno(b-eps);
          return res/2;
          };
        if(good[i].size() && bad[i].size() && good[i].back() >= bad[i][0] - 2 * eps) {
          fast_loglik_cont(last, fun, nullptr, 0.1, 1e-4);
          extra_info -= fun(last);
          }
        }
      }
    if(report) {
      println(hlog, "FOR epsilon = ", eps);
      println(hlog, "  dist_entropy = ", dist_entropy);
      println(hlog, "  sphere_entropy = ", sphere_entropy);
      println(hlog, "  negloglik = ", extra_info);
      println(hlog, "  control = ", format("%.1f", dist_entropy + sphere_entropy + extra_info));
      }
    return dist_entropy + sphere_entropy + extra_info;
    };

  ld last_rep = HUGE_VAL;
  bool rep = !symmetric;
  ld step = symmetric ? 0.1 : 1;
  for(double eps=step; eps<=10; eps+=(symmetric ? 0.1 : 1)) {
    ld cur_rep = compute_for_epsilon(eps, rep);
    if(eps >= 9) { if(!rep) compute_for_epsilon(eps, true); break; }
    if(cur_rep > last_rep) {
      ld eps_min = eps-2*step, eps_max = eps;
      int iterations = 1;
      ld last_control = 0;
      while(iterations--) {
        for(int it=0; it< (symmetric ? 100 : 10); it++) {
          ld a = (eps_min * 2 + eps_max) / 3;
          ld b = (eps_min + 2 * eps_max) / 3;
          if(compute_for_epsilon(a, rep) < compute_for_epsilon(b, false)) eps_max = b; else eps_min = a;
          }
        ld control = compute_for_epsilon(eps_min, true);
        if(abs(control - last_control) < .1) break;
        last_control = control;
        if(iterations) fast_loglik_cont(cont, llcont_eps(eps_min), "lcont_eps", 1, 1e-6);  
        eps_min = eps_min - 1; eps_max = eps_max + 1;
        }
      break;
      }
    last_rep = cur_rep;
    }
  }

}
}
