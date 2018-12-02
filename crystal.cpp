// Hyperbolic Rogue
// This file implements the multi-dimensional (aka crystal) geometries.
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

namespace crystal {

bool pure() {
  return PURE && ginf[gCrystal].vertex == 4;
  }

bool view_coordinates = false;

const int MAXDIM = 7;

typedef array<int, MAXDIM> coord;
static const coord c0 = {};

typedef array<ld, MAXDIM> ldcoord;
static const ldcoord ldc0 = {};

ldcoord told(coord c) { ldcoord a; for(int i=0; i<MAXDIM; i++) a[i] = c[i]; return a; }
// strange number to prevent weird acting in case of precision errors
coord roundcoord(ldcoord c) { coord a; for(int i=0; i<MAXDIM; i++) a[i] = floor(c[i] + .5136); return a; }

ldcoord operator + (ldcoord a, ldcoord b) { ldcoord r; for(int i=0; i<MAXDIM; i++) r[i] = a[i] + b[i]; return r; }
ldcoord operator - (ldcoord a, ldcoord b) { ldcoord r; for(int i=0; i<MAXDIM; i++) r[i] = a[i] - b[i]; return r; }
ldcoord operator * (ldcoord a, ld v) { ldcoord r; for(int i=0; i<MAXDIM; i++) r[i] = a[i] * v; return r; }
ldcoord operator / (ldcoord a, ld v) { ldcoord r; for(int i=0; i<MAXDIM; i++) r[i] = a[i] / v; return r; }

ld operator | (ldcoord a, ldcoord b) { ld r=0; for(int i=0; i<MAXDIM; i++) r += a[i] * b[i]; return r; }

int tocode(int cname) { return (1 << (cname >> 1)); }

void resize2(vector<vector<int>>& v, int a, int b, int z) {
  v.clear();
  v.resize(a);
  for(auto& w: v) w.resize(b, z);
  }

const int FULLSTEP = 2;
const int HALFSTEP = 1;

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
  
