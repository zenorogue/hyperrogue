videopar vid;

charstyle& getcs() {
  if(multi::players>1 && multi::cpid >= 0 && multi::cpid < multi::players)
    return multi::scs[multi::cpid];
  else
    return vid.cs;
  }

string csnameid(int id) {
  if(id == 0) return XLAT("male");
  if(id == 1) return XLAT("female");
  if(id == 2) return XLAT("Prince");
  if(id == 3) return XLAT("Princess");
  if(id == 4 || id == 5) return XLAT("cat");
  if(id == 6 || id == 7) return XLAT("dog");
  if(id == 8 || id == 9) return XLATN("Familiar");
  return XLAT("none");
  }

string csname(charstyle& cs) {
  return csnameid(cs.charid);
  }

int playergender() {
  return (getcs().charid&1) ? GEN_F : GEN_M; 
  }
int princessgender() {
  int g = playergender();
  if(vid.samegender) return g;
  return g == GEN_M ? GEN_F : GEN_M;
  }

int default_language;

int lang() { 
  if(vid.language >= 0)
    return vid.language; 
  return default_language;
  }

// Hyperbolic Rogue -- configuration

// Copyright (C) 2017- Zeno Rogue, see 'hyper.cpp' for details

bool autojoy = true;

/*struct saver {
  string name;
  virtual string save();
  virtual void load(string& s);
  };

struct intsaver : saver {
  int& val;
  int dft;
  void intsaver() { val = dft; }
  string save() { return its(val); }
  void load(string& s) { val = atoi(s.c_str()); }
  };

struct hexsaver : saver {
  int& val;
  int dft;
  void intsaver() { val = dft; }
  string save() { return itsh(val); }
  void load(string& s) { val = strtol(s.c_str(), 16); }
  };

struct ldsaver : saver {
  ld& val;
  ld dft;
  void intsaver() { val = dft; }
  string save() { return ffts(val); }
  void load(string& s) { val = atof(s.c_str()); }
  };

vector<shared_ptr<saver>> savers;

void initConfig0() {
  savers.push_back(make_shared<intsaver> ({"cs.charid", cs.charid, 0}));
  }
*/

// R:239, G:208, B:207 

unsigned int skincolors[]  = { 7, 0xD0D0D0FF, 0xEFD0C9FF, 0xC77A58FF, 0xA58869FF, 0x602010FF, 0xFFDCB1FF, 0xEDE4C8FF };
unsigned int haircolors[]  = { 8, 0x686868FF, 0x8C684AFF, 0xF2E1AEFF, 0xB55239FF, 0xFFFFFFFF, 0x804000FF, 0x502810FF, 0x301800FF };
unsigned int dresscolors[] = { 6, 0xC00000FF, 0x00C000FF, 0x0000C0FF, 0xC0C000FF, 0xC0C0C0FF, 0x202020FF };
unsigned int dresscolors2[] = { 7, 0x8080FFC0, 0x80FF80C0, 0xFF8080C0, 0xFFFF80C0, 0xFF80FFC0, 0x80FFFFC0, 0xFFFFFF80 };
unsigned int swordcolors[] = { 6, 0xC0C0C0FF, 0xFFFFFFFF, 0xFFC0C0FF, 0xC0C0FFFF, 0x808080FF, 0x202020FF };
unsigned int eyecolors[] = { 4, 0x00C000FF, 0x0000C0FF, 0xC00000FF, 0xC0C000FF };


void initcs(charstyle &cs) {
  cs.charid     = 0;
  cs.skincolor  = 0xD0D0D0FF;
  cs.haircolor  = 0x686868FF;
  cs.dresscolor = 0xC00000FF;
  cs.swordcolor = 0xD0D0D0FF;
  cs.dresscolor2= 0x8080FFC0;
  cs.uicolor    = 0xFF0000FF;
  }

#ifndef NOCONFIG
void savecs(FILE *f, charstyle& cs, int xvernum) {
  int gflags = cs.charid;
  if(vid.samegender) gflags |= 16;
  
  fprintf(f, "%d %d %08x %08x %08x %08x", 
    gflags, vid.language, cs.skincolor, cs.haircolor, cs.swordcolor, cs.dresscolor);
  if(cs.charid == 3) fprintf(f, " %08x", cs.dresscolor2);
  if(xvernum >= 8990) fprintf(f, " %x", cs.uicolor);
  fprintf(f, "\n");
  }

void loadcs(FILE *f, charstyle& cs, int xvernum) {
  int gflags, err = 
  fscanf(f, "%d%d%x%x%x%x", &gflags, &vid.language, &cs.skincolor, &cs.haircolor, &cs.swordcolor, &cs.dresscolor);

  if(err) cs.charid = gflags & 15;
  if(err) vid.samegender = (gflags & 16) ? true : false;
  if(cs.charid == 3) if(fscanf(f, "%x", &cs.dresscolor2)) 
    ;
  if(xvernum >= 8990) if(fscanf(f, "%x", &cs.uicolor)) 
    ;
  }

