#include "../rogueviz/rogueviz.h"


// torus balloon:
// ./ht -smart .5 -geo beti -canvas-random 0 -zoom .1  -sight3 30 -smartlimit 10 -noplayer  -PM 0 -clip -50 50 -ibal balloon_volume=1 -run "balloon_volume=10..150000../0..10" -grotate 1 2 90*deg -grotate 0 2 30*deg -animperiod 10000 -animrot 0 90 -shot-1000 -back ffffff -fore 0 -animvideo 1800 torus-balloon.mp4
// donutrad = 4 hexagonal = true SIZE = 21 noise = 1e-2

// sphere balloon:
// ./ht -smart .5 -geo beti -canvas-random 0 -zoom .1  -sight3 30 -smartlimit 10 -noplayer  -PM 0 -clip -50 50 -ibal balloon_volume=1 "balloon_volume=1../0..150000../0..1../0" -grotate 1 2 90*deg -grotate 0 2 30*deg -animperiod 10000 -animrot 0 90 -shot-1000 -back ffffff -fore 0 -animvideo 1800 sphere-balloon.mp4
// donutrad = -1 hexagonal = true SIZE = 21 noise = 0

// S2xE simulation:
// ./ht -smart .5 -geo beti -canvas-random 0 -zoom .01  -sight3 30 -smartlimit 10 -noplayer  -PM 0 -clip -50 50 -ibal balloon_volume=150000 -wsh 9 -ray-do -run -ibale -switch-fpp wall_height=100 depth=50 -ray-iter 600 -ray-range 1 100 -dgl -run -grotate 1 2 60*deg -animmove 400 -60 90 -shot-1000 -animvideo 600 balloonxe-sphere.mp4
// donutrad = -1 hexagonal = true SIZE = 21 noise = 0

// flatxE simulation:
// ./ht -smart .5 -geo beti -canvas-random 0 -zoom .01  -sight3 30 -smartlimit 10 -noplayer  -PM 0 -clip -50 50 -ibal balloon_volume=.01 -wsh 9 -ray-do -run -ibale -switch-fpp wall_height=100 depth=50 -ray-iter 600 -ray-range 1 100 -dgl -run -grotate 1 2 60*deg -animmove 400 -60 90 -shot-1000 -animvideo 600 balloonxe-flat.mp4

// intermediate xE
// ./ht -msens 0 -smart .5 -geo beti -canvas-random 0 -zoom .01  -sight3 30 -smartlimit 10 -noplayer  -PM 0 -clip -50 50 -ibal balloon_volume=150000 -run balloon_volume=5000 -grotate 1 2 90*deg -grotate 0 2 30*deg -animperiod 10000 -animrot 0 90  -shot-1000 -animvideo 120 balloonxe-intermediate.mp4

// intermediate show:
// ./ht -msens 0 -smart .5 -geo beti -canvas-random 0 -zoom .01  -sight3 30 -smartlimit 10 -noplayer  -PM 0 -clip -50 50 -ibal balloon_volume=150000 -run balloon_volume=5000 -zoom 0.2 -run -grotate 1 2 90*deg -grotate 0 2 30*deg -animperiod 10000 -animrot 0 90  -shot-1000 -animvideo 120 intermediate-balloon.mp4

// balloon-net:
// ./ht -smart .5 -geo beti -canvas-random 0 -zoom .01  -sight3 30 -smartlimit 10 -noplayer  -PM 0 -clip -50 50 -ibal balloon_volume=150000 -shot-1000 -back ffffff -fore 0 -wsh 9 
// animated: add  -run -animmove 20 0 30 -animvideo 600 net-moving.mp4

// balloon algorithm on manifolds:

