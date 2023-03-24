// Hyperbolic Rogue -- cells
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file cell.cpp 
 *  \brief General cells and maps
 *
 *  Start with locations.cpp
 */

#include "hyper.h"
namespace hr {

#if HDR
extern int default_levs();

struct hrmap {
  virtual heptagon *getOrigin() { return NULL; }
  virtual cell *gamestart() { return getOrigin()->c7; }
  virtual ~hrmap() { }
  virtual vector<cell*>& allcells();
  virtual void verify() { }
  virtual void on_dim_change() { }
  virtual bool link_alt(heptagon *h, heptagon *alt, hstate firststate, int dir);
  virtual void extend_altmap(heptagon *h, int levs = default_levs(), bool link_cdata = true);
  heptagon *may_create_step(heptagon *h, int direction) {
    if(h->move(direction)) return h->move(direction);
    return create_step(h, direction);
    }
  virtual heptagon *create_step(heptagon *h, int direction);
protected:
  virtual transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint);
  virtual transmatrix relative_matrixc(cell *c2, cell *c1, const hyperpoint& hint);
public:
  transmatrix relative_matrix(heptagon *h2, heptagon *h1, const hyperpoint& hint) { return relative_matrixh(h2, h1, hint); }
  transmatrix relative_matrix(cell *h2, cell *h1, const hyperpoint& hint) { return relative_matrixc(h2, h1, hint); }
  
  virtual transmatrix adj(cell *c, int i) { return adj(c->master, i); }
  virtual transmatrix adj(heptagon *h, int i);
  transmatrix iadj(cell *c, int i) { cell *c1 = c->cmove(i); return adj(c1, c->c.spin(i)); }
  transmatrix iadj(heptagon *h, int d) { 
    heptagon *h1 = h->cmove(d); return adj(h1, h->c.spin(d));
    }
  virtual void draw_all();
  virtual void draw_at(cell *at, const shiftmatrix& where);

  virtual void virtualRebase(heptagon*& base, transmatrix& at);

  static constexpr ld SPIN_NOT_AVAILABLE = 1e5;
  virtual ld spin_angle(cell *c, int d) { return SPIN_NOT_AVAILABLE; }
  
  virtual transmatrix spin_to(cell *c, int d, ld bonus=0);
  virtual transmatrix spin_from(cell *c, int d, ld bonus=0);
  
  virtual double spacedist(cell *c, int i);
  
  virtual bool strict_tree_rules() { return false; }

  virtual void find_cell_connection(cell *c, int d);
  virtual int shvid(cell *c) { return 0; }
  virtual int full_shvid(cell *c) { return shvid(c); }
  virtual hyperpoint get_corner(cell *c, int cid, ld cf=3) { return C0; }
  virtual transmatrix master_relative(cell *c, bool get_inverse = false) { return Id; }
  virtual int wall_offset(cell *c);

  virtual transmatrix ray_iadj(cell *c, int i);

  virtual subcellshape& get_cellshape(cell *c);

  /** \brief in 3D honeycombs, returns a cellwalker res at cw->move(j) such that the face pointed at by cw and res share an edge */
  virtual cellwalker strafe(cellwalker cw, int j);

  /** \brief in 3D honeycombs, returns a vector<bool> v, where v[j] iff faces i and j are adjacent */
  const vector<char>& dirdist(cellwalker cw) { return get_cellshape(cw.at).dirdist[cw.spin]; }

  /** \brief the sequence of heptagon movement direction to get from c->master to c->move(i)->master; implemented only for reg3 */
  virtual const vector<int>& get_move_seq(cell *c, int i);

  /** generate a new map that is disconnected from what we already have, disconnected from the map we have so far */
  virtual cell* gen_extra_origin(int fv) { throw hr_exception("gen_extra_origin not supported on this map"); }
  };

/** hrmaps which are based on regular non-Euclidean 2D tilings, possibly quotient  
 *  Operators can be applied to these maps. 
 *  Liskov substitution warning: maps which produce both tiling like above and 3D tilings
 *  (e.g. Euclidean and Crystal) also inherit from hrmap_standard
 **/
struct hrmap_standard : hrmap {
  void draw_at(cell *at, const shiftmatrix& where) override;
  transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override;
  transmatrix relative_matrixc(cell *c2, cell *c1, const hyperpoint& hint) override;
  heptagon *create_step(heptagon *h, int direction) override;
  transmatrix adj(cell *c, int d) override;
  transmatrix adj(heptagon *h, int d) override;
  ld spin_angle(cell *c, int d) override;
  double spacedist(cell *c, int i) override;
  void find_cell_connection(cell *c, int d) override;
  virtual int shvid(cell *c) override;
  virtual hyperpoint get_corner(cell *c, int cid, ld cf) override;
  virtual transmatrix master_relative(cell *c, bool get_inverse) override;
  virtual bool link_alt(heptagon *h, heptagon *alt, hstate firststate, int dir) override;
  };

void clearfrom(heptagon*);
void verifycells(heptagon*);

struct hrmap_hyperbolic : hrmap_standard {
  heptagon *origin;
  hrmap_hyperbolic();
  hrmap_hyperbolic(heptagon *origin);
  heptagon *getOrigin() override { return origin; }
  ~hrmap_hyperbolic() {
    // verifycells(origin);
    // printf("Deleting hyperbolic map: %p\n", hr::voidp(this));
    clearfrom(origin);
    }
  void verify() override { verifycells(origin); }
  void virtualRebase(heptagon*& base, transmatrix& at) override;
  };
#endif

double hrmap::spacedist(cell *c, int i) { return hdist(tile_center(), adj(c, i) * tile_center()); }

heptagon *hrmap::create_step(heptagon *h, int direction) {
  throw hr_exception("create_step called unexpectedly");
  return NULL;
  }

transmatrix hrmap::relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) {
  println(hlog, "relative_matrixh called unexpectedly\n"); 
  return Id;
  }

transmatrix hrmap::relative_matrixc(cell *c2, cell *c1, const hyperpoint& hint) {
  return relative_matrixh(c2->master, c1->master, hint);
  }

bool hrmap::link_alt(heptagon *h, heptagon *alt, hstate firststate, int dir) {
  return true; 
  }

bool hrmap_standard::link_alt(heptagon *h, heptagon *alt, hstate firststate, int dir) {
  altmap::relspin(alt) = 3;
  return true;
  }

void hrmap::virtualRebase(heptagon*& base, transmatrix& at) {
  printf("virtualRebase called unexpectedly\n"); 
  return;
  }

