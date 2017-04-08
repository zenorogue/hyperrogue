/* Missing.

#ifndef MOBILE
  dialog::addItemHelp(16, XLAT("use Shift to decrease and Ctrl to fine tune "));
  dialog::addItemHelp(17, XLAT("(e.g. Shift+Ctrl+Z)"));
#endif

  if(xuni == 'i') {
    }
  

*/

namespace dialog {

  namespace zoom {
    int zoomf = 1, shiftx, shifty;
    bool zoomoff = false;
    };

#ifdef MENU_SCALING
#ifndef MOBILE
  void handleZooming(SDL_Event &ev) {
    using namespace zoom;
    if(zoomoff || (cmode != emOverview && cmode != emTactic)) { 
      zoomf = 1; shiftx = shifty = 0; zoomoff = false; return; 
      }
    if(ev.type == SDL_MOUSEBUTTONDOWN) {
      zoomf = 3;
      }
    if(zoomf == 3) { 
      shiftx = -2*mousex;
      shifty = -2*mousey;
      }
    if(ev.type == SDL_MOUSEBUTTONUP && zoomf > 1) {
      zoomoff = true;
      }
    }
#endif
#else
  inline void handleZooming(SDL_Event &ev) {}
#endif

  bool displayzoom(int x, int y, int b, int size, const string &s, int color, int align) {
    using namespace zoom;
    return displayfr(x * zoomf + shiftx, y * zoomf + shifty, b, size * zoomf, s, color, align);
    }
  
  vector<item> items;
  
  item& lastItem() { return items[items.size() - 1]; }
  
  void init() { items.clear(); }
  
  string keyname(int k) {
    if(k == 0) return "";
    if(k == SDLK_ESCAPE) return "Esc";
    if(k == SDLK_F5) return "F5";
    if(k == SDLK_F10) return "F10";
    if(k == SDLK_HOME) return "Home";
    if(k == SDLK_BACKSPACE) return "Backspace";
    if(k == SDLK_RETURN) return "Enter";
    if(k == 32) return "space";
    if(k >= 1 && k <= 26) { string s = "Ctrl+"; s += (k+64); return s; }
    if(k < 128) { string s; s += k; return s; }
    return "?";
    }

  void addSlider(double d1, double d2, double d3, int key) {
    item it;
    it.type = diSlider;
    it.color = 0xC0C0C0;
    it.scale = 100;
    it.key = key;
    it.param = (d2-d1) / (d3-d1);
    items.push_back(it);
    }
  
  void addSelItem(string body, string value, int key) {
    item it;
    it.type = diItem;
    it.body = body;
    it.value = value;
    it.keycaption = keyname(key);
    it.key = key;
    it.color = 0xC0C0C0;
    it.colork = 0x808080;
    it.colorv = 0x80A040;
    it.colors = 0xFFD500;
    it.colorc = 0xFFD500;
    it.colors = 0xFF8000;
    if(value == ONOFF(true)) it.colorv = 0x40FF40;
    if(value == ONOFF(false)) it.colorv = 0xC04040;
    it.scale = 100;
    items.push_back(it);
    }

  void addBoolItem(string body, bool value, int key) {
    addSelItem(body, ONOFF(value), key);
    }

  void addColorItem(string body, int value, int key) {
    item it;
    it.type = diItem;
    it.body = body;
    it.value = COLORBAR;
    it.keycaption = keyname(key);
    it.key = key;
    it.color = it.colorv = value >> 8;
    it.colors = it.color ^ 0x404040;
    it.colorc = it.color ^ 0x808080;
    it.scale = 100;
    items.push_back(it);
    }

  void addHelp(string body) {
    item it;
    it.type = diHelp;
    it.body = body;
    it.scale = 100;

    if(size(body) >= 500) it.scale = 70;

    items.push_back(it);
    }

  void addInfo(string body, int color) {
    item it;
    it.type = diInfo;
    it.body = body;
    it.color = color;
    it.scale = 100;
    items.push_back(it);
    }

