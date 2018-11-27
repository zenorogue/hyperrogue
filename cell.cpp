// Hyperbolic Rogue -- cells
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// cells the game is played on

namespace hr {

int fix6(int a) { return (a+MODFIXER)%S6; }
int fix7(int a) { return (a+MODFIXER)%S7; }

int dirdiff(int dd, int t) {
  dd %= t;
  if(dd<0) dd += t;
  if(t-dd < dd) dd = t-dd;
  return dd;
  }

int fixdir(int a, cell *c) { a %= c->type; if(a<0) a += c->type; return a; }

int cellcount = 0;

void initcell(cell *c); // from game.cpp

cell *newCell(int type, heptagon *master) {
  cell *c = tailored_alloc<cell> (type);
  c->type = type;
  c->master = master;
  initcell(c);
  return c;
  }

struct cdata {
  int val[4];
  int bits;
  };

// -- hrmap ---

hrmap *currentmap;
vector<hrmap*> allmaps;

// --- auxiliary hyperbolic map for horocycles ---
struct hrmap_alternate : hrmap {
  heptagon *origin;
  hrmap_alternate(heptagon *o) { origin = o; }
  ~hrmap_alternate() { clearfrom(origin); }
  };

hrmap *newAltMap(heptagon *o) { return new hrmap_alternate(o); }
// --- hyperbolic geometry ---

hrmap_hyperbolic::hrmap_hyperbolic() {
  // printf("Creating hyperbolic map: %p\n", this);
  origin = tailored_alloc<heptagon> (S7);
  heptagon& h = *origin;
  h.s = hsOrigin;
  h.emeraldval = a46 ? 0 : 98;
  h.zebraval = 40;
  h.fiftyval = 0;
  h.fieldval = 0;
  h.rval0 = h.rval1 = 0;
  h.cdata = NULL;
  h.alt = NULL;
  h.distance = 0;
  mvar = variation;
  if(binarytiling) {
    #if DEBUG_BINARY_TILING
    binary::xcode.clear();
    binary::rxcode.clear();
    binary::xcode[&h] = (1 << 16);
    binary::rxcode[1<<16] = &h;
    #endif
    h.zebraval = 0,
    h.c7 = newCell(6, origin);
    }
  else if(IRREGULAR)
    irr::link_start(origin);
  else
    h.c7 = newCell(S7, origin);
  }

// --- spherical geometry ---

int spherecells() {
  if(S7 == 5) return (elliptic?6:12);
  if(S7 == 4) return (elliptic?3:6);
  if(S7 == 3) return 4;
  if(S7 == 2) return (elliptic?1:2);
  if(S7 == 1) return 1;
  return 12;
  }

vector<int> siblings;
  
struct hrmap_spherical : hrmap {
  heptagon *dodecahedron[12];
  eVariation mvar;

  hrmap_spherical() {
    mvar = variation;
    for(int i=0; i<spherecells(); i++) {
      heptagon& h = *(dodecahedron[i] = new heptagon);
      h.s = hsOrigin;
      h.emeraldval = i;
      h.zebraval = i;
      h.fiftyval = i;
      h.rval0 = h.rval1 = 0;
      h.alt = NULL;
      h.cdata = NULL;
      h.c.fullclear();
      h.fieldval = i;
      if(!IRREGULAR) h.c7 = newCell(S7, &h);
      }
    if(S7 == 5)
      siblings = {1, 0, 10, 4, 3, 8, 9, 11, 5, 6, 2, 7};
    else
      siblings = {1, 0, 3, 2, 5, 4};
      
    if(S7 == 4 && elliptic) {
      for(int i=0; i<3; i++) {
        int i1 = (i+1)%3;
        int i2 = (i+2)%3;
        dodecahedron[i]->move(0) = dodecahedron[i1];
        dodecahedron[i]->c.setspin(0, 1, false);
        dodecahedron[i]->move(1) = dodecahedron[i2];
        dodecahedron[i]->c.setspin(1, 0, false);
        dodecahedron[i]->move(2) = dodecahedron[i1];
        dodecahedron[i]->c.setspin(2, 3, true);
        dodecahedron[i]->move(3) = dodecahedron[i2];
        dodecahedron[i]->c.setspin(3, 2, true);
        }
      }
    else for(int i=0; i<S7; i++) {
      dodecahedron[0]->move(i) = dodecahedron[i+1];
      dodecahedron[0]->c.setspin(i, 0, false);
      dodecahedron[i+1]->move(0) = dodecahedron[0];
      dodecahedron[i+1]->c.setspin(0, i, false);
      
      dodecahedron[i+1]->move(1) = dodecahedron[(i+S7-1)%S7+1];
      dodecahedron[i+1]->c.setspin(1, S7-1, false);
      dodecahedron[i+1]->move(S7-1) = dodecahedron[(i+1)%S7+1];
      dodecahedron[i+1]->c.setspin(S7-1, 1, false);
      
      if(S7 == 5 && elliptic) {
        dodecahedron[i+1]->move(2) = dodecahedron[(i+2)%S7+1];
        dodecahedron[i+1]->c.setspin(2, 3, true);
        dodecahedron[i+1]->move(3) = dodecahedron[(i+3)%S7+1];
        dodecahedron[i+1]->c.setspin(3, 2, true);
        }

      else if(S7 == 5) {
        dodecahedron[6]->move(i) = dodecahedron[7+i];
        dodecahedron[6]->c.setspin(i, 0, false);
        dodecahedron[7+i]->move(0) = dodecahedron[6];
        dodecahedron[7+i]->c.setspin(0, i, false);
  
        dodecahedron[i+7]->move(1) = dodecahedron[(i+4)%5+7];
        dodecahedron[i+7]->c.setspin(1, 4, false);
        dodecahedron[i+7]->move(4) = dodecahedron[(i+1)%5+7];
        dodecahedron[i+7]->c.setspin(4, 1, false);
        
        dodecahedron[i+1]->move(2) = dodecahedron[7+(10-i)%5];
        dodecahedron[i+1]->c.setspin(2, 2, false);
        dodecahedron[7+(10-i)%5]->move(2) = dodecahedron[1+i];
        dodecahedron[7+(10-i)%5]->c.setspin(2, 2, false);
  
        dodecahedron[i+1]->move(3) = dodecahedron[7+(9-i)%5];
        dodecahedron[i+1]->c.setspin(3, 3, false);
        dodecahedron[7+(9-i)%5]->move(3) = dodecahedron[i+1];
        dodecahedron[7+(9-i)%5]->c.setspin(3, 3, false);
        }
      if(S7 == 4) {
        dodecahedron[5]->move(3-i) = dodecahedron[i+1];
        dodecahedron[5]->c.setspin(3-i, 2, false);
        dodecahedron[i+1]->move(2) = dodecahedron[5];
        dodecahedron[i+1]->c.setspin(2, 3-i, false);
        }
      }

    if(IRREGULAR) {
      irr::link_start(dodecahedron[0]);
      for(int i=0; i<spherecells(); i++)
        for(int j=0; j<S7; j++)
          irr::may_link_next(dodecahedron[i], j);
      }
    }

  heptagon *getOrigin() { return dodecahedron[0]; }

  ~hrmap_spherical() {
    dynamicval<eVariation> ph(variation, mvar);
    for(int i=0; i<spherecells(); i++) clearHexes(dodecahedron[i]);
    for(int i=0; i<spherecells(); i++) delete dodecahedron[i];
    }    

  void verify() {
    for(int i=0; i<spherecells(); i++) for(int k=0; k<S7; k++) {
      heptspin hs(dodecahedron[i], k, false);
      heptspin hs2 = hs + wstep + (S7-1) + wstep + (S7-1) + wstep + (S7-1);
      if(hs2.at != hs.at) printf("error %d,%d\n", i, k);
      }
    for(int i=0; i<spherecells(); i++) verifycells(dodecahedron[i]);
    }
  };

heptagon *getDodecahedron(int i) {
  hrmap_spherical *s = dynamic_cast<hrmap_spherical*> (currentmap);
  if(!s) return NULL;
  return s->dodecahedron[i];
  }

// --- euclidean geometry ---

// NOTE: patterns assume that pair_to_vec(0,1) % 3 == 2!
// Thus, pair_to_vec(0,1) must not be e.g. a power of four

int pair_to_vec(int x, int y) {
  return x + (y << 15);
  }

pair<int, int> vec_to_pair(int vec) {
  int x = vec & ((1<<15)-1);
  int y = (vec >> 15);
  if(x >= (1<<14)) x -= (1<<15), y++;
  return {x, y};
  }

namespace torusconfig {
  // the configuration of the torus topology.
  // torus cells are indexed [0..qty),
  // where the cell to the right from i is indexed i+dx,
  // and the cell to the down-right is numbered i+dy