transmatrix hrmap::ray_iadj(cell *c, int i) {
  if(WDIM == 2) {
    return to_other_side(get_corner(c, i), get_corner(c, (i+1)));
    }
  return currentmap->iadj(c, i);
  }

subcellshape& hrmap::get_cellshape(cell *c) { 
  if(cgi.heptshape) return *cgi.heptshape;
  throw hr_exception("get_cellshape called unexpectedly"); 
  }

cellwalker hrmap::strafe(cellwalker cw, int j) {
  throw hr_exception("strafe called unexpectedly");
  }

const vector<int>& hrmap::get_move_seq(cell *c, int i) {
  throw hr_exception("get_move_seq not implemented for this map class");
  }

transmatrix hrmap::spin_to(cell *c, int d, ld bonus) {
  ld sa = spin_angle(c, d);
  if(sa != SPIN_NOT_AVAILABLE) { return spin(bonus + sa); }
  transmatrix T = lrspintox(tC0(adj(c, d)));
  if(WDIM == 3) return T * cspin(2, 0, bonus);
  return T * spin(bonus);
  }

transmatrix hrmap::spin_from(cell *c, int d, ld bonus) {
  ld sa = spin_angle(c, d);
  if(sa != SPIN_NOT_AVAILABLE) { return spin(bonus - sa); }
  transmatrix T = lspintox(tC0(adj(c, d)));
  if(WDIM == 3) return T * cspin(2, 0, bonus);
  return T * spin(bonus);
  }

transmatrix hrmap::adj(heptagon *h, int i) { return relative_matrix(h->cmove(i), h, C0); }

vector<cell*>& hrmap::allcells() { 
  static vector<cell*> default_allcells;
  if(disksize) return all_disk_cells;
  if(closed_manifold && !(cgflags & qHUGE_BOUNDED) && !(mhybrid && hybrid::csteps == 0)) {
    celllister cl(gamestart(), 1000000, 1000000, NULL);
    default_allcells = cl.lst;
    return default_allcells;
    }
  if(isize(dcal) <= 1) {
    extern cellwalker cwt;
    celllister cl(cwt.at, 1, 1000, NULL);
    default_allcells = cl.lst;
    return default_allcells;
    }
  return dcal; 
  }

EX int dirdiff(int dd, int t) {
  dd %= t;
  if(dd<0) dd += t;
  if(t-dd < dd) dd = t-dd;
  return dd;
  }

EX int cellcount = 0;

EX void destroy_cell(cell *c) {
  tailored_delete(c);
  cellcount--;
  }

EX cell *newCell(int type, heptagon *master) {
  cell *c = tailored_alloc<cell> (type);
  c->type = type;
  c->master = master;
  initcell(c);
  hybrid::will_link(c);
  cellcount++;
  return c;
  }

// -- hrmap ---

EX hrmap *currentmap;
EX vector<hrmap*> allmaps;

EX hrmap *newAltMap(heptagon *o) { 
  #if MAXMDIM >= 4
  if(reg3::in_hrmap_rule_or_subrule())
    return reg3::new_alt_map(o);
  #endif
  if(currentmap->strict_tree_rules())
    return rulegen::new_hrmap_rulegen_alt(o);
  return new hrmap_hyperbolic(o); 
  }
// --- hyperbolic geometry ---

EX heptagon* hyperbolic_origin() {
  int odegree = geometry == gBinaryTiling ? 6 : S7;
  heptagon *origin = init_heptagon(odegree);
  heptagon& h = *origin;
  h.s = hsOrigin;
  h.emeraldval = a46 ? 0 : 98;
  h.zebraval = 40;
  #if CAP_IRR
  if(IRREGULAR) irr::link_start(origin);
  else 
  #endif
  h.c7 = newCell(odegree, origin);
  return origin;
  }

hrmap_hyperbolic::hrmap_hyperbolic(heptagon *o) { origin = o; }

hrmap_hyperbolic::hrmap_hyperbolic() { origin = hyperbolic_origin(); }

void hrmap::find_cell_connection(cell *c, int d) {
  heptagon *h2 = createStep(c->master, d);
  c->c.connect(d, h2->c7,c->master->c.spin(d), c->master->c.mirror(d));
  hybrid::link();
  }

void hrmap_standard::find_cell_connection(cell *c, int d) {
  #if CAP_IRR
  if(IRREGULAR) {
    irr::link_cell(c, d);
    }
  #else
  if(0) {}
  #endif
  #if CAP_GP
  else if(GOLDBERG) {
    gp::extend_map(c, d);
    if(!c->move(d)) {
      println(hlog, "extend failed to create for ", cellwalker(c, d));
      exit(1);
      }
    hybrid::link();
    }
  #endif
  else if(PURE) {
    hrmap::find_cell_connection(c, d);
    }
  else if(c == c->master->c7) {
    
    cell *n = newCell(S6, c->master);
    
    heptspin hs(c->master, d, false);
    
    int alt3 = c->type/2;
    int alt4 = alt3+1;
        
    for(int u=0; u<S6; u+=2) {
      if(hs.mirrored && (S7%2 == 0)) hs++;
      hs.at->c7->c.connect(hs.spin, n, u, hs.mirrored);
      if(hs.mirrored && (S7%2 == 0)) hs--;
      hs = hs + alt3 + wstep - alt4;
      }
    hybrid::link();
    extern void verifycell(cell *c);
    verifycell(n);
    }

  else {
    cellwalker cw(c, d, false);
    cellwalker cw2 = cw - 1 + wstep - 1 + wstep - 1;
    c->c.connect(d, cw2);
    hybrid::link();
    }    
  }

/** very similar to createMove in heptagon.cpp */
EX cell *createMov(cell *c, int d) {
  if(d<0 || d>= c->type)
    throw hr_exception("ERROR createmov\n");
  if(c->move(d)) return c->move(d);  
  currentmap->find_cell_connection(c, d);  
  return c->move(d);
  }

EX void eumerge(cell* c1, int s1, cell *c2, int s2, bool mirror) {
  if(!c2) return;
  c1->move(s1) = c2; c1->c.setspin(s1, s2, mirror);
  c2->move(s2) = c1; c2->c.setspin(s2, s1, mirror);
  }

//  map<pair<eucoord, eucoord>, cell*> euclidean;

EX hookset<hrmap*()> hooks_newmap;

#if HDR
enum eDiskShape { dshTiles, dshVertices, dshGeometric };
#endif

