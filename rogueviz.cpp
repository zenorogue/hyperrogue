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

const char *fname;
const char *cfname;

enum eVizkind { kNONE, kAnyGraph, kTree, kSpiral, kSAG, kCollatz };
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

void addedge(int i, int j, edgeinfo *ei) {
  double d = hdist(vdata[i].m->at * C0, vdata[j].m->at * C0);
  if(d >= 4) {
    // printf("splitting %lf\n", d);
    hyperpoint h = mid(vdata[i].m->at * C0, vdata[j].m->at * C0);
    int id = size(vdata);
    vdata.resize(id+1);
    vertexdata& vd(vdata[id]);
    vd.cp = colorpair(0x400000FF);
    vd.virt = ei;
    
    createViz(id, currentmap->gamestart(), rgpushxto0(h));
    
    addedge(i, id, ei);
    addedge(id, j, ei);
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

  void place(int N, ld mul) {
    init(); kind = kSpiral;
    vdata.resize(N);
  
    for(int i=0; i<N; i++) {
      vertexdata& vd = vdata[i];
      
      double d = i + 1;
      
      transmatrix h = spin(log(d) * 2 * M_PI / log(mul)) * xpush(log(d));
  
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
  
  void start() {
    init(); kind = kCollatz;
    vdata.resize(1);
    vertexdata& vd = vdata[0];    
    createViz(0, cwt.c, Id);    
    virtualRebase(vd.m, true);
    vd.cp = dftcolor;
    vd.data = 0;
    addedge(0, 0, 0, false);
    vd.name = "1";    
    storeall();
    }  
  }

int readLabel(FILE *f) {
  char xlabel[10000];
  if(fscanf(f, "%9500s", xlabel) <= 0) return -1;
  return getid(xlabel);
  }

namespace anygraph {
  double R, alpha, T;
  vector<pair<double, double> > coords;

  void read(string fn, bool subdiv = true) {
    init(); kind = kAnyGraph;
    FILE *f = fopen((fn + "-coordinates.txt").c_str(), "rt");
    if(!f) {
      printf("Missing file: %s-coordinates.txt\n", fname);
      exit(1);
      }
    printf("Reading coordinates...\n");
    char buf[100];  
    int N;
    int err;
    err = fscanf(f, "%s%s%s%s%d%lf%lf%lf", buf, buf, buf, buf, &N, 
      &anygraph::R, &anygraph::alpha, &anygraph::T);
    if(err < 8) { printf("Error: incorrect format of the first line\n"); exit(1); }
    vdata.reserve(N);
    while(true) {
      int id = readLabel(f);
      if(id < 0) break;
      vertexdata& vd(vdata[id]);
      vd.name = its(id);
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
      printf("Missing file: %s-links.txt\n", fname);
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
  
    printf("Rebasing...\n");
    for(int i=0; i<size(vdata); i++) {
      if(i % 10000 == 0) printf("%d/%d\n", i, size(vdata));
      if(vdata[i].m) virtualRebase(vdata[i].m, true);
      }
    printf("Done.\n");
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
    
  void read(const char *fname) {
    init(); kind = kTree;
    printf("Reading the tree of life...\n");
    FILE *f = fopen(fname, "rt");
    if(!f) { printf("Failed to open tree file: %s\n", fname); exit(1); }
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

namespace sag {

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
  
  void loadsnake(const char *fname) {
    printf("Loading the sag from: %s\n", fname);
    FILE *sf = fopen(fname, "rt");
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
  
  void dofullsa(int satime) {
    sagmode = sagSA;
    enable_snake();
    int t1 = SDL_GetTicks();
    
    while(true) {
      int t2 = SDL_GetTicks();
      double d = (t2-t1) / (1000. * satime);
      if(d > 1) break;
      temperature = 10 - (d*25);
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

  void read(const char *fname) {
    init(); kind = kSAG;
    temperature = 0; sagmode = sagOff;
    readsag(fname);

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

      ei.weight2 = pow(ei.weight, edgepower) * edgemul;
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
    
  help = "Edges: ";

  if(vd.info) help = (*vd.info) + "\n" + help;

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
  queuepoly(V, shDisk, cp.color1);
  if(cp.shade == 't') queuepoly(V, shDiskT, cp.color2);
  if(cp.shade == 's') queuepoly(V, shDiskS, cp.color2);
  if(cp.shade == 'q') queuepoly(V, shDiskSq, cp.color2);
  if(cp.shade == 'm') queuepoly(V, shDiskM, cp.color2);
  }

ld maxweight;

ld ggamma = .5;

void drawVertex(const transmatrix &V, cell *c, shmup::monster *m) {
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
      else xlalpha = int(pow(.5, ggamma) * 255);
      
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
        ((hilite ? 0xFF0000 : backcolor ? 0x808080 : 0xFFFFFF) << 8) + xlalpha;
      
      if(pmodel) {
        queueline(h1, h2, col, 2);
        lastptd().prio = PPR_STRUCT0;
        }
      else {
      
        if(ei->orig && ei->orig->cpdist >= 3) ei->orig = NULL;
        if(!ei->orig) {
          ei->orig = cwt.c;
          ei->prec.clear();
          transmatrix T = inverse(shmup::ggmatrix(ei->orig));
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
    if(kind == kTree && i > 0) {
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
    if(vd.data == 2) {
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

void drawExtra() {
  for(int i=0; i<size(legend); i++) {
    int k = legend[i];
    vertexdata& vd = vdata[k];
    
    int rad = vid.radius/10;
    
    ld x = vid.xres - rad;
    ld y = (vid.radius * (i+.5)) / size(legend) * 2 - vid.radius + vid.yres/2;

    transmatrix V = Id;
    V[0][2] += (x - vid.xcenter) / vid.radius * (1+vid.alphax);
    V[1][2] += (y - vid.ycenter) / vid.radius * (1+vid.alphax);
    
    poly_outline = OUTLINE_NONE;
    queuedisk(V, vd.cp, true);
    queuestr(int(x-rad), int(y), 0, rad*(svg::in?5:3)/4, vd.name, backcolor ? 0 : 0xFFFFFF, 0, 16);
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
        if(vdata[i].name.find(lab) != string::npos)
          vdata[i].cp = x;
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
  on = autocheat = true; 
#ifndef WEB
  mapeditor::drawplayer = false;
  firstland = euclidland = laCanvas;
  if(!shmup::on) restartGame('s');
#else
  firstland = euclidland = laCanvas;
  restartGame();
#endif
  items[itOrbLife] = 0;
  timerghost = false;

  gmatrix.clear();
  drawthemap();
  gmatrix0 = gmatrix;
  addMessage("RogueViz enabled");
  }

void close() { 
  vdata.clear();
  labeler.clear();
  legend.clear();
  for(int i=0; i<size(edgeinfos); i++) delete edgeinfos[i];
  edgeinfos.clear();
  anygraph::coords.clear();
  on = false;
  }

void turn(int delta) {
  if(!on) return;
  if(kind == kSAG) sag::iterate();
  // shmup::pc[0]->rebase();
  }

void fixparam() {
  if((svg::in || inHighQual) && size(legend)) vid.xres = vid.xres * 22/16;
  while(vid.xres & 15) vid.xres++;
  if(size(legend)) vid.xcenter = vid.ycenter;
  }

void rvvideo(const char *fname) {
  for(int i=0; i<1050; i++) {
    char buf[500];
    snprintf(buf, 500, fname, i);
    shmup::pc[0]->base = currentmap->gamestart();
    shmup::pc[0]->at = spin(i * 2 * M_PI / 1000.) * xpush(1.7);
    if(i == 0) drawthemap();
    shmup::turn(100);
    if(i == 0) drawthemap();
    centerpc(100);
    printf("%s\n", buf);
    saveHighQualityShot(buf);
    }
  }

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
// (2) read the edge data
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
  else if(argis("-lq")) {
    shift(); linequality = argf();
    }
  else if(argis("-ggamma")) {
    shift(); ggamma = argf();
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
  else if(argis("-video")) {
    shift(); rvvideo(args());
    }
  else return 1;
  return 0;
  }

void showMenu() {
  dialog::init(XLAT("rogueviz configuration"));

  dialog::addSelItem(XLAT("temperature"), fts(sag::temperature), 't');
  dialog::addSelItem(XLAT("SAG mode"), sag::sagmodes[sag::sagmode], 'm');
  dialog::addBoolItem(XLAT("show labels"), showlabels, 'l');
  dialog::addBoolItem(XLAT("mark special vertices"), specialmark, 'x');
  dialog::addSelItem(XLAT("background color"), itsh(backcolor), 'b');
  dialog::addSelItem(XLAT("gamma value for edges"), fts(ggamma), 'g');

  dialog::addBreak(50);
  dialog::addItem(XLAT("exit menu"), 'v');

  dialog::display();
  }

void handleMenu(int sym, int uni) {
  if(uni == 't')
    dialog::editNumber(sag::temperature, -10, 15, 1, 0, XLAT("temperature"), "");
  else if(uni == 'm') {
    sag::sagmode = sag::eSagmode( (1+sag::sagmode) % 3 );
    }
  else if(uni == 'l') showlabels = !showlabels;
  else if(uni == 'x') specialmark = !specialmark;
  else if(uni == 'b') backcolor ^= 0xFFFFFF;
  else if(uni == 'g') {
    dialog::editNumber(ggamma, 0, 5, .01, 0.5, XLAT("gamma value for edges"), "");
    dialog::sidedialog = true;
    }
  else if(uni) cmode = emNormal;
  }

void processKey(int sym, int uni) { }

};
