// this generates data for Klein's Quartic (-geo 0 -quartic) and Bolza surface (in -geo 7 -quartic) and 2x Bolza (-geo7 -quartic2)
// Copyright (C) 2018 Zeno Rogue, see 'hyper.cpp' for details

#include "../init.cpp"

using namespace hr;

int qty;

namespace hr {
cellwalker& operator += (cellwalker& cw, int spin);
cellwalker& operator += (cellwalker& cw, wstep_t);
}

void recursive(cell *c, int col, int dir, int dbl) {
  c->landparam = col;
  c->mondir = dir;
  c->monst = moButterfly;
  if(S7 == 7) {
    for(int i=0; i<7; i++) {
      for(int j=3; j<5; j++) {
        cellwalker cw(c, dir + i);
        for(int u=0; u<4; u++) {
          cw += wstep;
          cw += j;
          }
        cw += -dir;
        if(cw.c->landparam == -1) recursive(cw.c, col^dbl, dir, dbl);
        }
      }
    }
  if(S7 == 8) {
    for(int i=0; i<8; i++) {
      cellwalker cw(c, dir);
      cw += i;
      cw += wstep;
      cw += 4;
      cw += wstep;
      cw += (4-i);
//    if(cw.c->landparam == -1) recursive(cw.c, (col ^ 1));
      if(cw.c->landparam == -1) recursive(cw.c, col ^ dbl, cw.spin, dbl);
      }
    }
  }

void generate_quartic(bool dbl) {
  celllister clgen(cwt.c, S7==7?10:8, 1000000, NULL);
  int d;
  for(cell *c1: clgen.lst) { c1->landparam = -1; d = celldist(c1); }
  for(cell *c1: clgen.lst) if(c1->landparam == -1) {
    recursive(c1, qty, 0, dbl);
    qty++;
    if(dbl) qty++;
    }
  vector<unsigned> colors;
  for(cell *c1: clgen.lst) {
    if(celldist(c1) >= d-1) continue;
    if(c1->mov[c1->mondir]->landparam > c1->mov[c1->mondir ^ 4]->landparam)
      c1->mondir ^= 4;
    }

  if(0) for(cell *c1: clgen.lst) {
    if(celldist(c1) >= d-2) continue;
    vector<int> connections;
    cellwalker cw(c1, c1->mondir);
    for(int i=0; i<S7; i++) {
      cellwalker cwx = cw; cwx += wstep;
      connections.push_back(cwx.c->landparam * S7 + (cwx.spin - cwx.c->mondir + MODFIXER) % S7);
      cw += 1;
      }
    printf("/* %03d */", c1->landparam);
    for(int i=0; i<S7; i++) printf(" %d,", connections[i]);
    printf("\n");
    }
  printf("qty = %d\n", qty);
  for(int i=0; i<qty; i++) colors.push_back(hrand(0x1000000));
  for(cell *c1: clgen.lst) {
    c1->landparam = colors[c1->landparam % qty];
    }
  }

int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-quartic")) {
    PHASE(3);
    start_game();
    generate_quartic(0);
    }
  else if(argis("-quartic2")) {
    PHASE(3);
    start_game();
    generate_quartic(1);
    }
  else return 1;
  return 0;
  }

auto hook = addHook(hooks_args, 100, readArgs);
 
// Bolza:: genus 2 => Euler characteristic -2
// octagon: -2/6
// ~> 6 octagons