void initConfig() {
  vid.usingGL = true;
  vid.antialias = AA_NOGL | AA_FONT | AA_LINES | AA_LINEWIDTH | AA_VERSION;
  vid.linewidth = 1;
  vid.flashtime = 8;
  vid.scale = 1;
  vid.alpha = 1;
  vid.sspeed = 0;
  vid.mspeed = 1;
  vid.eye = 0;
  vid.full = false;
  vid.ballangle = 20;
  vid.yshift = 0;
  vid.camera_angle = 0;
  vid.ballproj = 1;
  vid.aurastr = 128;
  vid.aurasmoothen = 5;
  vid.graphglyph = 1;

#ifdef ANDROID
  vid.monmode = 2;
  vid.wallmode = 3;
#else
#ifdef PANDORA
  vid.monmode = 2;
  vid.wallmode = 3;
#else
  vid.monmode = 4;
  vid.wallmode = 5;
#endif
#endif

  vid.particles = 1;
  vid.mobilecompasssize = 30;

  vid.joyvalue = 4800;
  vid.joyvalue2 = 5600;
  vid.joypanthreshold = 2500;
#ifdef PANDORA
  vid.joypanspeed = 0.0001;
#else
  vid.joypanspeed = 0;
#endif

  vid.framelimit = 75;
  vid.axes = 1;
  vid.shifttarget = 2;
  vid.steamscore = 1;
  
  initcs(vid.cs);
  
  vid.killreduction = 0;
  
  vid.samegender = false;
  vid.language = -1;
  
  vid.drawmousecircle = false;
  vid.revcontrol = false;
#ifdef MOBILE
  vid.drawmousecircle = true;
#endif
#ifdef PANDORA
  vid.drawmousecircle = true;
#endif
  
  shmup::initConfig();  
  }

void saveConfig() {
  DEBB(DF_INIT, (debugfile,"save config\n"));
  FILE *f = fopen(conffile, "wt");
  if(!f) {
    addMessage(s0 + "Could not open the config file: " + conffile);
    return;
    }
  fprintf(f, "%d %d %d %d\n", vid.xres, vid.yres, vid.full, vid.fsize);
  fprintf(f, "%f %f %f %f\n", float(vid.scale), float(vid.eye), float(vid.alpha), float(vid.sspeed));
  fprintf(f, "%d %d %d %d %d %d %d\n", vid.wallmode, vid.monmode, vid.axes, musicvolume, vid.framelimit, vid.usingGL, vid.antialias);
  fprintf(f, "%d %d %d %f %d %d\n", vid.joyvalue, vid.joyvalue2, vid.joypanthreshold, float(vid.joypanspeed), autojoy, vid.flashtime);
  
  savecs(f, vid.cs, 0);
  
  fprintf(f, "%d %d\n", vid.darkhepta, vid.shifttarget);
  
  fprintf(f, "%d %d %d %d\n", euclid, euclidland, shmup::on, hardcore);

  shmup::saveConfig(f);

  fprintf(f, "%d %d %d %d %f %d %d\n", rug::renderonce, rug::rendernogl, rug::texturesize, purehepta, rug::scale, vid.steamscore, chaosmode);

  fprintf(f, "%d %d %f %d %d %f\n",
    pmodel, polygonal::SI, float(polygonal::STAR), polygonal::deg, 
    conformal::includeHistory, float(conformal::lvspeed));
  
  fprintf(f, "%d %d %d %d %d %d\n", 
    conformal::bandhalf, conformal::bandsegment, 
    conformal::rotation, conformal::autoband, conformal::autobandhistory,
    conformal::dospiral);
  
  fprintf(f, "%d", polygonal::maxcoef);
  for(int i=0; i<=polygonal::maxcoef; i++) fprintf(f, " %lf %lf", 
    (double) real(polygonal::coef[i]), (double) imag(polygonal::coef[i]));

  fprintf(f, "\n%d %d %d %f %d %d\n", 
    vid.revcontrol, vid.drawmousecircle, sightrange, float(vid.mspeed), effvolume, vid.particles);
  
  {
  int pt_depth = 0, pt_camera = 0, pt_alpha = 0;
  using namespace geom3;
  if(tc_depth > tc_camera) pt_depth++;
  if(tc_depth < tc_camera) pt_camera++;
  if(tc_depth > tc_alpha ) pt_depth++;
  if(tc_depth < tc_alpha ) pt_alpha ++;
  if(tc_alpha > tc_camera) pt_alpha++;
  if(tc_alpha < tc_camera) pt_camera++;
  
  fprintf(f, "%f %f %f %f %f %f %f %d %d %d %f %f %d\n",
    float(geom3::depth), float(geom3::camera), float(geom3::wall_height), 
    float(geom3::rock_wall_ratio),
    float(geom3::human_wall_ratio), 
    float(geom3::lake_top),
    float(geom3::lake_bottom),
    pt_depth, pt_camera, pt_alpha,
    float(geom3::highdetail), float(geom3::middetail),
    glyphsortorder);
  
  fprintf(f, "%f %f %f %f\n",
    float(vid.yshift), float(vid.camera_angle),
    float(vid.ballangle), float(vid.ballproj)
    );

  fprintf(f, "%d %d %d %d %f\n", vid.mobilecompasssize, vid.aurastr, vid.aurasmoothen, vid.graphglyph, float(vid.linewidth));
  }
    
  fprintf(f, "\n\nThis is a configuration file for HyperRogue (version " VER ")\n");
  fprintf(f, "\n\nThe numbers are:\n");
  fprintf(f, "screen width & height, fullscreen mode (0=windowed, 1=fullscreen), font size\n");
  fprintf(f, "scale, eye distance, parameter, scrolling speed\n");
  fprintf(f, "wallmode, monster mode, cross mode, music volume, framerate limit, usingGL, antialiasing flags\n");
  fprintf(f, "calibrate first joystick (threshold A, threshold B), calibrate second joystick (pan threshold, pan speed), joy mode\n");
  fprintf(f, "gender (1=female, 16=same gender prince), language, skin color, hair color, sword color, dress color\n");
  fprintf(f, "darken hepta, shift target\n");
  fprintf(f, "euclid, euclid land, shmup, hardcore\n");
  fprintf(f, "version number, shmup players, alwaysuse, shmup keyboard/joystick config\n");
  fprintf(f, "hypersian rug config: renderonce, rendernogl, texturesize; purehepta; rug scale; share score; chaosmode\n");
  fprintf(f, "conformal: model, sides, star, degree, includeHistory, speed\n");
  fprintf(f, "conformal: bandwidth, segment, rotation, autoband, autohistory, dospiral\n");
  fprintf(f, "conformal: degree, (degree+1) times {real, imag}\n");
  fprintf(f, "vid.revcontrol, drawmousecircle, sight range, movement animation speed, sound effect volume, particle effects\n");
  fprintf(f, "3D parameters, sort order\n");
  fprintf(f, "yhsift, camera angle, ball angle, ball projection\n");
  fprintf(f, "compass size, aura strength, aura smoothen factor, graphical glyphs\n");
  
  fclose(f);
#ifndef MOBILE
  addMessage(s0 + "Configuration saved to: " + conffile);
#else
  addMessage(s0 + "Configuration saved");
#endif
  }

