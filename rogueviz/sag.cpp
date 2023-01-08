#include "rogueviz.h"

// SAG visualizer (e.g. Reddit roguelikes, GitHub languages)
//-----------------------------------------------------------

// see: https://www.youtube.com/watch?v=mDG3_f8R2Ns (SAG boardgames)
// see: https://www.youtube.com/watch?v=WSyygk_3j9o (SAG roguelikes)
// see: https://www.youtube.com/watch?v=HWQkDkeEUeM (SAG programming languages)

#include "dhrg/dhrg.h"
#include <thread>
#include "leastsquare.cpp"

namespace rogueviz {

namespace sag {

  int threads = 1;

  int informat; /* format ID */

  bool turn(int delta);
  
  int sagpar = 0;

  bool angular = false;
  bool report_tempi = false;

  int best_cost = 1000000000;

  enum eSagmode { sagOff, sagHC, sagSA };
  
  eSagmode sagmode; // 0 - off, 1 - hillclimbing, 2 - SA

  const char *sagmodes[3] = {"off", "HC", "SA"};
  
  ld temperature = -4;
  const char *loadfname;
  
  string auto_save;

  bool auto_visualize = true;

  int vizsa_start;
  int vizsa_len = 5;
  
  /** all the SAG cells */
  vector<cell*> sagcells;

  /** table of distances between SAG cells */
  vector<vector<unsigned short>> sagdist;

  /** what node is on sagcells[i] */
  vector<int> sagnode;

  /** node i is on sagcells[sagid[i]] */
  vector<int> sagid;

  /** sagcells[ids[c]]] == c */
  map<cell*, int> ids;

  /** if i in neighbors[j], sagcells[i] is a neighbor of sagcells[j] */
  vector<vector<int>> neighbors;
  
  ld pdist(hyperpoint hi, hyperpoint hj);
  
  /** matrix for every sagcell */
  vector<transmatrix> cell_matrix;
  
  /** precision of geometric distances */
  int gdist_prec;

  /** max edge for dijkstra */
  int dijkstra_maxedge;
  
  /** the maximum value in sagdist +1 */
  int max_sag_dist;

  vector<edgeinfo> sagedges;  
  vector<vector<int>> edges_yes, edges_no;

  enum eSagMethod { smClosest, smLogistic, smMatch };
  eSagMethod method;

  bool loglik_repeat;

  /* parameters for smMatch */
  ld match_a = 1, match_b = 0;

  /* parameters for smLogistic */
  dhrg::logistic lgsag(1, 1);
  
  vector<ld> loglik_tab_y, loglik_tab_n;

  int ipturn = 100;
  long long numiter = 0;
  
  int hightemp = 10;
  int lowtemp = -15;
  
  /* for the embedding method: */  
  bool embedding;
  dhrg::logistic lgemb(1, 1);
  vector<hyperpoint> placement;  

  string distance_file;

  void compute_dists() {
    int N = isize(sagcells);

    neighbors.clear();
    neighbors.resize(N);
    
    for(int i=0; i<N; i++)
      for(cell *c1: adj_minefield_cells(sagcells[i]))
        if(ids.count(c1)) neighbors[i].push_back(ids[c1]);

    const ld ERRORV = -17.3;
    transmatrix unknown = Id; unknown[0][0] = ERRORV;
    cell_matrix.clear();
    cell_matrix.resize(N, unknown);
    vector<int> visited;

    auto visit = [&] (int id, const transmatrix& T) {
      if(cell_matrix[id][0][0] != ERRORV) return;
      cell_matrix[id] = T;
      visited.push_back(id);
      };

    visit(0, Id);
    for(int i=0; i<isize(visited); i++) {
      cell *c0 = sagcells[visited[i]];
      const transmatrix& T0 = cell_matrix[visited[i]];
      for(int d=0; d<c0->type; d++)
        if(ids.count(c0->move(d)))
          visit(ids[c0->move(d)], T0 * currentmap->adj(c0, d));
      }
    
    sagdist.clear();
    sagdist.resize(N);
    for(int i=0; i<N; i++) sagdist[i].resize(N, N);

    if(distance_file != "") {
      fhstream f(distance_file, "rt");
      f.read(sagdist);
      }
    else if(gdist_prec && dijkstra_maxedge) {
      vector<vector<pair<int, ld>>> dijkstra_edges(N);
      for(int i=0; i<N; i++) {
        celllister cl(sagcells[i], dijkstra_maxedge, 50000, nullptr);
        for(auto c1: cl.lst) if(ids.count(c1)) if(c1 != sagcells[i])
          dijkstra_edges[i].emplace_back(ids[c1], pdist(tC0(cell_matrix[i]), tC0(cell_matrix[ids[c1]])));
        if(i == 0) println(hlog, i, " has ", isize(dijkstra_edges[i]), " edges");
        }
      parallelize(N, [&] (int a, int b) {
      vector<ld> distances(N);
      for(int i=a; i<b; i++) {
        if(i % 500 == 0) println(hlog, "computing dijkstra for ", i , "/", N);
        for(int j=0; j<N; j++) distances[j] = HUGE_VAL;
        std::priority_queue<pair<ld, int>> pq;
        auto visit = [&] (int i, ld dist) {
          if(distances[i] <= dist) return;
          distances[i] = dist;
          pq.emplace(-dist, i);
          };
        visit(i, 0);
        while(!pq.empty()) {
          ld d = -pq.top().first;
          int at = pq.top().second;
          pq.pop();
          for(auto e: dijkstra_edges[at]) visit(e.first, d + e.second);
          }
        for(int j=0; j<N; j++) sagdist[i][j] = distances[j] * gdist_prec + .5;
        }
      return 0;
      }
      );
      }

    else if(gdist_prec) {
      for(int i=0; i<N; i++)
      for(int j=0; j<N; j++)
        sagdist[i][j] = (pdist(tC0(cell_matrix[i]), tC0(cell_matrix[j])) + .5) * gdist_prec;
      }
    
    else {
      for(int i=0; i<N; i++) {
        auto &sdi = sagdist[i];
        vector<int> q;
        auto visit = [&] (int j, int dist) { if(sdi[j] < N) return; sdi[j] = dist; q.push_back(j); };
        visit(i, 0);
        for(int j=0; j<isize(q); j++) for(int k: neighbors[q[j]]) visit(k, sdi[q[j]]+1);
        }
      }
    
    max_sag_dist = 0;
    for(auto& d: sagdist) for(auto& x: d) max_sag_dist = max<int>(max_sag_dist, x);
    max_sag_dist++;
    }

