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

hyperpoint adist(ld a, ld x) {
  return spin(a) * xpush(x) * C0;
  }

typedef pair<transmatrix, array<transmatrix, 8>> matrixitem;

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

  hyperpoint rot = xpush(v0) * spin(M_PI - M_PI/sym) * xpush(main) * C0;
  hyperpoint bnlfar = xpush(v0) * spin(M_PI) * rspintox(rot) * rspintox(rot) * rspintox(rot) * xpush(hdist0(rot)) * C0;
  hyperpoint bnrfar = xpush(v0) * spin(M_PI) * spintox(rot) * spintox(rot) * spintox(rot) * xpush(hdist0(rot)) * C0;

  m.lcorner = adist (bspi-M_PI/sym, main);
  m.rcorner = adist (bspi+M_PI/sym, main);
  m.mfar[0] = adist (bspi, v0);
  m.mfar[1] = adist (bspi, v1);
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


void bshape_regular(floorshape &fsh, int id, int sides, int shift, ld size) {
  bshape(fsh.b[id], fsh.prio);
  for(int t=0; t<=sides; t++)
    hpcpush(ddi(t*S84 / sides + shift, size) * C0);

  bshape(fsh.shadow[id], fsh.prio);
  for(int t=0; t<=sides; t++)
    hpcpush(ddi(t*S84 / sides + shift, size * SHADMUL) * C0);

  for(int k=0; k<SIDEPARS; k++) {
    fsh.side[k].resize(2);
    bshape(fsh.side[k][id], PPR_LAKEWALL);
    hpcpush(ddi(S42/sides, size) * C0);
    hpcpush(ddi(-S42/sides, size) * C0);
    chasmifyPoly(dlow_table[k], dhi_table[k], k);
    }
  }

namespace irr { void generate_floorshapes(); }

