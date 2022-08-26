// Hyperbolic Rogue -- texture mode
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file textures.cpp
 *  \brief texture mode, also the basic definitions of textures, used in 3D mode
 */

#include "hyper.h"
#if CAP_TEXTURE
namespace hr {

#if HDR
struct texture_triangle {
  array<hyperpoint, 3> v;
  array<shiftpoint, 3> tv;
  texture_triangle(array<hyperpoint, 3> _v, array<shiftpoint, 3> _tv) : v(_v), tv(_tv) {}
  };

struct textureinfo : basic_textureinfo {
  shiftmatrix M;
  vector<texture_triangle> triangles;
  vector<glvertex> vertices;
  cell *c;
  vector<shiftmatrix> matrices;
  
  // these are required to adjust to geometry changes
  int current_type, symmetries;
  };
#endif

#if HDR
string find_file(string s);
#endif

EX namespace texture {

#if HDR
enum eTextureState {
  tsOff, tsAdjusting, tsActive
  };

struct texture_data {
  GLuint textureid;

  int twidth, theight;
  bool stretched, original;
  int tx, ty, origdim;
  
  int strx, stry, base_x, base_y;
  
  texture_data() { textureid = 0; twidth = 2048; theight = 0; stretched = false; original = false; }

  vector<color_t> texture_pixels;

  color_t& get_texture_pixel(int x, int y) {
    return texture_pixels[(y&(theight-1))*twidth+(x&(twidth-1))];
    }
  
  vector<pair<color_t*, color_t>> undos;
  vector<tuple<cell*, shiftpoint, int> > pixels_to_draw;

  bool loadTextureGL();
  bool whitetexture();
  bool readtexture(string tn);
  void saveRawTexture(string tn);

  void undo();
  void undoLock();
  void update();
  };

struct texture_config {
  string texturename;
  string configname;
  eTextureState tstate;
  eTextureState tstate_max;

  transmatrix itt;
  
  color_t grid_color;
  color_t mesh_color;
  color_t master_color;
  color_t slave_color;
  
  int color_alpha;
  
  int gsplits;

  int recolor(color_t col);

  typedef tuple<eGeometry, eVariation, char, int, eModel, ld, ld> texture_parameters; 
  texture_parameters orig_texture_parameters;
  
  map<int, textureinfo> texture_map, texture_map_orig;
  set<cell*> models;
  
  basic_textureinfo tinf3;

  bool texture_tuned;
  string texture_tuner;
  vector<shiftpoint*> tuned_vertices;

  bool apply(cell *c, const shiftmatrix &V, color_t col);
  void mark_triangles();

  void clear_texture_map();
  void perform_mapping();
  void mapTextureTriangle(textureinfo &mi, const array<hyperpoint, 3>& v, const array<shiftpoint, 3>& tv, int splits);
  void mapTextureTriangle(textureinfo &mi, const array<hyperpoint, 3>& v, const array<shiftpoint, 3>& tv) { mapTextureTriangle(mi, v, tv, gsplits); }
  void mapTexture2(textureinfo& mi);
  void finish_mapping();
  void true_remap();
  void remap();
  bool correctly_mapped;
  hyperpoint texture_coordinates(shiftpoint);

  void drawRawTexture();
  void saveFullTexture(string tn);

  bool save();
  bool load();
  
  texture_data data;

  texture_config() {
    // argh, no member initialization in some of my compilers
    texturename = find_file("textures/hyperrogue-texture.png");
    configname = find_file("textures/hyperrogue.txc");
    itt = Id; 
    grid_color = 0;
    mesh_color = 0;
    master_color = 0xFFFFFF30;
    slave_color = 0xFF000008;
    color_alpha = 128;
    gsplits = 1;
    texture_tuned = false;
    }
  
  };
#endif

EX cpatterntype cgroup;

#if CAP_PNG
SDL_Surface *convertSurface(SDL_Surface* s) {
  SDL_PixelFormat fmt;
  // fmt.format = SDL_PIXELFORMAT_BGRA8888;
  fmt.BitsPerPixel = 32;
  fmt.BytesPerPixel = 4;

  fmt.Ashift=24;
  fmt.Rshift=16; 
  fmt.Gshift=8; 
  fmt.Bshift=0; 
  fmt.Amask=0xff<<24;
  fmt.Rmask=0xff<<16;  
  fmt.Gmask=0xff<<8;  
  fmt.Bmask=0xff;  
  fmt.Aloss = fmt.Rloss = fmt.Gloss = fmt.Bloss = 0;
  fmt.palette = NULL;

#if !CAP_SDL2
  fmt.alpha = 0;
  fmt.colorkey = 0x1ffffff;
#endif
  
  return SDL_ConvertSurface(s, &fmt, SDL_SWSURFACE);
  }
#endif

struct undo {
  unsigned* pix;
  unsigned last;
  };

EX texture_config config;

EX bool saving = false;

template<class T, class U> void scale_colorarray(int origdim, int targetdim, const T& src, const U& dest) {
  int ox = 0, tx = 0, partials[4];
  int omissing = targetdim, tmissing = origdim;
  for(int p=0; p<4; p++) partials[p] = 0;
  
  while(tx < targetdim) {
    int fv = min(omissing, tmissing);
    color_t c = src(ox);
    for(int p=0; p<4; p++) 
      partials[p] += part(c, p) * fv;
    omissing -= fv; tmissing -= fv;
    if(omissing == 0) { 
      ox++; omissing = targetdim; 
      }
    if(tmissing == 0) {
      color_t target;
      for(int p=0; p<4; p++) {
        part(target, p) = partials[p] / origdim;
        partials[p] = 0;
        }
      dest(tx++, target);
      tmissing = origdim;
      }
    }
  }
  
bool texture_data::loadTextureGL() {

  if(textureid == 0) glGenTextures(1, &textureid);

  glBindTexture( GL_TEXTURE_2D, textureid);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  
  // BGRA may be not supported in the web version
  if(ISWEB) for(auto& p: texture_pixels) swap(part(p, 0), part(p, 2));
  
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, twidth, theight, 0,
    ISWEB ? GL_RGBA : GL_BGRA, GL_UNSIGNED_BYTE, 
    &texture_pixels[0] );

  if(ISWEB) for(auto& p: texture_pixels) swap(part(p, 0), part(p, 2));
 
  return true;
  }

bool texture_data::whitetexture() {
  undos.clear();
  texture_pixels.resize(0);
  if(theight == 0) theight = twidth;
  texture_pixels.resize(twidth * theight, 0xFFFFFFFF);
  pixels_to_draw.clear();
  return true;
  }

bool texture_data::readtexture(string tn) {

#if CAP_SDL_IMG || CAP_PNG
  undos.clear();
  
#if CAP_SDL_IMG  
  SDL_Surface *txt = IMG_Load(tn.c_str());
  if(!txt) {
    addMessage(XLAT("Failed to load %1", texturename));
    return false;
    }
  auto txt2 = convertSurface(txt);
  SDL_FreeSurface(txt);

  tx = txt2->w, ty = txt2->h;
  
  auto pix = [&] (int x, int y) { return qpixel(txt2, x, y); };

#elif CAP_PNG
  
  FILE *f = fopen(tn.c_str(), "rb");
  if(!f) { printf("failed to open file\n"); return false; }
  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png) { printf("failed to create_read_struct\n"); return false; }
  if(setjmp(png_jmpbuf(png))) { 
    printf("failed to read\n");
    return false;  
    }
  png_init_io(png, f);

