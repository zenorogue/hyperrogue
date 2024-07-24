// RogueViz -- SAG embedder: the implementation of Simulated Annealing
// Copyright (C) 2011-2024 Zeno Rogue, see 'hyper.cpp' for details

#include "../rogueviz.h"

namespace rogueviz {
namespace sag {

enum eSagmode { sagOff, sagHC, sagSA };
eSagmode sagmode; // 0 - off, 1 - hillclimbing, 2 - SA
const char *sagmodes[3] = {"off", "HC", "SA"};

ld temperature = 0;

int hightemp = 10;
int lowtemp = -15;

long long numiter = 0;  

int vizsa_start;
int vizsa_len = 5;
  
bool chance(double p) {
  p *= double(hrngen.max()) + 1;
  auto l = hrngen();
  auto pv = (decltype(l)) p;
  if(l < pv) return true;
  if(l == pv) return chance(p-pv);
  return false;
  }

bool twoway = false;
int moves, nomoves;

void saiter() {
  int DN = isize(sagid);
  int t1 = hrand(DN);
  int sid1 = sagid[t1];
  
  int sid2;
  
  int s = twoway ? pick(1,4) : hrand(4)+1;

  if(s == 4) sid2 = hrand(isize(sagcells));
  else {
    sid2 = sid1;
    for(int ii=0; ii<s; ii++) sid2 = hrand_elt(neighbors[sid2]);
    }
  int t2 = allow_doubles ? -1 : sagnode[sid2];
  
  sagnode[sid1] = -1; sagid[t1] = -1;
  sagnode[sid2] = -1; if(t2 >= 0) sagid[t2] = -1;
  
  double change = 
    costat(t1,sid2) + costat(t2,sid1) - costat(t1,sid1) - costat(t2,sid2);
  
  sagnode[sid1] = t1; sagid[t1] = sid1;
  sagnode[sid2] = t2; if(t2 >= 0) sagid[t2] = sid2;
  
  if(change > 0 && (sagmode == sagHC || !chance(exp(-change * exp(-temperature))))) { nomoves++; return; }
  moves++;

  sagnode[sid1] = t2; sagnode[sid2] = t1;
  sagid[t1] = sid2; if(t2 >= 0) sagid[t2] = sid1;

  if(should_good) {
    auto dcost = cost;
    compute_cost();
    println(hlog, "dcost=", dcost, " change=", change, " cost=", cost, " error = ", dcost + change - cost);
    if(abs(dcost + change - cost) > .1) throw hr_exception("dcost fail");
    cost = dcost;
    }

  cost += change;
  }

ld checkmark_cost;

int hillclimb() {
  int DN = isize(sagid);
  int changes = 0;
  vector<ld> succ;

  for(int t1=0; t1<DN; t1++) {
    int sid1 = sagid[t1];    
    for(int sid2: neighbors[sid1]) {
      int t2 = allow_doubles ? -1 : sagnode[sid2];

      sagnode[sid1] = -1; sagid[t1] = -1;
      sagnode[sid2] = -1; if(t2 >= 0) sagid[t2] = -1;
      
      double change = 
        costat(t1,sid2) + costat(t2,sid1) - (costat(t1,sid1) + costat(t2,sid2));

      if(change >= -1e-10) {
        sagnode[sid1] = t1; sagid[t1] = sid1;
        sagnode[sid2] = t2; if(t2 >= 0) sagid[t2] = sid2;
        }
      else {
        changes++;
        sagnode[sid1] = t2; sagnode[sid2] = t1;
        sagid[t1] = sid2; if(t2 >= 0) sagid[t2] = sid1;
        cost += change;
        succ.push_back(change);
        break;
        }
      }
    }
  // println(hlog, "successes = ", succ);

  return changes;
  }

int checkmark_hillclimb() {
  compute_cost();
  if(cost > checkmark_cost) {
    println(hlog, "checkmark failed");
    throw hr_exception("checkmark failed");
    return 0;
    }
  checkmark_cost = cost;
  return hillclimb();
  }

int view_each = 1000;

void dofullsa(ld satime) {
  sagmode = sagSA;
  int t1 = SDL_GetTicks();
  int tl = -999999;
  
  while(true) {
    int t2 = SDL_GetTicks();
    double d = (t2-t1) / (1000. * satime);
    if(d > 1) break;

    temperature = hightemp - (d*(hightemp-lowtemp));
    for(int i=0; i<10000; i++) {
      numiter++;
      sag::saiter();
      }
    
    if(t2 - tl > view_each * .98) {
      tl = t2;
      println(hlog, format("it %12lld temp %6.4f [1/e at %13.6f] cost = %f ",
        numiter, double(sag::temperature), (double) exp(sag::temperature),
        double(sag::cost)));
      }
    
    }
  
  temperature = -5;
  sagmode = sagOff;
  create_viz();
  }

/** after how many moves should we fix the values of R and T during SA */
int recost_each;

/** 2 = fix both R and T, 1 = fix only R, 0 = fix nothing, 3 = fix both R and T but avoid fixing early */
int autofix_rt;

void optimize_sag_loglik_logistic();
void compute_loglik_tab();

bool output_fullsa = true;

void dofullsa_iterations(long long saiter) {
  sagmode = sagSA;
  moves = 0; nomoves = 0; numiter = 0;

  // decltype(SDL_GetTicks()) t1 = SDL_GetTicks();

  // println(hlog, "before dofullsa_iterations, cost = ", double(sag::cost), " iterations = ", fts(saiter));

  ld last_ratio;

  int lpct = 0;

  bool was_fixed = false;

  for(int i=0; i<saiter; i++) {

    temperature = hightemp - ((i+.5)/saiter*(hightemp-lowtemp));
    numiter++;
    sag::saiter();

    if(recost_each && moves > recost_each) {
      last_ratio = moves / (moves + nomoves + 0.);
      if(((autofix_rt == 3 && was_fixed) || nomoves > recost_each) && autofix_rt) {
        was_fixed = true;
        optimize_sag_loglik_logistic();
        if(autofix_rt == 1) {
          lgsag.T = best.T;
          compute_loglik_tab();
          compute_cost();
          }
        }
      nomoves = 0; moves = 0;
      }

    int cpct = numiter * 20 / (saiter-1);

    if(cpct > lpct && output_fullsa) {
      lpct = cpct;
      println(hlog, format("it %12lld ratio %6.3f temp %8.4f step %9.3g cost %9.2f R=%8.4f T=%8.4f",
        numiter, last_ratio, double(sag::temperature), (double) exp(sag::temperature), cost, lgsag.R, lgsag.T));
      }

    /* if(numiter % 10000 == 0) {
      auto t2 = SDL_GetTicks();
      if(int(t2 - t1) > view_each) {
        t1 = t2;
        println(hlog, format("it %12Ld temp %6.4f [1/e at %13.6f] cost = %f ",
          numiter, double(sag::temperature), (double) exp(sag::temperature),
          double(sag::cost)));
        }
      } */
    }

  // println(hlog, "after dofullsa_iterations, cost = ", double(sag::cost));

  temperature = -5;
  sagmode = sagOff;
  create_viz();
  }

int anneal_read_args() {
#if CAP_COMMANDLINE
  using namespace arg;

  if(0) ;

  else if(argis("-sagtemp")) {
    shift(); sag::hightemp = argi();
    shift(); sag::lowtemp = argi();
    }

  else if(argis("-sagfull")) {
    shift(); sag::dofullsa(argf());
    }
  else if(argis("-sagfulli")) {
    shift(); sag::dofullsa_iterations(argll());
    }
  else if(argis("-sagmode")) {
    shift();
    vizsa_start = 0;
    sagmode = (eSagmode) argi();
    if(sagmode == sagSA) {
      shift(); temperature = argf();
      }
    }
  else if(argis("-sag-recost")) {
    method = smLogistic; prepare_method();
    shift(); recost_each = argi();
    shift(); autofix_rt = argi();
    }

  else return 1;  
#endif
  return 0;
  }

int ahanneal = addHook(hooks_args, 100, anneal_read_args);

}
}
