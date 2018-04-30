// Hyperbolic Rogue -- cells
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// cells the game is played on

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
  cell *c = new cell;
  c->type = type;
  c->master = master;
  for(int i=0; i<MAX_EDGE; i++) c->mov[i] = NULL;
  initcell(c);
  return c;
  }

void merge(cell *c, int d, cell *c2, int d2, bool mirrored = false) {
  c->mov[d] = c2;
  tsetspin(c->spintable, d, d2 + (mirrored?8:0));
  c2->mov[d2] = c;
  tsetspin(c2->spintable, d2, d + (mirrored?8:0));
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
  origin = new heptagon;
  heptagon& h = *origin;
  h.s = hsOrigin;
  h.emeraldval = a46 ? 0 : 98;
  h.zebraval = 40;
  h.fiftyval = 0;
  h.fieldval = 0;
  h.rval0 = h.rval1 = 0;
  h.cdata = NULL;
  for(int i=0; i<MAX_EDGE; i++) h.move[i] = NULL;
  h.spintable = 0;
  h.alt = NULL;
  h.distance = 0;
  isnonbitrunc = nonbitrunc;
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
  bool isnonbitrunc;

  hrmap_spherical() {
    isnonbitrunc = nonbitrunc;
    for(int i=0; i<spherecells(); i++) {
      heptagon& h = *(dodecahedron[i] = new heptagon);
      h.s = hsOrigin;
      h.emeraldval = i;
      h.zebraval = i;
      h.fiftyval = i;
      h.rval0 = h.rval1 = 0;
      h.alt = NULL;
      h.cdata = NULL;
      h.spintable = 0;
      h.fieldval = i;
      for(int i=0; i<S7; i++) h.move[i] = NULL;
      h.c7 = newCell(S7, &h);
      }
    if(S7 == 5)
      siblings = {1, 0, 10, 4, 3, 8, 9, 11, 5, 6, 2, 7};
    else
      siblings = {1, 0, 3, 2, 5, 4};
    for(int i=0; i<S7; i++) {
      dodecahedron[0]->move[i] = dodecahedron[i+1];
      dodecahedron[0]->setspin(i, 0);
      dodecahedron[i+1]->move[0] = dodecahedron[0];
      dodecahedron[i+1]->setspin(0, i);
      
      dodecahedron[i+1]->move[1] = dodecahedron[(i+S7-1)%S7+1];
      dodecahedron[i+1]->setspin(1, S7-1);
      dodecahedron[i+1]->move[S7-1] = dodecahedron[(i+1)%S7+1];
      dodecahedron[i+1]->setspin(S7-1, 1);
      
      if(S7 == 5 && elliptic) {
        dodecahedron[i+1]->move[2] = dodecahedron[(i+2)%S7+1];
        dodecahedron[i+1]->setspin(2, 3 + 8);
        dodecahedron[i+1]->move[3] = dodecahedron[(i+3)%S7+1];
        dodecahedron[i+1]->setspin(3, 2 + 8);
        }

      else if(S7 == 5) {
        dodecahedron[6]->move[i] = dodecahedron[7+i];
        dodecahedron[6]->setspin(i, 0);
        dodecahedron[7+i]->move[0] = dodecahedron[6];
        dodecahedron[7+i]->setspin(0, i);
  
        dodecahedron[i+7]->move[1] = dodecahedron[(i+4)%5+7];
        dodecahedron[i+7]->setspin(1, 4);
        dodecahedron[i+7]->move[4] = dodecahedron[(i+1)%5+7];
        dodecahedron[i+7]->setspin(4, 1);
        
        dodecahedron[i+1]->move[2] = dodecahedron[7+(10-i)%5];
        dodecahedron[i+1]->setspin(2, 2);
        dodecahedron[7+(10-i)%5]->move[2] = dodecahedron[1+i];
        dodecahedron[7+(10-i)%5]->setspin(2, 2);
  
        dodecahedron[i+1]->move[3] = dodecahedron[7+(9-i)%5];
        dodecahedron[i+1]->setspin(3, 3);
        dodecahedron[7+(9-i)%5]->move[3] = dodecahedron[i+1];
        dodecahedron[7+(9-i)%5]->setspin(3, 3);
        }
      if(S7 == 4) {
        dodecahedron[5]->move[3-i] = dodecahedron[i+1];
        dodecahedron[5]->setspin(3-i, 2);
        dodecahedron[i+1]->move[2] = dodecahedron[5];
        dodecahedron[i+1]->setspin(2, 3-i);
        }
      }
    }

  heptagon *getOrigin() { return dodecahedron[0]; }

  ~hrmap_spherical() {
    dynamicval<bool> ph(nonbitrunc, isnonbitrunc);
    for(int i=0; i<spherecells(); i++) clearHexes(dodecahedron[i]);
    for(int i=0; i<spherecells(); i++) delete dodecahedron[i];
    }    

  void verify() {
    for(int i=0; i<spherecells(); i++) for(int k=0; k<S7; k++) {
      heptspin hs;
      hs.h = dodecahedron[i];
      hs.spin = k;
      hs = hs + wstep + (S7-1) + wstep + (S7-1) + wstep + (S7-1);
      if(hs.h != dodecahedron[i]) printf("error %d,%d\n", i, k);
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
  
  enum eTorusMode { 
    tmSingleHex, 
    tmSingle, 
    tmSlantedHex, 
    tmStraight, 
    tmStraightHex,
    tmKlein,
    tmKleinHex
    };
  
  static const flagtype TF_SINGLE = 1;
  static const flagtype TF_SIMPLE = 2;
  static const flagtype TF_WEIRD  = 4;

  static const flagtype TF_HEX    = 16;
  static const flagtype TF_SQUARE = 32;

  static const flagtype TF_KLEIN = 256;
  
  struct torusmode_info {
    string name;
    flagtype flags;
    };
  
  vector<torusmode_info> tmodes = {
    {"single row (hex)", TF_SINGLE | TF_HEX},
    {"single row (squares)", TF_SINGLE | TF_SQUARE},
    {"parallelogram (hex)", TF_SIMPLE | TF_HEX},
    {"rectangle (squares)", TF_SIMPLE | TF_SQUARE},
    {"rectangle (hex)", TF_WEIRD | TF_HEX},
    {"Klein bottle (squares)", TF_SIMPLE | TF_KLEIN | TF_SQUARE},
    {"Klein bottle (hex)", TF_WEIRD | TF_KLEIN | TF_HEX},
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
    }
  }

int decodeId(heptagon* h);
heptagon* encodeId(int id);

int euclid_getvec(int dx, int dy) {
  if(torus) return torusconfig::getvec(dx, dy);
  else return pair_to_vec(dx, dy);
  }

template<class T> void build_euclidean_moves(cell *c, int vec, const T& builder) {
  int x, y;
  tie(x,y) = vec_to_pair(vec);
  c->type = a4 ? (nonbitrunc || ((x^y^1) & 1) ? 4 : 8) : 6;

  if(c->type == 4) {
    int m = nonbitrunc ? 1 : 2;
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
        all[i]->mov[d] = all[im.first];
        tsetspin(all[i]->spintable, d, im.second);
        });
      }
    for(cell *c: all) for(int d=0; d<c->type; d++) {
      cell *c2 = c->mov[d];
      for(int d2=0; d2<c2->type; d2++) 
        if(c2->mov[d2] == c)
          tsetspin(c->spintable, d, d2 + (8 * c->spin(d)));
      }
    celllister cl(gamestart(), 100, 100000000, NULL);
    dists.resize(q);
    for(int i=0; i<size(cl.lst); i++)
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
    return euclideanAtCreate(0);
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
  
  cell*& at(int vec) {
    auto p = vec_to_pair(vec);
    int x = p.first, y = p.second;
    euclideanSlab*& slab = euclidean[(y>>8)&(slabs-1)][(x>>8)&(slabs-1)];
    if(!slab) slab = new hrmap_euclidean::euclideanSlab;
    return slab->a[y&255][x&255];
    }
  
  map<heptagon*, struct cdata> eucdata;

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
  if(!torus) 
    return cellwalker(euclideanAtCreate(vec), 0, false);
  else {
    hrmap_torus *cur = torusmap();
    if(!cur) return cellwalker(NULL, 0);
    auto p = torusconfig::vec_to_id_mirror(vec);
    return cellwalker(cur->all[p.first], 0, p.second);
    }
  }

