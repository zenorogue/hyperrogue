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

EX hyperpoint final_coords(hyperpoint h) {
  if(sn::in() || !bt::in())
    return ultra_normalize(h);
  #if CAP_BT
  if(bt::in() && !mproduct)
    return bt::bt_to_minkowski(h);
  #endif
  return h;
  }

void subcellshape::compute_common() {
  reg3::make_vertices_only(vertices_only, faces);

  faces_local = faces;
  for(auto& face: faces_local) for(auto& v: face) v = from_cellcenter * final_coords(v);

  vertices_only_local = vertices_only;
  for(auto& v: vertices_only_local) v = from_cellcenter * final_coords(v);

  int N = isize(faces);

  dirdist.resize(N);
  for(int i=0; i<N; i++) {
    auto& da = dirdist[i];
    da.resize(N, false);
    set<unsigned> cface;
    for(auto& v: faces[i]) cface.insert(bucketer(v));
    for(int j=0; j<N; j++) {
      int mutual = 0;
      for(auto& w: faces[j]) if(cface.count(bucketer(w))) mutual++;
      da[j] = i == j ? 0 : mutual == 2 ? 1 : INFD;
      }
    }
  floyd_warshall(dirdist);

  next_dir.resize(N);
  for(int a=0; a<N; a++) next_dir[a].resize(N);
  
  for(int a=0; a<N; a++)
  for(int b=0; b<N; b++)
    if(dirdist[a][b] == 1) 
      for(int c=0; c<N; c++)
        if(dirdist[a][c] == 1 && dirdist[b][c] == 1) {
          transmatrix t = build_matrix(tC0(cgi.adjmoves[a]), tC0(cgi.adjmoves[b]), tC0(cgi.adjmoves[c]), C0);
          if(det(t) > 0) next_dir[a][b] = c;
          }  
  }

void subcellshape::compute_hept() {
  cellcenter = C0;
  to_cellcenter = Id;
  from_cellcenter = Id;
  compute_common();
  }

EX namespace reg3 {
EX void make_vertices_only(vector<hyperpoint>& vo, const vector<vector<hyperpoint>>& csh) {
  vo.clear();
  for(auto& v: csh)
  for(hyperpoint h: v) {
    bool found = false;
    for(hyperpoint h2: vo) if(hdist(h, h2) < 1e-6) found = true;
    if(!found) vo.push_back(h);
    }
  }
EX }

transmatrix sfix_rgpushxto0(hyperpoint h) {
  if(stretch::applicable() && !fake::in()) return stretch::translate(h);
  return rgpushxto0(h);
  };

transmatrix sfix_gpushxto0(hyperpoint h) {
  if(stretch::applicable() && !fake::in()) return stretch::itranslate(h);
  return gpushxto0(h);
  };

#if MAXMDIM >= 4
void subcellshape::compute_sub() {
  hyperpoint gres = Hypc;
  for(auto& face: faces) {
    hyperpoint res = Hypc;
    for(auto& vertex: face) 
      res += vertex;
    face_centers.push_back(normalize(res));
    gres += res;
    }
  cellcenter = normalize(gres);
  to_cellcenter = sfix_rgpushxto0(cellcenter);
  from_cellcenter = sfix_gpushxto0(cellcenter);
  compute_common();
  }

