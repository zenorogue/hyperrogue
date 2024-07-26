// RogueViz -- SAG embedder: implementation of SAG method evaluation
// Copyright (C) 2011-24 Zeno Rogue, see 'hyper.cpp' for details

#include "../rogueviz.h"

#include "../dhrg/dhrg.h"
#include "../statistics.cpp"

namespace rogueviz {
namespace sag {

enum eSagMethod { smClosest, smLogistic, smMatch };
eSagMethod method;

vector<string> method_names = {"closest", "logistic", "match"};

int method_count = 3;

/* parameters for smMatch */
ld match_a = 1, match_b = 0;

/* parameters for smLogistic */
dhrg::logistic lgsag(1, 1), lgsag_pre(1, 1);

vector<ld> loglik_tab_y, loglik_tab_n;

dhrg::logistic best;

bool opt_debug = false;

bool should_good = false;

double costat(int vid, int sid) {
  if(vid < 0) return 0;
  double cost = 0;

  switch(method) {
    case smLogistic: {
      auto s = sagdist[sid];
      for(auto j: edges_yes[vid]) if(sagid[j] >= -1)
        cost += loglik_tab_y[s[sagid[j]]];
      for(auto j: edges_no[vid]) if(sagid[j] >= -1)
        cost += loglik_tab_n[s[sagid[j]]];
      return -cost;
      }

    case smMatch: {
      vertexdata& vd = vdata[vid];
      for(int j=0; j<isize(vd.edges); j++) {
        edgeinfo *ei = vd.edges[j].second;
        int t2 = vd.edges[j].first;
        if(sagid[t2] != -1) {
          ld cdist = sagdist[sid][sagid[t2]];
          ld expect = match_a / ei->weight2 + match_b;
          ld dist = cdist - expect;
          cost += dist * dist;
          }
        }
      return cost;
      }

    case smClosest: {
      vertexdata& vd = vdata[vid];
      for(int j=0; j<isize(vd.edges); j++) {
        edgeinfo *ei = vd.edges[j].second;
        int t2 = vd.edges[j].first;
        if(sagid[t2] != -1) cost += sagdist[sid][sagid[t2]] * ei->weight2;
        }
      
      if(!hubval.empty()) {
        for(auto sid2: neighbors[sid]) {
          int vid2 = sagnode[sid2];
          if(vid2 >= 0 && (hubval[vid] & hubval[vid]) == 0)
            cost += hub_penalty;
          }
        }
      
      return cost;
      }
    }

  throw hr_exception("unknwon SAG method");
  }

double cost;

double best_cost = 1000000000;

void compute_cost() {
  int DN = isize(sagid);
  cost = 0;
  for(int i=0; i<DN; i++)
    cost += costat(i, sagid[i]);
  cost /= 2;
  }
  
void compute_loglik_tab() {
  loglik_tab_y.resize(max_sag_dist);
  loglik_tab_n.resize(max_sag_dist);
  for(int i=0; i<max_sag_dist; i++) {
    loglik_tab_y[i] = lgsag.lyes(i);
    loglik_tab_n[i] = lgsag.lno(i);
    }
  }

void compute_auto_rt() {
  ld sum0 = 0, sum1 = 0, sum2 = 0;

  for(auto i: sagdist) {
    sum0 ++;
    sum1 += i;
    sum2 += i*i;
    }

  lgsag.R = sum1 / sum0;
  lgsag.T = sqrt((sum2 - sum1*sum1/sum0) / sum0);
  println(hlog, "automatically set R = ", lgsag.R, " and ", lgsag.T, " max_sag_dist = ", max_sag_dist);
  if(method == smLogistic) compute_loglik_tab();
  }

void optimize_sag_loglik_logistic() {
  vector<int> indist(max_sag_dist, 0);
  
  const int mul = 1;

  int N = isize(sagid);
  for(int i=0; i<N; i++)
  for(int j=0; j<i; j++) {
    int d = sagdist[sagid[i]][sagid[j]];
    indist[d]++;
    }
  
  vector<int> pedge(max_sag_dist, 0);
    
  for(int i=0; i<isize(sagedges); i++) {
    edgeinfo& ei = sagedges[i];
    // if(int(sagdist[sagid[ei.i]][sagid[ei.j]] * mul) == 136) printf("E %d,%d\n", ei.i, ei.j);
    if(ei.i != ei.j)
    if(ei.weight >= sag_edge->visible_from)
      pedge[sagdist[sagid[ei.i]][sagid[ei.j]] * mul]++;
    }
  
  if(opt_debug) for(int d=0; d<max_sag_dist; d++) 
    if(indist[d])
      printf("%2d: %7d/%7d %7.3lf\n", 
        d, pedge[d], indist[d], double(pedge[d] * 100. / indist[d]));
      
  ld loglik = 0;
  for(int d=0; d<max_sag_dist; d++) {
    int p = pedge[d], pq = indist[d];
    int q = pq - p;
    if(p && q) {
      loglik += p * log(p) + q * log(q) - pq * log(pq);
      if(opt_debug) println(hlog, tie(d, p, q), loglik);
      }
    }
  
  if(opt_debug) println(hlog, "loglikelihood best = ", fts(loglik));
  
  auto logisticf = [&] (dhrg::logistic&  l) {
    ld loglik = 0;
    for(int d=0; d<max_sag_dist; d++) {
      int p = pedge[d], pq = indist[d];
      if(p) loglik += p * l.lyes(d);
      if(pq > p) loglik += (pq-p) * l.lno(d);
      }
    return loglik;
    };

  if(opt_debug) println(hlog, "cost = ", cost, " logisticf = ", logisticf(lgsag), " R= ", lgsag.R, " T= ", lgsag.T);
  if(should_good && abs(cost + logisticf(lgsag)) > 0.1) throw hr_exception("computation error");
  
  dhrg::fast_loglik_cont(lgsag, logisticf, nullptr, 1, 1e-5);
  if(opt_debug) println(hlog, "loglikelihood logistic = ", logisticf(lgsag), " R= ", lgsag.R, " T= ", lgsag.T);    
  
  if(method == smLogistic) {
    compute_loglik_tab();
    compute_cost();
    if(opt_debug) println(hlog, "cost = ", cost);
    }
  }

void optimize_sag_loglik_match() {
  if(state &~ SS_WEIGHTED) return;
  stats::leastsquare_solver<2> lsqs;

  for(auto& ei: sagedges) {
    ld y = sagdist[sagid[ei.i]][sagid[ei.j]];
    ld x = 1. / ei.weight;
    lsqs.add_data({{x, 1}}, y);
    }

  array<ld, 2> solution = lsqs.solve();
  match_a = solution[0];
  match_b = solution[1];

  println(hlog, "got a = ", match_a, " b = ", match_b);
  if(method == smMatch)
    prepare_graph();
  }

void optimize_sag_loglik_auto() {
  if(method == smLogistic) optimize_sag_loglik_logistic();
  if(method == smMatch) optimize_sag_loglik_match();
  }

pair<ld, ld> compute_mAP() {
  int DN = isize(sagid);

  ld meanrank = 0;
  int tgood = 0;
  ld maprank = 0;

  for(int i=0; i<DN; i++) {
    vector<int> alldist(max_sag_dist, 0);
    for(int j=0; j<DN; j++) if(i != j) alldist[sagdist[sagid[i]][sagid[j]]]++;
    vector<int> edgedist(max_sag_dist, 0);
    for(auto j: edges_yes[i]) edgedist[sagdist[sagid[i]][sagid[j]]]++;

    int pgood = 0;
    ld bad = 0;
    ld ap = 0;
    ld pall = 0;

    for(int j=0; j<max_sag_dist; j++) {
      ld good = edgedist[j];
      ld all = alldist[j];
      ld err = all - good;

      bad += err / 2.;
      meanrank += bad * good;
      bad += err / 2.;

      for(int k=0; k<good; k++) {
        pgood++, pall++;
        pall += err/2 / good;
        ap += pgood / pall;
        pall += err/2 / good;
        }
      if(!good) pall += err;
      }

    tgood += pgood;
    if(pgood) maprank += ap / pgood;
    }
  return make_pair(maprank / DN, meanrank / tgood);
  }

void prepare_method() {
  if(method == smLogistic) compute_loglik_tab();
  optimize_sag_loglik_auto();
  }

bool known_pairs = false;

int function_read_args() {
#if CAP_COMMANDLINE
  using namespace arg;

  if(0) ;

  else if(argis("-sagrt")) {
    shift(); sag::lgsag.R = argf();
    shift(); sag::lgsag.T = argf();
    if(method == smLogistic) compute_loglik_tab();
    }

  else if(argis("-sagmatch-ab")) {
    shift(); sag::match_a = argf();
    shift(); sag::match_b = argf();
    if(method == smMatch) prepare_graph();
    }

  else if(argis("-sagrt-auto")) {
    compute_auto_rt();
    }

  else if(argis("-sag-method-closest")) {  
    method = smClosest; prepare_method();
    }

  else if(argis("-sag-method-logistic")) {  
    method = smLogistic; prepare_method();
    }

  else if(argis("-sag-method-match")) {  
    method = smMatch; prepare_method();
    }

  else if(argis("-sag-logistic-recalc")) {
    method = smLogistic; prepare_method();
    }

  else if(argis("-sagloglik-l")) {
    sag::optimize_sag_loglik_logistic();
    }
  else if(argis("-sagloglik-m")) {
    sag::optimize_sag_loglik_match();
    }
  else if(argis("-sagloglik-a")) {
    sag::optimize_sag_loglik_auto();
    }

  else return 1;  
#endif
  return 0;
  }

int ahfun = addHook(hooks_args, 100, function_read_args);

}
}