// ./ht -geo Zebra -gp 10 0 -ibalc -msens 0 -smart .5 -geo beti -canvas-random 0 -zoom .1  -sight3 30 -smartlimit 10 -noplayer  -PM 0 -clip -10 10 -ibal balloon_volume=150000 -lw 8 -fore 0 -back ffffff -run -animrot 0 90 -shot-1000 -animvideo 300 balloon-zebra.mp4
// ./ht -geo Klein -gp 10 0 -ibalc -msens 0 -smart .5 -geo beti -canvas-random 0 -zoom .1  -sight3 30 -smartlimit 10 -noplayer  -PM 0 -clip -10 10 -ibal balloon_volume=150000 -lw 8 -fore 0 -back ffffff -run -animrot 0 90 -shot-1000 -animvideo 300 balloon-klein.mp4
// ./ht -geo Macbeath -gp 10 0 -ibalc -msens 0 -smart .5 -geo beti -canvas-random 0 -zoom .1  -sight3 30 -smartlimit 10 -noplayer  -PM 0 -clip -10 10 -ibal balloon_volume=150000 -lw 8 -fore 0 -back ffffff -run -animrot 0 90 -shot-1000 -animvideo 300 balloon-mcb.mp4
// ./ht -geo Bolza -gp 10 0 -ibalc -msens 0 -smart .5 -geo beti -canvas-random 0 -zoom .1  -sight3 30 -smartlimit 10 -noplayer  -PM 0 -clip -10 10 -ibal balloon_volume=150000 -lw 8 -fore 0 -back ffffff -run -animrot 0 90 -shot-1000 -animvideo 300 balloon-bolza.mp4

