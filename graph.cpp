// Hyperbolic Rogue -- graphics

// Copyright (C) 2011-2012 Zeno Rogue, see 'hyper.cpp' for details

// basic graphics:

#define WOLNIEJ 1
#define BTOFF 0x404040
#define BTON  0xC0C000

// #define PANDORA


#ifndef MOBILE
#include <SDL/SDL.h>

#ifdef AUDIO
#include <SDL/SDL_mixer.h>
#endif
bool audio;
int audiovolume = 60;

#ifndef MAC
#undef main
#endif

#include <SDL/SDL_ttf.h>
#endif

#ifndef MOBILE

// x resolutions

#define NUMMODES 7

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

emtype cmode = emNormal, lastmode = emNormal; // last mode in Help

// 
int axestate;

int ticks;

videopar vid;
int default_language;

charstyle& getcs() {
  if(shmup::on && shmup::players>1 && shmup::cpid >= 0 && shmup::cpid < shmup::players)
    return shmup::scs[shmup::cpid];
  else
    return vid.cs;
  }

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

int sightrange = 7;
               
cell *mouseover, *mouseover2, *lmouseover, *centerover, *lcenterover; 
ld modist, modist2, centdist;
string mouseovers;

movedir mousedest, joydir;

int mousex, mousey, joyx, joyy, panjoyx, panjoyy;
bool autojoy = true;
hyperpoint mouseh;

bool leftclick, rightclick, targetclick, hiliteclick, anyshiftclick;
bool gtouched;
bool revcontrol;

int getcstat; ld getcshift;

int ZZ;

string help;

int andmode = 0;

int darken = 0;

bool doHighlight() {
  return (hiliteclick && darken < 2) ? vid.monmode == 2 : vid.monmode == 3;
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
    font[siz] = TTF_OpenFont("DejaVuSans-Bold.ttf", siz);
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

int textwidth(int siz, const string &str) {
  if(size(str) == 0) return 0;
  
  loadfont(siz);
  
  int w, h;
  TTF_SizeUTF8(font[siz], str.c_str(), &w, &h);
  // printf("width = %d [%d]\n", w, size(str));
  return w;
  }
#endif

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

int lalpha = 0xFF;

void glcolor(int color) {
  unsigned char *c = (unsigned char*) (&color);
  glColor4f(c[2] / 255.0, c[1] / 255.0, c[0]/255.0, lalpha / 255.0); // c[3]/255.0);
  }

void selectEyeGL(int ed) {
  DEBB(DF_GRAPH, (debugfile,"selectEyeGL\n"));
  float ve = ed*vid.eye;
  ve *= 2; // vid.xres; ve /= vid.radius;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  float lowdepth = 0.1;
  float hidepth = 1e9;
  
  // simulate glFrustum
  GLfloat frustum[16] = {
    GLfloat(vid.yres * 1./vid.xres), 0, 0, 0, 
    0, 1, 0, 0, 
    0, 0, -(hidepth+lowdepth)/(hidepth-lowdepth), -1,
    0, 0, -2*lowdepth*hidepth/(hidepth-lowdepth), 0};

  if(ve)
    glTranslatef(-(ve * vid.radius) * (vid.alpha - (vid.radius*1./vid.xres) * vid.eye) / vid.xres, 0, 0);

  glTranslatef((vid.xcenter*2.)/vid.xres - 1, 1 - (vid.ycenter*2.)/vid.yres, 0);

  glMultMatrixf(frustum);  

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLfloat sc = vid.radius / (vid.yres/2.);
  GLfloat mat[16] = {sc,0,0,0, 0,-sc,0,0, 0,0,-1,0, 0,0, GLfloat(-vid.alpha),1};
  glMultMatrixf(mat);
  
  if(ve) glTranslatef(ve, 0, vid.eye);

  vid.scrdist = -vid.alpha + vid.yres * sc / 2;
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
  
  glClearColor(0,0,0,1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);     

  selectEyeGL(0);
  }

bool GL_initialized = false;
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

void init(const char * fname, unsigned int h);

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
    fprintf(stderr, "OPENGL ERROR #%i: in file %s on line %i :: %s\n",errCode,file, line, GLcall);
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

  loadfont(size);
  if(!font[size]) return;

  char str[2]; str[1] = 0;
  
  SDL_Color white;
  white.r = white.g = white.b = 255;
  
  glListBase(0);
 
  for(unsigned char ch=1;ch<128+NUMEXTRA;ch++) {
    // printf("init size=%d ch=%d\n", size, ch);
  
    SDL_Surface *txt;
    
    if(ch < 128) {
      str[0] = ch;
      txt = TTF_RenderText_Blended(font[size], str, white);
      }
    else {
      txt = TTF_RenderUTF8_Blended(font[size], natchars[ch-128], white);
      }
    if(txt == NULL) continue;
//  printf("a\n");
  
    int twidth = next_p2( txt->w );
    int theight = next_p2( txt->h );
   
    Uint16 expanded_data[twidth * theight];

    for(int j=0; j <theight;j++) for(int i=0; i < twidth; i++) {
      expanded_data[(i+j*twidth)] = 
          (i>=txt->w || j>=txt->h) ? 0 : ((qpixel(txt, i, j)>>24)&0xFF) * 0x101;
      }

//  printf("b\n");
    f.widths[ch] = txt->w;
    f.heights[ch] = txt->h;
  
    glBindTexture( GL_TEXTURE_2D, f.textures[ch]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, twidth, theight, 0,
      GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data );
   
//  printf("c\n");

    float x=(float)txt->w / (float)twidth;
    float y=(float)txt->h / (float)theight;
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

    SDL_FreeSurface(txt);    
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

bool gl_print(int x, int y, int shift, int size, const char *s, int color, int align) {

  // printf("gl_print: %s\n", s.c_str());
  
  // We Want A Coordinate System Where Distance Is Measured In Window Pixels.
  
  /*
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, 0, vid.xscr, vid.yscr); */
  
  init_glfont(size);
  if(!font[size]) return false;

  glfont_t& f(*glfont[size]);
  
  glDisable(GL_LIGHTING);
  
  glEnable(GL_TEXTURE_2D);

  glMatrixMode(GL_MODELVIEW);
  
  glcolor(color);

  int tsize = 0;
  for(int i=0; s[i];) tsize += f.widths[getnext(s,i)];
  x -= tsize * align / 16;
  y += f.heights[32]/2;

  bool clicked = (mousex >= x && mousey <= y && mousex <= x+tsize && mousey >= y-f.heights[32]);
  
  for(int i=0; s[i];) {
  
    // glListBase(f.list_base);
    // glCallList(s[i]); // s[i]);
    
    int tabid = getnext(s,i);
    float fx=f.tx[tabid];
    float fy=f.ty[tabid];
    int wi = f.widths[tabid];
    int hi = f.heights[tabid];

    GLERR("pre-print");
    
    for(int ed = (vid.goteyes && shift)?-1:0; ed<2; ed+=2) {
      glPushMatrix();
      glTranslatef(x-ed*shift-vid.xcenter,y-vid.ycenter, vid.scrdist);
      selectEyeMask(ed);
      glBindTexture(GL_TEXTURE_2D, f.textures[tabid]);
      glBegin(GL_QUADS);
      glTexCoord2d(0,0); glVertex2f(0, -hi);
      glTexCoord2d(0,fy); glVertex2f(0, 0);
      glTexCoord2d(fx,fy); glVertex2f(wi, 0);
      glTexCoord2d(fx,0); glVertex2f(wi, -hi);
      glEnd();
      glPopMatrix();
      }
    
    if(vid.goteyes) selectEyeMask(0);
    
    GLERR("print");
    
    // int tabid = s[i];
    x += f.widths[tabid];
    
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
  GL_initialized = false;
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

  if(size <= 0 || size > 255) {
    // printf("size = %d\n", size);
    return false;
    }
  
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
  int len = (int) s.size();
  return 
    mx >= -len*size*align/32   && mx <= +len*size*(16-align)/32 && 
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
  string msg;
  };

vector<msginfo> msgs;

vector<string> gamelog;

void flashMessages() {
  for(int i=0; i<size(msgs); i++) 
    if(msgs[i].stamp < ticks - 1000 && !msgs[i].flashout) {
      msgs[i].flashout = true;
      msgs[i].stamp = ticks;
      }
  }

void addMessage(string s, char spamtype) {
  DEBB(DF_MSG, (debugfile,"addMessage: %s\n", s.c_str()));
  if(gamelog.size() == 20) {
    for(int i=0; i<19; i++) gamelog[i] = gamelog[i+1];
    gamelog[19] = s;
    }
  else gamelog.push_back(s);
  msginfo m;
  m.msg = s; m.spamtype = spamtype; m.flashout = false; m.stamp = ticks;
  msgs.push_back(m);
  // printf("%s\n", s.c_str());
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
      displayfr(x, y, 1, vid.fsize, msgs[j].msg, 0x10101 * (255 - age/vid.flashtime), 8);
      msgs[i++] = msgs[j];
      }
    }
  msgs.resize(i);
  }

int pmodel = 0;

ld ghx, ghy, ghgx, ghgy;
hyperpoint ghpm = C0;

void xybound(int& x, int &y) {
  if(x<-vid.xres) x=-vid.xres; if(x>2*vid.xres) x=2*vid.xres;
  if(y<-vid.yres) y=-vid.yres; if(y>2*vid.yres) y=2*vid.yres;   
  }

void ghcheck(int& x, int &y, const hyperpoint &H) {
  xybound(x,y);
  if(hypot(x-ghx, y-ghy) < hypot(ghgx-ghx, ghgy-ghy)) {
    ghpm = H; ghgx = x; ghgy = y;
    }
  }

hyperpoint gethyper(ld x, ld y) {

  if(pmodel) {
    ghx = x, ghy = y;
    return ghpm;
    }
  
  ld hx = (x - vid.xcenter) / vid.radius;
  ld hy = (y - vid.ycenter) / vid.radius;
  
  if(euclid)
    return hpxy(hx * (EUCSCALE + vid.alphax), hy * (EUCSCALE + vid.alphax));
    
  ld hr = hx*hx+hy*hy;
  
  if(hr > .9999) return Hypc;
  
  // hz*hz-(hx/(hz+alpha))^2 - (hy/(hz+alpha))^2 =
  
  // hz*hz-hr*(hz+alpha)^2 == 1
  // hz*hz - hr*hr*hz*Hz
  
  ld A = 1-hr;
  ld B = 2*hr*vid.alphax;
  ld C = 1 + hr*vid.alphax*vid.alphax;
  
  // Az^2 - Bz = C
  B /= A; C /= A;
  
  // z^2 - Bz = C
  // z^2 - Bz + (B^2/4) = C + (B^2/4)
  // z = (B/2) + sqrt(C + B^2/4)
  
  ld hz = B / 2 + sqrt(C + B*B/4);
  hyperpoint H;
  H[0] = hx * (hz+vid.alphax);
  H[1] = hy * (hz+vid.alphax);
  H[2] = hz;
  
  return H;
  }

void getcoord(const hyperpoint& H, int& x, int& y, int &shift) {
  
  if(H[2] < 0.999) {
    // printf("error: %s\n", display(H));
    // exit(1);
    }
  ld tz = euclid ? (EUCSCALE+vid.alphax) : vid.alphax+H[2];
  if(tz < 1e-3 && tz > -1e-3) tz = 1000;
  
  if(pmodel == 0) {
    x = vid.xcenter + int(vid.radius * H[0] / tz);
    y = vid.ycenter + int(vid.radius * H[1] / tz);
    #ifndef MOBILE
    shift = vid.goteyes ? int(vid.eye * vid.radius * (1 - vid.beta / tz)) : 0;
    #endif
    xybound(x,y);
    return;
    }

  if(pmodel == 3 || pmodel == 4) {
    pair<long double, long double> p = polygonal::compute(H[0]/tz, H[1]/tz);
    x = vid.xcenter + int(vid.radius * p.first);
    y = vid.ycenter + int(vid.radius * p.second);
    shift = 0; ghcheck(x,y,H);
    return;
    }
  
  // Poincare to half-plane
  tz = H[2]+vid.alphax;
  
  ld x0, y0;  
  x0 = H[0] / tz;
  y0 = H[1] / tz;
  y0 += 1;
  double rad = x0*x0 + y0*y0;
  y0 /= rad;
  x0 /= rad;
  y0 -= .5;
  
  if(pmodel == 1) {
    x = vid.xcenter + int(x0 * vid.radius);
    y = vid.ycenter*2 - int(y0 * vid.radius);
    shift = 0; ghcheck(x,y,H);
    return;
    }

  // center
  x0 *= 2; y0 *= 2;
  
  // half-plane to band
  double tau = (log((x0+1)*(x0+1) + y0*y0) - log((x0-1)*(x0-1) + y0*y0)) / 2;
  double u=(1-x0*x0-y0*y0);
  u = (1 - x0*x0 - y0*y0 + sqrt(u*u+4*y0*y0));
  double sigma = 2 * atan(2*y0 / u) - M_PI/2;
  
  x0 = tau; y0 = sigma;
  
  x = vid.xcenter + int(x0 * vid.radius/M_PI*2);
  y = vid.ycenter - int(y0 * vid.radius/M_PI*2);
  shift = 0; ghcheck(x,y,H);
  }

int dlit;

void drawline(const hyperpoint& H1, int x1, int y1, int s1, const hyperpoint& H2, int x2, int y2, int col) {
  dlit++; if(dlit>500) return;
  
  int dst = (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);

  #ifdef GL
  if(vid.usingGL && dst <= (ISMOBILE ? 100 : 20)) {
    if(pmodel) {
      glcoords[qglcoords][0] = x1 - vid.xcenter;
      glcoords[qglcoords][1] = y1 - vid.ycenter;
      glcoords[qglcoords][2] = vid.scrdist;
      }
    else if(euclid) {
      for(int i=0; i<2; i++) glcoords[qglcoords][i] = H1[i]; glcoords[qglcoords][2] = EUCSCALE;
      }
    else {
      for(int i=0; i<3; i++) glcoords[qglcoords][i] = H1[i];
      }
    qglcoords++;
    return;
    }
  #endif

  #ifdef MOBILE
  if(dst <= 400 && !vid.usingGL) {
    if(polyi >= POLYMAX) return;
    polyx[polyi] = x1;
    polyy[polyi] = y1;
    polyi++;
    return;
    }
  #else
  #ifdef GFX
  if(dst <= 20 && col == -1) {
    if(polyi >= POLYMAX) return;
    polyx[polyi] = x1-s1;
    polyxr[polyi] = x1+s1;
    polyy[polyi] = y1;
    polyi++;
    return;
    }
  if(dst <= 20 && !vid.usingGL) {
    (vid.usingAA?aalineColor:lineColor) (s, x1, y1, x2, y2, col);
    return;
    }
  #endif
  if(dst <= 2) {
    return;
    }
  #endif
  
  hyperpoint H3 = mid(H1, H2);
  int x3, y3, s3; getcoord(H3, x3, y3, s3);
  #ifndef GFX
  if(!vid.usingGL) {
    qpixel(s, x3-s3, y3) |= col & 0xFF0000;
    qpixel(s, x3+s3, y3) |= col & 0x00FFFF;
    }
  #endif

  drawline(H1, x1, y1, s1, H3, x3, y3, col);
  drawline(H3, x3, y3, s3, H2, x2, y2, col);
  }

void fixcolor(int& col) {
  if(col != -1) {
    col = (col << 8) + lalpha;
    if(col == -1) col = -2;
    polyi = 0;
    #ifndef GFX
    if(!vid.usingGL) {
      SDL_LockSurface(s);
      col >>= 8;
      }
    #endif
    }
  }

void drawline(const hyperpoint& H1, const hyperpoint& H2, int col) {
  if(vid.usingGL && !pmodel) {
    qglcoords = 0;
    }

  dlit = 0;
  int x1, y1, s1; getcoord(H1, x1, y1, s1);
  int x2, y2, s2; getcoord(H2, x2, y2, s2);
  drawline(H1, x1, y1, s1, H2, x2, y2, col);

  if(vid.usingGL) {

    // EUCLIDEAN
    if(pmodel) {
      glcoords[qglcoords][0] = x2 - vid.xcenter;
      glcoords[qglcoords][1] = y2 - vid.ycenter;
      glcoords[qglcoords][2] = vid.scrdist;
      }
    else if(euclid) {
      for(int i=0; i<2; i++) glcoords[qglcoords][i] = H2[i]; glcoords[qglcoords][2] = EUCSCALE;
      }
    else {
      for(int i=0; i<3; i++) glcoords[qglcoords][i] = H2[i];
      }
    qglcoords++;
    
    if(pmodel) return;

    glVertexPointer(3, GL_FLOAT, 0, glcoords);
    glEnableClientState(GL_VERTEX_ARRAY);
    glcolor(col >> 8);
    
    if(vid.goteyes) {
      selectEyeGL(-1);
      selectEyeMask(-1);
      glDrawArrays(GL_LINE_STRIP, 0, qglcoords);
      selectEyeGL(+1);
      selectEyeMask(+1);
      glDrawArrays(GL_LINE_STRIP, 0, qglcoords);
      selectEyeGL(0);
      selectEyeMask(0);
      }
    else glDrawArrays(GL_LINE_STRIP, 0, qglcoords);
    }
  
  #ifdef MOBILE
  else if(col != -1) {
    gdpush(3); gdpush(col); 
    gdpush(polyi+1);
    for(int i=0; i<polyi; i++) gdpush(polyx[i]), gdpush(polyy[i]);
    gdpush(x2), gdpush(y2);
    }
  #endif
  #ifndef GFX
  if(col != -1) {
    SDL_UnlockSurface(s);
    }
  #endif
  // printf("ok\n");
  }

// game-related graphics

transmatrix View; // current rotation, relative to viewctr
transmatrix cwtV; // player-relative view
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

int firecolor(int phase) {
  return gradient(0xFFFF00, 0xFF0000, -1, sin((phase + ticks)/100.0), 1);
  }

int watercolor(int phase) {
  return 0x0080C0FF + 256 * int(63 * sin((ticks + phase) / 50.));
  }

int aircolor(int phase) {
  return 0x8080FF00 | int(32 + 32 * sin(ticks/200.0 + 2 * phase * M_PI / 21.));
  }

int weakfirecolor(int phase) {
  return gradient(0xFF8000, 0xFF0000, -1, sin((phase + ticks)/500.0), 1);
  }

int fc(int ph, int col, int z) {
  if(items[itOrbFire]) col = darkena(firecolor(ph), 0, 0xFF);
  if(items[itOrbGhost]) col = (col &~0XFF) | (col&0xFF) / 2;
  if(items[itOrbFish] && isWatery(cwt.c) && z != 3) return watercolor(ph);
  if(invismove) 
    col = 
      shmup::on ?
        (col &~0XFF) | (int((col&0xFF) * .25))
      : (col &~0XFF) | (int((col&0xFF) * (100+100*sin(ticks / 500.)))/200);
  return col;
  }

int flashat, bigflashat, lightat, safetyat;
cell *flashcell;

void drawFlash(cell *c) { flashat = ticks; flashcell = c; }
void drawBigFlash(cell *c) { bigflashat = ticks; flashcell = c; }
void drawLightning() { lightat = ticks; }
void drawSafety() { safetyat = ticks; }

double eurad = 0.52;

bool outofmap(hyperpoint h) {
  if(euclid) 
    return h[0] * h[0] + h[1] * h[1] > 15 * eurad;
  else
    return h[2] < .5;
  }

void drawShield(const transmatrix& V, eItem it) {
  float ds = ticks / 300.;
  int col = darkened(iinf[it].color);
  if(it == itOrbShield && items[itOrbPreserve] && !orbused[it])
    col = (col & 0xFEFEFE) / 2;
  double d = it == itOrbShield ? hexf : hexf - .1;
  for(int a=0; a<84*5; a++)
    queueline(V*ddi(a, d + sin(ds + M_PI*2*a/20)*.1)*C0, V*ddi((a+1), d + sin(ds + M_PI*2*(a+1)/20)*.1)*C0, col);
  }

void drawShell(const transmatrix& V) {
  float ds = ticks / 300.;
  int col = darkened(iinf[itOrbShell].color);
  for(int a=0; a<84*5; a++)
    queueline(V*ddi(a, hexf + sin(ds + M_PI*2*a/20)*.1)*C0, V*ddi((a+1), hexf + sin(ds + M_PI*2*(a+1)/20)*.1)*C0, col);
  }

void drawSpeed(const transmatrix& V) {
  ld ds = ticks / 10.;
  int col = darkened(iinf[itOrbSpeed].color);
  for(int b=0; b<84; b+=14)
  for(int a=0; a<84; a++)
    queueline(V*ddi(ds+b+a, hexf*a/84)*C0, V*ddi(ds+b+(a+1), hexf*(a+1)/84)*C0, col);
  }

void drawSafety(const transmatrix& V, int ct) {
  ld ds = ticks / 50.;
  int col = darkened(iinf[itOrbSafety].color);
  for(int a=0; a<ct; a++)
    queueline(V*ddi(ds+a*84/ct, 2*hexf)*C0, V*ddi(ds+(a+(ct-1)/2)*84/ct, 2*hexf)*C0, col);
  }

void drawFlash(const transmatrix& V) {
  float ds = ticks / 300.;
  int col = darkened(iinf[itOrbFlash].color);
  col &= ~1;
  for(int u=0; u<5; u++) {
    ld rad = hexf * (2.5 + .5 * sin(ds+u*.3));
    for(int a=0; a<84; a++)
      queueline(V*ddi(a, rad)*C0, V*ddi(a+1, rad)*C0, col);
    }
  }

void drawLove(const transmatrix& V, int hdir) {
  float ds = ticks / 300.;
  int col = darkened(iinf[itOrbLove].color);
  col &= ~1;
  for(int u=0; u<5; u++) {
    for(int a=0; a<84; a++) {
      double d = (1 + cos(a * M_PI/42)) / 2;
      int z = a; if(z>42) z = 84-z;
      if(z <= 10) d += (10-z) * (10-z) * (10-z) / 3000.;

      ld rad = hexf * (2.5 + .5 * sin(ds+u*.3)) * d;
      queueline(V*ddi(hdir+a-1, rad)*C0, V*ddi(hdir+a+1, rad)*C0, col);
      }
    }
  }

void drawWinter(const transmatrix& V, int hdir) {
  float ds = ticks / 300.;
  int col = darkened(iinf[itOrbWinter].color);
  for(int u=0; u<20; u++) {
    ld rad = 6 * sin(ds+u * 2 * M_PI / 20);
    queueline(V*ddi(hdir+rad, hexf*.5)*C0, V*ddi(hdir+rad, hexf*3)*C0, col);
    }
  }

void drawLightning(const transmatrix& V) {
  int col = iinf[itOrbLightning].color;
  for(int u=0; u<20; u++) {
    ld leng = 0.5 / (0.1 + (rand() % 100) / 100.0);
    ld rad = rand() % 84;
    queueline(V*ddi(rad, hexf*0.3)*C0, V*ddi(rad, hexf*leng)*C0, col);
    }
  }

int displaydir(cell *c, int d) {
  if(euclid)
    return - d * 84 / c->type;
  else
    return 42 - d * 84 / c->type;
  }

#include "shmup.cpp"
#include "rug.cpp"
#include "conformal.cpp"

void drawPlayerEffects(const transmatrix& V, cell *c, bool onplayer) {
  if(!onplayer && !items[itOrbEmpathy]) return;
  if(items[itOrbShield] > (shmup::on ? 0 : 1)) drawShield(V, itOrbShield);
  if(items[itOrbShell] > (shmup::on ? 0 : 1)) drawShield(V, itOrbShell);

  if(items[itOrbSpeed]) drawSpeed(V); 

  int ct = c->type;
  
  if(onplayer && items[itOrbSafety]) drawSafety(V, ct);

  if(onplayer && items[itOrbFlash]) drawFlash(V); 
  if(onplayer && items[itOrbLove]) drawLove(V, displaydir(c, cwt.spin)); 

  if(items[itOrbWinter]) 
    drawWinter(V, displaydir(c, cwt.spin));
  
  if(onplayer && items[itOrbLightning] > 1) drawLightning(V);
  
  if(safetyat > 0) {
    int tim = ticks - safetyat;
    if(tim > 2500) safetyat = 0;
    for(int u=tim; u<=2500; u++) {
      if((u-tim)%250) continue;
      ld rad = hexf * u / 250;
      int col = iinf[itOrbSafety].color;
      for(int a=0; a<84; a++)
        queueline(V*ddi(a, rad)*C0, V*ddi(a+1, rad)*C0, col);
      }
    }
  }

void drawStunStars(const transmatrix& V, int t) {
  for(int i=0; i<3*t; i++) {
    transmatrix V2 = V * spin(M_PI * 2 * i / (3*t) + M_PI * ticks/600.);
    queuepoly(V2, shFlailBall, 0xFFFFFFFF);
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
      queuepoly(V, sh, ds.color ? darkena(ds.color, 0, 0xFF) : color);
    }
  
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
    
      int xc, yc, sc;
      getcoord(P2, xc, yc, sc);
      queuechr(xc, yc, sc, 10, 'x', 0xFF00FF);

      /* if(crad > 0 && c->cpdist <= 3) {
        lalpha = 0x80;
        transmatrix movtocc = V2 * inverse(cwtV) * rgpushxto0(ccenter);
        for(int d=0; d<84; d++) 
          queueline(movtocc * ddi(d+1, crad) * C0, movtocc * ddi(d, crad) * C0, 0xC00000);
        lalpha = 0xFF;
        } */
      }
    }
    
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
      /* poly_outline = 0;
      queuepoly(V, shHeptaMarker, darkena(mcol, 0, dd));
      poly_outline = OUTLINE_NONE; */
    return gradient(0x487830, mcol, 0, dd, 0xFF);
    }
  };

