#include "rogueviz.h"

// for this video: https://youtu.be/Rhjv_PazzZE

namespace hr {

namespace embchess {

namespace smoothcam { using namespace rogueviz::smoothcam; }

/** load Chess? */
EX bool with_chess = false;
/** load other models? */
EX bool with_models = false;
/** including the CGtrader models? (not included here) */
EX bool cgtrader_models = false;
/** floor disabled */
EX bool no_floor = false;
/** do not include the models inside the house for faster transition */
EX bool light_models = false;

/** higher periods needs to be set to display scaffold correctly */
int periods = 1;

/** how many chess moves performed */
int chess_moves = 0;

vector<pair<hyperpoint, hyperpoint>> map54_edges;
vector<hyperpoint> map54_nodes;

vector<pair<hyperpoint, hyperpoint>> map534_edges;
vector<hyperpoint> map534_nodes;

struct embset {
  string name;
  geom3::eSpatialEmbedding se;
  ld walls, scale, depth, eye, sun, sky, star;
  };

embset current() {
  embset e;
  e.name = "current";
  e.se = geom3::spatial_embedding;
  e.scale = geom3::euclid_embed_scale;
  auto actscale = e.scale;
  if(e.se == geom3::seDefault) { e.scale = 0; actscale = 1; }
  e.walls = vid.wall_height / actscale;
  e.depth = vid.depth / e.walls;
  e.eye = (vid.eye-vid.depth) / e.walls;
  e.sun = vid.sun_size / e.walls;
  e.sky = vid.sky_height;
  e.star = vid.star_height;
  e.se = geom3::spatial_embedding;
  return e;
  }

void activate(const embset& e) {
  if(GDIM == 2) invoke_embed(geom3::seDefault);
  embset c = current();
  geom3::changing_embedded_settings = true;
  geom3::switch_always3();
  geom3::euclid_embed_scale = e.se == geom3::seDefault ? 1 : e.scale;
  vid.wall_height = e.walls * geom3::euclid_embed_scale;
  vid.depth = e.depth * vid.wall_height;
  vid.eye = vid.depth + e.eye * vid.wall_height;
  vid.sun_size = e.sun * vid.wall_height;
  vid.sky_height = e.sky;
  vid.star_height = e.star;
  geom3::spatial_embedding = e.se;
  geom3::switch_always3();
  geom3::changing_embedded_settings = false;
  if(e.se != c.se) {
    if(vid.usingGL) resetGL();
    }
  delete_sky();
  pmodel = default_model();
  };

embset emb_lerp(const embset& a, const embset& b, ld f) {
  embset e;
  e.name = lalign(0, "lerp(", a, ", ", b, ", ", f, ")");
  e.se = b.se;
  e.scale = lerp(a.scale, b.scale, f);
  e.walls = lerp(a.walls, b.walls, f);
  e.depth = lerp(a.depth, b.depth, f);
  e.eye = lerp(a.eye, b.eye, f);
  e.sun = lerp(a.sun, b.sun, f);
  e.sky = lerp(a.sky, b.sky, f);
  e.star = lerp(a.star, b.star, f);  
  return e;
  }

void print(hstream& hs, const embset& e) {
  print(hlog, "embset{.name=\"", e.name, "\", .se=eEmbeddingMethod(", int(e.se), "), .walls=", e.walls, ", .scale=", e.scale, ", .depth=", e.depth, ", .eye=", e.eye, ", .sun=", e.sun, ", .sky=", e.sky, ", .star=", e.star, "}");
  }

embset edefault = embset{.name="default", .se=geom3::seDefault, .walls=1.2, .scale=0, .depth=0, .eye=1.5, .sun=0.333333, .sky=10, .star=9};
embset edefaulti = embset{.name="default", .se=geom3::seDefault, .walls=-1.2, .scale=0, .depth=0, .eye=-1.5, .sun=0.333333, .sky=10, .star=9};
// embset eincyl = embset{.name="in cylinder", .se=geom3::seCylinderE, .walls=0.75, .scale=M_PI/10, .depth=0, .eye=1.5, .sun=0.10472, .sky=4, .star=3.6};
embset eincyl = embset{.name="in cylinder E", .se=geom3::seCylinderE, .walls=1.2, .scale=M_PI/10, .depth=0, .eye=1.5, .sun=0.10472, .sky=2.6, .star=2.4};
embset eoutcyl = embset{.name="out cylinder E", .se=geom3::seCylinderE, .walls=-1.2, .scale=M_PI/10, .depth=0, .eye=1.5, .sun=0.3, .sky=10, .star=9};
embset eouthoro = embset{.name="out horosphere", .se=geom3::seLowerCurvature, .walls=1.2, .scale=0.1, .depth=0, .eye=1.5, .sun=0.25, .sky=8, .star=7.5};
embset einhoro = embset{.name="in horosphere", .se=geom3::seLowerCurvature, .walls=-1.2, .scale=0.1, .depth=0, .eye=1.5, .sun=0.25, .sky=12, .star=11};
embset einhoro_small = embset{.name="in horosphere (small sky)", .se=geom3::seLowerCurvature, .walls=-1.2, .scale=0.1, .depth=0, .eye=1.5, .sun=0.25, .sky=18, .star=17};
embset esolv = embset{.name="solv", .se=geom3::seSol, .walls=1.2, .scale=0.1, .depth=0, .eye=1.5, .sun=0.25, .sky=12, .star=11};
embset einnih = embset{.name="in NIH", .se=geom3::seNIH, .walls=-1.2, .scale=0.1, .depth=0, .eye=1.5, .sun=0.25, .sky=12, .star=11};
embset eoutnih = embset{.name="out NIH", .se=geom3::seNIH, .walls=1.2, .scale=0.1, .depth=0, .eye=1.5, .sun=0.25, .sky=8, .star=7.5};
embset eclifford = embset{.name="Clifford", .se=geom3::seCliffordTorus, .walls=1.2, .scale=M_PI/10, .depth=-0.0561826, .eye=1.5, .sun=0.25, .sky=2.55, .star=2.3};
embset enil = embset{.name="Nil flat", .se=geom3::seNil, .walls=1.2, .scale=0.1, .depth=0, .eye=1.5, .sun=0.25, .sky=12, .star=11};
embset esl2 = embset{.name="SL(2,R) flat", .se=geom3::seSL2, .walls=1.2, .scale=0.1, .depth=0, .eye=1.5, .sun=0.25, .sky=12, .star=11};
embset eincylh = embset{.name="in cylinderH", .se=geom3::seCylinderH, .walls=1.2, .scale=M_PI/10, .depth=0, .eye=1.5, .sun=0.10472, .sky=2.6, .star=2.4};
embset eincylhe = embset{.name="in cylinderHE", .se=geom3::seCylinderHE, .walls=1.2, .scale=M_PI/10, .depth=0, .eye=1.5, .sun=0.10472, .sky=2.6, .star=2.4};
embset eincylnil = embset{.name="in cylinder Nil", .se=geom3::seCylinderNil, .walls=1.2, .scale=M_PI/10, .depth=0, .eye=1.5, .sun=0.10472, .sky=2.6, .star=2.4};
embset eincylsl = embset{.name="in cylinder SL(2,R)", .se=geom3::seCylinderSL2, .walls=1.2, .scale=M_PI/10, .depth=0, .eye=1.5, .sun=0.10472, .sky=2.6, .star=2.4};
embset einhorocyl = embset{.name="in horocylinder", .se=geom3::seCylinderHoro, .walls=-1.2, .scale=M_PI/10, .depth=0, .eye=1.5, .sun=0.10472, .sky=8, .star=7.5};
embset eouthorocyl = embset{.name="out horocylinder", .se=geom3::seCylinderHoro, .walls=1.2, .scale=M_PI/10, .depth=0, .eye=1.5, .sun=0.10472, .sky=4, .star=3.5};
embset eprodh_flat = embset{.name="hyperbolic product (flat)", .se=geom3::seProductH, .walls=1.2, .scale=M_PI/10, .depth=0, .eye=1.5, .sun=0.10472, .sky=2.6, .star=2.4};
embset eprodh_concave = embset{.name="hyperbolic product (concave)", .se=geom3::seProductH, .walls=1.2, .scale=M_PI/10, .depth=1, .eye=1.5, .sun=0.10472, .sky=2.6, .star=2.4};
embset eprods_flat = embset{.name="spherical product (flat)", .se=geom3::seProductS, .walls=1.2, .scale=M_PI/10, .depth=0, .eye=1.5, .sun=0.10472, .sky=2.6, .star=2.4};
embset eprods_concave = embset{.name="spherical product (concave)", .se=geom3::seProductS, .walls=1.2, .scale=M_PI/10, .depth=0.8333, .eye=1.5, .sun=0.10472, .sky=2.6, .star=2.4};

embset& edok() { return vid.wall_height > 0 ? edefault : edefaulti; }

vector<embset> embsets = {
  edefault, 
  eincyl, eoutcyl, eouthoro, einhoro, einhoro_small, esolv, eclifford, einnih, eoutnih,
  enil, esl2, eincylh, eincylhe, eincylnil, eincylsl, 
  einhorocyl, eouthorocyl, eprodh_flat, eprodh_concave, eprods_flat, eprods_concave
  };

ld fix_chess = 0;

int draw_digger = 0;

bool move_cat = false;

vector<string> xmap = {
  {"......B..........p#"},
  {".................p#"},
  {"..xxxxxxxx.......p#"},
  {"..xxxxxxxx..T....p#"},
  {"..xxxxxxxx.....T.p#"},
  {"..xxxxxxxx...~~..p#"},
  {"..xxxxxxxx..~uv~.p#"},
  {"..xxxxxxxx..~vu~.p#"},
  {"..xxxxxxxx..~~~~.p#"},
  {"..xxxxxxxx......Tp#"},
  {".............T...p#"},
  {"......C..........p#"},
  {"......p..........p#"},
  {".>>>..p..DDDDDDD.p#"},
  {".>>>..pK.DddddrD.p#"},
  {".>>>..pK.DdddtdD.p#"},
  {"......pK.DdddddD.p#"},
  {"..e...p..DOD+DOD.p#"},
  {"......p.....p....p#"},
  {"....^.pppppppppppp#"},
  };

void build_map();

ld hmul = 1;

set<cell*> house1, house2;

vector<cell*> ac, bac;

using namespace rogueviz::objmodels;

void prepare_tf();

hyperpoint adjust(hyperpoint h) { 
  h = cspin90(1, 2) * h * 40. + point3(-0.5, -0.5, 0);
  return h;
  }

hyperpoint rechess(hyperpoint h) {
  h[2] = lerp(cgi.FLOOR, cgi.WALL, 0.01-h[2]/4 * hmul * 2 / 0.6) + fix_chess;
  if(GDIM == 3) h = cgi.emb->logical_to_actual(h);
  else h[2] = 1;
  return h;
  }

struct split_model_data : gi_extension {
  map<pair<int, int>, vector<shared_ptr<object>>> objs_at;
  void render(const shiftmatrix& V, int x, int y);
  };

struct chessmodel : public model {

