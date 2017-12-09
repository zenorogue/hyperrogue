#include <SDL/SDL_image.h>

glfont_t textures;

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

vector<int> expanded_data;

string texturename;

void sdltogl_bmp(SDL_Surface *txt, glfont_t& f, int ch) {

  int dim = min(txt->w, txt->h);
  int otwidth = dim;
  int otheight = dim;
  
  int twidth = next_p2( otwidth );
  int theight = next_p2( otheight );

  expanded_data.resize(twidth * theight);
 
  for(int j=0; j <theight;j++) for(int i=0; i < twidth; i++) {
    expanded_data[(i+j*twidth)] = qpixel(txt, i, j);
    }
  
  f.widths[ch] = otwidth;
  f.heights[ch] = otheight;

  glBindTexture( GL_TEXTURE_2D, f.textures[ch]);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, twidth, theight, 0,
    GL_BGRA, GL_UNSIGNED_BYTE, 
    &expanded_data[0] );
 
  float x=(float)otwidth / (float)twidth;
  float y=(float)otheight / (float)theight;
  f.tx[ch] = x;
  f.ty[ch] = y;
  }
  
void readtexture() {
  texture_read = true;
  
  glfont_t& f = textures;

  int NUMPICS = 2;
  
  f.textures = new GLuint[NUMPICS];
  glGenTextures( NUMPICS, f.textures );
  if(0) for(int i=0; i<NUMPICS; i++)
    printf("pic %d = %d\n", i, f.textures[i]);

  for(int ch=1; ch<NUMPICS; ch++) {  
    SDL_Surface *txt = IMG_Load(texturename.c_str());
    auto txt2 = convertSurface(txt);

    sdltogl_bmp(txt2, f, ch);
    SDL_FreeSurface(txt);
    SDL_FreeSurface(txt2);
    }

//printf("init size=%d ok\n", size);
//  GLERR("initfont");
  }

ld iscale = 1;
ld irotate = 0;

unsigned int glc = 0xFFFFFFDD;

void mapTextureTriangle(textureinfo &mi, array<hyperpoint, 3> v) {

  int tabid = 1;

  glfont_t& f(textures);
  float fx=f.tx[tabid];
  float fy=f.ty[tabid];

  for(int i=0; i<3; i++) {
    for(int j=0; j<3; j++) 
      mi.vertices.push_back(v[i][j]);
    hyperpoint inmodel;
    applymodel(mi.M * v[i], inmodel);
    inmodel = spin(M_PI * irotate / 180) * inmodel;
    mi.tvertices.push_back(fx * (iscale * inmodel[0]+1)/2);
    mi.tvertices.push_back(fy * (iscale * inmodel[1]+1)/2);
    mi.tvertices.push_back(0);
    }
  }

map<int, textureinfo> texture_map;

bool applyTextureMap(cell *c, const transmatrix &V, int col) {
  using namespace mapeditor;
  int t = subpattern(c, whichPattern);
  try {
    auto& mi = texture_map.at(t);
    qfi.spin = Id;
    
    int d = patterndir(c, whichPattern);
      qfi.spin = qfi.spin * spin(-M_PI * 2 * d / c->type);
    if(reflectPatternAt(c)) 
      qfi.spin = qfi.spin * Mirror;

    int n = mi.vertices.size() / 3;
    
    qfi.special = false;
    qfi.shape = &shFullFloor[ctof(c)];
    qfi.tinf = &mi;

    if(chasmg == 2) return false;
    else if(chasmg && wmspatial) {
      if(detaillevel == 0) return false;
      queuetable(V * qfi.spin, &mi.vertices[0], n, 0, c->land == laCocytus ? 0x080808FF : 0x101010FF, PPR_LAKEBOTTOM);
      }
    else {
      queuetable(V * qfi.spin, &mi.vertices[0], n, 0, col, PPR_FLOOR);
      }
        
    lastptd().u.poly.tinf = &mi;
    
    return true;
    }
  catch(out_of_range) {
    return false;
    }
  }
  
void perform_mapping() {
  using namespace mapeditor;
  if(!texture_read) readtexture();
  texture_map.clear();
  glfont_t& f(textures); int tabid = 1;
  for(auto p: gmatrix) {
    cell *c = p.first;
    int t = subpattern(c, whichPattern);
    bool replace = false;

    if(!texture_map.count(t)) 
      replace = true;
    else if(p.second[2][2] < texture_map[t].M[2][2])
      replace = true;

    if(replace) {
      auto& mi = texture_map[t];
      mi.M = p.second;
      mi.vertices.clear();
      mi.tvertices.clear();

      int d = patterndir(c, whichPattern);
      if(d) 
        mi.M = mi.M * spin(-M_PI * 2 * d / c->type);
      if(reflectPatternAt(c, whichPattern)) 
        mi.M = mi.M * Mirror;

      ld z = ctof(c) ? rhexf : hexvdist;
      ld base = ctof(c) ? 0 : 0; // -hexshift;
      if(!ctof(c) || nontruncated) base -= M_PI / c->type;

      for(int i=0; i<c->type; i++) {
        hyperpoint h1 = spin(base + M_PI * (2*i) / c->type) * xpush(z) * C0;
        hyperpoint h2 = spin(base + M_PI * (2*i+2) / c->type) * xpush(z) * C0;
        mapTextureTriangle(mi, {C0, h1, h2});
        }
      
      mi.texture_id = f.textures[tabid];
      }
    }
  }

bool forge_handleKey(int sym, int uni) {  

  if(sym == SDLK_F4) {
    glc += 0x11;
    return true;
    }

  if(sym == SDLK_F5) {
    glc -= 0x11;
    return true;
    }

  return false;
  }

int forgeArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-forge")) {
    shift(); texturename = args();
    }

  else return 1;
  return 0;
  }

auto texture_hook = 
  addHook(hooks_args, 100, forgeArgs)
+ addHook(hooks_handleKey, 100, forge_handleKey);

bool texture_on = false;

void showTextureMenu() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);  
  
  dialog::init(XLAT("texture mode"));

  dialog::addSelItem(XLAT("select the texture's pattern"), XLAT("..."), 'r');
  dialog::addSelItem(XLAT("texture file"), texturename, 'f');
  dialog::addBoolItem(XLAT("texture mode enabled"), texture_on, 'm');
  
  if(texture_on) {
    dialog::addSelItem(XLAT("texture scale"), fts(iscale), 's');
    dialog::addSelItem(XLAT("texture rotation"), fts(irotate), 'p');
    }
  
  dialog::addItem(XLAT("help"), SDLK_F1);  
  dialog::addItem(XLAT("back"), '0');  
  
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    if(uni == 'r')
      pushScreen(mapeditor::showPattern);
    else if(uni == 'f') {
      mapeditor::cfileptr = &texturename;
      mapeditor::filecaption = XLAT("texture to load:");
      mapeditor::cfileext = ".png";
      pushScreen(mapeditor::drawFileDialog);
      }
    else if(uni == 'm') {
      texture_on = !texture_on;
      if(texture_on) perform_mapping();
      else texture_map.clear();
      }
    else if(uni == 's') {
      dialog::editNumber(iscale, 0, 2, .01, 1, XLAT("texture scale"), 
        XLAT("Texture scale."));
      dialog::reaction = perform_mapping;
      }    
    else if(uni == 'p') {
      dialog::editNumber(irotate, -360, 360, 15, 0, XLAT("texture rotation"), 
        XLAT("Texture rotation."));
      dialog::reaction = perform_mapping;
      }    
    else if(doexiton(sym, uni))
      popScreen();
    };
  }

