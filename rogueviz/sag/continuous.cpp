#include "../rogueviz.h"
#include <sys/mman.h>
#include <fcntl.h>

namespace rogueviz {
namespace sag {

bool embedding;
dhrg::logistic lgemb(1, 1);
vector<hyperpoint> placement;
int embiter;

ld pdist(int i, int j) {
  return pdist(placement[i], placement[j]);
  };

void disttable_add(ld dist, int qty0, int qty1) {
  using namespace dhrg;
  size_t i = dist * llcont_approx_prec;
  constexpr array<ll, 2> zero = {0, 0};
  while(disttable_approx.size() <= i) disttable_approx.push_back(zero);
  disttable_approx[i][0] += qty0;
  disttable_approx[i][1] += qty1;
  }

void compute_loglik() {    
  dhrg::llcont_approx_prec = 10;
  
  dhrg::disttable_approx.clear();
  int DN = isize(sagid);
  for(int i=0; i<DN; i++)
  for(int j=0; j<i; j++)
    disttable_add(pdist(i, j), 1, 0);

  for(int i=0; i<isize(sagedges); i++) {
    edgeinfo& ei = sagedges[i];
    if(ei.i != ei.j)
      disttable_add(pdist(ei.i, ei.j), -1, 1);
    }
  
  dhrg::logisticfun lc = dhrg::loglik_cont_approx;

  dhrg::fast_loglik_cont(lgemb, lc, nullptr, 1, 1e-5);
  
  println(hlog, "loglik = ", format("%.6f", lc(lgemb)), " R = ", lgemb.R, " T = ", lgemb.T, " iterations = ", embiter);
  }

void prepare_embedding() {
  map<int, transmatrix> maps;    
  vector<int> visited;

  auto visit = [&] (int id, const transmatrix& T) {
    if(maps.count(id)) return;
    maps[id] = T;
    visited.push_back(id);
    };
    
  visit(0, Id);
  for(int i=0; i<isize(visited); i++) {
    cell *c0 = sagcells[i].first;
    transmatrix T0 = maps[i];
    for(int d=0; d<c0->type; d++)
      if(ids.count({c0->move(d), 0}))
        visit(ids[{c0->move(d), 0}], T0 * currentmap->adj(c0, d));
    }
  
  int DN = isize(sagid);
  placement.resize(DN);
  for(int i=0; i<DN; i++) placement[i] = tC0(maps[sagid[i]]);    
  }
  
void reassign_embedding() {
  int DN = isize(sagid);
  for(int i=0; i<DN; i++) {
    vdata[i].m->base = sagcells[0].first;
    vdata[i].m->at = rgpushxto0(placement[i]);
    virtualRebase(vdata[i].m);
    forgetedges(i);
    }
  shmup::fixStorage();
  }

void improve_embedding() {
  embiter++;
  if(placement.empty()) {
    prepare_embedding();
    compute_loglik();
    }
  ld eps = .1;
  int DN = isize(sagid);

  hyperpoint h = C0;
  for(int i=0; i<WDIM; i++) h[i] += (hrandf() - 0.5) * eps;
  h = normalize(h);
  
  auto nplacement = placement;
  parallelize(DN, [&] (int a, int b) {
    for(int i=a; i<b; i++) {

      hyperpoint np = rgpushxto0(placement[i]) * h;
      
      ld change;
      for(auto e: edges_yes[i]) change -= lgemb.lyes(pdist(placement[i], placement[e]));
      for(auto e: edges_no[i]) change -= lgemb.lno(pdist(placement[i], placement[e]));
      for(auto e: edges_yes[i]) change += lgemb.lyes(pdist(np, placement[e]));
      for(auto e: edges_no[i]) change += lgemb.lno(pdist(np, placement[e]));

      if(change > 0) nplacement[i] = np;
      }
    return 0;
    });
  
  placement = nplacement;
  }
  
void save_embedding(const string& fname) {
  fhstream f(fname, "wt");
  for(int i=0; i<isize(sagid); i++) {
    println(f, vdata[i].name);
    for(int d=0; d<MDIM; d++)
      println(f, format("%.20f", placement[i][d]));
    }
  }

void load_embedding(const string& fname) {
  prepare_embedding();
  fhstream f(fname, "rt");
  if(informat == 2) {
    /* H2 embedding */
    while(!feof(f.f)) {
      string lab = scan<string>(f);
      int id;
      if(!labeler.count(lab)) {
        printf("unknown vertex: %s\n", lab.c_str());
        continue;
        }
      else id = getid(lab);
      ld alpha, r;
      if(1) {
        dynamicval<eGeometry> g(geometry, gNormal);
        hyperpoint h;
        for(int d=0; d<MDIM; d++) h[d] = scan<ld>(f);
        alpha = atan2(h);
        r = hdist0(h);
        println(hlog, "read ", lab, " as ", h, " which is ", tie(alpha, r));
        }
      placement[id] = direct_exp(cspin(0, 2, alpha) * ctangent(0, r));
      println(hlog, "dist = ", pdist(placement[id], C0), " expected: ", r);
      }
    }
  else if(informat == 3) {
    /* BFKL */
    string ignore;
    if(!scan(f, ignore, ignore, ignore, ignore, ignore, ignore, ignore, ignore)) {
      printf("Error: incorrect format of the first line\n"); exit(1);
      }      
    while(true) {
      string lab = scan<string>(f);
      if(lab == "" || lab == "#ROGUEVIZ_ENDOFDATA") break;
      ld r, alpha;
      if(!scan(f, r, alpha)) { printf("Error: incorrect format of r/alpha\n"); exit(1); }
      hyperpoint h = spin(alpha * degree) * xpush0(r);

      if(!labeler.count(lab)) {
        printf("unknown vertex: %s\n", lab.c_str());
        }
      else {
        int id = getid(lab);
        placement[id] = h;
        }      
      }
    }
  else if(informat == 4) {
    while(true) {
      string lab = scan<string>(f);
      if(lab == "") break;
      ld r, alpha;
      if(!scan(f, r, alpha)) { printf("Error: incorrect format of r/alpha\n"); exit(1); }
      hyperpoint h = spin(alpha) * xpush0(r);

      if(!labeler.count(lab)) {
        printf("unknown vertex: %s\n", lab.c_str());
        }
      else {
        int id = getid(lab);
        placement[id] = h;
        }      
      }
    }
  else {
    while(!feof(f.f)) {
      string lab = scan<string>(f);
      int id;
      if(!labeler.count(lab)) {
        printf("unknown vertex: %s\n", lab.c_str());
        continue;
        }
      else id = getid(lab);
      hyperpoint h;
      for(int d=0; d<MDIM; d++) h[d] = scan<ld>(f);
      placement[id] = h;
      }
    }
  reassign_embedding();
  compute_loglik();
  }

int embturn = 1;
void embedding_iterate() {
  int t1 = SDL_GetTicks();
  for(int i=0; i<embturn; i++) {
    improve_embedding();
    }
  int t2 = SDL_GetTicks();
  int t = t2 - t1;
  if(t < 50) embturn *= 2;
  else if(t > 200) embturn = (embturn + 1) / 2;
  else embturn = (embturn * 100 + (t-1)) / t;
  
  compute_loglik();
  if(auto_visualize) reassign_embedding();
  }

int embed_read_args() {
#if CAP_COMMANDLINE
  using namespace arg;

  if(0) ;

  else if(argis("-sagembed")) {
    sag::embedding = true;
    }
  else if(argis("-sagembedoff")) {
    sag::embedding = false;
    }
  else if(argis("-sagsavee")) {
    PHASE(3); shift(); sag::save_embedding(args());
    }
  else if(argis("-sagloade")) {
    PHASE(3); shift(); sag::load_embedding(args());
    }

  else return 1;  
#endif
  return 0;
  }

int ahembed = addHook(hooks_args, 100, embed_read_args);

}
}
