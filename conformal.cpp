// Hyperbolic Rogue -- the conformal/history mode
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

#if ISMOBWEB
typedef double precise;
#else
typedef long double precise;
#endif

namespace polygonal {

  typedef long double xld;

  typedef complex<xld> cld;

  int SI = 4;
  ld  STAR = 0;
  
  int deg = ISMOBWEB ? 2 : 20;

  precise matrix[MSI][MSI];
  precise ans[MSI];
  
  cld coef[MSI];
  ld coefr[MSI], coefi[MSI]; 
  int maxcoef, coefid;
  
  void solve() {
    if(pmodel == mdPolynomial) {
      for(int i=0; i<MSI; i++) coef[i] = cld(coefr[i], coefi[i]);
      return;
      } 
    if(pmodel != mdPolygonal) return;
    if(SI < 3) SI = 3;
    for(int i=0; i<MSI; i++) ans[i] = cos(M_PI / SI);
    for(int i=0; i<MSI; i++)
      for(int j=0; j<MSI; j++) {
        precise i0 = (i+0.) / (MSI-1);
        // i0 *= i0;
        // i0 = 1 - i0;
        i0 *= M_PI;
        matrix[i][j] = 
          cos(i0 * (j + 1./SI)) * (STAR > 0 ? (1+STAR) : 1)
        - sin(i0 * (j + 1./SI)) * (STAR > 0 ? STAR : STAR/(1+STAR));
        }
    
    for(int i=0; i<MSI; i++) {
      precise dby = matrix[i][i];
      for(int k=0; k<MSI; k++) matrix[i][k] /= dby;
      ans[i] /= dby; 
      for(int j=i+1; j<MSI; j++) {
        precise sub = matrix[j][i];
        ans[j] -= ans[i] * sub;
        for(int k=0; k<MSI; k++)
           matrix[j][k] -= sub * matrix[i][k];
        }
      }
    for(int i=MSI-1; i>=0; i--) {
      for(int j=0; j<i; j++) {
        precise sub = matrix[j][i];
        ans[j] -= ans[i] * sub;
        for(int k=0; k<MSI; k++)
           matrix[j][k] -= sub * matrix[i][k];
        }
      }
    }
  
  pair<ld, ld> compute(ld x, ld y, int prec) {
    if(x*x+y*y > 1) {
      xld r  = hypot(x,y);
      x /= r;
      y /= r;
      }
    if(pmodel == mdPolynomial) {
      cld z(x,y);
      cld res (0,0);
      for(int i=maxcoef; i>=0; i--) { res += coef[i]; if(i) res *= z; }
      return make_pair(real(res), imag(res));    
      }
      
    cld z(x, y);
    cld res (0,0);
    cld zp = 1; for(int i=0; i<SI; i++) zp *= z;

    for(int i=prec; i>0; i--) { 
      res += ans[i]; 
      res *= zp;
      }
    res += ans[0]; res *= z;
    return make_pair(real(res), imag(res));
    }

  pair<ld, ld> compute(ld x, ld y) { return compute(x,y,deg); }

  void drawBoundary(int color) {
    queuereset(mdDisk, PPR::CIRCLE);

    for(int r=0; r<=2000; r++) {
      cld z = exp(cld(0, 2*M_PI * r / 2000.0));
      pair<xld,xld> z2 = compute(real(z), imag(z), deg);
      hyperpoint h;
      h[0] = z2.first * vid.radius;
      h[1] = z2.second * vid.radius;
      h[2] = stereo::scrdist;
      curvepoint(h);
      }
    
    queuecurve(color, 0, PPR::CIRCLE);
    queuereset(pmodel, PPR::CIRCLE);
    }

  }

#if CAP_SDL
namespace spiral {

  typedef long double ld;
  typedef complex<long double> cld;

  int shiftx, shifty, velx, vely;

  vector<pair<short, short> > quickmap;

  int CX, CY, SX, SY, Yshift;
  
  vector<SDL_Surface*> band;
  SDL_Surface *out;
  
  bool displayhelp = true;
  
