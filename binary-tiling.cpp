
namespace hr {

namespace binary {
#if CAP_BT

  enum bindir {
    bd_right = 0,
    bd_up_right = 1,
    bd_up = 2,
    bd_up_left = 3,
    bd_left = 4,
    bd_down = 5, /* for cells of degree 6 */
    bd_down_left = 5, /* for cells of degree 7 */
    bd_down_right = 6 /* for cells of degree 7 */
    };
  
  int type_of(heptagon *h) {
    return h->c7->type;
    }

  // 0 - central, -1 - left, +1 - right
  int mapside(heptagon *h) {
    return h->zebraval;
    }
  
  #if DEBUG_BINARY_TILING
  map<heptagon*, long long> xcode;
  map<long long, heptagon*> rxcode;

  long long expected_xcode(heptagon *h, int d) {
    auto r =xcode[h];
    if(d == 0) return r + 1;
    if(d == 1) return 2*r + 1;
    if(d == 2) return 2*r;
    if(d == 3) return 2*r - 1;
    if(d == 4) return r-1;
    if(d == 5 && type_of(h) == 6) return r / 2;
    if(d == 5 && type_of(h) == 7) return (r-1) / 2;
    if(d == 6 && type_of(h) == 7) return (r+1) / 2;
    breakhere();
    }
  #endif
  
  void breakhere() {
    exit(1);
    }

  const transmatrix& tmatrix(heptagon *h, int dir);
  const transmatrix& itmatrix(heptagon *h, int dir);
  
  heptagon *path(heptagon *h, int d, int d1, std::initializer_list<int> p) {
    static int rec = 0;
    rec++; if(rec>100) exit(1);
    // printf("{generating path from %p (%d/%d) dir %d:", h, type_of(h), mapside(h), d);
    heptagon *h1 = h;
    for(int d0: p) {
      // printf(" [%d]", d0);
      h1 = currentmap->may_create_step(h1, d0);
      // printf(" %p", h1);
      }

    #if DEBUG_BINARY_TILING    
    if(xcode[h1] != expected_xcode(h, d)) {
      printf("expected_xcode mismatch\n");
      breakhere();
      }
    #endif
    // printf("}\n");
    if(h->move(d) && h->move(d) != h1) {
      printf("already connected to something else (1)\n");
      breakhere();
      }
    if(h1->move(d1) && h1->move(d1) != h) {
      printf("already connected to something else (2)\n");
      breakhere();
      }
    h->c.connect(d, h1, d1, false);
    rec--;
    return h1;
    }

  heptagon *pathc(heptagon *h, int d, int d1, std::vector<std::initializer_list<int>> p) {
    h->cmove(S7-1);
    int z = h->c.spin(S7-1);
    return path(h, d, d1, p[z]);
    }
    
  ld hororec_scale = 0.25;
  ld horohex_scale = 0.6;
  
  heptagon *build(heptagon *parent, int d, int d1, int t, int side, int delta) {
    auto h = buildHeptagon1(tailored_alloc<heptagon> (t), parent, d, hsOrigin, d1);
    h->distance = parent->distance + delta;
    h->c7 = NULL;
    if(parent->c7) h->c7 = newCell(t, h);
    h->cdata = NULL;
    h->zebraval = side;
    h->emeraldval = 0;
    if(DIM == 3 && h->c7) {
      if(!parent->emeraldval) parent->emeraldval = currentmap->gamestart()->land;
      eLand z = eLand(parent->emeraldval);
      int chance = 0;
      if(specialland == laCrossroads4 || parent->emeraldval == laCrossroads4) {
        eLand x = parent->c7->land;
        parent->c7->land = z;
        chance = wallchance(parent->c7, deep_ocean_at(parent->c7, parent->c7));
        parent->c7->land = x;
        }
      if(chaosmode) chance = 1000;
      if(chance && hrand(40000) < chance)
        h->emeraldval = getNewLand(z);
      else
        h->emeraldval = z;
      }
    #if DEBUG_BINARY_TILING
    xcode[h] = expected_xcode(parent, d);
    if(rxcode.count(xcode[h])) {
      printf("xcode clash\n");
      breakhere();
      }
    rxcode[xcode[h]] = h;
    #endif
    return h;
    }

