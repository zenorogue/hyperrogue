// show the fundamental domain for quotient spaces
// Copyright (C) 2018 Zeno and Tehora Rogue, see 'hyper.cpp' for details

#include "rogueviz.h"

namespace hr {

namespace fundamental {

color_t color1, color2;

map<cell*, int> same;
map<cell*, shiftmatrix> gm;

bool is_connected(cellwalker cw) {
  return same[cw.at] & (1<<cw.spin);
  }

void be_connected(cellwalker cw) {
  // transmatrix T = gm[cw.at];
  same[cw.at] |= (1<<cw.spin);
  cw += wstep;
  same[cw.at] |= (1<<cw.spin);
  /* printf("%s", display(T * C0)); 
  printf(" %s\n", display(gm[cw.at] * C0)); */
  // queueline(T * C0, gm[cw.at] * C0, 0xFF0000FF, 3);
  }

int funmode = 0;

shiftpoint corner(cellwalker cw) {
  shiftmatrix T = gm[cw.at];
  if(funmode == 2) {
    while(cw.at->type != S7) { 
      cw++; 
      T = T * currentmap->adj(cw.at, cw.spin);
      cw += wstep; 
      }
    return T * C0;
    }
  return gm[cw.at] * get_corner_position(cw.at, cw.spin+(cw.mirrored?0:1), 3);
  }

transmatrix rel(cellwalker cw) {
  return currentmap->adj(cw.at, cw.spin);
  }

ld label_dist = .3;

shiftmatrix labelpos(shiftpoint h1, shiftpoint h2) {
  shiftpoint h = mid(h1, h2);
  shiftmatrix T = rgpushxto0(h);
  hyperpoint hx = inverse_shift(T, h2);
  ld alpha = atan2(-hx[1], hx[0]);
  return T * xspinpush(alpha + 90._deg, label_dist);
  }
 
ld widthfactor = 5;
ld label_scale = 1;

void fundamental_marker() {
  if(!funmode || !quotient) return;
  same.clear();
  gm.clear();
  
  same[cwt.at] = 0;
  gm[cwt.at] = ggmatrix(cwt.at);
  
  vector<cell*> cells;
  cells.push_back(cwt.at);
  
  int tree_edges = 0;
  int face_edges = 0;
  
  for(int k=0; k<isize(cells); k++) {
    cell *c = cells[k];
    for(int i=0; i<c->type; i++) {
      cellwalker cw(c, i);
      cell *c2 = cw.cpeek();
      if(gm.count(c2)) continue;
      gm[c2] = gm[c] * rel(cw);
      // queueline(gm[c2] * C0, gm[c2] * xspinpush0(ticks, 0.2), 0xFFFFFFFF, 3);
      be_connected(cw);
      tree_edges++;
      cells.push_back(c2);
      }
    }
  
  while(true) {
    int f = face_edges;
    for(int k=0; k<isize(cells); k++) {
      cell *c = cells[k];
      for(int i=0; i<c->type; i++) {
        cellwalker cw(c, i);
        if(is_connected(cw) && is_connected(cw+1) && !is_connected(cw+wstep-1)) {
          face_edges++;
          be_connected(cw+wstep-1);
          }
        }
      }
    if(f == face_edges) break;
    }
  
  cellwalker cw;
  
  int corners = 0;

  for(int k=0; k<isize(cells); k++) {
    cell *c = cells[k];
    for(int i=0; i<c->type; i++) {
      cellwalker cw0(c, i);
      if(!is_connected(cw0) && !is_connected(cw0+1) && !is_connected(cw0+wstep-1))
        corners++, cw = cw0;
      }
    }
  
  // printf("tree edges = %d, face edges = %d, corners = %d\n", tree_edges, face_edges, corners);
  
  map<cellwalker, cellwalker> next_corner;
  map<cellwalker, cellwalker> prev_corner;
  
  for(int ci=0; ci<corners; ci++) {
    cellwalker cw0 = cw;
        
    while(true) {
      cw++;
      if(is_connected(cw)) {
        cw += wstep;
        cw++;
        }
      if(!is_connected(cw+1) && !is_connected(cw+wstep-1))
        break;
      }
    
    next_corner[cw0] = cw;
    prev_corner[cw] = cw0;
    }

  vector<pair<shiftmatrix, shiftmatrix>> nearm;

  for(int ci=0; ci<corners; ci++) {
    for(int u=0; u<1; u++) {
      cellwalker cw1 = cw+u+wstep+(u-1);
      /* printf("%p/%d %p/%d ", cw.at, cw.spin, cw1.at, cw1.spin);
      printf("[%d %d %d] ", is_connected(cw), is_connected(cw+1), is_connected(cw+wstep-1));
      printf("[%d %d %d] ", is_connected(cw1), is_connected(cw1+1), is_connected(cw1+wstep-1));
      printf("%d %d;\n", !!next_corner.count(cw1), !!next_corner.count(cw1+wmirror-1)); */
      shiftmatrix T_here = gm[cw.at] * rel(cw+u);
      shiftmatrix T_there = gm[cw1.at];
      nearm.emplace_back(T_here, T_there);
      }
    cw = next_corner[cw];
    }
  
  vid.linewidth *= widthfactor;

  for(int ci=0; ci<corners; ci++) {

    shiftpoint h = corner(cw);
    cw = next_corner[cw];
    shiftpoint h2 = corner(cw);
    
    for(auto& n: nearm) queueline(n.first * inverse_shift(n.second, h), n.first * inverse_shift(n.second, h2), color1, 3);
    }
    
  for(int ci=0; ci<corners; ci++) {

    shiftpoint h = corner(cw);
    cw = next_corner[cw];
    shiftpoint h2 = corner(cw);
    
    queueline(h, h2, color2, 3);
    }
  
  if(0) for(int k=0; k<isize(cells); k++) {
    cell *c = cells[k];
    for(int i=0; i<c->type; i++) {
      cellwalker cw0(c, i);
      if(!is_connected(cw0)) continue;
      int v = 0;
      for(auto& n: nearm) {
        queueline(n.first * inverse_shift(n.second, gm[cw0.at]) * xspinpush0(v, .05), n.first * inverse_shift(n.second, gm[cw0.cpeek()]) * xspinpush0(v, .05), 0xFF8000FF, 0);
        v++;
        }
      queueline(gm[cw0.at] * C0, gm[cw0.cpeek()] * C0, 0xFF0000FF, 0);
      }
    }

  set<cellwalker> visited;
  
  int id = 0;
  
  for(int ci=0; ci<corners; ci++) {
    
    cellwalker cw1 = (cw+1+wstep);
    bool mirrored = false;
    if(!next_corner.count(cw1)) cw1 = cw1 + wmirror - 1, mirrored = true;

    // visited.insert(next_corner[cw]);
    // cellwalker cw2 = next_corner[cw];
    if(next_corner[cw] < (mirrored ? next_corner[cw1] : cw1)) {
    
      int mc = (mirrored ? color1 : color2) >> 8;
      if(hdist(corner(cw), corner(next_corner[cw])) > 1e-3) {
        queuestr(labelpos(corner(cw), corner(next_corner[cw])), label_scale/cgi.scalefactor, its(id), mc);
        if(mirrored)
          queuestr(labelpos(corner(cw1), corner(next_corner[cw1])), label_scale/cgi.scalefactor, its(id), mc);
        else
          queuestr(labelpos(corner(prev_corner[cw1]), corner(cw1)), label_scale/cgi.scalefactor, its(id), mc);
        id++;
        }      
      }
    cw = next_corner[cw];
    }

  vid.linewidth /= widthfactor;
  }

int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-fundamental")) {
    shift(); funmode = argi();
    shift(); color1 = arghex();
    shift(); color2 = arghex();
    shift_arg_formula(widthfactor);
    shift_arg_formula(label_scale);
    shift_arg_formula(label_dist);
    }
  else return 1;
  return 0;
  }

auto fundamentalhook = addHook(hooks_args, 100, readArgs) + addHook(hooks_frame, 100, fundamental_marker) + addHook(hooks_clearmemory, 100, [] { same.clear(); gm.clear(); });

}

}