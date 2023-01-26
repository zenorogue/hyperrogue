// Hyperbolic Rogue -- Irregular (Voronoi) tilings
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file irregular.cpp
 *  \brief Irregular (Voronoi) tilings
 */

#include "hyper.h"
namespace hr { 

EX namespace irr {

EX int irrid;

#if CAP_IRR
EX ld density = 2;
EX ld quality = .2;
EX int place_attempts = 10;
EX int rearrange_max_attempts = 50;
EX int rearrange_less = 10;

EX int cellcount;

#if HDR
struct cellinfo {
  cell *owner;
  map<cell*, transmatrix> relmatrices;
  vector<hyperpoint> jpoints;
  hyperpoint p;
  transmatrix pusher, rpusher;
  vector<int> neid;
  vector<int> spin;
  vector<hyperpoint> vertices;
  int localindex;
  bool is_pseudohept;
  int patterndir;
  int generation;
  };
#endif

EX map<cell*, int> cellindex;

EX vector<cellinfo> cells;

EX map<heptagon*, vector<int> > cells_of_heptagon;

int runlevel;
vector<ld> edgelens, distlens;

void make_cells_of_heptagon() {
  cells_of_heptagon.clear();
  for(int i=0; i<isize(cells); i++) {
    auto &p1 = cells[i];
    auto &vc = cells_of_heptagon[p1.owner->master];
    p1.localindex = isize(vc);
    vc.push_back(i);
    }
  }
  
string status[5];
  
EX hrmap *base;

EX euc::torus_config_full base_config;

bool gridmaking;

int rearrange_index;

bool cell_sorting;

EX int bitruncations_requested = 1;
EX int bitruncations_performed = 0;

int black_adjacent, white_three;

void set_relmatrices(cellinfo& ci) {
  auto& all = base->allcells();
  ci.relmatrices.clear();
  for(auto c0: all) ci.relmatrices[c0] = calc_relative_matrix(c0, ci.owner, ci.p);
  }

void rebase(cellinfo& ci) {
  cell *cx = ci.owner;
  virtualRebase(ci.owner, ci.p);
  if(ci.owner != cx) {
    printf("rebased %p to %p\n", hr::voidp(cx), hr::voidp(ci.owner));
    set_relmatrices(ci);
    }
  }

void compute_jpoints() {
  for(int i=0; i<isize(cells); i++) {
    auto &ci = cells[i];

    ci.pusher = rgpushxto0(ci.p);
    ci.rpusher = gpushxto0(ci.p);
    
    ci.jpoints.clear();

    for(int j=0; j<isize(cells); j++) {
      auto &cj = cells[j];
      ci.jpoints.push_back(ci.rpusher * ci.relmatrices[cj.owner] * cj.p);
      }
    }
  }
    
void bitruncate() {
  int cc = isize(cells);
  map<pair<int, int>, int> bitruncated_id;
  for(int i=0; i<cc; i++) {
    int v = isize(cells[i].vertices);
    for(int j=0; j<v; j++) {
      int last = cells[i].neid[(j+v-1)%v];
      int next = cells[i].neid[j];
      if(!bitruncated_id.count(make_pair(i, last))) {
        bitruncated_id[make_pair(i, last)] = 
        bitruncated_id[make_pair(last, next)] = 
        bitruncated_id[make_pair(next, i)] =                 
          isize(cells);
        cells.emplace_back();
        cellinfo& s = cells.back();
        s.patterndir = -1;
        s.owner = cells[i].owner;
        s.p = cells[i].pusher * cells[i].vertices[j];
        s.neid.push_back(i);
        s.neid.push_back(-1);
        s.neid.push_back(last);
        s.neid.push_back(-1);
        s.neid.push_back(next);
        s.neid.push_back(-1);
        s.generation = bitruncations_performed + 1;
        virtualRebase(s.owner, s.p);
        set_relmatrices(s);
        }
      }
    }
  for(int i=0; i<cc; i++) {
    int v = isize(cells[i].vertices);
    vector<int> newnei;
    for(int j=0; j<v; j++) {
      int last = cells[i].neid[(j+v-1)%v];
      int next = cells[i].neid[j];
      auto id = bitruncated_id[make_pair(i, last)];
      newnei.push_back(id);
      for(int k=0; k<6; k++) 
        if(cells[id].neid[k] == i) {
          cells[id].neid[(k+5)%6] = bitruncated_id[make_pair(i, next)];
          }
      }
    cells[i].neid = std::move(newnei);
    }
  make_cells_of_heptagon();
  compute_jpoints();
  for(int i=0; i<isize(cells); i++) {
    auto &ci = cells[i];
    ci.vertices.clear();

    ci.pusher = rgpushxto0(ci.p);
    ci.rpusher = gpushxto0(ci.p);

    int v = isize(ci.neid);
    for(int j=0; j<v; j++) {
      int last = ci.neid[(j+v-1)%v];
      int next = ci.neid[j];
      hyperpoint h1 = ci.rpusher * ci.relmatrices[cells[last].owner] * cells[last].p;
      hyperpoint h2 = ci.rpusher * ci.relmatrices[cells[next].owner] * cells[next].p;
      ci.vertices.push_back(mid3(C0, h1, h2));
      }
    }
  bitruncations_performed++;
  cell_sorting = false;
  }

int rearrange(bool total, ld minedge) {
  int tooshort = 0;
  for(int i=0; i<isize(cells); i++) {
    auto& p1 = cells[i];
    hyperpoint h = Hypc;
    for(auto v: p1.vertices) h = h + v;
    
    bool changed = total;

    for(int j=0; j<isize(p1.vertices); j++)
      if(hdist(p1.vertices[j], p1.vertices[(j+1) % isize(p1.vertices)]) < minedge) {
        tooshort++; changed = true;
        h = h + p1.vertices[j] + p1.vertices[(j+1) % isize(p1.vertices)];
        }
    if(changed)
      cells[i].p = p1.pusher * normalize(h);
    }
  return tooshort;
  }

bool step(int delta) {

  if(!gridmaking) return false;
  timetowait = 0;

  auto& all = base->allcells();

  auto t = SDL_GetTicks();
  while(SDL_GetTicks() < t + 250)
  switch(runlevel) {
    case 0: {

     cells.clear();
     cells_of_heptagon.clear();
     cellindex.clear();
     
     if(0) if(cellcount <= isize(all) * 2) {
       for(auto h: all) {
         cells.emplace_back();
         cellinfo& s = cells.back();
         s.patterndir = -1;
         s.owner = h, s.p = xspinpush0(hrand(1000), .01);
         s.generation = 0;
         set_relmatrices(s);
         }
       }
      runlevel++;
      break;
      }
     
    case 1: {
      while(isize(cells) < cellcount) {
        if(SDL_GetTicks() > t + 250) { make_cells_of_heptagon(); status[0] = its(isize(cells)) + " cells"; return false; }
        cells.emplace_back();
        cellinfo& s = cells.back();
        s.patterndir = -1;
        ld bestval = 0;
        for(int j=0; j<place_attempts; j++) {
          int k = hrand(isize(all));
          cell *c = all[k];
          map<cell*, transmatrix> relmatrices;
          hyperpoint h = randomPointIn(c->type);
          for(auto c0: all) relmatrices[c0] = calc_relative_matrix(c0, c, h);
          ld mindist = 1e6;
          for(auto p: cells) {
            if(!relmatrices.count(p.owner)) continue;
            ld val = hdist(h, relmatrices[p.owner] * p.p);
            if(val < mindist) mindist = val;
            }
          if(mindist > bestval) bestval = mindist, s.owner = c, s.p = h, s.relmatrices = std::move(relmatrices);
          }
        }
      make_cells_of_heptagon();
      cell_sorting = true; bitruncations_performed = 0;
      runlevel++;
      status[0] = "all " + its(isize(cells)) + " cells";
      break;
      }
    
    case 2: {

      if(cell_sorting)
        sort(cells.begin(), cells.end(), [] (const cellinfo &s1, const cellinfo &s2) { return hdist0(s1.p) < hdist0(s2.p); });
      make_cells_of_heptagon();

      edgelens.clear();
      distlens.clear();
  
      int stats[16];
      for(int k=0; k<16; k++) stats[k] = 0;
      
      compute_jpoints();
      
      for(int i=0; i<isize(cells); i++) {
        auto &p1 = cells[i];
    
        p1.vertices.clear();
        p1.neid.clear();
    
        int j = 0;
        if(j == i) j = 1;
    
        for(int k=0; k<isize(cells); k++) if(k != i) {
          if(hdist(p1.jpoints[k], C0) < hdist(p1.jpoints[j], C0))
            j = k;
          }
          
        hyperpoint t = mid(p1.jpoints[j], C0);
              // p1.vertices.push_back(p1.pusher * t);
        int j0 = j;
        int oldj = j;
        do {
          int best_k = -1;
          hyperpoint best_h;
          for(int k=0; k<isize(cells); k++) if(k != i && k != j && k != oldj) {
            hyperpoint h = circumscribe(C0, p1.jpoints[j], p1.jpoints[k]);
            if(h[LDIM] < 0) continue;
            if(!clockwise(t, h)) continue;
            if(best_k == -1)
              best_k = k, best_h = h;
            else if(clockwise(h, best_h))
              best_k = k, best_h = h;
            }
          p1.vertices.push_back(best_h);
          p1.neid.push_back(best_k);
          distlens.push_back(hdist0(best_h));
          oldj = j, j = best_k, t = best_h;
          if(j == -1) break;
          if(isize(p1.vertices) == 15) break;
          }
        while(j != j0);
        
        for(int j=0; j<isize(p1.vertices); j++)
          edgelens.push_back(hdist(p1.vertices[j], p1.vertices[(j+1) % isize(p1.vertices)]));
    
        stats[isize(p1.vertices)]++;
        }
    
      for(int a=0; a<16; a++) printf("%3d ", stats[a]);
      if(isize(edgelens)) {
        printf("|");
        printf("%4d ", isize(edgelens));
        sort(edgelens.begin(), edgelens.end());
        for(int a=0; a<=8; a++) printf("%6.3lf", double(edgelens[(a * isize(edgelens) - 1) / 8]));
        printf(" | ");
        sort(distlens.begin(), distlens.end());
        for(int a=0; a<=8; a++) printf("%5.2lf", double(distlens[(a * isize(edgelens) - 1) / 8]));
        }
      printf("\n");
      
      runlevel++;
      break;
      }
    
    case 3: {
    
      int errors = 0, toobig = 0;
  
      for(int i=0; i<isize(cells); i++) {
        int v = isize(cells[i].vertices);
        if(v > 8 || v< 3) {
          if(v < 3 || v >= 15)
            errors++;
          else toobig++;
          cells[i] = cells.back();
          i--; cells.pop_back();
          }
        }
      
      if(errors > 0) status[1] = XLAT("bad cells: %1", its(errors)); else status[1] = " ";
      if(toobig > 0) status[2] = XLAT("too many edges: %1", its(toobig)); else status[2] = " ";
      if(isize(cells) < cellcount*3/4) runlevel = 0;
      else if(isize(cells) < cellcount) runlevel = 1;
      else { rearrange_index = 0; runlevel++; }
      break;
      }
    
    case 4: {
  
      ld median = edgelens[isize(edgelens) / 2];
      ld minedge = median * quality;
      status[3] = XLAT("median edge: %1 minimum: %2", fts(median), fts(edgelens[0]));
      if(!bitruncations_performed && edgelens[0] < minedge) {
        if(rearrange_index >= rearrange_max_attempts) {
          runlevel = 0; break;
          }
        int tooshort = rearrange(rearrange_index < rearrange_less, minedge);
        
        status[3] += XLAT(" (edges too short: %1)", its(tooshort));
        runlevel = 2;
        rearrange_index++;
        break;
        }
      runlevel++;
      break;
      }
      
    case 5: {
      if(bitruncations_performed < bitruncations_requested) 
        bitruncate();
      else
        runlevel = 6;
      break;
      }
    
    case 6: {
      
      int notfound = 0;
    
      for(int i=0; i<isize(cells); i++) {
        auto &p1 = cells[i];
        int N = isize(p1.vertices);
        p1.spin.resize(N);
        for(int j=0; j<N; j++) {
          auto i1 = p1.neid[j];
          if(i1 < 0 || i1 >= isize(cells)) {
            runlevel = 0;
            return false;
            }
          bool found = false;          
          for(int k=0; k < isize(cells[i1].vertices); k++)
            if(cells[i1].neid[k] == i)
              found = true, p1.spin[j] = k;
          if(!found) notfound++;
          }
        }
      
      if(notfound) { status[4] = XLAT("cells badly paired: %1", its(notfound)); runlevel = 0; break; }
      
      int heptas = 0;
      for(auto p: cells_of_heptagon) {
        printf("%p: %d\n", hr::voidp(p.first), isize(p.second));
        heptas++;
        }
      
      if(heptas != isize(all)) {
        status[4] = XLAT("cells not covered: %1", its(isize(all) - heptas));
        printf("heptas = %d\n", heptas);
        runlevel = 0; break;
        }
    
      int faredge = 0;
      for(int i=0; i<isize(cells); i++) {
        auto &p1 = cells[i];
        for(int j: p1.neid) {
          auto &p2 = cells[j];
          bool ok = p1.owner == p2.owner || isNeighbor(p1.owner, p2.owner);
          if(!ok) faredge++;
          }
        }

      if(faredge) {
        status[4] = XLAT("adjacent cells from nonadjacent heptagons: %1", its(faredge));
        runlevel = 0; return false;
        }
      
      /*
      black_adjacent = 0;
      white_three = 0;
      for(int i=0; i<isize(cells); i++) {
        if(!cells[i].by_bitruncation) {
          for(int j: cells[i].neid) if(!cells[j].by_bitruncation) black_adjacent++;
          }
        else {
          int v = isize(cells[i].neid);
          for(int j=0; j<v; j++)
            if(cells[cells[i].neid[j]].by_bitruncation)
            if(cells[cells[i].neid[(j+1)%v]].by_bitruncation)
              white_three++;
          }
        }
      printf("black_adjacent = %d, white_three = %d\n", black_adjacent, white_three);
      */

      status[4] = XLAT("OK");
      runlevel = 10;
      
      for(auto& s: cells) s.is_pseudohept = false;
      for(auto& s: cells) {
        s.is_pseudohept = true;
        for(int i: s.neid) if(cells[i].is_pseudohept) s.is_pseudohept = false;
        }

      for(auto& s: cells) {
        int d = -1;
        ld dist = cgi.hcrossf / 2;
        ld dists[8];
        for(int i=0; i<S7; i++) {
          dists[i] = hdist(s.p, xspinpush0(cgi.hexshift - i * TAU / S7, -cgi.hcrossf));
          if(dists[i] < dist)
            d = i, dist = dists[i];
          }
        if(d != -1 && dists[(d+1) % S7] > dists[(d+S7-1) % S7])
          d = (d + S7 - 1) % S7;
        s.patterndir = d;
        }

      break;
      }
    
    case 10:
      return false;
    }

  return false;
  }

EX void compute_geometry() {
  if(IRREGULAR) {
    ld scale = sqrt(isize(cells_of_heptagon) * 1. / isize(cells));
    cgi.crossf *= scale;
    cgi.hepvdist *= scale;
    cgi.rhexf *= scale;
    cgi.hexhexdist *= scale;
    cgi.hexvdist *= scale;
    cgi.base_distlimit = (cgi.base_distlimit + log(scale) / log(2.618)) / scale;
    if(cgi.base_distlimit > 25) cgi.base_distlimit = 25;
    }
  }

bool draw_cell_schematics(cell *c, const shiftmatrix& V) {
  if(gridmaking) {
    heptagon *h = c->master;
    for(int i: cells_of_heptagon[h]) {
      auto& p = cells[i];
      if(p.owner == c) {
        queuestr(V * rgpushxto0(p.p), .1, its(i), isize(p.vertices) > 8 ? 0xFF0000 : 0xFFFFFF);
        int N = isize(p.vertices);
        for(int j=0; j<N; j++)
          gridline(V, p.pusher * p.vertices[j], p.pusher * p.vertices[(1+j)%N], 0xFFFFFFFF, 0);

        gridline(V, p.p, C0, 0xFF0000FF, 0);
        if(p.patterndir != -1)
          gridline(V, p.p, calc_relative_matrix(c->master->move(p.patterndir)->c7, c, p.p) * C0, 0x00FF00FF, 0);
        }
      }
    }
  return false;
  }

#if HDR
struct heptinfo {
  heptspin base;
  vector<cell*> subcells;
  vector<int> celldists[2];
  };
#endif

EX map<heptagon*, heptinfo> periodmap;

EX void link_to_base(heptagon *h, heptspin base) {
  // printf("linking %p to %p/%d\n", hr::voidp(h), hr::voidp(base.at), base.spin);
  auto &hi = periodmap[h];
  hi.base = base;
  for(int k: cells_of_heptagon[base.at]) {
    cell *c = newCell(isize(cells[k].vertices), h);
    hi.subcells.push_back(c);
    cellindex[c] = k;
    }
  h->c7 = hi.subcells[0];
  }

EX void clear_links(heptagon *h) {
  auto& hi = periodmap[h];
  for(cell *c: hi.subcells) {
    for(int i=0; i<c->type; i++) if(c->move(i)) c->move(i)->move(c->c.spin(i)) = NULL;
    cellindex.erase(c);
    delete c;
    }
  h->c7 = NULL;
  periodmap.erase(h);
  }

EX void link_start(heptagon *h) {
  link_to_base(h, heptspin(cells[0].owner->master, 0));
  }

EX void link_next(heptagon *parent, int d) {
  if(!periodmap.count(parent))
    link_to_base(parent, heptspin(cells[0].owner->master, 0));
  // printf("linking next: %p direction %d [s%d]\n", hr::voidp(parent), d, parent->c.spin(d));
  auto *h = parent->move(d);
  heptspin hs = periodmap[parent].base + d + wstep - parent->c.spin(d);
  link_to_base(h, hs);
  }

EX void may_link_next(heptagon *parent, int d) {
  if(!periodmap.count(parent->move(d)))
    link_next(parent, d);
  }


EX void link_cell(cell *c, int d) {
  // printf("linking cell: %p direction %d\n", hr::voidp(c), d);
  int ci = cellindex[c];
  auto& sc = cells[ci];
  int ci2 = sc.neid[d];
  auto& sc2 = cells[ci2];
  
  heptagon *master2 = NULL;

  if(sc2.owner == sc.owner) {
    master2 = c->master;
    // printf("local\n");
    }
  else {
    int dirs = 0;
    int os = periodmap[c->master].base.spin;
    for(int d=0; d<S7; d++) if(sc2.owner->master == sc.owner->master->modmove(os+d)) {
      heptspin hss(c->master, d);
      hss += wstep;
      master2 = hss.at;
      // printf("master2 is %p; base = %p; should be = %p\n", hr::voidp(master2), hr::voidp(periodmap[master2].base.at), hr::voidp(sc2.owner->master));
      dirs++;
      }
    if(dirs != 1) { printf("dirs error\n"); exit(1); }
    }
  
  cell *c2 = periodmap[master2].subcells[sc2.localindex];
  c->c.connect(d, c2, sc.spin[d], false);
  }

int hdist(heptagon *h1, heptagon *h2) {
  if(h1 == h2) return 0;
  for(int i=0; i<S7; i++) if(h1->move(i) == h2) return 1;
  return 2;
  }

// compute celldist or celldistalt for all the subcells of h.
// We use the following algorithm:
// - assume that everything is computed for all the adjacent heptagons of h which are closer to the origin
// - consider h and its two neighbors which are in the same distance to the origin ('siblings')
// - compute celldists for all the cells in these three heptagons, by bfs, based on the 'parent' heptagons adjacent to h
// - record the computed distances for h, but not for its siblings

static const int NODISTANCE = 2000000000;

map<heptagon*, heptagon*> last_on_horocycle;

void compute_horocycle(heptagon *);

void compute_distances(heptagon *h, bool alts) {
  /* if(alts) printf("[%p] compute_distances %p\n", hr::voidp(h->alt->alt), hr::voidp(h));
  printf("neighbors:"); for(int i=0; i<S7; i++) printf(" %p", createStep(h, i)); printf("\n"); */
  
  if(alts) {
    if(!last_on_horocycle[h->alt->alt])
      last_on_horocycle[h->alt->alt] = h;
    
    if(h->alt->alt->s != hsOrigin)
    while(h->alt->distance <= last_on_horocycle[h->alt->alt]->alt->distance)
      compute_horocycle(h->alt->alt);
    }

  auto dm4 = [alts, h] (heptagon *h1) -> unsigned {
    if(!alts) return h1->dm4;
    if(alts && !h1->alt) return 100; // error
    if(alts && h1->alt->alt != h->alt->alt) return 100; // error
    return h1->alt->dm4;
    };
  unsigned cdm = dm4(h), pdm = (cdm-1)&3;
  vector<heptagon*> hs;
  hs.push_back(h);
  for(int i=0; i<S7; i++) if(dm4(createStep(h, i)) == cdm)
    hs.push_back(h->move(i));
  
  vector<vector<int>*> to_clear;

  for(auto hx: hs) {  
    auto &hi = periodmap[hx];
    int ct = isize(hi.subcells);
    auto& cd = hi.celldists[alts];
    if(cd.empty() && hx != h) to_clear.push_back(&cd);
    cd.resize(ct, NODISTANCE);
    if(h == hx && (alts ? h->alt->s == hsOrigin : h->s == hsOrigin))
      cd[0] = 0;
    }
  while(true) {
    bool changed = false;
    for(auto hx: hs) {
      auto& hi = periodmap[hx];
      auto& cd = hi.celldists[alts];
      for(int i=0; i<isize(hi.subcells); i++)
        forCellCM(c2, hi.subcells[i]) 
          if(among(dm4(c2->master), cdm, pdm) && hdist(h, c2->master) < 2) {
            int d = irr::celldist(c2, alts) + 1;
            if(d < cd[i]) cd[i] = d, changed = true;
            }
      }
    if(!changed) break;
    }

  /* for(auto hx: hs) {
    auto& hi = periodmap[hx];
    auto& cd = hi.celldists[alts];
    // for(int i: cd) if(i == NODISTANCE) printf("distances not computed\n");
    } */

  for(auto x: to_clear) x->clear();

  // for(int i: cd) printf(" %d", i); printf("\n");
  }

void erase_alt(heptagon *alt) { 
  last_on_horocycle.erase(alt);
  }

void compute_horocycle(heptagon *alt) {
  heptagon *master = last_on_horocycle[alt];
  // printf("computing horocycle, master distance = %d [M=%p, A=%p]\n", master->alt->distance, hr::voidp(master), hr::voidp(alt));
  
  static const int LOOKUP = 16;
  set<heptagon*> hs[LOOKUP];
  hs[0].insert(master);
  set<heptagon*> region;
  for(int i=0; i<LOOKUP-1; i++) {
    for(auto h: hs[i]) {
      currentmap->extend_altmap(h);
      for(int j=0; j<S7; j++) {
        if(h->move(j)->alt->alt != master->alt->alt) continue;
        region.insert(h->move(j));
        if(h->move(j)->alt->distance < h->alt->distance)
          hs[i+1].insert(h->move(j));
        }
      }
    if(hs[i+1].empty()) { printf("error: hs[%d] not found\n", i+1); exit(1); }
    }
  /* printf("[%p] compute_horocycle ");
  for(int i=0; i<LOOKUP-1; i++) printf("%d -> ", isize(hs[i])); printf("%p\n", isize(hs[LOOKUP-1])); */
  map<cell*, int> xdist; 
  vector<cell*> xdqueue;
  cell *orig = periodmap[*(hs[LOOKUP-1].begin())].subcells[0];
  xdist[orig] = 0;
  xdqueue.push_back(orig);
  for(int i=0; i<isize(xdqueue); i++) {
    forCellCM(c1, xdqueue[i])
      if(!xdist.count(c1) && region.count(c1->master)) {
        xdist[c1] = xdist[xdqueue[i]] + 1;
        xdqueue.push_back(c1);
        }
    }
  int delta = NODISTANCE;
  for(int i=0; i<S7; i++) {
    heptagon *h = master->move(i);
    if(h->alt->alt != master->alt->alt) continue;
    heptinfo& hi = periodmap[h];
    if(!isize(hi.celldists[1])) continue;
    for(int c=0; c<isize(hi.subcells); c++) {
      if(hi.celldists[1][c] == NODISTANCE) continue;
      int delta_candidate = hi.celldists[1][c] - xdist[hi.subcells[c]];
      if(delta != NODISTANCE && delta_candidate != delta) {
        printf("delta conflict: %d vs %d\n", delta, delta_candidate);
        delta = max(delta, delta_candidate);
        }
      delta = delta_candidate;
      }
    }
  if(delta == NODISTANCE) {
    delta = master->alt->distance - xdist[periodmap[master].subcells[0]];
    // printf("delta not found, set to %d\n", delta);
    }
  // printf("using delta = %d\n", delta);

  for(int i=0; i<LOOKUP/2; i++) {
    for(auto h: hs[i]) for(int j=-1; j<S7; j++) {
      heptinfo& hi = periodmap[j == -1 ? h : h->move(j)];
      hi.celldists[1].resize(isize(hi.subcells));
      for(int c=0; c<isize(hi.subcells); c++)
        hi.celldists[1][c] = delta + xdist[hi.subcells[c]];
      }
    }

  last_on_horocycle[alt] = *(hs[LOOKUP/2].begin());
  }

EX int celldist(cell *c, bool alts) {
  heptagon *master = c->master;
  auto &hi = periodmap[master];
  /* if(alts && master->alt->alt->s != hsOrigin && isize(hi.celldists[alts]) == 0) {
    int doalts = 0;
    for(int i=0; i<S7; i++) if(master->move(i)->alt == master->alt->move[0]) {
      doalts = 1;
      if(periodmap[master->move(i)].celldists[true].empty()) {
        compute_horocycle(master);
        doalts = 2;
        }
      }
    if(doalts == 0) {
      currentmap->extend_altmap(master);
      for(int i=0; i<S7; i++) if(master->move(i)->alt == master->alt->move[0] && periodmap[master->move(i)].celldists[true].empty())
        compute_horocycle(master);
      }
    } */
  if(isize(hi.celldists[alts]) == 0) 
    compute_distances(master, alts);
  return hi.celldists[alts][cells[cellindex[c]].localindex];
  }

eGeometry orig_geometry;

void start_game_on_created_map() {    
  popScreen();
  for(hrmap *& hm : allmaps) if(hm == base) hm = NULL;
  stop_game();
  geometry = orig_geometry;
  variation = eVariation::irregular;
  irrid++;
  gridmaking = false;
  start_game();
  }

bool save_map(const string& fname) {
  fhstream f(fname, "wt");
  if(!f.f) return false;
  auto& all = base->allcells();
  int origcells = 0;
  for(cellinfo& ci: cells) 
    if(ci.generation == 0) 
      origcells++;
  println(f, spaced(int(geometry), isize(all), origcells));
  
  for(auto h: all) {
    origcells = 0;
    for(auto i: cells_of_heptagon[h->master]) 
      if(cells[i].generation == 0)
        origcells++;
    println(f, origcells);
    for(auto i: cells_of_heptagon[h->master]) if(cells[i].generation == 0) {
      auto &ci = cells[i];
      println(f, spaced(ci.p[0], ci.p[1], ci.p[LDIM]));
      }
    }
  return true;
  }

bool load_map(const string &fname) {
  fhstream f(fname, "rt");
  if(!f.f) return false;
  auto& all = base->allcells();
  int g, sa;
  scan(f, g, sa, cellcount);
  if(sa != isize(all) || g != geometry) { printf("bad parameters\n"); addMessage(XLAT("bad format or bad map geometry")); return false; }
  density = cellcount * 1. / isize(all);
  
  cells.clear();
  
  for(auto h: all) {
    int q = 0;
    scan(f, q);
    if(q < 0 || q > cellcount) { runlevel = 0; return false; }
    while(q--) {
      cells.emplace_back();
      cellinfo& s = cells.back();
      s.patterndir = -1;
      double a, b, c;
      scan(f, a, b, c);
      s.p = hpxyz(a, b, c);
      for(auto c0: all) s.relmatrices[c0] = calc_relative_matrix(c0, h, s.p);
      s.owner = h;
      }
    }

  make_cells_of_heptagon();
  runlevel = 2;
  return true;
  }

void cancel_map_creation() {
  base = NULL;
  runlevel = 0;
  popScreen();
  gridmaking = false;
  stop_game();
  geometry = orig_geometry;
  start_game();
  }

string irrmapfile = "irregularmap.txt";

string irrhelp = 
  "This option creates irregular grids to play the game on. "
  "Currently rather slow algorithms are used, "
  "so not recommended with too high density or "
  "with too large periodic base geometry. "
  "For technical reasons, the density cannot be too small.";

bool too_small_euclidean() {
  for(cell *c: base->allcells())
    forCellIdEx(c1, i1, c)
    forCellIdEx(c2, i2, c)
      if(i1 != i2 && c1 == c2) return true;
  return false;
  }

void show_gridmaker() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("irregular grid"));
  dialog::addSelItem(XLAT("density"), fts(density), 'd');
  dialog::add_action([] {
    dialog::editNumber(density, 1, 10, .1, 4, XLAT("density"), XLAT(irrhelp));
    dialog::reaction = [] () {
      int s = cellcount;
      if(density < 1) density = 1;
      cellcount = int(isize(currentmap->allcells()) * density + .5);
      println(hlog, "density = ", fts(density), " cellcount = ", cellcount);
      if(cellcount > s) runlevel = 1;
      if(cellcount < s) runlevel = 0;
      };
    });
  dialog::addSelItem(XLAT("min edge to median"), fts(quality), 'q');
  dialog::add_action([] {
    dialog::editNumber(quality, 0, 1, .05, .2, XLAT("quality"), XLAT(
      "The smallest allowed ratio of edge length to median edge length. "
      "Tilings with low values are easier to generate, but tend to be more ugly."
      ));
    dialog::reaction = [] () {
      println(hlog, "quality = ", density);
      if(runlevel > 4) runlevel = 4;
      };
    });
  dialog::addBreak(100);
  for(int i=0; i<5; i++)
    dialog::addInfo(status[i]);
  dialog::addBreak(100);
  dialog::addSelItem(XLAT("activate"), runlevel == 10 ? XLAT("ready") : XLAT("wait..."), 'f');
  if(runlevel == 10) dialog::add_action(start_game_on_created_map);
  dialog::addItem(XLAT("cancel"), 'c');
  dialog::add_action(cancel_map_creation);
  dialog::addItem(XLAT("save"), 's');
  dialog::add_action([] () {
    dialog::openFileDialog(irrmapfile, XLAT("irregular to save:"), ".txt", [] () {
      if(save_map(irrmapfile)) {
        addMessage(XLAT("Map saved to %1", irrmapfile));
        return true;
        }
      else {
        addMessage(XLAT("Failed to save map to %1", irrmapfile));
        return false;
        }
      });
    });
  dialog::addItem(XLAT("load"), 'l');
  dialog::add_action([] () {
    dialog::openFileDialog(irrmapfile, XLAT("irregular to load:"), ".txt", [] () {
      if(load_map(irrmapfile)) {
        addMessage(XLAT("Map loaded from %1", irrmapfile));
        return true;
        }
      else {
        addMessage(XLAT("Failed to load map from %1", irrmapfile));
        return false;
        }
      });
    });
  dialog::addSelItem(XLAT("bitruncation count"), its(bitruncations_requested), 'b');
  dialog::add_action([] () { 
    dialog::editNumber(bitruncations_requested, 0, 5, 1, 1, XLAT("bitruncation const"),
      XLAT("Bitruncation introduces some regularity, allowing more sophisticated floor tilings and textures."));
    dialog::reaction = [] () {
      if(bitruncations_requested > bitruncations_performed && runlevel > 5) runlevel = 5;
      if(bitruncations_requested < bitruncations_performed) runlevel = 0;
      };
    });
  if(too_small_euclidean())
    dialog::addInfo(XLAT("too small period -- irregular tiling generation fails"));
  dialog::addItem(XLAT("reset"), 'r');
  dialog::add_action([] () { runlevel = 0; });
  dialog::addHelp();
  dialog::display();
  keyhandler = [] (int sym, int uni) {
    handlePanning(sym, uni);
    if(uni == 'h' || sym == SDLK_F1) gotoHelp(XLAT(irrhelp));
    dialog::handleNavigation(sym, uni);
    // no exit
    };
  }

