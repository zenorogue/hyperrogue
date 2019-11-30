// Hyperbolic Rogue -- nonisotropic spaces (Solv and Nil)
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file nonisotropic.cpp
 *  \brief nonisotropic spaces (Solv and Nil)
 */

#include "hyper.h"
namespace hr {

EX namespace nisot {
  #if HDR
  typedef array<float, 3> ptlow;
  #endif

  #if HDR
  inline bool local_perspective_used() { return nonisotropic || prod; }
  #endif
  
  EX bool geodesic_movement = true;

  EX transmatrix translate(hyperpoint h) {
    if(sl2)
      return slr::translate(h);
    transmatrix T = Id;
    for(int i=0; i<GDIM; i++) T[i][LDIM] = h[i];
    if(sol && nih) {
      T[0][0] = pow(2, -h[2]);
      T[1][1] = pow(3, h[2]);
      }
    else if(sol) {
      T[0][0] = exp(-h[2]);
      T[1][1] = exp(+h[2]);
      }
    else if(nih) {
      T[0][0] = pow(2, h[2]);
      T[1][1] = pow(3, h[2]);
      }
    if(nil) 
      T[2][1] = h[0];
    return T;
    }

  EX }

#if CAP_SOLV
EX namespace solnihv {

  EX eGeometry geom() {
    if(asonov::in()) return gSol;
    else return geometry;
    }

  #if HDR
  struct tabled_inverses {
    int PRECX, PRECY, PRECZ;
    vector<nisot::ptlow> tab;
    string fname;
    bool loaded;
    
    void load();
    hyperpoint get(ld ix, ld iy, ld iz, bool lazy);
    
    nisot::ptlow& get_int(int ix, int iy, int iz) { return tab[(iz*PRECY+iy)*PRECX+ix]; }
  
    GLuint texture_id;
    bool toload;
    
    GLuint get_texture_id();
  
    tabled_inverses(string s) : fname(s), texture_id(0), toload(true) {}  
    };
  #endif
  
  void tabled_inverses::load() {
    if(loaded) return;
    FILE *f = fopen(fname.c_str(), "rb");
    // if(!f) f = fopen("/usr/lib/soltable.dat", "rb");
    if(!f) { addMessage(XLAT("geodesic table missing")); pmodel = mdPerspective; return; }
    ignore(fread(&PRECX, 4, 1, f));
    ignore(fread(&PRECY, 4, 1, f));
    ignore(fread(&PRECZ, 4, 1, f));
    tab.resize(PRECX * PRECY * PRECZ);
    ignore(fread(&tab[0], sizeof(nisot::ptlow) * PRECX * PRECY * PRECZ, 1, f));
    fclose(f);
    loaded = true;    
    }
  
  hyperpoint tabled_inverses::get(ld ix, ld iy, ld iz, bool lazy) {
    ix *= PRECX-1;
    iy *= PRECY-1;
    iz *= PRECZ-1;
    
    hyperpoint res;
    
    if(lazy) {
      auto r = get_int(int(ix), int(iy), int(iz));
      for(int i=0; i<3; i++) res[i] = r[i];
      }
    
    else {
  
      if(ix >= PRECX-1) ix = PRECX-2;
      if(iy >= PRECX-1) iy = PRECX-2;
      if(iz >= PRECZ-1) iz = PRECZ-2;
      
      int ax = ix, bx = ax+1;
      int ay = iy, by = ay+1;
      int az = iz, bz = az+1;
      
      #define S0(x,y,z) get_int(x, y, z)[t]
      #define S1(x,y) (S0(x,y,az) * (bz-iz) + S0(x,y,bz) * (iz-az))
      #define S2(x) (S1(x,ay) * (by-iy) + S1(x,by) * (iy-ay))
  
      for(int t=0; t<3; t++)
        res[t] = S2(ax) * (bx-ix) + S2(bx) * (ix-ax);
      
      res[3] = 0;
      }
    
    return res;
    }
  
  GLuint tabled_inverses::get_texture_id() {
    if(!toload) return texture_id;
  
    load();
    if(!loaded) return 0;
  
    println(hlog, "installing table");
    toload = false;
    
    if(texture_id == 0) glGenTextures(1, &texture_id);
  
    glBindTexture( GL_TEXTURE_3D, texture_id);
  
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    auto xbuffer = new glvertex[PRECZ*PRECY*PRECX];
    
    for(int z=0; z<PRECZ*PRECY*PRECX; z++) {
      auto& t = tab[z];
      xbuffer[z] = glhr::makevertex(t[0], t[1], t[2]);
      }
    
    #if !ISWEB
    glTexImage3D(GL_TEXTURE_3D, 0, 34836 /*GL_RGBA32F*/, PRECX, PRECX, PRECZ, 0, GL_RGBA, GL_FLOAT, xbuffer);
    #else
    // glTexStorage3D(GL_TEXTURE_3D, 1, 34836 /*GL_RGBA32F*/, PRECX, PRECX, PRECZ);
    // glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, PRECX, PRECY, PRECZ, GL_RGBA, GL_FLOAT, xbuffer);
    #endif
    delete[] xbuffer;    
    return texture_id;
    }
  
  EX ld x_to_ix(ld u) {
    if(u == 0.) return 0.;
    ld diag = u*u/2.;
 
    ld x = diag;
    ld y = u;
    ld z = diag+1.;
  
    x /= (1.+z);
    y /= (1.+z);
  
    return 0.5 - atan((0.5-x) / y) / M_PI;
    }

  struct hrmap_solnih : hrmap {
    hrmap *binary_map;
    hrmap *ternary_map; /* nih only */
    unordered_map<pair<heptagon*, heptagon*>, heptagon*> at;
    unordered_map<heptagon*, pair<heptagon*, heptagon*>> coords;
    
    heptagon *origin;
    
    heptagon *getOrigin() override { return origin; }
    
    heptagon *get_at(heptagon *x, heptagon *y) {
      auto& h = at[make_pair(x, y)];
      if(h) return h;
      h = tailored_alloc<heptagon> (S7);
      h->c7 = newCell(S7, h);
      coords[h] = make_pair(x, y);
      h->distance = x->distance;
      h->dm4 = 0;
      h->zebraval = x->emeraldval;
      h->emeraldval = y->emeraldval;
      h->fieldval = 0;
      h->cdata = NULL;
      h->alt = NULL;
      return h;      
      }

    hrmap_solnih() {
    
      heptagon *alt;
      heptagon *alt3;
    
      if(true) {
        dynamicval<eGeometry> g(geometry, gBinary4); 
        alt = tailored_alloc<heptagon> (S7);
        alt->s = hsOrigin;
        alt->alt = alt;
        alt->cdata = NULL;
        alt->c7 = NULL;
        alt->zebraval = 0;
        alt->distance = 0;
        alt->emeraldval = 0;
        binary_map = binary::new_alt_map(alt);
        }
      
      if(nih) {
        dynamicval<eGeometry> g(geometry, gTernary); 
        alt3 = tailored_alloc<heptagon> (S7);
        alt3->s = hsOrigin;
        alt3->alt = alt3;
        alt3->cdata = NULL;
        alt3->c7 = NULL;
        alt3->zebraval = 0;
        alt3->distance = 0;
        alt3->emeraldval = 0;
        ternary_map = binary::new_alt_map(alt3);
        }
      else {
        alt3 = alt;
        ternary_map = nullptr;
        }
      
      origin = get_at(alt, alt3);
      }
    
    heptagon *altstep(heptagon *h, int d) {
      dynamicval<eGeometry> g(geometry, gBinary4); 
      dynamicval<hrmap*> cm(currentmap, binary_map);
      return h->cmove(d);
      }
    
    heptagon *altstep3(heptagon *h, int d) {
      dynamicval<eGeometry> g(geometry, gTernary); 
      dynamicval<hrmap*> cm(currentmap, ternary_map);
      return h->cmove(d);
      }
    
    heptagon *create_step(heptagon *parent, int d) override {
      auto p = coords[parent];
      auto pf = p.first, ps = p.second;
      auto rule = [&] (heptagon *c1, heptagon *c2, int d1) {
        auto g = get_at(c1, c2);
        parent->c.connect(d, g, d1, false);
        return g;
        };
      
      switch(geometry){

      case gSol: switch(d) {
        case 0: // right
          return rule(altstep(pf, 2), ps, 4);
        case 1: // up
          return rule(pf, altstep(ps, 2), 5);
        case 2: // front left
          return rule(altstep(pf, 0), altstep(ps, 3), ps->zebraval ? 7 : 6);
        case 3: // front right
          return rule(altstep(pf, 1), altstep(ps, 3), ps->zebraval ? 7 : 6);
        case 4: // left
          return rule(altstep(pf, 4), ps, 0);
        case 5: // down
          return rule(pf, altstep(ps, 4), 1);
        case 6: // back down
          return rule(altstep(pf, 3), altstep(ps, 0), pf->zebraval ? 3 : 2);
        case 7: // back up
          return rule(altstep(pf, 3), altstep(ps, 1), pf->zebraval ? 3 : 2);
        default:
          return NULL;
        }

      case gNIH: switch(d) {
        case 0: // right
          return rule(altstep(pf, 2), ps, 2);
        case 1: // up
          return rule(pf, altstep3(ps, 3), 3);
        case 2: // left
          return rule(altstep(pf, 4), ps, 0);
        case 3: // down
          return rule(pf, altstep3(ps, 5), 1);
        case 4: // back
          return rule(altstep(pf, 3), altstep3(ps, 4), 5 + pf->zebraval + 2 * ps->zebraval);
        default:
          return rule(altstep(pf, (d-5) % 2), altstep3(ps, (d-5)/2), 4);
        }

      case gSolN: switch(d) {
        case 0: // right
          return rule(altstep(pf, 2), ps, 2);
        case 1: // up
          return rule(pf, altstep3(ps, 3), 3);
        case 2: // left
          return rule(altstep(pf, 4), ps, 0);
        case 3: // down
          return rule(pf, altstep3(ps, 5), 1);
        case 4: case 5: 
          return rule(altstep(pf, d-4), altstep3(ps, 4), ps->zebraval + 6);
        case 6: case 7: case 8: 
          return rule(altstep(pf, 3), altstep3(ps, d-6), pf->zebraval + 4);
        default:
          return NULL;
        }

        default: throw "not solnihv";
        }
      }

    ~hrmap_solnih() {
      delete binary_map;
      if(ternary_map) delete ternary_map;
      for(auto& p: at) clear_heptagon(p.second);
      }

    transmatrix adjmatrix(int i, int j) {
      switch(geometry) {
      case gSol: {
        ld z = log(2);
        ld bw = vid.binary_width * z;
        switch(i) {
          case 0: return xpush(+bw);
          case 1: return ypush(+bw);
          case 2: case 3:
            return ypush(bw*(6.5-j)) * zpush(+z) * xpush(bw*(i-2.5));
          case 4: return xpush(-bw);
          case 5: return ypush(-bw);
          case 6: case 7:
            return xpush(bw*(2.5-j)) * zpush(-z) * ypush(bw*(i-6.5));
          default:return Id;
          }
        }
      case gNIH: {
        ld bw = vid.binary_width;
        switch(i) {          
          case 0: return xpush(+bw);
          case 1: return ypush(+bw);
          case 2: return xpush(-bw);
          case 3: return ypush(-bw);
          case 4: return xpush(-((j-5)%2-.5)*bw) * ypush(-((j-5)/2-1)*bw) * zpush(1);
          default:
            return zpush(-1) * xpush(((i-5)%2-.5)*bw) * ypush(((i-5)/2-1)*bw);
          }
        }
      case gSolN: {
        ld bw = vid.binary_width;
        switch(i) {          
          case 0: return xpush(+bw);
          case 1: return ypush(+bw);
          case 2: return xpush(-bw);
          case 3: return ypush(-bw);
          case 4: 
          case 5:
            return ypush(bw*(7-j)) * zpush(+1) * xpush(bw*(i-4.5));
          case 6:
          case 7:
          case 8:
            return xpush(bw*(4.5-j)) * zpush(-1) * ypush(bw*(i-7));
          }
        }

      default: throw "not nihsolv";
      }
      }

    transmatrix adj(heptagon *h, int d) override {
      h->cmove(d); return adjmatrix(d, h->c.spin(d));
      }
    
    virtual transmatrix relative_matrix(heptagon *h2, heptagon *h1, const hyperpoint& hint) override { 
      for(int i=0; i<h1->type; i++) if(h1->move(i) == h2) return adjmatrix(i, h1->c.spin(i));
      if(gmatrix0.count(h2->c7) && gmatrix0.count(h1->c7))
        return inverse(gmatrix0[h1->c7]) * gmatrix0[h2->c7];
      
      transmatrix front = Id, back = Id;
      
      int up, down;

      switch(geometry) {
        case gSol: up = 2; down = 6; break;
        case gSolN: up = 4; down = 7; break;
        case gNIH: up = 4; down = 4; break;
        default: throw "not nihsolv";
        }
      
      while(h1->distance > h2->distance) front = front * adj(h1, down), h1 = h1->cmove(down);
      while(h1->distance < h2->distance) back = iadj(h2, down) * back, h2 = h2->cmove(down);
      while(coords[h1].first != coords[h2].first) front = front * adj(h1, down), back = iadj(h2, down) * back, h1 = h1->cmove(down), h2 = h2->cmove(down);
      while(coords[h1].second != coords[h2].second) front = front * adj(h1, up), back = iadj(h2, up) * back, h1 = h1->cmove(up), h2 = h2->cmove(up);
      return front * back;
      }

    void draw() override {
      dq::visited.clear();

      dq::enqueue(centerover->master, cview());
      
      while(!dq::drawqueue.empty()) {
        auto& p = dq::drawqueue.front();
        heptagon *h = get<0>(p);
        transmatrix V = get<1>(p);
        dq::drawqueue.pop();
        
        cell *c = h->c7;
        if(!do_draw(c, V)) continue;
        drawcell(c, V);
        if(wallopt && isWall3(c) && isize(dq::drawqueue) > 1000) continue;
  
        for(int i=0; i<S7; i++) {
          // note: need do cmove before c.spin
          heptagon *h1 = h->cmove(i);
          dq::enqueue(h1, V * adjmatrix(i, h->c.spin(i)));
          }
        }
      }

    };

  EX pair<heptagon*,heptagon*> getcoord(heptagon *h) {
    return ((hrmap_solnih*)currentmap)->coords[h];
    }

  EX heptagon *get_at(heptagon *h1, heptagon *h2, bool gen) {
    auto m = ((hrmap_solnih*)currentmap);
    if(!gen && !m->at.count(make_pair(h1, h2))) return nullptr;
    return m->get_at(h1, h2);
    }
  
  EX string common = 
    "uniform mediump sampler3D tInvExpTable;"    
    "uniform mediump float PRECX, PRECY, PRECZ;"

    "float x_to_ix(float u) {"
    "  if(u < 1e-6) return 0.;"
    "  float diag = u*u/2.;"
   
    "  float x = diag;"
    "  float y = u;"
    "  float z = diag+1.;"
    
    "  x /= (1.+z);"
    "  y /= (1.+z);"
    
    "  return 0.5 - atan((0.5-x) / y) / 3.1415926535897932384626433832795;"
    "  }";

  hyperpoint christoffel(const hyperpoint at, const hyperpoint velocity, const hyperpoint transported) {
    const ld l2 = log(2);
    const ld l3 = log(3);
    switch(geom()) {
      case gSolN:
        return hpxyz3(
          -(velocity[2] * transported[0] + velocity[0] * transported[2]) * l2,
           (velocity[2] * transported[1] + velocity[1] * transported[2]) * l3,
           velocity[0] * transported[0] * exp(2*l2*at[2]) * l2 - velocity[1] * transported[1] * exp(-2*l3*at[2]) * l3,
           0
           );
      case gSol:
        return hpxyz3(
          -velocity[2] * transported[0] - velocity[0] * transported[2],
           velocity[2] * transported[1] + velocity[1] * transported[2],
           velocity[0] * transported[0] * exp(2*at[2]) - velocity[1] * transported[1] * exp(-2*at[2]),
           0
           );
      case gNIH:
        return hpxyz3(
           (velocity[2] * transported[0] + velocity[0] * transported[2]) * l2,
           (velocity[2] * transported[1] + velocity[1] * transported[2]) * l3,
           -(velocity[0] * transported[0] * exp(-2*l2*at[2]) * l2 + velocity[1] * transported[1] * exp(-2*l3*at[2]) * l3),
           0
           );
      default:
        throw "christoffel not in solnihv";
      }
    }
  
  EX hyperpoint get_inverse_exp_symsol(hyperpoint h, bool lazy, bool just_direction) {
    auto& s = get_tabled();
    s.load();
    
    ld ix = h[0] >= 0. ? solnihv::x_to_ix(h[0]) : solnihv::x_to_ix(-h[0]);
    ld iy = h[1] >= 0. ? solnihv::x_to_ix(h[1]) : solnihv::x_to_ix(-h[1]);
    ld iz = tanh(h[2]);
    
    if(h[2] < 0.) { iz = -iz; swap(ix, iy); }
    
    hyperpoint res = s.get(ix, iy, iz, lazy);
  
    if(h[2] < 0.) { swap(res[0], res[1]); res[2] = -res[2]; }
    if(h[0] < 0.) res[0] = -res[0];
    if(h[1] < 0.) res[1] = -res[1];
    
    if(!just_direction) {
      ld r = hypot_d(3, res);
      if(r == 0.) return res;
      return res * atanh(r) / r;
      }

    return res;
    }

  EX hyperpoint get_inverse_exp_nsym(hyperpoint h, bool lazy, bool just_direction) {
    auto& s = get_tabled();
    s.load();
    
    ld ix = h[0] >= 0. ? solnihv::x_to_ix(h[0]) : solnihv::x_to_ix(-h[0]);
    ld iy = h[1] >= 0. ? solnihv::x_to_ix(h[1]) : solnihv::x_to_ix(-h[1]);
    ld iz = (tanh(h[2]/4)+1)/2;
    
    hyperpoint res = s.get(ix, iy, iz, lazy);
  
    if(h[0] < 0.) res[0] = -res[0];
    if(h[1] < 0.) res[1] = -res[1];
    
    if(!just_direction) {
      ld r = hypot_d(3, res);
      if(r == 0.) return res;
      return res * atanh(r) / r;
      }

    return res;
    }

  EX string shader_symsol = solnihv::common +

    "vec4 inverse_exp(vec4 h) {"
    
    "float ix = h[0] >= 0. ? x_to_ix(h[0]) : x_to_ix(-h[0]);"
    "float iy = h[1] >= 0. ? x_to_ix(h[1]) : x_to_ix(-h[1]);"
    "float iz = tanh(h[2]);"
    
    "if(h[2] < 1e-6) { iz = -iz; float s = ix; ix = iy; iy = s; }"
    "if(iz < 0.) iz = 0.;"
    
    "vec4 res;"

    "float cx = ix*(1.-1./PRECX) + .5/PRECX;"
    "float cy = iy*(1.-1./PRECY) + .5/PRECY;"
    "float cz = iz*(1.-1./PRECZ) + .5/PRECZ;"

    "if(ix > .5 && iy > .6 && ix < iy + .05 && iz < .2 && iz < (iy - 0.5) * 0.6)"
       "res = vec4(0.,0.,0.,1.); "
    
     "else "
  
      "res = texture3D(tInvExpTable, vec3(cx, cy, cz));"

    "if(h[2] < 1e-6) { res.xy = res.yx; res[2] = -res[2]; }"
    "if(h[0] < 0.) res[0] = -res[0];"
    "if(h[1] < 0.) res[1] = -res[1];"
    
    "return res;"
    "}";

  EX string shader_nsymsol = solnihv::common + R"*(

    vec4 inverse_exp(vec4 h) {
      
    float ix = h[0] >= 0. ? x_to_ix(h[0]) : x_to_ix(-h[0]);
    float iy = h[1] >= 0. ? x_to_ix(h[1]) : x_to_ix(-h[1]);
    float iz = (tanh(h[2]/4.)+1.) / 2.;
      
    vec4 res;
  
    float cx = ix*(1.-1./PRECX) + .5/PRECX;
    float cy = iy*(1.-1./PRECY) + .5/PRECY;
    float cz = iz*(1.-1./PRECZ) + .5/PRECZ;
  
    if(ix > .65 && iy > .5 && iz > .45 && iz < .55)
      res = vec4(0.,0.,0.,1.);
    else if(ix > .55 && iy > .75 && ix < .7 && iz > .45 && iz < .55)
      res = vec4(0.,0.,0.,1.);
    else if(ix > .45 && iy > .75 && ix < .7 && iz > .4 && iz < .5)
      res = vec4(0.,0.,0.,1.);
    else if(ix > .85 && iy > .5 && iz > .55 && iz < .75)
      res = vec4(0.,0.,0.,1.);
    else if(ix > .7 && iy > .55 && iz > .42 && iz < .58)
      res = vec4(0.,0.,0.,1.);
    else if(iz > 0.45 && ix > 0.8 && iy > 0.3 && iy < 0.6)
      res = vec4(0.,0.,0.,1.);
    else if(iz > 0.45 && ix > 0.8 && iy > 0.3 && iy < 0.6)
      res = vec4(0.,0.,0.,1.);
    else if(iz > .4 && iz < .55 && ix > .7 && iy > .36 && iy < .5 && ix < .8 && ix+iy > 1.2)
      res = vec4(0.,0.,0.,1.);
    else res = texture3D(tInvExpTable, vec3(cx, cy, cz));
  
    if(h[0] < 0.) res[0] = -res[0];
    if(h[1] < 0.) res[1] = -res[1];
      
    return res;
    })*";