  bool legacy;

  /* legacy method */
  void init_snake(int n) {
    sagcells.clear();
    ids.clear();

    auto enlist = [&] (cellwalker cw) {
      ids[cw.at] = isize(sagcells);
      sagcells.push_back(cw.at);
      };

    cellwalker cw = cwt;
    enlist(cw);
    cw += wstep;
    enlist(cw);
    for(int i=2; i<n; i++) {
      cw += wstep;
      while(ids.count(cw.at)) {
        cw = cw + wstep + 1 + wstep;
        }
      enlist(cw); cw += 1;
      }
    }

  void init_sag_cells() {
    sagcells = currentmap->allcells();
    int N = isize(sagcells);

    ids.clear();
    for(int i=0; i<N; i++) ids[sagcells[i]] = i;
    }

  /* separate hubs -- only for smClosest */
  ld hub_penalty;
  string hub_filename;
  vector<int> hubval;
  
  double costat(int vid, int sid) {
    if(vid < 0) return 0;
    double cost = 0;
    
    if(method == smLogistic) {
      auto &s = sagdist[sid];
      for(auto j: edges_yes[vid])
        cost += loglik_tab_y[s[sagid[j]]];
      for(auto j: edges_no[vid])
        cost += loglik_tab_n[s[sagid[j]]];
      return -cost;
      }
    
    if(method == smMatch) {
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
  
  // std::mt19937 los;

  double cost;

  vector<double> chgs;  
  
  edgetype *sag_edge;

  void forgetedges(int id) {
    for(int i=0; i<isize(vdata[id].edges); i++) 
      vdata[id].edges[i].second->orig = NULL;
    }
  
  bool chance(double p) {
    p *= double(hrngen.max()) + 1;
    auto l = hrngen();
    auto pv = (decltype(l)) p;
    if(l < pv) return true;
    if(l == pv) return chance(p-pv);
    return false;
    }

  void saiter() {
    int DN = isize(sagid);
    int t1 = hrand(DN);
    int sid1 = sagid[t1];
    
    int sid2;
    
    int s = hrand(4)+1;
    
    if(s == 4) sid2 = hrand(isize(sagcells));
    else {
      sid2 = sid1;
      for(int ii=0; ii<s; ii++) sid2 = hrand_elt(neighbors[sid2]);
      }
    int t2 = sagnode[sid2];
    
    sagnode[sid1] = -1; sagid[t1] = -1;
    sagnode[sid2] = -1; if(t2 >= 0) sagid[t2] = -1;
    
    double change = 
      costat(t1,sid2) + costat(t2,sid1) - costat(t1,sid1) - costat(t2,sid2);
    
    sagnode[sid1] = t1; sagid[t1] = sid1;
    sagnode[sid2] = t2; if(t2 >= 0) sagid[t2] = sid2;
    
    if(change > 0 && (sagmode == sagHC || !chance(exp(-change * exp(-temperature))))) return;

    sagnode[sid1] = t2; sagnode[sid2] = t1;
    sagid[t1] = sid2; if(t2 >= 0) sagid[t2] = sid1;
    cost += change;
    }
  
  void prepare_graph() {
    int DN = isize(sagid);

    set<pair<int, int>> alledges;
    for(auto e: sagedges) {
      if(e.i == e.j) continue;
      alledges.emplace(e.i, e.j);
      alledges.emplace(e.j, e.i);
      }
    
    edges_yes.clear(); edges_yes.resize(DN);
    edges_no.clear(); edges_no.resize(DN);
    
    for(int i=0; i<DN; i++) for(int j=0; j<DN; j++) if(i != j) {
      if(alledges.count({i, j}))
        edges_yes[i].push_back(j);
      else
        edges_no[i].push_back(j);
      }          

    sagnode.clear();
    sagnode.resize(isize(sagcells), -1);
    for(int i=0; i<DN; i++)
      sagnode[sagid[i]] = i;
    cost = 0;
    for(int i=0; i<DN; i++)
      cost += costat(i, sagid[i]);
    cost /= 2;
    }

  void set_inverse() {
    if(method == smMatch) vizflags |= RV_INVERSE_WEIGHT;
    else vizflags &=~ RV_INVERSE_WEIGHT;
    }

  void reassign() {
    int DN = isize(sagid);
    for(int i=0; i<DN; i++) {
      vdata[i].m->base = sagcells[sag::sagid[i]];
      forgetedges(i);
      }
    shmup::fixStorage();
    set_inverse();
    }

  void load_sag_solution(const string& fname) {
    printf("Loading the sag from: %s\n", fname.c_str());
    FILE *sf = fopen(fname.c_str(), "rt");
    if(!sf) { printf("Failed to open file.\n"); exit(1); }
    int SN = isize(sagcells);
    if(sf) while(true) {
      string lab;
      while(true) {
        int c = fgetc(sf);
        if(c == EOF) goto afterload;
        else if(c == ',' || c == ';') break;
        else if(rv_ignore(c)) ;
        else lab += c;
        }
      int sid = -1;
      int err = fscanf(sf, "%d", &sid);
      if(sid < 0 || sid >= SN || err < 1) sid = -1;
      if(!labeler.count(lab)) {
        printf("unknown vertex: %s\n", lab.c_str());
        }
      else {
        int id = getid(lab);
        sagid[id] = sid;
        }
      }
    afterload: 
    if(sf) fclose(sf);

    prepare_graph();
    reassign();
    }
  
  void dofullsa(int satime) {
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
      
      if(t2 - tl > 980) {
        tl = t2;
        println(hlog, format("it %12Ld temp %6.4f [1/e at %13.6f] cost = %f ",
          numiter, double(sag::temperature), (double) exp(sag::temperature),
          double(sag::cost)));
        }
      
      }
    
    temperature = -5;
    sagmode = sagOff;
    reassign();
    }

  void dofullsa_iterations(long long saiter) {
    sagmode = sagSA;

    decltype(SDL_GetTicks()) t1 = -999999;

    for(int i=0; i<saiter; i++) {

      temperature = hightemp - ((i+.5)/saiter*(hightemp-lowtemp));
      numiter++;
      sag::saiter();

      if(numiter % 10000 == 0) {
        auto t2 = SDL_GetTicks();
        if(t2 - t1 > 1000) {
          t1 = t2;
          println(hlog, format("it %12Ld temp %6.4f [1/e at %13.6f] cost = %f ",
            numiter, double(sag::temperature), (double) exp(sag::temperature),
            double(sag::cost)));
          }
        }
      }

    temperature = -5;
    sagmode = sagOff;
    reassign();
    }

  int sag_ittime = 100;

  void iterate() {
    if(!sagmode) return;
    int t1 = SDL_GetTicks();
    #if CAP_SDL && !CAP_SDL2
    int last = -1;
    #endif
    for(int i=0; i<ipturn; i++) {
      numiter++;
      sag::saiter();
      #if CAP_SDL && !CAP_SDL2
      int q = i * sag_ittime / ipturn;
      if(q > last) { last = 1; SDL_PumpEvents(); }
      #endif
      }
    int t2 = SDL_GetTicks();
    int t = t2 - t1;
    if(t < (sag_ittime+1) / 2) ipturn *= 2;
    else if(t > sag_ittime * 2) ipturn /= 2;
    else ipturn = ipturn * sag_ittime / t;
    print(hlog, format("it %12Ld temp %6.4f [2:%8.6f,10:%8.6f,50:%8.6f] cost = %f\n",
      numiter, double(sag::temperature), 
      (double) exp(-2 * exp(-sag::temperature)),
      (double) exp(-10 * exp(-sag::temperature)),
      (double) exp(-50 * exp(-sag::temperature)),
      (double) sag::cost));

    if(auto_visualize) reassign();
    }
  
  void save_sag_solution(const string& fname) {
    FILE *f = fopen(fname.c_str(), "wt");
    for(int i=0; i<isize(sagid); i++)
      fprintf(f, "%s;%d\n", vdata[i].name.c_str(), sagid[i]);
    fclose(f);
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

    for(auto& tab: sagdist) for(auto i: tab) {
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
      if(int(sagdist[sagid[ei.i]][sagid[ei.j]] * mul) == 136) printf("E %d,%d\n", ei.i, ei.j);
      if(ei.i != ei.j)
      if(ei.weight >= sag_edge->visible_from)
        pedge[sagdist[sagid[ei.i]][sagid[ei.j]] * mul]++;
      }
    
    for(int d=0; d<max_sag_dist; d++) 
      if(indist[d])
        printf("%2d: %7d/%7d %7.3lf\n", 
          d, pedge[d], indist[d], double(pedge[d] * 100. / indist[d]));
        
    ld loglik = 0;
    for(int d=0; d<max_sag_dist; d++) {
      int p = pedge[d], pq = indist[d];
      int q = pq - p;
      if(p && q) {
        loglik += p * log(p) + q * log(q) - pq * log(pq);
        println(hlog, tie(d, p, q), loglik);
        }
      }
    
    println(hlog, "loglikelihood best = ", fts(loglik));
    
    auto logisticf = [&] (dhrg::logistic&  l) {
      ld loglik = 0;
      for(int d=0; d<max_sag_dist; d++) {
        int p = pedge[d], pq = indist[d];
        if(p) loglik += p * l.lyes(d);
        if(pq > p) loglik += (pq-p) * l.lno(d);
        }
      return loglik;
      };
    
    dhrg::fast_loglik_cont(lgsag, logisticf, nullptr, 1, 1e-5);
    println(hlog, "loglikelihood logistic = ", logisticf(lgsag), " R= ", lgsag.R, " T= ", lgsag.T);    
    
    if(method == smLogistic) {
      compute_loglik_tab();
      prepare_graph();       
      println(hlog, "cost = ", cost);
      }
    }

  void optimize_sag_loglik_match() {
    lsq::leastsquare_solver<2> lsqs;

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

  void disttable_add(ld dist, int qty0, int qty1) {
    using namespace dhrg;
    size_t i = dist * llcont_approx_prec;
    constexpr array<ll, 2> zero = {0, 0};
    while(disttable_approx.size() <= i) disttable_approx.push_back(zero);
    disttable_approx[i][0] += qty0;
    disttable_approx[i][1] += qty1;
    }
  
  ld approx_01(hyperpoint h) {
    ld d = 0;
    if(h[0] > 1) {
      ld z = log(h[0]);
      d += z; h[1] *= h[0]; h[0] = 1; h[2] += z;
      }
    d += h[0];
    if(h[1] > 1) {
      ld z = log(h[1]);
      d += z; h[1] = 1; h[2] -= z;
      }
    d += h[1];
    d += abs(h[2]);
    return d;
    }

  ld pdist(hyperpoint hi, hyperpoint hj) {
    if(sol) return min(geo_dist(hi, hj), geo_dist(hj, hi));
    if(mproduct && angular) {

      auto di = product_decompose(hi);
      auto dj = product_decompose(hj);
      ld x = hdist(di.second, dj.second);
      ld z = di.first - dj.first;
      return log((x*x+z*z) * (x > 0 ? sinh(x) / x : 0));
      }
    return geo_dist(hi, hj);
    };

  ld pdist(int i, int j) {
    return pdist(placement[i], placement[j]);
    };

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
      cell *c0 = sagcells[i];
      transmatrix T0 = maps[i];
      for(int d=0; d<c0->type; d++)
        if(ids.count(c0->move(d)))
          visit(ids[c0->move(d)], T0 * currentmap->adj(c0, d));
      }
    
    int DN = isize(sagid);
    placement.resize(DN);
    for(int i=0; i<DN; i++) placement[i] = tC0(maps[sagid[i]]);    
    }
  
