// Hyperbolic Rogue
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

// Hyperbolic geometry is a good tool to visualize data,
// especially trees and scale-free networks. This file
// uses HyperRogue to browse such vizualizations.

// Since it is not of use for general HyperRogue players, 
// it is disabled by default -- compile with the ROGUEVIZ flag to enable this.

// How to use:

// hyper -embed <start of filename> -- visualize a social network
//   embedded into hyperbolic plane, see:
//   https://bitbucket.org/HaiZhung/hyperbolic-embedder/overview
//   (it uses the same format)

// hyper -tol <filename.xml> -- visualize the tree of life,
//   based on a XML dump from https://tree.opentreeoflife.org/

// hyper -tess <parameter file> -- visualize a horocyclic tesselation,

namespace rogueviz {

void init();

bool showlabels = false;
bool specialmark = false;

bool rog3 = false;

string fname;

// const char *fname;
// const char *cfname;

enum eVizkind { kNONE, kAnyGraph, kTree, kSpiral, kSAG, kCollatz, kFullNet, kKohonen };
eVizkind kind;

bool on;

struct edgeinfo {
  int i, j;
  double weight, weight2;
  bool visible;
  vector<GLfloat> prec;
  cell *orig;
  int lastdraw;
  edgeinfo() { visible = true; orig = NULL; lastdraw = -1; }
  };

struct colorpair {
  int color1, color2;
  char shade;
  colorpair(int col = 0xC0C0C0FF) { shade = 0; color1 = col; }
  };

colorpair parse(const string& s) {
  colorpair cp;
  cp.shade = 0; cp.color2 = 0;
  sscanf(s.c_str(), "%x:%c%x", &cp.color1, &cp.shade, &cp.color2);
  return cp;
  }

int nh = 0;
int hues[256*6];

void buildhue() {
  unsigned mh = 193;
  for(unsigned y=0; y<=mh; y++)
    hues[nh++] = (int) (0xFF + 0x1000000*mh + (unsigned) 0x10000 * y);
  for(unsigned y=0; y<=mh; y++)
    hues[nh++] = (int) (0xFF + 0x1010000*mh - 0x1000000 * y);
  for(unsigned y=0; y<=mh; y++)
    hues[nh++] = (int) (0xFF + 0x0010000*mh + 0x100 * y);
  for(unsigned y=0; y<=mh; y++)
    hues[nh++] = (int) (0xFF + 0x0010100*mh - 0x10000 * y);
  for(unsigned y=0; y<=mh; y++)
    hues[nh++] = (int) (0xFF + 0x0000100*mh + 0x1000000 * y);
  for(unsigned y=0; y<=mh; y++)
    hues[nh++] = (int) (0xFF + 0x1000100*mh - 0x100 * y);
  }

int perturb(int c) {
  if(nh == 0) buildhue();
  int hueid = 0;
  for(int t=0; t<nh; t++) if(hues[t] == c) hueid = t;
  hueid += rand() % 50;
  hueid -= rand() % 50;
  if(hueid<0) hueid += nh;
  hueid %= nh;
  return hues[hueid];
  /*
  int part[4];
  for(int u=0; u<=3; u++) {
    part[u] = (c >> (8*u)) & 0xFF;
    }
  int 
  if(part[1] == 255 && part[2] == 0) 
    int k = 
    k += rand() % 16;
    k -= rand() % 16;
    if(k<0) k=-k;
    if(k>255) k = 255-(k-255);
    c &=~ (0xFF << (8*u));
    c |= k << (8*u);
    } */
  return c;
  }

colorpair perturb(colorpair cp) {
  cp.color1 = perturb(cp.color1);
  cp.color2 = perturb(cp.color2);
  return cp;
  }

struct vertexdata {
  vector<pair<int, edgeinfo*> > edges;
  string name;
  colorpair cp;
  edgeinfo *virt;
  bool special;
  int data;
  string *info;
  shmup::monster *m;
  vertexdata() { virt = NULL; m = NULL; info = NULL; special = false; }
  };

vector<vertexdata> vdata;

transmatrix cspin(int i, int ch) {
  return spin(M_PI + (2 * M_PI * (i+1)) / (ch+1));
  }

map<string, int> labeler;

int getid(const string& s) {
  if(labeler.count(s)) return labeler[s];
  else {
    int id = vdata.size();
    vdata.resize(vdata.size() + 1);
    vdata[id].name = s;
    return labeler[s] = id;
    }
  }

int getnewid(string s) {
  while(labeler.count(s)) s += "'";
  return getid(s);
  }
  
void addedge0(int i, int j, edgeinfo *ei) {
  vdata[i].edges.push_back(make_pair(j, ei));
  vdata[j].edges.push_back(make_pair(i, ei));
  }

void createViz(int id, cell *c, transmatrix at) {
  vertexdata& vd(vdata[id]);
  vd.m = new shmup::monster;
  vd.m->pid = id;
  vd.m->type = moRogueviz;
  vd.m->base = c;
  vd.m->at = at;
  }

void notimpl() {
  printf("Not implemented\n"); exit(1); 
  }

hyperpoint where(int i) {
  auto m = vdata[i].m;
  if(m->base == currentmap->gamestart()) return tC0(m->at);
  else { 
    notimpl(); // actually probably that's a buug
    return inverse(shmup::ggmatrix(currentmap->gamestart())) * (shmup::ggmatrix(m->base) * tC0(m->at));
    }
  }
  
void addedge(int i, int j, edgeinfo *ei) {
  hyperpoint hi = where(i);
  hyperpoint hj = where(j);
  double d = hdist(hi, hj);
  if(d >= 4) {
    // printf("splitting %lf\n", d);
    hyperpoint h = mid(hi, hj);
    int id = size(vdata);
    vdata.resize(id+1);
    vertexdata& vd(vdata[id]);
    vd.cp = colorpair(0x400000FF);
    vd.virt = ei;
    
    createViz(id, currentmap->gamestart(), rgpushxto0(h));
    
    addedge(i, id, ei);
    addedge(id, j, ei);
    shmup::virtualRebase(vd.m, true);
    }
  else addedge0(i, j, ei);
  }

vector<edgeinfo*> edgeinfos;

void addedge(int i, int j, double wei, bool subdiv) {
  edgeinfo *ei = new edgeinfo;
  edgeinfos.push_back(ei);
  ei->i = i;
  ei->j = j;
  ei->weight = wei;
  if(subdiv) addedge(i, j, ei);
  else addedge0(i, j, ei);
  }

void storeall() {
  for(int i=0; i<size(vdata); i++)
    if(vdata[i].m)
      vdata[i].m->store();
  }

int dftcolor = 0x282828FF;

namespace spiral {

  ld mul;
  
  transmatrix at(double d) {
    return spin(log(d) * 2 * M_PI / log(mul)) * xpush(log(d));
    }