  #if MAXMDIM==4
  heptagon *build3(heptagon *parent, int d, int d1, int delta) {
    int side = 0;
    if(geometry == gBinary3) {
      if(d < 4) side = (parent->zebraval * 2 + d) % 5;
      if(d == S7-1) side = ((5+parent->zebraval-d1) * 3) % 5;
      }
    if(geometry == gHoroHex) {
      if(d < 3) side = (parent->zebraval + d) % 3;
      if(d == S7-1) side = (parent->zebraval + 3 - d1) % 3;
      }
    return build(parent, d, d1, S7, side, delta);
    }
  #endif
  
  struct hrmap_binary : hrmap_hyperbolic {
  
    std::mt19937 directions_generator;
  
    hrmap_binary(heptagon *o) : hrmap_hyperbolic(o) { set_seed(); }
    
    void set_seed() { directions_generator.seed(137137137); }
    
    int nextdir(int choices) { return directions_generator() % choices; }

    hrmap_binary() : hrmap_hyperbolic() { set_seed(); }

    heptagon *create_step(heptagon *parent, int d) {
      auto h = parent;
      switch(geometry) {
        case gBinaryTiling: {
          switch(d) {
            case bd_right: {
              if(mapside(h) > 0 && type_of(h) == 7) 
                return path(h, d, bd_left, {bd_left, bd_down, bd_right, bd_up});
              else if(mapside(h) >= 0) 
                return build(parent, bd_right, bd_left, type_of(parent) ^ 1, 1, 0);
              else if(type_of(h) == 6)
                return path(h, d, bd_left, {bd_down, bd_right, bd_up, bd_left});
              else
                return path(h, d, bd_left, {bd_down_right, bd_up});
              }
            case bd_left: {
              if(mapside(h) < 0 && type_of(h) == 7) 
                return path(h, d, bd_right, {bd_right, bd_down, bd_left, bd_up});
              else if(mapside(h) <= 0) 
                return build(parent, bd_left, bd_right, type_of(parent) ^ 1, -1, 0);
              else if(type_of(h) == 6)
                return path(h, d, bd_right, {bd_down, bd_left, bd_up, bd_right});
              else
                return path(h, d, bd_right, {bd_down_left, bd_up});
              }
            case bd_up_right: {
              return path(h, d, bd_down_left, {bd_up, bd_right});
              }
            case bd_up_left: {
              return path(h, d, bd_down_right, {bd_up, bd_left});
              }
            case bd_up: 
              return build(parent, bd_up, bd_down, 6, mapside(parent), 1);
            default:
              /* bd_down */
              if(type_of(h) == 6) {
                if(mapside(h) == 0)
                  return build(parent, bd_down, bd_up, 6, 0, -1);
                else if(mapside(h) == 1)
                  return path(h, d, bd_up, {bd_left, bd_left, bd_down, bd_right});
                else if(mapside(h) == -1)
                  return path(h, d, bd_up, {bd_right, bd_right, bd_down, bd_left});
                }
              /* bd_down_left */
              else if(d == bd_down_left) {
                return path(h, d, bd_up_right, {bd_left, bd_down});
                }
              else if(d == bd_down_right) {
                return path(h, d, bd_up_left, {bd_right, bd_down});
                }
              }
          printf("error: case not handled in binary tiling\n");
          breakhere();
          return NULL;
          }
        #if MAXMDIM >= 4
        case gBinary3: {
          switch(d) {
            case 0: case 1:
            case 2: case 3:
              return build3(parent, d, 8, 1);
            case 8:
              return build3(parent, 8, nextdir(4), -1);
            case 4:
              parent->cmove(8);
              if(parent->c.spin(8) & 1)
                return path(h, 4, 5, {8, parent->c.spin(8) ^ 1});
              else
                return path(h, 4, 5, {8, 4, parent->c.spin(8) ^ 1});
            case 5:
              parent->cmove(8);
              if(!(parent->c.spin(8) & 1))
                return path(h, 5, 4, {8, parent->c.spin(8) ^ 1});
              else
                return path(h, 5, 4, {8, 5, parent->c.spin(8) ^ 1});
            case 6:
              parent->cmove(8);
              if(parent->c.spin(8) & 2)
                return path(h, 6, 7, {8, parent->c.spin(8) ^ 2});
              else
                return path(h, 6, 7, {8, 6, parent->c.spin(8) ^ 2});
            case 7:
              parent->cmove(8);
              if(!(parent->c.spin(8) & 2))
                return path(h, 7, 6, {8, parent->c.spin(8) ^ 2});
              else
                return path(h, 7, 6, {8, 7, parent->c.spin(8) ^ 2});
            }
          }
        case gHoroRec: {
          switch(d) {
            case 0: case 1:
              return build3(parent, d, 6, 1);
            case 6:
              return build3(parent, 6, nextdir(2), -1);
            case 2:
              parent->cmove(6);
              if(parent->c.spin(6) == 0)
                return path(h, 2, 4, {6, 1});
              else
                return path(h, 2, 4, {6, 3, 0});
            case 4:
              parent->cmove(6);
              if(parent->c.spin(6) == 0)
                return path(h, 4, 2, {6, 5, 1});
              else
                return path(h, 4, 2, {6, 0});
            case 3:
              parent->cmove(6);
              return path(h, 3, 5, {6, 4, parent->c.spin(6)});
            case 5:
              parent->cmove(6);
              return path(h, 5, 3, {6, 2, parent->c.spin(6)});
            }
          }
        case gHoroTris: {            
          switch(d) {
            case 0: case 1: case 2: case 3:
              return build3(parent, d, 7, 1);
            case 7:
              return build3(parent, 7, nextdir(3), -1);
            case 4: case 5: case 6: 
              parent->cmove(7);
              int s = parent->c.spin(7);
              if(s == 0) return path(h, d, d, {7, d-3});
              else if(s == d-3) return path(h, d, d, {7, 0});
              else return path(h, d, d, {7, d, 9-d-s});
            }
          }
        case gHoroHex: {
          // the comment is a picture...
          // generated with the help of hexb.cpp          
          switch(d) {
            case 0: case 1: case 2:
              return build3(parent, d, 13, 1);
            case 13:
              return build3(parent, 13, nextdir(3), -1);
            case 3:
              return pathc(h, 3, 12, {{13,4,2}, {13,5,2}, {13,3,2}});
            case 4:
              return pathc(h, 4, 12, {{13,6,2,0}, {13,7,0,0}, {13,8,1,0}});
            case 5:
              return pathc(h, 5, 12, {{13,1,1}, {13,2,1}, {13,0,1}});
            case 6:
              return pathc(h, 6, 10, {{13,5}, {13,3}, {13,4}});
            case 7:
              return pathc(h, 7, 11, {{13,2}, {13,0}, {13,1}});
            case 8:
              return pathc(h, 8, 9, {{13,6,0}, {13,7,1}, {13,8,2}});
            case 9:
              return pathc(h, 9, 8, {{13,4}, {13,5}, {13,3}});
            case 10:
              return pathc(h, 10, 6, {{13,6,2}, {13,7,0}, {13,8,1}});
            case 11:
              return pathc(h, 11, 7, {{13,1}, {13,2}, {13,0}});
            case 12: 
              h->cmove(13);
              int z = h->c.spin(13);
              return path(h, 12, (z+1)%3+3, {13, z+6});
            }
          }
        #endif
        default: ;
        }
      printf("error: case not handled in binary tiling\n");
      breakhere();
      return NULL;
      }

