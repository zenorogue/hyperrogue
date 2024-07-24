// RogueViz -- SAG embedder: some experiments
// Copyright (C) 2011-24 Zeno Rogue, see 'hyper.cpp' for details

#include "../rogueviz.h"

namespace rogueviz {
namespace sag {

int recover_from;

vector<pair<dhrg::logistic, ld>> results;

ld bestcost;

int logid;
int lastmethod = 0;
int mul_used;

bool optimized_embedding(int mul, ld bonus = 0) {
  if(logid < recover_from) { println(hlog, "skipped ", logid++, " due to recover"); return false; }
  println(hlog, "starting, logid = ", logid, " recover_from = ", recover_from, " R = ", best.R+bonus, " T = ", best.T);
  int DN = isize(sagid);
  mul_used = mul;
  lgsag_pre = best;
  lgsag_pre.R += bonus;
  lgsag = lgsag_pre;
  compute_loglik_tab(); compute_cost();
  dofullsa_iterations(mul * DN);
  optimize_sag_loglik_logistic();
  hlog.flush();
  while(true) {
    int ch = hillclimb();
    println(hlog, "changes = ", ch, " cost = ", cost, " R=", lgsag.R, " T=", lgsag.T);
    if(!ch) break;
    optimize_sag_loglik_logistic();  
    hlog.flush();
    }
  bool new_best = cost < bestcost;
  if(new_best) best = lgsag, bestcost = cost;
  sag::output_stats();
  results.emplace_back(lgsag, cost);
  return new_best;
  }

void sag_new_experiment() {
  view_each = 999999;
  println(hlog, "SAG new experiment started");
  int DN = isize(sagid);
  println(hlog, "N = ", DN);
  hlog.flush();
  twoway = true; allow_doubles = true;

  method = smLogistic;
  if(recover_from) lgsag = best;
  if(!recover_from) lgsag.R = max_sag_dist;
  if(!recover_from) lgsag.T = 1;
  compute_loglik_tab();
  compute_cost();
  best = lgsag; bestcost = HUGE_VAL;

  for(int i=10; i<=1; i--)
    optimized_embedding(15000, i);
  for(int i=1; i<=10; i++)
    optimized_embedding(15000);
  for(int i=1; i<=10; i++)
    optimized_embedding(15000, -i);

  optimized_embedding(24000);
  optimized_embedding(32000);
  optimized_embedding(40000);
  optimized_embedding(48000);
  optimized_embedding(60000);
  optimized_embedding(80000);
  optimized_embedding(100000);
  optimized_embedding(120000);
  optimized_embedding(120000);
  optimized_embedding(120000);
  }

void sag_v5() {
  println(hlog, "SAG v5 started");
  int DN = isize(sagid);
  println(hlog, "N = ", DN);
  recost_each = DN; autofix_rt = 2;
  hlog.flush();
  twoway = true; allow_doubles = true;

  method = smLogistic;
  lgsag.R = max_sag_dist;
  lgsag.T = 1;
  compute_loglik_tab();
  compute_cost();
  best = lgsag; bestcost = HUGE_VAL;

  for(int i=0; i<30; i++) optimized_embedding(10000);
  }

void sag_v6() {
  println(hlog, "SAG v6 started");
  int DN = isize(sagid);
  println(hlog, "N = ", DN);
  recost_each = DN; autofix_rt = 2;
  hlog.flush();
  twoway = true; allow_doubles = true;

  method = smLogistic;
  lgsag.R = max_sag_dist;
  lgsag.T = 1;
  compute_loglik_tab();
  compute_cost();
  best = lgsag; bestcost = HUGE_VAL;

  for(int i=0; i<30; i++) optimized_embedding(100000);

  autofix_rt = 1;
  for(int i=0; i<30; i++) optimized_embedding(100000);

  autofix_rt = 0;
  for(int i=0; i<30; i++) optimized_embedding(100000);
  }

void sag_test_mul() {
  allow_doubles = true; twoway = true;
  int DN = isize(sagid);
  // lgsag.R=9.19925; lgsag.T=0.587723;
  method = smLogistic;
  lgsag.R = max_sag_dist;
  lgsag.T = 1;
  compute_loglik_tab();
  compute_cost();
  best = lgsag; bestcost = 999999;
  recost_each = DN; autofix_rt = 2;
  output_fullsa = false;

  println(hlog, "sag_test_mul started");

  if(1) for(int mul=25;; mul *= 2) for(int af: {3, 2, 1, 0}) {
    autofix_rt = af;
    ld tcost = 0;
    ld tcost2 = 0;
    int qty = 100;
    vector<ld> costs;
    // println(hlog, "R=", best.R, " T=", best.T);
    for(int i=0; i<qty; i++) {
      println(hlog, tie(mul, af, i));
      lgsag = best; compute_loglik_tab(); compute_cost();
      sag::dofullsa_iterations(mul * DN);
      sag::optimize_sag_loglik_logistic();
      checkmark_cost = HUGE_VAL;
      while(sag::checkmark_hillclimb()) sag::optimize_sag_loglik_logistic();
      tcost += cost;
      tcost2 += cost * cost;
      costs.push_back(cost);
      bool new_best = cost < bestcost;
      if(new_best) best = lgsag, bestcost = cost;
      // println(hlog, "CSV;", mul, ";", af, ";", i, ";", cost);
      }
    sort(costs.begin(), costs.end());
    println(hlog, "mul=", mul, " AF=", autofix_rt, " ECost=", tcost/qty, " sigma Cost = ", sqrt(tcost2/qty - tcost*tcost/qty/qty), " : ", costs);
    }

  auto tpair = [&] (int lt, int ht) {
    ld tcost = 0;

    for(int i=0; i<20; i++) {

      sagnode.clear();
      sagnode.resize(isize(sagcells), -1);
      for(int i=0; i<DN; i++) sagid[i] = i;
      for(int i=0; i<DN; i++) sagnode[i] = i;
      compute_cost();

      lowtemp = 20;
      hightemp = 20;
      sag::dofullsa_iterations(20 * DN);

      lowtemp = lt;
      hightemp = ht;
      sag::dofullsa_iterations(1000 * DN);
      tcost += cost;
      }
    println(hlog, "lt=", lt, " ht=", ht, " tcost=", tcost);    
    };

  if(false)
  for(int lt: {-3, -5, -10, -15, -20, -25})
  for(int ht: {-2, -1, 0, 1, 2, 5, 10})
    tpair(lt, ht);
  }

void write_colors(string s) {
  auto_orth(true);
  if(s == "-") return;
  fhstream f(s, "wt");
  for(int i=0; i<isize(vdata); i++) {
    println(f, vdata[i].name, ",", format("%8x", vdata[i].cp.color1));
    }
  }

void sag_new_experiment_viz() {
  view_each = 999999;
  println(hlog, "SAG new experiment started");
  int DN = isize(sagid);
  println(hlog, "N = ", DN);
  hlog.flush();
  twoway = true; allow_doubles = true;

  method = smLogistic;
  lgsag.R = max_sag_dist;
  lgsag.T = 1;
  compute_loglik_tab();
  compute_cost();
  best = lgsag; bestcost = HUGE_VAL;

  optimized_embedding(15000, 0);
  optimized_embedding(15000, 0);
  }

bool report_tempi = false;
string auto_save;

void output_stats() {
  if(auto_save != "" && cost < best_cost) {
    println(hlog, "cost ", cost, " beats ", best_cost);
    best_cost = cost;
    sag::save_sag_solution(auto_save);
    }
  println(hlog, "solution: ", sagid);
  int DN = isize(sagid);
  auto [mAP, MeanRank] = compute_mAP();
  dhrg::iddata routing_result;
  if(!known_pairs) { known_pairs = true; dhrg::prepare_pairs(DN, [] (int i) { return edges_yes[i]; }); }
  dhrg::greedy_routing(routing_result, [] (int i, int j) { return sagdist[sagid[i]][sagid[j]]; });
  print(hlog, "CSV;", logid++, ";", isize(sagnode), ";", DN, ";", isize(sagedges), ";", lgsag_pre.R, ";", lgsag_pre.T, ";", lgsag.R, ";", lgsag.T, ";", cost, ";", mAP, ";", MeanRank, ";", routing_result.suc / routing_result.tot, ";", routing_result.routedist / routing_result.bestdist);
  if(lastmethod) print(hlog, ";", lastmethod);
  if(mul_used) print(hlog, ";", mul_used);
  if(report_tempi) print(hlog, ";", hightemp,";",lowtemp,";",format("%lld", numiter));
  println(hlog);
  }

int exp_read_args() {
#if CAP_COMMANDLINE
  using namespace arg;

  if(0) ;

  else if(argis("-sag-new")) sag_new_experiment();
  else if(argis("-sag-v5")) sag_v5();
  else if(argis("-sag-v6")) sag_v6();
  else if(argis("-sag-new-viz")) sag_new_experiment_viz();
  else if(argis("-sag-test-mul")) sag_test_mul();
  else if(argis("-sag-write-colors")) {
    shift(); write_colors(args());
    }

  else if(argis("-sag-recover")) {
    shift(); best.R = argf();
    shift(); best.T = argf();
    shift(); bestcost = argf();
    shift(); recover_from = argi();
    println(hlog, "set recover_from to ", recover_from);
    // 58.6509;7.08961;26492.1
    }

  else if(argis("-sag-load-solution")) {
    PHASE(3); shift(); sag::load_sag_solution_basic(args());
    method = smLogistic;
    lgsag.R = max_sag_dist;
    lgsag.T = 1;
    opt_debug = true;
    twoway = true; allow_doubles = true;
    optimize_sag_loglik_auto();
    }

  else if(argis("-sagstats-logid")) {
    shift(); logid = argi();
    }
  else if(argis("-sag0")) {
    sag::report_tempi = true;
    numiter = 0;
    }
  else if(argis("-sagsave-auto")) {
    PHASE(3); shift(); auto_save = args();
    }
  else if(argis("-sagstats")) {
    output_stats();
    }

  else return 1;  
#endif
  return 0;
  }

int ahexp = addHook(hooks_args, 100, exp_read_args);

}
}
