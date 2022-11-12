// some CA suggestions:
// https://www.quora.com/Are-there-variations-of-Conways-Game-of-Life-based-upon-a-triangular-or-hexagonal-grid
// https://arunarjunakani.github.io/HexagonalGameOfLife/

// Creating a roguelike in 2+1D anti-de Sitter spacetime seems hard, so let's start with a cellular automaton. The rules are like Conway's Game of life, except that hex tile are born at 2 neighbors and overcrowded at 3. (1/5)
// ./hyper ads_srange=7 -shothud 1 -csc .4 -rotspace -canvas-random 10 -prodperiod 0 -ads-ca-check -ads-ca-view -noscr ads_simspeed=3 -ads-keys -ads-keylist xaxwxsxdx -ads-keylist 0 -zoom .95 -shot-1000 -animvideo 1800 ads-ca.mp4

// However, it takes 4 iterations for neighbors (brown) to affect a cell (red). (Information cannot travel faster than c -- with <4 iterations cells would depend on their own future.) The blue 'plague' shows which cells are affected by the original source. (2/5)
// ./hyper ads_plague=221 ads_srange=7 -shothud 1 -csc .4 -rotspace -canvas-random 10 -prodperiod 0 -ads-ca-check -ads-ca-view -noscr ads_simspeed=3 -ads-keys -ads-keylist xaxwxsxdx -ads-keylist 0 -zoom .95 -shot-1000 -ads-highlight -animvideo 1800 ads-ca-plague.mp4 -exit

// Here is how the situation changes when we don't just sit on a star and try to travel. In this viz we see the situation "at the current time" (of course IRL we would see the past states). (It is more fun to play with when you can control it manually, of course.) (3/5)
// ./hyper ads_accel=20 ads_srange=7 -shothud 1 -csc .4 -rotspace -canvas-random 10 -prodperiod 0 -ads-ca-check -ads-ca-view -noscr ads_simspeed=6 -ads-keys -ads-keylist xaxwxxxxxaxwx -zoom .95 -shot-1000 -animvideo 3600 ads-ca-move.mp4 -exit

// (Why would we want a roguelike in adS spacetime? In our world we do not feel whether we are moving on not; in the world of HyperRogue, this is not the case. https://twitter.com/ZenoRogue/status/1418957609269440512) (4/5)

// (This why in HyperRogue we have long but narrow snakes, and ivies which grow, but nothing wider than a Kraken could move. This is because in HyperRogue time is added in in Euclidean way; anti-de Sitter spacetime adds time to the hyperbolic plane in a "better" way.) (5/5)

// [https://twitter.com/ZenoRogue/status/1538874019948150790] Actually it would not remain in the same space (i.e., that is not a timelike geodesic) -- see the star in the center of the simulation here, it starts without movement relative to C (unmarked center), but then "wobbles". [This animation is faster than the ones in the thread]
// [moved the center manually using ijkl, then enter to animate]
// ./hyper -shothud 1 ads_srange=7 -csc .4 -rotspace -canvas-random 10 -prodperiod 0 -ads-ca-check -ads-ca-view -noscr -ads-keys -run ads_simspeed="2*pi" -zoom .95 -shot-1000 -animvideo 1800 ads-ca-faster.mp4 -exit

// [https://twitter.com/ZenoRogue/status/1539871748950315010] However, we could render the hyperbolic plane using Beltrami-Klein projection (as below, contrary to Poincar‚ in other animations), then Beltrami-Klein stretching counterbalances the Lorentz contraction and we get (rotating) hyperbolic plane in Poincar‚ model.
// ./hyper ads_srange=7 -shothud 1 -csc .4 -rotspace -canvas-random 10 -prodperiod 0 -ads-ca-check -ads-ca-view -noscr ads_simspeed=3 -ads-keys -ads-keylist xaxwxsxdx -ads-keylist 0 -zoom .95 -shot-1000 -alpha 0 -animvideo 1800 ads-ca-klein.mp4



#include "../rogueviz/rogueviz.h"