  void process_triangle(vector<hyperpoint>& hys, vector<hyperpoint>& tot, bool textured, object *co) override {

    for(auto& h: hys) h = adjust(h);

    hyperpoint norm = (hys[1] - hys[0]) ^ (hys[2] - hys[0]);
    norm /= hypot_d(3, norm);
    ld y = .5 + (.2 * norm[0] + .16 * norm[1] + .14 * norm[2]);
    glvertex shade = glhr::makevertex(0, y, 0);
    glvertex shadecol = glhr::makevertex(y, y, y);

    auto elen = [] (hyperpoint a, hyperpoint b) { return ceil(hypot_d(3, a-b) * 5); };

    int parts = max( max(elen(hys[0], hys[1]), elen(hys[0], hys[2])), elen(hys[1], hys[1]) );
    // if(parts > 8) println(hlog, "parts = ", parts, " levels = ", tie(hys[0][2], hys[1][2], hys[2][2]), " .. ", hys);
    // if(parts > 8) return;

    auto tri = [&] (int a, int b) {
      cgi.hpcpush(rechess(hys[0] + (hys[1] - hys[0]) * a / parts + (hys[2] - hys[0]) * b / parts));
      if(textured) {
        co->tv.tvertices.push_back(glhr::pointtogl(tot[0] + (tot[1] - tot[0]) * a / parts + (tot[2] - tot[0]) * b / parts));
        co->tv.colors.push_back(shadecol);
        }
      else {
        co->tv.tvertices.push_back(shade);
        }
      };

    for(int a=0; a<parts; a++)
    for(int b=0; b<parts-a; b++) {
      tri(a, b);
      tri(a+1, b);
      tri(a, b+1);
      if(a+b < parts-1) {
        tri(a, b+1);
        tri(a+1, b);
        tri(a+1, b+1);
        }
      }
    }

  chessmodel(string a, string b) : model(a,b) {}

  void split(model_data& md, split_model_data& smd);