/** \brief regular three-dimensional tessellations */
EX namespace reg3 {

  EX int subcube_count = 1;
  
  EX flagtype coxeter_param = 0;

  const flagtype cox_othercell = 1;
  const flagtype cox_midedges = 2;
  const flagtype cox_vertices = 4;

  #if HDR
  inline short& altdist(heptagon *h) { return h->emeraldval; }
  #endif
  
  EX int extra_verification;
  
  EX bool ultra_mirror_on;

  EX bool ultra_mirror_in() { return (cgflags & qULTRA) && ultra_mirror_on; }
  
  EX bool cubes_reg3;

  EX bool in() {
    if(fake::in()) return FPIU(in());
    if(geometry == gCubeTiling && (cubes_reg3 || !PURE)) return true;
    return WDIM == 3 && !euclid && !bt::in() && !nonisotropic && !mhybrid && !kite::in();
    }

  EX void compute_ultra() {
    cgi.ultra_mirror_part = .99;    
    cgi.ultra_material_part = .99;
    
    cgi.ultra_mirrors.clear();

    if(cgflags & qULTRA) {
    
      for(auto& v: cgi.heptshape->vertices_only) {
      
        hyperpoint nei;
        auto& faces = cgi.heptshape->faces;
      
        for(int i=0; i<isize(faces); i++)
        for(int j=0; j<isize(faces[i]); j++)
          if(sqhypot_d(WDIM, faces[i][j]-v) < 1e-6)
            nei = faces[i][j?j-1:j+1]; 
            
        transmatrix T = spintox(v);
        hyperpoint a = T * v;
        hyperpoint b = T * nei;
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
        
        if(cgi.ultra_mirror_part >= 1-1e-6) continue;
        
        cgi.ultra_mirrors.push_back(rspintox(v) * xpush(cgi.ultra_mirror_dist*2) * MirrorX * spintox(v));
        }
      }    
    }

  EX void generate() {

    if(fake::in()) {
      fake::generate();
      return;
      }
  
    auto& hsh = get_hsh();
    
    int& loop = cgi.loop;
    int& face = cgi.face;
    auto& spins = cgi.spins;
    auto& cellshape = hsh.faces;
    auto& adjcheck = cgi.adjcheck;
  
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
      angle_between_faces = edge_of_triangle_with_angles(TAU/mid, M_PI/face, M_PI/face);
      
      h0 = xtangent(1);
      h1 = cspin(0, 1, angle_between_faces) * h0;
      h2 = cspin(1, 2, TAU/face) * h1;
      h3 = cspin(1, 2, -TAU/face) * h1;

      hcrossf = edge_of_triangle_with_angles(90._deg, M_PI/mid, M_PI/face);

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
      for(int i=0; i<face; i++) h += d * (cspin(1, 2, TAU*i/face) * h012);
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

    if(euclid) klein_scale = sqrt(3)/2;
    
    /* actual vertex */
    hyperpoint v2 = C0 + klein_scale * h012;

    hyperpoint midface = Hypc;
    for(int i=0; i<face; i++) midface += cspin(1, 2, TAU * i/face) * v2;
    midface = normalize(midface);
    ld between_centers = 2 * hdist0(midface);
    DEBB(DF_GEOM, ("between_centers = ", between_centers));
    
    if(S7 == 20) {
      spins[0] = Id;
      spins[1] = cspin(0, 1, angle_between_faces) * cspin180(1, 2);
      spins[2] = spins[1] * cspin(1, 2, -TAU/face) * spins[1];
      spins[3] = spins[1] * cspin(1, 2, +TAU/face) * spins[1];
      for(int a=4; a<10; a++) spins[a] = cspin(1, 2, TAU/face) * spins[a-3];
      for(int a=S7/2; a<S7; a++) spins[a] = spins[a-S7/2] * spin180();
      }

    if(S7 == 12 || S7 == 8) {
      spins[0] = Id;
      spins[1] = cspin(0, 1, angle_between_faces) * cspin180(1, 2);
      for(int a=2; a<face+1; a++) spins[a] = cspin(1, 2, TAU*(a-1)/face) * spins[1];
      for(int a=S7/2; a<S7; a++) spins[a] = cspin180(0, 1) * spins[a-S7/2];
      if(S7 == 8) swap(spins[6], spins[7]);
      if(S7 == 12) swap(spins[8], spins[11]);
      if(S7 == 12) swap(spins[9], spins[10]);
      }
    
    if(S7 == 6) {
      spins[0] = Id;
      spins[1] = cspin(0, 1, angle_between_faces) * cspin180(1, 2);
      spins[2] = cspin90(1, 2) * spins[1];
      for(int a=S7/2; a<S7; a++) spins[a] = spins[a-S7/2] * cspin180(0, 1);
      }
    
    if(S7 == 4) {
      spins[0] = Id;
      spins[1] = cspin(0, 1, angle_between_faces) * cspin180(1, 2);
      for(int a=2; a<face+1; a++) spins[a] = cspin(1, 2, TAU*(a-1)/face) * spins[1];      
      }
    
    cellshape.clear();
    cellshape.resize(S7);
    for(int a=0; a<S7; a++) {
      for(int b=0; b<face; b++)
        cellshape[a].push_back(spins[a] * cspin(1, 2, TAU*b/face) * v2);
      }
    
    cgi.adjmoves[0] = cpush(0, between_centers) * cspin180(0, 2);
    for(int i=1; i<S7; i++) cgi.adjmoves[i] = spins[i] * cgi.adjmoves[0];

    for(int a=0; a<S7; a++)
      DEBB(DF_GEOM, ("center of ", a, " is ", kz(tC0(cgi.adjmoves[a]))));
    
    DEBB(DF_GEOM, ("doublemove = ", kz(tC0(cgi.adjmoves[0] * cgi.adjmoves[0]))));

    adjcheck = hdist(tC0(cgi.adjmoves[0]), tC0(cgi.adjmoves[1])) * 1.0001;

    if(loop == 4) cgi.strafedist = adjcheck;
    else cgi.strafedist = hdist(cgi.adjmoves[0] * C0, cgi.adjmoves[1] * C0);
    
    if(stretch::applicable()) {
      transmatrix T = cspin90(0, 2);
      transmatrix iT = inverse(T);
      for(auto& v: cgi.adjmoves) v = T * v * iT;
      for(auto& vv: cellshape) for(auto& v: vv) v = T * v;
      }

    hsh.compute_hept();
    compute_ultra();
    
    generate_subcells();
    }

  EX void generate_plain_subcubes() {
    if(S7 != 6) throw hr_exception("generate_plain_subcubes but no cubes");
    auto& ssh = cgi.subshapes;
    const int sub = subcube_count;
    auto vx = abs(cgi.heptshape->faces[0][0][0]);
    auto vz = abs(cgi.heptshape->faces[0][0][3]);
    for(int x=1-sub; x<sub; x+=2)
    for(int y=1-sub; y<sub; y+=2)
    for(int z=1-sub; z<sub; z+=2) {
      ssh.emplace_back();
      auto &ss = ssh.back();
      ss.faces = cgi.heptshape->faces;
      for(auto& face: ss.faces) for(auto& v: face) {
        v[0] += vx * x;
        v[1] += vx * y;
        v[2] += vx * z;
        v[3] += vz * (sub-1);
        }
      }
    }
  
  EX void generate_coxeter(flagtype f) {
    auto& ssh = cgi.subshapes;
    for(auto& fac: cgi.heptshape->faces) {
      hyperpoint facectr = Hypc;
      vector<hyperpoint> ring;
      hyperpoint last = fac.back();
      ring.push_back(last);
      for(hyperpoint h: fac) {
        if(f & cox_midedges)
          ring.push_back(mid(last, h));
        ring.push_back(last = h);
        facectr += h;
        }
      facectr = normalize(facectr);

      hyperpoint fc2 = rspintox(facectr) * xpush0(2*hdist0(facectr));
      
      if(f & cox_vertices) {
        for(int i=1; i<isize(ring); i++) {
          ssh.emplace_back();
          auto &ss = ssh.back();
          auto h = (f & cox_othercell) ? facectr : fc2;
          ss.faces.push_back({C0, h, ring[i-1]});
          ss.faces.push_back({C0, h, ring[i]});
          ss.faces.push_back({C0, ring[i-1], ring[i]});
          ss.faces.push_back({h, ring[i-1], ring[i]});
          }
        }
      else if(f & cox_midedges) {
        ring.push_back(ring[1]);
        for(int i=3; i<isize(ring); i+=2) {
          ssh.emplace_back();
          auto &ss = ssh.back();
          auto h = (f & cox_othercell) ? facectr : fc2;
          ss.faces.push_back({C0, ring[i-2], ring[i-1]});
          ss.faces.push_back({C0, ring[i-1], ring[i-0]});
          ss.faces.push_back({C0, h, ring[i-2]});
          ss.faces.push_back({C0, h, ring[i-0]});
          if(f & cox_othercell) {
            ss.faces.push_back({facectr, ring[i-2], ring[i-1], ring[i-0]});
            }
          else {
            ss.faces.push_back({fc2, ring[i-1], ring[i-0]});
            ss.faces.push_back({fc2, ring[i-2], ring[i-1]});
            }
          }
        }
      else {
        ssh.emplace_back();
        auto &ss = ssh.back();
        for(int i=1; i<isize(ring); i++)
          ss.faces.push_back({C0, ring[i-1], ring[i]});
        if(f & cox_othercell) {
          ring.pop_back();
          ss.faces.push_back(ring);
          }
        else {
          for(int i=1; i<isize(ring); i++)
            ss.faces.push_back({fc2, ring[i-1], ring[i]});
          }
        }
      }
    }
  
  EX void generate_special_subcubes(bool bch) {
    if(S7 != 6) throw hr_exception("generate_plain_subcubes but no cubes");
    const int sub = subcube_count;
    if(1) {
      auto vx = abs(cgi.heptshape->faces[0][0][0]);
      auto vz = abs(cgi.heptshape->faces[0][0][3]);
      auto step = hdist0(tC0(cgi.adjmoves[0]));
      array<int, 3> co;
      int s = bch ? 1 : 2;
      for(co[0]=-sub; co[0]<=sub; co[0]+=s)
      for(co[1]=-sub; co[1]<=sub; co[1]+=s)
      for(co[2]=-sub; co[2]<=sub; co[2]+=s) 
      if(((co[0]^co[1]^1)&1) && ((co[0]^co[2]^1)&1)) {
        hyperpoint ctr = Hypc;
        ctr[3] = vz * sub;
        
        struct direction {
          hyperpoint dir;
          int limit;
          transmatrix mirror;
          void flip() { dir = -dir; limit = 200 - limit; }
          };          
        
        array<direction, 3> di;

        int mi = 0;
        
        for(int i=0; i<3; i++) {
          ctr[i] += co[i] * vx;
          auto& dii = di[i];
          if(co[i] >= 0) {
            dii.dir = ctangent(i, vx);
            dii.limit = sub - co[i];
            dii.mirror = cpush(i, +step/2) * cmirror(i) * cpush(i, -step/2);
            }
          else {
            dii.dir = ctangent(i, -vx);
            dii.limit = co[i] + sub;
            dii.mirror = cpush(i, -step/2) * cmirror(i) * cpush(i, +step/2);
            }
          if(dii.limit == 0) mi++;
          }
        
        sort(di.begin(), di.end(), [] (direction& d1, direction& d2) { return d1.limit > d2.limit; });

        cgi.subshapes.emplace_back();
        auto &ss = cgi.subshapes.back();

        auto pt0 = [&] (const array<ld, 3>& x) {
          transmatrix M = Id;
          hyperpoint res = ctr;
          for(int i=0; i<3; i++) {
            ld xx = x[i];
            if(xx > di[i].limit) xx = 2*di[i].limit-xx, M = di[i].mirror * M;
            res += di[i].dir * xx;
            }
          return normalize(M * res);
          };

        auto pt = [&] (ld x, ld y, ld z) { 
          if(sub == 1 || !bch || sphere) return pt0(make_array(x,y,z));
          
          // Unfortunately using the rule above for bch (with sub > 1) generates faces which are not flat.
          // Therefore, we replace the vertices by the centers of their Voronoi cells
          // we do this only in the hyperbolic case -- it does not work correctly in the spherical case because of Voronoi not working as expected
          
          // the arguments for pt1 are the Voronoi centers for: (x,y,z) = (1,.5,0)
          // pt1 rearranges them to whatever (x,y,z) actually is

          array<ld, 3> arg1 = {x, y, z};

          auto pt1 = [&] (ld x1, ld y1, ld z1) {
            array<ld, 3> arg0;
            for(int i=0; i<3; i++) {
              if(arg1[i] == 1) arg0[i] = x1;
              else if(arg1[i] == -1) arg0[i] = -x1;
              else if(arg1[i] == .5) arg0[i] = y1;
              else if(arg1[i] == -.5) arg0[i] = -y1;
              else if(arg1[i] == 0) arg0[i] = z1;
              else throw hr_exception("unknown number in pt1");
              }
            return normalize(pt0(arg0));
            };
          hyperpoint a = pt1(0,0,0);
          hyperpoint b = pt1(2,0,0);
          hyperpoint c = pt1(1,1,1);
          hyperpoint d = pt1(1,1,-1);
          hyperpoint res = circumscribe(a, b, c, d);
          return res;
          };
        
        auto add_face = [&] (std::initializer_list<hyperpoint> vh) {
          ss.faces.push_back(vh);
          };
        
        const ld h = .5;
        
        if(mi == 0) {
          for(int s: {-1, 1}) {
            for(int i=0; i<3; i++) {
              if(bch)
                add_face({pt(0,.5,s), pt(.5,0,s), pt(0,-.5,s), pt(-.5,0,s)});
              else
                add_face({pt(-1,-1,s), pt(-1,+1,s), pt(+1,+1,s), pt(+1,-1,s)});
              tie(di[0], di[1], di[2]) = make_tuple(di[1], di[2], di[0]);
              }
            }
          if(bch) for(int u=0; u<8; u++) {
            for(int j=0; j<3; j++) if((u>>j)&1) di[j].flip();
            add_face({pt(0,.5,1), pt(0,1,.5), pt(.5,1,0), pt(1,.5,0), pt(1,0,.5), pt(.5,0,1)});
            for(int j=0; j<3; j++) if((u>>j)&1) di[j].flip();
            }
          }
        else if(mi == 1) {
          auto& M = di[2].mirror;
          for(int s: {-1, 1}) {
            if(bch)
              add_face({pt(0,h,s), pt(h,0,s), pt(0,-h,s), pt(-h,0,s)});
            else
              add_face({pt(-1,-1,s), pt(-1,+1,s), pt(+1,+1,s), pt(+1,-1,s)});
            for(int i=0; i<2; i++) {
              if(bch)
                add_face({pt(1,0,-.5), pt(1,-.5,0), M*pt(1,0,-.5), pt(1,.5,0)});
              else
                add_face({pt(-1,-1,-1), pt(-1,+1,-1), pt(-1,+1,+1), pt(-1,-1,+1)});
              tie(di[0], di[1]) = make_tuple(di[1], di[0]); di[0].flip();
              }
            }
          if(bch) for(ld s: {-1, 1}) for(int i=0; i<4; i++) {
            add_face({pt(0,.5,s), pt(0,1,s/2), pt(.5,1,0), pt(1,.5,0), pt(1,0,s/2), pt(.5,0,s)});
            tie(di[0], di[1]) = make_tuple(di[1], di[0]); di[0].flip();
            }
          }
        else {
          transmatrix spi = mi == 2 ? di[1].mirror * di[2].mirror : di[0].mirror * di[1].mirror;
          if(cgi.loop == 5) spi = spi * spi;
          vector<transmatrix> spi_power = {Id};
          for(int i=1; i<cgi.loop; i++) spi_power.push_back(spi_power.back() * spi);
          if(mi == 2) {
            for(auto P: spi_power) {
              if(bch)
                add_face({P*pt(.5,0,-1), P*pt(0,-.5,-1), P*pt(-.5,0,-1), P*pt(0,.5,-1)});
              else
                add_face({P*pt(-1,-1,-1), P*pt(1,-1,-1), P*spi*pt(1,-1,-1), P*spi*pt(-1,-1,-1)});
              }
            vector<hyperpoint> f0, f1;
            for(auto P: spi_power) f0.push_back(bch ? P*pt(-1,-.5,0) : P*pt(-1,-1,-1));
            for(auto P: spi_power) f1.push_back(bch ? P*pt(+1,-.5,0) : P*pt(+1,-1,-1));
            ss.faces.push_back(f0);
            ss.faces.push_back(f1);
            
            if(bch) for(auto P: spi_power) for(int s: {-1,1})
              add_face({P*pt(-.5*s,0,-1), P*pt(0,-.5,-1), P*pt(0,-1,-.5), P*pt(-.5*s,-1,0), P*pt(-1*s,-.5,0), P*pt(-1*s,0,-.5)});
            }
          else {
            vector<transmatrix> face_dirs = {Id};
            for(int i=0; i<isize(face_dirs); i++)
            for(int j=0; j<2; j++) 
            for(auto P1: spi_power) {
              auto T = face_dirs[i];
              if(j == 0) T = T * P1 * di[1].mirror * di[2].mirror;
              if(j == 1) T = T * P1 * di[2].mirror * di[0].mirror;
              bool fnd = false;
              for(auto& F: face_dirs) 
              for(auto P: spi_power)
                if(eqmatrix(T, F*P)) fnd = true;
              if(!fnd) face_dirs.push_back(T);
              }
            // tetrahedron in {4,3,3}; dodecahedron in {4,3,5}
            if(cgi.loop == 3) hassert(isize(face_dirs) == 4);
            if(cgi.loop == 5) hassert(isize(face_dirs) == 12);
            for(auto F: face_dirs) {
              vector<hyperpoint> f0;
              for(auto P: spi_power) f0.push_back(bch ? F*P*pt(-.5,0,-1) : F*P*pt(-1,-1,-1));
              ss.faces.push_back(f0);
              }

            vector<transmatrix> vertex_dirs;
            hyperpoint pter = normalize(pt0(make_array(-.5,-.5,-.5)));
            for(auto& F: face_dirs) for(auto& P: spi_power) {
              transmatrix T = F * P;
              bool fnd = false;
              for(auto T1: vertex_dirs) if(hdist(T * pter, T1*pter) < 1e-3) fnd = true;
              if(!fnd) vertex_dirs.push_back(T);
              }
            if(cgi.loop == 3) hassert(isize(vertex_dirs) == 4);
            if(cgi.loop == 5) hassert(isize(vertex_dirs) == 20);
            if(bch) for(auto& V: vertex_dirs)
              add_face({V*pt(-1,-.5,0), V*pt(-1,0,-.5), V*pt(-.5,0,-1), V*pt(0,-.5,-1), V*pt(0,-1,-.5), V*pt(-.5,-1,0)});
            }
          }
        }
      }
    }

  EX void generate_bch_oct() {
    if(S7 != 6) throw hr_exception("generate_bch_oct but no cubes");
    const int sub = subcube_count;
    if(1) {
      auto vx = abs(cgi.heptshape->faces[0][0][0]);
      auto vz = abs(cgi.heptshape->faces[0][0][3]);
      array<int, 3> co;
      // vx = 1; vz = 0;
      for(co[0]=-sub; co[0]<sub; co[0]++)
      for(co[1]=-sub; co[1]<sub; co[1]++)
      for(co[2]=-sub; co[2]<sub; co[2]++) {
        auto co1 = co;
        array<ld, 3> sgn = {1,1,1};
        if((co[1] ^ co[0]) & 1) co1[1]++, sgn[1] = -1;
        if((co[2] ^ co[0]) & 1) co1[2]++, sgn[2] = -1;

        hyperpoint ctr = Hypc;
        ctr[3] = vz * sub;
        
        auto pt = [&] (int m, ld x0, ld x1, ld x2) {
          hyperpoint res = ctr;
          auto x = make_array(x0, x1, x2);
          for(int i=0; i<3; i++)
            res[i] = vx * (co1[i] + x[(m+i)%3] * sgn[i]);
          return res;
          };

        for(int it=0; it<2; it++) {        
          cgi.subshapes.emplace_back();
          auto &ss = cgi.subshapes.back();
          for(int m=0; m<3; m++) {
            ss.faces.push_back({pt(m,0,0,0), pt(m,1,0,0), pt(m,1,0,.5), pt(m,.5,0,1), pt(m,0,0,1)});
            ss.faces.push_back({pt(m,1,0,0), pt(m,1,0,.5), pt(m,1,.5,0) });
            }
          ss.faces.push_back({pt(0,1,0,.5), pt(0,1,.5,0), pt(0,.5,1,0), pt(0,0,1,.5), pt(0,0,.5,1), pt(0,.5,0,1)});
          for(int d=0; d<3; d++)
            co1[d] += sgn[d], sgn[d] *= -1;
          println(hlog, ss.faces);
          }
        }
      }
    }

  EX void generate_subcells() {

    cgi.subshapes.clear();
    
    switch(variation) {
      case eVariation::subcubes:
        generate_plain_subcubes();
        break;
      
      case eVariation::dual_subcubes:
        generate_special_subcubes(false);
        break;
      
      case eVariation::bch:
        generate_special_subcubes(true);
        break;
      
      case eVariation::bch_oct:
        generate_bch_oct();
        break;
      
      case eVariation::coxeter:
        generate_coxeter(coxeter_param);
        break;
    
      case eVariation::pure: {
        cgi.subshapes.emplace_back();
        cgi.subshapes[0].faces = cgi.heptshape->faces;
        break;
        }
    
      default:
        throw hr_exception("unknown variation in generate_subcells");
      }

    for(auto& ss: cgi.subshapes) ss.compute_sub();

    println(hlog, "subcells generated = ", isize(cgi.subshapes));
    }

  void binary_rebase(heptagon *h, const transmatrix& V) {
    }
  
  void test();

  #if HDR
  /** \brief vertex_adjacencies[heptagon id] is a list of other heptagons which are vertex adjacent 
   *  note: in case of ideal vertices this is just the face adjacency
   **/
  struct vertex_adjacency_info {
    /** id of the adjacent heptagon */
    int h_id;
    /** transition matrix to that heptagon */
    transmatrix T;
    /** the sequence of moves we need to make to get there */
    vector<int> move_sequence;
    };
  
  struct hrmap_closed3 : hrmap {
    vector<heptagon*> allh;
    vector<vector<vector<int>>> strafe_data;
    vector<vector<transmatrix>> tmatrices;    
    vector<vector<transmatrix>> tmatrices_cell;
    vector<cell*> acells;
    map<cell*, pair<int, int> > local_id; /* acells index, subshape index */
    vector<vector<cell*>> acells_by_master;
    vector<vector<vertex_adjacency_info> > vertex_adjacencies;
    vector<vector<vector<int>>> move_sequences;

    transmatrix adj(heptagon *h, int d) override { return tmatrices[h->fieldval][d]; }
    transmatrix adj(cell *c, int d) override { return tmatrices_cell[local_id.at(c).first][d]; }

    heptagon *getOrigin() override { return allh[0]; }

    transmatrix relative_matrixc(cell *h2, cell *h1, const hyperpoint& hint) override;
    
    void initialize(int cell_count);
    vector<cell*>& allcells() override { return acells; }

    ~hrmap_closed3() {
      clearfrom(getOrigin());
      }

    subcellshape& get_cellshape(cell *c) override {
      if(PURE) return *cgi.heptshape ;
      int id = local_id.at(c).second;
      return cgi.subshapes[id];
      }

    transmatrix master_relative(cell *c, bool get_inverse) override {
      int id = local_id.at(c).second;
      auto& ss = cgi.subshapes[id];
      return get_inverse ? ss.from_cellcenter : ss.to_cellcenter;
      }
    
    void make_subconnections();
    
    int wall_offset(cell *c) override;
    int shvid(cell *c) override { return local_id.at(c).second; }
    
    transmatrix ray_iadj(cell *c, int i) override;

    cellwalker strafe(cellwalker cw, int j) override {
      int id = local_id.at(cw.at).first;
      return cellwalker(cw.at->cmove(j), strafe_data[id][j][cw.spin]);
      }

    const vector<int>& get_move_seq(cell *c, int i) override {
      int id = local_id.at(c).first;
      return move_sequences[id][i];
      }
    };

  struct hrmap_quotient3 : hrmap_closed3 { };
  #endif

  transmatrix hrmap_closed3::ray_iadj(cell *c, int i) {
    if(PURE) return iadj(c, i);
    auto& v = get_face_vertices(c, i); 
    hyperpoint h = 
      project_on_triangle(v[0], v[1], v[2]);
    transmatrix T = rspintox(h);
    return T * xpush(-2*hdist0(h)) * spintox(h);
    }
  
  int hrmap_closed3::wall_offset(cell *c) {
    if(PURE) return 0;
    auto& wo = cgi.walloffsets[ local_id.at(c).second ];
    if(wo.second == nullptr)
      wo.second = c;
    return wo.first;
    }

  EX const vector<hyperpoint>& get_face_vertices(cell *c, int d) {
    return cgi.subshapes[currentmap->shvid(c)].faces_local[d];
    }

  EX int get_face_vertex_count(cell *c, int d) {
    return isize(get_face_vertices(c, d));
    }

  void hrmap_closed3::initialize(int big_cell_count) {
    allh.resize(big_cell_count);
    tmatrices.resize(big_cell_count);
    acells.clear();
    for(int a=0; a<big_cell_count; a++) {
      allh[a] = init_heptagon(S7);
      allh[a]->fieldval = a;
      }
    }

  const static bool testing_subconnections = false;
  
  void hrmap_closed3::make_subconnections() {
    auto& ss = cgi.subshapes;

    auto& vas = vertex_adjacencies;
    vas.resize(isize(allh));
    for(int a=0; a<isize(allh); a++) {
      auto& va = vas[a];
      va.emplace_back(vertex_adjacency_info{a, Id, {}});
      
      set<unsigned> buckets;
      for(auto& v: cgi.heptshape->vertices_only) buckets.insert(bucketer(v));

      if(cgflags & qIDEAL) {
        for(int d=0; d<S7; d++) {
          transmatrix T = adj(allh[a], d);
          va.emplace_back(vertex_adjacency_info{allh[a]->move(d)->fieldval, T, {d}});
          }
        }
      else
      for(int i=0; i<isize(va); i++) {
        for(int d=0; d<S7; d++) {
          transmatrix T = va[i].T * adj(allh[va[i].h_id], d);
          bool found = false;
          for(auto& va2: va) if(eqmatrix(va2.T, T)) found = true;
          if(found) continue;

          bool found_va = false;
          for(auto& w: cgi.heptshape->vertices_only)
            if(buckets.count(bucketer(T*w)))
              found_va = true;
          if(!found_va) continue;
          va.emplace_back(vertex_adjacency_info{allh[va[i].h_id]->move(d)->fieldval, T, va[i].move_sequence});
          va.back().move_sequence.push_back(d);
          }
        }
      }
    
    map<int, int> by_sides;

    vector<map<unsigned, vector<hyperpoint> > > which_cell_0;
    which_cell_0.resize(isize(allh));
    
    acells_by_master.resize(isize(allh));
    for(int a=0; a<isize(allh); a++) {
      for(int id=0; id<isize(ss); id++) {
        bool exists = false;
        auto& cc = ss[id].cellcenter;
        for(auto& va: vertex_adjacencies[a]) {
          hyperpoint h = iso_inverse(va.T) * cc;
          for(auto h1: which_cell_0[va.h_id][bucketer(h)])
            if(hdist(h1, h) < 1e-6)
              exists = true;
          }
        if(exists) continue;
        cell *c = newCell(isize(ss[id].faces), allh[a]);
        by_sides[isize(ss[id].faces)]++;
        if(!allh[a]->c7)
          allh[a]->c7 = c;
        local_id[c] = {isize(acells), id};
        acells.push_back(c);
        acells_by_master[a].push_back(c);
        which_cell_0[a][bucketer(cc)].push_back(cc);
        }
      }
    
    println(hlog, "found ", isize(acells), " cells, ", by_sides);

    tmatrices_cell.resize(isize(acells));
    move_sequences.resize(isize(acells));
    int failures = 0;
    
    vector<map<unsigned, vector<pair<cell*, int> > > > which_cell;
    which_cell.resize(isize(allh));

    for(cell *c: acells) {
      int id = local_id[c].second;
      for(int i=0; i<c->type; i++)
        which_cell[c->master->fieldval][bucketer(ss[id].face_centers[i])].emplace_back(c, i);
      }
    
    strafe_data.resize(isize(acells));
    
    for(cell *c: acells) {
      int id = local_id[c].second;      
      int cid = local_id[c].first;
      auto& tmcell = tmatrices_cell[cid];
      vector<int> foundtab;
      vector<tuple<int, int, int>> foundtab_ids;
      strafe_data[cid].resize(c->type);
      for(int i=0; i<c->type; i++) {
        int found = 0;
        hyperpoint ctr = ss[id].face_centers[i];
        transmatrix T1 = Id;
        int h_id = c->master->fieldval;
        vector<int> path;        
        while(true) {
          int d = -1;
          ld dist = hdist0(ctr);
          for(int d1=0; d1<S7; d1++) {
            auto ctr1 = iso_inverse(tmatrices[h_id][d1]) * ctr;
            ld dist1 = hdist0(ctr1);
            if(dist1 < dist - 1e-6) d = d1, dist = dist1;
            }
          if(d == -1) break;
          path.push_back(d);
          T1 = T1 * tmatrices[h_id][d];
          ctr = iso_inverse(tmatrices[h_id][d]) * ctr;
          h_id = allh[h_id]->move(d)->fieldval;
          }
        
        for(auto& va: vertex_adjacencies[h_id]) {
          hyperpoint ctr1 = iso_inverse(va.T) * ctr;
          auto bucket = bucketer(ctr1);
          for(auto p: which_cell[va.h_id][bucket]) {
            cell *c1 = p.first;
            int j = p.second;
            int id1 = local_id[c1].second;
            if(hdist(ctr1, ss[id1].face_centers[j]) < 1e-6) {
              transmatrix T2 = T1 * va.T;
              if(id == id1 && eqmatrix(T2, Id)) continue;
              c->c.connect(i, c1, j, false);
              if(!found) {                
                tmcell.push_back(ss[id].from_cellcenter * T2 * ss[id1].to_cellcenter);
                if(elliptic) fixelliptic(tmcell.back());

                auto& ms = move_sequences[local_id[c].first];
                ms.push_back(path);
                for(auto dir: va.move_sequence) ms.back().push_back(dir);

                auto& sd = strafe_data[cid][i];                
                sd.resize(c->type, -1);
                
                for(int i1=0; i1<c->type; i1++) {
                  set<unsigned> facevertices;
                  for(auto v: ss[id].faces[i1]) facevertices.insert(bucketer(v));
                  if(ss[id].dirdist[i][i1] == 1) {
                    int found_strafe = 0;
                    for(int j1=0; j1<c1->type; j1++) if(j1 != j) {
                      int num = 0;
                      for(auto v: ss[id1].faces[j1])
                        if(facevertices.count(bucketer(T2*v)))
                          num++;
                      if(num == 2) sd[i1] = j1, found_strafe++;
                      }
                    if(found_strafe != 1) println(hlog, "found_strafe = ", found_strafe);
                    }
                  }
                
                /* for bch, also provide second-order strafe */
                if(variation == eVariation::bch) for(int i1=0; i1<c->type; i1++) {
                  if(ss[id].dirdist[i][i1] != 2) continue;
                  if(isize(ss[id].faces[i]) == 6) continue;
                  if(isize(ss[id].faces[i1]) == 6) continue;
                  vector<int> fac;
                  for(int i2=0; i2<c->type; i2++) if(ss[id].dirdist[i][i2] == 1 && ss[id].dirdist[i2][i1] == 1)
                    fac.push_back(sd[i2]);
                  if(isize(fac) != 2) {
                    println(hlog, "fac= ", fac);
                    throw hr_exception("fac error");
                    }
                  int found_strafe = 0;
                  for(int j1=0; j1<c1->type; j1++) if(j1 != j)
                    if(ss[id1].dirdist[j1][fac[0]] == 1)
                    if(ss[id1].dirdist[j1][fac[1]] == 1) {
                      sd[i1] = j1;
                      found_strafe++;
                      }
                  if(found_strafe != 1) println(hlog, "found_strafe = ", found_strafe, " (second order)");
                  }
                }
              foundtab_ids.emplace_back(va.h_id, id1, j);
              found++;
              }
            }
          if(found && !testing_subconnections) break;
          }
        if(testing_subconnections && !found) {
          c->c.connect(i, c, i, false);
          tmcell.push_back(Id);
          }
        foundtab.push_back(found);
        if(found != 1) failures++;
        }
      if(testing_subconnections) println(hlog, "foundtab = ", foundtab);
      }
    println(hlog, "total failures = ", failures);
    if(failures && !testing_subconnections) throw hr_exception("hrmap_closed3 failures");
    }

  transmatrix hrmap_closed3::relative_matrixc(cell *c2, cell *c1, const hyperpoint& hint) {
    if(c1 == c2) return Id;
    int d = hr::celldistance(c2, c1);

    for(int a=0; a<c1->type; a++) if(hr::celldistance(c2, c1->move(a)) < d)
      return adj(c1, a) * relative_matrix(c2, c1->move(a), hint);
      
    for(int a=0; a<c1->type; a++) println(hlog, "d=", d, " vs ", hr::celldistance(c2, c1->move(a)));
    
    println(hlog, "error in hrmap_quotient3:::relative_matrix");
    return Id;
    }
    
#if CAP_CRYSTAL
  int encode_coord(int bits, const crystal::coord& co) {
    int c = 0;
    for(int i=0; i<(1<<bits); i++) c |= ((co[i]>>1) & ((1<<bits)-1)) << (bits*i);
    return c;
    }

  EX crystal::coord decode_coord(int bits, int a) {
    crystal::coord co;
    for(int i=0; i<(1<<bits); i++) co[i] = (a & ((1<<bits)-1)) * 2, a >>= bits;
    return co;
    }
  
  struct hrmap_from_crystal : hrmap_quotient3 {
    int bits;

    hrmap_from_crystal(int b) : bits(b) {
      int size = 1 << (4*bits);
      initialize(size);
      if(1) {
        auto m = crystal::new_map();
        dynamicval<hrmap*> cm(currentmap, m);
        for(int a=0; a<size; a++) {
          auto co = decode_coord(bits, a);
          heptagon *h1 = get_heptagon_at(co);
          for(int d=0; d<8; d++) {
            int b = encode_coord(bits, crystal::get_coord(h1->cmove(d)));
            allh[a]->c.connect(d, allh[b], h1->c.spin(d), false);
            tmatrices[a].push_back(crystal::get_adj(h1, d));
            }
          }
        delete m;
        }
      make_subconnections();
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
          k = f->matcode[ f->mmul(f->mmul(f->matrices[k], f->matrices[moveid[b]]), f->P) ];
          for(int l=0; l<lgr; l++) if(f->gmul(k, l) % lgr == 0) {
            tmatrices[a][b] = cgi.adjmoves[b] * f->fullv[l];
            allh[a]->c.connect(b, allh[k/lgr], movedir[l], false);
            }
          }
        }      
      make_subconnections();
      create_patterns();
      }
    
    set<cellwalker> plane;

    void make_plane(cellwalker cw) {
      if(plane.count(cw)) return;
      plane.insert(cw);
      auto& ss = get_cellshape(cw.at);
      for(int i=0; i<cw.at->type; i++)
        if(ss.dirdist[i][cw.spin] == 1)
          make_plane(strafe(cw, i));
      }
    
    
    void create_patterns() {
      DEBB(DF_GEOM, ("creating pattern = ", isize(allh)));
      
      if(!PURE) {
         println(hlog, "create_patterns not implemented");
         return;
         }

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
      auto& hsh = get_hsh();
      
      set<coord> boundaries;
      
      for(int a=0; a<12; a++)
      for(int b=0; b<12; b++) if(hsh.dirdist[a][b] == 1) {
        coord res = crystal::c0;
        int sa = a, sb = b;
        do {
          // printf("%d ", sa);
          if(sa < 6) res[sa]++; else res[sa-6]--;
          sa = flip(sa);
          sb = flip(sb);
          swap(sa, sb);
          sb = hsh.next_dir[sa][sb];
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
          
          boundaries = std::move(nb);
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
        make_subconnections();
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
            tmatrices[a][b] = rspintox(p) * xpush(hdist0(p)) * cspin(2, 1, 108._deg) * spintox(p);
            }
          }      
        make_subconnections();
        }
      };
    
    }

  EX bool minimize_quotient_maps = false;

  EX bool strafe_test = false;

  hrmap_quotient3 *gen_quotient_map(bool minimized, fieldpattern::fpattern &fp) {
    #if CAP_FIELD
    if(geometry == gSpace535 && minimized) {
      return new seifert_weber::hrmap_singlecell(108._deg);
      }
    else if(geometry == gSpace535)
      return new seifert_weber::hrmap_seifert_cover;
    else if(hyperbolic) {
      return new hrmap_field3(&fp);
      }
    else if(geometry == gCubeTiling)
      return new seifert_weber::hrmap_singlecell(0);
    #endif
    return nullptr;
    }

  struct hrmap_h3_abstract : hrmap {

    reg3::hrmap_quotient3 *quotient_map;

    transmatrix adj(heptagon *h, int d) override {
      throw hr_exception("any adj");
      }

    transmatrix relative_matrixc(cell *c2, cell *c1, const hyperpoint& hint) override {
      if(PURE) return relative_matrix(c2->master, c1->master, hint);
      return relative_matrix_via_masters(c2, c1, hint);
      }

    transmatrix master_relative(cell *c, bool get_inverse) override {
      if(PURE) return Id;
      int aid = cell_id.at(c);
      return quotient_map->master_relative(quotient_map->acells[aid], get_inverse);
      }

    int shvid(cell *c) override {
      if(PURE) return 0;
      if(!cell_id.count(c)) return quotient_map->shvid(c);
      int aid = cell_id.at(c);
      return quotient_map->shvid(quotient_map->acells[aid]);
      }

    int wall_offset(cell *c) override {
      if(PURE) return 0;
      if(!cell_id.count(c)) return quotient_map->wall_offset(c); /* necessary because ray samples are from quotient_map */
      int aid = cell_id.at(c);
      return quotient_map->wall_offset(quotient_map->acells[aid]);
      }

    transmatrix adj(cell *c, int d) override {
      if(PURE) return adj(c->master, d);
      if(!cell_id.count(c)) return quotient_map->adj(c, d); /* necessary because ray samples are from quotient_map */
      int aid = cell_id.at(c);
      return quotient_map->tmatrices_cell[aid][d];
      }

    subcellshape& get_cellshape(cell *c) override {
      if(PURE) return *cgi.heptshape;
      int aid = cell_id.at(c);
      return quotient_map->get_cellshape(quotient_map->acells[aid]);
      }

    map<cell*, int> cell_id;
    map<pair<heptagon*, int>, cell*> cell_at;

    cell *get_cell_at(heptagon *h, int acell_id) {
      pair<heptagon*, int> p(h, acell_id);
      auto& ca = cell_at[p];
      if(!ca) {
        ca = newCell(quotient_map->acells[acell_id]->type, h);
        cell_id[ca] = acell_id;
        if(!h->c7) h->c7 = ca;
        }
      return ca;
      }

    void find_cell_connection(cell *c, int d) override {
      if(PURE) {
        auto h = c->master->cmove(d);
        c->c.connect(d, h->c7, c->master->c.spin(d), false);
        return;
        }
      int id = cell_id.at(c);
      heptagon *h = c->master;
      for(int dir: quotient_map->move_sequences[id][d])
        h = h->cmove(dir);
      auto ac = quotient_map->acells[id];
      cell *c1 = get_cell_at(h, quotient_map->local_id[ac->move(d)].first);
      c->c.connect(d, c1, ac->c.spin(d), false);
      }

    transmatrix ray_iadj(cell *c, int i) override {
      if(PURE) return iadj(c, i);
      if(!cell_id.count(c)) return quotient_map->ray_iadj(c, i); /* necessary because ray samples are from quotient_map */
      int aid = cell_id.at(c);
      return quotient_map->ray_iadj(quotient_map->acells[aid], i);
      }

    const vector<int>& get_move_seq(cell *c, int i) override {
      int aid = cell_id.at(c);
      return quotient_map->get_move_seq(quotient_map->acells[aid], i);
      }

    cellwalker strafe(cellwalker cw, int j) override {

      int aid = PURE ? cw.at->master->fieldval : cell_id.at(cw.at);
      auto ress = quotient_map->strafe(cellwalker(quotient_map->acells[aid], cw.spin), j);
      cellwalker res = cellwalker(cw.at->cmove(j), ress.spin);

      if(PURE && strafe_test) {
        hyperpoint hfront = tC0(cgi.adjmoves[cw.spin]);
        cw.at->cmove(j);
        transmatrix T = currentmap->adj(cw.at, j);
        for(int i=0; i<S7; i++) if(i != cw.at->c.spin(j))
          if(hdist(hfront, T * tC0(cgi.adjmoves[i])) < cgi.strafedist + .01) {
            auto resx = cellwalker(cw.at->cmove(j), i);
            if(res == resx) return res;
            if(PURE && res != resx) println(hlog, "h3: ", res, " vs ", resx);
            }
        throw hr_exception("incorrect strafe");
        }

      return res;
      }
    };

  struct hrmap_h3 : hrmap_h3_abstract {
  
    heptagon *origin;
    hrmap *binary_map;
    vector<heptagon*> extra_origins;
    
    map<heptagon*, pair<heptagon*, transmatrix>> reg_gmatrix;
    map<heptagon*, vector<pair<heptagon*, transmatrix> > > altmap;

    vector<cell*>& allcells() override { 
      return hrmap::allcells();
      }

    hrmap_h3() {
      println(hlog, "generating hrmap_h3");
      origin = init_heptagon(S7);
      heptagon& h = *origin;
      h.s = hsOrigin;
      if(PURE) h.c7 = newCell(S7, origin);
      worst_error1 = 0, worst_error2 = 0;
      
      dynamicval<hrmap*> cr(currentmap, this);
      
      heptagon *alt = NULL;
      transmatrix T = Id;
      
      binary_map = nullptr;
      quotient_map = gen_quotient_map(minimize_quotient_maps, currfp);
      
      h.zebraval = quotient_map ? quotient_map->allh[0]->zebraval : 0;
      
      #if CAP_BT
      if(hyperbolic) {
        dynamicval<eGeometry> g(geometry, gBinary3); 
        bt::build_tmatrix();
        alt = init_heptagon(S7);
        alt->s = hsOrigin;
        alt->alt = alt;
        binary_map = bt::new_alt_map(alt);
        T = xpush(.01241) * spin(1.4117) * xpush(0.1241) * cspin(0, 2, 1.1249) * xpush(0.07) * Id;
        }
      #endif
      
      reg_gmatrix[origin] = make_pair(alt, T);
      altmap[alt].emplace_back(origin, T);

      if(PURE) {
        celllister cl(origin->c7, 4, 100000, NULL);
        for(cell *c: cl.lst) {
          hyperpoint h = tC0(relative_matrix(c->master, origin, C0));
          cgi.close_distances[bucketer(h)] = cl.getdist(c);
          }
        }

      if(!PURE) get_cell_at(origin, 0);
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

    void verify_neighbors(heptagon *alt, int steps, const hyperpoint& hT) {
      ld err;
      for(auto& p2: altmap[alt]) if((err = intval(tC0(p2.second), hT)) < 1e-3) {
        println(hlog, "FAIL");
        exit(3);
        }
      #if CAP_BT
      if(steps) { 
        dynamicval<eGeometry> g(geometry, gBinary3);
        dynamicval<hrmap*> cm(currentmap, binary_map);
        for(int i=0; i<alt->type; i++)
          verify_neighbors(alt->cmove(i), steps-1, currentmap->iadj(alt, i) * hT);
        }
      #endif
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
      #if CAP_BT
      if(hyperbolic) {
        dynamicval<eGeometry> g(geometry, gBinary3);
        dynamicval<hrmap*> cm(currentmap, binary_map);
        binary_map->virtualRebase(alt, T);
        }
      #endif

      fixmatrix(T);
      auto hT = tC0(T);
      
      if(DEB) println(hlog, "searching at ", alt, ":", hT);

      if(DEB) for(auto& p2: altmap[alt]) println(hlog, "for ", tC0(p2.second), " intval is ", intval(tC0(p2.second), hT));
      
      ld err;
      
      for(auto& p2: altmap[alt]) if((err = intval(tC0(p2.second), hT)) < 1e-3) {
        if(err > worst_error1) println(hlog, format("worst_error1 = %lg", double(worst_error1 = err)));
        // println(hlog, "YES found in ", isize(altmap[alt]));
        if(DEB) println(hlog, "-> found ", p2.first);
        int fb = 0;
        hyperpoint old = tC0(p1.second);;
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
      
      if(extra_verification) verify_neighbors(alt, extra_verification, hT);
      
      if(DEB) println(hlog, "-> not found");
      int d2 = 0, fv = isize(reg_gmatrix);
      #if CAP_FIELD
      if(quotient_map) {
        auto cp = counterpart(parent);
        d2 = cp->c.spin(d);
        fv = cp->c.move(d)->fieldval;
        }
      #endif
      heptagon *created = init_heptagon(S7);
      if(PURE && parent->c7) created->c7 = newCell(S7, created);
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

    ~hrmap_h3() {
      #if CAP_BT
      if(binary_map) {        
        dynamicval<eGeometry> g(geometry, gBinary3);
        delete binary_map;
        }
      #endif
      if(quotient_map) delete quotient_map;
      clearfrom(origin);
      for(auto e: extra_origins) clearfrom(e);
      }
    
    map<heptagon*, int> reducers;

    bool link_alt(heptagon *h, heptagon *alt, hstate firststate, int dir) override {
      altdist(h) = 0;
      if(firststate != hsOrigin) reducers[h] = dir;
      return true;
      }
    
    void extend_altmap(heptagon* h, int levs, bool link_cdata) override {
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

    transmatrix adj(heptagon *h, int d) override {
      #if CAP_FIELD
      if(quotient_map) return quotient_map->adj(h, d);
      else
      #endif
      return relative_matrix(h->cmove(d), h, C0);
      }
     
    transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
      auto p1 = reg_gmatrix[h1];
      auto p2 = reg_gmatrix[h2];
      transmatrix T = Id;
      #if CAP_BT
      if(hyperbolic) { 
        dynamicval<eGeometry> g(geometry, gBinary3);
        dynamicval<hrmap*> cm(currentmap, binary_map);
        T = binary_map->relative_matrix(p2.first, p1.first, hint);
        }
      #endif
      T = inverse(p1.second) * T * p2.second;      
      if(elliptic && T[LDIM][LDIM] < 0) T = centralsym * T;
      return T;
      }
    
    cell* gen_extra_origin(int fv) override {
      auto orig = isize(extra_origins) ? extra_origins.back() : origin;

      auto& p1 = reg_gmatrix[orig];
      heptagon *alt = p1.first;

      transmatrix T = p1.second;

      for(int a=0; a<10; a++) {
        T = T * xpush(euclid ? 1000 : 10);
        #if CAP_BT
        if(hyperbolic) {
          dynamicval<eGeometry> g(geometry, gBinary3);
          dynamicval<hrmap*> cm(currentmap, binary_map);
          binary_map->virtualRebase(alt, T);
          fixmatrix(T);
          }
        #endif
        }

      heptagon *created = init_heptagon(S7);

      created->s = hsOrigin;
      created->fieldval = quotient_map->acells[fv]->master->fieldval;
      reg_gmatrix[created] = make_pair(alt, T);
      altmap[alt].emplace_back(created, T);

      extra_origins.push_back(created);
      return get_cell_at(created, fv);
      }
    };

  EX int get_aid(cell *c) {
    auto m = dynamic_cast<hrmap_h3*> (currentmap);
    if(!m) throw hr_exception("get_aid incorrect");
    if(PURE) return c->master->fieldval;
    return m->cell_id[c];
    }

  EX int get_size_of_aid(int aid) {
    auto m = dynamic_cast<hrmap_h3*> (currentmap);
    if(!m) throw hr_exception("get_size_of_fv incorrect");
    return m->quotient_map->acells[aid]->type;
    }

  struct hrmap_sphere3 : hrmap_closed3 {
  
    vector<transmatrix> locations;
  
    hrmap_sphere3() {    
      heptagon *h = init_heptagon(S7);
      h->s = hsOrigin;
      
      locations.push_back(Id);
      allh.push_back(h);
      
      for(int i=0; i<isize(allh); i++) {
        tmatrices.emplace_back();
        auto& tmi = tmatrices.back();
        transmatrix T1 = locations[i];
        hyperpoint old = tC0(T1);
        for(int d=0; d<S7; d++) {
          transmatrix T = T1 * cgi.adjmoves[d];
          fixmatrix(T);
          auto hT = tC0(T);
          
          bool hopf = stretch::applicable();
    
          if(hopf)
            T = stretch::translate(hT);          
          
          for(int i1=0; i1<isize(allh); i1++)
            if(intval(tC0(locations[i1]), hT) < 1e-3) {
              int fb = 0;
              for(int d2=0; d2<S7; d2++) {
                hyperpoint back = locations[i1] * tC0(cgi.adjmoves[d2]);
                if(intval(back, old) < 1e-3) {
                  allh[i]->c.connect(d, allh[i1], d2, false);
                  fb++;
                  tmi.push_back(inverse(T1) * locations[i1]);
                  }
                }
              if(fb != 1) throw hr_exception("friend not found");
              goto next_d;
              }

          if(1) {
            int d2 = 0;

            if(hopf) {
              for(d2=0; d2<S7; d2++) {
                hyperpoint back = T * tC0(cgi.adjmoves[d2]);
                if(intval(back, old) < 1e-3) 
                  break;
                }
              if(d2 == S7)
                throw hr_exception("Hopf connection failed"); 
              }
  
            heptagon *h = init_heptagon(S7);
            h->zebraval = hrand(10);
            h->fieldval = isize(allh);
            h->fiftyval = 9999;
            allh.push_back(h);
            locations.push_back(T);
            if(isnan(T[0][0])) exit(1);
            
            allh[i]->c.connect(d, h, d2, false);
            tmi.push_back(inverse(T1) * T);
            if(elliptic) fixelliptic(tmi.back());
            }
          next_d: ;
          }
        }
      
      make_subconnections();
      }
    
    transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
      return iso_inverse(locations[h1->fieldval]) * locations[h2->fieldval];
      }

    };

  EX const transmatrix& get_sphere_loc(int v) {
    return ((hrmap_sphere3*)currentmap)->locations[v];
    }

  struct ruleset {
    fieldpattern::fpattern fp;
    vector<int> root;
    string other;
    vector<short> children;
    vector<int> childpos;

    vector<int> otherpos;

    virtual hrmap_quotient3 *qmap() = 0;

    ruleset() : fp(0) {}

    void load_ruleset_new(string fname) {

      shstream ins(decompress_string(read_file_as_string(fname)));
      ins.read(ins.vernum);
      if(1) {
        dynamicval<eVariation> dv(variation);
        dynamicval<flagtype> dc(coxeter_param);
        dynamicval<int> ds(subcube_count);
        mapstream::load_geometry(ins);
        }
      ins.read(fieldpattern::use_rule_fp);
      ins.read(fieldpattern::use_quotient_fp);
      ins.read(reg3::minimize_quotient_maps);

      hread_fpattern(ins, fp);

      hread(ins, root);
      hread(ins, children);
      hread(ins, other);
      hread(ins, childpos);

      println(hlog, "roots = ", isize(root), " states = ", isize(childpos)-1, " hashv = ", fp.hashv);
      }

    /** \brief address = (fieldvalue, state) */
    typedef pair<int, int> address;
    
    /** nles[x] lists the addresses from which we can reach address x 
     *  without ever ending in the starting point */

    map<address, set<address>> nonlooping_earlier_states;

    vector<vector<int>> possible_states;

    virtual int connection(int fv, int d) = 0;

    void find_mappings() {
      int opos = 0;
      for(int c: children) {
        if(c < -1) c += (1<<16);
        if(c >= 0)
          otherpos.push_back(-1);
        else {
          otherpos.push_back(opos);
          while(other[opos] != ',') opos++;
          opos++;
          }
        }

      /* find all back paths */

      auto &nles = nonlooping_earlier_states;
      nles.clear();
      vector<address> bfs;
      int qty = isize(root);
      for(int i=0; i<qty; i++) 
        bfs.emplace_back(i, root[i]);
      int qstate = isize(childpos) - 1;
      DEBB(DF_GEOM, ("qstate = ", qstate));
      for(int i=0; i<isize(bfs); i++) {
        address last = bfs[i];
        int state = last.second;
        int fv = last.first;
        for(int d=0; d<childpos[state+1]-childpos[state]; d++) {
          int nstate = children[childpos[state]+d];
          if(nstate < -1) nstate += (1<<16);
          if(nstate >= 0) {
            address next = {connection(fv, d), nstate};
            if(!nles.count(next)) bfs.push_back(next);
            nles[next].insert(last);
            }
          }
        }
      
      /* remove BFS roots, and addresses that lead only there */

      bfs = {};
      for(int i=0; i<isize(root); i++)
        bfs.emplace_back(i, root[i]);
      for(int i=0; i<isize(bfs); i++) {
        address last = bfs[i];
        int state = last.second;
        int fv = last.first;
        for(int d=0; d<childpos[state+1]-childpos[state]; d++) {
          int nstate = children[childpos[state]+d];
          if(nstate < -1) nstate += (1<<16);
          if(nstate >= 0) {
            address next = {connection(fv, d), nstate};
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

      /* remove non-branching states */

      set<address> good;
      bfs = {};
      auto visit = [&] (address a) { if(good.count(a)) return; good.insert(a); bfs.push_back(a); };
      for(auto& a: nonlooping_earlier_states) if(isize(a.second) > 1) visit(a.first);
      for(int i=0; i<isize(bfs); i++) {
        address last = bfs[i];
        int state = last.second;
        int fv = last.first;
        for(int d=0; d<childpos[state+1]-childpos[state]; d++) {
          int nstate = children[childpos[state]+d];
          if(nstate < -1) nstate += (1<<16);
          if(nstate >= 0) {
            address next = {connection(fv, d), nstate};
            if(!nles.count(next)) continue;
            visit(next);
            }
          }
        }

      set<address> bad;
      for(auto& a: nonlooping_earlier_states) if(!good.count(a.first)) bad.insert(a.first);

      for(auto& b: bad) nonlooping_earlier_states.erase(b);

      int to_cut = 0;

      for(auto& a: nonlooping_earlier_states) {
        set<address> goods;
        for(auto& b: a.second) if(good.count(b)) goods.insert(b); else to_cut++;
        a.second = goods;
        }

      println(hlog, "to_cut = ", to_cut, " from ", isize(bad), " bad cases");

      // just the number of FV's
      int pstable = 0;
      for(auto& p: nonlooping_earlier_states)
        pstable = max(pstable, p.first.first+1);
      
      println(hlog, "pstable size = ", pstable, " (states: ", qstate, ")");
      
      possible_states.resize(pstable);
      for(auto& p: nonlooping_earlier_states)
        possible_states[p.first.first].push_back(p.first.second);
      }


    bool ruleset_link_alt(heptagon *h, heptagon *alt, hstate firststate, int dir) {
      alt->fieldval = h->fieldval;
      if(firststate == hsOrigin) {
        alt->fiftyval = root[alt->fieldval % isize(root)];
        return true;
        }
      vector<int>& choices = possible_states[alt->fieldval];
      vector<int> choices2;
      for(auto c: choices) {
        bool ok = true;
        for(int d=0; d<childpos[c+1]-childpos[c]; d++)
          if(h->cmove(d)->distance < h->distance)
            if(children[childpos[c]+d] == -1)
              ok = false;
        if(ok) choices2.push_back(c);
        }
      alt->fiftyval = hrand_elt(choices2, -1);
      return alt->fiftyval != -1;
      }
    };

  struct hrmap_h3_rule : hrmap_h3_abstract, ruleset {

    heptagon *origin;
    reg3::hrmap_quotient3 *emerald_map;

    hrmap_quotient3 *qmap() override { return quotient_map; }

    hrmap_h3_rule() {

      println(hlog, "generating hrmap_h3_rule");

      load_ruleset_new(get_rule_filename(false));
      quotient_map = gen_quotient_map(minimize_quotient_maps, fp);
      find_mappings();
      
      origin = init_heptagon(S7);
      heptagon& h = *origin;
      h.s = hsOrigin;
      h.fiftyval = root[0];
      if(PURE) h.c7 = newCell(S7, origin);
      
      emerald_map = gen_quotient_map(false, currfp);

      h.emeraldval = 0;
      
      if(!PURE) get_cell_at(origin, 0);
      }
    
    int connection(int fv, int d) override {
      return qmap()->allh[fv]->move(d)->fieldval;
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
      generate_cellrotations();
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
      if(id < 0) id += (1<<16);

      auto cp = counterpart(parent);
      int d2 = cp->c.spin(d);
      int fv = cp->c.move(d)->fieldval;
      
      // indenter ind(2);
      
      heptagon *res = nullptr;
      
      int id1 = children[S7*id+d];
      int pos = otherpos[S7*id+d];
      if(id1 < -1) id1 += (1<<16);
      
      if(id1 == -1 && false) {
        int kk = pos;
        string s;
        while(other[kk] != ',') s += other[kk++];
        println(hlog, "id=", id, " d=", d, " d2=", d2, " id1=", id1, " pos=", pos, " s = ", s);
        }
        
      if(id1 != -1) {
        res = init_heptagon(S7);
        if(PURE && parent->c7)
          res->c7 = newCell(S7, res);
        res->fieldval = fv;
        res->distance = parent->distance + 1;
        res->fiftyval = id1;
        find_emeraldval(res, parent, d);
        // res->c.connect(d2, parent, d, false);
        }
      
      else if(other[pos] == ('A' + d) && other[pos+1] == ',') {
        res = init_heptagon(S7);
        res->alt = parent->alt;
        res->fieldval = fv;
        res->distance = parent->distance - 1;
        vector<int> possible;
        int pfv = parent->fieldval;
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
      
      if(!res) throw hr_exception("res missing");
      
      if(res->move(d2)) println(hlog, "res conflict");

      res->c.connect(d2, parent, d, false);
      return res;
      }

    ~hrmap_h3_rule() {
      if(quotient_map) delete quotient_map;
      clearfrom(origin);
      }
    
    transmatrix adj(heptagon *h, int d) override {
      return quotient_map->adj(h, d);
      }
     
    transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
      return relative_matrix_recursive(h2, h1);
      }
    
    virtual bool link_alt(heptagon *h, heptagon *alt, hstate firststate, int dir) override {
      return ruleset_link_alt(h, alt, firststate, dir);
      }
    };

  vector<heptspin> starts = {nullptr};

  struct hrmap_h3_subrule : hrmap, ruleset {

    heptagon *origin;
    hrmap_quotient3 *quotient_map;
    hrmap_quotient3 *qmap() override { return quotient_map; }

    int connection(int fv, int d) override {
      return quotient_map->local_id[quotient_map->acells[fv]->move(d)].first;
      }

    void explain_conflict(vector<heptspin> hs) {
      vector<heptagon*> v;
      for(auto h: hs) v.push_back(h.at);
      int N = isize(v);
      vector<vector<int>> paths(N);
      while(true) {

        bool eq = true;
        for(auto c: v) if(c != v[0]) eq = false;
        if(eq) break;

        int mindist = 999999;
        int maxdist = -999999;
        for(auto c: v) {
          if(c->distance < mindist) mindist = c->distance;
          if(c->distance > maxdist) maxdist = c->distance;
          }

        int goal = min(mindist, maxdist-1);
        println(hlog, "mindist = ", mindist, " maxdist = ", maxdist, " goal = ", goal);

        int id = 0;
        for(auto& c: v) {
          while(c->distance > goal) {
            println(hlog, c, " distance is ", c->distance);
            int d = find_parent(c);
            paths[id].push_back(c->c.spin(d));
            c = c->move(d);
            }
          id++;
          }
        }
      for(auto& p: paths) reverse(p.begin(), p.end());
      hs.push_back(heptspin(v[0], find_parent(v[0])));
      for(auto h: hs) {
        println(hlog, h, " : dist = ", h.at->distance, " id = ", h.at->fiftyval, " qid = ", h.at->fieldval);
        }
      println(hlog, "paths = ", paths);
      }

    hrmap_h3_subrule() {

      println(hlog, "loading a subrule ruleset");

      load_ruleset_new(get_rule_filename(true));
      quotient_map = gen_quotient_map(minimize_quotient_maps, fp);
      int t = quotient_map->acells[0]->type;
      find_mappings();

      origin = init_heptagon(t);
      heptagon& h = *origin;
      h.s = hsOrigin;
      h.fieldval = 0;
      h.fiftyval = root[0];
      h.c7 = newCell(t, origin);      
      }

    heptagon *getOrigin() override {
      return origin;
      }

    heptagon *create_step(heptagon *parent, int d) override {
      heptspin parentd(parent, d);
      if(starts[isize(starts)/2] == parentd) {
        int i = 0;
        vector<heptspin> cut;
        for(auto s: starts) if(i++ >= isize(starts)/2) cut.push_back(s);
        println(hlog, "cycle detected is ", cut);
        explain_conflict(cut);
        throw hr_exception("create_step cycle detected");
        }
      starts.push_back(parentd);
      finalizer f([] { starts.pop_back(); });

      int id = parent->fiftyval;
      if(id < 0) id += (1<<16);

      int qid = parent->fieldval;

      int d2 = quotient_map->acells[qid]->c.spin(d);
      int qid2 = quotient_map->local_id[quotient_map->acells[qid]->move(d)].first;

      heptagon *res = nullptr;

      int id1 = children[childpos[id]+d];
      int pos = otherpos[childpos[id]+d];
      if(id1 < -1) id1 += (1<<16);
      
      if(id1 != -1) {
        int t = childpos[id1+1] - childpos[id1];
        res = init_heptagon(t);
        res->c7 = newCell(t, res);
        res->fieldval = qid2;
        res->distance = parent->distance + 1;
        res->fiftyval = id1;
        }

      else if(other[pos] == ('A' + d) && other[pos+1] == ',') {
        vector<int> possible;
        for(auto s: nonlooping_earlier_states[address{qid, id}]) possible.push_back(s.second);
        if(possible.empty()) throw hr_exception("impossible");
        id1 = hrand_elt(possible, 0);

        int t = childpos[id1+1] - childpos[id1];
        res = init_heptagon(t);
        res->alt = parent->alt;
        res->fieldval = qid2;
        res->distance = parent->distance - 1;
        res->fiftyval = id1;
        }

      else {
        heptagon *at = parent;
        while(other[pos] != ',') {
          int dir = other[pos++] - 'a';
          at = at->cmove(dir);
          }
        res = at;
        }

      if(!res) throw hr_exception("res missing");

      if(res->move(d2)) {
        heptspin a(res, d2);
        heptspin b = a + wstep;
        heptspin c(parent, d);
        println(hlog, "res conflict: ", a, " already connected to ", b, " and should be connected to ", c);
        explain_conflict({a, b, c});
        }

      res->c.connect(d2, parent, d, false);
      return res;
      }

    int find_parent(heptagon *h) {
      int id = h->fiftyval;
      int pos = childpos[id];
      for(int i=0; i<childpos[id+1]-childpos[id]; i++)
        if(other[otherpos[pos+i]] == 'A'+i && other[otherpos[pos+i]+1] == ',') {
          println(hlog, "find_parent returns ", i, " for ", h);
          return i;
          }
      println(hlog, "find_parent fails for ", h);
      println(hlog, "aid size = ", isize(quotient_map->acells));
      println(hlog, "roots size = ", isize(root));
      return 0;
      }

    ~hrmap_h3_subrule() {
      if(quotient_map) delete quotient_map;
      clearfrom(origin);
      }

    transmatrix adj(heptagon *h, int d) override {
      return quotient_map->adj(quotient_map->acells[h->fieldval], d);
      }

    transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
      return relative_matrix_recursive(h2, h1);
      }

    int shvid(cell *c) override {
      return quotient_map->shvid(quotient_map->acells[c->master->fieldval]);
      }

    int wall_offset(cell *c) override {
      return quotient_map->wall_offset(quotient_map->acells[c->master->fieldval]);
      }

    subcellshape& get_cellshape(cell *c) override {
      return quotient_map->get_cellshape(quotient_map->acells[c->master->fieldval]);
      }

    transmatrix ray_iadj(cell *c, int i) override {
      return quotient_map->ray_iadj(quotient_map->acells[c->master->fieldval], i);
      }

    cellwalker strafe(cellwalker cw, int j) override {
      int aid = cw.at->master->fieldval;
      auto ress = quotient_map->strafe(cellwalker(quotient_map->acells[aid], cw.spin), j);
      return cellwalker(cw.at->cmove(j), ress.spin);
      }

    bool link_alt(heptagon *h, heptagon *alt, hstate firststate, int dir) override {
      return ruleset_link_alt(h, alt, firststate, dir);
      }

    void dump(string fname) {
      fhstream f(fname, "wt");
      int T = isize(quotient_map->acells);
      println(f, hyperbolic ? "h3." : "e3.");
      println(f, "celltypes ", T);
      for(int t=0; t<T; t++) {
        auto &sh = quotient_map->get_cellshape(quotient_map->acells[t]);
        println(f, "celltype ", t, " ", isize(sh.faces));
        int id = 0;
        for(auto& fa: sh.faces_local) {
          println(f, "face ", t, " ", id++, " ", isize(fa));
          for(auto& h: fa) {
            auto h1 = kleinize(h);
            println(f, format("%.20f %.20f %.20f", h1[0], h1[1], h1[2]));
            }
          }
        println(f);
        }
      for(int t=0; t<T; t++) {
        auto c = quotient_map->acells[t];
        for(int i=0; i<c->type; i++) {
          auto c1 = c->move(i);
          auto t1 = c1->master->fieldval % T;
          auto s1 = c->c.spin(i);
          println(f, "connection ", t, " ", i, " ", t1, " ", s1);
          transmatrix T = quotient_map->adj(c, i);
          for(int i=0; i<4; i++) {
            for(int j=0; j<4; j++) {
              print(f, format("%.20f", T[i][j]), j == 3 ? "\n" : " ");
              }
            }
          println(f);
          }
        }
      println(f, "states ", isize(childpos) - 1);
      for(int t=0; t<T; t++) print(f, root[t % isize(root)], t == T-1 ? "\n" : " ");
      for(int i=0; i<isize(childpos)-1; i++) {
        int S = childpos[i+1] - childpos[i];
        print(f, "state ", i, " ", S);
        for(int u=childpos[i]; u<childpos[i+1]; u++) {
          if(children[u] >= 0) {
            print(f, " ", children[u]);
            continue;
            }
          int pos = otherpos[u];
          if(other[pos] == ('A' + u - childpos[i]) && other[pos+1] == ',') {
            print(f, " P");
            continue;
            }
          print(f, " (");
          while(other[pos] != ',') {
            print(f, " ", other[pos++] - 'a');
            }
          print(f, " )");
          }
        println(f);
        }
      }
    };

  struct hrmap_h3_rule_alt : hrmap {
    
    heptagon *origin;
    
    hrmap_h3_rule_alt(heptagon *o) {
      origin = o;
      }

    };

EX hrmap *new_alt_map(heptagon *o) {
  println(hlog, "new_alt_map called");
  return new hrmap_h3_rule_alt(o);
  }

/** 1 -- consider pure rules, 2 -- consider variation rules, 3 -- consider both */
EX int consider_rules = 3;

EX string replace_rule_file;

EX string get_rule_filename(bool with_variations) {
  if(replace_rule_file != "") return replace_rule_file;
  string s;
  s += "honeycombs/";
  s += ginf[geometry].tiling_name[1];
  s += ginf[geometry].tiling_name[3];
  s += ginf[geometry].tiling_name[5];
  if(hyperbolic) s += "h";
  else s += "c";
  if(with_variations) {
    if(variation == eVariation::coxeter) s += "-c" + its(coxeter_param);
    if(variation == eVariation::subcubes) s += "-s" + its(subcube_count);
    if(variation == eVariation::dual_subcubes) s += "-d" + its(subcube_count);
    if(variation == eVariation::bch) s += "-bs" + its(subcube_count);
    }
  s += ".dat";
  return find_file(s);
  }

EX bool variation_rule_available() {
  return (consider_rules & 2) && file_exists(get_rule_filename(true));
  }

EX bool pure_rule_available() {
  return (consider_rules & 1) && file_exists(get_rule_filename(false));
  }

ruleset& get_ruleset() {
  auto h1 = dynamic_cast<hrmap_h3_subrule*> (currentmap);
  if(h1) return *h1;
  auto h2 = dynamic_cast<hrmap_h3_rule*> (currentmap);
  if(h2) return *h2;
  throw hr_exception("get_ruleset called but not in rule");
  }

EX void dump_rules(string fname) {
  auto h = dynamic_cast<hrmap_h3_subrule*> (currentmap);
  if(h) h->dump(fname);
  }

EX int rule_get_root(int i) {
  return get_ruleset().root[i];
  }

EX const vector<short>& rule_get_children() {
  return get_ruleset().children;
  }

EX const vector<int>& rule_get_childpos() {
  return get_ruleset().childpos;
  }

EX hrmap* new_map() {
  if(geometry == gSeifertCover) return new seifert_weber::hrmap_seifert_cover;
  if(geometry == gSeifertWeber) return new seifert_weber::hrmap_singlecell(108._deg);
  if(geometry == gHomologySphere) return new seifert_weber::hrmap_singlecell(36._deg);
  if(quotient && !sphere) return new hrmap_field3(&currfp);
  if(variation_rule_available()) return new hrmap_h3_subrule;
  if(pure_rule_available()) return new hrmap_h3_rule;
  if(sphere) return new hrmap_sphere3;
  return new hrmap_h3;
  }

hrmap_h3* hypmap() {
  return ((hrmap_h3*) currentmap);
  }

EX bool in_hrmap_h3() {
  return dynamic_cast<hrmap_h3*> (currentmap);
  }

EX bool in_hrmap_rule_or_subrule() {
  return dynamic_cast<hrmap_h3_rule*> (currentmap) || dynamic_cast<hrmap_h3_subrule*> (currentmap);
  }

EX bool exact_rules() {
  if(PURE) return in_hrmap_rule_or_subrule();
  return dynamic_cast<hrmap_h3_subrule*> (currentmap);
  }

EX int quotient_count() {
  return isize(hypmap()->quotient_map->allh);
  }

EX int quotient_count_sub() {
  return isize(hypmap()->quotient_map->acells);
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
  
  if(geometry == gSpace534 && PURE) return celldistance_534(c1, c2);

  auto r = hypmap();

  hyperpoint h = tC0(r->relative_matrix(c1->master, c2->master, C0));
  int b = bucketer(h);
  if(cgi.close_distances.count(b)) return cgi.close_distances[b];
  
  if(in_hrmap_rule_or_subrule())
    return clueless_celldistance(c1, c2);

  dynamicval<eGeometry> g(geometry, gBinary3);  
  #if CAP_BT
  return 20 + bt::celldistance3(r->reg_gmatrix[c1->master].first, r->reg_gmatrix[c2->master].first);
  #else
  return 20;
  #endif
  }

EX bool pseudohept(cell *c) {
  if(sphere) {
    auto m = currentmap;
    hyperpoint h = tC0(m->relative_matrix(c->master, m->getOrigin(), C0));
    if(S7 == 12) {
      hyperpoint h1 = cspin(0, 1, atan2(16, 69) + 45._deg) * h;
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
  auto m = hypmap();
  if(cgflags & qSINGLE) return true;
  if(fake::in()) return FPIU(reg3::pseudohept(c));
  // chessboard pattern in 534
  if(geometry == gField534) 
    return hr::celldistance(c, currentmap->gamestart()) & 1;
  if(geometry == gCrystal344 || geometry == gCrystal534 || geometry == gSeifertCover)
    return false;
  if(quotient) return false; /* added */
  auto mr = dynamic_cast<hrmap_h3_rule*> (currentmap);
  if(mr) {
    if(geometry == gSpace535) 
      return c->master->fieldval % 31 == 0;
    return c->master->fieldval == 0;
    }
  auto ms = dynamic_cast<hrmap_h3_subrule*> (currentmap);
  if(ms) return c->master->fieldval == 0;
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

map<pair<cell*, cell*>, int> memo;

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
  if(cgi.xp_order) return;

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

eVariation target_variation;
flagtype target_coxeter;
int target_subcube_count;

EX void edit_variation() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("variations"));

  dialog::addBoolItem(XLAT("pure"), target_variation == eVariation::pure, 'p');
  dialog::add_action([] { target_variation = eVariation::pure; });

  dialog::addBoolItem(XLAT("symmetric subdivision"), target_variation == eVariation::coxeter, 't');
  dialog::add_action([] { target_variation = eVariation::coxeter; });

  if(S7 == 6) {
    dialog::addBoolItem(XLAT("sub-cubes"), target_variation == eVariation::subcubes, 'c');
    dialog::add_action([] { target_variation = eVariation::subcubes; });

    if(!(cgflags & qIDEAL)) {
      dialog::addBoolItem(XLAT("dual sub-cubes"), target_variation == eVariation::dual_subcubes, 'd');
      dialog::add_action([] { target_variation = eVariation::dual_subcubes; });

      dialog::addBoolItem(XLAT("bitruncated sub-cubes"), target_variation == eVariation::bch, 'b');
      dialog::add_action([] { target_variation = eVariation::bch; });
      }
    }
  
  else 
    dialog::addInfo(XLAT("note: more choices in cubic honeycombs"));
  
  if(is_subcube_based(target_variation)) {
    dialog::addBreak(100);
    dialog::addSelItem(XLAT("subdivision"), its(target_subcube_count), 'z');
    dialog::add_action([] {
      dialog::editNumber(target_subcube_count, 1, 8, 1, 2, XLAT("subdivision"), "");
      dialog::bound_low(1);
      });
    }
  
  if(target_variation == eVariation::coxeter) {
    dialog::addBreak(100);
    dialog::addBoolItem(XLAT("split by original faces"), target_coxeter & cox_othercell, 'f');
    dialog::add_action([] { target_coxeter ^= cox_othercell; });
    dialog::addBoolItem(XLAT("split by vertex axes"), target_coxeter & cox_vertices, 'v');
    dialog::add_action([] { target_coxeter ^= cox_vertices; });
    dialog::addBoolItem(XLAT("split by midedges"), target_coxeter & cox_midedges, 'm');
    dialog::add_action([] { target_coxeter ^= cox_midedges; });
    }
    
  dialog::addBreak(100);
  dialog::addItem(XLAT("activate"), 'x');
  dialog::add_action([] {
    stop_game();
    set_variation(target_variation);
    subcube_count = target_subcube_count;
    coxeter_param = target_coxeter;
    start_game();
    });
  dialog::addBack();
  dialog::display();
  }

EX void configure_variation() {
  target_variation = variation;
  target_subcube_count = subcube_count;
  target_coxeter = coxeter_param;
  pushScreen(edit_variation);
  }

EX }
#endif

#if MAXMDIM == 3
EX namespace reg3 {
EX bool in() { return false; }
EX bool in_rule() { return false; }
EX bool cubes_reg3;
EX bool exact_rules() { return false; }
EX }
#endif

}

