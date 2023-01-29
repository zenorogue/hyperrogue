// Hyperbolic Rogue - Floor Shapes

// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file floorshapes.cpp
 *  \brief Adjusting the floor shapes to various geometries.
 */

#include "hyper.h"
namespace hr {
#if CAP_SHAPES

#if HDR
struct qfloorinfo {
  transmatrix spin;
  const struct hpcshape *shape;
  floorshape *fshape;
  struct textureinfo *tinf;
  int usershape;
  };

extern qfloorinfo qfi;
#endif

EX vector<basic_textureinfo> floor_texture_vertices;
EX vector<glvertex> floor_texture_map;
EX struct renderbuffer *floor_textures;

/* 0: generate no floorshapes; 1: generate only plain floorshapes; 2: generate all */
EX int floorshapes_level = 2;

EX ld global_boundary_ratio = 1;

void geometry_information::init_floorshapes() {
  if(floorshapes_level == 0) return;
  all_escher_floorshapes.clear();
  all_plain_floorshapes = { 
    &shFloor, &shMFloor, &shMFloor2, &shMFloor3, &shMFloor4, 
    &shFullFloor, &shBigTriangle, &shTriheptaFloor, &shBigHepta
    };
  
  for(auto s: all_plain_floorshapes) s->is_plain = true;
  
  auto init_escher = [this] (escher_floorshape& sh, int s0, int s1, int noft, int s2) {
    if(floorshapes_level == 1) return;
    sh.shapeid0 = s0;
    sh.shapeid1 = s1;
    sh.noftype = noft;
    sh.shapeid2 = s2;
    sh.scale = 1;
    sh.is_plain = false;
    all_escher_floorshapes.push_back(&sh);
    };
  
  init_escher(shStarFloor, 1, 2, 0, 0);
  init_escher(shCloudFloor, 3, 4, 0, 0);
  init_escher(shCrossFloor, 5, 6, 2, 54);
  init_escher(shChargedFloor, 7, 385, 1, 10);
  init_escher(shSStarFloor, 11, 12, 0, 0);
  init_escher(shOverFloor, 13, 15, 1, 14);
  init_escher(shTriFloor, 17, 18, 0, 385);
  init_escher(shFeatherFloor, 19, 21, 1, 20);
  init_escher(shBarrowFloor, 23, 24, 1, 25);
  init_escher(shNewFloor, 26, 27, 2, 54);
  init_escher(shTrollFloor, 28, 29, 0, 0);
  init_escher(shButterflyFloor, 325, 326, 1, 178);
  init_escher(shLavaFloor, 359, 360, 1, 178);
  init_escher(shLavaSeabed, 386, 387, 1, 178);
  init_escher(shSeabed, 334, 335, 0, 0);
  init_escher(shCloudSeabed, 336, 337, 0, 0);
  init_escher(shCaveSeabed, 338, 339, 2, 54);
  init_escher(shPalaceFloor, 45, 46, 0, 385);
  init_escher(shDemonFloor, 51, 50, 1, 178);
  init_escher(shCaveFloor, 52, 53, 2, 54);
  init_escher(shDesertFloor, 55, 56, 0, 4);
  init_escher(shPowerFloor, 57, 58, 0, 12); /* dragon */
  init_escher(shRoseFloor, 174, 175, 1, 173);
  init_escher(shSwitchFloor, 377, 378, 1, 379);
  init_escher(shTurtleFloor, 176, 177, 1, 178);
  for(int i: {0,1,2})
    init_escher(shRedRockFloor[i], 55, 56, 0, 0);
  init_escher(shDragonFloor, 181, 182, 2, 183); /* dragon */
  
  int ids = 0;
  for(auto sh: all_plain_floorshapes) sh->id = ids++;
  for(auto sh: all_escher_floorshapes) sh->id = ids++;
  }

/** matrixitem::second[2][2] == APEIROGONAL_INVALID is used to denote a matrix that uses fake apeirogon vertices and thus should not be used */
const ld APEIROGONAL_INVALID = -2;

typedef pair<transmatrix, vector<transmatrix>> matrixitem;

struct mesher {
  eGeometry g;
  int sym;
  ld bspi;
  hyperpoint lcorner, rcorner, mfar[2], vfar[4];
  };

mesher msh(eGeometry g, int sym, ld main, ld v0, ld v1, ld bspi, ld scale) {
  main *= scale; v0 *= scale; v1 *= scale;
  mesher m;
  m.sym = sym;
  m.bspi = bspi;
  dynamicval<eGeometry> dg(geometry, g);

  hyperpoint rot = xpush(v0) * xspinpush0(M_PI - M_PI/sym, main);
  hyperpoint bnlfar = xpush(v0) * spin180() * rspintox(rot) * rspintox(rot) * rspintox(rot) * xpush0(hdist0(rot));
  hyperpoint bnrfar = xpush(v0) * spin180() * spintox(rot) * spintox(rot) * spintox(rot) * xpush0(hdist0(rot));

  m.lcorner = xspinpush0 (bspi - M_PI/sym, main);
  m.rcorner = xspinpush0 (bspi + M_PI/sym, main);
  m.mfar[0] = xspinpush0 (bspi, v0);
  m.mfar[1] = xspinpush0 (bspi, v1);
  m.vfar[0] = spin(bspi) * bnlfar;
  m.vfar[2] = spin(bspi) * bnrfar;
  m.vfar[1] = spin(-TAU/sym) * m.vfar[2];
  m.vfar[3] = spin(+TAU/sym) * m.vfar[0];
  
  return m;
  }

struct matrixlist {
  mesher o, n;
  vector<matrixitem> v;
  };

matrixitem genitem(const transmatrix& m1, const transmatrix& m2, int nsym) {
  matrixitem mi;
  mi.first = m1;
  mi.second.resize(nsym);
  for(int i=0; i<nsym; i++)
    mi.second[i] = spin(TAU*i/nsym) * m2;
  return mi;
  }

bool do_kleinize() { return S3 >= OINF || (cgflags & qIDEAL); }

EX hyperpoint may_kleinize(hyperpoint h) { 
  if(do_kleinize()) return kleinize(h);
  else return h;
  }

void addmatrix(matrixlist& matrices, hyperpoint o0, hyperpoint o1, hyperpoint o2, hyperpoint n0, hyperpoint n1, hyperpoint n2, int d, int osym, int nsym) {
  if(do_kleinize()) o0 = kleinize(o0), o1 = kleinize(o1), o2 = kleinize(o2), n0 = kleinize(n0), n1 = kleinize(n1), n2 = kleinize(n2);
  matrices.v.push_back(genitem(inverse(spin(TAU*d/osym)*build_matrix(o0, o1, o2,C02)), spin(TAU*d/nsym)*build_matrix(n0, n1, n2,C02), nsym));
  }

matrixlist hex_matrices, hept_matrices;

void generate_matrices(matrixlist& matrices, const mesher& o, const mesher& n) {
  matrices.v.clear();
  matrices.o = o;
  matrices.n = n;
  
  for(int d=0; d<o.sym; d++) {
    hyperpoint center = hpxy(0,0);
    int d1 = d&1;
    addmatrix(matrices, center, o.lcorner, o.rcorner, center, n.lcorner, n.rcorner, d, o.sym, n.sym);
    addmatrix(matrices, o.mfar[d1], o.lcorner, o.rcorner, n.mfar[d1], n.lcorner, n.rcorner, d, o.sym, n.sym);
    addmatrix(matrices, o.mfar[d1], o.lcorner, o.vfar[d1], n.mfar[d1], n.lcorner, n.vfar[d1], d, o.sym, n.sym);
    addmatrix(matrices, o.mfar[d1], o.rcorner, o.vfar[d1+2], n.mfar[d1], n.rcorner, n.vfar[d1+2], d, o.sym, n.sym);
    }
  }

int nsym0;

void generate_matrices_scale(ld scale, int noft) {
  mesher ohex = msh(gNormal, 6, 0.329036, 0.566256, 0.620672, 0, 1);
  mesher ohept = msh(gNormal, 7, hexf7, hcrossf7, hcrossf7, M_PI/7, 1);
  if(!BITRUNCATED) {
    mesher nall = msh(geometry, S7, cgi.rhexf, cgi.tessf, cgi.tessf, -M_PI, scale);
    bool use = geosupport_football() < 2;
    if(use && noft == 1) {
      mesher opure = msh(gNormal, 7, 0.620672, 1.090550, 1.090550, M_PI/7, 1);
      generate_matrices(hept_matrices, opure, nall);
      }
    else if(use && noft == 2) {
      mesher oeuc = msh(gNormal, 6, sqrt(3)/6, .5, .5, 0, 1);
      generate_matrices(hept_matrices, oeuc, nall);
      }
    else if(use && noft == 3) {
      generate_matrices(hept_matrices, ohex, nall);
      }
    else {
      generate_matrices(hex_matrices, ohex, nall);  
      generate_matrices(hept_matrices, ohept, nall);
      }
    }
  else {
    generate_matrices(hex_matrices, ohex, msh(geometry, S6, cgi.hexvdist, cgi.hexhexdist, cgi.hcrossf, (S3-3)*M_PI/S3, scale));
    generate_matrices(hept_matrices, ohept, msh(geometry, S7, cgi.rhexf, cgi.hcrossf, cgi.hcrossf, M_PI/S7, scale));
    }
  }

void geometry_information::bshape2(hpcshape& sh, PPR prio, int shapeid, matrixlist& m) {
  auto& matrices = m.v;
  int osym = m.o.sym;
  int nsym = m.n.sym;

  int whereis = 0;
  while(polydata[whereis] != NEWSHAPE || polydata[whereis+1] != shapeid) whereis++;
  int rots = polydata[whereis+2]; int sym = polydata[whereis+3];
  whereis += 4;
  int qty = 0;
  while(polydata[whereis + 2*qty] != NEWSHAPE) qty++;
  
  vector<hyperpoint> lst;
  for(int i=0; i<qty; i++) {
    dynamicval<eGeometry> dg(geometry, gNormal);
    lst.push_back(hpxy(polydata[whereis+2*i], polydata[whereis+2*i+1]));
    }
  if(sym == 2)
  for(int i=qty-1; i>=0; i--) {
    dynamicval<eGeometry> dg(geometry, gNormal);
    lst.push_back(hpxy(polydata[whereis+2*i], -polydata[whereis+2*i+1]));
    }
  
  hyperpoint lstmid = hpxyz(0,0,0);
  for(auto pp: lst) lstmid += pp;
  transmatrix T = spin(-m.o.bspi);
  while((spin(TAU / rots) * T* lstmid)[0] < (T*lstmid)[0])
    T = spin(TAU / rots) * T;
  while((spin(-TAU / rots) * T* lstmid)[0] < (T*lstmid)[0])
    T = spin(-TAU / rots) * T;
  T = spin(m.o.bspi) * T;
  for(auto &pp: lst) pp = T * pp;
  
  if(osym % rots && rots % osym) printf("warning: rotation oddity (shapeid %d, osym=%d rots=%d)\n", shapeid, osym, rots);

  if(rots > osym && rots % osym == 0) {
    int rep = rots / osym;
    int s = lst.size();
    for(int i=0; i<s*(rep-1); i++)
      lst.push_back(spin(TAU/rots) * lst[i]);
    rots /= rep;
    }
  
  bshape(sh, prio);

  /* in case of apeirogonal shapes, we may need to cyclically rotate */
  bool apeirogonal = false;
  vector<hyperpoint> tail, head;

  for(int r=0; r<nsym; r+=osym/rots) {
    for(hyperpoint h: lst) {
      hyperpoint nh = may_kleinize(h);
      int mapped = 0;
      int invalid = 0;
      for(auto& m: matrices) {        
        hyperpoint z = m.first * h;
        if(z[0] > -1e-5 && z[1] > -1e-5 && z[2] > -1e-5) {
          if(m.second[r][2][2] == APEIROGONAL_INVALID) invalid++;
          nh = m.second[r] * z, mapped++;
          }
        }
      if(mapped == 0) printf("warning: not mapped (shapeid %d)\n", shapeid);
      if(invalid) {
        apeirogonal = true;        
        for(auto h: head) tail.push_back(h);
        head.clear();
        }
      if(!invalid) head.push_back(nh);
      }
    }

  for(auto& h: head) hpcpush(h);
  for(auto& h: tail) hpcpush(h);
  if(!apeirogonal) hpcpush(starting_point);
  }

template<class T> void sizeto(T& t, int n) {
  if(isize(t) <= n) t.resize(n+1);
  }

void geometry_information::bshape_regular(floorshape &fsh, int id, int sides, ld shift, ld size, cell *c) {
  
  sizeto(fsh.b, id);
  sizeto(fsh.shadow, id);

  #if CAP_BT
  if(bt::in()) {

    const int STEP = vid.texture_step;

    for(int t=0; t<2; t++) {
    
      if(t == 0) 
        bshape(fsh.b[id], fsh.prio);
      if(t == 1)
        bshape(fsh.shadow[id], fsh.prio);

      int STEP1 = STEP;
      if((embedded_plane || geom3::flipped) && t == 0) STEP1 = 1;
      
      for(int i=0; i<sides; i++) {
        hyperpoint h0 = bt::get_corner_horo_coordinates(c, i) * size;
        hyperpoint h1 = bt::get_corner_horo_coordinates(c, i+1) * size;
        if(t) h0 *= SHADMUL, h1 *= SHADMUL;
        hyperpoint hd = (h1 - h0) / STEP1;
        for(int j=0; j<STEP1; j++) {
          hpcpush(bt::get_horopoint(h0 + hd * j));
          if(geometry == gBinary4 && among(i, 2, 4)) break;
          if(geometry == gBinaryTiling && among(i, 0, 4)) break;
          if(geometry == gTernary && among(i, 3, 5)) break;
          }
        }
      
      hpcpush(hpc[last->s]);
      }
    
    for(int k=0; k<SIDEPARS; k++) {
      if(isize(fsh.gpside[k]) < c->type)
        fsh.gpside[k].resize(c->type);
      for(int i=0; i<c->type; i++) {
        sizeto(fsh.gpside[k][i], id);
        bshape(fsh.gpside[k][i][id], PPR::LAKEWALL); 
        hyperpoint h0 = bt::get_corner_horo_coordinates(c, i) * size;
        hyperpoint h1 = bt::get_corner_horo_coordinates(c, i+1) * size;
        hyperpoint hd = (h1 - h0) / STEP;
        for(int j=0; j<=STEP; j++)
          hpcpush(iddspin_side(c, i) * bt::get_horopoint(h0 + hd * j));
        chasmifyPoly(dlow_table[k], dhi_table[k], k);
        }
      }

    return;
    }
  #endif
  
  bshape(fsh.b[id], fsh.prio);
  for(int t=0; t<=sides; t++)
    hpcpush(xspinpush0(t * TAU / sides + shift * S_step, size));

  bshape(fsh.shadow[id], fsh.prio);
  for(int t=0; t<=sides; t++)
    hpcpush(xspinpush0(t * TAU / sides + shift * S_step, size * SHADMUL));

  for(int k=0; k<SIDEPARS; k++) {
    fsh.side[k].resize(2);
    bshape(fsh.side[k][id], PPR::LAKEWALL);
    hpcpush(xspinpush0(+M_PI/sides, size));
    hpcpush(xspinpush0(-M_PI/sides, size));
    chasmifyPoly(dlow_table[k], dhi_table[k], k);

    if(cgi.emb->is_euc_in_noniso()) {
      fsh.gpside[k].resize(c->type);
      for(int i=0; i<c->type; i++) {
        sizeto(fsh.gpside[k][i], id);
        bshape(fsh.gpside[k][i][id], PPR::LAKEWALL);
        hpcpush(xspinpush0(M_PI - i * TAU / sides + shift * S_step, size));
        hpcpush(xspinpush0(M_PI - (i + 1) * TAU / sides + shift * S_step, size));
        chasmifyPoly(dlow_table[k], dhi_table[k], k);
        }
      }
    }
  }

#if CAP_IRR
namespace irr { void generate_floorshapes(); }
#endif

void geometry_information::finish_apeirogon(hyperpoint center) {
  last->flags |= POLY_APEIROGONAL;
  last->she = isize(hpc);
  if(arb::apeirogon_simplified_display) {
    hyperpoint p = towards_inf(last_point, center, ideal_limit);
    hyperpoint q = towards_inf(starting_point, center, ideal_limit);
    hpc.push_back(p);
    hpc_connect_ideal(p, q);
    hpc.push_back(q);
    }
  else {
    hpcpush(center);
    hpcpush(starting_point);
    }
  }

// !siid equals pseudohept(c)

void geometry_information::generate_floorshapes_for(int id, cell *c, int siid, int sidir) {
  DEBBI(DF_POLY, ("generate_floorshapes_for ", id));

  for(auto pfsh: all_plain_floorshapes) {
    auto& fsh = *pfsh;

    if(STDVAR && (standard_tiling() || bt::in())) {

      ld hexside = fsh.rad0, heptside = fsh.rad1;
      
      for(int k=0; k<SIDEPARS; k++) sizeto(fsh.side[k], id);
      
      ld td = (PURE && !(S7&1)) ? S42+S6 : 0;
      if(&fsh == &shBigHepta) td += S6;
      
      if(S3 >= OINF && !(S7 & 1)) td = S42 * 1. / S7;
    
      int b = 0;
      if(S3 == 4 && BITRUNCATED) b += S14;
  
      if(id)
        bshape_regular(fsh, id, S7, td, heptside, c);
      
      else if(PURE) {
        if(&fsh == &shTriheptaFloor)
          bshape_regular(fsh, 0, S7/2, 0, hexside, c);
        else if(&fsh == &shBigTriangle)
          bshape_regular(fsh, 0, S7/2, S12, hexside, c);
        else
          bshape_regular(fsh, 0, S7, td, heptside, c);
        }
      else if(&fsh == &shBigTriangle) 
        bshape_regular(fsh, 0, S3, b+S14, hexside, c);
      else if(&fsh == &shTriheptaFloor)
        bshape_regular(fsh, 0, S3, b, hexside, c);
      else 
        bshape_regular(fsh, 0, S6, S7, hexside, c);
    
      
      continue;
      }

    // special
    ld sca = 3 * shFullFloor.rad0 / fsh.rad0;
    
    vector<hyperpoint> cornerlist;
    
    int cor = c->type;
    bool apeirogonal = false;

    if(&fsh == &shTriheptaFloor) {
      if(!siid) {
        for(int i=0; i<cor; i++)
          cornerlist.push_back(midcorner(c, i, .5 - .01 * global_boundary_ratio));
        }
      else {
        for(int i=0; i<cor; i++) {
          int ri = i;
          if((i&1) == ((sidir+siid)&1)) ri--;
          ri = c->c.fix(ri);
          ld val = 3 + 0.1 * global_boundary_ratio;
          cornerlist.push_back(mid(get_corner_position(c, ri, val), get_corner_position(c, c->c.fix(ri+1), val)));
          }
        }
      }
    
    else if(&fsh == &shBigTriangle) {
      ld val = 1 - 0.06 * global_boundary_ratio;
      if(!siid) {
        for(int i=0; i<cor; i++) cornerlist.push_back(hpxy(0,0));
        }
      else if(geosupport_chessboard()) {
        for(int i=0; i<cor; i++) {
          hyperpoint nc = nearcorner(c, i);
          cornerlist.push_back(mid_at(hpxy(0,0), nc, val));
          }
        }
      else {
        for(int i=0; i<cor; i++) {
          int ri = i;
          if((i&1) != ((sidir+siid)&1)) ri--;
          ri = c->c.fix(ri);
          hyperpoint nc = nearcorner(c, ri);
          cornerlist.push_back(mid_at(hpxy(0,0), nc, val));
          }
        }
      }
  
    else if(&fsh == &shBigHepta) {
      ld val = 1 - 0.06 * global_boundary_ratio;
      if(!siid) {
        for(int i=0; i<cor; i++) {
          hyperpoint nc = nearcorner(c, i);
          cornerlist.push_back(mid_at(hpxy(0,0), nc, val));
          }
        }
      else {
        for(int i=0; i<cor; i++) cornerlist.push_back(hpxy(0,0));
        }
      }
    
    else if(arb::in()) {
      vector<hyperpoint> actual;
      for(int j=0; j<cor; j++) 
        actual.push_back(get_corner_position(c, j));
      
      ld min_dist = 1e3;
      for(int j=0; j<cor; j++) 
      for(int k=0; k<j; k++) {
        ld dist = hdist(actual[j], actual[k]);
        if(dist > 1e-6 && dist < min_dist)
          min_dist = dist;
        }
      
      auto &ac = arb::current_or_slided();
      ld dist = min_dist * (1 - 3 / sca) * ac.boundary_ratio;
      
      ld area = 0;
      for(int j=0; j<cor; j++) {
        hyperpoint current = kleinize(actual[j]);
        hyperpoint last = kleinize(actual[j?j-1:cor-1]);
        area += current[0] * last[1] - last[0] * current[1];
        }
      if(area < 0) dist = -dist;

      int id = arb::id_of(c->master);
      auto& sh = ac.shapes[id];
      apeirogonal = sh.apeirogonal;
      
      for(int j=0; j<cor; j++) {
        hyperpoint last = actual[j?j-1:cor-1];
        hyperpoint current = ypush(1e-6 * randd()) * xpush(1e-6) * actual[j];
        hyperpoint next = actual[j<cor-1?j+1:0];

        if(apeirogonal) {
          if(j == 0) last = arb::get_adj(arb::current_or_slided(), id, cor-1, id, cor-2, false) * actual[cor-3];
          if(j == cor-2) next = arb::get_adj(arb::current_or_slided(), id, cor-2, id, cor-1, false) * actual[1];
          if(j == cor-1) { cornerlist.push_back(sh.vertices.back()); continue; }
          }

        auto T = gpushxto0(current);
        last = T * last;
        next = T * next;
        hyperpoint a = rspintox(last) * ypush0(dist);
        hyperpoint b = rspintox(last) * xpush(hdist0(last)) * ypush0(dist);

        hyperpoint c = rspintox(next) * ypush0(-dist);
        hyperpoint d = rspintox(next) * xpush(hdist0(next)) * ypush0(-dist);
        
        hyperpoint h = linecross(a, b, c, d);
        
        cornerlist.push_back(rgpushxto0(current) * h);
        }
      }
  
    else {
      for(int j=0; j<cor; j++)
        cornerlist.push_back(get_corner_position(c, j, sca));
      }
    
    sizeto(fsh.b, id);

    bshape(fsh.b[id], fsh.prio);
    if(cor == 2) {
      /* give digons some width */
      for(int i=0; i<cor; i++) hpcpush(spin(-.1) * cornerlist[i]), hpcpush(spin(+.1) * cornerlist[i]);
      hpcpush(spin(-.1) * cornerlist[0]);
      }
    else if(&fsh == &shTriheptaFloor && cor == 4 && siid)
      /* trihepta floors generate digons too */
      for(int i=0; i<=cor; i++) hpcpush(spin((i&1) ? .1 : -.1) * cornerlist[i%cor]);
    else if(apeirogonal) {
      for(int i=0; i<=cor-2; i++) hpcpush(cornerlist[i]);
      finish_apeirogon(cornerlist.back());
      }
    else
      for(int i=0; i<=cor; i++) hpcpush(cornerlist[i%cor]);
    
    sizeto(fsh.shadow, id);
    bshape(fsh.shadow[id], fsh.prio);
    for(int i=0; i<=cor; i++)
      hpcpush(mid_at(hpxy(0,0), cornerlist[i%cor], SHADMUL));
    
    for(int k=0; k<SIDEPARS; k++) {
      if(isize(fsh.gpside[k]) < cor)
        fsh.gpside[k].resize(cor);
      for(int cid=0; cid<cor; cid++) {
        sizeto(fsh.gpside[k][cid], id);
        bshape(fsh.gpside[k][cid][id], fsh.prio);
        hpcpush(iddspin_side(c, cid) * cornerlist[cid]);
        hpcpush(iddspin_side(c, cid) * cornerlist[(cid+1)%cor]);
        chasmifyPoly(dlow_table[k], dhi_table[k], k);
        }
      }
    }
  
  for(auto pfsh: all_escher_floorshapes) {
  
    auto& fsh = *pfsh;
    
    sizeto(fsh.b, id);
    sizeto(fsh.shadow, id);
    
    if(STDVAR && standard_tiling()) {
      generate_matrices_scale(fsh.scale, fsh.noftype);
      if(PURE && geosupport_football() < 2) {
        bshape2(fsh.b[id], fsh.prio, fsh.shapeid2 ? fsh.shapeid2 : fsh.shapeid1, hept_matrices);
        }
      else {
        if(id == 0) bshape2(fsh.b[0], fsh.prio, fsh.shapeid0, hex_matrices);
        if(id == 1) bshape2(fsh.b[1], fsh.prio, fsh.shapeid1, hept_matrices);
        }
      generate_matrices_scale(fsh.scale * SHADMUL, fsh.noftype);
      if(PURE && geosupport_football() < 2) {
        bshape2(fsh.shadow[id], fsh.prio, fsh.shapeid2 ? fsh.shapeid2 : fsh.shapeid1, hept_matrices);
        }
      else {
        if(id == 0) bshape2(fsh.shadow[0], fsh.prio, fsh.shapeid0, hex_matrices);
        if(id == 1) bshape2(fsh.shadow[1], fsh.prio, fsh.shapeid1, hept_matrices);
        }
      }
    
    else {
      generate_matrices_scale(fsh.scale, fsh.noftype);

      auto& m = (siid && geosupport_football() == 2) ? hex_matrices : hept_matrices;
      
      int cor = c->type;
      bool apeirogonal = arb::is_apeirogonal(c);

      m.n.sym = cor;

      int v = sidir+siid;
      for(auto& mvi: m.v) mvi.second.resize(cor);
      
      for(int ii=0; ii<2; ii++) {
        int i = 0;       
        for(int d=0; d<m.o.sym; d++) {
          hyperpoint center = hpxy(0,0);
    
          for(int cid=0; cid<cor; cid++) {
            int dcidv = d + cid + v;
            if(apeirogonal) dcidv--;
            int dcidv1 = gmod(dcidv + 1, cor);
            int dcidv2 = gmod(dcidv + 2, cor);

            if(apeirogonal && dcidv1 >= cor-2) {
              for(int j: {0,1,2,3})
                m.v[i+j].second[cid][2][2] = APEIROGONAL_INVALID;
              continue;
              }

            hyperpoint nlcorner = get_corner_position(c, dcidv1, 3 / fsh.scale * (ii ? 1/SHADMUL : 1));
            hyperpoint nrcorner = get_corner_position(c, dcidv2, 3 / fsh.scale * (ii ? 1/SHADMUL : 1));

            hyperpoint nfar = nearcorner(c, dcidv1);

            hyperpoint nlfar = farcorner(c, dcidv1, 0);
            hyperpoint nrfar = farcorner(c, dcidv1, 1);
            m.v[i].second[cid] = build_matrix(center, nlcorner, nrcorner,C02);
            m.v[i+1].second[cid] = build_matrix(nfar, nlcorner, nrcorner,C02);
            m.v[i+2].second[cid] = build_matrix(nfar, nlcorner, nlfar,C02);
            m.v[i+3].second[cid] = build_matrix(nfar, nrcorner, nrfar,C02);
            }
          
          i += 4;
          }
  
        if(i != isize(m.v)) printf("warning: i=%d sm=%d\n", i, isize(m.v));
        bshape2((ii?fsh.shadow:fsh.b)[id], fsh.prio, (fsh.shapeid2 && geosupport_football() < 2) ? fsh.shapeid2 : siid?fsh.shapeid0:fsh.shapeid1, m);

        if(apeirogonal && !first) {
          int id = arb::id_of(c->master);
          auto &ac = arb::current_or_slided();
          auto& sh = ac.shapes[id];
          hpcpush(arb::get_adj(arb::current_or_slided(), id, cor-2, id, cor-1, false) * starting_point);
          finish_apeirogon(sh.vertices.back());
          }
        }
      }
    }
  
  #if MAXMDIM >= 4
  if(embedded_plane) {
    finishshape();
    for(auto pfsh: all_plain_floorshapes) {
      auto& fsh = *pfsh;
      
      for(int i=fsh.shadow[id].s; i<fsh.shadow[id].e; i++)
        hpc[i] = orthogonal_move(hpc[i], FLOOR - human_height / 100);

      for(int k=0; k<SIDEPARS; k++) {
        sizeto(fsh.levels[k], id);
        bshape(fsh.levels[k][id], fsh.prio);    
        last->flags |= POLY_TRIANGLES;
        last->tinf = &floor_texture_vertices[fsh.id];
        last->texture_offset = 0;

        if(1) {
          int s = fsh.b[id].s;
          int e = fsh.b[id].e-1;
          if(vid.pseudohedral) {
            hyperpoint ctr = Hypc;
            for(int t=0; t<e-s; t++)
              ctr += kleinize(cgi.emb->orthogonal_move(hpc[s+t], dfloor_table[k]));
            ctr = normalize(ctr);

            for(int t=0; t<e-s; t++) {
              hyperpoint v1 = kleinize(cgi.emb->orthogonal_move(hpc[s+t], dfloor_table[k])) - ctr;
              hyperpoint v2 = kleinize(cgi.emb->orthogonal_move(hpc[s+t+1], dfloor_table[k])) - ctr;
              texture_order([&] (ld x, ld y) {
                hpcpush(normalize(ctr + v1 * x + v2 * y));
                });
              }
            }
          if(!vid.pseudohedral) for(int t=0; t<e-s; t++) {

            hyperpoint v1 = cgi.emb->actual_to_logical(hpc[s+t]);
            hyperpoint v2 = cgi.emb->actual_to_logical(hpc[s+t+1]);

            texture_order([&] (ld x, ld y) { 
              hyperpoint a = v1 * x + v2 * y;
              a[2] = dfloor_table[k];
              auto c = cgi.emb->logical_to_actual(a);
              cgi.hpcpush(c);
              });
            }
          }

        finishshape();
        ensure_vertex_number(fsh.levels[k][id]);
        }
      
      for(int co=0; co<2; co++) {
        sizeto(fsh.cone[co], id);
        bshape(fsh.cone[co][id], fsh.prio);    
        last->flags |= POLY_TRIANGLES;
        last->tinf = &floor_texture_vertices[fsh.id];
        last->texture_offset = 0;
        ld h = (FLOOR - WALL) / (co+1);
        ld top = co ? (FLOOR + WALL) / 2 : WALL;
        if(1) {
          int s = fsh.b[id].s;
          int e = fsh.b[id].e-1;        
          for(int t=0; t<e-s; t++) {
            hyperpoint v1 = cgi.emb->actual_to_logical(hpc[s+t]);
            hyperpoint v2 = cgi.emb->actual_to_logical(hpc[s+t+1]);
            texture_order([&] (ld x, ld y) {
              hyperpoint a = v1 * x + v2 * y; a[2] = top + h * (x+y);
              hpcpush(cgi.emb->logical_to_actual(a));
              });
            }
          }

        finishshape();
        ensure_vertex_number(fsh.cone[co][id]);
        }

      for(int l=0; l<SIDEPARS; l++) {
        for(auto& li: fsh.side[l]) 
          bind_floor_texture(li, fsh.id);
        if(isize(fsh.gpside[l]) < c->type)
          fsh.gpside[l].resize(c->type);
        for(auto& gs: fsh.gpside[l]) {
          for(auto& li: gs) 
            bind_floor_texture(li, fsh.id);
          }
        }
      }
      
    for(auto pfsh: all_escher_floorshapes) {
      auto& fsh = *pfsh;
      
      for(int l=0; l<SIDEPARS; l++) {
        fsh.levels[l] = shFullFloor.levels[l];
        fsh.shadow = shFullFloor.shadow;
        for(auto& li: fsh.levels[l]) bind_floor_texture(li, fsh.id);
        fsh.side[l] = shFullFloor.side[l];
        for(auto& li: fsh.side[l]) bind_floor_texture(li, fsh.id);
        if(isize(fsh.gpside[l]) < c->type)
          fsh.gpside[l].resize(c->type);
        for(int e=0; e<c->type; e++) {
          fsh.gpside[l][e] = shFullFloor.gpside[l][e];
          for(auto& li: fsh.gpside[l][e]) 
            bind_floor_texture(li, fsh.id);
          }
        fsh.cone[0] = shFullFloor.cone[0];
        fsh.cone[1] = shFullFloor.cone[1];
        for(int c=0; c<2; c++)
          for(auto& li: fsh.cone[c])
            bind_floor_texture(li, fsh.id);
        }
      }
    finishshape();
    }
  #endif
  }

void geometry_information::generate_floorshapes() {

  DEBBI(DF_POLY, ("generate_floorshapes"));
  
  heptagon modelh;
  cell model;
  model.master = &modelh;
  modelh.c7 = &model;
  model.type = modelh.type = FULL_EDGE;
  
  auto mmerge1 = [&] (int i, int j) { model.c.setspin(i, j, false); modelh.c.setspin(i, j, false); };  
  auto mmerge = [&] (int i, int j) { mmerge1(i, j); mmerge1(j, i); };  

  for(int i=0; i<FULL_EDGE; i++) {
    model.move(i) = &model;
    modelh.move(i) = &modelh;
    model.c.setspin(i, i, false);
    modelh.c.setspin(i, i, false);
    }

  model.type = modelh.type = S7;

  if(WDIM == 3) ;
  
  #if CAP_IRR
  else if(IRREGULAR) {
    DEBBI(DF_POLY, ("generate_floorshapes: irregular"));

    int cc = isize(irr::cells);
    
    for(int id=0; id<cc; id++) {
      irr::cellindex[&model] = id;
      auto& vs = irr::cells[id];
      model.type = isize(vs.vertices);
      int siid = !vs.is_pseudohept;
      int sidir = 0;
      if(siid) sidir = irr::cells[vs.neid[0]].is_pseudohept;
      generate_floorshapes_for(id, &model, !vs.is_pseudohept, sidir);
      }

    printf("done\n");
    }
  #endif
    
  else if(GOLDBERG_INV) { /* will be generated on the fly */ }

  else if(inforder::mixed()) { /* will be generated on the fly */ }
  
  #if CAP_BT
  else if(kite::in()) {
    dynamicval<bool> ncor(approx_nearcorner, true);
    for(int i=0; i<2; i++) {
      modelh.s = hstate(i); /* kite/dart shape */
      kite::no_adj = true;
      generate_floorshapes_for(i, &model, 0, 0);
      kite::no_adj = false;
      }
    }
  #endif
  
  #if CAP_ARCM
  else if(arcm::in()) {
    arcm::parent_index_of(&modelh) = 0;
    auto &ac = arcm::current;
    for(int i=0; i<2*ac.N + 2; i++) {
      if(ac.regular && i>=2 && i < 2*ac.N) continue;
      arcm::id_of(&modelh) = i;
      model.type = isize(ac.triangles[i]);
      if(DUAL) model.type /= 2, arcm::parent_index_of(&modelh) = !(i&1);
      
      if(BITRUNCATED)
        generate_floorshapes_for(i, &model, !arcm::pseudohept(&model), arcm::pseudohept(&model) ? 0 : 1^(i&1));
      else if(geosupport_football() == 2)
        generate_floorshapes_for(i, &model, !arcm::pseudohept(&model), i >= 4 ? 1 : 0);
      else
        generate_floorshapes_for(i, &model, 0, 0);
      }
    }
  #endif
  
  else if(arb::in()) {
    auto& c = arb::current;
    int n = isize(c.shapes);
    vector<cell> models(n);
    vector<heptagon> modelh(n);
    for(int i=0; i<n; i++) {
      auto &ms = models[i];
      auto &mh = modelh[i];
      mh.fieldval = -1;
      for(auto& t: ms.c.move_table) t = nullptr;
      for(auto& t: mh.c.move_table) t = nullptr;
      }
    for(int i=0; i<n; i++) {
      auto &ms = models[i];
      auto &mh = modelh[i];
      ms.master = &mh;
      mh.c7 = &ms;
      mh.zebraval = i;
      auto& sh = c.shapes[i];
      ms.type = mh.type = sh.size();
      }
    for(int i=0; i<n; i++) {
      auto &ms = models[i];
      auto &mh = modelh[i];
      auto& sh = c.shapes[i];
      for(int j=0; j<sh.size(); j++) {
        auto& co = sh.connections[j];
        mh.c.connect(j, &modelh[co.sid], co.eid, co.mirror);
        ms.c.connect(j, &models[co.sid], co.eid, co.mirror);
        }
      }
    for(int i=0; i<n; i++) generate_floorshapes_for(i, &models[i], c.shapes[i].football_type < 2, c.shapes[i].football_type == 0);
    }
  
  else if(geometry == gBinary4) {
    for(int i: {0,1}) {
      modelh.zebraval = i;
      mmerge(2, 4); mmerge(0, 3); mmerge(1, 3); mmerge(i, 3);
      generate_floorshapes_for(i, &model, 1, 0);
      }
    }

  else if(geometry == gTernary) {
    for(int i: {0,1,2}) {
      modelh.zebraval = i;
      mmerge(3, 5); for(int a=0; a<3; a++) mmerge1(a, 4); mmerge(4, i);
      generate_floorshapes_for(i, &model, 1, 0);
      }
    }

  else if(PURE && geometry != gBinaryTiling && geosupport_football() < 2) {
    generate_floorshapes_for(0, &model, 1, 0);
    }

  #if CAP_BT
  else if(bt::in()) {
    dynamicval<hrmap*> c(currentmap, bt::new_alt_map(nullptr));
    model.type = S6; generate_floorshapes_for(0, &model, 0, 0);
    model.type = S7; generate_floorshapes_for(1, &model, 1, 0);
    delete currentmap;
    }
  #endif

  else {
    static hrmap_standard stdmap;
    dynamicval<hrmap*> c(currentmap, &stdmap);
    // cell model;
    model.type = S6; generate_floorshapes_for(0, &model, 0, 0);
    model.type = S7; generate_floorshapes_for(1, &model, 0, 0);
    }
  }

#if CAP_GP
EX namespace gp {
  
  EX void clear_plainshapes() {
    for(int m=0; m<3; m++)
    for(int sd=0; sd<8; sd++)
    for(int i=0; i<GOLDBERG_LIMIT; i++)
    for(int j=0; j<GOLDBERG_LIMIT; j++)
    for(int k=0; k<8; k++)
      cgi.gpdata->pshid[m][sd][i][j][k] = -1;
    cgi.gpdata->nextid = 0;
    }

  void build_plainshape(int& id, gp::local_info& li, cell *c0, int siid, int sidir) {
    cgi.require_shapes();
    id = cgi.gpdata->nextid++;
  
    bool master = !(li.relative.first||li.relative.second);
    int cor = master ? S7 : SG6;
    if(master) li.last_dir = -1;
    DEBB(DF_GP, (format("last=%d at=%d,%d tot=%d siid=%d sidir=%d cor=%d id=%d\n", li.last_dir, li.relative.first, li.relative.second, li.total_dir, siid, sidir, cor, id)));
      
    cgi.generate_floorshapes_for(id, c0, siid, sidir);
    
    cgi.finishshape();
    cgi.extra_vertices();
    }
  
  EX int get_plainshape_id(cell *c) {
    if(li_for != c) {
      li_for = c;
      current_li = get_local_info(c);
      }
    int siid, sidir;
    cell *c1 = c;
    auto f = [&] {
      if(geosupport_threecolor() == 2) {
        auto si = patterns::getpatterninfo(c1, patterns::PAT_COLORING, patterns::SPF_NO_SUBCODES);
        siid = si.id>>2;
        // if(siid == 2) si.dir++;
        // if(siid != pattern_threecolor(c)) printf("threecolor mismatch\n");
        // if(pattern_threecolor(createMov(c, c->fixd(si.dir))) != (siid+1)%3) printf("threecolor mismatch direction\n");
        sidir = c1->c.fix(si.dir);
        }
      else if(geosupport_football() == 2) {
        siid = !pseudohept(c1);
        sidir = !ishex1(c1);
        }
      else if(geosupport_chessboard()) {
        siid = !chessvalue(c1);
        sidir = 0;
        }
      else {
        siid = 0;
        sidir = 0;
        }
      };
    if(INVERSE && gp::variation_for(gp::param) == eVariation::goldberg) {
      c1 = gp::get_mapped(c);
      UIU(f());
      }
    else if(INVERSE) {
      siid = 0;
      sidir = 0;
      }
    else f();
    auto& id = cgi.gpdata->pshid[siid][sidir][current_li.relative.first&GOLDBERG_MASK][current_li.relative.second&GOLDBERG_MASK][gmod(current_li.total_dir, S6)];
    if(id == -1 && sphere && isize(cgi.shFloor.b) > 0) {
      forCellEx(c1, c) if(!gmatrix0.count(c1)) return 0;
      }
    if(id == -1) build_plainshape(id, current_li, c, siid, sidir);
    return id;
    }
  EX }
#endif

qfloorinfo qfi;

EX void set_no_floor() {
  qfi.fshape = NULL;
  qfi.shape = NULL;
  qfi.tinf = NULL;
  qfi.usershape = -1;
  }

EX void set_floor(floorshape& sh) {
  qfi.fshape = &sh;
  qfi.shape = NULL;
  qfi.tinf = NULL;
  qfi.usershape = -1;
  }

EX void set_floor(hpcshape& sh) {
  qfi.shape = &sh;
  qfi.fshape = NULL;
  qfi.spin = Id;
  qfi.tinf = NULL;
  qfi.usershape = -1;
  }

EX void set_floor(const transmatrix& spin, hpcshape& sh) {
  qfi.shape = &sh;
  qfi.fshape = NULL;
  qfi.spin = spin;
  qfi.usershape = -1;
  }

EX int shvid(cell *c) {
  return currentmap->shvid(c);
  }

int hrmap_standard::shvid(cell *c) {
  if(GOLDBERG)
    return gp::get_plainshape_id(c);
  #if CAP_IRR
  else if(IRREGULAR)
    return irr::cellindex[c];
  #endif
  else if(geosupport_football() == 2)
    return pseudohept(c);
  else if(inforder::mixed()) {
    int t = c->type;
    static vector<bool> computed;
    if(isize(computed) <= t) computed.resize(t+1);
    if(!computed[t]) {
      computed[t] = true;
      cell model;
      heptagon modelh;
      model.type = t;
      modelh.type = t;
      S7 = t;
      for(int i=0; i<S7; i++) {
        model.move(i) = &model;
        modelh.move(i) = &modelh;
        model.c.setspin(i, i, false);
        modelh.c.setspin(i, i, false);
        }

      cgi.tessf = edge_of_triangle_with_angles(0, M_PI/t, M_PI/t);
      cgi.crossf = cgi.tessf;
      
      cgi.require_shapes();
      println(hlog, "generating floorshapes for ", t);
      cgi.generate_floorshapes_for(t, &model, 0, 0);
      cgi.finishshape();
      cgi.extra_vertices();
      }
    return t;
    }
  else if(PURE)
    return 0;
  else
    return ctof(c);
  }

EX struct dqi_poly *draw_shapevec(cell *c, const shiftmatrix& V, const vector<hpcshape> &shv, color_t col, PPR prio IS(PPR::DEFAULT)) {
  if(no_wall_rendering) return NULL;
  if(!c) return &queuepolyat(V, shv[0], col, prio);
  else if(WDIM == 3) return NULL;
  else if(currentmap->strict_tree_rules()) return &queuepolyat(V, shv[shvid(c)], col, prio);

  #if CAP_GP
  else if(GOLDBERG) {
    int id = gp::get_plainshape_id(c);
    if(isize(shv) > id) return &queuepolyat(V, shv[id], col, prio);
    return NULL;
    }
  #endif
  #if CAP_IRR
  else if(IRREGULAR) {
    int id = irr::cellindex[c];
    if(id < 0 || id >= isize(shv)) {
      return NULL;
      }
    return &queuepolyat(V, shv[id], col, prio);
    }
  #endif
  #if CAP_ARCM
  else if(arcm::in()) {
    return &queuepolyat(V, shv[shvid(c)], col, prio);
    }
  #endif
  else if(GOLDBERG && ishex1(c)) 
    return &queuepolyat(V * pispin, shv[0], col, prio);
  else if(!(S7&1) && PURE && !kite::in() && !a4) {
    auto si = patterns::getpatterninfo(c, patterns::PAT_COLORING, 0);
    if(si.id == 8) si.dir++;
    transmatrix D = applyPatterndir(c, si);
    return &queuepolyat(V*D, shv[shvid(c)], col, prio);
    }
  else 
    return &queuepolyat(V, shv[shvid(c)], col, prio);
  }

EX void draw_floorshape(cell *c, const shiftmatrix& V, const floorshape &fsh, color_t col, PPR prio IS(PPR::DEFAULT)) {
  if(no_wall_rendering) return;
  draw_shapevec(c, V, fsh.b, col, prio);
  }

EX void draw_qfi(cell *c, const shiftmatrix& V, color_t col, PPR prio IS(PPR::DEFAULT), vector<hpcshape> floorshape::* tab IS(&floorshape::b)) {
  if(no_wall_rendering) return;
  if(qfi.shape)
    queuepolyat(V * qfi.spin, *qfi.shape, col, prio);
  else if(qfi.usershape >= 0) {
    mapeditor::drawUserShape(V * qfi.spin, mapeditor::sgFloor, qfi.usershape, col, c);
    }
  else if(!qfi.fshape) ;
#if CAP_TEXTURE
  else if(qfi.tinf) {
    auto& poly = queuetable(V * qfi.spin, qfi.tinf->vertices, isize(qfi.tinf->vertices), texture::config.mesh_color, texture::config.recolor(col), prio == PPR::DEFAULT ? PPR::FLOOR : prio);
    poly.tinf = qfi.tinf;
    poly.offset_texture = 0;
    poly.flags = POLY_INVERSE;
    }
#endif
  else draw_shapevec(c, V, (qfi.fshape->*tab), col, prio);
  }

EX bool floorshape_debug;
EX void viewmat() {
  if(floorshape_debug) {
    shiftmatrix V = ggmatrix(cwt.at);
    
    for(int i=0; i<cwt.at->type; i++) {
      shiftpoint ci = V * get_corner_position(cwt.at, i);
      shiftpoint ci1 = V * get_corner_position(cwt.at, (i+1) % cwt.at->type);

      shiftpoint cn = V * nearcorner(cwt.at, i);
      shiftpoint cf0 = V * farcorner(cwt.at, i, 0);
      shiftpoint cf1 = V * farcorner(cwt.at, i, 1);
      queuestr(ci, 20, its(i), 0x0000FF, 1);
      if(vid.grid)
        queuestr(cn, 20, its(i), 0x00FF00, 1);
      else 
        queuestr(V * currentmap->adj(cwt.at, i) * C0, 20, its(i), 0x00FFFF, 1);
      queueline(V * C0, ci, 0xFFFFFFFF, 3);
      queueline(ci, ci1, 0xFFFF00FF, 3);
      queueline(ci, cn, 0xFF00FFFF, 3);
      queueline(ci1, cn, 0xFF0000FF, 3);
      queueline(ci, cf0, 0x00FFFFFF, 3);
      queueline(cn, cf0, 0x00FF00FF, 3);
      queueline(cn, cf1, 0x0000FFFF, 3);
      }
    }
  }

auto floor_hook = arg::add1("-floordebug", [] { floorshape_debug = true; });
#endif

#if MAXMDIM < 4 || !CAP_GL
EX void ensure_vertex_number(basic_textureinfo& bti, int qty) {}
EX void ensure_vertex_number(hpcshape& sh) {}
EX void bind_floor_texture(hpcshape& li, int id) {}
#endif

#if MAXMDIM >= 4 && CAP_GL

EX ld floor_texture_square_size;

void draw_shape_for_texture(floorshape* sh) {

  int id = sh->id;
  
  const ld s1 = 1;
  const ld s3 = 3 * s1;
  const ld sd = s1/2;

  ld gx = (id % 8) * s3 - 3.5 * s3;
  ld gy = (id / 8) * s3 - 3.5 * s3;

  if(1) {
    dynamicval<ld> v(vid.linewidth, 8);
    curvepoint(eupush(gx+s1, gy-s1) * C0);
    curvepoint(eupush(gx+s1, gy+s1) * C0);
    curvepoint(eupush(gx-s1, gy+s1) * C0);
    curvepoint(eupush(gx-s1, gy-s1) * C0);
    curvepoint(eupush(gx+s1, gy-s1) * C0);
    queuecurve(shiftless(Id), 0x000000FF, 0xFFFFFFFF - 0x1010100 * (sh->pstrength * 24/10), PPR::LAKELEV);
    }

  poly_outline = 0xFFFFFFFF - 0x1010100 * (sh->pstrength * 3/2);

  for(int a=-1; a<=1; a++)
  for(int b=-1; b<=1; b++)
    queuepoly(shiftless(eupush(gx+a, gy+b)), sh->b[0], 0xFFFFFFFF);

  if(sh == &cgi.shCrossFloor) {
    queuepoly(shiftless(eupush(gx, gy) * spin(45._deg)), cgi.shCross, 0x808080FF);
    }

  if(1) {
    dynamicval<ld> v(vid.linewidth, 8);
    curvepoint(eupush(gx+sd, gy-sd) * C0);
    curvepoint(eupush(gx+sd, gy+sd) * C0);
    curvepoint(eupush(gx-sd, gy+sd) * C0);
    curvepoint(eupush(gx-sd, gy-sd) * C0);
    curvepoint(eupush(gx+sd, gy-sd) * C0);
    queuecurve(shiftless(Id), 0x40404000 + sh->fstrength * 192/10, 0, PPR::LINE);
    }
  
  for(int i=0; i<(ISMOBILE ? 10 : 1000); i++) {
    hyperpoint h1 = hpxy(sd * (6*randd()-3), sd * (6*randd()-3));
    hyperpoint h2 = hpxy(sd * (6*randd()-3), sd * (6*randd()-3));
    ld d = hdist(h1, h2);
    hyperpoint h3 = h1 + (h2-h1) /d * min(d, .1);
    for(int a=0; a<4; a++) {
      curvepoint(eupush(gx,gy) * eupush(spin(90._deg*a) * h1) * C0);
      curvepoint(eupush(gx,gy) * eupush(spin(90._deg*a) * h3) * C0);
      queuecurve(shiftless(Id), 0x10101010, 0, PPR::LINE);
      }
    }
  
  auto& ftv = floor_texture_vertices[sh->id];
  ftv.tvertices.clear();
  ftv.texture_id = floor_textures->renderedTexture;
  
  hyperpoint center = eupush(gx, gy) * C0;
  hyperpoint v1 = hpxyz3(sd, sd, 0, 0);
  hyperpoint v2 = hpxyz3(sd, -sd, 0, 0);

  if(1) {
    hyperpoint inmodel;
    applymodel(shiftless(center), inmodel);
    glvertex tmap;
    tmap[0] = (1 + inmodel[0] * pconf.scale) / 2;
    tmap[1] = (1 - inmodel[1] * pconf.scale) / 2;
    applymodel(shiftless(center + v1), inmodel);
    tmap[2] = (1 + inmodel[0] * pconf.scale) / 2 - tmap[0];
    floor_texture_map[sh->id] = tmap;
    }

  auto tvec_at = [&] (ld x, ld y) {
    hyperpoint h = center + v1 * x + v2 * y;
    hyperpoint inmodel;
    applymodel(shiftless(h), inmodel);
    glvec2 v;
    v[0] = (1 + inmodel[0] * pconf.scale) / 2;
    v[1] = (1 - inmodel[1] * pconf.scale) / 2;
    return v;
    };
  
  // SL2 needs 6 times more
  texture_order([&] (ld x, ld y) {
    auto v = tvec_at(x, y);
    ftv.tvertices.push_back(glhr::makevertex(v[0], v[1], 0));
    });
  
  floor_texture_square_size = 2 * (tvec_at(1, 0)[0] - tvec_at(0, 0)[0]);
  }

/** copy the texture vertices so that there are at least qty of them */
EX void ensure_vertex_number(basic_textureinfo& bti, int qty) {
  int s = isize(bti.tvertices);
  if(!s) return;
  while(isize(bti.tvertices) <= qty) {
    for(int i=0; i<s; i++) bti.tvertices.push_back(bti.tvertices[i]);
    }
  }

/** ensure_vertex_number for a hpcshape */
EX void ensure_vertex_number(hpcshape& sh) {
  ensure_vertex_number(*sh.tinf, sh.e - sh.s);
  }

EX void bind_floor_texture(hpcshape& li, int id) {
  li.tinf = &floor_texture_vertices[id];
  ensure_vertex_number(li);
  }

#if HDR
const int FLOORTEXTURESIZE = 4096;
#endif

void geometry_information::make_floor_textures_here() {
  require_shapes();

  dynamicval<videopar> vi(vid, vid);
  vid.xres = FLOORTEXTURESIZE;
  vid.yres = FLOORTEXTURESIZE;
  pconf.scale = 0.125;
  pconf.camera_angle = 0;
  pconf.alpha = 1;
  dynamicval<ld> lw(vid.linewidth, 2);

  floor_textures = new renderbuffer(vid.xres, vid.yres, vid.usingGL);
  resetbuffer rb;

  int q = isize(all_escher_floorshapes) + isize(all_plain_floorshapes);
  floor_texture_vertices.resize(q);
  floor_texture_map.resize(q);
  
  auto cd = current_display;
  cd->xtop = cd->ytop = 0;
  cd->xsize = cd->ysize = FLOORTEXTURESIZE;
  cd->xcenter = cd->ycenter = cd->scrsize = FLOORTEXTURESIZE/2;
  
  cd->radius = cd->scrsize * pconf.scale;

  floor_textures->enable();
  #if CAP_VR
  dynamicval<int> i(vrhr::state, 0);
  #endif
  floor_textures->clear(0); // 0xE8E8E8 = 1
  
  // gradient vertices
  vector<glhr::colored_vertex> gv;
  gv.emplace_back(-1, -1, 0, 0, 0);
  gv.emplace_back(+1, -1, 0, 0, 0);
  gv.emplace_back(+1, +1, 1, 1, 1);
  gv.emplace_back(-1, -1, 0, 0, 0);
  gv.emplace_back(+1, +1, 1, 1, 1);
  gv.emplace_back(-1, +1, 1, 1, 1);
  
  #if CAP_RAY
  dynamicval<bool> riu(ray::in_use, false);
  #endif
  
  if(1) {
    current_display->next_shader_flags = GF_VARCOLOR;
    dynamicval<eModel> m(pmodel, mdPixel);
    current_display->set_all(0,0);
    glhr::new_projection();
    glhr::id_modelview();
    glhr::prepare(gv);
    glhr::set_depthtest(false);
    glDrawArrays(GL_TRIANGLES, 0, isize(gv));
    }
  
  shOverFloor.pstrength = 20;
  shFeatherFloor.pstrength = 40;
  shFeatherFloor.fstrength = 5;
  shTrollFloor.pstrength = 25;
  shCaveFloor.pstrength = 40;
  shCaveFloor.fstrength = 0;
  shDesertFloor.pstrength = 30;
  shDesertFloor.fstrength =10;
  shRoseFloor.pstrength = 30;
  shDragonFloor.pstrength = 30;
  shBarrowFloor.pstrength = 40;

  // all using Tortoise
  for(auto v: all_escher_floorshapes) if(v->shapeid2 == 178) v->pstrength = 20;
  
  ptds.clear();
  
  for(auto v: all_plain_floorshapes) draw_shape_for_texture(v);
  for(auto v: all_escher_floorshapes) draw_shape_for_texture(v);
  
  drawqueue();
  
  /*
  SDL_Surface *sdark = floor_textures->render();
  IMAGESAVE(sdark, "texture-test.png"); 
  */
  rb.reset();
  last_texture_step = vid.texture_step;
  }

EX void make_floor_textures() {
  if(noGUI || !vid.usingGL) return;
  DEBBI(DF_POLY, ("make_floor_textures"));
  dynamicval<eGeometry> g(geometry, gEuclidSquare);
  dynamicval<eModel> gm(pmodel, mdDisk);
  dynamicval<eVariation> va(variation, eVariation::pure);
  dynamicval<geometryinfo1> gie(ginf[geometry].g, giEuclid2);
  dynamicval<flagtype> gief(ginf[geometry].flags, qOPTQ);
  dynamicval<geometryinfo1> gih(ginf[gNormal].g, giHyperb2);
  dynamicval<flagtype> gihf(ginf[gNormal].flags, 0);
  dynamicval<bool> a3(vid.always3, false);
  dynamicval<bool> hq(inHighQual, true);
  dynamicval<int> hd(darken, 0);
  dynamicval<ld> hll(levellines, 0);
  dynamicval<ld> gd(vid.depth, 1);
  dynamicval<ld> gc(vid.camera, 1);
  dynamicval<geometry_information*> dcgip(cgip, cgip);
  dynamicval<eStereo> gvs(vid.stereo_mode, sOFF);
  dynamicval<int> vgp(global_projection, 0);
  check_cgi();
  cgi.make_floor_textures_here();
  /* update texture ID in existing cgi's */
  for(auto& c: cgis) c.second.models_texture.texture_id = floor_textures->renderedTexture;
  }


#endif
}