  EX string shader_nsym = solnihv::common +

    "vec4 inverse_exp(vec4 h) {"
    
    "float ix = h[0] >= 0. ? x_to_ix(h[0]) : x_to_ix(-h[0]);"
    "float iy = h[1] >= 0. ? x_to_ix(h[1]) : x_to_ix(-h[1]);"
    "float iz = (tanh(h[2]/4.)+1.) / 2.;"
    
    "vec4 res;"

    "float cx = ix*(1.-1./PRECX) + .5/PRECX;"
    "float cy = iy*(1.-1./PRECY) + .5/PRECY;"
    "float cz = iz*(1.-1./PRECZ) + .5/PRECZ;"

    "res = texture3D(tInvExpTable, vec3(cx, cy, cz));"

    "if(h[0] < 0.) res[0] = -res[0];"
    "if(h[1] < 0.) res[1] = -res[1];"
    
    "return res;"
    "}";

  EX ld solrange_xy = 15;
  EX ld solrange_z = 4;
  
  EX bool in_table_range(hyperpoint h) {
    return abs(h[0]) < solrange_xy && abs(h[1]) < solrange_xy && abs(h[2]) < solrange_z;
    }

  EX tabled_inverses solt = solnihv::tabled_inverses("solv-geodesics.dat");
  EX tabled_inverses niht = solnihv::tabled_inverses("shyp-geodesics.dat");
  EX tabled_inverses sont = solnihv::tabled_inverses("ssol-geodesics.dat");
  