  void place(int N, ld _mul) {
    mul = _mul;
    init(); kind = kSpiral;
    vdata.resize(N);
  
    for(int i=0; i<N; i++) {
      vertexdata& vd = vdata[i];
      
      double d = i + 1;
      
      transmatrix h = at(d);
  
      createViz(i, cwt.c, h);
      vd.name = its(i+1);
      virtualRebase(vd.m, true);
  
      vd.cp = dftcolor;
      }
  
    storeall();
    }
  
  void edge(ld shift, ld mul) {
    int N = size(vdata);
    for(int i=0; i<N; i++) {
      int i0 = i+1;
      int j0 = int(i0 * mul + shift) - 1;
      if(j0 >= 0 && j0 < N) addedge(i, j0, 0, false);
      }
    }
  
  void color(ld start, ld period, colorpair c) {
    int N = size(vdata);
    int maxw = N;
    while(start >= 0 && start < N) {
      int i = int(start);
      vdata[i].cp = c;
      start += period;
      maxw--; if(maxw <= 0) break;
      }
    }
  }

namespace collatz {

  double s2, s3, p2, p3;
  double cshift = -1;
  
  void start() {
    init(); kind = kCollatz;
    vdata.resize(1);
    vertexdata& vd = vdata[0];
    createViz(0, cwt.c, xpush(cshift));
    virtualRebase(vd.m, true);
    vd.cp = perturb(dftcolor);
    vd.data = 0;
    addedge(0, 0, 0, false);
    vd.name = "1";    
    storeall();
    }  
  }

string readLabel_s(FILE *f) {
  char xlabel[10000];
  if(fscanf(f, "%9500s", xlabel) <= 0) return "";
  return xlabel;
  }

int readLabel(FILE *f) {
  string s = readLabel_s(f);
  if(s == "") return -1;
  return getid(s);
  }

namespace anygraph {
  double R, alpha, T;
  vector<pair<double, double> > coords;
  
  int N;
               
  void fixedges() {
    for(int i=N; i<size(vdata); i++) if(vdata[i].m) vdata[i].m->dead = true;
    for(int i=0; i<size(vdata); i++) vdata[i].edges.clear();
    vdata.resize(N);
    for(auto e: edgeinfos) {
      e->orig = NULL;
      addedge(e->i, e->j, e);
      }
    }

  void read(string fn, bool subdiv = true, bool doRebase = true, bool doStore = true) {
    init(); kind = kAnyGraph;
    fname = fn;
    FILE *f = fopen((fn + "-coordinates.txt").c_str(), "rt");
    if(!f) {
      printf("Missing file: %s-coordinates.txt\n", fname.c_str());
      exit(1);
      }
    printf("Reading coordinates...\n");
    char buf[100];  
    int err;
    err = fscanf(f, "%s%s%s%s%d%lf%lf%lf", buf, buf, buf, buf, &N, 
      &anygraph::R, &anygraph::alpha, &anygraph::T);
    if(err < 8) { printf("Error: incorrect format of the first line\n"); exit(1); }
    vdata.reserve(N);
    while(true) {
      string s = readLabel_s(f);
      if(s == "" || s == "-1") break;
      int id = getid(s);
      vertexdata& vd(vdata[id]);
      vd.name = s;
      vd.cp = colorpair(dftcolor);
      
      double r, alpha;
      int err = fscanf(f, "%lf%lf", &r, &alpha);
      coords.push_back(make_pair(r, alpha));
      if(err < 2) { printf("Error: incorrect format of r/alpha\n"); exit(1); }
  
      transmatrix h = spin(alpha * 2 * M_PI / 360) * xpush(r);
      
      createViz(id, currentmap->gamestart(), h);
      }
    fclose(f);
    
    f = fopen((fn + "-links.txt").c_str(), "rt");
    if(!f) {
      printf("Missing file: %s-links.txt\n", fname.c_str());
      exit(1);
      }
    printf("Reading links...\n");
    int qlink = 0;
    while(true) {
      int i = readLabel(f), j = readLabel(f);
      if(i == -1 || j == -1) break;
      addedge(i, j, 0, subdiv);
      if(qlink % 10000 == 0) printf("%d\n", qlink);
      qlink++;
      }
    fclose(f);
  
    if(doRebase) {
      printf("Rebasing...\n");
      for(int i=0; i<size(vdata); i++) {
        if(i % 10000 == 0) printf("%d/%d\n", i, size(vdata));
        if(vdata[i].m) virtualRebase(vdata[i].m, true);
        }
      printf("Done.\n");
      }
    
    if(doStore) storeall();
    }
  
  }

namespace tree {

  struct treevertex {
    int origid;
    int parent;
    int depth;
    int spos, epos;
    vector<int> children;
    };
  
  vector<treevertex> tol;
  
  void child(int pid, int id) {
    if(size(tol) <= id) tol.resize(id+1);
    
    treevertex& v = tol[id];
    v.parent = pid;
    tol.push_back(v);
    if(pid >= 0) tol[pid].children.push_back(id);
    }

  void readnode(FILE *f, int pid) {
    string lab = "";
    while(true) {
      int c = fgetc(f);
      if(c == EOF) { fprintf(stderr, "Ended prematurely\n"); exit(1); }
      if(c == ',') break;
      if(c == ')') { int id = getnewid(lab); child(pid, id); return; }
      lab += c;
      }
    int id = getnewid(lab);
    child(pid, id);
    while(true) {
      int c = fgetc(f);
//      printf("c=%c at %d/%d\n", c, pid, id);
      if(c == EOF) { fprintf(stderr, "Ended prematurely\n"); exit(1); }
      if(c == ' ' || c == 10 || c == 13 || c == 9 || c == ',') continue;
      else if(c == '(') readnode(f, id);
      else if(c == ')') break;
      }
    }
  
  int xpos;
  void spos(int at, int d) {
    tol[at].spos = xpos++;
    tol[at].depth = d;
    for(int i=0; i<size(tol[at].children); i++)
      spos(tol[at].children[i], d+1);
    tol[at].epos = ++xpos;
    }
    