int cellwalker_to_vec(cellwalker cw) {
  int id = decodeId(cw.c->master);
  if(!torus) return id;
  return torusconfig::id_to_vec(id, cw.mirrored);
  }

int cell_to_vec(cell *c) {
  int id = decodeId(c->master);
  if(!torus) return id;
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
    res.c[0] = cod(hs.h);
    for(int i=1; i<=S7; i++) {
      res.c[i] = cod((hs + wstep).h);
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

    if(quotient == 2) {
      connections = currfp.connections;
      }
    else {
      heptspin hs; hs.h = base.origin; hs.spin = 0;
      reachable.clear();
      bfsq.clear();
      connections.clear();
      add(hs);

      for(int i=0; i<(int)bfsq.size(); i++) {
        hs = bfsq[i] + wstep;
        add(hs);
        connections.push_back(reachable[get(hs)]);
        }

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
        h->c7 = newCell(S7, h);
        }
      for(int j=0; j<S7; j++) {
        h->move[rv(j)] = allh[connections[i*S7+j]/S7];
        h->setspin(rv(j), rv(connections[i*S7+j]%S7));
        }
      }
  
    for(int i=0; i<TOT; i++) {
      generateAlts(allh[i], S3-3, false);
      allh[i]->emeraldval = allh[i]->alt->emeraldval;
      allh[i]->zebraval = allh[i]->alt->zebraval;
      allh[i]->fiftyval = allh[i]->alt->fiftyval;
      allh[i]->distance = allh[i]->alt->distance;
      /* for(int j=0; j<7; j++)
        allh[i]->move[j]->alt = createStep(allh[i]->alt, j); */
      }    
    
    celllister cl(gamestart(), 100, 100000000, NULL);
    celllist = cl.lst;
    }

  heptagon *getOrigin() { return allh[0]; }

  ~hrmap_quotient() {
    for(int i=0; i<size(allh); i++) {
      clearHexes(allh[i]);
      delete allh[i];
      }
    }
  
  vector<cell*>& allcells() { return celllist; }
  };

  };

