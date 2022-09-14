// Hyperbolic Rogue -- screenshots and animations
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file screenshot.cpp
 *  \brief screenshots, SVG format, animations, start animations
 */

#include "hyper.h"
namespace hr {

EX bool hide_hud = true;

#if HDR
namespace shot { void default_screenshot_content(); }
#endif

// svg renderer
EX namespace svg {

#if !CAP_SVG
EX always_false in;
#endif

#if CAP_SVG
  #if ISWEB
  shstream f;
  #else
  fhstream f;
  #endif
  
  EX bool in = false;
  
  ld cta(color_t col) {
    // col >>= 24;
    col &= 0xFF;
    return col / 255.0;
    }
  
  bool invisible(color_t col) { return (col & 0xFF) == 0; }
  
  void fixgamma(unsigned int& color) {
    unsigned char *c = (unsigned char*) (&color);
    for(int i=1; i<4; i++) c[i] = 255 * pow(float(c[i] / 255.0), float(shot::gamma));
    }
  
  int svgsize;
  EX int divby = 10;
  
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
  
  char* stylestr(color_t fill, color_t stroke, ld width=1) {
    fixgamma(fill);
    fixgamma(stroke);
    static char buf[600];
    // printf("fill = %08X stroke = %08x\n", fill, stroke);
  
    if(stroke == 0xFF00FF && false) {
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
  
  EX void circle(int x, int y, int size, color_t col, color_t fillcol, double linewidth) {
    if(!invisible(col) || !invisible(fillcol)) {
      if(pconf.stretch == 1)
        println(f, "<circle cx='", coord(x), "' cy='", coord(y), "' r='", coord(size), "' ", stylestr(fillcol, col, linewidth), "/>");
      else
        println(f, "<ellipse cx='", coord(x), "' cy='", coord(y), "' rx='", coord(size), "' ry='", coord(size*pconf.stretch), "' ", stylestr(fillcol, col), "/>");
      }
    }
  
  EX string link;
  
  void startstring() {
    if(link != "") print(f, "<a xlink:href=\"", link, "\" xlink:show=\"replace\">");
    }

  void stopstring() {
    if(link != "") print(f, "</a>");
    }

  string font = "Times";
  
  ld text_width_multiplier = 1/40.;
  int min_text = 3;
  
  EX void text(int x, int y, int size, const string& str, bool frame, color_t col, int align) {
    if(size < min_text) return;

    double dfc = (x - current_display->xcenter) * (x - current_display->xcenter) + 
      (y - current_display->ycenter) * (y - current_display->ycenter);
    dfc /= current_display->radius;
    dfc /= current_display->radius;
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
      
      print(f, "<text x='", coord(x), "' y='", coord(y+size*.4), "' text-anchor='", align == 8 ? "middle" :
        align < 8 ? "start" :
        "end", "' ");
      if(!uselatex)
        print(f, "font-family='", font, "' font-size='", coord(size), "' ");
      print(f, 
        stylestr(col, frame ? 0x0000000FF : 0, (1<<get_sightrange())*dfc*text_width_multiplier), 
        ">", str2, "</text>");
      stopstring();
      println(f);
      }
    }
  
  EX void polygon(int *polyx, int *polyy, int polyi, color_t col, color_t outline, double linewidth) {
  
    if(invisible(col) && invisible(outline)) return;
    if(polyi < 2) return;

    startstring();
    for(int i=0; i<polyi; i++) {
      if(i == 0)
        print(f, "<path d=\"M ");
      else
        print(f, " L ");
      print(f, coord(polyx[i]), " ", coord(polyy[i]));
      }
    
    print(f, "\" ", stylestr(col, outline, (hyperbolic ? current_display->radius : current_display->scrsize) * linewidth/256), "/>");
    stopstring();
    println(f);
    }
  
  EX void render(const string& fname, const function<void()>& what IS(shot::default_screenshot_content)) {
    dynamicval<bool> v2(in, true);
    dynamicval<bool> v3(vid.usingGL, false);
    
    #if ISWEB
    f.s = "";
    #else
    f.f = fopen(fname.c_str(), "wt");
    #endif

    println(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"", coord(vid.xres), "\" height=\"", coord(vid.yres), "\">");
    if(!shot::transparent)
      println(f, "<rect width=\"", coord(vid.xres), "\" height=\"", coord(vid.yres), "\" ", stylestr((backcolor << 8) | 0xFF, 0, 0), "/>");
    what();
    println(f, "</svg>");
    
    #if ISWEB
    EM_ASM_({
      var x=window.open();
      x.document.open();
      x.document.write(UTF8ToString($0));
      x.document.close();
      }, f.s.c_str());
    #else
    fclose(f.f); f.f = NULL;
    #endif
    }

#if CAP_COMMANDLINE && CAP_SHOT
int read_args() {
  using namespace arg;
  if(argis("-svgsize")) {
    shift(); sscanf(argcs(), "%d/%d", &shot::shoty, &svg::divby);
    if(shot::shotformat == -1) shot::shotformat = 0;
    }
  else if(argis("-svgfont")) {
    shift(); svg::font = args();
    // note: use '-svgfont latex' to produce text output as: \myfont{size}{text}
    // (this is helpful with Inkscape's PDF+TeX output feature; define \myfont yourself)
    }
  else if(argis("-svggamma")) {
    shift_arg_formula(shot::gamma);
    }
  else if(argis("-svgfade")) {
    shift_arg_formula(shot::fade);
    }
  else if(argis("-svgshot")) {
    PHASE(3); shift(); start_game();
    printf("saving SVG screenshot to %s\n", argcs());
    shot::format = shot::screenshot_format::svg;
    shot::take(argcs());
    }
  else if(argis("-svgtwm")) {
    shift_arg_formula(svg::text_width_multiplier);
    }
  else if(argis("-svgmt")) {
    shift(); svg::min_text = argi();
    }
  else return 1;
  return 0;
  }

auto ah = addHook(hooks_args, 0, read_args);
#endif
auto ah2 = addHook(hooks_configfile, 100, [] {
  #if CAP_CONFIG
  addsaver(shot::shotx, "shotx");
  addsaver(shot::shoty, "shoty");
  addsaverenum(shot::format, "shotsvg");
  addsaver(shot::transparent, "shottransparent");
  param_f(shot::gamma, "shotgamma");
  addsaver(shot::caption, "shotcaption");
  param_f(shot::fade, "shotfade");
  #endif
  });

#endif
EX }

/** wrl renderer */
EX namespace wrl {
#if !CAP_WRL
EX always_false in;
#endif

#if CAP_WRL
  EX bool in;
  
  EX bool print;
  EX bool textures = true;
  
  EX ld rug_width = .01;
  
  fhstream f;
  string filename;
  
  string coord(ld val) {
    char buf[100];
    snprintf(buf, 100, "%f", val);
    return buf;
    }

  string coord(const hyperpoint& v, int q) {
    char buf[100];
    if(q == 3) snprintf(buf, 100, "%f, %f, %f", v[0], v[1], v[2]);
    if(q == 2) snprintf(buf, 100, "%f, %f", v[0], v[1]);
    return buf;
    }
  
  string color(color_t col, ld v) {
    char buf[100];
    ld cols[4];
    for(int i=0; i<4; i++) {

      cols[i] = part(col, i);
      cols[i] /= 255;
      cols[i] = pow(cols[i], shot::gamma) * shot::fade * v;
      }
          
    snprintf(buf, 100, "%.3f %.3f %.3f", cols[3], cols[2], cols[1]);
    return buf;
    }
  
  typedef unsigned long long hashtype;
  hashtype hash(ld x) { return hashtype(x * 1000000 + .5); }
  
  hashtype hash(hyperpoint h) {
    return hash(h[0]) + 7 * hash(h[1]) + 13 * hash(h[2]);
    }
  
  EX void fatten(vector<hyperpoint>& data, vector<glvertex>& tdata) {
    map<hashtype, hyperpoint> normals;
    for(int i=0; i<isize(data); i++) 
      normals[hash(data[i])] = Hypc;
    for(int i=0; i<isize(data); i++) {
      int j = i%3 ? i-1 : i+2;
      int k = j%3 ? j-1 : j+2;
      hyperpoint normal = (data[j] - data[i]) ^ (data[k] - data[i]);
      #if MAXMDIM >= 4
      normal[3] = 0;
      #endif
      if(sqhypot_d(3, normal) < 1e-6) {
        println(hlog, "bug ", tie(data[i], data[j], data[k]));
        }
      normal /= hypot_d(3, normal);
      auto& res = normals[hash(data[i])];
      ld q = res[3];
      if((res | normal) < 0) res -= normal;
      else res += normal;
      res[3] = q + 1;
      }
    for(auto& p: normals) {
      auto w = hypot_d(3, p.second);
      if(w == 0) println(hlog, "width is 0, ", p.second, " appeared ", p.second[3], " times");
      if(isnan(w)) println(hlog, "width is NAN, ", p.second, " appeared ", p.second[3], " times");
      p.second = p.second * (rug_width / w);
      }
    vector<hyperpoint> data2;
    vector<glvertex> tdata2;
    for(int i=0; i<isize(data); i+=3) {
      auto a = data[i], b = data[i+1], c = data[i+2];
      hyperpoint normal = (b-a) ^ (c-a);
      auto na = normals[hash(a)];
      auto nb = normals[hash(b)];
      auto nc = normals[hash(c)];
      if((normal | na) > 0) na = -na;
      if((normal | nb) > 0) nb = -nb;
      if((normal | nc) > 0) nc = -nc;
      bool bad = false;
      for(int i=0; i<3; i++) {
        if(isnan(na[i]) || isnan(nb[i]) || isnan(nc[i])) bad = true;
        }
      if(bad) {
        println(hlog, "bad vertex");
        continue;
        }
      data2.push_back(a+na); data2.push_back(b+nb); data2.push_back(c+nc);
      data2.push_back(b+nb); data2.push_back(a+na); data2.push_back(a-na);
      data2.push_back(b+nb); data2.push_back(a-na); data2.push_back(b-nb);
      data2.push_back(c+nc); data2.push_back(b+nb); data2.push_back(b-nb);
      data2.push_back(c+nc); data2.push_back(b-nb); data2.push_back(c-nc);
      data2.push_back(a+na); data2.push_back(c+nc); data2.push_back(c-nc);
      data2.push_back(a+na); data2.push_back(c-nc); data2.push_back(a-na);
      data2.push_back(b-nb); data2.push_back(a-na); data2.push_back(c-nc);
      if(isize(tdata)) {
        auto ta = tdata[i], tb = tdata[i+1], tc = tdata[i+2];
        for(auto p: {ta, tb, tc, tb, ta, ta, tb, ta, tb, tc, tb, tb, tc, tb, tc, ta, tc, tc, ta, tc, ta, tb, ta, tc})
          tdata2.push_back(p);
        }
      }
    data = data2;
    tdata = tdata2;
    }
  
  bool used_rug;
  
  map<pair<color_t, glvertex>, int> texture_position;
  map<color_t, int> gradient_position;

  pair<color_t, glvertex> texid(dqi_poly& p) {
    return make_pair(p.color, p.tinf->tvertices[0]);
    }

  /** 0 = no/unknown/disabled texture, 1 = rug, 2 = gradient, 3 = floor texture */
  EX int texture_type(dqi_poly& p) {
    if(!p.tinf) return 0;
#if CAP_PNG
    if(!textures) return 0;
    #if CAP_RUG
    if(p.tinf == &rug::tinf) return 1;
    #endif
    #if MAXMDIM >= 4
    if(p.tinf->texture_id == (int) floor_textures->renderedTexture)
      return (p.tinf->tvertices[0][0] == 0) ? 2 : 3;
    #endif
#endif
    return 0;
    }

  EX void prepare(dqi_poly& p) {
    if(print && !(p.flags & POLY_PRINTABLE)) return;
    if(!(p.flags & POLY_TRIANGLES)) return;
    int tt = texture_type(p);
    if(tt == 2) gradient_position[p.color] = 0;
    if(tt == 3) texture_position[texid(p)] = 0;
    }
  
  #if MAXMDIM >= 4
  int fts_int, fts, fts_row;
  #endif
  
  map<string, pair<vector<hyperpoint>, vector<glvertex>>> all_data;
  
  EX void polygon(dqi_poly& p) {
    if(print && !(p.flags & POLY_PRINTABLE)) return;
    if(!(p.flags & POLY_TRIANGLES)) return;
    int tt = texture_type(p);

    vector<hyperpoint> data;
    vector<glvertex> tdata;
    for(int i=0; i<p.cnt; i++) {
      glvertex v = p.tab[0][p.offset+i];
      data.push_back(glhr::gltopoint(v));
      if(p.tinf) 
        tdata.push_back(p.tinf->tvertices[p.offset_texture+i]);
      }
    for(auto& d: data) {
      shiftpoint h = p.V * d;
      applymodel(h, d);
      }
    if(print && (p.flags & POLY_FAT)) {
      fatten(data, tdata);
      p.cnt = isize(data);
      }
    else if(print) {
      hyperpoint ctr1;
      applymodel(p.V * p.intester, ctr1);
      println(hlog, "intester = ", p.intester);
      ld sdet = 0;
      if(1) {
        dynamicval<eGeometry> g(geometry, gEuclid);
        for(int i=0; i<p.cnt; i+=3) {
          transmatrix T;
          T[0] = data[i] - ctr1;
          T[1] = data[i+1] - ctr1;
          T[2] = data[i+2] - ctr1;
          sdet += det(T);
          }
        println(hlog, "sdet = ", sdet);
        if(sdet > 0)
          for(int i=0; i<p.cnt; i+=3) {
            swap(data[i+1], data[i+2]);
            if(!tdata.empty())
              swap(tdata[i+1], tdata[i+2]);
            }
        }
      }
    
    shstream app;
    println(app, "    material Material {");
    if(!tt) println(app, "      diffuseColor ", color(p.color, .8));
    if(part(p.color, 0) != 255) println(app, "      transparency ", (255 - part(p.color, 0)) / 255.);
    println(app, "      }");
    if(tt == 1) {
      println(f, "    texture ImageTexture {");
      println(app, "      url \"", filename, "-rug.png\"");
      println(app, "      }");
      used_rug = true;
      }      
    if(tt == 2 || tt == 3) {
      println(app, "    texture ImageTexture {");
      println(app, "      url \"", filename, "-floors.png\"");
      println(app, "      }");
      }
    
    auto &ad = all_data[app.s];
    for(auto& d: data) ad.first.push_back(d);

    #if MAXMDIM >= 4
    if(tt == 2) {
      ld x = (fts - .5 - gradient_position[p.color]) / fts;
      for(auto& d: tdata) d[0] = x;
      }
    
    #if CAP_GL
    if(tt == 3) {
      int tp = texture_position[texid(p)];
      auto xy = make_array<int>(tp % fts_row, tp / fts_row);
      auto zero = p.tinf->tvertices[0];
      ld sca = FLOORTEXTURESIZE*1./fts_int;
      for(auto& d: tdata) 
        for(int c: {0, 1})
          d[c] = ((d[c] - zero[c])*sca + xy[c] + .5) * fts_int / fts;
      }
    #endif
    #endif

    for(auto& d: tdata) ad.second.push_back(d);
    }
  
  EX void render() {
    #if MAXMDIM >= 4
    for(auto& p: ptds) {
      auto p2 = dynamic_cast<dqi_poly*>(&*p);
      if(p2)
        prepare(*p2);
      }

    int tps = 0;
    for(auto& p: texture_position) p.second = tps++;
    int gps = 0;
    for(auto& p: gradient_position) p.second = gps++;
    
    #if CAP_TEXTURE
    fts_int = floor_texture_square_size * FLOORTEXTURESIZE + 4;
    fts = 64;
    
    while(fts < gps || (fts-gps)/fts_int * fts/fts_int < tps)
      fts *= 2;    
    
    fts_row = (fts-gps)/fts_int;
    #endif
    #endif
    
    for(auto& p: ptds) {
      auto p2 = dynamic_cast<dqi_poly*>(&*p);
      if(p2)
        polygon(*p2);
      }
    }
  
  EX void take(const string& fname, const function<void()>& what IS(shot::default_screenshot_content)) {
    dynamicval<bool> v2(in, true);
    dynamicval<bool> v3(noshadow, true);
    filename = fname;
    
    ptds.clear();
    all_data.clear();
    what();

    f.f = fopen(fname.c_str(), "wt");
    
    println(f, "#VRML V2.0 utf8");
    println(f, "WorldInfo { title \"3D model exported from HyperRogue\" info [ \"3D models exported from HyperRogue are public domain\" ] }");
    
    for(auto& p: all_data) {
      const string& app = p.first;
      auto& data = p.second.first;
      auto& tdata = p.second.second;
      
      println(f, "Shape {");
      println(f, "  appearance Appearance {");
      println(f, app);
      println(f, "    }");
      // println(f, "# V = ", p.V);
      println(f, "  geometry IndexedFaceSet {");    
      println(f, "    coord Coordinate {");
      
      println(f, "      point [");
      for(auto& d: data) println(f, "       ", coord(d, 3), ",");
      println(f, "        ]");
      println(f, "      }");
  
      if(!tdata.empty()) {
        println(f, "      texCoord TextureCoordinate {");
        println(f, "        point [");
  
        for(auto& d: tdata)
          println(f, "          ", coord(glhr::gltopoint(d), 2), ",");
        println(f, "        ]");
        println(f, "      }");
        }
      
      println(f, "    coordIndex [");
      for(int i=0; i<isize(data); i+=3) {
        println(f, "        ", i, " ", i+1, " ", i+2, " -1,");
        }
      println(f, "      ]");
      if(print)
        println(f, "    creaseAngle 0.0 convex FALSE solid TRUE ccw FALSE");
      else
        println(f, "    creaseAngle 0.0 convex FALSE solid FALSE");
      println(f, "    }");
      println(f, "  }");
      }
    
    #if CAP_PNG
    #if CAP_RUG
    if(used_rug) {
      resetbuffer rb;
      rug::glbuf->enable();
      SDL_Surface *s = rug::glbuf->render();
      dynamicval<int> dx(shot::shotx, rug::texturesize);
      dynamicval<int> dy(shot::shoty, rug::texturesize);
      shot::postprocess(filename + "-rug.png", s, s);
      }
    #endif
    
    #if MAXMDIM >= 4
    if(isize(texture_position) || isize(gradient_position)) {
      SDL_Surface *s = shot::empty_surface(fts, fts, false);
      for(auto& p: gradient_position) {
        int x = fts - p.second - 1;
        for(int y=0; y<fts; y++) {
          qpixel(s, x, fts-y-1) = gradient(0, p.first, 0, y, fts-1) >> 8;
          part(qpixel(s, x, y), 3) = 0xFF;
          }
        }
      
      SDL_Surface *floor = floor_textures->render();
      for(auto& p: texture_position) {
        int nx = p.second % fts_row;
        int ny = p.second / fts_row;
        color_t col = p.first.first;
        int xs = p.first.second[0] * FLOORTEXTURESIZE - fts_int/2;
        int ys = p.first.second[1] * FLOORTEXTURESIZE - fts_int/2;
        swap(xs, ys); // I do not understand why
        for(int y=0; y<fts_int; y++)
        for(int x=0; x<fts_int; x++) {
          auto& tgt = qpixel(s, nx*fts_int+x, fts-1-(ny*fts_int+y));
          auto& src = qpixel(floor, xs+x, FLOORTEXTURESIZE-1-(ys+y));
          for(int p=0; p<3; p++)
            part(tgt, p) = (part(src, p) * part(col, p+1) + 127) / 255;
          part(tgt, 3) = 0xFF;
          }
        }
      IMAGESAVE(s, (filename + "-floors.png").c_str());
      SDL_FreeSurface(s);
      }
    #endif
    #endif
    
    fclose(f.f);
    f.f = nullptr;
    }
#endif
EX }

#if CAP_PNG
void IMAGESAVE(SDL_Surface *s, const char *fname) {
  SDL_Surface *s2 = SDL_PNGFormatAlpha(s);
  SDL_SavePNG(s2, fname);
  if(s != s2) SDL_FreeSurface(s2);
  }
#endif

#if CAP_SHOT
EX namespace shot {

purehookset hooks_hqshot;

#if HDR
enum screenshot_format { png, svg, wrl, rawfile };
#endif

EX int rawfile_handle;

EX int shotx = 2000;
EX int shoty = 2000;
EX screenshot_format format;
EX bool transparent = true;
EX ld gamma = 1;
EX int shotformat = -1;
EX string caption;
EX ld fade = 1;

void set_shotx() {
  if(shotformat == -1) return;
  shotx = shoty;
  if(shotformat == 1) shotx = shotx * 4/3;
  if(shotformat == 2) shotx = shotx * 16/9;
  if(shotformat == 3) {
    shotx = shotx * 22/16;
    while(shotx & 15) shotx++;
    }
  }

EX int shot_aa = 1;

EX void default_screenshot_content() {

  gamescreen();

  if(caption != "")
    displayfr(vid.xres/2, vid.fsize+vid.fsize/4, 3, vid.fsize*2, caption, forecolor, 8);
  callhooks(hooks_hqshot);
  drawStats();    
  }

#if CAP_SDL
EX SDL_Surface *empty_surface(int x, int y, bool alpha) {
  return SDL_CreateRGBSurface(SDL_SWSURFACE,x,y,32,0xFF<<16,0xFF<<8,0xFF, (alpha) ? (0xFF<<24) : 0);
  }
#endif

#if CAP_PNG

EX void output(SDL_Surface* s, const string& fname) {
  if(format == screenshot_format::rawfile) {
    for(int y=0; y<shoty; y++)
      ignore(write(rawfile_handle, &qpixel(s, 0, y), 4 * shotx));
    }
  else
    IMAGESAVE(s, fname.c_str());
  }

EX hookset<bool(string, SDL_Surface*, SDL_Surface*)> hooks_postprocess;

EX void postprocess(string fname, SDL_Surface *sdark, SDL_Surface *sbright) {
  if(callhandlers(false, hooks_postprocess, fname, sdark, sbright)) return;
  if(gamma == 1 && shot_aa == 1 && sdark == sbright) {
    output(sdark, fname);
    return;
    }

  SDL_Surface *sout = empty_surface(shotx, shoty, sdark != sbright);
  for(int y=0; y<shoty; y++)
  for(int x=0; x<shotx; x++) {
    int val[2][4];
    for(int a=0; a<2; a++) for(int b=0; b<3; b++) val[a][b] = 0;
    for(int ax=0; ax<shot_aa; ax++) for(int ay=0; ay<shot_aa; ay++)
    for(int b=0; b<2; b++) for(int p=0; p<3; p++)
      val[b][p] += part(qpixel((b?sbright:sdark), x*shot_aa+ax, y*shot_aa+ay), p);
    
    int transparent = 0;
    int maxval = 255 * 3 * shot_aa * shot_aa;
    
    for(int p=0; p<3; p++) transparent += val[1][p] - val[0][p];
    
    color_t& pix = qpixel(sout, x, y);
    pix = 0;
    part(pix, 3) = 255 - (255 * transparent + (maxval/2)) / maxval;
    
    if(transparent < maxval) for(int p=0; p<3; p++) {
      ld v = (val[0][p] * 3. / maxval) / (1 - transparent * 1. / maxval);
      v = pow(v, gamma) * fade;
      v *= 255;
      if(v > 255) v = 255;
      part(pix, p) = v;
      }
    }
  output(sout, fname);
  SDL_FreeSurface(sout);
  }
#endif

EX purehookset hooks_take;

#if CAP_PNG
void render_png(string fname, const function<void()>& what) {
  resetbuffer rb;

  renderbuffer glbuf(vid.xres, vid.yres, vid.usingGL);
  glbuf.enable();
  current_display->set_viewport(0);

  dynamicval<color_t> v8(backcolor, transparent ? 0xFF000000 : backcolor);
  #if CAP_RUG
  if(rug::rugged && !rug::renderonce) rug::prepareTexture();
  #endif
  glbuf.clear(backcolor);
  what();
  
  SDL_Surface *sdark = glbuf.render();

  if(transparent) {
    renderbuffer glbuf1(vid.xres, vid.yres, vid.usingGL);
    backcolor = 0xFFFFFFFF;
    #if CAP_RUG
    if(rug::rugged && !rug::renderonce) rug::prepareTexture();
    #endif
    glbuf1.enable();
    glbuf1.clear(backcolor);
    current_display->set_viewport(0);
    what();
    
    postprocess(fname, sdark, glbuf1.render());
    }
  else postprocess(fname, sdark, sdark);
  }
#endif

EX void take(string fname, const function<void()>& what IS(default_screenshot_content)) {

  if(cheater) doOvergenerate();
  
  #if CAP_SVG  
  int multiplier = (format == screenshot_format::svg) ? svg::divby : shot_aa;
  #else
  int multiplier = shot_aa;
  #endif
  
  vector<bool> chg;
  for(auto ap: anims::aps) chg.push_back(*ap.value == ap.last);
  finalizer f([&] { 
    for(int i=0; i<isize(anims::aps); i++) 
      if(chg[i]) *anims::aps[i].value = anims::aps[i].last;
    });
  
  if(intra::in) what();

  dynamicval<videopar> v(vid, vid);
  dynamicval<bool> v2(inHighQual, true);
  dynamicval<bool> v6(auraNOGL, true);
  dynamicval<bool> vn(nohud, nohud || hide_hud);

  vid.smart_range_detail *= multiplier;
  darken = 0;
  
  set_shotx();
  vid.xres = shotx * multiplier;
  vid.yres = shoty * multiplier;
  vid.fsize *= multiplier;
  calcparam();
  models::configure();
  callhooks(hooks_take);
  
  switch(format) {
    case screenshot_format::wrl:
      #if CAP_WRL
      wrl::take(fname);
      #endif
      break;
    
    case screenshot_format::svg:
      #if CAP_SVG
      svg::render(fname, what);
      #endif
      break;
    
    case screenshot_format::png:
    case screenshot_format::rawfile:
      #if CAP_PNG
      render_png(fname, what);
      #endif
      break;
    }

  v.backup.plevel_factor = vid.plevel_factor;
  }

#if CAP_COMMANDLINE
int png_read_args() {
  using namespace arg;
  if(argis("-pngshot")) {
    PHASE(3); shift(); start_game();
    printf("saving PNG screenshot to %s\n", argcs());
    format = screenshot_format::png;
    shot::take(argcs());
    }
  else if(argis("-pngsize")) {
    shift(); shoty = argi(); if(shotformat == -1) shotformat = 0;
    }
  else if(argis("-pngformat")) {
    shift(); shotformat = argi();
    }
  else if(argis("-shotxy")) {
    shift(); shotformat = -1; shotx = argi(); shift(); shoty = argi();
    }
  else if(argis("-shothud")) {
    shift(); hide_hud = !argi();
    }
  else if(argis("-shott")) {
    shift(); shot::transparent = argi();
    }
  else if(argis("-shot-fhd")) {
    shot::shotformat = -1;
    shot::shotx = 1920;
    shot::shoty = 1080;
    shot::transparent = false;
    }
  else if(argis("-shot-hd")) {
    shot::shotformat = -1;
    shot::shotx = 1280;
    shot::shoty = 720;
    shot::transparent = false;
    }
  else if(argis("-shot-qfhd")) {
    shot::shotformat = -1;
    shot::shotx = 960;
    shot::shoty = 540;
    shot::transparent = false;
    }
  else if(argis("-shot-qhd")) {
    shot::shotformat = -1;
    shot::shotx = 640;
    shot::shoty = 360;
    shot::transparent = false;
    }
  else if(argis("-shot-1000")) {
    shot::shotformat = -1;
    shot::shotx = 1000;
    shot::shoty = 1000;
    shot::transparent = false;
    }
  else if(argis("-shot-500")) {
    shot::shotformat = -1;
    shot::shotx = 500;
    shot::shoty = 500;
    shot::transparent = false;
    }
  else if(argis("-shot-vertical")) {
    shot::shotformat = -1;
    shot::shotx = 720;
    shot::shoty = 1080;
    shot::transparent = false;
    }
  else if(argis("-shotaa")) {
    shift(); shot_aa = argi();
    }
  #if CAP_WRL
  else if(argis("-modelshot")) {
    PHASE(3); shift(); start_game();
    printf("saving WRL model to %s\n", argcs());
    shot::format = screenshot_format::wrl; wrl::print = false;
    shot::take(argcs());
    }
  else if(argis("-printshot")) {
    PHASE(3); shift(); start_game();
    printf("saving 3D printable model to %s\n", argcs());
    shot::format = screenshot_format::wrl; wrl::print = true;
    shot::take(argcs());
    }
  #endif
  else return 1;
  return 0;
  }

auto ah_png = addHook(hooks_args, 0, png_read_args);
#endif

EX string format_name() {
  if(format == screenshot_format::svg) return "SVG";
  if(format == screenshot_format::wrl) return "WRL";
  if(format == screenshot_format::png) return "PNG";
  return "?";
  }

EX string format_extension() {
  if(format == screenshot_format::svg) return ".svg";
  if(format == screenshot_format::wrl) return ".wrl";
  if(format == screenshot_format::png) return ".png";
  return "?";
  }


EX void choose_screenshot_format() {
  cmode = sm::SIDE; 
  gamescreen();
  dialog::init(XLAT("screenshots"), iinf[itPalace].color, 150, 100);
  #if CAP_PNG
  dialog::addItem(XLAT("PNG"), 'p');
  dialog::add_action([] { format = screenshot_format::png; popScreen(); });
  #endif
  #if CAP_SVG
  dialog::addItem(XLAT("SVG"), 's');
  dialog::add_action([] { format = screenshot_format::svg; popScreen(); });
  #endif
  #if CAP_WRL
  dialog::addItem(XLAT("WRL"), 'w');
  dialog::add_action([] { format = screenshot_format::wrl; popScreen(); });
  #endif
  dialog::addBack();
  dialog::display();
  }

EX void menu() {
  cmode = sm::SIDE; 
  gamescreen();
  if(format == screenshot_format::svg && !CAP_SVG) 
    format = screenshot_format::png;
  if(format == screenshot_format::png && !CAP_PNG) 
    format = screenshot_format::svg;
  dialog::init(XLAT("screenshots"), iinf[itPalace].color, 150, 100);
  dialog::addSelItem(XLAT("format"), format_name(), 'f');
  dialog::add_action_push(choose_screenshot_format);
  bool dowrl = format == screenshot_format::wrl;
  if(!dowrl) {
    dialog::addSelItem(XLAT("pixels (X)"), its(shotx), 'x');
    dialog::add_action([] { shotformat = -1; dialog::editNumber(shotx, 500, 8000, 100, 2000, XLAT("pixels (X)"), ""); });
    dialog::addSelItem(XLAT("pixels (Y)"), its(shoty), 'y');
    dialog::add_action([] { shotformat = -1; dialog::editNumber(shoty, 500, 8000, 100, 2000, XLAT("pixels (Y)"), ""); });
    }
  
  switch(format) {
    case screenshot_format::svg: {
      #if CAP_SVG
      using namespace svg;
      dialog::addSelItem(XLAT("precision"), "1/"+its(divby), 'p');
      dialog::add_action([] { divby *= 10; if(divby > 1000000) divby = 1; });
      #endif
      
      if(models::is_3d(vpconf) || rug::rugged) {
        dialog::addInfo("SVG screenshots do not work in this 3D mode", 0xFF0000);
        if(GDIM == 2 && !rug::rugged) 
          menuitem_projection('1');
        #if CAP_WRL
        else {
          dialog::addItem(XLAT("WRL"), 'w');
          dialog::add_action([] { format = screenshot_format::wrl; });
          }
        #endif
        }

      #if CAP_TEXTURE
      if(texture::config.tstate == texture::tsActive)
        dialog::addInfo("SVG screenshots do not work with textures", 0xFF0000);
      #endif
      break;
      }
    
    case screenshot_format::rawfile:
    case screenshot_format::png: {
      #if CAP_PNG
      dialog::addSelItem(XLAT("supersampling"), its(shot_aa), 's');
      dialog::add_action([] { shot_aa *= 2; if(shot_aa > 16) shot_aa = 1; });
      #endif
      break;
      }

    case screenshot_format::wrl: {
      #if CAP_WRL
      if(!models::is_3d(vpconf) && !rug::rugged) {
        dialog::addInfo("this format is for 3D projections", 0xFF0000);
        #if CAP_RUG
        if(GDIM == 2) {
          dialog::addItem(XLAT("hypersian rug mode"), 'u');
          dialog::add_action_push(rug::show);
          }
        #endif
        }
      #if CAP_RUG
      else if(rug::rugged ? rug::perspective() : models::is_perspective(vpconf.model)) {
      #else
      else if(models::is_perspective(vpconf.model)) {
      #endif
        dialog::addInfo("this does not work well in perspective projections", 0xFF8000);
        menuitem_projection('1');
        }
      dialog::addBoolItem_action("generate a model for 3D printing", wrl::print, 'p');
      #if CAP_PNG
      dialog::addBoolItem_action("use textures", wrl::textures, 'u');
      #endif
      #endif
      }      
    }
  if(!dowrl) dialog::addBoolItem_action(XLAT("transparent"), transparent, 't');

  dialog::addSelItem(XLAT("gamma"), fts(gamma), 'g');
  dialog::add_action([] { dialog::editNumber(gamma, 0, 2, .1, .5, XLAT("gamma"), "higher value = darker"); });

  dialog::addSelItem(XLAT("brightness"), fts(fade), 'b');
  dialog::add_action([] { dialog::editNumber(fade, 0, 2, .1, 1, XLAT("brightness"), "higher value = lighter"); });

  if(!dowrl) dialog::addBoolItem_action(XLAT("disable the HUD"), hide_hud, 'h');
  
  dialog::addBoolItem_action_neg(XLAT("hide the player"), mapeditor::drawplayer, 'H');
  #if CAP_WRL
  if(dowrl && wrl::print) dialog::lastItem().value = XLAT("N/A");
  #endif

  if(WDIM == 2) {
    dialog::addItem(XLAT("centering"), 'C');
    dialog::add_action([] {
      dialog::editNumber(vid.fixed_facing_dir, 0, 360, 15, 90, XLAT("centering"), 
        XLAT("You can pick the angle. Note: the direction the PC is facing matters."));
      dialog::reaction = fullcenter;
      dialog::extra_options = [] () { 
        dialog::addBoolItem(XLAT("rotate PC"), centering == eCentering::face, 'R');
        dialog::add_action([] { 
          flipplayer = false;
          cwt++;
          mirror::act(1, mirror::SPINSINGLE);
          cwt.at->mondir++;
          cwt.at->mondir %= cwt.at->type;
          fullcenter();
          });
        dialog::addBoolItem(XLAT("face"), centering == eCentering::face, 'F');
        dialog::add_action([] { centering = eCentering::face; fullcenter(); });
        dialog::addBoolItem(XLAT("edge"), centering == eCentering::edge, 'E');
        dialog::add_action([] { centering = eCentering::edge; fullcenter(); });
        dialog::addBoolItem(XLAT("vertex"), centering == eCentering::vertex, 'V');
        dialog::add_action([] { centering = eCentering::vertex; fullcenter(); });
        };
      });
    }

  dialog::addItem(XLAT("colors & aura"), 'c');
  dialog::add_action_push(show_color_dialog);

  menuitem_sightrange('r');

  dialog::addBreak(100);
  
  dialog::addItem(XLAT("take screenshot"), 'z');
  dialog::add_action([] () { 
    #if ISWEB
    shot::take("new window");
    #else
    static string pngfile = "hqshot.png";
    static string svgfile = "svgshot.svg";
    static string wrlfile = "model.wrl";
    string& file = 
      format == screenshot_format::png ? pngfile :
      format == screenshot_format::svg ? svgfile :
      wrlfile;

    dialog::openFileDialog(file, XLAT("screenshot"), format_extension(), [&file] () {
      dynamicval<int> cgl(vid.cells_generated_limit, 9999999);
      shot::take(file);
      return true;
      });
    #endif
    });
  dialog::addBack();
  dialog::display();
  }

EX }
#endif

#if CAP_ANIMATIONS
EX namespace anims {

#if HDR
enum eMovementAnimation {
  maNone, maTranslation, maRotation, maCircle, maParabolic, maTranslationRotation
  };
#endif

EX eMovementAnimation ma;

EX ld shift_angle, movement_angle, movement_angle_2;
EX ld normal_angle = 90;
EX ld period = 10000;
EX int noframes = 30;
EX ld cycle_length = 2 * M_PI;
EX ld parabolic_length = 1;
EX ld skiprope_rotation;

EX string time_formula = "-";

int lastticks, bak_turncount;

EX ld rug_rotation1, rug_rotation2, rug_forward, ballangle_rotation, env_ocean, env_volcano, rug_movement_angle, rug_shift_angle;
EX bool env_shmup;
EX ld rug_angle;

EX ld rotation_distance;
cell *rotation_center;
transmatrix rotation_center_View;

color_t circle_display_color = 0x00FF00FF;

EX ld circle_radius = acosh(2.);
EX ld circle_spins = 1;

EX void moved() {
  optimizeview();
  if(cheater || autocheat) {
    if(hyperbolic && memory_saving_mode && centerover && gmatrix.size() && cwt.at != centerover && !quotient) {
      if(isNeighbor(cwt.at, centerover)) {
        cwt.spin = neighborId(centerover, cwt.at);
        flipplayer = true;
        }
      animateMovement(match(cwt.at, centerover), LAYER_SMALL);
      cwt.at = centerover;
      save_memory();
      return;
      }
    setdist(centerover, 7 - getDistLimit() - genrange_bonus, NULL);
    }
  playermoved = false;
  }

#if HDR
struct animated_parameter {
  ld *value;
  ld last;
  string formula;
  reaction_t reaction;
  };
#endif

EX vector<animated_parameter> aps;

EX void deanimate(ld &x) {
  for(int i=0; i<isize(aps); i++) 
    if(aps[i].value == &x)
      aps.erase(aps.begin() + (i--));
  }

EX void get_parameter_animation(ld &x, string &s) {
  for(auto &ap: aps)
    if(ap.value == &x && ap.last == x)
      s = ap.formula;
  }

EX void animate_parameter(ld &x, string f, const reaction_t& r) {
  deanimate(x);
  aps.emplace_back(animated_parameter{&x, x, f, r});
  }

int ap_changes;

void apply_animated_parameters() {
  ap_changes = 0;
  for(auto &ap: aps) {
    if(*ap.value != ap.last) continue;
    try {
      *ap.value = parseld(ap.formula);
      }
    catch(hr_parse_exception&) {
      continue;
      }
    if(*ap.value != ap.last) {
      if(ap.reaction) ap.reaction();
      ap_changes++;
      ap.last = *ap.value;
      }
    }
  }

bool needs_highqual;

bool joukowsky_anim;

EX void reflect_view() {
  if(centerover) {
    shiftmatrix T = shiftless(Id);
    cell *mbase = centerover;
    cell *c = centerover;
    if(shmup::reflect(c, mbase, T))
      View = iso_inverse(T.T) * View;
    }
  }

bool clearup;

EX purehookset hooks_anim;

EX void animate_rug_movement(ld t) {
  rug::using_rugview urv;
  shift_view(
    cspin(0, GDIM-1, rug_movement_angle * degree) * spin(rug_shift_angle * degree) * xtangent(t)
    );
  }

vector<reaction_t> on_rollback;

EX void apply() {
  int t = ticks - lastticks;
  lastticks = ticks;
  
  callhooks(hooks_anim);

  switch(ma) {
    case maTranslation:
      if(history::on) {
        history::phase = (isize(history::v) - 1) * ticks * 1. / period;
        history::movetophase();        
        }
      else if(centerover) {
        reflect_view();
        if((hyperbolic && !quotient && 
          (centerover->land != cwt.at->land || memory_saving_mode) && among(centerover->land, laHaunted, laIvoryTower, laDungeon, laEndorian) && centerover->landparam >= 10
          ) ) {
          if(memory_saving_mode) {
            activateSafety(laIce);
            return;
            }
          else {
            fullcenter(); View = spin(rand() % 1000) * View;
            }
          }
        shift_view(
          cspin(0, GDIM-1, movement_angle * degree) * spin(shift_angle * degree) * xtangent(cycle_length * t / period)
          );
        moved();
        if(clearup) {
          centerover->wall = waNone;
          forCellEx(c1, centerover) c1->wall = waNone;
          }
        }
      break;

    case maRotation:
      shift_view(ztangent(-rotation_distance));
      if(GDIM == 3) {
        rotate_view(spin(-movement_angle * degree));
        rotate_view(cspin(1, 2, normal_angle * degree));
        rotate_view(spin(-movement_angle_2 * degree));
        }
      rotate_view(spin(2 * M_PI * t / period));
      if(GDIM == 3) {
        rotate_view(spin(movement_angle_2 * degree));
        rotate_view(cspin(2, 1, normal_angle * degree));
        rotate_view(spin(movement_angle * degree));
        }
      shift_view(ztangent(rotation_distance));
      moved();
      break;
    
    case maTranslationRotation:
      shift_view(
        cspin(0, GDIM-1, movement_angle * degree) * spin(shift_angle * degree) * xtangent(cycle_length * t / period)
        );
      moved();
      rotate_view(cspin(0, GDIM-1, 2 * M_PI * t / period));
      if(clearup) {
        centerover->wall = waNone;
        }
      break;

    #if CAP_BT
    case maParabolic:
      reflect_view();
      View = ypush(-shift_angle * degree) * spin(-movement_angle * degree) * View;
      if(GDIM == 2)
        View = bt::parabolic(parabolic_length * t / period) * View;
      else
        View = bt::parabolic3(parabolic_length * t / period, 0) * View;
      View = spin(movement_angle * degree) * ypush(shift_angle * degree) * View;
      moved();
      break;
    #endif
    case maCircle: {
      centerover = rotation_center;
      ld alpha = circle_spins * 2 * M_PI * ticks / period;
      View = spin(-cos_auto(circle_radius)*alpha) * xpush(circle_radius) * spin(alpha) * rotation_center_View;
      moved();
      break;
      }
    default: 
      break;
    }
  if(env_ocean) {
    turncount += env_ocean * ticks * tidalsize / period;
    calcTidalPhase();
    for(auto& p: gmatrix) if(p.first->land == laOcean) checkTide(p.first);
    turncount -= ticks * tidalsize / period;
    }
  if(env_volcano) {
    auto bak_turncount = turncount;
    on_rollback.push_back([bak_turncount] { turncount = bak_turncount; });
    turncount += env_volcano * ticks * 64 / period;
    for(auto& p: gmatrix) if(p.first->land == laVolcano) checkTide(p.first);
    }
  #if CAP_RUG
  if(rug::rugged) {
    if(rug_rotation1) {
      rug::using_rugview rv;
      rotate_view(cspin(1, 2, -rug_angle * degree) * cspin(0, 2, rug_rotation1 * 2 * M_PI * t / period) * cspin(1, 2, rug_angle * degree));
      }
    if(rug_rotation2) {
      rug::using_rugview rv;
      View = View * cspin(0, 1, rug_rotation2 * 2 * M_PI * t / period);
      }
    if(rug_forward) 
      animate_rug_movement(rug_forward * t / period);
    }
  #endif
  pconf.skiprope += skiprope_rotation * t * 2 * M_PI / period;

  if(ballangle_rotation) {
    if(models::has_orientation(vpconf.model))
      vpconf.model_orientation += ballangle_rotation * 360 * t / period;
    else
      vpconf.ballangle += ballangle_rotation * 360 * t / period;
    }
  if(joukowsky_anim) {
    ld t = ticks / period;
    t = t - floor(t);
    if(pmodel == mdBand) {
      vpconf.model_transition = t * 4 - 1;
      }
    else {
      vpconf.model_transition = t / 1.1;
      vpconf.scale = (1 - vpconf.model_transition) / 2.;
      }
    }
  apply_animated_parameters();
  calcparam();
  }

EX void rollback() {
  while(!on_rollback.empty()) {
    on_rollback.back()();
    on_rollback.pop_back();
    }
  }

#if CAP_FILES && CAP_SHOT
EX string animfile = "animation-%04d.png";

EX string videofile = "animation.mp4";

int min_frame = 0, max_frame = 999999;

int numturns = 0;

EX hookset<void(int, int)> hooks_record_anim;

EX bool record_animation_of(reaction_t content) {
  lastticks = 0;
  ticks = 0;
  int oldturn = -1;
  for(int i=0; i<noframes; i++) {
    if(i < min_frame || i > max_frame) continue;
    printf("%d/%d\n", i, noframes);
    callhooks(hooks_record_anim, i, noframes);
    int newticks = i * period / noframes;
    if(time_formula != "-") {
      dynamicval<int> t(ticks, newticks);
      exp_parser ep;
      ep.s = time_formula;
      try {
        newticks = ep.iparse();
        }
      catch(hr_parse_exception& e) {
        println(hlog, "warning: failed to parse time_formula, ", e.s);
        }
      }
    cmode = (env_shmup ? sm::NORMAL : 0);
    while(ticks < newticks) shmup::turn(1), ticks++;
    if(cheater && numturns) {
      int nturn = numturns * i / noframes;
      if(nturn != oldturn) monstersTurn();
      oldturn = nturn;
      }
    if(playermoved) centerpc(INF), optimizeview();
    dynamicval<bool> v2(inHighQual, true);
    models::configure();
    if(history::on) {
      ld len = (isize(history::v)-1) + 2 * history::extra_line_steps;
      history::phase = len * i / (noframes-1);
      if(history::lvspeed < 0) history::phase = len - history::phase;
      history::phase -= history::extra_line_steps;
      history::movetophase();
      }
    
    char buf[1000];
    snprintf(buf, 1000, animfile.c_str(), i);
    shot::take(buf, content);
    }
  lastticks = ticks = SDL_GetTicks();
  return true;
  }

EX bool record_animation() {
  return record_animation_of(shot::default_screenshot_content);
  }
#endif

EX purehookset hooks_after_video;

#if CAP_VIDEO
EX bool record_video(string fname IS(videofile), bool_reaction_t rec IS(record_animation)) {
  
  array<int, 2> tab;
  if(pipe(&tab[0])) {
    addMessage(format("Error: %s", strerror(errno)));
    return false;
    }
  println(hlog, "tab = ", tab);
  
  int pid = fork();
  if(pid == 0) {
    close(0);
    if(dup(tab[0]) != 0) exit(1);
    if(close(tab[1]) != 0) exit(1);
    if(close(tab[0]) != 0) exit(1);
    string fformat = "ffmpeg -y -f rawvideo -pix_fmt bgra -s " + its(shot::shotx) + "x" + its(shot::shoty) + " -r 60 -i - -pix_fmt yuv420p -codec:v libx264 \"" + fname + "\"";    
    ignore(system(fformat.c_str()));
    exit(0);
    }
  
  close(tab[0]);
  shot::rawfile_handle = tab[1];
  dynamicval<shot::screenshot_format> sf(shot::format, shot::screenshot_format::rawfile);
  rec();
  close(tab[1]);
  wait(nullptr);
  callhooks(hooks_after_video);
  return true;
  }

EX bool record_video_std() {
  return record_video(videofile, record_animation);
  }
#endif

void display_animation() {
  if(ma == maCircle && (circle_display_color & 0xFF)) {
    for(int s=0; s<10; s++) {
      if(s == 0) curvepoint(xpush0(circle_radius - .1));
      for(int z=0; z<100; z++) curvepoint(xspinpush0((z+s*100) * 2 * M_PI / 1000., circle_radius));
      queuecurve(ggmatrix(rotation_center), circle_display_color, 0, PPR::LINE);
      }
    if(sphere) for(int s=0; s<10; s++) {
      if(s == 0) curvepoint(xpush0(circle_radius - .1));
      for(int z=0; z<100; z++) curvepoint(xspinpush0((z+s*100) * 2 * M_PI / 1000., circle_radius));
      queuecurve(ggmatrix(rotation_center) * centralsym, circle_display_color, 0, PPR::LINE);
      }
    }
  }

void animator(string caption, ld& param, char key) {
  dialog::addBoolItem(caption, param, key);
  if(param) dialog::lastItem().value = fts(param);
  dialog::add_action([&param, caption] () { 
    if(param == 0) {
      param = 1;
      string s = 
        XLAT(
          "The value of 1 means that the period of this animation equals the period set in the animation menu. "
          "Larger values correspond to faster animations.");

      dialog::editNumber(param, 0, 10, 1, 1, caption, s); 
      }
    else param = 0;
    });
  }

EX ld a, b;

ld animation_period;

EX void rug_angle_options() {
  dialog::addSelItem(XLAT("shift"), fts(rug_shift_angle) + "°", 'C');
  dialog::add_action([] () { 
    popScreen();
    dialog::editNumber(rug_shift_angle, 0, 90, 15, 0, XLAT("shift"), ""); 
    });
  dialog::addSelItem(XLAT("movement angle"), fts(rug_movement_angle) + "°", 'M');
  dialog::add_action([] () { 
    popScreen();
    dialog::editNumber(rug_movement_angle, 0, 360, 15, 0, XLAT("movement angle"), ""); 
    });
  }

EX void show() {
  cmode = sm::SIDE; needs_highqual = false;
  animation_lcm = 1;
  gamescreen();
  animation_period = 2 * M_PI * animation_lcm / animation_factor;
  dialog::init(XLAT("animations"), iinf[itPalace].color, 150, 100);
  dialog::addSelItem(XLAT("period"), fts(period)+ " ms", 'p');
  dialog::add_action([] () { dialog::editNumber(period, 0, 10000, 1000, 200, XLAT("period"), 
    XLAT("This is the period of the whole animation, though in some settings the animation can have a different period or be aperiodic. "
      "Changing the value will make the whole animation slower or faster."
    )); });
  if(animation_lcm > 1) {
    dialog::addSelItem(XLAT("game animation period"), fts(animation_period)+ " ms", 'G');
    dialog::add_action([] () {
      dialog::editNumber(animation_period, 0, 10000, 1000, 1000, XLAT("game animation period"), 
        XLAT("Least common multiple of the animation periods of all the game objects on screen, such as rotating items.")
        );
      dialog::reaction = [] () { animation_factor = 2 * M_PI * animation_lcm / animation_period; };
      dialog::extra_options = [] () {
        dialog::addItem("default", 'D');
        dialog::add_action([] () {
          animation_factor = 1;
          popScreen();
          });
        };
      });
    }
  dialog::addBoolItem_choice(XLAT("no movement animation"), ma, maNone, '0');
  dialog::addBoolItem_choice(XLAT("translation"), ma, maTranslation, '1');
  dialog::addBoolItem_choice(XLAT("rotation"), ma, maRotation, '2');
  if(hyperbolic) {
    dialog::addBoolItem_choice(XLAT("parabolic"), ma, maParabolic, '3');
    }
  if(among(pmodel, mdJoukowsky, mdJoukowskyInverted)) {
    dialog::addBoolItem_action(XLAT("joukowsky_anim"), joukowsky_anim, 'j');
    }
  if(among(pmodel, mdJoukowsky, mdJoukowskyInverted)) {
    animator(XLAT("Möbius transformations"), skiprope_rotation, 'S');
    }
  if(!prod) {
    dialog::addBoolItem(XLAT("circle"), ma == maCircle, '4');
    dialog::add_action([] () { ma = maCircle; 
      rotation_center = centerover;
      rotation_center_View = View;
      });
    }
  dialog::addBoolItem_choice(XLAT("translation")+"+"+XLAT("rotation"), ma, maTranslationRotation, '5');
  switch(ma) {
    case maCircle: {
      animator(XLAT("circle spins"), circle_spins, 'C');
      dialog::addSelItem(XLAT("circle radius"), fts(circle_radius), 'c');
      dialog::add_action([] () { 
        dialog::editNumber(circle_radius, 0, 10, 0.1, acosh(1.), XLAT("circle radius"), ""); 
        dialog::extra_options = [] () {
          if(hyperbolic) {
            // area = 2pi (cosh(r)-1) 
            dialog::addSelItem(XLAT("double spin"), fts(acosh(2.)), 'A');
            dialog::add_action([] () { circle_radius = acosh(2.); });
            dialog::addSelItem(XLAT("triple spin"), fts(acosh(3.)), 'B');
            dialog::add_action([] () { circle_radius = acosh(3.); });
            }
          if(sphere) {
            dialog::addSelItem(XLAT("double spin"), fts(acos(1/2.)), 'A');
            dialog::add_action([] () { circle_radius = acos(1/2.); });
            dialog::addSelItem(XLAT("triple spin"), fts(acos(1/3.)), 'B');
            dialog::add_action([] () { circle_radius = acos(1/3.); });
            }
          };
        });
      dialog::addColorItem(XLAT("draw the circle"), circle_display_color, 'd');
      dialog::add_action([] () {
        dialog::openColorDialog(circle_display_color, NULL);
        });
      dialog::addBreak(100);
      break;
      }
    case maTranslation: 
    case maTranslationRotation:
    case maParabolic: {
      if(ma == maTranslation && history::on)
        dialog::addBreak(300);
      else if(ma == maTranslation) {
        dialog::addSelItem(XLAT("cycle length"), fts(cycle_length), 'c');
        dialog::add_action([] () { 
          dialog::editNumber(cycle_length, 0, 10, 0.1, 2*M_PI, "shift", ""); 
          dialog::extra_options = [] () {
            dialog::addSelItem(XLAT("full circle"), fts(2 * M_PI), 'A');
            dialog::add_action([] () { cycle_length = 2 * M_PI; });
            dialog::addSelItem(XLAT("Zebra period"), fts(2.898149445355172), 'B');
            dialog::add_action([] () { cycle_length = 2.898149445355172; });
            dialog::addSelItem(XLAT("Bolza period"), fts(2 * 1.528571), 'C');
            dialog::add_action([] () { cycle_length = 2 * 1.528571; });
            };
          });
        }
      else 
        add_edit(parabolic_length);
      dialog::addSelItem(XLAT("shift"), fts(shift_angle) + "°", 'C');
      dialog::add_action([] () { 
        dialog::editNumber(shift_angle, 0, 90, 15, 0, XLAT("shift"), ""); 
        });
      dialog::addSelItem(XLAT("movement angle"), fts(movement_angle) + "°", 'm');
      dialog::add_action([] () { 
        dialog::editNumber(movement_angle, 0, 360, 15, 0, XLAT("movement angle"), ""); 
        });
      break;
      }
    case maRotation:
      if(GDIM == 3) {
        dialog::addSelItem(XLAT("angle to screen normal"), fts(normal_angle) + "°", 'C');
        dialog::add_action([] () { 
          dialog::editNumber(normal_angle, 0, 360, 15, 0, XLAT("angle to screen normal"), ""); 
          });
        dialog::addSelItem(XLAT("movement angle"), fts(movement_angle) + "°", 'm');
        dialog::add_action([] () { 
          dialog::editNumber(movement_angle, 0, 360, 15, 0, XLAT("movement angle"), ""); 
          });
        dialog::addBreak(100);
        dialog::addSelItem(XLAT("distance from rotation center"), fts(rotation_distance), 'r');
        dialog::add_action([] () { 
          dialog::editNumber(rotation_distance, 0, 10, .1, 0, XLAT("distance from rotation center"), ""); 
          });
        dialog::addBreak(100);
        }
      else
        dialog::addBreak(300);
      break;
    default: {
      dialog::addBreak(300);
      }
    }
  animator(XLATN("Ocean"), env_ocean, 'o');
  animator(XLATN("Volcanic Wasteland"), env_volcano, 'v');
  if(shmup::on) dialog::addBoolItem_action(XLAT("shmup action"), env_shmup, 'T');
  #if CAP_FILES && CAP_SHOT
  if(cheater) {
    dialog::addSelItem(XLAT("monster turns"), its(numturns), 'n');
    dialog::add_action([] {      
      dialog::editNumber(numturns, 0, 100, 1, 0, XLAT("monster turns"), XLAT("Number of turns to pass. Useful when simulating butterflies or cellular automata."));
      });
    }
  #endif

  #if CAP_RUG
  if(rug::rugged) {
    animator(XLAT("screen-relative rotation"), rug_rotation1, 'r');
    if(rug_rotation1) { 
      dialog::addSelItem(XLAT("angle"), fts(rug_angle) + "°", 'a');
      dialog::add_action([] () { 
        dialog::editNumber(rug_angle, 0, 360, 15, 0, "Rug angle", ""); 
        });
      }
    else dialog::addBreak(100);
    animator(XLAT("model-relative rotation"), rug_rotation2, 'r');
    animator(XLAT("automatic move speed"), rug_forward, 'M');
    dialog::add_action([] () { 
      dialog::editNumber(rug_forward, 0, 10, 1, 1, XLAT("automatic move speed"), XLAT("Move automatically without pressing any keys."));
      dialog::extra_options = [] () {
        if(among(rug::gwhere, gSphere, gElliptic))  {
          dialog::addItem(XLAT("synchronize"), 'S');
          dialog::add_action([] () { rug_forward = 2 * M_PI; popScreen(); });
          }
        rug_angle_options();
        };
      });
    }
  #endif
  if(models::has_orientation(vpconf.model))
    animator(XLAT("model rotation"), ballangle_rotation, 'I');
  else if(models::is_3d(vpconf))
    animator(XLAT("3D rotation"), ballangle_rotation, '3');
  
  dialog::addSelItem(XLAT("animate parameters"), fts(a), 'a');
  dialog::add_action([] () {
    dialog::editNumber(a, -100, 100, 1, 0, XLAT("animate parameters"), "");
    });

  dialog::addSelItem(XLAT("animate parameters"), fts(b), 'b');
  dialog::add_action([] () {
    dialog::editNumber(b, -100, 100, 1, 0, XLAT("animate parameters"), "");
    });

  dialog::addBoolItem(XLAT("history mode"), (history::on || history::includeHistory), 'h');
  dialog::add_action_push(history::history_menu);

  #if CAP_FILES && CAP_SHOT
  dialog::addItem(XLAT("shot settings"), 's');
  dialog::add_action_push(shot::menu);

  if(needs_highqual) 
    dialog::addInfo(XLAT("some parameters will only change in recorded animation"));
  else
    dialog::addBreak(100);
  dialog::addSelItem(XLAT("frames to record"), its(noframes), 'n');
  dialog::add_action([] () { dialog::editNumber(noframes, 0, 300, 30, 5, XLAT("frames to record"), ""); });
  dialog::addSelItem(XLAT("record to sequence of image files"), animfile, 'R');
  dialog::add_action([] () {
    dialog::openFileDialog(animfile, XLAT("record to sequence of image files"), 
      shot::format_extension(), record_animation);
    });
  #endif
  #if CAP_VIDEO
  dialog::addSelItem(XLAT("record to video file"), videofile, 'M');
  dialog::add_action([] () {
    dialog::openFileDialog(videofile, XLAT("record to video file"), 
      ".mp4", record_video_std);
    });
  #endif
  dialog::addBack();
  dialog::display();
  }

#if CAP_COMMANDLINE
int readArgs() {
  using namespace arg;
           
  if(0) ;
#if CAP_ANIMATIONS
  else if(argis("-animmenu")) {
    PHASE(3); showstartmenu = false; pushScreen(show);
    }
  else if(argis("-animperiod")) {
    PHASEFROM(2); shift_arg_formula(period);
    }
  else if(argis("-animformula")) {
    PHASEFROM(2); shift(); time_formula = args();
    }
#if CAP_SHOT
  else if(argis("-animrecordf")) {
    PHASEFROM(2); shift(); noframes = argi();
    shift(); animfile = args();
    }
  else if(argis("-animrecord") || argis("-animrec")) {
    PHASE(3); shift(); noframes = argi();
    shift(); animfile = args(); record_animation();
    }
  else if(argis("-record-only")) {
    PHASEFROM(2); 
    shift(); min_frame = argi();
    shift(); max_frame = argi();
    }
#endif
#if CAP_VIDEO
  else if(argis("-animvideo")) {
    start_game();
    PHASE(3); shift(); noframes = argi();
    shift(); videofile = args(); record_video();
    }
#endif
  else if(argis("-animcircle")) {
    PHASE(3); start_game();
    ma = maCircle; 
    rotation_center = centerover;
    rotation_center_View = View;
    shift_arg_formula(circle_spins);
    shift_arg_formula(circle_radius);
    shift(); circle_display_color = argcolor(24);
    }
  else if(argis("-animmove")) {
    ma = maTranslation; 
    shift_arg_formula(cycle_length);
    shift_arg_formula(shift_angle);
    shift_arg_formula(movement_angle);
    }
  else if(argis("-animmoverot")) {
    ma = maTranslationRotation; 
    shift_arg_formula(cycle_length);
    shift_arg_formula(shift_angle);
    shift_arg_formula(movement_angle);
    }
  else if(argis("-wallopt")) {
    wallopt = true;
    }
  else if(argis("-animpar")) {
    ma = maParabolic; 
    shift_arg_formula(parabolic_length);
    shift_arg_formula(shift_angle);
    shift_arg_formula(movement_angle);
    }
  else if(argis("-animclear")) { clearup = true; }
  else if(argis("-animrot")) {
    ma = maRotation;
    if(GDIM == 3) {
      shift_arg_formula(movement_angle);
      shift_arg_formula(normal_angle);
      }
    }
  else if(argis("-animrotd")) {
    start_game();
    ma = maRotation;
    shift_arg_formula(rotation_distance);
    }
  else if(argis("-animrug")) {
    shift_arg_formula(rug_rotation1);
    shift_arg_formula(rug_angle);
    shift_arg_formula(rug_rotation2);
    }
  else if(argis("-animenv")) {
    shift_arg_formula(env_ocean);
    shift_arg_formula(env_volcano);
    }
  else if(argis("-animball")) {
    shift_arg_formula(ballangle_rotation);
    }
  else if(argis("-animj")) {
    shift(); joukowsky_anim = true;
    }
#endif
  else return 1;
  return 0;
  }
#endif

auto animhook = addHook(hooks_frame, 100, display_animation)
  #if CAP_COMMANDLINE
  + addHook(hooks_args, 100, readArgs)
  #endif
  + addHook(hooks_configfile, 100, [] {
    #if CAP_CONFIG
    param_f(anims::period, "aperiod", "animation period");
    addsaver(anims::noframes, "animation frames");
    param_f(anims::cycle_length, "acycle", "animation cycle length");
    param_f(anims::parabolic_length, "aparabolic", "animation parabolic length")
      ->editable(0, 10, 1, "cells to go", "", 'c');
    param_f(anims::rug_angle, "arugangle", "animation rug angle");
    param_f(anims::circle_radius, "acradius", "animation circle radius");
    param_f(anims::circle_spins, "acspins", "animation circle spins");
    addsaver(anims::rug_movement_angle, "rug forward movement angle", 90);
    addsaver(anims::rug_shift_angle, "rug forward shift angle", 0);
    addsaver(anims::a, "a", 0);
    addsaver(anims::b, "b", 0);
    param_f(anims::movement_angle_2, "movement angle 2", 0);
    #endif
    });

EX bool any_animation() {
  if(history::on) return true;
  if(ma) return true;
  if(ballangle_rotation || rug_rotation1 || rug_rotation2) return true;
  if(ap_changes) return true;
  return false;
  }

EX bool any_on() {
  return any_animation() || history::includeHistory;
  }

EX bool center_music() {
  return among(ma, maParabolic, maTranslation);
  }

EX }
#endif

EX namespace startanims {

EX bool enabled = true;

int ticks_start = 0;

#if HDR
struct startanim {
  string name;
  reaction_t init;
  reaction_t render;
  };

const int EXPLORE_START_ANIMATION = 2003;
#endif

reaction_t exploration;

void explorable(reaction_t ee) {
  if(displayButtonS(4, vid.yres - 4 - vid.fsize/2, XLAT("explore this animation"), 0x202020, 0, vid.fsize/2))
    getcstat = EXPLORE_START_ANIMATION;
  exploration = ee;
  }

void no_init() { }

startanim null_animation { "", no_init, [] { gamescreen(); }};

#if CAP_STARTANIM
startanim joukowsky { "Joukowsky transform", no_init, [] {
  dynamicval<eModel> dm(pmodel, mdJoukowskyInverted);
  dynamicval<ld> dt(pconf.model_orientation, ticks / 25.);
  dynamicval<int> dv(vid.use_smart_range, 2);
  dynamicval<ld> ds(pconf.scale, 1/4.);
  models::configure();
  dynamicval<color_t> dc(ringcolor, 0);  
  gamescreen();
  explorable([] { pmodel = mdJoukowskyInverted; pushScreen(models::model_menu); });
  }};

startanim bandspin { "spinning in the band model", no_init, [] {
  dynamicval<eModel> dm(pmodel, mdBand);
  dynamicval<ld> dt(pconf.model_orientation, ticks / 25.);
  dynamicval<int> dv(vid.use_smart_range, 2);
  models::configure();
  gamescreen();
  explorable([] { pmodel = mdJoukowskyInverted; pushScreen(models::model_menu); });
  }};

startanim perspective { "projection distance", no_init, [] {
  ld x = sin(ticks / 1500.);
  x += 1;
  x /= 2;
  x *= 1.5;
  x = tan(x);
  dynamicval<ld> da(pconf.alpha, x);
  dynamicval<ld> ds(pconf.scale, (1+x)/2);
  calcparam();
  gamescreen();
  explorable(projectionDialog);
  }};

startanim rug { "Hypersian Rug", [] { 
#if CAP_RUG
  rug::init();
  rug::rugged = false;
#else
  pick();
#endif
  }, [] {
  #if CAP_RUG
  dynamicval<bool> b(rug::rugged, true);
  rug::physics();
  dynamicval<transmatrix> t(rug::rugView, cspin(1, 2, ticks / 3000.) * rug::rugView);
  gamescreen();
  if(!rug::rugged) current = &null_animation;
  explorable([] { rug::rugged = true; pushScreen(rug::show); });
  #endif
  }};

startanim spin_around { "spinning around", no_init,  [] {
  dynamicval<ld> da(pconf.alpha, 999);
  dynamicval<ld> ds(pconf.scale, 500);
  ld alpha = 2 * M_PI * ticks / 10000.;
  ld circle_radius = acosh(2.);
  dynamicval<transmatrix> dv(View, spin(-cos_auto(circle_radius)*alpha) * xpush(circle_radius) * spin(alpha) * View);
  gamescreen();
  }};
#endif

reaction_t add_to_frame;

#if CAP_STARTANIM
void draw_ghost(const transmatrix V, int id) {
  auto sV = shiftless(V);
  if(id % 13 == 0) {
    queuepoly(sV, cgi.shMiniGhost, 0xFFFF00C0);
    queuepoly(sV, cgi.shMiniEyes, 0xFF);
    }
  else {
    queuepoly(sV, cgi.shMiniGhost, 0xFFFFFFC0);
    queuepoly(sV, cgi.shMiniEyes, 0xFF);
    }
  }

startanim row_of_ghosts { "row of ghosts", no_init, [] {
  dynamicval<reaction_t> r(add_to_frame, [] {
    int t = ticks/400;
    ld mod = (ticks-t*400)/400.;
    for(int x=-25; x<=25; x++)
      for(int y=-25; y<=25; y++) {
        ld ay = (y + mod)/5.;
        draw_ghost(xpush(x/5.) * spin(M_PI/2) * xpush(ay), int(y-t));
        }
    });
  dynamicval<bool> rd(mapeditor::drawplayer, false);
  gamescreen();
  }};

startanim army_of_ghosts { "army of ghosts", no_init, [] {
  dynamicval<bool> rd(mapeditor::drawplayer, false);
  dynamicval<reaction_t> r(add_to_frame, [] {
    int tt = ticks - ticks_start + 1200;
    int t = tt/400;
    ld mod = (tt-t*400)/400.;
    for(int x=-12; x<=12; x++) {
      ld ax = x/4.;
      transmatrix T = spin(-M_PI/2) * xpush(ax) * spin(M_PI/2);
      for(int y=0;; y++) {
        ld ay = (mod - y)/4.;
        transmatrix U = spin(M_PI/2) * xpush(ay / cosh(ax)) * T;
        if(!in_smart_range(shiftless(U))) break;
        draw_ghost(U, (-y - t));
        if(y) {
          ay = (mod + y)/4.;
          transmatrix U = spin(M_PI/2) * xpush(ay / cosh(ax)) * T;
          draw_ghost(U, (y - t));
          }
        }
      }
    });
  gamescreen();
  }};

startanim ghost_spiral { "ghost spiral", no_init, [] {
  dynamicval<reaction_t> r(add_to_frame, [] {
    ld t = (ticks - ticks_start - 2000) / 150000.;
    for(ld i=3; i<=40; i++) {
      draw_ghost(spin(t * i * 2 * M_PI) * xpush(asinh(15. / i)) * spin(M_PI/2), 1);
      }
    });
  gamescreen();
  }};

startanim fib_ghosts { "Fibonacci ghosts", no_init, [] {
  dynamicval<bool> rd(mapeditor::drawplayer, false);
  dynamicval<reaction_t> r(add_to_frame, [] {
    ld phase = (ticks - ticks_start - 2000) / 1000.;
    for(int i=0; i<=500; i++) {
      ld step = M_PI * (3 - sqrt(5));
      ld density = 0.01;
      ld area = 1 + (i+.5) * density;
      ld r = acosh(area);
      ld length = sinh(r);
      transmatrix T = spin(i * step + phase / length) * xpush(r) * spin(M_PI/2);
      draw_ghost(T, i);
      }
    });
  gamescreen();
  }};

startanim fpp { "first-person perspective", no_init, [] {
  if(MAXMDIM == 3) { current = &null_animation; return; }
  geom3::switch_fpp();
  View = cspin(0, 2, ticks / 5000.) * View;
  gamescreen();
  View = cspin(0, 2, -ticks / 5000.) * View;
  geom3::switch_fpp();
  }};

// more start animations:
// - fly a ghost around center, in Gans model
// - triangle edges?

EX startanim *current = &null_animation;

EX void pick() {
  if(((gold() > 0 || tkills() > 0) && canmove) || geometry != gNormal || ISWEB || ISMOBILE || vid.always3 || pmodel || rug::rugged || vid.wallmode < 2 || vid.monmode < 2 || glhr::noshaders || !vid.usingGL || !enabled) {
    current = &null_animation;
    return;
    }
  vector<startanim*> known = { &null_animation, &perspective, &joukowsky, &bandspin, &rug, &spin_around, &row_of_ghosts, &ghost_spiral, &army_of_ghosts, &fib_ghosts, &fpp };
  int id = rand() % 11;
  current = known[id];
  ticks_start = ticks;
  current->init();
  }

auto sanimhook = addHook(hooks_frame, 100, []() { if(add_to_frame) add_to_frame(); });

EX void display() {
  current->render();
  int z = vid.fsize/2 + 2;
  if(displayButtonS(4, vid.yres - 4 - z*3, VER, 0x202020, 0, vid.fsize/2))
    getcstat = SDLK_F5;
  if(displayButtonS(4, vid.yres - 4 - z*2, XLAT(current->name), 0x202020, 0, vid.fsize/2))
    getcstat = SDLK_F5;
  }

EX void explore() { exploration();  }
#endif

EX }
}