  int embiter;
  
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

  void reassign_embedding() {
    int DN = isize(sagid);
    for(int i=0; i<DN; i++) {
      vdata[i].m->base = sagcells[0];
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
  
  void read_hubs(const string& fname) {
    hubval.resize(isize(vdata), -1);
    fhstream f(fname, "rt");
    if(!f.f) { printf("Failed to open hub file: %s\n", fname.c_str()); exit(1); }
    println(hlog, "loading hubs: ", fname);
    while(!feof(f.f)) {
      string l1, l2;
      while(true) {
        int c = fgetc(f.f);
        if(c == EOF) return;
        else if(c == ';') break;
        else if(rv_ignore(c)) ;
        else l1 += c;
        }
      while(true) {
        int c = fgetc(f.f);
        if(c == EOF) return;
        else if(c == ';') return;
        else if(rv_ignore(c)) break;
        else l2 += c;
        }
      if(!id_known(l1)) {
        printf("label unknown: %s\n", l1.c_str());
        exit(1);
        }
      hubval[getid(l1)] = atoi(l2.c_str());
      }
    }
  
  void readsag(const char *fname) {
    maxweight = 0;
    sag_edge = add_edgetype("SAG edge");
    rogueviz::cleanup.push_back([] { sag_edge = nullptr; });
    fhstream f(fname, "rt");
    if(!f.f) {
      printf("Failed to open SAG file: %s\n", fname);
      throw "failed to open SAG file";
      }
    if(informat == 1) {
      scanline(f);
      set<pair<int, int> > edges;
      
      int all = 0, good = 0;
      while(!feof(f.f)) {        
        string l1 = scan<string>(f);
        string l2 = scan<string>(f);
        if(l1 == "") continue;
        if(l2 == "") continue;
        edgeinfo ei(sag_edge);
        ei.i = getid(l1);
        ei.j = getid(l2);
        if(ei.i > ei.j) swap(ei.i, ei.j);
        all++;
        if(edges.count({ei.i, ei.j})) continue;
        good++;
        edges.emplace(ei.i, ei.j);
        ei.weight = 1;
        sagedges.push_back(ei);
        }
      println(hlog, "N = ", isize(vdata), " edges = ", good, "/", all);
      return;
      }
    while(!feof(f.f)) {
      string l1, l2;
      while(true) {
        int c = fgetc(f.f);
        if(c == EOF) return;
        else if(c == ';') break;
        else if(rv_ignore(c)) ;
        else l1 += c;
        }
      while(true) {
        int c = fgetc(f.f);
        if(c == EOF) return;
        else if(c == ';') break;
        else if(rv_ignore(c)) ;
        else l2 += c;
        }
      ld wei;
      if(!scan(f, wei)) continue;
      edgeinfo ei(sag_edge);
      ei.i = getid(l1);
      ei.j = getid(l2);
      ei.weight = wei;
      sagedges.push_back(ei);
      }
    }
  
  ld edgepower=1, edgemul=1;

  void init() {
    rogueviz::init(RV_GRAPH | RV_WHICHWEIGHT | RV_AUTO_MAXWEIGHT | RV_HAVE_WEIGHT);

    rv_hook(rogueviz::hooks_close, 100, [] { sag::sagedges.clear(); });
    rv_hook(shmup::hooks_turn, 100, turn);
    rv_hook(rogueviz::hooks_rvmenu, 100, [] { 
      dialog::addSelItem(XLAT("temperature"), fts(sag::temperature), 't');
      dialog::add_action([] {
        dialog::editNumber(sag::temperature, sag::lowtemp, sag::hightemp, 1, 0, XLAT("temperature"), "");
        });
      dialog::addSelItem(XLAT("SAG mode"), sag::sagmodes[sag::sagmode], 'm');      
      dialog::add_action([] { sag::sagmode = sag::eSagmode( (1+sag::sagmode) % 3 ); });

      dialog::addSelItem(XLAT("min temperature"), fts(sag::lowtemp), 'i');
      dialog::add_action([] {
        dialog::editNumber(sag::lowtemp, -20, 20, 1, 0, XLAT("min temperature"), "");
        });

      dialog::addSelItem(XLAT("max temperature"), fts(sag::hightemp), 'i');
      dialog::add_action([] {
        dialog::editNumber(sag::hightemp, -20, 20, 1, 0, XLAT("high temperature"), "");
        });

      dialog::addSelItem(XLAT("automatic cycle"), fts(sag::vizsa_len), 'c');
      dialog::add_action([] {
        dialog::editNumber(sag::vizsa_len, 5, 1800, 1, 0, XLAT("automatic cycle"), "");
        });
      
      dialog::addBoolItem(XLAT("automatic"), sag::vizsa_start, 'a');
      dialog::add_action([] { 
        sag::vizsa_start = sag::vizsa_start ? 0 : SDL_GetTicks();
        sag::sagmode = sagOff;
        });

      dialog::addSelItem(XLAT("smoothness"), its(sag_ittime), 's');
      dialog::add_action([] {
        dialog::editNumber(sag_ittime, 0, 1000, 10, 100, XLAT("smoothness"),
          XLAT("How much milliseconds to compute before re-rendering the screen when optimizing in the background. Low values look nicer, but may cause less time to be spent on iterations.")
          );
        });

      dialog::addBoolItem_action(XLAT("auto-visualize"), sag::auto_visualize, 'b');

      dialog::addBoolItem_action(XLAT("continuous embedding"), sag::embedding, 'e'); 

      if(method == smMatch) {
        dialog::addSelItem(XLAT("match parameter A"), fts(match_a), 'A');
        dialog::add_action([] {
          dialog::editNumber(match_a, 0, 10, 1, 1, XLAT("match parameter A"), "");
          dialog::reaction = prepare_graph;
          });
        dialog::addSelItem(XLAT("match parameter B"), fts(match_b), 'B');
        dialog::add_action([] {
          dialog::editNumber(match_b, 0, 10, 1, 1, XLAT("match parameter B"), "");
          dialog::reaction = prepare_graph;
          });
        }

      dialog::addSelItem(XLAT("cost value"), fts(cost), 'X');
      dialog::add_action([] {
        optimize_sag_loglik_auto();
        });
      });

    weight_label = "min weight";
    temperature = 0; sagmode = sagOff;
    }

  void create_viz() {
    int DN = isize(vdata);
    
    for(int i=0; i<DN; i++) vdata[i].data = 0;
    for(int i=0; i<isize(sagedges); i++) {
      edgeinfo& ei = sagedges[i];

      ei.weight2 = pow((double) ei.weight, (double) edgepower) * edgemul;
      
      addedge0(ei.i, ei.j, &ei);
      }

    for(int i=0; i<DN; i++) {
      int ii = i;
      vertexdata& vd = vdata[ii];
      vd.cp = colorpair(dftcolor);
      createViz(ii, sagcells[sagid[i]], Id);
      }

    storeall();
    }

  void read(string fn) {
    fname = fn;
    init();
    readsag(fname.c_str());
    if(hub_filename != "")
      read_hubs(hub_filename);
  
    int DN = isize(vdata);
    if(legacy)
      init_snake(2 * DN);
    else
      init_sag_cells();

    compute_dists();

    int SN = isize(sagcells);
    if(SN < DN) {
      println(hlog, "SN = ", SN, " DN = ", DN);
      throw hr_exception("not enough cells for SAG");
      exit(1);
      }
    sagid.resize(DN);
    for(int i=0; i<DN; i++) sagid[i] = i;
    prepare_graph();
    create_viz();
    }

  void generate_fake_data(int n, int m) {
    init();
    init_sag_cells();
    compute_dists();

    sagid.resize(n);
    for(int i=0; i<n; i++) sagid[i] = i;
    hrandom_shuffle(sagid);
    if(m > n || m < 0) throw hr_exception("generate_fake_data parameters incorrect");
    sagid.resize(m);
    int SN = isize(sagcells);
    int DN = isize(sagid);
    vdata.resize(DN);
    for(int i=0; i<DN; i++)
      vdata[i].name = its(i) + "@" + its(sagid[i]);

    sag_edge = add_edgetype("SAG edge");
    for(int i=0; i<DN; i++)
    for(int j=i+1; j<DN; j++) {
      edgeinfo ei(sag_edge);
      ei.i = i;
      ei.j = j;
      ei.weight = 1. / sagdist[sagid[i]][sagid[j]];
      sagedges.push_back(ei);
      }

    if(SN < DN) {
      println(hlog, "SN = ", SN, " DN = ", DN);
      throw hr_exception("not enough cells for SAG");
      exit(1);
      }

    prepare_graph();
    create_viz();

    for(int i=0; i<DN; i++) {
      color_t col = patterns::compute_cell_color(sagcells[sagid[i]]);
      col <<= 8;
      col |= 0xFF;
      vdata[i].cp.color1 = vdata[i].cp.color2 = col;
      }
    }

ld compute_mAP() {
  ld mAP = 0;
  int DN = isize(sagid);
  for(int i=0; i<DN; i++) {
    vector<int> alldist;
    for(int j=0; j<DN; j++) if(i != j) alldist.push_back(sagdist[sagid[i]][sagid[j]]);
    sort(alldist.begin(), alldist.end());
    int q = isize(edges_yes[i]);
    int qmin = q-1, qmax = q+1;
    int threshold = alldist[q-1];
    while(qmin && alldist[qmin-1] == threshold) qmin--;
    while(qmax < isize(alldist)-2 && alldist[qmax+1] == threshold) qmax++;
    ld on_threshold = (q - qmin) / (qmax + 1. - qmin);
    int good = 0, onthr = 0;
    for(auto j: edges_yes[i]) {
      int d = sagdist[sagid[i]][sagid[j]];
      if(d < threshold) good++;
      if(d == threshold) onthr++;
      }
    mAP += (good + onthr * on_threshold) / q / DN;
    }
  return mAP;
  }

int logid;

void geo_stats() {
  start_game();
  println(hlog, "init_sag_cells started");
  init_sag_cells();
  println(hlog, "compute_dists started");
  compute_dists();
  println(hlog, "real");

  vector<short> sorted_sagdist;
  for(auto& a: sagdist) for(auto b: a) sorted_sagdist.push_back(b);
  sort(sorted_sagdist.begin(), sorted_sagdist.end());
  vector<int> d(5, 0);
  for(auto a: sagdist[0]) if(a < 5) d[a]++;

  for(int i=0; i<3; i++) {
    bool first = false;
    #define out(x, y) if(i == 0) println(hlog, x, " = ", y); else if(first) print(hlog, ";"); first = true; if(i == 1) print(hlog, x); if(i == 2) print(hlog, y);
    out("nodes", isize(sagcells));
    out("maxsagdist", max_sag_dist);
    out("dim", (euclid && WDIM == 2 && euc::eu.user_axes[1][1] == 1) ? 1 : WDIM);
    out("geometry", S3 >= OINF ? "tree" : hyperbolic ? "hyperbolic" : sphere ? "sphere" : euclid ? "euclid" : nil ? "nil" : sol ? "solv" : mproduct ? "product" : "other");
    out("closed", max_sag_dist == isize(sagcells) ? 0 : closed_manifold ? 1 : 0);
    out("angular", angular);
    for(int p: {1, 10, 50}) { out(format("sagdist%02d", p), sorted_sagdist[(p * sorted_sagdist.size()) / 100]); }
    for(int p: {1, 2, 3, 4}) { out(format("d%d", p), d[p]); }
    println(hlog);
    #undef out
    }
  }

void output_stats() {
  if(auto_save != "" && cost < best_cost) {
    println(hlog, "cost ", cost, " beats ", best_cost);
    best_cost = cost;
    sag::save_sag_solution(auto_save);
    }
  println(hlog, "solution: ", sagid);
  int DN = isize(sagid);
  ld mAP = compute_mAP();
  dhrg::iddata routing_result;
  dhrg::prepare_pairs(DN, [] (int i) { return edges_yes[i]; });
  dhrg::greedy_routing(routing_result, [] (int i, int j) { return sagdist[sagid[i]][sagid[j]]; });
  print(hlog, "CSV;", logid++, ";", isize(sagnode), ";", DN, ";", isize(sagedges), ";", lgsag.R, ";", lgsag.T, ";", cost, ";", mAP, ";", routing_result.suc / routing_result.tot, ";", routing_result.routedist / routing_result.bestdist);
  if(report_tempi) print(hlog, ";", hightemp,";",lowtemp,";",format("%lld", numiter));
  println(hlog);
  }

int readArgs() {
#if CAP_COMMANDLINE
  using namespace arg;

  if(0) ;

  else if(argis("-sagmin")) {
    auto& ed = sag_edge ? *sag_edge : default_edgetype;
    shift_arg_formula(ed.visible_from);
    ed.visible_from_hi = ed.visible_from;
    }
  else if(argis("-sagminhi")) {
    auto& ed = sag_edge ? *sag_edge : default_edgetype;
    shift_arg_formula(ed.visible_from_hi);
    }
  else if(argis("-sag_gdist")) {
    shift(); sag::gdist_prec = argi();
    }
  else if(argis("-sag_gdist_dijkstra")) {
    shift(); sag::dijkstra_maxedge = argi();
    }
  else if(argis("-sag_gdist_save")) {
    shift();
    fhstream f(args(), "wt");
    f.write(sagdist);
    }
  else if(argis("-sag_gdist_load")) {
    shift(); distance_file = args();
    }

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

  else if(argis("-sag_use_loglik")) {  
    shift(); int mtd = argi();
    if(mtd == 0) method = smClosest, loglik_repeat = false;
    if(mtd == 1) method = smLogistic, loglik_repeat = false;
    if(mtd == 2) method = smLogistic, loglik_repeat = true;
    if(mtd == 3) method = smMatch, loglik_repeat = false;
    if(mtd == 4) method = smMatch, loglik_repeat = true;
    if(method == smLogistic)
      compute_loglik_tab();
    if(method == smMatch) prepare_graph();
    }

  else if(argis("-sagformat")) {
    shift(); informat = argi();
    }

// (1) configure edge weights
  else if(argis("-sag-edgepower")) {
    shift_arg_formula(sag::edgepower);
    shift_arg_formula(sag::edgemul);
    }
// (1) configure temperature (high, low)
  else if(argis("-sagtemp")) {
    shift(); sag::hightemp = argi();
    shift(); sag::lowtemp = argi();
    }
// (2) read the edge data
  else if(argis("-sagpar")) {
    PHASE(3);
    shift();
    sag::sagpar = argi();
    }
  else if(argis("-sag")) {
    PHASE(3); 
    shift(); sag::read(args());
    }
  else if(argis("-sagfake")) {
    PHASE(3);
    shift(); int n = argi();
    shift(); int m = argi();
    sag::generate_fake_data(n, m);
    }
  else if(argis("-sagaviz")) {
    PHASE(3); 
    shift(); sag::auto_visualize = argi();
    }
  else if(argis("-saghubs")) {
    println(hlog, "HUBS");
    PHASE(3); 
    shift_arg_formula(sag::hub_penalty);
    shift(); hub_filename = args();
    }
// (3) load the initial positioning
  else if(argis("-sagload")) {
    PHASE(3); shift(); sag::load_sag_solution(args());
    }
// (4) perform simulated annealing: -fullsa <time in seconds>
  else if(argis("-sagfull")) {
    shift(); sag::dofullsa(argi());
    }
  else if(argis("-sagfulli")) {
    shift(); sag::dofullsa_iterations(argll());
    }
  else if(argis("-sagviz")) {
    sag::vizsa_start = SDL_GetTicks();
    shift(); sag::vizsa_len = argi();
    }
  else if(argis("-sagsmooth")) {
    shift(); sag::sag_ittime = argi();
    }
  else if(argis("-sagstats")) {
    output_stats();
    }
  else if(argis("-sag-angular")) {
    shift(); angular = argi();
    }
  else if(argis("-sagstats-logid")) {
    shift(); logid = argi();
    }
// (5) save the positioning
  else if(argis("-sagsave")) {
    PHASE(3); shift(); sag::save_sag_solution(args());
    }
  else if(argis("-sagsave-auto")) {
    PHASE(3); shift(); auto_save = args();
    }
// (6) output loglikelihood
  else if(argis("-sagloglik-l")) {
    sag::optimize_sag_loglik_logistic();
    }
  else if(argis("-sagloglik-m")) {
    sag::optimize_sag_loglik_match();
    }
  else if(argis("-sagloglik-a")) {
    sag::optimize_sag_loglik_auto();
    }
  else if(argis("-sagmode")) {
    shift();
    vizsa_start = 0;
    sagmode = (eSagmode) argi();
    if(sagmode == sagSA) {
      shift(); temperature = argf();
      }
    }
  else if(argis("-sagembed")) {
    sag::embedding = true;
    }
  else if(argis("-sag0")) {
    sag::report_tempi = true;
    numiter = 0;
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
  else if(argis("-sag-geo-stats")) geo_stats();
  else return 1;
#endif
  return 0;
  }

bool turn(int delta) {
  if(vizsa_start) {
    if(vizsa_start == -1) vizsa_start = ticks;
    auto t = ticks;
    double d = (t-vizsa_start) / (1000. * vizsa_len);
    if(d > 1 && loglik_repeat) {
      optimize_sag_loglik_auto();
      output_stats();
      vizsa_start = -1;
      }
    if(d > 1) sagmode = sagOff;
    else {
      temperature = hightemp - (d*(hightemp-lowtemp));
      sagmode = sagSA;
      }
    }
  if(sagmode == sagOff && embedding) {
    embedding_iterate();
    }
  iterate();
  return false;
  // shmup::pc[0]->rebase();
  }

string cname() {
  if(euclid) return "coord-6.txt";
  if(PURE) return "coord-7.txt";
  return "coord-67.txt";
  }

int ah = addHook(hooks_args, 100, readArgs)
  + addHook_rvslides(120, [] (string s, vector<tour::slide>& v) {
    if(s != "data") return;
    using namespace pres;
    string sagf = "SAG/";
    v.push_back(
      slide{sagf+"Roguelikes", 63, LEGAL::UNLIMITED | QUICKGEO,
        "A visualization of roguelikes, based on discussion on /r/reddit. "
        "See: http://www.roguetemple.com/z/hyper/reddit.php",
        roguevizslide('0', [] () {
          rogueviz::dftcolor = 0x282828FF;
    
          rogueviz::showlabels = true;
          part(rogueviz::default_edgetype.color, 0) = 181;
          rogueviz::sag::edgepower = 1;
          rogueviz::sag::edgemul = 1;
          
          gmatrix.clear();
          drawthemap();
          gmatrix0 = gmatrix;
    
          slide_backup(rogueviz::sag::legacy, true);
          rogueviz::sag::read(RVPATH "roguelikes/edges.csv");
          rogueviz::readcolor(RVPATH "roguelikes/color.csv");
          rogueviz::sag::load_sag_solution(RVPATH "roguelikes/" + cname());
          })    
        }
      );
    v.push_back(slide  {sagf+"Programming languages of GitHub", 64, LEGAL::UNLIMITED | QUICKGEO,
    "A visualization of programming languages.",
    roguevizslide('0', [] () {
      rogueviz::dftcolor = 0x282828FF;

      rogueviz::showlabels = true;
      part(rogueviz::default_edgetype.color, 0) = 128;
      rogueviz::sag::edgepower = .4;
      rogueviz::sag::edgemul = .02;
      
      gmatrix.clear();
      drawthemap();
      gmatrix0 = gmatrix;

      slide_backup(rogueviz::sag::legacy, true);
      rogueviz::sag::read(RVPATH "lang/edges.csv");
      rogueviz::readcolor(RVPATH "lang/color.csv");
      rogueviz::sag::load_sag_solution(RVPATH "lang/" + cname());
      if(euclid) rogueviz::legend.clear();
      })
    });
    
    v.push_back(slide {sagf+"Boardgames", 62, LEGAL::UNLIMITED | QUICKGEO,
        "A visualization of board games, based on discussions on Reddit.",
    roguevizslide('0', [] () {
      rogueviz::dftcolor = 0x282828FF;

      rogueviz::showlabels = true;
      part(rogueviz::default_edgetype.color, 0) = 157;
      rogueviz::sag::edgepower = 1;
      rogueviz::sag::edgemul = 1;
      
      gmatrix.clear();
      drawthemap();
      gmatrix0 = gmatrix;

      slide_backup(rogueviz::sag::legacy, true);
      rogueviz::sag::read(RVPATH "boardgames/edges.csv");
      rogueviz::readcolor(RVPATH "boardgames/color.csv");
      rogueviz::sag::load_sag_solution(RVPATH "boardgames/" + cname());
      })
        });

    });

EX }

}
