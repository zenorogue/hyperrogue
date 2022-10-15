namespace hr {

namespace ads_game {

struct earth_texture {
  string filename;
  string copyright;
  texture::texture_data tex;
  basic_textureinfo tinf;
  bool loaded;
  };

vector<earth_texture> earth_textures;

struct texture_to_use {
  ld from, ctr, to;
  earth_texture *tx;
  };

vector<texture_to_use> textures_to_use;

void reset_textures() {
  textures_to_use.clear();
  }

void init_textures() {
  earth_textures.clear();
  string dir = "ds-images/";
  fhstream f(dir + "list.txt", "rt");
  if(!f.f) return;
  string cur_copyright = "";
  while(!feof(f.f)) {
    string s = scanline_noblank(f);
    if(s == "") continue;
    if(s[0] == '#') continue;
    if(s[0] == '$') { cur_copyright = s.substr(1); continue; }
    if(s.substr(0, 2) == "*/") s = dir + s.substr(2);
    earth_textures.emplace_back();
    auto& et = earth_textures.back();
    println(hlog, "trying '", s, "'");
    et.filename = s;
    et.copyright = cur_copyright;
    et.loaded = false;

    auto& tex = et.tex;
    tex.readtexture(s);
    int tty = tex.theight - 2 * tex.base_y;
    int u = 1;
    for(int y=0; y<tex.theight; y++)
    for(int x=0; x<tex.twidth; x++)
      tex.get_texture_pixel(x, y) =
      tex.get_texture_pixel(x, tex.base_y + u + gmod(y - tex.base_y - u, tty - u - u));
    }
  }

void pick_textures() {
  if(earth_textures.empty()) return;
  bool first = !isize(textures_to_use);
  auto last = first ? 0 : textures_to_use.back().to;
  if(first || current.shift > last) {
    auto et = &(earth_textures[hrand(isize(earth_textures))]);
    textures_to_use.emplace_back();
    auto& tu = textures_to_use.back();
    auto& tex = et->tex;
    ld ratio = tex.tx * 1. / tex.ty;
    ld length = M_PI * ratio + 6;
    ld t = first ? 0 : last + lerp(1, 3, randd()) + length;
    tu.ctr = t;
    tu.from = t - length;
    tu.to = t + length;
    tu.tx = et;
    }
  }

ld smoothstep(ld x) {
  if(x < 0) return 0;
  if(x > 1) return 1;
  return x * x * (3-2*x);
  }

string copyright_shown;

void draw_texture(texture_to_use& tu) {
  if(!talpha) return;
  auto& et = *tu.tx;
  auto& tex = et.tex;

  if(!et.loaded) {
    et.loaded = true;
    tex.loadTextureGL();
    et.tinf.texture_id = tex.textureid;
    }
  et.tinf.tvertices.clear();
  
  ld MWIDTH = tex.tx * .5 / tex.ty;

  array<hyperpoint, 3> pts;
  int pts_id = 0;

  auto add = [&] (int x, int y) {
    ld x0 = (y-(YSCALE/2.)) / YSCALE * M_PI * 4;
    ld mercator_y0 = (x-(XSCALE/2.)) / (XSCALE/2.) * M_PI * MWIDTH;
    ld y0 = asin(tanh(2 * mercator_y0));
    ld y1 = y0 - 90 * degree;
    
    et.tinf.tvertices.push_back(glhr::makevertex(x * 1. / XSCALE, .5 + (y-0.5) / MWIDTH / YSCALE, 0));

    cross_result cr;
    if(1) {
      dynamicval<eGeometry> g(geometry, gSpace435);
      ld s = current.shift - tu.ctr;

      // We actually want to compute this, but this is not precise enough:
      // cr = ds_cross0(current.T * lorentz(2, 3, -current.shift) * cspin(0, 1, x0) * cspin(0, 2, y0 - 90*degree));
      
      // Here is what we get for current.T == Id: (computed with sympy)      
      hyperpoint now;
      ld ts = tanh(s) * tanh(s);
      ld cy = cos(y1) * cos(y1);
      ld sq = sqrt(1 - cy * ts);
      now[0] =  sin(y1) * cos(x0) / sq;
      now[1] = -sin(x0) * sin(y1) / sq;
      now[2] =  cos(y1) / cosh(s) / sq;
      now[3] = 0;
      
      // And here is the derivative over t (t = the local variable from ds_cross0)
      hyperpoint der;
      sq = -sqrt(2)*tanh(s)/(4*sqrt(-cos(2*y1)*ts - ts + 2));
      der[0] = sq*(sin(x0 - 2*y1) - sin(x0 + 2*y1));
      der[1] = sq*(cos(x0 - 2*y1) - cos(x0 + 2*y1));
      sq = sqrt(-cy*ts + 1);
      der[2] =  (cy - 1)*sinh(s)/sq;
      der[3] = -(cy*sinh(s)*tanh(s) - cosh(s))/sq;      
      
      cr.h = now;
      
      // now and der give us our wordline shifted to roughly the time of interest, so we can now use ds_cross0 safely
      transmatrix U = Id;
      set_column(U, 2, now);
      set_column(U, 3, der);
      cr = ds_cross0(current.T * U);
      }

    curvepoint(cr.h);
    cr.h[2]++; cr.h /= cr.h[2];
    pts[pts_id++] = cr.h;

    if(pts_id == 3) {
      ld area = (pts[0] ^ pts[1])[2] + (pts[1] ^ pts[2])[2] + (pts[2] ^ pts[0])[2];
      if(area < 0) { et.tinf.tvertices.resize(et.tinf.tvertices.size()-3); curvedata.resize(curvedata.size()-3); }
      pts_id = 0;
      }
    };

  for(int x=0; x<XSCALE; x++)
  for(int y=0; y<YSCALE; y++) {
    add(x, y);
    add(x+1, y);
    add(x, y+1);
    add(x, y+1);
    add(x+1, y);
    add(x+1, y+1);
    }
  
  color_t col = 0xFFFFFFFF;
  part(col, 0) = talpha;
  part(col, 0) *= smoothstep(current.shift - tu.from);
  part(col, 0) *= smoothstep(tu.to - current.shift);
  
  
  auto& poly = queuecurve(shiftless(Id), 0, col, PPR::FLOOR);
  poly.tinf = &et.tinf;
  poly.flags |= POLY_TRIANGLES;
  poly.offset_texture = 0;
  
  copyright_shown = et.copyright;
  }

void draw_textures() {
  pick_textures();
  for(auto& tu: textures_to_use)
    if(current.shift > tu.from && current.shift < tu.to)
      draw_texture(tu);
  }

}}
