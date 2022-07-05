// Hyperbolic Rogue -- the history mode
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file history.cpp
 *  \brief Implementation of the history mode, including the long band and long spiral.
 */

#include "hyper.h"
namespace hr {

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

    renderbuffer rb(vid.xres, vid.yres, false);

    if(vid.usingGL) {
      rb.make_surface();
      out = rb.srf;
      }
    else
      out = s;

    band = _band;
    precompute();
    if(CX == 0) return;
    shiftx = shifty = 0;
    velx=1; vely=1;
    bool dosave = false;
    
    while(true) {

      time_t timer;
      timer = time(NULL);
      char buf[128]; 
      strftime(buf, 128, "spiral-%y%m%d-%H%M%S" IMAGEEXT, localtime(&timer));

      SDL_LockSurface(out);
      draw();
      if(vid.usingGL) {
        calcparam();
        setGLProjection();
        current_display->next_shader_flags = GF_TEXTURE;
        dynamicval<eModel> m(pmodel, mdPixel);
        glhr::color2(0xFFFFFFFF);
        glhr::set_depthtest(false);
        glhr::current_vertices = NULL;
        current_display->set_all(0,0);
        vector<glhr::textured_vertex> rtver(4);
        for(int i=0; i<4; i++) {
          int cx[4] = {0, 1, 1, 0};
          int cy[4] = {0, 0, 1, 1};
          rtver[i].texture[0] = cx[i] * rb.x * 1. / rb.tx;
          rtver[i].texture[1] = 1 - cy[i] * rb.y * 1. / rb.ty;
          rtver[i].coords[0] = vid.xres * cx[i] - current_display->xcenter;
          rtver[i].coords[1] = vid.yres * cy[i] - current_display->ycenter;
          rtver[i].coords[2] = 0;
          rtver[i].coords[3] = 1;
          }
        glhr::prepare(rtver);
        rb.use_as_texture();
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
      if(dosave) { dosave = false; IMAGESAVE(out, buf); }
      SDL_UnlockSurface(out);
      if(displayhelp) {
        displaystr(SX/2, vid.fsize*2, 0, vid.fsize, "arrows = navigate, ESC = return, h = hide help", forecolor, 8);
        displaystr(SX/2, SY - vid.fsize*2, 0, vid.fsize, XLAT("s = save to " IMAGEEXT, buf), forecolor, 8);
        glflush();
        }
      present_screen();
      shiftx += velx; shifty += vely;

      SDL_Event event;
      while(SDL_PollEvent(&event)) switch (event.type) {

        #if !CAP_SDL2
        case SDL_VIDEORESIZE: {
          resize_screen_to(event.resize.w, event.resize.h);
          precompute();
          break;
          }
        #endif
        #if CAP_SDL2
        case SDL_WINDOWEVENT: {
          if(event.window.event == SDL_WINDOWEVENT_RESIZED)
          resize_screen_to(event.window.data1, event.window.data2);
          precompute();
          break;
          }
        #endif
        case SDL_QUIT: case SDL_MOUSEBUTTONDOWN:
          goto breakloop;

        case SDL_KEYDOWN: {
          int sym = event.key.keysym.sym;
          int uni = 0;
          numlock_on = event.key.keysym.mod & KMOD_NUM;
          if(DKEY == SDLK_RIGHT) velx++;
          if(DKEY == SDLK_LEFT) velx--;
          if(DKEY == SDLK_UP) vely++;
          if(DKEY == SDLK_DOWN) vely--;
          if(sym == SDLK_ESCAPE) goto breakloop;
          if(sym == 'h') displayhelp = !displayhelp;
          if(sym == 's') dosave = true;
          }
        }
      }
    
    breakloop:
    quickmap.clear();
    }

  }
#endif

