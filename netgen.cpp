// Hyperbolic Rogue -- paper model generator
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file netgen.cpp
 *  \brief paper model generator
 */

#include "hyper.h"
#if CAP_MODEL
namespace hr { 

EX namespace netgen {

  // We need a two-dimensional vector class for this.
  // (actually we could just use hyperpoint but this is very old...)
  
  struct vec {
    double x, y;
    vec(double _x, double _y) : x(_x), y(_y) { }
    vec() : x(0), y(0) {}
    };
  
  vec& operator += (vec& a, const vec b) { a.x += b.x; a.y += b.y; return a; }
  vec& operator -= (vec& a, const vec b) { a.x -= b.x; a.y -= b.y; return a; }
  
  // coordinatewise multiplication and division
  vec& operator *= (vec& a, const vec b) { a.x *= b.x; a.y *= b.y; return a; }
  vec& operator *= (vec& a, double scalar) { a.x *= scalar; a.y *= scalar; return a; }
  vec& operator /= (vec& a, const vec b) { a.x /= b.x; a.y /= b.y; return a; }
  vec& operator /= (vec& a, double scalar) { a.x /= scalar; a.y /= scalar; return a; }
  
  vec  operator +  (vec a, const vec b) { return a+=b; }
  vec  operator -  (vec a, const vec b) { return a-=b; }
  vec  operator *  (vec a, const vec b) { return a*=b; }
  vec  operator /  (vec a, const vec b) { return a/=b; }
  
  vec  operator *  (vec a, double scalar)   { return a*=scalar; }
  vec  operator *  (double scalar, vec a)   { return a*=scalar; }
  vec  operator /  (vec a, double scalar)   { return a/=scalar; }
  vec  operator /  (double scalar, vec a)   { return a/=scalar; }

  vec ang(double f) { return vec(cos(f), sin(f)); }

  double norm(vec v) { return v.x*v.x+v.y*v.y; }

  // the parameters.
  
  bool loaded;
  
  int nscale, PX, PY, BASE, SX, SY, CELLS, fontsize, created;
  double el;
  #define MAXCELLS 1000
  
  // All the datatables stored in the net files.
  
  int ct[MAXCELLS];
  double vx[MAXCELLS][16];
  vec center[MAXCELLS];
  double rot[MAXCELLS];  
  int glued[MAXCELLS];
  vector<int> nei[MAXCELLS];

  // auxiliary data
  double raylen[MAXCELLS];
  double edgist[MAXCELLS];  
  vector<char> patek[MAXCELLS];
  
  // data generated by HyperRogue
  vector<hyperpoint> hcenter[MAXCELLS];
  
  // Functions handling the data.
  //==============================
  
  // Use HyperRogue to generate the data (ct, vx, nei).

  EX int mode = 0;
  
  EX void buildVertexInfo(cell *c, transmatrix V) {
    
    if(mode == 1)
    for(int ii=0; ii<CELLS; ii++) if(dcal[ii] == c) {
    
      hcenter[ii].resize(c->type+1);
      hcenter[ii][c->type] = V * C0;

      if(c->type == S7) {
        for(int i=0; i<c->type; i++) {
  
          transmatrix V2 = V * ddspin(c, i, M_PI/S7) * xpush(cgi.hexf);
          
          hcenter[ii][i] = V2 * C0;
          }
        }
  
      if(c->type == S6) {
        for(int i=0; i<c->type; i++) {
  
          transmatrix V2 = 
            V * ddspin(c, i, 0) * xpush(cgi.crossf) * spin(M_PI+M_PI/S7) * xpush(cgi.hexf);
          
          hcenter[ii][i] = V2 * C0;
          }
        }
      
      }
    }

  void dataFromHR() {
    mode = 1;
    drawthemap();
    mode = 0;

    for(int i=0; i<CELLS; i++) {
      ct[i] = dcal[i]->type;
      for(int k=0; k<=ct[i]; k++)
        vx[i][2*k] = hcenter[i][k][0],
        vx[i][2*k+1] = hcenter[i][k][1];
      
      nei[i].clear();
      nei[i].resize(ct[i], -1);

      for(int j=0; j<CELLS; j++) {
        cell *c1 = dcal[i];
        cell *c2 = dcal[j];
        for(int k=0; k<c1->type; k++) if(c1->move(k) == c2)
          nei[i][k] = j;
        }
      }

    for(int i=0; i<CELLS; i++) {
      center[i] = vec(SX/2, SY/2);
      rot[i] = 0;
      glued[i] = -1;
      for(int e=0; e<ct[i]; e++)
        if(nei[i][e] < i && nei[i][e] != -1 && (glued[i] == -1 || nei[i][e] < glued[i])) {
          glued[i] = nei[i][e];
          }
      }
    }
  
  void loadData() {

    fhstream f("papermodeldata.txt", "rt");
    if(!f.f) return;

    if(!scan(f, CELLS, SX, SY, PX, PY, nscale, BASE, el, created)) return;
    
    loaded = true;
      
    if(!created) return;
    
    for(int i=0; i<CELLS; i++) scan(f, ct[i]);

    for(int i=0; i<CELLS; i++) nei[i].clear(), nei[i].resize(ct[i], -1);

    for(int i=0; i<CELLS; i++) for(int j=0; j<16; j++) scan(f, vx[i][j]);

    while(true) {
      int a, b, c;
      scan(f, a, b, c);
      if(a < 0) break;
      else nei[a][c] = b;
      }
    
    for(int i=0; i<CELLS; i++) {
      double dx, dy, dr;
      int g;
      scan(f, dx, dy, dr, g);
      center[i] = vec(dx, dy);
      rot[i] = dr;
      glued[i] = g;
      }
    }

  void saveData() {
    // global parameters
    fhstream f("papermodeldata2.txt", "wt");
    if(!f.f) {
      addMessage("Could not save the paper model data");
      return;
      }
    println(f, spaced(CELLS, SX, SY, PX, PY, nscale, BASE, el, created), "\n");

    // net parameters: cell types    
    println(f, spaced_of(ct, CELLS));
    
    // net parameters: hcenters    
    for(int i=0; i<CELLS; i++) {
      println(f, spaced_of(vx[i], 16));
      }
    println(f, "\n");
    
    // create netgen
    for(int i=0; i<CELLS; i++) for(int j=0; j<CELLS; j++) {
      for(int k=0; k<ct[i]; k++) if(nei[i][k] == j)
        print(f, spaced(i, j, k), "  ");
      }
    println(f, "-1 -1 -1");
    
    // graphics
    for(int i=0; i<CELLS; i++) 
      println(f, spaced(center[i].x, center[i].y, rot[i], glued[i]));
    }  

  // Simple graphical functions
  //============================
  
  color_t argb(color_t c) { return ((c & 0xFFFFFF) >> 8) | ((c & 0xFF) << 24); }
  
  void blackline(vec v1, vec v2, color_t col = 0x000000FF) {
#if CAP_SDLGFX
    aalineColor(srend, int(v1.x), int(v1.y), int(v2.x), int(v2.y), align(col));
#elif CAP_SDL
    SDL_LockSurface(s);
    int len = abs(v1.x-v2.x) + abs(v1.y-v2.y);
    for(int i=0; i<=len; i++) 
      qpixel(s, int(v1.x + (v2.x-v1.x)*i/len), int(v1.y + (v2.y-v1.y)*i/len)) = argb(col);      
    SDL_UnlockSurface(s);
#endif
    }
  
  void drawtriangle(vec v1, vec v2, vec v3, color_t col) {
#if CAP_SDLGFX
    polyx[0] = int(v1.x);
    polyx[1] = int(v2.x);
    polyx[2] = int(v3.x);
    polyy[0] = int(v1.y);
    polyy[1] = int(v2.y);
    polyy[2] = int(v3.y);
    filledPolygonColorI(srend, polyx, polyy, 3, col);
#elif CAP_SDL
    SDL_LockSurface(s);
    int len = abs(v1.x-v2.x) + abs(v1.y-v2.y);
    for(int i=0; i<=len; i++) for(int j=0; j<=len; j++) if(i+j <= len)
      qpixel(s, int(v3.x + (v2.x-v3.x)*i/len + (v1.x-v3.x)*j/len), int(v3.y + (v2.y-v3.y)*i/len + (v1.y-v3.y)*j/len)) = argb(col);
    SDL_UnlockSurface(s);
#endif
    }
    
  void blackcircle(vec v, int r, color_t col = 0x000000FF) {
#if CAP_SDLGFX
    aacircleColor(srend, int(v.x), int(v.y), r, col);
#endif
    }
  
  void blacktext(vec v, char c) {
    char str[2]; str[0] = c; str[1] = 0;
    int tsize = int(el * 12/27);
    displaystr(int(v.x), int(v.y), 0, tsize, str, 0, 8);
    }
  
  hyperpoint hvec(int i, int e) {
    return hpxy(vx[i][2*e], vx[i][2*e+1]);
    }
  
  bool wellspread(double d1, double d2, double d3, int &co) {
    int id1 = int(d1);
    int id2 = int(d2);
    int id3 = int(d3);
    co = min(min(id1,id2),id3);
    return (id1 <= co+1 && id2 <= co+1 && id3 <= co+1);
    }
  
  SDL_Surface *net, *hqsurface;
  
  color_t& hqpixel(hyperpoint h) {
    int hx, hy, hs;
    getcoord0(shiftless(h), hx, hy, hs);
    return qpixel(hqsurface, hx, hy);
    }
  
  void copyhypertriangle(
    vec g1, vec g2, vec g3,
    hyperpoint h1, hyperpoint h2, hyperpoint h3) {
    int ix, iy;

    if(wellspread(g1.x,g2.x,g3.x,ix) && wellspread(g1.y,g2.y,g3.y,iy))
      qpixel(net,ix,iy) = hqpixel(h1);
    else {
    
      vec g4 = (g2+g3)/2;
      vec g5 = (g3+g1)/2;
      vec g6 = (g1+g2)/2;
      
      hyperpoint h4 = mid(h2,h3);
      hyperpoint h5 = mid(h3,h1);
      hyperpoint h6 = mid(h1,h2);
      
      copyhypertriangle(g1,g5,g6, h1,h5,h6);
      copyhypertriangle(g5,g3,g4, h5,h3,h4);
      copyhypertriangle(g6,g4,g2, h6,h4,h2);
      copyhypertriangle(g4,g6,g5, h4,h6,h5);
      }
    }
  
  void setRaylen() {
    for(int i=0; i<CELLS; i++) {
      raylen[i] = el / sin(M_PI / ct[i]);
      edgist[i] = raylen[i] * cos(M_PI / ct[i]);
      }
    }
  
  // draw the model
  void createPapermodel() {

    loadData();
    
    renderbuffer rbuf(2000, 2000, vid.usingGL);

    dynamicval<videopar> dv(vid, vid);
    vid.xres = vid.yres = 2000; pconf.scale = 0.99; 
    
    if(1) {
      resetbuffer rb;
      calcparam();
      models::configure();
    
      mode = 2;
  
      darken = 0;
      rbuf.enable();
      current_display->set_viewport(0);
      drawfullmap();
      
      hqsurface = rbuf.render();
      IMAGESAVE(hqsurface, "test.png");
      rb.reset();
      }
    
    vid.usingGL = false;

    mode = 0;
    
/*    for(int i=0; i<CELLS; i++) {
      int t = ct[i];  
      for(int e=0; e<t; e++) 
        drawline(hvec(i,e), hvec(i,(e+1)%t), 0x80808080);
      for(int e=0; e<7; e++) 
        drawline(hvec(i,e), hvec(i,7), 0x80808080);
      } */

    dynamicval<SDL_Surface*> ds(s);

    s = net = SDL_CreateRGBSurface(SDL_SWSURFACE,SX*nscale,SY*nscale,32,0,0,0,0);
    #if SDLVER >= 2
    dynamicval<SDL_Renderer*> dr(srend);
    srend = SDL_CreateSoftwareRenderer(s);
    #endif

    SDL_FillSurfaceRect(net, NULL, 0xFFFFFF);
    
    int pateks = 0;
    for(int i=0; i<CELLS; i++) patek[i].resize(ct[i]);
    
    int zeroi = nei[0][0];
    int zeroe = 0;
    for(int e=0; e<6; e++) if(nei[zeroi][e] == 0) zeroe = e;
    
    el *= nscale;
    setRaylen();
    
    for(int faza=0; faza<2; faza++) for(int i=0; i<CELLS; i++) {
      
      int t = ct[i];
      
      printf("faza %d cell %d\n", faza, i);
  
      for(int e=0; e<t; e++) {
        vec v1 = center[i] * nscale + raylen[i] * ang(rot[i] + TAU * e/t);
        vec v2 = center[i] * nscale + raylen[i] * ang(rot[i] + TAU * (e+1)/t);
        vec v3 = (v1+v2)/2;
  
        if(faza == 1) blackline(v1, v2);
        
        int ofs = t == 7 ? 0 : 5;
        
        // 0,2,0 ~ 2,0,0
        
        if(0) if((i==0 && e == 0) || (i == zeroi && e == zeroe)) {
          for(int ofs=0; ofs<t; ofs++) {
            println(hlog, "OFS ", ofs, hvec(i, (e+ofs)%t), hvec(i, (e+1+ofs)%t));
            }
          }

        if(faza == 0) copyhypertriangle(
          center[i] * nscale, v1, v2,
          hvec(i,7), hvec(i, (e+ofs)%t), hvec(i, (e+1+ofs)%t)
          );
        
        if(faza == 1)
        if(nei[i][e] != -1 && nei[i][e] != glued[i] && glued[nei[i][e]] != i) {
          vec vd = v2-v1;
          swap(vd.x, vd.y); vd.x = -vd.x;
          double factor = -sqrt(3)/6;
          vd.x *= factor;
          vd.y *= factor;
          vec v4 = v3 + vd;
          vec v5 = v3 + vd/2;
          
          if(!patek[i][e]) {
            int i2 = nei[i][e];
            for(int e2=0; e2<ct[nei[i][e]]; e2++) if(nei[i2][e2] == i) 
              patek[i][e] = patek[i2][e2] = 
                "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "!@#$%^&*+=~:;<>?/|\"., [{(\\]})" [(pateks++) % 85];
            }
          
          color_t col = 0xFFFFFFFF;
          int p = patek[i][e];
          col -= 0x8000 * (p&1); p /= 2;
          col -= 0x800000 * (p&1); p /= 2;
          col -= 0x80000000 * (p&1); p /= 2;
          col -= 0x4000 * (p&1); p /= 2;
          col -= 0x400000 * (p&1); p /= 2;
          col -= 0x40000000 * (p&1); p /= 2;
          col -= 0x2000 * (p&1); p /= 2;
          col -= 0x200000 * (p&1); p /= 2;
          col -= 0x20000000 * (p&1); p /= 2;
          
          drawtriangle(v1,v2,v4, col);
            
          blacktext(v5, patek[i][e]);

          blackline(v1, v4);
          blackline(v2, v4);          
          }
        }
      }
    
    println(hlog, "pateks = ", pateks);
        
    IMAGESAVE(net, "papermodel-all" IMAGEEXT);
    IMAGESAVE(hqsurface, "papermodel-source" IMAGEEXT);

    int qx = SX*nscale/PX;
    int qy = SY*nscale/PY;
    SDL_Surface *quarter = SDL_CreateRGBSurface(SDL_SWSURFACE,qx,qy,32,0,0,0,0);
    for(int iy=0; iy<PY; iy++)
    for(int ix=0; ix<PX; ix++) {
      for(int y=0; y<qy; y++) for(int x=0; x<qx; x++)
        qpixel(quarter,x,y) = qpixel(net, x+qx*ix, y+qy*iy);
      IMAGESAVE(quarter, hr::format("papermodel-page%d%d" IMAGEEXT, iy, ix).c_str());
      }
    
    SDL_DestroySurface(net);
    SDL_DestroySurface(quarter);
    #if SDLVER >= 2
    SDL_DestroyRenderer(srend);
    #endif
    }
  
  vec mousepos, rel;

  int bei = 0, bee = 0, whichcell = 0;
  double cedist;
  bool dragging = false;
  
  int glueroot(int i) {
    if(glued[i] == -1) return i;
    return glueroot(glued[i]);
    }
  
  void clicked(int x, int y, int b) {
  
    mousepos = vec(x, y);
    
    if(b == 1)
      rel = center[glueroot(whichcell)] - mousepos,
      dragging = true;
    
    if(b == 17)
      dragging = false;
    
    if(b == 32 && dragging)
      center[glueroot(whichcell)] = rel + mousepos;
    
    }
  
  void applyGlue(int i) {
    int j = glued[i];
    int it = ct[i];
    int jt = ct[j];
    int ie = 0, je = 0;
    for(int e=0; e<it; e++) if(nei[i][e] == j) ie = e;
    for(int e=0; e<jt; e++) if(nei[j][e] == i) je = e;
    
    rot[i] = rot[j] + TAU*(je+.5)/jt - TAU*(ie+.5)/it + M_PI;
    center[i] = 
      center[j] +
        (edgist[i]+edgist[j]) * ang(rot[j] + TAU*(je+.5)/jt);
    }

  shiftpoint vec_to_p(vec v) {
    return shiftless(hyperpoint(v.x - current_display->xcenter, v.y - current_display->ycenter, 0, 1));
    }

  void netline(vec a, vec b, color_t col) {
    if(vid.usingGL)
      queueline(vec_to_p(a), vec_to_p(b), col, 0);
    else
      blackline(a, b, col);
    }

  void netcircle(vec ctr, int rad, color_t col) {
    if(vid.usingGL)
      queuecircle(ctr.x, ctr.y, rad, col);
    else
      blackcircle(ctr, rad, col);
    }

  void displaynets() {
    if(!vid.usingGL) SDL_LockSurface(s);

    setRaylen();
    
    if(vid.usingGL) {
      calcparam();
      setGLProjection();
      glhr::set_depthtest(false);
      current_display->set_all(0,0);
      }
    else {
      for(int uy=SY-1; uy>=0; uy--)
      for(int ux=SX-1; ux>=0; ux--) {
        qpixel(s, ux, uy) = 0;
        }
      initquickqueue();
      }
    
    for(int y=1; y<PY; y++)
      netline(vec(0,SY*y/PY), vec(SX,SY*y/PY), 0x404080FF);

    for(int x=1; x<PX; x++)
      netline(vec(SX*x/PX,0), vec(SX*x/PX,SY), 0x404080FF);
    
    for(int i=0; i<CELLS; i++) {
      
      if(norm(center[i]-mousepos) < norm(center[whichcell]-mousepos))
        whichcell = i;
  
      int t = ct[i];
  
      if(i == whichcell)
        netcircle(center[i], 10, 0x40FF40FF);
  
      if(i == bei || i == nei[bei][bee])
        netcircle(center[i], 5, 0x40FF40FF);
      
      if(glued[i] == -1)
        netcircle(center[i], 7, 0xFF4040FF);
      
      if(glued[i] != -1) 
        applyGlue(i);
  
      for(int e=0; e<t; e++) {
        vec v1 = center[i] + raylen[i] * ang(rot[i] + TAU*e/t);
        vec v2 = center[i] + raylen[i] * ang(rot[i] + TAU*(e+1)/t);
        vec v3 = (v1+v2)/2;
  
        if(nei[i][e] >= 0 && !dragging) {
          if(norm(v3-mousepos) < cedist) bei = i, bee = e;
          if(i == bei && e == bee) cedist = norm(v3-mousepos);
          }

        color_t col = 
          i == bei && e == bee ? 0x40FF40FF:
          i == nei[bei][bee] && nei[i][e] == bei ? 0x40FF40FF :
          nei[i][e] == glued[i] ? 0x303030FF :
          glued[nei[i][e]] == i ? 0x303030FF :
          nei[i][e] >= 0 ? 0xC0C0C0FF : 
          0x808080FF;
  
        netline(v1, v2, col);
        
        if(nei[i][e] != -1 && nei[i][e] != glued[i] && glued[nei[i][e]] != i) {
          vec vd = v2-v1;
          swap(vd.x, vd.y); vd.x = -vd.x;
          double factor = -sqrt(3)/6;
          vd.x *= factor; vd.y *= factor;
          vec v4 = v3 + vd;
          
          netline(v1, v4, 0xFFC0C0C0);
          netline(v2, v4, 0xFFC0C0C0);
          }
        }
      }
    
    if(!vid.usingGL) SDL_UnlockSurface(s);
    else quickqueue();
    
    present_screen();
    }

  double rs, rz;
  
  void addglue() {
    int i = bei;
    int j = nei[bei][bee];
    if(glued[i] == j)
      glued[i] = -1;
    else if(glued[j] == i)
      glued[j] = -1;
    else if(glueroot(i) == glueroot(j))
      ;
    else if(glued[j] == -1)
      glued[j] = i;
    }
  
  int nti;
  
  void smooth() {
    int ti = SDL_GetTicks();
    rot[whichcell] += rs * (nti - ti) / 1000.0;
    
    el += rz * (nti - ti) / 1000.0;
    nti = ti;
    }
  
  void netgen_loop() {
    nti = SDL_GetTicks();
    while(true) {
      smooth();
      displaynets();
      SDL_Event event;
      
      while(SDL_PollEvent(&event)) switch (event.type) {
        case SDL_EVENT_QUIT:
          exit(1);
          return;
    
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
          clicked(event.button.x, event.button.y, event.button.button);
          break;
          }
        
        case SDL_EVENT_MOUSE_BUTTON_UP: {
          clicked(event.button.x, event.button.y, 16+event.button.button);
          break;
          }
        
        case SDL_EVENT_MOUSE_MOTION: {
          clicked(event.motion.x, event.motion.y, 32);
          break;
          }
        
        case SDL_EVENT_KEY_DOWN: {
          #if SDLVER >= 3
          int key = event.key.key;
          #else
          int key = event.key.keysym.sym;
          #endif
          #if SDLVER >= 2
          int uni = key;
          #else
          int uni = event.key.keysym.unicode;
          #endif
          
          if(uni == 'q' || key == SDLK_ESCAPE || key == SDLK_F10) 
            return;
          
          if(key == SDLK_PAGEUP) rs = 3;
          if(key == SDLK_PAGEDOWN) rs = -3;    
          if(uni == 'z') rz = 1;
          if(uni == 'x') rz = -1;          
          if(uni == 'g') addglue();
          
          break;
          }
    
        case SDL_EVENT_KEY_UP: {
          rs = 0;
          rz = 0;          
          break;
          }
        
        }
      }
    }
  
