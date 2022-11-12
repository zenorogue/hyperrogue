// Hyperbolic Rogue -- Crystal geometries
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file crystal.cpp
 *  \brief Multi-dimensional (aka crystal) geometries. 
 */

#include "hyper.h"
namespace hr {

EX namespace crystal {

#if HDR
static const int MAXDIM = 7;
static const int MAX_EDGE_CRYSTAL = 2 * MAXDIM;

struct coord : public array<int, MAXDIM> {
  coord operator + (coord b) { for(int i=0; i<MAXDIM; i++) b[i] += self[i]; return b; }  
  coord operator - (coord b) { for(int i=0; i<MAXDIM; i++) b[i] = self[i] - b[i]; return b; }  
  coord operator * (int x) { coord res; for(int i=0; i<MAXDIM; i++) res[i] = x * self[i]; return res; } 
  };

static const coord c0 = {};

struct ldcoord : public array<ld, MAXDIM> {
  friend ldcoord operator + (ldcoord a, ldcoord b) { ldcoord r; for(int i=0; i<MAXDIM; i++) r[i] = a[i] + b[i]; return r; }
  friend ldcoord operator - (ldcoord a, ldcoord b) { ldcoord r; for(int i=0; i<MAXDIM; i++) r[i] = a[i] - b[i]; return r; }
  friend ldcoord operator * (ldcoord a, ld v) { ldcoord r; for(int i=0; i<MAXDIM; i++) r[i] = a[i] * v; return r; }
  friend ldcoord operator / (ldcoord a, ld v) { ldcoord r; for(int i=0; i<MAXDIM; i++) r[i] = a[i] / v; return r; }
  friend ld operator | (ldcoord a, ldcoord b) { ld r=0; for(int i=0; i<MAXDIM; i++) r += a[i] * b[i]; return r; }
  };

static const ldcoord ldc0 = {};
#endif

#if CAP_CRYSTAL

/** Crystal can be bitruncated either by changing variation to bitruncated.
 *  In case of the 4D Crystal, the standard HyperRogue bitruncation becomes
 *  confused by having both the original and new vertices of degree 8.
 *  Hence Crystal implements its own bitruncation, which is selected/checked
 *  by setting ginf[gCrystal].vertex to 3. Additionally, this lets us double
 *  bitruncate.
 *  Function pure() checks for both kinds of bitruncation (or any other variations).
 */

EX bool pure() {
  return PURE && ginf[gCrystal].vertex == 4;
  }

EX bool view_coordinates = false;
bool view_east = false;

EX bool used_compass_inside;

EX ldcoord told(coord c) { ldcoord a; for(int i=0; i<MAXDIM; i++) a[i] = c[i]; return a; }
// strange number to prevent weird acting in case of precision errors
coord roundcoord(ldcoord c) { coord a; for(int i=0; i<MAXDIM; i++) a[i] = floor(c[i] + .5136); return a; }

EX ld compass_probability = 1;

int tocode(int cname) { return (1 << (cname >> 1)); }

void resize2(vector<vector<int>>& v, int a, int b, int z) {
  v.clear();
  v.resize(a);
  for(auto& w: v) w.resize(b, z);
  }

/** in the "pure" form, the adjacent vertices are internaly spaced by 2 */
const int FULLSTEP = 2;

/** to make space for the additional vertices which are added in the bitruncated version */
const int HALFSTEP = 1;

/** with variations, the connections of the vertex at coordinate v+FULLSTEP mirror the connections
 *  of the vertex at coordinate v. Therefore, the period of our construction is actually 2*FULLSTEP. */
const int PERIOD = 2 * FULLSTEP;

struct crystal_structure {
  int dir;
  int dim;
  
  vector<vector<int>> cmap;
  vector<vector<int>> next;
  vector<vector<int>> prev;
  vector<vector<int>> order;
  
  void coord_to_next() {
    resize2(next, 1<<dim, 2*dim, -1);
    for(int a=0; a<(1<<dim); a++) 
      for(int b=0; b<dir; b++)
        next[a][cmap[a][b]] = cmap[a][(b+1)%dir];
    println(hlog, next);
    }
  
  void next_to_coord() {
    resize2(cmap, 1<<dim, dir, -1);
    for(int a=0; a<(1<<dim); a++) {
      int at = 0;
      for(int b=0; b<dir; b++) {
        cmap[a][b] = at;
        at = next[a][at];
        }
      }
    println(hlog, "coordinate map is:\n", cmap);
    }

  void next_to_prev() {
    resize2(prev, 1<<dim, 2*dim, -1);
    for(int a=0; a<(1<<dim); a++)
      for(int b=0; b<dir; b++) {
        if(next[a][b] != -1)
          prev[a][next[a][b]] = b;
        }
    }

  void coord_to_order() {
    println(hlog, dir, dim);
    resize2(order, 1<<dim, 2*dim, -1);
    for(int a=0; a<(1<<dim); a++) 
    for(int b=0; b<dir; b++)
      order[a][cmap[a][b]] = b;
    println(hlog, order);
    }
  
  int count_bugs() {
    int bugcount = 0;
      
    for(int a=0; a<(1<<dim); a++) 
    for(int b=0; b<2*dim; b++) {
      if(next[a][b] == -1) continue;
      int qa = a, qb = b;
      for(int i=0; i<4; i++) {
        if(i == 2 && (qb != (b^1))) bugcount++;
        qa ^= tocode(qb);
        qb ^= 1;
        qb = next[qa][qb];
        }
      if(a != qa || b != qb) bugcount++;
      }
    
    return bugcount;
    }
  
  void next_insert(int a, int at, int val) {
    int pd = next[a].size();

    next[a].resize(pd + 2);
    next[a][val] = next[a][at];
    next[a][at] = val;
    next[a][val^1] = next[a][at^1];
    next[a][at^1] = val^1;

    prev[a].resize(pd + 2);
    prev[a][val] = at;
    prev[a][next[a][val]] = val;
    prev[a][val^1] = at^1;
    prev[a][next[a][val^1]] = val^1;
    }

  void prev_insert(int a, int at, int val) {
    next_insert(a, prev[a][at], val);
    }
  
  int errors;
  
  crystal_structure() { errors = 0; }
  
  bool may_next_insert(int a, int at, int val) {
    if(isize(next[a]) != dir) {
      next_insert(a, at, val);
      return true;
      }
    else if(next[a][at] != val) errors++;
    return false;
    }
  
  bool may_prev_insert(int a, int at, int val) {
    if(isize(prev[a]) != dir) {
      prev_insert(a, at, val);
      return true;
      }
    else if(prev[a][at] != val) errors++;
    return false;
    }
  
  void add_dimension_to(crystal_structure& poor) {
    dir = poor.dir + 2;
    dim = poor.dim + 1;

    printf("Building dimension %d\n", dim);

    next.resize(1<<dim);
    prev.resize(1<<dim);
    int mask = (1<<poor.dim) - 1;
    
    int mm = tocode(poor.dir);

    for(int i=0; i<(1<<dim); i++) {
      if(i < mm)
        next[i] = poor.next[i&mask], prev[i] = poor.prev[i&mask];
      else
        next[i] = poor.prev[i&mask], prev[i] = poor.next[i&mask];
      }

    next_insert(0, 0, poor.dir);
    
    for(int s=2; s<1<<(dim-2); s+=2) {
      if(next[s][0] < 4)
        prev_insert(s, 0, poor.dir);
      else
        next_insert(s, 0, poor.dir);
      }
    // printf("next[%d][%d] = %d\n", 4, 2, next[4][2]);
    
    for(int s=0; s<8; s++) for(int a=0; a<(1<<dim); a++) if(isize(next[a]) > poor.dir) {
      int which = next[a][poor.dir];
      int a1 = a ^ tocode(which);
      
      may_next_insert(a1, which^1, poor.dir);
      may_next_insert(a ^ mm, which, poor.dir^1);
      which = prev[a][poor.dir];
      a1 = a ^ tocode(which);
      may_prev_insert(a1, which^1, poor.dir);
      }
      
    // println(hlog, next);
    
    if(errors) { printf("errors: %d\n", errors); exit(1);; }
    
    int unf = 0;
    for(int a=0; a<(1<<dim); a++) if(isize(next[a]) == poor.dir) {
      if(!unf) printf("unf: ");
      printf("%d ", a);
      unf ++;
      }
    
    if(unf) { printf("\n"); exit(2); }

    for(int a=0; a<(1<<dim); a++) for(int b=0; b<dir; b++)
      if(prev[a][next[a][b]] != b) {
        println(hlog, next[a], prev[a]);
        printf("next/prev %d\n", a);
        exit(3);
        }
      
    if(count_bugs()) {
      printf("bugs reported: %d\n", count_bugs());
      exit(4);
      }
    }
  
  void remove_half_dimension() {
    dir--;
    for(int i=0; i<(1<<dim); i++) {
      int take_what = dir-1;
      if(i >= (1<<(dim-1))) take_what = dir;
      next[i][prev[i][take_what]] = next[i][take_what],
      prev[i][next[i][take_what]] = prev[i][take_what],
      next[i].resize(dir),
      prev[i].resize(dir);
      }
    }

  void build() {
    dir = 4;
    dim = 2;
    next.resize(4, {2,3,1,0});
    next_to_prev();
    while(dir < S7) {
      crystal_structure csx = std::move(*this);
      add_dimension_to(csx);
      }
    if(dir > S7) remove_half_dimension();
      
    next_to_coord();
    
    coord_to_order();
    coord_to_next();
    if(count_bugs()) {
      printf("bugs found\n");
      }
    if(dir > MAX_EDGE_CRYSTAL || dim > MAXDIM) {
      printf("Dimension or directions exceeded -- I have generated it, but won't play");
      exit(0);
      }
    }
  
  };

struct lwalker {
  crystal_structure& cs;
  int id;
  int spin;
  lwalker(crystal_structure& cs) : cs(cs) {}
  void operator = (const lwalker& x) { id = x.id; spin = x.spin; }
  constexpr lwalker(const lwalker& l) : cs(l.cs), id(l.id), spin(l.spin) {}
  };

lwalker operator +(lwalker a, int v) { a.spin = gmod(a.spin + v, a.cs.dir); return a; }

lwalker operator +(lwalker a, wstep_t) {
  a.spin = a.cs.cmap[a.id][a.spin];
  a.id ^= tocode(a.spin);
  a.spin = a.cs.order[a.id][a.spin^1];
  return a;
  }

coord add(coord c, lwalker a, int val) {
  int code = a.cs.cmap[a.id][a.spin];
  c[code>>1] += ((code&1) ? val : -val);
  return c;
  }
  
coord add(coord c, int cname, int val) {
  int dim = (cname>>1);
  c[dim] = (c[dim] + (cname&1?val:-val));
  return c;
  }

ld sqhypot2(crystal_structure& cs, ldcoord co1, ldcoord co2) {
  int result = 0;
  for(int a=0; a<cs.dim; a++) result += (co1[a] - co2[a]) * (co1[a] - co2[a]);
  return result;
  }

static const int Modval = 64;

struct east_structure {
  map<coord, int> data;
  int Xmod, cycle;
  int zeroshift;
  int coordid;
  };

int fiftyrule(coord c) {
  int res[256] = {
     1,-1,32,-1,-1, 2,-1,35,32,-1, 1,-1,-1,35,-1, 2,
    -1,-1,-1,-1, 4,-1,36,-1,-1,-1,-1,-1,36,-1, 4,-1,
    32,-1, 1,-1,-1,34,-1, 3, 1,-1,32,-1,-1, 3,-1,34,
    -1,-1,-1,-1,36,-1, 4,-1,-1,-1,-1,-1, 4,-1,36,-1,
    -1, 4,-1,36,-1,-1,-1,-1,-1,36,-1, 4,-1,-1,-1,-1,
     3,-1,35,-1,-1,-1,-1,-1,35,-1, 3,-1,-1,-1,-1,-1,
    -1,36,-1, 4,-1,-1,-1,-1,-1, 4,-1,36,-1,-1,-1,-1,
    34,-1, 2,-1,-1,-1,-1,-1, 2,-1,34,-1,-1,-1,-1,-1,
    32,-1, 1,-1,-1,34,-1, 3, 1,-1,32,-1,-1, 3,-1,34,
    -1,-1,-1,-1,36,-1, 4,-1,-1,-1,-1,-1, 4,-1,36,-1,
     1,-1,32,-1,-1, 2,-1,35,32,-1, 1,-1,-1,35,-1, 2,
    -1,-1,-1,-1, 4,-1,36,-1,-1,-1,-1,-1,36,-1, 4,-1,
    -1,36,-1, 4,-1,-1,-1,-1,-1, 4,-1,36,-1,-1,-1,-1,
    34,-1, 2,-1,-1,-1,-1,-1, 2,-1,34,-1,-1,-1,-1,-1,
    -1, 4,-1,36,-1,-1,-1,-1,-1,36,-1, 4,-1,-1,-1,-1,
     3,-1,35,-1,-1,-1,-1,-1,35,-1, 3,-1,-1,-1,-1,-1,
     };
    
  int index = 0;
  for(int i=0; i<4; i++) index += (c[i] & 3) << (2 * i);
  
  if(res[index] == -1) exit(1);
  
  return res[index];
  }

bool is_bi(crystal_structure& cs, coord co);

#if MAXMDIM >= 4
typedef array<coord, 12> shifttable;

int ctable[64][6] = {
   {0, 1, 2, 3, 4, 5, },
   {6, 1, 5, 4, 3, 2, },
   {0, 7, 5, 4, 3, 2, },
   {6, 7, 2, 3, 4, 5, },
   {0, 1, 5, 4, 3, 8, },
   {6, 1, 8, 3, 4, 5, },
   {0, 7, 8, 3, 4, 5, },
   {6, 7, 5, 4, 3, 8, },
   {0, 1, 5, 4, 9, 2, },
   {6, 1, 2, 9, 4, 5, },
   {0, 7, 2, 9, 4, 5, },
   {6, 7, 5, 4, 9, 2, },
   {0, 1, 8, 9, 4, 5, },
   {6, 1, 5, 4, 9, 8, },
   {0, 7, 5, 4, 9, 8, },
   {6, 7, 8, 9, 4, 5, },
   {0, 1, 5, 10, 3, 2, },
   {6, 1, 2, 3, 10, 5, },
   {0, 7, 2, 3, 10, 5, },
   {6, 7, 5, 10, 3, 2, },
   {0, 1, 8, 3, 10, 5, },
   {6, 1, 5, 10, 3, 8, },
   {0, 7, 5, 10, 3, 8, },
   {6, 7, 8, 3, 10, 5, },
   {0, 1, 2, 9, 10, 5, },
   {6, 1, 5, 10, 9, 2, },
   {0, 7, 5, 10, 9, 2, },
   {6, 7, 2, 9, 10, 5, },
   {0, 1, 5, 10, 9, 8, },
   {6, 1, 8, 9, 10, 5, },
   {0, 7, 8, 9, 10, 5, },
   {6, 7, 5, 10, 9, 8, },
   {0, 1, 11, 4, 3, 2, },
   {6, 1, 2, 3, 4, 11, },
   {0, 7, 2, 3, 4, 11, },
   {6, 7, 11, 4, 3, 2, },
   {0, 1, 8, 3, 4, 11, },
   {6, 1, 11, 4, 3, 8, },
   {0, 7, 11, 4, 3, 8, },
   {6, 7, 8, 3, 4, 11, },
   {0, 1, 2, 9, 4, 11, },
   {6, 1, 11, 4, 9, 2, },
   {0, 7, 11, 4, 9, 2, },
   {6, 7, 2, 9, 4, 11, },
   {0, 1, 11, 4, 9, 8, },
   {6, 1, 8, 9, 4, 11, },
   {0, 7, 8, 9, 4, 11, },
   {6, 7, 11, 4, 9, 8, },
   {0, 1, 2, 3, 10, 11, },
   {6, 1, 11, 10, 3, 2, },
   {0, 7, 11, 10, 3, 2, },
   {6, 7, 2, 3, 10, 11, },
   {0, 1, 11, 10, 3, 8, },
   {6, 1, 8, 3, 10, 11, },
   {0, 7, 8, 3, 10, 11, },
   {6, 7, 11, 10, 3, 8, },
   {0, 1, 11, 10, 9, 2, },
   {6, 1, 2, 9, 10, 11, },
   {0, 7, 2, 9, 10, 11, },
   {6, 7, 11, 10, 9, 2, },
   {0, 1, 8, 9, 10, 11, },
   {6, 1, 11, 10, 9, 8, },
   {0, 7, 11, 10, 9, 8, },
   {6, 7, 8, 9, 10, 11, },
   };

shifttable get_canonical(coord co) {
  shifttable res;
  if(S7 == 12) {
    int eid = 0;
    for(int a=0; a<6; a++) if(co[a] & 2) eid += (1<<a);
    for(int i=0; i<12; i++) res[i] = c0;
    for(int i=0; i<6; i++) {
      int c = ctable[eid][i];
      res[i][c % 6] = (c>=6) ? -2 : 2;
      res[6+i][c % 6] = (c>=6) ? 2 : -2;
      }
    }
  else {
    for(int i=0; i<4; i++) {
      res[i] = c0;
      res[i][i] = 2;
      res[i+4] = c0;
      res[i+4][i] = -2;
      }
    for(int a=0; a<4; a++) if(co[a] & 2) swap(res[a], res[a+4]);
    int bts = 0;
    for(int a=0; a<4; a++) if(co[a] & 2) bts++;
    if(bts & 1) swap(res[2], res[3]), swap(res[6], res[7]);
    }
  return res;
  }
#endif

EX int crystal_period = 0;

struct hrmap_crystal : hrmap_standard {
  heptagon *getOrigin() override { return get_heptagon_at(c0, S7); }

  map<heptagon*, coord> hcoords;
  map<coord, heptagon*> heptagon_at;
  map<int, eLand> landmemo;
  map<coord, eLand> landmemo4;
  map<cell*, map<cell*, int>> distmemo;
  map<cell*, ldcoord> sgc;
  cell *camelot_center;
  ldcoord camelot_coord;
  ld camelot_mul;
  
  crystal_structure cs;
  east_structure east;   

  lwalker makewalker(coord c, int d) {
    lwalker a(cs);
    a.id = 0;
    for(int i=0; i<cs.dim; i++) if(c[i] & FULLSTEP) a.id += (1<<i);
    a.spin = d;
    return a;
    }
  
  bool crystal3() { return WDIM == 3; }
  
  hrmap_crystal() {
#if MAXMDIM >= 4
    if(crystal3()) reg3::generate(), cs.dim = S7 / 2; else 
#endif
    cs.build();
    
    camelot_center = NULL;
    }

  ~hrmap_crystal() {
    clearfrom(getOrigin());
    }
  
  heptagon *get_heptagon_at(coord c, int deg) {
    if(heptagon_at.count(c)) return heptagon_at[c];
    heptagon*& h = heptagon_at[c];
    h = init_heptagon(deg);
    h->c7 = newCell(deg, h);
    
    /* in {6,4} we need emeraldval for some patterns, including (bitruncated) football and (bitruncated) three-color */
    h->emeraldval = (c[0] ^ c[1] ^ c[2]) & 2;    
    h->emeraldval ^= (c[1] & 4);
    h->emeraldval ^= (c[0] & 4);
    h->emeraldval ^= (c[2] & 4);
    h->emeraldval ^= ((c[2] & 2) << 1);    
    if(c[0] & 2) h->emeraldval ^= 1;

    if(ginf[gCrystal].vertex == 3) 
      h->fiftyval = fiftyrule(c);    
    for(int i=0; i<cs.dim; i++) h->distance += abs(c[i]);
    h->distance /= 2;
    hcoords[h] = c;
    // for(int i=0; i<6; i++) crystalstep(h, i);
    return h;
    }
  
  ldcoord get_coord(cell *c) {
    // in C++14?
    // auto b = sgc.emplace(c, ldc0);
    // ldcoord& res = b.first->second;
    if(sgc.count(c)) return sgc[c];
    ldcoord& res = (sgc[c] = ldc0);
    { // if(b.second) {
      if(BITRUNCATED && c->master->c7 != c) {
        for(int i=0; i<c->type; i+=2)
          res = res + told(hcoords[c->cmove(i)->master]);
        res = res * 2 / c->type;
        }
      else if(GOLDBERG && c->master->c7 != c) {
        auto m = gp::get_masters(c);
        auto H = gp::get_master_coordinates(c);
        for(int i=0; i<cs.dim; i++)
          res = res + told(hcoords[m[i]]) * H[i];
        }
      else
        res = told(hcoords[c->master]);
      }
    return res;
    }
  
  coord long_representant(cell *c);  

  int get_east(cell *c);

  void build_east(int cid);
  
  void verify() override { }
  
  void prepare_east();
  
  void apply_period(coord& co) {
    for(int a=0; a<cs.dim; a++)
      co[a] = szgmod(co[a], 2*crystal_period);
    }

  heptagon *create_step(heptagon *h, int d) override {
    if(!hcoords.count(h)) {
      printf("not found\n");
      return NULL;
      }
    auto co = hcoords[h];
    
    #if MAXMDIM >= 4
    if(crystal3()) {
      auto st = get_canonical(co);
      auto co1 = co + st[d];
      apply_period(co1);
      auto h1 = get_heptagon_at(co1, S7);
      auto st1 = get_canonical(co1);
    
      for(int d1=0; d1<S7; d1++) if(st1[d1] == st[d])
        h->c.connect(d, h1, (d1+S7/2) % S7, false);
      
      return h1;
      }
    #endif
    
    if(is_bi(cs, co)) {
      heptspin hs(h, d);
      (hs + 1 + wstep + 1).cpeek();
      return h->move(d);
      }
  
    auto lw = makewalker(co, d);
  
    if(ginf[gCrystal].vertex == 4) {
      auto c1 = add(co, lw, FULLSTEP);
      auto lw1 = lw+wstep;
      apply_period(c1);
      
      h->c.connect(d, heptspin(get_heptagon_at(c1, S7), lw1.spin));
      }
    else {
      auto coc = add(add(co, lw, HALFSTEP), lw+1, HALFSTEP);
      auto hc = get_heptagon_at(coc, 8);
      apply_period(coc);
      for(int a=0; a<8; a+=2) {
        hc->c.connect(a, heptspin(h, lw.spin));
        if(h->modmove(lw.spin-1)) {
          hc->c.connect(a+1, heptspin(h, lw.spin) - 1 + wstep - 1);
          }
        co = add(co, lw, FULLSTEP);
        apply_period(co);
        lw = lw + wstep + (-1);
        h = get_heptagon_at(co, S7);
        }
      }
    return h->move(d);
    }

  #if MAXMDIM >= 4
  map<int, transmatrix> adjs;
  
  transmatrix adj(heptagon *h, int d) override {
    if(!crystal3()) return hrmap_standard::adj(h, d);
    auto co = hcoords[h];
    int id = 0;
    for(int a=0; a<S7/2; a++) id = (2*id) + ((co[a]>>1) & 1);
    id = S7*id + d;
    if(adjs.count(id)) return adjs[id];
    transmatrix T = cgi.adjmoves[d];
    reg3::generate_cellrotations();
    auto st = get_canonical(co);
    auto co1 = co + st[d];
    auto st1 = get_canonical(co1);
    int qty = 0;
    transmatrix res;
    ld gdist = S7 == 12 ? hdist0(tC0(cgi.adjmoves[0])) : cgi.strafedist;

    h->cmove(d);

    for(auto& cr: cgi.cellrotations) {

      transmatrix U = T * cr.M;
      
      ld go = hdist0(U * tC0(cgi.adjmoves[h->c.spin(d)]));
      if(go > 1e-2) continue;

      for(int s=0; s<S7; s++) 
        if(cgi.heptshape->dirdist[d][s] == 1)
          for(int t=0; t<S7; t++) 
            if(st1[t] == st[s]) {
              if(hdist(U * tC0(cgi.adjmoves[t]), tC0(cgi.adjmoves[s])) > gdist + .1)
                goto wrong;
              }
      res = U;
      qty++;
      wrong: ;
      }
    adjs[id] = res;
    if(qty == 1) return res;
    println(hlog, "qty = ", qty);
    exit(1);
    }

  transmatrix adj(cell *c, int d) override { 
    if(crystal3()) return adj(c->master, d);
    return hrmap_standard::adj(c, d); 
    }

  void draw_at(cell *at, const shiftmatrix& where) override {
    if(!crystal3()) { hrmap_standard::draw_at(at, where); return; }
    else hrmap::draw_at(at, where);
    }

  transmatrix relative_matrixc(cell *h2, cell *h1, const hyperpoint& hint) override { 
    if(!crystal3()) return hrmap_standard::relative_matrixc(h2, h1, hint);
    if(h2 == h1) return Id;
    for(int i=0; i<S7; i++) if(h2 == h1->move(i)) return adj(h1->master, i);
    if(gmatrix0.count(h2) && gmatrix0.count(h1))
      return inverse_shift(gmatrix0[h1], gmatrix0[h2]);
    println(hlog, "unknown relmatrix, distance = ", celldistance(h1, h2));
    return xpush(999);
    }

  transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override { 
    if(!crystal3()) return hrmap_standard::relative_matrixh(h2, h1, hint);
    return relative_matrixc(h2->c7, h1->c7, hint);
    }
  #endif
  };

hrmap_crystal *crystal_map() {
  return (hrmap_crystal*) currentmap;
  } 

EX heptagon *get_heptagon_at(coord c) { return crystal_map()->get_heptagon_at(c, S7); }
EX coord get_coord(heptagon *h) { return crystal_map()->hcoords[h]; }
EX ldcoord get_ldcoord(cell *c) { return crystal_map()->get_coord(c); }

EX int get_dim() { return crystal_map()->cs.dim; }

#if MAXMDIM >= 4
EX transmatrix get_adj(heptagon *h, int d) { return crystal_map()->adj(h, d); }
#endif

bool is_bi(crystal_structure& cs, coord co) {
  for(int i=0; i<cs.dim; i++) if(co[i] & HALFSTEP) return true;
  return false;
  }

array<array<int,2>, MAX_EDGE_CRYSTAL> distlimit_table = {{
  {{SEE_ALL,SEE_ALL}}, {{SEE_ALL,SEE_ALL}}, {{SEE_ALL,SEE_ALL}}, {{SEE_ALL,SEE_ALL}}, {{15, 10}}, 
  {{6, 4}}, {{5, 3}}, {{4, 3}}, {{4, 3}}, {{3, 2}}, {{3, 2}}, {{3, 2}}, {{3, 2}}, {{3, 2}}
  }};

EX color_t colorize(cell *c, char whichCanvas) {
  auto m = crystal_map();
  ldcoord co = ldc0;
  int dim = 3;
  if(cryst) co = m->get_coord(c), dim = m->cs.dim;
  #if MAXMDIM >= 4
  else if(geometry == gSpace344) {
    co = told(reg3::decode_coord(reg3::minimize_quotient_maps ? 1 : 2, c->master->fieldval)), dim = 4;
    for(int a=0; a<4; a++) if(co[a] > 4) co[a] -= 8;
    }
  else if(geometry == gSeifertCover) {
    int i = c->master->fieldval;
    for(int a=0; a<3; a++) co[a] = i%5, i /= 5;
    }
  #endif
  else if(euc::in()) {
    auto tab = euc::get_ispacemap()[c->master];
    for(int a=0; a<3; a++) co[a] = tab[a];
    if(PURE) for(int a=0; a<3; a++) co[a] *= 2;
    dim = 3;
    }

  color_t res = 0;
  coord ico = roundcoord(co);

  int ones = 0;
  for(int i=0; i<dim; i++) if((ico[i] & 2) == 2) ones++;

  switch(whichCanvas) {
    case 'K': 
      for(int i=0; i<3; i++)
        res |= ((int)(((i == 2 && S7 == 5) ? (128 + co[i] * 50) : (255&int(128 + co[i] * 25))))) << (8*i);
      return res;

    case '@': {
      if(ico[dim-1] == 2 && (ones & 1)) return 0x1C0FFC0;
      if(ico[dim-1] == 2 && !(ones & 1)) return 0x180FF80;
      if(ico[dim-1] == -4 && (ones & 1)) return 0x180C0FF;
      if(ico[dim-1] == -4 && !(ones & 1)) return 0x14080FF;
      return 0x101010;
      }
    
    case '=':
      if(ico[dim-1] == 2 && (ones & 1)) return 0x1C0FFC0;
      if(ico[dim-1] == 2 && !(ones & 1)) return 0x180FF80;
      if(ico[dim-1] == -2 && (ones & 1)) return 0x180C0FF;
      if(ico[dim-1] == -2 && !(ones & 1)) return 0x14080FF;

      return 0x101010;
    
    case '#': {
      bool grid = false;
      ico[dim-1] -= 2;
      for(int d=dim; d<MAXDIM; d++) ico[d] = 0;
      for(int i=0; i<dim; i++) if((ico[i] & 6) == 4) grid = true;
    
      for(int i=0; i<3; i++) part(res, i) = 0xFF + 0x18 * (ico[i]/2-2);
      if(grid) res |= 0x1000000;
      else if(GDIM == 2) res = (res & 0xFEFEFE) >> 1;
      if(ico == c0) res = 0x1FFD500;
      return res;
      }
    
    case 'O': {
      for(int i=0; i<3; i++) part(res, i) = 0xFF + 0x18 * (ico[i]/2-2);
      c->landparam = res;
      if(ones == dim-1) res |= 0x1000000;      
      else if(GDIM == 2) res = (res & 0xFEFEFE) >> 1;
      return res;
      }
      
    case '/': {
      int s = 0;
      for(int a=0; a<dim; a++) s += ico[a];
      if(s > 0) return 0x1FF20FF;
      else if (s < -2) return 0x1C0C0C0;
      }
    }
  return res;
  }

EX colortable coordcolors = {0xD04040, 0x40D040, 0x4040D0, 0xFFD500, 0xF000F0, 0x00F0F0, 0xF0F0F0 };

EX ld compass_angle() {
  bool bitr = ginf[gCrystal].vertex == 3;
  return (bitr ? 22.5_deg : 0) - master_to_c7_angle();
  }
      
EX bool crystal_cell(cell *c, shiftmatrix V) {

  if(!cryst) return false;

  if(view_east && cheater) {
    int d = dist_alt(c);
    queuestr(V, 0.3, its(d), 0xFFFFFF, 1);
    }

  if(view_coordinates && WDIM == 2 && (cheater || tour::on)) {
    
    auto m = crystal_map();
    
    if(c->master->c7 == c && !is_bi(m->cs, m->hcoords[c->master])) {
    
      ld dist = cellgfxdist(c, 0);

      for(int i=0; i<S7; i++)  {
        shiftmatrix T = V * spin(compass_angle() - TAU * i / S7) * xpush(dist*.3);
        
        auto co = m->hcoords[c->master];
        auto lw = m->makewalker(co, i);
        int cx = m->cs.cmap[lw.id][i];
        
        queuestr(T, 0.3, its(co[cx>>1] / FULLSTEP), coordcolors[cx>>1], 1);
        }
      }                                 
    }
  return false;
  }

EX vector<cell*> build_shortest_path(cell *c1, cell *c2) {
  auto m = crystal_map();
  ldcoord co1 = m->get_coord(c1);
  ldcoord co2 = m->get_coord(c2) - co1;
  
  // draw a cylinder from co1 to co2, and find the solution by going through that cylinder
  
  ldcoord mul = co2 / sqrt(co2|co2);
  
  ld mmax = (co2|mul);
  
  vector<cell*> p;  
  vector<int> parent_id;
  
  manual_celllister cl;
  cl.add(c2);
  parent_id.push_back(-1);
  
  int steps = 0;
  int nextsteps = 1;
  
  for(int i=0; i<isize(cl.lst); i++) {
    if(i == nextsteps) steps++, nextsteps = isize(cl.lst);
    cell *c = cl.lst[i];
    forCellCM(c3, c) if(!cl.listed(c3)) {
      if(c3 == c1) { 
        p.push_back(c1);
        while(c3 != c2) {
          while(i) {
            p.push_back(c3);
            i = parent_id[i];
            c3 = cl.lst[i];
            }
          }
        p.push_back(c3);
        return p;
        }

      auto h = m->get_coord(c3) - co1;
      ld dot = (h|mul);
      if(dot > mmax + PERIOD/2 + .1) continue;

      for(int k=0; k<m->cs.dim; k++) if(abs(h[k] - dot * mul[k]) > PERIOD + .1) goto next3;
      cl.add(c3);
      parent_id.push_back(i);
      next3: ;
      }
    }
  
  println(hlog, "Error: path not found");
  return p;
  }

EX int precise_distance(cell *c1, cell *c2) {
  if(c1 == c2) return 0;
  auto m = crystal_map();
  if(pure()) {
    coord co1 = m->hcoords[c1->master];
    coord co2 = m->hcoords[c2->master];
    int result = 0;
    for(int a=0; a<m->cs.dim; a++) result += abs(co1[a] - co2[a]);
    return result / FULLSTEP;
    }
  
  auto& distmemo = m->distmemo;
  
  if(c2 == currentmap->gamestart()) swap(c1, c2);
  else if(isize(distmemo[c2]) > isize(distmemo[c1])) swap(c1, c2);

  if(distmemo[c1].count(c2)) return distmemo[c1][c2];
  
  int zmin = 999999, zmax = -99;
  forCellEx(c3, c2) if(distmemo[c1].count(c3)) {
     int d = distmemo[c1][c3];
     if(d < zmin) zmin = d;
     if(d > zmax) zmax = d;
     }
  if(zmin+1 < zmax-1) println(hlog, "zmin < zmax");
  if(zmin+1 == zmax-1) return distmemo[c1][c2] = zmin+1;

  ldcoord co1 = m->get_coord(c1);
  ldcoord co2 = m->get_coord(c2) - co1;
  
  // draw a cylinder from co1 to co2, and find the solution by going through that cylinder
  
  ldcoord mul = co2 / sqrt(co2|co2);
  
  ld mmax = (co2|mul);
  
  manual_celllister cl;
  cl.add(c2);
  
  int steps = 0;
  int nextsteps = 1;
  
  for(int i=0; i<isize(cl.lst); i++) {
    if(i == nextsteps) steps++, nextsteps = isize(cl.lst);
    cell *c = cl.lst[i];
    forCellCM(c3, c) if(!cl.listed(c3)) {
      if(c3 == c1) { 
        return distmemo[c1][c2] = distmemo[c2][c1] = 1 + steps;
        }

      auto h = m->get_coord(c3) - co1;
      ld dot = (h|mul);
      if(dot > mmax + PERIOD/2 + .1) continue;

      for(int k=0; k<m->cs.dim; k++) if(abs(h[k] - dot * mul[k]) > PERIOD + .1) goto next3;
      cl.add(c3);
      next3: ;
      }
    }
  
  println(hlog, "Error: distance not found");
  return 999999;
  }

EX ld space_distance(cell *c1, cell *c2) {
  auto m = crystal_map();
  ldcoord co1 = m->get_coord(c1);
  ldcoord co2 = m->get_coord(c2);
  return sqrt(sqhypot2(m->cs, co1, co2));
  }

EX ld space_distance_camelot(cell *c) {
  auto m = crystal_map();
  return m->camelot_mul * sqrt(sqhypot2(m->cs, m->get_coord(c), m->camelot_coord));
  }

EX int dist_relative(cell *c) {
  auto m = crystal_map();
  auto& cc = m->camelot_center;
  int r = roundTableRadius(NULL);
  cell *start = m->gamestart();
  if(!cc) {
    println(hlog, "Finding Camelot center...");
    cc = start;
    while(precise_distance(cc, start) < r + 5)
      cc = cc->cmove(hrand(cc->type));
      
    m->camelot_coord = m->get_coord(m->camelot_center);
    if(m->cs.dir % 2)
      m->camelot_coord[m->cs.dim-1] = 1;
    
    m->camelot_mul = 1;
    m->camelot_mul *= (r+5) / space_distance_camelot(start);
    }

  if(pure()) 
    return precise_distance(c, cc) - r;

  ld dis = space_distance_camelot(c);
  if(dis < r) 
    return int(dis) - r;
  else {
    forCellCM(c1, c) if(space_distance_camelot(c1) < r)
      return 0;
    return int(dis) + 1 - r;
    }
  }

coord hrmap_crystal::long_representant(cell *c) {
  auto& coordid = east.coordid;
  auto co = roundcoord(get_coord(c) * Modval/PERIOD);
  for(int s=0; s<coordid; s++) co[s] = gmod(co[s], Modval);
  for(int s=coordid+1; s<cs.dim; s++) {
    int v = gdiv(co[s], Modval);
    co[s] -= v * Modval;
    co[coordid] += v * Modval;
    }
  return co;
  }

int hrmap_crystal::get_east(cell *c) {
  auto& coordid = east.coordid;
  auto& Xmod = east.Xmod;
  auto& data = east.data;
  auto& cycle = east.cycle;

  coord co = long_representant(c);
  int cycles = gdiv(co[coordid], Xmod);
  co[coordid] -= cycles * Xmod;
  return data[co] + cycle * cycles;
  }

void hrmap_crystal::build_east(int cid) {
  auto& coordid = east.coordid;
  auto& Xmod = east.Xmod;
  auto& data = east.data;
  auto& cycle = east.cycle;
  
  coordid = cid;
  map<coord, int> full_data;
  manual_celllister cl;
  
  for(int i=0; i<(1<<cid); i++) {
    auto co = c0; 
    for(int j=0; j<cid; j++) co[j] = ((i>>j)&1) * 2;
    cell *cc = get_heptagon_at(co, cs.dir)->c7;
    cl.add(cc);
    }
  
  map<coord, int> stepat;

  int steps = 0, nextstep = isize(cl.lst);
  
  cycle = 0;
  int incycle = 0;
  int needcycle = 16 + nextstep;
  int elongcycle = 0;
  
  Xmod = Modval;
  
  int modmul = 1;
  
  for(int i=0; i<isize(cl.lst); i++) {
    if(incycle > needcycle * modmul) break;
    if(i == nextstep) steps++, nextstep = isize(cl.lst);
    cell *c = cl.lst[i];

    auto co = long_representant(c);
    if(co[coordid] < -Modval) continue;
    if(full_data.count(co)) continue;
    full_data[co] = steps;
    
    auto co1 = co; co1[coordid] -= Xmod;
    auto co2 = co; co2[coordid] = gmod(co2[coordid], Xmod);

    if(full_data.count(co1)) {
      int ncycle = steps - full_data[co1];
      if(ncycle != cycle) incycle = 1, cycle = ncycle;
      else incycle++;
      int dd = gdiv(co[coordid], Xmod);
      // println(hlog, co, " set data at ", co2, " from ", data[co2], " to ", steps - dd * cycle, " at step ", steps);
      data[co2] = steps - dd * cycle;
      elongcycle++;
      if(elongcycle > 2 * needcycle * modmul) Xmod += Modval, elongcycle = 0, modmul++;
      }
    else incycle = 0, needcycle++, elongcycle = 0;
    forCellCM(c1, c) cl.add(c1);
    }
  
  east.zeroshift = 0;
  east.zeroshift = -get_east(cl.lst[0]);

  println(hlog, "cycle found: ", cycle, " Xmod = ", Xmod, " on list: ", isize(cl.lst), " zeroshift: ", east.zeroshift);
  }
  
void hrmap_crystal::prepare_east() {
  if(east.data.empty()) build_east(1);
  }

EX int dist_alt(cell *c) {
  auto m = crystal_map();
  if(specialland == laCamelot && m->camelot_center) {
    if(pure()) 
      return precise_distance(c, m->camelot_center);
    if(c == m->camelot_center) return 0;
    return 1 + int(2 * space_distance_camelot(c));
    }
  else {
    m->prepare_east();
    return m->get_east(c);
    }
  }

array<array<ld, MAXDIM>, MAXDIM> crug_rotation;

int ho = 1;

ldcoord rug_center;
bool draw_cut = false;
ld cut_level = 0;

EX void init_rotation() {
  for(int i=0; i<MAXDIM; i++)
  for(int j=0; j<MAXDIM; j++)
    crug_rotation[i][j] = i == j ? 1/2. : 0;
  
  auto& cs = crystal_map()->cs;

  if(ho & 1) {
    for(int i=(draw_cut ? 2 : cs.dim-1); i>=1; i--) {
      ld alpha = 90._deg / (i+1);
      ld c = cos(alpha);
      ld s = sin(alpha);
      for(int j=0; j<cs.dim; j++)
        tie(crug_rotation[j][0], crug_rotation[j][i]) =
          make_pair(
             crug_rotation[j][0] * s + crug_rotation[j][i] * c,
            -crug_rotation[j][i] * s + crug_rotation[j][0] * c
            );
      }
    }
  }

EX void random_rotation() {
  auto& cs = crystal_map()->cs;
  for(int i=0; i<100; i++) {
    int a = hrand(cs.dim);
    int b = hrand(cs.dim);
    if(a == b) continue;
    ld alpha = hrand(1000);
    ld c = cos(alpha);
    ld s = sin(alpha);
    for(int u=0; u<cs.dim; u++) {
      auto& x = crug_rotation[u][a];
      auto& y = crug_rotation[u][b];
      tie(x,y) = make_pair(x * c + y * s, y * c - x * s);
      }
    }
  }


EX void next_home_orientation() {
  ho++;
  init_rotation();
  }

EX void flip_z() {
  for(int i=0; i<MAXDIM; i++)
    crug_rotation[i][2] *= -1;
  }

#if CAP_RUG
#if MAXMDIM >= 4
hyperpoint coord_to_flat(ldcoord co, int dim = 3) {
  auto& cs = crystal_map()->cs;
  hyperpoint res = Hypc;
  co = co - rug_center;
  for(int a=0; a<cs.dim; a++)
    for(int b=0; b<dim; b++)
      res[b] += crug_rotation[b][a] * co[a] * rug::modelscale;
  return res;
  }
#endif

EX void switch_z_coordinate() {
  auto& cs = crystal_map()->cs;
  for(int i=0; i<cs.dim; i++) {
    ld tmp = crug_rotation[i][2];
    for(int u=2; u<cs.dim-1; u++) crug_rotation[i][u] = crug_rotation[i][u+1];
    crug_rotation[i][cs.dim-1] = tmp;
    }
  }

EX void apply_rotation(const transmatrix t) {
  auto& cs = crystal_map()->cs;
  for(int i=0; i<cs.dim; i++) {
    hyperpoint h;
    for(int j=0; j<3; j++) h[j] = crug_rotation[i][j];
    h = t * h;
    for(int j=0; j<3; j++) crug_rotation[i][j] = h[j];
    }
  }

EX void centerrug(ld aspd) {
  if(vid.sspeed >= 4.99) aspd = 1000;
  
  auto m = crystal_map();
  ldcoord at = m->get_coord(cwt.at) - rug_center;
  
  ld R = sqrt(at|at);
  
  if(R < 1e-9) rug_center = m->get_coord(cwt.at);  
  else {
    aspd *= (2+3*R*R);
    if(aspd > R) aspd = R;  
    rug_center = rug_center + at * aspd / R;
    }
  }
  
void cut_triangle2(const hyperpoint pa, const hyperpoint pb, const hyperpoint pc, const hyperpoint ha, const hyperpoint hb, const hyperpoint hc) {
  ld zac = pc[3] / (pc[3] - pa[3]);
  hyperpoint pac = pa * zac + pc * (1-zac);
  hyperpoint hac = ha * zac + hc * (1-zac);

  ld zbc = pc[3] / (pc[3] - pb[3]);
  hyperpoint pbc = pb * zbc + pc * (1-zbc);
  hyperpoint hbc = hb * zbc + hc * (1-zbc);
  
  pac[3] = pbc[3] = 1;
  
  rug::rugpoint *rac = rug::addRugpoint(shiftless(hac), 0);
  rug::rugpoint *rbc = rug::addRugpoint(shiftless(hbc), 0);
  rac->native = pac;
  rbc->native = pbc;
  rac->valid = true;
  rbc->valid = true;
  rug::triangles.push_back(rug::triangle(rac, rbc, NULL));
  }

void cut_triangle(const hyperpoint pa, const hyperpoint pb, const hyperpoint pc, const hyperpoint ha, const hyperpoint hb, const hyperpoint hc) {
  if((pa[3] >= 0) == (pb[3] >= 0))
    cut_triangle2(pa, pb, pc, ha, hb, hc);
  else if((pa[3] >= 0) == (pc[3] >= 0))
    cut_triangle2(pc, pa, pb, hc, ha, hb);
  else
    cut_triangle2(pb, pc, pa, hb, hc, ha);
  }

#if MAXMDIM >= 4
EX void build_rugdata() {
  using namespace rug;
  rug::clear_model(); 
  rug::good_shape = true;  
  rug::vertex_limit = 0;
  auto m = crystal_map();
  
  for(const auto& gp: gmatrix) {
            
    cell *c = gp.first;
    if(c->wall == waInvisibleFloor) continue;
    const shiftmatrix& V = gp.second;

    auto co = m->get_coord(c);
    vector<ldcoord> vcoord(c->type);

    for(int i=0; i<c->type; i++) 
      if(valence() == 4)
        vcoord[i] = ((m->get_coord(c->cmove(i)) + m->get_coord(c->cmodmove(i-1))) / 2);
      else
        vcoord[i] = ((m->get_coord(c->cmove(i)) + m->get_coord(c->cmodmove(i-1)) + co) / 3);
    
    if(!draw_cut) {
      rugpoint *v = addRugpoint(tC0(V), 0);
      v->native = coord_to_flat(co);
      v->valid = true;
      
      rugpoint *p[MAX_EDGE_CRYSTAL];
      
      for(int i=0; i<c->type; i++) {
        p[i] = addRugpoint(V * get_corner_position(c, i), 0);
        p[i]->valid = true;
        p[i]->native = coord_to_flat(vcoord[i]);
        }
  
      for(int i=0; i<c->type; i++) addTriangle(v, p[i], p[(i+1) % c->type]);
      }
    
    else {
      hyperpoint hco = coord_to_flat(co, 4);
      hco[3] -= cut_level * rug::modelscale;
      vector<hyperpoint> vco(c->type);
      for(int i=0; i<c->type; i++) {
        vco[i] = coord_to_flat(vcoord[i], 4);
        vco[i][3] -= cut_level * rug::modelscale;
        }
      
      for(int i=0; i<c->type; i++) {
        int j = (i+1) % c->type;
        if((vco[i][3] >= 0) != (hco[3] >= 0) || (vco[j][3] >= 0) != (hco[3] >= 0)) {
          cut_triangle(hco, vco[i], vco[j], unshift(tC0(V)), unshift(V * get_corner_position(c, i)), unshift(V * get_corner_position(c, j)));
          }
        }
      }
    }
  
  println(hlog, "cut ", cut_level, "r ", crug_rotation);
  }
#endif
#endif

EX void set_land(cell *c) {
  setland(c, specialland); 
  auto m = crystal_map();

  auto co = m->get_coord(c);
  auto co1 = roundcoord(co * 60);
  
  coord cx = roundcoord(co / 8);
  int hash = 0;
  for(int a=0; a<m->cs.dim; a++) hash = 1317 * hash + cx[a];
  
  set_euland3(c, co1[0], co1[1], dist_alt(c), hash);
  }

EX void set_crystal(int sides) {
  stop_game();
  set_geometry(gCrystal);
  set_variation(eVariation::pure);
  ginf[gCrystal].sides = sides;
  ginf[gCrystal].vertex = 4;
  static char buf[20];
  sprintf(buf, "{%d,4}", sides);
  ginf[gCrystal].tiling_name = buf;
  ginf[gCrystal].distlimit = distlimit_table[min(sides, MAX_EDGE_CRYSTAL-1)];
  }

void test_crt() {
  start_game();
  auto m = crystal_map();
  manual_celllister cl;
  cl.add(m->camelot_center);
  for(int i=0; i<isize(cl.lst); i++)
    forCellCM(c1, cl.lst[i]) {
      setdist(c1, 7, m->gamestart());
      if(c1->land == laCamelot && c1->wall == waNone)
        cl.add(c1);
      }
  println(hlog, "actual = ", isize(cl.lst), " algorithm = ", get_table_volume());
  if(its(isize(cl.lst)) != get_table_volume()) exit(1);
  }

void unit_test_tables() {
  stop_game();
  specialland = laCamelot;
  set_crystal(5);
  test_crt();
  set_crystal(6);
  test_crt();
  set_crystal(5); set_variation(eVariation::bitruncated);
  test_crt();
  set_crystal(6); set_variation(eVariation::bitruncated);
  test_crt();
  set_crystal(8); set_variation(eVariation::bitruncated); set_variation(eVariation::bitruncated);
  test_crt();
  }

EX void set_crystal_period_flags() {
  crystal_period &= ~1;
  for(auto& g: ginf)
    if(g.flags & qCRYSTAL) {
      set_flag(ginf[gNil].flags, qSMALL, crystal_period && crystal_period <= 8);
      set_flag(ginf[gNil].flags, qCLOSED, crystal_period);
      }
  }

#if CAP_COMMANDLINE
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-crystal")) {
    PHASEFROM(2);
    shift(); set_crystal(argi());
    }
  else if(argis("-cview")) {
    view_coordinates = true;
    }
  else if(argis("-ceast")) {
    view_east = true;
    }
  else if(argis("-cprob")) {
    PHASEFROM(2); shift_arg_formula(compass_probability);
    }
  else if(argis("-ccut")) {
    draw_cut = true;
    PHASEFROM(2); shift_arg_formula(cut_level);
    }
  else if(argis("-ccutoff")) {
    draw_cut = false;
    }
  else if(argis("-cho")) {
    shift(); ho = argi();
    init_rotation();
    }
  else if(argis("-chrr")) {
    random_rotation();
    }
  else if(argis("-test:crt")) {
    test_crt();
    }
  else if(argis("-crystal_period")) {
    if(cryst) stop_game();
    shift(); crystal_period = argi();
    set_crystal_period_flags();
    }
  else if(argis("-d:crystal"))
    launch_dialog(show);
  else if(argis("-cvcol")) {
    shift(); int d = argi();
    shift(); coordcolors[d] = argcolor(24);
    }
  else return 1;
  return 0;
  }