  EX tabled_inverses& get_tabled() {
    switch(geom()) {
      case gSol: return solt;
      case gNIH: return niht;
      case gSolN: return sont;
      default: throw "not solnih";
      }
    }

  EX int approx_distance(heptagon *h1, heptagon *h2) {
    auto m = (solnihv::hrmap_solnih*) currentmap;
    dynamicval<eGeometry> g(geometry, gBinary4); 
    dynamicval<hrmap*> cm(currentmap, m->binary_map);
    int d1 = binary::celldistance3_approx(m->coords[h1].first, m->coords[h2].first);
    int d2 = binary::celldistance3_approx(m->coords[h1].second, m->coords[h2].second);
    return d1 + d2 - abs(h1->distance - h2->distance);
    }    
EX }
#endif

EX namespace nilv {

  hyperpoint christoffel(const hyperpoint Position, const hyperpoint Velocity, const hyperpoint Transported) {
    ld x = Position[0];
    return point3(
      x * Velocity[1] * Transported[1] - 0.5 * (Velocity[1] * Transported[2] + Velocity[2] * Transported[1]),
      -.5 * x * (Velocity[1] * Transported[0] + Velocity[0] * Transported[1]) + .5 * (Velocity[2] * Transported[0] + Velocity[0] * Transported[2]),
      -.5 * (x*x-1) * (Velocity[1] * Transported[0] + Velocity[0] * Transported[1]) + .5 * x * (Velocity[2] * Transported[0] + Velocity[0] * Transported[2])
      );
    }

  EX hyperpoint formula_exp(hyperpoint v) {
    // copying Modelling Nil-geometry in Euclidean Space with Software Presentation
    // v[0] = c cos alpha
    // v[1] = c sin alpha
    // v[2] = w
    
    if(v[0] == 0 && v[1] == 0) return point31(v[0], v[1], v[2]);
    
    if(v[2] == 0) return point31(v[0], v[1], v[0] * v[1] / 2);
    
    ld alpha = atan2(v[1], v[0]);
    ld w = v[2];
    ld c = hypot(v[0], v[1]) / v[2];
    
    return point31(
      2 * c * sin(w/2) * cos(w/2 + alpha), 
      2 * c * sin(w/2) * sin(w/2 + alpha), 
      w * (1 + (c*c/2) * ((1 - sin(w)/w) + (1-cos(w))/w * sin(w + 2 * alpha)))
      );
    }
  
  EX hyperpoint get_inverse_exp(hyperpoint h, int iterations) {
    ld wmin, wmax;
    
    ld side = h[2] - h[0] * h[1] / 2;
    
    if(hypot_d(2, h) < 1e-6) return point3(h[0], h[1], h[2]);
    else if(side > 1e-6) {
      wmin = 0, wmax = 2 * M_PI;
      }
    else if(side < -1e-6) {
      wmin = - 2 * M_PI, wmax = 0;
      }
    else return point3(h[0], h[1], 0);
    
    ld alpha_total = h[0] ? atan(h[1] / h[0]) : M_PI/2;
  
    ld b;
    if(abs(h[0]) > abs(h[1]))
      b = h[0] / 2 / cos(alpha_total);
    else
      b = h[1] / 2 / sin(alpha_total);  
    
    ld s = sin(2 * alpha_total);
    
    for(int it=0;; it++) {
      ld w = (wmin + wmax) / 2;
      ld z = b * b * (s + (sin(w) - w)/(cos(w) - 1)) + w;

      if(it == iterations) {
        ld alpha = alpha_total - w/2;
        ld c = b / sin(w/2);
        return point3(c * w * cos(alpha), c * w * sin(alpha), w);
        }
      if(h[2] > z) wmin = w;
      else wmax = w;    
      }
    }
  
  EX string nilshader = 
    "vec4 inverse_exp(vec4 h) {"
      "float wmin, wmax;"
      "float side = h[2] - h[0] * h[1] / 2.;"
      "if(h[0]*h[0] + h[1]*h[1] < 1e-12) return vec4(h[0], h[1], h[2], 1);"
      "if(side > 1e-6) { wmin = 0.; wmax = 2.*PI; }"
      "else if(side < -1e-6) { wmin = -2.*PI; wmax = 0.; }"
      "else return vec4(h[0], h[1], 0., 1.);"
      "float at = h[0] != 0. ? atan(h[1] / h[0]) : PI/2.;"
      "float b = abs(h[0]) > abs(h[1]) ? h[0] / 2. / cos(at) : h[1] / 2. / sin(at);"
      "float s = sin(2. * at);"

      "for(int it=0; it<50; it++) {"
        "float w = (wmin + wmax) / 2.;"
        // the formula after ':' produces visible numerical artifacts for w~0
        "float z = b * b * (s + (abs(w) < .1 ? w/3. + w*w*w/90. + w*w*w*w*w/2520.: (sin(w) - w)/(cos(w) - 1.))) + w;"
        "if(h[2] > z) wmin = w;"
        "else wmax = w;"
        "}"

      "float w = (wmin + wmax) / 2.;"
      "float alpha = at - w/2.;"
      "float c = b / sin(w/2.);"
      "return vec4(c*w*cos(alpha), c*w*sin(alpha), w, 1.);"
      "}";
  
  #if HDR
  struct mvec : array<int, 3> {
    
    mvec() { }
  
    mvec(int x, int y, int z) { 
      auto& a = *this;
      a[0] = x; a[1] = y; a[2] = z;
      }
    mvec inverse() {  
      auto& a = *this;
      return mvec(-a[0], -a[1], -a[2]+a[1] * a[0]); 
      }
    mvec operator * (const mvec b) {
      auto& a = *this;
      return mvec(a[0] + b[0], a[1] + b[1], a[2] + b[2] + a[0] * b[1]);
      }
    };
  #endif

  static const mvec mvec_zero = mvec(0, 0, 0);

  hyperpoint mvec_to_point(mvec m) { return hpxy3(m[0], m[1], m[2]); }
  
  #if HDR
  struct nilstructure {
    vector<mvec> movevectors;
    vector<vector<hyperpoint>> facevertices;
    };
  #endif
  
  nilstructure ns6 = {
    {{ mvec(-1,0,0), mvec(0,-1,0), mvec(0,0,-1), mvec(1,0,0), mvec(0,1,0), mvec(0,0,1) }},
  
    {{
    { point31(-0.5,-0.5,-0.25), point31(-0.5,-0.5,0.75), point31(-0.5,0.5,0.25), point31(-0.5,0.5,-0.75), },
    { point31(0.5,-0.5,-0.5), point31(0.5,-0.5,0.5), point31(-0.5,-0.5,0.5), point31(-0.5,-0.5,-0.5), },
    { point31(0,0,-0.5), point31(-0.5,0.5,-0.75), point31(-0.5,-0.5,-0.25), point31(0,0,-0.5), point31(-0.5,-0.5,-0.25), point31(-0.5,-0.5,-0.5), point31(0,0,-0.5), point31(-0.5,-0.5,-0.5), point31(0.5,-0.5,-0.5), point31(0,0,-0.5), point31(0.5,-0.5,-0.5), point31(0.5,-0.5,-0.75), point31(0,0,-0.5), point31(0.5,-0.5,-0.75), point31(0.5,0.5,-0.25), point31(0,0,-0.5), point31(0.5,0.5,-0.25), point31(0.5,0.5,-0.5), point31(0,0,-0.5), point31(0.5,0.5,-0.5), point31(-0.5,0.5,-0.5), point31(0,0,-0.5), point31(-0.5,0.5,-0.5), point31(-0.5,0.5,-0.75), },
    { point31(0.5,0.5,-0.25), point31(0.5,0.5,0.75), point31(0.5,-0.5,0.25), point31(0.5,-0.5,-0.75), },
    { point31(-0.5,0.5,-0.5), point31(-0.5,0.5,0.5), point31(0.5,0.5,0.5), point31(0.5,0.5,-0.5), },
    { point31(0,0,0.5), point31(-0.5,0.5,0.25), point31(-0.5,-0.5,0.75), point31(0,0,0.5), point31(-0.5,-0.5,0.75), point31(-0.5,-0.5,0.5), point31(0,0,0.5), point31(-0.5,-0.5,0.5), point31(0.5,-0.5,0.5), point31(0,0,0.5), point31(0.5,-0.5,0.5), point31(0.5,-0.5,0.25), point31(0,0,0.5), point31(0.5,-0.5,0.25), point31(0.5,0.5,0.75), point31(0,0,0.5), point31(0.5,0.5,0.75), point31(0.5,0.5,0.5), point31(0,0,0.5), point31(0.5,0.5,0.5), point31(-0.5,0.5,0.5), point31(0,0,0.5), point31(-0.5,0.5,0.5), point31(-0.5,0.5,0.25), },
    }}
    };
  
  nilstructure ns8 = {
    {{ mvec(-1,0,0), mvec(-1,0,1), mvec(0,-1,0), mvec(0,0,-1), mvec(1,0,0), mvec(1,0,-1), mvec(0,1,0), mvec(0,0,1) }},
  
    {{
      { point31(-0.5,-0.5,-0.25), point31(-0.5,-0.5,0.75), point31(-0.5,0.5,-0.25), },
      { point31(-0.5,-0.5,0.75), point31(-0.5,0.5,0.75), point31(-0.5,0.5,-0.25), },
      { point31(-0.5,-0.5,-0.25), point31(-0.5,-0.5,0.75), point31(0.5,-0.5,0.25), point31(0.5,-0.5,-0.75), },
      { point31(-0.5,-0.5,-0.25), point31(-0.5,0.5,-0.25), point31(0.5,0.5,-0.75), point31(0.5,-0.5,-0.75), },
      { point31(0.5,0.5,0.25), point31(0.5,-0.5,0.25), point31(0.5,-0.5,-0.75), },
      { point31(0.5,0.5,-0.75), point31(0.5,0.5,0.25), point31(0.5,-0.5,-0.75), },
      { point31(-0.5,0.5,0.75), point31(-0.5,0.5,-0.25), point31(0.5,0.5,-0.75), point31(0.5,0.5,0.25), },
      { point31(-0.5,-0.5,0.75), point31(-0.5,0.5,0.75), point31(0.5,0.5,0.25), point31(0.5,-0.5,0.25), },
      }}
    };
  
  EX nilstructure& current_ns() { return S7 == 6 ? ns6 : ns8; }

  EX array<int,3> nilperiod, nilperiod_edit;
  int S7_edit;
  
  EX transmatrix adjmatrix(int i) {
     return nisot::translate(mvec_to_point(current_ns().movevectors[i]));
     }
    
  struct hrmap_nil : hrmap {
    unordered_map<mvec, heptagon*> at;
    unordered_map<heptagon*, mvec> coords;
    
    heptagon *getOrigin() override { return get_at(mvec_zero); }
    
    ~hrmap_nil() {
      for(auto& p: at) clear_heptagon(p.second);
      }

    heptagon *get_at(mvec c) {
      auto& h = at[c];
      if(h) return h;
      h = tailored_alloc<heptagon> (S7);
      h->c7 = newCell(S7, h);
      coords[h] = c;
      h->dm4 = 0;
      h->zebraval = c[0];
      h->emeraldval = c[1];
      h->fieldval = c[2];
      h->cdata = NULL;
      h->alt = NULL;
      return h;      
      }

    heptagon *create_step(heptagon *parent, int d) override {
      auto p = coords[parent];
      auto q = p * current_ns().movevectors[d];
      for(int a=0; a<3; a++) q[a] = zgmod(q[a], nilperiod[a]);
      auto child = get_at(q);
      parent->c.connect(d, child, (d + S7/2) % S7, false);
      return child;
      }

    transmatrix adj(heptagon *h, int i) override { return adjmatrix(i); }
  
    virtual transmatrix relative_matrix(heptagon *h2, heptagon *h1, const hyperpoint& hint) override { 
      for(int a=0; a<S7; a++) if(h2 == h1->move(a)) return adjmatrix(a);
      auto p = coords[h1].inverse() * coords[h2];
      for(int a=0; a<3; a++) p[a] = szgmod(p[a], nilperiod[a]);     
      return nisot::translate(mvec_to_point(p));
      }

    void draw() override {
      dq::visited_by_matrix.clear();

      dq::enqueue_by_matrix(centerover->master, cview());
      
      while(!dq::drawqueue.empty()) {
        auto& p = dq::drawqueue.front();
        heptagon *h = get<0>(p);
        transmatrix V = get<1>(p);
        dq::drawqueue.pop();
        
        cell *c = h->c7;
        if(!do_draw(c, V)) continue;
        drawcell(c, V);
        if(wallopt && isWall3(c) && isize(dq::drawqueue) > 1000) continue;

        if(0) for(int t=0; t<c->type; t++) {
          if(!c->move(t)) continue;
          dynamicval<color_t> g(poly_outline, darkena((0x142968*t) & 0xFFFFFF, 0, 255) );
          queuepoly(V, cgi.shWireframe3D[t], 0);
          }
  
        for(int i=0; i<S7; i++) {
          // note: need do cmove before c.spin
          heptagon *h1 = h->cmove(i);
          dq::enqueue_by_matrix(h1, V * adjmatrix(i));
          }
        }
      }
    };
  
  EX mvec get_coord(heptagon *h) { return ((hrmap_nil*)currentmap)->coords[h]; }

  EX void set_flags() {
    int coords = 0;
    for(int a=0; a<3; a++) if(nilperiod[a]) coords++;
    set_flag(ginf[gNil].flags, qANYQ, coords);
    set_flag(ginf[gNil].flags, qBOUNDED, coords == 3);
    set_flag(ginf[gNil].flags, qSMALL, coords == 3 && nilperiod[0] * nilperiod[1] * nilperiod[2] <= 4096);
    }

  EX hyperpoint on_geodesic(hyperpoint s0, hyperpoint s1, ld x) {
    hyperpoint local = inverse(nisot::translate(s0)) * s1;
    hyperpoint h = get_inverse_exp(local, 100);
    return nisot::translate(s0) * formula_exp(h * x);
    }

EX color_t colorize(cell *c, char whichCanvas) {
  mvec at = ((hrmap_nil*)currentmap)->coords[c->master];
  color_t res = 0;
  
  auto setres = [&] (int z, color_t which) {
    if(zgmod(at[2] - z, nilperiod[2]) == 0) res = which;
    if(zgmod(at[2] - z-1, nilperiod[2]) == 0) res = which;
    };

  if(at[1] == 0 && at[0] >=0 && at[0] < 4)
    setres(-at[0], gradient(0x1FF0000, 0x10000FF, 0, at[0], 4));
  else if(at[0] == 4 && at[1] >= 0 && at[1] < 4)
    setres(at[1]*3-4, gradient(0x10000FF, 0x100FF00, 0, at[1], 4));
  else if(at[1] == 4 && at[0] >= 0 && at[0] <= 4)
    setres(4+at[0], gradient(0x100FF00, 0x1FFFF00, 4, at[0], 0));
  else if(at[0] == 0 && at[1] >= 0 && at[1] <= 4)
    setres(at[1], gradient(0x1FFFF00, 0x1FF0000, 4, at[1], 0));
  
  return res;
  }

EX void prepare_niltorus3() {
  nilperiod_edit = nilperiod;
  S7_edit = ginf[gNil].sides;
  }
  
EX void show_niltorus3() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(1);  
  dialog::init(XLAT("Nil quotient spaces"));
  for(int a=0; a<3; a++) {
    string title = XLAT("%1 period", s0+char('X'+a));
    dialog::addSelItem(title, its(nilperiod_edit[a]), 'x');
    dialog::add_action([=] { 
      dialog::editNumber(nilperiod_edit[a], 0, 60, 1, 0, title, 
        XLAT("Set to 0 to make it non-periodic.")
        ); 
      dialog::bound_low(0);
      });      
    }
  dialog::addSelItem(XLAT("honeycomb"), its(S7_edit), 'h');
  dialog::add_action([] { S7_edit = S7_edit ^ 6 ^ 8; });
  
  bool ok = (!nilperiod_edit[1]) || (nilperiod_edit[2] && nilperiod_edit[1] % nilperiod_edit[2] == 0);

  dialog::addBreak(50);

  if(ok) {
    dialog::addItem(XLAT("activate"), 'a');
    dialog::add_action([] {
      stop_game();
      nilperiod = nilperiod_edit;
      ginf[gNil].sides = S7_edit;
      set_flags();
      geometry = gNil;
      start_game();
      });
    }
  else dialog::addInfo(XLAT("Y period must be divisible by Z period"));
    
  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  }
  
