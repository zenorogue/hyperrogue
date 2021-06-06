#include "hyper.h"

namespace hr {

EX bool context_fog = true;

EX ld camera_level;
#if MAXMDIM >= 4 && CAP_GL

EX int get_skybrightness(int mul IS(1)) {
  ld s = 1 - mul * (camera_level - cgi.WALL) / -2;
  if(s > 1) return 255;
  if(s < 0) return 0;
  return int(s * 255);
  }

struct sky_item {
  cell *c;
  shiftmatrix T;
  color_t color;
  color_t skycolor;
  sky_item(cell *_c, const struct shiftmatrix _T, color_t _color, color_t _skycolor) : c(_c), T(_T), color(_color), skycolor(_skycolor) {}
  };

struct dqi_sky : drawqueueitem {
  vector<sky_item> sky;
  void draw();
  virtual color_t outline_group() { return 3; }
  // singleton
  dqi_sky() { hr::sky = this; }
  ~dqi_sky() { hr::sky = NULL; }
  };
  
EX struct dqi_sky *sky;

EX void prepare_sky() {
  sky = NULL;
  if(euclid) {
    if(WDIM == 3 || GDIM == 2) return;
    if(no_wall_rendering) return;
    shiftmatrix T = ggmatrix(currentmap->gamestart());
    T.T = gpushxto0(tC0(T.T)) * T.T;
    queuepoly(T, cgi.shEuclideanSky, 0x0044e4FF);
    queuepolyat(T * zpush(cgi.SKY+0.5) * xpush(cgi.SKY+0.5), cgi.shSun, 0xFFFF00FF, PPR::SKY);
    }
  else if(!(cgflags & qIDEAL)) {
    sky = &queuea<dqi_sky> (PPR::MISSILE);
    }
  }

vector<glhr::colored_vertex> skyvertices;
cell *sky_centerover;
shiftmatrix sky_cview;

void compute_skyvertices(const vector<sky_item>& sky) {
  skyvertices.clear();

  int sk = get_skybrightness();
  
  std::map<cell*, pair<color_t, color_t>> colors;
  for(const sky_item& si: sky) colors[si.c] = 
    make_pair(darkena(gradient(0, si.color, 0, sk, 255), 0, 0xFF),
        darkena(si.skycolor, 0, 0xFF)
        );
  
  hyperpoint skypoint = cpush0(2, cgi.SKY);
  hyperpoint hellpoint = cpush0(2, -cgi.SKY);
  
  vector<glhr::colored_vertex> this_poly;
  
  for(const sky_item& si: sky) {
    auto c = si.c;
    
    if(c->land == laMirrorWall) continue;
    bool inmir = false;
    forCellEx(c1, c) if(c1->land == laMirrorWall) inmir = true;
    if(inmir) continue;
    
    for(int i=0; i<c->type; i++) {
      
      if(1) {
        cellwalker cw0(c, i);
        cellwalker cw2 = cw0;
        cw2--; cw2 += wstep;
        if(!colors.count(cw2.at)) {
          this_poly.clear();
          transmatrix T1 = unshift(si.T);
          auto cw = cw0;
          while(colors.count(cw.at)) {
            color_t col = colors[cw.at].second;
            this_poly.emplace_back(T1 * skypoint, colors[cw.at].first);
            this_poly.emplace_back(T1 * hellpoint, col);
            T1 = T1 * currentmap->adj(cw.at, cw.spin);
            cw += wstep; cw++;
            }

          int k = isize(this_poly);
          for(int j=2; j<k; j+=2) {
            skyvertices.push_back(this_poly[j-2]);
            skyvertices.push_back(this_poly[j-1]);
            skyvertices.push_back(this_poly[j]);
            skyvertices.push_back(this_poly[j-1]);
            skyvertices.push_back(this_poly[j]);
            skyvertices.push_back(this_poly[j+1]);
            }
          goto next;
          }        
        }

      if(true) {
        cellwalker cw0(c, i);
        cellwalker cw = cw0;
        do {
          cw += wstep; cw++;
          if(cw.at < c || !colors.count(cw.at)) goto next;
          }
        while(cw != cw0);
          
        this_poly.clear();
  
        transmatrix T1 = unshift(si.T);
        do {
          this_poly.emplace_back(T1 * skypoint, colors[cw.at].first);
          T1 = T1 * currentmap->adj(cw.at, cw.spin);
          cw += wstep; cw++;
          }
        while(cw != cw0);
  
        int k = isize(this_poly);
        for(int j=2; j<k; j++) {
          skyvertices.push_back(this_poly[0]);
          skyvertices.push_back(this_poly[j-1]);
          skyvertices.push_back(this_poly[j]);
          }
        }

      next: ;
      }
    }

  for(auto& v: skyvertices) for(int i=0; i<3; i++) v.color[i] *= 2;
  }

void dqi_sky::draw() {
  if(!vid.usingGL || sky.empty()) return;
  
  #if CAP_VR
  transmatrix s = (vrhr::rendering() ? vrhr::master_cview : cview()).T * inverse(sky_cview.T);
  #else
  transmatrix s = cview().T * inverse(sky_cview.T);
  #endif
    
  for(int ed = current_display->stereo_active() ? -1 : 0; ed<2; ed+=2) {
    if(global_projection && global_projection != ed) continue;
    current_display->next_shader_flags = GF_VARCOLOR;
    current_display->set_all(ed, 0);
    if(global_projection) {
      glhr::projection_multiply(glhr::tmtogl(xpush(-vid.ipd * global_projection/2)));
      glapplymatrix(xpush(vid.ipd * global_projection/2) * s);
      }
    else {
      glapplymatrix(s);
      }
    glhr::prepare(skyvertices);
    glhr::set_fogbase(1.0 + 5 / sightranges[geometry]);
    glhr::set_depthtest(model_needs_depth() && prio < PPR::SUPERLINE);
    glhr::set_depthwrite(model_needs_depth() && prio != PPR::TRANSPARENT_SHADOW && prio != PPR::EUCLIDEAN_SKY);
    glDrawArrays(GL_TRIANGLES, 0, isize(skyvertices));
    }
  }

color_t skycolor(cell *c) {
  int cd = (euclid || stdhyperbolic) ? getCdata(c, 1) : 0;
  int z = (cd * 5) & 127;
  if(z >= 64) z = 127 - z;
  return gradient(0x4040FF, 0xFFFFFF, 0, z, 63);
  }

void celldrawer::draw_ceiling() {

  if(pmodel != mdPerspective || sphere) return;
  
  auto add_to_sky = [this] (color_t col, color_t col2) {
    if(cgflags & qIDEAL)
      draw_shapevec(c, V, qfi.fshape->levels[SIDE_HIGH], darkena(col, 0, 0xFF), PPR::WALL);
    else if(sky) sky->sky.emplace_back(c, V, col, col2);
    };
  
  switch(ceiling_category(c)) {
    /* ceilingless levels */
    case 1: {
      if(euclid) return;
      if(fieldpattern::fieldval_uniq(c) % 3 == 0) {
        queuepolyat(V * zpush(cgi.SKY+1), cgi.shNightStar, 0xFFFFFFFF, PPR::SKY);
        }
      add_to_sky(0x00000F, 0x00000F);
      if(c->land == laAsteroids) {
        if(fieldpattern::fieldval_uniq(c) % 9 < 3) {
          queuepolyat(V * zpush(-1-cgi.SKY), cgi.shNightStar, 0xFFFFFFFF, PPR::SKY);
          }
        int sk = get_skybrightness(-1);
        auto sky = draw_shapevec(c, V * MirrorZ, cgi.shFullFloor.levels[SIDE_SKY], 0x000000FF + 0x100 * (sk/17), PPR::SKY);
        if(sky) sky->tinf = NULL, sky->flags |= POLY_INTENSE;
        }
      return;
      }
    
    case 2: {
      if(euclid) return;
      color_t col;
      color_t skycol;
      
      switch(c->land) {
        case laWineyard:
          col = 0x4040FF;
          skycol = 0x8080FF;
          if(emeraldval(c) / 4 == 11) {
            queuepolyat(V * zpush(cgi.SKY+1), cgi.shSun, 0xFFFF00FF, PPR::SKY);
            }
          break;

        case laDesert:
          col = 0x2020C0;
          skycol = 0x8080FF;
          if(emeraldval(c) / 4 == 11) {
            queuepolyat(V * zpush(cgi.SKY+1), cgi.shSun, 0xFFFF00FF, PPR::SKY);
            }
          break;

        case laFrog:
          col = 0x4040FF;
          skycol = 0x8080FF;
          if(zebra40(c) / 4 == 1) {
            queuepolyat(V * zpush(cgi.SKY+1), cgi.shSun, 0xFFFF00FF, PPR::SKY);
            }
          break;

        case laPower:
          skycol = col = c->landparam ? 0xFF2010 : 0x000020;
          break;
        
        /* case laDesert:
          col = 0x4040FF;
          skycol = (0xCDA98F & 0xFEFEFE) / 2;
          break; */
        
        case laAlchemist:
          skycol = col = fcol;
          break;
        
        case laVariant: {
          #if CAP_COMPLEX2
          int b = getBits(c);
          col = 0x404040;
          for(int a=0; a<21; a++)
            if((b >> a) & 1)
              col += variant::features[a].color_change;
          col = col & 0x00FF00;
          skycol = col;
          #endif
          break;
          }
        
        case laDragon:
          col = c->wall == waChasm ? 0xFFFFFF : 0x4040FF;
          skycol = 0;
          break;
        
        case laHell: {
          int a = 0;
          forCellEx(c1, c) if(among(c1->wall, waSulphur, waSulphurC)) a++;
          ld z = a * 1. / c->type;
          if(z < .5)
            col = gradient(0x400000, 0xFF0000, 0, z, .5);
          else
            col = gradient(0xFF0000, 0xFFFF00, .5, z, 1);
          skycol = col;
          break;
          }
        
        default: {
          col = skycolor(c);        
          skycol = 0xA0A0FF;
          }
        }
      add_to_sky(col, skycol);
      return;
      }
    
    case 3: {
      add_to_sky(0, 0);
      if(camera_level <= cgi.WALL) return;
      if(c->land == laMercuryRiver) fcol = linf[laTerracotta].color, fd = 1;
      if(qfi.fshape) draw_shapevec(c, V, qfi.fshape->levels[SIDE_WALL], darkena(fcol, fd, 0xFF), PPR::WALL);
      forCellIdEx(c2, i, c) 
        if(ceiling_category(c2) != 3) {
          color_t wcol2 = gradient(0, wcol, 0, .8, 1);
          placeSidewall(c, i, SIDE_HIGH, V, darkena(wcol2, fd, 0xFF));
          placeSidewall(c, i, SIDE_HIGH2, V, darkena(wcol2, fd, 0xFF));
          placeSidewall(c, i, SIDE_SKY, V, darkena(wcol2, fd, 0xFF));
          }
      return;
      }
    
    case 4: {
      add_to_sky(0x00000F, 0x00000F);
      if(camera_level <= cgi.HIGH2) return;
      auto ispal = [&] (cell *c0) { return c0->land == laPalace && among(c0->wall, waPalace, waClosedGate, waOpenGate); };
      color_t wcol2 = 0xFFD500;
      if(ispal(c)) {
        forCellIdEx(c2, i, c) if(!ispal(c2))
          placeSidewall(c, i, SIDE_HIGH, V, darkena(wcol2, fd, 0xFF));
        }
      else {
        bool window = false;
        forCellIdEx(c2, i, c) if(c2->wall == waPalace && ispal(c->cmodmove(i+1)) && ispal(c->cmodmove(i-1))) window = true;        
        if(qfi.fshape && !window) draw_shapevec(c, V, qfi.fshape->levels[SIDE_HIGH], darkena(fcol, fd, 0xFF), PPR::WALL);
        if(window) 
          forCellIdEx(c2, i, c) 
            placeSidewall(c, i, SIDE_HIGH2, V, darkena(wcol2, fd, 0xFF));
        }
      if(among(c->wall, waClosedGate, waOpenGate) && qfi.fshape) draw_shapevec(c, V, qfi.fshape->levels[SIDE_WALL], 0x202020FF, PPR::WALL);
      if(euclid) return;

      if(true) {
        queuepolyat(V * zpush(cgi.SKY+0.5), cgi.shNightStar, 0xFFFFFFFF, PPR::SKY);
        }
      break;
      }
    
    case 6: {
      add_to_sky(skycolor(c), 0x4040C0);
      if(camera_level <= cgi.HIGH2) return;
      color_t wcol2 = winf[waRuinWall].color;
      if(c->landparam == 1)
        forCellIdEx(c2, i, c) if(c2->landparam != 1)
          placeSidewall(c, i, SIDE_HIGH, V, darkena(wcol2, fd, 0xFF));
      if(c->landparam != 2)
        forCellIdEx(c2, i, c) if(c2->landparam == 2)
          placeSidewall(c, i, SIDE_HIGH2, V, darkena(wcol2, fd, 0xFF));
      /* if(c->landparam == 0)
        if(qfi.fshape) draw_shapevec(c, V, qfi.fshape->levels[SIDE_HIGH], darkena(wcol2, fd, 0xFF), PPR::WALL); */
      if(c->landparam == 1)
        if(qfi.fshape) draw_shapevec(c, V, qfi.fshape->levels[SIDE_WALL], darkena(wcol2, fd, 0xFF), PPR::WALL);
      break;
      }
    
    case 7: {
      add_to_sky(0x00000F, 0x00000F);
      if(fieldpattern::fieldval_uniq(c) % 5 < 2) {
        queuepolyat(V * zpush(cgi.SKY+1), cgi.shNightStar, 0xFFFFFFFF, PPR::SKY);
        }
      if(camera_level <= cgi.HIGH2) return;
      color_t wcol2 = winf[waColumn].color;
      if(c->landparam == 1)
        forCellIdEx(c2, i, c) if(c2->landparam != 1)
          placeSidewall(c, i, SIDE_HIGH, V, darkena(wcol2, fd, 0xFF));
      if(c->landparam != 2)
        forCellIdEx(c2, i, c) if(c2->landparam == 2)
          placeSidewall(c, i, SIDE_HIGH2, V, darkena(wcol2, fd, 0xFF));
      if(c->landparam == 0)
        if(qfi.fshape) draw_shapevec(c, V, qfi.fshape->levels[SIDE_HIGH], darkena(wcol2, fd, 0xFF), PPR::WALL);
      if(c->landparam == 1)
        if(qfi.fshape) draw_shapevec(c, V, qfi.fshape->levels[SIDE_WALL], darkena(wcol2, fd, 0xFF), PPR::WALL);
      break;
      }
    
    case 5: {
      add_to_sky(0x00000F, 0x00000F);
      if(camera_level <= cgi.WALL) return;
    
      if(pseudohept(c)) {
        forCellIdEx(c2, i, c) 
          placeSidewall(c, i, SIDE_HIGH, V, darkena(fcol, fd, 0xFF));
        }
      else if(qfi.fshape)
        draw_shapevec(c, V, qfi.fshape->levels[SIDE_WALL], darkena(fcol, fd, 0xFF), PPR::WALL);

      if(euclid) return;
      if(true) {
        queuepolyat(V * zpush(cgi.SKY+0.5), cgi.shNightStar, 0xFFFFFFFF, PPR::SKY);
        }
      }
    }
  }

EX struct renderbuffer *airbuf;

EX void make_air() {
  if(!sky) return;

  if(centerover != sky_centerover) {
    sky_centerover = centerover;
    sky_cview = cview();
    compute_skyvertices(sky->sky);
    }
  
  if(!context_fog) return;

  const int AIR_TEXTURE = 512;
  if(!airbuf) {
    airbuf = new renderbuffer(AIR_TEXTURE, AIR_TEXTURE, true);
    if(!airbuf->valid) {
      delete airbuf;
      airbuf = nullptr;
      println(hlog, "unable to make airbuf");
      return;
      }
    }

  #if CAP_VR
  dynamicval<int> i(vrhr::state, 0);
  #endif

  if(1) {
    //shot::take("airtest.png", drawqueue); 
    dynamicval<videopar> v(vid, vid);    
    dynamicval<bool> vi(inHighQual, true);

    vid.xres = AIR_TEXTURE;
    vid.yres = AIR_TEXTURE;
    calcparam();
    models::configure();
  
    resetbuffer rb;
    airbuf->enable();
    current_display->set_viewport(0);
  
    airbuf->clear(0xFFFF00FF);

    pconf.alpha = 1;
    pconf.scale = 1;
    pconf.camera_angle = 0;
    pconf.stretch = 1;
    pmodel = mdDisk;

    vid.always3 = false;
    geom3::apply_always3();    
    check_cgi();
    cgi.require_shapes();
    
    eGeometry orig = geometry;

    #if !ISIOS
    glDisable(GL_LINE_SMOOTH);
    #endif
    
    for(auto& g: sky->sky) {
      transmatrix S;
      if(1) {
        geometry = gSpace534;
        S = g.T.T;
        S = radar_transform * S;
        geometry = orig;
        swapmatrix(S);
        }
      
      auto& h = cgi.shFullFloor.b[shvid(g.c)];

      dqi_poly p;
      p.V = shiftless(S);
      p.offset = h.s;
      p.cnt = h.e - h.s;
      p.tab = &cgi.ourshape;
      p.color = (g.skycolor << 8) | 0xFF;
      p.outline = 0;
      
      p.linewidth = 1;
      p.flags = POLY_FORCEWIDE;
      p.tinf = nullptr;
      
      p.draw();
      }

    #if !ISIOS
    if(vid.antialias & AA_LINES)
      glEnable(GL_LINE_SMOOTH);
    #endif

#if CAP_SDL
    if(anyshiftclick) IMAGESAVE(airbuf->render(), "air.png");
#endif
    rb.reset();
    }

  GLERR("after draw");
  geom3::apply_always3();
  check_cgi();
  calcparam();
  GLERR("after make_air");
  current_display->set_viewport(0);
  current_display->set_all(0,0);
  }

#endif
}