namespace hr {

namespace ads {

shiftpoint kz(shiftpoint x) { x.h = hr::kz(x.h); x.shift = hr::kz(x.shift); return x; }
shiftmatrix kz(shiftmatrix x) { x.T = hr::kz(x.T); x.shift = hr::kz(x.shift); return x; }

vector<hyperpoint> hs;

int sq = 1;
ld sqf = sq;

int lim = 10;

int dispmode;

struct star {
  hyperpoint a;
  hyperpoint b;
  color_t col;
  };

vector<star> stars;

ld simspeed = 0;
ld accel = 6;

/* how many steps to generate */
int numsteps = 1000;

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

shiftmatrix current = shiftless(Id);

/* this is uzpush(-x) */
transmatrix chg_shift(ld x) {
  return cspin(2, 3, x) * cspin(0, 1, x);
  }

char forcekey;

shiftmatrix apply_lorentz(shiftmatrix S, transmatrix lor) {
  S.T = chg_shift(-S.shift) * lor * chg_shift(S.shift) * S.T;
  return S;
  }

string keylist = "0"; // xaxdxsxwx";

ld ang = 0;
bool acc = false;

void pass_time() {
  ld t = ticks / anims::period;
  static ld last_t = t;
  if(t < last_t) last_t = 0;
  no_find_player = true;
  
  char forcekey = keylist[frac(t) * isize(keylist)];
  acc = false;
  
  if(true) {
    ld delta = t - last_t;
    dynamicval<eGeometry> g(geometry, geometry == gRotSpace ? geometry : gCubeTiling);

    const Uint8 *keystate = SDL12_GetKeyState(NULL);
    if(keystate['a'] || forcekey == 'a') current = apply_lorentz(current, lorentz(0, 2, delta*accel)), ang = 180, acc = true;
    if(keystate['d'] || forcekey == 'd') current = apply_lorentz(current, lorentz(0, 2, -delta*accel)), ang = 0, acc = true; 
    if(keystate['w'] || forcekey == 'w') current = apply_lorentz(current, lorentz(1, 2, delta*accel)), ang = 90, acc = true; 
    if(keystate['s'] || forcekey == 's') current = apply_lorentz(current, lorentz(1, 2, -delta*accel)), ang = 270, acc = true; 

    current.T = cspin(3, 2, delta * simspeed) * current.T;
    optimize_shift(current);    
    hassert(eqmatrix(chg_shift(current.shift) * current.T, unshift(current)));
    }

  last_t = t;
  }

void draw_ads() {
  pass_time();
  
  if(stars.empty()) for(hyperpoint h: hs) {
    hyperpoint h1;
    if(1) {
      dynamicval<eGeometry> g(geometry, gCubeTiling);
      h1 = unshift(current * h);
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
      hyperpoint h1 = unshift(current * st.a);
      hyperpoint h2 = unshift(current * st.b);
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
  println(hlog, simspeed);
  if(uni == 'p') { simspeed += .01; return true; }
  if(uni == 'b') { simspeed -= .01; return true; }
  if(uni == 'z') { simspeed = 0; return true; }
  if(uni == 'x') { dispmode++; dispmode %= 4; return true; }
  if(among(uni, 'a', 'd', 's', 'w')) return true;
  
  if(uni == 'j') { current = apply_lorentz(current, lorentz(0, 3, .1)); return true; }
  if(uni == 'l') { current = apply_lorentz(current, lorentz(0, 3, -.1)); return true; }
  if(uni == 'i') { current = apply_lorentz(current, lorentz(1, 3, .1)); return true; }
  if(uni == 'k') { current = apply_lorentz(current, lorentz(1, 3, -.1)); return true; }
  /*
  if(uni == 'a') { current = apply_lorentz(current, lorentz(0, 2, +.1)); return true; }
  if(uni == 'd') { current = apply_lorentz(current, lorentz(0, 2, -.1)); return true; }
  if(uni == 'w') { current = apply_lorentz(current, lorentz(1, 2, +.1)); return true; }
  if(uni == 's') { current = apply_lorentz(current, lorentz(1, 2, -.1)); return true; } */
  return false;
  }

void auto_shift() {
   current.T = spin(30._deg) * lorentz(0, 3, 1) * spin(-30._deg) * current.T;
   }

/* in our cellular automaton, each tile is affected by its own history, and the spatial neighbors of its ms-step history */
/* ms==3 works for {7,3}; ms==4 works for bitruncated {7,3} */
int ms = 4;
/* the simulation range for the CA */
int srange = 5;

/* (c1,k) in causality(c) means that get_at(c,n) is affected by get_at(c1,n-k) */
map<cell*, vector<pair<cell*, int>> > causality;

/* for every fiber f, where_matrix[f] equals ggmatrix(get_at(f, base_level[f])) */
map<cell*, shiftmatrix> where_matrix;
map<cell*, int> base_level;

vector<pair<cell*, shiftmatrix>> checklist;

cell *pc0;

void check_all() {
  for(int i=0; i<isize(checklist); i++) {
    auto c = checklist[i].first;
    auto S = checklist[i].second;

    auto p = hybrid::get_where(c);
    if(causality.count(p.first)) continue;
    auto& caus = causality[p.first];
    auto c1 = c;
    optimize_shift(S);
    shiftmatrix S1 = S;
    // println(hlog, "we are : ", kz(S), " plevel = ", cgi.plevel);
    for(int i=0; i<ms; i++) {
      c1 = c1->cmove(c->type - 2);
      S1 = S1 * currentmap->adj(c1, c->type-2);
      optimize_shift(S1);
      // println(hlog, "below  : ", kz(S1));
      }
    
    where_matrix[p.first] = S;
    base_level[p.first] = p.second;
  
    hybrid::in_underlying_geometry([&] {
      setdist(p.first, 4, nullptr);
      p.first->land = laIce;
      });
      
    // hybrid::test_at(p.first);
  
    // time increases
    auto p1 = hybrid::get_where(c1);
    hassert(p1.second == p.second - ms);
    for(int i=0; i<c->type-2; i++) {
      cell *c2 = c1->cmove(i);
      auto p2 = hybrid::get_where(c2);
      bool ok;
      hybrid::in_underlying_geometry([&] {
        ok = celldist(p2.first) <= srange;
        });
      if(!ok) continue;
      caus.emplace_back(p2.first, p.second - p2.second);
      // (p,x) (p2,0) -> okay
      // minimum p1 above p2

      if(0) {
        shiftmatrix below = S1 * currentmap->adj(c1, i);
        optimize_shift(below);
        println(hlog, "for ", c->move(i));
        println(hlog, "matrix below is: ", kz(below));
        below = S * currentmap->adj(c1, i);
        optimize_shift(below);
        println(hlog, "matrix here  is: ", kz(below));
        }
      // go below
      checklist.emplace_back(c2, S1 * currentmap->adj(c1, i));

      // keep level
      // checklist.emplace_back(c->move(i), S * currentmap->adj(c1, i));
      }
    println(hlog, "for ", p.first, " base level is ", p.second, " and causality is ", caus);
    }
  
  }

/* {get_at(f,noncausal[f]): fiber f} is a set of cells such that none depends on any other */
map<cell*, int> noncausal;

bool randlive() { return hrand(100) < 20 ? 1 : 0; }

vector<cell*> compute_order;
/* inverse of compute_order */
map<cell*, int> index;

/* in each iteration i, for every k (increasing), we compute the state of get_where(compute_order[k], noncausal[k]+i) */
/* this is ca_state[i][k]; we never refer to states where (i,k) is greater this way */

vector<vector<int>> ca_states;
vector<vector<pair<int, int>>> relative;

int plague = -1;

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
        println(hlog, "set noncausal of ", u, " to ", noncausal[u]);
        }
      }
    if(to_remove.empty()) { println(hlog, "failed to remove"); return; }
    for(auto r: to_remove) unknown.erase(r), compute_order.push_back(r);
    }
  
