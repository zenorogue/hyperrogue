// Hyperbolic Rogue -- expansion analyzer
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file expansion.cpp
 *  \brief exponential growth of hyperbolic geometries
 *
 *  Calculations related to this exponential growth.
 *  Screens which display this exponential growth (e.g. 'size of the world' in geometry experiments) are also implemented here.
 */

#include "hyper.h"
namespace hr {

int subtype(cell *c) {
  return patterns::getpatterninfo(c, patterns::PAT_NONE, 0).id;
  }

void canonicize(vector<int>& t) {
  for(int i=2; i<isize(t); i++)
    if((t[i] & 3) == 1 && (t[i-1] & 3) != 1)
      std::rotate(t.begin()+1, t.begin()+i, t.end());
  }

#if HDR
struct expansion_analyzer {
  int sibling_limit;
  vector<int> gettype(cell *c);
  int N;
  vector<cell*> samples;  
  map<vector<int>, int> codeid;  
  vector<vector<int> > children;  
  int rootid, diskid;
  int coefficients_known;
  #if CAP_GMP
  vector<mpq_class> coef;
  #else
  vector<int> coef;
  #endif
  int valid_from, tested_to;
  ld growth;
  
  int sample_id(cell *c);
  void preliminary_grouping();
  void reduce_grouping();
  vector<vector<bignum>> descendants;
  bignum& get_descendants(int level);
  bignum& get_descendants(int level, int type);
  void find_coefficients();
  void reset();
  
  expansion_analyzer() { reset(); }

  string approximate_descendants(int d, int max_length);
  void view_distances_dialog();
  ld get_growth();