bool drawMonsterType(eMonster m, cell *where, const transmatrix& V, int col) {

  char xch = minf[m].glyph;    

#ifndef MOBILE
  if(where == mapeditor::drawcell)
    mapeditor::drawtrans = V;
#endif

  if(m == moTortoise && where->stuntime >= 3)
    drawStunStars(V, where->stuntime-2);
  else if (m == moTortoise || m == moPlayer || !where->stuntime) ;
  else if(!(isMetalBeast(m) && where->stuntime == 1))
    drawStunStars(V, where->stuntime);
    
  if(m == moTortoise) {
    int bits = tortoise::getb(where);
    tortoise::draw(V, bits, 0, where->stuntime);
    if(tortoise::seek() && !tortoise::diff(bits))
      queuepoly(V, shRing, darkena(0xFFFFFF, 0, 0x80 + 0x70 * sin(ticks / 200.0)));
    }
    
  else if(m == moPlayer) {
  
    charstyle& cs = getcs();
      
    bool havus = drawUserShape(V, 0, cs.charid, cs.skincolor);

    if(mapeditor::drawplayer && !havus) {
      
      if(cs.charid >= 6) {
        queuepoly(V, shWolf, fc(0, cs.skincolor, 0));
        if(!shmup::on || shmup::curtime >= shmup::getPlayer()->nextshot) {
          queuepoly(V, shWolf1, fc(314, cs.swordcolor, 3));
          queuepoly(V, shWolf2, fc(314, cs.swordcolor, 3));
          queuepoly(V, shWolf3, fc(314, cs.swordcolor, 3));
          }
        }
      else if(cs.charid >= 4) {
        queuepoly(V, shCatBody, fc(0, cs.skincolor, 0));
        queuepoly(V, shCatHead, fc(150, cs.haircolor, 2));
        queuepoly(V, shCatLegs, fc(500, cs.dresscolor, 4));
        if(!shmup::on || shmup::curtime >= shmup::getPlayer()->nextshot) {
          queuepoly(V * xpush(.04), shWolf1, fc(314, cs.swordcolor, 3));
          queuepoly(V * xpush(.04), shWolf2, fc(314, cs.swordcolor, 3));
          }
        }
      else {

      queuepoly(V, (cs.charid&1) ? shFemaleBody : shPBody, fc(0, cs.skincolor, 0));

      if(items[itOrbThorns])
        queuepoly(V, shHedgehogBladePlayer, items[itOrbDiscord] ? watercolor(0) : 0x00FF00FF);
      else if(!shmup::on && items[itOrbDiscord])
        queuepoly(V, shPSword, watercolor(0));
      else if(items[itRevolver])
        queuepoly(V, shGunInHand, fc(314, cs.swordcolor, 3)); // 3 not colored
      else if(!shmup::on)
        queuepoly(V, cs.charid >= 2 ? shSabre : shPSword, fc(314, cs.swordcolor, 3)); // 3 not colored
      else if(shmup::curtime >= shmup::getPlayer()->nextshot)
        queuepoly(V, shPKnife, fc(314, cs.swordcolor, 3)); // 3 not colored

      if(where->land == laWildWest) {
        queuepoly(V, shWestHat1, darkena(cs.swordcolor, 1, 0XFF));
        queuepoly(V, shWestHat2, darkena(cs.swordcolor, 0, 0XFF));
        }

      if(cheater) {
        queuepoly(V, (cs.charid&1) ? shGoatHead : shDemon, darkena(0xFFFF00, 0, 0xFF));
        // queuepoly(V, shHood, darkena(0xFF00, 1, 0xFF));
        }
      else {
        queuepoly(V, shPFace, fc(500, cs.skincolor, 1));
        queuepoly(V, (cs.charid&1) ? shFemaleHair : shPHead, fc(150, cs.haircolor, 2));
        }
      if(cs.charid&1)
        queuepoly(V, shFemaleDress, fc(500, cs.dresscolor, 4));

      if(cs.charid == 2)
        queuepoly(V, shPrinceDress,  fc(400, cs.dresscolor, 5));
      if(cs.charid == 3) 
        queuepoly(V, shPrincessDress,  fc(400, cs.dresscolor2, 5));
      
      }

      if(knighted)
        queuepoly(V, shKnightCloak, darkena(cloakcolor(knighted), 1, 0xFF));

      if(items[itOrbFish])
        queuepoly(V, shFishTail, watercolor(100));

      if(tortoise::seek())
        tortoise::draw(V * ypush(-crossf*.25), tortoise::seekbits, 4, 0);

      }
    }
  
  else if(drawUserShape(V, 1, m, darkena(col, 0, 0xFF))) return false;

  else if(isMimic(m) || m == moShadow || m == moIllusion) {
    charstyle& cs = getcs();
    if(drawUserShape(V, 0, (cs.charid&1)?1:0, darkena(col, 0, 0x80))) return false;
    
    if(cs.charid >= 6) {
      queuepoly(V, shWolf, darkena(col, 0, 0xC0));
      queuepoly(V, shWolf1, darkena(col, 1, 0xC0));
      queuepoly(V, shWolf2, darkena(col, 1, 0xC0));
      queuepoly(V, shWolf3, darkena(col, 1, 0xC0));
      }
    else if(cs.charid >= 4) {
      queuepoly(V, shCatBody, darkena(col, 0, 0xC0));
      queuepoly(V, shCatHead, darkena(col, 0, 0xC0));
      queuepoly(V, shCatLegs, darkena(col, 0, 0xC0));
      queuepoly(V * xpush(.04), shWolf1, darkena(col, 1, 0xC0));
      queuepoly(V * xpush(.04), shWolf2, darkena(col, 1, 0xC0));
      }
    else {
      queuepoly(V, (cs.charid&1) ? shFemaleBody : shPBody,  darkena(col, 0, 0X80));
  
      if(!shmup::on)
        queuepoly(V, (cs.charid >= 2 ? shSabre : shPSword), darkena(col, 0, 0XC0));
      else if(shmup::curtime >= shmup::getPlayer()->nextshot)
        queuepoly(V, shPKnife, darkena(col, 0, 0XC0));
  
      queuepoly(V, (cs.charid&1) ? shFemaleHair : shPHead,  darkena(col, 1, 0XC0));
      queuepoly(V, shPFace,  darkena(col, 0, 0XC0));
      if(cs.charid&1)
        queuepoly(V, shFemaleDress,  darkena(col, 1, 0XC0));
      if(cs.charid == 2)
        queuepoly(V, shPrinceDress,  darkena(col, 1, 0XC0));
      if(cs.charid == 3)
        queuepoly(V, shPrincessDress,  darkena(col, 1, 0XC0));
      }
    }
  
/*   else if(m == moShadow) {
    charstyle& cs = getcs();
    queuepoly(V, (cs.charid&1) ? shFemaleBody : shPBody,  darkena(col, 0, 0X80));
    queuepoly(V, (cscharid >= 2 ? shSabre : shPSword), darkena(col, 0, 0XC0));
    queuepoly(V, (cs.charid&1) ? shFemaleHair : shPHead,  darkena(col, 1, 0XC0));
    queuepoly(V, shPFace,  darkena(col, 0, 0XC0));

    if(cs.charid&1)
      queuepoly(V, shFemaleDress, darkena(col, 1, 0xC0));
    if(cs.charid == 2)
      queuepoly(V, shPrinceDress,  darkena(col, 1, 0XC0));
    if(cs.charid == 3)
      queuepoly(V, shPrincessDress,  darkena(col, 1, 0XC0));
    }

  else if(m == moIllusion) {
    charstyle& cs = getcs();
    if(drawUserShape(V, 0, (cs.charid&1)?1:0, darkena(col, 0, 0x80))) return false;
    queuepoly(V, (cs.charid&1) ? shFemaleBody : shPBody,  darkena(col, 0, 0X80));
    queuepoly(V, (cscharid >= 2 ? shSabre : shPSword), darkena(col, 0, 0XC0));
    queuepoly(V, (cs.charid&1) ? shFemaleHair : shPHead,  darkena(col, 1, 0XC0));
    queuepoly(V, shPFace,  darkena(col, 0, 0XC0));
    if(cs.charid&1)
      queuepoly(V, shFemaleDress,  darkena(col, 1, 0XC0));
    if(cs.charid == 2)
      queuepoly(V, shPrinceDress,  darkena(col, 1, 0XC0));
    if(cs.charid == 3)
      queuepoly(V, shPrincessDress,  darkena(col, 1, 0XC0));
    } */

  else if(m == moBullet) {
    queuepoly(V * spin(-M_PI/4), shKnife, getcs().swordcolor);
    }
  
  else if(m == moKnight || m == moKnightMoved) {
    queuepoly(V, shPBody, darkena(0xC0C0A0, 0, 0xC0));
    queuepoly(V, shPSword, darkena(0xFFFF00, 0, 0xFF));
    queuepoly(V, shKnightArmor, darkena(0xD0D0D0, 1, 0xFF));
    int col;
    if(!euclid && where->master->alt)
      col = cloakcolor(roundTableRadius(where));
    else
      col = cloakcolor(newRoundTableRadius());
    queuepoly(V, shKnightCloak, darkena(col, 1, 0xFF));
    queuepoly(V, shPHead, darkena(0x703800, 1, 0XFF));
    queuepoly(V, shPFace, darkena(0xC0C0A0, 0, 0XFF));
    return false;
    }
  
  else if(m == moGolem) {
    queuepoly(V, shPBody, darkena(col, 0, 0XC0));
    queuepoly(V, shGolemhead, darkena(col, 1, 0XFF));
    }

  else if(isPrincess(m) || m == moFalsePrincess || m == moRoseLady || m == moRoseBeauty) {
  
    bool girl = princessgender() == GEN_F;
    bool evil = !isPrincess(m);

    int facecolor = evil ? 0xC0B090FF : 0xD0C080FF;
    
    queuepoly(V, girl ? shFemaleBody : shPBody, facecolor);

    if(m == moPrincessArmed) 
      queuepoly(V, vid.cs.charid < 2 ? shSabre : shPSword, 0xFFFFFFFF);
    
    if((m == moFalsePrincess || m == moRoseBeauty) && where->cpdist == 1)
      queuepoly(V, shPKnife, 0xFFFFFFFF);

    if(m == moRoseLady) {
      queuepoly(V, shPKnife, 0xFFFFFFFF);
      queuepoly(V * Mirror, shPKnife, 0xFFFFFFFF);
      }

    if(m == moRoseLady) {
//    queuepoly(V, girl ? shGoatHead : shDemon,  0x800000FF);
      queuepoly(V, girl ? shFemaleHair : shPHead,  evil ? 0x500050FF : 0x332A22FF);
      }
    else if(m == moRoseBeauty) {
      if(girl) {
        queuepoly(V, shBeautyHair,  0xF0A0D0FF);
        queuepoly(V, shFlowerHair,  0xC00000FF);
        }
      else {
        queuepoly(V, shPHead,  0xF0A0D0FF);
        queuepoly(V, shFlowerHand,  0xC00000FF);
        queuepoly(V, shSuspenders,  0xC00000FF);
        }
      }
    else {
      queuepoly(V, girl ? shFemaleHair : shPHead,  
        evil ? 0xC00000FF : 0x332A22FF);
      }
    queuepoly(V, shPFace,  facecolor);

    if(girl) {
      queuepoly(V, shFemaleDress,  evil ? 0xC000C0FF : 0x00C000FF);
      if(vid.cs.charid < 2) 
        queuepoly(V, shPrincessDress, evil ? 0xC040C0C0 : 0x8080FFC0);
      }
    else {
      if(vid.cs.charid < 2) 
        queuepoly(V, shPrinceDress,  evil ? 0x802080FF : 0x404040FF);
      }    
    }

  else if(m == moWolf || m == moRedFox) {
    queuepoly(V, shWolfLegs, darkena(col, 0, 0xFF));
    queuepoly(V, shWolfBody, darkena(col, 0, 0xFF));
    queuepoly(V, shWolfHead, darkena(col, 0, 0xFF));
    queuepoly(V, shWolfEyes, darkena(col, 3, 0xFF));
    }
  else if(m == moVineBeast) {
    queuepoly(V, shWolfLegs, 0x00FF00FF);
    queuepoly(V, shWolfBody, darkena(col, 1, 0xFF));
    queuepoly(V, shWolfHead, darkena(col, 0, 0xFF));
    queuepoly(V, shWolfEyes, 0xFF0000FF);
    }
  else if(m == moMouse || m == moMouseMoved) {
    queuepoly(V, shMouse, darkena(col, 0, 0xFF));
    queuepoly(V, shMouseLegs, darkena(col, 1, 0xFF));
    queuepoly(V, shMouseEyes, 0xFF);
    }
  else if(isBug(m)) {
    queuepoly(V, shBugBody, darkena(col, 0, 0xFF));
    queuepoly(V, shBugArmor, darkena(col, 1, 0xFF));
    }
  else if(m == moRunDog) {
    queuepoly(V, shWolf, darkena(col, 0, 0xFF));
    queuepoly(V, shWolf1, darkena(0x202020, 0, 0xFF));
    queuepoly(V, shWolf2, darkena(0x202020, 0, 0xFF));
    queuepoly(V, shWolf3, darkena(0x202020, 0, 0xFF));
    }
  else if(m == moOrangeDog) {
    queuepoly(V, shWolf, darkena(0xFFFFFF, 0, 0xFF));
    queuepoly(V, shWolf1, darkena(0x202020, 0, 0xFF));
    queuepoly(V, shWolf2, darkena(0x202020, 0, 0xFF));
    queuepoly(V, shWolf3, darkena(0x202020, 0, 0xFF));
    queuepoly(V, shDogStripes, darkena(0x303030, 0, 0xFF));
    }
  else if(m == moShark || m == moGreaterShark || m == moCShark)
    queuepoly(V, shShark, darkena(col, 0, 0xFF));
  else if(m == moEagle || m == moParrot || m == moBomberbird || m == moAlbatross || 
    m == moTameBomberbird || m == moWindCrow)
    queuepoly(V, shEagle, darkena(col, 0, 0xFF));
  else if(m == moNighthawk || m == moKestrel)
    queuepoly(V, shHawk, darkena(col, 0, 0xFF));
  else if(m == moGargoyle) {
    queuepoly(V, shGargoyleWings, darkena(col, 0, 0xD0));
    queuepoly(V, shGargoyleBody, darkena(col, 0, 0xFF));
    }
  else if(m == moZombie)
    queuepoly(V, shPBody, darkena(col, 0, 0xFF));
  else if(m == moDesertman) {
    queuepoly(V, shPBody, darkena(col, 0, 0xC0));
    queuepoly(V, shPSword, 0xFFFF00FF);
    queuepoly(V, shHood, 0xD0D000C0);
    }
  else if(m == moPalace || m == moFatGuard || m == moVizier || m == moSkeleton) {
    queuepoly(V, shSabre, 0xFFFFFFFF);
    if(m == moSkeleton) {
      queuepoly(V, shSkeletonBody, darkena(0xFFFFFF, 0, 0xFF));
      queuepoly(V, shSkull, darkena(0xFFFFFF, 0, 0xFF));
      queuepoly(V, shSkullEyes, darkena(0, 0, 0xFF));
      }
    else {
      if(m == moFatGuard) {
        queuepoly(V, shFatBody, darkena(0xC06000, 0, 0xFF));
        col = 0xFFFFFF;
        if(where->hitpoints >= 3)
          queuepoly(V, shKnightCloak, darkena(0xFFC0C0, 1, 0xFF));
        }
      else {
        queuepoly(V, shPBody, darkena(0xFFD500, 0, 0xFF));
        queuepoly(V, shKnightArmor, m == moVizier ? 0xC000C0FF :
          darkena(0x00C000, 1, 0xFF));
        if(where->hitpoints >= 3)
          queuepoly(V, shKnightCloak, m == moVizier ? 0x800080Ff :
            darkena(0x00FF00, 1, 0xFF));
        }
      queuepoly(V, shTurban1, darkena(col, 1, 0xFF));
      if(where->hitpoints >= 2)
        queuepoly(V, shTurban2, darkena(col, 0, 0xFF));
      }
      
    drawStunStars(V, where->stuntime);
    }
  else if(m == moCrystalSage) {
    queuepoly(V, shPBody, 0xFFFFFFFF);
    queuepoly(V, shPHead, 0xFFFFFFFF);
    queuepoly(V, shPFace, 0xFFFFFFFF);
    }
  else if(m == moHedge) {
    queuepoly(V, shPBody, darkena(col, 0, 0xFF));
    queuepoly(V, shHedgehogBlade, 0xC0C0C0FF);
    queuepoly(V, shPHead, 0x804000FF);
    queuepoly(V, shPFace, 0xF09000FF);
    }
  else if(m == moYeti || m == moMonkey) {
    queuepoly(V, shYeti, darkena(col, 0, 0xC0));
    queuepoly(V, shPHead, darkena(col, 0, 0xFF));
    }
  else if(m == moLemur) {
    queuepoly(V, shPBody, darkena(0xFFFF00, 0, 0xC0));
    queuepoly(V, shAztecHead, darkena(col, 0, 0xFF));
    queuepoly(V, shAztecCap, darkena(0xC000C0, 0, 0xFF));
    }
  else if(m == moEdgeMonkey) {
    queuepoly(V, shYeti, darkena(0xC04040, 0, 0xC0));
    queuepoly(V, shPHead, darkena(col, 0, 0xFF));
    }
  else if(m == moRanger) {
    queuepoly(V, shPBody, darkena(col, 0, 0xC0));
    queuepoly(V, shPSword, darkena(col, 0, 0xFF));
    queuepoly(V, shArmor, darkena(col, 1, 0xFF));
    }
  else if(m == moGhost || m == moSeep || m == moFriendlyGhost) {
    queuepoly(V, shGhost, darkena(col, 0, 0x80));
    queuepoly(V, shEyes, 0xFF);
    }
  else if(m == moVineSpirit) {
    queuepoly(V, shGhost, 0xD0D0D0C0);
    queuepoly(V, shEyes, 0xFF0000FF);
    }
  else if(m == moFireFairy) {
    col = firecolor(0);
    queuepoly(V, shFemaleBody, darkena(col, 0, 0XC0));
    queuepoly(V, shWitchHair, darkena(col, 1, 0xFF));
    queuepoly(V, shPFace, darkena(col, 0, 0XFF));
    }
  else if(m == moSlime) {
    queuepoly(V, shSlime, darkena(col, 0, 0x80));
    queuepoly(V, shEyes, 0xFF);
    }
  else if(m == moCultist || m == moPyroCultist || m == moCultistLeader) {
    queuepoly(V, shPBody, darkena(col, 0, 0xC0));
    queuepoly(V, shPSword, darkena(col, 2, 0xFF));
    queuepoly(V, shHood, darkena(col, 1, 0xFF));
    }
  else if(m == moPirate) {
    queuepoly(V, shPBody, darkena(0x404040, 0, 0xFF));
    queuepoly(V, shPirateHook, darkena(0xD0D0D0, 0, 0xFF));
    queuepoly(V, shPFace, darkena(0xFFFF80, 0, 0xFF));
    queuepoly(V, shEyepatch, darkena(0, 0, 0xC0));
    queuepoly(V, shPirateHood, darkena(col, 0, 0xFF));
    }
  else if(m == moRatling || m == moRatlingAvenger) {
    queuepoly(V, shYeti, darkena(col, 1, 0xFF));
    queuepoly(V, shRatHead, darkena(col, 0, 0xFF));
    queuepoly(V, shRatTail, darkena(col, 0, 0xFF));

    float t = sin(ticks / 1000.0 + where->cpdist);
    int eyecol = t > 0.92 ? 0xFF0000 : 0;
    
    queuepoly(V, shWolf1, darkena(eyecol, 0, 0xFF));
    queuepoly(V, shWolf2, darkena(eyecol, 0, 0xFF));
    queuepoly(V, shWolf3, darkena(0x202020, 0, 0xFF));
    
    if(m == moRatlingAvenger) {
      queuepoly(V, shRatCape1, 0x303030FF);
      queuepoly(V, shRatCape2, 0x484848FF);
      }
    }
  else if(m == moViking) {
    queuepoly(V, shPBody, darkena(0xE00000, 0, 0xFF));
    queuepoly(V, shPSword, darkena(0xD0D0D0, 0, 0xFF));
    queuepoly(V, shKnightCloak, darkena(0x404040, 0, 0xFF));
    queuepoly(V, shVikingHelmet, darkena(0xC0C0C0, 0, 0XFF));
    queuepoly(V, shPFace, darkena(0xFFFF80, 0, 0xFF));
    }
  else if(m == moOutlaw) {
    queuepoly(V, shPBody, darkena(col, 0, 0xFF));
    queuepoly(V, shKnightCloak, darkena(col, 1, 0xFF));
    queuepoly(V, shWestHat1, darkena(col, 2, 0XFF));
    queuepoly(V, shWestHat2, darkena(col, 1, 0XFF));
    queuepoly(V, shPFace, darkena(0xFFFF80, 0, 0xFF));
    queuepoly(V, shGunInHand, darkena(col, 1, 0XFF));
    }
  else if(m == moNecromancer) {
    queuepoly(V, shPBody, 0xC00000C0);
    queuepoly(V, shHood, darkena(col, 1, 0xFF));
    }
  else if(m == moGoblin) {
    queuepoly(V, shYeti, darkena(col, 0, 0xC0));
    queuepoly(V, shArmor, darkena(col, 1, 0XFF));
    }
  else if(m == moLancer || m == moFlailer || m == moMiner) {
    transmatrix V2 = V;
    if(m == moLancer)
      V2 = V * spin(where->type == 6 ? -M_PI/3 : -M_PI/2 );
    queuepoly(V2, shPBody, darkena(col, 0, 0xC0));
    queuepoly(V2, m == moFlailer ? shArmor : shHood, darkena(col, 1, 0XFF));
    if(m == moMiner)
      queuepoly(V2, shPickAxe, darkena(0xC0C0C0, 0, 0XFF));
    if(m == moLancer)
      queuepoly(V2, shPike, darkena(col, 0, 0XFF));
    if(m == moFlailer) {
      queuepoly(V2, shFlailBall, darkena(col, 0, 0XFF));
      queuepoly(V2, shFlailChain, darkena(col, 1, 0XFF));
      queuepoly(V2, shFlailTrunk, darkena(col, 0, 0XFF));
      }
    }
  else if(m == moTroll) {
    queuepoly(V, shYeti, darkena(col, 0, 0xC0));
    queuepoly(V, shPHead, darkena(col, 1, 0XFF));
    queuepoly(V, shPFace, darkena(col, 2, 0XFF));
    }        
  else if(m == moFjordTroll || m == moForestTroll || m == moStormTroll) {
    queuepoly(V, shYeti, darkena(col, 0, 0xC0));
    queuepoly(V, shPHead, darkena(col, 1, 0XFF));
    queuepoly(V, shPFace, darkena(col, 2, 0XFF));
    }        
  else if(m == moDarkTroll) {
    queuepoly(V, shYeti, darkena(col, 0, 0xC0));
    queuepoly(V, shPHead, darkena(col, 1, 0XFF));
    queuepoly(V, shPFace, 0xFFFFFF80);
    }        
  else if(m == moRedTroll) {
    queuepoly(V, shYeti, darkena(col, 0, 0xC0));
    queuepoly(V, shPHead, darkena(0xFF8000, 0, 0XFF));
    queuepoly(V, shPFace, 0xFFFFFF80);
    }        
  else if(m == moEarthElemental) {
    queuepoly(V, shYeti, darkena(col, 0, 0xC0));
    queuepoly(V, shPHead, darkena(col, 0, 0XFF));
    queuepoly(V, shPFace, 0xF0000080);
    }        
  else if(m == moWaterElemental) {
    queuepoly(V, shWaterElemental, watercolor(0));
    queuepoly(V, shFemaleHair, watercolor(100));
    queuepoly(V, shPFace, watercolor(200));
    }        
  else if(m == moFireElemental) {
    queuepoly(V, shWaterElemental, darkena(firecolor(0), 0, 0xFF));
    queuepoly(V, shFemaleHair, darkena(firecolor(100), 0, 0xFF));
    queuepoly(V, shPFace, darkena(firecolor(200), 0, 0xFF));
    }
  else if(m == moAirElemental) {
    queuepoly(V, shWaterElemental, darkena(col, 0, 0x80));
    queuepoly(V, shFemaleHair, darkena(col, 0, 0x80));
    queuepoly(V, shPFace, darkena(col, 0, 0x80));
    }        
  else if(xch == 'd' || xch == 'D') {
    queuepoly(V, shPBody, darkena(col, 1, 0xC0));
    int acol = col;
    if(xch == 'D') acol = 0xD0D0D0;
    queuepoly(V, shDemon, darkena(acol, 0, 0xFF));
    }
  else if(isMetalBeast(m)) {
    queuepoly(V, shTrylobite, darkena(col, 1, 0xC0));
    int acol = col;
    queuepoly(V, shTrylobiteHead, darkena(acol, 0, 0xFF));
    }
  else if(m == moEvilGolem) {
    queuepoly(V, shPBody, darkena(col, 0, 0XC0));
    queuepoly(V, shGolemhead, darkena(col, 1, 0XFF));
    }
  else if(isWitch(m)) {
    int c = 0xFF;
    if(m == moWitchGhost) c = 0x85 + 120 * sin(ticks / 160.0);
    if(m == moWitchWinter) drawWinter(V, 42);
    if(m == moWitchFlash) drawFlash(V);
    if(m == moWitchSpeed) drawSpeed(V);
    if(m == moWitchFire) col = firecolor(0);
    queuepoly(V, shFemaleBody, darkena(col, 0, c));
//    queuepoly(cV2, ct, shPSword, darkena(col, 0, 0XFF));
//    queuepoly(V, shHood, darkena(col, 0, 0XC0));
    if(m == moWitchFire) col = firecolor(100);
    queuepoly(V, shWitchHair, darkena(col, 1, c));
    if(m == moWitchFire) col = firecolor(200);
    queuepoly(V, shPFace, darkena(col, 0, c));
    if(m == moWitchFire) col = firecolor(300);
    queuepoly(V, shWitchDress, darkena(col, 1, 0XC0));
    }

  else return true;
  
  return false;
  }

