// Hyperbolic Rogue -- nonisotropic spaces (Solv and Nil)
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file nonisotropic.cpp
 *  \brief nonisotropic spaces (Solv and Nil)
 */

#include "hyper.h"
namespace hr {

EX namespace nisot {

  #if HDR
  inline bool local_perspective_used() { return nonisotropic || gproduct; }
  #endif
  
  EX bool geodesic_movement = true;

  EX transmatrix translate(hyperpoint h, ld co IS(1)) {
    if(sl2 || sphere)
      return co > 0 ? stretch::translate(h) : stretch::itranslate(h);
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
    if(co < 0) return iso_inverse(T);
    return T;
    }

  EX }

#if !CAP_SOLV
EX namespace sn {
  EX always_false in() { return always_false(); }
EX }
#endif

#if CAP_SOLV
EX namespace sn {

  EX bool in() { return cgclass == gcSolNIH; }

  EX eGeometry geom() {
    if(asonov::in()) return gSol;
    else return geometry;
    }

  #if HDR
  typedef array<float, 3> compressed_point;
  
  inline hyperpoint decompress(compressed_point p) { return point3(p[0], p[1], p[2]); }
  inline compressed_point compress(hyperpoint h) { return make_array<float>(h[0], h[1], h[2]); }

  struct tabled_inverses {
    int PRECX, PRECY, PRECZ;
    vector<compressed_point> tab;
    string fname;
    bool loaded;
    
    void load();
    hyperpoint get(ld ix, ld iy, ld iz, bool lazy);
    
    compressed_point& get_int(int ix, int iy, int iz) { return tab[(iz*PRECY+iy)*PRECX+ix]; }
  
    GLuint texture_id;
    bool toload;
    
    GLuint get_texture_id();
  
    tabled_inverses(string s) : fname(s), texture_id(0), toload(true) {}  
    };
  #endif
  
  void tabled_inverses::load() {
    if(loaded) return;
    FILE *f = fopen(fname.c_str(), "rb");
    if(!f) f = fopen((rsrcdir + fname).c_str(), "rb");
    if(!f) { addMessage(XLAT("geodesic table missing")); pmodel = mdPerspective; return; }
    hr::ignore(fread(&PRECX, 4, 1, f));
    hr::ignore(fread(&PRECY, 4, 1, f));
    hr::ignore(fread(&PRECZ, 4, 1, f));
    tab.resize(PRECX * PRECY * PRECZ);
    hr::ignore(fread(&tab[0], sizeof(compressed_point) * PRECX * PRECY * PRECZ, 1, f));
    fclose(f);
    loaded = true;    
    }
  
  hyperpoint tabled_inverses::get(ld ix, ld iy, ld iz, bool lazy) {
    ix *= PRECX-1;
    iy *= PRECY-1;
    iz *= PRECZ-1;
    
    hyperpoint res;
    
    if(lazy) {
      if(isnan(ix) || isnan(iy) || isnan(iz)) return Hypc;
      return decompress(get_int(int(ix+.5), int(iy+.5), int(iz+.5)));
      }
    
    else {
  
      if(ix >= PRECX-1 || isnan(ix)) ix = PRECX-2;
      if(iy >= PRECX-1 || isnan(iy)) iy = PRECX-2;
      if(iz >= PRECZ-1 || isnan(iz)) iz = PRECZ-2;
      
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
    #if CAP_GL
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
    #endif
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

  EX ld ix_to_x(ld ix) {
    ld minx = 0;
    while(x_to_ix(minx) <= ix) minx++;
    ld maxx = minx; minx--;
    for(int it=0; it<20; it++) {
      ld x = (minx + maxx) / 2;
      if(x_to_ix(x) < ix) minx = x;
      else maxx = x;
      }
    return minx;
    }

  EX ld z_to_iz(ld z) { 
    z = sinh(z) / (1 + cosh(z));
    if(nih) z = (z+1) / 2;
    return z;
    }

  EX ld iz_to_z(ld iz) {
    ld minz = 0;
    while(z_to_iz(minz) <= iz) minz++;
    while(z_to_iz(minz) > iz) minz--;
    ld maxz = minz + 1;
    for(int it=0; it<20; it++) {
      ld z = (minz + maxz) / 2;
      if(z_to_iz(z) < iz) minz = z;
      else maxz = z;
      }
    return (minz+maxz) / 2;
    }

  EX hyperpoint azeq_to_table(hyperpoint x) {
    // azimuthal equidistant to Poincare
    ld r = hypot_d(3, x);
    if(r == 0) return point3(0,0,0);
    ld make_r = sinh(r) / (1 + cosh(r));
    ld d = make_r / r;
    return x * d;
    }

  EX hyperpoint table_to_azeq(hyperpoint x) {
    // Poincare to azimuthal equidistant  
    ld hr = sqhypot_d(3, x);
    if(hr < 1e-5) return x * 2;
    if(hr >= 1) return x * 60;
    ld hz = (1 + hr) / (1 - hr);
    ld d = (hz+1) * acosh(hz) / sinh(acosh(hz));    
    return x * d;
    }


  struct hrmap_solnih : hrmap {
    hrmap *binary_map;
    hrmap *ternary_map; /* nih only */
    map<pair<heptagon*, heptagon*>, heptagon*> at;
    map<heptagon*, pair<heptagon*, heptagon*>> coords;
    
    heptagon *origin;
    
    heptagon *getOrigin() override { return origin; }
    
    heptagon *get_at(heptagon *x, heptagon *y) {
      auto& h = at[make_pair(x, y)];
      if(h) return h;
      h = init_heptagon(S7);
      h->c7 = newCell(S7, h);
      coords[h] = make_pair(x, y);
      h->distance = x->distance;
      h->zebraval = x->emeraldval;
      h->emeraldval = y->emeraldval;
      return h;      
      }

    hrmap_solnih() {
    
      heptagon *alt;
      heptagon *alt3;
    
      if(true) {
        dynamicval<eGeometry> g(geometry, gBinary4); 
        alt = init_heptagon(S7);
        alt->s = hsOrigin;
        alt->alt = alt;
        binary_map = bt::new_alt_map(alt);
        }
      
      if(nih) {
        dynamicval<eGeometry> g(geometry, gTernary); 
        alt3 = init_heptagon(S7);
        alt3->s = hsOrigin;
        alt3->alt = alt3;
        ternary_map = bt::new_alt_map(alt3);
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

        default: throw hr_exception("not solnihv");
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
          default:
            throw hr_exception("wrong dir");
          }
        }

      default: throw hr_exception("wrong geometry");
      }
      }

    transmatrix adj(heptagon *h, int d) override {
      h->cmove(d); return adjmatrix(d, h->c.spin(d));
      }
    
    transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override { 
      for(int i=0; i<h1->type; i++) if(h1->move(i) == h2) return adjmatrix(i, h1->c.spin(i));
      if(gmatrix0.count(h2->c7) && gmatrix0.count(h1->c7))
        return inverse_shift(gmatrix0[h1->c7], gmatrix0[h2->c7]);
      
      transmatrix front = Id, back = Id;
      
      int up, down;

      switch(geometry) {
        case gSol: up = 2; down = 6; break;
        case gSolN: up = 4; down = 7; break;
        case gNIH: up = 4; down = 4; break;
        default: throw hr_exception("not nihsolv");
        }
      
      while(h1->distance > h2->distance) front = front * adj(h1, down), h1 = h1->cmove(down);
      while(h1->distance < h2->distance) back = iadj(h2, down) * back, h2 = h2->cmove(down);
      while(coords[h1].first != coords[h2].first) front = front * adj(h1, down), back = iadj(h2, down) * back, h1 = h1->cmove(down), h2 = h2->cmove(down);
      while(coords[h1].second != coords[h2].second) front = front * adj(h1, up), back = iadj(h2, up) * back, h1 = h1->cmove(up), h2 = h2->cmove(up);
      return front * back;
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
    "  }"
    
    "float z_to_iz_s(float z) {"
      "return sinh(z) / (1. + cosh(z));"
      "}"
    
    "float z_to_iz_ns(float z) {"
      "z = sinh(z) / (1. + cosh(z));"
      "return (z+1.)/2.;"
      "}";    

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
        throw hr_exception("christoffel not in solnihv");
      }
    }
  
  EX hyperpoint get_inverse_exp_symsol(hyperpoint h, flagtype flags) {
    auto& s = get_tabled();
    s.load();
    
    ld ix = h[0] >= 0. ? sn::x_to_ix(h[0]) : sn::x_to_ix(-h[0]);
    ld iy = h[1] >= 0. ? sn::x_to_ix(h[1]) : sn::x_to_ix(-h[1]);
    ld iz = sn::z_to_iz(h[2]);

    if(h[2] < 0.) { iz = -iz; swap(ix, iy); }
    
    hyperpoint res = s.get(ix, iy, iz, flags & pfNO_INTERPOLATION);
  
    if(h[2] < 0.) { swap(res[0], res[1]); res[2] = -res[2]; }
    if(h[0] < 0.) res[0] = -res[0];
    if(h[1] < 0.) res[1] = -res[1];
    
    if(flags & pfNO_DISTANCE) return res;
    return table_to_azeq(res);
    }

  EX hyperpoint get_inverse_exp_nsym(hyperpoint h, flagtype flags) {
    auto& s = get_tabled();
    s.load();
    
    ld ix = h[0] >= 0. ? sn::x_to_ix(h[0]) : sn::x_to_ix(-h[0]);
    ld iy = h[1] >= 0. ? sn::x_to_ix(h[1]) : sn::x_to_ix(-h[1]);
    ld iz = sn::z_to_iz(h[2]);
    
    hyperpoint res = s.get(ix, iy, iz, flags & pfNO_INTERPOLATION);
  
    if(h[0] < 0.) res[0] = -res[0];
    if(h[1] < 0.) res[1] = -res[1];
    
    if(flags & pfNO_DISTANCE) return res;
    return table_to_azeq(res);
    }

  EX string shader_symsol = sn::common +

    "vec4 inverse_exp(vec4 h) {"
    
    "float ix = h[0] >= 0. ? x_to_ix(h[0]) : x_to_ix(-h[0]);"
    "float iy = h[1] >= 0. ? x_to_ix(h[1]) : x_to_ix(-h[1]);"
    "float iz = z_to_iz_s(h[2]);"
    
    "if(h[2] < 1e-6) { iz = -iz; float s = ix; ix = iy; iy = s; }"
    "if(iz < 0.) iz = 0.;"
    
    "vec4 res;"

    "float cx = ix*(1.-1./PRECX) + .5/PRECX;"
    "float cy = iy*(1.-1./PRECY) + .5/PRECY;"
    "float cz = iz*(1.-1./PRECZ) + .5/PRECZ;"

    // "if(ix > .5 && iy > .6 && ix < iy + .05 && iz < .2 && iz < (iy - 0.5) * 0.6)"
    "\n#ifndef SOLV_ALL\n"

    "bool ok = true;"

    // hard to tell which triangles fall on the other sides
    "if(iz < .03 && ix > .65 && iy > .65) ok = false;"
    "if(iz < .013 && ix > .55 && iy > .55) ok = false;"
    "if(iz < .0075 && ix > .45 && iy > .45) ok = false;"  
    "if(iz > 0.004 && ix > 0.4 && iy > 0.4 && ix < .6 && iy < .6) ok = true;"
    "if(iz > 0.000004 && ix > 0.4 && ix < 0.7 && iy > 0.4 && iy < 0.7) ok = true;"
    "if(iz < 0.04 && ix > 0.70 && ix < 0.8 && iy > 0.5 && iy < 0.7) ok = false;"
    "if(iz < 0.05 && ix > .45 && iy > .75 && ix < .55 && iy < .95) ok = false;"
    "if(iz < 0.05 && ix > .85 && iy > .45 && iy < .75) ok = false;"
    "if(iz < 0.025 && ix > .65 && iy > .65 && ix < .8 && iy < .8) ok = false;"

    "if(!ok) res = vec4(0./0.,0./0.,0./0.,1);"
    "else "

    "\n#endif\n"
  
      "res = texture3D(tInvExpTable, vec3(cx, cy, cz));"

    "if(h[2] < 1e-6) { res.xy = res.yx; res[2] = -res[2]; }"
    "if(h[0] < 0.) res[0] = -res[0];"
    "if(h[1] < 0.) res[1] = -res[1];"
    
    "return res;"
    "}";

  EX string shader_nsymsol = sn::common + R"*(

    vec4 inverse_exp(vec4 h) {
      
    float ix = h[0] >= 0. ? x_to_ix(h[0]) : x_to_ix(-h[0]);
    float iy = h[1] >= 0. ? x_to_ix(h[1]) : x_to_ix(-h[1]);
    float iz = z_to_iz_ns(h[2]);
      
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

  EX string shader_nsym = sn::common +

    "vec4 inverse_exp(vec4 h) {"
    
    "float ix = h[0] >= 0. ? x_to_ix(h[0]) : x_to_ix(-h[0]);"
    "float iy = h[1] >= 0. ? x_to_ix(h[1]) : x_to_ix(-h[1]);"
    "float iz = z_to_iz_ns(h[2]);"
    
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

  EX tabled_inverses solt = sn::tabled_inverses("solv-geodesics.dat");
  EX tabled_inverses niht = sn::tabled_inverses("shyp-geodesics.dat");
  EX tabled_inverses sont = sn::tabled_inverses("ssol-geodesics.dat");
  
  EX tabled_inverses& get_tabled() {
    switch(geom()) {
      case gSol: return solt;
      case gNIH: return niht;
      case gSolN: return sont;
      default: throw hr_exception("not solnih");
      }
    }

  EX int approx_distance(heptagon *h1, heptagon *h2) {
    auto m = (sn::hrmap_solnih*) currentmap;
    dynamicval<eGeometry> g(geometry, gBinary4); 
    dynamicval<hrmap*> cm(currentmap, m->binary_map);
    int d1 = bt::celldistance3_approx(m->coords[h1].first, m->coords[h2].first);
    int d2 = bt::celldistance3_approx(m->coords[h1].second, m->coords[h2].second);
    return d1 + d2 - abs(h1->distance - h2->distance);
    }
  
  EX void create_faces() {
    if(geometry == gSol) {
      ld zstep = -log(2) / 2;
      ld bwh = vid.binary_width * zstep;
      auto pt = [&] (int x, int y, int z) { return xpush(bwh*x) * ypush(bwh*y) * zpush(zstep*z) * C0; };
      add_wall(0, {pt(-1,-1,-1), pt(-1,-1,+1), pt(-1,00,+1), pt(-1,+1,+1), pt(-1,+1,-1)});
      add_wall(1, {pt(-1,-1,-1), pt(00,-1,-1), pt(+1,-1,-1), pt(+1,-1,+1), pt(-1,-1,+1)});
      add_wall(2, {pt(+1,+1,-1), pt(+1,-1,-1), pt(00,-1,-1), pt(00,+1,-1)});
      add_wall(3, {pt(00,+1,-1), pt(00,-1,-1), pt(-1,-1,-1), pt(-1,+1,-1)});
      add_wall(4, {pt(+1,-1,-1), pt(+1,-1,+1), pt(+1,00,+1), pt(+1,+1,+1), pt(+1,+1,-1)});
      add_wall(5, {pt(-1,+1,-1), pt(00,+1,-1), pt(+1,+1,-1), pt(+1,+1,+1), pt(-1,+1,+1)});
      add_wall(6, {pt(-1,+1,+1), pt(+1,+1,+1), pt(+1,00,+1), pt(-1,00,+1)});
      add_wall(7, {pt(-1,00,+1), pt(+1,00,+1), pt(+1,-1,+1), pt(-1,-1,+1)});
      }
  
    if(geometry == gNIH) {
      ld zstep = .5;
      ld bwh = vid.binary_width / 6;
      auto pt = [&] (int x, int y, int z) { return xpush(bwh*x) * ypush(bwh*y) * zpush(zstep*z) * C0; };
      add_wall(0, {pt(+3,-3,-1), pt(+3,-3,+1), pt(+3,+3,+1), pt(+3,+3,-1), pt(+3,+1,-1), pt(+3,-1,-1) });
      add_wall(1, {pt(-3,+3,-1), pt(-3,+3,+1), pt(+3,+3,+1), pt(+3,+3,-1), pt(+0,+3,-1) });
      add_wall(2, {pt(-3,-3,-1), pt(-3,-3,+1), pt(-3,+3,+1), pt(-3,+3,-1), pt(-3,+1,-1), pt(-3,-1,-1) });
      add_wall(3, {pt(-3,-3,-1), pt(-3,-3,+1), pt(+3,-3,+1), pt(+3,-3,-1), pt(+0,-3,-1)});
      
      add_wall(4, {pt(-3,-3,+1), pt(-3,+3,+1), pt(+3,+3,+1), pt(+3,-3,+1)});
      
      for(int i=0; i<6; i++) {
        int x = -3 + (i%2) * 3;
        int y = -3 + (i/2) * 2;
        add_wall(5+i, {pt(x,y,-1), pt(x+3,y,-1), pt(x+3,y+2,-1), pt(x,y+2,-1)});
        }
      }
    
    if(geometry == gSolN) {
      ld zstep = -.5;
      ld bwh = vid.binary_width / 6;
      auto pt = [&] (int x, int y, int z) { return xpush(bwh*x) * ypush(bwh*y) * zpush(zstep*z) * C0; };
      add_wall(0, {pt(+3,-3,-1), pt(+3,-3,+1), pt(+3,-1,+1), pt(+3,+1,+1), pt(+3,+3,+1), pt(+3,+3,-1)});
      add_wall(1, {pt(-3,+3,-1), pt(00,+3,-1), pt(+3,+3,-1), pt(+3,+3,+1), pt(-3,+3,+1)});
      add_wall(2, {pt(-3,-3,-1), pt(-3,-3,+1), pt(-3,-1,+1), pt(-3,+1,+1), pt(-3,+3,+1), pt(-3,+3,-1)});
      add_wall(3, {pt(-3,-3,-1), pt(00,-3,-1), pt(+3,-3,-1), pt(+3,-3,+1), pt(-3,-3,+1)});
      add_wall(4, {pt(-3,+3,-1), pt(-3,-3,-1), pt(00,-3,-1), pt(00,+3,-1)});
      add_wall(5, {pt(00,+3,-1), pt(00,-3,-1), pt(+3,-3,-1), pt(+3,+3,-1)});
      add_wall(6, {pt(-3,-3,+1), pt(+3,-3,+1), pt(+3,-1,+1), pt(-3,-1,+1)});
      add_wall(7, {pt(-3,-1,+1), pt(+3,-1,+1), pt(+3,+1,+1), pt(-3,+1,+1)});
      add_wall(8, {pt(-3,+1,+1), pt(+3,+1,+1), pt(+3,+3,+1), pt(-3,+3,+1)});
      }
    
    get_hsh().compute_hept();
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
  
  EX hyperpoint get_inverse_exp(hyperpoint h, flagtype prec IS(pNORMAL)) {
    ld wmin, wmax;
    
    ld side = h[2] - h[0] * h[1] / 2;
    
    if(hypot_d(2, h) < 1e-6) return point3(h[0], h[1], h[2]);
    else if(side > 1e-6) {
      wmin = 0, wmax = TAU;
      }
    else if(side < -1e-6) {
      wmin = - TAU, wmax = 0;
      }
    else return point3(h[0], h[1], 0);
    
    ld alpha_total = h[0] ? atan(h[1] / h[0]) : 90._deg;
  
    ld b;
    if(abs(h[0]) > abs(h[1]))
      b = h[0] / 2 / cos(alpha_total);
    else
      b = h[1] / 2 / sin(alpha_total);  
    
    ld s = sin(2 * alpha_total);
    
    int max_iter = (prec & pfLOW_BS_ITER) ? 5 : 20;
    
    for(int it=0;; it++) {
      ld w = (wmin + wmax) / 2;
      ld z = b * b * (s + (sin(w) - w)/(cos(w) - 1)) + w;

      if(it == max_iter) {
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

  EX ld nilwidth = 1;
      
  hyperpoint mvec_to_point(mvec m) { return hpxy3(m[0] * nilwidth, m[1] * nilwidth, m[2] * nilwidth * nilwidth); }
  
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
    map<mvec, heptagon*> at;
    map<heptagon*, mvec> coords;
    
    heptagon *getOrigin() override { return get_at(mvec_zero); }
    
    ~hrmap_nil() {
      for(auto& p: at) clear_heptagon(p.second);
      }

    heptagon *get_at(mvec c) {
      auto& h = at[c];
      if(h) return h;
      h = init_heptagon(S7);
      h->c7 = newCell(S7, h);
      coords[h] = c;
      h->zebraval = c[0];
      h->emeraldval = c[1];
      h->fieldval = c[2];
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
  
    transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override { 
      for(int a=0; a<S7; a++) if(h2 == h1->move(a)) return adjmatrix(a);
      auto p = coords[h1].inverse() * coords[h2];
      for(int a=0; a<3; a++) p[a] = szgmod(p[a], nilperiod[a]);     
      return nisot::translate(mvec_to_point(p));
      }
    };

  EX mvec get_coord(heptagon *h) { return ((hrmap_nil*)currentmap)->coords[h]; }

  EX heptagon *get_heptagon_at(mvec m) { return ((hrmap_nil*)currentmap)->get_at(m); }

  EX void set_flags() {
    int coords = 0;
    for(int a=0; a<3; a++) if(nilperiod[a]) coords++;
    set_flag(ginf[gNil].flags, qANYQ, coords);
    set_flag(ginf[gNil].flags, qCLOSED, coords == 3);
    set_flag(ginf[gNil].flags, qSMALL, coords == 3 && nilperiod[0] * nilperiod[1] * nilperiod[2] <= 4096);
    }

  EX hyperpoint on_geodesic(hyperpoint s0, hyperpoint s1, ld x) {
    hyperpoint local = nisot::translate(s0, -1) * s1;
    hyperpoint h = get_inverse_exp(local);
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
  gamescreen();
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

EX void create_faces() {
  for(int i=0; i<S7; i++) {
    vector<hyperpoint> fvs = nilv::current_ns().facevertices[i];
    using nilv::nilwidth;
    for(auto& h: fvs) h[0] *= nilwidth, h[1] *= nilwidth, h[2] *= nilwidth * nilwidth;
    add_wall(i, fvs);
    }
  get_hsh().compute_hept();
  }
  
EX }

EX bool in_s2xe() { return gproduct && hybrid::under_class() == gcSphere; }
EX bool in_h2xe() { return gproduct && hybrid::under_class() == gcHyperbolic; }
EX bool in_e2xe() { return gproduct && hybrid::under_class() == gcEuclid; }

EX namespace hybrid {

  EX eGeometry underlying;
  EX geometry_information *underlying_cgip;

  EX eGeometryClass under_class() {
    if(embedded_plane) return geom3::ginf_backup[geometry].cclass;
    return ginf[hybrid::underlying].cclass;
    }

  EX int csteps;
  
  EX int disc_quotient = 0;
  
  EX map<heptagon*, short> altmap_heights;

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
      if(csteps && csteps != (sph ? cgi.psl_steps*2 : 0)) {
        string qplus;
        if(csteps == cgi.psl_steps)
          qplus = sph ? "elliptic" : "PSL";
        else if(csteps == 2 * cgi.psl_steps && !sph)
          qplus = "SL";
        else qplus = its(csteps);
        if(qn == "none") qn = qplus;
        else qn = qn + "/" + qplus;
        }
      if(sph) ginf[g].flags |= qELLIPTIC;
      if(csteps && csteps != cgi.psl_steps && csteps != 2*cgi.psl_steps) 
        ginf[g].flags |= qANYQ;
      }
    else {
      ginf[g].cclass = g == gRotSpace ? gcSL2 : gcProduct;
      ginf[g].g.gameplay_dimension++;
      ginf[g].g.graphical_dimension++;
      ginf[g].tiling_name += "xZ";
      if(csteps) ginf[g].flags |= qANYQ, ginf[g].tiling_name += its(csteps);
      }
    ginf[g].flags |= qHYBRID;
    }
  
  EX void reconfigure() {
    if(!mhybrid) return;
    stop_game();
    auto g = geometry;
    geometry = underlying;
    configure(g);
    geometry = g;
    }

  EX hrmap *pmap;
  EX geometry_information *pcgip;
  EX eGeometry actual_geometry;
  
  #if HDR
  template<class T> auto in_actual(const T& t) -> decltype(t()) {
    if(pmap == nullptr) return t();
    dynamicval<eGeometry> g(geometry, actual_geometry);
    dynamicval<geometry_information*> gc(cgip, pcgip);
    dynamicval<hrmap*> gu(currentmap, pmap);
    dynamicval<hrmap*> gup(pmap, NULL);
    return t();  
    }

  #define PIA(x) hr::hybrid::in_actual([&] { return (x); })
  #endif
  
  struct hrmap_hybrid : hrmap {
    
    hrmap *underlying_map;
    
    bool twisted;
    map<cell*, pair<cellwalker, cellwalker>> spins;
    
    map<pair<cell*, int>, cell*> at;
    map<cell*, pair<cell*, int>> where;
    
    heptagon *getOrigin() override { return underlying_map->getOrigin(); }

    const int SHIFT_UNKNOWN = 30000;
    map<cell*, vector<int>> shifts;
  
    EX vector<int>& make_shift(cell *c) {
      auto& res = shifts[c];
      if(res.empty()) res = vector<int> (c->type+1, SHIFT_UNKNOWN);
      return res;
      }
    
    EX int& get_shift_current(cellwalker cw) {
      return make_shift(cw.at)[cw.spin];
      }
    
    EX bool have_shift(cellwalker cw) {
      return shifts.count(cw.at) && get_shift_current(cw) != SHIFT_UNKNOWN;
      }
    
    EX int get_shift(cellwalker cw0) {
      if(S3 >= OINF) return 0;
      auto& v = get_shift_current(cw0);
      if(v != SHIFT_UNKNOWN) return v;
      
      vector<int> candidates;
      
      for(int a: {1, -1}) {
        cellwalker cw = cw0;
        cw += wstep; cw += a;
        int s = 0;
        while(cw != cw0) {
          if(!have_shift(cw)) goto next;
          s += shifts[cw.at][cw.spin];
          cw += wstep;
          cw += a;
          }
        candidates.push_back(-a * cgi.single_step * (sphere ? -1 : 1) - s);
        next: ;
        }
      
      if(candidates.size() == 2 && candidates[0] != candidates[1]) {
        int val = candidates[0] - candidates[1];
        if(disc_quotient == 0) disc_quotient = val;
        disc_quotient = gcd(val, disc_quotient);
        if(disc_quotient < 0) disc_quotient = -disc_quotient;
        }
  
      int val = 0;
      
      auto cw1 = cw0+wstep;
      if(1) {
        /* the value from PSL, helps to draw the underlying space correctly */
        auto ps = cgi.psl_steps;
        val = cw0.spin*ps / cw0.at->type - cw1.spin*ps / cw1.at->type + ps/2;
        }
      if(!candidates.empty()) val = candidates[0];
      
      v = val;
      get_shift_current(cw1) = -val;
            
      return val;
      }  
    
    EX void ensure_shifts(cell *c) {
      if(S3 >= OINF) return;
      if(!make_shift(c)[c->type]) return;
      forCellEx(c1, c)
      for(int a=0; a<c->type; a++) {
        cellwalker cw0(c, a);
        cellwalker cw = cw0;
        while(cw != cw0) {
          get_shift(cw);
          cw += wstep;
          cw += a;
          }
        }
      make_shift(c)[c->type] = 0;
      }
    
    EX int cycle_discrepancy(cellwalker cw0) {
      int total = 0;
      auto cw = cw0;
      do {
        total += get_shift(cw);
        cw += wstep;
        cw++;
        }
      while(cw != cw0);
      return total + cgi.single_step * (sphere ? -1 : 1);
      }
    
    EX void fix_bounded_cycles() {
      if(!rotspace) return;
      if(!closed_manifold) return;
      in_underlying([&] {
        cellwalker final(currentmap->gamestart(), 0);
        auto& ac = currentmap->allcells();
        for(cell *c: ac) for(int i=0; i<c->type; i++) {
          cellwalker cw(c, i);
          int cd = cycle_discrepancy(cw);
          if(!cd) continue;
          while(cw != final) {
            if(celldist(cw.peek()) < celldist(cw.at)) {
              cw += wstep;
              cw++;
              }              
            else {
              get_shift_current(cw) -= cd;
              get_shift_current(cw+wstep) += cd;
              cw++;
              }
            }
          }

        disc_quotient = abs(cycle_discrepancy(final));

        if(debugflags & DF_GEOM) for(cell *c: ac) for(int i=0; i<c->type; i++) {
          cellwalker cw(c, i);
          if(cycle_discrepancy(cw)) println(hlog, cw, cycle_discrepancy(cw));
          }
        });
      }
    
    template<class T> auto in_underlying(const T& t) -> decltype(t()) {
      pcgip = cgip; 
      dynamicval<hrmap*> gpm(pmap, this);
      dynamicval<eGeometry> gag(actual_geometry, geometry);
      dynamicval<eGeometry> g(geometry, underlying);
      dynamicval<int> gss(underlying_cgip->single_step, cgi.single_step);
      dynamicval<int> gsp(underlying_cgip->psl_steps, cgi.psl_steps);
      dynamicval<geometry_information*> gc(cgip, underlying_cgip);
      dynamicval<hrmap*> gu(currentmap, underlying_map);
      return t();
      }
    
    cell *getCell(cell *u, int h) {
      if(twisted) {
        if(!spins.count(u))
          println(hlog, "link missing: ", u);
        else {
          while(h >= csteps) h -= csteps, u = spins[u].first.at;
          while(h < 0) h += csteps, u = spins[u].second.at;
          }
        }
      h = zgmod(h, csteps);
      cell*& c = at[make_pair(u, h)];
      if(!c) { c = newCell(u->type+2, u->master); where[c] = {u, h}; }
      return c;
      }
  
    cell* gamestart() override { return getCell(underlying_map->gamestart(), 0); }
  
    hrmap_hybrid() {
      twisted = false;
      disc_quotient = 0;
      in_underlying([this] { initcells(); underlying_map = currentmap; });
      for(hrmap*& m: allmaps) if(m == underlying_map) m = NULL;
      fix_bounded_cycles();
      }
    
    ~hrmap_hybrid() {
      in_underlying([] { delete currentmap; });
      for(auto& p: at) destroy_cell(p.second);
      }
    
    void find_cell_connection(cell *c, int d) override { 
      hybrid::find_cell_connection(c, d); 
      }

    int shvid(cell *c) override {
      cell *c1 = hybrid::get_where(c).first; 
      return PIU( hr::shvid(c1) );      
      }

    int full_shvid(cell *c) override {
      cell *c1 = hybrid::get_where(c).first;
      return PIU( currentmap->full_shvid(c1) );
      }

    virtual transmatrix spin_to(cell *c, int d, ld bonus) override { if(d >= c->type-2) return Id; c = get_where(c).first; return in_underlying([&] { return currentmap->spin_to(c, d, bonus); }); }
    virtual transmatrix spin_from(cell *c, int d, ld bonus) override { if(d >= c->type-2) return Id; c = get_where(c).first; return in_underlying([&] { return currentmap->spin_from(c, d, bonus); }); }

    subcellshape& get_cellshape(cell *c) override {      
      int id = full_shvid(c);
      return generate_subcellshape_if_needed(c, id);      
      }
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
      int lev = m->where[c].second + (d == c->type-1 ? s : -s);
      cell *c1 = get_at(m->where[c].first, lev);
      c->c.connect(d, c1, c1->type - 3 + c->type - d, false);
      }
    else {
      auto cu = m->where[c].first;
      auto cu1 = m->in_underlying([&] { return cu->cmove(d); });
      int d1 = cu->c.spin(d);
      int s = 0;
      if(geometry == gRotSpace) {
        auto cm = (hrmap_hybrid*)currentmap;
        m->in_underlying([&] { cm->ensure_shifts(cu); });
        s = ((hrmap_hybrid*)currentmap)->get_shift(cellwalker(cu, d));
        }
      cell *c1 = get_at(cu1, m->where[c].second + s);
      c->c.connect(d, c1, d1, cu->c.mirror(d));
      }
    }  

  EX hrmap* get_umap() { if(!dynamic_cast<hrmap_hybrid*>(currentmap)) return nullptr; else return ((hrmap_hybrid*)currentmap)->underlying_map; }
  
  #if HDR
  template<class T> auto in_underlying_geometry(const T& f) -> decltype(f()) {
    if(!mhybrid && !gproduct) return f();
    if(embedded_plane) {
      geom3::light_flip(true);
      finalizer ff([] { geom3::light_flip(false); });
      return f();
      }
    if(geom3::flipped) throw hr_exception("called in_underlying_geometry in flipped");
    pcgip = cgip;
    dynamicval<eGeometry> gag(actual_geometry, geometry);
    dynamicval<eGeometry> g(geometry, underlying);
    dynamicval<int> gss(underlying_cgip->single_step, cgi.single_step);
    dynamicval<int> gsp(underlying_cgip->psl_steps, cgi.psl_steps);
    dynamicval<geometry_information*> gc(cgip, underlying_cgip);
    dynamicval<hrmap*> gpm(pmap, currentmap);
    dynamicval<hrmap*> gm(currentmap, get_umap());
    return f();
    }
  
  #define PIU(x) hr::hybrid::in_underlying_geometry([&] { return (x); })
  #endif

  /** like in_underlying_geometry but does not return */
  EX void switch_to_underlying() {
    if(!mhybrid && !gproduct) return;
    if(embedded_plane) throw hr_exception("switch_to_underlying in embedded_plane");
    auto m = hmap();
    pmap = m;
    actual_geometry = geometry;
    geometry = underlying;
    underlying_cgip->single_step = cgi.single_step;
    underlying_cgip->psl_steps = cgi.psl_steps;
    pcgip = cgip;
    cgip = underlying_cgip;
    currentmap = m->underlying_map;
    }

  /** like in_actual but does not return */
  EX void switch_to_actual() {
    if(!pmap) return;
    geometry = actual_geometry;
    cgip = pcgip;
    currentmap = pmap;
    pmap = nullptr;
    }

  // next: 0 = i-th corner, 1 = next corner, 2 = center of the wall
  EX hyperpoint get_corner(cell *c, int i, int next, ld z) {
    ld lev = cgi.plevel * z / 2;
    if(WDIM == 2) {
      ld zz = lerp(cgi.FLOOR, cgi.WALL, (1+z) / 2);
      hyperpoint h = orthogonal_move(get_corner_position(c, i+next), zz);
      return h;
      }
    if(gproduct) {
      dynamicval<eGeometry> g(geometry, hybrid::underlying);
      dynamicval<geometry_information*> gc(cgip, hybrid::underlying_cgip);
      dynamicval<hrmap*> gm(currentmap, ((hrmap_hybrid*)currentmap)->underlying_map);
      return scale_point(get_corner_position(c, i+next), exp(lev));
      }
    else {
      #if MAXMDIM >= 4
      ld tf, he, alpha;
      in_underlying_geometry([&] {
        hyperpoint h1 = get_corner_position(c, i);
        hyperpoint h2 = get_corner_position(c, i+1);
        hyperpoint hm;
        if(next == 2) {
          hm = h1;
          he = 0;
          }
        else {
          hyperpoint hm = mid(h1, h2);
          he = hdist(hm, h2)/2;
          if(next) he = -he;
          }
        tf = hdist0(hm)/2;
        alpha = atan2(hm[1], hm[0]);
        });
      return spin(alpha) * rots::uxpush(tf) * rots::uypush(he) * rots::uzpush(lev) * C0;
      #else
      throw hr_exception();
      #endif
      }
    }
  
  auto clear_samples = addHook(hooks_clearmemory, 40, [] () {
    for(auto& c: cgis) for(auto& v: c.second.walloffsets)
      v.second = nullptr;
    altmap_heights.clear();
    });
  
  EX vector<pair<int, cell*>> gen_sample_list() {
    if(!mhybrid && WDIM != 2 && PURE)
      return {make_pair(0, centerover), make_pair(centerover->type, nullptr)};
    vector<pair<int, cell*>> result;
    for(auto& v: cgi.walloffsets) if(v.first >= 0) result.push_back(v);
    sort(result.begin(), result.end());
    int last = 0;
    for(auto& r: result) if(r.second) last = r.first + r.second->type  + (WDIM == 2 ? 2 : 0);
    result.emplace_back(last, nullptr);
    return result;
    }

  vector<cell*> to_link;
  
  EX void will_link(cell *c) { if(pmap && ((hrmap_hybrid*) pmap)->twisted) to_link.push_back(c); }
  
  EX bool in_link = false;
  
  EX void link() {
    if(in_link) return;
    dynamicval<bool> b(in_link, true);
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

  EX int celldistance(cell *c1, cell *c2) {
    if(sl2) {
      auto w1 = hybrid::get_where(c1), w2 = hybrid::get_where(c2); 
      return PIU (hr::celldistance(w1.first, w2.first));
      }
    else if(csteps == 0) {
      auto w1 = hybrid::get_where(c1), w2 = hybrid::get_where(c2); 
      return PIU (hr::celldistance(w1.first, w2.first)) + abs(w1.second - w2.second);
      }
    else {
      int s = 0;
      int a = 999999, b = -999999;
      auto c = c1;
      do {
        auto w1 = hybrid::get_where(c), w2 = hybrid::get_where(c2); 
        if(w1.second == w2.second) {
          int d = PIU(hr::celldistance(w1.first, w2.first));
          a = min(s+d, a);
          b = max(s-d, b);
          }
        c = c->cmove(c1->type-1); s++;
        }
      while(c != c1);
      return min(a, s-b);
      }
    }

  EX void configure_period() {
    static int s;
    s = csteps / cgi.single_step;
    string str = "";
    if(rotspace)
      str = XLAT(
        "If the 2D underlying manifold is bounded, the period should be a divisor of the 'rotation space' "
        "value (PSL(2,R)) times the Euler characteristics of the underlying manifold. "
        "For unbounded underlying manifold, any value should work (theoretically, "
        "the current implementation in HyperRogue is not perfect).\n\n"
        "We won't stop you from trying illegal numbers, but they won't work correctly.");
    dialog::editNumber(s, 0, 16, 1, 0, XLAT("%1 period", "Z"), str);
    dialog::bound_low(0);
    auto set_s = [] (int s1, bool ret) {
      return [s1,ret] {
        if(ret) popScreen();
        if(csteps == s1) return;
        stop_game();
        csteps = s1 * cgi.single_step;
        hybrid::reconfigure();
        start_game();
        };
      };
    dialog::extra_options = [=] () { 
      if(rotspace) {
        int e_steps = cgi.psl_steps / gcd(cgi.single_step, cgi.psl_steps); 
        bool ubounded = PIU(closed_manifold);
        dialog::addSelItem( sphere ? XLAT("elliptic") : XLAT("PSL(2,R)"), its(e_steps), 'P');
        dialog::add_action(set_s(e_steps, true));
        dialog::addSelItem( sphere ? XLAT("sphere") : XLAT("SL(2,R)"), its(2*e_steps), 'P');
        dialog::add_action(set_s(2*e_steps, true));
        if(sl2 && !ubounded) {
          dialog::addSelItem( XLAT("universal cover"), its(0), 'P');
          dialog::add_action(set_s(0, true));
          }
        dialog::addSelItem(ubounded ? XLAT("maximum") : XLAT("works correctly so far"), its(disc_quotient), 'Q');
        dialog::add_action(set_s(disc_quotient, true));
        }
      else {
        dialog::addSelItem( XLAT("non-periodic"), its(0), 'N');
        dialog::add_action(set_s(0, true));
        }
      dialog::reaction_final = set_s(s, false);
      };
    }

EX }
  
EX namespace product {

  int z0;
  
  struct hrmap_product : hybrid::hrmap_hybrid {
    transmatrix relative_matrixc(cell *c2, cell *c1, const hyperpoint& hint) override {
      return in_underlying([&] { return calc_relative_matrix(where[c2].first, where[c1].first, hint); }) * cpush(2, cgi.plevel * szgmod(where[c2].second - where[c1].second, hybrid::csteps));
      }

    transmatrix adj(cell *c, int i) override {
      if(twisted && i == c->type-1 && where[c].second == hybrid::csteps-1) {
        auto b = spins[where[c].first].first;
        transmatrix T = cpush(2, cgi.plevel);
        T = T * spin(TAU * b.spin / b.at->type);
        if(b.mirrored) T = T * Mirror;
        return T;
        }
      if(twisted && i == c->type-2 && where[c].second == 0) {
        auto b = spins[where[c].first].second;
        transmatrix T = cpush(2, -cgi.plevel);
        T = T * spin(TAU * b.spin / b.at->type);
        if(b.mirrored) T = T * Mirror;
        return T;
        }
      if(i == c->type-1) return cpush(2, cgi.plevel);
      else if(i == c->type-2) return cpush(2, -cgi.plevel);
      c = where[c].first;
      return PIU(currentmap->adj(c, i));
      }
  
    hrmap_product() {
      current_spin_invalid = false;
      using hybrid::csteps;
      if((cspin || cmirror) && csteps) {
        in_underlying([&] {
          twisted = validate_spin();
          if(!twisted) { current_spin_invalid = true; return; }
          auto ugs = currentmap->gamestart();
          spins[ugs] = make_pair(
            cellwalker(ugs, gmod(+cspin, ugs->type), cmirror),
            cellwalker(ugs, gmod(-cspin, ugs->type), cmirror)
            );
          manual_celllister cl;
          cl.add(ugs);
          for(int i=0; i<isize(cl.lst); i++) {
            cell *c = cl.lst[i];
            hybrid::will_link(c);
            forCellEx(c2, c) cl.add(c2);
            }
          hybrid::link();
          });
        }
      }

    virtual transmatrix ray_iadj(cell *c, int i) override {
      if(i == c->type-2) return (cpush(2, +cgi.plevel));
      if(i == c->type-1) return (cpush(2, -cgi.plevel));
      transmatrix T;
      cell *cw = hybrid::get_where(c).first;
      hybrid::in_underlying_geometry([&] {
        T = currentmap->ray_iadj(cw, i);
        });
      return T;
      }
    };

  EX bool current_spin_invalid, cmirror;
  EX int cspin;
    
  /* todo might need a shiftpoint version */
  EX hyperpoint inverse_exp(hyperpoint h) {
    hyperpoint res;
    res[2] = zlevel(h);
    h = orthogonal_move(h, -res[2]);
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
    ld cd = d == 0 ? 0 : sin_auto(d) / d;
    res[0] = h[0] * cd;
    res[1] = h[1] * cd;
    res[2] = cos_auto(d);
    return orthogonal_move(res, h[2]);
    }

  EX bool validate_spin() {
    if(mproduct) return hybrid::in_underlying_geometry(validate_spin);
    if(kite::in()) return false;
    if(!quotient && !arcm::in()) return true;
    map<cell*, cellwalker> cws;
    manual_celllister cl;
    cell *start = currentmap->gamestart();
    cl.add(start);
    cws[start] = cellwalker(start, gmod(cspin, start->type), cmirror);
    for(int i=0; i<isize(cl.lst); i++) {
      cell *c = cl.lst[i];
      cellwalker cwc = cws.at(c);
      forCellIdEx(c2, j, c) {
        cellwalker cwc2 = cwc + j + wstep - c->c.spin(j);
        if(!cws.count(c2)) cws[c2] = cwc2;
        else if(cws[c2] != cwc2) return false;
        cl.add(c2);
        }
      }
    return true;
    }

  EX void show_config() {
    cmode = sm::SIDE | sm::MAYDARK;
    gamescreen();
    dialog::init(XLAT("quotient product spaces"));
    dialog::addSelItem(XLAT("%1 period", "Z"), its(hybrid::csteps), 'z');
    dialog::add_action(hybrid::configure_period);
    dialog::addSelItem(XLAT("rotation"), its(cspin), 'r');
    dialog::add_action([] {
      static int s;
      dialog::editNumber(s, 0, 16, 1, 0, XLAT("rotation", "Z"), 
        XLAT("Works if the underlying space is symmetric.")
        );
      dialog::reaction_final = [] {
        if(s == cspin) return;
        stop_game();
        cspin = s;
        start_game();
        };
      });
    dialog::addBoolItem(XLAT("reflect"), cmirror, 'f');
    dialog::add_action([]{
      stop_game();
      cmirror = !cmirror;
      start_game();
      });
    if(current_spin_invalid) 
      dialog::addInfo("the current rotation is invalid");
    else
      dialog::addBreak(100);

    dialog::addBreak(50);
    dialog::addBack();
    dialog::display();
    } 
  
EX }

EX namespace slr {

  /** in what range are we rendering SL(2,R) */
  EX ld range_xy = 2;

  /** in what Z range are we rendering SL(2,R) */
  EX ld range_z = 2;
  
  /** the number of steps for inverse_exp in the shader */
  EX int shader_iterations = 15;

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
  
  EX hyperpoint get_inverse_exp(shiftpoint h) {
    ld xy = hypot_d(2, h.h);
    ld phi = atan2(h[2], h[3]) + h.shift;

    if(xy < 1e-6) return point31(0.,0.,phi);

    bool flipped = phi > 0;
    if(flipped) phi = -phi;
    
    ld SV = stretch::not_squared();
    ld K = -1;
    
    ld alpha = flipped ? atan2(h[1], h[0]) - h.shift : atan2(h[1], -h[0]) + h.shift;
    
    hyperpoint res;
    
    ld fiber_barrier = atan(1/SV);
    
    ld flip_barrier = atan( 1 / tanh(asinh(xy)) / SV);
    
    // test the side of the flip barrier
    
    int part = -1;
    
    if(1) {
      ld kk = flip_barrier;
      
      ld x_part = cos(kk);
      ld z_part = sin(kk);
  
      ld rparam = x_part / z_part / SV;
      
      ld r = atanh(rparam);
      
      ld cr = cosh(r);
      ld sr = sinh(r);
      
      // sinh(r) = xy
      // r = tanh(sinh(xy))
      
  
      ld z = cr * (K - 1/SV/SV);
  
      ld k = 90._deg;
      ld a = k / K;
      ld zw = xy * cr / sr;
      ld u = z * a;
  
      ld phi1 = atan2(zw, cos(k)) - u;
      
      if(phi < phi1) part = 2;
      }
    
    if(part == -1) {
      ld zw = xy;
  
      ld u = xy * (K - 1/SV/SV) / K;
      ld phi1 = atan2(zw, 1) - u;
      
      if(phi > phi1) part = 0; else part = 1;
      }
    
    if(part == 2) {
      ld min_k = fiber_barrier;
      ld max_k = flip_barrier;
  
      for(int it=0; it<30; it++) {
        ld kk = (min_k + max_k) / 2;
        
        ld x_part = cos(kk);
        ld z_part = sin(kk);
        
        ld rparam = x_part / z_part / SV;
        
        assert(rparam <= 1);
  
        ld r = atanh(rparam);      
        ld cr = cosh(r);
        ld sr = sinh(r);
  
        ld z = cr * (K - 1/SV/SV);
  
        ld k = M_PI - asin(xy / sr);
        ld a = k / K;
        ld len = a * hypot(sr, cr/SV);
        ld zw = xy * cr / sr;
        ld u = z * a;
  
        ld phi1 = atan2(zw, cos(k)) - u;
  
        if(phi < phi1) max_k = kk;
        else min_k = kk;
  
        ld r_angle = alpha + u;
        res = point3(cos(r_angle) * x_part * len, -sin(r_angle) * x_part * len, z_part * len);
        }
      }
    
    if(part == 0) {
      ld min_k = 0;
      ld max_k = fiber_barrier;
  
      for(int it=0; it<30; it++) {
        ld kk = (min_k + max_k) / 2;
        
        ld x_part = cos(kk);
        ld z_part = sin(kk);
    
        ld rparam = x_part / z_part / SV;
        
        ld cr = 1 / sqrt(rparam*rparam - 1);
        ld sr = rparam * cr;
  
        ld z = cr * (K - 1/SV/SV);
        
        ld k = asinh(xy / sr);
        ld a = k / K;
        ld len = a * hypot(sr, cr/SV);
  
        ld zw = xy * cr / sr;
  
        ld u = z * a;
        ld phi1 = atan2(zw, cosh(k)) - u;
        
        if(phi > phi1) max_k = kk; else min_k = kk;
        
        ld r_angle = alpha + u;
        res = point3(cos(r_angle) * x_part * len, -sin(r_angle) * x_part * len, z_part * len);
        }
      }
    
    if(part == 1) {
      ld min_k = fiber_barrier;
      ld max_k = flip_barrier;
  
      for(int it=0; it<30; it++) {
        ld kk = (min_k + max_k) / 2;
        
        ld x_part = cos(kk);
        ld z_part = sin(kk);
    
        ld rparam = x_part / z_part / SV;
        
        ld r = atanh(rparam);      
        ld cr = cosh(r);
        ld sr = sinh(r);
  
        ld z = cr * (K - 1/SV/SV);
  
        ld k = asin(xy / sr);
        ld a = k / K;
        ld len = a * hypot(sr, cr/SV);
        ld zw = xy * cr / sr;
        ld u = z * a;
  
        ld phi1 = atan2(zw, cos(k)) - u;
        
        if(isnan(phi1)) max_k = kk;
        else if(phi > phi1) max_k = kk;
        else min_k = kk;
  
        ld r_angle = alpha + u;
        res = point3(cos(r_angle) * x_part * len, -sin(r_angle) * x_part * len, z_part * len);
        }      
      }
    
    if(flipped) res[0] *= -1, res[2] *= -1;

    return res;
    }

#if ISWEB
#define ITERATE "  for(int it=0; it<50; it++) { if(it >= uIterations) break; "
#else
#define ITERATE "  for(int it=0; it<uIterations; it++) {"
#endif

  EX string slshader = 

    "uniform mediump float uIndexSL;"
    "uniform mediump int uIterations;"
    "uniform mediump float uSV;"
    
    "vec4 inverse_exp(vec4 h) {"

      "float xy = length(h.xy);"
      "float phi = atan2(h[2], h[3]) + uIndexSL;"

      "if(xy < 1e-6) return vec4(0.,0.,phi,1.);"

      "vec4 res = vec4(sqrt(-1.),sqrt(-1.),sqrt(-1.),sqrt(-1.));"
      
      "bool flipped = phi > 0.;"
      
      "if(flipped) phi = -phi;"
      
      "float alpha = flipped ? atan2(h[1], h[0]) - uIndexSL : atan2(h[1], -h[0]) + uIndexSL;"
      
      "float fiber_barrier = atan(1./uSV);"
    
      "float flip_barrier = atan(1. / tanh(asinh(xy)) / uSV);"      
      
      "int part = 0;"
      
      "if(true) {"
        "float x_part = cos(flip_barrier);"
        "float z_part = sin(flip_barrier);"
        "float rparam = x_part / z_part / uSV;"
        "float r = atanh(rparam);"
        "float cr = cosh(r);"
        "float sr = sinh(r);"
        "float z = cr * (-1.-1./uSV/uSV);"
        "float k = PI/2.;"
        "float a = -k;"
        "float zw = xy * cr / sr;"
        "float u = z * a;"
        "float phi1 = atan2(zw, cos(k)) - u;"
        "if(phi < phi1) part = 2;"
        "}\n"
      
      "if(part == 0) {"
        "float zw = xy;"
        "float u = xy * (1. + 1./uSV/uSV);"
        "float phi1 = atan2(zw, 1.) - u;"
        "if(phi > phi1) part = 0; else part = 1;"
        "}\n"
      
      "if(part == 2) {"
        "float min_k = fiber_barrier;"
        "float max_k = flip_barrier;"
  
        ITERATE
          "float kk = (min_k + max_k) / 2.;"
          "float x_part = cos(kk);"
          "float z_part = sin(kk);"        
          "float rparam = x_part / z_part / uSV;"
          "float r = atanh(rparam);"
          "float cr = cosh(r);"
          "float sr = sinh(r);"
          
          "float z = cr * (-1. - 1./uSV/uSV);"
          "float k = PI - asin(xy / sr);"
          "float a = -k;"
          "float len = a * length(vec2(sr, cr/uSV));"
          "float zw = xy * cr / sr;"
          "float u = z * a;"
          "float phi1 = atan2(zw, cos(k)) - u;"
          "if(phi < phi1) max_k = kk; else min_k = kk;"
          "float r_angle = alpha + u;"
          "res = vec4(cos(r_angle) * x_part * len, -sin(r_angle) * x_part * len, z_part * len, 1);"
          "}"
        "}\n"
      
      "if(part == 0) {"
        "float min_k = 0.;"
        "float max_k = fiber_barrier;"
  
        ITERATE
          "float kk = (min_k + max_k) / 2.;"
          "float x_part = cos(kk);"
          "float z_part = sin(kk);"    
          "float rparam = x_part / z_part / uSV;"        
          "float cr = 1. / sqrt(rparam*rparam - 1.);"
          "float sr = rparam * cr;"  
          "float z = cr * (-1. - 1./uSV/uSV);"
          "float k = asinh(xy / sr);"
          "float a = -k;"
          "float len = a * length(vec2(sr, cr/uSV));"  
          "float zw = xy * cr / sr;"  
          "float u = z * a;"
          "float phi1 = atan2(zw, cosh(k)) - u;"
        
          "if(phi > phi1) max_k = kk; else min_k = kk;"
        
          "float r_angle = alpha + u;"
          "res = vec4(cos(r_angle) * x_part * len, -sin(r_angle) * x_part * len, z_part * len, 1);"
          "}"
        "}\n"
    
      "if(part == 1) {"
        "float min_k = fiber_barrier;"
        "float max_k = flip_barrier;"
  
        ITERATE
          "float kk = (min_k + max_k) / 2.;"
        
          "float x_part = cos(kk);"
          "float z_part = sin(kk);"
    
          "float rparam = x_part / z_part / uSV;"
        
          "float r = atanh(rparam);"
          "float cr = cosh(r);"
          "float sr = sinh(r);"
  
          "float z = cr * (-1. - 1./uSV/uSV);"
  
          "float k = asin(xy / sr);"
          "float a = -k;"
          "float len = a * length(vec2(sr, cr/uSV));"
          "float zw = xy * cr / sr;"
          "float u = z * a;"
  
          "float phi1 = atan2(zw, cos(k)) - u;"
        
          "if(phi > phi1) max_k = kk;"
          "else min_k = kk;"
  
          "float r_angle = alpha + u;"
          "res = vec4(cos(r_angle) * x_part * len, -sin(r_angle) * x_part * len, z_part * len, 1);"
          "}"
        "}\n"
    
      "if(flipped) res[0] *= -1., res[2] *= -1.;"

      "return res;"
      "}";

EX }

EX namespace rots {
  EX ld underlying_scale = 0;
  
#if MAXMDIM >= 4
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
  
  EX transmatrix lift_matrix(const transmatrix& T) {
    hyperpoint d;
    ld alpha, beta, distance;
    transmatrix Spin;
    hybrid::in_underlying_geometry([&] {
      hyperpoint h = tC0(T);
      Spin = iso_inverse(gpushxto0(h) * T);
      d = hr::inverse_exp(shiftless(h));
      alpha = atan2(Spin[0][1], Spin[0][0]);
      distance = hdist0(h);
      beta = atan2(h[1], h[0]);
      });
    for(int k=0; k<3; k++) Spin[3][k] = Spin[k][3] = 0; Spin[3][3] = 1;
    return spin(beta) * uxpush(distance/2) * spin(-beta+alpha);
    }
  
  EX std::map<int, transmatrix> saved_matrices_ray;

  struct hrmap_rotation_space : hybrid::hrmap_hybrid {

    std::map<int, transmatrix> saved_matrices;

    transmatrix adj(cell *c1, int i) override {    
      if(i == c1->type-2) return uzpush(-cgi.plevel) * spin(-2*cgi.plevel);
      if(i == c1->type-1) return uzpush(+cgi.plevel) * spin(+2*cgi.plevel);
      cell *c2 = c1->cmove(i);
      #if CAP_ARCM
      int id1 = hybrid::underlying == gArchimedean ? arcm::id_of(c1->master) + 20 * arcm::parent_index_of(c1->master) : shvid(c1);
      int id2 = hybrid::underlying == gArchimedean ? arcm::id_of(c2->master) + 20 * arcm::parent_index_of(c2->master) : shvid(c2);
      #else
      int id1 = shvid(c1);
      int id2 = shvid(c2);
      #endif
      int j = c1->c.spin(i);
      int id = id1 + (id2 << 10) + (i << 20) + (j << 26);
      auto &M = saved_matrices[id];
      if(M[3][3]) return M;
      
      cell *cw = where[c1].first;
      return M = lift_matrix(PIU(currentmap->adj(cw, i)));      
      }
    
    transmatrix relative_matrixc(cell *c2, cell *c1, const hyperpoint& hint) override { 
      if(c1 == c2) return Id;
      if(gmatrix0.count(c2) && gmatrix0.count(c1))
        return inverse_shift(gmatrix0[c1], gmatrix0[c2]);
      for(int i=0; i<c1->type; i++) if(c1->move(i) == c2) return adj(c1, i);
      return Id; // not implemented yet
      }

    transmatrix ray_iadj(cell *c1, int i) override {
      if(i == c1->type-1) return uzpush(-cgi.plevel) * spin(-2*cgi.plevel);
      if(i == c1->type-2) return uzpush(+cgi.plevel) * spin(+2*cgi.plevel);
      cell *c2 = c1->cmove(i);
      #if CAP_ARCM
      int id1 = hybrid::underlying == gArchimedean ? arcm::id_of(c1->master) + 20 * arcm::parent_index_of(c1->master) : shvid(c1);
      int id2 = hybrid::underlying == gArchimedean ? arcm::id_of(c2->master) + 20 * arcm::parent_index_of(c2->master) : shvid(c2);
      #else
      int id1 = shvid(c1);
      int id2 = shvid(c2);
      #endif
      int j = c1->c.spin(i);
      int id = id1 + (id2 << 10) + (i << 20) + (j << 26);
      auto &M = saved_matrices_ray[id];
      if(M[3][3]) return M;
      
      cell *cw = hybrid::get_where(c1).first;
      
      transmatrix T;
      hybrid::in_underlying_geometry([&] {
        hyperpoint h0 = get_corner_position(cw, i);
        hyperpoint h1 = get_corner_position(cw, (i+1));
        T = to_other_side(h0, h1);
        });
  
      return M = lift_matrix(T);
      }
    };

  /** reinterpret the given point of rotspace as a rotation matrix in the underlying geometry (note: this is the inverse) */
  EX transmatrix qtm(hyperpoint h) {

    ld& x = h[0];
    ld& y = h[1];
    ld& z = h[2];
    ld& w = h[3];
    
    ld xx = x*x;
    ld yy = y*y;
    ld zz = z*z;
    ld ww = w*w;
    
    ld xy = x*y;
    ld xz = x*z;
    ld xw = x*w;
    ld yz = y*z;
    ld yw = y*w;
    ld zw = z*w;
    
    transmatrix M;  
  
    M[0][0] = +xx - yy - zz + ww;
    M[1][1] = -xx + yy - zz + ww;
    M[2][2] = -xx - yy + zz + ww;
    
    M[0][1] = -2 * (xy + zw);
    M[1][0] = -2 * (xy - zw);
    
    M[0][2] = 2 * (xz - yw);
    M[2][0] = 2 * (xz + yw);

    M[1][2] = -2 * (yz + xw);
    M[2][1] = -2 * (yz - xw);
  
    if(hyperbolic) {
      swap(M[0][2], M[1][2]);
      swap(M[2][0], M[2][1]);
      M[1][2] *= -1;
      M[2][0] *= -1;
      M[2][2] = xx + yy + zz + ww;
      return M;
      }


    return M;
    }
    
  EX bool drawing_underlying = false;
  
  EX void draw_underlying(bool cornermode) {
    if(underlying_scale <= 0) return;
    ld d = hybrid::get_where(centerover).second;
    d *= cgi.plevel;
    transmatrix T = rots::uzpush(-d) * spin(-2*d);
  
    if(det(T) < 0) T = centralsym * T;
    
    if(mproduct) d = 0;
  
    hyperpoint h = inverse(View * spin(master_to_c7_angle()) * T) * C0;
    
    auto g = std::move(gmatrix);
    auto g0 = std::move(gmatrix0);
    
    ld alpha = atan2(ortho_inverse(NLP) * point3(1, 0, 0));
    
    bool inprod = mproduct;
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
      if(PURE && !inprod) View = View * pispin;
      View = inverse(stretch::mstretch_matrix) * spin(2*d) * View;
      dynamicval<shiftmatrix> m3(playerV, shiftless(Id));
      dynamicval<transmatrix> m4(actual_view_transform, Id);
      dynamicval<shiftmatrix> m6(cwtV, shiftless(Id));
      dynamicval<eModel> pm(pmodel, mdDisk);
      dynamicval<ld> pss(pconf.scale, (sphere ? 10 : euclid ? .4 : 1) * underlying_scale);
      dynamicval<ld> psa(pconf.alpha, sphere ? 10 : 1);
      dynamicval<hrmap*> p(hybrid::pmap, NULL);
      dynamicval<int> psr(sightrange_bonus, 0);

      dynamicval<int> psx(vid.use_smart_range, 2);
      dynamicval<ld> psy(vid.smart_range_detail, 1);
      dynamicval<bool> pdu(drawing_underlying, true);

      calcparam();
      reset_projection(); current_display->set_all(0, 0);
      ptds.clear();
      drawthemap();
      drawqueue();
      displaychr(current_display->xcenter, current_display->ycenter, 0, 24, '+', 0xFFFFFFFF);
      glflush();
      });
    gmatrix = std::move(g);
    gmatrix0 = std::move(g0);
    calcparam();
    reset_projection(); current_display->set_all(0, 0);
    }

  /** @brief exponential function for both slr and Berger sphere */

  EX hyperpoint formula_exp(hyperpoint vel) {
    bool sp = sphere;
    ld K = sp ? 1 : -1;

    if(vel[0] == 0 && vel[1] == 0 && vel[2] == 0) return C0;
  
    ld len = hypot_d(3, vel);
  
    if(vel[2] < 0) len = -len;
    
    ld z_part = vel[2]/len;
    ld x_part = sqrt(max<ld>(1 - z_part * z_part, 0));
  
    ld SV = stretch::not_squared();
    
    ld rparam = x_part / z_part / SV;
    
    ld beta = atan2(vel[1], vel[0]);
    if(len < 0) beta += M_PI;
    
    if(sl2 && rparam > 1) {  
      ld cr = 1 / sqrt(rparam*rparam - 1); // *i
      ld sr = rparam * cr;                 // *i
      
      if(z_part == 0) cr = 0, sr = 1;
  
      ld z = cr * (K - 1/SV/SV); // *i
  
      ld a = len / hypot(sr, cr/SV); // /i
  
      ld k = K*a; // /i
      ld u = z*a;
  
      ld xy = sr * sinh(k);
      ld zw = cr * sinh(k);
      
      return hyperpoint(K*xy * cos(u+beta), K*xy * sin(u+beta), zw * cos(u) - cosh(k) * sin(u), zw * sin(u) + cosh(k)*cos(u));
      }
  
    else {
      ld r = atan_auto(rparam);      
      ld cr = cos_auto(r);
      ld sr = sin_auto(r);
    
      ld z = cr * (K - 1/SV/SV);
      
      ld a = len / hypot(sr, cr/SV);
    
      ld k = K*a;
      ld u = z*a;
      
      ld xy = sr * sin(k);  
      ld zw = cr * sin(k);    
      
      return hyperpoint(K*xy * cos(u+beta), K*xy * sin(u+beta), zw * cos(u) - cos(k) * sin(u), zw * sin(u) + cos(k)*cos(u));
      }
    }

#endif
EX }

/** stretched rotation space (S3 or SLR) */
EX namespace stretch {

  EX ld factor;
  
  EX bool mstretch;
  
  EX transmatrix m_itoa, m_atoi, m_pd;
  EX ld ms_christoffel[3][3][3];
  
  EX transmatrix mstretch_matrix;
  
  EX void enable_mstretch() {
    mstretch = true;

    for(int a=0; a<4; a++)
    for(int b=0; b<4; b++)
      if(a==3 || b==3) m_atoi[a][b] = (a==b);

    m_itoa = inverse3(m_atoi);
    
    for(int a=0; a<4; a++)
    for(int b=0; b<4; b++)
      if(a==3 || b==3)
        m_itoa[a][b] = m_atoi[a][b] = 0;

    for(int j=0; j<3; j++)
    for(int k=0; k<3; k++) {
      m_pd[j][k] = 0;
      for(int i=0; i<3; i++)
        m_pd[j][k] += m_atoi[i][j] * m_atoi[i][k];
      }

    auto& c = ms_christoffel;

    ld A00 = m_pd[0][0];
    ld A11 = m_pd[1][1];
    ld A22 = m_pd[2][2];
    ld A01 = m_pd[0][1] + m_pd[1][0];
    ld A02 = m_pd[0][2] + m_pd[2][0];
    ld A12 = m_pd[2][1] + m_pd[1][2];
    ld B01 = A01 * A01;
    ld B02 = A02 * A02;
    ld B12 = A12 * A12;
    ld B00 = A00 * A00;
    ld B11 = A11 * A11;
    ld B22 = A22 * A22;
    
    ld den = (-4*A00*A11*A22 + A00*B12 + B01*A22 - A01*A02*A12 + B02*A11);
    
    if(sl2) {
      c[ 0 ][ 0 ][ 0 ] =  (A01*(A01*A12 - 2*A02*A11) - A02*(2*A01*A22 - A02*A12))/den;
      c[ 0 ][ 0 ][ 1 ] =  (A00*A01*A12 - 2*A00*A02*A11 - A01*A11*A12 + A01*A12*A22 + 2*A02*B11 + 2*A02*A11*A22 - A02*B12)/-den ;
      c[ 0 ][ 0 ][ 2 ] =  (-A01*(4*A11*A22 - B12)/2 + A12*(A01*A12 - 2*A02*A11)/2 - (A00 + A22)*(2*A01*A22 - A02*A12))/den;
      c[ 0 ][ 1 ][ 0 ] =  (A00*A01*A12 - 2*A00*A02*A11 - A01*A11*A12 + A01*A12*A22 + 2*A02*B11 + 2*A02*A11*A22 - A02*B12)/-den ;
      c[ 0 ][ 1 ][ 1 ] =  -(A01*(A01*A12 - 2*A02*A11) + A12*(4*A11*A22 - B12))/den;
      c[ 0 ][ 1 ][ 2 ] =  (B01*A22 - B02*A11 + 4*B11*A22 - A11*B12 + 4*A11*B22 - B12*A22)/-den ;
      c[ 0 ][ 2 ][ 0 ] =  (-A01*(4*A11*A22 - B12)/2 + A12*(A01*A12 - 2*A02*A11)/2 - (A00 + A22)*(2*A01*A22 - A02*A12))/den;
      c[ 0 ][ 2 ][ 1 ] =  (B01*A22 - B02*A11 + 4*B11*A22 - A11*B12 + 4*A11*B22 - B12*A22)/-den ;
      c[ 0 ][ 2 ][ 2 ] =  -(A02*(2*A01*A22 - A02*A12) + A12*(4*A11*A22 - B12))/den;
      c[ 1 ][ 0 ][ 0 ] =  (-A01*(2*A00*A12 - A01*A02) + A02*(4*A00*A22 - B02))/den;
      c[ 1 ][ 0 ][ 1 ] =  (A02*(2*A01*A22 - A02*A12)/2 + A12*(4*A00*A22 - B02)/2 + (A00 - A11)*(2*A00*A12 - A01*A02))/den;
      c[ 1 ][ 0 ][ 2 ] =  (-4*B00*A22 + A00*B02 + A00*B12 - 4*A00*B22 - B01*A22 + B02*A22)/-den ;
      c[ 1 ][ 1 ][ 0 ] =  (A02*(2*A01*A22 - A02*A12)/2 + A12*(4*A00*A22 - B02)/2 + (A00 - A11)*(2*A00*A12 - A01*A02))/den;
      c[ 1 ][ 1 ][ 1 ] =  (A01*(2*A00*A12 - A01*A02) + A12*(2*A01*A22 - A02*A12))/den;
      c[ 1 ][ 1 ][ 2 ] =  (A01*(4*A00*A22 - B02)/2 + A02*(2*A00*A12 - A01*A02)/2 + (A11 + A22)*(2*A01*A22 - A02*A12))/den;
      c[ 1 ][ 2 ][ 0 ] =  (-4*B00*A22 + A00*B02 + A00*B12 - 4*A00*B22 - B01*A22 + B02*A22)/-den ;
      c[ 1 ][ 2 ][ 1 ] =  (A01*(4*A00*A22 - B02)/2 + A02*(2*A00*A12 - A01*A02)/2 + (A11 + A22)*(2*A01*A22 - A02*A12))/den;
      c[ 1 ][ 2 ][ 2 ] =  (A02*(4*A00*A22 - B02) + A12*(2*A01*A22 - A02*A12))/den;
      c[ 2 ][ 0 ][ 0 ] =  (A01*(4*A00*A11 - B01) - A02*(2*A00*A12 - A01*A02))/den;
      c[ 2 ][ 0 ][ 1 ] =  (4*B00*A11 - A00*B01 - 4*A00*B11 + A00*B12 + B01*A11 - B02*A11)/-den ;
      c[ 2 ][ 0 ][ 2 ] =  (-A01*(A01*A12 - 2*A02*A11)/2 + A12*(4*A00*A11 - B01)/2 - (A00 + A22)*(2*A00*A12 - A01*A02))/den;
      c[ 2 ][ 1 ][ 0 ] =  (4*B00*A11 - A00*B01 - 4*A00*B11 + A00*B12 + B01*A11 - B02*A11)/-den ;
      c[ 2 ][ 1 ][ 1 ] =  -(A01*(4*A00*A11 - B01) + A12*(A01*A12 - 2*A02*A11))/den;
      c[ 2 ][ 1 ][ 2 ] =  (A00*A01*A12 + 2*A00*A02*A11 - B01*A02 + A01*A11*A12 + A01*A12*A22 - 2*A02*B11 - 2*A02*A11*A22)/-den ;
      c[ 2 ][ 2 ][ 0 ] =  (-A01*(A01*A12 - 2*A02*A11)/2 + A12*(4*A00*A11 - B01)/2 - (A00 + A22)*(2*A00*A12 - A01*A02))/den;
      c[ 2 ][ 2 ][ 1 ] =  (A00*A01*A12 + 2*A00*A02*A11 - B01*A02 + A01*A11*A12 + A01*A12*A22 - 2*A02*B11 - 2*A02*A11*A22)/-den ;
      c[ 2 ][ 2 ][ 2 ] =  -(A02*(2*A00*A12 - A01*A02) + A12*(A01*A12 - 2*A02*A11))/den;
      }
    else {
      c[ 0 ][ 0 ][ 0 ] =  (A01*(A01*A12 - 2*A02*A11) + A02*(2*A01*A22 - A02*A12))/den ;
      c[ 0 ][ 0 ][ 1 ] =  (A02*(4*A11*A22 - B12)/2 + A12*(2*A01*A22 - A02*A12)/2 - (A00 - A11)*(A01*A12 - 2*A02*A11))/den ;
      c[ 0 ][ 0 ][ 2 ] =  (-A01*(4*A11*A22 - B12)/2 + A12*(A01*A12 - 2*A02*A11)/2 - (A00 - A22)*(2*A01*A22 - A02*A12))/den ;
      c[ 0 ][ 1 ][ 0 ] =  (A02*(4*A11*A22 - B12)/2 + A12*(2*A01*A22 - A02*A12)/2 - (A00 - A11)*(A01*A12 - 2*A02*A11))/den ;
      c[ 0 ][ 1 ][ 1 ] =  (-A01*(A01*A12 - 2*A02*A11) + A12*(4*A11*A22 - B12))/den ;
      c[ 0 ][ 1 ][ 2 ] =  (B01*A22 - B02*A11 + 4*B11*A22 - A11*B12 - 4*A11*B22 + B12*A22)/(4*A00*A11*A22 - A00*B12 - B01*A22 + A01*A02*A12 - B02*A11) ;
      c[ 0 ][ 2 ][ 0 ] =  (-A01*(4*A11*A22 - B12)/2 + A12*(A01*A12 - 2*A02*A11)/2 - (A00 - A22)*(2*A01*A22 - A02*A12))/den ;
      c[ 0 ][ 2 ][ 1 ] =  (B01*A22 - B02*A11 + 4*B11*A22 - A11*B12 - 4*A11*B22 + B12*A22)/(4*A00*A11*A22 - A00*B12 - B01*A22 + A01*A02*A12 - B02*A11) ;
      c[ 0 ][ 2 ][ 2 ] =  -(A02*(2*A01*A22 - A02*A12) + A12*(4*A11*A22 - B12))/den ;
      c[ 1 ][ 0 ][ 0 ] =  -(A01*(2*A00*A12 - A01*A02) + A02*(4*A00*A22 - B02))/den ;
      c[ 1 ][ 0 ][ 1 ] =  (-A02*(2*A01*A22 - A02*A12)/2 - A12*(4*A00*A22 - B02)/2 + (A00 - A11)*(2*A00*A12 - A01*A02))/den ;
      c[ 1 ][ 0 ][ 2 ] =  (-4*B00*A22 + A00*B02 + A00*B12 + 4*A00*B22 - B01*A22 - B02*A22)/(4*A00*A11*A22 - A00*B12 - B01*A22 + A01*A02*A12 - B02*A11) ;
      c[ 1 ][ 1 ][ 0 ] =  (-A02*(2*A01*A22 - A02*A12)/2 - A12*(4*A00*A22 - B02)/2 + (A00 - A11)*(2*A00*A12 - A01*A02))/den ;
      c[ 1 ][ 1 ][ 1 ] =  (A01*(2*A00*A12 - A01*A02) - A12*(2*A01*A22 - A02*A12))/den ;
      c[ 1 ][ 1 ][ 2 ] =  (A01*(4*A00*A22 - B02)/2 + A02*(2*A00*A12 - A01*A02)/2 + (A11 - A22)*(2*A01*A22 - A02*A12))/den ;
      c[ 1 ][ 2 ][ 0 ] =  (-4*B00*A22 + A00*B02 + A00*B12 + 4*A00*B22 - B01*A22 - B02*A22)/(4*A00*A11*A22 - A00*B12 - B01*A22 + A01*A02*A12 - B02*A11) ;
      c[ 1 ][ 2 ][ 1 ] =  (A01*(4*A00*A22 - B02)/2 + A02*(2*A00*A12 - A01*A02)/2 + (A11 - A22)*(2*A01*A22 - A02*A12))/den ;
      c[ 1 ][ 2 ][ 2 ] =  (A02*(4*A00*A22 - B02) + A12*(2*A01*A22 - A02*A12))/den ;
      c[ 2 ][ 0 ][ 0 ] =  (A01*(4*A00*A11 - B01) + A02*(2*A00*A12 - A01*A02))/den ;
      c[ 2 ][ 0 ][ 1 ] =  (4*B00*A11 - A00*B01 - 4*A00*B11 - A00*B12 + B01*A11 + B02*A11)/(4*A00*A11*A22 - A00*B12 - B01*A22 + A01*A02*A12 - B02*A11) ;
      c[ 2 ][ 0 ][ 2 ] =  (-A01*(A01*A12 - 2*A02*A11)/2 + A12*(4*A00*A11 - B01)/2 - (A00 - A22)*(2*A00*A12 - A01*A02))/den ;
      c[ 2 ][ 1 ][ 0 ] =  (4*B00*A11 - A00*B01 - 4*A00*B11 - A00*B12 + B01*A11 + B02*A11)/(4*A00*A11*A22 - A00*B12 - B01*A22 + A01*A02*A12 - B02*A11) ;
      c[ 2 ][ 1 ][ 1 ] =  (-A01*(4*A00*A11 - B01) + A12*(A01*A12 - 2*A02*A11))/den ;
      c[ 2 ][ 1 ][ 2 ] =  (A00*A01*A12 + 2*A00*A02*A11 - B01*A02 + A01*A11*A12 - A01*A12*A22 - 2*A02*B11 + 2*A02*A11*A22)/(4*A00*A11*A22 - A00*B12 - B01*A22 + A01*A02*A12 - B02*A11) ;
      c[ 2 ][ 2 ][ 0 ] =  (-A01*(A01*A12 - 2*A02*A11)/2 + A12*(4*A00*A11 - B01)/2 - (A00 - A22)*(2*A00*A12 - A01*A02))/den ;
      c[ 2 ][ 2 ][ 1 ] =  (A00*A01*A12 + 2*A00*A02*A11 - B01*A02 + A01*A11*A12 - A01*A12*A22 - 2*A02*B11 + 2*A02*A11*A22)/(4*A00*A11*A22 - A00*B12 - B01*A22 + A01*A02*A12 - B02*A11) ;
      c[ 2 ][ 2 ][ 2 ] =  -(A02*(2*A00*A12 - A01*A02) + A12*(A01*A12 - 2*A02*A11))/den ;
      }
    
    for(int i=0; i<3; i++)
    for(int j=0; j<3; j++)
    for(int k=0; k<3; k++)
      if(c[i][j][k])
        println(hlog, tie(i,j,k), " : ", c[i][j][k]);
      
    
    println(hlog, "ATOI = ", m_atoi);
    println(hlog, "ITOA = ", m_itoa, " vs ", 1/not_squared());
    println(hlog, "PD   = ", m_pd, " vs ", factor);

    ray::reset_raycaster();
    }

  EX bool applicable() {
    return rotspace || (cgflags & qSTRETCHABLE);
    }

  EX bool in() {
    return (factor || mstretch) && applicable();
    }

  EX transmatrix translate(hyperpoint h) {
    if(!sphere) return slr::translate(h);
    return matrix4(
      h[3], -h[2],  h[1],  h[0],
      h[2],  h[3], -h[0],  h[1],
     -h[1],  h[0],  h[3],  h[2],
     -h[0], -h[1], -h[2],  h[3]
      );
    }
  
  EX transmatrix itranslate(hyperpoint h) {
    h[0] = -h[0];
    h[1] = -h[1];
    h[2] = -h[2];
    if(!sphere) return slr::translate(h);
    return translate(h);
    }

  hyperpoint mulz(const hyperpoint at, const hyperpoint velocity, ld zf) {
    auto vel = itranslate(at) * velocity;
    vel[2] *= zf;
    return translate(at) * vel;
    }
  
  EX ld squared() {
    return abs(1 + factor);
    }
  
  EX ld not_squared() {
    return sqrt(squared());
    }
  
  EX hyperpoint isometric_to_actual(const hyperpoint at, const hyperpoint velocity) {
    if(mstretch)
      return translate(at) * m_itoa * itranslate(at) * velocity;
    else
      return mulz(at, velocity, 1/not_squared());
    }
 
  EX hyperpoint actual_to_isometric(const hyperpoint at, const hyperpoint velocity) {
    if(mstretch)
      return translate(at) * m_atoi * itranslate(at) * velocity;
    else
      return mulz(at, velocity, not_squared());
    }
  
  EX hyperpoint christoffel(const hyperpoint at, const hyperpoint velocity, const hyperpoint transported) {
  
    auto vel = itranslate(at) * velocity;
    auto tra = itranslate(at) * transported;  
    
    hyperpoint c;
    
    if(mstretch) {    
      c = Hypc;
      for(int i=0; i<3; i++)
      for(int j=0; j<3; j++)
      for(int k=0; k<3; k++)
        c[i] += vel[j] * tra[k] * ms_christoffel[i][j][k];
      }
    
    else {    
      auto K = factor;    
      c[0] = (sphere ? -K : K+2) * (vel[1] * tra[2] + vel[2] * tra[1]);
      c[1] = (sphere ? K : -(K+2)) * (vel[0] * tra[2] + vel[2] * tra[0]);
      c[2] = 0;
      c[3] = 0;
      }

    return translate(at) * c;
    }  

  EX ld sqnorm(hyperpoint at, hyperpoint h) {
    if(sphere)
      return sqhypot_d(4, h);
    h = itranslate(at) * h;    
    return h[0] * h[0] + h[1] * h[1] + h[2] * h[2];
    }

  EX vector<hyperpoint> inverse_exp_all(hyperpoint h, int generations) {
  
    vector<hyperpoint> res;
    
    ld SV = stretch::not_squared();

    if(stretch::factor == 0) {
      ld d = hypot_d(3, h);
      if(h[3] >= 1 || h[3] <= -1|| d == 0) return res;
      ld a = acos(h[3]);
      
      res.push_back(point31(h[0] * a / d, h[1] * a / d, h[2] * a / d));
      
      a = a - TAU;

      res.push_back(point31(h[0] * a / d, h[1] * a / d, h[2] * a / d));
      
      return res;
      }
    
    if(h[0] == 0 && h[1] == 0) {
      ld a = atan2(h[2], h[3]);
      
      for(int it=-generations; it<generations; it++) {
        res.push_back(point31(0, 0, (a + TAU * it) * SV));
        }
      
      return res;
      }

    ld xy = hypot_d(2, h);
  
    ld base_min_a = asin(xy);
    ld base_max_a = M_PI - base_min_a;
  
    ld seek = 90._deg - atan2(h[3], h[2]);
  
    auto ang = [&] (ld a) {
      ld rp = xy / sin(a);
      ld co = abs(rp) >= 1 ? 0 : sqrt(1-rp*rp);
      
      return atan2(co * sin(a), cos(a)) - co * (1 - 1/SV/SV) * a;
      };
    
    for(int shift=-generations; shift<generations; shift++) {
      ld min_a = base_min_a + M_PI * shift;
      ld max_a = base_max_a + M_PI * shift;
  
      ld ang_min = ang(min_a);
      ld ang_max = ang(max_a);
      
      for(int mi=0; mi<2; mi++) {
        // 0 : minimum, 1 : maximum
        ld tl = min_a, tr = max_a;
        for(int it=0; it<20; it++) {
          ld t1 = tl * .51 + tr * .49;
          ld t2 = tl * .49 + tr * .51;
          if((ang(t1) < ang(t2)) == mi) 
            tr = t1;
          else
            tl = t2;
          }
        ld extreme = (tl + tr) / 2;
        ld ang_extreme = ang(extreme);
        for(int t=0; t<2; t++) {
          ld mmin = t == 0 ? min_a : extreme;
          ld mmax = t == 0 ? extreme : max_a;
          ld vmin = t == 0 ? ang_min : ang_extreme;
          ld vmax = t == 0 ? ang_extreme : ang_max;
          
          // make it increasing
          if(t != mi) swap(mmin, mmax), swap(vmin, vmax);
          
          // println(hlog, "*** ", mi, t, " ** ", tie(min_a, ang_min), tie(extreme, ang_extreme), tie(max_a, ang_max), " -> ", vmin, " to ", vmax);
          
          int cmin = ceil((vmin - seek) / TAU);
          int cmax = floor((vmax - seek) / TAU);
          for(int c = cmin; c <= cmax; c++) {
            ld cseek = seek + c * TAU;
  
            for(int it=0; it<40; it++) {
            
              ld a = (mmin + mmax) / 2;
              
              ld cros = ang(a);
              if(cros > cseek) mmax = a; else mmin = a;
              }
            
            ld a = (mmin + mmax) / 2;
            
            ld r = asin_clamp( xy / sin(a) );
          
            ld z_part = 1;
            ld x_part = SV * tan(r);
            
            ld db = hypot(x_part, z_part);
            x_part /= db;
            z_part /= db;
            
            ld alpha = atan2(-h[1], h[0]);
          
            ld z = cos(r) * (1 - 1/SV/SV);
            ld u = z * a;
          
            ld r_angle = alpha + u;
             
            ld len = a * hypot(sin_auto(r), cos_auto(r)/SV);
      
            auto answer = point3(cos(r_angle) * x_part * len, -sin(r_angle) * x_part * len, z_part * len);
            
            // int id = (shift << 10) + (mi << 9) + (t << 8) + c;
            
            /*
            auto f = formula_exp(answer);
            
            ld err = sqhypot_d(4, f - h);
            
            println(hlog, "************************* ", answer, ": error = ", err, " id = ", id, " params = ", tie(shift, mi, t, c));
            */
  
            res.emplace_back(answer);
            }
          }
        }    
      }
    
    return res;
    }


EX }

EX namespace nisot {

  EX hyperpoint christoffel(const hyperpoint at, const hyperpoint velocity, const hyperpoint transported) {
    if(nil) return nilv::christoffel(at, velocity, transported);
    #if CAP_SOLV
    else if(sn::in()) return sn::christoffel(at, velocity, transported);
    #endif
    else if(stretch::in() || sl2) return stretch::christoffel(at, velocity, transported);
    else return point3(0, 0, 0);
    }

  EX bool in_table_range(hyperpoint h) {
    #if CAP_SOLV
    if(sol) return sn::in_table_range(h);
    #endif
    return true;
    }

  EX hyperpoint get_acceleration(const hyperpoint& at, const hyperpoint& vel) {
    return christoffel(at, vel, vel);
    }
  
  EX void geodesic_step(hyperpoint& at, hyperpoint& vel) {
    /* RK4 method */
    auto acc1 = get_acceleration(at, vel);
    auto acc2 = get_acceleration(at + vel/2, vel + acc1/2);
    auto acc3 = get_acceleration(at + vel/2 + acc1/4, vel + acc2/2);
    auto acc4 = get_acceleration(at + vel + acc2/2, vel + acc3);
    
    at += vel + (acc1+acc2+acc3)/6;
    vel += (acc1+2*acc2+2*acc3+acc4)/6;
    }
  
  EX int rk_steps = 20;

  EX hyperpoint numerical_exp(hyperpoint v) {
    hyperpoint at = point31(0, 0, 0);
    v /= rk_steps;
    v[3] = 0;
    for(int i=0; i<rk_steps; i++) geodesic_step(at, v);
    return at;
    }

  EX transmatrix parallel_transport_bare(transmatrix Pos, hyperpoint h) {
  
    bool stretch = stretch::in() || sl2;

    h[3] = 0;
  
    if(stretch::in() && stretch::mstretch)
      Pos = stretch::mstretch_matrix * Pos;
    
    auto tPos = transpose(Pos);
    
    h = Pos * h;

    int steps = rk_steps;
    h /= steps;

    auto& at = tPos[3];
    auto& vel = h;
    
    array<ld, 4> ms;
    
    if(stretch) {
      for(int i=0; i<3; i++) {
        ms[i] = stretch::sqnorm(at, tPos[i]);
        tPos[i] = stretch::isometric_to_actual(at, tPos[i]);
        }
      ms[3] = stretch::sqnorm(at, vel);
      if(!ms[3]) return Pos;
      vel = stretch::isometric_to_actual(at, vel);
      }

    for(int i=0; i<steps; i++) {
      auto acc1 = get_acceleration(at, vel);
      auto at1 = at + vel/2; auto vel1 = vel + acc1/2;
      auto acc2 = get_acceleration(at1, vel1);
      auto at2 = at1 + acc1/4; auto vel2 = vel + acc2/2;
      auto acc3 = get_acceleration(at2, vel2);
      auto at3 = at + vel + acc2/2; auto vel3 = vel + acc3;
      auto acc4 = get_acceleration(at3, vel3);

      for(int j=0; j<3; j++) {
        auto& tra = tPos[j];
        
        auto tacc1 = christoffel(at, vel, tra);
        auto tacc2 = christoffel(at1, vel1, tra + tacc1/2);
        auto tacc3 = christoffel(at2, vel2, tra + tacc2/2);
        auto tacc4 = christoffel(at3, vel3, tra + tacc3);
        
        tra += (tacc1+tacc2*2+tacc3*2+tacc4) / 6;
        }

      at += vel + (acc1+acc2+acc3)/6;
      vel += (acc1+2*acc2+2*acc3+acc4)/6;
      
      if(stretch) {
        at = normalize(at);
        
        auto fix = [&] (hyperpoint& h, ld& m) {
          h = stretch::itranslate(at) * h;
          h[3] = 0;
          ld m1;
          if(stretch::mstretch) {
            m1 = 0;
            for(int i=0; i<3; i++) for(int j=0; j<3; j++)
              m1 += h[i] * stretch::m_pd[i][j] * h[j];
            }
          else
            m1 = h[0] * h[0] + h[1] * h[1] + h[2] * h[2] * stretch::squared();
          h /= sqrt(m1/m);
          h = stretch::translate(at) * h;
          };

        for(int i=0; i<3; i++) fix(tPos[i], ms[i]);
        fix(vel, ms[3]); 
        }

      }

    if(stretch) {
      vel = stretch::actual_to_isometric(at, vel);
      for(int i=0; i<3; i++) tPos[i] = stretch::actual_to_isometric(at, tPos[i]);
      }

    Pos = transpose(tPos);
    
    if(stretch::in() && stretch::mstretch)
      Pos = inverse(stretch::mstretch_matrix) * Pos;
    
    return Pos;
    }

  EX void fixmatrix(transmatrix& T) {
    if(sphere) return hr::fixmatrix(T);
    transmatrix push = eupush( tC0(T) );
    transmatrix push_back = eupush(tC0(T), -1);
    transmatrix gtl = push_back * T;
    fix_rotation(gtl);
    T = push * gtl;
    }

  EX transmatrix parallel_transport(const transmatrix Position, const hyperpoint direction) {
    auto P = Position;
    nisot::fixmatrix(P);  
    if(!geodesic_movement) return eupush(Position * translate(-direction) * inverse(Position) * C0, -1) * Position;
    return parallel_transport_bare(P, direction);
    }
  
  EX transmatrix spin_towards(const transmatrix Position, const hyperpoint goal, flagtype prec IS(pNORMAL)) {

    hyperpoint at = tC0(Position);
    transmatrix push_back = translate(at, -1);
    hyperpoint back_goal = push_back * goal;
    back_goal = inverse_exp(shiftless(back_goal), prec);
    
    transmatrix back_Position = push_back * Position;

    return rspintox(inverse(back_Position) * back_goal);
    }
  
  EX hrmap *new_map() { 
    #if CAP_SOLV
    if(sn::in()) return new sn::hrmap_solnih;
    #endif
    if(mproduct) return new product::hrmap_product;
    #if MAXMDIM >= 4
    if(nil) return new nilv::hrmap_nil;
    if(mhybrid) return new rots::hrmap_rotation_space;
    #endif
    return NULL;
    }
  
  #if CAP_COMMANDLINE
  auto config = addHook(hooks_args, 0, [] () {
    using namespace arg;
    #if CAP_SOLV
    if(argis("-solrange")) {
      shift_arg_formula(sn::solrange_xy);
      shift_arg_formula(sn::solrange_z);
      return 0;
      }
    #endif
    if(argis("-slrange")) {
      shift_arg_formula(slr::range_xy);
      shift_arg_formula(slr::range_z);
      return 0;
      }
    #if CAP_SOLV
    else if(argis("-fsol")) {
      shift(); sn::solt.fname = args();
      return 0;
      }
    else if(argis("-nihsol")) {
      shift(); sn::niht.fname = args();
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
    else if(argis("-nilwidth")) {
      PHASEFROM(2);
      shift_arg_formula(nilv::nilwidth);
      return 0;
      }
    else if(argis("-nilh")) {
      PHASEFROM(2);
      stop_game();
      shift(); ginf[gNil].sides = argi();
      nilv::set_flags();
      start_game();
      }
    else if(argis("-rk-steps")) {
      PHASEFROM(2);
      shift(); rk_steps = argi();
      return 0;
      }      
    else if(argis("-nilv")) {
      PHASEFROM(2);
      if(nil) stop_game();
      shift();
      ginf[gNil].sides = argi();
      return 0;
      }
    #if CAP_SOLV
    else if(argis("-catperiod")) {
      PHASEFROM(2);
      if(sol) stop_game();
      shift(); asonov::period_xy = argi();
      shift(); asonov::period_z = argi();
      asonov::set_flags();
      return 0;
      }
    #endif
    else if(argis("-prodperiod")) {
      PHASEFROM(2);
      if(mproduct) stop_game();
      shift(); hybrid::csteps = argi();
      hybrid::reconfigure();
      return 0;
      }
    else if(argis("-rot-stretch")) {
      PHASEFROM(2);
      shift_arg_formula(stretch::factor, ray::reset_raycaster);
      return 0;
      }
    else if(argis("-mstretch")) {
      PHASEFROM(2);
      auto& M = stretch::m_atoi;
      M = Id;
      stretch::enable_mstretch();
      while(true) {
        shift();
        string s = args();
        if(isize(s) == 2 && among(s[0], 'a', 'b','c') && among(s[1], 'a', 'b', 'c'))
          shift_arg_formula(M[s[0]-'a'][s[1]-'a'], stretch::enable_mstretch);
        else break;
        }
      // shift_arg_formula(stretch::yfactor, ray::reset_raycaster);
      return 0;
      }
    else if(argis("-mstretch1")) {
      PHASEFROM(2);
      auto& M = stretch::m_atoi;
      M = Id;
      M[2][2] = stretch::not_squared();
      stretch::enable_mstretch();
      // shift_arg_formula(stretch::yfactor, ray::reset_raycaster);
      return 0;
      }
    else if(argis("-prodturn")) {
      PHASEFROM(2);
      if(mproduct) stop_game();
      shift(); product::cspin = argi();
      shift(); product::cmirror = argi();
      return 0;
      }
    return 1;
    });
  #endif  
  }

}
