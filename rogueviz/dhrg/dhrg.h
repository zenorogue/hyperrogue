#ifndef _DHRG_H_
#define _DHRG_H_
#define DHRGVER "7.1"
#include "../rogueviz.h"

#define LONG_BRACKETS

namespace dhrg {

using namespace hr;

typedef long long ll;

#ifndef BOXSIZE
static const int BOXSIZE = 32;
#endif
static const int MAXDIST = (2*BOXSIZE);
static const int SETS = 4;

struct segment;

inline cell *croot() { return currentmap->gamestart(); }

extern int M;
extern vector<struct mycell*> vertices;

extern vector<ld> disttable0, disttable1;

void memoryInfo();

void cellcoords();
void origcoords();
void build_disttable();

void dhrg_init();
bool dhrg_animate(int sym, int uni);

/* implemented in loglik.cpp: */

/* for logistic regression */
struct logistic {
  ld R, T;
  ld yes(ld d) { return 1/(1 + exp((d-R) / 2 / T)); }
  ld no(ld d) { return 1/(1 + exp(-(d-R) / 2 / T)); }
  ld lyes(ld d) { return log(yes(d)); }
  ld lno(ld d) { return log(no(d)); }
  logistic() {}
  logistic(ld _R, ld _T) : R(_R), T(_T) {}  
  void setRT(ld _R, ld _T) { R = _R; T = _T; }
  };

extern ld llcont_approx_prec;
extern vector<array<ll, 2>> disttable_approx;

using logisticfun = std::function<ld(logistic&)>;

extern logistic current_logistic;

ld loglik_cont_approx(logistic& l = current_logistic);

void fast_loglik_cont(logistic& l, const logisticfun& f, const char *name, ld start, ld eps);

/* greedy routing */

struct iddata {
  ld tot, suc, routedist, bestdist;
  iddata() { tot = suc = routedist = bestdist = 0; }
  };

using neighborhoodfun = std::function<vector<int> (int)>;
using distfun = std::function<ld(int a, int b)>;

void prepare_pairs(int N, const neighborhoodfun& nei);
void greedy_routing(iddata& d, const distfun& distance_function);

}
#endif