  color_t& bandpixel(int x, int y) {
    int i = 0;
    while(i < isize(band) && x >= band[i]->w)
      x -= band[i]->w, i++;
    return qpixel(band[i], x, y);
    }
  
  void precompute() {
  
    CX = 0;
    for(int i=0; i<isize(band); i++) CX += band[i]->w;
    if(CX == 0) { printf("ERROR: no CX\n"); return; }
    CY = band[0]->h;
    SX = out->w;
    SY = out->h;

    ld k = -2*M_PI*M_PI / log(2.6180339);

//   cld mnoznik = cld(0, M_PI) / cld(k, M_PI);

    cld factor = cld(0, -CY/2/M_PI/M_PI) * cld(k, M_PI);
    
    Yshift = CY * k / M_PI;
    
    quickmap.clear();
    
    double xc = ((SX | 1) - 2) / 2.;
    double yc = ((SY | 1) - 2) / 2.;
    
    for(int y=0; y<SY; y++)
    for(int x=0; x<SX; x++) {
      cld z(x-xc, y-yc);
      cld z1 = log(z);

      z1 = z1 * factor;

      quickmap.push_back(make_pair(int(real(z1)) % CX, int(imag(z1))));
      }
    }
  
  void draw() {
    int c = 0;
    for(int y=0; y<SY; y++) for(int x=0; x<SX; x++) {
      pair<short,short> p = quickmap[c++];
      int cx = p.first + shiftx;
      int cy = p.second + + shifty;
      int d = cy / CY;
      cy -= d * CY; cx -= d * Yshift;
      if(cy<0) cy += CY, cx += Yshift;
      cx %= CX; if(cx<0) cx += CX;
      qpixel(out, x, y) = bandpixel(cx, cy);
      }
    }

  void loop(vector<SDL_Surface*> _band) {

    band = _band;
    out = s;
    precompute();
    if(CX == 0) return;
    shiftx = shifty = 0;
    velx=1; vely=1;
    bool dosave = false;

    bool saveGL = vid.usingGL;
    if(saveGL) switchGL(); //  { vid.usingGL = false; setvideomode(); }

    while(true) {

      time_t timer;
      timer = time(NULL);
      char buf[128]; 
      strftime(buf, 128, "spiral-%y%m%d-%H%M%S" IMAGEEXT, localtime(&timer));

      SDL_LockSurface(s);
      draw();
      if(dosave) { dosave = false; IMAGESAVE(s, buf); }
      SDL_UnlockSurface(s);
      if(displayhelp) {
        displaystr(SX/2, vid.fsize*2, 0, vid.fsize, "arrows = navigate, ESC = return, h = hide help", forecolor, 8);
        displaystr(SX/2, SY - vid.fsize*2, 0, vid.fsize, XLAT("s = save to " IMAGEEXT, buf), forecolor, 8);
        }
      SDL_UpdateRect(s, 0, 0, 0, 0);  
      shiftx += velx; shifty += vely;

      SDL_Event event;
      while(SDL_PollEvent(&event)) switch (event.type) {

        case SDL_VIDEORESIZE: {
          resize_screen_to(event.resize.w, event.resize.h);
          precompute();
          break;
          }
        case SDL_QUIT: case SDL_MOUSEBUTTONDOWN:
          goto breakloop;

        case SDL_KEYDOWN: {
          int key = event.key.keysym.sym;
          // int uni = event.key.keysym.unicode;      
          if(key == SDLK_RIGHT) velx++;
          if(key == SDLK_LEFT) velx--;
          if(key == SDLK_UP) vely++;
          if(key == SDLK_DOWN) vely--;
          if(key == SDLK_ESCAPE) goto breakloop;
          if(key == 'h') displayhelp = !displayhelp;
          if(key == 's') dosave = true;
          }
        }
      }
    
    breakloop:
    quickmap.clear();
    if(saveGL) switchGL(); // { vid.usingGL = true; setvideomode(); }
    }

  }
#endif

bool isbad(ld z) { return !isfinite(z) || fabs(z) > 1e6; }

namespace conformal {

  void handleKeyC(int sym, int uni);
  
  int lastprogress;
  
  void progress_screen() {
    gamescreen(0);
    mouseovers = "";
    }

