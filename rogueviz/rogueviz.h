#ifndef _ROGUEVIZ_H_
#define _ROGUEVIZ_H_
// See: http://www.roguetemple.com/z/hyper/rogueviz.php

#include "../hyper.h"

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
  string weight_label;

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
  
  inline hookset<void(vertexdata&, cell*, shmup::monster*, int)> *hooks_drawvertex;
  inline hookset<bool(edgeinfo*, bool store)> *hooks_alt_edges;
  
  void readcolor(const string& cfname);

  void close();
  extern bool showlabels;
  
  namespace rvtour {
    using namespace hr::tour;
    inline hookset<void(vector<slide>&)> *hooks_build_rvtour;
    slide *gen_rvtour();
    }

  void createViz(int id, cell *c, transmatrix at);
  }

#endif