  void addItem(string body, int key) {
    item it;
    it.type = diItem;
    it.body = body;
    it.keycaption = keyname(key);
    it.key = key;
    it.color = 0xC0C0C0;
    it.colork = 0x808080;
    it.colors = 0xFFD500;
    it.colorc = 0xFF8000;
    it.scale = 100;
    items.push_back(it);
    }

  void addBreak(int val) {
    item it;
    it.type = diBreak;
    it.scale = val;
    items.push_back(it);
    }
  
  void addTitle(string body, int color, int scale) {
    item it;
    it.type = diTitle;
    it.body = body;
    it.color = color;
    it.scale = scale;
    items.push_back(it);
    }

  void init(string title, int color, int scale, int brk) { 
    init();
    addTitle(title, color, scale);
    addBreak(brk);
    }

  int dcenter, dwidth;

  bool sidedialog;

  int displayLong(string str, int siz, int y, bool measure) {

    int last = 0;
    int lastspace = 0;
    
    int xs, xo;
    if(sidescreen)
      xs = dwidth - vid.fsize*2, xo = vid.yres + vid.fsize;
    else
      xs = vid.xres * 618/1000, xo = vid.xres * 186/1000;
    
    for(int i=0; i<=size(str); i++) {
      int ls = 0;
      int prev = last;
      if(str[i] == ' ') lastspace = i;
      if(textwidth(siz, str.substr(last, i-last)) > xs) {
        if(lastspace == last) ls = i-1, last = i-1;
        else ls = lastspace, last = ls+1;
        }
      if(str[i] == 10 || i == size(str)) ls = i, last = i+1;
      if(ls) {
        if(!measure) displayfr(xo, y, 2, siz, str.substr(prev, ls-prev), 0xC0C0C0, 0);
        if(ls == prev) y += siz/2;
        else y += siz;
        lastspace = last;
        }
      
      }
    
    y += siz/2;
    return y;
    }

  int tothei, dialogwidth, dfsize, dfspace, leftwidth, rightwidth, innerwidth, itemx, keyx, valuex;
  
  string highlight_text;
  
  void measure() {
    tothei = 0;
    dialogwidth = 0;
    innerwidth = 0;
    int N = items.size();
    for(int i=0; i<N; i++) {
      if(items[i].type == diHelp)
        tothei += displayLong(items[i].body, dfsize * items[i].scale / 100, 0, true);
      else {
        tothei += dfspace * items[i].scale / 100;
        if(items[i].type == diItem) 
          innerwidth = max(innerwidth, textwidth(dfsize * items[i].scale / 100, items[i].body));
        if(items[i].type == diTitle || items[i].type == diInfo)
          dialogwidth = max(dialogwidth, textwidth(dfsize * items[i].scale / 100, items[i].body) * 10/9);
        }
      }
    
    leftwidth = ISMOBILE ? 0 : textwidth(dfsize, "MMMMM") + dfsize/2;
    rightwidth = textwidth(dfsize, "MMMMMMMM") + dfsize/2;
    
    int fwidth = innerwidth + leftwidth + rightwidth;
    dialogwidth = max(dialogwidth, fwidth);
    itemx  = dcenter - fwidth / 2 + leftwidth;
    keyx   = dcenter - fwidth / 2 + leftwidth - dfsize/2;
    valuex = dcenter - fwidth / 2 + leftwidth + innerwidth + dfsize/2;
    }

