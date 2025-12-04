// log-likelihood computation

namespace dhrg {

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
  int N = isize(rogueviz::vdata);
  
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
  int N = isize(rogueviz::vdata);
  progressbar pb(N, "Destroying tallies");
  for(int i=0; i<N; i++) add_to_set(vertices[i], -1, 0), pb++;
  for(int i=0; i<MAXDIST; i++)
    tally[i] = edgetally[i] = 0;
  }

// various methods of loglikelihood computation

using rogueviz::embeddings::logistic;

logistic current_logistic;
logistic saved_logistic;

// --- placement loglikelihood

ld loglik_placement() {
  mycell *root = mroot;
  ld placement_loglik = 0;  
  auto seg = getsegment(root,root,0);
  int N = isize(rogueviz::vdata);
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
  for(int u=0; u<MAXDIST; u++) result += rogueviz::embeddings::bestll2(edgetally[u], tally[u]);
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
    result += rogueviz::embeddings::bestll2(p.first, p.second);
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
  int N = isize(rogueviz::vdata);
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
    println(hlog, hr::format("* %4d: %8d / %12lld = %lf %.10" PLDF " %.10" PLDF,
      u, edgetally[u], tally[u], double(edgetally[u]) / tally[u], 
      saved_logistic.yes(u), current_logistic.yes(u)));
    }
    
  println(hlog, "log likelihood\n");
  ld loglik_chaos = rogueviz::embeddings::bestll2(M, N*(N-1)/2);
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

}
