namespace hr {

namespace synt {

#define SDEBUG(x) if(debug_geometry) { doindent(); x; fflush(stdout); }

static const int sfPH = 1;
static const int sfLINE = 2;

// Marek-snub
vector<int> faces = {3, 6, 6, 6};
vector<int> adj = {1, 0, 2, 3};
vector<bool> invert = {false, false, true, false};
vector<int> nflags = {sfPH | sfLINE, 0, 0, 0};

bool have_ph, have_line, have_symmetry;

int repetition = 1;
int N;

vector<int> flags;

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

ld edgelength;

vector<ld> inradius, circumradius, alphas;

int matches[30][30];
int periods[30];
int tilegroup[30], groupoffset[30], tilegroups;

int gcd(int x, int y) { return x ? gcd(y%x, x) : y; }

int errors;

pair<int, int>& get_adj(heptagon *h, int cid);
pair<ld, ld>& get_triangle(heptagon *h, int cid);
pair<ld, ld>& get_triangle(const pair<int, int>& p, int delta = 0);
pair<int, int>& get_adj(const pair<int, int>& p, int delta = 0);

void make_match(int a, int i, int b, int j) {
  if(periods[a] != periods[b]) 
    errors++;
  if(matches[a][b] == -1)
    matches[a][b] = j - i, matches[b][a] = i - j;
  else
    periods[a] = periods[b] = gcd(matches[a][b] - (j-i), periods[a]);
  }

void prepare() {

  errors = 0;

  /* build the 'adjacent' table */

  N = isize(faces);
  int M = 2 * N + 2;
  adjacent.clear();
  adjacent.resize(M);

  have_symmetry = false;
  for(int i=0; i<N; i++) if(invert[i]) have_symmetry = true;  
  
  for(int i=0; i<M; i++) for(int j=0; j<M; j++) matches[i][j] = i==j ? 0 : -1;

  for(int i=0; i<M; i++) periods[i] = i<2*N ? faces[i/2] : N;
  
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
      }
    }
  for(int i=0; i<N; i++) {
    adjacent[2*N].emplace_back(2*i, 0);
    int ai = (i+1) % N;
    adjacent[2*N].emplace_back(2*N+int(invert[ai]), (2*adj[ai]+2*N-1) % (2*N));
    }
  
  for(int d=0; d<=2*N; d+=2) {
    int s = isize(adjacent[d]);
    for(int i=0; i<s; i++) {
      auto& orig = adjacent[d][s-1-i];
      adjacent[d+1].emplace_back(orig.first ^ 1, orig.second);
      }
    }
  for(int d=0; d<M; d++) {
    int s = isize(adjacent[d]);
    for(int i=0; i<s; i++) {
      auto& orig = adjacent[d][i];
      if(orig.first & 1) orig.second = isize(adjacent[orig.first]) - 1 - orig.second;
      }
    }
  
  for(int i=0; i<M; i++) {
    printf("adjacent %2d:", i);
    for(int j=0; j<isize(adjacent[i]); j++) {
      auto p = adjacent[i][j];
      printf(" (%d,%d)", p.first, p.second);
      }
    printf("\n");
    }
    
  /* verify all the triangles */
  for(int i=0; i<M; i++) {
    for(int j=0; j<isize(adjacent[i]); j++) {
      int ai = i, aj = j;
      printf("triangle ");
      for(int s=0; s<3; s++) {
        printf("[%d %d] ", ai, aj); fflush(stdout);
        tie(ai, aj) = adjacent[ai][aj];
        aj++; if(aj >= isize(adjacent[ai])) aj = 0;
        }
      printf("-> [%d %d]\n", ai, aj);
      make_match(i, j, ai, aj);
      }
    }

  for(int i=0; i<2*N; i++) {
    for(int j=0; j<isize(adjacent[i]); j++) {
      auto aa = make_pair(i, j);
      aa = get_adj(aa, 1);
      aa = get_adj(aa, 2);
      aa = get_adj(aa, 1);
      aa = get_adj(aa, 2);
      make_match(i, j, aa.first, aa.second);
      }
    }
  
  for(int i=0; i<M; i++) for(int j=0; j<M; j++) if(matches[i][j] != -1)
  for(int l=0; l<M; l++) for(int k=0; k<M; k++) if(matches[j][k] != -1) {
    make_match(i, 0, k, matches[i][j] + matches[j][k]);
    make_match(i, 0, k, matches[i][j] + matches[j][k] + gcd(periods[i], periods[j]));
    }
  for(int i=0; i<M; i++) tilegroup[i] = -1;
  tilegroups = 0;
  for(int i=0; i<M; i+=(have_symmetry?1:2)) if(tilegroup[i] == -1) {
    if(periods[i] < 0) periods[i] = -periods[i];
    for(int j=0; j<M; j++) if(matches[i][j] != -1)
      tilegroup[j] = tilegroups, groupoffset[j] = matches[i][j] % periods[i];
    tilegroups++;
    }
  