#endif

EX hrmap *new_map() {
  return new hrmap_crystal;
  }

EX string compass_help() {
  return XLAT(
    "Lands in this geometry are usually built on North-South or West-East axis. "
    "Compasses always point North, and all the cardinal directions to the right from compass North are East (this is not "
    "true in general, but it is true for the cells where compasses are generated). "
    "North is the first coordinate, while East is the sum of other coordinates."
    );
  }

string make_help() {
  return XLAT(
    "This space essentially lets you play in a d-dimensional grid. Pick three "
    "dimensions and '3D display' to see how it works -- we are essentially playing on a periodic surface in "
    "three dimensions, made of hexagons; each hexagon connects to six other hexagons, in each of the 6 "
    "possible directions. Normally, the game visualizes this from the point of view of a creature living inside "
    "the surface (regularized and smoothened somewhat), assuming that light rays are also restricted to the surface -- "
    "this will look exactly like the {2d,4} tiling, except that the light rays may thus "
    "sometimes make a loop, causing you to see images of yourself in some directions (in other words, "
    "the d-dimensional grid is a quotient of the hyperbolic plane). The same construction works in other dimensions. "
    "Half dimensions are interpreted in the following way: the extra dimension only has two 'levels', for example 2.5D "
    "has a top plane and a bottom plane.\n\n"
    "You may also bitruncate this tessellation -- which makes it work better with the rules of HyperRogue, but a bit harder to understand."
    );
  }

