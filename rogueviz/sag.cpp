#include "rogueviz.h"

// SAG visualizer (e.g. Reddit roguelikes, GitHub languages)
//-----------------------------------------------------------

// see: https://www.youtube.com/watch?v=mDG3_f8R2Ns (SAG boardgames)
// see: https://www.youtube.com/watch?v=WSyygk_3j9o (SAG roguelikes)
// see: https://www.youtube.com/watch?v=HWQkDkeEUeM (SAG programming languages)

namespace rogueviz {

namespace sag {

  bool turn(int delta);
  
  int sag_id;

  int sagpar = 0;

  enum eSagmode { sagOff, sagHC, sagSA };
  
  eSagmode sagmode; // 0 - off, 1 - hillclimbing, 2 - SA

  const char *sagmodes[3] = {"off", "HC", "SA"};
  
  ld temperature = -4;
  const int INSNAKE = 117;
  int numsnake;
  const char *loadfname;
  
  #define MAXSNAKETAB 1000
  int sdist[MAXSNAKETAB][MAXSNAKETAB];
  int insnaketab = 0;

  vector<cell*> snakecells;
  vector<int> snakefirst, snakelast;
  vector<int> snakenode;
  vector<int> snakeid;
  vector<int> lpbak;
  vector<int> wpbak;
  
  bool snake_enabled;

  void setsnake(cellwalker& cw, int i) {
    lpbak[i] = cw.at->landparam;
    wpbak[i] = cw.at->wparam;
    cw.at->landparam = i; cw.at->wparam = INSNAKE;
    // cw.at->monst = moWormtail; cw.at->mondir = cw.spin;
    snakecells[i] = cw.at;
    }
  
  void snakeswitch() { 
    for(int i=0; i<numsnake; i++) {
      cell *c = snakecells[i];
      int x;
      x = lpbak[i]; lpbak[i] = c->landparam; c->landparam = x;
      x = wpbak[i]; wpbak[i] = c->wparam; c->wparam = x;
      }
    snake_enabled = !snake_enabled;
    }
    
  void enable_snake() { if(!snake_enabled) snakeswitch(); }
    
  void disable_snake() { if(snake_enabled) snakeswitch(); }
    
  int snakedist(int i, int j) {
    if(i < insnaketab && j < insnaketab) return sdist[i][j];
    if(bounded) return celldistance(snakecells[i], snakecells[j]);
    int i0 = i, i1 = i, j0 = j, j1 = j;
    int cost = 0;
    // intersect
    while(true) {
      if(j0 > i1+1) { j0 = snakefirst[j0], j1 = snakelast[j1]; cost++; }
      else if(i0 > j1+1) { i0 = snakefirst[i0], i1 = snakelast[i1]; cost++; }
      else if(j1+1 == i0) return cost+1;
      else if(i1+1 == j0) return cost+1;
      else return cost;
      }
    }
  
  void initSnake(int n) {
    if(bounded) n = isize(currentmap->allcells());
    numsnake = n;
    snakecells.resize(numsnake);
    snakefirst.resize(numsnake);
    snakelast.resize(numsnake);
    snakenode.resize(numsnake);
    lpbak.resize(numsnake);
    wpbak.resize(numsnake);
    if(bounded) {
      for(int i=0; i<n; i++) {
        cellwalker cw(currentmap->allcells()[i], 0);
        setsnake(cw, i);
        }
      }
    else {
      cellwalker cw = cwt;
      setsnake(cw, 0);
      cw += wstep;
      setsnake(cw, 1);
      for(int i=2; i<=numsnake; i++) {
        if(i == numsnake && sphere) break;
        cw += wstep;
        snakefirst[i-1] = cw.at->landparam;
        while(cw.at->wparam == INSNAKE) {
          snakelast[i-1] = cw.at->landparam;
          cw = cw + wstep + 1 + wstep;
          }
        if(i == numsnake) break;
        setsnake(cw, i); cw += 1;
        }
      }
    int stab = min(numsnake, MAXSNAKETAB);
    for(int i=0; i<stab; i++)
    for(int j=0; j<stab; j++)
      sdist[i][j] = snakedist(i,j);
    insnaketab = stab;
    snake_enabled = true;
    }
  
