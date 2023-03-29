// Hyperbolic Rogue -- Kite-and-dart tiling
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file apeirodic-hat.cpp
 *  \brief Apeirodic Hat tiling, based on https://arxiv.org/pdf/2303.10798.pdf
 */

#include "hyper.h"
namespace hr {

EX namespace hat {

EX bool in() { return cgflags & qHAT; }

struct rule_base {
  int id0, edge0, id1, edge1, master_connection;
  };

struct rule_recursive {
  int id0, id1, child, parent, rev_child;
  };

vector<rule_base> rules_base = {
  rule_base{0, 0, 4, 6, -1},
  {0, 10, 2, 6, -1},
  {0, 11, 2, 5, -1},
  {0, 12, 4, 8, -1},
  {0, 13, 4, 7, -1},
  {0, 1, 4, 5, -1},
  {0, 2, 5, 6, -1},
  {0, 3, 5, 5, -1},
  {0, 4, 1, 0, -1},
  {0, 5, 1, 13, -1},
  {0, 6, 1, 12, -1},
  {0, 7, 1, 11, -1},
  {0, 8, 2, 8, -1},
  {0, 9, 2, 7, -1},
  {1, 0, 0, 4, -1},
  {1, 10, 2, 9, -1},
  {1, 11, 0, 7, -1},
  {1, 12, 0, 6, -1},
  {1, 13, 0, 5, -1},
  {1, 1, 6, 6, -1},
  {1, 2, 6, 5, -1},
  {1, 3, 6, 4, -1},
  {1, 4, 2, 13, 24},
  {1, 4, 3, 13, 33},
  {1, 4, 4, 13, 8},
  {1, 4, 5, 13, 13},
  {1, 4, 6, 13, 18},
  {1, 4, 7, 13, 10},
  {1, 5, 2, 12, 24},
  {1, 5, 3, 12, 33},
  {1, 5, 4, 12, 8},
  {1, 5, 5, 12, 13},
  {1, 5, 6, 12, 18},
  {1, 5, 7, 12, 10},
  {1, 6, 2, 11, 24},
  {1, 6, 3, 11, 33},
  {1, 6, 4, 11, 8},
  {1, 6, 5, 11, 13},
  {1, 6, 6, 11, 18},
  {1, 6, 7, 11, 10},
  {1, 7, 3, 0, 28},
  {1, 7, 3, 4, 8},
  {1, 7, 6, 0, 17},
  {1, 7, 7, 0, 32},
  {1, 8, 3, 13, 28},
  {1, 8, 3, 3, 8},
  {1, 8, 6, 13, 17},
  {1, 8, 7, 13, 32},
  {1, 9, 3, 12, 28},
  {1, 9, 3, 2, 8},
  {1, 9, 6, 12, 17},
  {1, 9, 7, 12, 32},
  {2, 0, 2, 13, 21},
  {2, 0, 3, 13, 23},
  {2, 0, 5, 13, 31},
  {2, 0, 6, 3, 22},
  {2, 10, 3, 11, 28},
  {2, 10, 3, 1, 8},
  {2, 10, 6, 11, 17},
  {2, 10, 7, 11, 32},
  {2, 11, 1, 6, 22},
  {2, 11, 3, 10, 28},
  {2, 11, 6, 10, 17},
  {2, 11, 7, 10, 32},
  {2, 1, 2, 12, 21},
  {2, 12, 1, 5, 22},
  {2, 12, 2, 1, 28},
  {2, 12, 3, 1, 32},
  {2, 12, 5, 1, 17},
  {2, 1, 3, 12, 23},
  {2, 13, 1, 4, 22},
  {2, 13, 2, 0, 28},
  {2, 13, 3, 0, 32},
  {2, 13, 5, 0, 17},
  {2, 1, 5, 12, 31},
  {2, 1, 6, 2, 22},
  {2, 2, 3, 9, -1},
  {2, 3, 3, 8, -1},
  {2, 4, 3, 7, -1},
  {2, 5, 0, 11, -1},
  {2, 6, 0, 10, -1},
  {2, 7, 0, 9, -1},
  {2, 8, 0, 8, -1},
  {2, 9, 1, 10, -1},
  {3, 0, 1, 7, 21},
  {3, 0, 2, 13, 15},
  {3, 0, 5, 13, 27},
  {3, 0, 6, 3, 16},
  {3, 0, 7, 3, 31},
  {3, 10, 2, 11, 21},
  {3, 10, 3, 11, 23},
  {3, 10, 5, 11, 31},
  {3, 10, 6, 1, 22},
  {3, 11, 1, 6, 16},
  {3, 11, 2, 10, 21},
  {3, 11, 3, 10, 23},
  {3, 11, 5, 10, 31},
  {3, 1, 2, 10, 11},
  {3, 1, 2, 12, 15},
  {3, 12, 1, 5, 16},
  {3, 12, 1, 9, 21},
  {3, 12, 2, 1, 23},
  {3, 12, 4, 1, 31},
  {3, 13, 1, 4, 16},
  {3, 13, 1, 8, 21},
  {3, 13, 2, 0, 23},
  {3, 13, 4, 0, 31},
  {3, 1, 5, 12, 27},
  {3, 1, 6, 2, 16},
  {3, 1, 7, 2, 31},
  {3, 2, 1, 9, 11},
  {3, 2, 7, 9, -1},
  {3, 3, 1, 8, 11},
  {3, 3, 7, 8, -1},
  {3, 4, 1, 7, 11},
  {3, 4, 7, 7, -1},
  {3, 5, 4, 10, -1},
  {3, 6, 4, 9, -1},
  {3, 7, 2, 4, -1},
  {3, 8, 2, 3, -1},
  {3, 9, 2, 2, -1},
  {4, 0, 3, 13, 14},
  {4, 0, 6, 13, 26},
  {4, 0, 6, 3, 11},
  {4, 0, 7, 13, 20},
  {4, 10, 3, 5, -1},
  {4, 11, 1, 6, 11},
  {4, 11, 7, 6, -1},
  {4, 12, 1, 5, 11},
  {4, 12, 7, 5, -1},
  {4, 1, 3, 12, 14},
  {4, 13, 1, 4, 11},
  {4, 13, 7, 4, -1},
  {4, 1, 6, 12, 26},
  {4, 1, 6, 2, 11},
  {4, 1, 7, 12, 20},
  {4, 2, 5, 9, -1},
  {4, 3, 5, 8, -1},
  {4, 4, 5, 7, -1},
  {4, 5, 0, 1, -1},
  {4, 6, 0, 0, -1},
  {4, 7, 0, 13, -1},
  {4, 8, 0, 12, -1},
  {4, 9, 3, 6, -1},
  {5, 0, 2, 13, 29},
  {5, 0, 5, 13, 19},
  {5, 0, 6, 3, 30},
  {5, 10, 3, 11, 14},
  {5, 10, 6, 1, 11},
  {5, 10, 6, 11, 26},
  {5, 10, 7, 11, 20},
  {5, 11, 1, 6, 30},
  {5, 11, 3, 10, 14},
  {5, 11, 6, 10, 26},
  {5, 11, 7, 10, 20},
  {5, 1, 2, 12, 29},
  {5, 12, 1, 5, 30},
  {5, 12, 2, 1, 14},
  {5, 12, 3, 1, 20},
  {5, 12, 5, 1, 26},
  {5, 13, 1, 4, 30},
  {5, 13, 2, 0, 14},
  {5, 13, 3, 0, 20},
  {5, 13, 5, 0, 26},
  {5, 1, 5, 12, 19},
  {5, 1, 6, 2, 30},
  {5, 2, 6, 9, -1},
  {5, 3, 6, 8, -1},
  {5, 4, 6, 7, -1},
  {5, 5, 0, 3, -1},
  {5, 6, 0, 2, -1},
  {5, 7, 4, 4, -1},
  {5, 8, 4, 3, -1},
  {5, 9, 4, 2, -1},
  {6, 0, 1, 7, 29},
  {6, 0, 6, 3, 25},
  {6, 0, 7, 3, 19},
  {6, 10, 2, 11, 29},
  {6, 10, 5, 11, 19},
  {6, 10, 6, 1, 30},
  {6, 11, 1, 6, 25},
  {6, 11, 2, 10, 29},
  {6, 11, 5, 10, 19},
  {6, 12, 1, 5, 25},
  {6, 12, 1, 9, 29},
  {6, 12, 4, 1, 19},
  {6, 1, 3, 10, 24},
  {6, 13, 1, 4, 25},
  {6, 13, 1, 8, 29},
  {6, 13, 4, 0, 19},
  {6, 1, 5, 10, 8},
  {6, 1, 6, 10, 13},
  {6, 1, 6, 2, 25},
  {6, 1, 7, 10, 33},
  {6, 1, 7, 2, 19},
  {6, 2, 2, 1, 24},
  {6, 2, 3, 1, 33},
  {6, 2, 4, 1, 8},
  {6, 2, 5, 1, 13},
  {6, 2, 6, 1, 18},
  {6, 2, 7, 1, 10},
  {6, 3, 2, 0, 24},
  {6, 3, 3, 0, 33},
  {6, 3, 4, 0, 8},
  {6, 3, 5, 0, 13},
  {6, 3, 6, 0, 18},
  {6, 3, 7, 0, 10},
  {6, 4, 1, 3, -1},
  {6, 5, 1, 2, -1},
  {6, 6, 1, 1, -1},
  {6, 7, 5, 4, -1},
  {6, 8, 5, 3, -1},
  {6, 9, 5, 2, -1},
  {7, 0, 1, 7, 15},
  {7, 0, 6, 3, 12},
  {7, 0, 7, 3, 27},
  {7, 10, 2, 11, 15},
  {7, 10, 5, 11, 27},
  {7, 10, 6, 1, 16},
  {7, 10, 7, 1, 31},
  {7, 11, 1, 6, 12},
  {7, 11, 2, 10, 15},
  {7, 11, 5, 10, 27},
  {7, 12, 1, 5, 12},
  {7, 12, 1, 9, 15},
  {7, 12, 4, 1, 27},
  {7, 13, 1, 4, 12},
  {7, 13, 1, 8, 15},
  {7, 13, 4, 0, 27},
  {7, 1, 6, 2, 12},
  {7, 1, 7, 10, 14},
  {7, 1, 7, 2, 27},
  {7, 2, 3, 1, 14},
  {7, 2, 6, 1, 26},
  {7, 2, 7, 1, 20},
  {7, 3, 3, 0, 14},
  {7, 3, 6, 0, 26},
  {7, 3, 7, 0, 20},
  {7, 4, 4, 13, -1},
  {7, 5, 4, 12, -1},
  {7, 6, 4, 11, -1},
  {7, 7, 3, 4, -1},
  {7, 8, 3, 3, -1},
  {7, 9, 3, 2, -1},
  };

vector<rule_recursive> rules_recursive = {
  rule_recursive{0, 0, -1, -1, -1},
  {0, 1, 10, 18, 12},
  {0, 1, 25, -1, 18},
  {0, 1, 32, 17, 15},
  {0, 2, 10, 13, 12},
  {0, 2, 30, -1, 13},
  {0, 3, 10, 8, 12},
  {0, 3, 11, -1, 8},
  {0, 4, 10, 24, 12},
  {0, 4, 21, -1, 28},
  {0, 5, 10, 33, 12},
  {0, 5, 17, 8, 29},
  {0, 5, 32, 28, 15},
  {0, 6, 10, 10, 12},
  {0, 6, 32, 32, 15},
  {1, 0, 12, 25, 10},
  {1, 0, 15, 29, 32},
  {1, 0, 18, -1, 25},
  {1, 1, -1, -1, -1},
  {1, 1, 14, 13, 31},
  {1, 1, 19, 18, 26},
  {1, 1, 26, 25, 19},
  {1, 1, 31, 30, 14},
  {1, 2, 14, 8, 31},
  {1, 2, 17, -1, 29},
  {1, 2, 19, 13, 26},
  {1, 2, 23, 19, 23},
  {1, 3, 19, 8, 26},
  {1, 3, 27, 19, 20},
  {1, 4, 19, 24, 26},
  {1, 4, 23, 29, 23},
  {1, 5, 14, 24, 31},
  {1, 5, 19, 33, 26},
  {1, 6, 14, 33, 31},
  {1, 6, 19, 10, 26},
  {1, 6, 26, 19, 19},
  {2, 0, 12, 30, 10},
  {2, 0, 13, -1, 30},
  {2, 1, 23, 26, 23},
  {2, 1, 26, 30, 19},
  {2, 1, 29, -1, 17},
  {2, 1, 31, 11, 14},
  {2, 2, -1, -1, -1},
  {2, 2, 20, 19, 27},
  {2, 2, 27, 26, 20},
  {2, 3, 17, -1, 29},
  {2, 4, 20, 29, 27},
  {2, 4, 27, 14, 20},
  {2, 5, 23, 14, 23},
  {2, 5, 27, 20, 20},
  {2, 6, 23, 20, 23},
  {3, 0, 12, 11, 10},
  {3, 0, 8, -1, 11},
  {3, 1, 20, 26, 27},
  {3, 1, 26, 11, 19},
  {3, 2, 29, -1, 17},
  {3, 3, -1, -1, -1},
  {3, 4, 22, -1, 24},
  {3, 5, 16, -1, 33},
  {3, 5, 20, 14, 27},
  {3, 6, 12, -1, 10},
  {3, 6, 20, 20, 27},
  {4, 0, 12, 22, 10},
  {4, 0, 28, -1, 21},
  {4, 1, 23, 17, 23},
  {4, 1, 26, 22, 19},
  {4, 2, 20, 31, 27},
  {4, 2, 27, 17, 20},
  {4, 3, 24, -1, 22},
  {4, 4, -1, -1, -1},
  {4, 4, 20, 21, 27},
  {4, 4, 27, 28, 20},
  {4, 5, 20, 23, 27},
  {4, 5, 23, 28, 23},
  {4, 5, 27, 32, 20},
  {4, 5, 29, -1, 17},
  {4, 5, 31, 8, 14},
  {4, 6, 23, 32, 23},
  {5, 0, 12, 16, 10},
  {5, 0, 15, 21, 32},
  {5, 0, 29, 11, 17},
  {5, 1, 26, 16, 19},
  {5, 1, 31, 22, 14},
  {5, 2, 20, 27, 27},
  {5, 2, 23, 31, 23},
  {5, 3, 27, 31, 20},
  {5, 3, 33, -1, 16},
  {5, 4, 14, 11, 31},
  {5, 4, 17, -1, 29},
  {5, 4, 20, 15, 27},
  {5, 4, 23, 21, 23},
  {5, 4, 27, 23, 20},
  {5, 5, -1, -1, -1},
  {5, 5, 23, 23, 23},
  {5, 6, 26, 31, 19},
  {5, 6, 29, -1, 17},
  {6, 0, 12, 12, 10},
  {6, 0, 15, 15, 32},
  {6, 1, 19, 26, 26},
  {6, 1, 26, 12, 19},
  {6, 1, 31, 16, 14},
  {6, 2, 23, 27, 23},
  {6, 3, 10, -1, 12},
  {6, 3, 27, 27, 20},
  {6, 4, 23, 15, 23},
  {6, 5, 17, -1, 29},
  {6, 5, 19, 14, 26},
  {6, 6, -1, -1, -1},
  {6, 6, 14, 14, 31},
  {6, 6, 19, 20, 26},
  {6, 6, 26, 27, 19},
  {6, 6, 31, 31, 14},
  };

EX ld hat_param = 1;

struct hrmap_hat : hrmap {

  // always generate the same way
  std::mt19937 hatrng;

  int hatrand(int i) {    
    return hatrng() % i;
    }

  // cluster centers are of type 1, all the rest are of type 0 (they are mirror images)
  int shvid(cell *c) override {
    return c->master->c7 == c ? 1 : 0;
    }

  // vertices of each type of hat
  vector<hyperpoint> hatcorners[2];

  struct memo_matrix : transmatrix {
    bool known;
    transmatrix& get() { return (transmatrix&) (*this); }
    void clear() { known = false; }
    void set(const transmatrix& T) { known = true; get() = T; }
    };

  memo_matrix adj_memo[2][2][14][14];
  vector<vector<memo_matrix>> long_transformations;

  void fill_transform_levels(int lev) {
    int clev = isize(long_transformations);
    while(clev <= lev) {
      auto& lt = long_transformations;
      lt.emplace_back();
      auto& last = lt.back();
      last.resize(relations+1);
      for(auto& t: last) t.clear();
      last[0].set(Id);
      last[1].set(Id);

      while(true) {
        int chg = 0;
        int unknown = 0;
        int errors = 0;

        auto products_equal = [&] (memo_matrix& A, memo_matrix& B, memo_matrix& C, memo_matrix& D) {
          if(A.known && B.known && C.known && D.known) {
            if(!eqmatrix(A*B, C*D)) errors++;
            }
          else if(B.known && C.known && D.known) chg++, A.set( C * D * inverse(B) );
          else if(A.known && C.known && D.known) chg++, B.set( inverse(A) * C * D );
          else if(A.known && B.known && D.known) chg++, C.set( A * B * inverse(D) );
          else if(A.known && B.known && C.known) chg++, D.set( inverse(C) * A * B );
          else unknown++;
          };

        if(clev == 1) for(auto& b: rules_base) {
          products_equal(lt[0][b.id0+1], adj(b.id0==0, fix(b.edge0), b.id1==0, fix(b.edge1)), lt[1][b.master_connection+1], lt[0][b.id1+1]);
          }

        if(clev >= 2) for(auto& b: rules_recursive) {
          products_equal(lt[clev][b.id0+1], lt[clev-1][b.child+1], lt[clev][b.parent+1], lt[clev][b.id1+1]);
          }

        if(debugflags & DF_GEOM) println(hlog, "changed = ", chg, " unknown = ", unknown, " errors = ", errors);
        if(!chg) break;
        }

      clev++;
      }
    }

  void clear_adj_memo() {
    for(int a=0; a<2; a++)
    for(int b=0; b<2; b++)
    for(int c=0; c<14; c++)
    for(int d=0; d<14; d++)
      adj_memo[a][b][c][d].clear();
    }

  void init() {

    transmatrix T = Id;
    auto& hc = hatcorners[0];
    hc.clear();
    hatcorners[1].clear();

    bool f = geom3::flipped;
    bool emb = embedded_plane;
    if(emb) geom3::light_flip(true);

    auto move = [&] (ld angle, ld dist) {
      hc.push_back(T * C0);
      T = T * cspin(0, 1, angle * degree);
      T = T * xpush(dist);
      };

    ld q = 6;
    ld eshort = 0.3;
    ld elong = sqrt(3) * eshort;
    if(fake::in()) q = fake::around;

    if(q != 6) {
      eshort = edge_of_triangle_with_angles(M_PI / q, 60._deg, 90._deg);
      elong = edge_of_triangle_with_angles(60._deg, M_PI / q, 90._deg);
      }
    else {
      eshort *= hat_param;
      elong *= 2 - hat_param;
      // 0-length edges cause problems...
      if(abs(eshort) < 1e-6) eshort = .0001;
      if(abs(elong) < 1e-6) elong = .0001;
      }

    ld i60 = (M_PI - TAU*2/q)/degree;
    ld n60 = (M_PI - TAU*4/q)/degree;

    move(-90, eshort); move( 60, eshort); move(  0, eshort);
    move( 60, eshort); move( 90, elong);  move(n60, elong);
    move( 90, eshort); move(-60, eshort); move( 90, elong);
    move(i60, elong);  move(-90, eshort); move( 60, eshort);
    move( 90, elong);  move(i60, elong);

    if(q == 6) {
      ld area = 0;
      for(int i=0; i<14; i++) area += (hc[(i+1)%14] ^ hc[i]) [2];
      println(hlog, "area = ", area);
      area = abs(area);
      ld scale = sqrt(2.5 / area);
      for(auto& h: hc) h = h * scale + (C0) * (1-scale);
      }

    hyperpoint ctr = Hypc;
    for(auto h: hc) ctr += h;
    ctr /= isize(hc);
    ctr = normalize(ctr);
    for(auto& h: hc) h = gpushxto0(ctr) * h;

    hatcorners[1] = hc;
    for(auto& h: hc) h = MirrorX * h;
    reverse(hatcorners[1].begin(), hatcorners[1].end());

    clear_adj_memo();
    if(q == 6) {
      hyperpoint hfar = 
        adj(1,9,0,7) * adj(0,11,0,10) * adj(0,1,0,2) * adj(0,8,0,5) * adj(0,11,0,10) *
        adj(0,1,0,2) * adj(0,8,0,5) * adj(0,11,0,2) * adj(0,8,0,5) * adj(0,11,0,10) *
        adj(0,1,0,2) * adj(0,8,0,5) * adj(0,11,0,10) * adj(0,1,0,2) * adj(0,8,0,5) *
        adj(0,11,0,2) * adj(0,8,0,5) * C0;
      transmatrix T = spintox(hfar);
      for(auto& h: hc) h = inverse(T) * h;
      for(auto& h: hatcorners[1]) h = T * h;
      }

    if(emb) {
      geom3::light_flip(f);
      for(auto i:{0, 1}) for(auto& p: hatcorners[i]) {
        p = cgi.emb->base_to_actual(p);
        }
      }

    clear_adj_memo();
    long_transformations.clear();
    }

  constexpr static int relations = 34;

  // heptagons represent clusters
  // heptagon->distance is 0 for clusters of hats, d+1 for supercluster of heptagon d
  // heptagon->zebraval is 1 for central, 0 for non-central
  // 0 is supercluster, 1..(7-zebraval) are child clusters, 8..(relations-1) are nearby clusters on the same level

  /** for 0-level, the list of hats*/
  map<heptagon*, vector<cell*>> hats;

  heptagon *origin;
  
  heptagon *getOrigin() override { return origin; }

  hyperpoint get_corner(cell *c, int cid, ld cf) override {
    cid = gmod(cid, isize(hatcorners[0]));
    int t = c->master->c7 == c;
    auto& h = hatcorners[t][cid];

    return C0 * (1-3./cf) + h * (3./cf);
    }

  heptagon *create_step(heptagon *h, int dir) override {
    auto h1 = get_step(h, dir);
    if(!h1) throw hr_exception("create_step");
    return h1;
    }

  heptagon *get_step(heptagon *h, int dir) {
    if(dir == -1) return h;
    if(h->move(dir)) return h->move(dir);
    if(dir == 0) {
      // create parent
      auto h1 = init_heptagon(relations); 
      h1->distance = h->distance + 1;
      if(h->distance >= 1000) throw hr_exception("too much recursion");
      h1->zebraval = hatrand(2);
      if(h->zebraval == 1) {
        h->c.connect(dir, h1, 1, false);
        }
      else {
        h->c.connect(dir, h1, 2 + hatrand(5-h->zebraval), false);
        }
      return h1;
      }
    if(dir <= 7 - h->zebraval) {
      // create child
      auto h1 = init_heptagon(relations); 
      h1->distance = h->distance - 1;
      h1->zebraval = dir == 1;
      h->c.connect(dir, h1, 0, false);
      if(h1->distance == 0) build_cells(h1);
      return h1;
      }
    // create side connection
    createStep(h, 0);
    int id = h->c.spin(0)-1;
    indenter ind(2);
    for(auto& ru: rules_recursive) {
      if(ru.id0 == id && ru.child == dir) {
        heptagon *h1 = get_step(h->move(0), ru.parent);
        if(!h1) continue;
        heptagon *h2 = get_step(h1, ru.id1+1);
        if(!h2) continue;
        h->c.connect(dir, h2, ru.rev_child, false);
        return h2;
        }
      }
    return nullptr;
    }

  int fix(int d) {
    int n = isize(hatcorners[0]);
    return gmod(n-2-d, n);
    }

  int hat_id(cell *c) {
    auto& gha = hats[c->master];
    for(int i=0; i<isize(gha); i++) if(gha[i] == c) return i;
    throw hr_exception("not in hats");
    }

  void find_cell_connection(cell *c, int d) {
    int id = hat_id(c);
    indenter ind(2);
    for(auto& ru: rules_base) {
      if(ru.id0 == id && ru.edge0 == fix(d)) {
        heptagon *h = get_step(c->master, ru.master_connection);
        if(!h) continue;
        build_cells(h);
        auto& ha = hats[h];
        if(ru.id1 >= isize(ha)) continue;
        auto& c1 = ha[ru.id1];
        c->c.connect(d, c1, fix(ru.edge1), false);
        return;
        }
      }
    throw hr_exception("cell connection not found");
    }

  transmatrix adj(cell *c0, int d0) override {
    cell *c1 = c0->cmove(d0);
    int t0 = c0 == c0->master->c7;
    int t1 = c1 == c1->master->c7;
    int d1 = c0->c.spin(d0);
    return adj(t0, d0, t1, d1);
    }

  memo_matrix& adj(int t0, int d0, int t1, int d1) {
    auto& mm = adj_memo[t0][t1][d0][d1];
    if(mm.known) return mm;

    int n = isize(hatcorners[0]);

    hyperpoint vl = hatcorners[t0][d0];
    hyperpoint vr = hatcorners[t0][(d0+1)%n];

    hyperpoint xvl = hatcorners[t1][d1];
    hyperpoint xvr = hatcorners[t1][(d1+1)%n];

    bool emb = embedded_plane;
    if(emb) {
      vl = cgi.emb->actual_to_base(vl);
      vr = cgi.emb->actual_to_base(vr);
      xvl = cgi.emb->actual_to_base(xvl);
      xvr = cgi.emb->actual_to_base(xvr);
      geom3::light_flip(true);
      }

    hyperpoint vm = mid(vl, vr);
    transmatrix rm = gpushxto0(vm);
    hyperpoint xvm = mid(xvl, xvr);
    transmatrix xrm = gpushxto0(xvm);

    if(abs(hdist(vl, vr) - hdist(xvl, xvr)) > 1e-3)
      throw hr_exception("wrong length connection");

    transmatrix T = rgpushxto0(vm) * rspintox(rm*vr) * spintox(xrm*xvl) * xrm;

    if(emb) {
      T = cgi.emb->base_to_actual(T);
      geom3::light_flip(false);
      }

    mm.set(T);
    return mm;
    }

  void build_cells(heptagon *h) {
    if(h->c7) return;
    auto& ha = hats[h];
    ha.resize(8 - h->zebraval);
    for(auto& hac: ha) hac = newCell(isize(hatcorners[0]), h);
    h->c7 = ha[0];
    }

  hrmap_hat() {
    hatrng.seed(37);
    init();
    origin = init_heptagon(relations);
    origin->distance = 0;
    origin->zebraval = 1;
    build_cells(origin);
    }

  void on_dim_change() override {
    init();
    }

  transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
    if(h1 == h2) return Id;
    int d = h2->distance + 2;
    fill_transform_levels(d);
    return iso_inverse(long_transformations[d][h1->c.spin(0)]) * relative_matrixh(h2->move(0), h1->move(0), hint) * long_transformations[d][h2->c.spin(0)];
    }

  transmatrix relative_matrixc(cell *c2, cell *c1, const hyperpoint& hint) override {
    if(c1 == c2) return Id;
    fill_transform_levels(1);
    transmatrix T = iso_inverse(long_transformations[0][hat_id(c1)+1]) * relative_matrixh(c2->master, c1->master, hint) * long_transformations[0][hat_id(c2)+1];
    return T;
    }

  ~hrmap_hat() {
    clearfrom(origin);
    }

  };

EX hrmap *new_map() { return new hrmap_hat; }

hrmap_hat* hat_map() { return dynamic_cast<hrmap_hat*>(currentmap); }

EX bool pseudohept(cell *c) {
  int id = get_hat_id(c);
  return id == 0 || id == 6;
  }

EX int get_hat_id(cell *c) {
  return FPIU(hat_map())->hat_id(c);
  }

EX void reshape() {
  hrmap_hat *hatmap;
  hatmap = FPIU( hat_map() );
  if(!hatmap) return;
  hatmap->init();
  }

EX color_t hatcolor(cell *c, int mode) {
  vector<int> cols;
  auto *m = (hrmap_hat*) (currentmap);
  cols.push_back(m->hat_id(c));
  heptagon *h = c->master;
  for(int i=0; i<6; i++) { h->cmove(0); cols.push_back(h->c.spin(0)-1); h = h->cmove(0); }
  color_t col = 0xFFFFFF;
  if(cols[0] == 0) col |= 0x1000000;
  vector<int> ads = {0, 0x1, 0x100, 0x101, 0x10000, 0x10001, 0x10100, 0x10101 };
  for(int a=0; a<7; a++) {
    col -= ads[cols[a]] << (mode - a) % 7;
    }
  return col;
  }

auto hooksw = addHook(hooks_swapdim, 100, [] { auto h = hat_map(); if(h) h->init(); });

}}

   