// --- general ---

cell *createMov(cell *c, int d);

cellwalker& operator += (cellwalker& cw, int spin) {
  cw.spin = (cw.spin+(MIRR(cw)?-spin:spin) + MODFIXER) % cw.c->type;
  return cw;
  }

cellwalker& operator += (cellwalker& cw, wstep_t) {
  createMov(cw.c, cw.spin);
  int nspin = cw.c->spn(cw.spin);
  if(cw.c->mirror(cw.spin)) cw.mirrored = !cw.mirrored;
  cw.c = cw.c->mov[cw.spin];
  cw.spin = nspin;
  return cw;
  }

cellwalker& operator -= (cellwalker& cw, int i) { return cw += (-i); }

cellwalker& operator += (cellwalker& cw, wmirror_t) {
  cw.mirrored = !cw.mirrored;
  return cw;
  }

template <class T> cellwalker operator + (cellwalker h, T t) { return h += t; }
template <class T> cellwalker operator - (cellwalker h, T t) { return h += (-t); }

cellwalker& operator ++ (cellwalker& h, int) { return h += 1; }
cellwalker& operator -- (cellwalker& h, int) { return h -= 1; }

bool cwstepcreates(cellwalker& cw) {
  return cw.c->mov[cw.spin] == NULL;
  }

/*
cell *cwpeek(cellwalker cw, int dir) {
  return (cw+dir+wstep).c;.
  // return createMov(cw.c, (cw.spin+MODFIXER+(MIRR(cw)?-dir:dir)) % cw.c->type);
  } */

void cwmirrorat(cellwalker& cw, int d) {
  cw.spin = (d+d - cw.spin + MODFIXER) % cw.c->type;
  cw.mirrored = !cw.mirrored;
  }

static const struct rev_t { rev_t() {} } rev;

cellwalker& operator += (cellwalker& cw, rev_t) {
  cw += cw.c->type/2 + ((cw.c->type&1)?hrand(2):0);
  return cw;
  }

static const struct revstep_t { revstep_t() {}} revstep;

cellwalker& operator += (cellwalker& cw, revstep_t) {
  cw += rev; cw += wstep; return cw;
  }