  void display() {
    int N = items.size();
    dfsize = vid.fsize;
    #ifdef MOBILE
    dfsize *= 3;
    #endif
    #ifdef PANDORA
    dfsize *= 3;
    #endif
    dfspace = dfsize * 5/4;
    
    dcenter = vid.xres/2;
    dwidth = vid.xres;
    measure();
    
    if(sidescreen) {
      dwidth = vid.xres - vid.yres;
      dcenter = vid.xres - dwidth / 2;
      }
    
    while(tothei > vid.yres - 5 * vid.fsize) {
      int adfsize = int(dfsize * sqrt((vid.yres - 5. * vid.fsize) / tothei));
      if(adfsize < dfsize-1) dfsize = adfsize + 1;
      else dfsize--;
      dfspace = dfsize * 5/4;
      measure();
      }
    while(dialogwidth > dwidth) {
      int adfsize = int(dfsize * sqrt(vid.xres * 1. / dialogwidth));
      if(adfsize < dfsize-1) dfsize = adfsize + 1;
      else dfsize--; // keep dfspace
      measure();
      }
    
    tothei = (vid.yres - tothei) / 2;
    
    for(int i=0; i<N; i++) {
      item& I = items[i];

      if(I.type == diHelp) {
        tothei = displayLong(items[i].body, dfsize * items[i].scale / 100, tothei, false);
        continue;
        }
        
      int top = tothei;
      tothei += dfspace * I.scale / 100;
      int mid = (top + tothei) / 2;
      if(I.type == diTitle || I.type == diInfo) {
        displayfr(dcenter, mid, 2, dfsize * I.scale/100, I.body, I.color, 8);
        }
      else if(I.type == diItem) {
        bool xthis = (mousey >= top && mousey < tothei);
#ifdef MOBILE
        if(xthis && mousepressed) 
          I.color = I.colorc;
#else
        if(xthis && mousemoved) {
          highlight_text = I.body;
          mousemoved = false;
          }
        if(highlight_text == I.body) {
          I.color = (xthis&&mousepressed&&actonrelease) ? I.colorc : I.colors;
          }
#endif        
        if(!mousing)
          displayfr(keyx, mid, 2, dfsize * I.scale/100, I.keycaption, I.colork, 16);
        displayfr(itemx, mid, 2, dfsize * I.scale/100, I.body, I.color, 0);
        displayfr(valuex, mid, 2, dfsize * I.scale/100, I.value, I.colorv, 0);
        if(xthis) getcstat = I.key;
        }      
      else if(I.type == diSlider) {
        bool xthis = (mousey >= top && mousey < tothei);
        int sl, sr;
        if(sidescreen)
          sl = vid.yres + vid.fsize*2, sr = vid.xres - vid.fsize*2;
        else
          sl = vid.xres/4, sr = vid.xres*3/4;
        int sw = sr-sl;
        displayfr(sl, mid, 2, dfsize * I.scale/100, "(", I.color, 16);
        displayfr(sl + double(sw * I.param), mid, 2, dfsize * I.scale/100, "#", I.color, 8);
        displayfr(sr, mid, 2, dfsize * I.scale/100, ")", I.color, 0);
        if(xthis) getcstat = I.key, inslider = true;
        }
      }
    }
  
  void handleNavigation(int &sym, int &uni) {
#ifndef MOBILE
    if(uni == '\n' || uni == '\r' || sym == SDLK_KP5)
      for(int i=0; i<size(items); i++) 
        if(items[i].type == diItem)
          if(items[i].body == highlight_text) {
            uni = sym = items[i].key;
            return;
            }
    if(sym == SDLK_PAGEDOWN || sym == SDLK_KP3) {
      for(int i=0; i<size(items); i++)
        if(items[i].type == diItem)
          highlight_text = items[i].body;
      }
    if(sym == SDLK_PAGEUP || sym == SDLK_KP9) {
      for(int i=0; i<size(items); i++) 
        if(items[i].type == diItem) {
          highlight_text = items[i].body;
          break;
          }
      }    
    if(sym == SDLK_UP || sym == SDLK_KP8) {
      string last = "";
      for(int i=0; i<size(items); i++) 
        if(items[i].type == diItem)
          last = items[i].body;
      uni = sym = 0;
      for(int i=0; i<size(items); i++)
        if(items[i].type == diItem) {
          if(items[i].body == highlight_text) {
            highlight_text = last; return;
            }
          else last = items[i].body;
          }
      highlight_text = last;
      }
    if(sym == SDLK_DOWN || sym == SDLK_KP2) {
      int state = 0;
      for(int i=0; i<size(items); i++)
        if(items[i].type == diItem) {
          if(state) { highlight_text = items[i].body; return; }
          else if(items[i].body == highlight_text) state = 1;
          }
      for(int i=0; i<size(items); i++)
        if(items[i].type == diItem) 
          highlight_text = items[i].body;
      uni = sym = 0;
      }
#endif
    }

