/**

Honeycomb data generator. See rulegen.sh

This algorithm works as follows:

- We use a DFS-like algorithm to identify all the required states. To tell whether two cells
  c1 and c2 are in the same state, we compute its generate_ext_nei -- the same generate_ext_nei
  is the same state. To compute generate_ext_nei(c), we list all cells vertex-adjacent to c,
  and for each c' in this list, we compute FV(c')-FV(c), where FV is the distance from
  some central tile. It is crucial to identify the directions in unique way (in 2D we can simply
  use clockwise order, in 3D it is more difficult) -- we do this by using a regular pattern
  (see get_id).
  
  After all states are identified, we construct the tree of states -- every non-root state is
  attached to the first neighbor (according to the direction order) which has smaller FV.
  For non-tree directions, we construct a path going through nodes with smaller values of FV --
  this guarantees termination of the algorithm in amortized time O(1).

*/

#include "zlib.h"
#include "../hyper.h"

namespace hr {

int exh;

map<string, map<string,int> > rules;

/** \brief distance from the center */
#define FV master->fiftyval

/** \brief change i into a string containing a displayable character */
auto dis = [] (int i, char init='a') { return s0 + char(init + i); };

bool optimize_344 = false;

/** \brief we use a regular pattern to make sure that the directions are identified consistently.
    In {5,3,5} we can just use the Seifert-Weber space for this identification; otherwise,
    we use the field pattern. */
    
int get_id(cell *c) { 
  if(geometry == gSpace535) return 0;
  if(optimize_344 && geometry == gSpace344) {
    /* we use the 'pattern from crystal' */
    /* but it is mod 4, mod 2 is enough for us */
    int res = 0;
    int fv = c->master->fieldval;
    for(int i=0; i<4; i++) {
      res = 2 * res + (fv&1);
      fv >>= 2;
      }
    return res;
    }
  return c->master->fieldval;
  }

/** \brief aux function for find_path which limits path length 
 *  the rule is that we make some moves which decrease FV, then we make some moves which increase FV
 */

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

/** \brief aux function for find_path which limits path length 
 *  the rule is that we keep to a fixed FV-level (this works better in {x,x,3})
 */

string find_path_side(cell *x, cell *y, int steps) {
  if(x->FV != y->FV) {
    println(hlog, x, y, " steps=", steps, " d=", x->FV, " vs ", y->FV);
    exit(3);
    }
  if(x == y) return "";
  if(steps == 0) return "?";
  for(int i=0; i<S7; i++)
    if(x->move(i) && x->move(i)->FV == x->FV) {
      string ch = find_path_side(x->move(i), y, steps-1);
      if(ch == "?") continue;
      return dis(i) + ch;
      }
  return "?";
  }

/** \brief find the sequence of moves we need to take to get from y to x (x and y must be the same fv-level)
 *  return '?' if nothing found
 */

string find_path(cell *x, cell *y) {
  if(x == y) return "";

  if(geometry == gSpace353) {
    static int max_steps = -1;
    
    for(int steps=0; steps<5; steps++) {
      string f = find_path_side(x, y, steps);
      if(f != "?") {
        if(steps > max_steps) {
          println(hlog, "found a sidepath with ", max_steps = steps, " steps");
          }
        return f;
        }
      }
    
    if(max_steps < 10) {
      max_steps = 10;
      println(hlog, "failed to find_path_side");
      }
    }
  
  for(int steps=0;; steps++) {
    string f = find_path(x, y, steps);
    if(f != "?") return f;
    }
  }

/** a map of all the cells vertex-adjacent to c */
struct ext_nei_rules_t {
  vector<int> from, dir, original;
  };

/** ext_nei_rules_t need to be created only once for each get_id */
map<int, ext_nei_rules_t> ext_nei_rules;

/** aux recursive function of construct_rules: the compact variant */
void listnear_compact(cell *c, ext_nei_rules_t& e, const transmatrix& T, int id, set<cell*>& visi) {
  visi.insert(c);
  int a = 0, b = 0;
  for(int i=0; i<S7; i++) {
    bool ok = false;
    transmatrix U = T * currentmap->adj(c, i);
    for(auto v: cgi.heptshape->vertices_only) for(auto w: cgi.heptshape->vertices_only)
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
      listnear_compact(c1, e, U, id1, visi);
      }
    }
  }

