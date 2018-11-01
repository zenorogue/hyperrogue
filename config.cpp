// Hyperbolic Rogue -- configuration

// Copyright (C) 2017-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

const char *conffile = "hyperrogue.ini";

videopar vid;

#define DEFAULT_WALLMODE (ISMOBILE ? 3 : 5)
#define DEFAULT_MONMODE  (ISMOBILE ? 2 : 4)

#if ISANDROID
#define ANDROID_SETTINGS settingsChanged = true;
#else
#define ANDROID_SETTINGS ;
#endif

charstyle& getcs(int id) {
  if(multi::players>1 && id >= 0 && id < multi::players)
    return multi::scs[id];
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

bool autojoy = true;

#if CAP_CONFIG
saverlist savers;
#endif

#if !CAP_CONFIG
template<class T, class U, class V> void addsaver(T& i, U name, V dft) {
  i = dft;
  }

template<class T, class U> void addsaver(T& i, U name) {}
template<class T, class U> void addsaverenum(T& i, U name) {}
template<class T, class U> void addsaverenum(T& i, U name, T dft) {}
#endif

void addsaver(charstyle& cs, string s) {
  addsaver(cs.charid, s + ".charid");
  addsaver(cs.skincolor, s + ".skincolor");
  addsaver(cs.haircolor, s + ".haircolor");
  addsaver(cs.dresscolor, s + ".dresscolor");
  addsaver(cs.swordcolor, s + ".swordcolor");
  addsaver(cs.dresscolor2, s + ".dresscolor2");
  addsaver(cs.uicolor, s + ".uicolor");
  }
  
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

#if CAP_CONFIG
void loadcs(FILE *f, charstyle& cs, int xvernum) {
  int gflags, err = 
  fscanf(f, "%d%d%x%x%x%x", &gflags, &vid.language, &cs.skincolor, &cs.haircolor, &cs.swordcolor, &cs.dresscolor);

  if(err) cs.charid = gflags & 15;
  if(err) vid.samegender = (gflags & 16) ? true : false;
  if(cs.charid == 3) hr::ignore(fscanf(f, "%x", &cs.dresscolor2));
  if(xvernum >= 8990) hr::ignore(fscanf(f, "%x", &cs.uicolor));
  }
#endif

void initConfig() {
  
  // basic config
  addsaver(vid.flashtime, "flashtime", 8);
  addsaver(vid.msgleft, "message style", 2);
  addsaver(vid.msglimit, "message limit", 5);
  addsaver(vid.timeformat, "message log time format", 0);
  addsaver(fontscale, "fontscale", 100);

  addsaver(vid.mobilecompasssize, "mobile compass size", ISMOBILE || ISPANDORA ? 30 : 0);
  addsaver(vid.axes, "movement help", 1);
  addsaver(vid.shifttarget, "shift-targetting", 2);
  addsaver(vid.steamscore, "scores to Steam", 1);
  initcs(vid.cs); addsaver(vid.cs, "single");
  addsaver(vid.samegender, "princess choice", false);
  addsaver(vid.language, "language", -1);  
  addsaver(vid.drawmousecircle, "mouse circle", ISMOBILE || ISPANDORA);
  addsaver(vid.revcontrol, "reverse control", false);
  addsaver(musicvolume, "music volume");
  addsaver(effvolume, "sound effect volume");
  addsaverenum(glyphsortorder, "glyph sort order");
  
  // basic graphics
  
  addsaver(vid.usingGL, "usingGL", true);
  addsaver(vid.antialias, "antialias", AA_NOGL | AA_FONT | AA_LINES | AA_LINEWIDTH | AA_VERSION);
  addsaver(vid.linewidth, "linewidth", 1);
  addsaver(vid.scale, "scale", 1);
  addsaver(vid.xposition, "xposition", 0);
  addsaver(vid.yposition, "yposition", 0);
  addsaver(vid.alpha, "projection", 1);
  addsaver(vid.sspeed, "scrollingspeed", 0);
  addsaver(vid.mspeed, "movement speed", 1);
  addsaver(vid.full, "fullscreen", false);
  addsaver(vid.aurastr, "aura strength", ISMOBILE ? 0 : 128);
  addsaver(vid.aurasmoothen, "aura smoothen", 5);
  addsaver(vid.graphglyph, "graphical items/kills", 1);
  addsaver(vid.particles, "extra effects", 1);
  addsaver(vid.framelimit, "frame limit", 75);
  addsaver(vid.xres, "xres");
  addsaver(vid.yres, "yres");
  addsaver(vid.fsize, "font size");
  addsaver(vid.darkhepta, "mark heptagons", false);
  
  // special graphics

  addsaver(vid.ballangle, "ball angle", 20);
  addsaver(vid.yshift, "Y shift", 0);
  addsaver(vid.camera_angle, "camera angle", 0);
  addsaver(vid.ballproj, "ballproj", 1);
  addsaver(vid.monmode, "monster display mode", DEFAULT_MONMODE);
  addsaver(vid.wallmode, "wall display mode", DEFAULT_WALLMODE);

  addsaver(geom3::depth, "3D depth");
  addsaver(geom3::camera, "3D camera level");
  addsaver(geom3::wall_height, "3D wall height");
  addsaver(geom3::rock_wall_ratio, "3D rock-wall ratio");
  addsaver(geom3::human_wall_ratio, "3D human-wall ratio");
  addsaver(geom3::lake_top, "3D lake top");
  addsaver(geom3::lake_bottom, "3D lake bottom");
  addsaver(geom3::tc_depth, "3D TC depth");
  addsaver(geom3::tc_camera, "3D TC camera");
  addsaver(geom3::tc_alpha, "3D TC alpha");
  addsaver(geom3::highdetail, "3D highdetail");
  addsaver(geom3::middetail, "3D middetail");
  addsaver(geom3::gp_autoscale_heights, "3D Goldberg autoscaling");
  
  addsaver(memory_saving_mode, "memory_saving_mode", (ISMOBILE || ISPANDORA || ISWEB) ? 1 : 0);

  addsaver(rug::renderonce, "rug-renderonce");
  addsaver(rug::rendernogl, "rug-rendernogl");
  addsaver(rug::texturesize, "rug-texturesize");
#if CAP_RUG
  addsaver(rug::model_distance, "rug-model-distance");
#endif

  addsaverenum(pmodel, "used model");
  addsaver(polygonal::SI, "polygon sides");
  addsaver(polygonal::STAR, "polygon star factor");
  addsaver(polygonal::deg, "polygonal degree");
  addsaver(conformal::autobandhistory, "include history"); // check!
  addsaver(conformal::lvspeed, "lineview speed");
  addsaver(conformal::extra_line_steps, "lineview extension");
  
  addsaver(polygonal::maxcoef, "polynomial degree");
  for(int i=0; i<polygonal::MSI; i++) {
    addsaver(polygonal::coefr[i], "polynomial "+its(i)+".real");
    addsaver(polygonal::coefi[i], "polynomial "+its(i)+".imag");
    }
  
  addsaver(conformal::bandhalf, "band width");
  addsaver(conformal::bandsegment, "band segment");
  addsaver(conformal::rotation, "conformal rotation");
  addsaver(conformal::do_rotate, "conformal rotation mode", 1);
  addsaver(conformal::model_orientation, "model orientation", 0);
  addsaver(conformal::top_z, "topz", 5);
  addsaver(conformal::model_transition, "model transition", 1);
  addsaver(conformal::halfplane_scale, "halfplane scale", 1);
  addsaver(conformal::autoband, "automatic band");
  addsaver(conformal::autobandhistory, "automatic band history");
  addsaver(conformal::dospiral, "do spiral");
  
  addsaver(vid.backeffects, "background particle effects", (ISMOBILE || ISPANDORA) ? false : true);
  // control
  
  addsaver(vid.joyvalue, "vid.joyvalue", 4800);
  addsaver(vid.joyvalue2, "vid.joyvalue2", 5600);
  addsaver(vid.joypanthreshold, "vid.joypanthreshold", 2500);
  addsaver(vid.joypanspeed, "vid.joypanspeed", ISPANDORA ? 0.0001 : 0);
  addsaver(autojoy, "autojoy");
    
  vid.killreduction = 0;
  
  addsaver(vid.skipstart, "skip the start menu", false);
  addsaver(vid.quickmouse, "quick mouse", !ISPANDORA);
  
  // colors
  
  addsaver(backcolor, "color:background");
  addsaver(forecolor, "color:foreground");
  addsaver(bordcolor, "color:borders");
  addsaver(dialog::dialogcolor, "color:dialog");

  // modes
    
  addsaverenum(geometry, "mode-geometry");
  addsaver(shmup::on, "mode-shmup", false);
  addsaver(hardcore, "mode-hardcore", false);
  addsaver(chaosmode, "mode-chaos");
  addsaver(inv::on, "mode-Orb Strategy");
  addsaverenum(variation, "mode-variation", eVariation::bitruncated);
  addsaver(peace::on, "mode-peace");
  addsaver(peace::otherpuzzles, "mode-peace-submode");
  addsaverenum(specialland, "land for special modes");
  
  addsaver(viewdists, "expansion mode");
  addsaver(backbrightness, "brightness behind sphere");

  addsaver(stereo::ipd, "interpupilar-distance", 0.05);
  addsaver(stereo::lr_eyewidth, "eyewidth-lr", 0.5);
  addsaver(stereo::anaglyph_eyewidth, "eyewidth-anaglyph", 0.1);
  addsaver(stereo::fov, "field-of-vision", 90);
  addsaverenum(stereo::mode, "stereo-mode");
  addsaver(vid.euclid_to_sphere, "euclid to sphere projection", 1.5);
  addsaver(vid.twopoint_param, "twopoint parameter", 1);
  addsaver(vid.stretch, "stretch", 1);
  addsaver(vid.binary_width, "binary-tiling-width", 1);
  
  addsaver(gp::param.first, "goldberg-x", gp::param.first);
  addsaver(gp::param.second, "goldberg-y", gp::param.second);
  
  addsaver(nohud, "no-hud", false);
  addsaver(nofps, "no-fps", false);
  
  addsaver(irr::density, "irregular-density", 2);
  addsaver(irr::cellcount, "irregular-cellcount", 150);
  addsaver(irr::quality, "irregular-quality", .2);
  addsaver(irr::place_attempts, "irregular-place", 10);
  addsaver(irr::rearrange_max_attempts, "irregular-rearrange-max", 50);
  addsaver(irr::rearrange_less, "irregular-rearrangeless", 10);
  
  addsaver(vid.linequality, "line quality", 0);
  
  addsaver(anims::animfile, "animation file format");
  addsaver(anims::period, "animation period");
  addsaver(anims::noframes, "animation frames");
  addsaver(anims::cycle_length, "animation cycle length");
  addsaver(anims::parabolic_length, "animation parabolic length");
  addsaver(anims::rug_angle, "animation rug angle");
  addsaver(anims::circle_radius, "animation circle radius");
  addsaver(anims::circle_spins, "animation circle spins");

#if CAP_TEXTURE  
  addsaver(texture::texture_aura, "texture-aura", false);
#endif

  addsaver(vid.use_smart_range, "smart-range", 0);
  addsaver(vid.smart_range_detail, "smart-range-detail", 8);
  addsaver(vid.cells_drawn_limit, "limit on cells drawn", 10000);

#if CAP_SHMUP  
  shmup::initConfig();
#endif

#if CAP_CONFIG
  for(auto s: savers) s->reset();
#endif
  }

bool inSpecialMode() {
  return chaosmode || !BITRUNCATED || peace::on || 
  #if CAP_TOUR
    tour::on ||
  #endif
    yendor::on || tactic::on || randomPatternsMode ||
    geometry != gNormal || pmodel != mdDisk || vid.alpha != 1 || vid.scale != 1 || 
    rug::rugged || vid.monmode != DEFAULT_MONMODE ||
    vid.wallmode != DEFAULT_WALLMODE;
  }

bool have_current_settings() {
  int modecount = 0;
  if(inv::on) modecount++;
  if(shmup::on) modecount += 10;
#if CAP_TOUR
  if(tour::on) modecount += 10;
#endif
  if(chaosmode) modecount += 10;
  if(!BITRUNCATED) modecount += 10;
  if(peace::on) modecount += 10;
  if(yendor::on) modecount += 10;
  if(tactic::on) modecount += 10;
  if(randomPatternsMode) modecount += 10;
  if(geometry != gNormal) modecount += 10;
  
  if(vid.xposition || vid.yposition || vid.alpha != 1 || vid.scale != 1)
    return true;
  if(pmodel != mdDisk || vid.monmode != DEFAULT_MONMODE || vid.wallmode != DEFAULT_WALLMODE)
    return true;
  if(firstland != laIce || vid.scfg.players != 1 || rug::rugged)
    return true;
  if(modecount > 1)
    return true;
  return false;
  }

void resetModes(char leave) {
  popAllGames();
  if(shmup::on != (leave == rg::shmup)) stop_game_and_switch_mode(rg::shmup);
  if(inv::on != (leave == rg::inv)) stop_game_and_switch_mode(rg::inv);
  if(chaosmode != (leave == rg::chaos)) stop_game_and_switch_mode(rg::chaos);

  if(peace::on != (leave == rg::peace)) stop_game_and_switch_mode(rg::peace);
#if CAP_TOUR
  if(tour::on != (leave == rg::tour)) stop_game_and_switch_mode(rg::tour);
#endif
  if(yendor::on != (leave == rg::yendor)) stop_game_and_switch_mode(rg::yendor);
  if(tactic::on != (leave == rg::tactic)) stop_game_and_switch_mode(rg::tactic);
  if(randomPatternsMode != (leave == rg::randpattern)) stop_game_and_switch_mode(rg::randpattern);
  if(vid.scfg.players != 1) {
    vid.scfg.players = 1; stop_game_and_switch_mode();
    }
  if(firstland != laIce || specialland != laIce) {
    stop_game();
    firstland = laIce; specialland = laIce; stop_game_and_switch_mode();
    }

  set_geometry(gNormal);
  set_variation(eVariation::bitruncated);
  
  pmodel = mdDisk; vid.alpha = 1; vid.scale = 1;
  vid.xposition = vid.yposition = 0;
  #if CAP_RUG
  if(rug::rugged) rug::close();
  #endif

  vid.monmode = DEFAULT_MONMODE;
  vid.wallmode = DEFAULT_WALLMODE;
  start_game();
  }

#if CAP_CONFIG  
void resetConfig() {
  dynamicval<int> rx(vid.xres, 0);
  dynamicval<int> ry(vid.yres, 0);
  dynamicval<int> rf(vid.fsize, 0);
  dynamicval<bool> rfs(vid.full, false);
  for(auto s: savers) 
    if(s->name.substr(0,5) != "mode-")
      s->reset();
  }
#endif

#if CAP_CONFIG
void saveConfig() {
  DEBB(DF_INIT, (debugfile,"save config\n"));
  FILE *f = fopen(conffile, "wt");
  if(!f) {
    addMessage(s0 + "Could not open the config file: " + conffile);
    return;
    }
  
  {
  int pt_depth = 0, pt_camera = 0, pt_alpha = 0;
  using namespace geom3;
  if(tc_depth > tc_camera) pt_depth++;
  if(tc_depth < tc_camera) pt_camera++;
  if(tc_depth > tc_alpha ) pt_depth++;
  if(tc_depth < tc_alpha ) pt_alpha ++;
  if(tc_alpha > tc_camera) pt_alpha++;
  if(tc_alpha < tc_camera) pt_camera++;
  tc_alpha = pt_alpha;
  tc_camera = pt_camera;
  tc_depth = pt_depth;
  }
  
  for(auto s: savers) if(s->dosave())
    fprintf(f, "%s=%s\n", s->name.c_str(), s->save().c_str());
  
  fclose(f);
#if ISMOBILE==0
  addMessage(s0 + "Configuration saved to: " + conffile);
#else
  addMessage(s0 + "Configuration saved");
#endif
  }

void readf(FILE *f, ld& x) {
  double fl = x; 
  hr::ignore(fscanf(f, "%lf", &fl));
  x = fl;
  }

void loadOldConfig(FILE *f) {
  int gl=1, aa=1, bb=1, cc, dd;
  int err;
  float a, b, c, d;
  err=fscanf(f, "%f%f%f%f\n", &a, &b, &c, &d);
  if(err == 4) {
    vid.scale = a; vid.alpha = c; vid.sspeed = d;
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

  aa = geometry; bb = specialland; cc = shmup::on; dd = hardcore;
  err=fscanf(f, "%d%d%d%d", &aa, &bb, &cc, &dd);
  geometry = eGeometry(aa); specialland = eLand(bb); shmup::on = cc; hardcore = dd;

  shmup::loadConfig(f);

  aa = rug::renderonce; bb = rug::rendernogl; dd = chaosmode; 
  int ee = vid.steamscore;
  #if CAP_RUG
  double rs = 2/rug::model_distance;
  #else
  double rs = 0;
  #endif
  err=fscanf(f, "%d%d%d%d%lf%d%d", &aa, &bb, &rug::texturesize, &cc, &rs, &ee, &dd);
  rug::renderonce = aa; rug::rendernogl = bb; 
  chaosmode = dd; vid.steamscore = ee;
  #if CAP_RUG
  rug::model_distance = 2/rs;
  #endif

  aa=conformal::autobandhistory;
  double ps = polygonal::STAR, lv = conformal::lvspeed;
  int pmb = pmodel;
  err=fscanf(f, "%d%d%lf%d%d%lf",
    &pmb, &polygonal::SI, &ps, &polygonal::deg,
    &aa, &lv);
  pmodel = eModel(pmb);
  conformal::autobandhistory = aa; polygonal::STAR = ps; conformal::lvspeed = lv;
  
  aa=conformal::autoband; bb=conformal::autobandhistory; cc=conformal::dospiral;    
  int crot;
  err=fscanf(f, "%d%d%d%d%d%d", 
    &conformal::bandhalf, &conformal::bandsegment, &crot,
    &aa, &bb, &cc);
  conformal::autoband = aa; conformal::autobandhistory = bb; conformal::dospiral = cc;
  conformal::rotation = crot * 90;
  
  err=fscanf(f, "%d", &polygonal::maxcoef);
  if(polygonal::maxcoef < 0) polygonal::maxcoef = 0;
  if(polygonal::maxcoef > polygonal::MSI) polygonal::maxcoef = polygonal::MSI;
  for(int i=0; i<=polygonal::maxcoef; i++) {
    double re=0, im=0;
    err=fscanf(f, "%lf%lf", &re, &im);
    polygonal::coefr[i] = re;
    polygonal::coefi[i] = im;
    }
  
  aa=vid.revcontrol; bb=vid.drawmousecircle;
  d = vid.mspeed;
  int sr;
  err=fscanf(f, "%d%d%d%f%d%d", &aa, &bb, &sr, &d, &effvolume, &vid.particles);
  vid.mspeed = d;
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
  }

map<string, shared_ptr<supersaver> > allconfigs;

void parseline(const string& str) {
  if(str[0] == '#') return;
  for(int i=0; i<isize(str); i++) if(str[i] == '=') {
    string cname = str.substr(0, i);
    if(!allconfigs.count(cname)) {
      printf("Warning: unknown config variable: %s\n", str.c_str());
      return;
      }
    auto sav = allconfigs[cname];
    sav->load(str.substr(i+1));
    return;
    }
  printf("Warning: config line without equality sign: %s\n", str.c_str());
  }

void loadNewConfig(FILE *f) {
  for(auto& c: savers) allconfigs[c->name] = c;
  string rd;
  while(true) {
    int c = fgetc(f);
    if(c == -1) break;
    if(c == 10 || c == 13) {
      if(rd != "") parseline(rd);
      rd = "";
      }
    else rd += c;
    }
  allconfigs.clear();
  }

void edit_sightrange() {
  if(vid.use_smart_range == 0) {
    dialog::editNumber(sightrange_bonus, -5, allowIncreasedSight() ? 3 : 0, 1, 0, XLAT("sight range"), 
      XLAT("Roughly 42% cells are on the edge of your sight range. Reducing "
      "the sight range makes HyperRogue work faster, but also makes "
      "the game effectively harder."));
    dialog::reaction = [] () { 
      doOvergenerate();
      };
    }
  else {
    dialog::editNumber(vid.smart_range_detail, 1, 50, 1, 8, XLAT("minimum visible cell in pixels"), "");
    }  
  dialog::extra_options = [] () {
    dialog::addBoolItem(XLAT("draw range based on distance"), vid.use_smart_range == 0, 'd');
    dialog::add_action([] () { vid.use_smart_range = 0; popScreen(); edit_sightrange(); });
    if(allowIncreasedSight()) {
      dialog::addBoolItem(XLAT("draw based on size in the projection (no generation)"), vid.use_smart_range == 1, 'n');
      dialog::add_action([] () { vid.use_smart_range = 1; popScreen(); edit_sightrange(); });
      }
    if(allowChangeRange()) {
      dialog::addBoolItem(XLAT("draw based on size in the projection (generation)"), vid.use_smart_range == 2, 'g');
      dialog::add_action([] () { vid.use_smart_range = 2; popScreen(); edit_sightrange(); });
      }
    if(vid.use_smart_range == 0 && allowChangeRange()) {
      dialog::addSelItem(XLAT("generation range bonus"), its(genrange_bonus), 'o');
      dialog::add_action([] () { genrange_bonus = sightrange_bonus; doOvergenerate(); });
      dialog::addSelItem(XLAT("game range bonus"), its(gamerange_bonus), 'O');
      dialog::add_action([] () { gamerange_bonus = sightrange_bonus; doOvergenerate(); });
      }      
    if(!allowChangeRange() || !allowIncreasedSight()) {
      dialog::addItem(XLAT("enable the cheat mode for additional options"), 'C');
      dialog::add_action(enable_cheat);
      }
    dialog::addSelItem(XLAT("cells drawn"), its(cells_drawn), 'c');
    dialog::add_action([] () { 
      popScreen();
      dialog::editNumber(vid.cells_drawn_limit, 100, 1000000, log(10), 10000, XLAT("limit on cells drawn"), 
        XLAT("This limit exists to protect the engine from freezing when too many cells would be drawn according to the current options.")
        );
      dialog::scaleLog();
      });
    };
  }

void menuitem_sightrange(char c) {
  if(vid.use_smart_range)
    dialog::addSelItem(XLAT("minimum visible cell in pixels"), fts(vid.smart_range_detail), c);
  else
    dialog::addSelItem(XLAT("sight range"), its(sightrange_bonus), c);
  dialog::add_action(edit_sightrange);
  }
  
void loadConfig() {
 
  DEBB(DF_INIT, (debugfile,"load config\n"));
  vid.xres = 9999; vid.yres = 9999; vid.framelimit = 300;
  FILE *f = fopen(conffile, "rt");
  if(f) {
    int err;
    int fs;
    err=fscanf(f, "%d%d%d%d", &vid.xres, &vid.yres, &fs, &vid.fsize);
    if(!err) 
      loadNewConfig(f);
    else {
      vid.full = fs;
      loadOldConfig(f);
      }
  
    fclose(f);
    DEBB(DF_INIT, (debugfile,"Loaded configuration: %s\n", conffile));
    }

  polygonal::solve();
  precalc();
  }
#endif

void showAllConfig() {
  dialog::addBreak(50);
  dialog::addBack();
#if CAP_CONFIG
  dialog::addItem(XLAT("save the current config"), 's');
  if(getcstat == 's')
    mouseovers = XLAT("Config file: %1", conffile);
#endif
  }

void handleAllConfig(int sym, int uni) {
  if(sym == SDLK_F1 || uni == 'h') gotoHelp(help);

  else if(uni == ' ' || sym == SDLK_ESCAPE) popScreen();
#if CAP_CONFIG
  else if(uni == 's') saveConfig();
#endif  
  }

void showGraphConfig() {
  cmode = vid.xres > vid.yres * 1.4 ? sm::SIDE : sm::MAYDARK;
  gamescreen(0);

  dialog::init(XLAT("graphics configuration"));

  #if CAP_GLORNOT
  dialog::addBoolItem(XLAT("openGL mode"), vid.usingGL, 'o');
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
  else
    dialog::addBreak(100);
  
  dialog::addSelItem(XLAT("line quality"), its(vid.linequality), 'L');

  #if CAP_FRAMELIMIT
  dialog::addSelItem(XLAT("framerate limit"), its(vid.framelimit), 'l');
  if(getcstat == 'l') 
    mouseovers = XLAT("Reduce the framerate limit to conserve CPU energy");
  #endif

#if !ISIOS && !ISWEB
  dialog::addBoolItem(XLAT("fullscreen mode"), (vid.full), 'f');
#endif

  dialog::addSelItem(XLAT("scrolling speed"), fts(vid.sspeed), 'a');
    
  dialog::addSelItem(XLAT("movement animation speed"), fts(vid.mspeed), 'm');

  dialog::addBoolItem(XLAT("extra graphical effects"), (vid.particles), 'u');
  dialog::addBoolItem(XLAT("background particle effects"), (vid.backeffects), 'p');

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

  dialog::addSelItem(XLAT("font scale"), its(fontscale), 'b');

  menuitem_sightrange();
  
  dialog::addSelItem(XLAT("compass size"), its(vid.mobilecompasssize), 'c');

  dialog::addSelItem(XLAT("aura brightness"), its(vid.aurastr), 'z');
  dialog::addSelItem(XLAT("aura smoothening factor"), its(vid.aurasmoothen), 'x');
  
  showAllConfig();
  dialog::display();

  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
  
    if(uni == 'O') uni = 'o', shiftmul = -1;
    
    char xuni = uni | 96;
  
    if((uni >= 32 && uni < 64) || uni == 'L') xuni = uni;
    
    if(xuni == 'u') vid.particles = !vid.particles;
    if(xuni == 'd') vid.graphglyph = (1+vid.graphglyph)%3;
        
    if(xuni == 'j') {
      dialog::editNumber(whatever, -10, 10, 1, 0, XLAT("whatever"), 
        XLAT("Whatever."));
      dialog::reaction = resetGeometry;
      }
  
    if(xuni == 'a') dialog::editNumber(vid.sspeed, -5, 5, 1, 0, 
      XLAT("scrolling speed"),
      XLAT("+5 = center instantly, -5 = do not center the map")
      + "\n\n" +
      XLAT("press Space or Home to center on the PC"));
  
    if(xuni == 'm') dialog::editNumber(vid.mspeed, -5, 5, 1, 0, 
      XLAT("movement animation speed"),
      XLAT("+5 = move instantly"));
  
    if(xuni == 'k') {
      glyphsortorder = eGlyphsortorder((glyphsortorder+6+(shiftmul>0?1:-1)) % gsoMAX);
      }
    
    if(xuni == 'f') switchFullscreen();
  
  #if CAP_GLORNOT
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
#if CAP_SDL
      setvideomode();
#endif
      }
    
    // if(xuni == 'b') vid.antialias ^= AA_LINEWIDTH;
   
    if(xuni == 'w' && vid.usingGL) {
      dialog::editNumber(vid.linewidth, 0, 10, 0.1, 1, XLAT("line width"), "");
      dialog::extra_options = [] () {
        dialog::addBoolItem("finer lines at the boundary", vid.antialias & AA_LINEWIDTH, 'o');
        dialog::add_action([] () { vid.antialias ^= AA_LINEWIDTH; });
        };
      }
    
    if(xuni == 'L') {
      dialog::editNumber(vid.linequality, -3, 5, 1, 1, XLAT("line quality"), 
        XLAT("Higher numbers make the curved lines smoother, but reduce the performance."));
      dialog::reaction = [] () { resetGeometry(); };
      }
  
    if(xuni == 'c') {
      dialog::editNumber(vid.mobilecompasssize, 0, 100, 10, 20, XLAT("compass size"), "");
      // we need to check the moves
      dialog::reaction = checkmove;
      }

  #if CAP_FRAMELIMIT    
    if(xuni == 'l') 
      dialog::editNumber(vid.framelimit, 5, 300, 10, 300, XLAT("framerate limit"), "");
  #endif
      
    if(xuni =='b') {
      dialog::editNumber(fontscale, 25, 400, 10, 100, XLAT("font scale"), "");
      #if !ISMOBILE
        dialog::reaction = [] () { setfsize = true; if(fontscale < 25) fontscale = 25; do_setfsize(); };
      #endif
      }
  
    if(xuni =='p') 
      vid.backeffects = !vid.backeffects;
  
    if(xuni =='z') 
      dialog::editNumber(vid.aurastr, 0, 256, 10, 128, XLAT("aura brightness"), "");
    else if(xuni =='x') 
      dialog::editNumber(vid.aurasmoothen, 1, 180, 1, 5, XLAT("aura smoothening factor"), "");
  
    handleAllConfig(sym, xuni);
    };
  }
  