    void draw() {
      dq::visited.clear();
      dq::enqueue(viewctr.at, cview());
      
      while(!dq::drawqueue.empty()) {
        auto& p = dq::drawqueue.front();
        heptagon *h = get<0>(p);
        transmatrix V = get<1>(p);
        dynamicval<ld> b(band_shift, get<2>(p));
        bandfixer bf(V);
        dq::drawqueue.pop();
        
        
        cell *c = h->c7;
        if(!do_draw(c, V)) continue;
        drawcell(c, V, 0, false);
  
        if(DIM == 2) {
          dq::enqueue(h->move(bd_up), V * xpush(-log(2)));
          dq::enqueue(h->move(bd_right), V * parabolic(1));
          dq::enqueue(h->move(bd_left), V * parabolic(-1));
          if(c->type == 6)
            dq::enqueue(h->move(bd_down), V * xpush(log(2)));
          if(c->type == 7) {
            dq::enqueue(h->move(bd_down_left), V * parabolic(-1) * xpush(log(2)));
            dq::enqueue(h->move(bd_down_right), V * parabolic(1) * xpush(log(2)));
            }
          }
        else {
          for(int i=0; i<S7; i++)
            dq::enqueue(h->move(i), V * tmatrix(h, i));
          }
        }
      }
    