EX }

EX bool in_s2xe() { return prod && hybrid::under_class() == gcSphere; }
EX bool in_h2xe() { return prod && hybrid::under_class() == gcHyperbolic; }
EX bool in_e2xe() { return prod && hybrid::under_class() == gcEuclid; }

EX namespace hybrid {

  EX eGeometry underlying;
  EX geometry_information *underlying_cgip;

  EX eGeometryClass under_class() { return ginf[hybrid::underlying].cclass; }  
  
  EX void configure(eGeometry g) {
    if(WDIM == 3) return;
    ray::reset_raycaster();
    check_cgi();
    cgi.require_basics();
    underlying = geometry;
    underlying_cgip = cgip;
    bool sph = sphere;
    auto keep = ginf[g].menu_displayed_name;
    ginf[g] = ginf[underlying];
    ginf[g].menu_displayed_name = keep;
    if(g == gRotSpace) {
      ginf[g].g = sph ? giSphere3 : giSL2;
      ginf[g].tiling_name = "Iso(" + ginf[g].tiling_name + ")";
      string& qn = ginf[g].quotient_name;
      string qplus = sph ? "elliptic" : qn;
      if(qn == "none" || qn == "elliptic") qn = qplus;
      else qn = qn + "/" + qplus;
      if(sph) ginf[g].flags |= qELLIPTIC;
      }
    else {
      ginf[g].cclass = g == gRotSpace ? gcSL2 : gcProduct;
      ginf[g].g.gameplay_dimension++;
      ginf[g].g.graphical_dimension++;
      ginf[g].tiling_name += "xZ";
      if(product::csteps) ginf[g].flags |= qANYQ, ginf[g].tiling_name += its(product::csteps);
      }
    ginf[g].flags |= qHYBRID;
    }
  
  EX void reconfigure() {
    if(!hybri) return;
    stop_game();
    auto g = geometry;
    geometry = underlying;
    configure(g);
    geometry = g;
    }

  EX hrmap *pmap;
  EX geometry_information *pcgip;
  EX eGeometry actual_geometry;
  
  template<class T> auto in_actual(const T& t) -> decltype(t()) {
    dynamicval<eGeometry> g(geometry, actual_geometry);
    dynamicval<geometry_information*> gc(cgip, pcgip);
    dynamicval<hrmap*> gu(currentmap, pmap);
    dynamicval<hrmap*> gup(pmap, NULL);
    return t();  
    }
  
  struct hrmap_hybrid : hrmap {
    
    hrmap *underlying_map;
    
    int space_spin;
    map<cell*, pair<cellwalker, cellwalker>> spins;
    
    map<pair<cell*, int>, cell*> at;
    map<cell*, pair<cell*, int>> where;
    
    heptagon *getOrigin() override { return underlying_map->getOrigin(); }
    
    template<class T> auto in_underlying(const T& t) -> decltype(t()) {
      pcgip = cgip; 
      dynamicval<hrmap*> gpm(pmap, this);
      dynamicval<eGeometry> gag(actual_geometry, geometry);
      dynamicval<eGeometry> g(geometry, underlying);
      dynamicval<geometry_information*> gc(cgip, underlying_cgip);
      dynamicval<hrmap*> gu(currentmap, underlying_map);
      return t();
      }
    
    cell *getCell(cell *u, int h) {
      if(space_spin) {
        if(!spins.count(u))
          println(hlog, "link missing: ", u);
        else {
          while(h >= cgi.steps) h -= cgi.steps, u = spins[u].first.at;
          while(h < 0) h += cgi.steps, u = spins[u].second.at;
          }
        }
      h = zgmod(h, cgi.steps);
      cell*& c = at[make_pair(u, h)];
      if(!c) { c = newCell(u->type+2, u->master); where[c] = {u, h}; }
      return c;
      }
  
    cell* gamestart() override { return getCell(underlying_map->gamestart(), 0); }
  
    hrmap_hybrid() {
      space_spin = 0;
      in_underlying([this] { initcells(); underlying_map = currentmap; });
      for(hrmap*& m: allmaps) if(m == underlying_map) m = NULL;
      }
    
    ~hrmap_hybrid() {
      in_underlying([] { delete currentmap; });
      for(auto& p: at) tailored_delete(p.second);
      }
  
    virtual transmatrix spin_to(cell *c, int d, ld bonus) override { if(d >= c->type-2) return Id; c = get_where(c).first; return in_underlying([&] { return currentmap->spin_to(c, d, bonus); }); }
    virtual transmatrix spin_from(cell *c, int d, ld bonus) override { if(d >= c->type-2) return Id; c = get_where(c).first; return in_underlying([&] { return currentmap->spin_from(c, d, bonus); }); }

    };
  
  hrmap_hybrid* hmap() { return (hrmap_hybrid*) currentmap; }
  
  EX cell *get_at(cell *base, int level) {
    return hmap()->getCell(base, level);
    }
  
  EX pair<cell*, int> get_where(cell *c) { return hmap()->where[c]; }

  EX void find_cell_connection(cell *c, int d) {
    auto m = hmap();
    if(d >= c->type - 2) {
      int s = cgi.single_step;
      cell *c1 = get_at(m->where[c].first, m->where[c].second + (d == c->type-1 ? s : -s));
      c->c.connect(d, c1, c1->type - 3 + c->type - d, false);
      }
    else {
      auto cu = m->where[c].first;
      auto cu1 = m->in_underlying([&] { return cu->cmove(d); });
      int d1 = cu->c.spin(d);
      int s = (geometry == gRotSpace && cgi.steps) ? d*cgi.steps / cu->type - d1*cgi.steps / cu1->type + cgi.steps/2 : 0;
      cell *c1 = get_at(cu1, m->where[c].second + s);
      c->c.connect(d, c1, d1, cu->c.mirror(d));
      }
    }  

  EX hrmap* get_umap() { return ((hrmap_hybrid*)currentmap)->underlying_map; }
  
  #if HDR
  template<class T> auto in_underlying_geometry(const T& f) -> decltype(f()) {
    if(!hybri) return f();
    dynamicval<eGeometry> g(geometry, underlying);
    dynamicval<eGeometry> gag(actual_geometry, geometry);
    dynamicval<geometry_information*> gc(cgip, underlying_cgip);
    dynamicval<hrmap*> gpm(pmap, currentmap);
    dynamicval<hrmap*> gm(currentmap, get_umap());
    return f();
    }
  
  #define PIU(x) hr::hybrid::in_underlying_geometry([&] { return (x); })
  #endif

  EX hyperpoint get_corner(cell *c, int i, int next, ld z) {
    ld lev = cgi.plevel * z / 2;
    if(prod) {
      dynamicval<eGeometry> g(geometry, hybrid::underlying);
      dynamicval<geometry_information*> gc(cgip, hybrid::underlying_cgip);
      dynamicval<hrmap*> gm(currentmap, ((hrmap_hybrid*)currentmap)->underlying_map);
      return mscale(get_corner_position(c, i+next), exp(lev));
      }
    else {
      ld tf, he, alpha;
      in_underlying_geometry([&] {
        hyperpoint h1 = get_corner_position(c, i);
        hyperpoint h2 = get_corner_position(c, i+1);
        hyperpoint hm = mid(h1, h2);
        tf = hdist0(hm)/2;
        he = hdist(hm, h2)/2;
        alpha = atan2(hm[1], hm[0]);
        });
      return spin(alpha) * rots::uxpush(tf) * rots::uypush(next?he:-he) * rots::uzpush(lev) * C0;
      }
    }
  
  EX int wall_offset(cell *c) {
    int id = hybrid::underlying == gArchimedean ? arcm::id_of(c->master) + 20 * arcm::parent_index_of(c->master) : shvid(c);
    if(isize(cgi.walloffsets) <= id) cgi.walloffsets.resize(id+1, -1);
    int &wo = cgi.walloffsets[id];
    if(wo == -1) {
      cell *c1 = hybrid::get_where(c).first;
      wo = isize(cgi.shWall3D);
      int won = wo + c->type;
      if(!cgi.wallstart.empty()) cgi.wallstart.pop_back();
      cgi.reserve_wall3d(won);
      
      if(prod) for(int i=0; i<c1->type; i++) {
        hyperpoint w;
        ((hrmap_hybrid*)currentmap)->in_underlying([&] { 
          /* mirror image of C0 in the axis h1-h2 */
          hyperpoint h1 = get_corner_position(c1, i);
          hyperpoint h2 = get_corner_position(c1, i+1);
          transmatrix T = gpushxto0(h1);
          T = spintox(T * h2) * T;
          w = T * C0;
          w[1] = -w[1];
          w = inverse(T) * w;
          });
        cgi.walltester[wo + i] = w;
        } 

      for(int i=0; i<c1->type; i++)
       cgi.make_wall(wo + i, {hybrid::get_corner(c1, i, 0, -1), hybrid::get_corner(c1, i, 0, +1), hybrid::get_corner(c1, i, 1, +1), hybrid::get_corner(c1, i, 1, -1)});

      for(int a: {0,1}) {
        vector<hyperpoint> l;
        int z = a ? 1 : -1;
        hyperpoint ctr = zpush0(z * cgi.plevel/2);
        for(int i=0; i<c1->type; i++)
          if(prod)
            l.push_back(hybrid::get_corner(c1, i, 0, z));
          else {
            l.push_back(ctr);
            l.push_back(hybrid::get_corner(c1, i, 0, z));
            l.push_back(hybrid::get_corner(c1, i+1, 1, z));
            l.push_back(ctr);
            l.push_back(hybrid::get_corner(c1, i, 1, z));
            l.push_back(hybrid::get_corner(c1, i, 0, z));
            }
        if(a == 0) std::reverse(l.begin()+1, l.end());
        cgi.make_wall(won-2+a, l);
        }

      cgi.wallstart.push_back(isize(cgi.raywall));
      cgi.compute_cornerbonus();
      cgi.extra_vertices();
      }
    return wo;
    }

  EX bool do_draw(cell *c, const transmatrix& T) {
    return in_actual([&] { return hr::do_draw(hybrid::get_at(c, get_where(centerover).second), T); });
    }
  
  vector<cell*> to_link;
  
  EX void will_link(cell *c) { if(pmap && ((hrmap_hybrid*) pmap)->space_spin) to_link.push_back(c); }
  
  EX void link() {
    auto pm = (hrmap_hybrid*) pmap;
    if(!pm) return;
    auto& ss = pm->spins;
    int success = -1;
    while(success) {
      vector<cell*> xlink = std::move(to_link);
      success = 0;
      for(cell *c: xlink) {
        bool success_here = ss.count(c);
        if(!success_here) forCellIdEx(c2, i, c) if(ss.count(c2)) {
          ss[c].first = ss[c2].first + c->c.spin(i) + wstep - i;
          ss[c].second = ss[c2].second + c->c.spin(i) + wstep - i;
          success++;
          success_here = true;
          break;
          }
        if(!success_here) to_link.push_back(c);
        }
      }
    }

EX }
  