void readf(FILE *f, ld& x) {
  double fl = x; 
  if(fscanf(f, "%lf", &fl)) 
    ; 
  x = fl;
  }

void loadConfig() {
 
    DEBB(DF_INIT, (debugfile,"load config\n"));
  vid.xres = 9999; vid.yres = 9999; vid.framelimit = 300;
  FILE *f = fopen(conffile, "rt");
  if(f) {
    int fs, gl=1, aa=1, bb=1, cc, dd, ee;
    int err;
    err=fscanf(f, "%d%d%d%d", &vid.xres, &vid.yres, &fs, &vid.fsize);
    vid.full = fs;
    float a, b, c, d;
    err=fscanf(f, "%f%f%f%f\n", &a, &b, &c, &d);
    if(err == 4) {
      vid.scale = a; vid.eye = b; vid.alpha = c; vid.sspeed = d;
      }
    err=fscanf(f, "%d%d%d%d%d%d%d", &vid.wallmode, &vid.monmode, &vid.axes, &musicvolume, &vid.framelimit, &gl, &vid.antialias);
    vid.usingGL = gl;
    if(vid.antialias == 0) vid.antialias = AA_VERSION | AA_LINES | AA_LINEWIDTH;
    if(vid.antialias == 1) vid.antialias = AA_NOGL | AA_VERSION | AA_LINES | AA_LINEWIDTH | AA_FONT;
    double jps = vid.joypanspeed;
    err=fscanf(f, "%d%d%d%lf%d%d", &vid.joyvalue, &vid.joyvalue2, &vid.joypanthreshold, &jps, &aa, &vid.flashtime);
    vid.joypanspeed = jps;
    autojoy = aa; aa = 0;

    loadcs(f, vid.cs, 0);

    aa=0; bb=0;
    err=fscanf(f, "%d%d", &aa, &bb);
    vid.darkhepta = aa; vid.shifttarget = bb;

    aa = geometry; bb = euclidland; cc = shmup::on; dd = hardcore;
    err=fscanf(f, "%d%d%d%d", &aa, &bb, &cc, &dd);
    geometry = eGeometry(aa); euclidland = eLand(bb); shmup::on = cc; hardcore = dd;

    shmup::loadConfig(f);

    aa = rug::renderonce; bb = rug::rendernogl; cc = purehepta; dd = chaosmode; ee = vid.steamscore;
    double rs = rug::scale;
    err=fscanf(f, "%d%d%d%d%lf%d%d", &aa, &bb, &rug::texturesize, &cc, &rs, &ee, &dd);
    rug::renderonce = aa; rug::rendernogl = bb; purehepta = cc; chaosmode = dd; vid.steamscore = ee;
    rug::scale = rs;

    aa=conformal::includeHistory;
    double ps = polygonal::STAR, lv = conformal::lvspeed;
    int pmb = pmodel;
    err=fscanf(f, "%d%d%lf%d%d%lf",
      &pmb, &polygonal::SI, &ps, &polygonal::deg,
      &aa, &lv);
    pmodel = eModel(pmb);
    conformal::includeHistory = aa; polygonal::STAR = ps; conformal::lvspeed = lv;
    
    aa=conformal::autoband; bb=conformal::autobandhistory; cc=conformal::dospiral;    
    err=fscanf(f, "%d%d%d%d%d%d", 
      &conformal::bandhalf, &conformal::bandsegment, &conformal::rotation,
      &aa, &bb, &cc);
    conformal::autoband = aa; conformal::autobandhistory = bb; conformal::dospiral = cc;
    
    err=fscanf(f, "%d", &polygonal::maxcoef);
    if(polygonal::maxcoef < 0) polygonal::maxcoef = 0;
    if(polygonal::maxcoef > polygonal::MSI) polygonal::maxcoef = polygonal::MSI;
    for(int i=0; i<=polygonal::maxcoef; i++) {
      double re=0, im=0;
      err=fscanf(f, "%lf%lf", &re, &im);
      polygonal::coef[i] = cld(re, im);
      }
    
    aa=vid.revcontrol; bb=vid.drawmousecircle;
    d = vid.mspeed;
    err=fscanf(f, "%d%d%d%f%d%d", &aa, &bb, &sightrange, &d, &effvolume, &vid.particles);
    vid.mspeed = d;
    if(sightrange < 4) sightrange = 4;
    if(sightrange > 7) sightrange = 7;
    vid.revcontrol = aa; vid.drawmousecircle = bb;
     
    readf(f, geom3::depth); readf(f, geom3::camera); readf(f, geom3::wall_height);
    readf(f, geom3::rock_wall_ratio); readf(f, geom3::human_wall_ratio);
    readf(f, geom3::lake_top); readf(f, geom3::lake_bottom);
    
    err=fscanf(f, "%d %d %d", &geom3::tc_depth, &geom3::tc_camera, &geom3::tc_alpha);
    
    readf(f, geom3::highdetail); 
    geom3::middetail = 200; readf(f, geom3::middetail);
    if(geom3::middetail == 200) {
      if(ISMOBILE)
        geom3::highdetail = 0, geom3::middetail = 3;
      else
        geom3::highdetail = geom3::middetail = 5;
      }

    int gso = glyphsortorder; err=fscanf(f, "%d", &gso); glyphsortorder = eGlyphsortorder(gso);
    
    readf(f, vid.yshift); readf(f, vid.camera_angle); readf(f, vid.ballangle); readf(f, vid.ballproj);

    jps = vid.linewidth;
    err=fscanf(f, "%d%d%d%d%lf\n", &vid.mobilecompasssize, &vid.aurastr, &vid.aurasmoothen, &vid.graphglyph, &jps);
    vid.linewidth = jps;
  
    fclose(f);
    DEBB(DF_INIT, (debugfile,"Loaded configuration: %s\n", conffile));
    
    if(err)
      ;
    }

  precalc();
  }
