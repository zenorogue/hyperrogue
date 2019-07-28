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

  typedef complex<xld> cxld;

  int SI = 4;
  ld  STAR = 0;
  
  int deg = ISMOBWEB ? 2 : 20;

  precise matrix[MSI][MSI];
  precise ans[MSI];
  
  cxld coef[MSI];
  ld coefr[MSI], coefi[MSI]; 
  int maxcoef, coefid;
  
  void solve() {
    if(pmodel == mdPolynomial) {
      for(int i=0; i<MSI; i++) coef[i] = cxld(coefr[i], coefi[i]);
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
      cxld z(x,y);
      cxld res (0,0);
      for(int i=maxcoef; i>=0; i--) { res += coef[i]; if(i) res *= z; }
      return make_pair(real(res), imag(res));    
      }
      
    cxld z(x, y);
    cxld res (0,0);
    cxld zp = 1; for(int i=0; i<SI; i++) zp *= z;

    for(int i=prec; i>0; i--) { 
      res += ans[i]; 
      res *= zp;
      }
    res += ans[0]; res *= z;
    return make_pair(real(res), imag(res));
    }

  pair<ld, ld> compute(ld x, ld y) { return compute(x,y,deg); }
  }

#if CAP_SDL
namespace spiral {

  typedef long double ld;
  typedef complex<long double> cxld;

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

//   cxld mnoznik = cxld(0, M_PI) / cxld(k, M_PI);

    cxld factor = cxld(0, -CY/2/M_PI/M_PI) * cxld(k, M_PI);
    
    Yshift = CY * k / M_PI;
    
    quickmap.clear();
    
    double xc = ((SX | 1) - 2) / 2.;
    double yc = ((SY | 1) - 2) / 2.;
    