EX namespace product {

  int z0;
  
  struct hrmap_product : hybrid::hrmap_hybrid {
    transmatrix relative_matrix(cell *c2, cell *c1, const hyperpoint& hint) override {
      return in_underlying([&] { return calc_relative_matrix(where[c2].first, where[c1].first, hint); }) * mscale(Id, cgi.plevel * szgmod(where[c2].second - where[c1].second, csteps));
      }

    transmatrix adj(cell *c, int i) override {
      if(space_spin && i == c->type-1 && where[c].second == cgi.steps-1) {
        auto b = spins[where[c].first].first;
        transmatrix T = mscale(Id, cgi.plevel);
        T = T * spin(2 * M_PI * b.spin / b.at->type);
        if(b.mirrored) T = T * Mirror;
        return T;
        }
      if(space_spin && i == c->type-2 && where[c].second == 0) {
        auto b = spins[where[c].first].second;
        transmatrix T = mscale(Id, -cgi.plevel);
        T = T * spin(2 * M_PI * b.spin / b.at->type);
        if(b.mirrored) T = T * Mirror;
        return T;
        }
      if(i == c->type-1) return mscale(Id, cgi.plevel);
      else if(i == c->type-2) return mscale(Id, -cgi.plevel);
      c = where[c].first;
      return PIU(currentmap->adj(c, i));
      }
  
    void draw() override {
      auto w = hybrid::get_where(centerover);
      z0 = w.second;
      actual_view_level = z0 - floor(zlevel(tC0(cview())) / cgi.plevel + .5);
      dynamicval<cell*> co(centerover, hybrid::get_where(centerover).first);
      in_underlying([=] { currentmap->draw(); });
      }
    
    hrmap_product() {
      if(cspin) {
        space_spin = cspin;
        in_underlying([&] {
          auto ugs = currentmap->gamestart();
          spins[ugs] = make_pair(
            cellwalker(ugs, gmod(+space_spin, ugs->type)),
            cellwalker(ugs, gmod(-space_spin, ugs->type))
            );
          manual_celllister cl;
          cl.add(ugs);
          for(int i=0; i<isize(cl.lst); i++) {
            cell *c = cl.lst[i];
            hybrid::will_link(c);
            forCellIdEx(c2, i, c) cl.add(c2);
            }
          hybrid::link();
          });
        }
      }
    };

  EX int cwall_offset, cwall_mask, actual_view_level, csteps, cspin;
  
  EX void drawcell_stack(cellwalker cw, transmatrix V) {
    cell *c = cw.at;
    if(sphere) gmatrix[c] = V; /* some computations need gmatrix0 for underlying geometry */
    bool s = sphere || pmodel != mdPerspective;
    bool euc = euclid;
    hybrid::in_actual([&] { 
      cell *c0 = hybrid::get_at(c, z0);
      cwall_offset = hybrid::wall_offset(c0);
      if(s) cwall_mask = (1<<c->type) - 1;
      else {
        cwall_mask = 0;
        ld d = euc ? sqhypot_d(2, tC0(V)) : V[2][2];
        for(int i=0; i<c->type; i++) {
          hyperpoint h = (V * cgi.walltester[cwall_offset + i]);
          ld d1 = euc ? sqhypot_d(2, h) : h[2];
          if(d1 < d - 1e-6) cwall_mask |= (1<<i);
          }
        }
      cwall_mask |= (2<<c->type);
      int flat_distance = hdist0(product_decompose(tC0(V)).second);
      int max_z = flat_distance > sightranges[gProduct] ? 0 : sqrt(sightranges[gProduct] * sightranges[gProduct] - flat_distance * flat_distance) + 1;
      for(int si: {0, 1}) {
        cell *c1 = hybrid::get_at(c, z0);
        int dir = c1->type-1-si;
        int z1 = z0;
        auto V0 = V;
        for(int z=0; z<=max_z; z++) {
          if(((hybrid::hrmap_hybrid*)currentmap)->space_spin) cwall_mask = -1;
          cwall_mask &= ~(3<<c->type);
          if(z1 > actual_view_level) cwall_mask |= (1<<c->type);
          if(z1 < actual_view_level) cwall_mask |= (2<<c->type);
          setdist(c1, 7, NULL);
          if(z || !si) drawcell(c1, V0); 
          V0 = V0 * currentmap->adj(c1, dir);
          c1 = c1->cmove(dir);
          z1 += (si ? -1 : 1);
          }
        }
      });
    }
  
  EX hyperpoint inverse_exp(hyperpoint h) {
    hyperpoint res;
    res[2] = zlevel(h);
    h = zshift(h, -res[2]);
    ld r = hypot_d(2, h);
    if(hybrid::under_class() == gcEuclid) {
      res[0] = h[0];
      res[1] = h[1];
      }
    else if(r < 1e-6) {
      res[0] = h[0];
      res[1] = h[1];
      }
    else {
      auto c = acos_auto_clamp(h[2]);
      r = c / r;
      res[0] = h[0] * r;
      res[1] = h[1] * r;
      }
    return res;
    }
  
  EX hyperpoint direct_exp(hyperpoint h) {
    hyperpoint res;
    ld d = hypot_d(2, h);
    ld cd = d == 0 ? 0 : sinh(d) / d;
    res[0] = h[0] * cd;
    res[1] = h[1] * cd;
    res[2] = cos_auto(d);
    return zshift(res, h[2]);
    }
  
EX }

