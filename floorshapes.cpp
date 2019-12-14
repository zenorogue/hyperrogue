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

void geometry_information::init_floorshapes() {
  all_escher_floorshapes.clear();
  all_plain_floorshapes = { 
    &shFloor, &shMFloor, &shMFloor2, &shMFloor3, &shMFloor4, 
    &shFullFloor, &shBigTriangle, &shTriheptaFloor, &shBigHepta
    };
  
  for(auto s: all_plain_floorshapes) s->is_plain = true;
  
  auto init_escher = [this] (escher_floorshape& sh, int s0, int s1, int noft=0, int s2=0) {
    sh.shapeid0 = s0;
    sh.shapeid1 = s1;
    sh.noftype = noft;
    sh.shapeid2 = s2;
    sh.scale = 1;
    sh.is_plain = false;
    all_escher_floorshapes.push_back(&sh);
    };
  
  init_escher(shStarFloor, 1,2);
  init_escher(shCloudFloor, 3, 4);
  init_escher(shCrossFloor, 5, 6, 2, 54);
  init_escher(shChargedFloor, 7, 385, 1, 10);
  init_escher(shSStarFloor, 11, 12);
  init_escher(shOverFloor, 13, 15, 1, 14);
  init_escher(shTriFloor, 17, 18, 0, 385);
  init_escher(shFeatherFloor, 19, 21, 1, 20);
  init_escher(shBarrowFloor, 23, 24, 1, 25);
  init_escher(shNewFloor, 26, 27, 2, 54);
  init_escher(shTrollFloor, 28, 29); 
  init_escher(shButterflyFloor, 325, 326, 1, 178);
  init_escher(shLavaFloor, 359, 360, 1, 178);
  init_escher(shLavaSeabed, 386, 387, 1, 178);
  init_escher(shSeabed, 334, 335);
  init_escher(shCloudSeabed, 336, 337);
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
    init_escher(shRedRockFloor[i], 55, 56);
  init_escher(shDragonFloor, 181, 182, 2, 183); /* dragon */
  
  int ids = 0;
  for(auto sh: all_plain_floorshapes) sh->id = ids++;
  for(auto sh: all_escher_floorshapes) sh->id = ids++;
  }

typedef pair<transmatrix, array<transmatrix, MAX_EDGE>> matrixitem;

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
  hyperpoint bnlfar = xpush(v0) * spin(M_PI) * rspintox(rot) * rspintox(rot) * rspintox(rot) * xpush0(hdist0(rot));
  hyperpoint bnrfar = xpush(v0) * spin(M_PI) * spintox(rot) * spintox(rot) * spintox(rot) * xpush0(hdist0(rot));

  m.lcorner = xspinpush0 (bspi-M_PI/sym, main);
  m.rcorner = xspinpush0 (bspi+M_PI/sym, main);
  m.mfar[0] = xspinpush0 (bspi, v0);
  m.mfar[1] = xspinpush0 (bspi, v1);
  m.vfar[0] = spin(bspi) * bnlfar;
  m.vfar[2] = spin(bspi) * bnrfar;
  m.vfar[1] = spin(-2*M_PI/sym) * m.vfar[2];
  m.vfar[3] = spin(+2*M_PI/sym) * m.vfar[0];
  
  return m;
  }

struct matrixlist {
  mesher o, n;
  vector<matrixitem> v;
  };

matrixitem genitem(const transmatrix& m1, const transmatrix& m2, int nsym) {
  matrixitem mi;
  mi.first = m1;
  for(int i=0; i<nsym; i++)
    mi.second[i] = spin(2*M_PI*i/nsym) * m2;
  return mi;
  }

bool do_kleinize() { return S3 >= OINF; }

EX hyperpoint kleinize(hyperpoint h) { 
  if(GDIM == 2) return point3(h[0]/h[2], h[1]/h[2], 1); 
  else return point31(h[0]/h[3], h[1]/h[3], h[2]/h[3]);
  }