  void progress(string str) {
#if CAP_SDL
    int tick = SDL_GetTicks();
    if(tick > lastprogress + 250) {
      lastprogress = tick;
      msgs.clear();
      addMessage(str);
      drawscreen();
      }
#endif
    }
  
  bool on;
  vector<shmup::monster*> v;
  int llv;
  double phase;
  
  vector<pair<cell*, eMonster> > killhistory;
  vector<pair<cell*, eItem> > findhistory;
  vector<cell*> movehistory;
  
  bool includeHistory;
  ld lvspeed = 1;
  int bandhalf = 200;
  int bandsegment = 16000;
  ld rotation = 0;
  int do_rotate = 1;
  bool lower_halfplane;
  bool autoband = false;
  bool autobandhistory = false;
  bool dospiral = true;
  
  ld extra_line_steps = 0;

  void clear() {
    on = false;
    int N = isize(v);
    for(int i=0; i<N; i++) delete v[i];
    v.resize(0);
    }
  
  void create() {
    if(celldist(cwt.at) == 0) {
      addMessage("Must go a distance from the starting point");
      return;
      }
    
    on = true;
    cell *c = cwt.at;
    
    while(true) {
      shmup::monster *m = new shmup::monster;
      m->at = Id;
      m->base = c;
      v.push_back(m);
      if(c == currentmap->gamestart()) break;
      for(int i=0; i<c->type; i++)
        if(celldist(c->move(i)) < celldist(c)) {
          c = c->move(i);
          break;
          }
      }
  
    reverse(v.begin(), v.end());
  
    int Q = isize(v)-1;
    // virtualRebase(v[0], false);
    // virtualRebase(v[Q], false);
  
    for(int i=0; i<1000; i++) {
      progress(XLAT("Preparing the line (%1/1000)...", its(i+1)));

      for(int j=1; j<Q; j++) if((j^i)&1) {
      
        // virtualRebase(v[j], false);
        
        hyperpoint prev = calc_relative_matrix(v[j-1]->base, v[j]->base, C0) *
          v[j-1]->at * C0;

        hyperpoint next = calc_relative_matrix(v[j+1]->base, v[j]->base, C0) * 
          v[j+1]->at * C0;
        
        hyperpoint hmid = mid(prev, next);
        
        transmatrix at = rgpushxto0(hmid);

        v[j]->at = at * rspintox(inverse(at) * next);
        fixmatrix(v[j]->at);
        }
      }
    
    hyperpoint next0 = calc_relative_matrix(v[1]->base, v[0]->base, C0) * v[1]->at * C0;
    v[0]->at = v[0]->at * rspintox(inverse(v[0]->at) * next0);
    
    llv = ticks;
    phase = 0;

    if(0)
      for(int j=0; j<=Q; j++) {
        hyperpoint cur = v[j]->at * C0;
        printf("%4d/%3d. %p [%3d] %s\n", j, Q, v[j]->base, celldist(v[j]->base), display(cur));
        }

    }
  
  void movetophase() {
    
    int ph = int(phase);
    int siz = isize(v);
    if(ph<0) ph = 0;
    if(ph >= siz-1) ph = siz-2;
    
    viewctr.at = v[ph]->base->master;
    viewctr.spin = 0;
    
    View = inverse(master_relative(v[ph]->base) * v[ph]->at);
  
    hyperpoint now = v[ph]->at * C0;

    hyperpoint next = calc_relative_matrix(v[ph+1]->base, v[ph]->base, C0) * 
      v[ph+1]->at * C0;
  
    View = spin(M_PI/180 * rotation) * xpush(-(phase-ph) * hdist(now, next)) * View;
    playermoved = false;
    centerover.at = v[ph]->base;
    compute_graphical_distance();
    }
  
  void apply() {
    int t = ticks;
    phase += (t-llv) * lvspeed / 400.;
    llv = t;
    
    int siz = isize(v);
    
    while(phase > siz-1 + extra_line_steps) phase -= (siz + 2 * extra_line_steps-1);
    while(phase < - extra_line_steps) phase += (siz + 2 * extra_line_steps-1);
    
    movetophase();
    }
  
