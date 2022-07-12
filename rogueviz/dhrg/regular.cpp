// find the value of D(G) algorithmically (see the paper)

namespace dhrg {

// c2-c1

int cycle_minus(cell *c2, cell *c1) {
  int acc = 0;
  while(c1 != c2) c1 = ts::right_of(c1, celldist), acc++;
  return acc;
  }

// c2 to the right from c1
int unlimited_distance(cell *c2, cell *c1) {
  int at_least = cycle_minus(c2, c1);
  int steps = 0;
  while(true) {
    steps += 2;
    if(steps >= at_least) return at_least;
    c1 = ts::right_parent(c1, celldist);
    c2 = ts::left_parent(c2, celldist);
    int ndist = steps + cycle_minus(c2, c1);
    if(ndist < at_least) at_least = ndist;
    }
  }

int gettypeof(cell *c) { return type_in_reduced(*(cgi.expansion), c, celldist); }

vector<bool> grow_forever;

set<vector<int>> checked;

bool err = false;

int my_sibling_limit;

void find_sibling_limit(cell *c2, cell *c1) {
  if(err) return;
  if(celldist(c2) != celldist(c1)) {
    printf("not the same ring %d/%d\n", celldist(c1), celldist(c2));
    c1->item = itSilver;
    c2->item = itGold;
    err = true;
    return;
    }
  vector<int> signature;
  cell *cx = c1;
  cell *cy = c1;
  bool gf = false;
  while(cx != c2) { 
    int t = gettypeof(cx);
    if(cx != c1 && grow_forever[t]) gf = true;
    signature.push_back(t); cy = cx; cx = ts::right_of(cx, celldist);
    }
  signature.push_back(gettypeof(cx)); signature.push_back(unlimited_distance(cy, c1) - unlimited_distance(c2, c1));
  if(checked.count(signature)) return;
  checked.insert(signature);
  // for(int v: signature) printf("%d ", v); 
  int cm = cycle_minus(c2, c1);
  int ud = c1 == c2 ? -1 : 2 + unlimited_distance(ts::left_parent(c2, celldist), ts::right_parent(c1, celldist));
  // printf(": %d/%d {%p/%p} [%d]\n", cm, ud, c1, c2, my_sibling_limit);
  if(cm < ud && cm > my_sibling_limit) { my_sibling_limit = cm; }
  if(gf) return;
  int t1 = gettypeof(c1);
  int t2 = gettypeof(c2);
  for(int i1=0; i1<isize(cgi.expansion->children[t1]); i1++) 
  for(int i2=0; i2<isize(cgi.expansion->children[t2]); i2++) 
    if(c1 != c2 || i1 <= i2+1)
      find_sibling_limit(ts::child_number(c2, i2+1, celldist), ts::child_number(c1, i1, celldist));
  }

void correct_sibling_limit() {
  my_sibling_limit = 0;
  if(S3 < 4) {
    grow_forever.clear();
    grow_forever.resize(cgi.expansion->N, true);
    
    while(true) {
      bool changed = false;
      for(int i=0; i<cgi.expansion->N; i++) if(grow_forever[i]) {
        grow_forever[i] = false;
        if(isize(cgi.expansion->children[i]) == 0)
          throw hr_exception("Error: our algorithm does not work if some vertices have no tree children");
        if(isize(cgi.expansion->children[i]) > 1) 
          for(int c: cgi.expansion->children[i])
            if(grow_forever[c] || c == i) grow_forever[i] = true;
        if(!grow_forever[i]) changed = true;
        }
      if(!changed) break;
      }
     
    print(hlog, "The following grow forever:"); for(int i=0; i<cgi.expansion->N; i++) if(grow_forever[i]) print(hlog, " ", i); println(hlog);
  
    cell *root = currentmap->gamestart();
    my_sibling_limit = 0;
    forCellCM(c1, root) forCellCM(c2, root) find_sibling_limit(c2, c1);        
    }
  println(hlog, "The correct value of sibling_limit is ", my_sibling_limit);
  cgi.expansion->sibling_limit = my_sibling_limit;
  }

void regular_info() {
  indenter_finish im("regular_info");

  cgi.expansion->get_descendants(0);
  println(hlog, "growth = ", cgi.expansion->get_growth());
  
  // int typecount = expansion.N;
  
  correct_sibling_limit();
  }

}