EX void crystal_knight_help() {  
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init();
  
  dialog::addHelp(XLAT(
    "This is a representation of four-dimensional geometry. Can you find the Holy Grail in the center of the Round Table?\n\n"
    "In 'Knight of the 16-Cell Table', each cell has 8 adjacent cells, "
    "which correspond to 8 adjacent points in the four-dimensional grid. The Round Table has the shape of a 16-cell.\n\n"
    "In 'Knight of the 3-Spherical Table', it is the same map, but double bitruncated. The Round Table has the shape of a hypersphere.\n\n"
    ));

  dialog::addItem(XLAT("let me understand how the coordinates work"), 'e');
  dialog::add_action([] { cheater = true; view_coordinates = true; compass_probability = 1; restart_game(); popScreenAll(); });

  dialog::addItem(XLAT("thanks, I need no hints (achievement)"), 't');
  dialog::add_action([] { view_coordinates = false; compass_probability = 0; restart_game(); popScreenAll(); });

  dialog::addItem(XLAT("more about this geometry..."), 'm');
  dialog::add_action([] { popScreenAll(); pushScreen(show); });
  
  dialog::display();
  }

EX void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("dimensional crystal"));
  for(int i=5; i<=14; i++) {
    string s;
    if(i % 2) s = its(i/2) + ".5D";
    else s = its(i/2) + "D";
    dialog::addBoolItem(s, cryst && ginf[gCrystal].sides == i && ginf[gCrystal].vertex == 4, 'a' + i - 5);
    dialog::add_action(dialog::add_confirmation([i]() { set_crystal(i); start_game(); }));
    }
  dialog::addBoolItem(XLAT("4D double bitruncated"), ginf[gCrystal].vertex == 3, 'D');
  dialog::add_action(dialog::add_confirmation([]() { set_crystal(8); set_variation(eVariation::bitruncated); set_variation(eVariation::bitruncated); start_game(); }));
  dialog::addBreak(50);
  dialog::addBoolItem_action(XLAT("view coordinates in the cheat mode"), view_coordinates, 'v');
  dialog::addSelItem(XLAT("compass probability"), fts(compass_probability), 'p');
  if(geometry == gCrystal344) dialog::lastItem().value += " (N/A)";
  dialog::add_action([]() { 
    dialog::editNumber(compass_probability, 0, 1, 0.1, 1, XLAT("compass probability"), compass_help()); 
    dialog::bound_low(0);
    });