  split_model_data& get_split() {
    auto& md = (unique_ptr<split_model_data>&) cgi.ext[fname + "-SPLIT"];
  
    if(!md) {
      md = std::make_unique<split_model_data>();
      auto& md0 = get();
      split(md0, *md);
      }
  
    return *md;
    }
  };  

bool ispiece(color_t col) {
  return among(col, color_t(0xFFFFFFFF), color_t(0x202020FF), color_t(0x2B2B2BFF));
  }

void chessmodel::split(model_data& md, split_model_data& smd) {
  cgi.extra_vertices();
  map<pair<int, int>, map<color_t, vector<pair<hyperpoint, glvertex>>>> mm;
  for(auto& obj: md.objs) {
    for(int i=obj->sh.s; i<obj->sh.e; i+=3) {
      hyperpoint ctr = Hypc;
      array<hyperpoint, 3> hs;
      for(int j=0; j<3; j++) hs[j] = GDIM == 2 ? cgi.hpc[i+j] : cgi.emb->actual_to_logical(cgi.hpc[i+j]);
      ctr += hs[0];
      ctr += hs[1];
      ctr += hs[2];
      ctr /= 3;
      int x = floor(ctr[0] + .5);
      int y = floor(ctr[1] + .5);
      int ax = x + 6, ay = y + 6;
      if(chess_moves >= 1 && ispiece(obj->color)) {
        if(ax == 6) { if(ay == 8) ay = 6; else if(ay == 6) ay = 8; }
        }
      if(chess_moves >= 2 && ispiece(obj->color)) {
        if(ax == 6) { if(ay == 3) ay = 5; else if(ay == 5) ay = 3; }
        }
      if(chess_moves >= 3 && ispiece(obj->color)) {
        if(ax == 8) { if(ay == 9) ay = 7, ax = 7; }
        }
      for(int j=0; j<3; j++)
        mm[{ax,ay}][obj->color].emplace_back(hs[j] - hyperpoint(x, y, 0, 0), obj->tv.tvertices[i+j-obj->sh.s]);
      }
    }
  for(auto& a: mm)
  for(auto& b: a.second) {
    auto& objs = smd.objs_at[a.first];
    objs.push_back(make_shared<object>());
    auto co = &*objs.back();
    cgi.bshape(co->sh, PPR::THORNS);
    cgi.last->flags |= POLY_TRIANGLES;
    cgi.last->texture_offset = 0;
    cgi.last->tinf = &co->tv;
    co->tv.texture_id = floor_textures->renderedTexture;  
    if(GDIM == 2) cgi.last->tinf = nullptr;
    co->color = b.first;
    if(co->color == 0xCCCCCCFF) co->color = GDIM == 2 ? 0 : 0xFFE080FF;
    else if(co->color == 0x5B5B5BFF) co->color = GDIM == 2 ? 0 : 0x807020FF;
    // else for(auto& v: b.second) v.first = cspin180(0, 1) * v.first;
    for(auto v: b.second) {
      cgi.hpcpush(GDIM == 2 ? v.first : cgi.emb->logical_to_actual(v.first));
      co->tv.tvertices.push_back(v.second);
      }
    cgi.finishshape();
    }
  cgi.extra_vertices();
  if(0) for(auto& p: smd.objs_at) {
    println(hlog, p.first, " : ", isize(p.second), " objects");
    for(auto& obj: p.second) {
      vector<hyperpoint> hs;
      for(int i=obj->sh.s; i<obj->sh.e; i++) hs.push_back(cgi.hpc[i]);
      if(isize(hs) > 3) hs.resize(3);
      println(hlog, "vertices: ", obj->sh.e - obj->sh.s, " of color ", obj->color, " : ", hs);
      }
    }
  }

void split_model_data::render(const shiftmatrix& V, int x, int y) {
  for(auto& obj: objs_at[{x, y}]) if(obj->color) {
    queuepoly(V, obj->sh, obj->color);
    }
  }

chessmodel chess("rogueviz/models/", "Polyfjord_Chess_Set.obj");

struct bunnymodel : public model {
  hyperpoint transform(hyperpoint h) override { 
    h = cspin90(1, 2) * h;
    h = cspin(0, 1, 15._deg) * h;
    h[0] *= 800;
    h[1] *= 800;
    h[2] = lerp(cgi.FLOOR, cgi.WALL, ilerp(-0.000333099, -0.00186996, h[2]) * hmul * 4/3.);
    if(GDIM == 3) h = cgi.emb->logical_to_actual(h);
    else h[2] = 1;
    return h;
    }  
  bunnymodel(string a, string b) : model(a,b) {}
  };

bunnymodel bunny("rogueviz/models/", "bunny.obj");

struct catmodel : public model {
  hyperpoint transform(hyperpoint h) override { 

    if(0) println(hlog, "scale = 1 to ", 
      lerp(cgi.FLOOR, cgi.WALL, ilerp(0.00767046, -0.184471, 1) * hmul * 4/3.)
    - lerp(cgi.FLOOR, cgi.WALL, ilerp(0.00767046, -0.184471, 0) * hmul * 4/3.)
      );

    h = cspin90(1, 2) * h;
    h = cspin180(0, 1) * h;
    h = cspin(1, 0, 30._deg) * h;
    h[0] *= 6;
    h[1] *= 6;
    h[2] = lerp(cgi.FLOOR, cgi.WALL, ilerp(0.00767046, -0.184471, h[2]) * hmul * 4/3.) / 8.3272 * 6;
    if(GDIM == 3) h = cgi.emb->logical_to_actual(h);
    else h[2] = 1;
    return h;
    }  
  void load_obj(model_data& md) override {
    model::load_obj(md);
    for(auto& o: md.objs) if(o->mtlname == "whiskers") o->sh.prio = PPR::TRANSPARENT_WALL;
    }
  catmodel(string a, string b) : model(a,b) {}
  };

catmodel maxwellcat("rogueviz/models/", "maxwellcat.obj");

struct tulipmodel : public model {
  hyperpoint transform(hyperpoint h) override { 
    h = cspin90(1, 2) * h;
    h = cspin180(0, 1) * h;
    h[0] *= 400;
    h[1] *= 400;
    h[0] -= 0.1*2; h[1] += 0.36*2;
    h[2] = lerp(cgi.FLOOR, cgi.WALL, ilerp(0, -0.00307639, h[2]) * hmul);
    if(GDIM == 3) h = cgi.emb->logical_to_actual(h);
    else h[2] = 1;
    return h;
    }  
  tulipmodel(string a, string b) : model(a,b) {}
  };

tulipmodel tulip("rogueviz/models/", "tulip.obj");
tulipmodel tulip1("rogueviz/models/", "tulip1.obj");
tulipmodel tulip2("rogueviz/models/", "tulip2.obj");

struct diggermodel : public model {
  hyperpoint transform(hyperpoint h) override { 
    h = cspin90(1, 2) * h;
    h[0] *= 50;
    h[1] *= 50;
    /* h[0] *= 400;
    h[1] *= 400;
    h[0] -= 0.1*2; h[1] += 0.36*2; */
    h[2] = lerp(cgi.FLOOR, cgi.WALL, ilerp(0.00027, -0.0549454, h[2]) * hmul * 1.5);
    if(GDIM == 3) h = cgi.emb->logical_to_actual(h);
    else h[2] = 1;
    return h;
    }  
  diggermodel(string a, string b) : model(a,b) {}
  };

diggermodel digger("rogueviz/models/", "digger.obj");

struct tablemodel : public model {
  hyperpoint transform(hyperpoint h) override { 
    h = cspin90(1, 2) * h;
    h[1] -= 0.005;
    h[0] *= 100;
    h[1] *= 100;
    /* h[0] *= 400;
    h[1] *= 400;
    h[0] -= 0.1*2; h[1] += 0.36*2; */
    h[2] = lerp(cgi.FLOOR, cgi.WALL, ilerp(0, -0.008, h[2]) * hmul / 2);
    if(GDIM == 3) h = cgi.emb->logical_to_actual(h);
    else h[2] = 1;
    return h;
    }  
  tablemodel(string a, string b) : model(a,b) {}
  };

tablemodel table("rogueviz/models/", "table.obj");

struct cheesemodel : public model {
  hyperpoint transform(hyperpoint h) override { 
    h = cspin90(1, 2) * h;
    h[0] *= 15;
    h[1] *= 15;
    /* h[0] *= 400;
    h[1] *= 400;
    h[0] -= 0.1*2; h[1] += 0.36*2; */
    h[0] -= 0.2;
    h[2] = lerp(cgi.FLOOR, cgi.WALL, (ilerp(0.00577916, -0.01166, h[2])/3 + 1.05) * hmul / 2);
    if(GDIM == 3) h = cgi.emb->logical_to_actual(h);
    else h[2] = 1;
    return h;
    }  
  cheesemodel(string a, string b) : model(a,b) {}
  };

cheesemodel cheese("rogueviz/models/", "cheese.obj");

struct coffeemodel : public model {
  int cid;
  hyperpoint transform(hyperpoint h) override { 
    h = cspin90(1, 2) * h;
    h[0] += 0.00077;
    h[1] += 0.011;
    h[0] *= 200; h[1] *= 200;
    if(cid == 1) h[0] += 0.3;
    if(cid == 2) h[1] += 0.5;
    h[2] = lerp(cgi.FLOOR, cgi.WALL, (ilerp(-0.0083, -0.0089, h[2])) * 1.2 / 10 + .51);
    if(GDIM == 3) h = cgi.emb->logical_to_actual(h);
    else h[2] = 1;
    return h;
    }  
  coffeemodel(string a, string b, int _id) : model(a,b), cid(_id) {}
  };

/* need different file names */
coffeemodel coffee1("rogueviz/models/", "coffee.obj", 1), coffee2("rogueviz/models/", "./coffee.obj", 2);

struct lilymodel : public model {
  hyperpoint transform(hyperpoint h) override { 
    h = cspin90(1, 2) * h;
    h[0] += 0.67; h[1] -= 0.36; 
    h[0] *= 5;
    h[1] *= 5;
    h[2] = lerp(cgi.LAKE, cgi.FLOOR, ilerp(0.018, -0.0496, h[2]) * hmul);
    if(GDIM == 3) h = cgi.emb->logical_to_actual(h);
    else h[2] = 1;
    return h;
    }  
  lilymodel(string a, string b) : model(a,b) {}
  };

lilymodel lily("rogueviz/models/", "lily.obj");

struct duckmodel : public model {
  hyperpoint transform(hyperpoint h) override { 
    h = cspin90(1, 2) * h;
    h[0] += 3.8; h[1] -= 2;
    h[2] = lerp(cgi.LAKE, cgi.FLOOR, ilerp(0.056, -0.408, h[2]) * hmul * 1.5);
    if(GDIM == 3) h = cgi.emb->logical_to_actual(h);
    else h[2] = 1;
    return h;
    }  
  duckmodel(string a, string b) : model(a,b) {}
  };

duckmodel duck("rogueviz/models/", "duck.obj");

struct ratmodel : public model {
  hyperpoint transform(hyperpoint h) override { 
    h = cspin90(1, 2) * h;
    h[0] *= 100;
    h[1] *= 100;
    h[2] *= 100;
    h[0] /= 3 * 3;
    h[1] /= 3 * 3;
    h[2] /= 3 * 3;
    h = cspin(0, 1, 135._deg) * h;

    h[2] = lerp(cgi.FLOOR, cgi.WALL, ilerp(1/9., 1/9. - 1.2, h[2]) * hmul);
    if(GDIM == 3) h = cgi.emb->logical_to_actual(h);
    else h[2] = 1;
    return h;
    }  
  ratmodel(string a, string b) : model(a,b) {}
  };

ratmodel rat("rogueviz/models/", "spinning-rat.obj");

bool hedge_constructed = false;

const int hx = 7;
array<char, hx*hx*hx> hedge_visited;
array<int, 6> hdirs = {1, hx, hx*hx, -1, -hx, -hx*hx};

array<vector<int>, hx*hx*hx> hedge_dirs;

int hstart;

void construct_hedge() {
  for(int x=0; x<hx*hx*hx; x++) hedge_visited[x] = false;
  hstart = (hx/2)+(hx/2)*hx;
  hedge_visited[hstart] = true;
  vector<int> inorder = {hstart};
  for(int i=0; i<hx*hx*hx; i++) {
    int at = inorder[i];
    for(int dir: {0, 1, 3, 4, 2, 5}) {
      int co = (at / abs(hdirs[dir])) % hx;
      if(hdirs[dir] < 0 && co == 0) continue;
      if(hdirs[dir] > 0 && co == hx-1) continue;
      if(hedge_visited[at+hdirs[dir]]) continue;
      hedge_visited[at+hdirs[dir]] = true;
      hedge_dirs[at].push_back(dir);
      inorder.push_back(at+hdirs[dir]);
      }
    }
  /*
  int qty = hx*hx*hx-1;
  while(qty > 0) {
    int at = hrand(hx*hx*hx);
    if(!hedge_visited[at]) continue;
    int dir = hrand(6);
    int co = (at / abs(hdirs[dir])) % hx;
    if(hdirs[dir] < 0 && co == 0) continue;
    if(hdirs[dir] > 0 && co == hx-1) continue;
    if(hedge_visited[at+hdirs[dir]]) continue;
    hedge_visited[at+hdirs[dir]] = true;
    hedge_dirs[at].push_back(dir);
    qty--;
    } */
  }

void add_oct(transmatrix T, int idx, int face, ld len) {
  array<hyperpoint, 3> p;
  for(int i=0; i<3; i++) { p[i] = C03; p[i][i] = ((face>>i)&1) ? len/2 : -len/2; p[i] = cgi.emb->logical_scaled_to_intermediate * p[i]; }
  hyperpoint c = (p[0] + p[1] + p[2]) / 3;
  for(int f=0; f<3; f++) {
    hyperpoint a = p[f] - c;
    hyperpoint b = p[(f+1)%3] - c;
    texture_order([&] (ld x, ld y) {
      hyperpoint h = c + a * x + b * y;
      hyperpoint t = T * cpush(0, h[0]) * cpush(1, h[1]) * cpush(2, h[2]) * C0;
      cgi.hpcpush(t);
      });
    }
  if(true) for(auto d: hedge_dirs[idx]) {
    hyperpoint h = C0;
    h[d%3] = (d<3 ? -len: len);
    h = cgi.emb->logical_scaled_to_intermediate * h;
    transmatrix T1 = T;
    for(int i=0; i<3; i++) if(h[i]) T1 = T1 * cpush(i, h[i]);
    add_oct(T1, idx + hdirs[d], face, len);
    }
  }

void create_hedge(model_data& md) {
  if(!hedge_constructed) construct_hedge();
  hyperpoint start = cgi.emb->logical_to_actual(point31(0, 0, cgi.FLOOR));
  hyperpoint npt = cgi.emb->logical_to_actual(point31(0.01, 0, cgi.FLOOR));
  transmatrix S = cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point31(0, 0, cgi.emb->center_z() + cgi.FLOOR));
  ld len = hdist(start, npt) * 100 / 7;
  len *= 3;
  if(vid.wall_height < 0) len = -len;
  md.objs.resize(8);
  for(int i=0; i<8; i++) {
    md.objs[i] = make_shared<object> ();
    auto& obj = *md.objs[i];
    obj.color = 0xFF00FF;
    if(i & 1) obj.color ^= 0x200000;
    if(i & 2) obj.color ^= 0x20000000;
    if(i & 4) obj.color ^= 0x2000;
    cgi.bshape(obj.sh, PPR::FLOOR_DRAGON);
    cgi.last->flags |= POLY_TRIANGLES;
    cgi.last->texture_offset = 0;
    cgi.last->tinf = &obj.tv;
    obj.tv.texture_id = floor_textures->renderedTexture;
    add_oct(S * lzpush(-len/2), hstart, i, len);
    cgi.finishshape();
    bind_floor_texture(obj.sh, cgi.shFeatherFloor.id);
    }
  cgi.extra_vertices();
  }

void draw_hedge(const shiftmatrix& V) {
  auto& md = (unique_ptr<model_data>&) cgi.ext["hedge"];
  if(!md) {
    md = std::make_unique<model_data>();
    create_hedge(*md);
    }
  md->render(V);  
  }

hyperpoint inverse_exp_newton(hyperpoint h, int iter) {
  auto approx = inverse_exp(shiftless(h));
  for(int i=0; i<iter; i++) {
    transmatrix T;
    ld eps = 1e-3;
    hyperpoint cur = direct_exp(approx);
    println(hlog, approx, " error = ", hdist(cur, h), " iteration ", i, "/", iter);
    for(int i=0; i<3; i++)
      set_column(T, i, direct_exp(approx + ctangent(i, eps)) - h);
    set_column(T, 3, C03);
    approx = approx - inverse(T) * (cur - h) * eps;
    }
  return approx;
  }

hpcshape& get_noniso_pipe(hyperpoint target, ld width, ePipeEnd endtype) {
  int id = bucketer(target)  + int(157003 * log(width+.001));
  if(cgi.shPipe.count(id)) return cgi.shPipe[id];
  hpcshape& pipe = cgi.shPipe[id];
  println(hlog, "generating pipe at target ", target, " and width ", width);
  cgi.bshape(pipe, PPR::HEPTAMARK);

#if CAP_GL
  auto& utt = cgi.models_texture;
  if(floor_textures) {
    pipe.tinf = &utt;
    pipe.texture_offset = isize(utt.tvertices);
    }
#endif

  hyperpoint lmax = inverse_exp_newton(target, 10);
  println(hlog, "error = ", hdist(direct_exp(lmax), target));
  transmatrix lT;
  ld length;
  if(1) {
    dynamicval<eGeometry> g(geometry, gCubeTiling);
    length = hdist0(lmax);
    lT = rspintox(lmax);
    println(hlog, "target = ", target, " lmax = ", lmax, " length = ", length, " lT = ", kz(lT));
    println(hlog, "test: ", lT * xpush0(length), " vs ", lmax);
    }

  const int MAX_X = 32;
  const int MAX_R = 20;
  auto at = [&] (ld i, ld a, ld z = 1, ld s = 1) {
    a += 0.5;
    ld alpha = TAU * a / MAX_R;
    hyperpoint p;
    if(1) {
      dynamicval<eGeometry> g(geometry, gCubeTiling);
      p = xpush(i * length / MAX_X) * cspin(1, 2, alpha) * ypush0(width*z);
      p = lT * p;
      }
    p = direct_exp(p);
    cgi.hpcpush(p);
    #if CAP_GL
    if(floor_textures) utt.tvertices.push_back(glhr::makevertex(0, true ? 0.549 - s * 0.45 * sin(alpha) : 0.999, 0));
    #endif
    };
  for(int i=0; i<MAX_X; i++) {
    for(int a=0; a<MAX_R; a++) {
      at(i, a, 1);
      at(i, a+1, 1);
      at(i+1, a, 1);
      at(i+1, a+1, 1);
      at(i+1, a, 1);
      at(i, a+1, 1);
      }
    }

  if(endtype == ePipeEnd::sharp) for(int a=0; a<MAX_R; a++) for(int x: {0, MAX_X}) {
    at(x, a, 1, 0);
    at(x, a+1, 1, 0);
    at(x, 0, 0, 0);
    }

  if(endtype == ePipeEnd::ball) for(int a=0; a<MAX_R; a++) for(int x=-MAX_R; x<MAX_R; x++) {
    ld xb = x < 0 ? 0 : MAX_X;
    ld mul = MAX_X * width/length * .9; // .9 to prevent Z-fighting
    ld x0 = xb + mul * sin(x * 90._deg / MAX_R);
    ld x1 = xb + mul * sin((x+1) * 90._deg / MAX_R);
    ld z0 = cos(x * 90._deg / MAX_R);
    ld z1 = cos((x+1) * 90._deg / MAX_R);
    at(x0, a, z0, z0);
    at(x0, a+1, z0, z0);
    at(x1, a, z1, z1);
    at(x1, a+1, z1, z1);
    at(x1, a, z1, z1);
    at(x0, a+1, z0, z0);
    }

  cgi.last->flags |= POLY_TRIANGLES | POLY_PRINTABLE;
  cgi.finishshape();
  cgi.extra_vertices();
  return pipe;
  }

void fat_line(const shiftmatrix& V1, const hyperpoint h1, const shiftmatrix& V2, const hyperpoint h2, color_t col, int prec, ld lw) {
  if(nonisotropic) {
    auto nV1 = V1 * rgpushxto0(h1);
    hyperpoint U2 = inverse_shift(nV1, V2*rgpushxto0(h2)) * C0;
    auto& p = get_noniso_pipe(U2, lw, ePipeEnd::ball);
    queuepoly(nV1, p, col);
    return;
    }

  ld d = hdist(V1.T*h1, V2.T*h2);

  shiftmatrix T = V1 * rgpushxto0(h1);
  transmatrix S = rspintox(inverse_shift(T, V2) * h2);
  transmatrix U = rspintoc(inverse_shift(T*S, shiftless(C0)), 2, 1);
  auto& p = queuepoly(T * S * U, cgi.generate_pipe(d, lw, ePipeEnd::ball), col);
  p.intester = xpush0(d/2);
  }

int scaffoldx = 0, scaffoldy = 0, scaffoldb = 0, scaffoldx_move = 0, scaffoldy_move;

template<class T> void draw_map54(const shiftmatrix& S, const T& f, color_t col) {
  for(auto p: map54_nodes) queuepoly(S * rgpushxto0(f(p)), cgi.shSnowball, 0xFFFFFFFF);
  for(auto& p: map54_edges) fat_line(S, f(p.first), S, f(p.second), (abs(p.first[1])<1e-3 && abs(p.second[1])<1e-3 && scaffoldx && scaffoldy) ? 0xFFFFFFFF : col, 2, 0.01);
  }

void draw_scaffold(const shiftmatrix& V) {
  ld levz = log(2);
  if(nih) levz = 1;
  ld levx = 1;
  if(hyperbolic) levx = exp(-cgi.FLOOR);
  levx *= 5;

  if(scaffoldx == 1) 
  for(int z=-5; z<=5; z++)
  for(int s=-20; s<=20; s++) {
    shiftmatrix S = V * lzpush(cgi.FLOOR + z * levz) * cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point3(s*levx,0,0));
    queuepoly(S, cgi.shSnowball, 0xFFFFFFFF);
    fat_line(S, C0, S, lzpush(-levz) * C0, (s == 0 && scaffoldy) ? 0xFFFFFFFF : 0xFF0000FF, 2, 0.01);
    for(int s=0; s<8; s++)
      fat_line(S, cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point3(levx*(s+0)/8.,0,0)) * C0, 
               S, cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point3(levx*(s+1)/8.,0,0)) * C0, 
               0x8000FFFF, 2, 0.01);
    }

  if(scaffoldx == 2) {
    if(hyperbolic) {
      draw_map54(V, [] (hyperpoint h) { return cgi.emb->intermediate_to_actual_translation(point3(h[1],0,0)) * zpush(h[0]) * C0; }, 0xFFD500FF);
      }
    if(sol) {
      draw_map54(V, [] (hyperpoint h) { return cgi.emb->intermediate_to_actual_translation(point3(h[1],0,0)) * zpush(-h[0]) * C0; }, 0xFFD500FF);
      }
    if(nih) {
      draw_map54(V, [] (hyperpoint h) { return cgi.emb->intermediate_to_actual_translation(point3(-h[1]/log(2),0,0)) * zpush(h[0]/log(2)) * C0; }, 0xFFD500FF);
      }
    }

  ld levy = 5;
  if(scaffoldy == 1)
  for(int z=-5; z<=5; z++)
  for(int s=-20; s<=20; s++) {
    shiftmatrix S = V * lzpush(cgi.FLOOR + z * levz) * cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point3(0,s*levy,0));
    queuepoly(S, cgi.shSnowball, 0xFFFFFFFF);
    fat_line(S, C0, S, lzpush(sol ? levz : -levz) * C0, (s == 0 && scaffoldx) ? 0xFFFFFFFF : 0x0000FFFF, 2, 0.01);
    for(int s=0; s<8; s++)
      fat_line(S, cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point3(0, levy*(s+0)/8.,0)) * C0, 
               S, cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point3(0, levy*(s+1)/8.,0)) * C0, 
               0x0080FF, 2, 0.01);
    }

  if(scaffoldy == 2) {
    if(hyperbolic) {
      draw_map54(V, [] (hyperpoint h) { return cgi.emb->intermediate_to_actual_translation(point3(0,h[1],0)) * zpush(h[0]) * C0; }, 0x80D500FF);
      }
    if(sol) {
      draw_map54(V, [] (hyperpoint h) { return cgi.emb->intermediate_to_actual_translation(point3(0,h[1],0)) * zpush(h[0]) * C0; }, 0x80D500FF);
      }
    if(nih) {
      draw_map54(V, [] (hyperpoint h) { return cgi.emb->intermediate_to_actual_translation(point3(0,h[1]/log(3),0)) * zpush(h[0]/log(3)) * C0; }, 0x80D500FF);
      }
    }

  if(scaffoldb == 1)
  for(int z=-5; z<=5; z++)
  for(int sx=-5; sx<=5; sx++)
  for(int sy=-5; sy<=5; sy++) {
    shiftmatrix S = V * lzpush(cgi.FLOOR + z * levz) * cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point3(sx*levx,sy*levy,0));
    queuepoly(S, cgi.shSnowball, 0xFFFFFFFF);
    fat_line(S, C0, S, lzpush(-levz) * C0, 0xFF0000FF, 2, 0.01);
    for(int s=0; s<8; s++) {
      fat_line(S, cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point3(levx*(s+0)/8.,0,0)) * C0, 
               S, cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point3(levx*(s+1)/8.,0,0)) * C0, 
               0xFF8000FF, 2, 0.01);
      fat_line(S, cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point3(0,levy*(s+0)/8.,0)) * C0, 
               S, cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point3(0,levy*(s+1)/8.,0)) * C0, 
               0xFF8000FF, 2, 0.01);
      }
    }

  if(scaffoldb == 2 && hyperbolic) {
    for(auto h: map534_nodes) queuepoly(V * rgpushxto0(h), cgi.shSnowball, 0xFFFFFFFF);
    for(auto h: map534_edges) fat_line(V, h.first, V, h.second, 0xFFFFFFFF, 2, 0.01);
    }
  }

