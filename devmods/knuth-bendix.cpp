#include "../hyper.h"
#include <fstream>
#include <semaphore.h>

namespace hr {

namespace rulegen {
bool set_general(const string& s);
}

struct symbol {
  unsigned char sid;
  unsigned char eid;
  bool operator < (const symbol s1) const { return (int16_t&) (*this) < (int16_t&) s1; } 
  bool operator == (const symbol s1) const { return (int16_t&) (*this) == (int16_t&) s1; } 
  bool operator != (const symbol s1) const { return (int16_t&) (*this) != (int16_t&) s1; } 
  };

using seq = vector<symbol>;

void block(symbol& s) {
  s.sid ^= 128;
  }

void unblock(symbol& s) {
  s.sid ^= 128;
  }

bool is_blocked(symbol& s) {
  return s.sid & 128;
  }

void print(hstream& hs, symbol s) {
  print(hs, "[", int(s.sid), ":", int(s.eid), "]");
  }

bool reversed;

bool lexless(const seq& a, const seq& b) {
  int al = isize(a);
  int bl = isize(b);
  if(al != bl) return al < bl;
  if(reversed) {
    for(int i=al-1; i>=0; i--) if(a[i].eid < 120 && a[i] != b[i]) return a[i] < b[i];
    return a.back() < b.back();
    }
  else {
    return a < b;
    }
  }

struct lexless_comparator {
    bool operator()(const seq& a, const seq& b) const { return lexless(a, b); }
};

map<seq, seq, lexless_comparator> rules;

seq sub(const seq& s, int a, int l) {
  seq result;
  for(int i=0; i<l; i++) result.push_back(s[a+i]);
  return result;
  }

seq cat(const seq& a, const seq& b) {
  seq result;
  for(auto e: a) result.push_back(e);
  for(auto e: b) result.push_back(e);
  return result;
  }

void catto(seq& a, const seq& b) {
  a.reserve(a.size() + b.size());
  for(auto e: b) a.push_back(e);
  }

void catto_sub(seq& a, const seq& b, int pos, int len) {
  a.reserve(a.size() + len);
  for(int i=0; i<len; i++) a.push_back(b[pos+i]);
  }

int max_rule_length;

set<pair<seq, seq>> rules_checked;

queue<pair<seq, seq>> rulequeue;

void enqueue_rule(seq l, seq r) {
  rulequeue.emplace(l, r);
  }

cellwalker trace(cellwalker cw, const seq& sq, bool debug) {
  for(auto sym: sq) {
    if(shvid(cw.at) != sym.sid) println(hlog, "error: wrong type");
    if(debug) println(hlog, "at: ", cw);
    cw += sym.eid;
    cw += wstep;
    cw -= cw.spin % arb::current.shapes[shvid(cw.at)].cycle_length;
    }
  return cw;
  }

cell *get_sample(int sid) {
  celllister cl(currentmap->gamestart(), 20, 100000, nullptr);
  for(cell *c: cl.lst) if(shvid(c) == sid) return c;
  println(hlog, "sample not found of ", sid);
  exit(1);
  }

string verify_rule(const pair<seq, seq>& rule, bool debug = false) {
  cell *cstart = get_sample(rule.first[0].sid);
  cellwalker cw(cstart);
  cellwalker cw1 = trace(cw, rule.first, debug);
  cellwalker cw2 = trace(cw, rule.second, debug);
  return cw1 == cw2 ? "OK" : lalign(0, "FAIL ", cw1, " VS ", cw2);
  }

int add_rule(seq l, seq r) {
  indenter ind(2);
  if(l == r) return 0;
  if(lexless(l, r)) swap(l, r);
  
  again:
  
  if(!rules.count(l)) {
    // println(hlog, "rule added: ", l, " -> ", r, " verify: ", verify_rule({l, r}));
    rules[l] = r;
    return 1;
    }
  else if(rules[l] == r) return 0;
  else if(lexless(r, rules[l])) {
    l = rules[l];
    goto again;
    }
  else {
    // println(hlog, "rule simplified: ", l, " -> ", r);
    auto x = rules[l];
    x = r;
    l = x;
    goto again;
    }
  }

symbol gen_symbol(int sid, int eid) {
  symbol sym;
  sym.sid = sid;
  sym.eid = eid;
  return sym;
  }

bool include_rotations = false;

void prepare_rules() {
  auto& ac = arb::current;
  int N = isize(ac.shapes);
  
  /* move and back and move */
  if(!include_rotations) for(int i=0; i<N; i++) {
    auto& sh = ac.shapes[i];
    int K = sh.size();
    for(int k=0; k<K; k++)
    for(int l=0; l<K; l++) {
      seq sq, sq1;
      sq.push_back(gen_symbol(i, k));
      auto con = sh.connections[k % sh.cycle_length];
      auto& sh1 = ac.shapes[con.sid];
      con.eid %= sh1.cycle_length;
      sq.push_back(gen_symbol(con.sid, con.eid));
      sq.push_back(gen_symbol(i, l));
      int k1 = k / sh.cycle_length * sh.cycle_length;
      // if(k1) 
      sq1.push_back(gen_symbol(i, (k1 + l) % K));
      enqueue_rule(sq, sq1);
      }
    }

  /* move and back -> nothing or rotate */
  if(include_rotations) for(int i=0; i<N; i++) {
    auto& sh = ac.shapes[i];
    int K = sh.size();
    for(int k=0; k<K; k++) {
      seq sq, sq1;
      sq.push_back(gen_symbol(i, k));
      auto con = sh.connections[k % sh.cycle_length];
      auto& sh1 = ac.shapes[con.sid];
      con.eid %= sh1.cycle_length;
      sq.push_back(gen_symbol(con.sid, con.eid));
      int k1 = k / sh.cycle_length * sh.cycle_length;
      if(k1) sq1.push_back(gen_symbol(i, 120 + k1));
      if(k1) sq1.push_back(gen_symbol(255, 255));
      if(lexless(sq, sq1)) swap(sq, sq1);
      enqueue_rule(sq, sq1);
      }
    }

  /* rotate + move -> move */
  if(include_rotations) for(int i=0; i<N; i++) {
    auto& sh = ac.shapes[i];
    int K = sh.size();
    int C = sh.cycle_length;
    for(int k=C; k<K; k+=C) 
    for(int l=0; l<K; l++) {
      seq sq, sq1;
      sq.push_back(gen_symbol(i, 120+k));
      sq.push_back(gen_symbol(255, 255));
      sq.push_back(gen_symbol(i, l));
      sq1.push_back(gen_symbol(i, (k+l) % K));
      if(lexless(sq, sq1)) swap(sq, sq1);
      enqueue_rule(sq, sq1);
      }
    }
  
  /* cycle a vertex */
  for(int i=0; i<N; i++) {
    auto& sh = ac.shapes[i];
    for(int j=0; j<sh.cycle_length; j++) {
      int val = sh.vertex_valence[j]; // (j+1) % sh.cycle_length];
      int ai = i, aj = j;
      seq sq;
      // cellwalker cw(get_sample(i), j);
      // auto cw0 = cw;
      for(int v=0; v<val; v++) {
        // println(hlog, "at: ", cw);
        /*
        cw++;
        cw += wstep;
        */
        auto& ash = ac.shapes[ai];
        aj += 1;
        aj %= ash.size();
        sq.push_back(gen_symbol(ai, aj));
        auto co = ash.connections[aj % ash.cycle_length];
        ai = co.sid;
        aj = co.eid;
        aj %= ac.shapes[ai].cycle_length;
        }
      // println(hlog, "finish at: ", cw, " from: ", cw0);
      aj %= sh.cycle_length;
      if(i != ai || j != aj /* || cw != cw0 */) { println(hlog, "bad cycling!"); return; }
      enqueue_rule(sq, {});
      }
    }
  
  for(auto r: rules) println(hlog, r);
  }

void final_rules() {
  auto& ac = arb::current;
  int N = isize(ac.shapes);
  for(int i=0; i<N; i++) {
    auto& sh = ac.shapes[i];
    int K = sh.size();
    int C = sh.cycle_length;
    for(int k=0; k<K; k++)
    for(int l=0; l<K; l+=C) {
      seq sq, sq1;
      sq.push_back(gen_symbol(i, k));
      auto con = sh.connections[k % sh.cycle_length];
      auto& sh1 = ac.shapes[con.sid];
      con.eid %= sh1.cycle_length;
      sq.push_back(gen_symbol(con.sid, con.eid));
      sq.push_back(gen_symbol(i, 120 + l));
      int k1 = k / sh.cycle_length * sh.cycle_length;
      // if(k1) 
      sq1.push_back(gen_symbol(i, 120 + (k1 + l) % K));
      enqueue_rule(sq, sq1);
      }
    }
  }

/* the substring of s starting at pos equals needle */

bool sub_at(const seq& s, int pos, const seq& needle) {
  for(int i=0; i<isize(needle); i++)
    if(s[i+pos] != needle[i])
      return false;
  return true;
  }

/* the suffix of s1 of length len, and the prefix of s2 of length len, agree */
bool suf_pref_agree(const seq& s1, const seq& s2, int len) {
  for(int i=0; i<len; i++)
    if(s1[isize(s1)-len+i] != s2[i])
      return false;
  return true;
  }

bool is_reducible(const seq& s, const pair<seq, seq>& r, seq& result) {  
  int rl = isize(r.first);
  int sl = isize(s);
  for(int i=0; i<=sl-rl; i++)
    if(sub_at(s, i, r.first)) {
      result = sub(s, 0, i);
      catto(result, r.second);
      catto_sub(result, s, i+rl, sl-rl-i);
      return true;
      }
  return false;
  }

void find_critical(const pair<seq,seq>& p, const pair<seq,seq>& q) {
  int pl = isize(p.first);
  int ql = isize(q.first);
  for(int i=1; i<pl && i < ql; i++)
    if(suf_pref_agree(p.first, q.first, i)) {
      seq nleft = sub(p.first, 0, pl-i);
      catto(nleft, q.second);
      seq nright = p.second;
      catto_sub(nright, q.first, i, ql-i);
      enqueue_rule(nleft, nright);
      }
 }

void handle_rule(const pair<seq, seq>& nr) {
  auto& lh = nr.first;
  auto& rh = nr.second;

  for(auto& r: rules) {
    seq res;
    if(is_reducible(lh, r, res)) {
      enqueue_rule(res, rh);
      return;
      }
    if(is_reducible(rh, r, res)) {
      enqueue_rule(lh, res);
      return;
      }
    }
  
  vector<seq> to_erase;

  for(auto& r: rules) {
    seq res;
    if(is_reducible(r.first, nr, res)) {
      to_erase.push_back(r.first);
      enqueue_rule(res, r.second);
      }
    if(is_reducible(rh, r, res)) {
      to_erase.push_back(r.first);
      enqueue_rule(r.first, res);
      }
    }
  
  for(auto s: to_erase) rules.erase(s);
  
  for(auto& r: rules) {
    find_critical(r, nr);
    find_critical(nr, r);
    }
  
  rules[nr.first] = nr.second;
  }

int kb_result;
double total_time;

int main_loop(int timelimit = 5, int lenlimit = 300) {
  
  clock_t start = clock();
  while(!rulequeue.empty()) {
    if(clock() > start + timelimit * CLOCKS_PER_SEC) return 1;
    auto p = rulequeue.front();
    rulequeue.pop();
    if(p.first == p.second) continue;
    if(lexless(p.first, p.second)) swap(p.first, p.second);
    if(rules_checked.count(p)) continue;
    rules_checked.insert(p);
    int len = p.first.size() + p.second.size();
    if(len > max_rule_length) max_rule_length = len;
    if(len > lenlimit) return 2;
    handle_rule(p);
    }
  
  println(hlog, "finished in ", (clock() - start) * 1. / CLOCKS_PER_SEC);
  
  return 0;  
  }

void test_knuth_bendix() {
  rules.clear();
  rulequeue = {};
  rules_checked.clear();
  max_rule_length = 0;
  start_game();
  
  prepare_rules();
  
  clock_t total_start = clock();
  println(hlog, "total_start = ", total_start * 1. / CLOCKS_PER_SEC);
  
  kb_result = main_loop(10);
  
  println(hlog, "after first phase = ", clock() * 1. / CLOCKS_PER_SEC);

  if(kb_result == 0) {

    println(hlog, "intermediate rules:");
    for(auto s: rules) println(hlog, s.first, " => ", s.second);

    final_rules();
    kb_result = main_loop(50, 999999);
    if(kb_result) kb_result += 2;
    }
  
  println(hlog, "after second phase = ", clock() * 1. / CLOCKS_PER_SEC);

  total_time = (clock() - total_start) * 1. / CLOCKS_PER_SEC;
  }

bool is_end_reducible(const seq& s1) {
  for(auto r: rules)
    if(isize(s1) >= isize(r.first) && sub(s1, isize(s1)-isize(r.first), isize(r.first)) == r.first)
      return true;
  return false;
  }

int count_tree_states() {
  set<seq> all_prefixes;
  for(auto r: rules) {
    auto left = r.first;
    for(int i=0; i<isize(left); i++) {
      seq prefix = sub(left, 0, i);
      seq s = prefix;
      s.emplace_back(gen_symbol(left[i].eid, 120));
      if(!is_end_reducible(s)) 
        all_prefixes.emplace(prefix);
      }
    }
  all_prefixes.erase(seq{});
  for(auto x: all_prefixes)
  println(hlog, x);
  return isize(all_prefixes) + isize(arb::current.shapes);  
  }

bool forked = false;
int max_children = 7;

void test_all(string setname) {
  floorshapes_level = 1;
  vector<string> filenames;
  std::ifstream is("devmods/rulegen-tests/"+setname+".lst");
  string s;
  while(getline(is, s)) {
    while(s != "" && s[0] == ' ')  s = s.substr(1);
    if(s != "" && s[0] != '#') filenames.push_back(s);
    }

  println(hlog, "CSV;kbres;Tp;kbrules;tree;maxlen;file");
  
  int children = 0;

  sem_t sem;
  if(forked) sem_init(&sem, true, 1);
  fflush(stdout);

  for(const string& s: filenames) {  

    if(forked) {
      int pid;
      if(children >= max_children) { 
        wait(&pid); children--; 
        }
      if((pid = fork())) children++;
      else goto doit;
      continue;
      }

    doit:

    if(rulegen::set_general(s)) {

      if(!arb::in()) try {
        arb::convert::convert();
        arb::convert::activate();
        }
      catch(hr_exception& e) {
        println(hlog, "CSV; failed to convert ", s);
        return;
        }
      // println(hlog, "will call test_knuth_bendix");
      test_knuth_bendix();
      // println(hlog, "after test_knuth_bendix");
      if(forked) sem_wait(&sem);
      println(hlog, "CSV;",kb_result,";", total_time, ";", isize(rules), ";", kb_result == 0 ? count_tree_states() : 0, ";", max_rule_length, ";", s);
      if(forked) sem_post(&sem);
      fflush(stdout);
      }
    if(forked) exit(0);
    }

  while(children) { int pid; wait(&pid); children--; }
  if(forked) sem_destroy(&sem);
  }

void kb_merge() {
  vector<string> filenames;
  std::ifstream is("devmods/rulegen-tests/all.lst");
  string s;
  while(getline(is, s)) {
    while(s != "" && s[0] == ' ')  s = s.substr(1);
    if(s != "" && s[0] != '#') filenames.push_back(s);
    }
  
  map<string, string> cas;
  if(std::ifstream is = std::ifstream("devmods/rulegen-tests/kbmerge.txt")) {;
    while(getline(is, s)) {
      int i = 0, j = 0;
      for(char c: s) { i++; if(c == ';') j = i; }
      string fname = s.substr(j);
      if(cas.count(fname) && cas[fname].substr(0,5) != s.substr(0,5)) {
//        println(hlog, "repetition:\n", cas[fname], "\n", s);
        s = min(s, cas[fname]);
        }
      cas[fname] = s;
//      println(hlog, "assigned to ", fname, ": ", s);
      }
    }
  
  println(hlog, "CSV;kbres;Tp;kbrules;tree;maxlen;file");
  
  for(auto ss: filenames)
    if(!cas.count(ss))
      println(hlog, "MISSING: ", ss);
    else
      println(hlog, cas[ss]);  
  }

map<cell*, seq> first_seq_last;
map<cell*, seq> first_seq;
map<cell*, int> qty;

void view_kb_tree(const shiftmatrix& V, cellwalker cw, seq s) {
  qty[cw.at]++;
  if(cw.at == lmouseover) println(hlog, s, " ; ", cw.spin);
  for(int i=0; i<cw.at->type; i++) {
    seq s1 = s;
    s1.emplace_back(gen_symbol(shvid(cw.at), i));
    if(is_end_reducible(s1)) continue;
    auto cw1 = cw;
    cw1 += i;
    int is = cw1.spin;
    cw1 += wstep;
    if(!gmatrix.count(cw1.at)) continue;
    transmatrix A = currentmap->adj(cw.at, is);
    int eid = shvid(cw1.at);
    int clen = arb::current.shapes[eid].cycle_length;
    cw1 -= cw1.spin % clen;
    // queueline(V * get_corner_position(cw.at, cw.spin, 6), V * A * get_corner_position(cw1.at, cw1.spin, 6), 0xC00000FF);
    
    seq s2 = s1;
    s2.emplace_back(gen_symbol(shvid(cw1.at), 120));
    
    if(!is_end_reducible(s2)) { // first_seq_last[cw1.at] == s1) {
      if(cw1.at == lmouseover) println(hlog, "is not end_reducible: ", s2);
      vid.linewidth *= 3;
      queueline(V * C0, V * mid(tC0(A), mid(C0, tC0(A))), 0xFFC0C0FF, 3);
      vid.linewidth /= 3;
      }
    
    if(!first_seq.count(cw1.at))
      first_seq[cw1.at] = s1;
    else
      first_seq[cw1.at] = min(s1, first_seq[cw1.at]);

    view_kb_tree(V*A, cw1, s1);
    }
  }

void kb_marker() {
  qty.clear();
  first_seq_last = std::move(first_seq);
  first_seq.clear();
  view_kb_tree(gmatrix[cwt.at], cellwalker(cwt.at, 0), {});
  for(auto p: qty)
    queuestr(gmatrix[p.first], .5, its(p.second), 0xFFFFFFFF, 1);    
  }

void add_kb_view() {
  addHook(hooks_markers, 100, kb_marker);
  }

int u = arg::add3("-kb", [] { 
  test_knuth_bendix(); 
  println(hlog, "result: ", kb_result, " rules: ", isize(rules));
  println(hlog, "final rules:");
  for(auto s: rules) println(hlog, s.first, " => ", s.second);
  if(kb_result == 0) println(hlog, "tree states: ", count_tree_states());
  }) 
  + arg::add3("-kb-test-all", [] { arg::shift(); test_all(arg::args()); }) 
  + arg::add3("-kb-forked", [] { arg::shift(); max_children = arg::argi(); forked = max_children; }) 
  + arg::add3("-kb-rev", [] { arg::shift(); reversed = arg::argi(); }) 
  + arg::add3("-kb-view", add_kb_view) 
  + arg::add3("-kb-merge", kb_merge);

}
