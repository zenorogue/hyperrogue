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
EX int max_getside = 10000;
EX int rulegen_timeout = 60;
EX int max_shortcut_length = 1200;
EX int first_restart_on = 512;

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
const int MYSTERY_LARGE = 31999999;
#endif

/* === tcell === */

/** number of tcells created */
EX int tcellcount = 0;
/** number of tcells united into other tcells */
EX int tunified = 0;
/** hard cases for get_parent_dir */
EX int hard_parents = 0;
/** the number of roots with single live branches */
EX int single_live_branches = 0;
/** the number of roots with double live branches */
EX int double_live_branches = 0;
/** the number of treestates pre-minimization */
EX int states_premini = 0;

#if HDR
/** change some flags -- they usually make it worse */
static const flagtype w_numerical = Flag(1); /*< build trees numerically */
static const flagtype w_near_solid = Flag(2); /*< solid's pre-parent is also solid */
static const flagtype w_no_shortcut = Flag(3); /*< generate no shortcuts */
static const flagtype w_no_restart = Flag(4); /*< do not restart at powers of two */
static const flagtype w_no_sidecache = Flag(5); /*< do not cache get_side */
static const flagtype w_no_relative_distance = Flag(6); /*< do not build relative distances into codes */
static const flagtype w_examine_once = Flag(7); /*< restart after first conflict found in analysis */
static const flagtype w_examine_all = Flag(8); /*< focus on all conflicts found in analysis even if we know them */
static const flagtype w_conflict_all = Flag(9); /*< full extension in case of conflicts */
static const flagtype w_parent_always = Flag(10); /*< always consider the full parent rule */
static const flagtype w_parent_reverse = Flag(11); /*< reverse paths in parent_dir */
static const flagtype w_parent_side = Flag(12); /*< allow side paths in parent_dir */
static const flagtype w_parent_never = Flag(13); /*< never consider the full parent rule */
static const flagtype w_always_clean = Flag(14); /*< restart following phases after any distance errors */
static const flagtype w_single_origin = Flag(15); /*< consider only one origin */
static const flagtype w_slow_side = Flag(16); /*< do not try get_side optimization */
static const flagtype w_bfs = Flag(17); /*< compute distances using BFS */
static const flagtype w_numerical_fix = Flag(18); /*< when doing numerical, find out filled vertices */
static const flagtype w_known_structure = Flag(19); /*< do flagless first, then use the known distances from there (handled in ruletest) */
static const flagtype w_known_distances = Flag(20); /*< with, use the actual distances */
static const flagtype w_no_smart_shortcuts = Flag(21); /*< disable the 'smart shortcut' optimization */
static const flagtype w_less_smart_retrace = Flag(22); /*< stop early when examining smart shortcut retraction */
static const flagtype w_less_smart_advance = Flag(23); /*< stop early when examining smart shortcut advancement */
static const flagtype w_no_queued_extensions = Flag(24); /*< consider extensions one by one */
static const flagtype w_no_branch_skipping = Flag(24); /*< do not skip branches */

/* for 3D honeycombs */
static const flagtype w_skip_transducers = Flag(32); /*< skip the transducer test */
static const flagtype w_skip_transducer_loops = Flag(33); /*< skip loops during the transducer test */
static const flagtype w_skip_transducer_terminate = Flag(34); /*< skip termination during the transducer test */
static const flagtype w_r3_all_errors = Flag(35); /*< consider all errors for R3 */
static const flagtype w_r3_no_road_shortcuts = Flag(36); /*< consider all errors for R3 */
static const flagtype w_ignore_transducer_dist = Flag(37); /*< ignore distance errors while testing the transducers */
#endif

/** these control the output */
EX flagtype rdebug_flags;

EX flagtype flags = 0;

EX int64_t movecount;

EX int current_getside, current_examine_branch;

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
  int code;
  /** direction to the parent in the tree */
  short parent_dir;
  /** direction to the OLD parent in the tree */
  short old_parent_dir;
  /** direction to anyone closer */
  short any_nearer;
  /** can we assume that dist is correct? if we assumed that the dist is correct but then find out it was wrong, throw an error */
  bool is_solid;
  bool distance_fixed;
  /** is side info cached? */
  unsigned long long known_sides;
  /** which side is it */
  unsigned long long which_side;
  /** sometimes we find out that multiple tcells represent the same actual cell -- in this case we unify them; unified_to is used for the union-find algorithm */
  walker<tcell> unified_to;
  int degree() { return type; }
  connection_table<tcell> c;
  tcell*& move(int d) { movecount++; return c.move(d); }
  tcell*& modmove(int d) { movecount++; return c.modmove(d); }
  tcell* cmove(int d) { movecount++; return tmove(this, d); }
  tcell* cmodmove(int d) { movecount++; return tmove(this, c.fix(d)); }
  tcell() { }
  };

inline void print(hstream& hs, tcell* h) { print(hs, "P", index_pointer(h)); }

using twalker = walker<tcell>;
#endif

EX hookset<void(int, twalker)> hooks_gen_tcell;

queue<reaction_t> fix_queue;

void push_unify(twalker a, twalker b) {
  if(WDIM == 3 && a != b) {
    println(hlog, "pushing unify of ", tie(a, b));
    throw hr_exception("bad unify");
    }
  if(a.at->id != b.at->id) {
    throw hr_exception("queued bad unify");
    }
  fix_queue.push([=] { unify(a, b); });
  }

bool in_fixing = false;

void process_fix_queue() {
  if(in_fixing) return;
  in_fixing = true;
  while(!fix_queue.empty()) {
    fix_queue.front()();
    fix_queue.pop();
    }
  in_fixing = false;
  }

EX void ufind(twalker& p) {
  if(p.at->unified_to.at == p.at) return;
  twalker p1 = p.at->unified_to;
  ufind(p1);
  p.at->unified_to = p1;
  p = p1 + p.spin;
  }

EX void ufindc(tcell*& c) {
  twalker cw = c; ufind(cw); c = cw.at;
  }

EX tcell *first_tcell = nullptr;

// sometimes the standard x+wstep returns nullptr because of unification
twalker addstep(twalker x) {
  x.cpeek();
  ufind(x);
  return x + wstep;
  }

EX int less_states;

EX int number_of_types() {
  if(arb::in() || WDIM == 2) return isize(arb::current.shapes);
  if(WDIM == 3) return gcd(reg3::quotient_count_sub(), less_states);
  throw hr_exception("unknown number_of_types");
  }

EX int get_id(cell *c) {
  if(arb::in() || WDIM == 2) return shvid(c);
  if(WDIM == 3) return zgmod(reg3::get_aid(c), less_states);
  throw hr_exception("unknown get_id");
  }

int shape_size(int id) {
  if(arb::in() || WDIM == 2) return isize(arb::current.shapes[id].connections);
  if(WDIM == 3) return reg3::get_size_of_aid(id);
  throw hr_exception("unknown shape_size");
  }

int cycle_size(int id) {
  if(arb::in() || WDIM == 2) return arb::current.shapes[id].cycle_length;
  if(WDIM == 3) return reg3::get_size_of_aid(id);
  throw hr_exception("unknown shape size");
  }

tcell *gen_tcell(int id) {
  int d = shape_size(id);
  auto c = tailored_alloc<tcell> (d);
  c->id = id;
  c->next = first_tcell;
  c->unified_to = twalker(c, 0);
  c->is_solid = false;
  c->distance_fixed = false;
  c->dist = MYSTERY;
  c->code = MYSTERY_LARGE;
  c->parent_dir = MYSTERY;
  c->old_parent_dir = MYSTERY;
  c->known_sides = 0;
  c->which_side = 0;
  first_tcell = c;
  // println(hlog, c, " is a new tcell of id ", id);
  tcellcount++;
  return c;
  }

EX map<cell*, tcell*> cell_to_tcell;
EX map<tcell*, cell*> tcell_to_cell;