  unsigned int colorhistory[10] = {
    0x202020FF, 0x800000FF, 0x008000FF, 0x000080FF, 
    0x404040FF, 0xC0C0C0FF, 0x804000FF, 0xC0C000FF,
    0x408040FF, 0xFFD500FF
    }, lch;
  
  unsigned int *palette;
  
  void drawColorDialog(int color) {
    int ash = 8;
    
    for(int j=0; j<10; j++) {
      int x = vid.xres / 2 + vid.fsize * 2 * (j-5);
      int y = vid.yres / 2- 5 * vid.fsize;
      
      string s0 = ""; s0 += ('0'+j);

      vid.fsize *= 2;
      displayColorButton(x, y, s0, '0'+j, 0, 0, colorhistory[j] >> ash);
      vid.fsize /= 2;
      }
    
    if(palette) {
      int q = palette[0]; 
      for(int i=0; i<q; i++) {
        int x = vid.xres / 2 + vid.fsize * (2 * i-q);
        int y = vid.yres / 2- 7 * vid.fsize;
        string s0 = ""; s0 += ('a'+i);
        vid.fsize *= 2;
        displayColorButton(x, y, s0, 'a'+i, 0, 0, palette[i+1] >> ash);
        vid.fsize /= 2;
        }
      }

    for(int i=0; i<4; i++) {
      int y = vid.yres / 2 + (2-i) * vid.fsize * 2;

      displayColorButton(vid.xres / 4, y, "(", 0, 16, 0, 0xFFFFFF);
      string rgt = ") "; rgt += "ABGR" [i];
      displayColorButton(vid.xres * 3/4, y, rgt, 0, 0, 0, 0xFFFFFF);
      displayColorButton(vid.xres /4 + vid.xres * ((color >> (8*i)) & 0xFF) / 510, y, "#", 0, 8, 0, 0xFFFFFF);
      
      if(mousey >= y - vid.fsize && mousey < y + vid.fsize)
        getcstat = 'A' + i, inslider = true;
      }
    
    displayColorButton(vid.xres/2, vid.yres/2+vid.fsize * 6, XLAT("select this color") + " : " + itsh(color), ' ', 8, 0, color >> ash);
    }
  
  // 0: nothing happened, 1: color accepted, 2: break
  int handleKeyColor(int sym, int uni, int& color) {

    if(uni >= 'A' && uni <= 'D') {
      int x = (mousex - vid.xres/4) * 510 / vid.xres;
      if(x < 0) x = 0;
      if(x > 255) x = 255;
      unsigned char* pts = (unsigned char*) &color;
      pts[uni - 'A'] = x;
      }
    else if(uni == ' ') {
      bool inHistory = false;
      for(int i=0; i<10; i++) if(colorhistory[i] == (unsigned) color)
        inHistory = true;
      if(!inHistory) { colorhistory[lch] = color; lch++; lch %= 10; }
      return 1;
      }
    else if(uni >= '0' && uni <= '9') {
      color = colorhistory[uni - '0'];
      }
    else if(palette && uni >= 'a' && uni < 'a'+(int) palette[0]) {
      color = palette[1 + uni - 'a'];
      }
    else if(uni || sym == SDLK_F10) return 2;
    return 0;
    }
  
  int *colorPointer;
  emtype lastmode;
  
