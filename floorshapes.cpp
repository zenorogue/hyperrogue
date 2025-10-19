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

EX basic_textureinfo* get_floor_texture_vertices(int index) {
  if(noGUI || !vid.usingGL) return nullptr;
  return &floor_texture_vertices[index];
  }

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
  
  if(osym % rots && rots % osym && debug_geometry)
    printf("warning: rotation oddity (shapeid %d, osym=%d rots=%d)\n", shapeid, osym, rots);

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
      if(mapped == 0 && debug_geometry)
        println(hlog, "warning: not mapped shapeid = ", shapeid);
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

template<class T, class U> void sizeto(T& t, int n, const U& val) {
  if(isize(t) <= n) t.resize(n+1, val);
  }

#if CAP_BT
void geometry_information::bshape_bt(floorshape &fsh, int id, int sides, ld size, cell *c) {
  
  sizeto(fsh.b, id);
  sizeto(fsh.shadow, id);

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

  for(auto p: allsides) {
    for(int i=0; i<c->type; i++) {
      sizeto(fsh.side[p], c->type);
      sizeto(fsh.side[p][i], id);
      bshape(fsh.side[p][i][id], PPR::FLOOR_SIDE);
      hyperpoint h0 = bt::get_corner_horo_coordinates(c, i) * size;
      hyperpoint h1 = bt::get_corner_horo_coordinates(c, i+1) * size;
      hyperpoint hd = (h1 - h0) / STEP;
      for(int j=0; j<=STEP; j++)
        hpcpush(bt::get_horopoint(h0 + hd * j));
      chasmifyPoly(dlow_table[p], dhi_table[p], p);
      }
    }
  }
#endif

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

hyperpoint get_circumscribed_corner(cell *c, int t, hyperpoint h) {

  hyperpoint h0 = currentmap->adjmod(c, t) * h;
  hyperpoint h1 = currentmap->adjmod(c, t-1) * h;
  transmatrix T;
  array<hyperpoint, 3> hs = {h, h0, h1};
  set_column(T, 3, C03);
  hyperpoint res = C03;
  for(int i=0; i<3; i++) {
    hyperpoint ahs = hs[i];
    if(hyperbolic) ahs[3] *= -1;
    set_column(T, i, ahs);
    res[i] = dot_d(4, hs[i], ahs);
    }
  T = transpose(T);
  return inverse(T) * res;
  }

EX hookset<bool(cell*, int)> hooks_floorshapes_for;

// !siid equals pseudohept(c)

