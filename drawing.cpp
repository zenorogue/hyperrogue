
// implementation of the rendering queue

// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

unsigned char& part(color_t& col, int i) {
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

color_t poly_outline;

// #define STLSORT

vector<unique_ptr<drawqueueitem>> ptds;

#if CAP_GL
color_t text_color;
int text_shift;
GLuint text_texture;
int texts_merged;
int shapes_merged;

vector<glhr::textured_vertex> text_vertices;

#if MINIMIZE_GL_CALLS
color_t triangle_color, line_color;
vector<glvertex> triangle_vertices;
vector<glvertex> line_vertices;
void glapplymatrix(const transmatrix& V);
#endif

void glflush() {
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
    glhr::be_textured();
    dynamicval<eModel> pm(pmodel, mdUnchanged);
    if(!svg::in) current_display->set_all(0);
    glBindTexture(GL_TEXTURE_2D, text_texture);
    glhr::color2(text_color);
    glhr::set_depthtest(false);
    for(int ed = (current_display->stereo_active() && text_shift)?-1:0; ed<2; ed+=2) {
      glhr::set_modelview(glhr::translate(-ed*text_shift-current_display->xcenter,-current_display->ycenter, current_display->scrdist));
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
#define POLYMAX 60000

vector<glvertex> glcoords;

#endif

int spherespecial, spherephase;

#if CAP_POLY
int polyi;

int polyx[POLYMAX], polyxr[POLYMAX], polyy[POLYMAX];

int poly_flags;

void add1(const hyperpoint& H) {
  glcoords.push_back(glhr::pointtogl(H)); 
  }  

bool is_behind(const hyperpoint& H) {
  return pmodel == mdDisk && (hyperbolic ? H[2] >= 0 : true) && vid.alpha + H[2] <= BEHIND_LIMIT;
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

bool two_sided_model() {
  if(DIM == 3) return false;
  if(pmodel == mdHyperboloid) return !euclid;
  // if(pmodel == mdHemisphere) return true;
  if(pmodel == mdDisk) return sphere;
  if(pmodel == mdHemisphere) return true;
  if(pmodel == mdRotatedHyperboles) return true;
  if(pmodel == mdSpiral && conformal::spiral_cone < 360) return true;
  return false;
  }

int get_side(const hyperpoint& H) {
  if(pmodel == mdDisk && sphere) {
    double curnorm = H[0]*H[0]+H[1]*H[1]+H[2]*H[2];
    double horizon = curnorm / vid.alpha;
    return (H[2] <= -horizon) ? -1 : 1;
    }
  if(pmodel == mdRotatedHyperboles)
    return H[1] > 0 ? -1 : 1;
  if(pmodel == mdHyperboloid && hyperbolic)
    return (conformal::sin_ball * H[2] > -conformal::cos_ball * H[1]) ? -1 : 1;
  if(pmodel == mdHyperboloid && sphere)
    return (conformal::sin_ball * H[2] > conformal::cos_ball * H[1]) ? -1 : 1;
  if(pmodel == mdHemisphere) {
    hyperpoint res;
    applymodel(H, res);
    return res[2] < 0 ? -1 : 1;
    }
  if(pmodel == mdSpiral && conformal::spiral_cone < 360) {    
    return cone_side(H);
    }
  return 0;
  }

bool correct_side(const hyperpoint& H) {
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
    if(DIM == 2) {
      for(int i=0; i<3; i++) Hscr[i] *= z;
      Hscr[1] *= vid.stretch;
      }
    else {
      Hscr[0] *= z;
      Hscr[1] *= z * vid.stretch;
      Hscr[2] = 1 - 2 * (-Hscr[2] - conformal::clip_min) / (conformal::clip_max - conformal::clip_min);
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
  if(pmodel == mdFlatten) {
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

void filledPolygonColorI(SDL_Surface *s, int* px, int *py, int polyi, color_t col) {
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

void glapplymatrix(const transmatrix& V) {
  GLfloat mat[16];
  int id = 0;

  if(in_perspective() && DIM == 3) {
    if(spherephase & 4) {
      for(int y=0; y<4; y++) {
        for(int x=0; x<4; x++) mat[id++] = -V[x][y];
        }
      }
    else {
      for(int y=0; y<4; y++) {
        for(int x=0; x<4; x++) mat[id++] = V[x][y];
        }
      }
    glhr::set_modelview(glhr::as_glmatrix(mat));
    return;
    }
  
  if(DIM == 3) {
    for(int y=0; y<4; y++) 
      for(int x=0; x<4; x++) mat[id++] = V[x][y];
    }
  else {
    for(int y=0; y<3; y++) {
      for(int x=0; x<3; x++) mat[id++] = V[x][y];
      mat[id++] = 0;
      }
    mat[12] = 0;
    mat[13] = 0;
    if(glhr::current_shader_projection != glhr::shader_projection::band)
      mat[14] = GLfloat(vid.alpha);
    else
      mat[14] = 0;
    mat[15] = 1;  
    }
  
  if(vid.stretch != 1) mat[1] *= vid.stretch, mat[5] *= vid.stretch, mat[9] *= vid.stretch, mat[13] *= vid.stretch;
  
  if(conformal::model_has_orientation()) {
    if(DIM == 3) for(int a=0; a<4; a++) 
      conformal::apply_orientation_yz(mat[a*4+1], mat[a*4+2]);
    for(int a=0; a<4; a++) 
      conformal::apply_orientation(mat[a*4], mat[a*4+1]);
    }

  glhr::set_modelview(glhr::as_glmatrix(mat));
  }

int global_projection;

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

  for(int ed = current_display->stereo_active() ? -1 : 0; ed<2; ed+=2) {
    if(global_projection && global_projection != ed) continue;
    current_display->set_all(ed);
    bool draw = color;
    
    if(shaderside_projection) {
      if(glhr::current_shader_projection == glhr::shader_projection::band && V[2][2] > 1e8) continue;
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
          GLfloat dist = shaderside_projection ? current_display->scrdist : 0;
          vector<glvertex> scr = {
            glhr::makevertex(-xx, -yy, dist), 
            glhr::makevertex(+xx, -yy, dist), 
            glhr::makevertex(+xx, +yy, dist), 
            glhr::makevertex(-xx, +yy, dist)
            };
          glhr::vertices(scr);
          glhr::id_modelview();
          glDrawArrays(tinf ? GL_TRIANGLES : GL_TRIANGLE_FAN, 0, 4);
          glhr::vertices(v);
          if(shaderside_projection) glapplymatrix(V);
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
  }
#endif

ld scale_at(const transmatrix& T) {
  if(DIM == 3 && pmodel == mdPerspective) return 1 / abs((tC0(T))[2]);
  if(sol) return 1;
  hyperpoint h1, h2, h3;
  applymodel(tC0(T), h1);
  applymodel(T * xpush0(.01), h2);
  applymodel(T * ypush(.01) * C0, h3);
  return sqrt(hypot_d(2, h2-h1) * hypot_d(2, h3-h1) / .0001);
  }

ld linewidthat(const hyperpoint& h) {
  if(!(vid.antialias & AA_LINEWIDTH)) return 1;
  else if(hyperbolic && pmodel == mdDisk && vid.alpha == 1 && !ISWEB) {
    double dz = h[DIM];
    if(dz < 1 || abs(dz-current_display->scrdist) < 1e-6) return 1;
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

void set_width(ld w) {
  #if MINIMIZE_GL_CALLS
  if(w != glhr::current_linewidth) glflush();
  #endif
  glhr::set_linewidth(w);
  }

// -radius to +3radius

int mercator_coord;
int mercator_loop_min = 0, mercator_loop_max = 0;
ld mercator_period;

void fixMercator(bool tinf) {

  if(pmodel == mdBand)
    mercator_period = 4 * current_display->radius;
  else
    mercator_period = 2 * current_display->radius;
  
  if(!conformal::model_straight)
    for(auto& g: glcoords)
      conformal::apply_orientation(g[0], g[1]);
    
  if(pmodel == mdSinusoidal)
    for(int i = 0; i<isize(glcoords); i++) 
      glcoords[i][mercator_coord] /= cos(glcoords[i][1] / current_display->radius / vid.stretch * M_PI);

  ld hperiod = mercator_period / 2;
    
  mercator_coord = 0;
  
  auto dist = [] (ld a, ld b) { return max(b, a-b); };
  
  ld chypot = hypot(dist(vid.xres, current_display->xcenter), dist(vid.yres, current_display->ycenter));
  
  ld cmin = -chypot/2, cmax = chypot/2, dmin = -chypot, dmax = chypot;

  if(mercator_coord)
    swap(cmin, dmin), swap(cmax, dmax);
  if(pmodel == mdSinusoidal)
    dmin = -vid.stretch * current_display->radius / 2, dmax = vid.stretch * current_display->radius / 2;
  if(pmodel == mdBandEquidistant)
    dmin = -vid.stretch * current_display->radius / 2, dmax = vid.stretch * current_display->radius / 2;
  if(pmodel == mdBandEquiarea)
    dmin = -vid.stretch * current_display->radius / M_PI, dmax = vid.stretch * current_display->radius / M_PI;

  for(int i = 0; i<isize(glcoords); i++) {
    while(glcoords[0][mercator_coord] < hperiod) glcoords[0][mercator_coord] += mercator_period;
    while(glcoords[0][mercator_coord] > hperiod) glcoords[0][mercator_coord] -= mercator_period;
    }
    
  ld first = glcoords[0][mercator_coord];
  ld next = first;
  
  ld mincoord = first, maxcoord = first;

  for(int i = 0; i<isize(glcoords); i++) {
    while(glcoords[i][mercator_coord] < next - hperiod)
      glcoords[i][mercator_coord] += mercator_period;
    while(glcoords[i][mercator_coord] > next + hperiod)
      glcoords[i][mercator_coord] -= mercator_period;
    next = glcoords[i][mercator_coord];
    mincoord = min<ld>(mincoord, glcoords[i][mercator_coord]);
    maxcoord = max<ld>(maxcoord, glcoords[i][mercator_coord]);
    }
  
  if(abs(mincoord) > 50000 || abs(maxcoord) > 50000 || std::isnan(mincoord) || std::isnan(maxcoord)) {
    mercator_loop_max--;
    return;
    }
  
  ld last = first;
  while(last < next - hperiod) last += mercator_period;
  while(last > next + hperiod) last -= mercator_period;
    
  if(first == last) {
    while(mincoord > cmin)
      mercator_loop_min--, mincoord -= mercator_period;
    while(maxcoord < cmax)
      mercator_loop_max++, maxcoord += mercator_period;
    if(pmodel == mdSinusoidal)
      for(int i = 0; i<isize(glcoords); i++) 
        glcoords[i][mercator_coord] *= cos(glcoords[i][1] / current_display->radius / vid.stretch * M_PI);    
    if(!conformal::model_straight)
      for(auto& g: glcoords)
        conformal::apply_orientation(g[1], g[0]);
    }
  else {
    if(tinf) { 
      // this cannot work in Mercator
      mercator_loop_max--; return; 
      }
    if(last < first) {
      reverse(glcoords.begin(), glcoords.end());
      swap(first, last);
      }
    while(maxcoord > cmin) {
      for(int i=0; i<isize(glcoords); i++) glcoords[i][mercator_coord] -= mercator_period;
      first -= mercator_period; last -= mercator_period;
      mincoord -= mercator_period; maxcoord -= mercator_period;
      }
    int base = isize(glcoords);
    int minto = mincoord;
    while(minto < cmax) {
      for(int i=0; i<base; i++) {
        glcoords.push_back(glcoords[isize(glcoords)-base]);
        glcoords.back()[mercator_coord] += mercator_period;
        }
      minto += mercator_period;
      }
    if(pmodel == mdSinusoidal)
      for(int i = 0; i<isize(glcoords); i++) 
        glcoords[i][mercator_coord] *= cos(glcoords[i][1] / current_display->radius / vid.stretch * M_PI);
    glcoords.push_back(glcoords.back());
    glcoords.push_back(glcoords[0]);
    for(int u=1; u<=2; u++) {
      auto& v = glcoords[isize(glcoords)-u][1-mercator_coord];
      v = v < 0 ? dmin : dmax;
      }
    if(!conformal::model_straight)
      for(auto& g: glcoords)
        conformal::apply_orientation(g[1], g[0]);
    /* printf("cycling %d -> %d\n", base, qglcoords);
    for(int a=0; a<qglcoords; a++)
      printf("[%3d] %10.5lf %10.5lf\n", a, glcoords[a][0], glcoords[a][1]); */
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
  if((p->flags & POLY_FORCEWIDE) || pmodel == mdUnchanged)
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

void dqi_poly::draw() {
  if(flags & POLY_DEBUG) debug_this();

  dynamicval<ld> bs(hr::band_shift, band_shift);
  if(!hyperbolic && among(pmodel, mdPolygonal, mdPolynomial)) {
    bool any = false;
    for(int i=0; i<cnt; i++) {
      hyperpoint h1 = V * glhr::gltopoint((*tab)[offset+i]);
      if(h1[2] > 0) any = true;
      }
    if(!any) return;
    }
  
  if(sphere && tinf && DIM == 2 && cnt > 3) {
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
          conformal::apply_orientation(ah1[0], ah1[1]);
          conformal::apply_orientation(ah2[0], ah2[1]);
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
    dynamicval<eModel> d1(pmodel, mdUnchanged);
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
  if(vid.usingGL && (current_display->set_all(global_projection), shaderside_projection)) {
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
  
  mercator_loop_min = mercator_loop_max = 0;
  if(sphere && mdBandAny())
    fixMercator(tinf);
    
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

  for(int l=mercator_loop_min; l <= mercator_loop_max; l++) {
  
    if(l || lastl) { 
      for(int i=0; i<isize(glcoords); i++) {
        if(pmodel == mdSinusoidal) {
          ld y = glcoords[i][1], x = glcoords[i][0];
          conformal::apply_orientation(x, y);
          mercator_period = 2 * current_display->radius * cos(y / current_display->radius / vid.stretch * M_PI);
          }
        glcoords[i][mercator_coord] += conformal::ocos * mercator_period * (l - lastl);
        glcoords[i][1-mercator_coord] += conformal::osin * mercator_period * (l - lastl);
        }
      lastl = l;
      }
    
    if(equi && (poly_flags & POLY_INVERSE)) {
      if(abs(zlevel(V * C0) - 1) < 1e-6 && !tinf) {
        // we should fill the other side
        ld h = atan2(glcoords[0][0], glcoords[0][1]);
        for(int i=0; i<=360; i++) {
          ld a = i * degree + h;
          glcoords.push_back(glhr::makevertex(current_display->radius * sin(a), current_display->radius * vid.stretch * cos(a), current_display->scrdist));
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

void prettypoint(const hyperpoint& h) {
  prettylinepoints.push_back(glhr::pointtogl(h));
  }

void prettylinesub(const hyperpoint& h1, const hyperpoint& h2, int lev) {
  if(lev >= 0) {
    hyperpoint h3 = midz(h1, h2);
    prettylinesub(h1, h3, lev-1);
    prettylinesub(h3, h2, lev-1);
    }
  else prettypoint(h2);
  }

void prettyline(hyperpoint h1, hyperpoint h2, color_t col, int lev, int flags, PPR prio) {
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

void prettypoly(const vector<hyperpoint>& t, color_t fillcol, color_t linecol, int lev) {
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

void queuereset(eModel m, PPR prio) {
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
        
void initquickqueue() {
  ptds.clear();
  poly_outline = OUTLINE_NONE;
  }

void sortquickqueue() {
  for(int i=1; i<isize(ptds);)
    if(i && ptds[i]->prio < ptds[i-1]->prio) {
      swap(ptds[i], ptds[i-1]);
      i--;
      }
    else i++;
  }

void quickqueue() {
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

ld backbrightness = .25;

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

void sort_drawqueue() {
  
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

void reverse_priority(PPR p) {
  reverse(ptds.begin()+qp0[int(p)], ptds.begin()+qp[int(p)]);
  }

void reverse_side_priorities() {
  for(PPR p: {PPR::REDWALLs, PPR::REDWALLs2, PPR::REDWALLs3, PPR::WALL3s,
    PPR::LAKEWALL, PPR::INLAKEWALL, PPR::BELOWBOTTOM})
      reverse_priority(p);
  }

// on the sphere, parts on the back are drawn first
void draw_backside() {  
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

void reverse_transparent_walls() {
  int pt = int(PPR::TRANSPARENT_WALL);
  reverse(&ptds[qp0[pt]], &ptds[qp[pt]]);
  }

void draw_main() {
  if(sphere && DIM == 3 && pmodel == mdPerspective) {
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

  
void drawqueue() {
  callhooks(hook_drawqueue);
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
  if(DIM == 2) sort(&ptds[qp0[int(p)]], &ptds[qp[int(p)]], 
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

template<class T, class... U> T& queuea(PPR prio, U... u) {
  ptds.push_back(unique_ptr<T>(new T (u...)));
  ptds.back()->prio = prio;  
  return (T&) *ptds.back();
  }

#if CAP_SHAPES
dqi_poly& queuepolyat(const transmatrix& V, const hpcshape& h, color_t col, PPR prio) {
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

dqi_poly& queuetable(const transmatrix& V, const vector<glvertex>& f, int cnt, color_t linecol, color_t fillcol, PPR prio) {
 
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
dqi_poly& queuepoly(const transmatrix& V, const hpcshape& h, color_t col) {
  return queuepolyat(V,h,col,h.prio);
  }

void queuepolyb(const transmatrix& V, const hpcshape& h, color_t col, int b) {
  queuepolyat(V,h,col,h.prio+b);
  }
#endif

void curvepoint(const hyperpoint& H1) {
  curvedata.push_back(glhr::pointtogl(H1));
  }

dqi_poly& queuecurve(color_t linecol, color_t fillcol, PPR prio) {
  auto &res = queuetable(Id, curvedata, isize(curvedata)-curvestart, linecol, fillcol, prio);
  res.offset = curvestart;
  curvestart = isize(curvedata);
  return res;
  }

dqi_action& queueaction(PPR prio, const reaction_t& action) {
  return queuea<dqi_action> (prio, action);
  }

dqi_line& queueline(const hyperpoint& H1, const hyperpoint& H2, color_t col, int prf, PPR prio) {
  auto& ptd = queuea<dqi_line> (prio);

  ptd.H1 = H1;
  ptd.H2 = H2;
  ptd.band_shift = band_shift;
  ptd.prf = prf;
  ptd.width = vid.linewidth;
  ptd.color = (darkened(col >> 8) << 8) + (col & 0xFF);
  
  return ptd;
  }

void queuestr(int x, int y, int shift, int size, string str, color_t col, int frame, int align) {
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

void queuechr(int x, int y, int shift, int size, char chr, color_t col, int frame, int align) {
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

void queuecircle(int x, int y, int size, color_t color, PPR prio, color_t fillcolor) {
  auto& ptd = queuea<dqi_circle>(prio);
  ptd.x = x;
  ptd.y = y;
  ptd.size = size;
  ptd.color = color;
  ptd.fillcolor = fillcolor;
  ptd.linewidth = vid.linewidth;
  }

void getcoord0(const hyperpoint& h, int& xc, int &yc, int &sc) {
  hyperpoint hscr;
  applymodel(h, hscr);
  xc = current_display->xcenter + current_display->radius * hscr[0];
  yc = current_display->ycenter + current_display->radius * vid.stretch * hscr[1];
  sc = 0;
  // EYETODO sc = vid.eye * current_display->radius * hscr[2];
  }

ld scale_in_pixels(const transmatrix& V) {
  return scale_at(V) * cgi.scalefactor * current_display->radius / 2.5;
  }

bool getcoord0_checked(const hyperpoint& h, int& xc, int &yc, int &zc) {
  if(invalid_point(h)) return false;
  if(point_behind(h)) return false;
  getcoord0(h, xc, yc, zc);
  return true;
  }

void queuechr(const hyperpoint& h, int size, char chr, color_t col, int frame) {
  int xc, yc, sc;
  if(getcoord0_checked(h, xc, yc, sc))
    queuechr(xc, yc, sc, size, chr, col, frame);
  }

void queuechr(const transmatrix& V, double size, char chr, color_t col, int frame) {
  int xc, yc, sc; 
  if(getcoord0_checked(tC0(V), xc, yc, sc))
    queuechr(xc, yc, sc, scale_in_pixels(V) * size, chr, col, frame);
  }
  
void queuestr(const hyperpoint& h, int size, const string& chr, color_t col, int frame) {
  int xc, yc, sc; 
  if(getcoord0_checked(h, xc, yc, sc))
    queuestr(xc, yc, sc, size, chr, col, frame);
  }
  
void queuestr(const transmatrix& V, double size, const string& chr, color_t col, int frame, int align) {
  int xc, yc, sc; 
  if(getcoord0_checked(tC0(V), xc, yc, sc))
    queuestr(xc, yc, sc, scale_in_pixels(V) * size, chr, col, frame, align);
  }
  
void queuecircle(const transmatrix& V, double size, color_t col) {
  int xc, yc, sc; 
  if(!getcoord0_checked(tC0(V), xc, yc, sc)) return;
  int xs, ys, ss; getcoord0(V * xpush0(.01), xs, ys, ss);  
  queuecircle(xc, yc, scale_in_pixels(V) * size, col);
  }

void queuemarkerat(const transmatrix& V, color_t col) {
#if CAP_SHAPES
  queuepolyat(V, cgi.shTriangle, col, PPR::LINE);
#endif
  }

#endif

}