#endif

void showAllConfig() {
  dialog::addBreak(50);
  dialog::addItem(XLAT("exit configuration"), 'v');
#ifndef NOCONFIG
  dialog::addItem(XLAT("save the current config"), 's');
#endif
  }

void handleAllConfig(int sym, int uni) {
  if(sym == SDLK_F1 || uni == 'h') gotoHelp(help);

  else if(uni == 'v') popScreen();
  else if(sym == SDLK_ESCAPE) popScreen();
#ifndef NOCONFIG
  else if(uni == 's') saveConfig();
#endif  
  }

void showGraphConfig() {
  gamescreen(3);

  dialog::init(XLAT("graphics configuration"));

  #ifndef ONEGRAPH
  #ifdef GL
  dialog::addBoolItem(XLAT("openGL mode"), vid.usingGL, 'o');
  #endif
  #endif

  if(!vid.usingGL)
    dialog::addBoolItem(XLAT("anti-aliasing"), vid.antialias & AA_NOGL, 'O');

  if(vid.usingGL)
    dialog::addSelItem(XLAT("anti-aliasing"), 
      (vid.antialias & AA_POLY) ? "polygons" :
      (vid.antialias & AA_LINES) ? "lines" :
      (vid.antialias & AA_MULTI) ? "multisampling" :
      "NO", 'O');

  if(vid.usingGL) {
    dialog::addSelItem(XLAT("line width"), fts(vid.linewidth), 'w');
//    dialog::addBoolItem(XLAT("finer lines at the boundary"), vid.antialias & AA_LINEWIDTH, 'b');
    }

  #ifndef MOBWEB
  dialog::addSelItem(XLAT("framerate limit"), its(vid.framelimit), 'l');
  #endif

#ifndef IOS
  dialog::addBoolItem(XLAT("fullscreen mode"), (vid.full), 'f');
#endif

  dialog::addSelItem(XLAT("scrolling speed"), fts(vid.sspeed), 'a');
  dialog::addSelItem(XLAT("movement animation speed"), fts(vid.mspeed), 'm');

  dialog::addBoolItem(XLAT("extra graphical effects"), (vid.particles), 'u');

#ifdef WHATEVER
  dialog::addSelItem(XLAT("whatever"), fts(whatever), 'j');
#endif

  const char *glyphsortnames[6] = {
    "first on top", "first on bottom", 
    "last on top", "last on bottom",
    "by land", "by number"
    };
 
  const char *glyphmodenames[3] = {"letters", "auto", "images"};
  dialog::addSelItem(XLAT("inventory/kill sorting"), XLAT(glyphsortnames[glyphsortorder]), 'k');

  dialog::addSelItem(XLAT("inventory/kill mode"), XLAT(glyphmodenames[vid.graphglyph]), 'd');

#ifdef MOBILE
  dialog::addSelItem(XLAT("font scale"), its(fontscale), 'b');
#endif

  dialog::addSelItem(XLAT("sight range"), its(sightrange), 'r');

#ifdef MOBILE
  dialog::addSelItem(XLAT("compass size"), its(vid.mobilecompasssize), 'c');
#endif

  dialog::addSelItem(XLAT("aura brightness"), its(vid.aurastr), 'z');
  dialog::addSelItem(XLAT("aura smoothening factor"), its(vid.aurasmoothen), 'x');
  
  showAllConfig();
  dialog::display();

  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
  
    if(uni == 'O') uni = 'o', shiftmul = -1;
    
    char xuni = uni | 96;
  
    if(xuni == 'u') vid.particles = !vid.particles;
    if(xuni == 'd') vid.graphglyph = (1+vid.graphglyph)%3;
    
    if(xuni == 'j') {
      dialog::editNumber(whatever, -10, 10, 1, 0, XLAT("whatever"), 
        XLAT("Whatever."));
      dialog::sidedialog = true;
      }
  
    if(xuni == 'a') dialog::editNumber(vid.sspeed, -5, 5, 1, 0, 
      XLAT("scrolling speed"),
      XLAT("+5 = center instantly, -5 = do not center the map"));
  
    if(xuni == 'm') dialog::editNumber(vid.mspeed, -5, 5, 1, 0, 
      XLAT("movement animation speed"),
      XLAT("+5 = move instantly"));
  
    if(xuni == 'r') {
      dialog::editNumber(sightrange, 4, cheater ? 10 : 7, 1, 7, XLAT("sight range"), 
        XLAT("Roughly 42% cells are on the edge of your sight range. Reducing "
        "the sight range makes HyperRogue work faster, but also makes "
        "the game effectively harder."));
      dialog::sidedialog = true;
      }
  
    if(xuni == 'k') {
      glyphsortorder = eGlyphsortorder((glyphsortorder+6+(shiftmul>0?1:-1)) % gsoMAX);
      }
    
    if(xuni == 'f') switchFullscreen();
  
  #ifndef ONEGRAPH
    if(xuni == 'o' && shiftmul > 0) switchGL();
  #endif
  
    if(xuni == 'o' && shiftmul < 0) {
      if(!vid.usingGL)
        vid.antialias ^= AA_NOGL | AA_FONT;
      else if(vid.antialias & AA_MULTI)
        vid.antialias ^= AA_MULTI;
      else if(vid.antialias & AA_POLY)
        vid.antialias ^= AA_POLY | AA_LINES | AA_MULTI;
      else if(vid.antialias & AA_LINES) 
        vid.antialias |= AA_POLY;
      else 
        vid.antialias |= AA_LINES;
      setvideomode();
      }
    
    // if(xuni == 'b') vid.antialias ^= AA_LINEWIDTH;
   
    if(xuni == 'w' && vid.usingGL)
      dialog::editNumber(vid.linewidth, 0, 10, 0.1, 1, XLAT("line width"), "");
  
    if(xuni == 'c') 
      dialog::editNumber(vid.mobilecompasssize, 0, 100, 10, 20, XLAT("compass size"), "");
    
    if(xuni == 'l') 
      dialog::editNumber(vid.framelimit, 5, 300, 10, 300, XLAT("framerate limit"), "");
    
  #ifdef MOBILE
    if(xuni =='b') 
      dialog::editNumber(fontscale, 0, 400, 10, 100, XLAT("font scale"), "");
  #endif
  
    if(xuni =='z') 
      dialog::editNumber(vid.aurastr, 0, 256, 10, 128, XLAT("aura brightness"), "");
    else if(xuni =='x') 
      dialog::editNumber(vid.aurasmoothen, 1, 180, 1, 5, XLAT("aura smoothening factor"), "");
  
    handleAllConfig(sym, xuni);
    };
  }
  
