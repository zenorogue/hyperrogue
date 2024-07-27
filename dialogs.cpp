// Hyperbolic Rogue -- dialogs
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file dialogs.cpp
 *  \brief Implementation of various generic dialogs and elements of dialog windows
 */

#include "hyper.h"
namespace hr {

EX const char* COLORBAR = "###";

EX namespace dialog {

#if HDR
  #define IFM(x) (mousing?"":x)

  static constexpr int DONT_SHOW = 16;

  enum tDialogItem {diTitle, diItem, diBreak, diHelp, diInfo, diIntSlider, diSlider, diBigItem, diKeyboard, diCustom, diColorItem, diListStart, diListEnd, diMatrixItem};

  struct item {
    tDialogItem type;
    string body;
    string value;
    key_type key;
    color_t color, colorv, colork, colors, colorc;
    int scale;
    double param;
    int p1, p2, p3;
    int position;
    void *ptr;
    reaction_t customfun;
    item(tDialogItem t = diBreak);
    };
  
  struct scaler {
    ld (*direct) (ld);
    ld (*inverse) (ld);
    bool positive;
    };

  static inline ld identity_f(ld x) { return x; }
  
  const static scaler identity = {identity_f, identity_f, false};
  const static scaler logarithmic = {log, exp, true};
  const static scaler asinhic = {asinh, sinh, false};
  const static scaler asinhic100 = {[] (ld x) { return asinh(x*100); }, [] (ld x) { return sinh(x)/100; }, false};
 
  /** extendable dialog */
  struct extdialog : funbase {
    string title, help;
    int dialogflags;
    reaction_t reaction;
    reaction_t reaction_final;
    reaction_t extra_options;
    virtual void draw() = 0;
    void operator() () { draw(); }
    virtual ~extdialog() {};
    extdialog();
    /** Pop screen, then call the final reaction. A bit more complex because it eeds to backup reaction_final due to popScreen */
    void popfinal() { auto rf = std::move(reaction_final); popScreen(); if(rf) rf(); }
    };

  /** number dialog */
  struct number_dialog : extdialog {
    ld *editwhat;
    string s;
    ld vmin, vmax, step, dft;
    scaler sc;
    int *intval; ld intbuf;
    bool animatable;
    bool *boolval;
    void draw() override;
    void apply_edit();
    void apply_slider();
    string disp(ld x);
    void reset_str() { s = disp(*editwhat); }
    };

  /** bool dialog */
  struct bool_dialog : extdialog {
    bool *editwhat, dft;
    reaction_t switcher;
    void draw() override;
    };
#endif

  EX number_dialog& get_ne() {
    auto ptr = dynamic_cast<number_dialog*> (screens.back().target_base());
    if(!ptr) throw hr_exception("get_ne() called without number dialog");
    return *ptr;
    }

  EX extdialog& get_di() {
    auto ptr = dynamic_cast<extdialog*> (screens.back().target_base());
    if(!ptr) throw hr_exception("get_di() called without extdialog");
    return *ptr;
    }

  EX void scaleLog() { get_ne().sc = logarithmic; }
  EX void scaleSinh() { get_ne().sc = asinhic; }
  EX void scaleSinh100() { get_ne().sc = asinhic100; }

  EX color_t dialogcolor = 0xC0C0C0;
  EX color_t dialogcolor_clicked = 0xFF8000;
  EX color_t dialogcolor_selected = 0xFFD500;
  EX color_t dialogcolor_key = 0x808080;
  EX color_t dialogcolor_value = 0x80A040;
  EX color_t dialogcolor_off = 0x40FF40;
  EX color_t dialogcolor_on = 0xC04040;
  EX color_t dialogcolor_big = 0xC06000;

  /** pick the correct dialogcolor, based on whether mouse is over */
  EX color_t dialogcolor_over(bool b) {
    if(!b) return dialogcolor;
    if(actonrelease) return dialogcolor_clicked;
    return dialogcolor_selected;
    }

  EX void addBack() {
    addItem(XLAT("go back"), 
      (cmode & sm::NUMBER) ? SDLK_RETURN :
      ISWEB ? SDLK_BACKSPACE :
      SDLK_ESCAPE);
    }

  EX void addHelp() {
    addItem(XLAT("help"), SDLK_F1);
    }

  EX namespace zoom {
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

    EX bool displayfr(int x, int y, int b, int size, const string &s, color_t color, int align) {
      return hr::displayfr(x * zoomf + shiftx, y * zoomf + shifty, b, size * zoomf, s, color, align);
      }
  
    EX bool displayfr_highlight(int x, int y, int b, int size, const string &s, color_t color, int align, int hicolor IS(0xFFFF00)) {
      bool clicked = hr::displayfr(x * zoomf + shiftx, y * zoomf + shifty, b, size * zoomf, s, color, align);
      if(clicked) hr::displayfr(x * zoomf + shiftx, y * zoomf + shifty, b, size * zoomf, s, hicolor, align);
      return clicked;
      }
    EX }

#if CAP_MENUSCALING && CAP_SDL
  EX void handleZooming(SDL_Event &ev) {
    using namespace zoom;
    if(zoomoff || !(cmode & sm::ZOOMABLE)) { 
      nozoom(); return; 
      }
    if(ev.type == SDL_MOUSEBUTTONDOWN) initzoom();
    if(ev.type == SDL_MOUSEBUTTONUP && zoomf > 1) stopzoom();
    }
#endif
#if !(CAP_MENUSCALING && CAP_SDL)
  EX void handleZooming(SDL_Event &ev) {}
#endif
  
  EX vector<item> items;
  
  EX item& lastItem() { return items[items.size() - 1]; }

  EX item& titleItem() { return items[0]; }
  
  EX map<key_type, reaction_t> key_actions;
  
  EX void add_key_action(key_type key, const reaction_t& action) {
    key_actions[key] = action;
    }
  
  EX void add_key_action_adjust(key_type& key, const reaction_t& action) {
    while(key_actions.count(key)) key++;
    add_key_action(key, action);
    }
  
  EX void extend() {
    items.back().key = items[isize(items)-2].key;
    }
  
  EX void add_action(const reaction_t& action) {
    add_key_action_adjust(lastItem().key, action);
    }
  
  EX void add_action_push(const reaction_t& action) { add_action([action] { pushScreen(action); }); }

  EX void add_action_push_clear(const reaction_t& action) { add_action([action] { clearMessages(); pushScreen(action); }); }

  EX void handler(int sym, int uni) {
    if(cmode & sm::PANNING) handlePanning(sym, uni);
    dialog::handleNavigation(sym, uni);
    if(doexiton(sym, uni)) popScreen();
    }

  EX int list_size_min, list_size_max, list_fake_key;
  EX const int first_list_fake_key = 10000;

  EX void init() {
    list_size_min = list_size_max = 0;
    list_fake_key = first_list_fake_key;
    items.clear();
    key_actions.clear(); 
    keyhandler = dialog::handler;
    }
  
  EX string keyname(key_type k) {
    if(k == 0) return "";
    if(k == SDLK_ESCAPE) return "Esc";
    if(k == SDLK_F1) return "F1";
    if(k == SDLK_F2) return "F2";
    if(k == SDLK_F3) return "F3";
    if(k == SDLK_F4) return "F4";
    if(k == SDLK_F5) return "F5";
    if(k == SDLK_F6) return "F6";
    if(k == SDLK_F7) return "F7";
    if(k == SDLK_F8) return "F8";
    if(k == SDLK_F9) return "F9";
    if(k == SDLK_F10) return "F10";
    if(k >= 10000 && k < 10500) return "";
    if(k == SDLK_HOME) return "Home";
    if(k == SDLK_BACKSPACE) return "Backspace";
    if(k == SDLK_RETURN) return "Enter";
    if(k == 32) return "space";
    if(k >= 1 && k <= 26) { string s = "Ctrl+"; s += (k+64); return s; }
    if(k < 128) { string s; s += k; return s; }
    if(k == 508) return "Alt+8";
    return "?";
    }

  item::item(tDialogItem t) {
    type = t;
    color = dialogcolor;
    colorc = dialogcolor_clicked;
    colors = dialogcolor_selected;
    colork = dialogcolor_key;
    colorv = dialogcolor_value;
    scale = 100;
    }

  EX void addSlider(double d1, double d2, double d3, key_type key) {
    item it(diSlider);
    it.key = key;
    it.param = (d2-d1) / (d3-d1);
    items.push_back(it);
    }
  
  EX void addIntSlider(int d1, int d2, int d3, key_type key) {
    item it(diIntSlider);
    it.key = key;
    it.p1 = (d2-d1);
    it.p2 = (d3-d1);
    items.push_back(it);
    }
  
  EX void addSelItem(string body, string value, key_type key) {
    item it(diItem);
    it.body = body;
    it.value = value;
    it.key = key;
    if(value == ONOFF(true)) it.colorv = dialogcolor_off;
    if(value == ONOFF(false)) it.colorv = dialogcolor_on;
    items.push_back(it);
    }

  EX void addBoolItem(string body, bool value, key_type key) {
    addSelItem(body, ONOFF(value), key);
    }
  
  EX int displaycolor(color_t col) {
    int c = col >> 8;
    if(!c) return 0x181818;
    return c;
    }

  EX void addKeyboardItem(string keys) {
    item it(diKeyboard);
    it.body = keys;
    it.scale = 150;
    items.push_back(it);
    }

  EX void addCustom(int size, reaction_t custom) {
    item it(diCustom);
    it.scale = size;
    it.customfun = custom;
    items.push_back(it);
    }

  EX void addColorItem(string body, int value, key_type key) {
    addSelItem(body, COLORBAR, key);
    auto& it = items.back();
    it.type = diColorItem;
    it.colorv = displaycolor(value);
    it.param = value & 0xFF;
    }

  ld as_degrees(transmatrix T) {
     hyperpoint h = T * point31(1, 0, 0);
     ld alpha = atan2(-h[1], h[0]);
     if(alpha == 0) alpha = 0;
     return alpha / degree;
     }

  EX void addMatrixItem(string body, transmatrix& value, key_type key, int dim IS(GDIM)) {
    addSelItem(body, COLORBAR, key);
    auto& it = items.back();
    it.type = diMatrixItem;
    it.ptr = &value;
    it.value = "";
    if(dim == 2) it.value = fts(as_degrees(value)) + "°";
    if(dim == 3) {
      for(int k=0; k<3; k++) {
        if(value[k][k] != 1) continue;
        int i=(k+1)%3;
        int j=(i+1)%3;
        if(i > j) swap(i, j);
        hyperpoint h = Hypc; h[i] = 1;
        h = value * h;
        ld alpha = atan2(-h[j], h[i]);
        if(alpha == 0) alpha = 0;
        it.value = fts(alpha / degree) + "° ";
        it.value += ('X' + i);
        it.value += ('X' + j);
        }
      if(eqmatrix(value, Id)) it.value = "Id";
      }
    it.p1 = dim;
    }

  EX void addHelp(string body) {
    item it(diHelp);
    it.body = body;

    if(isize(body) >= 500) it.scale = 70;

    items.push_back(it);
    }

  EX void addInfo(string body, color_t color IS(dialogcolor)) {
    item it(diInfo);
    it.body = body;
    it.color = color;
    items.push_back(it);
    }

  EX void addItem(string body, key_type key) {
    item it(diItem);
    it.body = body;
    it.key = key;
    items.push_back(it);
    }

  EX void addBigItem(string body, key_type key) {
    item it(diBigItem);
    it.body = body;
    it.key = key;
    it.color = dialogcolor_big;
    it.scale = 150;
    items.push_back(it);
    }

  EX int addBreak(int val) {
    item it(diBreak);
    it.scale = val;
    items.push_back(it);
    return items.size()-1;
    }
  
  EX void start_list(int size_min, int size_max, int key_start IS(0)) {
    item it(diListStart);
    it.key = key_start;
    it.scale = 0;
    it.param = size_min;
    list_size_min = size_min;
    list_size_max = size_max;
    items.push_back(it);
    }

  EX void end_list() {
    item it(diListEnd);
    it.scale = 0;
    items.push_back(it);
    }

  EX void addTitle(string body, color_t color, int scale) {
    item it(diTitle);
    it.body = body;
    it.color = color;
    it.scale = scale;
    items.push_back(it);
    }

  EX void init(string title, color_t color IS(0xE8E8E8), int scale IS(150), int brk IS(60)) { 
    init();
    addTitle(title, color, scale);
    addBreak(brk);
    }

  EX int dcenter, dwidth;

  EX int displayLong(string str, int siz, int y, bool measure) {

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

  EX int tothei, dialogwidth, dfsize, dfspace, odfspace, leftwidth, rightwidth, innerwidth, itemx, keyx, valuex, top, list_starts_at, list_ends_at, list_full_size, list_actual_size, list_skip, fwidth;

  EX string highlight_text;
  EX int highlight_key;

  EX bool is_highlight(item& I) { return I.body == highlight_text && among(highlight_key, I.key, PSEUDOKEY_SELECT); }
  EX void set_highlight(item& I) { highlight_text = I.body; highlight_key = I.key; }
  EX void find_highlight(const string& s) {
    println(hlog, "highlight_text set to ", s);
    highlight_text = s; highlight_key = PSEUDOKEY_SELECT;
    }
  
  EX void measure() {
    tothei = 0;
    dialogwidth = 0;
    innerwidth = 0;
    int N = items.size();
    list_starts_at = list_ends_at = list_actual_size = 0;

    bool autoval = cmode & sm::AUTO_VALUES;
    rightwidth = 0;
    if(!autoval) rightwidth = textwidth(dfsize, "MMMMMMMM") + dfsize/2;
    if(cmode & sm::DIALOG_WIDE)
      innerwidth = textwidth(dfsize, "MMMMM") * 7;

    for(int i=0; i<N; i++) {
      if(items[i].type == diListStart)
        list_starts_at = tothei;
      else if(items[i].type == diListEnd) {
        list_full_size = tothei - list_starts_at;
        list_actual_size = min(odfspace * list_size_max / 100, max(odfspace * list_size_min / 100, list_full_size));
        if(list_full_size < list_actual_size) list_full_size = list_actual_size;
        tothei = list_ends_at = list_starts_at + list_actual_size;
        }
      else if(items[i].type == diHelp)
        tothei += displayLong(items[i].body, dfsize * items[i].scale / 100, 0, true);
      else {
        tothei += dfspace * items[i].scale / 100;
        if(among(items[i].type, diItem, diColorItem))
          innerwidth = max(innerwidth, textwidth(dfsize * items[i].scale / 100, items[i].body));
        if(among(items[i].type, diItem))
          rightwidth = max(rightwidth, textwidth(dfsize * items[i].scale / 100, items[i].value) + dfsize);
        if(items[i].type == diTitle || items[i].type == diInfo || items[i].type == diBigItem)
          dialogwidth = max(dialogwidth, textwidth(dfsize * items[i].scale / 100, items[i].body) * 10/9);
        }
      }
    
    leftwidth = ISMOBILE ? 0 : textwidth(dfsize, "MMMMM") + dfsize/2;
    
    fwidth = innerwidth + leftwidth + rightwidth;
    if(list_actual_size) fwidth += dfsize;
    dialogwidth = max(dialogwidth, fwidth);
    itemx  = dcenter - fwidth / 2 + leftwidth;
    keyx   = dcenter - fwidth / 2 + leftwidth - dfsize/2;
    valuex = dcenter - fwidth / 2 + leftwidth + innerwidth + dfsize/2;
    }

  EX purehookset hooks_display_dialog;
  
  EX vector<int> key_queue;
  
  EX void queue_key(int key) { key_queue.push_back(key); }
  
  EX int uishape() {
    int a = S7;
    if(a < 3) a = 3;
    if(a > 36) a = 36;
    return a;
    }

  EX void draw_list_slider(int x, int yt) {
    int a = uishape();

    flat_model_enabler fme;
    initquickqueue();
    ld pix = 1 / (2 * cgi.hcrossf / cgi.crossf);
    shiftmatrix V = shiftless(atscreenpos(0, 0, pix));

    color_t col = 0xFFFFFFFF;

    ld siz = dfsize / 2;
    ld si = siz / 2;

    int yb = yt + list_actual_size;

    curvepoint(hyperpoint(x-si, yt, 1, 1));
    for(int i=0; i<=a/2; i++)
      curvepoint(hyperpoint(x - si * cos(i*TAU/a), yb + si * sin(i*TAU/a), 1, 1));
    for(int i=(a+1)/2; i<=a; i++)
      curvepoint(hyperpoint(x - si * cos(i*TAU/a), yt + si * sin(i*TAU/a), 1, 1));
    queuecurve(V, col, 0x80, PPR::LINE);

    int yt1 = yt + (list_actual_size * list_skip) / list_full_size;
    int yb1 = yt + (list_actual_size * (list_skip + list_actual_size)) / list_full_size;

    curvepoint(hyperpoint(x-siz, yt1, 1, 1));
    for(int i=0; i<=a/2; i++)
      curvepoint(hyperpoint(x - siz * cos(i*TAU/a), yb1 + siz * sin(i*TAU/a), 1, 1));
    for(int i=(a+1)/2; i<=a; i++)
      curvepoint(hyperpoint(x - siz * cos(i*TAU/a), yt1 + siz * sin(i*TAU/a), 1, 1));
    queuecurve(V, col, 0x80, PPR::LINE);

    quickqueue();
    }

  EX void draw_slider(int sl, int sr, int y, item& I) {
    int sw = sr-sl;
    int mid = y;

    if(!wmascii) {
      int a =uishape();

      flat_model_enabler fme;
      initquickqueue();
      ld pix = 1 / (2 * cgi.hcrossf / cgi.crossf);
      shiftmatrix V = shiftless(atscreenpos(0, 0, pix));

      color_t col = addalpha(I.color);

      ld siz = dfsize * I.scale / 150;
      ld si = siz / 2;
      if(I.type == diIntSlider && I.p2 < sw/4) {
        for(int a=0; a<=I.p2; a++) {
          ld x = sl + sw * a * 1. / I.p2;
          curvepoint(hyperpoint(x, y-si, 1, 1));
          curvepoint(hyperpoint(x, y+si, 1, 1));
          queuecurve(V, col, 0, PPR::LINE);
          }
        }

      curvepoint(hyperpoint(sl, y-si, 1, 1));
      for(int i=0; i<=a/2; i++)
        curvepoint(hyperpoint(sr + si * sin(i*TAU/a), y - si * cos(i*TAU/a), 1, 1));
      for(int i=(a+1)/2; i<=a; i++)
        curvepoint(hyperpoint(sl + si * sin(i*TAU/a), y - si * cos(i*TAU/a), 1, 1));
      queuecurve(V, col, 0x80, PPR::LINE);
      quickqueue();

      ld x = sl + sw * (I.type == diIntSlider ? I.p1 * 1. / I.p2 : I.param);
      if(x < sl-si) {
        curvepoint(hyperpoint(sl-si, y, 1, 1));
        curvepoint(hyperpoint(x, y, 1, 1));
        queuecurve(V, col, 0x80, PPR::LINE);
        quickqueue();
        }
      if(x > sr+si) {
        curvepoint(hyperpoint(sr+si, y, 1, 1));
        curvepoint(hyperpoint(x, y, 1, 1));
        queuecurve(V, col, 0x80, PPR::LINE);
        quickqueue();
        }
      for(int i=0; i<=a; i++) curvepoint(hyperpoint(x + siz * sin(i*TAU/a), y - siz * cos(i*TAU/a), 1, 1));
      queuecurve(V, col, col, PPR::LINE);
      quickqueue();
      }
    else if(I.type == diSlider) {
      displayfr(sl, mid, 2, dfsize * I.scale/100, "(", I.color, 16);
      displayfr(sl + double(sw * I.param), mid, 2, dfsize * I.scale/100, "#", I.color, 8);
      displayfr(sr, mid, 2, dfsize * I.scale/100, ")", I.color, 0);
      }
    else {
      displayfr(sl, mid, 2, dfsize * I.scale/100, "{", I.color, 16);
      if(I.p2 < sw / 4) for(int a=0; a<=I.p2; a++) if(a != I.p1)
        displayfr(sl + double(sw * a / I.p2), mid, 2, dfsize * I.scale/100, a == I.p1 ? "#" : ".", I.color, 8);
      displayfr(sl + double(sw * I.p1 / I.p2), mid, 2, dfsize * I.scale/100, "#", I.color, 8);
      displayfr(sr, mid, 2, dfsize * I.scale/100, "}", I.color, 0);
      }
    }

  EX void visualize_matrix(const trans23& T, ld x, ld y, ld r, int dim, ld tsize) {
    vector<hyperpoint> pts;
    for(int a=0; a<dim; a++) {
      hyperpoint h = C0; h[a] = r;
      pts.push_back(T.get() * h);
      }

    flat_model_enabler fme;
    initquickqueue();
    ld pix = 1 / (2 * cgi.hcrossf / cgi.crossf);
    shiftmatrix V = shiftless(atscreenpos(x, y, pix));

    for(int i=0; i<=360; i++)
      curvepoint(hyperpoint(r * sin(i*degree), r*cos(i*degree), 1, 1));
    queuecurve(V, 0xFFFFFFFF, 0x202020FF, PPR::LINE);

    color_t cols[3] = {0xFF8080FF, 0x80FF80FF, 0x8080FFFF};
    for(int a=0; a<dim; a++) {
      auto pt = pts[a]; pt[2] = 1; pt[3] = 1;
      curvepoint(hyperpoint(0,0,1,1));
      curvepoint(pt);
      // queueline(V * hyperpoint(0,0,1,1), V * pt, cols[a], 0);
      queuecurve(V, cols[a], 0, PPR::LINE);
      }
    if(dim == 3) for(int a=0; a<dim; a++) {
      auto pt = pts[a]; ld val = -pt[2] * tsize / r / 5;
      curvepoint(hyperpoint(pt[0], pt[1]+val, 1, 1));
      curvepoint(hyperpoint(pt[0]-val, pt[1]-val*sqrt(3)/2, 1, 1));
      curvepoint(hyperpoint(pt[0]+val, pt[1]-val*sqrt(3)/2, 1, 1));
      curvepoint(hyperpoint(pt[0], pt[1]+val, 1, 1));
      queuecurve(V, cols[a], cols[a] & 0xFFFFFF80, PPR::LINE);
      }
    quickqueue();
    }

  EX void draw_side_shade() {
    int steps = menu_darkening - darken;

    color_t col = (backcolor << 8) | (255 - (255 >> steps));

    if(svg::in || !(auraNOGL || vid.usingGL)) {
      flat_model_enabler fme;
      initquickqueue();
      ld pix = 1 / (2 * cgi.hcrossf / cgi.crossf);
      curvepoint(hyperpoint(vid.xres-dwidth, -10, 1, 1));
      curvepoint(hyperpoint(vid.xres + 10, -10, 1, 1));
      curvepoint(hyperpoint(vid.xres + 10, vid.yres + 10, 1, 1));
      curvepoint(hyperpoint(vid.xres-dwidth, vid.yres + 10, 1, 1));
      curvepoint(hyperpoint(vid.xres-dwidth, -10, 1, 1));
      shiftmatrix V = shiftless(atscreenpos(0, 0, pix));
      queuecurve(V, 0, col, PPR::LINE);
      quickqueue();
      }

    #if CAP_GL
    else {
      auto full = part(col, 0);
      static vector<glhr::colored_vertex> auravertices;
      auravertices.clear();
      ld width = vid.xres / 100;
      for(int i=4; i<steps && i < 8; i++) width /= sqrt(2);
      for(int x=0; x<16; x++) {
        for(int c=0; c<6; c++) {
          int bx = (c == 1 || c == 3 || c == 5) ? x+1 : x;
          int by = (c == 2 || c == 4 || c == 5) ? vid.yres : 0;
          int cx = bx == 0 ? 0 : bx == 16 ?vid.xres :
            vid.xres - dwidth + width * tan((bx-8)/8. * 90._deg);
          part(col, 0) = lerp(0, full, bx / 16.);
          auravertices.emplace_back(hyperpoint(cx - current_display->xcenter, by - current_display->ycenter, 0, 1), col);
          }
        }
      glflush();
      current_display->next_shader_flags = GF_VARCOLOR;
      dynamicval<eModel> m(pmodel, mdPixel);
      current_display->set_all(0, 0);
      glhr::id_modelview();
      glhr::prepare(auravertices);
      glhr::set_depthtest(false);
      glDrawArrays(GL_TRIANGLES, 0, isize(auravertices));
      }
    #endif
    }

  EX void display() {

    callhooks(hooks_display_dialog);
    if(just_refreshing) return;
    int N = items.size();
    dfsize = vid.fsize;
    #if ISMOBILE || ISPANDORA
    dfsize *= 3;
    #endif
    dfspace = dfsize * 5/4;
    odfspace = dfspace;
    
    dcenter = vid.xres/2;
    dwidth = vid.xres;

    if(current_display->sidescreen) {
      dwidth = vid.xres - vid.yres;
      dcenter = vid.xres - dwidth / 2;
      }
    else if(cmode & sm::DIALOG_OFFMAP) {
      dwidth = vid.xres / 3;
      dcenter = vid.xres * 5 / 6;
      }
    
    measure();
    
    while(tothei > vid.yres - 5 * vid.fsize) {
      int adfsize = int(dfsize * sqrt((vid.yres - 5. * vid.fsize) / tothei));
      if(adfsize < dfsize-1) dfsize = adfsize + 1;
      else dfsize--;
      odfspace = dfspace = dfsize * 5/4;
      measure();
      }
    while(dialogwidth > dwidth) {
      int adfsize = int(dfsize * sqrt(vid.xres * 1. / dialogwidth));
      if(adfsize < dfsize-1) dfsize = adfsize + 1;
      else dfsize--;
      // usually we want to keep dfspace, but with NARROW_LINES, just odfspace
      if(cmode & sm::NARROW_LINES)
        dfspace = (dfsize + 3) * 5 / 4;
      measure();
      }
    
    tothei = (vid.yres - tothei) / 2;
    int list_left = list_actual_size;
    int list_next_key;
    if(!list_actual_size) {
      if(list_skip) println(hlog, "list_skip reset");
      list_skip = 0;
      }
    
    if(current_display->sidescreen && darken < menu_darkening) draw_side_shade();

    bool inlist = false;
    int need_to_skip = 0;
    int list_more_skip = list_skip;

    for(int i=0; i<N; i++) {
      item& I = items[i];

      if(I.type == diHelp) {
        tothei = displayLong(items[i].body, dfsize * items[i].scale / 100, tothei, false);
        continue;
        }

      int size = dfspace * I.scale / 100;
      dynamicval<int> dkb(I.key);
      bool to_highlight = is_highlight(I);

      if(I.type == diListStart) {
        list_left = list_actual_size;
        inlist = true;
        list_next_key = I.key;
        list_starts_at = tothei;
        continue;
        }

      if(I.type == diListEnd) {
        tothei += list_left;
        inlist = false;
        list_ends_at = tothei;
        draw_list_slider(dcenter + fwidth / 2 - dfsize/2, list_starts_at);
        if(mousex >= dcenter + fwidth /2 - dfsize && mousey >= list_starts_at && mousey < list_ends_at)          
          getcstat = PSEUDOKEY_LIST_SLIDER;
        if(list_left > 0) {
          list_skip -= list_left;
          list_skip -= list_more_skip;
          if(list_skip < 0) list_skip = 0;
          }
        continue;
        }

      if(inlist && list_more_skip > 0) {
        if(to_highlight) {
          list_skip -= list_more_skip;
          list_more_skip = 0;
          }
        else {
          list_more_skip -= size;
          if(list_more_skip < 0) { tothei -= list_more_skip; list_left += list_more_skip; list_more_skip = 0; }
          continue;
          }
        }

      if(inlist) {
        if(list_left < size) {
          tothei += list_left; size -= list_left; need_to_skip += size; list_left = 0;
          if(to_highlight) list_skip += need_to_skip;
          continue;
          }
        else list_left -= size;
        if(list_next_key) { if(key_actions.count(I.key)) key_actions[list_next_key] = key_actions[I.key]; I.key = list_next_key++;  }
        }

      top = tothei;
      tothei += size;
      int mid = (top + tothei) / 2;
      I.position = mid;
      if(I.type == diTitle || I.type == diInfo) {
        bool xthis = (mousey >= top && mousey < tothei && I.key);
        if(cmode & sm::DIALOG_STRICT_X)
          xthis = xthis && (mousex >= dcenter - dialogwidth/2 && mousex <= dcenter + dialogwidth/2);
        displayfr(dcenter, mid, 2, dfsize * I.scale/100, I.body, I.color, 8);
        if(xthis) getcstat = I.key;
        }
      else if(among(I.type, diItem, diBigItem, diColorItem, diMatrixItem)) {
        bool xthis = (mousey >= top && mousey < tothei);
        if(cmode & sm::DIALOG_STRICT_X)
          xthis = xthis && (mousex >= dcenter - dialogwidth/2 && mousex <= dcenter + dialogwidth/2);
#if ISMOBILE
        if(xthis && mousepressed) 
          I.color = I.colorc;
#else
        if(xthis && mousemoved) {
          set_highlight(I);
          highlight_key = dkb.backup;
          mousemoved = false;
          to_highlight = true;
          }
        if(to_highlight) {
          I.color = (xthis&&mousepressed&&actonrelease) ? I.colorc : I.colors;
          }
#endif        

        if(I.type == diBigItem) {
          displayfr(dcenter, mid, 2, dfsize * I.scale/100, I.body, I.color, 8);
          }
        else {
          if(!mousing)
            displayfr(keyx, mid, 2, dfsize * I.scale/100, keyname(I.key), I.colork, 16);
          displayfr(itemx, mid, 2, dfsize * I.scale/100, I.body, I.color, 0);

          if(I.type == diColorItem && !wmascii) {
            int a = uishape();
            flat_model_enabler fme;
            initquickqueue();
            ld pix = 1 / (2 * cgi.hcrossf / cgi.crossf);
            color_t col = addalpha(I.color);
            ld sizf = dfsize * I.scale / 150;
            ld siz = sizf * sqrt(0.15+0.85*I.param/255.);
            for(int i=0; i<=a; i++) curvepoint(hyperpoint(siz * sin(i*TAU/a), -siz * cos(i*TAU/a), 1, 1));
            shiftmatrix V = shiftless(atscreenpos(valuex + sizf, mid, pix));
            queuecurve(V, col, (I.colorv << 8) | 0xFF, PPR::LINE);
            quickqueue();
            }
          else {
            int siz = dfsize * I.scale/100;
            while(siz > 6 && textwidth(siz, I.value) + (I.type == diMatrixItem ? siz*3/2 : 0) >= vid.xres - valuex) siz--;
            displayfr(valuex + (I.type == diMatrixItem ? siz*3/2 : 0), mid, 2, siz, I.value, I.colorv, 0);

            if(I.type == diMatrixItem) visualize_matrix(*((transmatrix*)I.ptr), valuex + siz/2, mid, siz/2, I.p1, vid.fsize);
            }
          }
        if(xthis) getcstat = I.key;
        }      
      else if(among(I.type, diSlider, diIntSlider)) {
        bool xthis = (mousey >= top && mousey < tothei);
        int sl, sr;
        if(current_display->sidescreen)
          sl = vid.yres + vid.fsize*2, sr = vid.xres - vid.fsize*2;
        else
          sl = vid.xres/4, sr = vid.xres*3/4;
        draw_slider(sl, sr, mid, I);
        if(xthis) getcstat = I.key, inslider = true, slider_x = mousex;
        }
      else if(I.type == diCustom) {
        I.customfun();
        }
      else if(I.type == diKeyboard) {
        int len = 0;
        for(char c: I.body) 
          if(c == ' ' || c == '\t') len += 3;
          else len++;
        int sl, sr;
        if(current_display->sidescreen)
          sl = vid.yres + vid.fsize*2, sr = vid.xres - vid.fsize*2;
        else
          sl = vid.xres/4, sr = vid.xres*3/4;
        int pos = 0;
        for(char c: I.body) {
          string s = "";
          s += c;
          int nlen = 1;
          if(c == ' ') s = "SPACE", nlen = 3;
          if(c == '\b') s = "⌫", nlen = 1;
          if(c == '\r' || c == '\n') s = "⏎", nlen = 1;
          if(c == 1) s = "←", nlen = 1;
          if(c == 2) s = "→", nlen = 1;
          if(c == 3) s = "π";
          if(c == '\t') s = "CLEAR", nlen = 3;
          int left = sl + (sr-sl) * pos / len;
          pos += nlen;
          int right = sl + (sr-sl) * pos / len;
          bool in = (mousex >= left && mousex <= right && mousey >= top && mousey < tothei);
          int xpos = (left + right) / 2;
          if(in) {
            if(c == 1) getcstat = SDLK_LEFT;
            else if(c == 2) getcstat = SDLK_RIGHT;
            else getcstat = c;
            }
          displayfr(xpos, mid, 2, dfsize * I.scale/100, s, dialogcolor_over(in), 8);
          }
        }
      }
    }

  bool isitem(item& it) {
    return among(it.type, diItem, diBigItem, diColorItem, diMatrixItem);
    }

  EX void handle_actions(int &sym, int &uni) {
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
  
  EX void handleNavigation(int &sym, int &uni) {
    if(sym == PSEUDOKEY_LIST_SLIDER) invslider = true;
    if(invslider) {
      uni = sym = 0;
      int max = list_full_size - list_actual_size;
      list_skip = (max * (mousey - list_starts_at)) / list_actual_size;
      if(list_skip < 0) list_skip = 0;
      if(list_skip > max) list_skip = max;
      highlight_text = "//missing";
      return;
      }
    if(uni == '\n' || uni == '\r' || DIRECTIONKEY == SDLK_KP5) {
      for(int i=0; i<isize(items); i++) 
        if(isitem(items[i]))
          if(is_highlight(items[i])) {
            uni = sym = items[i].key;
            handle_actions(sym, uni);
            return;
            }
      }
    if(sym == PSEUDOKEY_WHEELUP && list_actual_size) {
      sym = 0;
      list_skip -= 30;
      highlight_text = "//missing";
      }
    if(sym == PSEUDOKEY_WHEELDOWN && list_actual_size) {
      sym = 0;
      list_skip += 30;
      highlight_text = "//missing";
      }
    if(DKEY == SDLK_PAGEDOWN) {
      uni = sym = 0;
      for(int i=0; i<isize(items); i++)
        if(isitem(items[i])) {
          set_highlight(items[i]);
          }
      }
    if(DKEY == SDLK_PAGEUP) {
      uni = sym = 0;
      for(int i=0; i<isize(items); i++) 
        if(isitem(items[i])) {
          set_highlight(items[i]);
          break;
          }
      }    
    if(DKEY == SDLK_UP) {
      uni = sym = 0;
      dialog::item *last = nullptr;
      for(int i=0; i<isize(items); i++) 
        if(isitem(items[i])) {
          last = &(items[i]);
          }
      for(int i=0; i<isize(items); i++)
        if(isitem(items[i])) {
          if(is_highlight(items[i])) {
            set_highlight(*last);
            return;
            }
          else {
           last = &(items[i]);
           }
          }
      if(last)
        set_highlight(*last);
      }
    if(DKEY == SDLK_DOWN) {
      uni = sym = 0;
      int state = 0;
      for(int i=0; i<isize(items); i++)
        if(isitem(items[i])) {
          if(state) { set_highlight(items[i]); return; }
          else if(is_highlight(items[i])) state = 1;
          }
      for(int i=0; i<isize(items); i++)
        if(isitem(items[i])) {
          set_highlight(items[i]);
          break;
          }
      }
    handle_actions(sym, uni);
    }

  color_t colorhistory[10] = {
    0x202020FF, 0x800000FF, 0x008000FF, 0x000080FF, 
    0x404040FF, 0xC0C0C0FF, 0x804000FF, 0xC0C000FF,
    0x408040FF, 0xFFD500FF
    }, lch;
  
  EX color_t *palette;
  
  int colorp = 0;
  
  color_t *colorPointer;

  struct color_dialog : extdialog {
    void draw() override;
    };
  
  EX void handleKeyColor(int sym, int uni, struct color_dialog& ne) {
    unsigned& color = *colorPointer;
    int shift = colorAlpha ? 0 : 8;

    if(uni >= 'A' && uni <= 'D') {
      int x = (slider_x - (dcenter-dwidth/4)) * 510 / dwidth;
      if(x < 0) x = 0;
      if(x > 255) x = 255;
      part(color, uni - 'A') = x;
      }
    else if(uni == ' ' || uni == '\n' || uni == '\r') {
      bool inHistory = false;
      for(int i=0; i<10; i++) if(colorhistory[i] == (color << shift))
        inHistory = true;
      if(!inHistory) { colorhistory[lch] = (color << shift); lch++; lch %= 10; }
      if(ne.reaction) ne.reaction();
      ne.popfinal();
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
      unsigned char* pts = (unsigned char*) &color;
      pts[colorp] -= abs(shiftmul) < .6 ? 1 : 17;
      }
    else if(DKEY == SDLK_RIGHT) {
      unsigned char* pts = (unsigned char*) &color;
      pts[colorp] += abs(shiftmul) < .6 ? 1 : 17;
      }
    else if(doexiton(sym, uni)) ne.popfinal();
    }
  
  EX bool colorAlpha;
  
  void color_dialog::draw() {
    cmode = sm::NUMBER | dialogflags;
    if(cmode & sm::SIDE) gamescreen();
    else emptyscreen();

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

    item it(diSlider);

    for(int i=0; i<4; i++) {
      int y = vid.yres / 2 + (2-i) * vid.fsize * 2;
      if(i == 3 && !colorAlpha) continue;
      int in = 3 - i - (colorAlpha?0:1);

      color_t colors[4] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFFFFFF};
      it.color = colors[in];
      it.param = part(color, i) / 255.;
      draw_slider(dcenter - dwidth / 4, dcenter + dwidth / 4, y, it);

      color_t col = ((i==colorp) && !mousing) ? dialogcolor_selected : dialogcolor;
      string rgt; rgt += "RGBA" [in];
      displayColorButton(dcenter + dwidth/4 + vid.fsize, y, rgt, 0, 0, 0, col);
      if(mousey >= y - vid.fsize && mousey < y + vid.fsize)
        getcstat = 'A' + i, inslider = true, slider_x = mousex;
      }
    
    displayColorButton(dcenter, vid.yres/2+vid.fsize * 6, XLAT("select this color") + " : " + hr::format(colorAlpha ? "%08X" : "%06X", color), ' ', 8, 0, color >> (colorAlpha ? ash : 0));

    if(extra_options) extra_options();
    
    keyhandler = [this] (int sym, int uni) { return handleKeyColor(sym, uni, self); };
    }
  
  EX void openColorDialog(unsigned int& col, unsigned int *pal IS(palette)) {
    color_dialog cd;
    colorPointer = &col; palette = pal;
    colorAlpha = true;
    pushScreen(cd);
    }

  #if HDR
  struct matrix_dialog : extdialog {
    transmatrix *edit_matrix;
    int dim;
    void draw() override;
    void large_viz();
    };
  #endif

  void matrix_dialog::large_viz() {
    addCustom(500, [this] {
      int siz = dfsize * 5;
      int mid = (top + tothei) / 2;
      vid.linewidth *= 3;
      visualize_matrix(*edit_matrix, dcenter, mid, siz/2, dim, vid.fsize * 2);
      vid.linewidth /= 3;
      });
    }

  void matrix_dialog::draw() {
    cmode = dialogflags;
    gamescreen();
    init(title);
    addHelp(help);
    large_viz();
    addBreak(100);
    addItem("reset", 'r');
    dialog::add_action([this] { *edit_matrix = Id; });
    if(dim == 2) {
      static ld angle;
      angle = as_degrees(*edit_matrix);
      addSelItem("enter angle", fts(angle) + "°", 'a');
      dialog::add_action([this] {
        editNumber(angle, -180, 180, 90, 0, title, help);
        auto& ne = get_ne();
        auto re = reaction;
        ne.extra_options = [this] { large_viz(); };
        ne.reaction = [re, this] { *edit_matrix = spin(angle * degree); if(re) re(); };
        ne.reaction_final = reaction;
        ne.animatable = false;
        ne.dialogflags |= dialogflags;
        });
      }
    if(dim == 3) {
      transmatrix cur = *edit_matrix;
      auto rot_but = [this, cur] (int i, int j, string title, char key) {
        addItem(title, key);
        dialog::add_action([i, j, title, this, cur] {
          static ld angle; angle = 0;
          editNumber(angle, -180, 180, 90, 0, title, XLAT("Angle to rotate by."));
          auto& ne = get_ne();
          auto re = reaction;
          ne.extra_options = [this] { large_viz(); };
          ne.reaction = [re, i, j, this, cur] { *edit_matrix = cspin(i, j, angle * degree) * cur; if(re) re(); };
          ne.reaction_final = reaction;
          ne.animatable = false;
          ne.dialogflags |= dialogflags;
          });
        };
      rot_but(0, 2, "rotate in XZ", 'x');
      rot_but(1, 2, "rotate in YZ", 'y');
      rot_but(0, 1, "rotate in XY", 'z');
      }

#if !ISMOBILE
    addBoolItem("mouse control", dialogflags & sm::MOUSEAIM, 'm');
    dialog::add_action([this] { dialogflags ^= sm::MOUSEAIM; });
    if(dialogflags & sm::MOUSEAIM) {
      *edit_matrix = cspin(0, 2, mouseaim_x) * *edit_matrix;
      *edit_matrix = cspin(1, 2, mouseaim_y) * *edit_matrix;
      mouseaim_x = mouseaim_y = 0;
      }
#endif

    static string formula;
    formula = "?";
    anims::get_parameter_animation(anims::find_param(edit_matrix), formula);
    addSelItem("enter formula", formula, 'f');
    dialog::add_action([this] {
      if(formula == "?") formula = "id";
      anims::get_parameter_animation(anims::find_param(edit_matrix), formula);
      dialog::edit_string(formula, "formula", XLAT("dxy(n) = rotate n degrees from x to y\n\nd-degree, r-radian, t-turn\n\nexample: dxy(30)*dyz(45)"));
      dialog::get_di().extra_options = [this] { large_viz(); };
      dialog::get_di().reaction = [this] {
        try {
          *edit_matrix = parsematrix(formula);
          auto p = anims::find_param(edit_matrix);
          if(p) p->load_as_animation(formula);
          }
        catch(hr_parse_exception&) { }
        catch(param_exception&) { }
        };
      dialog::get_di().dialogflags |= dialogflags;
      });
    if(extra_options) extra_options();
    addBack();
    display();
    }

  EX void editMatrix(transmatrix& T, string t, string h, int dim) {
    matrix_dialog m;
    m.edit_matrix = &T;
    m.title = t;
    m.help = h;
    m.dim = dim;
    pushScreen(m);
    }

  EX bool editingDetail() {
    auto ptr = dynamic_cast<number_dialog*> (screens.back().target_base());
    if(!ptr) return false;
    auto& ne = get_ne();
    return ne.editwhat == &vid.highdetail || ne.editwhat == &vid.middetail;
    }
  
  int ldtoint(ld x) {
    if(x > 0) return int(x+.5);
    else return int(x-.5);
    }
  
  string number_dialog::disp(ld x) {
    if(dialogflags & sm::HEXEDIT) return "0x" + itsh((unsigned long long)(x));
    if(intval) return its(ldtoint(x));
    return fts(x);
    }

  void number_dialog::apply_slider() {
    auto &ne = self;
    if(ne.intval) *ne.intval = ldtoint(*ne.editwhat);
    if(ne.reaction) ne.reaction();
    if(ne.intval) *ne.editwhat = *ne.intval;
    reset_str();
    #if CAP_ANIMATIONS
    anims::deanimate(anims::find_param(ne.editwhat));
    #endif
    }
  
  EX void use_hexeditor() {
    auto& ne = get_ne();
    ne.dialogflags |= sm::HEXEDIT;
    ne.reset_str();
    }
  
  void number_dialog::apply_edit() {
    auto& ne = self;
    try {
      exp_parser ep;
      ep.s = ne.s;    
      ld x = ep.rparse();
      if(ne.sc.positive && x <= 0) return;
      *ne.editwhat = x;
      if(ne.intval) *ne.intval = ldtoint(*ne.editwhat);
      #if CAP_ANIMATIONS
      if(ne.animatable) {
        auto p = anims::find_param(ne.intval ? (void*) ne.intval : (void*) ne.editwhat);
        if(p) p->load_as_animation(ne.s);
        }
      #endif
      if(reaction) reaction();
      }
    catch(const hr_parse_exception&) { 
      }
    catch(param_exception&) { }
    }

  EX void bound_low(ld val) {
    auto& ne = get_ne();
    auto r = ne.reaction;
    ne.reaction = [r, val, &ne] () {
      if(*ne.editwhat < val) {
        *ne.editwhat = val;
        if(ne.intval) *ne.intval = ldtoint(*ne.editwhat);
        }
      if(r) r();
      };
    }

  EX void bound_up(ld val) {
    auto& ne = get_ne();
    auto r = ne.reaction;
    ne.reaction = [r, val, &ne] () {
      if(*ne.editwhat > val) {
        *ne.editwhat = val;
        if(ne.intval) *ne.intval = ldtoint(*ne.editwhat);
        }
      if(r) r();
      };
    }

  EX void formula_keyboard(bool lr) {
    addKeyboardItem("1234567890");
    addKeyboardItem("=+-*/^()\x3");
    addKeyboardItem("qwertyuiop");
    addKeyboardItem("asdfghjkl");
    addKeyboardItem("zxcvbnm,.\b");
    addKeyboardItem(lr ? " \t\x1\x2" : " \t");
    }
  
  EX bool onscreen_keyboard = ISMOBILE;

  struct number_dialog_help {
    number_dialog *ptr;
    void operator() ();
    };

  void number_dialog_help :: operator() () {
    auto ne = *ptr;
    init("number dialog help");
    dialog::addBreak(100);
    dialog::addHelp(XLAT("You can enter formulas in this dialog."));
    dialog::addBreak(100);
    dialog::addHelp(XLAT("Functions available:"));
    addHelp(available_functions());
    dialog::addBreak(100);
    dialog::addHelp(XLAT("Constants and variables available:"));
    addHelp(available_constants());
    if(ptr && ne.animatable) {
      dialog::addBreak(100);
      dialog::addHelp(XLAT("Animations:"));
      dialog::addHelp(XLAT("a..b -- animate linearly from a to b"));
      dialog::addHelp(XLAT("a..b..|c..d -- animate from a to b, then from c to d"));
      dialog::addHelp(XLAT("a../x..b../y -- change smoothly, x and y are derivatives"));
      }
    
    /* "Most parameters can be animated simply by using '..' in their editing dialog. "
      "For example, the value of a parameter set to 0..1 will grow linearly from 0 to 1. "
      "You can also use functions (e.g. cos(0..2*pi)) and refer to other parameters."
      )); */
    
    #if CAP_ANIMATIONS
    dialog::addBreak(50);
    auto f = find_edit(!ptr ? nullptr : ne.intval ? (void*) ne.intval : (void*) ne.editwhat);
    if(f)
      dialog::addHelp(XLAT("Parameter names, e.g. '%1'", f->name));
    else
      dialog::addHelp(XLAT("Parameter names"));
    dialog::addBreak(50);
    for(auto& ap: anims::aps) {
      dialog::addInfo(ap.par->name + " = " + ap.formula);
      }
    #endif
    dialog::addBreak(50);
    dialog::addHelp(XLAT("These can be combined, e.g. %1", "projection*sin(0..2*pi)"));
    display();
    }

  EX void parser_help() {
    number_dialog_help ndh;
    ndh.ptr = nullptr;
    addItem("help", SDLK_F1);
    add_action_push(ndh);
    }
  
  void number_dialog::draw() {
    cmode = sm::NUMBER | dialogflags;
    gamescreen();
    init(title);
    addInfo(s);
    auto& ne = self;
    if(ne.intval && ne.sc.direct == &identity_f)
      addIntSlider(int(ne.vmin), int(*ne.editwhat), int(ne.vmax), 500);
    else
      addSlider(ne.sc.direct(ne.vmin), ne.sc.direct(*ne.editwhat), ne.sc.direct(ne.vmax), 500);
    addBreak(100);
#if !ISMOBILE
    addHelp(XLAT("You can scroll with arrow keys -- Ctrl to fine-tune"));
    addBreak(100);
#endif
    
    dialog::addBack();
    addSelItem(XLAT("default value"), disp(ne.dft), SDLK_HOME);
    add_edit(onscreen_keyboard);
    addItem("help", SDLK_F1);
    add_action([this] { number_dialog_help ndh; ndh.ptr = this; pushScreen(ndh); });

    addBreak(100);
    
    if(ne.help != "") {
      addHelp(ne.help);
      // bool scal = !ISMOBILE && !ISPANDORA && isize(ne.help) > 160;
      // if(scal) lastItem().scale = 30;
      }

    if(extra_options) extra_options();
    
    if(onscreen_keyboard) {
      addBreak(100);
      formula_keyboard(false);
      }
    
    display();

    #if CAP_SDL2
    texthandler = [&ne] (const SDL_TextInputEvent& ev) {
      if(key_actions.count(ev.text[0])) return;
      ne.s += ev.text;
      ne.apply_edit();
      };
    #endif
    
    keyhandler = [this, &ne] (int sym, int uni) {
      handleNavigation(sym, uni);
      if((uni >= '0' && uni <= '9') || among(uni, '.', '+', '-', '*', '/', '^', '(', ')', ',', '|', 3) || (uni >= 'a' && uni <= 'z')) {
        #if !CAP_SDL2
        if(uni == 3) ne.s += "pi";
        else ne.s += uni;
        apply_edit();
        #endif
        }
      else if(uni == '\b' || uni == '\t') {
        ne.s = ne.s. substr(0, isize(ne.s)-utfsize_before(ne.s, isize(ne.s)));
        sscanf(ne.s.c_str(), LDF, ne.editwhat);
        apply_edit();
        }
  #if !ISMOBILE
      else if(DKEY == SDLK_RIGHT) {
        if(ne.intval && abs(shiftmul) < .6)
          (*ne.editwhat)++;
        else
          *ne.editwhat = ne.sc.inverse(ne.sc.direct(*ne.editwhat) + shiftmul * ne.step);
        if(abs(*ne.editwhat) < ne.step * 1e-6 && !ne.intval) *ne.editwhat = 0;
        apply_slider();
        }
      else if(DKEY == SDLK_LEFT) {
        if(ne.intval && abs(shiftmul) < .6)
          (*ne.editwhat)--;
        else
          *ne.editwhat = ne.sc.inverse(ne.sc.direct(*ne.editwhat) - shiftmul * ne.step);
        if(abs(*ne.editwhat) < ne.step * 1e-6 && !ne.intval) *ne.editwhat = 0;
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
        ld d = (slider_x - sl + .0) / (sr-sl);
        ld val = ne.sc.inverse(d * (ne.sc.direct(ne.vmax) - ne.sc.direct(ne.vmin)) + ne.sc.direct(ne.vmin));
        ld nextval = ne.sc.inverse((mousex + 1. - sl) / (sr - sl) * (ne.sc.direct(ne.vmax) - ne.sc.direct(ne.vmin)) + ne.sc.direct(ne.vmin));
        ld dif = abs(val - nextval);
        if(dif > 1e-6) {
          ld mul = 1;
          while(dif < 10) dif *= 10, mul *= 10;
          val *= mul;
          val = floor(val + 0.5);
          val /= mul;
          }
        *ne.editwhat = val;
          
        apply_slider();
        }
      else if(doexiton(sym, uni)) ne.popfinal();
      };
    }

  void bool_dialog::draw() {
    cmode = dialogflags;
    gamescreen();
    init(title);

    dialog::addBreak(100);

    auto switch_to = [this] (bool b) {
      bool do_switch = (*editwhat != b);
      auto sw = switcher;
      auto re = reaction;
      popScreen();
      if(do_switch) { sw(); if(re) re(); }
      };

    dialog::addBoolItem(XLAT("disable"), false, '0');
    dialog::add_action([switch_to] { switch_to(false); });

    dialog::addBoolItem(XLAT("enable"), true, '1');
    dialog::add_action([switch_to] { switch_to(true); });

    dialog::addBoolItem(XLAT("switch"), !*editwhat, 's');
    dialog::add_action([switch_to, this] { switch_to(!*editwhat); });

    dialog::addBoolItem(XLAT("set default"), dft, 'd');
    dialog::add_action([switch_to, this] { switch_to(dft); });

    dialog::addBreak(100);

    if(help != "") addHelp(help);

    if(extra_options) extra_options();

    display();
    }

  int nlpage = 1;
  int wheelshift = 0;
  
  EX int handlePage(int& nl, int& nlm, int perpage) {
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
  
  EX void displayPageButtons(int i, bool pages) {
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
    if(i&4) if(displayfrZH(xr*8, i0, 1, vid.fsize, IFM("1 - ") + XLAT("plain"), dialogcolor, 8))
      getcstat = '1';
    }
  
  EX bool handlePageButtons(int uni) {
    if(uni == '1') nlpage = 1, wheelshift = 0;
    else if(uni == '2') nlpage = 2, wheelshift = 0;
    else if(uni == '3') nlpage = 0, wheelshift = 0;
    else if(uni == PSEUDOKEY_WHEELUP) wheelshift--;
    else if(uni == PSEUDOKEY_WHEELDOWN) wheelshift++;
    else return false;
    return true;
    }
  
  extdialog::extdialog() {
    dialogflags = 0;
    if(cmode & sm::SIDE) dialogflags |= sm::MAYDARK | sm::SIDE;
    reaction = reaction_t();
    reaction_final = reaction_t();
    extra_options = reaction_t();
    }

  EX number_dialog& editNumber(ld& x, ld vmin, ld vmax, ld step, ld dft, string title, string help) {
    number_dialog ne;
    ne.editwhat = &x;
    ne.vmin = vmin;
    ne.vmax = vmax;
    ne.step = step;
    ne.dft = dft;
    ne.title = title;
    ne.help = help;
    ne.sc = identity;
    ne.intval = NULL;
    ne.animatable = true;
    #if CAP_ANIMATIONS
    anims::get_parameter_animation(anims::find_param(&x), ne.s);
    #endif
    ne.reset_str();
    pushScreen(ne);
    return get_ne();
    }

  EX extdialog& editBool(bool& b, bool dft, string title, string help, const reaction_t& switcher) {
    bool_dialog be;
    be.editwhat = &b;
    be.dft = dft;
    be.title = title;
    be.help = help;
    be.switcher = switcher;
    pushScreen(be);
    return get_di();
    }

  EX number_dialog& editNumber(int& x, int vmin, int vmax, ld step, int dft, string title, string help) {
    ld tmp;
    auto& ne = editNumber(tmp, vmin, vmax, step, dft, title, help);
    ne.editwhat = &ne.intbuf; ne.intbuf = x; ne.intval = &x; ne.s = its(x);
    anims::get_parameter_animation(anims::find_param(&x), ne.s);
    return ne;
    }
  
  EX void helpToEdit(int& x, int vmin, int vmax, int step, int dft) {
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

  bool search_mode;

  struct file_dialog : extdialog {
    void draw() override;
    };

  void file_dialog::draw() {
    cmode = sm::NUMBER | dialogflags | sm::DIALOG_WIDE;
    gamescreen();
    init(filecaption);

    string cfile = *cfileptr;
    dialog::addItem(cfile, 0);

    dialog::addBreak(100);

    v.clear();

    DIR           *d;
    struct dirent *dir;

    string where = ".";
    string what = cfile;
    for(int i=0; i<isize(cfile); i++)
      if(cfile[i] == '/' || cfile[i] == '\\') {
        where = cfile.substr(0, i+1);
        what = cfile.substr(i+1);
        }

    d = opendir(where.c_str());
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        string s = dir->d_name;
        if(s != ".." && s[0] == '.') ;
        else if(search_mode) {
          if(has_substring(human_simplify(s, true), human_simplify(what, true)))
            v.push_back(make_pair(s, CFILE));
          }
        else if(isize(s) > 4 && s.substr(isize(s)-4) == cfileext)
          v.push_back(make_pair(s, CFILE));
        else if(dir->d_type & DT_DIR)
          v.push_back(make_pair(s+"/", CDIR));
        }
      closedir(d);
      }
    sort(v.begin(), v.end(), filecmp);

    dialog::start_list(2000, 2000);
    for(auto& vv: v) {
      dialog::addItem(vv.first, list_fake_key++);
      dialog::lastItem().color = vv.second;
      string vf = vv.first;
      bool dir = vv.second == CDIR;
      dialog::add_action([vf, dir] {
        string& s(*cfileptr);
        string where = "", what = s, whereparent = "../";
        string last = "";
        for(int i=0; i<isize(s); i++)
          if(s[i] == '/') {
            if(i >= 2 && s.substr(i-2,3) == "../") {
              whereparent = s.substr(0, i+1) + "../";
              last = "";
              }
            else {
              last = s.substr(isize(where), i + 1 - isize(where));
              whereparent = where;
              }
            where = s.substr(0, i+1), what = s.substr(i+1);
            }
        string str1;
        if(vf == "../") {
          s = whereparent + what;
          find_highlight(last);
          list_skip = 0;
          }
        else if(dir) {
          s = where + vf + what;
          find_highlight("../");
          list_skip = 0;
          }
        else {
          str1 = where + vf;
          if(s == str1) {
            bool ac = file_action();
            if(ac) popScreen();
            }
          s = str1;
          }
        });
      }
    dialog::end_list();

    dialog::addBreak(100);

    dialog::addBoolItem_action("extension", editext, SDLK_F4);
    dialog::addBoolItem_action("search mode", search_mode, SDLK_F3);
    dialog::addItem("choose", SDLK_RETURN);
    dialog::addItem("cancel", SDLK_ESCAPE);
    dialog::display();

    #if CAP_SDL2
    texthandler = [this] (const SDL_TextInputEvent& ev) {
      int i = isize(*cfileptr) - (editext?0:4);
      cfileptr->insert(i, ev.text);
      };
    #endif

    keyhandler = handleKeyFile;
    }
  
  EX void handleKeyFile(int sym, int uni) {
    handleNavigation(sym, uni);
    string& s(*cfileptr);
    int i = isize(s) - (editext?0:4);
    
    if(sym == SDLK_ESCAPE) {
      popScreen();
      }
    else if(sym == SDLK_RETURN || sym == SDLK_KP_ENTER) {
      bool ac = file_action();
      if(ac) popScreen();
      }
    else if(sym == SDLK_BACKSPACE && i) {
      int len = utfsize_before(s, i);
      s.erase(i-len, len);
      highlight_text = "//missing";
      list_skip = 0;
      }
    #if !CAP_SDL2
    else if(uni >= 32 && uni < 127) {
      s.insert(i, s0 + char(uni));
      highlight_text = "//missing";
      list_skip = 0;
      }
    #endif
    return;
    }

  EX void openFileDialog(string& filename, string fcap, string ext, bool_reaction_t action) {
    file_dialog fd;
    cfileptr = &filename;
    filecaption = fcap;
    cfileext = ext;
    file_action = action;
    pushScreen(fd);
    }
  
  // infix/v/vpush

  EX string infix;
  
  string foreign_letters = "ÁÄÇÈÉÍÎÖÚÜßàáâãäçèéêìíîïòóôõöøùúüýąćČčĎďĘęĚěğİıŁłńňŘřŚśŞşŠšŤťůŹźŻżŽž";
  string latin_letters   = "AACEEIIOUUsAAAAACEEEIIIIOOOOOOUUUYACCCDDEEEEGIILLNNRRSSSSSSTTUZZZZZZ";

  EX string human_simplify(const string &s, bool include_symbols) {
    string t = "";
    for(int i=0; i<isize(s); i++) {
      char c = s[i];
      char tt = 0;
      if(c >= 'a' && c <= 'z') tt += c - 32;
      else if(c >= 'A' && c <= 'Z') tt += c;
      else if(c == '@') tt += c;
      else if(include_symbols && c > 0) tt += c;
      
      if(tt == 0) for(int k=0; k<isize(latin_letters); k++) {
        if(s[i] == foreign_letters[2*k] && s[i+1] == foreign_letters[2*k+1]) {
          if(latin_letters[k] == 's') t += "SS";
          else tt += latin_letters[k];
          }
        }

      if(tt) t += tt;
      }
    return t;
    }

  EX bool hasInfix(const string &s) {
    if(infix == "") return true;
    return human_simplify(s, false).find(infix) != string::npos;
    }

  EX bool has_substring(const string &s, const string& needle) {
    int spos = 0, npos = 0;
    while(true) {
      if(npos == isize(needle)) return true;
      if(spos == isize(s)) return false;
      if(s[spos++] == needle[npos]) npos++;
      }
    }

  EX bool editInfix(int uni) {
    if(uni >= 'A' && uni <= 'Z') infix += uni;
    else if(uni >= 'a' && uni <= 'z') infix += uni-32;
    else if(infix != "" && uni == 8) infix = infix.substr(0, isize(infix)-1);
    else if(infix != "" && uni != 0) infix = "";
    else return false;
    return true;
    }
    
  EX vector<pair<string, color_t> > v;  

  EX void vpush(color_t color, const char *name) {
    string s = XLATN(name);
    if(!hasInfix(s)) return;
    dialog::v.push_back(make_pair(s, color));
    }
  
  EX string editchecker(int sym, int uni) {
      if(uni >= 32 && uni < 127) return string("") + char(uni);
      return "";
      }

  #if HDR
  struct string_dialog : extdialog {
    int editpos = 0;
    string *edited_string;
    string view_edited_string();
    void draw() override;
    void start_editing(string& s);
    bool handle_edit_string(int sym, int uni, function<string(int, int)> checker = editchecker);
    void handle_textinput();
    };
  #endif
  
  string string_dialog::view_edited_string() {
    string cs = *edited_string;
    if(editpos < 0) editpos = 0;
    if(editpos > isize(cs)) editpos = isize(cs);
    cs.insert(editpos, "°");
    return cs;
    }    
  
  void string_dialog::start_editing(string& s) {
    edited_string = &s;
    editpos = isize(s);
    }
  
  bool string_dialog::handle_edit_string(int sym, int uni, function<string(int, int)> checker) {
    auto& es = *edited_string;
    string u2;
    if(DKEY == SDLK_LEFT) editpos -= utfsize_before(es, editpos);
    else if(DKEY == SDLK_RIGHT) editpos += utfsize(es[editpos]);
    else if(uni == 8) {
      if(editpos == 0) return true;
      int len = utfsize_before(es, editpos);
      es.replace(editpos-len, len, "");
      editpos -= len;
      if(reaction) reaction();
      }
    else if((u2 = checker(sym, uni)) != "") {
      #if !CAP_SDL2
      for(char c: u2) {
        es.insert(editpos, 1, c);
        editpos ++;
        }
      #endif
      if(reaction) reaction();
      }
    else return false;
    return true;
    }

  void string_dialog::handle_textinput() {
    #if CAP_SDL2
    texthandler = [this] (const SDL_TextInputEvent& ev) {
      auto& es = *edited_string;
      string txt = ev.text;
      es.insert(editpos, txt);
      editpos += isize(txt);
      if(reaction) reaction();
      };
    #endif
    }

  void string_dialog::draw() {
    cmode = sm::NUMBER | dialogflags;
    gamescreen();
    init(title);
    addInfo(view_edited_string());
    addBreak(100);
    formula_keyboard(true);
    addBreak(100);
    dialog::addBack();
    addBreak(100);
    
    if(help != "") {
      addHelp(help);
      }

    if(extra_options) extra_options();
    
    display();
    
    keyhandler = [this] (int sym, int uni) {
      handleNavigation(sym, uni);
      if(handle_edit_string(sym, uni)) ;
      else if(doexiton(sym, uni)) popfinal();
      };

    handle_textinput();
    }

  EX void edit_string(string& s, string title, string help) {
    string_dialog ne;
    ne.title = title;
    ne.help = help;
    ne.start_editing(s);
    pushScreen(ne);
    }

  EX void confirm_dialog(const string& text, const reaction_t& act) {
    cmode = sm::DARKEN;
    gamescreen();
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

  EX void addBoolItem_action(const string& s, bool& b, int c) { 
    dialog::addBoolItem(s, b, c);
    dialog::add_action([&b] { b = !b; });
    }

  EX void addBoolItem_action_neg(const string& s, bool& b, int c) { 
    dialog::addBoolItem(s, !b, c);
    dialog::add_action([&b] { b = !b; });
    }

  EX void addItem_mouse(const string& s, key_type c) {
    dialog::addBoolItem(s, mousekey == c, c);
    }

  EX bool cheat_forbidden() {
    if(tactic::on && !cheater) {
      addMessage(XLAT("Not available in the pure tactics mode!"));
      return true;
      }
    if(daily::on) {
      addMessage(XLAT("Not available in the daily challenge!"));
      return true;
      }
    return false;
    }
  
  EX void add_action_confirmed(const reaction_t& act) {
    dialog::add_action(dialog::add_confirmation(act));
    }

  #if HDR

  template<class T> void addBoolItem_choice(const string&  s, T& b, T val, char c) {
    addBoolItem(s, b == val, c);
    add_action([&b, val] { b = val; });
    }

  inline void cheat_if_confirmed(const reaction_t& act) {
    if(cheat_forbidden())
      return;
    if(needConfirmationEvenIfSaved()) pushScreen([act] () { confirm_dialog(XLAT("This will enable the cheat mode, making this game ineligible for scoring. Are you sure?"), act); });
    else act();
    }

  inline void do_if_confirmed(const reaction_t& act) {
    if(needConfirmationEvenIfSaved()) pushScreen([act] () { confirm_dialog(XLAT("This will end your current game and start a new one. Are you sure?"), act); });
    else act();
    }

  inline void push_confirm_dialog(const reaction_t& act, const string& s) {
    pushScreen([act, s] () { confirm_dialog(s, act); });
    }

  inline reaction_t add_confirmation(const reaction_t& act) {
    return [act] { do_if_confirmed(act); };
    }
  #endif

  }

}