  void read(const char *fn) {
    fname = fn;
    init(); kind = kTree;
    printf("Reading the tree of life...\n");
    FILE *f = fopen(fname.c_str(), "rt");
    if(!f) { printf("Failed to open tree file: %s\n", fname.c_str()); exit(1); }
    if(fgetc(f) != '(') {
      printf("Error: bad format\n");
      exit(1);
      }
    readnode(f, -1);
    fclose(f);
    int N = size(vdata);
    printf("N = %d\n", N);
    printf("Assigning spos/epos...\n");
    spos(0, 0);
    xpos *= 6;
    printf("Creating vertices...\n");
    for(int i=0; i<N; i++) {
      treevertex& lv = tol[i];
      vertexdata& vd = vdata[i];
    
      transmatrix h = spin((lv.spos + lv.epos) * M_PI / xpos) * xpush(-1.2 + (log(xpos) - log(lv.epos - lv.spos)));

      vd.special = false;
      vd.m = new shmup::monster;
      vd.m->pid = i;
      vd.data = lv.parent;
      createViz(i, cwt.c, h);
      vd.cp = dftcolor; 
      
      if(tol[i].parent >= 0) 
        addedge(i, tol[i].parent, 0, true);
      }
    
    for(int i=0; i<size(vdata); i++) {
      vertexdata& vd = vdata[i];
      virtualRebase(vd.m, true);
      }
    
    printf("Clearing the TOL data...\n");
    tol.clear();
    storeall();
    }
  }

ld maxweight;

namespace sag {

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
    lpbak[i] = cw.c->landparam;
    wpbak[i] = cw.c->wparam;
    cw.c->landparam = i; cw.c->wparam = INSNAKE;
    // cw.c->monst = moWormtail; cw.c->mondir = cw.spin;
    snakecells[i] = cw.c;
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
    if(euclid || sphere) return celldistance(snakecells[i], snakecells[j]);
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
    if(sphere && purehepta) n = 12;
    else if(sphere) n = 32;
    numsnake = n;
    snakecells.resize(numsnake);
    snakefirst.resize(numsnake);
    snakelast.resize(numsnake);
    snakenode.resize(numsnake);
    lpbak.resize(numsnake);
    wpbak.resize(numsnake);
    cellwalker cw = cwt;
    setsnake(cw, 0);
    cwstep(cw);
    setsnake(cw, 1);
    for(int i=2; i<=numsnake; i++) {
      if(i == numsnake && sphere) break;
      cwstep(cw);
      snakefirst[i-1] = cw.c->landparam;
      while(cw.c->wparam == INSNAKE) {
        snakelast[i-1] = cw.c->landparam;
        cwstep(cw); cwspin(cw, 1); cwstep(cw);
        }
      if(i == numsnake) break;
      setsnake(cw, i); cwspin(cw, 1);
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
    for(int j=0; j<size(vd.edges); j++) {
      edgeinfo *ei = vd.edges[j].second;
      int t2 = vd.edges[j].first;
      if(snakeid[t2] != -1) cost += snakedist(sid, snakeid[t2]) * ei->weight2;
      }
    /* cell *c = snakecells[id];
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->mov[i];
      if(c2 && c2->wparam == INSNAKE && snakenode[c2->landparam] >= 0)
        cost += 100;
      } */
    return cost;
    }
  
  MTRand53 los;

  bool infullsa;
  
  double cost;
  int N;

  vector<double> chgs;  

