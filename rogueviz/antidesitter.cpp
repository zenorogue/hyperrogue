// https://www.quora.com/Are-there-variations-of-Conways-Game-of-Life-based-upon-a-triangular-or-hexagonal-grid
// https://arunarjunakani.github.io/HexagonalGameOfLife/
// B2 S34

#include "../rogueviz/rogueviz.h"

namespace hr {

namespace ads {

vector<hyperpoint> hs;

int sq = 1;
ld sqf = sq;

int lim = 10;

struct star {
  hyperpoint a;
  hyperpoint b;
  color_t col;
  };

vector<star> stars;

bool ord(hyperpoint a, hyperpoint b) {
  for(int i=0; i<4; i++) if(a[i] != b[i]) return a[i] > b[i];
  return false;
  }

void gen_get() {
  for(auto g: gmatrix) hs.push_back(unshift(g.second*C0));
  }

void gen_rational() {
  dynamicval<eGeometry> g(geometry, gCubeTiling);

  map<int, vector<pair<int, int>>> roots;
  for(int t=-lim; t<=lim; t++)
  for(int u=-lim; u<=lim; u++) roots[t*t+u*u].emplace_back(t, u);
  
  for(int x=-lim; x<=lim; x++)
  for(int y=-lim; y<=lim; y++)
/*  for(int u=-lim; u<=lim; u++)
    if(t*t+u*u == x*x+y*y+sq*sq) */
  for(auto [t, u]: roots[x*x+y*y+sq*sq])
      hs.push_back(hyperpoint(x/sqf, y/sqf, t/sqf, u/sqf));
  println(hlog, "hs size = ", isize(hs));  
  }

void gen_stars() {
  map<hyperpoint, color_t> cols;
  for(auto h: hs) cols[h] = 0x000000FF | (hrand(0x1000000) << 8);

  for(auto h1: hs) if(ord(h1, Hypc)) 
  for(auto h2: hs) if(ord(h2, Hypc)) {
    hyperpoint h = h1 - h2;
    if(h[0]*h[0]+h[1]*h[1]-h[2]*h[2]-h[3]*h[3] == -2) if(hrand(100) < 100) {
      stars.emplace_back(star{h1, h2, gradient(cols[h1], cols[h2], 0, .5, 1)});

      hyperpoint hm = h1 * h2[3] - h2 * h1[3];
      ld d = hypot(h2[3], h1[3]);
      if(d == 0) continue;
      hm /= d;
      if(hm[2] < 0) hm = -hm;
      // println(hlog, hm);
      if(abs(hm[0]) < .1 && abs(hm[1]) < .1 && abs(hm[2]-1) < .1 && abs(hm[3]) < .1)
        println(hlog, h1, " & ", h2, " -> ", hm);

      }
    }
  
  println(hlog, "stars size = ", isize(stars));
  }
  
void init_ads() {
  }

color_t color_of(ld z) {
  z *= 2;
  if(z < -1) return 0;
  if(z > 1) return 0;
  if(z > 0) return gradient(0xFFFFFFFF, 0xFF000000, 0, z, 1);
  return gradient(0xFFFFFFFF, 0xFF00, 0, z, -1);
  }

transmatrix lorentz(int a, int b, ld v) {
  transmatrix T = Id;
  T[a][a] = T[b][b] = cosh(v);
  T[a][b] = T[b][a] = sinh(v);
  return T;
  }

transmatrix current = Id;

void draw_ads() {
  ld t = ticks / anims::period * 2 * M_PI;
  static ld last_t = t;
  no_find_player = true;
  
  if(1) {
    dynamicval<eGeometry> g(geometry, gCubeTiling);
    current = cspin(2, 3, t - last_t) * current;
    }
  last_t = t;
  
  if(stars.empty()) for(hyperpoint h: hs) {
    hyperpoint h1;
    if(1) {
      dynamicval<eGeometry> g(geometry, gCubeTiling);
      h1 = current * h;
      }
    hyperpoint h2 = h1; h2[2] = sqrt(h2[2] * h2[2] + h2[3] * h2[3]);
    if(h1[2] < 0) continue;
    color_t c = color_of(h1[3]);
    if(c) 
      queuepoly(shiftless(rgpushxto0(h2)), cgi.shGem[0], c);
    }

  for(auto st: stars) {
    
    hyperpoint hm;
    
    if(1) {
      dynamicval<eGeometry> g(geometry, gCubeTiling);
      hyperpoint h1 = current * st.a;
      hyperpoint h2 = current * st.b;
      hm = h1 * h2[3] - h2 * h1[3];
      ld d = hypot(h2[3], h1[3]);
      if(d == 0) continue;
      hm /= d;
      if(hm[2] < 0) hm = -hm;
      // ld ii = hm[2]*hm[2] - hm[0]*hm[0] - hm[1]*hm[1];
      }

    queuepoly(shiftless(rgpushxto0(hm)), cgi.shGem[0], st.col);
    }
  }

bool handleKey(int sym, int uni) {
  dynamicval<eGeometry> g(geometry, gCubeTiling);
  if(uni == 'a') { current = lorentz(0, 3, +.1) * current; return true; }
  if(uni == 'd') { current = lorentz(0, 3, -.1) * current; return true; }
  if(uni == 'w') { current = lorentz(1, 3, +.1) * current; return true; }
  if(uni == 's') { current = lorentz(1, 3, -.1) * current; return true; }
  return false;
  }

void run_ads() {

  rogueviz::rv_hook(hooks_frame, 100, draw_ads);
  rogueviz::rv_hook(hooks_handleKey, 0, handleKey);
  }

void auto_shift() {
   current = spin(30*degree) * lorentz(0, 3, 1) * spin(-30*degree) * current;
   }

map<cell*, vector<pair<cell*, int>> > causality;

map<cell*, int> cell_id;

set<cell*> cs;

int ms = 4;
int srange = 7;

map<cell*, shiftmatrix> where_matrix;

void check(cell *c, const shiftmatrix& S) {
  auto p = hybrid::get_where(c);
  if(causality.count(p.first)) return;
  auto& caus = causality[p.first];
  auto c1 = c;
  shiftmatrix S1 = S;
  for(int i=0; i<ms; i++) {
    c1 = c1->cmove(c->type - 2);
    S1 = S1 * currentmap->adj(c1, c->type-2);
    }
  
  where_matrix[p.first] = S;

  hybrid::in_underlying_geometry([&] {
    setdist(p.first, 4, nullptr);
    p.first->land = laIce;
    });
    
  hybrid::test_at(p.first);

  // time increases
  auto p1 = hybrid::get_where(c1);
  println(hlog, p.second, " -> ", p1.second);
  for(int i=0; i<c->type-2; i++) {
    cell *c2 = c1->cmove(i);
    auto p2 = hybrid::get_where(c2);
    bool ok = cs.count(p2.first);
    hybrid::in_underlying_geometry([&] {
      ok = celldist(p2.first) <= srange;
      });
    if(!ok) continue;
    caus.emplace_back(p2.first, p.second - p2.second);
    // (p,x) (p2,0) -> okay
    // minimum p1 above p2
    check(c2, S1 * currentmap->adj(c1, i));
    }
  }

map<cell*, int> noncausal;

bool randlive() { return hrand(100) < 20 ? 1 : 0; }

vector<cell*> compute_order;
map<cell*, int> index;

void fine() {
  compute_order.clear();
  index.clear();
  set<cell*> unknown;
  for(auto& p: causality) unknown.insert(p.first);
  while(true) {
    vector<cell*> to_remove;
    println(hlog, "unknown = ", isize(unknown));
    if(unknown.empty()) break;
    for(auto u: unknown) {
      bool ok = true;
      for(auto pp: causality[u]) {
        int need = noncausal[u] + 1 - pp.second;
        if(noncausal[pp.first] < need) ok = false;
        }
      if(ok) {
        to_remove.push_back(u);
        noncausal[u]++;
        }
      }
    if(to_remove.empty()) { println(hlog, "failed to remove"); return; }
    for(auto r: to_remove) unknown.erase(r), compute_order.push_back(r);
    }
  
  int ids = 0;
  for(auto& p: compute_order) index[p] = ids++;
  vector<vector<pair<int, int>>> relative(ids);
  for(auto p: compute_order) {
    int i = index[p];
    for(auto& pp: causality[p]) {
      int j = index[pp.first];
      int k = noncausal[p] - pp.second - noncausal[pp.first];
      relative[i].emplace_back(j, k);
      }
    }
  println(hlog, relative);
  
  vector<int> unknown_neighbors(ids);
  for(auto& p: compute_order) unknown_neighbors[index[p]] = p->type - isize(relative[index[p]]);

  vector<int> noncausal_indexed(ids);
  for(auto& p: compute_order) noncausal_indexed.push_back(noncausal[p]);
  
  vector<vector<int>> ca_states;
  
  println(hlog, "generating initial random");
  for(int t=0; t<ms; t++) {
    ca_states.emplace_back();
    ca_states.back().resize(ids);
    for(int j=0; j<ids; j++) ca_states[t][j] = randlive();
    }

  vector<int> lifecounts;
  
  int steps = 50;
  for(int t=ms; t<steps; t++) {
    println(hlog, "generating step t=", t);
    ca_states.emplace_back();
    ca_states.back().resize(ids);
    int countlife = 0;
    for(int j=0; j<ids; j++) {     
      int lives = 0;
      bool early = false;
      for(auto [id, d]: relative[j]) {
        if(t+d < 0) early = true;
        else lives += ca_states[t+d][id];
        }
      if(early) {
        ca_states[t][j] = randlive();
        continue;
        }
      for(int i=0; i<unknown_neighbors[j]; i++) lives += randlive();
      int old = ca_states[t-ms][j];
      int newstate;
      if(old == 0) newstate = lives == 2 ? 1 : 0;
      if(old == 1) newstate = (lives == 3 || lives == 4) ? 1 : 0;
      ca_states[t][j] = newstate;
      countlife += newstate;
      }
    lifecounts.push_back(countlife);
    }

  for(int t=0; t<steps; t++)
    for(int j=0; j<ids; j++) {
      cell *h = hybrid::get_at(compute_order[j], noncausal_indexed[j] + t);
      setdist(h, 7, nullptr);
      if(t < ms) h->land = laIce;
      else if(t < 2*ms) h->land = laJungle;
      else h->land = laAlchemist;
      h->wall = ca_states[t][j] ? waPlatform : waNone;
      }

  println(hlog, lifecounts);
  }

void reset_to_underlying(cell *where) {
  hrmap *m;
  hybrid::in_underlying_geometry([&] { m = currentmap; });
  currentmap = m;
  geometry = hybrid::underlying;
  pmodel = mdDisk;
  cwt = centerover = where; View = Id;
  check_cgi();
  resetview();
  }

void ads_ca_check() {

  auto pc = hybrid::get_where(cwt.at).first;
  hybrid::in_underlying_geometry([&] {
    cs.insert(pc);
    cs.insert(pc->cmove(0));
    cs.insert(pc->cmove(1));
    });
  for(auto cc: cs)
  println(hlog, "SET = ", cc);

  check(cwt.at, shiftless(Id));

  println(hlog, causality);
  // at step t, for (c,i) in noncausal, change (c,i+t)
  for(auto p: causality) noncausal[p.first] = 0;
  vector<int> ncs;
  map<cell*, cell*> source;
  vector<int> changelist;
  for(int it=0; it<1000; it++) {
    int changes = 0;
    for(auto p: causality) 
      for(auto pp: p.second) {
        auto& here = noncausal[p.first];
        auto cand = noncausal[pp.first] + pp.second;
        if(cand < here) {
          source[p.first] = pp.first;
          here = cand;
          changes++;
          }
        }
    ncs.push_back(noncausal[pc]);
    if(it == 998) println(hlog, noncausal);
    if(it == 999) println(hlog, noncausal);
    changelist.push_back(changes);
    if(!changes) break;
    }
  println(hlog, ncs);
  
  cell *x = pc;
  for(int it=0; it<1000; it++) if(x) x = source[x];
  if(!x) { println(hlog, "all fine!"); fine(); return; }
  auto x1 = x;
  vector<cell*> circ;
  int dsum = 0;
  while(true) {
    x->item = itDiamond;
    for(auto pp: causality[x]) if(pp.first == source[x]) {
      dsum += pp.second - ms;
      println(hlog, "delta = ", pp.second);
      }
    circ.push_back(x);
    x = source[x];
    if(x == x1) break;
    }    
  int circum = isize(circ);
  println(hlog, "circum = ", circum);
  int anglesum = 0;
  for(int i=0; i<circum; i++) {
    int d1 = neighborId(circ[i], circ[gmod(i+1, circum)]);
    int d2 = neighborId(circ[i], circ[gmod(i-1, circum)]);
    int d = gmod(d2 - d1, circ[i]->type);
    // println(hlog, "angle = ", d);
    anglesum += d;
    }
  println(hlog, "anglesum = ", anglesum);
  int area = 7 * (circum - 2) - 2 * anglesum;
  println(hlog, "area = ", area);
  println(hlog, "dsum = ", dsum);

  reset_to_underlying(x1);
  }

hyperpoint findflat(shiftpoint S0) {
  hyperpoint h = S0.h;
  ld at = atan2(h[3], h[2]);
  h = cspin(2, 3, at) * cspin(0, 1, at) * h;
  return h;
  }

bool view_ads_ca() {
  if(1) {
    dynamicval<eGeometry> g(geometry, gNormal);
    dynamicval<eModel> p(pmodel, mdDisk);
    check_cgi();
    initquickqueue();
    for(int i=0; i<isize(compute_order); i++) {
      hyperpoint h;
      if(1) {
        dynamicval<eGeometry> b(geometry, gCubeTiling);
        cell *c = compute_order[i];
        shiftmatrix S = where_matrix[c];
        h = findflat(S * C0);
        }
      queuepoly(shiftless(rgpushxto0(h)), cgi.shGem[0], 0xFFD500FF);
      }
    quickqueue();
    }
  check_cgi();
  return true;
  }

void run_ads_ca_view() {
  nomap = true;
  no_find_player = true;
  rogueviz::rv_hook(hooks_prestats, 100, view_ads_ca);
  }

auto shot_hooks = arg::add3("-ads", run_ads) + arg::add3("-ads-rat", gen_rational) + arg::add3("-ads-stars", gen_stars) + arg::add3("-ads-get", gen_get) + arg::add3("-ads-shift", auto_shift)
  + arg::add3("-ads-ca-check", ads_ca_check) + arg::add3("-ads-ca-view", run_ads_ca_view);

}
}