  private:
  bool verify(int id);
  int valid(int v, int step);
  };
#endif

vector<int> expansion_analyzer::gettype(cell *c) {
  vector<int> res;
  res.push_back(subtype(c) * 4 + 2);
  int d = celldist(c);
  for(int i=0; i<c->type; i++) {
    cell *c1 = c->cmove(i);
    int bonus = 0;
    if(bt::in()) bonus += 16 * (celldistAlt(c1) - celldistAlt(c));
    res.push_back(bonus + subtype(c1) * 4 + celldist(c1) - d);
    }
  canonicize(res);
  return res;
  }

int expansion_analyzer::sample_id(cell *c) {
  auto t = gettype(c);
  if(codeid.count(t)) return codeid[t];
  auto &cit = codeid[t];
  cit = isize(samples);
  samples.push_back(c);
  return cit;
  }

template<class T, class U> vector<int> get_children_codes(cell *c, const T& distfun, const U& typefun) {
  vector<int> res;
  int d = distfun(c);
  cellwalker cw(c, 0);
  if(d > 0) {
    forCellCM(c2, c) if(celldist(cw.peek()) < d) break; else cw++;
    }
  for(int k=0; k<c->type; k++) {
    cell *c1 = cw.cpeek();
    cw++;
    if(distfun(c1) != d+1) continue;
    cell *c2 = cw.cpeek();
    if(distfun(c2) != d+1) continue;
    res.push_back(typefun(c1));
    }
  return res;
  }

void expansion_analyzer::preliminary_grouping() {
  samples.clear();
  codeid.clear();
  children.clear();
  if(currentmap->strict_tree_rules()) {
    N = isize(rulegen::treestates);
    children.resize(N);
    rootid = rulegen::rule_root;
    for(int i=0; i<N; i++)
      for(int v: rulegen::treestates[i].rules)
        if(v >= 0) children[i].push_back(v);
    }
  else if(reg3::exact_rules()) {
#if MAXMDIM >= 4
    rootid = reg3::rule_get_root(0);
    auto& chi = reg3::rule_get_children();
    auto& chpos = reg3::rule_get_childpos();
    N = isize(chpos) - 1;
    children.resize(N);
    int k = 0;
    for(int i=0; i<N; i++) for(int j=0; j<chpos[i+1]-chpos[i]; j++) {
      int ck = chi[k];
      if(ck < -1) ck += (1<<16);
      if(ck >= 0)
        children[i].push_back(ck);
      k++;
      }
#endif
    }
  else {
    sample_id(currentmap->gamestart());
    // queue for, do not change to range-based for
    for(int i=0; i<isize(samples); i++) 
      children.push_back(get_children_codes(samples[i], celldist, [this] (cell *c) { return sample_id(c); }));
    N = isize(samples);
    rootid = 0;
    }
  diskid = N;
  children.push_back(children[rootid]);
  children[diskid].push_back(diskid);
  N++;
  }

void expansion_analyzer::reduce_grouping() {
  if(reg3::exact_rules()) return;
  if(currentmap->strict_tree_rules()) return;
  int old_N = N;
  vector<int> grouping;
  grouping.resize(N);
  int nogroups = 1;
  for(int i=0; i<N; i++) grouping[i] = 0;
  while(true) {
    vector< pair<vector<int>, int > > childgroups(N);
    for(int i=0; i<N; i++) {
      childgroups[i].second = i;
      for(int j: children[i])
        childgroups[i].first.push_back(grouping[j]);
      // sort(childgroups[i].first.begin(), childgroups[i].first.end());
      }
    sort(childgroups.begin(), childgroups.end());
    int newgroups = 0;
    for(int i=0; i<N; i++) {
      if(i == 0 || childgroups[i].first != childgroups[i-1].first) newgroups++;
      grouping[childgroups[i].second] = newgroups - 1;
      }
    if(nogroups == newgroups) break;
    nogroups = newgroups;
    }
  
  vector<int> groupsample(nogroups, -1);
  for(int i=0; i<N; i++) {
    int& g = groupsample[grouping[i]];
    if(g == -1) g = i;
    }
  
  vector<int> reorder(nogroups);
  for(int i=0; i<nogroups; i++) reorder[i] = i;
  sort(reorder.begin(), reorder.end(), [&] (int i, int j) { return groupsample[i] < groupsample[j]; });

  vector<int> inv_reorder(nogroups);
  for(int i=0; i<nogroups; i++) inv_reorder[reorder[i]] = i;

  for(int i=0; i<N; i++) grouping[i] = inv_reorder[grouping[i]];

  for(int i=0; i<N; i++) groupsample[grouping[i]] = i;
  
  vector<vector<int>> newchildren(nogroups);
  for(int i=0; i<nogroups; i++) 
    for(int j: children[groupsample[i]])
      newchildren[i].push_back(grouping[j]);
  children = std::move(newchildren);
  for(auto& p: codeid) p.second = grouping[p.second];
  N = nogroups;
  rootid = grouping[rootid];
  diskid = grouping[diskid];
  for(int g=0; g<old_N; g++) if(grouping[g] != g) descendants.clear();
  }

template<class T> int size_upto(vector<T>& v, int s) {
  int res = isize(v);
  if(res < s) v.resize(s);
  return res;
  }

bignum& expansion_analyzer::get_descendants(int level) {
  if(!N) preliminary_grouping(), reduce_grouping();
  return get_descendants(level, rootid);
  }

bignum& expansion_analyzer::get_descendants(int level, int type) {
  if(!N) preliminary_grouping(), reduce_grouping();
  auto& pd = descendants;
  size_upto(pd, level+1);
  for(int d=0; d<=level; d++)
  for(int i=size_upto(pd[d], N); i<N; i++)
    if(d == 0) pd[d][i].be(1);
    else for(int j: children[i]) 
      pd[d][i] += pd[d-1][j];
  return pd[level][type];
  }

bool expansion_analyzer::verify(int id) {
  if(id < isize(coef)) return false;
  #if CAP_GMP
  mpq_class res = 0;
  for(int t=0; t<isize(coef); t++)
    res += coef[t] * get_descendants(id-t-1).as_mpq();
  return res == get_descendants(id).as_mpq();
  #else
  long long res = 0;
  for(int t=0; t<isize(coef); t++)
    res += coef[t] * get_descendants(id-t-1).approx_ll();
  return res == get_descendants(id).approx_ll();
  #endif
  }

int expansion_analyzer::valid(int v, int step) {
  if(step < 0) return 0;
  int more = reg3::exact_rules() ? 1 : 5;
  #if CAP_GMP == 0
  if(get_descendants(step+v+v+more).approx_int() >= bignum::BASE) return 0;
  typedef ld val;
  const val unit = 1;
  #else
  typedef mpq_class val;
  const val unit = 1;
  #endif
  val matrix[100][128];
  for(int i=0; i<v; i++)
  for(int j=0; j<v+1; j++)
    #if CAP_GMP == 0
    matrix[i][j] = get_descendants(step+i+j).approx_ll();
    #else
    matrix[i][j] = get_descendants(step+i+j).as_mpq();
    #endif

  for(int k=0; k<v; k++) {
    int nextrow = k;
    #if CAP_GMP == 0
    while(nextrow < v && std::abs(matrix[nextrow][k]) < 1e-6)
      nextrow++;
    #else
    while(nextrow < v && matrix[nextrow][k] == 0)
      nextrow++;
    #endif
    if(nextrow == v) return 1;
    if(nextrow != k) {
      // printf("swap %d %d\n", k, nextrow);
      for(int l=0; l<=v; l++) swap(matrix[k][l], matrix[nextrow][l]);
      // display();
      }
    val divv = unit / matrix[k][k];
    for(int k1=k; k1<=v; k1++) matrix[k][k1] *= divv;
    // printf("divide %d\n", k);
    // display();
    for(int k1=k+1; k1<v; k1++) if(matrix[k1][k] != 0) {
      val coef = -matrix[k1][k];
      for(int k2=k; k2<=v; k2++) matrix[k1][k2] += matrix[k][k2] * coef;
      }
    // printf("zeros below %d\n", k);
    // display();
    }
  
  for(int k=v-1; k>=0; k--)
  for(int l=k-1; l>=0; l--)
    if(matrix[l][k]) matrix[l][v] -= matrix[l][k] * matrix[k][v];
  
  coef.resize(v);
  #if CAP_GMP
  for(int i=0; i<v; i++) coef[i] = matrix[v-1-i][v];
  #else
  for(int i=0; i<v; i++) coef[i] = int(floor(matrix[v-1-i][v] + .5));
  #endif
    
  for(int t=step+v; t<step+v+v+more; t++) if(!verify(t)) return 2;
  tested_to = step+v+v+more;
  while(tested_to < step+v+v+100) {
    #if !CAP_GMP
    if(get_descendants(tested_to).approx_ll() >= bignum::BASE2) break;
    #endif
    if(!verify(tested_to)) return 2;
    tested_to++;
    }
  
  valid_from = step+v;
  return 3;
  }

void expansion_analyzer::find_coefficients() {
  if(coefficients_known) return;
  if(!N) preliminary_grouping(), reduce_grouping();
  for(int v=1; v<25; v++) 
  for(int step=0; step<3 * v; step++) { 
    int val = valid(v, step);
    if(val == 0) break;
    if(val == 3) { coefficients_known = 2; return; }
    }
  coefficients_known = 1;
  }

ld growth;

ld expansion_analyzer::get_growth() {
  if(growth >= 1) return growth;
  if(!N) preliminary_grouping(), reduce_grouping();
  vector<ld> eigen(N, 1);
  ld total;

  for(int iter=0; iter<100000; iter++) {
    total = 0;
    vector<ld> neweigen(N, 0);
    for(int i=0; i<N; i++) {
      for(int j: children[i]) neweigen[i] += eigen[j];
      total += neweigen[i];
      }
    for(int i=0; i<N; i++) eigen[i] = .1 * eigen[i] + .9 * neweigen[i] / total;
    }
  return growth = total;
  }

void expansion_analyzer::reset() {
  N = 0;
  growth = 0;
  coefficients_known = 0;
  samples.clear();
  codeid.clear();
  children.clear();
  coef.clear();
  descendants.clear();
  }

EX int type_in(expansion_analyzer& ea, cell *c, const cellfunction& f) {
  if(!ea.N) ea.preliminary_grouping(), ea.reduce_grouping();
  vector<int> res;
  res.push_back(subtype(c) * 4 + 2);
  int d = f(c);
  for(int i=0; i<c->type; i++) {
    cell *c1 = c->cmove(i);
    int bonus = 0;
    if(bt::in()) bonus += 16 * (celldistAlt(c1) - celldistAlt(c));
    res.push_back(bonus + subtype(c1) * 4 + f(c1) - d);
    }
  
  canonicize(res);
  if(ea.codeid.count(res)) return ea.codeid[res];
  int ret = ea.N++;
  ea.codeid[res] = ret;
  
  ea.children.emplace_back();
  ea.children[ret] = get_children_codes(c, f, [&ea, &f] (cell *c1) { return type_in(ea, c1, f); });

  return ret;
  }

int type_in_quick(expansion_analyzer& ea, cell *c, const cellfunction& f) {
  vector<int> res;
  res.push_back(subtype(c) * 4 + 2);
  int d = f(c);
  for(int i=0; i<c->type; i++) {
    cell *c1 = c->cmove(i);
    int dd = f(c1) - d;
    if(dd < -1 || dd > 1) return -1;
    res.push_back(subtype(c1) * 4 + dd);
    }
  
  canonicize(res);
  if(ea.codeid.count(res)) return ea.codeid[res];
  return -1;
  }

EX bool sizes_known() {
  if(reg3::exact_rules()) return true;
  if(closed_manifold) return false;
  // Castle Anthrax is infinite
  if(bt::in()) return false;
  // not implemented
  if(arcm::in()) return false;
  if(kite::in()) return false;
  if(currentmap->strict_tree_rules()) return true;
  if(arb::in()) return false;
  return true;  
  }

EX bool trees_known() {
  return sizes_known() && !(BITRUNCATED && a4 && S7 <= 5);
  }

string expansion_analyzer::approximate_descendants(int d, int max_length) {
  auto t = SDL_GetTicks();
  while(isize(descendants) <= d && SDL_GetTicks() < t + 100)
    get_descendants(isize(descendants));
  if(isize(descendants) > d) 
    return get_descendants(d).get_str(max_length);
  int v = isize(descendants) - 1;
  bignum& b = get_descendants(v);
  if(b.digits.empty()) return "0";
  ld log_10 = log(b.digits.back()) / log(10) + 9 * (isize(b.digits) - 1) + (d - v) * log(get_growth()) / log(10);
  int more_digits = int(log_10);
  return XLAT("about ") + fts(pow(10, log_10 - more_digits)) + "E" + its(more_digits);
  }

#if HDR
enum eDistanceFrom { dfPlayer, dfStart, dfWorld };
#endif
EX string dfnames[3] = { "player", "start", "land" };

EX eDistanceFrom distance_from = dfPlayer;

#if HDR
enum eNumberCoding { ncNone, ncDistance, ncType, ncDebug, ncError };
#endif
EX string ncnames[5] = { "NO", "distance", "type", "debug", "error" };
EX eNumberCoding number_coding = ncDistance;

EX bool mod_allowed() {
  return cheater || autocheat || arcm::in() || arb::in() || tour::on;
  }

EX int curr_dist(cell *c) {
  switch(distance_from) {
    case dfPlayer:
      return c->cpdist < INFD ? c->cpdist : celldistance(cwt.at, c);
    case dfStart:
      if(!mod_allowed()) return 0;
      return celldist(c);
    case dfWorld:
      if(!mod_allowed() && !among(c->land, laOcean, laIvoryTower, laEndorian, laDungeon, laTemple, laWhirlpool, laCanvas))
        return 0;
      if((isCyclic(c->land) || among(c->land, laCanvas, laCaribbean, laStorms, laRlyeh))) {
        if(eubinary || c->master->alt) return celldistAlt(c);
        return UNKNOWN;
        }
      return inmirror(c) ? (c->landparam & 255) : c->landparam;
    }
  return 0;    
  }

int position;

EX int type_in_reduced(expansion_analyzer& ea, cell *c, const cellfunction& f) {
  int a = ea.N;
  int t = type_in(ea, c, f);
  auto& expansion = get_expansion();
  if(expansion.N != a) {
    expansion.reduce_grouping();
    t = type_in(ea, c, f);
    }
  return t;
  }

// which=1 => right, which=-1 => left

EX int parent_id(cell *c, int which, const cellfunction& cf) {
  int d = cf(c)-1;
  for(int i=0; i<c->type; i++) {
    
    if(cf(c->cmove(i)) == d) {
      int steps = 0;
      again:
      if(!which || steps == c->type) return i;
      int i2 = c->c.fix(i+which);
      if(cf(c->cmove(i2)) == d) {
        i = i2; steps++; goto again;
        }
      else return i;
      }
    }
  
  return -1;
  }

// set which=1,bonus=1 to get right neighbor on level

EX void generate_around(cell *c) {
  forCellCM(c2, c) if(c2->mpdist > BARLEV) setdist(c2, BARLEV, c);
  }
  
EX namespace ts {
  EX cell *verified_add(cell *c, int which, int bonus, const cellfunction& cf) {
    int id = parent_id(c, which, cf);
    if(id == -1) return NULL;
    return c->cmodmove(id + bonus);
    }

  EX cell *verified_add_gen(cell *c, int which, int bonus, const cellfunction& cf) {
    return verified_add(c, which, bonus, cf);
    }
  
  EX cell *add(cell *c, int which, int bonus, const cellfunction& cf) {
    int pid = parent_id(c, which, cf);
    if(pid == -1) pid = 0;
    return c->cmodmove(pid + bonus);
    }
  
  EX cell *left_of(cell *c, const cellfunction& cf) {
    int pid = parent_id(c, 1, cf);
    if(pid == -1) return c;
    if(valence() == 3) return c->cmodmove(pid+1);
    else return (cellwalker(c, pid) + wstep - 1).cpeek();
    }

  EX cell *right_of(cell *c, const cellfunction& cf) {
    int pid = parent_id(c, -1, cf);
    if(pid == -1) return c;
    if(valence() == 3) return c->cmodmove(pid-1);
    else return (cellwalker(c, pid) + wstep + 1).cpeek();
    }

  EX cell *child_number(cell *c, int id, const cellfunction& cf) { 
    int pid = parent_id(c, 1, cf);
    if(pid == -1) return c->cmove(id);
    return c->cmodmove(pid + (valence() == 3 ? 2 : 1) + id);
    }

  #if HDR
  inline cell *left_parent(cell *c, const cellfunction& cf) { return verified_add(c, 1, 0, cf); }
  inline cell *right_parent(cell *c, const cellfunction& cf) { return verified_add(c, -1, 0, cf); }
  #endif

  EX }

EX bool viewdists = false;
EX bool use_color_codes = true;
EX bool use_analyzer = true;
EX bool show_distance_lists = true;

int first_distance = 0, scrolltime = 0;
bool scrolling_distances = false;

EX map<int, color_t> expcolors;

color_t distribute_color(int id) {
  if(expcolors.count(id)) return expcolors[id];
  color_t v = forecolor; // 0xFFFFFF;
  for(int z=0; z<24; z++) if(id & (1<<z)) part(v, (z%3)) ^= (1<<(7-(z/3)));
  return v; 
  }

EX bool dist_label_colored = true;
EX color_t dist_label_color = 0;

void celldrawer::do_viewdist() {
  if(behindsphere(V)) return;
  
  int cd = (use_color_codes || number_coding == ncDistance || number_coding == ncDebug) ? curr_dist(c) : 0;
  
  if(use_color_codes) {
    int dc = distcolors[cd];
    wcol = gradient(wcol, dc, 0, .4, 1);
    fcol = gradient(fcol, dc, 0, .4, 1);
    }
  
  string label = "";
  int dc = 0xFFD500;
 
  switch(number_coding) {
    case ncDistance: { 
      label = cd == UNKNOWN ? "?" : its(cd);
      dc = distcolors[cd];
      break;
      }
    case ncType: {
      int t = -1;
      if(reg3::exact_rules()) switch(distance_from) {
        case dfPlayer: 
          t = -1;
          break;
        case dfStart:
          t = c->master->fiftyval;
          break;
        case dfWorld:
          if(c->master->alt) t = c->master->alt->fiftyval;
          break;
        }
      else if(currentmap->strict_tree_rules()) switch(distance_from) {
        case dfPlayer: 
          t = -1;
          break;
        case dfStart:
          t = c->master->fieldval;
          break;
        case dfWorld:
          if(c->master->alt) t = c->master->alt->fieldval;
          break;
        }
      else t = type_in_reduced(get_expansion(), c, curr_dist);
      if(t >= 0) label = its(t), dc = distribute_color(t);
      break;
      }
    case ncDebug: {
      int d = 
        distance_from == dfStart && cwt.at == currentmap->gamestart() && c->cpdist < INFD ? c->cpdist : 
        celldistance(c, distance_from == dfPlayer ? cwt.at : currentmap->gamestart());
      dc = (d != cd) ? 0xFF0000 : 0x00FF00;
      label = its(d);
      }
    case ncError: {
      if(pointer_indices.count(c)) label = index_pointer(c);
      }
    case ncNone: ;
    }
  
  if(!dist_label_colored) dc = dist_label_color;

  if(label != "")
    queuestr(V, (isize(label) > 1 ? .6 : 1), label, 0xFF000000 + dc, 1);
  }

EX void viewdist_configure_dialog() {
  dialog::init("");
  cmode |= sm::SIDE | sm::MAYDARK | sm::EXPANSION;
  gamescreen();
  
  dialog::addSelItem(XLAT("which distance"), XLAT(dfnames[distance_from]), 'c');
  dialog::add_action([] () { distance_from = mod_allowed() ? eDistanceFrom((distance_from + 1) % 3) : eDistanceFrom(2 - distance_from); });
             
  dialog::addSelItem(XLAT("number codes"), XLAT(ncnames[number_coding]), 'n');
  dialog::add_action([] () { number_coding = eNumberCoding((number_coding + 1) % (mod_allowed() ? 4 : 2)); });

  dialog::addBoolItem_action(XLAT("color codes"), use_color_codes, 'u');

  dialog::addSelItem(XLAT("display distances from"), its(first_distance), 'd');
  dialog::add_action([] () { 
    scrolling_distances = false;
    dialog::editNumber(first_distance, 0, 3000, 1, 0, XLAT("display distances from"), "");
    dialog::bound_low(0);
    });

  dialog::addBoolItem(XLAT("strict tree maps"), currentmap->strict_tree_rules(), 's');
  dialog::add_action_push(rulegen::show);

  dialog::addItem(XLAT("line patterns"), 'L');
  dialog::add_action_push(linepatterns::showMenu);
  
  if(!mod_allowed()) {
    dialog::addItem(XLAT("enable the cheat mode for additional options"), 'C');
    dialog::add_action(enable_cheat);
    }
  else 
    dialog::addBreak(100);

  dialog::addBreak(100);

  dialog::addItem(XLAT("disable"), 'x');
  dialog::add_action([] () { viewdists = false; popScreen(); });
  
  dialog::addItem(XLAT("move the player"), 'm');
  dialog::add_action([] () { show_distance_lists = false; popScreenAll(); });
  
  dialog::addItem(distance_from ? XLAT("show number of descendants by distance") : XLAT("show number of cells by distance"), 'l');
  dialog::add_action([] () { show_distance_lists = true; popScreenAll(); });

  dialog::display();
  }

bool is_descendant(cell *c) {
  if(c == cwt.at) return true;
  if(curr_dist(c) < curr_dist(cwt.at)) return false;
  return is_descendant(ts::right_parent(c, curr_dist));
  }

const int scrollspeed = 100;

bool not_only_descendants = false;

#if CAP_GMP
string produce_coef_formula(vector<mpq_class> coef) {
#else
string produce_coef_formula(vector<int> coef) {
#endif
  string fmt = "a(d+" + its(isize(coef)) + ") = ";
  bool first = true;
  for(int i=0; i<isize(coef); i++) if(coef[i]) {
    if(first && coef[i] == 1) ;
    else if(first) fmt += its(coef[i]);
    else if(coef[i] == 1) fmt += " + ";
    else if(coef[i] == -1) fmt += " - ";
    else if(coef[i] > 1) fmt += " + " + its(coef[i]);
    else if(coef[i] < -1) fmt += " - " + its(-coef[i]);
    fmt += "a(d";
    if(i != isize(coef) - 1)
      fmt += "+" + its(isize(coef) - 1 - i);
    fmt += ")";
    first = false;
    }
  return fmt;
  }

void expansion_analyzer::view_distances_dialog() {
  static int lastticks;
  if(scrolling_distances && !closed_manifold) {
    scrolltime += SDL_GetTicks() - lastticks;
    first_distance += scrolltime / scrollspeed;
    scrolltime %= scrollspeed;
    }
  lastticks = SDL_GetTicks();
  if(first_distance < 0) first_distance = 0;
  
  dynamicval<color_t> dv(distcolors[0], forecolor);
  dialog::init("");
  cmode |= sm::DIALOG_STRICT_X | sm::EXPANSION;
  
  int maxlen = closed_manifold ? 128 : 16 + first_distance;
  vector<bignum> qty(maxlen);
  auto& expansion = get_expansion();
  
  bool really_use_analyzer = use_analyzer && sizes_known();
  
  if(really_use_analyzer) {
    int t;
    if(reg3::exact_rules() || currentmap->strict_tree_rules()) {
      if(!N) preliminary_grouping();      
      t = rootid;
      }
    else 
      t = type_in_reduced(expansion, cwt.at, curr_dist);
    for(int r=0; r<maxlen; r++)
      qty[r] = expansion.get_descendants(r, t);
    }
  else {
    if(distance_from == dfPlayer) {
      celllister cl(cwt.at, closed_manifold ? maxlen-1 : gamerange(), 100000, NULL);
      for(int d: cl.dists)
        if(d >= 0 && d < maxlen) qty[d]++;
      }
    else {
      celllister cl(cwt.at, closed_manifold ? maxlen-1 : gamerange(), 100000, NULL);
      for(cell *c: cl.lst) if((not_only_descendants || is_descendant(c)) && curr_dist(c) < maxlen) qty[curr_dist(c)]++;
      }
    #if !CAP_GMP
    if(sizes_known() && !not_only_descendants) {
      find_coefficients();
      if(gamerange()+1 >= valid_from && coefficients_known == 2) {
        for(int i=gamerange()+1; i<maxlen; i++)
          for(int j=0; j<isize(coef); j++) {
            qty[i].addmul(qty[i-1-j], coef[j]);
            }
        }
      }
    #endif
    }
  
  dialog::addBreak(100 - 100 * scrolltime / scrollspeed);

  for(int i=first_distance; i<maxlen; i++) if(!qty[i].digits.empty())
    dialog::addInfo(its(i) + ": " + qty[i].get_str(100), distcolors[i]);
  
  dialog::addBreak(100 * scrolltime / scrollspeed);

  if(sizes_known() || bt::in()) {
    if(euclid && !arb::in()) {
      dialog::addBreak(200);
      dialog::addInfo("a(d) = " + its(get_descendants(10).approx_int() - get_descendants(9).approx_int()) + "d", forecolor);
      }
    else {
      dialog::addBreak(100);
      
      find_coefficients();
      if(coefficients_known == 2) {
        string fmt = produce_coef_formula(coef);
        fmt += " (d>" + its(valid_from-1) + ")";
        dialog::addHelp(fmt);
        }
      else dialog::addBreak(100);
      
      dialog::addInfo("Θ(" + fts(get_growth(), 8) + "...ᵈ)", forecolor);
      }
    }
  
  dialog::addItem(XLAT("scroll"), 'S');
  dialog::addItem(XLAT("configure"), 'C');
  dialog::display();
  }

EX void enable_viewdists() {
  first_distance = 0;
  scrolltime = 0;
  viewdists = true;
  if(!mod_allowed()) {
    number_coding = ncDistance;
    distance_from = dfPlayer;
    }
  show_distance_lists = true;
  }

bool expansion_handleKey(int sym, int uni) {
  if((cmode & sm::NORMAL) && viewdists) {
    if(uni == 'S' && (cmode & sm::EXPANSION)) scrolling_distances = !scrolling_distances;
    else if(uni == 'C') pushScreen(viewdist_configure_dialog);
    else if(uni == 'A' && (cmode & sm::EXPANSION)) use_analyzer = !use_analyzer;
    else if(sym == SDLK_ESCAPE) first_distance = 0, viewdists = false;
    else return false;
    return true;
    }
  return false;
  }

int expansion_hook = addHook(hooks_handleKey, 0, expansion_handleKey);

#if !ISMINI
void compute_coefficients() {
  println(hlog, gp::operation_name(), " ", ginf[geometry].tiling_name);
  start_game();
  
    auto& expansion = get_expansion();
    printf("  sizes:");
    for(int i=0; i<expansion.valid_from+10; i++) printf(" %d", expansion.get_descendants(i).approx_int());
    
    printf("  N = %d\n", expansion.N);

  expansion.find_coefficients();      
  if(expansion.coefficients_known == 2) {
    println(hlog, "  coefficients:"); 
    for(auto& x: expansion.coef) println(hlog, " ", x);
    println(hlog, " (tested on %d to %d)\n", expansion.valid_from, expansion.tested_to);
    }
  }

#if CAP_COMMANDLINE
int expansion_readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-vap")) { 
    PHASEFROM(2); 
    start_game();
    auto& expansion = get_expansion();
    shift(); int radius = argi();
    while(true) {
      string s = expansion.approximate_descendants(radius, 100);
      printf("s = %s\n", s.c_str());
      if(isize(expansion.descendants) >= radius) break;
      }
    }
  else if(argis("-csizes")) { 
    PHASEFROM(2); 
    start_game();
    auto& expansion = get_expansion();
    expansion.get_growth();
    shift(); for(int i=0; i<argi(); i++)
      printf("%s / %s\n", expansion.get_descendants(i).get_str(1000).c_str(), expansion.get_descendants(i, expansion.diskid).get_str(1000).c_str());  
    }
  else if(argis("-csolve")) { 
    PHASEFROM(2); 
    start_game();
    auto& expansion = get_expansion();
    printf("preliminary_grouping...\n");
    expansion.preliminary_grouping();
    printf("N = %d\n", expansion.N);
    for(int i=0; i<expansion.N; i++) {
      printf("%d:", i);
      for(int c: expansion.children[i]) printf(" %d", c);
      printf("\n");
      }
    printf("reduce_grouping...\n");
    expansion.reduce_grouping();
    printf("N = %d\n", expansion.N);
    for(int i=0; i<expansion.N; i++) {
      printf("%d:", i);
      for(int c: expansion.children[i]) printf(" %d", c);
      printf("\n");
      }
    println(hlog, "growth = ", expansion.get_growth());
    expansion.find_coefficients();      
    if(expansion.coefficients_known == 2) {

      println(hlog, "  sizes:");
      for(int i=0; i<expansion.valid_from+10; i++) 
        println(hlog, "[", i, "] = ", expansion.get_descendants(i).get_str(10000));

      println(hlog, "  disks:");
      for(int i=0; i<expansion.valid_from+10; i++) 
        println(hlog, "[", i, "] = ", expansion.get_descendants(i, expansion.diskid).get_str(10000));
      
      vector<string> disks;
      for(int i=0; i<expansion.valid_from+10; i++) 
        disks.push_back(expansion.get_descendants(i, expansion.diskid).get_str(10000));      
      println(hlog, "disks = ", disks);

      println(hlog, "coefficients: ", expansion.coef); 
      println(hlog, "i.e. ", produce_coef_formula(expansion.coef)); 
      println(hlog, "coefficients tested from ", expansion.valid_from, " to ", expansion.tested_to);
      }
    }
  #if CAP_GP
  else if(argis("-csolve_tab")) {
    for(eGeometry geo: {gNormal, gOctagon, g45, g46, g47}) {
      set_geometry(geo);
      set_variation(eVariation::pure);
      compute_coefficients();
      set_variation(eVariation::bitruncated);
      compute_coefficients();
      for(int x=1; x<9; x++)
      for(int y=0; y<=x; y++) {
        if(x == 1 && y == 0) continue;
        if(x == 1 && y == 1 && S3 == 3) continue;
        if(x+y > 10) continue;
        stop_game();
        gp::param = gp::loc(x, y);
        set_variation(eVariation::goldberg);
        compute_coefficients();
        }
      }
    }
  #endif

