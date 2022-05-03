namespace nilrider {

void level::init() {
  if(initialized) return;
  initialized = true;

  unil_texture = new texture::texture_data;
  
  auto& tex = *unil_texture;
  
  real_minx = HUGE_VAL;
  real_miny = HUGE_VAL;
  real_maxx = -HUGE_VAL;
  real_maxy = -HUGE_VAL;
  
  if(flags & nrlPolar)
    scale = 1;
  else
    scale = abs(maxx - minx) / isize(map_tiles[0]);
  
  println(hlog, "SCALE IS ", this->scale);

  int tY = isize(map_tiles);
  int tX = isize(map_tiles[0]);
  
  tex.twidth = tex.tx = tX * 64;
  tex.theight = tex.ty = tY * 64;
  tex.stretched = false;
  tex.strx = tex.tx;
  tex.stry = tex.ty;
  tex.base_x = 0;
  tex.base_y = 0;
  tex.whitetexture();
  println(hlog, "tX=", tX, " tY=", tY, " tex=", tex.tx, "x", tex.ty);
  
  for(int y=0; y<tex.ty; y++)
  for(int x=0; x<tex.tx; x++) {
    int bx = x / 64;
    int by = y / 64;    
    char bmch = map_tiles[by][bx];
    int sx = (x % 64) / 4;
    int sy = (y % 64) / 4;
    if(bmch == '!') continue;
    char smch = submaps[bmch][sy][sx];
    tex.get_texture_pixel(x, y) = bcols[smch];
    tex.get_texture_pixel(x, y) |= 0xFF000000;
    tex.get_texture_pixel(x, y) ^= hrand(0x1000000) & 0xF0F0F;
    // (x * 256 / TEXSIZE + (((y * 256) / TEXSIZE) << 8)) | 0xFF000000;
    }
  tex.loadTextureGL();
  
  start.where = mappt(startx+.5, starty+.5, 1);
  start.t = 0;
  current = start;
  println(hlog, "start.where = ", start.where);
  println(hlog, "current.where = ", current.where, " : ", format("%p", &current));
  
  for(int s=0; s<2; s++) {  
    cgi.bshape(s == 0 ? shFloor : shPlanFloor, PPR::WALL);
    shFloor.flags |= POLY_TRIANGLES;
    shPlanFloor.flags |= POLY_TRIANGLES;
    
    auto pt = [&] (int x, int y) {
      if(s == 0) uniltinf.tvertices.push_back(glhr::makevertex(x * 1. / tX / 16, y * 1. / tY / 16, 0));
      hyperpoint h = mappt(x, y, 16);
      real_minx = min(real_minx, h[0]);
      real_maxx = max(real_maxx, h[0]);
      real_miny = min(real_miny, h[1]);
      real_maxy = max(real_maxy, h[1]);
      if(s == 1) h[2] = h[3] = 1;
      // h[2] = h[0] * h[1] / 2 + .1;
      // h[3] = 1;
      cgi.hpcpush(h);
      // println(hlog, "entered ", h);
      // cgi.hpcpush(hyperpoint(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 5, 1));
      };

    for(int y=0; y<tY * 16; y++)
    for(int x=0; x<tX * 16; x++) {
      char bmch = map_tiles[y/16][x/16];
      if(bmch == '!') continue;
      pt(x, y);
      pt(x, y+1);
      pt(x+1, y);
      pt(x+1, y+1);
      pt(x+1, y);
      pt(x, y+1);
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
          hyperpoint a = spin(90*degree*s) * point3(.1, .1, 0);
          hyperpoint b = spin(90*degree*(s+1)) * point3(.1, .1, 0);
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

          hyperpoint a = spin(90*degree*s) * point3(1, 0, 0);
          hyperpoint b = spin(90*degree*s) * point3(0, 1, 0);
          
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
        triangles.emplace_back(d);
        }
      }
    cgi.finishshape();
    // println(hlog, shFloor[i].s, " to ", shFloor[i].e);
    }
  
  cgi.extra_vertices();
  init_plan();
  }

xy_float level::get_xy_f(hyperpoint h) {
  if(flags & nrlPolar) {
    tie(h[0], h[1]) = make_pair(atan2(h[0], h[1]), hypot(h[0], h[1]));
    ld bar = (minx + maxx) / 2;
    while(h[0] < bar - M_PI) h[0] += 2 * M_PI;
    while(h[0] > bar + M_PI) h[0] -= 2 * M_PI;
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

/*
  plan.emplace_back(start.where, hpxy(0, 1));
  plan.emplace_back(mappt(4.5, 10.5), hpxy(1, 1));
  plan.emplace_back(mappt(0.01, 3, 1), hpxy(0, -2));
  plan.emplace_back(mappt(2, 3.99, 1), hpxy(4, 0));
  plan.emplace_back(mappt(42, 3.99, 1), hpxy(4, 0));
*/

void level::init_plan() {
  plan.emplace_back(start.where, hpxy(0, -1));
  plan.emplace_back(mappt(6.8, 10.2, 1), hpxy(1.5, -1.5));
  plan.emplace_back(mappt(10.5, 10.5, 1), hpxy(1.5, 1.5));
  plan.emplace_back(mappt(10.5, 4.5, 1), hpxy(-1.5, 1.5));
  plan.emplace_back(mappt(4.5, 4.5, 1), hpxy(-1.5, 1.5));
  plan.emplace_back(mappt(4.5, 2, 1), hpxy(1.5, 0.5));
  plan.emplace_back(mappt(10.5, 2, 1), hpxy(1.5, -0.5));
  plan.emplace_back(mappt(10.5, 4.5, 1), hpxy(-2, 0));
  plan.emplace_back(mappt(6.5, 6.5, 1), hpxy(-1.5, -1.5));
  plan.emplace_back(mappt(4.5, 10.5, 1), hpxy(1.5, -1.5));
  plan.emplace_back(mappt(10.5, 9.5, 1), hpxy(1.5, 1.5));
/*  plan.emplace_back(mappt(0.01, 3, 1), hpxy(0, -2));
  plan.emplace_back(mappt(2, 3.99, 1), hpxy(4, 0));
  plan.emplace_back(mappt(42, 3.99, 1), hpxy(4, 0));
*/
  current = start;
  timer = 0;
  }

ld level::safe_alt(hyperpoint h, ld mul, ld mulx) {
  ld maxv = 0;
  for(int x: {-1, 0, 1})
  for(int y: {-1, 0, 1}) {
    hyperpoint c = sym_to_heis(point31(x*.5*scale*mulx, y*.5*scale*mulx, 0));
    hyperpoint j = rgpushxto0(h) * c;
    maxv = max(maxv, mul * (surface(j) - j[2]));
    }    
  return maxv;
  }

void level::draw_level(const shiftmatrix& V) {
  int id = 0;
  for(auto& t: triangles) {
    bool gotit = current.collected_triangles & Flag(id);
    id++;
    if(!gotit) {
      for(int i=0; i<6; i++) {
        auto &poly = queuepoly(V * rgpushxto0(t.where) * cpush(2, abs(0.2 * sin(timer * 5))), shMini[i], t.colors[i]);
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

  auto& poly = queuepoly(V, shFloor, 0xFFFFFFFF); // 0xFFFFFFFF);
  poly.tinf = &uniltinf;
  uniltinf.texture_id = unil_texture->textureid;  
  }
  
}
