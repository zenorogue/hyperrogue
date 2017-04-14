// Hyperbolic Rogue -- graphics

// Copyright (C) 2011-2012 Zeno Rogue, see 'hyper.cpp' for details

// basic graphics:

bool quitsaves() { return (items[itOrbSafety] && havesave); }

bool wmspatial, wmescher, wmplain, wmblack, wmascii;
bool mmspatial, mmhigh, mmmon, mmitem;
int maxreclevel, reclevel;

int lastt;

int backcolor = 0;

int detaillevel = 0;

#define WOLNIEJ 1
#define BTOFF 0x404040
#define BTON  0xC0C000

#define K(c) (c->type==6?0:1)

#ifdef ROGUEVIZ
#define DOSHMUP (shmup::on || rogueviz::on)
#else
#define DOSHMUP shmup::on
#endif

// #define PANDORA

#ifndef MOBILE
#include <SDL/SDL.h>

#ifdef SDLAUDIO
#include <SDL/SDL_mixer.h>
#endif

#ifndef MAC
#undef main
#endif

#include <SDL/SDL_ttf.h>
#endif

int colorbar;
#define COLORBAR "###"

#ifndef MOBILE
SDL_Surface *s;
TTF_Font *font[256];
SDL_Joystick* sticks[8];
int numsticks;
#endif

ld shiftmul = 1;

bool inHighQual; // taking high quality screenshot
int webdisplay = 0;

// R:239, G:208, B:207 

unsigned int skincolors[]  = { 7, 0xD0D0D0FF, 0xEFD0C9FF, 0xC77A58FF, 0xA58869FF, 0x602010FF, 0xFFDCB1FF, 0xEDE4C8FF };
unsigned int haircolors[]  = { 8, 0x686868FF, 0x8C684AFF, 0xF2E1AEFF, 0xB55239FF, 0xFFFFFFFF, 0x804000FF, 0x502810FF, 0x301800FF };
unsigned int dresscolors[] = { 6, 0xC00000FF, 0x00C000FF, 0x0000C0FF, 0xC0C000FF, 0xC0C0C0FF, 0x202020FF };
unsigned int dresscolors2[] = { 7, 0x8080FFC0, 0x80FF80C0, 0xFF8080C0, 0xFFFF80C0, 0xFF80FFC0, 0x80FFFFC0, 0xFFFFFF80 };
unsigned int swordcolors[] = { 6, 0xC0C0C0FF, 0xFFFFFFFF, 0xFFC0C0FF, 0xC0C0FFFF, 0x808080FF, 0x202020FF };
unsigned int eyecolors[] = { 4, 0x00C000FF, 0x0000C0FF, 0xC00000FF, 0xC0C000FF };

// is the player using mouse? (used for auto-cross)
bool mousing = true;

// is the mouse button pressed?
bool mousepressed = false;
bool mousemoved = false;
bool actonrelease = false;

emtype cmode = emNormal, lastmode = emNormal; // last mode in Help

// 
int axestate;

int ticks;
int frameid;

videopar vid;
int default_language;

charstyle& getcs() {
  if(multi::players>1 && multi::cpid >= 0 && multi::cpid < multi::players)
    return multi::scs[multi::cpid];
  else
    return vid.cs;
  }

bool camelotcheat;

int playergender() {
  return (getcs().charid&1) ? GEN_F : GEN_M; 
  }
int princessgender() {
  int g = playergender();
  if(vid.samegender) return g;
  return g == GEN_M ? GEN_F : GEN_M;
  }

int lang() { 
  if(vid.language >= 0)
    return vid.language; 
  return default_language;
  }

eItem orbToTarget;
eMonster monsterToSummon;

int sightrange = 7;
bool overgenerate = false; // generate a bigger area with high sightrange

cell *mouseover, *mouseover2, *lmouseover, *centerover;
ld modist, modist2, centdist;
string mouseovers;

movedir mousedest, joydir;

int mousex, mousey, joyx, joyy, panjoyx, panjoyy;
bool autojoy = true;
hyperpoint mouseh;

bool leftclick, rightclick, targetclick, hiliteclick, anyshiftclick, wheelclick,
  forcetarget, lshiftclick, lctrlclick;
bool gtouched;
bool revcontrol;

int getcstat; ld getcshift; bool inslider;

int ZZ;

string help;

int andmode = 0;

int darken = 0;

struct fallanim {
  int t_mon, t_floor;
  eWall walltype;
  eMonster m;
  fallanim() { t_floor = 0; t_mon = 0; walltype = waNone; }
  };

map<cell*, fallanim> fallanims;

bool doHighlight() {
  return (hiliteclick && darken < 2) ? !mmhigh : mmhigh;
  }

#ifndef MOBILE
int& qpixel(SDL_Surface *surf, int x, int y) {
  if(x<0 || y<0 || x >= surf->w || y >= surf->h) return ZZ;
  char *p = (char*) surf->pixels;
  p += y * surf->pitch;
  int *pi = (int*) (p);
  return pi[x];
  }

int qpixel3(SDL_Surface *surf, int x, int y) {
  if(x<0 || y<0 || x >= surf->w || y >= surf->h) return ZZ;
  char *p = (char*) surf->pixels;
  p += y * surf->pitch;
  p += x;
  int *pi = (int*) (p);
  return pi[0];
  }

void loadfont(int siz) {
  if(!font[siz]) {
#ifdef WEB
    font[siz] = TTF_OpenFont("sans-serif", siz);
#else
    font[siz] = TTF_OpenFont("DejaVuSans-Bold.ttf", siz);
#endif
    // Destination set by ./configure (in the GitHub repository)
    #ifdef FONTDESTDIR
    if (font[siz] == NULL) {
      font[siz] = TTF_OpenFont(FONTDESTDIR, siz);
      }
    #endif
    if (font[siz] == NULL) {
      printf("error: Font file not found\n");
      exit(1);
      }
    }
  }

int gl_width(int size, const char *s);

int textwidth(int siz, const string &str) {
  if(size(str) == 0) return 0;

#ifdef WEB
  return gl_width(siz, str.c_str());

#else

  loadfont(siz);
  
  int w, h;
  TTF_SizeUTF8(font[siz], str.c_str(), &w, &h);
  // printf("width = %d [%d]\n", w, size(str));
  return w;
#endif
  }
#endif

int textwidth(int siz, const string &str);

#ifdef IOS

int textwidth(int siz, const string &str) {
  return mainfont->getSize(str, siz / 36.0).width;
  }

#endif

int gradient(int c0, int c1, ld v0, ld v, ld v1);

#ifdef LOCAL
double fadeout = 1;
#endif

int darkened(int c) {
#ifdef LOCAL
  c = gradient(0, c, 0, fadeout, 1);
#endif
  // c = ((c & 0xFFFF) << 8) | ((c & 0xFF0000) >> 16);
  // c = ((c & 0xFFFF) << 8) | ((c & 0xFF0000) >> 16);
  for(int i=0; i<darken; i++) c &= 0xFEFEFE, c >>= 1;
  return c;
  }

int darkenedby(int c, int lev) {
  for(int i=0; i<lev; i++) c &= 0xFEFEFE, c >>= 1;
  return c;
  }

int darkena(int c, int lev, int a) {
  for(int i=0; i<lev; i++) c &= 0xFEFEFE, c >>= 1;
  return (c << 8) + a;
  }

#ifdef GL

bool cameraangle_on;

void setcameraangle(bool b) {
  if(cameraangle_on != b) {
    glMatrixMode(GL_PROJECTION);
    cameraangle_on = b;
    ld cam = vid.camera_angle * M_PI / 180;

    GLfloat cc = cos(cam);
    GLfloat ss = sin(cam * (b?1:-1));
    
    GLfloat yzspin[16] = {
      1, 0, 0, 0,
      0, cc, ss, 0,
      0, -ss, cc, 0,
      0, 0, 0, 1
      };
  
    glMultMatrixf(yzspin);
    }
  }

void selectEyeGL(int ed) {
  DEBB(DF_GRAPH, (debugfile,"selectEyeGL\n"));

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  glTranslatef((vid.xcenter*2.)/vid.xres - 1, 1 - (vid.ycenter*2.)/vid.yres, 0);

  if(pmodel) {
    vid.scrdist = 4 * vid.radius;

    // simulate glOrtho
    GLfloat ortho[16] = {
      GLfloat(2. / vid.xres), 0, 0, 0, 
      0, GLfloat(-2. / vid.yres), 0, 0, 
      0, 0, GLfloat(.4 / vid.scrdist), 0,
      0, 0, 0, 1};
  
    vid.scrdist = -vid.scrdist;
    glMultMatrixf(ortho);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    }
  else {
    float ve = ed*vid.eye;
    ve *= 2; // vid.xres; ve /= vid.radius;
    if(ve)
      glTranslatef(-(ve * vid.radius) * (vid.alpha - (vid.radius*1./vid.xres) * vid.eye) / vid.xres, 0, 0);

    float lowdepth = .1;
    float hidepth = 1e9;
  
    // simulate glFrustum
    GLfloat frustum[16] = {
      GLfloat(vid.yres * 1./vid.xres), 0, 0, 0, 
      0, 1, 0, 0, 
      0, 0, -(hidepth+lowdepth)/(hidepth-lowdepth), -1,
      0, 0, -2*lowdepth*hidepth/(hidepth-lowdepth), 0};
  
    glMultMatrixf(frustum);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  
    GLfloat sc = vid.radius / (vid.yres/2.);
    GLfloat mat[16] = {sc,0,0,0, 0,-sc,0,0, 0,0,-1,0, 0,0, 0,1};
    glMultMatrixf(mat);
    
    if(ve) glTranslatef(ve, 0, vid.eye);
    vid.scrdist = vid.yres * sc / 2;
    }
  
  cameraangle_on = false;
  }

void selectEyeMask(int ed) {
  if(ed == 0) {    
    glColorMask( GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE );
    }
  else if(ed == 1) {
    glColorMask( GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE );
    }
  else if(ed == -1) {
    glColorMask( GL_FALSE,GL_TRUE,GL_TRUE,GL_TRUE );
    }
  }

void setGLProjection() {
  DEBB(DF_GRAPH, (debugfile,"setGLProjection\n"));

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  unsigned char *c = (unsigned char*) (&backcolor);
  glClearColor(c[2] / 255.0, c[1] / 255.0, c[0]/255.0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  if(pmodel == mdBall || pmodel == mdHyperboloid) {
#ifdef GL_ES
    glClearDepthf(1.0f);
#else
    glClearDepth(1.0f);
#endif
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    }
  else
    glDisable(GL_DEPTH_TEST);
  
  selectEyeGL(0);
  }

void buildpolys();

#ifndef MOBILE

struct glfont_t {
  GLuint * textures;                                  // Holds The Texture Id's   
//GLuint list_base;                                   // Holds The First Display List ID  
  int widths[128+NUMEXTRA];
  int heights[128+NUMEXTRA];
  float tx[128+NUMEXTRA];
  float ty[128+NUMEXTRA];
  };

glfont_t *glfont[256];

inline int next_p2 (int a )
{
    int rval=1;
    // rval<<=1 Is A Prettier Way Of Writing rval*=2;
    while(rval<a) rval<<=1;
    return rval;
}

void glError(const char* GLcall, const char* file, const int line) {
  GLenum errCode = glGetError();
  if(errCode!=GL_NO_ERROR) {
    // fprintf(stderr, "OPENGL ERROR #%i: in file %s on line %i :: %s\n",errCode,file, line, GLcall);
    }
  }
#define GLERR(call) glError(call, __FILE__, __LINE__)

void init_glfont(int size) {
  if(glfont[size]) return;
  DEBB(DF_INIT, (debugfile,"init GL font: %d\n", size));
  
  glfont[size] = new glfont_t;
  
  glfont_t& f(*(glfont[size]));

  f.textures = new GLuint[128+NUMEXTRA];
//f.list_base = glGenLists(128);
  glGenTextures( 128+NUMEXTRA, f.textures );

#ifndef WEB
  loadfont(size);
  if(!font[size]) return;

  char str[2]; str[1] = 0;
  
  SDL_Color white;
  white.r = white.g = white.b = 255;
#endif
  
//  glListBase(0);
 
  for(int ch=1;ch<128+NUMEXTRA;ch++) {
  
#ifdef WEB
    if(ch<32) continue;
    int otwidth, otheight, tpix[3000], tpixindex = 0;    
    loadCompressedChar(otwidth, otheight, tpix);

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

    int otwidth = txt->w;
    int otheight = txt->h;
#endif

    int twidth = next_p2( otwidth );
    int theight = next_p2( otheight );

#ifdef WEB
    int expanded_data[twidth * theight];
#else
    Uint16 expanded_data[twidth * theight];
#endif

    for(int j=0; j <theight;j++) for(int i=0; i < twidth; i++) {
#ifdef WEB
      expanded_data[(i+j*twidth)] = (i>=otwidth || j>=otheight) ? 0 : tpix[tpixindex++];
#else
      expanded_data[(i+j*twidth)] = 
          (i>=txt->w || j>=txt->h) ? 0 : ((qpixel(txt, i, j)>>24)&0xFF) * 0x101;
#endif
      }
    
/*    if(ch == '@') {
      for(int j=0; j <theight;j++) {
        for(int i=0; i < twidth; i++) printf("%4x ", expanded_data[(i+j*twidth)]);
        printf("\n");
        }
      } */

//  printf("b\n");
    f.widths[ch] = otwidth;
    f.heights[ch] = otheight;
  
    glBindTexture( GL_TEXTURE_2D, f.textures[ch]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, twidth, theight, 0,
#ifdef WEB
      GL_RGBA, GL_UNSIGNED_BYTE, 
#else
      GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 
#endif
      expanded_data );
   
//  printf("c\n");

    float x=(float)otwidth / (float)twidth;
    float y=(float)otheight / (float)theight;
    f.tx[ch] = x;
    f.ty[ch] = y;

/*  glNewList(f.list_base+ch,GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, f.textures[ch]);
   
    //glPushMatrix();
   
    // glTranslatef(bitmap_glyph->left,0,0);
    // glTranslatef(0,bitmap_glyph->top-bitmap.rows,0);
   
//  printf("d\n");
  
    glBegin(GL_QUADS);
    float eps=0;
    glTexCoord2d(eps,eps); glVertex2f(0, txt->h);
    glTexCoord2d(eps,y-0); glVertex2f(0, 0);
    glTexCoord2d(x-eps,y-0); glVertex2f(txt->w, 0);
    glTexCoord2d(x-eps,0); glVertex2f(txt->w, txt->h);
    glEnd();
    glEndList(); */
    //glPopMatrix();

#ifndef WEB
    SDL_FreeSurface(txt);    
#endif
    }

//printf("init size=%d ok\n", size);
  GLERR("initfont");
  }

int getnext(const char* s, int& i) {
  for(int k=0; k<NUMEXTRA; k++)
    if(s[i] == natchars[k][0] && s[i+1] == natchars[k][1]) {
      i += 2; return 128+k;
      }
  if(s[i] < 0 && s[i+1] < 0) {
    printf("Unknown character: '%c%c'\n", s[i], s[i+1]);
    i += 2; return '?';
    }
  return s[i++];
  }

GLfloat tver[24];

int gl_width(int size, const char *s) {
  int gsiz = size;
  if(size > vid.fsize || size > 72) gsiz = 72;

#ifdef WEB
  gsiz = 36;
#endif

  init_glfont(gsiz);

#ifndef WEB
  if(!font[gsiz]) return 0;
#endif

  glfont_t& f(*glfont[gsiz]);

  int x = 0;
  for(int i=0; s[i];) {
    int tabid = getnext(s,i);    
    x += f.widths[tabid] * size/gsiz;
    }
  
  return x;
  }

bool gl_print(int x, int y, int shift, int size, const char *s, int color, int align) {

  // printf("gl_print: %s\n", s.c_str());
  
  // We Want A Coordinate System Where Distance Is Measured In Window Pixels.
  
  /*
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, 0, vid.xscr, vid.yscr); */
  
  int gsiz = size;
  if(size > vid.fsize || size > 72) gsiz = 72;
  // if(size >= 36) gsiz = 72;

#ifdef WEB
  gsiz = 36;
#endif
  
  init_glfont(gsiz);

#ifndef WEB
  if(!font[gsiz]) return false;
#endif

  glfont_t& f(*glfont[gsiz]);
  
  glDisable(GL_LIGHTING);
  
  glEnable(GL_TEXTURE_2D);

  glMatrixMode(GL_MODELVIEW);
  
  glcolor2((color << 8) | 0xFF);

  int tsize = 0;
  for(int i=0; s[i];) tsize += f.widths[getnext(s,i)] * size/gsiz;
  x -= tsize * align / 16;
  y += f.heights[32] * size / (gsiz*2);
  
  int ysiz = f.heights[32] * size / gsiz;

  bool clicked = (mousex >= x && mousey <= y && mousex <= x+tsize && mousey >= y-ysiz);

  /* extern bool markcorner;
  if(clicked && markcorner) {
    markcorner = false;
    int w = tsize, h = -ysiz;
    displaystr(x, y, 1, 10, "X", 0xFFFFFF, 8);
    displaystr(x+w, y, 1, 10, "X", 0xFFFFFF, 8);
    displaystr(x, y+h, 1, 10, "X", 0xFFFFFF, 8);
    displaystr(x+w, y+h, 1, 10, "X", 0xFFFFFF, 8);
    markcorner = true;
    } */
    
  for(int i=0; s[i];) {
  
    // glListBase(f.list_base);
    // glCallList(s[i]); // s[i]);
    
    int tabid = getnext(s,i);
    float fx=f.tx[tabid];
    float fy=f.ty[tabid];
    int wi = f.widths[tabid] * size/gsiz;
    int hi = f.heights[tabid] * size/gsiz;

    GLERR("pre-print");
    
    for(int ed = (vid.goteyes && shift)?-1:0; ed<2; ed+=2) {
      glPushMatrix();
      glTranslatef(x-ed*shift-vid.xcenter,y-vid.ycenter, vid.scrdist);
      selectEyeMask(ed);
      glBindTexture(GL_TEXTURE_2D, f.textures[tabid]);

#if 1
      tver[1] = tver[10] = -hi;
      tver[6] = tver[9] = wi;
      tver[12+4] = tver[12+7] = fy;
      tver[12+6] = tver[12+9] = fx;
      activateVertexArray(tver, 8);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(3, GL_FLOAT, 0, &tver[12]);
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#else
      glBegin(GL_QUADS);
      glTexCoord2d(0,0); glVertex2f(0, -hi);
      glTexCoord2d(0,fy); glVertex2f(0, 0);
      glTexCoord2d(fx,fy); glVertex2f(wi, 0);
      glTexCoord2d(fx,0); glVertex2f(wi, -hi);
      glEnd();
#endif
      glPopMatrix();
      }
    
    if(vid.goteyes) selectEyeMask(0);
    
    GLERR("print");
    
    // int tabid = s[i];
    x += wi;
    
/*  
    printf("point %d,%d\n", x, y);
    glBegin(GL_POINTS);
    glVertex3f(rand() % 100 - rand() % 100, rand() % 100 - rand() % 100, 100);
    glEnd(); */

    }
 
  glDisable(GL_TEXTURE_2D);
  
  return clicked;
  // printf("gl_print ok\n");
  }
#endif

void resetGL() {
  DEBB(DF_INIT, (debugfile,"reset GL\n"));
#ifndef MOBILE
  for(int i=0; i<128; i++) if(glfont[i]) {
    delete glfont[i];
    glfont[i] = NULL;
    }
#endif
  buildpolys();
  }

#endif

#ifndef MOBILE
bool displaystr(int x, int y, int shift, int size, const char *str, int color, int align) {
  if(strlen(str) == 0) return false;

  if(size < 4 || size > 255) {
    // printf("size = %d\n", size);
    return false;
    }
  
#ifdef WEB
  return gl_print(x, y, shift, size, str, color, align);
#endif
  
#ifdef GL
  if(vid.usingGL) return gl_print(x, y, shift, size, str, color, align);
#endif
  
  SDL_Color col;
  col.r = (color >> 16) & 255;
  col.g = (color >> 8 ) & 255;
  col.b = (color >> 0 ) & 255;
  
  col.r >>= darken; col.g >>= darken; col.b >>= darken;

  loadfont(size);

  SDL_Surface *txt = (vid.usingAA?TTF_RenderUTF8_Blended:TTF_RenderUTF8_Solid)(font[size], str, col);
  
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
    int c0 = qpixel(txt2, 0, 0);
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
  }
                  
bool displaystr(int x, int y, int shift, int size, const string &s, int color, int align) {
  return displaystr(x, y, shift, size, s.c_str(), color, align);
  }

bool displayfr(int x, int y, int b, int size, const string &s, int color, int align) {
  displaystr(x-b, y, 0, size, s, 0, align);
  displaystr(x+b, y, 0, size, s, 0, align);
  displaystr(x, y-b, 0, size, s, 0, align);
  displaystr(x, y+b, 0, size, s, 0, align);
  return displaystr(x, y, 0, size, s, color, align);
  }

bool displaychr(int x, int y, int shift, int size, char chr, int col) {

  char buf[2];
  buf[0] = chr; buf[1] = 0;
  return displaystr(x, y, shift, size, buf, col, 8);
  }

#else

vector<int> graphdata;

void gdpush(int t) {
  graphdata.push_back(t);
  }

bool displaychr(int x, int y, int shift, int size, char chr, int col) {
  gdpush(2); gdpush(x); gdpush(y); gdpush(8);
  gdpush(col); gdpush(size); gdpush(0);
  gdpush(1); gdpush(chr); 
  return false;
  }

void gdpush_utf8(const string& s) {
  int g = (int) graphdata.size(), q = 0;
  gdpush((int) s.size()); for(int i=0; i<s.size(); i++) {
#ifdef ANDROID
    unsigned char uch = (unsigned char) s[i];
    if(uch >= 192 && uch < 224) {
      int u = ((s[i] - 192)&31) << 6;
      i++;
      u += (s[i] - 128) & 63;
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

bool displayfr(int x, int y, int b, int size, const string &s, int color, int align) {
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

bool displaystr(int x, int y, int shift, int size, const string &s, int color, int align) {
  return displayfr(x,y,0,size,s,color,align);
  }

bool displaystr(int x, int y, int shift, int size, char const *s, int color, int align) {
  return displayfr(x,y,0,size,s,color,align);
  }

#endif

bool displaynum(int x, int y, int shift, int size, int col, int val, string title) {
  char buf[64];
  sprintf(buf, "%d", val);
  bool b1 = displayfr(x-8, y, 1, size, buf, col, 16);
  bool b2 = displayfr(x, y, 1, size, title, col, 0);
  if((b1 || b2) && gtouched) {
    col ^= 0x00FFFF;
    displayfr(x-8, y, 1, size, buf, col, 16);
    displayfr(x, y, 1, size, title, col, 0);
    }
  return b1 || b2;
  }

struct msginfo {
  int stamp;
  char flashout;
  char spamtype;
  int quantity;
  string msg;
  };

vector<msginfo> msgs;

vector<msginfo> gamelog;

void flashMessages() {
  for(int i=0; i<size(msgs); i++) 
    if(msgs[i].stamp < ticks - 1000 && !msgs[i].flashout) {
      msgs[i].flashout = true;
      msgs[i].stamp = ticks;
      }
  }

void addMessageToLog(msginfo& m, vector<msginfo>& log) {

  if(size(log) != 0) {
    msginfo& last = log[size(log)-1];
    if(last.msg == m.msg) {
      int q = m.quantity + last.quantity;
      last = m; last.quantity = q;
      return;
      }
    }
  if(size(log) < 200)
    log.push_back(m);
  else {
    for(int i=0; i<size(log)-1; i++) swap(log[i], log[i+1]);
    log[size(log)-1] = m;
    }
  }

void clearMessages() { msgs.clear(); }

void addMessage(string s, char spamtype) {
  DEBB(DF_MSG, (debugfile,"addMessage: %s\n", s.c_str()));

  msginfo m;
  m.msg = s; m.spamtype = spamtype; m.flashout = false; m.stamp = ticks;
  m.quantity = 1;
  
  addMessageToLog(m, gamelog);
  addMessageToLog(m, msgs);
  }

void drawmessages() {
  DEBB(DF_GRAPH, (debugfile,"draw messages\n"));
  int i = 0;
  int t = ticks;
  for(int j=0; j<size(msgs); j++) {
    int age = msgs[j].flashout * (t - msgs[j].stamp);
    if(msgs[j].spamtype) {
      for(int i=j+1; i<size(msgs); i++) if(msgs[i].spamtype == msgs[j].spamtype)
        msgs[j].flashout = 2;
      }
    if(age < 256*vid.flashtime) {
      int x = vid.xres / 2;
      int y = vid.yres - vid.fsize * (size(msgs) - j) - (ISIOS ? 4 : 0);
      string s = msgs[j].msg;
      if(msgs[j].quantity > 1) s += " (x" + its(msgs[j].quantity) + ")";
      displayfr(x, y, 1, vid.fsize, s, 0x10101 * (255 - age/vid.flashtime), 8);
      msgs[i++] = msgs[j];
      }
    }
  msgs.resize(i);
  }

eModel pmodel = mdDisk;

ld ghx, ghy, ghgx, ghgy;
hyperpoint ghpm = C0;

void ghcheck(hyperpoint &ret, const hyperpoint &H) {
  if(hypot(ret[0]-ghx, ret[1]-ghy) < hypot(ghgx-ghx, ghgy-ghy)) {
    ghpm = H; ghgx = ret[0]; ghgy = ret[1];
    }
  }

hyperpoint gethyper(ld x, ld y) {

  ld hx = (x - vid.xcenter) / vid.radius;
  ld hy = (y - vid.ycenter) / vid.radius;
  
  if(pmodel) {
    ghx = hx, ghy = hy;
    return ghpm;
    }
  
  if(euclid)
    return hpxy(hx * (EUCSCALE + vid.alphax), hy * (EUCSCALE + vid.alphax));
  
  ld hr = hx*hx+hy*hy;
  
  if(hr > .9999 && !sphere) return Hypc;
  
  // hz*hz-(hx/(hz+alpha))^2 - (hy/(hz+alpha))^2 =
  
  // hz*hz-hr*(hz+alpha)^2 == 1
  // hz*hz - hr*hr*hz*Hz
  
  
  ld A, B, C;
  
  ld curv = sphere ? 1 : -1;
  
  A = 1+curv*hr;
  B = 2*hr*vid.alphax*-curv;
  C = 1 - curv*hr*vid.alphax*vid.alphax;
  
  // Az^2 - Bz = C
  B /= A; C /= A;
  
  // z^2 - Bz = C
  // z^2 - Bz + (B^2/4) = C + (B^2/4)
  // z = (B/2) + sqrt(C + B^2/4)
  
  ld rootsign = 1;
  if(sphere && vid.alphax > 1) rootsign = -1;
  
  ld hz = B / 2 + rootsign * sqrt(C + B*B/4);
  
  hyperpoint H;
  H[0] = hx * (hz+vid.alphax);
  H[1] = hy * (hz+vid.alphax);
  H[2] = hz;
  
  return H;
  }

void ballmodel(hyperpoint& ret, double alpha, double d, double zl) {
  hyperpoint H = ypush(geom3::camera) * xpush(d) * ypush(zl) * C0;
  ld tzh = vid.ballproj + H[2];
  ld ax = H[0] / tzh;
  ld ay = H[1] / tzh;
  ld ball = vid.ballangle * M_PI / 180;
  
  ld ca = cos(alpha), sa = sin(alpha);
  ld cb = cos(ball), sb = sin(ball);
  
  ret[0] = ax * ca;
  ret[1] = ay * cb + ax * sa * sb;
  ret[2] = - ax * sa * cb - ay * sb;
  }

void applymodel(hyperpoint H, hyperpoint& ret) {
  
  ld tz = euclid ? (EUCSCALE+vid.alphax) : vid.alphax+H[2];
  if(tz < 1e-3 && tz > -1e-3) tz = 1000;
  
  if(pmodel == mdUnchanged) { 
    for(int i=0; i<3; i++) ret[i] = H[i] / vid.radius;
    return; 
    }
  
  if(pmodel == mdBall) {
    ld zlev = zlevel(H);
    using namespace hyperpoint_vec;
    H = H / zlev;
    
    ld zl = geom3::depth-geom3::factor_to_lev(zlev);
    double alpha = atan2(H[1], H[0]);
    double d = hdist0(H);

    ballmodel(ret, alpha, d, zl);
    ghcheck(ret,H);
    
    return;
    }
  
  if(pmodel == mdHyperboloid) {

    ld ball = vid.ballangle * M_PI / 180;
    ld cb = cos(ball), sb = sin(ball);

    ret[0] = H[0] / 3;
    ret[1] = (1 - H[2]) / 3 * cb + H[1] / 3 * sb;
    ret[2] = H[1] / 3 * cb - (1 - H[2]) / 3 * sb;

    ghcheck(ret,H);
    return;
    }
  
  if(pmodel == mdDisk) {
    ret[0] = H[0] / tz;
    ret[1] = H[1] / tz;
    ret[2] = (1 - vid.beta / tz);
    return;
    }

  ld zlev = 1;

  if(wmspatial || mmspatial) {
    zlev = zlevel(H);
    using namespace hyperpoint_vec;
    H = H / zlev;
    }

  if(pmodel == mdEquidistant || pmodel == mdEquiarea) {
    ld rad = sqrt(H[0] * H[0] + H[1] * H[1]);
    ld d = hdist0(H);
    if(pmodel == 6 && sphere)
      d = sqrt(2*(1 - cos(d))) * 1.25; // /1.5 to make it fit on the screen better
    else if(pmodel == 6)
      d = sqrt(2*(cosh(d) - 1)) / 1.5;
    ret[0] = d * H[0] / rad / 4;
    ret[1] = d * H[1] / rad / 4;
    ret[2] = 0; 
    if(zlev != 1 && vid.goteyes) 
      ret[2] = geom3::factor_to_lev(zlev);
    ghcheck(ret,H);
    return;
    }
  
  tz = H[2]+vid.alphax;

  if(pmodel == mdPolygonal || pmodel == mdPolynomial) {
    pair<long double, long double> p = polygonal::compute(H[0]/tz, H[1]/tz);
    ret[0] = p.first;
    ret[1] = p.second;
    ret[2] = 0;
    ghcheck(ret,H);
    return;
    }
  
  // Poincare to half-plane
  
  ld x0, y0;  
  x0 = H[0] / tz;
  y0 = H[1] / tz;
  y0 += 1;
  double rad = x0*x0 + y0*y0;
  y0 /= rad;
  x0 /= rad;
  y0 -= .5;
  
  if(pmodel == mdHalfplane) {
    ret[0] = x0;
    if(wmspatial || mmspatial) y0 *= zlev;
    ret[1] = 1 - y0;
    ret[2] = 0;
    if(zlev != 1 && vid.goteyes) 
      ret[2] = y0 * geom3::factor_to_lev(zlev);
    ghcheck(ret,H);
    return;
    }

  // center
  x0 *= 2; y0 *= 2;
  
  // half-plane to band
  double tau = (log((x0+1)*(x0+1) + y0*y0) - log((x0-1)*(x0-1) + y0*y0)) / 2;
  double u=(1-x0*x0-y0*y0);
  u = (1 - x0*x0 - y0*y0 + sqrt(u*u+4*y0*y0));
  double yv = 2*y0 / u;
  double sigma = 2 * atan(yv * zlev) - M_PI/2;
  
  x0 = tau; y0 = sigma;
  
  /* if(zlev != 1) {
    double alp = (y0 * y0) / (1-y0*y0);
    double gx = alp + sqrt(alp*alp-1);
    double gy = y0 * (gx+1);
    double yr = zlev * gy / (zlev * gx + 1);
    printf("zlev = %10.5lf y0 = %20.10lf yr = %20.10lf\n", double(zlev), (double)y0, yr);
    y0 = yr;
    } */
  
  ret[0] = x0/M_PI*2;
  ret[1] = -y0/M_PI*2;
  ret[2] = 0; 

  if(zlev != 1 && vid.goteyes) 
    ret[2] = geom3::factor_to_lev(zlev) / (1 + yv * yv);

  ghcheck(ret,H);
  }

int dlit;

// game-related graphics

transmatrix View; // current rotation, relative to viewctr
transmatrix cwtV; // player-relative view
transmatrix sphereflip; // on the sphere, flip
heptspin viewctr; // heptagon and rotation where the view is centered at
bool playerfound; // has player been found in the last drawing?

ld spina(cell *c, int dir) {
  return 2 * M_PI * dir / c->type;
  }

int gradient(int c0, int c1, ld v0, ld v, ld v1) {
  int vv = int(256 * ((v-v0) / (v1-v0)));
  int c = 0;
  for(int a=0; a<3; a++) {
    int p0 = (c0 >> (8*a)) & 255;
    int p1 = (c1 >> (8*a)) & 255;
    int p = (p0*(256-vv) + p1*vv + 127) >> 8;
    c += p << (8*a);
    }
  return c;
  }

// cloak color 
int cloakcolor(int rtr) {
  rtr -= 28;
  rtr /= 2;
  rtr %= 10;
  if(rtr < 0) rtr += 10;
  // rtr = time(NULL) % 10;
  int cc[10] = {
    0x8080FF, 0x80FFFF, 0x80FF80, 0xFF8080, 0xFF80FF, 0xFFFF80, 
    0xFFFFC0, 0xFFD500, 0x421C52, 0
    };
  return cc[rtr];
  }

int firegradient(double p) {
  return gradient(0xFFFF00, 0xFF0000, 0, p, 1);
  }
  
int firecolor(int phase) {
  return gradient(0xFFFF00, 0xFF0000, -1, sin((phase + ticks)/100.0), 1);
  }

int watercolor(int phase) {
  return 0x0080C0FF + 256 * int(63 * sin((ticks + phase) / 50.));
  }

int aircolor(int phase) {
  return 0x8080FF00 | int(32 + 32 * sin(ticks/200.0 + 2 * phase * M_PI / (S21+.0)));
  }

int fghostcolor(int phase, cell *c) {
  phase += (int)(size_t)c;  
  phase %= 4000;
  if(phase<0) phase+=4000;
  if(phase < 1000)      return gradient(0xFFFF80, 0xA0C0FF,    0, phase, 1000);
  else if(phase < 2000) return gradient(0xA0C0FF, 0xFF80FF, 1000, phase, 2000);
  else if(phase < 3000) return gradient(0xFF80FF, 0xFF8080, 2000, phase, 3000);
  else if(phase < 4000) return gradient(0xFF8080, 0xFFFF80, 3000, phase, 4000);
  return 0xFFD500;
  }

int weakfirecolor(int phase) {
  return gradient(0xFF8000, 0xFF0000, -1, sin((phase + ticks)/500.0), 1);
  }

int fc(int ph, int col, int z) {
  if(items[itOrbFire]) col = darkena(firecolor(ph), 0, 0xFF);
  if(items[itOrbAether]) col = (col &~0XFF) | (col&0xFF) / 2;
  for(int i=0; i<numplayers(); i++) if(multi::playerActive(i))
    if(items[itOrbFish] && isWatery(playerpos(i)) && z != 3) return watercolor(ph);
  if(invismove) 
    col = 
      shmup::on ?
        (col &~0XFF) | (int((col&0xFF) * .25))
      : (col &~0XFF) | (int((col&0xFF) * (100+100*sin(ticks / 500.)))/200);
  return col;
  }

int lightat, safetyat;
void drawLightning() { lightat = ticks; }
void drawSafety() { safetyat = ticks; }

double eurad = 0.52;
double q3 = sqrt(double(3));

bool outofmap(hyperpoint h) {
  if(euclid) 
    return h[0] * h[0] + h[1] * h[1] > 15 * eurad;
  else if(sphere)
    return h[2] < .1 && h[2] > -.1 && h[1] > -.1 && h[1] < .1 && h[0] > -.1 && h[0] < .1;
  else
    return h[2] < .5;
  }

void drawShield(const transmatrix& V, eItem it) {
  float ds = ticks / 300.;
  int col = iinf[it].color;
  if(it == itOrbShield && items[itOrbTime] && !orbused[it])
    col = (col & 0xFEFEFE) / 2;
  if(sphere && cwt.c->land == laHalloween && !wmblack && !wmascii)
    col = 0;
  double d = it == itOrbShield ? hexf : hexf - .1;
  int mt = sphere ? 7 : 5;
  for(int a=0; a<=S84*mt; a++)
    curvepoint(V*ddi0(a, d + sin(ds + M_PI*2*a/4/mt)*.1));
  queuecurve(darkena(col, 0, 0xFF), 0x8080808, PPR_LINE);
  }

void drawSpeed(const transmatrix& V) {
  ld ds = ticks / 10.;
  int col = darkena(iinf[itOrbSpeed].color, 0, 0xFF);
  for(int b=0; b<S84; b+=S14) {
    for(int a=0; a<=S84; a++)
      curvepoint(V*ddi0(ds+b+a, hexf*a/S84));
    queuecurve(col, 0x8080808, PPR_LINE);
    }
  }

void drawSafety(const transmatrix& V, int ct) {
  ld ds = ticks / 50.;
  int col = darkena(iinf[itOrbSafety].color, 0, 0xFF);
  for(int a=0; a<ct; a++)
    queueline(V*ddi0(ds+a*S84/ct, 2*hexf), V*ddi0(ds+(a+(ct-1)/2)*S84/ct, 2*hexf), col);
  }

void drawFlash(const transmatrix& V) {
  float ds = ticks / 300.;
  int col = darkena(iinf[itOrbFlash].color, 0, 0xFF);
  col &= ~1;
  for(int u=0; u<5; u++) {
    ld rad = hexf * (2.5 + .5 * sin(ds+u*.3));
    for(int a=0; a<=S84; a++) curvepoint(V*ddi0(a, rad));
    queuecurve(col, 0x8080808, PPR_LINE);
    }
  }

void drawLove(const transmatrix& V, int hdir) {
  float ds = ticks / 300.;
  int col = darkena(iinf[itOrbLove].color, 0, 0xFF);
  col &= ~1;
  for(int u=0; u<5; u++) {
    for(int a=0; a<=S84; a++) {
      double d = (1 + cos(a * M_PI/S42)) / 2;
      int z = a; if(z>S42) z = S84-z;
      if(z <= 10) d += (10-z) * (10-z) * (10-z) / 3000.;

      ld rad = hexf * (2.5 + .5 * sin(ds+u*.3)) * d;
      curvepoint(V*ddi0(S42+hdir+a-1, rad));
      }
    queuecurve(col, 0x8080808, PPR_LINE);
    }
  }

void drawWinter(const transmatrix& V, int hdir) {
  float ds = ticks / 300.;
  int col = darkena(iinf[itOrbWinter].color, 0, 0xFF);
  for(int u=0; u<20; u++) {
    ld rad = 6 * sin(ds+u * 2 * M_PI / 20);
    queueline(V*ddi0(S42+hdir+rad, hexf*.5), V*ddi0(S42+hdir+rad, hexf*3), col, 2);
    }
  }

void drawLightning(const transmatrix& V) {
  int col = darkena(iinf[itOrbLightning].color, 0, 0xFF);
  for(int u=0; u<20; u++) {
    ld leng = 0.5 / (0.1 + (rand() % 100) / 100.0);
    ld rad = rand() % S84;
    queueline(V*ddi0(rad, hexf*0.3), V*ddi0(rad, hexf*leng), col, 2);
    }
  }

int displaydir(cell *c, int d) {
  if(euclid)
    return - d * S84 / c->type;
  else
    return S42 - d * S84 / c->type;
  }

transmatrix ddspin(cell *c, int d, int bonus = 0) {
  int hdir = displaydir(c, d) + bonus;
  return getspinmatrix(hdir);
  }

#ifdef WEB
Uint8 *SDL_GetKeyState(void *v) { static Uint8 tab[1024]; return tab; }
#endif

#include "shmup.cpp"
#include "conformal.cpp"
#include "rug.cpp"

void drawPlayerEffects(const transmatrix& V, cell *c, bool onplayer) {
  if(!onplayer && !items[itOrbEmpathy]) return;
  if(items[itOrbShield] > (shmup::on ? 0 : ORBBASE)) drawShield(V, itOrbShield);
  if(items[itOrbShell] > (shmup::on ? 0 : ORBBASE)) drawShield(V, itOrbShell);

  if(items[itOrbSpeed]) drawSpeed(V); 

  int ct = c->type;
  
  if(onplayer && (items[itOrbSword] || items[itOrbSword2])) {
    using namespace sword;
  
    double esh = euclid ? M_PI - M_PI*3/S84 + 2.5 * M_PI/S42: 0;
    
    if(shmup::on) {
      if(items[itOrbSword])
        queuepoly(V*spin(esh+shmup::pc[multi::cpid]->swordangle), shMagicSword, darkena(iinf[itOrbSword].color, 0, 0xC0 + 0x30 * sin(ticks / 200.0)));
  
      if(items[itOrbSword2])
        queuepoly(V*spin(esh+shmup::pc[multi::cpid]->swordangle+M_PI), shMagicSword, darkena(iinf[itOrbSword2].color, 0, 0xC0 + 0x30 * sin(ticks / 200.0)));
      }                  
    
    else {
      int& ang = angle[multi::cpid];
      ang %= S42;
      
      transmatrix Vnow = gmatrix[c] * rgpushxto0(inverse(gmatrix[c]) * tC0(V));
      
      if(!euclid) for(int a=0; a<S42; a++) {
        int dda = S42 + (-1-2*a);
        if(a == ang && items[itOrbSword]) continue;
        if(purehepta && a%3 != ang%3) continue;
        if((a+S21)%S42 == ang && items[itOrbSword2]) continue;
        bool longer = sword::pos(cwt.c, a-1) != sword::pos(cwt.c, a+1);
        int col = darkena(0xC0C0C0, 0, 0xFF);
        queueline(Vnow*ddi0(dda, purehepta ? 0.6 : longer ? 0.36 : 0.4), Vnow*ddi0(dda, purehepta ? 0.7 : longer ? 0.44 : 0.42), col, 1);
        }
      
      if(items[itOrbSword])
        queuepoly(Vnow*spin(esh+M_PI+(-1-2*ang)*2*M_PI/S84), shMagicSword, darkena(iinf[itOrbSword].color, 0, 0x80 + 0x70 * sin(ticks / 200.0)));
  
      if(items[itOrbSword2])
        queuepoly(Vnow*spin(esh+(-1-2*ang)*2*M_PI/S84), shMagicSword, darkena(iinf[itOrbSword2].color, 0, 0x80 + 0x70 * sin(ticks / 200.0)));
      }
    }

  if(onplayer && items[itOrbSafety]) drawSafety(V, ct);

  if(onplayer && items[itOrbFlash]) drawFlash(V); 
  if(onplayer && items[itOrbLove]) drawLove(V, 0); // displaydir(c, cwt.spin)); 

  if(items[itOrbWinter]) 
    drawWinter(V, 0); // displaydir(c, cwt.spin));
  
  if(onplayer && items[itOrbLightning]) drawLightning(V);
  
  if(safetyat > 0) {
    int tim = ticks - safetyat;
    if(tim > 2500) safetyat = 0;
    for(int u=tim; u<=2500; u++) {
      if((u-tim)%250) continue;
      ld rad = hexf * u / 250;
      int col = darkena(iinf[itOrbSafety].color, 0, 0xFF);
      for(int a=0; a<S84; a++)
        queueline(V*ddi0(a, rad), V*ddi0(a+1, rad), col, 0);
      }
    }
  }

void drawStunStars(const transmatrix& V, int t) {
  for(int i=0; i<3*t; i++) {
    transmatrix V2 = V * spin(M_PI * 2 * i / (3*t) + M_PI * ticks/600.);
    queuepolyat(V2, shFlailBall, 0xFFFFFFFF, PPR_STUNSTARS);
    }
  }

bool drawUserShape(transmatrix V, int group, int id, int color) {
#ifdef MOBILE
  return false;
#else
  usershape *us = usershapes[group][id];
  if(!us) return false;

  for(int i=0; i<USERLAYERS; i++) {
    usershapelayer& ds(us->d[i]);
    hpcshape& sh(ds.sh);

    if(sh.s != sh.e) 
      queuepoly(V, sh, ds.color ? ds.color : color);
    }

#ifndef NOEDIT  
  if(cmode == emDraw && mapeditor::editingShape(group, id)) {

    usershapelayer &ds(usershapes[group][id]->d[mapeditor::dslayer]);
    
    /* for(int a=0; a<size(ds.list); a++) {
      hyperpoint P2 = V * ds.list[a];

      int xc, yc, sc;
      getcoord(P2, xc, yc, sc);
      queuechr(xc, yc, sc, 10, 'x', 
        a == 0 ? 0x00FF00 : 
        a == size(ds.list)-1 ? 0xFF0000 :
        0xFFFF00);
      } */
    
    hyperpoint mh = inverse(mapeditor::drawtrans) * mouseh;

    for(int a=0; a<ds.rots; a++) 
    for(int b=0; b<(ds.sym?2:1); b++) {

      if(outofmap(mouseh)) break;

      hyperpoint P2 = V * spin(2*M_PI*a/ds.rots) * (b?Mirror*mh:mh);
    
      queuechr(P2, 10, 'x', 0xFF00FF);
      }
    }
#endif

  return true;
#endif
  }

string csnameid(int id) {
  if(id == 0) return XLAT("male");
  if(id == 1) return XLAT("female");
  if(id == 2) return XLAT("Prince");
  if(id == 3) return XLAT("Princess");
  if(id == 4 || id == 5) return XLAT("cat");
  if(id == 6 || id == 7) return XLAT("dog");
  if(id == 8 || id == 9) return XLATN("Familiar");
  return XLAT("none");
  }

string csname(charstyle& cs) {
  return csnameid(cs.charid);
  }

namespace tortoise {

  // small is 0 or 2
  void draw(const transmatrix& V, int bits, int small, int stuntime) {

    int eyecolor = getBit(bits, tfEyeHue) ? 0xFF0000 : 0xC0C0C0;
    int shellcolor = getBit(bits, tfShellHue) ? 0x00C040 : 0xA06000;
    int scutecolor = getBit(bits, tfScuteHue) ? 0x00C040 : 0xA06000;
    int skincolor = getBit(bits, tfSkinHue) ? 0x00C040 : 0xA06000;
    if(getBit(bits, tfShellSat)) shellcolor = gradient(shellcolor, 0xB0B0B0, 0, .5, 1);
    if(getBit(bits, tfScuteSat)) scutecolor = gradient(scutecolor, 0xB0B0B0, 0, .5, 1);
    if(getBit(bits, tfSkinSat)) skincolor = gradient(skincolor, 0xB0B0B0, 0, .5, 1);
    if(getBit(bits, tfShellDark)) shellcolor = gradient(shellcolor, 0, 0, .5, 1);
    if(getBit(bits, tfSkinDark)) skincolor = gradient(skincolor, 0, 0, .5, 1);
    
    for(int i=0; i<12; i++) {    
      int col = 
        i == 0 ? shellcolor:
        i <  8 ? scutecolor :
        skincolor;
      int b = getBit(bits, i);
      int d = darkena(col, 0, 0xFF);
      if(i >= 1 && i <= 7) if(b) { d = darkena(col, 1, 0xFF); b = 0; }
      
      if(i >= 8 && i <= 11 && stuntime >= 3) continue;
      
      queuepoly(V, shTortoise[i][b+small], d);
      if((i >= 5 && i <= 7) || (i >= 9 && i <= 10))
        queuepoly(V * Mirror, shTortoise[i][b+small], d);
      
      if(i == 8) {
        for(int k=0; k<stuntime; k++) {
          eyecolor &= 0xFEFEFE; 
          eyecolor >>= 1;
          }
        queuepoly(V, shTortoise[12][b+small], darkena(eyecolor, 0, 0xFF));
        queuepoly(V * Mirror, shTortoise[12][b+small], darkena(eyecolor, 0, 0xFF));
        }
      }
    }

  int getMatchColor(int bits) {
    int mcol = 1;
    double d = tortoise::getScent(bits);
    
    if(d > 0) mcol = 0xFFFFFF;
    else if(d < 0) mcol = 0;
    
      int dd = 0xFF * (atan(fabs(d)/2) / (M_PI/2));
      /* poly_outline = OUTLINE_TRANS;
      queuepoly(V, shHeptaMarker, darkena(mcol, 0, dd));
      poly_outline = OUTLINE_NONE; */
    return gradient(0x487830, mcol, 0, dd, 0xFF);
    }
  };

double footfun(double d) {
  d -= floor(d);
  return
    d < .25 ? d :
    d < .75 ? .5-d :
    d-1;
  }

bool ivoryz;

void animallegs(const transmatrix& V, eMonster mo, int col, double footphase) {
  footphase /= SCALE;
  
  bool dog = mo == moRunDog;
  bool bug = mo == moBug0 || mo == moMetalBeast;

  if(bug) footphase *= 2.5;
  
  double rightfoot = footfun(footphase / .4 / 2) / 4 * 2;
  double leftfoot = footfun(footphase / .4 / 2 - (bug ? .5 : dog ? .1 : .25)) / 4 * 2;
  
  if(bug) rightfoot /= 2.5, leftfoot /= 2.5;
  
  if(!footphase) rightfoot = leftfoot = 0;

  transmatrix VAML = mmscale(V, 1.04);
  
  hpcshape* sh[6][4] = {
    {&shDogFrontPaw, &shDogRearPaw, &shDogFrontLeg, &shDogRearLeg},
    {&shWolfFrontPaw, &shWolfRearPaw, &shWolfFrontLeg, &shWolfRearLeg},
    {&shReptileFrontFoot, &shReptileRearFoot, &shReptileFrontLeg, &shReptileRearLeg},
    {&shBugLeg, NULL, NULL, NULL},
    {&shTrylobiteFrontClaw, &shTrylobiteRearClaw, &shTrylobiteFrontLeg, &shTrylobiteRearLeg},
    {&shBullFrontHoof, &shBullRearHoof, &shBullFrontHoof, &shBullRearHoof},
    };

  hpcshape **x = sh[mo == moRagingBull ? 5 : mo == moBug0 ? 3 : mo == moMetalBeast ? 4 : mo == moRunDog ? 0 : mo == moReptile ? 2 : 1];

  if(x[0]) queuepolyat(V * xpush(rightfoot), *x[0], col, PPR_MONSTER_FOOT);
  if(x[0]) queuepolyat(V * Mirror * xpush(leftfoot), *x[0], col, PPR_MONSTER_FOOT);
  if(x[1]) queuepolyat(V * xpush(-rightfoot), *x[1], col, PPR_MONSTER_FOOT);
  if(x[1]) queuepolyat(V * Mirror * xpush(-leftfoot), *x[1], col, PPR_MONSTER_FOOT);


  if(x[2]) queuepolyat(VAML * xpush(rightfoot/2), *x[2], col, PPR_MONSTER_FOOT);
  if(x[2]) queuepolyat(VAML * Mirror * xpush(leftfoot/2), *x[2], col, PPR_MONSTER_FOOT);
  if(x[3]) queuepolyat(VAML * xpush(-rightfoot/2), *x[3], col, PPR_MONSTER_FOOT);
  if(x[3]) queuepolyat(VAML * Mirror * xpush(-leftfoot/2), *x[3], col, PPR_MONSTER_FOOT);
  }

void ShadowV(const transmatrix& V, const hpcshape& bp, int prio) {
  if(mmspatial) { 
    if(pmodel == mdHyperboloid || pmodel == mdBall) 
      return; // shadows break the depth testing
    int p = poly_outline; poly_outline = OUTLINE_TRANS; 
    queuepolyat(V, bp, SHADOW_MON, prio); 
    poly_outline = p; 
    }
  }


void otherbodyparts(const transmatrix& V, int col, eMonster who, double footphase) {

#define VFOOT V
#define VLEG mmscale(V, geom3::LEG)
#define VGROIN mmscale(V, geom3::GROIN)
#define VBODY mmscale(V, geom3::BODY)
#define VNECK mmscale(V, geom3::NECK)
#define VHEAD mmscale(V, geom3::HEAD)

#define VALEGS V
#define VABODY mmscale(V, geom3::ABODY)
#define VAHEAD mmscale(V, geom3::AHEAD)

#define VFISH V
#define VBIRD  mmscale(V, geom3::BIRD + .05 * sin((int) (size_t(where)) + ticks / 1000.))
#define VGHOST  mmscale(V, geom3::GHOST)

#define VSLIMEEYE  mscale(V, geom3::FLATEYE)

  // if(!mmspatial && !footphase && who != moSkeleton) return;
  
  footphase /= SCALE;
  double rightfoot = footfun(footphase / .4 / 2.5) / 4 * 2.5;

  // todo

  if(detaillevel >= 2) { 
    transmatrix VL = mmscale(V, geom3::LEG1);
    queuepoly(VL * xpush(rightfoot*3/4), shHumanLeg, col);
    queuepoly(VL * Mirror * xpush(-rightfoot*3/4), shHumanLeg, col);
    }

  if(true) {
    transmatrix VL = mmscale(V, geom3::LEG);
    queuepoly(VL * xpush(rightfoot/2), shHumanLeg, col);
    queuepoly(VL * Mirror * xpush(-rightfoot/2), shHumanLeg, col);
    }

  if(detaillevel >= 2) { 
    transmatrix VL = mmscale(V, geom3::LEG3);
    queuepoly(VL * xpush(rightfoot/4), shHumanLeg, col);
    queuepoly(VL * Mirror * xpush(-rightfoot/4), shHumanLeg, col);
    }

  if(who == moWaterElemental) {
    double fishtail = footfun(footphase / .4) / 4 * 1.5;
    queuepoly(VFOOT * xpush(fishtail), shFishTail, watercolor(100));
    }
  else if(who == moSkeleton) {
    queuepoly(VFOOT * xpush(rightfoot), shSkeletalFoot, col);
    queuepoly(VFOOT * Mirror * xpush(-rightfoot), shSkeletalFoot, col);
    return;
    }
  else if(isTroll(who) || who == moMonkey || who == moYeti || who == moRatling || who == moRatlingAvenger || who == moGoblin) {
    queuepoly(VFOOT * xpush(rightfoot), shYetiFoot, col);
    queuepoly(VFOOT * Mirror * xpush(-rightfoot), shYetiFoot, col);
    }
  else {
    queuepoly(VFOOT * xpush(rightfoot), shHumanFoot, col);
    queuepoly(VFOOT * Mirror * xpush(-rightfoot), shHumanFoot, col);
    }

  if(!mmspatial) return;

  if(detaillevel >= 2 && who != moZombie)
    queuepoly(mmscale(V, geom3::NECK1), shHumanNeck, col);
  if(detaillevel >= 1) {
    queuepoly(VGROIN, shHumanGroin, col);
    if(who != moZombie) queuepoly(VNECK, shHumanNeck, col);
    }
  if(detaillevel >= 2) {
    queuepoly(mmscale(V, geom3::GROIN1), shHumanGroin, col);
    if(who != moZombie) queuepoly(mmscale(V, geom3::NECK3), shHumanNeck, col);
    }
  }

bool drawstar(cell *c) {
  for(int t=0; t<c->type; t++) 
    if(c->mov[t] && c->mov[t]->wall != waSulphur && c->mov[t]->wall != waSulphurC &&
     c->mov[t]->wall != waBarrier)
       return false;
  return true;
  }

bool drawMonsterType(eMonster m, cell *where, const transmatrix& V, int col, double footphase) {

  char xch = minf[m].glyph;

#ifndef NOEDIT
  if(where == mapeditor::drawcell)
    mapeditor::drawtrans = V;
#endif

  if(m == moTortoise && where && where->stuntime >= 3)
    drawStunStars(V, where->stuntime-2);
  else if (m == moTortoise || m == moPlayer || (where && !where->stuntime)) ;
  else if(where && !(isMetalBeast(m) && where->stuntime == 1))
    drawStunStars(V, where->stuntime);
    
  if(m == moTortoise) {
    int bits = where ? tortoise::getb(where) : 0;
    tortoise::draw(V, bits, 0, where ? where->stuntime : 0);
    if(tortoise::seek() && !tortoise::diff(bits))
      queuepoly(V, shRing, darkena(0xFFFFFF, 0, 0x80 + 0x70 * sin(ticks / 200.0)));
    }
    
  else if(m == moPlayer) {
  
    charstyle& cs = getcs();
      
    bool havus = drawUserShape(V, 0, cs.charid, cs.skincolor);

    if(mapeditor::drawplayer && !havus) {
    
      if(cs.charid >= 8) {
        queuepoly(VABODY, shWolfBody, fc(0, cs.skincolor, 0));
        if(!mmspatial && !footphase)
          queuepoly(VALEGS, shWolfLegs, fc(150, cs.dresscolor, 4));   
        else {
          ShadowV(V, shWolfBody);
          animallegs(VALEGS, moWolf, fc(500, cs.dresscolor, 4), footphase);
          }
        queuepoly(VAHEAD, shFamiliarHead, fc(500, cs.haircolor, 2));
        if(!shmup::on || shmup::curtime >= shmup::getPlayer()->nextshot) {
          int col = items[itOrbDiscord] ? watercolor(0) : fc(314, cs.swordcolor, 3);
          queuepoly(VAHEAD, shFamiliarEye, col);
          queuepoly(VAHEAD * Mirror, shFamiliarEye, col);
          }
        }
      else if(cs.charid >= 6) {
        if(!mmspatial && !footphase)
          queuepoly(VABODY, shDogBody, fc(0, cs.skincolor, 0));
        else {
          ShadowV(V, shDogTorso);          
          queuepoly(VABODY, shDogTorso, fc(0, cs.skincolor, 0));
          animallegs(VALEGS, moRunDog, fc(500, cs.dresscolor, 4), footphase);
          }
        queuepoly(VAHEAD, shDogHead, fc(150, cs.haircolor, 2));

        if(!shmup::on || shmup::curtime >= shmup::getPlayer()->nextshot) {
          int col = items[itOrbDiscord] ? watercolor(0) : fc(314, cs.swordcolor, 3);
          queuepoly(VAHEAD, shWolf1, col);
          queuepoly(VAHEAD, shWolf2, col);
          queuepoly(VAHEAD, shWolf3, col);
          }
        }
      else if(cs.charid >= 4) {
        queuepoly(VABODY, shCatBody, fc(0, cs.skincolor, 0));
        queuepoly(VAHEAD, shCatHead, fc(150, cs.haircolor, 2));
        if(!mmspatial && !footphase)
          queuepoly(VALEGS, shCatLegs, fc(500, cs.dresscolor, 4));
        else {
          ShadowV(V, shCatBody);
          animallegs(VALEGS, moRunDog, fc(500, cs.dresscolor, 4), footphase);
          }
        if(!shmup::on || shmup::curtime >= shmup::getPlayer()->nextshot) {
          int col = items[itOrbDiscord] ? watercolor(0) : fc(314, cs.swordcolor, 3);
          queuepoly(VAHEAD * xpush(.04), shWolf1, col);
          queuepoly(VAHEAD * xpush(.04), shWolf2, col);
          }
        }
      else {

      otherbodyparts(V, fc(0, cs.skincolor, 0), items[itOrbFish] ? moWaterElemental : moPlayer, footphase);

      queuepoly(VBODY, (cs.charid&1) ? shFemaleBody : shPBody, fc(0, cs.skincolor, 0));      

      ShadowV(V, (cs.charid&1) ? shFemaleBody : shPBody);

      if(items[itOrbHorns]) {
        queuepoly(VBODY, shBullHead, items[itOrbDiscord] ? watercolor(0) : 0xFF000030);
        queuepoly(VBODY, shBullHorn, items[itOrbDiscord] ? watercolor(0) : 0xFF000040);
        queuepoly(VBODY * Mirror, shBullHorn, items[itOrbDiscord] ? watercolor(0) : 0xFF000040);
        }
      if(items[itOrbThorns])
        queuepoly(VBODY, shHedgehogBladePlayer, items[itOrbDiscord] ? watercolor(0) : 0x00FF00FF);
      else if(!shmup::on && items[itOrbDiscord])
        queuepoly(VBODY, cs.charid >= 2 ? shSabre : shPSword, watercolor(0));
      else if(items[itRevolver])
        queuepoly(VBODY, shGunInHand, fc(314, cs.swordcolor, 3)); // 3 not colored
      else if(!shmup::on)
        queuepoly(VBODY, cs.charid >= 2 ? shSabre : shPSword, fc(314, cs.swordcolor, 3)); // 3 not colored
      else if(shmup::curtime >= shmup::getPlayer()->nextshot)
        queuepoly(VBODY, shPKnife, fc(314, cs.swordcolor, 3)); // 3 not colored
      
      if(items[itOrbBeauty]) {
        if(cs.charid&1)
          queuepoly(VHEAD, shFlowerHair, darkena(iinf[itOrbBeauty].color, 0, 0xFF));
        else
          queuepoly(VBODY, shFlowerHand, darkena(iinf[itOrbBeauty].color, 0, 0xFF));
        }
      
      if(where && where->land == laWildWest) {
        queuepoly(VHEAD, shWestHat1, darkena(cs.swordcolor, 1, 0XFF));
        queuepoly(VHEAD, shWestHat2, darkena(cs.swordcolor, 0, 0XFF));
        }

      if(cheater && !autocheat) {
        queuepoly(VHEAD, (cs.charid&1) ? shGoatHead : shDemon, darkena(0xFFFF00, 0, 0xFF));
        // queuepoly(V, shHood, darkena(0xFF00, 1, 0xFF));
        }
      else {
        queuepoly(VHEAD, shPFace, fc(500, cs.skincolor, 1));
        queuepoly(VHEAD, (cs.charid&1) ? shFemaleHair : shPHead, fc(150, cs.haircolor, 2));
        }
      if(cs.charid&1)
        queuepoly(VBODY, shFemaleDress, fc(500, cs.dresscolor, 4));

      if(cs.charid == 2)
        queuepoly(VBODY, shPrinceDress,  fc(400, cs.dresscolor, 5));
      if(cs.charid == 3) 
        queuepoly(VBODY, shPrincessDress,  fc(400, cs.dresscolor2, 5));
      
      }

      if(knighted)
        queuepoly(VBODY, shKnightCloak, darkena(cloakcolor(knighted), 1, 0xFF));

      if(tortoise::seek())
        tortoise::draw(VBODY * ypush(-crossf*.25), tortoise::seekbits, 4, 0);

      }
    }
  
  else if(drawUserShape(V, 1, m, darkena(col, 0, 0xFF))) return false;

  else if(isMimic(m) || m == moShadow || m == moIllusion) {
    charstyle& cs = getcs();
    
    if(drawUserShape(V, 0, (cs.charid&1)?1:0, darkena(col, 0, 0x80))) return false;
    
    if(cs.charid >= 8) {
      queuepoly(VABODY, shWolfBody, darkena(col, 0, 0xC0));
      ShadowV(V, shWolfBody);

      if(mmspatial || footphase)
        animallegs(VALEGS, moWolf, darkena(col, 0, 0xC0), footphase);
      else 
        queuepoly(VABODY, shWolfLegs, darkena(col, 0, 0xC0));

      queuepoly(VABODY, shFamiliarHead, darkena(col, 0, 0xC0));
      queuepoly(VAHEAD, shFamiliarEye, darkena(col, 0, 0xC0));
      queuepoly(VAHEAD * Mirror, shFamiliarEye, darkena(col, 0, 0xC0));
      }
    else if(cs.charid >= 6) {
      ShadowV(V, shDogBody);
      queuepoly(VAHEAD, shDogHead, darkena(col, 0, 0xC0));
      if(mmspatial || footphase) {
        animallegs(VALEGS, moRunDog, darkena(col, 0, 0xC0), footphase);
        queuepoly(VABODY, shDogTorso, darkena(col, 0, 0xC0));
        }
      else 
        queuepoly(VABODY, shDogBody, darkena(col, 0, 0xC0));
      queuepoly(VABODY, shWolf1, darkena(col, 1, 0xC0));
      queuepoly(VABODY, shWolf2, darkena(col, 1, 0xC0));
      queuepoly(VABODY, shWolf3, darkena(col, 1, 0xC0));
      }
    else if(cs.charid >= 4) {
      ShadowV(V, shCatBody);
      queuepoly(VABODY, shCatBody, darkena(col, 0, 0xC0));
      queuepoly(VAHEAD, shCatHead, darkena(col, 0, 0xC0));
      if(mmspatial || footphase) 
        animallegs(VALEGS, moRunDog, darkena(col, 0, 0xC0), footphase);
      else 
        queuepoly(VALEGS, shCatLegs, darkena(col, 0, 0xC0));
      queuepoly(VAHEAD * xpush(.04), shWolf1, darkena(col, 1, 0xC0));
      queuepoly(VAHEAD * xpush(.04), shWolf2, darkena(col, 1, 0xC0));
      }
    else {
      otherbodyparts(V, darkena(col, 0, 0x40), m, footphase);
      queuepoly(VBODY, (cs.charid&1) ? shFemaleBody : shPBody,  darkena(col, 0, 0X80));
  
      if(!shmup::on)
        queuepoly(VBODY, (cs.charid >= 2 ? shSabre : shPSword), darkena(col, 0, 0XC0));
      else if(shmup::curtime >= shmup::getPlayer()->nextshot)
        queuepoly(VBODY, shPKnife, darkena(col, 0, 0XC0));
  
      queuepoly(VHEAD, (cs.charid&1) ? shFemaleHair : shPHead,  darkena(col, 1, 0XC0));
      queuepoly(VHEAD, shPFace,  darkena(col, 0, 0XC0));
      if(cs.charid&1)
        queuepoly(VBODY, shFemaleDress,  darkena(col, 1, 0XC0));
      if(cs.charid == 2)
        queuepoly(VBODY, shPrinceDress,  darkena(col, 1, 0XC0));
      if(cs.charid == 3)
        queuepoly(VBODY, shPrincessDress,  darkena(col, 1, 0XC0));
      }
    }
  
  else if(m == moBullet) {
    ShadowV(V, shKnife);
    queuepoly(VBODY * spin(-M_PI/4), shKnife, getcs().swordcolor);
    }
  
  else if(m == moKnight || m == moKnightMoved) {
    ShadowV(V, shPBody);
    otherbodyparts(V, darkena(0xC0C0A0, 0, 0xC0), m, footphase);
    queuepoly(VBODY, shPBody, darkena(0xC0C0A0, 0, 0xC0));
    queuepoly(VBODY, shPSword, darkena(0xFFFF00, 0, 0xFF));
    queuepoly(VBODY, shKnightArmor, darkena(0xD0D0D0, 1, 0xFF));
    int col;
    if(!euclid && where && where->master->alt)
      col = cloakcolor(roundTableRadius(where));
    else
      col = cloakcolor(newRoundTableRadius());
    queuepoly(VBODY, shKnightCloak, darkena(col, 1, 0xFF));
    queuepoly(VHEAD, shPHead, darkena(0x703800, 1, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xC0C0A0, 0, 0XFF));
    return false;
    }
  
  else if(m == moGolem || m == moGolemMoved) {
    ShadowV(V, shPBody);
    otherbodyparts(V, darkena(col, 1, 0XC0), m, footphase);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0XC0));
    queuepoly(VHEAD, shGolemhead, darkena(col, 1, 0XFF));
    }

  else if(isPrincess(m) || m == moFalsePrincess || m == moRoseLady || m == moRoseBeauty) {
  
    bool girl = princessgender() == GEN_F;
    bool evil = !isPrincess(m);

    int facecolor = evil ? 0xC0B090FF : 0xD0C080FF;
    
    ShadowV(V, girl ? shFemaleBody : shPBody);
    otherbodyparts(V, facecolor, m, footphase);
    queuepoly(VBODY, girl ? shFemaleBody : shPBody, facecolor);

    if(m == moPrincessArmed) 
      queuepoly(VBODY, vid.cs.charid < 2 ? shSabre : shPSword, 0xFFFFFFFF);
    
    if((m == moFalsePrincess || m == moRoseBeauty) && where && where->cpdist == 1)
      queuepoly(VBODY, shPKnife, 0xFFFFFFFF);

    if(m == moRoseLady) {
      queuepoly(VBODY, shPKnife, 0xFFFFFFFF);
      queuepoly(VBODY * Mirror, shPKnife, 0xFFFFFFFF);
      }

    if(m == moRoseLady) {
//    queuepoly(V, girl ? shGoatHead : shDemon,  0x800000FF);
      queuepoly(VHEAD, girl ? shFemaleHair : shPHead,  evil ? 0x500050FF : 0x332A22FF);
      }
    else if(m == moRoseBeauty) {
      if(girl) {
        queuepoly(VHEAD, shBeautyHair,  0xF0A0D0FF);
        queuepoly(VHEAD, shFlowerHair,  0xC00000FF);
        }
      else {
        queuepoly(VHEAD, shPHead,  0xF0A0D0FF);
        queuepoly(VHEAD, shFlowerHand,  0xC00000FF);
        queuepoly(VBODY, shSuspenders,  0xC00000FF);
        }
      }
    else {
      queuepoly(VHEAD, girl ? shFemaleHair : shPHead,  
        evil ? 0xC00000FF : 0x332A22FF);
      }
    queuepoly(VHEAD, shPFace,  facecolor);

    if(girl) {
      queuepoly(VBODY, shFemaleDress,  evil ? 0xC000C0FF : 0x00C000FF);
      if(vid.cs.charid < 2) 
        queuepoly(VBODY, shPrincessDress, evil ? 0xC040C0C0 : 0x8080FFC0);
      }
    else {
      if(vid.cs.charid < 2) 
        queuepoly(VBODY, shPrinceDress,  evil ? 0x802080FF : 0x404040FF);
      }    
    }

  else if(m == moWolf || m == moRedFox || m == moWolfMoved) {
    ShadowV(V, shWolfBody);
    if(mmspatial || footphase)
      animallegs(VALEGS, moWolf, darkena(col, 0, 0xFF), footphase);
    else
      queuepoly(VALEGS, shWolfLegs, darkena(col, 0, 0xFF));
    queuepoly(VABODY, shWolfBody, darkena(col, 0, 0xFF));
    queuepoly(VAHEAD, shWolfHead, darkena(col, 0, 0xFF));
    queuepoly(VAHEAD, shWolfEyes, darkena(col, 3, 0xFF));
    }

  else if(isBull(m)) {
    ShadowV(V, shBullBody);
    int hoofcol = darkena(gradient(0, col, 0, .65, 1), 0, 0xFF);
    if(mmspatial || footphase)
      animallegs(VALEGS, moRagingBull, hoofcol, footphase);
    queuepoly(VABODY, shBullBody, darkena(gradient(0, col, 0, .80, 1), 0, 0xFF));
    queuepoly(VAHEAD, shBullHead, darkena(col, 0, 0xFF));
    queuepoly(VAHEAD, shBullHorn, darkena(0xFFFFFF, 0, 0xFF));
    queuepoly(VAHEAD * Mirror, shBullHorn, darkena(0xFFFFFF, 0, 0xFF));
    }

  else if(m == moReptile) {
    ShadowV(V, shReptileBody);
    animallegs(VALEGS, moReptile, darkena(col, 0, 0xFF), footphase);
    queuepoly(VABODY, shReptileBody, darkena(col, 0, 0xFF));
    queuepoly(VAHEAD, shReptileHead, darkena(col, 0, 0xFF));
    queuepoly(VAHEAD, shReptileEye, darkena(col, 3, 0xFF));
    queuepoly(VAHEAD * Mirror, shReptileEye, darkena(col, 3, 0xFF));
    queuepoly(VABODY, shReptileTail, darkena(col, 2, 0xFF));
    }
  else if(m == moVineBeast) {
    ShadowV(V, shWolfBody);
    if(mmspatial || footphase)
      animallegs(VALEGS, moWolf, 0x00FF00FF, footphase);
    else
      queuepoly(VALEGS, shWolfLegs, 0x00FF00FF);
    queuepoly(VABODY, shWolfBody, darkena(col, 1, 0xFF));
    queuepoly(VAHEAD, shWolfHead, darkena(col, 0, 0xFF));
    queuepoly(VAHEAD, shWolfEyes, 0xFF0000FF);
    }
  else if(m == moMouse || m == moMouseMoved) {
    queuepoly(VALEGS, shMouse, darkena(col, 0, 0xFF));
    queuepoly(VALEGS, shMouseLegs, darkena(col, 1, 0xFF));
    queuepoly(VALEGS, shMouseEyes, 0xFF);
    }
  else if(isBug(m)) {
    ShadowV(V, shBugBody);
    if(!mmspatial && !footphase)
      queuepoly(VABODY, shBugBody, darkena(col, 0, 0xFF));
    else {
      animallegs(VALEGS, moBug0, darkena(col, 0, 0xFF), footphase);
      queuepoly(VABODY, shBugAntenna, darkena(col, 1, 0xFF));
      }
    queuepoly(VABODY, shBugArmor, darkena(col, 1, 0xFF));
    }
  else if(m == moRunDog) {
    if(!mmspatial && !footphase) 
      queuepoly(VABODY, shDogBody, darkena(col, 0, 0xFF));
    else {
      ShadowV(V, shDogTorso);
      queuepoly(VABODY, shDogTorso, darkena(col, 0, 0xFF));
      animallegs(VALEGS, moRunDog, darkena(col, 0, 0xFF), footphase);
      }
    queuepoly(VAHEAD, shDogHead, darkena(col, 0, 0xFF));
    queuepoly(VAHEAD, shWolf1, darkena(0x202020, 0, 0xFF));
    queuepoly(VAHEAD, shWolf2, darkena(0x202020, 0, 0xFF));
    queuepoly(VAHEAD, shWolf3, darkena(0x202020, 0, 0xFF));
    }
  else if(m == moOrangeDog) {
    if(!mmspatial && !footphase) 
      queuepoly(VABODY, shDogBody, darkena(0xFFFFFF, 0, 0xFF));
    else {
      ShadowV(V, shDogTorso);
      queuepoly(VABODY, shDogTorso, darkena(0xFFFFFF, 0, 0xFF));
      animallegs(VALEGS, moRunDog, darkena(0xFFFFFF, 0, 0xFF), footphase);
      }
    queuepoly(VAHEAD, shDogHead, darkena(0xFFFFFF, 0, 0xFF));
    queuepoly(VABODY, shDogStripes, darkena(0x303030, 0, 0xFF));
    queuepoly(VAHEAD, shWolf1, darkena(0x202020, 0, 0xFF));
    queuepoly(VAHEAD, shWolf2, darkena(0x202020, 0, 0xFF));
    queuepoly(VAHEAD, shWolf3, darkena(0x202020, 0, 0xFF));
    }
  else if(m == moShark || m == moGreaterShark || m == moCShark)
    queuepoly(VFISH, shShark, darkena(col, 0, 0xFF));
  else if(m == moEagle || m == moParrot || m == moBomberbird || m == moAlbatross || 
    m == moTameBomberbird || m == moWindCrow || m == moTameBomberbirdMoved) {
    ShadowV(V, shEagle);
    queuepoly(VBIRD, shEagle, darkena(col, 0, 0xFF));
    }
  else if(m == moSparrowhawk) {
    ShadowV(V, shHawk);
    queuepoly(VBIRD, shHawk, darkena(col, 0, 0xFF));
    }
  else if(m == moButterfly) {
    transmatrix Vwing = Id;
    Vwing[1][1] = .85 + .15 * sin(ticks / 100.0);
    ShadowV(V * Vwing, shButterflyWing);
    queuepoly(VBIRD * Vwing, shButterflyWing, darkena(col, 0, 0xFF));
    queuepoly(VBIRD, shButterflyBody, darkena(col, 2, 0xFF));
    }
  else if(m == moGadfly) {
    transmatrix Vwing = Id;
    Vwing[1][1] = .85 + .15 * sin(ticks / 100.0);
    ShadowV(V * Vwing, shGadflyWing);
    queuepoly(VBIRD * Vwing, shGadflyWing, darkena(col, 0, 0xFF));
    queuepoly(VBIRD, shGadflyBody, darkena(col, 1, 0xFF));
    queuepoly(VBIRD, shGadflyEye, darkena(col, 2, 0xFF));
    queuepoly(VBIRD * Mirror, shGadflyEye, darkena(col, 2, 0xFF));
    }
  else if(m == moVampire || m == moBat) {
    if(m == moBat) ShadowV(V, shBatWings); // but vampires have no shadow
    queuepoly(VBIRD, shBatWings, darkena(0x303030, 0, 0xFF));
    queuepoly(VBIRD, shBatBody, darkena(0x606060, 0, 0xFF));
    /* queuepoly(V, shBatMouth, darkena(0xC00000, 0, 0xFF));
    queuepoly(V, shBatFang, darkena(0xFFC0C0, 0, 0xFF));
    queuepoly(V*Mirror, shBatFang, darkena(0xFFC0C0, 0, 0xFF));
    queuepoly(V, shBatEye, darkena(00000000, 0, 0xFF));
    queuepoly(V*Mirror, shBatEye, darkena(00000000, 0, 0xFF)); */
    }
  else if(m == moGargoyle) {
    ShadowV(V, shGargoyleWings);
    queuepoly(VBIRD, shGargoyleWings, darkena(col, 0, 0xD0));
    queuepoly(VBIRD, shGargoyleBody, darkena(col, 0, 0xFF));
    }
  else if(m == moZombie) {
    int c = darkena(col, where->land == laHalloween ? 1 : 0, 0xFF);
    otherbodyparts(V, c, m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, c);
    }
  else if(m == moDesertman) {
    otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0xC0));
    queuepoly(VBODY, shPSword, 0xFFFF00FF);
    queuepoly(VHEAD, shHood, 0xD0D000C0);
    }
  else if(m == moPalace || m == moFatGuard || m == moVizier || m == moSkeleton) {
    queuepoly(VBODY, shSabre, 0xFFFFFFFF);
    if(m == moSkeleton) {
      otherbodyparts(V, darkena(0xFFFFFF, 0, 0xFF), moSkeleton, footphase);
      queuepoly(VBODY, shSkeletonBody, darkena(0xFFFFFF, 0, 0xFF));
      queuepoly(VHEAD, shSkull, darkena(0xFFFFFF, 0, 0xFF));
      queuepoly(VHEAD, shSkullEyes, darkena(0, 0, 0xFF));
      ShadowV(V, shSkeletonBody);
      }
    else {
      ShadowV(V, shPBody);
      otherbodyparts(V, darkena(0xFFD500, 0, 0xFF), m, footphase);
      if(m == moFatGuard) {
        queuepoly(VBODY, shFatBody, darkena(0xC06000, 0, 0xFF));
        col = 0xFFFFFF;
        if(where && where->hitpoints >= 3)
          queuepoly(VBODY, shKnightCloak, darkena(0xFFC0C0, 1, 0xFF));
        }
      else {
        queuepoly(VBODY, shPBody, darkena(0xFFD500, 0, 0xFF));
        queuepoly(VBODY, shKnightArmor, m == moVizier ? 0xC000C0FF :
          darkena(0x00C000, 1, 0xFF));
        if(where && where->hitpoints >= 3)
          queuepoly(VBODY, shKnightCloak, m == moVizier ? 0x800080Ff :
            darkena(0x00FF00, 1, 0xFF));
        }
      queuepoly(VHEAD, shTurban1, darkena(col, 1, 0xFF));
      if(where && where->hitpoints >= 2)
        queuepoly(VHEAD, shTurban2, darkena(col, 0, 0xFF));
      }
      
    drawStunStars(V, where ? where->stuntime : 0);
    }
  else if(m == moCrystalSage) {
    otherbodyparts(V, 0xFFFFFFFF, m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, 0xFFFFFFFF);
    queuepoly(VHEAD, shPHead, 0xFFFFFFFF);
    queuepoly(VHEAD, shPFace, 0xFFFFFFFF);
    }
  else if(m == moHedge) {
    ShadowV(V, shPBody);
    otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0xFF));
    queuepoly(VBODY, shHedgehogBlade, 0xC0C0C0FF);
    queuepoly(VHEAD, shPHead, 0x804000FF);
    queuepoly(VHEAD, shPFace, 0xF09000FF);
    }
  else if(m == moYeti || m == moMonkey) {
    otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(col, 0, 0xFF));
    }
  else if(m == moResearcher) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, darkena(0xFFFF00, 0, 0xC0));
    queuepoly(VHEAD, shAztecHead, darkena(col, 0, 0xFF));
    queuepoly(VHEAD, shAztecCap, darkena(0xC000C0, 0, 0xFF));
    }
  else if(m == moFamiliar) {
    /* queuepoly(V, shFemaleBody, darkena(0xC0B070, 0, 0xFF));
    queuepoly(V, shPFace, darkena(0xC0B070, 0, 0XFF));
    queuepoly(V, shWizardCape1, 0x601000FF);
    queuepoly(V, shWizardCape2, 0x781800FF);
    queuepoly(V, shWizardHat1, 0x902000FF);
    queuepoly(V, shWizardHat2, 0xA82800FF); */
    
   // queuepoly(V, shCatBody, darkena(0x601000, 0, 0xFF));
   // queuepoly(V, shGargoyleBody, darkena(0x903000, 0, 0xFF));
   
   ShadowV(V, shWolfBody);
   queuepoly(VABODY, shWolfBody, darkena(0xA03000, 0, 0xFF));
    if(mmspatial || footphase)
      animallegs(VALEGS, moWolf, darkena(0xC04000, 0, 0xFF), footphase);
    else
     queuepoly(VALEGS, shWolfLegs, darkena(0xC04000, 0, 0xFF));
   
   queuepoly(VAHEAD, shFamiliarHead, darkena(0xC04000, 0, 0xFF));
   // queuepoly(V, shCatLegs, darkena(0x902000, 0, 0xFF));
   if(true) {
     queuepoly(VAHEAD, shFamiliarEye, darkena(0xFFFF000, 0, 0xFF));
     queuepoly(VAHEAD * Mirror, shFamiliarEye, darkena(0xFFFF000, 0, 0xFF));
     }
    }
  else if(m == moRanger) {
    ShadowV(V, shPBody);
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0xC0));
    queuepoly(VBODY, shPSword, darkena(col, 0, 0xFF));
    queuepoly(VHEAD, shArmor, darkena(col, 1, 0xFF));
    }
  else if(m == moGhost || m == moSeep || m == moFriendlyGhost) {
    if(m == moFriendlyGhost) col = fghostcolor(ticks, where);
    queuepoly(VGHOST, shGhost, darkena(col, 0, m == moFriendlyGhost ? 0xC0 : 0x80));
    queuepoly(VGHOST, shEyes, 0xFF);
    }
  else if(m == moVineSpirit) {
    queuepoly(VGHOST, shGhost, 0xD0D0D0C0);
    queuepoly(VGHOST, shEyes, 0xFF0000FF);
    }
  else if(m == moFireFairy) {
    col = firecolor(0);
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shFemaleBody);
    queuepoly(VBODY, shFemaleBody, darkena(col, 0, 0XC0));
    queuepoly(VHEAD, shWitchHair, darkena(col, 1, 0xFF));
    queuepoly(VHEAD, shPFace, darkena(col, 0, 0XFF));
    }
  else if(m == moSlime) {
    queuepoly(VFISH, shSlime, darkena(col, 0, 0x80));
    queuepoly(VSLIMEEYE, shEyes, 0xFF);
    }
  else if(m == moKrakenH) {
    queuepoly(VFISH, shKrakenHead, darkena(col, 0, 0xD0));
    queuepoly(VFISH, shKrakenEye, 0xFFFFFFC0);
    queuepoly(VFISH, shKrakenEye2, 0xC0);
    queuepoly(VFISH * Mirror, shKrakenEye, 0xFFFFFFC0);
    queuepoly(VFISH * Mirror, shKrakenEye2, 0xC0);
    }
  else if(m == moKrakenT) {
    queuepoly(VFISH, shSeaTentacle, darkena(col, 0, 0xD0));
    }
  else if(m == moCultist || m == moPyroCultist || m == moCultistLeader) {
    otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0xC0));
    queuepoly(VBODY, shPSword, darkena(col, 2, 0xFF));
    queuepoly(VHEAD, shHood, darkena(col, 1, 0xFF));
    }
  else if(m == moPirate) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, darkena(0x404040, 0, 0xFF));
    queuepoly(VBODY, shPirateHook, darkena(0xD0D0D0, 0, 0xFF));
    queuepoly(VHEAD, shPFace, darkena(0xFFFF80, 0, 0xFF));
    queuepoly(VHEAD, shEyepatch, darkena(0, 0, 0xC0));
    queuepoly(VHEAD, shPirateHood, darkena(col, 0, 0xFF));
    }
  else if(m == moRatling || m == moRatlingAvenger) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBODY, shYeti, darkena(col, 1, 0xFF));
    queuepoly(VHEAD, shRatHead, darkena(col, 0, 0xFF));
    queuepoly(VLEG, shRatTail, darkena(col, 0, 0xFF));

    float t = sin(ticks / 1000.0 + (where ? where->cpdist : 0));
    int eyecol = t > 0.92 ? 0xFF0000 : 0;
    
    queuepoly(VHEAD, shWolf1, darkena(eyecol, 0, 0xFF));
    queuepoly(VHEAD, shWolf2, darkena(eyecol, 0, 0xFF));
    queuepoly(VHEAD, shWolf3, darkena(0x202020, 0, 0xFF));
    
    if(m == moRatlingAvenger) {
      queuepoly(VBODY, shRatCape1, 0x303030FF);
      queuepoly(VHEAD, shRatCape2, 0x484848FF);
      }
    }
  else if(m == moViking) {
    otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, darkena(0xE00000, 0, 0xFF));
    queuepoly(VBODY, shPSword, darkena(0xD0D0D0, 0, 0xFF));
    queuepoly(VBODY, shKnightCloak, darkena(0x404040, 0, 0xFF));
    queuepoly(VHEAD, shVikingHelmet, darkena(0xC0C0C0, 0, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xFFFF80, 0, 0xFF));
    }
  else if(m == moOutlaw) {
    otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0xFF));
    queuepoly(VBODY, shKnightCloak, darkena(col, 1, 0xFF));
    queuepoly(VHEAD, shWestHat1, darkena(col, 2, 0XFF));
    queuepoly(VHEAD, shWestHat2, darkena(col, 1, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(0xFFFF80, 0, 0xFF));
    queuepoly(VBODY, shGunInHand, darkena(col, 1, 0XFF));
    }
  else if(m == moNecromancer) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, 0xC00000C0);
    queuepoly(VHEAD, shHood, darkena(col, 1, 0xFF));
    }
  else if(m == moDraugr) {
    otherbodyparts(V, 0x483828D0, m, footphase);
    queuepoly(VBODY, shPBody, 0x483828D0);
    queuepoly(VBODY, shPSword, 0xFFFFD0A0);
    queuepoly(VHEAD, shPHead, 0x483828D0);
    // queuepoly(V, shSkull, 0xC06020D0);
    //queuepoly(V, shSkullEyes, 0x000000D0);
//  queuepoly(V, shWightCloak, 0xC0A080A0);
    int b = where->cpdist;
    b--;
    if(b < 0) b = 0;
    if(b > 6) b = 6;
    queuepoly(VHEAD, shWightCloak, 0x605040A0 + 0x10101000 * b);
    }
  else if(m == moGoblin) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBODY, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shArmor, darkena(col, 1, 0XFF));
    }
  else if(m == moLancer || m == moFlailer || m == moMiner) {
    transmatrix V2 = V;
    if(m == moLancer)
      V2 = V * spin((where && where->type == 6) ? -M_PI/3 : -M_PI/2 );
    transmatrix Vh = mmscale(V2, geom3::HEAD);
    transmatrix Vb = mmscale(V2, geom3::BODY);
    otherbodyparts(V2, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V2, shPBody);
    queuepoly(Vb, shPBody, darkena(col, 0, 0xC0));
    queuepoly(Vh, m == moFlailer ? shArmor : shHood, darkena(col, 1, 0XFF));
    if(m == moMiner)
      queuepoly(Vb, shPickAxe, darkena(0xC0C0C0, 0, 0XFF));
    if(m == moLancer)
      queuepoly(Vb, shPike, darkena(col, 0, 0XFF));
    if(m == moFlailer) {
      queuepoly(Vb, shFlailBall, darkena(col, 0, 0XFF));
      queuepoly(Vb, shFlailChain, darkena(col, 1, 0XFF));
      queuepoly(Vb, shFlailTrunk, darkena(col, 0, 0XFF));
      }
    }
  else if(m == moTroll) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBODY, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(col, 1, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(col, 2, 0XFF));
    }        
  else if(m == moFjordTroll || m == moForestTroll || m == moStormTroll) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBODY, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(col, 1, 0XFF));
    queuepoly(VHEAD, shPFace, darkena(col, 2, 0XFF));
    }        
  else if(m == moDarkTroll) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBODY, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(col, 1, 0XFF));
    queuepoly(VHEAD, shPFace, 0xFFFFFF80);
    }        
  else if(m == moRedTroll) {
    otherbodyparts(V, darkena(col, 0, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBODY, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(0xFF8000, 0, 0XFF));
    queuepoly(VHEAD, shPFace, 0xFFFFFF80);
    }        
  else if(m == moEarthElemental) {
    otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    ShadowV(V, shYeti);
    queuepoly(VBODY, shYeti, darkena(col, 0, 0xC0));
    queuepoly(VHEAD, shPHead, darkena(col, 0, 0XFF));
    queuepoly(VHEAD, shPFace, 0xF0000080);
    }        
  else if(m == moWaterElemental) {
    otherbodyparts(V, watercolor(50), m, footphase);
    ShadowV(V, shWaterElemental);
    queuepoly(VBODY, shWaterElemental, watercolor(0));
    queuepoly(VHEAD, shFemaleHair, watercolor(100));
    queuepoly(VHEAD, shPFace, watercolor(200));
    }        
  else if(m == moFireElemental) {
    otherbodyparts(V, darkena(firecolor(50), 0, 0xFF), m, footphase);
    ShadowV(V, shWaterElemental);
    queuepoly(VBODY, shWaterElemental, darkena(firecolor(0), 0, 0xFF));
    queuepoly(VHEAD, shFemaleHair, darkena(firecolor(100), 0, 0xFF));
    queuepoly(VHEAD, shPFace, darkena(firecolor(200), 0, 0xFF));
    }
  else if(m == moAirElemental) {
    otherbodyparts(V, darkena(col, 0, 0x40), m, footphase);
    ShadowV(V, shWaterElemental);
    queuepoly(VBODY, shWaterElemental, darkena(col, 0, 0x80));
    queuepoly(VHEAD, shFemaleHair, darkena(col, 0, 0x80));
    queuepoly(VHEAD, shPFace, darkena(col, 0, 0x80));
    }        
  else if(xch == 'd' || xch == 'D') {
    otherbodyparts(V, darkena(col, 0, 0xC0), m, footphase);
    queuepoly(VBODY, shPBody, darkena(col, 1, 0xC0));
    ShadowV(V, shPBody);
    int acol = col;
    if(xch == 'D') acol = 0xD0D0D0;
    queuepoly(VHEAD, shDemon, darkena(acol, 0, 0xFF));
    }
  else if(isMetalBeast(m)) {
    ShadowV(V, shTrylobite);
    if(!mmspatial)
      queuepoly(VABODY, shTrylobite, darkena(col, 1, 0xC0));
    else {
      queuepoly(VABODY, shTrylobiteBody, darkena(col, 1, 0xFF));
      animallegs(VALEGS, moMetalBeast, darkena(col, 1, 0xFF), footphase);
      }
    int acol = col;
    queuepoly(VAHEAD, shTrylobiteHead, darkena(acol, 0, 0xFF));
    }
  else if(m == moEvilGolem) {
    otherbodyparts(V, darkena(col, 2, 0xC0), m, footphase);
    ShadowV(V, shPBody);
    queuepoly(VBODY, shPBody, darkena(col, 0, 0XC0));
    queuepoly(VHEAD, shGolemhead, darkena(col, 1, 0XFF));
    }
  else if(isWitch(m)) {
    otherbodyparts(V, darkena(col, 1, 0xFF), m, footphase);
    int c = 0xFF;
    if(m == moWitchGhost) c = 0x85 + 120 * sin(ticks / 160.0);
    if(m == moWitchWinter) drawWinter(V, 0);
    if(m == moWitchFlash) drawFlash(V);
    if(m == moWitchSpeed) drawSpeed(V);
    if(m == moWitchFire) col = firecolor(0);
    ShadowV(V, shFemaleBody);
    queuepoly(VBODY, shFemaleBody, darkena(col, 0, c));
//    queuepoly(cV2, ct, shPSword, darkena(col, 0, 0XFF));
//    queuepoly(V, shHood, darkena(col, 0, 0XC0));
    if(m == moWitchFire) col = firecolor(100);
    queuepoly(VHEAD, shWitchHair, darkena(col, 1, c));
    if(m == moWitchFire) col = firecolor(200);
    queuepoly(VHEAD, shPFace, darkena(col, 0, c));
    if(m == moWitchFire) col = firecolor(300);
    queuepoly(VBODY, shWitchDress, darkena(col, 1, 0XC0));
    }

  else return true;
  
  return false;
  }

bool drawMonsterTypeDH(eMonster m, cell *where, const transmatrix& V, int col, bool dh, ld footphase) {
  if(dh) {
    poly_outline = OUTLINE_DEAD;
    darken++;
    }
  bool b = drawMonsterType(m,where,V,col, footphase);
  if(dh) {
    poly_outline = OUTLINE_NONE;
    darken--;
    }
  return b;
  }

transmatrix playerV;

bool applyAnimation(cell *c, transmatrix& V, double& footphase, int layer) {
  if(!animations[layer].count(c)) return false;
  animation& a = animations[layer][c];

  int td = ticks - a.ltick;
  ld aspd = td / 1000.0 * exp(vid.mspeed);
  ld R = hdist0(tC0(a.wherenow));
  aspd *= (1+R+(shmup::on?1:0));
  
  if(R < aspd || isnan(R) || isnan(aspd) || R > 10) {
    animations[layer].erase(c);
    return false;
    }
  else {
    a.wherenow = a.wherenow * rspintox(tC0(inverse(a.wherenow)));
    a.wherenow = a.wherenow * xpush(aspd);
    fixmatrix(a.wherenow);
    a.footphase += aspd;
    footphase = a.footphase;
    V = V * a.wherenow;
    a.ltick = ticks;
    return true;
    }
  }

double chainAngle(cell *c, transmatrix& V, cell *c2, double dft) {
  if(!gmatrix0.count(c2)) return dft;
  hyperpoint h = C0;
  if(animations[LAYER_BIG].count(c2)) h = animations[LAYER_BIG][c2].wherenow * h;
  h = inverse(V) * gmatrix0[c2] * h;
  return atan2(h[1], h[0]);
  }

// equivalent to V = V * spin(-chainAngle(c,V,c2,dft));
bool chainAnimation(cell *c, transmatrix& V, cell *c2, int i, int b) {
  if(!gmatrix0.count(c2)) {
    V = V * ddspin(c,i,b);
    return false;
    }
  hyperpoint h = C0;
  if(animations[LAYER_BIG].count(c2)) h = animations[LAYER_BIG][c2].wherenow * h;
  h = inverse(V) * gmatrix0[c2] * h;
  V = V * rspintox(h);
  return true;  
  }

// push down the queue after q-th element, `down` absolute units down,
// based on cell c and transmatrix V
// do change the zoom factor? do change the priorities?

int cellcolor(cell *c) {
  if(isPlayerOn(c) || isFriendly(c)) return OUTLINE_FRIEND;
  if(noHighlight(c->monst)) return OUTLINE_NONE;
  if(c->monst) return OUTLINE_ENEMY;
  
  if(c->wall == waMirror) return c->land == laMirror ? OUTLINE_TREASURE : OUTLINE_ORB;

  if(c->item) {
    int k = itemclass(c->item);
    if(k == IC_TREASURE)
      return OUTLINE_TREASURE;
    else if(k == IC_ORB)
      return OUTLINE_ORB;
    else
      return OUTLINE_OTHER;
    }

  return OUTLINE_NONE;
  } 

bool drawMonster(const transmatrix& Vparam, int ct, cell *c, int col) {

  bool darkhistory = conformal::includeHistory && eq(c->aitmp, sval);
  
  if(doHighlight())
    poly_outline = 
      (isPlayerOn(c) || isFriendly(c)) ? OUTLINE_FRIEND : 
      noHighlight(c->monst) ? OUTLINE_NONE :
      OUTLINE_ENEMY;
    
  bool nospins = false, nospinb = false;
  double footphaseb = 0, footphase = 0;
  
  transmatrix Vs = Vparam; nospins = applyAnimation(c, Vs, footphase, LAYER_SMALL);
  transmatrix Vb = Vparam; nospinb = applyAnimation(c, Vb, footphaseb, LAYER_BIG);
//  nospin = true;

  eMonster m = c->monst;
    
  if(isIvy(c) || isWorm(c) || isMutantIvy(c) || c->monst == moFriendlyIvy) {
    
    if(isDragon(c->monst) && c->stuntime == 0) col = 0xFF6000;
    
    transmatrix Vb0 = Vb;
    if(c->mondir != NODIR) {
      
      if(mmmon) {
        if(nospinb)
          chainAnimation(c, Vb, c->mov[c->mondir], c->mondir, 0);
        else 
          Vb = Vb * ddspin(c, c->mondir);

#ifndef NOEDIT
        if(c == mapeditor::drawcell) mapeditor::drawtrans = Vb;
#endif

        if(drawUserShape(Vb, 1, c->monst, (col << 8) + 0xFF)) return false;

        if(isIvy(c) || isMutantIvy(c) || c->monst == moFriendlyIvy)
          queuepoly(Vb, shIBranch, (col << 8) + 0xFF);
        else if(c->monst < moTentacle) {
          ShadowV(Vb, shTentacleX, PPR_GIANTSHADOW);
          queuepoly(mmscale(Vb, geom3::ABODY), shTentacleX, 0xFF);
          queuepoly(mmscale(Vb, geom3::ABODY), shTentacle, (col << 8) + 0xFF);
          }
        else if(c->monst == moDragonHead || c->monst == moDragonTail) {
          char part = dragon::bodypart(c, dragon::findhead(c));
          if(part != '2') {
            queuepoly(mmscale(Vb, geom3::ABODY), shDragonSegment, darkena(col, 0, 0xFF));
            ShadowV(Vb, shDragonSegment, PPR_GIANTSHADOW);
            }
          }
        else {
          if(c->monst == moTentacleGhost) {
            hyperpoint V0 = conformal::on ? tC0(Vs) : inverse(cwtV) * tC0(Vs);
            hyperpoint V1 = spintox(V0) * V0;
            Vs = cwtV * rspintox(V0) * rpushxto0(V1) * pispin;
            drawMonsterType(moGhost, c, Vs, darkena(col, 0, 0xFF), footphase);
            col = minf[moTentacletail].color;
            }
          queuepoly(mmscale(Vb, geom3::ABODY), shTentacleX, 0xFFFFFFFF);
          queuepoly(mmscale(Vb, geom3::ABODY), shTentacle, (col << 8) + 0xFF);
          ShadowV(Vb, shTentacleX, PPR_GIANTSHADOW);
          }
        }
        
      else {
        int hdir = displaydir(c, c->mondir);
        int col = darkena(0x606020, 0, 0xFF);
        for(int u=-1; u<=1; u++)
          queueline(Vparam*ddi0(hdir+S21, u*crossf/5), Vparam*ddi(hdir, crossf)*ddi0(hdir+S21, u*crossf/5), col, 2);
        }
      }

    if(mmmon) {
      if(isIvy(c) || isMutantIvy(c) || c->monst == moFriendlyIvy) {
        queuepoly(mmscale(Vb, geom3::ABODY), shILeaf[K(c)], darkena(col, 0, 0xFF));
        ShadowV(Vb, shILeaf[K(c)], PPR_GIANTSHADOW);
        }
      else if(m == moWorm || m == moWormwait || m == moHexSnake) {
        Vb = Vb * pispin;
        transmatrix Vbh = mmscale(Vb, geom3::AHEAD);
        queuepoly(Vbh, shWormHead, darkena(col, 0, 0xFF));
        queuepolyat(Vbh, shEyes, 0xFF, PPR_ONTENTACLE_EYES);
        ShadowV(Vb, shWormHead, PPR_GIANTSHADOW);
        }
      else if(m == moDragonHead) {
        transmatrix Vbh = mmscale(Vb, geom3::AHEAD);
        ShadowV(Vb, shDragonHead, PPR_GIANTSHADOW);
        queuepoly(Vbh, shDragonHead, darkena(col, c->hitpoints?0:1, 0xFF));
        queuepolyat(Vbh/* * pispin */, shEyes, 0xFF, PPR_ONTENTACLE_EYES);
        
        int noscolor = (c->hitpoints == 1 && c->stuntime ==1) ? 0xFF0000FF : 0xFF;
        queuepoly(Vbh, shDragonNostril, noscolor);
        queuepoly(Vbh * Mirror, shDragonNostril, noscolor);
        }
      else if(m == moTentacle || m == moTentaclewait || m == moTentacleEscaping) {
        Vb = Vb * pispin;
        transmatrix Vbh = mmscale(Vb, geom3::AHEAD);
        queuepoly(Vbh, shTentHead, darkena(col, 0, 0xFF));
        ShadowV(Vb, shTentHead, PPR_GIANTSHADOW);
        }
      else if(m == moDragonTail) {
        cell *c2 = NULL;
        for(int i=0; i<c->type; i++)
          if(c->mov[i] && isDragon(c->mov[i]->monst) && c->mov[i]->mondir == c->spn(i))
            c2 = c->mov[i];
        int nd = neighborId(c, c2);
        char part = dragon::bodypart(c, dragon::findhead(c));
        if(part == 't') {
          if(nospinb) {
            chainAnimation(c, Vb, c2, nd, 0);
            Vb = Vb * pispin;
            }
          else {
            Vb = Vb0 * ddspin(c, nd, S42);
            }
          transmatrix Vbb = mmscale(Vb, geom3::ABODY);
          queuepoly(Vbb, shDragonTail, darkena(col, c->hitpoints?0:1, 0xFF));
          ShadowV(Vb, shDragonTail, PPR_GIANTSHADOW);
          }
        else if(true) {
          if(nospinb) {
            chainAnimation(c, Vb, c2, nd, 0);
            Vb = Vb * pispin;
            double ang = chainAngle(c, Vb, c->mov[c->mondir], (displaydir(c, c->mondir) - displaydir(c, nd)) * M_PI / S42);
            ang /= 2;
            Vb = Vb * spin(M_PI-ang);
            }
          else {
            int hdir0 = displaydir(c, nd) + S42;
            int hdir1 = displaydir(c, c->mondir);
            while(hdir1 > hdir0 + S42) hdir1 -= S84;
            while(hdir1 < hdir0 - S42) hdir1 += S84;
            Vb = Vb0 * spin((hdir0 + hdir1)/2 * M_PI / S42 + M_PI);
            }
          transmatrix Vbb = mmscale(Vb, geom3::ABODY);
          if(part == 'l' || part == '2') {
            queuepoly(Vbb, shDragonLegs, darkena(col, c->hitpoints?0:1, 0xFF));
            }
          queuepoly(Vbb, shDragonWings, darkena(col, c->hitpoints?0:1, 0xFF));
          }
        }
      else if(!(c->mondir == NODIR && (c->monst == moTentacletail || (c->monst == moWormtail && wormpos(c) < WORMLENGTH))))
        queuepoly(Vb, shJoint, darkena(col, 0, 0xFF));
      }

    if(!mmmon) return true;
    }
  
  else if(isMimic(c)) {
  
    if(!nospins) 
      Vs = Vs * ddspin(c, c->mondir, flipplayer ? S42 : 0);

    if(c->monst == moMirror) Vs = Vs * Mirror;
        
    multi::cpid = c->hitpoints;

    if(mmmon) {
      drawMonsterType(c->monst, c, Vs, col, footphase);
      drawPlayerEffects(Vs, c, false);
      }

    if(flipplayer) Vs = Vs * pispin;

    if(!outofmap(mouseh) && !nospins) {
      // transmatrix invxy = Id; invxy[0][0] = invxy[1][1] = -1;
      
      hyperpoint P2 = Vs * inverse(cwtV) * mouseh;
      queuechr(P2, 10, 'x', 0xFF00);
      }
    
    return !mmmon;
    }
  
  else if(c->monst && !mmmon) return true;
  
  // illusions face randomly
  
  else if(c->monst == moIllusion) {
    multi::cpid = 0;
    drawMonsterType(c->monst, c, Vs, col, footphase);
    drawPlayerEffects(Vs, c, false);
    }

  // wolves face the heat
  
  else if(c->monst == moWolf && c->cpdist > 1) {
    if(!nospins) {
      int d = 0;
      double bheat = -999;
      for(int i=0; i<c->type; i++) if(c->mov[i] && HEAT(c->mov[i]) > bheat) {
        bheat = HEAT(c->mov[i]);
        d = i;
        }
      Vs = Vs * ddspin(c, d);
      }
    return drawMonsterTypeDH(m, c, Vs, col, darkhistory, footphase);
    }

  // golems, knights, and hyperbugs don't face the player (mondir-controlled)
  // also whatever in the lineview mode

  else if(isFriendly(c) || isBug(c) || (c->monst && conformal::on) || c->monst == moKrakenH || (isBull(c->monst) && c->mondir != NODIR) || c->monst == moButterfly) {
    if(c->monst == moKrakenH) Vs = Vb, nospins = nospinb;
    if(!nospins) Vs = Vs * ddspin(c, c->mondir, S42);
    if(isFriendly(c)) drawPlayerEffects(Vs, c, false);
    return drawMonsterTypeDH(m, c, Vs, col, darkhistory, footphase);
    }

  else if(c->monst == moKrakenT) {
    if(c->hitpoints == 0) col = 0x404040;
    if(nospinb) {
      chainAnimation(c, Vb, c->mov[c->mondir], c->mondir, 0);
      Vb = Vb * pispin;
      }
    else Vb = Vb * ddspin(c, c->mondir, S42);
    if(c->type != 6) Vb = Vb * xpush(hexhexdist - hcrossf);
    return drawMonsterTypeDH(m, c, Vb, col, darkhistory, footphase);
    }

  else if(c->monst) {
    // other monsters face the player
    
    if(!nospins) {
      hyperpoint V0 = inverse(cwtV) * tC0(Vs);
      hyperpoint V1 = spintox(V0) * V0;

      Vs = cwtV * rspintox(V0) * rpushxto0(V1) * pispin;
      }
    
    if(c->monst == moShadow) 
      multi::cpid = c->hitpoints;
    
    return drawMonsterTypeDH(m, c, Vs, col, darkhistory, footphase);
    }
  
  for(int i=0; i<numplayers(); i++) if(c == playerpos(i) && !shmup::on && mapeditor::drawplayer) {
    if(!nospins) {
      Vs = playerV;
      if(multi::players > 1 ? multi::flipped[i] : flipplayer) Vs = Vs * pispin;
      }
    shmup::cpid = i;

    drawPlayerEffects(Vs, c, true);
    if(!mmmon) return true;
    
    if(isWorm(m)) {
      ld depth = geom3::factor_to_lev(wormhead(c) == c ? geom3::AHEAD : geom3::ABODY);
      footphase = 0;
      int q = ptds.size();
      drawMonsterType(moPlayer, c, Vs, col, footphase);
      pushdown(c, q, Vs, -depth, true, false);
      }
    
    else if(mmmon)
      drawMonsterType(moPlayer, c, Vs, col, footphase);
    }

  return false;
  }

bool showPirateX;
cell *keycell, *pirateTreasureSeek, *pirateTreasureFound;
hyperpoint pirateCoords;

double downspin;
cell *straightDownSeek;

int keycelldist;

void drawCircle(int x, int y, int size, int color) {
  if(size < 0) size = -size;
  #ifdef GL
  if(vid.usingGL) {
    qglcoords = 0;
    glcolor2(color);
    x -= vid.xcenter; y -= vid.ycenter;
    int pts = size * 4;
    if(pts > 1500) pts = 1500;
    if(ISMOBILE && pts > 72) pts = 72;
    for(int r=0; r<pts; r++) {
      float rr = (M_PI * 2 * r) / pts;
      glcoords[r][0] = x + size * sin(rr);
      glcoords[r][1] = y + size * cos(rr);
      glcoords[r][2] = vid.scrdist;
      }

    qglcoords = pts; 
    activateGlcoords();
    glDrawArrays(GL_LINE_LOOP, 0, pts);
    return;
    }
  #endif

#ifdef MOBILE
  gdpush(4); gdpush(color); gdpush(x); gdpush(y); gdpush(size);
#else
#ifdef GFX
  (vid.usingAA?aacircleColor:circleColor) (s, x, y, size, color);
#else
  int pts = size * 4;
  if(pts > 1500) pts = 1500;
  for(int r=0; r<pts; r++)
    qpixel(s, x + int(size * sin(r)), y + int(size * cos(r))) = color;
#endif
#endif
  }

int fnt[100][7];

bool bugsNearby(cell *c, int dist = 2) {
  if(!(havewhat&HF_BUG)) return false;
  if(isBug(c)) return true;
  if(dist) for(int t=0; t<c->type; t++) if(c->mov[t] && bugsNearby(c->mov[t], dist-1)) return true;
  return false;
  }

int minecolors[8] = {
  0xFFFFFF, 0xF0, 0xF060, 0xF00000, 
  0x60, 0x600000, 0x00C0C0, 0x000000
  };

int distcolors[8] = {
  0xFFFFFF, 0xF0, 0xF060, 0xF00000, 
  0xA0A000, 0xA000A0, 0x00A0A0, 0xFFD500
  };

const char* minetexts[8] = {
  "No mines next to you.",
  "A mine is next to you!",
  "Two mines next to you!",
  "Three mines next to you!",
  "Four mines next to you!",
  "Five mines next to you!",
  "Six mines next to you!",
  "Seven mines next to you!"
  };

int countMinesAround(cell *c) {
  int mines = 0;
  for(int i=0; i<c->type; i++)
    if(c->mov[i] && c->mov[i]->wall == waMineMine)
      mines++;
  return mines;
  }

transmatrix applyPatterndir(cell *c, char patt = mapeditor::whichPattern) {
  transmatrix V = ddspin(c, mapeditor::patterndir(c, patt), S42);
  
  if(mapeditor::reflectPatternAt(c, patt)) 
    return V * Mirror;
  
  return V;
  }

transmatrix applyDowndir(cell *c, cellfunction *cf) {
  return ddspin(c, mapeditor::downdir(c, cf), S42);
  }

void drawTowerFloor(const transmatrix& V, cell *c, int col, cellfunction *cf = coastvalEdge) {
  int j = -1;

  if(euclid) j = 10;
  else if((*cf)(c) > 1) { 
    int i = towerval(c, cf);
    if(i == 4) j = 0;
    if(i == 5) j = 1;
    if(i == 6) j = 2;
    if(i == 8) j = 3;
    if(i == 9) j = 4;
    if(i == 10) j = 5;
    if(i == 13) j = 6;
    if(purehepta) {
      if(i == 7) j = 7;
      if(i == 11) j = 8;
      if(i == 15) j = 9;
      }
    }

  if(j >= 0)
    qfloor(c, V, applyDowndir(c, cf), shTower[j], col);
  else if(c->wall != waLadder)
    qfloor(c, V, shMFloor[K(c)], col);
  }

void drawZebraFloor(const transmatrix& V, cell *c, int col) {

  if(euclid) { qfloor(c, V, shTower[10], col); return; }
  
  int i = zebra40(c);
  i &= ~3;
  
  int j;

  if(purehepta) j = 4;
  else if(i >=4 && i < 16) j = 2;
  else if(i >= 16 && i < 28) j = 1;
  else if(i >= 28 && i < 40) j = 3;
  else j = 0;

  qfloor(c, V, applyPatterndir(c, 'z'), shZebra[j], col);
  }

void qplainfloor(cell *c, bool warp, const transmatrix &V, int col);

void drawReptileFloor(const transmatrix& V, cell *c, int col, bool usefloor) {

  int i = zebra40(c);
  i &= ~3;
  
  int j;

  if(!wmescher) j = 4;
  else if(purehepta) j = 0;
  else if(i < 4) j = 0;
  else if(i >=4 && i < 16) j = 1;
  else if(i >= 16 && i < 28) j = 2;
  else if(i >= 28 && i < 40) j = 3;
  else j = 4;

  transmatrix V2 = V * applyPatterndir(c, 'z');
  
  if(wmescher) {
    if(usefloor)
      qfloor(c, V, applyPatterndir(c, 'z'), shReptile[j][0], darkena(col, 0, 0xFF));
    else
      queuepoly(V2, shReptile[j][0], darkena(col, 0, 0xFF));
    }
  else 
    qplainfloor(c, isWarped(c), V, darkena(col, 0, 0xFF));

  if(usefloor && chasmg == 2) return;

  int dcol = 0;

  int ecol = -1;

  if(isReptile(c->wall)) {
    unsigned char wp = c->wparam;
    if(wp == 1)        
      ecol = 0xFFFF00;
    else if(wp <= 5)
      ecol = 0xFF0000;
    else
      ecol = 0;
    if(ecol) ecol = gradient(0, ecol, -1, sin(M_PI / 100 * ticks), 1);
    }
  
  if(ecol == -1 || ecol == 0) dcol = darkena(col, 1, 0xFF);
  else dcol = darkena(ecol, 0, 0x80);

  dynamicval<int> p(poly_outline, 
    doHighlight() && ecol != -1 && ecol != 0 ? OUTLINE_ENEMY : OUTLINE_NONE);

  if(!chasmg) {
    if(wmescher)
      queuepoly(V2, shReptile[j][1], dcol);
    else
      queuepoly(V2, shMFloor[c->type!=6], dcol);
    }
  
  if(ecol != -1) {
    queuepoly(V2, shReptile[j][2], (ecol << 8) + 0xFF);
    queuepoly(V2, shReptile[j][3], (ecol << 8) + 0xFF);
    }
  }

#define ECT (euclid?2:ct6)

void drawEmeraldFloor(const transmatrix& V, cell *c, int col) {
  int j = -1;
  
  if(!euclid && !purehepta) {
    int i = emeraldval(c) & ~3;
    if(i == 8) j = 0;
    else if(i == 12) j = 1;
    else if(i == 16) j = 2;
    else if(i == 20) j = 3;
    else if(i == 28) j = 4;
    else if(i == 36) j = 5;
    }

  if(j >= 0)
    qfloor(c, V, applyPatterndir(c, 'f'), shEmeraldFloor[j], col);
  else
    qfloor(c, V, shCaveFloor[euclid?2:K(c)], col);
  }

double fanframe;

void viewBuggyCells(cell *c, transmatrix V) {
  for(int i=0; i<size(buggycells); i++)
    if(c == buggycells[i]) {
      queuepoly(V, shPirateX, 0xC000C080);
      return;
      }

  for(int i=0; i<size(buggycells); i++) {
    cell *c1 = buggycells[i];
    cell *cf = cwt.c;
    
    while(cf != c1) {
      cf = pathTowards(cf, c1);
      if(cf == c) {
        queuepoly(V, shMineMark[1], 0xC000C0D0);
        return;
        }
      }
    }
  }

transmatrix pushone() { return euclid ? eupush(1, 0) : xpush(sphere?.5 : 1); }

void drawMovementArrows(cell *c, transmatrix V) {

  if(viewdists) return;

  for(int d=0; d<8; d++) {
  
    movedir md = vectodir(spin(-d * M_PI/4) * tC0(pushone()));
    int u = md.d;
    cellwalker xc = cwt; cwspin(xc, u); cwstep(xc);
    if(xc.c == c) {
      transmatrix fixrot = rgpushxto0(tC0(V));
      // make it more transparent
      int col = getcs().uicolor;
      col -= (col & 0xFF) >> 1;
      poly_outline = OUTLINE_NONE;
      queuepoly(fixrot * spin(-d * M_PI/4 + (sphere && vid.alpha>1?M_PI:0))/* * eupush(1,0)*/, shArrow, col);

      if(c->type != 6 && (isStunnable(c->monst) || c->wall == waThumperOn)) {
        transmatrix Centered = rgpushxto0(tC0(cwtV));
        int sd = md.subdir;
        if(sphere) sd = -sd;
        queuepoly(inverse(Centered) * rgpushxto0(Centered * tC0(V)) * rspintox(Centered*tC0(V)) * spin(-sd * M_PI/S7) * xpush(0.2), shArrow, col);
        }
      else break;
      }
    }
  }

transmatrix screenpos(ld x, ld y) {
  transmatrix V = Id;
  V[0][2] += (x - vid.xcenter) / vid.radius * (1+vid.alphax);
  V[1][2] += (y - vid.ycenter) / vid.radius * (1+vid.alphax);
  // V[2][0] -= (x - vid.xcenter) / vid.radius * (1+vid.alphax);
  // V[2][1] -= (y - vid.ycenter) / vid.radius * (1+vid.alphax);
  return V;
  }

#define SKIPFAC .4

void drawMobileArrow(cell *c, transmatrix V) {

  // int col = getcs().uicolor;
  // col -= (col & 0xFF) >> 1;
  
  int dir = neighborId(cwt.c, c);
  bool invalid = !legalmoves[dir];
  
  int col = cellcolor(c);
  if(col == OUTLINE_NONE) col = 0xC0C0C0FF;
  col -= (col & 0xFF) >> 1;
  if(invalid) col -= (col & 0xFF) >> 1;
  if(invalid) col -= (col & 0xFF) >> 1;
  
  poly_outline = OUTLINE_NONE;
  transmatrix m2 = Id;
  ld scale = vid.mobilecompasssize / 15.;
  m2[0][0] = scale; m2[1][1] = scale; m2[2][2] = 1;

  transmatrix Centered = rgpushxto0(tC0(cwtV));
  transmatrix t = inverse(Centered) * V;
  double alpha = atan2(tC0(t)[1], tC0(t)[0]);
  
  using namespace shmupballs;
  
  double dx = xmove + rad*(1+SKIPFAC-.2)/2 * cos(alpha);
  double dy = yb + rad*(1+SKIPFAC-.2)/2 * sin(alpha);
  
  queuepoly(
    screenpos(dx, dy) * spin(-alpha) * m2, shArrow, col);
  /*
      if(c->type != 6 && (isStunnable(c->monst) || c->wall == waThumperOn)) {
        transmatrix Centered = rgpushxto0(tC0(cwtV));
        int sd = md.subdir;
        if(sphere) sd = -sd;
        queuepoly(inverse(Centered) * rgpushxto0(Centered * tC0(V)) * rspintox(Centered*tC0(V)) * spin(-sd * M_PI/S7) * xpush(0.2), shArrow, col);
        }
      else break;
      } */
  }

int celldistAltPlus(cell *c) { return 1000000 + celldistAlt(c); }

bool drawstaratvec(double dx, double dy) {
  return dx*dx+dy*dy > .05;
  }

int reptilecolor(cell *c) {
  int i = zebra40(c);
  
  if(!euclid) {
    if(i >= 4 && i < 16) i = 0; 
    else if(i >= 16 && i < 28) i = 1;
    else if(i >= 28 && i < 40) i = 2;
    else i = 3;
    }

  int fcoltab[4] = {0xe3bb97, 0xc2d1b0, 0xebe5cb, 0xA0A0A0};
  return fcoltab[i];
  }

ld wavefun(ld x) { 
  return sin(x);
  /* x /= (2*M_PI);
  x -= (int) x;
  if(x > .5) return (x-.5) * 2;
  else return 0; */
  }

void setcolors(cell *c, int& wcol, int &fcol) {

  wcol = fcol = winf[c->wall].color;

  // floor colors for all the lands
  if(c->land == laKraken) fcol = 0x20A020;
  if(c->land == laBurial) fcol = linf[laBurial].color;    
  if(c->land == laTrollheim) fcol = linf[c->land].color;
  
  if(c->land == laBarrier) fcol = linf[c->land].color;
  if(c->land == laOceanWall) fcol = linf[c->land].color;

  if(c->land == laAlchemist) {
    fcol = 0x202020;
    if(c->item && !(conformal::includeHistory && eq(c->aitmp, sval)))
      fcol = wcol = iinf[c->item].color;
    }
  
  if(c->land == laBull)
    fcol = 0x800080;
  
  if(c->land == laCA)
    fcol = 0x404040;
  
  if(c->land == laReptile) {
    fcol = reptilecolor(c);
    }
  
  if(c->land == laCrossroads) fcol = (vid.goteyes2 ? 0xFF3030 : 0xFF0000);
  if(c->land == laCrossroads2) fcol = linf[laCrossroads2].color;
  if(c->land == laCrossroads3) fcol = linf[laCrossroads3].color;
  if(c->land == laCrossroads4) fcol = linf[laCrossroads4].color;
  if(c->land == laCrossroads5) fcol = linf[laCrossroads5].color;
  if(isElemental(c->land)) fcol = linf[c->land].color;    
  if(c->land == laDesert) fcol = 0xEDC9AF;
  if(c->land == laCaves) fcol = 0x202020;
  if(c->land == laEmerald) fcol = 0x202020;
  if(c->land == laDeadCaves) fcol = 0x202020;
  if(c->land == laJungle)  fcol = (vid.goteyes2 ? 0x408040 : 0x008000);
  if(c->land == laMountain) {
    if(euclid || c->master->alt)
      fcol = celldistAlt(c) & 1 ? 0x604020 : 0x302010;
    else fcol = 0;
    if(c->wall == waPlatform) wcol = 0xF0F0A0;
    }
  if(c->land == laWineyard) fcol = 0x006000;
  if(c->land == laMirror) fcol = 0x808080;
  if(c->land == laMotion) fcol = 0xF0F000;
  if(c->land == laGraveyard) fcol = 0x107010;
  if(c->land == laDryForest) fcol = gradient(0x008000, 0x800000, 0, c->landparam, 10);
  if(c->land == laRlyeh) fcol = (vid.goteyes2 ? 0x4080C0 : 0x004080);
  if(c->land == laPower) fcol = linf[c->land].color;
  if(c->land == laHell) fcol = (vid.goteyes2 ? 0xC03030 : 0xC00000);
  if(c->land == laLivefjord) fcol = 0x306030;    
  if(c->land == laWildWest) fcol = linf[c->land].color;
  if(c->land == laHalloween) fcol = linf[c->land].color;
  if(c->land == laMinefield) fcol = 0x80A080;    
  if(c->land == laCaribbean) fcol = 0x006000;
  if(c->land == laRose) fcol = linf[c->land].color;
  if(c->land == laCanvas) fcol = c->landparam;
  if(c->land == laRedRock) fcol = linf[c->land].color;
  if(c->land == laDragon) fcol = linf[c->land].color;
  if(c->land == laStorms) fcol = linf[c->land].color;

  if(c->land == laPalace) {
    fcol = 0x806020;
    if(c->wall == waClosedGate || c->wall == waOpenGate)
      fcol = wcol;
    }
  
  if(c->land == laElementalWall) 
    fcol = (linf[c->barleft].color>>1) + (linf[c->barright].color>>1);
    
  if(c->land == laZebra) {
    fcol = 0xE0E0E0;
    if(c->wall == waTrapdoor) fcol = 0x808080;
    }
    
  if(c->land == laCaribbean && (c->wall == waCIsland || c->wall == waCIsland2))
    fcol = wcol = winf[c->wall].color;

  if(isHive(c->land)) {
    fcol = linf[c->land].color;
    if(c->wall == waWaxWall) wcol = c->landparam;
    if(items[itOrbInvis] && c->wall == waNone && c->landparam)
      fcol = gradient(fcol, 0xFF0000, 0, c->landparam, 100);
    if(c->bardir == NOBARRIERS && c->barleft) 
      fcol = minf[moBug0+c->barright].color;
    }

  if(isWarped(c->land)) {
    fcol = pseudohept(c) ? 0x80C080 : 0xA06020;
    if(c->wall == waSmallTree) wcol = 0x608000;
    }  

  if(c->land == laTortoise) {
    fcol = tortoise::getMatchColor(getBits(c));
    if(c->wall == waBigTree) wcol = 0x709000;
    else if(c->wall == waSmallTree) wcol = 0x905000;
    }

  if(c->land == laOvergrown || c->land == laClearing) {
    fcol = (c->land == laOvergrown/* || (celldistAlt(c)&1)*/) ? 0x00C020 : 0x60E080;
    if(c->wall == waSmallTree) wcol = 0x008060;
    else if(c->wall == waBigTree) wcol = 0x0080C0;
    }

  if(c->land == laTemple) {
    int d = showoff ? 0 : (euclid||c->master->alt) ? celldistAlt(c) : 99;
    if(chaosmode)
      fcol = 0x405090;
    else if(d % TEMPLE_EACH == 0)
      fcol = gradient(0x304080, winf[waColumn].color, 0, 0.5, 1);
//    else if(c->type == 7)
//      wcol = 0x707070;
    else if(d% 2 == -1)
      fcol = 0x304080;
    else
      fcol = 0x405090;
    }

  if(isHaunted(c->land)) {
    int itcolor = 0;
    for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->item)
      itcolor = 1;
    if(c->item) itcolor |= 2;
    fcol = 0x609F60 + 0x202020 * itcolor;

    forCellEx(c2, c) if(c2->monst == moFriendlyGhost)
      fcol = gradient(fcol, fghostcolor(ticks, c2), 0, .25, 1);

    if(c->monst == moFriendlyGhost) 
      fcol = gradient(fcol, fghostcolor(ticks, c), 0, .5, 1);    

    if(c->wall == waSmallTree) wcol = 0x004000;
    else if(c->wall == waBigTree) wcol = 0x008000;
    }

  if(c->land == laCamelot) {
    int d = showoff ? 0 : ((euclid||c->master->alt) ? celldistAltRelative(c) : 0);
#ifdef TOUR
    if(!tour::on) camelotcheat = false;
    if(camelotcheat) 
        fcol = (d&1) ? 0xC0C0C0 : 0x606060;
    else 
#endif
    if(d < 0) {
      fcol = 0xA0A0A0;
      }
    else {
      // a nice floor pattern
      int v = emeraldval(c);
      int v0 = (v&~3);
      bool sw = (v&1);
      if(v0 == 8 || v0 == 12 || v0 == 20 || v0 == 40 || v0 == 36 || v0 == 24)
        sw = !sw;
      if(sw)
        fcol = 0xC0C0C0;
      else
        fcol = 0xA0A0A0;
      }
    }

  if(c->land == laPrairie) {
    /* if(isWateryOrBoat(c)) {
      if(prairie::isriver(c))
        fcol = ((c->LHU.fi.rval & 1) ? 0x000090 : 0x0000E0)
          + int(16 * wavefun(ticks / 200. + (c->wparam)*1.5))
          + ((prairie::next(c) ? 0 : 0xC00000));
      else
        fcol = 0x000080;
      } */
  
    if(prairie::isriver(c)) {
      fcol = ((c->LHU.fi.rval & 1) ? 0x402000: 0x503000);
      }
    else {
      fcol = 0x004000 + 0x001000 * c->LHU.fi.walldist;
      fcol += 0x10000 * (255 - 511 / (1 + max((int) c->LHU.fi.flowerdist, 1)));
      // fcol += 0x1 * (511 / (1 + max((int) c->LHU.fi.walldist2, 1)));
      }
    }
  
  else if(isIcyLand(c) && isIcyWall(c)) {
    float h = HEAT(c);
    bool showcoc = c->land == laCocytus && chaosmode && !wmescher;
    if(h < -0.4)
      wcol = gradient(showcoc ? 0x4080FF : 0x4040FF, 0x0000FF, -0.4, h, -1);
    else if(h < 0)
      wcol = gradient(showcoc ? 0x80C0FF : 0x8080FF, showcoc ? 0x4080FF : 0x4040FF, 0, h, -0.4);
    else if(h < 0.2)
      wcol = gradient(showcoc ? 0x80C0FF : 0x8080FF, 0xFFFFFF, 0, h, 0.2);
    // else if(h < 0.4)
    //  wcol = gradient(0xFFFFFF, 0xFFFF00, 0.2, h, 0.4);
    else if(h < 0.6)
      wcol = gradient(0xFFFFFF, 0xFF0000, 0.2, h, 0.6);
    else if(h < 0.8)
      wcol = gradient(0xFF0000, 0xFFFF00, 0.6, h, 0.8);
    else
      wcol = 0xFFFF00;
    if(c->wall == waFrozenLake) 
      fcol = wcol;
    else
      fcol = (wcol & 0xFEFEFE) >> 1;
    if(c->wall == waLake)
      fcol = wcol = (wcol & 0xFCFCFC) >> 2;
    }
  
  else if(isWateryOrBoat(c) || c->wall == waReptileBridge) {
    if(c->land == laOcean)
      fcol = (c->landparam > 25 && !chaosmode) ? 0x000090 : 
        0x1010C0 + int(32 * sin(ticks / 500. + (chaosmode ? c->CHAOSPARAM : c->landparam)*1.5));
    else if(c->land == laOceanWall)
      fcol = 0x2020FF;
    else if(c->land == laKraken) {
      fcol = 0x0000A0;
      int mafcol = (pseudohept(c) ? 64 : 8);
      /* bool nearshore = false;
      for(int i=0; i<c->type; i++) 
        if(c->mov[i]->wall != waSea && c->mov[i]->wall != waBoat)
          nearshore = true;
      if(nearshore) mafcol += 30; */
      fcol = fcol + mafcol * (4+sin(ticks / 500. + ((euclid||c->master->alt) ? celldistAlt(c) : 0)*1.5))/5;
      }
    else if(c->land == laAlchemist)
      fcol = 0x900090;
    else if(c->land == laWhirlpool)
      fcol = 0x0000C0 + int(32 * sin(ticks / 200. + ((euclid||c->master->alt) ? celldistAlt(c) : 0)*1.5));
    else if(c->land == laLivefjord)
      fcol = 0x000080;
    else if(isWarped(c->land))
      fcol = 0x0000C0 + int((pseudohept(c)?30:-30) * sin(ticks / 600.));
    else
      fcol = wcol;
    }
  
  else if(c->land == laOcean) {
    if(chaosmode)
      fcol = gradient(0xD0A090, 0xD0D020, 0, c->CHAOSPARAM, 30);
    else
      fcol = gradient(0xD0D090, 0xD0D020, -1, sin((double) c->landparam), 1);
    }

  if(c->land == laEmerald) {
    if(c->wall == waCavefloor || c->wall == waCavewall) {
      fcol = wcol = gradient(winf[waCavefloor].color, 0xFF00, 0, 0.5, 1);
      if(c->wall == waCavewall) wcol = 0xC0FFC0;
      }
    }

  if(c->land == laWhirlwind) {
    int wcol[4] = {0x404040, 0x404080, 0x2050A0, 0x5050C0};
    fcol = wcol[whirlwind::fzebra3(c)];
    }

  if(c->land == laIvoryTower) 
    fcol = 0x10101 * (32 + (c->landparam&1) * 32) - 0x000010;
  
  if(c->land == laDungeon) {
    int lp = c->landparam % 5;
      // xcol = (c->landparam&1) ? 0xD00000 : 0x00D000;
    int lps[5] = { 0x402000, 0x302000, 0x202000, 0x282000, 0x382000 };
    fcol = lps[lp];
    if(c->wall == waClosedGate)
      fcol = wcol = 0xC0C0C0;
    if(c->wall == waOpenGate)
      fcol = wcol = 0x404040;
    if(c->wall == waPlatform)
      fcol = wcol = 0xDFB520;
    }
  
  if(c->land == laEndorian) {
    int clev = cwt.c->land == laEndorian ? edgeDepth(cwt.c) : 0;
      // xcol = (c->landparam&1) ? 0xD00000 : 0x00D000;
    fcol = 0x10101 * (32 + (c->landparam&1) * 32) - 0x000010;
    fcol = gradient(fcol, 0x0000D0, clev-10, edgeDepth(c), clev+10);
    if(c->wall == waTrunk) fcol = winf[waTrunk].color;

    if(c->wall == waCanopy || c->wall == waSolidBranch || c->wall == waWeakBranch) {
      fcol = winf[waCanopy].color;
      if(c->landparam & 1) fcol = gradient(0, fcol, 0, .75, 1);
      }
    
    }
    
  // floors become fcol
  if(c->wall == waSulphur || c->wall == waSulphurC || isAlch(c) || c->wall == waPlatform)
    fcol = wcol;
  
  if(c->wall == waDeadTroll2 || c->wall == waPetrified) {
    eMonster m = eMonster(c->wparam);
    if(c->wall == waPetrified) 
      wcol = gradient(wcol, minf[m].color, 0, .2, 1);
    if(c->wall == waPetrified || isTroll(m)) if(!(m == moForestTroll && c->land == laOvergrown))
      wcol = gradient(wcol, minf[m].color, 0, .4, 1);
    }

  if(c->land == laNone && c->wall == waNone) 
    wcol = fcol = 0x101010;

  if(isFire(c))
    fcol = wcol = c->wall == waEternalFire ? weakfirecolor(1500) : firecolor(100);
    
  if(c->wall == waBoat && wmascii) {
    wcol = 0xC06000;
    }
  
  if(mightBeMine(c) || c->wall == waMineOpen) {
    fcol = wcol;
    if(wmblack || wmascii) fcol >>= 1, wcol >>= 1;
    }
  
  if(c->wall == waAncientGrave || c->wall == waFreshGrave || c->wall == waThumperOn || c->wall == waThumperOff || c->wall == waBonfireOff)
    fcol = wcol;
    
  if(c->land == laMinefield && c->wall == waMineMine && (cmode == emMapEditor || !canmove))
    fcol = wcol = 0xFF4040;

  if(mightBeMine(c) && mineMarkedSafe(c))
    fcol = wcol = gradient(wcol, 0x40FF40, 0, 0.2, 1);
    
  if(mightBeMine(c) && mineMarked(c))
    fcol = wcol = gradient(wcol, 0xFF4040, -1, sin(ticks/100.0), 1);

  int rd = rosedist(c);
  if(rd == 1) 
    wcol = gradient(0x804060, wcol, 0,1,3),
    fcol = gradient(0x804060, fcol, 0,1,3);
  if(rd == 2) 
    wcol = gradient(0x804060, wcol, 0,2,3),
    fcol = gradient(0x804060, fcol, 0,2,3);
  
  if(items[itRevolver] && c->pathdist > GUNRANGE && !shmup::on)
    fcol = gradient(fcol, 0, 0, 25, 100),
    wcol = gradient(wcol, 0, 0, 25, 100);
    
  if(c->wall == waDeadfloor || c->wall == waCavefloor) fcol = wcol;
  if(c->wall == waDeadwall) fcol = winf[waDeadfloor].color;
  if(c->wall == waCavewall && c->land != laEmerald) fcol = winf[waCavefloor].color;
  
  if(highwall(c) && !wmspatial)
    fcol = wcol;
  
  if(wmascii && (c->wall == waNone || isWatery(c))) wcol = fcol;
  
  if(c->wall == waNone && c->land == laHive) wcol = fcol;
  
  if(!wmspatial && snakelevel(c) && !realred(c->wall)) fcol = wcol;
  
  if(c->wall == waGlass && !wmspatial) fcol = wcol;
  
  if(c->wall == waRoundTable) fcol = wcol;
  }

bool noAdjacentChasms(cell *c) {
  forCellEx(c2, c) if(c2->wall == waChasm) return false;
  return true;
  }

// -1 if away, 0 if not away
int away(const transmatrix& V2) {
  return intval(C0, V2 * xpush0(1)) > intval(C0, tC0(V2));
  }

void floorShadow(cell *c, const transmatrix& V, int col, bool warp) {
  if(pmodel == mdHyperboloid || pmodel == mdBall) 
    return; // shadows break the depth testing
  if(shmup::on || purehepta) warp = false;
  dynamicval<int> p(poly_outline, OUTLINE_TRANS);
  if(wmescher && qfi.special) {
    queuepolyat(V * qfi.spin * shadowmulmatrix, *qfi.shape, col, PPR_WALLSHADOW);
    }
  else if(warp) {
    if(euclid) {
      if(ishex1(c))
        queuepolyat(V * pispin * applyPatterndir(c), shTriheptaEucShadow[0], col, PPR_WALLSHADOW);
      else
        queuepolyat(V * applyPatterndir(c), shTriheptaEucShadow[ishept(c)?1:0], col, PPR_WALLSHADOW);
      }
    else 
      queuepolyat(V * applyPatterndir(c), shTriheptaFloorShadow[ishept(c)?1:0], col, PPR_WALLSHADOW);
    }
  else {
    queuepolyat(V, shFloorShadow[c->type==6?0:1], col, PPR_WALLSHADOW);
    }
  }

void plainfloor(cell *c, bool warp, const transmatrix &V, int col, int prio) {
  if(warp) {
    if(euclid) {
      if(ishex1(c))
        queuepolyat(V * pispin * applyPatterndir(c), shTriheptaEuc[0], col, prio);
      else
        queuepolyat(V * applyPatterndir(c), shTriheptaEuc[ishept(c)?1:0], col, prio);
      }
    else 
      queuepolyat(V * applyPatterndir(c), shTriheptaFloor[sphere ? 6-c->type : mapeditor::nopattern(c)], col, prio);
    }
  else {
    queuepolyat(V, shFloor[c->type==6?0:1], col, prio);
    }
  }

void qplainfloor(cell *c, bool warp, const transmatrix &V, int col) {
  if(warp) {
    if(euclid) {
      if(ishex1(c))
        qfloor(c, V, pispin * applyPatterndir(c), shTriheptaEuc[0], col);
      else
        qfloor(c, V, applyPatterndir(c), shTriheptaEuc[ishept(c)?1:0], col);
      }
    else 
      qfloor(c, V, applyPatterndir(c), shTriheptaFloor[sphere ? 6-c->type : mapeditor::nopattern(c)], col);
    }
  else {
    qfloor(c, V, shFloor[c->type==6?0:1], col);
    }
  }

void warpfloor(cell *c, const transmatrix& V, int col, int prio, bool warp) {
  if(shmup::on || purehepta) warp = false;
  if(wmescher && qfi.special)
    queuepolyat(V*qfi.spin, *qfi.shape, col, prio);
  else plainfloor(c, warp, V, col, prio);
  }

#define placeSidewallX(a,b,c,d,e,f,g) \
  { if((wmescher && qfi.special) || !validsidepar[c]) { \
    escherSidewall(a,c,d,g); break; } \
    else placeSidewall(a,b,c,d,e,f,g); }
#define placeSidewallXB(a,b,c,d,e,f,g, Break) \
  { if((wmescher && qfi.shape) || !validsidepar[c]) { \
    escherSidewall(a,c,d,g); Break; break; } \
    else placeSidewall(a,b,c,d,e,f,g); }

/* double zgrad(double f1, double f2, int nom, int den) {
  using namespace geom3;
  ld fo1 = factor_to_lev(f1);
  ld fo2 = factor_to_lev(f2);
  return lev_to_factor(fo1 + (fo2-fo1) * nom / den);
  } */

double zgrad0(double l1, double l2, int nom, int den) {
  using namespace geom3;
  return lev_to_factor(l1 + (l2-l1) * nom / den);
  }

void escherSidewall(cell *c, int sidepar, const transmatrix& V, int col) {
  if(sidepar >= SIDE_SLEV && sidepar <= SIDE_SLEV+2) {
    int sl = sidepar - SIDE_SLEV;
    for(int z=1; z<=4; z++) if(z == 1 || (z == 4 && detaillevel == 2))
      warpfloor(c, mscale(V, zgrad0(geom3::slev * sl, geom3::slev * (sl+1), z, 4)), col, PPR_REDWALL-4+z+4*sl, false);
    }
  else if(sidepar == SIDE_WALL) {
    const int layers = 2 << detaillevel;
    for(int z=1; z<layers; z++) 
      warpfloor(c, mscale(V, zgrad0(0, geom3::wall_height, z, layers)), col, PPR_WALL3+z-layers, false);
    }
  else if(sidepar == SIDE_LAKE) {
    const int layers = 1 << (detaillevel-1);
    if(detaillevel) for(int z=0; z<layers; z++) 
      warpfloor(c, mscale(V, zgrad0(-geom3::lake_top, 0, z, layers)), col, PPR_FLOOR+z-layers, false);
    }
  else if(sidepar == SIDE_LTOB) {
    const int layers = 1 << (detaillevel-1);
    if(detaillevel) for(int z=0; z<layers; z++) 
      warpfloor(c, mscale(V, zgrad0(-geom3::lake_bottom, -geom3::lake_top, z, layers)), col, PPR_INLAKEWALL+z-layers, false);
    }
  else if(sidepar == SIDE_BTOI) {
    const int layers = 1 << detaillevel;
    warpfloor(c, mscale(V, geom3::INFDEEP), col, PPR_MINUSINF, false);
    for(int z=1; z<layers; z++) 
      warpfloor(c, mscale(V, zgrad0(-geom3::lake_bottom, -geom3::lake_top, -z, 1)), col, PPR_LAKEBOTTOM+z-layers, false);
    }
  }

void placeSidewall(cell *c, int i, int sidepar, const transmatrix& V, bool warp, bool mirr, int col) {
  if(shmup::on || purehepta) warp = false;
  if(warp && !ishept(c) && (!c->mov[i] || !ishept(c->mov[i]))) return;
  int prio;
  if(mirr) prio = PPR_GLASS - 2;
  else if(sidepar == SIDE_WALL) prio = PPR_WALL3 - 2;
  else if(sidepar == SIDE_WTS3) prio = PPR_WALL3 - 2;
  else if(sidepar == SIDE_LAKE) prio = PPR_LAKEWALL;
  else if(sidepar == SIDE_LTOB) prio = PPR_INLAKEWALL;
  else if(sidepar == SIDE_BTOI) prio = PPR_BELOWBOTTOM;
  else prio = PPR_REDWALL-2+4*(sidepar-SIDE_SLEV);
  
  transmatrix V2 = V * ddspin(c, i);
  
  int aw = away(V2); prio += aw;
  if(!detaillevel && aw < 0) return;

  // prio += c->cpdist - c->mov[i]->cpdist;  
  queuepolyat(V2, 
    (mirr?shMFloorSide:warp?shTriheptaSide:shFloorSide)[sidepar][c->type==6?0:1], col, prio);
  }

bool openorsafe(cell *c) {
  return c->wall == waMineOpen || mineMarkedSafe(c);
  }

#define Dark(x) darkena(x,0,0xFF)

int gridcolor(cell *c1, cell *c2) {
  if(cmode == emDraw) return Dark(0xFFFFFF);
  if(!c2)
    return 0x202020 >> darken;
  int rd1 = rosedist(c1), rd2 = rosedist(c2);
  if(rd1 != rd2) {
    int r = rd1+rd2;
    if(r == 1) return Dark(0x802020);
    if(r == 3) return Dark(0xC02020);
    if(r == 2) return Dark(0xF02020);
    }
  if(chasmgraph(c1) != chasmgraph(c2))
    return Dark(0x808080);
  if(c1->land == laAlchemist && c2->land == laAlchemist && c1->wall != c2->wall)
    return Dark(0xC020C0);
  if((c1->land == laWhirlpool || c2->land == laWhirlpool) && (celldistAlt(c1) != celldistAlt(c2)))
    return Dark(0x2020A0);
  if(c1->land == laMinefield && c2->land == laMinefield && (openorsafe(c1) != openorsafe(c2)))
    return Dark(0xA0A0A0);
  return Dark(0x202020);
  }

void pushdown(cell *c, int& q, const transmatrix &V, double down, bool rezoom, bool repriority) {

  // since we might be changing priorities, we have to make sure that we are sorting correctly
  if(down > 0 && repriority) { 
    int qq = q+1;
    while(qq < size(ptds))
      if(qq > q && ptds[qq].prio < ptds[qq-1].prio) {
        swap(ptds[qq], ptds[qq-1]);
        qq--;
        }
      else qq++;
    }
  
  while(q < size(ptds)) {
    polytodraw& ptd = ptds[q++];
    if(ptd.kind == pkPoly) {
    
      double z2;
      
      double z = zlevel(tC0(ptd.u.poly.V));
      double lev = geom3::factor_to_lev(z);
      double nlev = lev - down;
      
      double xyscale = rezoom ? geom3::scale_at_lev(lev) / geom3::scale_at_lev(nlev) : 1;
      z2 = geom3::lev_to_factor(nlev);
      double zscale = z2 / z;
      
      // xyscale = xyscale + (zscale-xyscale) * (1+sin(ticks / 1000.0)) / 2;
      
      ptd.u.poly.V = xyzscale( V, xyscale*zscale, zscale)
        * inverse(V) * ptd.u.poly.V;
      
      if(!repriority) ;
      else if(nlev < -geom3::lake_bottom-1e-3) {
        ptd.prio = PPR_BELOWBOTTOM;
        if(c->wall != waChasm)
          ptd.col = 0; // disappear!
        }
      else if(nlev < -geom3::lake_top-1e-3)
        ptd.prio = PPR_INLAKEWALL;
      else if(nlev < 0)
        ptd.prio = PPR_LAKEWALL;
      }
    }
  }

bool dodrawcell(cell *c) {
  // todo: fix when scrolling
  if(!buggyGeneration && c->land != laCanvas && sightrange < 10) {
    // not yet created
    if(c->mpdist > 7 && !cheater) return false;
    // in the Yendor Challenge, scrolling back is forbidden
    if(c->cpdist > 7 && (yendor::on && !cheater)) return false;
    // (incorrect comment) too far, no bugs nearby
    if(playermoved && sightrange <= 7 && c->cpdist > sightrange) return false;
    }
  
  return true;
  }

// 1 : (floor, water); 2 : (water, bottom); 4 : (bottom, inf)

int shallow(cell *c) {
  if(cellUnstable(c)) return 0;
  else if(
    c->wall == waReptile) return 1;
  else if(c->wall == waReptileBridge ||
    c->wall == waGargoyleFloor ||
    c->wall == waGargoyleBridge ||
    c->wall == waTempFloor ||
    c->wall == waTempBridge ||
    c->wall == waFrozenLake)
    return 5;
  return 7;
  }

bool viewdists = false;

bool allemptynear(cell *c) {
  if(c->wall) return false;
  forCellEx(c2, c) if(c2->wall) return false;
  return true;
  }

void drawcell(cell *c, transmatrix V, int spinv, bool mirrored) {

  qfi.shape = NULL; qfi.special = false;
  ivoryz = isGravityLand(c->land);

  transmatrix& gm = gmatrix[c];
  bool orig = (gm[2][2] == 0 || fabs(gm[2][2]-1) >= fabs(V[2][2]-1) - 1e-8);

  if(sphere && vid.alpha > 1) {
     long double d = V[2][2];
     if(d > -1/vid.alpha) return;
     }  
  
  if(sphere && vid.alpha <= 1) {
    if(V[2][2] < -.8) return;
    }
  
  if(orig) gm = V;

  ld dist0 = hdist0(tC0(V)) - 1e-6;
  if(dist0 < geom3::highdetail) detaillevel = 2;
  else if(dist0 < geom3::middetail) detaillevel = 1;
  else detaillevel = 0;

#ifdef BUILDZEBRA
  if(c->type == 6 && c->tmp > 0) {
    int i = c->tmp;
    zebra(cellwalker(c, i&15), 1, i>>4, "", 0);
    }
  
  c->item = eItem(c->heat / 4);
  buildAutomatonRule(c);
#endif

  viewBuggyCells(c,V);
  
  if(conformal::on || inHighQual) checkTide(c);
  
  if(!euclid) {
    // draw a web-like map
    if(webdisplay & 1) {
      if(c->type == 6) {
        for(int a=0; a<3; a++)
        queueline(V*Crad[a*S7], V*Crad[a*S7+S42/2], darkena(0xd0d0, 0, 0xFF), 2);
        }
      else {
        for(int a=0; a<S7; a++)
        queueline(tC0(V), V*Crad[(3*S7+a*6)%S42], darkena(0xd0d0, 0, 0xFF), 2);
        }
      }
  
    if(webdisplay & 2) if(c->type != 6) {
      queueline(tC0(V), V*ddi0(purehepta?S42:0, tessf), darkena(0xd0d0, 0, 0xFF), 2);
      }
    
    if(webdisplay & 4) if(c->type != 6 && !euclid && c->master->alt) {
      for(int i=0; i<S7; i++)
        if(c->master->move[i] && c->master->move[i]->alt == c->master->alt->move[0])
          queueline(tC0(V), V*xspinpush0((purehepta?M_PI:0) -2*M_PI*i/S7, tessf), darkena(0xd000d0, 0, 0xFF), 2);
      }
    }
  
  // save the player's view center
  if(isPlayerOn(c) && !shmup::on) {
    playerfound = true;

/*   if(euclid)
    return d * S84 / c->type;
  else
    return S42 - d * S84 / c->type;
    cwtV = V * spin(-cwt.spin * 2*M_PI/c->type) * pispin; */

    if(multi::players > 1) {
      for(int i=0; i<numplayers(); i++) 
        if(playerpos(i) == c) {
          playerV = V * ddspin(c, multi::player[i].spin);
          if(multi::player[i].mirrored) playerV = playerV * Mirror;
          if(multi::player[i].mirrored == mirrored)
            multi::whereis[i] = playerV;
          }
      }
    else {
      playerV = V * ddspin(c, cwt.spin);
      //  playerV = V * spin(displaydir(c, cwt.spin) * M_PI/S42);
      if(cwt.mirrored) playerV = playerV * Mirror;
      if(orig) cwtV = playerV;
      }
    }
  
  /* if(cwt.c->land == laEdge) {   
    if(c == chosenDown(cwt.c, 1, 0)) 
      playerfoundL = c, cwtVL = V;
    if(c == chosenDown(cwt.c, -1, 0)) 
      playerfoundR = c, cwtVR = V;
    } */

  if(1) {
  
    hyperpoint VC0 = tC0(V);
  
    if(intval(mouseh, VC0) < modist) {
      modist2 = modist; mouseover2 = mouseover;
      modist = intval(mouseh, VC0);
      mouseover = c;
      }
    else if(intval(mouseh, VC0) < modist2) {
      modist2 = intval(mouseh, VC0);
      mouseover2 = c;
      }

    double dfc = euclid ? intval(VC0, C0) : VC0[2];
    
    if(dfc < centdist) {
      centdist = dfc;
      centerover = c;
      }
    
    int orbrange = (items[itRevolver] ? 3 : 2);
    
    if(c->cpdist <= orbrange) if(multi::players > 1 || multi::alwaysuse) 
    for(int i=0; i<multi::players; i++) if(multi::playerActive(i)) {
      double dfc = intval(VC0, tC0(multi::crosscenter[i]));
      if(dfc < multi::ccdist[i] && celldistance(playerpos(i), c) <= orbrange) {
        multi::ccdist[i] = dfc;
        multi::ccat[i] = c;
        }
      }

    // int col = 0xFFFFFF - 0x20 * c->maxdist - 0x2000 * c->cpdist;

    if(!buggyGeneration && c->mpdist > 8 && !cheater) return; // not yet generated
    
    if(c->land == laNone && cmode == emMapEditor) {
      queuepoly(V, shTriangle, 0xFF0000FF);
      }
  
    char ch = winf[c->wall].glyph;
    int wcol, fcol, asciicol;
    
    setcolors(c, wcol, fcol);

    if(viewdists) {
      int cd = celldistance(c, cwt.c);
      string label = its(cd);
      // string label = its(fieldpattern::getriverdistleft(c)) + its(fieldpattern::getriverdistright(c));
      int dc = distcolors[cd&7];
      wcol = gradient(wcol, dc, 0, .4, 1);
      fcol = gradient(fcol, dc, 0, .4, 1);
      /* queuepolyat(V, shFloor[ct6], darkena(gradient(0, distcolors[cd&7], 0, .25, 1), fd, 0xC0),
        PPR_TEXT); */
      queuestr(V, (cd > 9 ? .6 : 1) * .2, label, 0xFF000000 + distcolors[cd&7], 1);
      }

    asciicol = wcol;
    
    if(c->land == laNone && c->wall == waNone) 
      queuepoly(V, shTriangle, 0xFFFF0000);

    if(c->wall == waThumperOn) {
      int ds = ticks;
      for(int u=0; u<5; u++) {
        ld rad = hexf * (.3 * u + (ds%1000) * .0003);
        int tcol = darkena(gradient(0xFFFFFF, 0, 0, rad, 1.5 * hexf), 0, 0xFF);
        for(int a=0; a<S84; a++)
          queueline(V*ddi0(a, rad), V*ddi0(a+1, rad), tcol, 0);
        }
      }
  
    // bool dothept = false;

    /* if(pseudohept(c) && vid.darkhepta) {
      col = gradient(0, col, 0, 0.75, 1);
      } */
      
    eItem it = c->item;
    
    bool hidden = itemHidden(c);
    bool hiddens = itemHiddenFromSight(c);
    
    if(conformal::includeHistory && eq(c->aitmp, sval)) {
      hidden = true;
      hiddens = false;
      }
    
    if(hiddens && cmode != emMapEditor)
      it = itNone;

    int icol = 0, moncol = 0xFF00FF;
    
    if(it) 
      ch = iinf[it].glyph, asciicol = icol = iinf[it].color;
    
    if(c->monst) {
      ch = minf[c->monst].glyph, moncol = minf[c->monst].color;
      if(c->monst == moMutant) {
        // root coloring
        if(c->stuntime != mutantphase)
          moncol = 
            gradient(0xC00030, 0x008000, 0, (c->stuntime-mutantphase) & 15, 15);
        }
      if(isMetalBeast(c->monst) && c->stuntime) 
        moncol >>= 1;

      if(c->monst == moSlime) {
        moncol = winf[c->wall].color;
        moncol |= (moncol>>1);
        }
      
      asciicol = moncol;

      if(isDragon(c->monst) || isKraken(c->monst)) if(!c->hitpoints)
        asciicol = 0x505050;
      
      if(c->monst == moTortoise)
        asciicol =  tortoise::getMatchColor(tortoise::getb(c));
      
      if(c->monst != moMutant) for(int k=0; k<c->stuntime; k++) 
        asciicol = ((asciicol & 0xFEFEFE) >> 1) + 0x101010;
      }
    
    if(c->cpdist == 0 && mapeditor::drawplayer) { 
      ch = '@'; 
      if(!mmitem) asciicol = moncol = cheater ? 0xFF3030 : 0xD0D0D0; 
      }
    
    if(c->ligon) {
      int tim = ticks - lightat;
      if(tim > 1000) tim = 800;
      if(elec::havecharge && tim > 400) tim = 400;
      for(int t=0; t<c->type; t++) if(c->mov[t] && c->mov[t]->ligon) {
        int hdir = displaydir(c, t);
        int lcol = darkena(gradient(iinf[itOrbLightning].color, 0, 0, tim, 1100), 0, 0xFF);
        queueline(V*ddi0(ticks, hexf/2), V*ddi0(hdir, crossf), lcol, 2);
        }
      }
    
    int ct = c->type;
    int ct6 = K(c);

    bool error = false;
    
    chasmg = chasmgraph(c);
    
    int fd = 
      c->land == laRedRock ? 0 : 
      (c->land == laOcean || c->land == laLivefjord || c->land == laWhirlpool) ? 1 :
      c->land == laAlchemist || c->land == laIce || c->land == laGraveyard ||
      c->land == laRlyeh || c->land == laTemple || c->land == laWineyard ||
      c->land == laDeadCaves || c->land == laPalace || c->land == laCA ? 1 : 
      c->land == laCanvas ? 0 :
      c->land == laKraken ? 1 :
      c->land == laBurial ? 1 :
      c->land == laIvoryTower ? 1 :
      c->land == laDungeon ? 1 :
      c->land == laMountain ? 1 :
      c->land == laEndorian ? 1 :
      c->land == laCaribbean ? 1 :
      c->land == laWhirlwind ? 1 :
      c->land == laRose ? 1 :
      c->land == laWarpSea ? 1 :
      c->land == laTortoise ? 1 :
      c->land == laDragon ? 1 :
      c->land == laHalloween ? 1 :
      c->land == laTrollheim ? 2 :
      c->land == laReptile ? 0 :
      2;

    poly_outline = OUTLINE_NONE;

    int sl = snakelevel(c);
    
    transmatrix Vd0, Vf0, Vboat0;
    const transmatrix *Vdp =
      (!wmspatial) ? &V : 
      sl ? &(Vd0= mscale(V, geom3::SLEV[sl])) : 
      highwall(c) ? &(Vd0= mscale(V, (1+geom3::WALL)/2)) :
      (chasmg==1) ? &(Vd0 = mscale(V, geom3::LAKE)) :
      &V;
    
    const transmatrix& Vf = (chasmg && wmspatial) ? (Vf0=mscale(V, geom3::BOTTOM)) : V;

    const transmatrix *Vboat = &(*Vdp);
      
    if(DOSHMUP) {
      ld zlev = -geom3::factor_to_lev(zlevel(tC0((*Vdp))));
      shmup::drawMonster(V, c, Vboat, Vboat0, zlev);
      }

    poly_outline = (backcolor << 8) + 0xFF;

    if(!wmascii) {
    
      // floor
      
#ifndef NOEDIT
      transmatrix Vpdir = V * applyPatterndir(c);
#endif
        
      bool eoh = euclid || purehepta;

#ifndef NOEDIT
      if(c == mapeditor::drawcell && c != cwt.c && !c->monst && !c->item) {
        mapeditor::drawtrans = Vpdir;
        }
#endif

      if(c->wall == waChasm) {
        if(c->land == laZebra) fd++;
        if(c->land == laHalloween && !wmblack) {
          transmatrix Vdepth = mscale(V, geom3::BOTTOM);
          queuepolyat(Vdepth, shFloor[ct6], darkena(firecolor(ticks / 10), 0, 0xDF),
            PPR_LAKEBOTTOM);
          }
        }
              
#ifndef NOEDIT
      if(drawUserShape(Vpdir, mapeditor::cellShapeGroup(), mapeditor::realpatternsh(c),
        darkena(fcol, fd, cmode == emDraw ? 0xC0 : 0xFF)));
      
      else if(mapeditor::whichShape == '7') {
        if(ishept(c))
          qfloor(c, Vf, wmblack ? shBFloor[ct6] : 
            euclid ? shBigHex :
            shBigHepta, darkena(fcol, fd, 0xFF));
        }
      
      else if(mapeditor::whichShape == '8') {
        if(euclid) 
          qfloor(c, Vf, shTriheptaEuc[ishept(c) ? 1 : ishex1(c) ? 0 : 2], darkena(fcol, fd, 0xFF));
        else
          qfloor(c, Vf, shTriheptaFloor[ishept(c) ? 1 : 0], darkena(fcol, fd, 0xFF));
        }
      
      else if(mapeditor::whichShape == '6') {
        if(!ishept(c))
          qfloor(c, Vf, 
            wmblack ? shBFloor[ct6] : 
            euclid ? (ishex1(c) ? shBigHexTriangle : shBigHexTriangleRev) :
            shBigTriangle, darkena(fcol, fd, 0xFF));
        }
#endif
      
      else if(c->land == laWineyard && cellHalfvine(c)) {

        int i =-1;
        for(int t=0;t<6; t++) if(c->mov[t] && c->mov[t]->wall == c->wall)
          i = t;

        qfi.spin = ddspin(c, i, S14);
        transmatrix V2 = V * qfi.spin;
        
        if(wmspatial && wmescher) {
          qfi.shape = &shSemiFeatherFloor[0]; qfi.special = true;
          int dk = 1;
          int vcol = winf[waVinePlant].color;
          warpfloor(c, mscale(V, geom3::WALL), darkena(vcol, dk, 0xFF), PPR_WALL3A, false);
          escherSidewall(c, SIDE_WALL, V, darkena(gradient(0, vcol, 0, .8, 1), dk, 0xFF));
          qfloor(c, V2, shSemiFeatherFloor[1], darkena(fcol, dk, 0xFF));
          qfi.shape = &shFeatherFloor[0]; qfi.special = true;
          }
        
        else if(wmspatial) {
          hpcshape *shar = wmplain ? shFloor : shFeatherFloor;
          
          int dk = 1;
          qfloor(c, V, shar[0], darkena(fcol, dk, 0xFF));
          
          int vcol = winf[waVinePlant].color;
          int vcol2 = gradient(0, vcol, 0, .8, 1);
          
          transmatrix Vdepth = mscale(V2, geom3::WALL);

          queuepolyat(Vdepth, shSemiFloor[0], darkena(vcol, dk, 0xFF), PPR_WALL3A);
          {dynamicval<int> p(poly_outline, OUTLINE_TRANS); queuepolyat(V2 * spin(M_PI*2/3), shSemiFloorShadow, SHADOW_WALL, PPR_WALLSHADOW); }
          queuepolyat(V2, shSemiFloorSide[SIDE_WALL], darkena(vcol, dk, 0xFF), PPR_WALL3A-2+away(V2));

          if(validsidepar[SIDE_WALL]) forCellIdEx(c2, j, c) {
            int dis = i-j;
            dis %= 6;
            if(dis<0) dis += 6;
            if(dis != 1 && dis != 5) continue;
            placeSidewall(c, j, SIDE_WALL, V, false, false, darkena(vcol2, fd, 0xFF));
            }
          }
        
        else {        
          hpcshape *shar = shSemiFeatherFloor;
          
          if(wmblack) shar = shSemiBFloor;
          if(wmplain) shar = shSemiFloor;
          
          int dk = wmblack ? 0 : wmplain ? 1 : 1;
          
          qfloor(c, V2, shar[0], darkena(winf[waVinePlant].color, dk, 0xFF));
          qfloor(c, V2, shar[1], darkena(fcol, dk, 0xFF));
          }
        }

      else if(c->land == laReptile || c->wall == waReptile)
        drawReptileFloor(Vf, c, fcol, true);
      
      else if(wmblack == 1 && c->wall == waMineOpen && vid.grid) 
        ;
      
      else if(wmblack) {
        qfloor(c, Vf, shBFloor[ct6], darkena(fcol, 0, 0xFF));
        int rd = rosedist(c);
        if(rd == 1)
          qfloor(c, Vf, shHeptaMarker, darkena(fcol, 0, 0x80));
        else if(rd == 2)
          qfloor(c, Vf, shHeptaMarker, darkena(fcol, 0, 0x40));
        }
      
      else if(isWarped(c) && euclid)
        qfloor(c, Vf, shTriheptaEuc[ishept(c)?1:ishex1(c)?0:2], darkena(fcol, fd, 0xFF));

      else if(isWarped(c) && !purehepta && !shmup::on) {
        int np = mapeditor::nopattern(c);
        if(c->landparam == 1337) np = 0; // for the achievement screenshot
        if(np < 11)
          qfloor(c, Vf, applyPatterndir(c), shTriheptaFloor[np], darkena(fcol, fd, 0xFF));
        }

      else if(wmplain) {
        if(wmspatial && highwall(c)) ;
        else qfloor(c, Vf, shFloor[ct6], darkena(fcol, fd, 0xFF));
        }

      else if(randomPatternsMode && c->land != laBarrier && !isWarped(c->land)) {
        int j = (randompattern[c->land]/5) % 15;
        int dfcol = darkena(fcol, fd, 0xFF);
        int k = randompattern[c->land] % RPV_MODULO;
        int k7 = randompattern[c->land] % 7;
        
        if(k == RPV_ZEBRA && k7 < 2) drawZebraFloor(Vf, c, dfcol);
        else if(k == RPV_EMERALD && k7 == 0) drawEmeraldFloor(Vf, c, dfcol);
        else if(k == RPV_CYCLE && k7 < 4) drawTowerFloor(Vf, c, dfcol, celldist);
 
        else switch(j) {
          case 0:  qfloor(c, Vf, shCloudFloor[ct6], dfcol); break;
          case 1:  qfloor(c, Vf, shFeatherFloor[ECT], dfcol); break;
          case 2:  qfloor(c, Vf, shStarFloor[ct6], dfcol); break;
          case 3:  qfloor(c, Vf, shTriFloor[ct6], dfcol); break;
          case 4:  qfloor(c, Vf, shSStarFloor[ct6], dfcol); break;
          case 5:  qfloor(c, Vf, shOverFloor[ECT], dfcol); break;
          case 6:  qfloor(c, Vf, shFeatherFloor[ECT], dfcol); break;
          case 7:  qfloor(c, Vf, shDemonFloor[ct6], dfcol); break;
          case 8:  qfloor(c, Vf, shCrossFloor[ct6], dfcol); break;
          case 9:  qfloor(c, Vf, shMFloor[ct6], dfcol); break;
          case 10: qfloor(c, Vf, shCaveFloor[ECT], dfcol); break;
          case 11: qfloor(c, Vf, shPowerFloor[ct6], dfcol); break;
          case 12: qfloor(c, Vf, shDesertFloor[ct6], dfcol); break;
          case 13: qfloor(c, Vf, purehepta ? shChargedFloor[3] : shChargedFloor[ct6], dfcol); break;
          case 14: qfloor(c, Vf, ct==6?shChargedFloor[2]:shFloor[1], dfcol); break;
          }
        }

      // else if(c->land == laPrairie && !eoh && allemptynear(c) && fieldpattern::getflowerdist(c) <= 1)
      //   queuepoly(Vf, shLeafFloor[ct6], darkena(fcol, fd, 0xFF));

      /* else if(c->land == laPrairie && prairie::isriver(c))
        drawTowerFloor(Vf, c, darkena(fcol, fd, 0xFF), 
          prairie::isleft(c) ? river::towerleft : river::towerright); */
      
      else if(c->land == laPrairie)
        qfloor(c, Vf, shCloudFloor[ct6], darkena(fcol, fd, 0xFF));
      
      else if(c->land == laWineyard) {
        qfloor(c, Vf, shFeatherFloor[euclid?2:ct6], darkena(fcol, fd, 0xFF));
        }

      else if(c->land == laZebra) 
        drawZebraFloor(Vf, c, darkena(fcol, fd, 0xFF));
      
      else if(c->wall == waTrunk) 
        qfloor(c, Vf, shFloor[ct6], darkena(fcol, fd, 0xFF));

      else if(c->wall == waCanopy || c->wall == waSolidBranch || c->wall == waWeakBranch) 
        qfloor(c, Vf, shFeatherFloor[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laMountain) 
        drawTowerFloor(Vf, c, darkena(fcol, fd, 0xFF), 
          euclid ? celldist : c->master->alt ? celldistAltPlus : celldist);

      else if(isGravityLand(c->land)) 
        drawTowerFloor(Vf, c, darkena(fcol, fd, 0xFF));

      else if(c->land == laEmerald) 
        drawEmeraldFloor(Vf, c, darkena(fcol, fd, 0xFF));

      else if(c->land == laRlyeh)
        qfloor(c, Vf, (eoh ? shFloor: shTriFloor)[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laTemple)
        qfloor(c, Vf, (eoh ? shFloor: shTriFloor)[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laAlchemist)
        qfloor(c, Vf, shCloudFloor[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laRose)
        qfloor(c, Vf, shRoseFloor[purehepta ? 2 : ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laTortoise)
        qfloor(c, Vf, shTurtleFloor[purehepta ? 2 : ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laDragon && !purehepta) {
        /* if(!wmspatial || noAdjacentChasms(c)) */
          qfloor(c, Vf, shDragonFloor[euclid?2:ct6], darkena(fcol, fd, 0xFF));
        /* if(wmspatial)
          qfloor(c, Vf, shFloor[euclid?2:ct6], darkena(fcol, fd, 0xFF)); */
        }

      else if((isElemental(c->land) || c->land == laElementalWall) && !eoh)
        qfloor(c, Vf, shNewFloor[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laBurial)
        qfloor(c, Vf, shBarrowFloor[euclid?0:purehepta?2:ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laTrollheim && !eoh)
        qfloor(c, Vf, shTrollFloor[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laTrollheim)
        qfloor(c, Vf, shCaveFloor[euclid?2:1], darkena(fcol, fd, 0xFF));

      else if(c->land == laJungle)
        qfloor(c, Vf, shFeatherFloor[euclid?2:ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laMountain)
        qfloor(c, Vf, shFeatherFloor[euclid?2:ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laGraveyard)
        qfloor(c, Vf, (eoh ? shFloor : shCrossFloor)[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laDeadCaves) {
        qfloor(c, Vf, shCaveFloor[euclid?2:ct6], darkena(fcol, fd, 0xFF));
        }

      else if(c->land == laMotion)
        qfloor(c, Vf, shMFloor[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laWhirlwind)
//      drawZebraFloor(V, c, darkena(fcol, fd, 0xFF));
        qfloor(c, Vf, (eoh ? shCloudFloor : shNewFloor)[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laHell)
        qfloor(c, Vf, (euclid ? shStarFloor : shDemonFloor)[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laIce)
//      qfloor(c, V, shFloor[ct6], darkena(fcol, 2, 0xFF));
        qfloor(c, Vf, shStarFloor[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laCocytus)
        qfloor(c, Vf, (eoh ? shCloudFloor : shDesertFloor)[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laStorms) {
        if(euclid) 
          qfloor(c, ishex1(c) ? V*pispin : Vf, 
            ishept(c) ? shFloor[0] : shChargedFloor[2], darkena(fcol, fd, 0xFF));
        else 
          qfloor(c, Vf, (purehepta ? shChargedFloor[3] : ct==6 ? shChargedFloor[2] : shFloor[1]), darkena(fcol, fd, 0xFF));
        }

      else if(c->land == laWildWest)
        qfloor(c, Vf, (eoh ? shCloudFloor : shSStarFloor)[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laPower)
        qfloor(c, Vf, (eoh ? shStarFloor : shPowerFloor)[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laHive && c->wall != waFloorB && c->wall != waFloorA && c->wall != waMirror && c->wall != waCloud) {
        qfloor(c, Vf, shFloor[ct6], darkena(fcol, 1, 0xFF));
        if(c->wall != waMirror && c->wall != waCloud)
          qfloor(c, Vf, shMFloor[ct6], darkena(fcol, 2, 0xFF));
        if(c->wall != waMirror && c->wall != waCloud)
          qfloor(c, Vf, shMFloor2[ct6], darkena(fcol, fcol==wcol ? 1 : 2, 0xFF));
        }

      else if(c->land == laCaves)
        qfloor(c, Vf, shCaveFloor[ECT], darkena(fcol, fd, 0xFF));

      else if(c->land == laDesert)
        qfloor(c, Vf, (eoh ? shCloudFloor : shDesertFloor)[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laOvergrown || c->land == laClearing || isHaunted(c->land))
        qfloor(c, Vf, shOverFloor[ECT], darkena(fcol, fd, 0xFF));

      else if(c->land == laRose)
        qfloor(c, Vf, shOverFloor[ECT], darkena(fcol, fd, 0xFF));

      else if(c->land == laBull)
        qfloor(c, Vf, (eoh ? shFloor : shButterflyFloor)[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laDryForest)
        qfloor(c, Vf, (eoh ? shStarFloor : shDesertFloor)[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laCaribbean || c->land == laOcean || c->land == laOceanWall || c->land == laWhirlpool)
        qfloor(c, Vf, shCloudFloor[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laLivefjord)
        qfloor(c, Vf, shCaveFloor[ECT], darkena(fcol, fd, 0xFF));

      else if(c->land == laRedRock)
        qfloor(c, Vf, eoh ? shFloor[ct6] : shDesertFloor[ct6], darkena(fcol, fd, 0xFF));

      else if(c->land == laPalace)
        qfloor(c, Vf, (eoh?shFloor:shPalaceFloor)[ct6], darkena(fcol, fd, 0xFF));

      else {
        qfloor(c, Vf, shFloor[ct6], darkena(fcol, fd, 0xFF));
        }
      // walls

#ifndef NOEDIT

      if(mapeditor::displaycodes) {

        int labeli = mapeditor::displaycodes == 1 ? mapeditor::realpattern(c) : mapeditor::subpattern(c);
        
        string label = its(labeli);
        if(svg::in)
          queuestr(V, .5, label, 0xFF000000);
        else
          queuestr(V, .2, label, 0xFFFFFFFF);
        
        /* transmatrix V2 = V * applyPatterndir(c);
        qfloor(c, V2, shNecro, 0x80808080);
        qfloor(c, V2, shStatue, 0x80808080); */
        }
#endif

      if(cmode == emNumber && (dialog::editingDetail())) {
        int col = 
          dist0 < geom3::highdetail ? 0xFF80FF80 :
          dist0 >= geom3::middetail ? 0xFFFF8080 :
          0XFFFFFF80;
#if 1
        queuepoly(V, shHeptaMarker, darkena(col & 0xFFFFFF, 0, 0xFF));
#else
        char buf[64];
        sprintf(buf, "%3.1f", float(dist0));
        

        queuestr(V, .6, buf, col);
#endif
        }

      if(realred(c->wall) && !wmspatial) {
        int s = snakelevel(c);
        if(s >= 1)
          qfloor(c, V, shRedRockFloor[0][ct6], darkena(winf[waRed1].color, 0, 0xFF));
        if(s >= 2)
          queuepoly(V, shRedRockFloor[1][ct6], darkena(winf[waRed2].color, 0, 0xFF));
        if(s >= 3)
          queuepoly(V, shRedRockFloor[2][ct6], darkena(winf[waRed3].color, 0, 0xFF));
        }
      
      if(c->wall == waTower && !wmspatial) {
        qfloor(c, V, shMFloor[ct6], darkena(0xE8E8E8, fd, 0xFF));
        }
      
      if(pseudohept(c) && (
        c->land == laRedRock || 
        vid.darkhepta ||
        (purehepta && (c->land == laClearing || isWarped(c))))) {
        queuepoly((*Vdp), shHeptaMarker, wmblack ? 0x80808080 : 0x00000080);
        }

      if(conformal::includeHistory && eq(c->aitmp, sval-1))
        queuepoly((*Vdp), shHeptaMarker, 0x000000C0);

      char xch = winf[c->wall].glyph;
      
      if(c->wall == waBigBush) {
        if(detaillevel >= 2)
          queuepolyat(mmscale(V, zgrad0(0, geom3::slev, 1, 2)), shHeptaMarker, darkena(wcol, 0, 0xFF), PPR_REDWALL);
        if(detaillevel >= 1)
          queuepolyat(mmscale(V, geom3::SLEV[1]) * pispin, shWeakBranch, darkena(wcol, 0, 0xFF), PPR_REDWALL+1);
        if(detaillevel >= 2)
          queuepolyat(mmscale(V, zgrad0(0, geom3::slev, 3, 2)), shHeptaMarker, darkena(wcol, 0, 0xFF), PPR_REDWALL+2);
        queuepolyat(mmscale(V, geom3::SLEV[2]), shSolidBranch, darkena(wcol, 0, 0xFF), PPR_REDWALL+3);
        }

      else if(c->wall == waSmallBush) {
        if(detaillevel >= 2)
          queuepolyat(mmscale(V, zgrad0(0, geom3::slev, 1, 2)), shHeptaMarker, darkena(wcol, 0, 0xFF), PPR_REDWALL);
        if(detaillevel >= 1)
          queuepolyat(mmscale(V, geom3::SLEV[1]) * pispin, shWeakBranch, darkena(wcol, 0, 0xFF), PPR_REDWALL+1);
        if(detaillevel >= 2)
          queuepolyat(mmscale(V, zgrad0(0, geom3::slev, 3, 2)), shHeptaMarker, darkena(wcol, 0, 0xFF), PPR_REDWALL+2);
        queuepolyat(mmscale(V, geom3::SLEV[2]), shWeakBranch, darkena(wcol, 0, 0xFF), PPR_REDWALL+3);
        }

      else if(c->wall == waSolidBranch) {
        queuepoly(V, shSolidBranch, darkena(wcol, 0, 0xFF));
        }

      else if(c->wall == waWeakBranch) {
        queuepoly(V, shWeakBranch, darkena(wcol, 0, 0xFF));
        }

      else if(c->wall == waLadder) {
        if(euclid) {
          queuepoly(V, shMFloor[ct6], 0x804000FF);
          queuepoly(V, shMFloor2[ct6], 0x000000FF);
          }
        else {
          queuepolyat(V, shFloor[ct6], 0x804000FF, PPR_FLOOR+1);
          queuepolyat(V, shMFloor[ct6], 0x000000FF, PPR_FLOOR+2);
          }
        }

      if(c->wall == waReptileBridge) {
        Vboat = &(Vboat0 = V);
        dynamicval<qfloorinfo> qfi2(qfi, qfi);
        int col = reptilecolor(c);
        chasmg = 0;
        drawReptileFloor(V, c, col, true);
        forCellIdEx(c2, i, c) if(chasmgraph(c2)) 
          placeSidewallX(c, i, SIDE_LAKE, V, isWarped(c), false, darkena(gradient(0, col, 0, .8, 1), fd, 0xFF));
        chasmg = 1;
        }

      if(c->wall == waBoat || c->wall == waStrandedBoat) {
        double footphase;
        bool magical = items[itOrbWater] && (isPlayerOn(c) || (isFriendly(c) && items[itOrbEmpathy]));
        int outcol = magical ? watercolor(0) : 0xC06000FF;
        int incol = magical ? 0x0060C0FF : 0x804000FF;
        bool nospin = false;

        Vboat = &(Vboat0 = *Vboat); 
        if(wmspatial && c->wall == waBoat) {
          nospin = c->wall == waBoat && applyAnimation(c, Vboat0, footphase, LAYER_BOAT);
          if(!nospin) Vboat0 = Vboat0 * ddspin(c, c->mondir, S42);
          queuepolyat(Vboat0, shBoatOuter, outcol, PPR_BOATLEV);
          Vboat = &(Vboat0 = V);
          }
        if(c->wall == waBoat) {
          nospin = applyAnimation(c, Vboat0, footphase, LAYER_BOAT);
          }
        if(!nospin) 
          Vboat0 = Vboat0 * ddspin(c, c->mondir, S42);
        else {
          transmatrix Vx;
          if(applyAnimation(c, Vx, footphase, LAYER_SMALL))
            animations[LAYER_SMALL][c].footphase = 0;
          }
        if(wmspatial)
          queuepolyat(mscale(Vboat0, (geom3::LAKE+1)/2), shBoatOuter, outcol, PPR_BOATLEV2);
        queuepoly(Vboat0, shBoatOuter, outcol);
        queuepoly(Vboat0, shBoatInner, incol);
        }

      else if(c->wall == waBigStatue) {
        transmatrix V2 = V;
        double footphase;
        applyAnimation(c, V2, footphase, LAYER_BOAT);
        
        queuepolyat(V2, shStatue, 
          darkena(winf[c->wall].color, 0, 0xFF),
          PPR_BIGSTATUE
          );
        }
      
      else if(c->wall == waSulphurC) {
        if(drawstar(c)) {
          if(wmspatial) 
            queuepolyat(mscale(V, geom3::HELLSPIKE), shGiantStar[ct6], darkena(wcol, 0, 0x40), PPR_HELLSPIKE);
          else
            queuepoly(V, shGiantStar[ct6], darkena(wcol, 0, 0xFF));
          }
        }

      else if(c->wall == waClosePlate || c->wall == waOpenPlate || (c->wall == waTrapdoor && c->land != laZebra)) {
        transmatrix V2 = V;
        if(ct != 6 && wmescher) V2 = V * pispin;
        queuepoly(V2, shMFloor[ct6], darkena(winf[c->wall].color, 0, 0xFF));
        queuepoly(V2, shMFloor2[ct6], (!wmblack) ? darkena(fcol, 1, 0xFF) : darkena(0,1,0xFF));
        }

      else if(c->wall == waFrozenLake || c->wall == waLake || c->wall == waCamelotMoat ||
        c->wall == waSea || c->wall == waClosePlate || c->wall == waOpenPlate ||
        c->wall == waOpenGate || c->wall == waTrapdoor)
        ;
      
      else if(c->wall == waRose) {
        wcol <<= 1;
        if(c->cpdist > 5)
          wcol = 0xC0C0C0;
        else if(rosephase == 7)
          wcol = 0xFF0000;
        else 
          wcol = gradient(wcol, 0xC00000, 0, rosephase, 6);
        queuepoly(V, shThorns, 0xC080C0FF);

        for(int u=0; u<4; u+=2)
          queuepoly(V * spin(2*M_PI / 3 / 4 * u), shRose, darkena(wcol, 0, 0xC0));
        }
      
      else if(sl && wmspatial) {
      
        bool w = isWarped(c);
        warpfloor(c, (*Vdp), darkena(wcol, fd, 0xFF), PPR_REDWALL-4+4*sl, w);
        floorShadow(c, V, SHADOW_SL * sl, w);
        bool tower = c->wall == waTower;
        for(int s=0; s<sl; s++) 
        forCellIdEx(c2, i, c) {
          int sl2 = snakelevel(c2);
          if(s >= sl2)
            placeSidewallX(c, i, SIDE_SLEV+s, V, w, false, 
              darkena(tower?0xD0D0D0-i*0x101010 : s==sl-1?wcol:winf[waRed1+s].color, fd, 0xFF));
          }
        }

      else if(c->wall == waRoundTable) ;

      else if(c->wall == waGlass && wmspatial) {
        int col = winf[waGlass].color;
        int dcol = darkena(col, 0, 0x80);
        transmatrix Vdepth = mscale((*Vdp), geom3::WALL);
        queuepolyat(Vdepth, shMFloor[ct6], dcol, PPR_GLASS);
        if(validsidepar[SIDE_WALL]) forCellIdEx(c2, i, c) 
          placeSidewall(c, i, SIDE_WALL, (*Vdp), false, true, dcol);
        }

      else if(c->wall == waGlass && !wmspatial) ;
      
      else if(wmescher && wmspatial && c->wall == waBarrier && c->land == laOceanWall) {
       const int layers = 2 << detaillevel;
       dynamicval<const hpcshape*> ds(qfi.shape, &shCircleFloor);
       dynamicval<bool> db(qfi.special, true);
       for(int z=1; z<layers; z++) {
         double zg = zgrad0(-geom3::lake_top, geom3::wall_height, z, layers);
         warpfloor(c, xyzscale(V, zg*(layers-z)/layers, zg),
           darkena(gradient(0, wcol, -layers, z, layers), 0, 0xFF), PPR_WALL3+z-layers+2, isWarped(c));
         }
        }

      else if(highwall(c)) {
        int wcol0 = wcol;
        int starcol = wcol;        
        if(c->wall == waWarpGate) starcol = 0;
        if(c->wall == waVinePlant) starcol = 0x60C000;

        int wcol2 = gradient(0, wcol0, 0, .8, 1);

        if(c->wall == waClosedGate) {
          int hdir = 0;
          for(int i=0; i<c->type; i++) if(c->mov[i]->wall == waClosedGate)
            hdir = i;
          transmatrix V2 = mscale(V, wmspatial?geom3::WALL:1) * ddspin(c, hdir, S42);
          queuepolyat(V2, shPalaceGate, darkena(wcol, 0, 0xFF), wmspatial?PPR_WALL3A:PPR_WALL);
          starcol = 0;
          }
        
        hpcshape& shThisWall = isGrave(c->wall) ? shCross : shWall[ct6];
        
        if(conegraph(c)) {

         const int layers = 2 << detaillevel;
         for(int z=1; z<layers; z++) {
           double zg = zgrad0(0, geom3::wall_height, z, layers);
           warpfloor(c, xyzscale(V, zg*(layers-z)/layers, zg),
             darkena(gradient(0, wcol, -layers, z, layers), 0, 0xFF), PPR_WALL3+z-layers+2, isWarped(c));
           }
         floorShadow(c, V, SHADOW_WALL, isWarped(c));
         }
        
        else if(true) {
          if(!wmspatial) {
            if(starcol) queuepoly(V, shThisWall, darkena(starcol, 0, 0xFF));
            }
          else {
            transmatrix Vdepth = mscale(V, geom3::WALL);
    
            bool warp = isWarped(c);
            
            if(starcol && !(wmescher && c->wall == waPlatform)) 
              queuepolyat(Vdepth, shThisWall, darkena(starcol, 0, 0xFF), PPR_WALL3A);
    
            warpfloor(c, Vdepth, darkena(wcol0, fd, 0xFF), PPR_WALL3, warp);
            floorShadow(c, V, SHADOW_WALL, warp);
            
            if(c->wall == waCamelot) {
              forCellIdEx(c2, i, c) {
                placeSidewallX(c, i, SIDE_SLEV, V, warp, false, darkena(wcol2, fd, 0xFF));
                }
              forCellIdEx(c2, i, c) {
                placeSidewallX(c, i, SIDE_SLEV+1, V, warp, false, darkena(wcol2, fd, 0xFF));
                }
              forCellIdEx(c2, i, c) {
                placeSidewallX(c, i, SIDE_SLEV+2, V, warp, false, darkena(wcol2, fd, 0xFF));
                }
              forCellIdEx(c2, i, c) {
                placeSidewallX(c, i, SIDE_WTS3, V, warp, false, darkena(wcol2, fd, 0xFF));
                }
              }
            else forCellIdEx(c2, i, c) {
              if(!highwall(c2) || conegraph(c2)) 
                { placeSidewallX(c, i, SIDE_WALL, V, warp, false, darkena(wcol2, fd, 0xFF)); }
              }
            }
          }
        }
      
      else if(c->wall == waFan) {
        queuepoly(V * spin(M_PI/6 - fanframe * M_PI / 3), shFan, darkena(wcol, 0, 0xFF));
        }
      
      else if(xch == '%') {
        if(doHighlight())
          poly_outline = (c->land == laMirror) ? OUTLINE_TREASURE : OUTLINE_ORB;
        
        if(wmspatial) {
          int col = winf[c->wall].color;
          int dcol = darkena(col, 0, 0xC0);
          transmatrix Vdepth = mscale((*Vdp), geom3::WALL);
          queuepolyat(Vdepth, shMFloor[ct6], dcol, PPR_GLASS);
          if(validsidepar[SIDE_WALL]) forCellIdEx(c2, i, c) 
            placeSidewall(c, i, SIDE_WALL, (*Vdp), false, true, dcol);
          }
        else {
          queuepoly(V, shMirror, darkena(wcol, 0, 0xC0));
          }
        poly_outline = OUTLINE_NONE;
        }
      
      else if(isFire(c) || isThumper(c) || c->wall == waBonfireOff) {
        ld sp = 0;
        if(hasTimeout(c)) sp = ticks / (c->land == laPower ? 5000. : 500.);
        queuepoly(V * spin(sp), shStar, darkena(wcol, 0, 0xF0));
        if(isFire(c) && rand() % 300 < ticks - lastt)
          drawParticle(c, wcol, 75);
        }
      
      else if(c->wall == waFreshGrave || c->wall == waAncientGrave)
        queuepoly(V, shCross, darkena(wcol, 0, 0xFF));

      else if(xch == '+' && c->wall == waGiantRug) {
        queuepoly(V, shBigCarpet1, darkena(0xC09F00, 0, 0xFF));
        queuepoly(V, shBigCarpet2, darkena(0x600000, 0, 0xFF));
        queuepoly(V, shBigCarpet3, darkena(0xC09F00, 0, 0xFF));
        }

      else if(xch != '.' && xch != '+' && xch != '>' && xch != ':'&& xch != '-' && xch != ';' && c->wall != waSulphur && xch != ',')
        error = true;
      }
    else if(!(it || c->monst || c->cpdist == 0)) error = true;
    
    int sha = shallow(c);

    if(wmspatial && sha) {
      bool w = isWarped(c);
      int col = (highwall(c) || c->wall == waTower) ? wcol : fcol;
      if(!chasmg) {
        if(sha & 1) {
          forCellIdEx(c2, i, c) if(chasmgraph(c2)) 
            placeSidewallX(c, i, SIDE_LAKE, V, w, false, darkena(gradient(0, col, 0, .8, 1), fd, 0xFF));
          }
        if(sha & 2) {
          forCellIdEx(c2, i, c) if(chasmgraph(c2)) 
            placeSidewallX(c, i, SIDE_LTOB, V, w, false, darkena(gradient(0, col, 0, .7, 1), fd, 0xFF));
          }
        if(sha & 4) {
          bool dbot = true;
          forCellIdEx(c2, i, c) if(chasmgraph(c2) == 2) {
            if(dbot) dbot = false,
              warpfloor(c, mscale(V, geom3::BOTTOM), 0x080808FF, PPR_LAKEBOTTOM, isWarped(c));
            placeSidewallX(c, i, SIDE_BTOI, V, w, false, darkena(gradient(0, col, 0, .6, 1), fd, 0xFF));
            }
          }
        }
      // wall between lake and chasm -- no Escher here
      if(chasmg == 1) forCellIdEx(c2, i, c) if(chasmgraph(c2) == 2) {
        placeSidewall(c, i, SIDE_LAKE, V, w, false, 0x202030FF);
        placeSidewall(c, i, SIDE_LTOB, V, w, false, 0x181820FF);
        placeSidewall(c, i, SIDE_BTOI, V, w, false, 0x101010FF);
        }
      }
    
    if(chasmg == 1 && wmspatial) {
      int fd0 = fd ? fd-1 : 0;
      warpfloor(c, (*Vdp), darkena(fcol, fd0, 0x80), PPR_LAKELEV, isWarped(c));
      }
    
    if(chasmg) {
      int q = size(ptds);
      if(fallanims.count(c)) {
         fallanim& fa = fallanims[c];
         bool erase = true;
         if(fa.t_floor) {
           int t = (ticks - fa.t_floor);
           if(t <= 1500) {
             erase = false;
             if(fa.walltype == waNone)
               warpfloor(c, V, darkena(fcol, fd, 0xFF), PPR_FLOOR, isWarped(c));
             else {
               int wcol2, fcol2;
               eWall w = c->wall; int p = c->wparam;
               c->wall = fa.walltype; c->wparam = fa.m;
               setcolors(c, wcol2, fcol2);
               int starcol = c->wall == waVinePlant ? 0x60C000 : wcol2;
               c->wall = w; c->wparam = p;
               bool warp = isWarped(c);
               warpfloor(c, mscale(V, geom3::WALL), darkena(starcol, fd, 0xFF), PPR_WALL3, warp);
               queuepolyat(mscale(V, geom3::WALL), shWall[ct6], darkena(wcol2, 0, 0xFF), PPR_WALL3A);
               forCellIdEx(c2, i, c)
                 placeSidewallX(c, i, SIDE_WALL, V, warp, false, darkena(wcol2, 1, 0xFF));
               }
             pushdown(c, q, V, t*t / 1000000. + t / 1000., true, true);
             }
           }
         if(fa.t_mon) {
           int t = (ticks - fa.t_mon);
           if(t <= 1500) {
             erase = false;
             c->stuntime = 0;
             transmatrix V2 = V;
             double footphase = t / 200.0;
             applyAnimation(c, V2, footphase, LAYER_SMALL);
             drawMonsterType(fa.m, c, V2, minf[fa.m].color, footphase);
             pushdown(c, q, V2, t*t / 1000000. + t / 1000., true, true);
             }
           }
         if(erase) fallanims.erase(c);
         }
       }

    if(c->wall == waMineOpen) {
      int mines = countMinesAround(c);
      
      if(wmascii) {
        if(ch == '.') {
          if(mines == 0) ch = ' ';
          else ch = '0' + mines, asciicol = minecolors[mines];
          }
        else if(ch == '@') asciicol = minecolors[mines];
        }
      else if(mines > 0)
        queuepoly(V, shMineMark[ct6], (minecolors[mines] << 8) | 0xFF);
      }
    
    // treasure
    
    char xch = iinf[it].glyph;
    hpcshape *xsh = 
      (it == itPirate || it == itKraken) ? &shPirateX :
      (it == itBuggy || it == itBuggy2) ? &shPirateX :
      it == itHolyGrail ? &shGrail :
      isElementalShard(it) ? &shElementalShard :
      (it == itBombEgg || it == itTrollEgg) ? &shEgg :
      it == itDodeca ? &shDodeca :
      xch == '*' ? &shGem[ct6] : 
      it == itTreat ? &shTreat :
      it == itSlime ? &shEgg :
      xch == '%' ? &shDaisy : xch == '$' ? &shStar : xch == ';' ? &shTriangle :
      xch == '!' ? &shTriangle : it == itBone ? &shNecro : it == itStatue ? &shStatue :
      it == itIvory ? &shFigurine : 
      xch == '?' ? &shBookCover : 
      it == itKey ? &shKey : 
      it == itRevolver ? &shGun :
      NULL;

    if(c->land == laWhirlwind && c->wall != waBoat) {
      double footphase = 0;
      Vboat = &(Vboat0 = *Vboat);
      applyAnimation(c, Vboat0, footphase, LAYER_BOAT);
      }
    
    if(it && cellHalfvine(c)) {
      int i =-1;
      for(int t=0;t<6; t++) if(c->mov[t] && c->mov[t]->wall == c->wall)
        i = t;

      Vboat = &(Vboat0 = *Vboat * ddspin(c, i) * xpush(-.13));
      }
    
    if(doHighlight()) {
      int k = itemclass(it);
      if(k == IC_TREASURE)
        poly_outline = OUTLINE_TREASURE;
      else if(k == IC_ORB)
        poly_outline = OUTLINE_ORB;
      else
        poly_outline = OUTLINE_OTHER;
      }
    
    if(conformal::includeHistory && eq(c->aitmp, sval)) poly_outline = OUTLINE_DEAD;
      
#ifndef NOEDIT
    if(c == mapeditor::drawcell && mapeditor::drawcellShapeGroup() == 2)
      mapeditor::drawtrans = V;
#endif
      
    if(!mmitem && it)
      error = true;
    
    else if(it == itBabyTortoise) {
      int bits = tortoise::babymap[c];
      int over = c->monst == moTortoise;
      tortoise::draw(*Vboat * spin(ticks / 5000.) * ypush(crossf*.15), bits, over ? 4 : 2, 0);
      // queuepoly(V, shHeptaMarker, darkena(tortoise::getMatchColor(bits), 0, 0xC0));
      }
    
    else if(it == itCompass) {
      if(euclid) Vboat0 = (*Vdp) * spin(M_PI/2); // todo incorrect
      else Vboat0 = *Vboat * rspintox(inverse(*Vboat) * pirateCoords);
      Vboat0 = Vboat0 * spin(M_PI * sin(ticks/100.) / 30);
      queuepoly(Vboat0, shCompass1, 0xFF8080FF);
      queuepoly(Vboat0, shCompass2, 0xFFFFFFFF);
      queuepoly(Vboat0, shCompass3, 0xFF0000FF);
      queuepoly(Vboat0 * pispin, shCompass3, 0x000000FF);
      xsh = NULL;
      }

    else if(it == itPalace) {
      Vboat0 = *Vboat * spin(ticks / 1500.);
      queuepoly(Vboat0, shMFloor3[ct6], 0xFFD500FF);
      queuepoly(Vboat0, shMFloor4[ct6], darkena(icol, 0, 0xFF));
      queuepoly(Vboat0, shGem[ct6], 0xFFD500FF);
      xsh = NULL;
      }
    
    else if(drawUserShape(*Vboat, 2, it, darkena(icol, 0, 0xFF))) ;
    
    else if(it == itRose) {
      for(int u=0; u<4; u++)
        queuepoly(*Vboat * spin(ticks / 1500.) * spin(2*M_PI / 3 / 4 * u), shRose, darkena(icol, 0, hidden ? 0x30 : 0xA0));
      }

    else if(it == itBarrow) {
      for(int i = 0; i<c->landparam; i++)
        queuepolyat(*Vboat * spin(2 * M_PI * i / c->landparam) * xpush(.15) * spin(ticks / 1500.), *xsh, darkena(icol, 0, hidden ? 0x40 : 
          (highwall(c) && wmspatial) ? 0x60 : 0xFF),
          PPR_HIDDEN);

//      queuepoly(V*spin(M_PI+(1-2*ang)*2*M_PI/S84), shMagicSword, darkena(0xC00000, 0, 0x80 + 0x70 * sin(ticks / 200.0)));
      }
      
    else if(xsh) {
      if(it == itFireShard) icol = firecolor(100);
      if(it == itWaterShard) icol = watercolor(100) >> 8;
      
      if(it == itZebra) icol = 0xFFFFFF;
      if(it == itLotus) icol = 0x101010;
    
      queuepoly(*Vboat * spin(ticks / 1500.), *xsh, darkena(icol, 0, hidden ? (it == itKraken ? 0xC0 : 0x40) : 0xF0));

      if(xsh == &shBookCover && mmitem)
        queuepoly(*Vboat * spin(ticks / 1500.), shBook, 0x805020FF);
      if(it == itZebra)
        queuepolyat(*Vboat * spin(ticks / 1500. + M_PI/c->type), *xsh, darkena(0x202020, 0, hidden ? 0x40 : 0xF0), PPR_ITEMb);
      }
    
    else if(xch == 'o') {
      if(it == itOrbFire) icol = firecolor(100);
      queuepoly(*Vboat, shDisk, darkena(icol, 0, hidden ? 0x20 : 0xC0));
      if(it == itOrbFire) icol = firecolor(200);
      if(it == itOrbFriend || it == itOrbDiscord) icol = 0xC0C0C0;
      if(it == itOrbFrog) icol = 0xFF0000;
      if(it == itOrbDash) icol = 0xFF0000;
      if(it == itOrbFreedom) icol = 0xC0FF00;
      if(it == itOrbAir) icol = 0xFFFFFF;
      if(it == itOrbUndeath) icol = minf[moFriendlyGhost].color;
      if(it == itOrbRecall) icol = 0x101010;
      hpcshape& sh = 
        isRangedOrb(it) ? shTargetRing :
        isOffensiveOrb(it) ? shSawRing :
        isFriendOrb(it) ? shPeaceRing :
        isUtilityOrb(it) ? shGearRing :
        isDirectionalOrb(it) ? shSpearRing :
        it == itOrb37 ? shHeptaRing :
        shRing;
      queuepoly(*Vboat * spin(ticks / 1500.), sh, darkena(icol, 0, int(0x80 + 0x70 * sin(ticks / 300.))));
      }

    else if(it) error = true;

    if(true) {
      int q = ptds.size();
      error |= drawMonster(V, ct, c, moncol); 
      if(Vboat != &V && Vboat != &Vboat0 && q != size(ptds)) 
        pushdown(c, q, V, -geom3::factor_to_lev(zlevel(tC0((*Vboat)))),
          !isMultitile(c->monst), false);
      }
      
    if(!shmup::on && sword::at(c)) {
      queuepolyat(V, shDisk, 0xC0404040, PPR_SWORDMARK);
      }
    
    int ad = airdist(c);
    if(ad == 1 || ad == 2) {

     for(int i=0; i<c->type; i++) {
       cell *c2 = c->mov[i]; 
       if(airdist(c2) < airdist(c)) {
         calcAirdir(c2); // printf("airdir = %d\n", airdir);
         transmatrix V0 = ddspin(c, i, S42);
         
         double ph = ticks / (purehepta?150:75.0) + airdir * M_PI / (S21+.0);
         
         int aircol = 0x8080FF00 | int(32 + 32 * -cos(ph));
         
         double ph0 = ph/2;
         ph0 -= floor(ph0/M_PI)*M_PI;

         poly_outline = OUTLINE_TRANS;
         queuepoly((*Vdp)*V0*xpush(hexf*-cos(ph0)), shDisk, aircol);
         poly_outline = OUTLINE_NONE;
         }
       }

//    queuepoly(V*ddi(rand() % S84, hexf*(rand()%100)/100), shDisk, aircolor(airdir));
      }

    /* int rd = rosedist(c);
    if(rd > 0 && ((rd&7) == (turncount&7))) {

     for(int i=0; i<c->type; i++) {
       cell *c2 = c->mov[i]; 
       if(rosedist(c2) == rosedist(c)-1) {
         int hdir = displaydir(c, i);
         transmatrix V0 = spin((S42+hdir) * M_PI / S42);
         
         double ph = ticks / 75.0; // + airdir * M_PI / (S21+.0);
         
         int rosecol = 0x764e7c00 | int(32 + 32 * -cos(ph));
         
         double ph0 = ph/2;
         ph0 -= floor(ph0/M_PI)*M_PI;

         poly_outline = OUTLINE_TRANS;
         queuepoly(V*V0*ddi(0, hexf*-cos(ph0)), shDisk, rosecol);
         poly_outline = OUTLINE_NONE;
         }
       }
      } */

    if(c->land == laWhirlwind) {
      whirlwind::calcdirs(c);
      
      for(int i=0; i<whirlwind::qdirs; i++) {
        int hdir0 = displaydir(c, whirlwind::dfrom[i]) + S42;
        int hdir1 = displaydir(c, whirlwind::dto[i]);
 
        double ph1 = fanframe;
        
        int aircol = 0xC0C0FF40;
        
        ph1 -= floor(ph1);
        
        if(hdir1 < hdir0-S42) hdir1 += S84;
        if(hdir1 >= hdir0+S42) hdir1 -= S84;
        
        int hdir = (hdir1*ph1+hdir0*(1-ph1));
 
        transmatrix V0 = spin((hdir) * M_PI / S42);
        
        double ldist = purehepta ? crossf : c->type == 6 ? .2840 : 0.3399;
 
        poly_outline = OUTLINE_TRANS;
        queuepoly((*Vdp)*V0*xpush(ldist*(2*ph1-1)), shDisk, aircol);
        poly_outline = OUTLINE_NONE;
        }

      }

    if(error) {
      queuechr(V, 1, ch, asciicol, 2);
      }
    
    if(vid.grid) {
      // sphere: 0.3948
      // sphere heptagonal: 0.5739
      // sphere trihepta: 0.3467
      
      // hyper trihepta: 0.2849
      // hyper heptagonal: 0.6150
      // hyper: 0.3798
      
      if(purehepta) {
        double x = sphere?.645:.6150;
        for(int t=0; t<S7; t++) 
          if(c->mov[t] && c->mov[t] < c)
          queueline(V * ddspin(c,t,-6) * xpush0(x), 
                    V * ddspin(c,t,6) * xpush0(x), 
                    gridcolor(c, c->mov[t]), 1);
        }
      else if(isWarped(c)) {
        double x = sphere?.3651:euclid?.2611:.2849;
        if(!ishept(c)) for(int t=0; t<6; t++) if(c->mov[t] && ishept(c->mov[t]))
          queueline(V * ddspin(c,t,-S14) * xpush0(x), 
                    V * ddspin(c,t,+S14) * xpush0(x), 
                    gridcolor(c, c->mov[t]), 1);
        }
      else if(ishept(c) && !euclid) ;
      else {
        double x = sphere?.401:euclid?.3 : .328;
        for(int t=0; t<6; t++) 
          if(euclid ? c->mov[t]<c : (((t^1)&1) || c->mov[t] < c))
          queueline(V * ddspin(c,t,-S7) * xpush0(x), 
                    V * ddspin(c,t,+S7) * xpush0(x), 
                    gridcolor(c, c->mov[t]), 1);
        }
      }

    if(!euclid && (!pirateTreasureSeek || compassDist(c) < compassDist(pirateTreasureSeek)))
      pirateTreasureSeek = c;

    if(!euclid) {
      bool usethis = false;
      double spd = 1;
      bool rev = false;
      
      if(isGravityLand(cwt.c->land)) {
        if(cwt.c->land == laDungeon) rev = true;
        if(!straightDownSeek || edgeDepth(c) < edgeDepth(straightDownSeek)) {
          usethis = true;
          spd = cwt.c->landparam / 10.;
          }
        }

      if(c->master->alt && cwt.c->master->alt &&
        (cwt.c->land == laMountain || 
        (pmodel && 
          (cwt.c->land == laTemple || cwt.c->land == laWhirlpool || 
          (cheater && (cwt.c->land == laClearing || cwt.c->land == laCaribbean ||
          cwt.c->land == laCamelot || cwt.c->land == laPalace))) 
          ))
        && c->land == cwt.c->land && c->master->alt->alt == cwt.c->master->alt->alt) {
        if(!straightDownSeek || !straightDownSeek->master->alt || celldistAlt(c) < celldistAlt(straightDownSeek)) {
          usethis = true;
          spd = .5;
          if(cwt.c->land == laMountain) rev = true;
          }
        }
  
      if(pmodel && cwt.c->land == laOcean && cwt.c->landparam < 25) {
        if(!straightDownSeek || coastval(c, laOcean) < coastval(straightDownSeek, laOcean)) {
          usethis = true;
          spd = cwt.c->landparam / 10;
          }
          
        }

      if(usethis) {
        straightDownSeek = c;
        downspin = atan2(VC0[1], VC0[0]);
        downspin -= M_PI/2;
        if(rev) downspin += M_PI;
        downspin += M_PI/2 * (conformal::rotation%4);
        while(downspin < -M_PI) downspin += 2*M_PI;
        while(downspin > +M_PI) downspin -= 2*M_PI;
        downspin = downspin * min(spd, (double)1);
        }
      }
      
  if(!inHighQual) {
    
#ifndef NOEDIT
    if(cmode == emMapEditor && !mapeditor::subscreen && lmouseover && darken == 0 &&
      (mapeditor::whichPattern ? mapeditor::subpattern(c) == mapeditor::subpattern(lmouseover) : c == lmouseover)) {
      queuecircle(V, .78, 0x00FFFFFF);
      }
#endif
    
#ifndef NOEDIT
    mapeditor::drawGhosts(c, V, ct);
#endif
    }
    
    if(c->bardir != NODIR && c->bardir != NOBARRIERS && c->land != laHauntedWall &&
      c->barleft != NOWALLSEP_USED) {
      int col = darkena(0x505050, 0, 0xFF);
      queueline(tC0(V), V*tC0(heptmove[c->bardir]), col, 2);
      queueline(tC0(V), V*tC0(hexmove[c->bardir]), col, 2);
      }
    
#ifndef NOMODEL
    netgen::buildVertexInfo(c, V);
#endif

#ifdef LOCAL
    extern void localdraw (const transmatrix& V, cell *c);
    localdraw(V, c);
#endif
    }
  }

bool confusingGeometry() {
  return elliptic || quotient == 1;
  }

struct flashdata {
  int t;
  int size;
  cell *where;
  double angle;
  int spd; // 0 for flashes, >0 for particles
  int color;
  flashdata(int _t, int _s, cell *_w, int col, int sped) { 
    t=_t; size=_s; where=_w; color = col; 
    angle = rand() % 1000; spd = sped;
    }
  };

vector<flashdata> flashes;

void drawFlash(cell *c) {
  flashes.push_back(flashdata(ticks, 1000, c, iinf[itOrbFlash].color, 0)); 
  }
void drawBigFlash(cell *c) { 
  flashes.push_back(flashdata(ticks, 2000, c, 0xC0FF00, 0)); 
  }
void drawParticle(cell *c, int col, int maxspeed) {
  if(vid.particles && !confusingGeometry())
    flashes.push_back(flashdata(ticks, rand() % 16, c, col, 1+rand() % maxspeed)); 
  }
void drawParticles(cell *c, int col, int qty, int maxspeed) { 
  if(vid.particles)
    while(qty--) drawParticle(c,col, maxspeed); 
  }
void drawFireParticles(cell *c, int qty, int maxspeed) { 
  if(vid.particles)
    for(int i=0; i<qty; i++)
      drawParticle(c, firegradient(i / (qty-1.)), maxspeed); 
  }
void fallingFloorAnimation(cell *c, eWall w, eMonster m) {
  if(!wmspatial) return;
  fallanim& fa = fallanims[c];
  fa.t_floor = ticks;
  fa.walltype = w; fa.m = m;
  // drawParticles(c, darkenedby(linf[c->land].color, 1), 4, 50);
  }
void fallingMonsterAnimation(cell *c, eMonster m) {
  if(!mmspatial) return;
  fallanim& fa = fallanims[c];
  fa.t_mon = ticks;
  fa.m = m;
  // drawParticles(c, darkenedby(linf[c->land].color, 1), 4, 50);
  }

void queuecircleat(cell *c, double rad, int col) {
  if(!c) return;
  if(!gmatrix.count(c)) return;
  queuecircle(gmatrix[c], rad, col);  
  if(!wmspatial) return;
  if(highwall(c))
    queuecircle(mscale(gmatrix[c], geom3::WALL), rad, col);
  int sl;
  if((sl = snakelevel(c))) {
    queuecircle(mscale(gmatrix[c], geom3::SLEV[sl]), rad, col);
    }
  if(chasmgraph(c))
    queuecircle(mscale(gmatrix[c], geom3::LAKE), rad, col);
  }

#define G(x) x && gmatrix.count(x)
#define IG(x) if(G(x))
#define Gm(x) gmatrix[x]
#define Gm0(x) tC0(gmatrix[x])

#ifdef MOBILE
#define MOBON (clicked)
#else
#define MOBON true
#endif

void drawMarkers() {

  if(darken || cmode == emNumber) return;

  if(!inHighQual) {

#ifdef PANDORA
    bool ok = mousepressed;
#else
    bool ok = true;
#endif
     
    if(G(dragon::target) && haveMount()) {
      queuechr(Gm0(dragon::target), 2*vid.fsize, 'X', 
        gradient(0, iinf[itOrbDomination].color, -1, sin(ticks/(dragon::whichturn == turncount ? 75. : 150.)), 1));
      }

    /* for(int i=0; i<12; i++) if(c->type == 5 && c->master == &dodecahedron[i])
      queuechr(xc, yc, sc, 4*vid.fsize, 'A'+i, iinf[itOrbDomination].color); */
    
    IG(keycell) {
      queuechr(Gm0(keycell), 2*vid.fsize, 'X', 0x10101 * int(128 + 100 * sin(ticks / 150.)));
      queuestr(Gm0(keycell), vid.fsize, its(keycelldist), 0x10101 * int(128 - 100 * sin(ticks / 150.)));
      }
    
    IG(pirateTreasureFound) { 
      pirateCoords = Gm0(pirateTreasureFound);
      if(showPirateX) {
        queuechr(pirateCoords, 2*vid.fsize, 'X', 0x10100 * int(128 + 100 * sin(ticks / 150.)));
        if(numplayers() == 1 && cwt.c->master->alt)
          queuestr(pirateCoords, vid.fsize, its(-celldistAlt(cwt.c)), 0x10101 * int(128 - 100 * sin(ticks / 150.)));
        }
      }
          
    if(lmouseover && vid.drawmousecircle && ok && DEFAULTCONTROL && MOBON) {
      queuecircleat(lmouseover, .8, darkena(lmouseover->cpdist > 1 ? 0x00FFFF : 0xFF0000, 0, 0xFF));
      }

    if(global_pushto && vid.drawmousecircle && ok && DEFAULTCONTROL && MOBON) {
      queuecircleat(global_pushto, .6, darkena(0xFFD500, 0, 0xFF));
      }

    if(joydir.d >= 0) 
      queuecircleat(cwt.c->mov[(joydir.d+cwt.spin) % cwt.c->type], .78 - .02 * sin(ticks/199.0), 
        darkena(0x00FF00, 0, 0xFF));

#ifndef NOMODEL
    if(centerover && !playermoved && netgen::mode == 0 && !conformal::on)
      queuecircleat(centerover, .70 - .06 * sin(ticks/200.0), 
        darkena(int(175 + 25 * sin(ticks / 200.0)), 0, 0xFF));
#endif

    if(multi::players > 1 || multi::alwaysuse) for(int i=0; i<numplayers(); i++) {
      multi::cpid = i;
      if(multi::players == 1) multi::player[i] = cwt;
      cell *ctgt = multi::multiPlayerTarget(i);
      queuecircleat(ctgt, .40 - .06 * sin(ticks/200.0 + i * 2 * M_PI / numplayers()), getcs().uicolor);
      }

    // process mouse

#ifdef MOBILE
  extern bool useRangedOrb;
  if(canmove && !shmup::on && andmode == 0 && !useRangedOrb && vid.mobilecompasssize > 0) {
    using namespace shmupballs;
    calc();
    queuecircle(xmove, yb, rad, 0xFF0000FF);
    queuecircle(xmove, yb, rad*SKIPFAC, 
      legalmoves[7] ? 0xFF0000FF : 0xFF000080
      );
    forCellAll(c2, cwt.c) IG(c2) drawMobileArrow(c2, Gm(c2));
    }
#endif

    if((vid.axes == 4 || (vid.axes == 1 && !mousing)) && !shmup::on) {
      if(multi::players == 1) {
        forCellAll(c2, cwt.c) IG(c2) drawMovementArrows(c2, Gm(c2));
        }
      else if(multi::players > 1) for(int p=0; p<multi::players; p++) {
        if(multi::playerActive(p) && (vid.axes == 4 || !drawstaratvec(multi::mdx[p], multi::mdy[p]))) 
        forCellAll(c2, multi::player[p].c) IG(c2) {
          multi::cpid = p;
          dynamicval<transmatrix> ttm(cwtV, multi::whereis[p]);
          dynamicval<cellwalker> tcw(cwt, multi::player[p]);
          drawMovementArrows(c2, Gm(c2));
          }
        }
      }
    }

  monsterToSummon = moNone;
  orbToTarget = itNone;

  if(mouseover && targetclick && cmode == emNormal) {
    shmup::cpid = 0;
    orbToTarget = targetRangedOrb(mouseover, roCheck);
    if(orbToTarget == itOrbSummon) {
      monsterToSummon = summonedAt(mouseover);
      queuechr(mousex, mousey, 0, vid.fsize, minf[monsterToSummon].glyph, minf[monsterToSummon].color);
      queuecircleat(mouseover, 0.6, darkena(minf[monsterToSummon].color, 0, 0xFF));
      }
    else if(orbToTarget) {
      queuechr(mousex, mousey, 0, vid.fsize, '@', iinf[orbToTarget].color);
      queuecircleat(mouseover, 0.6, darkena(iinf[orbToTarget].color, 0, 0xFF));
      }
    if(orbToTarget && rand() % 200 < ticks - lastt) {
      if(orbToTarget == itOrbDragon)
        drawFireParticles(mouseover, 2);
      else if(orbToTarget == itOrbSummon) {
        drawParticles(mouseover, iinf[orbToTarget].color, 1);
        drawParticles(mouseover, minf[monsterToSummon].color, 1);
        }
      else {
        drawParticles(mouseover, iinf[orbToTarget].color, 2);
        }
      }
    }  
  }

void drawFlashes() {
  for(int k=0; k<size(flashes); k++) {
    flashdata& f = flashes[k];
    transmatrix V = gmatrix[f.where];
    int tim = ticks - f.t;
    
    bool kill = tim > f.size;
    
    if(f.spd) {
      kill = tim > 300;
      int partcol = darkena(f.color, 0, max(255 - kill/2, 0));
      poly_outline = OUTLINE_NONE;
      queuepoly(V * spin(f.angle) * xpush(f.spd * tim / 50000.), shParticle[f.size], partcol);
      }
    
    else if(f.size == 1000) {
      for(int u=0; u<=tim; u++) {
        if((u-tim)%50) continue;
        if(u < tim-150) continue;
        ld rad = u * 3 / 1000.;
        rad = rad * (5-rad) / 2;
        rad *= hexf;
        int flashcol = f.color;
        if(u > 500) flashcol = gradient(flashcol, 0, 500, u, 1100);
        flashcol = darkena(flashcol, 0, 0xFF);
        for(int a=0; a<=S84; a++) curvepoint(V*ddi0(a, rad));
        queuecurve(flashcol, 0x8080808, PPR_LINE);
        }
      }
    else if(f.size == 2000) {
      for(int u=0; u<=tim; u++) {
        if((u-tim)%50) continue;
        if(u < tim-250) continue;
        ld rad = u * 3 / 2000.;
        rad = rad * (5-rad) * 1.25;
        rad *= hexf;
        int flashcol = f.color;
        if(u > 1000) flashcol = gradient(flashcol, 0, 1000, u, 2200);
        flashcol = darkena(flashcol, 0, 0xFF);
        for(int a=0; a<=S84; a++) curvepoint(V*ddi0(a, rad));
        queuecurve(flashcol, 0x8080808, PPR_LINE);
        }
      }

    if(kill) {
      f = flashes[size(flashes)-1];
      flashes.pop_back(); k--;
      }
    }
  }

string buildCredits();

string buildHelpText() {
  DEBB(DF_GRAPH, (debugfile,"buildHelpText\n"));
  string h;
  h += XLAT("Welcome to HyperRogue");
#ifdef ANDROID  
  h += XLAT(" for Android");
#endif
#ifdef IOS
  h += XLAT(" for iOS");
#endif
  h += XLAT("! (version %1)\n\n", VER);
  
  h += XLAT(
    "You have been trapped in a strange, non-Euclidean world. Collect as much treasure as possible "
    "before being caught by monsters. The more treasure you collect, the more "
    "monsters come to hunt you, as long as you are in the same land type. The "
    "Orbs of Yendor are the ultimate treasure; get at least one of them to win the game!"
    );
  h += XLAT(" (press ESC for some hints about it).");
  h += "\n\n";
  
  h += XLAT(
    "You can fight most monsters by moving into their location. "
    "The monster could also kill you by moving into your location, but the game "
    "automatically cancels all moves which result in that.\n\n"
    );
    
  h += XLAT(
    "There are many lands in HyperRogue. Collect 10 treasure "
    "in the given land type to complete it; this enables you to "
    "find the magical Orbs of this land, and in some cases "
    "get access to new lands. At 25 treasures "
    "this type of Orbs starts appearing in other lands as well. Press 'o' to "
    "get the details of all the Lands.\n\n");
  h += "\n\n";
    
#ifdef MOBILE
  h += XLAT(
    "Usually, you move by touching somewhere on the map; you can also touch one "
    "of the four buttons on the map corners to change this (to scroll the map "
    "or get information about map objects). You can also touch the "
    "numbers displayed to get their meanings.\n"
    );
#else
  h += XLAT(
    "Move with mouse, num pad, qweadzxc, or hjklyubn. Wait by pressing 's' or '.'. Spin the world with arrows, PageUp/Down, and Home/Space. "
    "To save the game you need an Orb of Safety. Press 'v' for the main menu (configuration, special modes, etc.), ESC for the quest status.\n\n"
    );
  h += XLAT(
    "You can right click any element to get more information about it.\n\n"
    );
  h += XLAT("(You can also use right Shift)\n\n");
#endif
  h += XLAT("See more on the website: ") 
    + "http//roguetemple.com/z/hyper/\n\n";
  
#ifdef TOUR
  h += XLAT("Try the Tutorial to help with understanding the "
    "geometry of HyperRogue (menu -> special modes).\n\n");
#endif
  
  h += XLAT("Still confused? Read the FAQ on the HyperRogue website!\n\n");
  
  return h;
  }

string buildCredits() {
  string h;
  h += XLAT("game design, programming, texts and graphics by Zeno Rogue <zeno@attnam.com>\n\n");
  if(lang() != 0)
    h += XLAT("add credits for your translation here");
#ifndef NOLICENSE
  h += XLAT(
    "released under GNU General Public License version 2 and thus "
    "comes with absolutely no warranty; see COPYING for details\n\n"
    );
#endif
  h += XLAT(
    "special thanks to the following people for their bug reports, feature requests, porting, and other help:\n\n%1\n\n",
    "Konstantin Stupnik, ortoslon, chrysn, Adam Borowski, Damyan Ivanov, Ryan Farnsley, mcobit, Darren Grey, tricosahedron, Maciej Chojecki, Marek Čtrnáct, "
    "wonderfullizardofoz, Piotr Migdał, tehora, Michael Heerdegen, Sprite Guard, zelda0x181e, Vipul, snowyowl0, Patashu, phenomist, Alan Malloy, Tom Fryers, Sinquetica, _monad, CtrlAltDestroy, jruderman"
    );
#ifdef EXTRALICENSE
  h += EXTRALICENSE;
#endif
#ifndef MOBILE
  h += XLAT(
    "\n\nSee sounds/credits.txt for credits for sound effects"
    );
  #endif
  if(musiclicense != "") h += musiclicense;
  return h;
  }

string pushtext(stringpar p) {
  string s = XLAT(
    "\n\nNote: when pushing %the1 off a heptagonal cell, you can control the pushing direction "
    "by clicking left or right half of the heptagon.", p);
#ifndef MOBILE
  s += XLAT(" With the keyboard, you can rotate the view for a similar effect (Page Up/Down).");
#endif
  return s;
  }

string princedesc() {
  if(princessgender() == GEN_M)
    return XLAT("Apparently a prince is kept locked somewhere, but you won't ever find him in this hyperbolic palace. ");
  else
    return XLAT("Apparently a princess is kept locked somewhere, but you won't ever find her in this hyperbolic palace. ");
  }

string helptitle(string s, int col) {
  return "@" + its(col) + "\t" + s + "\n";
  }

string princessReviveHelp() {
  string h = "\n\n" +
    XLAT("Killed %1 can be revived with Orb of the Love, after you collect 20 more $$$.", moPrincess);
  if(princess::reviveAt)
    h += "\n\n" +
    XLAT("%The1 will be revivable at %2 $$$", moPrincess, its(princess::reviveAt));
  return h;
  }

string generateHelpForItem(eItem it) {

   string help = helptitle(XLATN(iinf[it].name), iinf[it].color);
   
   help += XLAT(iinf[it].help);
   
   if(it == itSavedPrincess || it == itOrbLove)
     help += princessReviveHelp();
     
   if(it == itTrollEgg)
     help += XLAT("\n\nAfter the Trolls leave, you have 750 turns to collect %the1, or it gets stolen.", it);
   
   if(it == itIvory || it == itAmethyst || it == itLotus || it == itMutant) {
     help += XLAT(
       "\n\nEasy %1 might disappear when you collect more of its kind.", it);
     if(it != itMutant) help += XLAT(
       " You need to go deep to collect lots of them.");
     }
     
#ifdef MOBILE
   if(it == itOrbSafety)
     help += XLAT("This might be very useful for devices with limited memory.");
#else
   if(it == itOrbSafety)
     help += XLAT("Thus, it is potentially useful for extremely long games, which would eat all the memory on your system otherwise.\n");
#endif

   if(isRangedOrb(it)) {
     help += XLAT("\nThis is a ranged Orb. ");
#ifdef ISMOBILE   
     if(vid.shifttarget&2)
       help += XLAT("\nRanged Orbs can be targeted by long touching the desired location.");
     else
       help += XLAT("\nRanged Orbs can be targeted by touching the desired location.");
#else
     if(vid.shifttarget&1)
       help += XLAT("\nRanged Orbs can be targeted by shift-clicking the desired location. "
     else
       help += XLAT("\nRanged Orbs can be targeted by clicking the desired location. ");
     help += "You can also scroll to the desired location and then press 't'.");
#endif
     help += XLAT("\nYou can never target cells which are adjacent to the player character, or ones out of the sight range.");
     }

#ifdef MOBILE
   if(it == itGreenStone)
     help += XLAT("You can touch the Dead Orb in your inventory to drop it.");
#else
   if(it == itGreenStone)
     help += XLAT("You can press 'g' or click them in the list to drop a Dead Orb.");
#endif
   if(it == itOrbLightning || it == itOrbFlash)
     help += XLAT("\n\nThis Orb is triggered on your first attack or illegal move.");
   if(it == itOrbShield)
     help += XLAT("\n\nThis Orb protects you from attacks, scents, and insulates you "
       "from electricity. It does not let you go through deadly terrain, but "
       "if you are attacked with fire, it lets you stay in place in it.");
  if(it == itOrbEmpathy) {
    int cnt = 0;
    for(int i=0; i<ittypes; i++) {
      eItem it2 = eItem(i);
      if(isEmpathyOrb(it2)) {
        help += XLAT(cnt ? ", %1" : " %1", it2);
        cnt++;
        }
      }
    }
  
  if(itemclass(it) == IC_ORB || it == itGreenStone || it == itOrbYendor) {
    eOrbLandRelation olr = getOLR(it, cwt.c->land);

    for(int i=0; i<ORBLINES; i++) {
      orbinfo& oi(orbinfos[i]);
      if(oi.orb == it) {
        eItem tr = treasureType(oi.l);
        help += "\n\n" + XLAT(olrDescriptions[olr], cwt.c->land, tr, treasureType(cwt.c->land));
        int t = items[tr] * landMultiplier(oi.l);
        if(t >= 25)
        if(olr == olrPrize25 || olr == olrPrize3 || olr == olrGuest || olr == olrMonster || olr == olrAlways) {
          help += XLAT("\nSpawn rate (as prize Orb): %1%/%2\n", 
            its(int(.5 + 100 * orbprizefun(t))),
            its(oi.gchance));
          }
        if(t >= 10)
        if(olr == olrHub) {
          help += XLAT("\nSpawn rate (in Hubs): %1%/%2\n", 
            its(int(.5 + 100 * orbcrossfun(t))),
            its(oi.gchance));
          }
        }
      }
    }

  return help;
  }

void addMinefieldExplanation(string& s) {

  s += XLAT(
    "\n\nOnce you collect 10 Bomberbird Eggs, "
    "stepping on a cell with no adjacent mines also reveals the adjacent cells. "
    "Collecting even more Eggs will increase the radius. Additionally, collecting "
    "25 Bomberbird Eggs will reveal adjacent cells even in your future games."
    );

  s += "\n\n";
#ifdef MOBILE
  s += XLAT("Known mines may be marked by pressing 'm'. Your allies won't step on marked mines.");
#else
  s += XLAT("Known mines may be marked by touching while in drag mode. Your allies won't step on marked mines.");
#endif
  }

string generateHelpForWall(eWall w) {

  string s = helptitle(XLATN(winf[w].name), winf[w].color);
   
  s += XLAT(winf[w].help);
  if(w == waMineMine || w == waMineUnknown || w == waMineOpen)
    addMinefieldExplanation(s);
  if(isThumper(w)) s += pushtext(w);
  if((w == waClosePlate || w == waOpenPlate) && purehepta) 
    s += "\n\n(For the heptagonal mode, the radius has been reduced to 2 for closing plates.)";
  return s;
  }

void buteol(string& s, int current, int req) {
  int siz = size(s);
  if(s[siz-1] == '\n') s.resize(siz-1);
  char buf[100]; sprintf(buf, " (%d/%d)", current, req);
  s += buf; s += "\n";
  }

string generateHelpForMonster(eMonster m) {
  string s = helptitle(XLATN(minf[m].name), minf[m].color);

  s += XLAT(minf[m].help);      
  if(m == moPalace || m == moSkeleton)
    s += pushtext(m);  
  if(m == moTroll) s += XLAT(trollhelp2);  

  if(isMonsterPart(m))
    s += XLAT("\n\nThis is a part of a monster. It does not count for your total kills.", m);

  if(isFriendly(m))
    s += XLAT("\n\nThis is a friendly being. It does not count for your total kills.", m);

  if(m == moTortoise)
    s += XLAT("\n\nTortoises are not monsters! They are just annoyed. They do not count for your total kills.", m);
  
  if(isGhost(m))
    s += XLAT("\n\nA Ghost never moves to a cell which is adjacent to another Ghost of the same kind.", m);
    
  if(m == moBat || m == moEagle)
    s += XLAT("\n\nFast flying creatures may attack or go against gravity only in their first move.", m);

  return s;
  }

string generateHelpForLand(eLand l) {
  string s = helptitle(XLATN(linf[l].name), linf[l].color);
  
  if(l == laPalace) s += princedesc();

  s += XLAT(linf[l].help);

  if(l == laMinefield) addMinefieldExplanation(s);

  s += "\n\n";
  if(l == laIce || l == laCaves || l == laDesert || l == laMotion || l == laJungle ||
    l == laCrossroads || l == laAlchemist)
      s += XLAT("Always available.\n");

  #define ACCONLY(z) s += XLAT("Accessible only from %the1.\n", z);
  #define ACCONLY2(z,x) s += XLAT("Accessible only from %the1 or %the2.\n", z, x);
  #define ACCONLYF(z) s += XLAT("Accessible only from %the1 (until finished).\n", z);
  #define TREQ(z) { s += XLAT("Treasure required: %1 $$$.\n", #z); buteol(s, gold(), z); }
  #define TREQ2(z,x) { s += XLAT("Treasure required: %1 x %2.\n", #z, x); buteol(s, items[x], z); }
  
  if(l == laMirror || l == laMinefield || l == laPalace ||
    l == laOcean || l == laLivefjord || l == laZebra || l == laWarpCoast || l == laWarpSea ||
    l == laReptile || l == laIvoryTower)
      TREQ(30)

  
  if(isCoastal(l)) 
    s += XLAT("Coastal region -- connects inland and aquatic regions.\n");
    
  if(isPureSealand(l)) 
    s += XLAT("Aquatic region -- accessible only from coastal regions and other aquatic regions.\n");
    
  if(l == laWhirlpool) ACCONLY(laOcean)
  if(l == laRlyeh) ACCONLYF(laOcean)
  if(l == laTemple) ACCONLY(laRlyeh)  
  if(l == laClearing) ACCONLY(laOvergrown)  
  if(l == laHaunted) ACCONLY(laGraveyard)  
  if(l == laPrincessQuest) ACCONLY(laPalace)
  if(l == laMountain) ACCONLY(laJungle)
  if(l == laCamelot) ACCONLY2(laCrossroads, laCrossroads3)
  
  if(l == laDryForest || l == laWineyard || l == laDeadCaves || l == laHive || l == laRedRock ||
    l == laOvergrown || l == laStorms || l == laWhirlwind || l == laRose ||
    l == laCrossroads2 || l == laRlyeh)
      TREQ(60)
    
  if(l == laReptile) TREQ2(10, itElixir)
  if(l == laEndorian) TREQ2(10, itIvory)
  if(l == laKraken) TREQ2(10, itFjord)
  if(l == laBurial) TREQ2(10, itKraken)

  if(l == laDungeon) TREQ2(5, itIvory)
  if(l == laDungeon) TREQ2(5, itPalace)
  if(l == laMountain) TREQ2(5, itIvory)
  if(l == laMountain) TREQ2(5, itRuby)
    
  if(l == laPrairie) TREQ(90)
  if(l == laBull) TREQ(90)
  if(l == laCrossroads4) TREQ(200)
  if(l == laCrossroads5) TREQ(300)
  
  if(l == laGraveyard || l == laHive) {
    s += XLAT("Kills required: %1.\n", "100");
    buteol(s, tkills(), 100);
    }
  
  if(l == laDragon) {
    s += XLAT("Different kills required: %1.\n", "20");
    buteol(s, killtypes(), 20);
    }
  
  if(l == laTortoise) ACCONLY(laDragon)
  if(l == laTortoise) s += XLAT("Find a %1 in %the2.", itBabyTortoise, laDragon);

  if(l == laHell || l == laCrossroads3) {
    s += XLAT("Finished lands required: %1 (collect 10 treasure)\n", "9");
    buteol(s, orbsUnlocked(), 9);
    }
  
  if(l == laCocytus || l == laPower) TREQ2(10, itHell)
  if(l == laRedRock) TREQ2(10, itSpice)
  if(l == laOvergrown) TREQ2(10, itRuby)
  if(l == laClearing) TREQ2(5, itMutant)
  if(l == laCocytus) TREQ2(10, itDiamond)
  if(l == laDeadCaves) TREQ2(10, itGold)
  if(l == laTemple) TREQ2(5, itStatue)
  if(l == laHaunted) TREQ2(10, itBone)
  if(l == laCamelot) TREQ2(5, itEmerald)
  if(l == laEmerald) {
    TREQ2(5, itFernFlower) TREQ2(5, itGold)
    s += XLAT("Alternatively: kill a %1 in %the2.\n", moVizier, laPalace);
    buteol(s, kills[moVizier], 1);
    }
  
#define KILLREQ(who, where) { s += XLAT("Kills required: %1 (%2).\n", who, where); buteol(s, kills[who], 1); }

  if(l == laPrincessQuest)
    KILLREQ(moVizier, laPalace);
    
  if(l == laElementalWall) {
    KILLREQ(moFireElemental, laDragon);
    KILLREQ(moEarthElemental, laDeadCaves);
    KILLREQ(moWaterElemental, laLivefjord);
    KILLREQ(moAirElemental, laWhirlwind);
    }
  
  if(l == laTrollheim) {
    KILLREQ(moTroll, laCaves);
    KILLREQ(moFjordTroll, laLivefjord);
    KILLREQ(moDarkTroll, laDeadCaves);
    KILLREQ(moStormTroll, laStorms);
    KILLREQ(moForestTroll, laOvergrown);
    KILLREQ(moRedTroll, laRedRock);
    }
  
  if(l == laZebra) TREQ2(10, itFeather)
  
  if(l == laCamelot || l == laPrincessQuest)
    s += XLAT("Completing the quest in this land is not necessary for the Hyperstone Quest.");

  int rl = isRandland(l);
  if(rl == 2)
    s += XLAT("Variants of %the1 are always available in the Random Pattern Mode.", l);
  else if(rl == 1)
    s += XLAT(
      "Variants of %the1 are available in the Random Pattern Mode after "
      "getting a highscore of at least 10 %2.", l, treasureType(l));

  if(l == laPrincessQuest) {
    s += XLAT("Unavailable in the shmup mode.\n");
    s += XLAT("Unavailable in the multiplayer mode.\n");
    }
  
  if(noChaos(l))
    s += XLAT("Unavailable in the Chaos mode.\n");
  
  if(l == laWildWest)
    s += XLAT("Bonus land, available only in some special modes.\n");
  
  if(l == laWhirlpool)
    s += XLAT("Orbs of Safety always appear here, and may be used to escape.\n");

  /* if(isHaunted(l) || l == laDungeon)
    s += XLAT("You may be unable to leave %the1 if you are not careful!\n", l); */
    
  if(l == laStorms) {
    if(elec::lightningfast == 0) s += XLAT("\nSpecial conduct (still valid)\n");
    else s += XLAT("\nSpecial conduct failed:\n");
    
    s += XLAT(
      "Avoid escaping from a discharge (\"That was close\").");
    }

  if(isHaunted(l)) {
    if(survivalist) s += XLAT("\nSpecial conduct (still valid)\n");
    else s += XLAT("\nSpecial conduct failed:\n");

    s += XLAT(
      "Avoid chopping trees, using Orbs, and non-graveyard monsters in the Haunted Woods."
      );
    }
  
#ifndef ISMOBILE
  if(l == laCA)
    s += XLAT("\n\nHint: use 'm' to toggle cells quickly");
#endif

  return s;
  }

bool instat;

void describeMouseover() {
  DEBB(DF_GRAPH, (debugfile,"describeMouseover\n"));

  cell *c = mousing ? mouseover : playermoved ? NULL : centerover;
  string out = mouseovers;
  if(!c || instat || getcstat) { }
  else if(cmode == emNormal || cmode == emQuit || cmode == emMapEditor) {
    out = XLAT1(linf[c->land].name);
    help = generateHelpForLand(c->land);
        
    // Celsius
    
    // if(c->land == laIce) out = "Icy Lands (" + fts(60 * (c->heat - .4)) + " C)";
    
    if(c->land == laIce || c->land == laCocytus) 
      out += " (" + fts(heat::celsius(c)) + " °C)";
    if(c->land == laDryForest && c->landparam) 
      out += " (" + its(c->landparam)+"/10)";
    if(c->land == laOcean && chaosmode)
      out += " (" + its(c->CHAOSPARAM)+"S"+its(c->SEADIST)+"L"+its(c->LANDDIST)+")";
    else if(c->land == laOcean && c->landparam <= 25) {
      if(shmup::on)
        out += " (" + its(c->landparam)+")";
      else {
        bool b = c->landparam >= tide[(turncount-1) % tidalsize];
        int t = 1;
        for(; t < 1000 && b == (c->landparam >= tide[(turncount+t-1) % tidalsize]); t++) ;
        if(b)
          out += " (" + its(t) + " turns to surface)";
        else 
          out += " (" + its(t) + " turns to submerge)";
        }
      }

    if(c->land == laTortoise && tortoise::seek()) out += " " + tortoise::measure(getBits(c));

    /* if(c->land == laGraveyard || c->land == laHauntedBorder || c->land == laHaunted)
      out += " (" + its(c->landparam)+")"; */
    
    if(buggyGeneration) {
      char buf[20]; sprintf(buf, " H=%d M=%d", c->landparam, c->mpdist); out += buf;
      }
    
//  if(c->land == laBarrier)
//    out += "(" + string(linf[c->barleft].name) + " / " + string(linf[c->barright].name) + ")";

    // out += "(" + its(c->bardir) + ":" + string(linf[c->barleft].name) + " / " + string(linf[c->barright].name) + ")";
    
    // out += " MD"+its(c->mpdist);

    // out += " WP:" + its(c->wparam);
    // out += " rose:" + its(rosemap[c]/4) + "." + its(rosemap[c]%4);
    // out += " MP:" + its(c->mpdist);
    // out += " cda:" + its(celldistAlt(c));
    
    /* out += " DP=" + its(celldistance(c, cwt.c));
    out += " DO=" + its(celldist(c));
    out += " PD=" + its(c->pathdist); */
    if(webdisplay & 8) {

      out += " LP:" + itsh(c->landparam)+"/"+its(turncount);

      out += " CD:" + its(celldist(c));
      
      out += " D:" + its(c->mpdist);
      
      char zz[64]; sprintf(zz, " P%p", c); out += zz;
      // out += " rv" + its(rosedist(c));
  //  if(rosemap.count(c))
  //    out += " rv " + its(rosemap[c]/8) + "." + its(rosemap[c]%8);
  //  out += " ai" + its(c->aitmp);
      if(euclid) {
        for(int i=0; i<4; i++) out += " " + its(getEuclidCdata(c->master)->val[i]);
        out += " " + itsh(getBits(c));
        }
      else {
        for(int i=0; i<4; i++) out += " " + its(getHeptagonCdata(c->master)->val[i]);
  //  out += " " + itsh(getHeptagonCdata(c->master)->bits);
        out += " " + fts(tortoise::getScent(getBits(c)));
        }
      // itsh(getHeptagonCdata(c->master)->bits);
  //  out += " barleft: " + s0 + dnameof(c->barleft);
  //  out += " barright: " + s0 + dnameof(c->barright);
      }
    
    // char zz[64]; sprintf(zz, " P%p", c); out += zz;
    
    /* whirlwind::calcdirs(c);
    for(int i=0; i<whirlwind::qdirs; i++) 
      out += " " + its(whirlwind::dfrom[i]) + ":" + its(whirlwind::dto[i]); */
    // out += " : " + its(whirlwinddir(c));
    
    

    if(randomPatternsMode)
      out += " " + describeRPM(c->land);
      
    if(euclid && cheater) {
      eucoord x, y;
      decodeMaster(c->master, x, y);
      out += " ("+its(short(x))+","+its(short(y))+")";
      }
      
    // char zz[64]; sprintf(zz, " P%d", princess::dist(c)); out += zz;
    // out += " MD"+its(c->mpdist);
    // out += " H "+its(c->heat);
    // if(c->type != 6) out += " Z"+its(c->master->zebraval);
    // out += " H"+its(c->heat);
    
/*  // Hive debug
    if(c->land == laHive) {
      out += " [" + its(c->tmp) + " H" + its(int(c->heat));
      if(c->tmp >= 0 && c->tmp < size(buginfo) && buginfo[c->tmp].where == c) {
        buginfo_t b(buginfo[c->tmp]);
        for(int k=0; k<3; k++) out += ":" + its(b.dist[k]);
        for(int k=0; k<3; k++) 
        for(int i=0; i<size(bugqueue[k]); i++)
          if(bugqueue[k][i] == c->tmp)
            out += " B"+its(k)+":"+its(i);
        }
      out += "]";
      } */
  
    if(c->wall && 
      !((c->wall == waFloorA || c->wall == waFloorB || c->wall == waFloorC || c->wall == waFloorD) && c->item)) { 
      out += ", "; out += XLAT1(winf[c->wall].name); 
      
      if(c->wall == waRose) out += " (" + its(7-rosephase) + ")";
      
      if((c->wall == waBigTree || c->wall == waSmallTree) && c->land != laDryForest)
        help = 
          "Trees in this forest can be chopped down. Big trees take two turns to chop down.";
      else if(c->wall != waSea && c->wall != waPalace)
      if(!((c->wall == waCavefloor || c->wall == waCavewall) && c->land == laEmerald))
        help = generateHelpForWall(c->wall);
      }
    
    if(isActivable(c)) out += XLAT(" (touch to activate)");
    
    if(hasTimeout(c)) out += XLAT(" [%1 turns]", its(c->wparam));
    
    if(isReptile(c->wall))
      out += XLAT(" [%1 turns]", its((unsigned char) c->wparam));
  
    if(c->monst) {
      out += ", "; out += XLAT1(minf[c->monst].name); 
      if(hasHitpoints(c->monst))
        out += " (" + its(c->hitpoints)+" HP)";
      if(isMutantIvy(c))
        out += " (" + its((c->stuntime - mutantphase) & 15) + "*)";
      else if(c->stuntime)
        out += " (" + its(c->stuntime) + "*)";

      if(c->monst == moTortoise && tortoise::seek()) 
        out += " " + tortoise::measure(tortoise::getb(c));

      help = generateHelpForMonster(c->monst);
      }
  
    if(c->item && !itemHiddenFromSight(c)) {
      out += ", "; 
      out += XLAT1(iinf[c->item].name); 
      if(c->item == itBarrow) out += " (x" + its(c->landparam) + ")";
      if(c->item == itBabyTortoise && tortoise::seek()) 
        out += " " + tortoise::measure(tortoise::babymap[c]);
      if(!c->monst) help = generateHelpForItem(c->item);
      }
    
    if(isPlayerOn(c) && !shmup::on) out += XLAT(", you");

    if(shmup::mousetarget && intval(mouseh, tC0(shmup::mousetarget->pat)) < .1) {
      out += ", "; 
#ifdef ROGUEVIZ
      if(shmup::mousetarget->type == moRogueviz) {
        help = XLAT(minf[shmup::mousetarget->type].help);
        out += rogueviz::describe(shmup::mousetarget);
        }
      else 
#endif
      {
        out += XLAT1(minf[shmup::mousetarget->type].name);
        help = XLAT(minf[shmup::mousetarget->type].help);
        }
/*    char buf[64];
      sprintf(buf, "%Lf", intval(mouseh, shmup::mousetarget->pat*C0));
      mouseovers = mouseovers + " D: " + buf;
      printf("ms = %s\n", mouseovers.c_str());*/
      }

    if(rosedist(c) == 1)
      out += ", wave of scent (front)";

    if(rosedist(c) == 2)
      out += ", wave of scent (back)";
    
    if(sword::at(c)) out += ", Energy Sword";
    
    if(rosedist(c) || c->land == laRose || c->wall == waRose)
      help += s0 + "\n\n" + rosedesc;
    
    if(isWarped(c) && !isWarped(c->land))
      out += ", warped";

    if(isWarped(c)) 
      help += s0 + "\n\n" + warpdesc;

    }
  else if(cmode == emVisual1) {
    if(getcstat == 'p') {
      out = XLAT("0 = Klein model, 1 = Poincaré model");
      if(vid.alpha < -0.5)
        out = XLAT("you are looking through it!");
      if(vid.alpha > 5)
        out = XLAT("(press 'i' to approach infinity (Gans model)");
      }
    else if(getcstat == 'r') {
      out = XLAT("simply resize the window to change resolution");
      }
    /* else if(getcstat == 'f') {
      out = XLAT("[+] keep the window size, [-] use the screen resolution");
      } */
    else if(getcstat == 'a' && vid.sspeed > -4.99)
      out = XLAT("+5 = center instantly, -5 = do not center the map");
    else if(getcstat == 'a')
      out = XLAT("press Space or Home to center on the PC");
    else if(getcstat == 'w')
      out = XLAT("also hold Alt during the game to toggle high contrast");
    else if(getcstat == 'w' || getcstat == 'm')
      out = XLAT("You can choose one of the several modes");
    else if(getcstat == 'c')
      out = XLAT("The axes help with keyboard movement");
    else if(getcstat == 'g')
      out = XLAT("Affects looks and grammar");
#ifndef MOBILE
    else if(getcstat == 's')
      out = XLAT("Config file: %1", conffile);
#endif
    else out = "";
    }
  else if(cmode == emVisual2) {
    if(getcstat == 'p') {
      if(autojoy) 
        out = XLAT("joystick mode: automatic (release the joystick to move)");
      if(!autojoy) 
        out = XLAT("joystick mode: manual (press a button to move)");
      }
    else if(getcstat == 'e')
      out = XLAT("You need special glasses to view the game in 3D");
    else if(getcstat == 'f')
      out = XLAT("Reduce the framerate limit to conserve CPU energy");
    }
  else if(cmode == emChangeMode) {
    if(getcstat == 'h')
      out = XLAT("One wrong move and it is game over!");
    }
    
  mouseovers = out;
  
  int col = linf[cwt.c->land].color;
  if(cwt.c->land == laRedRock) col = 0xC00000;

#ifndef MOBILE
  displayfr(vid.xres/2, vid.fsize,   2, vid.fsize, out, col, 8);
#endif

  if(mousey < vid.fsize * 3/2) getcstat = SDLK_F1;

  if(false && shmup::mousetarget) {
    char buf[64];
    sprintf(buf, "%Lf", (long double) intval(mouseh, tC0(shmup::mousetarget->pat)));
    mouseovers = mouseovers + " D: " + buf;
    return;
    }
  }

void drawrec(const heptspin& hs, int lev, hstate s, const transmatrix& V) {

  // shmup::calc_relative_matrix(cwt.c, hs.h);
    
  cell *c = hs.h->c7;
  
  transmatrix V10;
  const transmatrix& V1 = hs.mirrored ? (V10 = V * Mirror) : V;
  
  if(dodrawcell(c)) {
    reclevel = maxreclevel - lev;
    drawcell(c, (hs.spin || purehepta) ? V1 * spin(hs.spin*2*M_PI/S7 + (purehepta ? M_PI:0)) : V1, hs.spin,
      hs.mirrored);
    }
  
  if(lev <= 0) return;
  
  if(!purehepta) for(int d=0; d<S7; d++) {
    int ds = fixrot(hs.spin + d);
    reclevel = maxreclevel - lev + 1;
    // createMov(c, ds);
    if(c->mov[ds] && c->spn(ds) == 0 && dodrawcell(c->mov[ds])) {
      drawcell(c->mov[ds], V1 * hexmove[d], 0, hs.mirrored ^ c->mirror(ds));
      }
    }

  if(lev <= 1) return;
  
  for(int d=0; d<S7; d++) {
    hstate s2 = transition(s, d);
    if(s2 == hsError) continue;
    heptspin hs2 = hsstep(hsspin(hs, d), 0);
    drawrec(hs2, lev-2, s2, V * heptmove[d]);
    }
  
  }

int mindx=-7, mindy=-7, maxdx=7, maxdy=7;
  
transmatrix eumove(int x, int y) {
  transmatrix Mat = Id;
  Mat[2][2] = 1;
  Mat[0][2] += (x + y * .5) * eurad;
  // Mat[2][0] += (x + y * .5) * eurad;
  Mat[1][2] += y * q3 /2 * eurad;
  // Mat[2][1] += y * q3 /2 * eurad;
  while(Mat[0][2] <= -16384 * eurad) Mat[0][2] += 32768 * eurad;
  while(Mat[0][2] >= 16384 * eurad) Mat[0][2] -= 32768 * eurad;
  while(Mat[1][2] <= -16384 * q3 * eurad) Mat[1][2] += 32768 * q3 * eurad;
  while(Mat[1][2] >= 16384 * q3 * eurad) Mat[1][2] -= 32768 * q3 * eurad;
  return Mat;
  }

void drawEuclidean() {
  DEBB(DF_GRAPH, (debugfile,"drawEuclidean\n"));
  eucoord px, py;
  if(!centerover) centerover = cwt.c;
  // printf("centerover = %p player = %p [%d,%d]-[%d,%d]\n", lcenterover, cwt.c,
  //   mindx, mindy, maxdx, maxdy);
  decodeMaster(centerover->master, px, py);
  
  int minsx = mindx-1, maxsx=maxdx+1, minsy=mindy-1, maxsy=maxdy+1;
  mindx=maxdx=mindy=maxdy=0;
  
  for(int dx=minsx; dx<=maxsx; dx++)
  for(int dy=minsy; dy<=maxsy; dy++) {
    eucoord x = dx+px;
    eucoord y = dy+py;
    reclevel = eudist(dx, dy);
    cell *c = euclideanAt(x,y);
    if(!c) continue;
    transmatrix Mat = eumove(x, y);
    Mat = View * Mat;
    
    // Mat[0][0] = -1;
    // Mat[1][1] = -1;
    
    // Mat[2][0] = x*x/10;
    // Mat[2][1] = y*y/10;
    // Mat = Mat * xpush(x-30) * ypush(y-30);

    int cx, cy, shift;
    getcoord0(tC0(Mat), cx, cy, shift);
    if(cx >= 0 && cy >= 0 && cx < vid.xres && cy < vid.yres) {
      if(dx < mindx) mindx = dx;
      if(dy < mindy) mindy = dy;
      if(dx > maxdx) maxdx = dx;
      if(dy > maxdy) maxdy = dy;
      }
    
    if(dodrawcell(c)) {
      drawcell(c, Mat, 0, false);
      }
    }
  }

void drawthemap() {

  frameid++;

  if(!cheater && !svg::in && !inHighQual) {
    if(sightrange > 7) sightrange = 7;
    overgenerate = false;
    }
  
  profile_frame();
  profile_start(0);
  swap(gmatrix0, gmatrix);
  gmatrix.clear();

  wmspatial = vid.wallmode == 4 || vid.wallmode == 5;
  wmescher = vid.wallmode == 3 || vid.wallmode == 5;
  wmplain = vid.wallmode == 2 || vid.wallmode == 4;
  wmascii = vid.wallmode == 0;
  wmblack = vid.wallmode == 1;
  
  mmitem = vid.monmode >= 1;
  mmmon = vid.monmode >= 2;
  mmhigh = vid.monmode == 3 || vid.monmode == 5;
  mmspatial = vid.monmode == 4 || vid.monmode == 5;

  DEBB(DF_GRAPH, (debugfile,"draw the map\n"));
  fanframe = ticks / (purehepta ? 300 : 150.0) / M_PI;
  
  for(int m=0; m<motypes; m++) if(isPrincess(eMonster(m))) 
    minf[m].name = princessgender() ? "Princess" : "Prince";
    
  iinf[itSavedPrincess].name = minf[moPrincess].name;

  for(int i=0; i<NUM_GS; i++) {
    genderswitch_t& g = genderswitch[i];
    if(g.gender != princessgender()) continue;
    minf[g.m].help = g.desc;
    minf[g.m].name = g.name;
    }

  keycell = NULL;

  pirateTreasureFound = pirateTreasureSeek;
  pirateTreasureSeek = NULL;
  straightDownSeek = NULL; downspin = 0;
  shmup::mousetarget = NULL;
  showPirateX = false;
  for(int i=0; i<numplayers(); i++) if(multi::playerActive(i))
    if(playerpos(i)->item == itCompass) showPirateX = true;
    
  using namespace yendor;
  
  if(yii < size(yi)) {
    if(!yi[yii].found) 
      for(int i=0; i<YDIST; i++) 
        if(yi[yii].path[i]->cpdist <= sightrange) {
      keycell = yi[yii].path[i];
      keycelldist = YDIST - i;
      }                                                                
    }
  
  modist = 1e20; mouseover = NULL; 
  modist2 = 1e20; mouseover2 = NULL; 
  mouseovers = XLAT("Press F1 or right click for help");
#ifdef ROGUEVIZ
  if(rogueviz::on) mouseovers = " ";
#endif
#ifdef TOUR
  if(tour::on) mouseovers = tour::tourhelp;
#endif
  centdist = 1e20; centerover = NULL; 

  for(int i=0; i<multi::players; i++) {
    multi::ccdist[i] = 1e20; multi::ccat[i] = NULL;
    }

  #ifdef MOBILE
  mouseovers = XLAT("No info about this...");
  #endif
  if(outofmap(mouseh)) 
    modist = -5;
  playerfound = false;
  // playerfoundL = false;
  // playerfoundR = false;

  sphereflip = Id;
  profile_start(1);
  if(euclid)
    drawEuclidean();
  else {
    if(sphere && vid.alpha > 1) sphereflip[2][2] = -1;
    maxreclevel = 
      conformal::on ? sightrange + 2:
      (!playermoved) ? sightrange+1 : sightrange + 4;
    
    drawrec(viewctr, 
      maxreclevel,
      hsOrigin, ypush(vid.yshift) * sphereflip * View);
    }
  
  #ifdef ROGUEVIZ
  rogueviz::drawExtra();
  #endif

  #ifdef TOUR
  if(tour::on) tour::presentation(2);
  #endif
  
  profile_stop(1);
  profile_start(4);
  drawMarkers();
  profile_stop(4);
  drawFlashes();
  
  if(multi::players > 1 && !shmup::on) {
    if(shmup::centerplayer != -1) 
      cwtV = multi::whereis[shmup::centerplayer];
    else {
      hyperpoint h;
      for(int i=0; i<3; i++) h[i] = 0;
      for(int p=0; p<multi::players; p++) if(multi::playerActive(p)) {
        hyperpoint h1 = tC0(multi::whereis[p]);
        for(int i=0; i<3; i++) h[i] += h1[i];
        }
      h = mid(h, h);
      cwtV = rgpushxto0(h);
      }
    }
  
  if(shmup::on) {
    if(shmup::players == 1)
      cwtV = shmup::pc[0]->pat;
    else if(shmup::centerplayer != -1) 
      cwtV = shmup::pc[shmup::centerplayer]->pat;
    else {
      hyperpoint h;
      for(int i=0; i<3; i++) h[i] = 0;
      for(int p=0; p<multi::players; p++) {
        hyperpoint h1 = tC0(shmup::pc[p]->pat);
        for(int i=0; i<3; i++) h[i] += h1[i];
        }
      h = mid(h, h);
      cwtV = rgpushxto0(h);
      }
    }

  #ifndef MOBILE
  Uint8 *keystate = SDL_GetKeyState(NULL);
  lmouseover = mouseover;
  bool useRangedOrb = (!(vid.shifttarget & 1) && haveRangedOrb() && lmouseover && lmouseover->cpdist > 1) || (keystate[SDLK_RSHIFT] | keystate[SDLK_LSHIFT]);
  if(!useRangedOrb && cmode != emMapEditor && DEFAULTCONTROL && !outofmap(mouseh)) {
    void calcMousedest();
    calcMousedest();
    cellwalker cw = cwt; bool f = flipplayer;
    items[itWarning]+=2;
    
    bool recorduse[ittypes];
    for(int i=0; i<ittypes; i++) recorduse[i] = orbused[i];
    movepcto(mousedest.d, mousedest.subdir, true);
    for(int i=0; i<ittypes; i++) orbused[i] = recorduse[i];
    items[itWarning] -= 2;
    if(multi::players == 1 && cw.spin != cwt.spin) mirror::spin(-mousedest.d);
    cwt = cw; flipplayer = f;
    lmouseover = mousedest.d >= 0 ? cwt.c->mov[(cwt.spin + mousedest.d) % cwt.c->type] : cwt.c;
    }
  #endif
  profile_stop(0);
  }

void spinEdge(ld aspd) { 
  if(downspin >  aspd) downspin =  aspd;
  if(downspin < -aspd) downspin = -aspd;
  View = spin(downspin) * View;
  }

void centerpc(ld aspd) { 
  if(vid.sspeed >= 4.99) aspd = 1000;
  DEBB(DF_GRAPH, (debugfile,"center pc\n"));
  hyperpoint H = ypush(-vid.yshift) * sphereflip * tC0(cwtV);
  if(H[0] == 0 && H[1] == 0) return; // either already centered or direction unknown
  ld R = hdist0(H); // = sqrt(H[0] * H[0] + H[1] * H[1]);
  if(R < 1e-9) {
    /* if(playerfoundL && playerfoundR) {
      
      } */
    spinEdge(aspd);
    fixmatrix(View);
    return;
    }
  
  if(euclid) {
    // Euclidean
    aspd *= (2+3*R*R);
    if(aspd > R) aspd = R;
  
    View[0][2] -= cwtV[0][2] * aspd / R;
    View[1][2] -= cwtV[1][2] * aspd / R;
    }
  
  else {
    aspd *= (1+R+(shmup::on?1:0));

    if(R < aspd) {
      View = gpushxto0(H) * View;
      }
    else 
      View = rspintox(H) * xpush(-aspd) * spintox(H) * View;
      
    fixmatrix(View);
    spinEdge(aspd);
    }
  }

void drawmovestar(double dx, double dy) {

  if(viewdists) return;

  DEBB(DF_GRAPH, (debugfile,"draw movestar\n"));
  if(!playerfound) return;
  
  if(shmup::on) return;
#ifndef NORUG
  if(rug::rugged && multi::players == 1 && !multi::alwaysuse) return;
#endif

  hyperpoint H = tC0(cwtV);
  ld R = sqrt(H[0] * H[0] + H[1] * H[1]);
  transmatrix Centered = Id;

  if(euclid) 
    Centered = eupush(H[0], H[1]);
  else if(R > 1e-9) Centered = rgpushxto0(H);
  
  Centered = Centered * rgpushxto0(hpxy(dx*5, dy*5));
  if(multi::cpid >= 0) multi::crosscenter[multi::cpid] = Centered;
  
  int rax = vid.axes;
  if(rax == 1) rax = drawstaratvec(dx, dy) ? 2 : 0;
  
  if(rax == 0 || vid.axes == 4) return;

  int starcol = getcs().uicolor;
  
  if(vid.axes == 3)
    queuepoly(Centered, shMovestar, starcol);
  
  else for(int d=0; d<8; d++) {
    int col = starcol;
#ifdef PANDORA
    if(leftclick && (d == 2 || d == 6 || d == 1 || d == 7)) col &= 0xFFFFFF3F;
    if(rightclick && (d == 2 || d == 6 || d == 3 || d == 5)) col &= 0xFFFFFF3F;
    if(!leftclick && !rightclick && (d&1)) col &= 0xFFFFFF3F;
#endif
//  EUCLIDEAN
    if(euclid)
      queueline(tC0(Centered), Centered * ddi0(d * 10.5, 0.5) , col, 0);
    else
//    queueline(tC0(Centered), Centered * spin(M_PI*d/4)* xpush(d==0?.7:d==2?.6:.5) * C0, col >> darken);
      queueline(tC0(Centered), Centered * xspinpush0(M_PI*d/4, d==0?.7:d==2?.5:.2), col, 3);
    }
  }

void optimizeview() {
  
  DEBB(DF_GRAPH, (debugfile,"optimize view\n"));
  int turn = 0;
  ld best = INF;
  
  transmatrix TB = Id;
  
  for(int i=-1; i<S7; i++) {

    ld trot = -i * M_PI * 2 / (S7+.0);
    transmatrix T = i < 0 ? Id : spin(trot) * xpush(tessf) * pispin;
    hyperpoint H = View * tC0(T);
    if(H[2] < best) best = H[2], turn = i, TB = T;
    }
  
  if(turn >= 0) {
    View = View * TB;
    fixmatrix(View);
    viewctr = hsspin(viewctr, turn);
    viewctr = hsstep(viewctr, 0);
    }
  }

movedir vectodir(const hyperpoint& P) {

  hyperpoint H = sphereflip * tC0(cwtV);
  ld R = sqrt(H[0] * H[0] + H[1] * H[1]);
  transmatrix Centered = sphereflip * cwtV;
  if(!euclid)
    Centered = gpushxto0(H) * Centered;
  else if(R > 1e-9)
    Centered = eupush(-H[0], -H[1]) * Centered;
  ld binv = 99;
  
  ld dirdist[7];
  for(int i=0; i<cwt.c->type; i++) {
    dirdist[i] = intval(Centered * xspinpush0(-i * 2 * M_PI /cwt.c->type, .5), P);
    }
    
  movedir res;
  res.d = -1;
  
  for(int i=0; i<cwt.c->type; i++) {
    if(dirdist[i] < binv) {
      binv = dirdist[i];
      res.d = i;
      res.subdir = dirdist[(i+1)%cwt.c->type] < dirdist[(i+cwt.c->type-1)%cwt.c->type] ? 1 : -1;
      if(sphere) res.subdir = -res.subdir;
      }
    }
  
  // if(euclid) bdir = (bdir + 3) % 6;
  return res;
  }

void movepckeydir(int d) {
  DEBB(DF_GRAPH, (debugfile,"movepckeydir\n"));
  // EUCLIDEAN

  movedir md = 
    vectodir(spin(-d * M_PI/4) * tC0(pushone()));
    
  movepcto(md);
  }

void calcMousedest() {
  if(outofmap(mouseh)) return;
  if(revcontrol == true) { mouseh[0] = -mouseh[0]; mouseh[1] = -mouseh[1]; }
  ld mousedist = intval(mouseh, tC0(shmup::ggmatrix(cwt.c)));
  mousedest.d = -1;
  
  cellwalker bcwt = cwt;
  
  ld dists[7];
  
  for(int i=0; i<cwt.c->type; i++)
    dists[i] = intval(mouseh, tC0(shmup::ggmatrix(cwt.c->mov[i])));
  
  /* printf("curcell = %Lf\n", mousedist);
  for(int i=0; i<cwt.c->type; i++)
    printf("d%d = %Lf\n", i, dists[i]); */

  for(int i=0; i<cwt.c->type; i++) if(dists[i] < mousedist) {
    mousedist = dists[i];
    mousedest.d = fixdir(i - cwt.spin, cwt.c);
    
    mousedest.subdir =
       dists[(i+1)%cwt.c->type] < dists[(i+cwt.c->type-1)%cwt.c->type] ? 1 : -1;

    if(cwt.mirrored) 
      mousedest.d = fixdir(-mousedest.d, cwt.c), 
      mousedest.subdir = -mousedest.subdir;
    
    if(sphere) mousedest.subdir = -mousedest.subdir;
    }
  
  if(revcontrol == true) { mouseh[0] = -mouseh[0]; mouseh[1] = -mouseh[1]; }
  cwt = bcwt;
  }

void mousemovement() {
  calcMousedest();
  movepcto(mousedest);
  lmouseover = NULL;
  }

long double sqr(long double x) { return x*x; }

// old style joystick control

void checkjoy() {
  DEBB(DF_GRAPH, (debugfile,"check joy\n"));
  if(!DEFAULTCONTROL) return;
  ld joyvalue1 = sqr(vid.joyvalue);
  ld joyvalue2 = sqr(vid.joyvalue2);
  
  ld jx = joyx;
  ld jy = joyy;
  ld sq = jx*jx+jy*jy;

  static int laststate = 0;
  int curstate = sq < joyvalue1 ? 0 : sq < joyvalue2 ? 1 : 2;
  if(curstate != laststate) flashMessages(), laststate = curstate;
  
  if(autojoy) {
    if(sq < joyvalue1) { if(joydir.d >= 0) movepcto(joydir); joydir.d = -1; return; }
    if(sq < joyvalue2 && joydir.d == -1) return;
    }
  else {
    if(sq < joyvalue1) { joydir.d = -1; return; }
    }
  
  joydir = vectodir(hpxy(jx, jy));
  }

void checkpanjoy(double t) {
 
  if(shmup::on) return;
  
  if(vid.joypanspeed < 1e-7) return;
  
  if(sqr(panjoyx) + sqr(panjoyy) < sqr(vid.joypanthreshold))
    return;
  
  ld jx = panjoyx * t * vid.joypanspeed;
  ld jy = panjoyy * t * vid.joypanspeed;
  
  playermoved = false;
  View = gpushxto0(hpxy(jx, jy)) * View;
  }

int realradius;

bool sidescreen;

void calcparam() {
  DEBB(DF_GRAPH, (debugfile,"calc param\n"));
  vid.xcenter = vid.xres / 2;
  vid.ycenter = vid.yres / 2;
  
  realradius = min(vid.xcenter, vid.ycenter);

  vid.radius = int(vid.scale * vid.ycenter) - (ISANDROID ? 2 : ISIOS ? 40 : 40);
  
  realradius = min(realradius, vid.radius);
  
  sidescreen = false;
  
  if(vid.xres < vid.yres) {
    vid.radius = int(vid.scale * vid.xcenter) - (ISIOS ? 10 : 2);
    vid.ycenter = vid.yres - realradius - vid.fsize - (ISIOS ? 10 : 0);
    }
  else {
    if(vid.xres >= vid.yres * 5/4-16 && dialog::sidedialog && cmode == emNumber) 
      sidescreen = true;
    if(viewdists && cmode == emNormal && vid.xres > vid.yres) sidescreen = true;
    if(sidescreen) vid.xcenter = vid.yres/2;
    }

  ld eye = vid.eye; if(pmodel || rug::rugged) eye = 0;
  vid.beta = 1 + vid.alpha + eye;
  vid.alphax = vid.alpha + eye;
  vid.goteyes = vid.eye > 0.001 || vid.eye < -0.001;
  vid.goteyes2 = vid.goteyes;
  }

void displayButton(int x, int y, const string& name, int key, int align, int rad) {
  if(displayfr(x, y, rad, vid.fsize, name, 0x808080, align)) {
    displayfr(x, y, rad, vid.fsize, name, 0xFFFF00, align);
    getcstat = key;
    }
  }

bool displayButtonS(int x, int y, const string& name, int col, int align, int size) {
  if(displaystr(x, y, 0, size, name, col, align)) {
    displaystr(x, y, 0, size, name, 0xFFFF00, align);
    return true;
    }
  else return false;
  }

void displayColorButton(int x, int y, const string& name, int key, int align, int rad, int color, int color2) {
  if(displayfr(x, y, rad, vid.fsize, name, color, align)) {
    if(color2) displayfr(x, y, rad, vid.fsize, name, color2, align);
    getcstat = key;
    }
  }

#ifndef MOBILE
void quitOrAgain() {
  int y = vid.yres * (618) / 1000;
  displayButton(vid.xres/2, y + vid.fsize*1/2, 
    (items[itOrbSafety] && havesave) ? 
      XLAT("Press Enter or F10 to save") : 
      XLAT("Press Enter or F10 to quit"), 
    SDLK_RETURN, 8, 2);
  displayButton(vid.xres/2, y + vid.fsize*2, XLAT("or 'r' or F5 to restart"), 'r', 8, 2);
  displayButton(vid.xres/2, y + vid.fsize*7/2, XLAT("or 't' to see the top scores"), 't', 8, 2);
  displayButton(vid.xres/2, y + vid.fsize*10/2, XLAT("or 'v' to see the main menu"), 'v', 8, 2);
  displayButton(vid.xres/2, y + vid.fsize*13/2, XLAT("or 'o' to see the world overview"), 'o', 8, 2);
  }
#endif

int calcfps() {
  #define CFPS 30
  static int last[CFPS], lidx = 0;
  int ct = ticks;
  int ret = ct - last[lidx];
  last[lidx] = ct;
  lidx++; lidx %= CFPS;
  if(ret == 0) return 0;
  return (1000 * CFPS) / ret;
  }

int msgscroll = 0;

string timeline() {
    int timespent = (int) (savetime + (timerstopped ? 0 : (time(NULL) - timerstart)));
  char buf[20];
  sprintf(buf, "%d:%02d", timespent/60, timespent % 60);
  return 
    shmup::on ? 
      XLAT("%1 knives (%2)", its(turncount), buf)
    :
      XLAT("%1 turns (%2)", its(turncount), buf);
  }

void showGameover() {

  dialog::init(
#ifdef TOUR
    tour::on ? (canmove ? XLAT("Tutorial") : XLAT("GAME OVER")) :
#endif
    cheater ? XLAT("It is a shame to cheat!") : 
    showoff ? XLAT("Showoff mode") :
    canmove && princess::challenge ? XLAT("%1 Challenge", moPrincess) :
    canmove ? XLAT("Quest status") : 
    XLAT("GAME OVER"), 
    0xC00000, 200, 100
    );
  dialog::addInfo(XLAT("Your score: %1", its(gold())));
  dialog::addInfo(XLAT("Enemies killed: %1", its(tkills())));

  if(tour::on) ;
  else if(items[itOrbYendor]) {
    dialog::addInfo(XLAT("Orbs of Yendor found: %1", its(items[itOrbYendor])), iinf[itOrbYendor].color);
    dialog::addInfo(XLAT("CONGRATULATIONS!"), iinf[itOrbYendor].color);
    }
  else {
    if(princess::challenge) 
      dialog::addInfo(XLAT("Follow the Mouse and escape with %the1!", moPrincess));
    else if(gold() < 30)
      dialog::addInfo(XLAT("Collect 30 $$$ to access more worlds"));
    else if(gold() < 60)
      dialog::addInfo(XLAT("Collect 60 $$$ to access even more lands"));
    else if(!hellUnlocked())
      dialog::addInfo(XLAT("Collect at least 10 treasures in each of 9 types to access Hell"));
    else if(items[itHell] < 10)
      dialog::addInfo(XLAT("Collect at least 10 Demon Daisies to find the Orbs of Yendor"));
    else if(size(yendor::yi) == 0)
      dialog::addInfo(XLAT("Look for the Orbs of Yendor in Hell or in the Crossroads!"));
    else 
      dialog::addInfo(XLAT("Unlock the Orb of Yendor!"));
    }
  
  if(!timerstopped && !canmove) {
    savetime += time(NULL) - timerstart;
    timerstopped = true;
    }
  if(canmove && !timerstart)
    timerstart = time(NULL);
  
  if(princess::challenge) ;
#ifdef TOUR
  else if(tour::on) ;
#endif
  else if(tkills() < 100)
    dialog::addInfo(XLAT("Defeat 100 enemies to access the Graveyard"));
  else if(kills[moVizier] == 0 && (items[itFernFlower] < 5 || items[itGold] < 5))
    dialog::addInfo(XLAT("Kill a Vizier in the Palace to access Emerald Mine"));
  else if(items[itEmerald] < 5)
    dialog::addInfo(XLAT("Collect 5 Emeralds to access Camelot"));
  else if(hellUnlocked() && !chaosmode) {
    bool b = true;
    for(int i=0; i<LAND_HYP; i++)
      if(b && items[treasureType(land_hyp[i])] < 10) {
        dialog::addInfo(
          XLAT(
            land_hyp[i] == laTortoise ? "Hyperstone Quest: collect at least 10 points in %the2" :
            "Hyperstone Quest: collect at least 10 %1 in %the2", 
            treasureType(land_hyp[i]), land_hyp[i]));
        b = false;
        }
    if(b) 
      dialog::addInfo(XLAT("Hyperstone Quest completed!"), iinf[itHyperstone].color);
    }
  else dialog::addInfo(XLAT("Some lands unlock at specific treasures or kills"));
  if(cheater) {
    dialog::addInfo(XLAT("you have cheated %1 times", its(cheater)), 0xFF2020);
    }
  if(!cheater) {
    dialog::addInfo(timeline(), 0xC0C0C0);
    }
  
  msgs.clear();
  if(msgscroll < 0) msgscroll = 0;
  int gls = size(gamelog) - msgscroll;
  int mnum = 0;
  for(int i=gls-5; i<gls; i++) 
    if(i>=0) {
      msginfo m;
      m.spamtype = 0;
      m.flashout = true;
      m.stamp = ticks-128*vid.flashtime-128*(gls-i);
      m.msg = gamelog[i].msg;
      m.quantity = gamelog[i].quantity;
      mnum++,
      msgs.push_back(m);
      }

  dialog::addBreak(100);
  
  bool intour = false;
  
#ifdef TOUR
  intour = tour::on;
#endif

  if(intour) {
    if(canmove) {
      dialog::addItem("spherical geometry", '1');
      dialog::addItem("Euclidean geometry", '2');
      dialog::addItem("more curved hyperbolic geometry", '3');
      }
    if(!items[itOrbTeleport])
      dialog::addItem("teleport away", '4');
    else if(!items[itOrbAether])
      dialog::addItem("move through walls", '4');
    else
      dialog::addItem("flash", '4');
    if(canmove) {
      dialog::addItem("slide-specific command", '5');
      dialog::addItem("static mode", '6');
      dialog::addItem("enable/disable texts", '7');
      dialog::addItem("next slide", SDLK_RETURN);
      dialog::addItem("previous slide", SDLK_BACKSPACE);
      }
    else
      dialog::addBreak(200);
    dialog::addItem("main menu", 'v');
    }
  else {
    dialog::addItem(canmove ? "continue" : "see how it ended", SDLK_ESCAPE);
    dialog::addItem("main menu", 'v');
    dialog::addItem("restart", SDLK_F5);
    #ifndef MOBILE
    dialog::addItem(quitsaves() ? "save" : "quit", SDLK_F10);
    #endif
    #ifdef ANDROIDSHARE
    dialog::addItem("SHARE", 's'-96);
    #endif
    }
  
  dialog::display();

  if(mnum)
    displayfr(vid.xres/2, vid.yres-vid.fsize*(mnum+1), 2, vid.fsize/2,  XLAT("last messages:"), 0xC0C0C0, 8);  
  }

#ifdef MOBILE
void displayabutton(int px, int py, string s, int col) {
  // TMP
  int siz = vid.yres > vid.xres ? vid.fsize*2 : vid.fsize * 3/2;
  int vrx = min(vid.radius, vid.xres/2 - 40);
  int vry = min(vid.radius, min(vid.ycenter, vid.yres - vid.ycenter) - 20);
  int x = vid.xcenter + px * vrx;
  int y = vid.ycenter + py * (vry - siz/2);
  int vrr = int(hypot(vrx, vry) * sqrt(2.));
  if(gtouched && !mouseover
    && abs(mousex - vid.xcenter) < vrr
    && abs(mousey - vid.ycenter) < vrr
    && hypot(mousex-vid.xcenter, mousey-vid.ycenter) > vrr
    && px == (mousex > vid.xcenter ? 1 : -1)
    && py == (mousey > vid.ycenter ? 1 : -1)
    ) col = 0xFF0000;
  if(displayfr(x, y, 0, siz, s, col, 8+8*px))
    buttonclicked = true;
  }
#endif

#ifndef NOSAVE
vector<score> scores;

int scoresort = 2;
int scoredisplay = 1;
int scorefrom = 0;
int scoremode = 0;
bool scorerev = false;

bool scorecompare(const score& s1, const score &s2) {
  return s1.box[scoresort] > s2.box[scoresort];
  }

bool fakescore() {
  return fakebox[scoredisplay];
  }

string displayfor(score* S) {
  // printf("S=%p, scoredisplay = %d\n", S, scoredisplay);
  if(S == NULL) {
    return XLATN(boxname[scoredisplay]);
    }
  if(scoredisplay == 0) {
    char buf[10];
    snprintf(buf, 10, "%d:%02d", S->box[0]/60, S->box[0]%60);
    return buf;
    }
  if(scoredisplay == 1) {
    time_t tim = S->box[1];
    char buf[128]; strftime(buf, 128, "%c", localtime(&tim));
    return buf;
    }
  return its(S->box[scoredisplay]);
  }

void loadScores() {
  scores.clear();
  FILE *f = fopen(scorefile, "rt");
  if(!f) {
    printf("Could not open the score file '%s'!\n", scorefile);
    addMessage(s0 + "Could not open the score file: " + scorefile);
    return;
    }
  while(!feof(f)) {
    char buf[120];
    if(fgets(buf, 120, f) == NULL) break;
    if(buf[0] == 'H' && buf[1] == 'y') {
      score sc; bool ok = true;
      {if(fscanf(f, "%s", buf) <= 0) break;} sc.ver = buf;

      for(int i=0; i<MAXBOX; i++) {
        if(fscanf(f, "%d", &sc.box[i]) <= 0) { boxid = i; break; }
        }
      
      for(int i=boxid; i<MAXBOX; i++) sc.box[i] = 0;

      if(sc.ver >= "4.4") {
        sc.box[0] = sc.box[65];
        // the first executable on Steam included a corruption
        if(sc.box[65] > 1420000000 && sc.box[65] < 1430000000) {
          sc.box[0] = sc.box[65] - sc.box[1];
          sc.box[65] = sc.box[0];
          }
        // do not include saves
        if(sc.box[65 + 4 + itOrbSafety - itOrbLightning]) ok = false;
        }
      else 
        sc.box[0] = sc.box[1] - sc.box[0]; // could not save then
      if(ok && boxid > 20) scores.push_back(sc);
      }
    }
  fclose(f);
  addMessage(its(size(scores))+" games have been recorded in "+scorefile);
  cmode = emScores;
  boxid = 0; applyBoxes();
  scoresort = 2; reverse(scores.begin(), scores.end());
  scoremode = 0;
  if(shmup::on) scoremode = 1;
  else if(hardcore) scoremode = 2;
  scorefrom = 0;
  stable_sort(scores.begin(), scores.end(), scorecompare);
  #ifdef MOBILE
  extern int andmode;
  andmode = 2;
  #endif
  }

vector<pair<string, int> > pickscore_options;

bool notgl = false;
#endif

void setAppropriateOverview() {
  clearMessages();
  if(tactic::on)
    cmode = emTactic;
  else if(yendor::on)
    cmode = emYendor;
  else if(geometry != gNormal)
    cmode = emPickEuclidean;
  else 
    cmode = emOverview;
  }

ld textscale() { 
  return vid.fsize / (vid.radius * crossf) * (1+vid.alphax) * 2;
  }
  
transmatrix xymatrix(int x, int y, ld scale) {
  transmatrix V;
  {for(int i=0; i<3; i++) for(int j=0; j<3; j++) V[i][j] = i==j ? 1 : 0; }
  V[0][2] = (x - vid.xcenter + .0) / vid.radius * (1+vid.alphax);
  V[1][2] = (y - vid.ycenter + .0) / vid.radius * (1+vid.alphax);
  V[0][0] = scale;
  V[1][1] = scale;
  V[2][2] = 0;
  return V;
  }

int monsterclass(eMonster m) {
  if(isFriendly(m) || m == moTortoise) return 1;
  else if(isMonsterPart(m)) return 2;
  else return 0;
  }

int glyphclass(int i) {
  if(i < ittypes) {
    eItem it = eItem(i);
    return itemclass(it) == IC_TREASURE ? 0 : 1; 
    }
  else {
    eMonster m = eMonster(i-ittypes);
    return monsterclass(m) == 0 ? 2 : 3; 
    }
  }

int subclass(int i) {
  if(i < ittypes) 
    return itemclass(eItem(i)); 
  else 
    return monsterclass(eMonster(i-ittypes));
  }

#define GLYPH_MARKTODO   1
#define GLYPH_MARKOVER   2
#define GLYPH_LOCAL      4
#define GLYPH_IMPORTANT  8
#define GLYPH_NONUMBER   16
#define GLYPH_DEMON      32
#define GLYPH_RUNOUT     64
#define GLYPH_INPORTRAIT 128
#define GLYPH_LOCAL2     256
#define GLYPH_TARGET     512

eGlyphsortorder glyphsortorder;
  
int zero = 0;

int& ikmerge(int i) {
  if(i < ittypes) return items[i];
  else if(i == ittypes) return zero;
  else return kills[i-ittypes];
  }

const int glyphs = ittypes + motypes;

int gfirsttime[glyphs], glasttime[glyphs], gcopy[glyphs], ikland[glyphs];
int glyphorder[glyphs];

void updatesort() {
  for(int i=0; i<glyphs; i++) {
    if(ikmerge(i) && gfirsttime[i] == 0)
      gfirsttime[i] = ticks;
    if(ikmerge(i) != gcopy[i])
      gcopy[i] = items[i], glasttime[i] = ticks;
    }    
  }

void preparesort() {
  for(int i=0; i<glyphs; i++) glyphorder[i] = i;
  for(int i=0; i<LAND_OVERX; i++) {
    eLand l = land_over[i];
    ikland[treasureType(l)] = i+1;
    for(int mi=0; mi<motypes; mi++) 
      if(isNative(l, eMonster(mi)))
        ikland[mi+ittypes] = i+1;
    }
  glyphsortorder = gsoLand; updatesort();
  glyphsortorder = gsoFirstTop;
  }

int glyphsortkey = 0;

bool glyphsort(int i, int j) {
  if(subclass(i) != subclass(j))
    return subclass(i) < subclass(j);
  if(glyphsortorder == gsoFirstTop)
    return gfirsttime[i] < gfirsttime[j];
  if(glyphsortorder == gsoFirstBottom)
    return gfirsttime[i] > gfirsttime[j];
  if(glyphsortorder == gsoLastTop)
    return glasttime[i] > glasttime[j];
  if(glyphsortorder == gsoLastBottom)
    return glasttime[i] < glasttime[j];
  if(glyphsortorder == gsoValue)
    return ikmerge(i) > ikmerge(j);
  if(glyphsortorder == gsoLand)
    return ikland[i] < ikland[j];
  return 0;
  }

int glyphflags(int gid) {
  int f = 0;
  if(gid < ittypes) {
    eItem i = eItem(gid);
    if(itemclass(i) == IC_NAI) f |= GLYPH_NONUMBER;
    if(isElementalShard(i)) {
      f |= GLYPH_LOCAL;
      if(i == localshardof(cwt.c->land)) f |= GLYPH_LOCAL2;
      }
    if(i == treasureType(cwt.c->land)) f |= GLYPH_LOCAL | GLYPH_LOCAL2 | GLYPH_IMPORTANT;
    if(i == itHolyGrail) {
      if(items[i] >= 3) f |= GLYPH_MARKOVER;
      }
    else if(itemclass(i) == IC_TREASURE) {
      if(items[i] >= 25 && items[i] < 100) f |= GLYPH_MARKOVER;
      else if(items[i] < 10) f |= GLYPH_MARKTODO;
      }
    else {
      f |= GLYPH_IMPORTANT;
      if(itemclass(i) == IC_ORB && items[i] < 10) f |= GLYPH_RUNOUT;
      }
    if(i == orbToTarget) f |= GLYPH_TARGET;
    f |= GLYPH_INPORTRAIT;
    }
  else {
    eMonster m = eMonster(gid-ittypes);
    if(m == moLesser) f |= GLYPH_IMPORTANT | GLYPH_DEMON | GLYPH_INPORTRAIT;
    int isnat = isNative(cwt.c->land, m);
    if(isnat) f |= GLYPH_LOCAL | GLYPH_IMPORTANT | GLYPH_INPORTRAIT;
    if(isnat == 2) f |= GLYPH_LOCAL2;
    if(m == monsterToSummon) f |= GLYPH_TARGET;
    }
  return f;
  }

bool displayglyph(int cx, int cy, int buttonsize, char glyph, int color, int qty, int flags) {
    
  bool b =
    mousex >= cx && mousex < cx+buttonsize && mousey >= cy-buttonsize/2 && mousey <= cy-buttonsize/2+buttonsize;

  int glsize = buttonsize;
  if(glyph == '%' || glyph == 'M' || glyph == 'W') glsize = glsize*4/5;

  if(glyph == '*')
    displaychr(cx + buttonsize/2, cy+buttonsize/4, 0, glsize*3/2, glyph, darkenedby(color, b?0:1));
  else
    displaychr(cx + buttonsize/2, cy, 0, glsize, glyph, darkenedby(color, b?0:1));

  string fl = "";
  string str = its(qty);

  if(flags & GLYPH_TARGET) fl += "!";
  if(flags & GLYPH_LOCAL2) fl += "+";
  else if(flags & GLYPH_LOCAL) fl += "-";
  if(flags & GLYPH_DEMON) fl += "X";
  if(flags & GLYPH_MARKOVER) str += "!";

  if(fl != "") 
    displaystr(cx + buttonsize, cy-buttonsize/2 + buttonsize/4, 0, buttonsize/2, fl, darkenedby(color, 0), 16);

  if(flags & GLYPH_NONUMBER) str = "";
  
  int bsize = 
    (qty < 10 && (flags & (GLYPH_MARKTODO | GLYPH_RUNOUT))) ? buttonsize*3/4 :
    qty < 100 ? buttonsize / 2 :
    buttonsize / 3;
  if(str != "")
    displayfr(cx + buttonsize, cy + buttonsize/2 - bsize/2, 1, bsize, str, color, 16);

  return b;
  }

void drawStats() {
#ifdef ROGUEVIZ
  if(rogueviz::on) return;
#endif
  if(viewdists && sidescreen) {
    dialog::init("");
    int qty[64];
    vector<cell*>& ac = currentmap->allcells();
    for(int i=0; i<64; i++) qty[i] = 0;
    for(int i=0; i<size(ac); i++) {
      int d = celldistance(ac[i], cwt.c);
      if(d >= 0 && d < 64) qty[d]++;
      }
    if(geometry == gNormal)
      for(int i=purehepta?6:8; i<=15; i++) 
        qty[i] = 
          purehepta ?
            3*qty[i-1] - qty[i-2]
          : qty[i-1] + qty[i-2] + qty[i-3] - qty[i-4];
    if(geometry == gEuclid)
      for(int i=8; i<=15; i++) qty[i] = 6*i;
    for(int i=0; i<64; i++) if(qty[i])
      dialog::addInfo(its(qty[i]), distcolors[i&7]);
    if(geometry == gNormal && !purehepta) {
      dialog::addBreak(200);
      dialog::addHelp("a(d+4) = a(d+3) + a(d+2) + a(d+1) - a(d)");
      dialog::addInfo("a(d) ~ 1.72208^d", 0xFFFFFF);
      }
    if(geometry == gNormal && purehepta) {
      dialog::addBreak(200);
      dialog::addHelp("a(d+2) = 3a(d+1) - a(d+2)");
      dialog::addInfo("a(d) ~ 2.61803^d", 0xFFFFFF);
      }
    if(geometry == gEuclid) {
      dialog::addBreak(300);
      dialog::addInfo("a(n) = 6n", 0xFFFFFF);
      }
    dialog::display();
    }
  if(sidescreen) return;
  instat = false;
  bool portrait = vid.xres < vid.yres;
  int colspace = portrait ? (vid.yres - vid.xres - vid.fsize*3) : (vid.xres - vid.yres - 16) / 2;
  int rowspace = portrait ? vid.xres - 16 : vid.yres - vid.fsize * 4;
  int colid[4], rowid[4];
  int maxbyclass[4];
  for(int z=0; z<4; z++) maxbyclass[z] = 0;
  for(int i=0; i<glyphs; i++) if(ikmerge(i))
    if(!portrait || (glyphflags(i) | GLYPH_INPORTRAIT))
      maxbyclass[glyphclass(i)]++;
  int buttonsize;
  int columns, rows;
  bool imponly = false;
  int minsize = vid.fsize * (portrait ? 4 : 2);  
  rows = 0;
  while((buttonsize = minsize - vid.killreduction)) {
    columns = colspace / buttonsize;
    rows = rowspace / buttonsize;
    int coltaken = 0;
    for(int z=0; z<4; z++) {
      if(z == 2 && !portrait) {
        if(coltaken > columns) { vid.killreduction++; continue; }
        coltaken = 0;
        }
      colid[z] = coltaken, rowid[z] = 0,
      coltaken += (maxbyclass[z] + rows-1) / rows;
      }
    if(coltaken > columns) { vid.killreduction++; continue; }
    break;
    }

  if(buttonsize <= vid.fsize*3/4) {
    imponly = true; buttonsize = minsize;
    rows = rowspace / buttonsize; if(!rows) return;
    colid[0] = 0; colid[2] = portrait ? 1 : 0;
    }  
  
  updatesort();
  stable_sort(glyphorder, glyphorder+glyphs, glyphsort);
  
  for(int i0=0; i0<glyphs; i0++) {
    int i = glyphorder[i0];
    if(!ikmerge(i)) continue;
    int z = glyphclass(i);
    int imp = glyphflags(i);
    if(imponly) { z &=~1; if(!(imp & GLYPH_IMPORTANT)) continue; }

    int cx, cy;
    if(portrait)
      cx = 8 + buttonsize * rowid[z], cy = vid.fsize*2 + buttonsize * (colid[z]) + buttonsize/2;
    else
      cx = 8 + buttonsize * (colid[z]), cy = vid.fsize * 3 + buttonsize * rowid[z];
    
    if(!portrait && z < 2) cx = vid.xres - cx - buttonsize;

    rowid[z]++; if(rowid[z] >= rows) rowid[z] = 0, colid[z]++;
    
    char glyph = i < ittypes ? iinf[i].glyph : minf[i-ittypes].glyph;
    int color = i < ittypes ? iinf[i].color : minf[i-ittypes].color;
    
    if(displayglyph(cx, cy, buttonsize, glyph, color, ikmerge(i), imp)) {
      instat = true;
      getcstat = SDLK_F1;
      if(i < ittypes) {
        eItem it = eItem(i);
        int t = itemclass(it);
        if(t == IC_TREASURE)
          mouseovers = XLAT("treasure collected: %1", it);
        if(t == IC_OTHER)
          mouseovers = XLAT("objects found: %1", it);
        if(t == IC_NAI)
          mouseovers = XLAT("%1", it);
        if(t == IC_ORB)
          mouseovers = XLAT("orb power: %1", eItem(i));
        if(it == itGreenStone) {
          mouseovers += XLAT(" (click to drop)");
          getcstat = 'g';
          }
        if(imp & GLYPH_LOCAL) mouseovers += XLAT(" (local treasure)");
        help = generateHelpForItem(it);
        }
      else {
        eMonster m = eMonster(i-ittypes);
        if(isMonsterPart(m))
          mouseovers = s0 + XLAT("parts destroyed: %1", m);
        else if(isFriendly(m) && isNonliving(m))
          mouseovers = s0 + XLAT("friends destroyed: %1", m);
        else if(isFriendly(m))
          mouseovers = s0 + XLAT("friends killed: %1", m);
        else if(isNonliving(m))
          mouseovers = s0 + XLAT("monsters destroyed: %1", m);
        else if(m == moTortoise)
          mouseovers = s0 + XLAT("animals killed: %1", m);
        else 
          mouseovers = s0 + XLAT("monsters killed: %1", m);
        if(imp & GLYPH_LOCAL2) mouseovers += XLAT(" (killing increases treasure spawn)");
        else if(imp & GLYPH_LOCAL) mouseovers += XLAT(" (appears here)");
        help = generateHelpForMonster(m);
        }
      }
    }

  string s0;
  if(displayButtonS(vid.xres - 8, vid.fsize, "score: " + its(gold()), 0xFFFFFFF, 16, vid.fsize)) {
    mouseovers = XLAT("Your total wealth"),
    instat = true,
    getcstat = SDLK_F1,
    help = helptitle(XLAT("Your total wealth"), 0xFFD500) + 
    XLAT(
      "The total value of the treasure you have collected.\n\n"
      "Every world type contains a specific type of treasure, worth 1 $$$; "
      "your goal is to collect as much treasure as possible, but every treasure you find "
      "causes more enemies to hunt you in its native land.\n\n"
      "Orbs of Yendor are worth 50 $$$ each.\n\n"
      );
    }
  if(displayButtonS(8, vid.fsize, "kills: " + its(tkills()), 0xFFFFFFF, 0, vid.fsize)) {
    instat = true,
    getcstat = SDLK_F1,
    mouseovers = XLAT("Your total kills")+": " + its(tkills()),
    help = helptitle(XLAT("Your total kills") + ": " + its(tkills()), 0x404040) + 
      XLAT(
      "In most lands, more treasures are generated with each enemy native to this land you kill. "
      "Moreover, 100 kills is a requirement to enter the Graveyard and the Hive.\n\n"
      "Friendly creatures and parts of monsters (such as the Ivy) do appear in the list, "
      "but are not counted in the total kill count.");
    }
  if(displayButtonS(4, vid.yres - 4 - vid.fsize/2, s0+VER+ " fps: " + its(calcfps()), 0x202020, 0, vid.fsize/2)) {
    mouseovers = XLAT("frames per second"),
    getcstat = SDLK_F1,
    instat = true,
    help = 
      helptitle(XLAT("frames per second"), 0xFF4040) +
      XLAT(
      "The higher the number, the smoother the animations in the game. "
      "If you find that animations are not smooth enough, you can try "
      "to change the options "
      ) +
#ifdef IOS
XLAT(
      "(in the MENU). You can reduce the sight range, this should make "
      "the animations smoother.");
#else
XLAT(
      "(press v) and change the wall/monster mode to ASCII, or change "
      "the resolution.");
#endif
    }

  achievement_display();
#ifdef LOCAL
  process_local_stats();
#endif
  }

#ifndef MOBILE

#ifndef NOPNG
void IMAGESAVE(SDL_Surface *s, const char *fname) {
  SDL_Surface *s2 = SDL_PNGFormatAlpha(s);
  SDL_SavePNG(s2, fname);
  SDL_FreeSurface(s2);
  }
#endif

int pngres = 2000;

void saveHighQualityShot(const char *fname) {

#ifndef GFX
  addMessage(XLAT("High quality shots not available on this platform"));
  return;
#endif

  dynamicval<int> v3(sightrange, (cheater && sightrange < 10) ? 10 : sightrange);

  if(cheater) doOvergenerate();

  time_t timer;
  timer = time(NULL);

  dynamicval<videopar> v(vid, vid);
  dynamicval<bool> v2(inHighQual, true);
  dynamicval<int> v4(cheater, 0);
  vid.xres = vid.yres = pngres;
  vid.usingGL = false;
  // if(vid.pmodel == 0) vid.scale = 0.99;
  calcparam();
  #ifdef ROGUEVIZ
  rogueviz::fixparam();
  #endif

  dynamicval<SDL_Surface*> v5(s, SDL_CreateRGBSurface(SDL_SWSURFACE,vid.xres,vid.yres,32,0,0,0,0));

  darken = 0;

  for(int i=0; i<(fname?1:2); i++) {
    SDL_FillRect(s, NULL, fname ? backcolor : i ? 0xFFFFFF : 0);
    drawfullmap();
  
    char buf[128]; strftime(buf, 128, "bigshota-%y%m%d-%H%M%S" IMAGEEXT, localtime(&timer));
    buf[7] += i;
    if(!fname) fname = buf;
    IMAGESAVE(s, fname);

    if(i == 0) addMessage(XLAT("Saved the high quality shot to %1", fname));
    }
  
  SDL_FreeSurface(s);
  }
#endif

void addball(ld a, ld b, ld c) {
  hyperpoint h;
  ballmodel(h, a, b, c);
  for(int i=0; i<3; i++) h[i] *= vid.radius;
  curvepoint(h);
  }

void ballgeometry() {
  queuereset(vid.usingGL ? mdDisk : mdUnchanged, PPR_CIRCLE);
  for(int i=0; i<60; i++)
    addball(i * M_PI/30, 10, 0);
  for(double d=10; d>=-10; d-=.2)
    addball(0, d, 0);
  for(double d=-10; d<=10; d+=.2)
    addball(0, d, geom3::depth);
  addball(0, 0, -geom3::camera);
  addball(0, 0, geom3::depth);
  addball(0, 0, -geom3::camera);
  addball(0, -10, 0);
  addball(0, 0, -geom3::camera);
  queuecurve(darkena(0xFF, 0, 0x80), 0, PPR_CIRCLE);
  queuereset(pmodel, PPR_CIRCLE);
  }

void drawfullmap() {

  DEBB(DF_GRAPH, (debugfile,"draw full map\n"));
    
  ptds.clear();

  if(!vid.goteyes && !euclid && (pmodel == mdDisk || pmodel == mdBall)) {
    double rad = vid.radius;
    if(sphere) {
      if(!vid.grid && !elliptic)
        rad = 0; 
      else if(vid.alphax <= 0)
        ;
      else if(vid.alphax <= 1 && (vid.grid || elliptic)) // mark the equator
        rad = rad * 1 / vid.alphax;
      else if(vid.grid) // mark the edge
        rad /= sqrt(vid.alphax*vid.alphax - 1);
      }
    queuecircle(vid.xcenter, vid.ycenter, rad, 
      svg::in ? 0x808080FF : darkena(0xFF, 0, 0xFF), 
      vid.usingGL ? PPR_CIRCLE : PPR_OUTCIRCLE);
    if(pmodel == mdBall) ballgeometry();
    }
  
  if(pmodel == mdHyperboloid) {
    int col = darkena(0x80, 0, 0x80);
    queueline(hpxyz(0,0,1), hpxyz(0,0,-vid.alpha), col, 0, PPR_CIRCLE);
    queueline(xpush(+4)*C0, hpxyz(0,0,0), col, 0, PPR_CIRCLE);
    queueline(xpush(+4)*C0, hpxyz(0,0,-vid.alpha), col, 0, PPR_CIRCLE);
    queueline(xpush(-4)*C0, hpxyz(0,0,0), col, 0, PPR_CIRCLE);
    queueline(xpush(-4)*C0, hpxyz(0,0,-vid.alpha), col, 0, PPR_CIRCLE);
    queueline(hpxyz(-1,0,0), hpxyz(1,0,0), col, 0, PPR_CIRCLE);
    }
  
  if(pmodel == mdPolygonal || pmodel == mdPolynomial) 
    polygonal::drawBoundary(darkena(0xFF, 0, 0xFF));
  
  /* if(vid.wallmode < 2 && !euclid && !mapeditor::whichShape) {
    int ls = size(lines);
    if(ISMOBILE) ls /= 10;
    for(int t=0; t<ls; t++) queueline(View * lines[t].P1, View * lines[t].P2, lines[t].col >> (darken+1));
    } */

  drawthemap();
  #ifndef NORUG
  if(!inHighQual) {
    if(cmode == emNormal && !rug::rugged) {
      if(multi::players > 1) {
        transmatrix bcwtV = cwtV;
        for(int i=0; i<multi::players; i++) if(multi::playerActive(i))
          cwtV = multi::whereis[i], multi::cpid = i, drawmovestar(multi::mdx[i], multi::mdy[i]);
        cwtV = bcwtV;
        }
      else if(multi::alwaysuse)
        drawmovestar(multi::mdx[0], multi::mdy[0]);
      else 
        drawmovestar(0, 0);
      }
    if(rug::rugged && !rug::renderonce) queueline(C0, mouseh, 0xFF00FFFF, 5);
    mapeditor::drawGrid();
    }
  #endif
  profile_start(2);
  drawqueue();
  profile_stop(2);
  }

#include "menus.cpp"

void drawscreen() {

  if(vid.xres == 0 || vid.yres == 0) return;

  DEBB(DF_GRAPH, (debugfile,"drawscreen\n"));

  calcparam();
  #ifdef ROGUEVIZ
  rogueviz::fixparam();
  #endif
#ifdef GL
  if(vid.usingGL) setGLProjection();
#endif
  if(cmode != emHelp) help = "@";
  
  #ifndef MOBILE
  // SDL_LockSurface(s);
  // unsigned char *b = (unsigned char*) s->pixels;
  // int n = vid.xres * vid.yres * 4;
  // while(n) *b >>= 1, b++, n--;
  // memset(s->pixels, 0, vid.xres * vid.yres * 4);
  if(!vid.usingGL) SDL_FillRect(s, NULL, backcolor);
  #endif
  
  if(!canmove) darken = 1;
  if(cmode != emNormal && cmode != emDraw && cmode != emCustomizeChar) darken = 2;
  if(cmode == emQuit && !canmove) darken = 0;
  if(cmode == emOverview) darken = 16;
  
  if(sidescreen) darken = 0;

#ifndef NOEDIT
  if(cmode == emMapEditor && !mapeditor::subscreen && !mapeditor::choosefile) darken = 0;
  if(cmode == emDraw && mapeditor::choosefile) darken = 2;
#endif
  if(hiliteclick && darken == 0 && mmmon) darken = 1;
  if(cmode == emProgress) darken = 0;

  if(conformal::includeHistory && cmode != emProgress) conformal::restore();
  
  if(darken >= 8) ;
#ifndef NORUG
  else if(rug::rugged) {
    rug::actDraw();
    }
#endif
  else drawfullmap();

  if(conformal::includeHistory && cmode != emProgress) conformal::restoreBack();
  
  getcstat = 0; inslider = false;
  
  if(cmode == emNormal || cmode == emQuit) drawStats();

  #ifdef MOBILE
  
  buttonclicked = false;
  
  if(cmode == (canmove ? emNormal : emQuit)) {
    if(andmode == 0 && shmup::on) {
      using namespace shmupballs;
      calc();
      drawCircle(xmove, yb, rad, 0xFFFFFFFF);
      drawCircle(xmove, yb, rad/2, 0xFFFFFFFF);
      drawCircle(xfire, yb, rad, 0xFF0000FF);
      drawCircle(xfire, yb, rad/2, 0xFF0000FF);
      }
    else {
      if(andmode != 0) displayabutton(-1, +1, XLAT("MOVE"),  andmode == 0 ? BTON : BTOFF);
      displayabutton(+1, +1, XLAT(andmode == 1 ? "BACK" : "DRAG"),  andmode == 1 ? BTON : BTOFF);
      }
    displayabutton(-1, -1, XLAT("INFO"),  andmode == 12 ? BTON : BTOFF);
    displayabutton(+1, -1, XLAT("MENU"), andmode == 3 ? BTON : BTOFF);
    }
  
  #endif
 
  // displaynum(vx,100, 0, 24, 0xc0c0c0, celldist(cwt.c), ":");

  darken = 0;
  drawmessages();
  
  if(cmode == emNormal) {
    if(!canmove) showGameover();
    }
  
  if(cmode == emProgress) mouseovers = "";
  
  displayMenus();
  
  describeMouseover();

  if((havewhat&HF_BUG) && darken == 0 && (cmode == emNormal || cmode == emQuit)) for(int k=0; k<3; k++)
    displayfr(vid.xres/2 + vid.fsize * 5 * (k-1), vid.fsize*2,   2, vid.fsize, 
      its(hive::bugcount[k]), minf[moBug0+k].color, 8);
    
  bool minefieldNearby = false;
  int mines[4], tmines=0;
  for(int p=0; p<numplayers(); p++) {
    mines[p] = 0;
    cell *c = playerpos(p);
    if(!c) continue;
    for(int i=0; i<c->type; i++) if(c->mov[i]) {
      if(c->mov[i]->land == laMinefield) 
        minefieldNearby = true;
      if(c->mov[i]->wall == waMineMine) {
        bool ep = false;
        if(!ep) mines[p]++, tmines++;
        }
      }
    }

  if((minefieldNearby || tmines) && canmove && !items[itOrbAether] && darken == 0 && cmode == emNormal) {
    string s;
    if(tmines > 7) tmines = 7;
    int col = minecolors[tmines];
    
    if(tmines == 7) seenSevenMines = true;
    
    for(int p=0; p<numplayers(); p++) if(multi::playerActive(p))
      displayfr(vid.xres * (p+.5) / numplayers(),
        vid.ycenter - vid.radius * 3/4, 2,
        vid.fsize, 
        XLAT(minetexts[mines[p]]), minecolors[mines[p]], 8);

    if(minefieldNearby && !shmup::on && cwt.c->land != laMinefield && cwt.c->mov[cwt.spin]->land != laMinefield) {
      displayfr(vid.xres/2, vid.ycenter - vid.radius * 3/4 - vid.fsize*3/2, 2,
        vid.fsize, 
        XLAT("WARNING: you are entering a minefield!"), 
        col, 8);
      }
    }

  #ifndef MOBILE
  if(cmode == emNormal || cmode == emVisual1 || cmode == emVisual2 || cmode == emChangeMode ) {
    if(tour::on) 
      displayButton(vid.xres-8, vid.yres-vid.fsize, XLAT("(ESC) tour menu"), SDLK_ESCAPE, 16);
    else
      displayButton(vid.xres-8, vid.yres-vid.fsize, XLAT("(v) menu"), 'v', 16);
    }
  #endif  

  if(cmode == emQuit) {
    if(canmove) showGameover();
    }

  #ifndef MOBILE
  // SDL_UnlockSurface(s);

  DEBT("swapbuffers");
#ifdef GL
  if(vid.usingGL) SDL_GL_SwapBuffers(); else
#endif
  SDL_UpdateRect(s, 0, 0, vid.xres, vid.yres);
  
//printf("\ec");

  #endif
  }

#ifndef MOBILE
bool setfsize = true;

void setvideomode() {

  DEBB(DF_INIT, (debugfile,"setvideomode\n"));
  
  if(!vid.full) {
    if(vid.xres > vid.xscr) vid.xres = vid.xscr * 9/10, setfsize = true;
    if(vid.yres > vid.yscr) vid.yres = vid.yscr * 9/10, setfsize = true;    
    }
  
  if(setfsize) vid.fsize = min(vid.yres / 32, vid.xres / 48), setfsize = false;

  int flags = 0;
  
#ifdef GL
  if(vid.usingGL) {
    flags = SDL_OPENGL | SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER;
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
    }
#endif

  int sizeflag = (vid.full ? SDL_FULLSCREEN : SDL_RESIZABLE);
  
  s= SDL_SetVideoMode(vid.xres, vid.yres, 32, flags | sizeflag);
  
  if(vid.full && !s) {
    vid.xres = vid.xscr;
    vid.yres = vid.yscr;
    vid.fsize = min(vid.yres / 32, vid.xres / 48);
    s = SDL_SetVideoMode(vid.xres, vid.yres, 32, flags | SDL_FULLSCREEN);
    }

  if(!s) {
    addMessage("Failed to set the graphical mode: "+its(vid.xres)+"x"+its(vid.yres)+(vid.full ? " fullscreen" : " windowed"));
    vid.xres = 640;
    vid.yres = 480;
    s = SDL_SetVideoMode(vid.xres, vid.yres, 32, flags | SDL_RESIZABLE);
    }

#ifdef GL
  if(vid.usingGL) {
    glViewport(0, 0, vid.xres, vid.yres);
    resetGL();
    }
#endif
  }
#endif

void restartGraph() {
  DEBB(DF_INIT, (debugfile,"restartGraph\n"));
  
  View = Id;
  webdisplay = 0;
  if(currentmap) {
    if(euclid) {
      centerover = euclideanAtCreate(0,0);
      }
    else {
      viewctr.h = currentmap->getOrigin();
      viewctr.spin = 0;
      viewctr.mirrored = false;
      }
    if(sphere) View = spin(-M_PI/2);
    }
  }

void resetview() {
  DEBB(DF_GRAPH, (debugfile,"reset view\n"));
  View = Id;
  // EUCLIDEAN
  if(!euclid) 
    viewctr.h = cwt.c->master,
    viewctr.spin = cwt.spin;
  else centerover = cwt.c;
  // SDL_LockSurface(s);
  // SDL_UnlockSurface(s);
  }


void initcs(charstyle &cs) {
  cs.charid     = 0;
  cs.skincolor  = 0xD0D0D0FF;
  cs.haircolor  = 0x686868FF;
  cs.dresscolor = 0xC00000FF;
  cs.swordcolor = 0xD0D0D0FF;
  cs.dresscolor2= 0x8080FFC0;
  cs.uicolor    = 0xFF0000FF;
  }

#ifndef NOCONFIG
void savecs(FILE *f, charstyle& cs, int xvernum) {
  int gflags = cs.charid;
  if(vid.samegender) gflags |= 16;
  
  fprintf(f, "%d %d %08x %08x %08x %08x", 
    gflags, vid.language, cs.skincolor, cs.haircolor, cs.swordcolor, cs.dresscolor);
  if(cs.charid == 3) fprintf(f, " %08x", cs.dresscolor2);
  if(xvernum >= 8990) fprintf(f, " %x", cs.uicolor);
  fprintf(f, "\n");
  }

void loadcs(FILE *f, charstyle& cs, int xvernum) {
  int gflags, err = 
  fscanf(f, "%d%d%x%x%x%x", &gflags, &vid.language, &cs.skincolor, &cs.haircolor, &cs.swordcolor, &cs.dresscolor);

  if(err) cs.charid = gflags & 15;
  if(err) vid.samegender = (gflags & 16) ? true : false;
  if(cs.charid == 3) if(fscanf(f, "%x", &cs.dresscolor2)) 
    ;
  if(xvernum >= 8990) if(fscanf(f, "%x", &cs.uicolor)) 
    ;
  }

void saveConfig() {
  DEBB(DF_INIT, (debugfile,"save config\n"));
  FILE *f = fopen(conffile, "wt");
  if(!f) {
    addMessage(s0 + "Could not open the config file: " + conffile);
    return;
    }
  fprintf(f, "%d %d %d %d\n", vid.xres, vid.yres, vid.full, vid.fsize);
  fprintf(f, "%f %f %f %f\n", float(vid.scale), float(vid.eye), float(vid.alpha), float(vid.sspeed));
  fprintf(f, "%d %d %d %d %d %d %d\n", vid.wallmode, vid.monmode, vid.axes, musicvolume, vid.framelimit, vid.usingGL, vid.usingAA);
  fprintf(f, "%d %d %d %f %d %d\n", vid.joyvalue, vid.joyvalue2, vid.joypanthreshold, float(vid.joypanspeed), autojoy, vid.flashtime);
  
  savecs(f, vid.cs, 0);
  
  fprintf(f, "%d %d\n", vid.darkhepta, vid.shifttarget);
  
  fprintf(f, "%d %d %d %d\n", euclid, euclidland, shmup::on, hardcore);

  shmup::saveConfig(f);

  fprintf(f, "%d %d %d %d %f %d %d\n", rug::renderonce, rug::rendernogl, rug::texturesize, purehepta, rug::scale, vid.steamscore, chaosmode);

  fprintf(f, "%d %d %f %d %d %f\n",
    pmodel, polygonal::SI, float(polygonal::STAR), polygonal::deg, 
    conformal::includeHistory, float(conformal::lvspeed));
  
  fprintf(f, "%d %d %d %d %d %d\n", 
    conformal::bandhalf, conformal::bandsegment, 
    conformal::rotation, conformal::autoband, conformal::autobandhistory,
    conformal::dospiral);
  
  fprintf(f, "%d", polygonal::maxcoef);
  for(int i=0; i<=polygonal::maxcoef; i++) fprintf(f, " %lf %lf", 
    (double) real(polygonal::coef[i]), (double) imag(polygonal::coef[i]));

  fprintf(f, "\n%d %d %d %f %d %d\n", 
    revcontrol, vid.drawmousecircle, sightrange, float(vid.mspeed), effvolume, vid.particles);
  
  {
  int pt_depth = 0, pt_camera = 0, pt_alpha = 0;
  using namespace geom3;
  if(tc_depth > tc_camera) pt_depth++;
  if(tc_depth < tc_camera) pt_camera++;
  if(tc_depth > tc_alpha ) pt_depth++;
  if(tc_depth < tc_alpha ) pt_alpha ++;
  if(tc_alpha > tc_camera) pt_alpha++;
  if(tc_alpha < tc_camera) pt_camera++;
  
  fprintf(f, "%f %f %f %f %f %f %f %d %d %d %f %f %d\n",
    float(geom3::depth), float(geom3::camera), float(geom3::wall_height), 
    float(geom3::rock_wall_ratio),
    float(geom3::human_wall_ratio), 
    float(geom3::lake_top),
    float(geom3::lake_bottom),
    pt_depth, pt_camera, pt_alpha,
    float(geom3::highdetail), float(geom3::middetail),
    glyphsortorder);
  
  fprintf(f, "%f %f %f %f\n",
    float(vid.yshift), float(vid.camera_angle),
    float(vid.ballangle), float(vid.ballproj)
    );

  fprintf(f, "%d\n", vid.mobilecompasssize);

  }
    
  fprintf(f, "\n\nThis is a configuration file for HyperRogue (version " VER ")\n");
  fprintf(f, "\n\nThe numbers are:\n");
  fprintf(f, "screen width & height, fullscreen mode (0=windowed, 1=fullscreen), font size\n");
  fprintf(f, "scale, eye distance, parameter, scrolling speed\n");
  fprintf(f, "wallmode, monster mode, cross mode, music volume, framerate limit, usingGL, usingAA\n");
  fprintf(f, "calibrate first joystick (threshold A, threshold B), calibrate second joystick (pan threshold, pan speed), joy mode\n");
  fprintf(f, "gender (1=female, 16=same gender prince), language, skin color, hair color, sword color, dress color\n");
  fprintf(f, "darken hepta, shift target\n");
  fprintf(f, "euclid, euclid land, shmup, hardcore\n");
  fprintf(f, "version number, shmup players, alwaysuse, shmup keyboard/joystick config\n");
  fprintf(f, "hypersian rug config: renderonce, rendernogl, texturesize; purehepta; rug scale; share score; chaosmode\n");
  fprintf(f, "conformal: model, sides, star, degree, includeHistory, speed\n");
  fprintf(f, "conformal: bandwidth, segment, rotation, autoband, autohistory, dospiral\n");
  fprintf(f, "conformal: degree, (degree+1) times {real, imag}\n");
  fprintf(f, "revcontrol, drawmousecircle, sight range, movement animation speed, sound effect volume, particle effects\n");
  fprintf(f, "3D parameters, sort order\n");
  fprintf(f, "yhsift, camera angle, ball angle, ball projection\n");
  fprintf(f, "compass size\n");
  
  fclose(f);
#ifndef MOBILE
  addMessage(s0 + "Configuration saved to: " + conffile);
#else
  addMessage(s0 + "Configuration saved");
#endif
  }

void readf(FILE *f, ld& x) {
  double fl = x; 
  if(fscanf(f, "%lf", &fl)) 
    ; 
  x = fl;
  }

void loadConfig() {
 
    DEBB(DF_INIT, (debugfile,"load config\n"));
  vid.xres = 9999; vid.yres = 9999; vid.framelimit = 300;
  FILE *f = fopen(conffile, "rt");
  if(f) {
    int fs, gl=1, aa=1, bb=1, cc, dd, ee;
    int err;
    err=fscanf(f, "%d%d%d%d", &vid.xres, &vid.yres, &fs, &vid.fsize);
    vid.full = fs;
    float a, b, c, d;
    err=fscanf(f, "%f%f%f%f\n", &a, &b, &c, &d);
    if(err == 4) {
      vid.scale = a; vid.eye = b; vid.alpha = c; vid.sspeed = d;
      }
    err=fscanf(f, "%d%d%d%d%d%d%d", &vid.wallmode, &vid.monmode, &vid.axes, &musicvolume, &vid.framelimit, &gl, &aa);
    vid.usingGL = gl; vid.usingAA = aa;
    double jps = vid.joypanspeed;
    err=fscanf(f, "%d%d%d%lf%d%d", &vid.joyvalue, &vid.joyvalue2, &vid.joypanthreshold, &jps, &aa, &vid.flashtime);
    vid.joypanspeed = jps;
    autojoy = aa; aa = 0;

    loadcs(f, vid.cs, 0);

    aa=0; bb=0;
    err=fscanf(f, "%d%d", &aa, &bb);
    vid.darkhepta = aa; vid.shifttarget = bb;

    aa = geometry; bb = euclidland; cc = shmup::on; dd = hardcore;
    err=fscanf(f, "%d%d%d%d", &aa, &bb, &cc, &dd);
    geometry = eGeometry(aa); euclidland = eLand(bb); shmup::on = cc; hardcore = dd;

    shmup::loadConfig(f);

    aa = rug::renderonce; bb = rug::rendernogl; cc = purehepta; dd = chaosmode; ee = vid.steamscore;
    double rs = rug::scale;
    err=fscanf(f, "%d%d%d%d%lf%d%d", &aa, &bb, &rug::texturesize, &cc, &rs, &ee, &dd);
    rug::renderonce = aa; rug::rendernogl = bb; purehepta = cc; chaosmode = dd; vid.steamscore = ee;
    rug::scale = rs;

    aa=conformal::includeHistory;
    double ps = polygonal::STAR, lv = conformal::lvspeed;
    int pmb = pmodel;
    err=fscanf(f, "%d%d%lf%d%d%lf",
      &pmb, &polygonal::SI, &ps, &polygonal::deg,
      &aa, &lv);
    pmodel = eModel(pmb);
    conformal::includeHistory = aa; polygonal::STAR = ps; conformal::lvspeed = lv;
    
    aa=conformal::autoband; bb=conformal::autobandhistory; cc=conformal::dospiral;    
    err=fscanf(f, "%d%d%d%d%d%d", 
      &conformal::bandhalf, &conformal::bandsegment, &conformal::rotation,
      &aa, &bb, &cc);
    conformal::autoband = aa; conformal::autobandhistory = bb; conformal::dospiral = cc;
    
    err=fscanf(f, "%d", &polygonal::maxcoef);
    if(polygonal::maxcoef < 0) polygonal::maxcoef = 0;
    if(polygonal::maxcoef > MSI) polygonal::maxcoef = MSI;
    for(int i=0; i<=polygonal::maxcoef; i++) {
      double re=0, im=0;
      err=fscanf(f, "%lf%lf", &re, &im);
      polygonal::coef[i] = polygonal::cld(re, im);
      }
    
    aa=revcontrol; bb=vid.drawmousecircle;
    d = vid.mspeed;
    err=fscanf(f, "%d%d%d%f%d%d", &aa, &bb, &sightrange, &d, &effvolume, &vid.particles);
    vid.mspeed = d;
    if(sightrange < 4) sightrange = 4;
    if(sightrange > 7) sightrange = 7;
    revcontrol = aa; vid.drawmousecircle = bb;
     
    readf(f, geom3::depth); readf(f, geom3::camera); readf(f, geom3::wall_height);
    readf(f, geom3::rock_wall_ratio); readf(f, geom3::human_wall_ratio);
    readf(f, geom3::lake_top); readf(f, geom3::lake_bottom);
    
    err=fscanf(f, "%d %d %d", &geom3::tc_depth, &geom3::tc_camera, &geom3::tc_alpha);
    
    readf(f, geom3::highdetail); 
    geom3::middetail = 200; readf(f, geom3::middetail);
    if(geom3::middetail == 200) {
      if(ISMOBILE)
        geom3::highdetail = 0, geom3::middetail = 3;
      else
        geom3::highdetail = geom3::middetail = 5;
      }

    int gso = glyphsortorder; err=fscanf(f, "%d", &gso); glyphsortorder = eGlyphsortorder(gso);
    
    readf(f, vid.yshift); readf(f, vid.camera_angle); readf(f, vid.ballangle); readf(f, vid.ballproj);

    err=fscanf(f, "%d\n", &vid.mobilecompasssize);
  
    fclose(f);
    DEBB(DF_INIT, (debugfile,"Loaded configuration: %s\n", conffile));
    
    if(err)
      ;
    }

  precalc();
  }
#endif

#ifndef MOBILE
void initJoysticks() {
  DEBB(DF_INIT, (debugfile,"init joysticks\n"));
  numsticks = SDL_NumJoysticks();
  if(numsticks > 8) numsticks = 8;
  for(int i=0; i<numsticks; i++) {
    sticks[i] = SDL_JoystickOpen(i);
    /* printf("axes = %d, balls = %d, buttons = %d, hats = %d\n",
      SDL_JoystickNumAxes(sticks[i]),
      SDL_JoystickNumBalls(sticks[i]),
      SDL_JoystickNumButtons(sticks[i]),
      SDL_JoystickNumHats(sticks[i])
      ); */
    }
  }

void closeJoysticks() {
  DEBB(DF_INIT, (debugfile,"close joysticks\n"));
  for(int i=0; i<numsticks; i++) {
    SDL_JoystickClose(sticks[i]), sticks[i] = NULL;
    }
  numsticks = 0;
  }
#endif

void initgraph() {

  DEBB(DF_INIT, (debugfile,"initgraph\n"));

  vid.usingGL = true;
  vid.usingAA = true;
  vid.flashtime = 8;
  vid.scale = 1;
  vid.alpha = 1;
  vid.sspeed = 0;
  vid.mspeed = 1;
  vid.eye = 0;
  vid.full = false;
  vid.ballangle = 20;
  vid.yshift = 0;
  vid.camera_angle = 0;
  vid.ballproj = 1;

#ifdef ANDROID
  vid.monmode = 2;
  vid.wallmode = 3;
#else
#ifdef PANDORA
  vid.monmode = 2;
  vid.wallmode = 3;
#else
  vid.monmode = 4;
  vid.wallmode = 5;
#endif
#endif

  vid.particles = 1;
  vid.mobilecompasssize = 30;

  vid.joyvalue = 4800;
  vid.joyvalue2 = 5600;
  vid.joypanthreshold = 2500;
#ifdef PANDORA
  vid.joypanspeed = 0.0001;
#else
  vid.joypanspeed = 0;
#endif

  vid.framelimit = 75;
  vid.axes = 1;
  vid.shifttarget = 2;
  vid.steamscore = 1;
  
  initcs(vid.cs);
  
  vid.killreduction = 0;
  
  vid.samegender = false;
  vid.language = -1;
  
  joyx = joyy = 0; joydir.d = -1;
  
  vid.drawmousecircle = false;
  revcontrol = false;
#ifdef MOBILE
  vid.drawmousecircle = true;
#endif
#ifdef PANDORA
  vid.drawmousecircle = true;
#endif
  
  shmup::initConfig();

  restartGraph();
  
  initgeo();

  buildpolys();

  #ifndef MOBILE  
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1)
  {
    printf("Failed to initialize video.\n");
    exit(2);
  }

#ifdef WEB
  vid.xscr = vid.xres = 1200;
  vid.yscr = vid.yres = 900;
#else
  const SDL_VideoInfo *inf = SDL_GetVideoInfo();
  vid.xscr = vid.xres = inf->current_w;
  vid.yscr = vid.yres = inf->current_h;
#endif
  
  SDL_WM_SetCaption("HyperRogue " VER, "HyperRogue " VER);
  #endif
  
  preparesort();
#ifndef NOCONFIG
  loadConfig();
#endif

#ifdef USE_COMMANDLINE
  arg::read(2);
#endif

  #ifndef MOBILE  
  setvideomode();
  if(!s) {
    printf("Failed to initialize graphics.\n");
    exit(2);
    }
    
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
  SDL_EnableUNICODE(1);
  
  if(TTF_Init() != 0) {
    printf("Failed to initialize TTF.\n");
    exit(2);
    }
  
  initJoysticks();

  #ifdef SDLAUDIO
  initAudio();
  #endif
    
  #endif
  }

int frames;
bool outoffocus = false;

void panning(hyperpoint hf, hyperpoint ht) {
  View = 
    rgpushxto0(hf) * rgpushxto0(gpushxto0(hf) * ht) * gpushxto0(hf) * View;
  playermoved = false;
  }

#ifdef LOCAL
#include "local.cpp"
#endif

bool needConfirmation() {
  return canmove && (gold() >= 30 || tkills() >= 50) && !cheater && !quitsaves();
  }

void fullcenter() {
  if(playerfound && false) centerpc(INF);
  else {
    bfs();
    resetview();
    drawthemap();
    centerpc(INF);
    }
  playermoved = true;
  }

bool didsomething;

bool quitmainloop = false;

void handleKeyQuit(int sym, int uni) {
  dialog::handleNavigation(sym, uni);
  // ignore the camera movement keys

#ifndef NORUG
  if(rug::rugged && (sym == SDLK_UP || sym == SDLK_DOWN || sym == SDLK_PAGEUP || sym == SDLK_PAGEDOWN ||
    sym == SDLK_RIGHT || sym == SDLK_LEFT))
    sym = 0;
#endif

  if(sym == SDLK_RETURN || sym == SDLK_F10) quitmainloop = true;
  else if(uni == 'r' || sym == SDLK_F5) {
    restartGame(), cmode = emNormal;
    msgs.clear();
    }
  else if(sym == SDLK_UP || sym == SDLK_KP8) msgscroll++;
  else if(sym == SDLK_DOWN || sym == SDLK_KP2) msgscroll--;
  else if(sym == SDLK_PAGEUP || sym == SDLK_KP9) msgscroll+=5;
  else if(sym == SDLK_PAGEDOWN || sym == SDLK_KP3) msgscroll-=5;
  else if(uni == 'v') cmode = emMenu;
  else if(sym == SDLK_HOME || sym == SDLK_F3 || (sym == ' ' && DEFAULTCONTROL)) 
    fullcenter();
  else if(uni == 'o') setAppropriateOverview();
#ifndef NOSAVE
  else if(uni == 't') {
    if(!canmove) restartGame();
    loadScores();
    msgs.clear();
    }
  #endif
  
  else if((sym != 0 && sym != SDLK_F12) && !didsomething) {
    cmode = emNormal;
    msgscroll = 0;
    msgs.clear();
    }
  }

#ifdef MOBILE
#define extra int
#else
#define extra SDL_Event
#endif

void handleKeyNormal(int sym, int uni, extra& ev) {

  if(cheater) {
    if(applyCheat(uni, mouseover))
      sym = 0;
    }

  if(!(uni >= 'A' && uni <= 'Z') && DEFAULTCONTROL) {
    if(sym == 'l' || sym == 'd' || sym == SDLK_KP6) movepckeydir(0);
    if(sym == 'n' || sym == 'c' || sym == SDLK_KP3) movepckeydir(1);
    if(sym == 'j' || sym == 'x' || sym == SDLK_KP2) movepckeydir(2);
    if(sym == 'b' || sym == 'z' || sym == SDLK_KP1) movepckeydir(3);
    if(sym == 'h' || sym == 'a' || sym == SDLK_KP4) movepckeydir(4);
    if(sym == 'y' || sym == 'q' || sym == SDLK_KP7) movepckeydir(5);
    if(sym == 'k' || sym == 'w' || sym == SDLK_KP8) movepckeydir(6);
    if(sym == 'u' || sym == 'e' || sym == SDLK_KP9) movepckeydir(7);
    }

#ifdef PANDORA
  if(DEFAULTCONTROL) {
    if(sym == SDLK_RIGHT) movepckeydir(0);
    if(sym == SDLK_LEFT) movepckeydir(4);
    if(sym == SDLK_DOWN) movepckeydir(2 + (leftclick?1:0) - (rightclick?1:0));
    if(sym == SDLK_UP) movepckeydir(6 - (leftclick?1:0) + (rightclick?1:0));
    }
#endif

  if(DEFAULTCONTROL) {
    if(sym == '.' || sym == 's') movepcto(-1, 1);
    if(uni == '%' && sym == '5') { 
      if(vid.wallmode == 0) vid.wallmode = 6;
      vid.wallmode--;
      }
    if(uni == sym) {
      if(uni == '1') { 
        vid.alpha = 999; vid.scale = 998;
        }
      if(uni == '2') { 
        vid.alpha = 1; vid.scale = 0.4;
        }
      if(uni == '3') { 
        vid.alpha = 1; vid.scale = 1;
        }
      if(uni == '4') { 
        vid.alpha = 0; vid.scale = 1;
        }
      if(uni == '5') { 
        vid.wallmode++;
        if(vid.wallmode == 6) vid.wallmode = 0;
        }
      if(uni == '6') {
        vid.grid = !vid.grid;
        }
      if(uni == '7') {
        vid.darkhepta = !vid.darkhepta;
        }
      if(uni == '8') {
        backcolor = backcolor ^ 0xFFFFFF;
        printf("back = %x\n", backcolor);
        }
      if(uni == '9') {
        pmodel = eModel(8 - pmodel);
        // vid.yshift = 1 - vid.yshift;
        // vid.drawmousecircle = true;
        }
      }
    if((sym == SDLK_DELETE || sym == SDLK_KP_PERIOD || sym == 'g') && uni != 'G' && uni != 'G'-64) 
      movepcto(MD_DROP, 1);
    if(sym == 'm' && canmove && cmode == emNormal && (centerover == cwt.c ? mouseover : centerover))
      performMarkCommand(mouseover);
    if(sym == 't' && uni != 'T' && uni != 'T'-64 && canmove && cmode == emNormal) {
      if(playermoved && items[itStrongWind]) {
        cell *c = whirlwind::jumpDestination(cwt.c);
        if(c) centerover = c;
        }
      targetRangedOrb(centerover, roKeyboard);
      sym = 0; uni = 0;
      }
    }

  if(sym == SDLK_KP5 && DEFAULTCONTROL) movepcto(-1, 1);

  // if(sym == SDLK_F4)  restartGameSwitchEuclid();

  if(sym == SDLK_F5) {
    if(needConfirmation()) cmode = emQuit;
    else restartGame();
    }

  if(sym == SDLK_ESCAPE) {
    cmode = emQuit;
    achievement_final(false);
    if(!canmove) {
      addMessage(XLAT("GAME OVER"));
      addMessage(timeline());
      }
    msgscroll = 0;
    }
  if(sym == SDLK_F10) {
    if(needConfirmation()) cmode = emQuit;
    else quitmainloop = true;
    }
  
  if(!canmove) {
    if(sym == SDLK_RETURN) quitmainloop = true;
    else if(uni == 'r') restartGame();
#ifndef NOSAVE
    else if(uni == 't') {
      restartGame();
      loadScores();
      }
#endif
#ifndef NORUG
    else if(rug::rugged) ;
#endif
    else if(sym == SDLK_UP || sym == SDLK_KP8) msgscroll++;
    else if(sym == SDLK_DOWN || sym == SDLK_KP2) msgscroll--;
    else if(sym == SDLK_PAGEUP || sym == SDLK_KP9) msgscroll+=5;
    else if(sym == SDLK_PAGEDOWN || sym == SDLK_KP3) msgscroll-=5;
    }
  
  if(uni == 'o') setAppropriateOverview();
  
  if(sym == SDLK_HOME || sym == SDLK_F3 || (sym == ' ' && DEFAULTCONTROL)) 
    fullcenter();
  
/*      if(sym == SDLK_F6) {
    View = spin(M_PI/2) * inverse(cwtV) * View;
    if(flipplayer) View = pispin * View;
    cmode = emDraw;
    } */

  if(sym == 'v') {
    cmode = emMenu;
    }

  if(sym == SDLK_F2) {
    cmode = emVisual1;
    }

#ifndef MOBILE
#ifdef PANDORA
  if(ev.type == SDL_MOUSEBUTTONUP && sym == 0 && !rightclick) 
#else
  if(ev.type == SDL_MOUSEBUTTONDOWN && sym == 0 && !rightclick) 
#endif
  if(canmove && getcstat != 'v' && getcstat != 'g' && getcstat != SDLK_F1)
    {
    actonrelease = false;
    
    shmup::cpid = 0;
    if(mouseover && 
      targetclick && (!shmup::on || numplayers() == 1) && targetRangedOrb(mouseover, forcetarget ? roMouseForce : roMouse)) {
      }
    else if(forcetarget)
      ;
    else if(!DEFAULTCONTROL) {
      if(!shmup::on)
        multi::mousemovement(mouseover);
      }
    else mousemovement();
    }
#endif

  if(sym == SDLK_F1) {
    lastmode = cmode;
    cmode = emHelp;
    }

#ifdef ROGUEVIZ
  rogueviz::processKey(sym, uni);
#endif

#ifdef LOCAL
  process_local0(sym);
#endif
  }

void handlekey(int sym, int uni, extra& ev) {

#ifdef TOUR
  if(tour::on && tour::handleKeyTour(sym, uni)) return;
#endif

  if(((cmode == emNormal && canmove) || (cmode == emQuit && !canmove) || cmode == emDraw || cmode == emMapEditor) && DEFAULTCONTROL && !rug::rugged) {
#ifndef PANDORA
    if(sym == SDLK_RIGHT) { 
      if(conformal::on)
        conformal::lvspeed += 0.1 * shiftmul;
      else
        View = xpush(-0.2*shiftmul) * View, playermoved = false, didsomething = true;
      }
    if(sym == SDLK_LEFT) {
      if(conformal::on)
        conformal::lvspeed -= 0.1 * shiftmul;
      else
        View = xpush(+0.2*shiftmul) * View, playermoved = false, didsomething = true;
      }
    if(sym == SDLK_UP) {
      if(conformal::on)
        conformal::lvspeed += 0.1 * shiftmul;
      else
        View = ypush(+0.2*shiftmul) * View, playermoved = false, didsomething = true;
      }
    if(sym == SDLK_DOWN) {
      if(conformal::on)
        conformal::lvspeed -= 0.1 * shiftmul;
      else
        View = ypush(-0.2*shiftmul) * View, playermoved = false, didsomething = true;
      }
#endif
    if(sym == SDLK_PAGEUP) {
      if(conformal::on)
        conformal::rotation++;
      else
        View = spin(M_PI/S21*shiftmul) * View, didsomething = true;
      }
    if(sym == SDLK_PAGEDOWN) {
      if(conformal::on)
        conformal::rotation++;
      else
        View = spin(-M_PI/S21*shiftmul) * View, didsomething = true;
      }
    
    if(sym == SDLK_PAGEUP || sym == SDLK_PAGEDOWN) 
      if(isGravityLand(cwt.c->land)) playermoved = false;
    }
  
#ifndef MOBILE
  if(sym == SDLK_F7 && !vid.usingGL) {

    time_t timer;
    timer = time(NULL);
    char buf[128]; strftime(buf, 128, "shot-%y%m%d-%H%M%S" IMAGEEXT, localtime(&timer));

    IMAGESAVE(s, buf);
    addMessage(XLAT("Screenshot saved to %1", buf));
    }
#endif

  #ifdef DEMO
  if(cmode == emOverview || cmode == emMenu) handleDemoKey(sym, uni); else
  #endif

  if(cmode == emNormal) handleKeyNormal(sym, uni, ev);
  else if(cmode == emMenu) handleMenuKey(sym, uni);
  else if(cmode == emCheatMenu) handleCheatMenu(sym, uni);
  else if(cmode == emVisual1) handleVisual1(sym, uni);
  else if(cmode == emJoyConfig) handleJoystickConfig(sym, uni);
  else if(cmode == emCustomizeChar) handleCustomizeChar(sym, uni);
  else if(cmode == emVisual2) handleVisual2(sym, uni);
  else if(cmode == emChangeMode) handleChangeMode(sym, uni);
  else if(cmode == emShmupConfig) shmup::handleConfig(sym, uni);
#ifndef NOMODEL
  else if(cmode == emNetgen) netgen::handleKey(sym, uni);
#endif
#ifndef NORUG
  else if(cmode == emRugConfig) rug::handleKey(sym, uni);
#endif
#ifndef NOEDIT
  else if(cmode == emMapEditor) mapeditor::handleKey(sym, uni);
  else if(cmode == emDraw) mapeditor::drawHandleKey(sym, uni);
#endif
#ifndef NOSAVE
#ifndef MOBILE
  else if(cmode == emScores) handleScoreKeys(sym);
#endif
  else if(cmode == emPickScores) handlePickScoreKeys(uni);
#endif
  else if(cmode == emConformal) conformal::handleKey(sym, uni);
  else if(cmode == emYendor) yendor::handleKey(sym, uni);
  else if(cmode == emTactic) tactic::handleKey(sym, uni);
  else if(cmode == emOverview) handleOverview(sym, uni);
  else if(cmode == emPickEuclidean) handleEuclidean(sym, uni);
#ifdef MOBILE
#ifdef HAVE_ACHIEVEMENTS
  else if(cmode == emLeader) leader::handleKey(sym, uni);
#endif
#endif
  else if(cmode == emColor) dialog::handleColor(sym, uni);
  else if(cmode == emNumber) dialog::handleNumber(sym, uni);      
  else if(cmode == emHelp) handleHelp(sym, uni);    
  else if(cmode == em3D) handle3D(sym, uni);
  else if(cmode == emQuit) handleKeyQuit(sym, uni);
#ifdef ROGUEVIZ
  else if(cmode == emRogueviz) rogueviz::handleMenu(sym, uni);
#endif
  }

#ifndef MOBILE

// Warning: a very long function! todo: refactor


void mainloopiter() {

  DEBB(DF_GRAPH, (debugfile,"main loop\n"));

  #ifndef GFX
  #ifndef GL
  vid.wallmode = 0;
  vid.monmode = 0;
  #endif
  #endif

#ifdef LOCAL
  process_local_extra();
#endif
  
  optimizeview();

  if(conformal::on) conformal::apply();
  
  ticks = SDL_GetTicks();
    
  int cframelimit = vid.framelimit;
  if((cmode == emVisual1 || cmode == emVisual2 || cmode == emHelp || cmode == emQuit ||
    cmode == emCustomizeChar || cmode == emMenu || cmode == emPickEuclidean ||
    cmode == emScores || cmode == emPickScores) && cframelimit > 15)
    cframelimit = 15;
  if(outoffocus && cframelimit > 10) cframelimit = 10;
  
  int timetowait = lastt + 1000 / cframelimit - ticks;

  if(DOSHMUP && cmode == emNormal) 
    timetowait = 0, shmup::turn(ticks - lastt);
    
  if(!DOSHMUP && (multi::alwaysuse || multi::players > 1) && cmode == emNormal)
    timetowait = 0, multi::handleMulti(ticks - lastt);

  if(vid.sspeed >= 5 && gmatrix.count(cwt.c) && !elliptic) {
    cwtV = gmatrix[cwt.c] * ddspin(cwt.c, cwt.spin);
    if(cwt.mirrored) playerV = playerV * Mirror;
    }

#ifdef WEB
  if(playermoved && vid.sspeed > -4.99 && !outoffocus) {
    centerpc((ticks - lastt) / 1000.0 * exp(vid.sspeed));
    }
  if(!outoffocus) drawscreen();
#else
  if(timetowait > 0)
    SDL_Delay(timetowait);
  else {
    if(cmode != emOverview) {
      if(playermoved && vid.sspeed > -4.99 && !outoffocus)
        centerpc((ticks - lastt) / 1000.0 * exp(vid.sspeed));
      if(panjoyx || panjoyy) 
        checkpanjoy((ticks - lastt) / 1000.0);
      }
    tortoise::updateVals(ticks - lastt);
    frames++;
    if(!outoffocus) {
      drawscreen();
      }
    lastt = ticks;
    }      
#endif

  Uint8 *keystate = SDL_GetKeyState(NULL);
  rightclick = keystate[SDLK_RCTRL];
  leftclick = keystate[SDLK_RSHIFT];
  lctrlclick = keystate[SDLK_LCTRL];
  lshiftclick = keystate[SDLK_LSHIFT];
  forcetarget = (keystate[SDLK_RSHIFT] | keystate[SDLK_LSHIFT]);
  hiliteclick = keystate[SDLK_LALT] | keystate[SDLK_RALT];
  anyshiftclick = keystate[SDLK_LSHIFT] | keystate[SDLK_RSHIFT];
  wheelclick = false;

  getcshift = 1;
  if(keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]) getcshift = -1;
  if(keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL]) getcshift /= 10;
  if(keystate[SDLK_LALT] || keystate[SDLK_RALT]) getcshift *= 10;
  
  didsomething = false;
  
  if(vid.shifttarget&1) {
    leftclick = false;
    targetclick = keystate[SDLK_RSHIFT] | keystate[SDLK_LSHIFT];
    }
  else {
    leftclick = keystate[SDLK_RSHIFT];
    targetclick = true;
    }
  
#ifdef SDLAUDIO
  if(audio) handlemusic();
#endif
  SDL_Event ev;
  DEBB(DF_GRAPH, (debugfile,"polling for events\n"));
  
  achievement_pump();
  while(SDL_PollEvent(&ev)) {
    DEBB(DF_GRAPH, (debugfile,"got event type #%d\n", ev.type));
    int sym = 0;
    int uni = 0;
    shiftmul = 1;
    
/*    if(ev.type == SDL_JOYDEVICEADDED || ev.type == SDL_JOYDEVICEREMOVED) {
      joyx = joyy = 0;
      panjoyx = panjoyy = 0;
      closeJoysticks();
      initJoysticks();
      }*/

    if(ev.type == SDL_ACTIVEEVENT) {
      if(ev.active.state & SDL_APPINPUTFOCUS) {
        if(ev.active.gain) {
          outoffocus = false;
          }
        else {
          outoffocus = true;
          }
        }
      }
    
    if(ev.type == SDL_VIDEORESIZE) {
      vid.xres = ev.resize.w;
      vid.yres = ev.resize.h;
      vid.killreduction = 0;
      extern bool setfsize;
      setfsize = true;
      setvideomode();
#ifdef GL
      if(vid.usingGL) glViewport(0, 0, vid.xres, vid.yres);
#endif
      }
    
    if(ev.type == SDL_VIDEOEXPOSE) {
      drawscreen();
      }
    
    if(ev.type == SDL_JOYAXISMOTION) {
      if(ev.jaxis.which == 0) {
        if(ev.jaxis.axis == 0)
          joyx = ev.jaxis.value;
        else if(ev.jaxis.axis == 1)
          joyy = ev.jaxis.value;
        else if(ev.jaxis.axis == 3)
          panjoyx = ev.jaxis.value;
        else if(ev.jaxis.axis == 4)
          panjoyy = ev.jaxis.value;
        checkjoy();
        // printf("panjoy = %d,%d\n", panjoyx, panjoyy);
        }
      else {
        if(ev.jaxis.axis == 0)
          panjoyx = ev.jaxis.value;
        else 
          panjoyy = ev.jaxis.value;
        }
      }

    if(ev.type == SDL_JOYBUTTONDOWN && cmode == emShmupConfig && vid.scfg.setwhat) {
      int joyid = ev.jbutton.which;
      int button = ev.jbutton.button;
      if(joyid < 8 && button < 32)
         vid.scfg.joyaction[joyid][button] = vid.scfg.setwhat;
      vid.scfg.setwhat = 0;
      }

    else if(ev.type == SDL_JOYHATMOTION && cmode == emShmupConfig && vid.scfg.setwhat) {
      int joyid = ev.jhat.which;
      int hat = ev.jhat.hat;
      int dir = 4;
      if(ev.jhat.value == SDL_HAT_UP) dir = 0;
      if(ev.jhat.value == SDL_HAT_RIGHT) dir = 1;
      if(ev.jhat.value == SDL_HAT_DOWN) dir = 2;
      if(ev.jhat.value == SDL_HAT_LEFT) dir = 3;
      if(joyid < 8 && hat < 4 && dir < 4) {
        vid.scfg.hataction[joyid][hat][dir] = vid.scfg.setwhat;
        vid.scfg.setwhat = 0;
        }
      }

    else if(ev.type == SDL_JOYBUTTONDOWN && DEFAULTCONTROL) {
      flashMessages();
      movepcto(joydir);
      checkjoy();
      }

    if(ev.type == SDL_KEYDOWN) {
      flashMessages();
      mousing = false;
      sym = ev.key.keysym.sym;
      uni = ev.key.keysym.unicode;
      if(ev.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) shiftmul = -1;
      if(ev.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) shiftmul /= 10;
      }
    
    dialog::handleZooming(ev);
    
    if(sym == SDLK_F1 && cmode == emNormal && playermoved)
      help = "@";
    
    bool rollchange = 
     cmode == emOverview && getcstat >= 2000 && cheater;

    if(ev.type == SDL_MOUSEBUTTONDOWN) {
      flashMessages();
      mousepressed = true;
      mousing = true;
      actonrelease = true;
      if(ev.button.button==SDL_BUTTON_WHEELUP && ((cmode == emQuit) ^ !canmove)) {
        }
      else if(ev.button.button==SDL_BUTTON_WHEELDOWN) {
        if(cmode == (canmove ? emQuit : emNormal)) {
          sym = 1; msgscroll--; didsomething = true;
          }
        else 
          if(cmode == emTactic || cmode == emYendor || cmode == emPickEuclidean ||
            cmode == emLeader || cmode == emScores || cmode == emOverview)
          if(!rollchange)
            sym = uni = PSEUDOKEY_WHEELDOWN;
        }
      if(ev.button.button==SDL_BUTTON_WHEELUP) {
        if(cmode == (canmove ? emQuit : emNormal)) {
          sym = 1; msgscroll++; didsomething = true;
          }
        else if(cmode == (canmove ? emNormal : emQuit) || cmode == emMapEditor || cmode == emDraw) {
          ld jx = (mousex - vid.xcenter - .0) / vid.radius / 10;
          ld jy = (mousey - vid.ycenter - .0) / vid.radius / 10;
          playermoved = false;
          View = gpushxto0(hpxy(jx, jy)) * View;
          sym = 1;
          }
        else 
          if(cmode == emTactic || cmode == emYendor || cmode == emPickEuclidean ||
            cmode == emLeader || cmode == emScores || cmode == emOverview)
          if(getcstat < 2000 || !cheater)
            sym = uni = PSEUDOKEY_WHEELUP;
        }
      else if(ev.button.button == SDL_BUTTON_RIGHT) {
        sym = 1; didsomething = true;
        }
      else if(ev.button.button == SDL_BUTTON_MIDDLE) {
        sym = 2; didsomething = true;
        }
      }

    if(ev.type == SDL_MOUSEBUTTONUP) {
      mousepressed = false;
      mousing = true;
      if(ev.button.button==SDL_BUTTON_RIGHT || leftclick) 
        sym = SDLK_F1;
      else if(ev.button.button==SDL_BUTTON_MIDDLE || rightclick) 
        sym = 1, didsomething = true;
      else if(ev.button.button == SDL_BUTTON_LEFT && actonrelease) {
        sym = getcstat, uni = getcstat, shiftmul = getcshift;
        }
      else if(ev.button.button == SDL_BUTTON_WHEELUP && rollchange) {
        sym = getcstat, uni = getcstat, shiftmul = getcshift, wheelclick = true;
        }
      else if(ev.button.button == SDL_BUTTON_WHEELDOWN && rollchange) {
        sym = getcstat, uni = getcstat, shiftmul = -getcshift, wheelclick = true;
        }
      }

    if(ev.type == SDL_MOUSEMOTION) {
      hyperpoint mouseoh = mouseh;
      
      mousing = true;
      mousemoved = true;
      mousex = ev.motion.x;
      mousey = ev.motion.y;

#ifndef NORUG
      if(rug::rugged)
        mouseh = rug::gethyper(mousex, mousey);
      else
#endif
        mouseh = gethyper(mousex, mousey);
      
      if((rightclick || (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_MMASK)) && 
        !outofmap(mouseh) && !outofmap(mouseoh) && 
         mouseh[2] < 50 && mouseoh[2] < 50) {
        panning(mouseoh, mouseh);
        }

#ifdef SIMULATE_JOYSTICK
      // pretend that both joysticks are present
      stick = panstick = (SDL_Joystick*) (&vid);
      panjoyx = 20 * (mousex - vid.xcenter);
      panjoyy = 20 * (mousey - vid.ycenter);
      checkjoy();
#endif

      if(mousepressed && inslider) {
        sym = getcstat, uni = getcstat, shiftmul = getcshift;
        }
      }

    handlekey(sym, uni, ev);
    
    if(ev.type == SDL_QUIT) {
      if(needConfirmation() && cmode !=emQuit) cmode = emQuit;
      else quitmainloop = true;
      }

    }
  }

void mainloop() {
  lastt = 0;
  cmode = emNormal;
#ifdef WEB
  initweb();
  emscripten_set_main_loop(mainloopiter, 0, true);
#else
  while(!quitmainloop) mainloopiter();
#endif
  }
#endif

#ifndef MOBILE
void cleargraph() {
  DEBB(DF_INIT, (debugfile,"clear graph\n"));
  for(int i=0; i<256; i++) if(font[i]) TTF_CloseFont(font[i]);
  for(int i=0; i<128; i++) if(glfont[i]) delete glfont[i];
#ifndef SIMULATE_JOYSTICK
  closeJoysticks();
#endif
  SDL_Quit();
  }
#endif

void cleargraphmemory() {
  DEBB(DF_INIT, (debugfile,"clear graph memory\n"));
  mouseover = centerover = lmouseover = NULL;  
#ifndef NOEDIT
  if(mapeditor::painttype == 4) 
    mapeditor::painttype = 0, mapeditor::paintwhat = 0,
    mapeditor::paintwhat_str = "clear monster";
  mapeditor::copywhat = NULL;
  mapeditor::undo.clear();
  if(!cheater) mapeditor::displaycodes = 0;
  if(!cheater) mapeditor::whichShape = 0;
#endif
  for(int i=0; i<ANIMLAYERS; i++) animations[i].clear();
  gmatrix.clear(); gmatrix0.clear();
  }

void showMissionScreen() {
  cmode = emQuit;
  msgscroll = 0;
  }

void resetGeometry() {
  precalc();
  fp43.analyze(); 
  resetGL();
  }


//=== animation

map<cell*, animation> animations[ANIMLAYERS];
unordered_map<cell*, transmatrix> gmatrix, gmatrix0;

void animateMovement(cell *src, cell *tgt, int layer) {
  if(vid.mspeed >= 5) return; // no animations!
  if(confusingGeometry()) return;
  if(gmatrix.count(src) && gmatrix.count(tgt)) {
    animation& a = animations[layer][tgt];
    if(animations[layer].count(src)) {
      a = animations[layer][src];
      a.wherenow = inverse(gmatrix[tgt]) * gmatrix[src] * a.wherenow;
      animations[layer].erase(src);
      }
    else {
      a.ltick = ticks;
      a.wherenow = inverse(gmatrix[tgt]) * gmatrix[src];
      a.footphase = 0;
      }
    }
  }

vector<pair<cell*, animation> > animstack;

void indAnimateMovement(cell *src, cell *tgt, int layer) {
  if(vid.mspeed >= 5) return; // no animations!
  if(confusingGeometry()) return;
  if(animations[layer].count(tgt)) {
    animation res = animations[layer][tgt];
    animations[layer].erase(tgt);
    animateMovement(src, tgt, layer);
    if(animations[layer].count(tgt)) 
      animstack.push_back(make_pair(tgt, animations[layer][tgt]));
    animations[layer][tgt] = res;
    }
  else {
    animateMovement(src, tgt, layer);
    if(animations[layer].count(tgt)) {
      animstack.push_back(make_pair(tgt, animations[layer][tgt]));
      animations[layer].erase(tgt);
      }
    }
  }

void commitAnimations(int layer) {
  for(int i=0; i<size(animstack); i++)
    animations[layer][animstack[i].first] = animstack[i].second;
  animstack.clear();
  }

void animateReplacement(cell *a, cell *b, int layer) {
  if(vid.mspeed >= 5) return; // no animations!
  static cell c1;
  gmatrix[&c1] = gmatrix[b];
  if(animations[layer].count(b)) animations[layer][&c1] = animations[layer][b];
  animateMovement(a, b, layer);
  animateMovement(&c1, a, layer);
  }
