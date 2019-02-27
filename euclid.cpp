// Hyperbolic Rogue -- Euclidean geometry, including 2D, 3D, and quotient spaces
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

// 2D Euclidean space

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
    auto& gi(ginf[gTorus]);

    if(tmflags() & TF_HEX)
      gi.vertex = 3, gi.sides = 6, gi.tiling_name = "{6,3}";
    else
      gi.vertex = 4, gi.sides = 4, gi.tiling_name = "{4,4}";

    flagtype& flags = gi.flags;
    
    set_flag(flags, qNONORIENTABLE, tmflags() & TF_KLEIN);
    set_flag(flags, qBOUNDED, !(tmflags() & TF_CYL));
    
    int i = 0;
    if(tmflags() & TF_KLEIN) i++;
    if(tmflags() & TF_CYL) i+=2;
    
    const char *quonames[4] = {"torus", "Klein bottle", "cylinder", "Möbius band"};
    gi.quotient_name = quonames[i];
    }

  int dscalar(gp::loc e1, gp::loc e2) {
    return 2 * (e1.first * e2.first + e1.second*e2.second) + (S3 == 3 ? e1.first*e2.second + e2.first * e1.second : 0);
    }
    
  int dcross(gp::loc e1, gp::loc e2) {
    return e1.first * e2.second - e1.second*e2.first;
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
    for(cell *c: all) tailored_delete(c);
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
        if(a[y][x]) tailored_delete(a[y][x]);
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
        tailored_delete(euclidean[y][x]);
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

// 3D Euclidean space

#if MAXDIM == 4

namespace euclid3 {

  typedef long long coord;
  
  struct hrmap_euclid3 : hrmap {
    map<coord, heptagon*> spacemap;
    map<heptagon*, coord> ispacemap;
    hrmap_euclid3() {
      getOrigin();
      }
    heptagon *getOrigin() {
      return get_at(0);
      }

    heptagon *get_at(coord at) {
      if(spacemap.count(at)) 
        return spacemap[at];
      else {
        auto h = tailored_alloc<heptagon> (6);
        h->c7 = newCell(6, h);
        h->distance = 0;
        h->cdata = NULL;
        spacemap[at] = h;
        ispacemap[h] = at;
        return h;
        }
      }
    
    heptagon *build(heptagon *parent, int d, coord at) {
      auto h = get_at(at);
      h->c.connect((d+3)%6, parent, d, false);
      return h;
      }
  
    heptagon *createStep(heptagon *parent, int d) {
      int at = ispacemap[parent];
      coord shifttable[6] = { +1, +1000, +1000000, -1, -1000, -1000000 };
      return build(parent, d, at + shifttable[d]);
      }
    };
  
  hrmap_euclid3* cubemap() {
    return ((hrmap_euclid3*) currentmap);
    }

  hrmap* new_map() {
    return new hrmap_euclid3;
    }

  heptagon *createStep(heptagon *parent, int d) {
    return cubemap()->createStep(parent, d);
    }
  
  int getcoord(coord x, int a) { 
    for(int k=0; k<a; k++) { x -= getcoord(x, 0); x /= 1000; }
    x %= 1000; 
    if(x>500) x -= 1000; 
    if(x<-500) x += 500; 
    return x; 
    }

  bool pseudohept(cell *c) {
    coord co = cubemap()->ispacemap[c->master];
    for(int i=0; i<3; i++) if(!(getcoord(co, i) & 1)) return false;
    return true;
    }

  bool dist_alt(cell *c) {
    coord co = cubemap()->ispacemap[c->master];
    return getcoord(co, 2);
    }

  void draw() {
    dq::visited.clear();
    dq::enqueue(viewctr.at, cview());
    
    while(!dq::drawqueue.empty()) {
      auto& p = dq::drawqueue.front();
      heptagon *h = get<0>(p);
      transmatrix V = get<1>(p);
      dynamicval<ld> b(band_shift, get<2>(p));
      bandfixer bf(V);
      dq::drawqueue.pop();
            
      cell *c = h->c7;
      if(!do_draw(c, V)) continue;
      drawcell(c, V, 0, false);

      for(int i=0; i<6; i++)
        dq::enqueue(h->move(i), V * cpush(i%3, (i>=3) ? -1 : 1));
      }
    }
  
  transmatrix relative_matrix(heptagon *h2, heptagon *h1) {
    auto cm = cubemap();
    coord a = cm->ispacemap[h2] - cm->ispacemap[h1];
    return eupush3(getcoord(a, 0), getcoord(a, 1), getcoord(a, 2));
    }

  int celldistance(cell *c1, cell *c2) {
    auto cm = cubemap();
    coord a = cm->ispacemap[c1->master] - cm->ispacemap[c2->master];
    return abs(getcoord(a, 0)) + abs(getcoord(a, 1)) + abs(getcoord(a, 2));
    }

  }
#endif
}