#define OUTLINE_NONE     0x000000FF
#define OUTLINE_FRIEND   0x00FF00FF
#define OUTLINE_ENEMY    0xFF0000FF
#define OUTLINE_TREASURE 0xFFFF00FF
#define OUTLINE_ORB      0xFF8000FF
#define OUTLINE_OTHER    0xFFFFFFFF
#define OUTLINE_DEAD     0x800000FF

bool drawMonsterTypeDH(eMonster m, cell *where, const transmatrix& V, int col, bool dh) {
  if(dh) {
    poly_outline = OUTLINE_DEAD;
    darken++;
    }
  bool b = drawMonsterType(m,where,V,col);
  if(dh) {
    poly_outline = OUTLINE_NONE;
    darken--;
    }
  return b;
  }

bool drawMonster(const transmatrix& V, int ct, cell *c, int col) {

  if(shmup::on) shmup::drawMonster(V, c);
  
  bool darkhistory = conformal::includeHistory && eq(c->aitmp, sval);
  
  if(doHighlight())
    poly_outline = 
      (c == cwt.c || isFriendly(c)) ? OUTLINE_FRIEND : OUTLINE_ENEMY;

  eMonster m = c->monst;
    
  if(c == cwt.c && !shmup::on && mapeditor::drawplayer) {
    transmatrix cV2 = cwtV;
    // if(flipplayer) cV2 = cV2 * spin(M_PI);
    if(flipplayer) cV2 = cV2 * spin(M_PI);
    
    drawPlayerEffects(V, c, true);
    if(vid.monmode > 1) {
      drawMonsterType(moPlayer, c, cV2, col);
      }
    else return true;
    }

  if(isIvy(c) || isWorm(c) || isMutantIvy(c)) {
    
    transmatrix V2 = V;
    if(isDragon(c->monst) && c->stuntime == 0) col = 0xFF6000;
    
    if(c->mondir != NODIR) {
      int hdir = displaydir(c, c->mondir);
      
      if(vid.monmode > 1) {
        V2 = V2 * spin(hdir * M_PI / 42);

#ifndef MOBILE
        if(c == mapeditor::drawcell) mapeditor::drawtrans = V2;
#endif

        if(drawUserShape(V2, 1, c->monst, (col << 8) + 0xFF)) return false;

        if(isIvy(c) || isMutantIvy(c))
          queuepoly(V2, shIBranch, (col << 8) + 0xFF);
        else if(c->monst < moTentacle) {
          queuepoly(V2, shTentacleX, 0xFF);
          queuepoly(V2, shTentacle, (col << 8) + 0xFF);
          }
        else if(c->monst == moDragonHead || c->monst == moDragonTail) {
          char part = dragon::bodypart(c, dragon::findhead(c));
          if(part != '2') queuepoly(V2, shDragonSegment, darkena(col, 0, 0xFF));
          }
        else {
          if(c->monst == moTentacleGhost) {
            hyperpoint V0 = conformal::on ? V*C0 : inverse(cwtV) * V * C0;
            hyperpoint V1 = spintox(V0) * V0;
            transmatrix VL = cwtV * rspintox(V0) * rpushxto0(V1) * spin(M_PI);
            drawMonsterType(moGhost, c, VL, darkena(col, 0, 0xFF));
            col = minf[moTentacletail].color;
            }
          queuepoly(V2, shTentacleX, 0xFFFFFFFF);
          queuepoly(V2, shTentacle, (col << 8) + 0xFF);
          }
        }
        
      else for(int u=-1; u<=1; u++)
        queueline(V*ddi(hdir+21, u*crossf/5)*C0, V*ddi(hdir, crossf)*ddi(hdir+21, u*crossf/5)*C0, 0x606020 >> darken);
      }

    if(vid.monmode > 1) {
      if(isIvy(c) || isMutantIvy(c)) 
        queuepoly(V, shILeaf[ct-6], darkena(col, 0, 0xFF));
      else if(m == moWorm || m == moWormwait || m == moHexSnake) {
        queuepoly(V2 * spin(M_PI), shWormHead, darkena(col, 0, 0xFF));
        queuepoly(V2 * spin(M_PI), shDragonEyes, 0xFF);
        }
      else if(m == moDragonHead) {
        queuepoly(V2, shDragonHead, darkena(col, c->hitpoints?0:1, 0xFF));
        queuepoly(V2/* * spin(M_PI) */, shDragonEyes, 0xFF);
        
        int noscolor = (c->hitpoints == 1 && c->stuntime ==1) ? 0xFF0000FF : 0xFF;
        queuepoly(V2/* * spin(M_PI) */, shDragonNostril, noscolor);
        queuepoly(V2 * Mirror, shDragonNostril, noscolor);
        }
      else if(m == moTentacle || m == moTentaclewait || m == moTentacleEscaping)
        queuepoly(V2 * spin(M_PI), shTentHead, darkena(col, 0, 0xFF));
      else if(m == moDragonTail) {
        cell *c2 = NULL;
        for(int i=0; i<c->type; i++)
          if(c->mov[i] && isDragon(c->mov[i]->monst) && c->mov[i]->mondir == c->spn[i])
            c2 = c->mov[i];
        int nd = neighborId(c, c2);
        char part = dragon::bodypart(c, dragon::findhead(c));
        if(part == 't') {
          int hdir = displaydir(c, nd);
          V2 = V * spin(hdir * M_PI / 42 + M_PI);
          queuepoly(V2, shDragonTail, darkena(col, c->hitpoints?0:1, 0xFF));
          }
        else if(true) {
          int hdir0 = displaydir(c, nd) + 42;
          int hdir1 = displaydir(c, c->mondir);
          while(hdir1 > hdir0 + 42) hdir1 -= 84;
          while(hdir1 < hdir0 - 42) hdir1 += 84;
          V2 = V * spin((hdir0 + hdir1)/2 * M_PI / 42 + M_PI);
          if(part == 'l' || part == '2')
            queuepoly(V2, shDragonLegs, darkena(col, c->hitpoints?0:1, 0xFF));
          queuepoly(V2, shDragonWings, darkena(col, c->hitpoints?0:1, 0xFF));
          }
        }
      else
        queuepoly(V2, shJoint, darkena(col, 0, 0xFF));
      }

    return vid.monmode < 2;
    }
  
  else if(isMimic(c)) {
  
    int hdir = displaydir(c, c->mondir);

    transmatrix V2 = V * spin(M_PI*hdir/42);
    if(c->monst == moMirror) V2 = V2 * Mirror;
    
    if(flipplayer) V2 = V2 * spin(M_PI);

    if(vid.monmode > 1) {
      drawMonsterType(c->monst, c, V2, col);
      drawPlayerEffects(V2, c, false);
      }

    if(flipplayer) V2 = V2 * spin(M_PI);

    if(!outofmap(mouseh)) {
      // transmatrix invxy = Id; invxy[0][0] = invxy[1][1] = -1;
      
      hyperpoint P2 = V2 * inverse(cwtV) * mouseh;
      int xc, yc, sc;
      getcoord(P2, xc, yc, sc);
      queuechr(xc, yc, sc, 10, 'x', 0xFF00);
      }
    
    return vid.monmode < 2;
    }
  
  else if(c->monst && vid.monmode < 2) return true;
  
  // illusions face randomly
  
  else if(c->monst == moIllusion) {
    drawMonsterType(c->monst, c, V, col);
    drawPlayerEffects(V, c, false);
    }

  // wolves face the heat
  
  else if(c->monst == moWolf && c->cpdist > 1) {
    int d = 0;
    double bheat = -999;
    for(int i=0; i<c->type; i++) if(c->mov[i] && HEAT(c->mov[i]) > bheat) {
      bheat = HEAT(c->mov[i]);
      d = i;
      }
    int hdir = displaydir(c, d);
    transmatrix V2 = V * spin(hdir * M_PI / 42);
    return drawMonsterTypeDH(m, c, V2, col, darkhistory);
    }

  // golems, knights, and hyperbugs don't face the player (mondir-controlled)
  // also whatever in the lineview mode

  else if(isFriendly(c) || isBug(c) || (c->monst && conformal::on)) {
    int hdir = displaydir(c, c->mondir) + 42;
    transmatrix V2 = V * spin(hdir * M_PI / 42);
    if(!isBug(c)) drawPlayerEffects(V2, c, false);
    return drawMonsterTypeDH(m, c, V2, col, darkhistory);
    }

  else if(c->monst) {
    // other monsters face the player
    transmatrix VL;
    
    if(false) {
      // hyperpoint V0 = cwtV * C0;
      hyperpoint V1 = V * C0;
      VL = V * spin(hypot(V1[0], V1[1]));
      }
    else {
      hyperpoint V0 = inverse(cwtV) * V * C0;
      hyperpoint V1 = spintox(V0) * V0;

      VL = cwtV * rspintox(V0) * rpushxto0(V1) * spin(M_PI);
      }
    
    return drawMonsterTypeDH(m, c, VL, col, darkhistory);
    }
  
  return false;
  }

bool showPirateX;
cell *keycell, *pirateTreasureSeek, *pirateTreasureFound;
transmatrix pirateCoords;

double downspin;
cell *straightDownSeek;

int keycelldist;

void drawCircle(int x, int y, int size, int color) {
  #ifdef GL
  if(vid.usingGL) {
    qglcoords = 0;
    glcolor(color);
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
    
    glVertexPointer(3, GL_FLOAT, 0, glcoords);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_LINE_LOOP, 0, pts);
    return;
    }
  #endif

#ifdef MOBILE
  gdpush(4); gdpush(color); gdpush(x); gdpush(y); gdpush(size);
#else
#ifdef GFX
  (vid.usingAA?aacircleColor:circleColor) (s, x, y, size, (color << 8) | 0x80);
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
  if(!havebugs) return false;
  if(isBug(c)) return true;
  if(dist) for(int t=0; t<c->type; t++) if(c->mov[t] && bugsNearby(c->mov[t], dist-1)) return true;
  return false;
  }