void switchFullscreen() {
  vid.full = !vid.full;
#if ISANDROID
  addMessage(XLAT("Reenter HyperRogue to apply this setting"));
  ANDROID_SETTINGS
#endif
#if CAP_SDL
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
  ANDROID_SETTINGS;
#if CAP_SDL
  setvideomode();
  if(vid.usingGL) {
    glhr::be_textured(); glhr::be_nontextured();
    }
#endif
  }

void resetConfigMenu();

void showBasicConfig() {
  gamescreen(3);
  const char *axmodes[5] = {"OFF", "auto", "light", "heavy", "arrows"};
  dialog::init(XLAT("basic configuration"));

  if(CAP_TRANS) dialog::addSelItem(XLAT("language"), XLAT("EN"), 'l');
  dialog::addSelItem(XLAT("player character"), numplayers() > 1 ? "" : csname(vid.cs), 'g');
  if(getcstat == 'g') 
    mouseovers = XLAT("Affects looks and grammar");

  if(CAP_AUDIO) {
    dialog::addSelItem(XLAT("background music volume"), its(musicvolume), 'b');
    dialog::addSelItem(XLAT("sound effects volume"), its(effvolume), 'e');
    }

// input:  
  dialog::addSelItem(XLAT("help for keyboard users"), XLAT(axmodes[vid.axes]), 'c');

  dialog::addBoolItem(XLAT("reverse pointer control"), (vid.revcontrol), 'r');
  dialog::addBoolItem(XLAT("draw circle around the target"), (vid.drawmousecircle), 'd');
  
  dialog::addSelItem(XLAT("message flash time"), its(vid.flashtime), 't');
  dialog::addSelItem(XLAT("limit messages shown"), its(vid.msglimit), 'z');
  
  const char* msgstyles[3] = {"centered", "left-aligned", "line-broken"};
  
  dialog::addSelItem(XLAT("message style"), XLAT(msgstyles[vid.msgleft]), 'a');

#if ISMOBILE
  dialog::addBoolItem(XLAT("targetting ranged Orbs long-click only"), (vid.shifttarget&2), 'i');
#else
  dialog::addBoolItem(XLAT("targetting ranged Orbs Shift+click only"), (vid.shifttarget&1), 'i');
#endif

#if ISSTEAM
  dialog::addBoolItem(XLAT("send scores to Steam leaderboards"), (vid.steamscore&1), 'x');
#endif

  dialog::addBoolItem(XLAT("skip the start menu"), vid.skipstart, 'm');
#if !ISMOBILE
  dialog::addBoolItem(XLAT("quick mouse"), vid.quickmouse, 'M');
#endif

  dialog::addBoolItem(XLAT("forget faraway cells"), memory_saving_mode, 'y');

  dialog::addSelItem(XLAT("scrolling by device rotation"), ors::choices[ors::mode], '1');  

  if(CAP_SHMUP && !ISMOBILE)
    dialog::addSelItem(XLAT("configure keys/joysticks"), "", 'p');

#if CAP_CONFIG
  dialog::addItem(XLAT("reset all configuration"), 'R');
#endif
  showAllConfig();
  
  dialog::display();
  
  keyhandler = []   (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
      
    char xuni = uni | 96;
    
    if(uni >= 32 && uni < 64) xuni = uni;
    
    if(xuni == '1') pushScreen(ors::show);

    if(uni == 'M') vid.quickmouse = !vid.quickmouse;
    else if(xuni == 'm') vid.skipstart = !vid.skipstart;

    if(xuni == 'y') memory_saving_mode = !memory_saving_mode;
  
    if(xuni == 'c') { vid.axes += 60 + (shiftmul > 0 ? 1 : -1); vid.axes %= 5; }

    if(CAP_AUDIO && xuni == 'b') {
      dialog::editNumber(musicvolume, 0, 128, 10, 60, XLAT("background music volume"), "");
      }
    if(CAP_AUDIO && xuni == 'e') {
      dialog::editNumber(effvolume, 0, 128, 10, 60, XLAT("sound effects volume"), "");
      }
    
    if(CAP_TRANS && xuni == 'l')
      pushScreen(selectLanguageScreen); 
    
    if(xuni == 'g') pushScreen(showCustomizeChar);
  
#if CAP_SHMUP
    if(xuni == 'p') {
      pushScreen(shmup::showShmupConfig);
      multi::shmupcfg = shmup::on;
      }
#endif
    
    if(uni == 'r') vid.revcontrol = !vid.revcontrol;
    if(xuni == 'd') vid.drawmousecircle = !vid.drawmousecircle;
#if CAP_CONFIG
    if(uni == 'R') pushScreen(resetConfigMenu);
#endif
  
  #if ISSTEAM
    if(xuni == 'x') vid.steamscore = vid.steamscore^1;
  #endif
    if(xuni == 't') 
      dialog::editNumber(vid.flashtime, 0, 64, 1, 8, XLAT("message flash time"),
        XLAT("How long should the messages stay on the screen."));

    if(xuni == 'z') 
      dialog::editNumber(vid.msglimit, 0, 64, 1, 5, XLAT("limit messages shown"),
        XLAT("Maximum number of messages on screen."));
    
    if(xuni == 'i') { vid.shifttarget = vid.shifttarget^3; }
    
    if(xuni == 'a') { vid.msgleft = (1+vid.msgleft) % 3; }
  
    handleAllConfig(sym, xuni);
    };
  }

