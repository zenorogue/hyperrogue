// HyperRogue -- expansion_analyzer
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

int subtype(cell *c) {
  return patterns::getpatterninfo(c, patterns::PAT_NONE, 0).id;
  }

bignum& bignum::operator +=(const bignum& b) {
  int K = isize(b.digits);
  if(K > isize(digits)) digits.resize(K);
  int carry = 0;
  for(int i=0; i<K || carry; i++) {
    if(i >= isize(digits)) digits.push_back(0);
    digits[i] += carry;
    if(i < K) digits[i] += b.digits[i];
    if(digits[i] >= BASE) {
      digits[i] -= BASE;
      carry = 1;
      }
    else carry = 0;
    }
  return *this;
  }

string bignum::get_str(int max_length) {
  if(digits.empty()) return "0";
  string ret = its(digits.back());
  for(int i=isize(digits)-2; i>=0; i--) {
    if(isize(ret) > max_length && i) {
      ret += XLAT(" (%1 more digits)", its(9 * (i+1)));
      return ret;
      }

    ret += " ";
    string val = its(digits[i]);
    while(isize(val) < 9) val = "0" + val;
    ret += val;
    }
  return ret;
  }

vector<int> expansion_analyzer::gettype(cell *c) {
  vector<int> res;
  res.push_back(subtype(c) * 4 + 2);
  int d = celldist(c);
  for(int i=0; i<c->type; i++) {
    cell *c1 = c->cmove(i);
    res.push_back(subtype(c1) * 4 + celldist(c1) - d);
    }
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
  
void expansion_analyzer::preliminary_grouping() {
  samples.clear();
  codeid.clear();
  children.clear();
  sample_id(currentmap->gamestart());
  for(int i=0; i<isize(samples); i++) {
    children.emplace_back();
    auto c = samples[i];
    int d = celldist(c);
    for(int k=0; k<c->type; k++) {
      cell *c1 = c->cmove(k);
      if(celldist(c1) != d+1) continue;
      cell *c2 = c->cmove((k+1) % c->type);
      if(celldist(c2) != d+1) continue;
      children.back().push_back(sample_id(c1));
      }
    }
  N = isize(samples);
  rootid = 0;
  }

void expansion_analyzer::reduce_grouping() {
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
      sort(childgroups[i].first.begin(), childgroups[i].first.end());
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
  
  vector<int> groupsample(nogroups);
  for(int i=0; i<N; i++) groupsample[grouping[i]] = i;
  vector<vector<int>> newchildren(nogroups);
  for(int i=0; i<nogroups; i++) 
    for(int j: children[groupsample[i]])
      newchildren[i].push_back(grouping[j]);
  children = move(newchildren);
  for(auto& p: codeid) p.second = grouping[p.second];
  N = nogroups;
  rootid = grouping[rootid];
  diskid = N;
  children.push_back(children[rootid]);
  children[diskid].push_back(diskid);
  N++;
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
  long long res = 0;
  for(int t=0; t<isize(coef); t++)
    res += coef[t] * get_descendants(id-t-1).approx_ll();
  return res == get_descendants(id).approx_ll();
  }

int expansion_analyzer::valid(int v, int step) {
  if(step < 0) return 0;
  if(get_descendants(step+v+v+5).approx_int() >= bignum::BASE) return 0;
  ld matrix[100][128];
  for(int i=0; i<v; i++)
  for(int j=0; j<v+1; j++)
    matrix[i][j] = get_descendants(step+i+j).approx();
  
  for(int k=0; k<v; k++) {
    int nextrow = k;
    while(nextrow < v && std::abs(matrix[nextrow][k]) < 1e-6)
      nextrow++;
    if(nextrow == v) return 1;
    if(nextrow != k) {
      // printf("swap %d %d\n", k, nextrow);
      for(int l=0; l<=v; l++) swap(matrix[k][l], matrix[nextrow][l]);
      // display();
      }
    ld divv = 1. / matrix[k][k];
    for(int k1=k; k1<=v; k1++) matrix[k][k1] *= divv;
    // printf("divide %d\n", k);
    // display();
    for(int k1=k+1; k1<v; k1++) if(matrix[k1][k] != 0) {
      ld coef = -matrix[k1][k];
      for(int k2=k; k2<=v; k2++) matrix[k1][k2] += matrix[k][k2] * coef;
      }
    // printf("zeros below %d\n", k);
    // display();
    }
  
  for(int k=v-1; k>=0; k--)
  for(int l=k-1; l>=0; l--)
    if(matrix[l][k]) matrix[l][v] -= matrix[l][k] * matrix[k][v];
  
  coef.resize(v);
  for(int i=0; i<v; i++) coef[i] = int(floor(matrix[v-1-i][v] + .5));
    
  for(int t=step+v; t<step+v+v+5; t++) if(!verify(t)) return 2;
  tested_to = step+v+v+5;
  while(tested_to < step+v+v+100 && get_descendants(tested_to).approx_ll() < bignum::BASE2) {
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
  for(int step=0; step<1000; step++) { 
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
    // for(int i=0; i<N; i++) printf("%lf ", eigen[i]); printf("total = %lf\n", total);
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

bool sizes_known() {
  if(bounded) return false;
  // Castle Anthrax is infinite
  if(binarytiling) return false;
  // not implemented
  if(archimedean) return false;
  return true;  
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

void expansion_analyzer::view_distances_dialog() {
  distcolors[0] = forecolor;
  dialog::init("");
  long long qty[64];
  vector<cell*>& ac = currentmap->allcells();
  for(int i=0; i<64; i++) qty[i] = 0;
  for(int i=0; i<isize(ac); i++) {
    int d = celldistance(ac[i], cwt.at);
    if(d >= 0 && d < 64) qty[d]++;
    }
  
  if(sizes_known()) {
    find_coefficients();
    if(gamerange() >= valid_from && coefficients_known == 2) {
      for(int i=gamerange()+1; i<64; i++) {
        qty[i] = 0;
        for(int j=0; j<isize(coef); j++)
          qty[i] += qty[i-1-j] * coef[j];
        }
      }
    }

  for(int i=0; i<(bounded ? 64 : 16); i++) if(qty[i])
    dialog::addInfo(its(qty[i]), distcolors[i&7]);
  
  if(binarytiling) {
    dialog::addBreak(300);
    dialog::addInfo("a(d) ~ 2ᵈ");
    }
  else if(sizes_known()) {
    if(euclid) {
      dialog::addBreak(300);
      dialog::addInfo("a(d) = " + its(get_descendants(10).approx_int() - get_descendants(9).approx_int()) + "d", forecolor);
      }
    else {
      dialog::addBreak(200);
      
      if(coefficients_known == 2) {
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
        dialog::addHelp(fmt);
        }
      else dialog::addBreak(100);
      
      char buf[20];
      snprintf(buf, 20, "%.8lf", (double) get_growth());
      dialog::addInfo("a(d) ~ " + string(buf) + "ᵈ", forecolor);
      }
    }
  dialog::display();
  }

#if !CAP_MINI
void compute_coefficients() {
  printf("%s %s\n", gp::operation_name().c_str(), ginf[geometry].name);
  start_game();
  
    printf("  sizes:");
    for(int i=0; i<10; i++) printf(" %d", expansion.get_descendants(i).approx_int());
    
    printf("  N = %d\n", expansion.N);

  expansion.find_coefficients();      
  if(expansion.coefficients_known == 2) {
    printf("  coefficients:"); for(int x: expansion.coef) printf(" %d", x);
    printf(" (tested on %d to %d)\n", expansion.valid_from, expansion.tested_to);
    }
  }

int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-vap")) { 
    PHASEFROM(2); 
    start_game();
    while(true) {
      string s = expansion.approximate_descendants(10000, 100);
      printf("s = %s\n", s.c_str());
      if(isize(expansion.descendants) >= 10000) break;
      }
    }
  else if(argis("-csizes")) { 
    PHASEFROM(2); 
    start_game();
    expansion.get_growth();
    for(int i=0; i<30; i++)
      printf("%s / %s\n", expansion.get_descendants(i).get_str(1000).c_str(), expansion.get_descendants(i, expansion.diskid).get_str(1000).c_str());  
    }
  else if(argis("-csolve")) { 
    PHASEFROM(2); 
    start_game();
    printf("preliminary_grouping...\n");
    expansion.preliminary_grouping();
    printf("N = %d\n", expansion.N);
    printf("reduce_grouping...\n");
    expansion.reduce_grouping();
    printf("N = %d\n", expansion.N);
    printf("growth = %lf\n", (double) expansion.get_growth());
    expansion.find_coefficients();      
    if(expansion.coefficients_known == 2) {
      printf("coefficients:"); for(int x: expansion.coef) printf(" %d", x);
      printf(", valid from %d to %d\n", expansion.valid_from, expansion.tested_to);
      }
    }
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
        need_reset_geometry = true;
        set_variation(eVariation::goldberg);
        compute_coefficients();
        }
      }
    }
  else return 1;
  return 0;
  }

auto ea_hook = addHook(hooks_args, 100, readArgs);
#endif

expansion_analyzer expansion;

}