  // set the expected format
  png_infop info = png_create_info_struct(png);
  png_read_info(png, info);
  tx = png_get_image_width(png, info);
  ty = png_get_image_height(png, info);    
  png_byte color_type = png_get_color_type(png, info);
  png_byte bit_depth = png_get_bit_depth(png, info);
  if(bit_depth == 16) png_set_strip_16(png);  
  if(color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
  if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png);
  if(png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);  
  if(color_type == PNG_COLOR_TYPE_RGB ||
     color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

  if(color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);  
  png_read_update_info(png, info);

  // read png
  vector<png_bytep> row_pointers(ty);

  vector<color_t> origpixels(ty * tx);

  for(int y = 0; y < ty; y++) 
    row_pointers[y] = (png_bytep) & origpixels[y * tx];

  png_read_image(png, &row_pointers[0]);
  fclose(f);

  for(int i=0; i<ty*tx; i++)
    swap(part(origpixels[i], 0), part(origpixels[i], 2));
    
  auto pix = [&] (int x, int y) { 
    if(x<0 || y<0 || x >= tx || y >= ty) return (color_t) 0;
    return origpixels[y*tx + x];
    };
  
  printf("texture read OK\n");

#endif

  if(twidth == 0) 
    twidth = next_p2(tx);
  if(theight == 0) theight = (stretched || original) ? next_p2(ty) : twidth;

  texture_pixels.resize(twidth * theight);

  if(stretched) {
    int i = 0;
    println(hlog, tx, " -> " , twidth, " / " , ty, " -> ", theight);
    for(int y=0; y<theight; y++)
    for(int x=0; x<twidth; x++)
      texture_pixels[i++] = pix(x * tx / twidth, y * ty / theight);
    strx = twidth; stry = theight; base_x = base_y = 0;
    }
  
  else if(tx == twidth && ty == theight) {
    int i = 0;
    for(int y=0; y<ty; y++)
    for(int x=0; x<tx; x++)
      texture_pixels[i++] = pix(x, y);
    strx = twidth; stry = theight; base_x = base_y = 0;
    }
  
  else if(original) {
    base_x = 0;
    base_y = 0;
    strx = tx; stry = ty;
    for(int y=0; y<ty; y++)
    for(int x=0; x<tx; x++)
      get_texture_pixel(x, y) = pix(x,y);
    }
   
  else {
  
    origdim = max(tx, ty);
    base_x = origdim/2 - tx/2;
    base_y = origdim/2 - ty/2;
    
    strx = twidth * tx / origdim;
    stry = theight * ty / origdim;

    qpixel_pixel_outside = 0; // outside is black
  
    vector<int> half_expanded(twidth * ty);  
    for(int y=0; y<ty; y++)
      scale_colorarray(origdim, twidth,
        [&] (int x) { return pix(x - base_x,y); },
        [&] (int x, int v) { half_expanded[twidth * y + x] = v; }
        );

    for(int x=0; x<twidth; x++)
      scale_colorarray(origdim, twidth,
        [&] (int y) { return y-base_y < 0 || y-base_y >= ty ? 0 : half_expanded[x + (y-base_y) * twidth]; }, 
        [&] (int y, int v) { get_texture_pixel(x, y) = v; }
        );
    
    base_x = base_x * twidth / origdim;
    base_y = base_y * theight / origdim;
    }

#if CAP_SDL_IMG
  SDL_FreeSurface(txt2);
#endif
  
  return true;
#else
  return false;
#endif
  }

void texture_data::saveRawTexture(string tn) {
  SDL_Surface *sraw = SDL_CreateRGBSurface(SDL_SWSURFACE,twidth,twidth,32,0,0,0,0);
  for(int y=0; y<twidth; y++)
  for(int x=0; x<twidth; x++)
    qpixel(sraw,x,y) = get_texture_pixel(x, y);
  IMAGESAVE(sraw, tn.c_str());
  SDL_FreeSurface(sraw);
  addMessage(XLAT("Saved the raw texture to %1", tn));
  }

hyperpoint texture_config::texture_coordinates(shiftpoint h) {
  hyperpoint inmodel;
  applymodel(h, inmodel);
  inmodel[0] *= current_display->radius * 1. / current_display->scrsize;
  inmodel[1] *= current_display->radius * pconf.stretch / current_display->scrsize;
  inmodel[2] = 1;
  inmodel = itt * inmodel;
  inmodel[0] = (inmodel[0] + 1) / 2;
  inmodel[1] = (inmodel[1] + 1) / 2;
  return inmodel;
  }

void texture_config::mapTextureTriangle(textureinfo &mi, const array<hyperpoint, 3>& v, const array<shiftpoint, 3>& tv, int splits) {

  if(splits) {
    array<hyperpoint, 3> v2 = make_array( mid(v[1], v[2]), mid(v[2], v[0]), mid(v[0], v[1]) );
    array<shiftpoint, 3> tv2 = make_array( mid(tv[1], tv[2]), mid(tv[2], tv[0]), mid(tv[0], tv[1]) );
    mapTextureTriangle(mi, make_array(v[0], v2[2], v2[1]),  make_array(tv[0], tv2[2], tv2[1]),  splits-1);
    mapTextureTriangle(mi, make_array(v[1], v2[0], v2[2]),  make_array(tv[1], tv2[0], tv2[2]),  splits-1);
    mapTextureTriangle(mi, make_array(v[2], v2[1], v2[0]),  make_array(tv[2], tv2[1], tv2[0]),  splits-1);
    mapTextureTriangle(mi, make_array(v2[0], v2[1], v2[2]), make_array(tv2[0], tv2[1], tv2[2]), splits-1);
    return;
    }
    
  for(int i=0; i<3; i++) {
    mi.vertices.push_back(glhr::pointtogl(v[i]));
    hyperpoint inmodel;
    mi.tvertices.push_back(glhr::pointtogl(texture_coordinates(tv[i])));
    }
  
  // when reading a spherical band in a cylindrical projection,
  // take care that texture vertices are mapped not around the sphere
  if(sphere && mdBandAny()) {
    int s = isize(mi.tvertices)-3;
    ld xmin = min(mi.tvertices[s][0], min(mi.tvertices[s+1][0], mi.tvertices[s+2][0]));
    ld xmax = max(mi.tvertices[s][0], max(mi.tvertices[s+1][0], mi.tvertices[s+2][0]));
    if(xmax - xmin > .5) {
      for(int ss=s; ss<s+3; ss++) 
        if(mi.tvertices[ss][0] < .5) mi.tvertices[ss][0] += 1;
      }
    }
  }

texture_triangle *edited_triangle;
textureinfo *edited_tinfo;

int celltriangles(cell *c) {
  return c->type;
  }

array<shiftpoint, 3> findTextureTriangle(cell *c, patterns::patterninfo& si, int i) {
  shiftmatrix M = ggmatrix(c) * applyPatterndir(c, si);
  return make_array(M * C0, M * get_corner_position(c, i), M * get_corner_position(c, i+1));
  }

// using: mouseh, mouseouver
int getTriangleID(cell *c, patterns::patterninfo& si, shiftpoint h) {
  // auto si = getpatterninfo0(c);
  ld quality = 1e10;
  int best = 0;
  for(int i=0; i<celltriangles(c); i++) {
    auto t = findTextureTriangle(c, si, i);
    ld q = hdist(t[1], h) + hdist(t[2], h);
    if(q < quality) quality = q, best = i;
    }
  return best;
  }

void mapTexture(cell *c, textureinfo& mi, patterns::patterninfo &si, const shiftmatrix& T, int shift = 0) {
  mi.c = c;
  mi.symmetries = si.symmetries;
  mi.current_type = celltriangles(c);
  
  mi.M = T * applyPatterndir(c, si);
  mi.triangles.clear();
  
  transmatrix iv = inverse(applyPatterndir(c, si));

  int sd = si.dir;
  if((NONSTDVAR) || bt::in()) sd = 0;
  
  for(int i=0; i<c->type; i++) {
    hyperpoint h1 = iv * get_corner_position(c, (i + sd + shift) % c->type);
    hyperpoint h2 = iv * get_corner_position(c, (i + sd + shift + 1) % c->type);
    mi.triangles.emplace_back(make_array(C0, h1, h2), make_array(mi.M*C0, mi.M*h1, mi.M*h2));
    }
  }

void texture_config::mapTexture2(textureinfo& mi) {
  mi.vertices.clear();
  mi.tvertices.clear();
  for(auto& t: mi.triangles)
    mapTextureTriangle(mi, t.v, t.tv);
  }
  
int texture_config::recolor(color_t col) {
  if(color_alpha == 0) return col;
  for(int i=1; i<4; i++)
    part(col, i) = color_alpha + ((255-color_alpha) * part(col,i) + 127) / 255;
  return col;
  }

EX bool texture_aura;

EX bool using_aura() {
  return texture_aura && config.tstate == texture::tsActive;
  }

bool texture_config::apply(cell *c, const shiftmatrix &V, color_t col) {
  if(config.tstate == tsOff || !correctly_mapped) return false;

  using namespace patterns;
  auto si = getpatterninfo0(c);

  if(config.tstate == tsAdjusting) {
    dynamicval<color_t> d(poly_outline, slave_color);
    draw_floorshape(c, V, cgi.shFullFloor, 0, PPR::LINE);
    
    curvepoint(C0);
    for(int i=0; i<c->type; i++) 
      curvepoint(get_corner_position(c, i)), curvepoint(C0);
    queuecurve(V, slave_color, 0, PPR::LINE);

    return false;
    }
  try {
    auto& mi = texture_map.at(si.id);
    
    set_floor(cgi.shFullFloor);
    qfi.tinf = &mi;
    qfi.spin = applyPatterndir(c, si);

    if(grid_color) {
      dynamicval<color_t> d(poly_outline, grid_color);
      draw_floorshape(c, V, cgi.shFullFloor, 0, PPR::FLOOR);
      }
      
    if(using_aura()) {
      for(int i=0; i<isize(mi.tvertices); i += 3) {
        ld p[3];
        if(inHighQual)
        while(true) {
          p[0] = hrandf();
          p[1] = hrandf();
          p[2] = 1 - p[0] - p[1];
          if(p[2] >= 0) break;
          }
        else p[0] = p[1] = p[2] = 1/3.;
        ld v[2] = {0,0};
        for(int j=0; j<2; j++) for(int k=0; k<3; k++)
          v[j] += mi.tvertices[i+k][j] * p[k];
  
        int vi[2] = {int(v[0] * config.data.twidth), int(v[1] * config.data.twidth)};
  
        col = config.data.get_texture_pixel(vi[0], vi[1]);
        hyperpoint h = glhr::gltopoint(mi.vertices[i]);
        addaura(V*h, col, 0);
        }
      }

    return true;
    }
  catch(out_of_range&) {
    // printf("Ignoring tile #%d / %08x: not mapped\n", si.id, patterns::subcode(c, si));
    return false;
    }
  }

void texture_config::mark_triangles() {
  if(config.tstate == tsAdjusting) 
    for(auto& mi: texture_map) {
      for(auto& t: mi.second.triangles) {
        vector<hyperpoint> t2;
        for(int i=0; i<3; i++)
          t2.push_back(unshift(t.tv[i]));
        prettypoly(t2, master_color, master_color, gsplits);
        }
      }
  }

static const auto current_texture_parameters = tie(geometry, variation, patterns::whichPattern, patterns::subpattern_flags, pmodel, pconf.scale, pconf.alpha);

void texture_config::clear_texture_map() {
  texture_map.clear();
  edited_triangle = nullptr;
  edited_tinfo = nullptr;
  tuned_vertices.clear();
  models.clear();
  texture_tuned = false;
  texture_tuner = "";
  }
  
void texture_config::perform_mapping() {
  if(gsplits < 0) gsplits = 0;
  if(gsplits > 4) gsplits = 4;
  using namespace patterns;
  
  clear_texture_map();

  for(auto& p: gmatrix) {
    cell *c = p.first;
    auto si = getpatterninfo0(c);
    bool replace = false;
    
    // int sgn = sphere ? -1 : 1;
    
    if(!texture_map.count(si.id)) 
      replace = true;
    else if(hdist0(p.second*sphereflip * C0) < hdist0(texture_map[si.id].M * sphereflip * C0))
      replace = true;

    if(replace) {
      auto& mi = texture_map[si.id];
      mapTexture(c, mi, si, p.second);
      mi.texture_id = config.data.textureid;
      }
    }
  
  for(auto& t: texture_map) models.insert(t.second.c);
  
  for(auto& p: gmatrix) {
    cell *c = p.first;
    bool nearmodel = models.count(c);
    forCellEx(c2, c) 
      if(models.count(c2)) 
        nearmodel = true;
    if(nearmodel) {
      auto si = getpatterninfo0(c);
      texture_map[si.id].matrices.push_back(p.second * applyPatterndir(c, si));
      }
    }
    
  }

set<int> missing_cells_known;

void texture_config::finish_mapping() {
  tinf3.tvertices.clear();
  tinf3.texture_id = config.data.textureid;
  if(isize(texture_map) && isize(texture_map.begin()->second.triangles)) {
    auto& tris = texture_map.begin()->second.triangles;

    for(int a=0; a<8; a++) {
      auto& tri = tris[a % isize(tris)];
      shiftpoint center = tri.tv[0];
      hyperpoint v1 = unshift(tri.tv[1], center.shift) - center.h;
      hyperpoint v2 = unshift(tri.tv[2], center.shift) - center.h;
      texture_order([&] (ld x, ld y) {
        shiftpoint h = shiftless(normalize(center.h + v1 * x + v2 * y), center.shift);
        tinf3.tvertices.push_back(glhr::pointtogl(texture_coordinates(h)));
        });
      }
    }
  
  if(config.tstate == tsActive) {
    for(auto& mi: texture_map)
      mapTexture2(mi.second);
    correctly_mapped = true;
    missing_cells_known.clear();
    }

  patterns::computeCgroup();
  texture::cgroup = patterns::cgroup;
  texture_map_orig = texture_map;
  orig_texture_parameters = current_texture_parameters;
  // printf("texture_map has %d elements (S%d)\n", isize(texture_map), config.tstate);
  }

#if CAP_SHOT
void texture_config::saveFullTexture(string tn) {
  addMessage(XLAT("Saving full texture to %1...", tn));
  dynamicval<color_t> dd(grid_color, 0);
  dynamicval<color_t> dm(mesh_color, 0);
  dynamicval<ld> dx(pconf.xposition, 0);
  dynamicval<ld> dy(pconf.yposition, 0);
  dynamicval<ld> dvs(pconf.scale, (pmodel == mdDisk && !euclid) ? 1 : pconf.scale);
  dynamicval<bool> dro(rug::rugged, false);
  dynamicval<bool> dnh(nohud, true);
  texture::saving = true;
  drawscreen();

  dynamicval<int> dvx(shot::shotx, data.twidth);
  dynamicval<int> dvy(shot::shoty, data.twidth);
  dynamicval<int> dvf(shot::shotformat, -1);
  shot::take(tn.c_str());
  texture::saving = false;
  
  drawscreen();
  if(data.readtexture(tn) && data.loadTextureGL()) {
    itt = Id; // xyscale(Id, current_display->scrsize * 1. / current_display->radius);
    perform_mapping();
    finish_mapping();
    }
  }
#endif

bool newmove = false;

vector<glhr::textured_vertex> rtver(4);

void texture_config::drawRawTexture() {
  glflush();
  current_display->next_shader_flags = GF_TEXTURE;
  dynamicval<eModel> m(pmodel, mdPixel);
  current_display->set_all(0, 0);
  glhr::color2(0xFFFFFF20);
  glBindTexture(GL_TEXTURE_2D, config.data.textureid);
  for(int i=0; i<4; i++) {
    int cx[4] = {2, -2, -2, 2};
    int cy[4] = {2, 2, -2, -2};
    int x = cx[i];
    int y = cy[i];
    hyperpoint inmodel = hpxyz(x, y, 1);
    inmodel = itt * inmodel;
    rtver[i].texture[0] = (inmodel[0]+1)/2;
    rtver[i].texture[1] = (inmodel[1]+1)/2;
    rtver[i].coords[0] = x * current_display->scrsize;
    rtver[i].coords[1] = y * current_display->scrsize;
    rtver[i].coords[2] = 0;
    rtver[i].coords[3] = 1;
    }
  glhr::id_modelview();
  glhr::prepare(rtver);
  glhr::set_depthtest(false);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  }

struct magicmapper_point {
  cell *c;
  hyperpoint cell_relative;
  hyperpoint texture_coords;
  };

vector<magicmapper_point> amp;

enum eMagicParameter {
  mpScale,
  mpProjection,
  mpMove,
  mpRotate,
  mpSlant,
  mpStretch,
  mpTexPosX,
  mpTexPosY,
  mpMAX
  };

EX vector<string> mpnames = {
  "affect model scale",
  "affect model projection",
  "affect model central point",
  "affect model rotation",
  "affect texture slanting",
  "affect texture stretching",
  "affect texture position X",
  "affect texture position Y"
  };

flagtype current_magic = 15;

bool have_mp(eMagicParameter i) { return (current_magic >> i) & 1; }

struct magic_param {
  bool do_spin;
  int texmode;
  ld spinangle, scale, proj, moveangle, shift, slant, stretch, tx, ty;