  ld measureLength() {
    ld r = bandhalf * vid.scale;
    
    ld tpixels = 0;
    int siz = isize(v);

    for(int j=0; j<siz-1; j++) {
      hyperpoint next = 
        inverse(v[j]->at) *
        calc_relative_matrix(v[j+1]->base, v[j]->base, C0) * 
        v[j+1]->at * C0;
        
      hyperpoint nextscr;
      applymodel(next, nextscr);
      tpixels += nextscr[0] * r;
      
      if(j == 0 || j == siz-2)
        tpixels += nextscr[0] * r * extra_line_steps;
      }
  
    return tpixels;
    }
  
  void restore();
  void restoreBack();

#if CAP_SDL
  void createImage(bool dospiral) {
    int segid = 1;
    if(includeHistory) restore();
  
    int bandfull = 2*bandhalf;
    ld len = measureLength();
    
    time_t timer;
    timer = time(NULL);
    char timebuf[128]; 
    strftime(timebuf, 128, "%y%m%d-%H%M%S", localtime(&timer));

    vector<SDL_Surface*> bands;
    
    resetbuffer rbuf;
    
    if(1) {
      // block for RAII
      dynamicval<videopar> dv(vid, vid);
      dynamicval<ld> dr(rotation, 0);
      dynamicval<bool> di(inHighQual, true);
      
      renderbuffer glbuf(bandfull, bandfull, vid.usingGL);
      vid.xres = vid.yres = bandfull;
      glbuf.enable(); vid.radius = bandhalf;  
      calcparam();
      stereo::set_viewport(0);
      
      ld xpos = 0;
      
      int seglen = min(int(len), bandsegment);
      
      SDL_Surface *band = SDL_CreateRGBSurface(SDL_SWSURFACE, seglen, bandfull,32,0,0,0,0);
      
      if(!band) {
        addMessage("Could not create an image of that size.");
        }
      else {
  
        int siz = isize(v);
        
        int bonus = ceil(extra_line_steps);
  
        cell *last_base = NULL;
        hyperpoint last_relative;
        
        for(int j=-bonus; j<siz+bonus; j++) {
          /*
          SDL_Surface *buffer = s;
          s = sav; 
          
          pushScreen(progress_screen);
  
          char buf[128];
          sprintf(buf, "#%03d", segid);
  
          progress(s0 + buf + " ("+its(j+bonus)+"/"+its(siz+bonus+bonus-1)+")"); */
  
          // calcparam(); vid.radius = bandhalf;
          phase = j; movetophase();
      
          glbuf.clear(backcolor);
          drawfullmap();
          
          if(last_base) {
            hyperpoint last = ggmatrix(last_base) * last_relative;
            hyperpoint hscr;
            applymodel(last, hscr);
            ld bwidth = -vid.radius * hscr[0];
            printf("bwidth = %lf/%lf\n", bwidth, len);
  
            drawsegment:
            SDL_Surface *gr = glbuf.render();
  
            for(int cy=0; cy<bandfull; cy++) for(int cx=0; cx<=bwidth+3; cx++)
              qpixel(band, int(xpos+cx), cy) = qpixel(gr, int(bandhalf+cx-bwidth), cy);
            
            if(j == 1-bonus)
              xpos = bwidth * (extra_line_steps - bonus);
        
            if(xpos+bwidth > bandsegment) {
              char buf[154];
              sprintf(buf, "bandmodel-%s-%03d" IMAGEEXT, timebuf, segid++);
    
              IMAGESAVE(band, buf);
    
              if(dospiral) 
                bands.push_back(band);
              else 
                SDL_FreeSurface(band);
    
              len -= bandsegment; xpos -= bandsegment;
              seglen = min(int(len), bandsegment);
              band = SDL_CreateRGBSurface(SDL_SWSURFACE, seglen, bandfull,32,0,0,0,0);
              goto drawsegment;
              }  
            xpos += bwidth;      
            }
          
          last_base = viewctr.at->c7;
          last_relative = inverse(ggmatrix(last_base)) * C0;        
          }
        }

      char buf[154];
      sprintf(buf, "bandmodel-%s-%03d" IMAGEEXT, timebuf, segid++);
      IMAGESAVE(band, buf);
      addMessage(XLAT("Saved the band image as: ") + buf);
  
      if(dospiral) 
        bands.push_back(band);
      else 
        SDL_FreeSurface(band);
      }

    rbuf.reset();
    stereo::set_viewport(0);

    if(includeHistory) restoreBack();
    
    if(dospiral) {
      spiral::loop(bands);
      for(int i=0; i<isize(bands); i++) SDL_FreeSurface(bands[i]);
      }
    }
#endif