namespace hr {

namespace balloonsim {

#define SIZE 21

struct data;
using tri = array<data*, 3>;

bool hexagonal = true;
int donutrad = -1;
ld noise = 0;

ld part = 0.5;

struct data {
  hyperpoint loc;
  hyperpoint vel;
  int side;
  vector<pair<data*, data*> > tris;
  data(hyperpoint h ) { loc = h; vel = C0-C0; side = 0; }
  };

vector<data*> allpoints;

data* top[SIZE][SIZE];
data* bot[SIZE][SIZE];

vector<pair<data*, data*> > edges;

vector<tri> allfaces;
vector<color_t> facecolor;

vector<pair<hyperpoint, hyperpoint> > intersections;

bool draw_intersections;

ld total_volume = 0;
ld req_volume = 5;

ld volume_of(hyperpoint a, hyperpoint b, hyperpoint c) {
  transmatrix T;
  T[0] = a;
  T[1] = b;
  T[2] = c;
  return det3(T);
  }

int slowdown = 1;

int steps = 0;

int tsteps = 0, ssteps = 0;

bool physics(int delta) {
  if(draw_intersections) return false;
  tsteps += delta;
  while(tsteps > ssteps) {
    ssteps += slowdown;
    steps++;

    // surface tension
    for(auto e: edges) {
      hyperpoint dis = e.second->loc - e.first->loc;
      ld len = hypot_d(3, dis);
      hyperpoint force = dis * ((len - 1) / len);
      e.first->vel += force;
      }
    
    ld pressure = req_volume / max(total_volume, req_volume * 1e-1);
    pressure *= 1e-2;
    
    for(auto t: allfaces) {
      hyperpoint e1 = t[1]->loc - t[0]->loc;
      hyperpoint e2 = t[2]->loc - t[0]->loc;
      hyperpoint cros = (e1 ^ e2);
      cros[3] = 0;
      // ld croslen = hypot_d(3, cros);
      hyperpoint normal = pressure * cros; // * croslen / croslen;
      // println(hlog, "normal = ", normal, " cros = ", cros, " pressure = ", pressure);
      t[0]->vel += normal;
      t[1]->vel += normal;
      t[2]->vel += normal;
      }

    /* if(false) if(steps < -1000) 
      for(auto p: allpoints) p->vel = hyperpoint(0, 0, -p->side, 0);
    else
      for(auto p: allpoints) p->vel = hyperpoint(0, 0, 0, 0); */
    
    ld step = 1e-2;

    // move stuff and friction
    for(auto p: allpoints) {
      // println(hlog, p->loc, " ++ ", p->vel);
      for(auto tr: p->tris)
        total_volume += volume_of(p->vel, tr.first->loc - p->loc, tr.second->loc - p->loc) * step;
      p->loc = p->loc + p->vel * step;
      p->vel = p->vel * .9;
      }
    println(hlog, "total volume = ", total_volume, " / ", req_volume, " ST = ", steps);
    }    
  return true;
  }

void explore();

int startid = 0;

void create_balloon() {

  for(int x=0; x<SIZE; x++)
  for(int y=0; y<SIZE; y++) {
    hyperpoint ini = C0;
    ld dx = (x-(SIZE-1)/2);
    ld dy = (y-(SIZE-1)/2);
    ini[0] += dx + dy/2;
    ini[1] += dy * sqrt(3) / 2;

    bool good = false;
    
    if(hexagonal) {
      good = x>0 && y>0 && x<SIZE-1 && y<SIZE-1 && x+y >= SIZE/2+1 && x+y < 3*SIZE/2-1;
      good &= (dx>donutrad || dy>donutrad || dx<-donutrad || dy<-donutrad || dx+dy>donutrad || dx+dy<-donutrad);
      }
    else
      good = hdist0(ini) < SIZE * sqrt(3)/4.-2;
    
    if(good) {
      if(x==0 || y==0 || x==SIZE-1 || y==SIZE-1) {
        println(hlog, "BUG ", tie(x, y));
        exit(1);
        }
      top[y][x] = new data(ini);
      allpoints.push_back(top[y][x]);
      }
    }
  
  vector<pair<int, int> > dirs =
    { {-1,0}, {1,0}, {0,-1}, {0,1}, {1, -1}, {-1, 1} };

  for(int x=0; x<SIZE; x++)
  for(int y=0; y<SIZE; y++) if(top[y][x]) {
    int nei = 0;
    for(pair<int, int> p: dirs)
      if(top[y+p.first][x+p.second])
        nei++;
    if(nei == 6) {
      bot[y][x] = new data(top[y][x]->loc);
      allpoints.push_back(bot[y][x]);
      top[y][x]->side = 1;
      bot[y][x]->side = -1;
      }
    else
      bot[y][x] = top[y][x];
    }

  println(hlog, "all");
  
  auto central = [] (array<data*, 3> arr) {
    ld dist = hdist0(arr[0]->loc) + hdist0(arr[1]->loc) + hdist0(arr[2]->loc);
    return dist <= 4;
    };

  for(int x=0; x<SIZE; x++)
  for(int y=0; y<SIZE; y++) if(top[y][x]) {

    for(pair<int, int> p: dirs)
      if(top[y+p.first][x+p.second]) {
        println(hlog, "len = ", top[y][x]->loc -  top[y+p.first][x+p.second]->loc);
        edges.emplace_back(top[y][x], top[y+p.first][x+p.second]);
        if(top[y][x] != bot[y][x] || top[y+p.first][x+p.second] != bot[y+p.first][x+p.second])
          edges.emplace_back(bot[y][x], bot[y+p.first][x+p.second]);
        }
      
    println(hlog, "point @ ", tie(x,y));

    for(auto side: {top, bot}) {
      array<data*, 3> arr;
      arr[0] = side[y][x];
      arr[1] = side[y][x+1];
      if(side == bot) swap(arr[0], arr[1]);
      arr[2] = side[y+1][x];

      if(arr[0] && arr[1] && arr[2])  {
        if(arr[0] == top[SIZE/2][SIZE/2]) startid = isize(allfaces);
        allfaces.push_back(arr);
        facecolor.push_back(central(arr) ? 0x8080FF : side == top ? 0xFFD500 : 0xC00000);
        }

      swap(arr[0], arr[1]);
      arr[2] = side[y-1][x+1];
      if(arr[0] && arr[1] && arr[2]) {
        allfaces.push_back(arr);
        facecolor.push_back(central(arr) ? 0x8080FF : side == top ? 0xFFFF00 : 0xC04000);
        }
      }
    }
  
  for(auto& f: allfaces) for(int i: {0,1,2})
    f[i]->tris.emplace_back(f[(i+1)%3], f[(i+2)%3]);
  
  for(auto p: allpoints) p->loc[0] += (randd()-randd()) * noise;
  
  println(hlog, "points = ", isize(allpoints));
  println(hlog, "edges = ", isize(edges));
  println(hlog, "faces = ", isize(allfaces));
  }

void compute_intersections() {
  intersections.clear();
  for(auto& f1: allfaces)
  for(auto& f2: allfaces) {
  
    if(f1 == f2) break;
  
    bool adjacent = false;

    for(int i=0; i<3; i++)
    for(int j=0; j<3; j++)
      if(f1[i] == f2[j])
        adjacent = true;
    
    if(adjacent) continue;
    
    vector<hyperpoint> ix;
    
    for(int i=0; i<2; i++) {
      auto& f = i ? f1 : f2;
      auto& g = i ? f2 : f1;
      hyperpoint cros = (f[1]->loc-f[0]->loc) ^ (f[2]->loc-f[0]->loc);
      ld dotval = dot_d(3, f[1]->loc, cros);
      transmatrix T;
      for(int i=0; i<3; i++)
        set_column(T, i, f[i]->loc);
      set_column(T, 3, C0);
      T = inverse(T);
      for(int i=0; i<3; i++) {
        hyperpoint g1 = g[i]->loc;
        hyperpoint g2 = g[(i+1)%3]->loc;
        ld dg1 = dot_d(3, g1, cros);
        ld dg2 = dot_d(3, g2, cros);
        if(dg1 == dg2) continue;
        ld il = ilerp(dg1, dg2, dotval);
        if(il < 0 || il > 1) continue;
        hyperpoint cp = lerp(g1, g2, il);
        hyperpoint res = T * cp;
        if(res[0] >= 0 && res[1] >= 0 && res[2] >= 0)
          ix.push_back(cp);
        }
      }
    if(ix.empty()) continue;
    if(isize(ix) != 2) {
      println(hlog, "ix is: ", ix);
      intersections.emplace_back(ix[0], ix[0] + hyperpoint(1,0,0,0));
      continue;
      }
    sort(ix.begin(), ix.end());
    intersections.emplace_back(ix[0], ix.back());
    }
  }

hyperpoint lownoise() {
  hyperpoint h;
  h[3] = 1;
  for(int i=0; i<3; i++)
    h[i] = randd() * 1e-2;
  return h;
  }

void delocate_balloon() {
  for(auto p: allpoints)
    p->loc = lownoise();
  }

map<cell*, int> dist;

void current_balloon() {
  auto ac = currentmap->allcells();
  map<cell*, data*> pts;
  
  for(auto c: ac) dist[c] = c->type != 6 ? 0 : 100;
  for(int i=0; i<50; i++)
  for(auto c: ac) forCellEx(c1, c) 
    dist[c1] = min(dist[c1], dist[c]+1);
  
  int maxdist = 0;
  for(auto c: ac) maxdist = max(maxdist, dist[c]);

  for(auto c: ac) {
    pts[c] = new data(lownoise());
    allpoints.push_back(pts[c]);
    }
  
  for(auto c: ac) forCellEx(c1, c)
    edges.emplace_back(pts[c], pts[c1]);

  for(auto c: ac) for(int i=0; i<c->type; i++) {
    cellwalker cw(c, i);
    array<data*, 3> res;
    int tdist = 0;
    for(int i=0; i<3; i++) {
      tdist += dist[cw.at];
      res[i] = pts[cw.at];
      cw += wstep;
      cw++;
      }
    if(cw != cellwalker(c, i)) throw hr_exception("wrong!");
    if(res[0] > res[1] && res[0] > res[2]) {
      allfaces.push_back(res);
      facecolor.push_back(gradient(0xFF0000, 0x00FF00, 0, tdist, maxdist*3));
      }
    }

  for(auto& f: allfaces) for(int i: {0,1,2})
    f[i]->tris.emplace_back(f[(i+1)%3], f[(i+2)%3]);

  rogueviz::rv_hook(hooks_drawcell, 50, [maxdist] (cell *c, const shiftmatrix& V) {
    for(int i=0; i<c->type; i++) {
      cellwalker cw(c, i);
      array<cell*, 3> res;
      int tdist = 0;
      for(int i=0; i<3; i++) {
        tdist += dist[cw.at];
        res[i] = cw.at;
        cw += wstep;
        cw++;
        }
      if(res[0] > res[1] && res[0] > res[2]) {
        curvepoint(C0);
        curvepoint(currentmap->adj(c, i) * C0);
        curvepoint(currentmap->adj(c, gmod(i-1, c->type)) * C0);
        queuecurve(V, (forecolor << 8) | 0xFF, gradient(0xFF0000FF, 0x00FF00FF, 0, tdist, maxdist*3), PPR::FLOOR);
        }
      }
    return false;
    });
  }

void init_balloon() {

  if(allpoints.empty()) {
    create_balloon();
    }

  rogueviz::rv_hook(shmup::hooks_turn, 100, physics);

  rogueviz::rv_hook(hooks_frame, 100, [] {
    shiftmatrix T = ggmatrix(currentmap->gamestart());
    
    if(draw_intersections) {
      for(auto p: intersections)
        queueline(T*p.first, T*p.second, 0x0000FFFF).prio = PPR::SUPERLINE;
      }
    
    shiftmatrix T1 = T;
    if(pmodel == mdDisk) T1[2][3] -= 0.01;
    for(auto e: edges)
      queueline(T1*e.first->loc, T1*e.second->loc, (forecolor << 8) | 0xFF).prio = PPR::WALL;
    int id = 0;
    for(auto f: allfaces) {
      curvepoint(f[0]->loc);
      curvepoint(f[1]->loc);
      curvepoint(f[2]->loc);
      if(part) {
        hyperpoint ctr = (f[0]->loc + f[1]->loc + f[2]->loc) / 3;
        curvepoint(f[0]->loc);
        curvepoint(lerp(ctr, f[0]->loc, part));
        curvepoint(lerp(ctr, f[1]->loc, part));
        curvepoint(lerp(ctr, f[2]->loc, part));
        curvepoint(lerp(ctr, f[0]->loc, part));
        }
      if(!anyshiftclick) queuecurve(T, 0, (facecolor[id++] << 8) | 0xFF, PPR::WALL);
      }
    });  

  rogueviz::rv_hook(hooks_handleKey, 50, [] (int sym, int uni) {
    if((cmode & sm::NORMAL) && uni == 'r') {
      dialog::editNumber(req_volume, 100, 1000000000, .1, 1000000, "", "");
      dialog::scaleLog();
      return true;
      }
    if((cmode & sm::NORMAL) && uni == 'e') {
      explore();
      return true;
      }
    if((cmode & sm::NORMAL) && uni == 'i') {
      draw_intersections = !draw_intersections;
      if(draw_intersections) compute_intersections();
      return true;
      }
    return false;
    });
  
  param_f(req_volume, "balloon_volume");  
  }

int t = 3;

struct hrmap_arbi_full : hrmap {
  vector<heptagon*> alls;
  heptagon *getOrigin() override { return alls[startid]; }
  
