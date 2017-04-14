// Hyperbolic Rogue -- the conformal/history mode
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details
#include <complex>

namespace polygonal {

  typedef complex<ld> cld;

  int SI = 4;
  ld  STAR = 0;
  
  int deg = 20;

  #define MSI 120

  ld matrix[MSI][MSI];
  ld ans[MSI];
  
  cld coef[MSI];
  int maxcoef, coefid;
  
  void solve() {
    if(SI < 3) SI = 3;
    for(int i=0; i<MSI; i++) ans[i] = cos(M_PI / SI);
    for(int i=0; i<MSI; i++)
      for(int j=0; j<MSI; j++) {
        double i0 = (i+0.) / (MSI-1);
        // i0 *= i0;
        // i0 = 1 - i0;
        i0 *= M_PI;
        matrix[i][j] = 
          cos(i0 * (j + 1./SI)) * (STAR > 0 ? (1+STAR) : 1)
        - sin(i0 * (j + 1./SI)) * (STAR > 0 ? STAR : STAR/(1+STAR));
        }
    
    for(int i=0; i<MSI; i++) {
      ld dby = matrix[i][i];
      for(int k=0; k<MSI; k++) matrix[i][k] /= dby;
      ans[i] /= dby; 
      for(int j=i+1; j<MSI; j++) {
        ld sub = matrix[j][i];
        ans[j] -= ans[i] * sub;
        for(int k=0; k<MSI; k++)
           matrix[j][k] -= sub * matrix[i][k];
        }
      }
    for(int i=MSI-1; i>=0; i--) {
      for(int j=0; j<i; j++) {
        ld sub = matrix[j][i];
        ans[j] -= ans[i] * sub;
        for(int k=0; k<MSI; k++)
           matrix[j][k] -= sub * matrix[i][k];
        }
      }
    }
  
  pair<ld, ld> compute(ld x, ld y, int prec) {
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
    queuereset(mdDisk, PPR_CIRCLE);

    for(int r=0; r<=2000; r++) {
      cld z = exp(cld(0, 2*M_PI * r / 2000.0));
      pair<ld,ld> z2 = compute(real(z), imag(z), deg);
      hyperpoint h;
      h[0] = z2.first * vid.radius;
      h[1] = z2.second * vid.radius;
      h[2] = vid.scrdist;
      curvepoint(h);
      }
    
    queuecurve(color, 0, PPR_CIRCLE);
    queuereset(pmodel, PPR_CIRCLE);
    }

  }

#ifndef MOBILE
namespace spiral {

  typedef long double ld;
  typedef complex<long double> cld;

  int shiftx, shifty, velx, vely;

  vector<pair<short, short> > quickmap;

  int CX, CY, SX, SY, Yshift;
  
  vector<SDL_Surface*> band;
  SDL_Surface *out;
  
  bool displayhelp = true;
  
  int& bandpixel(int x, int y) {
    int i = 0;
    while(i < size(band) && x >= band[i]->w)
      x -= band[i]->w, i++;
    return qpixel(band[i], x, y);
    }
  
  void precompute() {
  
    CX = 0;
    for(int i=0; i<size(band); i++) CX += band[i]->w;
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

    bool saveGL = vid.usingGL;
    if(saveGL) { vid.usingGL = false; setvideomode(); }

    band = _band;
    out = s;
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

      SDL_LockSurface(s);
      draw();
      if(dosave) { dosave = false; IMAGESAVE(s, buf); }
      SDL_UnlockSurface(s);
      if(displayhelp) {
        displaystr(SX/2, vid.fsize*2, 0, vid.fsize, "arrows = navigate, ESC = return, h = hide help", 0xFFFFFF, 8);
        displaystr(SX/2, SY - vid.fsize*2, 0, vid.fsize, XLAT("s = save to " IMAGEEXT, buf), 0xFFFFFF, 8);
        }
      SDL_UpdateRect(s, 0, 0, 0, 0);  
      shiftx += velx; shifty += vely;

      SDL_Event event;
      while(SDL_PollEvent(&event)) switch (event.type) {
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
    if(saveGL) { vid.usingGL = true; setvideomode(); }
    }

  }
#endif

bool isbad(ld z) { return !isfinite(z) || fabs(z) > 1e6; }

namespace conformal {