void numerical_fix(twalker pw) {
  auto& shs = arb::current.shapes;
  int id = pw.at->id;
  int valence = shs[id].vertex_valence[pw.spin];
    
  int steps = 0;
  twalker pwf = pw;
  twalker pwb = pw;
  vector<twalker> deb = {pwb};
  while(true) {
    if(!pwb.peek()) break;
    pwb = pwb + wstep - 1;
    deb.push_back(pwb);
    steps++;
    if(pwb == pwf) {
      if(steps == valence) return; /* that's great, we already know this loop */
      else {
        debuglist = deb;
        println(hlog, "deb = ", deb);
        throw rulegen_failure("vertex valence too small");
        }
      }
    if(steps == valence) {
      println(hlog, "steps = ", steps, " valence = ", valence, " (D)");
      debuglist = deb;
      println(hlog, "deb = ", deb);
      throw rulegen_failure("incorrect looping");
      }
    }
  
  while(true) {
    pwf++;
    if(!pwf.peek()) break;
    pwf += wstep;
    steps++;
    if(pwb == pwf) {
      if(steps == valence) return; /* that's great, we already know this loop */
      else throw rulegen_failure("vertex valence too small");
      }
    if(steps == valence) {
      println(hlog, "steps = ", steps, " valence = ", valence, " (C)");
      debuglist = deb;
      println(hlog, "deb = ", deb);
      throw rulegen_failure("incorrect looping");
      }
    }
  
  if(steps == valence - 1) {
    pwb.at->c.connect(pwb.spin, pwf.at, pwf.spin, false);
    fix_distances(pwb.at);
    }
  }

tcell* tmove(tcell *c, int d) {
  if(d<0 || d >= c->type) throw hr_exception("wrong d");
  if(c->c.move(d)) return c->c.move(d);
  if(flags & (w_numerical | w_known_structure)) {
    indenter ind(2);
    if(flags & w_known_structure) swap_treestates();
    cell *oc = tcell_to_cell[c];
    int d1 = d;

    if(flags & w_known_structure) {
      d1 = gmod(d1 - treestates[oc->master->fieldval].parent_dir, oc->type);
      }

    cell *oc1 = oc->cmove(d1);
    auto& c1 = cell_to_tcell[oc1];
    if(!c1) {
      c1 = gen_tcell(get_id(oc1));
      tcell_to_cell[c1] = oc1;
      if(flags & w_known_distances)
        c1->dist = oc1->master->distance;
      }

    int d2 = oc->c.spin(d1);
    if(flags & w_known_structure) {
      d2 = gmod(d2 + treestates[oc1->master->fieldval].parent_dir, oc1->type);
      }

    c->c.connect(d, cell_to_tcell[oc1], d2, false);
    /* if(arb::current.shapes[c->id].connections[d].eid != d2)
      throw hr_exception("Wrong type!"); */

    if(flags & w_known_structure)
      swap_treestates();

    if(!(flags & w_known_distances))
      fix_distances(c);

    ensure_shorter(c1);

    if(flags & w_numerical_fix) {
      numerical_fix(twalker(c, d));
      numerical_fix(twalker(c, d) + wstep);
      }
    return c1;
    }
  auto cd = twalker(c, d);
  ufind(cd);
  auto& co = arb::current.shapes[c->id].connections[cd.spin];
  tcell *c1 = gen_tcell(co.sid);
  c1->c.connect(co.eid, cd.at, cd.spin, false);
  callhooks(hooks_gen_tcell, 1, twalker(c1, co.eid));
  connect_and_check(cd, twalker(c1, co.eid));
  return c1;
  }

/** check whether we have completed the vertex to the right of edge d of c */
void check_loops(twalker pw) {
  if(GDIM == 3) throw hr_exception("check_loops called");
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
      push_unify(pwf, pwb);
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
      push_unify(pwf, pwb);
      return;
      }
    }
  
  if(steps == valence - 1) {
    callhooks(hooks_gen_tcell, 2, pwb);
    connect_and_check(pwb, pwf);
    fix_distances(pwb.at);
    }
  }

EX void connect_and_check(twalker p1, twalker p2) {
  if(GDIM == 3) throw hr_exception("connect_and_check called");
  ufind(p1); ufind(p2);
  p1.at->c.connect(p1.spin, p2.at, p2.spin, false);
  fix_queue.push([=] { check_loops(p1); });
  fix_queue.push([=] { check_loops(p2); });
  process_fix_queue();
  }

EX void unify(twalker pw1, twalker pw2) {
  ufind(pw1);
  ufind(pw2);
  if(pw1 == pw2) return;
  if(GDIM == 3) throw hr_exception("unify called");
  callhooks(hooks_gen_tcell, 3, pw1);
  callhooks(hooks_gen_tcell, 4, pw2);
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

  if((pw1.spin - pw2.spin) % cycle_size(pw1.at->id))
    throw hr_exception("unification spin disagrees with cycle_length");

  unify_distances(pw1.at, pw2.at, pw2.spin - pw1.spin);

  for(int i=0; i<pw1.at->type; i++) {
    if(!pw2.peek()) {
      /* no need to reconnect */
      }
    else if(!pw1.peek()) {
      connect_and_check(pw1, pw2+wstep);
      }
    else {
      push_unify(pw1+wstep, pw2+wstep);
      auto ss = pw1+wstep;
      connect_and_check(pw1, pw2+wstep);
      connect_and_check(pw1, ss);
      }
    pw1++;
    pw2++;
    }
  pw2.at->unified_to = pw1 - pw2.spin;
  tunified++;
  fix_distances(pw1.at);
  }

EX vector<twalker> t_origin;

