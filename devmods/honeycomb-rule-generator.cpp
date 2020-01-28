/**

Honeycomb data generator.

Usage: 

./hyper -geo 534h -gen-rule honeycomb-rules-534.dat -quit
./hyper -geo 535h -gen-rule honeycomb-rules-535.dat -quit
./hyper -geo 435h -gen-rule honeycomb-rules-435.dat -quit

You need to change the value of XS7 to 6 (for 435) or 12 (for others)

You also need to select 'fp used for rules' 

*/

#include "zlib.h"
#include "../hyper.h"

namespace hr {

map<string, map<string,int> > rules;

#define XS7 12
#define FV master->fiftyval

auto dis = [] (int i, char init='a') { return s0 + char(init + i); };

int get_id(cell *c) { 
  if(geometry == gSpace535) return 0;
  return c->master->fieldval;
  }

string find_path(cell *x, cell *y, int steps) {
  if(x->FV != y->FV) {
    println(hlog, x, y, " steps=", steps, " d=", x->FV, " vs ", y->FV);
    exit(3);
    }
  if(x == y) return "";
  if(steps == 0) return "?";
  for(int i=0; i<S7; i++)
    if(x->move(i) && x->move(i)->FV < x->FV)
      for(int j=0; j<S7; j++)
        if(y->move(j) && y->move(j)->FV < y->FV) {
          string ch = find_path(x->move(i), y->move(j), steps-1);
          if(ch == "?") continue;
          return dis(i) + ch + dis(y->c.spin(j));
          }
  return "?";
  }

string find_path(cell *x, cell *y) {
  if(x == y) return "";
  
  for(int steps=0;; steps++) {
    string f = find_path(x, y, steps);
    if(f != "?") return f;
    }
  }

vector<array<string, XS7>> rule_list;

struct ext_nei_rules_t {
  vector<int> from, dir, original;
  };

map<int, ext_nei_rules_t> ext_nei_rules;

set<cell*> visi;

void listnear(cell *c, ext_nei_rules_t& e, const transmatrix& T, int id) {
  visi.insert(c);
  int a = 0, b = 0;
  for(int i=0; i<S7; i++) {
    bool ok = false;
    transmatrix U = T * currentmap->adj(c, i);
    for(auto v: reg3::vertices_only) for(auto w: reg3::vertices_only)
      if(hdist(v, U*w) < 1e-3) ok = true;
    if(!ok) continue;
    cell *c1 = c->cmove(i);
    int id1 = isize(e.from);
    e.from.push_back(id);
    e.dir.push_back(i);
    a++;
    e.original.push_back(!visi.count(c1));
    if(e.original.back()) {
      b++;
      listnear(c1, e, U, id1);
      }
    }
  }

void construct_rules(cell *c, ext_nei_rules_t& e) {
  visi.clear();
  e.from = {-1};
  e.dir = {-1};
  e.original = {1};
  listnear(c, e, Id, 0);
  int orgc = 0;
  for(auto i: e.original) orgc += i;
  println(hlog, "id ", get_id(c), " list length = ", isize(e.original), " original = ", orgc);
  }

void fix_dist(cell *a, cell *b) {
  if(a->FV > b->FV+1) {
    a->FV = b->FV+1;
    forCellEx(c, a) fix_dist(a, c);
    }
  if(b->FV > a->FV+1) {
    b->FV = a->FV+1;
    forCellEx(c, b) fix_dist(b, c);
    }  
  }

string generate_ext_nei(cell *c) {
  int fv = get_id(c);
  auto& e = ext_nei_rules[fv];
  if(e.from.empty()) construct_rules(c, e);
  vector<cell*> ext_nei = {c};
  for(int i=1; i<isize(e.from); i++) {
    cell *last = ext_nei[e.from[i]];
    cell *next = last->cmove(e.dir[i]);
    fix_dist(last, next);
    ext_nei.push_back(next);
    }
  string res;
  for(int i=0; i<isize(e.from); i++) if(e.original[i]) res += char('L' + ext_nei[i]->FV - c->FV);
  return its(fv) + ":" + res;
  }

set<cell*> candidates;
vector<cell*> candidates_list;
map<string, int> id_of;
vector<cell*> rep_of;

int number_states = 0;

vector<array<int, XS7> > child_rules;

vector<array<string, XS7> > side_rules;

void add_candidate(cell *c) {
  if(candidates.count(c)) return;
  candidates.insert(c);
  candidates_list.push_back(c);
  }

void test_canonical(string fname) {
  if(S7 != XS7) { println(hlog, "fix XS7=", S7); exit(4); }
  stop_game();
  reg3::reg3_rule_available = false;
  start_game();

  int qc = reg3::quotient_count();
  
  vector<cell*> c0;
  
  if(geometry == gSpace535) {
    c0.resize(qc, cwt.at);
    }
  else {
    for(int fv=0; fv<qc; fv++) {
      cell *c = cwt.at;
      for(int i=0; i<100 || c->master->fieldval != fv; i++) c = c->cmove(hrand(S7));
      c->FV = 0;
      c0.push_back(c);
      }
    }

  for(cell* c: c0) add_candidate(c);
  
  array<int, XS7> empty;
  for(auto& e: empty) e = -1;
  println(hlog, "empty = ", empty);
  
  for(int i=0; i<isize(candidates_list); i++) {
    cell *c = candidates_list[i];
    string s = generate_ext_nei(c);
    if(!id_of.count(s)) {
      // println(hlog, "found candidate for: ", s);
      id_of[s] = number_states++;
      rep_of.push_back(c);
      for(int i=0; i<S7; i++) add_candidate(c->cmove(i));
      }
    }
  
  child_rules.resize(number_states, empty);
  
  println(hlog, "found ", its(number_states), " states");
  fflush(stdout);

  for(int i=0; i<number_states; i++) {
    cell *c = rep_of[i];
    for(int a=0; a<S7; a++) {
      cell *c1 = c->move(a);
      if(c1->FV <= c->FV) continue;
      for(int b=0; b<S7; b++) {
        cell *c2 = c1->move(b);
        if(c2->FV != c->FV) continue;
        if(c2 == c) {
          child_rules[i][a] = id_of[generate_ext_nei(c1)];
          }
        break;
        }
      continue;
      }
    }
  
  if(true) {

    println(hlog, "original rules: ", child_rules);
    fflush(stdout);

    vector<int> ih(number_states, 0);
    
    int lqids = 0;
    
    for(int a=0; a<100; a++) {
      set<array<int, XS7>> found;
      vector<array<int, XS7>> v(number_states);
      map<array<int, XS7>, int> ids;
      for(int i=0; i<number_states; i++) {
        array<int, XS7> res;
        for(int d=0; d<XS7; d++) res[d] = (child_rules[i][d] != -1) ? ih[child_rules[i][d]] : -1;
        v[i] = res;
        found.insert(res);
        }
      int qids = 0;
      for(auto hash: found) ids[hash] = qids++;
      println(hlog, "qids = ", qids);
      if(qids == lqids) break;
      lqids = qids;
      for(int i=0; i<number_states; i++)
        ih[i] = ids[v[i]];
      }
    
    println(hlog, "lqids = ", lqids);
    vector<array<int, XS7> > new_child_rules;
    new_child_rules.resize(lqids, empty);  
    for(int i=0; i<number_states; i++) {
      int j = ih[i];
      for(int d=0; d<XS7; d++) {
        int cid = child_rules[i][d];
        new_child_rules[j][d] = cid == -1 ? -1 : ih[cid];
        }
      }
    child_rules = new_child_rules;
    number_states = lqids;
    for(auto& p: id_of) p.second = ih[p.second];
    println(hlog, "rehashed");
    fflush(stdout);
    }

  for(auto& a: child_rules) for(auto i:a) print(hlog, i, ",");
  println(hlog);
  
  side_rules.resize(number_states);

  for(int i=0; i<isize(candidates_list); i++) {
    cell *c = candidates_list[i];
    string s = generate_ext_nei(c);
    int id = id_of[s];
    
    cell *cpar = nullptr;
    int a0;

    for(int a=0; a<S7; a++) {
      cell *c1 = c->move(a);
      if(!c1) continue;
      if(c1->FV < c->FV && !cpar) cpar = c1, a0 = a;      
      }
    
    for(int a=0; a<S7; a++) {
      cell *c1 = c->move(a);
      if(!c1) continue;
      bool is_child = false;
      cell* c2 = nullptr;
      int dir = 0;
      
      if(c1->FV >= c->FV) {
        for(int b=0; b<S7; b++) {
          c2 = c1->move(b);
          if(!c2) continue;
          if(c2->FV >= c1->FV) continue;
          dir = c1->c.spin(b);
          break;
          }
        }

      is_child = (c2 == c);
      bool was_child = child_rules[id][a] >= 0;

      if(is_child ^ was_child) {
        println(hlog, "id=", id, " a=", a);
        println(hlog, "is_child = ", is_child);
        println(hlog, "was_child = ", was_child);
        println(hlog, "c fv = ", c->FV);
        println(hlog, "c1 fv = ", c1->FV, " [", a, "]");
        if(c2 == nullptr) { println(hlog, "c2 missing"); }
        else
          println(hlog, "c2 fv = ", c2->FV, " [", c2->c.spin(dir), "]");
        println(hlog, c, "->", c1, "->", c2);
        fflush(stdout);
        
        cell *r = rep_of[id];
        println(hlog, r, " at ", r->FV);
        cell *r1 = r->move(a);
        if(!r1) { println(hlog, "r1 missing"); continue; }
        println(hlog, r1, " at ", r1->FV);
        for(int a=0; a<S7; a++) if(r1->move(a)) println(hlog, a, ":", r1->move(a), " at ", r1->move(a)->FV);
        fflush(stdout);
        exit(3);
        }
      
      if(is_child) continue;
      
      string solu;
      
      if(c1->FV < c->FV)
        solu = dis(a0, 'A') + find_path(cpar, c1);
      else if(c1->FV == c->FV)
        solu = dis(a0, 'A') + find_path(cpar, c2) + dis(dir);
      else 
        solu = find_path(c, c2) + dis(dir);
      
      auto& sr = side_rules[id][a];
      
      if(sr != "" && sr != solu) {
        println(hlog, "conflict: ", solu, " vs ", sr);
        if(isize(sr) < isize(solu)) continue;
        }

      sr = solu;
      
      continue;
      }
    }
  
  println(hlog, side_rules);
  
  string side_data;
  for(auto& a: side_rules) for(auto&b :a) if(b != "") side_data += b + ",";
  println(hlog, side_data);
  
  vector<short> data;
  for(auto& a: child_rules) for(auto i:a) data.push_back(i);  

  shstream ss;
  
  auto& fp = currfp;
  hwrite_fpattern(ss, fp);

  vector<int> root(qc, 0);
  for(int i=0; i<qc; i++) root[i] = id_of[generate_ext_nei(c0[i])];
  println(hlog, "root = ", root);

  hwrite(ss, root);
  
  println(hlog, "copy data");
  hwrite(ss, data);
  println(hlog, "copy side_data");
  hwrite(ss, side_data);
  
  println(hlog, "compress_string");
  string s = compress_string(ss.s);
  
  fhstream of(fname, "wb");
  print(of, s);
  }

auto fqhook = 
  addHook(hooks_args, 100, [] {
  using namespace arg;
           
  if(0) ;
  else if(argis("-gen-rule")) {
    shift(); test_canonical(args());
    }
  else return 1;
  return 0;
  });

}
  
// 1 + 12 + 30 + 20 = 55