  int errors = 0;
  
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
      int take_what = dir;
      if(i >= (1<<(dim-1))) take_what = dir-1;
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
      crystal_structure csx = move(*this);
      add_dimension_to(csx);
      }
    if(dir > S7) remove_half_dimension();
      
    next_to_coord();
    
    coord_to_order();
    coord_to_next();
    if(count_bugs()) {
      printf("bugs found\n");
      }
    if(dir > MAX_EDGE || dim > MAXDIM) {
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

ld hypot2(crystal_structure& cs, ldcoord co1, ldcoord co2) {
  int result = 0;
  for(int a=0; a<cs.dim; a++) result += (co1[a] - co2[a]) * (co1[a] - co2[a]);
  return result;
  }

void crystalstep(heptagon *h, int d);

static const int Modval = 64;

struct east_structure {
  map<coord, int> data;
  int Xmod, cycle;
  int zeroshift;
  int coordid;
  };

struct hrmap_crystal : hrmap {
  heptagon *getOrigin() { return get_heptagon_at(c0, S7); }

  map<heptagon*, coord> hcoords;
  map<coord, heptagon*> heptagon_at;
  map<int, eLand> landmemo;
  unordered_map<cell*, unordered_map<cell*, int>> distmemo;
  map<cell*, ldcoord> sgc;
  cell *camelot_center;
  
  crystal_structure cs;
  east_structure east;   

  lwalker makewalker(coord c, int d) {
    lwalker a(cs);
    a.id = 0;
    for(int i=0; i<cs.dim; i++) if(c[i] & FULLSTEP) a.id += (1<<i);
    a.spin = d;
    return a;
    }
  
  hrmap_crystal() {
    cs.build();
    }

  ~hrmap_crystal() {
    clearfrom(getOrigin());
    }
  
  heptagon *get_heptagon_at(coord c, int deg) {
    if(heptagon_at.count(c)) return heptagon_at[c];
    heptagon*& h = heptagon_at[c];
    h = tailored_alloc<heptagon> (deg);
    h->alt = NULL;
    h->cdata = NULL;
    h->c7 = newCell(deg, h);
    h->distance = 0;
    for(int i=0; i<cs.dim; i++) h->distance += abs(c[i]);
    h->distance /= 2;
    hcoords[h] = c;
    // for(int i=0; i<6; i++) crystalstep(h, i);
    return h;
    }
  
  ldcoord get_coord(cell *c) {
    auto b = sgc.emplace(c, ldc0);
    ldcoord& res = b.first->second;
    if(b.second) {
      if(c->master->c7 != c) {
        for(int i=0; i<c->type; i+=2)
          res = res + told(hcoords[c->cmove(i)->master]);
        res = res * 2 / c->type;
        }
      else
        res = told(hcoords[c->master]);
      }
    return res;
    }
  
  coord long_representant(cell *c);  

  int get_east(cell *c);

  void build_east(int cid);
  
  void verify() { }
  
  void prepare_east();
  };

hrmap_crystal *crystal_map() {
  return (hrmap_crystal*) currentmap;
  } 

bool is_bi(crystal_structure& cs, coord co) {
  for(int i=0; i<cs.dim; i++) if(co[i] & HALFSTEP) return true;
  return false;
  }

void create_step(heptagon *h, int d) {
  auto m = crystal_map();
  if(geometry != gCrystal) return;
  if(!m->hcoords.count(h)) {
    printf("not found\n");
    return;
    }
  auto co = m->hcoords[h];
  
  if(is_bi(m->cs, co)) {
    heptspin hs(h, d);
    (hs + 1 + wstep + 1).cpeek();
    return;
    }

  auto lw = m->makewalker(co, d);

  if(ginf[gCrystal].vertex == 4) {
    auto c1 = add(co, lw, FULLSTEP);
    auto lw1 = lw+wstep;
    
    h->c.connect(d, heptspin(m->get_heptagon_at(c1, S7), lw1.spin));
    }
  else {
    auto coc = add(add(co, lw, HALFSTEP), lw+1, HALFSTEP);
    auto hc = m->get_heptagon_at(coc, 8);
    for(int a=0; a<8; a+=2) {
      hc->c.connect(a, heptspin(h, lw.spin));
      if(h->modmove(lw.spin-1)) {
        hc->c.connect(a+1, heptspin(h, lw.spin) - 1 + wstep - 1);
        }
      co = add(co, lw, FULLSTEP);
      lw = lw + wstep + (-1);
      h = m->get_heptagon_at(co, S7);
      }
    }
  }

array<array<int,2>, MAX_EDGE> distlimit_table = {{
  {SEE_ALL,SEE_ALL}, {SEE_ALL,SEE_ALL}, {SEE_ALL,SEE_ALL}, {SEE_ALL,SEE_ALL}, {15, 10}, 
  {6, 4}, {5, 3}, {4, 3}, {4, 3}, {3, 2}, {3, 2}, {3, 2}, {3, 2}, {3, 2}
  }};

color_t colorize(cell *c) {
  auto m = crystal_map();
  ldcoord co = m->get_coord(c);
  color_t res;
  res = 0;
  for(int i=0; i<3; i++)
    res |= ((int)(((i == 2 && S7 == 5) ? (128 + co[i] * 50) : (255&int(128 + co[i] * 50))))) << (8*i);
  return res;
  }

bool crystal_cell(cell *c, transmatrix V) {

  if(geometry != gCrystal) return false;

  if(view_coordinates && cheater) {
    int d = dist_alt(c);
    queuestr(V, 0.3, its(d), 0xFFFFFF, 1);
    }

  if(view_coordinates && cheater) for(int i=0; i<S7; i++) {
    
    auto m = crystal_map();

    if(c->master->c7 == c) {    
      transmatrix V1 = cellrelmatrix(c, i);
      ld dist = hdist0(V1 * C0);
      ld alpha = -atan2(V1 * C0);
      transmatrix T = V * spin(alpha) * xpush(dist*.3);

      auto co = m->hcoords[c->master];
      int our_id = 0;
      for(int a=0; a<MAXDIM; a++) if(co[a] & FULLSTEP) our_id += (1<<a);
      int cx = m->cs.cmap[our_id][i];
      
      int coordcolors[MAXDIM] = {0x4040D0, 0x40D040, 0xD04040, 0xFFD500, 0xF000F0, 0x00F0F0, 0xF0F0F0 };
    
      queuestr(T, 0.3, its(co[cx>>1] / (ginf[gCrystal].vertex == 3 ? HALFSTEP : FULLSTEP)), coordcolors[cx>>1], 1);
      }                                 
    }
  return false;
  }

int precise_distance(cell *c1, cell *c2) {
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
      if(dot > mmax + 2.5) continue;

      for(int k=0; k<m->cs.dim; k++) if(abs(h[k] - dot * mul[k]) > 4.1) goto next3;
      cl.add(c3);
      next3: ;
      }
    }
  
  println(hlog, "Error: distance not found");
  return 999999;
  }

