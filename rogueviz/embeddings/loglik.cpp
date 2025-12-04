namespace rogueviz {

namespace embeddings {

// log likelihood utilities
//--------------------------

// MLE of the binomial distribution (a successes, b failures)

ld bestll(ld a, ld b) {
  if(a == 0 || b == 0) return 0;
  return a * log(a/(a+b)) + b * log(b/(a+b));
  }

// a successes, ab total

ld bestll2(ld a, ld ab) { return bestll(a, ab-a); }

// --- continuous logistic loglikelihood
// --------------------------------------

vector<ld> disttable0, disttable1;

using logisticfun = std::function<ld(logistic&)>;

// needs cellcoords/rvcoords/origcoords
void build_disttable() {
  indenter_finish im("build_disttable");
  int N = isize(rogueviz::vdata);
  vector<int> tab(N, N);
  disttable0.clear();
  disttable1.clear();

  using namespace rogueviz;
  
  for(int i=0; i<N; i++) {
    for(auto p: vdata[i].edges) {
      int j = p.second->i ^ p.second->j ^ i;
      if(j<i) tab[j] = i;
      }
    for(int j=0; j<i; j++) {
      ld dist = current->distance(i, j);
      if(dist < 0) continue;
      (tab[j] == i ? disttable1:disttable0).push_back(dist);
      }
    }
  
  sort(disttable0.begin(), disttable0.end());
  sort(disttable1.begin(), disttable1.end());
  }

// needs build_disttable
ld loglik_cont(logistic& l = current_logistic) {
    
  ld llh = 1;
  for(auto p: disttable1) llh += l.lyes(p);
  for(auto p: disttable0) {
    ld lp = l.lno(p);
    llh += lp;
    if(lp > -1e-10) break;
    }
  
  return llh;
  }

using ll = long long;
vector<array<ll, 2>> disttable_approx;

ld llcont_approx_prec = 10000;

int threads = 32;

void build_disttable_approx() {
  indenter_finish im("build_disttable_approx");

  array<ll, 2> zero = {0, 0};

  using namespace rogueviz;

  std::vector<vector<array<ll, 2>>> results(threads);
  std::vector<std::thread> v;
  int N = isize(rogueviz::vdata);
  for(int k=0; k<threads; k++)
    v.emplace_back([&,k] () {
      auto& dt = results[k];
      vector<int> tab(N, N);
      auto p = k ? nullptr : new progressbar(N/threads, "build_disttable_approx");
      for(int i=k; i<N; i+=threads) {
        if(p) (*p)++;
        for(auto p: vdata[i].edges) {
          int j = p.second->i ^ p.second->j ^ i;
          if(j<i) tab[j] = i;
          }
        for(int j=0; j<i; j++) {
          ld dist = current->distance(i, j);
          if(dist < 0) continue;
          int dista = dist * llcont_approx_prec;
          if(isize(dt) < dista+1)
            dt.resize(dista+1, zero);
          dt[dista][(tab[j] == i) ? 1 : 0]++;
          }
        }
      if(p) delete p;
      });
  for(std::thread& t:v) t.join();
  
  int mx = 0;
  for(auto& r: results) mx = max(mx, isize(r));
  disttable_approx.clear();
  disttable_approx.resize(mx, zero);
  
  for(auto& r: results)
    for(int i=0; i<isize(r); i++) 
      for(int j=0; j<2; j++)
        disttable_approx[i][j] += r[i][j];
  }

ld loglik_cont_approx(logistic& l) {
    
  ld llh = 0;
  int N = isize(disttable_approx);
  for(int i=0; i<N; i++) {
    if(disttable_approx[i][0])
      llh += l.lno((i+.5)/llcont_approx_prec) * disttable_approx[i][0];
    if(disttable_approx[i][1])
      llh += l.lyes((i+.5)/llcont_approx_prec) * disttable_approx[i][1];
    }
  return llh;
  }

int max_steps = 100000;

void fix_logistic_parameters(logistic& l, const logisticfun& f, const char *name, ld eps) {
  indenter_finish im("fix_logistic_parameters");
  ld cur = f(l);
  println(hlog, hr::format("%s = %20.10" PLDF " (R=%10.5" PLDF " T=%" PLDF ")", name, cur, l.R, l.T));

  for(ld step=1; step>eps; step /= 2) {
    
    while(true) { l.R += step; ld t = f(l); if(t <= cur) break; cur = t; }
    l.R -= step;
    
    while(true) { l.R -= step; ld t = f(l); if(t <= cur) break; cur = t; }
    l.R += step;

    while(true) { l.T += step; ld t = f(l); if(t <= cur) break; cur = t; }
    l.T -= step;
    
    while(true) { l.T -= step; ld t = f(l); if(t <= cur) break; cur = t; }
    l.T += step;

    println(hlog, hr::format("%s = %20.10" PLDF " (R=%10.5" PLDF " T=%10.5" PLDF ")", name, cur, l.R, l.T));
    fflush(stdout);
    }
  }

void fast_loglik_cont(logistic& l, const logisticfun& f, const char *name, ld start, ld eps) {

  if(name) println(hlog, "fix_logistic_parameters");
  indenter_finish im(name);
  ld cur = f(l);
  if(name) println(hlog, hr::format("%s = %20.10" PLDF " (R=%10.5" PLDF " T=%" PLDF ")", name, cur, l.R, l.T));

  map<pair<double, double>, double> memo;
  auto ff = [&] () {
    if(l.T < -5) exit(1);
    if(memo.count(make_pair(l.R, l.T)))
      return memo[make_pair(l.R, l.T)];
    return memo[make_pair(l.R, l.T)] = f(l);
    };
  
  int steps = 0;

  for(ld step=start; step>eps; step /= 2) {
  
    loop:
    bool changed = false;        
    
    while(true) { steps++; l.R += step; ld t = ff(); if(t <= cur || steps > max_steps) break; cur = t; changed = true; }
    l.R -= step;
    
    while(true) { steps++; l.R -= step; ld t = ff(); if(t <= cur || steps > max_steps) break; cur = t; changed = true; }
    l.R += step;

    while(true) { steps++; l.T += step; ld t = ff(); if(t <= cur || steps > max_steps) break; cur = t; changed = true; }
    l.T -= step;
    
    while(true) { steps++; l.T -= step; ld t = ff(); if(t <= cur || l.T < 1e-3 || steps > max_steps) break; cur = t; changed = true; }
    l.T += step;
    
    if(changed) goto loop;

    if(name) println(hlog, hr::format("%s = %20.10" PLDF " (R=%10.5" PLDF " T=%10.5" PLDF ")", name, cur, l.R, l.T));
    fflush(stdout);
    }
  }

logistic cont_logistic;

int loglik_args() {
  using namespace arg;

  if(argis("-loglik-approx")) {
    build_disttable_approx();
    fast_loglik_cont(cont_logistic, loglik_cont_approx, "lcont", 1, 1e-6);
    // return loglik_cont();
    }
  else if(argis("-loglik-precise")) {
    build_disttable();
    fast_loglik_cont(cont_logistic, loglik_cont, "lcont", 1, 1e-6);
    // return loglik_cont();
    }
  else return 1;

  return 0;
  }

auto alol = addHook(hooks_args, 50, loglik_args);

}
}

