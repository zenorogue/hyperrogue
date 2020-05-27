// Hyperbolic Rogue -- regular honeycombs
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file reg3.cpp
 *  \brief regular honeycombs
 *
 *  works with spherical and hyperbolic ones -- Euclidean cubic tiling implemented in euclid.cpp
 *  includes non-quotient spaces as well as field quotient and elliptic spaces
 *  hyperbolic honeycombs rely on bt:: to deal with floating point errors (just like archimedean)
 */

#include "hyper.h"
namespace hr {
#if MAXMDIM >= 4

namespace binary {   
  void build_tmatrix(); 
  void virtualRebaseSimple(heptagon*& base, transmatrix& at);
  int celldistance3(heptagon *c1, heptagon *c2);
  hyperpoint deparabolic3(hyperpoint h);
  }

/** \brief regular three-dimensional tessellations */
EX namespace reg3 {

  #if HDR
  inline short& altdist(heptagon *h) { return h->emeraldval; }
  #endif
  
  EX bool ultra_mirror_on;

  EX bool ultra_mirror_in() { return (cgflags & qULTRA) && ultra_mirror_on; }
  
  EX bool in() {
    if(fake::in()) return FPIU(in());
    return GDIM == 3 && !euclid && !bt::in() && !nonisotropic && !hybri && !kite::in();
    }

  EX void compute_ultra() {
    cgi.ultra_mirror_part = .99;    
    cgi.ultra_material_part = .99;
    
    if(cgflags & qULTRA) {
      transmatrix T = spintox(cgi.cellshape[0]);
      hyperpoint a = T * cgi.cellshape[0];
      hyperpoint b = T * cgi.cellshape[1];
      ld f0 = 0.5;
      ld f1 = binsearch(0.5, 1, [&] (ld d) {
        hyperpoint c = lerp(b, a, d);
        if(debugflags & DF_GEOM) 
          println(hlog, "d=", d, " c= ", c, " material = ", material(c));
        return material(c) <= 0;
        });
      cgi.ultra_material_part = f1;
      auto f = [&] (ld d) {
        hyperpoint c = lerp(b, a, d);
        c = normalize(c);
        return c[1] * c[1] + c[2] * c[2];
        };
      for(int it=0; it<100; it++) {
        ld fa = (f0*2+f1) / 3;
        ld fb = (f0*1+f1*2) / 3;
        if(debugflags & DF_GEOM) 
          println(hlog, "f(", fa, ") = ", f(fa), " f(", fb, ") = ", f(fb));
        if(f(fa) > f(fb)) f0 = fa;
        else f1 = fb;
        }
      
      cgi.ultra_mirror_part = f0;

      hyperpoint c = lerp(b, a, f0);
      c = normalize(c);
      c[1] = c[2] = 0;
      c = normalize(c);
      cgi.ultra_mirror_dist = hdist0(c);
      }

    if(cgflags & qULTRA) for(auto v: cgi.vertices_only)
      cgi.ultra_mirrors.push_back(rspintox(v) * xpush(cgi.ultra_mirror_dist*2) * MirrorX * spintox(v));
    }

  EX void generate() {

    if(fake::in()) {
      fake::generate();
      return;
      }
  
    int& loop = cgi.loop;
    int& face = cgi.face;
    auto& vertices_only = cgi.vertices_only;
    auto& spins = cgi.spins;
    auto& cellshape = cgi.cellshape;
    auto& adjcheck = cgi.adjcheck;
    auto& dirs_adjacent = cgi.dirs_adjacent;
  
    int& mid = cgi.schmid;
    mid = 3;
    
    face = 3;
    if(S7 == 6) face = 4;
    if(S7 == 8) mid = 4;
    if(S7 == 12) face = 5;
    if(S7 == 20) mid = 5;
    /* icosahedron not implemented */
    loop = ginf[geometry].tiling_name[5] - '0';
    DEBB(DF_GEOM, ("face = ", face, " loop = ", loop, " S7 = ", S7));    
    
    ld angle_between_faces, hcrossf;
    
    /* frontal face direction */
    hyperpoint h0, h1, h2, h3, h012, h013;
        
    if(1) {
      dynamicval<eGeometry> dg(geometry, gSphere);
      angle_between_faces = edge_of_triangle_with_angles(2*M_PI/mid, M_PI/face, M_PI/face);
      
      h0 = xtangent(1);
      h1 = cspin(0, 1, angle_between_faces) * h0;
      h2 = cspin(1, 2, 2*M_PI/face) * h1;
      h3 = cspin(1, 2, -2*M_PI/face) * h1;

      hcrossf = edge_of_triangle_with_angles(M_PI/2, M_PI/mid, M_PI/face);

      h012 = cspin(1, 2, M_PI/face) * cspin(0, 1, hcrossf) * h0;
      h013 = cspin(1, 2, -M_PI/face) * cspin(0, 1, hcrossf) * h0;
      }
    
    for(auto hx: {&h0, &h1, &h2, &h3, &h012, &h013}) (*hx)[3] = 0;
    
    ld klein_scale = binsearch(0, 10, [&] (ld d) {
      dynamicval<eGeometry> g(geometry, elliptic ? gCell120 : geometry);

      /* center of an edge */
      hyperpoint u = C0 + (h012 + h013) * d / 2;

      if(material(u) <= 0) {
        println(hlog, "klein_scale = ", d, " bad");
        return true;
        }

      u = normalize(u);

      hyperpoint h = C0 * face;
      for(int i=0; i<face; i++) h += d * (cspin(1, 2, M_PI*2*i/face) * h012);
      h = normalize(h);

      hyperpoint h2 = rspintox(h) * xpush0(2 * hdist0(h));
      
      h2 = spintox(u) * h2;
      u = spintox(u) * u;
      
      h2 = gpushxto0(u) * h2;
      u = gpushxto0(u) * u;
      
      ld x = hypot(h2[1], h2[2]);
      ld y = h2[0];
    
      ld loop2 = 360 / (90 + atan(y/x) / degree);
      
      println(hlog, "d=", d, " loop2= ", loop2);
      
      if(sphere) return loop2 < loop;
      return loop2 > loop;
      });
    
    /* precise ideal vertex */
    if(klein_scale > 1-1e-5 && klein_scale < 1+1e-5) klein_scale = 1;
    
    /* actual vertex */
    hyperpoint v2 = C0 + klein_scale * h012;

    hyperpoint midface = Hypc;
    for(int i=0; i<face; i++) midface += cspin(1, 2, 2*i*M_PI/face) * v2;
    midface = normalize(midface);
    ld between_centers = 2 * hdist0(midface);
    DEBB(DF_GEOM, ("between_centers = ", between_centers));
    
    if(S7 == 20) {
      spins[0] = Id;
      spins[1] = cspin(0, 1, angle_between_faces) * cspin(1, 2, M_PI);
      spins[2] = spins[1] * cspin(1, 2, -2 * M_PI/face) * spins[1];
      spins[3] = spins[1] * cspin(1, 2, +2 * M_PI/face) * spins[1];
      for(int a=4; a<10; a++) spins[a] = cspin(1, 2, 2*M_PI/face) * spins[a-3];
      for(int a=S7/2; a<S7; a++) spins[a] = spins[a-S7/2] * cspin(0, 1, M_PI);
      }

    if(S7 == 12 || S7 == 8) {
      spins[0] = Id;
      spins[1] = cspin(0, 1, angle_between_faces) * cspin(1, 2, M_PI);
      for(int a=2; a<face+1; a++) spins[a] = cspin(1, 2, 2*M_PI*(a-1)/face) * spins[1];      
      for(int a=S7/2; a<S7; a++) spins[a] = cspin(0, 1, M_PI) * spins[a-S7/2];
      if(S7 == 8) swap(spins[6], spins[7]);
      if(S7 == 12) swap(spins[8], spins[11]);
      if(S7 == 12) swap(spins[9], spins[10]);
      }
    
    if(S7 == 6) {
      spins[0] = Id;
      spins[1] = cspin(0, 1, angle_between_faces) * cspin(1, 2, M_PI);
      spins[2] = cspin(1, 2, M_PI/2) * spins[1];
      for(int a=S7/2; a<S7; a++) spins[a] = spins[a-S7/2] * cspin(0, 1, M_PI);
      }
    
    if(S7 == 4) {
      spins[0] = Id;
      spins[1] = cspin(0, 1, angle_between_faces) * cspin(1, 2, M_PI);
      for(int a=2; a<face+1; a++) spins[a] = cspin(1, 2, 2*M_PI*(a-1)/face) * spins[1];      
      }
    
    cellshape.clear();
    for(int a=0; a<S7; a++)
    for(int b=0; b<face; b++)
      cellshape.push_back(spins[a] * cspin(1, 2, 2*M_PI*b/face) * v2);
    
    cgi.adjmoves[0] = cpush(0, between_centers) * cspin(0, 2, M_PI);
    for(int i=1; i<S7; i++) cgi.adjmoves[i] = spins[i] * cgi.adjmoves[0];

    for(int a=0; a<S7; a++)
      DEBB(DF_GEOM, ("center of ", a, " is ", tC0(cgi.adjmoves[a])));
    
    DEBB(DF_GEOM, ("doublemove = ", tC0(cgi.adjmoves[0] * cgi.adjmoves[0])));

    adjcheck = hdist(tC0(cgi.adjmoves[0]), tC0(cgi.adjmoves[1])) * 1.0001;

    int numedges = 0;
    for(int a=0; a<S7; a++) for(int b=0; b<S7; b++) {
      dirs_adjacent[a][b] = a != b && hdist(tC0(cgi.adjmoves[a]), tC0(cgi.adjmoves[b])) < adjcheck;
      if(dirs_adjacent[a][b]) numedges++;
      }
    DEBB(DF_GEOM, ("numedges = ", numedges));
    
    if(loop == 4) cgi.strafedist = adjcheck;
    else cgi.strafedist = hdist(cgi.adjmoves[0] * C0, cgi.adjmoves[1] * C0);
    
    if(stretch::applicable()) {
      transmatrix T = cspin(0, 2, 90 * degree);
      transmatrix iT = inverse(T);
      for(auto& v: cgi.adjmoves) v = T * v * iT;
      for(auto& v: cellshape) v = T * v;
      }

    vertices_only.clear();
    for(hyperpoint h: cellshape) {
      bool found = false;
      for(hyperpoint h2: vertices_only) if(hdist(h, h2) < 1e-6) found = true;
      if(!found) vertices_only.push_back(h);
      }

    compute_ultra();
    
    for(int a=0; a<12; a++)
    for(int b=0; b<12; b++)
      if(cgi.dirs_adjacent[a][b]) 
        for(int c=0; c<12; c++)
          if(cgi.dirs_adjacent[a][c] && cgi.dirs_adjacent[b][c]) {
            transmatrix t = build_matrix(tC0(cgi.adjmoves[a]), tC0(cgi.adjmoves[b]), tC0(cgi.adjmoves[c]), C0);
            if(det(t) > 1e-3) cgi.next_dir[a][b] = c;
            }  
    }

  void binary_rebase(heptagon *h, const transmatrix& V) {
    }
  
  void test();

  #if HDR
  struct hrmap_quotient3 : hrmap {
    vector<heptagon*> allh;
    vector<vector<transmatrix>> tmatrices;    
    vector<cell*> acells;

    transmatrix adj(heptagon *h, int d) override { return tmatrices[h->fieldval][d]; }

    heptagon *getOrigin() override { return allh[0]; }

    void draw() override;
    transmatrix relative_matrix(heptagon *h2, heptagon *h1, const hyperpoint& hint) override;
    
    void initialize(int cell_count);
    vector<cell*>& allcells() override { return acells; }

    vector<hyperpoint> get_vertices(cell* c) override { return cgi.vertices_only; }
    };
  #endif
  
  void hrmap_quotient3::initialize(int cell_count) {
    allh.resize(cell_count);
    acells.clear();
    tmatrices.resize(cell_count);
    for(int a=0; a<cell_count; a++) {
      allh[a] = tailored_alloc<heptagon> (S7);
      allh[a]->c7 = newCell(S7, allh[a]);
      allh[a]->fieldval = a;
      allh[a]->zebraval = 0;
      allh[a]->alt = NULL;
      acells.push_back(allh[a]->c7);
      }
    }

  void hrmap_quotient3::draw() {
    sphereflip = Id;
    
    // for(int i=0; i<S6; i++) queuepoly(ggmatrix(cwt.at), shWall3D[i], 0xFF0000FF);
    
    dq::visited_by_matrix.clear();
    dq::enqueue_by_matrix(centerover->master, cview());
    
    while(!dq::drawqueue.empty()) {
      auto& p = dq::drawqueue.front();
      heptagon *h = get<0>(p);
      transmatrix V = get<1>(p);
      dynamicval<ld> b(band_shift, get<2>(p));
      bandfixer bf(V);
      dq::drawqueue.pop();
            
      cell *c = h->c7;
      if(!do_draw(c, V)) continue;
      drawcell(c, V);
      if(in_wallopt() && isWall3(c) && isize(dq::drawqueue) > 1000) continue;
      
      if(ultra_mirror_in())
        for(auto& T: cgi.ultra_mirrors) 
          dq::enqueue_by_matrix(h, V * T);
  
      for(int d=0; d<S7; d++)
        dq::enqueue_by_matrix(h->move(d), V * tmatrices[h->fieldval][d]);
      }
    }

  transmatrix hrmap_quotient3::relative_matrix(heptagon *h2, heptagon *h1, const hyperpoint& hint) {
    if(h1 == h2) return Id;
    int d = hr::celldistance(h2->c7, h1->c7);

    for(int a=0; a<S7; a++) if(hr::celldistance(h1->move(a)->c7, h2->c7) < d)
      return adj(h1, a) * relative_matrix(h2, h1->move(a), hint);
      
    for(int a=0; a<S7; a++) println(hlog, "d=", d, " vs ", hr::celldistance(h1->move(a)->c7, h2->c7));
    
    println(hlog, "error in hrmap_quotient3:::relative_matrix");
    return Id;
    }
    
#if CAP_CRYSTAL
  int encode_coord(const crystal::coord& co) {
    int c = 0;
    for(int i=0; i<4; i++) c |= ((co[i]>>1) & 3) << (2*i);
    return c;
    }

  EX crystal::coord decode_coord(int a) {
    crystal::coord co;
    for(int i=0; i<4; i++) co[i] = (a & 3) * 2, a >>= 2;
    return co;
    }
  
  struct hrmap_from_crystal : hrmap_quotient3 {
  
    hrmap_from_crystal() {
      initialize(256);
      if(1) {
        auto m = crystal::new_map();
        dynamicval<hrmap*> cm(currentmap, m);
        for(int a=0; a<256; a++) {
          auto co = decode_coord(a);
          heptagon *h1 = get_heptagon_at(co);
          for(int d=0; d<8; d++) {
            int b = encode_coord(crystal::get_coord(h1->cmove(d)));
            allh[a]->c.connect(d, allh[b], h1->c.spin(d), false);
            tmatrices[a].push_back(crystal::get_adj(h1, d));
            }
          }
        delete m;
        }
      }        
    };
#endif

  struct hrmap_field3 : reg3::hrmap_quotient3 {
  
    fieldpattern::fpattern *f;
    
    hrmap_field3(fieldpattern::fpattern *ptr) {
    
      f = ptr;

      auto lgr = f->local_group;
      
      int N = isize(f->matrices) / lgr;
      initialize(N);
      
      vector<int> moveid(S7), movedir(lgr);
      for(int s=0; s<lgr; s++) 
      for(int i=0; i<S7; i++) if(eqmatrix(f->fullv[s] * cgi.adjmoves[0], cgi.adjmoves[i]))
        moveid[i] = s;
  
      for(int s=0; s<lgr; s++) 
      for(int i=0; i<S7; i++) if(hdist(tC0(inverse(f->fullv[s]) * cgi.adjmoves[0]), tC0(cgi.adjmoves[i])) < 1e-4)
        movedir[s] = i;
  
      for(int a=0; a<N; a++) {
        tmatrices[a].resize(S7);
        for(int b=0; b<S7; b++) {
          int k = lgr*a;
          k = f->gmul(f->gmul(k, moveid[b]), lgr);
          for(int l=0; l<lgr; l++) if(f->gmul(k, l) % lgr == 0) {
            tmatrices[a][b] = cgi.adjmoves[b] * f->fullv[l];
            allh[a]->c.connect(b, allh[k/lgr], movedir[l], false);
            }
          }
        }      
      create_patterns();
      }
    
    set<cellwalker> plane;

    void make_plane(cellwalker cw) {
      if(plane.count(cw)) return;
      plane.insert(cw);
      for(int i=0; i<S7; i++)
        if(cgi.dirs_adjacent[i][cw.spin])
          make_plane(reg3::strafe(cw, i));
      }
    
    
    void create_patterns() {
      DEBB(DF_GEOM, ("creating pattern = ", isize(allh)));

      // also, strafe needs currentmap
      dynamicval<hrmap*> c(currentmap, this);

      if(S7 == 12) {
        // Emerald in 534
        cell *a = gamestart();
        cell *b = a;
        for(cell *c: allcells())
          if(bounded_celldistance(a, c) == 5) {
            b = c;
            break;
            }
        for(cell *c: allcells())
          if(bounded_celldistance(a, c) > bounded_celldistance(b, c))
            c->master->zebraval |= 1;
          
        // Vineyard in 534
        b = (cellwalker(a, 0) + wstep + rev + wstep).at;
        for(cell *c: allcells())
          if(bounded_celldistance(a, c) == bounded_celldistance(b, c))
            c->master->zebraval |= 2;
        }
      
      if(S7 == 6 && ginf[geometry].vertex == 5) {
        // Emerald in 534
        cell *a = gamestart();
        for(cell *c: allcells())
          if(bounded_celldistance(a, c) > 3)
            c->master->zebraval |= 1;
        
        // Vineyard in 435
        make_plane(cellwalker(gamestart(), 0));
        DEBB(DF_GEOM, ("plane size = ", isize(plane)));
        
        set<int> plane_indices;
        for(auto cw: plane) plane_indices.insert(cw.at->master->fieldval);

        int fN = isize(f->matrices);

        set<int> nwi;
        for(int i=0; i<fN; i++) {
          bool ok = true;
          for(auto o: plane_indices) {
            int j = f->gmul(i, o * f->local_group) / f->local_group;
            if(plane_indices.count(j)) ok = false;
            forCellEx(c1, allcells()[j]) if(plane_indices.count(c1->master->fieldval)) ok = false;
            }
          if(ok) nwi.insert(i);
          }

        int gpow = 0;
        
        for(int i: nwi) {
          int pw = 1;
          int at = i;
          while(true) {
            at = f->gmul(at, i);
            if(!nwi.count(at)) break;
            pw++;
            }
          if(pw == 4) gpow = i;
          }
        
        int u = 0;
        for(int a=0; a<5; a++) {
          for(int o: plane_indices) {
            int j = f->gmul(u, o * f->local_group) / f->local_group;
            allcells()[j]->master->zebraval |= 2;
            }
          u = f->gmul(u, gpow);
          }
        }
      }
    };

  /** \brief homology cover of the Seifert-Weber space */
  namespace seifert_weber {
  
    using crystal::coord;
  
    vector<coord> periods;
  
    int flip(int x) { return (x+6) % 12; }
  
    void build_reps() {
      // start_game();
      for(int a=0; a<12; a++)
      for(int b=0; b<12; b++)
        if(cgi.dirs_adjacent[a][b]) 
          for(int c=0; c<12; c++)
            if(cgi.dirs_adjacent[a][c] && cgi.dirs_adjacent[b][c]) {
              transmatrix t = build_matrix(tC0(cgi.adjmoves[a]), tC0(cgi.adjmoves[b]), tC0(cgi.adjmoves[c]), C0);
              if(det(t) > 0) cgi.next_dir[a][b] = c;
              }  
      
      set<coord> boundaries;
      
      for(int a=0; a<12; a++)
      for(int b=0; b<12; b++) if(cgi.dirs_adjacent[a][b]) {
        coord res = crystal::c0;
        int sa = a, sb = b;
        do {
          // printf("%d ", sa);
          if(sa < 6) res[sa]++; else res[sa-6]--;
          sa = flip(sa);
          sb = flip(sb);
          swap(sa, sb);
          sb = cgi.next_dir[sa][sb];
          // sb = next_dirsa][sb];
          }
        while(a != sa || b != sb);
        // printf("\n");
        if(res > crystal::c0) 
          boundaries.insert(res);
        }
      
      periods.clear();
      
      for(int index = 5; index >= 0; index--) {
        for(auto k: boundaries) println(hlog, k);
        DEBB(DF_GEOM, ("simplifying..."));
        
        for(auto by: boundaries) if(among(by[index], 1, -1)) {
          DEBB(DF_GEOM, ("simplifying by ", by));
          periods.push_back(by);
          set<coord> nb;
      
          for(auto v: boundaries)
            if(v == by) ;
            else if(v[index] % by[index] == 0)
              nb.insert(v - by * (v[index] / by[index]));
            else println(hlog, "error");    
          
          boundaries = move(nb);
          break;
          }
        }
      }

    int get_rep(coord a) {
      a = a - periods[0] * (a[5] / periods[0][5]);
      a = a - periods[1] * (a[4] / periods[1][4]);
      a = a - periods[2] * (a[3] / periods[2][3]);
      for(int i=0; i<3; i++) a[i] = gmod(a[i], 5);
      return a[2] * 25 + a[1] * 5 + a[0];
      }
    
    coord decode(int id) {
      coord res = crystal::c0;
      for(int a=0; a<3; a++) res[a] = id % 5, id /= 5;
      return res;
      }

    struct hrmap_singlecell : hrmap_quotient3 {    
      hrmap_singlecell(ld angle) {
        initialize(1);
        tmatrices[0].resize(S7);
        for(int b=0; b<S7; b++) {
          allh[0]->c.connect(b, allh[0], (b+S7/2) % S7, false);
          transmatrix T = cgi.adjmoves[b];
          hyperpoint p = tC0(T);
          tmatrices[0][b] = rspintox(p) * xpush(hdist0(p)) * cspin(2, 1, angle) * spintox(p);
          }
        }
      };

    struct hrmap_seifert_cover : hrmap_quotient3 {

      hrmap_seifert_cover() {
        if(periods.empty()) build_reps();
        initialize(125);
        for(int a=0; a<125; a++) {
          tmatrices[a].resize(12);
          for(int b=0; b<12; b++) {
            coord x = decode(a);
            if(b < 6) x[b]++;
            else x[b-6]--;
            int a1 = get_rep(x);
            allh[a]->c.connect(b, allh[a1], flip(b), false);
            transmatrix T = cgi.adjmoves[b];
            hyperpoint p = tC0(T);
            tmatrices[a][b] = rspintox(p) * xpush(hdist0(p)) * cspin(2, 1, 108 * degree) * spintox(p);
            }
          }      
        }
      };
    
    }

  struct hrmap_reg3 : hrmap {
  
    heptagon *origin;
    hrmap *binary_map;
    hrmap_quotient3 *quotient_map;
    
    unordered_map<heptagon*, pair<heptagon*, transmatrix>> reg_gmatrix;
    unordered_map<heptagon*, vector<pair<heptagon*, transmatrix> > > altmap;

    vector<cell*> spherecells;  

    vector<cell*>& allcells() override { 
      if(sphere) return spherecells;
      return hrmap::allcells();
      }

    hrmap_reg3() {
      origin = tailored_alloc<heptagon> (S7);
      heptagon& h = *origin;
      h.s = hsOrigin;
      h.cdata = NULL;
      h.alt = NULL;
      h.distance = 0;
      h.fiftyval = 0;
      h.fieldval = 0;
      h.emeraldval = 0;
      h.c7 = newCell(S7, origin);
      if(sphere) spherecells.push_back(h.c7);
      worst_error1 = 0, worst_error2 = 0;
      
      dynamicval<hrmap*> cr(currentmap, this);
      
      heptagon *alt = NULL;
      transmatrix T = Id;
      
      binary_map = nullptr;
      quotient_map = nullptr;
      
      #if CAP_FIELD
      if(geometry == gSpace344) {
        quotient_map = new hrmap_from_crystal;
        }
      else if(geometry == gSpace535) {
        quotient_map = new seifert_weber::hrmap_seifert_cover;
        }
      else if(hyperbolic) {
        quotient_map = new hrmap_field3(&currfp);
        }
      #endif
      h.zebraval = quotient_map ? quotient_map->allh[0]->zebraval : 0;
      
      if(hyperbolic) {
        dynamicval<eGeometry> g(geometry, gBinary3); 
        bt::build_tmatrix();
        alt = tailored_alloc<heptagon> (S7);
        alt->s = hsOrigin;
        alt->emeraldval = 0;
        alt->zebraval = 0;
        alt->distance = 0;
        alt->alt = alt;
        alt->cdata = NULL;
        alt->c7 = NULL;
        binary_map = bt::new_alt_map(alt);
        T = xpush(.01241) * spin(1.4117) * xpush(0.1241) * cspin(0, 2, 1.1249) * xpush(0.07) * Id;
        }
      
      reg_gmatrix[origin] = make_pair(alt, T);
      altmap[alt].emplace_back(origin, T);
      
      celllister cl(origin->c7, 4, 100000, NULL);
      for(cell *c: cl.lst) {
        hyperpoint h = tC0(relative_matrix(c->master, origin, C0));
        cgi.close_distances[bucketer(h)] = cl.getdist(c);
        }
      }
    
    ld worst_error1, worst_error2;

    heptagon *getOrigin() override {
      return origin;
      }
    
    void fix_distances(heptagon *h, heptagon *h2) {
      vector<heptagon*> to_fix;

      auto fix_pair = [&] (heptagon *h, heptagon *h2) {
        if(!h2) return;
        if(h->distance > h2->distance+1) {
          h->distance = h2->distance + 1;
          to_fix.push_back(h);
          }
        else if(h2->distance > h->distance+1) {
          h2->distance = h->distance + 1;
          to_fix.push_back(h2);
          }
        if(h->alt && h->alt == h2->alt) {
          if(altdist(h) > altdist(h2) + 1) {
            altdist(h) = altdist(h2) + 1;
            to_fix.push_back(h);
            }
          else if (altdist(h2) > altdist(h) + 1) {
            altdist(h2) = altdist(h) + 1;
            to_fix.push_back(h2);
            }
          }
        };

      if(!h2) to_fix = {h};
      else fix_pair(h, h2);
      
      for(int i=0; i<isize(to_fix); i++) {
        h = to_fix[i];
        for(int j=0; j<S7; j++) fix_pair(h, h->move(j));
        }
      }
    
    #define DEB 0
    
    heptagon *counterpart(heptagon *h) {
      return quotient_map->allh[h->fieldval];
      }

    heptagon *create_step(heptagon *parent, int d) override {
      auto& p1 = reg_gmatrix[parent];
      if(DEB) println(hlog, "creating step ", parent, ":", d, ", at ", p1.first, tC0(p1.second));
      heptagon *alt = p1.first;
      #if CAP_FIELD
      transmatrix T = p1.second * (quotient_map ? quotient_map->tmatrices[parent->fieldval][d] : cgi.adjmoves[d]);
      #else
      transmatrix T = p1.second * cgi.adjmoves[d];
      #endif
      transmatrix T1 = T;
      if(hyperbolic) {
        dynamicval<eGeometry> g(geometry, gBinary3);
        dynamicval<hrmap*> cm(currentmap, binary_map);
        binary_map->virtualRebase(alt, T);
        }

      fixmatrix(T);
      auto hT = tC0(T);
      
      bool hopf = stretch::applicable();

      if(hopf)
        T = stretch::translate(hT);      
      
      if(DEB) println(hlog, "searching at ", alt, ":", hT);

      if(DEB) for(auto& p2: altmap[alt]) println(hlog, "for ", tC0(p2.second), " intval is ", intval(tC0(p2.second), hT));
      
      ld err;
      
      for(auto& p2: altmap[alt]) if((err = intval(tC0(p2.second), hT)) < 1e-3) {
        if(err > worst_error1) println(hlog, format("worst_error1 = %lg", double(worst_error1 = err)));
        // println(hlog, "YES found in ", isize(altmap[alt]));
        if(DEB) println(hlog, "-> found ", p2.first);
        int fb = 0;
        hyperpoint old = tC0(p1.second);;
        if(!hopf) T * (inverse(T1) * old);
        #if CAP_FIELD
        if(quotient_map) {
          p2.first->c.connect(counterpart(parent)->c.spin(d), parent, d, false);
          fix_distances(p2.first, parent);
          return p2.first;
          }
        #endif
        for(int d2=0; d2<S7; d2++) {
          hyperpoint back = p2.second * tC0(cgi.adjmoves[d2]);
          if((err = intval(back, old)) < 1e-3) {
            if(err > worst_error2) println(hlog, format("worst_error2 = %lg", double(worst_error2 = err)));
            if(p2.first->move(d2)) println(hlog, "error: repeated edge");
            p2.first->c.connect(d2, parent, d, false);
            fix_distances(p2.first, parent);
            fb++;
            }
          }
        if(fb != 1) { 
          println(hlog, "found fb = ", fb); 
          println(hlog, old);
          for(int d2=0; d2<S7; d2++) {
            println(hlog, p2.second * tC0(cgi.adjmoves[d2]), " in distance ", intval(p2.second * tC0(cgi.adjmoves[d2]), old));
            }
          parent->c.connect(d, parent, d, false);
          return parent;
          }
        return p2.first;
        }
      
      if(DEB) println(hlog, "-> not found");
      int d2 = 0, fv = isize(reg_gmatrix);
      #if CAP_FIELD
      if(quotient_map) {
        auto cp = counterpart(parent);
        d2 = cp->c.spin(d);
        fv = cp->c.move(d)->fieldval;
        }
      #endif
      if(hopf) {
        hyperpoint old = tC0(p1.second);
        for(d2=0; d2<S7; d2++) {
          hyperpoint back = T * tC0(cgi.adjmoves[d2]);
          if((err = intval(back, old)) < 1e-3) 
            break;
          }
        if(d2 == S7) { 
          d2 = 0; 
          println(hlog, "Hopf connection failed"); 
          }
        println(hlog, "found d2 = ", d2);
        }
      heptagon *created = tailored_alloc<heptagon> (S7);
      created->c7 = newCell(S7, created);
      if(sphere) spherecells.push_back(created->c7);
      created->alt = NULL;
      created->cdata = NULL;
      #if CAP_FIELD
      if(quotient_map) {
        created->emeraldval = fv;
        created->zebraval = quotient_map->allh[fv]->zebraval;
        }
      else
      #endif
      created->zebraval = hrand(10);
      created->fieldval = fv;
      created->distance = parent->distance + 1;
      created->fiftyval = 9999;
      fixmatrix(T);
      reg_gmatrix[created] = make_pair(alt, T);
      altmap[alt].emplace_back(created, T);
      created->c.connect(d2, parent, d, false);
      return created;
      }

    ~hrmap_reg3() {
      if(binary_map) {        
        dynamicval<eGeometry> g(geometry, gBinary3);
        delete binary_map;
        }
      if(quotient_map) delete quotient_map;
      clearfrom(origin);
      }
    
    map<heptagon*, int> reducers;

    void link_alt(const cellwalker& hs) override {
      auto h = hs.at->master;
      altdist(h) = 0;
      if(h->alt->s != hsOrigin) reducers[h] = hs.spin;
      }
    
    void generateAlts(heptagon* h, int levs, bool link_cdata) override {
      if(reducers.count(h)) {
        heptspin hs(h, reducers[h]);
        reducers.erase(h);
        hs += wstep;
        hs += rev;
        altdist(hs.at) = altdist(h) - 1;
        hs.at->alt = h->alt;
        reducers[hs.at] = hs.spin;
        fix_distances(hs.at, NULL);
        }
      for(int i=0; i<S7; i++) {
        auto h2 = h->cmove(i);
        if(h2->alt == NULL) {
          h2->alt = h->alt;
          altdist(h2) = altdist(h) + 1;
          fix_distances(h2, NULL);
          }
        }
      }

    void draw() override {
      sphereflip = Id;
      
      dq::clear_all();
      auto& enq = (ultra_mirror_in() ? dq::enqueue_by_matrix : dq::enqueue);
      enq(centerover->master, cview());
      
      while(!dq::drawqueue.empty()) {
        auto& p = dq::drawqueue.front();
        heptagon *h = get<0>(p);
        transmatrix V = get<1>(p);
        dynamicval<ld> b(band_shift, get<2>(p));
        bandfixer bf(V);
        dq::drawqueue.pop();
        
        
        cell *c = h->c7;
        if(!do_draw(c, V)) continue;
        drawcell(c, V);
        if(in_wallopt() && isWall3(c) && isize(dq::drawqueue) > 1000) continue;
        
        if(sightranges[geometry] == 0) return;

        if(ultra_mirror_in())
          for(auto& T: cgi.ultra_mirrors) 
            dq::enqueue_by_matrix(h, V * T);
    
        for(int i=0; i<S7; i++) if(h->move(i)) {
          enq(h->move(i), V * adj(h, i));
          }
        }
      }
    
    transmatrix adj(heptagon *h, int d) override {
      #if CAP_FIELD
      if(quotient_map) return quotient_map->adj(h, d);
      else
      #endif
      return relative_matrix(h->cmove(d), h, C0);
      }
     
    transmatrix relative_matrix(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
      auto p1 = reg_gmatrix[h1];
      auto p2 = reg_gmatrix[h2];
      transmatrix T = Id;
      if(hyperbolic) { 
        dynamicval<eGeometry> g(geometry, gBinary3);
        dynamicval<hrmap*> cm(currentmap, binary_map);
        T = binary_map->relative_matrix(p2.first, p1.first, hint);
        }
      T = inverse(p1.second) * T * p2.second;
      if(elliptic && T[LDIM][LDIM] < 0) T = centralsym * T;
      return T;
      }
    
    vector<hyperpoint> get_vertices(cell* c) override {
      return cgi.vertices_only;
      }
    };

  struct hrmap_reg3_rule : hrmap {
  
    heptagon *origin;
    reg3::hrmap_quotient3 *quotient_map;
    reg3::hrmap_quotient3 *emerald_map;

    fieldpattern::fpattern fp;

    vector<int> root;
    string other;
    vector<short> children;
    
    vector<int> otherpos;
    
    void load_ruleset(string fname) {
      FILE *f = fopen(fname.c_str(), "rb");
      if(!f) f = fopen((rsrcdir + fname).c_str(), "rb");
      string buf;
      buf.resize(1000000);
      int qty = fread(&buf[0], 1, 1000000, f);
      buf.resize(qty);

      shstream ins(decompress_string(buf));
      hread_fpattern(ins, fp);
      
      hread(ins, root);
      hread(ins, children);
      hread(ins, other);
      fclose(f);
      }
    
    /** \brief address = (fieldvalue, state) */
    typedef pair<int, int> address;
    
    /** nles[x] lists the addresses from which we can reach address x 
     *  without ever ending in the starting point */

    map<address, set<address>> nonlooping_earlier_states;

    vector<vector<int>> possible_states;

    void find_mappings() {
      auto &nles = nonlooping_earlier_states;
      nles.clear();
      vector<address> bfs;
      int qty = isize(quotient_map->allh);
      if(geometry == gSpace535) qty = 1;
      for(int i=0; i<qty; i++) 
        bfs.emplace_back(i, root[i]);
      auto mov = [&] (int fv, int d) {
        if(geometry == gSpace535) return 0;
        return quotient_map->allh[fv]->move(d)->fieldval;
        };
      int qstate = isize(children) / S7;
      DEBB(DF_GEOM, ("qstate = ", qstate));
      for(int i=0; i<isize(bfs); i++) {
        address last = bfs[i];
        int state = last.second;
        int fv = last.first;
        for(int d=0; d<S7; d++) {
          int nstate = children[state*S7+d];
          if(nstate >= 0) {
            address next = {mov(fv, d), nstate};
            if(!nles.count(next)) bfs.push_back(next);
            nles[next].insert(last);
            }
          }
        }
      
      vector<int> q(qstate, 0);
      for(auto p: bfs) q[p.second]++;
      vector<int> q2(isize(quotient_map->allh)+1, 0);
      for(auto p: q) q2[p]++;
      DEBB(DF_GEOM, ("q2 = ", q2));
      
      bfs = {};
      for(int i=0; i<qty; i++) 
        bfs.emplace_back(i, root[i]);
      for(int i=0; i<isize(bfs); i++) {
        address last = bfs[i];
        int state = last.second;
        int fv = last.first;
        for(int d=0; d<S7; d++) {
          int nstate = children[state*S7+d];
          if(nstate >= 0) {
            address next = {mov(fv, d), nstate};
            if(!nles.count(next)) continue;
            int c = isize(nles[next]);
            nles[next].erase(last);
            if(nles[next].empty() && c) {
              nles.erase(next);
              bfs.push_back(next);
              }
            }
          }
        }
      
      DEBB(DF_GEOM, ("removed cases = ", isize(bfs)));
      
      possible_states.resize(qstate);
      for(auto& p: nonlooping_earlier_states)
        possible_states[p.first.first].push_back(p.first.second);
      }

    hrmap_reg3_rule() : fp(0) {

      if(S7 == 6) load_ruleset("honeycomb-rules-435.dat");
      else if(ginf[geometry].vertex == 5) load_ruleset("honeycomb-rules-535.dat");
      else load_ruleset("honeycomb-rules-534.dat");
      
      origin = tailored_alloc<heptagon> (S7);
      heptagon& h = *origin;
      h.s = hsOrigin;
      h.cdata = NULL;
      h.alt = NULL;
      h.distance = 0;
      h.zebraval = 0;
      h.fieldval = 0;
      h.fiftyval = root[0];
      h.c7 = NULL;
      h.c7 = newCell(S7, origin);
      
      int opos = 0;
      for(int c: children) {
        if(c >= 0)
          otherpos.push_back(-1);
        else {
          otherpos.push_back(opos);
          while(other[opos] != ',') opos++;
          opos++;
          }
        }
      
      quotient_map = nullptr;
      
      if(geometry == gSpace535)
        quotient_map = new seifert_weber::hrmap_seifert_cover();
      else
        quotient_map = new hrmap_field3(&fp);
      
      if(geometry == gSpace535)
        emerald_map = new seifert_weber::hrmap_seifert_cover();
      else
        emerald_map = new hrmap_field3(&currfp);
      h.emeraldval = 0;
      
      find_mappings();
      }
    
    heptagon *getOrigin() override {
      return origin;
      }
    
    #define DEB 0
    
    heptagon *counterpart(heptagon *h) {
      return quotient_map->allh[h->fieldval];
      }
    
    vector<short> evmemo;
    
    void find_emeraldval(heptagon *target, heptagon *parent, int d) {
      if(geometry == gSpace535) {
        target->emeraldval = target->fieldval;
        target->zebraval = 0;
        return;
        }
      auto& cr = cgi.cellrotations;
      if(evmemo.empty()) {
        println(hlog, "starting");
        map<int, int> matrix_hashtable;
        auto matrix_hash = [] (const transmatrix& M) {
          return bucketer(M[0][0]) 
               + bucketer(M[0][1]) * 71 
               + bucketer(M[0][2]) * 113
               + bucketer(M[1][0]) * 1301
               + bucketer(M[1][1]) * 1703
               + bucketer(M[1][2]) * 17031
               + bucketer(M[2][2]) * 2307
               + bucketer(M[2][0]) * 2311
               + bucketer(M[2][1]) * 10311;
          };
        for(int i=0; i<isize(cr); i++) matrix_hashtable[matrix_hash(cr[i].M)] = cr[i].inverse_id;
        println(hlog, "ids size = ", isize(matrix_hashtable));
        
        for(int eid=0; eid<isize(emerald_map->allh); eid++)
        for(int k0=0; k0<isize(cr); k0++)
        for(int fv=0; fv<isize(quotient_map->allh); fv++) {
          for(int d=0; d<S7; d++) {
            int ed = cr[k0].mapping[d];
            auto cpart = emerald_map->allh[eid];
            int eid1 = emerald_map->allh[eid]->move(ed)->fieldval;
            const transmatrix& X = cr[cr[k0].inverse_id].M;
            transmatrix U = quotient_map->iadj(quotient_map->allh[fv], d) * X * emerald_map->adj(cpart, ed);
            int k1 = matrix_hashtable[matrix_hash(U)];
            /* for(int ik1=0; ik1<isize(cr); ik1++)  {
              auto& mX1 = cr[ik1].M;
              if(eqmatrix(mX1, U)) k1 = cr[ik1].inverse_id;
              } */
            evmemo.push_back(eid1 * isize(cr) + k1); 
            }
          }
        println(hlog, "generated ", isize(evmemo));
        }
      int memo_id = parent->emeraldval;
      memo_id = memo_id * isize(quotient_map->allh) + parent->fieldval;
      memo_id = memo_id * S7 + d;
      target->emeraldval = evmemo[memo_id];
      target->zebraval = emerald_map->allh[target->emeraldval / isize(cr)]->zebraval;
      }

    heptagon *create_step(heptagon *parent, int d) override {
      int id = parent->fiftyval;

      auto cp = counterpart(parent);
      int d2 = cp->c.spin(d);
      int fv = cp->c.move(d)->fieldval;
      
      // indenter ind(2);
      
      heptagon *res = nullptr;
      
      int id1 = children[S7*id+d];
      int pos = otherpos[S7*id+d];
      // println(hlog, "id=", id, " d=", d, " d2=", d2, " id1=", id1, " pos=", pos);
      if(id1 != -1) {
        res = tailored_alloc<heptagon> (S7);
        if(parent->c7)
          res->c7 = newCell(S7, res);
        else
          res->c7 = nullptr;
        res->alt = nullptr;
        res->cdata = nullptr;
        res->fieldval = fv;
        res->distance = parent->distance + 1;
        res->fiftyval = id1;
        find_emeraldval(res, parent, d);
        // res->c.connect(d2, parent, d, false);
        }
      
      else if(other[pos] == ('A' + d) && other[pos+1] == ',') {
        res = tailored_alloc<heptagon> (S7);
        res->c7 = nullptr;
        res->alt = parent->alt;
        res->cdata = nullptr;
        res->fieldval = fv;
        res->distance = parent->distance - 1;
        vector<int> possible;
        int pfv = parent->fieldval;
        if(geometry == gSpace535) pfv = 0;
        for(auto s: nonlooping_earlier_states[address{pfv, id}]) possible.push_back(s.second);
        id1 = hrand_elt(possible, 0);
        res->fiftyval = id1;
        find_emeraldval(res, parent, d);
        }

      else {
        heptagon *at = parent;
        while(other[pos] != ',') {
          int dir = (other[pos++] & 31) - 1;
          // println(hlog, "from ", at, " go dir ", dir);
          at = at->cmove(dir);
          }
        res = at;
        }
      
      if(!res) throw "res missing";
      
      if(res->move(d2)) println(hlog, "res conflict");

      res->c.connect(d2, parent, d, false);
      return res;
      }

    ~hrmap_reg3_rule() {
      if(quotient_map) delete quotient_map;
      clearfrom(origin);
      }
    
    void draw() override {
      sphereflip = Id;
      
      // for(int i=0; i<S6; i++) queuepoly(ggmatrix(cwt.at), shWall3D[i], 0xFF0000FF);
      
      dq::visited.clear();
      dq::enqueue(centerover->master, cview());
      
      while(!dq::drawqueue.empty()) {
        auto& p = dq::drawqueue.front();
        heptagon *h = get<0>(p);
        transmatrix V = get<1>(p);
        dynamicval<ld> b(band_shift, get<2>(p));
        bandfixer bf(V);
        dq::drawqueue.pop();
        
        
        cell *c = h->c7;
        if(!do_draw(c, V)) continue;
        drawcell(c, V);
        if(in_wallopt() && isWall3(c) && isize(dq::drawqueue) > 1000) continue;
    
        for(int i=0; i<S7; i++) if(h->move(i)) {
          dq::enqueue(h->move(i), V * adj(h, i));
          }
        }
      }
    
    transmatrix adj(heptagon *h, int d) override {
      return quotient_map->adj(h, d);
      }
     
    transmatrix relative_matrix(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
      return relative_matrix_recursive(h2, h1);
      }
    
    vector<hyperpoint> get_vertices(cell* c) override {
      return cgi.vertices_only;
      }
    };

  struct hrmap_reg3_rule_alt : hrmap {
    
    heptagon *origin;
    
    hrmap_reg3_rule_alt(heptagon *o) {
      origin = o;
      }

    };

EX hrmap *new_alt_map(heptagon *o) {
  return new hrmap_reg3_rule_alt(o);
  }

EX void link_structures(heptagon *h, heptagon *alt, hstate firststate) {
  auto cm = (hrmap_reg3_rule*) currentmap;
  alt->fieldval = h->fieldval;
  if(geometry == gSpace535) alt->fieldval = 0;
  if(firststate == hsOrigin) {
    alt->fiftyval = cm->root[alt->fieldval];
    return;
    }
  vector<int>& choices = cm->possible_states[alt->fieldval];
  vector<int> choices2;
  for(auto c: choices) {
    bool ok = true;
    for(int d=0; d<12; d++) 
      if(h->cmove(d)->distance < h->distance)
        if(cm->children[S7*c+d] == -1)
          ok = false;
    if(ok) choices2.push_back(c);
    }
  alt->fiftyval = hrand_elt(choices2, -1);
  }

EX bool reg3_rule_available = true;

EX bool in_rule() {
  return reg3_rule_available && among(geometry, gSpace534, gSpace435, gSpace535);
  }
  
EX hrmap* new_map() {
  if(geometry == gSeifertCover) return new seifert_weber::hrmap_seifert_cover;
  if(geometry == gSeifertWeber) return new seifert_weber::hrmap_singlecell(108*degree);
  if(geometry == gHomologySphere) return new seifert_weber::hrmap_singlecell(36*degree);
  if(quotient && !sphere) return new hrmap_field3(&currfp);
  if(in_rule()) return new hrmap_reg3_rule;
  return new hrmap_reg3;
  }

hrmap_reg3* regmap() {
  return ((hrmap_reg3*) currentmap);
  }

EX int quotient_count() {
  return isize(regmap()->quotient_map->allh);
  }

/** This is a generalization of hyperbolic_celldistance in expansion.cpp to three dimensions.
    It still assumes that there are at most 4 cells around every edge, and that distances from
    the origin are known, so it works only in {5,3,4}.
 */

int celldistance_534(cell *c1, cell *c2) {
  int d1 = celldist(c1);
  int d2 = celldist(c2);

  vector<cell*> s1 = {c1};
  vector<cell*> s2 = {c2};
  int best = 99999999;
  int d0 = 0;

  auto go_nearer = [&] (vector<cell*>& v, int& d) {
    vector<cell*> w;
    for(cell *c: v) 
      forCellEx(c1, c) 
        if(celldist(c1) < d)
          w.push_back(c1);
    sort(w.begin(), w.end());
    d--; d0++;
    auto last = std::unique(w.begin(), w.end());
    w.erase(last, w.end());
    v = w;
    };

  while(d0 < best) {  
    for(cell *a1: s1) for(cell *a2: s2) {
      if(a1 == a2) best = min(best, d0);
      else if(isNeighbor(a1, a2)) best = min(best, d0+1);
      }

    if(d1 == 0 && d2 == 0) break;

    if(d1 >= d2) go_nearer(s1, d1);
    if(d1 < d2) go_nearer(s2, d2);
    }
  
  return best;  
  }


EX int celldistance(cell *c1, cell *c2) {
  if(c1 == c2) return 0;
  if(c1 == currentmap->gamestart()) return c2->master->distance;
  if(c2 == currentmap->gamestart()) return c1->master->distance;
  
  if(geometry == gSpace534) return celldistance_534(c1, c2);

  auto r = regmap();

  hyperpoint h = tC0(r->relative_matrix(c1->master, c2->master, C0));
  int b = bucketer(h);
  if(cgi.close_distances.count(b)) return cgi.close_distances[b];
  
  if(in_rule())
    return clueless_celldistance(c1, c2);

  dynamicval<eGeometry> g(geometry, gBinary3);  
  return 20 + bt::celldistance3(r->reg_gmatrix[c1->master].first, r->reg_gmatrix[c2->master].first);
  }

EX bool pseudohept(cell *c) {
  auto m = regmap();
  if(cgflags & qSINGLE) return true;
  if(fake::in()) return FPIU(reg3::pseudohept(c));
  if(sphere) {
    hyperpoint h = tC0(m->relative_matrix(c->master, regmap()->origin, C0));
    if(S7 == 12) {
      hyperpoint h1 = cspin(0, 1, atan2(16, 69) + M_PI/4) * h;
      for(int i=0; i<4; i++) if(abs(abs(h1[i]) - .5) > .01) return false;
      return true;
      }
    if(S7 == 8)
      return h[3] >= .99 || h[3] <= -.99 || abs(h[3]) < .01; 
    if(cgi.loop == 3 && cgi.face == 3 && S7 == 4)
      return c == m->gamestart();
    if(cgi.loop == 4 && cgi.face == 3)
      return abs(h[3]) > .9;
    if(cgi.loop == 3 && cgi.face == 4)
      return abs(h[3]) > .9;
    if(cgi.loop == 5 && cgi.face == 3)
      return abs(h[3]) > .99 || abs(h[0]) > .99 || abs(h[1]) > .99 || abs(h[2]) > .99;
    }
  // chessboard pattern in 534
  if(geometry == gField534) 
    return hr::celldistance(c, currentmap->gamestart()) & 1;
  if(geometry == gCrystal344 || geometry == gCrystal534 || geometry == gSeifertCover)
    return false;
  if(quotient) return false; /* added */
  auto mr = dynamic_cast<hrmap_reg3_rule*> (currentmap);
  if(mr) {
    if(geometry == gSpace535) 
      return c->master->fieldval % 31 == 0;
    return c->master->fieldval == 0;
    }
  if(m && hyperbolic) {
    heptagon *h = m->reg_gmatrix[c->master].first;
    return (h->zebraval == 1) && (h->distance & 1);
    }    
  
  return false;
  }

EX void generate_cellrotations() {
  auto &cr = cgi.cellrotations;
  if(isize(cr)) return;

  for(int a=0; a<S7; a++)
  for(int b=0; b<S7; b++)
  for(int c=0; c<S7; c++) {
    transmatrix T = build_matrix(cgi.adjmoves[a]*C0, cgi.adjmoves[b]*C0, cgi.adjmoves[c]*C0, C0);
    if(abs(det(T)) < 0.001) continue;
    transmatrix U = build_matrix(cgi.adjmoves[0]*C0, cgi.adjmoves[1]*C0, cgi.adjmoves[2]*C0, C0);
    transmatrix S = U * inverse(T);
    if(abs(det(S) - 1) > 0.01) continue;    
    vector<int> perm(S7);
    for(int x=0; x<S7; x++) perm[x] = -1;
    for(int x=0; x<S7; x++)
    for(int y=0; y<S7; y++)
      if(hdist(S * cgi.adjmoves[x] * C0, cgi.adjmoves[y] * C0) < .1) perm[x] = y;
    bool bad = false;
    for(int x=0; x<S7; x++) if(perm[x] == -1) bad = true;
    if(bad) continue;
    
    cr.emplace_back(geometry_information::cellrotation_t{S, perm, 0});
    }
  
  int rots = isize(cr);
  for(int i=0; i<rots; i++)
    for(int j=0; j<rots; j++)
      if(cr[i].mapping[cr[j].mapping[0]] == 0 && cr[i].mapping[cr[j].mapping[1]] == 1 && cr[i].mapping[cr[j].mapping[2]] == 2)
        cr[i].inverse_id = j;
  }
#endif

#if 0
/* More precise, but very slow distance. Not used/optimized for now */

ld adistance(cell *c) {  
  hyperpoint h = tC0(regmap()->reg_gmatrix[c->master].second);
  h = bt::deparabolic3(h);
  return regmap()->reg_gmatrix[c->master].first->distance * log(2) - h[0];
  }

unordered_map<pair<cell*, cell*>, int> memo;

bool cdd;

int celldistance(cell *c1, cell *c2) {
  if(memo.count(make_pair(c1, c2))) return memo[make_pair(c1, c2)];
  if(c1 == c2) return 0;
  vector<cell*> v[2];
  v[0].push_back(c1);
  v[1].push_back(c2);
  
  int steps = 0;
  
  map<cell*, int> visited;
  visited[c1] = 1;
  visited[c2] = 2;
  
  while(true) {
    if(cdd) {
      println(hlog, "state ", steps, "/",isize(v[0]), "/", isize(v[1]));
      println(hlog, "  A: ", v[0]);
      println(hlog, "  B: ", v[1]); 
      }
    for(int i: {0,1}) {
      vector<cell*> new_v;
      for(cell *c: v[i]) forCellCM(cn, c) if(adistance(cn) < adistance(c)) {
        auto &vi = visited[cn];
        if((vi&3) == 0) {
          vi = 4 * (steps+1);
          vi |= (1<<i);
          new_v.push_back(cn);
          }
        else if((vi&3) == 2-i) {
          vector<pair<cell*, int>> ca1, ca2;
          int b1 = 4*steps-4;
          int b2 = ((vi>>2)<<2) - 4;
          for(auto p: visited) {
            if(cdd) println(hlog, p);
            int ps = p.second & 3;
            if(ps == 1+i && p.second >= b1)
              ca1.emplace_back(p.first, p.second/4);
            if(ps == 2-i && p.second >= b2 && p.second <= b2+8)
              ca2.emplace_back(p.first, p.second/4);
            }
          int bound = 1<<16;
          for(auto p1: ca1) for(auto p2: ca2) {
            hyperpoint h = tC0(relative_matrix(p1.first->master, p2.first->master));
            int b = bucketer(h);
            if(close_distances.count(b)) {
              int d = close_distances[b] + p1.second + p2.second;
              if(cdd) println(hlog, "candidate: close=", close_distances[b], p1, p2, "; h = ", h);
              if(d < bound) bound = d;
              }
            else if(cdd) println(hlog, "bucket missing");
            }
          return memo[make_pair(c1, c2)] = bound;
          return bound;
          }
        }
      v[i] = std::move(new_v);
      }
    steps++;
    }
  }

cellwalker target;
int tsteps;

int dist_alt(cell *c) {
  if(!target.at) {
    target = cellwalker(currentmap->gamestart(), 0);
    tsteps = 0;
    for(int i=0; i<30; i++) target += wstep, target += rev, tsteps++;
    }
  if(specialland == laCamelot) return reg3::celldistance(c, target.at);
  else {
    int d = reg3::celldistance(c, target.at) - tsteps;
    if(d < 10) target += wstep, target += rev, tsteps++;
    return d;
    }
  }
#endif

// Construct a cellwalker in direction j from cw.at, such that its direction is as close
// as possible to cw.spin. Assume that j and cw.spin are adjacent

#if MAXMDIM >= 4
EX cellwalker strafe(cellwalker cw, int j) {
  hyperpoint hfront = tC0(cgi.adjmoves[cw.spin]);
  cw.at->cmove(j);
  transmatrix T = currentmap->adj(cw.at, j);
  for(int i=0; i<S7; i++) if(i != cw.at->c.spin(j))
    if(hdist(hfront, T * tC0(cgi.adjmoves[i])) < cgi.strafedist + .01)
      return cellwalker(cw.at->cmove(j), i);
  println(hlog, "incorrect strafe");
  exit(1);
  }

EX int matrix_order(const transmatrix A) {
  transmatrix T = A;
  int res = 1;
  while(!eqmatrix(T, Id)) {
    res++; T = T * A;
    }
  return res;
  }

EX void generate_fulls() {
  reg3::generate_cellrotations();

  auto cons = [&] (int i0, int i1, int i2) {
    transmatrix T = build_matrix(cgi.adjmoves[ 0]*C0, cgi.adjmoves[ 1]*C0, cgi.adjmoves[ 2]*C0, C0);
    transmatrix U = build_matrix(cgi.adjmoves[i0]*C0, cgi.adjmoves[i1]*C0, cgi.adjmoves[i2]*C0, C0);
    return U * inverse(T);
    };
  
  cgi.full_P = cgi.adjmoves[0];
  cgi.full_R = S7 == 8 ? cons(1, 7, 0) : S7 == 20 ? cons(1,2,6) : cons(1, 2, 0);
  cgi.full_X = S7 == 8 ? cons(1, 0, 6) : S7 == 6 ? cons(1, 0, 5) : S7 == 20 ? cons(1,0,7) : cons(1, 0, cgi.face);
  
  cgi.xp_order = matrix_order(cgi.full_X * cgi.full_P);
  cgi.r_order = matrix_order(cgi.full_R);
  cgi.rx_order = matrix_order(cgi.full_R * cgi.full_X);
  println(hlog, "orders = ", tie(cgi.rx_order, cgi.r_order, cgi.xp_order));
  }

EX void construct_relations() {
  auto& rels = cgi.rels;
  if(!rels.empty()) return;
  rels.clear();

  reg3::generate_cellrotations();
  reg3::generate_fulls();
  vector<transmatrix> all;

  vector<string> formulas;
  
  formulas.push_back("");

  all.push_back(Id);
  hyperpoint v = cgi.cellshape[0];
  auto add = [&] (transmatrix T) {
    for(int i=0; i<isize(all); i++) if(eqmatrix(all[i], T)) return i;
    int S = isize(all);
    all.push_back(T);
    return S;
    };
  
  println(hlog, cgi.cellshape);

  println(hlog, "cellshape = ", isize(cgi.cellshape));
  bool ok = true;
  int last_i = -1;
  for(hyperpoint h: cgi.cellshape) {
    int i = 0, j = 0;
    for(hyperpoint u: cgi.cellshape) if(hdist(h, cgi.full_X*u) < 5e-2) i++;
    for(hyperpoint u: cgi.cellshape) if(hdist(h, cgi.full_R*u) < 5e-2) j++;
    if(last_i == -1) last_i = i;
    if(i != j || i != last_i) ok = false;
    }
  
  if(!ok) { println(hlog, "something wrong"); exit(1); }
  
  add(Id);
  
  auto work = [&] (transmatrix T, int p, char c) {
    if(hdist0(tC0(T)) > 5) return;
    for(hyperpoint h: cgi.cellshape) if(hdist(T * h, v) < 1e-4) goto ok;
    return;
    ok:
    int id = add(T);
    // println(hlog, p, " x ", (s0+c), " = ", id);

    if(id >= isize(formulas)) formulas.push_back(formulas[p] + c);
    else if(id == 0) println(hlog, "reached identity: ", formulas[p]+c);
    else if(formulas[p][0] != formulas[id][0])
      rels.emplace_back(formulas[p] + c, formulas[id]);
    };
  
  for(int i=0; i<isize(all); i++) {
    transmatrix T = all[i];
    work(T * cgi.full_R, i, 'R');
    work(T * cgi.full_X, i, 'X');
    work(T * cgi.full_P, i, 'P');
    }  
  }

EX }
#endif

#if MAXMDIM == 3
EX namespace reg3 {
EX bool in() { return false; }
EX }
#endif

}

