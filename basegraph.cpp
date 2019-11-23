// Hyperbolic Rogue -- basic graphics
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file basegraph.cpp 
 *  \brief This file implements the basic graphical routines
 */

#include "hyper.h"
namespace hr {

EX int fontscale = 100;

#if HDR
/** configuration of the current view */
struct display_data {
  /** The cell which is currently in the center. */
  cell *precise_center;
  /** The current rotation, relative to precise_center. */
  transmatrix view_matrix;
  /** Camera rotation, used in nonisotropic geometries. */
  transmatrix local_perspective;
  /** The view relative to the player character. */
  transmatrix player_matrix;
  /** On-screen coordinates for all the visible cells. */
  unordered_map<cell*, transmatrix> cellmatrices, old_cellmatrices;
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

  ld eyewidth();
  bool stereo_active();
  bool in_anaglyph();

  void set_viewport(int ed);
  void set_projection(int ed);
  void set_mask(int ed);

  void set_all(int ed);
  /** Which copy of the player cell? */
  transmatrix which_copy;
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
EX int get_sightrange_ambush() { return max(get_sightrange(), ambush_distance); }

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
  printf("Unknown character in: '%s' at position %d\n", s, i);
  i ++; return '?';
  }

#if CAP_SDLTTF
TTF_Font *font[256];
#endif

#if CAP_SDL
EX SDL_Surface *s;
EX SDL_Surface *s_screen;

EX color_t qpixel_pixel_outside;

EX color_t& qpixel(SDL_Surface *surf, int x, int y) {
  if(x<0 || y<0 || x >= surf->w || y >= surf->h) return qpixel_pixel_outside;
  char *p = (char*) surf->pixels;
  p += y * surf->pitch;
  color_t *pi = (color_t*) (p);
  return pi[x];
  }

#endif

#if CAP_SDLTTF

EX string fontpath = ISWEB ? "sans-serif" : HYPERPATH "DejaVuSans-Bold.ttf";

void loadfont(int siz) {
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

EX int darkenedby(int c, int lev) {
  for(int i=0; i<lev; i++)
    c = ((c & 0xFEFEFE) >> 1);
  return c;
  }

bool fading = false;

ld fadeout = 1;

EX color_t darkened(color_t c) {
  if(inmirrorcount&1)
    c = gradient(c, winf[waMirror].color, 0, 0.5, 1);
  else if(inmirrorcount)
    c = gradient(c, winf[waCloud].color, 0, 0.5, 1);
  if(fading) c = gradient(backcolor, c, 0, fadeout, 1);
  if(vid.desaturate) {
    ld luminance = 0.2125 * part(c,2) + 0.7154 * part(c,1) + 0.0721 * part(c, 0);
    c = gradient(c, int(luminance+.5) * 0x10101, 0, vid.desaturate, 100);
    }
  for(int i=0; i<darken; i++)
    c = ((c & 0xFEFEFE) >> 1) + ((backcolor & 0xFEFEFE) >> 1);
  return c;
  }

EX color_t darkena3(color_t c, int lev, int a) {
  return (darkenedby(c, lev) << 8) + a;
  }

EX color_t darkena(color_t c, int lev, int a) {
  return darkena3(c, lev, GDIM == 3 ? 255 : a);
  }

#if !CAP_GL
void setcameraangle(bool b) { }
#endif

#if CAP_GL

#if CAP_VR
EX hookset<bool()> *hooks_vr_eye_view, *hooks_vr_eye_projection;
#endif

EX void eyewidth_translate(int ed) {
  glhr::using_eyeshift = false;
#if CAP_VR
  if(callhandlers(false, hooks_vr_eye_view)) ; else
#endif
  if(ed) glhr::projection_multiply(glhr::translate(-ed * current_display->eyewidth(), 0, 0));
  }

tuple<int, eModel, display_data*, int> last_projection;
EX bool new_projection_needed;
#if HDR
inline void reset_projection() { new_projection_needed = true; }
#endif

void display_data::set_all(int ed) {
  auto t = this;
  auto current_projection = tie(ed, pmodel, t, current_rbuffer);
  if(new_projection_needed || !glhr::current_glprogram || (next_shader_flags & GF_which) != (glhr::current_glprogram->shader_flags & GF_which) || current_projection != last_projection) {
    last_projection = current_projection;
    set_projection(ed);
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

struct glfont_t {
  GLuint texture;                                     // Holds The Texture Id
//GLuint list_base;                                   // Holds The First Display List ID  
  int widths[CHARS];
  int heights[CHARS];
  float tx0[CHARS], tx1[CHARS], ty0[CHARS], ty1[CHARS];
  };

glfont_t *glfont[256];

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
  
  if(otwidth+curx > FONTTEXTURESIZE) curx = 0, cury += theight, theight = 0;
  
  theight = max(theight, otheight);
  
  for(int j=0; j<otheight;j++) for(int i=0; i<otwidth; i++) {
    fontdata[j+cury][i+curx] = 
#if CAP_TABFONT
    (i>=otwidth || j>=otheight) ? 0 : (tpix[tpixindex++] * 0x100) | 0xFF;
#else
    ((i>=txt->w || j>=txt->h) ? 0 : ((qpixel(txt, i, j)>>24)&0xFF) * 0x100) | 0x00FF;
#endif
    }
  
  f.widths[ch] = otwidth;
  f.heights[ch] = otheight;

  f.tx0[ch] = (float) curx / (float) FONTTEXTURESIZE;
  f.tx1[ch] = (float) (curx+otwidth) / (float) FONTTEXTURESIZE;
  f.ty0[ch] = (float) cury;
  f.ty1[ch] = (float) (cury+otheight);
  curx += otwidth;
  }
  
void init_glfont(int size) {
  if(glfont[size]) return;
  DEBBI(DF_GRAPH, ("init GL font: ", size));
  
#if !CAP_TABFONT
  loadfont(size);
  if(!font[size]) return;
#endif
  
  glfont[size] = new glfont_t;
  
  glfont_t& f(*(glfont[size]));

//f.list_base = glGenLists(128);
  glGenTextures(1, &f.texture );

#if !CAP_TABFONT
  char str[2]; str[1] = 0;
  
  SDL_Color white;
  white.r = white.g = white.b = 255;
#endif

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
    if(ch < 128) {
      str[0] = ch;
      txt = TTF_RenderText_Blended(font[size], str, white);
      }
    else {
      txt = TTF_RenderUTF8_Blended(font[size], natchars[ch-128], white);
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

  for(int ch=0; ch<CHARS; ch++) f.ty0[ch] /= theight, f.ty1[ch] /= theight;
 
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
  if(size > vid.fsize || size > 72) gsiz = 72;

#if CAP_FIXEDSIZE
  gsiz = CAP_FIXEDSIZE;
#endif

  init_glfont(gsiz);
  if(!glfont[gsiz]) return 0;

  glfont_t& f(*glfont[gsiz]);

  int x = 0;
  for(int i=0; s[i];) {
    int tabid = getnext(s,i);    
    x += f.widths[tabid] * size/gsiz;
    }
  
  return x;
  }

vector<glhr::textured_vertex> tver;

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
  if(size > vid.fsize || size > 72) gsiz = 72;

#if CAP_FIXEDSIZE
  gsiz = CAP_FIXEDSIZE;
#endif
  
  init_glfont(gsiz);
  if(!glfont[gsiz]) return false;

  glfont_t& f(*glfont[gsiz]);
  
  int tsize = 0;
  
  for(int i=0; s[i];) {
    tsize += f.widths[getnext(s,i)] * size/gsiz;
    }
  x -= tsize * align / 16;
  y += f.heights[32] * size / (gsiz*2);
  
  int ysiz = f.heights[32] * size / gsiz;

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
    int wi = f.widths[tabid] * size/gsiz;
    int hi = f.heights[tabid] * size/gsiz;

    GLERR("pre-print");
  
    glhr::textured_vertex t00 = charvertex(x,    y-hi, f.tx0[tabid], f.ty0[tabid]);
    glhr::textured_vertex t01 = charvertex(x,    y,    f.tx0[tabid], f.ty1[tabid]);
    glhr::textured_vertex t11 = charvertex(x+wi, y,    f.tx1[tabid], f.ty1[tabid]);
    glhr::textured_vertex t10 = charvertex(x+wi, y-hi, f.tx1[tabid], f.ty0[tabid]);
    
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
  for(int i=0; i<128; i++) if(glfont[i]) {
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
  if(rug::glbuf) rug::close_glbuf();
  }

#endif

#if CAP_XGD

vector<int> graphdata;

void gdpush(int t) {
  graphdata.push_back(t);
  }

bool displaychr(int x, int y, int shift, int size, char chr, color_t col) {
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

  if(size < 4 || size > 255) {
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
    SDL_Surface* txt2 = SDL_DisplayFormat(txt);
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

EX int rhypot(int a, int b) { return (int) sqrt(a*a - b*b); }

EX ld realradius() {
  ld vradius = current_display->radius;
  if(sphere) {
    if(sphereflipped()) 
      vradius /= sqrt(vid.alpha*vid.alpha - 1);
    else
      vradius = 1e12; // use the following
    }
  if(euclid)
    vradius = current_display->radius * get_sightrange() / (1 + vid.alpha) / 2.5;
  vradius = min<ld>(vradius, min(vid.xres, vid.yres) / 2);
  return vradius;
  }

EX void drawmessage(const string& s, int& y, color_t col) {
  int rrad = (int) realradius();
  int space;
  if(dual::state)
    space = vid.xres;
  else if(y > current_display->ycenter + rrad * vid.stretch)
    space = vid.xres;
  else if(y > current_display->ycenter)
    space = current_display->xcenter - rhypot(rrad, (y-current_display->ycenter) / vid.stretch);
  else if(y > current_display->ycenter - vid.fsize)
    space = current_display->xcenter - rrad;
  else if(y > current_display->ycenter - vid.fsize - rrad * vid.stretch)
    space = current_display->xcenter - rhypot(rrad, (current_display->ycenter-vid.fsize-y) / vid.stretch);
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
    int y = vid.yres - vid.fsize - (ISIOS ? 4 : 0);
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

EX color_t gradient(color_t c0, color_t c1, ld v0, ld v, ld v1) {
  int vv = int(256 * ((v-v0) / (v1-v0)));
  color_t c = 0;
  for(int a=0; a<4; a++) {
    int p0 = part(c0, a);
    int p1 = part(c1, a);
    part(c, a) = (p0*(256-vv) + p1*vv + 127) >> 8;
    }
  return c;
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
      glcoords.push_back(glhr::makevertex(x + size * sin(rr), y + size * vid.stretch * cos(rr), 0));
      }
    current_display->set_all(0);
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
  gdpush(4); gdpush(color); gdpush(x); gdpush(y); gdpush(size);
#elif CAP_SDLGFX
  if(vid.stretch == 1) {
    if(fillcolor) filledCircleColor(s, x, y, size, fillcolor);
    if(color) ((vid.antialias && AA_NOGL)?aacircleColor:circleColor) (s, x, y, size, color);
    }
  else {
    if(fillcolor) filledEllipseColor(s, x, y, size, size * vid.stretch, fillcolor);
    if(color) ((vid.antialias && AA_NOGL)?aaellipseColor:ellipseColor) (s, x, y, size, size * vid.stretch, color);
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
  return vid.fsize / (current_display->radius * cgi.crossf) * (1+vid.alpha) * 2;
  }
  
EX bool setfsize = true;

EX bool vsync_off;

EX void do_setfsize() {
  dual::split_or_do([&] {
    vid.fsize = min(vid.yres * fontscale/ 3200, vid.xres * fontscale/ 4800), setfsize = false;
    });
  }

EX void disable_vsync() {
#if !ISMOBWEB
  SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 0 ); 
#endif
  }
  
#if CAP_SDL
EX void setvideomode() {

  DEBBI(DF_INIT | DF_GRAPH, ("setvideomode"));
  
  if(!vid.full) {
    if(vid.xres > vid.xscr) vid.xres = vid.xscr * 9/10, setfsize = true;
    if(vid.yres > vid.yscr) vid.yres = vid.yscr * 9/10, setfsize = true;    
    }
  
  if(setfsize) do_setfsize();

  int flags = 0;
  
#if CAP_GL
  if(vid.usingGL) {
    flags = SDL_OPENGL | SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER;
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
    if(vsync_off) disable_vsync();
    if(vid.antialias & AA_MULTI) {
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, (vid.antialias & AA_MULTI16) ? 16 : 4);
      }
    }
#endif

  int sizeflag = (vid.full ? SDL_FULLSCREEN : SDL_RESIZABLE);
  
  s = s_screen = SDL_SetVideoMode(vid.xres, vid.yres, 32, flags | sizeflag);
  
  if(vid.full && !s) {
    vid.xres = vid.xscr;
    vid.yres = vid.yscr;
    do_setfsize();
    s = s_screen = SDL_SetVideoMode(vid.xres, vid.yres, 32, flags | SDL_FULLSCREEN);
    }

  if(!s) {
    addMessage("Failed to set the graphical mode: "+its(vid.xres)+"x"+its(vid.yres)+(vid.full ? " fullscreen" : " windowed"));
    vid.xres = 640;
    vid.yres = 480;
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    vid.antialias &= ~AA_MULTI;
    s = s_screen = SDL_SetVideoMode(vid.xres, vid.yres, 32, flags | SDL_RESIZABLE);
    }

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
  
    glViewport(0, 0, vid.xres, vid.yres);
    glhr::init();
    resetGL();
    }
#endif
  }
#endif

EX bool noGUI = false;

EX void initgraph() {

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

#if CAP_SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1)
  {
    printf("Failed to initialize video.\n");
    exit(2);
  }

#if ISWEB
  vid.xscr = vid.xres = 1200;
  vid.yscr = vid.yres = 900;
#else
  const SDL_VideoInfo *inf = SDL_GetVideoInfo();
  vid.xscr = vid.xres = inf->current_w;
  vid.yscr = vid.yres = inf->current_h;
#endif

#ifdef CUSTOM_CAPTION  
  SDL_WM_SetCaption(CUSTOM_CAPTION, CUSTOM_CAPTION);
#else
  SDL_WM_SetCaption("HyperRogue " VER, "HyperRogue " VER);
#endif
#endif
  
  preparesort();
#if CAP_CONFIG
  loadConfig();
#endif
  arcm::current.parse();
  if(hybri) geometry = hybrid::underlying;

#if CAP_COMMANDLINE
  arg::read(2);
#endif
  check_cgi();
  cgi.require_basics();

#if CAP_SDL
  setvideomode();
  if(!s) {
    printf("Failed to initialize graphics.\n");
    exit(2);
    }
    
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
  SDL_EnableUNICODE(1);
#endif
  
#if CAP_SDLTTF
  if(TTF_Init() != 0) {
    printf("Failed to initialize TTF.\n");
    exit(2);
    }
#endif

#if CAP_SDLJOY  
  initJoysticks();
#endif

#if CAP_SDLAUDIO
  initAudio();
#endif
    
  }

EX void cleargraph() {
  DEBBI(DF_INIT, ("clear graph"));
#if CAP_SDLTTF
  for(int i=0; i<256; i++) if(font[i]) TTF_CloseFont(font[i]);
#endif
#if CAL_GLFONT
  for(int i=0; i<128; i++) if(glfont[i]) delete glfont[i];
#endif
#if CAP_SDLJOY
  closeJoysticks();
#endif
#if CAP_SDL
  SDL_Quit();
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
    if(!racing::on && !(shmup::on && GDIM == 3)) return false;
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