    // hrmap_standard overrides hrmap's default, override it back
    virtual transmatrix relative_matrix(cell *c2, cell *c1, const hyperpoint& point_hint) override {
      return relative_matrix(c2->master, c1->master);
      }

    transmatrix relative_matrix(heptagon *h2, heptagon *h1) override {
      if(gmatrix0.count(h2->c7) && gmatrix0.count(h1->c7))
        return inverse(gmatrix0[h1->c7]) * gmatrix0[h2->c7];
      transmatrix gm = Id, where = Id;
      while(h1 != h2) {
        if(h1->distance <= h2->distance) {
          if(DIM == 3)
            where = itmatrix(h2, S7-1) * where, h2 = may_create_step(h2, S7-1);
          else {
            if(type_of(h2) == 6)
              h2 = may_create_step(h2, bd_down), where = xpush(-log(2)) * where;
            else if(mapside(h2) == 1)
              h2 = may_create_step(h2, bd_left), where = parabolic(+1) * where;
            else if(mapside(h2) == -1)
              h2 = may_create_step(h2, bd_right), where = parabolic(-1) * where;
            }
          }
        else {
          if(DIM == 3)
            gm = gm * tmatrix(h1, S7-1), h1 = may_create_step(h1, S7-1);
          else {
            if(type_of(h1) == 6)
              h1 = may_create_step(h1, bd_down), gm = gm * xpush(log(2));
            else if(mapside(h1) == 1)
              h1 = may_create_step(h1, bd_left), gm = gm * parabolic(-1);
            else if(mapside(h1) == -1)
              h1 = may_create_step(h1, bd_right), gm = gm * parabolic(+1);
            }
          }
        }
      return gm * where;
      }

    vector<hyperpoint> get_vertices(cell* c) override {
      vector<hyperpoint> res;
      ld yy = log(2) / 2;
      using namespace hyperpoint_vec;
      auto add = [&] (hyperpoint h) { 
        res.push_back(binary::parabolic3(h[0], h[1]) * xpush0(yy*h[2]));
        };
      switch(geometry) {
        case gBinary3: 
          for(int x=-1; x<2; x++) for(int y=-1; y<2; y++) for(int z=-1; z<=1; z+=2)
            if(z == -1 || x != 0 || y != 0)
              add(point3(x,y,z));
          break;
        case gHoroTris: {
          ld r = sqrt(3)/6;
          ld r2 = r * 2;
          
          hyperpoint shift3 = point3(0,0,-3);
          hyperpoint shift1 = point3(0,0,-1);
          
          for(int i=0; i<3; i++) {
            hyperpoint t0 = spin(120 * degree * i) * point3(0,-r2,-1);          
            add(t0);
            add(-2 * t0 + shift3);
            add(-2 * t0 + shift1);
            }
          }
        case gHoroRec: {
          ld r2 = sqrt(2);
          for(int y=-1; y<=1; y++) for(int x=-1; x<=1; x+=2) for(int z=-1; z<=1; z++)
            if(z == -1 || y != 0)
              add(point3(-r2*x*hororec_scale, -2*y*hororec_scale, z*.5));
          break;
          }
        case gHoroHex: {
          // complicated and unused for now -- todo
          break;
          }
        default: ;
        }
      return res;
      }
    };