  void shuffle() {
    do_spin = hrand(2);
    spinangle = hrandf() - hrandf();
    moveangle = hrandf() * 2 * M_PI;
    shift = hrandf() - hrandf();
    scale = hrandf() - hrandf();
    proj = hrandf() - hrandf();
    texmode = hrand(3);
    slant = have_mp(mpSlant) ? hrandf() - hrandf() : 0;
    stretch = have_mp(mpStretch) ? hrandf() - hrandf() : 0;
    tx = have_mp(mpTexPosX) ? hrandf() - hrandf() : 0;
    ty = have_mp(mpTexPosY) ? hrandf() - hrandf() : 0;
    }
  
  void affect_itt(const transmatrix& T) {
    transmatrix Ti = inverse(T);
    for(auto& p: amp)
      p.texture_coords = Ti * p.texture_coords;
    config.itt = config.itt * T;
    }
  
  void apply(ld delta) {
    if(have_mp(mpProjection))
      pconf.alpha *= exp(delta * proj);
    if(have_mp(mpScale))
      pconf.scale *= exp(delta * scale);

    if(do_spin) {
      if(have_mp(mpRotate))
        View = spin(delta * spinangle) * View;
      }
    else {
      if(have_mp(mpMove))
        View = spin(moveangle) * xpush(delta*shift) * spin(-moveangle) * View;
      }
    
    if(texmode == 0 && have_mp(mpStretch)) 
      affect_itt(euaffine(hpxyz(0, delta * stretch, 0)));

    if(texmode == 1 && have_mp(mpSlant))
      affect_itt(euaffine(hpxyz(delta * slant, 0, 0)));
    
    if(texmode == 2 && (have_mp(mpTexPosX) || have_mp(mpTexPosY)))
      affect_itt(eupush(delta * tx, delta * ty));
    
    fixmatrix(View);
    }
  };

ld magic_quality() {
  gmatrix.clear();
  calcparam();

  ld q = 0;
  for(auto& p: amp) {
    hyperpoint inmodel;
    applymodel(ggmatrix(p.c) * p.cell_relative, inmodel);
    inmodel[0] *= current_display->radius * 1. / current_display->scrsize;
    inmodel[1] *= current_display->radius * 1. / current_display->scrsize;
    q += sqhypot_d(2, inmodel - p.texture_coords);
    }
  return q;
  }

void applyMagic() {
  ld cq = magic_quality();
  
  int last_success = 0;
  
  for(int s=0; s<50000 &&  s<last_success + 1000; s++) {
    magic_param p;
    p.shuffle();
    
    bool failed = false;
    
    for(ld delta = 1; abs(delta) > 1e-9; delta *= (failed ? -.7 : 1.2)) {
      p.apply(delta);
      ld nq = magic_quality();
      if(nq < cq) {
        cq = nq; 
        last_success = s;
        }
      else {
        p.apply(-delta);
        failed = true;
        }
      }
    }
  config.perform_mapping();
  }

enum eTexturePanstate {tpsModel, tpsMove, tpsScale, tpsAffine, tpsZoom, tpsProjection, tpsCell, tpsTriangle, tpsTune};
eTexturePanstate panstate;

void mousemovement() {
  static hyperpoint lastmouse;
  
  hyperpoint mouseeu = hpxyz((mousex - current_display->xcenter + .0) / current_display->scrsize, (mousey - current_display->ycenter + .0) / current_display->scrsize, 1);
  bool nonzero = mouseeu[0] || mouseeu[1];

  switch(panstate) {
    case tpsModel:
      if(!newmove && mouseh[2] < 50 && lastmouse[2] < 50) {
        panning(shiftless(lastmouse), mouseh);
        config.perform_mapping();
        }
      lastmouse = unshift(mouseh); newmove = false;
      break;
    
    case tpsMove: {
      if(!newmove) 
        config.itt = config.itt * inverse(eupush(mouseeu)) * eupush(lastmouse);
      lastmouse = mouseeu; newmove = false;
      break;
      }
    
    case tpsScale: {
      if(nonzero && !newmove) 
        config.itt = config.itt * inverse(euscalezoom(mouseeu)) * euscalezoom(lastmouse);
      if(nonzero) lastmouse = mouseeu;
      newmove = false;
      break;
      }
    
    case tpsAffine: {
      if(!newmove) 
        config.itt = config.itt * inverse(euaffine(mouseeu)) * euaffine(lastmouse);
      lastmouse = mouseeu; newmove = false;
      break;
      }

    case tpsZoom: {
      // do not zoom in portrait!
      if(nonzero && !newmove) {
        View = inverse(spintox(mouseeu)) * spintox(lastmouse) * View;
        pconf.scale = pconf.scale * sqhypot_d(2, mouseeu) / sqhypot_d(2, lastmouse);
        config.perform_mapping();
        }
      if(nonzero) lastmouse = mouseeu;
      newmove = false;
      break;
      }
    
    case tpsProjection: {
      if(nonzero && !newmove) {          
        pconf.alpha = pconf.alpha * sqhypot_d(2, mouseeu) / sqhypot_d(2, lastmouse);
        }
      if(nonzero) lastmouse = mouseeu;
      newmove = false;
      break;
      }
    
    case tpsCell: {
      cell *c = mouseover;
      if(!c) break;
      auto si = patterns::getpatterninfo0(c);
      if(newmove) {
        edited_tinfo = NULL;
        if(config.texture_map.count(si.id)) {
          edited_tinfo = &config.texture_map[si.id];
          newmove = false;
          }
        }
      if(edited_tinfo && isize(edited_tinfo->triangles) == celltriangles(c)) {
        for(int i=0; i<celltriangles(c); i++)
          edited_tinfo->triangles[i].tv = findTextureTriangle(c, si, i);
        config.texture_tuned = true;
        }
      break;
      }
    
    case tpsTriangle: {
      cell *c = mouseover;
      if(!c) break;
      auto si = patterns::getpatterninfo0(c);
      int i = getTriangleID(c, si, mouseh);
      if(newmove) {
        edited_triangle = NULL;
        if(config.texture_map.count(si.id)) {
          edited_triangle = &config.texture_map[si.id].triangles[i];
          newmove = false;
          }
        }
      if(edited_triangle) {
        edited_triangle->tv = findTextureTriangle(c, si, i);
        config.texture_tuned = true;
        }
      break;
      }

    case tpsTune: {
      ld tdist = 1e20;
      if(newmove) {
        config.tuned_vertices.clear();
        for(auto& a: config.texture_map)
          for(auto& t: a.second.triangles)
            for(auto& v: t.tv)
              if(hdist(v, mouseh) < tdist)
                tdist = hdist(v, mouseh);
        for(auto& a: config.texture_map)
          for(auto& t: a.second.triangles)
            for(auto& v: t.tv)
              if(hdist(v, mouseh) < tdist * (1.000001))
                config.tuned_vertices.push_back(&v);
        newmove = false;
        }
      for(auto v: config.tuned_vertices) {
        *v = mouseh;
        config.texture_tuned = true;
        }
      break;
      }
    
    default: break;
    }
  }

patterns::patterninfo si_save;  

saverlist texturesavers;

eVariation targetvariation;
eGeometry targetgeometry;

string csymbol;

string tes;

void init_textureconfig() {
#if CAP_CONFIG
  texturesavers = std::move(savers);  
  for(int i=0; i<3; i++)
  for(int j=0; j<3; j++)
    addsaver(config.itt[i][j], "texturematrix_" + its(i) + its(j), i==j ? 1 : 0);

  for(int i=0; i<3; i++)
  for(int j=0; j<3; j++)
    addsaver(View[i][j], "viewmatrix_" + its(i) + its(j), i==j ? 1 : 0);

  addsaverenum(targetgeometry, "geometry", gNormal);
  addsaver(tes, "tes", "");
  addsaverenum(pmodel, "used model", mdDisk);
  addsaver(vid.yshift, "Y shift", 0);
  addsaver(pconf.yposition, "Y position", 0);
  addsaver(pconf.xposition, "X position", 0);
  addsaver(pconf.camera_angle, "camera angle", 0);
  addsaverenum(targetvariation, "bitruncated", eVariation::bitruncated);
  // ... geometry parameters

  addsaverenum(patterns::whichPattern, "pattern", patterns::PAT_TYPES);
  addsaver(patterns::subpattern_flags, "pattern flags", 0);

  addsaver(si_save.id, "center type", 1);
  addsaver(si_save.dir, "center direction", 0);
  addsaver(si_save.reflect, "center reflection", false);
  addsaver(config.data.twidth, "texture resolution", 2048);
  addsaver(config.gsplits, "precision", 1);
  
  addsaver(config.grid_color, "grid color", 0);
  addsaver(config.color_alpha, "alpha color", 0);
  addsaver(config.mesh_color, "mesh color", 0);
  
  addsaver(pconf.alpha, "projection", 1);
  addsaver(pconf.scale, "scale", 1);
  addsaver(pconf.stretch, "stretch", 1);
  addsaver(vid.binary_width, "binary-tiling-width", 1);
  
  addsaver(config.texturename, "texture filename", "");
  addsaver(config.texture_tuner, "texture tuning", "");
  
  addsaver(csymbol, "symbol", "");
  
  swap(texturesavers, savers);
#endif
  }

bool texture_config::save() {
#if CAP_CONFIG
  init_textureconfig();
  FILE *f = fopen(configname.c_str(), "wt");
  if(!f) return false;
  
  if(texture_tuned) {
    texture_tuner = "";
    for(auto& a: config.texture_map)
      for(auto& t: a.second.triangles)
        for(auto& v: t.tv) 
          for(int i=0; i<3; i++) {
            texture_tuner += fts(v[i]);
            texture_tuner += ';';
            }
    }

  targetgeometry = geometry;
  targetvariation = variation;

  cell *ctr = centerover;
  si_save = patterns::getpatterninfo0(ctr);

  if(arb::in()) tes = arb::current.filename;
  
  csymbol = "";
  #if CAP_ARCM
  if(arcm::in()) csymbol = arcm::current.symbol;
  #endif
  
  for(auto s: texturesavers) if(s->dosave())
    fprintf(f, "%s=%s\n", s->name.c_str(), s->save().c_str());
  
  fclose(f);
#endif
  return true;
  }

bool texture_config::load() {
#if CAP_CONFIG
  init_textureconfig();

  FILE *f = fopen(configname.c_str(), "rt");
  if(!f) return false;
  swap(texturesavers, savers);
  for(auto s: savers) s->reset();
  loadNewConfig(f);
  swap(texturesavers, savers);
  fclose(f);
  polygonal::solve();
  
  if(1) {
    dynamicval<patterns::ePattern> d1(patterns::whichPattern, patterns::whichPattern);
    dynamicval<int> d2(patterns::subpattern_flags, patterns::subpattern_flags);

    if(targetgeometry != geometry) {
      stop_game();
      #if CAP_ARCM
      if(targetgeometry == gArchimedean) {
        arcm::current.symbol = csymbol;
        arcm::current.parse();
        if(arcm::current.errors) {
          printf("Error while reading Archimedean texture: %s\n", arcm::current.errormsg.c_str());
          addMessage("Error: " + arcm::current.errormsg);
          return false;
          }
        }
      #endif
      if(targetgeometry == gArbitrary) {
        arb::run(tes);
        stop_game();
        }
      set_geometry(targetgeometry);
      start_game();
      return config.load();
      }
    
    if(variation != targetvariation) {
      set_variation(targetvariation);
      start_game();
      return config.load();
      }
    }
  

  if(true) {
    celllister cl(currentmap->gamestart(), 20, 10000, NULL);
    bool found = false;
    for(cell *c: cl.lst) if(euclid || ctof(c)) {
      auto si_here = patterns::getpatterninfo0(c);
      if(si_here.id == si_save.id && si_here.reflect == si_save.reflect && si_here.dir == si_save.dir) {
        centerover = c;
        found = true;
        break;
        }
      }
    if(!found)
      addMessage(XLAT("warning: unable to find the center"));
    }
  
  if(!data.readtexture(texturename)) return false;
  if(!data.loadTextureGL()) return false;
  calcparam();
  models::configure();
  drawthemap();
  config.tstate = config.tstate_max = tsActive;
  string s = std::move(texture_tuner);
  perform_mapping();
  
  texture_tuner = std::move(s);
  
  if(texture_tuner != "") {
    texture_tuned = true;
    vector<ld*> coords;
    for(auto& a: config.texture_map)
      for(auto& t: a.second.triangles)
        for(auto& v: t.tv) 
          for(int i=0; i<3; i++) 
            coords.push_back(&v[i]);
    int semicounter = 0;
    for(char c: texture_tuner) if(c == ';') semicounter++;
    if(semicounter != isize(coords))
      addMessage("Tuning error: wrong number");
    else {
      string cur = "";
      int index = 0;
      for(char c: texture_tuner)
        if(c == ';') {
          *(coords[index++]) = atof(cur.c_str());
          cur = "";
          }
        else cur += c;
      printf("index = %d semi = %d sc = %d\n", index, semicounter, isize(coords));
      }
    }
    
  finish_mapping();
#endif
  return true;
  }

void showMagicMenu() {
  cmode = sm::SIDE | sm::MAYDARK | sm::DIALOG_STRICT_X;
  gamescreen();

  dialog::init(XLAT("texture auto-adjustment"));

  dialog::addInfo(XLAT("drag from the model to the texture"));
  
  for(int i=0; i<mpMAX; i++)
    dialog::addBoolItem(XLAT(mpnames[i]), have_mp(eMagicParameter(i)), 'a'+i);
  
  dialog::addSelItem(XLAT("delete markers"), its(isize(amp)), 'D');
  dialog::addItem(XLAT("perform auto-adjustment"), 'R');
  dialog::addBack();

  getcstat = '-';

  dialog::display();

  if(holdmouse) {
    hyperpoint mouseeu = hpxyz((mousex - current_display->xcenter + .0) / current_display->scrsize, (mousey - current_display->ycenter + .0) / current_display->scrsize, 1);
    if(newmove) {
      magicmapper_point newpoint;
      newpoint.c = mouseover;
      newpoint.cell_relative = inverse_shift(gmatrix[mouseover], mouseh);
      amp.push_back(newpoint);
      newmove = false;
      }
    amp.back().texture_coords = mouseeu;
    }
  
  if(config.tstate == tsAdjusting) {
    initquickqueue();
    char letter = 'A';
    for(auto& am: amp) {
      shiftpoint h = ggmatrix(am.c) * am.cell_relative;
      queuestr(h, vid.fsize, s0+letter, 0xC00000, 1);

      /*
      hyperpoint inmodel;
      applymodel(h, inmodel);
      inmodel[0] *= current_display->radius * 1. / current_display->scrsize;
      inmodel[1] *= current_display->radius * 1. / current_display->scrsize;
      */

      queuestr(
        current_display->xcenter + current_display->scrsize * am.texture_coords[0], 
        current_display->ycenter + current_display->scrsize * am.texture_coords[1],
        0, vid.fsize, s0+letter, 0x00C000, 1);
      
      letter++;
      }
    quickqueue();
    }
  
  keyhandler = [] (int sym, int uni) {
    // handlePanning(sym, uni);
    dialog::handleNavigation(sym, uni);
    
    if(uni == '-' && config.tstate == tsAdjusting) {
      if(!holdmouse) {
        holdmouse = true;
        newmove = true;
        }
      }
    else if(uni >= 'a' && uni < 'a' + mpMAX) 
      current_magic ^= 1<<(uni - 'a');

    else if(uni == 'D') amp.clear();
    else if(uni == 'R') applyMagic();
    else if(doexiton(sym, uni))
      popScreen();
    };
  }

string texturehelp = 
  "This mode lets you change the floor tesselation easily -- "
  "select 'paint a new texture' and draw like in a Paint program. "
  "The obtained pattern can then be easily changed to another geometry, "
  "or saved.\n\n"
  "Instead of drawing, it is also possible to use an arbitrary image "
  "as a texture. "
  "Works best with spherical/Euclidean/hyperbolic tesselations "
  "(e.g., a photo of a soccerball, or one of the tesselations by M. C. "
  "Escher), but it can be also used on arbitrary photos to make them periodic "
  "(these probably work best with the 'large picture' setting in geometry selection). "
  "Again, tesselations can have their geometry changed.\n\n";

#if CAP_EDIT
EX void start_editor() {
  if(config.data.whitetexture() && config.data.loadTextureGL()) {
    config.tstate = config.tstate_max = tsActive;
    config.perform_mapping();
    config.finish_mapping();
    mapeditor::initdraw(cwt.at);
    mapeditor::intexture = true;
    mapeditor::drawing_tool = false;
    pushScreen(mapeditor::showDrawEditor);
    }
  }
#endif

EX void showMenu() {
  cmode = sm::SIDE | sm::MAYDARK | sm::DIALOG_STRICT_X;
  gamescreen();
  if(config.tstate == tsAdjusting) {
    ptds.clear();
    config.mark_triangles();
    drawqueue();
    }
  
  if(config.tstate == tsOff) {
    dialog::init(XLAT("texture mode (off)"));
    dialog::addItem(XLAT("select geometry/pattern"), 'r');
    dialog::add_action(patterns::pushChangeablePatterns);
    if(config.tstate_max == tsAdjusting || config.tstate_max == tsActive) {
      dialog::addItem(XLAT("reactivate the texture"), 't');
      dialog::add_action([] {
        config.tstate = config.tstate_max;
        });    
      }
    if(GDIM == 2 && !rug::rugged) {
      dialog::addItem(XLAT("open PNG as texture"), 'o');
      dialog::add_action([] {
        dialog::openFileDialog(config.texturename, XLAT("open PNG as texture"), ".png", 
          [] () {
            if(config.data.readtexture(config.texturename) && config.data.loadTextureGL()) {
              if(config.tstate_max == tsOff) config.tstate_max = tsAdjusting;
              config.tstate = config.tstate_max;
              config.perform_mapping();
              config.finish_mapping();
              return true;
              }
            else return false;
            });
        });
      }
    dialog::addItem(XLAT("load texture config"), 'l');
    dialog::add_action([] {
      dialog::openFileDialog(config.configname, XLAT("load texture config"), ".txc", 
        [] () {
          return config.load();
          });
      });
    dialog::addSelItem(XLAT("texture size"), its(config.data.twidth), 'w');
    dialog::add_action([] {
      config.data.twidth *= 2;
      if(config.data.twidth > 9000) config.data.twidth = 256;
      config.tstate_max = tsOff;
      });
#if CAP_EDIT
    if(GDIM == 2) {
      dialog::addItem(XLAT("paint a new texture"), 'n');
      dialog::add_action(start_editor);
      }
#endif

    dialog::addBoolItem(XLATN("Canvas"), specialland == laCanvas, 'X');
    dialog::add_action([] () {
      bool inwhite = specialland == laCanvas && patterns::whichCanvas == 'g' && patterns::canvasback == 0xFFFFFF;
      if(inwhite) 
        pushScreen(patterns::showPrePattern);
      else {
        stop_game();
        enable_canvas();
        patterns::whichCanvas = 'g';
        patterns::canvasback = 0xFFFFFF;
        start_game();
        }
      });
    }

  if(config.tstate == tsAdjusting) {
    dialog::init(XLAT("texture mode (overlay)"));
    dialog::addItem(XLAT("select the texture's pattern"), 'r');
    dialog::add_action(patterns::pushChangeablePatterns);
    dialog::addItem(XLAT("enable the texture"), 't');
    dialog::add_action([] {
      config.tstate = config.tstate_max = tsActive;
      config.finish_mapping();
      });
    dialog::addItem(XLAT("cancel the texture"), 'T');
    dialog::add_action([] {
      config.tstate = tsOff;
      config.tstate_max = tsOff;
      });    
    dialog::addBoolItem_choice(XLAT("move the model"), panstate, tpsModel, 'm');
    dialog::addBoolItem_choice(XLAT("move the texture"), panstate, tpsMove, 'a');
    dialog::addBoolItem_choice(XLAT("scale/rotate the texture"), panstate, tpsScale, 'x');
    dialog::addBoolItem_choice(XLAT("scale/rotate the model"), panstate, tpsZoom, 'z');
    dialog::addBoolItem_choice(XLAT("projection"), panstate, tpsProjection, 'p');
    dialog::addBoolItem_choice(XLAT("affine transformations"), panstate, tpsAffine, 'y');
    dialog::addBoolItem(XLAT("magic"), false, 'A');
    dialog::add_action_push(showMagicMenu);
    
    dialog::addBreak(50);

    dialog::addBoolItem_choice(XLAT("select master cells"), panstate, tpsCell, 'C');
    dialog::addBoolItem_choice(XLAT("select master triangles"), panstate, tpsTriangle, 'X');
    dialog::addBoolItem_choice(XLAT("fine tune vertices"), panstate, tpsTune, 'F');

    dialog::addColorItem(XLAT("grid color (master)"), config.master_color, 'M');
    dialog::add_action([] { dialog::openColorDialog(config.master_color, NULL); });
    dialog::addColorItem(XLAT("grid color (copy)"), config.slave_color, 'K');
    dialog::add_action([] { dialog::openColorDialog(config.slave_color, NULL); });    

    dialog::addBreak(50);
    
#if CAP_SHOT
    dialog::addItem(XLAT("save the raw texture"), 'S');
    dialog::add_action([] {
      dialog::openFileDialog(config.texturename, XLAT("save the raw texture"), ".png", 
        [] () {
          config.data.saveRawTexture(config.texturename); return true;
          });
      });
#endif
    }
  
  if(config.tstate == tsActive) {
    dialog::init(XLAT("texture mode (active)"));
    dialog::addItem(XLAT("deactivate the texture"), 't');
    dialog::add_action([] { config.tstate = tsOff; });
    dialog::addItem(XLAT("back to overlay mode"), 'T');
    dialog::add_action([] {config.tstate = tsAdjusting; });
    dialog::addItem(XLAT("change the geometry"), 'r');
    dialog::add_action(patterns::pushChangeablePatterns);
    dialog::addColorItem(XLAT("grid color"), config.grid_color, 'g');
    dialog::add_action([] { dialog::openColorDialog(config.grid_color, NULL); });
    dialog::addColorItem(XLAT("mesh color"), config.mesh_color, 'm');
    dialog::add_action([] { dialog::openColorDialog(config.mesh_color, NULL); });
    dialog::addSelItem(XLAT("color alpha"), its(config.color_alpha), 'c');
    dialog::add_action([] {
      dialog::editNumber(config.color_alpha, 0, 255, 15, 0, XLAT("color alpha"),      
        XLAT("The higher the value, the less important the color of underlying terrain is."));
      });
    dialog::addBoolItem_action(XLAT("aura from texture"), texture_aura, 'a');
#if CAP_EDIT
    if(GDIM == 2) {
      dialog::addItem(XLAT("edit the texture"), 'e');
      dialog::add_action([] {
        mapeditor::intexture = true;
        mapeditor::drawing_tool = false;
        mapeditor::initdraw(cwt.at);
        pushScreen(mapeditor::showDrawEditor);
        });
      }
#endif
#if CAP_SHOT
    dialog::addItem(XLAT("save the full texture image"), 'S');
    dialog::add_action([] {
      dialog::openFileDialog(config.texturename, XLAT("save the full texture image"), ".png", 
        [] () {
          config.saveFullTexture(config.texturename);
          return true;
          });
      });
#endif
    dialog::addItem(XLAT("save texture config"), 's');
    dialog::add_action([] {
      dialog::openFileDialog(config.configname, XLAT("save texture config"), ".txc", 
        [] () {
          return config.save();
          });
      });
    }
  
  dialog::addSelItem(XLAT("precision"), its(config.gsplits), 'P');
  dialog::add_action([] {
    dialog::editNumber(config.gsplits, 0, 4, 1, 1, XLAT("precision"),
      XLAT("precision"));
    if(config.tstate == tsActive) dialog::reaction = [] () { config.finish_mapping();
      };
    });
  dialog::addHelp();
  dialog::addBack();
  
  getcstat = '-';

  dialog::display();
  
  if(holdmouse) mousemovement();
  
  keyhandler = [] (int sym, int uni) {
    // handlePanning(sym, uni);
    dialog::handleNavigation(sym, uni);    
    if(uni == '-' && config.tstate == tsAdjusting) {
      if(!holdmouse) {
        holdmouse = true;
        newmove = true;
        }
      }
    else if(uni == SDLK_F1)
      gotoHelp(texturehelp);
    else if(doexiton(sym, uni))
      popScreen();
    };
  }

typedef pair<int,int> point;

point ptc(shiftpoint h) {
  hyperpoint inmodel = config.texture_coordinates(h);
  return make_pair(int(inmodel[0] * config.data.twidth), int(inmodel[1] * config.data.twidth));
  }

array<point, 3> ptc(const array<shiftpoint, 3>& h) {
  return make_array(ptc(h[0]), ptc(h[1]), ptc(h[2]));
  }

int texture_distance(pair<int, int> p1, pair<int, int> p2) {
  return max(abs(p1.first-p2.first), abs(p1.second - p2.second));
  }

void fillpixel(int x, int y, unsigned col) {
  if(x<0 || y<0 || x >= config.data.twidth || y >= config.data.twidth) return;
  auto& pix = config.data.get_texture_pixel(x, y);
  if(pix != col) {
    config.data.undos.emplace_back(&pix, pix);
    pix = col;
    }
  }

void texture_data::undo() {
  texture::config.data.pixels_to_draw.clear();
  while(!undos.empty()) {
    auto p = undos.back();
    undos.pop_back();
    if(!p.first) {
      loadTextureGL(); 
      return;
      }
    *p.first = p.second;
    }
  }

void texture_data::undoLock() {
  printf("undos size = %d\n", isize(undos));
  if(isize(undos) > 2000000) {
    // limit undo memory
    int moveto = 0;
    for(int i=0; i < isize(undos) - 1000000; i++)
      if(!undos[i].first) moveto = i;
    if(moveto) {
      for(int i=0; i+moveto < isize(undos); i++)
        undos[i] = undos[i+moveto];
      undos.resize(isize(undos) - moveto);
      printf("undos sized to = %d\n", isize(undos));
      }
    }
    
  undos.emplace_back(nullptr, 1);
  }

void filltriangle(const array<shiftpoint, 3>& v, const array<point, 3>& p, color_t col, int lev) {
  
  int d2 = texture_distance(p[0], p[1]), d1 = texture_distance(p[0], p[2]), d0 = texture_distance(p[1], p[2]);
  
  int a, b, c;

  if((d0 <= 1 && d1 <= 1 && d2 <= 1) || lev >= 20) {
    for(int i=0; i<3; i++)
      fillpixel(p[i].first, p[i].second, col);
    return;
    }
  else if(d1 >= d0 && d1 >= d2)
    a = 0, b = 2, c = 1;
  else if(d2 >= d0 && d2 >= d1)
    a = 0, b = 1, c = 2;
  else
    a = 1, b = 2, c = 0;
  
  shiftpoint v3 = mid(v[a], v[b]);
  point p3 = ptc(v3);
  filltriangle(make_array(v[c], v[a], v3), make_array(p[c], p[a], p3), col, lev+1);
  filltriangle(make_array(v[c], v[b], v3), make_array(p[c], p[b], p3), col, lev+1);
  }

void splitseg(const shiftmatrix& A, const array<ld, 2>& angles, const array<shiftpoint, 2>& h, const array<point, 2>& p, color_t col, int lev) {
  ld newangle = (angles[0] + angles[1]) / 2;
  shiftpoint nh = A * xspinpush0(newangle, mapeditor::dtwidth);
  auto np = ptc(nh);
  
  filltriangle(make_array(h[0],h[1],nh), make_array(p[0],p[1],np), col, lev);
  if(lev < 10) {
    if(texture_distance(p[0],np) > 1)
      splitseg(A, make_array(angles[0], newangle), make_array(h[0], nh), make_array(p[0], np), col, lev+1);
    if(texture_distance(np,p[1]) > 1)
      splitseg(A, make_array(newangle, angles[1]), make_array(nh, h[1]), make_array(np, p[1]), col, lev+1);
    }
  }

void fillcircle(shiftpoint h, color_t col) {
  shiftmatrix A = rgpushxto0(h);
  
  ld step = M_PI * 2/3;
  
  array<shiftpoint, 3> mh = make_array(A * xpush0(mapeditor::dtwidth), A * xspinpush0(step, mapeditor::dtwidth), A * xspinpush0(-step, mapeditor::dtwidth));
  auto mp = ptc(mh);

  filltriangle(mh, mp, col, 0);
  
  for(int i=0; i<3; i++) {
    int j = (i+1) % 3;
    if(texture_distance(mp[i], mp[j]) > 1)
      splitseg(A, make_array(step*i, step*(i+1)), make_array(mh[i], mh[j]), make_array(mp[i], mp[j]), col, 1);
    }
  }

EX bool texturesym = false;

void actDrawPixel(cell *c, shiftpoint h, color_t col) {
  try {
    shiftmatrix M = gmatrix.at(c);
    auto si = patterns::getpatterninfo0(c);
    hyperpoint h1 = inverse_shift(M * applyPatterndir(c, si), h);
    auto& tinf = config.texture_map[si.id];
    for(auto& M2: tinf.matrices) for(int i = 0; i<c->type; i += si.symmetries) {
      fillcircle(M2 * spin(2 * M_PI * i / c->type) * h1, col);
      if(texturesym)
        fillcircle(M2 * spin(2 * M_PI * i / c->type) * Mirror * h1, col);
      }
    }
  catch(out_of_range&) {}
  }
  
EX void drawPixel(cell *c, shiftpoint h, color_t col) {
  config.data.pixels_to_draw.emplace_back(c, h, col);
  }

EX cell *where;

EX void drawPixel(shiftpoint h, color_t col) {
  try {
    again:
    shiftmatrix g0 = gmatrix[where];
    ld cdist0 = hdist(tC0(g0), h);

    forCellEx(c, where) 
      try {
        shiftmatrix g = gmatrix[c];
        ld cdist = hdist(tC0(g), h);
        if(cdist < cdist0) {
          cdist0 = cdist;
          where = c; g0 = g;
          goto again;
          }
        }
      catch(out_of_range&) {}
    drawPixel(where, h, col);
    }
  catch(out_of_range&) {}
  }

EX void drawLine(shiftpoint h1, shiftpoint h2, color_t col, int steps IS(10)) {
  if(steps > 0 && hdist(h1, h2) > mapeditor::dtwidth / 3) {
    shiftpoint h3 = mid(h1, h2);
    drawLine(h1, h3, col, steps-1);
    drawLine(h3, h2, col, steps-1);
    }
  else
    drawPixel(h2, col);
  }

void texture_config::true_remap() {
  models::configure();
  drawthemap();
  if(GDIM == 3) return;
  texture_map.clear();
  missing_cells_known.clear();
  for(cell *c: dcal) {
    auto si = patterns::getpatterninfo0(c);
    int oldid = patterns::getpatterninfo(c, patterns::whichPattern, patterns::subpattern_flags | patterns::SPF_NO_SUBCODES).id;
    
    if(texture_map.count(si.id)) continue;
    
    int pshift = 0;
    if(texture::cgroup == cpSingle) oldid = 0;
    if(texture::cgroup == cpFootball && patterns::cgroup == cpThree) {
      if(si.id == 4) pshift = 1;
      oldid = !si.id;
      }

    try {

      auto& mi = texture_map_orig.at(oldid);  
      int ncurr = isize(mi.tvertices);  
      int ntarget = ncurr * celltriangles(c) / mi.current_type;
      vector<glvertex> new_tvertices = mi.tvertices;
      new_tvertices.resize(ntarget);
      for(int i=ncurr; i<ntarget; i++) {
        new_tvertices[i] = new_tvertices[i - ncurr];
        }
      
      auto& mi2 = texture_map[si.id];
      mi2 = mi;
      if(GOLDBERG || IRREGULAR) pshift += si.dir;
      mapTexture(c, mi2, si, ggmatrix(c), pshift);
      mapTexture2(mi2);
      mi2.tvertices = std::move(new_tvertices);
      // printf("%08x remapping %d vertices to %d vertices\n", si.id, isize(mi.tvertices), isize(mi2.tvertices));
      }
    catch(out_of_range&) { 
      if(missing_cells_known.count(si.id) == 0) {
        missing_cells_known.insert(si.id);
        printf("Unexpected missing cell #%d/%d\n", si.id, oldid);
        addMessage(XLAT("Unexpected missing cell #%1/%1", its(si.id), its(oldid)));
        }
      // config.tstate_max = config.tstate = tsAdjusting;
      return;
      }
    }
  }

void texture_config::remap() {
  if(tstate == tsActive) {
    if(geometry == gFake) {
      /* always correct */
      true_remap();
      return;
      }
    patterns::computeCgroup();
    correctly_mapped = patterns::compatible(texture::cgroup, patterns::cgroup);
    if(!correctly_mapped)
      correctly_mapped = 
        current_texture_parameters == config.orig_texture_parameters;
    if(correctly_mapped) true_remap();
    else addMessage(XLAT("Pattern incompatible."));
    }
  else if(tstate == tsAdjusting) {
    printf("perform_mapping %d/%d\n", config.tstate, config.tstate_max);
    calcparam();
    models::configure();
    drawthemap();
    perform_mapping();
    finish_mapping();
    printf("texture_map size = %d\n", isize(texture_map));
    }
  }

#if CAP_TEXTURE
texture_data txp;

EX double get_txp(ld x, ld y, int p) {
  if(txp.texture_pixels.empty()) return 0.5;
  color_t col = txp.get_texture_pixel(txp.twidth * x, txp.twidth * y);
  return part(col, p) / 255.;
  }
#endif

int textureArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-txpic")) {
    shift(); config.texturename = args();
    }

  else if(argis("-txp")) {
    shift(); config.gsplits = argi();
    }

  else if(argis("-txc")) {
    shift(); config.configname = args();
    }

  else if(argis("-txc")) {
    shift(); config.configname = args();
    }

  else if(argis("-txpsize")) {
    shift(); txp.twidth = argi();
    }

  else if(argis("-txpf")) {
    shift(); txp.readtexture(args());
    }

  else if(argis("-txcl")) {
    PHASE(3); drawscreen();
    config.load();
    }

  else return 1;
  return 0;
  }

auto texture_hook = 
  addHook(hooks_args, 100, textureArgs)
+ addHook(hooks_clearmemory, 100, [] () { config.data.pixels_to_draw.clear(); });

int lastupdate;

void texture_data::update() {
  if(!pixels_to_draw.empty()) {
    auto t = SDL_GetTicks();
    while(SDL_GetTicks() < t + 75 && !pixels_to_draw.empty()) {
      auto p = pixels_to_draw.back();
      actDrawPixel(get<0>(p), get<1>(p), get<2>(p));
      pixels_to_draw.pop_back();
      }
    loadTextureGL(); 
    }
  }

EX }
}
#endif
