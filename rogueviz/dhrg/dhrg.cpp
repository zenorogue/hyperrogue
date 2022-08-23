// see this paper: https://arxiv.org/abs/2109.11772

#include "dhrg.h"

namespace rogueviz { extern string fname; }

#include "readgraph.cpp"
#include "dhrg-utils.cpp"
#include "regular.cpp"
#include "gridmapping.cpp"
#include "mycell.cpp"
#include "segment.cpp"
#include "dynamic.cpp"
#include "loglik.cpp"
#include "paths.cpp"
#include "embedder.cpp"
#include "tests.cpp"
#include "betweenness.cpp"
#include "groundtruth.cpp"
#include "routing.cpp"

namespace dhrg {

int M;
vector<struct mycell*> vertices;
vector<ld> disttable0, disttable1;

void memoryInfo() {
  string s = "";
  
  ll totalmemory = 0;

  for(int it=0; it<2; it++) {
    auto pct = [&] (auto x, auto y) { 
      if(it == 0) totalmemory += x*y;
      else s += " " + its(x) + "x" + its(y) + "B=" + its((x*y*100)/totalmemory) + "%";
      };
    pct(cellcount, sizeof(cell));
    pct(heptacount, sizeof(heptagon));
    pct(mycellcount, sizeof(mycell));
    pct(segmentcount, sizeof(segment));
    }
  println(hlog, "Memory info: ", s, " (", int(totalmemory/1048576), " MB)");
  fflush(stdout);
  }

void debugtally() {
  print(hlog, "T"); for(int i=0; i<MAXDIST; i++) if(tally[i]) print(hlog, format(" %4d/%4lld", edgetally[i], tally[i]));
  println(hlog, " .. %", loglikopt()); 
  }

void place_rogueviz_vertices() {
  progressbar pb(N, "place_rogueviz_vertices");
  // transmatrix In = inverse(ggmatrix(currentmap->gamestart()));
  using namespace rogueviz;
/*  for(int i=0; i<N; i++) vdata[i].m->base = currentmap->gamestart();
  for(int i=0; i<N; i++) vdata[i].m->at = In * shmup::ggmatrix(vertices[i]->ascell()); */
  for(int i=0; i<N; i++) vdata[i].m->base = vertices[i]->ascell();
  for(int i=0; i<N; i++) vdata[i].m->at = Id;
  fixedges(); 
  }

int destroy;

void clear() {
  coords.clear();  
  // destroytallies();
  //  todo: correct cleanup!
  for(int i=0; i<MAXDIST; i++) tally[i] = 0;
  for(int i=0; i<MAXDIST; i++) edgetally[i] = 0;
#ifdef BUILD_ON_HR
  mymap.clear();
#else
  delete mroot;
  mroot = NULL;
#endif
  }
  
// actual octiles of normal are roughly: -1.15 -.674 -.318 0 .318 .674 1.15

void dhrg_init() {
  if(!mroot) {
    println(hlog, "DHRG version " DHRGVER "\n");
    rogueviz::init(0);
    rogueviz::rv_hook(hooks_handleKey, 100, dhrg_animate);
    regular_info();
    generate_root();
    }
  }

int next_timestamp;
int ts_rogueviz;
int ts_rbase;
int ts_coords;
int ts_vertices;
bool stored;

void graphv(string s) {
  dhrg_init(); read_graph(s, true, true, true);
  next_timestamp++;
  ts_rogueviz = next_timestamp;
  ts_rbase = next_timestamp;
  stored = true;
  }

int dhrgArgs() {
  using namespace arg;
           
  if(argis("-dhrg")) {
    PHASE(3); shift(); dhrg_init(); read_graph_full(args());
    next_timestamp++;
    ts_rogueviz = next_timestamp;
    ts_vertices = next_timestamp;
    }
    
  else if(argis("-graph")) {
    PHASE(3); shift(); dhrg_init(); read_graph(args(), false, false, false);
    next_timestamp++;
    ts_rogueviz = next_timestamp;
    // stored = true;
    }

  else if(argis("-graphv")) {
    PHASE(3); shift(); graphv(args());
    }
  
  else if(argis("-analyze_grid")) {
    PHASE(3); shift(); dhrg_init(); do_analyze_grid(argi());
    }

  else if(argis("-analyze_dists")) {
    PHASE(3); dhrg_init(); shift(); do_analyze_dists(argi());
    }

  else if(argis("-test_paths")) {
    PHASE(3); dhrg_init(); shift(); test_paths(argi());
    }

  else if(argis("-contll")) {
    if(ts_rogueviz >= ts_rbase && ts_rogueviz >= ts_vertices && ts_rogueviz > ts_coords) {
      origcoords();
      build_disttable_approx();
      ts_coords = ts_rogueviz;
      }
    else if(ts_rbase >= ts_vertices && ts_rbase > ts_coords) {
      ts_coords = ts_rbase;
      rvcoords();
      build_disttable_approx();
      }
    else if(ts_vertices > ts_coords) {
      ts_coords = ts_vertices;
      cellcoords();
      build_disttable_approx();
      }
    logistic cont;
    cont.setRT(graph_R, graph_T);
    fast_loglik_cont(cont, loglik_cont_approx, "lcont", 1, 1e-6);
    // return loglik_cont();
    }

  else if(argis("-dhrgview")) {
    if(ts_vertices > ts_rbase) {
      place_rogueviz_vertices();
      ts_rbase = ts_vertices;
      }
    if(!stored) rogueviz::storeall(), stored = true;
    else shmup::fixStorage();
    mainloop(); quitmainloop = false;
    }

  else if(argis("-iterate")) {
  
    if(ts_rbase > ts_vertices) {
      PHASE(3); dhrg_init(); graph_from_rv(); 
      ts_vertices = ts_rbase;
      }
    if(!ts_vertices) {
      printf("Error: read vertices with -dhrg or -graph\n");
      exit(1);
      }
    shift();
    embedder_loop(argi());
    next_timestamp++;
    ts_vertices = next_timestamp;
    }

  else if(argis("-dorestart")) {
    dorestart = true;
    }

  else if(argis("-dontrestart")) {
    dorestart = false;
    }

  else if(argis("-lctype")) {
    shift(); lc_type = args()[0];
    }

  else if(argis("-loadtest")) {
    dhrg_init(); load_test();
    }

  else if(argis("-buildtest")) {
    shift(); dhrg_init(); build_all(argi());
    }

  else if(argis("-pbv")) {
    compute_betweenness(true);
    }

  else if(argis("-pb")) {
    compute_betweenness(false);
    }

  else if(argis("-gtt")) {
    shift(); ground_truth_test(args());
    }

  else if(argis("-routing")) {
    shift(); routing_test(args());
    }

  else if(argis("-esaveas")) {
    shift(); save_embedding(args());
    }

  else if(argis("-esave")) {
    save_embedding(rogueviz::fname + "-dhrg.txt");
    }

  else if(argis("-eload")) {
    PHASE(3); shift(); dhrg_init(); load_embedded(args());
    next_timestamp++;
    ts_rogueviz = next_timestamp;
    ts_vertices = next_timestamp;
    }

  else return 1;

  return 0;
  }

void store_gamedata(struct hr::gamedata* gd) {
  if(true) {
    for(auto& t: tally) gd->store(t);
    for(auto& t: edgetally) gd->store(t);
    gd->store(coords);
#ifdef BUILD_ON_HR
    gd->store(mymap);
#else
    gd->store(mroot);
#endif
    }
  }

auto hook = 
    addHook(hooks_args, 50, dhrgArgs)
  + addHook(hooks_gamedata, 230, store_gamedata)
  + addHook(hooks_clearmemory, 200, clear);

#if CAP_SDL
#include "visualize.cpp"
#endif

}
