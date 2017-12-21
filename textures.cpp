#if CAP_SDL_IMG
#include <SDL/SDL_image.h>
#elif CAP_PNG
#include <png.h>
#endif

#if CAP_TEXTURE
namespace texture {

GLuint textureid = 0;

cpatterntype cgroup;

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

#ifndef SDL2
  fmt.alpha = 0;
  fmt.colorkey = 0x1ffffff;
#endif
  
  return SDL_ConvertSurface(s, &fmt, SDL_SWSURFACE);
  }

int twidth = 2048;

unsigned paint_color = 0x000000FF;

vector<unsigned> texture_pixels;

string texturename = "hyperrogue-texture.png";
string configname = "hyperrogue.txc";

eTextureState tstate;
eTextureState tstate_max;

template<class T, class U> void scale_colorarray(int origdim, const T& src, const U& dest) {
  int ox = 0, tx = 0, partials[4];
  int omissing = twidth, tmissing = origdim;
  for(int p=0; p<4; p++) partials[p] = 0;
  
  while(tx < twidth) {
    int fv = min(omissing, tmissing);
    int c = src(ox);
    for(int p=0; p<4; p++) 
      partials[p] += part(c, p) * fv;
    omissing -= fv; tmissing -= fv;
    if(omissing == 0) { 
      ox++; omissing = twidth; 
      }
    if(tmissing == 0) {
      int target;
      for(int p=0; p<4; p++) {
        part(target, p) = partials[p] / origdim;
        partials[p] = 0;
        }
      dest(tx++, target);
      tmissing = origdim;
      }
    }
  }
  
bool loadTextureGL() {

  if(textureid == 0) glGenTextures(1, &textureid );

  glBindTexture( GL_TEXTURE_2D, textureid);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, twidth, twidth, 0,
    GL_BGRA, GL_UNSIGNED_BYTE, 
    &texture_pixels[0] );
 
  return true;
  }

bool whitetexture() {
  texture_pixels.resize(0);
  texture_pixels.resize(twidth * twidth, 0xFFFFFFFF);
  return true;
  }

bool readtexture() {

#if CAP_SDL_IMG  
  SDL_Surface *txt = IMG_Load(texturename.c_str());
  if(!txt) {
    addMessage(XLAT("Failed to load %1", texturename));
    return false;
    }
  auto txt2 = convertSurface(txt);
  SDL_FreeSurface(txt);

  int tx = txt2->w, ty = txt2->h;
  
  auto pix = [&] (int x, int y) { return qpixel(txt2, x, y); };

#elif CAP_PNG
  
  FILE *f = fopen(texturename.c_str(), "r");  
  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png) return false;  
  if(setjmp(png_jmpbuf(png))) return false;  
  png_init_io(png, f);

  // set the expected format
  png_infop info = png_create_info_struct(png);
  png_read_info(png, info);
  int tx = png_get_image_width(png, info);
  int ty = png_get_image_height(png, info);    
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

  vector<int> origpixels(ty * tx);

  for(int y = 0; y < ty; y++) 
    row_pointers[y] = (png_bytep) & origpixels[y * tx];

  png_read_image(png, &row_pointers[0]);
  fclose(f);

  auto pix = [&] (int x, int y) { 
    if(x<0 || y<0 || x >= tx || y >= ty) return 0;
    return origpixels[y*tx + x];
    };

#endif
  
  if(tx == twidth && ty == twidth) {
    int i = 0;
    for(int y=0; y<ty; y++)
    for(int x=0; x<tx; x++)
      texture_pixels[i++] = pix(x, y);
    }
   
  else {
  
    int origdim = max(tx, ty);
    int base_x = tx/2 - origdim/2;
    int base_y = ty/2 - origdim/2;
  
    ZZ = 0; // outside is black
  
  /*  for(int y=0; y<twidth; y++)
    for(int x=0; x<twidth; x++)
      texture_pixels[y*twidth+x] = qpixel(txt2, y%ty, x%tx); */
  
    vector<int> half_expanded(twidth * ty);  
    for(int y=0; y<ty; y++)
      scale_colorarray(origdim,
        [&] (int x) { return pix(base_x + x,y); },
        [&] (int x, int v) { half_expanded[twidth * y + x] = v; }
        );

    texture_pixels.resize(twidth * twidth);
    for(int x=0; x<twidth; x++)
      scale_colorarray(origdim, 
        [&] (int y) { return base_y+y < 0 || base_y+y >= ty ? 0 : half_expanded[x + (base_y + y) * twidth]; }, 
        [&] (int y, int v) { texture_pixels[twidth * y + x] = v; }
        );
    
    }