string fixed_random_digits = "1100002102001211";

void draw_shape(const shiftmatrix& V, vector<ld> tab, color_t col, ld mul = 1) {
  int n = isize(tab);
  for(int i=0; i<n; i+=2) curvepoint(hpxy(tab[i]*mul, tab[i+1]*mul));
  curvepoint(hpxy(tab[0]*mul, tab[1]*mul));
  queuecurve(V, 0xFF, col, PPR::MONSTER_LEG);
  }

bool draw_chess_at(cell *c, const shiftmatrix& V) {
  if(c->item ==	itHyperstone) c->item = itNone;
  if(false) {
    if(c == cwt.at) chess.render(V);
    }
  else {
    auto co = euc::full_coords2(c);
    char& chr = xmap[gmod(co.second, 20)][gmod(co.first, 20)];
    char ch = chr;
    if(no_floor && ch != '^') ch = '<';

    // if(co.first == 0 && co.second == 0) bunny.render(V);
    switch(ch) {
      case 'r':
        if(GDIM == 3 && with_models) {
          rat.render(V);
          }
        else {
          drawMonsterType(moMouse, c, V * spin(-135._deg), 0x804000, 0, 0x804000);
          }
        break;
      case '<':
        c->wall = waChasm;
        house1.erase(c); house2.erase(c);
        fat_line(V, cgi.emb->logical_to_actual(point31(0.5, 0.5, 0)), V, cgi.emb->logical_to_actual(point31(0.5, -0.5, 0)), 0xFFFFFFFF, 2, 0.001);
        fat_line(V, cgi.emb->logical_to_actual(point31(0.5, 0.5, 0)), V, cgi.emb->logical_to_actual(point31(-0.5, 0.5, 0)), 0xFFFFFFFF, 2, 0.001);
        break;
      case 'u': {
        if(!cgtrader_models) break;
        if(GDIM == 3 && with_models) {
          lily.render(V);
          c->item = itNone;
          }
        else c->item = itWet;
        break;
        }
      case 'v': {
        if(!cgtrader_models) break;
        if(GDIM == 3 && with_models) duck.render(V);
        else {
          draw_shape(V, {-0.164988, 0.0033671, -0.114197, 0.0638159, 0.0401905, 0.063635, 0.10735, 0.0268375, 0.10735, -0.0268375, 0.0401905, -0.063635, -0.114197, -0.0638159, -0.164988, -0.0033671, }, 0xC0C0C0FF, 2);
          draw_shape(V, {-0.0753584, 0.00669852, -0.111558, 0.0184533, -0.0812589, 0.0326711, -0.161669, 0.0479953, -0.10074, 0.0780737, -0.0184331, 0.0896519, 0.0830064, 0.065399, 0.113269, 0.0335611, 0.117466, 0.0151028, 0.117466, -0.0151028, 0.113269, -0.0335611, 0.0830064, -0.065399, -0.0184331, -0.0896519, -0.10074, -0.0780737, -0.161669, -0.0479953, -0.0812589, -0.0326711, -0.111558, -0.0184533, -0.0753584, -0.00669852, }, 0x806010FF, 2);
          draw_shape(V, {0.164139, 0.0109426, 0.200142, 0.0185786, 0.239985, 0.00848005, 0.239985, -0.00848005, 0.200142, -0.0185786, 0.164139, -0.0109426, }, 0xE0E000FF, 2);
          draw_shape(V, {0.0820785, 0.00670028, 0.0963908, 0.0243072, 0.121722, 0.0352575, 0.159071, 0.0361908, 0.183838, 0.028672, 0.199268, 0.00844354, 0.199268, -0.00844354, 0.183838, -0.028672, 0.159071, -0.0361908, 0.121722, -0.0352575, 0.0963908, -0.0243072, 0.0820785, -0.00670}, 0x106010FF, 2);
          }
        break;
        }
      case 'B':
        if(GDIM == 3 && with_models) bunny.render(V);
        else {
          auto V1 = V * spin270();
          draw_shape(V1, {-0.272437, 0.00681093, -0.247911, 0.0339604, -0.199391, 0.0506926, -0.165034, 0.0336804, -0.151363, 0.0100908, -0.151363, -0.0100908, -0.165034, -0.0336804, -0.199391, -0.0506926, -0.247911, -0.0339604, -0.272437, -0.00681093, }, 0xFFFFFFFF, 2);
          draw_shape(V1, {-0.209613, 0.00676172, -0.161797, 0.0741567, -0.107602, 0.100877, 0.0033535, 0.103959, 0.100724, 0.0738645, 0.134426, 0.0369672, 0.14796, 0.00672545, 0.14796, -0.00672545, 0.134426, -0.0369672, 0.100724, -0.0738645, 0.0033535, -0.103959, -0.107602, -0.100877, -0.161797, -0.0741567, -0.209613, -0.00676172, }, 0xC0C0C0FF, 2);
          draw_shape(V1, {0.0502023, 0.0167341, 0.0737312, 0.0536227, 0.151647, 0.0876183, 0.240969, 0.0441211, 0.272441, 0.0102165, 0.272441, -0.0102165, 0.240969, -0.0441211, 0.151647, -0.0876183, 0.0737312, -0.0536227, 0.0502023, -0.0167341, }, 0xD0D0D0FF, 2);
          draw_shape(V1, {0.103977, 0.0268328, 0.0267753, 0.0468568, -0.0536042, 0.063655, -0.00335237, 0.0972189, 0.094003, 0.080574, 0.144678, 0.0571982, 0.154836, 0.0437579, 0.134405, 0.0268809, }, 0xC0C0C0FF, 2);
          draw_shape(V1 * MirrorY, {0.103977, 0.0268328, 0.0267753, 0.0468568, -0.0536042, 0.063655, -0.00335237, 0.0972189, 0.094003, 0.080574, 0.144678, 0.0571982, 0.154836, 0.0437579, 0.134405, 0.0268809, }, 0xC0C0C0FF, 2);
          draw_shape(V1, {0.168425, 0.0235794, 0.178774, 0.0505965, 0.188982, 0.0269974, }, 0xC00000FF, 2);
          draw_shape(V1 * MirrorY, {0.168425, 0.0235794, 0.178774, 0.0505965, 0.188982, 0.0269974, }, 0xC00000FF, 2);
          }
        break;
      case 't':
        if(!cgtrader_models) break;
        if(GDIM == 2) c->wall = waRoundTable; else if(with_models && !light_models) {
          c->wall = waNone;
          table.render(V);
          cheese.render(V);
          coffee1.render(V);
          coffee2.render(V);
          }
        break;
      case 'C':
        if(GDIM == 3 && with_models) maxwellcat.render(V);
        else {
          dynamicval<bool> b(mapeditor::drawplayer, true);
          drawPlayer(moPlayer, c, V * spin90(), 0xFFFFFFFF, 0);
          }
        if(move_cat) {
          move_cat = false;
          mapeditor::drawplayer = true;
          chr = '.';
          cwt.at = c;
          vid.sspeed = -5;
          cwt.spin = 1;
          vid.axes = 0;
          }
        break;
      case 'e':
        if(!cgtrader_models) break;
        if(GDIM == 3 && draw_digger == 2) digger.render(V);
        break;
      case 'K': {
        if(!cgtrader_models) break;
        int a = gmod(co.first + co.second, 3);
        if(GDIM == 3 && with_models) {
          auto gtulip = [&] (int a, int b) -> tulipmodel& { char ch = fixed_random_digits[5*a+b]; if(ch == '0') return tulip; if(ch == '1') return tulip1; if(ch == '2') return tulip2; return tulip2; };
          gtulip(a, 0).render(V);
          gtulip(a, 1).render(V * cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point3(1/3., 1/3., 0)));
          gtulip(a, 2).render(V * cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point3(1/3., -1/3., 0)));
          gtulip(a, 3).render(V * cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point3(-1/3., 1/3., 0)));
          gtulip(a, 4).render(V * cgi.emb->intermediate_to_actual_translation(cgi.emb->logical_to_intermediate * point3(-1/3., -1/3., 0)));
          }
        else {
          vector<color_t> tcols = {0xFFFF80FF, 0xFF1010FF, 0xC000C0FF};
          draw_shape(V*spin90(), {-0.202738, 0.0236527, -0.145213, 0.135082, -0.0438177, 0.1719, 0.0708783, 0.178883, 0.220846, 0.125712, 0.120989, 0.0705769, 0.219991, 0.00338448, 0.219991, -0.00338448, 0.120989, -0.0705769, 0.220846, -0.125712, 0.0708783, -0.178883, -0.0438177, -0.1719, -0.145213, -0.135082, -0.202738, -0.0236527}, tcols[a], 2);
          }
        break;
        }
      case 'H':
        if(GDIM == 3) draw_hedge(V);
        else queuepoly(V, cgi.shGem[0], 0xFF00FF);
        break;
      case '^': {
        c->wall = no_floor ? waChasm : waNone;
        auto ax = co.first - scaffoldx_move * 20;
        auto ay = co.second - scaffoldy_move * 20;
        if(ax >= 0 && ay >= 0 && ax < 20 && ay < 20)
          draw_scaffold(V);
        break;
        }
      case '>':
        c->wall = draw_digger ? waChasm : waNone;
        break;
      }
    if(with_chess) chess.get_split().render(V, gmod(co.first, 20), gmod(co.second, 20));
    }
  return false;
  }

