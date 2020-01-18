// Hyperbolic Rogue -- infinite-order tessellations
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file inforder3.cpp
 *  \brief infinite-order tessellations
 *
 *  very simple
 */

#include "../hyper.h"

namespace hr {

namespace rewriting {

bool symmetric;

string start;
vector<pair<string, string> > rules;

set<string> find_matches(string s) {
  set<string> res;
  for(auto& p: rules) {
    size_t next = s.find(p.first);
    while(next != string::npos) {
      res.emplace(s.substr(0, next) + p.second + s.substr(next+isize(p.first)));
      next = s.find(p.first, next+1);
      }
    }
  return res;
  }

struct hrmap_rewrite : hrmap_hyperbolic {

  map<heptagon*, string> asg;
  map<string, heptagon*> asg_rev;

  heptagon *create_step(heptagon *h, int direction) {
    if(h->move(direction)) return h->move(direction);
    if(asg.empty()) asg[h] = start;
    
    auto matches = find_matches(asg[h]);
    
    int next = (symmetric || h == getOrigin()) ? 0 : 1;
    
    for(auto& m: matches) {
      if(symmetric) {
        if(h->move(next)) { next++; continue; }
        auto matches1 = find_matches(m);
        heptagon *h1;
        if(asg_rev[m]) h1 = asg_rev[m];
        else {
          h1 = tailored_alloc<heptagon> (isize(matches1));
          h1->alt = NULL;
          h1->s = hsA;
          h1->cdata = NULL;
          h1->distance = 0;
          h1->c7 = newCell(isize(matches1), h1);
          asg[h1] = m;
          asg_rev[m] = h1;
          }
        int next1 = 0;
        for(auto& s: matches1) { if(s == asg[h]) break; next1++; }
        h->c.connect(next, h1, next1, false);
        }
      else {
        int deg = 1 + isize(find_matches(m));
        auto h1 = tailored_alloc<heptagon> (deg);
        h->c.connect(next, h1, 0, false);
  
        h1->alt = NULL;
        h1->s = hsA;
        h1->cdata = NULL;
        h1->distance = h->distance + 1;
        h1->c7 = newCell(deg, h1);
        
        asg[h1] = m;
        }
      next++;
      }

    return h->move(direction);
    }

  };

bool labeller(cell* c, const transmatrix& V) {
  auto m = dynamic_cast<hrmap_rewrite*> (currentmap);
  if(m) {
    string s = m->asg[c->master];
    queuestr(V, 0.5, s, 0xFFFFFF);
    }
  return false;
  }


auto hooks =
  addHook(hooks_args, 100, [] {
  using namespace arg;
           
  if(0) ;
  else if(argis("-rwr")) {
    stop_game();
    set_geometry(gInfOrderMixed);
    if(PURE) println(hlog, "pure");
    ginf[gInfOrderMixed].distlimit = {1, 1};
    ginf[gInfOrderMixed].flags |= qEXPERIMENTAL;
    shift();
    fhstream ss(argcs(), "rt");
    scan(ss, start);
    string line;
    while(scan(ss, line)) {
      auto i = line.find("-");
      if(i != string::npos) {
        rules.emplace_back(line.substr(0, i), line.substr(i+1));
        }
      }
    symmetric = true;
    set<pair<string, string> > checker;
    for(auto r: rules) checker.insert(r);
    for(auto r: rules) if(!checker.count({r.second, r.first})) {
      if(symmetric) println(hlog, "asymmetric rule: ", r);
      symmetric = false;
      }
    ginf[gInfOrderMixed].sides = isize(find_matches(start));
    ginf[gInfOrderMixed].flags |= qANYQ;
    }
  else return 1;
  return 0;
  }) +

  addHook(hooks_newmap, 100, [] { 
    if(geometry == gInfOrderMixed && !rules.empty()) return (hrmap*) new hrmap_rewrite;
    return (hrmap*) nullptr; 
    })
  
  + addHook(hooks_drawcell, 100, labeller);

}
}