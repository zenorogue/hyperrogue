namespace nilrider {

const int steps_per_block = 16;
const int texture_density = 64;

void level::init_textures() {
  create_castle();
  int tY = isize(map_tiles);
  int tX = isize(map_tiles[0]);

  transmatrix T = gpushxto0(new_levellines_for);

  for(int which: {0, 1, 2}) {

    bool stepped = which == 1;
    bool levels = which == 2;

    auto& target = stepped ? unil_texture_stepped : levels ? unil_texture_levels : unil_texture;
    if(target && !(levels && levellines_for != new_levellines_for)) continue;

    bool regen = !target;
    if(regen) target = new texture::texture_data;

    auto& tex = *target;

    if(regen) {
      tex.twidth = tex.tx = tX * texture_density;
      tex.theight = tex.ty = tY * texture_density;
      tex.stretched = false;
      tex.strx = tex.tx;
      tex.stry = tex.ty;
      tex.base_x = 0;
      tex.base_y = 0;
      tex.whitetexture();
      }

    auto getpix = [&] (int x, int y) {
      int px = x / pixel_per_block;
      int py = y / pixel_per_block;
      if(px < 0 || py < 0 || px >= tX || py >= tY) return '!';
      char bmch = map_tiles[py][px];
      if(bmch == '!') return '!';
      return submaps[bmch][y % pixel_per_block][x % pixel_per_block];
      };

    int subpixels = texture_density / pixel_per_block;
    int stepdiv = texture_density / steps_per_block;

    for(int y=0; y<tex.ty; y++)
    for(int x=0; x<tex.tx; x++) {
      color_t col;
      if(!stepped) {
        char c = getpix(x / subpixels, y / subpixels);
        col = bcols[c];
        if(levels && new_levellines_for[3]) {
          hyperpoint h = T * mappt(x, y, texture_density);
          ld z = h[2] - sym_to_used_bonus(h);
          if(z > 0) col = gradient(col, 0xFFFF0000, 0, z - floor(z), 4);
          if(z < 0) col = gradient(col, 0xFF0000FF, 0, -z - floor(-z), 4);
          }
        }
      else {
        int mx = x % stepdiv;
        int my = y % stepdiv;
        int mx0, mx1, my0, my1;
        if(mx < stepdiv/2) { mx0 = mx1 = 2*mx; } else {mx0 = stepdiv-2; mx1=stepdiv; }
        if(my < stepdiv/2) { my0 = my1 = 2*my; } else {my0 = stepdiv-2; my1=stepdiv; }
        int ax = (x/stepdiv) * stepdiv;
        int ay = (y/stepdiv) * stepdiv;

        ld maxh = -HUGE_VAL;
        char c = '!';
        for(int i=0; i<4; i++) {
          hyperpoint h = mappt(ax + ((mx0 != mx1) ? (i&1?3:1)*stepdiv/2 : mx0), ay + ((my0 != my1) ? (i&2?3:1)*stepdiv/2 : my0), texture_density);
          if(h[2] > maxh) {
            c = getpix(((i&1?mx1:mx0) + ax) / subpixels, ((i&2?my1:my0)+ ay) / subpixels);
            if(c != '!') maxh = h[2];
            }
          }
        col = bcols[c];
        if(mx0 != mx1)  col = gradient(col, 0xFF000000, 0, .1, 1);
        if(my0 != my1)  col = gradient(col, 0xFF000000, 0, .2, 1);
        }
      tex.get_texture_pixel(x, y) = col;
      tex.get_texture_pixel(x, y) |= 0xFF000000;
      if(!levels)
        tex.get_texture_pixel(x, y) ^= hrand(0x1000000) & 0xF0F0F;
      }
    tex.loadTextureGL();
    }
  levellines_for = new_levellines_for;
  }

void level::init_shapes() {
  check_cgi();
  string s = "nillevel-" + name;
  if(cgi.ext.count(s)) return;
  cgi.ext[s] = nullptr;
  
  int tY = isize(map_tiles);
  int tX = isize(map_tiles[0]);
  
  for(int s=0; s<3; s++) {
    if(euclid && s != 1) continue;
    if(nil && s == 1) continue;
    cgi.bshape(s == 0 ? shFloor : s == 1 ? shPlanFloor : shStepFloor, PPR::WALL);
    shFloor.flags |= POLY_TRIANGLES;
    shPlanFloor.flags |= POLY_TRIANGLES;
    shStepFloor.flags |= POLY_TRIANGLES;

    int prec = 16;
    if(s == 2) prec *= 4;
    int cdiv = prec / steps_per_block;
    prec >>= reduce_quality;
    
    bool need_uniltinf = uniltinf.tvertices.empty();
    
    auto pt = [&] (int x, int y, int qx, int qy) {
      if(need_uniltinf) uniltinf.tvertices.push_back(glhr::makevertex(x * 1. / tX / prec, y * 1. / tY / prec, 0));
      if(s == 2) {
        ld ax = x, ay = y;
        if(qx) {
          if(x % cdiv == cdiv/2+1) qx = 0;
          }
        else {
          ax -= (x % cdiv);
          ax += .5;
          ax += (x % cdiv) * (cdiv/2-1.) / (cdiv/2);
          }
        if(qy) {
          if(y % cdiv == cdiv/2+1) qy = 0;
          }
        else {
          ay -= (y % cdiv);
          ay += .5;
          ay += (y % cdiv) * (cdiv/2-1.) / (cdiv/2);
          }
        uniltinf_stepped.tvertices.push_back(glhr::makevertex((ax+qx) / tX / prec, (ay+qy) / tY / prec, 0));
        }
      
      hyperpoint h = mappt(x, y, prec);
      if(s == 2) {
        if(x % cdiv == cdiv/2+1) x += cdiv/2 - 1;
        if(y % cdiv == cdiv/2+1) y += cdiv/2 - 1;
        int gx = x/cdiv*cdiv + (x%cdiv) * 2;
        int gy = y/cdiv*cdiv + (y%cdiv) * 2;
        hyperpoint gh = mappt(gx, gy, prec);
        int ax = x/cdiv*cdiv + cdiv/2;
        int ay = y/cdiv*cdiv + cdiv/2;
        hyperpoint ah = mappt(ax, ay, prec);
        gh[0] = gh[0] - ah[0];
        gh[1] = gh[1] - ah[1];
        gh[2] = 0;
        gh = sym_to_used(gh);
        h = rgpushxto0(ah) * gh;
        ld delta = 0;
        // make sure steps are below the actual level
        for(int z=0; z<4; z++) {
          int zx = x/cdiv*cdiv + ((z&1)?cdiv:0);
          int zy = y/cdiv*cdiv + ((z&2)?cdiv:0);
          hyperpoint zh = mappt(zx, zy, prec);
          hyperpoint uh;
          uh[0] = zh[0] - ah[0];
          uh[1] = zh[1] - ah[1];
          uh[2] = 0; uh[3] = 1;
          uh = sym_to_used(uh);
          uh = rgpushxto0(ah) * uh;
          delta = max(delta, uh[2] - zh[2]);
          }
        h[2] -= delta;
        }

      real_minx = min(real_minx, h[0]);
      real_maxx = max(real_maxx, h[0]);
      real_miny = min(real_miny, h[1]);
      real_maxy = max(real_maxy, h[1]);
      if(s == 1) h[2] = h[3] = 1;
      cgi.hpcpush(h);
      };

    for(int y=0; y<tY * prec; y++)
    for(int x=0; x<tX * prec; x++) {
      char bmch = map_tiles[y/prec][x/prec];
      if(bmch == '!') continue;
      int qx = 0, qy = 0;
      if(s == 2) {
        if(x % cdiv == (cdiv/2)) qx++;
        if(x % cdiv > (cdiv/2)) qx+=2;
        if(y % cdiv == (cdiv/2)) qy++;
        if(y % cdiv > (cdiv/2)) qy+=2;
        if(qx + qy > 1) continue;
        }
      pt(x, y, qx, qy);
      pt(x, y+1, qx, qy);
      pt(x+1, y, qx, qy);
      pt(x+1, y+1, qx, qy);
      pt(x+1, y, qx, qy);
      pt(x, y+1, qx, qy);
      }
    cgi.finishshape();
    }

  if(1) {
    cgi.bshape(shField, PPR::WALL);
    shField.flags |= POLY_TRIANGLES;
    
    auto pt = [&] (hyperpoint p) {
      hyperpoint h = mappt(p[0], p[1], 16);
      h[2] += p[2];
      cgi.hpcpush(h);
      // cgi.hpcpush(hyperpoint(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5, 1));
      };

    for(int y=0; y<tY * 16; y++)
    for(int x=0; x<tX * 16; x++) {
      int bx = x / 16;
      int by = y / 16;
      char bmch = map_tiles[by][bx];
      if(bmch == 'f' && (x&1) && (y&1)) {
        for(int s=0; s<4; s++) {
          hyperpoint st = point3(x+.1, y+.1, 0);
          hyperpoint a = spin(90._deg*s) * point3(.1, .1, 0);
          hyperpoint b = spin(90._deg*(s+1)) * point3(.1, .1, 0);
          hyperpoint hi = point3(0, 0, 1);
          for(int z=0; z<3; z++) {
            ld z1 = (3-z) / 3.;
            ld z2 = (2-z) / 3.;
            pt(st + a * z1 + hi*z);
            pt(st + b * z1 + hi*z);
            pt(st + a * z2 + hi*(z+1));
            pt(st + a * z2 + hi*(z+1));
            pt(st + b * z1 + hi*z);
            pt(st + b * z2 + hi*(z+1));
            }
          }
        }
      }
    cgi.finishshape();
    }
  
  if(1) {
    cgi.bshape(shCastle, PPR::WALL);
    shCastle.flags |= POLY_TRIANGLES;
    
    for(int y=0; y<tY; y++)
    for(int x=0; x<tX; x++) {
      char bmch = map_tiles[y][x];
      if(bmch == 'r') {
        for(int s=0; s<4; s++) {
          hyperpoint ctr = mappt(x+.5, y+.5, 1);
          ctr[2] += safe_alt(ctr) + .5 * scale;
          ld need = safe_alt(ctr, -1) / scale / scale;
          int max_y = need * 2 + 1;

          hyperpoint a = spin(90._deg*s) * point3(1, 0, 0);
          hyperpoint b = spin(90._deg*s) * point3(0, 1, 0);
          
          auto pt = [&] (ld af, ld bf, ld yf) {
            hyperpoint ha = a * af * scale; ha[3] = 1;
            hyperpoint hb = b * bf * scale; hb[3] = 1;
            hyperpoint res = rgpushxto0(ctr) * rgpushxto0(ha) * rgpushxto0(hb) * point31(0, 0, yf * scale * scale);
            cgi.hpcpush(res);
            };
          
          auto ptf = [&] (ld af, ld bf, ld yf) {
            pt(af, bf, yf);
            castle_tinf.tvertices.push_back(glhr::makevertex(bf, yf*4, 0));
            };          
          
          auto ptc = [&] (ld af, ld bf, ld yf, ld xt, ld yt) {
            pt(af, bf, yf);
            castle_tinf.tvertices.push_back(glhr::makevertex(xt, yt, 0));
            };          
          
          for(int w=0; w<2; w++)
          for(int as=0; as<8; as++)
            for(int y=0; y<max_y; y++) {
              ld xf = w ? .4 : .5;
              ld y1 = -y/2.;
              ld y2 = -(y+1)/2.;
              ld asd = (as-4) / 8.;
              ld asd1 = (as-3) / 8.;
              auto oasd = asd / 4;
              if(w) asd *= .8, asd1 *= .8, oasd *= .8 * .8;
              ptf(xf, asd, y1 - oasd);
              ptf(xf, asd1, y1 - oasd);
              ptf(xf, asd, y2 - oasd);
              ptf(xf, asd, y2 - oasd);
              ptf(xf, asd1, y1 - oasd);
              ptf(xf, asd1, y2 - oasd);
              }

          ld x1 = 1/32.;
          ld x2 = 1/4. + x1;
          ld y1 = 1/32.;
          ld y2 = 1/8. + x1;

          for(int as=0; as<8; as++) {
            ld asd = (as-4) / 8.;
            ld asd1 = (as-3) / 8.;
            ld asdw = asd * .8;
            ld asdw1 = asd1 * .8;
            ld asd2 = (as-5) / 8.;
            //ld asdw2 = asd2 * .8;
            ld oasd = asd / 4;
            ld oasdw = oasd * .8 * .8;
            ld oasd2 = asd2 / 4;
            ld oasdw2 = oasd2 * .8 * .8;
            /* tops */
            ptc(.5, asd, -oasd, x1, y1);
            ptc(.5, asd1, - oasd, x1, y2);
            ptc(.4, asdw, -oasdw, x2, y1);
            ptc(.4, asdw, -oasdw, x2, y1);
            ptc(.5, asd1, - oasd, x1, y2);
            ptc(.4, asdw1, -oasdw, x2, y2);
            /* sides */
            ptc(.5, asd, -oasd, x1, y1);
            ptc(.5, asd, -oasd2, x1, y2);
            ptc(.4, asdw, -oasdw, x2, y1);
            ptc(.4, asdw, -oasdw, x2, y1);
            ptc(.5, asd, -oasd2, x1, y2);
            ptc(.4, asdw, -oasdw2, x2, y2);
            }
          }
        }      
      }
    cgi.finishshape();
    }

  cgi.extra_vertices();
  }

void level::init() {
  if(initialized) return;
  initialized = true;
  check_cgi();

  real_minx = HUGE_VAL;
  real_miny = HUGE_VAL;
  real_maxx = -HUGE_VAL;
  real_maxy = -HUGE_VAL;

  if(flags & nrlPolar)
    scale = 1;
  else
    scale = abs(maxx - minx) / isize(map_tiles[0]);
  println(hlog, "SCALE IS ", this->scale);

  levellines_for = new_levellines_for = Hypc;

  if(1) {
    int tY = isize(map_tiles);
    int tX = isize(map_tiles[0]);

    for(int y=0; y<tY; y++)
    for(int x=0; x<tX; x++) {
      char bmch = map_tiles[y][x];
      
      if(bmch == 'o') {
        hyperpoint h = mappt(x+.5, y+.5, 1);
        h[2] += safe_alt(h) + 1;
        statues.emplace_back(statue{rgpushxto0(h), &shBall, 0xFFFFFFFF});
        }

      if(bmch == 'x') {
        hyperpoint h = mappt(x+.5, y+.5, 1);
        statues.emplace_back(statue{rgpushxto0(h), &shGeostatue, 0xFFFFFFFF});
        }

      if(bmch == '*') {
        hyperpoint h = mappt(x+.5, y+.5, 1);
        h[2] += safe_alt(h, .5, .85);
        triangledata d;
        d.where = h;
        d.x = x;
        d.y = y;
        for(int i=0; i<7; i++)
          d.colors[i] = gradient(0xFFD500FF, 0xFF, 0, i, 8);
        d.colors[6] = d.colors[0];
        triangles.emplace_back(d);
        }
      }
    cgi.finishshape();
    }

  start.where = mappt(startx+.5, starty+.5, 1);
  start.t = 0;
  start.timer = 0;
  current = start;
  println(hlog, "start.where = ", start.where);
  println(hlog, "current.where = ", current.where, " : ", format("%p", &current));
  
  int qgoals = isize(goals);
  records[0].resize(qgoals, 0);
  records[1].resize(qgoals, 0);
  current_score.resize(qgoals, 0);

  /* start facing slightly to the right from the slope */
  for(auto b: {true, false}) while(true) {
    auto c = start;
    /* no treasures are known, which confuses goals */
    dynamicval<bool> lop1(loaded_or_planned, true);
    dynamicval<bool> lop2(planning_mode, false);
    if(c.tick(this) == b) break;
    start.heading_angle -= degree;
    }

  if(flags & nrlOrder) {
    sort(triangles.begin(), triangles.end(), [this] (triangledata a, triangledata b) {
      return atan2(spin(120._deg)*(a.where - start.where)) < atan2(spin(120._deg)*(b.where - start.where));
      });
    for(auto t: triangles) println(hlog, t.where);
    }
  
  init_plan();
  }

xy_float level::get_xy_f(hyperpoint h) {
  if(flags & nrlPolar) {
    tie(h[0], h[1]) = make_pair(atan2(h[0], h[1]), hypot(h[0], h[1]));
    ld bar = (minx + maxx) / 2;
    cyclefix(h[0], bar);
    }
  int tY = isize(map_tiles);
  int tX = isize(map_tiles[0]);
  ld rtx = ilerp(minx, maxx, h[0]) * tX;
  ld rty = ilerp(miny, maxy, h[1]) * tY;
  return {rtx, rty};
  }

char level::mapchar(xy_int p) {
  auto x = p.first;
  auto y = p.second;
  int tY = isize(map_tiles);
  int tX = isize(map_tiles[0]);
  if(x < 0 || y < 0 || x >= tX || y >= tY) return '!';
  return map_tiles[y][x];
  }

/* convert ASCII map coordinates to Heisenberg coordinates */
hyperpoint level::mappt(ld x, ld y, int s) {
  int tY = isize(map_tiles);
  int tX = isize(map_tiles[0]);
  x /= s;
  y /= s;
  hyperpoint h;
  h[0] = lerp(minx, maxx, x / tX);
  h[1] = lerp(miny, maxy, y / tY);
  if(flags & nrlPolar)
    tie(h[0], h[1]) = make_pair(h[1] * sin(h[0]), h[1] * cos(h[0]));
  h[2] = surface(h);
  h[3] = 1;
  return h;
  };

void level::init_plan() {
  plan.emplace_back(start.where, hpxy(cos(start.heading_angle + 90._deg) * 2, sin(start.heading_angle + 90._deg) * 2));
  current = start;
  }

ld level::safe_alt(hyperpoint h, ld mul, ld mulx) {
  ld maxv = 0;
  for(int x: {-1, 0, 1})
  for(int y: {-1, 0, 1}) {
    hyperpoint c = sym_to_used(point31(x*.5*scale*mulx, y*.5*scale*mulx, 0));
    hyperpoint j = rgpushxto0(h) * c;
    maxv = max(maxv, mul * (surface(j) - j[2]));
    }    
  return maxv;
  }

bool stepped_display;

int nilrider_tempo = 562;
int nilrider_shift = 2633;

void level::draw_level(const shiftmatrix& V) {
  int id = 0;
  init_statues();
  curlev->init_shapes();
  curlev->init_textures();

  for(auto& t: triangles) {
    bool gotit = current.collected_triangles & Flag(id);
    id++;
    if(!gotit) {
      for(int i=0; i<6; i++) {
        ld tim = current.timer * 1000;
        tim -= nilrider_shift;
        tim /= nilrider_tempo;
        transmatrix spin = Id;
        if(nilv::model_used == 0) spin = cspin(0, 1, tim * M_PI / 6);
        tim = abs(0.2 * sin(tim * M_PI));
        auto &poly = queuepoly(V * rgpushxto0(t.where) * cpush(2, tim) * spin, shMini[i], t.colors[i]);
        poly.tinf = &floor_texture_vertices[cgi.shFloor.id];
        ensure_vertex_number(*poly.tinf, poly.cnt);
        }
      }
    }

  if(true) {
    auto& poly = queuepoly(V, shCastle, 0xC02020FF);
    poly.tinf = &castle_tinf;
    castle_tinf.texture_id = castle_texture->textureid;
    }
  
  for(auto st: statues) queuepoly(V * st.T, *st.shape, st.color);

  queuepoly(V, shField, 0xFFFF00FF);

  if(!stepped_display) {
    auto& poly = queuepoly(V, shFloor, 0xFFFFFFFF); // 0xFFFFFFFF);
    poly.tinf = &uniltinf;
    uniltinf.texture_id = unil_texture->textureid;
    }
  else {
    auto& poly = queuepoly(V, shStepFloor, 0xFFFFFFFF); // 0xFFFFFFFF);
    poly.tinf = &uniltinf_stepped;
    uniltinf_stepped.texture_id = unil_texture_stepped->textureid;
    }
  }

void cleanup_texture(texture::texture_data*& d) {
  if(d) delete d;
  d = nullptr;
  }

void cleanup_textures() {
  for(auto l: all_levels) {
    cleanup_texture(l->unil_texture);
    cleanup_texture(l->unil_texture_stepped);
    }
  println(hlog, "CLEANUP texture");
  cleanup_texture(castle_texture);
  }
}
