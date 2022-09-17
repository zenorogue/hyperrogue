// Hyperbolic Rogue -- basic graphics
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file basegraph.cpp 
 *  \brief This file implements the basic graphical routines
 */

#include "hyper.h"
namespace hr {

#if HDR
struct radarpoint {
  hyperpoint h;
  char glyph;
  color_t color;
  color_t line;
  };

struct radarline {
  hyperpoint h1, h2;
  color_t line;
  };

/** configuration of the current view */
struct display_data {
  /** The cell which is currently in the center. */
  cell *precise_center;
  /** The current rotation, relative to precise_center. */
  transmatrix view_matrix;
  /** Camera rotation, used in nonisotropic geometries. */
  transmatrix local_perspective;
  /** The view relative to the player character. */
  shiftmatrix player_matrix;
  /** On-screen coordinates for all the visible cells. */
  map<cell*, shiftmatrix> cellmatrices, old_cellmatrices;
  /** Position of the current map view, relative to the screen (0 to 1). */
  ld xmin, ymin, xmax, ymax;
  /** Position of the current map view, in pixels. */
  ld xtop, ytop, xsize, ysize;
  display_data() { xmin = ymin = 0; xmax = ymax = 1; }

  /** Center of the current map view, in pixels. */
  int xcenter, ycenter;
  ld radius;
  int scrsize;  
  bool sidescreen;

  ld tanfov;
  flagtype next_shader_flags;

  vector<radarpoint> radarpoints;
  vector<radarline> radarlines;
  transmatrix radar_transform;

  ld eyewidth();
  bool stereo_active();
  bool in_anaglyph();

  void set_viewport(int ed);
  void set_projection(int ed, ld shift);
  void set_mask(int ed);

