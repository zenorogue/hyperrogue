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

#ifndef CAP_ROGUEVIZ
#define CU 1
#include "init.cpp"
#endif

#include "rogueviz.h"

namespace hr { extern hpcshape shEagle, shMiniGhost, shGhost, shShark, shAnimatedEagle[30], shAnimatedTinyEagle[30]; }

namespace rogueviz {

const transmatrix centralsym = {{{-1,0,0}, {0,-1,0}, {0,0,-1}}};

using namespace hr;

edgetype default_edgetype = { .1, DEFAULT_COLOR, "default" };

void init();

bool showlabels = false;
bool specialmark = false;
bool edge_legend = false;

bool rog3 = false;
int vertex_shape = 1;

string edgename;
string fname;

// const char *fname;
// const char *cfname;

eVizkind kind;

bool on;

vector<shared_ptr<edgetype>> edgetypes;

edgetype *add_edgetype(const string& name) {
  auto e = make_shared<edgetype> (default_edgetype);
  e->name = name;
  edgetypes.push_back(e);
  return &*e;
  }

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

vector<vertexdata> vdata;

transmatrix cspin(int i, int ch) {
  return spin(M_PI + (2 * M_PI * (i+1)) / (ch+1));
  }

map<string, int> labeler;

int getid(const string& s) {
  if(labeler.count(s)) return labeler[s];
  else {
    int id = isize(vdata);
    vdata.resize(isize(vdata) + 1);
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

hyperpoint where(int i, cell *base) {
  auto m = vdata[i].m;
  if(m->base == base) return tC0(m->at);
  else if(confusingGeometry()) {
    return calc_relative_matrix(m->base, base, C0) * tC0(m->at);
    }
  else {
    // notimpl(); // actually probably that's a buug
    return inverse(ggmatrix(currentmap->gamestart())) * (ggmatrix(m->base) * tC0(m->at));
    }
  }

void addedge(int i, int j, edgeinfo *ei) {
  cell *base = 
    confusingGeometry() ? vdata[i].m->base : currentmap->gamestart();
  hyperpoint hi = where(i, base);
  hyperpoint hj = where(j, base);
  double d = hdist(hi, hj);
  if(d >= 4) {
    // printf("splitting %lf\n", d);
    hyperpoint h = mid(hi, hj);
    int id = isize(vdata);
    vdata.resize(id+1);
    vertexdata& vd(vdata[id]);
    vd.cp = colorpair(0x400000FF);
    vd.virt = ei;
    
    createViz(id, base, rgpushxto0(h));
    vd.m->no_targetting = true;
    
    addedge(i, id, ei);
    addedge(id, j, ei);
    virtualRebase(vdata[i].m, true);
    }
  else addedge0(i, j, ei);
  }

vector<edgeinfo*> edgeinfos;

void addedge(int i, int j, double wei, bool subdiv, edgetype *t) {
  edgeinfo *ei = new edgeinfo(t);
  edgeinfos.push_back(ei);
  ei->i = i;
  ei->j = j;
  ei->weight = wei;
  if(subdiv) addedge(i, j, ei);
  else addedge0(i, j, ei);
  }

void storeall(int from) {
  for(int i=from; i<isize(vdata); i++)
    if(vdata[i].m)
      vdata[i].m->store();
  }

colorpair dftcolor = 0x282828FF;

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
  
      createViz(i, cwt.at, h);
      vd.name = its(i+1);
      virtualRebase(vd.m, true);
  
      vd.cp = dftcolor;
      }
  
    storeall();
    }
  
  void edge(ld shift, ld mul) {
    int N = isize(vdata);
    auto t = add_edgetype(fts(shift)+" " + fts(mul));
    t->visible_from = 1. / (N+.5);
    for(int i=0; i<N; i++) {
      int i0 = i+1;
      int j0 = int(i0 * mul + shift) - 1;
      if(j0 >= 0 && j0 < N) addedge(i, j0, 1/(i+1), false, t);
      }
    }
  
  void color(ld start, ld period, colorpair c) {
    int N = isize(vdata);
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
  
  transmatrix T2, T3;
  
  edgetype *collatz1, *collatz2;
  
  void start() {
    init(); kind = kCollatz;
    collatz1 = add_edgetype("1");
    collatz2 = add_edgetype("2");
    vdata.resize(1);
    vertexdata& vd = vdata[0];
    createViz(0, cwt.at, xpush(cshift));
    virtualRebase(vd.m, true);
    vd.cp = perturb(dftcolor);
    vd.data = 0;
    addedge(0, 0, 1, false, collatz::collatz1);
    vd.name = "1";
    storeall();

    T2 = spin(collatz::s2) * xpush(collatz::p2);
    T3 = spin(collatz::s3) * xpush(collatz::p3);
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
  
  edgetype *any;

  
  int N;
               
  void fixedges() {
    for(int i=N; i<isize(vdata); i++) if(vdata[i].m) vdata[i].m->dead = true;
    for(int i=0; i<isize(vdata); i++) vdata[i].edges.clear();
    vdata.resize(N);
    for(auto e: edgeinfos) {
      e->orig = NULL;
      addedge(e->i, e->j, e);
      }
    }

  void read(string fn, bool subdiv, bool doRebase, bool doStore) {
    init(); kind = kAnyGraph;
    any = add_edgetype("embedded edges");
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
      if(s == "" || s == "#ROGUEVIZ_ENDOFDATA") break;
      int id = getid(s);
      vertexdata& vd(vdata[id]);
      vd.name = s;
      vd.cp = colorpair(dftcolor);
      
      double r, alpha;
      int err = fscanf(f, "%lf%lf", &r, &alpha);
      coords.push_back(make_pair(r, alpha));
      if(err < 2) { printf("Error: incorrect format of r/alpha\n"); exit(1); }
  
      transmatrix h = spin(alpha * degree) * xpush(r);
      
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
      addedge(i, j, 1, subdiv, any);
      qlink++;
      }
    fclose(f);
  
    if(doRebase) {
      printf("Rebasing...\n");
      for(int i=0; i<isize(vdata); i++) {
        if(i % 10000 == 0) printf("%d/%d\n", i, isize(vdata));
        if(vdata[i].m) virtualRebase(vdata[i].m, true);
        }
      printf("Done.\n");
      }
    
    if(doStore) storeall();
    }
  
  }

namespace tree {