  else if(argis("-expansion")) {
    cheat(); viewdists = true;
    shift(); distance_from = (eDistanceFrom) argi();
    shift(); number_coding = (eNumberCoding) argi();
    shift(); use_color_codes = argi() & 1; use_analyzer = argi() & 2; show_distance_lists = argi() & 4;
    not_only_descendants = argi() & 8;
    }

  else if(argis("-expansion-labelcolor")) {
    dist_label_colored = false;
    shift(); dist_label_color = argcolor(24);
    }

  else if(argis("-expansion-off")) {
    viewdists = false;
    }

  else return 1;
  return 0;
  }

auto ea_hook = addHook(hooks_args, 100, expansion_readArgs);
#endif
#endif

EX expansion_analyzer& get_expansion() {
  if(!cgi.expansion) cgi.expansion = make_shared<expansion_analyzer> ();
  return *cgi.expansion;
  }

EX void set_sibling_limit() {
  auto& sibling_limit = get_expansion().sibling_limit;
  if(0) ;
  #if CAP_IRR
  else if(IRREGULAR) sibling_limit = 3;
  #endif
  #if CAP_BT
  else if(bt::in()) sibling_limit = 3;
  #endif
  #if CAP_GP
  else {
    auto p = gp::univ_param();
    sibling_limit = 2 * p.first + p.second;
    }
  #else
  else sibling_limit = PURE ? 2 : 3;
  #endif
  }

