#include <SDL/SDL_image.h>

namespace texture {

GLuint textureid = 0;

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

vector<int> expanded_data;

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
    &expanded_data[0] );
 
  return true;
  }

bool whitetexture() {
  expanded_data.resize(0);
  expanded_data.resize(twidth * twidth, 0xFFFFFFFF);
  return true;
  }

bool readtexture() {
  
  SDL_Surface *txt = IMG_Load(texturename.c_str());
  if(!txt) {
    addMessage(XLAT("Failed to load %1", texturename));
    return false;
    }
  auto txt2 = convertSurface(txt);
  
  int tx = txt->w, ty = txt->h;
  
  SDL_FreeSurface(txt);

  vector<int> half_expanded(twidth * ty);
  expanded_data.resize(twidth * twidth);
  
  int origdim = max(tx, ty);
  int base_x = tx/2 - origdim/2;
  int base_y = ty/2 - origdim/2;

  ZZ = 0; // outside is black

/*  for(int y=0; y<twidth; y++)
  for(int x=0; x<twidth; x++)
    expanded_data[y*twidth+x] = qpixel(txt2, y%ty, x%tx); */

  for(int x=0; x<twidth; x++)
    scale_colorarray(origdim, 
      [&] (int y) { return base_y+y < 0 || base_y+y >= ty ? 0 : half_expanded[x + (base_y + y) * twidth]; }, 
      [&] (int y, int v) { expanded_data[twidth * y + x] = v; }
      );
  
  for(int y=0; y<ty; y++)
    scale_colorarray(origdim, 
      [&] (int x) { return qpixel(txt2, base_x + x, y); }, 
      [&] (int x, int v) { half_expanded[twidth * y + x] = v; }
      );

  SDL_FreeSurface(txt2);
  
  return true;
  }

transmatrix itt = Id;

unsigned grid_color = 0;
unsigned mesh_color = 0;
unsigned master_color = 0xFFFFFF10;
unsigned slave_color = 0xFF000008;

int color_alpha = 0;

int gsplits = 1;

void mapTextureTriangle(textureinfo &mi, array<hyperpoint, 3> vview, array<hyperpoint, 3> vmap, int splits = gsplits) {

  if(splits) {
    array<hyperpoint, 3> vview2 = { mid(vview[1], vview[2]), mid(vview[2], vview[0]), mid(vview[0], vview[1]) };
    array<hyperpoint, 3> vmap2  = { mid(vmap [1], vmap [2]), mid(vmap [2], vmap [0]), mid(vmap [0], vmap [1]) };
    mapTextureTriangle(mi, {vview[0], vview2[1], vview2[2]}, {vmap[0], vmap2[1], vmap2[2]}, splits-1);
    mapTextureTriangle(mi, {vview[1], vview2[2], vview2[0]}, {vmap[1], vmap2[2], vmap2[0]}, splits-1);
    mapTextureTriangle(mi, {vview[2], vview2[0], vview2[1]}, {vmap[2], vmap2[0], vmap2[1]}, splits-1);
    mapTextureTriangle(mi, {vview2[0], vview2[1], vview2[2]}, {vmap2[0], vmap2[1], vmap2[2]}, splits-1);
    return;
    }
    
  for(int i=0; i<3; i++) {
    for(int j=0; j<3; j++) 
      mi.vertices.push_back(vview[i][j]);
    hyperpoint inmodel;
    applymodel(mi.M * vmap[i], inmodel);
    inmodel = itt * inmodel;
    inmodel[0] *= vid.radius * 1. / vid.scrsize;
    inmodel[1] *= vid.radius * 1. / vid.scrsize;
    mi.tvertices.push_back((inmodel[0]+1)/2);
    mi.tvertices.push_back((inmodel[1]+1)/2);
    mi.tvertices.push_back(0);
    }
  }

map<int, textureinfo> texture_map;
set<cell*> models;

