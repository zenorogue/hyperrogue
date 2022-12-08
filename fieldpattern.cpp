// Hyperbolic Rogue -- Field Quotient geometry
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file fieldpattern.cpp
 *  \brief Field Quotient geometry
 */

#include "hyper.h"
#if CAP_FIELD
namespace hr {

EX namespace fieldpattern {

EX bool use_rule_fp = false;

EX bool use_quotient_fp = false;

int limitsq = 10;
int limitp = 10000;
int limitv = 100000;

#if HDR
#define currfp fieldpattern::getcurrfp()

struct primeinfo {
  int p;
  int cells;
  bool squared;
  };  

struct fgeomextra {
  eGeometry base;
  vector<primeinfo> primes;
  vector<int> dualval;
  int current_prime_id;
  fgeomextra(eGeometry b, int i) : base(b), current_prime_id(i) {}
  };
#endif

EX bool isprime(int n) {
  for(int k=2; k<n; k++) if(n%k == 0) return false;
  return true;
  }
  
#if HDR
#define MWDIM (mproduct ? 3 : WDIM+1)

struct matrix : array<array<int, MAXMDIM>, MAXMDIM> {
  bool operator == (const matrix& B) const {
    for(int i=0; i<MWDIM; i++) for(int j=0; j<MWDIM; j++)
      if(self[i][j] != B[i][j]) return false;
    return true;
    }
  
  bool operator != (const matrix& B) const {
    for(int i=0; i<MWDIM; i++) for(int j=0; j<MWDIM; j++)
      if(self[i][j] != B[i][j]) return true;
    return false;
    }
  
  bool operator < (const matrix& B) const {
    for(int i=0; i<MWDIM; i++) for(int j=0; j<MWDIM; j++)
      if(self[i][j] != B[i][j]) return self[i][j] < B[i][j];
    return false;
    }
  
  };
#endif

EX int groupspin(int id, int d, int group) {
  return group*(id/group) + (id + d) % group;
  }

EX int btspin(int id, int d) {
  return groupspin(id, d, S7);
  }

#if HDR

static const int ERR = -99;

struct triplet_info {
  int i, j, size;
  };

struct fpattern {

  unsigned force_hash;
  
  int Prime, wsquare, Field, dual;
  // we perform our computations in the field Z_Prime[w] where w^2 equals wsquare
  // (or simply Z_Prime for wsquare == 0)

  #define EASY
  // 'easy' assumes that all elements of the field actually used
  // are of form n or mw (not n+mw), and cs and ch are both of form n
  // by experimentation, such cs and ch always exist
  // many computations are much simpler under that assumption
  
  #ifndef EASY
  static int neasy;
  
  int m(int x) { x %= Prime; if(x<0) x+= Prime; return x; }
  #endif
  
  int sub(int a, int b) { 
    #ifdef EASY
    return (a + b * (Prime-1)) % Prime;
    #else
    return m(a%Prime-b%Prime) + Prime * m(a/Prime-b/Prime);
    #endif
    }
  
  int add(int a, int b) { 
    #ifdef EASY
    if(a == ERR || b == ERR || a*b<0) return ERR;
    return (a+b)%Prime;
    #else
    return m(a%Prime+b%Prime) + Prime * m(a/Prime+b/Prime);
    #endif
    }
  
  int mul(int tx, int ty) {
    #ifdef EASY
    return (tx*ty*((tx<0&&ty<0)?wsquare:1)) % Prime;
    #else
    if(tx >= Prime && tx % Prime) neasy++; 
    if(ty >= Prime && ty % Prime) neasy++; 
    int x[2], y[2], z[3];
    for(int i=0; i<3; i++) z[i] = 0;
    for(int i=0; i<2; i++) 
      x[i] = tx%Prime, tx /= Prime;
    for(int i=0; i<2; i++) 
      y[i] = ty%Prime, ty /= Prime;
    for(int i=0; i<2; i++)
    for(int j=0; j<2; j++)
      z[i+j] = (z[i+j] + x[i] * y[j]) % Prime;
    z[0] += z[2] * wsquare;
    
    return m(z[0]) + Prime * m(z[1]);
    #endif
    }
  
  int sqr(int x) { return mul(x,x); }

  int err;

  matrix mmul(const matrix& A, const matrix& B) {
    matrix res;
    for(int i=0; i<MWDIM; i++) for(int k=0; k<MWDIM; k++) {
      int t = 0;
  #ifdef EASY
      int tp = 0, tn = 0;
      for(int j=0; j<MWDIM; j++) {
        int val = mul(A[i][j], B[j][k]);
        if(val > 0) tp += val;
        else tn += val;
        }
      tp %= Prime; tn %= Prime;
      if(tp && tn) err++;
      t = tp + tn;
  #else
      for(int j=0; j<MWDIM; j++) t = add(t, mul(A[i][j], B[j][k]));
  #endif
      res[i][k] = t;
      }
    return res;
    }
  
  map<matrix, int> matcode;
  vector<matrix> matrices;
  
  vector<string> qpaths;
  
  vector<matrix> qcoords;
  
  // S7 in 2D, but e.g. 4 for a 3D cube
  int rotations;
  
  // S7 in 2D, but e.g. 24 for a 3D cube
  int local_group;
  
  // Id: Identity
  // R : rotate by 1/rotations of the full circle
  // P : make a step and turn backwards
  // X : in 3-dim, turn by 90 degrees

  matrix Id, R, P, X;
  
  matrix strtomatrix(string s) {
    matrix res = Id;
    matrix m = Id;
    for(int i=isize(s)-1; i>=0; i--)
      if(s[i] == 'R') res = mmul(R, res);
      else if (s[i] == 'P') res = mmul(P, res);
      else if (s[i] == 'x') { m[0][0] = -1; res = mmul(m, res); m[0][0] = +1; }
      else if (s[i] == 'y') { m[1][1] = -1; res = mmul(m, res); m[1][1] = +1; }
      else if (s[i] == 'z') { m[2][2] = -1; res = mmul(m, res); m[2][2] = +1; }
    return res;
    }
  
  void addas(const matrix& M, int i) {
    if(!matcode.count(M)) {
      matcode[M] = i;
      for(int j=0; j<isize(qcoords); j++)
        addas(mmul(M, qcoords[j]), i);
      }
    }
  
  void add(const matrix& M) {
    if(!matcode.count(M)) {
      int i = isize(matrices);
      matcode[M] = i, matrices.push_back(M);
      for(int j=0; j<isize(qcoords); j++)
        addas(mmul(M, qcoords[j]), i);
      if(WDIM == 3) add(mmul(X, M));
      add(mmul(R, M));
      }
    }
  
  #define MXF 1000000
  
  vector<int> connections;
  
  vector<int> inverses; // NYI in 3D
  
  // 2D only
  vector<int> rrf; // rrf[i] equals gmul(i, rotations-1)
  vector<int> rpf; // rpf[i] equals gmul(i, rotations)
  
  matrix mpow(matrix M, int N) {
    while((N&1) == 0) N >>= 1, M = mmul(M, M);
    matrix res = M;
    N >>= 1;
    while(N) {      
      M = mmul(M,M); if(N&1) res = mmul(res, M);
      N >>= 1;
      }
    return res;
    }
  
  int gmul(int a, int b) { return matcode[mmul(matrices[a], matrices[b])]; }
  int gpow(int a, int N) { return matcode[mpow(matrices[a], N)]; }
  
  int gorder(int a) {
    int b = a;
    int q = 1;
    while(b) b = gmul(b, a), q++;
    return q;
    }

  pair<int,bool> gmul(pair<int, bool> a, int b) { 
    return make_pair(gmul(a.first,b), a.second); 
    }
  
  int order(const matrix& M);
  
  string decodepath(int i) {
    string s;
    while(i) {
      if(i % S7) i--, s += 'R';
      else i = connections[i], s += 'P';
      }
    return s;
    }
  
  int orderstats();
  
  int cs, sn, ch, sh;
  
  int solve();
  
  void build();
  
  static const int MAXDIST = 120;
  
  vector<char> disthep;
  vector<char> disthex;
  
  vector<char> distwall, distriver, distwall2, distriverleft, distriverright, distflower;
  int distflower0;
  
  vector<eItem> markers;

  int getdist(pair<int,bool> a, vector<char>& dists);
  int getdist(pair<int,bool> a, pair<int,bool> b);
  int dijkstra(vector<char>& dists, vector<int> indist[MAXDIST]);
  void analyze();
  
  int maxdist, otherpole, circrad, wallid, wallorder, riverid;

  bool easy(int i) {
    return i < Prime || !(i % Prime);
    }
  
  // 11 * 25
  // (1+z+z^3) * (1+z^3+z^4) ==
  // 1+z+z^7 == 1+z+z^2(z^5) == 1+z+z^2(1+z^2) = 1+z+z^2+z^4
  
  void init(int p) {
    Prime = p;
    if(solve()) {
      printf("error: could not solve the fieldpattern\n");
      exit(1);
      }
    build();
    analyze();
    }
    
  fpattern(int p) {
    force_hash = 0;
    #if CAP_THREAD && MAXMDIM >= 4
    dis = nullptr;
    #endif
    if(!p) return;
    init(p);
    }
  
  void findsubpath();
  
  vector<matrix> generate_isometries();
  
  bool check_order(matrix M, int req);
  
  unsigned compute_hash();

  void set_field(int p, int sq);
  
  unsigned hashv;

  #if MAXMDIM >= 4
  // general 4D
  vector<transmatrix> fullv;

  void add1(const matrix& M);
  void add1(const matrix& M, const transmatrix& Full);
  vector<matrix> generate_isometries3();
  int solve3();
  bool generate_all3();
  
  #if CAP_THREAD
  struct discovery *dis;
  #endif
  #endif

  vector<triplet_info> find_triplets();
  void generate_quotientgroup();
  };

#if CAP_THREAD && MAXMDIM >= 4
struct discovery {
  fpattern experiment;
  std::shared_ptr<std::thread> discoverer;
  std::mutex lock;
  std::condition_variable cv;
  bool is_suspended;
  bool stop_it;
  
  map<unsigned, tuple<int, int, matrix, matrix, matrix, int> > hashes_found;
  discovery() : experiment(0) { is_suspended = false; stop_it = false; experiment.dis = this; experiment.Prime = experiment.Field = experiment.wsquare = 0; }
  
  void activate();
  void suspend();
  void check_suspend();
  void schedule_destruction();
  void discovered();
  ~discovery();
  };
#endif

#endif

bool fpattern::check_order(matrix M, int req) {
  int err = 0;
  matrix P = M;
  for(int i=1; i<req; i++) {
    if(P == Id) return false;
    P = mmul(P, M);
    }
  return P == Id && !err;
  }

vector<matrix> fpattern::generate_isometries() {
  matrix T = Id;
  int low = wsquare ? 1-Prime : 0;
  vector<matrix> res;
  
  auto colprod = [&] (int a, int b) {
    return add(add(mul(T[0][a], T[0][b]), mul(T[1][a], T[1][b])), mul(T[2][a], T[2][b]));
    };

  for(T[0][0]=low; T[0][0]<Prime; T[0][0]++)
  for(T[1][0]=low; T[1][0]<Prime; T[1][0]++)
  for(T[2][0]=low; T[2][0]<Prime; T[2][0]++)
  if(colprod(0, 0) == 1)
  for(T[0][1]=low; T[0][1]<Prime; T[0][1]++)
  for(T[1][1]=low; T[1][1]<Prime; T[1][1]++)
  for(T[2][1]=low; T[2][1]<Prime; T[2][1]++)
  if(colprod(1, 1) == 1)
  if(colprod(1, 0) == 0)
  for(T[0][2]=low; T[0][2]<Prime; T[0][2]++)
  for(T[1][2]=low; T[1][2]<Prime; T[1][2]++)
  for(T[2][2]=low; T[2][2]<Prime; T[2][2]++)
  if(colprod(2, 2) == 1)
  if(colprod(2, 0) == 0)
  if(colprod(2, 1) == 0)
    res.push_back(T);

  return res;
  }

#if MAXMDIM >= 4
vector<matrix> fpattern::generate_isometries3() {
  
  matrix T = Id;
  int low = wsquare ? 1-Prime : 0;
  vector<matrix> res;
  
  auto colprod = [&] (int a, int b) {
    return add(add(mul(T[0][a], T[0][b]), mul(T[1][a], T[1][b])), sub(mul(T[2][a], T[2][b]), mul(T[3][a], T[3][b])));
    };

  auto rowcol = [&] (int a, int b) {
    return add(add(mul(T[a][0], T[0][b]), mul(T[a][1], T[1][b])), add(mul(T[a][2], T[2][b]), mul(T[a][3], T[3][b])));
    };

  for(T[0][0]=low; T[0][0]<Prime; T[0][0]++)
  for(T[1][0]=low; T[1][0]<Prime; T[1][0]++)
  for(T[2][0]=low; T[2][0]<Prime; T[2][0]++)
  for(T[3][0]=low; T[3][0]<Prime; T[3][0]++)
  if(colprod(0, 0) == 1)
  for(T[0][1]=low; T[0][1]<Prime; T[0][1]++)
  for(T[1][1]=low; T[1][1]<Prime; T[1][1]++)
  for(T[2][1]=low; T[2][1]<Prime; T[2][1]++)
  for(T[3][1]=low; T[3][1]<Prime; T[3][1]++)
  if(colprod(1, 1) == 1)
  if(colprod(1, 0) == 0) {
    #if CAP_THREAD && MAXMDIM >= 4
    if(dis) dis->check_suspend();
    if(dis && dis->stop_it) return res;
    #endif

    for(T[0][2]=low; T[0][2]<Prime; T[0][2]++)
    for(T[0][3]=low; T[0][3]<Prime; T[0][3]++)
    if(rowcol(0, 0) == 1)
    if(rowcol(0, 1) == 0)
    for(T[1][2]=low; T[1][2]<Prime; T[1][2]++)
    for(T[1][3]=low; T[1][3]<Prime; T[1][3]++)
    if(rowcol(1, 0) == 0)
    if(rowcol(1, 1) == 1)
    for(T[2][2]=low; T[2][2]<Prime; T[2][2]++)
    for(T[3][2]=low; T[3][2]<Prime; T[3][2]++)
    if(colprod(2, 2) == 1)
    if(colprod(2, 0) == 0)
    if(colprod(2, 1) == 0)
    for(T[2][3]=low; T[2][3]<Prime; T[2][3]++)
    for(T[3][3]=low; T[3][3]<Prime; T[3][3]++)
    if(rowcol(2, 0) == 0)
    if(rowcol(2, 1) == 0)
    if(rowcol(2, 2) == 1)
    // if(colprod(3, 3) == 1)
    if(add(colprod(3, 3), 1) == 0)
    if(colprod(3, 0) == 0)
    if(colprod(3, 1) == 0)
    if(colprod(3, 2) == 0)
    if(rowcol(3, 3) == 1)
    if(rowcol(3, 0) == 0)
    if(rowcol(3, 1) == 0)
    if(rowcol(3, 2) == 0)
      res.push_back(T);
    if(isize(res) > limitp) return res;
    }

  return res;
  }

void fpattern::add1(const matrix& M) {
  if(!matcode.count(M)) {
    int i = isize(matrices);
    matcode[M] = i, matrices.push_back(M);
    }
  }

void fpattern::add1(const matrix& M, const transmatrix& Full) {
  if(!matcode.count(M)) {
    int i = isize(matrices);
    matcode[M] = i, matrices.push_back(M), fullv.push_back(Full);
    }
  }
#endif

map<unsigned,int> hash_found;

unsigned fpattern::compute_hash() {
  unsigned hashv = 0;
  int iR = matcode[R];
  int iP = matcode[P];
  int iX = matcode[X];
  for(int i=0; i<isize(matrices); i++) {
    hashv = 3 * hashv + gmul(i, iP) + 7 * gmul(i, iR);
    if(MWDIM == 4) hashv += 11 * gmul(i, iX);
    }
  return hashv;
  }

#if MAXMDIM >= 4
bool fpattern::generate_all3() {

  reg3::generate_fulls();
  err = 0;

  matrices.clear();
  matcode.clear();
  add1(Id);
  fullv = {hr::Id};
  for(int i=0; i<isize(matrices); i++) {
    add1(mmul(matrices[i], R), fullv[i] * cgi.full_R);
    add1(mmul(matrices[i], X), fullv[i] * cgi.full_X);
    if(err) return false;
    }
  local_group = isize(matrices);
  if(local_group != isize(cgi.cellrotations)) return false;

  for(int i=0; i<(int)matrices.size(); i++) {
    matrix E = mmul(matrices[i], P);
    if(!matcode.count(E))
      for(int j=0; j<local_group; j++) add1(mmul(E, matrices[j]));
    if(err) return false;
    if(isize(matrices) >= limitv) { println(hlog, "limitv exceeded"); return false; }
    }
  hashv = compute_hash();
  DEBB(DF_FIELD, ("all = ", isize(matrices), "/", local_group, " = ", isize(matrices) / local_group, " hash = ", hashv, " count = ", ++hash_found[hashv]));
  
  if(use_quotient_fp) 
    generate_quotientgroup();  
  return true;
  }

void fpattern::generate_quotientgroup() {
  int MS = isize(matrices);
  int best_p = 0, best_i = 0;
  for(int i=0; i<MS; i++) {
    int j = i, p = 1;
    while(j >= local_group) 
      j = gmul(j, i), p++;
    if(j == 0 && p > best_p) {
      bool okay = true;

      vector<bool> visited(MS, false);
      for(int ii=0; ii<MS; ii++) if(!visited[ii]) {
        int jj = ii;
        for(int k=0; k<p; k++) {
          if(k && jj/local_group == ii/local_group) okay = false;
          visited[jj] = true;
          jj = gmul(i, jj);
          }
        }
      
      if(okay) {
        bool chk = (MS/p) % local_group;
        println(hlog, "quotient by ", i, " : ", p, " times less, ", (MS/p/local_group), " tiles, check ", chk);
        best_p = p; best_i = i;
        if(chk) {
          exit(1);
          }
        }
      }
    }
  
  if(best_p > 1) {
    vector<int> new_id(MS, -1);
    vector<int> orig_id(MS, -1);
    vector<matrix> new_matrices;
    int nv = 0;
    for(int i=0; i<MS; i++) if(new_id[i] == -1) {
      int prode = i;
      for(int l=0; l<local_group; l++) {
        new_matrices.push_back(matrices[i+l]);
        }
      for(int k=0; k<best_p; k++) {
        for(int l=0; l<local_group; l++) {
          new_id[gmul(prode, l)] = nv + l;
          }
        prode = gmul(best_i, prode);
        }
      nv += local_group;
      }
    println(hlog, "got nv = ", nv, " / ", local_group);
    
    for(int i=0; i<MS; i++)
      matcode[matrices[i]] = new_id[i];
    matrices = std::move(new_matrices);
    println(hlog, "size matrices = ", isize(matrices), " size matcode = ", isize(matcode));
    println(hlog, tie(P, R, X));
    
    /*println(hlog, "TRY AGAIN");
    generate_quotientgroup();
    exit(1);*/
    }  
  
  }

EX purehookset hooks_solve3;

int fpattern::solve3() {

  reg3::generate_fulls();
  
  DEBB(DF_FIELD, ("generating isometries for ", Field));
  
  auto iso3 = generate_isometries();
  auto iso4 = generate_isometries3();

  int cmb = 0;
  
  vector<matrix> possible_P, possible_X, possible_R;
  
  for(auto& M: iso3) {
    if(check_order(M, 2)) 
      possible_X.push_back(M);
    if(check_order(M, cgi.r_order)) 
      possible_R.push_back(M);
    }
  for(auto& M: iso4) 
    if(check_order(M, 2)) 
      possible_P.push_back(M);
    
  DEBB(DF_FIELD, ("field = ", Field, " #P = ", isize(possible_P), " #X = ", isize(possible_X), " #R = ", isize(possible_R), " r_order = ", cgi.r_order, " xp_order = ", cgi.xp_order));
                                                                                                                               
  for(auto& xX: possible_X)
  for(auto& xP: possible_P) if(check_order(mmul(xP, xX), cgi.xp_order)) 
  for(auto& xR: possible_R) if(check_order(mmul(xR, xX), cgi.rx_order)) {

    err = 0;
    if(mmul(xX, xP) != mmul(xR, mmul(mmul(xP, xX), xR))) continue;
    if(err) continue;

    #if CAP_THREAD && MAXMDIM >= 4
    if(dis) dis->check_suspend();
    if(dis && dis->stop_it) return 0;
    #endif

    P = xP; R = xR; X = xX;
    if(!generate_all3()) continue;
    callhooks(hooks_solve3);
    #if CAP_THREAD && MAXMDIM >= 4
    if(dis) { dis->discovered(); continue; }
    #endif
    if(force_hash && hashv != force_hash) continue;
    cmb++;
    goto ok;
    }

  ok:

  DEBB(DF_FIELD, ("cmb = ", cmb, " for field = ", Field));
  
  return cmb;
  }
#endif

void fpattern::set_field(int p, int sq) {
  Prime = p;
  Field = sq ? Prime*Prime : Prime;
  wsquare = sq;
  for(int a=0; a<MWDIM; a++) for(int b=0; b<MWDIM; b++) Id[a][b] = a==b?1:0;
  }

int fpattern::solve() {
  
  for(int a=0; a<MWDIM; a++) for(int b=0; b<MWDIM; b++) Id[a][b] = a==b?1:0;

  if(!isprime(Prime)) {
    return 1;
    }
  
  rotations = WDIM == 2 ? S7 : 4;
  local_group = WDIM == 2 ? S7 : 24;
  
  for(dual=0; dual<3; dual++) {
  for(int pw=1; pw<3; pw++) {
    if(pw>3) break;
    Field = pw==1? Prime : Prime*Prime;
    
    if(pw == 2) {
      for(wsquare=1; wsquare<Prime; wsquare++) {
        int roots = 0;
        for(int a=0; a<Prime; a++) if((a*a)%Prime == wsquare) roots++;
        if(!roots) break;
        }
      } else wsquare = 0;

    #if MAXMDIM >= 4
    if(WDIM == 3) {
      if(dual == 0 && (Prime <= limitsq || pw == 1)) {
        int s = solve3();
        if(s) return 0;
        }
      continue;
      }
    #endif

    if(dual == 2) {
      if(Field <= 10) {
        vector<matrix> all_isometries = generate_isometries();
        for(auto& X: all_isometries) 
          if(check_order(X, rotations))
            for(auto& Y: all_isometries)
              if(check_order(Y, 2) && check_order(mmul(X, Y), S3)) {
                R = X; P = Y;
                return 0;
                }
        }
      continue;
      }

#ifdef EASY        
    std::vector<int> sqrts(Prime, 0);
    for(int k=1-Prime; k<Prime; k++) sqrts[sqr(k)] = k;
    int fmax = Prime;
#else
    std::vector<int> sqrts(Field);
    for(int k=0; k<Field; k++) sqrts[sqr(k)] = k;
    int fmax = Field;
#endif

    R = P = X = Id;
    X[1][1] = 0; X[2][2] = 0;
    X[1][2] = 1; X[2][1] = Prime-1;
            
    for(cs=0; cs<fmax; cs++) {
      int sb = sub(1, sqr(cs));

      sn = sqrts[sb];

      R[0][0] = cs; R[1][1] = cs;
      R[0][1] = sn; R[1][0] = sub(0, sn);
      
      if(!check_order(R, dual ? S3 : rotations)) continue;
      
      if(R[0][0] == 1) continue;
      
      for(ch=2; ch<fmax; ch++) {
        int chx = sub(mul(ch,ch), 1);
        
        sh = sqrts[chx];
        P[0][0] = sub(0, ch);
        P[0][WDIM] = sub(0, sh);
        P[1][1] = Prime-1;
        P[WDIM][0] = sh;
        P[WDIM][WDIM] = ch;
        
        if(!check_order(mmul(P, R), dual ? rotations : S3)) continue;
        
        if(dual) R = mmul(P, R);
        
        return 0;
        }
      }
    }
    }

  return 2;
  }
  
int fpattern::order(const matrix& M) {
  int cnt = 1;
  matrix Po = M;
  while(Po != Id) Po = mmul(Po, M), cnt++;
  return cnt;
  }

EX int triplet_id = 0;

vector<triplet_info> fpattern::find_triplets() {
  int N = isize(matrices);
  auto compute_transcript = [&] (int i, int j) {

    vector<int> indices(N, -1);
    vector<int> transcript;
    vector<int> q;
    
    int qty = 0;
    
    auto visit = [&] (int id) {
      transcript.push_back(indices[id]);
      if(indices[id] == -1) {
        indices[id] = isize(q);
        q.push_back(id);
        qty++;
        }
      };
    
    visit(0);
    for(int x=0; x<isize(q); x++) {
      int at = q[x];
      visit(gmul(at, i));
      visit(gmul(at, j));
      }
    
    transcript.push_back(qty);
    
    return transcript;
    };
  
  DEBB(DF_FIELD, ("looking for alternate solutions"));
  auto orig_transcript = compute_transcript(1, S7);
  
  set<vector<int>> transcripts_seen;
  transcripts_seen.insert(orig_transcript);
  
  set<int> conjugacy_classes;
  vector<int> cc;
  
  for(int i=0; i<N; i++) conjugacy_classes.insert(i);
  for(int i=0; i<N; i++) {
    if(!conjugacy_classes.count(i)) continue;
    vector<int> removals;
    for(int j=0; j<N; j++) {
      int c = gmul(inverses[j], gmul(i, j));
      if(c > i) removals.push_back(c);
      }
    for(auto r: removals) conjugacy_classes.erase(r);
    cc.push_back(i);
    }    
  
  DEBB(DF_FIELD, ("conjugacy_classes = ", cc));
  
  vector<triplet_info> tinf;
  triplet_info ti;
  ti.i = 1; ti.j = S7; ti.size = orig_transcript.back();
  tinf.push_back(ti);
  
  for(int i: conjugacy_classes) if(gorder(i) == S7) {
    DEBB(DF_FIELD, ("checking i=", i));
    for(int j=1; j<N; j++) if(gorder(j) == 2 && gorder(gmul(i, j)) == S3) {
      auto t = compute_transcript(i, j);    
      if(!transcripts_seen.count(t)) {
        transcripts_seen.insert(t);
        triplet_info ti;
        ti.i = i; ti.j = j; ti.size = t.back();
        tinf.push_back(ti);
        }
      }
    }
  
  DEBB(DF_FIELD, ("solutions found = ", isize(transcripts_seen)));
  return tinf;
  }

void fpattern::build() {

  if(WDIM == 3) return;
  
  for(int i=0; i<isize(qpaths); i++) {
    matrix M = strtomatrix(qpaths[i]);
    qcoords.push_back(M);
    printf("Solved %s as matrix of order %d\n", qpaths[i].c_str(), order(M));
    }
  
  matcode.clear(); matrices.clear();
  add(Id);
  if(isize(matrices) != local_group) { printf("Error: rotation crash #1 (%d)\n", isize(matrices)); exit(1); }
  
  connections.clear();
  
  for(int i=0; i<(int)matrices.size(); i++) {
  
    matrix M = matrices[i];
    
    matrix PM = mmul(P, M);
    
    add(PM);

    if(isize(matrices) % local_group) { printf("Error: rotation crash (%d)\n", isize(matrices)); exit(1); }
    
    if(!matcode.count(PM)) { printf("Error: not marked\n"); exit(1); }

    connections.push_back(matcode[PM]);
    }

  DEBB(DF_FIELD, ("Computing inverses...\n"));
  int N = isize(matrices);

  DEBB(DF_FIELD, ("Number of heptagons: %d\n", N));
  
  if(WDIM == 3) return;

  rrf.resize(N); rrf[0] = S7-1;
  for(int i=0; i<N; i++) 
    rrf[btspin(i,1)] = btspin(rrf[i], 1),
    rrf[connections[i]] = connections[rrf[i]];

  rpf.resize(N); rpf[0] = S7;
  for(int i=0; i<N; i++) 
    rpf[btspin(i,1)] = btspin(rpf[i], 1),
    rpf[connections[i]] = connections[rpf[i]];

  inverses.resize(N);
  inverses[0] = 0;
  for(int i=0; i<N; i++) // inverses[i] = gpow(i, N-1);
    inverses[btspin(i,1)] = rrf[inverses[i]], // btspin(inverses[i],6), 
    inverses[connections[i]] = rpf[inverses[i]];
  
  int errs = 0;
  for(int i=0; i<N; i++) if(gmul(i, inverses[i])) errs++;
  if(errs) printf("errs = %d\n", errs);
    
  if(0) for(int i=0; i<isize(matrices); i++) {
    printf("%5d/%4d", connections[i], inverses[i]);
    if(i%S7 == S7-1) printf("\n");       
    }
  
  DEBB(DF_FIELD, ("triplet_id = ", triplet_id, " N = ", N));
  if(triplet_id) {  
    auto triplets = find_triplets();
    if(triplet_id >= 0 && triplet_id < isize(triplets)) {
      auto ti = triplets[triplet_id];
      R = matrices[ti.i];
      P = matrices[ti.j];
      dynamicval<int> t(triplet_id, 0);
      build();
      DEBB(DF_FIELD, ("triplet built successfully"));
      return;
      }
    }
  
  DEBB(DF_FIELD, ("Built.\n"));
  }

int fpattern::getdist(pair<int,bool> a, vector<char>& dists) {
  if(!a.second) return dists[a.first];
  int m = MAXDIST;
  int ma = dists[a.first];
  int mb = dists[connections[btspin(a.first, 3)]];
  int mc = dists[connections[btspin(a.first, 4)]];
  m = min(m, 1 + ma);
  m = min(m, 1 + mb);
  m = min(m, 1 + mc);
  if(m <= 2 && ma+mb+mc <= m*3-2) return m-1; // special case
  m = min(m, 2 + dists[connections[btspin(a.first, 2)]]);
  m = min(m, 2 + dists[connections[btspin(a.first, 5)]]);
  m = min(m, 2 + dists[connections[btspin(connections[btspin(a.first, 3)], 5)]]);
  return m;
  }

int fpattern::getdist(pair<int,bool> a, pair<int,bool> b) {
  if(a.first == b.first) return a.second == b.second ? 0 : 1;
  if(b.first) a.first = gmul(a.first, inverses[b.first]), b.first = 0;
  return getdist(a, b.second ? disthex : disthep);
  }
 
int fpattern::dijkstra(vector<char>& dists, vector<int> indist[MAXDIST]) {
  int N = isize(matrices);
  dists.resize(N);
  for(int i=0; i<N; i++) dists[i] = MAXDIST-1;
  int maxd = 0;
  for(int i=0; i<MAXDIST; i++) while(!indist[i].empty()) {
    int at = indist[i].back();
    indist[i].pop_back();
    if(dists[at] <= i) continue;
    maxd = i;
    dists[at] = i;
    int lg = MWDIM == 4 ? local_group : S7;
    for(int q=0; q<lg; q++) {
      dists[at] = i;
      if(WDIM == 3)
        indist[i+1].push_back(gmul(at, local_group));
      else if(PURE) // todo-variation: PURE here?
        indist[i+1].push_back(connections[at]);
      else {
        indist[i+2].push_back(connections[at]);
        indist[i+3].push_back(connections[btspin(connections[at], 2)]);
        }
      at = groupspin(at, 1, lg);
      }
    }
  return maxd;
  }

void fpattern::analyze() {

  if(MWDIM == 4) {
    /* we need to compute inverses */
    int N = isize(matrices);
    inverses.resize(N);
    for(int i=0; i<N; i++) {
      matrix M = matrices[i];
      matrix M2 = mpow(M, N-1);
      inverses[i] = matcode[M2];
      }
    }
    
  DEBB(DF_FIELD, ("variation = %d\n", int(variation)));
  int N = isize(connections);
  
  markers.resize(N);
  
  vector<int> indist[MAXDIST];

  indist[0].push_back(0);
  int md0 = dijkstra(disthep, indist);
  
  if(MWDIM == 4) return;

  indist[1].push_back(0);
  indist[1].push_back(connections[3]);
  indist[1].push_back(connections[4]);
  indist[2].push_back(connections[btspin(connections[3], 5)]);
  indist[2].push_back(connections[2]);
  indist[2].push_back(connections[5]);
  int md1 = dijkstra(disthex, indist);
  
  maxdist = max(md0, md1);

  otherpole = 0;
  
  for(int i=0; i<N; i+=S7) {
    int mp = 0;
    for(int q=0; q<S7; q++) if(disthep[connections[i+q]] < disthep[i]) mp++;
    if(mp == S7) {
      bool eq = true;
      for(int q=0; q<S7; q++) if(disthep[connections[i+q]] != disthep[connections[i]]) eq = false;
      if(eq) {
        // for(int q=0; q<S7; q++) printf("%3d", disthep[connections[i+q]]);
        // printf(" (%2d) at %d\n", disthep[i], i);
        if(disthep[i] > disthep[otherpole]) otherpole = i;
        // for(int r=0; r<S7; r++) {
        // printf("Matrix: "); for(int a=0; a<3; a++) for(int b=0; b<3; b++)
        //    printf("%4d", matrices[i+r][a][b]); printf("\n");
        //  }
        }
      }
    }

  circrad = 99;

  for(int i=0; i<N; i++) for(int u=2; u<4; u++) if(disthep[i] < circrad)
    if(disthep[connections[i]] < disthep[i] && disthep[connections[btspin(i,u)]] < disthep[i])
      circrad = disthep[i];

  DEBB(DF_FIELD, ("maxdist = %d otherpole = %d circrad = %d\n", maxdist, otherpole, circrad));
  
  matrix PRRR = strtomatrix("PRRR");
  matrix PRRPRRRRR = strtomatrix("PRRPRRRRR");
  matrix PRRRP = strtomatrix("PRRRP");
  matrix PRP = strtomatrix("PRP");
  matrix PR = strtomatrix("PR");
  matrix Wall = strtomatrix("RRRPRRRRRPRRRP");

  wallorder = order(Wall);
  wallid = matcode[Wall];
  
  DEBB(DF_FIELD, ("wall order = %d\n", wallorder));

#define SETDIST(X, d, it) {int c = matcode[X]; indist[d].push_back(c); if(it == itNone) ; else if(markers[c] && markers[c] != it) markers[c] = itBuggy; else markers[c] = it; }
  
  matrix W = Id;
  for(int i=0; i<wallorder; i++) {
    SETDIST(W, 0, itAmethyst)
    W = mmul(W, Wall);
    }
  W = P;
  for(int i=0; i<wallorder; i++) {
    SETDIST(W, 0, itEmerald)
    W = mmul(W, Wall);
    }
  
  int walldist = dijkstra(distwall, indist);
  DEBB(DF_FIELD, ("wall dist = %d\n", walldist));
  
  
  W = strtomatrix("RRRRPR");
  for(int j=0; j<wallorder; j++) {
    W = mmul(W, Wall);
    for(int i=0; i<wallorder; i++) {
      SETDIST(W, 0, itNone)
      SETDIST(mmul(PRRR, W), 1, itNone)
      W = mmul(Wall, W);
      }
    }
  dijkstra(distwall2, indist);
  
  int rpushid = matcode[PRRPRRRRR];
  riverid = 0;
  
  for(int i=0; i<N; i++) {
    int j = i;
    int ipush = gmul(rpushid, i);
    for(int k=0; k<wallorder; k++) {
      if(ipush == j) {
        DEBB(DF_FIELD, ("River found at %d:%d\n", i, k));
        riverid = i;
        goto riveridfound;
        }
      j = gmul(j, wallid);
      }
    }
  
  riveridfound: ;

  W = strtomatrix("RRRRPR");
  for(int j=0; j<wallorder; j++) {
    W = mmul(W, Wall);
    for(int i=0; i<wallorder; i++) {
      if(i == 7) SETDIST(W, 0, itCoast)
      if(i == 3) SETDIST(mmul(PRRRP, W), 0, itWhirlpool)
      W = mmul(Wall, W);
      }
    }
  dijkstra(PURE ? distriver : distflower, indist);
  
  W = matrices[riverid];
  for(int i=0; i<wallorder; i++) {
    SETDIST(W, 0, itStatue)
    W = mmul(W, Wall);
    }
  W = mmul(P, W);
  for(int i=0; i<wallorder; i++) {
    SETDIST(W, 0, itSapphire)
    W = mmul(W, Wall);
    }
  W = mmul(PRP, matrices[riverid]);
  for(int i=0; i<wallorder; i++) {
    SETDIST(W, 1, itShard)
    W = mmul(W, Wall);
    }
  W = mmul(PR, matrices[riverid]);
  for(int i=0; i<wallorder; i++) {
    SETDIST(W, 1, itGold)
    W = mmul(W, Wall);
    }
  int riverdist = dijkstra(PURE ? distflower : distriver, indist);
  DEBB(DF_FIELD, ("river dist = %d\n", riverdist));
  
  for(int i=0; i<isize(matrices); i++)
    if(distflower[i] == 0) {
      distflower0 = inverses[i]+1;
      break;
      }
  
  if(!PURE) {
    W = matrices[riverid];
    for(int i=0; i<wallorder; i++) {
      SETDIST(W, 0, itStatue)
      W = mmul(W, Wall);
      }
    W = mmul(PR, matrices[riverid]);
    for(int i=0; i<wallorder; i++) {
      SETDIST(W, 0, itGold)
      W = mmul(W, Wall);
      }
    W = mmul(P, matrices[riverid]);
    for(int i=0; i<wallorder; i++) {
      SETDIST(W, 1, itSapphire)
      W = mmul(W, Wall);
      }
    dijkstra(distriverleft, indist);
    W = mmul(PRP, matrices[riverid]);
    for(int i=0; i<wallorder; i++) {
      SETDIST(W, 0, itShard)
      W = mmul(W, Wall);
      }
    W = mmul(P, matrices[riverid]);
    for(int i=0; i<wallorder; i++) {
      SETDIST(W, 0, itSapphire)
      W = mmul(W, Wall);
      }
    W = matrices[riverid];
    for(int i=0; i<wallorder; i++) {
      SETDIST(W, 1, itStatue)
      W = mmul(W, Wall);
      }
    dijkstra(distriverright, indist);
    }
  else {
    W = strtomatrix("RRRRPR");
    for(int j=0; j<wallorder; j++) {
      W = mmul(W, Wall);
      for(int i=0; i<wallorder; i++) {
        if(i == 7) SETDIST(W, 0, itCoast)
        W = mmul(Wall, W);
        }
      }
    dijkstra(distriverleft, indist);
    W = strtomatrix("RRRRPR");
    for(int j=0; j<wallorder; j++) {
      W = mmul(W, Wall);
      for(int i=0; i<wallorder; i++) {
        if(i == 3) SETDIST(mmul(PRRRP, W), 0, itWhirlpool)
        W = mmul(Wall, W);
        }
      }
    dijkstra(distriverright, indist);
    }

  DEBB(DF_FIELD, ("wall-river distance = %d\n", distwall[riverid]));
  DEBB(DF_FIELD, ("river-wall distance = %d\n", distriver[0]));
  }

int fpattern::orderstats() {
  int N = isize(matrices);

  #define MAXORD 10000
  int ordcount[MAXORD];
  int ordsample[MAXORD];
  
  for(int i=0; i<MAXORD; i++) ordcount[i] = 0;

  for(int i=0; i<N; i++) {
    int cnt = order(matrices[i]);

    if(cnt < MAXORD) {
      if(!ordcount[cnt]) ordsample[cnt] = i;
      ordcount[cnt]++;
      }
    }
  
  printf("Listing:\n");
  for(int i=0; i<MAXORD; i++) if(ordcount[i])
    printf("Found %4d matrices of order %3d: %s\n", ordcount[i], i, decodepath(ordsample[i]).c_str());
  
  return ordsample[Prime];
  }

void fpattern::findsubpath() {
  int N = isize(matrices);
  for(int i=1; i<N; i++)
    if(gpow(i, Prime) == 0) {
      subpathid = i;
      subpathorder = Prime;
      DEBB(DF_FIELD, ("Subpath found: %s\n", decodepath(i).c_str()));
      return;
      }
  }

fpattern *fp43;

EX void info() {
  fpattern fp(0);
  int cases = 0, hard = 0;
  for(int p=0; p<500; p++) {
    fp.Prime = p;
    if(fp.solve() == 0) {
      printf("%4d: wsquare=%d cs=%d sn=%d ch=%d sh=%d dual=%d\n",
        p, fp.wsquare, fp.cs, fp.sn, fp.ch, fp.sh, fp.dual);
      cases++;
      if(!fp.easy(fp.cs) || !fp.easy(fp.sn) || !fp.easy(fp.ch) || !fp.easy(fp.sn))
        hard++;
      #ifndef EASY
      neasy = 0; 
      #endif
      if(WDIM == 3) continue;
      fp.build();
      #ifndef EASY
      printf("Not easy: %d\n", neasy);
      #endif
      int N = isize(fp.matrices);
      int left = N / fp.Prime;
      printf("Prime decomposition: %d = %d", N, fp.Prime);
      for(int p=2; p<=left; p++) while(left%p == 0) printf("*%d", p), left /= p;
      printf("\n");
      printf("Order of RRP is: %d\n", fp.order(fp.strtomatrix("RRP")));
      printf("Order of RRRP is: %d\n", fp.order(fp.strtomatrix("RRRP")));
      printf("Order of RRRPRRRRRPRRRP is: %d\n", fp.order(fp.strtomatrix("RRRPRRRRRPRRRP")));
      }                                  
    }
  printf("cases found = %d (%d hard)\n", cases, hard);
  }

EX fpattern current_quotient_field = fpattern(0);
EX fpattern fp_invalid = fpattern(0);
EX bool quotient_field_changed;

// these strings contain \x00
#define STR(x) string(x, sizeof(x))

EX struct fpattern& getcurrfp() {
  if(fake::in()) return *FPIU(&getcurrfp());
  if(geometry == gFieldQuotient && quotient_field_changed)
    return current_quotient_field;
  if(geometry == gSpace535) {
    // 120 cells, hash = 9EF7A9C4
    static fpattern fp(0);
    if(use_rule_fp) {
      fp.Prime = 5; fp.force_hash = 0xDCC3CACEu; fp.solve();
      }
    else {
      fp.Prime = 5; fp.force_hash = 0x9EF7A9C4u; fp.solve();
      }
    return fp;
    }
  if(geometry == gSpace534) {
    // 260 cells, hash = 72414D0C (not 0C62E214)
    static fpattern fp(0);
    if(fp.Prime) return fp;
    // fp.Prime = 5; fp.force_hash = 0x72414D0C; fp.solve();
    
    if(use_rule_fp) {
      fp.Prime = 11; fp.force_hash = 0x5FC4CFF0u; fp.solve();
      }
    else {
      shstream ins(STR("\x05\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00\xfc\xff\xff\xff\x01\x00\x00\x00\x04\x00\x00\x00\xfc\xff\xff\xff\x04\x00\x00\x00\xfe\xff\xff\xff\x00\x00\x00\x00\x01\x00\x00\x00\xfe\xff\xff\xff\x04\x00\x00\x00\x02\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x04\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\xfc\xff\xff\xff\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\xfc\xff\xff\xff\x02\x00\x00\x00\x00\x00\x00\x00\xfc\xff\xff\xff\x01\x00\x00\x00\xfd\xff\xff\xff\x00\x00\x00\x00\x02\x00\x00\x00\xfd\xff\xff\xff\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00"));
      hread_fpattern(ins, fp);
      }
    return fp;
    }
  if(geometry == gSpace435) {
    // 650 cells, hash = EB201050
    static fpattern fp(0);
    if(fp.Prime) return fp;
    // fp.Prime = 5; fp.force_hash = 0xEB201050; fp.solve();
    // what is 0x72414D0C??
    
    if(use_rule_fp) {
      fp.Prime = 11; fp.force_hash = 0x65CE0C00u; fp.solve();
      }
    else {
      shstream ins(STR("\x05\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\xfc\xff\xff\xff\xff\xff\xff\xff\xfe\xff\xff\xff\xfc\xff\xff\xff\x04\x00\x00\x00\x02\x00\x00\x00\x04\x00\x00\x00\xff\xff\xff\xff\x02\x00\x00\x00\x03\x00\x00\x00\x03\x00\x00\x00\xfd\xff\xff\xff\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\xfc\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\xfd\xff\xff\xff\xfd\xff\xff\xff\x00\x00\x00\x00\xfd\xff\xff\xff\x02\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\xfd\xff\xff\xff\x03\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00"));
      hread_fpattern(ins, fp);
      }
    return fp;
    }
  if(geometry == gSpace436) {
    static fpattern fp(0);
    if(fp.Prime) return fp;
    if(use_rule_fp) {
      fp.Prime = 2; fp.force_hash = 0x235F7508u; fp.solve();
      }
    else {
      // FF82A214
      shstream ins(STR("\x05\x00\x00\x00\x02\x00\x00\x00\x01\x00\x00\x00\xfd\xff\xff\xff\x00\x00\x00\x00\xfe\xff\xff\xff\xfd\xff\xff\xff\x01\x00\x00\x00\x01\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x04\x00\x00\x00\xfd\xff\xff\xff\x02\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x02\x00\x00\x00\xfc\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\xfd\xff\xff\xff\xfd\xff\xff\xff\x00\x00\x00\x00\xfd\xff\xff\xff\x02\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\xfd\xff\xff\xff\x03\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00"));
      hread_fpattern(ins, fp);
      }
    return fp;
    }
  if(geometry == gSpace336) {
    static fpattern fp(0);
    if(fp.Prime) return fp;
    if(use_rule_fp) {
      fp.Prime = 3; fp.force_hash = 0xD29C2418u; fp.solve();
      }
    else {
      // fp.Prime = 7; fp.force_hash = 0xE3F6B7BCu; fp.solve();
      shstream ins(STR("\x07\x00\x00\x00\x03\x00\x00\x00\xfa\xff\xff\xff\x02\x00\x00\x00\x03\x00\x00\x00\x06\x00\x00\x00\x02\x00\x00\x00\xfe\xff\xff\xff\xfb\xff\xff\xff\xfc\xff\xff\xff\x03\x00\x00\x00\xfb\xff\xff\xff\xfd\xff\xff\xff\xfb\xff\xff\xff\x01\x00\x00\x00\xfd\xff\xff\xff\xfe\xff\xff\xff\xfd\xff\xff\xff\x03\x00\x00\x00\x00\x00\x00\x00\xfd\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xfc\xff\xff\xff\x00\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\xfa\xff\xff\xff\xfb\xff\xff\xff\x00\x00\x00\x00\xfa\xff\xff\xff\x02\x00\x00\x00\x06\x00\x00\x00\x00\x00\x00\x00\xfb\xff\xff\xff\x06\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00"));
      hread_fpattern(ins, fp);
      }
    return fp;
    }
  if(geometry == gSpace344) {
    // 600 cells in 558C8ED0
    // 2400 cells in AF042EA8 
    // 2600 cells in D26948E0 
    // 2600 cells in EC29DCEC 
    static fpattern fp(0);
    if(fp.Prime) return fp;
    if(use_rule_fp) {
      fp.Prime = 3; fp.force_hash = 0xB23AF1F4u; fp.solve();
      }
    else {
      fp.Prime = 5; fp.force_hash = 0x558C8ED0u; fp.solve();
      }
    return fp;
    // 4900 cells in CDCC7860 (7)
    }
  if(geometry == gSpace536) {
    static fpattern fp(0);
    if(fp.Prime) return fp;
    // 130 cells in 3BA5C5A4
    // 260 cells in 9FDE7B38
    if(use_rule_fp) {
      fp.Prime = 5; fp.force_hash = 0x61385498u; fp.solve();
      }
    else {
      fp.Prime = 5; fp.force_hash = 0x9FDE7B38u; fp.solve();
      }
    return fp;
    }
  if(geometry == gSpace345) {
    static fpattern fp(0);
    if(fp.Prime) return fp;
    // 30 cells in 02ADCAA4 (3^2)
    // 650 cells in 7EFE8D98 (5^2)
    // 55 cells in F447F75C (11)
    if(use_rule_fp) {
      fp.Prime = 3; fp.force_hash = 0xF978E264u; fp.solve();
      }
    else {
      fp.Prime = 11; fp.force_hash = 0xF447F75Cu; fp.solve();
      }
    return fp;
    }
  if(geometry == gSpace353) {
    static fpattern fp(0);
    if(fp.Prime) return fp;
    // 130 cells in 1566EBAC (5^2)
    // 11 cells in 5A2E2B88 (11)
    fp.Prime = 11; fp.force_hash = 0x5A2E2B88u; fp.solve();
    return fp;
    }
  if(geometry == gSpace354) {
    static fpattern fp(0);
    // fp.Prime = 11; fp.force_hash = 0x363D8DA4u; fp.solve();
    fp.Prime = 5; fp.force_hash = 0x58A8E850u; fp.solve();
    return fp;
    }
  if(geometry == gCubeTiling) {
    static fpattern fp(2);
    return fp;
    }
  if(!hyperbolic) return fp_invalid;
  if(WDIM == 3 && !quotient && !mhybrid && !bt::in()) {
    static fpattern fp(0);
    if(fp.Prime) return fp;
    for(int p=2; p<20; p++) { fp.Prime = p; if(!fp.solve()) break; }
    DEBB(DF_FIELD, ("set prime = ", fp.Prime));
    return fp;
    }
  if(S7 == 8 && S3 == 3 && !bt::in()) {
    static fpattern fp(17);
    return fp;
    }
  if(S7 == 5 && S3 == 4 && !bt::in()) {
    static fpattern fp(11);
    return fp;
    }
  if(S7 == 6 && S3 == 4 && !bt::in()) {
    static fpattern fp(13);
    return fp;
    }
  if(S7 == 7 && S3 == 4 && !bt::in()) {
    static fpattern fp(13);
    return fp;
    }
  if(sphere || euclid) return fp_invalid;
  if(S7 == 7 && S3 == 3 && !bt::in()) {
    if(!fp43) fp43 = new fpattern(43);
    return *fp43;
    }
  return fp_invalid;
  }

#undef STR

// todo undefined behavior
EX int subpathid = -1; 
EX int subpathorder = -1; 

// extra information for field quotient extra configuration

EX vector<fgeomextra> fgeomextras = {
  fgeomextra(gNormal, 4),
  fgeomextra(gOctagon, 1),
  fgeomextra(g45, 1),
  fgeomextra(g46, 5),
  fgeomextra(g47, 1),
  fgeomextra(gSchmutzM3, 0),
/*  fgeomextra(gSphere, 0),
  fgeomextra(gSmallSphere, 0), -> does not find the prime
  fgeomextra(gEuclid, 0),
  fgeomextra(gEuclidSquare, 0),
  fgeomextra(gTinySphere, 0) */
  };

EX int current_extra = 0;

EX void nextPrime(fgeomextra& ex) {
  dynamicval<eGeometry> g(geometry, ex.base);
  dynamicval<int> t(triplet_id, 0);
  int nextprime;
  if(isize(ex.primes))
    nextprime = ex.primes.back().p + 1;
  else
    nextprime = 2;
  while(true) {
    fieldpattern::fpattern fp(0);
    fp.Prime = nextprime;
    if(fp.solve() == 0) {
      fp.build();
      int cells = isize(fp.matrices) / S7;
      ex.primes.emplace_back(primeinfo{nextprime, cells, (bool) fp.wsquare});
      ex.dualval.emplace_back(fp.dual);
      break;
      }
    nextprime++;
    }
  }

EX void nextPrimes(fgeomextra& ex) {
  while(isize(ex.primes) < 6) 
    nextPrime(ex);
  }

EX void enableFieldChange() {
  fgeomextra& gxcur = fgeomextras[current_extra];
  fieldpattern::quotient_field_changed = true;
  nextPrimes(gxcur);
  dynamicval<eGeometry> g(geometry, gFieldQuotient);
  ginf[geometry].g = ginf[gxcur.base].g;
  ginf[geometry].sides = ginf[gxcur.base].sides;
  ginf[geometry].vertex = ginf[gxcur.base].vertex;
  ginf[geometry].distlimit = ginf[gxcur.base].distlimit;
  ginf[geometry].tiling_name = ginf[gxcur.base].tiling_name;
  ginf[geometry].default_variation = ginf[gxcur.base].default_variation;
  ginf[geometry].flags = qFIELD | qANYQ | qCLOSED;
  fieldpattern::current_quotient_field.init(gxcur.primes[gxcur.current_prime_id].p);
  }

EX eGeometry underlying_geometry;

EX void field_from_current() {
  auto& go = ginf[geometry];
  underlying_geometry = geometry;
  dynamicval<eGeometry> g(geometry, gFieldQuotient);
  auto& gg = ginf[geometry];
  gg.sides = go.sides;
  gg.vertex = go.vertex;
  gg.distlimit = go.distlimit;
  gg.tiling_name = go.tiling_name;
  gg.flags = go.flags | qANYQ | qFIELD | qCLOSED;
  gg.g = go.g;
  gg.default_variation = go.default_variation;
  fieldpattern::quotient_field_changed = true;  
  }

#if CAP_THREAD && MAXMDIM >= 4
EX map<string, discovery> discoveries;

void discovery::activate() {
  if(!discoverer) {
    discoverer = std::make_shared<std::thread> ( [this] {
      for(int p=2; p<100; p++) {
        experiment.Prime = p;
        experiment.solve();
        if(stop_it) break;
        }
      });
    }
  if(is_suspended) {
    if(1) {
      std::unique_lock<std::mutex> lk(lock);
      is_suspended = false;
      }
    cv.notify_one();
    }
  }

void discovery::discovered() {
  std::unique_lock<std::mutex> lk(lock);
  auto& e = experiment;
  hashes_found[e.hashv] = make_tuple(e.Prime, e.wsquare, e.R, e.P, e.X, isize(e.matrices) / e.local_group);
  }

void discovery::suspend() { is_suspended = true; }

void discovery::check_suspend() { 
  std::unique_lock<std::mutex> lk(lock);
  if(is_suspended) cv.wait(lk, [this] { return !is_suspended; });
  }

void discovery::schedule_destruction() { stop_it = true; }
discovery::~discovery() { schedule_destruction(); if(discoverer) discoverer->join(); }
#endif

int hk = 
#if CAP_THREAD
#if MAXMDIM >= 4
  + addHook(hooks_on_geometry_change, 100, [] { for(auto& d:discoveries) if(!d.second.is_suspended) d.second.suspend(); })
  + addHook(hooks_final_cleanup, 100, [] { 
      for(auto& d:discoveries) { d.second.schedule_destruction(); if(d.second.is_suspended) d.second.activate(); }
      discoveries.clear();
      })
#endif
#endif
#if CAP_COMMANDLINE
  + addHook(hooks_args, 0, [] {
      using namespace arg;
      if(0) ;
      else if(argis("-q3-limitsq")) { shift(); limitsq = argi(); }
      else if(argis("-q3-limitp")) { shift(); limitp = argi(); }
      else if(argis("-q3-limitv")) { shift(); limitv = argi(); }
      else return 1;
      return 0;
      })
#endif
  + 0;

EX purehookset hooks_on_geometry_change;

EX int field_celldistance(cell *c1, cell *c2) {
  if(geometry != gFieldQuotient) return DISTANCE_UNKNOWN;
  if(GOLDBERG) return DISTANCE_UNKNOWN;
  auto v1 =fieldpattern::fieldval(c1);
  auto v2 =fieldpattern::fieldval(c2);
  int d = currfp.getdist(v1, v2);
  return d;
  }

EX }

#define currfp fieldpattern::getcurrfp()

EX int currfp_gmul(int a, int b) { return currfp.gmul(a,b); }
EX int currfp_inverses(int i) { return currfp.inverses[i]; }
EX int currfp_distwall(int i) { return currfp.distwall[i]; }
EX int currfp_n() { return isize(currfp.matrices); }
EX int currfp_get_R() { return currfp.matcode[currfp.R]; }
EX int currfp_get_P() { return currfp.matcode[currfp.P]; }
EX int currfp_get_X() { return currfp.matcode[currfp.X]; }

EX void hread_fpattern(hstream& hs, fieldpattern::fpattern& fp) {
  hread(hs, fp.Prime);
  hread(hs, fp.wsquare);
  hread(hs, fp.P);
  hread(hs, fp.R);
  hread(hs, fp.X);
  fp.set_field(fp.Prime, fp.wsquare);
  #if MAXMDIM >= 4
  fp.generate_all3();
  #endif
  }

EX void hwrite_fpattern(hstream& hs, fieldpattern::fpattern& fp) {
  hwrite(hs, fp.Prime);
  hwrite(hs, fp.wsquare);
  hwrite(hs, fp.P);
  hwrite(hs, fp.R);
  hwrite(hs, fp.X);
  }

}
#endif