  int lastprogress;

  void progress(string str) {
#ifndef MOBILE
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
  int rotation = 0;
  bool autoband = false;
  bool autobandhistory = false;
  bool dospiral = true;

  void clear() {
    on = false;
    int N = size(v);
    for(int i=0; i<N; i++) delete v[i];
    v.resize(0);
    }
  
  void create() {
    if(celldist(cwt.c) <= 7) {
      addMessage("Must go a distance from the starting point");
      return;
      }
    
    on = true;
    cell *c = cwt.c;
    
    for(int q=0; q<5; q++) {
      for(int i=0; i<c->type; i++)
        if(celldist(c->mov[i]) > celldist(c)) {
          c = c->mov[i];
          break;
          }
      }

    while(true) {
      shmup::monster *m = new shmup::monster;
      m->at = Id;
      m->base = c;
      v.push_back(m);
      if(c == currentmap->gamestart()) break;
      for(int i=0; i<c->type; i++)
        if(celldist(c->mov[i]) < celldist(c)) {
          c = c->mov[i];
          break;
          }
      }
  
    reverse(v.begin(), v.end());
  
    int Q = size(v)-1;
  
    for(int i=0; i<1000; i++) {
      progress(XLAT("Preparing the line (%1/1000)...", its(i+1)));

      /*for(int j=1; j<Q; j++) {
        hyperpoint cur = v[j]->at * C0;
        printf("%4d/%3d. %p [%3d] %Lf %Lf %Lf\n", i, j, v[j]->base, celldist(v[j]->base), cur[0], cur[1], cur[2]);
        } */

      for(int j=1; j<Q; j++) if((j^i)&1) {
      
        virtualRebase(v[j], false);
        
        hyperpoint prev = shmup::calc_relative_matrix(v[j-1]->base, v[j]->base->master) *
          v[j-1]->at * C0;

        hyperpoint next = shmup::calc_relative_matrix(v[j+1]->base, v[j]->base->master) * 
          v[j+1]->at * C0;
        
        hyperpoint hmid = mid(prev, next);
      
        v[j]->at = rgpushxto0(hmid);

        v[j]->at = v[j]->at * rspintox(inverse(v[j]->at) * next);
        fixmatrix(v[j]->at);
        }
      }
    llv = ticks;
    phase = 0;
    }
  
  void apply() {
    int t = ticks;
    phase += (t-llv) * lvspeed / 400.;
    llv = t;
    
    int siz = size(v);
    
    while(phase < 1) phase += siz - 2;
    while(phase >= siz-1) phase -= siz - 2;
    
    int ph = int(phase);
    if(ph<1 || ph >= siz-1) return;
    
    viewctr.h = v[ph]->base->master;
    viewctr.spin = 0;
  
    View = inverse(v[ph]->at);
  
    int j = ph;
    
    hyperpoint now = v[j]->at * C0;

    hyperpoint next = shmup::calc_relative_matrix(v[j+1]->base, v[j]->base->master) * 
      v[j+1]->at * C0;
  
    View = spin(M_PI/2 * rotation) * xpush(-(phase-ph) * hdist(now, next)) * View;
    playermoved = false;
    }
  
  int measureLength() {
    int rad = vid.radius;
    vid.radius = bandhalf;
    
    int tpixels = 0;
    int siz = size(v);

    for(int j=1; j<siz-1; j++) {
      hyperpoint next = 
        inverse(v[j]->at) *
        shmup::calc_relative_matrix(v[j+1]->base, v[j]->base->master) * 
        v[j+1]->at * C0;
        
      hyperpoint nextscr;
      applymodel(next, nextscr);
      tpixels += nextscr[0] * vid.radius;
      }
  
    vid.radius = rad;
    return tpixels;
    }
  
  void restore();
  void restoreBack();

#ifndef MOBILE
  void createImage(bool dospiral) {
    int segid = 1;
    inHighQual = true;
    if(includeHistory) restore();
  
    int bandfull = 2*bandhalf;
    int len = measureLength();
    
    time_t timer;
    timer = time(NULL);
    char timebuf[128]; 
    strftime(timebuf, 128, "%y%m%d-%H%M%S", localtime(&timer));

    rotation = 0;

    SDL_Surface *sav = s;

    SDL_Surface *bbuf = SDL_CreateRGBSurface(SDL_SWSURFACE,bandfull,bandfull,32,0,0,0,0);
    s = bbuf;
    int ssr = sightrange; sightrange = 10; int sch = cheater; cheater = 0;
    videopar vid2 = vid; vid.xres = vid.yres = bandfull; vid.scale = 1;
    calcparam();
    vid.radius = bandhalf;
    
    int xpos = 0;
    
    vector<SDL_Surface*> bands;
    
    SDL_Surface *band = SDL_CreateRGBSurface(SDL_SWSURFACE, min(len, bandsegment), bandfull,32,0,0,0,0);
    
    if(!band) {
      addMessage("Could not create an image of that size.");
      }
    else {

      int siz = size(v);
      for(int j=1; j<siz-1; j++) {
        SDL_Surface *buffer = s;
        emtype cm = cmode;
        s = sav; 
        cmode = emProgress;

        char buf[128];
        sprintf(buf, "#%03d", segid);

        progress(s0 + buf + " ("+its(j)+"/"+its(siz-2)+")");
        calcparam();
        vid.radius = bandhalf;

        cmode = cm;
        s = buffer;
        viewctr.h = v[j]->base->master;
        viewctr.spin = 0; 
        View = inverse(v[j]->at);
    
        SDL_FillRect(s, NULL, 0);
        bool ugl = vid.usingGL;
        vid.usingGL = false;
        drawfullmap();
        vid.usingGL = ugl;
    
        hyperpoint next = 
          inverse(v[j]->at) *
          shmup::calc_relative_matrix(v[j+1]->base, v[j]->base->master) * 
          v[j+1]->at * C0;
          
        int x, y, shift;
        getcoord0(next, x, y, shift);
        
        int bwidth = x-bandhalf;
        
        drawsegment:
        
        for(int cy=0; cy<bandfull; cy++) for(int cx=0; cx<bwidth; cx++)
          qpixel(band, xpos+cx, cy) = qpixel(s, bandhalf+cx, cy);
    
        if(xpos+bwidth > bandsegment) {
          char buf[128];
          sprintf(buf, "bandmodel-%s-%03d" IMAGEEXT, timebuf, segid++);

          IMAGESAVE(band, buf);

          if(dospiral) 
            bands.push_back(band);
          else 
            SDL_FreeSurface(band);

          len -= bandsegment; xpos -= bandsegment;
          band = SDL_CreateRGBSurface(SDL_SWSURFACE, min(len, bandsegment), bandfull,32,0,0,0,0);
          goto drawsegment;
          }  
        xpos += bwidth;      
        }  
      }

    char buf[128];
    sprintf(buf, "bandmodel-%s-%03d" IMAGEEXT, timebuf, segid++);
    IMAGESAVE(band, buf);
    addMessage(XLAT("Saved the band image as: ") + buf);

    if(dospiral) 
      bands.push_back(band);
    else 
      SDL_FreeSurface(band);

    SDL_FreeSurface(sav);
    s = sav; vid = vid2; sightrange = ssr; cheater = sch;
    if(includeHistory) restoreBack();
    
    if(dospiral) {
      spiral::loop(bands);
      for(int i=0; i<size(bands); i++) SDL_FreeSurface(bands[i]);
      }

    inHighQual = false;
    }
#endif

  const char* directions[MODELCOUNT][4] = {
     { "right", "up", "left", "down" },
     { "counterclockwise", "zoom out", "clockwise", "zoom in" },
     { "left to right", "spin down", "right to left", "spin up" },
     { "right", "up", "left", "down" },
     { "right", "up", "left", "down" },
     { "right", "up", "left", "down" },
     { "right", "up", "left", "down" },
     { "right", "up", "left", "down" },
     { "right", "up", "left", "down" }
     };
     
  const char *modelnames[MODELCOUNT] = {
    "disk", "half-plane", "band", "polygonal", "polynomial",
    "azimuthal equidistant", "azimuthal equi-area", 
    "ball model", "hyperboloid"
    };
  
  void show() {
    dialog::init(XLAT("conformal/history mode"));

    dialog::addBoolItem(XLAT("include history"), (includeHistory), 'i');
    
    bool notconformal = (pmodel >= 5 && pmodel <= 6) || abs(vid.alpha-1) > 1e-3;

    dialog::addSelItem(notconformal ? XLAT("model used (not conformal!)") : XLAT("model used"), XLAT(modelnames[pmodel]), 'm');
    dialog::addSelItem(XLAT("rotation"), directions[pmodel][rotation&3], 'r');

    if(pmodel == 4) {
      dialog::addSelItem(XLAT("coefficient"), 
        fts4(real(polygonal::coef[polygonal::coefid])), 'x');
      dialog::addSelItem(XLAT("coefficient (imaginary)"), 
        fts4(imag(polygonal::coef[polygonal::coefid])), 'y');
      dialog::addSelItem(XLAT("which coefficient"), its(polygonal::coefid), 'n');
      }

    if(pmodel == 3) {
      dialog::addSelItem(XLAT("polygon sides"), its(polygonal::SI), 'x');
      dialog::addSelItem(XLAT("star factor"), fts(polygonal::STAR), 'y');
      dialog::addSelItem(XLAT("degree of the approximation"), its(polygonal::deg), 'n');
      }
    
    dialog::addBoolItem(XLAT("prepare the line animation"), (on), 'e');
    if(on) dialog::addSelItem(XLAT("animation speed"), fts(lvspeed), 'a');
    
#ifndef MOBILE
    dialog::addBoolItem(XLAT("render bands automatically"), (autoband), 'o');
    if(autoband)
      dialog::addBoolItem(XLAT("include history when auto-rendering"), (autobandhistory), 'j');
    
    bool renderable = on && pmodel == 2;
    if(renderable || autoband) {
      dialog::addSelItem(XLAT("band width"), "2*"+its(bandhalf), 'd');
      dialog::addSelItem(XLAT("length of a segment"), its(bandsegment), 's');
      dialog::addBoolItem(XLAT("spiral on rendering"), (dospiral), 'g');
      if(renderable)
        dialog::addItem(XLAT("render now (length: %1)", its(measureLength())), 'f');
      }
#endif
      
    dialog::addItem(XLAT("exit this menu"), 'q');
    dialog::display();
    mouseovers = XLAT("see http://www.roguetemple.com/z/hyper/conformal.php");
    }

  int ib = 0;
  ld compbuf;
  void applyIB() {
    using namespace polygonal;
    cld& tgt = coef[coefid];
    if(ib == 1) tgt = cld(compbuf, imag(tgt));
    if(ib == 2) tgt = cld(real(tgt), compbuf);
    }
    
  void handleKey(int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    ib = 0;
  
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
    else if(uni == 'm' || uni == 'M') {

      switchagain: {
        pmodel = eModel((pmodel + (shiftmul > 0 ? 1 : -1) + MODELCOUNT) % MODELCOUNT);
        if(sphere) 
          if(pmodel == mdHalfplane || pmodel == mdBand || pmodel == mdEquidistant || pmodel == mdEquiarea)
            goto switchagain;
        }
      if(pmodel == mdPolygonal) polygonal::solve();
      /* if(pmodel && vid.usingGL) {
        addMessage(XLAT("openGL mode disabled"));
        vid.usingGL = false;
        setvideomode();
        } */
      }
    else if(sym == 'x' && pmodel == mdPolygonal) {
      dialog::editNumber(polygonal::SI, 3, 10, 1, 4, XLAT("polygon sides"), "");
      dialog::sidedialog = true;
      }
    else if(sym == 'y' && pmodel == mdPolygonal) {
      dialog::editNumber(polygonal::STAR, -1, 1, .1, 0, XLAT("star factor"), "");
      dialog::sidedialog = true;
      }
    else if(sym == 'n' && pmodel == mdPolygonal) {
      dialog::editNumber(polygonal::deg, 2, MSI-1, 1, 2, XLAT("degree of the approximation"), "");
      dialog::sidedialog = true;
      }
    else if(sym == 'x' && pmodel == mdPolynomial)  {
      polygonal::maxcoef = max(polygonal::maxcoef, polygonal::coefid);
      int ci = polygonal::coefid + 1;
      compbuf = real(polygonal::coef[polygonal::coefid]);
      dialog::editNumber(compbuf, -10, 10, .01/ci/ci, 0, XLAT("coefficient"), "");
      ib = 1;
      }
    else if(sym == 'y' && pmodel == mdPolynomial) {
      polygonal::maxcoef = max(polygonal::maxcoef, polygonal::coefid);
      int ci = polygonal::coefid + 1;
      compbuf = imag(polygonal::coef[polygonal::coefid]);
      dialog::editNumber(compbuf, -10, 10, .01/ci/ci, 0, XLAT("coefficient (imaginary)"), "");
      ib = 2;
      }
    else if(sym == 'n' && pmodel == mdPolynomial)
      dialog::editNumber(polygonal::coefid, 0, MSI-1, 1, 0, XLAT("which coefficient"), "");
    else if(sym == 'r') rotation += (shiftmul > 0 ? 1:3);
    else if(sym == 'a') 
      dialog::editNumber(lvspeed, -5, 5, .1, 1, XLAT("animation speed"), "");
    else if(sym == 'd') 
      dialog::editNumber(bandhalf, 5, 1000, 5, 200, XLAT("band width"), "");
    else if(sym == 's') 
      dialog::editNumber(bandsegment, 500, 32000, 500, 16000, XLAT("band segment"), "");
    else if(sym == 'g') { dospiral = !dospiral; }
#ifndef MOBILE
    else if(uni == 'f' && pmodel == mdBand && on) createImage(dospiral);
#endif
    else if(sym == 'q' || sym == SDLK_ESCAPE || sym == '0') { cmode = emNormal; }
    else if(sym == 'i') { 
      if(canmove && !cheater) {
        addMessage("Enable cheat mode or GAME OVER to use this");
        return;
        }
      if(canmove && cheater) cheater++;
      includeHistory = !includeHistory; 
      }
    else if(sym == 'j') { 
      autobandhistory = !autobandhistory; 
      }
    }
  
  void restore() {
    sval++;
    for(int i=0; i<size(movehistory); i++)
      movehistory[i]->aitmp = sval;
    sval++;
    int sk = size(killhistory);
    for(int i=0; i<sk; i++) {
      eMonster m = killhistory[i].second;
      killhistory[i].second = killhistory[i].first->monst;
      killhistory[i].first->monst = m;
      killhistory[i].first->aitmp = sval;
      }
    int si = size(findhistory);
    for(int i=0; i<si; i++) {
      eItem m = findhistory[i].second;
      findhistory[i].second = findhistory[i].first->item;
      findhistory[i].first->item = m;
      findhistory[i].first->aitmp = sval;
      }
    }

  void restoreBack() {
    int sk = size(killhistory);
    for(int i=sk-1; i>=0; i--) {
      eMonster m = killhistory[i].second;
      killhistory[i].second = killhistory[i].first->monst;
      killhistory[i].first->monst = m;
      }
    int si = size(findhistory);
    for(int i=si-1; i>=0; i--) {
      eItem m = findhistory[i].second;
      findhistory[i].second = findhistory[i].first->item;
      findhistory[i].first->item = m;
      }
    }
  
  void renderAutoband() {
#ifndef MOBILE
    if(celldist(cwt.c) <= 7) return;
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
  }