  const char *modelnames[MODELCOUNT] = {
    "disk", "half-plane", "band", "polygonal", "polynomial",
    "azimuthal equidistant", "azimuthal equi-area", 
    "ball model", "Minkowski hyperboloid", "hemisphere",
    "band equidistant", "band equi-area", "sinusoidal", "two-point equidistant",
    "fisheye"
    };
  
  string get_model_name(eModel pm) {
    return XLAT(
      pm == mdBand && sphere ? "Mercator" : 
      pm == mdHalfplane && euclid ? "inversion" : 
      pm == mdHemisphere && !hyperbolic ? "sphere" :
      pm == mdHyperboloid && euclid ? "plane" :
      pm == mdHyperboloid && sphere ? "sphere" :
      modelnames[pm]);
    }
  
  bool model_available(eModel pm) {
    if(sphere && (pm == mdHalfplane || pm == mdBall))
      return false;
    return true;
    }    
  
  void model_menu() {
    cmode = sm::SIDE | sm::MAYDARK | sm::CENTER;
    gamescreen(0);
    dialog::init(XLAT("models of hyperbolic geometry"));
    for(int i=0; i<mdGUARD; i++) {
      eModel m = eModel(i);
      if(model_available(m))
        dialog::addBoolItem(get_model_name(m), pmodel == m, "0123456789!@#$%^&*()" [m]);
      }
    
    dialog::addBreak(100);

    dialog::addBoolItem(XLAT("rotation"), do_rotate == 2, 'r');
    if(do_rotate == 0) dialog::lastItem().value = XLAT("NEVER");
    dialog::lastItem().value += " " + its(rotation) + "°";

    // if(pmodel == mdBand && sphere)
    dialog::addSelItem(XLAT("scale factor"), fts(vid.scale), 'z');
    
    if(abs(vid.alpha-1) > 1e-3 && pmodel != mdBall && pmodel != mdHyperboloid && pmodel != mdHemisphere && pmodel != mdDisk) {
      dialog::addBreak(50);
      dialog::addInfo("NOTE: this works 'correctly' only if the Poincaré model/stereographic projection is used.");
      dialog::addBreak(50);
      }
    
    if(pmodel == mdDisk || pmodel == mdBall || pmodel == mdHyperboloid) {
      dialog::addSelItem(XLAT("Projection at the ground level"), fts3(vid.alpha), 'p');
      }
    
    if(pmodel == mdPolynomial) {
      dialog::addSelItem(XLAT("coefficient"), 
        fts4(polygonal::coefr[polygonal::coefid]), 'x');
      dialog::addSelItem(XLAT("coefficient (imaginary)"), 
        fts4(polygonal::coefi[polygonal::coefid]), 'y');
      dialog::addSelItem(XLAT("which coefficient"), its(polygonal::coefid), 'n');
      }

    if(pmodel == mdHalfplane)
      dialog::addBoolItem(XLAT("lower half-plane"), lower_halfplane, 'l');

    if(pmodel == mdBall)
      dialog::addSelItem(XLAT("projection in ball model"), fts3(vid.ballproj), 'x');

    if(pmodel == mdPolygonal) {
      dialog::addSelItem(XLAT("polygon sides"), its(polygonal::SI), 'x');
      dialog::addSelItem(XLAT("star factor"), fts(polygonal::STAR), 'y');
      dialog::addSelItem(XLAT("degree of the approximation"), its(polygonal::deg), 'n');
      }
    
    if(pmodel == mdBall || pmodel == mdHyperboloid || pmodel == mdHemisphere) {
      dialog::addSelItem(XLAT("camera rotation in 3D models"), fts3(vid.ballangle), 'b');
      }    
    
    if(pmodel == mdHemisphere && euclid) {
      dialog::addSelItem(XLAT("parameter"), fts3(vid.euclid_to_sphere), 'l');
      }
      
    if(pmodel == mdTwoPoint) {
      dialog::addSelItem(XLAT("parameter"), fts3(vid.twopoint_param), 'l');
      }

    dialog::addSelItem(XLAT("vertical stretch"), fts3(vid.stretch), 's');
      
    dialog::addBreak(100);
    dialog::addItem(XLAT("history mode"), 'a');
#if CAP_RUG
    dialog::addItem(XLAT("hypersian rug mode"), 'u');
#endif
    dialog::addBack();
    dialog::display();
    
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      
      if(uni >= '0' && uni <= '9') {
        pmodel = eModel(uni - '0');
        polygonal::solve();
        vid.alpha = 1; vid.scale = 1;
        if(pmodel == mdBand && sphere)
          vid.scale = .3;
        if(pmodel == mdDisk && sphere)
          vid.scale = .4;
        }
      else if(uni == '!') 
        pmodel = eModel(10);
      else if(uni == '@') 
        pmodel = eModel(11);
      else if(uni == '#') 
        pmodel = eModel(12);
      else if(uni == '$') 
        pmodel = eModel(13);
      else if(uni == '%') 
        pmodel = eModel(14);
      else if(uni == '6')
        vid.alpha = 1, vid.scale = 1;
      else if(uni == 'z')
        editScale();
      else if(uni == 'p')
        projectionDialog();
      else if(uni == 'b') 
        config_camera_rotation();
#if CAP_RUG
      else if(uni == 'u')
        pushScreen(rug::show);
#endif
      else if(uni == 'l' && pmodel == mdHalfplane)
        lower_halfplane = !lower_halfplane;
      else if(uni == 's') {
       dialog::editNumber(vid.stretch, 0, 10, .1, 1, XLAT("vertical stretch"), 
          "Vertical stretch factor."
          );
        dialog::extra_options = [] () {
          dialog::addBreak(100);
          if(sphere && pmodel == mdBandEquiarea) {
            dialog::addBoolItem("Gall-Peters", vid.stretch == 2, 'o');
            dialog::add_action([] { vid.stretch = 2; dialog::ne.s = "2"; });
            }
          if(pmodel == mdBandEquiarea) {
            // y = K * sin(phi)
            // cos(phi) * cos(phi) = 1/K
            if(sphere && vid.stretch >= 1) {
              ld phi = acos(sqrt(1/vid.stretch));
              dialog::addInfo(XLAT("The current value makes the map conformal at the latitude of %1 (%2°).", fts(phi), fts(phi * 180 / M_PI)));
              }
            else if(hyperbolic && abs(vid.stretch) <= 1 && abs(vid.stretch) >= 1e-9) {
              ld phi = acosh(abs(sqrt(1/vid.stretch)));
              dialog::addInfo(XLAT("The current value makes the map conformal %1 units from the main line.", fts(phi)));
              }
            else dialog::addInfo("");
            }
          };
        }
      else if(uni == 'a')
        pushScreen(history_menu);
      else if(uni == 'l' && pmodel == mdHemisphere && euclid)  {
        dialog::editNumber(vid.euclid_to_sphere, 0, 10, .1, 1, XLAT("parameter"), 
          "Stereographic projection to a sphere. Choose the radius of the sphere."
          );
        dialog::scaleLog();
        }
      else if(uni == 'l' && pmodel == mdTwoPoint)  {
        dialog::editNumber(vid.twopoint_param, 0, 10, .1, 1, XLAT("parameter"), 
          "This model maps the world so that the distances from two points "
          "are kept. This parameter gives the distance from the two points to "
          "the center."
          );
        dialog::scaleLog();
        }
      else if(uni == 'x' && pmodel == mdBall) 
        dialog::editNumber(vid.ballproj, 0, 100, .1, 0, XLAT("projection in ball model"), 
          "This parameter affects the ball model the same way as the projection parameter affects the disk model.");
      else if(sym == 'x' && pmodel == mdPolygonal)
        dialog::editNumber(polygonal::SI, 3, 10, 1, 4, XLAT("polygon sides"), "");
      else if(sym == 'y' && pmodel == mdPolygonal)
        dialog::editNumber(polygonal::STAR, -1, 1, .1, 0, XLAT("star factor"), "");
      else if(sym == 'n' && pmodel == mdPolygonal)
        dialog::editNumber(polygonal::deg, 2, polygonal::MSI-1, 1, 2, XLAT("degree of the approximation"), "");
      else if(sym == 'x' && pmodel == mdPolynomial)  {
        polygonal::maxcoef = max(polygonal::maxcoef, polygonal::coefid);
        int ci = polygonal::coefid + 1;
        dialog::editNumber(polygonal::coefr[polygonal::coefid], -10, 10, .01/ci/ci, 0, XLAT("coefficient"), "");
        }
      else if(sym == 'y' && pmodel == mdPolynomial) {
        polygonal::maxcoef = max(polygonal::maxcoef, polygonal::coefid);
        int ci = polygonal::coefid + 1;
        dialog::editNumber(polygonal::coefi[polygonal::coefid], -10, 10, .01/ci/ci, 0, XLAT("coefficient (imaginary)"), "");
        }
      else if(sym == 'n' && pmodel == mdPolynomial)
        dialog::editNumber(polygonal::coefid, 0, polygonal::MSI-1, 1, 0, XLAT("which coefficient"), "");
      else if(sym == 'r') {
        if(rotation < 0) rotation = 0;
        dialog::editNumber(rotation, 0, 360, 90, 0, XLAT("rotation"), 
          "This controls the automatic rotation of the world. "
          "It affects the line animation in the history mode, and "
          "lands which have a special direction. Note that if finding this special direction is a part of the puzzle, "
          "it works only in the cheat mode.");
        dialog::dialogflags |= sm::CENTER;
        dialog::extra_options = [] () {
          dialog::addBreak(100);
          dialog::addBoolItem("line animation only", conformal::do_rotate == 0, 'n');
          dialog::add_action([] () { conformal::do_rotate = 0; });
          dialog::addBoolItem("gravity lands", conformal::do_rotate == 1, 'g');
          dialog::add_action([] () { conformal::do_rotate = 1; });
          dialog::addBoolItem("all directional lands", conformal::do_rotate == 2, 'd');
          dialog::add_action([] () { conformal::do_rotate = 2; });
          };
        }
      else if(doexiton(sym, uni)) popScreen();
      };
    }

  bool band_renderable_now() {
    return on && (pmodel == mdBand || pmodel == mdBandEquidistant || pmodel == mdBandEquiarea) && !euclid && !sphere;
    }
  
  void history_menu() {
    cmode = sm::SIDE | sm::MAYDARK;
    gamescreen(0);
    
    dialog::init(XLAT("history mode"));

    dialog::addBoolItem(XLAT("include history"), (includeHistory), 'i');
    
    // bool notconformal0 = (pmodel >= 5 && pmodel <= 6) && !euclid;
    // bool notconformal = notconformal0 || abs(vid.alpha-1) > 1e-3;

    dialog::addSelItem(XLAT("model used"), get_model_name(pmodel), 'm');
    
    if(!bounded && !euclid) dialog::addBoolItem(XLAT("prepare the line animation"), (on), 'e');
    if(on) dialog::addSelItem(XLAT("animation speed"), fts(lvspeed), 'a');
    dialog::addSelItem(XLAT("extend the ends"), fts(extra_line_steps), 'p');
    
#if CAP_SDL
    dialog::addBoolItem(XLAT("render bands automatically"), (autoband), 'o');
    if(autoband)
      dialog::addBoolItem(XLAT("include history when auto-rendering"), (autobandhistory), 'j');
    
    if(band_renderable_now() || autoband) {
      dialog::addSelItem(XLAT("band width"), "2*"+its(bandhalf), 'd');
      dialog::addSelItem(XLAT("length of a segment"), its(bandsegment), 's');
      dialog::addBoolItem(XLAT("spiral on rendering"), (dospiral), 'g');
      if(band_renderable_now())
        dialog::addItem(XLAT("render now (length: %1)", fts(measureLength())), 'f');
      }
#endif
      
    dialog::addBack();
    dialog::display();
    mouseovers = XLAT("see http://www.roguetemple.com/z/hyper/models.php");
    keyhandler = handleKeyC;
    }

  void handleKeyC(int sym, int uni) {
    dialog::handleNavigation(sym, uni);
  
    if(uni == 'e') {
      if(on) clear();
      else {
        if(canmove && !cheater) {
          addMessage("Enable cheat mode or GAME OVER to use this");
          return;
          }
        if(canmove && cheater) cheater++;
        create();
        }
      }
    else if(uni == 'o') 
      autoband = !autoband;
    else if(uni == 'm') 
      pushScreen(model_menu);
    else if(sym == 'a') 
      dialog::editNumber(lvspeed, -5, 5, .1, 1, XLAT("animation speed"), "");
    else if(sym == 'd') 
      dialog::editNumber(bandhalf, 5, 1000, 5, 200, XLAT("band width"), "");
    else if(sym == 's') 
      dialog::editNumber(bandsegment, 500, 32000, 500, 16000, XLAT("band segment"), "");
    else if(sym == 'p') 
      dialog::editNumber(extra_line_steps, 0, 5, 1, 1, XLAT("extend the ends"), 
        "0 = start at the game start, endat the end position; "
        "larger numbers give extra space at the ends."
        );
    else if(sym == 'g') { dospiral = !dospiral; }
    else if(sym == 'i') { 
      if(canmove && !cheater) {
        addMessage("Enable cheat mode or GAME OVER to use this");
        return;
        }
      if(canmove && cheater) cheater++;
      includeHistory = !includeHistory; 
      }
#if CAP_SDL
    else if(uni == 'f' && band_renderable_now()) createImage(dospiral);
#endif
    else if(sym == 'j') { 
      autobandhistory = !autobandhistory; 
      }
    else if(doexiton(sym, uni)) popScreen();
    }
  
  set<cell*> inmovehistory, inkillhistory, infindhistory;
  
  void restore() {
    inmovehistory.clear();
    inkillhistory.clear();
    infindhistory.clear();
    for(int i=0; i<isize(movehistory); i++)
      inmovehistory.insert(movehistory[i]);
    int sk = isize(killhistory);
    for(int i=0; i<sk; i++) {
      eMonster m = killhistory[i].second;
      killhistory[i].second = killhistory[i].first->monst;
      killhistory[i].first->monst = m;
      inkillhistory.insert(killhistory[i].first);
      }
    int si = isize(findhistory);
    for(int i=0; i<si; i++) {
      eItem m = findhistory[i].second;
      findhistory[i].second = findhistory[i].first->item;
      findhistory[i].first->item = m;
      infindhistory.insert(findhistory[i].first);
      }
    }

  void restoreBack() {
    int sk = isize(killhistory);
    for(int i=sk-1; i>=0; i--) {
      eMonster m = killhistory[i].second;
      killhistory[i].second = killhistory[i].first->monst;
      killhistory[i].first->monst = m;
      }
    int si = isize(findhistory);
    for(int i=si-1; i>=0; i--) {
      eItem m = findhistory[i].second;
      findhistory[i].second = findhistory[i].first->item;
      findhistory[i].first->item = m;
      }
    }
  
  void renderAutoband() {
#if CAP_SDL
    if(!cwt.at || celldist(cwt.at) <= 7) return;
    if(!autoband) return;
    eModel spm = pmodel;
    bool ih = includeHistory;
    includeHistory = autobandhistory;
    pmodel = mdBand;
    create();
    createImage(dospiral);
    clear();
    pmodel = spm;
    includeHistory = ih;
#endif
    }

  auto hooks = addHook(clearmemory, 0, [] () {
    conformal::renderAutoband();
    conformal::on = false;
    conformal::killhistory.clear();
    conformal::findhistory.clear();
    conformal::movehistory.clear();
    conformal::includeHistory = false;
    });

  }

}