  void set_all(int ed, ld shift);
  /** Which copy of the player cell? */
  transmatrix which_copy;
  /** On-screen coordinates for all the visible cells. */
  map<cell*, vector<shiftmatrix>> all_drawn_copies;
  };

#define View (::hr::current_display->view_matrix)
#define cwtV (::hr::current_display->player_matrix)
#define centerover (::hr::current_display->precise_center)
#define gmatrix (::hr::current_display->cellmatrices)
#define gmatrix0 (::hr::current_display->old_cellmatrices)
#define NLP (::hr::current_display->local_perspective)

#endif

EX display_data default_display;
EX display_data *current_display = &default_display;

/** Color of the background. */
EX unsigned backcolor = 0;
EX unsigned bordcolor = 0;
EX unsigned forecolor = 0xFFFFFF;

int utfsize(char c) {
  unsigned char cu = c;
  if(cu < 128) return 1;
  if(cu < 224) return 2;
  if(cu < 0xF0) return 3;
  return 4;
  }

EX int get_sightrange() { return getDistLimit() + sightrange_bonus; }

EX int get_sightrange_ambush() { 
  #if CAP_COMPLEX2
  return max(get_sightrange(), ambush::distance); 
  #else
  return get_sightrange();
  #endif
  }

bool display_data::in_anaglyph() { return vid.stereo_mode == sAnaglyph; }
bool display_data::stereo_active() { return vid.stereo_mode != sOFF; }

ld display_data::eyewidth() { 
  switch(vid.stereo_mode) {
    case sAnaglyph:
      return vid.anaglyph_eyewidth;
    case sLR:
      return vid.lr_eyewidth;
    default:
      return 0;
    }
  }

bool eqs(const char* x, const char* y) {
  return *y? *x==*y?eqs(x+1,y+1):false:true;
  }

EX int getnext(const char* s, int& i) {

  int siz = utfsize(s[i]);
// if(fontdeb) printf("s=%s i=%d siz=%d\n", s, i, siz);
  if(siz == 1) return s[i++];
  for(int k=0; k<NUMEXTRA; k++)
    if(eqs(s+i, natchars[k])) {
      i += siz; return 128+k;
      }

#ifdef REPLACE_LETTERS
  for(int j=0; j<isize(dialog::latin_letters_l); j++)
    if(s[i] == dialog::foreign_letters[2*j] && s[i+1] == dialog::foreign_letters[2*j+1]) {
      i += 2;
      return int(dialog::latin_letters_l[j]);
      }
#endif

  printf("Unknown character in: '%s' at position %d\n", s, i);
  i += siz; return '?';
  }

#if CAP_SDLTTF
const int max_font_size = 288;
TTF_Font* font[max_font_size+1];

void fix_font_size(int& size) {
  if(size < 1) size = 1;
  if(size > max_font_size) size = max_font_size;
  if(size > 72) size &=~ 3;
  if(size > 144) size &=~ 7;
  }
#endif

#if CAP_SDL

#if !CAP_SDL2
#if HDR
typedef SDL_Surface SDL_Renderer;
#define srend s
#endif
#endif

EX SDL_Surface *s;
EX SDL_Surface *s_screen;
#if CAP_SDL2
EX SDL_Renderer *s_renderer, *s_software_renderer;
#if HDR
#define srend s_software_renderer
#endif
EX SDL_Texture *s_texture;
EX SDL_Window *s_window;
EX SDL_GLContext s_context;
EX bool s_have_context;
#endif

EX color_t qpixel_pixel_outside;

EX color_t& qpixel(SDL_Surface *surf, int x, int y) {
  if(x<0 || y<0 || x >= surf->w || y >= surf->h) return qpixel_pixel_outside;
  char *p = (char*) surf->pixels;
  p += y * surf->pitch;
  color_t *pi = (color_t*) (p);
  return pi[x];
  }

EX void present_surface() {
  #if CAP_SDL2
  SDL_UpdateTexture(s_texture, nullptr, s->pixels, s->w * sizeof (Uint32));
  SDL_RenderClear(s_renderer);
  SDL_RenderCopy(s_renderer, s_texture, nullptr, nullptr);
  SDL_RenderPresent(s_renderer);
  #else
  SDL_UpdateRect(s, 0, 0, 0, 0);  
  #endif
  }

EX void present_screen() {
#if CAP_GL
  if(vid.usingGL) {
    #if CAP_SDL2
    SDL_GL_SwapWindow(s_window);
    #else
    SDL_GL_SwapBuffers();
    #endif
    return;
    }
#endif
  present_surface();
  }

#endif

#if CAP_SDLTTF

EX string fontpath = ISWEB ? "sans-serif" : HYPERFONTPATH "DejaVuSans-Bold.ttf";

void loadfont(int siz) {
  fix_font_size(siz);
  if(!font[siz]) {
    font[siz] = TTF_OpenFont(fontpath.c_str(), siz);
    // Destination set by ./configure (in the GitHub repository)
    #ifdef FONTDESTDIR
    if (font[siz] == NULL) {
      font[siz] = TTF_OpenFont(FONTDESTDIR, siz);
      }
    #endif
    if (font[siz] == NULL) {
      printf("error: Font file not found: %s\n", fontpath.c_str());
      exit(1);
      }
    }
  }
#endif

#if !ISFAKEMOBILE && !ISANDROID & !ISIOS
int textwidth(int siz, const string &str) {
  if(isize(str) == 0) return 0;

#if CAP_SDLTTF
  fix_font_size(siz);
  loadfont(siz);
  
  int w, h;
  TTF_SizeUTF8(font[siz], str.c_str(), &w, &h);
  // printf("width = %d [%d]\n", w, isize(str));
  return w;

#elif CAP_GL
  return gl_width(siz, str.c_str());
#else
  return 0;
#endif
  }
#endif

#if ISIOS
int textwidth(int siz, const string &str) {
  return mainfont->getSize(str, siz / 36.0).width;
  }
#endif

#if !CAP_GL
EX void setcameraangle(bool b) { }
#endif

#if !CAP_GL
EX void reset_projection() { }
EX void glflush() { }
EX bool model_needs_depth() { return false; }
void display_data::set_all(int ed, ld lshift) {}
#endif

#if CAP_GL

EX void eyewidth_translate(int ed) {
  glhr::using_eyeshift = false;
  if(ed) glhr::projection_multiply(glhr::translate(-ed * current_display->eyewidth(), 0, 0));
  }

tuple<int, eModel, display_data*, int> last_projection;
EX bool new_projection_needed;
#if HDR
inline void reset_projection() { new_projection_needed = true; }
#endif

EX ld lband_shift;

void display_data::set_all(int ed, ld shift) {
  auto t = this;
  auto current_projection = tie(ed, pmodel, t, current_rbuffer);
  if(new_projection_needed || !glhr::current_glprogram || (next_shader_flags & GF_which) != (glhr::current_glprogram->shader_flags & GF_which) || current_projection != last_projection || shift != lband_shift) {
    last_projection = current_projection;
    lband_shift = shift;
    set_projection(ed, shift);
    set_mask(ed);
    set_viewport(ed);
    new_projection_needed = false;
    }
  }  

void display_data::set_mask(int ed) { 
  if(ed == 0 || vid.stereo_mode != sAnaglyph) {
    glColorMask( GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE );
    }
  else if(ed == 1) {
    glColorMask( GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE );
    }
  else if(ed == -1) {
    glColorMask( GL_FALSE,GL_TRUE,GL_TRUE,GL_TRUE );
    }
  }

void display_data::set_viewport(int ed) {  
  ld xtop = current_display->xtop;
  ld ytop = current_display->ytop;
  ld xsize = current_display->xsize;
  ld ysize = current_display->ysize;
  
  if(ed == 0 || vid.stereo_mode != sLR) ;
  else if(ed == 1) xsize /= 2;
  else if(ed == -1) xsize /= 2, xtop += xsize;
    
  glViewport(xtop, ytop, xsize, ysize);
  }

EX bool model_needs_depth() {
  return GDIM == 3 || pmodel == mdBall;
  }

EX void setGLProjection(color_t col IS(backcolor)) {
  DEBBI(DF_GRAPH, ("setGLProjection"));
  GLERR("pre_setGLProjection");

  glClearColor(part(col, 2) / 255.0, part(col, 1) / 255.0, part(col, 0) / 255.0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  GLERR("setGLProjection #1");

  glEnable(GL_BLEND);
#ifndef GLES_ONLY  
  if(vid.antialias & AA_LINES) {
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }
  else glDisable(GL_LINE_SMOOTH);
#endif

  glLineWidth(vid.linewidth);

  GLERR("setGLProjection #2");

#ifndef GLES_ONLY
  if(vid.antialias & AA_POLY) {
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    }
  else glDisable(GL_POLYGON_SMOOTH);
#endif

  GLERR("setGLProjection #3");

  //glLineWidth(1.0f);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
#ifdef GL_ES
  glClearDepthf(1.0f);
#else
  glClearDepth(1.0f);
#endif
  glDepthFunc(GL_LEQUAL);
  
  GLERR("setGLProjection");
  reset_projection();
  
  glhr::set_depthwrite(true);
  glClear(GL_DEPTH_BUFFER_BIT);
  }

EX  int next_p2 (int a ) {
    int rval=1;
    // rval<<=1 Is A Prettier Way Of Writing rval*=2;
    while(rval<a) rval<<=1;
    return rval;
}

#if CAP_GLFONT

#define CHARS (128+NUMEXTRA)

#if HDR
struct charinfo_t {
  int w, h;
  float tx0, ty0, tx1, ty1;
  };

struct glfont_t {
  GLuint texture;                                     // Holds The Texture Id
//GLuint list_base;                                   // Holds The First Display List ID  
  vector<charinfo_t> chars; 
  };

const int max_glfont_size = 72;
#endif

EX glfont_t *glfont[max_glfont_size+1];

typedef Uint16 texturepixel;

#define FONTTEXTURESIZE 2048

int curx = 0, cury = 0, theight = 0;
texturepixel fontdata[FONTTEXTURESIZE][FONTTEXTURESIZE];

void sdltogl(SDL_Surface *txt, glfont_t& f, int ch) {
#if CAP_TABFONT
  if(ch < 32) return;
  int otwidth, otheight, tpixindex = 0;
  unsigned char tpix[3000];
  loadCompressedChar(otwidth, otheight, tpix);
#else
  if(!txt) return;
  int otwidth = txt->w;
  int otheight = txt->h;
#endif
  
  if(otwidth+curx+1 > FONTTEXTURESIZE) curx = 0, cury += theight+1, theight = 0;
  
  theight = max(theight, otheight);
  
  for(int j=0; j<otheight;j++) for(int i=0; i<otwidth; i++) {
    fontdata[j+cury][i+curx] = 
#if CAP_TABFONT
    (i>=otwidth || j>=otheight) ? 0 : (tpix[tpixindex++] * 0x100) | 0xFF;
#else
    ((i>=txt->w || j>=txt->h) ? 0 : ((qpixel(txt, i, j)>>24)&0xFF) * 0x100) | 0x00FF;
#endif
    }
  
  auto& c = f.chars[ch];
  
  c.w = otwidth;
  c.h = otheight;

  c.tx0 = (float) curx / (float) FONTTEXTURESIZE;
  c.tx1 = (float) (curx+otwidth) / (float) FONTTEXTURESIZE;
  c.ty0 = (float) cury;
  c.ty1 = (float) (cury+otheight);
  curx += otwidth+1;
  }
  
EX void init_glfont(int size) {
  if(glfont[size]) return;
  DEBBI(DF_GRAPH, ("init GL font: ", size));
  
#if !CAP_TABFONT
  loadfont(size);
  if(!font[size]) return;
#endif
  
  glfont[size] = new glfont_t;
  
  glfont_t& f(*(glfont[size]));
  
  f.chars.resize(CHARS);

//f.list_base = glGenLists(128);
  glGenTextures(1, &f.texture );

#if !CAP_TABFONT
  char str[2]; str[1] = 0;
  
  SDL_Color white;
  white.r = white.g = white.b = 255;
#endif

  for(int y=0; y<FONTTEXTURESIZE; y++)
  for(int x=0; x<FONTTEXTURESIZE; x++)
    fontdata[y][x] = 0;

#if CAP_TABFONT
  resetTabFont();
#endif
  
//  glListBase(0);

  curx = 0, cury = 0, theight = 0;
  
  for(int ch=1;ch<CHARS;ch++) {
  
    if(ch<32) continue;

#if CAP_TABFONT
    sdltogl(NULL, f, ch);

#else
    SDL_Surface *txt;
    int siz = size;
    fix_font_size(siz);
    if(ch < 128) {
      str[0] = ch;
      txt = TTF_RenderText_Blended(font[siz], str, white);
      }
    else {
      txt = TTF_RenderUTF8_Blended(font[siz], natchars[ch-128], white);
      }
    if(txt == NULL) continue;
#if CAP_CREATEFONT
    generateFont(ch, txt);
#endif
    sdltogl(txt, f, ch);
    SDL_FreeSurface(txt);    
#endif
    }

  glBindTexture( GL_TEXTURE_2D, f.texture);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  
  theight = next_p2(cury + theight);
  
  glTexImage2D( GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, FONTTEXTURESIZE, theight, 0,
    GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 
    fontdata);

  for(int ch=0; ch<CHARS; ch++) f.chars[ch].ty0 /= theight, f.chars[ch].ty1 /= theight;
 
#if CAP_CREATEFONT
  printf("#define NUMEXTRA %d\n", NUMEXTRA);
#define DEMACRO(x) #x
  printf("#define NATCHARS " DEMACRO(NATCHARS) "\n");
#endif

//printf("init size=%d ok\n", size);
  GLERR("initfont");
  }

int gl_width(int size, const char *s) {
  int gsiz = size;
  if(size > vid.fsize || size > max_glfont_size) gsiz = max_glfont_size;

#if CAP_FIXEDSIZE
  gsiz = CAP_FIXEDSIZE;
#endif

  init_glfont(gsiz);
  if(!glfont[gsiz]) return 0;

  glfont_t& f(*glfont[gsiz]);

  int x = 0;
  for(int i=0; s[i];) {
    int tabid = getnext(s,i);    
    x += f.chars[tabid].w * size/gsiz;
    }
  
  return x;
  }

glhr::textured_vertex charvertex(int x1, int y1, ld tx, ld ty) {
  glhr::textured_vertex res;
  res.coords[0] = x1;
  res.coords[1] = y1;
  res.coords[2] = 0;
  res.coords[3] = 1;
  res.texture[0] = tx;
  res.texture[1] = ty;
  return res;
  }

bool gl_print(int x, int y, int shift, int size, const char *s, color_t color, int align) {
  int gsiz = size;
  if(size > vid.fsize || size > max_glfont_size) gsiz = max_glfont_size;

#if CAP_FIXEDSIZE
  gsiz = CAP_FIXEDSIZE;
#endif
  
  init_glfont(gsiz);
  if(!glfont[gsiz]) return false;

  glfont_t& f(*glfont[gsiz]);
  
  int tsize = 0;
  
  for(int i=0; s[i];) {
    tsize += f.chars[getnext(s,i)].w * size/gsiz;
    }
  x -= tsize * align / 16;
  y += f.chars[32].h * size / (gsiz*2);
  
  int ysiz = f.chars[32].h * size / gsiz;

  bool clicked = (mousex >= x && mousey <= y && mousex <= x+tsize && mousey >= y-ysiz);
  
  color_t icolor = (color << 8) | 0xFF;
  if(icolor != text_color || f.texture != text_texture || shift != text_shift || shapes_merged) {
    glflush();
    text_color = icolor;
    text_texture = f.texture;
    text_shift = shift;
    }
  texts_merged++;
  
  auto& tver = text_vertices;

  glBindTexture(GL_TEXTURE_2D, f.texture);

  for(int i=0; s[i];) {
  
    int tabid = getnext(s,i);
    auto& c = f.chars[tabid];
    int wi = c.w * size/gsiz;
    int hi = c.h * size/gsiz;

    GLERR("pre-print");
      
    glhr::textured_vertex t00 = charvertex(x,    y-hi, c.tx0, c.ty0);
    glhr::textured_vertex t01 = charvertex(x,    y,    c.tx0, c.ty1);
    glhr::textured_vertex t11 = charvertex(x+wi, y,    c.tx1, c.ty1);
    glhr::textured_vertex t10 = charvertex(x+wi, y-hi, c.tx1, c.ty0);
    
    tver.push_back(t00);
    tver.push_back(t01);
    tver.push_back(t10);
    tver.push_back(t10);
    tver.push_back(t01);
    tver.push_back(t11);
      
    x += wi;
    }
  
  return clicked;
  }

#endif

EX purehookset hooks_resetGL;

EX void resetGL() {
  DEBBI(DF_INIT | DF_GRAPH, ("reset GL"))
  callhooks(hooks_resetGL);
#if CAP_GLFONT
  for(int i=0; i<=max_glfont_size; i++) if(glfont[i]) {
    delete glfont[i];
    glfont[i] = NULL;
    }
#endif
#if MAXMDIM >= 4
  if(floor_textures) {
    delete floor_textures;
    floor_textures = NULL;
    }
#endif
  #if MAXMDIM >= 4 && CAP_GL
  if(airbuf) {
    delete airbuf;
    airbuf = nullptr;
    }
  #endif
  check_cgi();
  if(currentmap) cgi.require_shapes();
  #if MAXMDIM >= 4
  if(GDIM == 3 && !floor_textures) make_floor_textures();
  #endif
  cgi.initPolyForGL();
  compiled_programs.clear();
  matched_programs.clear();
  glhr::current_glprogram = nullptr;
  ray::reset_raycaster();
  #if CAP_RUG
  if(rug::glbuf) rug::close_glbuf();
  #endif
  }

#endif

#if CAP_XGD

vector<int> graphdata;

EX void gdpush(int t) {
  graphdata.push_back(t);
  }

EX bool displaychr(int x, int y, int shift, int size, char chr, color_t col) {
  gdpush(2); gdpush(x); gdpush(y); gdpush(8);
  gdpush(col); gdpush(size); gdpush(0);
  gdpush(1); gdpush(chr); 
  return false;
  }

void gdpush_utf8(const string& s) {
  int g = (int) graphdata.size(), q = 0;
  gdpush((int) s.size()); for(int i=0; i<isize(s); i++) {
#if ISANDROID
    unsigned char uch = (unsigned char) s[i];
    if(uch >= 192 && uch < 224) {
      int u = ((s[i] - 192)&31) << 6;
      i++;
      u += (s[i] - 128) & 63;
      gdpush(u); q++;
      }
    else if(uch >= 224 && uch < 240) {
      int u = ((s[i] - 224)&15) << 12;
      i++;
      u += (s[i] & 63) << 6;
      i++;
      u += (s[i] & 63) << 0;
      gdpush(u); q++;
      }
    else
#endif
      {
      gdpush(s[i]); q++;
      }
    }
  graphdata[g] = q;
  }

EX bool displayfr(int x, int y, int b, int size, const string &s, color_t color, int align) {
  gdpush(2); gdpush(x); gdpush(y); gdpush(align);
  gdpush(color); gdpush(size); gdpush(b);
  gdpush_utf8(s);
  int mx = mousex - x;
  int my = mousey - y;
  int len = textwidth(size, s);
  return 
    mx >= -len*align/32   && mx <= +len*(16-align)/32 && 
    my >= -size*3/4 && my <= +size*3/4;
  }

EX bool displaystr(int x, int y, int shift, int size, const string &s, color_t color, int align) {
  return displayfr(x,y,0,size,s,color,align);
  }

EX bool displaystr(int x, int y, int shift, int size, char const *s, color_t color, int align) {
  return displayfr(x,y,0,size,s,color,align);
  }

#endif
#if !CAP_XGD
EX bool displaystr(int x, int y, int shift, int size, const char *str, color_t color, int align) {

  if(strlen(str) == 0) return false;

  if(size < 4 || size > 2000) {
    return false;
    }
  
#if CAP_GLFONT
  if(vid.usingGL) return gl_print(x, y, shift, size, str, color, align);
#endif

#if !CAP_SDLTTF
  static bool towarn = true;
  if(towarn) towarn = false, printf("WARNING: NOTTF works only with OpenGL!\n");
  return false;
#else
  
  SDL_Color col;
  col.r = (color >> 16) & 255;
  col.g = (color >> 8 ) & 255;
  col.b = (color >> 0 ) & 255;
  
  col.r >>= darken; col.g >>= darken; col.b >>= darken;

  fix_font_size(size);
  loadfont(size);

  SDL_Surface *txt = ((vid.antialias & AA_FONT)?TTF_RenderUTF8_Blended:TTF_RenderUTF8_Solid)(font[size], str, col);
  
  if(txt == NULL) return false;

  SDL_Rect rect;

  rect.w = txt->w;
  rect.h = txt->h;

  rect.x = x - rect.w * align / 16;
  rect.y = y - rect.h/2;
  
  bool clicked = (mousex >= rect.x && mousey >= rect.y && mousex <= rect.x+rect.w && mousey <= rect.y+rect.h);
  
  if(shift) {
    #if CAP_SDL2
    SDL_Surface* txt2 = SDL_ConvertSurfaceFormat(txt, SDL_PIXELFORMAT_RGBA8888, 0);
    #else
    SDL_Surface* txt2 = SDL_DisplayFormat(txt);
    #endif
    SDL_LockSurface(txt2);
    SDL_LockSurface(s);
    color_t c0 = qpixel(txt2, 0, 0);
    for(int yy=0; yy<rect.h; yy++)
    for(int xx=0; xx<rect.w; xx++) if(qpixel(txt2, xx, yy) != c0)
      qpixel(s, rect.x+xx-shift, rect.y+yy) |= color & 0xFF0000,
      qpixel(s, rect.x+xx+shift, rect.y+yy) |= color & 0x00FFFF;
    SDL_UnlockSurface(s);
    SDL_UnlockSurface(txt2);
    SDL_FreeSurface(txt2);
    }
  else {
    SDL_BlitSurface(txt, NULL, s,&rect); 
    }
  SDL_FreeSurface(txt);
  
  return clicked;
#endif
  }
                  
EX bool displaystr(int x, int y, int shift, int size, const string &s, color_t color, int align) {
  return displaystr(x, y, shift, size, s.c_str(), color, align);
  }

EX bool displayfrSP(int x, int y, int sh, int b, int size, const string &s, color_t color, int align, int p) {
  if(b) {
    displaystr(x-b, y, 0, size, s, p, align);
    displaystr(x+b, y, 0, size, s, p, align);
    displaystr(x, y-b, 0, size, s, p, align);
    displaystr(x, y+b, 0, size, s, p, align);
    }
  if(b >= 2) {
    int b1 = b-1;
    displaystr(x-b1, y-b1, 0, size, s, p, align);
    displaystr(x-b1, y+b1, 0, size, s, p, align);
    displaystr(x+b1, y-b1, 0, size, s, p, align);
    displaystr(x+b1, y+b1, 0, size, s, p, align);
    }
  return displaystr(x, y, 0, size, s, color, align);
  }

EX bool displayfr(int x, int y, int b, int size, const string &s, color_t color, int align) {
  return displayfrSP(x, y, 0, b, size, s, color, align, poly_outline>>8);
  }

EX bool displaychr(int x, int y, int shift, int size, char chr, color_t col) {

  char buf[2];
  buf[0] = chr; buf[1] = 0;
  return displaystr(x, y, shift, size, buf, col, 8);
  }
#endif

#if HDR
struct msginfo {
  int stamp;
  time_t rtstamp;
  int gtstamp;
  int turnstamp;
  char flashout;
  char spamtype;
  int quantity;
  string msg;
  };
#endif

EX vector<msginfo> msgs;

EX vector<msginfo> gamelog;

EX void flashMessages() {
  for(int i=0; i<isize(msgs); i++) 
    if(msgs[i].stamp < ticks - 1000 && !msgs[i].flashout) {
      msgs[i].flashout = true;
      msgs[i].stamp = ticks;
      }
  }

EX string fullmsg(msginfo& m) {
  string s = m.msg;
  if(m.quantity > 1) s += " (x" + its(m.quantity) + ")";
  return s;
  }

void addMessageToLog(msginfo& m, vector<msginfo>& log) {

  if(isize(log) != 0) {
    msginfo& last = log[isize(log)-1];
    if(last.msg == m.msg) {
      int q = m.quantity + last.quantity;
      last = m; last.quantity = q;
      return;
      }
    }
  if(isize(log) < 1000)
    log.push_back(m);
  else {
    for(int i=0; i<isize(log)-1; i++) swap(log[i], log[i+1]);
    log[isize(log)-1] = m;
    }
  }

EX void clearMessages() { msgs.clear(); }

EX void addMessage(string s, char spamtype) {
  LATE( addMessage(s, spamtype); )
  DEBB(DF_MSG, ("addMessage: ", s));

  msginfo m;
  m.msg = s; m.spamtype = spamtype; m.flashout = false; m.stamp = ticks;
  m.rtstamp = time(NULL);
  m.gtstamp = getgametime();
  m.turnstamp = turncount;
  m.quantity = 1;
  
  addMessageToLog(m, gamelog);
  addMessageToLog(m, msgs);
  }

EX color_t colormix(color_t a, color_t b, color_t c) {
  for(int p=0; p<3; p++)
    part(a, p) = part(a,p) + (part(b,p) - part(a,p)) * part(c,p) / 255;
  return a;
  }

/* color difference for 24-bit colors, from 0 to 255*3 */
EX int color_diff(color_t a, color_t b) {
  int res = 0;
  for(int i=0; i<3; i++) res += abs(part(a, i) - part(b, i));
  return res;
  }

EX int rhypot(int a, int b) { return (int) sqrt(a*a - b*b); }

EX ld realradius() {
  ld vradius = current_display->radius;
  if(sphere) {
    if(sphereflipped()) 
      vradius /= sqrt(pconf.alpha*pconf.alpha - 1);
    else
      vradius = 1e12; // use the following
    }
  if(euclid)
    vradius = current_display->radius * get_sightrange() / (1 + pconf.alpha) / 2.5;
  vradius = min<ld>(vradius, min(vid.xres, vid.yres) / 2);
  return vradius;
  }

EX void drawmessage(const string& s, int& y, color_t col) {
  if(nomsg) return;
  int rrad = (int) realradius();
  int space;
  if(dual::state)
    space = vid.xres;
  else if(y > current_display->ycenter + rrad * pconf.stretch)
    space = vid.xres;
  else if(y > current_display->ycenter)
    space = current_display->xcenter - rhypot(rrad, (y-current_display->ycenter) / pconf.stretch);
  else if(y > current_display->ycenter - vid.fsize)
    space = current_display->xcenter - rrad;
  else if(y > current_display->ycenter - vid.fsize - rrad * pconf.stretch)
    space = current_display->xcenter - rhypot(rrad, (current_display->ycenter-vid.fsize-y) / pconf.stretch);
  else
    space = vid.xres;

  if(textwidth(vid.fsize, s) <= space) {
    displayfr(0, y, 1, vid.fsize, s, col, 0);
    y -= vid.fsize;
    return;
    }

  for(int i=1; i<isize(s); i++)
    if(s[i-1] == ' ' && textwidth(vid.fsize, "..."+s.substr(i)) <= space) {    
      displayfr(0, y, 1, vid.fsize, "..."+s.substr(i), col, 0);
      y -= vid.fsize;
      drawmessage(s.substr(0, i-1), y, col);
      return;
      }  

  // no chance
  displayfr(0, y, 1, vid.fsize, s, col, 0);
  y -= vid.fsize;
  return;
  }

EX void drawmessages() {
  DEBBI(DF_GRAPH, ("draw messages"));
  int i = 0;
  int t = ticks;
  for(int j=0; j<isize(msgs); j++) {
    if(j < isize(msgs) - vid.msglimit) continue;
    int age = msgs[j].flashout * (t - msgs[j].stamp);
    if(msgs[j].spamtype) {
      for(int i=j+1; i<isize(msgs); i++) if(msgs[i].spamtype == msgs[j].spamtype)
        msgs[j].flashout = 2;
      }
    if(age < 256*vid.flashtime)
      msgs[i++] = msgs[j];
    }
  msgs.resize(i);
  if(vid.msgleft == 2) {
    int y = vid.yres - vid.fsize - hud_margin(1);
    for(int j=isize(msgs)-1; j>=0; j--) {
      int age = msgs[j].flashout * (t - msgs[j].stamp);
      poly_outline = gradient(bordcolor, backcolor, 0, age, 256*vid.flashtime) << 8;
      color_t col = gradient(forecolor, backcolor, 0, age, 256*vid.flashtime);
      drawmessage(fullmsg(msgs[j]), y, col);
      }
    }
  else {
    for(int j=0; j<isize(msgs); j++) {
      int age = msgs[j].flashout * (t - msgs[j].stamp);
      int x = vid.msgleft ? 0 : vid.xres / 2;
      int y = vid.yres - vid.fsize * (isize(msgs) - j) - (ISIOS ? 4 : 0);
      poly_outline = gradient(bordcolor, backcolor, 0, age, 256*vid.flashtime) << 8;
      displayfr(x, y, 1, vid.fsize, fullmsg(msgs[j]), gradient(forecolor, backcolor, 0, age, 256*vid.flashtime), vid.msgleft ? 0 : 8);
      }
    }
  }

EX void drawCircle(int x, int y, int size, color_t color, color_t fillcolor IS(0)) {
  if(size < 0) size = -size;
  #if CAP_GL && CAP_POLY
  if(vid.usingGL) {
    glflush();
    glhr::be_nontextured();
    glhr::id_modelview();
    dynamicval<eModel> em(pmodel, mdPixel);
    glcoords.clear();
    x -= current_display->xcenter; y -= current_display->ycenter;
    int pts = size * 4;
    if(pts > 1500) pts = 1500;
    if(ISMOBILE && pts > 72) pts = 72;
    for(int r=0; r<pts; r++) {
      float rr = (M_PI * 2 * r) / pts;
      glcoords.push_back(glhr::makevertex(x + size * sin(rr), y + size * pconf.stretch * cos(rr), 0));
      }
    current_display->set_all(0, lband_shift);
    glhr::vertices(glcoords);
    glhr::set_depthtest(false);
    if(fillcolor) {
      glhr::color2(fillcolor);
      glDrawArrays(GL_TRIANGLE_FAN, 0, pts);
      }
    if(color) { 
      glhr::color2(color);
      glDrawArrays(GL_LINE_LOOP, 0, pts);
      }
    return;
    }
  #endif

#if CAP_XGD
  gdpush(4); gdpush(color); gdpush(fillcolor); gdpush(x); gdpush(y); gdpush(size);
#elif CAP_SDLGFX
  if(pconf.stretch == 1) {
    if(fillcolor) filledCircleColor(srend, x, y, size, fillcolor);
    if(color) ((vid.antialias && AA_NOGL)?aacircleColor:circleColor) (srend, x, y, size, align(color));
    }
  else {
    if(fillcolor) filledEllipseColor(srend, x, y, size, size * pconf.stretch, fillcolor);
    if(color) ((vid.antialias && AA_NOGL)?aaellipseColor:ellipseColor) (srend, x, y, size, size * pconf.stretch, align(color));
    }
#elif CAP_SDL
  int pts = size * 4;
  if(pts > 1500) pts = 1500;
  for(int r=0; r<pts; r++)
    qpixel(s, x + int(size * sin(r)), y + int(size * cos(r))) = color;
#endif
  }

EX void displayButton(int x, int y, const string& name, int key, int align, int rad IS(0)) {
  if(displayfr(x, y, rad, vid.fsize, name, 0x808080, align)) {
    displayfr(x, y, rad, vid.fsize, name, 0xFFFF00, align);
    getcstat = key;
    }
  }

#if HDR
#define SETMOUSEKEY 5000
#endif

EX char mousekey = 'n';
EX char newmousekey;

EX void displaymm(char c, int x, int y, int rad, int size, const string& title, int align) {
  if(displayfr(x, y, rad, size, title, c == mousekey ? 0xFF8000 : 0xC0C0C0, align)) {
    displayfr(x, y, rad, size, title, 0xFFFF00, align);
    getcstat = SETMOUSEKEY, newmousekey = c;
    }
  }  

EX bool displayButtonS(int x, int y, const string& name, color_t col, int align, int size) {
  if(displaystr(x, y, 0, size, name, col, align)) {
    displaystr(x, y, 0, size, name, 0xFFFF00, align);
    return true;
    }
  else return false;
  }

EX void displayColorButton(int x, int y, const string& name, int key, int align, int rad, color_t color, color_t color2 IS(0)) {
  if(displayfr(x, y, rad, vid.fsize, name, color, align)) {
    if(color2) displayfr(x, y, rad, vid.fsize, name, color2, align);
    getcstat = key;
    }
  }

ld textscale() { 
  return vid.fsize / (current_display->radius * cgi.crossf) * (1+pconf.alpha) * 2;
  }

EX void compute_fsize() {
  dual::split_or_do([&] {
    if(vid.relative_font)
      vid.fsize = min(vid.yres * vid.fontscale/ 3200, vid.xres * vid.fontscale/ 4800);
    else
      vid.fsize = vid.abs_fsize;
    if(vid.fsize < 6) vid.fsize = 6;
    });
  }

EX bool graphics_on;

EX bool request_resolution_change;

EX void do_request_resolution_change() { request_resolution_change = true; }

EX bool want_vsync() {
  if(vrhr::active())
    return false;
  return vid.want_vsync;
  }

EX bool need_to_reopen_window() {
  if(vid.want_antialias != vid.antialias)
    return true;
  if(vid.wantGL != vid.usingGL)
    return true;
  if(want_vsync() != vid.current_vsync)
    return true;
  if(request_resolution_change)
    return true;
  return false;
  }

EX bool need_to_apply_screen_settings() {
  if(need_to_reopen_window())
    return true;
  if(vid.want_fullscreen != vid.full)
    return true;
  if(make_pair(vid.xres, vid.yres) != get_requested_resolution())
    return true;
  return false;
  }

EX void close_renderer() {
  #if CAP_SDL2
  if(s_renderer) SDL_DestroyRenderer(s_renderer), s_renderer = nullptr;
  if(s_texture) SDL_DestroyTexture(s_texture), s_texture = nullptr;
  if(s) SDL_FreeSurface(s), s = nullptr;
  if(s_software_renderer) SDL_DestroyRenderer(s_software_renderer), s_software_renderer = nullptr;
  #endif
  }

EX void close_window() {
  #if CAP_SDL2
  close_renderer();
  if(s_have_context) {
    SDL_GL_DeleteContext(s_context), s_have_context = false;
    }
  if(s_window) SDL_DestroyWindow(s_window), s_window = nullptr;
  #endif
  }

EX void apply_screen_settings() {
  if(!need_to_apply_screen_settings()) return;
  if(!graphics_on) return;
 
#if ISANDROID
  if(vid.full != vid.want_fullscreen)
    addMessage(XLAT("Reenter HyperRogue to apply this setting"));
#endif

  close_renderer();
  #if CAP_VR
  if(vrhr::state) vrhr::shutdown_vr();
  #endif

  #if CAP_SDL
  #if !CAP_SDL2
  if(need_to_reopen_window())
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
  #endif
  #endif

  graphics_on = false;
  android_settings_changed();
  init_graph();
  #if CAP_GL
  if(vid.usingGL) {
    glhr::be_textured(); glhr::be_nontextured();
    }
  #endif
  }

EX pair<int, int> get_requested_resolution() {
  #if ISMOBILE || ISFAKEMOBILE
  return { vid.xres, vid.yres };
  #endif
  if(vid.want_fullscreen && vid.change_fullscr)
    return { vid.fullscreen_x, vid.fullscreen_y };
  else if(vid.want_fullscreen)
    return { vid.xres = vid.xscr, vid.yres = vid.yscr };
  else if(vid.relative_window_size)
    return { vid.xscr * vid.window_rel_x + .5, vid.yscr * vid.window_rel_y + .5 };
  else
    return { vid.window_x, vid.window_y };
  }

#ifndef CUSTOM_CAPTION
#define CUSTOM_CAPTION ("HyperRogue " VER)
#endif

EX bool resizable = true;

EX void setvideomode_android() {
  vid.usingGL = vid.wantGL;
  vid.full = vid.want_fullscreen;
  vid.antialias = vid.want_antialias;
  }

#if CAP_SDL

EX int current_window_flags = -1;

EX void setvideomode() {

  DEBBI(DF_INIT | DF_GRAPH, ("setvideomode"));
  
  vid.full = vid.want_fullscreen;
  
  tie(vid.xres, vid.yres) = get_requested_resolution();
    
  compute_fsize();

  int flags = 0;
  
  vid.antialias = vid.want_antialias;
    
#if CAP_GL
  vid.usingGL = vid.wantGL;
  if(vid.usingGL) {
    flags = SDL12(SDL_OPENGL | SDL_HWSURFACE, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);

    vid.current_vsync = want_vsync();
    #if !ISMOBWEB && !CAP_SDL2
    if(vid.current_vsync) 
      SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1 );
    else
      SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 0 ); 
    #endif
    if(vid.antialias & AA_MULTI) {
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, (vid.antialias & AA_MULTI16) ? 16 : 4);
      }
    }