EX namespace slr {

  /* This implementation is based on:
  // https://pdfs.semanticscholar.org/bf46/824df892593a1b6d1c84a5f99e90eece7c54.pdf
  // However, to make it consistent with the conventions in HyperRogue,
  // coordinates 0<->2 and 1<->3 are swapped,
  // then coordinates 2<->3 are swapped
  */

  EX ld range_xy = 2;
  EX int steps = 15;

  EX hyperpoint from_phigans(hyperpoint h) {
    ld r = asinh(hypot_d(2, h));
    ld x = h[0];
    ld y = h[1];
    ld z = h[2];
    return hyperpoint(x * cos(z) + y * sin(z), y * cos(z) - x * sin(z), cosh(r) * sin(z), cosh(r) * cos(z));
    }
  
  EX hyperpoint to_phigans(hyperpoint h) {
    ld z = atan2(h[2], h[3]);
    ld x = h[0];
    ld y = h[1];
    return point31(x * cos(z) - y * sin(z), y * cos(z) + x * sin(z), z);
    }

  /** in the 'phigans' model */
  hyperpoint christoffel(const hyperpoint Position, const hyperpoint Velocity, const hyperpoint Transported) {
    ld x = Position[0];
    ld y = Position[1];
    ld s = x*x + y*y + 1;
    ld x2 = x * x;
    ld y2 = y * y;
    ld x4 = x2 * x2;
    ld y4 = y2 * y2;
    return point3(
     + Velocity[ 0 ] * Transported[ 0 ] * (x*(4*x2*y2 + 4*y4 + 9*y2 + 1))
     + Velocity[ 0 ] * Transported[ 1 ] * (-y*(4*x4 + 4*x2*y2 + 9*x2 + 2))
     + Velocity[ 0 ] * Transported[ 2 ] * (-x*y*(x2 + y2 + 2))
     + Velocity[ 1 ] * Transported[ 0 ] * (-y*(4*x4 + 4*x2*y2 + 9*x2 + 2))
     + Velocity[ 1 ] * Transported[ 1 ] * (x*(4*x4 + 4*x2*y2 + 9*x2 + 5))
     + Velocity[ 1 ] * Transported[ 2 ] * (x4 + x2*y2 + 2*x2 + 1)
     + Velocity[ 2 ] * Transported[ 0 ] * (-x*y*(x2 + y2 + 2))
     + Velocity[ 2 ] * Transported[ 1 ] * (x4 + x2*y2 + 2*x2 + 1),
  
     + Velocity[ 0 ] * Transported[ 0 ] *  (y*(4*x2*y2 + 4*y4 + 9*y2 + 5))
     + Velocity[ 0 ] * Transported[ 1 ] *  (-x*(4*x2*y2 + 4*y4 + 9*y2 + 2))
     + Velocity[ 0 ] * Transported[ 2 ] *  (-x2*y2 - y4 - 2*y2 - 1)
     + Velocity[ 1 ] * Transported[ 0 ] *  (-x*(4*x2*y2 + 4*y4 + 9*y2 + 2))
     + Velocity[ 1 ] * Transported[ 1 ] *  (y*(4*x4 + 4*x2*y2 + 9*x2 + 1))
     + Velocity[ 1 ] * Transported[ 2 ] *  (x*y*(x2 + y2 + 2))
     + Velocity[ 2 ] * Transported[ 0 ] *  (-x2*y2 - y4 - 2*y2 - 1)
     + Velocity[ 2 ] * Transported[ 1 ] *  (x*y*(x2 + y2 + 2)),
  
     + Velocity[ 0 ] * Transported[ 0 ] *  (-4*x*y)
     + Velocity[ 0 ] * Transported[ 1 ] *  (2*x2 - 2*y2)
     + Velocity[ 0 ] * Transported[ 2 ] *  x
     + Velocity[ 1 ] * Transported[ 0 ] *  (2*x2 - 2*y2)
     + Velocity[ 1 ] * Transported[ 1 ] *  4*x*y
     + Velocity[ 1 ] * Transported[ 2 ] *  y
     + Velocity[ 2 ] * Transported[ 0 ] *  x
     + Velocity[ 2 ] * Transported[ 1 ] *  y
       ) / s;
    }

  EX transmatrix translate(hyperpoint h) {
    return matrix4(
      h[3], -h[2],  h[1],  h[0],
      h[2],  h[3], -h[0],  h[1],
      h[1], -h[0],  h[3],  h[2],
      h[0],  h[1], -h[2],  h[3]
      );
    }
  
  EX hyperpoint polar(ld r, ld theta, ld phi) {
    return hyperpoint(sinh(r) * cos(theta-phi), sinh(r) * sin(theta-phi), cosh(r) * sin(phi), cosh(r) * cos(phi));
    }
  
  EX hyperpoint xyz_point(ld x, ld y, ld z) {
    ld r = hypot(x, y);
    ld f = r ? sinh(r) / r : 1;
    return hyperpoint(x * f * cos(z) + y * f * sin(z), y * f * cos(z) - x * f * sin(z), cosh(r) * sin(z), cosh(r) * cos(z));
    }
  
  ld rootsin(ld square, ld s) {
    if(square > 0) return sinh(sqrt(square) * s) / sqrt(square);
    else if(square < 0) return sin(sqrt(-square) * s) / sqrt(-square);
    else return s;
    }
  
  /** it==0 is standard asin, it==1 is the next solution (PI-asin) */
  ld asin_it(ld z, int it) {
    auto ans = asin(z);
    if(it & 1) ans = M_PI - ans;
    return ans;
    }
  
  ld arootsin(ld square, ld v, int it) {
    if(square > 0) return asinh(v * sqrt(square)) / sqrt(square);
    else if(square < 0) return asin_it(v * sqrt(-square), it) / sqrt(-square);
    else return v;
    }
  
  ld roottan(ld square, ld s) {
    if(square > 0) return tanh(sqrt(square) * s) / sqrt(square);
    else if(square < 0) return tan(sqrt(-square) * s) / sqrt(-square);
    else return s;
    }

  hyperpoint geodesic_polar(ld alpha, ld beta, ld s) {
    auto c = cos(2*alpha);
    
    ld t;
    if(c > 0) 
      t = atan(sin(alpha) * tanh(sqrt(c) * s) / sqrt(c));
    else if(c < 0) {
      /* the formula in the paper is roughly atan(k*tan(s))
       * however, atan is not always to be taken in [-PI/2,PI/2]:
       * if s is in [kPI-PI/2, kPI+PI/2], we should also increase the result by kPI
       */
      ld x = sqrt(-c) * s;
      ld steps = floor(x/M_PI + 0.5);
      t = atan(sin(alpha) * tan(sqrt(-c) * s) / sqrt(-c)) + M_PI * steps;
      }
    else t = atan(sin(alpha) * s);
    
    return polar(
      asinh(cos(alpha) * rootsin(c, s)),
      beta - t,
      2*sin(alpha)*s - t
      );
    }

  EX hyperpoint formula_exp(hyperpoint h) {
    ld s = hypot_d(3, h);
    ld beta = atan2(h[1], h[0]);
    ld alpha = asin(h[2] / s);
    return geodesic_polar(alpha, beta, s);
    }

  void find_alpha(ld phi, ld r, ld theta, ld &alpha, ld &s, ld &beta) {
    if(phi < 0) { find_alpha(-phi, r, -theta, alpha, s, beta); alpha = -alpha; beta = -beta; return; }
    ld mina = 0, maxa = M_PI/2;
  
    bool next_nan = true;
    ld c;
    
    for(int it=0; it<40; it++) {
      alpha = (mina + maxa) / 2;
  
      c = cos(2 * alpha);    
      s = arootsin(c, sinh(r) / cos(alpha), 0);
      if(isnan(s)) { next_nan = true, maxa = alpha; continue; }
      ld got_phi = 2*sin(alpha)*s - atan(sin(alpha) * roottan(c, s));
      if(got_phi > phi) next_nan = false, maxa = alpha;
      else mina = alpha;
      }
    
    if(next_nan) {
      mina = M_PI/4;
  
      for(int it=0; it<40; it++) {
        alpha = (mina + maxa) / 2;
        c = cos(2 * alpha);    
        s = arootsin(c, sinh(r) / cos(alpha), 1);
        ld got_phi = 2*sin(alpha)*s - atan(sin(alpha) * roottan(c, s)) - M_PI;
        if(got_phi < phi) maxa = alpha;
        else mina = alpha;
        }
      beta = theta + atan(sin(alpha) * roottan(c, s)) + M_PI;
      }
    else beta = theta + atan(sin(alpha) * roottan(c, s));
    }

  EX hyperpoint get_inverse_exp(hyperpoint h, ld index IS(0)) {
    if(sqhypot_d(2, h) < 1e-12) return point3(0, 0, atan2(h[2], h[3]) + index);
    ld r = asinh(hypot_d(2, h));
    ld phi = atan2(h[2], h[3]) + index;
    ld theta = atan2(h[1], h[0]) + phi + index;
    
    ld alpha, s, beta;
    find_alpha(phi, r, theta, alpha, s, beta);
    
    return point3(s * cos(beta) * cos(alpha), s * sin(beta) * cos(alpha), s * sin(alpha));
    }

  EX string slshader = 

    "uniform mediump float uIndexSL;"
    "uniform mediump int uIterations;"
    
    "vec4 inverse_exp(vec4 h) {"
      "if(h[0]*h[0] + h[1] * h[1] < 1e-6) return vec4(0, 0, atan2(h[2], h[3]) + uIndexSL, 1);"
      "float r = asinh(sqrt(h[0] * h[0] + h[1] * h[1]));"
      "float phi = atan2(h[2], h[3]) + uIndexSL;"
      "float theta = atan2(h[1], h[0]) + phi + uIndexSL;"
      "float alpha;"
      "float s;"
      "float beta;"
      "float sgn = 1.;"
      "float bound = .999;"
      "if(phi < 0.) { phi = -phi; theta = -theta; sgn = -1.; }"
      "float c;"
      "s = sinh(r) / cos(PI/4.);"
      "float gphi = 2.*sin(PI/4.)*s - atan(sin(PI/4.) * s);"
      "float lo_gphi = gphi;"
      "float lo_s = s;"
      "float lo_alpha = PI/4.;"
      "float lx_gphi = gphi;"
      "float lx_s = s;"
      "float lx_alpha = PI/4.;"
      "float hi_gphi = gphi;"
      "float hi_s = s;"
      "float hi_alpha = PI/4.;"
      "if(gphi > phi) {"
      "  float mina = 0.;"
      "  float maxa = PI/4.;"
      "  lo_gphi = 0.; lo_s = r; lo_alpha = 0.;"
#if ISWEB
      "  for(int it=0; it<50; it++) { if(it >= uIterations) break; "
#else
      "  for(int it=0; it<uIterations; it++) {"
#endif
      "    alpha = (mina + maxa) / 2.;"
      "    c = sqrt(cos(2. * alpha));"
      "    s = asinh(sinh(r) / cos(alpha) * c) / c;"
      "    gphi = 2.*sin(alpha)*s - atan(sin(alpha) * tanh(c * s) / c);"
      "    if(gphi > phi) { maxa = alpha; hi_alpha = alpha; hi_s = s; hi_gphi = gphi; }"
      "    else { mina = alpha; lo_alpha = alpha; lo_s = s; lo_gphi = gphi; }"
      "    }"
      "  }"
      "else {"
      "  hi_gphi = phi; hi_s = phi; hi_alpha = 9.;"
      "  int next_nan = 1;"
      "  float mina = PI/4.;"
      "  float maxa = PI/2.;"
#if ISWEB
      "  for(int it=0; it<50; it++) { if(it >= uIterations) break; "
#else
      "  for(int it=0; it<uIterations; it++) {"
#endif
      "    alpha = (mina + maxa) / 2.;"
      "    c = sqrt(-cos(2. * alpha));"
      "    if(sinh(r) * c > bound * cos(alpha)) { next_nan = 1; maxa = alpha; continue; }"
      "    s = asin(sinh(r) * c / cos(alpha)) / c;"
      "    gphi = 2.*sin(alpha)*s - atan(sin(alpha) * tan(c*s) / c);"
      "    if(gphi > phi) { next_nan = 0; maxa = alpha; hi_gphi = gphi; hi_s = s; hi_alpha = alpha; }"
      "    else { mina = alpha; lx_gphi = lo_gphi; lx_s = lo_s; lx_alpha = lo_alpha; lo_gphi = gphi; lo_s = s; lo_alpha = alpha; }"
      "    }"
      "  if(next_nan != 0) {"
      "    mina = PI/4.; "
#if ISWEB
      "    for(int it=0; it<50; it++) { if(it >= uIterations) break; "
#else
      "    for(int it=0; it<uIterations; it++) {"
#endif
      "      alpha = (mina + maxa) / 2.;"
      "      c = sqrt(-cos(2. * alpha));"
      "      float z = sinh(r) * c / cos(alpha);"
      "      if(z>bound) { maxa = alpha; next_nan = 1; continue; }"
      "      float s1 = PI - asin(z);"
      "      s = s1 / c;"
      "      gphi = 2.*sin(alpha)*s - atan(sin(alpha) * tan(s1) / c) - PI;"
      "      if(gphi < phi) { next_nan = 0; maxa = alpha; hi_gphi = gphi; hi_s = s; hi_alpha = alpha; }"
      "      else { mina = alpha; lo_gphi = gphi; lo_s = s; lo_alpha = alpha; }"
      "      }"
      "    }"
      "  }"
      "if(hi_alpha <= 9.) { hi_gphi = lx_gphi; hi_s = lx_s; hi_alpha = lx_alpha; } "
        "float fr = (phi-lo_gphi) / (hi_gphi-lo_gphi);"
        "alpha = lo_alpha + (hi_alpha-lo_alpha) * fr;"
        "s = lo_s + (hi_s-lo_s) * fr;"
      "beta = theta - phi + 2.*sin(alpha)*s;"
      "alpha = alpha * sgn; beta = beta * sgn;"
      "return vec4(s * cos(beta) * cos(alpha), s * sin(beta) * cos(alpha), s * sin(alpha), 1.);"
      "}";

EX }

