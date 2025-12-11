#ifndef _EMBEDDINGS_H_
#define _EMBEDDINGS_H_

namespace rogueviz {
namespace embeddings {

struct embedding {
  virtual hyperpoint as_hyperpoint(int id) = 0;
  virtual pair<cell*, hyperpoint> as_location(int id) = 0;
  virtual ld distance(int i, int j) { return hdist(as_hyperpoint(i), as_hyperpoint(j)); }
  virtual ld zero_distance(int id) { return hdist0(as_hyperpoint(id)); }
  virtual void save(fhstream& f) { throw hr_exception("save not implemented"); }
  virtual string name() { return "unnamed"; }
  virtual int get_dimension() { return WDIM; }
  virtual eGeometryClass get_geometry() { return cgclass; }
  virtual string get_space();
  virtual ~embedding() {}
  };

struct tiled_embedding : embedding {
  hyperpoint as_hyperpoint(int id) override {
    auto p = as_location(id);
    return currentmap->relative_matrix(p.first, currentmap->gamestart(), C0) * p.second;
    }
  };

struct untiled_embedding : embedding {
  pair<cell*, hyperpoint>  as_location(int id) override {
    return { currentmap->gamestart(), as_hyperpoint(id) };
    }
  };

extern shared_ptr<embedding> current;

/* for logistic regression */
struct logistic {
  ld R, T;
  ld yes1(ld d) { return 1/(1 + exp(d)); }
  ld no1(ld d) { return 1/(1 + exp(-d)); }
  ld nor(ld d) { return (d-R) / 2 / T; }
  ld yes(ld d) { return yes1(nor(d)); }
  ld no(ld d) { return no1(nor(d)); }
  ld lyes(ld d) { d = nor(d); return d > 200 ? -d : log(yes1(d)); }
  ld lno(ld d) { d = nor(d); return d < -200 ? d : log(no1(d)); }
  logistic() {}
  logistic(ld _R, ld _T) : R(_R), T(_T) {}  
  void setRT(ld _R, ld _T) { R = _R; T = _T; }
  };

using logisticfun = std::function<ld(logistic&)>;

extern logistic current_logistic, cont_logistic;

void enable_embedding(std::shared_ptr<embedding> pe);
void reenable_embedding();

ld loglik_cont_approx(logistic& l = current_logistic);

void fix_logistic_parameters(logistic& l, const logisticfun& f, const char *name, ld eps);
void fast_loglik_cont(logistic& l, const logisticfun& f, const char *name, ld start, ld eps);

extern double graph_R, graph_alpha, graph_T;

ld bestll(ld a, ld b);
ld bestll2(ld a, ld ab);

/* for routing */

/* greedy routing */

struct iddata {
  ld tot, suc, routedist, bestdist;
  iddata() { tot = suc = routedist = bestdist = 0; }
  };

void prepare_pairs();
void greedy_routing(iddata& d);
std::vector<int> path(int src);
int get_actual(int src);
void prepare_goal(int goal);

void read_edgelist(const string& fn);
void read_polar(const string& fn);
}
}

#endif