  hrmap *new_map() { return new hrmap_binary; }
  
  struct hrmap_alternate_binary : hrmap_binary {
    heptagon *origin;
    hrmap_alternate_binary(heptagon *o) { origin = o; }
    ~hrmap_alternate_binary() { clearfrom(origin); }
    };

  hrmap *new_alt_map(heptagon *o) { return new hrmap_binary(o); }

  transmatrix direct_tmatrix[14];
  transmatrix inverse_tmatrix[14];

  int use_direct;
  // directions below 'use_direct' are taken from direct_tmatrix;
  // directions at/above are taken by checking spin and inverse_tmatrix based on that
  
  void build_tmatrix() {
    use_direct = S7-1;
    if(geometry == gBinary3) {
      direct_tmatrix[0] = xpush(-log(2)) * parabolic3(-1, -1);
      direct_tmatrix[1] = xpush(-log(2)) * parabolic3(1, -1);
      direct_tmatrix[2] = xpush(-log(2)) * parabolic3(-1, 1);
      direct_tmatrix[3] = xpush(-log(2)) * parabolic3(1, 1);
      direct_tmatrix[4] = parabolic3(-2, 0);
      direct_tmatrix[5] = parabolic3(+2, 0);
      direct_tmatrix[6] = parabolic3(0, -2);
      direct_tmatrix[7] = parabolic3(0, +2);
      }
    if(geometry == gHoroTris) {
      ld r3 = sqrt(3);
      direct_tmatrix[0] = xpush(-log(2)) * cspin(1,2, M_PI);
      direct_tmatrix[1] = parabolic3(0, +r3/3) * xpush(-log(2));
      direct_tmatrix[2] = parabolic3(-0.5, -r3/6) * xpush(-log(2));
      direct_tmatrix[3] = parabolic3(+0.5, -r3/6) * xpush(-log(2));
      direct_tmatrix[4] = parabolic3(0, -r3*2/3) * cspin(1,2, M_PI);
      direct_tmatrix[5] = parabolic3(1, r3/3) * cspin(1,2,M_PI);
      direct_tmatrix[6] = parabolic3(-1, r3/3) * cspin(1,2,M_PI);
      }
    if(geometry == gHoroRec) {
      ld r2 = sqrt(2);
      ld l = -log(2)/2;
      ld z = hororec_scale;
      direct_tmatrix[0] = parabolic3(0, -z) * xpush(l) * cspin(2,1,M_PI/2);
      direct_tmatrix[1] = parabolic3(0, +z) * xpush(l) * cspin(2,1,M_PI/2);
      direct_tmatrix[2] = parabolic3(+2*r2*z, 0);
      direct_tmatrix[3] = parabolic3(0, +4*z);
      direct_tmatrix[4] = parabolic3(-2*r2*z, 0);
      direct_tmatrix[5] = parabolic3(0, -4*z);
      }
    if(geometry == gHoroHex) {
      // also generated with the help of hexb.cpp
      ld l = log(3)/2;
      auto& t = direct_tmatrix;
      t[0] = parabolic3(horohex_scale, 0) * xpush(-l) * cspin(1, 2, M_PI/2);
      t[1] = cspin(1, 2, 2*M_PI/3) * t[0];
      t[2] = cspin(1, 2, 4*M_PI/3) * t[0];
      auto it = inverse(t[0]);

      t[5] = it * t[1] * t[1];
      t[6] = it * t[5];
      t[4] = it * t[6] * t[2] * t[0];
      t[3] = it * t[4] * t[2];

      t[7] = it * t[2];
      t[8] = it * t[6] * t[0];
      t[9] = it * t[4];
      t[10] = it * t[6] * t[2];
      t[11] = it * t[1];

      for(int a=0; a<12; a++) println(hlog, t[a]);
      use_direct--;
      }
    for(int i=0; i<use_direct; i++)
      inverse_tmatrix[i] = inverse(direct_tmatrix[i]);
    }
  