EX namespace rots {

  EX transmatrix uxpush(ld x) { 
    if(sl2) return xpush(x);
    return cspin(1, 3, x) * cspin(0, 2, x);
    }

  EX transmatrix uypush(ld y) { 
    if(sl2) return ypush(y);
    return cspin(0, 3, -y) * cspin(1, 2, y);
    }
     
  EX transmatrix uzpush(ld z) {
    if(sl2) return zpush(z);
    return cspin(3, 2, -z) * cspin(0, 1, -z);
    }
  
  struct hrmap_rotation_space : hybrid::hrmap_hybrid {

    std::unordered_map<int, transmatrix> saved_matrices;

    transmatrix get_relative_matrix(cell *c1, int i) {
      if(i == c1->type-2) return uzpush(-cgi.plevel) * spin(-2*cgi.plevel);
      if(i == c1->type-1) return uzpush(+cgi.plevel) * spin(+2*cgi.plevel);
      cell *c2 = c1->cmove(i);
      int id1 = hybrid::underlying == gArchimedean ? arcm::id_of(c1->master) + 20 * arcm::parent_index_of(c1->master) : shvid(c1);
      int id2 = hybrid::underlying == gArchimedean ? arcm::id_of(c2->master) + 20 * arcm::parent_index_of(c2->master) : shvid(c2);
      int j = c1->c.spin(i);
      int id = id1 + (id2 << 10) + (i << 20) + (j << 26);
      auto &M = saved_matrices[id];
      if(M[3][3]) return M;
      
      /*if(PURE && hybrid::underlying != gArchimedean) {
        ld A = master_to_c7_angle();
        transmatrix Q = spin(-A + 2 * M_PI * i / S7) * uxpush(cgi.tessf) * spin(M_PI - 2 * M_PI * j / S7 + A);
        return Q;
        } */
      hyperpoint d;
      ld alpha, beta, distance;
      transmatrix Spin;
      cell *cw = where[c1].first;
      in_underlying([&] {
        transmatrix T = adj(cw, i);
        hyperpoint h = tC0(T);
        Spin = inverse(gpushxto0(h) * T);
        d = hr::inverse_exp(h, iTable);
        alpha = atan2(Spin[0][1], Spin[0][0]);
        distance = hdist0(h);
        beta = atan2(h[1], h[0]);
        });
      for(int k=0; k<3; k++) Spin[3][k] = Spin[k][3] = 0; Spin[3][3] = 1;
      return M = spin(beta) * uxpush(distance/2) * spin(-beta+alpha);
      }
    
    virtual transmatrix relative_matrix(cell *c2, cell *c1, const hyperpoint& hint) override { 
      if(c1 == c2) return Id;
      if(gmatrix0.count(c2) && gmatrix0.count(c1))
        return inverse(gmatrix0[c1]) * gmatrix0[c2];
      for(int i=0; i<c1->type; i++) if(c1->move(i) == c2) return get_relative_matrix(c1, i);
      return Id; // not implemented yet
      }

    void draw() override {
      set<cell*> visited;
      
      cell* start = centerover;
      vector<pair<cell*, transmatrix>> dq;
      
      visited.insert(start);
      dq.emplace_back(start, cview());
      
      for(int i=0; i<isize(dq); i++) {
        cell *c = dq[i].first;
        transmatrix V = dq[i].second;
        
        if(sl2) {
          if(V[3][3] < 0) V = centralsym * V;
          if(!do_draw(c, V)) continue;
          drawcell(c, V);
          }
        else {
          drawcell(c, V);
          }
        if(wallopt && isWall3(c) && isize(dq::drawqueue) > 1000) continue;

        for(int i=0; i<c->type; i++) {
          cell *c1 = c->cmove(i);
          if(visited.count(c1)) continue;
          visited.insert(c1);
          dq.emplace_back(c1, V * get_relative_matrix(c, i));
          }
        }
      }
    };

  /** reinterpret the given point of rotspace as a rotation matrix in the underlying geometry */
  EX transmatrix qtm(hyperpoint h) {
    if(hyperbolic) {
      hyperpoint k = slr::to_phigans(h);
      ld z = k[2]; k[2] = 0;
      ld r = hypot_d(2, k);
      // k[1] = -k[1];
      k[0] = -k[0];
      if(r) k = tangent_length(k, asinh(r) * 2);
      return spin(-z * 2) * rgpushxto0(direct_exp(k, 0));
      }

    double sq0 = h[0]*h[0];
    double sq1 = h[1]*h[1];
    double sq2 = h[2]*h[2];
    double sq3 = h[3]*h[3];
    
    transmatrix M;  
  
    M[0][0] =  sq0 - sq1 - sq2 + sq3;
    M[1][1] = -sq0 + sq1 - sq2 + sq3;
    M[2][2] = -sq0 - sq1 + sq2 + sq3;
    
    double tmp1 = h[0]*h[1];
    double tmp2 = h[2]*h[3];
    M[0][1] = -2 * (tmp1 + tmp2);
    M[1][0] = -2 * (tmp1 - tmp2);
    
    tmp1 = h[0]*h[2];
    tmp2 = h[1]*h[3];
    M[0][2] = 2 * (tmp1 - tmp2);
    M[2][0] = 2 * (tmp1 + tmp2);

    tmp1 = h[1]*h[2];
    tmp2 = h[0]*h[3];
    M[1][2] = -2 * (tmp1 + tmp2);
    M[2][1] = -2 * (tmp1 - tmp2);
  
    return M;
    }
  
  EX ld underlying_scale = 0;
  
  EX void draw_underlying(bool cornermode) {
    if(underlying_scale <= 0) return;
    ld d = hybrid::get_where(centerover).second;
    d *= cgi.plevel;
    transmatrix T = rots::uzpush(-d) * spin(-2*d);
  
    if(det(T) < 0) T = centralsym * T;
  
    hyperpoint h = inverse(View * spin(master_to_c7_angle()) * T) * C0;
    
    auto g = std::move(gmatrix);
    auto g0 = std::move(gmatrix0);
    
    ld alpha = atan2(inverse(NLP) * point3(1, 0, 0));
    
    bool inprod = prod;
    transmatrix pView = View;
    if(inprod) {
      pView = spin(alpha) * View;
      ld z = zlevel(tC0(View));
      for(int a=0; a<3; a++) pView[a] *= exp(-z);
      }
    
    cell *co = hybrid::get_where(centerover).first;
  
    hybrid::in_underlying_geometry([&] {
      cgi.require_shapes();
      dynamicval<int> pcc(corner_centering, cornermode ? 1 : 2);
      dynamicval<bool> pf(playerfound, true);
      dynamicval<cell*> m5(centerover, co);
      dynamicval<transmatrix> m2(View, inprod ? pView : ypush(0) * qtm(h));
      dynamicval<transmatrix> m3(playerV, Id);
      dynamicval<transmatrix> m4(actual_view_transform, Id);
      dynamicval<eModel> pm(pmodel, mdDisk);
      dynamicval<ld> pss(vid.scale, (sphere ? 10 : 1) * underlying_scale);
      dynamicval<ld> psa(vid.alpha, sphere ? 10 : 1);
      dynamicval<hrmap*> p(hybrid::pmap, NULL);
      dynamicval<int> psr(sightrange_bonus, 0);
      calcparam();
      reset_projection(); current_display->set_all(0);
      ptds.clear();
      drawthemap();
      drawqueue();
      displaychr(current_display->xcenter, current_display->ycenter, 0, 24, '+', 0xFFFFFFFF);
      glflush();
      });
    gmatrix = std::move(g);
    gmatrix0 = std::move(g0);
    calcparam();
    reset_projection(); current_display->set_all(0);
    }

EX }