#if CAP_RUG
  if(cryst && WDIM == 2) {
    dialog::addBoolItem(XLAT("3D display"), rug::rugged, 'r');
    dialog::add_action_push(rug::show);
    }
  else
    dialog::addBreak(100);
  if(rug::rugged && cryst && ginf[gCrystal].sides == 8 && WDIM == 2) {
    dialog::addBoolItem(XLAT("render a cut"), draw_cut, 'x');
    dialog::add_action([]() { 
      draw_cut = true;
      dialog::editNumber(cut_level, -1, 1, 0.1, 0, XLAT("cut level"), ""); 
      dialog::extra_options = [] {
        dialog::addItem(XLAT("disable"), 'D');
        dialog::add_action([] { draw_cut = false; popScreen(); });
        };
      });
    }
  else dialog::addBreak(100);
#endif
  dialog::addSelItem(XLAT("Crystal torus"), its(crystal_period), 'C');
  dialog::add_action([] {
    dialog::editNumber(crystal_period, 0, 16, 2, 0, XLAT("Crystal torus"), 
      XLAT("Z_k^d instead of Z^d. Only works with k even."));
    dialog::reaction_final = [] {
      if(cryst) stop_game();
      set_crystal_period_flags();
      if(cryst) start_game();
      };      
    });
  dialog::addBack();
  dialog::addHelp();
  dialog::add_action([] { gotoHelp(make_help()); });
  dialog::display();
  }