/** requested disk size */
EX int req_disksize;
/** currently used disk size */
EX int disksize;
/** all the cells in the disk */
EX vector<cell*> all_disk_cells;
/** for quick test of membership */
EX vector<cell*> all_disk_cells_sorted;
/** the disk shape to use */
EX eDiskShape diskshape;

EX void init_disk_cells() {
  disksize = req_disksize;
  all_disk_cells.clear();
  all_disk_cells_sorted.clear();
  if(!disksize) return;
  if(diskshape == dshTiles) {
    celllister cl(currentmap->gamestart(), 1000000, disksize, NULL);
    all_disk_cells = cl.lst;
    }
  else {
    struct tileinfo {
      ld dist;
      cell *c;
      transmatrix T;
      bool operator < (const tileinfo& t2) const { return -dist < -t2.dist - 1e-6; }
      };
    set<cell*> seen;
    std::priority_queue<tileinfo> tiles;
    tiles.push(tileinfo{0, currentmap->gamestart(), Id});
    ld last_dist = 0;
    dynamicval<int> dmar(mine_adjacency_rule, 1);
    while(isize(tiles)) {
      auto ti = tiles.top();
      tiles.pop();
      println(hlog, "dist=", ti.dist, " for c=", ti.c);
      if(seen.count(ti.c)) continue;
      seen.insert(ti.c);
      if(ti.dist > last_dist + 1e-6 && isize(all_disk_cells) >= disksize) break;
      last_dist = ti.dist;
      all_disk_cells.push_back(ti.c);
      for(auto p: adj_minefield_cells_full(ti.c)) {
        tileinfo next;
        next.c = p.c;
        next.T = ti.T * p.T;
        if(diskshape == dshVertices) next.dist = ti.dist + 1;
        else next.dist = hdist0(tC0(next.T));
        println(hlog, ti.c, " -> ", p.c, " at ", next.dist);
        tiles.push(next);
        }
      }
    }
  all_disk_cells_sorted = all_disk_cells;
  sort(all_disk_cells_sorted.begin(), all_disk_cells_sorted.end());
  }

EX bool is_in_disk(cell *c) {
  auto it = lower_bound(all_disk_cells_sorted.begin(), all_disk_cells_sorted.end(), c);
  if(it == all_disk_cells_sorted.end()) return false;
  return *it == c;
  }

/** create a map in the current geometry */
EX void initcells() {
  DEBB(DF_INIT, ("initcells"));

  if(embedded_plane) {
    IPF(initcells());
    currentmap->on_dim_change();
    return;
    }

  hrmap* res = callhandlers((hrmap*)nullptr, hooks_newmap);
  if(res) currentmap = res;
  #if CAP_SOLV
  else if(asonov::in()) currentmap = asonov::new_map();
  #endif
  else if(nonisotropic || mhybrid) currentmap = nisot::new_map();
  else if(INVERSE) currentmap = gp::new_inverse();
  else if(fake::in()) currentmap = fake::new_map();
  #if CAP_CRYSTAL
  else if(cryst) currentmap = crystal::new_map();
  #endif
  else if(arb::in() && rulegen::known()) currentmap = rulegen::new_hrmap_rulegen();
  else if(arb::in()) currentmap = arb::new_map();
  #if CAP_ARCM
  else if(arcm::in()) currentmap = arcm::new_map();
  #endif
  else if(euc::in()) currentmap = euc::new_map();
  else if(hat::in()) currentmap = hat::new_map();
  #if CAP_BT
  else if(kite::in()) currentmap = kite::new_map();
  #endif
  #if MAXMDIM >= 4
  else if(WDIM == 3 && !bt::in()) currentmap = reg3::new_map();
  #endif
  else if(sphere) currentmap = new_spherical_map();
  else if(quotient) currentmap = quotientspace::new_map();
  #if CAP_BT
  else if(bt::in()) currentmap = bt::new_map();
  #endif
  else if(S3 >= OINF) currentmap = inforder::new_map();
  else currentmap = new hrmap_hyperbolic;
  
  allmaps.push_back(currentmap);

  #if CAP_FIELD
  windmap::create();  
  #endif
  
  // origin->emeraldval = 
  }

EX void clearcell(cell *c) {
  if(!c) return;
  DEBB(DF_MEMORY, (format("c%d %p\n", c->type, hr::voidp(c))));
  for(int t=0; t<c->type; t++) if(c->move(t)) {
    DEBB(DF_MEMORY, (format("mov %p [%p] S%d\n", hr::voidp(c->move(t)), hr::voidp(c->move(t)->move(c->c.spin(t))), c->c.spin(t))));
    if(c->move(t)->move(c->c.spin(t)) != NULL &&
      c->move(t)->move(c->c.spin(t)) != c) {
        DEBB(DF_MEMORY | DF_ERROR, (format("cell error: type = %d %d -> %d\n", c->type, t, c->c.spin(t))));
        if(worst_precision_error < 1e-3) exit(1);
        }
    c->move(t)->move(c->c.spin(t)) = NULL;
    }
  DEBB(DF_MEMORY, (format("DEL %p\n", hr::voidp(c))));
  gp::delete_mapped(c);
  destroy_cell(c);
  }

EX heptagon deletion_marker;

template<class T> void subcell(cell *c, const T& t) {
  if(GOLDBERG) {
    forCellEx(c2, c) if(c2->move(0) == c && c2 != c2->master->c7) {
      subcell(c2, t);
      }
    }
  else if(BITRUNCATED && !arcm::in() && !bt::in())
    forCellEx(c2, c) t(c2);
  t(c);
  }

EX void clearHexes(heptagon *at) {
  if(at->c7 && at->cdata) {
    delete at->cdata;
    at->cdata = NULL;
    }
  if(0);
  #if CAP_IRR
  else if(IRREGULAR) irr::clear_links(at);
  #endif
  else if(at->c7) subcell(at->c7, clearcell);
  }

void unlink_cdata(heptagon *h) {
  if(h->alt && h->c7) {
    if(h->alt->cdata == (cdata*) h)
      h->alt->cdata = NULL;
    }
  }

EX void clear_heptagon(heptagon *at) {
  clearHexes(at);
  tailored_delete(at);
  }

