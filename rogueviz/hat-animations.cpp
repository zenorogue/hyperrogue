// RogueViz: hat animations
// Copyright (C) 2011-2023 Zeno Rogue, see 'hyper.cpp' for details

/*

// compile with mymake -O3 -rv rogueviz/hat-animations, then:

// for the animations in https://twitter.com/ZenoRogue/status/1639644061823819777 :

// Dual animations (does not actually use this module; this will actually all generate fixed versions since this is probably what you want anyway)

hyper -geo hat -canvas i -palrgba dual ffffffff -canvas i -noplayer -smart 1 "dual_length=0.1" "hat_param=(1+sin(0..2*pi))" -animperiod 10000 dual_angle=0..3600 -zoom 0.3 -shot-1000 -lw 10 -shotaa 2 -gridon -stdgrid a00000ff # -animvideo 600 dual-anim.mp4 # "hat_param=1+sin(0..2*pi)" -
hyper -geo hat -canvas i -palrgba dual ffffffff -canvas i -noplayer -smart 1 "dual_length=0.1" "hat_param=(1+sin(0..2*pi))" -animperiod 10000 dual_angle=0..3600 -zoom 0.3 -shot-1000 -lw 10 -shotaa 2 -gridon -stdgrid a00000ff -animvideo 600 dual-anim-hatparam.mp4
hyper -geo hat -canvas i -palrgba dual ff -back ffffff -canvas i -noplayer -smart 1 "dual_length=0.2" -animperiod 2000 dual_angle=0..360 -zoom 0.15 -shot-1000 -lw 20 -shotaa 2 -animvideo 120 dual-anim-short-loop.mp4
hyper -geo hat -canvas i -palrgba dual ffffffff -canvas i -noplayer -smart 1 "dual_length=0.2*(1-cos(0..2*pi))" -animperiod 100000 dual_angle=0..3600 -zoom 0.3 -shot-1000 -lw 10 -shotaa 2 -gridon -stdgrid 300000ff -animvideo 600 dual-anim-fixed.mp4

// "straight lines" (called 'Othello' because of this: https://twitter.com/ZenoRogue/status/1639916929862254594)

hyper -geo hat -canvas X -othello -smart 1 -genlimit 1000000 -smartlimit 1000000 global_boundary_ratio=0.2 -zoom "exp(0../0..-4../0)" "a=exp(0../0..7../0)" -shot-500 -shotaa 2 -noplayer -animvideo 600 othello.mp4
hyper -geo hat -canvas X -othello -smart 1 -genlimit 1000000 -smartlimit 1000000 global_boundary_ratio=0.2 -zoom "exp(0../0..-4../0)" "a=exp(0../0..7../0)" -shot-500 -shotaa 2 -noplayer -othello-next -1 -animvideo 600 othello-split.mp4
hyper -geo hat -canvas X -othello -smart 1 -genlimit 1000000 -smartlimit 1000000 global_boundary_ratio=0.2 -zoom "exp(0../0..-4../0)" "a=exp(0../0..7../0)" -shot-500 -shotaa 2 -noplayer -othello-next -1 -othello-swap-ppair -animvideo 600 othello-split-swapped.mp4

// zoom-in (called 'hat-descent')

hyper -canvas i -geo hat -hat-descent -hat-normal -smart 0.001 -smartlimit 1000000000 -genlimit 1000000000 -lw 2 perfect_linewidth=0 -zoom "exp(-5.5..-1../0)" -shot-1000 -animvideo 1800 hat-atoms.mp4
hyper -canvas i -geo hat -hat-descent -smart 0.001 -smartlimit 1000000000 -genlimit 1000000000 -lw 2 perfect_linewidth=0 -zoom "exp(-5.5..-1../0)" -shot-1000 -animvideo 1800 hat-atoms-aperiodic.mp4
hyper -canvas i -geo hat -hat-descent -smart 0.001 -smartlimit 1000000000 -genlimit 1000000000 -lw 2 perfect_linewidth=0 -run -hat-normal -hat-shift 2 -hat-ori -zoom "exp(-4.5..-1../0)" hat_param="1+cos(0..10*pi)" -shot-1000 -animvideo 1800 hat-atoms-colored.mp4

*/