int celldist0(cell *c) {
  if(bt::in()) return celldistAlt(c);
  else return celldist(c);
  }

bool in_segment(cell *left, cell *mid, cell *right) {
  while(true) {
    if(mid == left) return true;
    if(left == right) return false;
    left = ts::right_of(left, celldist0);
    }
  }

int sibling_distance(cell *a, cell *b, int limit) {
  int counting = 0;
  while(true) {
    if(a == b) return counting;
    if(limit == 0) return INF;
    counting++; limit--;
    a = ts::right_of(a, celldist0);
    }
  }

/** An algorithm for computing distance between two cells.
    This algorithm runs correctly in O(d) assuming that:
    - distances from the origin are known
    - the set of cells in distance d from the origin forms a cycle
    - the map is Gromov hyperbolic (with sibling_limit computed correctly) and planar
    - all vertices have valence <= 4
    - each vertex has at most two parents
    */
EX int hyperbolic_celldistance(cell *c1, cell *c2) {
  int found_distance = INF;
  
  int d = 0, d1 = celldist0(c1), d2 = celldist0(c2), sl_used = 0;
  auto& sibling_limit = get_expansion().sibling_limit;

  cell *cl1=c1, *cr1=c1, *cl2=c2, *cr2=c2;
  while(true) {
  
    if(a45 && BITRUNCATED) {
      // some cells in this tiling have three parents,
      // making the usual algorithm fail
      if(d2 == d1+1) {
        swap(d1, d2); swap(cl1, cl2); swap(c1, c2); swap(cr1, cr2);
        }
      auto short_distances = [cl1, cr1, d, &found_distance] (cell *c) {
        celllister cl(c, 4, 1000, cl1);
        if(cl.listed(cl1)) found_distance = min(found_distance, d + cl.getdist(cl1));
        if(cl.listed(cr1)) found_distance = min(found_distance, d + cl.getdist(cr1));
        };
      
      if(d1 <= d2+1) {
        short_distances(cl2);
        if(cl2 != cr2) short_distances(cr2);
        }
      }
    
    if(d >= found_distance) {
      if(sl_used == sibling_limit && IRREGULAR) { 
        printf("sibling_limit used: %d\n", sibling_limit); sibling_limit++;
        }
      return found_distance;
      }

    if(d1 == d2) {
      if(cl1 == c1 && in_segment(cl2, c1, cr2)) return d;
      if(cl2 == c2 && in_segment(cl1, c2, cr1)) return d;
      if(valence() == 3) {
        int dx = min(sibling_distance(cr1, cl2, sibling_limit), sibling_distance(cr2, cl1, sibling_limit));
        if(d + dx <= found_distance) {
          found_distance = d + dx;
          sl_used = dx;
          }
        }
      else {
        if(cl1 == cr2 || cr1 == cl2) found_distance = d;
        }
      }    
    
    if(d >= found_distance) {
      if(sl_used == sibling_limit && IRREGULAR) { 
        printf("sibling_limit used: %d\n", sibling_limit); sibling_limit++; 
        }
      return found_distance;
      }

    if(d1 >= d2) {
      cl1 = ts::left_parent(cl1, celldist0);
      cr1 = ts::right_parent(cr1, celldist0);
      d++; d1--;
      }
    if(d1 < d2) {
      cl2 = ts::left_parent(cl2, celldist0);
      cr2 = ts::right_parent(cr2, celldist0);
      d++; d2--;
      }
    }
  }

}