void switchFullscreen() {
  vid.full = !vid.full;
#ifdef ANDROID
  addMessage(XLAT("Reenter HyperRogue to apply this setting"));
  settingsChanged = true;
#endif
#ifndef NOSDL
  if(true) {
    vid.xres = vid.full ? vid.xscr : 9999;
    vid.yres = vid.full ? vid.yscr : 9999;
    extern bool setfsize;
    setfsize = true;
    }
  setvideomode();
#endif
  }

void switchGL() {
  vid.usingGL = !vid.usingGL;
  if(vid.usingGL) addMessage(XLAT("openGL mode enabled"));
  if(!vid.usingGL) addMessage(XLAT("openGL mode disabled"));
#ifndef ANDROID
  if(!vid.usingGL) addMessage(XLAT("shift+O to switch anti-aliasing"));
#endif
#ifdef ANDROID
  settingsChanged = true;
#else
#ifndef NOSDL
  setvideomode();
#endif
#endif
  }

void showBasicConfig() {
  gamescreen(3);
  const char *axmodes[5] = {"OFF", "auto", "light", "heavy", "arrows"};
  dialog::init(XLAT("basic configuration"));

#ifndef NOTRANS
  dialog::addSelItem(XLAT("language"), XLAT("EN"), 'l');
#endif
  dialog::addSelItem(XLAT("player character"), numplayers() > 1 ? "" : csname(vid.cs), 'g');

#ifndef NOAUDIO
  dialog::addSelItem(XLAT("background music volume"), its(musicvolume), 'b');
  dialog::addSelItem(XLAT("sound effects volume"), its(effvolume), 'e');
#endif

// input:  
  dialog::addSelItem(XLAT("help for keyboard users"), XLAT(axmodes[vid.axes]), 'c');

  dialog::addBoolItem(XLAT("reverse pointer control"), (vid.revcontrol), 'r');
  dialog::addBoolItem(XLAT("draw circle around the target"), (vid.drawmousecircle), 'd');
  
  dialog::addSelItem(XLAT("message flash time"), its(vid.flashtime), 't');
#ifdef MOBILE
  dialog::addBoolItem(XLAT("targetting ranged Orbs long-click only"), (vid.shifttarget&2), 'i');
#else
  dialog::addBoolItem(XLAT("targetting ranged Orbs Shift+click only"), (vid.shifttarget&1), 'i');
#endif
#ifdef STEAM
  dialog::addBoolItem(XLAT("send scores to Steam leaderboards"), (vid.steamscore&1), 'l');
#endif

#ifndef MOBILE
  dialog::addSelItem(XLAT("configure keys/joysticks"), "", 'p');
#endif

  showAllConfig();

  if(lang() != 0) {
    string tw = "";
    string s = XLAT("TRANSLATIONWARNING");
    if(s != "" && s != "TRANSLATIONWARNING") tw += s;
    s = XLAT("TRANSLATIONWARNING2");
    if(s != "" && s != "TRANSLATIONWARNING2") { if(tw != "") tw += " "; tw += s; }
    if(tw != "") {
      dialog::addBreak(50);
      dialog::addHelp(tw);
      dialog::lastItem().color = 0xFF0000;
      }
    }

  dialog::display();
  
  keyhandler = []   (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
      
    char xuni = uni | 96;
    
    if(uni >= 32 && uni < 64) xuni = uni;
  
    if(xuni == 'c') { vid.axes += 60 + (shiftmul > 0 ? 1 : -1); vid.axes %= 5; }
  #ifndef NOAUDIO
    if(xuni == 'b') {
      dialog::editNumber(musicvolume, 0, 128, 10, 60, XLAT("background music volume"), "");
      }
    if(xuni == 'e') {
      dialog::editNumber(effvolume, 0, 128, 10, 60, XLAT("sound effects volume"), "");
      }
  #endif
    
  #ifndef NOTRANS
    if(xuni == 'l') {
      vid.language += (shiftmul>0?1:-1);
      vid.language %= NUMLAN;
      if(vid.language < 0) vid.language += NUMLAN;
  #ifdef ANDROID
      settingsChanged = true;
  #endif
      }
  #endif
    
    if(xuni == 'g') pushScreen(showCustomizeChar);
  
  #ifndef MOBILE
    if(xuni == 'p') {
      pushScreen(shmup::showShmupConfig);
      multi::shmupcfg = shmup::on;
      }
  #endif
    
    if(xuni == 'r') vid.revcontrol = !vid.revcontrol;
    if(xuni == 'd') vid.drawmousecircle = !vid.drawmousecircle;
  
  #ifdef STEAM
    if(xuni == 'l') vid.steamscore = vid.steamscore^1;
  #endif
    if(xuni == 't') 
      dialog::editNumber(vid.flashtime, 0, 64, 1, 8, XLAT("message flash time"),
        XLAT("How long should the messages stay on the screen."));
    
    if(xuni == 'i') { vid.shifttarget = vid.shifttarget^3; }
  
    handleAllConfig(sym, xuni);
    };
  }