auto crystalhook = 
#if CAP_COMMANDLINE
    addHook(hooks_args, 100, readArgs)
#endif
  + addHook(hooks_drawcell, 100, crystal_cell)
  + addHook(hooks_tests, 200, unit_test_tables);

map<pair<int, int>, bignum> volume_memo;

bignum& compute_volume(int dim, int rad) {
  auto p = make_pair(dim, rad);
  int is = volume_memo.count(p);
  auto& m = volume_memo[p];
  if(is) return m;
  if(dim == 0) { m = 1; return m; }
  m = compute_volume(dim-1, rad);
  for(int r=0; r<rad; r++) 
    m.addmul(compute_volume(dim-1, r), 2);
  return m;
  }

// shift_data_zero.children[x1].children[x2]....children[xk].result[r2]
// is the number of grid points in distance at most sqrt(r2) from (x1,x2,...,xk)

struct eps_comparer {
  bool operator() (ld a, ld b) const { return a < b-1e-6; }
  };

struct shift_data {
  shift_data *parent;
  ld shift;
  map<ld, shift_data, eps_comparer> children;
  map<ld, bignum, eps_comparer> result;
  
  shift_data() { parent = NULL; }
  
  bignum& compute(ld rad2) {
    if(result.count(rad2)) return result[rad2];
    // println(hlog, "compute ", format("%p", this), " [shift=", shift, "], r2 = ", rad2);
    // indenter i(2);
    auto& b = result[rad2];
    if(!parent) {
      if(rad2 >= 0) b = 1;
      }
    else if(rad2 >= 0) {
      for(int x = -2-sqrt(rad2); x <= sqrt(rad2)+2; x++) {
        ld ax = x - shift;
        if(ax*ax <= rad2) 
          b.addmul(parent->compute(rad2 - (ax*ax)), 1);
        }
      }
    // println(hlog, "result = ", b.get_str(100));
    return b;
    }
  };