#if CAP_SDL_IMG
  SDL_FreeSurface(txt2);
#endif
  
  return true;
  }

void saveRawTexture() {
  SDL_Surface *sraw = SDL_CreateRGBSurface(SDL_SWSURFACE,twidth,twidth,32,0,0,0,0);
  for(int y=0; y<twidth; y++)
  for(int x=0; x<twidth; x++)
    qpixel(sraw,x,y) = texture_pixels[y * twidth + x];
  IMAGESAVE(sraw, texturename.c_str());
  SDL_FreeSurface(sraw);
  }

transmatrix itt = Id;

unsigned grid_color = 0;
unsigned mesh_color = 0;
unsigned master_color = 0xFFFFFF10;
unsigned slave_color = 0xFF000008;

int color_alpha = 0;

int gsplits = 1;

void mapTextureTriangle(textureinfo &mi, array<hyperpoint, 3> v, int splits = gsplits) {

  if(splits) {
    array<hyperpoint, 3> v2 = { mid(v[1], v[2]), mid(v[2], v[0]), mid(v[0], v[1]) };
    mapTextureTriangle(mi, {v[0], v2[1], v2[2]}, splits-1);
    mapTextureTriangle(mi, {v[1], v2[2], v2[0]}, splits-1);
    mapTextureTriangle(mi, {v[2], v2[0], v2[1]}, splits-1);
    mapTextureTriangle(mi, {v2[0], v2[1], v2[2]}, splits-1);
    return;
    }
    
  for(int i=0; i<3; i++) {
    for(int j=0; j<3; j++) 
      mi.vertices.push_back(v[i][j]);
    hyperpoint inmodel;
    applymodel(mi.M * v[i], inmodel);
    inmodel = itt * inmodel;
    inmodel[0] *= vid.radius * 1. / vid.scrsize;
    inmodel[1] *= vid.radius * 1. / vid.scrsize;
    mi.tvertices.push_back((inmodel[0]+1)/2);
    mi.tvertices.push_back((inmodel[1]+1)/2);
    mi.tvertices.push_back(0);
    }
  }

map<int, textureinfo> texture_map, texture_map_orig;

set<cell*> models;

void mapTexture(cell *c, textureinfo& mi, patterns::patterninfo &si, const transmatrix& T, int shift = 0) {
  mi.c = c;
  mi.vertices.clear();
  mi.tvertices.clear();
  mi.symmetries = si.symmetries;
  mi.current_type = c->type;
  
  mi.M = T * applyPatterndir(c, si);

  if(tstate == tsAdjusting) return;

  ld z = ctof(c) ? rhexf : hexvdist;
  
  // int sym = si.symmetries;

  for(int i=0; i<c->type; i++) {
    int i2 = i+shift;
    hyperpoint h1 =  spin(M_PI + M_PI * (2*i2 +1) / c->type) * xpush(z) * C0;
    hyperpoint h2 =  spin(M_PI + M_PI * (2*i2 -1) / c->type) * xpush(z) * C0;
    mapTextureTriangle(mi, {C0, h1, h2});
    }  
  }

int recolor(int col) {
  if(color_alpha == 255 || (cmode & sm::DRAW)) return col | 0xFFFFFF00;
  if(color_alpha == 0) return col;
  for(int i=1; i<4; i++)
    part(col, i) = color_alpha + ((255-color_alpha) * part(col,i) + 127) / 255;
  return col;
  }

