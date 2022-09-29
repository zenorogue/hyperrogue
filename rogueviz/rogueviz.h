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
  extern ld ggamma;
  extern bool highlight_target;
  
  extern int vertex_shape;
  extern int search_for;

  void drawExtra();
  void close();

  void init(flagtype flags);
  
  void graph_rv_hooks();

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
  
  int readLabel(fhstream& f);

  #if CAP_TEXTURE
  struct rvimage {
    basic_textureinfo tinf;
    texture::texture_data tdata;
    vector<hyperpoint> vertices;
    };
  #endif
  
  extern int brm_limit;

  struct colorpair {
    color_t color1, color2;
    char shade;
    #if CAP_TEXTURE
    shared_ptr<rvimage> img;
    #endif
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
  
  extern vector<reaction_t> cleanup;
  
  void do_cleanup();

  inline void on_cleanup_or_next(const reaction_t& del) {
    #if CAP_TOUR
    if(tour::on) tour::on_restore(del);
    else
    #endif
    cleanup.push_back(del);
    }

  template<class T> void rv_change(T& variable, const T& value) {
    T backup = variable;
    variable = value;
    on_cleanup_or_next([backup, &variable] { variable = backup; });
    }

  template<class T, class U> void rv_hook(hookset<T>& m, int prio, U&& hook) {
    int p = addHook(m, prio, hook);
    auto del = [&m, p] { 
      delHook(m, p); 
      };
    on_cleanup_or_next(del);
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

  namespace pres {
    using namespace hr::tour;
#if CAP_RVSLIDES
    inline hookset<void(string, vector<slide>&)> hooks_build_rvtour;
    slide *gen_rvtour();
    #if CAP_TEXTURE
    void draw_texture(texture::texture_data& tex, ld dx = 0, ld dy = 0, ld scale = 1);
    #endif

    extern map<string, texture::texture_data> textures;

template<class T, class U> function<void(presmode)> roguevizslide(char c, const T& t, const U& f) {
  return [c,t,f] (presmode mode) {
    f(mode);
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

template<class T> function<void(presmode)> roguevizslide(char c, const T& t) { return roguevizslide(c, t, [] (presmode mode) {}); }

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


    void add_end(vector<slide>& s);

    template<class T, class U> void add_temporary_hook(int mode, hookset<T>& m, int prio, U&& hook) {
      using namespace tour;
      if(mode == pmStart) {
        int p = addHook(m, prio, hook);
        on_restore([&m, p] { 
          delHook(m, p); 
          });
        }
      }

  /* maks graphs in presentations */
  struct grapher {
  
    ld minx, miny, maxx, maxy;
    
    shiftmatrix T;
    
    grapher(ld _minx, ld _miny, ld _maxx, ld _maxy);
    void line(hyperpoint h1, hyperpoint h2, color_t col);
    void arrow(hyperpoint h1, hyperpoint h2, ld sca, color_t col = 0xFF);
    shiftmatrix pos(ld x, ld y, ld sca);
    };
  
  void add_stat(presmode mode, const bool_reaction_t& stat);  
  void compare_projections(presmode mode, eModel a, eModel b);
  void no_other_hud(presmode mode);
  void non_game_slide(presmode mode);
  void non_game_slide_scroll(presmode mode);
  void white_screen(presmode mode, color_t col = 0xFFFFFFFF);
  void empty_screen(presmode mode, color_t col = 0xFFFFFFFF);
  void show_picture(presmode mode, string s, flagtype flags = 0);
  void sub_picture(string s, flagtype flags = 0, ld dx = 0, ld dy = 0, ld scale = 1);
  void show_animation(presmode mode, string s, int sx, int sy, int frames, int fps);
  void use_angledir(presmode mode, bool reset);
  void slide_error(presmode mode, string s);

  static const flagtype LATEX_COLOR = 1;
  
  void show_latex(presmode mode, string s);
  void dialog_add_latex(string s, color_t color, int size = 100, flagtype flag = 0);
  void dialog_may_latex(string latex, string normal, color_t col = dialog::dialogcolor, int size = 100, flagtype flag = 0);
  void uses_game(presmode mode, string name, reaction_t launcher, reaction_t restore);
  void latex_slide(presmode mode, string s, flagtype flags = 0, int size = 100);
  
  inline purehookset hooks_latex_slide;

  inline ld angle = 0;
  inline int dir = -1;
  hyperpoint p2(ld x, ld y);
#endif
  }

  void createViz(int id, cell *c, transmatrix at);

  extern map<string, int> labeler;
  bool id_known(const string& s);
  int getid(const string& s);
  int getnewid(string s);
  extern string fname;

  bool rv_ignore(char c);

  colorpair perturb(colorpair cp);
  void queuedisk(const shiftmatrix& V, const colorpair& cp, bool legend, const string* info, int i);

/* 3D models */

namespace objmodels {

  using tf_result = pair<int, hyperpoint>;

  using transformer = std::function<tf_result(hyperpoint)>;
  using subdivider = std::function<int(vector<hyperpoint>&)>;
  
  inline ld prec = 1;
  
  inline bool shift_to_ctr = false;

  struct object {
    hpcshape sh;
    basic_textureinfo tv;
    color_t color;
    };
  
  struct model_data : gi_extension {
    ld prec_used;
    vector<shared_ptr<object>> objs;
    vector<int> objindex;
    void render(const shiftmatrix& V);
    };
  
  inline tf_result default_transformer(hyperpoint h) { return {0, direct_exp(h) };}
  
  inline int default_subdivider(vector<hyperpoint>& hys) { 
    if(euclid) return 1;
    ld maxlen = prec * max(hypot_d(3, hys[1] - hys[0]), max(hypot_d(3, hys[2] - hys[0]), hypot_d(3, hys[2] - hys[1])));
    return int(ceil(maxlen));
    }
  
  #if CAP_TEXTURE
  struct model {
  
    string path, fname;
    reaction_t preparer;
    transformer tf;
    subdivider sd;
  
    bool is_available, av_checked;

    model(string path = "", string fn = "", 
      transformer tf = default_transformer,
      reaction_t prep = [] {},
      subdivider sd = default_subdivider
      ) : path(path), fname(fn), preparer(prep), tf(tf), sd(sd) { av_checked = false; }
  
    map<string, texture::texture_data> materials;
    map<string, color_t> colors;
    
    /* private */
    void load_obj(model_data& objects);
    
    model_data& get();

    void render(const shiftmatrix& V) { get().render(V); }
    
    bool available();
    };

  void add_model_settings();
  #endif
  
  }

#if CAP_RVSLIDES
#define addHook_rvslides(x, y) addHook(rogueviz::pres::hooks_build_rvtour, x, y)
#define addHook_slideshows(x, y) addHook(tour::ss::hooks_extra_slideshows, x, y)
#define addHook_rvtour(x, y) addHook(pres::hooks_build_rvtour, x, y)
#else
#define addHook_rvslides(x, y) 0
#define addHook_slideshows(x, y) 0
#define addHook_rvtour(x, y) 0
#endif

  /* parallelize a computation */
  inline int threads = 1;

  #if CAP_THREAD
  template<class T> auto parallelize(long long N, T action) -> decltype(action(0,0)) {
    if(threads == 1) return action(0,N);
    std::vector<std::thread> v;
    typedef decltype(action(0,0)) Res;
    std::vector<Res> results(threads);
    for(int k=0; k<threads; k++)
      v.emplace_back([&,k] () { 
        results[k] = action(N*k/threads, N*(k+1)/threads); 
        });
    for(std::thread& t:v) t.join();
    Res res = 0;
    for(Res r: results) res += r;
    return res;
    }
  #endif

  }

#endif
