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
set<pair<string, string> > ruleset;

map<string, int> find_matches(string s) {
  map<string, int> res;
  for(auto& p: rules) {
    size_t next = s.find(p.first);
    while(next != string::npos) {
      string t = s.substr(0, next) + p.second + s.substr(next+isize(p.first));
      auto& r = res[t];
      if(ruleset.count({p.second, p.first})) r = 2;
      else r = max(r, 1);
      next = s.find(p.first, next+1);
      }
    }
  return res;
  }

struct hrmap_rewrite : hrmap_hyperbolic {

  map<heptagon*, pair<heptagon*, string> > asg;
  map<pair<heptagon*, string>, heptagon*> asg_rev;

  heptagon *create_step(heptagon *h, int direction) override {
    if(h->move(direction)) return h->move(direction);
    if(asg.empty()) { asg[h] = {h, start}; h->zebraval = 0; }
    
    auto s = asg[h].second;
    auto root = asg[h].first;
    
    auto matches = find_matches(s);
    
    int next = h->zebraval;
    
    if(matches.empty() && next == 0) { 
      h->c.connect(0, h, 0, false);
      return h;
      }
    
    for(auto& match: matches) {
      if(h->move(next)) { next++; continue; }
      bool symmetric = match.second == 2;
      const string& m = match.first;
      if(symmetric) {
        auto matches1 = find_matches(m);
        heptagon *h1;
        if(asg_rev[{root, m}]) h1 = asg_rev[{root, m}];
        else {
          h1 = tailored_alloc<heptagon> (isize(matches1));
          h1->alt = NULL;
          h1->s = hsA;
          h1->cdata = NULL;
          h1->distance = h->distance;
          h1->zebraval = 0;
          h1->c7 = newCell(isize(matches1), h1);
          asg[h1] = {root, m};
          asg_rev[{root, m}] = h1;
          }
        int next1 = 0;
        for(auto& match2: matches1) { if(match2.first == s) break; next1++; }
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
        h1->zebraval = 1;
        h1->c7 = newCell(deg, h1);
        
        asg[h1] = {h1, m};
        asg_rev[{h1, m}] = h1;
        }
      next++;
      }
    
    if(next != h->type) { println(hlog, "degree error"); exit(1); }

    return h->move(direction);
    }

  };

bool labeller(cell* c, const shiftmatrix& V) {
  auto m = dynamic_cast<hrmap_rewrite*> (currentmap);
  if(m) {
    string s = m->asg[c->master].second;
    cgi.scalefactor = 1;
    queuestr(V, 0.5, s, colortables['j'][c->master->distance+1]);
    }
  return false;
  }

void load_rules(vector<string> vs) {

  stop_game();
  set_geometry(gInfOrderMixed);
  ginf[gInfOrderMixed].distlimit = {{1, 1}};
  ginf[gInfOrderMixed].flags |= qEXPERIMENTAL;

  start = "";
  rules.clear();
  ruleset.clear();
  for(string line: vs) {
    if(line == "") continue;

    auto i = line.find("->");
    if(i != string::npos) {
      rules.emplace_back(line.substr(0, i), line.substr(i+2));
      ruleset.emplace(line.substr(0, i), line.substr(i+2));
      }
    else start = line;
    }
  
  ginf[gInfOrderMixed].sides = isize(find_matches(start));
  if(!ginf[gInfOrderMixed].sides) ginf[gInfOrderMixed].sides = 1;
  ginf[gInfOrderMixed].flags |= qANYQ;  
  }

#if ISWEB
extern "C" {
  void load_web_rules() {
    string s = get_value("rules") + '\n';
    vector<string> split;
    string cc = "";
    for(char c: s) if(c == '\n' || c == '\r') split.push_back(cc), cc = ""; else cc += c;
    load_rules(split);
    start_game();
    clearMessages();

    bfs();
    resetview();
    drawthemap();
    centerpc(INF);
    centerover = cwt.at;
    }
  }
#endif

auto hooks =
  addHook(hooks_args, 100, [] {
  using namespace arg;
           
  if(0) ;
  #if ISWEB
  else if(argis("-rww")) {
    load_web_rules();
    }
  #endif
  else if(argis("-rwr")) {
    shift();
    fhstream ss(argcs(), "rt");
    vector<string> vs;
    string line;
    while(scan(ss, line)) vs.push_back(line);
    load_rules(vs);
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