  void openColorDialog(int& col, unsigned int *pal) {
    colorPointer = &col; palette = pal;
    lastmode = cmode; cmode = emColor;
    }
  
  void handleColor(int sym, int uni) {
    int ret = handleKeyColor(sym, uni, *colorPointer);
    if(ret) cmode = lastmode;
    }
  
  struct numberEditor {
    ld *editwhat;
    string s;
    ld vmin, vmax, step, dft;
    string title, help;
    ld (*scale) (ld);
    ld (*inverse_scale) (ld);
    int *intval; ld intbuf;
    bool positive;
    } ne;
  
  bool editingDetail() {
    return ne.editwhat == &geom3::highdetail || ne.editwhat == &geom3::middetail;
    }
  
  ld identity(ld x) { return x; }
  
  void scaleSinh() {
    ne.scale = ASINH;
    ne.inverse_scale = sinh;
    }
  
  void scaleLog() {
    ne.scale = log;
    ne.inverse_scale = exp;
    ne.positive = true;
    }
  
  void editNumber(ld& x, ld vmin, ld vmax, ld step, ld dft, string title, string help) {
    ne.editwhat = &x;
    ne.s = fts(x);
    ne.vmin = vmin;
    ne.vmax = vmax;
    ne.step = step;
    ne.dft = dft;
    ne.title = title;
    ne.help = help;
    lastmode = cmode; cmode = emNumber;
    ne.scale = ne.inverse_scale = identity;
    ne.intval = NULL;
    ne.positive = false;
    sidedialog = false;
    }

  void editNumber(int& x, int vmin, int vmax, int step, int dft, string title, string help) {
    editNumber(ne.intbuf, vmin, vmax, step, dft, title, help);
    ne.intbuf = x; ne.intval = &x; ne.s = its(x);
    sidedialog = true;
    }
  
  string disp(ld x) { if(ne.intval) return its((int) (x+.5)); else return fts(x); }

  void affect(char kind) {

    if(ne.intval) {
      if(kind == 's') sscanf(ne.s.c_str(), "%d", ne.intval), *ne.editwhat = *ne.intval;
      if(kind == 'v') *ne.intval = (int) (*ne.editwhat + .5), ne.s = its(*ne.intval);
      }
    else {
      if(kind == 's') {
        ld x;
        sscanf(ne.s.c_str(), LDF, &x);
        if(ne.positive && x <= 0) return;
        *ne.editwhat = x;
        }
      if(kind == 'v') ne.s = fts(*ne.editwhat);
      }

#ifndef NOAUDIO
    if(ne.intval == &musicvolume) {
      if(musicvolume < 0) 
        *ne.editwhat = musicvolume = 0, affect('v');
      else if(musicvolume > MIX_MAX_VOLUME) 
        *ne.editwhat = musicvolume = MIX_MAX_VOLUME, affect('v');
#ifdef SDLAUDIO
      else Mix_VolumeMusic(musicvolume);
#endif
#ifdef ANDROID
      settingsChanged = true;
#endif
      }

    if(ne.intval == &effvolume) {
      if(effvolume < 0) 
        *ne.editwhat = effvolume = 0, affect('v');
      else if(effvolume > MIX_MAX_VOLUME) 
        *ne.editwhat = effvolume = MIX_MAX_VOLUME, affect('v');
#ifdef ANDROID
      settingsChanged = true;
#endif
      }
#endif

    if(ne.intval == &vid.framelimit && vid.framelimit < 5) 
      *ne.editwhat = vid.framelimit = 5, affect('v');

#ifdef MOBILE
    if(ne.intval == &fontscale && fontscale < 50) 
      *ne.editwhat = fontscale = 50, affect('v');
#endif

    if(ne.intval == &sightrange && sightrange < 4) 
      *ne.editwhat = sightrange = 4, affect('v');
    
    int msr = cheater ? 15 : 7;

    if(ne.intval == &sightrange && sightrange > msr) 
      *ne.editwhat = sightrange = msr, affect('v');
    
    if(ne.intval == &conformal::bandhalf && conformal::bandhalf < 5) 
      *ne.editwhat = *ne.intval = 5, affect('v');
    
    if(ne.intval == &conformal::bandsegment && conformal::bandsegment < 500) 
      *ne.editwhat = *ne.intval = 500, affect('v');

    if(ne.intval == &polygonal::coefid && polygonal::coefid < 0) 
      *ne.editwhat = *ne.intval = 0, affect('v');
      
    if(ne.intval == &polygonal::coefid && polygonal::coefid >= MSI) 
      *ne.editwhat = *ne.intval = MSI-1, affect('v');
      
    if(ne.intval == &polygonal::deg && polygonal::deg < 0) 
      *ne.editwhat = *ne.intval = MSI-1, affect('v');
      
    if(ne.intval == &polygonal::deg && polygonal::deg >= MSI) 
      *ne.editwhat = *ne.intval = MSI-1, affect('v');
      
    if(ne.intval == &polygonal::SI) polygonal::solve();
    if(ne.editwhat == &polygonal::STAR) polygonal::solve();
    
    conformal::applyIB();
    
    if(ne.editwhat == &geom3::highdetail && geom3::highdetail > geom3::middetail)
      geom3::middetail = geom3::highdetail;
    
    if(ne.editwhat == &geom3::middetail && geom3::highdetail > geom3::middetail)
      geom3::highdetail = geom3::middetail;
    
    if(lastmode == em3D) buildpolys(), resetGL();
    }
  