  double costat(int vid, int sid) {
    if(vid < 0) return 0;
    double cost = 0;
    vertexdata& vd = vdata[vid];
    for(int j=0; j<isize(vd.edges); j++) {
      edgeinfo *ei = vd.edges[j].second;
      int t2 = vd.edges[j].first;
      if(snakeid[t2] != -1) cost += snakedist(sid, snakeid[t2]) * ei->weight2;
      }
    /* cell *c = snakecells[id];
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->move(i);
      if(c2 && c2->wparam == INSNAKE && snakenode[c2->landparam] >= 0)
        cost += 100;
      } */
    return cost;
    }
  
  // std::mt19937 los;

  bool infullsa;
  
  double cost;
  int N;

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
    aiter:

    int t1 = hrand(N);
    int sid1 = snakeid[t1];
    
    int sid2;
    
    int s = hrand(6);
    
    if(s == 3) s = 2;
    if(s == 4) s = 5;
    
    if((sagpar&1) && (s == 2 || s == 3 || s == 4)) return;
    
    if(s == 5) sid2 = hrand(numsnake);
    
    else {
      cell *c;
      if(s>=2 && isize(vdata[t1].edges)) c = snakecells[snakeid[hrand(isize(vdata[t1].edges))]];
      else c = snakecells[sid1];
      
      int it = s<2 ? (s+1) : s-2;
      for(int ii=0; ii<it; ii++) {
        int d = hrand(c->type);
        c = c->move(d);
        if(!c) goto aiter;
        if(c->wparam != INSNAKE) goto aiter;
        }
      sid2 = c->landparam;
      }
    int t2 = snakenode[sid2];
    
    snakenode[sid1] = -1; snakeid[t1] = -1;
    snakenode[sid2] = -1; if(t2 >= 0) snakeid[t2] = -1;
    
    double change = 
      costat(t1,sid2) + costat(t2,sid1) - costat(t1,sid1) - costat(t2,sid2);

    snakenode[sid1] = t1; snakeid[t1] = sid1;
    snakenode[sid2] = t2; if(t2 >= 0) snakeid[t2] = sid2;
    
    if(change < 0) chgs.push_back(-change);
      
    if(change > 0 && (sagmode == sagHC || !chance(exp(-change * exp(-temperature))))) return;

    snakenode[sid1] = t2; snakenode[sid2] = t1;
    snakeid[t1] = sid2; if(t2 >= 0) snakeid[t2] = sid1;
    if(vdata[t1].m) vdata[t1].m->base = snakecells[sid2];
    if(t2 >= 0 && vdata[t2].m) vdata[t2].m->base = snakecells[sid1];
    cost += 2*change;
    
    if(t1 >= 0) forgetedges(t1);
    if(t2 >= 0) forgetedges(t2);
    }
  
  void organize() {
    for(int i=0; i<numsnake; i++) snakenode[i] = -1;
    vector<int> freenodes;
    for(int i=0; i<N; i++) 
      if(snakeid[i] != -1)
        snakenode[snakeid[i]] = i;

    for(int i=0; i<N; i++) 
      if(snakeid[i] != -1)
        if(snakenode[snakeid[i]] != i)
          snakeid[i] = -1;

    for(int i=0; i<numsnake; i++) 
      if(snakenode[i] == -1)
        freenodes.push_back(i);
    
    int j = 0;
    for(int i=0; i<N; i++) 
      if(snakeid[i] == -1) {
        snakeid[i] = freenodes[j];
        snakenode[freenodes[j]] = i;
        j++;
        }
    cost = 0; for(int i=0; i<N; i++) cost += costat(i, i);
    }
  