void addmatrix(matrixlist& matrices, hyperpoint o0, hyperpoint o1, hyperpoint o2, hyperpoint n0, hyperpoint n1, hyperpoint n2, int d, int osym, int nsym) {
  if(do_kleinize()) o0 = kleinize(o0), o1 = kleinize(o1), o2 = kleinize(o2), n0 = kleinize(n0), n1 = kleinize(n1), n2 = kleinize(n2);
  matrices.v.push_back(genitem(inverse(spin(2*M_PI*d/osym)*build_matrix(o0, o1, o2,C02)), spin(2*M_PI*d/nsym)*build_matrix(n0, n1, n2,C02), nsym));
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
  while((spin(2*M_PI / rots) * T* lstmid)[0] < (T*lstmid)[0])
    T = spin(2*M_PI / rots) * T;
  while((spin(-2*M_PI / rots) * T* lstmid)[0] < (T*lstmid)[0])
    T = spin(-2*M_PI / rots) * T;
  T = spin(m.o.bspi) * T;
  for(auto &pp: lst) pp = T * pp;
  
  if(osym % rots && rots % osym) printf("warning: rotation oddity (shapeid %d, osym=%d rots=%d)\n", shapeid, osym, rots);

  if(rots > osym && rots % osym == 0) {
    int rep = rots / osym;
    int s = lst.size();
    for(int i=0; i<s*(rep-1); i++)
      lst.push_back(spin(2*M_PI/rots) * lst[i]);
    rots /= rep;
    }
  
  bshape(sh, prio);

  for(int r=0; r<nsym; r+=osym/rots) {
    for(hyperpoint h: lst) {
      hyperpoint nh = h;
      if(do_kleinize()) nh = kleinize(nh);
      int mapped = 0;
      for(auto& m: matrices) {        
        hyperpoint z = m.first * h;
        if(z[0] > -1e-5 && z[1] > -1e-5 && z[2] > -1e-5) {
          nh = m.second[r] * z, mapped++;
          }
        }
      if(mapped == 0) printf("warning: not mapped (shapeid %d)\n", shapeid);
      hpcpush(mid(nh, nh));
      }
    }
  
  hpcpush(hpc[last->s]);
  }

template<class T> void sizeto(T& t, int n) {
  if(isize(t) <= n) t.resize(n+1);
  }

void geometry_information::bshape_regular(floorshape &fsh, int id, int sides, int shift, ld size, cell *c) {
  
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
      
      for(int i=0; i<sides; i++) {
        hyperpoint h0 = bt::get_corner_horo_coordinates(c, i) * size;
        hyperpoint h1 = bt::get_corner_horo_coordinates(c, i+1) * size;
        if(t) h0 *= SHADMUL, h1 *= SHADMUL;
        hyperpoint hd = (h1 - h0) / STEP;
        for(int j=0; j<STEP; j++)
          hpcpush(bt::get_horopoint(h0 + hd * j));
        }
      
      hpcpush(hpc[last->s]);
      }
    
    for(int k=0; k<SIDEPARS; k++) {
      for(int i=0; i<c->type; i++) {
        sizeto(fsh.gpside[k][i], id);
        bshape(fsh.gpside[k][i][id], PPR::LAKEWALL); 
        hyperpoint h0 = bt::get_corner_horo_coordinates(c, i) * size;
        hyperpoint h1 = bt::get_corner_horo_coordinates(c, i+1) * size;
        hyperpoint hd = (h1 - h0) / STEP;
        for(int j=0; j<=STEP; j++)
          hpcpush(iddspin(c, i) * bt::get_horopoint(h0 + hd * j));
        chasmifyPoly(dlow_table[k], dhi_table[k], k);
        }
      }

    return;
    }
  #endif
  
  bshape(fsh.b[id], fsh.prio);
  for(int t=0; t<=sides; t++)
    hpcpush(xspinpush0(t*2 * M_PI / sides + shift * M_PI / S42, size));

  bshape(fsh.shadow[id], fsh.prio);
  for(int t=0; t<=sides; t++)
    hpcpush(xspinpush0(t*2 * M_PI / sides + shift * M_PI / S42, size * SHADMUL));

  for(int k=0; k<SIDEPARS; k++) {
    fsh.side[k].resize(2);
    bshape(fsh.side[k][id], PPR::LAKEWALL);
    hpcpush(xspinpush0(+M_PI/sides, size));
    hpcpush(xspinpush0(-M_PI/sides, size));
    chasmifyPoly(dlow_table[k], dhi_table[k], k);
    }
  }

#if CAP_IRR
namespace irr { void generate_floorshapes(); }
#endif

// !siid equals pseudohept(c)

