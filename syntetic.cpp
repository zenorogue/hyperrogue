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

map<heptagon*, int> create_order;

map<heptagon*, transmatrix> syntetic_gmatrix;

int nextorder = 1;

static const int PRUNED = 100;

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

extern void draw_debug_map(heptagon *h);
void draw_debug_map_exit(heptagon *h) { draw_debug_map(h); exit(1); }

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
 
  /* initialize the root */

  parent_index_of(h) = 0;
  id_of(h) = 0;
  h->c7 = newCell(isize(adjacent[0]), h);
  
  if(!hyperbolic) syntetic_gmatrix[h] = Id;
  
  if(sphere) celllister cl(h->c7, 1000, 1000000, NULL);
  };

void verify_distance_delta(heptagon *h, int d, int delta) {
  if(!h->move(d)) return;
  if(h->move(d)->distance != h->distance + delta) {
    SDEBUG( printf("ERROR: delta H%d.%d (%d/%d)\n", create_order[h], d, h->move(d)->distance, h->distance + delta); )
    // exit(1);
    }
  }

void debug(heptagon *h) {
  if(id_of(h) == PRUNED) return;
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
    for(int d=0; d<isize(p); d++) if(h->move(d)) {
      auto& p = adjacent[id_of(h)];
      auto uv = p[(parent_index_of(h) + d) % isize(p)];
      if(neighbors_of(h->move(d)) != isize(adjacent[uv.first])) {
        SDEBUG( printf("neighbors mismatch at H%d.%d->H%d: is %d expected %d\n", create_order[h], d, create_order[h->move(d)], neighbors_of(h->move(d)), isize(adjacent[uv.first])); )
        draw_debug_map_exit(h);
        }
      }
    }
  }

transmatrix adjcell_matrix(heptagon *h, int d);

heptagon *build_child(heptagon *parent, int d, int id, int pindex) {
  indenter ind;
  auto h = buildHeptagon1(new heptagon, parent, d, hstate(1), 0);
  create_order[h] = nextorder++;
  id_of(h) = id;
  parent_index_of(h) = pindex;
  int nei = neighbors_of(h);
  right_sibling_of(h) = nei - 1;
  h->distance = parent->distance + 1;
  h->c7 = newCell(nei, h);
  SDEBUG( printf("H%d.%d/%d ~ H%d.0/%d (state=1/NEW,id=%d,pindex=%d,distance=%d)\n", create_order[parent], d, neighbors_of(parent), create_order[h], neighbors_of(h), id, pindex, h->distance); )
  if(!hyperbolic) syntetic_gmatrix[h] = syntetic_gmatrix[parent] * adjcell_matrix(parent, d);
  else { debug(h); debug(parent); }  
  return h;
  }

void connectHeptagons(heptagon *h, int i, heptspin hs) {
  if(id_of(h) == PRUNED) { h->move(i) = h; return; }
  if(id_of(hs.at) == PRUNED) { hs.at->move(i) = hs.at; return; }
  indenter ind;
  SDEBUG( printf("H%d.%d/%d ~ H%d.%d/%d (state=%d,id=%d,pindex=%d,distance=%d)\n", create_order[h], i, neighbors_of(h), create_order[hs.at], hs.spin, neighbors_of(hs.at), 
    hs.at->s, id_of(hs.at), parent_index_of(hs.at), hs.at->distance); )
  if(h->move(i) == hs.at && h->c.spin(i) == hs.spin) {
    SDEBUG( printf("WARNING: already connected\n"); )
    return;
    }
  if(h->move(i)) {
    SDEBUG( printf("ERROR: already connected left to: H%d not H%d\n", create_order[h->move(i)], create_order[hs.at]); )
    draw_debug_map_exit(h);
    }
  if(hs.peek()) {
    SDEBUG( printf("ERROR: already connected right to: H%d not H%d\n", create_order[hs.peek()], create_order[h]); )
    draw_debug_map_exit(h);
    // exit(1);
    }
  h->c.connect(i, hs);
  if(hyperbolic) {
    debug(h);
    debug(hs.at);
    }
  }

int prune(heptagon*& h) {
  if(!h) return 0;
  int result = 1;
  int n = neighbors_of(h);
  auto h0 = h;
  SDEBUG( printf("pruning: H%d\n", create_order[h0]); )
  for(int i=0; i<n; i++) 
    if(h0->move(i)) {
      if(h0->c.spin(i) == 0)
        result += prune(h0->move(i));
      else {
        h0->move(i)->move(h0->c.spin(i)) = NULL;
        h0->move(i) = NULL;
        }
      }
  id_of(h0) = PRUNED;
  /*
  delete h0->c7;
  delete h0;
  */
  return result;
  }

