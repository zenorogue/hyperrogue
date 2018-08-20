namespace hr {

vector<plain_floorshape*> all_plain_floorshapes;
vector<escher_floorshape*> all_escher_floorshapes;

plain_floorshape
  shFloor, 
  shMFloor, shMFloor2, shMFloor3, shMFloor4, shFullFloor,
  shBigTriangle, shTriheptaFloor, shBigHepta;

escher_floorshape shStarFloor(1,2), 
  shCloudFloor(3, 4),
  shCrossFloor(5, 6, 2, 54),
  shChargedFloor(7, 385, 1, 10),
  shSStarFloor(11, 12),
  shOverFloor(13, 15, 1, 14),
  shTriFloor(17, 18, 0, 385),
  shFeatherFloor(19, 21, 1, 20),
  shBarrowFloor(23, 24, 1, 25),
  shNewFloor(26, 27, 2, 54),
  shTrollFloor(28, 29), 
  shButterflyFloor(325, 326, 1, 178),
  shLavaFloor(359, 360, 1, 178),
  shSeabed(334, 335),
  shCloudSeabed(336, 337),
  shCaveSeabed(338, 339, 2, 54),
  shPalaceFloor(45, 46, 0, 385),
  shDemonFloor(51, 50, 1, 178),
  shCaveFloor(52, 53, 2, 54),
  shDesertFloor(55, 56, 0, 4),
  shPowerFloor(57, 58, 0, 12), /* dragon */
  shRoseFloor(174, 175, 1, 173),
  shSwitchFloor(377, 378, 1, 379),
  shTurtleFloor(176, 177, 1, 178),
  shRedRockFloor[3] = {{55, 56}, {55, 56}, {55, 56}}, // 1 - .1 * i   
  shDragonFloor(181, 182, 2, 183); /* dragon */

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

void addmatrix(matrixlist& matrices, hyperpoint o0, hyperpoint o1, hyperpoint o2, hyperpoint n0, hyperpoint n1, hyperpoint n2, int d, int osym, int nsym) {
  matrices.v.push_back(genitem(inverse(spin(2*M_PI*d/osym)*build_matrix(o0, o1, o2)), spin(2*M_PI*d/nsym)*build_matrix(n0, n1, n2), nsym));
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
  if(nonbitrunc) {
    mesher nall = msh(geometry, S7, rhexf, tessf, tessf, -M_PI, scale);
    bool use = geosupport_graveyard() < 2;
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
    generate_matrices(hex_matrices, ohex, msh(geometry, S6, hexvdist, hexhexdist, hcrossf, (S3-3)*M_PI/S3, scale));
    generate_matrices(hept_matrices, ohept, msh(geometry, S7, rhexf, hcrossf, hcrossf, euclid6?0:euclid4?0:M_PI/S7, scale));
    }
  }

void bshape2(hpcshape& sh, int p, int shapeid, matrixlist& m) {
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
  using namespace hyperpoint_vec;
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
  
  bshape(sh, p);

  for(int r=0; r<nsym; r+=osym/rots) {
    for(hyperpoint h: lst) {
      hyperpoint nh = h;
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

void horopoint(ld y, ld x) {
  hpcpush(get_horopoint(y, x));
  }

void horopoint(ld y, ld x, cell &fc, int c) {
  hpcpush(iddspin(&fc, c) * get_horopoint(y, x));
  }

void horoline(ld y, ld x1, ld x2) {
  for(int a=0; a<=16; a++)
    horopoint(y, x1 + (x2-x1) * a / 16.);
  }

void horoline(ld y, ld x1, ld x2, cell &fc, int c) {
  for(int a=0; a<=16; a++)
    horopoint(y, x1 + (x2-x1) * a / 16., fc, c);
  }

void bshape_regular(floorshape &fsh, int id, int sides, int shift, ld size) {
  
  fsh.b.resize(2);
  fsh.shadow.resize(2);

  if(binarytiling) {
    bshape(fsh.b[id], fsh.prio);
    
    ld yx = size * log(2) / 2;
    ld yy = yx;
    ld xx = size / sqrt(2)/2;
    horoline(-yx, -xx, xx); horoline(yx, xx*2, -xx*2); horopoint(-yx, -xx);

    bshape(fsh.shadow[id], fsh.prio);
    horoline(-yx*SHADMUL, -xx*SHADMUL, xx*SHADMUL); horoline(yx*SHADMUL, xx*SHADMUL*2, -xx*SHADMUL*2); horopoint(-yx*SHADMUL, -xx*SHADMUL);

    cell fc;
    fc.type = 6+id;

    for(int k=0; k<SIDEPARS; k++) {
      for(int i=0; i<fc.type; i++) fsh.gpside[k][i].resize(2);
      bshape(fsh.gpside[k][0][id], PPR_LAKEWALL); horopoint(-yy, xx, fc, 0); horopoint(yy, 2*xx, fc, 0); chasmifyPoly(dlow_table[k], dhi_table[k], k);
      bshape(fsh.gpside[k][1][id], PPR_LAKEWALL); horoline(yy, 2*xx, xx, fc, 1); chasmifyPoly(dlow_table[k], dhi_table[k], k);
      bshape(fsh.gpside[k][2][id], PPR_LAKEWALL); horoline(yy, xx, -xx, fc, 2); chasmifyPoly(dlow_table[k], dhi_table[k], k);
      bshape(fsh.gpside[k][3][id], PPR_LAKEWALL); horoline(yy, -xx, -2*xx, fc, 3); chasmifyPoly(dlow_table[k], dhi_table[k], k);
      bshape(fsh.gpside[k][4][id], PPR_LAKEWALL); horopoint(yy, -2*xx, fc, 4); horopoint(-yy, -xx, fc, 4); chasmifyPoly(dlow_table[k], dhi_table[k], k);
      if(id == 0) {
        bshape(fsh.gpside[k][5][id], PPR_LAKEWALL); horoline(-yy, -xx, xx, fc, 5); chasmifyPoly(dlow_table[k], dhi_table[k], k);
        }
      else {
        bshape(fsh.gpside[k][5][id], PPR_LAKEWALL); horoline(-yy, -xx, 0, fc, 5); chasmifyPoly(dlow_table[k], dhi_table[k], k);
        bshape(fsh.gpside[k][6][id], PPR_LAKEWALL); horoline(-yy, -0, xx, fc, 6); chasmifyPoly(dlow_table[k], dhi_table[k], k);
        }
      }

    return;
    }
  
  bshape(fsh.b[id], fsh.prio);
  for(int t=0; t<=sides; t++)
    hpcpush(xspinpush0(t*2 * M_PI / sides + shift * M_PI / S42, size));

  bshape(fsh.shadow[id], fsh.prio);
  for(int t=0; t<=sides; t++)
    hpcpush(xspinpush0(t*2 * M_PI / sides + shift * M_PI / S42, size * SHADMUL));

  for(int k=0; k<SIDEPARS; k++) {
    fsh.side[k].resize(2);
    bshape(fsh.side[k][id], PPR_LAKEWALL);
    hpcpush(xspinpush0(M_PI/sides, size));
    hpcpush(xspinpush0(-M_PI/sides, size));
    chasmifyPoly(dlow_table[k], dhi_table[k], k);
    }
  }

namespace irr { void generate_floorshapes(); }

template<class T> void sizeto(T& t, int n) {
  if(isize(t) <= n) t.resize(n+1);
  }

// !siid equals pseudohept(c)

void generate_floorshapes_for(int id, cell *c, int siid, int sidir) {

  for(auto pfsh: all_plain_floorshapes) {
    auto& fsh = *pfsh;

    if(!gp::on && !irr::on && !archimedean) {

      // standard and binary
      ld hexside = fsh.rad0, heptside = fsh.rad1;
      
      for(int k=0; k<SIDEPARS; k++) sizeto(fsh.side[k], id);
      
      int td = ((nonbitrunc || euclid) && !(S7&1)) ? S42+S6 : 0;
      if(&fsh == &shBigHepta) td += S6;
    
      int b = 0;
      if(S3 == 4 && !nonbitrunc) b += S14;
  
      if(id == 1)
        bshape_regular(fsh, 1, S7, td, heptside);
      
      else if(nonbitrunc) {
        if(&fsh == &shTriheptaFloor)
          bshape_regular(fsh, 0, S7/2, 0, hexside);
        else if(&fsh == &shBigTriangle)
          bshape_regular(fsh, 0, S7/2, S12, hexside);
        else
          bshape_regular(fsh, 0, S7, td, heptside);
        }
      else if(&fsh == &shBigTriangle) 
        bshape_regular(fsh, 0, S3, b+S14, hexside);
      else if(&fsh == &shTriheptaFloor)
        bshape_regular(fsh, 0, S3, b, hexside);
      else 
        bshape_regular(fsh, 0, S6, S7, hexside);
    
      
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
          ri = fixdir(ri, c);
          cornerlist.push_back(mid(get_corner_position(c, ri, 3.1), get_corner_position(c, (ri+1) % c->type, 3.1)));
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
          ri = fixdir(ri, c);
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
    for(int i=0; i<=cor; i++) hpcpush(cornerlist[i%cor]);
    
    sizeto(fsh.shadow, id);
    bshape(fsh.shadow[id], fsh.prio);
    for(int i=0; i<=cor; i++)
      hpcpush(mid_at(hpxy(0,0), cornerlist[i%cor], SHADMUL));
    
    // printf("at = %d,%d cor = %d sca = %lf\n", li.relative.first, li.relative.second, cor, sca);

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
    
    if(!gp::on && !irr::on && !binarytiling && !archimedean) {
      generate_matrices_scale(fsh.scale, fsh.noftype);
      if(nonbitrunc && geosupport_graveyard() < 2 && fsh.shapeid2) {
        if(id == 0) bshape2(fsh.b[0], fsh.prio, fsh.shapeid2, hept_matrices);
        if(id == 1) bshape2(fsh.b[1], fsh.prio, fsh.shapeid2, hept_matrices);
        }
      else {
        if(id == 0) bshape2(fsh.b[0], fsh.prio, fsh.shapeid0, hex_matrices);
        if(id == 1) bshape2(fsh.b[1], fsh.prio, fsh.shapeid1, hept_matrices);
        }
      generate_matrices_scale(fsh.scale * SHADMUL, fsh.noftype);
      if(nonbitrunc && geosupport_graveyard() < 2 && fsh.shapeid2) {
        if(id == 0) bshape2(fsh.shadow[0], fsh.prio, fsh.shapeid2, hept_matrices);
        if(id == 1) bshape2(fsh.shadow[1], fsh.prio, fsh.shapeid2, hept_matrices);
        }
      else {
        if(id == 0) bshape2(fsh.shadow[0], fsh.prio, fsh.shapeid0, hex_matrices);
        if(id == 1) bshape2(fsh.shadow[1], fsh.prio, fsh.shapeid1, hept_matrices);
        }
      }
    
    else {
      generate_matrices_scale(fsh.scale, fsh.noftype);

      auto& m = (siid && geosupport_graveyard() == 2) ? hex_matrices : hept_matrices;
      
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
            m.v[i].second[cid] = build_matrix(center, nlcorner, nrcorner);
            m.v[i+1].second[cid] = build_matrix(nfar, nlcorner, nrcorner);
            m.v[i+2].second[cid] = build_matrix(nfar, nlcorner, nlfar);
            m.v[i+3].second[cid] = build_matrix(nfar, nrcorner, nrfar);
            }
          
          i += 4;
          }
  
        if(i != isize(m.v)) printf("warning: i=%d sm=%d\n", i, isize(m.v));      
        bshape2((ii?fsh.shadow:fsh.b)[id], fsh.prio, (fsh.shapeid2 && geosupport_graveyard() < 2) ? fsh.shapeid2 : siid?fsh.shapeid0:fsh.shapeid1, m);
        }
      }
    }
  }

void generate_floorshapes() {

  if(irr::on) {
    printf("generating irregular floorshapes...\n");
    cell model;

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
    
  else if(gp::on) { /* will be generated on the fly */ }
  
  else if(archimedean) {
    heptagon master;
    cell model;
    model.master = &master;
    arcm::parent_index_of(&master) = 0;
    auto &ac = arcm::current;
    for(int i=0; i<2*ac.N + (nonbitrunc ? 0 : 2); i++) {
      arcm::id_of(&master) = i;
      model.type = isize(ac.triangles[i]);
      if(geosupport_graveyard() == 2)
        generate_floorshapes_for(i, &model, !arcm::pseudohept(i), i/2);
      else
        generate_floorshapes_for(i, &model, 0, 0);
      }
    }
  
  else {
    cell model;
    model.type = S6; generate_floorshapes_for(0, &model, 0, 0);
    model.type = S7; generate_floorshapes_for(1, &model, 1, 0);
    }
  }

namespace gp {
  int pshid[3][8][32][32][8];
  int nextid;
  extern gp::local_info draw_li;
  
  void clear_plainshapes() {
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
    if(debug_geometry) 
      printf("last=%d at=%d,%d tot=%d siid=%d sidir=%d cor=%d id=%d\n", li.last_dir, li.relative.first, li.relative.second, li.total_dir, siid, sidir, cor, id);
      
    generate_floorshapes_for(id, c0, siid, sidir);
    
    finishshape(); last = NULL;
    extra_vertices();
    }
  
  int get_plainshape_id(cell *c) {
    int siid, sidir;
    if(geosupport_threecolor() == 2) {
      auto si = patterns::getpatterninfo(c, patterns::PAT_COLORING, 0);
      siid = si.id>>2;
      // if(siid == 2) si.dir++;
      // if(siid != pattern_threecolor(c)) printf("threecolor mismatch\n");
      // if(pattern_threecolor(createMov(c, fixdir(si.dir, c))) != (siid+1)%3) printf("threecolor mismatch direction\n");
      sidir = fixdir(si.dir, c);
      }
    else if(geosupport_graveyard() == 2) {
      siid = !pseudohept(c);
      sidir = !ishex1(c);
      }
    else {
      siid = 0;
      sidir = 0;
      }
    auto& id = pshid[siid][sidir][draw_li.relative.first&31][draw_li.relative.second&31][fix6(draw_li.total_dir)];
    if(id == -1) build_plainshape(id, draw_li, c, siid, sidir);
    return id;
    }
  }

qfloorinfo qfi;
qfloorinfo qfi_dc;

int chasmg;

void set_no_floor() {
  qfi.fshape = NULL;
  qfi.shape = NULL;
  }

void set_floor(floorshape& sh) {
  qfi.fshape = &sh;
  qfi.shape = NULL;
  qfi.tinf = NULL;
  }

void set_floor(hpcshape& sh) {
  qfi.shape = &sh;
  qfi.fshape = NULL;
  qfi.spin = Id;
  qfi.tinf = NULL;
  }

void set_floor(const transmatrix& spin, hpcshape& sh) {
  qfi.shape = &sh;
  qfi.fshape = NULL;
  qfi.spin = spin;
  }

void draw_shapevec(cell *c, const transmatrix& V, const vector<hpcshape> &shv, int col, int prio = -1) {
  if(!c) queuepolyat(V, shv[0], col, prio);
  else if(gp::on) {
    int id = gp::get_plainshape_id(c);
    queuepolyat(V, shv[id], col, prio);
    }
  else if(irr::on) {
    int id = irr::cellindex[c];
    if(id < 0 || id >= isize(shv)) {
      return;
      }
    queuepolyat(V, shv[id], col, prio);
    }
  else if(archimedean) {
    queuepolyat(V, shv[arcm::id_of(c->master)], col, prio);
    }
  else if((euclid || gp::on) && ishex1(c)) 
    queuepolyat(V * pispin, shv[0], col, prio);
  else if(!(S7&1) && nonbitrunc) {
    auto si = patterns::getpatterninfo(c, patterns::PAT_COLORING, 0);
    if(si.id == 8) si.dir++;
    transmatrix D = applyPatterndir(c, si);    
    queuepolyat(V*D, shv[pseudohept(c)], col, prio);
    }
  else if(geosupport_threecolor() == 2)
    queuepolyat(V, shv[pseudohept(c)], col, prio);
  else if(binarytiling)
    queuepolyat(V, shv[c->type-6], col, prio);
  else
    queuepolyat(V, shv[ctof(c)], col, prio);
  }

void draw_floorshape(cell *c, const transmatrix& V, const floorshape &fsh, int col, int prio = -1) {
  draw_shapevec(c, V, fsh.b, col, prio);
  }

void draw_qfi(cell *c, const transmatrix& V, int col, int prio = -1, vector<hpcshape> floorshape::* tab = &floorshape::b) {
  if(qfi.shape)
    queuepolyat(V * qfi.spin, *qfi.shape, col, prio);
  else if(!qfi.fshape) ;
#if CAP_TEXTURE
  else if(qfi.tinf) {
    queuetable(V * qfi.spin, qfi.tinf->vertices, isize(qfi.tinf->vertices), texture::config.mesh_color, texture::config.recolor(col), prio == -1 ? PPR_FLOOR : prio);
    lastptd().u.poly.tinf = qfi.tinf;
    if(true) 
      lastptd().u.poly.flags = POLY_INVERSE;
    }
#endif
  else draw_shapevec(c, V, (qfi.fshape->*tab), col, prio);
  }

bool floorshape_debug;
void viewmat() {
  if(floorshape_debug) {
    transmatrix V = ggmatrix(cwt.at);
    
    for(int i=0; i<cwt.at->type; i++) {
      hyperpoint ci = V * get_corner_position(cwt.at, i);
      hyperpoint ci1 = V * get_corner_position(cwt.at, (i+1) % cwt.at->type);
      hyperpoint cn = V * nearcorner(cwt.at, i);
      hyperpoint cf0 = V * farcorner(cwt.at, i, 0);
      hyperpoint cf1 = V * farcorner(cwt.at, i, 1);
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

auto hook = 
  addHook(hooks_args, 100, [] () {
    using namespace arg;             
    if(argis("-floordebug")) { floorshape_debug = true; return 0; }
    else return 1;
    });


}