ld space_distance(cell *c1, cell *c2) {
  auto m = crystal_map();
  ldcoord co1 = m->get_coord(c1);
  ldcoord co2 = m->get_coord(c2);
  return sqrt(hypot2(m->cs, co1, co2));
  }

int dist_relative(cell *c) {
  auto m = crystal_map();
  auto& cc = m->camelot_center;
  int r = roundTableRadius(NULL);
  cell *start = m->gamestart();
  if(!cc) {
    printf("Finding Camelot center...");
    cc = start;
    while(precise_distance(cc, start) < r + 5)
      cc = cc->cmove(hrand(cc->type));
    }

  if(pure()) 
    return precise_distance(c, cc) - r;

  ld sdmul = (r+5) / space_distance(cc, start);
  ld dis = space_distance(cc, c) * sdmul;
  println(hlog, "dis = ", dis);
  if(dis < r) 
    return int(dis) - r;
  else {
    forCellCM(c1, c) if(space_distance(cc, c1) * sdmul < r)
      return 0;
    return int(dis) + 1 - r;
    }
  }

coord hrmap_crystal::long_representant(cell *c) {
  auto& coordid = east.coordid;
  auto co = roundcoord(get_coord(c) * Modval/4);
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

int dist_alt(cell *c) {
  auto m = crystal_map();
  if(specialland == laCamelot && m->camelot_center) {
    if(pure()) 
      return precise_distance(c, m->camelot_center);
    if(c == m->camelot_center) return 0;
    return 1 + int(4 * space_distance(m->camelot_center, c));
    }
  else {
    m->prepare_east();
    return m->get_east(c);
    }
  }

ld crug_rotation[MAXDIM][MAXDIM];

int ho = 1;

void init_rotation() {
  for(int i=0; i<MAXDIM; i++)
  for(int j=0; j<MAXDIM; j++)
    crug_rotation[i][j] = i == j ? 1/2. : 0;
  
  auto& cs = crystal_map()->cs;

  if(ho & 1) {
    for(int i=cs.dim-1; i>=1; i--) {
      ld c = cos(M_PI / 2 / (i+1));
      ld s = sin(M_PI / 2 / (i+1));
      for(int j=0; j<cs.dim; j++)
        tie(crug_rotation[j][0], crug_rotation[j][i]) =
          make_pair(
             crug_rotation[j][0] * s + crug_rotation[j][i] * c,
            -crug_rotation[j][i] * s + crug_rotation[j][0] * c
            );
      }
    }
  }

void next_home_orientation() {
  ho++;
  init_rotation();
  }

hyperpoint coord_to_flat(ldcoord co) {
  hyperpoint res = hpxyz(0, 0, 0);
  for(int a=0; a<MAXDIM; a++)
    for(int b=0; b<3; b++)
      res[b] += crug_rotation[b][a] * co[a];
  return res;
  }

void switch_z_coordinate() {
  auto& cs = crystal_map()->cs;
  for(int i=0; i<cs.dim; i++) {
    ld tmp = crug_rotation[i][2];
    for(int u=2; u<cs.dim-1; u++) crug_rotation[i][u] = crug_rotation[i][u+1];
    crug_rotation[i][cs.dim-1] = tmp;
    }
  }

void apply_rotation(const transmatrix t) {
  for(int i=0; i<MAXDIM; i++) {
    hyperpoint h;
    for(int j=0; j<3; j++) h[j] = crug_rotation[i][j];
    h = t * h;
    for(int j=0; j<3; j++) crug_rotation[i][j] = h[j];
    }
  }

void build_rugdata() {
  using namespace rug;
  rug::clear_model(); 
  rug::good_shape = true;  
  rug::vertex_limit = 0;
  auto m = crystal_map();
  
  for(const auto& gp: gmatrix) {
            
    cell *c = gp.first;
    const transmatrix& V = gp.second;
    
    rugpoint *v = addRugpoint(tC0(V), 0);
    auto co = m->get_coord(c);
    v->flat = coord_to_flat(co);
    v->valid = true;
    
    rugpoint *p[MAX_EDGE];
    
    for(int i=0; i<c->type; i++) {
      p[i] = addRugpoint(V * get_corner_position(c, i), 0);
      p[i]->valid = true;
      if(VALENCE == 4)
        p[i]->flat = coord_to_flat((m->get_coord(c->cmove(i)) + m->get_coord(c->cmodmove(i-1))) / 2);
      else
        p[i]->flat = coord_to_flat((m->get_coord(c->cmove(i)) + m->get_coord(c->cmodmove(i-1)) + co) / 3);
      }

    for(int i=0; i<c->type; i++) addTriangle(v, p[i], p[(i+1) % c->type]);
    }
  }

eLand getCLand(int x) {
  auto& landmemo = crystal_map()->landmemo;
     
  if(landmemo.count(x)) return landmemo[x]; 
  if(x > 0) return landmemo[x] = getNewLand(landmemo[x-1]);
  if(x < 0) return landmemo[x] = getNewLand(landmemo[x+1]);
  return landmemo[x] = laCrossroads;
  }

void set_land(cell *c) {
  setland(c, specialland); 
  auto m = crystal_map();

  auto co = m->get_coord(c);
  auto co1 = roundcoord(co * 60);
  int cv = co1[0];

  if(specialland == laCrossroads) {
    eLand l1 = getCLand(gdiv(cv, 360));
    eLand l2 = getCLand(gdiv(cv+59, 360));
    if(l1 != l2) setland(c, laBarrier);
    else setland(c, l1);
    }
  
  if(specialland == laCamelot) {
    setland(c, laCrossroads);
    buildCamelot(c);
    }

  if(specialland == laTerracotta) {
    int v = dist_alt(c);
    if(((v&15) == 8) && hrand(100) < 90)
       c->wall = waMercury;
    }

  if(among(specialland, laOcean, laIvoryTower, laDungeon)) {
    int v = dist_alt(c);
    if(v == 0)
      c->land = laCrossroads4;
    else if(v > 0)
      c->landparam = v;
    else
      c->landparam = -v;
    }
  
  if(specialland == laWarpCoast) {
    if(gmod(cv, 240) >= 120)
      c->land = laWarpSea;
    }
  }

void set_crystal(int sides) {
  stop_game();
  set_geometry(gCrystal);
  set_variation(eVariation::pure);
  ginf[gCrystal].sides = sides;
  ginf[gCrystal].vertex = 4;
  if(sides < MAX_EDGE)
    ginf[gCrystal].distlimit = distlimit_table[sides];
  }

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
  else if(argis("-crug")) {
    PHASE(3);
    if(rug::rugged) rug::close();
    calcparam();
    rug::reopen();
    init_rotation();
    surface::sh = surface::dsCrystal;
    rug::good_shape = true;
    }
  else return 1;
  return 0;
  }