// very similar to createMove in heptagon.cpp
cell *createMov(cell *c, int d) {
  if(d<0 || d>= c->type) {
    printf("ERROR createmov\n");
    }

  if(euclid && !c->mov[d]) {
    int id = decodeId(c->master);
    for(int dx=-1; dx<=1; dx++)
    for(int dy=-1; dy<=1; dy++)
      euclideanAtCreate(id + pair_to_vec(dx, dy));
    if(!c->mov[d]) { printf("fail!\n"); }
    }
  
  if(c->mov[d]) return c->mov[d];
  else if(nonbitrunc && gp::on) {
    gp::extend_map(c, d);
    if(!c->mov[d]) {
      printf("extend failed to create for %p/%d\n", c, d);
      exit(1);
      }
    }
  else if(nonbitrunc) {
    heptagon *h2 = createStep(c->master, d);
    merge(c,d,h2->c7,c->master->spin(d),false);
    }
  else if(c == c->master->c7) {
    
    cell *n = newCell(S6, c->master);

    merge(c,d,n,0,false);
    
    heptspin hs; hs.h = c->master; hs.spin = d; hs.mirrored = false;
    
    int alt3 = c->type/2;
    int alt4 = alt3+1;
        
    /*
    heptspin hs2 = hsstep(hsspin(hs, a3), -alt4);
    merge(hs2.h->c7, hs2.spin, n, 2, hs2.mirrored);
    
    heptspin hs3 = hsstep(hsspin(hs, a4), -alt3);
    merge(hs3.h->c7, hs3.spin, n, S6-2, hs3.mirrored);
    */
    
    for(int u=2; u<S6; u+=2) {
      hs = hs + alt3 + wstep - alt4;
      merge(hs.h->c7, hs.spin, n, u, hs.mirrored);
      }
    
    extern void verifycell(cell *c);
    verifycell(n);
    }

  else {
    bool mirr = c->mirror(d-1);
    int di = fixrot(c->spn(d-1)-(mirr?-1:1));
    cell *c2 = createMov(c->mov[d-1], di);
    bool nmirr = mirr ^ c->mov[d-1]->mirror(di);
    merge(c, d, c2, fix6(c->mov[d-1]->spn(di) - (nmirr?-1:1)), nmirr);
    }
  return c->mov[d];
  }

cell *createMovR(cell *c, int d) {
  d %= MODFIXER; d += MODFIXER; d %= c->type;
  return createMov(c, d);
  }

cell *getMovR(cell *c, int d) {
  d %= MODFIXER; d += MODFIXER; d %= c->type;
  return c->mov[d];
  }

void eumerge(cell* c1, cell *c2, int s1, int s2) {
  if(!c2) return;
  c1->mov[s1] = c2; tsetspin(c1->spintable, s1, s2);
  c2->mov[s2] = c1; tsetspin(c2->spintable, s2, s1);
  }       

//  map<pair<eucoord, eucoord>, cell*> euclidean;

cell*& euclideanAt(int vec) {
  if(torus) { printf("euclideanAt called\n"); exit(1); }
  hrmap_euclidean* euc = dynamic_cast<hrmap_euclidean*> (currentmap);
  return euc->at(vec);
  }

cell*& euclideanAtCreate(int vec) {
  cell*& c = euclideanAt(vec);
  if(!c) {
    c = newCell(8, encodeId(vec));
    euclideanAt(vec) = c;
    build_euclidean_moves(c, vec, [c,vec] (int delta, int d, int d2) { eumerge(c, euclideanAt(vec + delta), d, d2); });
    }
  return c;
  }

// initializer (also inits origin from heptagon.cpp)
void initcells() {
  DEBB(DF_INIT, (debugfile,"initcells\n"));
  
  if(torus) currentmap = new hrmap_torus;
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
  for(int t=0; t<c->type; t++) if(c->mov[t]) {
    DEBMEM ( printf("mov %p [%p] S%d\n", c->mov[t], c->mov[t]->mov[c->spn(t)], c->spn(t)); )
    if(c->mov[t]->mov[c->spn(t)] != NULL &&
      c->mov[t]->mov[c->spn(t)] != c) {
        printf("type = %d %d -> %d\n", c->type, t, c->spn(t));
        printf("cell error\n");
        exit(1);
        }
    c->mov[t]->mov[c->spn(t)] = NULL;
    }
  DEBMEM ( printf("DEL %p\n", c); )
  delete c;
  }