  void designNet() {
    if(1) {
      dynamicval<int> dwx(vid.window_x, SX);
      dynamicval<int> dwy(vid.window_y, SY);
      dynamicval<int> dfx(vid.fullscreen_x, SX);
      dynamicval<int> dfy(vid.fullscreen_y, SY);
      dynamicval<bool> dr(resizable, false);
      dynamicval<bool> dws(vid.relative_window_size, false);
      dynamicval<bool> dfs(vid.change_fullscr, true);
      dynamicval<bool> dcf(vid.want_fullscreen, false);
      dynamicval<eModel> m(pmodel, mdPixel);
      request_resolution_change = true;

      apply_screen_settings();

      netgen_loop();
      saveData();
      setvideomode();
      }
    apply_screen_settings();
    }

  void show() {
    cmode = sm::SIDE;
    gamescreen();
    if(true) {
      initquickqueue();
      for(int i=0; i<CELLS; i++) {
        int t = ct[i];
        int ofs = t == 7 ? 0 : 5;
        for(int e=0; e<t; e++) {
          color_t col = 
            nei[i][e] == glued[i] && glued[i] >= 0 ? 0x303030 :
            nei[i][e] >= 0 && glued[nei[i][e]] == i ? 0x303030 :
            nei[i][e] >= 0 ? 0x808080 : 
            0xC0C0C0;
    
          queueline(shiftless(hvec(i, (e+ofs)%t)), shiftless(hvec(i, (e+1+ofs)%t)), (col << 8) + 0xFF, 3);
          }
        }
      quickqueue();
      }
    if(mode != 2) {
      dialog::init("paper model creator");

      dialog::addItem(XLAT("synchronize net and map"), 's');
      dialog::addItem(XLAT("display the scope"), 't');  
      dialog::addItem(XLAT("create the model"), 'c');
      dialog::addItem(XLAT("design the net"), 'd');
      dialog::addBreak(50);
      dialog::addBack();
      
      dialog::display();
      }
    
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
  
      if(!loaded) { 
        loadData(); 
        if(!loaded) { 
          addMessage(XLAT("Failed to load the file 'papermodeldata.txt'"));
          popScreen();
          return;
          }
        if(!created) {
          View = Id;
          playermoved = false;
          dataFromHR();
          designNet();
          created = 1;
          return;
          }
        }
  
      if(mode == 2 && uni != 0) {
        mode = 0;
        return;
        }
      if(uni == 's') {
        View = Id;
        if(ctof(centerover)) View = spin(TAU * (rand() % 7) / 7) * View;
        playermoved = false;
        }
      else if(uni == 'c') {
        createPapermodel();
        addMessage(XLAT("The paper model created as papermodel-*.bmp"));
        }
      else if(uni == 'd') designNet();
      else if(uni == 't') mode = 2;
      else if(doexiton(sym, uni))
        popScreen();
      };
    }

  EX void run() { 
    if(euclid) 
      addMessage("Useless in Euclidean geometry.");
    else if(sphere)
      addMessage("Not implemented for spherical geometry. Please tell me if you really want this.");
    else
      pushScreen(show);
    }
EX }

}
#endif