#else
  vid.usingGL = false;
#endif

  int sizeflag = SDL12(vid.full ? SDL_FULLSCREEN : resizable ? SDL_RESIZABLE : 0, vid.full ? SDL_WINDOW_FULLSCREEN : resizable ? SDL_WINDOW_RESIZABLE : 0);

  #ifdef WINDOWS
  #ifndef OLD_MINGW
  static bool set_awareness = true;
  if(set_awareness) {
    set_awareness = false;
    HMODULE user32_dll = LoadLibraryA("User32.dll");
    if (user32_dll) {
      DPI_AWARENESS_CONTEXT (WINAPI * Loaded_SetProcessDpiAwarenessContext) (DPI_AWARENESS_CONTEXT) =
        (DPI_AWARENESS_CONTEXT (WINAPI *) (DPI_AWARENESS_CONTEXT)) (void*)
        GetProcAddress(user32_dll, "SetProcessDpiAwarenessContext");
      if(Loaded_SetProcessDpiAwarenessContext) {
        Loaded_SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        }
      FreeLibrary(user32_dll);
      }
    }
  #endif
  #endif
  
  #if CAP_SDL2
  if(s_renderer) SDL_DestroyRenderer(s_renderer), s_renderer = nullptr;
  #endif

  auto create_win = [&] {
    #if CAP_SDL2
    if(s_window && current_window_flags != (flags | sizeflag)) {
      if(s_have_context) {
        SDL_GL_DeleteContext(s_context), s_have_context = false;
        glhr::glew = false;
        }
      SDL_DestroyWindow(s_window), s_window = nullptr;
      }
    if(s_window)
      SDL_SetWindowSize(s_window, vid.xres, vid.yres);
    else
      s_window = SDL_CreateWindow(CUSTOM_CAPTION, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
      vid.xres, vid.yres,
      flags | sizeflag
      );
    current_window_flags = (flags | sizeflag);
    #else
    s = SDL_SetVideoMode(vid.xres, vid.yres, 32, flags | sizeflag);
    #endif  
    };
  
  create_win();
  
  auto& sw = SDL12(s, s_window);
  
  if(vid.full && !sw) {
    vid.xres = vid.xscr;
    vid.yres = vid.yscr;
    vid.fsize = 10;
    sizeflag = SDL12(SDL_FULLSCREEN, SDL_WINDOW_FULLSCREEN);
    create_win();
    }

  if(!sw) {
    addMessage("Failed to set the graphical mode: "+its(vid.xres)+"x"+its(vid.yres)+(vid.full ? " fullscreen" : " windowed"));
    vid.xres = 640;
    vid.yres = 480;
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);    
    vid.antialias &= ~AA_MULTI;
    sizeflag = SDL12(SDL_RESIZABLE, SDL_WINDOW_RESIZABLE);
    create_win();
    }
  
  #if CAP_SDL2
  if(s_renderer) SDL_DestroyRenderer(s_renderer), s_renderer = nullptr;
  s_renderer = SDL_CreateRenderer(s_window, -1, vid.current_vsync ? SDL_RENDERER_PRESENTVSYNC : 0);
  SDL_GetRendererOutputSize(s_renderer, &vid.xres, &vid.yres);
  
  if(s_texture) SDL_DestroyTexture(s_texture), s_texture = nullptr;
  s_texture = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, vid.xres, vid.yres);
  
  if(s) SDL_FreeSurface(s), s = nullptr;
  s = shot::empty_surface(vid.xres, vid.yres, false);
  
  if(s_software_renderer) SDL_DestroyRenderer(s_software_renderer), s_software_renderer = nullptr;
  s_software_renderer = SDL_CreateSoftwareRenderer(s);
  #endif
  s_screen = s;