EX namespace history {

  void handleKeyC(int sym, int uni);
  
  int lastprogress;
  
  EX void progress_screen() {
    gamescreen();
    mouseovers = "";
    }

  EX int progress_each = 250;

  EX void progress(string str) {
#if CAP_SDL
    int tick = SDL_GetTicks();
    if(tick > lastprogress + progress_each) {
      lastprogress = tick;
      msgs.clear();
      addMessage(str);
      drawscreen();
      }
#endif
    }
  
  EX bool on;
  EX vector<shmup::monster*> v;
  int llv;
  EX double phase;
  
  EX vector<pair<cell*, eMonster> > killhistory;
  EX vector<pair<cell*, eItem> > findhistory;
  EX vector<cell*> movehistory;
  
  EX bool includeHistory;
  EX ld lvspeed = 1;
  EX int bandhalf = 200;
  EX int bandsegment = 16000;
  
  EX int saved_ends;
  
  EX cell* first_center_at;
  EX transmatrix first_center_view;
  
  EX void save_end() {
    if(!allowIncreasedSight()) {
      addMessage("Enable cheat mode or GAME OVER to use this");
      return;
      }
    if(cheater) cheater++;
    switch(saved_ends) {
      case 0:
        first_center_at = centerover;
        first_center_view = View;
        saved_ends = 1;
        return;
      
      case 1: {
        shmup::monster *m = new shmup::monster;
        m->at = inverse(first_center_view);
        m->base = first_center_at;
        v.push_back(m);
        create(first_center_at, centerover, inverse(unshift(ggmatrix(centerover))));
        if(on) saved_ends = 2;
        return;
        }
      
      case 2:
        on = false;
        saved_ends = 0;
        return;
      }
    }

  EX bool autoband = false;
  EX bool autobandhistory = false;
  EX bool dospiral = true;
  
  EX ld extra_line_steps = 0;
  
  EX vector<cell*> path_for_lineanimation;

  EX void clear() {
    on = false;
    int N = isize(v);
    for(int i=0; i<N; i++) delete v[i];
    v.resize(0);
    }
  
  EX void smoothen_line() {
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
    }

  EX void create(cell *start, cell *target, transmatrix last) {
    
    if(target == start && !(quotient && isize(path_for_lineanimation) > 1)) {
      addMessage("Must go a distance from the starting point");
      return;
      }
    
    on = true;
  
    if(!quotient && !arb::in()) try {
      auto p = build_shortest_path(start, target);
      path_for_lineanimation = p;
      }
    catch(const hr_shortest_path_exception&) {
      addMessage("Could not build a path");
      return;
      }

    for(cell *c: path_for_lineanimation) {
      shmup::monster *m = new shmup::monster;
      m->at = Id;
      m->base = c;
      v.push_back(m);
      }
    
    v.back()->at = last;
  
    smoothen_line();
  
    llv = ticks;
    phase = 0;
    }

  EX void create_playerpath() {
    create(currentmap->gamestart(), cwt.at, Id);
    }
  
  EX void create_recenter_to_view(bool precise) {
    cell *c = centerover ? centerover : cwt.at;
    create(path_for_lineanimation[0], c, precise ? inverse(unshift(ggmatrix(c))) : Id);
    }
  
  EX void movetophase() {
    
    int ph = int(phase);
    int siz = isize(v);
    if(ph<0) ph = 0;
    if(ph >= siz-1) ph = siz-2;
    
    cell *old = centerover;
    
    centerover = v[ph]->base;
    
    ld angle = 0;
    if(WDIM == 3) {
      hyperpoint h = inverse(models::rotmatrix()) * View * hpxy3(1,2,3);
      angle = atan2(h[1], h[2]);
      }
    
    View = inverse(v[ph]->at);
    
    if(arb::in() && v[ph]->base->master->emeraldval) View = Mirror * View;
  
    hyperpoint now = v[ph]->at * C0;

    hyperpoint next = calc_relative_matrix(v[ph+1]->base, v[ph]->base, C0) * 
      v[ph+1]->at * C0;
  
    View = xpush(-(phase-ph) * hdist(now, next)) * View;
    if(WDIM == 2 || prod) {
      View = models::rotmatrix() * View;
      }
    else {
      if(celldistance(v[ph]->base, old) <= 2) {
        hyperpoint h1 = View * currentmap->relative_matrix(old, centerover, C0) * hpxy3(1,2,3);
        ld angle1 = atan2(h1[1], h1[2]);
        View = cspin(2, 1, angle1 - angle) * View;
        }
      View = models::rotmatrix() * View;
      }

    playermoved = false;
    centerover = v[ph]->base;
    compute_graphical_distance();
    }
  
  EX void apply() {
    int t = ticks;
    phase += (t-llv) * lvspeed / 400.;
    llv = t;
    
    int siz = isize(v);
    
    while(phase > siz-1 + extra_line_steps) phase -= (siz + 2 * extra_line_steps-1);
    while(phase < - extra_line_steps) phase += (siz + 2 * extra_line_steps-1);
    
    movetophase();
    }
  
  ld measureLength() {
    ld r = bandhalf * pconf.scale;
    
    ld tpixels = 0;
    int siz = isize(v);

    for(int j=0; j<siz-1; j++) {
      hyperpoint next = 
        inverse(v[j]->at) *
        calc_relative_matrix(v[j+1]->base, v[j]->base, C0) * 
        v[j+1]->at * C0;
        
      hyperpoint nextscr;
      applymodel(shiftless(next), nextscr);
      tpixels += nextscr[0] * r;
      
      if(j == 0 || j == siz-2)
        tpixels += nextscr[0] * r * extra_line_steps;
      }
  
    return tpixels;
    }
  
  void restore();
  void restoreBack();

#if CAP_SHOT && CAP_SDL
  string band_format_now = "bandmodel-$DATE-$ID" IMAGEEXT;
  string band_format_auto = "bandmodel-$DATE-$ID" IMAGEEXT;
#endif

#if CAP_SDL
  EX void createImage(const string& name_format, bool dospiral) {
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
      dynamicval<ld> dr(models::rotation, 0);
      dynamicval<bool> di(inHighQual, true);
      
      renderbuffer glbuf(bandfull, bandfull, vid.usingGL);
      vid.xres = vid.yres = bandfull;
      glbuf.enable(); current_display->radius = bandhalf;  
      calcparam();
      
      ld xpos = 0;
      
      int seglen = min(int(len), bandsegment);
      
      SDL_Surface *band = SDL_CreateRGBSurface(SDL_SWSURFACE, seglen, bandfull,32,0,0,0,0);
      
      auto save_band_segment = [&] {
        string fname = name_format;
        replace_str(fname, "$DATE", timebuf);
        replace_str(fname, "$ID", format("%03d", segid++));
        IMAGESAVE(band, fname.c_str());

        if(dospiral) 
          bands.push_back(band);
        else 
          SDL_FreeSurface(band);
        };
      
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
            shiftpoint last = ggmatrix(last_base) * last_relative;
            hyperpoint hscr;
            applymodel(last, hscr);
            ld bwidth = -current_display->radius * hscr[0];
            println(hlog, "bwidth = ", bwidth, "/", len, " : ", xpos, "..", xpos+bwidth);
            
            drawsegment:
            SDL_Surface *gr = glbuf.render();
  
            for(int cy=0; cy<bandfull; cy++) for(int cx=0; cx<=bwidth+3; cx++)
              qpixel(band, int(xpos+cx), cy) = qpixel(gr, int(bandhalf+cx-bwidth), cy);
            
            if(j == 1-bonus)
              xpos = bwidth * (extra_line_steps - bonus);
        
            if(xpos+bwidth > bandsegment) {
              save_band_segment();    
    
              len -= bandsegment; xpos -= bandsegment;
              seglen = min(int(len), bandsegment);
              band = SDL_CreateRGBSurface(SDL_SWSURFACE, seglen, bandfull,32,0,0,0,0);
              goto drawsegment;
              }  
            xpos += bwidth;      
            }
          
          last_base = centerover;
          last_relative = tC0(v[j]->at);
          }
        }

      save_band_segment();
      }

    rbuf.reset();

    if(includeHistory) restoreBack();
    
    if(dospiral) {
      spiral::loop(bands);
      for(int i=0; i<isize(bands); i++) SDL_FreeSurface(bands[i]);
      }
    }

  EX void open_filedialog_to_create_image(bool ds) {
    #if CAP_SHOT && CAP_SDL
    dialog::openFileDialog(band_format_now, XLAT("rendered band ($ID=segment, $DATE=date)"), ".png", [ds] () {
      createImage(band_format_now, ds);
      return true;
      });
    #endif
    }