shift_data shift_data_zero;

EX string get_table_volume() {
  if(!pure()) {
    auto m = crystal_map();
    bignum res;
    manual_celllister cl;
    cl.add(m->gamestart());
    ld rad2 = pow(roundTableRadius(NULL) / m->camelot_mul / PERIOD, 2) + 1e-4;
    for(int i=0; i<isize(cl.lst); i++) {
      cell *c = cl.lst[i];
      ld mincoord = 9, maxcoord = -9;
      auto co = m->get_coord(c);
      for(int i=0; i<m->cs.dim; i++) {
        if(co[i] < mincoord) mincoord = co[i];
        if(co[i] > maxcoord) maxcoord = co[i];
        }
      static const ld eps = 1e-4;
      if(mincoord >= 0-eps && maxcoord < PERIOD-eps) {
        ld my_rad2 = rad2;
        auto cshift = (co - m->camelot_coord) / PERIOD;
        auto sd = &shift_data_zero;
        for(int i=0; i<m->cs.dim; i++) {
          if(i == m->cs.dim-1 && (m->cs.dir&1)) {
            my_rad2 -= pow(cshift[i] / m->camelot_mul, 2);
            }
          else {
            ld val = cshift[i] - floor(cshift[i]);
            if(!sd->children.count(val)) {
              sd->children[val].parent = sd;
              sd->children[val].shift = val;
              }
            sd = &sd->children[val];
            }
          }
        res.addmul(sd->compute(my_rad2), 1);
        }
      if(mincoord < -2 || maxcoord > 6) continue;
      forCellCM(c2, c) cl.add(c2);
      }
    return res.get_str(100);
    }
  int s = ginf[gCrystal].sides;
  int r = roundTableRadius(NULL);
  if(s % 2 == 0)
    return compute_volume(s/2, r-1).get_str(100);
  else
    return (compute_volume(s/2, r-1) + compute_volume(s/2, r-2)).get_str(100);
  }

