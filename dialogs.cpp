// Hyperbolic Rogue -- dialogs
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/* Missing.

#if ISMOBILE==0
  dialog::addItemHelp(16, XLAT("use Shift to decrease and Ctrl to fine tune "));
  dialog::addItemHelp(17, XLAT("(e.g. Shift+Ctrl+Z)"));
#endif

  if(xuni == 'i') {
    }
  

*/

namespace hr {

const char* COLORBAR = "###";

namespace dialog {

  color_t dialogcolor = 0xC0C0C0;

  void addBack() {
    addItem(XLAT("go back"), SDLK_ESCAPE);
    }

  void addHelp() {
    addItem(XLAT("help"), SDLK_F1);
    }

  namespace zoom {
    int zoomf = 1, shiftx, shifty;
    bool zoomoff = false;

    void nozoom() {
      zoomf = 1; shiftx = 0; shifty = 0; zoomoff = false;
      }
    
    void initzoom() {
      zoomf = 3;
      shiftx = -2*mousex;
      if(mousex < vid.xres / 6) shiftx = 0;
      if(mousex > vid.xres * 5 / 6) shiftx = -2 * vid.xres;
      shifty = -2*mousey;
      if(mousey < vid.yres / 6) shifty = 0;
      if(mousey > vid.yres * 5 / 6) shifty = -2 * vid.yres;
      }
    
    void stopzoom() { zoomoff = true; }

    bool displayfr(int x, int y, int b, int size, const string &s, color_t color, int align) {
      return hr::displayfr(x * zoomf + shiftx, y * zoomf + shifty, b, size * zoomf, s, color, align);
      }
  
    bool displayfr_highlight(int x, int y, int b, int size, const string &s, color_t color, int align, int hicolor) {
      bool clicked = hr::displayfr(x * zoomf + shiftx, y * zoomf + shifty, b, size * zoomf, s, color, align);
      if(clicked) hr::displayfr(x * zoomf + shiftx, y * zoomf + shifty, b, size * zoomf, s, hicolor, align);
      return clicked;
      }
    };

#if CAP_MENUSCALING && CAP_SDL
  void handleZooming(SDL_Event &ev) {
    using namespace zoom;
    if(zoomoff || !(cmode & sm::ZOOMABLE)) { 
      nozoom(); return; 
      }
    if(ev.type == SDL_MOUSEBUTTONDOWN) initzoom();
    if(ev.type == SDL_MOUSEBUTTONUP && zoomf > 1) stopzoom();
    }
#else
  inline void handleZooming(SDL_Event &ev) {}
#endif
  
  vector<item> items;
  
  item& lastItem() { return items[items.size() - 1]; }
  
  map<int, reaction_t> key_actions;
  
  void add_key_action(int key, const reaction_t& action) {
    while(key_actions.count(key)) key++;
    key_actions[key] = action;
    }

  void add_action(const reaction_t& action) {
    add_key_action(lastItem().key, action);
    }
  
  void handler(int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    if(doexiton(sym, uni)) popScreen();
    };

  void init() {
    items.clear();
    key_actions.clear(); 
    keyhandler = dialog::handler;
    }
  