  void loadsnake(const string& fname) {
    printf("Loading the sag from: %s\n", fname.c_str());
    FILE *sf = fopen(fname.c_str(), "rt");
    if(!sf) { printf("Failed to open file.\n"); exit(1); }
    if(sf) while(true) {
      string lab;
      while(true) {
        int c = fgetc(sf);
        if(c == EOF) goto afterload;
        else if(c == 10 || c == 13 || c == 32 || c == 9) ;
        else if(c == ',' || c == ';') break;
        else lab += c;
        }
      int sid = -1;
      int err = fscanf(sf, "%d", &sid);
      if(sid < 0 || sid >= numsnake || err < 1) sid = -1;
      if(!labeler.count(lab)) {
        printf("unknown vertex: %s\n", lab.c_str());
        }
      else {
        int id = getid(lab);
        snakeid[id] = sid;
        }
      }
    afterload: 
    if(sf) fclose(sf);

    organize();
    for(int i=0; i<N; i++) {
      if(vdata[i].m) vdata[i].m->base = snakecells[sag::snakeid[i]];
      forgetedges(i);
      }

    shmup::fixStorage();
    }
  
  vector<edgeinfo> sagedges;
  
  /* bool totcmp(int i, int j) {
    return totwei[i] > totwei[j];
    } */
  
  int ipturn = 100;
  int numiter = 0;
  
  int hightemp = 10;
  int lowtemp = -15;
  
  void dofullsa(int satime) {
    sagmode = sagSA;
    enable_snake();
    int t1 = SDL_GetTicks();
    
    while(true) {
      int t2 = SDL_GetTicks();
      double d = (t2-t1) / (1000. * satime);
      if(d > 1) break;
      temperature = hightemp - (d*(hightemp-lowtemp));
      chgs.clear();
      for(int i=0; i<50000; i++) {
        numiter++;
        sag::saiter();
        }
      DEBB(DF_LOG, (format("it %8d temp %6.4f [1/e at %13.6f] cost = %f ", 
        numiter, double(sag::temperature), (double) exp(sag::temperature),
        double(sag::cost))));
      
      sort(chgs.begin(), chgs.end());
      int cc = chgs.size() - 1;
      DEBB(DF_LOG, (format("%9.4f .. %9.4f .. %9.4f .. %9.4f .. %9.4f\n", 
        double(chgs[0]), double(chgs[cc/4]), double(chgs[cc/2]), double(chgs[cc*3/4]), double(chgs[cc]))));
      fflush(stdout);
      }
    
    temperature = -5;
    disable_snake();
    sagmode = sagOff;
    }

  void iterate() {
    if(!sagmode) return;
    int t1 = SDL_GetTicks();
    enable_snake();
    for(int i=0; i<ipturn; i++) {
      numiter++;
      sag::saiter();
      }
    disable_snake();
    int t2 = SDL_GetTicks();
    int t = t2 - t1;
    if(t < 50) ipturn *= 2;
    else if(t > 200) ipturn /= 2;
    else ipturn = ipturn * 100 / t;
    DEBB(DF_LOG, ("it %8d temp %6.4f [2:%8.6f,10:%8.6f,50:%8.6f] cost = %f\n", 
      numiter, double(sag::temperature), 
      (double) exp(-2 * exp(-sag::temperature)),
      (double) exp(-10 * exp(-sag::temperature)),
      (double) exp(-50 * exp(-sag::temperature)),
      (double) sag::cost));
    }
  
  void savesnake(const string& fname) {
    FILE *f = fopen(fname.c_str(), "wt");
    for(int i=0; i<N; i++)
      fprintf(f, "%s;%d\n", vdata[i].name.c_str(), snakeid[i]);
    fclose(f);
    }
  