hrmap *new_map() {
  return new hrmap_crystal;
  }

void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);  
  dialog::init(XLAT("multi-dimensional"));
  for(int i=5; i<=14; i++) {
    string s;
    if(i % 2) s = its(i/2) + ".5D";
    else s = its(i/2) + "D";
    dialog::addBoolItem(s, geometry == gCrystal && ginf[gCrystal].sides == i && ginf[gCrystal].vertex == 4, 'a' + i - 5);
    dialog::add_action([i]() { set_crystal(i); start_game(); });
    }
  dialog::addBoolItem("4D double bitruncated", ginf[gCrystal].vertex == 3, 'D');
  dialog::add_action([]() { set_crystal(8); set_variation(eVariation::bitruncated); set_variation(eVariation::bitruncated); });
  dialog::addBreak(50);
  dialog::addBoolItem("view coordinates in the cheat mode", view_coordinates, 'v');
  dialog::add_action([]() { view_coordinates = !view_coordinates; });
  if(geometry == gCrystal) {
    dialog::addBoolItem("3D display", rug::rugged, 'r');
    dialog::add_action([]() { pushScreen(rug::show); });
    }
  else
    dialog::addBreak(100);
  dialog::addBack();
  dialog::display();
  }

auto crystalhook = addHook(hooks_args, 100, readArgs)
  + addHook(hooks_drawcell, 100, crystal_cell);

}

}
