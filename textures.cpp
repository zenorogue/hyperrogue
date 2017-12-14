#include <SDL/SDL_image.h>

namespace texture {

GLuint textureid;

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

bool texture_read = false;

int twidth = 2048;

vector<int> expanded_data;

string texturename;

eTextureState tstate;

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
  
void readtexture() {
  texture_read = true;
  
  glGenTextures(1, &textureid );

  SDL_Surface *txt = IMG_Load(texturename.c_str());
  auto txt2 = convertSurface(txt);
  
  int tx = txt->w, ty = txt->h;
  
  SDL_FreeSurface(txt);

  vector<int> half_expanded(twidth * ty);
  expanded_data.resize(twidth * twidth);
  
  int origdim = max(tx, ty);
  int base_x = tx/2 - origdim/2;
  int base_y = ty/2 - origdim/2;
  ZZ = 0;

/*  for(int y=0; y<twidth; y++)
  for(int x=0; x<twidth; x++)
    expanded_data[y*twidth+x] = qpixel(txt2, y%ty, x%tx); */

  for(int y=0; y<ty; y++)
    scale_colorarray(origdim, 
      [&] (int x) { return qpixel(txt2, base_x + x, y); }, 
      [&] (int x, int v) { half_expanded[twidth * y + x] = v; }
      );

  SDL_FreeSurface(txt2);

  for(int x=0; x<twidth; x++)
    scale_colorarray(origdim, 
      [&] (int y) { return base_y+y < 0 || base_y+y >= ty ? 0 : half_expanded[x + (base_y + y) * twidth]; }, 
      [&] (int y, int v) { expanded_data[twidth * y + x] = v; }
      );
  
  glBindTexture( GL_TEXTURE_2D, textureid);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, twidth, twidth, 0,
    GL_BGRA, GL_UNSIGNED_BYTE, 
    &expanded_data[0] );
 
  }

transmatrix itt = Id;

int grid_alpha = 0;
int mesh_alpha = 0;
int color_alpha = 0;

unsigned int glc = 0xFFFFFFDD;

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
  
  int sym = si.symmetries;

  for(int i=0; i<c->type; i++) {
    int is = i % sym;
    hyperpoint h1 =  spin(M_PI + M_PI * (2*i +1) / c->type) * xpush(z) * C0;
    hyperpoint h2 =  spin(M_PI + M_PI * (2*i -1) / c->type) * xpush(z) * C0;
    hyperpoint hm1 = spin(M_PI + M_PI * (2*is+1) / c->type) * xpush(z) * C0;
    hyperpoint hm2 = spin(M_PI + M_PI * (2*is-1) / c->type) * xpush(z) * C0;
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
    lastptd().u.poly.outline = models.count(c) ? 0xFFFFFF08 : 0xFF000008;
    queuepolyat(V, shFullFloor[ctof(c)], 0, PPR_LINE);
    lastptd().u.poly.outline = models.count(c) ? 0xFFFFFF10 : 0xFF000010;
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
      queuetable(V * qfi.spin, &mi.vertices[0], n, mesh_alpha, recolor(c->land == laCocytus ? 0x080808FF : 0x101010FF), PPR_LAKEBOTTOM);
      }
    else {
      queuetable(V * qfi.spin, &mi.vertices[0], n, mesh_alpha, recolor(col), PPR_FLOOR);
      }
        
    lastptd().u.poly.tinf = &mi;
    if(grid_alpha) {
      queuepolyat(V, shFullFloor[ctof(c)], 0, PPR_FLOOR);
      lastptd().u.poly.outline = grid_alpha;
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
  if(!texture_read) readtexture();
  texture_map.clear();
  for(auto p: gmatrix) {
    cell *c = p.first;
    auto si = getpatterninfo0(c);
    bool replace = false;
    
    // int sgn = sphere ? -1 : 1;

    if(!texture_map.count(si.id)) 
      replace = true;
    else if(hdist0(p.second*C0) < hdist0(texture_map[si.id].M * C0))
      replace = true;

    if(replace) {
      auto& mi = texture_map[si.id];
      mapTexture(c, mi, si, p.second);
      mi.texture_id = textureid;
      }    
    }
  models.clear();
  for(auto& t: texture_map) models.insert(t.second.c);
  }

int forgeArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-forge")) {
    shift(); texturename = args();
    }

  else if(argis("-fsp")) {
    shift(); gsplits = argf();
    }

  else return 1;
  return 0;
  }

bool newmove = false;

auto texture_hook = 
  addHook(hooks_args, 100, forgeArgs);

void drawRawTexture() {
  if(!texture_read) readtexture();
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

enum eTexturePanstate {tpsModel, tpsMove, tpsScale, tpsAffine, tpsZoom, tpsProjection};
eTexturePanstate panstate;

void showMenu() {
  cmode = sm::SIDE | sm::MAYDARK | sm::DIALOG_STRICT_X;
  gamescreen(0);  
  
  if(tstate == tsAdjusting) 
    drawRawTexture();
  
  dialog::init(XLAT("texture mode"));

  dialog::addSelItem(XLAT("select the texture's pattern"), XLAT("..."), 'r');
  dialog::addSelItem(XLAT("texture file"), texturename, 'f');
  dialog::addSelItem(XLAT("texture mode enabled"), its(tstate), 't');

  if(tstate == tsAdjusting) {
    dialog::addBoolItem(XLAT("move the model"), panstate == tpsModel, 'm');
    dialog::addBoolItem(XLAT("move the texture"), panstate == tpsMove, 'a');
    dialog::addBoolItem(XLAT("zoom/scale the texture"), panstate == tpsScale, 'x');
    dialog::addBoolItem(XLAT("zoom/scale the model"), panstate == tpsZoom, 'z');
    dialog::addBoolItem(XLAT("projection"), panstate == tpsProjection, 'p');
    dialog::addBoolItem(XLAT("affine transformations"), panstate == tpsAffine, 'y');
    }
  
  if(tstate == tsActive) {
    /* dialog::addSelItem(XLAT("texture scale"), fts(iscale), 's');
    dialog::addSelItem(XLAT("texture angle"), fts(irotate), 'a');
    dialog::addSelItem(XLAT("texture position X"), fts(ix), 'x');
    dialog::addSelItem(XLAT("texture position Y"), fts(iy), 'y'); */
    dialog::addSelItem(XLAT("grid alpha"), its(grid_alpha), 'g');
    dialog::addSelItem(XLAT("mesh alpha"), its(mesh_alpha), 'm');
    dialog::addSelItem(XLAT("precision"), its(gsplits), 'p');
    dialog::addSelItem(XLAT("color alpha"), its(color_alpha), 'c');
    }
  
  dialog::addItem(XLAT("help"), SDLK_F1);  
  dialog::addItem(XLAT("back"), '0');  
  
  getcstat = '-';

  dialog::display();
  
  if(holdmouse) {
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
          View = View * inverse(spintox(mouseeu)) * spintox(lastmouse);
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
      
      default: break;
      }
    }
  
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

    else if(uni == 'r')
      pushScreen(patterns::showPattern);
    else if(uni == 'f') 
      dialog::openFileDialog(texturename, XLAT("texture to load:"), ".png");
    else if(uni == 't') {
      if(tstate == tsOff) {
        tstate = tsAdjusting;
        perform_mapping();
        }
      else if(tstate == tsAdjusting) {
        tstate = tsActive;
        perform_mapping();
        }
      else {
        tstate = tsOff;
        texture_map.clear();
        }
      }
    else if(uni == 'g') {
      dialog::editNumber(grid_alpha, 0, 255, 15, 0, XLAT("grid alpha"), 
        XLAT("Grid alpha."));
      }    
    else if(uni == 'm') {
      dialog::editNumber(mesh_alpha, 0, 255, 15, 0, XLAT("mesh alpha"),
        XLAT("Mesh alpha."));
      }    
    else if(uni == 'c') {
      dialog::editNumber(color_alpha, 0, 255, 15, 0, XLAT("color alpha"),
        XLAT("The higher the value, the less important the color of underlying terrain is."));
      }    
    else if(uni == 'p') {
      dialog::editNumber(gsplits, 0, 4, 1, 1, XLAT("precision"),
        XLAT("precision"));
      dialog::reaction = perform_mapping;
      }    
    else if(doexiton(sym, uni))
      popScreen();
    };
  }

}
// - dual grid
// todo save/load texture configuration
// todo texture editor