EX void delete_tmap() {
  clean_analyzers();
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

EX vector<twalker> solid_errors_list;

/* === distances === */

bool no_errors = false;

struct hr_solid_error : rulegen_retry {
  hr_solid_error() : rulegen_retry("solid error") {}
  };

/** since the last restart */
EX int solid_errors;

/** total solid errors */
EX int all_solid_errors;

/** the next distance to warn about */
EX int next_distance_warning;

/** current distance warnings */
EX int distance_warnings;

#if HDR
struct shortcut {
  vector<int> pre;
  vector<int> post;
  tcell *sample;
  int delta;
  int last_dir;
  };
#endif

EX vector<vector<unique_ptr<shortcut>> > shortcuts;

vector<reaction_t> skipped_branches;
using branch_check = tuple<int, int, int>;
set<branch_check> checks_to_skip;

vector<int> root_path(twalker& cw) {
  cw += wstep;
  vector<int> res;
  while(true) {
    if(cw.at->dist == 0) {
      int j = cw.to_spin(0);
      res.push_back(j);
      return res;
      }
    else {
      auto cwd = get_parent_dir(cw);
      int j = cw.to_spin(cwd.spin);
      res.push_back(j);
      cw = cwd + wstep;
      }
    }
  }

EX void calc_distances(tcell *c);

EX void shortcut_found(tcell *c, tcell *alt, vector<twalker> &walkers, vector<twalker> &walkers2, const vector<int>& walkerdir, const vector<int>& walkerdir2, int wpos) {

  vector<int> pre;
  for(int i=wpos; i>=1; i--) pre.push_back(walkerdir[i]);
  reverse(pre.begin(), pre.end());

  vector<int> post;
  for(int i=isize(walkers2)-1; i>=1; i--) post.push_back(walkerdir2[i]);
  reverse(post.begin(), post.end());

  int delta = walkers[wpos].to_spin(walkers2.back().spin);

  for(auto& s: shortcuts[c->id]) if(s->pre == pre && s->post == post) {
    if(rdebug_flags & 16)
      println(hlog, "already knew that ", pre, " ~ ", post);
    return;
    }

  if(debugflags & DF_GEOM)
    println(hlog, "new shortcut found, pre =  ", pre, " post = ", post, " pre reaches ", walkers[wpos], " post reaches ", walkers2.back(), " of type ", walkers[wpos].at->id, " sample = ", c);

  if(isize(pre) > max_shortcut_length) {
    debuglist = { c };
    throw rulegen_failure("shortcut too long");
    }

  shortcuts[c->id].emplace_back(unique_ptr<shortcut> (new shortcut));
  auto& sh = shortcuts[c->id].back();
  sh->pre = pre;
  sh->post = post;
  sh->sample = c;
  sh->delta = delta;
  sh->last_dir = c->any_nearer;
  auto& sh1 = *sh;

  if(debugflags & DF_GEOM) println(hlog, "exhaustive search:");
  indenter ind(2);
  tcell* c1 = first_tcell;
  while(c1) {
    if(c1->id == c->id) look_for_shortcuts(c1, sh1);
    c1 = c1->next;
    }
  }

EX void find_new_shortcuts(tcell *c, int d, tcell *alt, int newdir, int delta) {

  if(!solid_errors) debuglist = {};
  solid_errors_list.push_back(c);
  solid_errors++;
  all_solid_errors++;
  check_timeout(); /* may freeze no this */
  if(flags & w_no_shortcut) return;
  if(flags & w_known_distances) return;

  ufindc(c);
  if(debugflags & DF_GEOM)
    println(hlog, "solid ", c, " changes ", c->dist, " to ", d, " alt=", alt);

  if(newdir == c->any_nearer) {
    if(debugflags & DF_GEOM)
      println(hlog, "same direction");
    return;
    }
  /* {
    throw rulegen_failure("direction did not change");
    } */

  if(c->dist == MYSTERY)
    throw rulegen_failure("find_new_shortcuts with MYSTERY distance");

  map<tcell*, int> seen;
  vector<twalker> walkers;
  vector<int> walkerdir = {-1};
  seen[c] = 0;
  walkers.push_back(c);
  
  for(int j=0; j<isize(walkers); j++) {
    auto w = walkers[j];
    if(w.at->dist == 0) break;
    for(int s=0; s<w.at->type; s++) {
      twalker w1 = w + s;
       if(w1.peek() && w1.spin == w.at->any_nearer && !seen.count(w1.peek())) {
        seen[w1.peek()] = isize(walkers);
        walkers.push_back(w1 + wstep);
        walkerdir.push_back(s);
        }
      }
    }

  set<tcell*> seen2; /* prevent loops */
  c->dist = d;
  c->any_nearer = gmod(newdir, c->type);
  fix_distances(c);
  vector<twalker> walkers2;
  vector<int> walkerdir2 = {-1};
  walkers2.push_back(twalker(alt, delta));
  for(int j=0; j<isize(walkers2); j++) {
    auto w = walkers2[j];
    if(w.at->dist == 0) break;
    for(int s=0; s<w.at->type; s++) {
      twalker w1 = w + s;
      ufind(w1);
      if(w1.spin != w.at->any_nearer) continue;
      if(!w1.peek()) continue;
      if(seen2.count(w1.peek())) break;
      seen2.insert(w1.peek());
      if(true) {
        walkers2.push_back(w1 + wstep);
        walkerdir2.push_back(s);
        if(seen.count(w1.peek())) {
          shortcut_found(c, alt, walkers, walkers2, walkerdir, walkerdir2, seen[w1.peek()]);
          return;
          }
        }
      }
    }
  }

EX void remove_parentdir(tcell *c) {
  if(c->parent_dir != MYSTERY) {
    clear_sidecache_and_codes();
    c->old_parent_dir = c->parent_dir;
    }
  c->parent_dir = MYSTERY;
  c->code = MYSTERY_LARGE;
  for(int i=0; i<c->type; i++) if(c->move(i)) {
    if(c->move(i)->parent_dir) c->move(i)->old_parent_dir = c->move(i)->parent_dir;
    c->move(i)->parent_dir = MYSTERY;
    c->move(i)->code = MYSTERY_LARGE;
    }
  }

queue<tcell*> bfs_queue;

EX void fix_distances(tcell *c) {
  if(flags & w_bfs) while(true) {
    if(in_fixing) return;
    ufindc(c);
    if(c->dist != MYSTERY) return;
    if(tcellcount >= max_tcellcount) throw rulegen_surrender("max_tcellcount exceeded");
    if(bfs_queue.empty()) throw rulegen_failure("empty bfs queue");
    auto c1 = bfs_queue.front();
    ufindc(c1);
    bfs_queue.pop();
    for(int i=0; i<c1->type; i++) {
      tcell *c2 = c1->cmove(i);
      if(c2->dist == MYSTERY) {
        c2->dist = c1->dist + 1;
        bfs_queue.push(c2);
        }
      }
    }
  c->distance_fixed = true;
  if(flags & w_known_distances) return;
  vector<tcell*> q = {c};
  
  for(int qi=0; qi<isize(q); qi++) {
    c = q[qi];
    restart:
    for(int i=0; i<c->type; i++) {
      if(!c->move(i)) continue;
      ufindc(c);

      auto process_edge = [&] (twalker tgtw, twalker srcw) {
        tcell *tgt = tgtw.at;
        tcell *src = srcw.at;
        auto& tgt_d = tgt->dist;
        int new_d = src->dist + 1;
        if(tgt_d > new_d) {
          if(tgt->is_solid)
            find_new_shortcuts(tgt, new_d, tgt, tgtw.spin, 0);
          ufind(tgtw); tgt = tgtw.at;
          remove_parentdir(tgt);
          tgt_d = new_d;
          tgt->any_nearer = tgtw.spin;
          if(new_d >= next_distance_warning) {
            if(new_d >= MYSTERY-1) throw rulegen_failure("distance limit exceeded");
            if(next_distance_warning < 10000) next_distance_warning *= 2;
            else if(next_distance_warning < 20000) next_distance_warning = 20000;
            else next_distance_warning = new_d; distance_warnings++;
            }
          return true;
          }
        return false;
        };

      twalker ci1(c->cmove(i), c->c.spin(i));
      twalker ci(c, i);

      if(process_edge(ci, ci1)) goto restart;
      if(process_edge(ci1, ci)) q.push_back(ci1.at);
      }
    }
  }

void calc_distances(tcell *c) {
  if(c->dist != MYSTERY) return;
  fix_distances(c);
  }

EX void unify_distances(tcell *c1, tcell *c2, int delta) {
  int d1 = c1->dist;
  int d2 = c2->dist;
  int d = min(d1, d2);
  if(c1->is_solid && d != d1) { solid_errors++; find_new_shortcuts(c1, d, c2, c2->any_nearer - delta, +delta); remove_parentdir(c1); }
  if(d != d1) fix_distances(c1);
  c1->dist = d;
  if(c2->is_solid && d != d2) { solid_errors++; find_new_shortcuts(c2, d, c1, c1->any_nearer + delta, -delta); remove_parentdir(c2); }
  if(d != d2) fix_distances(c2);
  c2->dist = d;
  c1->distance_fixed = c2->distance_fixed = c1->distance_fixed || c2->distance_fixed;
  c1->is_solid = c2->is_solid = c1->is_solid || c2->is_solid;
  }

EX void handle_distance_errors() {
  bool b = solid_errors;
  solid_errors = 0;
  if(b && !no_errors) {
    clear_sidecache_and_codes();
    if(flags & w_always_clean) clean_data();
    debuglist = solid_errors_list;
    solid_errors_list = {};
    checks_to_skip.clear();
    throw hr_solid_error();
    }
  b = distance_warnings;
  distance_warnings = 0;
  if(b && !no_errors) {
    clean_parents();
    checks_to_skip.clear();
    throw rulegen_retry("distance exceeded");
    }
  }

/** make sure that we know c->dist */
EX void be_solid(tcell *c) {
  if(c->is_solid) return;
  if(tcellcount >= max_tcellcount) 
    throw rulegen_surrender("max_tcellcount exceeded");
  ufindc(c);
  calc_distances(c);
  ufindc(c);
  look_for_shortcuts(c);
  ufindc(c);
  if(c->dist == MYSTERY) {
    if(debugflags & DF_GEOM)
      println(hlog, "set solid but no dist ", c);
    debuglist = { c };
    throw rulegen_failure("set solid but no dist");
    }
  c->is_solid = true;
  if(c->dist > 0 && !(flags & w_near_solid) && c->any_nearer >= 0 && c->any_nearer < c->type) {
    tcell *c1 = c->move(c->any_nearer);
    if(c1) be_solid(c1);
    }
  }

EX void look_for_shortcuts(tcell *c, shortcut& sh) {
  if(c->dist <= 0) return;

  if(!(flags & w_no_smart_shortcuts)) {
    twalker tw0(c, 0);
    twalker tw(c, 0);
    ufind(tw);
    ufind(tw0);

    for(auto& v: sh.pre) {
      tw += v;
      if(!tw.peek() && !(flags & w_less_smart_retrace)) return;
      ufind(tw);
      tw += wstep;
      calc_distances(tw.at);
      }

    int more_steps = isize(sh.post);
    int d = cycle_size(c->id);
    if(sh.last_dir % d < c->any_nearer % d) more_steps--;

    tw += sh.delta;

    for(auto it = sh.post.rbegin(); it != sh.post.rend(); it++) {
      auto& v = *it;
      ufind(tw);
      if(!tw.peek() && tw.at->dist + more_steps > c->dist && !(flags & w_less_smart_advance)) return;
      tw += wstep;
      calc_distances(tw.at);
      more_steps--;
      tw -= v;
      }

    process_fix_queue();
    if(tw.at->dist < c->dist) {
      if(debugflags & DF_GEOM)
        println(hlog, "smart shortcut updated ", c->dist, " to ", tw.at->dist);
      }
    push_unify(tw, tw0);

    process_fix_queue();
    }

  else {
    twalker tw0(c, 0);
    twalker tw(c, 0);
    ufind(tw);
    ufind(tw0);

    vector<tcell*> opath;

    for(auto& v: sh.pre) {
      opath.push_back(tw.at);
      tw += v;
      if(!tw.peek()) return;
      if(tw.peek()->dist != tw.at->dist-1) return;
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
    if(tw1.at->id != tw0.at->id)
      println(hlog, "ERROR: improper shortcut");
    else
      push_unify(tw1, tw0);
    process_fix_queue();
    for(auto t: npath) {
      ufindc(t);
      fix_distances(t);
      }

    ufindc(c);
    }
  }

EX void look_for_shortcuts(tcell *c) {
  if(c->dist > 0)
  for(int i=0; i<isize(shortcuts[c->id]); i++)
    look_for_shortcuts(c, *shortcuts[c->id][i]);
  }

EX void ensure_shorter(twalker cw) {
  /* if cw.peek() has shorter dist, ensure it exists */
  /* only with w_known_distances */
  if(flags & w_known_distances) {
    swap_treestates();
    int d1 = cw.spin;
    auto oc = tcell_to_cell[cw.at];
    d1 = gmod(d1 - treestates[oc->master->fieldval].parent_dir, oc->type);
    cell *c1 = oc->cmove(d1);
    // println(hlog, "cw=", cw, " oc=", oc, " c1=", c1, " d=", oc->master->distance, "=", cw.at->dist, " vs ", c1->master->distance);
    bool ok = c1->master->distance < cw.at->dist;
    swap_treestates();
    if(ok)
      cw.at->cmove(cw.spin);
    }
  }

void trace_root_path(vector<int>& rp, twalker cw) {
  auto d = cw.peek()->dist;
  cw += wstep; auto scw = cw;

  bool side = (flags & w_parent_side);

  next:
  if(d > 0) {
    ufind(cw);
    handle_distance_errors();
    auto cwd = get_parent_dir(cw);
    for(int i=0; i<cw.at->type; i++) {
      if((!side) && (cw+i) != cwd) continue;
      tcell *c1 = cwd.peek();
      if(!c1) continue;
      be_solid(c1);
      handle_distance_errors();
      if(c1->dist < d) {
        rp.push_back(i);
        cw += i;
        cw += wstep;
        d--;
        goto next;
        }
      }
    }
  if(d > 0) {
    debuglist = {scw};
    throw rulegen_failure("should not happen [trace]");
    }
  rp.push_back(cw.to_spin(0));
  if(flags & w_parent_reverse) reverse(rp.begin(), rp.end());
  }

EX int parent_updates;

/** which neighbor will become the parent of c */

EX twalker get_parent_dir(twalker& cw) {
  tcell*& c = cw.at;
  if(c->parent_dir != MYSTERY) return twalker(c, c->parent_dir);
  int bestd = -1;
  vector<int> bestrootpath;
  
  be_solid(c);

  auto oc = c;

  if(c->dist > 0) {
    int n = c->type;
    int k = cycle_size(c->id);

    vector<int> nearer;

    auto beats = [&] (int i, int old) {
      if(old == -1) return true;
      if(i%k != old%k) return i%k < old%k;
      return true;
      /* if(old < i) old += n;
      return old <= i+n/2; */
      };

    int d = c->dist;

    for(int i=0; i<n; i++) {
      ensure_shorter(cw+i);
      tcell *c1 = c->cmove(i);
      be_solid(c1);
      if(rdebug_flags & 16) println(hlog, "direction = ", i, " is ", c1, " distance = ", c1->dist);
      if(c1->dist < d) nearer.push_back(i);
      ufind(cw); if(d != cw.at->dist || oc != cw.at) return get_parent_dir(cw);
      }

    if(rdebug_flags & 16) println(hlog, "nearer = ", nearer, " n=", n, " k=", k);

    bool failed = false;
    if(flags & w_parent_always) {failed = true; goto resolve; }

    // celebrity identification problem

    for(auto ne: nearer)
      if(beats(ne, bestd))
        bestd = ne;

    if(rdebug_flags & 16) for(auto ne: nearer) println(hlog, "beats", tie(ne, bestd), " = ", beats(ne, bestd));

    for(auto ne: nearer)
      if(ne != bestd && beats(ne, bestd))
        failed = true;

    if(failed) {

      if(flags & w_parent_never) {
        debuglist = { c };
        throw rulegen_failure("still confused");
        }

      resolve:
      hard_parents++;
      vector<int> best;
      int bestfor = nearer[0];
      trace_root_path(best, twalker(c, nearer[0]));

      for(auto ne1: nearer) {
        vector<int> other;
        trace_root_path(other, twalker(c, ne1));
        if(other < best) best = other, bestfor = ne1;
        }

      bestd = bestfor;
      }

    if(bestd == -1) {
      debuglist = { c };
      throw rulegen_failure("should not happen");
      }
    }
    
  if(rdebug_flags & 16) println(hlog, "set parent_dir to ", bestd);
  c->parent_dir = bestd;

  if(c->old_parent_dir != MYSTERY && c->old_parent_dir != bestd && c == oc) {
    c->any_nearer = c->old_parent_dir;
    find_new_shortcuts(c, c->dist, c, bestd, 0);
    }

  parent_updates++;

  return twalker(c, bestd);
  }

/** determine states for tcells */
  
#if HDR
using aid_t = pair<int, int>;

/* for leaves, id equals MYSTERY and dir equals treestate ID for this code */

struct analyzer_state {
  int analyzer_id;
  int id, dir;
  map<int, analyzer_state*> substates;
  analyzer_state() { id = MYSTERY; dir = MYSTERY_LARGE; } // for(int i=0; i<10; i++) substates[i] = nullptr; }
  vector<twalker> inhabitants;
  };

#endif

int next_analyzer_id;

EX vector<vector<analyzer_state*>> analyzers;
EX vector<analyzer_state*> all_analyzers;

analyzer_state *alloc_analyzer() {
  auto a = new analyzer_state;
  a->analyzer_id = next_analyzer_id++;
  all_analyzers.push_back(a);
  return a;
  }

EX aid_t get_aid(twalker cw) {
  ufind(cw);
  auto ide = cw.at->id;
  return {ide, gmod(cw.to_spin(0), cycle_size(ide))};
  }

vector<int> gen_rule(twalker cwmain, int id);

void extend_analyzer(twalker cwmain, int z, twalker giver) {
  ufind(giver);
  ufind(cwmain);

  vector<twalker> giver_sprawl, main_sprawl, sub_sprawl;
  vector<analyzer_state*> giver_states, main_states, sub_states;
  
  id_at_spin(cwmain, main_sprawl, main_states);

  id_at_spin((cwmain+z)+wstep, sub_sprawl, sub_states);

  id_at_spin((giver+z)+wstep, giver_sprawl, giver_states);

  int currently_at = 1+z;

  vector<int> idlist;

  for(int i=0;; i++) {
    if(i == isize(sub_states) || i == isize(giver_states)) {
      /* may happen if something changed but not updated */

      cwmain.at->code = MYSTERY_LARGE;
      giver.at->code = MYSTERY_LARGE;
      (cwmain+z+wstep).at->code = MYSTERY_LARGE;
      (giver+z+wstep).at->code = MYSTERY_LARGE;

      throw rulegen_retry("reached the end");
      }
    if(giver_states[i] != sub_states[i]) {
      i--;
      while(i != 0) {
        idlist.push_back(i);
        i = giver_states[i]->id;
        }
      break;
      }
    }

  reverse(idlist.begin(), idlist.end());

  auto v = main_states.back();
  auto v1 = v;
  int new_id = isize(main_states)-1;

  for(auto l: idlist) {

    /* check if already tested */
    for(int u=1; u<isize(main_states); u++)
      if(main_states[u]->id == currently_at && main_states[u]->dir == sub_states[l]->dir) {
        currently_at = u;
        goto next_l;
        }

    v->id = currently_at;
    v->dir = sub_states[l]->dir;

    for(auto p: sub_states[l]->substates) {
      int i = p.first;
      if(sub_states[l]->substates[i] == sub_states[l+1]) {
        v = v->substates[i] = alloc_analyzer();
        currently_at = new_id++;
        goto next_l;
        }
      }

    next_l: ;
    }

  update_all_codes(v1);
  }

#if HDR

struct treestate {
  int id;
  bool known;
  vector<int> rules;
  twalker giver;
  int sid;
  int parent_dir;
  int astate;
  twalker where_seen;
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

EX set<tcell*> single_live_branch_close_to_root;

/** is what on the left side, or the right side, of to_what? */

void treewalk(twalker& cw, int delta) {
  auto cwd = get_parent_dir(cw);
  if(cw == cwd) cw = addstep(cw);
  else {
    auto cw1 = addstep(cw);
    auto cwd = get_parent_dir(cw1);
    if(cwd == cw1) cw = cw1;
    }
  cw+=delta;
  }

EX vector<tcell*> sidecaches_to_clear;

void clear_sidecache() {
  if(sidecaches_to_clear.size()) {
    for(auto c: sidecaches_to_clear)
      c->which_side = c->known_sides = 0;
    sidecaches_to_clear.clear();
    }
  }

void set_sidecache(twalker what, int side) {
  auto c = what.at;
  if(c->known_sides == 0) sidecaches_to_clear.push_back(c);
  unsigned long long bit = 1ll<<what.spin;
  c->known_sides |= bit;
  if(side > 0)
    c->which_side |= bit;
  }

int get_sidecache(twalker what) {
  auto c = what.at;
  unsigned long long bit = 1ll<<what.spin;
  if(c->known_sides & bit)
    return (c->which_side & bit) ? 1 : -1;
  return 0;
  }

int get_side(twalker what) {
  if(WDIM == 3) throw hr_exception("called get_side");

  bool side = !(flags & w_no_sidecache);
  bool fast = (flags & w_slow_side);

  if(side) {
    auto w = get_sidecache(what);
    if(w) return w;
    }

  int res = 99;
  int steps = 0;

  if(fast) {
    twalker w = what;
    twalker tw = what + wstep;
    auto adv = [] (twalker& cw) {
      cw = get_parent_dir(cw);
      if(cw.peek()->dist >= cw.at->dist) {
        handle_distance_errors();
        if(debugflags & DF_GEOM)
          println(hlog, "get_parent_dir error at ", cw, " and ", cw.at->move(cw.spin), ": ", cw.at->dist, "::", cw.at->move(cw.spin)->dist);
        throw rulegen_failure("get_parent_dir error");
        }
      cw += wstep;
      };
    while(w.at != tw.at) {
      steps++; if(steps > max_getside) {
        debuglist = {what, w, tw};
        throw rulegen_failure("qsidefreeze");
        }
      ufind(w); ufind(tw);
      if(w.at->dist > tw.at->dist)
        adv(w);
      else if(w.at->dist < tw.at->dist)
        adv(tw);
      else {
        adv(w); adv(tw);
        }
      }

    if(w.at->dist && !single_live_branch_close_to_root.count(w.at)) {
      twalker wd = get_parent_dir(w);
      ufind(tw);
      res = wd.to_spin(w.spin) - wd.to_spin(tw.spin);
      }
    }

  // failed to solve this in the simple way (ended at the root) -- go around the tree
  twalker wl = what;
  twalker wr = wl;
  auto to_what = what + wstep;
  auto ws = what; treewalk(ws, 0); if(ws == to_what) res = 0;

  static vector<twalker> lstack = {nullptr}, rstack = {nullptr};
  lstack.resize(1); rstack.resize(1);
  while(res == 99) {
    handle_distance_errors();
    steps++; if(steps > current_getside) {
      debuglist = {what, to_what, wl, wr};
      checks_to_skip.clear();
      if(parent_updates) throw rulegen_retry("xsidefreeze");
      else if(steps > max_getside) {
        throw rulegen_failure("xsidefreeze");
        }
      else {
        current_getside *= 2;
        throw rulegen_retry("xsidefreeze double");
        }
      }
    bool gl = wl.at->dist <= wr.at->dist;
    bool gr = wl.at->dist >= wr.at->dist;
    if(gl) {
      if(side && get_sidecache(wl) == 1) wl += wstep;
      treewalk(wl, -1);
      if(wl == to_what) { res = 1; }
      if(!side) ;
      else if(lstack.back() == wl+wstep) {
        set_sidecache(lstack.back(), 1);
        set_sidecache(wl, -1);
        lstack.pop_back();
        }
      else if(wl.at->parent_dir != wl.spin && (wl+wstep).at->parent_dir != (wl+wstep).spin) lstack.push_back(wl);
      }
    if(gr) {
      if(side && get_sidecache(wr) == -1) wr += wstep;
      treewalk(wr, +1);
      if(wr == to_what) {res = -1; }
      if(!side) ;
      else if(rstack.back() == wr+wstep) {
        set_sidecache(rstack.back(), -1);
        set_sidecache(wr, +1);
        rstack.pop_back();
        }
      else if(wr.at->parent_dir != wr.spin && (wr+wstep).at->parent_dir != (wr+wstep).spin) rstack.push_back(wr);
      }
    }

  if(side && res)
    set_sidecache(what, res), set_sidecache(what + wstep, -res);
  return res;
  }

EX int move_code(twalker cs) {
   bool child = false;
   if(cs.at->dist) {
     auto csd = get_parent_dir(cs);
     child = cs == csd;
     }
   if(child)
     return C_CHILD;
   else {
     auto cs2 = cs + wstep;
     be_solid(cs.at); ufind(cs); ufind(cs2); be_solid(cs2.at);
     fix_distances(cs.at);

     if(WDIM == 3) {
       if(cs2.at->parent_dir == cs2.spin) return C_PARENT;
       else return get_roadsign(cs+wstep);
       }

     int y = cs.at->dist - cs.peek()->dist;
     int x;

     if(!(flags & w_no_relative_distance)) x = C_EQUAL;
     else if(y == 1) x = C_NEPHEW;
     else if(y == 0) x = C_EQUAL;
     else if(y == -1) x = C_UNCLE;
     else throw rulegen_failure("distance problem y=" + its(y) + lalign(0, " cs=", cs, " cs2=", cs2, " peek=", cs.peek(), " dist=", cs.at->dist, " dist2=", cs2.at->dist));
     auto gs = get_side(cs);
     if(gs == 0 && x == C_UNCLE) x = C_PARENT;
     if(gs > 0) x++;
     return x;
     }
  }

EX void id_at_spin(twalker cw, vector<twalker>& sprawl, vector<analyzer_state*>& states) {
  ufind(cw);
  auto aid = get_aid(cw);
  auto a_ptr = &(analyzers[aid.first][aid.second]);
  sprawl = { cw };
  states = { nullptr };

  indenter ind(2);
  while(true) {    
    auto& a = *a_ptr;
    if(!a) {
      a = alloc_analyzer();
      }
    states.push_back(a);
    if(isize(sprawl) <= cw.at->type) {
      a->id = 0, a->dir = isize(sprawl)-1;
      // println(hlog, "need to go in direction ", a->dir);
      }
    if(a->id == MYSTERY) {
      return;
      }
    if(a->id >= isize(sprawl)) {
      println(hlog, sprawl);
      println(hlog, "id = ", a->id);
      throw hr_exception("sprawl error");
      }
    auto t = sprawl[a->id];
    twalker tw = t + a->dir;
    ufind(tw);
    tw.cpeek();
    ufind(tw);
    int mc = move_code(tw + wstep);
    sprawl.push_back(tw + wstep);
    a_ptr = &(a->substates[mc]);
    }
  }

EX pair<int, int> get_code(twalker& cw) {
  tcell *c = cw.at;
  if(c->code != MYSTERY_LARGE && c->parent_dir != MYSTERY) {
    int bestd = c->parent_dir;
    if(bestd == -1) bestd = 0;
    return {bestd, c->code};
    }

  be_solid(c);

  twalker cd = c->dist == 0 ? twalker(c, 0) : get_parent_dir(cw);
  if(cd.at != c) ufind(cw);
  
  indenter ind(2);

  static vector<twalker> sprawl;
  static vector<analyzer_state*> states;
  id_at_spin(cd, sprawl, states);
  auto v = states.back();
  
  v->inhabitants.push_back(cw);
  
  cd.at->code = v->analyzer_id;
  return {cd.spin, v->analyzer_id};
  }

EX pair<int, int> get_treestate_id(twalker& cw) {
  auto co = get_code(cw);
  auto v = all_analyzers[co.second];
  if(v->dir == MYSTERY_LARGE) {
    int id = isize(treestates);
    v->dir = id;
    treestates.emplace_back();
    auto& nts = treestates.back();
    nts.id = id;
    nts.where_seen = cw;
    nts.known = false;
    nts.is_live = true;
    nts.astate = co.second;
    }
  co.second = v->dir;
  return co;
  }

/* == rule generation == */

EX int rule_root;

vector<int> gen_rule(twalker cwmain);

EX int try_count;
EX vector<twalker> important;

vector<twalker> cq;

#if HDR
/* special codes */
static const int DIR_UNKNOWN = -1;
static const int DIR_LEFT = -4;
static const int DIR_RIGHT = -5;
static const int DIR_PARENT = -6;
#endif

vector<int> gen_rule(twalker cwmain, int id) {
  vector<int> cids;
  for(int a=0; a<cwmain.at->type; a++) {
    auto front = cwmain+a;
    twalker c1 = front + wstep;
    be_solid(c1.at);
    if(a == 0 && cwmain.at->dist) { cids.push_back(DIR_PARENT); continue; }
    if(c1.at->dist <= cwmain.at->dist) { cids.push_back(DIR_UNKNOWN); continue; }
    auto co = get_treestate_id(c1);
    auto& d1 = co.first;
    auto& id1 = co.second;
    if(c1.at->cmove(d1) != cwmain.at || c1.at->c.spin(d1) != front.spin) {
      cids.push_back(DIR_UNKNOWN); continue;
      }
    cids.push_back(id1);
    }

  if(WDIM != 3) for(int i=0; i<isize(cids); i++) if(cids[i] == DIR_UNKNOWN)
    cids[i] = get_side(cwmain+i) < 0 ? DIR_RIGHT : DIR_LEFT;

  if(WDIM == 3) for(int i=0; i<isize(cids); i++) if(cids[i] == DIR_UNKNOWN)
    cids[i] = get_roadsign(cwmain+i);

  return cids;
  }

vector<reaction_t> queued_extensions;

void handle_queued_extensions() {
  if(queued_extensions.empty()) return;
  for(auto& r: queued_extensions) r();
  throw rulegen_retry("mismatch error");
  }

EX void rules_iteration_for(twalker& cw) {
  indenter ri(2);
  ufind(cw);
  auto co = get_treestate_id(cw);
  auto& d = co.first;
  auto& id = co.second;
  twalker cwmain(cw.at, d);
  ufind(cwmain);

  vector<int> cids = gen_rule(cwmain, id);
  auto& ts = treestates[id];

  if(!ts.known) {
    ts.known = true;
    ts.rules = cids;
    ts.giver = cwmain;
    ts.sid = cwmain.at->id;
    ts.parent_dir = cwmain.spin;
    ts.is_root = cw.at->dist == 0;
    }
  else if(ts.rules != cids) {
    handle_distance_errors();
    auto& r = ts.rules;
    if(debugflags & DF_GEOM) {
      println(hlog, "merging ", ts.rules, " vs ", cids);
      }
    int mismatches = 0;
    for(int z=0; z<isize(cids); z++) {
      if(r[z] == cids[z]) continue;
      if(r[z] < 0 || cids[z] < 0) {
        debuglist = { cwmain, ts.giver };
        cwmain.at->code = MYSTERY_LARGE;
        ts.giver.at->code = MYSTERY_LARGE;
        throw rulegen_retry("neg rule mismatch");
        }

      auto tg = ts.giver;
      
      if(!(flags & w_no_queued_extensions)) {
        queued_extensions.push_back([cwmain, z, tg] {
          extend_analyzer(cwmain, z, tg);
          });
        return;
        }

      extend_analyzer(cwmain, z, tg);
      mismatches++;

      debuglist = { cwmain, ts.giver };

      if(!(flags & w_conflict_all))
        throw rulegen_retry("mismatch error");
      }

    debuglist = { cwmain, ts.giver };
    
    if(mismatches)
      throw rulegen_retry("mismatch error");
    
    throw rulegen_failure("no mismatches?!");
    }
  }

void minimize_rules() {
  states_premini = isize(treestates);
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

using tsinfo = pair<int, int>;

tsinfo get_tsinfo(twalker& tw) {
  auto co = get_treestate_id(tw);
  int spin;
  if(co.first == -1) spin = tw.spin;
  else spin = gmod(tw.spin - co.first, tw.at->type);
  return {co.second, spin};
  }

int get_rule(const twalker tw, tsinfo s) {

  auto& r = treestates[s.first].rules;
  if(r.empty()) {
    important.push_back(tw.at);
    throw rulegen_retry("unknown rule in get_rule");
    }

  return r[s.second];
  }

set<vector<tsinfo> > verified_branches;

void push_deadstack(vector<tsinfo>& hash, twalker w, tsinfo tsi, int dir) {

  hash.push_back(tsi);

  while(true) {
    ufind(w);
    if(isize(hash) > 10000) throw rulegen_failure("deadstack overflow");
    tsi.second += dir; w += dir;
    auto& ts = treestates[tsi.first];
    if(ts.is_root) return;
    if(tsi.second == 0 || tsi.second == isize(ts.rules)) {
      w += wstep;
      tsi = get_tsinfo(w);
      hash.push_back(tsi);
      }
    else {
      if(ts.rules.empty()) throw rulegen_retry("empty rule");
      int r = ts.rules[tsi.second];
      if(r > 0 && treestates[r].is_live) return;
      }
    }
  }

struct verify_advance_failed : hr_exception {};

using conflict_id_type = pair<pair<int, int>, pair<int, int>>;

set<conflict_id_type> branch_conflicts_seen;

void verified_treewalk(twalker& tw, int id, int dir) {
  if(id >= 0) {
    auto tw1 = tw + wstep;
    auto co = get_treestate_id(tw1);
    if(co.second != id || co.first != tw1.spin) {
      handle_distance_errors();

      conflict_id_type conflict_id = make_pair(make_pair((tw+wstep).spin,id), co);

      if((flags & w_examine_all) || !branch_conflicts_seen.count(conflict_id)) {
        branch_conflicts_seen.insert(conflict_id);
        important.push_back(tw.at);
        if(debugflags & DF_GEOM)
          println(hlog, "branch conflict ", conflict_id, " found");
        }
      else if(debugflags & DF_GEOM)
        println(hlog, "branch conflict ", conflict_id, " found again");
      debuglist = {tw, tw+wstep};
      throw verify_advance_failed();
      }
    }
  treewalk(tw, dir);
  }

bool examine_branch(int id, int left, int right) {
  if(WDIM == 3) return true;
  auto rg = treestates[id].giver;

  if(debugflags & DF_GEOM)
    println(hlog, "need to examine branches ", tie(left, right), " of ", id, " starting from ", rg, " step = ", rg+left+wstep, " vs ", rg+right+wstep);

  indenter ind(2);

  auto wl = rg+left;
  auto wr = rg+left+1;

  vector<twalker> lstack, rstack;

  int steps = 0;
  try {
  while(true) {
    handle_distance_errors();
    steps++;
    if(steps > current_examine_branch) {
      debuglist = { rg+left, wl, wr };
      if(skipped_branches.size()) {
        checks_to_skip.clear();
        throw rulegen_retry("max_examine_branch exceeded after a skipped check");
        }
      else if(branch_conflicts_seen.size())
        /* may be not a real problem, but caused by incorrect detection of live branches */
        throw rulegen_retry("max_examine_branch exceeded after a conflict");
      else if(steps > max_examine_branch)
        throw rulegen_failure("max_examine_branch exceeded");
      else {
        current_examine_branch *= 2;
        throw rulegen_retry("max_examine_branch exceeded, doubling");
        }
      }
    
    auto tsl = get_tsinfo(wl);
    auto tsr = get_tsinfo(wr);

    auto rl = get_rule(wl, tsl);
    auto rr = get_rule(wr, tsr);

    if(rdebug_flags & 32)
      println(hlog, "wl = ", wl, " -> ", wl+wstep, " R", rl, " wr = ", wr, " -> ", wr+wstep, " R", rr, " lstack = ", lstack, " rstack = ", rstack);

    if(rl == DIR_RIGHT && rr == DIR_LEFT && lstack.empty() && rstack.empty()) {
      vector<tsinfo> hash;
      push_deadstack(hash, wl, tsl, -1);
      hash.emplace_back(-1, wl.at->dist - wr.at->dist);
      push_deadstack(hash, wr, tsr, +1);
      if(rdebug_flags & 32)
        println(hlog, "got hash: ", hash);
      if(verified_branches.count(hash)) {
        return true;
        }
      verified_branches.insert(hash);

      verified_treewalk(wl, rl, -1);
      verified_treewalk(wr, rr, +1);
      }

    else if(rl == DIR_RIGHT && !lstack.empty() && lstack.back() == wl+wstep) {
      lstack.pop_back();
      verified_treewalk(wl, rl, -1);
      }

    else if(rr == DIR_LEFT && !rstack.empty() && rstack.back() == wr+wstep) {
      rstack.pop_back();
      verified_treewalk(wr, rr, +1);
      }

    else if(rl == DIR_LEFT) {
      lstack.push_back(wl);
      verified_treewalk(wl, rl, -1);
      }

    else if(rr == DIR_RIGHT) {
      rstack.push_back(wr);
      verified_treewalk(wr, rr, +1);
      }

    else if(rl != DIR_RIGHT)
      verified_treewalk(wl, rl, -1);

    else if(rr != DIR_RIGHT)
      verified_treewalk(wr, rr, +1);

    else throw rulegen_failure("cannot advance while examining");
    }
    }
  catch(verify_advance_failed&) {
    if(flags & w_examine_once) throw rulegen_retry("advance failed");
    return false;
    }
  }

/* == main algorithm == */

bool need_clear_codes;

EX void clear_codes() {
  need_clear_codes = false;
  for(auto a: all_analyzers) {
    for(auto tw: a->inhabitants) tw.at->code = MYSTERY_LARGE;
    a->inhabitants.clear();
    }
  }

void find_single_live_branch(twalker& at) {
  handle_distance_errors();
  rules_iteration_for(at);
  handle_queued_extensions();
  int id = get_treestate_id(at).second;
  int t = at.at->type;
  auto r = treestates[id].rules; /* no & because may move */
  int q = 0;
  if(r.empty()) { important.push_back(at.at); throw rulegen_retry("no giver in find_single_live_branch"); }
  for(int i=0; i<t; i++) if(r[i] >= 0) {
    if(treestates[r[i]].is_live) q++;
    }
  for(int i=0; i<t; i++) if(r[i] >= 0) {
    single_live_branch_close_to_root.insert(at.at);
    if(!treestates[r[i]].is_live || q == 1) {
      auto at1 = at + i + wstep;
      find_single_live_branch(at1);
      }
    }
  }

EX void clean_analyzers() {
  for(auto a: all_analyzers) for(auto tw: a->inhabitants) tw.at->code = MYSTERY_LARGE;
  for(auto a: all_analyzers) delete a;
  for(auto& av: analyzers) for(auto& a: av) a = nullptr;
  all_analyzers.clear();
  next_analyzer_id = 0;
  }

EX void clean_data() {
  clean_analyzers();
  checks_to_skip.clear();
  important = t_origin;
  }

EX void clear_sidecache_and_codes() {
  clear_sidecache();
  need_clear_codes = true;
  }

EX void update_all_codes(analyzer_state *a) {
  vector<twalker> old;
  swap(old, a->inhabitants);
  for(auto tw: old) {
    ufind(tw);
    if(tw.at->code == a->analyzer_id)
      tw.at->code = MYSTERY_LARGE;
    }
  }

EX void clean_parents() {
  clear_sidecache_and_codes();
  clean_data();
  auto c = first_tcell;
  while(c) { c->parent_dir = MYSTERY; c = c->next; }
  }

int qshortcuts() {
  int res = 0;
  for(auto& sh: shortcuts) res += isize(sh);
  return res;
  }

void clear_treestates() {
  treestates.clear();
  for(auto a: all_analyzers)
    if(a->id == MYSTERY) a->dir = MYSTERY_LARGE;
  }

EX void rules_iteration() {
  try_count++;
  debuglist = {};

  queued_extensions.clear();

  if((try_count & (try_count-1)) == 0) if(!(flags & w_no_restart)) {
    clean_data();
    clean_parents();
    }

  if(rdebug_flags & 1) println(hlog, "attempt: ", try_count, " important = ", isize(important), " cells = ", tcellcount, " shortcuts = ", qshortcuts());

  parent_updates = 0;
  clear_treestates();
  if(need_clear_codes) clear_codes();
  
  cq = important;
  
  if(rdebug_flags & 2)
    println(hlog, "important = ", cq);

  for(int i=0; i<isize(cq); i++) {
    rules_iteration_for(cq[i]);
    }
  
  handle_distance_errors();
  if(debugflags & DF_GEOM)
    println(hlog, "number of treestates = ", isize(treestates));
  rule_root = get_treestate_id(t_origin[0]).second;
  if(debugflags & DF_GEOM)
    println(hlog, "rule_root = ", rule_root);

  for(int id=0; id<isize(treestates); id++) {
    if(!treestates[id].known) {
      auto ws = treestates[id].where_seen;
      rules_iteration_for(ws);
      continue;
      }
    }

  handle_queued_extensions();

  int N = isize(important);

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
    
    if(rdebug_flags & 4)
      println(hlog, "deadend states found: ", new_deadends);
    }
  
  handle_distance_errors();
  verified_branches.clear();

  int q = isize(single_live_branch_close_to_root);
  
  single_live_branches = 0;
  double_live_branches = 0;

  branch_conflicts_seen.clear();

  // handle dead roots -- some of their branches MUST live
  if(WDIM == 2) for(int id=0; id<isize(treestates); id++) if(treestates[id].is_root && !treestates[id].is_live) {
    auto r = treestates[id].rules;
    for(int i=0; i<isize(r); i++) if(r[i] >= 0) {
      examine_branch(id, i, i);
      break;
      }
    }

  handle_queued_extensions();

  skipped_branches.clear();

  auto examine_or_skip_branch = [&] (int id, int fb, int sb) {
    if(flags & w_no_branch_skipping) {
      examine_branch(id, fb, sb);
      return;
      }
    auto b = branch_check{treestates[id].astate, fb, sb};
    if(checks_to_skip.count(b)) {
      skipped_branches.emplace_back([id, fb, sb] { examine_branch(id, fb, sb); });
      return;
      }
    if(examine_branch(id, fb, sb)) checks_to_skip.insert(b);
    };

  if(WDIM == 2) for(int id=0; id<isize(treestates); id++) if(treestates[id].is_live) {
    auto r = treestates[id].rules; /* no & because treestates might have moved */
    if(r.empty()) continue;
    int last_live_branch = -1;
    int first_live_branch = -1;
    int qbranches = 0;
    for(int i=0; i<isize(r); i++)
      if(r[i] >= 0 && treestates[r[i]].is_live) {
        if(first_live_branch == -1) first_live_branch = i;
        if(last_live_branch >= 0)
          examine_or_skip_branch(id, last_live_branch, i);
        last_live_branch = i;
        qbranches++;
        }
    if(qbranches == 2) double_live_branches++;
    if((flags & w_slow_side) && first_live_branch == last_live_branch && treestates[id].is_root) {
      if(debugflags & DF_GEOM)
        println(hlog, "for id ", id, " we have a single live branch");
      single_live_branches++;
      indenter ind(2);
      debuglist = { treestates[id].giver };
      find_single_live_branch(treestates[id].giver);
      }
    if(isize(single_live_branch_close_to_root) != q) {
      vector<tcell*> v;
      for(auto c: single_live_branch_close_to_root) v.push_back(c);
      if(debugflags & DF_GEOM) 
        println(hlog, "changed single_live_branch_close_to_root from ", q, " to ", v);
      debuglist = { treestates[id].giver };
      clear_sidecache_and_codes();
      throw rulegen_retry("single live branch");
      }
    if(treestates[id].is_root)
      examine_or_skip_branch(id, last_live_branch, first_live_branch);
    }

  after_branches:
  for(int id=0; id<isize(treestates); id++) if(!treestates[id].giver.at) {
    important.push_back(treestates[id].where_seen);
    }
  
  handle_distance_errors();
  handle_queued_extensions();
  if(isize(important) != N)
    throw rulegen_retry("need more rules after examine");

  if(WDIM == 3) {
    check_road_shortcuts();
    optimize();
    N = isize(important);
    check_validity_3d();
    }

  if(skipped_branches.size()) {
    checks_to_skip.clear();
    for(auto sb: skipped_branches) sb();
    skipped_branches.clear();
    goto after_branches;
    }

  if(WDIM == 2) minimize_rules();
  find_possible_parents();
  
  if(isize(important) != N)
    throw rulegen_retry("need more rules after minimize");
  handle_distance_errors();
  }

void clear_tcell_data() {
  auto c = first_tcell;
  while(c) {
    c->is_solid = false;
    // c->dist = MYSTERY;
    c->parent_dir = MYSTERY;
    c->code = MYSTERY_LARGE;
    c->distance_fixed = false;
    c = c->next;
    }
  in_fixing = false; fix_queue = std::queue<reaction_t>{};
  }

EX void cleanup() {
  clear_tcell_data();
  clean_analyzers();
  important.clear();
  shortcuts.clear();
  single_live_branch_close_to_root.clear();
  cleanup3();
  }

EX void clear_all() {  
  treestates.clear();
  cleanup();
  }

EX int origin_id;

EX unsigned start_time;

EX void check_timeout() {
  if(SDL_GetTicks() > start_time + 1000 * rulegen_timeout)
    throw rulegen_surrender("timeout");
  }

EX void generate_rules() {

  start_time = SDL_GetTicks();
  delete_tmap();

  if(WDIM == 3 && reg3::in_hrmap_rule_or_subrule()) {
    stop_game();
    reg3::consider_rules = 0;
    flags |= w_numerical;
    start_game();
    }
  else if(WDIM == 3) {
    flags |= w_numerical;
    }
  else if(!arb::in()) try {
    arb::convert::convert();
    if(flags & w_numerical) arb::convert::activate();
    }
  catch(hr_exception& e) {
    throw rulegen_surrender("conversion failure");
    }
  
  clear_all();

  analyzers.clear();
  important.clear();
  treestates.clear();
  hard_parents = single_live_branches = double_live_branches = all_solid_errors = solid_errors = 0;

  next_distance_warning = first_restart_on;
  current_getside = first_restart_on;
  current_examine_branch = first_restart_on;

  int NS = number_of_types();
  shortcuts.resize(NS);
  analyzers.resize(NS);
  for(int i=0; i<NS; i++) analyzers[i].resize(cycle_size(i));

  t_origin.clear();
  cell_to_tcell.clear();
  tcell_to_cell.clear();
  branch_conflicts_seen.clear();
  sidecaches_to_clear.clear();
  clear_sidecache_and_codes();
  fix_queue = queue<reaction_t>();; in_fixing = false;

  if(flags & (w_numerical | w_known_structure)) {
    if(flags & w_known_structure) swap_treestates();
    stop_game();
    start_game();
    cell *s = currentmap->gamestart();
    tcell *c = gen_tcell(get_id(s));
    cell_to_tcell[s] = c;
    tcell_to_cell[c] = s;
    c->dist = 0;
    t_origin.push_back(twalker(c, 0));

    if(!(flags & w_single_origin))
      add_other_origins(NS);

    if(flags & w_known_structure) swap_treestates();
    }
  else if(flags & w_single_origin) {
    tcell *c = gen_tcell(origin_id);
    c->dist = 0;
    t_origin.push_back(twalker(c, 0));
    }
  else for(auto& ts: arb::current.shapes) {
    tcell *c = gen_tcell(ts.id);
    c->dist = 0;
    t_origin.push_back(twalker(c, 0));
    }

  if(GDIM == 3) build_cycle_data();

  bfs_queue = queue<tcell*>();
  if(flags & w_bfs) for(auto c: t_origin) bfs_queue.push(c.at);
  
  try_count = 0;
  
  important = t_origin;
  
  rule_iterations();
  }

EX void rule_iterations() {
  while(true) {
    check_timeout();
    try {
      rules_iteration();
      break;
      }
    catch(rulegen_retry& e) { 
      if(rdebug_flags & 8)
        println(hlog, "result ", try_count, ": ", e.what());
      if(try_count >= max_retries) throw;
      }
    }
  }

int reclevel;

void build_test();

/* == hrmap_rulegen == */

struct hrmap_rulegen : hrmap {
  hrmap *base;
  heptagon *origin;
  vector<heptagon*> extra_origins;

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

  cell* gen_extra_origin(int fv) override {
    heptagon *extra_origin = gen(fv, 0, true);
    extra_origin->s = hsOrigin;
    extra_origins.push_back(extra_origin);
    return extra_origin->c7;
    }
  
  ~hrmap_rulegen() { 
    clearfrom(origin);
    for(auto eo: extra_origins) clearfrom(eo);
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
    else if(r == DIR_LEFT || r == DIR_RIGHT) {
      heptspin hs1 = hs;
      int delta = r == DIR_LEFT ? -1 : 1;
      int rev = (DIR_LEFT ^ DIR_RIGHT ^ r);
      hs1 += delta;
      while(true) {
        int r1 = get_rule(hs1);
        if(r1 == rev) {
          hsconnect(hs, hs1);
          return hs1.at;
          }
        else if(r1 == r || r1 == DIR_PARENT || r1 >= 0) {
          hs1 += wstep;
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
      return arb::get_adj(arb::current_or_slided(), h->zebraval, dir);

    int s = h->fieldval;
    int dir0 = get_arb_dir(s, dir);
    
    int dir1 = -1;
    int sid1 = -1;
    
    if(h->c.move(dir)) {
      auto s1 = h->c.move(dir)->fieldval;
      dir1 = get_arb_dir(s1, h->c.spin(dir));
      sid1 = treestates[s1].sid;
      }

    return arb::get_adj(arb::current_or_slided(), treestates[s].sid, dir0, sid1, dir1, false);
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
    
    int cl = cycle_size(psid);

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

EX vector<treestate> alt_treestates;

EX void swap_treestates() {
  swap(treestates, alt_treestates);
  }

EX void add_other_origins(int qty) {
  for(int i=1; i<qty; i++) {
    cell *s = currentmap->gen_extra_origin(i);
    tcell *c = gen_tcell(get_id(s));
    cell_to_tcell[s] = c;
    tcell_to_cell[c] = s;
    c->dist = 0;
    t_origin.push_back(twalker(c, 0));
    }

  println(hlog, "t_origin size = ", isize(t_origin));
  }

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

EX string rules_known_for = "unknown";
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
      param_i(max_getside, "max_getside");
      param_i(max_bdata, "max_bdata");
      param_i(max_shortcut_length, "max_shortcut_length");
      param_i(rulegen_timeout, "rulegen_timeout");
      param_i(first_restart_on, "first_restart_on");
      param_i(max_ignore_level_pre, "max_ignore_level_pre");
      param_i(max_ignore_level_post, "max_ignore_level_post");
      param_i(max_ignore_time_pre, "max_ignore_time_pre");
      param_i(max_ignore_time_post, "max_ignore_time_post");
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
    else { int i = ep.iparse(); ts.rules.push_back(i); }
    }
  for(int i=0; i<N; i++) {
    if(ts.rules[i] == DIR_PARENT) qparent++, sumparent += i;
    }
  ts.is_root = qparent == 0;
  if(qparent > 1) throw hr_parse_exception("multiple parent at " + ep.where());
  if(qparent == 1) {
    ts.parent_dir = sumparent;
    if(debugflags & DF_GEOM) println(hlog, "before: ", ts.rules);
    std::rotate(ts.rules.begin(), ts.rules.begin() + sumparent, ts.rules.end());
    if(debugflags & DF_GEOM) println(hlog, "after : ", ts.rules);
    }
  ep.force_eat(")");
  }

EX void verify_parsed_treestates(arb::arbi_tiling& c) {
  if(rule_root < 0 || rule_root >= isize(treestates))
    throw hr_parse_exception("undefined treestate as root");
  for(auto& ts: treestates) for(auto& r: ts.rules) {
    if(r < 0 && !among(r, DIR_PARENT, DIR_LEFT, DIR_RIGHT))
      throw hr_parse_exception("negative number in treestates");
    if(r > isize(treestates))
      throw hr_parse_exception("undefined treestate");
    }
  for(auto& sh: c.shapes) sh.cycle_length = sh.size();
  find_possible_parents();
  }

EX void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
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

  dialog::addBoolItem(XLAT("extended football colorability"), arb::extended_football, 'f');
  dialog::add_action([] {
    arb::extended_football = !arb::extended_football;
    rules_known_for = "unknown";
    rule_status = "manually disabled";
    if(arb::convert::in()) {
      stop_game();
      arb::convert::convert();
      arb::convert::activate();
      start_game();
      }
    else if(arb::in()) {
      stop_game();
      try {
        arb::load(arb::tes);
        }
      catch(hr_parse_exception& ex) {
        println(hlog, "failed: ", ex.s);
        }
      start_game();
      }
    });
  add_edit(arb::convert::minimize_on_convert);
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

#if CAP_COMMANDLINE
int readRuleArgs() {
  using namespace arg;

  if(0) ;

  else if(argis("-ruleflag")) {
    shift();
    rulegen::flags ^= Flag(argi());
    }

  else if(argis("-ruledflags")) {
    shift();
    rulegen::rdebug_flags = argi();
    }

  else return 1;
  return 0;
  }

auto hook = addHook(hooks_args, 100, readRuleArgs);
#endif

EX }
}
