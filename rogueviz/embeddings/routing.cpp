
//	$(VARIANT) -nogui -dhrg embedded-graphs/facebook_combined_result -contll -iterate 99 -contll -esave > $@

namespace rogueviz {

namespace embeddings {

struct pairdata { ld success, route_length, modded_success, modded_route_length; };

#define NOYET 127

vector<vector<char> > actual;

vector<pairdata> pairs;

vector<int> last_goal;
vector<int> next_stop;

void prepare_pairs() {
  int N = isize(rogueviz::vdata);
  pairs.resize(N);
  actual.resize(N);
  for(int i=0; i<N; i++) actual[i].clear();
  for(int i=0; i<N; i++) actual[i].resize(N, NOYET);
  progressbar pb(N, "prepare pairs");
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
      for(auto ed: rogueviz::vdata[a].edges)
        visit(ed.first, p[a] + 1);
      }
    pb++;
    }
  last_goal.clear();
  last_goal.resize(N, -1);
  next_stop.clear();
  next_stop.resize(N, -1);
  }

void route_from(int src, int goal, const vector<ld>& distances_from_goal) {
  if(last_goal[src] == goal) return;
  if(src == goal) {
    pairs[src].success = 1;
    pairs[src].route_length = 0;
    pairs[src].modded_success = 1;
    pairs[src].modded_route_length = 0;
    next_stop[src] = -1;
    }
  else {
    pairs[src].modded_success = 0;
    pairs[src].modded_route_length = 0;
    pairs[src].success = 0;
    pairs[src].route_length = 0;
    last_goal[src] = goal;

    ld bestd = HUGE_VAL;

    /* iprintf("route_from goal=%d a=%d, bestd = %f\n", goal, src, bestd + 1e-5);
    indent += 2; */

    vector<int> candidates;
    for(auto& ed: rogueviz::vdata[src].edges) {
      int e = ed.first;
      ld d = e == goal ? -1 : distances_from_goal[e];
      if(d < bestd) bestd = d, candidates.clear();
      if(d == bestd) candidates.push_back(e);
      }

    if(1) {
      ld tmp_success = 0, tmp_route_length = 0;
      for(int c: candidates) {
        route_from(c, goal, distances_from_goal);
        tmp_success += pairs[c].modded_success;
        tmp_route_length += pairs[c].modded_route_length;
        }

      pairs[src].modded_success = tmp_success / isize(candidates);
      pairs[src].modded_route_length = tmp_route_length / isize(candidates) + pairs[src].modded_success;
      }

    if(bestd < distances_from_goal[src] - 1e-7) {
      ld tmp_success = 0, tmp_route_length = 0;
      for(int c: candidates) {
        tmp_success += pairs[c].success;
        tmp_route_length += pairs[c].route_length;
        }

      next_stop[src] = candidates[0];
      pairs[src].success = tmp_success / isize(candidates);
      pairs[src].route_length = tmp_route_length / isize(candidates) + pairs[src].success;
      }

    else {
      pairs[src].success = 0;
      pairs[src].route_length = 0;
      next_stop[src] = -1;
      }
    }
  last_goal[src] = goal;
  }

void greedy_routing_to(iddata& d, int goal) {
  int N = isize(rogueviz::vdata);
  vector<ld> distances_from_goal(N);
  for(int src=0; src<N; src++)
    distances_from_goal[src] = current->distance(goal, src);
  for(int src=0; src<N; src++)
    route_from(src, goal, distances_from_goal);

  for(int j=0; j<N; j++) if(j != goal){
    d.tot++;
    ld p = pairs[j].success;

    d.suc += p;
    if(pairs[j].route_length > 0) {
      d.routedist += pairs[j].route_length / actual[goal][j];
      d.eff += p * p * actual[goal][j] / pairs[j].route_length;
      }

    ld mp = pairs[j].modded_success;
    d.msuc += mp;
    if(pairs[j].modded_route_length > 0) {
      d.mroutedist += pairs[j].modded_route_length / actual[goal][j];
      d.meff += mp * mp * actual[goal][j] / pairs[j].modded_route_length;
      }
    }
  }

void greedy_routing(iddata& d) {
  int N = isize(rogueviz::vdata);
  for(int goal=0; goal<N; goal++) greedy_routing_to(d, goal);
  }

#if 0
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
    report("str_" + cap, d.routedist / d.tot);
    }

  println(hlog, "HDR;", separated(";", reps), ";N;GROWTH\n");
  println(hlog, "RES;", separated(";", values), ";", N, ";", cgi.expansion->get_growth());
  }
#endif

int current_goal;
iddata prepared;
void prepare_goal(int goal) {
  greedy_routing_to(prepared, current_goal = goal);
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

void full_routing() {
  iddata result;
  prepare_pairs();
  if(1) {
    int N = isize(rogueviz::vdata);
    progressbar pb(N, "greedy routing");
    for(int goal=0; goal<N; goal++) {
      pb++;
      greedy_routing_to(result, goal);
      }
    }
  println(hlog, "greedy routing: success = ", result.suc / result.tot, " stretch = ", result.routedist / result.suc, " efficiency = ", result.eff / result.tot);
  println(hlog, "modded routing: success = ", result.msuc / result.tot, " stretch = ", result.mroutedist / result.msuc, " efficiency = ", result.meff / result.tot);
  }

int routing_args() {
  using namespace arg;

  if(argis("-routing")) {
    // shift(); routing_test(args());
    }
  else if(argis("-gr")) full_routing();
  else return 1;

  return 0;
  }

auto arou = addHook(hooks_args, 50, routing_args);

}
}