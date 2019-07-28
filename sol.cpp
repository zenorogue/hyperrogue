
// implementation of the Solv space

// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

#include "extra/solv/common.cpp"

namespace hr {

namespace solv {
  
  const int PRECX = 64;
  const int PRECZ = 32;
  const ld SXY = 32.;
  const ld SZ = 8.;
  
  using pt = hyperpoint;
  using ptlow = array<float, 3>;

  ptlow inverse_exp_table[PRECZ][PRECX][PRECX];
  
  bool table_loaded;
  
  void load_table() {
    if(table_loaded) return;
    FILE *f = fopen("soltable.dat", "rb");
    if(!f) f = fopen("/usr/lib/soltable.dat", "rb");
    if(!f) { addMessage(XLAT("geodesic table missing")); pmodel = mdPerspective; return; }
    fread(inverse_exp_table, sizeof(inverse_exp_table), 1, f);
    fclose(f);
    table_loaded = true;
    }
  
  void step2(ld& x, ld &y, ld &z, ld &vx, ld &vy, ld &vz) {
  
    ld ax = -2 * vx * vz;
    ld ay = 2 * vy * vz;
    ld az = exp(2*z) * vx * vx - exp(-2*z) * vy * vy;
  
    // ld x2 = x + vx/2;
    // ld y2 = y + vy/2;
    ld z2 = z + vz/2;
    
    ld vx2 = vx + ax/2;
    ld vy2 = vy + ay/2;
    ld vz2 = vz + az/2;
    
    ld ax2 = -2 * vx2 * vz2;
    ld ay2 = 2 * vy2 * vz2;
    ld az2 = exp(2*z2) * vx2 * vx2 - exp(-2*z2) * vy2 * vy2;
    
    x += vx + ax/2;
    y += vy + ay/2;
    z += vz + az/2;
    
    vx += ax2;
    vy += ay2;
    vz += az2;
    }
  
  pt direct_exp(pt v, int steps) {
    pt at = hpxy(0, 0);
    v[0] /= steps;
    v[1] /= steps;
    v[2] /= steps;
    for(int i=0; i<steps; i++) step2(at[0], at[1], at[2], v[0],v[1],v[2]);
    return at;
    }

  void parallel(ld x, ld y, ld z, ld vx, ld vy, ld vz, ld& wx, ld& wy, ld& wz, ld t) {
    // ld dax = -dz * gxyz * ay;
    
    ld dwx = -vz * wx - vx * wz;
    ld dwy =  vz * wy + vy * wz;
    ld dwz = vx * wx * exp(2*z) - vy * wy * exp(-2*z);
      
    wx += dwx * t;
    wy += dwy * t;
    wz += dwz * t;
    }

  pt direct_exp(pt v, int steps, vector<pt> transported) {
    ld x = 0, y = 0, z = 0;
    v[0] /= steps;
    v[1] /= steps;
    v[2] /= steps;
    for(int i=0; i<steps; i++) {
      for(auto t: transported) parallel(x,y,z, v[0],v[1],v[2], t[0], t[1], t[2], 1);
      step2(x,y,z, v[0],v[1],v[2]);
      }
    return hpxy3(x, y, z);
    }
  
  pt inverse_exp(pt h) {
    load_table();
    
    ld sx = 1, sy = 1;
    bool nz = false;
    
    ld ix = asinh(h[0]) * SXY;
    ld iy = asinh(h[1]) * SXY;
    ld iz = h[2] * SZ / log(2);

    if(ix < 0) ix = -ix, sx = -sx;
    if(iy < 0) iy = -iy, sy = -sy;
    if(iz < 0) iz = -iz, nz = true, swap(ix, iy);

    if(ix >= PRECX-1) ix = PRECX-2;
    if(iy >= PRECX-1) iy = PRECX-2;
    if(iz >= PRECZ-1) iz = PRECZ-2;
    
    int ax = ix, bx = ax+1;
    int ay = iy, by = ay+1;
    int az = iz, bz = az+1;
    
    pt res;
    
    // println(hlog, "inv_table", make_tuple(iz,iy,ix), " = ", make_tuple(inverse_exp_table[z][y][x][0], inverse_exp_table[z][y][x][1], inverse_exp_table[z][y][x][2]));
    
    #define S0(x,y,z) inverse_exp_table[z][y][x][t]
    #define S1(x,y) (S0(x,y,az) * (bz-iz) + S0(x,y,bz) * (iz-az))
    #define S2(x) (S1(x,ay) * (by-iy) + S1(x,by) * (iy-ay))
  
    for(int t=0; t<3; t++)
      res[t] = S2(ax) * (bx-ix) + S2(bx) * (ix-ax);
    if(nz) swap(res[0], res[1]), res[2] = -res[2];
    res[0] *= sx; res[1] *= sy;
    res[3] = 1;
    
    #undef S0
    #undef S1
    #undef S2
    
    // println(hlog, kz(h), " => ", kz(res), " => ", kz(direct_exp(res, 1000)), " [", ix, ",", iy, ",", iz, " | ", sx, "/", sy, "/", nz, "]");
    
    return res;
    }

