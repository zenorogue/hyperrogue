// Hyperbolic Rogue -- rendering
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file drawing.cpp
 *  \brief Rendering shapes (dqi_draw), queue of shapes to render (ptds), etc.
 */

#include "hyper.h"
namespace hr {

#if HDR
static const int POLY_DRAWLINES = 1;            // draw the lines
static const int POLY_DRAWAREA = 2;             // draw the area
static const int POLY_INVERSE = 4;              // draw the inverse -- useful in stereographic projection
static const int POLY_ISSIDE = 8;               // never draw in inverse
static const int POLY_BEHIND = 16;              // there are points behind the camera
static const int POLY_TOOLARGE = 32;            // some coordinates are too large -- best not to draw to avoid glitches
static const int POLY_INFRONT = 64;             // on the sphere (orthogonal projection), do not draw without any points in front
static const int POLY_HASWALLS = 128;           // floor shapes which have their sidewalls
static const int POLY_PLAIN = 256;              // plain floors
static const int POLY_FULL = 512;               // full floors
static const int POLY_HASSHADOW = 1024;         // floor shapes which have their shadows, or can use shFloorShadow
static const int POLY_GP = 2048;                // Goldberg shapes
static const int POLY_VCONVEX = 4096;           // Convex shape (vertex)
static const int POLY_CCONVEX = 8192;           // Convex shape (central)
static const int POLY_CENTERIN = 16384;         // new system of side checking 
static const int POLY_FORCEWIDE = (1<<15);      // force wide lines
static const int POLY_NOTINFRONT = (1<<16);     // points not in front
static const int POLY_NIF_ERROR = (1<<17);      // points moved to the outline cross the image, disable
static const int POLY_BADCENTERIN = (1<<18);    // new system of side checking 
static const int POLY_PRECISE_WIDE = (1<<19);   // precise width calculation
static const int POLY_FORCE_INVERTED = (1<<20); // force inverted
static const int POLY_ALWAYS_IN = (1<<21);      // always draw this
static const int POLY_TRIANGLES = (1<<22);      // made of TRIANGLES, not TRIANGLE_FAN
static const int POLY_INTENSE = (1<<23);        // extra intense colors
static const int POLY_DEBUG = (1<<24);          // debug this shape

struct drawqueueitem {
  PPR prio;
  color_t color;
  int subprio;
  virtual void draw() = 0;
  virtual void draw_back() {}
  virtual void draw_pre() {}
  virtual ~drawqueueitem() {}
  void draw_darker();
  virtual color_t outline_group() = 0;
  };

struct dqi_poly : drawqueueitem {
  ld band_shift;
  transmatrix V;
  const vector<glvertex> *tab;
  int offset, cnt, offset_texture;
  color_t outline;
  double linewidth;
  int flags;
  struct basic_textureinfo *tinf;
  hyperpoint intester;
  void draw();
  void gldraw();
  void draw_back();
  virtual color_t outline_group() { return outline; }
  };

struct dqi_line : drawqueueitem {
  ld band_shift;
  hyperpoint H1, H2;
  int prf;
  double width;
  void draw();
  void draw_back();
  virtual color_t outline_group() { return color; }
  };
      
struct dqi_string : drawqueueitem {
  string str;
  int x, y, shift, size, frame;
  int align;
  void draw();
  virtual color_t outline_group() { return 1; }
  };

struct dqi_circle : drawqueueitem {
  int x, y, size, fillcolor;
  double linewidth;
  void draw();
  virtual color_t outline_group() { return 2; }
  };

struct dqi_action : drawqueueitem {
  reaction_t action;
  dqi_action(const reaction_t& a) : action(a) {}
  void draw() { action(); }
  virtual color_t outline_group() { return 2; }
  };
#endif

EX unsigned char& part(color_t& col, int i) {
  unsigned char* c = (unsigned char*) &col;
#if ISMOBILE
  return c[i];
#else
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  return c[sizeof(col) - 1 - i];
#else
  return c[i];
#endif
#endif
  }

bool fatborder;

EX color_t poly_outline;

EX vector<unique_ptr<drawqueueitem>> ptds;

#if CAP_GL
EX color_t text_color;
EX int text_shift;
EX GLuint text_texture;
EX int texts_merged;
EX int shapes_merged;

#if MINIMIZE_GL_CALLS
color_t triangle_color, line_color;
vector<glvertex> triangle_vertices;
vector<glvertex> line_vertices;
#endif

EX void glflush() {
  #if MINIMIZE_GL_CALLS
  if(isize(triangle_vertices)) {
    // printf("%08X %08X | %d shapes, %d/%d vertices\n", triangle_color, line_color, shapes_merged, isize(triangle_vertices), isize(line_vertices));
    if(triangle_color) {
      glhr::be_nontextured();
      glapplymatrix(Id);
      glhr::current_vertices = NULL;
      glhr::vertices(triangle_vertices);
      glhr::color2(triangle_color);
      glDrawArrays(GL_TRIANGLES, 0, isize(triangle_vertices));
      }
    triangle_vertices.clear();
    }
  if(isize(line_vertices)) {
    if(line_color) {
      glhr::be_nontextured();
      glapplymatrix(Id);
      glhr::current_vertices = NULL;
      glhr::vertices(line_vertices);
      glhr::color2(line_color);
      glDrawArrays(GL_LINES, 0, isize(line_vertices));
      }
    line_vertices.clear();
    }
  shapes_merged = 0;
  #endif

  if(isize(text_vertices)) {
    // printf("%08X | %d texts, %d vertices\n", text_color, texts_merged, isize(text_vertices));
    current_display->next_shader_flags = GF_TEXTURE;
    dynamicval<eModel> m(pmodel, mdPixel);
    if(!svg::in) current_display->set_all(0);
    glBindTexture(GL_TEXTURE_2D, text_texture);
    glhr::color2(text_color);
    glhr::set_depthtest(false);
    for(int ed = (current_display->stereo_active() && text_shift)?-1:0; ed<2; ed+=2) {
      glhr::set_modelview(glhr::translate(-ed*text_shift-current_display->xcenter,-current_display->ycenter, 0));
      current_display->set_mask(ed);
  
      glhr::current_vertices = NULL;
      glhr::prepare(text_vertices);
      glDrawArrays(GL_TRIANGLES, 0, isize(text_vertices));
      
      GLERR("print");
      }

    if(current_display->stereo_active() && text_shift && !svg::in) current_display->set_mask(0);
 
    texts_merged = 0;
    text_vertices.clear();
    }
  }
#endif

#if ISMOBILE==0
SDL_Surface *aux;
#endif

#if CAP_POLY
#if HDR
#define POLYMAX 60000
#endif

EX vector<glvertex> glcoords;

#endif

EX int spherespecial, spherephase;

#if CAP_POLY
int polyi;

EX int polyx[POLYMAX], polyxr[POLYMAX], polyy[POLYMAX];

int poly_flags;

void add1(const hyperpoint& H) {
  glcoords.push_back(glhr::pointtogl(H)); 
  }  

bool is_behind(const hyperpoint& H) {
  return pmodel == mdDisk && (hyperbolic ? H[2] >= 0 : true) && (nonisotropic ? false : vid.alpha + H[2] <= BEHIND_LIMIT);
  }

hyperpoint be_just_on_view(const hyperpoint& H1, const hyperpoint &H2) {
  // H1[2] * t + H2[2] * (1-t) == BEHIND_LIMIT - vid.alpha
  // H2[2]- BEHIND_LIMIT + vid.alpha = t * (H2[2] - H1[2])
  ld t = (H2[2] - BEHIND_LIMIT + vid.alpha) / (H2[2] - H1[2]);
  return H1 * t + H2 * (1-t);
  }

bool last_infront;

bool nif_error_in(ld x1, ld y1, ld x2, ld y2) {
  return pow(x1 * x2 + y2 * y2, 2) < (x1*x1+y1*y1)*(x2*x2+y2*y2)*.5;
  }

bool knowgood;
hyperpoint goodpoint;
vector<pair<int, hyperpoint>> tofix;

EX bool two_sided_model() {
  if(GDIM == 3) return false;
  if(pmodel == mdHyperboloid) return !euclid;
  // if(pmodel == mdHemisphere) return true;
  if(pmodel == mdDisk) return sphere;
  if(pmodel == mdHemisphere) return true;
  if(pmodel == mdRotatedHyperboles) return true;
  if(pmodel == mdSpiral && models::spiral_cone < 360) return true;
  return false;
  }

EX int get_side(const hyperpoint& H) {
  if(pmodel == mdDisk && sphere) {
    double curnorm = H[0]*H[0]+H[1]*H[1]+H[2]*H[2];
    double horizon = curnorm / vid.alpha;
    return (H[2] <= -horizon) ? -1 : 1;
    }
  if(pmodel == mdRotatedHyperboles)
    return H[1] > 0 ? -1 : 1;
  if(pmodel == mdHyperboloid && hyperbolic)
    return (models::sin_ball * H[2] > -models::cos_ball * H[1]) ? -1 : 1;
  if(pmodel == mdHyperboloid && sphere)
    return (models::sin_ball * H[2] > models::cos_ball * H[1]) ? -1 : 1;
  if(pmodel == mdHemisphere) {
    hyperpoint res;
    applymodel(H, res);
    return res[2] < 0 ? -1 : 1;
    }
  if(pmodel == mdSpiral && models::spiral_cone < 360) {    
    return cone_side(H);
    }
  return 0;
  }

EX bool correct_side(const hyperpoint& H) {
  return get_side(H) == spherespecial;
  }

hyperpoint Hlast;

void fixpoint(glvertex& hscr, hyperpoint H) {
  hyperpoint bad = H, good = goodpoint;

  for(int i=0; i<10; i++) {
    hyperpoint mid = midz(bad, good);
    if(correct_side(mid))
      good = mid;
    else
      bad = mid;
    }
  hyperpoint Hscr;
  applymodel(good, Hscr); 
  hscr = glhr::makevertex(Hscr[0]*current_display->radius, Hscr[1]*current_display->radius*vid.stretch, Hscr[2]*current_display->radius); 
  }

void addpoint(const hyperpoint& H) {
  if(true) {
    ld z = current_display->radius;
    // if(vid.alpha + H[2] <= BEHIND_LIMIT && pmodel == mdDisk) poly_flags |= POLY_BEHIND;
    
    if(spherespecial) {
      
      if(correct_side(H)) {
        poly_flags |= POLY_INFRONT, last_infront = false;
        if(!knowgood || (spherespecial > 0 ? H[2]>goodpoint[2] : H[2]<goodpoint[2])) goodpoint = H, knowgood = true;
        } 
      else if(sphere && (poly_flags & POLY_ISSIDE)) {
        double curnorm = H[0]*H[0]+H[1]*H[1]+H[2]*H[2];
        double horizon = curnorm / vid.alpha;
        poly_flags |= POLY_NOTINFRONT;
        if(last_infront && nif_error_in(glcoords.back()[0], glcoords.back()[1], H[0], H[1]))
          poly_flags |= POLY_NIF_ERROR;

        last_infront = true;
        
        z *=
          (sqrt(curnorm - horizon*horizon) / (vid.alpha - horizon)) / 
          (sqrt(curnorm - H[2]*H[2]) / (vid.alpha+H[2]));
        }
      else {
        poly_flags |= POLY_NOTINFRONT;
        tofix.push_back(make_pair(glcoords.size(), H));
        add1(H);
        return;
        }
      }
    hyperpoint Hscr;
    applymodel(H, Hscr); 
    if(sphere && pmodel == mdSpiral) {
      if(isize(glcoords)) {
        hyperpoint Hscr1;
        band_shift += 2 * M_PI;
        applymodel(H, Hscr1);
        if(hypot_d(2, Hlast-Hscr1) < hypot_d(2, Hlast-Hscr)) { Hscr = Hscr1; }
        band_shift -= 4 * M_PI;
        applymodel(H, Hscr1);
        if(hypot_d(2, Hlast-Hscr1) < hypot_d(2, Hlast-Hscr)) { Hscr = Hscr1; }
        band_shift += 2 * M_PI;
        }
      Hlast = Hscr;
      }
    if(GDIM == 2) {
      for(int i=0; i<3; i++) Hscr[i] *= z;
      Hscr[1] *= vid.stretch;
      }
    else {
      Hscr[0] *= z;
      Hscr[1] *= z * vid.stretch;
      Hscr[2] = 1 - 2 * (-Hscr[2] - models::clip_min) / (models::clip_max - models::clip_min);
      }
    add1(Hscr);
    }
  }

void coords_to_poly() {
  polyi = isize(glcoords);
  for(int i=0; i<polyi; i++) {
    if(!current_display->stereo_active()) glcoords[i][2] = 0;

    polyx[i]  = current_display->xcenter + glcoords[i][0] - glcoords[i][2]; 
    polyxr[i] = current_display->xcenter + glcoords[i][0] + glcoords[i][2]; 
    polyy[i]  = current_display->ycenter + glcoords[i][1];
    }
  }

void addpoly(const transmatrix& V, const vector<glvertex> &tab, int ofs, int cnt) {
  if(pmodel == mdPixel) {
    for(int i=ofs; i<ofs+cnt; i++) {
      hyperpoint h = glhr::gltopoint(tab[i]);
      h[3] = 1;
      h = V * h;
      add1(h);
      }
    return;
    }
  tofix.clear(); knowgood = false;
  hyperpoint last = V * glhr::gltopoint(tab[ofs]);
  bool last_behind = is_behind(last);
  if(!last_behind) addpoint(last);
  hyperpoint enter = C0;
  hyperpoint firstleave;
  int start_behind = last_behind ? 1 : 0;
  for(int i=ofs+1; i<ofs+cnt; i++) {
    hyperpoint curr = V*glhr::gltopoint(tab[i]);
    if(is_behind(curr) != last_behind) {
      hyperpoint h = be_just_on_view(last, curr);
      if(start_behind == 1) start_behind = 2, firstleave = h;
      if(!last_behind) enter = h;
      else if(h[0] * enter[0] + h[1] * enter[1] < 0) poly_flags |= POLY_BEHIND;
      addpoint(h);
      last_behind = !last_behind;
      }
    if(!last_behind) addpoint(curr);
    last = curr;
    }
  if(start_behind == 2) {
    if(firstleave[0] * enter[0] + firstleave[1] * enter[1] < 0) poly_flags |= POLY_BEHIND;
    else addpoint(firstleave);
    }
  if(knowgood && isize(tofix)) {
    
    if(true) {
      hyperpoint Hx = V * C0, Hy = goodpoint;
      for(int i=0; i<20; i++) {
        hyperpoint mid = midz(Hx, Hy);
        if(correct_side(mid)) Hy = mid;
        else Hx = mid;
        }
      goodpoint = midz(Hy, goodpoint);
      }
    
    for(auto& p: tofix)
      fixpoint(glcoords[p.first], p.second);
    /*
    hyperpoint Hscr;
    applymodel(goodpoint, Hscr); 
    glcoords.push_back(make_array<GLfloat>(Hscr[0]*current_display->radius+10, Hscr[1]*current_display->radius*vid.stretch, Hscr[2]*vid.radius)); 
    glcoords.push_back(make_array<GLfloat>(Hscr[0]*current_display->radius, Hscr[1]*current_display->radius*vid.stretch+10, Hscr[2]*vid.radius)); 
    glcoords.push_back(make_array<GLfloat>(Hscr[0]*current_display->radius-10, Hscr[1]*current_display->radius*vid.stretch, Hscr[2]*vid.radius)); 
    glcoords.push_back(make_array<GLfloat>(Hscr[0]*current_display->radius, Hscr[1]*current_display->radius*vid.stretch-10, Hscr[2]*vid.radius)); 
    glcoords.push_back(make_array<GLfloat>(Hscr[0]*current_display->radius+10, Hscr[1]*current_display->radius*vid.stretch, Hscr[2]*vid.radius));  */
    }
  }

#if CAP_SDLGFX
void aapolylineColor(SDL_Surface *s, int*x, int *y, int polyi, color_t col) {
  for(int i=1; i<polyi; i++)
    aalineColor(s, x[i-1], y[i-1], x[i], y[i], col);
  }

void polylineColor(SDL_Surface *s, int *x, int *y, int polyi, color_t col) {
  for(int i=1; i<polyi; i++)
    lineColor(s, x[i-1], y[i-1], x[i], y[i], col);
  }

EX void filledPolygonColorI(SDL_Surface *s, int* px, int *py, int polyi, color_t col) {
  std::vector<Sint16> spx(px, px + polyi);
  std::vector<Sint16> spy(py, py + polyi);
  filledPolygonColor(s, spx.data(), spy.data(), polyi, col);
  }
#endif

#if CAP_TEXTURE
void drawTexturedTriangle(SDL_Surface *s, int *px, int *py, glvertex *tv, color_t col) {
  transmatrix source = matrix3(
    px[0], px[1], px[2],
    py[0], py[1], py[2],
        1,      1,    1);
    

  transmatrix target = matrix3(
    tv[0][0], tv[1][0], tv[2][0], 
    tv[0][1], tv[1][1], tv[2][1],
           1,        1,        1
    );

  transmatrix isource = inverse(source);
  int minx = px[0], maxx = px[0];
  int miny = py[0], maxy = py[0];
  for(int i=1; i<3; i++)
    minx = min(minx, px[i]), maxx = max(maxx, px[i]),
    miny = min(miny, py[i]), maxy = max(maxy, py[i]);
  for(int mx=minx; mx<maxx; mx++)
  for(int my=miny; my<maxy; my++) {
    hyperpoint h = isource * point3(mx, my, 1);
    if(h[0] >= -1e-7 && h[1] >= -1e-7 && h[2] >= -1e-7) {
      hyperpoint ht = target * h;
      int tw = texture::config.data.twidth;
      int x = int(ht[0] * tw) & (tw-1);
      int y = int(ht[1] * tw) & (tw-1);
      color_t c = texture::config.data.texture_pixels[y * tw + x];
      auto& pix = qpixel(s, mx, my);
      for(int p=0; p<3; p++) {
        int alpha = part(c, 3) * part(col, 0);
        auto& v = part(pix, p);
        v = ((255*255 - alpha) * 255 * v + alpha * part(col, p+1) * part(c, p) + 255 * 255 * 255/2 + 1) / (255 * 255 * 255);
        }
      }
    }
  }
#endif

#if CAP_GL

EX int global_projection;

int min_slr, max_slr = 0;

#if MAXMDIM >= 4
extern renderbuffer *floor_textures;
#endif

void dqi_poly::gldraw() {
  auto& v = *tab;
  int ioffset = offset;
  
#if MINIMIZE_GL_CALLS  
  if(current_display->stereo_active() == 0 && !tinf && (color == 0 || ((flags & (POLY_VCONVEX | POLY_CCONVEX)) && !(flags & (POLY_INVERSE | POLY_FORCE_INVERTED))))) {
    if(color != triangle_color || outline != line_color || texts_merged) {
      glflush();
      triangle_color = color;
      line_color = outline;
      }
    shapes_merged++;

    if((flags & POLY_CCONVEX) && !(flags & POLY_VCONVEX)) {
      vector<glvertex> v2(cnt+1);
      for(int i=0; i<cnt+1; i++) v2[i] = glhr::pointtogl( V * glhr::gltopoint( v[offset+i-1] ) );
      if(color) for(int i=0; i<cnt; i++) triangle_vertices.push_back(v2[0]), triangle_vertices.push_back(v2[i]), triangle_vertices.push_back(v2[i+1]);
      for(int i=1; i<cnt; i++) line_vertices.push_back(v2[i]), line_vertices.push_back(v2[i+1]);
      }
    else {
      vector<glvertex> v2(cnt);
      for(int i=0; i<cnt; i++) v2[i] = glhr::pointtogl( V * glhr::gltopoint( v[offset+i] ) );
      if(color) for(int i=2; i<cnt-1; i++) triangle_vertices.push_back(v2[0]), triangle_vertices.push_back(v2[i-1]), triangle_vertices.push_back(v2[i]);
      for(int i=1; i<cnt; i++) line_vertices.push_back(v2[i-1]), line_vertices.push_back(v2[i]);
      }
    return;
    }
  else glflush();
#endif
  
  if(tinf) {
    glhr::be_textured();
    glBindTexture(GL_TEXTURE_2D, tinf->texture_id);
    glhr::vertices_texture(v, tinf->tvertices, offset, offset_texture);
    ioffset = 0;
    }
  else { 
    glhr::be_nontextured();
    glhr::vertices(v);
    }
  
  next_slr:

  for(int ed = current_display->stereo_active() ? -1 : 0; ed<2; ed+=2) {
    if(global_projection && global_projection != ed) continue;
    current_display->set_all(ed);
    bool draw = color;
    
    flagtype sp = get_shader_flags();
    
    if(sp & SF_DIRECT) {
      if((sp & SF_BAND) && V[2][2] > 1e8) continue;
      glapplymatrix(V);
      }

    if(draw) {
      if(flags & POLY_TRIANGLES) {
        glhr::color2(color, (flags & POLY_INTENSE) ? 2 : 1);
        glhr::set_depthtest(model_needs_depth() && prio < PPR::SUPERLINE);
        glhr::set_depthwrite(model_needs_depth() && prio != PPR::TRANSPARENT_SHADOW && prio != PPR::EUCLIDEAN_SKY);
        glhr::set_fogbase(prio == PPR::SKY ? 1.0 + (euclid ? 20 : 5 / sightranges[geometry]) : 1.0);
        glDrawArrays(GL_TRIANGLES, ioffset, cnt);
        }
      else {
        glEnable(GL_STENCIL_TEST);
  
        glColorMask( GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE );
        glhr::set_depthtest(false);
        glStencilOp( GL_INVERT, GL_INVERT, GL_INVERT);
        glStencilFunc( GL_ALWAYS, 0x1, 0x1 );
        glhr::color2(0xFFFFFFFF);
        glDrawArrays(tinf ? GL_TRIANGLES : GL_TRIANGLE_FAN, offset, cnt);
        
        current_display->set_mask(ed);
        glhr::color2(color);
        glhr::set_depthtest(model_needs_depth() && prio < PPR::SUPERLINE);
        glhr::set_depthwrite(model_needs_depth() && prio != PPR::TRANSPARENT_SHADOW && prio != PPR::EUCLIDEAN_SKY);
        glhr::set_fogbase(prio == PPR::SKY ? 1.0 + (euclid ? 20 : 5 / sightranges[geometry]) : 1.0);
  
        if(flags & (POLY_INVERSE | POLY_FORCE_INVERTED)) {
          glStencilOp( GL_ZERO, GL_ZERO, GL_ZERO);
          glStencilFunc( GL_NOTEQUAL, 1, 1);
          GLfloat xx = vid.xres;
          GLfloat yy = vid.yres;
          vector<glvertex> scr = {
            glhr::makevertex(-xx, -yy, 0), 
            glhr::makevertex(+xx, -yy, 0), 
            glhr::makevertex(+xx, +yy, 0), 
            glhr::makevertex(-xx, +yy, 0)
            };
          glhr::vertices(scr);
          glhr::id_modelview();
          glDrawArrays(tinf ? GL_TRIANGLES : GL_TRIANGLE_FAN, 0, 4);
          glhr::vertices(v);
          if(sp & SF_DIRECT) glapplymatrix(V);
          }
        else { 
          glStencilOp( GL_ZERO, GL_ZERO, GL_ZERO);
          glStencilFunc( GL_EQUAL, 1, 1);
          glDrawArrays(tinf ? GL_TRIANGLES : GL_TRIANGLE_FAN, offset, cnt);
          }
        
        glDisable(GL_STENCIL_TEST);
        }
      }
    
    if(outline && !(flags & POLY_TRIANGLES)) {
      glhr::color2(outline);
      glhr::set_depthtest(model_needs_depth() && prio < PPR::SUPERLINE);
      glhr::set_depthwrite(model_needs_depth() && prio != PPR::TRANSPARENT_SHADOW && prio != PPR::EUCLIDEAN_SKY);
      glhr::set_fogbase(prio == PPR::SKY ? 1.0 + (euclid ? 20 : 5 / sightranges[geometry]) : 1.0);
      glDrawArrays(GL_LINE_STRIP, offset, cnt);
      }
    }

  if(min_slr < max_slr) {
    min_slr++;
    glhr::set_index_sl(M_PI * min_slr);
    goto next_slr;
    }
  }
#endif

EX ld scale_at(const transmatrix& T) {
  if(GDIM == 3 && pmodel == mdPerspective) return 1 / abs((tC0(T))[2]);
  if(sol) return 1;
  hyperpoint h1, h2, h3;
  applymodel(tC0(T), h1);
  applymodel(T * xpush0(.01), h2);
  applymodel(T * ypush(.01) * C0, h3);
  return sqrt(hypot_d(2, h2-h1) * hypot_d(2, h3-h1) / .0001);
  }

EX ld linewidthat(const hyperpoint& h) {
  if(!(vid.antialias & AA_LINEWIDTH)) return 1;
  else if(hyperbolic && pmodel == mdDisk && vid.alpha == 1 && !ISWEB) {
    double dz = h[LDIM];
    if(dz < 1) return 1;
    else {
      double dx = sqrt(dz * dz - 1);
      double dfc = dx/(dz+1);
      dfc = 1 - dfc*dfc;
      return dfc;
      }
    }
  else if(svg::in || inHighQual) {
    hyperpoint h0 = h / zlevel(h);
    transmatrix T = rgpushxto0(h0);
    return scale_at(T);
    }
  return 1;
  }

EX void set_width(ld w) {
  #if MINIMIZE_GL_CALLS
  if(w != glhr::current_linewidth) glflush();
  #endif
  glhr::set_linewidth(w);
  }

// this part makes cylindrical projections on the sphere work

namespace cyl {

int loop_min = 0, loop_max = 0;
vector<ld> periods;

ld period_at(ld y) {
  
  ld m = current_display->radius;
  y /= (m * vid.stretch);
  
  switch(pmodel) {
    case mdBand:
      return m * 4;
    case mdSinusoidal:
      return m * 2 * cos(y * M_PI);
    case mdMollweide:
      return m * 2 * sqrt(1 - y*y*4);
    case mdCollignon: {
      if(vid.collignon_reflected && y > 0) y = -y;
      y += signed_sqrt(vid.collignon_parameter);
      return abs(m*y*2/1.2);
      }
    default:
      return m * 2;
    }     
  }

void adjust(bool tinf) {

  periods.resize(isize(glcoords));
  
  if(!models::model_straight)
    for(auto& g: glcoords)
      models::apply_orientation(g[0], g[1]);

  for(int i = 0; i<isize(glcoords); i++) periods[i] = period_at(glcoords[i][1]);  
    
  auto dist = [] (ld a, ld b) { return max(b, a-b); };
  
  ld chypot = hypot(dist(vid.xres, current_display->xcenter), dist(vid.yres, current_display->ycenter));
  
  ld cmin = -chypot/2, cmax = chypot/2, dmin = -chypot, dmax = chypot;
  
  ld z = vid.stretch * current_display->radius;

  switch(pmodel) {
    case mdSinusoidal: case mdBandEquidistant: case mdMollweide:
      dmax = z/2, dmin = -dmax;
      break;

    case mdBandEquiarea:
      dmax = z/M_PI, dmin = -dmax;
      break;

    case mdCollignon:      
      dmin = z * (signed_sqrt(vid.collignon_parameter - 1) - signed_sqrt(vid.collignon_parameter));      
      if(vid.collignon_reflected) dmax = -dmin;
      else dmax = z * (signed_sqrt(vid.collignon_parameter + 1) - signed_sqrt(vid.collignon_parameter));
      break;
    
    default: ;
    }

  bool had = false;
  ld first, next;
    
  for(int i = 0; i<isize(glcoords); i++) if(periods[i] > 1) {
    if(!had) {
      next = first = glcoords[i][0] / periods[i];
      had = true;
      }
    else {
      glcoords[i][0] /= periods[i];
      glcoords[i][0] -= round_nearest(glcoords[i][0]-next);
      next = glcoords[i][0];
      glcoords[i][0] *= periods[i];
      }
    loop_min = min<int>(loop_min, floor((cmin - glcoords[i][0]) / periods[i]));
    loop_max = max<int>(loop_max, ceil((cmax - glcoords[i][0]) / periods[i]));
    }
  
  if(!had) return;
  
  ld last = first - round_nearest(first-next);
  
  if(loop_max > 100) loop_max = 100;
  if(loop_min < -100) loop_min = -100;
    
  if(abs(first - last) < 1e-6) {
    if(!models::model_straight)
      for(auto& g: glcoords)
        models::apply_orientation(g[1], g[0]);
    }
  else {
    if(tinf) { 
      // this cannot work after cycled
      loop_min = 1; loop_max = 0; return; 
      }
    if(last < first) {
      reverse(glcoords.begin(), glcoords.end());
      reverse(periods.begin(), periods.end());
      swap(first, last);
      }

    for(int i=0; i<isize(glcoords); i++) glcoords[i][0] += periods[i] * loop_min;

    int base = isize(glcoords);

    for(int i=loop_min; i<loop_max; i++) {
      for(int j=0; j<base; j++) {
        glcoords.push_back(glcoords[isize(glcoords)-base]);
        glcoords.back()[0] += periods[j];
        }
      }
    glcoords.push_back(glcoords.back());
    glcoords.push_back(glcoords[0]);
    for(int u=1; u<=2; u++) {
      auto& v = glcoords[isize(glcoords)-u][1];
      v = v < 0 ? dmin : dmax;
      }
    if(!models::model_straight)
      for(auto& g: glcoords)
        models::apply_orientation(g[1], g[0]);
    // we have already looped
    loop_min = loop_max = 0;  
    }
  }
}
  
bool in_twopoint = false;

ld glhypot2(glvertex a, glvertex b) { 
  return (a[0]-b[0]) * (a[0]-b[0]) + (a[1]-b[1]) * (a[1]-b[1]) + (a[2]-b[2]) * (a[2]-b[2]);
  }

void compute_side_by_centerin(dqi_poly *p, bool& nofill) {

  hyperpoint hscr;
  hyperpoint h1 = p->V * p->intester;
  if(is_behind(h1)) {
    if(sphere) {
      for(int i=0; i<3; i++) h1[i] = -h1[i];
      poly_flags &= ~POLY_CENTERIN;
      }
    else
      nofill = true; 
    }
  applymodel(h1, hscr); hscr[0] *= current_display->radius; hscr[1] *= current_display->radius * vid.stretch;
  for(int i=0; i<isize(glcoords)-1; i++) {
    double x1 = glcoords[i][0] - hscr[0];
    double y1 = glcoords[i][1] - hscr[1];
    double x2 = glcoords[i+1][0] - hscr[0];
    double y2 = glcoords[i+1][1] - hscr[1];
    if(asign(y1, y2)) {
      ld x = xcross(x1, y1, x2, y2);
      if(x < -1e-6) poly_flags ^= POLY_CENTERIN;
      else if (x < 1e-6) nofill = true;
      }
    }
  
  poly_flags &= ~POLY_INVERSE;
  if(poly_flags & POLY_CENTERIN) {
    poly_flags |= POLY_INVERSE;
    if(abs(zlevel(tC0(p->V)) - 1) > 1e-6) nofill = true;

    /* nofill = true;
    outline = (flags & POLY_CENTERIN) ? 0x00FF00FF : 0xFF0000FF;
    addpoint(hscr); */
    }
  
  /*
  if(poly_flags & POLY_BADCENTERIN) {
    glcoords.push_back(glhr::makevertex(hscr[0]+10, hscr[1]*vid.stretch, hscr[2]));
    glcoords.push_back(glhr::makevertex(hscr[0], hscr[1]*vid.stretch+10, hscr[2]));
    glcoords.push_back(glhr::makevertex(hscr[0]-10, hscr[1]*vid.stretch, hscr[2]));
    glcoords.push_back(glhr::makevertex(hscr[0], hscr[1]*vid.stretch-10, hscr[2]));
    glcoords.push_back(glhr::makevertex(hscr[0]+10, hscr[1]*vid.stretch, hscr[2]));
    } */
  }

void compute_side_by_area() {

  double rarea = 0;
  for(int i=0; i<isize(glcoords)-1; i++) 
    rarea += glcoords[i][0] * glcoords[i+1][1] - glcoords[i][1] * glcoords[i+1][0];
  rarea += glcoords.back()[0] * glcoords[0][1] - glcoords.back()[1] * glcoords[0][0];

  if(rarea>0)
    poly_flags ^= POLY_INVERSE;
  }

ld get_width(dqi_poly* p) {
  if((p->flags & POLY_FORCEWIDE) || pmodel == mdPixel)
    return p->linewidth;
  else if(p->flags & POLY_PRECISE_WIDE) {
    ld maxwidth = 0;
    for(int i=0; i<p->cnt; i++) {
      hyperpoint h1 = p->V * glhr::gltopoint((*p->tab)[p->offset+i]);
      maxwidth = max(maxwidth, linewidthat(h1));
      }
    return maxwidth * p->linewidth;
    }
  else
    return linewidthat(tC0(p->V)) * p->linewidth;
  }

void debug_this() { }

glvertex junk = glhr::makevertex(0,0,1);

EX namespace s2xe {

  int maxgen;
  bool with_zero;
  ld minz, maxy, miny;
  
  typedef array<ld, 5> pt;

  basic_textureinfo stinf;  

  pt lerp(const pt& h0, const pt& h1, ld x) {
    pt s;
    for(int i=0; i<5; i++) s[i] = h0[i] + (h1[i]-h0[i]) * x;
    return s;
    }
  
  void add2(pt h, int gen) {
    glcoords.push_back(glhr::pointtogl(point31(sin(h[0]) * (h[1] + 2 * M_PI * gen), cos(h[0]) * (h[1] + 2 * M_PI * gen), h[2])));
    stinf.tvertices.push_back(glhr::makevertex(h[3], h[4], 0));
    }

  void addall(pt h0, pt h1, pt h2) {
    for(int gen=-maxgen; gen <= maxgen; gen++) if(gen || with_zero) {
      add2(h0, gen);
      add2(h1, gen);
      add2(h2, gen);
      }
    }
  
  void draw_s2xe0(dqi_poly *p);
  
  bool to_right(const pt& h2, const pt& h1) {
    ld x2 = h2[0];
    ld x1 = h1[0];
    if(x2 < x1) x2 += 2 * M_PI;
    return x2 >= x2 && x2 <= x1 + M_PI;
    }
  
  EX int qrings = 32;
  
  ld seg() { return 2 * M_PI / qrings; }
  
  void add_ortho_triangle(pt bl, pt tl, pt br, pt tr) {
    
    auto sg = seg();
    
    int s0 = ceil(bl[0] / sg);
    int s1 = floor(br[0] / sg);
    
    pt bat[1000], tat[1000];
      
    bat[0] = bl; tat[0] = tl;
    
    int s = 1;
    
    for(int i = s0; i <= s1; i++) {
      ld f = (i*sg-bl[0]) / (br[0]-bl[0]);

      bat[s] = lerp(bl, br, f);
      tat[s] = lerp(tl, tr, f);
      
      s++;      
      }

    bat[s] = br; tat[s] = tr;
    
    while(s--) {
      addall(bat[s], bat[s+1], tat[s+1]);
      addall(bat[s], tat[s+1], tat[s]);
      }
    }
  
  void add_ordered_triangle(array<pt, 3> v) {
    if(v[1][0] < v[0][0]) v[1][0] += 2 * M_PI;
    if(v[2][0] < v[1][0]) v[2][0] += 2 * M_PI;
    if(v[2][0] - v[0][0] < 1e-6) return;
    ld x = (v[1][0] - v[0][0]) / (v[2][0] - v[0][0]);
    
    if(v[2][0] < v[0][0] + M_PI / 4 && maxy < M_PI - M_PI/4 && sightranges[geometry] <= 5) {
      addall(v[0], v[1], v[2]);
      return;
      }

    auto mv = lerp(v[0], v[2], x);
  
    add_ortho_triangle(v[0], v[0], mv, v[1]);
    add_ortho_triangle(mv, v[1], v[2], v[2]);
    
    /*
    int zl = floor(v[1][0] / seg());
    int zr = ceil(v[1][0] / seg());
    if(zl < zr && zl * seg > v[0][0] && zr * seg < v[2][0]) {

      ld fl = (zl*seg-v[0][0]) / (v[2][0]-v[0][0]);
      ld fr = (zr*seg-v[0][0]) / (v[2][0]-v[0][0]);
      
      addall(lerp(v[0], v[2], fl), v[1], lerp(v[0], v[2], fr));
      }
    */
    
    // add_ortho_triangle(v[0], tv[0], v[1], tv[1], v[2], tv[2], v[2], tv[2]);
    }

  void add_triangle_around(array<pt, 3> v) {
    ld baseheight = (v[0][1] > M_PI/2) ? M_PI : 0;
    ld tu = (v[0][3] + v[1][3] + v[2][3]) / 3;
    ld tv = (v[0][4] + v[1][4] + v[2][4]) / 3;
    array<pt, 3> vhigh;
    for(int i=0; i<3; i++) { vhigh[i] = v[i]; vhigh[i][1] = baseheight; vhigh[i][3] = tu; vhigh[i][4] = tv; }
    if(v[1][0] < v[0][0]) v[1][0] = v[1][0] + 2 * M_PI, vhigh[1][0] = vhigh[1][0] + 2 * M_PI;
    add_ortho_triangle(v[0], vhigh[0], v[1], vhigh[1]);
    if(v[2][0] < v[1][0]) v[2][0] = v[2][0] + 2 * M_PI, vhigh[2][0] = vhigh[2][0] + 2 * M_PI;
    add_ortho_triangle(v[1], vhigh[1], v[2], vhigh[2]);
    if(v[0][0] < v[2][0]) v[0][0] = v[0][0] + 2 * M_PI, vhigh[0][0] = vhigh[0][0] + 2 * M_PI;
    add_ortho_triangle(v[2], vhigh[2], v[0], vhigh[0]);
    }

  void add_s2xe_triangle(array<pt, 3> v) {
    bool r0 = to_right(v[1], v[0]);
    bool r1 = to_right(v[2], v[1]);
    bool r2 = to_right(v[0], v[2]);
    
    minz = min(abs(v[0][2]), max(abs(v[1][2]), abs(v[2][2])));
    auto& s = sightranges[geometry];
    maxgen = sqrt(s * s - minz * minz) / (2 * M_PI) + 1;
    
    maxy = max(v[0][1], max(v[1][1], v[2][1]));
    miny = min(v[0][1], min(v[1][1], v[2][1]));
    with_zero = true;
    if(maxy < M_PI / 4) {
      add2(v[0], 0);
      add2(v[1], 0);
      add2(v[2], 0);
      with_zero = false;
      }
     
    rotated:
    if(r0 && r1 && r2) {
      add_triangle_around(v);
      }
    else if(r0 && r1) {
      add_ordered_triangle(v);
      }
    else if(r2 && !r0 && !r1) {
      add_ordered_triangle(make_array(v[2], v[1], v[0]));
      }
    else if(!r0 && !r1 && !r2) {
      add_triangle_around(make_array(v[2], v[1], v[0]));
      }
    else {
      tie(r0, r1, r2) = make_tuple(r1, r2, r0);
      tie(v[0], v[1], v[2]) = make_tuple(v[1], v[2], v[0]);
      goto rotated;
      }
    }
  
void draw_s2xe(dqi_poly *p) {
  if(!p->cnt) return;
  if(p->flags & POLY_TRIANGLES) {
    dqi_poly npoly = *p;
    npoly.offset = 0;
    npoly.tab = &glcoords;
    if(p->tinf) {
      npoly.tinf = p->tinf ? &stinf : NULL;
      npoly.offset_texture = 0;
      stinf.texture_id = p->tinf->texture_id;
      }
    else {
      npoly.tinf = NULL;
      }
    npoly.V = Id;
    set_width(1);
    glcoords.clear();
    stinf.tvertices.clear();
    for(int i=0; i<p->cnt; i+=3) {
      array<pt, 3> v;
      for(int k=0; k<3; k++) {
        hyperpoint h = p->V * glhr::gltopoint( (*p->tab)[p->offset+i+k]);
        v[k][2] = hypot_d(3, h);

        auto dp = product_decompose(h);
        v[k][2] = dp.first;
        v[k][0] = atan2(h[0], h[1]);
        v[k][1] = acos_auto_clamp(dp.second[2]);
        if(p->tinf) {
          auto& tv = p->tinf->tvertices[p->offset_texture+i+k];
          v[k][3] = tv[0];
          v[k][4] = tv[1];
          }
        }
      add_s2xe_triangle(v);
      }
    npoly.cnt = isize(glcoords);
    npoly.gldraw();        
    }
  else draw_s2xe0(p);
  }

struct point_data {
  hyperpoint direction;
  ld distance;
  ld z;
  int bad;
  };

void draw_s2xe0(dqi_poly *p) {
  if(!p->cnt) return;
  dqi_poly npoly = *p;
  npoly.offset = 0;
  npoly.tab = &glcoords;
  npoly.V = Id;
  npoly.flags &= ~ (POLY_INVERSE | POLY_FORCE_INVERTED);
  set_width(1);
  glcoords.clear();

  int maxgen = sightranges[geometry] / (2 * M_PI) + 1;
  
  auto crossdot = [&] (const hyperpoint h1, const hyperpoint h2) { return make_pair(h1[0] * h2[1] - h1[1] * h2[0], h1[0] * h2[0] + h1[1] * h2[1]); };
  vector<point_data> pd;
  for(int i=0; i<p->cnt; i++) {
    hyperpoint h = p->V * glhr::gltopoint( (*p->tab)[p->offset+i]);
    pd.emplace_back();
    auto& next = pd.back();
    auto dp = product_decompose(h);
    next.direction = dp.second;
    next.z = dp.first;
    // next.tpoint = p->tinf ? p->tinf->tvertices[p->offset+i] : glvertex();
    ld hyp = hypot_d(2, next.direction);
    next.distance = acos_auto_clamp(next.direction[2]);
    if(hyp == 0) {
      next.direction = point2(1, 0);
      }
    else {
      next.direction[0] /= hyp;
      next.direction[1] /= hyp;
      }
    if(next.distance < 1e-3) next.bad = 1;
    else if(next.distance > M_PI - 1e-3) next.bad = 2;
    else next.bad = 0;
    }
  
  glcoords.resize(p->cnt);
  if(p->flags & POLY_TRIANGLES) {
    for(int i=0; i<p->cnt; i+=3) {
      int nbad = 0;
      for(int j=i; j<i+3; j++) {
        if(pd[j].bad >= 1) nbad = max(nbad, 0);
        if(pd[j].bad >= 2) nbad = 2;
        }
      
      int g = (pd[i].distance > M_PI/2 || pd[i+1].distance >= M_PI/2 || pd[i+2].distance >= M_PI/2) ? 2 : 1;
        
      auto c1 = crossdot(pd[i+0].direction, pd[i+1].direction);
      auto c2 = crossdot(pd[i+1].direction, pd[i+2].direction);
      auto c3 = crossdot(pd[i+2].direction, pd[i+0].direction);
      if(c1.second < 0 || c2.second < 0 || c3.second < 0) nbad = max(nbad, g);
      if(c1.first > 0 && c2.first > 0 && c3.first > 0) nbad = max(nbad, g);
      if(c1.first < 0 && c2.first < 0 && c3.first < 0) nbad = max(nbad, g);
      pd[i].bad = pd[i+1].bad = pd[i+2].bad = nbad;
      }
    
    for(int gen=-maxgen; gen<=maxgen; gen++) {
      for(int i=0; i<p->cnt; i++) {
        auto& cur = pd[i];
        if(cur.bad >= (gen ? 1 : 2)) glcoords[i] = junk;
        else {
          hyperpoint h;
          ld d = cur.distance + 2 * M_PI * gen;
          h[0] = cur.direction[0] * d;
          h[1] = cur.direction[1] * d;
          h[2] = cur.z;
          glcoords[i] = glhr::pointtogl(h);
          }
        }
      npoly.gldraw();
      }
    }
  else {
    for(auto c: pd) if(c.bad == 2) return;
    bool no_gens = false;
    for(int i=0; i<p->cnt; i++) {
      auto &c1 = pd[i];
      auto &c0 = pd[i==0?p->cnt-1 : i-1];
      if(c1.distance > M_PI/2 && c0.distance > M_PI/2 && crossdot(c0.direction, c1.direction).second < 0) return;
      if(c1.bad == 2) return;
      if(c1.bad == 1) no_gens = true;
      }
    
    if(!no_gens) {

      vector<ld> angles(p->cnt);
      for(int i=0; i<p->cnt; i++) {
        angles[i] = atan2(pd[i].direction[1], pd[i].direction[0]);
        }
      sort(angles.begin(), angles.end());
      angles.push_back(angles[0] + 2 * M_PI);
      bool ok = false;
      for(int i=1; i<isize(angles); i++)
        if(angles[i] >= angles[i-1] + M_PI) ok = true;
      if(!ok) {
        for(auto &c: pd) if(c.distance > M_PI/2) return;
        no_gens = true;
        }
      }
    
    int g = no_gens ? 0 : maxgen;

    for(int gen=-g; gen<=g; gen++) {
      for(int i=0; i<p->cnt; i++) {
        auto& cur = pd[i];
        ld d = cur.distance + 2 * M_PI * gen;
        hyperpoint h;
        h[0] = cur.direction[0] * d;
        h[1] = cur.direction[1] * d;
        h[2] = cur.z;
        glcoords[i] = glhr::pointtogl(h);
        }
      npoly.gldraw();
      }
    }
  }
EX }

EX namespace ods {
#if CAP_ODS
  
  EX bool project(hyperpoint h, hyperpoint& h1, hyperpoint& h2, bool eye) {
    ld tanalpha = tan_auto(vid.ipd/2);
    if(eye) tanalpha = -tanalpha;
    if(!sphere) tanalpha = -tanalpha;

    ld& x = h[0];
    ld z = -h[1];
    ld y = -h[2];
    ld& t = h[3];

    ld y02 = (x*x + y*y - tanalpha*tanalpha*t*t);
    if(y02 < 0) return false;
    ld y0 = sqrt(y02);
    ld theta = atan(z / y0);
  
    for(int i=0; i<2; i++) {
      hyperpoint& h = (i ? h1 : h2);
      if(i == 1) theta = -theta, y0 = -y0;
        
      ld x0 = t * tanalpha;
      
      ld phi = atan2(y, x) - atan2(y0, x0) + M_PI;
      
      // ld delta = euclid ? hypot(y0,z) : atan2_auto(z / sin(theta), t / cos_auto(vid.ipd/2));
      ld p = z / sin(theta) / t * cos_auto(vid.ipd / 2);
      ld delta = (p > 1) ? 13 : (p < -1) ? -13 : atanh(p);
      
      if(euclid || hyperbolic) phi -= M_PI;
      if(hyperbolic) delta = -delta;
      
      h[0] = phi;
      h[1] = theta;
      h[2] = delta;
      if(euclid || hyperbolic) h[1] = -theta;
      }
    
    return true;
    }
  
  void draw_ods(dqi_poly *p) {
    auto& stinf = s2xe::stinf;

    if(!p->cnt) return;
    if(!(p->flags & POLY_TRIANGLES)) return;

    dqi_poly npoly = *p;
    npoly.offset = 0;
    npoly.tab = &glcoords;
    npoly.V = Id;
    npoly.tinf = p->tinf ? &stinf : NULL;
    if(npoly.tinf) {
      npoly.offset_texture = 0;
      stinf.texture_id = p->tinf->texture_id;
      stinf.tvertices.clear();
      }
    npoly.V = Id;
    glcoords.clear();

    array<hyperpoint, 6> h;

    if(0) for(int i=0; i<p->cnt; i+=3) {
      for(int j=0; j<3; j++)
        h[j] = p->V * glhr::gltopoint((*p->tab)[p->offset+i+j]);

      for(int j=0; j<3; j++) {
        glcoords.push_back(glhr::makevertex(h[j][0], h[j][1], h[j][2]));
        if(npoly.tinf) stinf.tvertices.push_back(p->tinf->tvertices[i+j]);
        }
      }

    if(1) for(int i=0; i<p->cnt; i+=3) {

      for(int j=0; j<3; j++) {
        hyperpoint o = p->V * glhr::gltopoint((*p->tab)[p->offset+i+j]);
        if(!project(o, h[j], h[j+3], global_projection == -1))
          goto next_i;
        }
      
      for(int j=0; j<6; j++) {
        // let Delta be from 0 to 2PI
        if(h[j][2]<0) h[j][2] += 2 * M_PI;
        // Theta is from -PI/2 to PI/2. Let it be from 0 to PI
        h[j][1] += global_projection * M_PI/2;
        h[j][3] = 1;
        }

      /* natsph here */
      
      if(h[0][2] < 0) swap(h[0], h[3]);
      if(h[1][2] < 0) swap(h[1], h[4]);
      if(h[2][2] < 0) swap(h[2], h[5]);

      cyclefix(h[0][0], 0);
      cyclefix(h[1][0], h[0][0]);
      cyclefix(h[2][0], h[0][0]);
      cyclefix(h[3][0], 0);
      cyclefix(h[4][0], h[3][0]);
      cyclefix(h[5][0], h[3][0]);

      if(abs(h[1][1] - h[0][1]) > M_PI/2) goto next_i;
      if(abs(h[2][1] - h[0][1]) > M_PI/2) goto next_i;
      
      if(h[0][0] < -M_PI || h[0][0] > M_PI) println(hlog, h[0][0]);

      if(1) {
        int fst = 0, lst = 0;
        if(h[1][0] < -M_PI || h[2][0] < -M_PI) lst++;
        if(h[1][0] > +M_PI || h[2][0] > +M_PI) fst--;
        for(int x=fst; x<=lst; x++) for(int j=0; j<3; j++) {
          glcoords.push_back(glhr::makevertex(h[j][0] + 2 * M_PI * x, h[j][1], h[j][2]));
          if(npoly.tinf) stinf.tvertices.push_back(p->tinf->tvertices[p->offset_texture+i+j]);
          }
        }
      
      /* natsph here */
      
      next_i: ;
      }

    npoly.cnt = isize(glcoords);
    // npoly.color = 0xFFFFFFFF;
    npoly.gldraw();
    }
#endif
  EX }

void dqi_poly::draw() {
  if(flags & POLY_DEBUG) debug_this();
  
  #if CAP_ODS  
  if(vid.stereo_mode == sODS) {
    ods::draw_ods(this);
    return;
    }
  #endif

  if(in_s2xe() && vid.usingGL && pmodel == mdPerspective && (current_display->set_all(global_projection), (get_shader_flags() & SF_DIRECT))) {
    s2xe::draw_s2xe(this);
    return;
    }

  dynamicval<ld> bs(hr::band_shift, band_shift);
  if(!hyperbolic && among(pmodel, mdPolygonal, mdPolynomial)) {
    bool any = false;
    for(int i=0; i<cnt; i++) {
      hyperpoint h1 = V * glhr::gltopoint((*tab)[offset+i]);
      if(h1[2] > 0) any = true;
      }
    if(!any) return;
    }
  
  if(sphere && tinf && GDIM == 2 && cnt > 3) {
    int i = cnt;
    cnt = 3;
    for(int j=0; j<i; j+=3) {
      offset += j;
      offset_texture += j;
      draw();
      offset -= j;
      offset_texture -= j;
      }
    cnt = i;
    return;
    }

  if(sphere && pmodel == mdTwoPoint && !in_twopoint) {
    #define MAX_PHASE 4
    vector<glvertex> phases[MAX_PHASE];
    extern int twopoint_sphere_flips;
    extern bool twopoint_do_flips;
    int pha;
    if(twopoint_do_flips) {
      for(int i=0; i<cnt; i++) {
        hyperpoint h1 = V * glhr::gltopoint((*tab)[offset+i]);
        for(int j=0; j<MAX_PHASE; j++) {
          twopoint_sphere_flips = j;
          hyperpoint h2; applymodel(h1, h2);
          glvertex h = glhr::pointtogl(h2 * current_display->radius); h[1] *= vid.stretch;
          if(i == 0)
            phases[j].push_back(h);
          else {
            int best = -1;
            ld bhypot = 1e60;
            for(int j0=0; j0<MAX_PHASE; j0++)
              if(isize(phases[j0]) == i) {
                ld chypot = glhypot2(phases[j0].back(), h);
                if(chypot < bhypot || best == -1) bhypot = chypot, best = j0;
                }
            phases[best].push_back(h);
            }
          }
        }
      twopoint_sphere_flips = 0;
      pha = MAX_PHASE-1;
      }
    else {
      pha = 1;
      if(true) {
        // a
        // b
        // lin(a,b) is of form (x, 0, z)
        int cpha = 0;
        for(int i=0; i<cnt; i++) {

          hyperpoint h1 = V * glhr::gltopoint((*tab)[offset+i]);
          hyperpoint mh1; applymodel(h1, mh1); mh1[1] *= vid.stretch;
          phases[cpha].push_back(glhr::pointtogl(mh1 * current_display->radius));

          // check if the i-th edge intersects the boundary of the ellipse
          // (which corresponds to the segment between the antipodes of foci)
          // if yes, switch cpha to the opposite
          hyperpoint h2 = V * glhr::gltopoint((*tab)[offset+(i+1)%cnt]);
          
          hyperpoint ah1 = h1, ah2 = h2;
          models::apply_orientation(ah1[0], ah1[1]);
          models::apply_orientation(ah2[0], ah2[1]);
          if(ah1[1] * ah2[1] > 0) continue;
          ld c1 = ah1[1], c2 = -ah2[1];
          if(c1 < 0) c1 = -c1, c2 = -c2;
          hyperpoint h = ah1 * c1 + ah2 * c2;
          h /= hypot_d(3, h);
          if(h[2] < 0 && abs(h[0]) < sin(vid.twopoint_param)) cpha = 1-cpha, pha = 2;
          }
        if(cpha == 1) pha = 0;
        }
      }
    dynamicval<eModel> d1(pmodel, mdPixel);
    dynamicval<transmatrix> d2(V, Id);
    dynamicval<int> d3(offset, 0);
    dynamicval<decltype(tab)> d4(tab, tab);
    for(int j=0; j<pha; j++) {
      dynamicval<int> d5(cnt, isize(phases[j]));
      tab = &phases[j];
      draw();
      }
    return;
    }
  
  /* if(spherespecial && prio == PPR::MOBILE_ARROW) {
    if(spherephase == 0) return;
    dynamicval<int> ss(spherespecial, 0);
    draw();
    return;
    } */

#if CAP_GL
  if(vid.usingGL && (current_display->set_all(global_projection), get_shader_flags() & SF_DIRECT)) {
    if(sl2 && pmodel == mdGeodesic) {
      ld z = atan2(V[2][3], V[3][3]);
      auto zr = sightranges[geometry];
      min_slr = ceil((-zr - z) / M_PI);
      max_slr = floor((zr - z) / M_PI);
      if(min_slr > max_slr) return;
      glhr::set_index_sl(M_PI * min_slr);
      }
    set_width(get_width(this));
    flags &= ~POLY_INVERSE;
    gldraw();
    return;
    }
#endif
  
  glcoords.clear();
  poly_flags = flags;
  
  double d = 0, curradius = 0;
  if(sphere) {
    d = det(V);
    curradius = pow(abs(d), 1/3.);
    }
  
  /* outline = 0x80808080;
  color = 0; */
  
  last_infront = false;
  
  addpoly(V, *tab, offset, cnt);
  if(!(sphere && vid.alpha < .9)) if(pmodel != mdJoukowsky) if(!(flags & POLY_ALWAYS_IN)) for(int i=1; i<isize(glcoords); i++) {
    ld dx = glcoords[i][0] - glcoords[i-1][0];
    ld dy = glcoords[i][1] - glcoords[i-1][1];
    if(dx > vid.xres * 2 || dy > vid.yres * 2) return;
    }
  if(poly_flags & POLY_BEHIND) return;
  if(isize(glcoords) <= 1) return;
  
  cyl::loop_min = cyl::loop_max = 0;
  if(sphere && mdBandAny())
    cyl::adjust(tinf);
    
  int poly_limit = max(vid.xres, vid.yres) * 2;
  

  if(0) for(auto& p: glcoords) {
    if(abs(p[0]) > poly_limit || abs(p[1]) > poly_limit)
      return; // too large!
    }
  
  bool equi = mdAzimuthalEqui() || pmodel == mdFisheye;

  bool nofill = false;

  if(poly_flags & POLY_NIF_ERROR) return;
  
  if(spherespecial == 1 && sphere && (poly_flags & POLY_INFRONT) && (poly_flags & POLY_NOTINFRONT) && vid.alpha <= 1) {
    bool around_center = false;
    for(int i=0; i<isize(glcoords)-1; i++) {
      double x1 = glcoords[i][0];
      double y1 = glcoords[i][1];
      double x2 = glcoords[i+1][0];
      double y2 = glcoords[i+1][1];
      if(asign(y1, y2)) {
        ld x = xcross(x1, y1, x2, y2);
        if(x < -1e-6) around_center = !around_center;
        }
      }
    if(around_center) return;
    }
  
  bool can_have_inverse = false;  
  if(sphere && pmodel == mdDisk && (spherespecial > 0 || equi)) can_have_inverse = true;
  if(pmodel == mdJoukowsky) can_have_inverse = true;
  if(pmodel == mdJoukowskyInverted && vid.skiprope) can_have_inverse = true;
  if(pmodel == mdDisk && hyperbolic && vid.alpha <= -1) can_have_inverse = true;
  if(pmodel == mdSpiral && vid.skiprope) can_have_inverse = true;
  if(pmodel == mdCentralInversion) can_have_inverse = true;
    
  if(can_have_inverse && !(poly_flags & POLY_ISSIDE)) {
  
    if(!tinf) 
      compute_side_by_centerin(this, nofill);
        
    else {
      if(d < 0) poly_flags ^= POLY_INVERSE;  
      if(pmodel == mdCentralInversion) poly_flags ^= POLY_INVERSE;
      compute_side_by_area();
      }
    
    if(poly_flags & POLY_INVERSE) {
      if(curradius < vid.alpha - 1e-6) return;
      if(!sphere) return;
      }
    
    }
  else poly_flags &=~ POLY_INVERSE;
  
  if(spherespecial) {
    if(!hiliteclick && !(poly_flags & POLY_INFRONT)) return;
    }
    
  int lastl = 0;

  for(int l=cyl::loop_min; l <= cyl::loop_max; l++) {
  
    if(l || lastl) { 
      for(int i=0; i<isize(glcoords); i++) {
        glcoords[i][0] += models::ocos * cyl::periods[i] * (l - lastl);
        glcoords[i][1] += models::osin * cyl::periods[i] * (l - lastl);
        }
      lastl = l;
      }
    
    if(equi && (poly_flags & POLY_INVERSE)) {
      if(abs(zlevel(V * C0) - 1) < 1e-6 && !tinf) {
        // we should fill the other side
        ld h = atan2(glcoords[0][0], glcoords[0][1]);
        for(int i=0; i<=360; i++) {
          ld a = i * degree + h;
          glcoords.push_back(glhr::makevertex(current_display->radius * sin(a), current_display->radius * vid.stretch * cos(a), 0));
          }
        poly_flags ^= POLY_INVERSE;
        }
      else {
        // If we are on a zlevel, the algorithm above will not work correctly.
        // It is hard to tell what to do in this case. Just fill neither side
        nofill = true;
        }
      }
  
  #if CAP_GL
    if(vid.usingGL) {
      poly_flags &= ~(POLY_VCONVEX | POLY_CCONVEX);
      // if(pmodel == 0) for(int i=0; i<qglcoords; i++) glcoords[i][2] = current_display->scrdist;
      if(tinf && (poly_flags & POLY_INVERSE)) {
        return; 
        }
      set_width(get_width(this));
      dqi_poly npoly = (*this);
      npoly.V = Id;
      npoly.tab = &glcoords;
      npoly.offset = 0;
      npoly.cnt = isize(glcoords);
      if(nofill) npoly.color = 0, npoly.tinf = NULL;
      npoly.flags = poly_flags;
      npoly.gldraw();
      continue;
      }
  #endif
  
  #if CAP_SVG==1
    if(svg::in) {
      coords_to_poly();
      color_t col = color;
      if(poly_flags & POLY_INVERSE) col = 0;
      svg::polygon(polyx, polyy, polyi, col, outline, get_width(this));
      continue;
      }
  #endif
  
    coords_to_poly();
  
  #if CAP_XGD==1
    gdpush(1); gdpush(color); gdpush(outline); gdpush(polyi);
    for(int i=0; i<polyi; i++) gdpush(polyx[i]), gdpush(polyy[i]);
  #elif CAP_SDLGFX==1
  
    if(tinf) {
      #if CAP_TEXTURE
      if(!(poly_flags & POLY_INVERSE))
        for(int i=0; i<polyi; i += 3)
          drawTexturedTriangle(s, polyx+i, polyy+i, &tinf->tvertices[offset_texture + i], color);
      #endif
      }
    else if(poly_flags & POLY_INVERSE) {
      int i = polyi;
      if(true) {
        polyx[i] = 0; polyy[i] = 0; i++;
        polyx[i] = vid.xres; polyy[i] = 0; i++;
        polyx[i] = vid.xres; polyy[i] = vid.yres; i++;
        polyx[i] = 0; polyy[i] = vid.yres; i++;
        polyx[i] = 0; polyy[i] = 0; i++;
        }
      filledPolygonColorI(s, polyx, polyy, polyi+5, color);
      }
    else  
      filledPolygonColorI(s, polyx, polyy, polyi, color);
  
    if(current_display->stereo_active()) filledPolygonColorI(aux, polyxr, polyy, polyi, color);
    
    ((vid.antialias & AA_NOGL) ?aapolylineColor:polylineColor)(s, polyx, polyy, polyi, outline);
    if(current_display->stereo_active()) aapolylineColor(aux, polyxr, polyy, polyi, outline);
    
    if(vid.xres >= 2000 || fatborder) {
      int xmi = 3000, xma = -3000;
      for(int t=0; t<polyi; t++) xmi = min(xmi, polyx[t]), xma = max(xma, polyx[t]);
      
      if(xma > xmi + 20) for(int x=-1; x<2; x++) for(int y=-1; y<=2; y++) if(x*x+y*y == 1) {
        for(int t=0; t<polyi; t++) polyx[t] += x, polyy[t] += y;
        aapolylineColor(s, polyx, polyy, polyi, outline);
        for(int t=0; t<polyi; t++) polyx[t] -= x, polyy[t] -= y;
        }
      }
  #endif
    }
  }

vector<glvertex> prettylinepoints;

EX void prettypoint(const hyperpoint& h) {
  prettylinepoints.push_back(glhr::pointtogl(h));
  }

EX void prettylinesub(const hyperpoint& h1, const hyperpoint& h2, int lev) {
  if(lev >= 0 && pmodel != mdPixel) {
    hyperpoint h3 = midz(h1, h2);
    prettylinesub(h1, h3, lev-1);
    prettylinesub(h3, h2, lev-1);
    }
  else prettypoint(h2);
  }

EX void prettyline(hyperpoint h1, hyperpoint h2, color_t col, int lev, int flags, PPR prio) {
  prettylinepoints.clear();
  prettypoint(h1);
  prettylinesub(h1, h2, lev);
  dqi_poly ptd;
  ptd.V = Id;
  ptd.band_shift = band_shift;
  ptd.tab = &prettylinepoints;
  ptd.offset = 0;
  ptd.cnt = isize(prettylinepoints);
  ptd.linewidth = vid.linewidth;
  ptd.color = 0;
  ptd.outline = col;
  ptd.flags = POLY_ISSIDE | POLY_PRECISE_WIDE | flags;
  ptd.tinf = NULL;
  ptd.intester = C0;
  ptd.prio = prio;
  ptd.draw();
  }

EX void prettypoly(const vector<hyperpoint>& t, color_t fillcol, color_t linecol, int lev) {
  prettylinepoints.clear();
  prettypoint(t[0]);
  for(int i=0; i<isize(t); i++)
    prettylinesub(t[i], t[(i+1)%3], lev);
  dqi_poly ptd;
  ptd.V = Id;
  ptd.band_shift = band_shift;
  ptd.tab = &prettylinepoints;
  ptd.offset = 0;
  ptd.cnt = isize(prettylinepoints);
  ptd.linewidth = vid.linewidth;
  ptd.color = fillcol;
  ptd.outline = linecol;
  ptd.flags = POLY_ISSIDE | POLY_PRECISE_WIDE;
  ptd.tinf = NULL;
  ptd.intester = C0;
  ptd.draw();
  }
  
vector<glvertex> curvedata;
int curvestart = 0;
bool keep_curvedata = false;

EX void queuereset(eModel m, PPR prio) {
  queueaction(prio, [m] () { glflush(); pmodel = m; });
  }

void dqi_line::draw() {
  dynamicval<ld> d(vid.linewidth, width); 
  dynamicval<ld> bs(hr::band_shift, band_shift);
  prettyline(H1, H2, color, prf, 0, prio);
  }

void dqi_string::draw() {
  #if CAP_SVG
  if(svg::in) {
    svg::text(x, y, size, str, frame, color, align);
    return;
    }
  #endif
  #if ISMOBILE==0
  int fr = frame & 255;
  displayfrSP(x, y, shift, fr, size, str, color, align, frame >> 8);
  #else
  displayfr(x, y, frame, size, str, color, align);
  #endif
  }

void dqi_circle::draw() {
  #if CAP_SVG
  if(svg::in) {
    svg::circle(x, y, size, color, fillcolor, linewidth);
    }
  else
  #endif
  drawCircle(x, y, size, color, fillcolor);
  }
        
EX void initquickqueue() {
  ptds.clear();
  poly_outline = OUTLINE_NONE;
  }

EX void sortquickqueue() {
  for(int i=1; i<isize(ptds);)
    if(i && ptds[i]->prio < ptds[i-1]->prio) {
      swap(ptds[i], ptds[i-1]);
      i--;
      }
    else i++;
  }

EX void quickqueue() {
  current_display->next_shader_flags = 0;
  spherespecial = 0; 
  reset_projection(); current_display->set_all(0);
  int siz = isize(ptds);
  for(int i=0; i<siz; i++) ptds[i]->draw();
  ptds.clear();
  if(!keep_curvedata) {
    curvedata.clear();
    curvestart = 0;
    }
  }

ld xintval(const hyperpoint& h) {
  if(sphereflipped()) return -h[2];
  return -intval(h, C0);
  }

EX ld backbrightness = .25;

purehookset hook_drawqueue;

constexpr int PMAX = int(PPR::MAX);
int qp[PMAX], qp0[PMAX];

color_t darken_color(color_t& color, bool outline) {
  int alpha = color & 255;
  if(sphere && pmodel == mdDisk && vid.alpha <= 1)
    return 0;
  else {
    if(outline && alpha < 255) 
      return color - alpha + int(backbrightness * alpha);
    else
      return (gradient(modelcolor>>8, color>>8, 0, backbrightness, 1)<<8) | 0xFF;
    }
  }

void dqi_poly::draw_back() { 
  dynamicval<color_t> dvo(outline, darken_color(outline, true));
  dynamicval<color_t> dvc(color, darken_color(color, false));
  draw();
  }

void dqi_line::draw_back() { 
  dynamicval<color_t> dvc(color, darken_color(color, true));
  draw();
  }

EX void sort_drawqueue() {
  
  for(int a=0; a<PMAX; a++) qp[a] = 0;
  
  int siz = isize(ptds);

  #if MINIMIZE_GL_CALLS
  unordered_map<color_t, vector<unique_ptr<drawqueueitem>>> subqueue;
  for(auto& p: ptds) subqueue[(p->prio == PPR::CIRCLE || p->prio == PPR::OUTCIRCLE) ? 0 : p->outline_group()].push_back(move(p));
  ptds.clear();
  for(auto& p: subqueue) for(auto& r: p.second) ptds.push_back(move(r));
  subqueue.clear();
  for(auto& p: ptds) subqueue[(p->prio == PPR::CIRCLE || p->prio == PPR::OUTCIRCLE) ? 0 : p->color].push_back(move(p));
  ptds.clear();
  for(auto& p: subqueue) for(auto& r: p.second) ptds.push_back(move(r));
  #endif
    
  for(auto& p: ptds) {
    int pd = p->prio - PPR::ZERO;
    if(pd < 0 || pd >= PMAX) {
      printf("Illegal priority %d\n", pd);
      p->prio = PPR(rand() % int(PPR::MAX));
      }
    qp[pd]++;
    }
  
  int total = 0;
  for(int a=0; a<PMAX; a++) {
    int b = qp[a];
    qp0[a] = qp[a] = total; total += b;
    }

  vector<unique_ptr<drawqueueitem>> ptds2;  
  ptds2.resize(siz);
  
  for(int i = 0; i<siz; i++) ptds2[qp[int(ptds[i]->prio)]++] = move(ptds[i]);
  swap(ptds, ptds2);
  }

EX void reverse_priority(PPR p) {
  reverse(ptds.begin()+qp0[int(p)], ptds.begin()+qp[int(p)]);
  }

EX void reverse_side_priorities() {
  for(PPR p: {PPR::REDWALLs, PPR::REDWALLs2, PPR::REDWALLs3, PPR::WALL3s,
    PPR::LAKEWALL, PPR::INLAKEWALL, PPR::BELOWBOTTOM})
      reverse_priority(p);
  }

// on the sphere, parts on the back are drawn first
EX void draw_backside() {
  if(pmodel == mdHyperboloid && hyperbolic) {
    dynamicval<eModel> dv (pmodel, mdHyperboloidFlat);
    for(auto& ptd: ptds) 
      if(!among(ptd->prio, PPR::MOBILE_ARROW, PPR::OUTCIRCLE, PPR::CIRCLE))
        ptd->draw();
    }

  spherespecial = sphereflipped() ? 1 : -1;
  reset_projection();
  
  if(pmodel == mdRotatedHyperboles) {
    for(auto& ptd: ptds)
      if(!among(ptd->prio, PPR::MOBILE_ARROW, PPR::OUTCIRCLE, PPR::CIRCLE))
        ptd->draw();
    glflush();
    }
  else {
    reverse_side_priorities();
    for(int i=ptds.size()-1; i>=0; i--) 
      if(!among(ptds[i]->prio, PPR::MOBILE_ARROW, PPR::OUTCIRCLE, PPR::CIRCLE))
        ptds[i]->draw_back();
    
    glflush();
    reverse_side_priorities();
    }

  spherespecial *= -1;
  spherephase = 1;
  reset_projection();
  }

extern bool lshiftclick, lctrlclick;

EX void reverse_transparent_walls() {
  int pt = int(PPR::TRANSPARENT_WALL);
  reverse(&ptds[qp0[pt]], &ptds[qp[pt]]);
  }

EX void draw_main() {
  if(sphere && GDIM == 3 && pmodel == mdPerspective) {
    for(int p: {1, 0, 2, 3}) {
      if(elliptic && p < 2) continue;
      glhr::set_depthwrite(true);
      if(p == 0 || p == 3) {
  #ifdef GL_ES
        glClearDepthf(1.0f);
  #else
        glClearDepth(1.0f);
  #endif
        glDepthFunc(GL_LEQUAL);
        }
      else {
  #ifdef GL_ES
        glClearDepthf(0.0f);
  #else
        glClearDepth(0.0f);
  #endif
        glDepthFunc(GL_GEQUAL);
        }
      glClear(GL_DEPTH_BUFFER_BIT);
      glhr::be_nontextured();
      spherephase = p;
      reset_projection();
      for(auto& ptd: ptds) ptd->draw();
      if(elliptic) {
        spherephase = p | 4;
        reset_projection();
        for(auto& ptd: ptds) ptd->draw();
        }
      // glflush();
      }
    }
  else {
    for(auto& ptd: ptds) if(ptd->prio == PPR::OUTCIRCLE)
      ptd->draw();
    
    if(two_sided_model()) draw_backside();
  
    for(auto& ptd: ptds) if(ptd->prio != PPR::OUTCIRCLE) {
      dynamicval<int> ss(spherespecial, among(ptd->prio, PPR::MOBILE_ARROW, PPR::OUTCIRCLE, PPR::CIRCLE) ? 0 : spherespecial);
      ptd->draw();
      }
    glflush();
    }
  }

#if CAP_VR
EX hookset<bool()> *hooks_vr_draw_all;
#endif
  
EX void drawqueue() {
  callhooks(hook_drawqueue);
  current_display->next_shader_flags = 0;
  reset_projection();
  // reset_projection() is not sufficient here, because we need to know shaderside_projection

#if CAP_GL
  if(vid.usingGL) 
    glClear(GL_STENCIL_BUFFER_BIT);
#endif
  
  profile_start(3);
  
  sort_drawqueue();
  
  for(PPR p: {PPR::REDWALLs, PPR::REDWALLs2, PPR::REDWALLs3, PPR::WALL3s,
    PPR::LAKEWALL, PPR::INLAKEWALL, PPR::BELOWBOTTOM}) 
  if(GDIM == 2) sort(&ptds[qp0[int(p)]], &ptds[qp[int(p)]], 
    [] (const unique_ptr<drawqueueitem>& p1, const unique_ptr<drawqueueitem>& p2) {
      auto ap1 = (dqi_poly&) *p1;
      auto ap2 = (dqi_poly&) *p2;
      return xintval(ap1.V * xpush0(.1))
        < xintval(ap2.V * xpush0(.1));
      });

  for(PPR p: {PPR::TRANSPARENT_WALL})
    sort(&ptds[qp0[int(p)]], &ptds[qp[int(p)]], 
      [] (const unique_ptr<drawqueueitem>& p1, const unique_ptr<drawqueueitem>& p2) {
        return p1->subprio > p2->subprio;
        });

  profile_stop(3);

#if CAP_SDL
  if(current_display->stereo_active() && !vid.usingGL) {

    if(aux && (aux->w != s->w || aux->h != s->h))
      SDL_FreeSurface(aux);
  
    if(!aux) {
      aux = SDL_CreateRGBSurface(SDL_SWSURFACE,s->w,s->h,32,0,0,0,0);
      }

    // SDL_LockSurface(aux);
    // memset(aux->pixels, 0, vid.xres * vid.yres * 4);
    // SDL_UnlockSurface(aux);
    SDL_BlitSurface(s, NULL, aux, NULL);
    }
#endif

  spherespecial = 0;
  spherephase = 0;
  reset_projection();
  
  #if CAP_VR
  if(callhandlers(false, hooks_vr)) {} else
  #endif
  if(model_needs_depth() && current_display->stereo_active()) {
    global_projection = -1;
    draw_main();
    glClear(GL_DEPTH_BUFFER_BIT);
    global_projection = +1;
    draw_main();
    global_projection = 0;
    }
  else {
    draw_main();
    }    

#if CAP_SDL
  if(vid.stereo_mode == sAnaglyph && !vid.usingGL) {
    int qty = s->w * s->h;
    int *a = (int*) s->pixels;
    int *b = (int*) aux->pixels;
    SDL_LockSurface(aux);
    while(qty) {
      *a = ((*a) & 0xFF0000) | ((*b) & 0x00FFFF);
      a++; b++; qty--;
      }
    SDL_UnlockSurface(aux);
    }

  if(vid.stereo_mode == sLR && !vid.usingGL) {
    SDL_LockSurface(aux);
    for(int y=0; y<vid.yres; y++)
    for(int x=vid.xres/2; x<vid.xres; x++)
      qpixel(s,x,y) = qpixel(aux,x,y);
    SDL_UnlockSurface(aux);
    }
#endif

  if(!keep_curvedata) {
    curvedata.clear(); curvestart = 0;
    }
  }

#if HDR
template<class T, class... U> T& queuea(PPR prio, U... u) {
  ptds.push_back(unique_ptr<T>(new T (u...)));
  ptds.back()->prio = prio;  
  return (T&) *ptds.back();
  }
#endif

#if CAP_SHAPES
EX dqi_poly& queuepolyat(const transmatrix& V, const hpcshape& h, color_t col, PPR prio) {
  if(prio == PPR::DEFAULT) prio = h.prio;

  auto& ptd = queuea<dqi_poly> (prio);

  ptd.V = V;
  ptd.band_shift = band_shift;
  ptd.offset = h.s;
  ptd.cnt = h.e-h.s;
  ptd.tab = &cgi.ourshape;
  if(cblind) {
    // protanopia
    /* int r = (56 * part(col,3) + 43 * part(col,2)) / 100;
    int g = (58 * part(col,3) + 42 * part(col,2)) / 100;
    int b = (24 * part(col,2) + 75 * part(col,1)) / 100; */
    // deuteranopia
    /* int r = (625 * part(col,3) + 375 * part(col,2)) / 1000;
    int g = (700 * part(col,3) + 300 * part(col,2)) / 1000;
    int b = (300 * part(col,2) + 700 * part(col,1)) / 1000; 
    part(col,3) = r;
    part(col,2) = g;
    part(col,1) = b; */
    part(col,2) = part(col,3) = (part(col,2) * 2 + part(col,3) + 1)/3;
    }
  ptd.color = (darkened(col >> 8) << 8) + (col & 0xFF);
  ptd.outline = poly_outline;
  ptd.linewidth = vid.linewidth;
  ptd.flags = h.flags;
  ptd.tinf = h.tinf;
  ptd.offset_texture = h.texture_offset;
  ptd.intester = h.intester;
  return ptd;
  }
#endif

void addfloats(vector<GLfloat>& v, hyperpoint h) {
  for(int i=0; i<3; i++) v.push_back(h[i]);
  }

EX dqi_poly& queuetable(const transmatrix& V, const vector<glvertex>& f, int cnt, color_t linecol, color_t fillcol, PPR prio) {
 
  auto& ptd = queuea<dqi_poly> (prio);

  ptd.V = V;
  ptd.band_shift = band_shift;
  ptd.tab = &f;
  ptd.offset = 0;
  ptd.cnt = cnt;
  ptd.color = fillcol;
  ptd.outline = linecol;
  ptd.linewidth = vid.linewidth;
  ptd.flags = POLY_ISSIDE | POLY_PRECISE_WIDE;
  ptd.tinf = NULL;
  ptd.intester = C0;
  return ptd;
  }

#if CAP_SHAPES
EX dqi_poly& queuepoly(const transmatrix& V, const hpcshape& h, color_t col) {
  return queuepolyat(V,h,col,h.prio);
  }

void queuepolyb(const transmatrix& V, const hpcshape& h, color_t col, int b) {
  queuepolyat(V,h,col,h.prio+b);
  }
#endif

EX void curvepoint(const hyperpoint& H1) {
  curvedata.push_back(glhr::pointtogl(H1));
  }

EX dqi_poly& queuecurve(color_t linecol, color_t fillcol, PPR prio) {
  auto &res = queuetable(Id, curvedata, isize(curvedata)-curvestart, linecol, fillcol, prio);
  res.offset = curvestart;
  curvestart = isize(curvedata);
  return res;
  }

EX dqi_action& queueaction(PPR prio, const reaction_t& action) {
  return queuea<dqi_action> (prio, action);
  }

EX dqi_line& queueline(const hyperpoint& H1, const hyperpoint& H2, color_t col, int prf IS(0), PPR prio IS(PPR::LINE)) {
  auto& ptd = queuea<dqi_line> (prio);

  ptd.H1 = H1;
  ptd.H2 = H2;
  ptd.band_shift = band_shift;
  ptd.prf = prf;
  ptd.width = vid.linewidth;
  ptd.color = (darkened(col >> 8) << 8) + (col & 0xFF);
  
  return ptd;
  }

EX void queuestr(int x, int y, int shift, int size, string str, color_t col, int frame IS(0), int align IS(8)) {
  auto& ptd = queuea<dqi_string> (PPR::TEXT);
  ptd.x = x;
  ptd.y = y;
  ptd.str = str;
  ptd.align = align;
  ptd.shift = shift;
  ptd.size = size;
  ptd.color = darkened(col);
  ptd.frame = frame ? ((poly_outline & ~ 255)+frame) : 0;
  }

EX void queuechr(int x, int y, int shift, int size, char chr, color_t col, int frame IS(0), int align IS(8)) {
  auto& ptd = queuea<dqi_string> (PPR::TEXT);
  ptd.x = x;
  ptd.y = y;
  ptd.str = chr;
  ptd.shift = shift;
  ptd.size = size;
  ptd.align = align;
  ptd.color = col;
  ptd.frame = frame ? (poly_outline & ~ 255) : 0;
  }

EX void queuecircle(int x, int y, int size, color_t color, PPR prio IS(PPR::CIRCLE), color_t fillcolor IS(0)) {
  auto& ptd = queuea<dqi_circle>(prio);
  ptd.x = x;
  ptd.y = y;
  ptd.size = size;
  ptd.color = color;
  ptd.fillcolor = fillcolor;
  ptd.linewidth = vid.linewidth;
  }

EX void getcoord0(const hyperpoint& h, int& xc, int &yc, int &sc) {
  hyperpoint hscr;
  applymodel(h, hscr);
  xc = current_display->xcenter + current_display->radius * hscr[0];
  yc = current_display->ycenter + current_display->radius * vid.stretch * hscr[1];
  sc = 0;
  // EYETODO sc = vid.eye * current_display->radius * hscr[2];
  }

EX ld scale_in_pixels(const transmatrix& V) {
  return scale_at(V) * cgi.scalefactor * current_display->radius / 2.5;
  }

EX bool getcoord0_checked(const hyperpoint& h, int& xc, int &yc, int &zc) {
  if(invalid_point(h)) return false;
  if(point_behind(h)) return false;
  getcoord0(h, xc, yc, zc);
  return true;
  }

EX void queuechr(const hyperpoint& h, int size, char chr, color_t col, int frame IS(0)) {
  int xc, yc, sc;
  if(getcoord0_checked(h, xc, yc, sc))
    queuechr(xc, yc, sc, size, chr, col, frame);
  }

EX void queuechr(const transmatrix& V, double size, char chr, color_t col, int frame IS(0)) {
  int xc, yc, sc; 
  if(getcoord0_checked(tC0(V), xc, yc, sc))
    queuechr(xc, yc, sc, scale_in_pixels(V) * size, chr, col, frame);
  }
  
EX void queuestr(const hyperpoint& h, int size, const string& chr, color_t col, int frame IS(0)) {
  int xc, yc, sc; 
  if(getcoord0_checked(h, xc, yc, sc))
    queuestr(xc, yc, sc, size, chr, col, frame);
  }
  
EX void queuestr(const transmatrix& V, double size, const string& chr, color_t col, int frame IS(0), int align IS(8)) {
  int xc, yc, sc; 
  if(getcoord0_checked(tC0(V), xc, yc, sc))
    queuestr(xc, yc, sc, scale_in_pixels(V) * size, chr, col, frame, align);
  }
  
EX void queuecircle(const transmatrix& V, double size, color_t col) {
  int xc, yc, sc; 
  if(!getcoord0_checked(tC0(V), xc, yc, sc)) return;
  int xs, ys, ss; getcoord0(V * xpush0(.01), xs, ys, ss);  
  queuecircle(xc, yc, scale_in_pixels(V) * size, col);
  }

#endif

}