/** aux recursive function of construct_rules: the maxdist variant */
void listnear_exh(cell *c, ext_nei_rules_t& e, int maxdist) {
  map<cell*, int> dist;
  map<cell*, int> origdir;
  vector<cell*> lst;

  println(hlog, "called listnear_exh for: ", c);
  
  auto enqueue = [&] (cell *c, int d, int od) {
    if(dist.count(c)) return;
    dist[c] = d;
    origdir[c] = od;
    lst.push_back(c);
    };
  
  enqueue(c, 0, -1);
  for(int k=0; k<isize(lst); k++) {
    cell *ca = lst[k];
    int di = dist[ca] + 1;
    int odi = origdir[ca];
    for(int i=0; i<S7; i++) {
      if(odi >= 0 && cgi.heptshape->dirdist[i][odi] != 1) continue;
      cell *c1 = ca->cmove(i);
      e.from.push_back(k);
      e.dir.push_back(i);
      e.original.push_back(!dist.count(c1));
      if(e.original.back() && di < maxdist) 
        enqueue(c1, di, ca->c.spin(i));
      }
    }
  }

/** \brief create ext_nei_rules_t for the given c */
void construct_rules(cell *c, ext_nei_rules_t& e) {
  e.from = {-1};
  e.dir = {-1};
  e.original = {1};
  if(!exh) {
    set<cell*> visi;
    listnear_compact(c, e, Id, 0, visi);
    }
  else {
    listnear_exh(c, e, exh);
    }
  int orgc = 0;
  for(auto i: e.original) orgc += i;
  println(hlog, "id ", get_id(c), " list length = ", isize(e.original), " original = ", orgc);
  }

/** \brief we learn that a and b are connected -- make sure that their FV's match */
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

/** \brief compute ext_nei_rules_t for the given cell, and make it into a string form; also do fix_dist */

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

/** cells become 'candidates' before their generate_ext_nei is checked in order to let them become states */
set<cell*> candidates;
vector<cell*> candidates_list;

/** the state ID for a given string returned by generate_ext_nei */
map<string, int> id_of;

/** cell representing the given state ID */
vector<cell*> rep_of;

/** current number of states */
int number_states = 0;

/** \brief for state s, child_rules[s][i] is -1 if i-th neighbor not a child; otherwise, the state index of that neighbor */
vector<vector<int> > child_rules;

/** parent direction for every state */
vector<int> parent_list;

/** \brief if child_rules[s][i] is -1, the rules to get to that neighbor */
vector<vector<string> > side_rules;

void add_candidate(cell *c) {
  if(candidates.count(c)) return;
  candidates.insert(c);
  candidates_list.push_back(c);
  }

bool single_origin = false;