  string keyname(int k) {
    if(k == 0) return "";
    if(k == SDLK_ESCAPE) return "Esc";
    if(k == SDLK_F5) return "F5";
    if(k == SDLK_F10) return "F10";
    if(k == SDLK_F1) return "F1";
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
    it.color = dialogcolor;
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
    it.color = dialogcolor;
    it.colork = 0x808080;
    it.colorv = 0x80A040;
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
  
  int displaycolor(unsigned col) {
    int c = col >> 8;
    if(!c) return 0x181818;
    return c;
    }

  void addColorItem(string body, int value, int key) {
    item it;
    it.type = diItem;
    it.body = body;
    it.value = COLORBAR;
    it.keycaption = keyname(key);
    it.key = key;
    it.color = it.colorv = displaycolor(value);
    it.colors = it.color ^ 0x404040;
    it.colorc = it.color ^ 0x808080;
    it.colork = 0x808080;
    it.scale = 100;
    items.push_back(it);
    }

  void addHelp(string body) {
    item it;
    it.type = diHelp;
    it.body = body;
    it.scale = 100;

    if(isize(body) >= 500) it.scale = 70;

    items.push_back(it);
    }

  void addInfo(string body, color_t color) {
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
    it.color = dialogcolor;
    it.colork = 0x808080;
    it.colors = 0xFFD500;
    it.colorc = 0xFF8000;
    it.scale = 100;
    items.push_back(it);
    }

  void addBigItem(string body, int key) {
    item it;
    it.type = diBigItem;
    it.body = body;
    it.keycaption = keyname(key);
    it.key = key;
    it.color = 0xC06000;
    it.colors = 0xFFD500;
    it.colorc = 0xFF8000;
    it.scale = 150;
    items.push_back(it);
    }

  int addBreak(int val) {
    item it;
    it.type = diBreak;
    it.scale = val;
    items.push_back(it);
    return items.size()-1;
    }
  
  void addTitle(string body, color_t color, int scale) {
    item it;
    it.type = diTitle;
    it.body = body;
    it.color = color;
    it.scale = scale;
    items.push_back(it);
    }

  void init(string title, color_t color, int scale, int brk) { 
    init();
    addTitle(title, color, scale);
    addBreak(brk);
    }

  int dcenter, dwidth;

  int dialogflags;

  int displayLong(string str, int siz, int y, bool measure) {

    int last = 0;
    int lastspace = 0;
    
    int xs, xo;
    if(current_display->sidescreen)
      xs = dwidth - vid.fsize*2, xo = vid.yres + vid.fsize;
    else
      xs = vid.xres * 618/1000, xo = vid.xres * 186/1000;
    
    for(int i=0; i<=isize(str); i++) {
      int ls = 0;
      int prev = last;
      if(str[i] == ' ') lastspace = i;
      if(textwidth(siz, str.substr(last, i-last)) > xs) {
        if(lastspace == last) ls = i-1, last = i-1;
        else ls = lastspace, last = ls+1;
        }
      if(str[i] == 10 || i == isize(str)) ls = i, last = i+1;
      if(ls) {
        if(!measure) displayfr(xo, y, 2, siz, str.substr(prev, ls-prev), dialogcolor, 0);
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
        if(items[i].type == diTitle || items[i].type == diInfo || items[i].type == diBigItem)
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
    #if ISMOBILE || ISPANDORA
    dfsize *= 3;
    #endif
    dfspace = dfsize * 5/4;
    
    dcenter = vid.xres/2;
    dwidth = vid.xres;

    if(current_display->sidescreen) {
      dwidth = vid.xres - vid.yres;
      dcenter = vid.xres - dwidth / 2;
      }
    
    measure();
    
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
      I.position = mid;
      if(I.type == diTitle || I.type == diInfo) {
        displayfr(dcenter, mid, 2, dfsize * I.scale/100, I.body, I.color, 8);
        }
      else if(I.type == diItem || I.type == diBigItem) {
        bool xthis = (mousey >= top && mousey < tothei);
        if(cmode & sm::DIALOG_STRICT_X)
          xthis = xthis && (mousex >= dcenter - dialogwidth/2 && mousex <= dcenter + dialogwidth/2);
#if ISMOBILE
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
        if(I.type == diBigItem) {
          displayfr(dcenter, mid, 2, dfsize * I.scale/100, I.body, I.color, 8);
          }
        else {
          if(!mousing)
            displayfr(keyx, mid, 2, dfsize * I.scale/100, I.keycaption, I.colork, 16);
          displayfr(itemx, mid, 2, dfsize * I.scale/100, I.body, I.color, 0);
          displayfr(valuex, mid, 2, dfsize * I.scale/100, I.value, I.colorv, 0);
          }
        if(xthis) getcstat = I.key;
        }      
      else if(I.type == diSlider) {
        bool xthis = (mousey >= top && mousey < tothei);
        int sl, sr;
        if(current_display->sidescreen)
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

  bool isitem(item& it) {
    return it.type == diItem || it.type == diBigItem;
    }
  
  void handleNavigation(int &sym, int &uni) {
    if(uni == '\n' || uni == '\r' || DIRECTIONKEY == SDLK_KP5)
      for(int i=0; i<isize(items); i++) 
        if(isitem(items[i]))
          if(items[i].body == highlight_text) {
            uni = sym = items[i].key;
            return;
            }
    if(DKEY == SDLK_PAGEDOWN) {
      for(int i=0; i<isize(items); i++)
        if(isitem(items[i]))
          highlight_text = items[i].body;
      }
    if(DKEY == SDLK_PAGEUP) {
      for(int i=0; i<isize(items); i++) 
        if(isitem(items[i])) {
          highlight_text = items[i].body;
          break;
          }
      }    
    if(DKEY == SDLK_UP) {
      string last = "";
      for(int i=0; i<isize(items); i++) 
        if(isitem(items[i]))
          last = items[i].body;
      uni = sym = 0;
      for(int i=0; i<isize(items); i++)
        if(isitem(items[i])) {
          if(items[i].body == highlight_text) {
            highlight_text = last; return;
            }
          else last = items[i].body;
          }
      highlight_text = last;
      }
    if(DKEY == SDLK_DOWN) {
      int state = 0;
      for(int i=0; i<isize(items); i++)
        if(isitem(items[i])) {
          if(state) { highlight_text = items[i].body; return; }
          else if(items[i].body == highlight_text) state = 1;
          }
      for(int i=0; i<isize(items); i++)
        if(isitem(items[i])) 
          highlight_text = items[i].body;
      uni = sym = 0;
      }
    if(key_actions.count(uni)) {
      key_actions[uni]();
      sym = uni = 0;
      return;
      }
    if(key_actions.count(sym)) {
      key_actions[sym]();
      sym = uni = 0;
      return;
      }
    }

  color_t colorhistory[10] = {
    0x202020FF, 0x800000FF, 0x008000FF, 0x000080FF, 
    0x404040FF, 0xC0C0C0FF, 0x804000FF, 0xC0C000FF,
    0x408040FF, 0xFFD500FF
    }, lch;
  
  color_t *palette;
  
  int colorp = 0;
  
  color_t *colorPointer;
  
  bool handleKeyColor(int sym, int uni) {
    unsigned& color = *colorPointer;
    int shift = colorAlpha ? 0 : 8;

    if(uni >= 'A' && uni <= 'D') {
      int x = (mousex - (dcenter-dwidth/4)) * 510 / dwidth;
      if(x < 0) x = 0;
      if(x > 255) x = 255;
      setpart(color, uni - 'A') = x;
      }
    else if(uni == ' ' || uni == '\n' || uni == '\r') {
      bool inHistory = false;
      for(int i=0; i<10; i++) if(colorhistory[i] == (color << shift))
        inHistory = true;
      if(!inHistory) { colorhistory[lch] = (color << shift); lch++; lch %= 10; }
      if(reaction) reaction();
      popScreen();
      }
    else if(uni >= '0' && uni <= '9') {
      color = colorhistory[uni - '0'] >> shift;
      }
    else if(palette && uni >= 'a' && uni < 'a'+(int) palette[0]) {
      color = palette[1 + uni - 'a'] >> shift;
      }
    else if(DKEY == SDLK_DOWN) {
      colorp = (colorp-1) & 3;
      }
    else if(DKEY == SDLK_UP) {
      colorp = (colorp+1) & 3;
      }
    else if(DKEY == SDLK_LEFT) {
      setpart(color, colorp) -= abs(shiftmul) < .6 ? 1 : 17;
      }
    else if(DKEY == SDLK_RIGHT) {
      setpart(color, colorp) += abs(shiftmul) < .6 ? 1 : 17;
      }
    else if(doexiton(sym, uni)) {
      popScreen();
      }
    return false;
    }
  
  bool colorAlpha;
  
  void drawColorDialog() {
    cmode = sm::NUMBER | dialogflags;
    if(cmode & sm::SIDE) gamescreen(0);

    dcenter = vid.xres/2;
    dwidth = vid.xres;

    if(current_display->sidescreen) {
      dwidth = vid.xres - vid.yres;
      dcenter = vid.xres - dwidth / 2;
      }

    color_t color = *colorPointer;
    
    int ash = 8;
    
    for(int j=0; j<10; j++) {
      int x = dcenter + vid.fsize * 2 * (j-5);
      int y = vid.yres / 2- 5 * vid.fsize;
      
      string s0 = ""; s0 += ('0'+j);

      vid.fsize *= 2;
      displayColorButton(x, y, s0, '0'+j, 0, 0, colorhistory[j] >> ash);
      vid.fsize /= 2;
      }
    
    if(palette) {
      int q = palette[0]; 
      for(int i=0; i<q; i++) {
        int x = dcenter + vid.fsize * (2 * i-q);
        int y = vid.yres / 2- 7 * vid.fsize;
        string s0 = ""; s0 += ('a'+i);
        vid.fsize *= 2;
        displayColorButton(x, y, s0, 'a'+i, 0, 0, palette[i+1] >> ash);
        vid.fsize /= 2;
        }
      }

    for(int i=0; i<4; i++) {
      int y = vid.yres / 2 + (2-i) * vid.fsize * 2;
      if(i == 3 && !colorAlpha) continue;
      
      color_t col = ((i==colorp) && !mousing) ? 0xFFD500 : dialogcolor;

      displayColorButton(dcenter - dwidth/4, y, "(", 0, 16, 0, col);
      string rgt = ") "; rgt += "ABGR" [i+(colorAlpha?0:1)];
      displayColorButton(dcenter + dwidth/4, y, rgt, 0, 0, 0, col);
      displayColorButton(dcenter - dwidth/4 + dwidth * part(color, i) / 510, y, "#", 0, 8, 0, col);
      
      if(mousey >= y - vid.fsize && mousey < y + vid.fsize)
        getcstat = 'A' + i, inslider = true;
      }
    
    displayColorButton(dcenter, vid.yres/2+vid.fsize * 6, XLAT("select this color") + " : " + itsh(color), ' ', 8, 0, color >> (colorAlpha ? ash : 0));

    if(extra_options) extra_options();
    
    keyhandler = handleKeyColor;
    }
  
  void openColorDialog(unsigned int& col, unsigned int *pal) {
    colorPointer = &col; palette = pal;
    colorAlpha = true;
    dialogflags = 0;
    pushScreen(drawColorDialog);
    reaction = reaction_t();
    extra_options = reaction_t();
    }
  
  numberEditor ne;
  
  bool editingDetail() {
    return ne.editwhat == &geom3::highdetail || ne.editwhat == &geom3::middetail;
    }
  
  int ldtoint(ld x) {
    if(x > 0) return int(x+.5);
    else return int(x-.5);
    }
  
  string disp(ld x) { if(ne.intval) return its(ldtoint(x)); else if(ne.vmax-ne.vmin < 1) return fts4(x); else return fts(x); }

  reaction_t reaction;
  reaction_t reaction_final;
  
  reaction_t extra_options;
  
  void apply_slider() {
    if(ne.intval) *ne.intval = ldtoint(*ne.editwhat);
    if(reaction) reaction();
    if(ne.intval) *ne.editwhat = *ne.intval;
    ne.s = disp(*ne.editwhat);
    anims::deanimate(*ne.editwhat);
    }
  
  void apply_edit() {
    exp_parser ep;
    ep.s = ne.s;
    ld x = real(ep.parse());
    if(!ep.ok()) return;
    if(ne.sc.positive && x <= 0) return;
    *ne.editwhat = x;
    if(ne.intval) *ne.intval = ldtoint(*ne.editwhat);
    if(ne.animatable) anims::animate_parameter(*ne.editwhat, ne.s, reaction ? reaction : reaction_final);    
    if(reaction) reaction();
    }

  void bound_low(ld val) {
    auto r = reaction;
    reaction = [r, val] () {
      if(*ne.editwhat < val) {
        *ne.editwhat = val;
        if(ne.intval) *ne.intval = ldtoint(*ne.editwhat);
        }
      if(r) r();
      };
    }

  void bound_up(ld val) {
    auto r = reaction;
    reaction = [r, val] () {
      if(*ne.editwhat > val) {
        *ne.editwhat = val;
        if(ne.intval) *ne.intval = ldtoint(*ne.editwhat);
        }
      if(r) r();
      };
    }

  int numberdark;

  void drawNumberDialog() {
    cmode = sm::NUMBER | dialogflags;
    gamescreen(numberdark);
    init(ne.title);
    addInfo(ne.s);
    addSlider(ne.sc.direct(ne.vmin), ne.sc.direct(*ne.editwhat), ne.sc.direct(ne.vmax), 500);
    addBreak(100);
#if ISMOBILE==0
    addHelp(XLAT("You can scroll with arrow keys -- Ctrl to fine-tune"));
    addBreak(100);
#endif
    
    dialog::addBack();
    addSelItem(XLAT("default value"), disp(ne.dft), SDLK_HOME);

    addBreak(100);
    
    if(cmode & sm::A3) ne.help = explain3D(ne.editwhat);

    if(ne.help != "") {
      addHelp(ne.help);
      // bool scal = !ISMOBILE && !ISPANDORA && isize(ne.help) > 160;
      // if(scal) lastItem().scale = 30;
      }

    if(extra_options) extra_options();
    
    display();
    
    keyhandler = [] (int sym, int uni) {
      handleNavigation(sym, uni);
      if((uni >= '0' && uni <= '9') || among(uni, '.', '+', '-', '*', '/', '^', '(', ')') || (uni >= 'a' && uni <= 'z')) {
        ne.s += uni;
        apply_edit();
        }
      else if(uni == '\b' || uni == '\t') {
        ne.s = ne.s. substr(0, isize(ne.s)-1);
        sscanf(ne.s.c_str(), LDF, ne.editwhat);
        apply_edit();
        }
  #if !ISMOBILE
      else if(DKEY == SDLK_RIGHT) {
        if(ne.intval && abs(shiftmul) < .6)
          (*ne.editwhat)++;
        else
          *ne.editwhat = ne.sc.inverse(ne.sc.direct(*ne.editwhat) + shiftmul * ne.step);
        apply_slider();
        }
      else if(DKEY == SDLK_LEFT) {
        if(ne.intval && abs(shiftmul) < .6)
          (*ne.editwhat)--;
        else
          *ne.editwhat = ne.sc.inverse(ne.sc.direct(*ne.editwhat) - shiftmul * ne.step);
        apply_slider();
        }
  #endif
      else if(sym == SDLK_HOME) {
        *ne.editwhat = ne.dft;
        apply_slider();
        }
      else if(uni == 500) {
        int sl, sr;
        if(current_display->sidescreen)
          sl = vid.yres + vid.fsize*2, sr = vid.xres - vid.fsize*2;
        else
          sl = vid.xres/4, sr = vid.xres*3/4;
        ld d = (mousex - sl + .0) / (sr-sl);
        *ne.editwhat = 
          ne.sc.inverse(d * (ne.sc.direct(ne.vmax) - ne.sc.direct(ne.vmin)) + ne.sc.direct(ne.vmin));
        apply_slider();
        }
      else if(doexiton(sym, uni)) { popScreen(); if(reaction_final) reaction_final(); }
      };
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
    if(pages) if(displayfrZH(xr*8, i0, 1, vid.fsize, IFM("1 - ") + XLAT("page") + " 1", nlpage == 1 ? 0xD8D8C0 : dialogcolor, 8))
      getcstat = '1';
    if(pages) if(displayfrZH(xr*24, i0, 1, vid.fsize, IFM("2 - ") + XLAT("page") + " 2", nlpage == 1 ? 0xD8D8C0 : dialogcolor, 8))
      getcstat = '2';
    if(pages) if(displayfrZH(xr*40, i0, 1, vid.fsize, IFM("3 - ") + XLAT("all"), nlpage == 1 ? 0xD8D8C0 : dialogcolor, 8))
      getcstat = '3';
    if(i&1) if(displayfrZH(xr*56, i0, 1, vid.fsize, IFM(keyname(SDLK_ESCAPE) + " - ") + XLAT("go back"), dialogcolor, 8))
      getcstat = '0';
    if(i&2) if(displayfrZH(xr*72, i0, 1, vid.fsize, IFM("F1 - ") + XLAT("help"), dialogcolor, 8))
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
  
  void editNumber(ld& x, ld vmin, ld vmax, ld step, ld dft, string title, string help) {
    ne.editwhat = &x;
    ne.s = disp(x);
    ne.vmin = vmin;
    ne.vmax = vmax;
    ne.step = step;
    ne.dft = dft;
    ne.title = title;
    ne.help = help;
    ne.sc = identity;
    ne.intval = NULL;
    dialogflags = (cmode & sm::A3);
    if(cmode & sm::SIDE) dialogflags |= sm::MAYDARK | sm::SIDE;
    cmode |= sm::NUMBER;
    pushScreen(drawNumberDialog);
    reaction = reaction_t();
    reaction_final = reaction_t();
    extra_options = reaction_t();
    numberdark = 0;
    ne.animatable = true;
    anims::get_parameter_animation(x, ne.s);
    }

  void editNumber(int& x, int vmin, int vmax, int step, int dft, string title, string help) {
    editNumber(ne.intbuf, vmin, vmax, step, dft, title, help);
    ne.intbuf = x; ne.intval = &x; ne.s = its(x);
    ne.animatable = false;
    }
  
  void helpToEdit(int& x, int vmin, int vmax, int step, int dft) {
    popScreen();
    string title = "help";
    if(help[0] == '@') {
      int iv = help.find("\t");
      int id = help.find("\n");
      title = help.substr(iv+1, id-iv-1);
      help = help.substr(id+1);
      }
    editNumber(x, vmin, vmax, step, dft, title, help);
    }
  
  //-- choose file dialog--

  #define CDIR dialogcolor
  #define CFILE forecolor
  
  bool filecmp(const pair<string,color_t> &f1, const pair<string,color_t> &f2) {
    if(f1.first == "../") return true;
    if(f2.first == "../") return false;
    if(f1.second != f2.second)
      return f1.second == CDIR;
    return f1.first < f2.first;
    }
  
  string filecaption, cfileext;
  string *cfileptr;
  bool editext = false;
  
  bool_reaction_t file_action;
  
  void handleKeyFile(int sym, int uni);

  void drawFileDialog() {
    displayfr(vid.xres/2, 30 + vid.fsize, 2, vid.fsize, 
      filecaption, forecolor, 8);
      
    string& cfile = *cfileptr;

    displayfr(vid.xres/2, 34 + vid.fsize * 2, 2, vid.fsize, 
      cfile, 0xFFFF00, 8);
    
    displayColorButton(vid.xres*1/4, 38+vid.fsize * 3, 
      XLAT("F4 = extension"), SDLK_F4, 8, 0, editext ? 0xFF00FF : 0xFFFF00, editext ? 0x800080 : 0x808000);
    displayButton(vid.xres*2/4, 38+vid.fsize * 3, 
      XLAT("Enter = choose"), SDLK_RETURN, 8);
    displayButton(vid.xres*3/4, 38+vid.fsize * 3, 
      XLAT("Esc = cancel"), SDLK_ESCAPE, 8);

    v.clear();
    
    DIR           *d;
    struct dirent *dir;
    
    string where = ".";
    for(int i=0; i<isize(cfile); i++)
      if(cfile[i] == '/' || cfile[i] == '\\')
        where = cfile.substr(0, i+1);
    
    d = opendir(where.c_str());
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        string s = dir->d_name;
        if(s != ".." && s[0] == '.') ;
        else if(isize(s) > 4 && s.substr(isize(s)-4) == cfileext)
          v.push_back(make_pair(s, CFILE));
        else if(dir->d_type & DT_DIR)
          v.push_back(make_pair(s+"/", CDIR));
        }
      closedir(d);
      }
    sort(v.begin(), v.end(), filecmp);

    int q = v.size();
    int percolumn = (vid.yres-38) / (vid.fsize+5) - 4;
    int columns = 1 + (q-1) / percolumn;
      
    for(int i=0; i<q; i++) {
      int x = 16 + (vid.xres * (i/percolumn)) / columns;
      int y = 42 + vid.fsize * 4 + (vid.fsize+5) * (i % percolumn);
        
      displayColorButton(x, y, v[i].first, 1000 + i, 0, 0, v[i].second, 0xFFFF00);
      }

    keyhandler = handleKeyFile;
    }
  
  void handleKeyFile(int sym, int uni) {
    string& s(*cfileptr);
    int i = isize(s) - (editext?0:4);
    
    if(sym == SDLK_ESCAPE) {
      popScreen();
      }
    else if(sym == SDLK_RETURN || sym == SDLK_KP_ENTER) {
      // we pop and re-push, in case if action opens something
      popScreen();
      if(!file_action()) pushScreen(drawFileDialog);
      }
    else if(sym == SDLK_F4) {
      editext = !editext;
      }
    else if(sym == SDLK_BACKSPACE && i) {
      s.erase(i-1, 1);
      }
    else if(uni >= 32 && uni < 127) {
      s.insert(i, s0 + char(uni));
      }
    else if(uni >= 1000 && uni <= 1000+isize(v)) {
      string where = "", what = s, whereparent = "../";
      for(int i=0; i<isize(s); i++)
        if(s[i] == '/') {
          if(i >= 2 && s.substr(i-2,3) == "../")
            whereparent = s.substr(0, i+1) + "../";
          else
            whereparent = where;
          where = s.substr(0, i+1), what = s.substr(i+1);
          }
      int i = uni - 1000;
      if(v[i].first == "../") {
        s = whereparent + what;
        }
      else if(v[i].second == CDIR)
        s = where + v[i].first + what;
      else
        s = where + v[i].first;
      }
    return;
    }

  void openFileDialog(string& filename, string fcap, string ext, bool_reaction_t action) {
    cfileptr = &filename;
    filecaption = fcap;
    cfileext = ext;
    file_action = action;
    pushScreen(dialog::drawFileDialog);
    }
  
  // infix/v/vpush

  string infix;
  
  bool hasInfix(const string &s) {
    if(infix == "") return true;
    string t = "";
    for(int i=0; i<isize(s); i++) {
      char c = s[i];
      char tt = 0;
      if(c >= 'a' && c <= 'z') tt += c - 32;
      else if(c >= 'A' && c <= 'Z') tt += c;
      else if(c == '@') tt += c;
      if(tt) t += tt;
      }
    return t.find(infix) != string::npos;
    }
  
  bool editInfix(int uni) {
    if(uni >= 'A' && uni <= 'Z') infix += uni;
    else if(uni >= 'a' && uni <= 'z') infix += uni-32;
    else if(infix != "" && uni == 8) infix = infix.substr(0, isize(infix)-1);
    else if(infix != "" && uni != 0) infix = "";
    else return false;
    return true;
    }
    
  vector<pair<string, color_t> > v;  

  void vpush(color_t color, const char *name) {
    string s = XLATN(name);
    if(!hasInfix(s)) return;
    dialog::v.push_back(make_pair(s, color));
    }
  
  int editpos = 0;
  string *edited_string;

  string view_edited_string() {
    string cs = *edited_string;
    if(editpos < 0) editpos = 0;
    if(editpos > isize(cs)) editpos = isize(cs);
    cs.insert(editpos, "°");
    return cs;
    }    
  
  void start_editing(string& s) {
    edited_string = &s;
    editpos = isize(s);
    }
  
  string editchecker(int sym, int uni) {
    if(uni >= 32 && uni < 127) return string("") + char(uni);
    return "";
    }

  bool handle_edit_string(int sym, int uni, function<string(int, int)> checker) {
    auto& es = *edited_string;
    string u2;
    if(DKEY == SDLK_LEFT) editpos--;
    else if(DKEY == SDLK_RIGHT) editpos++;
    else if(uni == 8) {
      if(editpos == 0) return true;
      es.replace(editpos-1, 1, "");
      editpos--;
      }
    else if((u2 = checker(sym, uni)) != "") {
      for(char c: u2) {
        es.insert(editpos, 1, c);
        editpos ++;
        }
      }
    else return false;
    return true;
    }
  
  void string_edit_dialog() {
    cmode = sm::NUMBER | dialogflags;
    gamescreen(numberdark);
    init(ne.title);
    addInfo(view_edited_string());
    addBreak(100);
    dialog::addBack();
    addBreak(100);
    
    if(ne.help != "") {
      addHelp(ne.help);
      }

    if(extra_options) extra_options();
    
    display();
    
    keyhandler = [] (int sym, int uni) {
      handleNavigation(sym, uni);
      if(handle_edit_string(sym, uni)) ;
      else if(doexiton(sym, uni)) {
        popScreen();
        if(reaction_final) reaction_final();
        }
      };
    }  

  void edit_string(string& s, string title, string help) {
    start_editing(s);
    ne.title = title;
    ne.help = help;
    dialogflags = (cmode & sm::A3);
    if(cmode & sm::SIDE) dialogflags |= sm::MAYDARK | sm::SIDE;
    cmode |= sm::NUMBER;
    pushScreen(string_edit_dialog);
    reaction = reaction_t();
    extra_options = reaction_t();
    numberdark = 0;
    }

  void confirm_dialog(const string& text, const reaction_t& act) {
    gamescreen(1);
    dialog::addBreak(250);
    dialog::init(XLAT("WARNING"), 0xFF0000, 150, 100);
    dialog::addHelp(text);
    dialog::addItem(XLAT("YES"), 'y');
    auto yes = [act] () { popScreen(); act(); };
    dialog::add_action(yes);
    dialog::add_key_action(SDLK_RETURN, yes);
    dialog::addItem(XLAT("NO"), 'n');
    dialog::add_action([] () { popScreen(); });
    dialog::display();
    }

  };

}
