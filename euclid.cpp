// Hyperbolic Rogue -- Euclidean geometry, including 2D, 3D, and quotient spaces
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

// 2D Euclidean space

// --- euclidean geometry ---

// NOTE: patterns assume that pair_to_vec(0,1) % 3 == 2!
// Thus, pair_to_vec(0,1) must not be e.g. a power of four

EX int cell_to_vec(cell *c);

EX int pair_to_vec(int x, int y) {
  return x + (y << 15);
  }

EX pair<int, int> vec_to_pair(int vec) {
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

struct hrmap_euclid_any : hrmap {
  void draw() override;
  };

struct hrmap_torus : hrmap_euclid_any {

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

  transmatrix relative_matrix(cell *c2, cell *c1, const hyperpoint& point_hint) {
    transmatrix t = Id;
    // if(whateveri) printf("[%p,%d] ", c2, celldistance(c2, c1));
    int d = celldistance(c2, c1);
    while(d) {
      forCellIdEx(cc, i, c1) {
        int d1 = celldistance(cc, c2);
        if(d1 < d) {
          t = t * cellrelmatrix(c1, i);
          c1 = cc;
          d = d1;
          goto again;
          }
        }
      printf("ERROR not reached\n");
      break;
      again: ;
      }
    return t;
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

struct hrmap_euclidean : hrmap_euclid_any {

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

  transmatrix relative_matrix(cell *c2, cell *c1, const hyperpoint& point_hint) {
    return eumove(cell_to_vec(c2) - cell_to_vec(c1));
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

EX int cell_to_vec(cell *c) {
  int id = decodeId(c->master);
  if(!fulltorus) return id;
  return torusconfig::id_to_vec(id, false);
  }

EX pair<int, int> cell_to_pair(cell *c) {
  return vec_to_pair(cell_to_vec(c));
  }

union heptacoder {
  heptagon *h;
  int id;
  };

EX int decodeId(heptagon* h) {
  heptacoder u;
  u.h = h; return u.id;
  }

EX heptagon* encodeId(int id) {
  heptacoder u;
  u.id = id;
  return u.h;
  }

// 3D Euclidean space

#if MAXMDIM == 4

namespace euclid3 {

  typedef long long coord; 
  static const long long COORDMAX = (1<<16);
  typedef array<coord, 3> axes;  
  typedef array<array<int, 3>, 3> intmatrix;  


  static const axes main_axes = make_array<coord>(1, COORDMAX, COORDMAX * COORDMAX );
  
  array<int, 3> getcoord(coord x) {
    array<int, 3> res;
    for(int k=0; k<3; k++) {
      int next = x % COORDMAX;
      if(next>COORDMAX/2) next -= COORDMAX; 
      if(next<-COORDMAX/2) next += COORDMAX; 
      res[k] = next;
      x -= next;
      x /= COORDMAX;
      }
    return res; 
    }

  vector<coord> get_shifttable() {
    static const coord D0 = main_axes[0];
    static const coord D1 = main_axes[1];
    static const coord D2 = main_axes[2];
    vector<coord> shifttable;
    switch(geometry) {
      case gCubeTiling:
        shifttable = { +D0, +D1, +D2 };
        break;
      
      case gRhombic3:
        shifttable = { D0+D1, D0+D2, D1+D2, D1-D2, D0-D2, D0-D1 };
        break;
      
      case gBitrunc3:
        shifttable = { 2*D0, 2*D1, 2*D2, D0+D1+D2, D0+D1-D2, D0-D1-D2, D0-D1+D2 };
        break;
      
      default:
        printf("euclid3::get_shifttable() called in geometry that is not euclid3");
        exit(1);
      }
    
    // reverse everything
    int s = isize(shifttable);
    for(int i=0; i<s; i++) shifttable.push_back(-shifttable[i]);
    return shifttable;
    }
  
  coord canonicalize(coord x);
  void build_torus3();
  coord twist(coord x, transmatrix& M);
  extern int twisted;
  extern intmatrix T0;
  
  struct hrmap_euclid3 : hrmap {
    vector<coord> shifttable;
    vector<transmatrix> tmatrix;
    map<coord, heptagon*> spacemap;
    map<heptagon*, coord> ispacemap;
    cell *camelot_center;

    vector<cell*> toruscells;  
    vector<cell*>& allcells() override { 
      if(bounded) {
        if(isize(toruscells) == 0) {
          celllister cl(getOrigin()->c7, 1000, 1000000, NULL);
          toruscells = cl.lst;
          }
        return toruscells;
        }
      return hrmap::allcells();
      }

    hrmap_euclid3() {
      shifttable = get_shifttable();
      tmatrix.resize(S7);
      for(int i=0; i<S7; i++) tmatrix[i] = Id;
      for(int i=0; i<S7; i++) for(int j=0; j<3; j++)
        tmatrix[i][j][DIM] = getcoord(shifttable[i])[j];
      camelot_center = NULL;
      build_torus3();
      }

    heptagon *getOrigin() override {
      return get_at(0);
      }

    heptagon *get_at(coord at) {
      if(spacemap.count(at)) 
        return spacemap[at];
      else {
        auto h = tailored_alloc<heptagon> (S7);
        h->c7 = newCell(S7, h);
        h->distance = 0;
        h->cdata = NULL;
        h->alt = NULL;
        auto co = getcoord(at);
        if(S7 != 14)
          h->zebraval = gmod(co[0] + co[1] * 2 + co[2] * 4, 5);
        else 
          h->zebraval = co[0] & 1;
        spacemap[at] = h;
        ispacemap[h] = at;
        return h;
        }
      }
    
    heptagon *build(heptagon *parent, int d, coord at) {
      auto h = get_at(at);
      int d1 = (d+S7/2)%S7;
      if(twisted) {
        coord a = ispacemap[parent];
        coord b = ispacemap[h];
        for(int i=0; i<S7; i++) 
          if(canonicalize(b + shifttable[i]) == a)
            d1 = i;
        }
      h->c.connect(d1, parent, d, false);
      return h;
      }
  
    heptagon *create_step(heptagon *parent, int d) override {
      return build(parent, d, canonicalize(ispacemap[parent] + shifttable[d]));
      }  

    transmatrix get_move(cell *c, int i) {
      if(!twisted) return tmatrix[i];
      transmatrix res = tmatrix[i];
      coord id = ispacemap[c->master];
      id += shifttable[i];
      twist(id, res);
      return res;
      }

    void draw() override {
      dq::visited_by_matrix.clear();
      dq::enqueue_by_matrix(viewctr.at, cview());
      
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
  
        for(int i=0; i<S7; i++)
          dq::enqueue_by_matrix(h->move(i), V * get_move(h->c7, i));
        }
      }
    
    transmatrix warppush(coord dif) {
      auto v = getcoord(dif);
      for(int i: {0, 1})
        if(T0[i][i])
          v[i] = gmod(v[i] + T0[i][i] / 2, T0[i][i]) - T0[i][i] / 2;
      return eupush3(v[0], v[1], v[2]);
      }
    
    transmatrix relative_matrix(heptagon *h2, heptagon *h1) override {
      if(twisted) {
        coord c1 = ispacemap[h1];
        coord c2 = ispacemap[h2];
        transmatrix T = warppush(c2 - c1);
        for(int d: {-1, 1}) {
          transmatrix I = Id;
          coord cs = c1;
          for(int s=0; s<3; s++) {
            cs += d * T0[2][2] * main_axes[2];
            I = I * eupush3(0, 0, d * T0[2][2]);
            cs = twist(cs, I);
            transmatrix T1 = I * warppush(c2 - cs);
            if(hdist0(tC0(T1)) < hdist0(tC0(T)))
              T = T1;
            }
          }
        return T;
        }
      auto d = ispacemap[h2] - ispacemap[h1];
      d = canonicalize(d);
      auto v = getcoord(d);
      return eupush3(v[0], v[1], v[2]);
      }
    
    vector<hyperpoint> get_vertices(cell* c) override {
      vector<hyperpoint> res;
      if(S7 < 14)
        for(ld a: {-.5,.5}) for(ld b: {-.5,.5}) for(ld c: {-.5, .5}) res.push_back(hpxy3(a,b,c));
      if(S7 == 12) {
        res.push_back(hpxy3(1,0,0));
        res.push_back(hpxy3(-1,0,0));
        res.push_back(hpxy3(0,1,0));
        res.push_back(hpxy3(0,-1,0));
        res.push_back(hpxy3(0,0,1));
        res.push_back(hpxy3(0,0,-1));
        }
      if(S7 == 14) {
        for(ld a: {-1.,-.5,0.,.5,1.})
        for(ld b: {-1.,-.5,0.,.5,1.})
        for(ld c: {-1.,-.5,0.,.5,1.})
          if(a == 0 || b == 0 || c == 0)
          if(a == .5 || a == -.5 || b == .5 || b == -.5 || c == .5 || c == -.5)
          if(a == 1 || a == -1 || b == 1 || b == -1 || c == 1 || c == -1)
            res.push_back(hpxy3(a,b,c));
        }
      return res;
      }
    };
  
  hrmap_euclid3* cubemap() {
    return ((hrmap_euclid3*) currentmap);
    }

  hrmap* new_map() {
    return new hrmap_euclid3;
    }

  transmatrix move_matrix(cell *c, int i) { 
    return cubemap()->get_move(c, i);
    }
  
  bool pseudohept(cell *c) {
    coord co = cubemap()->ispacemap[c->master];
    auto v = getcoord(co);
    if(S7 == 12) {
      for(int i=0; i<3; i++) if((v[i] & 1)) return false;
      }
    else {
      for(int i=0; i<3; i++) if(!(v[i] & 1)) return false;
      }
    return true;
    }

  int dist_alt(cell *c) {
    if(specialland == laCamelot) return dist_relative(c) + roundTableRadius(c);
    coord co = cubemap()->ispacemap[c->master];
    auto v = getcoord(co);
    if(S7 == 6) return v[2];
    else if(S7 == 12) return (v[0] + v[1] + v[2]) / 2;
    else return v[2]/2;
    }

  bool get_emerald(cell *c) {
    auto v = getcoord(cubemap()->ispacemap[c->master]);
    int s0 = 0, s1 = 0;
    for(int i=0; i<3; i++) {
      v[i] = gmod(v[i], 6);
      int d = min(v[i], 6-v[i]);;
      s0 += min(v[i], 6-v[i]);
      s1 += 3-d;
      }
    if(s0 == s1) println(hlog, "equality");
    return s0 > s1;
    }
  
  bool cellvalid(coord co) {
    auto v = getcoord(co);
    if(S7 == 6) return true;
    if(S7 == 12) return (v[0] + v[1] + v[2]) % 2 == 0;
    if(S7 == 14) return v[0] % 2 == v[1] % 2 && v[0] % 2 == v[2] % 2;
    return false;
    }

  int celldistance(coord co) {
    auto v = getcoord(co);
    if(S7 == 6) 
      return abs(v[0]) + abs(v[1]) + abs(v[2]);
    else {
      for(int i=0; i<3; i++) v[i] = abs(v[i]);
      sort(v.begin(), v.end());
      int dist = 0;
      if(S7 == 12) {
        int d = v[1] - v[0]; v[1] -= d; v[2] -= d;
        dist += d;
        int m = min((v[2] - v[0]), v[0]);
        dist += 2 * m;
        v[0] -= m; v[1] -= m; v[2] -= m * 2;
        if(v[0])
          dist += (v[0] + v[1] + v[2]) / 2;
        else
          dist += v[2];
        }
      else {
        dist = v[0] + (v[1] - v[0]) / 2 + (v[2] - v[0]) / 2;
        }
      return dist;
      }
    }

  int celldistance(cell *c1, cell *c2) {
    auto cm = cubemap();
    return celldistance(cm->ispacemap[c1->master] - cm->ispacemap[c2->master]);
    }

  void set_land(cell *c) {
    setland(c, specialland); 
    auto m = cubemap();
    auto co = getcoord(m->ispacemap[c->master]);
    
    int dv = 1;
    if(geometry != gCubeTiling) dv = 2;
  
    int hash = 0;
    for(int a=0; a<3; a++) hash = 1317 * hash + co[a] / 4;
  
    set_euland3(c, co[0]*120, co[1]*120, (co[1]+co[2]) / dv, hash);
    }
  
  int dist_relative(cell *c) {
    auto m = cubemap();
    auto& cc = m->camelot_center;
    int r = roundTableRadius(NULL);
    cell *start = m->gamestart();
    if(!cc) {
      cc = start;
      while(euclid3::celldistance(cc, start) < r + 5)
        cc = cc->cmove(hrand(cc->type));
      }
    
    return euclid3::celldistance(cc, c) - r;
    }
  
  /* quotient spaces */

  intmatrix make_intmatrix(axes a) {
    intmatrix T;
    T[0] = getcoord(a[0]);
    T[1] = getcoord(a[1]);
    T[2] = getcoord(a[2]);
    return T;
    }
  
  int determinant(const intmatrix T) {
    int det = 0;
    for(int i=0; i<3; i++) 
      det += T[0][i] * T[1][(i+1)%3] * T[2][(i+2)%3];
    for(int i=0; i<3; i++) 
      det -= T[0][i] * T[1][(i+2)%3] * T[2][(i+1)%3];
    return det;
    }
  
  intmatrix scaled_inverse(const intmatrix T) {
    intmatrix T2;
    for(int i=0; i<3; i++) 
    for(int j=0; j<3; j++) 
      T2[j][i] = (T[(i+1)%3][(j+1)%3] * T[(i+2)%3][(j+2)%3] - T[(i+1)%3][(j+2)%3] * T[(i+2)%3][(j+1)%3]);
    return T2;
    }
  
  axes user_axes;
  axes optimal_axes;
  axes regular_axes;
  
  intmatrix T, T2, T0, T_edit;
  int det;
  int coords;
  int twisted, twisted0, twisted_edit;
  
  void clear_torus3() {
    for(int i=0; i<3; i++) user_axes[i] = 0;
    }
  
  unordered_map<coord, int> canonical_hash;
  vector<coord> canonical_seq;
  int canonical_index;
  
  coord compute_cat(coord co) {
    auto coo = getcoord(co);
    coord cat = 0;
    for(int i=0; i<3; i++) {
      int val = T2[0][i] * coo[0] + T2[1][i] * coo[1] + T2[2][i] * coo[2];
      if(i < coords) val = gmod(val, det);
      cat += val * main_axes[i];
      }
    return cat;
    };
  
  
  void add_canonical(coord val) {
    auto cat = compute_cat(val);
    if(canonical_hash.count(cat)) return;
    canonical_hash[cat] = isize(canonical_seq);
    canonical_seq.push_back(val);
    }
  
  void build_torus3() {
  
    for(int i=0; i<3; i++) {
      user_axes[i] = 0;
      for(int j=0; j<3; j++) user_axes[i] += main_axes[j] * T0[i][j];
      }
  
    optimal_axes = user_axes;
    
    again:
    for(int i=0; i<3; i++) if(optimal_axes[i] < 0) optimal_axes[i] = -optimal_axes[i];
    if(optimal_axes[0] < optimal_axes[1]) swap(optimal_axes[0], optimal_axes[1]);
    if(optimal_axes[1] < optimal_axes[2]) swap(optimal_axes[1], optimal_axes[2]);
    if(optimal_axes[0] < optimal_axes[1]) swap(optimal_axes[0], optimal_axes[1]);
    for(int i=0; i<3; i++) {
      int i1 = (i+1) % 3;
      int i2 = (i+2) % 3;
      for(int a=-10; a<=10; a++)
      for(int b=-10; b<=10; b++) {
        coord cand = optimal_axes[i] + optimal_axes[i1] * a + optimal_axes[i2] * b;
        if(celldistance(cand) < celldistance(optimal_axes[i])) {
          optimal_axes[i] = cand;
          goto again; 
          }
        }
      }
  
    regular_axes = optimal_axes;
    coords = 0;
    for(int i=0; i<3; i++) if(optimal_axes[i]) coords++;
  
    int attempt = 0;
    next_attempt:
    for(int i=coords; i<3; i++)
      regular_axes[i] = main_axes[(attempt+i)%3];
      
    T = make_intmatrix(regular_axes);
    det = determinant(T);
    if(det == 0) { 
      attempt++;
      if(attempt == 3) {
        println(hlog, "weird singular!\n");
        exit(1);
        }
      goto next_attempt;
      }
      
    if(det < 0) det = -det;
    
    T2 = scaled_inverse(T);
    canonical_hash.clear();
    canonical_seq.clear();  
    canonical_index = 0;  
    add_canonical(0);
    
    twisted = twisted0;
    if(geometry != gCubeTiling && ((T0[0][0]+T0[2][2]) & 1)) twisted &=~ 1;
    if(geometry != gCubeTiling && ((T0[1][1]+T0[2][2]) & 1)) twisted &=~ 2;
    for(int i=0; i<3; i++) for(int j=0; j<3; j++)
      if(i != j && T0[i][j]) twisted = 0;
    if(T0[2][2] == 0) twisted = 0;
    if(T0[0][0] != T0[1][1]) twisted &= 3;
    
    for(eGeometry g: {gCubeTiling, gRhombic3, gBitrunc3}) {
      set_flag(ginf[g].flags, qANYQ, coords);
      set_flag(ginf[g].flags, qBOUNDED, coords == 3);
      bool nonori = false;
      if(twisted&1) nonori = !nonori;
      if(twisted&2) nonori = !nonori;
      if(twisted&4) nonori = !nonori;
      set_flag(ginf[g].flags, qNONORIENTABLE, nonori);      
      }
    }
  
  void swap01(transmatrix& M) {
    for(int i=0; i<4; i++) swap(M[i][0], M[i][1]);
    }

  coord twist(coord x, transmatrix& M) {
    auto coo = getcoord(x);
    while(coo[2] >= T0[2][2]) {
      coo[2] -= T0[2][2];
      if(twisted & 1) coo[0] *= -1, M = M * MirrorX;
      if(twisted & 2) coo[1] *= -1, M = M * MirrorY;
      if(twisted & 4) swap(coo[0], coo[1]), swap01(M);
      }
    while(coo[2] < 0) {
      coo[2] += T0[2][2];
      if(twisted & 4) swap(coo[0], coo[1]), swap01(M);
      if(twisted & 1) coo[0] *= -1, M = M * MirrorX;
      if(twisted & 2) coo[1] *= -1, M = M * MirrorY;
      }
    for(int i: {0,1})
      if(T0[i][i]) coo[i] = gmod(coo[i], T0[i][i]);
    return coo[0] * main_axes[0] + coo[1] * main_axes[1] + coo[2] * main_axes[2];
    }

  coord canonicalize(coord x) {
    if(twisted) {
      transmatrix M = Id;
      return twist(x, M);
      }
    if(coords == 0) return x;
    if(coords == 1) {
      while(celldistance(x + optimal_axes[0]) <= celldistance(x)) x += optimal_axes[0];
      while(celldistance(x - optimal_axes[0]) <  celldistance(x)) x -= optimal_axes[0];
      return x;
      }
    auto cat = compute_cat(x);
    auto& st = cubemap()->shifttable;
    while(!canonical_hash.count(cat)) {
      if(canonical_index == isize(canonical_seq)) throw hr_exception();
      auto v = canonical_seq[canonical_index++];
      for(auto s: st) add_canonical(v + s);
      }
    return canonical_seq[canonical_hash[cat]];
    }

  void prepare_torus3() {
    T_edit = T0;
    twisted_edit = twisted0;
    }

  void show_torus3() {
    cmode = sm::SIDE | sm::MAYDARK;
    gamescreen(1);  
    dialog::init(XLAT("3D Euclidean spaces"));
    for(int y=0; y<4; y++)
      dialog::addBreak(100);
    
    dialog::addBreak(50);

    bool nondiag = false;
    for(int i=0; i<3; i++) 
      for(int j=0; j<3; j++) 
        if(T_edit[i][j] && i != j) nondiag = true;
      
    if(nondiag) {
      dialog::addInfo(XLAT("twisting implemented only for diagonal matrices"));
      dialog::addBreak(200);
      }
    else if(T_edit[2][2] == 0) {
      dialog::addInfo(XLAT("nothing to twist"));
      dialog::addInfo(XLAT("change the bottom left corner"));
      dialog::addBreak(100);
      }
    else {
      if(geometry == gCubeTiling || (T_edit[0][0]+T_edit[2][2]) % 2 == 0)
        dialog::addBoolItem(XLAT("flip X coordinate"), twisted_edit & 1, 'x');
      else
        dialog::addBoolItem(XLAT("flipping X impossible"), twisted_edit & 1, 'x');
      dialog::add_action([] { twisted_edit ^= 1; });

      if(geometry == gCubeTiling || (T_edit[1][1]+T_edit[2][2]) % 2 == 0)
        dialog::addBoolItem(XLAT("flip Y coordinate"), twisted_edit & 2, 'y');
      else
        dialog::addBoolItem(XLAT("flipping Y impossible"), twisted_edit & 2, 'y');
      dialog::add_action([] { twisted_edit ^= 2; });

      if(T_edit[0][0] == T_edit[1][1])
        dialog::addBoolItem(XLAT("swap X and Y"), twisted_edit & 4, 'z');
      else
        dialog::addBoolItem(XLAT("swapping impossible"), twisted_edit & 4, 'z');
      dialog::add_action([] { twisted_edit ^= 4; });
      }
    
    dialog::addBreak(50);
    
    char xch = 'p';
    for(eGeometry g: {gCubeTiling, gRhombic3, gBitrunc3}) {
      dialog::addItem(XLAT(ginf[g].menu_displayed_name), xch++);
      dialog::add_action([g] {
        stop_game();
        set_geometry(g);
        T0 = T_edit;
        twisted0 = twisted_edit;
        start_game();
        });
      }
    dialog::addBreak(50);
    dialog::addBack();
    dialog::display();
    
    int i = -1;
    for(auto& v: dialog::items) if(v.type == dialog::diBreak) {
      if(i >= 0 && i < 3) {
        for(int j=0; j<3; j++) {
          char ch = 'a' + i * 3 + j;
          if(displayfr(dialog::dcenter + dialog::dfspace * 4 * (j-1), v.position, 2, dialog::dfsize, its(T_edit[j][i]), 0xFFFFFF, 8))
            getcstat = ch;
          dialog::add_key_action(ch, [=] {
            dialog::editNumber(T_edit[j][i], -10, +10, 1, 0, "", XLAT(
              "This matrix lets you play on the quotient spaces of three-dimensional. "
              "Euclidean space. Every column specifies a translation vector which "
              "takes you back to the starting point. For example, if you put "
              "set 2, 6, 0 on the diagonal, you get back to the starting point "
              "if you move 2 steps in the X direction, 6 steps in the Y direction "
              "(the quotient space is infinite in the Z direction).\n\n"
              "You can also introduce twists for diagonal matrices: after going "
              "the given number of steps in the Z direction, the space is also "
              "mirrored or rotated. (More general 'twisted' spaces are currently "
              "not implemented.)"
              )
              );
            });
          }
        }
      i++;
      }
    }

  #if CAP_COMMANDLINE
  int euArgs() {
    using namespace arg;
             
    if(0) ;
    else if(argis("-t3")) {
      PHASEFROM(2);
      stop_game();
      for(int i=0; i<3; i++)
      for(int j=0; j<3; j++) {
        shift(); T0[i][j] = argi();
        }
      build_torus3();
      }
    else if(argis("-twist3")) {
      PHASEFROM(2);
      stop_game();
      for(int i=0; i<3; i++)
      for(int j=0; j<3; j++) T0[i][j] = 0;
      
      for(int i=0; i<3; i++) {
        shift(); T0[i][i] = argi();
        }
      shift(); twisted0 = argi();
      build_torus3();
      }
    else if(argis("-twisttest")) {
      start_game();
      celllister cl(cwt.at, 10000, 10000, NULL);
      for(cell *c: cl.lst) {
        for(int i=0; i<S7; i++)
        for(int j=0; j<S7; j++)
        for(int k=0; k<S7; k++)
        for(int l=0; l<S7; l++)
          if(c->move(i) && c->move(k) && c->move(i)->move(j) == c->move(k)->move(l) && c->move(i)->move(j)) {
            transmatrix T1 = move_matrix(c, i) * move_matrix(c->move(i), j);
            transmatrix T2 = move_matrix(c, k) * move_matrix(c->move(k), l);
            if(!eqmatrix(T1, T2)) {
              println(hlog, c, " @ ", getcoord(cubemap()->ispacemap[c->master]), " : ", i, "/", j, "/", k, "/", l, " :: ", T1, " vs ", T2);
              exit(1);
              }
            }
        }
      }
  
    else return 1;
    return 0;
    }
  
  auto euhook = addHook(hooks_args, 100, euArgs);
  #endif
  }

#endif

EX ld matrixnorm(const transmatrix& Mat) {
  return Mat[0][DIM] * Mat[0][DIM] + Mat[1][DIM] * Mat[1][DIM] + Mat[2][DIM] * Mat[2][DIM];
  }
  
void hrmap_euclid_any::draw() {
  DEBB(DF_GRAPH, ("drawEuclidean\n"));
  sphereflip = Id;
  if(!centerover.at) centerover = cwt;
  // printf("centerover = %p player = %p [%d,%d]-[%d,%d]\n", lcenterover, cwt.c,
  //   mindx, mindy, maxdx, maxdy);
  int pvec = cellwalker_to_vec(centerover);
  
  typedef pair<int, int> euspot;
  
  const euspot zero = {0,0};
  
  set<euspot> visited = {zero};
  vector<euspot> dfs = {zero};

  ld centerd = matrixnorm(View);
  auto View0 = cview();
  
  for(int i=0; i<isize(dfs); i++) {
    int dx, dy;
    tie(dx, dy) = dfs[i];
    
    cellwalker cw = vec_to_cellwalker(pvec + euclid_getvec(dx, dy));
    if(!cw.at) continue;
    transmatrix Mat = View0 * eumove(dx, dy);
    torusconfig::torus_cx = dx;
    torusconfig::torus_cy = dy;

    if(true) {
      ld locald = matrixnorm(Mat);
      if(locald < centerd) centerd = locald, centerover = cw, View = inverse(actual_view_transform) * Mat;
      }

    if(do_draw(cw.at, Mat)) {
      drawcell(cw.at, cw.mirrored ? Mat * spin(-2*M_PI*cw.spin / cw.at->type) * Mirror : Mat, cw.spin, cw.mirrored);
      for(int x=-1; x<=+1; x++)
      for(int y=-1; y<=+1; y++) {
        euspot p(dx+x, dy+y);
        if(!visited.count(p)) visited.insert(p), dfs.push_back(p);
        }
      }
    }
  }

}
