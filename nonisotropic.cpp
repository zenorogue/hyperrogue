// Hyperbolic Rogue -- nonisotropic spaces (Solv and Nil)
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file nonisotropic.cpp
 *  \brief nonisotropic spaces (Solv and Nil)
 */

namespace hr {

EX namespace nisot {
  typedef array<float, 3> ptlow;

  EX transmatrix local_perspective;
  #if HDR
  inline bool local_perspective_used() { return nonisotropic || prod; }
  #endif
  
  EX bool geodesic_movement = true;

  EX transmatrix translate(hyperpoint h) {
    transmatrix T = Id;
    for(int i=0; i<GDIM; i++) T[i][LDIM] = h[i];
    if(sol) {
      T[0][0] = exp(-h[2]);
      T[1][1] = exp(+h[2]);
      }
    if(nil) 
      T[2][1] = h[0];
    return T;
    }

  EX }

EX namespace solv {
  
  int PRECX, PRECY, PRECZ;
  
  vector<nisot::ptlow> inverse_exp_table;
  
  bool table_loaded;
  
  EX string solfname = "solv-geodesics.dat";
  
  EX void load_table() {
    if(table_loaded) return;
    FILE *f = fopen(solfname.c_str(), "rb");
    // if(!f) f = fopen("/usr/lib/soltable.dat", "rb");
    if(!f) { addMessage(XLAT("geodesic table missing")); pmodel = mdPerspective; return; }
    fread(&PRECX, 4, 1, f);
    fread(&PRECY, 4, 1, f);
    fread(&PRECZ, 4, 1, f);
    inverse_exp_table.resize(PRECX * PRECY * PRECZ);
    fread(&inverse_exp_table[0], sizeof(nisot::ptlow) * PRECX * PRECY * PRECZ, 1, f);
    fclose(f);
    table_loaded = true;    
    }
  
  hyperpoint christoffel(const hyperpoint at, const hyperpoint velocity, const hyperpoint transported) {
    return hpxyz3(
      -velocity[2] * transported[0] - velocity[0] * transported[2],
       velocity[2] * transported[1] + velocity[1] * transported[2],
       velocity[0] * transported[0] * exp(2*at[2]) - velocity[1] * transported[1] * exp(-2*at[2]),
       0
       );
    }
  
  ld x_to_ix(ld u) {
    if(u == 0.) return 0.;
    ld diag = u*u/2.;
 
    ld x = diag;
    ld y = u;
    ld z = diag+1.;
  
    x /= (1.+z);
    y /= (1.+z);
  
    return 0.5 - atan((0.5-x) / y) / M_PI;
    }

  hyperpoint get_inverse_exp(hyperpoint h, bool lazy, bool just_direction) {
    load_table();
    
    ld ix = h[0] >= 0. ? x_to_ix(h[0]) : x_to_ix(-h[0]);
    ld iy = h[1] >= 0. ? x_to_ix(h[1]) : x_to_ix(-h[1]);
    ld iz = tanh(h[2]);
    
    if(h[2] < 0.) { iz = -iz; swap(ix, iy); }
    
    ix *= PRECX-1;
    iy *= PRECY-1;
    iz *= PRECZ-1;
    
    hyperpoint res = C0;

    if(lazy) {
      auto r = inverse_exp_table[(int(iz)*PRECY+int(iy))*PRECX+int(ix)];
      for(int i=0; i<3; i++) res[i] = r[i];
      }
    
    else {

      if(ix >= PRECX-1) ix = PRECX-2;
      if(iy >= PRECX-1) iy = PRECX-2;
      if(iz >= PRECZ-1) iz = PRECZ-2;
      
      int ax = ix, bx = ax+1;
      int ay = iy, by = ay+1;
      int az = iz, bz = az+1;
      
      #define S0(x,y,z) inverse_exp_table[(z*PRECY+y)*PRECX+x][t]
      #define S1(x,y) (S0(x,y,az) * (bz-iz) + S0(x,y,bz) * (iz-az))
      #define S2(x) (S1(x,ay) * (by-iy) + S1(x,by) * (iy-ay))
  
      for(int t=0; t<3; t++)
        res[t] = S2(ax) * (bx-ix) + S2(bx) * (ix-ax);
      }
  
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

  struct hrmap_sol : hrmap {
    hrmap *binary_map;
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

    hrmap_sol() {
    
      heptagon *alt;
    
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
      
      origin = get_at(alt, alt);
      }
    
    heptagon *altstep(heptagon *h, int d) {
      dynamicval<eGeometry> g(geometry, gBinary4); 
      dynamicval<hrmap*> cm(currentmap, binary_map);
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

      switch(d) {
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
      }

    ~hrmap_sol() {
      delete binary_map;
      for(auto& p: at) clear_heptagon(p.second);
      }
    
    transmatrix adjmatrix(int i, int j) {
      ld z = log(2);
      ld bw = vid.binary_width * z;
      ld bwh = bw / 4;
      switch(i) {
        case 0: return xpush(+bw);
        case 1: return ypush(+bw);
        case 2: return xpush(-bwh) * zpush(+z) * ypush(j == 6 ? +bwh : -bwh);
        case 3: return xpush(+bwh) * zpush(+z) * ypush(j == 6 ? +bwh : -bwh);
        case 4: return xpush(-bw);
        case 5: return ypush(-bw);
        case 6: return ypush(-bwh) * zpush(-z) * xpush(j == 2 ? +bwh : -bwh);
        case 7: return ypush(+bwh) * zpush(-z) * xpush(j == 2 ? +bwh : -bwh);
        default:return Id;
        }
      }
    
    virtual transmatrix relative_matrix(heptagon *h2, heptagon *h1) override { 
      for(int i=0; i<h1->type; i++) if(h1->move(i) == h2) return adjmatrix(i, h1->c.spin(i));
      if(gmatrix0.count(h2->c7) && gmatrix0.count(h1->c7))
        return inverse(gmatrix0[h1->c7]) * gmatrix0[h2->c7];
      return Id; // not implemented yet
      }

    void draw() override {
      dq::visited.clear();

      dq::enqueue(viewctr.at, cview());
      
      while(!dq::drawqueue.empty()) {
        auto& p = dq::drawqueue.front();
        heptagon *h = get<0>(p);
        transmatrix V = get<1>(p);
        dq::drawqueue.pop();
        
        cell *c = h->c7;
        if(!do_draw(c, V)) continue;
        drawcell(c, V, 0, false);
  
        for(int i=0; i<S7; i++) {
          // note: need do cmove before c.spin
          heptagon *h1 = h->cmove(i);
          dq::enqueue(h1, V * adjmatrix(i, h->c.spin(i)));
          }
        }
      }

    };

  EX pair<heptagon*,heptagon*> getcoord(heptagon *h) {
    return ((hrmap_sol*)currentmap)->coords[h];
    }

  EX heptagon *get_at(heptagon *h1, heptagon *h2, bool gen) {
    auto m = ((hrmap_sol*)currentmap);
    if(!gen && !m->at.count(make_pair(h1, h2))) return nullptr;
    return m->get_at(h1, h2);
    }
  
  EX ld solrange_xy = 15;
  EX ld solrange_z = 4;
  
  EX ld glitch_xy = 2, glitch_z = 0.6;

  EX bool in_table_range(hyperpoint h) {
    if(abs(h[0]) > glitch_xy && abs(h[1]) > glitch_xy && abs(h[2]) < glitch_z) return false;
    return abs(h[0]) < solrange_xy && abs(h[1]) < solrange_xy && abs(h[2]) < solrange_z;
    }

  EX int approx_distance(heptagon *h1, heptagon *h2) {
    auto m = (hrmap_sol*) currentmap;
    dynamicval<eGeometry> g(geometry, gBinary4); 
    dynamicval<hrmap*> cm(currentmap, m->binary_map);
    int d1 = binary::celldistance3_approx(m->coords[h1].first, m->coords[h2].first);
    int d2 = binary::celldistance3_approx(m->coords[h1].second, m->coords[h2].second);
    return d1 + d2 - abs(h1->distance - h2->distance);
    }
  
  EX string solshader = 
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


    "vec4 inverse_exp(vec4 h) {"
    
    "float ix = h[0] >= 0. ? x_to_ix(h[0]) : x_to_ix(-h[0]);"
    "float iy = h[1] >= 0. ? x_to_ix(h[1]) : x_to_ix(-h[1]);"
    "float iz = tanh(h[2]);"
    
    "if(h[2] < 1e-6) { iz = -iz; float s = ix; ix = iy; iy = s; }"
    "if(iz < 0.) iz = 0.;"
  
    "vec4 res = texture3D(tInvExpTable, vec3(ix*(1.-1./PRECX) + 0.5/PRECX, iy*(1.-1./PRECY) + .5/PRECY, iz*(1.-1./PRECZ) + .5/PRECZ));"
  
    "if(h[2] < 1e-6) { res.xy = res.yx; res[2] = -res[2]; }"
    "if(h[0] < 0.) res[0] = -res[0];"
    "if(h[1] < 0.) res[1] = -res[1];"
    
    "return res;"
    "}";
EX }

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

  static const mvec mvec_zero = mvec(0, 0, 0);

  hyperpoint mvec_to_point(mvec m) { return hpxy3(m[0], m[1], m[2]); }
  
  #if HDR
  static const int nilv_S7 = 8;
  #endif

  array<mvec, nilv_S7> movevectors = {{ mvec(-1,0,0), mvec(-1,0,1), mvec(0,-1,0), mvec(0,0,-1), mvec(1,0,0), mvec(1,0,-1), mvec(0,1,0), mvec(0,0,1) }};

  EX array<vector<hyperpoint>, nilv_S7> facevertices = {{
    { point31(-0.5,-0.5,-0.25), point31(-0.5,-0.5,0.75), point31(-0.5,0.5,-0.25), },
    { point31(-0.5,-0.5,0.75), point31(-0.5,0.5,0.75), point31(-0.5,0.5,-0.25), },
    { point31(-0.5,-0.5,-0.25), point31(-0.5,-0.5,0.75), point31(0.5,-0.5,0.25), point31(0.5,-0.5,-0.75), },
    { point31(-0.5,-0.5,-0.25), point31(-0.5,0.5,-0.25), point31(0.5,0.5,-0.75), point31(0.5,-0.5,-0.75), },
    { point31(0.5,0.5,0.25), point31(0.5,-0.5,0.25), point31(0.5,-0.5,-0.75), },
    { point31(0.5,0.5,-0.75), point31(0.5,0.5,0.25), point31(0.5,-0.5,-0.75), },
    { point31(-0.5,0.5,0.75), point31(-0.5,0.5,-0.25), point31(0.5,0.5,-0.75), point31(0.5,0.5,0.25), },
    { point31(-0.5,-0.5,0.75), point31(-0.5,0.5,0.75), point31(0.5,0.5,0.25), point31(0.5,-0.5,0.25), },
    }};
  
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
      auto q = p * movevectors[d];
      auto child = get_at(q);
      parent->c.connect(d, child, (d + nilv_S7/2) % nilv_S7, false);
      return child;
      }

    transmatrix adjmatrix(int i) {
      return nisot::translate(mvec_to_point(movevectors[i]));
      }
    
    virtual transmatrix relative_matrix(heptagon *h2, heptagon *h1) override { 
      return nisot::translate(mvec_to_point(coords[h1].inverse() * coords[h2]));
      }

    void draw() override {
      dq::visited.clear();

      dq::enqueue(viewctr.at, cview());
      
      while(!dq::drawqueue.empty()) {
        auto& p = dq::drawqueue.front();
        heptagon *h = get<0>(p);
        transmatrix V = get<1>(p);
        dq::drawqueue.pop();
        
        cell *c = h->c7;
        if(!do_draw(c, V)) continue;
        drawcell(c, V, 0, false);

        if(0) for(int t=0; t<c->type; t++) {
          if(!c->move(t)) continue;
          dynamicval<color_t> g(poly_outline, darkena((0x142968*t) & 0xFFFFFF, 0, 255) );
          queuepoly(V, cgi.shWireframe3D[t], 0);
          }
  
        for(int i=0; i<S7; i++) {
          // note: need do cmove before c.spin
          heptagon *h1 = h->cmove(i);
          dq::enqueue(h1, V * adjmatrix(i));
          }
        }
      }

    };