EX void clearfrom(heptagon *at) {
  if(!at) return;
  queue<heptagon*> q;
  unlink_cdata(at);
  q.push(at);
  at->alt = &deletion_marker;
//int maxq = 0;
  while(!q.empty()) {
    at = q.front(); 
//  if(q.size() > maxq) maxq = q.size();
    q.pop();
    DEBB(DF_MEMORY, ("from %p", at));
    if(!at->c7) {
      heptagon *h = dynamic_cast<heptagon*> ((cdata_or_heptagon*) at->cdata);
      if(h) {
        if(h->alt != at) { DEBB(DF_MEMORY | DF_ERROR, ("alt error :: h->alt = ", h->alt, " expected ", at)); }
        cell *c = h->c7;
        subcell(c, destroycellcontents);
        h->alt = NULL;
        at->cdata = NULL;
        }
      }
    int edges = at->degree();
    if(bt::in() && WDIM == 2) edges = at->c7->type;
    for(int i=0; i<edges; i++) if(at->move(i) && at->move(i) != at) {
      if(at->move(i)->alt != &deletion_marker)
        q.push(at->move(i));    
      unlink_cdata(at->move(i));
      at->move(i)->alt = &deletion_marker;
      DEBB(DF_MEMORY, ("!mov ", at->move(i), " [", at->move(i)->move(at->c.spin(i)), "]"));
      if(at->move(i)->move(at->c.spin(i)) != NULL &&
        at->move(i)->move(at->c.spin(i)) != at) {
          DEBB(DF_MEMORY | DF_ERROR, ("hept error"));
          exit(1);
          }
      at->move(i)->move(at->c.spin(i)) = NULL;
      at->move(i) = NULL;
      }
    clearHexes(at);
    tailored_delete(at);
    }
//printf("maxq = %d\n", maxq);
  }

EX void verifycell(cell *c) {
  int t = c->type;
  for(int i=0; i<t; i++) {
    cell *c2 = c->move(i);
    if(c2) {
      if(BITRUNCATED && c == c->master->c7) verifycell(c2);
      if(c2->move(c->c.spin(i)) && c2->move(c->c.spin(i)) != c) {
        printf("cell error %p:%d [%d] %p:%d [%d]\n", hr::voidp(c), i, c->type, hr::voidp(c2), c->c.spin(i), c2->type);
        exit(1);
        }
      }
    }
  }

EX void verifycells(heptagon *at) {
  if(GOLDBERG || IRREGULAR || arcm::in()) return;
  for(int i=0; i<at->type; i++) if(at->move(i) && at->move(i)->move(at->c.spin(i)) && at->move(i)->move(at->c.spin(i)) != at) {
    printf("hexmix error %p [%d s=%d] %p %p\n", hr::voidp(at), i, at->c.spin(i), hr::voidp(at->move(i)), hr::voidp(at->move(i)->move(at->c.spin(i))));
    }
  if(!sphere && !quotient) 
    for(int i=0; i<S7; i++) if(at->move(i) && at->c.spin(i) == 0 && at->s != hsOrigin)
      verifycells(at->move(i));
  verifycell(at->c7);
  }