void mapTexture(cell *c, textureinfo& mi, patterns::patterninfo &si, const transmatrix& T) {
  mi.c = c;
  mi.vertices.clear();
  mi.tvertices.clear();
  mi.symmetries = si.symmetries;
  mi.current_type = c->type;
  mi.current_geometry = geometry;
  mi.current_trunc = nontruncated;
  
  mi.M = T * applyPatterndir(c, si);

  if(tstate == tsAdjusting) return;

  ld z = ctof(c) ? rhexf : hexvdist;
  
  // int sym = si.symmetries;

  for(int i=0; i<c->type; i++) {
    hyperpoint h1 =  spin(M_PI + M_PI * (2*i +1) / c->type) * xpush(z) * C0;
    hyperpoint h2 =  spin(M_PI + M_PI * (2*i -1) / c->type) * xpush(z) * C0;
    hyperpoint hm1 = spin(M_PI + M_PI * (2*i +1) / c->type) * xpush(z) * C0;
    hyperpoint hm2 = spin(M_PI + M_PI * (2*i -1) / c->type) * xpush(z) * C0;
    mapTextureTriangle(mi, {C0, h1, h2}, {C0, hm1, hm2});
    }  
  }

int recolor(int col) {
  if(color_alpha == 0) return col;
  if(color_alpha == 255) return col | 0xFFFFFF00;
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

    if(geometry != mi.current_geometry || nontruncated != mi.current_trunc) {
      // we can easily make it more symmetric
      mi.symmetries = gcd(mi.symmetries, si.symmetries);

      printf("Redrawing tile #%d from %d to %d\n", si.id, mi.current_type, c->type);
      int nbase = n * mi.symmetries / mi.current_type;
      int ntarget = nbase * c->type / mi.symmetries;
      printf("n = %d nbase = %d ntarget = %d\n", n, nbase, ntarget);
      vector<GLfloat> new_tvertices = move(mi.tvertices);
      new_tvertices.resize(3*ntarget);
      for(int i=3*nbase; i<3*ntarget; i++) {
        new_tvertices[i] = new_tvertices[i - 3*nbase];
        }
      
      mapTexture(c, mi, si, Id);
      mi.tvertices = move(new_tvertices);
      n = mi.vertices.size() / 3;
      printf("new n = %d\n", n);
      }    
    
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
  
void perform_mapping() {
  if(gsplits < 0) gsplits = 0;
  if(gsplits > 4) gsplits = 4;
  using namespace patterns;
  texture_map.clear();
  for(auto p: gmatrix) {
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
    texture_map[si.id].matrices.push_back(p.second);
    }
  models.clear();
  for(auto& t: texture_map) models.insert(t.second.c);
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
    dialog::addSelItem(XLAT("select the texture's pattern"), XLAT("..."), 'r');
    if(tstate_max == tsAdjusting)
      dialog::addSelItem(XLAT("readjust the texture"), texturename, 't');
    if(tstate_max == tsActive)
      dialog::addSelItem(XLAT("reactivate the texture"), texturename, 't');
    dialog::addSelItem(XLAT("open texture file"), texturename, 'o');
    dialog::addSelItem(XLAT("load texture config"), "...", 'l');
    }

  if(tstate == tsAdjusting) {
    dialog::addSelItem(XLAT("enable the texture"), texturename, 't');
    dialog::addBoolItem(XLAT("move the model"), panstate == tpsModel, 'm');
    dialog::addBoolItem(XLAT("move the texture"), panstate == tpsMove, 'a');
    dialog::addBoolItem(XLAT("zoom/scale the texture"), panstate == tpsScale, 'x');
    dialog::addBoolItem(XLAT("zoom/scale the model"), panstate == tpsZoom, 'z');
    dialog::addBoolItem(XLAT("projection"), panstate == tpsProjection, 'p');
    dialog::addBoolItem(XLAT("affine transformations"), panstate == tpsAffine, 'y');
    dialog::addBoolItem(XLAT("magic"), panstate == tpsMagic, 'A');

    dialog::addBoolItem(XLAT("grid color (master)"), "...", 'M');
    dialog::addBoolItem(XLAT("grid color (copy)"), "...", 'C');
    
    if(panstate == tpsMagic) {
      dialog::addSelItem(XLAT("delete markers"), its(size(amp)), 'D');
      dialog::addSelItem(XLAT("perform auto-adjustment"), "...", 'R');
      }

    dialog::addSelItem(XLAT("precision"), its(gsplits), 'p');
    }
  
  if(tstate == tsActive) {
    /* dialog::addSelItem(XLAT("texture scale"), fts(iscale), 's');
    dialog::addSelItem(XLAT("texture angle"), fts(irotate), 'a');
    dialog::addSelItem(XLAT("texture position X"), fts(ix), 'x');
    dialog::addSelItem(XLAT("texture position Y"), fts(iy), 'y'); */
    dialog::addBoolItem(XLAT("deactivate the texture"), true, 't');
    dialog::addBoolItem(XLAT("readjust the texture"), true, 'r');
    dialog::addSelItem(XLAT("grid alpha"), "...", 'g');
    dialog::addSelItem(XLAT("mesh alpha"), "...", 'm');
    dialog::addSelItem(XLAT("color alpha"), its(color_alpha), 'c');
    dialog::addSelItem(XLAT("save the texture config"), "...", 's');
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

    else if(uni == 'r' && tstate == tsOff)
      pushScreen(patterns::showPattern);

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

    else if(uni == 'n' && tstate == tsOff) {
      addMessage("white");
      if(whitetexture() && loadTextureGL()) {
        tstate = tstate_max = tsActive;
        perform_mapping();
        mapeditor::colortouse = 0xFFFF00FF;
        mapeditor::initdraw(cwt.c);
        pushScreen(mapeditor::showDrawEditor);
        }
      }

    else if(uni == 't' && tstate == tsOff) 
      tstate = tstate_max;
    
    else if((uni == 't' || uni == 'r') && tstate == tsAdjusting) {
      tstate = tstate_max = tsActive;
      perform_mapping();
      }

    else if(uni == 't' && tstate == tsActive) 
      tstate = tsOff;
      
    else if(uni == 'r' && tstate == tsActive) {
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
    else if(uni == 'p' && tstate == tsAdjusting) {
      dialog::editNumber(gsplits, 0, 4, 1, 1, XLAT("precision"),
        XLAT("precision"));
      dialog::reaction = perform_mapping;
      }    
    else if(doexiton(sym, uni))
      popScreen();
    };
  }

int lastupdate;

void update() {
  if(lastupdate && ticks > lastupdate + 100) {
    loadTextureGL(); 
    lastupdate = 0;
    }
  }

pair<int,int> ptc(hyperpoint h) {
  hyperpoint inmodel;
  applymodel(h, inmodel);
  inmodel = itt * inmodel;
  inmodel[0] *= vid.radius * 1. / vid.scrsize;
  inmodel[1] *= vid.radius * 1. / vid.scrsize;
  int x = (1 + inmodel[0]) * twidth / 2;
  int y = (1 + inmodel[1]) * twidth / 2;
  return make_pair(x,y);
  }

int near(pair<int, int> p1, pair<int, int> p2) {
  return max(abs(p1.first-p2.first), abs(p1.second - p2.second));
  }

void filltriangle(array<hyperpoint, 3> v, int col, int lev) {
  pair<int,int> p[3] = {ptc(v[0]), ptc(v[1]), ptc(v[2])};
  
  if(0) for(int i=0; i<3; i++) 
    printf("#%d fillt #%d %s -> %d,%d\n", lev, i, display(v[i]), p[i].first, p[i].second);
  int d2 = near(p[0], p[1]), d1 = near(p[0], p[2]), d0 = near(p[1], p[2]);
  
  if((d0 <= 1 && d1 <= 1 && d2 <= 1) || lev >= 5) {
    for(int i=0; i<3; i++)
      expanded_data[((p[i].first) & (twidth-1)) + (p[i].second & (twidth-1)) * twidth] = col;
    return;
    }
  else if(d1 >= d0 && d1 >= d2)
    swap(v[0], v[1]);
  else if(d2 >= d0 && d2 >= d1)
    swap(v[0], v[2]);
  
  hyperpoint v3 = mid(v[1], v[2]);
  filltriangle({v[0], v[1], v3}, col, lev+1);
  filltriangle({v[0], v[2], v3}, col, lev+1);
  }

void fillcircle(hyperpoint h, int col) {
  transmatrix A = rgpushxto0(h);

  ld rad = .02;

  filltriangle({A * xpush(rad) * C0, A * spin(M_PI * 2/3) * C0, A * spin(-M_PI * 2/3) * C0}, col, 0);
  }

void drawPixel(cell *c, hyperpoint h, int col) {
  printf("s = %d\n", size(gmatrix));
  
  try {
    transmatrix M = gmatrix.at(c);
    auto si = patterns::getpatterninfo0(c);
    h = inverse(M * applyPatterndir(c, si)) * h;
    auto& tinf = texture_map[si.id];
    for(auto& M2: tinf.matrices) for(int i = 0; i<c->type; i += si.symmetries) {
      hyperpoint inmodel;
      
      fillcircle(M2 * spin(2 * M_PI * i / c->type) * h, col);
      lastupdate = ticks;
      }
    }
  catch(out_of_range) {}
  }
  
}

// todo texture editor
// todo `three octagons` with two colors

