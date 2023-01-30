namespace hr {
namespace bricks {

  void draw_ro();

  extern void build(bool in_pair);

  #if !NILRIDER
  struct brick {
    euc::coord co;
    color_t col;
    int walls;
    hyperpoint location;
    hpcshape shRotWall[6];
    };
  #endif
  
  extern vector<brick> bricks;
  }
}

namespace nilrider {

hpcshape shMini[6];

void create_minitriangle() {
  using namespace bricks;
  bricks::bricks.clear();
  build(false);
  hyperpoint ctr = Hypc;
  for(auto& b: bricks::bricks) ctr += b.location;
  ctr /= ctr[3];
  transmatrix B = gpushxto0(ctr);
  
  ld radius = 0;
  ld sca = .18;
  
  for(int f=0; f<6; f++) {
    cgi.bshape(shMini[f], PPR::WALL);
    shMini[f].flags |= POLY_TRIANGLES;
    
    for(auto& b: bricks::bricks) {
      transmatrix V = eupush(b.location);

      int which = b.walls;
      if(!((1<<f) & which)) continue;

      auto& sh = b.shRotWall[f];
      for(int i=sh.s; i<sh.e; i++) {
        hyperpoint p = B * V * cgi.hpc[i];
        p[0] *= sca;
        p[1] *= sca;
        p[2] *= sca*sca;
        cgi.hpcpush(p);
        radius = max(radius, sqhypot_d(2, p));
        }
      }    
    cgi.finishshape();
    }    
  cgi.extra_vertices();
  println(hlog, "radius = ", radius);
  }

/** the texture data used for the castle walls */
texture::texture_data *castle_texture;

void create_castle() {
  if(!castle_texture) {
    castle_texture = new texture::texture_data;
    auto& tex = *castle_texture;
    tex.twidth = 16;
    tex.theight = 16;
    tex.stretched = false;
    tex.strx = tex.tx;
    tex.stry = tex.ty;
    tex.base_x = 0;
    tex.base_y = 0;
    tex.whitetexture();
    for(int y=0; y<16; y++)
    for(int x=0; x<16; x++)
      tex.get_texture_pixel(x, y) = bcols[submaps['b'][y][x]];
    tex.loadTextureGL();
    }
  }

hpcshape shBall;

bool open_grid(int x, int y) {
  return among(y&3, 1, 2) || among(x&3, 0, 3);
  }

bool closed_grid(int x, int y) {
  return among(y&3, 0, 3) || among(x&3, 0, 3);
  }

template<class T1, class T2> void add_statue(const T1& grid, const T2& f) {
  auto pt = [&] (int x, int y) {
    ld x1 = x * TAU / 32;
    ld y1 = y * TAU / 64;
    cgi.hpcpush(f(x1,y1));
    };
  for(int y=-16; y<16; y++)
  for(int x=-16; x<16; x++) if(grid(x,y)) {
    pt(x, y);
    pt(x, y+1);
    pt(x+1, y);
    pt(x+1, y);
    pt(x, y+1);
    pt(x+1, y+1);
    }    
  }

hpcshape shGeostatue;

void init_statues() {
  check_cgi();
  if(cgi.ext.count("nil-statues")) return;
  cgi.ext["nil-statues"] = nullptr;
  create_minitriangle();
  
  cgi.bshape(shBall, PPR::WALL);
  shBall.flags |= POLY_TRIANGLES;
  add_statue(open_grid, [] (ld lon, ld lat) { return direct_exp(point3(cos(lat)*sin(lon)*.5, cos(lat)*cos(lon)*.5, sin(lat)*.5)); });
  cgi.finishshape();
  cgi.extra_vertices();
  cgi.add_texture(shBall);

  cgi.bshape(shGeostatue, PPR::WALL);
  shGeostatue.flags |= POLY_TRIANGLES;
  for(int i=0; i<8; i++) {
    hyperpoint z = point31(0, 1e-6, 8);
    hyperpoint ih = inverse_exp(shiftless(z));
    ih = spin(i * TAU / 8) * ih;
    add_statue(closed_grid, [&] (ld lon, ld lat) {
      lat = lat * .75;
      hyperpoint h = direct_exp(ih * (.5 + lat / M_PI));
      return rgpushxto0(h) * sym_to_used(point31(sin(lon)*.1, cos(lon)*.1, 0));
      });
    }
  cgi.finishshape();
  cgi.extra_vertices();
  cgi.add_texture(shGeostatue);

  for(ld z: vector<ld> {90._deg+1e-2, M_PI+1e-2, TAU+1e-2, 7, 10})
  for(hyperpoint h: {point31(0, 0, z), point31(1e-3, 0, z), point31(1e-6, 0, z), point31(0, 1e-6, z)}) {
    hyperpoint i = inverse_exp(shiftless(h));
    println(hlog, i, " @ ", hypot_d(3, i));
    }
  } 

}