  transmatrix local_perspective, ilocal_perspective;
  
  bool geodesic_movement = true;

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

      transmatrix T = eupush( tC0(inverse(View)) );
      local_perspective = View * T;
      ilocal_perspective = inverse(local_perspective);

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

  hrmap *new_map() { return new hrmap_sol; }

  bool in_table_range(hyperpoint h) {
    ld ix = asinh(h[0]) * SXY;
    ld iy = asinh(h[1]) * SXY;
    ld iz = h[2] * SZ / log(2);
    return abs(ix) < PRECX && abs(iy) < PRECX && abs(iz) < PRECZ;
    }

  transmatrix get_solmul(const transmatrix T, const transmatrix V) {
    if(!geodesic_movement) return V * eupush(inverse(V) * T * V * C0);

    transmatrix push_back = eupush( tC0(inverse(V)) );
    transmatrix space_to_view = V * push_back;
    
    transmatrix view_to_space = inverse(space_to_view);
    using namespace hyperpoint_vec;
    hyperpoint shift = /* inverse(V) * T * V * C0; */ view_to_space * T * C0;
    
    transmatrix push_to = inverse(push_back);

    int steps = 100;
    
    hyperpoint units[3] = { point3(1,0,0), point3(0,1,0), point3(0,0,1) };
    
    /*
    println(hlog, "shift = ", kz(shift));
    println(hlog, "space_to_view = ", kz(space_to_view));

    for(int i=0; i<3; i++)
      println(hlog, "view_to_space[", i, "] = ", kz(view_to_space * units[i]));
    */

    for(int s=0; s<3; s++) shift[s] /= steps;
    ld x = 0, y = 0, z = 0;
    

    for(int i=0; i<steps; i++) {
      for(int j=0; j<3; j++) 
        parallel(x, y, z, shift[0], shift[1], shift[2], view_to_space[0][j], view_to_space[1][j], view_to_space[2][j], -1);
      step2(x, y, z, shift[0], shift[1], shift[2]);
      }
    
    space_to_view = inverse(view_to_space);
    
    /*
    println(hlog, "arrive at = ", kz(pt({x, y, z})), " with vel = ", (shift * steps));

    for(int i=0; i<3; i++)
      println(hlog, "view_to_space[", i, "] = ", kz(view_to_space * units[i]));
    println(hlog, "space_to_view = ", kz(space_to_view));
    */

    for(int i=0; i<3; i++) for(int j=0; j<3; j++) {
      print(hlog, (view_to_space*units[i] | view_to_space*units[j]), " | ");
      }
      println(hlog);
    
    // println(hlog, "view_to_space = ", view_to_space);
    
    transmatrix npush = Id;
    npush[0][GDIM] = x;
    npush[1][GDIM] = y;
    npush[2][GDIM] = z;

    // println(hlog, "result = ", space_to_view * npush * push_to);
    // println(hlog, "naive = ", V * eupush(inverse(V) * T * V * C0));
    return space_to_view * npush * push_to;
    }

  string solshader = 
    "uniform mediump sampler3D tInvExpTable;"    
    "uniform mediump mat4 uILP;"

    "vec4 inverse_exp(vec4 h) {"
    
    "float ix = asinh(h[0]) * 32.;"
    "float iy = asinh(h[1]) * 32.;"
    "float iz = h[2] * 8. / log(2.);"
    
    "if(ix < 0.) ix = -ix;"
    "if(iy < 0.) iy = -iy;"
    "if(iz < 0.) { iz = -iz; float s = ix; ix = iy; iy = s; }"
  
    "vec4 res = texture3D(tInvExpTable, vec3((ix + 0.5) / 64., (iy + 0.5) / 64., (iz + 0.5) / 32.));"
  
    "if(h[2] < 0.) { res.xy = res.yx; res[2] = -res[2]; }"
    "if(h[0] < 0.) res[0] = -res[0];"
    "if(h[1] < 0.) res[1] = -res[1];"
    
    "return res;"
    "}";
  
  }

}