#if CAP_SDLJOY
void showJoyConfig() {
  gamescreen(4);

  dialog::init(XLAT("joystick configuration"));
  
  dialog::addSelItem(XLAT("first joystick position (movement)"), its(joyx)+","+its(joyy), 0);
  dialog::addSelItem(XLAT("second joystick position (panning)"), its(panjoyx)+","+its(panjoyy), 0);
  
  dialog::addSelItem(XLAT("joystick mode"), XLAT(autojoy ? "automatic" : "manual"), 'p');
  if(getcstat == 'p') {
    if(autojoy) 
      mouseovers = XLAT("joystick mode: automatic (release the joystick to move)");
    if(!autojoy) 
      mouseovers = XLAT("joystick mode: manual (press a button to move)");
    }
    
  dialog::addSelItem(XLAT("first joystick: movement threshold"), its(vid.joyvalue), 'a');
  dialog::addSelItem(XLAT("first joystick: execute movement threshold"), its(vid.joyvalue2), 'b');
  dialog::addSelItem(XLAT("second joystick: pan threshold"), its(vid.joypanthreshold), 'c');
  dialog::addSelItem(XLAT("second joystick: panning speed"), fts(vid.joypanspeed * 1000), 'd');

  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    if(uni == 'p') autojoy = !autojoy;
    else if(uni == 'a') 
      dialog::editNumber(vid.joyvalue, 0, 32768, 100, 4800, XLAT("first joystick: movement threshold"), "");
    else if(uni == 'b') 
      dialog::editNumber(vid.joyvalue2, 0, 32768, 100, 5600, XLAT("first joystick: execute movement threshold"), "");
    else if(uni == 'c')
      dialog::editNumber(vid.joypanthreshold, 0, 32768, 100, 2500, XLAT("second joystick: pan threshold"), "");
    else if(uni == 'd') 
      dialog::editNumber(vid.joypanspeed, 0, 1e-2, 1e-5, 1e-4, XLAT("second joystick: panning speed"), "");
  
    else if(doexiton(sym, uni)) popScreen();
    };
  }
