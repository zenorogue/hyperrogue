// mycell -- information about the given vertex of a triangulation
// cell is the relevant struct from HyperRogue; we do not use cell directly to conserve memory

namespace dhrg {

int mycellcount;

struct segmentlist {
  segment *s;
  segmentlist *next;
  };

struct mycell {
#ifdef BUILD_ON_HR
  cell *c;
#else
  int type;
#endif
  int lev;
  mycell *leftparent, *rightparent;
  mycell *leftsibling, *rightsibling;
  mycell *leftchild;
#ifdef LONG_BRACKETS
  segmentlist* bracketing;
#endif

#ifdef BUILD_ON_HR
  mycell(cell *_c) : c(_c) { 
    leftparent = rightparent = 
    leftsibling = rightsibling = 
    NULL;
#ifdef LONG_BRACKETS
    bracketing = NULL;
#endif
    mycellcount++;
    for(int i=0; i<SETS; i++)
      byleft[i] = byright[i] = NULL;
    }
#else
  mycell() { 
    leftsibling = rightsibling = leftchild = NULL;
    mycellcount++;
    for(int i=0; i<SETS; i++)
      byleft[i] = byright[i] = NULL;
#ifdef LONG_BRACKETS
    bracketing = NULL;
#endif
    }
#endif
  segment *byleft[SETS];
  segment *byright[SETS];
  ~mycell() { mycellcount--; }
#ifdef BUILD_ON_HR
  void build();
  mycell *grightsibling() { return rightsibling; }
  mycell *gleftsibling() { return leftsibling; }
  mycell *gleftchild() { return leftchild; }
  void gchildren() {}
  cell *ascell() { return c; }
  int gettype() { return celltype(c); }
#else
  void build() {}
  mycell *grightsibling();
  mycell *gleftsibling();
  mycell *gleftchild();
  void gchildren();
  cell *ascell();
  int gettype() { return type; }
#endif
  };

#ifdef BUILD_ON_HR
map<cell*, mycell*> mymap;

mycell *find_mycell(cell *c) {
  mycell*& mc = mymap[c];
  if(mc == NULL) mc = new mycell(c);
  return mc;
  }

void mycell::build() {
  const auto m = this;
  if(m->leftsibling) return; // already computed
  cell *c2[MAX_EDGE+1];
  int dist[MAX_EDGE+1];
  int t = m->c->type;
  int d = celldist(m->c);
  m->lev = d;
  if(d == 0) {
    m->leftsibling = m->rightsibling = m;
    m->leftchild = find_mycell(createMov(m->c,0));
    forCellCM(c2, croot()) find_mycell(c2)->build();
    }
  else {    
    for(int i=0; i<t; i++) c2[i] = createMov(m->c, i);
    for(int i=0; i<t; i++) dist[i] = celldist(c2[i]);
    dist[t] = dist[0]; c2[t] = c2[0];
    for(int i=0; i<t; i++) {
      if(dist[i] < d && dist[i+1] == d) {
        m->leftparent = find_mycell(c2[i]);
        m->leftsibling = find_mycell(c2[i+1]);
        m->leftchild = find_mycell(c2[(i+2)%t]);
        }
      if(dist[i] == d && dist[i+1] < d) {
        m->rightparent = find_mycell(c2[i+1]);
        m->rightsibling = find_mycell(c2[i]);
        }
      }
    }
  }
#endif

mycell *mroot;

void generate_root() {
#if BUILD_ON_HR
  mroot = find_mycell(croot());
#else
  int origtype = cgi.expansion->rootid;
  
  mroot = new mycell();
  mroot->lev = 0;
  mroot->type = origtype;
  mroot->leftsibling = mroot->rightsibling = mroot;
  mroot->leftparent = mroot->rightparent = NULL;
  
  mycell *child;
  bool first = true;
  for(int c: cgi.expansion->children[origtype]) {
    if(first) {   
      first = false;
      mroot->leftchild = child = new mycell();
      }
    else {
      child->rightsibling = new mycell();
      child->rightsibling->leftsibling = child;
      child = child->rightsibling;
      }
    
    child->leftparent = child->rightparent = mroot;
    child->type = c;
    child->lev = 1;
    }
  child->rightsibling = mroot->leftchild;
  mroot->leftchild->leftsibling = child;
#endif
  }

#ifndef BUILD_ON_HR

/* mycell *find_mycell(cell *c) {
  printf("find_mycell not implemented\n");
  exit(1);
  } */

mycell* mycell::gleftsibling() {
  if(leftsibling) return leftsibling;
  leftparent->gchildren();
  if(!leftsibling) {
    printf("error: no left sibling found\n");
    exit(1);
    }
  return leftsibling;
  }

mycell* mycell::grightsibling() {
  if(rightsibling) return rightsibling;
  rightparent->gchildren();
  if(!rightsibling) {
    printf("error: no right sibling found\n");
    }
  return rightsibling;
  }

mycell* mycell::gleftchild() {  
  if(leftchild) return leftchild;
  leftchild = new mycell();
  leftchild->leftparent = gleftsibling();
  leftchild->rightparent = this;
  leftchild->lev = lev+1;
  leftchild->type = cgi.expansion->children[type][0];
  return leftchild;
  }

void mycell::gchildren() {
  mycell *child = gleftchild();
  if(child->rightsibling) return;  
  bool first = true;
  for(int c: cgi.expansion->children[type]) {
    if(first) {
      first = false;
      continue;
      }
    child->rightsibling = new mycell();
    child->rightsibling->leftsibling = child;
    child = child->rightsibling;
    child->leftparent = child->rightparent = this;
    child->type = c;
    child->lev = lev + 1;
    }
  child->rightsibling = grightsibling()->gleftchild();
  child->rightsibling->leftsibling = child;
  }
#endif

vector<mycell*> allchildren(mycell *m, int dir=0) {
  m->build();
  vector<mycell*> res;
  if(m->lev == 0) {
    mycell *f = mroot->leftchild;
    int origtype = cgi.expansion->rootid;
    for(int i: cgi.expansion->children[origtype]) {
      ignore(i);
      res.push_back(f);
      f = f->rightsibling;
      }      
    return res;
    }
  auto m1 = m->gleftchild();
  while(true) {
    m1->build();
    bool isright = m1->rightparent == m;
    bool isleft = m1->leftparent == m;
    if(!isright && !isleft) return res;
    if(dir > 0 && !isright) ;
    else if(dir < 0 && !isleft) ;
    else res.push_back(m1);
    m1 = m1->grightsibling();
    }
  }

vector<mycell*> allneighbors(mycell *m) {
  auto ret = allchildren(m);
  if(m->gleftsibling() != m) {
    ret.push_back(m->leftsibling);
    ret.push_back(m->grightsibling());
    }
  if(m->leftparent) {
    ret.push_back(m->leftparent);
    if(m->rightparent != m->leftparent)
      ret.push_back(m->rightparent);
    }
  return ret;
  }

mycell *find_mycell_by_path(const string& s) {
  mycell *at = mroot;
  for(char c: s) {
    at = at->gleftchild();
    while(c > '0') c--, at = at->grightsibling();
    }
  return at;
  }

int childindex(mycell *c) {
  mycell *p = c->rightparent->leftchild;
  int i = 0;
  while(p != c) p = p->grightsibling(), i++;
  return i;
  }

string get_path(mycell *c) {
  string s;
  while(c != mroot) s += '0' + childindex(c), c = c->rightparent;
  reverse(s.begin(), s.end());
  return s;
  }

#ifndef BUILD_ON_HR
cell *mycell::ascell() {
  if(lev == 0) return croot();
  auto m = this; m->build();
  auto c = rightparent->ascell();
  int childid = 0;
  while(m != m->rightparent->leftchild) 
    childid++, m = m->gleftsibling();
  if(lev == 1) return createMov(croot(), childid);
  cell *c2 = ts::child_number(c, childid, celldist);

  return c2;
  }
#endif

}