  const transmatrix& tmatrix(heptagon *h, int dir) {
    if(dir >= use_direct) {
      h->cmove(dir);
      return inverse_tmatrix[h->c.spin(dir)];
      }
    else
      return direct_tmatrix[dir];
    }

  const transmatrix& itmatrix(heptagon *h, int dir) {
    if(dir >= use_direct) {
      h->cmove(dir);
      return h->cmove(dir), direct_tmatrix[h->c.spin(dir)];
      }
    else
      return inverse_tmatrix[dir];
    }
  
  #if MAXMDIM == 4

  void queuecube(const transmatrix& V, ld size, color_t linecolor, color_t facecolor) {
    ld yy = log(2) / 2;
    const int STEP=3;
    const ld MUL = 1. / STEP;
    auto at = [&] (ld x, ld y, ld z) { curvepoint(V * parabolic3(size*x, size*y) * xpush0(size*yy*z)); };
    for(int a:{-1,1}) {
      for(ld t=-STEP; t<STEP; t++) at(a, 1,t*MUL);
      for(ld t=-STEP; t<STEP; t++) at(a, -t*MUL,1);
      for(ld t=-STEP; t<STEP; t++) at(a, -1,-t*MUL);
      for(ld t=-STEP; t<STEP; t++) at(a, t*MUL,-1);
      at(a, 1,-1);
      queuecurve(linecolor, facecolor, PPR::LINE);

      for(ld t=-STEP; t<STEP; t++) at(1,t*MUL,a);
      for(ld t=-STEP; t<STEP; t++) at(-t*MUL,1,a);
      for(ld t=-STEP; t<STEP; t++) at(-1,-t*MUL,a);
      for(ld t=-STEP; t<STEP; t++) at(t*MUL,-1,a);
      at(1,-1,a);
      queuecurve(linecolor, facecolor, PPR::LINE);

      for(ld t=-STEP; t<STEP; t++) at(1,a,t*MUL);
      for(ld t=-STEP; t<STEP; t++) at(-t*MUL,a,1);
      for(ld t=-STEP; t<STEP; t++) at(-1,a,-t*MUL);
      for(ld t=-STEP; t<STEP; t++) at(t*MUL,a,-1);
      at(1,a,-1);
      queuecurve(linecolor, facecolor, PPR::LINE);
      }
    /*for(int a:{-1,1}) for(int b:{-1,1}) for(int c:{-1,1}) {
      at(0,0,0); at(a,b,c);  queuecurve(linecolor, facecolor, PPR::LINE);
      }*/
    }
  #endif

  transmatrix parabolic(ld u) {
    return parabolic1(u * vid.binary_width / log(2) / 2);
    }

  transmatrix parabolic3(ld y, ld z) {
    ld co = vid.binary_width / log(2) / 4;
    return hr::parabolic13(y * co, z * co);
    }

  // on which horocycle are we
  ld horo_level(hyperpoint h) {
    using namespace hyperpoint_vec;
    h /= (1 + h[DIM]);
    h[0] -= 1;
    h /= sqhypot_d(DIM, h);
    h[0] += .5;
    return log(2) + log(-h[0]);
    }
  