int minecolors[8] = {
  0xFFFFFF, 0xF0, 0xF060, 0xF00000, 
  0x60, 0x600000, 0x00C0C0, 0
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

transmatrix movecell[7], curcell;

transmatrix applyPatterndir(cell *c, char patt = mapeditor::whichPattern) {
  int hdir = displaydir(c, mapeditor::patterndir(c, patt));
  transmatrix V = spin((42+hdir) * M_PI / 42);
  
  if(mapeditor::reflectPatternAt(c, patt)) 
    return V * Mirror;
  
  return V;
  }

transmatrix applyDowndir(cell *c, cellfunction *cf) {
  int hdir = displaydir(c, mapeditor::downdir(c, cf));
  return spin((42+hdir) * M_PI / 42);
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
    queuepoly(V * applyDowndir(c, cf), shTower[j], col);
  else if(c->wall != waLadder)
    queuepoly(V, shMFloor[c->type-6], col);
  }

void drawZebraFloor(const transmatrix& V, cell *c, int col) {

  if(euclid) { queuepoly(V, shTower[10], col); return; }

  int i = zebra40(c);
  i &= ~3;
  
  int j;

  if(purehepta) j = 4;
  else if(i >=4 && i < 16) j = 2;
  else if(i >= 16 && i < 28) j = 1;
  else if(i >= 28 && i < 40) j = 3;
  else j = 0;

  queuepoly(V * applyPatterndir(c, 'z'), shZebra[j], col);
  }

#define ECT (euclid?2:ct-6)
      
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
    queuepoly(V * applyPatterndir(c, 'f'), shEmeraldFloor[j], col);
  else
    queuepoly(V, shCaveFloor[euclid?2:c->type-6], col);
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

void drawcell(cell *c, const transmatrix& V, int spinv) {

#ifdef BUILDZEBRA
  if(c->type == 6 && c->tmp > 0) {
    int i = c->tmp;
    zebra(cellwalker(c, i&15), 1, i>>4, "", 0);
    }
  
  c->item = eItem(c->heat / 4);
  buildAutomatonRule(c);
#endif

  viewBuggyCells(c,V);
  
  // todo: fix when scrolling
  if(!buggyGeneration && c->land != laCanvas && sightrange < 10) {
    // not yet created
    if(c->mpdist > 7 && !cheater) return;
    // in the Yendor Challenge, scrolling back is forbidden
    if(c->cpdist > 7 && (yendor::on && !cheater)) return;
    // (incorrect comment) too far, no bugs nearby
    if(playermoved && c->cpdist > sightrange) return;
    }

  if(conformal::on || inHighQual) checkTide(c);
  
  if(!euclid) {
    // draw a web-like map
    if(webdisplay & 1) {
      if(c->type == 6) {
        for(int a=0; a<3; a++)
        queueline(V*Crad[a*7], V*Crad[a*7+21], 0xd0d0 >> darken);
        }
      else {
        for(int a=0; a<7; a++)
        queueline(V*C0, V*Crad[(21+a*6)%42], 0xd0d0 >> darken);
        }
      }
  
    if(webdisplay & 2) if(c->type == 7) {
      queueline(V*C0, V*xpush(tessf)*C0, 0xd0d0 >> darken);
      }
    
    if(webdisplay & 4) if(c->type == 7 && !euclid && c->master->alt) {
      for(int i=0; i<7; i++)
        if(c->master->move[i] && c->master->move[i]->alt == c->master->alt->move[0])
          queueline(V*C0, V*spin(-2*M_PI*i/7)*xpush(tessf)*C0, 0xd000d0 >> darken);
      }
    }
  
  // save the player's view center
  if(c == cwt.c && !shmup::on) {
    playerfound = true;

/*   if(euclid)
    return d * 84 / c->type;
  else
    return 42 - d * 84 / c->type;
    cwtV = V * spin(-cwt.spin * 2*M_PI/c->type) * spin(M_PI); */

    cwtV = V * spin(displaydir(c, cwt.spin) * M_PI/42);
    }
  
  /* if(cwt.c->land == laEdge) {   
    if(c == chosenDown(cwt.c, 1, 0)) 
      playerfoundL = c, cwtVL = V;
    if(c == chosenDown(cwt.c, -1, 0)) 
      playerfoundR = c, cwtVR = V;
    } */

  if(1) {
  
    hyperpoint VC0 = V*C0;
  
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

    int xc, yc, sc, xs, ys, ss;
    getcoord(VC0, xc, yc, sc);
    getcoord(V*xpush(.5)*C0, xs, ys, ss);
    // int col = 0xFFFFFF - 0x20 * c->maxdist - 0x2000 * c->cpdist;

    if(!buggyGeneration && c->mpdist > 8 && !cheater) return; // not yet generated
    
    if(c->land == laNone && cmode == emMapEditor) {
      queuepoly(V, shTriangle, 0xFF0000FF);
      }
  
    char ch = winf[c->wall].glyph;
    int col = winf[c->wall].color;

    if(c->land == laAlchemist && c->wall == waNone) col = 0x202020;
    if(c->land == laCrossroads && c->wall == waNone) col = (vid.goteyes2 ? 0xFF3030 : 0xFF0000);
    if(c->land == laCrossroads2 && c->wall == waNone) 
      col = linf[laCrossroads2].color;

    if(c->land == laCrossroads3 && c->wall == waNone) 
      col = linf[laCrossroads3].color;

    if(c->land == laCrossroads4 && c->wall == waNone) 
      col = linf[laCrossroads4].color;

    if(isElemental(c->land) && c->wall == waNone) 
      col = linf[c->land].color;
      
    if(c->land == laElementalWall && c->wall == waNone) 
      col = (linf[c->barleft].color>>1) + (linf[c->barright].color>>1);
      
    if(c->land == laZebra && c->wall == waNone) {
      col = 0xE0E0E0;
      }
      
    if(c->land == laZebra && c->wall == waTrapdoor)
      col = 0x808080;
      
    if(c->land == laDesert && c->wall == waNone) col = 0xEDC9AF;
    if(c->land == laCaves && c->wall == waNone) col = 0x202020;
    if(c->land == laEmerald && c->wall == waNone) col = 0x202020;
    if(c->land == laDeadCaves && c->wall == waNone) col = 0x202020;

    if(c->land == laNone && c->wall == waNone) {
      col = 0x101010;
      queuepoly(V, shTriangle, 0xFFFF0000);
      }

    if(isHive(c->land) && !isWateryOrBoat(c) && c->wall != waCloud && c->wall != waMirror && c->wall != waMineMine) {
      col = linf[c->land].color;
      if(c->wall == waWaxWall) col = c->landparam;
      }
    if(c->land == laJungle && c->wall == waNone) col = (vid.goteyes2 ? 0x408040 : 0x008000);
    if(c->land == laPower && c->wall == waNone)
      col = linf[c->land].color;
/*     if(c->land == laEmerald && c->wall == waNone) {
      col = 0x50A020;
      }
    if(c->land == laEmerald && c->wall == waLake) {
      col = 0x202080;
      int i = 0;
      for(int k=0; k<c->type; k++) if(c->mov[k] && c->mov[k]->wall != waLake)
        i++;
      if(i > 0) {
        col = gradient(col, 0xFFFFFF, 0, i-fabs(sin(ticks/1500.0)), 7);
        }
      } */
    if(c->land == laWineyard && c->wall == waNone) {
      col = 0x006000;
      }
    if(c->land == laTortoise && (c->wall == waNone || c->wall == waBigTree || c->wall == waSmallTree)) {
      
      if(c->wall == waBigTree) col = 0x709000;
      else if(c->wall == waSmallTree) col = 0x905000;
      else col = tortoise::getMatchColor(getBits(c));
      }

    if(c->land == laDryForest && c->wall == waNone) {
      /*if(c->wall == waBigTree)
        col = (vid.goteyes ? 0xC0C060 : 0xC0C000);
      else if(c->wall == waSmallTree)
        col = (vid.goteyes ? 0x60C060 : 0x00C000);
      else*/ if(c->wall == waNone) {
        col = gradient(0x008000, 0x800000, 0, c->landparam, 10);
        }
      }
    if(c->land == laMirror && c->wall == waNone) col = 0x808080;
    if(c->land == laMotion && c->wall == waNone) col = 0xF0F000;
    if(c->land == laGraveyard && c->wall == waNone) col = 0x107010;
    if(c->land == laCamelot && c->wall == waNone) {
      int d = showoff ? 0 : ((euclid||c->master->alt) ? celldistAltRelative(c) : 0);
      if(d < 0)
        col = 0xA0A0A0;
      else {
        // a nice floor pattern
        int v = emeraldval(c);
        int v0 = (v&~3);
        bool sw = (v&1);
        if(v0 == 8 || v0 == 12 || v0 == 20 || v0 == 40 || v0 == 36 || v0 == 24)
          sw = !sw;
        if(sw)
          col = 0xC0C0C0;
        else
          col = 0xA0A0A0;
        }
      }
    if(c->land == laRlyeh && c->wall == waNone) col = (vid.goteyes2 ? 0x4080C0 : 0x004080);
    if(c->land == laTemple) {
      int d = showoff ? 0 : (euclid||c->master->alt) ? celldistAlt(c) : 99;
      if(chaosmode)
        col = c->wall == waColumn ? winf[waColumn].color : 0x405090;
      else if(d % TEMPLE_EACH == 0)
        col = c->wall == waColumn ? winf[waColumn].color : 
          gradient(0x304080, winf[waColumn].color, 0, 0.5, 1);
//    else if(c->type == 7)
//      col = 0x707070;
      else if(d% 2 == -1)
        col = 0x304080;
      else
        col = 0x405090;
      }
    if(c->land == laHell && c->wall == waNone) col = (vid.goteyes2 ? 0xC03030 : 0xC00000);

    if(c->land == laPalace && (c->wall == waNone || c->wall == waClosePlate || c->wall == waOpenPlate ||
      c->wall == waTrapdoor) && (c->wall == waNone || vid.wallmode != 0)) 
      col = 0x806020;
    
    if(c->land == laPalace && c->wall == waCamelot)
      col = 0xFFD500;
    
    if(isIcyLand(c) && isIcyWall(c)) {
      float h = HEAT(c);
      bool showcoc = c->land == laCocytus && chaosmode && vid.wallmode < 3;
      if(h < -0.4)
        col = gradient(showcoc ? 0x4080FF : 0x4040FF, 0x0000FF, -0.4, h, -1);
      else if(h < 0)
        col = gradient(showcoc ? 0x80C0FF : 0x8080FF, showcoc ? 0x4080FF : 0x4040FF, 0, h, -0.4);
      else if(h < 0.2)
        col = gradient(showcoc ? 0x80C0FF : 0x8080FF, 0xFFFFFF, 0, h, 0.2);
      // else if(h < 0.4)
      //  col = gradient(0xFFFFFF, 0xFFFF00, 0.2, h, 0.4);
      else if(h < 0.6)
        col = gradient(0xFFFFFF, 0xFF0000, 0.2, h, 0.6);
      else if(h < 0.8)
        col = gradient(0xFF0000, 0xFFFF00, 0.6, h, 0.8);
      else
        col = 0xFFFF00;
      if(c->wall == waNone) 
        col = (col & 0xFEFEFE) >> 1;
      if(c->wall == waLake)
        col = (col & 0xFCFCFC) >> 2;
      }
    
    /* if(c->wall == waBonfireOff)
      col = 0x404040; */
      
    if(isFire(c))
      col = c->wall == waEternalFire ? weakfirecolor(1500) : firecolor(100);
      
    /* if(c->wall == waThumperOff)
      col = 0xEDC9AF; */
      
    if(c->wall == waThumperOn) {
      int ds = ticks;
      for(int u=0; u<5; u++) {
        ld rad = hexf * (.3 * u + (ds%1000) * .0003);
        int col = gradient(0xFFFFFF, 0, 0, rad, 1.5 * hexf);
        for(int a=0; a<84; a++)
          queueline(V*ddi(a, rad)*C0, V*ddi(a+1, rad)*C0, col);
        }
      }
    
    if(c->land == laEmerald && c->wall == waCavefloor) {
      col = gradient(col, 0xFF00, 0, 0.5, 1);
      // col |= 0xFF00; // col += 0x300060; // col += 0x2F18; col -= 0x100000;
      }

    if(c->land == laOcean && (c->wall == waNone || c->wall == waStrandedBoat)) {
      if(chaosmode)
        col = gradient(0xD0A090, 0xD0D020, 0, c->CHAOSPARAM, 30);
      else
        col = gradient(0xD0D090, 0xD0D020, -1, sin((double) c->landparam), 1);
      }

    if(c->land == laLivefjord && (c->wall == waNone || c->wall == waStrandedBoat))
      col = 0x306030;
      
    if(c->land == laEmerald && c->wall == waCavewall) {
      col = 0xC0FFC0;
      // col |= 0xFF00; // col += 0x300060; // col += 0x2F18; col -= 0x100000;
      }
      
    if(c->land == laHive && items[itOrbInvis] && c->wall == waNone && c->landparam)
      col = gradient(col, 0xFF0000, 0, c->landparam, 100);

    if(c->land == laStorms && (c->wall == waNone))
      col = linf[c->land].color;

    if(c->land == laWhirlwind && (c->wall == waNone)) {
      int wcol[4] = {0x404040, 0x404080, 0x2050A0, 0x5050C0};
      col = wcol[whirlwind::fzebra3(c)];
      }

    if(c->land == laOvergrown || c->land == laClearing) {
      if(c->wall == waSmallTree) col = 0x008060;
      else if(c->wall == waBigTree) col = 0x0080C0;
      else if(c->wall == waNone) 
        col = (c->land == laOvergrown/* || (celldistAlt(c)&1)*/) ? 0x00C020 : 0x60E080;
      }

    if(c->land == laGridCoast && c->wall == waSmallTree) col = 0x608000;
    
    if(isHaunted(c->land)) {
      if(c->wall == waSmallTree) col = 0x004000;
      else if(c->wall == waBigTree) col = 0x008000;
      else if(c->wall == waNone) {
        int itcolor = 0;
        for(int i=0; i<c->type; i++) if(c->mov[i] && c->mov[i]->item)
          itcolor = 1;
        if(c->item) itcolor |= 2;
        col = 0x609F60 + 0x202020 * itcolor;
        }
      }

    if(c->land == laWildWest && (c->wall == waNone))
      col = linf[c->land].color;

    if(c->land == laCaribbean && (c->wall == waCIsland || c->wall == waCIsland2))
      col = winf[c->wall].color;

    if(c->wall == waBoat && !vid.wallmode) {
      col = 0xC06000;
      }
    
    if(c->land == laMinefield && c->wall == waMineMine && (cmode == emMapEditor || !canmove))
      col = 0xFF4040;
      
    if(c->wall == waMineMine && c->land != laMinefield)
      col = gradient(col, 0xFF4040, -1, sin(ticks/100.0), 1);
      
    if(c->land == laMinefield && c->wall == waNone)
      col = 0x80A080;
      
    if(c->land == laCaribbean && c->wall == waNone)
      col = 0x006000;

    if(c->land == laRose && c->wall == waNone) {
      col = linf[c->land].color;
      }

    if(isWarped(c->land) && c->wall == waNone)
      col = pseudohept(c) ? 0x80C080 : 0xA06020;
      
    if(c->land == laRedRock && (c->wall == waNone || snakelevel(c)) && c->wall != waDeadfloor2) {
      col = linf[c->land].color;
      }
    
    if(c->land == laDragon && (c->wall == waNone || snakelevel(c)) && c->wall != waDeadfloor2) {
      col = linf[c->land].color;
      }
    
    if(c->land == laCanvas && c->wall == waNone) {
      col = c->landparam;
      }

    if(pseudohept(c)) {
      if(vid.darkhepta)
        col = gradient(0, col, 0, 0.75, 1);
      }
      
    int rd = rosedist(c);
    if(rd == 1) col = gradient(0x804060, col, 0,1,3);
    if(rd == 2) col = gradient(0x804060, col, 0,2,3);
    
    int ycol = col;

    if(c->land == laHive && c->bardir == NOBARRIERS && c->barleft) {
      col = minf[moBug0+c->barright].color;
      }
    
    if(items[itRevolver] && c->pathdist > GUNRANGE && !shmup::on)
      col = gradient(col, 0, 0, 25, 100);

    int xcol = col;
    
    eItem it = c->item;
    
    bool hidden = itemHidden(c);
    bool hiddens = itemHiddenFromSight(c);
    
    if(conformal::includeHistory && eq(c->aitmp, sval)) {
      hidden = true;
      hiddens = false;
      }
    
    if(hiddens && cmode != emMapEditor)
      it = itNone;

    if(it) 
      ch = iinf[it].glyph, col = iinf[it].color;
    
    int icol = col;
    
    if(it && c->land == laAlchemist)
      if(!(conformal::includeHistory && eq(c->aitmp, sval)))
        xcol = col;

    if(c->monst) {
      ch = minf[c->monst].glyph, col = minf[c->monst].color;
      if(c->monst == moMutant) {
        // root coloring
        if(c->stuntime != mutantphase)
          col = gradient(0xC00030, 0x008000, 0, (c->stuntime-mutantphase) & 15, 15);
        }
      if(isMetalBeast(c->monst) && c->stuntime) 
        col >>= 1;
      }
    
    if(c->cpdist == 0 && mapeditor::drawplayer) { 
      ch = '@'; 
      if(vid.monmode == 0) col = cheater ? 0xFF3030 : 0xD0D0D0; 
      }
    
    if(c->monst == moSlime) {
      col = winf[c->wall].color;
      col |= (col>>1);
      }
    
    if(c->ligon) {
      int tim = ticks - lightat;
      if(tim > 1000) tim = 800;
      if(elec::havecharge && tim > 400) tim = 400;
      for(int t=0; t<7; t++) if(c->mov[t] && c->mov[t]->ligon) {
        int hdir = displaydir(c, t);
        int col = gradient(iinf[itOrbLightning].color, 0, 0, tim, 1100);
        queueline(V*ddi(ticks, hexf/2)*C0, V*ddi(hdir, crossf)*C0, col);
        }
      }
    
    int ct = c->type;

    bool error = false;
    
    if(c->land == laIvoryTower && (c->wall == waNone || c->wall == waLadder))
      // xcol = (c->landparam&1) ? 0xD00000 : 0x00D000;
      xcol = 0x10101 * (32 + (c->landparam&1) * 32) - 0x000010;
    
    if(c->land == laEndorian && c->wall == waNone) {
      int clev = cwt.c->land == laEndorian ? edgeDepth(cwt.c) : 0;
      // xcol = (c->landparam&1) ? 0xD00000 : 0x00D000;
      xcol = 0x10101 * (32 + (c->landparam&1) * 32) - 0x000010;
      xcol = gradient(xcol, 0x0000D0, clev-10, edgeDepth(c), clev+10);
      }
    
    if(c->wall == waTrunk) xcol = winf[waTrunk].color;
    
    if(c->wall == waCanopy || c->wall == waSolidBranch || c->wall == waWeakBranch) {
      xcol = winf[waCanopy].color;
      if(c->landparam & 1) xcol = gradient(0, xcol, 0, .75, 1);
      }
    
    if(c->wall == waSea || c->wall == waBoat) {
      if(c->land == laOcean)
        xcol = (c->landparam > 25 && !chaosmode) ? 0x000090 : 
          0x1010C0 + int(32 * sin(ticks / 500. + (chaosmode ? c->CHAOSPARAM : c->landparam)*1.5));
      else if(c->land == laOceanWall)
        xcol = 0x2020FF;
      else if(c->land == laAlchemist)
        xcol = 0x900090;
      else if(c->land == laWhirlpool)
        xcol = 0x0000C0 + int(32 * sin(ticks / 200. + ((euclid||c->master->alt) ? celldistAlt(c) : 0)*1.5));
      else if(c->land == laLivefjord)
        xcol = 0x000080;
      else if(isWarped(c->land))
        xcol = 0x0000C0 + int((pseudohept(c)?30:-30) * sin(ticks / 600.));
      }
    
    if(vid.wallmode) {
    
      poly_outline = OUTLINE_NONE;
    
      // floor
      
      int fd = 
        c->land == laRedRock ? 0 : 
        (c->land == laOcean || c->land == laLivefjord || c->land == laWhirlpool) ? 1 :
        c->land == laAlchemist || c->land == laIce || c->land == laGraveyard ||
        c->land == laRlyeh || c->land == laTemple || c->land == laWineyard ||
        c->land == laDeadCaves || c->land == laPalace ? 1 : 
        c->land == laCanvas ? 0 :
        c->land == laIvoryTower ? 1 :
        c->land == laEndorian ? 1 :
        c->land == laCaribbean ? 1 :
        c->land == laWhirlwind ? 1 :
        c->land == laRose ? 1 :
        c->land == laGridSea ? 1 :
        c->land == laTortoise ? 1 :
        c->land == laDragon ? 1 :
        2;

#ifndef MOBILE
      transmatrix Vpdir = V * applyPatterndir(c);
#endif
        
      // printf("ql\n"); 
      // queueline(Vpdir * C0, Vpdir * xpush(crossf/3) * C0, 0xFFFFFF);

      bool eoh = euclid || purehepta;

#ifndef MOBILE
      if(c == mapeditor::drawcell && c != cwt.c && !c->monst && !c->item) {
        mapeditor::drawtrans = Vpdir;
        }
#endif

      if(c->wall == waChasm) ;

      /* else if(purehepta)
        queuepoly(V * spin(M_PI), shBigHepta, darkena(xcol, fd, 0xFF)); */


#ifndef MOBILE
      else if(drawUserShape(Vpdir, mapeditor::cellShapeGroup(), mapeditor::realpattern(c),
        darkena(xcol, fd, cmode == emDraw ? 0xC0 : 0xFF)));
      
      else if(mapeditor::whichShape == '7') {
        if(ishept(c))
          queuepoly(V, vid.wallmode == 1 ? shBFloor[ct-6] : 
            euclid ? shBigHex :
            shBigHepta, darkena(xcol, fd, 0xFF));
        }
      
      else if(mapeditor::whichShape == '8') {
        if(euclid) 
          queuepoly(V, shTriheptaEuc[ishept(c) ? 1 : ishex1(c) ? 0 : 2], darkena(xcol, fd, 0xFF));
        else
          queuepoly(V, shTriheptaFloor[ishept(c) ? 1 : 0], darkena(xcol, fd, 0xFF));
        }
      
      else if(mapeditor::whichShape == '6') {
        if(!ishept(c))
          queuepoly(V, 
            vid.wallmode == 1 ? shBFloor[ct-6] : 
            euclid ? (ishex1(c) ? shBigHexTriangle : shBigHexTriangleRev) :
            shBigTriangle, darkena(xcol, fd, 0xFF));
        }
#endif
      
      else if(c->land == laWineyard && (c->wall == waVineHalfA || c-> wall == waVineHalfB)) {

        int i =-1;
        for(int t=0;t<6; t++) if(c->mov[t] && c->mov[t]->wall == c->wall)
          i = t;

        int hdir = 14 + displaydir(c, i);

        transmatrix V2 = V * spin(M_PI*hdir/42);
        
        hpcshape *shar = shSemiFeatherFloor;
        
        if(vid.wallmode == 1) shar = shSemiBFloor;
        if(vid.wallmode == 2) shar = shSemiFloor;
        
        int dk = vid.wallmode == 1 ? 0 : vid.wallmode == 2 ? 1 : 1;
        
        queuepoly(V2, shar[0], darkena(winf[waVinePlant].color, dk, 0xFF));
        queuepoly(V2, shar[1], darkena(xcol, dk, 0xFF));
        }

      else if(vid.wallmode == 1 && c->land == laAlchemist)
        queuepoly(V, shFloor[ct-6], darkena(xcol, 1, 0xFF));
      
      else if(vid.wallmode == 1 && c->wall == waMineOpen) 
        ;
      
      else if(vid.wallmode == 1) 
        queuepoly(V, shBFloor[ct-6], darkena(xcol, 0, 0xFF));
      
      else if(isWarped(c) && euclid)
        queuepoly(V, shTriheptaEuc[ishept(c)?1:ishex1(c)?0:2], darkena(xcol, fd, 0xFF));

      else if(isWarped(c) && !purehepta && !shmup::on) {
        transmatrix V2 = V * applyPatterndir(c);
        int np = mapeditor::nopattern(c);
        if(c->landparam == 1337) np = 0; // for the achievement screenshot
        if(np < 11)
          queuepoly(V2, shTriheptaFloor[np], darkena(xcol, fd, 0xFF));
        }

      else if(vid.wallmode == 2) {
        queuepoly(V, shFloor[ct-6], darkena(xcol, fd, 0xFF));
        }

      else if(randomPatternsMode && c->land != laBarrier && !isWarped(c->land)) {
        int j = (randompattern[c->land]/5) % 15;
        int col = darkena(xcol, fd, 0xFF);
        int k = randompattern[c->land] % RPV_MODULO;
        int k7 = randompattern[c->land] % 7;
        
        if(k == RPV_ZEBRA && k7 < 2) drawZebraFloor(V, c, col);
        else if(k == RPV_EMERALD && k7 == 0) drawEmeraldFloor(V, c, col);
        else if(k == RPV_CYCLE && k7 < 4) drawTowerFloor(V, c, col, celldist);

        else switch(j) {
          case 0:  queuepoly(V, shCloudFloor[ct-6], col); break;
          case 1:  queuepoly(V, shFeatherFloor[ECT], col); break;
          case 2:  queuepoly(V, shStarFloor[ct-6], col); break;
          case 3:  queuepoly(V, shTriFloor[ct-6], col); break;
          case 4:  queuepoly(V, shSStarFloor[ct-6], col); break;
          case 5:  queuepoly(V, shOverFloor[ECT], col); break;
          case 6:  queuepoly(V, shFeatherFloor[ECT], col); break;
          case 7:  queuepoly(V, shDemonFloor[ct-6], col); break;
          case 8:  queuepoly(V, shCrossFloor[ct-6], col); break;
          case 9:  queuepoly(V, shMFloor[ct-6], col); break;
          case 10: queuepoly(V, shCaveFloor[ECT], col); break;
          case 11: queuepoly(V, shPowerFloor[ct-6], col); break;
          case 12: queuepoly(V, shDesertFloor[ct-6], col); break;
          case 13: queuepoly(V, purehepta ? shChargedFloor[3] : shChargedFloor[ct-6], col); break;
          case 14: queuepoly(V, ct==6?shChargedFloor[2]:shFloor[1], col); break;
          }
        }
      
      else if(c->land == laWineyard) {
        queuepoly(V, shFeatherFloor[euclid?2:ct-6], darkena(xcol, fd, 0xFF));
        }

      else if(c->land == laZebra) 
        drawZebraFloor(V, c, darkena(xcol, fd, 0xFF));
      
      else if(c->wall == waTrunk) 
        queuepoly(V, shFloor[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->wall == waCanopy || c->wall == waSolidBranch || c->wall == waWeakBranch) 
        queuepoly(V, shFeatherFloor[ct-6], darkena(xcol, fd, 0xFF));

      else if(isGravityLand(c->land)) 
        drawTowerFloor(V, c, darkena(xcol, fd, 0xFF));

      else if(c->land == laEmerald) 
        drawEmeraldFloor(V, c, darkena(xcol, fd, 0xFF));

      else if(c->land == laRlyeh)
        queuepoly(V, (eoh ? shFloor: shTriFloor)[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laTemple)
        queuepoly(V, (eoh ? shFloor: shTriFloor)[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laAlchemist)
        queuepoly(V, shCloudFloor[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laRose)
        queuepoly(V, shRoseFloor[purehepta ? 2 : ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laTortoise)
        queuepoly(V, shTurtleFloor[purehepta ? 2 : ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laDragon && !purehepta)
        queuepoly(V, shDragonFloor[euclid?2:ct-6], darkena(xcol, fd, 0xFF));

      else if((isElemental(c->land) || c->land == laElementalWall) && !eoh)
        queuepoly(V, shNewFloor[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laJungle)
        queuepoly(V, shFeatherFloor[euclid?2:ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laGraveyard)
        queuepoly(V, (eoh ? shFloor : shCrossFloor)[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laDeadCaves) {
        queuepoly(V, shCaveFloor[euclid?2:ct-6], darkena(xcol, fd, 0xFF));
        }

      else if(c->land == laMotion)
        queuepoly(V, shMFloor[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laWhirlwind)
//      drawZebraFloor(V, c, darkena(xcol, fd, 0xFF));
        queuepoly(V, (eoh ? shCloudFloor : shNewFloor)[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laHell)
        queuepoly(V, (euclid ? shStarFloor : shDemonFloor)[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laIce)
//      queuepoly(V, shFloor[ct-6], darkena(xcol, 2, 0xFF));
        queuepoly(V, shStarFloor[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laCocytus)
        queuepoly(V, (eoh ? shCloudFloor : shDesertFloor)[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laStorms) {
        if(euclid) 
          queuepoly(ishex1(c) ? V*spin(M_PI) : V, 
            ishept(c) ? shFloor[0] : shChargedFloor[2], darkena(xcol, fd, 0xFF));
        else 
          queuepoly(V, (purehepta ? shChargedFloor[3] : ct==6 ? shChargedFloor[2] : shFloor[1]), darkena(xcol, fd, 0xFF));
        }

      else if(c->land == laWildWest)
        queuepoly(V, (eoh ? shCloudFloor : shSStarFloor)[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laPower)
        queuepoly(V, (eoh ? shStarFloor : shPowerFloor)[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laHive && !isWateryOrBoat(c) && c->wall != waFloorB && c->wall != waFloorA &&
        c->wall != waMirror && c->wall != waCloud) {
        queuepoly(V, shFloor[ct-6], darkena(xcol, 1, 0xFF));
        if(!snakelevel(c) && c->wall != waMirror && c->wall != waCloud)
          queuepoly(V, shMFloor[ct-6], darkena(xcol, 2, 0xFF));
        if(c->wall != waWaxWall && c->wall != waDeadTroll && c->wall != waDeadTroll2 && c->wall != waVinePlant &&
          !snakelevel(c) && c->wall != waMirror && c->wall != waCloud &&
          c->wall != waStrandedBoat) 
          queuepoly(V, shMFloor2[ct-6], darkena(xcol, xcol==ycol ? 1 : 2, 0xFF));
        }

      else if(c->land == laCaves)
        queuepoly(V, shCaveFloor[ECT], darkena(xcol, fd, 0xFF));

      else if(c->land == laDesert)
        queuepoly(V, (eoh ? shCloudFloor : shDesertFloor)[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laOvergrown || c->land == laClearing || isHaunted(c->land))
        queuepoly(V, shOverFloor[ECT], darkena(xcol, fd, 0xFF));

      else if(c->land == laRose)
        queuepoly(V, shOverFloor[ECT], darkena(xcol, fd, 0xFF));

      else if(c->land == laDryForest)
        queuepoly(V, (eoh ? shStarFloor : shDesertFloor)[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laCaribbean || c->land == laOcean || c->land == laOceanWall || c->land == laWhirlpool)
        queuepoly(V, shCloudFloor[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laLivefjord)
        queuepoly(V, shCaveFloor[ECT], darkena(xcol, fd, 0xFF));

      else if(c->land == laRedRock)
        queuepoly(V, eoh ? shFloor[ct-6] : shDesertFloor[ct-6], darkena(xcol, fd, 0xFF));

      else if(c->land == laPalace)
        queuepoly(V, (eoh?shFloor:shPalaceFloor)[ct-6], darkena(xcol, fd, 0xFF));

      else {
        queuepoly(V, shFloor[ct-6], darkena(xcol, fd, 0xFF));
        }
      // walls

#ifndef MOBILE
      if(cmode == emMapEditor && mapeditor::displaycodes) {

        int labeli = mapeditor::displaycodes == 1 ? mapeditor::realpattern(c) : mapeditor::subpattern(c);
        
        string label = its(labeli);
        int siz = int(sqrt(squar(xc-xs)+squar(yc-ys))) / 5;
        queuestr(xc, yc, sc, siz, label, 0xFFFFFFFF);
        
        /* transmatrix V2 = V * applyPatterndir(c);
        queuepoly(V2, shNecro, 0x80808080);
        queuepoly(V2, shStatue, 0x80808080); */
        }
#endif

      if(realred(c->wall)) {
        int s = snakelevel(c);
        if(s >= 1)
          queuepoly(V, shRedRockFloor[0][ct-6], darkena(winf[waRed1].color, 0, 0xFF));
        if(s >= 2)
          queuepoly(V, shRedRockFloor[1][ct-6], darkena(winf[waRed2].color, 0, 0xFF));
        if(s >= 3)
          queuepoly(V, shRedRockFloor[2][ct-6], darkena(winf[waRed3].color, 0, 0xFF));
        }
      
      if(pseudohept(c) && (c->land == laRedRock || (purehepta && (c->land == laClearing || isWarped(c))))) {
        queuepoly(V, shHeptaMarker, 0x00000080);
        }

      if(conformal::includeHistory && eq(c->aitmp, sval-1))
        queuepoly(V, shHeptaMarker, 0x000000C0);

      /* if(c->land == laBarrier || c->land == laCrossroads2) {
        int siz = int(sqrt(squar(xc-xs)+squar(yc-ys))) / 5;
        displaystr(xc, yc, sc, siz, its(int(c->heat + .5)), 0xFFFFFFFF, 8);
        } */
      
      char xch = winf[c->wall].glyph;

      if(c->wall == waSolidBranch) {
        queuepoly(V, shSolidBranch, 0x804000FF);
        }

      else if(c->wall == waWeakBranch) {
        queuepoly(V, shWeakBranch, 0x804000FF);
        }

      else if(c->wall == waLadder) {
        if(euclid) {
          queuepoly(V, shMFloor[ct-6], 0x804000FF);
          queuepoly(V, shMFloor2[ct-6], 0x000000FF);
          }
        else {
          queuepoly(V, shFloor[ct-6], 0x804000FF);
          queuepoly(V, shMFloor[ct-6], 0x000000FF);
          }
        }

      if(c->wall == waBoat || c->wall == waStrandedBoat) {    
        int hdir = displaydir(c, c->mondir);
        transmatrix V2 = V * spin((42+hdir) * M_PI / 42);
        if(items[itOrbWater] && (c == cwt.c || (isFriendly(c) && items[itOrbEmpathy]))) {
          queuepoly(V2, shBoatOuter, watercolor(0));
          queuepoly(V2, shBoatInner, 0x0060C0FF);
          }
        else {
          queuepoly(V2, shBoatOuter, 0xC06000FF);
          queuepoly(V2, shBoatInner, 0x804000FF);
          }
        }

      else if(c->wall == waBigStatue)
        queuepoly(V, shStatue, 
          darkena(winf[c->wall].color, 0, 0xFF)
          );
      
      else if(c->wall == waSulphurC) {
        bool drawStar = true;
        for(int t=0; t<c->type; t++) 
          if(c->mov[t] && c->mov[t]->wall != waSulphur && c->mov[t]->wall != waSulphurC &&
           c->mov[t]->wall != waBarrier)
            drawStar = false;
        if(drawStar) queuepoly(V, shGiantStar[ct-6], darkena(xcol, 0, 0xFF));
        }

      else if(c->wall == waClosePlate || c->wall == waOpenPlate || (c->wall == waTrapdoor && c->land != laZebra)) {
        transmatrix V2 = V;
        if(ct == 7 && vid.wallmode == 3) V2 = V * spin(M_PI);
        queuepoly(V2, shMFloor[ct-6], darkena(winf[c->wall].color, 0, 0xFF));
        queuepoly(V2, shMFloor2[ct-6], vid.wallmode >= 2 ? darkena(xcol, 1, 0xFF) : darkena(0,1,0xFF));
        }

      else if(c->wall == waFrozenLake || c->wall == waLake || c->wall == waCamelotMoat ||
        c->wall == waRoundTable || c->wall == waSea || c->wall == waClosePlate || c->wall == waOpenPlate ||
        c->wall == waOpenGate || c->wall == waTrapdoor)
        ;
      
      else if(c->wall == waRose) {
        xcol <<= 1;
        if(c->cpdist > 5)
          xcol = 0xC0C0C0;
        else if(rosephase == 7)
          xcol = 0xFF0000;
        else 
          xcol = gradient(xcol, 0xC00000, 0, rosephase, 6);
        queuepoly(V, shThorns, 0xC080C0FF);

        for(int u=0; u<4; u+=2)
          queuepoly(V * spin(2*M_PI / 3 / 4 * u), shRose, darkena(xcol, 0, 0xC0));      
        }

      else if(xch == '#') {
        if(c->wall == waVinePlant)
          xcol = 0x60C000;
        if(c->wall != waPlatform && c->wall != waWarpGate)
          queuepoly(V, shWall[ct-6], darkena(xcol, 0, 0xFF));
        }
      
      else if(c->wall == waFan) {
        queuepoly(V * spin(M_PI/6 - fanframe * M_PI / 3), shFan, darkena(xcol, 0, 0xFF));
        }
      
      else if(xch == '%') {
        if(doHighlight())
          poly_outline = (c->land == laMirror) ? OUTLINE_TREASURE : OUTLINE_ORB;
        queuepoly(V, shMirror, darkena(xcol, 0, 0xC0));
        poly_outline = OUTLINE_NONE;
        }
      
      else if(isFire(c) || isThumper(c) || c->wall == waBonfireOff) {
        ld sp = 0;
        if(hasTimeout(c)) sp = ticks / (c->land == laPower ? 5000. : 500.);
        queuepoly(V * spin(sp), shStar, darkena(col, 0, 0xF0));
        }
      
      else if(xch == '+' && (c->land == laGraveyard || isHaunted(c->land)) && c->wall != waFloorB && c->wall != waFloorA &&
        c->wall != waFloorC && c->wall != waFloorD)
        queuepoly(V, shCross, darkena(xcol, 0, 0xFF));

      else if(xch == '+' && c->wall == waClosedGate) {
        int hdir = 0;
        for(int i=0; i<c->type; i++) if(c->mov[i]->wall == waClosedGate)
          hdir = i;
        hdir = displaydir(c, hdir);
        transmatrix V2 = V * spin((42+hdir) * M_PI / 42);
        queuepoly(V2, shPalaceGate, darkena(xcol, 0, 0xFF));
        }

      else if(xch == '+' && c->wall == waGiantRug) {
        queuepoly(V, shBigCarpet1, darkena(0xC09F00, 0, 0xFF));
        queuepoly(V, shBigCarpet2, darkena(0x600000, 0, 0xFF));
        queuepoly(V, shBigCarpet3, darkena(0xC09F00, 0, 0xFF));
        }

      else if(xch != '.' && xch != '+' && xch != '>' && xch != ':' && xch != ';' && c->wall != waSulphur && xch != ',')
        error = true;

/*    if(c->master->alt) {
        int d = celldistAlt(c);
        int siz = int(sqrt(squar(xc-xs)+squar(yc-ys))) / 5;
        if(d != ALTDIST_UNKNOWN && d != ALTDIST_BOUNDARY)
          displaystr(xc, yc, sc, siz, its(d), 0xFFFFFFFF, 8);
        } */
      }
    else if(!(it || c->monst || c->cpdist == 0)) error = true;

    if(c->wall == waMineOpen) {
      int mines = countMinesAround(c);
      
      if(vid.wallmode == 0) {
        if(ch == '.') {
          if(mines == 0) ch = ' ';
          else ch = '0' + mines, col = minecolors[mines];
          }
        else if(ch == '@') col = minecolors[mines];
        }
      else if(mines > 0)
        queuepoly(V, shMineMark[c->type-6], (minecolors[mines] << 8) | 0xFF);
      }
    
    // treasure
    
    char xch = iinf[it].glyph;
    hpcshape *xsh = 
      it == itPirate ? &shPirateX :
      (it == itBuggy || it == itBuggy2) ? &shPirateX :
      it == itHolyGrail ? &shGrail :
      isElementalShard(it) ? &shElementalShard :
      xch == '*' ? &shGem[ct-6] : xch == '%' ? &shDaisy : xch == '$' ? &shStar : xch == ';' ? &shTriangle :
      xch == '!' ? &shTriangle : it == itBone ? &shNecro : it == itStatue ? &shStatue :
      it == itEdge ? &shFigurine : 
      xch == '?' ? &shBookCover : 
      it == itKey ? &shKey : 
      it == itRevolver ? &shGun :
      NULL;
    
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
      
#ifndef MOBILE
    if(c == mapeditor::drawcell && mapeditor::drawcellShapeGroup() == 2)
      mapeditor::drawtrans = V;
#endif
      
    if(vid.monmode == 0 && it)
      error = true;
    
    else if(it == itBabyTortoise) {
      int bits = tortoise::babymap[c];
      tortoise::draw(V * spin(ticks / 5000.) * ypush(crossf*.15), bits, 2, 0);
      // queuepoly(V, shHeptaMarker, darkena(tortoise::getMatchColor(bits), 0, 0xC0));
      }
    
    else if(it == itCompass) {
      transmatrix V2 = V;       
      if(euclid) V2 = V2 * spin(M_PI/2);
      else V2 = V2 * rspintox(inverse(V) * pirateCoords * C0);
      V2 = V2 * spin(M_PI * sin(ticks/100.) / 30);
      queuepoly(V2, shCompass1, 0xFF8080FF);
      queuepoly(V2, shCompass2, 0xFFFFFFFF);
      queuepoly(V2, shCompass3, 0xFF0000FF);
      queuepoly(V2 * spin(M_PI), shCompass3, 0x000000FF);
      xsh = NULL;
      }

    else if(it == itPalace) {
      transmatrix V2 = V * spin(ticks / 1500.);
      queuepoly(V2, shMFloor3[ct-6], 0xFFD500FF);
      queuepoly(V2, shMFloor4[ct-6], darkena(icol, 0, 0xFF));
      queuepoly(V2, shGem[ct-6], 0xFFD500FF);
      xsh = NULL;
      }
    
    else if(drawUserShape(V, 2, it, darkena(icol, 0, 0xFF))) ;
    
    else if(it == itRose) {
      for(int u=0; u<4; u++)
        queuepoly(V * spin(ticks / 1500.) * spin(2*M_PI / 3 / 4 * u), shRose, darkena(icol, 0, hidden ? 0x30 : 0xA0));
      }

    else if(xsh) {
      if(it == itFireShard) icol = firecolor(100);
      if(it == itWaterShard) icol = watercolor(100) >> 8;
      
      if(it == itZebra) icol = 0x202020;
      if(it == itLotus) icol = 0x101010;
    
      queuepoly(V * spin(ticks / 1500.), *xsh, darkena(icol, 0, hidden ? 0x40 : 0xF0));

      if(xsh == &shBookCover && vid.monmode)
        queuepoly(V * spin(ticks / 1500.), shBook, 0x805020FF);
      if(it == itZebra)
        queuepoly(V * spin(ticks / 1500. + M_PI/c->type), *xsh, darkena(0xFFFFFF, 0, hidden ? 0x40 : 0xF0));
      }
    
    else if(xch == 'o') {
      if(it == itOrbFire) icol = firecolor(100);
      queuepoly(V, shDisk, darkena(icol, 0, hidden ? 0x20 : 0xC0));
      if(it == itOrbFire) icol = firecolor(200);
      if(it == itOrbFriend || it == itOrbDiscord) icol = 0xC0C0C0;
      if(it == itOrbFrog) icol = 0xFF0000;
      if(it == itOrbFreedom) icol = 0xC0FF00;
      if(it == itOrbAir) icol = 0xFFFFFF;
      if(it == itOrbUndeath) icol = minf[moFriendlyGhost].color;
      hpcshape& sh = 
        isRangedOrb(it) ? shTargetRing :
        isOffensiveOrb(it) ? shSawRing :
        isFriendOrb(it) ? shPeaceRing :
        isUtilityOrb(it) ? shGearRing :
        it == itOrb37 ? shHeptaRing :
        shRing;
      queuepoly(V * spin(ticks / 1500.), sh, darkena(icol, 0, int(0x80 + 0x70 * sin(ticks / 300.))));
      }

    else if(it) error = true;
    
    
    
    // monsters
    
    if(flashat > 0 && c == flashcell) {
      int tim = ticks - flashat;
      if(tim > 1000) flashat = 0;
      for(int u=0; u<=tim; u++) {
        if((u-tim)%50) continue;
        if(u < tim-150) continue;
        ld rad = u * 3 / 1000.;
        rad = rad * (5-rad) / 2;
        rad *= hexf;
        int col = iinf[itOrbFlash].color;
        if(u > 500) col = gradient(col, 0, 500, u, 1100);
        for(int a=0; a<84; a++)
          queueline(V*ddi(a, rad)*C0, V*ddi(a+1, rad)*C0, col);
        }
      }

    if(bigflashat > 0 && c == flashcell) {
      int tim = ticks - bigflashat;
      if(tim > 2000) bigflashat = 0;
      for(int u=0; u<=tim; u++) {
        if((u-tim)%50) continue;
        if(u < tim-250) continue;
        ld rad = u * 3 / 2000.;
        rad = rad * (5-rad) * 1.25;
        rad *= hexf;
        int col = 0xC0FF00;
        if(u > 1000) col = gradient(col, 0, 1000, u, 2200);
        for(int a=0; a<84; a++)
          queueline(V*ddi(a, rad)*C0, V*ddi(a+1, rad)*C0, col);
        }
      }
    
    error |= drawMonster(V, ct, c, col);    
    
    int ad = airdist(c);
    if(ad == 1 || ad == 2) {

     for(int i=0; i<c->type; i++) {
       cell *c2 = c->mov[i]; 
       if(airdist(c2) < airdist(c)) {
         calcAirdir(c2); // printf("airdir = %d\n", airdir);
         int hdir = displaydir(c, i);
         transmatrix V0 = spin((42+hdir) * M_PI / 42);
         
         double ph = ticks / 75.0 + airdir * M_PI / 21.;
         
         int aircol = 0x8080FF00 | int(32 + 32 * -cos(ph));
         
         double ph0 = ph/2;
         ph0 -= floor(ph0/M_PI)*M_PI;

         poly_outline = 0;
         queuepoly(V*V0*ddi(0, hexf*-cos(ph0)), shDisk, aircol);
         poly_outline = OUTLINE_NONE;
         }
       }

//    queuepoly(V*ddi(rand() % 84, hexf*(rand()%100)/100), shDisk, aircolor(airdir));
      }

    /* int rd = rosedist(c);
    if(rd > 0 && ((rd&7) == (turncount&7))) {

     for(int i=0; i<c->type; i++) {
       cell *c2 = c->mov[i]; 
       if(rosedist(c2) == rosedist(c)-1) {
         int hdir = displaydir(c, i);
         transmatrix V0 = spin((42+hdir) * M_PI / 42);
         
         double ph = ticks / 75.0; // + airdir * M_PI / 21.;
         
         int rosecol = 0x764e7c00 | int(32 + 32 * -cos(ph));
         
         double ph0 = ph/2;
         ph0 -= floor(ph0/M_PI)*M_PI;

         poly_outline = 0;
         queuepoly(V*V0*ddi(0, hexf*-cos(ph0)), shDisk, rosecol);
         poly_outline = 0xFF;
         }
       }
      } */

    if(c->land == laWhirlwind) {
      whirlwind::calcdirs(c);
      
      for(int i=0; i<whirlwind::qdirs; i++) {
       int hdir0 = displaydir(c, whirlwind::dfrom[i]) + 42;
       int hdir1 = displaydir(c, whirlwind::dto[i]);

       double ph1 = fanframe;
       
       int aircol = 0xC0C0FF40;
       
       ph1 -= floor(ph1);
       
       if(hdir1 < hdir0-42) hdir1 += 84;
       if(hdir1 >= hdir0+42) hdir1 -= 84;
       
       int hdir = (hdir1*ph1+hdir0*(1-ph1));

       transmatrix V0 = spin((hdir) * M_PI / 42);
       
       double ldist = purehepta ? crossf : c->type == 6 ? .2840 : 0.3399;

       poly_outline = 0;
       queuepoly(V*V0*ddi(0, ldist*(2*ph1-1)), shDisk, aircol);
       poly_outline = OUTLINE_NONE;
       }

//    queuepoly(V*ddi(rand() % 84, hexf*(rand()%100)/100), shDisk, aircolor(airdir));
      }

/*    if(ch == '.') {
      col = darkened(col);
      for(int t=0; t<ct; t++)
        queueline(V*ddi(t*84/ct, hexf/3)*C0, V*ddi((t+1)*84/ct, hexf/3)*C0, col);
      }
      
    else if(ch == '#') {
      col = darkened(col);
      for(int u=1; u<6; u++)
      for(int t=0; t<ct; t++)
        queueline(V*ddi(0 + t*84/ct, u*hexf/6)*C0, V*ddi(0 + (t+1)*84/ct, u*hexf/6)*C0, col);
      }
      
    else */
    
    if(error) {
      int siz = int(sqrt(squar(xc-xs)+squar(yc-ys)));
      
      if(c->wall == waSea) col = xcol;

      queuechr(xc, yc, sc, siz, ch, col, 2);
      }

    if(c == dragon::target && getMount()) {
      queuechr(xc, yc, sc, 2*vid.fsize, 'X', 
        gradient(0, iinf[itOrbDomination].color, -1, sin(ticks/(dragon::whichturn == turncount ? 75. : 150.)), 1));
      }
    
    if(c == keycell) {
      queuechr(xc, yc, sc, 2*vid.fsize, 'X', 0x10101 * int(128 + 100 * sin(ticks / 150.)));
      queuestr(xc, yc, sc, vid.fsize, its(keycelldist), 0x10101 * int(128 - 100 * sin(ticks / 150.)));
      }
    
    if(c == pirateTreasureFound) { 
      pirateCoords = V;
      if(showPirateX) {
        queuechr(xc, yc, sc, 2*vid.fsize, 'X', 0x10100 * int(128 + 100 * sin(ticks / 150.)));
        if(cwt.c->master->alt)
          queuestr(xc, yc, sc, vid.fsize, its(-celldistAlt(cwt.c)), 0x10101 * int(128 - 100 * sin(ticks / 150.)));
        }
      }
    
    if(!euclid && (!pirateTreasureSeek || compassDist(c) < compassDist(pirateTreasureSeek)))
      pirateTreasureSeek = c;

    if(!euclid) {
      bool usethis = false;
      double spd = 1;
      
      if(isGravityLand(cwt.c->land)) {
        if(!straightDownSeek || edgeDepth(c) < edgeDepth(straightDownSeek)) {
          usethis = true;
          spd = cwt.c->landparam / 10.;
          }
        }

      if(pmodel) {
        if(c->master->alt && cwt.c->master->alt &&
          (cwt.c->land == laTemple || cwt.c->land == laWhirlpool || 
          (cheater && (cwt.c->land == laClearing || cwt.c->land == laCaribbean ||
            cwt.c->land == laCamelot || cwt.c->land == laPalace))) 
          && c->land == cwt.c->land && c->master->alt->alt == cwt.c->master->alt->alt) {
          if(!straightDownSeek || celldistAlt(c) < celldistAlt(straightDownSeek)) {
            usethis = true;
            spd = .5;
            }
          }
  
        if(cwt.c->land == laOcean && cwt.c->landparam < 25) {
          if(!straightDownSeek || coastval(c, laOcean) < coastval(straightDownSeek, laOcean)) {
            usethis = true;
            spd = cwt.c->landparam / 10;
            }
            
          }
        }

      if(usethis) {
        straightDownSeek = c;
        downspin = atan2(VC0[1], VC0[0]) - M_PI/2;
        downspin += M_PI/2 * (conformal::rotation%4);
        while(downspin < -M_PI) downspin += 2*M_PI;
        while(downspin > +M_PI) downspin -= 2*M_PI;
        downspin = downspin * min(spd, (double)1);
        // queuechr(xc, yc, sc, 2*vid.fsize, 'X', 0x10100 * int(128 + 100 * sin(ticks / 150.)));
        }
      }
      
    
    if(!inHighQual) {
    
#if defined(ANDROID) || defined(PANDORA) || defined(IOS)
    if(c == lmouseover && (mousepressed || ISANDROID || ISMOBILE)) {
      queuecircle(xc, yc, int(sqrt(squar(xc-xs)+squar(yc-ys)) * .8), c->cpdist > 1 ? 0x00FFFF : 0xFF0000);
      }
#endif

#ifndef MOBILE
    if(cmode == emMapEditor && !mapeditor::subscreen && lmouseover &&
      (mapeditor::whichPattern ? mapeditor::subpattern(c) == mapeditor::subpattern(lmouseover) : c == lmouseover)) {
      queuecircle(xc, yc, int(sqrt(squar(xc-xs)+squar(yc-ys)) * .8), 0x00FFFF);
      }
#endif
    
    if(joydir.d >= 0 && c == cwt.c->mov[(joydir.d+cwt.spin) % cwt.c->type])
      queuecircle(xc, yc, int(sqrt(squar(xc-xs)+squar(yc-ys)) * (.78 - .02 * sin(ticks/199.0))), 0x00FF00);

#ifndef MOBILE    
    if(c == lcenterover && !playermoved && netgen::mode == 0 && !conformal::on)
      queuecircle(xc, yc, int(sqrt(squar(xc-xs)+squar(yc-ys)) * (.70 - .06 * sin(ticks/200.0))), int(175 + 25 * sin(ticks / 200.0)));
#endif

#ifndef MOBILE
    mapeditor::drawGhosts(c, V, ct);
#endif
    }
    
    // process mouse
    
    if(c == cwt.c) curcell = V;
    for(int i=0; i<cwt.c->type; i++) 
      if(c == cwt.c->mov[i]) movecell[i] = V;
    
    // drawline(V*C0, V*Crad[0], 0xC00000);
    if(c->bardir != NODIR && c->bardir != NOBARRIERS && c->land != laHauntedWall &&
      c->barleft != NOWALLSEP_USED) {
      queueline(V*C0, V*heptmove[c->bardir]*C0, 0x505050 >> darken);
      queueline(V*C0, V*hexmove[c->bardir]*C0, 0x505050 >> darken);
      }
    
#ifndef MOBILE
    netgen::buildVertexInfo(c, V);
    rug::buildVertexInfo(c, V);
#endif

#ifdef LOCAL
    extern void localdraw (const transmatrix& V, cell *c);
    localdraw(V, c);
#endif
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
    "To save the game you need an Orb of Safety. Press 'v' for config, ESC for the quest status and menu.\n\n"
    );
  h += XLAT(
    "You can right click any element to get more information about it.\n\n"
    );
#endif
  h += XLAT("See more on the website: ") 
    + "http//roguetemple.com/z/hyper.php\n\n";
    
  
  h += XLAT("Still confused? Read the FAQ on the HyperRogue website!\n\n");
  
#ifdef MOBILE
  h += buildCredits();
#else
  h += XLAT("Press 'c' for credits.");
#endif
  return h;
  }

string musiclicense;

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
    "wonderfullizardofoz, Piotr Migdał, tehora, Michael Heerdegen, Sprite Guard, zelda0x181e, Vipul, snowyowl0, Patashu"
    );
#ifdef EXTRALICENSE
  h += EXTRALICENSE;
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

string generateHelpForItem(eItem it) {
   string help = XLAT(iinf[it].help);
#ifdef ANDROID
   if(it == itOrbSafety)
     help += XLAT("This might be useful for Android devices with limited memory.");
#else
   if(it == itOrbSafety)
     help += XLAT("Thus, it is potentially useful for extremely long games, which would eat all the memory on your system otherwise.\n");
#endif
#ifndef MOBILE
   if(isRangedOrb(it))
     help += XLAT("You can also scroll to the desired location and then press 't'.");
#endif
#ifdef MOBILE
   if(it == itGreenStone)
     help += XLAT("You can touch the Dead Orb in your inventory to drop it.");
#else
   if(it == itGreenStone)
     help += XLAT("You can press 'g' or click them in the list to drop a Dead Orb.");
#endif
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
  return help;
  }

string generateHelpForWall(eWall w) {
  string s = XLAT(winf[w].help);
  if(isThumper(w)) s += pushtext(w);
  if((w == waClosePlate || w == waOpenPlate) && purehepta) 
    s += "\n\n(For the heptagonal mode, the radius has been reduced to 2 for closing plates.)";
  return s;
  }

string generateHelpForMonster(eMonster m) {
  string s = XLAT(minf[m].help);      
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
    
  return s;
  }

string generateHelpForLand(eLand l) {
  string s = XLAT(linf[l].help);

  if(l == laPalace) s = princedesc() + s;

  s += "\n\n";
  if(l == laIce || l == laCaves || l == laDesert || l == laMotion || l == laJungle ||
    l == laCrossroads || l == laAlchemist)
      s += XLAT("Always available.\n");

  #define ACCONLY(z) s += XLAT("Accessible only from %the1.\n", z);
  #define ACCONLY2(z,x) s += XLAT("Accessible only from %the1 or %the2.\n", z, x);
  #define ACCONLYF(z) s += XLAT("Accessible only from %the1 (until finished).\n", z);
  #define TREQ(z) s += XLAT("Treasure required: %1 $$$.\n", #z);
  #define TREQ2(z,x) s += XLAT("Treasure required: %1 x %2.\n", #z, x);
  
  if(l == laMirror || l == laMinefield || l == laPalace ||
    l == laOcean || l == laLivefjord || l == laZebra || l == laGridCoast || l == laGridSea)
      TREQ(30)

  if(l == laCaribbean || l == laWhirlpool) ACCONLY(laOcean)
  if(l == laRlyeh) ACCONLYF(laOcean)
  if(l == laTemple) ACCONLY(laRlyeh)  
  if(l == laClearing) ACCONLY(laOvergrown)  
  if(l == laHaunted) ACCONLY(laGraveyard)  
  if(l == laPrincessQuest) ACCONLY(laPalace)
  if(l == laCamelot) ACCONLY2(laCrossroads, laCrossroads3)
  
  if(l == laDryForest || l == laWineyard || l == laDeadCaves || l == laHive || l == laRedRock ||
    l == laOvergrown || l == laStorms || l == laWhirlwind || l == laRose)
      TREQ(60)
    
  if(l == laIvoryTower) TREQ(30)
  if(l == laIvoryTower) TREQ2(10, itElixir)
  if(l == laEndorian) TREQ2(10, itEdge)
    
  if(l == laCrossroads4) TREQ(200)
  
  if(l == laGraveyard || l == laHive)
    s += XLAT("Kills required: %1.\n", "100");
  
  if(l == laDragon)
    s += XLAT("Different kills required: %1.\n", "20");
  
  if(l == laTortoise) ACCONLY(laDragon)
  if(l == laTortoise) s += XLAT("Find a %1 in %the2.", itBabyTortoise, laDragon);

  if(l == laHell || l == laCrossroads3)
    s += XLAT("Finished lands required: %1 (collect 10 treasure)\n", "9");
  
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
    }
  
  if(l == laPrincessQuest)
    s += XLAT("Kills required: %1.\n", moVizier);

  if(l == laElementalWall) {
    s += XLAT("Kills required: %1 (%2).\n", moFireElemental, laDragon);
    s += XLAT("Kills required: %1 (%2).\n", moEarthElemental, laDeadCaves);
    s += XLAT("Kills required: %1 (%2).\n", moWaterElemental, laLivefjord);
    s += XLAT("Kills required: %1 (%2).\n", moAirElemental, laWhirlwind);
    }
  
  if(l == laZebra) TREQ2(10, itFeather)

  int rl = isRandland(l);
  if(rl == 2)
    s += XLAT("Variants of %the1 are always available in the Random Pattern Mode.", l);
  else if(rl == 1)
    s += XLAT(
      "Variants of %the1 are available in the Random Pattern Mode after "
      "getting a highscore of at least 10 %2.", l, treasureType(l));

  return s;
  }

void describeMouseover() {
  DEBB(DF_GRAPH, (debugfile,"describeMouseover\n"));

#ifdef LOCAL
    if(localDescribe()) return;
#endif

  cell *c = mousing ? mouseover : playermoved ? NULL : centerover;
  string out = mouseovers;
  if(!c) { }
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
    else if(c->land == laOcean && c->landparam <= 25)
      out += " (" + its(c->landparam)+")";

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

      out += " LP:" + its(c->landparam)+"/"+its(turncount);
      
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
    // if(c->type == 7) out += " Z"+its(c->master->zebraval);
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
          "Trees in this forest can be cut down. Big trees take two turns to cut down.";
      else if(c->wall != waSea && c->wall != waPalace)
      if(!((c->wall == waCavefloor || c->wall == waCavewall) && c->land == laEmerald))
        help = generateHelpForWall(c->wall);
      }
    
    if(isActivable(c)) out += XLAT(" (touch to activate)");
    
    if(hasTimeout(c)) out += XLAT(" [%1 turns]", its(c->wparam));
  
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
      if(c->item == itBabyTortoise && tortoise::seek()) 
        out += " " + tortoise::measure(tortoise::babymap[c]);
      if(!c->monst) help = generateHelpForItem(c->item);
      }
    
    if(c == cwt.c && !shmup::on) out += XLAT(", you");

    if(shmup::mousetarget && intval(mouseh, shmup::mousetarget->pat*C0) < .1) {
      out += ", "; out += XLAT1(minf[shmup::mousetarget->type].name);
      help = XLAT(minf[shmup::mousetarget->type].help);
/*    char buf[64];
      sprintf(buf, "%Lf", intval(mouseh, shmup::mousetarget->pat*C0));
      mouseovers = mouseovers + " D: " + buf;
      printf("ms = %s\n", mouseovers.c_str());*/
      }

    if(rosedist(c) == 1)
      out += ", wave of scent (front)";

    if(rosedist(c) == 2)
      out += ", wave of scent (back)";
    
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
    else if(getcstat == 'f') {
      out = XLAT("[+] keep the window size, [-] use the screen resolution");
      }
    else if(getcstat == 'a' && vid.aspeed > -4.99)
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
  if(cmode != emPickScores)
#ifdef MOBILE
  if(cmode != emNormal && cmode != emQuit)
#endif
    displayfr(vid.xres/2, vid.fsize,   2, vid.fsize, out, col, 8);
  if(mousey < vid.fsize * 3/2) getcstat = SDLK_F1;

  if(false && shmup::mousetarget) {
    char buf[64];
    sprintf(buf, "%Lf", (long double) intval(mouseh, shmup::mousetarget->pat*C0));
    mouseovers = mouseovers + " D: " + buf;
    return;
    }
  }

void drawrec(const heptspin& hs, int lev, hstate s, transmatrix V) {

  shmup::calc_relative_matrix(cwt.c, hs.h);
    
  cell *c = hs.h->c7;
  
  drawcell(c, V * spin(hs.spin*2*M_PI/7 + (purehepta ? M_PI:0)), hs.spin);
  
  if(lev <= 0) return;
  
  if(!purehepta) for(int d=0; d<7; d++) {
    int ds = fixrot(hs.spin + d);
    // createMov(c, ds);
    if(c->mov[ds] && c->spn[ds] == 0)
      drawcell(c->mov[ds], V * hexmove[d], 0);
    }

  if(lev <= 1) return;
  
  for(int d=0; d<7; d++) {
    hstate s2 = transition(s, d);
    if(s2 == hsError) continue;
    heptspin hs2 = hsstep(hsspin(hs, d), 0);
    drawrec(hs2, lev-2, s2, V * heptmove[d]);
    }
  
  }

int mindx=-7, mindy=-7, maxdx=7, maxdy=7;
  

void drawEuclidean() {
  DEBB(DF_GRAPH, (debugfile,"drawEuclidean\n"));
  eucoord px, py;
  if(!lcenterover) lcenterover = cwt.c;
  // printf("centerover = %p player = %p [%d,%d]-[%d,%d]\n", lcenterover, cwt.c,
  //   mindx, mindy, maxdx, maxdy);
  decodeMaster(lcenterover->master, px, py);
  
  int minsx = mindx-1, maxsx=maxdx+1, minsy=mindy-1, maxsy=maxdy+1;
  mindx=maxdx=mindy=maxdy=0;
  
  for(int dx=minsx; dx<=maxsx; dx++)
  for(int dy=minsy; dy<=maxsy; dy++) {
    eucoord x = dx+px;
    eucoord y = dy+py;
    cell *c = euclideanAt(x,y);
    if(!c) continue;
    transmatrix Mat = Id;
    Mat[2][2] = 1;
    Mat[0][2] += (x + y * .5) * eurad;
    double q3 = sqrt(double(3));
    Mat[1][2] += y * q3 /2 * eurad;
    while(Mat[0][2] <= -16384 * eurad) Mat[0][2] += 32768 * eurad;
    while(Mat[0][2] >= 16384 * eurad) Mat[0][2] -= 32768 * eurad;
    while(Mat[1][2] <= -16384 * q3 * eurad) Mat[1][2] += 32768 * q3 * eurad;
    while(Mat[1][2] >= 16384 * q3 * eurad) Mat[1][2] -= 32768 * q3 * eurad;
    Mat = View * Mat;
    
    // Mat[0][0] = -1;
    // Mat[1][1] = -1;
    
    // Mat[2][0] = x*x/10;
    // Mat[2][1] = y*y/10;
    // Mat = Mat * xpush(x-30) * ypush(y-30);

    int cx, cy, shift;
    getcoord(Mat * C0, cx, cy, shift);
    if(cx >= 0 && cy >= 0 && cx < vid.xres && cy < vid.yres) {
      if(dx < mindx) mindx = dx;
      if(dy < mindy) mindy = dy;
      if(dx > maxdx) maxdx = dx;
      if(dy > maxdy) maxdy = dy;
      }
    
    drawcell(c, Mat, 0);
    }
  }

void drawthemap() {

  DEBB(DF_GRAPH, (debugfile,"draw the map\n"));
  fanframe = ticks / 150.0 / M_PI;
  
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
  showPirateX = cwt.c->item == itCompass;
    
  using namespace yendor;
  
  if(yii < size(yi)) {
    if(!yi[yii].found) 
      for(int i=0; i<YDIST; i++) 
        if(yi[yii].path[i]->cpdist <= sightrange) {
      keycell = yi[yii].path[i];
      keycelldist = YDIST - i;
      }
    }
  
  #ifndef MOBILE
  lmouseover = mouseover;
  #endif
  modist = 1e20; mouseover = NULL; 
  modist2 = 1e20; mouseover2 = NULL; 
  mouseovers = XLAT("Press F1 or right click for help");
  centdist = 1e20; lcenterover = centerover; centerover = NULL; 
  #ifdef MOBILE
  mouseovers = XLAT("No info about this...");
  #endif
  if(outofmap(mouseh)) 
    modist = -5;
  playerfound = false;
  // playerfoundL = false;
  // playerfoundR = false;
  
  if(euclid)
    drawEuclidean();
  else
    drawrec(viewctr, 
      conformal::on ? sightrange + 2:
      (!playermoved) ? sightrange+1 : sightrange + 4,
      hsOrigin, View);

  if(shmup::on) {
    if(shmup::players == 1)
      cwtV = shmup::pc[0]->pat;
    else if(shmup::centerplayer != -1) 
      cwtV = shmup::pc[shmup::centerplayer]->pat;
    else if(shmup::players == 2) {
      hyperpoint h0 = shmup::pc[0]->pat * C0;
      hyperpoint h1 = shmup::pc[1]->pat * C0;
      hyperpoint h2 = mid(h0, h1);
      cwtV = rgpushxto0(h2);
      }
    else if(shmup::players == 3) {
      hyperpoint h0 = shmup::pc[0]->pat * C0;
      hyperpoint h1 = shmup::pc[1]->pat * C0;
      hyperpoint h2 = shmup::pc[2]->pat * C0;
      hyperpoint h3 = mid3(h0, h1, h2);
      cwtV = rgpushxto0(h3);
      }
    else if(shmup::players == 4) {
      hyperpoint h0 = shmup::pc[0]->pat * C0;
      hyperpoint h1 = shmup::pc[1]->pat * C0;
      hyperpoint h2 = shmup::pc[2]->pat * C0;
      hyperpoint h3 = shmup::pc[3]->pat * C0;
      hyperpoint h4 = mid4(h0, h1, h2, h3);
      cwtV = rgpushxto0(h4);
      }
    }

  #ifdef LOCAL
  localDrawMap();
  #endif
  }

void spinEdge(ld aspd) { 
  if(downspin >  aspd) downspin =  aspd;
  if(downspin < -aspd) downspin = -aspd;
  View = spin(downspin) * View;
  }

void centerpc(ld aspd) { 
  DEBB(DF_GRAPH, (debugfile,"center pc\n"));
  hyperpoint H = cwtV * C0;
  ld R = sqrt(H[0] * H[0] + H[1] * H[1]);
  if(R < 1e-9) {
    /* if(playerfoundL && playerfoundR) {
      
      } */
    spinEdge(aspd);
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

void drawmovestar() {

  DEBB(DF_GRAPH, (debugfile,"draw movestar\n"));
  if(!playerfound) return;
  
  if(shmup::on) return;
  if(rug::rugged) return;
  
  if(vid.axes == 0 || (vid.axes == 1 && mousing)) return;

  hyperpoint H = cwtV * C0;
  ld R = sqrt(H[0] * H[0] + H[1] * H[1]);
  transmatrix Centered = Id;

  if(euclid) 
    Centered = eupush(H[0], H[1]);
  else if(R > 1e-9) Centered = rgpushxto0(H);
  
  int starcol = (vid.goteyes2 ? 0xE08060 : 0xC00000);
  
  if(vid.axes == 3 || (vid.wallmode == 2 && vid.axes == 1))
    queuepoly(Centered, shMovestar, darkena(starcol, 0, 0xFF));
  
  else for(int d=0; d<8; d++) {
    int col = starcol;
#ifdef PANDORA
    if(leftclick && (d == 2 || d == 6 || d == 1 || d == 7)) col >>= 2;
    if(rightclick && (d == 2 || d == 6 || d == 3 || d == 5)) col >>= 2;
    if(!leftclick && !rightclick && (d&1)) col >>= 2;
#endif
//  EUCLIDEAN
    if(euclid)
      queueline(Centered * C0, Centered * ddi(d * 10.5, 0.5) * C0, col >> darken);
    else
      queueline(Centered * C0, Centered * spin(M_PI*d/4)* xpush(.5) * C0, col >> darken);
    }
  }

void optimizeview() {
  
  DEBB(DF_GRAPH, (debugfile,"optimize view\n"));
  int turn = 0;
  ld best = INF;
  
  transmatrix TB = Id;
  
  for(int i=-1; i<7; i++) {

    ld trot = -i * M_PI * 2 / 7.0;
    transmatrix T = i < 0 ? Id : spin(trot) * xpush(tessf) * spin(M_PI);
    hyperpoint H = View * T * C0;
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
  hyperpoint H = cwtV * C0;
  ld R = sqrt(H[0] * H[0] + H[1] * H[1]);
  transmatrix Centered = cwtV;
  if(!euclid)
    Centered = gpushxto0(H)  * Centered;
  else if(R > 1e-9)
    Centered = eupush(-H[0], -H[1]) * Centered;
  ld binv = 99;
  
  ld dirdist[7];
  for(int i=0; i<cwt.c->type; i++) 
    dirdist[i] = intval(Centered * spin(-i * 2 * M_PI /cwt.c->type) * xpush(1) * C0, P);
  
  movedir res;
  res.d = -1;
  
  for(int i=0; i<cwt.c->type; i++) {
    if(dirdist[i] < binv) {
      binv = dirdist[i];
      res.d = i;
      res.subdir = dirdist[(i+1)%cwt.c->type] < dirdist[(i+cwt.c->type-1)%cwt.c->type] ? 1 : -1;
      }
    }
  // if(euclid) bdir = (bdir + 3) % 6;
  return res;
  }

void movepckeydir(int d) {
  DEBB(DF_GRAPH, (debugfile,"movepckeydir\n"));
  // EUCLIDEAN
  if(euclid)
    movepcto(vectodir(spin(-d * M_PI/4) * eupush(1, 0) * C0));
  else
    movepcto(vectodir(spin(-d * M_PI/4) * xpush(1) * C0));
  }

void calcMousedest() {
  if(outofmap(mouseh)) return;
  if(revcontrol == true) { mouseh[0] = -mouseh[0]; mouseh[1] = -mouseh[1]; }
  ld mousedist = intval(mouseh, curcell * C0);
  mousedest.d = -1;
  
  ld dists[7];
  
  for(int i=0; i<cwt.c->type; i++) 
    dists[i] = intval(mouseh, movecell[i] * C0);
  
  /* printf("curcell = %Lf\n", mousedist);
  for(int i=0; i<cwt.c->type; i++)
    printf("d%d = %Lf\n", i, dists[i]); */

  for(int i=0; i<cwt.c->type; i++) if(dists[i] < mousedist) {
    mousedist = dists[i];
    mousedest.d = fixdir(i - cwt.spin, cwt.c);
    
    mousedest.subdir =
       dists[(i+1)%cwt.c->type] < dists[(i+cwt.c->type-1)%cwt.c->type] ? 1 : -1;
    }
  
  }

void mousemovement() {
  calcMousedest();
  movepcto(mousedest);
  }

long double sqr(long double x) { return x*x; }

void checkjoy() {
  DEBB(DF_GRAPH, (debugfile,"check joy\n"));
  if(shmup::on) return;
  ld joyvalue1 = sqr(vid.joyvalue);
  ld joyvalue2 = sqr(vid.joyvalue2);
  
  ld jx = joyx;
  ld jy = joyy;
  ld sq = jx*jx+jy*jy;
  
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

void calcparam() {
  DEBB(DF_GRAPH, (debugfile,"calc param\n"));
  vid.xcenter = vid.xres / 2;
  vid.ycenter = vid.yres / 2;
  vid.radius = int(vid.scale * vid.ycenter) - (ISANDROID ? 2 : ISIOS ? 40 : 40);
  
  if(vid.xres < vid.yres) {
    vid.radius = int(vid.scale * vid.xcenter) - (ISIOS ? 10 : 2);
    vid.ycenter = vid.yres - vid.radius - vid.fsize - (ISIOS ? 10 : 0);
    }

  vid.beta = 1 + vid.alpha + vid.eye;
  vid.alphax = vid.alpha + vid.eye;
  vid.goteyes = vid.eye > 0.001 || vid.eye < -0.001;
  vid.goteyes2 = vid.goteyes;
  }

void displayStat(int y, const string& name, const string& val, char mkey) {
  
  int dy = vid.fsize * y + vid.yres/4;
  int dx = vid.xres/2 - 100;
  
  bool xthis = (mousey >= dy-vid.fsize/2 && mousey <= dy + vid.fsize/2);
  int xcol = 0x808080;
  
  if(xthis) {
    getcstat = mkey; getcshift = 0;
    int mx = mousex - dx;
    if(mx >= 0 && mx <= 100) {
      if(mx < 20) getcshift = -1   , xcol = 0xFF0000;
      else if(mx < 40) getcshift = -0.1 , xcol = 0x0000FF;
      else if(mx < 50) getcshift = -0.01, xcol = 0x00FF00;
      if(mx > 80) getcshift = +1   , xcol = 0xFF0000;
      else if(mx > 60) getcshift = +0.1 , xcol = 0x0000FF;
      else if(mx > 50) getcshift = +0.01, xcol = 0x00FF00;
      }
    }
  
  if(val != "") {
    if(val[0] != ' ') {
      displaystr(dx,    dy, 0, vid.fsize, val, xthis ? 0xFFFF00 : 0x808080, 16);
      displaystr(dx+25, dy, 0, vid.fsize, "-", xthis && getcshift < 0 ? xcol : 0x808080, 8);
      displaystr(dx+75, dy, 0, vid.fsize, "+", xthis && getcshift > 0 ? xcol : 0x808080, 8);
      }
    else
      displaystr(dx+75, dy, 0, vid.fsize, val, xthis ? 0xFFFF00 : 0x808080, 16);
    }

#ifndef MOBILE
  string mk = s0 + mkey;
  int hkx = dx + 100;
  if(mkey >= 1 && mkey <= 26) mk = s0 + "^", mk += (mkey+64), hkx -= vid.fsize;
  // if(mkey >= 64+1 && mkey <= 64+26) mk = s0 + "Shift+", mk += mkey;
  displaystr(hkx, dy, 0, vid.fsize, mk, xthis ? 0xFFFF00 : 0xC0F0C0, 0);
#endif

  displaystr(dx+125, dy, 0, vid.fsize, name, xthis ? 0xFFFF00 : 0x808080, 0);
  }

void displayStatHelp(int y, string name) {
  
  int dy = vid.fsize * y + vid.yres/4;
  int dx = vid.xres/2 - 100;
  
  displaystr(dx+100, dy, 0, vid.fsize, name, 0xC0C0C0, 0);
  }

void displayButton(int x, int y, const string& name, int key, int align, int rad) {
  if(displayfr(x, y, rad, vid.fsize, name, 0x808080, align)) {
    displayfr(x, y, rad, vid.fsize, name, 0xFFFF00, align);
    getcstat = key;
    }
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
  if(canmove) displayButton(vid.xres/2, y + vid.fsize*16/2, XLAT("or another key to continue"), ' ', 8, 2);
  else displayButton(vid.xres/2, y + vid.fsize*16/2, XLAT("or ESC to see how it ended"), SDLK_ESCAPE, 8, 2);
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
  int y = vid.yres * (1000-618) / 1000 - vid.fsize * 7/2;
  displayfr(vid.xres/2, y, 4, vid.fsize*2, 
    cheater ? XLAT("It is a shame to cheat!") : 
    showoff ? XLAT("Showoff mode") :
    canmove && princess::challenge ? XLAT("%1 Challenge", moPrincess) :
    canmove ? XLAT("Quest status") : 
    XLAT("GAME OVER"), 0xC00000, 8
    );
  displayfr(vid.xres/2, y + vid.fsize*2, 2, vid.fsize,   XLAT("Your score: %1", its(gold())), 0xD0D0D0, 8);
  displayfr(vid.xres/2, y + vid.fsize*3, 2, vid.fsize,   XLAT("Enemies killed: %1", its(tkills())), 0xD0D0D0, 8);
  
  if(!timerstopped && !canmove) {
    savetime += time(NULL) - timerstart;
    timerstopped = true;
    }
  if(canmove && !timerstart)
    timerstart = time(NULL);
  
  if(items[itOrbYendor]) {
    displayfr(vid.xres/2, y + vid.fsize*4, 2, vid.fsize,  XLAT("Orbs of Yendor found: %1", its(items[itOrbYendor])), 0xFF00FF, 8);
    displayfr(vid.xres/2, y + vid.fsize*5, 2, vid.fsize,  XLAT("CONGRATULATIONS!"), 0xFFFF00, 8);
    }
  else {
    if(princess::challenge) ;
    else if(gold() < 30)
      displayfr(vid.xres/2, y+vid.fsize*5, 2, vid.fsize, XLAT("Collect 30 $$$ to access more worlds"), 0xC0C0C0, 8);
    else if(gold() < 60)
      displayfr(vid.xres/2, y+vid.fsize*5, 2, vid.fsize, XLAT("Collect 60 $$$ to access even more lands"), 0xC0C0C0, 8);
    else if(!hellUnlocked())
      displayfr(vid.xres/2, y+vid.fsize*5, 2, vid.fsize, XLAT("Collect at least 10 treasures in each of 9 types to access Hell"), 0xC0C0C0, 8);
    else if(items[itHell] < 10)
      displayfr(vid.xres/2, y+vid.fsize*5, 2, vid.fsize, XLAT("Collect at least 10 Demon Daisies to find the Orbs of Yendor"), 0xC0C0C0, 8);
    else if(size(yendor::yi) == 0)
      displayfr(vid.xres/2, y+vid.fsize*5, 2, vid.fsize, XLAT("Look for the Orbs of Yendor in Hell or in the Crossroads!"), 0xC0C0C0, 8);
    else 
      displayfr(vid.xres/2, y+vid.fsize*5, 2, vid.fsize, XLAT("Unlock the Orb of Yendor!"), 0xC0C0C0, 8);
    }
  if(princess::challenge) 
    displayfr(vid.xres/2, y+vid.fsize*6, 2, vid.fsize, XLAT("Follow the Mouse and escape with %the1!", moPrincess), 0xC0C0C0, 8);
  else if(tkills() < 100)
    displayfr(vid.xres/2, y+vid.fsize*6, 2, vid.fsize, XLAT("Defeat 100 enemies to access the Graveyard"), 0xC0C0C0, 8);
  else if(kills[moVizier] == 0 && (items[itFernFlower] < 5 || items[itGold] < 5)) {
    displayfr(vid.xres/2, y+vid.fsize*6, 2, vid.fsize, XLAT("Kill a Vizier in the Palace to access Emerald Mine"), 0xC0C0C0, 8);
    }
  else if(items[itEmerald] < 5)
    displayfr(vid.xres/2, y+vid.fsize*6, 2, vid.fsize, XLAT("Collect 5 Emeralds to access Camelot"), 0xC0C0C0, 8);
  else if(hellUnlocked()) {
    bool b = true;
    for(int i=0; i<LAND_HYP; i++)
      if(b && items[treasureType(land_hyp[i])] < 10) {
        displayfr(vid.xres/2, y+vid.fsize*6, 2, vid.fsize, 
          XLAT(
            land_hyp[i] == laTortoise ? "Hyperstone Quest: collect at least 10 points in %the2" :
            "Hyperstone Quest: collect at least 10 %1 in %the2", 
            treasureType(land_hyp[i]), land_hyp[i]), 
          0xC0C0C0, 8);
        b = false;
        }
    if(b)
      displayfr(vid.xres/2, y+vid.fsize*6, 2, vid.fsize, XLAT("Hyperstone Quest completed!"), 0xC0C0C0, 8);
    }
  else {
    displayfr(vid.xres/2, y+vid.fsize*6, 2, vid.fsize, XLAT("Some lands unlock at specific treasures or kills"), 0xC0C0C0, 8);
    }
  if((!canmove) && (!ISMOBILE))
    displayfr(vid.xres/2, y+vid.fsize*7, 2, vid.fsize, XLAT("(press ESC during the game to review your quest)"), 0xB0B0B0, 8);
  if(cheater)
    displayfr(vid.xres/2, y+vid.fsize*9, 2, vid.fsize, XLAT("you have cheated %1 times", its(cheater)), 0xFF2020, 8);
  if(!cheater) {
    displayfr(vid.xres/2, y + vid.fsize*9, 2, vid.fsize, timeline(), 0xC0C0C0, 8);
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
      m.msg = gamelog[i];
      mnum++,
      msgs.push_back(m);
      }

  if(mnum)
    displayfr(vid.xres/2, vid.yres-vid.fsize*(mnum+1), 2, vid.fsize/2,  XLAT("last messages:"), 0xC0C0C0, 8);
  
  #ifndef MOBILE
  quitOrAgain();
  #endif
  }

string ifMousing(string key, string s) {
  if(mousing) return XLAT(s);
  else return key + " - " + XLAT(s);
  }

#ifdef MOBILE
void displayabutton(int px, int py, string s, int col) {
  // TMP
  int siz = vid.yres > vid.xres ? vid.fsize*2 : vid.fsize * 3/2;
  int x = vid.xcenter + px * (vid.radius);
  int y = vid.ycenter + py * (vid.radius - siz/2);
  if(gtouched && !mouseover
    && abs(mousex - vid.xcenter) < vid.radius
    && abs(mousey - vid.ycenter) < vid.radius
    && hypot(mousex-vid.xcenter, mousey-vid.ycenter) > vid.radius
    && px == (mousex > vid.xcenter ? 1 : -1)
    && py == (mousey > vid.ycenter ? 1 : -1)
    ) col = 0xFF0000;
  if(displayfr(x, y, 0, siz, s, col, 8+8*px))
    buttonclicked = true;
  }
#endif

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

#ifndef ANDROID
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
      if(fscanf(f, "%s", buf) <= 0) break; sc.ver = buf;

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
  }

bool notgl = false;

void showPickScores() {

  int d = scoredisplay;

  vector<pair<string, int> > v;
  for(int i=0; i<POSSCORE; i++) {
    scoredisplay = i;
    if(!fakescore())
      v.push_back(make_pair(displayfor(NULL), i));
    }
  sort(v.begin(), v.end());
  
  int q = (int) v.size();
  int percolumn = vid.yres / (vid.fsize+3) - 4;
  int columns = 1 + (q-1) / percolumn;
  
  for(int i=0; i<q; i++) {
    int x = 16 + (vid.xres * (i/percolumn)) / columns;
    int y = (vid.fsize+3) * (i % percolumn) + vid.fsize*2;
    
    scoredisplay = v[i].second;
    if(!fakescore())
      displayButton(x, y, v[i].first, char(33+scoredisplay), 0);
    }
    
  scoredisplay = d;
  }

void showScores() {
  int y = vid.fsize * 7/2;
  int bx = vid.fsize;
  
  string modes = 
    scoremode == 0 ? XLAT(", m - mode: normal") :
    scoremode == 1 ? XLAT(", m - mode: shoot'em up") :
    scoremode == 2 ? XLAT(", m - mode: hardcore only") :
    "?";

  if(euclid) modes += XLAT(" (E:%1)", euclidland);


  mouseovers = XLAT("t/left/right - change display, up/down - scroll, s - sort by") + modes;

  displaystr(bx*4, vid.fsize*2, 0, vid.fsize, "#", 0xFFFFFF, 16);
  displaystr(bx*8, vid.fsize*2, 0, vid.fsize, "$$$", 0xFFFFFF, 16);
  displaystr(bx*12, vid.fsize*2, 0, vid.fsize, XLAT("kills"), 0xFFFFFF, 16);
  displaystr(bx*18, vid.fsize*2, 0, vid.fsize, XLAT("time"), 0xFFFFFF, 16);
  displaystr(bx*22, vid.fsize*2, 0, vid.fsize, XLAT("ver"), 0xFFFFFF, 16);
  displaystr(bx*23, vid.fsize*2, 0, vid.fsize, displayfor(NULL), 0xFFFFFF, 0);
  if(scorefrom < 0) scorefrom = 0;
  int id = scorefrom;
  while(y < (ISIOS ? vid.yres - 3*vid.fsize : vid.yres)) { 
    if(id >= size(scores)) break;
        
    score& S(scores[id]);
    
    bool wrongtype = false;
    
    wrongtype |= (euclid && (!S.box[116] || S.box[120] != euclidland));
    wrongtype |= (!euclid && S.box[116]);

    wrongtype |= (scoremode == 1 && !S.box[119]);
    wrongtype |= (scoremode != 1 && S.box[119]);
    wrongtype |= (scoremode == 2 && (!S.box[117] || S.box[118] >= PUREHARDCORE_LEVEL));
    
    if(wrongtype) { id++; continue; }

    char buf[16];
    
    sprintf(buf, "%d", id+1);
    displaystr(bx*4,  y, 0, vid.fsize, buf, 0xC0C0C0, 16);
    
    sprintf(buf, "%d", S.box[2]);
    displaystr(bx*8,  y, 0, vid.fsize, buf, 0xC0C0C0, 16);
    
    sprintf(buf, "%d", S.box[3]);
    displaystr(bx*12,  y, 0, vid.fsize, buf, 0xC0C0C0, 16);

    sprintf(buf, "%d:%02d", S.box[0]/60, S.box[0] % 60);
    displaystr(bx*18,  y, 0, vid.fsize, buf, 0xC0C0C0, 16);

    displaystr(bx*22,  y, 0, vid.fsize, S.ver, 0xC0C0C0, 16);

    displaystr(bx*23, y, 0, vid.fsize, displayfor(&S), 0xC0C0C0, 0);
    
    y += vid.fsize*5/4; id++;
    }

#ifdef IOS
  buttonclicked = false;
  displayabutton(-1, +1, XLAT("SORT"), BTON);
  displayabutton(+1, +1, XLAT("PLAY"), BTON);
#endif
  }

void sortScores() {
  if(scorerev) reverse(scores.begin(), scores.end());
  else {
    scorerev = true;
    scoresort = scoredisplay; 
    stable_sort(scores.begin(), scores.end(), scorecompare);
    }
  }

void shiftScoreDisplay(int delta) {
  scoredisplay = (scoredisplay + POSSCORE + delta) % POSSCORE, scorerev = false;
  if(fakescore()) shiftScoreDisplay(delta);
  }
#endif

#ifndef MOBILE
void handleScoreKeys(int sym, SDL_Event& ev) {
  if(sym == SDLK_LEFT || sym == SDLK_KP4 || sym == 'h' || sym == 'a')
    shiftScoreDisplay(-1);
  else if(sym == SDLK_RIGHT || sym == SDLK_KP6 || sym == 'l' || sym == 'd')
    shiftScoreDisplay(1);
  else if(sym == 't') cmode = emPickScores;
  else if(sym == SDLK_UP || sym == 'k' || sym == 'w')
    scorefrom -= 5;
  else if(sym == SDLK_DOWN || sym == 'j' || sym == 'x')
    scorefrom += 5;
  else if(sym == 's') sortScores(); 
  else if(sym == 'm') { scoremode++; scoremode %= 3; }
  else if(sym != 0 || ev.type == SDL_MOUSEBUTTONDOWN) cmode = emNormal;
  }
        
void handlePickScoreKeys(int uni, SDL_Event& ev) {
  if(uni != 0) {
    int k = uni - '!';
    if(k >= 0 && k < POSSCORE) scoredisplay = k;
    cmode = emScores;
    scorerev = false;
    }
  }
        
#endif

void setAppropriateOverview() {
  if(tactic::on)
    cmode = emTactic;
  else if(yendor::on)
    cmode = emYendor;
  else if(euclid)
    cmode = emPickEuclidean;
  else 
    cmode = emOverview;
  }

void drawStats() {

  DEBB(DF_GRAPH, (debugfile,"drawStats\n"));

#ifdef IOS
  if(cmode != emNormal && cmode != emQuit)
    return;
#endif
  
  int vx, vy;

  int s = vid.fsize;
  
  bool portrait = vid.xres < vid.yres;
  
  if(portrait) {
    vx = vid.fsize * 3;
    vy = vid.fsize * 2;
    if(havebugs) vy += vid.fsize * 3/2;
    }
  else {
    vx = vid.xres - vid.fsize * 3;
    vy = vid.fsize;
    }
  
  #define ADV(z) \
    {if(portrait) { \
      vx += vid.fsize*4; \
      if(vx > vid.xres - vid.fsize*2) vx = vid.fsize * 3, vy += vid.fsize; \
      } \
    else { \
      vy += vid.fsize * z/2; \
      if(vy > vid.yres || (vx > vid.xres/2 && vy > vid.yres - s * 3)) vx += (vx > vid.xres/2 ? -5:5) * vid.fsize, vy = vid.fsize * 5/2; \
      }}

  if(!portrait) vid.portreduction = 0;
  if(vid.xres >= vid.yres * 5/3 || portrait) 
    vid.killreduction = vid.itemreduction = 0;
    
  if(portrait) vid.fsize = s - vid.portreduction;
  else vid.fsize = s - vid.itemreduction;
  
  if(displaynum(vx, vy, 0, vid.fsize, 0xFFFFFF, gold(), "$$$")) {
    mouseovers = XLAT("Your total wealth"),
    help = XLAT(
      "The total value of the treasure you have collected.\n\n"
      "Every world type contains a specific type of treasure, worth 1 $$$; "
      "your goal is to collect as much treasure as possible, but every treasure you find "
      "causes more enemies to hunt you in its native land.\n\n"
      "Orbs of Yendor are worth 50 $$$ each.\n\n"
      );
    }
      
  ADV(3); if(portrait) {ADV(3);}

#ifndef MOBILE
  int oldz = 0;
#endif
    
  for(int z=0; z<3; z++) for(int i=0; i<ittypes; i++) if(itemclass(eItem(i)) == z) {
#ifndef MOBILE
    if(z != oldz) { ADV(1); oldz = z; }
#endif
    if(items[i])  {
      bool b = displaynum(vx, vy, 0, vid.fsize, iinf[i].color, items[i], s0 + iinf[i].glyph);
      ADV(2);
      if(b) {
        int t = itemclass(eItem(i));
        if(t == IC_TREASURE)
          mouseovers = XLAT("treasure collected: %1", eItem(i));
        if(t == IC_OTHER)
          mouseovers = XLAT("objects found: %1", eItem(i));
        if(t == IC_ORB)
          mouseovers = XLAT("orb power: %1", eItem(i));
        if(i == itGreenStone) {
          mouseovers += XLAT(" (click to drop)");
          getcstat = 'g';
          }
        help = generateHelpForItem(eItem(i));
        }
      }
    }

  // printf("vx = %d/%d fsize = %d\n", vx, vid.xres - vid.fsize*4, vid.fsize);
  if(vx < vid.xres - s * 3)
    vid.itemreduction++;
  
  if(portrait) {
    vx = vid.fsize * 3;
    vy += vid.fsize * 2; // vy = vid.fsize * 5;
    }
  else {
    vx = vid.fsize * 3;
    vy = vid.fsize;
    vid.fsize = s;
    }
  
  if(displaynum(vx, vy, 0, vid.fsize, 0xFFFFFF, calcfps(), "fps"))
    mouseovers = XLAT("frames per second"),
    help = 
      XLAT(
      "The higher the number, the smoother the animations in the game. "
      "If you find that animations are not smooth enough, you can try "
      "to change the options "
      ) +
#ifdef ANDROID
XLAT(
      "(Menu button) and select the ASCII mode, which runs much faster. "
      "Depending on your device, turning the OpenGL mode on or off might "
      "make it faster, slower, or cause glitches.");
#else
#ifdef IOS
XLAT(
      "(in the MENU). You can reduce the sight range, this should make "
      "the animations smoother.");
#else
XLAT(
      "(press v) and change the wall/monster mode to ASCII, or change "
      "the resolution.");
#endif
// todo: iOS
#endif
  ADV(3);

  int killtypes = 0;
  for(int i=1; i<motypes; i++) if(kills[i]) killtypes++;
  
  if(killtypes >= 3) {
    int kvx, kvy;
    if(!portrait) 
      kvx = vid.fsize * 8, kvy = vid.fsize;
    else { ADV(3); kvx=vx, kvy=vy; }
    if(displaynum(kvx, kvy, 0, vid.fsize, 0xFFFFFF, tkills(), "XX"))
      mouseovers = XLAT("Your total kills"),
      help = XLAT(
        "In most lands, more treasures are generated with each enemy native to this land you kill. "
        "Moreover, 100 kills is a requirement to enter the Graveyard and the Hive.\n\n"
        "Friendly creatures and parts of monsters (such as the Ivy) do appear in the list, "
        "but are not counted in the total kill count.");
    if(portrait) ADV(6);
    }
  
  if(!portrait) vid.fsize = s - vid.killreduction;
  
  for(int i=1; i<motypes; i++) if(kills[i])  {
    eMonster m = eMonster(i);
    
    if(displaynum(vx, vy, 0, vid.fsize, minf[i].color, kills[i], s0 + minf[i].glyph)) {
      help = generateHelpForMonster(m);
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
      }
    ADV(2);
    }
  
  if(!portrait && vx > s * 3) vid.killreduction++;
  if(portrait && vy > vid.ycenter - vid.radius && vid.fsize > 1) vid.portreduction ++;
  vid.fsize = s;  
  
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

void saveHighQualityShot() {

#ifndef GFX
  addMessage(XLAT("High quality shots not available on this platform"));
  return;
#endif

  int dcs = size(dcal);
  for(int i=0; i<dcs; i++) {
    cell *c = dcal[i];
    if(c->cpdist <= 4) setdist(c, 1, NULL);
    }

  time_t timer;
  timer = time(NULL);

  SDL_Surface *sav = s;
  s = SDL_CreateRGBSurface(SDL_SWSURFACE,2000,2000,32,0,0,0,0);
  
  int ssr = sightrange; sightrange = 10; int sch = cheater; cheater = 0;

  bool b = vid.usingGL;
  vid.usingGL = false;
  videopar vid2 = vid;
  vid.xres = vid.yres = 2000; 
  // if(vid.pmodel == 0) vid.scale = 0.99;
  calcparam();
  inHighQual = true;

  darken = 0;

  for(int i=0; i<2; i++) {
    SDL_FillRect(s, NULL, i ? 0xFFFFFF : 0);
    drawfullmap();
  
    char buf[128]; strftime(buf, 128, "bigshota-%y%m%d-%H%M%S" IMAGEEXT, localtime(&timer));
    buf[7] += i;
    IMAGESAVE(s, buf);

    if(i == 0) addMessage(XLAT("Saved the high quality shot to %1", buf));
    }
  
  inHighQual = false;
  SDL_FreeSurface(s); s = sav; vid = vid2; sightrange = ssr; cheater = sch;
  vid.usingGL = b;
  }
#endif

void drawfullmap() {

  DEBB(DF_GRAPH, (debugfile,"draw full map\n"));
    
  ptds.clear();

  if(!vid.goteyes && !euclid && pmodel == 0) {
    queuecircle(vid.xcenter, vid.ycenter, vid.radius, 0xFF >> darken);
    }

  if(pmodel == 3 || pmodel == 4) polygonal::drawBoundary(0xFF >> darken);
  
  if(vid.wallmode < 2 && !euclid && !mapeditor::whichShape) {
    int ls = size(lines);
    if(ISMOBILE) ls /= 10;
    for(int t=0; t<ls; t++) queueline(View * lines[t].P1, View * lines[t].P2, lines[t].col >> darken);
    }

  drawthemap();
  #ifndef MOBILE
  if(!inHighQual) {
    if(cmode == emNormal && !rug::rugged) drawmovestar();
    if(rug::rugged && !rug::renderonce) queueline(C0, mouseh, 0xFF00FF);
    mapeditor::drawGrid();
    }
  #endif
  drawqueue();
  }

#include "menus.cpp"

void drawscreen() {

  DEBB(DF_GRAPH, (debugfile,"drawscreen\n"));

  calcparam();
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
  if(!vid.usingGL) SDL_FillRect(s, NULL, 0);
  #endif
  
  if(!canmove) darken = 1;
  if(cmode != emNormal && cmode != emDraw && cmode != emCustomizeChar) darken = 2;
  if(cmode == emQuit && !canmove) darken = 0;
  if(cmode == emOverview) darken = 4;

#ifdef MOBILE
  if(cmode == emQuit) darken = 1;
#endif

#ifndef MOBILE
  if(cmode == emMapEditor && !mapeditor::subscreen && !mapeditor::choosefile) darken = 0;
  if(cmode == emDraw && mapeditor::choosefile) darken = 2;
#endif
  if(hiliteclick && darken == 0 && vid.monmode == 2) darken = 1;
  if(cmode == emProgress) darken = 0;

  if(conformal::includeHistory && cmode != emProgress) conformal::restore();
  
  if(rug::rugged) {
#ifndef MOBILE
    rug::actDraw();
#endif
    }
  else drawfullmap();

  if(conformal::includeHistory && cmode != emProgress) conformal::restoreBack();
  
  getcstat = 0;
  
  if(cmode == emNormal || cmode == emQuit) drawStats();

  #ifdef MOBILE
  
  buttonclicked = false;
  
  if(cmode == emNormal) {
    displayabutton(-1, -1, XLAT("MOVE"),  andmode == 0 ? BTON : BTOFF);
    displayabutton(+1, -1, XLAT(andmode == 1 ? "BACK" : "DRAG"),  andmode == 1 ? BTON : BTOFF);
    displayabutton(-1, +1, XLAT("INFO"),  andmode == 2 ? BTON : BTOFF);
    displayabutton(+1, +1, XLAT("MENU"), andmode == 3 ? BTON : BTOFF);
    }
  
  if(cmode == emQuit) {
    displayabutton(-1, +1, XLAT("NEW"), BTON);
    displayabutton(+1, -1, XLAT(canmove ? "PLAY" : ISIOS ? " " : "SHARE"), BTON);
    displayabutton(+1, +1, XLAT("MENU"), BTON);
    }
  
  #endif
 
  // displaynum(vx,100, 0, 24, 0xc0c0c0, celldist(cwt.c), ":");

  darken = 0;
  drawmessages();
  
  if(cmode == emNormal) {
    #ifdef MOBILE
    if(!canmove) cmode = emQuit;
    #endif
    if(!canmove) showGameover();
    #ifndef MOBILE
//  if(!canmove)
//    displayButton(vid.xres-8, vid.yres-vid.fsize*2, XLAT("ESC for menu/quest"), SDLK_ESCAPE, 16);
    #endif
    }
  
  #ifndef ANDROID
  if(cmode == emProgress) mouseovers = "";
  #endif
  
  displayMenus();
  
  describeMouseover();

  if(havebugs && darken == 0) for(int k=0; k<3; k++)
    displayfr(vid.xres/2 + vid.fsize * 5 * (k-1), vid.fsize*2,   2, vid.fsize, 
      its(hive::bugcount[k]), minf[moBug0+k].color, 8);
    
  bool minefieldNearby = false;
  int mines[4], tmines=0;
  for(int p=0; p<numplayers(); p++) {
    mines[p] = 0;
    cell *c = playerpos(p);
    for(int i=0; i<c->type; i++) if(c->mov[i]) {
      if(c->mov[i]->land == laMinefield) 
        minefieldNearby = true;
      if(c->mov[i]->wall == waMineMine) {
        bool ep = false;
        if(!ep) mines[p]++, tmines++;
        }
      }
    }

  if((minefieldNearby || tmines) && canmove && !items[itOrbGhost] && darken == 0) {
    string s;
    if(tmines > 7) tmines = 7;
    int col = minecolors[tmines];
    
    if(tmines == 7) seenSevenMines = true;
    
    for(int p=0; p<numplayers(); p++)      
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
  if(cmode == emNormal || cmode == emVisual1 || cmode == emVisual2 || cmode == emChangeMode )
    displayButton(vid.xres-8, vid.yres-vid.fsize, XLAT("(v) menu"), 'v', 16);
  #endif
  
  if(cmode == emQuit) {
    if(canmove || ISMOBILE) showGameover();
    }

#ifndef ANDROID
  if(cmode == emHelp) {
    int last = 0;
    int lastspace = 0;
    int siz = vid.fsize;
    
    if(help == "@") help = buildHelpText();

    if(ISIOS && size(help) >= 500) siz = (siz * 9)/10;
    else if(ISIOS) siz = (siz * 3+1)/2;
    else if(size(help) >= 500) siz = siz * 2/3;
    
    int vy = vid.fsize * 4;
    
    int xs = vid.xres * 618/1000;
    int xo = vid.xres * 186/1000;
    
    for(int i=0; i<=size(help); i++) {
      int ls = 0;
      int prev = last;
      if(help[i] == ' ') lastspace = i;
      if(textwidth(siz, help.substr(last, i-last)) > xs) {
        if(lastspace == last) ls = i-1, last = i-1;
        else ls = lastspace, last = ls+1;
        }
      if(help[i] == 10 || i == size(help)) ls = i, last = i+1;
      if(ls) {
        displayfr(xo, vy, 2, siz, help.substr(prev, ls-prev), 0xC0C0C0, 0);
        if(ls == prev) vy += siz/2;
        else vy += siz;
        lastspace = last;
        }
      }
    }
#endif

#ifndef MOBILE  
  // DEB
  if(mouseover && targetclick) {
    shmup::cpid = 0;
    eItem i = targetRangedOrb(mouseover, roCheck);
    if(i == itOrbSummon) {
      eMonster m = summonedAt(mouseover);
      displaychr(mousex, mousey, 0, vid.fsize, minf[m].glyph, minf[m].color);
      }
    else if(i)
      displaychr(mousex, mousey, 0, vid.fsize, '@', iinf[i].color);
    }
#endif
  
  #ifndef MOBILE
  // SDL_UnlockSurface(s);

//profile("swapbuffers");
#ifdef GL
  if(vid.usingGL) SDL_GL_SwapBuffers(); else
#endif
  SDL_UpdateRect(s, 0, 0, vid.xres, vid.yres);
  
//printf("\ec");

  #endif
  
//profile("centerpc");
  if(playermoved && vid.aspeed > 4.99 && !shmup::on) { 
    centerpc(1000);
    playermoved = false; 
    return; 
    }

  }

#ifndef MOBILE
bool setfsize = false;

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
  if(euclid) {
    centerover = euclideanAtCreate(0,0);
    }
  else {
    viewctr.h = &origin;
    viewctr.spin = 0;
    }
  View = Id;
  webdisplay = 0;
  }

void initcs(charstyle &cs) {
  cs.charid     = 0;
  cs.skincolor  = 0xD0D0D0FF;
  cs.haircolor  = 0x686868FF;
  cs.dresscolor = 0xC00000FF;
  cs.swordcolor = 0xD0D0D0FF;
  cs.dresscolor2= 0x8080FFC0;
  }

#ifndef ANDROID

void savecs(FILE *f, charstyle& cs) {
  int gflags = cs.charid;
  if(vid.samegender) gflags |= 16;
  
  fprintf(f, "%d %d %08x %08x %08x %08x", 
    gflags, vid.language, cs.skincolor, cs.haircolor, cs.swordcolor, cs.dresscolor);
  if(cs.charid == 3) fprintf(f, " %08x", cs.dresscolor2);
  fprintf(f, "\n");
  }

void loadcs(FILE *f, charstyle& cs) {
  int gflags, err = 
  fscanf(f, "%d%d%x%x%x%x", &gflags, &vid.language, &cs.skincolor, &cs.haircolor, &cs.swordcolor, &cs.dresscolor);

  if(err) cs.charid = gflags & 15;
  if(err) vid.samegender = (gflags & 16) ? true : false;
  if(cs.charid == 3) if(fscanf(f, "%x", &cs.dresscolor2)) 
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
  fprintf(f, "%f %f %f %f\n", float(vid.scale), float(vid.eye), float(vid.alpha), float(vid.aspeed));
  fprintf(f, "%d %d %d %d %d %d %d\n", vid.wallmode, vid.monmode, vid.axes, audiovolume, vid.framelimit, vid.usingGL, vid.usingAA);
  fprintf(f, "%d %d %d %lf %d %d\n", vid.joyvalue, vid.joyvalue2, vid.joypanthreshold, vid.joypanspeed, autojoy, vid.flashtime);
  
  savecs(f, vid.cs);
  
  fprintf(f, "%d %d\n", vid.darkhepta, vid.shifttarget);
  
  fprintf(f, "%d %d %d %d\n", euclid, euclidland, shmup::on, hardcore);

  shmup::saveConfig(f);

  fprintf(f, "%d %d %d %d %f %d %d\n", rug::renderonce, rug::rendernogl, rug::texturesize, purehepta, rug::scale, vid.steamscore, chaosmode);

  fprintf(f, "%d %d %lf %d %d %lf\n",
    pmodel, polygonal::SI, polygonal::STAR, polygonal::deg, 
    conformal::includeHistory, conformal::lvspeed);
  
  fprintf(f, "%d %d %d %d %d %d\n", 
    conformal::bandhalf, conformal::bandsegment, 
    conformal::rotation, conformal::autoband, conformal::autobandhistory,
    conformal::dospiral);
  
  fprintf(f, "%d", polygonal::maxcoef);
  for(int i=0; i<=polygonal::maxcoef; i++) fprintf(f, " %lf %lf", 
    (double) real(polygonal::coef[i]), (double) imag(polygonal::coef[i]));
    
  fprintf(f, "\n\nThis is a configuration file for HyperRogue (version " VER ")\n");
  fprintf(f, "\n\nThe numbers are:\n");
  fprintf(f, "screen width & height, fullscreen mode (0=windowed, 1=fullscreen), font size\n");
  fprintf(f, "scale, eye distance, parameter, animation speed\n");
  fprintf(f, "wallmode, monster mode, cross mode, audiovolume, framerate limit, usingGL, usingAA\n");
  fprintf(f, "calibrate first joystick (threshold A, threshold B), calibrate second joystick (pan threshold, pan speed), joy mode\n");
  fprintf(f, "gender (1=female, 16=same gender prince), language, skin color, hair color, sword color, dress color\n");
  fprintf(f, "darken hepta, shift target\n");
  fprintf(f, "euclid, euclid land, shmup, hardcore\n");
  fprintf(f, "version number, shmup players, shmup keyboard/joystick config\n");
  fprintf(f, "hypersian rug config: renderonce, rendernogl, texturesize; purehepta; rug scale; share score; chaosmode\n");
  fprintf(f, "conformal: model, sides, star, degree, includeHistory, speed\n");
  fprintf(f, "conformal: bandwidth, segment, rotation, autoband, autohistory, dospiral\n");
  fprintf(f, "conformal: degree, (degree+1) times {real, imag}\n");
  
  fclose(f);
#ifndef MOBILE
  addMessage(s0 + "Configuration saved to: " + conffile);
#else
  addMessage(s0 + "Configuration saved");
#endif
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
      vid.scale = a; vid.eye = b; vid.alpha = c; vid.aspeed = d;
      }
    err=fscanf(f, "%d%d%d%d%d%d%d", &vid.wallmode, &vid.monmode, &vid.axes, &audiovolume, &vid.framelimit, &gl, &aa);
    vid.usingGL = gl; vid.usingAA = aa;
    err=fscanf(f, "%d%d%d%f%d%d", &vid.joyvalue, &vid.joyvalue2, &vid.joypanthreshold, &vid.joypanspeed, &aa, &vid.flashtime);
    autojoy = aa; aa = 0;

    loadcs(f, vid.cs);

    aa=0; bb=0;
    err=fscanf(f, "%d%d", &aa, &bb);
    vid.darkhepta = aa; vid.shifttarget = bb;

    aa = euclid; bb = euclidland; cc = shmup::on; dd = hardcore;
    err=fscanf(f, "%d%d%d%d", &aa, &bb, &cc, &dd);
    euclid = aa; euclidland = eLand(bb); shmup::on = cc; hardcore = dd;

    shmup::loadConfig(f);

    aa = rug::renderonce; bb = rug::rendernogl; cc = purehepta; dd = chaosmode; ee = vid.steamscore;
    err=fscanf(f, "%d%d%d%d%lf%d%d", &aa, &bb, &rug::texturesize, &cc, &rug::scale, &ee, &dd);
    rug::renderonce = aa; rug::rendernogl = bb; purehepta = cc; chaosmode = dd; vid.steamscore = ee;

    aa=conformal::includeHistory;
    err=fscanf(f, "%d%d%lf%d%d%lf",
      &pmodel, &polygonal::SI, &polygonal::STAR, &polygonal::deg,
      &aa, &conformal::lvspeed);
    conformal::includeHistory = aa;
    
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
  
    fclose(f);
    DEBB(DF_INIT, (debugfile,"Loaded configuration: %s\n", conffile));
    }

#ifndef MOBILE
  if(clWidth) vid.xres = clWidth;
  if(clHeight) vid.yres = clHeight;
  if(clFont) vid.fsize = clFont;
  
  for(int k=0; k<int(commandline.size()); k++) switch(commandline[k]) {
    case 'f': vid.full = true; break;
    case 'w': vid.full = false; break;
    case 'e': vid.wallmode = 3; vid.monmode = 2; break;
    case 'p': vid.wallmode = 2; vid.monmode = 2; break;
    case 'a': vid.wallmode = 0; vid.monmode = 0; break;
    case 'o': vid.usingGL = !vid.usingGL; break;
    case char(200): vid.usingGL = false; break;
    case char(201): vid.usingGL = true; break;
    case 'O': vid.usingGL = false; break;
    case 'E': euclid = !euclid; if(euclid) euclidland = firstland; break;
    case 'S': shmup::on = !shmup::on; break;
    case 'P': k++; vid.scfg.players = commandline[k]-'0'; break;
    case 'H': hardcore = !hardcore; break;
    case '7': purehepta = !purehepta; break;
    case 'C': chaosmode = !chaosmode; break;
    case 'T': tactic::on = !tactic::on; break;
    case 'R': randomPatternsMode = !randomPatternsMode; break;
    case 'D':
      randomPatternsMode = false;
      tactic::on = false;
      hardcore = false;
      vid.scfg.players = 1;
      euclid = false;
      shmup::on = false;
      break;
    }
#endif
  precalc();
  }
#endif

#ifndef MOBILE
string musfname[landtypes];

bool loadMusicInfo(string dir) {
  DEBB(DF_INIT, (debugfile,"load music info\n"));
  if(dir == "") return false;
  FILE *f = fopen(dir.c_str(), "rt");
  if(f) {
    string dir2;
    for(int i=0; i<size(dir); i++) if(dir[i] == '/' || dir[i] == '\\')
      dir2 = dir.substr(0, i+1);
    char buf[1000];
    while(fgets(buf, 800, f) > 0) {
      for(int i=0; buf[i]; i++) if(buf[i] == 10 || buf[i] == 13) buf[i] = 0;
      if(buf[0] == '[' && buf[3] == ']') {
        int id = (buf[1] - '0') * 10 + buf[2] - '0';
        if(id >= 0 && id < landtypes) {
          if(buf[5] == '*' && buf[6] == '/') musfname[id] = dir2 + (buf+7);
          else musfname[id] = buf+5;
          }
        else {
          fprintf(stderr, "warning: bad soundtrack id, use the following format:\n");
          fprintf(stderr, "[##] */filename\n");
          fprintf(stderr, "where ## are two digits, and */ is optional and replaced by path to the music\n");
          fprintf(stderr, "alternatively LAST = reuse the last track instead of having a special one");
          }
        // printf("[%2d] %s\n", id, buf);
        }
      else if(buf[0] == '#') {
        }
      else {
        musiclicense += buf;
        musiclicense += "\n";
        }
      }
    fclose(f);
    return true;
    }
  return false;
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
  vid.aspeed = 0;
  vid.eye = 0;
  vid.full = false;
  vid.quick = true;
  vid.wallmode = 3;

  vid.joyvalue = 4800;
  vid.joyvalue2 = 5600;
  vid.joypanthreshold = 2500;
#ifdef PANDORA
  vid.joypanspeed = 0.0001;
#else
  vid.joypanspeed = 0;
#endif

  vid.framelimit = 75;
  vid.monmode = 2;
  vid.axes = 1;
  vid.steamscore = true;
  
  initcs(vid.cs);
  
  vid.killreduction = 0;
  
  vid.samegender = false;
  vid.language = -1;
  
  joyx = joyy = 0; joydir.d = -1;
  
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
  const SDL_VideoInfo *inf = SDL_GetVideoInfo();
  vid.xscr = vid.xres = inf->current_w;
  vid.yscr = vid.yres = inf->current_h;
  
  SDL_WM_SetCaption("HyperRogue " VER, "HyperRogue " VER);
  
  loadConfig();

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

  #ifdef AUDIO

  audio = 
    loadMusicInfo(musicfile)
    || loadMusicInfo("./hyperrogue-music.txt") 
    || loadMusicInfo("music/hyperrogue-music.txt")
// Destination set by ./configure (in the GitHub repository)
#ifdef MUSICDESTDIR
    || loadMusicInfo(MUSICDESTDIR)
#endif
#ifdef FHS
    || loadMusicInfo("/usr/share/hyperrogue/hyperrogue-music.txt") 
    || loadMusicInfo(s0 + getenv("HOME") + "/.hyperrogue-music.txt")
#endif
    ;

  if(audio) {
    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) != 0) {
      fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
      audio = false;
      }
    else {
      audio = true;
      Mix_AllocateChannels(4);
      }
    }
  #endif
    
  #endif
  }

int frames;
bool outoffocus = false;

#ifdef AUDIO

bool loaded[landtypes];
Mix_Music* music[landtypes];
int musicpos[landtypes];
int musstart;
int musfadeval = 2000;

eLand cid = laNone;

void handlemusic() {
  DEBB(DF_GRAPH, (debugfile,"handle music\n"));
  if(audio && audiovolume) {
    eLand id = cwt.c->land;
    if(isHaunted(id)) id = laHaunted;
    if(id == laGridSea) id = laGridCoast;
#ifdef LOCAL
    extern bool local_changemusic(eLand& id);
    if(local_changemusic(id)) return;
#endif
    if(outoffocus) id = eLand(0);
    if(musfname[id] == "LAST") id = cid;
    if(!loaded[id]) {
      loaded[id] = true;
      // printf("loading (%d)> %s\n", id, musfname[id].c_str());
      if(musfname[id] != "") {
        music[id] = Mix_LoadMUS(musfname[id].c_str());
        if(!music[id]) {
           printf("Mix_LoadMUS: %s\n", Mix_GetError());
           }
        }
      }
    if(cid != id && !musfadeval) {
      musicpos[cid] = SDL_GetTicks() - musstart;
      musfadeval = musicpos[id] ? 500 : 2000;
      Mix_FadeOutMusic(musfadeval);
      // printf("fadeout %d, pos %d\n", musfadeval, musicpos[cid]);
      }
    if(music[id] && !Mix_PlayingMusic()) {
      cid = id;
      Mix_VolumeMusic(audiovolume);
      Mix_FadeInMusicPos(music[id], -1, musfadeval, musicpos[id] / 1000.0);
      // printf("fadein %d, pos %d\n", musfadeval, musicpos[cid]);
      musstart = SDL_GetTicks() - musicpos[id];
      musicpos[id] = 0;
      musfadeval = 0;
      }
    }
  }


void resetmusic() {
  if(audio && audiovolume) {
    Mix_FadeOutMusic(3000);
    cid = laNone;
    for(int i=0; i<landtypes; i++) musicpos[i] = 0;
    musfadeval = 2000;
    }
  }

#else
void resetmusic() {}
#endif

void panning(hyperpoint hf, hyperpoint ht) {
  View = 
    rgpushxto0(hf) * rgpushxto0(gpushxto0(hf) * ht) * gpushxto0(hf) * View;
  playermoved = false;
  }

#ifdef LOCAL
#include "local.cpp"
#endif

void resetview() {
  DEBB(DF_GRAPH, (debugfile,"reset view\n"));
  View = Id;
  // EUCLIDEAN
  if(!euclid) viewctr.h = cwt.c->master;
  else centerover = cwt.c;
  // SDL_LockSurface(s);
  // SDL_UnlockSurface(s);
  }

void fullcenter() {
  if(playerfound && false) centerpc(INF);
  else {
    resetview();
    drawthemap(); 
    centerpc(INF);
    }
  playermoved = true;
  }

#ifndef MOBILE

// Warning: a very long function! todo: refactor

void mainloop() {
  int lastt = 0;
  cmode = emNormal;
  while(true) {

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

    if(shmup::on && cmode == emNormal) 
      timetowait = 0, shmup::turn(ticks - lastt);

    if(timetowait > 0)
      SDL_Delay(timetowait);
    else {
      if(playermoved && vid.aspeed > -4.99 && !outoffocus)
        centerpc((ticks - lastt) / 1000.0 * exp(vid.aspeed));
      if(panjoyx || panjoyy) 
        checkpanjoy((ticks - lastt) / 1000.0);
      tortoise::updateVals(ticks - lastt);
      lastt = ticks;
      frames++;
      if(!outoffocus) {
        drawscreen();
        }
      }      

    Uint8 *keystate = SDL_GetKeyState(NULL);
    rightclick = keystate[SDLK_RCTRL];
    leftclick = keystate[SDLK_RSHIFT];
    hiliteclick = keystate[SDLK_LALT] | keystate[SDLK_RALT];
    anyshiftclick = keystate[SDLK_LSHIFT] | keystate[SDLK_RSHIFT];
    
    bool didsomething = false;
    
    if(vid.shifttarget) {
      leftclick = false;
      targetclick = keystate[SDLK_RSHIFT] | keystate[SDLK_LSHIFT];
      }
    else {
      leftclick = keystate[SDLK_RSHIFT];
      targetclick = true;
      }
    
#ifdef AUDIO
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
        flashMessages();
        if(ev.jaxis.value != 0)
        printf("which = %d axis = %d value = %d\n",
          ev.jaxis.which,
          ev.jaxis.axis,
          ev.jaxis.value);
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

      else if(ev.type == SDL_JOYBUTTONDOWN && !shmup::on) {
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

      if(ev.type == SDL_MOUSEBUTTONDOWN) {
        flashMessages();
        mousepressed = true;
        mousing = true;
        if(ev.button.button==SDL_BUTTON_RIGHT || leftclick) 
          sym = SDLK_F1;
        else if(ev.button.button==SDL_BUTTON_MIDDLE || rightclick) 
          sym = 1, didsomething = true;
        else if(ev.button.button==SDL_BUTTON_WHEELUP && ((cmode == emQuit) ^ !canmove)) {  
          sym = 1; msgscroll++; didsomething = true;
          }
        else if(ev.button.button==SDL_BUTTON_WHEELDOWN && ((cmode == emQuit) ^ !canmove)) {  
          sym = 1; msgscroll--; didsomething = true;
          }
        else if(ev.button.button==SDL_BUTTON_WHEELUP) {  
          ld jx = (mousex - vid.xcenter - .0) / vid.radius / 10;
          ld jy = (mousey - vid.ycenter - .0) / vid.radius / 10;
          playermoved = false;
          View = gpushxto0(hpxy(jx, jy)) * View;
          sym = 1;
          }
        else {
          sym = getcstat, uni = getcstat, shiftmul = getcshift;
          }
        }

      if(ev.type == SDL_MOUSEBUTTONUP) 
        mousepressed = false;

      if(((cmode == emNormal && canmove) || (cmode == emQuit && !canmove) || cmode == emDraw || cmode == emMapEditor) && !shmup::on && !rug::rugged) {
#ifndef PANDORA
        if(sym == SDLK_RIGHT) 
          View = xpush(-0.2*shiftmul) * View, playermoved = false, didsomething = true;
        if(sym == SDLK_LEFT) 
          View = xpush(+0.2*shiftmul) * View, playermoved = false, didsomething = true;
        if(sym == SDLK_UP) 
          View = ypush(+0.2*shiftmul) * View, playermoved = false, didsomething = true;
        if(sym == SDLK_DOWN) 
          View = ypush(-0.2*shiftmul) * View, playermoved = false, didsomething = true;
#endif
        if(sym == SDLK_PAGEUP) {
          View = spin(M_PI/21*shiftmul) * View, didsomething = true;
          }
        if(sym == SDLK_PAGEDOWN) 
          View = spin(-M_PI/21*shiftmul) * View, didsomething = true;
        
        if(sym == SDLK_PAGEUP || sym == SDLK_PAGEDOWN) 
          if(isGravityLand(cwt.c->land)) playermoved = false;
        }
      
      if(ev.type == SDL_MOUSEMOTION) {
        hyperpoint mouseoh = mouseh;
        
        mousing = true;
        mousex = ev.motion.x;
        mousey = ev.motion.y;
        if(rug::rugged)
          mouseh = rug::gethyper(mousex, mousey);
        else
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
        }

      if(sym == SDLK_F7 && !vid.usingGL) {

        time_t timer;
        timer = time(NULL);
        char buf[128]; strftime(buf, 128, "shot-%y%m%d-%H%M%S" IMAGEEXT, localtime(&timer));

        IMAGESAVE(s, buf);
        addMessage(XLAT("Screenshot saved to %1", buf));
        }

      if(cmode == emNormal) {

        if(cheater) {
          if(applyCheat(uni, mouseover))
            sym = 0;
          }
      
        if(!(uni >= 'A' && uni <= 'Z') && !shmup::on) {
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
        if(!shmup::on) {
          if(sym == SDLK_RIGHT) movepckeydir(0);
          if(sym == SDLK_LEFT) movepckeydir(4);
          if(sym == SDLK_DOWN) movepckeydir(2 + (leftclick?1:0) - (rightclick?1:0));
          if(sym == SDLK_UP) movepckeydir(6 - (leftclick?1:0) + (rightclick?1:0));
          }
#endif

        if(!shmup::on) {
          if(sym == '.' || sym == 's') movepcto(-1, 1);
          if((sym == SDLK_DELETE || sym == SDLK_KP_PERIOD || sym == 'g') && uni != 'G' && uni != 'G'-64) 
            movepcto(-2, 1);
          if(sym == 't' && uni != 'T' && uni != 'T'-64 && canmove && cmode == emNormal) {
            if(playermoved && items[itStrongWind]) {
              cell *c = whirlwind::jumpDestination(cwt.c);
              if(c) centerover = c;
              }
            targetRangedOrb(centerover, roKeyboard);
            sym = 0; uni = 0;
            }
          }

        if(sym == SDLK_KP5 && !shmup::on) movepcto(-1, 1);

        // if(sym == SDLK_F4)  restartGameSwitchEuclid();

        if(sym == SDLK_F5)  restartGame();
        if(sym == SDLK_ESCAPE) {
          cmode = emQuit;
          achievement_final(false);
          if(!canmove) {
            addMessage(XLAT("GAME OVER"));
            addMessage(timeline());
            }
          msgscroll = 0;
          }
        if(sym == SDLK_F10) return;
        
        if(!canmove) {
          if(sym == SDLK_RETURN) return;
          else if(uni == 'r') restartGame();
          else if(uni == 't') {
            restartGame();
            loadScores();
            }
          else if(rug::rugged) ;
          else if(sym == SDLK_UP || sym == SDLK_KP8) msgscroll++;
          else if(sym == SDLK_DOWN || sym == SDLK_KP2) msgscroll--;
          else if(sym == SDLK_PAGEUP || sym == SDLK_KP9) msgscroll+=5;
          else if(sym == SDLK_PAGEDOWN || sym == SDLK_KP3) msgscroll-=5;
          }
        
        if(uni == 'o') setAppropriateOverview();
        
        if(sym == SDLK_HOME || sym == SDLK_F3 || (sym == ' ' && !shmup::on)) 
          fullcenter();
        
/*      if(sym == SDLK_F6) {
          View = spin(M_PI/2) * inverse(cwtV) * View;
          if(flipplayer) View = spin(M_PI) * View;
          cmode = emDraw;
          } */

        if(sym == 'v') {
          cmode = emMenu;
          }

        if(sym == SDLK_F2) {
          cmode = emVisual1;
          }

#ifdef PANDORA
        if(ev.type == SDL_MOUSEBUTTONUP && sym == 0 && !rightclick) 
#else
        if(ev.type == SDL_MOUSEBUTTONDOWN && sym == 0 && !rightclick) 
#endif
          {
          bool forcetarget = (keystate[SDLK_RSHIFT] | keystate[SDLK_LSHIFT]);
          
          shmup::cpid = 0;
          if(mouseover && targetclick && (numplayers() == 1) && targetRangedOrb(mouseover, forcetarget ? roMouseForce : roMouse))
            ;
          else if(forcetarget)
            ;
          else if(shmup::on)
            ;
          else mousemovement();
          }
        
        if(sym == SDLK_F1) {
          lastmode = cmode;
          cmode = emHelp;
          }

#ifdef LOCAL
        process_local0(sym);
#endif
        }

      else if(cmode == emMenu) { if(handleMenuKey(sym, ev.type == SDL_MOUSEBUTTONDOWN)) return; }
      else if(cmode == emCheatMenu) handleCheatMenu(uni);
      else if(cmode == emVisual1) handleVisual1(sym, uni);
      else if(cmode == emCustomizeChar) handleCustomizeChar(sym, uni, ev.key.keysym.mod);
      else if(cmode == emVisual2) handleVisual2(sym, uni);
      else if(cmode == emChangeMode) handleChangeMode(sym, uni);
      else if(cmode == emShmupConfig) shmup::handleConfig(uni, sym);
      else if(cmode == emNetgen) netgen::handleKey(uni, sym);
      else if(cmode == emRugConfig) rug::handleKey(uni, sym);
      else if(cmode == emConformal) conformal::handleKey(uni, sym);
      else if(cmode == emYendor) yendor::handleKey(uni, sym);
      else if(cmode == emTactic) tactic::handleKey(uni, sym);
      else if(cmode == emMapEditor) mapeditor::handleKey(uni, sym);
      else if(cmode == emOverview) handleOverview(uni);
      else if(cmode == emDraw) mapeditor::drawHandleKey(uni, sym, shiftmul);
      else if(cmode == emPickEuclidean) handleEuclidean(sym, uni);
      else if(cmode == emScores) handleScoreKeys(sym, ev);
      else if(cmode == emPickScores) handlePickScoreKeys(uni, ev);
      
      else if(cmode == emHelp) {
        if(sym == SDLK_F1 && help != "@") 
          help = "@";
        else if(uni == 'c')
          help = buildCredits();
        else if((sym != 0 && sym != SDLK_F12) || ev.type == SDL_MOUSEBUTTONDOWN)
          cmode = lastmode;
        }
        
      else if(cmode == emQuit) {
        // ignore the camera movement keys
        if(rug::rugged && (sym == SDLK_UP || sym == SDLK_DOWN || sym == SDLK_PAGEUP || sym == SDLK_PAGEDOWN ||
          sym == SDLK_RIGHT || sym == SDLK_LEFT))
          sym = 0;

        if(sym == SDLK_RETURN || sym == SDLK_F10) return;
        else if(uni == 'r' || sym == SDLK_F5) {
          restartGame(), cmode = emNormal;
          msgs.clear();
          }
        else if(sym == SDLK_UP || sym == SDLK_KP8) msgscroll++;
        else if(sym == SDLK_DOWN || sym == SDLK_KP2) msgscroll--;
        else if(sym == SDLK_PAGEUP || sym == SDLK_KP9) msgscroll+=5;
        else if(sym == SDLK_PAGEDOWN || sym == SDLK_KP3) msgscroll-=5;
        else if(uni == 'v') cmode = emMenu;
        else if(sym == SDLK_HOME || sym == SDLK_F3 || (sym == ' ' && !shmup::on)) 
          fullcenter();
        else if(uni == 'o') setAppropriateOverview();
        else if(uni == 't') {
          if(!canmove) restartGame();
          loadScores();
          msgs.clear();
          }
        
        else if(((sym != 0 && sym != SDLK_F12) || ev.type == SDL_MOUSEBUTTONDOWN) && !didsomething) {
          cmode = emNormal;
          msgscroll = 0;
          msgs.clear();
          }
        }
      
      if(ev.type == SDL_QUIT)
        return;

      }
    
    if(playerdead) break;
    }
  
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
#ifndef MOBILE
  if(mapeditor::painttype == 4) 
    mapeditor::painttype = 0, mapeditor::paintwhat = 0,
    mapeditor::paintwhat_str = "clear monster";
  mapeditor::copywhat = NULL;
  mapeditor::undo.clear();
  if(!cheater) mapeditor::displaycodes = 0;
  if(!cheater) mapeditor::whichShape = 0;
#endif
  }

void showMissionScreen() {
  cmode = emQuit;
  msgscroll = 0;
  }