#endif

  EX bool band_renderable_now() {
    return on && (pmodel == mdBand || pmodel == mdBandEquidistant || pmodel == mdBandEquiarea) && !euclid && !sphere;
    }
  
  EX void history_menu() {
    cmode = sm::SIDE | sm::MAYDARK;
    gamescreen();
    
    dialog::init(XLAT("history mode"));

    dialog::addBoolItem(XLAT("include history"), (includeHistory), 'i');
    
    // bool notconformal0 = (pmodel >= 5 && pmodel <= 6) && !euclid;
    // bool notconformal = notconformal0 || abs(pconf.alpha-1) > 1e-3;

    dialog::addSelItem(XLAT("projection"), current_proj_name(), 'm');
    
    dialog::addBoolItem(XLAT("animate from start to current player position"), (on), 'e');
    dialog::addBoolItem(XLAT("animate from last recenter to current view"), (on), 'E');
    dialog::addBoolItem(XLAT("animate from last recenter to precise current view"), (on), 'E'-64);
    
    if(saved_ends == 0)
      dialog::addItem(XLAT("save the animation starting point"), '1');
    else if(saved_ends == 1)
      dialog::addItem(XLAT("animate from the starting point"), '1');
    else
      dialog::addItem(XLAT("reset animation"), '1');
    dialog::add_action(save_end);
    
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
    else if(!on) ;
    else if(!hyperbolic) 
      dialog::addInfo(XLAT("more options in hyperbolic geometry"));
    else if(!among(pmodel, mdBand, mdBandEquiarea, mdBandEquidistant))
      dialog::addInfo(XLAT("more options in band projections"));
    
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
        if(!allowIncreasedSight()) {
          addMessage("Enable cheat mode or GAME OVER to use this");
          return;
          }
        if(cheater) cheater++;
        if(uni == 'E') create_recenter_to_view(false);
        else if(uni == 'E'-64) create_recenter_to_view(true);
        else create_playerpath();
        }
      }
    else if(uni == 'o') {
      autoband = !autoband;
      #if CAP_SHOT && CAP_SDL
      if(autoband)
        dialog::openFileDialog(band_format_auto, XLAT("filename format to use ($ID=segment, $DATE=date)"), ".png", [] () { return true; });
      #endif
      }
    else if(uni == 'm') 
      pushScreen(models::model_menu);
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
      if(!allowIncreasedSight()) {
        addMessage("Enable cheat mode or GAME OVER to use this");
        return;
        }
      if(cheater) cheater++;
      includeHistory = !includeHistory; 
      }