  edgetype *tree_edge;

  struct treevertex {
    int origid;
    int parent;
    int depth;
    int spos, epos;
    vector<int> children;
    };
  
  vector<treevertex> tol;
  
  void child(int pid, int id) {
    if(isize(tol) <= id) tol.resize(id+1);
    
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
    for(int i=0; i<isize(tol[at].children); i++)
      spos(tol[at].children[i], d+1);
    tol[at].epos = ++xpos;
    }
    
  void read(string fn) {
    fname = fn;
    init(); kind = kTree;
    tree_edge = add_edgetype("tree edge");
    printf("Reading the tree of life...\n");
    FILE *f = fopen(fname.c_str(), "rt");
    if(!f) { printf("Failed to open tree file: %s\n", fname.c_str()); exit(1); }
    if(fgetc(f) != '(') {
      printf("Error: bad format\n");
      exit(1);
      }
    readnode(f, -1);
    fclose(f);
    int N = isize(vdata);
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
      createViz(i, cwt.at, h);
      vd.cp = dftcolor; 
      
      if(tol[i].parent >= 0) 
        addedge(i, tol[i].parent, 1, true, tree_edge);
      }
    
    for(int i=0; i<isize(vdata); i++) {
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
    
    printf("loglikelihood = %lf\n", (double) loglik);
    }
  