heptagon deletion_marker;

template<class T> void subcell(cell *c, const T& t) {
  if(gp::on) {
    forCellEx(c2, c) if(c2->mov[0] == c && c2 != c2->master->c7) {
      subcell(c2, t);
      }
    }
  else if(!nonbitrunc)
    forCellEx(c2, c) t(c2);
  t(c);
  }

void clearHexes(heptagon *at) {
  if(at->c7 && at->cdata) {
    delete at->cdata;
    at->cdata = NULL;
    }
  if(at->c7) subcell(at->c7, clearcell);
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
    for(int i=0; i<S7; i++) if(at->move[i]) {
      if(at->move[i]->alt != &deletion_marker)
        q.push(at->move[i]);    
      unlink_cdata(at->move[i]);
      at->move[i]->alt = &deletion_marker;
      DEBMEM ( printf("!mov %p [%p]\n", at->move[i], at->move[i]->move[at->spin(i)]); )
      if(at->move[i]->move[at->spin(i)] != NULL &&
        at->move[i]->move[at->spin(i)] != at) {
          printf("hept error\n");
          exit(1);
          }
      at->move[i]->move[at->spin(i)] = NULL;
      at->move[i] = NULL;
      }
    clearHexes(at);
    delete at;
    }
//printf("maxq = %d\n", maxq);
  }

void verifycell(cell *c) {
  int t = c->type;
  for(int i=0; i<t; i++) {
    cell *c2 = c->mov[i];
    if(c2) {
      if(!euclid && !nonbitrunc && c == c->master->c7) verifycell(c2);
      if(c2->mov[c->spn(i)] && c2->mov[c->spn(i)] != c) {
        printf("cell error %p:%d [%d] %p:%d [%d]\n", c, i, c->type, c2, c->spn(i), c2->type);
        exit(1);
        }
      }
    }
  }

void verifycells(heptagon *at) {
  if(gp::on) return;
  for(int i=0; i<S7; i++) if(at->move[i] && at->move[i]->move[at->spin(i)] && at->move[i]->move[at->spin(i)] != at) {
    printf("hexmix error %p [%d s=%d] %p %p\n", at, i, at->spin(i), at->move[i], at->move[i]->move[at->spin(i)]);
    }
  if(!sphere && !quotient) 
    for(int i=0; i<S7; i++) if(at->move[i] && at->spin(i) == 0 && at->s != hsOrigin)
      verifycells(at->move[i]);
  verifycell(at->c7);
  }

int eudist(int sx, int sy) {
  int z0 = abs(sx);
  int z1 = abs(sy);
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
  if(euclid) {
    if(torus) 
      return torusmap()->dists[decodeId(c->master)];
    return eudist(decodeId(c->master));
    }
  if(sphere) return celldistance(c, currentmap->gamestart());
  if(ctof(c)) return c->master->distance;
  if(gp::on) return gp::compute_dist(c, celldist);
  int dx[MAX_S3];
  for(int u=0; u<S3; u++)
    dx[u] = createMov(c, u+u)->master->distance;
  return compdist(dx);
  }

#define ALTDIST_BOUNDARY 99999
#define ALTDIST_UNKNOWN 99998

#define ALTDIST_ERROR 90000

// defined in 'game'
int euclidAlt(short x, short y);

