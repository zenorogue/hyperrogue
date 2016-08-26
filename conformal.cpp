// Hyperbolic Rogue -- the conformal/history mode
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details
#include <complex>

namespace polygonal {

  typedef long double ld;
  typedef complex<long double> cld;

  int SI = 4;
  double STAR = 0;
  
  int deg = 20;

  #define MSI 120

  ld matrix[MSI][MSI];
  ld ans[MSI];
  
  cld coef[MSI];
  int maxcoef, coefid;
  
  void solve() {
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
    if(pmodel == 4) {
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
    #ifdef GL
    if(vid.usingGL) {
      qglcoords = 0;
      glcolor(color);
      int pts = 0;
      for(int r=0; r<2000; r++) {
        cld z = exp(cld(0, 2*M_PI * r / 2000.0));
        pair<ld,ld> z2 = compute(real(z), imag(z), deg);
        glcoords[pts][0] = vid.radius * z2.first;
        glcoords[pts][1] = vid.radius * z2.second;
        glcoords[pts][2] = vid.scrdist;
        pts++;
        }
      
      glVertexPointer(3, GL_FLOAT, 0, glcoords);
      glEnableClientState(GL_VERTEX_ARRAY);
      glDrawArrays(GL_LINE_LOOP, 0, pts);
      return;
      }
    #endif
    }
  

  }

#ifndef MOBILE
namespace spiral {

  typedef long double ld;
  typedef complex<long double> cld;

  int shiftx, shifty, velx, vely;

  vector<pair<short, short> > quickmap;

  int CX, CY, SX, SY, Yshift;
  
  SDL_Surface *band, *out;
  
  bool displayhelp = true;
  
  void precompute() {
  
    CX = band->w;
    CY = band->h;
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
      qpixel(out, x, y) = qpixel(band, cx, cy);
      }
    }

