#ifndef _ROGUEVIZ_H_
#define _ROGUEVIZ_H_
// See: http://www.roguetemple.com/z/hyper/rogueviz.php

#include "../hyper.h"

#define RVPATH HYPERPATH "rogueviz/"

#ifndef CAP_NCONF
#define CAP_NCONF 0
#endif

#ifndef CAP_RVSLIDES
#define CAP_RVSLIDES (CAP_TOUR && !ISWEB)
#endif

namespace rogueviz {
  using namespace hr;
  
  inline void *vizid;
  
  constexpr flagtype RV_GRAPH = 1;
  constexpr flagtype RV_WHICHWEIGHT = 2; // sag
  constexpr flagtype RV_AUTO_MAXWEIGHT = 4; // sag
  constexpr flagtype RV_COMPRESS_LABELS = 8; // do not display some labels
  constexpr flagtype RV_COLOR_TREE = 16; // color vertex together with tree parents
  constexpr flagtype RV_HAVE_WEIGHT = 32; // edges have weights
  constexpr flagtype RV_INVERSE_WEIGHT = 64; // edit weight, not 1/weight
  
  inline flagtype vizflags;
  extern string weight_label;
  extern ld maxweight;

  void drawExtra();
  void close();

  void init(void *vizid, flagtype flags);
  
  struct edgetype {
    double visible_from;
    double visible_from_hi;
    double visible_from_help;
    unsigned color, color_hi;
    string name;
    };

  edgetype *add_edgetype(const string& name);
  
  static const unsigned DEFAULT_COLOR = 0x471293B5;

  extern edgetype default_edgetype;
  
  extern vector<shared_ptr<edgetype>> edgetypes;
    
  struct edgeinfo {
    int i, j;
    double weight, weight2;
    vector<glvertex> prec;
    basic_textureinfo tinf;
    cell *orig;
    int lastdraw;
    edgetype *type;
    edgeinfo(edgetype *t) { orig = NULL; lastdraw = -1; type = t; }
    };

  extern vector<edgeinfo*> edgeinfos;
  void addedge0(int i, int j, edgeinfo *ei);
  void addedge(int i, int j, edgeinfo *ei);
  void addedge(int i, int j, double wei, bool subdiv, edgetype *t);
  extern vector<int> legend;
  extern vector<cell*> named;
  
  struct rvimage {
    basic_textureinfo tinf;
    texture::texture_data tdata;
    vector<hyperpoint> vertices;
    };

  struct colorpair {
    color_t color1, color2;
    char shade;
    shared_ptr<rvimage> img;
    colorpair(color_t col = 0xC0C0C0FF) { shade = 0; color1 = color2 = col; }
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
 
  void storeall(int from = 0);

  namespace anygraph {
    extern double R, alpha, T;
    extern vector<pair<double, double> > coords;
    
    void fixedges();
    void read(string fn, bool subdiv = true, bool doRebase = true, bool doStore = true);
    extern int N;
    }
  
  extern bool showlabels;

  extern bool rog3;
  extern bool rvwarp;

  extern colorpair dftcolor;
  
  inline hookset<void(vertexdata&, cell*, shmup::monster*, int)> hooks_drawvertex;
  inline hookset<bool(edgeinfo*, bool store)> hooks_alt_edges;
  inline purehookset hooks_rvmenu;
  inline hookset<bool()> hooks_rvmenu_replace;
  inline hookset<bool(int&, string&, FILE*)> hooks_readcolor;
  inline purehookset hooks_close;
  
  void readcolor(const string& cfname);

  void close();
  extern bool showlabels;
  
  namespace rvtour {
    using namespace hr::tour;
    inline hookset<void(vector<slide>&)> hooks_build_rvtour;
    slide *gen_rvtour();

template<class T> function<void(presmode)> roguevizslide(char c, const T& t) {
  return [c,t] (presmode mode) {
    patterns::canvasback = 0x101010;
    setCanvas(mode, c);
    if(mode == 1 || mode == pmGeometryStart) t();
  
    if(mode == 3 || mode == pmGeometry || mode == pmGeometryReset) {
      rogueviz::close();
      shmup::clearMonsters();
      if(mode == pmGeometryReset && !(slides[currentslide].flags & QUICKGEO)) t();
      }
  
    slidecommand = "toggle the player";
    if(mode == 4) 
      mapeditor::drawplayer = !mapeditor::drawplayer;
    pd_from = NULL;
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
      if(mode == pmGeometryReset && !(slides[currentslide].flags & QUICKGEO)) t();
      }
  
    };
  }

    }

  void createViz(int id, cell *c, transmatrix at);

  extern map<string, int> labeler;
  int getid(const string& s);
  int getnewid(string s);
  extern string fname;

  colorpair perturb(colorpair cp);
  void queuedisk(const shiftmatrix& V, const colorpair& cp, bool legend, const string* info, int i);

  }

#endif