#endif

void projectionDialog() {
  geom3::tc_alpha = ticks;
  dialog::editNumber(vid.alpha, -5, 5, .1, 1,
    XLAT("projection"),
    XLAT("HyperRogue uses the Minkowski hyperboloid model internally. "
    "Klein and Poincaré models can be obtained by perspective, "
    "and the Gans model is obtained by orthogonal projection. "
//  "This parameter specifies the distance from the hyperboloid center "
//  "to the eye. "
    "See also the conformal mode (in the special modes menu) "
    "for more models."));
  dialog::extra_options = [] () {
    dialog::addBreak(100);
    dialog::addSelItem(sphere ? "stereographic" : "Poincaré model", "1", 'p');
    dialog::add_action([] () { *dialog::ne.editwhat = 1; vid.scale = 1; dialog::ne.s = "1"; });
    dialog::addSelItem(sphere ? "gnomonic" : "Klein model", "0", 'k');
    dialog::add_action([] () { *dialog::ne.editwhat = 0; vid.scale = 1; dialog::ne.s = "0"; });
    dialog::addItem(sphere ? "towards orthographic" : "towards Gans model", 'o');
    dialog::add_action([] () { double d = exp(shiftmul/10); vid.alpha *= d; vid.scale *= d; dialog::ne.s = dialog::disp(vid.alpha); });
    };
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
        fts3(actual_wall_height()), fts3(factor_to_projection(geom3::WALL)));

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