ld fov, otanfov;

void switch_fpp_fixed() {
  auto& cd = current_display;
  transmatrix tView;
  ld ys;

  tView = View;
  otanfov = (cd->xsize/2) / cd->radius * 2;
  auto d = vid.depth;

  if(GDIM == 3) invoke_embed(geom3::seNone);
  else {
    invoke_embed(geom3::seDefault);
    activate(edefault);
    }
  if(GDIM == 3) {
    ld tfov = vid.fov * degree / 2;
    cd->tanfov = tan(tfov);
    ld yshift = (otanfov - vid.depth) / cd->tanfov;
    println(hlog, "yshift = ", yshift, " from ", vid.yshift);
    View = Id;
    for(int a=0; a<2; a++)
    for(int b=0; b<2; b++)
      View[a][b] = tView[a][b];
    for(int a=0; a<2; a++)
      View[a][3] = tView[a][2];
    println(hlog, tie(tView[2][0], tView[2][1]));
    shift_view(zpush0(yshift)); 
    playermoved = false;
    }
  else {
    pconf.camera_angle = 0;
    vid.yshift = 0;
    ys = tView[2][3];
    println(hlog, "ys = ", ys, " from ", tView);
    otanfov = ys * cd->tanfov + d;
    cd->radius = cd->xsize / otanfov;
    pconf.scale = cd->radius /  cd->scrsize;
    }
  if(GDIM == 2) {
    View = spin90() * View;
    View[0][2] = tView[0][3];
    View[1][2] = tView[1][3];
    }
  }

void load_anim(string fname) {
  fhstream f(fname, "r");
  mapstream::cellbyid = ac;
  while(isize(mapstream::cellbyid) < 400 * 400) for(auto c: ac) mapstream::cellbyid.push_back(c);
  smoothcam::enable();
  smoothcam::load_animation(f);
  }

void append_anim(string fname) {
  smoothcam::backup();
  load_anim(fname);
  smoothcam::append_backup();
  }

void save_anim(string fname) {
  fhstream f(fname, "w");
  for(int i=0; i<isize(ac); i++) mapstream::cellids[ac[i]] = i;
  smoothcam::save_animation(f);
  }

#if CAP_VIDEO
void transition(ld a, ld b, int frames) {
  anims::noframes = frames;

  auto _edok = edok();
  auto cu = current();
  auto vn = glhr::vnear_default;
  auto vf = glhr::vfar_default;

  int lev = addHook(anims::hooks_anim, 100, [&] {
    ld t = ticks / anims::period;
    println(hlog, "transition, t = ", t);
    indenter ind(2);
    t = t * t * (3 - 2 * t);
    t = lerp(a, b, t);
    sightranges[geometry] = 50 * t;
    glhr::vnear_default = vn * t;
    glhr::vfar_default = vf * t;
    activate(emb_lerp(_edok, cu, t));
    });
  anims::record_video();
  delHook(anims::hooks_anim, lev);
  activate(cu);
  sightranges[geometry] = 50;
  glhr::vnear_default = vn;
  glhr::vfar_default = vf;
  }

void animate_forward(ld d, int frames) {
  anims::noframes = frames;

  ld lastt = 0;

  int lev = addHook(anims::hooks_anim, 100, [&] {
    println(hlog, "centerover is ", centerover);
    ld t = ticks / anims::period;
    t = t * t * (3 - 2 * t);
    shift_view(ztangent(-(t - lastt) * d));
    lastt = t;
    spinEdge(100);
    anims::moved();
    });
  anims::record_video();
  delHook(anims::hooks_anim, lev);
  }

void transition_test(ld t) {

  auto _edok = edok();
  auto cu = current();
  auto vn = glhr::vnear_default;
  auto vf = glhr::vfar_default;

  sightranges[geometry] = 50 * t;
  glhr::vnear_default = vn * t;
  glhr::vfar_default = vf * t;
  activate(emb_lerp(_edok, cu, t));
  }
#endif

bool adjust_to_period;

embset rescaled(const embset& e, ld p) {
  auto e1 = e;
  e1.scale /= p;
  e1.sky *= p;
  e1.star *= p;
  return e1;
  }

void embset_list() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("embset list"), 0xFFFFFFFF, 150, 0);
  dialog::start_list(900, 900, '1');
  for(auto& e: embsets) {
    dialog::addItem(e.name, dialog::list_fake_key++);
    dialog::add_action([&e] {
      activate(rescaled(e, periods));
      });
    }
  dialog::end_list();
  dialog::addBack();
  dialog::display();  
  }

