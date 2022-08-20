#include "rogueviz.h"

// SAG visualizer (e.g. Reddit roguelikes, GitHub languages)
//-----------------------------------------------------------

// see: https://www.youtube.com/watch?v=mDG3_f8R2Ns (SAG boardgames)
// see: https://www.youtube.com/watch?v=WSyygk_3j9o (SAG roguelikes)
// see: https://www.youtube.com/watch?v=HWQkDkeEUeM (SAG programming languages)

namespace rogueviz {

namespace sag {

  bool turn(int delta);
  
  int sagpar = 0;

  enum eSagmode { sagOff, sagHC, sagSA };
  
  eSagmode sagmode; // 0 - off, 1 - hillclimbing, 2 - SA

  const char *sagmodes[3] = {"off", "HC", "SA"};
  
  ld temperature = -4;
  const char *loadfname;
  
  int vizsa_start;
  int vizsa_len;
  
  /** all the SAG cells */
  vector<cell*> sagcells;

  /** table of distances between SAG cells */
  vector<vector<int>> sagdist;

  /** what node is on sagcells[i] */
  vector<int> sagnode;

  /** node i is on sagcells[sagid[i]] */
  vector<int> sagid;

  /** sagcells[ids[c]]] == c */
  map<cell*, int> ids;

  /** if i in neighbors[j], sagcells[i] is a neighbor of sagcells[j] */
  vector<vector<int>> neighbors;
  
  void compute_dists() {
    int N = isize(sagcells);

    neighbors.clear();
    neighbors.resize(N);

    for(int i=0; i<N; i++)
      for(cell *c1: adj_minefield_cells(sagcells[i]))
        if(ids.count(c1)) neighbors[i].push_back(ids[c1]);

    sagdist.clear();
    sagdist.resize(N);
    for(int i=0; i<N; i++) {
      auto &sdi = sagdist[i];
      sdi.resize(N, N);
      vector<int> q;
      auto visit = [&] (int j, int dist) { if(sdi[j] < N) return; sdi[j] = dist; q.push_back(j); };
      visit(i, 0);
      for(int j=0; j<isize(q); j++) for(int k: neighbors[q[j]]) visit(k, sdi[q[j]]+1);
      }
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

  ld hub_penalty;
  string hub_filename;
  vector<int> hubval;
  
  double costat(int vid, int sid) {
    if(vid < 0) return 0;
    double cost = 0;
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

  bool infullsa;
  
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
    cost += 2*change;
    }
  
  void create_sagnode() {
    sagnode.clear();
    sagnode.resize(isize(sagcells), -1);
    int DN = isize(sagid);
    for(int i=0; i<DN; i++)
      sagnode[sagid[i]] = i;
    cost = 0;
    for(int i=0; i<DN; i++)
      cost += costat(i, sagid[i]);
    }
  
  void reassign() {
    int DN = isize(sagid);
    for(int i=0; i<DN; i++) {
      vdata[i].m->base = sagcells[sag::sagid[i]];
      forgetedges(i);
      }
    shmup::fixStorage();
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

    create_sagnode();
    reassign();
    }
  
  vector<edgeinfo> sagedges;
  
  int ipturn = 100;
  int numiter = 0;
  
  int hightemp = 10;
  int lowtemp = -15;
  
  void dofullsa(int satime) {
    sagmode = sagSA;
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
      
      print(hlog, format("it %8d temp %6.4f [1/e at %13.6f] cost = %f ", 
        numiter, double(sag::temperature), (double) exp(sag::temperature),
        double(sag::cost)));
      
      sort(chgs.begin(), chgs.end());
      int cc = chgs.size() - 1;
      println(hlog, format("%9.4f .. %9.4f .. %9.4f .. %9.4f .. %9.4f", 
        double(chgs[0]), double(chgs[cc/4]), double(chgs[cc/2]), double(chgs[cc*3/4]), double(chgs[cc])));
      }
    
    temperature = -5;
    sagmode = sagOff;
    reassign();
    }

  void iterate() {
    if(!sagmode) return;
    int t1 = SDL_GetTicks();
    for(int i=0; i<ipturn; i++) {
      numiter++;
      sag::saiter();
      }
    int t2 = SDL_GetTicks();
    int t = t2 - t1;
    if(t < 50) ipturn *= 2;
    else if(t > 200) ipturn /= 2;
    else ipturn = ipturn * 100 / t;
    print(hlog, format("it %8d temp %6.4f [2:%8.6f,10:%8.6f,50:%8.6f] cost = %f\n", 
      numiter, double(sag::temperature), 
      (double) exp(-2 * exp(-sag::temperature)),
      (double) exp(-10 * exp(-sag::temperature)),
      (double) exp(-50 * exp(-sag::temperature)),
      (double) sag::cost));

    reassign();
    }
  
  void save_sag_solution(const string& fname) {
    FILE *f = fopen(fname.c_str(), "wt");
    for(int i=0; i<isize(sagid); i++)
      fprintf(f, "%s;%d\n", vdata[i].name.c_str(), sagid[i]);
    fclose(f);
    }
  
  void loglik() {
    int indist[30], pedge[30];
    for(int d=0; d<30; d++) indist[d] = 0, pedge[d] = 0;
    
    int N = isize(sagid);
    for(int i=0; i<N; i++)
    for(int j=0; j<i; j++)
      indist[sagdist[sagid[i]][sagid[j]]]++;
      
    for(int i=0; i<isize(sagedges); i++) {
      edgeinfo& ei = sagedges[i];
      if(ei.i != ei.j)
      if(ei.weight >= sag_edge->visible_from)
        pedge[sagdist[sagid[ei.i]][sagid[ei.j]]]++;
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
    fhstream f(fname, "rt");
    if(!f.f) {
      printf("Failed to open SAG file: %s\n", fname);
      throw "failed to open SAG file";
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

  void read(string fn) {
    fname = fn;
    init(RV_GRAPH | RV_WHICHWEIGHT | RV_AUTO_MAXWEIGHT | RV_HAVE_WEIGHT);

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
    if(hub_filename != "")
      read_hubs(hub_filename);
    
    int DN = isize(vdata);
    
    for(int i=0; i<DN; i++) vdata[i].data = 0;
    for(int i=0; i<isize(sagedges); i++) {
      edgeinfo& ei = sagedges[i];

      ei.weight2 = pow((double) ei.weight, (double) edgepower) * edgemul;
      
      addedge0(ei.i, ei.j, &ei);
      }
  
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
    create_sagnode();

    for(int i=0; i<DN; i++) {
      int ii = i;
      vertexdata& vd = vdata[ii];
      vd.cp = colorpair(dftcolor);
      createViz(ii, sagcells[sagid[i]], Id);
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
  else if(argis("-sagviz")) {
    sag::vizsa_start = SDL_GetTicks();
    shift(); sag::vizsa_len = argi();
    }
// (5) save the positioning
  else if(argis("-sagsave")) {
    PHASE(3); shift(); sag::save_sag_solution(args());
    }
// (6) output loglikelihood
  else if(argis("-sagloglik")) {
    sag::loglik();
    }
  else if(argis("-sagmode")) {
    shift();
    sagmode = (eSagmode) argi();
    if(sagmode == sagSA) {
      shift(); temperature = argf();
      }
    }
  else return 1;
#endif
  return 0;
  }

bool turn(int delta) {
  if(vizsa_start) {
    auto t = ticks;
    double d = (t-vizsa_start) / (1000. * vizsa_len);
    if(d > 1) sagmode = sagOff;
    else {
      temperature = hightemp - (d*(hightemp-lowtemp));
      sagmode = sagSA;
      }
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