void showStereo() {
  cmode = sm::SIDE;
  gamescreen(0);
  using namespace geom3;
  dialog::init(XLAT("stereo vision config"));

  string modenames[4] = { "OFF", "anaglyph", "side-by-side", "ODS" };
  
  dialog::addSelItem(XLAT("stereo mode"), XLAT(modenames[stereo::mode]), 'm');
  dialog::addSelItem(XLAT("pupillary distance"), fts3(stereo::ipd), 'e');
  
  switch(stereo::mode) {
    case stereo::sAnaglyph:
      dialog::addSelItem(XLAT("distance between images"), fts(stereo::anaglyph_eyewidth), 'd');
      break;
    case stereo::sLR:
      dialog::addSelItem(XLAT("distance between images"), fts(stereo::lr_eyewidth), 'd');
      break;
    default:
      dialog::addBreak(100);
      break;
    }

  dialog::addSelItem(XLAT("field of view"), fts(stereo::fov) + "°", 'f');

  dialog::addBack();
  dialog::display();

  keyhandler = [] (int sym, int uni) {
    using namespace geom3;
    dialog::handleNavigation(sym, uni);
    
    string help3 = XLAT(
      "This allows you to view the world of HyperRogue in three dimensions. "
      "Best used with the Hypersian Rug mode. When used in the disk model, "
      "this lets you look at the Minkowski hyperboloid (which means the "
      "depth of terrain features is actually reversed). It also works with non-disk models, "
      "from the conformal menu."
       ) + " " + XLAT(
       "Currently, red-cyan anaglyph glasses and mobile VR googles are supported."
        ) + "\n\n";

    if(uni == 'm')
      { stereo::mode = stereo::eStereo((1 + stereo::mode) % (CAP_ODS ? 4 : 3)); return; }
    
    else if(uni == 'e') 
      dialog::editNumber(stereo::ipd, -10, 10, 0.01, 0, XLAT("pupillary distance"),
        help3 + 
        XLAT("The distance between your eyes in the represented 3D object. This is given in absolute units.")
        );
      
    else if(uni == 'd' && stereo::mode == stereo::sAnaglyph)
      dialog::editNumber(stereo::anaglyph_eyewidth, -1, 1, 0.01, 0, XLAT("distance between images"),
        help3 +
        XLAT("The distance between your eyes. 1 is the width of the screen."));

    else if(uni == 'd' && stereo::mode == stereo::sLR)
      dialog::editNumber(stereo::lr_eyewidth, -1, 1, 0.01, 0, XLAT("distance between images"),
        help3 +
        XLAT("The distance between your eyes. 1 is the width of the screen."));
      
    else if(uni == 'f')
      dialog::editNumber(stereo::fov, 1, 170, 1, 45, "field of view", 
        help3 + XLAT(
          "Horizontal field of view, in angles. "
          "This affects the Hypersian Rug mode (even when stereo is OFF) "
          "and non-disk models.")
          );

    else if(doexiton(sym, uni)) popScreen();
    };
  }