void generate_floorshapes() {

  if(irr::on) {
    printf("generating irregular floorshapes...\n");
    irr::generate_floorshapes();
    printf("done\n");
    return;
    }
    
  if(gp::on) {
    return;
    }

  for(auto pfsh: all_plain_floorshapes) {
    auto& fsh = *pfsh;
    
    ld hexside = fsh.rad0, heptside = fsh.rad1;
    
    fsh.b.resize(2);
    fsh.shadow.resize(2);
    for(int k=0; k<SIDEPARS; k++) fsh.side[k].resize(2);
    
    int td = ((nonbitrunc || euclid) && !(S7&1)) ? S42+S6 : 0;
    if(&fsh == &shBigHepta) td += S6;
  
    int b = 0;
    if(S3 == 4 && !nonbitrunc) b += S14;

    bshape(fsh.b[0], fsh.prio);
    if(nonbitrunc) {
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
  
    bshape_regular(fsh, 1, S7, td, heptside);
    }
    
  for(auto pfsh: all_escher_floorshapes) {
    auto& fsh = *pfsh;
    generate_matrices_scale(fsh.scale, fsh.noftype);
    fsh.b.resize(2);
    if(nonbitrunc && geosupport_graveyard() < 2 && fsh.shapeid2) {
      bshape2(fsh.b[0], fsh.prio, fsh.shapeid2, hept_matrices);
      bshape2(fsh.b[1], fsh.prio, fsh.shapeid2, hept_matrices);
      }
    else {
      bshape2(fsh.b[0], fsh.prio, fsh.shapeid0, hex_matrices);
      bshape2(fsh.b[1], fsh.prio, fsh.shapeid1, hept_matrices);
      }
    }
  }

  hyperpoint mid3(hyperpoint h1, hyperpoint h2, hyperpoint h3) {
    using namespace hyperpoint_vec;
    return mid(h1+h2+h3, h1+h2+h3);
    }

  hyperpoint mid_at(hyperpoint h1, hyperpoint h2, ld v) {
    using namespace hyperpoint_vec;
    hyperpoint h = h1 * (1-v) + h2 * v;
    return mid(h, h);
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

  hyperpoint nearcorner(cell *c, local_info& li, int i) {
    cellwalker cw(c, i);
    cw += wstep;
    transmatrix cwm = shmup::calc_relative_matrix(cw.c, c, i);
    if(elliptic && cwm[2][2] < 0) cwm = centralsym * cwm;
    return cwm * C0;
    }
  
  hyperpoint hypercorner(cell *c, local_info& li, int i) {
    cellwalker cw(c, i);
    cw += wstep;
    transmatrix cwm = shmup::calc_relative_matrix(cw.c, c, i);
    if(elliptic && cwm[2][2] < 0) cwm = centralsym * cwm;
    auto li1 = get_local_info(cw.c);
    return cwm * get_corner_position(li1, (cw+2).spin);
    }

  hyperpoint midcorner(cell *c, local_info& li, int i, ld v) {
    auto hcor = hypercorner(c, li, i);
    auto tcor = get_corner_position(li, i, 3);
    return mid_at(tcor, hcor, v);
    }
  
  bool just_matrices = false;
  
  map<cell*, matrixlist> usedml;
  
  void build_plainshape(int& id, gp::local_info& li, cell *c0, int siid, int sidir) {
    if(!just_matrices) 
    id = nextid++;
  
    bool master = !(li.relative.first||li.relative.second);
    int cor = master ? S7 : SG6;
    if(master) li.last_dir = -1;
    if(debug_geometry) 
      printf("last=%d at=%d,%d tot=%d siid=%d sidir=%d cor=%d id=%d\n", li.last_dir, li.relative.first, li.relative.second, li.total_dir, siid, sidir, cor, id);
    
    for(auto pfsh: all_escher_floorshapes) {
      auto& fsh = *pfsh;
      generate_matrices_scale(1, fsh.noftype);
      auto& m = (siid && geosupport_graveyard() == 2) ? hex_matrices : hept_matrices;
      
      m.n.sym = cor;
      
      int i = 0;
      
      /* if(siid == 0)
        for(auto& ma: m.v) ma.first = ma.first * pispin; */
  
      for(int d=0; d<m.o.sym; d++) {
        hyperpoint center = hpxy(0,0);
  
        for(int c=0; c<cor; c++) {
          hyperpoint nlcorner = get_corner_position(li, d+c+sidir+siid+1, 3 / fsh.scale);
          hyperpoint nrcorner = get_corner_position(li, d+c+sidir+siid+2, 3 / fsh.scale);
          
          cellwalker cw(c0, c);
          cw += d+sidir+siid+1;
          int hint = cw.spin;
          cw += wstep;
          transmatrix cwm = shmup::calc_relative_matrix(cw.c, c0, hint);
          hyperpoint nfar = cwm*C0;
          auto li1 = get_local_info(cw.c);
          hyperpoint nlfar = cwm * get_corner_position(li1, (cw+2).spin);
          hyperpoint nrfar = cwm * get_corner_position(li1, (cw-1).spin);
          m.v[i].second[c] = build_matrix(center, nlcorner, nrcorner);
          m.v[i+1].second[c] = build_matrix(nfar, nlcorner, nrcorner);
          m.v[i+2].second[c] = build_matrix(nfar, nlcorner, nlfar);
          m.v[i+3].second[c] = build_matrix(nfar, nrcorner, nrfar);
          }
        
        i += 4;
        }
      
      if(i != isize(m.v)) printf("warning: i=%d sm=%d\n", i, isize(m.v));
      if(just_matrices) return;
      usedml[c0] = m;
      
      fsh.b.resize(nextid);
      m.n.sym = cor;
      bshape2(fsh.b[id], fsh.prio, (fsh.shapeid2 && geosupport_graveyard() < 2) ? fsh.shapeid2 : siid?fsh.shapeid0:fsh.shapeid1, m);
      }
    
    for(auto pfsh: all_plain_floorshapes) {
      auto& fsh = *pfsh;
        
      ld sca = 3 * shFullFloor.rad0 / fsh.rad0;
      
      fsh.b.resize(nextid);
      
      vector<hyperpoint> cornerlist;

      if(&fsh == &shTriheptaFloor) {
        if(!siid) {
          for(int i=0; i<cor; i++)
            cornerlist.push_back(midcorner(c0, li, i, .49));
          }
        else {
          for(int i=0; i<cor; i++) {
            int ri = i;
            if((i&1) == ((sidir+siid)&1)) ri--;
            cornerlist.push_back(mid(get_corner_position(li, ri, 3.1), get_corner_position(li, ri+1, 3.1)));
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
            ri = fixdir(ri, c0);
            hyperpoint nc = nearcorner(c0, li, ri);
            cornerlist.push_back(mid_at(hpxy(0,0), nc, .94));
            }
          }
        }

      else if(&fsh == &shBigHepta) {
        if(!siid) {
          for(int i=0; i<cor; i++) {
            hyperpoint nc = nearcorner(c0, li, i);
            cornerlist.push_back(mid_at(hpxy(0,0), nc, .94));
            }
          }
        else {
          for(int i=0; i<cor; i++) cornerlist.push_back(hpxy(0,0));
          }
        }

      else {
        for(int j=0; j<cor; j++)
          cornerlist.push_back(get_corner_position(li, j, sca));
        }
      
      bshape(fsh.b[id], fsh.prio);
      for(int i=0; i<=cor; i++) hpcpush(cornerlist[i%cor]);
      
      fsh.shadow.resize(nextid);
      bshape(fsh.shadow[id], fsh.prio);
      for(int i=0; i<=cor; i++)
        hpcpush(mid_at(hpxy(0,0), cornerlist[i%cor], SHADMUL));
      
      cell fc;
      fc.type = cor;
      
      // printf("at = %d,%d cor = %d sca = %lf\n", li.relative.first, li.relative.second, cor, sca);

      for(int k=0; k<SIDEPARS; k++) 
        for(int c=0; c<cor; c++) {
          fsh.gpside[k][c].resize(nextid);
          bshape(fsh.gpside[k][c][id], fsh.prio);
          hpcpush(iddspin(&fc, c) * cornerlist[c]);
          hpcpush(iddspin(&fc, c) * cornerlist[(c+1)%cor]);
          chasmifyPoly(dlow_table[k], dhi_table[k], k);
          }
      }
    
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
    if(id == -1 || just_matrices) build_plainshape(id, draw_li, c, siid, sidir);
    return id;
    }
  }

namespace irr {

  map<int, matrixlist> usedml;

  void generate_floorshapes() {
  
    if(irr::cells.empty()) return;

    for(auto pfsh: all_escher_floorshapes) {
      auto& fsh = *pfsh;
      generate_matrices_scale(1, fsh.noftype);
      auto& m = hept_matrices;
      
      /* if(siid == 0)
        for(auto& ma: m.v) ma.first = ma.first * pispin; */
  
      fsh.b.resize(irr::cellcount);

      for(int id=0; id<irr::cellcount; id++) {      
        auto& vs = irr::cells[id];

        int cor = isize(vs.vertices);
        m.n.sym = cor;      
        
        int i = 0;
  
        for(int d=0; d<m.o.sym; d++) {
          hyperpoint center = hpxy(0,0);
    
          for(int c=0; c<cor; c++) {
            hyperpoint nlcorner = vs.vertices[(d+c+1) % cor];
            hyperpoint nrcorner = vs.vertices[(d+c+2) % cor];
            
            hyperpoint nfar = vs.jpoints[vs.neid[(d+c+1) % cor]];
            hyperpoint nlfar = nfar;
            hyperpoint nrfar = nfar;
            m.v[i].second[c] = build_matrix(center, nlcorner, nrcorner);
            m.v[i+1].second[c] = build_matrix(nfar, nlcorner, nrcorner);
            m.v[i+2].second[c] = build_matrix(nfar, nlcorner, nlfar);
            m.v[i+3].second[c] = build_matrix(nfar, nrcorner, nrfar);
            }
          
          i += 4;
          }
          
        usedml[id] = m;
      
        m.n.sym = cor;
        bshape2(fsh.b[id], fsh.prio, fsh.shapeid2 ? fsh.shapeid2 : fsh.shapeid1, m);
        }
      }
    
    for(auto pfsh: all_plain_floorshapes) {
      auto& fsh = *pfsh;

      ld sca = fsh.rad0 / shFullFloor.rad0;
      
      fsh.b.resize(irr::cellcount);
      fsh.shadow.resize(irr::cellcount);        
      
      for(int i=0; i<irr::cellcount; i++) {      
        auto& vs = irr::cells[i];
        vector<hyperpoint> cornerlist;
        
        int cor = isize(vs.vertices);
        for(int j=0; j<cor; j++)
          cornerlist.push_back(rspintox(vs.vertices[j]) * xpush(hdist0(vs.vertices[j]) * sca) * C0);
      
        bshape(fsh.b[i], fsh.prio);
        for(int j=0; j<=cor; j++) hpcpush(cornerlist[j%cor]);
        
        bshape(fsh.shadow[i], fsh.prio);
        for(int j=0; j<=cor; j++)
          hpcpush(mid_at(hpxy(0,0), cornerlist[j%cor], SHADMUL));
        
        cell fc;
        fc.type = cor;
        irr::cellindex[&fc] = i;
        
        // printf("at = %d,%d cor = %d sca = %lf\n", li.relative.first, li.relative.second, cor, sca);
  
        for(int k=0; k<SIDEPARS; k++) 
          for(int c=0; c<cor; c++) {
            fsh.gpside[k][c].resize(irr::cellcount);
            bshape(fsh.gpside[k][c][i], fsh.prio);
            hpcpush(iddspin(&fc, c) * cornerlist[c]);
            hpcpush(iddspin(&fc, c) * cornerlist[(c+1)%cor]);
            chasmifyPoly(dlow_table[k], dhi_table[k], k);
            }
        }
      }
    
    finishshape(); last = NULL;
    extra_vertices();
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
  if(gp::on) {
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
    if(gp::on || irr::on) 
      lastptd().u.poly.flags = POLY_INVERSE;
    }
#endif
  else draw_shapevec(c, V, (qfi.fshape->*tab), col, prio);
  }

void viewmat() {
/*
  int id = 0;  
  if(gp::on) {
    gp::just_matrices = true;
    gp::draw_li = gp::get_local_info(cwt.c);
    if(gp::draw_li.last_dir == -1) gp::draw_li.total_dir = 0;
    gp::draw_li.total_dir = fix6(gp::draw_li.total_dir);
    gp::get_plainshape_id(cwt.c);
    gp::just_matrices = false;
    }
  // if(gp::on && !gp::usedml.count(cwt.c)) return;
//  for(auto& v: (pseudohept(cwt.c) ? hept_matrices : hex_matrices).v) {
//  for(auto& v: (gp::on ? gp::usedml[cwt.c] : pseudohept(cwt.c) ? hept_matrices : hex_matrices).v) {
//    hyperpoint h1 = gmatrix[cwt.c] * v.second[0] * hpxyz(1,0,0);
  id = irr::cellindex[cwt.c];
  for(auto& v: irr::usedml[id].v) { 
//  for(auto& v: (gp::on ? gp::usedml[cwt.c] : pseudohept(cwt.c) ? hept_matrices : hex_matrices).v) {
    
    hyperpoint h1 = gmatrix[cwt.c] * v.second[0] * hpxyz(1,0,0);
    hyperpoint h2 = gmatrix[cwt.c] * v.second[0] * hpxyz(0,1,0);
    hyperpoint h3 = gmatrix[cwt.c] * v.second[0] * hpxyz(0,0,1);
    queueline(h1, h2, 0xFFFFFFFF, 4, PPR_LINE);
    queueline(h2, h3, 0xFFFFFFFF, 4, PPR_LINE);
    queueline(h3, h1, 0xFFFFFFFF, 4, PPR_LINE);
    hyperpoint ch = mid3(h1, h2, h3);
    queuestr(ch, vid.fsize, its(id), 0xFFFFFF);
if(0) {
    hyperpoint h1 = gmatrix[cwt.c] * inverse(v.first) * hpxyz(1,0,0);
    hyperpoint h2 = gmatrix[cwt.c] * inverse(v.first) * hpxyz(0,1,0);
    hyperpoint h3 = gmatrix[cwt.c] * inverse(v.first) * hpxyz(0,0,1);
    queueline(h1, h2, 0xFF00FF80, 4, PPR_LINE);
    queueline(h2, h3, 0xFF00FF80, 4, PPR_LINE);
    queueline(h3, h1, 0xFF00FF80, 4, PPR_LINE);
    hyperpoint ch = mid3(h1, h2, h3); 
    queuestr(ch, vid.fsize, its(id), 0xFFFFFF);
    }
    id++;
    } */
  }

}