EX namespace nisot {

  EX hyperpoint christoffel(const hyperpoint at, const hyperpoint velocity, const hyperpoint transported) {
    if(nil) return nilv::christoffel(at, velocity, transported);
    #if CAP_SOLV
    else if(solnih) return solnihv::christoffel(at, velocity, transported);
    #endif
    else if(sl2) return slr::christoffel(at, velocity, transported);
    else return point3(0, 0, 0);
    }

  EX bool in_table_range(hyperpoint h) {
    #if CAP_SOLV
    if(sol) return solnihv::in_table_range(h);
    #endif
    return true;
    }
  
  EX void geodesic_step(hyperpoint& at, hyperpoint& velocity) {
    auto acc = christoffel(at, velocity, velocity);
    
    auto at2 = at + velocity / 2;
    auto velocity2 = velocity + acc / 2;
    
    auto acc2 = christoffel(at2, velocity2, velocity2);
    
    at = at + velocity + acc2 / 2;
    
    velocity = velocity + acc;
    }
  
  EX hyperpoint numerical_exp(hyperpoint v, int steps) {
    hyperpoint at = point31(0, 0, 0);
    v /= steps;
    v[3] = 0;
    for(int i=0; i<steps; i++) geodesic_step(at, v);
    return at;
    }

  EX transmatrix parallel_transport_bare(transmatrix Pos, hyperpoint h) {
  
    h[3] = 0;
  
    auto tPos = transpose(Pos);
    
    const ld eps = 1e-4;

    if(sl2) {
      hyperpoint p = slr::to_phigans(tPos[3]);
      for(int i=0; i<3; i++)
        tPos[i] = (slr::to_phigans(tPos[3] + tPos[i] * eps) - p) / eps;
      tPos[3] = p; 
      h = transpose(tPos) * h;
      }
    else h = Pos * h;

    int steps = 100;
    h /= steps;
    
    for(int i=0; i<steps; i++) {
      for(int j=0; j<3; j++)
        tPos[j] += christoffel(tPos[3], h, tPos[j]);
      geodesic_step(tPos[3], h);
      }
                                                                                  
    if(sl2) {
      hyperpoint p = slr::from_phigans(tPos[3]);
      for(int i=0; i<3; i++)
        tPos[i] = (slr::from_phigans(tPos[3] + tPos[i] * eps) - p) / eps;
      tPos[3] = p;
      }

    return transpose(tPos);
    }

  EX void fixmatrix(transmatrix& T) {
    transmatrix push = eupush( tC0(T) );
    transmatrix push_back = inverse(push);
    transmatrix gtl = push_back * T;
    { dynamicval<eGeometry> g(geometry, gSphere); hr::fixmatrix(gtl); }
    T = push * gtl;
    }

  EX transmatrix parallel_transport(const transmatrix Position, const hyperpoint direction) {
    auto P = Position;
    nisot::fixmatrix(P);  
    if(!geodesic_movement) return inverse(eupush(Position * translate(-direction) * inverse(Position) * C0)) * Position;
    return parallel_transport_bare(P, direction);
    }
  
  EX transmatrix spin_towards(const transmatrix Position, const hyperpoint goal) {

    hyperpoint at = tC0(Position);
    transmatrix push_back = inverse(translate(at));
    hyperpoint back_goal = push_back * goal;
    back_goal = inverse_exp(back_goal, iTable);
    
    transmatrix back_Position = push_back * Position;

    return rspintox(inverse(back_Position) * back_goal);
    }
  
  EX hrmap *new_map() { 
    #if CAP_SOLV
    if(solnih) return new solnihv::hrmap_solnih;
    #endif
    if(nil) return new nilv::hrmap_nil;
    if(prod) return new product::hrmap_product;
    if(hybri) return new rots::hrmap_rotation_space;
    return NULL;
    }
  
  auto config = addHook(hooks_args, 0, [] () {
    using namespace arg;
    #if CAP_SOLV
    if(argis("-solrange")) {
      shift_arg_formula(solnihv::solrange_xy);
      shift_arg_formula(solnihv::solrange_z);
      return 0;
      }
    #endif
    if(argis("-slrange")) {
      shift_arg_formula(slr::range_xy);
      return 0;
      }
    #if CAP_SOLV
    else if(argis("-fsol")) {
      shift(); solnihv::solt.fname = args();
      return 0;
      }
    else if(argis("-nihsol")) {
      shift(); solnihv::niht.fname = args();
      return 0;
      }
    #endif
    else if(argis("-solgeo")) {
      geodesic_movement = true;
      pmodel = mdGeodesic;
      return 0;
      }
    else if(argis("-solnogeo")) {
      geodesic_movement = false;
      pmodel = mdPerspective;
      return 0;
      }
    else if(argis("-product")) {
      PHASEFROM(2);
      set_geometry(gProduct);
      return 0;
      }
    else if(argis("-s2xe")) {
      PHASEFROM(2);
      shift(); s2xe::qrings = argi();
      return 0;
      }
    else if(argis("-rotspace")) {
      PHASEFROM(2);
      set_geometry(gRotSpace);
      return 0;
      }
    else if(argis("-rot_uscale")) {
      PHASEFROM(2);
      shift_arg_formula(rots::underlying_scale);
      return 0;
      }
    else if(argis("-nilperiod")) {
      PHASEFROM(2);
      if(nil) stop_game();
      for(int a=0; a<3; a++) { shift(); nilv::nilperiod[a] = argi(); }
      nilv::set_flags();
      return 0;
      }
    else if(argis("-nilv")) {
      PHASEFROM(2);
      if(nil) stop_game();
      shift();
      ginf[gNil].sides = argi();
      return 0;
      }
    else if(argis("-catperiod")) {
      PHASEFROM(2);
      if(sol) stop_game();
      shift(); asonov::period_xy = argi();
      shift(); asonov::period_z = argi();
      asonov::set_flags();
      return 0;
      }
    else if(argis("-prodperiod")) {
      PHASEFROM(2);
      if(prod) stop_game();
      shift(); product::csteps = argi();
      hybrid::reconfigure();
      return 0;
      }
    else if(argis("-prodturn")) {
      PHASEFROM(2);
      if(prod) stop_game();
      shift(); product::cspin = argi();
      return 0;
      }
    return 1;
    });
    
  }

}
