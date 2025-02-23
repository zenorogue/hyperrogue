// show the fundamental domain for quotient spaces
// Copyright (C) 2018 Zeno and Tehora Rogue, see 'hyper.cpp' for details

#include "rogueviz.h"

namespace hr {

namespace fundamental {

color_t color1, color2;

cell *starter;
map<cell*, set<int>> same;
map<cell*, shiftmatrix> gm;

bool is_connected(cellwalker cw) {
  return same[cw.at].count(cw.spin);
  }

void be_connected(cellwalker cw) {
  // transmatrix T = gm[cw.at];
  same[cw.at].insert(cw.spin);
  cw += wstep;
  same[cw.at].insert(cw.spin);
  /* printf("%s", display(T * C0)); 
  printf(" %s\n", display(gm[cw.at] * C0)); */
  // queueline(T * C0, gm[cw.at] * C0, 0xFF0000FF, 3);
  }

int funmode = 0;

shiftpoint corner(cellwalker cw) {
  shiftmatrix T = gm[cw.at];

  if(funmode == 3) switch(geometry) {
    case gKleinQuartic: {
      ld a = edge_of_triangle_with_angles(90._deg, M_PI/14, M_PI*2/14);
      shiftpoint at = gm[cw.at] * get_corner_position(cw.at, cw.spin+(cw.mirrored?0:1), 3);
      shiftpoint best = at; ld bestdist = 999;
      for(int i=0; i<14; i++) {
        shiftmatrix sm = gm[starter];
        if(variation == eVariation::untruncated)
          sm = sm * rgpushxto0(get_corner_position(starter, 0, 3));
        auto s = sm * xspinpush0(i * M_PI/7, a);
        ld d = hdist(at, s);
        if(d < bestdist) bestdist = d, best = s;
        }
      return best;
      }
    default: /* no special */;
    }

  if(funmode == 2 && BITRUNCATED) {
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

bool single_edges = false;
bool fill_faces = true;

int group_count(cellwalker cw) {
  if(is_connected(cw)) return 0;
  auto cw1 = cw;
  int groups = 0;
  do {
    if(!is_connected(cw1)) groups++;
    cw1 = cw1 + wstep - 1;
    }
  while(cw1 != cw);
  return groups;
  }

void fundamental_marker() {
  if(!funmode || !quotient) return;
  same.clear();
  gm.clear();
  
  gm[starter] = ggmatrix(starter);
  
  vector<cell*> cells;
  cells.push_back(starter);
  
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
  
  while(fill_faces) {
    int f = face_edges;
    for(int k=0; k<isize(cells); k++) {
      cell *c = cells[k];
      for(int i=0; i<c->type; i++) {
        cellwalker cw(c, i);
        if(group_count(cw) == 1) {
          face_edges++;
          be_connected(cw);
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
      if(single_edges) {
        if(!is_connected(cw0)) corners++, cw = cw0;
        }
      else {
        if(group_count(cw0) >= 3) corners++, cw = cw0;
        }
      }
    }

  if(!corners) return;
  
  // printf("tree edges = %d, face edges = %d, corners = %d\n", tree_edges, face_edges, corners);

  vector<cellwalker> cornerlist;
  map<cellwalker, int> corner_id;
    
  for(int ci=0; ci<corners; ci++) {
    corner_id[cw] = cornerlist.size();
    cornerlist.push_back(cw);
        
    while(true) {
      cw++;
      while(is_connected(cw)) {
        cw += wstep;
        cw++;
        }
      if(single_edges || group_count(cw) >= 3) break;
      }
    }
  auto corners0 = corners;
  corners = isize(cornerlist);
  cornerlist.push_back(cw);

  if(corners0 != corners) println(hlog, "corners=", tie(corners0, corners));

  vector<pair<shiftmatrix, shiftmatrix>> nearm;

  for(int ci=0; ci<corners; ci++) {
    auto cw = cornerlist[ci];
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
    }
  
  vid.linewidth *= widthfactor;

  vector<shiftpoint> cornerpos;
  for(auto c: cornerlist) cornerpos.push_back(corner(c));

  for(int ci=0; ci<corners; ci++) {

    shiftpoint h = cornerpos[ci];
    shiftpoint h2 = cornerpos[ci+1];
    
    for(auto& n: nearm) queueline(n.first * inverse_shift(n.second, h), n.first * inverse_shift(n.second, h2), color1, 3);
    }
    
  for(int ci=0; ci<corners; ci++) {

    shiftpoint h = cornerpos[ci];
    shiftpoint h2 = cornerpos[ci+1];
    
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
    
    auto cw = cornerlist[ci];
    cellwalker cw1 = (cw+1+wstep);
    bool mirrored = false;
    while(is_connected(cw1)) cw1 = cw1 + 1 + wstep;
    if(!corner_id.count(cw1)) cw1 = cw1 + wmirror - 1, mirrored = true;
    if(!corner_id.count(cw1)) println(hlog, "still bad");
    auto ci1 = corner_id[cw1];
    auto nx = cornerlist[ci+1];
    auto nx1 = cornerlist[ci1+1];
    auto pv1 = cornerlist[(ci1+corners-1) % corners];

    // visited.insert(next_corner[cw]);
    // cellwalker cw2 = next_corner[cw];
    if(nx < (mirrored ? nx1 : cw1)) {
    
      int mc = (mirrored ? color1 : color2) >> 8;
      if(hdist(corner(cw), corner(nx)) > 1e-3) {
        queuestr(labelpos(corner(cw), corner(nx)), label_scale/cgi.scalefactor, its(id), mc);
        if(mirrored)
          queuestr(labelpos(corner(cw1), corner(nx1)), label_scale/cgi.scalefactor, its(id), mc);
        else
          queuestr(labelpos(corner(pv1), corner(cw1)), label_scale/cgi.scalefactor, its(id), mc);
        id++;
        }      
      }
    }

  vid.linewidth /= widthfactor;
  }

void showMenu() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("display fundamental domains"), 0xFFFFFFFF, 150, 0);
  vector<string> mode_names = {"no display", "corners", "centers", "special"};
  dialog::addSelItem("mode", mode_names[funmode], 'm');
  dialog::add_action([] { funmode = (1 + funmode) % 4; });
  dialog::addSelItem("label distance", fts(label_dist), 'd');
  dialog::add_action([] {
    dialog::editNumber(label_dist, 0, 10, .1, 0.5, "label fistance", "label distance");
    });
  dialog::addSelItem("label scale", fts(label_scale), 's');
  dialog::add_action([] {
    dialog::editNumber(label_scale, 0, 10, .1, 0.5, "label scale", "label scale");
    });
  dialog::addSelItem("line width factor", fts(widthfactor), 'w');
  dialog::add_action([] {
    dialog::editNumber(widthfactor, 0, 5, .1, 1, "line width factor", "line width factor");
    });
  dialog::addColorItem("color of other domains", color1, 'o');
  dialog::add_action([] () {
    dialog::openColorDialog(color1, NULL);
    dialog::get_di().dialogflags |= sm::MAYDARK | sm::SIDE;
    });
  dialog::addColorItem("color of primary domain", color2, 'p');
  dialog::add_action([] () {
    dialog::openColorDialog(color2, NULL);
    dialog::get_di().dialogflags |= sm::MAYDARK | sm::SIDE;
    });
  dialog::addSelItem("set the central tile to current position", its(celldistance(starter, cwt.at)), 's');
  dialog::add_action([] () {
    starter = cwt.at;
    });

  dialog::addBack();
  dialog::display();
  }

void enable_fundamental() {
  start_game(); starter = cwt.at;
  rogueviz::rv_hook(hooks_frame, 100, fundamental_marker);
  rogueviz::rv_hook(hooks_clearmemory, 100, [] { same.clear(); gm.clear(); });
  rogueviz::rv_hook(hooks_o_key, 80, [] (o_funcs& v) { v.push_back(named_dialog("fundamental", showMenu)); });
  }

int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-fundamental0")) {
    enable_fundamental();
    }
  else if(argis("-fundamental")) {
    shift(); funmode = argi();
    shift(); color1 = arghex();
    shift(); color2 = arghex();
    shift_arg_formula(widthfactor);
    shift_arg_formula(label_scale);
    shift_arg_formula(label_dist);
    enable_fundamental();
    }
  else if(argis("-fundamental-more")) {
    shift(); single_edges = argi();
    shift(); fill_faces = argi();
    }
  else return 1;
  return 0;
  }

auto fundamentalhook = addHook(hooks_args, 100, readArgs);
}
}