  flags.clear();
  flags.resize(M);
  for(int i=0; i<M; i++)
  for(int j=0; j<2*N; j++)
    if(tilegroup[i] == tilegroup[j]) flags[i] |= nflags[j/2];
  
  if(!have_ph) {
    if(nonbitrunc) {
      for(int i=0; i<M; i++) if(tilegroup[i] == 0) flags[i] |= sfPH;
      }
    else {
      for(int z=2*N; z<2*N+2; z++) flags[z] |= sfPH;
      }
    }

  for(int i=0; i<M; i+=(have_symmetry?1:2)) {
    printf("tiling group of %2d: [%2d]%2d+Z%2d\n", i, tilegroup[i], groupoffset[i], periods[i]);
    printf("\n");
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
        hyperpoint p1 = xpush0(circumradius[i]);
        hyperpoint p2 = spin(2 * M_PI / faces[i]) * p1;
        inradius[i] = hdist0(mid(p1, p2));
        if(hdist(p1, p2) > edgelength) crmax = circumradius[i];
        else crmin = circumradius[i];
        }
      hyperpoint h = xpush(edgelength/2) * xspinpush0(M_PI/2, inradius[i]);
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
  triangles.resize(M);
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

map<heptagon*, vector<pair<heptagon*, transmatrix> > > altmap;

map<heptagon*, pair<heptagon*, transmatrix>> syntetic_gmatrix;

hrmap *current_altmap;

struct hrmap_syntetic : hrmap {
  heptagon *origin;
  heptagon *getOrigin() { return origin; }

  hrmap_syntetic() {
    origin = new heptagon;
    origin->s = hsOrigin;
    origin->emeraldval = 0;
    origin->zebraval = 0;
    origin->fiftyval = 0;
    origin->fieldval = 0;
    origin->rval0 = origin->rval1 = 0;
    origin->cdata = NULL;
    origin->c.clear();
    origin->alt = NULL;
    origin->distance = 0;

    parent_index_of(origin) = 0;
    id_of(origin) = 0;
    origin->c7 = newCell(isize(adjacent[0]), origin);
    
    heptagon *alt = NULL;
    
    if(hyperbolic) {
      dynamicval<eGeometry> g(geometry, gNormal); 
      alt = new heptagon;
      alt->s = hsOrigin;
      alt->emeraldval = 0;
      alt->zebraval = 0;
      alt->c.clear();
      alt->distance = 0;
      alt->c7 = NULL;
      alt->alt = alt;
      alt->cdata = NULL;
      current_altmap = newAltMap(alt); 
      }
  
    transmatrix T = xpush(.01241) * spin(1.4117) * xpush(0.1241) * Id;
    syntetic_gmatrix[origin] = make_pair(alt, T);
    altmap[alt].emplace_back(origin, T);
    
    base_distlimit = 0;
    celllister cl(origin->c7, 1000, 200, NULL);
    base_distlimit = cl.dists.back();
    if(sphere) base_distlimit = 15;
    }