int celldistAlt(cell *c) {
  if(euclid) {
    if(torus) return celldist(c);
    int x, y;
    tie(x,y) = vec_to_pair(decodeId(c->master));
    return euclidAlt(x, y);
    }
  if(sphere || quotient) {
    return celldist(c) - 3;
    }
  if(!c->master->alt) return 0;
  if(ctof(c)) return c->master->alt->distance;
  if(gp::on) return gp::compute_dist(c, celldistAlt);
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
  for(int i=0; i<cfrom->type; i++) if(cfrom->mov[i] == cto) return i;
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
  
  cdata mydata = *getHeptagonCdata(h->move[0]);

  for(int di=3; di<5; di++) {
    heptspin hs; hs.h = h; hs.spin = di;
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
      
      if(hstab[3].h->distance < hstab[7].h->distance) {
        hs = hstab[3]; continue;
        }

      if(hstab[11].h->distance < hstab[7].h->distance) {
        hs = hstab[11]; continue;
        }
      
      int jj = 7;
      for(int k=3; k<12; k++) if(hstab[k].h->distance < hstab[jj].h->distance) jj = k;
      
      int ties = 0, tiespos = 0;
      for(int k=3; k<12; k++) if(hstab[k].h->distance == hstab[jj].h->distance) 
        ties++, tiespos += (k-jj);
        
      // printf("ties=%d tiespos=%d jj=%d\n", ties, tiespos, jj);
      if(ties == 2) jj += tiespos/2;
      
      if(jj&1) signum = -1;
      hs = hstab[jj];
      
      break;
      }
    hs = hs + 3 + wstep;
    setHeptagonRval(hs.h);
    
    affect(mydata, hs.spin ? hs.h->rval0 : hs.h->rval1, signum);

    /* if(!(spins[hs.h] & hs.spin)) {
      spins[hs.h] |= (1<<hs.spin);
      int t = 0;
      for(int k=0; k<7; k++) if(spins[hs.h] & (1<<k)) t++;
      static bool wast[256];
      if(!wast[spins[hs.h]]) {
        printf("%p %4x\n", hs.h, spins[hs.h]);
        wast[spins[hs.h]] = true;
        }
      } */
    }

  return h->cdata = new cdata(mydata);
  }

cdata *getEuclidCdata(heptagon *h) {

  if(torus) {
    static cdata xx;
    return &xx;
    }
    
  int x, y;
  hrmap_euclidean* euc = dynamic_cast<hrmap_euclidean*> (currentmap);
  if(euc->eucdata.count(h)) return &(euc->eucdata[h]);
  
  tie(x,y) = vec_to_pair(decodeId(h));

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

    cdata *d1 = getEuclidCdata(encodeId(pair_to_vec(x1,y1)));
    cdata *d2 = getEuclidCdata(encodeId(pair_to_vec(x2,y2)));
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
  if(euclid) return getEuclidCdata(c->master)->val[j];
  else if(geometry) return 0;
  else if(ctof(c)) return getHeptagonCdata(c->master)->val[j]*3;
  else {
    int jj = 0;
    auto ar = gp::get_masters(c);
    for(int k=0; k<3; k++)
      jj += getHeptagonCdata(ar[k]->master)->val[j];
    return jj;
    }
  }

int getBits(cell *c) {
  if(euclid) return getEuclidCdata(c->master)->bits;
  else if(geometry) return 0;
  else if(c->type != 6) return getHeptagonCdata(c->master)->bits;
  else {
    auto ar = gp::get_masters(c);
    int b0 = getHeptagonCdata(ar[0]->master)->bits;
    int b1 = getHeptagonCdata(ar[1]->master)->bits;
    int b2 = getHeptagonCdata(ar[2]->master)->bits;
    return (b0 & b1) | (b1 & b2) | (b2 & b0);
    }
  }

cell *heptatdir(cell *c, int d) {
  if(d&1) {
    cell *c2 = createMov(c, d);
    int s = c->spin(d);
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
    for(int i=0; i<S7; i++) if(h1->move[i] == h2) return d + 1;
    int d1 = h1->distance, d2 = h2->distance;
    if(d1 >= d2) d++, h1 = h1->move[0];
    if(d2 >  d1) d++, h2 = h2->move[0];
    }
  }

int heptdistance(cell *c1, cell *c2) {
  if(!hyperbolic || quotient) return celldistance(c1, c2);
  else return heptdistance(c1->master, c2->master);
  }

map<pair<cell*, cell*>, int> saved_distances;

