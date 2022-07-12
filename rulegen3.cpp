// Hyperbolic Rogue -- rule generator
// Copyright (C) 2011-2021 Zeno Rogue, see 'hyper.cpp' for details

/** \file rulegen3.cpp 
 *  \brief An algorithm to create strict tree rules for arb tessellations -- 3D parts
 */

#include "hyper.h"

namespace hr {

EX namespace rulegen {

/** next roadsign ID -- they start at -100 and go downwards */
int next_roadsign_id = -100;

/** get the ID of a roadsign path */
EX map<vector<int>, int> roadsign_id;

EX int get_roadsign(twalker what) {
  int dlimit = what.at->dist - 1;
  tcell *s = what.at, *t = what.peek();
  vector<int> result;
  while(s->dist > dlimit) { 
    twalker s0 = s;
    get_parent_dir(s0);
    if(s->parent_dir == MYSTERY) throw hr_exception("parent_dir unknown");
    result.push_back(s->parent_dir); s = s->move(s->parent_dir);
    result.push_back(s->dist - dlimit);
    }
  vector<int> tail;
  while(t->dist > dlimit) {
    twalker t0 = t;
    get_parent_dir(t0);
    if(t->parent_dir == MYSTERY) throw hr_exception("parent_dir unknown");
    tail.push_back(t->dist - dlimit);
    tail.push_back(t->c.spin(t->parent_dir));
    t = t->move(t->parent_dir);
    }
  map<tcell*, int> visited;
  queue<tcell*> vqueue;
  auto visit = [&] (tcell *c, int dir) {
    if(visited.count(c)) return;
    visited[c] = dir;
    vqueue.push(c);
    };
  visit(s, MYSTERY);
  while(true) {
    if(vqueue.empty()) throw hr_exception("vqueue empty");
    tcell *c = vqueue.front();
    if(c == t) break;
    vqueue.pop();
    for(int i=0; i<c->type; i++)
      if(c->move(i) && c->move(i)->dist <= dlimit)
        visit(c->move(i), c->c.spin(i));
    }
  while(t != s) {
    int d = visited.at(t);
    tail.push_back(t->dist - dlimit);
    tail.push_back(t->c.spin(d));
    t = t->move(d);
    }
  reverse(tail.begin(), tail.end());
  for(auto t: tail) result.push_back(t);
  if(roadsign_id.count(result)) return roadsign_id[result];
  return roadsign_id[result] = next_roadsign_id--;
  }

map<pair<int, int>, vector<pair<int, int>> > all_edges;

EX vector<pair<int, int>>& check_all_edges(twalker cw, analyzer_state* a, int id) {
  auto& ae = all_edges[{cw.at->id, cw.spin}];
  if(ae.empty()) {
    set<tcell*> seen;
    vector<pair<twalker, transmatrix> > visited;
    auto visit = [&] (twalker tw, const transmatrix& T, int id, int dir) {
      if(seen.count(tw.at)) return;
      seen.insert(tw.at);
      auto& sh0 = currentmap->get_cellshape(tcell_to_cell[cw.at]);
      auto& sh1 = currentmap->get_cellshape(tcell_to_cell[tw.at]);
      int common = 0;
      vector<hyperpoint> kleinized;
      vector<hyperpoint> rotated;
      for(auto v: sh0.vertices_only) kleinized.push_back(kleinize(sh0.from_cellcenter * v));
      for(auto w: sh1.vertices_only) rotated.push_back(kleinize(T*sh1.from_cellcenter * w));
      
      for(auto v: kleinized)
      for(auto w: rotated)
        if(sqhypot_d(MDIM, v-w) < 1e-6)
          common++;
      if(common < 2) return;
      visited.emplace_back(tw, T);
      ae.emplace_back(id, dir);
      };
    visit(cw, Id, -1, -1);
    for(int i=0; i<isize(visited); i++) {
      auto tw = visited[i].first;      
      for(int j=0; j<tw.at->type; j++) {
        visit(tw + j + wstep, visited[i].second * currentmap->adj(tcell_to_cell[tw.at], (tw+j).spin), i, j);
        }
      }
    println(hlog, "for ", tie(cw.at->id, cw.spin), " generated all_edges structure: ", ae, " of size ", isize(ae));
    }
  return ae;
  }

EX void cleanup3() {
  all_edges.clear();
  roadsign_id.clear();
  next_roadsign_id = -100;
  }

}

}