EX int compdist(int dx[]) {
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

EX int celldist(cell *c) {
  if(experimental) return 0;
  if(mhybrid)
    return hybrid::celldistance(c, currentmap->gamestart());
  if(nil && !quotient) return DISTANCE_UNKNOWN;
  if(hat::in()) return clueless_celldistance(currentmap->gamestart(), c);
  if(euc::in()) return celldistance(currentmap->gamestart(), c);
  if(sphere || bt::in() || WDIM == 3 || cryst || sn::in() || aperiodic || closed_manifold) return celldistance(currentmap->gamestart(), c);
  #if CAP_IRR
  if(IRREGULAR) return irr::celldist(c, false);
  #endif
  if(arcm::in() || ctof(c) || arb::in()) return c->master->distance;
  #if CAP_GP
  if(INVERSE) {
    cell *c1 = gp::get_mapped(c);
    return UIU(gp::compute_dist(c1, celldist)) / 2;
    /* TODO */
    }
  if(GOLDBERG) return gp::compute_dist(c, celldist);
  #endif
  int dx[MAX_S3];
  for(int u=0; u<S3; u++)
    dx[u] = createMov(c, u+u)->master->distance;
  return compdist(dx);
  }

#if HDR
static const int ALTDIST_BOUNDARY = 99999;
static const int ALTDIST_UNKNOWN = 99998;
static const int ALTDIST_ERROR = 90000;
#endif

EX int celldistAlt(cell *c) {
  if(experimental) return 0;
  if(mhybrid) {
    if(in_s2xe()) return hybrid::get_where(c).second;
    auto w = hybrid::get_where(c); 
    int d = c->master->alt && c->master->alt->alt ? hybrid::altmap_heights[c->master->alt->alt] : 0;
    d = sl2 ? 0 : abs(w.second - d);
    PIU ( d += celldistAlt(w.first) );
    return d;
    }
  #if CAP_BT
  if(bt::in() || sn::in()) return c->master->distance + (specialland == laCamelot && !ls::single() ? 30 : 0);
  #endif
  if(nil) return c->master->zebraval + abs(c->master->emeraldval) + (specialland == laCamelot && !ls::single() ? 30 : 0);;
  #if CAP_CRYSTAL
  if(cryst) 
    return crystal::dist_alt(c);
  #endif
  if(sphere || quotient) {
    return celldist(c) - 3;
    }
  #if MAXMDIM >= 4
  if(euc::in()) return euc::dist_alt(c);
  if(hyperbolic && WDIM == 3 && !reg3::in_hrmap_rule_or_subrule())
    return reg3::altdist(c->master);
  #endif
  if(!c->master->alt) return 0;
  #if CAP_IRR
  if(IRREGULAR) return irr::celldist(c, true);
  #endif
  if(ctof(c)) return c->master->alt->distance;
  if(reg3::in()) return c->master->alt->distance;
  #if CAP_GP
  if(GOLDBERG) return gp::compute_dist(c, celldistAlt);
  if(INVERSE) {
    cell *c1 = gp::get_mapped(c);
    return UIU(gp::compute_dist(c1, celldistAlt)) / 2;
    /* TODO */
    }
  #endif
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

/** direction upwards in the tree */
EX int updir(heptagon *h) {
  #if CAP_BT
  if(bt::in()) return bt::updir();
  #endif
  #if MAXMDIM >= 4
  if(WDIM == 3 && reg3::in_hrmap_rule_or_subrule()) {
    for(int i=0; i<h->type; i++) if(h->move(i) && h->move(i)->distance < h->distance) 
      return i;
    return -1;
    }
  #endif
  if(h->s == hsOrigin) return -1;
  return 0;
  }

/** direction upwards in the alt-tree */
EX int updir_alt(heptagon *h) {
  if(euclid || !h->alt) return -1;
  #if MAXMDIM >= 4
  if(WDIM == 3 && reg3::in_hrmap_rule_or_subrule()) {
    for(int i=0; i<S7; i++) if(h->move(i) && h->move(i)->alt && h->move(i)->alt->distance < h->alt->distance) 
      return i;
    return -1;
    }
  #endif
  return gmod(updir(h->alt) + altmap::relspin(h->alt), h->type);
  }


#if HDR
static const int RPV_MODULO = 5;
static const int RPV_RAND = 0;
static const int RPV_ZEBRA = 1;
static const int RPV_EMERALD = 2;
static const int RPV_PALACE = 3;
static const int RPV_CYCLE = 4;
#endif

// x mod 5 = pattern type
// x mod (powers of 2) = pattern type specific
// (x/5) mod 15 = picture for drawing floors
// x mod 7 = chance of pattern-specific pic
// whole = randomization

EX bool randpattern(cell *c, int rval) {
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

EX string describeRPM(eLand l) {
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

EX int randpatternCode(cell *c, int rval) {
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

#if HDR
#define RANDITER 31
#endif

char rpm_memoize[3][256][RANDITER+1];

EX void clearMemoRPM() {
  for(int a=0; a<3; a++) for(int b=0; b<256; b++) for(int i=0; i<RANDITER+1; i++)
    rpm_memoize[a][b][i] = 2;
  }

EX bool randpatternMajority(cell *c, int ival, int iterations) {
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

#define RVAL_MASK 0x10000000
#define DATA_MASK 0x20000000

cdata orig_cdata;

EX bool geometry_supports_cdata() {
  if(mhybrid) return PIU(geometry_supports_cdata());
  if(embedded_plane) return IPF( geometry_supports_cdata() );
  return among(geometry, gEuclid, gEuclidSquare, gNormal, gOctagon, g45, g46, g47, gBinaryTiling) || (arcm::in() && !sphere) || (currentmap && currentmap->strict_tree_rules());
  }

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

EX bool dmeq(int a, int b) { return (a&3) == (b&3); }

/* kept for compatibility: Racing etc. */
cdata *getHeptagonCdata_legacy(heptagon *h) {
  if(h->cdata) return h->cdata;

  if(sphere || quotient) h = currentmap->gamestart()->master;

  if(h == currentmap->getOrigin()) {
    h->cdata = new cdata(orig_cdata);
    for(int& v: h->cdata->val) v = 0;
    h->cdata->bits = reptilecheat ? (1 << 21) - 1 : 0;
    if(yendor::on && specialland == laVariant) h->cdata->bits |= (1 << 8) | (1 << 9) | (1 << 12);
    return h->cdata;
    }
  
  cdata mydata = *getHeptagonCdata_legacy(h->move(0));

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
    }

  return h->cdata = new cdata(mydata);
  }


cdata *getHeptagonCdata(heptagon *h) {
  if(mhybrid) return PIU ( getHeptagonCdata(h) );
  if(embedded_plane) return IPF( getHeptagonCdata(h) );
  if(geometry == gNormal && BITRUNCATED) return getHeptagonCdata_legacy(h);
  if(h->cdata) return h->cdata;

  if(sphere || quotient) h = currentmap->gamestart()->master;
  
  bool starting = h->s == hsOrigin;
  #if CAP_BT
  if(bt::in()) {
    if(bt::mapside(h) == 0) starting = true;
    for(int i=0; i<h->type; i++) if(bt::mapside(h->cmove(i)) == 0) starting = true;
    }
  #endif

  if(currentmap->strict_tree_rules()) starting = h->distance <= 0;

  if(starting) {
    h->cdata = new cdata(orig_cdata);
    for(int& v: h->cdata->val) v = 0;
    h->cdata->bits = reptilecheat ? (1 << 21) - 1 : 0;
    if(yendor::on && specialland == laVariant) h->cdata->bits |= (1 << 8) | (1 << 9) | (1 << 12);
    return h->cdata;
    }
  
  int dir = updir(h);
  
  cdata mydata = *getHeptagonCdata(h->cmove(dir));

  if(S3 >= OINF) {
    setHeptagonRval(h);
    affect(mydata, h->rval0, 1); 
    }
  else if(currentmap->strict_tree_rules()) {
    for(eLand ws: {NOWALLSEP, NOWALLSEP_SWAP}) {
      lalign(0, h->c7);
      int dir = 1;
      cellwalker hs(h->c7, dir, false);
      eLand dummy = laNone;
      vector<cell*> lpath, rpath;
      while(true) {
        int d = hs.at->master->distance;
        general_barrier_advance(hs, dir, dummy, dummy, ws, false);
        lpath.push_back(hs.at);
        if(hs.at->master->distance > d) break;
        }
      dir = -dir;
      while(true) {
        int d = hs.at->master->distance;
        general_barrier_advance(hs, dir, dummy, dummy, ws, false);
        rpath.push_back(hs.at);
        if(hs.at->master->distance > d) break;
        }
      setHeptagonRval(hs.at->master);
      affect(mydata, ws == NOWALLSEP_SWAP ? hs.at->master->rval1 : hs.at->master->rval0, 1);
      }
    }
  else if(S3 == 4) {
    heptspin hs(h, 0);
    while(dmeq((hs+1).cpeek()->dm4, (hs.at->dm4 - 1))) hs = hs + 1 + wstep + 1;
    while(dmeq((hs-1).cpeek()->dm4, (hs.at->dm4 - 1))) hs = hs - 1 + wstep - 1;
    setHeptagonRval(hs.at);
    affect(mydata, hs.at->rval0, 1); 
    }
  else for(int di: {0,1}) {
    heptspin hs(h, dir, false);
    hs -= di;
    while(true) {
      heptspin hs2 = hs + wstep + 1 + wstep - 1;
      if(dmeq(hs2.at->dm4, hs.at->dm4 + 1)) break;
      hs = hs2;
      }
    while(true) {
      heptspin hs2 = hs + 1 + wstep - 1 + wstep;
      if(dmeq(hs2.at->dm4, hs.at->dm4 + 1)) break;
      hs = hs2;
      }
    setHeptagonRval(hs.at);
    affect(mydata, hs.spin == dir ? hs.at->rval0 : hs.at->rval1, 1);
    }

  return h->cdata = new cdata(mydata);
  }

cdata *getEuclidCdata(gp::loc h) {

  int x = h.first, y = h.second;
  
  #if CAP_ARCM
  auto& data = arcm::in() ? arcm::get_cdata() : euc::get_cdata();
  #else
  auto& data = euc::get_cdata();
  #endif
    
  // hrmap_euclidean* euc = dynamic_cast<hrmap_euclidean*> (currentmap);
  if(data.count(h)) return &(data[h]);
  
  if(x == 0 && y == 0) {
    cdata xx;
    for(int i=0; i<4; i++) xx.val[i] = 0;
    xx.bits = 0;
    return &(data[h] = xx);
    }
  int ord = 1, bid = 0;
  while(!((x|y)&ord)) ord <<= 1, bid++;
  
  for(int k=0; k<3; k++) {
    int x1 = x + (k<2 ? ord : 0);
    int y1 = y - (k>0 ? ord : 0);
    if((x1&ord) || (y1&ord)) continue;
    int x2 = x - (k<2 ? ord : 0);
    int y2 = y + (k>0 ? ord : 0);

    cdata *d1 = getEuclidCdata({x1,y1});
    cdata *d2 = getEuclidCdata({x2,y2});
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
    
    return &(data[h] = xx);
    }
  
  // impossible!
  return NULL;
  }

int ld_to_int(ld x) {
  return int(x + 1000000.5) - 1000000;
  }

#if CAP_ARCM
EX gp::loc pseudocoords(cell *c) {
  transmatrix T = arcm::archimedean_gmatrix[c->master].second;
  return {ld_to_int(T[0][LDIM]), ld_to_int((spin(60._deg) * T)[0][LDIM])};
  }

EX cdata *arcmCdata(cell *c) {
  auto &agm = arcm::archimedean_gmatrix;
  if(!agm.count(c->master) || !agm[c->master].first) {
    forCellEx(c1, c) if(agm.count(c->master) && agm[c->master].first) return arcmCdata(c1);
    static cdata dummy;
    return &dummy;
    }
  heptagon *h2 = agm[c->master].first;
  dynamicval<eGeometry> g(geometry, gNormal); 
  dynamicval<hrmap*> cm(currentmap, arcm::current_altmap);  
  return getHeptagonCdata(h2);
  }
#endif

EX int getCdata(cell *c, int j) {
  if(fake::in()) return FPIU(getCdata(c, j));
  if(embedded_plane) return IPF(getCdata(c, j));
  if(experimental) return 0;
  if(mhybrid) { c = hybrid::get_where(c).first; return PIU(getBits(c)); }
  else if(INVERSE) {
    cell *c1 = gp::get_mapped(c);
    return UIU(getCdata(c1, j));
    }
  else if(euc::in()) return getEuclidCdata(euc2_coordinates(c))->val[j];
#if CAP_ARCM
  else if(arcm::in() && euclid)
    return getEuclidCdata(pseudocoords(c))->val[j];
  else if(arcm::in() && (hyperbolic || sl2)) 
    return arcmCdata(c)->val[j]*3;
#endif
  else if(!geometry_supports_cdata()) return 0;
  else if(ctof(c)) return getHeptagonCdata(c->master)->val[j]*3;
  else {
    int jj = 0;
    auto ar = gp::get_masters(c);
    for(int k=0; k<3; k++)
      jj += getHeptagonCdata(ar[k])->val[j];
    return jj;
    }
  }

EX int getBits(cell *c) {
  if(fake::in()) return FPIU(getBits(c));
  if(embedded_plane) return IPF(getBits(c));
  if(experimental) return 0;
  if(mhybrid) { c = hybrid::get_where(c).first; return PIU(getBits(c)); }
  else if(INVERSE) {
    cell *c1 = gp::get_mapped(c);
    return UIU(getBits(c1));
    }
  else if(euc::in()) return getEuclidCdata(euc2_coordinates(c))->bits;
  #if CAP_ARCM
  else if(arcm::in() && euclid)  
    return getEuclidCdata(pseudocoords(c))->bits;
  else if(arcm::in() && (hyperbolic || sl2)) 
    return arcmCdata(c)->bits;
  #endif
  else if(!geometry_supports_cdata()) return 0;
  else if(c == c->master->c7) return getHeptagonCdata(c->master)->bits;
  else {
    auto ar = gp::get_masters(c);
    int b0 = getHeptagonCdata(ar[0])->bits;
    int b1 = getHeptagonCdata(ar[1])->bits;
    int b2 = getHeptagonCdata(ar[2])->bits;
    return (b0 & b1) | (b1 & b2) | (b2 & b0);
    }
  }

EX cell *heptatdir(cell *c, int d) {
  if(d&1) {
    cell *c2 = createMov(c, d);
    int s = c->c.spin(d);
    s += 3; s %= 6;
    return createMov(c2, s);
    }
  else return createMov(c, d);
  }

EX int heptdistance(heptagon *h1, heptagon *h2) {
  // very rough distance
  int d = 0;
  #if CAP_CRYSTAL
  if(cryst) return crystal::space_distance(h1->c7, h2->c7);
  #endif
  #if CAP_SOLV
  if(sn::in()) return sn::approx_distance(h1, h2);
  #endif
  while(true) {
    if(h1 == h2) return d;
    for(int i=0; i<S7; i++) if(h1->move(i) == h2) return d + 1;
    int d1 = h1->distance, d2 = h2->distance;
    if(d1 >= d2) d++, h1 = createStep(h1, bt::updir());
    if(d2 >  d1) d++, h2 = createStep(h2, bt::updir());
    }
  }

EX int heptdistance(cell *c1, cell *c2) {
  #if CAP_CRYSTAL
  if(cryst) return crystal::space_distance(c1, c2);
  #endif
  if(!hyperbolic || quotient || WDIM == 3) return celldistance(c1, c2);
  else return heptdistance(c1->master, c2->master);
  }

map<pair<cell*, cell*>, int> saved_distances;

EX set<cell*> keep_distances_from;

set<cell*> dists_computed;

int perma_distances;

EX void compute_saved_distances(cell *c1, int max_range, int climit) {
    
  celllister cl(c1, max_range, climit, NULL);

  for(int i=0; i<isize(cl.lst); i++)
    saved_distances[make_pair(c1, cl.lst[i])] = cl.dists[i];
  }

EX void permanent_long_distances(cell *c1) {
  keep_distances_from.insert(c1);
  if(racing::on)
    compute_saved_distances(c1, 300, 1000000);
  else
    compute_saved_distances(c1, 120, 200000);
  }

EX void erase_saved_distances() {
  saved_distances.clear(); dists_computed.clear();
  
  for(auto c: keep_distances_from) compute_saved_distances(c, 120, 200000);
  perma_distances = isize(saved_distances);
  }

EX int max_saved_distance(cell *c) {
  int maxsd = 0;
  for(auto& p: saved_distances) if(p.first.first == c) maxsd = max(maxsd, p.second);
  return maxsd;
  }

EX cell *random_in_distance(cell *c, int d) {
  vector<cell*> choices;
  for(auto& p: saved_distances) if(p.first.first == c && p.second == d) choices.push_back(p.first.second);
  println(hlog, "choices = ", isize(choices));
  if(choices.empty()) return NULL;
  return choices[hrand(isize(choices))];
  }

EX int bounded_celldistance(cell *c1, cell *c2) {
  int limit = 14400;
  #if CAP_SOLV
  if(geometry == gArnoldCat) { 
    c2 = asonov::get_at(asonov::get_coord(c2->master) - asonov::get_coord(c1->master))->c7;
    c1 = currentmap->gamestart(); 
    limit = 100000000;
    }
  #endif

  if(saved_distances.count(make_pair(c1,c2)))
    return saved_distances[make_pair(c1,c2)];

  celllister cl(c1, 100, limit, NULL);
  for(int i=0; i<isize(cl.lst); i++)
    saved_distances[make_pair(c1, cl.lst[i])] = cl.dists[i];

  if(saved_distances.count(make_pair(c1,c2)))
    return saved_distances[make_pair(c1,c2)];

  return DISTANCE_UNKNOWN;
  }

EX int clueless_celldistance(cell *c1, cell *c2) {
  if(saved_distances.count(make_pair(c1,c2)))
    return saved_distances[make_pair(c1,c2)];
  
  if(dists_computed.count(c1)) return DISTANCE_UNKNOWN;
    
  if(isize(saved_distances) > perma_distances + 1000000) erase_saved_distances();
  compute_saved_distances(c1, 64, 1000);
  
 dists_computed.insert(c1);

  if(saved_distances.count(make_pair(c1,c2)))
    return saved_distances[make_pair(c1,c2)];
    
  return DISTANCE_UNKNOWN;
  }

EX int celldistance(cell *c1, cell *c2) {

  if(embedded_plane) return IPF(celldistance(c1, c2));

  if(fake::in()) return FPIU(celldistance(c1, c2));

  if(mhybrid) return hybrid::celldistance(c1, c2);
  
  #if CAP_FIELD
  if(geometry == gFieldQuotient && (PURE || BITRUNCATED)) {
    int d = fieldpattern::field_celldistance(c1, c2);
    if(d != DISTANCE_UNKNOWN) return d;
    }
  #endif
  
  if(closed_manifold) return bounded_celldistance(c1, c2);
  
  #if CAP_CRYSTAL
  if(cryst) return crystal::precise_distance(c1, c2);
  #endif
  
  if(euc::in() && WDIM == 2) {
    return euc::cyldist(euc2_coordinates(c1), euc2_coordinates(c2));
    }

  if(arcm::in() || quotient || sn::in() || (aperiodic && euclid) || experimental || sl2 || nil || arb::in()) 
    return clueless_celldistance(c1, c2);
   
   if(S3 >= OINF) return inforder::celldistance(c1, c2);

  #if CAP_BT && MAXMDIM >= 4
  if(bt::in() && WDIM == 3) 
    return bt::celldistance3(c1, c2);
  #endif
  
  #if MAXMDIM >= 4
  if(euc::in()) 
    return euc::celldistance(c1, c2);
  
  if(hyperbolic && WDIM == 3) return reg3::celldistance(c1, c2);
  #endif

  if(INVERSE) {
    c1 = gp::get_mapped(c1);
    c2 = gp::get_mapped(c2);
    return UIU(celldistance(c1, c2)) / 2;
    /* TODO */
    }

  if(euclid) return clueless_celldistance(c1, c2);

  return hyperbolic_celldistance(c1, c2);
  }

EX vector<cell*> build_shortest_path(cell *c1, cell *c2) {
  #if CAP_CRYSTAL
  if(cryst) return crystal::build_shortest_path(c1, c2);
  #endif
  vector<cell*> p;
  if(euclid) {
    p.push_back(c1);
    hyperpoint h = tC0(calc_relative_matrix(c2, c1, C0));
    cell *x = c1;
    transmatrix T1 = rspintox(h);
    int d = celldistance(c1, c2);
    int steps = d * 10;
    ld step = hdist0(h) / steps;
    for(int i=0; i< steps; i++) {
      T1 = T1 * xpush(step);
      virtualRebase(x, T1);
      println(hlog, "x = ", x, "p length = ", isize(p), " dist = ", hdist0(tC0(T1)), " dist from end = ", hdist(tC0(T1), tC0(calc_relative_matrix(c2, x, C0))));
      while(x != p.back()) {
        forCellCM(c, p.back()) 
          if(celldistance(x, c) < celldistance(x, p.back())) {
            p.push_back(c);
            break;
            }
        }
      }
    if(isize(p) != d + 1)
      println(hlog, "warning: path size ", isize(p), " should be ", d+1);
    }
  else if(c2 == currentmap->gamestart()) {
    while(c1 != c2) {
      p.push_back(c1);
      forCellCM(c, c1) if(celldist(c) < celldist(c1)) { c1 = c; goto next1; }
      throw hr_shortest_path_exception();
      next1: ;
      }
    p.push_back(c1);
    }
  else if(c1 == currentmap->gamestart()) {
    p = build_shortest_path(c2, c1);
    reverse(p.begin(), p.end());
    }
  else {
    while(c1 != c2) {
      p.push_back(c1);
      forCellCM(c, c1) if(celldistance(c, c2) < celldistance(c1, c2)) { c1 = c; goto next; }
      throw hr_shortest_path_exception();
      next: ;
      }
    p.push_back(c1);
    }
  return p;
  }

EX void clearCellMemory() {
  #if MAXMDIM >= 4 && CAP_RAY
  if(intra::in) {
    intra::erase_all_maps();
    return;
    }
  #endif
  for(int i=0; i<isize(allmaps); i++) 
    if(allmaps[i])
      delete allmaps[i];
  allmaps.clear();
  currentmap = nullptr;
  last_cleared = NULL;
  saved_distances.clear();
  dists_computed.clear();
  keep_distances_from.clear(); perma_distances = 0;
  pd_from = NULL;
  gp::gp_adj.clear();
  }

auto cellhooks = addHook(hooks_clearmemory, 500, clearCellMemory);

EX bool isNeighbor(cell *c1, cell *c2) {
  for(int i=0; i<c1->type; i++) if(c1->move(i) == c2) return true;
  return false;
  }

EX bool isNeighborCM(cell *c1, cell *c2) {
  for(int i=0; i<c1->type; i++) if(createMov(c1, i) == c2) return true;
  return false;
  }

EX int neighborId(cell *ofWhat, cell *whichOne) {
  for(int i=0; i<ofWhat->type; i++) if(ofWhat->move(i) == whichOne) return i;
  return -1;
  }

EX int mine_adjacency_rule = 0;

#if HDR
struct adj_data {
  cell *c;
  bool mirrored;
  transmatrix T;
  };
#endif

EX array<map<cell*, vector<adj_data>>, 2> adj_memo;

EX bool geometry_has_alt_mine_rule() {
  if(S3 >= OINF) return false;
  if(aperiodic) return true;
  if(WDIM == 2) return valence() > 3;
  if(WDIM == 3) return !among(geometry, gHoroHex, gCell5, gBitrunc3, gCell8, gECell8, gCell120, gECell120);
  return true;
  }

EX vector<adj_data> adj_minefield_cells_full(cell *c) {
  auto& am = adj_memo[mine_adjacency_rule];
  if(am.count(c)) return am[c];
  if(isize(am) > 10000) am.clear();
  auto& res = am[c];
  if(mine_adjacency_rule == 0 || !geometry_has_alt_mine_rule()) {
    forCellIdCM(c2, i, c) res.emplace_back(adj_data{c2, c->c.mirror(i), currentmap->adj(c, i)});
    }
  else if(WDIM == 2) {
    cellwalker cw(c, 0);
    transmatrix T = Id;
    T = T * currentmap->adj(c, 0);
    cw += wstep;
    cw++;
    cellwalker cw1 = cw;
    do {
      res.emplace_back(adj_data{cw.at, cw.mirrored, T});
      T = T * currentmap->adj(cw.at, cw.spin);
      cw += wstep;
      cw++;
      if(cw.cpeek() == c) cw++;
      }
    while(cw != cw1);
    }
  else {
    auto& ss = currentmap->get_cellshape(c);
    const vector<hyperpoint>& vertices = ss.vertices_only_local;
    manual_celllister cl;
    cl.add(c);
    vector<transmatrix> M = {Id};
    for(int i=0; i<isize(cl.lst); i++) {
      cell *c1 = cl.lst[i];
      bool shares = false;
      transmatrix T = M[i];
      if(c != c1) {
        auto& ss1 = currentmap->get_cellshape(c1);
        auto& vertices1 = ss1.vertices_only_local;
        for(hyperpoint h: vertices) for(hyperpoint h2: vertices1)
          if(hdist(h, T * h2) < 1e-6) shares = true;
        if(shares) res.emplace_back(adj_data{c1, det(T) < 0, T});
        }
      if(shares || c == c1) forCellIdEx(c2, i, c1) {
        if(cl.listed(c2)) continue;
        cl.add(c2);
        M.push_back(T * currentmap->adj(c1, i));
        }
      }
    }
  return res;
  }

EX vector<cell*> adj_minefield_cells(cell *c) {
  vector<cell*> res;
  auto ori = adj_minefield_cells_full(c);
  for(auto p: ori) res.push_back(p.c);
  if(hat::in()) {
    // reduce repetitions
    sort(res.begin(), res.end());
    res.erase(std::unique(res.begin(), res.end()), res.end());
    }
  return res;
  }

EX vector<int> reverse_directions(cell *c, int dir) {
  if(PURE && !(aperiodic && WDIM == 2)) return reverse_directions(c->master, dir);
  int d = c->degree();
  if(d & 1)
    return { gmod(dir + c->type/2, c->type), gmod(dir + (c->type+1)/2, c->type) };
  else
    return { gmod(dir + c->type/2, c->type) };
  }

EX vector<int> reverse_directions(heptagon *c, int dir) { 
  int d = c->degree();
  switch(geometry) {
    case gBinary3:
      if(dir < 4) return {8};
      else if(dir >= 8) return {0, 1, 2, 3};
      else return {dir ^ 1};
    
    case gHoroTris:
      if(dir < 4) return {7};
      else if(dir == 4) return {5, 6};
      else if(dir == 5) return {6, 4};
      else if(dir == 6) return {4, 5};
      else return {0, 1, 2, 3};
    
    case gHoroRec:
      if(dir < 2) return {6};
      else if(dir == 6) return {0, 1};
      else return {dir^1};
      
    case gKiteDart3: {
      if(dir < 4) return {dir ^ 2};
      if(dir >= 6) return {4, 5};
      vector<int> res;
      for(int i=6; i<c->type; i++) res.push_back(i);
      return res;
      }
    
    case gHoroHex: {
      if(dir < 6) return {12, 13};
      if(dir >= 12) return {0, 1, 2, 3, 4, 5};
      const int dt[] = {0,0,0,0,0,0,10,11,9,8,6,7,0,0};
      return {dt[dir]};
      }
    
    default:
      if(d & 1)
        return { gmod(dir + c->type/2, c->type), gmod(dir + (c->type+1)/2, c->type) };
      else
        return { gmod(dir + c->type/2, c->type) };
    }
  }

EX bool standard_tiling() {
  return !arcm::in() && !aperiodic && !bt::in() && !arb::in() && (WDIM == 2 || !nonisotropic) && !mhybrid;
  }

EX int valence() {
  if(BITRUNCATED || IRREGULAR) return 3;
  if(INVERSE) return WARPED ? 4 : max(S3, S7);
  #if CAP_ARCM
  if(arcm::in()) return arcm::valence();
  #endif
  if(arb::in()) return arb::current.min_valence;
  return S3;
  }

/** portalspaces are not defined outside of a boundary */
EX bool is_boundary(cell *c) {
  if(c == &out_of_bounds) return true;
  return ((cgflags & qPORTALSPACE) || intra::in) && isWall(c->wall);
  }

/** compute the distlimit for a tessellation automatically */
EX int auto_compute_range(cell *c) {  
  if(sphere) {
    cgi.base_distlimit = SEE_ALL;
    return SEE_ALL;
    }
  cgi.base_distlimit = 0;
  const int expected_count = 400;
  celllister cl(c, 1000, expected_count, NULL);
  int z = isize(cl.dists);
  int d = cl.dists.back();
  while(cl.dists[z-1] == d) z--;
  if(true) { // if(cgflags & DF_GEOM) {
    println(hlog, "last distance = ", cl.dists.back());
    println(hlog, "ball size = ", isize(cl.dists));
    println(hlog, "previous ball size = ", z);
    }
  if(isize(cl.dists) * z > expected_count * expected_count) d--;
  return ginf[geometry].distlimit[0] = cgi.base_distlimit = d;
  }

EX cell out_of_bounds;
EX heptagon oob;

}
