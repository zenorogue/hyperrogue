#include <iostream>
#include <unordered_map>

// pseudo-betweenness

namespace dhrg {

int out(int x, string cmt="") {
  println(hlog, "result = ", x, " ", cmt);
  return x;
  }

ld outNZ(ld x, string cmt="") { 
  return x;
  }

int segmentlen(segment *s) {
  auto l = s->left;
  auto r = s->right;
  int i = 1;
  while(l != r) l = l->grightsibling(), i++;
  return i;
  }

int get0(qtybox& b) { return b.minv == 0 ? b[0] : 0; }

int tallybox_total(qtybox& box) {
  int tt = 0;
  for(int i=box.minv; i<box.maxv; i++)
    tt += box[i];
  return tt;
  }

string segdesc(segment *s) {
  return format("(%s-%s: len=%d, qty=%d/%d)", get_path(s->left).c_str(), get_path(s->right).c_str(), segmentlen(s), get0(s->qty), tallybox_total(s->qty));
  }


int quickdist(segment *p1, segment *p2) {
  int d = 0;
  int d1 = p1->left->lev;
  int d2 = p2->left->lev;
  
  while(d1>d2) { p1 = p1->parent; d1--; d++; }
  while(d2>d1) { p2 = p2->parent; d2--; d++; }
  
  return segmentdist(p1, p2, d);
  }

pair<ll, ll> betweenness3(mycell *c, int setid = 0) {

  vector<mycell*> neighbors;
  neighbors = allchildren(c);
  if(c->lev) {
    neighbors.push_back(c->grightsibling());
    neighbors.push_back(c->rightparent);
    if(c->leftparent != c->rightparent)
      neighbors.push_back(c->leftparent);
    neighbors.push_back(c->gleftsibling());
    }
      
  build_ack(c, setid);
  for(auto c1: neighbors) build_ack(c1, setid);
  
  std::unordered_map<segment*, int> info;
  
  for(segment* p: acknowledged) {

    int tot = tallybox_total(p->qty);

    info[p] += tot;
    
    segment *p2 = p->parent;
    while(p2) {
      if(p2->seen != -1) {
        info[p2] -= tot;
        break;
        }
      p2=p2->parent;
      }
    }

  for(segment* p: acknowledged) {
    p->seen = -1;
    }
  
  acknowledged.clear();

  vector<pair<segment*, int> > info_v;
  for(auto p: info) if(p.second) info_v.emplace_back(p);
  
  int NN = isize(neighbors);
  int NN2 = 2 * NN;
  
  vector<ll> bydir(NN2, 0);
  
  int bestdist;  
  vector<int> bestdir;

  vector<segment*> neighbor_segs;
  for(auto n: neighbors) neighbor_segs.push_back(getsegment(n, n, setid, true));
  
  for(auto p: info_v) {
    if(p.first->left == c && p.first->right == c) continue;
    bestdist = 1000;
    bestdir.clear();
    for(int k=0; k<NN; k++) {
      int dist = quickdist(neighbor_segs[k], p.first);
      if(dist < bestdist) bestdist = dist, bestdir.clear();
      if(dist == bestdist) bestdir.push_back(k);
      }    
    if(isize(bestdir) == 1)
      bydir[bestdir[0] * 2] += p.second;
    else if(isize(bestdir) > 2) {
      println(hlog, "many best dirs\n");
      throw hr_exception("many best dirs");
      }
    else if(bestdir[0] + 1 == bestdir[1])
      bydir[bestdir[0] + bestdir[1]] += p.second;
    else if(bestdir[0] == 0 && bestdir[1] == NN-1)
      bydir.back() += p.second;
    else {
      println(hlog, "non-adjacent best dirs\n");
      for(int k=0; k<NN; k++) print(hlog, quickdist(neighbor_segs[k], p.first), " "); printf("\n");
      println(hlog, "bestdir = ", bestdir);
      throw hr_exception("non-adjacent best dirs");
      }
    }
  
  ll result = 0;
  
  int here = getsegment(c, c, setid, true)->qty[0];
  
  ll elsewhere = 0;
  for(int a=0; a<NN2; a++) elsewhere += bydir[a];
  
  result += (here + elsewhere) * (here + elsewhere);

  // for(int a=0; a<NN2; a++) printf("%3d ", bydir[a]); printf("HERE %d EW=%d N=%d\n", here, elsewhere, N);
  
  for(int a=0; a<NN2; a+=2) {
    result -= bydir[a] * bydir[a];
    result -= 2 * bydir[a] * bydir[(a+1) % NN2];
    result -= 2 * bydir[a] * bydir[(a+2) % NN2];
    result -= 2 * bydir[a] * bydir[(a+3) % NN2];
    result -= 2 * bydir[a] * bydir[(a+NN2-1) % NN2];
    result -= 2 * bydir[a] * bydir[(a+NN2-3) % NN2];
    
    result -= bydir[a+1] * bydir[(a+1) % NN2];
    result -= 2 * bydir[a+1] * bydir[(a+3) % NN2];
    result -= 2 * bydir[a+1] * bydir[(a+5) % NN2];
    }
    
  ll result2 = here * here * NN + 2 * here * elsewhere * NN;
  
  for(int a=0; a<NN2; a++) if(bydir[a])
  for(int b=0; b<a; b++) if(bydir[b])
    result2 += 2 * bydir[a] * bydir[b] * min(a-b, NN2+b-a);
  
  return {result, result2};  
  }

bool dependent(segment *p1, segment *p2) {

  if(is_insegment(p1->left, p2) || is_insegment(p1->right, p2) || is_insegment(p2->left, p1) || is_insegment(p2->right, p1))
    return true;

  mycell *mright;
  
  mright = p1->right;
  for(int u=0; u<=cgi.expansion->sibling_limit; u++) {
    mright->build();
    if(mright == p2->left) return true;
    mright = mright->grightsibling();
    }

  mright = p2->right;
  for(int u=0; u<=cgi.expansion->sibling_limit; u++) {
    mright->build();
    if(mright == p1->left) return true;
    mright = mright->grightsibling();
    }
  
  return false;
  }

template<class T> void children(segment *s, const T& f) {
  s=s->firstchild;
  while(s) {
    f(s);
    s = s->nextchild;
    }
  }

typedef long double betweenness_type;

betweenness_type ack(int d01, int d02, int d12) {
  return pow(cgi.expansion->get_growth(), -(d01 + d02 - d12));
  // if(d01 + d02 == d12+2) return 1;
  // return 0;
  }

int sd(segment *s1, segment *s2) {
  /*
  for(auto l = s1->left;; l = l->grightsibling()) {
    if(is_insegment(l, s2)) return 0;
    if(l == s1->right) break;
    }
  
  for(auto l = s2->left;; l = l->grightsibling()) {
    if(is_insegment(l, s1)) return 0;
    if(l == s2->right) break;
    }
  */
  
  return quickdist(s1, s2);
  }

// di0: distance from Hidden to 0
// di0: distance from Hidden to 1
betweenness_type brec_fix_other(int di0, const vector<segment*>& lst, int pos, int di1, segment *s1) {
  // indent_measure im("brec-fo " + its(pos) + " in s1: " + segdesc(s1) + " di0= " + its(di0) + " di1= " + its(di1) );
  segment *s0 = lst[pos];
  if(!dependent(s0, s1) || pos == 0)
    return outNZ(tallybox_total(s1->qty) * ack(di0+pos, sd(s0, s1)+pos, di1));
  
  betweenness_type total = 0;
  if(get0(s1->qty))
    total += get0(s1->qty) * ack(di0+pos, sd(s0,s1)+pos, di1);

  for(segment *c1 = s1->firstchild; c1; c1 = c1->nextchild)
    total += brec_fix_other(di0+1, lst, pos-1, di1+1, c1);
  
  return outNZ(total);
  }

map<tuple<int, segment*, segment* >, betweenness_type> memo[BOXSIZE];

betweenness_type brec_fix_main(int d1, segment *s1, int d2, segment *s2);

betweenness_type brec_fix_main_actual(int d1, segment *s1, int d2, segment *s2) {
  betweenness_type total = 0;
  if(get0(s1->qty))
    total += get0(s1->qty) * tallybox_total(s2->qty) * ack(d1, d2, sd(s1, s2));
  if(get0(s2->qty))
    total += get0(s2->qty) * tallybox_total(s1->qty) * ack(d1, d2, sd(s1, s2));
  if(get0(s1->qty) && get0(s2->qty))
    total -= get0(s1->qty) * get0(s2->qty) * ack(d1, d2, sd(s1, s2));
  
  for(segment *c1 = s1->firstchild; c1; c1 = c1->nextchild)
  for(segment *c2 = s2->firstchild; c2; c2 = c2->nextchild)
    total += brec_fix_main(d1+1, c1, d2+1, c2);
  
  return total;
  }

betweenness_type brec_fix_main(int d1, segment *s1, int d2, segment *s2) {
  // indent_measure im("brec-main " + its(d1) + " in s1: " + segdesc(s1) + " " + its(d2) + " in s2: " + segdesc(s2));
  if(!dependent(s1, s2))
    return outNZ(tallybox_total(s1->qty) * tallybox_total(s2->qty) * ack(d1, d2, sd(s1, s2)));

  // if(s1->left->lev >= 10) return brec_fix_main_actual(d1, s1, d2, s2);

  auto& mem = memo[s1->left->lev][make_tuple(d1+d2, s1, s2)];
  if(mem) return mem-1;

  auto total = brec_fix_main_actual(d1, s1, d2, s2);
  mem = total + 1;
  return total;
  }

betweenness_type brec(const vector<segment*>& lst, int pos, segment *s1, segment *s2) {
  segment *s0 = lst[pos];
  // indent_measure im("brec " + its(pos) + " in s1: " + segdesc(s1) + " in s2: " + segdesc(s2) + " s0 = " + segdesc(s0) );
  bool id01 = !dependent(s0, s1);
  bool id02 = !dependent(s0, s2);
  if(id01 && id02) {
    int di01 = sd(s0, s1);
    int di02 = sd(s0, s2);
    return outNZ(brec_fix_main(di01+pos, s1, di02+pos, s2));
    }
  else {
    bool id12 = (id01 || id02) && !dependent(s1, s2);
    if(id12 && id02) {
      //01 not
      int di12 = sd(s1, s2);
      int di02 = sd(s0, s2);
      return outNZ(tallybox_total(s2->qty) * brec_fix_other(di02, lst, pos, di12, s1));
      }
    else if(id12 && id01) {
      int di12 = sd(s1, s2);
      int di01 = sd(s0, s1);
      return outNZ(tallybox_total(s1->qty) * brec_fix_other(di01, lst, pos, di12, s2));
      }
    else if(pos == 0)
      return outNZ(brec_fix_main(sd(s0, s1), s1, sd(s0, s2), s2));
    else {
      betweenness_type total = 0;
      if(get0(s1->qty))
        total += get0(s1->qty) * brec_fix_other(sd(s0, s1), lst, pos, sd(s1, s2), s2);
      if(get0(s2->qty))
        total += get0(s2->qty) * brec_fix_other(sd(s0, s2), lst, pos, sd(s1, s2), s1);
      if(get0(s1->qty) && get0(s2->qty))
        total -= get0(s1->qty) * get0(s2->qty) * ack(pos + sd(s0, s1), pos + sd(s0, s2), sd(s1, s2));
      for(segment *c1 = s1->firstchild; c1; c1 = c1->nextchild)
      for(segment *c2 = s2->firstchild; c2; c2 = c2->nextchild)
        total += brec(lst, pos-1, c1, c2);
      return outNZ(total);
      }
    }
  }

betweenness_type betweenness4(mycell *c, int setid = 0) {
  segment *s = getsegment(c, c, setid, true);
  vector<segment*> lst;
  while(s) { lst.push_back(s); s = s->parent; }
  segment *rs = getsegment(mroot, mroot, setid, true);
  return brec(lst, isize(lst)-1, rs, rs);
  }

bool neq(betweenness_type a, betweenness_type b) {
  return abs(a-b) > 1e-6;
  }

void compute_betweenness(bool verify) {
  progressbar pb(N, "compute_betweenness");
  int errorcount = 0, errorcount2 = 0;
  for(int i=0; i<N; i++) {
    long long total_cache = 0, x_total_cache = 0;
    int clean_level = -1;
    for(auto& mm: memo) {
      ll s = mm.size();
      if(!s) continue;
      total_cache += s;
      if(total_cache >= 8000000000ll / 64) { mm.clear(); }
      else { x_total_cache += s; clean_level++; }
      }
    if(total_cache != x_total_cache) printf("cleanup from %lld to %lld cache items, at level %d\n", total_cache, x_total_cache, clean_level);
    // if(i != 1) continue;
    mycell *c1 = vertices[i];
    // add_to_set(c1, -1, 0);
    auto b = betweenness3(c1);
    // add_to_set(c1, 1, 0);
    auto b4 = betweenness4(c1);
    print(hlog, format("B;%10Ld;%10Ld;%20.10Lf;%3d;%-40s", b.first, b.second, b4, vertices[i]->lev, rogueviz::vdata[i].name.c_str()));
    if(verify) {
      /*
      betweenness_type a = b.first;
      betweenness_type b = 0;
      for(int j=0; j<N; j++)
      for(int k=0; k<N; k++)
        b += ack(quickdist(vertices[j], c1), quickdist(vertices[k], c1), quickdist(vertices[j], vertices[k]));
        if(quickdist(vertices[j], vertices[k]) == quickdist(vertices[j], c1) + quickdist(vertices[k], c1)) {
          // iprintf("  %-40s %-40s %d/%d/%d\n", rogueviz::vdata[j].name.c_str(), rogueviz::vdata[k].name.c_str(), quickdist(vertices[j], vertices[k]), quickdist(vertices[j], c1), quickdist(vertices[k], c1));
          b++;
          }
      if(neq(a, b)) iprintf(" ERROR: %f\x1b[K\n", double(b));
      else iprintf(" CORRECT %f\x1b[K\n", double(b));
      if(b-a>0) errorcount += (b-a);
      if(b-a<0) errorcount2 += (a-b);
      if(neq(a, b)) exit(1);
      */
      }
    else printf("\n");
    pb++;
    }
  if(verify) println(hlog, format("errorcount = %d/%d\n", errorcount, errorcount2));
  }

void build(mycell *c, int lev, string s) {
  int id = 0;
  if(lev) for(mycell *c1: allchildren(c)) { build(c1, lev-1, s + std::to_string(id)); id++; }
  printf("* %s\n", s.c_str());
  vertices.push_back(c);
  rogueviz::vdata.emplace_back();  
  rogueviz::vdata.back().name = s;
  /*
  vertices.push_back(c);
  rogueviz::vdata.emplace_back();  
  rogueviz::vdata.back().name = s;
  */
  }

void build_all(int d) {
  build(mroot, d, "");
  N = isize(vertices);
  counttallies();
  }

void load_test() {
  string s;
  while(getline(std::cin, s)) {
    mycell *mc = mroot;
    if(s[0] == '#') continue;
    for(char c: s) if(c >= '0' && c <= '9') mc = allchildren(mc) [c - '0'];
    vertices.push_back(mc);
    rogueviz::vdata.emplace_back();
    rogueviz::vdata.back().name = "PATH:" + s;
    }
  // build(mroot, 5, "");
  N = isize(vertices);
  counttallies();
  // add_to_set(vertices[0], -1, 0);
  }

}