  void drawNumberDialog() {
    init(ne.title);
    addInfo(ne.s);
    addSlider(ne.scale(ne.vmin), ne.scale(*ne.editwhat), ne.scale(ne.vmax), 500);
    addBreak(100);
#ifndef MOBILE
    addHelp("You can scroll with arrow keys -- Ctrl to fine-tune");
    addBreak(100);
#endif
    
    addItem("return", ' ');
    addSelItem("default value", disp(ne.dft), SDLK_HOME);

    addBreak(100);
    
    if(lastmode == em3D) ne.help = explain3D(ne.editwhat);

    if(ne.help != "") {
      addHelp(ne.help);
      bool scal = size(ne.help) > 160;
#ifndef MOBILE
#ifndef PANDORA
      scal = false;
#endif
#endif
      if(scal) lastItem().scale = 30;
      }

    if(ne.editwhat == &vid.alpha) {
      addBreak(100);
      addSelItem(sphere ? "stereographic" : "Poincaré model", "1", 'p');
      addSelItem(sphere ? "gnomonic" : "Klein model", "0", 'k');
      addItem(sphere ? "towards orthographic" : "towards Gans model", 'o');
      }
    
    if(ne.editwhat == &ne.intbuf && ne.intval == &sightrange && cheater)
      addBoolItem("overgenerate", overgenerate, 'o');

    display();
    }
  
