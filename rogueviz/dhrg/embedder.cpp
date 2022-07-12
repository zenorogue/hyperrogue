// local search

namespace dhrg {

int newmoves = 0;
int iterations = 0;
int distlimit;

void dispnewmoves() {
  if(newmoves == 0) printf(".");
  else if(newmoves < 10) printf("%c", '0'+newmoves);
  else printf("%c", 'a' + newmoves/10);
  newmoves = 0;
  }

vector<char> tomove;

bool smartmove = false;
bool dorestart = false;

int lastmoves;

int movearound() {
  indenter_finish im("movearound");
  int total = 0;
  if(smartmove) for(bool b: tomove) if(b) total++;
  if(total == 0) {
    tomove.resize(0), tomove.resize(N, true);
    }
  int moves = 0;
  ld llo = loglik_chosen();
//  int im = 0;
  
  {progressbar pb(N, "tomove: " + its(total) + " (last: " + its(lastmoves) + ")");
  for(int i=0; i<N; i++) {
    if(!tomove[i]) { pb++; continue; }
    tomove[i] = false;
    // if(i && i % 100 == 0) dispnewmoves();
    mycell *mc = vertices[i];
    tallyedgesof(i, -1, mc);
    add_to_set(mc, -1, 0);
    add_to_tally(mc, -1, 0);
//    im += size(rogueviz::vdata[i].edges);
    mycell *mc2[FULL_EDGE];
    ld llo2[FULL_EDGE];
    int bestd = -1;
    ld bestllo = llo;
    
    auto nei = allneighbors(mc);
    
    for(int d=0; d<isize(nei); d++) {
      mc2[d] = nei[d];
      if(mc2[d]->lev >= distlimit) continue;
      tallyedgesof(i, 1, mc2[d]);
      add_to_tally(mc2[d], 1, 0);
      if(lc_type == 'C')
        add_to_set(mc2[d], 1, 0);
      llo2[d] = loglik_chosen();
      if(lc_type == 'C')
        add_to_set(mc2[d], -1, 0);
      if(llo2[d] > bestllo) bestd = d, bestllo = llo2[d];
      
      add_to_tally(mc2[d], -1, 0);
      tallyedgesof(i, -1, mc2[d]);
      }
    if(bestd >= 0) {
      moves++; newmoves++;
      vertices[i] = mc = mc2[bestd];
      llo = llo2[bestd];
      tomove[i] = true;
      for(auto p: rogueviz::vdata[i].edges) {
        int j = p.second->i ^ p.second->j ^ i;
        tomove[j] = true;
        }
      }
    tallyedgesof(i, 1, mc);
    add_to_tally(mc, 1, 0);
    add_to_set(mc, 1, 0);
    pb++;
    }}
  // dispnewmoves();
  println(hlog, " moves = ", moves);
  return lastmoves = moves;
  }

int move_restart() {
  indenter_finish im("move_restart");
  ld llo = loglik_chosen();
  array<array<int, 128>, 2> distances_map = {0};
  int moves = 0;
//  int im = 0;
  
  {progressbar pb(N, "move_restart");
  for(int i=0; i<N; i++) {
   // if(i && i % 100 == 0) dispnewmoves();
    mycell *mc = vertices[i];
    tallyedgesof(i, -1, mc);
    add_to_set(mc, -1, 0);
    add_to_tally(mc, -1, 0);
    
    auto getllo = [&] (mycell *m) {
      tallyedgesof(i, 1, m);
      add_to_tally(m, 1, 0);
      if(lc_type == 'C')
        add_to_set(m, 1, 0);
      ld res = loglik_chosen();
      if(lc_type == 'C')
        add_to_set(m, -1, 0);
      add_to_tally(m, -1, 0);
      tallyedgesof(i, -1, m);
      return res;
      };

    mycell* whereto = mroot;
    ld bestllo = getllo(whereto);
    bool changed = true;
    while(changed) {
      changed = false;
      mycell *mc2 = whereto;
      auto nei2 = allneighbors(whereto);
      for(mycell *mcn: nei2) {
        ld newllo = getllo(mcn);
        if(newllo > bestllo)
          bestllo = newllo, mc2 = mcn, changed = true;
        }
      if(changed) whereto = mc2;
      }
    bool better = bestllo > llo;
    distances_map[better][quickdist(whereto, mc)]++;
    if(better) {
      llo = bestllo;
      vertices[i] = mc = whereto;
      moves++;
      }
    tallyedgesof(i, 1, mc);
    add_to_tally(mc, 1, 0);
    add_to_set(mc, 1, 0);
    pb++;
    }}
  // dispnewmoves();
  println(hlog, " moves = ", moves);
  print(hlog, " stats:");
  for(int a=0; a<2; a++) for(int b=0; b<128; b++) {
    int d = distances_map[a][b];
    if(d) print(hlog, format(" %d/%d:%d", a,b, d));
    }
  println(hlog, "\n");
  return lastmoves = moves;
  }

// 7: 12694350 3847975716
// 6: 39472959 11969080911

void verifycs() {
  long long edgecs = 0, totalcs = 0;
  for(int u=0; u<MAXDIST; u++) 
    edgecs += edgetally[u] * u*u,
    totalcs += tally[u] * u*u;
  
  print(hlog, "edgecs=", format("%lld", edgecs), " totalcs=", format("%lld", totalcs));
  }

void preparegraph() {
  indenter_finish im("preparegraph");
  using namespace rogueviz;
  M = 0;
  vertices.resize(N);

  if(1) {
    progressbar pb(N, "Finding all");
    for(int i=0; i<N; i++) {
      M += isize(vdata[i].edges); 
      pb++;
      }
    M /= 2;
    }

  memoryInfo();
  
  cont_logistic.setRT(graph_R, graph_T);

  counttallies();
  
  memoryInfo();
  
  verifycs();

  if(1) {
    indenter_finish im("optimizing parameters"); 
    ld factor = 1/log(cgi.expansion->get_growth());
    current_logistic.setRT(factor * graph_R, factor * graph_T);
    saved_logistic = current_logistic; 

    // for(int u=0; u<MAXDIST; u++) iprintf("%d/%Ld\n", edgetally[u], tally[u]);

    fix_logistic_parameters(current_logistic, loglik_logistic, "logistic", 1e-6);
    writestats();
    fflush(stdout);
    }
  
  iterations = 0;

  distlimit = 0;
  for(int j=0; j<BOXSIZE; j++) 
    if(getsegment(mroot,mroot,0)->qty[j])
      distlimit = min(j+2, BOXSIZE-1);

  println(hlog, "Using distlimit = ", distlimit);
  }

void read_graph_full(const string& fname) {
  using namespace rogueviz;
  
  memoryInfo();

  if(true) {
    indenter_finish im("Read graph");
  
    // N = isize(vdata);
  
    read_graph(fname, false, false, false);
    vertices.resize(N);
    progressbar pb(N, "Translating to cells");

    for(int i=0; i<N; i++) {
#if BUILD_ON_HR    
      cell *c = currentmap->gamestart();
      hyperpoint T0 = vdata[i].m->at * C0;
      virtualRebase2(vdata[i].m->base, T0, true); 
      vertices[i] = find_mycell(vdata[i].m->base);
#else
      vertices[i] = find_mycell_by_path(computePath(vdata[i].m->at));
#endif
      vdata[i].m->at = Id;
      pb++;
      // printf("%s\n", computePath(vdata[i].m->base).c_str());
      }    
    }

  recycle_compute_map();
  preparegraph();  
  }

void graph_from_rv() {
  using namespace rogueviz;
  
  memoryInfo();
  
  vertices.resize(N);
  progressbar pb(N, "converting RogueViz to DHRG");

  for(int i=0; i<N; i++) {
#if BUILD_ON_HR    
    vertices[i] = find_mycell(vdata[i].m->base);
#else
    auto path1 = computePath(vdata[i].m->base);
    vertices[i] = find_mycell_by_path(path1);
#endif
    vdata[i].m->at = Id;
    pb++;
    }    

  preparegraph();
  }

bool iteration() {
  iterations++;
  indenter_finish im("Iteration #" + its(iterations));
  int m = movearound();
  if(!m && dorestart) m = move_restart();
  if(!m) return false;
  fix_logistic_parameters(current_logistic, loglik_logistic, "logistic", 1e-6);
  writestats();
  fflush(stdout);
  return true;
  }

void embedder_loop(int max) {
  indenter_finish im("embedder_loop");
  ld lastopt = loglik_chosen();
  
  while(max-- && iteration()) {
    ld curopt = loglik_chosen();
    println(hlog, "current = %", curopt);
    if(curopt <= lastopt) {
      println(hlog, "Not enough improvement -- breaking"); 
      break;
      }
    lastopt = curopt;
    }
  }

void save_embedding(const string s) {
  FILE *f = fopen(s.c_str(), "wt");
  for(int i=0; i<N; i++) {
    string p = get_path(vertices[i]);
    if(p == "") p = "X";
    fprintf(f, "%s %s\n", rogueviz::vdata[i].name.c_str(), p.c_str());
    }
  fclose(f);
  }

void load_embedded(const string s) {
  if(true) {
    read_graph(s, false, false, false);
    indenter_finish im("Read graph");
    }

  string t = rogueviz::fname + "-dhrg.txt";
      
  if(true) {
    progressbar pb(N, "reading embedding");
    vertices.resize(N, NULL);
    
    map<string, int> ids;
    for(int i=0; i<N; i++) ids[rogueviz::vdata[i].name] = i;

    FILE *f = fopen(t.c_str(), "rt");
    while(true) {
      char who[500], where[500];
      who[0] = 0;
      fscanf(f, "%s%s", who, where);
      if(who[0] == 0) break;
      if(!ids.count(who)) printf("unknown vertex: %s\n", who);
      string wh = where;
      if(wh == "X") wh = "";
      vertices[ids[who]] = find_mycell_by_path(wh);
      pb++;
      }
    fclose(f);
    
    for(int i=0; i<N; i++) if(vertices[i] == NULL) {
      printf("unmapped: %s\n", rogueviz::vdata[i].name.c_str());
      exit(1);
      }
    }
  preparegraph();
  }

}