  EX hyperpoint on_geodesic(hyperpoint s0, hyperpoint s1, ld x) {
    hyperpoint local = inverse(nisot::translate(s0)) * s1;
    hyperpoint h = get_inverse_exp(local, 100);
    return nisot::translate(s0) * formula_exp(h * x);
    }
EX }

EX namespace product {

  EX eGeometry underlying;
  EX geometry_information *underlying_cgip;
  
  void configure() {
    check_cgi();
    cgi.prepare_basics();
    underlying = geometry;
    underlying_cgip = cgip;
    geometry = gProduct;
    ginf[gProduct] = ginf[underlying];
    ginf[gProduct].cclass = gcProduct;
    }
  
  EX int current_view_level;
  
  hrmap *pmap;
  geometry_information *pcgip;
  
  template<class T> auto in_actual(const T& t) -> decltype(t()) {
    dynamicval<eGeometry> g(geometry, gProduct);
    dynamicval<geometry_information*> gc(cgip, pcgip);
    dynamicval<hrmap*> gu(currentmap, pmap);
    dynamicval<hrmap*> gup(pmap, NULL);
    return t();  
    }
  
  struct hrmap_product : hrmap {
    
    hrmap *underlying_map;
    
    map<pair<cell*, int>, cell*> at;
    map<cell*, pair<cell*, int>> where;
    
    heptagon *getOrigin() override { return underlying_map->getOrigin(); }
    
    template<class T> auto in_underlying(const T& t) -> decltype(t()) {
      pcgip = cgip; 
      dynamicval<hrmap*> gpm(pmap, this);
      dynamicval<eGeometry> g(geometry, underlying);
      dynamicval<geometry_information*> gc(cgip, underlying_cgip);
      dynamicval<hrmap*> gu(currentmap, underlying_map);
      return t();
      }
    
    cell *getCell(cell *u, int h) {
      cell*& c = at[make_pair(u, h)];
      if(!c) { c = newCell(u->type+2, u->master); where[c] = {u, h}; }
      return c;
      }
  
    cell* gamestart() override { return getCell(underlying_map->gamestart(), 0); }
  
    transmatrix relative_matrix(cell *c2, cell *c1, const hyperpoint& point_hint) override {
      return in_underlying([&] { return calc_relative_matrix(where[c2].first, where[c1].first, point_hint); }) * mscale(Id, cgi.plevel * (where[c2].second - where[c1].second));
      }
  
    hrmap_product() {
      in_underlying([this] { initcells(); underlying_map = currentmap; });
      for(hrmap*& m: allmaps) if(m == underlying_map) m = NULL;
      }
    
    ~hrmap_product() {
      in_underlying([this] { delete currentmap; });
      for(auto& p: at) tailored_delete(p.second);
      }
  
    void draw() override {
      in_underlying([this] { currentmap->draw(); });
      }
    };
  
  EX cell *get_at(cell *base, int level) {
    return ((hrmap_product*)currentmap)->getCell(base, level);
    }
  
  EX pair<cell*, int> get_where(cell *c) { return ((hrmap_product*)currentmap)->where[c]; }
  
  void drawcell_stack(cell *c, transmatrix V, int spinv, bool mirrored) {
    if(sphere) gmatrix[c] = V; /* some computations need gmatrix0 for underlying geometry */
    in_actual([&] { 
      int flat_distance = hdist0(product_decompose(tC0(V)).second);
      int max_z = flat_distance > sightranges[gProduct] ? 0 : sqrt(sightranges[gProduct] * sightranges[gProduct] - flat_distance * flat_distance) + 1;
      for(int z=-max_z; z<=max_z; z++) {
        cell *c1 = get_at(c, current_view_level+z);
        setdist(c1, 7, NULL);
        drawcell(c1, V * mscale(Id, cgi.plevel * z), spinv, mirrored); 
        }
      });
    }
  
  void find_cell_connection(cell *c, int d) {
    auto m = (hrmap_product*)currentmap;
    if(d >= c->type - 2) {
      cell *c1 = get_at(m->where[c].first, m->where[c].second + (d == c->type-1 ? 1 : -1));
      c->c.connect(d, c1, c1->type - 3 + c->type - d, false);
      }
    else {
      auto cu = m->where[c].first;
      auto cu1 = m->in_underlying([&] { return cu->cmove(d); });
      cell *c1 = get_at(cu1, m->where[c].second);
      c->c.connect(d, c1, cu->c.spin(d), cu->c.mirror(d));
      }
    }
  
  EX hyperpoint get_corner(cell *c, int i, ld z) {
    ld lev = cgi.plevel * z / 2;
    dynamicval<eGeometry> g(geometry, underlying);
    dynamicval<geometry_information*> gc(cgip, underlying_cgip);
    return mscale(get_corner_position(c, i), exp(lev));
    }
  
  EX int wall_offset(cell *c) {
    int id = underlying == gArchimedean ? arcm::id_of(c->master) + 20 * arcm::parent_index_of(c->master) : shvid(c);
    if(isize(cgi.walloffsets) <= id) cgi.walloffsets.resize(id+1, -1);
    int &wo = cgi.walloffsets[id];
    if(wo == -1) {
      cell *c1 = get_where(c).first;
      wo = isize(cgi.shWall3D);
      int won = wo + c->type;
      cgi.shWall3D.resize(won);
      cgi.shPlainWall3D.resize(won);
      cgi.shWireframe3D.resize(won);
      cgi.shMiniWall3D.resize(won);

      for(int i=0; i<c1->type; i++)
       cgi.make_wall(wo + i, {product::get_corner(c1, i, -1), product::get_corner(c1, i, +1), product::get_corner(c1, i+1, +1), product::get_corner(c1, i+1, -1)});
      for(int a: {0,1}) {
        vector<hyperpoint> l;
        int z = a ? 1 : -1;
        for(int i=0; i<c1->type; i++)
          l.push_back(product::get_corner(c1, i, z));
        cgi.make_wall(won-2+a, l);
        }

      cgi.compute_cornerbonus();
      cgi.extra_vertices();
      }
    return wo;
    }

  EX bool product_sphere() { return ginf[underlying].cclass == gcSphere; }
  
  EX hyperpoint inverse_exp(hyperpoint h) {
    hyperpoint res;
    res[2] = zlevel(h);
    h = zshift(h, -res[2]);
    ld r = hypot_d(2, h);
    if(r < 1e-6) {
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
    res[2] = cosh(d);
    return zshift(res, h[2]);
    }
  
  EX void in_underlying_map(const reaction_t& f) {
    if(!prod) f();
    else ((hrmap_product*)currentmap)->in_underlying(f);
    }

  #if HDR
  template<class T> auto in_underlying_geometry(const T& f) -> decltype(f()) {
    if(!prod) return f();
    dynamicval<eGeometry> g(geometry, underlying);
    dynamicval<geometry_information*> gc(cgip, underlying_cgip);
    return f();
    }
  
  #define PIU(x) hr::product::in_underlying_geometry([&] { return (x); })
  #endif
  
EX }

EX namespace nisot {

  EX hyperpoint christoffel(const hyperpoint at, const hyperpoint velocity, const hyperpoint transported) {
    if(sol) return solv::christoffel(at, velocity, transported);
    else if(nil) return nilv::christoffel(at, velocity, transported);
    else return point3(0, 0, 0);
    }

  EX bool in_table_range(hyperpoint h) {
    if(sol) return solv::in_table_range(h);
    return true;
    }
  
  #if HDR
  enum iePrecision { iLazy, iTable };
  #endif
  
  EX hyperpoint inverse_exp(const hyperpoint h, iePrecision p, bool just_direction IS(true)) {
    if(sol) return solv::get_inverse_exp(h, p == iLazy, just_direction);
    if(nil) return nilv::get_inverse_exp(h, p == iLazy ? 5 : 20);
    if(prod) return product::inverse_exp(h);
    return point3(h[0], h[1], h[2]);
    }
  
  EX ld geo_dist(const hyperpoint h1, const hyperpoint h2, iePrecision p) {
    if(!nonisotropic) return hdist(h1, h2);
    return hypot_d(3, inverse_exp(inverse(translate(h1)) * h2, p, false));
    }

  EX void geodesic_step(hyperpoint& at, hyperpoint& velocity) {
    auto acc = christoffel(at, velocity, velocity);
    
    auto at2 = at + velocity / 2;
    auto velocity2 = velocity + acc / 2;
    
    auto acc2 = christoffel(at2, velocity2, velocity2);
    
    at = at + velocity + acc2 / 2;
    
    velocity = velocity + acc;
    }
  
  EX hyperpoint direct_exp(hyperpoint v, int steps) {
    hyperpoint at = point31(0, 0, 0);
    v /= steps;
    v[3] = 0;
    for(int i=0; i<steps; i++) geodesic_step(at, v);
    return at;
    }

  EX hyperpoint get_exp(hyperpoint v, int steps) {
    if(sol) return direct_exp(v, steps);
    if(nil) return nilv::formula_exp(v);
    if(prod) return product::direct_exp(v);
    return v;
    }

  EX transmatrix parallel_transport_bare(transmatrix Pos, transmatrix T) {
  
    hyperpoint h = tC0(T);
    h[3] = 0;
  
    h = Pos * h;
    
    int steps = 100;
    h /= steps;
  
    auto tPos = transpose(Pos);
  
    for(int i=0; i<steps; i++) {
      for(int j=0; j<3; j++)
        tPos[j] += christoffel(tPos[3], h, tPos[j]);
      geodesic_step(tPos[3], h);
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

  EX transmatrix parallel_transport(const transmatrix Position, const transmatrix LPe, const transmatrix T) {
    if(prod) {
      hyperpoint h = product::direct_exp(inverse(LPe) * product::inverse_exp(tC0(T)));
      return Position * rgpushxto0(h);
      }
    auto P = Position;
    nisot::fixmatrix(P);  
    if(!geodesic_movement) return inverse(eupush(Position * inverse(T) * inverse(Position) * C0)) * Position;
    return parallel_transport_bare(P, T);
    }
  
  EX transmatrix transport_view(const transmatrix T, const transmatrix LPe, const transmatrix V) {
    if(prod) {
      hyperpoint h = product::direct_exp(inverse(LPe) * product::inverse_exp(tC0(T)));
      return rgpushxto0(h) * V;
      }
    if(!geodesic_movement) {
      transmatrix IV = inverse(V);
      nisot::fixmatrix(IV);
      const transmatrix V1 = inverse(IV);
      return V1 * eupush(IV * T * V1 * C0);
      }
    return inverse(parallel_transport(inverse(V), Id, inverse(T)));
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
    if(sol) return new solv::hrmap_sol; 
    if(nil) return new nilv::hrmap_nil;
    if(geometry == gProduct) return new product::hrmap_product;
    return NULL;
    }
  
  auto config = addHook(hooks_args, 0, [] () {
    using namespace arg;
    if(argis("-solrange")) {
      shift_arg_formula(solv::solrange_xy);
      shift_arg_formula(solv::solrange_z);
      return 0;
      }
    else if(argis("-fsol")) {
      shift(); solv::solfname = args();
      return 0;
      }
    else if(argis("-solglitch")) {
      shift_arg_formula(solv::glitch_xy);
      shift_arg_formula(solv::glitch_z);
      return 0;
      }
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
    return 1;
    });
    
  }

}
