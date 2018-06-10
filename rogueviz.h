// See: http://www.roguetemple.com/z/hyper/rogueviz.php

namespace rogueviz {
  using namespace hr;

  extern bool on;
  string describe(shmup::monster *m);
  void describe(cell *c);
  void activate(shmup::monster *m);
  void drawVertex(const transmatrix &V, cell *c, shmup::monster *m);
  bool virt(shmup::monster *m);
  void turn(int delta);
  void drawExtra();
  void fixparam();
  int readArgs();
  void close();
  void mark(cell *c);
  void showMenu();
  string makehelp();

  void init();
    
  struct edgeinfo {
    int i, j;
    double weight, weight2;
    bool visible;
    vector<glvertex> prec;
    cell *orig;
    int lastdraw;
    edgeinfo() { visible = true; orig = NULL; lastdraw = -1; }
    };
  
  struct colorpair {
    int color1, color2;
    char shade;
    colorpair(int col = 0xC0C0C0FF) { shade = 0; color1 = col; }
    };
  
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
  
  extern vector<vertexdata> vdata;
 
  void storeall();

  namespace anygraph {
    extern double R, alpha, T;
    extern vector<pair<double, double> > coords;
    
    void fixedges();
    void read(string fn, bool subdiv = true, bool doRebase = true, bool doStore = true);
    extern int N;
    }
  
  extern ld ggamma;
  extern bool showlabels;

  extern bool rog3;
  extern bool rvwarp;
#if CAP_TOUR
  namespace rvtour {
    extern tour::slide rvslides[];
    }
#endif

  namespace kohonen {
    extern int samples;
    void showsample(int id);
    void showsample(string id);
    void describe(cell *c);
    void steps();
    void showMenu();
    bool handleMenu(int sym, int uni);
    }
  
  namespace staircase {
    extern bool on;
    void showMenu();
    void make_staircase();
    }
}