  hrmap_arbi_full() {
    auto& cur = arb::current;
    int N = isize(allfaces);
    alls.resize(N);
    
    println(hlog, "creating a map of size ", N);

    for(int i=0; i<N; i++) {
      auto& h = alls[i];
      h = tailored_alloc<heptagon> (t); /* some compilerbug happens when we put 3 directly?? */
      h->zebraval = i;
      h->c7 = newCell(3, h);
      h->alt = nullptr;
      h->cdata = nullptr;
      h->emeraldval = 0;
      h->distance = 0;
      }

    for(int i=0; i<N; i++) 
      for(int d=0; d<3; d++) {
        auto& con = cur.shapes[i].connections[d];
        alls[i]->c.connect(d, alls[con.sid], con.eid, con.mirror);
        }

    println(hlog, "finished, startid = ", startid);
    }

  transmatrix adj(heptagon *h, int dl) override { 
    if(h->c.move(dl))
      return arb::get_adj(arb::current_or_slided(), arb::id_of(h), dl, arb::id_of(h->c.move(dl)), h->c.spin(dl), h->c.mirror(dl));
    else
      return arb::get_adj(arb::current_or_slided(), arb::id_of(h), dl);
    }

  ld spin_angle(cell *c, int d) override { return SPIN_NOT_AVAILABLE; }

