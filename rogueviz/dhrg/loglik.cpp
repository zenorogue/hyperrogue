// log-likelihood computation

#include <thread>
#define USE_THREADS

namespace dhrg {

int threads = 32;

ld llcont_approx_prec = 10000;

// tally edges of the given vertex at the given index

int edgetally[MAXDIST];

void tallyedgesof(int i, int delta, mycell *mc) {
  using namespace rogueviz;
  for(auto p: vdata[i].edges) {
    int j = p.second->i ^ p.second->j ^ i;
    if(j==i) printf("LOOP!\n");
    edgetally[quickdist(mc, vertices[j], 0)] += delta;
    }
  }

// --- count all edge tallies

void counttallies() {
  using namespace rogueviz;
  
  {
  progressbar pb(N, "Tallying pairs");
  for(int i=0; i<N; i++) {
    mycell* mc = vertices[i];
    add_to_tally(mc, 1, 0);
    add_to_set(mc, 1, 0); pb++;
    if(i % ((N/10)+1) == 0) {
      memoryInfo();
      }
    }
  }

  {
  progressbar pb(M, "Tallying edges");
  
  for(int u=0; u<MAXDIST; u++) edgetally[u] = 0;
  
  for(int i=0; i<N; i++) for(auto p: vdata[i].edges) {
    int j = p.second->i ^ p.second->j ^ i;
    if(j < i) { edgetally[quickdist(vertices[i], vertices[j], 0)]++; pb++; }
    }
  }

  }

void destroytallies() {
  progressbar pb(N, "Destroying tallies");
  for(int i=0; i<N; i++) add_to_set(vertices[i], -1, 0), pb++;
  for(int i=0; i<MAXDIST; i++)
    tally[i] = edgetally[i] = 0;
  }

// log likelihood utilities
//--------------------------

// MLE of the binomial distribution (a successes, b failures)

ld bestll(ld a, ld b) {
  if(a == 0 || b == 0) return 0;
  return a * log(a/(a+b)) + b * log(b/(a+b));
  }

// a successes, ab total

ld bestll2(ld a, ld ab) { return bestll(a, ab-a); }

// various methods of loglikelihood computation

template<class T> void fix_logistic_parameters(logistic& l, const T& f, const char *name, ld eps) {
  indenter_finish im("fix_logistic_parameters");
  ld cur = f(l);
  println(hlog, format("%s = %20.10" PLDF " (R=%10.5" PLDF " T=%" PLDF ")", name, cur, l.R, l.T));

  for(ld step=1; step>eps; step /= 2) {
    
    while(true) { l.R += step; ld t = f(l); if(t <= cur) break; cur = t; }
    l.R -= step;
    
    while(true) { l.R -= step; ld t = f(l); if(t <= cur) break; cur = t; }
    l.R += step;

    while(true) { l.T += step; ld t = f(l); if(t <= cur) break; cur = t; }
    l.T -= step;
    
    while(true) { l.T -= step; ld t = f(l); if(t <= cur) break; cur = t; }
    l.T += step;

    println(hlog, format("%s = %20.10" PLDF " (R=%10.5" PLDF " T=%10.5" PLDF ")", name, cur, l.R, l.T));
    fflush(stdout);
    }
  }

logistic current_logistic;
logistic saved_logistic;
logistic cont_logistic;

// --- continuous logistic loglikelihood
// --------------------------------------

vector<hyperpoint> vertexcoords;

// compute vertexcoords from the original embedding data
void origcoords() {
  indenter_finish im("origcoords");
  using namespace rogueviz;
  vertexcoords.resize(N);
  for(int i=0; i<N; i++) 
    vertexcoords[i] = spin(coords[i].second * degree) * xpush(coords[i].first) * C0;
  }

// compute vertexcoords from the RogueViz representation
void rvcoords() {
  indenter_finish im("rvcoords");
  using namespace rogueviz;
  vertexcoords.resize(N);
  for(int i=0; i<N; i++) 
    vertexcoords[i] = calc_relative_matrix(rogueviz::vdata[i].m->base, currentmap->gamestart(), C0) * rogueviz::vdata[i].m->at * C0;
  }

// compute vertexcoords from vertices
void cellcoords() {
  indenter_finish im("cellcoords");
  vertexcoords.resize(N);
  for(int i=0; i<N; i++) {
    vertexcoords[i] = celltopoint(vertices[i]->ascell()); // calc_relative_matrix(vertices[i]->ascell(), currentmap->gamestart(), C0) * C0;
    if(isnan(vertexcoords[i][0])) println(hlog, "got NAN for i=", i, " in lev= ", vertices[i]->lev);
    }
  }

// needs cellcoords/rvcoords/origcoords
void build_disttable() {
  indenter_finish im("build_disttable");
  int tab[N];
  for(int i=0; i<N; i++) tab[i] = N;
  disttable0.clear();
  disttable1.clear();

  using namespace rogueviz;
  
  for(int i=0; i<N; i++) {
    for(auto p: vdata[i].edges) {
      int j = p.second->i ^ p.second->j ^ i;
      if(j<i) tab[j] = i;
      }
    for(int j=0; j<i; j++) {
      ld dist = hdist(vertexcoords[i], vertexcoords[j]);
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

// --- placement loglikelihood

ld loglik_placement() {
  mycell *root = mroot;
  ld placement_loglik = 0;  
  auto seg = getsegment(root,root,0);
  for(int j=0; j<BOXSIZE; j++) {
    int qj = seg->qty[j];
    if(!qj) continue;
    placement_loglik += qj * (log(qj*1./N) - cgi.expansion->get_descendants(j).log_approx());
    }
  return placement_loglik;
  }

// --- logistic loglikelihood

ld loglik_logistic(logistic& l = current_logistic) {
  ld result = 0;
  for(int u=0; u<MAXDIST; u++) if(edgetally[u] && tally[u]-edgetally[u]) {
    result += edgetally[u] * l.lyes(u) +
      (tally[u]-edgetally[u]) * l.lno(u);
    }
  return result;
  }

// --- optimal loglikelihood

ld loglikopt() {
  ld result = 0;
  for(int u=0; u<MAXDIST; u++) result += bestll2(edgetally[u], tally[u]);
  return result;
  }

// --- optimal monotonic loglikelihood

ld loglikopt_mono() {
  vector<pair<ld, ld> > pairs;
  ld result = 0;
  for(int u=0; u<MAXDIST; u++) {
    auto p = make_pair<ld,ld> (edgetally[u], tally[u]);
    if(p.second == 0) continue;
    while(isize(pairs)) {
      auto pb = pairs.back();
      if(p.first / p.second > pb.first / pb.second)
        p.first += pb.first, p.second += pb.second, pairs.pop_back();      
      else break;
      }
    pairs.push_back(p);
    }
  for(auto p: pairs)      
    result += bestll2(p.first, p.second);
  return result;
  }

// --- compute loglikelihood according to current method

char lc_type = 'R';

ld loglik_chosen() {
  switch(lc_type) {
    case 'O':
      return loglikopt();
    case 'R':
      return loglik_logistic();
    case 'M':
      return loglikopt_mono();
    case 'C':
      return loglikopt() + loglik_placement();
    case 'D':
      return loglikopt_mono() + loglik_placement();
    default:
      return loglikopt();
    }
  }

// 1e-3 (cont), 1e-6 (normal)

// statistics

void writestats() {
  indenter_finish im("writestats");
  memoryInfo();
  println(hlog, "Vertices by distance (N = ", N, "):");
  mycell *root = mroot;
  for(int j=0; j<BOXSIZE; j++) {
    int qj = getsegment(root,root,0)->qty[j];
    if(!qj) continue;
    print(hlog, " ", j, ":", qj);
    }
  println(hlog);
  ld placement_loglik = loglik_placement();
  
  for(int u=0; u<MAXDIST; u++) if(tally[u]) {
    println(hlog, format("* %4d: %8d / %12Ld = %lf %.10" PLDF " %.10" PLDF, 
      u, edgetally[u], tally[u], double(edgetally[u]) / tally[u], 
      saved_logistic.yes(u), current_logistic.yes(u)));
    }
    
  println(hlog, "log likelihood\n");
  ld loglik_chaos = bestll2(M, N*(N-1)/2);
  ld loglik_opt = loglikopt();
  ld loglik_mono = loglikopt_mono();
  ld loglik_rt = loglik_logistic();
  println(hlog, "  placement     = ", placement_loglik);
  println(hlog, "  chaos         = ", loglik_chaos);
  println(hlog, "  optimal any   = ", loglik_opt);
  println(hlog, "  optimal mono  = ", loglik_mono);
  println(hlog, "  estimated R/T = ", loglik_logistic(saved_logistic), " (R=", saved_logistic.R, " T=", saved_logistic.T);
  println(hlog, "  optimal   R/T = ", loglik_rt, " (R=", current_logistic.R, " T=", current_logistic.T);
  
  println(hlog, "Compression ratio = %", (placement_loglik+loglik_opt)/loglik_chaos);
  }

template<class T> auto parallelize(long long N, T action) -> decltype(action(0,0)) {
#ifndef USE_THREADS
  return action(0,N);
#else
  if(threads == 1) return action(0,N);
  std::vector<std::thread> v;
  typedef decltype(action(0,0)) Res;
  std::vector<Res> results(threads);
  for(int k=0; k<threads; k++)
    v.emplace_back([&,k] () { 
      results[k] = action(N*k/threads, N*(k+1)/threads); 
      });
  for(std::thread& t:v) t.join();
  Res res = 0;
  for(Res r: results) res += r;
  return res;
#endif
  }

vector<array<ll, 2>> disttable_approx;

ld precise_hdist(hyperpoint vi, hyperpoint vj) {
  ld da = acosh(vi[2]);
  ld db = acosh(vj[2]);
  
  ld phia = atan2(vi[0], vi[1]);
  ld phib = atan2(vj[0], vj[1]);

  ld co = sinh(da) * sinh(db) * (1 - cos(phia-phib));
  // - (vi[0]*vj[0] + vi[1]*vj[1]);
  
  ld v = cosh(da - db) + co;
  if(v < 1) return 0;
  
  return acosh(v);
  }

void build_disttable_approx() {
  indenter_finish im("build_disttable_approx");

  array<ll, 2> zero = {0, 0};

  using namespace rogueviz;

  std::vector<vector<array<ll, 2>>> results(threads);
  std::vector<std::thread> v;
  for(int k=0; k<threads; k++)
    v.emplace_back([&,k] () {
      auto& dt = results[k];
      int tab[N];
      for(int i=0; i<N; i++) tab[i] = N;
      auto p = k ? nullptr : new progressbar(N/threads, "build_disttable_approx");
      for(int i=k; i<N; i+=threads) {
        if(p) (*p)++;
        for(auto p: vdata[i].edges) {
          int j = p.second->i ^ p.second->j ^ i;
          if(j<i) tab[j] = i;
          }
        for(int j=0; j<i; j++) {
          ld dist = precise_hdist(vertexcoords[i], vertexcoords[j]);
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

using logisticfun = std::function<ld(logistic&)>;

int max_steps = 100000;

void fast_loglik_cont(logistic& l, const logisticfun& f, const char *name, ld start, ld eps) {

  if(name) println(hlog, "fix_logistic_parameters");
  indenter_finish im(name);
  ld cur = f(l);
  if(name) println(hlog, format("%s = %20.10" PLDF " (R=%10.5" PLDF " T=%" PLDF ")", name, cur, l.R, l.T));

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

    if(name) println(hlog, format("%s = %20.10" PLDF " (R=%10.5" PLDF " T=%10.5" PLDF ")", name, cur, l.R, l.T));
    fflush(stdout);
    }
  }

}