bool apply(cell *c, const transmatrix &V, int col) {
  if(tstate == tsOff) return false;

  using namespace patterns;
  auto si = getpatterninfo0(c);

  if(tstate == tsAdjusting) {
    queuepolyat(V, shFullCross[ctof(c)], 0, PPR_LINE);
    lastptd().u.poly.outline = models.count(c) ? master_color : slave_color;
    queuepolyat(V, shFullFloor[ctof(c)], 0, PPR_LINE);
    lastptd().u.poly.outline = models.count(c) ? master_color : slave_color;
    return false;
    }
  try {
    auto& mi = texture_map.at(si.id);
    
    qfi.spin = applyPatterndir(c, si);
    
    int n = mi.vertices.size() / 3;

    qfi.special = false;
    qfi.shape = &shFullFloor[ctof(c)];
    qfi.tinf = &mi;

    if(chasmg == 2) return false;
    else if(chasmg && wmspatial) {
      if(detaillevel == 0) return false;
      queuetable(V * qfi.spin, &mi.vertices[0], n, mesh_color, recolor(c->land == laCocytus ? 0x080808FF : 0x101010FF), PPR_LAKEBOTTOM);
      }
    else {
      queuetable(V * qfi.spin, &mi.vertices[0], n, mesh_color, recolor(col), PPR_FLOOR);
      }
        
    lastptd().u.poly.tinf = &mi;
    if(grid_color) {
      queuepolyat(V, shFullFloor[ctof(c)], 0, PPR_FLOOR);
      lastptd().u.poly.outline = grid_color;
      }
    
    return true;
    }
  catch(out_of_range) {
    // printf("Ignoring tile #%d : not mapped\n", si.id);
    return false;
    }
  }
  
typedef tuple<eGeometry, bool, char, int, eModel, ld, ld> texture_parameters; 

static const auto current_texture_parameters = tie(geometry, nontruncated, patterns::whichPattern, patterns::subpattern_flags, pmodel, vid.scale, vid.alpha);

texture_parameters orig_texture_parameters;

void perform_mapping() {
  if(gsplits < 0) gsplits = 0;
  if(gsplits > 4) gsplits = 4;
  using namespace patterns;
  texture_map.clear();

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
      mi.texture_id = textureid;
      }
    }

  models.clear();
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
    
  computeCgroup();
  texture::cgroup = patterns::cgroup;
  texture_map_orig = texture_map;
  orig_texture_parameters = current_texture_parameters;
  }

int forgeArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-txpic")) {
    shift(); texturename = args();
    }

  else if(argis("-fsp")) {
    shift(); gsplits = argf();
    }

  else if(argis("-txc")) {
    shift(); configname = args();
    }

  else return 1;
  return 0;
  }

bool newmove = false;

auto texture_hook = 
  addHook(hooks_args, 100, forgeArgs);

void drawRawTexture() {
  glDisable(GL_LIGHTING);  
  glEnable(GL_TEXTURE_2D);
  glMatrixMode(GL_MODELVIEW);
  glcolor2(0xFFFFFF20);
  glPushMatrix();
  glTranslatef(0, 0, vid.scrdist);
  glBindTexture(GL_TEXTURE_2D, textureid);
  vector<GLfloat> tver, sver;
  for(int i=0; i<4; i++) {
    int cx[4] = {1, -1, -1, 1};
    int cy[4] = {1, 1, -1, -1};
    int x = cx[i];
    int y = cy[i];
    hyperpoint inmodel = hpxyz(x, y, 1);
    inmodel = itt * inmodel;
    tver.push_back((inmodel[0]+1)/2);
    tver.push_back((inmodel[1]+1)/2);
    tver.push_back(0);
    sver.push_back(x * vid.scrsize);
    sver.push_back(y * vid.scrsize);
    sver.push_back(0);            
    }
  activateVertexArray(&sver[0], 4);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(3, GL_FLOAT, 0, &tver[0]);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glPopMatrix(); 
  glDisable(GL_TEXTURE_2D);    
  }

struct magicmapper_point {
  cell *c;
  hyperpoint cell_relative;
  hyperpoint texture_coords;
  };

vector<magicmapper_point> amp;

struct magic_param {
  bool do_spin;
  ld spinangle, scale, proj, moveangle, shift;

  void shuffle() {
    do_spin = hrand(2);
    spinangle = hrandf() - hrandf();
    moveangle = hrandf() * 2 * M_PI;
    shift = hrandf() - hrandf();
    scale = hrandf() - hrandf();
    proj = hrandf() - hrandf();
    }
  