  int shvid(cell *c) override {
    return arb::id_of(c->master);
    }

  hyperpoint get_corner(cell *c, int cid, ld cf) override {
    auto& sh = arb::current_or_slided().shapes[arb::id_of(c->master)];
    int id = gmod(cid, c->type);
    return normalize(C0 + (sh.vertices[id] - C0) * 3 / cf);
    }

  ~hrmap_arbi_full() {
    clearfrom(getOrigin());
    }
  };

void explore() {
  auto& cur = arb::current;
  int N = isize(allfaces);

  ginf[gArbitrary].g = giEuclid2;
  ginf[gArbitrary].sides = 7;
  set_flag(ginf[gArbitrary].flags, qCLOSED, true);
  set_flag(ginf[gArbitrary].flags, qAFFINE, false);
  geom3::apply_always3();

  set_geometry(gCubeTiling);
  
  map<pair<data*, data*>, arb::connection_t > adjacent;

  for(int i=0; i<N; i++) {   
    auto& t = allfaces[i];
    for(int j=0; j<3; j++)
      adjacent[make_pair(t[(j+1)%3], t[j])] = arb::connection_t{i, j, 0};
    }

  arb::set_defaults(cur, false, "balloon");
  cur.shapes.resize(N);
  for(int i=0; i<N; i++) {   
    auto& cc = cur.shapes[i];
    cc.id = i;
    cc.vertices.resize(3);
    cc.flags = 0;
    cc.repeat_value = 1;
    auto& t = allfaces[i];
    hyperpoint cros = (t[1]->loc - t[0]->loc) ^ (t[2]->loc - t[0]->loc);
    cros /= hypot_d(3, cros);
    transmatrix T;
    if(1) {
      dynamicval<eGeometry> g(geometry, gSphere);
      T = gpushxto0(cros);
      for(int i=0; i<4; i++) T[3][i] = T[i][3] = i==3;
      }
    
    hyperpoint ctr = (t[0]->loc + t[1]->loc + t[2]->loc) / 3;
    recompute:
    for(int j=0; j<3; j++) {
      cc.vertices[j] = T * (t[j]->loc - ctr);
      // println(hlog, tie(i,j), " : ", cc.vertices[j]);
      cc.vertices[j][2] = 1;
      }
    
    cc.edges.resize(3);
    for(int j=0; j<3; j++) {
      cc.edges[j] = hdist(cc.vertices[j], cc.vertices[(j+1)%3]);
      }
    cc.angles.resize(3);
    for(int j=0; j<3; j++) cc.angles[j] = 1;

    ld area = ((cc.vertices[1]-cc.vertices[0]) ^ (cc.vertices[2]-cc.vertices[0])) [2];
    println(hlog, "area = ", area);
    if(area < 0) {
      T = MirrorX * T;
      println(hlog, "need to recompute triangle ", i);
      goto recompute;
      }
    
    cc.connections.resize(3);
    for(int j=0; j<3; j++)
      cc.connections[j] = adjacent.at(make_pair(t[j], t[(j+1)%3]));
    cc.stretch_shear.resize(3, make_pair(1, 0));
    }    
  
  set_geometry(gArbitrary);
  
  rogueviz::rv_hook(hooks_newmap, 0, [] {
    return new hrmap_arbi_full;
    });
  
  cur.cscale = 1;

  vid.cells_drawn_limit = 100000;
  canvas_default_wall = waNone;
  start_game();  
  ginf[geometry].distlimit[0] = cgi.base_distlimit = 20;
  pconf.scale = 1;
  
  hrmap_arbi_full *m = (hrmap_arbi_full*) currentmap;
  for(int i=0; i<isize(allfaces); i++) {
    setdist(m->alls[i]->c7, 0, nullptr);
    m->alls[i]->c7->landparam = facecolor[i];
    }
  }


auto shot_hooks = arg::add3("-ibal", init_balloon) + arg::add3("-ibalc", current_balloon) + arg::add3("-ibale", explore);

}
}