  // Changed with command line option: -tpar <qty>,<dx>,<dy>
  // Ideally, qty, dx, and dy should have the same "modulo 3"
  // values as the default -- otherwise the three-color
  // pattern breaks. Also, they should have no common
  // prime divisor.
  int def_qty = 127*3, dx = 1, def_dy = -11*2;
  int qty = def_qty, dy = def_dy;
  
  int sdx = 12, sdy = 12;

  // new values to change
  int newqty, newdy, newsdx, newsdy;
  int torus_cx, torus_cy;
  
  vector<torusmode_info> tmodes = {
    {"single row (hex)", TF_SINGLE | TF_HEX},
    {"single row (squares)", TF_SINGLE | TF_SQUARE},
    {"parallelogram (hex)", TF_SIMPLE | TF_HEX},
    {"rectangle (squares)", TF_SIMPLE | TF_SQUARE},
    {"rectangle (hex)", TF_WEIRD | TF_HEX},
    {"Klein bottle (squares)", TF_SIMPLE | TF_KLEIN | TF_SQUARE},
    {"Klein bottle (hex)", TF_WEIRD | TF_KLEIN | TF_HEX},
    {"cylinder (squares)", TF_SIMPLE | TF_CYL },
    {"cylinder (hex)", TF_SIMPLE | TF_CYL | TF_HEX},
    {"Möbius band (squares)", TF_SIMPLE | TF_CYL | TF_KLEIN},
    {"Möbius band (hex)", TF_SIMPLE | TF_CYL | TF_HEX | TF_KLEIN},
    };
  
  eTorusMode torus_mode, newmode;
  flagtype tmflags() { return tmodes[torus_mode].flags; }
  
  int getqty() {
    if(tmflags() & TF_SINGLE)
      return qty;
    else
      return sdx * sdy;
    }
  
  int getvec(int x, int y) {
    if(tmflags() & TF_SINGLE)
      return x * dx + y * dy;
    else if(tmflags() & TF_SIMPLE)
      return pair_to_vec(x, y);
    else
      return pair_to_vec(-y - 2 * x, 3 * y);
    }
  
  int id_to_vec(int id, bool mirrored = false) {
    if(tmflags() & TF_SINGLE)
      return id;
    else {
      int dx = id % sdx;
      int dy = id / sdx;
      if(mirrored) 
        dy = -dy, dx += sdx;
      if(tmflags() & TF_SIMPLE)
        return pair_to_vec(dx, dy);
      else
        return pair_to_vec(- 2 * dx - (dy & 1), 3 * dy);
      }
    }
  
  pair<int, bool> vec_to_id_mirror(int vec) {
    if(tmflags() & TF_SINGLE) {
      return {gmod(vec, qty), false};
      }
    else {
      int x, y;
      tie(x,y) = vec_to_pair(vec);
      bool mirror = false;
      if(tmflags() & TF_KLEIN) {
        if(tmflags() & TF_WEIRD) {
          x = gmod(x, 4 * sdx);
          mirror = x > 0 && x <= 2 * sdx;
          }
        else {
          x = gmod(x, 2 * sdx);
          mirror = x >= sdx;
          }
        if(mirror) y = -y;
        }
      if(tmflags() & TF_WEIRD) {
        y /= 3; x = (x + (y&1)) / -2;
        }
      x = gmod(x, sdx), y = gmod(y, sdy);
      return {y * sdx + x, mirror};
      }
    }

  int vec_to_id(int vec) {
    return vec_to_id_mirror(vec).first;
    }

  void torus_test() {
    printf("Testing torus vec_to_pair/pair_to_vec...\n");
    for(int x=-10; x<=10; x++)
    for(int y=-10; y<=10; y++) {
      auto p = vec_to_pair(pair_to_vec(x, y));
      if(p.first != x || p.second != y)
        printf("Failed for (%d,%d) -> [%d] -> (%d,%d)\n", x, y, pair_to_vec(x,y), p.first, p.second);
      }
    printf("Testing id_to_vec / vec_to_id...\n");
    for(int i=0; i < getqty(); i++) 
    for(int m=0; m< (torus_mode == tmKlein ? 2 : 1); m++)
      if(vec_to_id_mirror(id_to_vec(i, m)) != pair<int,bool> (i,m))
        printf("Failed for id %d.%d [%d] (%d.%d)\n", i, m, id_to_vec(i,m), vec_to_id(id_to_vec(i,m)), vec_to_id_mirror(id_to_vec(i,m)).second);
    }

  int tester = addHook(hooks_tests, 0, torus_test);
  
  void activate() {
    if(tmflags() & TF_HEX)
      ginf[gTorus].vertex = 3, ginf[gTorus].sides = 6;
    else
      ginf[gTorus].vertex = 4, ginf[gTorus].sides = 4;
    if(tmflags() & TF_KLEIN)
      ginf[gTorus].quotientstyle |= qNONOR;
    else 
      ginf[gTorus].quotientstyle &= ~qNONOR;
    if(tmflags() & TF_CYL)
      ginf[gTorus].quotientstyle &= ~qFULLTORUS;
    else
      ginf[gTorus].quotientstyle |= qFULLTORUS;
    }

  int dscalar(gp::loc e1, gp::loc e2) {
    return 2 * (e1.first * e2.first + e1.second*e2.second) + (S3 == 3 ? e1.first*e2.second + e2.first * e1.second : 0);
    }
    
  gp::loc sdxy() { return gp::loc(sdx, sdy); }
  
  int mobius_dir_basic() {
    int dscalars[6];
    for(int a=0; a<SG6; a++)
      dscalars[a] = dscalar(gp::eudir(a), sdxy());
    for(int a=0; a<SG6; a++)
    for(int b=0; b<SG6; b++)
      if(a != b && dscalars[a] == dscalars[b]) {
        return (a + b) % SG6;
      }
    return -1;
    }
  
  bool mobius_symmetric(bool square, int dx, int dy) {
    dynamicval<eGeometry> g(geometry, square ? gEuclidSquare : gEuclid);
    dynamicval<int> gx(sdx, dx);
    dynamicval<int> gy(sdy, dy);
    return mobius_dir_basic() != -1;
    }

  void mobius_flip(int&x, int& y) {
  
    int d = mobius_dir_basic();
    int a, b;
    if(d == 0) a = 1, b = SG6-1;
    else a = 0, b = d;
    auto p1 = gp::eudir(a);
    auto p2 = gp::eudir(b);

    // x = sdx * s + px * t
    // y = sdy * s + py * t
    // py * x = py * sdx * s + px * py * t
    // px * y = px * sdy * s + px + py * t
    // py * x - px * y = py * sdx * s - px * sdy * s
    // s = (py * x - px * y) / (py * sdx - px * sdy)
    
    int det = p1.second * sdx - p1.first * sdy;
    int smul = p1.second * x - p1.first * y;
    int tmul = sdx * y - sdy * x;
    
    x = (tmul * p2.first + smul * sdx) / det;
    y = (tmul * p2.second + smul * sdy) / det;
    
    // println(hlog, make_pair(ox,oy), " [", d, "] ", make_pair(x,y), " p1 = ", p1, " p2 = ", p2, " det = ", det, " smul = ", smul, " tmul = ", tmul);
    }

  int mobius_dir(cell *c) {
    if(c->type == 8) return mobius_dir_basic() * 2;
    else return mobius_dir_basic();
    }
  
  bool be_canonical(int& x, int& y) {
    using namespace torusconfig;
    
    int periods = gdiv(dscalar(gp::loc(x,y), sdxy()), dscalar(sdxy(), sdxy()));
    
    y -= sdy * periods;
    x -= sdx * periods;      
    
    bool b = false;

    if(nonorientable && (periods & 1)) {
      mobius_flip(x, y);
      b = true;
      }
    
    return b;
    }
  