void config_camera_rotation() {
  dialog::editNumber(vid.ballangle, 0, 90, 5, 0, XLAT("camera rotation in 3D models"), 
    "Rotate the camera in 3D models (ball model, hyperboloid, and hemisphere). "
    "Note that hyperboloid and hemisphere models are also available in the "
    "Hypersian Rug surfaces menu, but they are rendered differently there -- "
    "by making a flat picture first, then mapping it to a surface. "
    "This makes the output better in some ways, but 3D effects are lost. "
    "Hypersian Rug model also allows more camera freedom."
    );
  }

void show3D() {
  cmode = sm::SIDE | sm::A3 | sm::MAYDARK;
  gamescreen(0);
  using namespace geom3;
  dialog::init(XLAT("3D configuration"));

  if(vid.use_smart_range == 0) {
    dialog::addSelItem(XLAT("High detail range"), fts(highdetail), 'n');
    dialog::addSelItem(XLAT("Mid detail range"), fts(middetail), 'm');
    dialog::addBreak(50);
    }
  
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
  dialog::addBreak(50);
  dialog::addSelItem(XLAT("model used"), conformal::get_model_name(pmodel), 'M');
  
  if(sphere)
    dialog::addSelItem(XLAT("brightness behind the sphere"), fts3(backbrightness), 'i');

  dialog::addBreak(50);
  if(!(wmspatial || mmspatial))
    dialog::addInfo(XLAT("set 3D monsters or walls in basic config first"));
  else if(invalid != "")
    dialog::addInfo(XLAT("error: "+invalid));
  else
    dialog::addInfo(XLAT("parameters set correctly"));
  dialog::addBreak(50);
  dialog::addItem(XLAT("stereo vision config"), 'e');
  dialog::addBack();
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
    else if(uni == 'w') {
      dialog::editNumber(geom3::wall_height, 0, 1, .1, .3, XLAT("Height of walls"), "");
      dialog::extra_options = [] () {
        dialog::addBoolItem(XLAT("auto-adjust in Goldberg grids"), geom3::gp_autoscale_heights, 'o');
        dialog::add_action([] () {
          geom3::gp_autoscale_heights = !geom3::gp_autoscale_heights;
          buildpolys();
          #if CAP_GL
          resetGL();
          #endif
          });
        };
      }
    else if(uni == 'l') 
      dialog::editNumber(geom3::lake_top, 0, 1, .1, .25, XLAT("Level of water surface"), "");
    else if(uni == 'k') 
      dialog::editNumber(geom3::lake_bottom, 0, 1, .1, .9, XLAT("Level of water bottom"), "");
    else if(uni == 'r') 
      dialog::editNumber(geom3::rock_wall_ratio, 0, 1, .1, .9, XLAT("Rock-III to wall ratio"), "");
    else if(uni == 'h') 
      dialog::editNumber(geom3::human_wall_ratio, 0, 1, .1, .7, XLAT("Human to wall ratio"), "");

    else if(uni == 'e')
      pushScreen(showStereo);
    
    else if(uni == 'y') 
      dialog::editNumber(vid.yshift, 0, 1, .1, 0, XLAT("Y shift"), 
        XLAT("Don't center on the player character.")
        );
    else if(uni == 's') 
      dialog::editNumber(vid.camera_angle, -180, 180, 5, 0, XLAT("camera rotation"), 
        XLAT("Rotate the camera. Can be used to obtain a first person perspective, "
        "or third person perspective when combined with Y shift.")
        );
    else if(uni == 'b') 
      config_camera_rotation();
    else if(uni == 'i') 
      dialog::editNumber(backbrightness, 0, 1, .01, 0.25, XLAT("brightness behind the sphere"), 
        "brightness behind the sphere");
    else if(uni == 'M') 
      pushScreen(conformal::model_menu);  
    else if(doexiton(sym, uni)) 
      popScreen();
    };
  }

