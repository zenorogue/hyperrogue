namespace rogueviz {

  void init();
  
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
}