void look_downwards() {
  transmatrix T = View;
  if(GDIM == 3) {
    for(int i=0; i<3; i++) T[i][3] = T[3][i] = 0;
    rotate_view(inverse(T));
    ld x = T[3][0], y = T[3][1];
    shift_view(point31(x+.5, y+.5, 0));
    }
  else {
    for(int i=0; i<2; i++) T[i][2] = T[2][i] = 0;
    rotate_view(inverse(T));
    }
  }

color_t domcol = 0xf8dba0;
color_t domcolf = 0x785b20;
color_t domroof = 0x802020;

color_t our_skycolor(cell *c) {
  auto co = euc::full_coords2(c);
  ld x = co.first * TAU / 20;
  ld y = co.second * TAU / 20;
  ld p = sin(x+2*y) + cos(3*x-y) + sin(x);
  return gradient(0x4040FF, 0xFFFFFF, -3, p, 3);
  }

bool chess_ceiling(celldrawer *cw) {
  auto co = euc::full_coords2(cw->c);
  if(co == gp::loc{0, 0}) 
    draw_star(cw->V, cgi.shSun, 0xFFFF00FF);
  if(house2.count(cw->c)) {
    color_t col = (domcol << 8) | 0xFF;
    color_t wcol1 = (gradient(0, domcol, 0, .9, 1) << 8) | 0xFF;
    color_t wcol2 = (gradient(0, domcol, 0, .8, 1) << 8) | 0xFF;
    if(house1.count(cw->c)) forCellIdEx(c2, i, cw->c) if(!house1.count(c2)) {
      placeSidewall(cw->c, i, SIDE_HIGH, cw->V, (i&1)?wcol1:wcol2); 
      placeSidewall(cw->c, i, SIDE_HIGH2, cw->V, (i&1)?wcol1:wcol2); 
      }
    forCellIdEx(c2, i, cw->c) if(!house2.count(c2)) {
      placeSidewall(cw->c, i, SIDE_HIGH2, cw->V, (i&1)?wcol1:wcol2); 
      }
    if(house1.count(cw->c) != house2.count(cw->c))
      draw_shapevec(cw->c, cw->V, qfi.fshape->levels[SIDE_HIGH], col, PPR::REDWALL);
    if(house2.count(cw->c))
      draw_shapevec(cw->c, cw->V, qfi.fshape->levels[SIDE_HIGH2], (domroof << 8) | 0xFF, PPR::REDWALL);

    auto co = euc::full_coords2(cw->c);
    int x = gmod(co.first, 20);
    int y = gmod(co.second, 20);
    char ch = xmap[y][x];

    if(ch == 'O') {
      placeSidewall(cw->c, 1, SIDE_HIGH, cw->V, 0xC0E0FFC0);
      ((dqi_poly&)(*ptds.back())).tinf = nullptr;
      }
    }

  color_t skycol = our_skycolor(cw->c);
  g_add_to_sky(cw->c, cw->V, skycol, skycol);
  return true;
  }

int cells_drawn = 50, split_qty = 1;

void build_map() {
  for(cell *c: bac) {
    c->land = laCanvas;
    c->wall = waNone; // Chasm; // waNone;
    c->item = itNone;
    c->monst = moNone;
    c->mpdist = 0;
    c->landparam = 0x205020;
    auto co = euc::full_coords2(c);
    int x = gmod(co.first, 20);
    int y = gmod(co.second, 20);
    ac[x + 20*y + gmod(gdiv(co.first, 20), periods) * 400 + gmod(gdiv(co.second, 20), periods) * 400 * periods] = c;
    char ch = xmap[y][x];
    switch(ch) {
      case 'x':
        c->landparam = ((x+y) & 1) ? 0x807020 : 0xFFE080;
        break;
      case '1':
        c->wall = waRed3;
        house2.insert(c);
        c->landparam = domcol;
        break;
      case '2':
        c->wall = waRed2;
        break;
      case '3':
        c->wall = waRed3;
        break;
      case '#':
        c->wall = waBarrier;
        break;
      case 'T':
        c->wall = waCTree;
        break;
      case 'D':
        c->wall = waWaxWall;
        house1.insert(c);
        house2.insert(c);
        c->landparam = domcol;
        break;
      case 't':
      case 'r':
      case 'd':
        c->wall = waNone;
        house2.insert(c);
        c->landparam = domcolf;
        break;
      case '+':
        c->wall = waNone;
        house2.insert(c);
        c->landparam = domcol;
        break;
      case 'O':
        c->wall = waWaxWall;
        house2.insert(c);
        c->landparam = domcol;
        break;
      case 'u':
      case 'v':
      case '~':
        c->wall = waShallow;
        break;
      case 'p':
        c->land = laCrossroads;
        floorcolors[laCrossroads] = 0x808080;
        break;
        // c->landparam = 0xC0C080;
      case '>':
        if(draw_digger) c->wall = waChasm;
        break;
      }
    }
  }

ld square_diagram_size = 1000;

void enable_square_diagram() {
  rogueviz::rv_hook(hooks_frame, 100, [] {
    if(square_diagram_size >= 1000) return;
    auto p = [] (ld x, ld y) { return hpxy(x, y); };
    for(int i=0; i<4; i++) {
      ld si = square_diagram_size;
      ld big = 100;
      curvepoint(p(si, big));
      curvepoint(p(si, -big));
      curvepoint(p(big, -big));
      curvepoint(p(big, big));
      curvepoint(p(si, big));
      queuecurve(shiftless(Id) * cspin(0, 1, i*90._deg), 0, 0xFF, PPR::CIRCLE).flags |= POLY_ALWAYS_IN;
      }
    });
  }

bool activated = false;

void o_key(o_funcs& v);

void build() {
   auto& T0 = euc::eu_input.user_axes;
   T0[0][0] = 20 * periods;
   T0[1][1] = 20 * periods;
   euc::eu_input.twisted = 0;
   T0[0][1] = 0;
   T0[1][0] = 0;
   euc::build_torus3();
   geometry = gEuclidSquare;
   variation = eVariation::pure;
   start_game();

  bac = currentmap->allcells();
  ac.resize(bac.size());
  build_map();
  }

void enable() {

  if(activated) return;
  tour::slide_backup(activated, true);

  if(!params.count("square_diagram_size")) param_f(square_diagram_size, "square_diagram_size");
  stop_game();
  tour::slide_backup(never_invert, true);
  tour::slide_backup(vid.wallmode, 3);
  tour::slide_backup(vid.monmode, 2);

  geometry = gArchimedean;
  variation = eVariation::pure;
  arcm::current.parse("4^5");
  start_game();
  resetview();
  View = spin(45._deg);
  if(1) {
    dynamicval<int> d(min_cells_drawn, cells_drawn);
  // vid.cells_generated_limit = 9999;
  // vid.cells_drawn_limit = 9999;
  // vid.use_smart_range = 2;
    drawthemap();
    }
  auto f = [] (hyperpoint h) { return deparabolic13(h); };


  set<cell*> seen;
  for(auto c: dcal) if(gmatrix.count(c)) {
    map54_nodes.push_back(f(unshift(gmatrix[c]*C0)));
    seen.insert(c);
    }
  for(auto c: dcal) if(seen.count(c)) forCellEx(c1, c) if(seen.count(c1)) if(c1<c) {
    auto h = unshift(gmatrix[c]*C0);
    auto h1 = unshift(gmatrix[c1]*C0);
    int qty = split_qty;
    vector<hyperpoint> hs(qty+1);
    hs[0] = h; hs[qty] = h1;
    for(int a=qty/2; a; a/=2) for(int i=0; i<qty; i+=a*2) hs[i+a] = mid(hs[i], hs[i+2*a]);
    for(int i=0; i<=qty; i++) hs[i] = f(hs[i]);
    for(int i=0; i<qty; i++) map54_edges.emplace_back(hs[i], hs[i+1]);
    }
  println(hlog, "map54: nodes = ", isize(map54_nodes), " edges = ", isize(map54_edges));
  stop_game();

  geometry = gSpace435;
  start_game();
  resetview();
  if(1) {
    dynamicval<int> d(min_cells_drawn, 500);
    drawthemap();
    bfs();
    println(hlog, "dcal size = ", isize(dcal), " size map = ", isize(gmatrix));
    }
  seen.clear();
  for(auto c: dcal) if(gmatrix.count(c)) {
    map534_nodes.push_back(unshift(gmatrix[c]*C0));
    seen.insert(c);
    }
  for(auto c: dcal) if(seen.count(c)) forCellEx(c1, c) if(seen.count(c1)) if(c1<c) {
    map534_edges.emplace_back(unshift(gmatrix[c])*C0, unshift(gmatrix[c1])*C0);
    }
  stop_game();
  // vid.cells_drawn_limit = 400;
  build();

  if(!floor_textures) make_floor_textures();

  rogueviz::rv_hook(hooks_drawcell, 100, draw_chess_at);
  // rogueviz::rv_hook(hooks_frame, 100, [] { restart = true; });
  rogueviz::rv_hook(hooks_ceiling, 100, chess_ceiling);
  rogueviz::rv_hook(hooks_o_key, 80, o_key);
  rogueviz::rv_hook(hooks_handleKey, 101, [] (int sym, int uni) {
    if((cmode & sm::NORMAL) && uni == 't') {
      vid.sspeed = 0;
      playermoved = true;
      fix_whichcopy(cwt.at);
      // game_keys_scroll = true;
      return true;
      }
    return false;
    });

  tour::slide_backup(frustum_culling, false);
  tour::slide_backup(game_keys_scroll, true);
  tour::slide_backup(bright, true);
  tour::slide_backup(draw_sky, skyAlways);
  tour::slide_backup(vid.cells_generated_limit, 999999);
  tour::slide_backup(noshadow, true);
  tour::slide_backup(auto_remove_roofs, false);
  tour::slide_backup(vid.lake_top, 0.2);
  tour::slide_backup(simple_sky, true);
  tour::slide_backup(sightranges[geometry], 50);
  tour::slide_backup(backcolor, 0xA0C0FF);
  tour::slide_backup(vid.cs, vid.cs);

  tour::slide_backup(vid.use_smart_range, 2);

  // vid.cells_generated_limit = 9999;
  // vid.cells_drawn_limit = 9999;
  // vid.use_smart_range = 2;

  set_char_by_name(vid.cs, "dodek");
  }

void show_smoothcam() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("smoothcam options"), 0xFFFFFFFF, 150, 0);

  dialog::addItem("create/edit", 'c');
  dialog::add_action([] { smoothcam::enable_and_show(); });
  dialog::addBoolItem("run", smoothcam::animate_on, 'r');
  dialog::add_action([] {
    smoothcam::animate_on = !smoothcam::animate_on;
    smoothcam::last_time = HUGE_VAL;
    });
  dialog::addItem("move to the start", 'a');
  dialog::add_action([] { smoothcam::animate_on = true; smoothcam::handle_animation(0); smoothcam::animate_on = false; });
  dialog::addItem("move to the end", 'b');
  dialog::add_action([] { smoothcam::animate_on = true; smoothcam::handle_animation(1-1e-7); smoothcam::animate_on = false; });
  dialog::addItem("save", 's');
  dialog::add_action([] { save_anim("emb/animation.sav"); });
  dialog::addItem("load", 'l');
  dialog::add_action([] { load_anim("emb/animation.sav"); });
  dialog::addInfo("warning: saved animations need to be loaded in the same embedding");
  dialog::addBack();
  dialog::display();
  }

