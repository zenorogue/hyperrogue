// RogueViz -- SAG embedder: cell constructor
// Copyright (C) 2011-24 Zeno Rogue, see 'hyper.cpp' for details

#include "../rogueviz.h"
#ifdef LINUX
#include <sys/mman.h>
#endif
#include <fcntl.h>

namespace rogueviz {

namespace sag {

namespace cells {

bool angular = false;

using subcell = pair<cell*, int>;

/** all the SAG cells */
vector<subcell> sagcells;

/** sagcells[ids[c]]] == c */
map<subcell, int> ids;

/** if i in neighbors[j], sagcells[i] is a neighbor of sagcells[j] */
vector<vector<int>> neighbors;

vector<hyperpoint> sagsubcell_point;
vector<transmatrix> sagsubcell_inv;

/** matrix for every sagcell, not subdivided */
vector<transmatrix> cell_matrix;

/** point for every sagcell */
vector<hyperpoint> cellpoint;

/** precision of geometric distances */
int gdist_prec;

/** max edge for dijkstra */
int dijkstra_maxedge;

/** dijkstra with tile distances */
bool dijkstra_tile;  

string distance_file;

ld pdist(hyperpoint hi, hyperpoint hj);  

/** the maximum value in sagdist +1 */
int max_sag_dist;

/** new style cell request */
int cell_request;

/** the structure type used to hold a N*N table of distances */
struct sagdist_t {
  using distance = unsigned short;
  distance* tab;
  void* tabmap;
  int fd;
  size_t N;
  int format;

  distance* begin() { return tab; }
  distance* end() { return tab+N*N; }

  sagdist_t() { tab = nullptr; fd = 0; format = 1; }

  distance* operator [] (int y) { return tab + N * y; }

  void init(int _N, distance val) {
    clear();
    N = _N;
    tab = new distance[N*N];
    for(size_t i=0; i<N*N; i++) tab[i] = val;
    }

  #ifdef LINUX
  void map(string fname) {
    clear();
    fd = open(fname.c_str(), O_RDONLY | O_LARGEFILE);
    if(fd == -1) throw hr_exception("open failed in map");
    if(read(fd, &N, 8) < 8) throw hr_exception("file error");
    tabmap = (distance*) mmap(nullptr, N*N*sizeof(distance)+8, PROT_READ, MAP_SHARED, fd, 0);


    if(tabmap == MAP_FAILED) {
      perror("mmap");
      throw hr_exception("Mapping Failed\n");
      }

    tab = (distance*) (((char*)tabmap) + 8);
    println(hlog, "test: ", test());
    }
  #endif

  void load_old(string fname) {
    vector<vector<distance>> old;
    clear();
    fhstream f(fname, "rb");
    f.read(old);
    init(isize(old), 0);
    auto ptr = tab;
    for(auto& row: old) for(auto val: row) *(ptr++) = val;
    }

  void load(string fname) {
    #ifdef LINUX
    if(format == 1) map(fname);
    else if(format == 2) load_old(fname);
    else throw hr_exception("sagdist format unknown");
    #else
    load_old(fname);
    #endif
    }

  vector<int> test() {
    vector<int> ttab = {int(N)};
    for(int a=0; a<4; a++) for(int b=0; b<4; b++) ttab.push_back((*this)[a][b]);
    for(size_t a=N-4; a<N; a++) for(size_t b=N-4; b<N; b++) ttab.push_back((*this)[a][b]);
    return ttab;
    }

