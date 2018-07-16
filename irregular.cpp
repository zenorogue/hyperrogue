namespace hr { namespace irr {

bool on;

int sc = 100;

struct cellinfo {
  cell *owner;
  vector<hyperpoint> jpoints;
  hyperpoint p;
  transmatrix pusher, rpusher;
  vector<int> neid;
  vector<int> spin;
  vector<hyperpoint> vertices;
  int localindex;
  bool is_pseudohept;
  int patterndir;
  };

map<cell*, int> cellindex;

vector<cellinfo> cells;

ld inner(hyperpoint h1, hyperpoint h2) {
  return 
    hyperbolic ? h1[2] * h2[2] - h1[0] * h2[0] - h1[1] * h2[1] :
    h1[2] * h2[2] + h1[0] * h2[0] + h1[1] * h2[1];
  }

hyperpoint circumscribe(hyperpoint a, hyperpoint b, hyperpoint c) {
  using namespace hyperpoint_vec;
  hyperpoint h = C0;

  b = b - a;
  c = c - a;
  
  if(inner(b,b) < 0) {
    b = b / sqrt(-inner(b, b));
    c = c + b * inner(c, b);
    h = h + b * inner(h, b);
    }
  else {
    b = b / sqrt(inner(b, b));
    c = c - b * inner(c, b);
    h = h - b * inner(h, b);
    }
  
  if(inner(c,c) < 0) {
    c = c / sqrt(-inner(c, c));
    h = h + c * inner(h, c);
    }
  else {
    c = c / sqrt(inner(c, c));
    h = h - c * inner(h, c);
    }
  
  if(h[2] < 0) h[0] = -h[0], h[1] = -h[1], h[2] = -h[2];

  ld i = inner(h, h);
  if(i > 0) h /= sqrt(i);
  else h /= -sqrt(-i);

  return h;
  }

bool clockwise(hyperpoint h1, hyperpoint h2) {
  return h1[0] * h2[1] > h1[1] * h2[0];
  }

map<heptagon*, vector<int> > cells_of_heptagon;

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
  
hrmap *base;

bool gridmaking;

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
     
     if(sc <= isize(all) * 2) {
       for(auto h: all) {
         cellinfo s; s.patterndir = -1;
         s.owner = h, s.p = spin(hrand(1000)) * xpush(.01) * C0;
         cells.emplace_back(s);
         }
       }
      runlevel++;
      break;
      }
     
    case 1: {
      while(isize(cells) < sc) {
        if(SDL_GetTicks() > t + 250) { make_cells_of_heptagon(); return false; }
        cellinfo s; s.patterndir = -1;
        ld bestval = 0;
        for(int j=0; j<10; j++) {
          int k = hrand(isize(all));
          cell *c = all[k];
          hyperpoint h = randomPointIn(c->type);
          ld mindist = 1e6;
          for(auto p: cells) {
            ld val = hdist(h, shmup::calc_relative_matrix(p.owner, c, h) * p.p);
            if(val < mindist) mindist = val;
            }
          if(mindist > bestval) bestval = mindist, s.owner = c, s.p = h;
          }
        // printf("%lf %p %s\n", bestval, s.owner, display(s.p));
        cells.emplace_back(s);
        }
      make_cells_of_heptagon();
      runlevel++;
      break;
      }
    
    case 2: {

      sort(cells.begin(), cells.end(), [] (cellinfo &s1, cellinfo &s2) { return hdist0(s1.p) < hdist0(s2.p); });
      make_cells_of_heptagon();

      edgelens.clear();
      distlens.clear();
  
      int stats[16];
      for(int k=0; k<16; k++) stats[k] = 0;
      
      for(int i=0; i<sc; i++) {
        auto &p1 = cells[i];
        p1.vertices.clear();
        p1.neid.clear();
    
        p1.pusher = rgpushxto0(p1.p);
        p1.rpusher = gpushxto0(p1.p);
        
        p1.jpoints.clear();
    
        for(int j=0; j<sc; j++) {
          auto &p2 = cells[j];
          p1.jpoints.push_back(p1.rpusher * shmup::calc_relative_matrix(p2.owner, p1.owner, p1.p) * p2.p);
          }
    
        int j = 0;
        if(j == i) j = 1;
    
        for(int k=0; k<sc; k++) if(k != i) {
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
          for(int k=0; k<sc; k++) if(k != i && k != j && k != oldj) {
            hyperpoint h = circumscribe(C0, p1.jpoints[j], p1.jpoints[k]);
            if(h[2] < 0) continue;
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
  
      for(int i=0; i<isize(cells); i++) if(isize(cells[i].vertices) > 8 || isize(cells[i].vertices) < 3) {
        cells[i] = cells.back();
        i--; cells.pop_back();
        }
      if(isize(cells) < sc*3/4) runlevel = 0;
      else if(isize(cells) < sc) runlevel = 1;
      else runlevel++;
      break;
      }
    
    case 4: {
  
      ld minedge = edgelens[isize(edgelens) / 2] / 5;
      if(edgelens[0] < minedge) {
        printf("rearranging\n");
        for(int i=0; i<isize(cells); i++) {
          auto& p1 = cells[i];
          using namespace hyperpoint_vec;
          hyperpoint h = hpxyz(0, 0, 0);
          for(auto v: p1.vertices) h = h + v;
    
          for(int j=0; j<isize(p1.vertices); j++)
            if(hdist(p1.vertices[j], p1.vertices[(j+1) % isize(p1.vertices)]) < minedge)
              h = h + p1.vertices[j] + p1.vertices[(j+1) % isize(p1.vertices)];
          cells[i].p = p1.pusher * normalize(h);
          }
        runlevel = 2;
        break;
        }
      runlevel++;
      break;
      }
    
    case 5: {
      
      int notfound = 0;
    
      for(int i=0; i<sc; i++) {
        auto &p1 = cells[i];
        int N = isize(p1.vertices);
        p1.spin.resize(N);
        for(int j=0; j<N; j++) {
          auto i1 = p1.neid[j];
          bool found = false;
          for(int k=0; k < isize(cells[i1].vertices); k++)
            if(cells[i1].neid[k] == i)
              found = true, p1.spin[j] = k;
          if(!found) notfound++;
          }
        }
      
      printf("notfound = %d\n", notfound);
      if(notfound) { runlevel = 0; break; }
      
      int heptas = 0;
      for(auto p: cells_of_heptagon) {
        printf("%p: %d\n", p.first, isize(p.second));
        heptas++;
        }
      
      if(heptas != isize(all)) {
        printf("heptas = %d\n", heptas);
        runlevel = 0; break;
        }
    
      for(int i=0; i<sc; i++) {
        auto &p1 = cells[i];
        for(int j: p1.neid) {
          auto &p2 = cells[j];
          bool ok = p1.owner == p2.owner || isNeighbor(p1.owner, p2.owner);
          if(!ok) { printf("far edge\n"); runlevel = 0; return false; }
          }
        }
      runlevel = 10;
      
      for(auto& s: cells) s.is_pseudohept = false;
      for(auto& s: cells) {
        s.is_pseudohept = true;
        for(int i: s.neid) if(cells[i].is_pseudohept) s.is_pseudohept = false;
        }

      for(auto& s: cells) {
        int d = -1;
        ld dist = hcrossf / 2;
        ld dists[8];
        for(int i=0; i<S7; i++) {
          dists[i] = hdist(s.p, spin(hexshift - i * ALPHA) * xpush(-hcrossf) * C0);
          // shmup::calc_relative_matrix(s.owner->mov[i], s.owner, s.p) * C0);
          // spin(2 * M_PI * i / S7) * xpush(hcrossf) * C0);
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

ld scale;

void compute_geometry() {
  if(irr::on) {
    scale = sqrt(isize(cells_of_heptagon) * 1. / sc);
    crossf *= scale;
    hepvdist *= scale;
    rhexf *= scale;
    }
  else scale = 1;
  }

bool draw_cell_schematics(cell *c, transmatrix V) {
  if(gridmaking) {
    heptagon *h = c->master;
    for(int i: cells_of_heptagon[h]) {
      auto& p = cells[i];
      if(p.owner == c) {
        queuestr(V * rgpushxto0(p.p), .1, its(i), isize(p.vertices) > 8 ? 0xFF0000 : 0xFFFFFF);
        int N = isize(p.vertices);
        for(int j=0; j<N; j++)
          queueline(V * p.pusher * p.vertices[j], V * p.pusher * p.vertices[(1+j)%N], 0xFFFFFFFF);

        queueline(V * p.p, V * C0, 0xFF0000FF);
        if(p.patterndir != -1)
          queueline(V * p.p, V * shmup::calc_relative_matrix(c->master->move[p.patterndir]->c7, c, p.p) * C0, 0x00FF00FF);
        }
      }
    }
  return false;
  }

struct heptinfo {
  heptspin base;
  vector<cell*> subcells;
  };

map<heptagon*, heptinfo> periodmap;

void link_to_base(heptagon *h, heptspin base) {
  // printf("linking %p to %p/%d\n", h, base.h, base.spin);
  auto &hi = periodmap[h];
  hi.base = base;
  for(int k: cells_of_heptagon[base.h]) {
    cell *c = newCell(isize(cells[k].vertices), h);
    hi.subcells.push_back(c);
    cellindex[c] = k;
    }
  h->c7 = hi.subcells[0];
  }

void clear_links(heptagon *h) {
  auto& hi = periodmap[h];
  for(cell *c: hi.subcells) {
    for(int i=0; i<c->type; i++) if(c->mov[i]) c->mov[i]->mov[c->spin(i)] = NULL;
    cellindex.erase(c);
    delete c;
    }
  h->c7 = NULL;
  periodmap.erase(h);
  }

void link_start(heptagon *h) {
  link_to_base(h, heptspin(cells[0].owner->master, 0));
  }

void link_next(heptagon *parent, int d) {
  if(!periodmap.count(parent))
    link_to_base(parent, heptspin(cells[0].owner->master, 0));
  // printf("linking next: %p direction %d [s%d]\n", parent, d, parent->spin(d));
  auto *h = parent->move[d];
  heptspin hs = periodmap[parent].base + d + wstep - parent->spin(d);
  link_to_base(h, hs);
  }

void may_link_next(heptagon *parent, int d) {
  if(!periodmap.count(parent->move[d]))
    link_next(parent, d);
  }


void link_cell(cell *c, int d) {
  // printf("linking cell: %p direction %d\n", c, d);
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
    for(int d=0; d<S7; d++) if(sc2.owner->master == sc.owner->master->move[(os+d)%S7]) {
      heptspin hss(c->master, d);
      hss += wstep;
      master2 = hss.h;
      // printf("master2 is %p; base = %p; should be = %p\n", master2, periodmap[master2].base.h, sc2.owner->master);
      dirs++;
      }
    if(dirs != 1) { printf("dirs error\n"); exit(1); }
    }
  
  cell *c2 = periodmap[master2].subcells[sc2.localindex];
  c->mov[d] = c2;
  tsetspin(c->spintable, d, sc.spin[d]);
  c2->mov[sc.spin[d]] = c;
  tsetspin(c2->spintable, sc.spin[d], d);
  }

void show_gridmaker() {
  cmode = sm::SIDE;
  gamescreen(0);  
  dialog::init(XLAT("Irregular grid"));
  dialog::addSelItem(XLAT("activate"), its(runlevel), 'f');
  dialog::display();
  if(runlevel == 10) dialog::add_action([] {
    popScreen();
    pop_game();
    for(hrmap *& hm : allmaps) if(hm == base) hm = NULL;
    stop_game();
    irr::on = true;
    nonbitrunc = true;
    need_reset_geometry = true;
    gridmaking = false;
    start_game();
    });
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    // no exit
    };
  }

void create_map() {
  push_game();
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

  nonbitrunc = true;
  need_reset_geometry = true;
  start_game();
  base = currentmap; 
  drawthemap();
  pushScreen(show_gridmaker);
  runlevel = 0;
  gridmaking = true;
  }
  
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-irr")) {
    PHASE(3);
    shift(); sc = argi();
    restart_game();
    create_map();
    }
  else return 1;
  return 0;
  }

bool pseudohept(cell* c) {
  return cells[cellindex[c]].is_pseudohept;
  }

bool ctof(cell* c) {
  return cells[cellindex[c]].patterndir == -1;
  }

array<heptagon*, 3> get_masters(cell *c) {
  int d = cells[cellindex[c]].patterndir;
  heptspin s = periodmap[c->master].base;
  heptspin s0 = heptspin(c->master, 0) + (d - s.spin);
  return make_array(s0.h, (s0 + wstep).h, (s0 + 1 + wstep).h);
  }

auto hook = 
  addHook(hooks_args, 100, readArgs) + 
  addHook(hooks_drawcell, 100, draw_cell_schematics) +
  addHook(shmup::hooks_turn, 100, step);

  }}

/*
  if(mouseover && !ctof(mouseover)) {
    for(auto h: gp::get_masters(mouseover)) 
      queueline(shmup::ggmatrix(h->c7)*C0, shmup::ggmatrix(mouseover)*C0, 0xFFFFFFFF);
    }
    
*/