  int cyldist(int id1, int id2) {
  
    int x1, y1, x2, y2;
    tie(x1, y1) = vec_to_pair(id1);
    tie(x2, y2) = vec_to_pair(id2);
    be_canonical(x1, y1);
    be_canonical(x2, y2);
    
    int dist = 1000000000;
    
    for(int a1=-1; a1<=1; a1++) 
    for(int a2=-1; a2<=1; a2++) {
      int ax1 = x1 + sdx * a1;
      int ay1 = y1 + sdy * a1;
      if(nonorientable && a1) mobius_flip(ax1, ay1);
      int ax2 = x2 + sdx * a2;
      int ay2 = y2 + sdy * a2;
      if(nonorientable && a2) mobius_flip(ax2, ay2);
      dist = min(dist, eudist(ax1 - ax2, ay1 - ay2));
      
      }
    
    return dist;
    }
  }

int euclid_getvec(int dx, int dy) {
  if(euwrap) return torusconfig::getvec(dx, dy);
  else return pair_to_vec(dx, dy);
  }

template<class T> void build_euclidean_moves(cell *c, int vec, const T& builder) {
  int x, y;
  tie(x,y) = vec_to_pair(vec);
  c->type = a4 ? (PURE || ((x^y^1) & 1) ? 4 : 8) : 6;

  if(c->type == 4) {
    int m = PURE ? 1 : 2;
    builder(euclid_getvec(+1,+0), 0, 2 * m);        
    builder(euclid_getvec(+0,+1), 1, 3 * m);
    builder(euclid_getvec(-1,+0), 2, 0 * m);
    builder(euclid_getvec(+0,-1), 3, 1 * m);
    }
  else if(c->type == 8) {
    builder(euclid_getvec(+1,+0), 0, 2);
    builder(euclid_getvec(+1,+1), 1, 5);
    builder(euclid_getvec(+0,+1), 2, 3);
    builder(euclid_getvec(-1,+1), 3, 7);
    builder(euclid_getvec(-1,+0), 4, 0);
    builder(euclid_getvec(-1,-1), 5, 1);
    builder(euclid_getvec(+0,-1), 6, 1);
    builder(euclid_getvec(+1,-1), 7, 3);
    }
  else /* 6 */ {
    builder(euclid_getvec(+1,+0), 0, 3);
    builder(euclid_getvec(+0,+1), 1, 4);
    builder(euclid_getvec(-1,+1), 2, 5);
    builder(euclid_getvec(-1,+0), 3, 0);
    builder(euclid_getvec(+0,-1), 4, 1);
    builder(euclid_getvec(+1,-1), 5, 2);
    }
  }

struct hrmap_torus : hrmap {

  vector<cell*> all;
  vector<int> dists;

  virtual vector<cell*>& allcells() { return all; }
  
  cell *gamestart() {
    return all[0];
    }

  hrmap_torus() {
    using namespace torusconfig;
    int q = getqty();
    all.resize(q);
    for(int i=0; i<q; i++) {
      all[i] = newCell(8, encodeId(i));
      }
    for(int i=0; i<q; i++) {
      int iv = id_to_vec(i);
      build_euclidean_moves(all[i], iv, [&] (int delta, int d, int d2) {
        auto im = vec_to_id_mirror(iv + delta);
        all[i]->move(d) = all[im.first];
        all[i]->c.setspin(d, im.second, false);
        });
      }
    for(cell *c: all) for(int d=0; d<c->type; d++) {
      cell *c2 = c->move(d);
      for(int d2=0; d2<c2->type; d2++) 
        if(c2->move(d2) == c)
          c->c.setspin(d, d2, c->c.spin(d));
      }
    celllister cl(gamestart(), 100, 100000000, NULL);
    dists.resize(q);
    for(int i=0; i<isize(cl.lst); i++)
      dists[decodeId(cl.lst[i]->master)] = cl.dists[i];
    }
  
  ~hrmap_torus() {
    for(cell *c: all) delete c;
    }
  };

hrmap_torus *torusmap() {
  return dynamic_cast<hrmap_torus*> (currentmap);
  }

/* cell *getTorusId(int id) {
  hrmap_torus *cur = torusmap();
  if(!cur) return NULL;
  return cur->all[id];
  } */

struct hrmap_euclidean : hrmap {

  cell *gamestart() {
    return *(euclideanAtCreate(0).first);
    }

  struct euclideanSlab {
    cell* a[256][256];
    euclideanSlab() {
      for(int y=0; y<256; y++) for(int x=0; x<256; x++)
        a[y][x] = NULL;
      }
    ~euclideanSlab() {
      for(int y=0; y<256; y++) for(int x=0; x<256; x++)
        if(a[y][x]) delete a[y][x];
      }
    };
  
  static const int slabs = max_vec / 256;
  
  euclideanSlab* euclidean[slabs][slabs];
  
  hrmap_euclidean() {
    for(int y=0; y<slabs; y++) for(int x=0; x<slabs; x++)
      euclidean[y][x] = NULL;
    }
  
  euc_pointer at(int vec) {
    auto p = vec_to_pair(vec);
    int x = p.first, y = p.second;
    bool mobius = false;
    if(euwrap) 
      mobius = torusconfig::be_canonical(x, y);
    euclideanSlab*& slab = euclidean[(y>>8)&(slabs-1)][(x>>8)&(slabs-1)];
    if(!slab) slab = new hrmap_euclidean::euclideanSlab;
    return make_pair(&(slab->a[y&255][x&255]), mobius);
    }
  
  map<int, struct cdata> eucdata;

  ~hrmap_euclidean() {
    for(int y=0; y<slabs; y++) for(int x=0; x<slabs; x++)
      if(euclidean[y][x]) { 
        delete euclidean[y][x];
        euclidean[y][x] = NULL;
        }
    eucdata.clear();
    }
  };

cellwalker vec_to_cellwalker(int vec) {
  if(!fulltorus) {
    auto p = euclideanAtCreate(vec);
    if(p.second)
      return cellwalker(*p.first, torusconfig::mobius_dir(*p.first), true);
    else
      return cellwalker(*p.first, 0, false);
    }
  else {
    hrmap_torus *cur = torusmap();
    if(!cur) return cellwalker(NULL, 0);
    auto p = torusconfig::vec_to_id_mirror(vec);
    return cellwalker(cur->all[p.first], 0, p.second);
    }
  }

int cellwalker_to_vec(cellwalker cw) {
  int id = decodeId(cw.at->master);
  if(!fulltorus) {
    if(nonorientable) {
      auto ep = euclideanAt(id);
      if(ep.second != cw.mirrored) {
        int x, y;
        tie(x, y) = vec_to_pair(id);
        x += torusconfig::sdx;
        y += torusconfig::sdy;
        torusconfig::mobius_flip(x, y);
        return pair_to_vec(x, y);
        }
      }
    return id;
    }
  return torusconfig::id_to_vec(id, cw.mirrored);
  }

int cell_to_vec(cell *c) {
  int id = decodeId(c->master);
  if(!fulltorus) return id;
  return torusconfig::id_to_vec(id, false);
  }

pair<int, int> cell_to_pair(cell *c) {
  return vec_to_pair(cell_to_vec(c));
  }

union heptacoder {
  heptagon *h;
  int id;
  };

int decodeId(heptagon* h) {
  heptacoder u;
  u.h = h; return u.id;
  }

heptagon* encodeId(int id) {
  heptacoder u;
  u.id = id;
  return u.h;
  }

// --- quotient geometry ---

namespace quotientspace {
  struct code {
    int c[MAX_EDGE+1];
    };
  
  bool operator == (const code& c1, const code &c2) {
    for(int i=0; i<=S7; i++) if(c1.c[i] != c2.c[i]) return false;
    return true;
    }

  bool operator < (const code& c1, const code &c2) {
    for(int i=0; i<=S7; i++) if(c1.c[i] != c2.c[i]) return c1.c[i] < c2.c[i];
    return false;
    }  

  int cod(heptagon *h) {
    return zebra40(h->c7);
    }
  
  code get(heptspin hs) {
    code res;
    res.c[0] = cod(hs.at);
    for(int i=1; i<=S7; i++) {
      res.c[i] = cod((hs + wstep).at);
      hs += 1;
      }
    return res;
    }
  
  int rvadd = 0, rvdir = 1;
  
  int rv(int x) { return (rvadd+x*rvdir) % S7; }
  
struct hrmap_quotient : hrmap {

  hrmap_hyperbolic base;
  
  vector<cell*> celllist;
  
  cell *origin;
  
  map<quotientspace::code, int> reachable;
  vector<heptspin> bfsq;
  
  vector<int> connections;
  
  void add(const heptspin& hs) {
    code g = get(hs);
    if(!reachable.count(g)) {
      reachable[g] = bfsq.size();
      bfsq.push_back(hs);
      add(hs + 1);
      }
    }

  vector<heptagon*> allh;
  