void switchcolor(unsigned int& c, unsigned int* cs) {
  dialog::openColorDialog(c, cs);
  }

double cc_footphase;
int lmousex, lmousey;

void showCustomizeChar() {

  cc_footphase += hypot(mousex - lmousex, mousey - lmousey);
  lmousex = mousex; lmousey = mousey;

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
  dialog::addBack();
  dialog::display();
  
  int firsty = dialog::items[0].position / 2;
  int scale = firsty - 2 * vid.fsize;

  initquickqueue();
  transmatrix V = atscreenpos(vid.xres/2, firsty, scale);
  
  double alpha = atan2(mousex - vid.xres/2, mousey - firsty) - M_PI/2;
  drawMonsterType(moPlayer, NULL, V * spin(alpha), 0, cc_footphase / scale);
  quickqueue();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
  
    if(shmup::on || multi::players) shmup::cpid = shmup::cpid_edit % shmup::players;
    charstyle& cs = getcs();
    bool cat = cs.charid >= 4;
    if(uni == 'a') { shmup::cpid_edit++; shmup::cpid_edit %= 60; }
    else if(uni == 'g') {
      cs.charid++;
      if(cs.charid == 2 && !princess::everSaved && !autocheat) cs.charid = 4;
      cs.charid %= 10;
      }
    else if(uni == 'p') vid.samegender = !vid.samegender;
    else if(uni == 's') switchcolor(cs.skincolor, cat ? haircolors : skincolors);
    else if(uni == 'h') switchcolor(cs.haircolor, haircolors);
    else if(uni == 'w') switchcolor(cs.swordcolor, cat ? eyecolors : swordcolors);
    else if(uni == 'd') switchcolor(cs.dresscolor, cat ? haircolors : dresscolors);
    else if(uni == 'f') switchcolor(cs.dresscolor2, dresscolors2);
    else if(uni == 'u') switchcolor(cs.uicolor, eyecolors);
    else if(doexiton(sym, uni)) popScreen();
    };
  }

#if CAP_CONFIG
void resetConfigMenu() {
  dialog::init(XLAT("reset all configuration"));
  dialog::addInfo("Are you sure?");
  dialog::addItem("yes, and delete the config file", 'd');
  dialog::addItem("yes", 'y');
  dialog::addItem("cancel", 'n');
  dialog::addItem("reset the special game modes", 'r');
  dialog::display();
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);

    if(uni == 'd') { 
      resetConfig();
      unlink(conffile);
      popScreen();
      }
    else if(uni == 'y') {
      printf("reseting config\n");
      resetConfig();
      printf("config reset\n");
      popScreen();
      }
    else if(uni == 'r') 
      resetModes();
    else if(uni == 'n' || doexiton(sym, uni)) 
      popScreen();
    
    };
  }