#if CAP_GL
  if(vid.usingGL) {

    if(vid.antialias & AA_MULTI) {
      glEnable(GL_MULTISAMPLE);
      glEnable(GL_MULTISAMPLE_ARB);
      }
    else {
      glDisable(GL_MULTISAMPLE);
      glDisable(GL_MULTISAMPLE_ARB);
      }
  
    #if CAP_SDL2
    if(s_have_context) SDL_GL_DeleteContext(s_context), s_have_context = false;
    if(!s_have_context) s_context = SDL_GL_CreateContext(s_window);
    s_have_context = true; glhr::glew = false;
    #endif

    glViewport(0, 0, vid.xres, vid.yres);
    glhr::init();
    resetGL();
    }
#endif
  }
#endif

EX bool noGUI = false;

#if CAP_SDL
EX bool sdl_on = false;
EX int SDL_Init1(Uint32 flags) {
  if(!sdl_on) {
    sdl_on = true;
    return SDL_Init(flags);
    }
  else  
    return SDL_InitSubSystem(flags);
  }
#endif

EX void init_font() {
#if CAP_SDLTTF
  if(TTF_Init() != 0) {
    printf("Failed to initialize TTF.\n");
    exit(2);
    }
#endif
  }

EX void close_font() {
#if CAP_SDLTTF
  for(int i=0; i<=max_font_size; i++) if(font[i]) {
    TTF_CloseFont(font[i]);
    font[i] = nullptr;
    }
  TTF_Quit();
#endif
#if CAL_GLFONT
  for(int i=0; i<=max_glfont_size; i++) if(glfont[i]) {
    delete glfont[i];
    glfont[i] = nullptr;
    }
#endif
  }