EX string get_table_boundary() {
  if(!pure()) return "";
  int r = roundTableRadius(NULL);
  int s = ginf[gCrystal].sides;
  if(s % 2 == 0)
    return (compute_volume(s/2, r) - compute_volume(s/2, r-1)).get_str(100);
  else
    return (compute_volume(s/2, r) - compute_volume(s/2, r-2)).get_str(100);
  }

EX void may_place_compass(cell *c) {
  if(c != c->master->c7) return;
  if(WDIM == 3) return;
  auto m = crystal_map();
  auto co = m->hcoords[c->master];
  for(int i=0; i<m->cs.dim; i++) 
    if(co[i] % PERIOD)
      return;
  if(hrandf() < compass_probability)
    c->item = itCompass;
  }
#endif

#if CAP_CRYSTAL && MAXMDIM >= 4

euc::coord crystal_to_euclid(coord x) {
  return euc::coord(x[0]/2, x[1]/2, x[2]/2);
  }

coord euclid3_to_crystal(euc::coord x) {  
  coord res;
  for(int i=0; i<3; i++) res[i] = x[i] * 2;
  for(int i=3; i<MAXDIM; i++) res[i] = 0;
  return res;
  }
  

void transform_crystal_to_euclid () {
  euc::clear_torus3();
  geometry = gCubeTiling;
  auto e = euc::new_map();
  auto m = crystal_map();
  auto infront = cwt.cpeek();
  
  auto& spacemap = euc::get_spacemap();
  auto& ispacemap = euc::get_ispacemap();
  auto& camelot_center = euc::get_camelot_center();
  auto& shifttable = euc::get_current_shifttable();
  
  for(auto& p: m->hcoords) {
    auto co = crystal_to_euclid(p.second);
    spacemap[co] = p.first;
    ispacemap[p.first] = co;
    
    cell* c = p.first->c7;

    // rearrange the monster directions
    if(c->mondir < S7 && c->move(c->mondir)) {
      auto co1 = crystal_to_euclid(m->hcoords[c->move(c->mondir)->master]) - co;
      for(int i=0; i<6; i++) 
        if(co1 == shifttable[i])
          c->mondir = i;
      }
    
    for(int i=0; i<S7; i++) c->move(i) = NULL;
    }
  
  if(m->camelot_center) 
    camelot_center = spacemap[crystal_to_euclid(m->hcoords[m->camelot_center->master])]->c7;

  // clean hcoords and heptagon_at so that the map is not deleted when we delete m
  m->hcoords.clear();
  m->heptagon_at.clear();
  delete m;

  for(int i=0; i<isize(allmaps); i++) 
    if(allmaps[i] == m)
      allmaps[i] = e;

  currentmap = e;  
  
  // connect the cubes  
  for(auto& p: spacemap) {
    auto& co = p.first;
    auto& h = p.second;
    for(int i=0; i<S7; i++) 
      if(spacemap.count(co + shifttable[i]))
        h->move(i) = spacemap[co + shifttable[i]],
        h->c.setspin(i, (i + 3) % 6, false),
        h->c7->move(i) = h->move(i)->c7,
        h->c7->c.setspin(i, (i + 3) % 6, false);
    }
  
  clearAnimations();
  cwt.spin = neighborId(cwt.at, infront);
  View = iddspin(cwt.at, cwt.spin, 90._deg);
  if(!flipplayer) View = cspin180(0, 2) * View;
  
  if(pmodel == mdDisk) pmodel = mdPerspective;
  }