  void handleNumber(int sym, int uni) {
    handleNavigation(sym, uni);
    if((uni >= '0' && uni <= '9') || (uni == '.' && !ne.intval) || (uni == '-' && !ne.positive)) {
      ne.s += uni;
      affect('s');
      }
    else if(uni == '\b' || uni == '\t') {
      ne.s = ne.s. substr(0, size(ne.s)-1);
      sscanf(ne.s.c_str(), LDF, ne.editwhat);
      affect('s');
      }
#ifndef MOBILE
    else if(sym == SDLK_RIGHT || sym == SDLK_KP6) {
      if(ne.intval && abs(shiftmul) < .6)
        (*ne.editwhat)++;
      else
        *ne.editwhat = ne.inverse_scale(ne.scale(*ne.editwhat) + shiftmul * ne.step);
      affect('v');
      }
    else if(sym == SDLK_LEFT || sym == SDLK_KP4) {
      if(ne.intval && abs(shiftmul) < .6)
        (*ne.editwhat)--;
      else
        *ne.editwhat = ne.inverse_scale(ne.scale(*ne.editwhat) - shiftmul * ne.step);
      affect('v');
      }
#endif
    else if(sym == SDLK_HOME) {
      *ne.editwhat = ne.dft;
      affect('v');
      }
    else if(uni == 500) {
      ld d = (mousex - vid.xres/4 + .0) / (vid.xres/2);
      *ne.editwhat = 
        ne.inverse_scale(d * (ne.scale(ne.vmax) - ne.scale(ne.vmin)) + ne.scale(ne.vmin));
      affect('v');
      }
    else if(uni == 'o' && ne.editwhat == &ne.intbuf && ne.intval == &sightrange && cheater)
      overgenerate = !overgenerate;
    else if(uni == 'p' && ne.editwhat == &vid.alpha) {
      *ne.editwhat = 1; vid.scale = 1; ne.s = "1";
      }
    else if(uni == 'k' && ne.editwhat == &vid.alpha) {
      *ne.editwhat = 0; vid.scale = 1; ne.s = "0";
      }
    else if((uni == 'i' || uni == 'I' || uni == 'o' || uni == 'O') && ne.editwhat == &vid.alpha) {
      double d = exp(shiftmul/10);
      vid.alpha *= d;
      vid.scale *= d;
      ne.s = fts(vid.alpha);
      }
    else if(uni) {
      cmode = lastmode;
      }
    }

  int nlpage = 1;
  int wheelshift = 0;
  
  int handlePage(int& nl, int& nlm, int perpage) {
    nlm = nl;
    int onl = nl;
    int ret = 0;
    if(nlpage) {
      nl = nlm = perpage; 
      if(nlpage == 2) ret = nlm;
      int w = wheelshift;
      int realw = 0;
      while(w<0 && ret) {
        ret--; w++; realw--;
        }
      while(w>0 && ret+perpage < onl) {
        ret++; w--; realw++;
        }
      wheelshift = realw;
      if(ret+nl > onl) nl = onl-ret;
      }
    return ret;
    }
  
  void displayPageButtons(int i, bool pages) {
    int i0 = vid.yres - vid.fsize;
    int xr = vid.xres / 80;
    if(pages) if(displayfrZ(xr*8, i0, 1, vid.fsize, IFM("1 - ") + XLAT("page") + " 1", nlpage == 1 ? 0xD8D8C0 : 0xC0C0C0, 8))
      getcstat = '1';
    if(pages) if(displayfrZ(xr*24, i0, 1, vid.fsize, IFM("2 - ") + XLAT("page") + " 2", nlpage == 1 ? 0xD8D8C0 : 0xC0C0C0, 8))
      getcstat = '2';
    if(pages) if(displayfrZ(xr*40, i0, 1, vid.fsize, IFM("3 - ") + XLAT("all"), nlpage == 1 ? 0xD8D8C0 : 0xC0C0C0, 8))
      getcstat = '3';
    if(i&1) if(displayfrZ(xr*56, i0, 1, vid.fsize, IFM("0 - ") + XLAT("return"), 0xC0C0C0, 8))
      getcstat = '0';
    if(i&2) if(displayfrZ(xr*72, i0, 1, vid.fsize, IFM("F1 - ") + XLAT("help"), 0xC0C0C0, 8))
      getcstat = SDLK_F1;
    }
  
  bool handlePageButtons(int uni) {
    if(uni == '1') nlpage = 1, wheelshift = 0;
    else if(uni == '2') nlpage = 2, wheelshift = 0;
    else if(uni == '3') nlpage = 0, wheelshift = 0;
    else if(uni == PSEUDOKEY_WHEELUP) wheelshift--;
    else if(uni == PSEUDOKEY_WHEELDOWN) wheelshift++;
    else return false;
    return true;
    }
  
  };