EX void init_graph() {
#if CAP_SDL
  if (SDL_Init1(SDL_INIT_VIDEO) == -1)
  {
    printf("Failed to initialize video.\n");
    exit(2);
  }

#if ISWEB
  get_canvas_size();
#else
  if(!vid.xscr) {
    #if CAP_SDL2
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    vid.xscr = vid.xres = dm.w;
    vid.yscr = vid.yres = dm.h;
    #else
    const SDL_VideoInfo *inf = SDL_GetVideoInfo();
    vid.xscr = vid.xres = inf->current_w;
    vid.yscr = vid.yres = inf->current_h;
    #endif
    }
#endif

#if !CAP_SDL2
  SDL_WM_SetCaption(CUSTOM_CAPTION, CUSTOM_CAPTION);
#endif
#endif

  graphics_on = true;

#if ISIOS
  vid.usingGL = true;
#endif

#if ISANDROID
  setvideomode_android();
#endif

#if CAP_SDL
  setvideomode();
  if(!s) {
    printf("Failed to initialize graphics.\n");
    exit(2);
    }
    
  #if !CAP_SDL2
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
  SDL_EnableUNICODE(1);
  #endif
#endif  

#if ISANDROID
  vid.full = vid.want_fullscreen;
#endif
  }

EX void initialize_all() {

  DEBBI(DF_INIT | DF_GRAPH, ("initgraph"));
  
  initConfig();

#if CAP_SDLJOY
  joyx = joyy = 0; joydir.d = -1;
#endif
  
  restartGraph();
  
  if(noGUI) {
#if CAP_COMMANDLINE
    arg::read(2);
#endif
    return;
    }

  preparesort();
#if CAP_CONFIG
  loadConfig();
#endif
#if CAP_ARCM
  arcm::current.parse();
#endif
  if(hybri) geometry = hybrid::underlying;

#if CAP_COMMANDLINE
  arg::read(2);
#endif

  init_graph();
  check_cgi();
  cgi.require_basics();
  
  init_font();

#if CAP_SDLJOY  
  initJoysticks();
#endif

#if CAP_SDLAUDIO
  initAudio();
#endif
  }