EX void visual_creator() {
  stop_game();
  orig_geometry = geometry;
  switch(geometry) {
    case gNormal:
      geometry = gKleinQuartic;
      break;
    
    case gOctagon:
      geometry = gBolza2;
      break;
    
    default: ;
      break;
    }

  variation = eVariation::pure;
  start_game();
  if(base) delete base;
  base = currentmap; 
  base_config = euc::eu;
  drawthemap();
  cellcount = int(isize(base->allcells()) * density + .5);
  pushScreen(show_gridmaker);
  runlevel = 0;
  gridmaking = true;
  }

EX void auto_creator() {
  variation = eVariation::pure;
  int cc = cellcount;
  bitruncations_requested = bitruncations_performed;
  visual_creator();
  cellcount = cc; density = cc * 1. / isize(base->allcells());
  printf("Creating the irregular map automatically...\n");
  while(runlevel < 10) step(1000);
  start_game_on_created_map();
  }

#if CAP_COMMANDLINE  
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-irrvis")) {
    PHASE(3);
    restart_game();
    visual_creator();
    showstartmenu = false;
    }
  else if(argis("-irrdens")) {
    PHASEFROM(2);
    shift_arg_formula(density);
    }
  else if(argis("-irrb")) {
    PHASEFROM(2);
    shift(); bitruncations_requested = argi();
    }
  else if(argis("-irrq")) {
    PHASEFROM(2);
    shift_arg_formula(quality);
    }
  else if(argis("-irrload")) {
    PHASE(3);
    restart_game();
    visual_creator();
    showstartmenu = false;
    shift();
    load_map(args());
    while(runlevel < 10) step(1000);
    start_game_on_created_map();
    }
  else return 1;
  return 0;
  }