void mayprune(heptagon *hleft, heptagon *hright) {
  if(children_of(hleft) >= 1 && children_of(hright) >= 1)
  if(hleft->move(right_sibling_of(hleft) - 1) != hright->move(parents_of(hright)+1)) {
    SDEBUG( printf("pruning extra children after contraction\n"); )
    prune(hleft->move(right_sibling_of(hleft) - 1));
    prune(hright->move(parents_of(hright)+1));
    }
  }

void contract(heptagon *h) {
  if(id_of(h) == PRUNED) return;
  switch(children_of(h)) {
    case 0: {
      SDEBUG( printf("handling contraction (0) at H%d\n", create_order[h]); )
      heptspin right = heptspin(h, right_sibling_of(h)) + wstep + 1;
      heptspin left = heptspin(h, parents_of(h)) + wstep - 1;
      connectHeptagons(right.at, right.spin, left);
      right.at->s++;
      right_sibling_of(left.at)--;
      mayprune(left.at, right.at);
      contract(right.at);
      contract(left.at);
      break;
      }
    case -1: {
      SDEBUG( printf("handling contraction (-1) at H%d\n", create_order[h]); )
      indenter ind2;
      heptspin hs0(h, neighbors_of(h)-1);
      heptspin hs = hs0;
      hs = hs + 1 + wstep + 1;
      while(hs.spin == neighbors_of(hs.at) - 1) {
        SDEBUG( printf("hsr at H%d.%d/%d (%d parents)\n", create_order[hs.at], hs.spin, neighbors_of(hs.at), parents_of(hs.at)); )
        hs = hs + wstep + 1;
        }
      SDEBUG( printf("hsr at H%d.%d/%d (%d parents)\n", create_order[hs.at], hs.spin, neighbors_of(hs.at), parents_of(hs.at)); )
      heptspin correct = hs + wstep;
      SDEBUG( printf("correct is: H%d.%d/%d (%d parents)\n", create_order[correct.at], correct.spin, neighbors_of(correct.at), parents_of(correct.at)); )
      heptspin hsl = hs0;
      correct = correct+1; correct.at->s++;
      connectHeptagons(hsl.at, hsl.spin, correct);
      hsl = hsl - 1 + wstep - 1;
      while(true) {
        SDEBUG( printf("hsl at %d.%d/%d (%d parents)\n", create_order[hsl.at], hsl.spin, neighbors_of(hsl.at), parents_of(hsl.at)); )
        if(hsl.spin == parents_of(hsl.at)) {
          SDEBUG(printf("go left\n"))
          hsl = hsl + wstep - 1;
          }
        else if(hsl.peek() && hsl.peek() != correct.at) {
          SDEBUG(printf("prune\n");)
          if(neighbors_of(hsl.peek()) != neighbors_of(correct.at)) {
            SDEBUG(printf("neighbors mismatch while pruning %d -> %d\n",
              neighbors_of(hsl.peek()),
              neighbors_of(correct.at)
              );)
            draw_debug_map_exit(correct.at);
            }
          prune(hsl.peek());
          }
        else if(hsl.peek() == NULL) {
          correct = correct+1; correct.at->s++;
          SDEBUG( printf("connect\n") ) 
          connectHeptagons(hsl.at, hsl.spin, correct);
          }
        else if(hsl.spin == parents_of(hsl.at)+1) {
          SDEBUG( printf("single child so go left\n") )
          hsl = hsl - 1 + wstep - 1;
          }
        else { SDEBUG( printf("ready\n"); ) break; }
        }
      contract(correct.at);
      break;
      }
    case -2: {
      SDEBUG( printf("ERROR: contraction (-2) not handled\n"); )
      break;
      }
    }
  if(!sphere) for(int i=0; i<neighbors_of(h); i++) if(!h->move(i)) {
    auto uv = adjacent[id_of(h)][(parent_index_of(h) + i) % neighbors_of(h)];
    if(isize(adjacent[uv.first]) < 5 && hyperbolic) {
      SDEBUG( printf("prebuilding weak neighbor\n") )
      createStep(h, i);
      }
    }
  }

void build_siblings(heptagon *h, int x) {
  for(int i=right_sibling_of(h); i<neighbors_of(h); i++) createStep(h, i);
  for(int i=0; i<=parents_of(h); i++) createStep(h, i);
  }

pair<int, int>& get_adj(heptagon *h, int cid);
pair<ld, ld>& get_triangle(heptagon *h, int cid);

