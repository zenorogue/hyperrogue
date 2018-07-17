namespace hr { namespace irr {

bool on;

ld density = 6;
ld quality = .2;
int place_attempts = 10;
int rearrange_max_attempts = 50;
int rearrange_less = 10;

int cellcount;

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
  
string status[5];
  
hrmap *base;

bool gridmaking;

int rearrange_index;

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
         s.owner = h, s.p = spin(hrand(1000)) * xpush(.01) * C0;
         for(auto c0: all) s.relmatrices[c0] = shmup::calc_relative_matrix(c0, s.owner, s.p);
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
          for(auto c0: all) relmatrices[c0] = shmup::calc_relative_matrix(c0, c, h);
          ld mindist = 1e6;
          for(auto p: cells) {
            if(!relmatrices.count(p.owner)) continue;
            ld val = hdist(h, relmatrices[p.owner] * p.p);
            if(val < mindist) mindist = val;
            }
          if(mindist > bestval) bestval = mindist, s.owner = c, s.p = h, s.relmatrices = move(relmatrices);
          }
        // printf("%lf %p %s\n", bestval, s.owner, display(s.p));
        }
      make_cells_of_heptagon();
      runlevel++;
      status[0] = "all " + its(isize(cells)) + " cells";
      break;
      }
    
    case 2: {

      sort(cells.begin(), cells.end(), [] (const cellinfo &s1, const cellinfo &s2) { return hdist0(s1.p) < hdist0(s2.p); });
      make_cells_of_heptagon();

      edgelens.clear();
      distlens.clear();
  
      int stats[16];
      for(int k=0; k<16; k++) stats[k] = 0;
      
      for(int i=0; i<cellcount; i++) {
        auto &p1 = cells[i];
        p1.vertices.clear();
        p1.neid.clear();
    
        p1.pusher = rgpushxto0(p1.p);
        p1.rpusher = gpushxto0(p1.p);
        
        p1.jpoints.clear();
    
        for(int j=0; j<cellcount; j++) {
          auto &p2 = cells[j];
          p1.jpoints.push_back(p1.rpusher * p1.relmatrices[p2.owner] * p2.p);
          }
    
        int j = 0;
        if(j == i) j = 1;
    
        for(int k=0; k<cellcount; k++) if(k != i) {
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
          for(int k=0; k<cellcount; k++) if(k != i && k != j && k != oldj) {
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
      status[3] = XLAT("median edge: %1 minimum: %2", fts4(median), fts4(edgelens[0]));
      if(edgelens[0] < minedge) {
        if(rearrange_index >= rearrange_max_attempts) {
          runlevel = 0; break;
          }
        int tooshort = 0;
        for(int i=0; i<isize(cells); i++) {
          auto& p1 = cells[i];
          using namespace hyperpoint_vec;
          hyperpoint h = hpxyz(0, 0, 0);
          for(auto v: p1.vertices) h = h + v;
          
          bool changed = rearrange_index < rearrange_less;
    
          for(int j=0; j<isize(p1.vertices); j++)
            if(hdist(p1.vertices[j], p1.vertices[(j+1) % isize(p1.vertices)]) < minedge) {
              tooshort++; changed = true;
              h = h + p1.vertices[j] + p1.vertices[(j+1) % isize(p1.vertices)];
              }
          if(changed)
            cells[i].p = p1.pusher * normalize(h);
          }
        status[3] += XLAT(" (edges too short: %1)", its(tooshort));
        runlevel = 2;
        rearrange_index++;
        break;
        }
      runlevel++;
      break;
      }
    
    case 5: {
      
      int notfound = 0;
    
      for(int i=0; i<cellcount; i++) {
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
      
      if(notfound) { status[4] = XLAT("cells badly paired: %1", its(notfound)); runlevel = 0; break; }
      
      int heptas = 0;
      for(auto p: cells_of_heptagon) {
        printf("%p: %d\n", p.first, isize(p.second));
        heptas++;
        }
      
      if(heptas != isize(all)) {
        status[4] = XLAT("cells not covered: %1", its(isize(all) - heptas));
        printf("heptas = %d\n", heptas);
        runlevel = 0; break;
        }
    
      int faredge = 0;
      for(int i=0; i<cellcount; i++) {
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

      status[4] = XLAT("OK");
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
    scale = sqrt(isize(cells_of_heptagon) * 1. / cellcount);
    crossf *= scale;
    hepvdist *= scale;
    rhexf *= scale;
    base_distlimit = (base_distlimit + log(scale) / log(2.618)) / scale;
    if(base_distlimit > 25) base_distlimit = 25;
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

eGeometry orig_geometry;

void start_game_on_created_map() {    
  popScreen();
  for(hrmap *& hm : allmaps) if(hm == base) hm = NULL;
  stop_game();
  geometry = orig_geometry;
  irr::on = true;
  nonbitrunc = true;
  gp::on = false;
  need_reset_geometry = true;
  gridmaking = false;
  start_game();
  }

void cancel_map_creation() {
  popScreen();
  gridmaking = false;
  stop_game();
  geometry = orig_geometry;
  need_reset_geometry = true;
  start_game();
  }

void show_gridmaker() {
  cmode = sm::SIDE;
  gamescreen(0);  
  dialog::init(XLAT("Irregular grid"));
  dialog::addSelItem(XLAT("density"), fts(density), 'd');
  dialog::add_action([] {
    dialog::editNumber(density, 1, 10, .1, 4, "density", "");
    dialog::reaction = [] () {
      int s = cellcount;
      if(density < 1) density = 1;
      cellcount = int(isize(currentmap->allcells()) * density + .5);
      printf("density = %lf cellcount = %d\n", double(density), cellcount);
      if(cellcount > s) runlevel = 1;
      if(cellcount < s) runlevel = 0;
      };
    });
  dialog::addSelItem(XLAT("min edge to median"), fts(quality), 'q');
  dialog::add_action([] {
    dialog::editNumber(quality, 0, 1, .1, 4, "quality", "");
    dialog::reaction = [] () {
      printf("quality = %lf\n", double(density));
      if(runlevel > 4) runlevel = 4;
      };
    });
  dialog::addBreak(100);
  for(int i=0; i<5; i++)
    dialog::addInfo(status[i]);
  dialog::addBreak(100);
  dialog::addSelItem(XLAT("activate"), XLAT(runlevel == 10 ? "ready" : "wait..."), 'f');
  if(runlevel == 10) dialog::add_action(start_game_on_created_map);
  dialog::addItem(XLAT("cancel"), 'c');
  dialog::add_action(cancel_map_creation);
  dialog::display();
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    // no exit
    };
  }

void visual_creator() {
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

  nonbitrunc = true;
  gp::on = false;
  need_reset_geometry = true;
  start_game();
  if(base) delete base;
  base = currentmap; 
  drawthemap();
  cellcount = int(isize(base->allcells()) * density + .5);
  pushScreen(show_gridmaker);
  runlevel = 0;
  gridmaking = true;
  }

void auto_creator() {
  irr::on = false;
  int cc = cellcount;
  visual_creator();
  cellcount = cc; density = cc / isize(base->allcells());
  printf("Creating the irregular map automatically...\n");
  while(runlevel < 10) step(1000);
  start_game_on_created_map();
  }
  
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-irrvis")) {
    PHASE(3);
    restart_game();
    visual_creator();
    showstartmenu = false;
    }
  else return 1;
  return 0;
  }

unsigned char density_code() {
  if(cellcount < 128) return cellcount;
  else {
    int t = 127, a = cellcount;
    while(a > 127) a = a * 9/10, t++;
    return t;
    }
  }

bool pseudohept(cell* c) {
  return cells[cellindex[c]].is_pseudohept;
  }

bool ctof(cell* c) {
  return cells[cellindex[c]].patterndir == -1;
  }

bool supports(eGeometry g) {
  return among(g, gNormal, gKleinQuartic, gOctagon, gBolza2, gFieldQuotient, gSphere, gSmallSphere, gTinySphere);
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
