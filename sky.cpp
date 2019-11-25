#include "hyper.h"

namespace hr {

#if MAXMDIM >= 4
EX ld camera_level;

EX int get_skybrightness(int mul IS(1)) {
  ld s = 1 - mul * (camera_level - cgi.WALL) / -2;
  if(s > 1) return 255;
  if(s < 0) return 0;
  return int(s * 255);
  }

struct sky_item {
  cell *c;
  transmatrix T;
  color_t color;
  sky_item(cell *_c, const struct transmatrix _T, color_t _color) : c(_c), T(_T), color(_color) {}
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
    transmatrix T = ggmatrix(currentmap->gamestart());
    T = gpushxto0(tC0(T)) * T;
    queuepoly(T, cgi.shEuclideanSky, 0x0044e4FF);
    queuepolyat(T * zpush(cgi.SKY+0.5) * xpush(cgi.SKY+0.5), cgi.shSun, 0xFFFF00FF, PPR::SKY);
    }
  else {
    sky = &queuea<dqi_sky> (PPR::SKY);
    }
  }

void dqi_sky::draw() {

  if(!vid.usingGL || sky.empty()) return;
  vector<glhr::colored_vertex> skyvertices;

  int sk = get_skybrightness();
  
  unordered_map<cell*, color_t> colors;
  #ifdef USE_UNORDERED_MAP
  colors.reserve(isize(sky));
  #endif
  for(sky_item& si: sky) colors[si.c] = darkena(gradient(0, si.color, 0, sk, 255), 0, 0xFF);
  
  hyperpoint skypoint = cpush0(2, cgi.SKY);
  
  vector<glhr::colored_vertex> this_poly;
  
  // I am not sure why, but extra projection martix introduced in stereo
  // causes some vertices to not be drawn. Thus we apply separately
  transmatrix Tsh = Id;
  if(global_projection)
    Tsh = xpush(vid.ipd * global_projection/2);

  for(sky_item& si: sky) {
    auto c = si.c;
    for(int i=0; i<c->type; i++) {
      
      if(1) {
        cellwalker cw0(c, i);
        cellwalker cw = cw0;
        do {
          cw += wstep; cw++;
          if(cw.at < c || !colors.count(si.c)) goto next;
          }
        while(cw != cw0);
          
        this_poly.clear();
  
        transmatrix T1 = Tsh * si.T;
        do {
          this_poly.emplace_back(T1 * skypoint, colors[cw.at]);
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

  for(int ed = current_display->stereo_active() ? -1 : 0; ed<2; ed+=2) {
    if(global_projection && global_projection != ed) continue;
    current_display->next_shader_flags = GF_VARCOLOR;
    current_display->set_all(ed);
    if(global_projection)
      glhr::projection_multiply(glhr::tmtogl(xpush(-vid.ipd * global_projection/2)));
    glapplymatrix(Id);
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
  if(c->land == laHell)
    return z < 32 ? gradient(0x400000, 0xFF0000, 0, z, 32) : gradient(0xFF0000, 0xFFFF00, 32, z, 63);
  else
    return gradient(0x4040FF, 0xFFFFFF, 0, z, 63);
  }

void celldrawer::draw_ceiling() {

  if(pmodel != mdPerspective || sphere) return;
  
  switch(ceiling_category(c)) {
    /* ceilingless levels */
    case 1: {
      if(euclid) return;
      if(fieldpattern::fieldval_uniq(c) % 3 == 0) {
        queuepolyat(V * zpush(cgi.SKY+1), cgi.shNightStar, 0xFFFFFFFF, PPR::SKY);
        }
      if(sky) sky->sky.emplace_back(sky_item{c, V, 0x00000F});
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
      
      switch(c->land) {
        case laWineyard:
          col = 0x4040FF;
          if(emeraldval(c) / 4 == 11) {
            queuepolyat(V * zpush(cgi.SKY+1), cgi.shSun, 0xFFFF00FF, PPR::SKY);
            }
          break;

        case laPower:
          col = c->landparam ? 0xFF2010 : 0x000020;
          break;
        
        case laDesert:
        case laRedRock:
          col = 0x4040FF;
          break;
        
        case laAlchemist:
          col = fcol;
          break;
        
        case laVariant: {
          int b = getBits(c);
          col = 0x404040;
          for(int a=0; a<21; a++)
            if((b >> a) & 1)
              col += variant_features[a].color_change;
          col = col & 0x00FF00;
          break;
          }
        
        case laDragon:
          col = c->wall == waChasm ? 0xFFFFFF : 0x4040FF;
          break;
        
        default: 
          col = skycolor(c);        
        }
      if(sky) sky->sky.emplace_back(c, V, col);
      return;
      }
    
    case 3: {
      if(sky) sky->sky.emplace_back(c, V, 0);
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
      if(sky) sky->sky.emplace_back(c, V, 0x00000F);
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
      if(sky) sky->sky.emplace_back(c, V, skycolor(c));
      if(camera_level <= cgi.HIGH2) return;
      color_t wcol2 = winf[waRuinWall].color;
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
    
    case 7: {
      if(sky) sky->sky.emplace_back(c, V, 0x00000F);
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
      if(sky) sky->sky.emplace_back(c, V, 0x00000F);
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
#endif
}