    for(int y=0; y<SY; y++)
    for(int x=0; x<SX; x++) {
      cxld z(x-xc, y-yc);
      cxld z1 = log(z);

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
          int sym = event.key.keysym.sym;
          int uni = event.key.keysym.unicode;
          numlock_on = event.key.keysym.mod & KMOD_NUM;
          if(DKEY == SDLK_RIGHT) velx++;
          if(DKEY == SDLK_LEFT) velx--;
          if(DKEY == SDLK_UP) vely++;
          if(DKEY == SDLK_DOWN) vely--;
          if(sym == SDLK_ESCAPE) goto breakloop;
          if(uni == 'h') displayhelp = !displayhelp;
          if(uni == 's') dosave = true;
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

  string formula = "z^2";
  eModel basic_model;

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
  ld rotation_xz = 90;
  ld rotation_xy2 = 90;
  int do_rotate = 1;
  ld model_orientation, halfplane_scale, model_orientation_yz;
  ld clip_min, clip_max;
  ld ocos, osin, ocos_yz, osin_yz;
  ld cos_ball, sin_ball;
  bool model_straight, model_straight_yz;
  ld top_z = 5;
  ld model_transition = 1;

  bool autoband = false;
  bool autobandhistory = false;
  bool dospiral = true;
  
  ld extra_line_steps = 0;
  
  vector<cell*> path_for_lineanimation;

  void clear() {
    on = false;
    int N = isize(v);
    for(int i=0; i<N; i++) delete v[i];
    v.resize(0);
    }

  void create(cell *start, cell *target, transmatrix last) {
    
    if(target == start && !(quotient && isize(path_for_lineanimation) > 1)) {
      addMessage("Must go a distance from the starting point");
      return;
      }
    
    on = true;
  
    if(!quotient) try {
      path_for_lineanimation = build_shortest_path(start, target);
      }
    catch(hr_shortest_path_exception&) {
      addMessage("Error: could not build a path");
      return;
      }

    for(cell *c: path_for_lineanimation) {
      shmup::monster *m = new shmup::monster;
      m->at = Id;
      m->base = c;
      v.push_back(m);
      }
    
    v.back()->at = last;
  
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
    }

  void create_playerpath() {
    create(currentmap->gamestart(), cwt.at, Id);
    }
  
  void create_recenter_to_view(bool precise) {
    cell *c = centerover.at ? centerover.at : cwt.at;
    create(path_for_lineanimation[0], c, precise ? inverse(ggmatrix(c)) : Id);
    }
  
  transmatrix rotmatrix() {
    if(DIM == 2) return spin(rotation * degree);
    return spin(rotation_xy2 * degree) * cspin(0, 2, -rotation_xz * degree) * spin(rotation * degree);
    }
  
  void movetophase() {
    
    int ph = int(phase);
    int siz = isize(v);
    if(ph<0) ph = 0;
    if(ph >= siz-1) ph = siz-2;
    
    heptagon *old = viewctr.at;
    
    viewctr.at = v[ph]->base->master;
    viewctr.spin = 0;
    
    ld angle = 0;
    if(DIM == 3) {
      hyperpoint h = inverse(rotmatrix()) * View * hpxy3(1,2,3);
      angle = atan2(h[1], h[2]);
      }
    
    View = inverse(master_relative(v[ph]->base) * v[ph]->at);
  
    hyperpoint now = v[ph]->at * C0;

    hyperpoint next = calc_relative_matrix(v[ph+1]->base, v[ph]->base, C0) * 
      v[ph+1]->at * C0;
  
    View = xpush(-(phase-ph) * hdist(now, next)) * View;
    if(WDIM == 2) {
      View = rotmatrix() * View;
      }
    else {
      if(celldistance(v[ph]->base, old->c7) <= 2) {
        hyperpoint h1 = View * currentmap->relative_matrix(old, viewctr.at) * hpxy3(1,2,3);
        ld angle1 = atan2(h1[1], h1[2]);
        View = cspin(2, 1, angle1 - angle) * View;
        }
      View = rotmatrix() * View;
      }

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
  
  ld spiral_angle = 70, spiral_x = 10, spiral_y = 7;
  int spiral_id = 7;
  bool use_atan = false;
  
  cld spiral_multiplier;
  ld right_spiral_multiplier = 1;
  ld any_spiral_multiplier = 1;
  ld sphere_spiral_multiplier = 2;
  ld spiral_cone = 360;
  ld spiral_cone_rad;
  bool ring_not_spiral;

  void configure() {
    ld ball = -vid.ballangle * degree;
    cos_ball = cos(ball), sin_ball = sin(ball);
    ocos = cos(model_orientation * degree);
    osin = sin(model_orientation * degree);
    ocos_yz = cos(model_orientation_yz * degree);
    osin_yz = sin(model_orientation_yz * degree);
    model_straight = (ocos > 1 - 1e-9);
    model_straight_yz = DIM == 2 || (ocos_yz > 1-1e-9);
    if(conformal::on) conformal::apply();
    
    if(!euclid) {
      ld b = spiral_angle * degree;
      ld cos_spiral = cos(b);
      ld sin_spiral = sin(b);
      spiral_cone_rad = spiral_cone * degree;
      ring_not_spiral = abs(cos_spiral) < 1e-3;
      ld mul = 1;
      if(sphere) mul = .5 * sphere_spiral_multiplier;
      else if(ring_not_spiral) mul = right_spiral_multiplier;
      else mul = any_spiral_multiplier * cos_spiral;
      
      spiral_multiplier = cld(cos_spiral, sin_spiral) * cld(spiral_cone_rad * mul / 2., 0);
      }
    if(euclid) {
      hyperpoint h = tC0(eumove(spiral_x, spiral_y));
      spiral_multiplier = cld(0, 2 * M_PI) / cld(h[0], h[1]);
      }
    
    if(centerover.at && !on)
    if(isize(path_for_lineanimation) == 0 || (quotient && path_for_lineanimation.back() != centerover.at)) {
      path_for_lineanimation.push_back(centerover.at);
      }

    band_shift = 0;
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
      glbuf.enable(); current_display->radius = bandhalf;  
      calcparam();
      
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
  
          // calcparam(); current_display->radius = bandhalf;
          phase = j; movetophase();
      
          glbuf.clear(backcolor);
          drawfullmap();
          
          if(last_base) {
            hyperpoint last = ggmatrix(last_base) * last_relative;
            hyperpoint hscr;
            applymodel(last, hscr);
            ld bwidth = -current_display->radius * hscr[0];
            print(hlog, "bwidth = ", bwidth, "/", len);
  
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

    if(includeHistory) restoreBack();
    
    if(dospiral) {
      spiral::loop(bands);
      for(int i=0; i<isize(bands); i++) SDL_FreeSurface(bands[i]);
      }
    }
#endif

  bool model_available(eModel pm) {
    if(sol) return among(pm, mdDisk, mdPerspective, mdSolPerspective);
    if(pm == mdSolPerspective && !sol) return false;
    if(sphere && (pm == mdHalfplane || pm == mdBall))
      return false;
    if(DIM == 2 && pm == mdPerspective) return false;
    if(DIM == 2 && pm == mdEquivolume) return false;
    if(DIM == 3 && among(pm, mdBall, mdHyperboloid, mdFormula, mdPolygonal, mdRotatedHyperboles, mdSpiral, mdHemisphere)) return false;
    if(pm == mdCentralInversion && !euclid) return false;
    return true;
    }    
  
  bool model_has_orientation() {
    return
      among(pmodel, mdHalfplane, mdPolynomial, mdPolygonal, mdTwoPoint, mdJoukowsky, mdJoukowskyInverted, mdSpiral, mdSimulatedPerspective, mdTwoHybrid) || mdBandAny();
    }
  
  bool model_has_transition() {
    return among(pmodel, mdJoukowsky, mdJoukowskyInverted, mdBand) && DIM == 2;
    }
  
  int editpos = 0;
  
  string get_model_name(eModel m) {
    if(m == mdDisk && DIM == 3 && hyperbolic) return XLAT("ball model/Gans");
    if(sol) {
      if(m == mdDisk) return XLAT("simple model: projection");
      if(m == mdPerspective) return XLAT("simple model: perspective");
      if(m == mdSolPerspective) return XLAT("native perspective");
      }
    if(m == mdDisk && DIM == 3) return XLAT("perspective in 4D");
    if(m == mdHalfplane && DIM == 3 && hyperbolic) return XLAT("half-space");
    if(sphere) 
      return XLAT(models[m].name_spherical);
    if(euclid) 
      return XLAT(models[m].name_euclidean);
    if(hyperbolic) 
      return XLAT(models[m].name_hyperbolic);
    return "?";
    }

  vector<pair<int, int> > torus_zeros;

  void match_torus_period() {
    torus_zeros.clear();
    for(int y=0; y<=200; y++)
    for(int x=-200; x<=200; x++) {
      if(y == 0 && x <= 0) continue;
      auto zero = vec_to_cellwalker(euclid_getvec(x, y));
      if(zero.at == currentmap->gamestart() && !zero.mirrored)
        torus_zeros.emplace_back(x, y);      
      }
    sort(torus_zeros.begin(), torus_zeros.end(), [] (const pair<int,int> p1, const pair<int, int> p2) {
      ld d1 = hdist0(tC0(eumove(p1.first, p1.second)));
      ld d2 = hdist0(tC0(eumove(p2.first, p2.second)));
      if(d1 < d2 - 1e-6) return true;
      if(d1 > d2 + 1e-6) return false;
      return p1 < p2;
      });
    if(spiral_id > isize(torus_zeros)) spiral_id = 0;
    dialog::editNumber(spiral_id, 0, isize(torus_zeros)-1, 1, 10, XLAT("match the period of the torus"), "");
    dialog::reaction = [] () {
      tie(spiral_x, spiral_y) = torus_zeros[spiral_id];
      };
    dialog::bound_low(0);
    dialog::bound_up(isize(torus_zeros)-1);
    }
  
  void edit_formula() {
    if(pmodel != mdFormula) basic_model = pmodel;
    dialog::edit_string(formula, "formula", 
      XLAT(
      "This lets you specify the projection as a formula f. "
      "The formula has access to the value 'z', which is a complex number corresponding to the (x,y) coordinates in the currently selected model; "
      "the point z is mapped to f(z). You can also use the underlying coordinates ux, uy, uz."
      ) + "\n\n" + parser_help()
      );
    #if CAP_QUEUE && CAP_CURVE
    dialog::extra_options = [] () {
      initquickqueue();
      queuereset(mdUnchanged, PPR::LINE);              
      for(int a=-1; a<=1; a++) {
        curvepoint(point2(-M_PI/2 * current_display->radius, a*current_display->radius));
        curvepoint(point2(+M_PI/2 * current_display->radius, a*current_display->radius));
        queuecurve(forecolor, 0, PPR::LINE);
        curvepoint(point2(a*current_display->radius, -M_PI/2*current_display->radius));
        curvepoint(point2(a*current_display->radius, +M_PI/2*current_display->radius));
        queuecurve(forecolor, 0, PPR::LINE);
        }
      queuereset(pmodel, PPR::LINE);
      quickqueue();
      };
    #endif
    dialog::reaction_final = [] () {
      pmodel = mdFormula;
      };
    }
  
  void edit_rotation(ld& which) {
    dialog::editNumber(which, 0, 360, 90, 0, XLAT("rotation"), 
      "This controls the automatic rotation of the world. "
      "It affects the line animation in the history mode, and "
      "lands which have a special direction. Note that if finding this special direction is a part of the puzzle, "
      "it works only in the cheat mode.");
    dialog::dialogflags |= sm::CENTER;
    dialog::extra_options = [] () {
      dialog::addBreak(100);
      dialog::addBoolItem_choice("line animation only", conformal::do_rotate, 0, 'N');
      dialog::addBoolItem_choice("gravity lands", conformal::do_rotate, 1, 'G');
      dialog::addBoolItem_choice("all directional lands", conformal::do_rotate, 2, 'D');
      if(DIM == 3) {
        dialog::addBreak(100);
        dialog::addSelItem(XLAT("XY plane"), fts(conformal::rotation) + "°", 'A');
        dialog::add_action([] { popScreen(); edit_rotation(conformal::rotation); });
        dialog::addSelItem(XLAT("XZ plane"), fts(conformal::rotation_xz) + "°", 'B');
        dialog::add_action([] { popScreen(); edit_rotation(conformal::rotation_xz); });
        dialog::addSelItem(XLAT("XY plane #2"), fts(conformal::rotation_xy2) + "°", 'C');
        dialog::add_action([] { popScreen(); edit_rotation(conformal::rotation_xy2); });
        }
      };
    }
  
  void model_menu() {
    cmode = sm::SIDE | sm::MAYDARK | sm::CENTER;
    gamescreen(0);
    dialog::init(XLAT("models & projections"));
    for(int i=0; i<mdGUARD; i++) {
      eModel m = eModel(i);
      if(m == mdFormula && ISMOBILE) continue;
      if(model_available(m)) {
        dialog::addBoolItem(get_model_name(m), pmodel == m, "0123456789!@#$%^&*()][{}'" [m]);
        dialog::add_action([m] () {
          if(m == mdFormula) {
            edit_formula();
            return;
            }
          pmodel = m;
          polygonal::solve();
          vid.alpha = 1; vid.scale = 1;
          if(pmodel == mdBand && sphere)
            vid.scale = .3;
          if(pmodel == mdDisk && sphere)
            vid.scale = .4;
          });
        }
      }
    
    dialog::addBreak(100);
    
    if(sol)
      dialog::addBoolItem_action(XLAT("geodesic movement in Sol"), solv::geodesic_movement, 'G');

    dialog::addBoolItem(XLAT("rotation"), do_rotate == 2, 'r');
    if(do_rotate == 0) dialog::lastItem().value = XLAT("NEVER");
    if(DIM == 2)
      dialog::lastItem().value += " " + its(rotation) + "°";
    else
      dialog::lastItem().value += " " + its(rotation) + "°" + its(rotation_xz) + "°" + its(rotation_xy2) + "°";
    dialog::add_action([] { edit_rotation(conformal::rotation); });
    
    // if(pmodel == mdBand && sphere)
    if(!in_perspective())
      dialog::addSelItem(XLAT("scale factor"), fts(vid.scale), 'z');
    
    if(abs(vid.alpha-1) > 1e-3 && pmodel != mdBall && pmodel != mdHyperboloid && pmodel != mdHemisphere && pmodel != mdDisk) {
      dialog::addBreak(50);
      dialog::addInfo("NOTE: this works 'correctly' only if the Poincaré model/stereographic projection is used.");
      dialog::addBreak(50);
      }
    
    if(among(pmodel, mdDisk, mdBall, mdHyperboloid, mdRotatedHyperboles)) {
      dialog::addSelItem(XLAT("projection distance"), 
      fts(vid.alpha) + " (" + current_proj_name() + ")", 'p');
      }
                                  
    if(model_has_orientation()) {
      dialog::addSelItem(XLAT("model orientation"), fts(model_orientation) + "°", 'l');
      dialog::add_action([] () {
        dialog::editNumber(model_orientation, 0, 360, 90, 0, XLAT("model orientation"), "");
        });
      if(DIM == 3) {
        dialog::addSelItem(XLAT("model orientation (y/z plane)"), fts(model_orientation_yz) + "°", 'L');
        dialog::add_action([] () {
          dialog::editNumber(model_orientation_yz, 0, 360, 90, 0, XLAT("model orientation (y/z plane)"), "");
          });
        }
      }
        
  if(DIM == 3 && pmodel != mdPerspective) {
    const string cliphelp = XLAT(
      "Your view of the 3D model is naturally bounded from four directions by your window. "
      "Here, you can also set up similar bounds in the Z direction. Radius of the ball/band "
      "models, and the distance from the center to the plane in the halfspace model, are 1.\n\n");
    dialog::addSelItem(XLAT("near clipping plane"), fts(clip_max), 'c');
    dialog::add_action([cliphelp] () {
      dialog::editNumber(clip_max, -10, 10, 0.2, 1, XLAT("near clipping plane"), 
        cliphelp + XLAT("Objects with Z coordinate "
          "bigger than this parameter are not shown. This is useful with the models which "
          "extend infinitely in the Z direction, or if you want things close to your character "
          "to be not obscured by things closer to the camera."));
      });
    dialog::addSelItem(XLAT("far clipping plane"), fts(clip_min), 'C');
    dialog::add_action([cliphelp] () {
      dialog::editNumber(clip_min, -10, 10, 0.2, -1, XLAT("far clipping plane"), 
        cliphelp + XLAT("Objects with Z coordinate "
          "smaller than this parameter are not shown; it also affects the fog effect"
          " (near clipping plane = 0% fog, far clipping plane = 100% fog)."));
      });
    }
    
    if(pmodel == mdPolynomial) {
      dialog::addSelItem(XLAT("coefficient"), 
        fts(polygonal::coefr[polygonal::coefid]), 'x');
      dialog::add_action([] () {
        polygonal::maxcoef = max(polygonal::maxcoef, polygonal::coefid);
        int ci = polygonal::coefid + 1;
        dialog::editNumber(polygonal::coefr[polygonal::coefid], -10, 10, .01/ci/ci, 0, XLAT("coefficient"), "");
        });
      dialog::addSelItem(XLAT("coefficient (imaginary)"), 
        fts(polygonal::coefi[polygonal::coefid]), 'y');
      dialog::add_action([] () {
        polygonal::maxcoef = max(polygonal::maxcoef, polygonal::coefid);
        int ci = polygonal::coefid + 1;
        dialog::editNumber(polygonal::coefi[polygonal::coefid], -10, 10, .01/ci/ci, 0, XLAT("coefficient (imaginary)"), "");
        });
      dialog::addSelItem(XLAT("which coefficient"), its(polygonal::coefid), 'n');
      dialog::add_action([] () {
        dialog::editNumber(polygonal::coefid, 0, polygonal::MSI-1, 1, 0, XLAT("which coefficient"), "");
        dialog::bound_low(0); dialog::bound_up(polygonal::MSI-1);
        });
      }

    if(pmodel == mdHalfplane) {
      dialog::addSelItem(XLAT("half-plane scale"), fts(halfplane_scale), 'b');
      dialog::add_action([] () {
        dialog::editNumber(halfplane_scale, 0, 2, 0.25, 1, XLAT("half-plane scale"), "");
        });
      }

    if(pmodel == mdRotatedHyperboles) {
      dialog::addBoolItem_action(XLAT("use atan to make it finite"), use_atan, 'x');
      }

    if(pmodel == mdBall) {
      dialog::addSelItem(XLAT("projection in ball model"), fts(vid.ballproj), 'x');
      dialog::add_action([] () {
        dialog::editNumber(vid.ballproj, 0, 100, .1, 0, XLAT("projection in ball model"), 
          "This parameter affects the ball model the same way as the projection parameter affects the disk model.");
        });
      }

    if(pmodel == mdPolygonal) {
      dialog::addSelItem(XLAT("polygon sides"), its(polygonal::SI), 'x');
      dialog::add_action([] () {
        dialog::editNumber(polygonal::SI, 3, 10, 1, 4, XLAT("polygon sides"), "");
        dialog::reaction = polygonal::solve;
        });
      dialog::addSelItem(XLAT("star factor"), fts(polygonal::STAR), 'y');
      dialog::add_action([]() {
        dialog::editNumber(polygonal::STAR, -1, 1, .1, 0, XLAT("star factor"), "");
        dialog::reaction = polygonal::solve;
        });
      dialog::addSelItem(XLAT("degree of the approximation"), its(polygonal::deg), 'n');
      dialog::add_action([](){
        dialog::editNumber(polygonal::deg, 2, polygonal::MSI-1, 1, 2, XLAT("degree of the approximation"), "");
        dialog::reaction = polygonal::solve;
        dialog::bound_low(0); dialog::bound_up(polygonal::MSI-1);
        });
      }
    
    if(pmodel == mdBall || pmodel == mdHyperboloid || pmodel == mdHemisphere || (pmodel == mdSpiral && spiral_cone != 360)) {
      dialog::addSelItem(XLAT("camera rotation in 3D models"), fts(vid.ballangle) + "°", 'b');
      dialog::add_action(config_camera_rotation);
      }
    
    if(pmodel == mdHyperboloid) {
      dialog::addSelItem(XLAT("maximum z coordinate to show"), fts(top_z), 'l');
      dialog::add_action([](){
        dialog::editNumber(top_z, 1, 20, 0.25, 4, XLAT("maximum z coordinate to show"), "");
        });
      }
    
    if(model_has_transition()) {
      dialog::addSelItem(XLAT("model transition"), fts(model_transition), 't');
      dialog::add_action([]() {
        dialog::editNumber(model_transition, 0, 1, 0.1, 1, XLAT("model transition"), 
          "You can change this parameter for a transition from another model to this one."
          );
        });
      }

    if(among(pmodel, mdJoukowsky, mdJoukowskyInverted, mdSpiral) && DIM == 2) {
      dialog::addSelItem(XLAT("Möbius transformations"), fts(vid.skiprope) + "°", 'S');
      dialog::add_action([](){
        dialog::editNumber(vid.skiprope, 0, 360, 15, 0, XLAT("Möbius transformations"), "");
        });
      }
    
    if(pmodel == mdHemisphere && euclid) {
      dialog::addSelItem(XLAT("parameter"), fts(vid.euclid_to_sphere), 'l');
      dialog::add_action([] () {
        dialog::editNumber(vid.euclid_to_sphere, 0, 10, .1, 1, XLAT("parameter"), 
          "Stereographic projection to a sphere. Choose the radius of the sphere."
          );
        dialog::scaleLog();
        });
      }
      
    if(among(pmodel, mdTwoPoint, mdSimulatedPerspective, mdTwoHybrid)) {
      dialog::addSelItem(XLAT("parameter"), fts(vid.twopoint_param), 'b');
      dialog::add_action([](){
        dialog::editNumber(vid.twopoint_param, 1e-3, 10, .1, 1, XLAT("parameter"), 
          "This model maps the world so that the distances from two points "
          "are kept. This parameter gives the distance from the two points to "
          "the center."
          );
        dialog::scaleLog();
        });
      }
    
    if(pmodel == mdSpiral && !euclid) {
      dialog::addSelItem(XLAT("spiral angle"), fts(spiral_angle) + "°", 'x');
      dialog::add_action([](){
        dialog::editNumber(spiral_angle, 0, 360, 15, 0, XLAT("spiral angle"), 
          XLAT("set to 90° for the ring projection")
          );
        });

      ld& which =
        sphere ? sphere_spiral_multiplier :
        ring_not_spiral ? right_spiral_multiplier :
        any_spiral_multiplier;
                
      dialog::addSelItem(XLAT("spiral multiplier"), fts(which) + "°", 'M');
      dialog::add_action([&which](){
        dialog::editNumber(which, 0, 10, -.1, 1, XLAT("spiral multiplier"), 
          XLAT(
            "This parameter has a bit different scale depending on the settings:\n"
            "(1) in spherical geometry (with spiral angle=90°, 1 produces a stereographic projection)\n"
            "(2) in hyperbolic geometry, with spiral angle being +90° or -90°\n"
            "(3) in hyperbolic geometry, with other spiral angles (1 makes the bands fit exactly)"
            )
          );
        });

      dialog::addSelItem(XLAT("spiral cone"), fts(spiral_cone) + "°", 'C');
      dialog::add_action([](){
        dialog::editNumber(spiral_cone, 0, 360, -45, 360, XLAT("spiral cone"), "");
        });
      }

    if(pmodel == mdSpiral && euclid) {
      dialog::addSelItem(XLAT("spiral period: x"), fts(spiral_x), 'x');
      dialog::add_action([](){
        dialog::editNumber(spiral_x, -20, 20, 1, 10, XLAT("spiral period: x"), "");
        });
      dialog::addSelItem(XLAT("spiral period: y"), fts(spiral_y), 'y');
      dialog::add_action([](){
        dialog::editNumber(spiral_y, -20, 20, 1, 10, XLAT("spiral period: y"), "");
        });
      if(euwrap) {
        dialog::addSelItem(XLAT("match the period"), its(spiral_id), 'n');
        dialog::add_action(match_torus_period);
        }
      }

    dialog::addSelItem(XLAT("vertical stretch"), fts(vid.stretch), 's');

    dialog::addBoolItem(XLAT("use GPU to compute projections"), vid.consider_shader_projection, 'G');
    if(vid.consider_shader_projection && !shaderside_projection)
      dialog::lastItem().value = XLAT("N/A");
    if(vid.consider_shader_projection && shaderside_projection && pmodel)
      dialog::lastItem().value += XLAT(" (2D only)");
    dialog::add_action([] { vid.consider_shader_projection = !vid.consider_shader_projection; });

    menuitem_sightrange('R');
      
    dialog::addBreak(100);
    dialog::addItem(XLAT("history mode"), 'a');
#if CAP_RUG
    if(GDIM == 2) dialog::addItem(XLAT("hypersian rug mode"), 'u');
#endif
    dialog::addBack();

    dialog::display();
    mouseovers = XLAT("see http://www.roguetemple.com/z/hyper/models.php");
    
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      
      if(uni == 'z')
        editScale();
      else if(uni == 'p')
        projectionDialog();
#if CAP_RUG
      else if(uni == 'u' && DIM == 2)
        pushScreen(rug::show);
#endif
      else if(uni == 's') {
        dialog::editNumber(vid.stretch, 0, 10, .1, 1, XLAT("vertical stretch"), 
          "Vertical stretch factor."
          );
        dialog::extra_options = [] () {
          dialog::addBreak(100);
          if(sphere && pmodel == mdBandEquiarea) {
            dialog::addBoolItem("Gall-Peters", vid.stretch == 2, 'O');
            dialog::add_action([] { vid.stretch = 2; dialog::ne.s = "2"; });
            }
          if(pmodel == mdBandEquiarea) {
            // y = K * sin(phi)
            // cos(phi) * cos(phi) = 1/K
            if(sphere && vid.stretch >= 1) {
              ld phi = acos(sqrt(1/vid.stretch));
              dialog::addInfo(XLAT("The current value makes the map conformal at the latitude of %1 (%2°).", fts(phi), fts(phi / degree)));
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

    dialog::addSelItem(XLAT("projection"), current_proj_name(), 'm');
    
    dialog::addBoolItem(XLAT("animate from start to current player position"), (on), 'e');
    dialog::addBoolItem(XLAT("animate from last recenter to current view"), (on), 'E');
    dialog::addBoolItem(XLAT("animate from last recenter to precise current view"), (on), 'E'-64);
    if(on) dialog::addSelItem(XLAT("animation speed"), fts(lvspeed), 'a');
    else dialog::addBreak(100);
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
  
    if(uni == 'e' || uni == 'E' || uni == 'E'-64) {
      if(on) clear();
      else {
        if(canmove && !cheater) {
          addMessage("Enable cheat mode or GAME OVER to use this");
          return;
          }
        if(canmove && cheater) cheater++;
        if(uni == 'E') create_recenter_to_view(false);
        else if(uni == 'E'-64) create_recenter_to_view(true);
        else create_playerpath();
        }
      }
    else if(uni == 'o') 
      autoband = !autoband;
    else if(uni == 'm') 
      pushScreen(model_menu);
    else if(uni == 'a') 
      dialog::editNumber(lvspeed, -5, 5, .1, 1, XLAT("animation speed"), "");
    else if(uni == 'd') {
      dialog::editNumber(bandhalf, 5, 1000, 5, 200, XLAT("band width"), "");
      dialog::bound_low(5);
      }
    else if(uni == 's') {
      dialog::editNumber(bandsegment, 500, 32000, 500, 16000, XLAT("band segment"), "");
      dialog::bound_low(500);
      }
    else if(uni == 'p') 
      dialog::editNumber(extra_line_steps, 0, 5, 1, 1, XLAT("extend the ends"), 
        "0 = start at the game start, endat the end position; "
        "larger numbers give extra space at the ends."
        );
    else if(uni == 'g') { dospiral = !dospiral; }
    else if(uni == 'i') { 
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
    else if(uni == 'j') { 
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
    create_playerpath();
    createImage(dospiral);
    clear();
    pmodel = spm;
    includeHistory = ih;
#endif
    }

  #if CAP_COMMANDLINE
  int readArgs() {
    using namespace arg;
             
    if(0) ;
    else if(argis("-els")) {
      shift_arg_formula(conformal::extra_line_steps);
      }
    else if(argis("-stretch")) {
      PHASEFROM(2); shift_arg_formula(vid.stretch);
      }
    else if(argis("-PM")) { 
      PHASEFROM(2); shift(); pmodel = eModel(argi());
      if(pmodel == mdFormula) {
        shift(); basic_model = eModel(argi());
        shift(); formula = args();
        }
      }
    else if(argis("-ballangle")) { 
      PHASEFROM(2); 
      shift_arg_formula(vid.ballangle);
      }
    else if(argis("-topz")) { 
      PHASEFROM(2); 
      shift_arg_formula(conformal::top_z);
      }
    else if(argis("-twopoint")) { 
      PHASEFROM(2); 
      shift_arg_formula(vid.twopoint_param);
      }
    else if(argis("-hp")) { 
      PHASEFROM(2); 
      shift_arg_formula(conformal::halfplane_scale);
      }
    else if(argis("-mori")) { 
      PHASEFROM(2); 
      shift_arg_formula(conformal::model_orientation);
      }
    else if(argis("-mori2")) { 
      PHASEFROM(2); 
      shift_arg_formula(conformal::model_orientation);
      shift_arg_formula(conformal::model_orientation_yz);
      }
    else if(argis("-crot")) { 
      PHASEFROM(2); 
      shift_arg_formula(conformal::rotation);
      if(DIM == 3) shift_arg_formula(conformal::rotation_xz);
      if(DIM == 3) shift_arg_formula(conformal::rotation_xy2);
      }
    else if(argis("-playerpath")) {
      conformal::create_playerpath();
      }
    else if(argis("-clip")) { 
      PHASEFROM(2); 
      shift_arg_formula(conformal::clip_min);
      shift_arg_formula(conformal::clip_max);
      }
    else if(argis("-mtrans")) { 
      PHASEFROM(2); 
      shift_arg_formula(conformal::model_transition);
      }
    else if(argis("-sang")) { 
      PHASEFROM(2); 
      shift_arg_formula(conformal::spiral_angle);
      if(sphere)
        shift_arg_formula(conformal::sphere_spiral_multiplier);
      else if(conformal::spiral_angle == 90)
        shift_arg_formula(conformal::right_spiral_multiplier);
      }
    else if(argis("-ssm")) { 
      PHASEFROM(2);
      shift_arg_formula(conformal::any_spiral_multiplier);
      }
    else if(argis("-scone")) { 
      PHASEFROM(2); 
      shift_arg_formula(conformal::spiral_cone);
      }
    else if(argis("-sxy")) { 
      PHASEFROM(2); 
      shift_arg_formula(conformal::spiral_x);
      shift_arg_formula(conformal::spiral_y);
      }
    else if(argis("-mob")) { 
      PHASEFROM(2); 
      shift_arg_formula(vid.skiprope);
      }
    else if(argis("-zoom")) { 
      PHASEFROM(2); shift_arg_formula(vid.scale);
      }
    else if(argis("-alpha")) { 
      PHASEFROM(2); shift_arg_formula(vid.alpha);
      }
    else if(argis("-d:model")) 
      launch_dialog(model_menu);
    else if(argis("-d:formula")) {
      launch_dialog();
      edit_formula();
      }
    else if(argis("-d:match")) {
      launch_dialog(match_torus_period);
      edit_formula();
      }
    else return 1;
    return 0;
    }

  auto hookArg = addHook(hooks_args, 100, readArgs);
  #endif  

  auto hooks = addHook(clearmemory, 0, [] () {
    conformal::renderAutoband();
    conformal::on = false;
    conformal::killhistory.clear();
    conformal::findhistory.clear();
    conformal::movehistory.clear();
    conformal::path_for_lineanimation.clear();
    conformal::includeHistory = false;
    });

  }

}