void geometry_information::generate_floorshapes_for(int id, cell *c, int siid, int sidir) {
  DEBBI(DF_POLY, ("generate_floorshapes_for ", id));

  for(auto pfsh: all_plain_floorshapes) {
    auto& fsh = *pfsh;

    if(STDVAR && (standard_tiling() || bt::in())) {

      ld hexside = fsh.rad0, heptside = fsh.rad1;
      
      for(int k=0; k<SIDEPARS; k++) sizeto(fsh.side[k], id);
      
      int td = (PURE && !(S7&1)) ? S42+S6 : 0;
      if(&fsh == &shBigHepta) td += S6;
    
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

    if(&fsh == &shTriheptaFloor) {
      if(!siid) {
        for(int i=0; i<cor; i++)
          cornerlist.push_back(midcorner(c, i, .49));
        }
      else {
        for(int i=0; i<cor; i++) {
          int ri = i;
          if((i&1) == ((sidir+siid)&1)) ri--;
          ri = c->c.fix(ri);
          cornerlist.push_back(mid(get_corner_position(c, ri, 3.1), get_corner_position(c, c->c.fix(ri+1), 3.1)));
          }
        }
      }
    
    else if(&fsh == &shBigTriangle) {
      if(!siid) {
        for(int i=0; i<cor; i++) cornerlist.push_back(hpxy(0,0));
        }
      else {
        for(int i=0; i<cor; i++) {
          int ri = i;
          if((i&1) != ((sidir+siid)&1)) ri--;
          ri = c->c.fix(ri);
          hyperpoint nc = nearcorner(c, ri);
          cornerlist.push_back(mid_at(hpxy(0,0), nc, .94));
          }
        }
      }
  
    else if(&fsh == &shBigHepta) {
      if(!siid) {
        for(int i=0; i<cor; i++) {
          hyperpoint nc = nearcorner(c, i);
          cornerlist.push_back(mid_at(hpxy(0,0), nc, .94));
          }
        }
      else {
        for(int i=0; i<cor; i++) cornerlist.push_back(hpxy(0,0));
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
    else
      for(int i=0; i<=cor; i++) hpcpush(cornerlist[i%cor]);
    
    sizeto(fsh.shadow, id);
    bshape(fsh.shadow[id], fsh.prio);
    for(int i=0; i<=cor; i++)
      hpcpush(mid_at(hpxy(0,0), cornerlist[i%cor], SHADMUL));
    
    for(int k=0; k<SIDEPARS; k++) 
      for(int cid=0; cid<cor; cid++) {
        sizeto(fsh.gpside[k][cid], id);
        bshape(fsh.gpside[k][cid][id], fsh.prio);
        hpcpush(iddspin(c, cid) * cornerlist[cid]);
        hpcpush(iddspin(c, cid) * cornerlist[(cid+1)%cor]);
        chasmifyPoly(dlow_table[k], dhi_table[k], k);
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
          
      m.n.sym = cor;

      int v = sidir+siid;
      
      for(int ii=0; ii<2; ii++) {
        int i = 0;       
        for(int d=0; d<m.o.sym; d++) {
          hyperpoint center = hpxy(0,0);
    
          for(int cid=0; cid<cor; cid++) {
            hyperpoint nlcorner = get_corner_position(c, (d+cid+v+1) % cor, 3 / fsh.scale * (ii ? 1/SHADMUL : 1));
            hyperpoint nrcorner = get_corner_position(c, (d+cid+v+2) % cor, 3 / fsh.scale * (ii ? 1/SHADMUL : 1));
            
            hyperpoint nfar = nearcorner(c, (d+cid+v+1) % cor);
            
            hyperpoint nlfar = farcorner(c, (d+cid+v+1) % cor, 0);
            hyperpoint nrfar = farcorner(c, (d+cid+v+1) % cor, 1);
            m.v[i].second[cid] = build_matrix(center, nlcorner, nrcorner,C02);
            m.v[i+1].second[cid] = build_matrix(nfar, nlcorner, nrcorner,C02);
            m.v[i+2].second[cid] = build_matrix(nfar, nlcorner, nlfar,C02);
            m.v[i+3].second[cid] = build_matrix(nfar, nrcorner, nrfar,C02);
            }
          
          i += 4;
          }
  
        if(i != isize(m.v)) printf("warning: i=%d sm=%d\n", i, isize(m.v));      
        bshape2((ii?fsh.shadow:fsh.b)[id], fsh.prio, (fsh.shapeid2 && geosupport_football() < 2) ? fsh.shapeid2 : siid?fsh.shapeid0:fsh.shapeid1, m);
        }
      }
    }
  
  #if MAXMDIM >= 4
  if(WDIM == 2 && GDIM == 3) {
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

        #if CAP_BT
        if(bt::in()) 
          for(int t=0; t<c->type; t++)
            texture_order([&] (ld x, ld y) {
              hyperpoint left = bt::get_corner_horo_coordinates(c, t);
              hyperpoint right = bt::get_corner_horo_coordinates(c, t+1);
              hpcpush(orthogonal_move(bt::get_horopoint(left * x + right * y), dfloor_table[k]));
              });
        else 
        #endif
        if(1) {
          int s = fsh.b[id].s;
          int e = fsh.b[id].e-1;        
          for(int t=0; t<e-s; t++) {
            hyperpoint v1 = hpc[s+t] - C0;
            hyperpoint v2 = hpc[s+t+1] - C0;
            texture_order([&] (ld x, ld y) { hpcpush(orthogonal_move(normalize(C0 + v1 * x + v2 * y), dfloor_table[k])); });
            }
          }
        }
      
      for(int co=0; co<2; co++) {
        sizeto(fsh.cone[co], id);
        bshape(fsh.cone[co][id], fsh.prio);    
        last->flags |= POLY_TRIANGLES;
        last->tinf = &floor_texture_vertices[fsh.id];
        last->texture_offset = 0;
        ld h = (FLOOR - WALL) / (co+1);
        ld top = co ? (FLOOR + WALL) / 2 : WALL;
        #if CAP_BT
        if(bt::in())
          for(int t=0; t<c->type; t++)
            texture_order([&] (ld x, ld y) {
              hyperpoint left = bt::get_corner_horo_coordinates(c, t);
              hyperpoint right = bt::get_corner_horo_coordinates(c, t+1);
              hpcpush(orthogonal_move(bt::get_horopoint(left * x + right * y), top + h * (x+y)));
              });
        else
        #endif
        if(1) {
          int s = fsh.b[id].s;
          int e = fsh.b[id].e-1;        
          for(int t=0; t<e-s; t++) {
            hyperpoint v1 = hpc[s+t] - C0;
            hyperpoint v2 = hpc[s+t+1] - C0;
            texture_order([&] (ld x, ld y) { hpcpush(orthogonal_move(normalize(C0 + v1 * x + v2 * y), top + h * (x+y))); });
            }
          }
        }

      for(int l=0; l<SIDEPARS; l++) {
        for(auto& li: fsh.side[l]) li.tinf = &floor_texture_vertices[fsh.id];
        for(int e=0; e<MAX_EDGE; e++)
          for(auto& li: fsh.gpside[l][e]) li.tinf = &floor_texture_vertices[fsh.id];
        }
      }
      
    for(auto pfsh: all_escher_floorshapes) {
      auto& fsh = *pfsh;
      
      for(int l=0; l<SIDEPARS; l++) {
        fsh.levels[l] = shFullFloor.levels[l];
        fsh.shadow = shFullFloor.shadow;
        for(auto& li: fsh.levels[l]) li.tinf = &floor_texture_vertices[fsh.id];
        fsh.side[l] = shFullFloor.side[l];
        for(auto& li: fsh.side[l]) li.tinf = &floor_texture_vertices[fsh.id];
        for(int e=0; e<MAX_EDGE; e++) {
          fsh.gpside[l][e] = shFullFloor.gpside[l][e];
          for(auto& li: fsh.gpside[l][e]) li.tinf = &floor_texture_vertices[fsh.id];
          }
        fsh.cone[0] = shFullFloor.cone[0];
        fsh.cone[1] = shFullFloor.cone[1];
        for(int c=0; c<2; c++)
          for(auto& li: fsh.cone[c])
            li.tinf = &floor_texture_vertices[fsh.id];
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
  model.type = modelh.type = S7;
  
  auto mmerge1 = [&] (int i, int j) { model.c.setspin(i, j, false); modelh.c.setspin(i, j, false); };  
  auto mmerge = [&] (int i, int j) { mmerge1(i, j); mmerge1(j, i); };  

  for(int i=0; i<S7; i++) {
    model.move(i) = &model;
    modelh.move(i) = &modelh;
    model.c.setspin(i, i, false);
    modelh.c.setspin(i, i, false);
    }

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
    
  else if(GOLDBERG) { /* will be generated on the fly */ }
  
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
      ms.master = &mh;
      mh.c7 = &ms;
      mh.zebraval = i;
      auto& sh = c.shapes[i];
      ms.type = mh.type = sh.size();
      for(int j=0; j<sh.size(); j++) {
        auto& co = sh.connections[j];
        mh.c.connect(j, &modelh[get<0>(co)], get<1>(co), get<2>(co));
        ms.c.connect(j, &models[get<0>(co)], get<1>(co), get<2>(co));
        }
      }
    for(int i=0; i<n; i++) generate_floorshapes_for(i, &models[i], 0, 0);
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

  else {
    cell model;
    model.type = S6; generate_floorshapes_for(0, &model, 0, 0);
    model.type = S7; generate_floorshapes_for(1, &model, bt::in() ? 0 : 1, 0);
    }
  }

#if CAP_GP
EX namespace gp {
  int pshid[3][8][32][32][8];
  int nextid;
  
  EX void clear_plainshapes() {
    for(int m=0; m<3; m++)
    for(int sd=0; sd<8; sd++)
    for(int i=0; i<32; i++)
    for(int j=0; j<32; j++)
    for(int k=0; k<8; k++)
      pshid[m][sd][i][j][k] = -1;
    nextid = 0;
    }

  void build_plainshape(int& id, gp::local_info& li, cell *c0, int siid, int sidir) {
    id = nextid++;
  
    bool master = !(li.relative.first||li.relative.second);
    int cor = master ? S7 : SG6;
    if(master) li.last_dir = -1;
    DEBB(DF_GP, (format("last=%d at=%d,%d tot=%d siid=%d sidir=%d cor=%d id=%d\n", li.last_dir, li.relative.first, li.relative.second, li.total_dir, siid, sidir, cor, id)));
      
    cgi.generate_floorshapes_for(id, c0, siid, sidir);
    
    cgi.finishshape();
    cgi.extra_vertices();
    }
  
  int get_plainshape_id(cell *c) {
    int siid, sidir;
    if(geosupport_threecolor() == 2) {
      auto si = patterns::getpatterninfo(c, patterns::PAT_COLORING, patterns::SPF_NO_SUBCODES);
      siid = si.id>>2;
      // if(siid == 2) si.dir++;
      // if(siid != pattern_threecolor(c)) printf("threecolor mismatch\n");
      // if(pattern_threecolor(createMov(c, c->fixd(si.dir))) != (siid+1)%3) printf("threecolor mismatch direction\n");
      sidir = c->c.fix(si.dir);
      }
    else if(geosupport_football() == 2) {
      siid = !pseudohept(c);
      sidir = !ishex1(c);
      }
    else {
      siid = 0;
      sidir = 0;
      }
    auto& id = pshid[siid][sidir][draw_li.relative.first&31][draw_li.relative.second&31][gmod(draw_li.total_dir, S6)];
    if(id == -1 && sphere && isize(cgi.shFloor.b) > 0) {
      forCellEx(c1, c) if(!gmatrix0.count(c1)) return 0;
      }
    if(id == -1) build_plainshape(id, draw_li, c, siid, sidir);
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
  if(hybri) {
    cell *c1 = hybrid::get_where(c).first; 
    return PIU( shvid(c1) );
    }
  else if(GOLDBERG)
    return gp::get_plainshape_id(c);
  else if(IRREGULAR)
    return irr::cellindex[c];
  else if(arcm::in())
    return arcm::id_of(c->master);
  else if(arb::in())
    return arb::id_of(c->master);
  else if(geosupport_football() == 2)
    return pseudohept(c);
  else if(geometry == gBinaryTiling)
    return c->type-6;
  else if(kite::in())
    return kite::getshape(c->master);
  else if(geometry == gBinary4 || geometry == gTernary)
    return c->master->zebraval;
  else if(PURE)
    return 0;
  else
    return ctof(c);
  }

EX struct dqi_poly *draw_shapevec(cell *c, const transmatrix& V, const vector<hpcshape> &shv, color_t col, PPR prio IS(PPR::DEFAULT)) {
  if(!c) return &queuepolyat(V, shv[0], col, prio);
  else if(WDIM == 3) return NULL;
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
    return &queuepolyat(V, shv[arcm::id_of(c->master)], col, prio);
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

EX void draw_floorshape(cell *c, const transmatrix& V, const floorshape &fsh, color_t col, PPR prio IS(PPR::DEFAULT)) {
  draw_shapevec(c, V, fsh.b, col, prio);
  }

EX void draw_qfi(cell *c, const transmatrix& V, color_t col, PPR prio IS(PPR::DEFAULT), vector<hpcshape> floorshape::* tab IS(&floorshape::b)) {
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
    transmatrix V = ggmatrix(cwt.at);
    
    for(int i=0; i<cwt.at->type; i++) {
      hyperpoint ci = V * get_corner_position(cwt.at, i);
      hyperpoint ci1 = V * get_corner_position(cwt.at, (i+1) % cwt.at->type);

      hyperpoint cn = V * nearcorner(cwt.at, i);
      hyperpoint cf0 = V * farcorner(cwt.at, i, 0);
      hyperpoint cf1 = V * farcorner(cwt.at, i, 1);
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

#if CAP_COMMANDLINE
auto floor_hook = 
  addHook(hooks_args, 100, [] () {
    using namespace arg;             
    if(argis("-floordebug")) { floorshape_debug = true; return 0; }
    else return 1;
    });
#endif
#endif

#if MAXMDIM >= 4

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
    queuecurve(0x000000FF, 0xFFFFFFFF - 0x1010100 * (sh->pstrength * 24/10), PPR::LAKELEV);
    }

  poly_outline = 0xFFFFFFFF - 0x1010100 * (sh->pstrength * 3/2);

  for(int a=-1; a<=1; a++)
  for(int b=-1; b<=1; b++)
    queuepoly(eupush(gx+a, gy+b), sh->b[0], 0xFFFFFFFF);

  if(sh == &cgi.shCrossFloor) {
    queuepoly(eupush(gx, gy) * spin(M_PI/4), cgi.shCross, 0x808080FF);
    }

  if(1) {
    dynamicval<ld> v(vid.linewidth, 8);
    curvepoint(eupush(gx+sd, gy-sd) * C0);
    curvepoint(eupush(gx+sd, gy+sd) * C0);
    curvepoint(eupush(gx-sd, gy+sd) * C0);
    curvepoint(eupush(gx-sd, gy-sd) * C0);
    curvepoint(eupush(gx+sd, gy-sd) * C0);
    queuecurve(0x40404000 + sh->fstrength * 192/10, 0, PPR::LINE);
    }
  
  auto& ftv = floor_texture_vertices[sh->id];
  ftv.tvertices.clear();
  ftv.texture_id = floor_textures->renderedTexture;
  
  hyperpoint center = eupush(gx, gy) * C0;
  hyperpoint v1 = hpxyz3(sd, sd, 0, 0);
  hyperpoint v2 = hpxyz3(sd, -sd, 0, 0);

  if(1) {
    hyperpoint inmodel;
    applymodel(center, inmodel);
    glvertex tmap;
    tmap[0] = (1 + inmodel[0] * vid.scale) / 2;
    tmap[1] = (1 - inmodel[1] * vid.scale) / 2;
    applymodel(center + v1, inmodel);
    tmap[2] = (1 + inmodel[0] * vid.scale) / 2 - tmap[0];
    floor_texture_map[sh->id] = tmap;
    }

  // SL2 needs 6 times more
  for(int a=0; a<MAX_EDGE*6; a++)
    texture_order([&] (ld x, ld y) {
      hyperpoint h = center + v1 * x + v2 * y;
      hyperpoint inmodel;
      applymodel(h, inmodel);
      glvec2 v;
      v[0] = (1 + inmodel[0] * vid.scale) / 2;
      v[1] = (1 - inmodel[1] * vid.scale) / 2;
      ftv.tvertices.push_back(glhr::makevertex(v[0], v[1], 0));
      });
  }

const int FLOORTEXTURESIZE = 4096;

void geometry_information::make_floor_textures_here() {
  require_shapes();

  dynamicval<videopar> vi(vid, vid);
  vid.xres = FLOORTEXTURESIZE;
  vid.yres = FLOORTEXTURESIZE;
  vid.scale = 0.125;
  vid.camera_angle = 0;
  vid.alpha = 1;
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
  
  cd->radius = cd->scrsize * vid.scale;

  floor_textures->enable();
  floor_textures->clear(0); // 0xE8E8E8 = 1
  
  // gradient vertices
  vector<glhr::colored_vertex> gv;
  gv.emplace_back(-1, -1, 0, 0, 0);
  gv.emplace_back(+1, -1, 0, 0, 0);
  gv.emplace_back(+1, +1, 1, 1, 1);
  gv.emplace_back(-1, -1, 0, 0, 0);
  gv.emplace_back(+1, +1, 1, 1, 1);
  gv.emplace_back(-1, +1, 1, 1, 1);
  
  dynamicval<bool> riu(ray::in_use, false);
  
  if(1) {
    current_display->next_shader_flags = GF_VARCOLOR;
    dynamicval<eModel> m(pmodel, mdPixel);
    current_display->set_all(0);
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
  dynamicval<geometryinfo1> gih(ginf[gNormal].g, giHyperb2);
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