#if CAP_SDL
    else if(uni == 'f' && band_renderable_now()) 
      open_filedialog_to_create_image(dospiral);
    else if(uni == 'j') 
      autobandhistory = !autobandhistory; 
#endif
    else if(doexiton(sym, uni)) popScreen();
    }
  
  EX set<cell*> inmovehistory, inkillhistory, infindhistory;
  
  EX void restore() {
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

  EX void restoreBack() {
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
  
  EX void renderAutoband() {
#if CAP_SDL && CAP_SHOT
    if(!cwt.at || celldist(cwt.at) <= 7) return;
    if(!autoband) return;
    eModel spm = pmodel;
    bool ih = includeHistory;
    includeHistory = autobandhistory;
    pmodel = mdBand;
    create_playerpath();
    createImage(band_format_auto, dospiral);
    clear();
    pmodel = spm;
    includeHistory = ih;
#endif
    }

  auto hookArg = arg::add3("-playerpath", history::create_playerpath);

  auto hooks = addHook(hooks_clearmemory, 0, [] () {
    history::renderAutoband();
    history::on = false;
    history::killhistory.clear();
    history::findhistory.clear();
    history::movehistory.clear();
    history::path_for_lineanimation.clear();
    history::saved_ends = 0;
    history::includeHistory = false;
    }) + addHook(hooks_configfile, 0, [] {

    addsaver(autobandhistory, "include history"); // check!
    param_f(lvspeed, "lvspeed", "lineview speed");
    addsaver(extra_line_steps, "lineview extension");
      
    addsaver(bandhalf, "band width");
    addsaver(bandsegment, "band segment");
    addsaver(autoband, "automatic band");
    addsaver(autobandhistory, "automatic band history");
    addsaver(dospiral, "do spiral");      

    #if CAP_SHOT && CAP_SDL
    addsaver(band_format_auto, "band_format_auto");
    addsaver(band_format_now, "band_format_now");
    #endif
    });

  }

}
