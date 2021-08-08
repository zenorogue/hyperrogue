// Hyperbolic Rogue -- rule generator
// Copyright (C) 2011-2021 Zeno Rogue, see 'hyper.cpp' for details

/** \file rulegen.cpp 
 *  \brief An algorithm to create strict tree rules for arb tessellations
 */

#include "hyper.h"

namespace hr {

EX namespace rulegen {

/* limits */
EX int max_retries = 999;
EX int max_tcellcount = 1000000;
EX int max_adv_steps = 100;
EX int max_examine_branch = 5040;
EX int max_bdata = 1000;

/* other parameters */
EX int dlbonus = 0;

#if HDR
/** exception thrown by this algoritm in case of any problems */
struct rulegen_failure : hr_exception {
  rulegen_failure(string _s) : hr_exception(_s) {}
  };

/** this exception is thrown if we want to restart the computation -- this is normal, but if thrown more than max_retries times, just surrender */
struct rulegen_retry : rulegen_failure {
  rulegen_retry(string _s) : rulegen_failure(_s) {}
  };

/** this exception is thrown in case if we run into a special case that is not implemented yet */
struct rulegen_surrender : rulegen_failure {
  rulegen_surrender(string _s) : rulegen_failure(_s) {}
  };

const int MYSTERY = 31999;
const int MYSTERY_DIST = 31998;
#endif

/* === tcell === */

/** number of tcells created */
EX int tcellcount = 0;
/** number of tcells united into other tcells */
EX int tunified = 0;

#if HDR
struct tcell* tmove(tcell *c, int d);

/** rulegen algorithm works on tcells which have their own map generation */
struct tcell {
  /** tcells form a list */
  tcell *next;
  /** shape ID in arb::current */
  int id;
  /** degree */
  int type;
  /** distance from the root */
  short dist;
  /** cached code */
  short code;
  /** direction to the parent in the tree */
  short parent_dir;
  /** can we assume that dist is correct? if we assumed that the dist is correct but then find out it was wrong, throw an error */
  bool is_solid;
  bool distance_fixed;
  /** sometimes we find out that multiple tcells represent the same actual cell -- in this case we unify them; unified_to is used for the union-find algorithm */
  walker<tcell> unified_to;
  int degree() { return type; }
  connection_table<tcell> c;                           
  tcell*& move(int d) { return c.move(d); }
  tcell*& modmove(int d) { return c.modmove(d); }
  tcell* cmove(int d) { return tmove(this, d); }
  tcell* cmodmove(int d) { return tmove(this, c.fix(d)); }
  tcell() { }
  };

inline void print(hstream& hs, tcell* h) { print(hs, "P", index_pointer(h)); }

using twalker = walker<tcell>;
#endif

queue<reaction_t> fix_queue;

bool in_fixing = false;

void unify_distances(tcell *c1, tcell *c2);
void handle_distance_errors();

void process_fix_queue() {
  if(in_fixing) return;
  in_fixing = true;
  while(!fix_queue.empty()) {
    fix_queue.front()();
    fix_queue.pop();
    }
  in_fixing = false;
  }

void ufind(twalker& p) {
  if(p.at->unified_to.at == p.at) return;
  twalker p1 = p.at->unified_to;
  ufind(p1);
  p.at->unified_to = p1;
  p = p1 + p.spin;
  }

void ufindc(tcell*& c) {
  twalker cw = c; ufind(cw); c = cw.at;
  }

EX tcell *first_tcell = nullptr;

void connect_and_check(twalker p1, twalker p2);
void unify(twalker pw1, twalker pw2);

tcell *gen_tcell(int id) {
  int d = isize(arb::current.shapes[id].connections);
  auto c = tailored_alloc<tcell> (d);
  c->id = id;
  c->next = first_tcell;
  c->unified_to = twalker(c, 0);
  c->is_solid = false;
  c->distance_fixed = false;
  c->dist = MYSTERY;
  c->code = MYSTERY;
  c->parent_dir = MYSTERY;
  first_tcell = c;
  // println(hlog, c, " is a new tcell of id ", id);
  tcellcount++;
  return c;
  }

tcell* tmove(tcell *c, int d) {
  if(c->move(d)) return c->move(d);
  auto& co = arb::current.shapes[c->id].connections[d];
  auto cd = twalker(c, d);
  ufind(cd);
  tcell *c1 = gen_tcell(co.sid);
  connect_and_check(cd, twalker(c1, co.eid));
  return c1;
  }

/** check whether we have completed the vertex to the right of edge d of c */
void check_loops(twalker pw) {
  ufind(pw);
  auto& shs = arb::current.shapes;
  int id = pw.at->id;
  int valence = shs[id].vertex_valence[pw.spin];
    
  int steps = 0;
  twalker pwf = pw;
  twalker pwb = pw;
  while(true) {
    if(!pwb.peek()) break;
    pwb = pwb + wstep - 1;
    steps++;
    if(pwb == pwf) {
      if(steps == valence) return; /* that's great, we already know this loop */
      else throw hr_exception("vertex valence too small");
      }
    if(steps == valence) {
      fix_queue.push([=] { unify(pwf, pwb); });
      return;
      }
    }
  
  while(true) {
    pwf++;
    if(!pwf.peek()) break;
    pwf += wstep;
    steps++;
    if(pwb == pwf) {
      if(steps == valence) return; /* that's great, we already know this loop */
      else throw hr_exception("vertex valence too small");
      }
    if(steps == valence) {
      fix_queue.push([=] { unify(pwf, pwb); });
      return;
      }
    }
  
  if(steps == valence - 1) {
    connect_and_check(pwb, pwf);
    }
  }

void connect_and_check(twalker p1, twalker p2) {
  p1.at->c.connect(p1.spin, p2.at, p2.spin, false);
  fix_queue.push([=] { check_loops(p1); });
  fix_queue.push([=] { check_loops(p2); });
  process_fix_queue();
  }

void unify(twalker pw1, twalker pw2) {
  ufind(pw1);
  ufind(pw2);
  if(pw1.at->unified_to.at != pw1.at)
    throw hr_exception("not unified to itself");
  if(pw2.at->unified_to.at != pw2.at)
    throw hr_exception("not unified to itself");
  
  if(pw1.at == pw2.at) {
    if(pw1.spin != pw2.spin) throw hr_exception("called unify with self and wrong direction");
    return;
    }

  if(pw1.at->id != pw2.at->id)
    throw hr_exception("unifying two cells of different id's");

  auto& shs = arb::current.shapes;
  int id = pw1.at->id;
  for(int i=0; i<shs[id].size(); i++) {
    if(!pw2.peek()) {
      /* no need to reconnect */
      }
    else if(!pw1.peek()) {
      connect_and_check(pw1, pw2+wstep);
      }
    else {
      fix_queue.push([=] { unify(pw1+wstep, pw2+wstep); });
      auto ss = pw1+wstep;
      connect_and_check(pw1, pw2+wstep);
      connect_and_check(pw1, ss);
      }
    pw1++;
    pw2++;
    }
  pw2.at->unified_to = pw1 - pw2.spin;
  tunified++;
  unify_distances(pw1.at, pw2.at);
  }

EX vector<tcell*> t_origin;

void delete_tmap() {
  while(first_tcell) {
    auto second = first_tcell->next;
    tailored_delete(first_tcell);
    first_tcell = second;
    }
  tcellcount = 0;
  tunified = 0;
  t_origin.clear();
  }

/* used in the debugger */
EX vector<twalker> debuglist;

/* === distances === */

bool no_errors = false;

struct hr_solid_error : rulegen_retry {
  hr_solid_error() : rulegen_retry("solid error") {}
  };

int solid_errors;

int get_parent_dir(tcell *c);

struct shortcut {
  vector<int> pre;
  vector<int> post;
  tcell *sample;
  int delta;
  };

map<int, vector<shortcut> > shortcuts;

vector<int> root_path(twalker cw) {
  cw += wstep;
  vector<int> res;
  while(true) {
    int i = cw.at->dist == 0 ? 0 : get_parent_dir(cw.at);
    int j = cw.to_spin(i);
    res.push_back(j);
    if(cw.at->dist == 0) return res;
    cw += j;
    cw += wstep;
    }
  }

void check_solid(tcell *c, int d) {
  ufindc(c);
  if(debugflags & DF_GEOM)
    println(hlog, "solid ", c, " changes ", c->dist, " to ", d);
  if(c->dist == MYSTERY_DIST) exit(2);
  set<tcell*> seen;
  vector<twalker> walkers;
  vector<int> walkerdir = {-1};
  seen.insert(c);
  walkers.push_back(c);

  for(int j=0; j<isize(walkers); j++) {
    auto w = walkers[j];
    for(int s=0; s<w.at->type; s++) {
      twalker w1 = w + s;
      if(w1.peek() && w1.peek()->dist == w.at->dist - 1 && !seen.count(w1.peek())) {
        seen.insert(w1.peek());
        walkers.push_back(w1 + wstep);
        walkerdir.push_back(s);
        }
      }
    }

  c->dist = d;
  set<tcell*> seen2;
  vector<twalker> walkers2;
  vector<int> walkerdir2 = {-1};
  walkers2.push_back(c);
  for(int j=0; j<isize(walkers2); j++) {
    auto w = walkers2[j];
    for(int s=0; s<w.at->type; s++) {
      twalker w1 = w + s;
      if(!w1.peek()) continue;
      if(w1.peek()->dist == w.at->dist - 1 && !seen2.count(w1.peek())) {
        seen2.insert(w1.peek());
        walkers2.push_back(w1 + wstep);
        walkerdir2.push_back(s);
        if(seen.count(w1.peek()))  goto found;
        }
      }
    }

  return;

  found:

  auto at0 = walkers2.back().at;

  tcell* at = at0;

  twalker at1;
  for(int i=isize(walkers)-1; i>=1; i--) if(at == walkers[i].at) at1 = walkers[i];

  vector<int> pre;
  for(int i=isize(walkers)-1; i>=1; i--) if(at == walkers[i].at) {
    pre.push_back(walkerdir[i]); at = walkers[i].peek();
    }
  if(at != c) return;

  at = at0;
  vector<int> post;
  for(int i=isize(walkers2)-1; i>=1; i--) if(at == walkers2[i].at) {
    post.push_back(walkerdir2[i]); at = walkers2[i].peek();
    }
  if(at != c) return;
  reverse(pre.begin(), pre.end());
  reverse(post.begin(), post.end());

  int delta = at1.to_spin(walkers2.back().spin);

  for(auto s: shortcuts[c->id]) if(s.pre == pre && s.post == post) return;

  if(debugflags & DF_GEOM)
    println(hlog, "new shortcut found, pre =  ", pre, " post = ", post, " pre reaches ", at1, " post reaches ", walkers2.back(), " of type ", at1.at->id, " sample = ", c);

  shortcuts[c->id].emplace_back(shortcut{pre, post, c, delta});

  if(debugflags & DF_GEOM) println(hlog, "exhaustive search:");
  indenter ind(2);
  tcell* c1 = first_tcell;
  while(c1) {
    if(c1->id == c->id) look_for_shortcuts(c1);
    c1 = c1->next;
    }
  }

void fix_distances(tcell *c) {
  c->distance_fixed = true;
  vector<tcell*> q = {c};
  
  for(int qi=0; qi<isize(q); qi++) {
    c = q[qi];
    auto& d = c->dist;
    restart:
    for(int i=0; i<c->type; i++) {
      tcell *c1 = c->cmove(i);
      ufindc(c);
      c1 = c->cmove(i);
      if(c1->dist == MYSTERY) continue;
      auto& d1 = c1->dist;
      if(d > d1+1) { d = d1+1; goto restart; }
      if(d1 > d+1) {
        if(c1->is_solid) {
          check_solid(c1, d+1);
          solid_errors++;
          }
        d1 = d+1; 
        q.push_back(c1);
        }
      }
    }
  }

void calc_distances(tcell *c) {
  if(c->dist != MYSTERY) return;
  c->dist = MYSTERY_DIST;
  fix_distances(c);
  }

void unify_distances(tcell *c1, tcell *c2) {
  int d1 = c1->dist;
  int d2 = c2->dist;
  int d = min(d1, d2);
  c1->dist = d;
  c2->dist = d;
  if(c1->is_solid && d != d1) solid_errors++;
  if(c2->is_solid && d != d2) solid_errors++;
  c1->distance_fixed = c1->distance_fixed || c2->distance_fixed;
  c1->is_solid = c1->is_solid || c2->is_solid;
  if(c1->dist < MYSTERY) 
    fix_distances(c1);
  }

void handle_distance_errors() {
  bool b = solid_errors;
  solid_errors = 0;
  if(b && !no_errors) {
    analyzers.clear();
    throw hr_solid_error();
    }
  }

/** make sure that we know c->dist */
void be_solid(tcell *c) {
  if(c->is_solid) return;
  if(tcellcount >= max_tcellcount) 
    throw rulegen_surrender("max_tcellcount exceeded");
  ufindc(c);
  calc_distances(c);
  ufindc(c);
  look_for_shortcuts(c);
  ufindc(c);
  if(c->dist == MYSTERY_DIST) {
    println(hlog, "set solid but no dist ", c);
    exit(3);
    }
  c->is_solid = true;
  }

EX void look_for_shortcuts(tcell *c) {
  if(c->dist > 0) for(int i=0; i<isize(shortcuts[c->id]); i++) {
    auto sh = shortcuts[c->id][i];
    if(1) {
      twalker tw0(c, 0);
      twalker tw(c, 0);
      ufind(tw);
      ufind(tw0);

      vector<tcell*> opath;

      for(auto& v: sh.pre) {
        opath.push_back(tw.at);
        tw += v;
        if(!tw.peek()) goto next_shortcut;
        if(tw.peek()->dist != tw.at->dist-1) goto next_shortcut;
        ufind(tw);
        tw += wstep;
        }
      opath.push_back(tw.at);

      ufind(tw0);
      vector<tcell*> npath;
      for(auto& v: sh.post) {
        npath.push_back(tw0.at);
        tw0 += v;
        ufind(tw0);
        tw0 += wstep;
        calc_distances(tw0.at);
        }
      npath.push_back(tw0.at);
      int d = sh.delta;
      auto tw1 = tw + d;
      fix_queue.push([=] { unify(tw1, tw0); });
      process_fix_queue();
      for(auto t: npath) {
        ufindc(t);
        fix_distances(t);
        }

      ufindc(c);
      }

    next_shortcut: ;
    }
  }

/** which neighbor will become the parent of c */

int get_parent_dir(tcell *c) {
  if(c->parent_dir != MYSTERY) return c->parent_dir;
  int bestd = -1;
  vector<int> bestrootpath;
  
  look_for_shortcuts(c);
  be_solid(c);

  if(c->dist > 0) {
    auto& sh = arb::current.shapes[c->id];
    int n = sh.size();
    int k = sh.cycle_length;
    vector<int> olen;
    for(int i=0; i<k; i++) {
      vector<int> nearer;
      for(int j=0; j<n/k; j++) {
        tcell *c1 = c->cmove(i+j*k);
        be_solid(c1);
        olen.push_back(c1->dist);
        if(c1->dist < c->dist) {
          nearer.push_back(j);
          }
        }
      if(nearer.size() == 1) {bestd = i+nearer[0]*k; break; }
      if(nearer.size() == 2 && nearer[1] == nearer[0] + 1) {
        bestd = i + nearer[0] * k;
        break;
        }
      if(nearer.size() == 2 && nearer[0] == 0 && nearer[1] == n/k-1) {
        bestd = i + nearer[1] * k;
        break;
        }
      if(nearer.size() > 1) throw rulegen_failure("still confused");
      }
    if(bestd == -1) throw rulegen_failure("should not happen");
    }
    
  c->parent_dir = bestd;
  return bestd;
  }

/** determine states for tcells */
  
#if HDR
using aid_t = pair<int, int>;

struct analyzer {
  vector<twalker> spread;
  vector<int> parent_id;
  vector<int> spin;
  void add_step(int pid, int s);
  };
#endif

void analyzer::add_step(int pid, int s) {
  twalker cw = spread[pid];
  cw = cw + s;
  cw.peek();
  ufind(cw);
  cw = cw + wstep;
  spread.push_back(cw);
  parent_id.push_back(pid);
  spin.push_back(s);
  }

EX map<aid_t, analyzer> analyzers;

EX aid_t get_aid(twalker cw) {
  ufind(cw);
  auto ide = cw.at->id;
  return {ide, gmod(cw.to_spin(0), arb::current.shapes[ide].cycle_length)};
  }

EX analyzer& get_analyzer(twalker cw) {
  auto aid = get_aid(cw);
  auto& a = analyzers[aid];
  if(a.spread.empty()) {
    a.spread.push_back(cw);
    a.parent_id.push_back(-1);
    a.spin.push_back(-1);
    for(int i=0; i<cw.at->type; i++)
      a.add_step(0, i);
    }
  return a;
  }

EX vector<twalker> spread(analyzer& a, twalker cw) {
  vector<twalker> res;
  int N = isize(a.spread);
  res.reserve(N);  
  res.push_back(cw);
  for(int i=1; i<N; i++) {
    auto& r = res[a.parent_id[i]];
    ufind(r);
    auto r1 = r + a.spin[i];
    r1.peek(); ufind(r1);
    res.push_back(r1 + wstep);
    }
  return res;
  }

void extend_analyzer(twalker cw_target, int dir, int id, int mism, twalker rg) {
  if(debugflags & DF_GEOM)
    println(hlog, "extend called, cw_target = ", cw_target);
  twalker cw_conflict = cw_target + dir + wstep;
  auto &a_target = get_analyzer(cw_target);
  auto &a_conflict = get_analyzer(cw_conflict);
  // twalker model = a_target.spread[0] + dir + wstep;
  // auto res = spread(a_conflict, model);
  vector<int> ids_to_add;
  int k = id;
  while(k) {
    ids_to_add.emplace_back(a_conflict.spin[k]);
    k = a_conflict.parent_id[k];
    }
  int gid = 1 + dir;
  bool added = false;
  while(!ids_to_add.empty()) {
    int spin = ids_to_add.back();
    ids_to_add.pop_back();
    int next_gid = -1;
    for(int i=0; i<isize(a_target.parent_id); i++)
      if(a_target.parent_id[i] == gid && a_target.spin[i] == spin) {
        next_gid = i;
        }
    if(next_gid == -1) {
      next_gid = isize(a_target.parent_id);
      a_target.add_step(gid, spin);
      added = true;
      }
    gid = next_gid;
    }
  if(mism == 0 && !added) 
    throw rulegen_failure("no extension");
  }

#if HDR
using code_t = pair<aid_t, vector<int> >;

struct treestate {
  int id;
  bool known;
  vector<int> rules;
  twalker giver;
  int sid;
  int parent_dir;
  tcell* where_seen;
  code_t code;
  bool is_live;
  bool is_possible_parent;
  bool is_root;
  vector<pair<int, int>> possible_parents;
  };

static const int C_IGNORE = 0;
static const int C_CHILD = 1;
static const int C_UNCLE = 2;
static const int C_EQUAL = 4;
static const int C_NEPHEW = 6;  
static const int C_PARENT = 8;
#endif

EX vector<treestate> treestates;

set<twalker> sideswap;

/** is what on the left side, or the right side, of to_what? */

int get_side(tcell *what, tcell *to_what) {
  twalker w(what, -1);
  twalker tw(to_what, -1);
  auto adv = [] (twalker& cw) {
    int d = get_parent_dir(cw.at);
    cw.spin = d;
    cw += wstep;
    };
  while(w.at != tw.at) {
    ufind(w); ufind(tw);
    if(w.at->dist > tw.at->dist)
      adv(w);
    else if(w.at->dist < tw.at->dist)
      adv(tw);
    else {
      adv(w); adv(tw);
      }
    }
  if(w.spin == -1 || tw.spin == -1) return 0;
  int d = get_parent_dir(w.at);
  
  if(d >= 0) {
    twalker last(w.at, d);
    return last.to_spin(w.spin) - last.to_spin(tw.spin);
    }

  // failed to solve this in the simple way (ended at the root) -- go around the tree
  twalker wl(what, get_parent_dir(what));
  twalker wr = wl;
  auto go = [&] (twalker& cw, int delta) {
    int d = get_parent_dir(cw.at);
    if(cw.spin == d || get_parent_dir(cw.cpeek()) == (cw+wstep).spin)
      cw += wstep;
    cw+=delta;
    };
  while(true) {
    go(wl, -1);
    go(wr, +1);
    if(wl.at == to_what) return +1;
    if(wr.at == to_what) return -1;
    }
  }

code_t id_at_spin(twalker cw) {
  code_t res;
  ufind(cw);
  res.first = get_aid(cw);
  auto& a = get_analyzer(cw);
  vector<twalker> sprawl = spread(a, cw);
  int id = 0;
  for(auto cs: sprawl) {
    be_solid(cs.at);
    be_solid(cw.at);
    ufind(cw);
    ufind(cs);
    int x;
    int pid = a.parent_id[id];
    if(pid > -1 && (res.second[pid] != C_CHILD)) {
      x = C_IGNORE;
      }
    else {
      int p = get_parent_dir(cs.at);
      if(p >= 0 && get_parent_dir(cs.at) == cs.spin)
        x = C_CHILD;
      else {
        auto cs2 = cs + wstep;
        ufind(cs); ufind(cs2); be_solid(cs2.at);
        fix_distances(cs.at);
        int y = cs.at->dist - cs.peek()->dist;
        if(y == 1) x = C_NEPHEW;
        else if(y == 0) x = C_EQUAL;
        else if(y == -1) x = C_UNCLE;
        else throw rulegen_failure("distance problem y=" + its(y) + lalign(0, " cs=", cs, " cs2=", cs2, " peek=", cs.peek(), " dist=", cs.at->dist, " dist2=", cs2.at->dist));
        auto gs = get_side(cs.at, cs.peek());
        if(gs == 0 && x == C_UNCLE) x = C_PARENT;
        if(gs > 0) x++;
        }
      }
    res.second.push_back(x);
    id++;
    }
  return res;
  }

map<code_t, int> code_to_id;
  
EX pair<int, int> get_code(tcell *c) {
  if(c->code != MYSTERY) {
    int bestd = c->parent_dir;
    if(bestd == -1) bestd = 0;
    return {bestd, c->code};
    }

  be_solid(c);

  int bestd = get_parent_dir(c);
  if(bestd == -1) bestd = 0;
  indenter ind(2);

  code_t v = id_at_spin(twalker(c, bestd));
  
  if(code_to_id.count(v)) {
    c->code = code_to_id[v];
    return {bestd, code_to_id[v]};
    }
  
  int id = isize(treestates);
  code_to_id[v] = id;
  if(c->code != MYSTERY && (c->code != id || c->parent_dir != bestd)) exit(1);
  c->code = id;

  treestates.emplace_back();
  auto& nts = treestates.back();
  
  nts.id = id;
  nts.code = v;
  nts.where_seen = c;
  nts.known = false;
  nts.is_live = true;
  
  return {bestd, id};
  }

/* == rule generation == */

struct mismatch_error : rulegen_retry {
  mismatch_error() : rulegen_retry("mismatch error") {}
  };

struct double_edges: rulegen_surrender {
  double_edges() : rulegen_surrender("double edges detected") {}
  };

EX int rule_root;

vector<int> gen_rule(twalker cwmain);

EX int try_count;
vector<tcell*> important;

vector<tcell*> cq;

#if HDR
/* special codes */
static const int DIR_UNKNOWN = -1;
static const int DIR_MULTI_GO_LEFT = -2;
static const int DIR_MULTI_GO_RIGHT = -3;
static const int DIR_LEFT = -4;
static const int DIR_RIGHT = -5;
static const int DIR_PARENT = -6;
#endif

vector<int> gen_rule(twalker cwmain) {
  vector<int> cids;
  for(int a=0; a<cwmain.at->type; a++) {
    auto front = cwmain+a;
    tcell *c1 = front.cpeek();
    be_solid(c1);
    if(a == 0 && cwmain.at->dist) { cids.push_back(DIR_PARENT); continue; }
    if(c1->dist <= cwmain.at->dist) { cids.push_back(DIR_UNKNOWN); continue; }
    auto co = get_code(c1);
    auto& d1 = co.first;
    auto& id1 = co.second;
    if(c1->cmove(d1) != cwmain.at || c1->c.spin(d1) != front.spin) {
      cids.push_back(DIR_UNKNOWN); continue;
      }
    cids.push_back(id1);
    }
  return cids;
  }

void rules_iteration_for(tcell *c) {
  indenter ri(2);
  auto co = get_code(c);
  auto& d = co.first;
  auto& id = co.second;
  twalker cwmain(c,d);
  ufind(cwmain);

  vector<int> cids = gen_rule(cwmain);
  auto& ts = treestates[id];

  if(!ts.known) {
    ts.known = true;
    ts.rules = cids;
    ts.giver = cwmain;
    ts.sid = cwmain.at->id;
    ts.parent_dir = cwmain.spin;
    ts.is_root = c->dist == 0;
    for(int d=0; d<c->type; d++)
      cq.push_back(c->cmove(d));
    }
  else if(ts.rules != cids) {
    handle_distance_errors();
    auto& r = ts.rules;
    if(debugflags & DF_GEOM) {
      println(hlog, "merging ", ts.rules, " vs ", cids);
      println(hlog, "C ", treestates[id].code, " [", id, "]");
      }
    int mismatches = 0;
    for(int z=0; z<isize(cids); z++) {
      if(r[z] == cids[z]) continue;
      if(r[z] < 0 || cids[z] < 0) 
        throw rulegen_failure("neg rule mismatch");

      auto& c1 = treestates[r[z]].code.second;
      auto& c2 = treestates[cids[z]].code.second;
      if(debugflags & DF_GEOM) {
        println(hlog, "direction ", z, ":");
        println(hlog, "A ", treestates[r[z]].code, " [", r[z], "]");
        println(hlog, "B ", treestates[cids[z]].code, " [", cids[z], "]");
        }
      
      if(isize(c1) != isize(c2)) {
        throw rulegen_failure("length mismatch");
        }
      for(int k=0; k<isize(c1); k++) {
        if(c1[k] == C_IGNORE || c2[k] == C_IGNORE) continue;
        if(c1[k] != c2[k]) {
          if(debugflags & DF_GEOM) {
            println(hlog, "code mismatch (", c1[k], " vs ", c2[k], " at position ", k, " out of ", isize(c1), ")");
            println(hlog, "rulegiver = ", treestates[id].giver, " c = ", cwmain);
            println(hlog, "gshvid = ", c->id);
            println(hlog, "cellcount = ", tcellcount, "-", tunified, " codes discovered = ", isize(treestates));
            }

          extend_analyzer(cwmain, z, k, mismatches, treestates[id].giver);
          mismatches++;
          }
        }
      }

    debuglist = { cwmain, ts.giver };
    
    if(mismatches)
      throw mismatch_error();
    
    throw rulegen_failure("no mismatches?!");
    }
  }

void minimize_rules() {
  if(debugflags & DF_GEOM)
    println(hlog, "minimizing rules...");
  int next_id = isize(treestates);

  vector<int> new_id(next_id);

  map<aid_t, int> new_id_of;
  
  int new_ids = 0;
  
  for(int id=0; id<next_id; id++) {
    auto aid = get_aid(treestates[id].giver);
    
    if(!new_id_of.count(aid)) new_id_of[aid] = new_ids++;
    new_id[id] = new_id_of[aid];
    }
  
  int last_new_ids = 0;
  
  while(new_ids > last_new_ids && new_ids < next_id) {
  
    last_new_ids = new_ids;

    map<vector<int>, int> hashes;
    
    new_ids = 0;
    
    auto last_new_id = new_id;

    for(int id=0; id<next_id; id++) {
      vector<int> hash;
      hash.push_back(last_new_id[id]);
      auto& ts = treestates[id];
      for(auto& r: ts.rules)
        if(r >= 0) hash.push_back(last_new_id[r]);
        else hash.push_back(r);
      if(!hashes.count(hash)) 
        hashes[hash] = new_ids++;
      
      new_id[id] = hashes[hash];
      }
    }

  if(debugflags & DF_GEOM)
    println(hlog, "final new_ids = ", new_ids, " / ", next_id);

  if(1) {
    vector<int> old_id(new_ids, -1);
    for(int i=0; i<next_id; i++) if(old_id[new_id[i]] == -1) old_id[new_id[i]] = i;
    
    for(int i=0; i<new_ids; i++) treestates[i] = treestates[old_id[i]];
    for(int i=0; i<new_ids; i++) treestates[i].id = i;
    treestates.resize(new_ids);
    for(auto& ts: treestates) {
      for(auto& r: ts.rules)
        if(r >= 0) r = new_id[r];
      }
    
    for(auto& p: code_to_id) p.second = new_id[p.second];
    }
  }

void find_possible_parents() {

  for(auto& ts: treestates) {
    ts.is_possible_parent = false;
    for(int r: ts.rules) 
      if(r == DIR_PARENT) 
        ts.is_possible_parent = true;
    }
  while(true) {
    int changes = 0;
    for(auto& ts: treestates) ts.possible_parents.clear();
    for(auto& ts: treestates)
      if(ts.is_possible_parent) {
        int rid = 0;
        for(int r: ts.rules) {          
          if(r >= 0) treestates[r].possible_parents.emplace_back(ts.id, rid);
          rid++;
          }
        }
    for(auto& ts: treestates)
      if(ts.is_possible_parent && ts.possible_parents.empty()) {
        ts.is_possible_parent = false;
        changes++;
        }
    if(!changes) break;
    }
  
  int pp = 0;
  for(auto& ts: treestates) if(ts.is_possible_parent) pp++;
  if(debugflags & DF_GEOM)
    println(hlog, pp, " of ", isize(treestates), " states are possible_parents");
  }

/* == branch testing == */

struct bad_tree : rulegen_retry {
  bad_tree() : rulegen_retry("bad tree") {}
  };

bool equiv(twalker w1, twalker w2);

inline bool IS_DIR_MULTI(int d) { return among(d, DIR_MULTI_GO_LEFT, DIR_MULTI_GO_RIGHT); }
struct branchdata {
  int id;
  int dir;
  twalker at;
  int temporary;
  void step() { 
    if(treestates[id].rules[dir] < 0)
      throw rulegen_failure("invalid step");
    id = treestates[id].rules[dir]; dir = 0; at += wstep;
    auto co = get_code(at.at);
    auto& d1 = co.first;
    auto& id1 = co.second;
    if(id != id1 || d1 != at.spin) {
      important.push_back(at.at);
      if(debugflags & DF_GEOM)
        println(hlog, "expected ", id, " found ", id1, " at ", at);
      important.push_back(at.at->cmove(get_parent_dir(at.at)));
      throw bad_tree();
      }
    }
  void spin(int i) {
    at += i;
    dir += i;
    dir = gmod(dir, isize(treestates[id].rules));
    }
  void spin_full(int i) {
    spin(i);
    while(IS_DIR_MULTI(treestates[id].rules[dir]))
      spin(i);
    }
  };

inline void print(hstream& hs, const branchdata& bd) { print(hs, "[", bd.id,":",bd.dir, " ", bd.at, ":", bd.temporary, "]"); }

/* we need to be careful with multiple edges */

bool paired(twalker w1, twalker w2) {
  if(w1 + wstep == w2) return true;
  if(w1.cpeek() == w2.at && w2.cpeek() == w1.at) {
    return true;
    }
  return false;
  }

bool equiv(twalker w1, twalker w2) {
  if(w1 == w2) return true;
  if(w1.at == w2.at && w1.cpeek() == w2.cpeek()) {
    return true;
    }
  return false;
  }

void advance(vector<branchdata>& bdata, branchdata at, int dir, bool start_forward, bool stack, int distlimit) {
  if(start_forward) {
    at.step();
    at.spin_full(dir);
    }
  else {
    at.spin_full(dir);
    }
  vector<branchdata> b;
  int steps = 0;
  while(true) {
    steps++; if(steps == max_adv_steps) 
      throw rulegen_failure("max_adv_steps exceeded");
    auto& ts = treestates[at.id];
    auto r = ts.rules[at.dir];
    if(r < 0) {
      at.temporary = 0;
      b.push_back(at);
      break;
      }
    else if(!treestates[r].is_live) {
      advance(b, at, dir, true, false, distlimit);
      if(b.back().dir == 0)
        b.pop_back();
      else
        advance(b, at, -dir, true, true, distlimit);
      at.spin_full(dir);
      }
    else {
      at.step();
      if(at.at.at->dist < distlimit || !ts.is_live) at.spin_full(dir);
      else {
        at.temporary = dir;
        b.push_back(at);
        break;
        }
      }
    }
  if(stack) {
    while(b.size()) { bdata.push_back(b.back()); b.pop_back(); }
    }
  else {
    for(auto& bd: b) bdata.push_back(bd);
    }
  }

map<int, branchdata> split;

void assign_lr(branchdata bd, int dir) {
  if(dir) { bd.spin_full(dir); if(bd.dir == 0) bd.dir = bd.at.at->type; }
  auto& r = treestates[bd.id].rules;
  for(int i=0; i<isize(r); i++) {
    if(!among(r[i], DIR_UNKNOWN, DIR_LEFT, DIR_RIGHT)) continue;
    int val = i < bd.dir ? DIR_LEFT : DIR_RIGHT;
    if(r[i] == DIR_UNKNOWN)
      r[i] = val;
    else if(r[i] != val) {
      if(debugflags & DF_GEOM) {
        println(hlog, "state ", bd.id, " index ", i, ":", bd.dir, "/", bd.at.at->type, " was ", split[bd.id]);
        println(hlog, important);
        }
      important.push_back(bd.at.at);
      important.push_back(split[bd.id].at.at);
      throw mismatch_error();
      }
    }
  split[bd.id] = bd;
  }

set<vector<int> > branch_hashes;

void examine_branch(int id, int left, int right) {
  auto rg = treestates[id].giver;
  if(debugflags & DF_GEOM)
    println(hlog, "need to examine branches ", tie(left, right), " of ", id, " starting from ", rg);
  vector<branchdata> bdata;
  int dist_at = rg.at->dist;
  while(left != right) {
    /* can be false in case of multi-edges */
    if(treestates[id].rules[left] >= 0) {      

      if(bdata.size() && bdata.back().dir == 0)
        bdata.pop_back();
      else {
        auto bl = branchdata{id, left, rg+left, dist_at+dlbonus};
        advance(bdata, bl, -1, true, true, dist_at+5);
        }
      }
    left++;
    if(left == rg.at->type) left = 0;
    if(treestates[id].rules[left] >= 0) {
      auto br = branchdata{id, left, rg+left, dist_at+dlbonus};
      advance(bdata, br, +1, true, false, dist_at+5);
      }
    }
  int steps = 0;
  while(true) {
    steps++;
    if(steps == max_examine_branch)
      throw rulegen_failure("max_examine_branch exceeded");
    
    if(isize(bdata) > max_bdata)
      throw rulegen_failure("max_bdata exceeded");

    /* advance both */
    vector<branchdata> bdata2;
    int advcount = 0, eatcount = 0;
    for(int i=0; i<isize(bdata); i+=2) {
      if(!bdata[i].temporary && !bdata[i+1].temporary && paired(bdata[i].at, bdata[i+1].at) && min(bdata[i].at.at->dist, bdata[i+1].at.at->dist) <= dist_at) {
        advcount++;
        if(bdata2.size() && !bdata2.back().temporary && equiv(bdata2.back().at, bdata[i].at)) { 
          assign_lr(bdata[i], 0);
          eatcount++; bdata2.pop_back(); 
          }
        else
          advance(bdata2, bdata[i], -1, false, true, dist_at+dlbonus);
        if(i+2 < isize(bdata) && !bdata[i+1].temporary && !bdata[i+2].temporary && equiv(bdata[i+1].at, bdata[i+2].at)) { 
          assign_lr(bdata[i+1], +1);
          eatcount++; i += 2; bdata2.push_back(bdata[i+1]); 
          }
        else
          advance(bdata2, bdata[i+1], +1, false, false, dist_at+dlbonus);
        }
      else {
        if(bdata[i].temporary && bdata[i].at.at->dist <= dist_at+dlbonus-2) {
           advcount++;
           advance(bdata2, bdata[i], bdata[i].temporary, false, bdata[i].temporary < 0, dist_at+dlbonus);
           }
        else bdata2.push_back(bdata[i]);

        if(bdata[i+1].temporary && bdata[i+1].at.at->dist <= dist_at+3) {
           advcount++;
           advance(bdata2, bdata[i+1], bdata[i+1].temporary, false, bdata[i+1].temporary < 0, dist_at+dlbonus);
           }
        else bdata2.push_back(bdata[i+1]);
        }
      }
    bdata = bdata2;
    if(!advcount) dist_at++;  
    if(advcount) {
      vector<int> hash;
      for(int i=0; i<isize(bdata); i++) {
        hash.push_back(bdata[i].id);
        hash.push_back(bdata[i].dir);
        hash.push_back(bdata[i].temporary);
        hash.push_back(bdata[i].at.at->dist - dist_at);
        }
      if(branch_hashes.count(hash)) {
        return;
        }
      branch_hashes.insert(hash);
      }
    }
  }

/* == main algorithm == */

void clear_codes() {
  treestates.clear();
  code_to_id.clear();
  auto c = first_tcell;
  while(c) {
    c->code = MYSTERY;
    c = c->next;
    }
  }

void rules_iteration() {
  clear_codes();
  
  cq = important;
  
  if(debugflags & DF_GEOM)
    println(hlog, "important = ", cq);

  for(int i=0; i<isize(cq); i++) {
    rules_iteration_for(cq[i]);
    }

  handle_distance_errors();
  if(debugflags & DF_GEOM)
    println(hlog, "number of treestates = ", isize(treestates));
  rule_root = get_code(t_origin[0]).second;
  if(debugflags & DF_GEOM)
    println(hlog, "rule_root = ", rule_root);

  int N = isize(important);
  
  for(int id=0; id<isize(treestates); id++) {
    if(!treestates[id].known) {
      important.push_back(treestates[id].where_seen);
      if(debugflags & DF_GEOM)
        println(hlog, "no rule found for ", id);
      continue;
      }
    }

  if(isize(important) != N) 
    throw mismatch_error();

  int new_deadends = -1;
  
  while(new_deadends) {

    new_deadends = 0;
    
    for(int id=0; id<isize(treestates); id++) {
      auto& ts = treestates[id];
      if(!ts.known) continue;
      if(!ts.is_live) continue;
      int children = 0;
      for(int i: ts.rules) if(i >= 0 && treestates[i].is_live) children++;
      if(!children)
        treestates[id].is_live = false, new_deadends++;
      }
    
    if(debugflags & DF_GEOM)
      println(hlog, "deadend states found: ", new_deadends);
    }
  
  for(int id=0; id<isize(treestates); id++) {
    auto& rg = treestates[id].giver;
    auto& r = treestates[id].rules;

    for(int p=0; p<2; p++)
    for(int it=0; it<isize(r); it++) {
      for(int i=0; i<isize(r); i++) {
        int i1 = gmod(i+1, isize(r));
        if((rg+i).peek() == (rg+i1).peek()) {
          if(r[i1] == DIR_UNKNOWN && (r[i] >= (p?DIR_UNKNOWN:0) || r[i] == DIR_PARENT || r[i] == DIR_MULTI_GO_LEFT))
            r[i1] = DIR_MULTI_GO_LEFT;
          if(r[i] == DIR_UNKNOWN && (r[i1] >= 0 || r[i1] == DIR_PARENT || r[i+1] == DIR_MULTI_GO_RIGHT))
            r[i] = DIR_MULTI_GO_RIGHT;
          }
        }
      }
    }

  // print_rules();
  
  handle_distance_errors();
  branch_hashes.clear();

  for(int id=0; id<isize(treestates); id++) if(treestates[id].is_live) {
    auto& r = treestates[id].rules;
    int last_live_branch = -1;
    int first_live_branch = -1;
    for(int i=0; i<isize(r); i++)
      if(r[i] >= 0 && treestates[r[i]].is_live) {
        if(first_live_branch == -1) first_live_branch = i;
        if(last_live_branch >= 0)
          examine_branch(id, last_live_branch, i);
        else for(int a=0; a<i; a++) 
          if(r[a] == DIR_UNKNOWN) r[a] = DIR_LEFT;      
        last_live_branch = i;
        }
    if(treestates[id].is_root) examine_branch(id, last_live_branch, first_live_branch);
    for(int a=last_live_branch; a<isize(r); a++)
      if(r[a] == DIR_UNKNOWN) r[a] = DIR_RIGHT;
    }
  
  handle_distance_errors();
  if(isize(important) != N) 
    throw mismatch_error();

  minimize_rules();
  find_possible_parents();
  
  for(int id=0; id<isize(treestates); id++) {
    auto& ts = treestates[id];
    for(auto& r: ts.rules) if(r == DIR_UNKNOWN)
      throw rulegen_failure("UNKNOWN remaining");
    }

  if(isize(important) != N) 
    throw mismatch_error();
  handle_distance_errors();
  }

void clear_tcell_data() {
  auto c = first_tcell;
  while(c) {
    c->is_solid = false;
    // c->dist = MYSTERY;
    c->parent_dir = MYSTERY;
    c->code = MYSTERY;
    c->distance_fixed = false;
    c = c->next;
    }
  for(auto& c: t_origin) c->dist = 0;
  }

void cleanup() {
  clear_tcell_data();
  analyzers.clear();
  code_to_id.clear();
  split.clear();
  important.clear();
  shortcuts.clear();
  }

void clear_all() {  
  treestates.clear();
  cleanup();
  }

bool double_edges_check(cell *c, set<int>& visited) {
  int i = shvid(c);
  if(visited.count(i)) return false;
  visited.insert(i);
  for(int j=0; j<c->type; j++) {
    cellwalker cw(c, j);
    bool on = true;
    if(double_edges_check(cw.cpeek(), visited)) return true;
    int qty = 0;
    for(int k=0; k<=c->type; k++) {
      bool on2 = (cw+k).cpeek() == cw.cpeek();
      if(on != on2) qty++;
      on = on2;
      }
    if(qty > 2) return true;
    }
  return false;
  }

EX void generate_rules() {

  delete_tmap();

  if(!arb::in()) try {
    arb::convert::convert();
    }
  catch(hr_exception& e) {
    throw rulegen_surrender("conversion failure");
    }
  
  clear_all();

  analyzers.clear();
  split.clear();

  t_origin.clear();
  for(auto& ts: arb::current.shapes) {
    tcell *c = gen_tcell(ts.id);
    c->dist = 0;
    t_origin.push_back(c);
    }
  
  set<int> visited;
  if(double_edges_check(currentmap->gamestart(), visited))
    throw double_edges();
   
  try_count = 0;
  
  important = t_origin;
  
  retry:  
  try {
    rules_iteration();
  }
  catch(rulegen_retry& e) {
    try_count++;
    if(try_count >= max_retries)
      throw;
    if(debugflags & DF_GEOM) println(hlog, "attempt: ", try_count);
    auto c = first_tcell;
    while(c) {
      c->is_solid = false;
      c->parent_dir = MYSTERY;
      c->code = MYSTERY;
      c = c->next;
      }
    goto retry;
    }  
  }

int reclevel;

void build_test();

/* == hrmap_rulegen == */

struct hrmap_rulegen : hrmap {
  hrmap *base;
  heptagon *origin;

  heptagon* gen(int s, int d, bool c7) {
    int t = arb::current.shapes[treestates[s].sid].size();
    heptagon *h = init_heptagon(t);
    if(c7) h->c7 = newCell(t, h);
    h->distance = d;
    h->fieldval = s;
    h->zebraval = treestates[s].sid;
    h->s = hsA;
    return h;
    }
  
  ~hrmap_rulegen() { 
    clearfrom(origin);   
    }

  hrmap_rulegen() {
    origin = gen(rule_root, 0, true);
    origin->s = hsOrigin;
    }

  hrmap_rulegen(heptagon *h) {
    origin = h;
    }
  
  heptagon *getOrigin() override { 
    return origin;
    }
  
  int get_rule(heptspin hs) {
    int s = hs.at->fieldval;
    return treestates[s].rules[hs.spin];
    }
  
  static void hsconnect(heptspin a, heptspin b) {
    a.at->c.connect(a.spin, b.at, b.spin, false);
    }

  void group_connect(heptspin a, heptspin b) {
    /* go leftmost with a */
    while(get_rule(a) == DIR_MULTI_GO_LEFT || get_rule(a-1) == DIR_MULTI_GO_RIGHT)
      a--;
    /* go rightmost with b */
    while(get_rule(b) == DIR_MULTI_GO_RIGHT || get_rule(b+1) == DIR_MULTI_GO_LEFT)
      b++;
    int gr = 0;
    // verify_connection(a, b);
    while(true) {
      hsconnect(a, b); gr++;
      bool can_a = get_rule(a) == DIR_MULTI_GO_RIGHT || get_rule(a+1) == DIR_MULTI_GO_LEFT;
      if(can_a) a++;
      bool can_b = get_rule(b) == DIR_MULTI_GO_LEFT || get_rule(b-1) == DIR_MULTI_GO_RIGHT;
      if(can_b) b--;
      if(can_a && can_b) continue;
      if(can_a || can_b) 
        throw rulegen_failure("multi disagreement");
      break;
      }      
    }
  
  heptagon *create_step(heptagon *h, int d) override {
    heptspin hs(h, d);
    int r = get_rule(hs);
    indenter ind(2);
    if(hlog.indentation >= 6000)
      throw rulegen_failure("failed to create_step");
    if(r >= 0) {
      auto h1 = gen(r, h->distance + 1, h->c7);
      auto hs1 = heptspin(h1, 0);
      // verify_connection(hs, hs1);
      hsconnect(hs, hs1);
      return h1;
      }
    else if(r == DIR_PARENT) {
      auto& hts = treestates[h->fieldval];
      auto& choices = hts.possible_parents;
      if(choices.empty()) throw rulegen_failure("no possible parents");
      auto selected = hrand_elt(choices);
      auto h1 = gen(selected.first, h->distance - 1, h->c7);
      auto hs1 = heptspin(h1, selected.second);
      hsconnect(hs, hs1);
      return h1;
      }
    else if(r == DIR_UNKNOWN)
      throw rulegen_failure("UNKNOWN rule remained");
    else if(r == DIR_MULTI_GO_LEFT) {
      // hs = (hs - 1) + wstep;
      hsconnect(hs, hs - 1 + wstep - 1);
      return h->move(d);
      }
    else if(r == DIR_MULTI_GO_RIGHT) {
      // hs = (hs + 1) + wstep;
      hsconnect(hs, hs + 1 + wstep + 1);
      return h->move(d);
      }
    else if(r == DIR_LEFT || r == DIR_RIGHT) {
      heptspin hs1 = hs;
      int delta = r == DIR_LEFT ? -1 : 1;
      int rev = (DIR_LEFT ^ DIR_RIGHT ^ r);
      while(IS_DIR_MULTI(get_rule(hs1))) hs1 += delta;
      hs1 += delta;
      while(true) {
        int r1 = get_rule(hs1);
        if(r1 == rev) {
          group_connect(hs, hs1);
          return hs1.at;
          }
        else if(IS_DIR_MULTI(r1)) {
          hs1 += delta;
          }
        else if(r1 == r || r1 == DIR_PARENT || r1 >= 0) {
          hs1 += wstep;
          while(get_rule(hs1) == (r == DIR_RIGHT ? DIR_MULTI_GO_RIGHT : DIR_MULTI_GO_LEFT)) {
            hs1 += delta;
            }
          hs1 += delta;
          }
        else throw rulegen_failure("bad R1");
        }
      }
    else throw rulegen_failure("bad R");
    throw rulegen_failure("impossible");
    }
  
  int get_arb_dir(int s, int dir) {
    int sid = treestates[s].sid;
    int N = arb::current.shapes[sid].size();
    return gmod(dir + treestates[s].parent_dir, N);
    }
  
  transmatrix adj(heptagon *h, int dir) override {
    if(h->fieldval == -1)
      return arb::get_adj(arb::current_or_slided(), h->zebraval, dir, -1, -1);

    int s = h->fieldval;
    int dir0 = get_arb_dir(s, dir);
    
    int dir1 = -1;
    int sid1 = -1;
    
    if(h->c.move(dir)) {
      auto s1 = h->c.move(dir)->fieldval;
      dir1 = get_arb_dir(s1, h->c.spin(dir));
      sid1 = treestates[s1].sid;
      }

    return arb::get_adj(arb::current_or_slided(), treestates[s].sid, dir0, sid1, dir1);
    }

  int shvid(cell *c) override {
    return c->master->zebraval;
    }

  transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
    return relative_matrix_recursive(h2, h1);
    }
  
  hyperpoint get_corner(cell *c, int cid, ld cf) override {
    if(c->master->fieldval == -1) {
      auto& sh = arb::current_or_slided().shapes[c->master->zebraval];
      cid = gmod(cid, sh.size());
      return normalize(C0 + (sh.vertices[cid] - C0) * 3 / cf);
      }
    int s = c->master->fieldval;
    auto& sh = arb::current_or_slided().shapes[c->master->zebraval];
    auto dir = get_arb_dir(s, cid);

    return normalize(C0 + (sh.vertices[dir] - C0) * 3 / cf);
    }

  void find_cell_connection(cell *c, int d) override { 
    if(c->master->cmove(d) == &oob) {
      c->c.connect(d, &out_of_bounds, 0, false);
      }
    else hrmap::find_cell_connection(c, d); 
    }
  
  bool strict_tree_rules() override { return true; }

  virtual bool link_alt(heptagon *h, heptagon *alt, hstate firststate, int dir) override {
    auto& hts = treestates[h->fieldval];
    int psid = hts.sid;
    
    if(firststate == hsOrigin) {
      alt->s = hsOrigin;
      for(auto& ts: treestates) if(ts.sid == psid && ts.is_root) {
        alt->fieldval = ts.id;
        // ts.parent_dir should be 0, but anyway
        altmap::relspin(alt) = gmod(ts.parent_dir-hts.parent_dir, isize(hts.rules));
        return true;
        }
      return false;
      }

    int odir = hts.parent_dir + dir;
    
    int cl = arb::current.shapes[psid].cycle_length;

    vector<int> choices;
    for(auto& ts: treestates)
      if(ts.is_possible_parent && ts.sid == psid)
        if(gmod(ts.parent_dir - odir, cl) == 0)
          choices.push_back(ts.id);
    alt->fieldval = hrand_elt(choices, -1);
    alt->s = hsA;
    if(alt->fieldval == -1) return false;
    altmap::relspin(alt) = dir;
    return true;
    }
  };

EX int get_arb_dir(cell *c, int dir) {
  return ((hrmap_rulegen*)currentmap)->get_arb_dir(c->master->fieldval, dir);
  }

EX hrmap *new_hrmap_rulegen_alt(heptagon *h) {
  return new hrmap_rulegen(h);
  }

EX hrmap *new_hrmap_rulegen() { return new hrmap_rulegen(); }

EX int get_state(cell *c) {
  return c->master->fieldval;
  }

string rules_known_for = "unknown";
string rule_status;

EX bool known() {
  return arb::current.have_tree || rules_known_for == arb::current.name;
  }

EX bool prepare_rules() {
  if(known()) return true;
  try {
    generate_rules();
    rules_known_for = arb::current.name;
    rule_status = XLAT("rules generated successfully: %1 states using %2-%3 cells", 
      its(isize(treestates)), its(tcellcount), its(tunified));
    if(debugflags & DF_GEOM) println(hlog, rule_status);
    return true;
    }
  catch(rulegen_retry& e) {
    rule_status = XLAT("too difficult: %1", e.what());
    }
  catch(rulegen_surrender& e) {
    rule_status = XLAT("too difficult: %1", e.what());
    }
  catch(rulegen_failure& e) {
    rule_status = XLAT("bug: %1", e.what());
    }
  if(debugflags & DF_GEOM) println(hlog, rule_status);
  return false;
  }

#if CAP_COMMANDLINE
int args() {
  using namespace arg;
           
  if(0) ;

  else if(argis("-rulegen")) {
    PHASEFROM(3);
    prepare_rules();
    }
  else if(argis("-rulegen-cleanup"))
    cleanup();
  else if(argis("-rulegen-play")) {
    PHASEFROM(3);
    if(prepare_rules()) {
      stop_game();
      arb::convert::activate();
      start_game();
      }
    }
  else if(argis("-d:rulegen")) {
    launch_dialog(show);
    }
  else return 1;
  return 0;
  }

auto hooks_arg = 
    addHook(hooks_args, 100, args);
#endif

auto hooks = addHook(hooks_configfile, 100, [] {
      param_i(max_retries, "max_retries");
      param_i(max_tcellcount, "max_tcellcount")
      ->editable(0, 16000000, 100000, "maximum cellcount", "controls the max memory usage of conversion algorithm -- the algorithm fails if exceeded", 'c');
      param_i(max_adv_steps, "max_adv_steps");
      param_i(max_examine_branch, "max_examine_branch");
      param_i(max_bdata, "max_bdata");
      param_i(dlbonus, "dlbonus");
    });

EX void parse_treestate(arb::arbi_tiling& c, exp_parser& ep) {
  if(!c.have_tree) {
    c.have_tree = true;
    treestates.clear();
    rule_root = 0;
    }
  treestates.emplace_back();
  auto& ts = treestates.back();
  ts.id = isize(treestates) - 1;

  ts.sid = ep.iparse();
  ts.parent_dir = 0;
  if(!arb::correct_index(ts.sid, isize(c.shapes)))
    throw hr_parse_exception("incorrect treestate index at " + ep.where());

  int N = c.shapes[ts.sid].size();
  int qparent = 0, sumparent = 0;
  for(int i=0; i<N; i++) {
    ep.force_eat(","); ep.skip_white();
    if(ep.eat("PARENT")) ts.rules.push_back(DIR_PARENT);
    else if(ep.eat("LEFT")) ts.rules.push_back(DIR_LEFT);
    else if(ep.eat("RIGHT")) ts.rules.push_back(DIR_RIGHT);
    else if(ep.eat("MLEFT")) ts.rules.push_back(DIR_MULTI_GO_LEFT);
    else if(ep.eat("MRIGHT")) ts.rules.push_back(DIR_MULTI_GO_RIGHT);
    else { int i = ep.iparse(); ts.rules.push_back(i); }
    }
  for(int i=0; i<N; i++) {
    if(ts.rules[i] == DIR_PARENT) qparent++, sumparent += i;
    }
  ts.is_root = qparent == 0;
  if(qparent > 1) throw hr_parse_exception("multiple parent at " + ep.where());
  if(qparent == 1) {
    ts.parent_dir = sumparent;
    println(hlog, "before: ", ts.rules);
    std::rotate(ts.rules.begin(), ts.rules.begin() + sumparent, ts.rules.end());
    println(hlog, "after : ", ts.rules);
    }
  ep.force_eat(")");
  }

EX void verify_parsed_treestates() {
  if(rule_root < 0 || rule_root >= isize(treestates))
    throw hr_parse_exception("undefined treestate as root");
  for(auto& ts: treestates) for(auto& r: ts.rules) {
    if(r < 0 && !among(r, DIR_PARENT, DIR_LEFT, DIR_RIGHT, DIR_MULTI_GO_LEFT, DIR_MULTI_GO_RIGHT))
      throw hr_parse_exception("negative number in treestates");
    if(r > isize(treestates))
      throw hr_parse_exception("undefined treestate");
    }
  for(auto& sh: arb::current.shapes) sh.cycle_length = sh.size();
  find_possible_parents();
  }

EX void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(1);
  dialog::init(XLAT("strict tree maps"));