void create_adjacent(heptagon *h, int d) {

  if(!hyperbolic) {

    SDEBUG( printf("h=%d dist=%d d=%d/%d s=%d id=%d pindex=%d\n",
      create_order[h], h->distance, d, neighbors_of(h), h->s, id_of(h), parent_index_of(h)); )
    indenter ind2;

    auto& t1 = get_triangle(h, d);
  
    // * spin(-tri[id][pi+i].first) * xpush(t.second) * pispin * spin(tri[id'][p'+d'].first)
  
    transmatrix T = syntetic_gmatrix[h] * spin(-t1.first) * xpush(t1.second);

    for(auto gm: syntetic_gmatrix) if(intval(gm.second * C0, T * C0) < 1e-6) {
      SDEBUG( printf("cell found\n"); )
      for(int d2=0; d2<gm.first->c7->type; d2++) {
        auto& t2 = get_triangle(gm.first, d2);
        transmatrix T1 = T * spin(M_PI + t2.first);
        if(intval(T1 * xpush(1) * C0, gm.second * xpush(1) * C0) < 1e-6) {
          connectHeptagons(h, d, heptspin(gm.first, d2));
          return;
          }
        }
      SDEBUG( printf("but rotation not found\n"));
      }
    build_child(h, d, get_adj(h, d).first, get_adj(h, d).second);
    return;
    }

  if(id_of(h) == PRUNED) { h->move(d) = h; return; }
  if(indent >= 200) draw_debug_map_exit(h);
  indenter ind;
  int nei = neighbors_of(h);
  
  if(h->s == 0) {
    auto& p = adjacent[id_of(h)];
    for(int i=0; i<nei; i++) 
      build_child(h, i, p[i].first, p[i].second);
    for(int i=0; i<nei; i++) {
      heptagon *h1 = h->move(i);
      heptagon *h2 = h->move((i+nei-1)%nei);
      connectHeptagons(h1, 1, heptspin(h2, isize(adjacent[id_of(h2)])-1));
      }
    }
  
  else {
    int first = h->s + 1;
    SDEBUG( printf("h=%d dist=%d d=%d/%d s=%d id=%d pindex=%d\n",
      create_order[h], h->distance, d, nei, h->s, id_of(h), parent_index_of(h)); )
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
      if(h->move(d)) return;
      heptspin hs(h, d);
      // make sure no contractions on the left
      heptspin hsl(h, d);
      int steps = 0;
      while(hsl.spin == parents_of(hsl.at) + 1 && steps < 100) {
        hsl = hsl - 1 + wstep - 1;
        steps++;
        }
      if(steps == 100) {
        SDEBUG( printf("generating top\n"); )
        auto uv = adjacent[id_of(hs.at)][(parent_index_of(hs.at) + hs.spin) % neighbors_of(hs.at)];
        heptagon *newchild = build_child(hs.at, hs.spin, uv.first, uv.second);
        hs = hs - 1 + wstep - 1;
        while(hs.at != h) {
          newchild->s++;
          connectHeptagons(hs.at, hs.spin, heptspin(newchild, newchild->s-1));
          hs = hs - 1 + wstep - 1;
          }
        return;
        }
      // while trying to generate the last child, go right
      while(true) {
        if(h->move(d)) {
          SDEBUG( printf("solved itself\n"); )
          return;
          }
        SDEBUG( printf("going right at H%d.%d/%d parents = %d\n", create_order[hs.at], hs.spin, neighbors_of(hs.at), parents_of(hs.at)); )
        if(id_of(hs.at) == PRUNED) { create_adjacent(h, d); return; }
        // rightmost child
        if(hs.spin == right_sibling_of(hs.at) - 1)
          hs = hs + 1 + wstep + 1;
        else if(children_of(hs.at) <= 0) {
          SDEBUG( printf("unexpected situation\n"); )
          create_adjacent(h, d);
          return;
          }
        else break;
        }
      auto uv = adjacent[id_of(hs.at)][(parent_index_of(hs.at) + hs.spin) % neighbors_of(hs.at)];
      heptagon *newchild = build_child(hs.at, hs.spin, uv.first, uv.second);
      bool add_parent = false;
      while(true) {
        if(id_of(hs.at) == PRUNED) { create_adjacent(h, d); return; }
        SDEBUG( printf("going left at H%d.%d/%d parents = %d\n", create_order[hs.at], hs.spin, neighbors_of(hs.at), parents_of(hs.at)); )
        // add parent
        if(hs.spin > parents_of(hs.at) && add_parent) {
          SDEBUG( printf("add parent\n"); )
          newchild->s++;
          connectHeptagons(hs.at, hs.spin, heptspin(newchild, newchild->s-1));
          add_parent = false;
          }
        // childless
        if(children_of(hs.at) <= 0) {
          SDEBUG( printf("unexpected situation v2\n"); )
          create_adjacent(h, d);
          return;
          }
        // lefmost child
        else if(hs.spin == parents_of(hs.at)+1) {
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

pair<ld, ld>& get_triangle(heptagon *h, int cid) {
  return triangles[id_of(h)][(parent_index_of(h) + cid) % neighbors_of(h)];
  }

pair<int, int>& get_adj(heptagon *h, int cid) {
  return adjacent[id_of(h)][(parent_index_of(h) + cid) % neighbors_of(h)];
  }

pair<ld, ld>& get_triangle(const pair<int, int>& p) {
  return triangles[p.first][p.second];
  }

pair<int, int>& get_adj(const pair<int, int>& p, int delta = 0) {
  return adjacent[p.first][(p.second + delta) % isize(adjacent[p.first])];
  }

transmatrix adjcell_matrix(heptagon *h, int d) {
  auto& t1 = get_triangle(h, d);

  heptagon *h2 = h->move(d);

  int d2 = h->c.spin(d);
  auto& t2 = get_triangle(h2, d2);
  
  // * spin(-tri[id][pi+i].first) * xpush(t.second) * pispin * spin(tri[id'][p'+d'].first)

  return spin(-t1.first) * xpush(t1.second) * spin(M_PI + t2.first);
  }

void draw() {
  visited.clear();
  enqueue(viewctr.at, cview());
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
      h->cmove(i);
      if(nonbitrunc && id >= 2*N && h->move(i) && id_of(h->move(i)) >= 2*N) continue;
      enqueue(h->move(i), V * adjcell_matrix(h, i));
      }
    idx++;
    }
  }

transmatrix relative_matrix(heptagon *h2, heptagon *h1) {
  if(gmatrix0.count(h2->c7) && gmatrix0.count(h1->c7))
    return inverse(gmatrix0[h1->c7]) * gmatrix0[h2->c7];
  transmatrix gm = Id, where = Id;
  while(h1 != h2) {
    for(int i=0; i<neighbors_of(h1); i++) if(h1->move(i) == h2) {
      return gm * adjcell_matrix(h1, i) * where;
      }
    else if(h1->distance > h2->distance) {
      gm = gm * adjcell_matrix(h1, 0);
      h1 = h1->move(0);
      }
    else {
      where = inverse(adjcell_matrix(h2, 0)) * where;
      h2 = h2->move(0);
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
  invert.clear(); invert.resize(N, true);
  adj.clear(); adj.resize(N, 0); for(int i=0; i<N; i++) adj[i] = i;
  while(peek() != 0) {
    if(peek() == '^') at++, repetition = read_number();
    else if(peek() == '(') { 
      at++; int a = read_number(); while(!isnumber() && !among(peek(), '(', '[', ')',']', 0)) at++;
      if(isnumber()) { int b = read_number(); adj[a] = b; adj[b] = a; invert[a] = invert[b] = false; }
      else { invert[a] = false; }
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
  else if(argis("-sdeb")) { PHASE(3); draw_debug_map(cwt.at->master); }
  else return 1;
  return 0;
  }
#endif


map<heptagon*, transmatrix> debugmap;

void add_to_debug(heptagon *h, transmatrix T) {
  debugmap[h] = T;
  for(int i=0; i<neighbors_of(h); i++)
    if(h->move(i) && h->c.spin(i) == 0 && h->move(i)->s != hsOrigin)
      add_to_debug(h->move(i), T * adjcell_matrix(h, i));
  }

void draw_debug_map(heptagon *h) {
  debugmap.clear();
  while(h->s != hsOrigin) h = h->move(0);
  add_to_debug(h, Id);
  svg::render("syntetic-debug.svg", [] () {
    ptds.clear();
    for(auto p: debugmap) {
      heptagon *h = p.first;
      queuestr(p.second*C0, vid.yres/50, its(create_order[h]) + "/" + its(h->c7->mpdist), 0xFF000000);
      for(int i=0; i<neighbors_of(h); i++) {
        if(h->move(i))
          queueline(p.second*C0, debugmap[h->move(i)]*C0, 
            i == parents_of(h) ? 0xFF0000FF :
            i == right_sibling_of(h) ? 0x800000FF :
            i == 0 ? 0x008000FF :
            0x000000FF);
        }
      }
    hr::drawqueue();
    });
  exit(1);
  }

#if CAP_COMMANDLINE
auto hook = 
  addHook(hooks_args, 100, readArgs);
#endif



}

}