  hyperpoint deparabolic3(hyperpoint h) {
    using namespace hyperpoint_vec;
    h /= (1 + h[3]);
    hyperpoint one = point3(1,0,0);
    h -= one;
    h /= sqhypot_d(3, h);
    h[0] += .5;
    ld co = vid.binary_width / log(2) / 8;
    return point3(log(2) + log(-h[0]), h[1] / co, h[2] / co);
    }
  
#if CAP_COMMANDLINE
auto bt_config = addHook(hooks_args, 0, [] () {
  using namespace arg;
  if(argis("-btwidth")) {
    shift_arg_formula(vid.binary_width, delayed_geo_reset);
    return 0;
    }
  return 1;
  });
#endif

bool pseudohept(cell *c) {
  if(DIM == 2)
    return c->type & c->master->distance & 1;
  else if(geometry == gHoroRec)
    return c->c.spin(S7-1) == 0 && (c->master->distance & 1) && c->cmove(S7-1)->c.spin(S7-1) == 0;
  else if(geometry == gHoroTris)
    return c->c.spin(S7-1) == 0 && (c->master->distance & 1);
  else
    return (c->master->zebraval == 1) && (c->master->distance & 1);
  }

pair<gp::loc, gp::loc> gpvalue(heptagon *h) {
  int d = h->c.spin(S7-1);
  if(d == 0) return make_pair(gp::loc(0,0), gp::loc(-1,0));
  else return make_pair(gp::eudir((d-1)*2), gp::loc(1,0));
  }

// distance in a triangular grid
int tridist(gp::loc v) {
  using namespace gp;
  int d = v.first - v.second;
  int d0 = d % 3;
  if(d0 == 1 || d0 == -2) return 1 + min(tridist(v - eudir(0)), min(tridist(v - eudir(2)), tridist(v - eudir(4))));
  if(d0 == 2 || d0 == -1) return 1 + min(tridist(v + eudir(0)), min(tridist(v + eudir(2)), tridist(v + eudir(4))));
  return length(v * loc(1,1)) * 2 / 3;
  }

int equalize(heptagon*& c1, heptagon*& c2) {
  int steps = 0;
  int d1 = c1->distance;
  int d2 = c2->distance;
  while(d1 > d2) c1 = c1->cmove(S7-1), steps++, d1--;
  while(d2 > d1) c2 = c2->cmove(S7-1), steps++, d2--;
  return steps;
  }  

int celldistance3_tri(heptagon *c1, heptagon *c2) {
  using namespace gp;
  int steps = equalize(c1, c2);
  vector<pair<loc, loc> > m1, m2;
  while(c1 != c2) {
    m2.push_back(gpvalue(c2));
    m1.push_back(gpvalue(c1));
    c1 = c1->cmove(S7-1);
    c2 = c2->cmove(S7-1);
    steps += 2;
    }
  loc T1(0,0), T2(0,0), inv1(1,0), inv2(1,0);
  int xsteps = steps;
  while(isize(m1)) {
    xsteps -= 2;
    inv1 = inv1 * m1.back().second;
    inv2 = inv2 * m2.back().second;
    T1 = T1 + T1 + m1.back().first * inv1;
    T2 = T2 + T2 + m2.back().first * inv2;
    m1.pop_back(); m2.pop_back();
    loc T0 = T2 - T1;
    if(T0.first > 3 || T0.second > 3 || T0.first < -3 || T0.second < -3) break;
    steps = min(steps, xsteps + tridist(T0));
    }
  return steps;
  }

int celldistance3_rec(heptagon *c1, heptagon *c2) {
  int steps = equalize(c1, c2);
  vector<int> dx;
  while(c1 != c2) {
    dx.push_back(c1->c.spin(S7-1) - c2->c.spin(S7-1));
    c1 = c1->cmove(S7-1);
    c2 = c2->cmove(S7-1);
    steps += 2;
    }
  int xsteps = steps, sx = 0, sy = 0;
  while(isize(dx)) {
    xsteps -= 2;
    tie(sx, sy) = make_pair(-sy, 2 * sx + dx.back());
    dx.pop_back();
    int ysteps = xsteps + abs(sx) + abs(sy);
    if(ysteps < steps) steps = ysteps;
    if(sx >= 8 || sx <= -8 || sy >= 8 || sy <= -8) break;
    }
  return steps;
  }

int celldistance3_square(heptagon *c1, heptagon *c2) {
  int steps = equalize(c1, c2);
  vector<int> dx, dy;
  while(c1 != c2) {
    dx.push_back((c1->c.spin(S7-1) & 1) - (c2->c.spin(S7-1) & 1));
    dy.push_back((c1->c.spin(S7-1) >> 1) - (c2->c.spin(S7-1) >> 1));
    c1 = c1->cmove(S7-1);
    c2 = c2->cmove(S7-1);
    steps += 2;
    }
  int xsteps = steps, sx = 0, sy = 0;
  while(isize(dx)) {
    xsteps -= 2;
    sx *= 2;
    sy *= 2;
    sx += dx.back(); sy += dy.back();
    dx.pop_back(); dy.pop_back();
    int ysteps = xsteps + abs(sx) + abs(sy);
    if(ysteps < steps) steps = ysteps;
    if(sx >= 8 || sx <= -8 || sy >= 8 || sy <= -8) break;
    }
  return steps;
  }

// this algorithm is wrong: it never considers the "narrow gap" moves
int celldistance3_hex(heptagon *c1, heptagon *c2) {
  int steps = equalize(c1, c2);
  vector<int> d1, d2;
  while(c1 != c2) {
    d1.push_back(c1->c.spin(S7-1));
    d2.push_back(c2->c.spin(S7-1));
    c1 = c1->cmove(S7-1);
    c2 = c2->cmove(S7-1);
    steps += 2;
    }
  int xsteps = steps;
  dynamicval<eGeometry> g(geometry, gEuclid);
  transmatrix T = Id;
  while(isize(d1)) {
    xsteps -= 2;

    T = euscalezoom(hpxy(0,sqrt(3))) * eupush(1,0) * spin(-d2.back() * 2 * M_PI/3) * T * spin(d1.back() * 2 * M_PI/3) * eupush(-1,0) * euscalezoom(hpxy(0,-1/sqrt(3)));
    
    d1.pop_back(); d2.pop_back();
    
    hyperpoint h = tC0(T);
    int sx = int(floor(h[0] - h[1] / sqrt(3) + .5)) / 3;
    int sy = int(floor(h[1] * 2 / sqrt(3) + .5)) / 3;
    
    int ysteps = xsteps + eudist(sx, sy);
    if(ysteps < steps) steps = ysteps;
    if(sx >= 8 || sx <= -8 || sy >= 8 || sy <= -8) break;
    }
  return steps;
  }

int celldistance3(heptagon *c1, heptagon *c2) {
  switch(geometry) {
    case gBinary3: return celldistance3_square(c1, c2);
    case gHoroTris: return celldistance3_tri(c1, c2);
    case gHoroRec: return celldistance3_rec(c1, c2);
    case gHoroHex: return celldistance3_hex(c1, c2);
    default: println(hlog, "called celldistance3 for wrong geometry"); return 0;
    }
  }

int celldistance3(cell *c1, cell *c2) { return celldistance3(c1->master, c2->master); }

void virtualRebaseSimple(heptagon*& base, transmatrix& at) {

  while(true) {
  
    double currz = at[DIM][DIM];
    
    heptagon *h = base;
    
    heptagon *newbase = NULL;
    
    transmatrix bestV;
    
    for(int d=0; d<S7; d++) {
      transmatrix V2 = itmatrix(h, d) * at;
      double newz = V2[DIM][DIM];
      if(newz < currz) {
        currz = newz;
        bestV = V2;
        newbase = h->cmove(d);
        }
      }

    if(newbase) {
      base = newbase;
      at = bestV;
      continue;
      }

    return;
    }
  }
#endif


  }
}
