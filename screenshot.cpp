// Hyperbolic Rogue -- screenshots in SVG and PNG formats
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

#if ISMOBILE==1
namespace svg {
  bool in = false;
  }
#endif

#if ISMOBILE==0
// svg renderer
namespace svg {
  FILE *f;
  bool in = false;
  
  ld cta(int col) {
    // col >>= 24;
    col &= 0xFF;
    return col / 255.0;
    }
  
  bool invisible(int col) { return (col & 0xFF) == 0; }
  
  ld gamma = .5;
  
  void fixgamma(unsigned int& color) {
    unsigned char *c = (unsigned char*) (&color);
    for(int i=1; i<4; i++) c[i] = 255 * pow(float(c[i] / 255.0), float(gamma));
    }
  
  int svgsize;
  int divby = 10;
  
  const char* coord(int val) {
    static char buf[10][20];
    static int id;
    id++; id %= 10;
    if(divby == 1) {
      sprintf(buf[id], "%d", val); return buf[id];
      }
    else if(divby <= 10) {
      sprintf(buf[id], "%.1f", val*1./divby); return buf[id];
      }
    else {
      sprintf(buf[id], "%.2f", val*1./divby); return buf[id];
      }
    }
  
  char* stylestr(unsigned int fill, unsigned int stroke, ld width=1) {
    fixgamma(fill);
    fixgamma(stroke);
    static char buf[600];
    // printf("fill = %08X stroke = %08x\n", fill, stroke);
  
    if(stroke == 0xFF00FF) {
      stroke = 0x000000FF;
      
      if(fill == 0x332a22ff) fill = 0x000000FF;
      else if(fill == 0x686868FF) fill = 0x000000FF;
      else if(fill == 0xd0d0d0FF) fill = 0x000000FF;
      else fill = 0xFFFFFFFF;
      }
    
    sprintf(buf, "style=\"stroke:#%06x;stroke-opacity:%.3" PLDF ";stroke-width:%" PLDF "px;fill:#%06x;fill-opacity:%.3" PLDF "\"",
      (stroke>>8) & 0xFFFFFF, cta(stroke),
      width/divby,
      (fill>>8) & 0xFFFFFF, cta(fill)
      );
    return buf;
    }
  
  void circle(int x, int y, int size, int col) {
    int ba = (backcolor << 8) + 0xFF;
    if(!invisible(col)) {
      if(vid.stretch == 1)
        fprintf(f, "<circle cx='%s' cy='%s' r='%s' %s/>\n",
          coord(x), coord(y), coord(size), stylestr(ba, col));
      else
        fprintf(f, "<ellipse cx='%s' cy='%s' rx='%s' ry='%s' %s/>\n",
          coord(x), coord(y), coord(size), coord(size*vid.stretch), stylestr(ba, col));
      }
    }
  
  const string *link;
  
  void startstring() {
    if(link) fprintf(f, "<a xlink:href=\"%s\" xlink:show=\"replace\">", link->c_str());
    }

  void stopstring() {
    if(link) fprintf(f, "</a>");
    }

  string font = "Times";
  
  void text(int x, int y, int size, const string& str, bool frame, int col, int align) {

    double dfc = (x - vid.xcenter) * (x - vid.xcenter) + 
      (y - vid.ycenter) * (y - vid.ycenter);
    dfc /= vid.radius;
    dfc /= vid.radius;
    // 0 = center, 1 = edge
    dfc = 1 - dfc;
    
    col = 0xFF + (col << 8);

    bool uselatex = font == "latex";  

    if(!invisible(col)) {
      startstring();
      string str2 = "";
      for(int i=0; i<(int) str.size(); i++)
        if(str[i] == '&')
          str2 += "&amp;";
        else if(str[i] == '<')
          str2 += "&lt;";
        else if(str[i] == '>')
          str2 += "&gt;";
        else if(uselatex && str[i] == '#')
          str2 += "\\#";
        else str2 += str[i];
      if(uselatex) str2 = string("\\myfont{")+coord(size)+"}{" + str2 + "}";  
      fprintf(f, "<text x='%s' y='%s' text-anchor='%s' ",
        coord(x), coord(y+size*.4), 
        align == 8 ? "middle" :
        align < 8 ? "start" :
        "end");
      if(!uselatex)
        fprintf(f, "font-family='%s' font-size='%s' ", font.c_str(), coord(size));      
      fprintf(f, "%s>%s</text>",
        stylestr(col, frame ? 0x0000000FF : 0, (1<<get_sightrange())*dfc/40), str2.c_str());
      stopstring();
      fprintf(f, "\n");
      }
    }
  
  void polygon(int *polyx, int *polyy, int polyi, int col, int outline, double minwidth) {
  
    if(invisible(col) && invisible(outline)) return;
    if(polyi < 2) return;
    double dfc;
    if(!pmodel && hyperbolic) {
      int avgx = 0, avgy = 0;
      for(int i=0; i<polyi; i++) 
        avgx += polyx[i],
        avgy += polyy[i];
      avgx /= polyi;
      avgy /= polyi;
      dfc = (avgx - vid.xcenter) * (avgx - vid.xcenter) + 
        (avgy - vid.ycenter) * (avgy - vid.ycenter);
      dfc /= vid.radius;
      dfc /= vid.radius;
      // 0 = center, 1 = edge
      dfc = 1 - dfc;
      
      if(dfc < 0) dfc = 1;
      dfc = max<double>(dfc, 1) * minwidth;
      }
    else dfc = .8 * minwidth;
    
    startstring();
    for(int i=0; i<polyi; i++) {
      if(i == 0)
        fprintf(f, "<path d=\"M ");
      else
        fprintf(f, " L ");
      fprintf(f, "%s %s", coord(polyx[i]), coord(polyy[i]));
      }
    
    fprintf(f, "\" %s/>", stylestr(col, outline, (hyperbolic ? vid.radius : vid.scrsize) *dfc/256));
    stopstring();
    fprintf(f, "\n");
    }
  