  dialog::addHelp(XLAT(
    "Strict tree maps are generated using a more powerful algorithm.\n\nThis algorithms supports horocycles and knows the expansion rates of various "
    "tessellations (contrary to the basic implementation of Archimedean, tes, and unrectified/warped/untruncated tessellations).\n\nYou can convert mostly any "
    "non-spherical periodic 2D tessellation to strict tree based.\n\nSwitching the map format erases your map."));

  if(kite::in()) {
    dialog::addInfo("not available in aperiodic tessellations");
    dialog::addBack();
    dialog::display();
    }
  else if(WDIM == 3) {
    dialog::addInfo("not available in 3D tessellations");
    dialog::addBack();
    dialog::display();
    }

  dialog::addBoolItem(XLAT("in tes internal format"), arb::in(), 't');
  dialog::add_action([] {
    if(!arb::in()) {
      arb::convert::convert();
      arb::convert::activate();
      start_game();
      rule_status = XLAT("converted successfully -- %1 cell types", its(isize(arb::current.shapes)));
      rules_known_for = "unknown";
      }
    else if(arb::convert::in()) {
      stop_game();
      geometry = arb::convert::base_geometry;
      variation = arb::convert::base_variation;
      start_game();
      }
    else {
      addMessage(XLAT("cannot be disabled for this tiling"));
      }
    });

  dialog::addBoolItem(XLAT("strict tree based"), currentmap->strict_tree_rules(), 's');
  dialog::add_action([] {
    if(!currentmap->strict_tree_rules()) {
      if(prepare_rules()) {
        println(hlog, "prepare_rules returned true");
        stop_game();
        arb::convert::activate();
        start_game();
        delete_tmap();
        }
      }
    else if(arb::current.have_tree) {
      addMessage(XLAT("cannot be disabled for this tiling"));
      }
    else {
      rules_known_for = "unknown";
      rule_status = "manually disabled";
      stop_game();
      start_game();
      }
    });

  add_edit(max_tcellcount);

  dialog::addBreak(100);

  dialog::addHelp(rule_status);
  dialog::items.back().color = known() ? 0x00FF00 : rules_known_for == "unknown" ? 0xFFFF00 : 0xFF0000;

  dialog::addBreak(100);
  dialog::addBack();
  dialog::display();
  }

EX }
}