  void loglik() {
    int indist[30], pedge[30];
    for(int d=0; d<30; d++) indist[d] = 0, pedge[d] = 0;
    
    for(int i=0; i<N; i++)
    for(int j=0; j<i; j++)
      indist[snakedist(snakeid[i], snakeid[j])]++;
      
    for(int i=0; i<isize(sagedges); i++) {
      edgeinfo& ei = sagedges[i];
      if(snakedist(snakeid[ei.i], snakeid[ei.j]) == 0) {
        printf("zero between %d (%s) and %d (%s)\n", 
          snakeid[ei.i], vdata[ei.i].name.c_str(),
          snakeid[ei.j], vdata[ei.j].name.c_str());
        }
      if(ei.weight >= sag_edge->visible_from)
        pedge[snakedist(snakeid[ei.i], snakeid[ei.j])]++;
      }
    
    for(int d=0; d<30; d++) 
      if(indist[d])
        printf("%2d: %7d/%7d %7.3lf\n", 
          d, pedge[d], indist[d], double(pedge[d] * 100. / indist[d]));
        
    ld loglik = 0;
    for(int d=0; d<30; d++) {
      int p = pedge[d], pq = indist[d];
      int q = pq - p;
      if(p && q)
        loglik += p * log(p) + q * log(q) - pq * log(pq);
      }
    
    println(hlog, "loglikelihood = ", fts(loglik));
    }
  