int celldistance(cell *c1, cell *c2) {
  int d = 0;
  
  if(euclid) {
    if(torus) 
      return torusmap()->dists[torusconfig::vec_to_id(decodeId(c1->master)-decodeId(c2->master))];
    return eudist(decodeId(c1->master) - decodeId(c2->master));
    }
  
  if(quotient == 2 && !gp::on)
    return currfp.getdist(fieldpattern::fieldval(c1), fieldpattern::fieldval(c2));

  if(sphere || quotient) {
    
    if(saved_distances.count(make_pair(c1,c2)))
      return saved_distances[make_pair(c1,c2)];

    celllister cl(c1, 100, 100000000, NULL);
    for(int i=0; i<size(cl.lst); i++)
      saved_distances[make_pair(c1, cl.lst[i])] = cl.dists[i];

    if(saved_distances.count(make_pair(c1,c2)))
      return saved_distances[make_pair(c1,c2)];

    return 64;
    }
  
  if(gp::on) {
    
    if(saved_distances.count(make_pair(c1,c2)))
      return saved_distances[make_pair(c1,c2)];

    celllister cl(c1, 64, 1000, c2);

    for(int i=0; i<size(cl.lst); i++)
      saved_distances[make_pair(c1, cl.lst[i])] = cl.dists[i];

    if(saved_distances.count(make_pair(c1,c2)))
      return saved_distances[make_pair(c1,c2)];

    return 64;
    }
  
  int d1 = celldist(c1), d2 = celldist(c2);

  cell *cl1=c1, *cr1=c1, *cl2=c2, *cr2=c2;
  while(true) {
  
  if(weirdhyperbolic) {
      if(cl1 == cl2) return d;
      if(cl1 == cr2) return d;
      if(cr1 == cl2) return d;
      if(cr1 == cr2) return d;
          
      if(isNeighbor(cl1, cl2)) return d+1;
      if(isNeighbor(cl1, cr2)) return d+1;
      if(isNeighbor(cr1, cl2)) return d+1;
      if(isNeighbor(cr1, cr2)) return d+1;
      }
    
    if(d1 == d2) for(int u=0; u<2; u++) {
      cell *ac0 = u ? cr1 : cr2, *ac = ac0;
      cell *tgt = u ? cl2 : cl1;
      cell *xtgt = u ? cr2 : cr1;
      if(ac == tgt) return d;
      ac = chosenDown(ac, 1, 1, celldist);
      if(ac == tgt) return d+1; 
      if(ac == xtgt) return d;
      ac = chosenDown(ac, 1, 1, celldist);
      if(ac == tgt) return d+2;
      if(!nonbitrunc) {
        ac = chosenDown(ac, 1, 1, celldist);
        if(ac == tgt) {
          if(chosenDown(ac0, 1, 0, celldist) ==
            chosenDown(tgt, -1, 0, celldist))
              return d+2; 
          return d+3;
          }
        }
      }  
    
    if(weirdhyperbolic) {
      forCellEx(c, cl2) if(isNeighbor(c, cr1)) return d+2;
      forCellEx(c, cl1) if(isNeighbor(c, cr2)) return d+2;

      forCellEx(ca, cl2) forCellEx(cb, cr1) if(isNeighbor(ca, cb)) return d+3;
      forCellEx(ca, cl1) forCellEx(cb, cr2) if(isNeighbor(ca, cb)) return d+3;

      forCellEx(ca, cl2) forCellEx(cb, cr1) forCellEx(cc, cb) if(isNeighbor(ca, cc)) return d+4;
      forCellEx(ca, cl1) forCellEx(cb, cr2) forCellEx(cc, cb) if(isNeighbor(ca, cc)) return d+4;
      }

    if(d1 >= d2) {
      cl1 = chosenDown(cl1, -1, 0, celldist);
//    cl1->item = eItem(rand() % 10);
      cr1 = chosenDown(cr1,  1, 0, celldist);
//    cr1->item = eItem(rand() % 10);
      d++; d1--;
      }
    if(d1 < d2) {
      cl2 = chosenDown(cl2, -1, 0, celldist);
//    cl2->item = eItem(rand() % 10);
      cr2 = chosenDown(cr2,  1, 0, celldist);
//    cr2->item = eItem(rand() % 10);
      d++; d2--;
      }
    }
  }

void clearCellMemory() {
  for(int i=0; i<size(allmaps); i++) 
    if(allmaps[i])
      delete allmaps[i];
  allmaps.clear();
  last_cleared = NULL;
  saved_distances.clear();
  pd_from = NULL;
  }

auto cellhooks = addHook(clearmemory, 500, clearCellMemory);