  hrmap_quotient() {  
  
    static int symmask = (1<<30);

    connections.clear();
    switch(geometry) {
      case gFieldQuotient: {
        connections = currfp.connections;
        break;
        }
        
      case gZebraQuotient: {
        heptspin hs(base.origin);
        reachable.clear();
        bfsq.clear();
        add(hs);
  
        for(int i=0; i<(int)bfsq.size(); i++) {
          hs = bfsq[i] + wstep;
          add(hs);
          connections.push_back(reachable[get(hs)]);
          }
        break;
        }
      
      case gMinimal: {
        int altzebra[6][7] = {  
          { 16,125,111, 45, 32, 56, 20 },
          { 26,102,146,152, 35,124, 00 },
          { 06, 55,143,134,115,101, 10 },
          { 41, 50, 04, 44,123, 14,153 },
          { 51, 30,154,122, 33, 03,112 },
          { 31, 40,113,136,142, 21, 05 }
          };
               
        // int ok = 0;
        for(int a=0; a<6; a++) {
        for(int b=0; b<7; b++) {
          int s = altzebra[a][b];
          int mirr = s/100; s %= 100;
          int which = s/10; s %= 10;
          
          int shouldbe = a*10+b+mirr*100;
          
          if(altzebra[which][s] != shouldbe) {
            printf("error at %d:%d (is=%d shouldbe=%d)\n", a, b, altzebra[which][s], shouldbe);
            }
          
          connections.push_back(which * 7 + s + (mirr ? symmask : 0) );
          }
          }
        break;
        }
      
      case gKleinQuartic: {
        connections = {
          /* 000 */ 7, 14, 21, 28, 35, 42, 49,
          /* 001 */ 0, 55, 56, 63, 70, 77, 15,
          /* 002 */ 1, 13, 83, 84, 91, 98, 22,
          /* 003 */ 2, 20, 104, 105, 112, 119, 29,
          /* 004 */ 3, 27, 125, 74, 126, 133, 36,
          /* 005 */ 4, 34, 139, 95, 66, 140, 43,
          /* 006 */ 5, 41, 146, 116, 87, 147, 50,
          /* 007 */ 6, 48, 153, 130, 108, 57, 8,
          /* 008 */ 9, 54, 107, 102, 154, 142, 64,
          /* 009 */ 10, 62, 141, 39, 94, 161, 71,
          /* 010 */ 11, 69, 167, 127, 31, 124, 78,
          /* 011 */ 12, 76, 123, 158, 149, 85, 16,
          /* 012 */ 17, 82, 148, 46, 115, 163, 92,
          /* 013 */ 18, 90, 162, 67, 38, 138, 99,
          /* 014 */ 19, 97, 137, 155, 59, 106, 23,
          /* 015 */ 24, 103, 58, 53, 129, 165, 113,
          /* 016 */ 25, 111, 164, 88, 45, 145, 120,
          /* 017 */ 26, 118, 144, 159, 79, 75, 30,
          /* 018 */ 32, 73, 166, 109, 52, 152, 134,
          /* 019 */ 33, 132, 151, 156, 100, 96, 37,
          /* 020 */ 40, 65, 61, 160, 121, 117, 44,
          /* 021 */ 47, 86, 81, 157, 135, 131, 51,
          /* 022 */ 60, 101, 136, 150, 80, 122, 143,
          /* 023 */ 68, 93, 89, 114, 110, 128, 72,
          };
        break;
        }
      
      case gBolza: {
        connections = {
          /* 000 */ 8, 16, 24, 32, 12, 20, 28, 36,
          /* 001 */ 0, 35, 47, 21, 4, 39, 43, 17,
          /* 002 */ 1, 15, 42, 29, 5, 11, 46, 25,
          /* 003 */ 2, 23, 45, 37, 6, 19, 41, 33,
          /* 004 */ 3, 31, 40, 9, 7, 27, 44, 13,
          /* 005 */ 34, 30, 18, 14, 38, 26, 22, 10,
          };
        break;
        }
      
      case gBolza2: {
        connections = {
/* 000 */ 16, 32, 48, 64, 24, 40, 56, 72,
/* 001 */ 20, 44, 52, 76, 28, 36, 60, 68,
/* 002 */ 0, 79, 83, 45, 8, 67, 95, 33,
/* 003 */ 4, 71, 87, 37, 12, 75, 91, 41,
/* 004 */ 1, 23, 94, 61, 13, 27, 86, 49,
/* 005 */ 5, 31, 90, 53, 9, 19, 82, 57,
/* 006 */ 2, 39, 85, 77, 10, 43, 89, 65,
/* 007 */ 6, 47, 81, 69, 14, 35, 93, 73,
/* 008 */ 3, 55, 88, 21, 15, 59, 80, 25,
/* 009 */ 7, 63, 92, 29, 11, 51, 84, 17,
/* 010 */ 70, 58, 46, 18, 78, 50, 38, 26,
/* 011 */ 66, 54, 42, 30, 74, 62, 34, 22,
          };
        break;
        }

      case gMacbeath: {
        connections = {
/* 000 */ 359, 498, 189, 215, 424, 381, 20, 
/* 001 */ 366, 491, 217, 187, 431, 346, 27, 
/* 002 */ 380, 484, 168, 208, 438, 360, 6, 
/* 003 */ 345, 477, 196, 180, 445, 367, 13, 
/* 004 */ 352, 470, 203, 173, 396, 388, 48, 
/* 005 */ 373, 463, 175, 201, 403, 339, 55, 
/* 006 */ 387, 456, 210, 194, 410, 353, 34, 
/* 007 */ 338, 449, 182, 222, 417, 374, 41, 
/* 008 */ 254, 459, 140, 138, 414, 248, 69, 
/* 009 */ 247, 480, 112, 166, 435, 255, 62, 
/* 010 */ 233, 452, 161, 117, 407, 269, 83, 
/* 011 */ 268, 487, 133, 145, 442, 234, 76, 
/* 012 */ 261, 473, 126, 152, 400, 241, 97, 
/* 013 */ 240, 494, 154, 124, 421, 262, 90, 
/* 014 */ 226, 466, 119, 159, 393, 276, 111, 
/* 015 */ 275, 501, 147, 131, 428, 227, 104, 
/* 016 */ 65, 479, 343, 355, 408, 73, 167, 
/* 017 */ 100, 465, 371, 383, 422, 94, 160, 
/* 018 */ 86, 472, 350, 348, 429, 108, 153, 
/* 019 */ 79, 486, 378, 376, 415, 59, 146, 
/* 020 */ 58, 458, 385, 369, 443, 80, 139, 
/* 021 */ 107, 500, 357, 341, 401, 87, 132, 
/* 022 */ 93, 493, 364, 390, 394, 101, 125, 
/* 023 */ 72, 451, 336, 362, 436, 66, 118, 
/* 024 */ 16, 483, 294, 313, 397, 31, 209, 
/* 025 */ 37, 462, 322, 285, 446, 24, 202, 
/* 026 */ 51, 448, 287, 334, 432, 10, 223, 
/* 027 */ 2, 497, 315, 306, 411, 45, 216, 
/* 028 */ 23, 476, 308, 299, 404, 38, 181, 
/* 029 */ 30, 469, 280, 327, 439, 17, 174, 
/* 030 */ 44, 455, 329, 292, 425, 3, 195, 
/* 031 */ 9, 490, 301, 320, 418, 52, 188, 
/* 032 */ 324, 467, 98, 110, 427, 332, 258, 
/* 033 */ 289, 453, 70, 82, 441, 283, 265, 
/* 034 */ 303, 495, 91, 89, 399, 297, 272, 
/* 035 */ 310, 481, 63, 61, 413, 318, 279, 
/* 036 */ 331, 460, 56, 68, 434, 325, 230, 
/* 037 */ 282, 474, 84, 96, 420, 290, 237, 
/* 038 */ 296, 488, 77, 75, 406, 304, 244, 
/* 039 */ 317, 502, 105, 103, 392, 311, 251, 
/* 040 */ 205, 475, 259, 236, 447, 178, 328, 
/* 041 */ 184, 454, 231, 264, 426, 213, 335, 
/* 042 */ 170, 489, 266, 243, 405, 199, 314, 
/* 043 */ 219, 496, 238, 271, 412, 192, 321, 
/* 044 */ 198, 482, 245, 278, 398, 171, 300, 
/* 045 */ 191, 503, 273, 250, 419, 220, 307, 
/* 046 */ 177, 468, 224, 257, 440, 206, 286, 
/* 047 */ 212, 461, 252, 229, 433, 185, 293, 
/* 048 */ 163, 450, 49, 40, 402, 150, 363, 
/* 049 */ 114, 478, 21, 12, 430, 129, 356, 
/* 050 */ 128, 471, 28, 47, 409, 115, 349, 
/* 051 */ 149, 499, 0, 19, 437, 164, 342, 
/* 052 */ 156, 492, 7, 26, 444, 143, 391, 
/* 053 */ 121, 464, 35, 54, 416, 136, 384, 
/* 054 */ 135, 485, 14, 5, 423, 122, 377, 
/* 055 */ 142, 457, 42, 33, 395, 157, 370, 
/* 056 */ 277, 102, 158, 389, 32, 172, 312, 
/* 057 */ 242, 88, 151, 340, 39, 200, 298, 
/* 058 */ 270, 74, 116, 354, 46, 193, 305, 
/* 059 */ 249, 60, 137, 375, 53, 221, 319, 
/* 060 */ 263, 95, 123, 382, 4, 214, 291, 
/* 061 */ 228, 109, 130, 347, 11, 186, 333, 
/* 062 */ 256, 67, 165, 361, 18, 207, 326, 
/* 063 */ 235, 81, 144, 368, 25, 179, 284, 
/* 064 */ 183, 50, 337, 162, 71, 232, 288, 
/* 065 */ 211, 43, 386, 141, 57, 253, 330, 
/* 066 */ 176, 36, 372, 120, 99, 225, 323, 
/* 067 */ 204, 29, 351, 127, 85, 260, 281, 
/* 068 */ 197, 22, 344, 113, 64, 246, 309, 
/* 069 */ 169, 15, 379, 134, 78, 267, 295, 
/* 070 */ 218, 8, 365, 155, 92, 239, 302, 
/* 071 */ 190, 1, 358, 148, 106, 274, 316, 
          };
        break;
        }
      
      case gBring: {
        connections = {
/* 000 */ 5, 92, 46, 33, 14, 
/* 001 */ 0, 62, 56, 38, 24, 
/* 002 */ 15, 97, 76, 63, 4, 
/* 003 */ 10, 32, 86, 68, 29, 
/* 004 */ 25, 67, 106, 93, 9, 
/* 005 */ 20, 37, 116, 98, 19, 
/* 006 */ 35, 102, 16, 3, 44, 
/* 007 */ 30, 72, 26, 8, 54, 
/* 008 */ 45, 107, 66, 73, 34, 
/* 009 */ 40, 2, 81, 78, 59, 
/* 010 */ 55, 77, 96, 103, 39, 
/* 011 */ 50, 7, 111, 108, 49, 
/* 012 */ 65, 112, 6, 13, 74, 
/* 013 */ 60, 42, 21, 18, 84, 
/* 014 */ 75, 117, 36, 43, 64, 
/* 015 */ 70, 12, 51, 48, 89, 
/* 016 */ 85, 47, 91, 113, 69, 
/* 017 */ 80, 17, 101, 118, 79, 
/* 018 */ 95, 82, 1, 23, 104, 
/* 019 */ 90, 52, 11, 28, 114, 
/* 020 */ 105, 87, 31, 53, 94, 
/* 021 */ 100, 22, 41, 58, 119, 
/* 022 */ 115, 57, 61, 83, 99, 
/* 023 */ 110, 27, 71, 88, 109,         
          };
        break;
        }

      case gSchmutzM2: {
        connections = {
          23,  47,  27,  14,  38,  30,  17,  41,  33,  20,  44,  24, 
          35,  39,   3,  26,  42,   6,  29,  45,   9,  32,  36,   0, 
          11,  43,  15,   2,  46,  18,   5,  37,  21,   8,  40,  12, 
          22,  31,   4,  13,  34,   7,  16,  25,  10,  19,  28,   1, 
          };
        break;
        }
      
      case gSchmutzM3: {
        connections = {
          23,  47,  64,  28,  15,  39,  68,  32,  19,  43,  60,  24, 
          35,  52,  40,   4,  27,  56,  44,   8,  31,  48,  36,   0, 
          11,  71,  57,  16,   3,  63,  49,  20,   7,  67,  53,  12, 
          22,  59,  69,   5,  14,  51,  61,   9,  18,  55,  65,   1, 
          21,  30,  62,  41,  13,  34,  66,  45,  17,  26,  70,  37, 
          10,  42,  50,  29,   2,  46,  54,  33,   6,  38,  58,  25, 
          };
        break;
        }
        
      default: break; 
      }
    
    int TOT = connections.size() / S7;
    // printf("heptagons = %d\n", TOT);
    // printf("all cells = %d\n", TOT*(S7+S3)/S3);
    if(!TOT) exit(1);
    allh.resize(TOT);
    for(int i=0; i<TOT; i++) allh[i] = new heptagon;
    // heptagon *oldorigin = origin;
    allh[0]->alt = base.origin;
  
    for(int i=0; i<TOT; i++) {
      heptagon *h = allh[i];
      if(i) {
        h->alt = NULL;
        }
      if(true) {
        h->s = hsOrigin;
        h->emeraldval = 0;
        h->zebraval = 0;
        h->fiftyval = 0;
        h->fieldval = S7*i;
        h->rval0 = h->rval1 = 0; h->cdata = NULL;
        h->distance = 0;
        if(!IRREGULAR) h->c7 = newCell(S7, h);
        }
      for(int j=0; j<S7; j++) {
        int co = connections[i*S7+j];
        bool swapped = co & symmask;
        co &= ~symmask;
        h->move(rv(j)) = allh[co/S7];
        h->c.setspin(rv(j), rv(co%S7), swapped);
        }
      }

    vector<heptagon*> by_dist;
    by_dist.push_back(allh[0]);
    for(int i=0; i<TOT; i++) {
      if(i >= isize(by_dist)) { printf("too fast\n"); exit(1); }
      for(int a=0; a<S7; a++) if(by_dist[i]->move(a)->alt == NULL) by_dist.push_back(by_dist[i]->move(a));
      generateAlts(by_dist[i], 0, false);
      }
  
    for(int i=0; i<TOT; i++) {
      allh[i]->emeraldval = allh[i]->alt->emeraldval;
      allh[i]->zebraval = allh[i]->alt->zebraval;
      allh[i]->fiftyval = allh[i]->alt->fiftyval;
      allh[i]->distance = allh[i]->alt->distance;
      /* for(int j=0; j<7; j++)
        allh[i]->move[j]->alt = createStep(allh[i]->alt, j); */
      }    
    
    if(IRREGULAR) {
      irr::link_start(allh[0]);
      for(int i=0; i<TOT; i++)
        for(int j=0; j<S7; j++)
          irr::may_link_next(allh[i], j);
      }

    celllister cl(gamestart(), 100, 100000000, NULL);
    celllist = cl.lst;
    }