  void readsag(const char *fname) {
    maxweight = 0;
    sag_edge = add_edgetype("SAG edge");
    fhstream f(fname, "rt");
    if(!f.f) { printf("Failed to open SAG file: %s\n", fname); exit(1); }
    // while(fgetc(f) != 10 && fgetc(f) != 13 && !feof(f)) ;
    while(!feof(f.f)) {
      string l1, l2;
      while(true) {
        int c = fgetc(f.f);
        if(c == EOF) return;
        else if(c == ';') break;
        else if(c == 10 || c == 13 || c == 32 || c == 9) ;
        else l1 += c;
        }
      while(true) {
        int c = fgetc(f.f);
        if(c == EOF) return;
        else if(c == ';') break;
        else if(c == 10 || c == 13 || c == 32 || c == 9) ;
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

  void read(string fn) {
    fname = fn;
    init(&sag_id, RV_GRAPH | RV_WHICHWEIGHT | RV_AUTO_MAXWEIGHT | RV_HAVE_WEIGHT);

    rv_hook(rogueviz::hooks_close, 100, [] { sag::sagedges.clear(); });
    rv_hook(shmup::hooks_turn, 100, turn);
    rv_hook(rogueviz::hooks_rvmenu, 100, [] { 
      dialog::addSelItem(XLAT("temperature"), fts(sag::temperature), 't');
      dialog::add_action([] {
        dialog::editNumber(sag::temperature, sag::lowtemp, sag::hightemp, 1, 0, XLAT("temperature"), "");
        });
      dialog::addSelItem(XLAT("SAG mode"), sag::sagmodes[sag::sagmode], 'm'); 
      dialog::add_action([] { sag::sagmode = sag::eSagmode( (1+sag::sagmode) % 3 ); });
      });

    weight_label = "min weight";
    temperature = 0; sagmode = sagOff;
    readsag(fname.c_str());
    
    N = isize(vdata);
    // totwei.resize(N);
    // for(int i=0; i<N; i++) totwei[i] = 0;
    
    for(int i=0; i<N; i++) vdata[i].data = 0;
    /* for(int i=0; i<isize(sagedges); i++) {
      edgeinfo& ei = sagedges[i];
      // maxwei[ei.i] = max(maxwei[ei.i], ei.weight);
      // maxwei[ei.j] = max(maxwei[ei.j], ei.weight);
      // totwei[ei.i] += ei.weight;
      // totwei[ei.j] += ei.weight;
      } */
    for(int i=0; i<isize(sagedges); i++) {
      edgeinfo& ei = sagedges[i];
      // (ei.weight >= maxwei[ei.i] / 5 || ei.weight >= maxwei[ei.j] / 5);

      ei.weight2 = pow((double) ei.weight, (double) edgepower) * edgemul;
      // LANG:: pow(ei.weight, .4) / 50;      
      
      // ei.weight2 = 0; int w = ei.weight; while(w) { w >>= 1; ei.weight2++; }      
      /* if(totwei[ei.i] <= 0 || totwei[ei.j] <= 0) {
        printf("BAD TOTWEI\n");
        exit(1);
        }
      ei.weight2 = 3 * (
        sqrt(ei.weight * 1. / totwei[ei.i]) * log(totwei[ei.i]) * log(totwei[ei.i]) +
        sqrt(ei.weight * 1. / totwei[ei.j]) * log(totwei[ei.j]) * log(totwei[ei.j])); */
      // printf("%f\n", ei.weight2);
      addedge0(ei.i, ei.j, &ei);
      }
  
    initSnake(N*2);
    printf("numsnake = %d\n", numsnake);
    if(numsnake < N) {
      printf("Error: snake does not fit\n");
      exit(1);
      }
    snakeid.resize(N);
    for(int i=0; i<N; i++) snakeid[i] = -1;
    organize();
    disable_snake();

    for(int i=0; i<N; i++) {
      int ii = i;
      vertexdata& vd = vdata[ii];
      vd.cp = colorpair(dftcolor);
      createViz(ii, sag::snakecells[sag::snakeid[i]], Id);
      }

    storeall();
    }

int readArgs() {
#if CAP_COMMANDLINE
  using namespace arg;

  if(0) ;

  else if(argis("-sagmin")) {
    shift_arg_formula(default_edgetype.visible_from);
    default_edgetype.visible_from_hi = default_edgetype.visible_from;
    default_edgetype.visible_from_help = default_edgetype.visible_from;    
    }
  else if(argis("-sagminhi")) {
    shift_arg_formula(default_edgetype.visible_from_hi);
    }
  else if(argis("-sagminhelp")) {
    shift_arg_formula(default_edgetype.visible_from_help);
    }

// (1) configure edge weights
  else if(argis("-edgepower")) {
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
// (3) load the initial positioning
  else if(argis("-gload")) {
    PHASE(3); shift(); sag::loadsnake(args());
    }
// (4) perform simulated annealing: -fullsa <time in seconds>
  else if(argis("-fullsa")) {
    shift(); sag::dofullsa(argi());
    }
// (5) save the positioning
  else if(argis("-gsave")) {
    PHASE(3); shift(); sag::savesnake(args());
    }
// (6) output loglikelihood
  else if(argis("-lik")) {
    sag::loglik();
    }
  else return 1;
#endif
  return 0;
  }

bool turn(int delta) {
  return false;
  // shmup::pc[0]->rebase();
  }

string cname() {
  if(euclid) return "coord-6.txt";
  if(PURE) return "coord-7.txt";
  return "coord-67.txt";
  }

int ah = addHook(hooks_args, 100, readArgs)
  + addHook(pres::hooks_build_rvtour, 120, [] (string s, vector<tour::slide>& v) {
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
    
          rogueviz::sag::read(RVPATH "roguelikes/edges.csv");
          rogueviz::readcolor(RVPATH "roguelikes/color.csv");
          rogueviz::sag::loadsnake(RVPATH "roguelikes/" + cname());
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

      rogueviz::sag::read(RVPATH "lang/edges.csv");
      rogueviz::readcolor(RVPATH "lang/color.csv");
      rogueviz::sag::loadsnake(RVPATH "lang/" + cname());
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

      rogueviz::sag::read(RVPATH "boardgames/edges.csv");
      rogueviz::readcolor(RVPATH "boardgames/color.csv");
      rogueviz::sag::loadsnake(RVPATH "boardgames/" + cname());
      })
        });

    });

EX }

}