EX void quit_all() {
  DEBBI(DF_INIT, ("clear graph"));
#if CAP_SDLJOY
  closeJoysticks();
#endif
#if CAP_SDL
  close_window();
  SDL_Quit();
  sdl_on = false;
#endif
  }

EX int calcfps() {
  #define CFPS 30
  static int last[CFPS], lidx = 0;
  int ct = ticks;
  int ret = ct - last[lidx];
  last[lidx] = ct;
  lidx++; lidx %= CFPS;
  if(ret == 0) return 0;
  return (1000 * CFPS) / ret;
  }

EX namespace subscreens {

  EX vector<display_data> player_displays;
  /** 'in' is on if we are currently working on a single display */
  EX bool in;
  EX int current_player;
  
  EX bool is_current_player(int id) {
    if(!in) return true;
    return id == current_player;
    }

  EX void prepare() {
    int N = multi::players;
    if(N > 1) {
      player_displays.resize(N, *current_display);
      int qrows[10] = {1, 1, 1, 1, 2, 2, 2, 3, 3, 3};
      int rows = qrows[N];
      int cols = (N + rows - 1) / rows;
      for(int i=0; i<N; i++) {
        auto& pd = player_displays[i];
        pd.xmin = (i % cols) * 1. / cols;
        pd.xmax = ((i % cols) + 1.) / cols;
        pd.ymin = (i / cols) * 1. / rows;
        pd.ymax = ((i / cols) + 1.) / rows;
        }
      }
    else {
      player_displays.clear();
      }
    }

  EX bool split(reaction_t what) {
    using namespace racing;
    if(in) return false;
    if(!multi::split_screen) return false;
    if(!player_displays.empty()) {
      in = true;
      int& p = current_player;
      for(p = 0; p < multi::players; p++) {
        dynamicval<display_data*> c(current_display, &player_displays[p]);
        what();
        }
      in = false;
      return true;
      }
    return false;
    }

  }

}