  void forgetedges(int id) {
    for(int i=0; i<size(vdata[id].edges); i++) 
      vdata[id].edges[i].second->orig = NULL;
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
      if(s>=2 && size(vdata[t1].edges)) c = snakecells[snakeid[hrand(size(vdata[t1].edges))]];
      else c = snakecells[sid1];
      
      int it = s<2 ? (s+1) : s-2;
      for(int ii=0; ii<it; ii++) {
        int d = rand() % c->type;
        c = c->mov[d];
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
      
    if(change > 0 && (sagmode == sagHC || los() > exp(-change * exp(-temperature)))) return;

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
      printf("it %8d temp %6.4" PLDF" [1/e at %13.6" PLDF"] cost = %lf ", 
        numiter, sag::temperature, (ld) exp(sag::temperature),
        sag::cost);
      
      sort(chgs.begin(), chgs.end());
      int cc = chgs.size() - 1;
      printf("%9.4lf .. %9.4lf .. %9.4lf .. %9.4lf .. %9.4lf\n", 
        chgs[0], chgs[cc/4], chgs[cc/2], chgs[cc*3/4], chgs[cc]);
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
    printf("it %8d temp %6.4" PLDF" [2:%8.6" PLDF",10:%8.6" PLDF",50:%8.6" PLDF"] cost = %lf\n", 
      numiter, sag::temperature, 
      exp(-2 * exp(-sag::temperature)),
      exp(-10 * exp(-sag::temperature)),
      exp(-50 * exp(-sag::temperature)),
      sag::cost);
    }
  
  void savesnake(const char *fname) {
    FILE *f = fopen(fname, "wt");
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
      
    for(int i=0; i<size(sagedges); i++) {
      edgeinfo& ei = sagedges[i];
      if(snakedist(snakeid[ei.i], snakeid[ei.j]) == 0) {
        printf("zero between %d (%s) and %d (%s)\n", 
          snakeid[ei.i], vdata[ei.i].name.c_str(),
          snakeid[ei.j], vdata[ei.j].name.c_str());
        }
      if(ei.visible)
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
    
    printf("loglikelihood = %lf\n", (double) loglik);
    }
  
  void readsag(const char *fname) {
    maxweight = 0;
    FILE *f = fopen(fname, "rt");
    if(!f) { printf("Failed to open SAG file: %s\n", fname); exit(1); }
    // while(fgetc(f) != 10 && fgetc(f) != 13 && !feof(f)) ;
    while(!feof(f)) {
      string l1, l2;
      while(true) {
        int c = fgetc(f);
        if(c == EOF) { fclose(f); return; }
        else if(c == ';') break;
        else if(c == 10 || c == 13 || c == 32 || c == 9) ;
        else l1 += c;
        }
      while(true) {
        int c = fgetc(f);
        if(c == EOF) { fclose(f); return; }
        else if(c == ';') break;
        else if(c == 10 || c == 13 || c == 32 || c == 9) ;
        else l2 += c;
        }
      double wei;
      int err = fscanf(f, "%lf", &wei);
      if(err < 1) continue;
      edgeinfo ei;
      ei.i = getid(l1);
      ei.j = getid(l2);
      ei.weight = wei;
      sagedges.push_back(ei);
      }
    }
  
  ld edgepower=1, edgemul=1;

  void read(const char *fn) {
    fname = fn;
    init(); kind = kSAG;
    temperature = 0; sagmode = sagOff;
    readsag(fname.c_str());

    N = size(vdata);
    // totwei.resize(N);
    // for(int i=0; i<N; i++) totwei[i] = 0;
    
    for(int i=0; i<N; i++) vdata[i].data = 0;
    /* for(int i=0; i<size(sagedges); i++) {
      edgeinfo& ei = sagedges[i];
      // maxwei[ei.i] = max(maxwei[ei.i], ei.weight);
      // maxwei[ei.j] = max(maxwei[ei.j], ei.weight);
      // totwei[ei.i] += ei.weight;
      // totwei[ei.j] += ei.weight;
      } */
    for(int i=0; i<size(sagedges); i++) {
      edgeinfo& ei = sagedges[i];
      ei.visible = ei.weight >= 0.1;
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

  }

bool edgecmp(edgeinfo *e1, edgeinfo *e2) {
  return e1->weight > e2->weight;
  }

#include "kohonen.cpp"

void describe(cell *c) {
  if(kind == kKohonen) return kohonen::describe(c);
  }

string describe(shmup::monster *m) {
  int i = m->pid;
  vertexdata& vd = vdata[i];

  string o = vd.name+", "+its(size(vd.edges))+" edges";
  /* if(size(vd.edges) < 10) {
    for(int i=0; i<size(vd.edges); i++) 
      o += " " + its(snakedist(vd.snakeid, vd.edges[i]->snakeid));
    } */
  
  vector<edgeinfo*> alledges;
  
  for(int j=0; j<size(vd.edges); j++) 
    alledges.push_back(vd.edges[j].second);
  
  sort(alledges.begin(), alledges.end(), edgecmp);
    
  ::help = "Edges: ";

  if(vd.info) ::help = (*vd.info) + "\n" + help;

  for(int j=0; j<size(alledges); j++) {
    edgeinfo *ei = alledges[j];
    if(!ei->visible) continue;
    int k = ei->i ^ ei->j ^ i;
    help += vdata[k].name;
    help += "/" + fts(ei->weight)+":" + fts(ei->weight2) + " ";
    }
  
  return o;
  }

void activate(shmup::monster *m) {
  int i = m->pid;
  vertexdata& vd = vdata[i];

  vd.cp = colorpair(rand() & 0xFFFFFFFF);
  
  for(int i=0; i<size(vd.edges); i++) 
      vd.edges[i].second->orig = NULL;
  
  /* if(ealpha == 1) ealpha = 8;
  else if(ealpha == 8) ealpha = 32;
  else if(ealpha == 32) ealpha = 255;
  else ealpha = 1; */
  }

void storevertex(vector<GLfloat>& tab, const hyperpoint& h) {
  for(int i=0; i<3; i++) tab.push_back(h[i]);
  }

double linequality = .1;

void storelineto(vector<GLfloat>& tab, const hyperpoint& h1, const hyperpoint& h2) {
  if(intval(h1, h2) < linequality)
    storevertex(tab, h2);
  else {
    hyperpoint h3 = mid(h1, h2);
    storelineto(tab, h1, h3);
    storelineto(tab, h3, h2);
    }
  }

void storeline(vector<GLfloat>& tab, const hyperpoint& h1, const hyperpoint& h2) {
  storevertex(tab, h1);
  storelineto(tab, h1, h2);
  }

void queuedisk(const transmatrix& V, const colorpair& cp, bool legend) {
  if(legend && (int) cp.color1 == (int) 0x000000FF && backcolor == 0)
    poly_outline = 0x606060FF;
  else
    poly_outline = 0x000000FF;
  
  if(rog3) {
    int p = poly_outline; poly_outline = OUTLINE_TRANS; 
    queuepolyat(V, shDisk, 0x80, PPR_MONSTER_SHADOW); 
    poly_outline = p; 
    queuepolyat(mscale(V, geom3::BODY), shDisk, cp.color1, PPR_MONSTER_HEAD);
    }
  else {
    queuepoly(V, shDisk, cp.color1);
    }
  if(cp.shade == 't') queuepoly(V, shDiskT, cp.color2);
  if(cp.shade == 's') queuepoly(V, shDiskS, cp.color2);
  if(cp.shade == 'q') queuepoly(V, shDiskSq, cp.color2);
  if(cp.shade == 'm') queuepoly(V, shDiskM, cp.color2);
  }

ld ggamma = .5;

void drawVertex(const transmatrix &V, cell *c, shmup::monster *m) {
  if(m->dead) return;
  int i = m->pid;
  vertexdata& vd = vdata[i];
  
  bool ghilite = false;
  
  if(vd.special && specialmark) ghilite = true;
  
  if(!gmatrix.count(m->base)) printf("base not in gmatrix\n");

  int lid = shmup::lmousetarget ? shmup::lmousetarget->pid : -2;
  
  if(!leftclick) for(int j=0; j<size(vd.edges); j++) {
    edgeinfo *ei = vd.edges[j].second;
    if(!ei->visible) continue;
    vertexdata& vd1 = vdata[ei->i];
    vertexdata& vd2 = vdata[ei->j];

    int oi = ei->i, oj = ei->j;
    bool hilite = false;
    if(vdata[oi].special && vdata[oj].special && specialmark) hilite = true;
    else if(svg::in || inHighQual) hilite = false;
    else if(vd1.m == shmup::lmousetarget) hilite = true;
    else if(vd2.m == shmup::lmousetarget) hilite = true;
    else if(oi == lid || oj == lid) hilite = true;

    if(hilite) ghilite = true;

    if(ei->lastdraw < frameid) { 
      ei->lastdraw = frameid;
      
      int xlalpha = (hilite || hiliteclick) ? 64 : 20;
      
      if(kind == kSAG) {
        if(ei->weight2 > maxweight) maxweight = ei->weight2;
        xlalpha = int(pow(ei->weight2/ maxweight, ggamma) * 255);
        }
      else xlalpha = int(pow(ld(.5), ggamma) * 255);
      
      if(svg::in && xlalpha < 16) continue;

      transmatrix& gm1 = shmup::ggmatrix(vd1.m->base);
      transmatrix& gm2 = shmup::ggmatrix(vd2.m->base);
                
      hyperpoint h1 = gm1 * vd1.m->at * C0;
      hyperpoint h2 = gm2 * vd2.m->at * C0;
      
      /* if(hdist0(h1) < .001 || hdist0(h2) < .001) {
        printf("h1 = %s\n", display(h1));
        printf("h2 = %s\n", display(h2));
        display(m->at);
        display(vd2.m->at);
        display(V);
        display(gmatrix[vd2.m->base]);
        display(shmup::calc_gmatrix(vd2.m->base));
        } */
      
      int col = 
        ((hilite ? 0xFF0000 : forecolor) << 8) + xlalpha;

      bool onspiral = kind == kSpiral && abs(ei->i - ei->j) == 1;      
      if(pmodel || onspiral) {
        if(onspiral) {
          const int prec = 20; 
          transmatrix T = shmup::ggmatrix(currentmap->gamestart());
          hyperpoint l1 = T*tC0(spiral::at(1+ei->i));
          for(int z=1; z<=prec; z++) {
            hyperpoint l2 = T*tC0(spiral::at(1+ei->i+(ei->j-ei->i) * z / (prec+.0)));
            queueline(l1, l2, col, 0);
            l1 = l2;
            lastptd().prio = PPR_STRUCT0;
            }
          }
        else {
          queueline(h1, h2, col, 2);
          lastptd().prio = PPR_STRUCT0;
          }
        }
      else {
      
        if(ei->orig && ei->orig->cpdist >= 3) ei->orig = NULL;
        if(!ei->orig) {
          ei->orig = euclid ? cwt.c : viewctr.h->c7; // cwt.c;
          ei->prec.clear();
          
          transmatrix T = inverse(shmup::ggmatrix(ei->orig));

          if(kind == kSpiral && abs(ei->i - ei->j) == 1) {
            ei->orig = currentmap->gamestart();
            hyperpoint l1 = tC0(spiral::at(1+ei->i));
            storevertex(ei->prec, l1);
            const int prec = 20; 
            for(int z=1; z<=prec; z++) {
              hyperpoint l2 = tC0(spiral::at(1+ei->i+(ei->j-ei->i) * z / (prec+.0)));
              storeline(ei->prec, l1, l2);
              l1 = l2;
              }
            }
          else 
            storeline(ei->prec, T*h1, T*h2);
          }
        queuetable(shmup::ggmatrix(ei->orig), &ei->prec[0], size(ei->prec)/3, col, 0,
          PPR_STRUCT0);
        }
      }
/*
    */
    }

  if(!vd.virt) {
    queuedisk(V * m->at, ghilite ? colorpair(0xFF0000FF) : vd.cp, false);
    lastptd().info = vd.info;
    }
  
  
  if(showlabels) {
    bool doshow = true;
    if(kind == kTree && i > 0 && !vd.virt) {
      vertexdata& vdp = vdata[vd.data];
      hyperpoint h2 = shmup::ggmatrix(vdp.m->base) * vdp.m->at * C0;
      if(hdist(h2, V * m->at * C0) < 0.1) doshow = false;
      }
    
    hyperpoint h = tC0(V * m->at);
    transmatrix V2 = rgpushxto0(h) * ypush(purehepta ? .3 : .2);
    if(doshow) queuestr(V2, (svg::in ? .28 : .2) * crossf / hcrossf, vd.name, backcolor ? 0x000000 : 0xFFFF00, svg::in ? 0 : 1);
    lastptd().info = vd.info;
    }

  if(kind == kCollatz) {
    if(c->cpdist > 7 && euclid) ;
    else if(vd.data == 2) {
      // doubler vertex
      string s = vd.name;
      colorpair cp = vd.cp;
      vd.data = 20;
      int i0 = size(vdata);
      vdata.resize(i0+1);
      vertexdata& vdn = vdata[i0];
      createViz(i0, m->base, m->at * spin(collatz::s2) * xpush(collatz::p2));
      
      virtualRebase(vdn.m, true);
      vdn.cp = perturb(cp);
      vdn.data = 0;
      addedge(i, i0, 0, false);
      vdn.m->store();
      int carry = 0;
      string s2 = s;
      for(int i=size(s2)-1; i>=0; i--) {
        int x = 2*(s2[i] - '0') + carry;
        carry = x>=10;
        if(carry) x-=10;
        s2[i] = '0'+x;
        }
      if(carry) s2 = "1" + s2;
      vdn.name = s2;
      
      int m3 = 0;
      for(int i=0; i<size(s); i++) m3 += s[i] - '0';
      
      if(m3 % 3 == 2 && s != "2" && s != "1") {
        vdata.resize(i0+2);
        vertexdata& vdn = vdata[i0+1];
        createViz(i0+1, m->base, m->at * spin(collatz::s3) * xpush(collatz::p3));          
        virtualRebase(vdn.m, true);
        vdn.cp = perturb(cp);
        vdn.data = 0;
        addedge(i, i0+1, 0, false);
        vdn.m->store();
        int carry = -1;
        string s2 = s;
        for(int i=size(s2)-1; i>=0; i--) {
          carry += 2 * (s2[i] - '0');
          int ncarry = 0;
          while(carry % 3) carry += 10, ncarry--;
          if(carry >= 30) carry -= 30, ncarry += 3;
          s2[i] = '0'+carry/3;
          carry = ncarry;
          }
        if(s2[0] == '0') s2 = s2.substr(1);
        vdn.name = s2;
        vdn.cp = perturb(vdn.cp);
        }
      }
    else if(vd.data < 2) {
      vd.data++;
      fixmatrix(vd.m->at);
      }
    }

  }

bool virt(shmup::monster *m) {
  if(m->type != moRogueviz) return false;
  vertexdata& vd = vdata[m->pid];
  return vd.virt;
  }

vector<int> legend;

vector<cell*> named;

void drawExtra() {
  
  if(kind == kFullNet) {
    for(map<cell*, transmatrix>::iterator it = gmatrix.begin(); it != gmatrix.end(); it++) {
      cell *c = it->first;
      c->wall = waChasm;
      }

    for(map<cell*, transmatrix>::iterator it = gmatrix.begin(); it != gmatrix.end(); it++) {
      cell *c = it->first;
      bool draw = true;
      for(int i=0; i<size(named); i++) if(named[i] == c) draw = false;
      if(draw && gmatrix.count(c))
        queuepolyat(it->second, shDisk, dftcolor, PPR_LINE);
      }
    
    for(int i=0; i<size(named); i++) if(gmatrix.count(named[i])) {
      string s = ""; s += 'A'+i;
      queuestr(gmatrix[named[i]], 1, s, forecolor, 1);
      }
    
    canmove = true; items[itOrbAether] = true;
    }
  
#ifndef NORUG
  if(!rug::rugged) 
#endif
  for(int i=0; i<size(legend); i++) {
    int k = legend[i];
    vertexdata& vd = vdata[k];
    
    int rad = vid.radius/10;
    
    ld x = vid.xres - rad;
    ld y = (vid.radius * (i+.5)) / size(legend) * 2 - vid.radius + vid.yres/2;

    transmatrix V = atscreenpos(x, y, vid.radius/4);
    
    poly_outline = OUTLINE_NONE;
    queuedisk(V, vd.cp, true);
    poly_outline = OUTLINE_DEFAULT;
    queuestr(int(x-rad), int(y), 0, rad*(svg::in?5:3)/4, vd.name, forecolor, 0, 16);
    }
  }

void readcolor(const char *cfname) {
  FILE *f = fopen(cfname, "rt");
  if(!f) { printf("color file missing\n"); exit(1); }
  while(true) {
    string lab = "";
    while(true) {
      int c = fgetc(f);
      if(c == EOF) { fclose(f); return; }
      else if(c == 10 || c == 13 || c == 32 || c == 9) ;
      else if(c == ',' || c == ';') break;
      else lab += c;
      }
    
    colorpair x;
    int c2 = fgetc(f);
    if(c2 == '=') {
      string lab2 = "";
      while(true) {
        int c = fgetc(f);
        if(c == 10 || c == 13 || c == 32 || c == 9 || c == ',' || c == ';' || c == EOF) break;
        else lab2 += c;
        }
      x = vdata[getid(lab2)].cp;
      }
    else if(c2 == '@') {
      legend.push_back(getid(lab));
      continue;
      }
    else if(c2 == '/') {
      char buf[600];
      int err = fscanf(f, "%500s", buf);
      if(err > 0) 
        vdata[getid(lab)].info = new string(buf); // replace with std::shared_ptr in C++111
      continue;
      }
    else {
      ungetc(c2, f);
      char buf[60];
      int err = fscanf(f, "%30s", buf);
      if(err > 0) x = parse(buf);
      }
    
    if(size(lab) && lab[0] == '*') {
      lab = lab.substr(1);
      for(int i=0; i<size(vdata); i++)
        if(vdata[i].name.find(lab) != string::npos) {
          vdata[i].cp = x;
          }
      }
    else {
      int i = getid(lab);
      again: vdata[i].cp = x;
      
      if(kind == kTree) {
        i = vdata[i].data;
        if(i >= 0) goto again;
        }
      }
    }
  }

void init() {
  if(on) return;
  autocheat = true; 
#ifndef WEB
  mapeditor::drawplayer = false;
  firstland = euclidland = laCanvas;
  if(!shmup::on) restartGame('s');
  else restartGame();
#else
  firstland = euclidland = laCanvas;
  restartGame();
#endif
  on = true;
  items[itOrbLife] = 0;
  timerghost = false;

  gmatrix.clear();
  drawthemap();
  gmatrix0 = gmatrix;
  addMessage("RogueViz enabled");
  }

void close() { 
  for(int i=0; i<size(vdata); i++)
    if(vdata[i].m) vdata[i].m->dead = true;
  vdata.clear();
  labeler.clear();
  legend.clear();
  for(int i=0; i<size(edgeinfos); i++) delete edgeinfos[i];
  edgeinfos.clear();
  anygraph::coords.clear();
  sag::sagedges.clear();
  on = false;
  }

void turn(int delta) {
  if(!on) return;
  if(kind == kSAG) sag::iterate();
  if(kind == kKohonen) kohonen::steps();
  // shmup::pc[0]->rebase();
  }

void fixparam() {
  if((svg::in || inHighQual) && size(legend) && pngformat == 0) vid.xres = vid.xres * 22/16;
  if(size(legend)) vid.xcenter = vid.ycenter;
  }

#ifndef NOSDL
void rvvideo(const char *fname) {
  if(kind == kCollatz) {
    pngformat = 2;
    sightrange = 12;
    overgenerate = true;
    dronemode = true; vid.camera_angle = -45; rog3 = true; mapeditor::whichShape = '8';
    vid.aurastr = 512;
    long long reached = 763ll;
    while(reached < (1ll<<60)) {
      if(reached%3 == 2 && (2*reached-1) % 9 && hrand(100) < 50)
        reached = (2*reached-1) / 3;
      else reached *= 2;
      }
    printf("reached = %Ld\n", reached);
    vector<string> seq;
    while(reached>1) { 
      seq.push_back(llts(reached));
      if(reached&1) reached += (reached>>1)+1;
      else reached >>= 1;
      }
    // seq.push_back("1");
    reverse(seq.begin(), seq.end());
    
    int id = 0;
    int next = 0;
    
    int steps = 0;
    while(true) {
      steps++;
      if(isnan(View[0][0])) exit(1);
      shmup::turn(100);
      drawthemap();
      centerpc(100); optimizeview();
      fixmatrix(View);
      bfs(); setdist(cwt.c, 0, NULL);
      vertexdata& vd = vdata[id];
      for(int e=0; e<size(vd.edges); e++) {
        int id2 = vd.edges[e].first;
        if(vdata[id2].name == seq[next]) {
          id = id2; next++;
          cwt.c = shmup::pc[0]->base = vdata[id2].m->base;
          if(next == size(seq)) goto found;
          }
        }
      }
    
    found:
    printf("steps = %d\n", steps);
    conformal::create(), conformal::rotation = 1;
    // pmodel = mdBand;

#define STORYCOUNT 24
#define T(m,ss) (60*24*(m)+24*(ss))
#define FRAMECOUNT T(4,55)

printf("framecount = %d\n", FRAMECOUNT);

struct storydata { int s; int e; const char *text; } story[] = {
  {T(0,14), T(0,17), "I am flying above a tree of numbers."},
  {T(0,17), T(0,20), "It starts with the number 1."},
  {T(0,20), T(0,23), "Each number n branches left to 2n."},
  {T(0,23), T(0,28), "And it branches right to (2n-1)/3 if possible."},

  {T(1, 8), T(1,11), "What I am flying above is not a plane."},
  {T(1,11), T(1,14), "It is not a sphere either."},
  {T(1,14), T(1,17), "To be honest, the space I live in..."},
  {T(1,17), T(1,20), "...is not even Euclidean."},
  
  {T(2,12), T(2,15), "Look, angles of a triangle add up to..."},
  {T(2,15), T(2,18), "...less than 180 degrees in this world."},
  {T(2,18), T(2,21), "6/7 of 180 degrees, to be exact."},
  {T(2,21), T(2,24), "Do you see the regular heptagons?"},
  {T(2,36), T(2,42), "And all these lines are straight."},
  
  {T(3, 8), T(3,11), "Lots of space in my world."},
  {T(3,11), T(3,14), "In 105 steps from the root..."},
  {T(3,14), T(3,17), "...there are trillions of numbers."},
  {T(3,17), T(3,20), "That would not fit in your world."},

  {T(4,0),  T(4,3),  "Is every positive number somewhere in the tree?"},
  {T(4,3),  T(4,6),  "Your mathematicians do not know this yet."},
  {T(4,6),  T(4,10), "Will you find the answer?"},
  
  {T(4,44), T(4,54), "music: Ambient Flow, by Indjenuity"},
  
  {T(2,6),  T(2,27), "@triangles"},
  {T(2,27), T(2,42), "@network"},
  
  {0, T(0,7), "@fi"},
  {T(4,48), T(4,55), "@fo"},
  
  {0,0,NULL}
  };

    int drawtris=0, drawnet=0;
        
    for(int i=0; i<FRAMECOUNT; i++) {
      const char *caption = NULL;
      int fade = 255;
      
      bool dt = false, dn = false;
      
      for(int j=0; story[j].text; j++) if(i >= story[j].s && i <= story[j].e) {
        if(story[j].text[0] != '@')
          caption = story[j].text;
        else if(story[j].text[1] == 't')
          dt = true;
        else if(story[j].text[1] == 'n')
          dn = true;
        else if(story[j].text[2] == 'i')
          fade = 255 * (i - story[j].s) / (story[j].e-story[j].s);
        else if(story[j].text[2] == 'o')
          fade = 255 * (story[j].e - i) / (story[j].e-story[j].s);
        }
      
      if(dt && drawtris < 255) drawtris++;
      else if(drawtris && !dt) drawtris--;

      linepatterns::setColor(linepatterns::patZebraTriangles, 0x40FF4000 + drawtris);
      
      if(dn && drawnet < 255) drawnet++;
      else if(drawnet && !dn) drawnet--;

      linepatterns::setColor(linepatterns::patZebraLines, 0xFF000000 + drawnet);
      
      vid.grid = drawnet;
      
      conformal::phase = 1 + (size(conformal::v)-3) * i * .95 / FRAMECOUNT;
      conformal::movetophase();

      char buf[500];
      snprintf(buf, 500, fname, i);
      
      if(i == 0) drawthemap();
      shmup::turn(100);
      printf("%s\n", buf);
      pngres = 1080;
      saveHighQualityShot(buf, caption, fade);
      }
  
    return;
    }
  for(int i=0; i<1800; i++) {
    char buf[500];
    snprintf(buf, 500, fname, i);
    shmup::pc[0]->base = currentmap->gamestart();
    shmup::pc[0]->at = spin(i * 2 * M_PI / (58*30.)) * xpush(1.7);
    if(i == 0) drawthemap();
    shmup::turn(100);
    if(i == 0) drawthemap();
    centerpc(100);
    printf("%s\n", buf);
    saveHighQualityShot(buf);
    }
  }
#endif

int readArgs() {
  using namespace arg;
           
// options before reading
  if(0) ;
  else if(argis("-dftcolor")) {
    shift(); dftcolor = strtol(args(), NULL, 16);
    }  

// tree visualizer (e.g. Tree of Life)
//-------------------------------------

  else if(argis("-tree")) {
    PHASE(3); shift(); tree::read(args());
    }

// SAG visualizer (e.g. Reddit roguelikes, GitHub languages)
//-----------------------------------------------------------

// (1) configure edge weights
  else if(argis("-edgepower")) {
    shift(); sag::edgepower = argf();
    shift(); sag::edgemul = argf();
    }
// (1) configure temperature (high, low)
  else if(argis("-sagtemp")) {
    shift(); sag::hightemp = argf();
    shift(); sag::lowtemp = argf();
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


// graph visualizer
//------------------

// this visualizes the data from: https://hpi.de/friedrich/research/hyperbolic

  else if(argis("-graph")) {
    PHASE(3); shift(); anygraph::read(args());
    }
  
// draw spirals 
//--------------

// example commandline:
// -spiral 2,10000 -spiraledge 0,2 -spiraledge 1,1 -lab -spiralcolor 2 FF4040FF

  else if(argis("-collatz")) {
    PHASE(3); 
    using namespace collatz; 
    shift(); sscanf(args(), "%lf,%lf,%lf,%lf", &s2, &p2, &s3, &p3);
    start();
    }

  else if(argis("-spiral")) {
    PHASE(3); 
    ld mul = 2;
    int N = 1000;
    shift(); sscanf(args(), LDF ",%d", &mul, &N);
    spiral::place(N, mul);
    }

  else if(argis("-nogui")) {
    noGUI = true;
    }

  else if(argis("-net")) {
    PHASE(3);
    init(); kind = kFullNet;
    linepatterns::setColor(linepatterns::patTriNet, 0x30);
    linepatterns::setColor(linepatterns::patTriRings, 0xFF);
    }

  else if(argis("-spiraledge")) {
    PHASE(3); 
    ld shft = 1;
    ld mul = 1;
    shift(); sscanf(args(), LDF "," LDF, &shft, &mul);
    spiral::edge(shft, mul);
    }

  else if(argis("-spiralcolor")) {
    PHASE(3); 
    ld period = 1;
    ld start = 1;
    shift(); sscanf(args(), LDF "," LDF, &period, &start);
    start--;
    shift();
    spiral::color(start, period, parse(args()));
    }

// graphical parameters
//------------------

  // read the color/legend file
  else if(argis("-color")) {
    PHASE(3); shift(); readcolor(args());
    }
  else if(argis("-lab")) {
    showlabels = true;
    }
  else if(argis("-rog3")) {
    rog3 = true;
    }
  else if(argis("-cshift")) {
    shift(); collatz::cshift = argf();
    }
  else if(argis("-rvwarp")) {
    mapeditor::whichShape = '8';
    }
  else if(argis("-lq")) {
    shift(); linequality = argf();
    }
  else if(argis("-ggamma")) {
    shift(); ggamma = argf();
    }
  else if(argis("-som")) {
    PHASE(3);
    shift(); const char *fname = args();
    shift(); int percount = argi();
    shift(); kohonen::run(fname, percount, argf());
    }
  else if(argis("-rvpres")) {
    tour::slides = rvtour::rvslides;
    }
  else if(argis("-somsave")) {
    PHASE(3);
    while(!kohonen::finished()) kohonen::step();
    shift(); kohonen::ksave(args());
    }
  else if(argis("-somload")) {
    PHASE(3);
    shift(); kohonen::kload(args());
    }
  else if(argis("-nolegend")) {
    legend.clear();
    }
  
// graphical output 
//------------------

  // shmup::turn might be necessary when saving screenshots
  else if(argis("-TURN")) {
    PHASE(3); shmup::turn(100);
    }
#ifndef NOSDL
  else if(argis("-video")) {
    shift(); rvvideo(args());
    }
#endif
  else return 1;
  return 0;
  }

void showMenu() {
  dialog::init(XLAT("rogueviz configuration"));

  dialog::addSelItem(XLAT("temperature"), fts(sag::temperature), 't');
  if(kind == kSAG)
    dialog::addSelItem(XLAT("SAG mode"), sag::sagmodes[sag::sagmode], 'm');
  dialog::addBoolItem(XLAT("show labels"), showlabels, 'l');
  dialog::addBoolItem(XLAT("mark special vertices"), specialmark, 'x');
  dialog::addSelItem(XLAT("background color"), itsh(backcolor), 'b');
  dialog::addSelItem(XLAT("gamma value for edges"), fts(ggamma), 'g');
  dialog::addBoolItem(XLAT("vertices in 3D"), rog3, 'v');
  
  if(kind == kKohonen)
    kohonen::showMenu();

  dialog::addBreak(50);
  dialog::addItem(XLAT("exit menu"), 'v');

  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    if(uni == 't')
      dialog::editNumber(sag::temperature, sag::lowtemp, sag::hightemp, 1, 0, XLAT("temperature"), "");
    else if(uni == 'm') {
      sag::sagmode = sag::eSagmode( (1+sag::sagmode) % 3 );
      }
    else if(uni == 'l') showlabels = !showlabels;
    else if(uni == 'v') rog3 = !rog3;
    else if(uni == 'x') specialmark = !specialmark;
    else if(uni == 'b') backcolor ^= 0xFFFFFF, bordcolor ^= 0xFFFFFF, forecolor ^= 0xFFFFFF;
    else if(uni == 'g') {
      dialog::editNumber(ggamma, 0, 5, .01, 0.5, XLAT("gamma value for edges"), "");
      dialog::sidedialog = true;
      }
    else if(uni == 'z') {
      for(int i=0; i<size(named)-1; i++) if(named[i] == cwt.c)
        swap(named[i], named[i+1]);
      if(!size(named) || named[size(named)-1] != cwt.c) named.push_back(cwt.c);
      printf("named = %d\n", size(named));
      popScreen();
      }
    else if(kind == kKohonen && kohonen::handleMenu(sym, uni)) ;
    else if(doexiton(sym, uni)) popScreen();
    };
  }

void processKey(int sym, int uni) { }

string makehelp() {
  string ret = 
    "This is RogueViz, a visualization engine based on HyperRogue.\n\nUse WASD to move, v for menu.\n\n"
    "Read more about RogueViz on : http://roguetemple.com/z/hyper/rogueviz.php\n\n";
  if(kind == kAnyGraph)
    ret += "Current visualization: any graph\n\n" + fname;
  if(kind == kTree)
    ret += "Current visualization: tree\n\n" + fname;
  if(kind == kSpiral)
    ret += "Current visualization: spiral\n\n";
  if(kind == kSAG)
    ret += "Current visualization: SAG\n\n" + fname;
  if(kind == kCollatz)
    ret += "Current visualization: Collatz conjecture\n\n";
  if(kind == kFullNet)
    ret += "Current visualization: full net\n\n";
  return ret;
  }

namespace rvtour {

using namespace tour;

string cname() {
  if(euclid) return "coord-6.txt";
  if(purehepta) return "coord-7.txt";
  return "coord-67.txt";
  }

template<class T> function<void(presmode)> roguevizslide(char c, T t) {
  return [c,t] (presmode mode) {
    mapeditor::canvasback = 0x101010;
    setCanvas(mode, c);
    if(mode == 1 || mode == pmGeometryStart) t();
  
    if(mode == 3 || mode == pmGeometry || mode == pmGeometryReset) {
      rogueviz::close();
      shmup::clearMonsters();
      if(mode == pmGeometryReset) t();
      }
  
    slidecommand = "toggle the player";
    if(mode == 4) 
      mapeditor::drawplayer = !mapeditor::drawplayer;
    };
  }

template<class T, class T1> function<void(presmode)> roguevizslide_action(char c, T t, T1 act) {
  return [c,t,act] (presmode mode) {
    mapeditor::canvasback = 0x101010;
    setCanvas(mode, c);
    if(mode == pmStart || mode == pmGeometryStart) t();
  
    act(mode);

    if(mode == pmStop || mode == pmGeometry || mode == pmGeometryReset) {
      rogueviz::close();
      shmup::clearMonsters();
      if(mode == pmGeometryReset) t();
      }
  
    };
  }

#define RVPATH HYPERPATH "rogueviz/"

slide rvslides[] = {
    {"RogueViz", 999, LEGAL_ANY, 
      "This is a presentation of RogueViz, which "
      "is an adaptation of HyperRogue as a visualization tool "
      "rather than a game. Hyperbolic space is great "
      "for visualizing some kinds of data because of the vast amount "
      "of space.\n\n"
      "Press '5' to switch to the standard HyperRogue tutorial. "
      "Press ESC to look at other functions of this presentation."
      ,
      [] (presmode mode) {
        slidecommand = "the standard presentation";
        if(mode == pmStartAll) firstland = euclidland = laPalace;
        if(mode == 4) {
          tour::slides = default_slides;
          while(tour::on) restartGame('T', false);
          firstland = euclidland = laIce;
          tour::start();
          }
        }
      },
    {"straight lines in the Palace", 999, LEGAL_ANY, 
      "One simple slide about HyperRogue. Press '5' to show some hyperbolic straight lines.",
      [] (presmode mode) {
       using namespace linepatterns;
       slidecommand = "toggle the Palace lines";
       if(mode == 4)
         switchAlpha(patPalace, 0xFF);
       if(mode == 3)
         setColor(patPalace, 0xFFD50000);
        }
      },
  {"Collatz conjecture", 51, LEGAL_UNLIMITED,
    "The following slide is a visualization of the Collatz conjecture. "
    "Press '5' for a spiral rendering of the Collatz conjecture visualization.\n\n"
    "Note that this, and many other RogueViz visualizations, have "
    "Euclidean versions (press ESC).\n",
    roguevizslide('d', [] () {
      rogueviz::dftcolor = 0x206020FF;
      
      int fac = euclid ? 2 : 1;

      rogueviz::collatz::s2 = .3;
      rogueviz::collatz::p2 = .5 * fac;
      rogueviz::collatz::s3 = -.4;
      rogueviz::collatz::p3 = .4 * fac;

      rogueviz::showlabels = true;
      
      rogueviz::on = true;
      gmatrix.clear();
      drawthemap();
      gmatrix0 = gmatrix;

      rogueviz::collatz::start();
      })
    },

  {"Roguelikes", 63, LEGAL_UNLIMITED,
    "A visualization of roguelikes, based on disccusion on /r/reddit. "
    "See: http://www.roguetemple.com/z/hyper/reddit.php",
    roguevizslide('0', [] () {
      rogueviz::dftcolor = 0x282828FF;

      rogueviz::showlabels = true;
      rogueviz::ggamma = .5;
      rogueviz::sag::edgepower = 1;
      rogueviz::sag::edgemul = 1;
      
      rogueviz::on = true;
      gmatrix.clear();
      drawthemap();
      gmatrix0 = gmatrix;

      rogueviz::sag::read(RVPATH "roguelikes/edges.csv");
      rogueviz::readcolor(RVPATH "roguelikes/color.csv");
      rogueviz::sag::loadsnake(RVPATH "roguelikes/" + cname());
      })    
    },
  {"Programming languages of GitHub", 64, LEGAL_UNLIMITED,
    "A visualization of programming languages.",
    roguevizslide('0', [] () {
      rogueviz::dftcolor = 0x282828FF;

      rogueviz::showlabels = true;
      rogueviz::ggamma = 1;
      rogueviz::sag::edgepower = .4;
      rogueviz::sag::edgemul = .02;
      
      rogueviz::on = true;
      gmatrix.clear();
      drawthemap();
      gmatrix0 = gmatrix;

      rogueviz::sag::read(RVPATH "lang/edges.csv");
      rogueviz::readcolor(RVPATH "lang/color.csv");
      rogueviz::sag::loadsnake(RVPATH "lang/" + cname());
      if(euclid) rogueviz::legend.clear();
      })
    },
    {"Boardgames", 62, LEGAL_UNLIMITED,
        "A visualization of board games, based on discussions on Reddit.",
    roguevizslide('0', [] () {
      rogueviz::dftcolor = 0x282828FF;

      rogueviz::showlabels = true;
      rogueviz::ggamma = .7;
      rogueviz::sag::edgepower = 1;
      rogueviz::sag::edgemul = 1;
      
      rogueviz::on = true;
      gmatrix.clear();
      drawthemap();
      gmatrix0 = gmatrix;

      rogueviz::sag::read(RVPATH "boardgames/edges.csv");
      rogueviz::readcolor(RVPATH "boardgames/color.csv");
      rogueviz::sag::loadsnake(RVPATH "boardgames/" + cname());
      })
        },
    {"Tree of Life", 61, LEGAL_UNLIMITED,
      "Not described.",

    roguevizslide('0', [] () {

      rogueviz::dftcolor = 0x206020FF;

      rogueviz::showlabels = true;
      
      rogueviz::on = true;
      gmatrix.clear();
      drawthemap();
      gmatrix0 = gmatrix;

      rogueviz::tree::read(RVPATH "treeoflife/tol.txt");
      })},
  {"THE END", 99, LEGAL_ANY | FINALSLIDE,
    "Press '5' to leave the presentation.",
    [] (presmode mode) {
      firstland = euclidland = laIce;
      if(mode == 4) restartGame('T');
      }
    }
  };

}

auto hooks  = 
  addHook(hooks_frame, 0, drawExtra) +
  addHook(hooks_args, 100, readArgs) +
  addHook(clearmemory, 0, clear) +
  addHook(hooks_config, 0, [] () { ss::list(rogueviz::rvtour::rvslides); });


};