void show_embeddings() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("embeddings"), 0xFFFFFFFF, 150, 0);

  dialog::addItem("2D to 3D", 'a');
  dialog::add_action(switch_fpp_fixed);

  dialog::addItem("choose form the list", 'e');
  dialog::add_action_push(embset_list);

  dialog::addSelItem("invert walls", fts(vid.wall_height), 'i');
  dialog::add_action([] { 
    auto cur = current();
    cur.walls = -cur.walls;
    activate(cur);
    });

  dialog::addSelItem("closer to default", fts(geom3::euclid_embed_scale), '[');
  dialog::add_action([] { 
    activate(emb_lerp(edok(), current(), .9));
    });
  dialog::addSelItem("further from default", fts(geom3::euclid_embed_scale), ']');
  dialog::add_action([] { 
    activate(emb_lerp(edok(), current(), 10/9.));
    });

  dialog::addBack();
  dialog::display();
  }

void show_quality() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("quality / elements"), 0xFFFFFFFF, 150, 0);
  dialog::addInfo("hint: reduce range to improve performance");

  dialog::addBoolItem_action("with models", with_models, 'm');
  dialog::addBoolItem_action("with chess", with_chess, 'h');
  dialog::addSelItem("draw the hole", its(draw_digger), 'g');
  dialog::add_action([] { draw_digger = (1 + draw_digger) % 3; });
  dialog::addBreak(100);

  dialog::addItem("low range", '1');
  dialog::add_action([] { vid.cells_drawn_limit = 400; delete_sky(); });
  dialog::addItem("mid range", '2');
  dialog::add_action([] { vid.cells_drawn_limit = 2000; delete_sky(); });
  dialog::addItem("high range", '3');
  dialog::add_action([] { vid.cells_drawn_limit = 20000; delete_sky(); });
  dialog::addItem("extreme range", '4');
  dialog::add_action([] { vid.cells_drawn_limit = 200000; delete_sky(); });

  dialog::addSelItem("scaffolding X", its(scaffoldx), 'X');
  dialog::add_action([] { scaffoldx = (1 + scaffoldx) % 3; });
  dialog::addSelItem("scaffolding Y", its(scaffoldy), 'Y');
  dialog::add_action([] { scaffoldy = (1 + scaffoldy) % 3; });
  dialog::addSelItem("scaffolding B", its(scaffoldb), 'B');
  dialog::add_action([] { scaffoldb = (1 + scaffoldb) % 3; });
  dialog::addSelItem("no floor", ONOFF(no_floor), '<');
  dialog::add_action([] { no_floor = !no_floor; mapeditor::drawplayer = false; build_map(); });

  dialog::addSelItem("periods", its(periods), 'p');
  dialog::add_action([] { periods++; stop_game(); build(); 
    if(adjust_to_period) activate(rescaled(current(), periods / (periods-1.)));
    });
  dialog::addBoolItem_action("rescale by period", adjust_to_period, 'r');

  dialog::addInfo("hint: scaffolding needs periods to work");

  dialog::addBack();
  dialog::display();
  }

void show_technical() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("technical"), 0xFFFFFFFF, 150, 0);

  dialog::addItem("look downwards", 'l');
  dialog::add_action(look_downwards);

  dialog::addItem("print embedding", 'p');
  dialog::add_action([] { 
    embset e = current();
    println(hlog, e);
    });

  if(true) {
    static ld dist = 0;
    dialog::addSelItem("measure forward distance", fts(dist), 'f');
    dialog::add_action([] { 
      shift_view(ztangent(-0.2));
      spinEdge(100);
      dist += 0.2;
      });
    dialog::addSelItem("go backward", fts(dist), 'b');
    dialog::add_action([] { 
      shift_view(ztangent(+0.05));
      spinEdge(100);
      dist += -0.05;
      });
    }

  #if CAP_VIDEO
  if(false) {
    dialog::addItem("transition to", 'u');
    dialog::add_action([] { anims::videofile = "transition_to.mp4"; transition(0.01, 1, 60); });
    }
  if(false) {
    dialog::addItem("transition from", 'i');
    dialog::add_action([] { anims::videofile = "transition_from.mp4"; transition(1, 0.01, 60); });
    }
  #endif

  dialog::addBack();
  dialog::display();
  }

void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("Non-Euclidean 3D"), 0xFFFFFFFF, 150, 0);

  dialog::addItem("embedding options", 'e');
  dialog::add_action_push(show_embeddings);

  dialog::addItem("quality and elements", 'q');
  dialog::add_action_push(show_quality);

  dialog::addItem("smoothcam animation", 'a');
  dialog::add_action_push(show_smoothcam);
  
  dialog::addItem("technical stuff", 't');
  dialog::add_action_push(show_technical);
  
  dialog::addBoolItem("move the cat", mapeditor::drawplayer, 'd');
  dialog::add_action([] {
    move_cat = true;
    game_keys_scroll = false;
    });

  dialog::addBack();
  dialog::display();    
  }

void o_key(o_funcs& v) {
  v.push_back(named_dialog("embedded-chess options", show));
  }

struct solv_grapher : rogueviz::pres::grapher {

  solv_grapher(transmatrix U) : grapher(-2, -2, 12, 12) {
    T = T * U;
    using rogueviz::pres::p2;
    
    for(int x=-1; x<=11; x++) if(x) {
      line(p2(x,-2), p2(x,12), 0x8080FFFF);
      line(p2(-2,x), p2(12,x), 0x8080FFFF);
      }
    
    vid.linewidth *= 2;
    arrow(p2(0,-2), p2(0,12), .5);
    arrow(p2(-2,0), p2(12,0), .5);
    vid.linewidth /= 2;
    }
  };

ld geo_zero;

transmatrix xyzscale(ld x) {
  transmatrix T = Id;
  T[0][0] = T[1][1] = T[2][2] = x;
  return T;
  }

transmatrix solvscale(ld x) {
  transmatrix T = Id;
  T[0][0] *= exp(-x);
  T[1][1] *= exp(x);
  return T;
  }

ld geodesic_t = 0;

void geodesic_screen(tour::presmode mode, int id) {
  if(!params.count("geodesic_t")) param_f(geodesic_t, "geodesic_t");
  using namespace rogueviz::pres;
  if(mode == pmStart) geo_zero = ticks;

  use_angledir(mode, id == 0);

  static hyperpoint start, middle, target, nlh, nlh1, nlh2;
  const ld coord = 10;
  
  setCanvas(mode, '0');
  if(mode == pmStart) {
    slide_backup(pmodel);
    slide_backup(pconf.clip_min);
    slide_backup(pconf.clip_max);
    slide_backup(vid.cells_drawn_limit);
    stop_game(), pmodel = mdHorocyclic, geometry = gCubeTiling, variation = eVariation::pure, pconf.clip_min = -10000, pconf.clip_max = +100, start_game();

    dynamicval<eGeometry> dg(geometry, gSol);
    dynamicval<int> dr(nisot::rk_steps, 500);
    ld x = coord;
    start = C0;
    middle = point31(0, x, 0);
    auto bmiddle = point31(x, 0, 0);
    target = point31(x, x, 0);
    nlh = inverse_exp_newton(target, 200);
    nlh[2] *= -1;
    println(hlog, "best newton: ", hypot_d(3, nlh), " via ", nlh, " result ", nisot::numerical_exp(nlh));
    nlh1 = inverse_exp(shiftless(middle));
    nlh2 = inverse_exp(shiftless(bmiddle));
    println(hlog, "intermediate: ", hypot_d(3, nlh1) * 2, " via ", nlh1);
    println(hlog, "Pythagoras: ", x * sqrt(2));
    }
  
  add_stat(mode, [id, coord] {
    cmode |= sm::SIDE;
    calcparam();
    
    vid.cells_drawn_limit = 0;
    drawthemap();

    // flat_model_enabler fme;
    initquickqueue();

    solv_grapher g(MirrorZ * ypush(5 * angle / 90._deg) * cspin(1, 2, .9 * angle / 90._deg) * spin(angle/2) * xyzscale(lerp(1, 0.8, angle / 90._deg)));
    
    ld maxtime = 10 * sqrt(2) + 5;
    auto frac_of = [&] (ld t, ld z) { return t - z * floor(t/z); };
    
    ld t = inHighQual ? geodesic_t : frac_of((ticks - geo_zero) / 500, maxtime);

    auto draw_path = [&] (auto f, color_t col) {
      vid.linewidth *= 10;
      for(ld t=0; t<=maxtime; t+=1/16.) curvepoint(f(t));
      queuecurve(g.T, col, 0, PPR::LINE);
      
      auto be_shadow = [&] (hyperpoint& h) {
        // ld part = 1 - angle / 90._deg;
        // h[0] += h[2] * part / 10;
        h[2] = 0;
        };

      for(ld t=0; t<=25; t+=1/16.) {
        hyperpoint h = f(t);
        be_shadow(h);
        curvepoint(h);
        }
      queuecurve(g.T, col & 0xFFFFFF40, 0, PPR::LINE);
      vid.linewidth /= 10;
      
      hyperpoint eaglepos = f(t);
      hyperpoint next_eaglepos = f(t + 1e-2);

      auto z = eaglepos[2];

      // queuepolyat(g.pos(x+z * .1,y,1.5) * spin(s), cgi.shEagle, 0x40, PPR::MONSTER_SHADOW).outline = 0;
      drawMonsterType(moEagle, nullptr, g.T * eupush(eaglepos) * solvscale(z) * rspintox(next_eaglepos - eaglepos) * xyzscale(2), col >> 8, t, 0);            
      
      be_shadow(eaglepos);
      be_shadow(next_eaglepos);
      
      auto& bp = cgi.shEagle;
      
      if(bp.she > bp.shs && bp.she < bp.shs + 1000) {
        auto& p = queuepolyat(g.T * eupush(eaglepos) * solvscale(z) * rspintox(next_eaglepos - eaglepos) * xyzscale(2), bp, 0x18, PPR::TRANSPARENT_SHADOW); 
        p.outline = 0;
        p.subprio = -100;
        p.offset = bp.shs;
        p.cnt = bp.she - bp.shs;
        p.flags &=~ POLY_TRIANGLES;
        p.tinf = NULL;
        return;
        }
      };
      
    color_t pythagoras = 0xcd7f32FF;
    color_t hyperb = 0xaaa9adFF;
    color_t solv  = 0xFFD500FF;

    write_in_space(g.T * rgpushxto0(point31(-.5, -.5, 1)) * MirrorY * zpush(-1), 72, 1, "A", 0xFF);
    write_in_space(g.T * rgpushxto0(point31(coord+.5, coord+.5, 1)) * MirrorY * zpush(-1), 72, 1, "B", 0xFF);
    if(id >= 1) write_in_space(g.T * rgpushxto0(point31(0, coord+.5, 1)) * MirrorY * zpush(-1), 72, 1, "C", 0xFF);

    if(id >= 0)
      draw_path([&] (ld t) { return t < coord * sqrt(2) ? point31(t/sqrt(2), t/sqrt(2), 0) : target; }, pythagoras);

    if(id >= 1)
      draw_path([&] (ld t) { 
        ld len = hypot_d(3, nlh1);
        dynamicval<eGeometry> g(geometry, gSol);
        if(t < len)
          return nisot::numerical_exp(nlh1 * t / len);
        else if(t < len*2)
          return rgpushxto0(middle) * nisot::numerical_exp(nlh2 * (t-len) / len);
        else return target;
        }, hyperb);

    if(id >= 2) 
      draw_path([&] (ld t) { 
        ld len = hypot_d(3, nlh);
        dynamicval<eGeometry> g(geometry, gSol);
        if(t < len)
          return nisot::numerical_exp(nlh * t / len);
        else
          return target;
        }, solv);
      
    auto cat = [] (PPR x) { 
      if(x == PPR::MONSTER_SHADOW) return 1;
      else if(x == PPR::MONSTER_BODY) return 2;
      else return 0;
      };
      
    for(int i=1; i<isize(ptds);)
      if(i && cat(ptds[i]->prio) < cat(ptds[i-1]->prio)) {
        swap(ptds[i], ptds[i-1]);
        i--;
        }
      else i++;

    quickqueue();

    dialog::init();
    dialog_may_latex("\\textsf{from $(0,0,0)$ to $(10,10,0)$}", "from (0,0,0) to (10,10,0)", forecolor, 150);

    dialog::addBreak(100);
    dialog_may_latex("\\textsf{XY plane}", "XY plane", pythagoras >> 8);
    dialog_may_latex("\\textsf{$"+fts(coord)+"\\sqrt{2}$ (Pythagoras)}", fts(coord) + "√2 (Pythagoras)", pythagoras >> 8);
    ld len = coord * sqrt(2);
    dialog_may_latex("\\textsf{$" + fts(len) + "$}", fts(len), pythagoras >> 8);
    
    if(id >= 1) {
      dialog::addBreak(100);
      dialog_may_latex("\\textsf{YZ + XZ}", "YZ + XZ", hyperb >> 8);
      dialog_may_latex("\\textsf{hyperbolic geodesics}", "hyperbolic geodesics", hyperb >> 8);
      ld len = 2 * hypot_d(3, nlh1);
      dialog_may_latex("\\textsf{$" + fts(len) + "$}", fts(len), hyperb >> 8);
      }
    else dialog::addBreak(300);

    if(id >= 2) {
      dialog::addBreak(100);
      dialog_may_latex("\\textsf{optimal}", "optimal", solv >> 8);
      dialog_may_latex("\\textsf{no sharp bends}", "no sharp bends", solv >> 8);
      ld len = hypot_d(3, nlh);
      dialog_may_latex("\\textsf{$"+fts(len)+"$}", fts(len), solv >> 8);
      }
    else dialog::addBreak(300);

    dialog::display();
    
    return false;
    });
  }

