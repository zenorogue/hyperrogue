namespace hr {

namespace synt {

int indent;

struct indenter {
  indenter() { indent += 2; }
  ~indenter() { indent -= 2; }
  };

void doindent() { fflush(stdout); for(int i=0; i<indent; i++) printf(" "); }

bool do_sdebug = false;

#define SDEBUG(x) if(do_sdebug) { doindent(); x; fflush(stdout); }

// Marek-snub
vector<int> faces = {3, 6, 6, 6};
vector<int> adj = {1, 0, 2, 3};
vector<bool> invert = {false, false, true, false};

/*
vector<int> faces = {3, 6, 6, 6};
vector<int> adj = {1, 0, 2, 3};
vector<bool> invert = {false, false, false, false};
*/

/*
vector<int> faces = {7, 6, 6};
vector<int> adj = {1, 0, 2};
vector<bool> invert = {false, false, false};
*/

/*
vector<int> faces = {8, 8, 8};
vector<int> adj = {0, 1, 2};
vector<bool> invert = {false, false, false};
*/

int repetition = 1;
int N;

vector<vector<pair<int, int>>> adjacent;

vector<vector<pair<ld, ld>>> triangles;

// id of vertex in the syntetic tiling
// odd numbers = reflected tiles
// 0, 2, ..., 2(N-1) = as in the symbol
// 2N = bitruncated tile

short& id_of(heptagon *h) {
  return h->zebraval;
  }

// which index in id_of's neighbor list does h->move[0] have

short& parent_index_of(heptagon *h) {
  return h->emeraldval;
  }

// total number of neighbors

int neighbors_of(heptagon *h) {
  return isize(triangles[id_of(h)]);
  }

// right_sibling_of(h) has the same distance ('right sibling'), then we have some at smaller or equal distance,
// parents_of(h) has the same distance again ('left sibling'), and then we have vertices at bigger distance,
// who are our 'children' except the rightmost one which is typically a child of the right sibling

short& right_sibling_of(heptagon *h) {
  return h->fiftyval;
  }

int parents_of(heptagon *h) {
  return h->s;
  }

int children_of(heptagon *h) {
  return right_sibling_of(h) - 1 - parents_of(h);
  }

ld edgelength;
vector<ld> inradius, circumradius, alphas;

void prepare() {

  /* build the 'adjacent' table */

  N = isize(faces);
  adjacent.clear();
  adjacent.resize(2*N+2);
  for(int i=0; i<N; i++) {
    for(int oi=0; oi<1; oi++) {
      int at = (i+oi)%N;
      int inv = oi;
      printf("vertex ");
      for(int z=0; z<faces[i]; z++) {
        printf("[%d %d] " , at, inv);
        adjacent[2*i+oi].emplace_back(2*N+int(inv), inv ? (2*at+2*N-2) % (2*N) : 2*at);
        if(invert[at]) inv ^= 1;
        at = adj[at];
        if(inv) at = (at+1) % N;
        else at = (at+N-1) % N;
        }
      printf("-> [%d %d]\n", at, inv);
      if(faces[at] != faces[i]) printf("error!\n");
      }
    }
  for(int i=0; i<N; i++) {
    adjacent[2*N].emplace_back(2*i, 0);
    int ai = (i+1) % N;
    adjacent[2*N].emplace_back(2*N+int(invert[ai]), (2*adj[ai]+2*N-1) % (2*N));
    }                                               

  for(int i=0; i<2*N+2; i++) {
    printf("prelim adjacent %2d:", i);
    for(int j=0; j<isize(adjacent[i]); j++) {
      auto p = adjacent[i][j];
      printf(" (%d,%d)", p.first, p.second);
      }
    printf("\n");
    }

  for(int d=0; d<=2*N; d+=2) {
    int s = isize(adjacent[d]);
    for(int i=0; i<s; i++) {
      auto& orig = adjacent[d][s-1-i];
      adjacent[d+1].emplace_back(orig.first ^ 1, orig.second);
      }
    }
  for(int d=0; d<2*N+2; d++) {
    int s = isize(adjacent[d]);
    for(int i=0; i<s; i++) {
      auto& orig = adjacent[d][i];
      if(orig.first & 1) orig.second = isize(adjacent[orig.first]) - 1 - orig.second;
      }
    }
  
  for(int i=0; i<2*N+2; i++) {
    printf("adjacent %2d:", i);
    for(int j=0; j<isize(adjacent[i]); j++) {
      auto p = adjacent[i][j];
      printf(" (%d,%d)", p.first, p.second);
      auto q = adjacent[p.first][p.second];
      printf(" <%d,%d>", q.first, q.second);
      if(isize(adjacent[q.first]) != isize(adjacent[i])) printf(" {error}");
      }
    printf("\n");
    }
    
  /* verify all the triangles */
  for(int i=0; i<2*N+2; i++) {
    for(int j=0; j<isize(adjacent[i]); j++) {
      int ai = i, aj = j;
      printf("triangle ");
      for(int s=0; s<3; s++) {
        printf("[%d %d] ", ai, aj); fflush(stdout);
        tie(ai, aj) = adjacent[ai][aj];
        aj++; if(aj >= isize(adjacent[ai])) aj = 0;
        }
      printf("-> [%d %d]\n", ai, aj);
      if(isize(adjacent[ai]) != isize(adjacent[i])) printf("error!\n");
      }
    }
  
  ld sum = 0;
  for(int f: faces) sum += (f-2.) / f;
  if(sum < 1.999999) ginf[gSyntetic].cclass = gcSphere;
  else if(sum > 2.000001) ginf[gSyntetic].cclass = gcHyperbolic;
  else ginf[gSyntetic].cclass = gcEuclid;
  
  printf("sum = %lf\n", double(sum));
  
  dynamicval<eGeometry> dv(geometry, gSyntetic);
  
  /* compute the geometry */
  inradius.resize(N);
  circumradius.resize(N);
  alphas.resize(N);
  ld elmin = 0, elmax = hyperbolic ? 10 : sphere ? M_PI : 1;
  for(int p=0; p<100; p++) {
    edgelength = (elmin + elmax) / 2;
    
    ld alpha_total = 0;

    for(int i=0; i<N; i++) {
      ld crmin = 0, crmax = sphere ? M_PI : 10;
      for(int q=0; q<100; q++) {
        circumradius[i] = (crmin + crmax) / 2;
        hyperpoint p1 = xpush(circumradius[i]) * C0;
        hyperpoint p2 = spin(2 * M_PI / faces[i]) * p1;
        inradius[i] = hdist0(mid(p1, p2));
        if(hdist(p1, p2) > edgelength) crmax = circumradius[i];
        else crmin = circumradius[i];
        }
      hyperpoint h = xpush(edgelength/2) * spin(M_PI/2) * xpush(inradius[i]) * C0;
      alphas[i] = atan2(-h[1], h[0]);
      alpha_total += alphas[i];
      }
    
    // printf("el = %lf alpha = %lf\n", double(edgelength), double(alpha_total));

    if(sphere ^ (alpha_total > M_PI)) elmin = edgelength;
    else elmax = edgelength;
    if(euclid) break;
    }
  
  printf("computed edgelength = %lf\n", double(edgelength));
  
  triangles.clear();
  triangles.resize(2*N+2);
  for(int i=0; i<N; i++) for(int j=0; j<2; j++) 
    for(int k=0; k<faces[i]; k++) 
      triangles[2*i+j].emplace_back(2*M_PI/faces[i], circumradius[i]);
  
  for(int k=0; k<N; k++) {
    triangles[2*N].emplace_back(alphas[k], circumradius[k]);
    triangles[2*N].emplace_back(alphas[(k+1)%N], edgelength);
    triangles[2*N+1].emplace_back(alphas[N-1-k], edgelength);
    triangles[2*N+1].emplace_back(alphas[N-1-k], circumradius[N-1-k]);
    }
  
  for(auto& ts: triangles) {
    ld total = 0;
    for(auto& t: ts) tie(t.first, total) = make_pair(total, total + t.first);
    // printf("total = %lf\n", double(total));
    }

  for(auto& ts: triangles) {
    printf("T");
    for(auto& t: ts) printf(" %lf@%lf", double(t.first), double(t.second));
    printf("\n");
    }
  
  }

void initialize(heptagon *h) {
 
  prepare();

  /* initialize the root */

  parent_index_of(h) = 0;
  id_of(h) = 0;
  h->c7 = newCell(isize(adjacent[0]), h);
  
  if(sphere) celllister cl(h->c7, 1000, 1000000, NULL);

  /* test */
  SDEBUG(
  printf("started testing\n");
  heptagon *htest = h;
  for(int i=0; i<10000; i++) 
    htest = createStep(htest, hrand(neighbors_of(htest))); 
  )
  };

void verify_distance_delta(heptagon *h, int d, int delta) {
  if(!h->move[d]) return;
  if(h->move[d]->distance != h->distance + delta) {
    SDEBUG( printf("ERROR: delta %p.%d (%d/%d)\n", h, d, h->move[d]->distance, h->distance + delta); )
    // exit(1);
    }
  }

void debug(heptagon *h) {
  auto& p = adjacent[id_of(h)];
  if(h->s == hsOrigin) {
    for(int i=0; i<isize(p); i++) verify_distance_delta(h, i, 1);
    }
  else {
    int first = h->s + 1;
    verify_distance_delta(h, 0, -1);
    verify_distance_delta(h, first-2, -1);
    verify_distance_delta(h, first-1, 0);
    verify_distance_delta(h, isize(p)-1, 0);
    for(int d=first; d<isize(p)-1; d++) verify_distance_delta(h, d, 1);
    for(int d=0; d<isize(p); d++) if(h->move[d]) {
      auto& p = adjacent[id_of(h)];
      auto uv = p[(parent_index_of(h) + d) % isize(p)];
      if(neighbors_of(h->move[d]) != isize(adjacent[uv.first])) {
        SDEBUG( printf("neighbors mismatch at %p/%d->%p: is %d expected %d\n", h, d, h->move[d], neighbors_of(h->move[d]), isize(adjacent[uv.first])); )
        exit(1);
        }        
      }
    }
  }

heptagon *build_child(heptagon *parent, int d, int id, int pindex) {
  indenter ind;
  auto h = buildHeptagon(parent, d, hstate(1), 0);
  id_of(h) = id;
  parent_index_of(h) = pindex;
  int nei = neighbors_of(h);
  right_sibling_of(h) = nei - 1;
  h->distance = parent->distance + 1;
  h->c7 = newCell(nei, h);
  SDEBUG( printf("%p.%d/%d ~ %p.0/%d (state=1/NEW,id=%d,pindex=%d,distance=%d)\n", parent, d, neighbors_of(parent), h, neighbors_of(h), id, pindex, h->distance); )
  debug(h); debug(parent);
  return h;
  }

void connectHeptagons(heptagon *h, int i, heptspin hs) {
  indenter ind;
  SDEBUG( printf("%p.%d/%d ~ %p.%d/%d (state=%d,id=%d,pindex=%d,distance=%d)\n", h, i, neighbors_of(h), hs.h, hs.spin, neighbors_of(hs.h), 
    hs.h->s, id_of(hs.h), parent_index_of(hs.h), hs.h->distance); )
  if(h->move[i] == hs.h && h->spin(i) == hs.spin) {
    SDEBUG( printf("WARNING: already connected\n"); )
    return;
    }
  if(h->move[i]) {
    SDEBUG( printf("ERROR: already connected left to: %p not %p\n", h->move[i], hs.h); )
    exit(1);
    }
  if(hs.h->move[hs.spin]) {
    SDEBUG( printf("ERROR: already connected right to: %p not %p\n", hs.h->move[hs.spin], h); )
    exit(1);
    // exit(1);
    }
  hr::connectHeptagons(h, i, hs.h, hs.spin);
  debug(h);
  debug(hs.h);
  }

int prune(heptagon*& h) {
  int result = 1;
  int n = neighbors_of(h);
  auto h0 = h;
  SDEBUG( printf("pruning: %p\n", h0); )
  for(int i=0; i<n; i++) 
    if(h0->move[i]) {
      if(h0->spin(i) == 0)
        result += prune(h0->move[i]);
      else {
        h0->move[i]->move[h0->spin(i)] = NULL;
        h0->move[i] = NULL;
        }
      }
  delete h0->c7;
  delete h0;
  return result;
  }

void contract(heptagon *h) {
  switch(children_of(h)) {
    case 0: {
      SDEBUG( printf("handling contraction (0) at %p\n", h); )
      heptspin right = heptspin(h, right_sibling_of(h)) + wstep + 1;
      heptspin left = heptspin(h, parents_of(h)) + wstep - 1;
      connectHeptagons(right.h, right.spin, left);
      right.h->s++;
      right_sibling_of(left.h)--;
      contract(right.h);
      contract(left.h);
      break;
      }
    case -1: {
      SDEBUG( printf("handling contraction (-1) at %p\n", h); )
      indenter ind2;
      heptspin hs0(h, neighbors_of(h)-1);
      heptspin hs = hs0;
      hs = hs + 1 + wstep + 1;
      while(hs.spin == neighbors_of(hs.h) - 1) {
        SDEBUG( printf("hsr at %p.%d/%d (%d parents)\n", hs.h, hs.spin, neighbors_of(hs.h), parents_of(hs.h)); )
        hs = hs + wstep + 1;
        }
      SDEBUG( printf("hsr at %p.%d/%d (%d parents)\n", hs.h, hs.spin, neighbors_of(hs.h), parents_of(hs.h)); )
      heptspin correct = hs + wstep;
      SDEBUG( printf("correct is: %p.%d/%d (%d parents)\n", correct.h, correct.spin, neighbors_of(correct.h), parents_of(correct.h)); )
      heptspin hsl = hs0;
      correct = correct+1; correct.h->s++;
      connectHeptagons(hsl.h, hsl.spin, correct);
      hsl = hsl - 1 + wstep - 1;
      while(true) {
        SDEBUG( printf("hsl at %p.%d/%d (%d parents)\n", hsl.h, hsl.spin, neighbors_of(hsl.h), parents_of(hsl.h)); )
        if(hsl.spin == parents_of(hsl.h)) {
          SDEBUG(printf("go left\n"))
          hsl = hsl + wstep - 1;
          }
        else if(hsl.h->move[hsl.spin] && hsl.h->move[hsl.spin] != correct.h) {
          SDEBUG(printf("prune\n");)
          if(neighbors_of(hsl.h->move[hsl.spin]) != neighbors_of(correct.h)) {
            SDEBUG(printf("neighbors mismatch while pruning %d -> %d\n",
              neighbors_of(hsl.h->move[hsl.spin]),
              neighbors_of(correct.h)
              );)
            exit(1);
            }
          prune(hsl.h->move[hsl.spin]);
          }
        else if(hsl.h->move[hsl.spin] == NULL) {
          correct = correct+1; correct.h->s++;
          SDEBUG( printf("connect\n") ) 
          connectHeptagons(hsl.h, hsl.spin, correct);
          }
        else if(hsl.spin == parents_of(hsl.h)+1) {
          SDEBUG( printf("single child so go left\n") )
          hsl = hsl - 1 + wstep - 1;
          }
        else { SDEBUG( printf("ready\n"); ) break; }
        }
      contract(correct.h);
      break;
      }
    case -2: {
      SDEBUG( printf("ERROR: contraction (-2) not handled\n"); )
      exit(1);
      break;
      }
    }
  if(!sphere) for(int i=0; i<neighbors_of(h); i++) if(!h->move[i]) {
    auto uv = adjacent[id_of(h)][(parent_index_of(h) + i) % neighbors_of(h)];
    if(isize(adjacent[uv.first]) < 6) {
      SDEBUG( printf("prebuilding weak neighbor\n") )
      createStep(h, i);
      }
    }
  }

void build_siblings(heptagon *h, int x) {
  for(int i=right_sibling_of(h); i<neighbors_of(h); i++) createStep(h, i);
  for(int i=0; i<=parents_of(h); i++) createStep(h, i);
  }

void create_adjacent(heptagon *h, int d) {
  indenter ind;
  int nei = neighbors_of(h);
  
  if(h->s == 0) {
    auto& p = adjacent[id_of(h)];
    for(int i=0; i<nei; i++) 
      build_child(h, i, p[i].first, p[i].second);
    for(int i=0; i<nei; i++) {
      heptagon *h1 = h->move[i];
      heptagon *h2 = h->move[(i+nei-1)%nei];
      connectHeptagons(h1, 1, heptspin(h2, isize(adjacent[id_of(h2)])-1));
      }
    }
  
  else {
    int first = h->s + 1;
    SDEBUG( printf("h=%p dist=%d d=%d/%d s=%d id=%d pindex=%d\n",
      h, h->distance, d, nei, h->s, id_of(h), parent_index_of(h)); )
    indenter ind2;
      
    // these vertices are not children (or possibly contractions)
    if(d < first || d > right_sibling_of(h))
      connectHeptagons(h, d, heptspin(h, d-1) + wstep - 1 + wstep - 1);
    else if(d == right_sibling_of(h)) {
      connectHeptagons(h, d, heptspin(h, 0) + wstep + 1 + wstep + 1);
      }
    else {
      build_siblings(h, 10);
      build_siblings(h, -10);
      if(h->move[d]) return;
      heptspin hs(h, d);
      // make sure no contractions on the left
      heptspin hsl(h, d);
      int steps = 0;
      while(hsl.spin == parents_of(hsl.h) + 1 && steps < 100) {
        hsl = hsl - 1 + wstep - 1;
        steps++;
        }
      if(steps == 100) {
        SDEBUG( printf("generating top\n"); )
        auto uv = adjacent[id_of(hs.h)][(parent_index_of(hs.h) + hs.spin) % neighbors_of(hs.h)];
        heptagon *newchild = build_child(hs.h, hs.spin, uv.first, uv.second);
        hs = hs - 1 + wstep - 1;
        while(hs.h != h) {
          newchild->s++;
          connectHeptagons(hs.h, hs.spin, heptspin(newchild, newchild->s-1));
          hs = hs - 1 + wstep - 1;
          }
        return;
        }
      // while trying to generate the last child, go right
      while(true) {
        if(h->move[d]) {
          SDEBUG( printf("solved itself\n"); )
          return;
          }
        SDEBUG( printf("going right at %p.%d/%d parents = %d\n", hs.h, hs.spin, neighbors_of(hs.h), parents_of(hs.h)); )
        // rightmost child
        if(hs.spin == right_sibling_of(hs.h) - 1)
          hs = hs + 1 + wstep + 1;
        else if(children_of(hs.h) <= 0) {
          SDEBUG( printf("unexpected situation\n"); )
          return;
          }
        else break;
        }
      auto uv = adjacent[id_of(hs.h)][(parent_index_of(hs.h) + hs.spin) % neighbors_of(hs.h)];
      heptagon *newchild = build_child(hs.h, hs.spin, uv.first, uv.second);
      bool add_parent = false;
      while(true) {
        SDEBUG( printf("going left at %p.%d/%d parents = %d\n", hs.h, hs.spin, neighbors_of(hs.h), parents_of(hs.h)); )
        // add parent
        if(hs.spin > parents_of(hs.h) && add_parent) {
          SDEBUG( printf("add parent\n"); )
          newchild->s++;
          connectHeptagons(hs.h, hs.spin, heptspin(newchild, newchild->s-1));
          add_parent = false;
          }
        // childless
        if(children_of(hs.h) <= 0) {
          SDEBUG( printf("unexpected situation v2\n"); )
          return;
          }
        // lefmost child
        else if(hs.spin == parents_of(hs.h)+1) {
          SDEBUG( printf("(leftmost child)\n"); )
          hs = hs - 1 + wstep - 1;
          add_parent = true;
          }
        // no more parents
        else break;
        }
      contract(newchild);
      }
    }
  debug(h);
  }

set<heptagon*> visited;
queue<pair<heptagon*, transmatrix>> drawqueue;

void enqueue(heptagon *h, const transmatrix& T) {
  if(visited.count(h)) { return; }
  visited.insert(h);
  drawqueue.emplace(h, T);
  }

transmatrix adjcell_matrix(heptagon *h, int d) {
  int S = neighbors_of(h);
  int pindex = parent_index_of(h);
  int id = id_of(h);
  auto& t1 = triangles[id][(pindex + d)%S];

  heptagon *h2 = h->move[d];

  int d2 = h->spin(d);
  int id2 = id_of(h2);
  int pindex2 = parent_index_of(h2);
  auto& t2 = triangles[id2][(pindex2 + d2) % neighbors_of(h2)];
  
  // * spin(-tri[id][pi+i].first) * xpush(t.second) * pispin * spin(tri[id'][p'+d'].first)

  return spin(-t1.first) * xpush(t1.second) * spin(M_PI + t2.first);
  }

void draw() {
  visited.clear();
  enqueue(viewctr.h, cview());
  int idx = 0;
  
  while(!drawqueue.empty()) {
    auto p = drawqueue.front();
    drawqueue.pop();
    heptagon *h = p.first;
    transmatrix V = p.second;
    int id = id_of(h);
    int S = isize(triangles[id]);

    if(!nonbitrunc || id < 2*N) {
      if(!dodrawcell(h->c7)) continue;
      drawcell(h->c7, V, 0, false);
      }

    for(int i=0; i<S; i++) {
      if(nonbitrunc && id >= 2*N && h->move[i] && id_of(h->move[i]) >= 2*N) continue;
      enqueue(h->move[i], V * adjcell_matrix(h, i));
      }
    idx++;
    }
  }

transmatrix relative_matrix(heptagon *h2, heptagon *h1) {
  if(gmatrix0.count(h2->c7) && gmatrix0.count(h1->c7))
    return inverse(gmatrix0[h1->c7]) * gmatrix0[h2->c7];
  transmatrix gm = Id, where = Id;
  while(h1 != h2) {
    for(int i=0; i<neighbors_of(h1); i++) if(h1->move[i] == h2) {
      return gm * adjcell_matrix(h1, i) * where;
      }
    else if(h1->distance > h2->distance) {
      gm = gm * adjcell_matrix(h1, 0);
      h1 = h1->move[0];
      }
    else {
      where = inverse(adjcell_matrix(h2, 0)) * where;
      h2 = h2->move[0];
      }
    }
  return gm * where;
  }

int fix(heptagon *h, int spin) {
  int type = isize(adjacent[id_of(h)]);
  spin %= type;
  if(spin < 0) spin += type;
  return spin;
  }

void parse_symbol(string s) {
  int at = 0;
  
  auto peek = [&] () { if(at == isize(s)) return char(0); else return s[at]; };
  auto isnumber = [&] () { char p = peek(); return p >= '0' && p <= '9'; };
  auto read_number = [&] () { int result = 0; while(isnumber()) result = 10 * result + peek() - '0', at++; return result; };
  
  faces.clear();
  while(true) {
    if(peek() == ')' || peek() == '^' || (peek() == '(' && isize(faces)) || peek() == 0) break;
    else if(isnumber()) faces.push_back(read_number());
    else at++;
    }
  repetition = 1;
  N = isize(faces);
  invert.clear(); invert.resize(N, 0);
  adj.clear(); adj.resize(N, 0); for(int i=0; i<N; i++) adj[i] = i;
  while(peek() != 0) {
    if(peek() == '^') at++, repetition = read_number();
    else if(peek() == '(') { 
      at++; int a = read_number(); while(!isnumber() && !among(peek(), '(', '[', ')',']', 0)) at++;
      if(isnumber()) { int b = read_number(); adj[a] = b; adj[b] = a; }
      }
    else if(peek() == '[') { 
      at++; int a = read_number(); while(!isnumber() && !among(peek(), '(', '[', ')',']', 0)) at++;
      if(isnumber()) { int b = read_number(); adj[a] = b; adj[b] = a; invert[a] = invert[b] = true; }
      else { invert[a] = true; }
      }
    else at++;
    }
  prepare();  
  }

#if CAP_COMMANDLINE  
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-symbol")) {
    targetgeometry = gSyntetic;
    if(targetgeometry != geometry)
      stop_game_and_switch_mode(rg::geometry);
    showstartmenu = false;
    shift(); parse_symbol(args());
    }
  else if(argis("-sd")) do_sdebug = true;
  else return 1;
  return 0;
  }
#endif


#if CAP_COMMANDLINE
auto hook = 
  addHook(hooks_args, 100, readArgs); 
#endif

}

}