  int ids = 0;
  for(auto& p: compute_order) index[p] = ids++;
  relative.clear();
  relative.resize(ids);
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
  
  println(hlog, "generating initial random");
  for(int t=0; t<ms; t++) {
    ca_states.emplace_back();
    ca_states.back().resize(ids);
    for(int j=0; j<ids; j++) ca_states[t][j] = randlive();
    }

  vector<int> lifecounts;
  
  int steps = numsteps;
  for(int t=ms; t<steps; t++) {
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
      int old = ca_states[t-1][j];
      int newstate = old;
      if(isize(relative[j]) + unknown_neighbors[j] == 6 && lives >= 2) lives++;
      if(old == 0) newstate = lives == 3 ? 1 : 0;
      if(old == 1) newstate = (lives == 3 || lives == 2) ? 1 : 0;
      if(lives >= 10) newstate = 10;
      if(j == index[pc0] && t == plague) {
        println(hlog, "set 10 at ", tie(j, t));
        newstate = 10;
        }
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
  pc0 = pc;

  checklist.emplace_back(cwt.at, shiftless(Id));
  check_all();

  println(hlog, "causality is: ", causality);
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

ld get_shift_cycles(ld shift) {
  return floor(shift / TAU + .5) * TAU;
  }

shiftpoint shift_perfect_mul(shiftmatrix T, shiftpoint h) {
  ld sh = get_shift_cycles(h.shift);
  h.shift -= sh;
  shiftpoint res;
  for(int i=0; i<=10; i++) {
    shiftpoint cand = T * chg_shift(h.shift * i / 10.) * h.h;

    // println(hlog, "shift check: ", eqmatrix(chg_shift(current.shift) * current.T, unshift(current)));

    optimize_shift(cand);
    
    if(i > 0) {
      cyclefix(cand.shift, res.shift);
      }
    res = cand;    
    }
  res.shift += sh;
  return res;
  }

shiftpoint bshift(shiftpoint h, ld sh) {
  h.shift += sh; return h;
  }

struct flatresult {
  hyperpoint h;
  ld shift;
  };

flatresult findflat(shiftpoint h) {
  optimize_shift(h);

  shiftpoint hz = shift_perfect_mul(current, h);
  
  ld d = binsearch(-M_PI - hz.shift, M_PI - hz.shift, [&] (ld x) { return shift_perfect_mul(current, bshift(h, x)).shift > 0; }, 20);
  
  shiftpoint h1 = shift_perfect_mul(current, bshift(h, d));
  
  if(abs(h1.shift) > .1) {
    println(hlog, "h=", h, " current=", current);
    println(hlog, "shift = ", h1.shift, " for d = ", d, " h1 = ", h1);
    for(ld d=-10; d<=10; d+=.5)
      println(hlog, "for d = ", d, " h1 = ", kz(shift_perfect_mul(current, bshift(h, d))));
    exit(1);
    }
  
  hyperpoint h2 = h1.h;

  tie(h2[2], h2[3]) = make_pair(h2[3], -h2[2]);
  
  return flatresult{h2, d};

/*
  hyperpoint h = unshift(S0);
  transmatrix deg90 = cspin(2, 3, 90._deg) * cspin(0, 1, 90._deg);
  hyperpoint h0 = unshift(current) * h;
  hyperpoint h90 = unshift(current) * deg90 * h;

  // h = h0 * cos(t) + h90 * sin(t)
  // 0 == h0[3] * cos(t) + h90[3] * sin(t);
  // 0 == h0[3] + h90[3] * tan(t);
  ld cost, sint, tant;
  ld t;
  
  if(h90[3]) {
    tant = - h0[3] / h90[3];
    cost = 1 / sqrt(1 + tant * tant);
    sint = tant * cost;
    t = atan2(sint, cost);
    }
  else {
    cost = 0;
    sint = 1;
    t = 90._deg;
    }
  
  hyperpoint h2 = h0 * cost + h90 * sint;
  if(h2[2] < 0) { h2 = -h2; t += M_PI; }
  if(0) {
    ld at = atan2(h[3], h[2]);
    hyperpoint h1 = cspin(2, 3, at) * cspin(0, 1, at) * h;
    println(hlog, h2, " vs ", h1);
    if(sqhypot_d(4, h2-h1) > 1) {
      println(hlog, "h   = ", kz(h));
      println(hlog, "h0  = ", kz(h0));
      println(hlog, "h90 = ", kz(h90));
      println(hlog, "tant = ", tant);
      println(hlog, "cost = ", cost);
      println(hlog, "sint = ", sint);
      println(hlog, "at   = ", at, " : ", make_tuple(cos(at), sin(at), tan(at)));
      println(hlog, "h1  = ", kz(h1));
      println(hlog, "h2  = ", kz(h2));
      exit(1);
      }
    }
  
  return {h2, current.shift + t + cycles};
*/
  }

ld fracc(ld x) {
  x -= floor(x);
  if(x < .2) return ilerp(0, .2, x);
  if(x > .8) return ilerp(1, .8, x);
  return 1;
  }

int who = -1;

bool view_ads_ca() {
  pass_time();
  auto plev = cgi.plevel; /* we are in another CGI so we have no access to that... */
  
  if(1) {
    dynamicval<eGeometry> g(geometry, gNormal);
    dynamicval<eModel> p(pmodel, mdDisk);
    println(hlog, "zoom = ", pconf.scale);
    check_cgi();
    initquickqueue();
    println(hlog, "curvedata size = ", isize(curvedata), " curvestart = ", curvestart);
    calcparam();
    clearaura();
    for(int i=0; i<isize(compute_order); i++) {
    
      cell *c = compute_order[i];
      auto findflat_decode = [&] (shiftpoint h) {
        flatresult x = findflat(h);
        x.shift = base_level[c] + .5 + x.shift / plev - noncausal[c];
        return x;
        };

      flatresult center;
      vector<flatresult> hlist;

      color_t statecolor;
      if(1) {
        dynamicval<eGeometry> b(geometry, gRotSpace);
        shiftmatrix S = where_matrix[c];
        for(int i=0; i<=c->type; i++) {
          hyperpoint ha = hybrid::get_corner(c, i, 2, 0);
          hlist.push_back(findflat_decode(S * ha));
          }
        center = findflat_decode(S * C0);
        }
      
      // queuepolyat(shiftless(rgpushxto0(h)), cgi.shGem[0], 0xFFD500FF, PPR::LINE);
      
      auto get_statecolor = [&] (int t) {      
        if(t < 0) return 0xFF0000FFu;
        else if(t >= isize(ca_states)) return 0xFF00FFFFu;
        else {
          if(who != -1) {
            if(i == who && (t & 15) == 0)
              return 0xFF0000u;
            if(i == who && (t & 15) == 15)
              return 0xFFC000u;
            for(auto xx: relative[who])
              if(xx.first == i && (((t-xx.second)&15) == 0))
                return 0xFF8000u;
            }
          if(ca_states[t][i] == 2)
            return 0xFF0000u;
          else if(ca_states[t][i] == 10)
            return 0x2020F0u;
          else if(ca_states[t][i] == 1)
            return 0xFFFF40u;
          else
            return 0x808080u;
          }
        };
      
      ld minv = hlist[0].shift;
      ld maxv = hlist[0].shift;
      for(auto& h: hlist) {
        ld v = h.shift;
        if(v < minv) minv = v;
        if(v > maxv) maxv = v;
        }
      
      if(1) {
        for(int v=floor(minv); v<maxv+1; v++) {
          auto draw_slice = [&] (ld a, ld b, color_t col) {
            vector<hyperpoint> slice;
            for(int i=0; i<c->type; i++) {
              if(hlist[i].shift >= a && hlist[i].shift <= b)
                slice.push_back(hlist[i].h);
              if((hlist[i].shift < a) ^ (hlist[i+1].shift < a)) {
                ld p = ilerp(hlist[i].shift, hlist[i+1].shift, a);
                hyperpoint h1 = lerp(hlist[i].h, hlist[i+1].h, p);
                slice.push_back(h1);
                }
              if((hlist[i].shift > b) ^ (hlist[i+1].shift > b)) {
                ld p = ilerp(hlist[i].shift, hlist[i+1].shift, b);
                hyperpoint h1 = lerp(hlist[i].h, hlist[i+1].h, p);
                slice.push_back(h1);
                }
              if(hlist[i+1].shift < a && hlist[i].shift > b)
                swap((&slice.back())[-1], slice.back()); 
              }
            if(isize(slice) < 3) return;
            for(auto e: slice) curvepoint(e);
            curvepoint(slice[0]);
            queuecurve(shiftless(Id), 0, col, PPR::LINE);
            };
          draw_slice(v-.1, v+.1, 0xFF);
          draw_slice(v+.1, v+.9, (gradient(0, get_statecolor(v), 0, .5, 1) << 8) | 0xFF);
          }        
        addaura(shiftless(center.h), get_statecolor(center.shift), 0);
        }
      
      for(auto& p: hlist) curvepoint(p.h);
      queuecurve(shiftless(Id), 0xFFFFFFFF, 0, PPR::LINE);
      
      switch(dispmode) {
        case 0: {
          int bri = fracc(center.shift) * 255;
          color_t stat = get_statecolor(center.shift);
          color_t col = (stat << 8) | bri;
          poly_outline = (stat << 8) | (bri/2);
          queuepoly(shiftless(rgpushxto0(center.h)), cgi.shGem[ctof(c)], col);
          break;
          }
        
        case 1: {
         string str = its(floor(center.shift));
         queuestr(shiftless(rgpushxto0(center.h)), .3, str, gradient(0, 0xFFFFFF, 0, fracc(center.shift), 1), 8);
         break;
         }
        
        case 2: {
          string str = fts(center.shift); // str = lalign(0,c);      
          queuestr(shiftless(rgpushxto0(center.h)), .3, str, gradient(0, 0xFFFFFF, 0, fracc(center.shift), 1), 8);
          break;
          }
        
        case 3: {
          string str = lalign(0,c);
          queuestr(shiftless(rgpushxto0(center.h)), .3, str, gradient(0, 0xFFFFFF, 0, fracc(center.shift), 1), 8);
          break;
          }
        }
      addaura(shiftless(center.h), statecolor, 0);
      }
    if(acc) {
      poly_outline = 0xFF;
      queuepolyat(shiftless(spin(ang*degree) * Id), cgi.shEagle, 0x2020FFFF, PPR::LINE);
      }
    quickqueue();
    drawaura();
    }
  check_cgi();
  println(hlog, "plev = ", plev);
  return true;
  }

void run_ads_ca_view() {
  nomap = true;
  no_find_player = true;
  rogueviz::rv_hook(hooks_prestats, 100, view_ads_ca);
  }

auto shot_hooks = 
  arg::add3("-ads-display", [] { rogueviz::rv_hook(hooks_frame, 100, draw_ads); })
+ arg::add3("-ads-keys", [] { rogueviz::rv_hook(hooks_handleKey, 0, handleKey); })
+ arg::add3("-ads-rat", gen_rational) + arg::add3("-ads-stars", gen_stars) + arg::add3("-ads-get", gen_get) + arg::add3("-ads-shift", auto_shift)
+ arg::add3("-ads-ca-check", ads_ca_check) + arg::add3("-ads-ca-view", run_ads_ca_view)
+ arg::add3("-ads-keylist", [] { arg::shift(); keylist = arg::args(); })
+ arg::add3("-ads-highlight", [] { who = index[pc0]; })
+ addHook(hooks_configfile, 100, [] {
    param_f(simspeed, "ads_simspeed");
    param_f(accel, "ads_accel");
    param_i(dispmode, "ads_dispmode");
    param_i(srange, "ads_srange");
    param_i(plague, "ads_plague");
    param_i(numsteps, "ads_numsteps");
    param_i(ms, "ads_ms");
    });

}
}