  void readsag(const char *fname) {
    maxweight = 0;
    sag_edge = add_edgetype("SAG edge");
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
    init(); kind = kSAG;
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

  }

bool edgecmp(edgeinfo *e1, edgeinfo *e2) {
  return e1->weight > e2->weight;
  }

bool which_weight = false;

void rogueviz_help(int id, int pagenumber) {

  vertexdata& vd = vdata[id];
  int noedges = isize(vd.edges);
  help = helptitle(vd.name, vd.cp.color1 >> 8);

  if(vd.info) {
    #if CAP_URL
    help_extension hex;
    hex.key = 'L';
    hex.text = "open link";
    hex.subtext = *vd.info;
    hex.action = [&vd] () { open_url(*vd.info); };
    help_extensions.push_back(hex);
    #else
    help += "\n\nlink: " + *vd.info;
    #endif
    }
  
  vector<edgeinfo*> alledges;
  
  for(int j=0; j<isize(vd.edges); j++) 
    alledges.push_back(vd.edges[j].second);
  
  sort(alledges.begin(), alledges.end(), edgecmp);

  for(int i=0; i<10 && i+pagenumber < noedges; i++) {
    help_extension hex;
    hex.key = 'a' + i;

    edgeinfo *ei = alledges[pagenumber + i];
    if(ei->weight < ei->type->visible_from) continue;
    int k = ei->i ^ ei->j ^ id;
    hex.text = vdata[k].name;
    hex.color = vdata[k].cp.color1 >> 8;
    if(kind == kSAG) {
      if(which_weight)
        hex.subtext = fts(ei->weight2);
      else
        hex.subtext = fts(ei->weight);
      }

    hex.action = [k] () { help_extensions.clear(); rogueviz_help(k, 0); };
    help_extensions.push_back(hex);
    }

  if(noedges > pagenumber + 10) {
    help_extension hex;
    hex.key = 'z';
    hex.text = "next page";
    hex.subtext = its(pagenumber+10) + "/" + its(noedges) + " edges";
    hex.action = [id, pagenumber] () { help_extensions.clear(); rogueviz_help(id, pagenumber + 10); };
    help_extensions.push_back(hex);
    }
  
  if(kind == kSAG && noedges) {
    help_extension hex;
    hex.key = 'w';
    hex.text = "displayed weight";
    hex.subtext = which_weight ? "attraction force" : "weight from the data";
    hex.action = [id, pagenumber] () { which_weight = !which_weight; help_extensions.clear(); rogueviz_help(id, pagenumber); };
    help_extensions.push_back(hex);
    }
  }

bool describe_monster(shmup::monster *m, string& out) {

  if(m->type != moRogueviz) return false;
   
  int i = m->pid;
  vertexdata& vd = vdata[i];

  string o = vd.name + ", "+its(isize(vd.edges))+" edges";
  /* if(isize(vd.edges) < 10) {
    for(int i=0; i<isize(vd.edges); i++) 
      o += " " + its(snakedist(vd.snakeid, vd.edges[i]->snakeid));
    } */
  
  help = bygen([i] () { rogueviz_help(i, 0); });
  
  if(out == XLATN("Canvas")) out = o;
  else out = out + ", " + o;

  return true;
  }

bool activate(shmup::monster *m) {
  if(m->type != moRogueviz) return false;
  int i = m->pid;
  vertexdata& vd = vdata[i];

  vd.cp = colorpair(rand() & 0xFFFFFFFF);
  
  for(int i=0; i<isize(vd.edges); i++) 
      vd.edges[i].second->orig = NULL;
  
  return true;
  
  /* if(ealpha == 1) ealpha = 8;
  else if(ealpha == 8) ealpha = 32;
  else if(ealpha == 32) ealpha = 255;
  else ealpha = 1; */
  }

void storevertex(vector<glvertex>& tab, const hyperpoint& h) {
  tab.push_back(glhr::pointtogl(h));
  }

double linequality = .1;

void storelineto(vector<glvertex>& tab, const hyperpoint& h1, const hyperpoint& h2) {
  if(intval(h1, h2) < linequality)
    storevertex(tab, h2);
  else {
    hyperpoint h3 = mid(h1, h2);
    storelineto(tab, h1, h3);
    storelineto(tab, h3, h2);
    }
  }

void storeline(vector<glvertex>& tab, const hyperpoint& h1, const hyperpoint& h2) {
  storevertex(tab, h1);
  storelineto(tab, h1, h2);
  }

hpcshape *vshapes[4] = { &shDisk, &shDisk, &shHeptaMarker, &shSnowball };

color_t darken_a(color_t c) {
  for(int p=0; p<3; p++)
  for(int i=0; i<darken; i++) part(c, i+1) = (part(c, i+1) + part(backcolor, i)) >> 1;
  return c;
  }

void queuedisk(const transmatrix& V, const colorpair& cp, bool legend, const string* info, int i) {
  if(legend && (int) cp.color1 == (int) 0x000000FF && backcolor == 0)
    poly_outline = 0x606060FF;
  else
    poly_outline = (bordcolor << 8) | 0xFF;
    
  transmatrix V1;
  
  auto& sh = *(vshapes[vertex_shape]);
  
  if(vertex_shape == 0) ;
  else if(DIM == 3 && among(cp.shade, 'b', 'f', 'g', 'B', 'F', 'G')) {
    V1 = V;
    }
  else if(DIM == 3) {
    V1 = face_the_player(V);
    if(info) queueaction(PPR::MONSTER_HEAD, [info] () { svg::link = *info; });
    queuepolyat(V1, sh, darken_a(cp.color1), PPR::MONSTER_HEAD);
    if(info) queueaction(PPR::MONSTER_HEAD, [] () { svg::link = ""; });
    V1 = V;
    }
  else if(rog3) {
    int p = poly_outline; poly_outline = OUTLINE_TRANS; 
    queuepolyat(V, sh, 0x80, PPR::MONSTER_SHADOW); 
    poly_outline = p; 
    if(info) queueaction(PPR::MONSTER_HEAD, [info] () { svg::link = *info; });
    queuepolyat(V1 = mscale(V, geom3::BODY), sh, darken_a(cp.color1), PPR::MONSTER_HEAD);
    if(info) queueaction(PPR::MONSTER_HEAD, [] () { svg::link = ""; });
    }
  else {
    if(info) queueaction(PPR::MONSTER_HEAD, [info] () { svg::link = *info; });
    queuepoly(V1 = V, sh, darken_a(cp.color1));
    if(info) queueaction(PPR::MONSTER_HEAD, [] () { svg::link = ""; });
    }
  switch(cp.shade) {
    case 't': queuepoly(V1, shDiskT, darken_a(cp.color2)); return;
    case 's': queuepoly(V1, shDiskS, darken_a(cp.color2)); return;
    case 'q': queuepoly(V1, shDiskSq, darken_a(cp.color2)); return;
    case 'm': queuepoly(V1, shDiskM, darken_a(cp.color2)); return;
    case 'b': queuepoly(V1, DIM == 3 ? shAnimatedTinyEagle[((long long)(ticks) * 30 / 1000+i) % 30] : shTinyBird, darken_a(cp.color2)); return;
    case 'f': queuepoly(V1, shTinyShark, darken_a(cp.color2)); return;
    case 'g': queuepoly(V1, shMiniGhost, darken_a(cp.color2)); return;
    case 'B': queuepoly(V1, DIM == 3 ? shAnimatedEagle[((long long)(ticks) * 30 / 1000+i) % 30] : shEagle, darken_a(cp.color2)); return;
    case 'F': queuepoly(V1, shShark, darken_a(cp.color2)); return;
    case 'G': queuepoly(V1, shGhost, darken_a(cp.color2)); return;
    }
  }

unordered_map<pair<edgeinfo*, int>, int> drawn_edges;

map<pair<cell*, cell*>, transmatrix> relmatrices;

transmatrix& memo_relative_matrix(cell *c1, cell *c2) {
  auto& p = relmatrices[make_pair(c1, c2)];
  if(p[2][2] == 0)
    p = calc_relative_matrix(c1, c2,  C0);
  return p;
  }

bool drawVertex(const transmatrix &V, cell *c, shmup::monster *m) {
  if(m->dead) return true;
  int i = m->pid;
  vertexdata& vd = vdata[i];
  
  bool ghilite = false;
  
  if(vd.special && specialmark) ghilite = true;
  
  if(!gmatrix.count(m->base)) printf("base not in gmatrix\n");

  int lid = shmup::lmousetarget ? shmup::lmousetarget->pid : -2;
  
  if(!leftclick) for(int j=0; j<isize(vd.edges); j++) {
    edgeinfo *ei = vd.edges[j].second;
    if(ei->weight < ei->type->visible_from) continue;
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
    
    bool multidraw = quotient || euwrap;
        
    if(ei->lastdraw < frameid || multidraw) { 
      ei->lastdraw = frameid;
      
      color_t col = ei->type->color;
      auto& alpha = part(col, 0);
      
      if(kind == kSAG) {
        if(ei->weight2 > maxweight) maxweight = ei->weight2;
        ld gamma = log(alpha / 255.) / log(.5);
        alpha = int(pow(ei->weight2 / maxweight, gamma) * 255);
        }
      if(hilite || hiliteclick) alpha = (alpha + 256) / 2;
      
      if(svg::in && alpha < 16) continue;
      
      if(ISWEB) {
        if(alpha >= 128) alpha |= 15;
        else if(alpha >= 64) alpha |= 7;
        else if(alpha >= 32) alpha |= 3;
        else if(alpha >= 16) alpha |= 1;
        }
      
      alpha >>= darken;

      transmatrix gm1 = 
        (multidraw || elliptic) ? V * memo_relative_matrix(vd1.m->base, c) :
        ggmatrix(vd1.m->base);
      transmatrix gm2 = 
        (multidraw || elliptic) ? V * memo_relative_matrix(vd2.m->base, c) :
        ggmatrix(vd2.m->base);
                
      hyperpoint h1 = gm1 * vd1.m->at * C0;
      hyperpoint h2 = gm2 * vd2.m->at * C0;
      
      if(elliptic && intval(h1, h2) > intval(h1, centralsym * h2))
        h2 = centralsym * h2;
      
      if(multidraw) {
        int code = int(h1[0]) + int(h1[1]) * 12789117 + int(h2[0]) * 126081253 + int(h2[1]) * 126891531;
        int& lastdraw = drawn_edges[make_pair(ei, code)];
        if(lastdraw == frameid) continue;
        lastdraw = frameid;
        }

      /* if(hdist0(h1) < .001 || hdist0(h2) < .001) {
        printf("h1 = %s\n", display(h1));
        printf("h2 = %s\n", display(h2));
        display(m->at);
        display(vd2.m->at);
        display(V);
        display(gmatrix[vd2.m->base]);
        display(shmup::calc_gmatrix(vd2.m->base));
        } */
      
      if(hilite) {
        col &= 0xFF;
        col |= 0xFF000000;
        }

      else if((col >> 8) == (DEFAULT_COLOR >> 8)) {
        col &= 0xFF;
        col |= (forecolor << 8);
        }

      bool onspiral = kind == kSpiral && abs(ei->i - ei->j) == 1;      
      if(pmodel || onspiral) {
        if(onspiral) {
          const int prec = 20; 
          transmatrix T = ggmatrix(currentmap->gamestart());
          hyperpoint l1 = T*tC0(spiral::at(1+ei->i));
          for(int z=1; z<=prec; z++) {
            hyperpoint l2 = T*tC0(spiral::at(1+ei->i+(ei->j-ei->i) * z / (prec+.0)));
            queueline(l1, l2, col, vid.linequality).prio = PPR::STRUCT0;
            l1 = l2;
            }
          }
        else {
          queueline(h1, h2, col, 2 + vid.linequality).prio = PPR::STRUCT0;
          }
        }
      else {
      
        cell *center = multidraw ? c : euclid ? cwt.at : viewctr.at->c7;
      
        if(!multidraw && ei->orig && ei->orig != center && celldistance(ei->orig, center) > 3) 
          ei->orig = NULL;
        if(!ei->orig) {
          ei->orig = center; // cwt.at;
          ei->prec.clear();
          
          transmatrix T = inverse(ggmatrix(ei->orig));

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
        queuetable(multidraw ? V : ggmatrix(ei->orig), ei->prec, isize(ei->prec), col, 0,
          PPR::STRUCT0);
        if(elliptic)
        queuetable(centralsym * ggmatrix(ei->orig), ei->prec, isize(ei->prec), col, 0,
          PPR::STRUCT0);
        }
      }
/*
    */
    }

  if(!vd.virt) {
    queuedisk(V * m->at, ghilite ? colorpair(0xFF0000FF) : vd.cp, false, vd.info, i);
    }
  
  
  if(showlabels) {
    bool doshow = true;
    if(kind == kTree && i > 0 && !vd.virt) {
      vertexdata& vdp = vdata[vd.data];
      hyperpoint h2 = ggmatrix(vdp.m->base) * vdp.m->at * C0;
      if(hdist(h2, V * m->at * C0) < 0.1) doshow = false;
      }
    
    hyperpoint h = tC0(V * m->at);
    transmatrix V2 = DIM == 3 ? V : rgpushxto0(h) * ypush(PURE ? .3 : .2); // todo-variation
    if(doshow && !behindsphere(V2)) {
      auto info = vd.info;
      if(info) queueaction(PPR::MONSTER_HEAD, [info] () { svg::link = *info; });
      queuestr(V2, (svg::in ? .28 : .2) * crossf / hcrossf, vd.name, backcolor ? 0x000000 : 0xFFFF00, (svg::in || ISWEB) ? 0 : 1);
      if(info) queueaction(PPR::MONSTER_HEAD, [] () { svg::link = ""; });
      }
    }

  if(kind == kCollatz) {
    if(c->cpdist > 7 && euclid) ;
    else if(vd.data == 2) {
      // doubler vertex
      string s = vd.name;
      colorpair cp = vd.cp;
      vd.data = 20;
      int i0 = isize(vdata);
      vdata.resize(i0+1);
      vertexdata& vdn = vdata[i0];
      createViz(i0, m->base, m->at * collatz::T2);
      
      virtualRebase(vdn.m, true);
      vdn.cp = perturb(cp);
      vdn.data = 0;
      addedge(i, i0, 1, false, collatz::collatz1);
      vdn.m->store();
      int carry = 0;
      string s2 = s;
      for(int i=isize(s2)-1; i>=0; i--) {
        int x = 2*(s2[i] - '0') + carry;
        carry = x>=10;
        if(carry) x-=10;
        s2[i] = '0'+x;
        }
      if(carry) s2 = "1" + s2;
      vdn.name = s2;
      
      int m3 = 0;
      for(int i=0; i<isize(s); i++) m3 += s[i] - '0';
      
      if(m3 % 3 == 2 && s != "2" && s != "1") {
        vdata.resize(i0+2);
        vertexdata& vdn = vdata[i0+1];
        createViz(i0+1, m->base, m->at * collatz::T3);
        virtualRebase(vdn.m, true);
        vdn.cp = perturb(cp);
        vdn.data = 0;
        addedge(i, i0+1, 1, false, collatz::collatz2);
        vdn.m->store();
        int carry = -1;
        string s2 = s;
        for(int i=isize(s2)-1; i>=0; i--) {
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

  return true;
  }

vector<int> legend;

vector<cell*> named;

bool rogueviz_hud() {
  if(!rogueviz::on) return false;
  if(cmode & sm::DRAW) return false;

  int qet = isize(edgetypes);
  if(qet == 1 || !edge_legend) qet = 0;

  int legit = qet + isize(legend);
  
  if(legit == 0) return true;

  initquickqueue();
  
  int rad = current_display->radius/10;
  ld x = vid.xres - rad;

  for(int i=0; i<isize(legend); i++) {
    int k = legend[i];
    vertexdata& vd = vdata[k];
    
    ld y = (current_display->radius * (i+.5)) / legit * 2 - current_display->radius + vid.yres/2;

    transmatrix V = atscreenpos(x, y, current_display->radius/4);
    
    poly_outline = OUTLINE_NONE;
    queuedisk(V, vd.cp, true, NULL, i);
    poly_outline = OUTLINE_DEFAULT;
    queuestr(int(x-rad), int(y), 0, rad*(svg::in?5:3)/4, vd.name, forecolor, 0, 16);
    }

  for(int i=0; i<qet; i++) {
    auto t = edgetypes[i];
        
    ld y = (current_display->radius * (i+isize(legend)+.5)) / legit * 2 - current_display->radius + vid.yres/2;

    transmatrix V = atscreenpos(x, y, current_display->radius/8);
    
    poly_outline = t->color | 0xFF;
    queuepolyat(V, shTriangle, 0, PPR::MONSTER_HEAD);
    
    poly_outline = OUTLINE_DEFAULT;
    queuestr(int(x-rad), int(y), 0, rad*(svg::in?5:3)/4, t->name, forecolor, 0, 16);
    }
  
  quickqueue();
  return true;
  }

void drawExtra() {
  
  if(kind == kFullNet) {
    for(map<cell*, transmatrix>::iterator it = gmatrix.begin(); it != gmatrix.end(); it++) {
      cell *c = it->first;
      c->wall = waChasm;
      }

    for(map<cell*, transmatrix>::iterator it = gmatrix.begin(); it != gmatrix.end(); it++) {
      cell *c = it->first;
      bool draw = true;
      for(int i=0; i<isize(named); i++) if(named[i] == c) draw = false;
      if(draw && gmatrix.count(c))
        queuedisk(it->second, dftcolor, false, NULL);
        // queuepolyat(it->second, shDisk, dftcolor., PPR::LINE);
      }
    
    for(int i=0; i<isize(named); i++) if(gmatrix.count(named[i])) {
      string s = ""; s += 'A'+i;
      queuestr(gmatrix[named[i]], 1, s, forecolor, 1);
      }
    
    canmove = true; items[itOrbAether] = true;
    }
  }

void readcolor(const string& cfname) {
  FILE *f = fopen(cfname.c_str(), "rt");
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
    int known_id = -1;
    bool force = false;

    if(kohonen::samples && c2 == '!') {
      force = true;
      c2 = fgetc(f);
      if(c2 == 10 || c2 == 13) continue;
      }
    
    if(kohonen::samples && c2 == '+') {
      known_id = kohonen::showsample(lab);
      c2 = fgetc(f);
      if(c2 == 10 || c2 == 13) continue;
      }
    if(c2 == '#') {
      while(c2 != 10 && c2 != 13 && c2 != -1) c2 = fgetc(f);
      continue;
      }
    else if(c2 == '=') {
      string lab2 = "";
      while(true) {
        int c = fgetc(f);
        if(c == 10 || c == 13 || c == 32 || c == 9 || c == ',' || c == ';' || c == EOF) break;
        else lab2 += c;
        }
      x = vdata[getid(lab2)].cp;
      }
    else if(c2 == '@') {
      legend.push_back(known_id == -1 ? getid(lab) : known_id);
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
    
    if(isize(lab) && lab[0] == '*') {
      lab = lab.substr(1);
      for(int i=0; i<isize(vdata); i++)
        if(vdata[i].name.find(lab) != string::npos) {
          vdata[i].cp = x;
          }
      }
    else if(kohonen::samples && !force) {
      for(int i=0; i<isize(vdata); i++)
        if(vdata[i].name == lab) {
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
  showstartmenu = false;
#if !ISWEB
  mapeditor::drawplayer = false;
  stop_game();
  firstland = specialland = laCanvas;
  restart_game(shmup::on ? rg::shmup : rg::nothing);
#else
  stop_game();
  firstland = specialland = laCanvas;
  restart_game(rg::nothing);
#endif
  on = true;
  autocheat = true;
  items[itOrbLife] = 0;
  timerghost = false;

  gmatrix.clear();
  calcparam();
  drawthemap();
  gmatrix0 = gmatrix;
  addMessage("RogueViz enabled");
  }

int search_for = -1;

void close() { 
  search_for = -1;
  for(int i=0; i<isize(vdata); i++)
    if(vdata[i].m) vdata[i].m->dead = true;
  vdata.clear();
  labeler.clear();
  legend.clear();
  for(int i=0; i<isize(edgeinfos); i++) delete edgeinfos[i];
  edgeinfos.clear();
  anygraph::coords.clear();
  sag::sagedges.clear();
  edgetypes.clear();
  on = false;
  relmatrices.clear();
  }

bool turn(int delta) {
  if(!on) return false;
  if(kind == kSAG) sag::iterate(), timetowait = 0;
  if(kind == kKohonen) kohonen::steps(), timetowait = 0;
  return false;
  // shmup::pc[0]->rebase();
  }

#ifndef CAP_RVSLIDES
#define CAP_RVSLIDES (CAP_TOUR && !ISWEB)
#endif

int dimid(char x) {
  if(x >= 'a' && x < 'a' + DIM) return x - 'a';
  else if(x >= '0' && x < '0' + DIM) return x - '0';
  else if(x >= 'x' && x < 'x' + DIM) return x - 'x';
  else {
    println(hlog, "incorrect dimension ID");
    throw hr_exception();
    }
  }

#if CAP_COMMANDLINE
int readArgs() {
  using namespace arg;
           
// options before reading
  if(0) ;
  else if(argis("-dftcolor")) {
    shift(); dftcolor = parse(args());
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
    shift_arg_formula(sag::edgepower);
    shift_arg_formula(sag::edgemul);
    }
// (1) configure temperature (high, low)
  else if(argis("-sagtemp")) {
    shift(); sag::hightemp = argi();
    shift(); sag::lowtemp = argi();
    }
  else if(argis("-sagmin")) {
    shift_arg_formula(default_edgetype.visible_from);
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
    shift(); sscanf(argcs(), "%lf,%lf,%lf,%lf", &s2, &p2, &s3, &p3);
    start();
    }

  else if(argis("-collatz3")) {
    PHASE(3); 
    using namespace collatz; 
    s2 = p2 = s3 = p3 = 0;
    start();
    shift();
    transmatrix *T = &T2;
    while(true) {
      if(arg::nomore()) break;
      else if(argis("fd")) { shift(); *T = *T * xpush(argf()); shift(); }
      else if(argcs()[0] == 't') { int x = dimid(argcs()[1]); int y = dimid(argcs()[2]); shift(); *T = *T * hr::cspin(x, y, argf()); shift(); }
      else if(argis("/")) { shift(); if(T == &T2) T = &T3; else break; }
      else break;
      }
    }

  else if(argis("-spiral")) {
    PHASE(3); 
    ld mul = 2;
    int N = 1000;
    shift(); sscanf(argcs(), LDF ",%d", &mul, &N);
    spiral::place(N, mul);
    }

  else if(argis("-net")) {
    PHASE(3);
    init(); kind = kFullNet;
    linepatterns::setColor(linepatterns::patTriTree, 0x30);
    linepatterns::setColor(linepatterns::patTriOther, 0x10);
    linepatterns::setColor(linepatterns::patTriRings, 0xFF);
    }

  else if(argis("-spiraledge")) {
    PHASE(3); 
    ld shft = 1;
    ld mul = 1;
    shift(); sscanf(argcs(), LDF "," LDF, &shft, &mul);
    spiral::edge(shft, mul);
    }

  else if(argis("-spiralcolor")) {
    PHASE(3); 
    ld period = 1;
    ld start = 1;
    shift(); sscanf(argcs(), LDF "," LDF, &period, &start);
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
  else if(argis("-lab-off")) {
    showlabels = false;
    }
  else if(argis("-rog3")) {
    rog3 = true;
    }
  else if(argis("-rvedge")) {
    shift(); default_edgetype.color = arghex();
    }
  else if(argis("-ggamma")) {
    // backward compatibility
    shift(); part(default_edgetype.color, 0) = 255 * pow(.5, argf());
    }
  else if(argis("-cshift")) {
    shift_arg_formula(collatz::cshift);
    }
  else if(argis("-rvwarp")) {
    patterns::whichShape = '8';
    }
  else if(argis("-lq")) {
    shift_arg_formula(linequality);
    }
#if CAP_RVSLIDES
  else if(argis("-rvpres")) {
    tour::slides = rvtour::rvslides;
    }
#endif
  else if(argis("-nolegend")) {
    legend.clear();
    }
  else if(argis("-edgelegend")) {
    edge_legend = true;
    }
  else if(argis("-rvshape")) {
    shift(); vertex_shape = argi() & 3;
    }
  
// graphical output 
//------------------

  // shmup::turn might be necessary when saving screenshots
  else if(argis("-TURN")) {
    PHASE(3); shmup::turn(100);
    }
  else return 1;
  return 0;
  }
#endif

void configure_edge_display() {
  cmode = sm::SIDE | sm::MAYDARK | sm::DIALOG_STRICT_X;
  static int mode = 0;
  gamescreen(0);  
  dialog::init(XLAT("rogueviz edges"));
  string s3 = kind == kSAG ? "min weight" : kind == kKohonen ? "quantity" : "extent";
  for(int i=0; i<isize(edgetypes); i++) {
    auto t = edgetypes[i];
    switch(mode) {
      case 0:
        dialog::addSelItem(t->name, itsh(t->color), 'a' + i);
        dialog::lastItem().colorv = t->color >> 8;
        dialog::add_action([t] {
          dialog::openColorDialog(t->color, NULL);
          dialog::dialogflags |= sm::MAYDARK | sm::SIDE;
          });
        break;
      case 1:
        if(kind == kSAG) {
          dialog::addSelItem(t->name, fts(t->visible_from), 'a'+i);
          dialog::add_action([t] {
            dialog::editNumber(t->visible_from, 0.001, 1000, .1, .1, "min weight", "");
            dialog::scaleLog();
            });
          }
        else {
          dialog::addSelItem(t->name, its(1 / t->visible_from), 'a'+i);
          dialog::add_action([t,s3] {
            static int i;
            i = 1 / t->visible_from;
            dialog::editNumber(i, 1, 1000000, 1, 500, s3, "");
            dialog::reaction = [t] () { t->visible_from = i ? 1. / i : 5; };
            dialog::scaleLog(); dialog::ne.step = .2;
            });
          }
        break;
      default: break;
      }
    }
  dialog::addBreak(100);
  if(among(kind, kSAG, kKohonen, kSpiral)) {
    dialog::addBoolItem("color/alpha", mode == 0, '1');
    dialog::add_action([] () { mode = 0; });
    dialog::addBoolItem(s3, mode == 1, '2');
    dialog::add_action([] () { mode = 1; });
    }
  else mode = 0;
  
  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  }

void search_marker() {
  if(search_for >= 0 && search_for < isize(vdata)) {
    auto& vd = vdata[search_for];
    auto& m = vd.m;
    if(!m) return;
    hyperpoint H = ggmatrix(m->base) * tC0(m->at);
    queuechr(H, 2*vid.fsize, 'X', 0x10101 * int(128 + 100 * sin(ticks / 150.)));
    addauraspecial(H, iinf[itOrbYendor].color, 0);
    }
  }

void showVertexSearch() {
  cmode = sm::SIDE | sm::MAYDARK | sm::DIALOG_STRICT_X;
  gamescreen(0); search_for = -1;

  dialog::init(XLAT("vertex search"));
  dialog::v.clear();
  if(dialog::infix != "") mouseovers = dialog::infix;
  
  for(int i=0; i<isize(vdata); i++) if(vdata[i].name != "") dialog::vpush(i, vdata[i].name.c_str());
  
  for(int i=0; i<9; i++) {
    if(i < isize(dialog::v)) {
      int id = dialog::v[i].second;
      dialog::addItem(dialog::v[i].first, '1'+i);
      dialog::add_action([id] () { 
        search_for = id; 
        popScreenAll(); 
        });
      }
    else dialog::addBreak(100);
    }

  dialog::addSelItem("matching items", its(isize(dialog::v)), 0);
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);    
    if(dialog::editInfix(uni)) ;
    else if(doexiton(sym, uni)) popScreen();
    };

  }

void showMenu() {
  if(staircase::on) { staircase::showMenu(); return; }
  cmode = sm::SIDE | sm::MAYDARK | sm::DIALOG_STRICT_X;
  gamescreen(0);  

  dialog::init(XLAT("rogueviz configuration"));

  dialog::addSelItem(XLAT("temperature"), fts(sag::temperature), 't');
  if(kind == kSAG)
    dialog::addSelItem(XLAT("SAG mode"), sag::sagmodes[sag::sagmode], 'm');
  dialog::addBoolItem(XLAT("show labels"), showlabels, 'l');
  dialog::addBoolItem(XLAT("mark special vertices"), specialmark, 'x');
  dialog::addSelItem(XLAT("background color"), itsh(backcolor), 'b');
  if(isize(edgetypes))
    dialog::addSelItem(XLAT("edge types"), its(isize(edgetypes)), 'g');
  dialog::addBoolItem(XLAT("vertices in 3D"), rog3, 'v');
  dialog::addSelItem(XLAT("vertex shape"), its(vertex_shape), 'w');

  dialog::addItem(XLAT("vertex search"), '/');
  dialog::add_action_push(showVertexSearch);
  
  if(kind == kKohonen)
    kohonen::showMenu();

  dialog::addBreak(50);
  dialog::addBack();

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
    else if(uni == 'w') vertex_shape = (1 + vertex_shape) & 3;
    else if(uni == 'x') specialmark = !specialmark;
    else if(uni == 'b') backcolor ^= 0xFFFFFF, bordcolor ^= 0xFFFFFF, forecolor ^= 0xFFFFFF;
    else if(uni == 'g') pushScreen(configure_edge_display);
    else if(uni == 'z') {
      for(int i=0; i<isize(named)-1; i++) if(named[i] == cwt.at)
        swap(named[i], named[i+1]);
      if(!isize(named) || named[isize(named)-1] != cwt.at) named.push_back(cwt.at);
      printf("named = %d\n", isize(named));
      popScreen();
      }
    else if(kind == kKohonen && kohonen::handleMenu(sym, uni)) ;
    else if(doexiton(sym, uni)) popScreen();
    };
  }

#if CAP_RVSLIDES
namespace rvtour {

using namespace tour;

string cname() {
  if(euclid) return "coord-6.txt";
  if(PURE) return "coord-7.txt";
  return "coord-67.txt";
  }

template<class T> function<void(presmode)> roguevizslide(char c, const T& t) {
  return [c,t] (presmode mode) {
    patterns::canvasback = 0x101010;
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
    centerover.at = NULL; pd_from = NULL;
    };
  }

template<class T, class U>
function<void(presmode)> roguevizslide_action(char c, const T& t, const U& act) {
  return [c,t,act] (presmode mode) {
    patterns::canvasback = 0x101010;
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
        if(mode == pmStartAll) firstland = specialland = laPalace;
        if(mode == 4) {
          tour::slides = default_slides;
          while(tour::on) restart_game(rg::tour);
          firstland = specialland = laIce;
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
  {"Collatz conjecture", 51, LEGAL_UNLIMITED | QUICKGEO,
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

  {"Roguelikes", 63, LEGAL_UNLIMITED | QUICKGEO,
    "A visualization of roguelikes, based on discussion on /r/reddit. "
    "See: http://www.roguetemple.com/z/hyper/reddit.php",
    roguevizslide('0', [] () {
      rogueviz::dftcolor = 0x282828FF;

      rogueviz::showlabels = true;
      part(rogueviz::default_edgetype.color, 0) = 181;
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
  {"Programming languages of GitHub", 64, LEGAL_UNLIMITED | QUICKGEO,
    "A visualization of programming languages.",
    roguevizslide('0', [] () {
      rogueviz::dftcolor = 0x282828FF;

      rogueviz::showlabels = true;
      part(rogueviz::default_edgetype.color, 0) = 128;
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
    {"Boardgames", 62, LEGAL_UNLIMITED | QUICKGEO,
        "A visualization of board games, based on discussions on Reddit.",
    roguevizslide('0', [] () {
      rogueviz::dftcolor = 0x282828FF;

      rogueviz::showlabels = true;
      part(rogueviz::default_edgetype.color, 0) = 157;
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
    {"Tree of Life", 61, LEGAL_UNLIMITED | QUICKGEO,
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
    {"Spiral Staircase", 62, LEGAL_NONE | QUICKGEO,
     "Spiral Staircase Demo. Press '5' to change the curvature or other parameters.",
     
    [] (presmode mode) {
      if(mode == 1) staircase::make_staircase();
      if(mode == 3) rug::close();
      slidecommand = "staircase menu";
      if(mode == 4) pushScreen(staircase::showMenu);
      }},
    {"Banach-Tarski-like", 62, LEGAL_NONE,
     "Banach-Tarski-like decomposition. Break a hyperbolic plane into two hyperbolic planes.\n\n"
     "Press '5' to show the decomposition. Press any key to stop.\n\n"
     "You will see a map of the decomposition. Press '5' again to return.",
     
    [] (presmode mode) {
      slidecommand = "staircase menu";
      if(mode == 3) {
        while(gamestack::pushed()) pop_game();
        banachtarski::bmap = false;
        banachtarski::on = false;
        }
      if(mode == 4) {
        if(!banachtarski::on) {
          bool b = mapeditor::drawplayer;
          specialland = cwt.at->land;
          push_game();
          banachtarski::init_bantar();
          airmap.clear();
          dynamicval<int> vs(sightrange_bonus, 3);
          dynamicval<int> vg(genrange_bonus, 3);
          doOvergenerate();
          banachtarski::bantar_anim();
          quitmainloop = false;
          mapeditor::drawplayer = b;
          banachtarski::init_bantar_map();
          }
        else if(banachtarski::on && banachtarski::bmap) {
          banachtarski::bmap = false;
          banachtarski::on = false;
          pop_game();
          }
        }
      }},
    {"Pentagonal Exploration", 62, LEGAL_NONE | QUICKGEO,
     "Pentagonal Exploration explained at: http://www.roguetemple.com/z/sims/snub/\n\n"
     "Move the mouse nearer and further away from the X.\n\n"
     "Press 3 4 5 6 7 8 9 shift+4 shift+5 shift+6 to change the geometry.",
     
    [] (presmode mode) {
      if(mode == 1) {
        pentagonal::run_snub(5, 3);
        }
      if(mode == 3) {
        printf("stopping\n");
        set_geometry(gNormal);
        set_variation(eVariation::bitruncated);
        rug::close();
        start_game();
        }
      }},
  {"THE END", 99, LEGAL_ANY | FINALSLIDE,
    "Press '5' to leave the presentation.",
    [] (presmode mode) {
      firstland = specialland = laIce;
      if(mode == 4) restart_game(rg::tour);
      }
    }
  };

int rvtour_hooks = 
  addHook(hooks_startmenu, 100, [] () {
#if CAP_TOUR
    dialog::addBreak(100);
    dialog::addBigItem(XLAT("RogueViz"), 'r');
    dialog::add_action([] () {
      tour::slides = rogueviz::rvtour::rvslides;
      popScreenAll();
      tour::start();
      printf("tour start\n");
      });
    dialog::addInfo(XLAT("see the visualizations"));
#endif
    }) +
  addHook(hooks_slide, 100, [] (int mode) {
    if(currentslide == 0 && slides == default_slides) {
      slidecommand = "RogueViz presentation";
      if(mode == 1)
        help += 
          "\n\nYour version of HyperRogue is compiled with RogueViz. "
          "Press '5' to switch to the RogueViz slides. Watching the "
          "common HyperRogue tutorial first is useful too, "
          "as an introduction to hyperbolic geometry.";         
      if(mode == 4) {
        slides = rogueviz::rvtour::rvslides;
        while(tour::on) restart_game(rg::tour);
        tour::start();
        }
      }
    }) +
  0;

}
#endif

bool default_help() {
  if(!rogueviz::on) return false;

  help = 
    "This is RogueViz, a visualization engine based on HyperRogue.\n\nUse WASD to move, v for menu.\n\n"
    "Read more about RogueViz on : http://roguetemple.com/z/hyper/rogueviz.php\n\n";
  if(kind == kAnyGraph)
    help += "Current visualization: any graph\n\n" + fname;
  if(kind == kTree)
    help += "Current visualization: tree\n\n" + fname;
  if(kind == kSpiral)
    help += "Current visualization: spiral\n\n";
  if(kind == kSAG)
    help += "Current visualization: SAG\n\n" + fname;
  if(kind == kCollatz)
    help += "Current visualization: Collatz conjecture\n\n";
  if(kind == kFullNet)
    help += "Current visualization: full net\n\n";

  help_extensions.push_back(help_extension{'u', XLAT("RogueViz menu"), [] () { popScreen(); pushScreen(showMenu); }});    
  return true;
  }

named_functionality o_key() {
  if(rogueviz::on) return named_dialog(XLAT("rogueviz menu"), rogueviz::showMenu);
  return named_functionality();
  }

auto hooks  = 
  addHook(hooks_frame, 0, drawExtra) +
#if CAP_COMMANDLINE
  addHook(hooks_args, 100, readArgs) +
#endif
#if CAP_RVSLIDES
  addHook(hooks_config, 0, [] () { tour::ss::list(rogueviz::rvtour::rvslides); }) +
#endif
  addHook(clearmemory, 0, close) +
  addHook(hooks_prestats, 100, rogueviz_hud) +
  addHook(shmup::hooks_draw, 100, drawVertex) +
  addHook(shmup::hooks_describe, 100, describe_monster) +
  addHook(shmup::hooks_turn, 100, turn) + 
  addHook(shmup::hooks_kill, 100, activate) +
  addHook(hooks_o_key, 100, o_key) +
  addHook(hooks_mainmenu, 100, [] () {
    dialog::addItem(XLAT("rogueviz menu"), 'u'); 
    dialog::add_action_push(rogueviz::showMenu);
    }) +
  addHook(hooks_welcome_message, 100, [] () {
    if(rogueviz::on) addMessage(XLAT("Welcome to RogueViz!"));
    return rogueviz::on;
    }) +
  addHook(hooks_default_help, 100, default_help) +
  addHook(hooks_markers, 100, search_marker) +
 0;

};

#include "rogueviz-kohonen.cpp"
#include "rogueviz-staircase.cpp"
#include "rogueviz-banachtarski.cpp"
#include "rogueviz-video.cpp"
#include "rogueviz-pentagonal.cpp"
#include "rogueviz-graph.cpp"
#include "rogueviz-fundamental.cpp"
#include "rogueviz-sunflower.cpp"
#include "rogueviz-flocking.cpp"
#include "rogueviz-magiccube.cpp"
#include "rogueviz-cvl.cpp"