#endif

#if CAP_TRANS
void selectLanguageScreen() {
  gamescreen(4);
  dialog::init("select language"); // intentionally not translated

  int v = vid.language;  
  dynamicval<int> d(vid.language, -1);
  
  for(int i=0; i<NUMLAN-1 || i == v; i++) {
    vid.language = i;
    dialog::addSelItem(XLAT("EN"), its(100 * transcompleteness[i] / transcompleteness[0]) + "%", 'a'+i);
    }
  
  dialog::addBreak(50);
  vid.language = -1;
  dialog::addBoolItem(XLAT("default") + ": " + XLAT("EN"), v == -1, '0');
  dialog::addBack();

  dialog::addBreak(50);

  vid.language = v;
  if(lang() >= 1)
    dialog::addHelp(XLAT("add credits for your translation here"));
  else
    dialog::addHelp(XLAT("original language"));

  if(lang() != 0) {
    string tw = "";
    string s = XLAT("TRANSLATIONWARNING");
    if(s != "" && s != "TRANSLATIONWARNING") tw += s;
    s = XLAT("TRANSLATIONWARNING2");
    if(s != "" && s != "TRANSLATIONWARNING2") { if(tw != "") tw += " "; tw += s; }
    if(tw != "") {
      dialog::addHelp(tw);
      dialog::lastItem().color = 0xFF0000;
      }
    }

  dialog::display();
  
  keyhandler = []   (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    
    if(uni == '0') {
      vid.language = -1;
      ANDROID_SETTINGS;
      }

    else if(uni >= 'a' && uni < 'a'+NUMLAN) {
      vid.language = uni - 'a';
      ANDROID_SETTINGS;
      }
    
    else if(doexiton(sym, uni))
      popScreen();
    };
  }
#endif

#if CAP_COMMANDLINE

int read_config_args() {
  using namespace arg;

  if(argis("-c")) { PHASE(1); shift(); conffile = argcs(); }
// change the configuration from the command line
  else if(argis("-back")) {
    PHASEFROM(2); shift(); backcolor = arghex();
    }
  else if(argis("-ring")) {
    PHASEFROM(2); shift(); ringcolor = arghex();
    }
  else if(argis("-borders")) {
    PHASEFROM(2); shift(); bordcolor = arghex();
    }
  else if(argis("-fore")) {
    PHASEFROM(2); shift(); forecolor = arghex();
    }
  else if(argis("-dialog")) {
    PHASEFROM(2); shift(); dialog::dialogcolor = arghex();
    }
  else if(argis("-aa")) { PHASEFROM(2); shift(); vid.antialias = argi(); }
  else if(argis("-lw")) { PHASEFROM(2); shift(); vid.linewidth = argf(); }
  else if(argis("-wm")) { PHASEFROM(2); shift(); vid.wallmode = argi(); }
  else if(argis("-mm")) { PHASEFROM(2); shift(); vid.monmode = argi(); }

  else if(argis("-noshadow")) { noshadow = true; }
  else if(argis("-bright")) { bright = true; }

// non-configurable options
  else if(argis("-vsync_off")) {
    vsync_off = true;
    if(curphase == 3) setvideomode();
    }
  else if(argis("-nofps")) {
    nofps = true;
    }
  else if(argis("-nohud")) {
    nohud = true;
    }
  else if(argis("-nomenu")) {
    nomenukey = true;
    }
  else if(argis("-nohelp")) {
    nohelp = true;
    }
  else if(argis("-dont_face_pc")) {
    dont_face_pc = true;
    }

  else if(argis("-PM")) { 
    PHASEFROM(2); shift(); pmodel = eModel(argi());
    }
  else if(argis("-ballangle")) { 
    PHASEFROM(2); 
    shift(); vid.ballangle = argf();
    }
  else if(argis("-topz")) { 
    PHASEFROM(2); 
    shift(); conformal::top_z = argf();
    }
  else if(argis("-hp")) { 
    PHASEFROM(2); 
    shift(); conformal::halfplane_scale = argf();
    }
  else if(argis("-mori")) { 
    PHASEFROM(2); 
    shift(); conformal::model_orientation = argf();
    }
  else if(argis("-mtrans")) { 
    PHASEFROM(2); 
    shift(); conformal::model_transition = argf();
    }
  else if(argis("-zoom")) { 
    PHASEFROM(2); shift(); vid.scale = argf();
    }
  else if(argis("-alpha")) { 
    PHASEFROM(2); shift(); vid.alpha = argf();
    }
#if CAP_TRANS
  else if(argis("-lang")) { 
    PHASEFROM(2); shift(); vid.language = argi();
    }
#endif
  else if(argis("-r")) { 
    PHASEFROM(2);
    shift(); 
    int clWidth=0, clHeight=0, clFont=0;
    sscanf(argcs(), "%dx%dx%d", &clWidth, &clHeight, &clFont);
    if(clWidth) vid.xres = clWidth;
    if(clHeight) vid.yres = clHeight;
    if(clFont) vid.fsize = clFont;
    }    
  else if(argis("-els")) {
    shift(); conformal::extra_line_steps = argf();
    }
  else if(argis("-stretch")) {
    PHASEFROM(2); shift(); vid.stretch = argf();
    }
  else if(argis("-msm")) {
    PHASEFROM(2); memory_saving_mode = true;
    }
  else if(argis("-msmoff")) {
    PHASEFROM(2); memory_saving_mode = false;
    }
  TOGGLE('o', vid.usingGL, switchGL())
  TOGGLE('f', vid.full, switchFullscreen())
  else return 1;
  return 0;
  }

// mode changes:

int read_gamemode_args() {
  using namespace arg;

  if(argis("-P")) { 
    PHASE(2); shift(); 
    vid.scfg.players = argi();
    stop_game_and_switch_mode(rg::nothing);
    }
  TOGGLE('C', chaosmode, stop_game_and_switch_mode(rg::chaos))
  TOGGLE('S', shmup::on, stop_game_and_switch_mode(rg::shmup))
  TOGGLE('H', hardcore, switchHardcore())
  TOGGLE('R', randomPatternsMode, stop_game_and_switch_mode(rg::randpattern))
  TOGGLE('i', inv::on, stop_game_and_switch_mode(rg::inv))
  
  else return 1;
  return 0;
  }

auto ah_config = addHook(hooks_args, 0, read_config_args) + addHook(hooks_args, 0, read_gamemode_args);
#endif

}