  void save(string fname) {
    fd = open(fname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(write(fd, &N, 8) < 8) throw hr_exception("write error");
    size_t size =  N*N*sizeof(distance);
    #ifdef LINUX
    println(hlog, "size is ", hr::format("%zd", size));
    #else
    println(hlog, "size is ", hr::format("%lld", (long long) size));
    #endif
    char *p = (char*) tab;
    while(size) {
      size_t written = write(fd, p, size);
      if(written <= 0) throw hr_exception("bad written");
      p += written; size -= written;
      }
    println(hlog, "test: ", test());
    ::close(fd);
    }

  void clear() {
    #ifdef LINUX
    if(fd) { munmap(tabmap, N*N*sizeof(distance)+8); ::close(fd); }
    else 
    #endif
    delete[] tab;
    tab = nullptr; fd = 0;
    }

  ~sagdist_t() {
    clear();
    }

  };

sagdist_t sagdist;

vector<hyperpoint> subcell_points;

/** currently implemented only for Solv and Nil! */
void generate_subcellpoints() {
  start_game();
  subcell_points.clear();
  println(hlog, currentmap->get_cellshape(cwt.at).vertices_only);
  ld mx = 1, my = 1, mz = 1;
  if(sol) mx = my = mz = log(2);
  for(int x=0; x<4; x++)
  for(int y=0; y<4; y++) if((x&1) == (y&1))
  for(int z=0; z<4; z++) if((x&1) == (z&1)) {
    subcell_points.push_back(point31(mx * (x+.5-2)/4, my * (y+.5-2)/4, mz * (z+.5-2)/4));
    }
  println(hlog, subcell_points);
  }

void ensure_subcell_points() {
  if(isize(subcell_points) <= 1) subcell_points = { C0 };  
  }

void compute_dists() {
  int N = isize(sagcells);

  neighbors.clear();
  neighbors.resize(N);

  int Q = isize(subcell_points);

  for(int b=0; b<Q; b++)
  for(int i=0; i<N; i++)
    for(cell *c1: adj_minefield_cells(sagcells[i].first))
      if(ids.count({c1,b})) neighbors[i].push_back(ids[{c1,b}]);

  for(int i=0; i<N; i++) for(int b=0; b<Q; b++) if(b != sagcells[i].second)
    neighbors[i].push_back(ids[{sagcells[i].first, b}]);

  const ld ERRORV = -17.3;
  transmatrix unknown = Id; unknown[0][0] = ERRORV;
  cell_matrix.clear();
  cell_matrix.resize(N, unknown);
  cellpoint.clear();
  cellpoint.resize(N, C0);
  vector<int> visited;

  auto visit = [&] (int id, const transmatrix& T) {
    if(cell_matrix[id][0][0] != ERRORV) return;
    cell_matrix[id] = T;
    visited.push_back(id);
    };

  if(N == 0) return;

  visit(0, Id);
  for(int i=0; i<isize(visited); i++) {
    cell *c0 = sagcells[visited[i]].first;
    const transmatrix& T0 = cell_matrix[visited[i]];
    for(int d=0; d<c0->type; d++)
      if(ids.count({c0->move(d), 0}))
        visit(ids[{c0->move(d), 0}], T0 * currentmap->adj(c0, d));
    for(int q=0; q<Q; q++)
      cellpoint[visited[i]/Q*Q+q] = T0 * subcell_points[q];
    }
  
  if(distance_file != "") {
    sagdist.load(distance_file);
    }
  else if(gdist_prec && dijkstra_maxedge) {
    sagdist.init(N, N);
    println(hlog, "Computing Dijkstra distances...");
    vector<vector<pair<int, ld>>> dijkstra_edges(N);
    for(int i=0; i<N; i++) {
      celllister cl(sagcells[i].first, dijkstra_maxedge, 50000, nullptr);
      for(auto c1: cl.lst) for(int q=0; q<Q; q++) if(c1 != sagcells[i].first || q != sagcells[i].second) if(ids.count({c1, q}))
        dijkstra_edges[i].emplace_back(ids[{c1, q}], pdist(cellpoint[i], cellpoint[ids[{c1, q}]]));
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
    println(hlog, "N0 = ", neighbors[0]);
    println(hlog, "N1 = ", neighbors[1]);
    }

  else if(gdist_prec) {
    sagdist.init(N, N);
    println(hlog, "Computing distances... (N=", N, ")");
    ld mx = 1;
    for(int i=0; i<N; i++)
    for(int j=0; j<N; j++) {
      ld d = pdist(cellpoint[i], cellpoint[j]);
      sagdist[i][j] = (d + .5) * gdist_prec;
      if(d > mx) {
        println(hlog, kz(cellpoint[i]), kz(cellpoint[j]), " :: ", mx = d);
        }
      }
    }
  
  else {
    println(hlog, "no gdist_prec");
    sagdist.init(N, N);
    for(int i=0; i<N; i++) {
      auto sdi = sagdist[i];
      vector<int> q;
      auto visit = [&] (int j, int dist) { if(sdi[j] < N) return; sdi[j] = dist; q.push_back(j); };
      visit(i, 0);
      for(int j=0; j<isize(q); j++) for(int k: neighbors[q[j]]) visit(k, sdi[q[j]]+1);
      }
    }
  
  max_sag_dist = 0;
  for(auto x: sagdist) max_sag_dist = max<int>(max_sag_dist, x);
  max_sag_dist++;
  println(hlog, "max_sag_dist = ", max_sag_dist);
  }

bool legacy;

/* legacy method */
void init_snake(int n) {
  sagcells.clear();
  ids.clear();

  auto enlist = [&] (cellwalker cw) {
    ids[{cw.at, 0}] = isize(sagcells);
    sagcells.emplace_back(cw.at, 0);
    };

  cellwalker cw = cwt;
  enlist(cw);
  cw += wstep;
  enlist(cw);
  for(int i=2; i<n; i++) {
    cw += wstep;
    while(ids.count({cw.at, 0})) {
      cw = cw + wstep + 1 + wstep;
      }
    enlist(cw); cw += 1;
    }
  }

void init_cells_to_all() {

  ensure_subcell_points();

  sagcells.clear();
  for(auto c: currentmap->allcells()) for(int i=0; i<isize(subcell_points); i++) {
    ids[{c, i}] = isize(sagcells);
    sagcells.emplace_back(c, i);
    }
  }

void compute_creq_neighbors() {
  int SN = isize(sagcells);
  neighbors.resize(SN);
  vector<int> mindist_for(SN, 30000);
  for(int i=0; i<SN; i++) {
    auto& m = mindist_for[i];
    for(int j=0; j<SN; j++) if(j != i) m = min<int>(m, sagdist[i][j]);
    }

  for(int i=0; i<SN; i++)
  for(int j=0; j<SN; j++) if(i != j && sagdist[i][j] < mindist_for[i] + mindist_for[j]) neighbors[i].push_back(j);

  max_sag_dist = 0;
  for(auto x: sagdist) max_sag_dist = max<int>(max_sag_dist, x);
  max_sag_dist++;
  println(hlog, neighbors[0]);
  hlog.flush();
  }

vector<vector<pair<ld, subcell>>> dijkstra_edges;

void find_cells() {
  println(hlog, "cellcount = ", cellcount);
  ensure_subcell_points();
  struct qitem {
    ld dist; subcell sc; transmatrix T; 
    bool operator < (const qitem& b) const { return dist > b.dist + 1e-6; }
    };     

  std::priority_queue<qitem> pq;
  auto visit = [&] (subcell sc, ld dist, const transmatrix& T) {
    if(ids.count(sc)) return;
    pq.emplace(qitem{dist, sc, T});
    };

  sagsubcell_point.clear();
  sagsubcell_inv.clear();

  int Q = isize(subcell_points);
  visit(subcell{cwt.at,0}, 0, Id);
  ld maxdist0 = 0;
  for(int i=0;; i++) {
    if(pq.empty()) { println(hlog, "no more"); break; }
    auto p = pq.top();
    pq.pop();
    ld dist = p.dist;
    auto sc = p.sc;
    transmatrix T = p.T;
    if(ids.count(sc)) { i--; continue; }
    if(i == cell_request-1) maxdist0 = dist;
    if(i >= cell_request && dist > maxdist0 + 1e-6) break;

    sagcells.push_back(sc);
    sagsubcell_point.push_back(T * subcell_points[sc.second]);
    sagsubcell_inv.push_back(inverse(T));
    ids[sc] = i;
    println(hlog, "cell ", i, " is ", sc, " at ", sagsubcell_point.back(), " in distance ", dist);

    if(dijkstra_maxedge) {
      dijkstra_edges.emplace_back();
      auto& de = dijkstra_edges.back();

      set<cell*> vis;
      vector<tuple<cell*, transmatrix, int>> q;
      auto visit1 = [&] (cell *c, transmatrix T, int d) {
        if(vis.count(c)) return;
        vis.insert(c);
        q.emplace_back(c, T, d);
        };
      visit1(sc.first, Id, 0);
      for(int i1=0; i1 < isize(q); i1++)  {
        cell *c = get<0>(q[i1]);
        transmatrix T1 = get<1>(q[i1]);
        int dist1 = get<2>(q[i1]);
        if(dist1 < dijkstra_maxedge) for(int j=0; j<c->type; j++) {
          cell *c1 = c->cmove(j);
          visit1(c1, T1 * currentmap->adj(c, j), dist1+1);
          }

        for(int q=0; q<Q; q++) {
          subcell sc1 {c, q};
          ld ndist = dijkstra_tile ? dist1 : pdist(subcell_points[sc.second], T1 * subcell_points[q]);
          de.push_back({ndist, sc1});
          visit(sc1, dist + ndist, T*T1);
          }
        }
      }
    else {
      for(int j=0; j<sc.first->type; j++) for(int k=0; k<Q; k++) {
        cell *c1 = sc.first->cmove(j);
        transmatrix T1 = T * currentmap->adj(sc.first, j);
        visit(subcell{c1, k}, pdist(C0, T1*C0), T1);
        }
      }
    }
  int SN = isize(sagcells);
  println(hlog, "number of cells found: ", SN, " dijkstra_maxedge = ", dijkstra_maxedge);

  all_disk_cells_sorted = {};
  for(auto p: ids) if(all_disk_cells_sorted.empty() || p.first.first != all_disk_cells_sorted.back()) all_disk_cells_sorted.push_back(p.first.first);
  for(cell *c: all_disk_cells_sorted) c->mpdist = 0, c->land = laCanvas, c->landparam = 0x101010, c->wall = waNone;
  }

void init_cell_request() {
  println(hlog, "generating on cell request");
  find_cells();

  if(isize(subcell_points) == 1) {
    compute_dists();
    return;
    }

  int SN = isize(sagcells);
  sagdist.init(SN, 0);

  if(!dijkstra_maxedge) {
    println(hlog, "computing sagdist ...");
    parallelize(SN, [&] (int a, int b) {
      for(int i=a; i<b; i++) {
        for(int j=0; j<SN; j++) {
          ld dist = pdist(sagsubcell_point[i], sagsubcell_point[j]);
          sagdist[i][j] = int(dist * gdist_prec + 0.5);
          if(i < j && sagdist[i][j] == 0) println(hlog, "for ", tie(i,j), " pdist computed as ", dist);
          }
        }
      return 0;
      });
    println(hlog, "... done");
    }
  else {
    vector<vector<pair<ld, int>>> dijkstra_edges_2;
    dijkstra_edges_2.resize(SN);
    for(int i=0; i<SN; i++) for(auto p: dijkstra_edges[i]) if(ids.count(p.second)) dijkstra_edges_2[i].emplace_back(p.first, ids[p.second]);

    parallelize(SN, [&] (int a, int b) {
      vector<ld> distances(SN);
      for(int i=a; i<b; i++) {
        if(i % 500 == 0) println(hlog, "computing dijkstra for ", i , "/", SN);
        for(int j=0; j<SN; j++) distances[j] = HUGE_VAL;
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
          for(auto e: dijkstra_edges_2[at]) {
            // println(hlog, "move from ", at, " to ", e.first, " for ", d, "+", e.second);
            visit(e.second, d + e.first);
            }
          }
        for(int j=0; j<SN; j++) sagdist[i][j] = distances[j] * gdist_prec + .5;
        }
      return 0;
      });
    }

  compute_creq_neighbors();
  }

bool distance_only;

void init_cells() {
  if(state & SS_CELLS) return;
  sag::init();
  state |= SS_CELLS;

  if(cell_request) {
    if(distance_file != "") {
      println(hlog, "loading graph ", distance_file);
      sagdist.load(distance_file);
      if(distance_only) {
        sagcells.resize(sagdist.N, subcell{nullptr, 0});
        }
      else {
        find_cells();
        }
      compute_creq_neighbors();
      return;
      }

    init_cell_request();
    return;
    }

  else if(legacy) state |= SS_NEED_SNAKE;

  else init_cells_to_all();

  if(!cell_request) compute_dists();
  }

void init_snake_if_needed() {
  if(!(state & SS_NEED_SNAKE)) return;
  state &=~ SS_NEED_SNAKE;
  init_snake(2 * isize(vdata));
  compute_dists();
  }

ld pdist(hyperpoint hi, hyperpoint hj) {
  if(sol && angular) {
    return 10 * asinh(hypot_d(3, lie_log(shiftless(gpushxto0(hi) * hj))) / 10);
    }
  if(sol) return min(geo_dist(hi, hj), geo_dist(hj, hi));
  if(mproduct && angular) {

    auto di = product_decompose(hi);
    auto dj = product_decompose(hj);
    ld x = hdist(di.second, dj.second);
    ld z = di.first - dj.first;
    auto res = sqrt((x*x+z*z) * (x > 0 ? sinh(x) / x : 1));
    return res;
    }
  return geo_dist(hi, hj);
  };

void geo_stats() {
  init_cells();

  println(hlog, "counting sagdist, N=", int(sagdist.N), " max_sag_dist = ", max_sag_dist);
  vector<short> sgdc(max_sag_dist, 0);
  for(auto x: sagdist) sgdc[x]++;

  println(hlog, "building sorted_sagdist");
  vector<short> sorted_sagdist;
  for(int i=0; i<max_sag_dist; i++) for(int j=0; j<sgdc[i]; j++) sorted_sagdist.push_back(i);

  println(hlog, "computing min_max_nei");
  int minnei = 500, maxnei = 0;
  int SN = sagdist.N;
  for(int i=0; i<SN; i++) for(int j: neighbors[i]) {
    if(sagdist[i][j] < minnei) minnei = sagdist[i][j];
    if(sagdist[i][j] > maxnei) maxnei = sagdist[i][j];
    }   

  for(int i=0; i<3; i++) {
    bool first = false;
    #define out(x, y) if(i == 0) println(hlog, x, " = ", y); else if(first) print(hlog, ";"); first = true; if(i == 1) print(hlog, x); if(i == 2) print(hlog, y);
    out("nodes", SN);
    out("maxsagdist", max_sag_dist);
    out("dim", (euclid && WDIM == 2 && euc::eu.user_axes[1][1] == 1) ? 1 : WDIM);
    out("geometry", S3 >= OINF ? "tree" : hyperbolic ? "hyperbolic" : sphere ? "sphere" : euclid ? "euclid" : nil ? "nil" : sol ? "solv" : mproduct ? "product" : "other");
    out("closed", max_sag_dist == isize(sagcells) ? 0 : closed_manifold ? 1 : 0);
    out("angular", angular);
    for(int p: {1, 10, 50}) { out(format("sagdist%02d", p), sorted_sagdist[(p * sorted_sagdist.size()) / 100]); }
    out("minnei", minnei);
    out("maxnei", maxnei);
    out("neighbors", isize(neighbors[0]));
    println(hlog);
    #undef out
    }
  }

bool visualize_subcells_on = false;

bool visualize_subcells(cell *c, const shiftmatrix& V) {
  if(!visualize_subcells_on) return false;
  for(int i=0; i<isize(subcell_points); i++) {
    auto p = at_or_null(ids, pair<cell*,int>{c, i});
    if(!p) continue;
    queuepolyat(V * rgpushxto0(subcell_points[i]), cgi.shSnowball, 0x80FF80FF, PPR::FLOORb);

    if(sagsubcell_inv.size()) for(auto nei: neighbors[*p]) if(nei<*p) {
      queueline(V * subcell_points[i], V * sagsubcell_inv[*p] * sagsubcell_point[nei], 0x8000FF, 3).prio = PPR::FLOORa;
      }
    }
  return false;
  }


int cell_read_args() {
#if CAP_COMMANDLINE
  using namespace arg;

  if(0) ;
  else if(argis("-sag_gdist")) {
    shift(); gdist_prec = argi();
    }
  else if(argis("-sag_gdist_dijkstra")) {
    shift(); dijkstra_maxedge = argi(); dijkstra_tile = false;
    }
  else if(argis("-sag-dtile")) {
    dijkstra_tile = true; dijkstra_maxedge = 1;
    }
  else if(argis("-sag_gdist_save")) {
    init_cells();
    shift();
    sagdist.save(args());
    }
  else if(argis("-sag_gdist_load")) {
    distance_only = false;
    shift(); distance_file = args();
    }
  else if(argis("-sag-gdist_load1")) {
    distance_only = true;
    shift(); distance_file = args();
    }
  else if(argis("-sag-angular")) {
    shift(); angular = argi();
    }
  else if(argis("-sag-geo-stats")) geo_stats();
  else if(argis("-sag-creq")) {
   shift(); cell_request = argi();
   }
  else if(argis("-sag-initcells")) {
    init_cells();
    }
  else if(argis("-gen-subcellpoints")) {
    generate_subcellpoints();
    }
  /* to viz only subcellpoints */
  else if(argis("-sag-clear")) {
    shmup::monstersAt.clear();
    }
  else return 1;  
#endif
  return 0;
  }

int ah = addHook(hooks_args, 100, cell_read_args);

}
}
}
