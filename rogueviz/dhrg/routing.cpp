
//	$(VARIANT) -nogui -dhrg embedded-graphs/facebook_combined_result -contll -iterate 99 -contll -esave > $@

namespace dhrg {

struct pairdata { ld success, route_length; };

#define NOYET 127

vector<vector<char> > actual;

vector<pairdata> pairs;

vector<int> last_goal;
vector<int> next_stop;

int gr_N;

using neighborhoodfun = std::function<vector<int> (int)>;

void prepare_pairs(int N, const neighborhoodfun& nei) {
  gr_N = N;
  pairs.resize(N);
  actual.resize(N);
  for(int i=0; i<N; i++) actual[i].resize(N, NOYET);
  for(int i=0; i<N; i++) {
    vector<int> bfsqueue;
    auto& p = actual[i];
    auto visit = [&] (int j, int d) {
      if(p[j] == NOYET) {
        p[j] = d;
        bfsqueue.push_back(j);
        }
      };
    visit(i, 0);
    for(int k=0; k<isize(bfsqueue); k++) {
      int a = bfsqueue[k];
      for(auto b: nei(a))
        visit(b, p[a] + 1);
      }
    }
  last_goal.clear();
  last_goal.resize(N, -1);
  next_stop.clear();
  next_stop.resize(N, -1);
  }

void prepare_pairs() {
  prepare_pairs(N, [] (int a) {
    vector<int> res;
    for(auto ed: rogueviz::vdata[a].edges) {
      int b = ed.second->i ^ ed.second->j ^ a;
      res.push_back(b);
      }
    return res;
    });
  }

void route_from(int src, int goal, const vector<ld>& distances_from_goal) {
  if(last_goal[src] == goal) return;
  if(src == goal) {
    pairs[src].success = 1;
    pairs[src].route_length = 0;
    next_stop[src] = -1;
    }
  else {
    ld bestd = distances_from_goal[src] - 1e-5;

    /* iprintf("route_from goal=%d a=%d, bestd = %f\n", goal, src, bestd + 1e-5);
    indent += 2; */

    vector<int> candidates;
    for(auto ed: rogueviz::vdata[src].edges) {
      int e = ed.second->i ^ ed.second->j ^ src;
      ld d = e == goal ? -1 : distances_from_goal[e];
      if(d < bestd) bestd = d, candidates.clear();
      if(d == bestd) candidates.push_back(e);
      }
    pairs[src].success = pairs[src].route_length = 0;
    for(int c: candidates) {
      route_from(c, goal, distances_from_goal);
      // iprintf("candidate = %d\n", c);
      pairs[src].success += pairs[c].success / isize(candidates);
      pairs[src].route_length += (1 + pairs[c].route_length) / isize(candidates);
      }

    if(isize(candidates) > 0) next_stop[src] = candidates[0];
    else next_stop[src] = -1;
    // iprintf("success = %f, route = %f\n", pairs[src].success, pairs[src].route_length);
    // indent -= 2;
    }
  last_goal[src] = goal;
  }

void greedy_routing_to(iddata& d, int goal, const distfun& distance_function) {
  vector<ld> distances_from_goal(gr_N);
  for(int src=0; src<gr_N; src++)
    distances_from_goal[src] = distance_function(goal, src);
  for(int src=0; src<gr_N; src++)
    route_from(src, goal, distances_from_goal);

  for(int j=0; j<gr_N; j++) if(j != goal){
    d.tot++;
    ld p = pairs[j].success;
    d.suc += p;
    d.routedist += pairs[j].route_length;
    d.bestdist += p * actual[goal][j];
    }
  }

void greedy_routing(iddata& d, const distfun& distance_function) {
  for(int goal=0; goal<gr_N; goal++) greedy_routing_to(d, goal, distance_function);
  }

void routing_test(string s) {

  iddata datas[6];
  vector<string> reps;
  vector<ld> values;

  auto report = [&] (string s, ld val) {
    values.push_back(val);
    reps.push_back(s);
    println(hlog, "REPORT ", s, " = ", val);
    };

  if(1) {
    FILE *f = fopen(("embout/" + s).c_str(), "rb");
    char buf[999999];
    int siz = fread(buf, 1, 999999, f);
    
    for(int i=0; i<siz; i++) if(is(buf+i, "Embedded Log-likelihood: "))
      report("bfkl", atof(buf+i+25));

    fclose(f);
    }

  if(1) {
    dhrg_init(); read_graph_full("data/sime-" + s);
    origcoords();
    prepare_pairs();
    greedy_routing(datas[0], [] (int i, int j) { return hdist(vertexcoords[i], vertexcoords[j]); });
    greedy_routing(datas[1], [] (int i, int j) { return quickdist(vertices[i], vertices[j], 0); });

    embedder_loop(20);

    greedy_routing(datas[2], [] (int i, int j) { return quickdist(vertices[i], vertices[j], 0); });

    cellcoords();
    greedy_routing(datas[3], [] (int i, int j) { return hdist(vertexcoords[i], vertexcoords[j]); });

    greedy_routing(datas[4], [] (int i, int j) { return hdist(vertexcoords[i], vertexcoords[j]) + 100 * quickdist(vertices[i], vertices[j], 0); });
    }

  for(int id: {0,1,2,3,4}) {
    string caps[5] = {"ec", "ed", "od", "oc", "tie"};
    string cap = caps[id];
    auto& d = datas[id];
    report("suc_" + cap, d.suc / d.tot);
    report("str_" + cap, d.routedist / d.bestdist);
    }

  println(hlog, "HDR;", separated(";", reps), ";N;GROWTH\n");
  println(hlog, "RES;", separated(";", values), ";", N, ";", cgi.expansion->get_growth());
  }

int current_goal;
iddata prepared;
void prepare_goal(int goal) {
  greedy_routing_to(prepared, current_goal = goal, [] (int i, int j) { return hdist(vertexcoords[i], vertexcoords[j]); });
  }

vector<int> path(int src) {
  vector<int> res;
  while(src != -1) {
    res.push_back(src);
    src = next_stop[src];
    }
  return res;
  }

int get_actual(int src) {
  return actual[src][current_goal];
  }

}