using namespace rogueviz::pres;

string defs = 
  "\\def\\map{m}"
  "\\def\\VofH{V}"
  "\\def\\dist{\\delta}"
  "\\def\\ra{\\rightarrow}"
  "\\def\\bbH{\\mathbb{H}}"
  "\\def\\bbE{\\mathbb{E}}"
  "\\renewcommand{\\rmdefault}{\\sfdefault}\\sf"
  ;

void act_or_config() {
  if(activated) pushScreen(show);
  else { enable(); popScreenAll(); mapeditor::drawplayer = false; }
  }

slide embchess_slides[] = {
  {"3D world", 999, LEGAL::NONE | QUICKGEO | QUICKSKIP | USE_SLIDE_NAME,
   "This is the world from our YouTube video 'Non-Euclidean Third Dimension'. "
   "You can activate it by pressing '5', and then press '5' again "
   "to access various options. The next slides are slides from that video.\n\n",

  [] (presmode mode) {
    setCanvas(mode, '0');
    slide_url(mode, 'y', "YouTube link", "https://youtu.be/Rhjv_PazzZE");
    slide_action(mode, 'a', "activate / configure", act_or_config);
    if(mode == pmKey) act_or_config();
    }},

  {"Euler's polyhedron formula", 999, LEGAL::NONE | QUICKGEO | USE_SLIDE_NAME | NOTITLE, 
    "This is a proof that a sphere cannot be tiled with squares.",
    [] (presmode mode) {
      latex_slide(mode, defs+R"=(
   {\color{remph}Euler's polyhedron formula: }   
   \begin{itemize}
   \item $F+V-E = 2$
   \item $F$ -- the number of {\color{remph}faces} (square grid: the number of squares $n$)
   \item $V$ -- the number of {\color{remph}vertices} (square grid: also $n$)
   \item $E$ -- the number of {\color{remph}edges} (square grid: $2n$)
   \item $F + V - E = n + n - 2n = 0 = 2$
   \end{itemize}
   )=", sm::NOSCR, 150);
      }},


  {"from A to B in Solv 1", 999, LEGAL::NONE | QUICKGEO | NOTITLE, 
    "How to get from A to B in Solv as quickly as possible? The first attempt is to do so without leaving the plane. Since the XY plane in Solv has Euclidean geometry, we can compute the length of such a path using the Pythagoras theorem."
    "Press '5' to switch to the 3D view."
    ,
    [] (presmode mode) {
      println(hlog, "A mode ", int(mode));
      empty_screen(mode);
      geodesic_screen(mode, 0);
      no_other_hud(mode);
      }
    },
  {"from A to B in Solv 2", 999, LEGAL::NONE | QUICKGEO | NOTITLE, 
    "But we can do it faster like this, using two hyperbolic geodesics. "
    ,
    [] (presmode mode) {
      println(hlog, "B mode ", int(mode));
      empty_screen(mode);
      geodesic_screen(mode, 1);
      no_other_hud(mode);
      }
    },
  {"from A to B in Solv 3", 999, LEGAL::NONE | QUICKGEO | NOTITLE, 
    "And even faster, do it like this, avoiding sharp bends."
    ,
    [] (presmode mode) {
      println(hlog, "C mode ", int(mode));
      empty_screen(mode);
      geodesic_screen(mode, 2);
      no_other_hud(mode);
      }
    },

  {"final slide", 123, LEGAL::ANY | NOTITLE | QUICKSKIP | FINALSLIDE, 
    "End of the presentation.",
  
    [] (presmode mode) { }
    },
  };

auto embchess_ah = 
  /* enable this unit */
  arg::add3("-embchess", enable)
+ arg::add3("-embperiods", [] { arg::shift(); periods = arg::argi(); })
+ arg::add3("-emb-noceil", [] {
    rogueviz::rv_hook(hooks_ceiling, 100, [] (celldrawer*) { return true; });
    })
+ arg::add3("-chessmul", [] { arg::shift(); hmul = arg::argf(); })
+ arg::add3("-embset", [] {
    arg::shift(); string ss = arg::args();
    for(auto& e: embsets) if(appears(e.name, ss)) activate(e);
    })
+ arg::add3("-embscaffold", [] {
    arg::shift(); scaffoldx = arg::argi(); 
    arg::shift(); scaffoldy = arg::argi();
    arg::shift(); scaffoldb = arg::argi();
    })
+ arg::add3("-embscaffold-move", [] {
    arg::shift(); scaffoldx_move = arg::argi(); 
    arg::shift(); scaffoldy_move = arg::argi();
    })
+ arg::add3("-embwchess", [] { with_chess = !with_chess; })
+ arg::add3("-embfix", [] { arg::shift(); fix_chess = arg::argf(); })
+ arg::add3("-embwmodel", [] { with_models = !with_models; })
+ arg::add3("-embwmlight", [] { with_models = true; light_models = true; })
+ arg::add3("-embwdig", [] { arg::shift(); draw_digger = arg::argi(); })
+ arg::add3("-embload", [] { arg::shift(); load_anim(arg::args()); })
+ arg::add3("-embappend", [] { arg::shift(); append_anim(arg::args()); })
+ arg::add3("-embsave", [] { arg::shift(); save_anim(arg::args()); })
+ arg::add3("-embt0", [] { smoothcam::animate_on = false; smoothcam::handle_animation(0); playermoved = false; })
+ arg::add3("-embt1", [] { smoothcam::animate_on = false; smoothcam::handle_animation(1-1e-7); playermoved = false; })
+ arg::add3("-embmin", [] { arg::shift(); cells_drawn = arg::argi(); })
+ arg::add3("-emb-chess-moves", [] { arg::shift(); chess_moves = arg::argi(); })
+ arg::add3("-emb-sqd", enable_square_diagram)
#if CAP_VIDEO
+ arg::add3("-embtrans-test", [] { arg::shift(); transition_test(arg::argf()); })
#endif
+ arg::add3("-emb-no-floor", [] { no_floor = true; })
#if CAP_VIDEO
+ arg::add3("-embtrans", [] { 
    arg::shift(); anims::videofile = arg::args();
    arg::shift(); ld a = arg::argf();
    arg::shift(); ld b = arg::argf();
    arg::shift(); int t = arg::argi();
    transition(a, b, t);
    })
#endif
+ arg::add3("-avp", [] { semidirect_rendering = true; vid.consider_shader_projection = false; })
#if CAP_VIDEO
+ arg::add3("-embforward", [] { 
    arg::shift(); int t = arg::argi();
    arg::shift(); anims::videofile = arg::args();
    arg::shift(); ld d = arg::argf();
    animate_forward(d, t);
    })
#endif
+ arg::add3("-emb-goforward", [] { 
    arg::shift(); ld d = arg::argf();
    for(int i=0; i<1000; i++) {
      shift_view(ztangent(-d / 1000.));
      optimizeview();
      spinEdge(100);
      }
    });
  ;

auto embchess_show =
  addHook_slideshows(100, [] (tour::ss::slideshow_callback cb) {
    cb(XLAT("non-Euclidean third dimension"), &embchess_slides[0], '3');
    });

}
}