#include "rogueviz.h"

namespace hr {

// ---othello ---

ld w = 10;

int next = 7;

// set next to -1 for this...
array<vector<int>, 2> ppair = {
  vector<int>{{7, 6, 10, 11, 12, 9, 1, 0, 13, 5, 2, 3, 4, 8}}, // (1,2) to (6,10)
  vector<int>{{8, 9, 10, 7, 13, 12, 11, 3, 0, 1, 2, 6, 5, 4}}  // (2,6) to (10,11)
  };

void draw_loop(cellwalker cw, color_t col) {
  cell *c = cw.at;
  transmatrix T = Id;
  ld s = anims::a;

  curvepoint(T * C0);
  for(int j=0;; j++) {
    hyperpoint H1 = mid(currentmap->get_corner(cw.at, cw.spin, 3), currentmap->get_corner(cw.at, (cw+1).spin, 3));
    if( s < .5) { curvepoint(T * (s * 2 * H1 + (1-s*2) * C0)); break; } s -= 0.5;
    curvepoint(T * H1);
    T = T * currentmap->adj(cw.at, cw.spin);
    cw += wstep;
    hyperpoint H2 = mid(currentmap->get_corner(cw.at, cw.spin, 3), currentmap->get_corner(cw.at, (cw+1).spin, 3));
    if(s < .5) { curvepoint(T * (s * 2 * C0 + (1-s*2) * H2)); break; } s -= 0.5;
    curvepoint(T * C0);
    if(next == -1) cw.spin = ppair[cw.at->master->c7 == cw.at ? 1 : 0][cw.spin];
    else cw += next;
    }

  vid.linewidth *= w;
  queuecurve(ggmatrix(c), col, 0, PPR::LINE);
  vid.linewidth /= w;
  }

void draw_othello(cell *c, ld shift) {
  for(int i=0; i<c->type; i++) {
    color_t col = gradient(0, rainbow_color(0.5, next == -1 ? (min(i, ppair[1][i]) + shift) * 2. / c->type : (i + shift) * 2. / c->type) << 8, 0, 0.75, 1) | 0xff;
    draw_loop(cellwalker(c, i), col);
    }
  }

void othello_frame() {
  draw_othello(cwt.at, 0);
  }

void enable_othello() {
  rogueviz::rv_hook(hooks_frame, 100, othello_frame);
  }

void enable_othello1() {
  rogueviz::rv_hook(hooks_frame, 100, [] { draw_loop(cwt.at, 0xFF0000FF); });
  }

void enable_othello2() {
  rogueviz::rv_hook(hooks_frame, 100, [] { draw_loop(cellwalker(cwt.at, 1), 0xFF0000FF); });
  }

int heres = 1;

int maxq = 100000000;

void count_othello() {
  cell *c = cwt.at;
  for(int i=0; i<c->type; i++) {
    cellwalker cw(c, i);
    cellwalker cw0 = cw;
    int q = 0;
    do {
      cw += wstep;
      cw += 7;
      q++;
      }
    while(cw != cw0 && q < maxq);
    println(hlog, "direction ", i, " from ", c, " : ", q);
    }
  }

void find_oth_par() {
  for(cell *c: {cwt.at, cwt.at->move(0)})
    for(int i=0; i<14; i++)
      println(hlog, i, " : ", gpushxto0(currentmap->get_corner(c, i)) * currentmap->get_corner(c, i+1));
  }

void swap_ppair() {
  swap(ppair[0][1], ppair[0][2]); swap(ppair[0][6], ppair[0][10]);
  swap(ppair[1][10], ppair[1][11]); swap(ppair[1][6], ppair[1][2]);
  }

auto othello_hook =
    arg::add3("-othello", enable_othello)
  + arg::add3("-othello1", enable_othello1)
  + arg::add3("-othello2", enable_othello2)
  + arg::add3("-othello-lst", count_othello)
  + arg::add3("-othello-next", [] { arg::shift(); next = arg::argi(); })
  + arg::add3("-othello-here", [] { int h = heres++; cell *c = cwt.at; rogueviz::rv_hook(hooks_frame, 80, [c,h] { draw_othello(c, h * 1. / heres); }); })
  + arg::add3("-othello-parallel", [] { find_oth_par(); })
  + arg::add3("-othello-swap-ppair", [] { swap_ppair(); });

// --- hat descent ---

set<void*> drawn;

bool ori_color = false;

void *get_level(cell *c, int lev) {
  if(lev == 0) return c;
  heptagon *h = c->master;
  for(int i=1; i<lev; i++) h = h->cmove(0);
  return h;
  }

color_t get_ori_color(void *v, int lev) {
  int t = 0;
  cell *c;
  if(lev == 0) {
    c = (cell*) v;
    t = c->master->c7 == c;
    }
  else {
    heptagon *h = (heptagon*) v;
    t = h->zebraval;
    for(int i=1; i<lev; i++) h = h->cmove(1);
    c = h->c7;
    }
  transmatrix T = currentmap->relative_matrix(c, cwt.at, C0);
  T = gpushxto0(T * C0) * T;
  ld alpha = atan2(T * xpush0(1));

  return (rainbow_color(t ? 0.6 : 0.8, alpha / TAU) << 8) | 0xFF;
  }

ld hat_shift = 0.7;

void hat_descent() {
  ld sca = (3 + sqrt(5)) / 2; // scaling each axis
  ld levf = -log(pconf.scale) / log(sca) - hat_shift;
  ld levpart = frac(levf);
  int lev = floor(levf);
  if(lev < 0) lev = 0, levpart = 0;
  drawn.clear();
  for(int lv: {lev, lev+1}) for(auto& p: gmatrix) {
    cell *c = p.first;
    auto v = get_level(c, lv);
    if(drawn.count(v)) continue;
    int dir = -1;
    for(int i=0; i<c->type; i++) if(get_level(c->cmove(i), lv) != v) dir = i;
    if(dir == -1) continue;
    drawn.insert(v);
    // println(hlog, "draw ", (heptagon*)v);
    cellwalker cw(c, dir);
    cellwalker cw0 = cw;
    transmatrix T = Id;
    do {
      curvepoint(T * currentmap->get_corner(cw.at, cw.spin+1));
      cw++;
      if(get_level(cw.cpeek(), lv) == v) {
        T = T * currentmap->adj(cw.at, cw.spin);
        cw += wstep;
        }
      }
    while(cw0 != cw);
    // println(hlog, "done");
    int id = 0;
    heptagon *h;

    if(lv == lev+1) {
      queuecurve(p.second, 0xFF, 0, PPR::LINE);
      continue;
      }

    if(lev > 0) {
      id = ((heptagon*)v)->c.spin(0);
      h = ((heptagon*)v)->move(0);
      }
    else {
      id = hat::get_hat_id((cell*) v);
      h = ((cell*)v)->master;
      }
    int id1 = h->c.spin(0);

    color_t col;
    
    if(ori_color) {
      col = gradient(get_ori_color(v, lev), get_ori_color(h, lev+1), 0, levpart, 1);
      }
    else {
      col = 0xFFFFFFFF;
      vector<int> ads = {0, 0x1, 0x100, 0x101, 0x10000, 0x10001, 0x10100, 0x10101 };
      col -= (ads[id] * int(128 * (1-levpart))) << 8;
      col -= (ads[id1] * int(128 * (levpart))) << 8;
      }


    queuecurve(p.second, 0xFF * (1-levpart), col, PPR::LINE);
    }
  println(hlog, "levf = ", levf, " drawn = ", isize(drawn), " gm = ", isize(gmatrix));
  }

void enable_hat_descent() {
  rogueviz::rv_hook(hooks_frame, 100, hat_descent);
  }

auto hd_hook =
    arg::add3("-hat-descent", enable_hat_descent)
  + arg::add3("-hat-ori", [] { ori_color = true; })
  + arg::add3("-hat-shift", [] { arg::shift(); hat_shift = arg::argf(); })
  + arg::add3("-hat-normal", [] {
    start_game();
    heptagon *h = cwt.at->master;
    for(int i=0; i<20; i++) h = h->cmove(0);
    for(int i=0; i<20; i++) h = h->cmove(1);
    centerover = cwt.at = h->c7;
    });

EX }