void geometry_information::generate_floorshapes_for(int id, cell *c) {

  if(callhandlers(false, hooks_floorshapes_for, c, id)) return;

  int siid = 0, sidir = 0;

  if(0) ;

  #if CAP_ARCM
  else if(arcm::in()) {
    if(BITRUNCATED)
      siid = arcm::pseudohept(c), sidir = arcm::pseudohept(c) ? 0 : !arcm::pseudohept(c->cmove(0));
    else if(geosupport_football() == 2)
      siid = arcm::pseudohept(c), sidir = !arcm::pseudohept(c->cmove(0));
    else
      siid = 1, sidir = 0;
    }
  #endif

  #if CAP_IRR
  else if(IRREGULAR) {
    DEBBI(debug_poly, ("generate_floorshapes: irregular"));

    auto& vs = irr::cells[id];
    siid = vs.is_pseudohept;
    sidir = 0;
    if(siid) sidir = irr::cells[vs.neid[0]].is_pseudohept;
    }
  #endif
    
  else if(arb::in()) {
    auto& c = arb::current;
    siid = c.shapes[id].football_type >= 2;
    sidir = c.shapes[id].football_type == 1;
    if(!c.is_football_colorable) siid = 1;
    }
  
  else if(geometry == gBinary4) {
    siid = 1;
    }

  else if(geometry == gTernary) {
    siid = 1;
    }

  else if(GOLDBERG_INV) {
    auto data = cgi.gpdata;
    if(fake::in()) data = FPIU(cgi.gpdata);
    siid = data->id_to_params[id][0] == 1;
    sidir = data->id_to_params[id][1];
    }

  else if(PURE && geometry != gBinaryTiling && geosupport_football() < 2) {
    siid = 1;
    }

  else if(PURE && !(S7&1) && !aperiodic && !a4) {
    siid = id & 1; sidir = id >> 1;
    }

  else if(BITRUNCATED) {
    siid = ishept(c); sidir = 0;
    }

  #if CAP_BT
  else if(bt::in()) {
    siid = id;
    }
  #endif

  DEBBI(debug_poly, ("generate_floorshapes_for ", id));

  for(auto pfsh: all_plain_floorshapes) {
    auto& fsh = *pfsh;

    #if CAP_BT
    if(bt::in()) {
      bshape_bt(fsh, id, S7, fsh.rad1, c);
      continue;
      }
    #endif

    // special
    ld sca = 3 * shFullFloor.rad0 / fsh.rad0;

    vector<hyperpoint> cornerlist;
    
    int cor = c->type;
    bool apeirogonal = false;

    if(&fsh == &shTriheptaFloor) {
      if(siid) {
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
      if(siid) {
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
      if(siid) {
        for(int i=0; i<cor; i++) {
          hyperpoint nc = nearcorner(c, i);
          cornerlist.push_back(mid_at(hpxy(0,0), nc, val));
          }
        }
      else {
        for(int i=0; i<cor; i++) cornerlist.push_back(hpxy(0,0));
        }
      }
    
    else if(arb::in() || aperiodic || arcm::in() || IRREGULAR) {
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

      ld dist = min_dist * (1 - 3 / sca) * (arb::in() ? ac.boundary_ratio : 1);
      ld area = 0;
      for(int j=0; j<cor; j++) {
        hyperpoint current = kleinize(actual[j]);
        hyperpoint last = kleinize(atmod(actual, j-1));
        area += current[0] * last[1] - last[0] * current[1];
        }
      if(area < 0) dist = -dist;

      apeirogonal = false;
      int id = 0;
      arb::shape *sh = nullptr;
      if(arb::in()) {
        id = arb::id_of(c->master);
        sh = &ac.shapes[id];
        apeirogonal = sh->apeirogonal;
        }

      for(int j=0; j<cor; j++) {
        hyperpoint last = atmod(actual, j-1);
        hyperpoint current = ypush(1e-7) * xpush(1e-6) * actual[j];
        hyperpoint next = atmod(actual, j+1);

        if(apeirogonal) {
          if(j == 0) last = arb::get_adj(ac, id, cor-1, id, cor-2, false) * actual[cor-3];
          if(j == cor-2) next = arb::get_adj(ac, id, cor-2, id, cor-1, false) * actual[1];
          if(j == cor-1) { cornerlist.push_back(sh->vertices.back()); continue; }
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
    
    for(auto p: allsides) {
      if(isize(fsh.side[p]) < cor)
        fsh.side[p].resize(cor);
      for(int cid=0; cid<cor; cid++) {
        sizeto(fsh.side[p][cid], id);
        bshape(fsh.side[p][cid][id], fsh.prio);
        hpcpush(cornerlist[cid]);
        hpcpush(cornerlist[(cid+1)%cor]);
        chasmifyPoly(dlow_table[p], dhi_table[p], p);
        }
      }
    }
  
  for(auto pfsh: all_escher_floorshapes) {
  
    auto& fsh = *pfsh;
    
    sizeto(fsh.b, id);
    sizeto(fsh.shadow, id);
    
    if(STDVAR && standard_tiling()) {
      for(auto ii: {0, 1}) {
        auto& tgt = (ii ? fsh.shadow : fsh.b)[id];
        generate_matrices_scale(fsh.scale, fsh.noftype * (ii ? SHADMUL : 1));
        if(sidir & 1)
          for(auto& m: hex_matrices.v) for(auto& m2: m.second) m2 = spin180() * m2;
        if(PURE && geosupport_football() < 2) {
          bshape2(tgt, fsh.prio, fsh.shapeid2 ? fsh.shapeid2 : fsh.shapeid1, hept_matrices);
          }
        else {
          if(siid == 0) bshape2(tgt, fsh.prio, fsh.shapeid0, hex_matrices);
          if(siid == 1) bshape2(tgt, fsh.prio, fsh.shapeid1, hept_matrices);
          }
        }
      }
    
    else {
      generate_matrices_scale(fsh.scale, fsh.noftype);

      auto& m = (!siid && geosupport_football() == 2) ? hex_matrices : hept_matrices;
      
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
        bshape2((ii?fsh.shadow:fsh.b)[id], fsh.prio, (fsh.shapeid2 && geosupport_football() < 2) ? fsh.shapeid2 : siid?fsh.shapeid1:fsh.shapeid0, m);

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

      for(auto p: allsides) {
        sizeto(fsh.levels[p], id);
        bshape(fsh.levels[p][id], fsh.prio);
        last->flags |= POLY_TRIANGLES;
        last->tinf = get_floor_texture_vertices(fsh.id);
        last->texture_offset = 0;

        if(1) {
          int s = fsh.b[id].s;
          int e = fsh.b[id].e-1;

          if(vid.pseudohedral == phInscribed) {
            hyperpoint ctr = Hypc;
            for(int t=0; t<e-s; t++)
              ctr += kleinize(cgi.emb->orthogonal_move(hpc[s+t], dhi_table[p]));
            ctr = normalize(ctr);

            for(int t=0; t<e-s; t++) {
              hyperpoint v1 = kleinize(cgi.emb->orthogonal_move(hpc[s+t], dhi_table[p])) - ctr;
              hyperpoint v2 = kleinize(cgi.emb->orthogonal_move(hpc[s+t+1], dhi_table[p])) - ctr;
              texture_order([&] (ld x, ld y) {
                hpcpush(normalize(ctr + v1 * x + v2 * y));
                });
              }
            }
          if(vid.pseudohedral == phCircumscribed) {

            vector<hyperpoint> hs(c->type);
            hyperpoint z = Hypc; z[2] = dhi_table[p];
            hyperpoint ctr = cgi.emb->logical_to_actual(z);
            for(int t=0; t<c->type; t++) hs[t] = get_circumscribed_corner(c, t, ctr);
            // for(int t=0; t<c->type; t++) hs[t] = xspinpush0(t * TAU / c->type, 0.2); //  kleinize(get_circumscribed_corner(c, t, ctr));

            for(int t=0; t<c->type; t++) {
              hyperpoint v1 = hs[t] - ctr;
              hyperpoint v2 = atmod(hs, t+1) - ctr;
              texture_order([&] (ld x, ld y) {
                hpcpush(normalize(ctr + v1 * x + v2 * y));
                });
              }
            /* also affect the plain floor */
            bshape(fsh.b[id], PPR::FLOOR);
            for(hyperpoint& h: hs) hpcpush(h);
            hpcpush(hs[0]);
            }
          if(vid.pseudohedral == phOFF) for(int t=0; t<e-s; t++) {

            hyperpoint v1 = cgi.emb->actual_to_logical(hpc[s+t]);
            hyperpoint v2 = cgi.emb->actual_to_logical(hpc[s+t+1]);

            texture_order([&] (ld x, ld y) { 
              hyperpoint a = v1 * x + v2 * y;
              a[2] = dhi_table[p];
              auto c = cgi.emb->logical_to_actual(a);
              cgi.hpcpush(c);
              });
            }
          }

        finishshape();
        ensure_vertex_number(fsh.levels[p][id]);
        }
      
      for(int co=0; co<2; co++) {
        sizeto(fsh.cone[co], id);
        bshape(fsh.cone[co][id], fsh.prio);    
        last->flags |= POLY_TRIANGLES;
        last->tinf = get_floor_texture_vertices(fsh.id);
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

      for(auto p: allsides) {
        sizeto(fsh.side[p], c->type);
        for(auto& gs: fsh.side[p]) {
          for(auto& li: gs) 
            bind_floor_texture(li, fsh.id);
          }
        }
      }
      
    for(auto pfsh: all_escher_floorshapes) {
      auto& fsh = *pfsh;
      
      for(auto p: allsides) {
        fsh.levels[p] = shFullFloor.levels[p];
        fsh.shadow = shFullFloor.shadow;
        for(auto& li: fsh.levels[p]) bind_floor_texture(li, fsh.id);
        sizeto(fsh.side[p], c->type);
        for(int e=0; e<c->type; e++) {
          fsh.side[p][e] = shFullFloor.side[p][e];
          for(auto& li: fsh.side[p][e])
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

#if CAP_GP
EX namespace gp {
  
  EX void clear_plainshapes() {
    for(int m=0; m<3; m++)
    for(int sd=0; sd<8; sd++)
    for(int i=0; i<GOLDBERG_LIMIT; i++)
    for(int j=0; j<GOLDBERG_LIMIT; j++)
    for(int k=0; k<8; k++)
      cgi.gpdata->pshid[m][sd][i][j][k] = -1;
    }

  EX array<int, 5> get_plainshape_data(cell *c) {
    if(li_for != c) {
      li_for = c;
      current_li = get_local_info(c);
      }
    int siid, sidir;
    // note: this siid can equal 2! it should be treated as 0 by floorshapes
    cell *c1 = c;
    auto f = [&] {
      if(geosupport_threecolor() == 2) {
        auto si = patterns::getpatterninfo(c1, patterns::PAT_COLORING, patterns::SPF_NO_SUBCODES);
        siid = si.id>>2; siid++; if(siid == 3) siid = 0;
        if((si.id>>2) == 1) si.dir++;
        sidir = c1->c.fix(si.dir);
        }
      else if(geosupport_football() == 2) {
        siid = pseudohept(c1);
        sidir = ishex1(c1);
        }
      else if(geosupport_chessboard()) {
        siid = !chessvalue(c1);
        sidir = 0;
        }
      else {
        siid = 1;
        sidir = 0;
        }
      };
    if(INVERSE && gp::variation_for(gp::param) == eVariation::goldberg) {
      c1 = gp::get_mapped(c);
      UIU(f());
      siid = 1; sidir = 0;
      }
    else if(INVERSE) {
      siid = 1;
      sidir = 0;
      }
    else f();
    array<int, 5> res;
    res[0] = siid; res[1] = sidir;
    res[2] = current_li.relative.first&GOLDBERG_MASK; res[3] = current_li.relative.second&GOLDBERG_MASK;
    res[4] = gmod(current_li.total_dir, S6);
    return res;
    }

  EX int get_plainshape_id(cell *c) {
    auto res = get_plainshape_data(c);
    auto& id = cgi.gpdata->pshid[res[0]][res[1]][res[2]][res[3]][res[4]];
    if(id == -1 && sphere && isize(cgi.shFloor.b) > 0) {
      forCellEx(c1, c) if(!gmatrix0.count(c1)) return 0;
      }
    if(id == -1) {
      id = cgi.gpdata->id_to_params.size();
      cgi.gpdata->id_to_params.emplace_back(res);
      }
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

EX void ensure_floorshape_generated(int id, cell *c) {
  hpcshape nul; nul.s = -1;
  sizeto(cgi.shFloor.b, id, nul);
  if(cgi.shFloor.b[id].s == -1) {
    cgi.require_shapes();
    cgi.generate_floorshapes_for(id, c);
    cgi.finishshape();
    cgi.extra_vertices();
    }
  }

EX int shvid(cell *c) {
  return currentmap->shvid(c);
  }

int hrmap_standard::shvid(cell *c) {
  if(GOLDBERG || (INVERSE && fake::in()))
    return gp::get_plainshape_id(c);
  #if CAP_IRR
  else if(IRREGULAR)
    return irr::cellindex[c];
  #endif
  else if(PURE && !(S7&1) && !aperiodic && !a4) {
    auto si = patterns::getpatterninfo(c, patterns::PAT_COLORING, 0);
    if(si.id == 8) si.dir++;
    return (pseudohept(c) ? 1 : 0) + (si.dir&1) * 2;
    }
  else if(geosupport_football() == 2)
    return pseudohept(c);
  else if(inforder::mixed()) return c->type;
  else if(PURE)
    return 0;
  else
    return ctof(c);
  }

EX struct dqi_poly *draw_shapevec(cell *c, const shiftmatrix& V, const vector<hpcshape> &shv, color_t col, PPR prio IS(PPR::DEFAULT)) {
  if(no_wall_rendering) return NULL;
  if(!c) return &queuepolyat(V, shv[0], col, prio);
  if(WDIM == 3) return NULL;
  int id = shvid(c);
  ensure_floorshape_generated(id, c);
  if(id < 0) return NULL;
  if(id >= isize(shv)) return NULL;
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

struct texture_params {
  ld escher_strength;
  ld escher_strength2;
  ld escher_width;
  ld grid_strength;
  int grid_brightness;
  ld grid_width;
  int scratches_random, scratches_ortho, scratches_parallel, scratches_radial, scratches_around;
  int scratch_seed;
  ld scratch_width, scratch_length;
  int scratch_alpha, scratch_bright;
  };

texture_params tparams;

void reset_floor_textures() {
  if(floor_textures) delete floor_textures;
  floor_textures = NULL;
  }

EX void add_texture_params() {
  auto& tp = tparams;
  param_f(tp.escher_strength, "texture_escher_strength", 2.4)
  ->editable(0, 10, 0.1, "strength of the Escher texture (inside)", "", 'i')
  ->set_reaction(reset_floor_textures);
  param_f(tp.escher_strength2, "texture_escher_strength2", 1.5)
  ->editable(0, 10, 0.1, "strength of the Escher texture (boundary)", "", 'b')
  ->set_reaction(reset_floor_textures);
  param_f(tp.escher_width, "texture_escher_width", 1)
  ->editable(0, 10, 0.1, "strength of the Escher texture (boundary width)", "", 'w')
  ->set_reaction(reset_floor_textures);
  param_f(tp.grid_strength, "grid_strength", 19.2)
  ->editable(0, 50, 1, "grid strength", "", 'a')
  ->set_reaction(reset_floor_textures);
  param_i(tp.grid_brightness, "grid_brightness", 0x40)
  ->editable(0, 255, 16, "grid darkness", "", 'd')
  ->set_reaction(reset_floor_textures);
  param_f(tp.grid_width, "grid_width", 8)
  ->editable(0, 16, 1, "grid width", "", 'g')
  ->set_reaction(reset_floor_textures);
  param_i(tp.scratches_random, "texture_scratches_random", ISMOBILE ? 10 : 1000)
  ->editable(0, 10000, 0.1, "the number of random scratches", "", 'n')
  ->set_sets(dialog::scaleSinh)
  ->set_reaction(reset_floor_textures);
  param_i(tp.scratches_ortho, "texture_scratches_ortho", 0)
  ->editable(0, 10000, 0.1, "the number of orthogonal scratches", "", 'o')
  ->set_sets(dialog::scaleSinh)
  ->set_reaction(reset_floor_textures);
  param_i(tp.scratches_parallel, "texture_scratches_para", 0)
  ->editable(0, 10000, 0.1, "the number of parallel scratches", "", 'p')
  ->set_sets(dialog::scaleSinh)
  ->set_reaction(reset_floor_textures);
  param_i(tp.scratches_radial, "texture_scratches_radial", 0)
  ->editable(0, 10000, 0.1, "the number of radial scratches", "", 'r')
  ->set_sets(dialog::scaleSinh)
  ->set_reaction(reset_floor_textures);
  param_i(tp.scratches_around, "texture_scratches_around", 0)
  ->editable(0, 10000, 0.1, "the number of scratches around", "", 'c')
  ->set_sets(dialog::scaleSinh)
  ->set_reaction(reset_floor_textures);
  param_i(tp.scratch_seed, "texture_scratch_seed", 0)
  ->editable(0, 1000, 1, "the seed for scratch generation", "", 's')
  ->set_reaction(reset_floor_textures);
  param_f(tp.scratch_width, "texture_scratch_width", 1)
  ->editable(0, 16, 1, "scratch width", "", 'h')
  ->set_reaction(reset_floor_textures);
  param_f(tp.scratch_length, "texture_scratch_length", 0.1)
  ->editable(0, 5, 0.1, "scratch length", "", 'l')
  ->set_reaction(reset_floor_textures);
  param_i(tp.scratch_alpha, "texture_scratch_alpha", 16)
  ->editable(0, 255, 16, "scratch alpha", "", 'c')
  ->set_reaction(reset_floor_textures);
  param_i(tp.scratch_bright, "texture_scratch_bright", 16)
  ->editable(0, 255, 16, "scratch brightness", "", 'f')
  ->set_reaction(reset_floor_textures);
  }

EX void edit_texture_params() {
  cmode = sm::SIDE;
  gamescreen();
  dialog::init(XLAT("wall/floor texture settings"));
  auto& tp = tparams;
  add_edit(tp.escher_strength);
  add_edit(tp.escher_strength2);
  add_edit(tp.escher_width);

  add_edit(tp.grid_strength);
  add_edit(tp.grid_brightness);
  add_edit(tp.grid_width);

  add_edit(tp.scratches_random);
  add_edit(tp.scratches_ortho);
  add_edit(tp.scratches_parallel);
  add_edit(tp.scratches_radial);
  add_edit(tp.scratches_around);
  add_edit(tp.scratch_seed);
  add_edit(tp.scratch_width);
  add_edit(tp.scratch_length);
  add_edit(tp.scratch_alpha);
  add_edit(tp.scratch_bright);
  dialog::display();
  }

void draw_shape_for_texture(floorshape* sh) {

  auto& tp = tparams;

  int id = sh->id;
  
  const ld s1 = 1;
  const ld s3 = 3 * s1;
  const ld sd = s1/2;

  ld gx = (id % 8) * s3 - 3.5 * s3;
  ld gy = (id / 8) * s3 - 3.5 * s3;

  auto brightalpha = [] (int bright, int alpha) {
    if(bright > 255) bright = 255;
    if(bright < 0) bright = 0;
    if(alpha > 255) alpha = 255;
    if(alpha < 0) alpha = 0;
    return bright * 0x1010100 + alpha;
    };

  dynamicval<ld> v(vid.linewidth, vid.linewidth);

  if(1) {
    curvepoint(eupush(gx+s1, gy-s1) * C0);
    curvepoint(eupush(gx+s1, gy+s1) * C0);
    curvepoint(eupush(gx-s1, gy+s1) * C0);
    curvepoint(eupush(gx-s1, gy-s1) * C0);
    curvepoint(eupush(gx+s1, gy-s1) * C0);

    queuecurve(shiftless(Id), 0x000000FF, brightalpha(255 - sh->pstrength * tp.escher_strength, 255), PPR::WATERLEVEL_TOP);
    }

  vid.linewidth = tp.escher_width;
  poly_outline = brightalpha(255 - sh->pstrength * tp.escher_strength2, 255);

  for(int a=-1; a<=1; a++)
  for(int b=-1; b<=1; b++)
    queuepoly(shiftless(eupush(gx+a, gy+b)), sh->b[0], 0xFFFFFFFF);

  vid.linewidth = 0;
  if(sh == &cgi.shCrossFloor) {
    queuepoly(shiftless(eupush(gx, gy) * spin(45._deg)), cgi.shCross, 0x808080FF);
    }

  if(1) {
    vid.linewidth = tp.grid_width;
    curvepoint(eupush(gx+sd, gy-sd) * C0);
    curvepoint(eupush(gx+sd, gy+sd) * C0);
    curvepoint(eupush(gx-sd, gy+sd) * C0);
    curvepoint(eupush(gx-sd, gy-sd) * C0);
    curvepoint(eupush(gx+sd, gy-sd) * C0);
    queuecurve(shiftless(Id), brightalpha(tp.grid_brightness, sh->fstrength * tp.grid_strength), 0, PPR::LINE);
    }

  std::mt19937 scratchgen(tp.scratch_seed);

  auto srandd = [&] () { return randf_from(scratchgen); };

  vid.linewidth = tp.scratch_width;
  auto scratcher = [&] (int qty, hr::function<void(hyperpoint&, hyperpoint&)> f) {
    for(int i=0; i<qty; i++) {
      hyperpoint h1 = hpxy(sd * (6*srandd()-3), sd * (6*srandd()-3));
      hyperpoint h2 = hpxy(sd * (6*srandd()-3), sd * (6*srandd()-3));
      f(h1, h2);
      ld d = hdist(h1, h2);
      hyperpoint h3 = h1 + (h2-h1) /d * min(d, tp.scratch_length);
      for(int a=0; a<4; a++) {
        curvepoint(eupush(gx,gy) * eupush(spin(90._deg*a) * h1) * C0);
        curvepoint(eupush(gx,gy) * eupush(spin(90._deg*a) * h3) * C0);
        queuecurve(shiftless(Id), brightalpha(tp.scratch_bright, tp.scratch_alpha), 0, PPR::LINE);
        }
      }
    };

  scratcher(tp.scratches_random, [] (hyperpoint& h1, hyperpoint& h2) { });
  scratcher(tp.scratches_ortho, [] (hyperpoint& h1, hyperpoint& h2) { if(abs(h1[0]) > abs(h1[1])) h2[1] = h1[1]; else h2[0] = h1[0]; });
  scratcher(tp.scratches_parallel, [] (hyperpoint& h1, hyperpoint& h2) { if(abs(h1[0]) < abs(h1[1])) h2[1] = h1[1]; else h2[0] = h1[0]; });
  scratcher(tp.scratches_radial, [] (hyperpoint& h1, hyperpoint& h2) { h2 = C0; });
  scratcher(tp.scratches_around, [] (hyperpoint& h1, hyperpoint& h2) { h2 = h1 + hyperpoint(h1[1], -h1[0], 0, 0); });

  auto ftv = get_floor_texture_vertices(sh->id);
  if(ftv) {
    ftv->tvertices.clear();
    ftv->texture_id = floor_textures->renderedTexture;
    }
  
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
  if(ftv) texture_order([&] (ld x, ld y) {
    auto v = tvec_at(x, y);
    ftv->tvertices.push_back(glhr::makevertex(v[0], v[1], 0));
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
  if(!sh.tinf) return;
  ensure_vertex_number(*sh.tinf, sh.e - sh.s);
  }

EX void bind_floor_texture(hpcshape& li, int id) {
  li.tinf = get_floor_texture_vertices(id);
  ensure_vertex_number(li);
  }

#if HDR
const int FLOORTEXTURESIZE = 4096;
#endif

void geometry_information::make_floor_textures_here() {
  require_shapes();
  auto m = euc::new_map();
  dynamicval<hrmap*> dm(currentmap, m);
  cgi.generate_floorshapes_for(0, m->gamestart());
  cgi.finishshape(); cgi.extra_vertices();

  dynamicval<videopar> vi(vid, vid);
  vid.xres = FLOORTEXTURESIZE;
  vid.yres = FLOORTEXTURESIZE;
  pconf.scale = 0.125;
  dynamicval<transmatrix> vm(pconf.cam(), Id);
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
  DEBBI(debug_poly, ("make_floor_textures"));
  dynamicval<euc::torus_config_full> geu(euc::eu, euc::eu);
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