void showJoyConfig() {
  gamescreen(4);

  dialog::init(XLAT("joystick configuration"));
  
  dialog::addSelItem(XLAT("first joystick position (movement)"), its(joyx)+","+its(joyy), 0);
  dialog::addSelItem(XLAT("second joystick position (panning)"), its(panjoyx)+","+its(panjoyy), 0);
  
#ifndef MOBWEB
  dialog::addSelItem(XLAT("joystick mode"), XLAT(autojoy ? "automatic" : "manual"), 'p');
    
  dialog::addSelItem(XLAT("first joystick: movement threshold"), its(vid.joyvalue), 'a');
  dialog::addSelItem(XLAT("first joystick: execute movement threshold"), its(vid.joyvalue2), 'b');
  dialog::addSelItem(XLAT("second joystick: pan threshold"), its(vid.joypanthreshold), 'c');
  dialog::addSelItem(XLAT("second joystick: panning speed"), fts(vid.joypanspeed * 1000), 'd');
#endif

  dialog::addItem(XLAT("back"), 'v');
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    char xuni = uni | 96;
    if(xuni == 'p') autojoy = !autojoy;
    else if(xuni == 'a') 
      dialog::editNumber(vid.joyvalue, 0, 32768, 100, 4800, XLAT("first joystick: movement threshold"), "");
    else if(xuni == 'b') 
      dialog::editNumber(vid.joyvalue2, 0, 32768, 100, 5600, XLAT("first joystick: execute movement threshold"), "");
    else if(xuni == 'c')
      dialog::editNumber(vid.joypanthreshold, 0, 32768, 100, 2500, XLAT("second joystick: pan threshold"), "");
    else if(xuni == 'd') 
      dialog::editNumber(vid.joypanspeed, 0, 1e-2, 1e-5, 1e-4, XLAT("second joystick: panning speed"), "");
  
    else if(doexiton(sym, uni)) popScreen();
    };
  }

void projectionDialog() {
  geom3::tc_alpha = ticks;
  dialog::editNumber(vid.alpha, -5, 5, .1, 1,
    XLAT("projection"),
    XLAT("HyperRogue uses the Minkowski hyperboloid model internally. "
    "Klein and Poincar‚ models can be obtained by perspective, "
    "and the Gans model is obtained by orthogonal projection. "
//  "This parameter specifies the distance from the hyperboloid center "
//  "to the eye. "
    "See also the conformal mode (in the special modes menu) "
    "for more models."));
  dialog::sidedialog = true;
  }

