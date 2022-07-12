// algorithms to compute distances between mycells, and an implementation of tally counter

namespace dhrg {

int segmentcount;

struct qtybox {
  int minv, maxv;
  int qty[BOXSIZE];
  int& operator [] (int i) { 
    if(i>=BOXSIZE)
      throw hr_exception("not enough memory reserved -- increase BOXSIZE constant");
    return qty[i]; 
    }
  qtybox() { for(int i=0; i<BOXSIZE; i++) qty[i] = 0; minv = BOXSIZE; maxv = 0; }
  };
  
struct segment {
  mycell *left;
  mycell *right;
  segment *nextleft;
  segment *nextright;
  segment *parent;
  segment *nextchild;
  segment *firstchild;
  qtybox qty;
  int seen;
  segment() { seen = -1; segmentcount++; }
  ~segment() { segmentcount--; }
  };

segment *getsegment(mycell *pleft, mycell *pright, int setid, bool docreate = true) {
  pleft->build(); pright->build();
  segment *c1 = pleft->byleft[setid];
  while(c1) {
    if(c1->right == pright) {
      return c1;
      }
    c1 = c1 -> nextleft;
    }
  if(!docreate) return c1;
  segment *p = new segment;
  p->left = pleft;
  p->right = pright;
  p->nextleft = pleft->byleft[setid]; pleft->byleft[setid] = p;
  p->nextright = pright->byright[setid]; pright->byright[setid] = p;
  p->nextchild = NULL; p->firstchild = NULL;
  if(pleft->leftparent) {
    p->parent = getsegment(pleft->leftparent, pright->rightparent, setid);
    p->nextchild = p->parent->firstchild;
    p->parent->firstchild = p;
    }
  else 
    p->parent = NULL;
  #ifdef LONG_BRACKETS
  if(pleft != pright) {
    pleft = pleft->grightsibling();
    int slen = 0;
    while(pleft != pright) {
      segmentlist *n = new segmentlist;
      slen++;
      n->s = p;
      n->next = pleft->bracketing;
      pleft->bracketing = n;
      pleft = pleft->grightsibling();
      }
    if(slen > 10) throw hr_exception("bad bracket");
    }
  #endif
  return p;
  }

ll tally[MAXDIST];

ll *whichtally = tally;

vector<segment*> acknowledged;

void tallybox(qtybox& box, int d, int mul) {
  for(int i=box.minv; i<box.maxv; i++)
    whichtally[i+d] += box[i] * mul;
  }

void acknowledge(segment *p, int d) {
  if(!p) return;
  if(p->seen == -1) {
    p->seen = d;
    acknowledged.emplace_back(p);
    }
  else if(p->seen > d) 
    p->seen = d;
  }

void acknowledgments(int mul) {
  for(segment* p: acknowledged) {
    tallybox(p->qty, p->seen, mul);
    segment *p2 = p->parent;
    int dist = 1;
    while(p2) {
      if(p2->seen != -1) {
        tallybox(p->qty, p2->seen+dist, -mul);
        break;
        }
      p2=p2->parent; dist++;
      }
    p->seen = -1;
    }
  acknowledged.clear();
  }

void trim(qtybox& b) {
  while(b.minv < b.maxv && !b[b.minv]) b.minv++;
  if(b.minv == b.maxv) b.minv = BOXSIZE, b.maxv = 0;
  else while(!b[b.maxv-1]) b.maxv--;
  }

void add_to_set(mycell *c, int mul, int setid) {
  // assume mul does not equal 0 !
  segment *p = getsegment(c, c, setid);
  int d = 0;
  while(p) {    
    p->qty[d] += mul;
    if(p->qty[d]) {
      p->qty.minv = min(p->qty.minv, d);
      p->qty.maxv = max(p->qty.maxv, d+1);
      }
    else trim(p->qty);
    d++; p = p->parent;
    }
  }

int segmentdist(segment *p1, segment *p2, int d);

void build_ack(mycell *c, int setid) {
  segment *p = getsegment(c, c, setid);
  int d = 0;
  
  #ifdef LONG_BRACKETS
  segmentlist *brackets = c->bracketing;
  while(brackets) {
    acknowledge(brackets->s, 0);
    brackets = brackets->next;
    }
  #else
  acknowledge(getsegment(c->gleftsibling(), c->grightsibling(), setid, false), 0); // larger!
  #endif

  while(p) {
    if(p->left != p->right) {
      mycell *cc = p->left->grightsibling();
      while(cc != p->right) {
        acknowledge(getsegment(cc, cc, setid, false), d);
        cc = cc->grightsibling();
        }
      }
    
    int sl = cgi.expansion->sibling_limit;

    mycell *mright = p->right;    
    mycell *mright0 = mright;
    for(int u=0; u<=sl; u++) {
      mright->build();
      segment *mrightbyleft = mright->byleft[setid];
      while(mrightbyleft) {
        if(u == 3 && p->right->rightparent == mright->leftparent)
          acknowledge(mrightbyleft, d+2);
        else if(u > 3)
          acknowledge(mrightbyleft, segmentdist(getsegment(mright0, mright0, setid), mrightbyleft, d));
        else 
          acknowledge(mrightbyleft, d+u);
        mrightbyleft = mrightbyleft->nextleft;
        }
      mright=mright->grightsibling();
      }
    
    mycell *mleft = p->left;
    mycell *mleft0 = mleft;
    for(int u=0; u<=sl; u++) {
      mleft->build();
      segment *mleftbyright = mleft->byright[setid];
      while(mleftbyright) {
        if(u == 3 && p->left -> leftparent == mleft->rightparent)
          acknowledge(mleftbyright, d+2);
        else if(u > 3)
          acknowledge(mleftbyright, segmentdist(mleftbyright, getsegment(mleft0, mleft0, setid), d));
        else 
          acknowledge(mleftbyright, d+u);
        mleftbyright = mleftbyright->nextright;
        }
      mleft=mleft->gleftsibling();
      }
 
    // go forth
    d++; p = p->parent;
    }
  }

void add_to_tally(mycell *c, int mul, int setid) {
  build_ack(c, setid);
  acknowledgments(mul);
  }

bool is_insegment(mycell *c, segment *p) {
  mycell *p1 = p->left;
  mycell *p2 = p->right;
  while(true) {
    if(c == p1) return true;
    if(p1 == p2) return false;
    p1 = p1->grightsibling();
    }
  }

int segmentdist(segment *p1, segment *p2, int d) {

  if(p1->left == p1->right && is_insegment(p1->left, p2)) return d;
  if(p2->left == p2->right && is_insegment(p2->left, p1)) return d;
  
  int best = 1000000;
  
  while(true) {
    if(d >= best) return best;
    mycell *mright;
    
    int sl = cgi.expansion->sibling_limit;
    
    mright = p1->right;
    for(int u=0; u<=sl; u++) {
      mright->build();
      if(mright == p2->left) best = min(best, d+u);
      mright = mright->grightsibling();
      }

    mright = p2->right;
    for(int u=0; u<=sl; u++) {
      mright->build();
      if(mright == p1->left) best = min(best, d+u);
      mright = mright->grightsibling();
      }
    
    p1 = p1->parent;
    p2 = p2->parent;
    d += 2;
    }
  }

int quickdist(mycell *c1, mycell *c2, int setid=0) {
  int d = 0;
  c1->build();
  c2->build();
  int d1 = c1->lev;
  int d2 = c2->lev;
  segment *p1 = getsegment(c1, c1, setid);
  segment *p2 = getsegment(c2, c2, setid);
  
  while(d1>d2) { p1 = p1->parent; d1--; d++; }
  while(d2>d1) { p2 = p2->parent; d2--; d++; }
  
  return segmentdist(p1, p2, d);
  }
  
}