/** the main function */
void test_canonical(string fname) {
  stop_game();
  reg3::reg3_rule_available = false;
  fieldpattern::use_rule_fp = true;
  fieldpattern::use_quotient_fp = true;
  start_game();

  int qc = reg3::quotient_count();
  
  vector<cell*> c0;
  
  if(optimize_344 && geometry == gSpace344) qc = 16;
  
  /* we start from a 'center' in every get_id-type */
  if(single_origin) c0 = {cwt.at};
  else if(geometry == gSpace535) {
    c0.resize(qc, cwt.at);
    }
  else {
    for(int fv=0; fv<qc; fv++) {
      cell *c = cwt.at;
      /* 100 to ensure that the FV-spheres around candidates do not interact */
      for(int i=0; i<100 || get_id(c) != fv; i++) c = c->cmove(hrand(S7));
      c->FV = 0;
      c0.push_back(c);
      }
    }

  for(cell* c: c0) add_candidate(c);

  vector<int> empty(S7);
  for(auto& e: empty) e = -1;
  println(hlog, "empty = ", empty);
  
  /** generate candidate_list using a BFS-like algorithm, starting from c0 */
  
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
  
  parent_list.resize(number_states);
  
  println(hlog, "found ", its(number_states), " states");
  
  /** generate child_rules */

  for(int i=0; i<number_states; i++) {
    cell *c = rep_of[i];

    string st = generate_ext_nei(c);
    if(!id_of.count(st) || id_of[st] != i) {
      println(hlog, "error: ext_nei changed");
      }

    for(int a=0; a<S7; a++) {
      cell *c1 = c->move(a);
      if(c1->FV < c->FV) parent_list[i] = a;
      if(c1->FV <= c->FV) continue;
      for(int b=0; b<S7; b++) {
        cell *c2 = c1->move(b);
        if(c2->FV != c->FV) continue;
        if(c2 == c) {
          string st = generate_ext_nei(c1);
          if(!id_of.count(st)) {
            println(hlog, "error: new state generated while generating child_rules");
            }
          child_rules[i][a] = id_of[st];
          }
        break;
        }
      continue;
      }
    }

  if(true) {
  
    /* minimize the automaton */

    // println(hlog, "original rules: ", child_rules);
    fflush(stdout);

    vector<int> ih(number_states, 0);
    
    int lqids = 0;
    
    for(int a=0; a<100; a++) {
      set<vector<int>> found;
      vector<vector<int>> v(number_states);
      map<vector<int>, int> ids;
      for(int i=0; i<number_states; i++) {
        vector<int> res(S7+1);
        for(int d=0; d<S7; d++) res[d] = (child_rules[i][d] != -1) ? ih[child_rules[i][d]] : -1;
        res[S7] = parent_list[i];
        v[i] = res;
        found.insert(res);
        }
      int qids = 0;
      for(auto hash: found) ids[hash] = qids++;
      println(hlog, "minimization step: ", qids, " states");
      if(qids == lqids) break;
      lqids = qids;
      for(int i=0; i<number_states; i++)
        ih[i] = ids[v[i]];
      }
    
    println(hlog, "reduced states to = ", lqids);
    vector<vector<int> > new_child_rules;
    new_child_rules.resize(lqids, empty);  
    for(int i=0; i<number_states; i++) {
      int j = ih[i];
      for(int d=0; d<S7; d++) {
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

  // for(auto& a: child_rules) for(auto i:a) print(hlog, i, ",");
  println(hlog);
  
  /* generate side rules */
  side_rules.resize(number_states);
  for(auto& s: side_rules) s.resize(S7);

  for(int i=0; i<isize(candidates_list); i++) {
    cell *c = candidates_list[i];
    string s = generate_ext_nei(c);
    if(!id_of.count(s)) println(hlog, "error: MISSING");
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
        if(!r1) { println(hlog, "error: r1 missing"); continue; }
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
        println(hlog, "conflict: ", solu, " vs ", sr, " FV = ", c->FV, " vs ", c1->FV);
        if(isize(sr) < isize(solu)) continue;
        }

      sr = solu;
      
      continue;
      }
    }

  // println(hlog, side_rules);
  
  string side_data;
  for(auto& a: side_rules) for(auto&b :a) if(b != "") side_data += b + ",";
  println(hlog, side_data);
  
  vector<short> data;
  for(auto& a: child_rules) for(auto i:a) data.push_back(i);  

  shstream ss;
  
  auto& fp = currfp;
  hwrite_fpattern(ss, fp);

  qc = isize(c0);
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
  else if(argis("-extra-verification")) {
    reg3::extra_verification++;
    }
  else if(argis("-exh")) {
    shift(); exh = argi();
    }
  else if(argis("-no-rule")) {
    reg3::reg3_rule_available = false;
    }
  else if(argis("-other-rule")) {
    reg3::reg3_rule_available = true;
    shift(); reg3::other_rule = args();    
    }
  else if(argis("-urf")) {
    cheat(); fieldpattern::use_rule_fp = true;
    }
  else if(argis("-uqf")) {
    cheat(); fieldpattern::use_quotient_fp = true;
    }
  else if(argis("-gen-rule")) {
    shift(); test_canonical(args());
    }
  else return 1;
  return 0;
  });

}
  
// 1 + 12 + 30 + 20 = 55
