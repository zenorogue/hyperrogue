#include "../hyper.h"
#include <iostream>
#include <fstream>
#include <thread>

namespace hr {

namespace subquotient {

eGeometry gSubquotient(eGeometry(-1));

vector<int> connections;

void create_subquotient(int qty = -1, int id = 0) {
  start_game();
  auto ac = currentmap->allcells();
  auto ca = currentmap->gamestart();
  
  for(auto cb: ac) for(int i=0; i<cb->type; i++) for(int m=0; m<2; m++) {
    vector<cell*> visited;
    map<cell*, cellwalker> vmap;
    
    auto visit = [&] (cell *da, cellwalker db) { 
      if(vmap.count(da)) {
        // println(hlog, da, " -> ", db, " [old]");
        return;
        }
      // println(hlog, da, " -> ", db, " [new]");
      vmap[da] = db;
      visited.emplace_back(da); 
      };
    
    visit(ca, cellwalker(cb, i, m));
    
    for(int i=0; i<isize(visited); i++) {
      for(int j=0; j<visited[i]->type; j++) {
        cellwalker wa(visited[i], 0);
        cellwalker wb(vmap[visited[i]]);
        wa += j;
        wb += j;
        wa += wstep;
        wb += wstep;
        int r = wa.spin;
        wa -= r;
        wb -= r;
        // println(hlog, wa, " -> ", wb);
        setdist(wa.at, 7, nullptr);
        wa.at->item = itGold;
        visit(wa.at, wb);
        }
      }
    
    int vertex = 0, edge = 0, badcycle = 0;

    map<int, int> by_cycle;
    
    for(auto swb: vmap) {
      auto& s = swb.first;
      auto& wb = swb.second;
      if(s == wb.at) { vertex++; continue; }
      bool is_edge = false;
      for(int j=0; j<s->type; j++) if(s->move(j) == wb.at && (wb+j).peek() == s)
        is_edge = true;
      if(is_edge) { edge++; continue; }
      int cs = 0;
      cell *sx = s;
      auto cw = cellwalker(s, 0);
      vector<cell*> lst;
      do {
        int sp = cw.spin;
        bool mirr = cw.mirrored;
        if(cw.mirrored) sp = -sp;
        cw -= sp;
        lst.push_back(sx);
        cw = vmap[sx];
        if(mirr) cw += wmirror;
        sx = cw.at;
        cw += sp;
        cs++;
        if(cs >= 100) break;
        }
      while(sx != s);
      if(cw.spin) badcycle++;
      by_cycle[cs]++;      
      }
    
    if(vertex || edge || badcycle || m == 0) continue;
    
    vector<pair<int, int>> bcp;
    for(auto b: by_cycle) bcp.push_back(b);
    
    if(qty == -1)
      println(hlog, "m=", m, " vertex/edge = ", tie(vertex, edge), " badcycle = ", badcycle, " by_cycle = ", bcp);
    
    if(by_cycle[qty] == isize(vmap)) {
      if(id > 0) {id--; continue; }
      
      map<cell*, int> ids;
      int next_id = 0;
      vector<cell*> by_id;
      
      set<cell*> visited;
      
      for(auto s: ac) if(!visited.count(s)) {
        by_id.push_back(s);
        ids[s] = next_id;
        auto sx = s;
        do {
          visited.insert(sx);
          sx = vmap[sx].at;
          }
        while(sx != s);
        next_id++;
        }

      println(hlog, "ids = ", next_id);
      connections.clear();
      if(int(gSubquotient) == -1) {
        ginf.push_back(ginf[geometry]);
        gSubquotient = eGeometry(isize(ginf) - 1);        
        }
      
      ginf[gSubquotient] = ginf[geometry];
      /* we need to be 'pure', unrectified may not work */
      if(UNRECTIFIED) swap(ginf[gSubquotient].sides, ginf[gSubquotient].vertex);
      
      for(int i=0; i<next_id; i++) {
        cell *s = by_id[i];
        for(int j=0; j<s->type; j++) {
          cellwalker cw(s, j);
          cw += wstep;
          int res;
          while(!ids.count(cw.at)) {
            int sp = cw.spin;
            bool flip = cw.mirrored;
            if(flip) sp = -sp;
            cw -= sp;
            if(cw.spin) println(hlog, "bad spin");
            cw = vmap[cw.at];
            if(flip) cw += wmirror;
            cw += sp;            
            }
          res = ids[cw.at] * s->type + cw.spin;
          if(cw.mirrored)  res |= quotientspace::symmask;
          connections.push_back(res);
          }
        }
      
      stop_game();
      set_geometry(gSubquotient);
      variation = eVariation::pure;
      println(hlog, "variation = ", int(variation));
      start_game();
      println(hlog, "started");
      return;
      }
    }
  }

int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-subquotient")) { start_game(); shift(); create_subquotient(argi()); }

  else return 1;
  return 0;
  }

auto fundamentalhook = addHook(hooks_args, 100, readArgs)
  + addHook(hooks_newmap, 0, [] {
    if(geometry == gSubquotient) 
      return (hrmap*) new quotientspace::hrmap_quotient(connections);
    return (hrmap*) nullptr;
  });

}

}