  void apply(ld delta) {
    vid.alpha *= exp(delta * proj);
    vid.scale *= exp(delta * scale);

    if(do_spin)
      View = spin(delta * spinangle) * View;
    else
      View = spin(moveangle) * xpush(delta*shift) * spin(-moveangle) * View;
    
    fixmatrix(View);
    }
  };

ld magic_quality() {
  gmatrix.clear();
  calcparam();

  ld q = 0;
  for(auto& p: amp) {
    hyperpoint inmodel;
    applymodel(shmup::ggmatrix(p.c) * p.cell_relative, inmodel);
    inmodel[0] *= vid.radius * 1. / vid.scrsize;
    inmodel[1] *= vid.radius * 1. / vid.scrsize;
    q += intvalxy(inmodel, p.texture_coords);
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
    
    for(ld delta = 1; delta > 1e-9; delta *= (failed ? -.7 : 1.2)) {
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
  }

enum eTexturePanstate {tpsModel, tpsMove, tpsScale, tpsAffine, tpsZoom, tpsProjection, tpsMagic};
eTexturePanstate panstate;

void mousemovement() {
  static hyperpoint lastmouse;
  
  hyperpoint mouseeu = hpxyz((mousex - vid.xcenter + .0) / vid.scrsize, (mousey - vid.ycenter + .0) / vid.scrsize, 1);
  bool nonzero = mouseeu[0] || mouseeu[1];

  switch(panstate) {
    case tpsModel:
      if(!newmove && mouseh[2] < 50 && lastmouse[2] < 50) {
        panning(lastmouse, mouseh);
        perform_mapping();
        }
      lastmouse = mouseh; newmove = false;
      break;
    
    case tpsMove: {
      if(!newmove) 
        itt = itt * inverse(eupush(mouseeu)) * eupush(lastmouse);
      lastmouse = mouseeu; newmove = false;
      break;
      }
    
    case tpsScale: {
      if(nonzero && !newmove) 
        itt = itt * inverse(euscalezoom(mouseeu)) * euscalezoom(lastmouse);
      if(nonzero) lastmouse = mouseeu;
      newmove = false;
      break;
      }
    
    case tpsAffine: {
      if(!newmove) 
        itt = itt * inverse(euaffine(mouseeu)) * euaffine(lastmouse);
      lastmouse = mouseeu; newmove = false;
      break;
      }

    case tpsZoom: {
      // do not zoom in portrait!
      if(nonzero && !newmove) {
        View = inverse(spintox(mouseeu)) * spintox(lastmouse) * View;
        vid.scale = vid.scale * sqrt(intvalxy(C0, mouseeu)) / sqrt(intvalxy(C0, lastmouse));
        }
      if(nonzero) lastmouse = mouseeu;
      newmove = false;
      break;
      }
    
    case tpsProjection: {
      if(nonzero && !newmove) {          
        vid.alpha = vid.alpha * sqrt(intvalxy(C0, mouseeu)) / sqrt(intvalxy(C0, lastmouse));
        }
      if(nonzero) lastmouse = mouseeu;
      newmove = false;
      }
    
    case tpsMagic: {
      if(!mouseover) return;
      if(newmove) {
        magicmapper_point newpoint;
        newpoint.c = mouseover;
        newpoint.cell_relative = inverse(gmatrix[mouseover]) * mouseh;
        amp.push_back(newpoint);
        newmove = false;
        }
      amp.back().texture_coords = mouseeu;
      }
    
    default: break;
    }
  }

patterns::patterninfo si_save;  

saverlist texturesavers;

bool target_nontrunc;

void init_textureconfig() {
  texturesavers = move(savers);  
  for(int i=0; i<3; i++)
  for(int j=0; j<3; j++)
    addsaver(itt[i][j], "texturematrix_" + its(i) + its(j), i==j ? 1 : 0);

  for(int i=0; i<3; i++)
  for(int j=0; j<3; j++)
    addsaver(View[i][j], "viewmatrix_" + its(i) + its(j), i==j ? 1 : 0);

  addsaverenum(targetgeometry, "geometry", gNormal);
  addsaverenum(target_nontrunc, "chamfering", false);
  // ... geometry parameters

  addsaver(patterns::whichPattern, "pattern", 0);
  addsaver(patterns::subpattern_flags, "pattern flags", 0);

  cell *ctr = euclid ? centerover : viewctr.h->c7;
  si_save = patterns::getpatterninfo0(ctr);
  
  addsaver(si_save.id, "center type", 1);
  addsaver(si_save.dir, "center direction", 0);
  addsaver(si_save.reflect, "center reflection", false);
  addsaver(twidth, "texture resolution", 2048);
  addsaver(gsplits, "precision", 1);
  
  addsaver(grid_color, "grid color", 0);
  addsaver(color_alpha, "alpha color", 0);
  addsaver(mesh_color, "mesh color", 0);
  
  addsaver(vid.alpha, "projection", 1);
  addsaver(vid.scale, "scale", 1);
  
  addsaver(texturename, "texture filename", "");
  
  swap(texturesavers, savers);
  }

bool save_textureconfig() {
  init_textureconfig();
  FILE *f = fopen(configname.c_str(), "wt");
  if(!f) return false;

  targetgeometry = geometry;
  target_nontrunc = nontruncated;

  for(auto s: texturesavers) if(s->dosave())
    fprintf(f, "%s=%s\n", s->name.c_str(), s->save().c_str());
  
  fclose(f);
  return true;
  }

bool load_textureconfig() {
  init_textureconfig();

  FILE *f = fopen(configname.c_str(), "rt");
  if(!f) return false;
  swap(texturesavers, savers);
  for(auto s: savers) s->reset();
  loadNewConfig(f);
  swap(texturesavers, savers);
  fclose(f);
  
  if(1) {
    dynamicval<char> d1(patterns::whichPattern, patterns::whichPattern);
    dynamicval<int> d2(patterns::subpattern_flags, patterns::subpattern_flags);

    if(targetgeometry != geometry) {
      restartGame('g');
      return load_textureconfig();
      }
    
    if(nontruncated != target_nontrunc) {
      restartGame('7');
      }
    }
  
  if(!readtexture()) return false;
  if(!loadTextureGL()) return false;
  calcparam();
  drawthemap();
  perform_mapping();
  tstate = tstate_max = tsActive;
  return true;
  }

void showMenu() {
  cmode = sm::SIDE | sm::MAYDARK | sm::DIALOG_STRICT_X;
  gamescreen(0);  
  
  if(tstate == tsAdjusting) 
    drawRawTexture();
  
  dialog::init(XLAT("texture mode"));

  if(tstate == tsOff) {
    dialog::addItem(XLAT("select geometry/pattern"), 'r');
    if(tstate_max == tsAdjusting)
      dialog::addItem(XLAT("readjust the texture"), 't');
    if(tstate_max == tsActive)
      dialog::addItem(XLAT("reactivate the texture"), 't');
    dialog::addItem(XLAT("open PNG as texture"), 'o');
    dialog::addItem(XLAT("load texture config"), 'l');
    dialog::addSelItem(XLAT("texture size"), its(twidth), 'w');
    dialog::addItem(XLAT("paint a new texture"), 'n');
    dialog::addSelItem(XLAT("precision"), its(gsplits), 'P');
    }

  if(tstate == tsAdjusting) {
    dialog::addItem(XLAT("select the texture's pattern"), 'r');
    dialog::addItem(XLAT("enable the texture"), 't');
    dialog::addItem(XLAT("cancel the texture"), 'T');
    dialog::addBoolItem(XLAT("move the model"), panstate == tpsModel, 'm');
    dialog::addBoolItem(XLAT("move the texture"), panstate == tpsMove, 'a');
    dialog::addBoolItem(XLAT("zoom/scale the texture"), panstate == tpsScale, 'x');
    dialog::addBoolItem(XLAT("zoom/scale the model"), panstate == tpsZoom, 'z');
    dialog::addBoolItem(XLAT("projection"), panstate == tpsProjection, 'p');
    dialog::addBoolItem(XLAT("affine transformations"), panstate == tpsAffine, 'y');
    dialog::addBoolItem(XLAT("magic"), panstate == tpsMagic, 'A');

    dialog::addColorItem(XLAT("grid color (master)"), master_color, 'M');
    dialog::addColorItem(XLAT("grid color (copy)"), slave_color, 'C');
    
    if(panstate == tpsMagic) {
      dialog::addSelItem(XLAT("delete markers"), its(size(amp)), 'D');
      dialog::addItem(XLAT("perform auto-adjustment"), 'R');
      }

    dialog::addSelItem(XLAT("precision"), its(gsplits), 'P');
    dialog::addItem(XLAT("save the raw texture"), 'S');
    }
  
  if(tstate == tsActive) {
    /* dialog::addSelItem(XLAT("texture scale"), fts(iscale), 's');
    dialog::addSelItem(XLAT("texture angle"), fts(irotate), 'a');
    dialog::addSelItem(XLAT("texture position X"), fts(ix), 'x');
    dialog::addSelItem(XLAT("texture position Y"), fts(iy), 'y'); */
    dialog::addItem(XLAT("deactivate the texture"), 't');
    dialog::addItem(XLAT("readjust the texture"), 'T');
    dialog::addItem(XLAT("change the geometry"), 'r');
    dialog::addColorItem(XLAT("grid color"), grid_color, 'g');
    dialog::addColorItem(XLAT("mesh color"), mesh_color, 'm');
    dialog::addSelItem(XLAT("color alpha"), its(color_alpha), 'c');
    dialog::addItem(XLAT("edit the texture"), 'e');
    dialog::addItem(XLAT("save the texture image"), 'S');
    dialog::addItem(XLAT("save the texture config"), 's');
    }
  
  dialog::addItem(XLAT("help"), SDLK_F1);  
  dialog::addItem(XLAT("back"), '0');  
  
  getcstat = '-';

  dialog::display();
  
  if(tstate == tsAdjusting) {
    initquickqueue();
    char letter = 'A';
    for(auto& am: amp) {
      hyperpoint h = shmup::ggmatrix(am.c) * am.cell_relative;
      display(h);
      queuechr(h, vid.fsize, letter, 0xC00000, 1);

      hyperpoint inmodel;
      applymodel(h, inmodel);
      inmodel[0] *= vid.radius * 1. / vid.scrsize;
      inmodel[1] *= vid.radius * 1. / vid.scrsize;
      queuechr(
        vid.xcenter + vid.scrsize * inmodel[0], 
        vid.ycenter + vid.scrsize * inmodel[1],
        0, vid.fsize/2, letter, 0xC0C0C0, 1);

      queuechr(
        vid.xcenter + vid.scrsize * am.texture_coords[0], 
        vid.ycenter + vid.scrsize * am.texture_coords[1],
        0, vid.fsize, letter, 0x00C000, 1);
      
      letter++;
      }
    quickqueue();
    }
  
  if(holdmouse) mousemovement();
  
  keyhandler = [] (int sym, int uni) {
    // handlePanning(sym, uni);
    dialog::handleNavigation(sym, uni);
    
    if(uni == '-' && tstate == tsAdjusting) {
      if(!holdmouse) {
        holdmouse = true;
        newmove = true;
        }
      }

    else if(uni == 'm' && tstate == tsAdjusting) panstate = tpsModel;
    else if(uni == 'a' && tstate == tsAdjusting) panstate = tpsMove;
    else if(uni == 'x' && tstate == tsAdjusting) panstate = tpsScale;
    else if(uni == 'y' && tstate == tsAdjusting) panstate = tpsAffine;
    else if(uni == 'z' && tstate == tsAdjusting) panstate = tpsZoom;
    else if(uni == 'p' && tstate == tsAdjusting) panstate = tpsProjection;
    else if(uni == 'A' && tstate == tsAdjusting) panstate = tpsMagic;
    else if(uni == 'D' && tstate == tsAdjusting) amp.clear();
    else if(uni == 'R' && tstate == tsAdjusting) applyMagic();

    else if(uni == 's' && tstate == tsActive) 
      dialog::openFileDialog(configname, XLAT("texture config to save:"), ".txc", 
        [] () {
          return save_textureconfig();
          });

    else if(uni == 'l' && tstate == tsOff) 
      dialog::openFileDialog(configname, XLAT("texture config to load:"), ".txc", 
        [] () {
          return load_textureconfig();
          });

    else if(uni == 'r')
      patterns::pushChangeablePatterns();

    else if(uni == 'o' && tstate == tsOff) 
      dialog::openFileDialog(texturename, XLAT("texture to load:"), ".png", 
        [] () {
          if(readtexture() && loadTextureGL()) {
            if(tstate_max == tsOff) tstate_max = tsAdjusting;
            tstate = tstate_max;
            return true;
            }
          else return false;
          });

    else if(uni == 'w' && tstate == tsOff) {
      twidth *= 2;
      if(twidth > 9000) twidth = 256;
      tstate_max = tsOff;
      }
    
    else if(uni == 'e' && tstate == tsActive) {
      mapeditor::initdraw(cwt.c);
      pushScreen(mapeditor::showDrawEditor);
      }

    else if(uni == 'n' && tstate == tsOff) {
      addMessage("white");
      if(whitetexture() && loadTextureGL()) {
        tstate = tstate_max = tsActive;
        perform_mapping();
        mapeditor::initdraw(cwt.c);
        pushScreen(mapeditor::showDrawEditor);
        }
      }

    else if(uni == 't' && tstate == tsOff) 
      tstate = tstate_max;
    
    else if(uni == 't' && tstate == tsAdjusting) {
      tstate = tstate_max = tsActive;
      perform_mapping();
      }

    else if(uni == 't' && tstate == tsActive) 
      tstate = tsOff;
      
    else if(uni == 'T' && tstate == tsAdjusting) 
      tstate = tsOff;
      
    else if(uni == 'T' && tstate == tsActive) {
      tstate = tsAdjusting;
      texture_map.clear();
      }
        
    else if(uni == 'g' && tstate == tsActive) 
      dialog::openColorDialog(grid_color, NULL);
    else if(uni == 'm' && tstate == tsActive) 
      dialog::openColorDialog(mesh_color, NULL);

    else if(uni == 'M' && tstate == tsAdjusting) 
      dialog::openColorDialog(master_color, NULL);
    else if(uni == 'C' && tstate == tsActive) 
      dialog::openColorDialog(slave_color, NULL);

    else if(uni == 'c' && tstate == tsActive) {
      dialog::editNumber(color_alpha, 0, 255, 15, 0, XLAT("color alpha"),
        XLAT("The higher the value, the less important the color of underlying terrain is."));
      }    
    else if(uni == 'P' && tstate <= tsAdjusting) {
      dialog::editNumber(gsplits, 0, 4, 1, 1, XLAT("precision"),
        XLAT("precision"));
      dialog::reaction = perform_mapping;
      }    
    else if(uni == 'S' && tstate == tsAdjusting) 
      saveRawTexture();
    else if(doexiton(sym, uni))
      popScreen();
    };
  }

int lastupdate;

void update() {
  if(lastupdate && ticks > lastupdate + 50) {
    loadTextureGL(); 
    lastupdate = 0;
    }
  }

typedef pair<int,int> point;

point ptc(hyperpoint h) {
  hyperpoint inmodel;
  applymodel(h, inmodel);
  inmodel = itt * inmodel;
  inmodel[0] *= vid.radius * 1. / vid.scrsize;
  inmodel[1] *= vid.radius * 1. / vid.scrsize;
  int x = (1 + inmodel[0]) * twidth / 2;
  int y = (1 + inmodel[1]) * twidth / 2;
  return make_pair(x,y);
  }

array<point, 3> ptc(const array<hyperpoint, 3>& h) {
  return {ptc(h[0]), ptc(h[1]), ptc(h[2])};
  }

ld penwidth = .02;

int near(pair<int, int> p1, pair<int, int> p2) {
  return max(abs(p1.first-p2.first), abs(p1.second - p2.second));
  }

void filltriangle(const array<hyperpoint, 3>& v, const array<point, 3>& p, int col, int lev) {
  
  int d2 = near(p[0], p[1]), d1 = near(p[0], p[2]), d0 = near(p[1], p[2]);
  
  int a, b, c;

  if((d0 <= 1 && d1 <= 1 && d2 <= 1) || lev >= 20) {
    for(int i=0; i<3; i++)
      texture_pixels[((p[i].first) & (twidth-1)) + (p[i].second & (twidth-1)) * twidth] = col;
    return;
    }
  else if(d1 >= d0 && d1 >= d2)
    a = 0, b = 2, c = 1;
  else if(d2 >= d0 && d2 >= d1)
    a = 0, b = 1, c = 2;
  else
    a = 1, b = 2, c = 0;
  
  hyperpoint v3 = mid(v[a], v[b]);
  point p3 = ptc(v3);
  filltriangle({v[c], v[a], v3}, {p[c], p[a], p3}, col, lev+1);
  filltriangle({v[c], v[b], v3}, {p[c], p[b], p3}, col, lev+1);
  }

void splitseg(const transmatrix& A, const array<ld, 2>& angles, const array<hyperpoint, 2>& h, const array<point, 2>& p, int col, int lev) {
  ld newangle = (angles[0] + angles[1]) / 2;
  hyperpoint nh = A * spin(newangle) * xpush(penwidth) * C0;
  auto np = ptc(nh);
  
  filltriangle({h[0],h[1],nh}, {p[0],p[1],np}, col, lev);
  if(lev < 10) {
    if(near(p[0],np) > 1)
      splitseg(A, {angles[0], newangle}, {h[0], nh}, {p[0], np}, col, lev+1);
    if(near(np,p[1]) > 1)
      splitseg(A, {newangle, angles[1]}, {nh, h[1]}, {np, p[1]}, col, lev+1);
    }
  }

void fillcircle(hyperpoint h, int col) {
  transmatrix A = rgpushxto0(h);
  
  ld step = M_PI * 2/3;
  
  array<hyperpoint, 3> mh = {A * xpush(penwidth) * C0, A * spin(step) * xpush(penwidth) * C0, A * spin(-step) * xpush(penwidth) * C0};
  auto mp = ptc(mh);

  filltriangle(mh, mp, col, 0);
  
  for(int i=0; i<3; i++) {
    int j = (i+1) % 3;
    if(near(mp[i], mp[j]) > 1)
      splitseg(A, {step*i, step*(i+1)}, {mh[i], mh[j]}, {mp[i], mp[j]}, col, 1);
    }
  }

void drawPixel(cell *c, hyperpoint h, int col) {
  
  try {
    transmatrix M = gmatrix.at(c);
    auto si = patterns::getpatterninfo0(c);
    h = inverse(M * applyPatterndir(c, si)) * h;
    auto& tinf = texture_map[si.id];
    for(auto& M2: tinf.matrices) for(int i = 0; i<c->type; i += si.symmetries) {
      fillcircle(M2 * spin(2 * M_PI * i / c->type) * h, col);
      lastupdate = ticks;
      }
    }
  catch(out_of_range) {}
  }

void remap(eTextureState old_tstate, eTextureState old_tstate_max) {
  if(!patterns::compatible(texture::cgroup, patterns::cgroup)) return;
  texture_map.clear();
  if(tstate_max == tsActive) {
  
    tstate = old_tstate;
    tstate_max = old_tstate_max;
    for(cell *c: dcal) {
      auto si = patterns::getpatterninfo0(c);
      
      if(texture_map.count(si.id)) continue;
      
      int oldid = si.id;
      int pshift = 0;
      if(texture::cgroup == cpSingle) oldid = 1;
      if(texture::cgroup == cpFootball && patterns::cgroup == cpThree) {
        if(si.id == 4) pshift = 1;
        oldid = !si.id;
        }

      try {

        auto& mi = texture_map_orig.at(oldid);  
        int ncurr = size(mi.tvertices);  
        int ntarget = ncurr * c->type / mi.current_type;
        vector<GLfloat> new_tvertices = mi.tvertices;
        new_tvertices.resize(ntarget);
        for(int i=ncurr; i<ntarget; i++) {
          new_tvertices[i] = new_tvertices[i - ncurr];
          }
        
        auto& mi2 = texture_map[si.id];
        mi2 = mi;
        mapTexture(c, mi2, si, Id, pshift);
        mi2.tvertices = move(new_tvertices);
        }
      catch(out_of_range) { 
        printf("Unexpected missing cell #%d/%d", si.id, oldid);
        addMessage(XLAT("Unexpected missing cell #%d/%d", its(si.id), its(oldid)));
        tstate_max = tstate = tsAdjusting;
        return;
        }
      }     
    }
  }
  
}
#endif