  ~hrmap_syntetic() {
    clearfrom(origin);
    altmap.clear();
    syntetic_gmatrix.clear();
    if(current_altmap) {
      dynamicval<eGeometry> g(geometry, gNormal);       
      delete current_altmap;
      current_altmap = NULL;
      }
    }
  void verify() { }
  };

hrmap *new_map() { return new hrmap_syntetic; }

transmatrix adjcell_matrix(heptagon *h, int d);

heptagon *build_child(heptagon *parent, int d, int id, int pindex) {
  indenter ind;
  auto h = buildHeptagon1(new heptagon, parent, d, hstate(1), 0);
  SDEBUG( printf("NEW %p.%d ~ %p.0\n", parent, d, h); )
  id_of(h) = id;
  parent_index_of(h) = pindex;
  int nei = neighbors_of(h);
  h->c7 = newCell(nei, h);
  h->distance = parent->distance + 1;
  return h;
  }

void connectHeptagons(heptagon *h, int i, heptspin hs) {
  SDEBUG( printf("OLD %p.%d ~ %p.%d\n", h, i, hs.at, hs.spin); )
  if(h->move(i) == hs.at && h->c.spin(i) == hs.spin) {
    SDEBUG( printf("WARNING: already connected\n"); )
    return;
    }
  if(h->move(i)) {
    SDEBUG( printf("ERROR: already connected left\n"); )
    exit(1);
    }
  if(hs.peek()) {
    SDEBUG( printf("ERROR: already connected right\n"); )
    exit(1);
    }
  h->c.connect(i, hs);

  auto p = get_adj(h, i);
  if(tilegroup[p.first] != tilegroup[id_of(hs.at)])
    printf("should merge %d %d\n", p.first, id_of(hs.at));
  // heptagon *hnew = build_child(h, d, get_adj(h, d).first, get_adj(h, d).second);
  }

void create_adjacent(heptagon *h, int d) {

  SDEBUG( printf("%p.%d ~ ?\n", h, d); )

  auto& t1 = get_triangle(h, d);

  // * spin(-tri[id][pi+i].first) * xpush(t.second) * pispin * spin(tri[id'][p'+d'].first)
  
  auto& p = syntetic_gmatrix[h];
  
  heptagon *alt = p.first;

  transmatrix T = p.second * spin(-t1.first) * xpush(t1.second);
  
  if(hyperbolic) {
    dynamicval<eGeometry> g(geometry, gNormal); 
    virtualRebaseSimple(alt, T);
    }
  
  if(euclid) 
    alt = encodeId(pair_to_vec(int(T[0][2]), int(T[1][2])));
    
  SDEBUG( printf("look for: %p / %s\n", alt, display(T * C0)); )
  
  for(auto& p: altmap[alt]) if(intval(p.second * C0, T * C0) < 1e-6) {
    SDEBUG( printf("cell found: %p\n", p.first); )
    for(int d2=0; d2<p.first->c7->type; d2++) {
      auto& t2 = get_triangle(p.first, d2);
      transmatrix T1 = T * spin(M_PI + t2.first);
      SDEBUG( printf("compare: %s", display(T1 * xpush0(1)));  )
      SDEBUG( printf(":: %s\n", display(p.second * xpush0(1)));  )
      if(intval(T1 * xpush0(1), p.second * xpush0(1)) < 1e-6) {
        connectHeptagons(h, d, heptspin(p.first, d2));
        return;
        }
      }
    SDEBUG( printf("but rotation not found\n"));
    }
  
  auto& t2 = get_triangle(get_adj(h, d));
  transmatrix T1 = T * spin(M_PI + t2.first);
  fixmatrix(T1);

  heptagon *hnew = build_child(h, d, get_adj(h, d).first, get_adj(h, d).second);
  altmap[alt].emplace_back(hnew, T1);
  syntetic_gmatrix[hnew] = make_pair(alt, T1);
  }

set<heptagon*> visited;
queue<pair<heptagon*, transmatrix>> drawqueue;

void enqueue(heptagon *h, const transmatrix& T) {
  if(visited.count(h)) { return; }
  visited.insert(h);
  drawqueue.emplace(h, T);
  }

pair<ld, ld>& get_triangle(heptagon *h, int cid) {
  return triangles[id_of(h)][(parent_index_of(h) + cid + MODFIXER) % neighbors_of(h)];
  }

pair<int, int>& get_adj(heptagon *h, int cid) {
  return adjacent[id_of(h)][(parent_index_of(h) + cid + MODFIXER) % neighbors_of(h)];
  }

pair<int, int>& get_adj(const pair<int, int>& p, int delta) {
  return adjacent[p.first][(p.second + delta + MODFIXER) % isize(adjacent[p.first])];
  }

pair<ld, ld>& get_triangle(const pair<int, int>& p, int delta) {
  return triangles[p.first][(p.second + delta + MODFIXER) % isize(adjacent[p.first])];
  }

transmatrix adjcell_matrix(heptagon *h, int d) {
  auto& t1 = get_triangle(h, d);

  heptagon *h2 = h->move(d);

  int d2 = h->c.spin(d);
  auto& t2 = get_triangle(h2, d2);
  
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
  
  faces.clear(); nflags.clear();
  have_line = false;
  have_ph = false;
  while(true) {
    if(peek() == ')' || peek() == '^' || (peek() == '(' && isize(faces)) || peek() == 0) break;
    else if((peek() == 'L' || peek() == 'l') && faces.size())
      nflags.back() |= sfLINE, have_line = true, at++;
    else if((peek() == 'H' || peek() == 'h') && faces.size())
      nflags.back() |= sfPH, have_ph = true, at++;
    else if(isnumber()) faces.push_back(read_number()), nflags.push_back(0);
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
  else if(argis("-dgeom")) debug_geometry = true;
  else return 1;
  return 0;
  }
#endif

#if CAP_COMMANDLINE
auto hook = 
  addHook(hooks_args, 100, readArgs);
#endif

int support_threecolor() {
  if(nonbitrunc) 
    return 
      (isize(faces) == 3 && faces[0] % 2 == 0 && faces[1] % 2 == 0 && faces[2] % 2 == 0 && tilegroup[N*2] == 3) ? 2 :
      tilegroup[N*2] > 1 ? 1 :
      0;
  for(int i: faces) if(faces[i] % 2) return tilegroup[N*2] > 1 ? 1 : 0;
  return 2;
  }

int support_graveyard() {
  if(!nonbitrunc) return 2;
  return 
    isize(synt::faces) == 3 && synt::faces[0] % 2 == 0 ? 2 :
    synt::have_ph ? 1 :
    0;
  }

bool support_chessboard() {
  return 0;
  }

bool pseudohept(int id) {
  return flags[id] & synt::sfPH;
  }

bool linespattern(cell *c) {
  return flags[id_of(c->master)] & synt::sfLINE;
  }

int threecolor(int id) {
  if(nonbitrunc)
    return tilegroup[id];
  else {
    if(support_threecolor() == 2) return id < N * 2 ? (id&1) : 2;
    return tilegroup[id];
    }
  }

}

}