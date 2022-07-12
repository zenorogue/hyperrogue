// computing pairs of vertices in each distance using dynamic programming (as described in the paper)

namespace dhrg {

bool segmentValid(mycell *cl, mycell *cr) {
  if(cl == cr) return true;
  mycell *c1 = cl; 
  int d = 0;
  for(; d<7 && c1 != cr; d++) {
    c1->build(); c1 = c1->grightsibling();
    }
  if(d == 7) return false;
  cr->build();
  cl = cl->grightsibling()->gleftchild();
  cr = cr->gleftchild();
  return segmentValid(cl, cr);
  }

vector<segment*> all_child_segments(segment *s) {
  vector<segment*> res;
  for(auto m1: allchildren(s->left, -1))
  for(auto m2: allchildren(s->right, +1))
    if(segmentValid(m1, m2))
      res.push_back(getsegment(m1,m2,0,true));
  return res;
  }

// returns 0 if not in segment, 1-based index if in segment
int insegment(mycell *mc, segment *s1) {
  mycell *l = s1->left;
  int i = 1;
  while(true) {
    if(l == mc) return i;
    if(l == s1->right) return 0;
    l->build(); l = l->grightsibling();
    i++;
    }
  }

int segmentcode(segment *s) {
  mycell *l = s->left;
  int code = 0;
  while(true) {
    code += l->gettype();
    if(l == s->right) return code;
    l->build(); l = l->grightsibling();
    code *= 8;
    }
  }

int compute_descendants(segment *s, int d) {

  auto memokey = make_tuple(segmentcode(s), d);
  static map<decltype(memokey), int> mem;
  if(mem.count(memokey)) return mem[memokey];

  if(d == 0) return s->left == s->right ? 1 : 0;
  int total = 0;
  for(auto s1: all_child_segments(s))
    total += compute_descendants(s1, d-1);
  return mem[memokey] = total;
  }

// returns 0 if segments are not crossing, positive number if crossing
// for segments with equal codes, equal numbers = the same way of crossing
int segmentcross(segment *s1, segment *s2) {
  int i1;
  i1 = insegment(s1->left, s2);
  if(i1) return 4*i1+1;
  i1 = insegment(s1->right, s2);
  if(i1) return 4*i1+2;
  i1 = insegment(s2->left, s1);
  if(i1) return 4*i1+3;
  i1 = insegment(s2->right, s1);
  if(i1) return 4*i1+4;
  return 0;
  }

set<int> allsegments;

int compute_in_dist(segment *s1, segment *s2, int d1, int d2, int dex) {
  // if(d1 + d2 + 4 < dex) return 0;
  int d = -segmentcross(s1,s2);
  if(!d) d = segmentdist(s1, s2, 0);

  if(d > 2 || d1 == 0 || d2 == 0) {
    if(d < 0) d = 0;
    return d1+d2+d == dex ? compute_descendants(s1,d1) * compute_descendants(s2,d2) : 0;
    }
  else {
    mycell *ss1 = s1->right, *ss2 = s2->right;
    if(d > 0) {
      int side = 0;
      while(true) {
        ss1->build(); ss1 = ss1->grightsibling(); if(ss1 == s2->left) { side=1; break; }
        ss2->build(); ss2 = ss2->grightsibling(); if(ss2 == s1->left) { side=2; break; }
        }
      d += 100 * side;
      }
    allsegments.insert(segmentcode(s1));
    allsegments.insert(segmentcode(s2));
    auto memokey = make_tuple(segmentcode(s1), segmentcode(s2), d, d1, d2, dex);
    static map<decltype(memokey), int> mem;
    if(mem.count(memokey)) return mem[memokey];
    int total = 0;
    for(auto s3: all_child_segments(s1))
    for(auto s4: all_child_segments(s2))
      total += compute_in_dist(s3, s4, d1-1, d2-1, dex);
    if(0) if(mem.count(memokey) && mem[memokey] != total) {
      printf("%d vs %d :: %x %x d=%d %d,%d,%d\n", mem[memokey], total, segmentcode(s1), segmentcode(s2), d, d1, d2, dex);
      return mem[memokey];
      }
    return mem[memokey] = total;
    }
  }

void do_analyze_dists(int rad) {

  println(hlog, "do_analyze_dists (", rad, ")");
  indenter_finish indf;
  auto m = mroot;
  
  auto seg = getsegment(m, m, 0, true);

  // compute the correct answer, but not if this requires creating more than 1500 cells
  celllister cl(croot(), rad, 1500, NULL);
  vector<int> correct(2*rad+4, 0);
  
  for(cell *c1: cl.lst) if(celldist(c1) == rad)
  for(cell *c2: cl.lst) if(celldist(c2) == rad)
    correct[celldistance(c1,c2)]++;
    
  int total = 0;
  for(int a=0; a<2*rad+4; a++) {
    int cd = compute_in_dist(seg, seg, rad, rad, a);
    printf("%2d: %d/%d\n", a, cd, correct[a]);
    total += cd;
    }
  
  printf("total = %d (%d)\n", total, cgi.expansion->get_descendants(5).approx_int() * cgi.expansion->get_descendants(5).approx_int());
  printf("all segments = %d\n", isize(allsegments));
  
  // printf("descendants = %d (%d)\n", compute_descendants(seg, 5), int(.1+expansion.get_descendants(5).approx_int()));
  }

}