  heptagon *getOrigin() { return allh[0]; }

  ~hrmap_quotient() {
    for(int i=0; i<isize(allh); i++) {
      clearHexes(allh[i]);
      delete allh[i];
      }
    }
  
  vector<cell*>& allcells() { return celllist; }
  };

  };

// --- general ---

// very similar to createMove in heptagon.cpp
cell *createMov(cell *c, int d) {
  if(d<0 || d>= c->type) {
    printf("ERROR createmov\n");
    }

  if(masterless && !c->move(d)) {
    int id = decodeId(c->master);
    for(int dx=-1; dx<=1; dx++)
    for(int dy=-1; dy<=1; dy++)
      euclideanAtCreate(id + pair_to_vec(dx, dy));
    if(!c->move(d)) { 
      println(hlog, "id = ", id, " vec_to_pair(id) = ", vec_to_pair(id), ": failed to create move ", d, " in Euclidean\n");
      exit(0);
      }
    }
  
  if(c->move(d)) return c->move(d);
  else if(IRREGULAR) {
    irr::link_cell(c, d);
    }
  else if(GOLDBERG) {
    gp::extend_map(c, d);
    if(!c->move(d)) {
      printf("extend failed to create for %p/%d\n", c, d);
      exit(1);
      }
    }
  else if(archimedean && PURE) {
    if(arcm::id_of(c->master) < arcm::current.N * 2) {
      heptspin hs = heptspin(c->master, d) + wstep + 2 + wstep + 1;
      c->c.connect(d, hs.at->c7, hs.spin, hs.mirrored);
      }
    else c->c.connect(d, c, d, false);
    }
  else if(archimedean && DUAL) {
    if(arcm::id_of(c->master) >= arcm::current.N * 2) {
      heptagon *h2 = createStep(c->master, d*2);
      int d1 = c->master->c.spin(d*2);
      c->c.connect(d, h2->c7, d1/2, false);
      }
    else {
      printf("bad connection\n");
      c->c.connect(d,c,d,false);
      }
    }
  else if(archimedean || PURE) {
    heptagon *h2 = createStep(c->master, d);
    c->c.connect(d, h2->c7,c->master->c.spin(d),false);
    }
  else if(c == c->master->c7) {
    
    cell *n = newCell(S6, c->master);
    
    heptspin hs(c->master, d, false);
    
    int alt3 = c->type/2;
    int alt4 = alt3+1;
        
    for(int u=0; u<S6; u+=2) {
      if(hs.mirrored && geometry == gSmallElliptic) hs+=1;
      hs.at->c7->c.connect(hs.spin, n, u, hs.mirrored);
      if(hs.mirrored && geometry == gSmallElliptic) hs+=-1;
      hs = hs + alt3 + wstep - alt4;
      }
    extern void verifycell(cell *c);
    verifycell(n);
    }

  else {
    cellwalker cw(c, d, false);
    cellwalker cw2 = cw - 1 + wstep - 1 + wstep - 1;
    c->c.connect(d, cw2);
    }
  return c->move(d);
  }

cell *createMovR(cell *c, int d) {
  d %= MODFIXER; d += MODFIXER; d %= c->type;
  return createMov(c, d);
  }

cell *getMovR(cell *c, int d) {
  d %= MODFIXER; d += MODFIXER; d %= c->type;
  return c->move(d);
  }

void eumerge(cell* c1, int s1, cell *c2, int s2, bool mirror) {
  if(!c2) return;
  c1->move(s1) = c2; c1->c.setspin(s1, s2, mirror);
  c2->move(s2) = c1; c2->c.setspin(s2, s1, mirror);
  }

//  map<pair<eucoord, eucoord>, cell*> euclidean;

euc_pointer euclideanAt(int vec) {
  if(fulltorus) { printf("euclideanAt called\n"); exit(1); }
  hrmap_euclidean* euc = dynamic_cast<hrmap_euclidean*> (currentmap);
  return euc->at(vec);
  }

euc_pointer euclideanAtCreate(int vec) {
  euc_pointer ep = euclideanAt(vec);
  cell*& c = *ep.first;
  if(!c) {
    if(euwrap) {
      int x, y;
      tie(x, y) = vec_to_pair(vec);
      torusconfig::be_canonical(x, y);
      vec = pair_to_vec(x, y);
      }
    c = newCell(8, encodeId(vec));
    // euclideanAt(vec) = c;
    build_euclidean_moves(c, vec, [ep, c,vec] (int delta, int d, int d2) { 
      euc_pointer ep2 = euclideanAt(vec + delta);
      cell* c2 = *ep2.first;
      if(!c2) return;
      // if(ep.second) d = c->c.fix(torusconfig::mobius_dir(c) - d);
      if(ep2.second) d2 = c2->c.fix(torusconfig::mobius_dir(c2) - d2);
      eumerge(c, d, c2, d2, ep2.second);
      });
    }
  return ep;
  }

// initializer (also inits origin from heptagon.cpp)
void initcells() {
  DEBB(DF_INIT, (debugfile,"initcells\n"));
  
  if(archimedean) currentmap = arcm::new_map();
  else if(fulltorus) currentmap = new hrmap_torus;
  else if(euclid) currentmap = new hrmap_euclidean;
  else if(sphere) currentmap = new hrmap_spherical;
  else if(quotient) currentmap = new quotientspace::hrmap_quotient;
  else currentmap = new hrmap_hyperbolic;
  
  allmaps.push_back(currentmap);

  windmap::create();  
  
  // origin->emeraldval = 
  }

void clearcell(cell *c) {
  if(!c) return;
  DEBMEM ( printf("c%d %p\n", c->type, c); )
  for(int t=0; t<c->type; t++) if(c->move(t)) {
    DEBMEM ( printf("mov %p [%p] S%d\n", c->move(t), c->move(t)->move(c->c.spin(t)), c->c.spin(t)); )
    if(c->move(t)->move(c->c.spin(t)) != NULL &&
      c->move(t)->move(c->c.spin(t)) != c) {
        printf("type = %d %d -> %d\n", c->type, t, c->c.spin(t));
        printf("cell error\n");
        exit(1);
        }
    c->move(t)->move(c->c.spin(t)) = NULL;
    }
  DEBMEM ( printf("DEL %p\n", c); )
  delete c;
  }

heptagon deletion_marker;

template<class T> void subcell(cell *c, const T& t) {
  if(GOLDBERG) {
    forCellEx(c2, c) if(c2->move(0) == c && c2 != c2->master->c7) {
      subcell(c2, t);
      }
    }
  else if(BITRUNCATED && !archimedean && !binarytiling)
    forCellEx(c2, c) t(c2);
  t(c);
  }

void clearHexes(heptagon *at) {
  if(at->c7 && at->cdata) {
    delete at->cdata;
    at->cdata = NULL;
    }
  if(IRREGULAR) irr::clear_links(at);
  else if(at->c7) subcell(at->c7, clearcell);
  }

void unlink_cdata(heptagon *h) {
  if(h->alt && h->c7) {
    if(h->alt->cdata == (cdata*) h)
      h->alt->cdata = NULL;
    }
  }

void clearfrom(heptagon *at) {
  queue<heptagon*> q;
  unlink_cdata(at);
  q.push(at);
  at->alt = &deletion_marker;
//int maxq = 0;
  while(!q.empty()) {
    at = q.front(); 
//  if(q.size() > maxq) maxq = q.size();
    q.pop();
    DEBMEM ( printf("from %p\n", at); )
    if(!at->c7) {
      heptagon *h = (heptagon*) at->cdata;
      if(h) {
        if(h->alt != at) printf("alt error :: h->alt = %p\n", h->alt);
        cell *c = h->c7;
        subcell(c, destroycellcontents);
        h->alt = NULL;
        at->cdata = NULL;
        }
      }
    int edges = at->degree();
    if(binarytiling) edges = at->c7->type;
    for(int i=0; i<edges; i++) if(at->move(i)) {
      if(at->move(i)->alt != &deletion_marker)
        q.push(at->move(i));    
      unlink_cdata(at->move(i));
      at->move(i)->alt = &deletion_marker;
      DEBMEM ( printf("!mov %p [%p]\n", at->move(i), at->move(i)->move[at->c.spin(i)]); )
      if(at->move(i)->move(at->c.spin(i)) != NULL &&
        at->move(i)->move(at->c.spin(i)) != at) {
          printf("hept error\n");
          exit(1);
          }
      at->move(i)->move(at->c.spin(i)) = NULL;
      at->move(i) = NULL;
      }
    clearHexes(at);
    delete at;
    }
//printf("maxq = %d\n", maxq);
  }

void verifycell(cell *c) {
  int t = c->type;
  for(int i=0; i<t; i++) {
    cell *c2 = c->move(i);
    if(c2) {
      if(!masterless && BITRUNCATED && c == c->master->c7) verifycell(c2);
      if(c2->move(c->c.spin(i)) && c2->move(c->c.spin(i)) != c) {
        printf("cell error %p:%d [%d] %p:%d [%d]\n", c, i, c->type, c2, c->c.spin(i), c2->type);
        exit(1);
        }
      }
    }
  }

void verifycells(heptagon *at) {
  if(GOLDBERG || IRREGULAR || archimedean) return;
  for(int i=0; i<S7; i++) if(at->move(i) && at->move(i)->move(at->c.spin(i)) && at->move(i)->move(at->c.spin(i)) != at) {
    printf("hexmix error %p [%d s=%d] %p %p\n", at, i, at->c.spin(i), at->move(i), at->move(i)->move(at->c.spin(i)));
    }
  if(!sphere && !quotient) 
    for(int i=0; i<S7; i++) if(at->move(i) && at->c.spin(i) == 0 && at->s != hsOrigin)
      verifycells(at->move(i));
  verifycell(at->c7);
  }

int eudist(int sx, int sy) {
  int z0 = abs(sx);
  int z1 = abs(sy);
  if(a4 && BITRUNCATED)
    return (z0 == z1 && z0 > 0) ? z0+1: max(z0, z1);
  if(a4) return z0 + z1;
  int z2 = abs(sx+sy);
  return max(max(z0,z1), z2);
  }

int eudist(int vec) {
  auto p = vec_to_pair(vec);
  return eudist(p.first, p.second);
  }

int compdist(int dx[]) {
  int mi = dx[0];
  for(int u=0; u<S3; u++) mi = min(mi, dx[u]);
  for(int u=0; u<S3; u++) 
    if(dx[u] > mi+2)
      return -1; // { printf("cycle error!\n"); exit(1); }
  for(int u=0; u<S3; u++) 
    if(dx[u] == mi+2)
      return mi+1;
  int cnt = 0;
  for(int u=0; u<S3; u++) 
    if(dx[u] == mi) cnt++;
  if(cnt < 2)
    return mi+1;
  return mi;
  }

int celldist(cell *c) {
  if(fulltorus) 
    return torusmap()->dists[decodeId(c->master)];
  if(euwrap)
    return torusconfig::cyldist(decodeId(c->master), 0);
  if(masterless)
    return eudist(decodeId(c->master));
  if(sphere || binarytiling) return celldistance(c, currentmap->gamestart());
  if(IRREGULAR) return irr::celldist(c, false);
  if(archimedean || ctof(c)) return c->master->distance;
  if(GOLDBERG) return gp::compute_dist(c, celldist);
  int dx[MAX_S3];
  for(int u=0; u<S3; u++)
    dx[u] = createMov(c, u+u)->master->distance;
  return compdist(dx);
  }

#define ALTDIST_BOUNDARY 99999
#define ALTDIST_UNKNOWN 99998

#define ALTDIST_ERROR 90000

int celldistAlt(cell *c) {
  if(masterless) {
    if(fulltorus) return celldist(c);
    int x, y;
    tie(x,y) = vec_to_pair(decodeId(c->master));
    return euclidAlt(x, y);
    }
  if(binarytiling) return c->master->distance + (specialland == laCamelot && !tactic::on? 30 : 0);
  if(sphere || quotient) {
    return celldist(c) - 3;
    }
  if(!c->master->alt) return 0;
  if(IRREGULAR) return irr::celldist(c, true);
  if(ctof(c)) return c->master->alt->distance;
  if(GOLDBERG) return gp::compute_dist(c, celldistAlt);
  int dx[MAX_S3]; dx[0] = 0;
  for(int u=0; u<S3; u++) if(createMov(c, u+u)->master->alt == NULL)
    return ALTDIST_UNKNOWN;
  for(int u=0; u<S3; u++)
    dx[u] = createMov(c, u+u)->master->alt->distance;
  // return compdist(dx); -> not OK because of boundary conditions
  int mi = dx[0];
  for(int i=1; i<S3; i++) mi = min(mi, dx[i]);
  for(int i=0; i<S3; i++) if(dx[i] > mi+2)
    return ALTDIST_BOUNDARY; // { printf("cycle error!\n"); exit(1); }
  for(int i=0; i<S3; i++) if(dx[i] == mi+2)
    return mi+1;
  return mi;
  }

int dirfromto(cell *cfrom, cell *cto) {
  for(int i=0; i<cfrom->type; i++) if(cfrom->move(i) == cto) return i;
  return -1;
  }

#define RPV_MODULO 5

#define RPV_RAND 0
#define RPV_ZEBRA 1
#define RPV_EMERALD 2
#define RPV_PALACE 3
#define RPV_CYCLE 4

int getCdata(cell *c, int j);

// x mod 5 = pattern type
// x mod (powers of 2) = pattern type specific
// (x/5) mod 15 = picture for drawing floors
// x mod 7 = chance of pattern-specific pic
// whole = randomization

bool randpattern(cell *c, int rval) {
  int i, sw=0;
  switch(rval%5) {
    case 0:
      if(rval&1) {
        return hrandpos() < rval;
        }
      else {
        int cd = getCdata(c, 0);
        return !((cd/(((rval/2)&15)+1))&1);
        }
    case 1:
      i = zebra40(c);
      if(i&1) { if(rval&4) sw^=1; i &= ~1; }
      if(i&2) { if(rval&8) sw^=1; i &= ~2; }
      i >>= 2;
      i--; i /= 3;
      if(rval & (16<<i)) sw^=1;
      return sw;
    case 2:
      i = emeraldval(c);
      if(i&1) { if(rval&4) sw^=1; i &= ~1; }
      if(i&2) { if(rval&8) sw^=1; i &= ~2; }
      i >>= 2; i--;
      if(rval & (16<<i)) sw^=1;
      return sw;
    case 3:
      if(polara50(c)) { if(rval&4) sw^=1; }
      if(polarb50(c)) { if(rval&8) sw^=1; }
      i = fiftyval049(c); i += 6; i /= 7;
      if(rval & (16<<i)) sw^=1;
      return sw;
    case 4:
      i = (rval&3);
      if(i == 1 && (celldist(c)&1)) sw ^= 1;
      if(i == 2 && (celldist(c)&2)) sw ^= 1;
      if(i == 3 && ((celldist(c)/3)&1)) sw ^= 1;
      if(rval & (4<<towerval(c, celldist))) sw ^= 1;
      return sw;
    }
  return 0;
  }

string describeRPM(eLand l) {
  int rval = randompattern[l];
  switch(rval%5) {
    case 0:
      if(rval&1)
        return "R:"+its(rval/(HRANDMAX/100))+"%";
      else
        return "Landscape/"+its(((rval/2)&15)+1);
    case 1:
      return "Z/"+its((rval>>2)&3)+"/"+its((rval>>4)&15);
    case 2:
      return "E/"+its((rval>>2)&3)+"/"+its((rval>>4)&2047);
    case 3:
      return "P/"+its((rval>>2)&3)+"/"+its((rval>>4)&255);
    case 4:
      return "C/"+its(rval&3)+"/"+its((rval>>2)&65535);
    }
  return "?";
  }

int randpatternCode(cell *c, int rval) {
  switch(rval % RPV_MODULO) {
    case 1:
      return zebra40(c);
    case 2:
      return emeraldval(c);
    case 3:
      return fiftyval049(c) + (polara50(c)?50:0) + (polarb50(c)?1000:0);
    case 4:
      return towerval(c, celldist) * 6 + celldist(c) % 6;
    }
  return 0;  
  }

#define RANDITER 31

char rpm_memoize[3][256][RANDITER+1];

void clearMemoRPM() {
  for(int a=0; a<3; a++) for(int b=0; b<256; b++) for(int i=0; i<RANDITER+1; i++)
    rpm_memoize[a][b][i] = 2;
  }

bool randpatternMajority(cell *c, int ival, int iterations) {
  int rval = 0;
  if(ival == 0) rval = randompattern[laCaves];
  if(ival == 1) rval = randompattern[laLivefjord];
  if(ival == 2) rval = randompattern[laEmerald];
  if(rval%RPV_MODULO == RPV_RAND) return randpattern(c, rval);
  int code = randpatternCode(c, rval);
  char& memo(rpm_memoize[ival][code][iterations]);
  if(memo < 2) return memo;
  int z = 0;
  if(iterations) for(int i=0; i<c->type; i++) {
    if(randpatternMajority(createMov(c,i), ival, iterations-1))
      z++;
    else
      z--;
    }
  if(z!=0) memo = (z>0);
  else memo = randpattern(c, rval);
  // printf("%p] rval = %X code = %d iterations = %d result = %d\n", c, rval, code, iterations, memo);
  return memo;
  }

map<heptagon*, int> spins;

#define RVAL_MASK 0x10000000
#define DATA_MASK 0x20000000

cdata orig_cdata;

void affect(cdata& d, short rv, signed char signum) {
  if(rv&1) d.val[0]+=signum; else d.val[0]-=signum;
  if(rv&2) d.val[1]+=signum; else d.val[1]-=signum;
  if(rv&4) d.val[2]+=signum; else d.val[2]-=signum;
  if(rv&8) d.val[3]+=signum; else d.val[3]-=signum;
  int id = (rv>>4) & 63;
  if(id < 32) 
    d.bits ^= (1 << id);
  }

void setHeptagonRval(heptagon *h) {
  if(!(h->rval0 || h->rval1)) {
    h->rval0 = hrand(0x10000);
    h->rval1 = hrand(0x10000);
    }
  }

cdata *getHeptagonCdata(heptagon *h) {
  if(h->cdata) return h->cdata;

  if(sphere || quotient) h = currentmap->gamestart()->master;

  if(h == currentmap->gamestart()->master) {
    return h->cdata = new cdata(orig_cdata);
    }
  
  cdata mydata = *getHeptagonCdata(h->move(0));

  for(int di=3; di<5; di++) {
    heptspin hs(h, di, false);
    int signum = +1;
    while(true) {
      heptspin hstab[15];
      hstab[7] = hs;
      
      for(int i=8; i<12; i++) {
        hstab[i] = hstab[i-1];
        hstab[i] += ((i&1) ? 4 : 3);
        hstab[i] += wstep;
        hstab[i] += ((i&1) ? 3 : 4);
        }

      for(int i=6; i>=3; i--) {
        hstab[i] = hstab[i+1];
        hstab[i] += ((i&1) ? 3 : 4);
        hstab[i] += wstep;
        hstab[i] += ((i&1) ? 4 : 3);
        }
      
      if(hstab[3].at->distance < hstab[7].at->distance) {
        hs = hstab[3]; continue;
        }

      if(hstab[11].at->distance < hstab[7].at->distance) {
        hs = hstab[11]; continue;
        }
      
      int jj = 7;
      for(int k=3; k<12; k++) if(hstab[k].at->distance < hstab[jj].at->distance) jj = k;
      
      int ties = 0, tiespos = 0;
      for(int k=3; k<12; k++) if(hstab[k].at->distance == hstab[jj].at->distance) 
        ties++, tiespos += (k-jj);
        
      // printf("ties=%d tiespos=%d jj=%d\n", ties, tiespos, jj);
      if(ties == 2) jj += tiespos/2;
      
      if(jj&1) signum = -1;
      hs = hstab[jj];
      
      break;
      }
    hs = hs + 3 + wstep;
    setHeptagonRval(hs.at);
    
    affect(mydata, hs.spin ? hs.at->rval0 : hs.at->rval1, signum);

    /* if(!(spins[hs.at] & hs.spin)) {
      spins[hs.at] |= (1<<hs.spin);
      int t = 0;
      for(int k=0; k<7; k++) if(spins[hs.at] & (1<<k)) t++;
      static bool wast[256];
      if(!wast[spins[hs.at]]) {
        printf("%p %4x\n", hs.at, spins[hs.at]);
        wast[spins[hs.at]] = true;
        }
      } */
    }

  return h->cdata = new cdata(mydata);
  }

cdata *getEuclidCdata(int h) {

  if(euwrap) { // fix cylinder?
    static cdata xx;
    return &xx;
    }
  
  int x, y;
  hrmap_euclidean* euc = dynamic_cast<hrmap_euclidean*> (currentmap);
  if(euc->eucdata.count(h)) return &(euc->eucdata[h]);
  
  tie(x,y) = vec_to_pair(h);

  if(x == 0 && y == 0) {
    cdata xx;
    for(int i=0; i<4; i++) xx.val[i] = 0;
    xx.bits = 0;
    return &(euc->eucdata[h] = xx);
    }
  int ord = 1, bid = 0;
  while(!((x|y)&ord)) ord <<= 1, bid++;
  
  for(int k=0; k<3; k++) {
    int x1 = x + (k<2 ? ord : 0);
    int y1 = y - (k>0 ? ord : 0);
    if((x1&ord) || (y1&ord)) continue;
    int x2 = x - (k<2 ? ord : 0);
    int y2 = y + (k>0 ? ord : 0);

    cdata *d1 = getEuclidCdata(pair_to_vec(x1,y1));
    cdata *d2 = getEuclidCdata(pair_to_vec(x2,y2));
    cdata xx;
    double disp = pow(2, bid/2.) * 6;
    
    for(int i=0; i<4; i++) {
      double dv = (d1->val[i] + d2->val[i])/2 + (hrand(1000) - hrand(1000))/1000. * disp;
      xx.val[i] = floor(dv);
      if(hrand(1000) / 1000. < dv - floor(dv)) xx.val[i]++;
      }
    xx.bits = 0;

    for(int b=0; b<32; b++) {
      bool gbit = ((hrand(2)?d1:d2)->bits >> b) & 1;
      int flipchance = (1<<bid);
      if(flipchance > 512) flipchance = 512;
      if(hrand(1024) < flipchance) gbit = !gbit;
      if(gbit) xx.bits |= (1<<b);
      }
    
    return &(euc->eucdata[h] = xx);
    }
  
  // impossible!
  return NULL;
  }

int getCdata(cell *c, int j) {
  if(masterless) return getEuclidCdata(decodeId(c->master))->val[j];
  else if(geometry) return 0;
  else if(ctof(c)) return getHeptagonCdata(c->master)->val[j]*3;
  else {
    int jj = 0;
    auto ar = gp::get_masters(c);
    for(int k=0; k<3; k++)
      jj += getHeptagonCdata(ar[k])->val[j];
    return jj;
    }
  }

int getBits(cell *c) {
  if(masterless) return getEuclidCdata(decodeId(c->master))->bits;
  else if(geometry) return 0;
  else if(c->type != 6) return getHeptagonCdata(c->master)->bits;
  else {
    auto ar = gp::get_masters(c);
    int b0 = getHeptagonCdata(ar[0])->bits;
    int b1 = getHeptagonCdata(ar[1])->bits;
    int b2 = getHeptagonCdata(ar[2])->bits;
    return (b0 & b1) | (b1 & b2) | (b2 & b0);
    }
  }

cell *heptatdir(cell *c, int d) {
  if(d&1) {
    cell *c2 = createMov(c, d);
    int s = c->c.spin(d);
    s += 3; s %= 6;
    return createMov(c2, s);
    }
  else return createMov(c, d);
  }

int heptdistance(heptagon *h1, heptagon *h2) {
  // very rough distance
  int d = 0;
  while(true) {
    if(h1 == h2) return d;
    for(int i=0; i<S7; i++) if(h1->move(i) == h2) return d + 1;
    int d1 = h1->distance, d2 = h2->distance;
    if(d1 >= d2) d++, h1 = createStep(h1, binarytiling ? 5 : 0);
    if(d2 >  d1) d++, h2 = createStep(h2, binarytiling ? 5 : 0);
    }
  }

int heptdistance(cell *c1, cell *c2) {
  if(!hyperbolic || quotient) return celldistance(c1, c2);
  else return heptdistance(c1->master, c2->master);
  }

map<pair<cell*, cell*>, int> saved_distances;

int celldistance(cell *c1, cell *c2) {
  
  if((masterless) && (euclid6 || (euclid4 && PURE))) {
    if(!euwrap)
      return eudist(decodeId(c1->master) - decodeId(c2->master)); // fix cylinder
    else if(euwrap && torusconfig::torus_mode == 0) 
      return torusmap()->dists[torusconfig::vec_to_id(decodeId(c1->master)-decodeId(c2->master))];
    else if(euwrap && !fulltorus)
      return torusconfig::cyldist(decodeId(c1->master), decodeId(c2->master));
    }
  
  if(geometry == gFieldQuotient && !GOLDBERG)
    return currfp.getdist(fieldpattern::fieldval(c1), fieldpattern::fieldval(c2));

  if(sphere || quotient || fulltorus) {
    
    if(saved_distances.count(make_pair(c1,c2)))
      return saved_distances[make_pair(c1,c2)];

    celllister cl(c1, 100, 100000000, NULL);
    for(int i=0; i<isize(cl.lst); i++)
      saved_distances[make_pair(c1, cl.lst[i])] = cl.dists[i];

    if(saved_distances.count(make_pair(c1,c2)))
      return saved_distances[make_pair(c1,c2)];

    return 64;
    }
  
  if(masterless || archimedean) {
    
    if(saved_distances.count(make_pair(c1,c2)))
      return saved_distances[make_pair(c1,c2)];
      
    if(isize(saved_distances) > 1000000) saved_distances.clear();

    celllister cl(c1, 64, 1000, c2);

    for(int i=0; i<isize(cl.lst); i++)
      saved_distances[make_pair(c1, cl.lst[i])] = cl.dists[i];

    if(saved_distances.count(make_pair(c1,c2)))
      return saved_distances[make_pair(c1,c2)];

    return 64;
    }
  
  return hyperbolic_celldistance(c1, c2);
  }

void clearCellMemory() {
  for(int i=0; i<isize(allmaps); i++) 
    if(allmaps[i])
      delete allmaps[i];
  allmaps.clear();
  last_cleared = NULL;
  saved_distances.clear();
  pd_from = NULL;
  }

auto cellhooks = addHook(clearmemory, 500, clearCellMemory);

}