  void loop(SDL_Surface *_band) {

    bool saveGL = vid.usingGL;
    if(saveGL) { vid.usingGL = false; setvideomode(); }

    band = _band;
    out = s;
    precompute();
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
  double lvspeed = 1;
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
      if(c == origin.c7) break;
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
        
      int x, y, shift;
      getcoord(next, x, y, shift);
      
      tpixels += x-vid.xcenter;
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
        getcoord(next, x, y, shift);
        
        int bwidth = x-bandhalf;
        
        drawsegment:
        
        for(int cy=0; cy<bandfull; cy++) for(int cx=0; cx<bwidth; cx++)
          qpixel(band, xpos+cx, cy) = qpixel(s, bandhalf+cx, cy);
    
        if(xpos+bwidth > bandsegment) {
          char buf[128];
          sprintf(buf, "bandmodel-%s-%03d" IMAGEEXT, timebuf, segid++);

          if(dospiral) {
            swap(vid.xres, vid2.xres); swap(vid.yres, vid2.yres); s = sav;
            spiral::loop(band);
            swap(vid.xres, vid2.xres); swap(vid.yres, vid2.yres); s = bbuf;
            }

          IMAGESAVE(band, buf);
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
    SDL_FreeSurface(sav);
    s = sav; vid = vid2; sightrange = ssr; cheater = sch;
    if(includeHistory) restoreBack();
    if(dospiral) spiral::loop(band);
    addMessage(XLAT("Saved the band image as: ") + buf);
    SDL_FreeSurface(band);
    inHighQual = false;
    }
#endif

  const char* directions[5][4] = {
     { "right", "up", "left", "down" },
     { "counterclockwise", "zoom out", "clockwise", "zoom in" },
     { "left to right", "spin down", "right to left", "spin up" },
     { "right", "up", "left", "down" },
     { "right", "up", "left", "down" }
     };
     
  const char *modelnames[5] = {
    "disk", "half-plane", "band", "polygonal", "polynomial"
    };
  
  void show() {
    displayStat( 0, XLAT("conformal/history mode"), "", ' ');

    displayStat( 2, XLAT("include history"), ONOFF(includeHistory), 'i');

    displayStat( 4, XLAT("model used"), modelnames[pmodel], 'm');
    displayStat( 5, XLAT("rotation"), directions[pmodel][rotation&3], 'r');

    if(pmodel == 4) {
      displayStat( 6, XLAT("coefficient"), 
        fts4(real(polygonal::coef[polygonal::coefid]))+"+"+
        fts4(imag(polygonal::coef[polygonal::coefid]))+"i", 'x');
      displayStat( 7, XLAT("which coefficient"), its(polygonal::coefid), 'n');
      }

    if(pmodel == 3) {
      displayStat( 6, XLAT("polygon sides"), its(polygonal::SI), 'x');
      displayStat( 7, XLAT("star factor"), fts(polygonal::STAR), 'y');
      displayStat( 8, XLAT("degree of the approximation"), its(polygonal::deg), 'n');
      }
    
    displayStat(10, XLAT("prepare the line animation"), ONOFF(on), 'e');
    if(on) displayStat(11, XLAT("animation speed"), fts(lvspeed), 'a');
    
#ifndef MOBILE
    displayStat(13, XLAT("render bands automatically"), ONOFF(autoband), 'o');
    if(autoband)
      displayStat(14, XLAT("include history when auto-rendering"), ONOFF(autobandhistory), 'j');
    
    bool renderable = on && pmodel == 2;
    if(renderable || autoband) {
      displayStat(15, XLAT("band width"), its(bandhalf*2), 'd');
      displayStat(16, XLAT("length of a segment"), its(bandsegment), 's');
      displayStat(17, XLAT("spiral on rendering"), ONOFF(dospiral), 'g');
      if(renderable)
        displayStat(18, XLAT("render now (length: %1)", its(measureLength())), "", 'f');
      }
#endif
      
    displayStat(20, XLAT("exit this menu"), "", 'q');
    mouseovers = XLAT("see http://www.roguetemple.com/z/hyper/conformal.php");
    }

  void handleKey(int uni, int sym) {
  
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
    else if(uni == 'm') {
      pmodel++;
      pmodel %= 5;
      if(pmodel == 3) polygonal::solve();
      /* if(pmodel && vid.usingGL) {
        addMessage(XLAT("openGL mode disabled"));
        vid.usingGL = false;
        setvideomode();
        } */
      }
    else if(sym == 'x' && pmodel == 3) { polygonal::SI += (shiftmul > 0 ? 1:-1); polygonal::solve(); }
    else if(sym == 'y' && pmodel == 3) { polygonal::STAR += shiftmul/10; polygonal::solve(); }
    else if(sym == 'n' && pmodel == 3) { polygonal::deg += (shiftmul>0?1:-1); 
      if(polygonal::deg < 2) polygonal::deg = 2;
      if(polygonal::deg > MSI-1) polygonal::deg = MSI-1;
      }
    else if(sym == 'x' && pmodel == 4) { 
      int ci = polygonal::coefid;
      polygonal::coef[polygonal::coefid] += polygonal::cld(shiftmul/100/ci/ci, 0); 
      }
    else if(sym == 'y' && pmodel == 4) { 
      int ci = polygonal::coefid;
      polygonal::coef[polygonal::coefid] += polygonal::cld(0, shiftmul/100/ci/ci); 
      }
    else if(sym == 'n' && pmodel == 4) { polygonal::coefid += (shiftmul>0?1:-1); polygonal::maxcoef = max(polygonal::maxcoef, polygonal::coefid); }
    else if(sym == 'r') rotation += (shiftmul > 0 ? 1:3);
    else if(sym == 'a') { lvspeed += shiftmul/10; }
    else if(sym == 'd') { bandhalf += int(5 * shiftmul); if(bandhalf < 5) bandhalf = 5; }
    else if(sym == 's') { bandsegment += int(500 * shiftmul); if(bandsegment < 500) bandsegment = 500; }
    else if(sym == 'g') { dospiral = !dospiral; }
#ifndef MOBILE
    else if(uni == 'f' && pmodel == 2 && on) createImage(dospiral);
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
    int spm = pmodel;
    bool ih = includeHistory;
    includeHistory = autobandhistory;
    pmodel = 2;
    create();
    createImage(dospiral);
    clear();
    pmodel = spm;
    includeHistory = ih;
#endif
  }
  }
