#include "../hyper.h"
#include "rogueviz.h"

// hyper -tol <filename.xml> -- visualize the tree of life,
//   based on a XML dump from https://tree.opentreeoflife.org/

namespace rogueviz {

namespace tree {

  int tree_id;
  
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
    init(&tree_id, RV_GRAPH | RV_COMPRESS_LABELS | RV_COLOR_TREE);
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
      virtualRebase(vd.m);
      }
    
    printf("Clearing the TOL data...\n");
    tol.clear();
    storeall();
    }

#if CAP_COMMANDLINE
int readArgs() {
  using namespace arg;

  if(0) ;

  else if(argis("-tree")) {
    PHASE(3); shift(); tree::read(args());
    }
  
  else return 1;
  return 0;
  }

int ah = addHook(hooks_args, 120, readArgs)
+ addHook(rvtour::hooks_build_rvtour, 120, [] (vector<tour::slide>& v) {
    using namespace rvtour;
    v.push_back(
      tour::slide{"hyperbolic geometry and data/Tree of Life", 61, LEGAL::UNLIMITED | QUICKGEO,
      "Hyperbolic geometry is much better than the Euclidean geometry at visualizing large trees and other hierarchical structures. "
      "Here we visualize the data from the Tree of Life project.",

    roguevizslide('0', [] () {

      rogueviz::dftcolor = 0x206020FF;

      rogueviz::showlabels = true;
      
      gmatrix.clear();
      drawthemap();
      gmatrix0 = gmatrix;

      rogueviz::tree::read(RVPATH "treeoflife/tol.txt");
      })}
      );
    });
#endif
  }

}