  void render(const char *fname) {

    if(cheater) doOvergenerate();

    dynamicval<videopar> v(vid, vid);
    dynamicval<bool> v2(in, true);
    dynamicval<int> v5(ringcolor, 0x808080FF);
    
    vid.usingGL = false;
    vid.xres = vid.yres = svgsize ? svgsize : min(1 << (get_sightrange()+7), 16384);
    calcparam();
    dynamicval<bool> v6(inHighQual, true); 
    darken = 0;
    
    time_t timer;
    timer = time(NULL);

    char buf[128]; strftime(buf, 128, "svgshot-%y%m%d-%H%M%S.svg", localtime(&timer));
    if(!fname) fname = buf;

    f = fopen(fname, "wt");
    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"%s\" height=\"%s\">\n", coord(vid.xres), coord(vid.yres));
    drawfullmap();
    fprintf(f, "</svg>\n");
    fclose(f);
    addMessage(XLAT("Saved the SVG shot to %1 (sightrange %2)", fname, its(get_sightrange())));
    }

#if CAP_COMMANDLINE
int read_args() {
  using namespace arg;
  if(argis("-svgsize")) {
    shift(); sscanf(argcs(), "%d/%d", &svg::svgsize, &svg::divby);
    }
  else if(argis("-svgfont")) {
    shift(); svg::font = args();
    // note: use '-svgfont latex' to produce text output as: \myfont{size}{text}
    // (this is helpful with Inkscape's PDF+TeX output feature; define \myfont yourself)
    }
  else if(argis("-svggamma")) {
    shift(); svg::gamma = argf();
    }
  else if(argis("-svgshot")) {
    PHASE(3); shift(); start_game();
    printf("saving SVG screenshot to %s\n", argcs());
    svg::render(argcs());
    }
  else return 1;
  return 0;
  }

auto ah = addHook(hooks_args, 0, read_args);
#endif
  }
#endif

#if CAP_SDL
int pngres = 2000;
int pngformat = 0;

#if CAP_PNG
void IMAGESAVE(SDL_Surface *s, const char *fname) {
  SDL_Surface *s2 = SDL_PNGFormatAlpha(s);
  SDL_SavePNG(s2, fname);
  SDL_FreeSurface(s2);
  }
#endif

hookset<void(renderbuffer*)> *hooks_hqshot;

void saveHighQualityShot(const char *fname, const char *caption, int fade) {

  resetbuffer rb;

  // int maxrange = getDistLimit() * 3/2;

  // dynamicval<int> v3(sightrange, (cheater && sightrange < maxrange) ? maxrange : sightrange);

  if(cheater) doOvergenerate();

  time_t timer;
  timer = time(NULL);

  dynamicval<videopar> v(vid, vid);
  dynamicval<bool> v2(inHighQual, true);
  dynamicval<bool> v6(auraNOGL, fname ? true : false);
  
  vid.xres = vid.yres = pngres;
  if(pngformat == 1) vid.xres = vid.yres * 4/3;
  if(pngformat == 2) vid.xres = vid.yres * 16/9;
  if(pngformat == 3) {
    vid.xres = vid.yres * 22/16;
    while(vid.xres & 15) vid.xres++;
    }

  // if(vid.pmodel == 0) vid.scale = 0.99;
  calcparam();

  renderbuffer glbuf(vid.xres, vid.yres, vid.usingGL);
  glbuf.enable();
  stereo::set_viewport(0);

  // printf("format = %d, %d x %d\n", pngformat, vid.xres, vid.yres);

  darken = 0;
  
  int numi = (fname?1:2);

  for(int i=0; i<numi; i++) {
    glbuf.clear(numi==1 ? backcolor : i ? 0xFFFFFF : 0);
    
    #if CAP_RUG
    if(rug::rugged)
      rug::drawRugScene();
    else
    #endif
      drawfullmap();
    
    drawStats();
    
    callhooks(hooks_hqshot, &glbuf);

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
    IMAGESAVE(glbuf.render(), fname);
    
    if(i == 0) addMessage(XLAT("Saved the high quality shot to %1", fname));
    }
  
  rb.reset();
  }

#if CAP_COMMANDLINE
int png_read_args() {
  using namespace arg;
  if(argis("-pngshot")) {
    PHASE(3); shift(); start_game();
    printf("saving PNG screenshot to %s\n", argcs());
    saveHighQualityShot(argcs());
    }
  else if(argis("-pngsize")) {
    shift(); pngres = argi();
    }
  else if(argis("-pngformat")) {
    shift(); pngformat = argi();
    }
  else return 1;
  return 0;
  }

auto ah_png = addHook(hooks_args, 0, png_read_args);
#endif
#endif
}