string explain3D(ld *param) {
  using namespace geom3;
  if(param == &highdetail || param == &middetail)
    return 
      XLAT(
        "Objects at distance less than %1 absolute units "
        "from the center will be displayed with high "
        "detail, and at distance at least %2 with low detail.",
        fts3(highdetail), fts3(middetail)
        );
      
  if(param == &camera)
    return
      XLAT(
        "Camera is placed %1 absolute units above a plane P in a three-dimensional "
        "world. Ground level is actually an equidistant surface, %2 absolute units "
        "below the plane P. The plane P (as well as the ground level or any "
        "other equidistant surface below it) is viewed at an angle of %3 "
        "(the tangent of the angle between the point in "
        "the center of your vision and a faraway location is 1/cosh(c) = %4).",
        fts3(camera),
        fts3(depth),
        fts3(atan(1/cosh(camera))*2*180/M_PI),
        fts3(1/cosh(camera)));
  if(param == &depth)
    return
      XLAT(
        "Ground level is actually an equidistant surface, "
        "%1 absolute units below the plane P. "
        "Theoretically, this value affects the world -- "
        "for example, eagles could fly %2 times faster by "
        "flying above the ground level, on the plane P -- "
        "but the actual game mechanics are not affected. "
        "(Distances reported by the vector graphics editor "
        "are not about points on the ground level, but "
        "about the matching points on the plane P -- "
        "divide them by the factor above to get actual "
        "distances.)"
        
        ,
        fts3(depth), fts3(cosh(depth)));
        // mention absolute units
        
  if(param == &vid.alpha)
    return
      XLAT(
        "If we are viewing an equidistant g absolute units below a plane, "
        "from a point c absolute units above the plane, this corresponds "
        "to viewing a Minkowski hyperboloid from a point "
        "tanh(g)/tanh(c) units below the center. This in turn corresponds to "
        "the Poincaré model for g=c, and Klein-Beltrami model for g=0.");
      
  if(param == &wall_height)
    return
      XLAT(
        "The height of walls, in absolute units. For the current values of g and c, "
        "wall height of %1 absolute units corresponds to projection value of %2.",
        fts3(wall_height), fts3(factor_to_projection(geom3::WALL)));

  if(param == &rock_wall_ratio)
    return
      XLAT(
        "The ratio of Rock III to walls is %1, so Rock III are %2 absolute units high. "
        "Length of paths on the Rock III level is %3 of the corresponding length on the "
        "ground level.",
        fts3(rock_wall_ratio), fts3(wall_height * rock_wall_ratio),
        fts3(cosh(depth - wall_height * rock_wall_ratio) / cosh(depth)));

  if(param == &human_wall_ratio)
    return
      XLAT(
        "Humans are %1 "
        "absolute units high. Your head travels %2 times the distance travelled by your "
        "feet.",
        fts3(wall_height * human_wall_ratio),
        fts3(cosh(depth - wall_height * human_wall_ratio) / cosh(depth)));
        
  return "";
  }

void show3D() {
  gamescreen(4);
  using namespace geom3;
  dialog::init(XLAT("3D configuration"));

  dialog::addSelItem(XLAT("High detail range"), fts(highdetail), 'n');
  dialog::addSelItem(XLAT("Mid detail range"), fts(middetail), 'm');
  
  dialog::addBreak(50);
  dialog::addSelItem(XLAT("Camera level above the plane"), fts3(camera), 'c');
  dialog::addSelItem(XLAT("Ground level below the plane"), fts3(depth), 'g');

  dialog::addSelItem(XLAT("Projection at the ground level"), fts3(vid.alpha), 'a');
  dialog::addBreak(50);
  dialog::addSelItem(XLAT("Height of walls"), fts3(wall_height), 'w');
  
  dialog::addSelItem(XLAT("Rock-III to wall ratio"), fts3(rock_wall_ratio), 'r');
  dialog::addSelItem(XLAT("Human to wall ratio"), fts3(human_wall_ratio), 'h');
  dialog::addSelItem(XLAT("Level of water surface"), fts3(lake_top), 'l');
  dialog::addSelItem(XLAT("Level of water bottom"), fts3(lake_bottom), 'k');  

  dialog::addBreak(50);
  dialog::addSelItem(XLAT("Y shift"), fts3(vid.yshift), 'y');
  dialog::addSelItem(XLAT("camera rotation"), fts3(vid.camera_angle), 's');
  dialog::addSelItem(XLAT("distance between eyes"), fts3(vid.eye), 'e');
  dialog::addBreak(50);
  dialog::addBoolItem(XLAT("ball model"), pmodel == mdBall, 'B');
  dialog::addBoolItem(XLAT("hyperboloid model"), pmodel == mdHyperboloid, 'M');
  dialog::addSelItem(XLAT("camera rotation in ball model"), fts3(vid.ballangle), 'b');
  dialog::addSelItem(XLAT("projection in ball model"), fts3(vid.ballproj), 'x');

  dialog::addBreak(50);
  if(!(wmspatial || mmspatial))
    dialog::addInfo(XLAT("set 3D monsters or walls in basic config first"));
  else if(invalid != "")
    dialog::addInfo(XLAT("error: "+invalid));
  else
    dialog::addInfo(XLAT("parameters set correctly"));
  dialog::addBreak(50);
  dialog::addItem(XLAT("exit 3D configuration"), 'v');
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    using namespace geom3;
    dialog::handleNavigation(sym, uni);
    
    if(uni == 'n') 
      dialog::editNumber(geom3::highdetail, 0, 5, .5, 7, XLAT("High detail range"), "");
    else if(uni == 'm') 
      dialog::editNumber(geom3::middetail, 0, 5, .5, 7, XLAT("Mid detail range"), "");
    else if(uni == 'c') 
      tc_camera = ticks,
      dialog::editNumber(geom3::camera, 0, 5, .1, 1, XLAT("Camera level above the plane"), "");
    else if(uni == 'g') 
      tc_depth = ticks,
      dialog::editNumber(geom3::depth, 0, 5, .1, 1, XLAT("Ground level below the plane"), "");
    else if(uni == 'a') 
      projectionDialog();
    else if(uni == 'w') 
      dialog::editNumber(geom3::wall_height, 0, 1, .1, .3, XLAT("Height of walls"), "");
    else if(uni == 'l') 
      dialog::editNumber(geom3::lake_top, 0, 1, .1, .25, XLAT("Level of water surface"), "");
    else if(uni == 'k') 
      dialog::editNumber(geom3::lake_bottom, 0, 1, .1, .9, XLAT("Level of water bottom"), "");
    else if(uni == 'r') 
      dialog::editNumber(geom3::rock_wall_ratio, 0, 1, .1, .9, XLAT("Rock-III to wall ratio"), "");
    else if(uni == 'h') 
      dialog::editNumber(geom3::human_wall_ratio, 0, 1, .1, .7, XLAT("Human to wall ratio"), "");
    
    else if(uni == 'e') {
      dialog::editNumber(vid.eye, -10, 10, 0.01, 0, XLAT("distance between eyes"),
        XLAT("Watch the Minkowski hyperboloid or the hypersian rug mode with the "
        "red/cyan 3D glasses."));
      dialog::sidedialog = true;
      }
  
    else if(uni == 'y') 
      dialog::editNumber(vid.yshift, 0, 1, .1, 0, XLAT("Y shift"), 
        "Don't center on the player character."
        );
    else if(uni == 's') 
      dialog::editNumber(vid.camera_angle, -180, 180, 5, 0, XLAT("camera rotation"), 
        "Rotate the camera. Can be used to obtain a first person perspective, "
        "or third person perspective when combined with Y shift."
        );
    else if(uni == 'b') 
      dialog::editNumber(vid.ballangle, 0, 90, 5, 0, XLAT("camera rotation in ball model"), 
        "Rotate the camera in ball/hyperboloid model.");
    else if(uni == 'x') 
      dialog::editNumber(vid.ballproj, 0, 100, .1, 0, XLAT("projection in ball model"), 
        "This parameter affects the ball model the same way as the projection parameter affects the disk model.");
    else if(uni == 'B') 
      pmodel = (pmodel == mdBall ? mdDisk : mdBall);
    else if(uni == 'M') 
      pmodel = (pmodel == mdHyperboloid ? mdDisk : mdHyperboloid);
  
    else if(doexiton(sym, uni)) popScreen();
    
    if(cmode2 == smNumber) dialog::sidedialog = true;
    };
  }