void transform_euclid_to_crystal () {
  geometry = gCrystal;
  ginf[gCrystal].sides = 6;
  ginf[gCrystal].vertex = 4;
  ginf[gCrystal].tiling_name = "{6,4}";
  ginf[gCrystal].distlimit = distlimit_table[6];

  auto e = currentmap;
  auto m = new hrmap_crystal;
  auto infront = cwt.cpeek();

  auto& spacemap = euc::get_spacemap();
  auto& ispacemap = euc::get_ispacemap();
  auto& camelot_center = euc::get_camelot_center();
  
  for(auto& p: ispacemap) {
    auto co = euclid3_to_crystal(p.second);
    m->heptagon_at[co] = p.first;
    m->hcoords[p.first] = co;
    }

  for(auto& p: ispacemap) {
    cell *c = p.first->c7;
    if(c->mondir < S7 && c->move(c->mondir)) {
      auto co = euclid3_to_crystal(p.second);
      for(int d=0; d<S7; d++) {
        auto lw = m->makewalker(co, d);
        auto co1 = add(co, lw, FULLSTEP);
        if(m->heptagon_at.count(co1) && m->heptagon_at[co1] == c->move(c->mondir)->master)
          c->mondir = d;
        }
      }
    for(int i=0; i<S7; i++) c->move(i) = NULL;
    }
          
  if(camelot_center) 
    m->camelot_center = m->heptagon_at[euclid3_to_crystal(ispacemap[camelot_center->master])]->c7;

  spacemap.clear();
  ispacemap.clear();
  delete e;

  for(int i=0; i<isize(allmaps); i++) 
    if(allmaps[i] == e)
      allmaps[i] = m;

  currentmap = m;
  
  // connect the cubes  
  for(auto& p: m->heptagon_at) {
    auto& co = p.first;
    auto& h = p.second;
    for(int i=0; i<S7; i++) {
      auto lw = m->makewalker(co, i);
      auto co1 = add(co, lw, FULLSTEP);
      if(m->heptagon_at.count(co1)) {
        auto lw1 = lw+wstep;
        h->move(i) = m->heptagon_at[co1],
        h->c.setspin(i, lw1.spin, false),
        h->c7->move(i) = h->move(i)->c7;
        h->c7->c.setspin(i, h->c.spin(i), false);
        }
      }
    }
  
  View = Id;
  clearAnimations();
  cwt.spin = neighborId(cwt.at, infront);
  if(pmodel == mdPerspective) pmodel = mdDisk;
  }

EX void add_crystal_transform(char c) {
  if(shmup::on) return;
  if(cryst && ginf[gCrystal].sides == 6 && geometry != gCrystal344) {
    dialog::addItem("convert Crystal to 3D", c);
    dialog::add_action(transform_crystal_to_euclid);
    }
  if(geometry == gCubeTiling && !quotient) {
    dialog::addItem("convert 3D to Crystal", c);
    dialog::add_action(transform_euclid_to_crystal);
    }
  }

#endif

}

}


