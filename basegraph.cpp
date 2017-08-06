unsigned backcolor = 0;
unsigned bordcolor = 0;
unsigned forecolor = 0xFFFFFF;

int utfsize(char c) {
  unsigned char cu = c;
  if(cu < 128) return 1;
  if(cu < 224) return 2;
  if(cu < 0xF0) return 3;
  return 4;
  }

bool eqs(const char* x, const char* y) {
  return *y? *x==*y?eqs(x+1,y+1):false:true;
  }

int getnext(const char* s, int& i) {

  int siz = utfsize(s[i]);
// if(fontdeb) printf("s=%s i=%d siz=%d\n", s, i, siz);
  if(siz == 1) return s[i++];
  for(int k=0; k<NUMEXTRA; k++)
    if(eqs(s+i, natchars[k])) {
      i += siz; return 128+k;
      }
  printf("Unknown character in: '%s'\n", s);
  i ++; return '?';
  }

#if CAP_SDLTTF
TTF_Font *font[256];
#endif

#if CAP_SDL
SDL_Surface *s;

int ZZ;

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
#endif

#if CAP_SDLTTF
void loadfont(int siz) {
  if(!font[siz]) {
    font[siz] = TTF_OpenFont(ISWEB ? "sans-serif" : HYPERPATH "DejaVuSans-Bold.ttf", siz);
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
#endif

#if !ISFAKEMOBILE && !ISANDROID & !ISIOS
int textwidth(int siz, const string &str) {
  if(size(str) == 0) return 0;

#if CAP_SDLTTF
  loadfont(siz);
  
  int w, h;
  TTF_SizeUTF8(font[siz], str.c_str(), &w, &h);
  // printf("width = %d [%d]\n", w, size(str));
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

int darkenedby(int c, int lev) {
  for(int i=0; i<lev; i++)
    c = ((c & 0xFEFEFE) >> 1);
  return c;
  }

int darkened(int c) {
#ifdef EXTRA_FADEOUT
  c = gradient(backcolor, c, 0, extra::fadeout, 1);
#endif
  if(inmirrorcount&1)
    c = gradient(c, winf[waMirror].color, 0, 0.5, 1);
  else if(inmirrorcount)
    c = gradient(c, winf[waCloud].color, 0, 0.5, 1);
  for(int i=0; i<darken; i++)
    c = ((c & 0xFEFEFE) >> 1) + ((backcolor & 0xFEFEFE) >> 1);
  return c;
  }

int darkena(int c, int lev, int a) {
  return (darkenedby(c, lev) << 8) + a;
  }

#if !CAP_GL
void setcameraangle(bool b) { }
#else

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
  if(vid.antialias & AA_LINES) {
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    }
  else glDisable(GL_LINE_SMOOTH);
  glLineWidth(vid.linewidth);

#if !ISMOBILE
  if(vid.antialias & AA_POLY) {
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    }
  else glDisable(GL_POLYGON_SMOOTH);
#endif

  //glLineWidth(1.0f);
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

#if CAP_GLFONT

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

void sdltogl(SDL_Surface *txt, glfont_t& f, int ch) {
#if CAP_TABFONT
  int otwidth, otheight, tpix[3000], tpixindex = 0;
  loadCompressedChar(otwidth, otheight, tpix);
#else
  int otwidth = txt->w;
  int otheight = txt->h;
#endif
  
  int twidth = next_p2( otwidth );
  int theight = next_p2( otheight );

#if CAP_TABFONT
  int expanded_data[twidth * theight];
#else
  Uint16 expanded_data[twidth * theight];
#endif

  for(int j=0; j <theight;j++) for(int i=0; i < twidth; i++) {
#if CAP_TABFONT
    expanded_data[(i+j*twidth)] = (i>=otwidth || j>=otheight) ? 0 : tpix[tpixindex++];
#else
    expanded_data[(i+j*twidth)] = 
        ((i>=txt->w || j>=txt->h) ? 0 : ((qpixel(txt, i, j)>>24)&0xFF) * 0x100) | 0x00FF;
#endif
    }
  
  f.widths[ch] = otwidth;
  f.heights[ch] = otheight;

  glBindTexture( GL_TEXTURE_2D, f.textures[ch]);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
 
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, twidth, theight, 0,
#if CAP_TABFONT
    GL_RGBA, GL_UNSIGNED_BYTE, 
#else
    GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 
#endif
    expanded_data );
 
  float x=(float)otwidth / (float)twidth;
  float y=(float)otheight / (float)theight;
  f.tx[ch] = x;
  f.ty[ch] = y;
  }
  
void init_glfont(int size) {
  if(glfont[size]) return;
  DEBB(DF_INIT, (debugfile,"init GL font: %d\n", size));

#if !CAP_TABFONT
  loadfont(size);
  if(!font[size]) return;
#endif
  
  glfont[size] = new glfont_t;
  
  glfont_t& f(*(glfont[size]));

  f.textures = new GLuint[128+NUMEXTRA];
//f.list_base = glGenLists(128);
  glGenTextures( 128+NUMEXTRA, f.textures );
  if(0) for(int i=0; i<128+NUMEXTRA; i++)
    printf("texture %d = %d\n", i, f.textures[i]);

#if !CAP_TABFONT
  char str[2]; str[1] = 0;
  
  SDL_Color white;
  white.r = white.g = white.b = 255;
#endif
  
//  glListBase(0);
 
  for(int ch=1;ch<128+NUMEXTRA;ch++) {
  
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

//printf("init size=%d ok\n", size);
  GLERR("initfont");
  }

GLfloat tver[24];

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

bool gl_print(int x, int y, int shift, int size, const char *s, int color, int align) {

  int gsiz = size;
  if(size > vid.fsize || size > 72) gsiz = 72;

#if CAP_FIXEDSIZE
  gsiz = CAP_FIXEDSIZE;
#endif
  
  init_glfont(gsiz);
  if(!glfont[gsiz]) return false;

  glfont_t& f(*glfont[gsiz]);
  
  glDisable(GL_LIGHTING);
  
  glEnable(GL_TEXTURE_2D);

  glMatrixMode(GL_MODELVIEW);
  
  glcolor2((color << 8) | 0xFF);

  int tsize = 0;
  
  for(int i=0; s[i];) {
    tsize += f.widths[getnext(s,i)] * size/gsiz;
    }
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
  }
#endif

purehookset hooks_resetGL;

void resetGL() {
  DEBB(DF_INIT, (debugfile,"reset GL\n"));
  callhooks(hooks_resetGL);
#if CAP_GLFONT
  for(int i=0; i<128; i++) if(glfont[i]) {
    delete glfont[i];
    glfont[i] = NULL;
    }
#endif
  buildpolys();
  }

#endif

#if CAP_XGD

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
  gdpush((int) s.size()); for(int i=0; i<size(s); i++) {
#if ISANDROID
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

#else
bool displaystr(int x, int y, int shift, int size, const char *str, int color, int align) {

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
#endif
  }
                  
bool displaystr(int x, int y, int shift, int size, const string &s, int color, int align) {
  return displaystr(x, y, shift, size, s.c_str(), color, align);
  }

bool displayfrSP(int x, int y, int sh, int b, int size, const string &s, int color, int align, int p) {
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

bool displayfr(int x, int y, int b, int size, const string &s, int color, int align) {
  return displayfrSP(x, y, 0, b, size, s, color, align, poly_outline>>8);
  }

bool displaychr(int x, int y, int shift, int size, char chr, int col) {

  char buf[2];
  buf[0] = chr; buf[1] = 0;
  return displaystr(x, y, shift, size, buf, col, 8);
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

int colormix(int a, int b, int c) {
  for(int p=0; p<3; p++)
    part(a, p) = part(a,p) + (part(b,p) - part(a,p)) * part(c,p) / 255;
  return a;
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
      poly_outline = gradient(bordcolor, backcolor, 0, age, 256*vid.flashtime) << 8;
      displayfr(x, y, 1, vid.fsize, s, gradient(forecolor, backcolor, 0, age, 256*vid.flashtime), 8);
      msgs[i++] = msgs[j];
      }
    }
  msgs.resize(i);
  }

int gradient(int c0, int c1, ld v0, ld v, ld v1) {
  int vv = int(256 * ((v-v0) / (v1-v0)));
  int c = 0;
  for(int a=0; a<3; a++) {
    int p0 = (c0 >> (8*a)) & 255;
    int p1 = (c1 >> (8*a)) & 255;
    int p = (p0*(256-vv) + p1*vv + 127) >> 8;
    c |= p << (8*a);
    }
  return c;
  }

void drawCircle(int x, int y, int size, int color) {
  if(size < 0) size = -size;
  #if CAP_GL
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

#if CAP_XGD
  gdpush(4); gdpush(color); gdpush(x); gdpush(y); gdpush(size);
#elif CAP_SDLGFX
  ((vid.antialias && AA_NOGL)?aacircleColor:circleColor) (s, x, y, size, color);
#elif CAP_SDL
  int pts = size * 4;
  if(pts > 1500) pts = 1500;
  for(int r=0; r<pts; r++)
    qpixel(s, x + int(size * sin(r)), y + int(size * cos(r))) = color;
#endif
  }

void displayButton(int x, int y, const string& name, int key, int align, int rad) {
  if(displayfr(x, y, rad, vid.fsize, name, 0x808080, align)) {
    displayfr(x, y, rad, vid.fsize, name, 0xFFFF00, align);
    getcstat = key;
    }
  }

char mousekey = 'n';
char newmousekey;

void displaymm(char c, int x, int y, int rad, int size, const string& title, int align) {
  if(displayfr(x, y, rad, size, title, c == mousekey ? 0xFF8000 : 0xC0C0C0, align)) {
    displayfr(x, y, rad, size, title, 0xFFFF00, align);
    getcstat = SETMOUSEKEY, newmousekey = c;
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

ld textscale() { 
  return vid.fsize / (vid.radius * crossf) * (1+vid.alphax) * 2;
  }
  
// bool notgl = false;

int pngres = 2000;
int pngformat = 0;

#if CAP_PNG
void IMAGESAVE(SDL_Surface *s, const char *fname) {
  SDL_Surface *s2 = SDL_PNGFormatAlpha(s);
  SDL_SavePNG(s2, fname);
  SDL_FreeSurface(s2);
  }
#endif

#if CAP_SDL
void saveHighQualityShot(const char *fname, const char *caption, int fade) {

#if !CAP_SDLGFX
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
  dynamicval<bool> v6(auraNOGL, fname ? true : false);
  
  vid.xres = vid.yres = pngres;
  if(pngformat == 1) vid.xres = vid.yres * 4/3;
  if(pngformat == 2) vid.xres = vid.yres * 16/9;
  if(pngformat == 3) {
    vid.xres = vid.yres * 22/16;
    while(vid.xres & 15) vid.xres++;
    }
  printf("format = %d, %d x %d\n", pngformat, vid.xres, vid.yres);

  vid.usingGL = false;
  // if(vid.pmodel == 0) vid.scale = 0.99;
  calcparam();
  #if CAP_ROGUEVIZ
  rogueviz::fixparam();
  #endif

  printf("format = %d, %d x %d\n", pngformat, vid.xres, vid.yres);

  dynamicval<SDL_Surface*> v5(s, SDL_CreateRGBSurface(SDL_SWSURFACE,vid.xres,vid.yres,32,0,0,0,0));

  darken = 0;
  
  int numi = (fname?1:2);

  for(int i=0; i<numi; i++) {
    SDL_FillRect(s, NULL, numi==1 ? backcolor : i ? 0xFFFFFF : 0);
    drawfullmap();
    
    if(fade < 255) 
      for(int y=0; y<vid.yres; y++)
      for(int x=0; x<vid.xres; x++) {
        int& p = qpixel(s, x, y);
        for(int i=0; i<3; i++) {
          part(p,i) = (part(p,i) * fade + 127) / 255;
          }
        }

    if(caption)
      displayfr(vid.xres/2, vid.fsize+vid.fsize/4, 3, vid.fsize*2, caption, forecolor, 8);

    char buf[128]; strftime(buf, 128, "bigshota-%y%m%d-%H%M%S" IMAGEEXT, localtime(&timer));
    buf[7] += i;
    if(!fname) fname = buf;
    IMAGESAVE(s, fname);

    if(i == 0) addMessage(XLAT("Saved the high quality shot to %1", fname));
    }
  
  SDL_FreeSurface(s);
  }
#endif

#if CAP_SDL
bool setfsize = true;

void setvideomode() {

  DEBB(DF_INIT, (debugfile,"setvideomode\n"));
  
  if(!vid.full) {
    if(vid.xres > vid.xscr) vid.xres = vid.xscr * 9/10, setfsize = true;
    if(vid.yres > vid.yscr) vid.yres = vid.yscr * 9/10, setfsize = true;    
    }
  
  if(setfsize) vid.fsize = min(vid.yres / 32, vid.xres / 48), setfsize = false;

  int flags = 0;
  
#if CAP_GL
  if(vid.usingGL) {
    flags = SDL_OPENGL | SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER;
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);

    if(vid.antialias & AA_MULTI) {
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, (vid.antialias & AA_MULTI16) ? 16 : 4);
      }

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

#if CAP_GL
  if(vid.usingGL) {
    if(vid.antialias & AA_MULTI) 
      glEnable(GL_MULTISAMPLE);
    else
      glDisable(GL_MULTISAMPLE);
  
    glViewport(0, 0, vid.xres, vid.yres);
    resetGL();
    }
#endif
  }
#endif

bool noGUI = false;

void initgraph() {

  DEBB(DF_INIT, (debugfile,"initgraph\n"));
  
  initConfig();

#if CAP_SDLJOY
  joyx = joyy = 0; joydir.d = -1;
#endif
  
  restartGraph();
  
  initgeo();

  buildpolys();
  
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
  
  SDL_WM_SetCaption("HyperRogue " VER, "HyperRogue " VER);
#endif
  
  preparesort();
#if CAP_CONFIG
  loadConfig();
#endif

#if CAP_COMMANDLINE
  arg::read(2);
#endif

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

void cleargraph() {
  DEBB(DF_INIT, (debugfile,"clear graph\n"));
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