void switchcolor(int& c, unsigned int* cs) {
  dialog::openColorDialog(c, cs);
  }

void showCustomizeChar() {
  gamescreen(4);
  dialog::init(XLAT("Customize character"));
  
  if(shmup::on || multi::players) shmup::cpid = shmup::cpid_edit % shmup::players;
  charstyle& cs = getcs();
  
  dialog::addSelItem(XLAT("character"), csname(cs), 'g');
  dialog::addColorItem(XLAT("skin color"), cs.skincolor, 's');
  dialog::addColorItem(XLAT("weapon color"), cs.swordcolor, 'w');
  dialog::addColorItem(XLAT("hair color"), cs.haircolor, 'h');
  
  if(cs.charid >= 1) dialog::addColorItem(XLAT("dress color"), cs.dresscolor, 'd');
  else dialog::addBreak(100);
  if(cs.charid == 3) dialog::addColorItem(XLAT("dress color II"), cs.dresscolor2, 'f');
  else dialog::addBreak(100);
  
  dialog::addColorItem(XLAT("movement color"), cs.uicolor, 'u');

  if(!shmup::on && multi::players == 1) dialog::addSelItem(XLAT("save whom"), XLAT1(minf[moPrincess].name), 'p');
  
  if(numplayers() > 1) dialog::addSelItem(XLAT("player"), its(shmup::cpid+1), 'a');
  
  dialog::addBreak(50);
  dialog::addItem(XLAT("return to the game"), 'v');
  dialog::display();
  
  int firsty = dialog::items[0].position / 2;

  initquickqueue();
  transmatrix V = atscreenpos(vid.xres/2, firsty, firsty - 2 * vid.fsize);
  
  double alpha = atan2(mousex - vid.xres/2, mousey - firsty) - M_PI/2;
  drawMonsterType(moPlayer, NULL, V * spin(alpha), 0, 0);
  quickqueue();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    char xuni = uni | 96;
  
    if(shmup::on || multi::players) shmup::cpid = shmup::cpid_edit % shmup::players;
    charstyle& cs = getcs();
    if(xuni == 'a') { shmup::cpid_edit++; shmup::cpid_edit %= 60; }
    if(xuni == 'g') {
      cs.charid++;
      if(cs.charid == 2 && !princess::everSaved) cs.charid = 4;
      cs.charid %= 10;
      }
    if(xuni == 'p') vid.samegender = !vid.samegender;
    bool cat = cs.charid >= 4;
    if(xuni == 's') switchcolor(cs.skincolor, cat ? haircolors : skincolors);
    if(xuni == 'h') switchcolor(cs.haircolor, haircolors);
    if(xuni == 'w') switchcolor(cs.swordcolor, cat ? eyecolors : swordcolors);
    if(xuni == 'd') switchcolor(cs.dresscolor, cat ? haircolors : dresscolors);
    if(xuni == 'f') switchcolor(cs.dresscolor2, dresscolors2);
    if(xuni == 'u') switchcolor(cs.uicolor, eyecolors);
    if(xuni == 'v' || sym == SDLK_ESCAPE) popScreen();
    };
  }