#endif

EX unsigned char density_code() {
  if(isize(cells) < 128) return isize(cells);
  else {
    int t = 127, a = isize(cells);
    while(a > 127) a = a * 9/10, t++;
    return t;
    }
  }

EX bool pseudohept(cell* c) {
  return cells[cellindex[c]].is_pseudohept;
  }

EX bool ctof(cell* c) {
  return cells[cellindex[c]].patterndir == -1;
  }

EX bool supports(eGeometry g) {
  if(g == gEuclid || g == gEuclidSquare) return ginf[g].flags & qCLOSED;
  return among(g, gNormal, gKleinQuartic, gOctagon, gBolza2, gFieldQuotient, gSphere, gSmallSphere, gTinySphere);
  }

EX array<heptagon*, 3> get_masters(cell *c) {
  int d = cells[cellindex[c]].patterndir;
  heptspin s = periodmap[c->master].base;
  heptspin s0 = heptspin(c->master, 0) + (d - s.spin);
  return make_array(s0.at, (s0 + wstep).at, (s0 + 1 + wstep).at);
  }

EX void swap_vertices() {
  for(auto& c: cells) {
    swappoint(c.p);
    swapmatrix(c.pusher);
    swapmatrix(c.rpusher);
    for(auto& jp: c.jpoints) swappoint(jp);
    for(auto& rm: c.relmatrices) swapmatrix(rm.second);
    for(auto& v: c.vertices) swappoint(v);
    }
  }

auto hook = 
#if CAP_COMMANDLINE
  addHook(hooks_args, 100, readArgs) + 
#endif
#if MAXMDIM >= 4
  addHook(hooks_swapdim, 100, swap_vertices) +
#endif
  addHook(hooks_drawcell, 100, draw_cell_schematics) +
  addHook(shmup::hooks_turn, 100, step);

#endif
  }}

/*
  if(mouseover && !ctof(mouseover)) {
    for(auto h: gp::get_masters(mouseover)) 
      queueline(ggmatrix(h->c7)*C0, shmup::ggmatrix(mouseover)*C0, 0xFFFFFFFF);
    }
    
*/
