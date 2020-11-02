// Hyperbolic Rogue -- configuration
// Copyright (C) 2017-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file config.cpp 
 *  \brief Configuration -- initial settings, saving/loading ini files, menus, etc.
 */

#include "hyper.h"
namespace hr {

#if HDR
enum eCentering { face, edge, vertex };
#endif

EX eCentering centering;

#if HDR
struct supersaver {
  string name;
  virtual string save() = 0;
  virtual void load(const string& s) = 0;
  virtual bool dosave() = 0;
  virtual void reset() = 0;
  virtual ~supersaver() {};
  virtual bool affects(void* v) { return false; }
  virtual void set_default() = 0;
  };

typedef vector<shared_ptr<supersaver>> saverlist;

extern saverlist savers;

#if CAP_CONFIG

template<class T> struct dsaver : supersaver {
  T& val;
  T dft;
  bool dosave() { return val != dft; }
  void reset() { val = dft; }
  dsaver(T& val) : val(val) { }
  bool affects(void* v) { return v == &val; }
  void set_default() { dft = val; }
  };

template<class T> struct saver : dsaver<T> {};

template<class T, class U, class V> void addsaver(T& i, U name, V dft) {
  auto s = make_shared<saver<T>> (i);
  s->dft = dft;
  s->name = name;
  savers.push_back(s);
  }

template<class T> void addsaver(T& i, string name) {
  addsaver(i, name, i);
  }

template<class T> void removesaver(T& val) {
  for(int i=0; i<isize(savers); i++)
    if(savers[i]->affects(&val))
      savers.erase(savers.begin() + i);
  }

template<class T> void set_saver_default(T& val) {
  for(auto sav: savers)
    if(sav->affects(&val))
      sav->set_default();
  }

template<class T> struct saverenum : supersaver {
  T& val;
  T dft;
  bool dosave() { return val != dft; }
  void reset() { val = dft; }
  saverenum<T>(T& v) : val(v) { }
  string save() { return its(int(val)); }
  void load(const string& s) { val = (T) atoi(s.c_str()); }
  virtual bool affects(void* v) { return v == &val; }
  virtual void set_default() { dft = val; }
  };

template<class T, class U> void addsaverenum(T& i, U name, T dft) {
  auto s = make_shared<saverenum<T>> (i);
  s->dft = dft;
  s->name = name;
  savers.push_back(s);
  }

template<class T, class U> void addsaverenum(T& i, U name) {
  addsaverenum(i, name, i);
  }

template<> struct saver<int> : dsaver<int> {
  saver<int>(int& val) : dsaver<int>(val) { }
  string save() { return its(val); }
  void load(const string& s) { val = atoi(s.c_str()); }
  };

template<> struct saver<char> : dsaver<char> {
  saver<char>(char& val) : dsaver<char>(val) { }
  string save() { return its(val); }
  void load(const string& s) { val = atoi(s.c_str()); }
  };

template<> struct saver<bool> : dsaver<bool> {
  saver<bool>(bool& val) : dsaver<bool>(val) { }
  string save() { return val ? "yes" : "no"; }
  void load(const string& s) { val = isize(s) && s[0] == 'y'; }
  };

template<> struct saver<unsigned> : dsaver<unsigned> {
  saver<unsigned>(unsigned& val) : dsaver<unsigned>(val) { }
  string save() { return itsh(val); }
  void load(const string& s) { val = (unsigned) strtoll(s.c_str(), NULL, 16); }
  };

template<> struct saver<string> : dsaver<string> {
  saver<string>(string& val) : dsaver<string>(val) { }
  string save() { return val; }
  void load(const string& s) { val = s; }
  };

template<> struct saver<ld> : dsaver<ld> {
  saver<ld>(ld& val) : dsaver<ld>(val) { }
  string save() { return fts(val, 10); }
  void load(const string& s) { 
    if(s == "0.0000000000e+000") ; // ignore!
    else val = atof(s.c_str()); 
    }
  };
#endif

void addparam(ld& val, const string s);
#endif

#if CAP_CONFIG
void addparam(ld& val, const string s) {
  addsaver(val, s);
  params.insert({s, val});
  }
#else
void addparam(ld& val, const string s) {
  params.insert({s, val});
  }
#endif

EX ld bounded_mine_percentage = 0.1;
EX int bounded_mine_quantity, bounded_mine_max;

EX const char *conffile = "hyperrogue.ini";

/* extra space if more geometries are added */
EX array<ld, gGUARD+64> sightranges;

EX videopar vid;

#define DEFAULT_WALLMODE (ISMOBILE ? 3 : 5)
#define DEFAULT_MONMODE  (ISMOBILE ? 2 : 4)

#if ISANDROID
#define ANDROID_SETTINGS settingsChanged = true;
#else
#define ANDROID_SETTINGS ;
#endif

extern color_t floorcolors[landtypes];

EX charstyle& getcs(int id IS(multi::cpid)) {
  if(multi::players>1 && id >= 0 && id < multi::players)
    return multi::scs[id];
  else
    return vid.cs;
  }

struct charstyle_old {
  int charid;
  color_t skincolor, haircolor, dresscolor, swordcolor, dresscolor2, uicolor;
  bool lefthanded;
  };

EX void hread(hstream& hs, charstyle& cs) {
  // before 0xA61A there was no eyecolor
  if(hs.get_vernum() < 0xA61A) {
    charstyle_old cso;
    hread_raw(hs, cso);
    cs.charid = cso.charid;
    cs.skincolor = cso.skincolor;
    cs.haircolor = cso.haircolor;
    cs.dresscolor = cso.dresscolor;
    cs.swordcolor = cs.eyecolor = cso.swordcolor;
    if(cs.charid < 4) cs.eyecolor = 0;
    cs.dresscolor2 = cso.dresscolor2;
    cs.uicolor = cso.uicolor;
    cs.lefthanded = cso.lefthanded;    
    }
  else hread_raw(hs, cs);
  }

EX void hwrite(hstream& hs, const charstyle& cs) {
  hwrite_raw(hs, cs);
  }

// void hread(hstream& hs, charstyle& cs) { hread_raw(hs, cs); }
// void hwrite(hstream& hs, const charstyle& cs) { hwrite_raw(hs, cs); }

EX string csnameid(int id) {
  if(id == 0) return XLAT("male");
  if(id == 1) return XLAT("female");
  if(id == 2) return XLAT("Prince");
  if(id == 3) return XLAT("Princess");
  if(id == 4 || id == 5) return XLAT("cat");
  if(id == 6 || id == 7) return XLAT("dog");
  if(id == 8 || id == 9) return XLATN("Familiar");
  return XLAT("none");
  }

EX string csname(charstyle& cs) {
  return csnameid(cs.charid);
  }

EX int playergender() {
  return (getcs().charid >= 0 && (getcs().charid&1)) ? GEN_F : GEN_M; 
  }
EX int princessgender() {
  int g = playergender();
  if(vid.samegender) return g;
  return g == GEN_M ? GEN_F : GEN_M;
  }

EX int default_language;

EX int lang() { 
  if(vid.language >= 0)
    return vid.language; 
  return default_language;
  }

EX bool autojoy = true;

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

EX void addsaver(charstyle& cs, string s) {
  addsaver(cs.charid, s + ".charid");
  addsaver(cs.skincolor, s + ".skincolor");
  addsaver(cs.eyecolor, s + ".eyecolor");
  addsaver(cs.haircolor, s + ".haircolor");
  addsaver(cs.dresscolor, s + ".dresscolor");
  addsaver(cs.swordcolor, s + ".swordcolor");
  addsaver(cs.dresscolor2, s + ".dresscolor2");
  addsaver(cs.uicolor, s + ".uicolor");
  addsaver(cs.lefthanded, s + ".lefthanded");
  }
  
// R:239, G:208, B:207 

unsigned int skincolors[]  = { 7, 0xD0D0D0FF, 0xEFD0C9FF, 0xC77A58FF, 0xA58869FF, 0x602010FF, 0xFFDCB1FF, 0xEDE4C8FF };
unsigned int haircolors[]  = { 8, 0x686868FF, 0x8C684AFF, 0xF2E1AEFF, 0xB55239FF, 0xFFFFFFFF, 0x804000FF, 0x502810FF, 0x301800FF };
unsigned int dresscolors[] = { 6, 0xC00000FF, 0x00C000FF, 0x0000C0FF, 0xC0C000FF, 0xC0C0C0FF, 0x202020FF };
unsigned int dresscolors2[] = { 7, 0x8080FFC0, 0x80FF80C0, 0xFF8080C0, 0xFFFF80C0, 0xFF80FFC0, 0x80FFFFC0, 0xFFFFFF80 };
unsigned int swordcolors[] = { 6, 0xC0C0C0FF, 0xFFFFFFFF, 0xFFC0C0FF, 0xC0C0FFFF, 0x808080FF, 0x202020FF };
unsigned int eyecolors[] = { 4, 0x00C000FF, 0x0000C0FF, 0xC00000FF, 0xC0C000FF, 0x804010FF, 0x00C000FF };

EX void initcs(charstyle &cs) {
  cs.charid     = 0;
  cs.skincolor  = 0xD0D0D0FF;
  cs.haircolor  = 0x686868FF;
  cs.dresscolor = 0xC00000FF;
  cs.swordcolor = 0xD0D0D0FF;
  cs.dresscolor2= 0x8080FFC0;
  cs.uicolor    = 0xFF0000FF;
  cs.eyecolor   = 0x603000FF;
  cs.lefthanded = false;
  }

EX void savecolortable(colortable& ct, string name) {
  for(int i=0; i<isize(ct); i++)
    addsaver(ct[i], "color:" + name + ":" + its(i));
  }

EX purehookset hooks_configfile;

EX void initConfig() {
  
  // basic config
  addsaver(vid.flashtime, "flashtime", 8);
  addsaver(vid.msgleft, "message style", 2);
  addsaver(vid.msglimit, "message limit", 5);
  addsaver(vid.timeformat, "message log time format", 0);
  addsaver(fontscale, "fontscale", 100);

  addsaver(vid.mobilecompasssize, "mobile compass size", 0); // ISMOBILE || ISPANDORA ? 30 : 0);
  addsaver(vid.radarsize, "radarsize size", 120);
  addsaver(vid.radarrange, "radarrange", 2.5);
  addsaver(vid.axes, "movement help", 1);
  addsaver(vid.axes3, "movement help3", true);
  addsaver(vid.shifttarget, "shift-targetting", 2);
  addsaver(vid.steamscore, "scores to Steam", 1);
  initcs(vid.cs); addsaver(vid.cs, "single");
  addsaver(vid.samegender, "princess choice", false);
  addsaver(vid.language, "language", -1);  
  addsaver(vid.drawmousecircle, "mouse circle", ISMOBILE || ISPANDORA);
  addsaver(vid.revcontrol, "reverse control", false);
  addsaver(musicvolume, "music volume");
  #if CAP_SDLAUDIO
  addsaver(music_out_of_focus, "music out of focus", false);
  #endif
  addsaver(effvolume, "sound effect volume");
  addsaverenum(glyphsortorder, "glyph sort order");
  
  // basic graphics
  
  addsaver(vid.usingGL, "usingGL", true);
  addsaver(vid.antialias, "antialias", AA_NOGL | AA_FONT | (ISWEB ? AA_MULTI : AA_LINES) | AA_LINEWIDTH | AA_VERSION);
  addsaver(vid.linewidth, "linewidth", 1);
  addsaver(precise_width, "precisewidth", .5);
  addsaver(perfect_linewidth, "perfect_linewidth", 1);
  addsaver(linepatterns::width, "pattern-linewidth", 1);
  addsaver(fat_edges, "fat-edges");
  addsaver(pconf.scale, "scale", 1);
  addsaver(pconf.xposition, "xposition", 0);
  addsaver(pconf.yposition, "yposition", 0);
  addsaver(pconf.alpha, "projection", 1);
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
  
  for(auto& lp: linepatterns::patterns) {
    addsaver(lp->color, "lpcolor-" + lp->lpname);
    addsaver(lp->multiplier, "lpwidth-" + lp->lpname);
    }
  
  // special graphics

  addsaver(pconf.ballangle, "ball angle", 20);
  addsaver(vid.yshift, "Y shift", 0);
  addsaver(vid.use_wall_radar, "wallradar", true);
  addsaver(vid.fixed_facing, "fixed facing", 0);
  addsaver(vid.fixed_facing_dir, "fixed facing dir", 90);
  addsaver(vid.fixed_yz, "fixed YZ", true);
  addsaver(pconf.camera_angle, "camera angle", 0);
  addsaver(pconf.ballproj, "ballproj", 1);
  addsaver(vid.monmode, "monster display mode", DEFAULT_MONMODE);
  addsaver(vid.wallmode, "wall display mode", DEFAULT_WALLMODE);

  addsaver(vid.depth, "3D depth", 1);
  addsaver(vid.camera, "3D camera level", 1);
  addsaver(vid.wall_height, "3D wall height", .3);
  addsaver(vid.rock_wall_ratio, "3D rock-wall ratio", .9);
  addsaver(vid.human_wall_ratio, "3D human-wall ratio", .7);
  addsaver(vid.lake_top, "3D lake top", .25);
  addsaver(vid.lake_bottom, "3D lake bottom", .9);
  addsaver(vid.tc_depth, "3D TC depth", 1);
  addsaver(vid.tc_camera, "3D TC camera", 2);
  addsaver(vid.tc_alpha, "3D TC alpha", 3);
  addsaver(vid.highdetail, "3D highdetail", 8);
  addsaver(vid.middetail, "3D middetail", 8);
  addsaver(vid.gp_autoscale_heights, "3D Goldberg autoscaling", true);
  addsaver(vid.always3, "3D always", false);
  
  addsaver(vid.eye, "eyelevel", 0);
  addsaver(vid.auto_eye, "auto-eyelevel", false);
  
  addsaver(memory_saving_mode, "memory_saving_mode", (ISMOBILE || ISPANDORA || ISWEB) ? 1 : 0);
  addsaver(reserve_limit, "memory_reserve", 128);
  addsaver(show_memory_warning, "show_memory_warning");

  auto& rconf = vid.rug_config;
  addsaverenum(rconf.model, "rug-projection", mdEquidistant);
  addsaver(rconf.scale, "rug-projection-scale", 1);
  addsaver(rconf.alpha, "rug-projection-alpha", 1);
  addsaver(rconf.clip_min, "rug-projection-clip-min", -100);
  addsaver(rconf.clip_max, "rug-projection-clip-max", +10);
  addsaver(rconf.stretch, "rug-projection-stretch", 1);
  addsaver(rconf.halfplane_scale, "rug-projection-halfplane scale", 1);
  addsaver(rconf.collignon_parameter, "rug-collignon-parameter", 1);
  addsaver(rconf.collignon_reflected, "rug-collignon-reflect", false);
  addsaver(rconf.euclid_to_sphere, "rug-euclid to sphere projection", 1.5);
  addsaver(rconf.twopoint_param, "rug-twopoint parameter", 1);
  addsaver(rconf.fisheye_param, "rug-fisheye parameter", 1);
  addsaver(rconf.model_transition, "rug-model transition", 1);
  addsaver(rug::renderonce, "rug-renderonce");
  addsaver(rug::rendernogl, "rug-rendernogl");
  addsaver(rug::texturesize, "rug-texturesize");
#if CAP_RUG
  addsaver(rug::model_distance, "rug-model-distance");
#endif

  addsaverenum(pmodel, "used model", mdDisk);
  addsaver(polygonal::SI, "polygon sides");
  addsaver(polygonal::STAR, "polygon star factor");
  addsaver(polygonal::deg, "polygonal degree");
  addsaver(history::autobandhistory, "include history"); // check!
  addsaver(history::lvspeed, "lineview speed");
  addsaver(history::extra_line_steps, "lineview extension");
  
  addsaver(polygonal::maxcoef, "polynomial degree");
  for(int i=0; i<polygonal::MSI; i++) {
    addsaver(polygonal::coefr[i], "polynomial "+its(i)+".real");
    addsaver(polygonal::coefi[i], "polynomial "+its(i)+".imag");
    }
  
  addsaver(history::bandhalf, "band width");
  addsaver(history::bandsegment, "band segment");
  addsaver(models::rotation, "conformal rotation");
  addsaver(models::rotation_xz, "conformal rotation_xz");
  addsaver(models::rotation_xy2, "conformal rotation_2");
  addsaver(models::do_rotate, "conformal rotation mode", 1);
  addsaver(pconf.model_orientation, "model orientation", 0);
  addsaver(pconf.model_orientation_yz, "model orientation-yz", 0);
  addsaver(pconf.top_z, "topz", 5);
  addsaver(pconf.model_transition, "model transition", 1);
  addsaver(pconf.halfplane_scale, "halfplane scale", 1);
  addsaver(pconf.rotational_nil, "rotnil", 1);
  addsaver(history::autoband, "automatic band");
  addsaver(history::autobandhistory, "automatic band history");
  addsaver(history::dospiral, "do spiral");
  addsaver(pconf.clip_min, "clip-min", -1);
  addsaver(pconf.clip_max, "clip-max", +1);
  
  addsaver(vid.backeffects, "background particle effects", (ISMOBILE || ISPANDORA) ? false : true);
  // control
  
  addsaver(vid.joyvalue, "vid.joyvalue", 4800);
  addsaver(vid.joyvalue2, "vid.joyvalue2", 5600);
  addsaver(vid.joysmooth, "vid.joysmooth", 200);
  addsaver(vid.joypanthreshold, "vid.joypanthreshold", 2500);
  addsaver(vid.joypanspeed, "vid.joypanspeed", ISPANDORA ? 0.0001 : 0);
  addsaver(autojoy, "autojoy");
    
  vid.killreduction = 0;
  
  addsaver(vid.skipstart, "skip the start menu", false);
  addsaver(vid.quickmouse, "quick mouse", !ISPANDORA);
  
  // colors

  addsaver(crosshair_size, "size:crosshair");
  addsaver(crosshair_color, "color:crosshair");
  
  addsaver(backcolor, "color:background");
  addsaver(forecolor, "color:foreground");
  addsaver(bordcolor, "color:borders");
  addsaver(ringcolor, "color:ring");
  addsaver(vid.multiplier_ring, "mult:ring", 1);
  addsaver(modelcolor, "color:model");
  addsaver(periodcolor, "color:period");
  addsaver(stdgridcolor, "color:stdgrid"); 
  addsaver(vid.multiplier_grid, "mult:grid", 1);
  addsaver(dialog::dialogcolor, "color:dialog");
  for(auto& p: colortables)
    savecolortable(p.second, s0+"canvas"+p.first);
  savecolortable(distcolors, "distance");
  savecolortable(minecolors, "mines");
  #if CAP_COMPLEX2
  savecolortable(brownian::colors, "color:brown");
  #endif
  
  for(int i=0; i<motypes; i++)
    addsaver(minf[i].color, "color:monster:" + its(i));
  for(int i=0; i<ittypes; i++)
    addsaver(iinf[i].color, "color:item:" + its(i));
  for(int i=0; i<landtypes; i++)
    addsaver(floorcolors[i], "color:land:" + its(i));
  for(int i=0; i<walltypes; i++)
    addsaver(winf[i].color, "color:wall:" + its(i));

  // modes
    
  addsaverenum(geometry, "mode-geometry");
  addsaver(shmup::on, "mode-shmup", false);
  addsaver(hardcore, "mode-hardcore", false);
  addsaver(chaosmode, "mode-chaos");
  #if CAP_INV
  addsaver(inv::on, "mode-Orb Strategy");
  #endif
  addsaverenum(variation, "mode-variation", eVariation::bitruncated);
  addsaver(peace::on, "mode-peace");
  addsaver(peace::otherpuzzles, "mode-peace-submode");
  addsaverenum(specialland, "land for special modes");
  
  addsaver(viewdists, "expansion mode");
  addsaver(backbrightness, "brightness behind sphere");

  addsaver(vid.ipd, "interpupilar-distance", 0.05);
  addsaver(vid.lr_eyewidth, "eyewidth-lr", 0.5);
  addsaver(vid.anaglyph_eyewidth, "eyewidth-anaglyph", 0.1);
  addsaver(vid.fov, "field-of-vision", 90);
  addsaver(vid.desaturate, "desaturate", 0);
  addsaverenum(vid.stereo_mode, "stereo-mode");
  addsaver(pconf.euclid_to_sphere, "euclid to sphere projection", 1.5);
  addsaver(pconf.twopoint_param, "twopoint parameter", 1);
  addsaver(pconf.fisheye_param, "fisheye parameter", 1);
  addsaver(pconf.stretch, "stretch", 1);
  addsaver(vid.binary_width, "binary-tiling-width", 1);
  addsaver(pconf.collignon_parameter, "collignon-parameter", 1);
  addsaver(pconf.collignon_reflected, "collignon-reflect", false);
  addsaver(pconf.show_hyperboloid_flat, "hyperboloid-flat", true);

  addsaver(pconf.aitoff_parameter, "aitoff-parameter");
  addsaver(pconf.miller_parameter, "miller-parameter");
  addsaver(pconf.loximuthal_parameter, "loximuthal_parameter");
  addsaver(pconf.winkel_parameter, "winkel_parameter");

  addsaver(vid.plevel_factor, "plevel_factor", 0.7);

  addsaver(vid.creature_scale, "3d-creaturescale", 1);
  addsaver(vid.height_width, "3d-heightwidth", 1.5);

  #if CAP_GP
  addsaver(gp::param.first, "goldberg-x", gp::param.first);
  addsaver(gp::param.second, "goldberg-y", gp::param.second);
  #endif
  
  addsaver(nohud, "no-hud", false);
  addsaver(nofps, "no-fps", false);
  
  #if CAP_IRR
  addsaver(irr::density, "irregular-density", 2);
  addsaver(irr::cellcount, "irregular-cellcount", 150);
  addsaver(irr::quality, "irregular-quality", .2);
  addsaver(irr::place_attempts, "irregular-place", 10);
  addsaver(irr::rearrange_max_attempts, "irregular-rearrange-max", 50);
  addsaver(irr::rearrange_less, "irregular-rearrangeless", 10);
  #endif
  
  addsaver(vid.linequality, "line quality", 0);
  
  #if CAP_FILES && CAP_SHOT && CAP_ANIMATIONS
  addsaver(anims::animfile, "animation file format");
  #endif
  #if CAP_ANIMATIONS
  addsaver(anims::period, "animation period");
  addsaver(anims::noframes, "animation frames");
  addsaver(anims::cycle_length, "animation cycle length");
  addsaver(anims::parabolic_length, "animation parabolic length");
  addsaver(anims::rug_angle, "animation rug angle");
  addsaver(anims::circle_radius, "animation circle radius");
  addsaver(anims::circle_spins, "animation circle spins");
  #endif
  
  #if CAP_CRYSTAL
  addsaver(crystal::compass_probability, "compass-probability");
  addsaver(crystal::view_coordinates, "crystal-coordinates");
  #endif
  
  #if CAP_SHOT
  addsaver(shot::shotx, "shotx");
  addsaver(shot::shoty, "shoty");
  addsaverenum(shot::format, "shotsvg");
  addsaver(shot::transparent, "shottransparent");
  addsaver(shot::gamma, "shotgamma");
  addsaver(shot::caption, "shotcaption");
  addsaver(shot::fade, "shotfade");
  #endif

#if CAP_TEXTURE  
  addsaver(texture::texture_aura, "texture-aura", false);
#endif

  addsaver(vid.use_smart_range, "smart-range", 0);
  addsaver(vid.smart_range_detail, "smart-range-detail", 8);
  addsaver(vid.smart_range_detail_3, "smart-range-detail", 30);
  addsaver(vid.smart_area_based, "smart-area-based", false);
  addsaver(vid.cells_drawn_limit, "limit on cells drawn", 10000);
  addsaver(vid.cells_generated_limit, "limit on cells generated", 250);
  
  #if CAP_SOLV
  addsaver(sn::solrange_xy, "solrange-xy");
  addsaver(sn::solrange_z, "solrange-z");
  #endif
  addsaver(slr::steps, "slr-steps");
  addsaver(slr::range_xy, "slr-range-xy");
  
  addsaver(pconf.skiprope, "mobius", 0);
  
  addsaver(pconf.formula, "formula");
  addsaverenum(pconf.basic_model, "basic model");
  addsaver(pconf.use_atan, "use_atan");
  
  #if CAP_ARCM
  addsaver(arcm::current.symbol, "arcm-symbol", "4^5");
  #endif
  addsaverenum(hybrid::underlying, "product-underlying");
  
  for(int i=0; i<isize(ginf); i++) {
    if(ginf[i].flags & qELLIPTIC)
      sightranges[i] = M_PI;
    else if(ginf[i].cclass == gcSphere)
      sightranges[i] = 2 * M_PI;
    else if(ginf[i].cclass == gcEuclid)
      sightranges[i] = 10;
    else if(ginf[i].cclass == gcSL2)
      sightranges[i] = 4.5;
    else if(ginf[i].cclass == gcHyperbolic && ginf[i].g.gameplay_dimension == 2)
      sightranges[i] = 4.5;
    else
      sightranges[i] = 5;
    sightranges[gArchimedean] = 10;
    if(i < gBinary3) addsaver(sightranges[i], "sight-g" + its(i));
    }
  
  ld bonus = 0;
  ld emul = 1;

  addsaver(sightranges[gBinary3], "sight-binary3", 3.1 + bonus);
  addsaver(sightranges[gCubeTiling], "sight-cubes", 10);
  addsaver(sightranges[gCell120], "sight-120cell", 2 * M_PI);
  addsaver(sightranges[gECell120], "sight-120cell-elliptic", M_PI);
  addsaver(sightranges[gRhombic3], "sight-rhombic", 10.5 * emul);
  addsaver(sightranges[gBitrunc3], "sight-bitrunc", 12 * emul);
  addsaver(sightranges[gSpace534], "sight-534", 4 + bonus);
  addsaver(sightranges[gSpace435], "sight-435", 3.8 + bonus);

  addsaver(sightranges[gCell5], "sight-5cell", 2 * M_PI);
  addsaver(sightranges[gCell8], "sight-8cell", 2 * M_PI);
  addsaver(sightranges[gECell8], "sight-8cell-elliptic", M_PI);
  addsaver(sightranges[gCell16], "sight-16cell", 2 * M_PI);
  addsaver(sightranges[gECell16], "sight-16cell-elliptic", M_PI);
  addsaver(sightranges[gCell24], "sight-24cell", 2 * M_PI);
  addsaver(sightranges[gECell24], "sight-24cell-elliptic", M_PI);
  addsaver(sightranges[gCell600], "sight-600cell", 2 * M_PI);
  addsaver(sightranges[gECell600], "sight-600cell-elliptic", M_PI);
  addsaver(sightranges[gHoroTris], "sight-horotris", 2.9 + bonus);
  addsaver(sightranges[gHoroRec], "sight-hororec", 2.2 + bonus);
  addsaver(sightranges[gHoroHex], "sight-horohex", 2.75 + bonus);

  addsaver(sightranges[gKiteDart3], "sight-kd3", 2.25 + bonus);
  
  addsaver(sightranges[gField435], "sight-field435", 4 + bonus);
  addsaver(sightranges[gField534], "sight-field534", 3.8 + bonus);
  addsaver(sightranges[gSol], "sight-sol");
  addsaver(sightranges[gNil], "sight-nil", 6.5 + bonus);
  addsaver(sightranges[gNIH], "sight-nih");
  addsaver(sightranges[gSolN], "sight-solnih");

  addsaver(sightranges[gCrystal344], "sight-crystal344", 2.5); /* assume raycasting */
  addsaver(sightranges[gSpace344], "sight-344", 4.5);
  addsaver(sightranges[gSpace336], "sight-336", 4);

  addsaver(vid.sloppy_3d, "sloppy3d", true);

  addsaver(vid.texture_step, "wall-quality", 4);
  
  addsaver(smooth_scrolling, "smooth-scrolling", false);
  addsaver(mouseaim_sensitivity, "mouseaim_sensitivity", 0.01);

  addsaver(vid.consider_shader_projection, "shader-projection", true);
  
#if CAP_RACING
  addsaver(racing::race_advance, "race_advance");
  addsaver(racing::race_angle, "race_angle");
  addsaver(racing::ghosts_to_show, "race_ghosts_to_show");
  addsaver(racing::ghosts_to_save, "race_ghosts_to_save");
  addsaver(racing::guiding, "race_guiding");
  addsaver(racing::player_relative, "race_player_relative");
  addsaver(racing::standard_centering, "race_standard_centering");
#endif

  addsaver(tortoise::shading_enabled, "tortoise_shading", true);

  addsaver(bounded_mine_percentage, "bounded_mine_percentage");

  addsaver(nisot::geodesic_movement, "solv_geodesic_movement", true);

  addsaver(s2xe::qrings, "s2xe-rings");
  addsaver(rots::underlying_scale, "rots-underlying-scale");
  
  addsaver(vid.bubbles_special, "bubbles-special", 1);
  addsaver(vid.bubbles_threshold, "bubbles-special", 1);
  addsaver(vid.bubbles_all, "bubbles-special", 0);

#if CAP_SHMUP  
  multi::initConfig();
#endif

  addsaver(asonov::period_xy, "asonov:period_xy");
  addsaver(asonov::period_z, "asonov:period_z");
  addsaver(nilv::nilperiod[0], "nilperiod_x");
  addsaver(nilv::nilperiod[1], "nilperiod_y");
  addsaver(nilv::nilperiod[2], "nilperiod_z");
  
  addsaverenum(neon_mode, "neon_mode");
  addsaverenum(neon_nofill, "neon_nofill");
  addsaver(noshadow, "noshadow");
  addsaver(bright, "bright");
  addsaver(cblind, "cblind");
  
  addsaver(berger_limit, "berger_limit");
  
  addsaverenum(centering, "centering");
  
  addsaver(camera_speed, "camera-speed", 1);
  addsaver(camera_rot_speed, "camera-rot-speed", 1);

  addsaver(panini_alpha, "panini_alpha", 0);

  callhooks(hooks_configfile);

#if CAP_CONFIG
  for(auto s: savers) s->reset();
#endif
  }

EX bool inSpecialMode() {
  return chaosmode || !BITRUNCATED || peace::on || 
  #if CAP_TOUR
    tour::on ||
  #endif
    yendor::on || tactic::on || randomPatternsMode ||
    geometry != gNormal || pmodel != mdDisk || pconf.alpha != 1 || pconf.scale != 1 || 
    rug::rugged || vid.monmode != DEFAULT_MONMODE ||
    vid.wallmode != DEFAULT_WALLMODE;
  }

EX bool have_current_settings() {
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

  if(modecount > 1)
    return true;
  
  return false;
  }

EX bool have_current_graph_settings() {
  if(pconf.xposition || pconf.yposition || pconf.alpha != 1 || pconf.scale != 1)
    return true;
  if(pmodel != mdDisk || vid.monmode != DEFAULT_MONMODE || vid.wallmode != DEFAULT_WALLMODE)
    return true;
  if(firstland != laIce || multi::players != 1 || rug::rugged)
    return true;
  
  return false;
  }

EX void reset_graph_settings() {
  pmodel = mdDisk; pconf.alpha = 1; pconf.scale = 1;
  pconf.xposition = pconf.yposition = 0;
  #if CAP_RUG
  if(rug::rugged) rug::close();
  #endif

  vid.monmode = DEFAULT_MONMODE;
  vid.wallmode = DEFAULT_WALLMODE;
  }

EX void resetModes(char leave IS('c')) {
  while(game_active || gamestack::pushed()) {
    if(game_active) stop_game();
    if(gamestack::pushed()) gamestack::pop();
    }
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
  if(multi::players != 1) {
    stop_game_and_switch_mode(); multi::players = 1;
    }
  if(firstland != laIce || specialland != laIce) {
    stop_game();
    firstland = laIce; specialland = laIce; stop_game_and_switch_mode();
    }

  set_geometry(gNormal);
  set_variation(leave == rg::heptagons ? eVariation::pure : eVariation::bitruncated);
  
  start_game();
  }

#if CAP_CONFIG  
EX void resetConfig() {
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
EX void saveConfig() {
  DEBB(DF_INIT, ("save config\n"));
  FILE *f = fopen(conffile, "wt");
  if(!f) {
    addMessage(s0 + "Could not open the config file: " + conffile);
    return;
    }
  
  {
  int pt_depth = 0, pt_camera = 0, pt_alpha = 0;
  if(vid.tc_depth > vid.tc_camera) pt_depth++;
  if(vid.tc_depth < vid.tc_camera) pt_camera++;
  if(vid.tc_depth > vid.tc_alpha ) pt_depth++;
  if(vid.tc_depth < vid.tc_alpha ) pt_alpha ++;
  if(vid.tc_alpha > vid.tc_camera) pt_alpha++;
  if(vid.tc_alpha < vid.tc_camera) pt_camera++;
  vid.tc_alpha = pt_alpha;
  vid.tc_camera = pt_camera;
  vid.tc_depth = pt_depth;
  }
  
  for(auto s: savers) if(s->dosave())
    fprintf(f, "%s=%s\n", s->name.c_str(), s->save().c_str());
  
  fclose(f);
#if !ISMOBILE
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

map<string, shared_ptr<supersaver> > allconfigs;

EX void parseline(const string& str) {
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

EX void loadNewConfig(FILE *f) {
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

EX void loadConfig() {
 
  DEBB(DF_INIT, ("load config"));
  vid.xres = 9999; vid.yres = 9999; vid.framelimit = 300;
  FILE *f = fopen(conffile, "rt");
  if(f) {
    int err;
    int fs;
    err=fscanf(f, "%d%d%d%d", &vid.xres, &vid.yres, &fs, &vid.fsize);
    if(err != 4) 
      loadNewConfig(f);
    else {
      vid.full = fs;
      #if CAP_LEGACY
      loadOldConfig(f);
      #endif
      }
  
    fclose(f);
    DEBB(DF_INIT, ("Loaded configuration: %s\n", conffile));
    }

  geom3::apply_always3();
  polygonal::solve();
  check_cgi();
  cgi.prepare_basics();
  }
#endif

EX void add_cells_drawn(char c IS('C')) {
  dialog::addSelItem(XLAT("cells drawn"), (noclipped ? its(cells_drawn) + " (" + its(noclipped) + ")" : its(cells_drawn)) + " / " + its(vid.cells_drawn_limit), c);
  dialog::add_action([] () { 
    popScreen();
    dialog::editNumber(vid.cells_drawn_limit, 100, 1000000, log(10), 10000, XLAT("limit on cells drawn"), 
      XLAT("This limit exists to protect the engine from freezing when too many cells would be drawn according to the current options.")
      );
    dialog::scaleLog();
    });
  if(WDIM == 3 || vid.use_smart_range == 2) {
    dialog::addSelItem(XLAT("limit generated cells per frame"), its(vid.cells_generated_limit), 'L');
    dialog::add_action([] () { 
      dialog::editNumber(vid.cells_generated_limit, 1, 1000, log(10), 25, XLAT("limit generated cells per frame"), 
        XLAT("In the 3D mode, lowering this value may help if the game lags while exploring new areas.")
        );
      });
    }
  }

string solhelp() {
#if CAP_SOLV
  return XLAT(
    "Solv (aka Sol) is a 3D space where directions work in different ways. It is described by the following metric:\n"
    "ds² = (eᶻdx)² + (e⁻ᶻdy)² + dz²\n\n"
    "You are currently displaying Solv in the perspective projection based on native geodesics. You can control how "
    "the fog effects depends on the geodesic distance, and how far object in X/Y/Z coordinates are rendered."
    );
#else
  return "";
#endif
  }

EX void edit_sightrange() {
  #if CAP_RUG
  USING_NATIVE_GEOMETRY_IN_RUG;
  #endif
  if(vid.use_smart_range) {
    ld& det = WDIM == 2 ? vid.smart_range_detail : vid.smart_range_detail_3;
    dialog::editNumber(det, 1, 50, 1, WDIM == 2 ? 8 : 30, XLAT("minimum visible cell in pixels"), "");
    }
  else if(WDIM == 3) {
    dialog::editNumber(sightranges[geometry], 0, 2 * M_PI, 0.5, M_PI, XLAT("3D sight range"),
      (pmodel == mdGeodesic && sol) ? solhelp() : XLAT(
        "Sight range for 3D geometries is specified in the absolute units. This value also affects the fog effect.\n\n"
        "In spherical geometries, the sight range of 2? will let you see things behind you as if they were in front of you, "
        "and the sight range of ? (or more) will let you see things on the antipodal point just as if they were close to you.\n\n"
        "In hyperbolic geometries, the number of cells to render depends exponentially on the sight range. More cells to drawn "
        "reduces the performance.\n\n"
        "Sight range affects the gameplay, and monsters act iff they are visible. Monster generation takes this into account."
        )
      );
    }
  else {
    dialog::editNumber(sightrange_bonus, -5, allowIncreasedSight() ? 3 : 0, 1, 0, XLAT("sight range"), 
      XLAT("Roughly 42% cells are on the edge of your sight range. Reducing "
      "the sight range makes HyperRogue work faster, but also makes "
      "the game effectively harder."));
    dialog::reaction = doOvergenerate;
    dialog::bound_low(1-getDistLimit());
    dialog::bound_up(allowIncreasedSight() ? euclid ? 99 : gp::dist_2() * 5 : 0);
    }
  dialog::extra_options = [] () {
    if(pmodel == mdGeodesic && sol) {
      #if CAP_SOLV
      dialog::addSelItem(XLAT("fog effect"), fts(sightranges[geometry]), 'R');
      dialog::add_action([] {
        auto xo = dialog::extra_options;
        dialog::editNumber(sightranges[geometry], 0, 10, 0.5, M_PI, solhelp(), "");
        dialog::extra_options = xo; popScreen();
        });
      dialog::addSelItem(XLAT("max difference in X/Y coordinates"), fts(sn::solrange_xy), 'X');
      dialog::add_action([] {
        auto xo = dialog::extra_options;
        dialog::editNumber(sn::solrange_xy, 0.01, 200, 0.1, 50, XLAT("max difference in X/Y coordinates"), solhelp()), dialog::scaleLog();
        dialog::extra_options = xo; popScreen();
        });
      dialog::addSelItem(XLAT("max difference in Z coordinate"), fts(sn::solrange_z), 'Z');
      dialog::add_action([] {
        auto xo = dialog::extra_options;
        dialog::editNumber(sn::solrange_z, 0, 20, 0.1, 6, XLAT("max difference in Z coordinates"), solhelp());
        dialog::extra_options = xo; popScreen();
        });
      #endif
      }
    else if(pmodel == mdGeodesic && sl2) {
      dialog::addSelItem(XLAT("fog effect"), fts(sightranges[geometry]), 'R');
      dialog::add_action([] {
        auto xo = dialog::extra_options;
        dialog::editNumber(sightranges[geometry], 0, 10, 0.5, M_PI, "", "");
        dialog::extra_options = xo; popScreen();
        });
      dialog::addSelItem(XLAT("max difference in X/Y coordinates"), fts(slr::range_xy), 'X');
      dialog::add_action([] {
        auto xo = dialog::extra_options;
        dialog::editNumber(slr::range_xy, 0, 10, 0.5, 4, XLAT("max difference in X/Y coordinates"), "");
        dialog::extra_options = xo; popScreen();
        });
      dialog::addSelItem(XLAT("steps"), its(slr::steps), 'Z');
      dialog::add_action([] {
        auto xo = dialog::extra_options;
        dialog::editNumber(slr::steps, 0, 50, 1, 10, "", "");
        dialog::extra_options = xo; popScreen();
        });
      }
    else {
      dialog::addBoolItem(XLAT("draw range based on distance"), vid.use_smart_range == 0, 'D');
      dialog::add_action([] () { vid.use_smart_range = 0; popScreen(); edit_sightrange(); });
      if(WDIM == 2 && allowIncreasedSight()) {
        dialog::addBoolItem(XLAT("draw based on size in the projection (no generation)"), vid.use_smart_range == 1, 'N');
        dialog::add_action([] () { vid.use_smart_range = 1; popScreen(); edit_sightrange(); });
        }
      if(allowChangeRange() && allowIncreasedSight()) {
        dialog::addBoolItem(XLAT("draw based on size in the projection (generation)"), vid.use_smart_range == 2, 'G');
        dialog::add_action([] () { vid.use_smart_range = 2; popScreen(); edit_sightrange(); });
        }
      if(vid.use_smart_range == 0 && allowChangeRange() && WDIM == 2) {
        dialog::addSelItem(XLAT("generation range bonus"), its(genrange_bonus), 'O');
        dialog::add_action([] () { genrange_bonus = sightrange_bonus; doOvergenerate(); });
        dialog::addSelItem(XLAT("game range bonus"), its(gamerange_bonus), 'S');
        dialog::add_action([] () { gamerange_bonus = sightrange_bonus; doOvergenerate(); });
        }      
      if(vid.use_smart_range && WDIM == 2) {
        dialog::addBoolItem_action(XLAT("area-based range"), vid.smart_area_based, 'A');
        }
      if(!allowChangeRange() || !allowIncreasedSight()) {
        dialog::addItem(XLAT("enable the cheat mode for additional options"), 'X');
        dialog::add_action(enable_cheat);
        }
      if(WDIM == 3 && !vid.use_smart_range) {
        dialog::addBoolItem_action(XLAT("sloppy range checking"), vid.sloppy_3d, 'S');
        }
      if(GDIM == 3 && !vid.use_smart_range) {
        dialog::addSelItem(XLAT("limit generation"), fts(extra_generation_distance), 'E');
        dialog::add_action([] {
          auto xo = dialog::extra_options;
          dialog::editNumber(extra_generation_distance, 0, 999, 0.5, 999, XLAT("limit generation"), 
            "Cells over this distance will not be generated, but they will be drawn if they are already generated and in the sight range."
            );
          dialog::extra_options = xo; popScreen();
          });
        }
      }
    add_cells_drawn('C');
    if(GDIM == 3 && WDIM == 2 && pmodel == mdPerspective) {
      dialog::addSelItem(XLAT("fog effect"), fts(sightranges[geometry]), 'R');
      dialog::add_action([] {
        auto xo = dialog::extra_options;
        dialog::editNumber(sightranges[geometry], 0, 2 * M_PI, 0.5, M_PI, XLAT("fog effect"), "");
        dialog::extra_options = xo; popScreen();
        });
      };
    };
  }

EX void menuitem_sightrange(char c IS('c')) {
  if(vid.use_smart_range)
    dialog::addSelItem(XLAT("minimum visible cell in pixels"), fts(WDIM == 3 ? vid.smart_range_detail_3 : vid.smart_range_detail), c);
  #if CAP_SOLV
  else if(pmodel == mdGeodesic && sol)
    dialog::addSelItem(XLAT("3D sight range"), fts(sn::solrange_xy) + "x" + fts(sn::solrange_z), c);
  #endif
  else if(WDIM == 3)
    dialog::addSelItem(XLAT("3D sight range"), fts(sightranges[geometry]), c);
  else
    dialog::addSelItem(XLAT("sight range"), its(sightrange_bonus), c);
  dialog::add_action(edit_sightrange);
  }

EX void menuitem_sfx_volume() {
  dialog::addSelItem(XLAT("sound effects volume"), its(effvolume), 'e');
  dialog::add_action([] {
    dialog::editNumber(effvolume, 0, 128, 10, 60, XLAT("sound effects volume"), "");
    dialog::numberdark = dialog::DONT_SHOW;
    dialog::reaction = [] () {
      #if ISANDROID
      settingsChanged = true;
      #endif
      };
    dialog::bound_low(0);
    dialog::bound_up(MIX_MAX_VOLUME);
    });
  }

EX void menuitem_music_volume() {
  if (!audio) return;
  dialog::addSelItem(XLAT("background music volume"), its(musicvolume), 'b');
  dialog::add_action([] {
    dialog::editNumber(musicvolume, 0, 128, 10, 60, XLAT("background music volume"), "");
    dialog::numberdark = dialog::DONT_SHOW;
    dialog::reaction = [] () {
      #if CAP_SDLAUDIO
      Mix_VolumeMusic(musicvolume);
      #endif
      #if ISANDROID
      settingsChanged = true;
      #endif
      };
    dialog::bound_low(0);
    dialog::bound_up(MIX_MAX_VOLUME);
    dialog::extra_options = [] {
      dialog::addBoolItem_action(XLAT("play music when out of focus"), music_out_of_focus, 'A');
      };
    });
  }

EX void showSpecialEffects() {
  cmode = vid.xres > vid.yres * 1.4 ? sm::SIDE : sm::MAYDARK;
  gamescreen(0);
  dialog::init(XLAT("extra graphical effects"));

  dialog::addBoolItem_action(XLAT("particles on attack"), (vid.particles), 'p');
  dialog::addBoolItem_action(XLAT("floating bubbles: special"), vid.bubbles_special, 's');
  dialog::addBoolItem_action(XLAT("floating bubbles: treasure thresholds"), vid.bubbles_threshold, 't');
  dialog::addBoolItem_action(XLAT("floating bubbles: all treasures"), vid.bubbles_all, 'a');
  dialog::addBoolItem_action(XLAT("background particle effects"), (vid.backeffects), 'b');

  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  }

EX void showGraphConfig() {
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

  dialog::addSelItem(XLAT("vector graphics modes"), XLAT("width") + " " + fts(vid.linewidth), 'w');
  
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

  dialog::addSelItem(XLAT("camera movement speed"), fts(camera_speed), 'c');
  dialog::add_action([] { 
    dialog::editNumber(camera_speed, -10, 10, 0.1, 1, XLAT("camera movement speed"), 
      "This affects:\n\nin 2D: scrolling with arrow keys and Wheel Up\n\nin 3D: camera movement with Home/End."
      );
    });
  dialog::addSelItem(XLAT("camera rotation speed"), fts(camera_rot_speed), 'r');
  dialog::add_action([] { 
    dialog::editNumber(camera_rot_speed, -10, 10, 0.1, 1, XLAT("camera rotation speed"), 
      "This affects view rotation with Page Up/Down, and in 3D, camera rotation with arrow keys or mouse."
      );
    });
    
  dialog::addSelItem(XLAT("movement animation speed"), fts(vid.mspeed), 'm');

  dialog::addItem(XLAT("extra graphical effects"), 'u');

  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();

  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
  
    if(uni == 'O') uni = 'o', shiftmul = -1;
    
    char xuni = uni | 96;
  
    if((uni >= 32 && uni < 64) || uni == 'L' || uni == 'C') xuni = uni;
    
    if(xuni == 'u') pushScreen(showSpecialEffects);

    else if(xuni == 'a') dialog::editNumber(vid.sspeed, -5, 5, 1, 0, 
      XLAT("scrolling speed"),
      XLAT("+5 = center instantly, -5 = do not center the map")
      + "\n\n" +
      XLAT("press Space or Home to center on the PC"));
  
    else if(xuni == 'm') dialog::editNumber(vid.mspeed, -5, 5, 1, 0, 
      XLAT("movement animation speed"),
      XLAT("+5 = move instantly"));
  
    else if(xuni == 'f') switchFullscreen();
  
  #if CAP_GLORNOT
    else if(xuni == 'o' && shiftmul > 0) switchGL();
  #endif
  
    else if(xuni == 'o' && shiftmul < 0) {
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
   
    else if(xuni == 'w') {
      dialog::editNumber(vid.linewidth, 0, 10, 0.1, 1, XLAT("line width"), 
        vid.usingGL ? "" : XLAT("Line width setting is only taken into account in OpenGL."));
      dialog::extra_options = [] () {
        dialog::addBoolItem("finer lines at the boundary", vid.antialias & AA_LINEWIDTH, 'O');
        dialog::add_action([] () { 
          vid.antialias ^= AA_LINEWIDTH; 
          });

        dialog::addBoolItem("perfect width", perfect_linewidth == 2, 'P');
        if(perfect_linewidth == 1) 
          dialog::lastItem().value = XLAT("shots only");
        dialog::add_action([] { perfect_linewidth = (1 + perfect_linewidth) % 3; });

        if(vid.antialias & AA_LINEWIDTH) {
          dialog::addSelItem("variable width", fts(precise_width), 'M');
          dialog::add_action([] () {
            popScreen();
            dialog::editNumber(precise_width, 0, 2, 0.1, 0.5, 
              XLAT("variable width"), XLAT("lines longer than this value will be split into shorter lines, with width computed separately for each of them.")
              );
            });
          }
        else dialog::addBreak(100);
        
        auto neon_option = [&] (string s, eNeon val, char key) {
          dialog::addBoolItem(XLAT(s), neon_mode == val, key);
          dialog::add_action([val] { neon_mode = (neon_mode == val) ? eNeon::none : val; });
          };
        
        neon_option("neon mode", eNeon::neon, 'B');
        neon_option("no boundary mode", eNeon::no_boundary, 'C');
        neon_option("neon mode II", eNeon::neon2, 'D');
        neon_option("illustration mode", eNeon::illustration, 'E');
        dialog::addBreak(100);
        dialog::addInfo(XLAT("hint: press Alt while testing modes"));
        dialog::addBreak(100);
        dialog::addBoolItem_action(XLAT("disable shadows"), noshadow, 'F');
        dialog::addBoolItem_action(XLAT("bright mode"), bright, 'G');
        dialog::addBoolItem_action(XLAT("colorblind simulation"), cblind, 'H');

        dialog::addBoolItem_action(XLAT("no fill in neon mode"), neon_nofill, 'N');
        };
      }
    
    else if(xuni == 'L') {
      dialog::editNumber(vid.linequality, -3, 5, 1, 1, XLAT("line quality"), 
        XLAT("Higher numbers make the curved lines smoother, but reduce the performance."));
      }
  
  #if CAP_FRAMELIMIT    
    else if(xuni == 'l') {
      dialog::editNumber(vid.framelimit, 5, 300, 10, 300, XLAT("framerate limit"), "");
      dialog::bound_low(5);
      }
  #endif
      
    else if(xuni =='p') 
      vid.backeffects = !vid.backeffects;
      
    else if(doexiton(sym, uni)) popScreen();
    };
  }
  
EX void switchFullscreen() {
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

EX void switchGL() {
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

EX void edit_whatever(char type, int index) {
  if(type == 'f') {
    dialog::editNumber(whatever[index], -10, 10, 1, 0, XLAT("whatever"), 
      "f:" + its(index));
    }
  else {
    dialog::editNumber(whateveri[index], -10, 10, 1, 0, XLAT("whatever"), 
      "i:" + its(index));
    }
  dialog::extra_options = [type, index] {
    dialog::addItem(XLAT("integer"), 'X');
    dialog::add_action( [index] { popScreen(); edit_whatever('i', index); });
    dialog::addItem(XLAT("float"), 'Y');
    dialog::add_action( [index] { popScreen(); edit_whatever('f', index); });
    for(int x=0; x<8; x++) {
      dialog::addSelItem(its(x), type == 'i' ? its(whateveri[x]) : fts(whatever[x]), 'A' + x);
      dialog::add_action([type,x] { popScreen(); edit_whatever(type, x); });
      }
    };
  }

EX void configureOther() {
  gamescreen(3);

  dialog::init(XLAT("other settings"));

#if ISSTEAM
  dialog::addBoolItem(XLAT("send scores to Steam leaderboards"), (vid.steamscore&1), 'x');
  dialog::add_action([] {vid.steamscore = vid.steamscore^1; });
#endif

  dialog::addBoolItem_action(XLAT("skip the start menu"), vid.skipstart, 'm');
  
  dialog::addItem(XLAT("memory configuration"), 'y');
  dialog::add_action_push(show_memory_menu);

  // dialog::addBoolItem_action(XLAT("forget faraway cells"), memory_saving_mode, 'y');
  
#if CAP_AUDIO
  menuitem_music_volume();
  menuitem_sfx_volume();
#endif

  menuitem_sightrange('r');

#ifdef WHATEVER
  dialog::addSelItem(XLAT("whatever"), fts(whatever[0]), 'j');
  dialog::add_action([] { edit_whatever('f', 0); });
#endif
  
  dialog::addBreak(50);
  dialog::addBack();
  
  dialog::display();
  }

EX void configureInterface() {
  gamescreen(3);
  dialog::init(XLAT("interface"));

#if CAP_TRANS
  dialog::addSelItem(XLAT("language"), XLAT("EN"), 'l');
  dialog::add_action_push(selectLanguageScreen);
#endif

  dialog::addSelItem(XLAT("player character"), numplayers() > 1 ? "" : csname(vid.cs), 'g');
  dialog::add_action_push(showCustomizeChar);
  if(getcstat == 'g') mouseovers = XLAT("Affects looks and grammar");

  dialog::addSelItem(XLAT("message flash time"), its(vid.flashtime), 't');
  dialog::add_action([] {
    dialog::editNumber(vid.flashtime, 0, 64, 1, 8, XLAT("message flash time"),
      XLAT("How long should the messages stay on the screen."));
    dialog::bound_low(0);
    });

  dialog::addSelItem(XLAT("limit messages shown"), its(vid.msglimit), 'z');
  dialog::add_action([] {
    dialog::editNumber(vid.msglimit, 0, 64, 1, 5, XLAT("limit messages shown"),
      XLAT("Maximum number of messages on screen."));
    dialog::bound_low(0);
    });
  
  const char* msgstyles[3] = {"centered", "left-aligned", "line-broken"};
  
  dialog::addSelItem(XLAT("message style"), XLAT(msgstyles[vid.msgleft]), 'a');
  dialog::add_action([] {
    vid.msgleft = (1+vid.msgleft) % 3;
    });

  dialog::addSelItem(XLAT("font scale"), its(fontscale), 'b');
  dialog::add_action([] {
    dialog::editNumber(fontscale, 25, 400, 10, 100, XLAT("font scale"), "");
    const int minfontscale = ISMOBILE ? 50 : 25;
    dialog::reaction = [] () { setfsize = true; do_setfsize(); };
    dialog::bound_low(minfontscale);
    });  

  const char *glyphsortnames[6] = {
    "first on top", "first on bottom", 
    "last on top", "last on bottom",
    "by land", "by number"
    };
  dialog::addSelItem(XLAT("inventory/kill sorting"), XLAT(glyphsortnames[glyphsortorder]), 'k');
  dialog::add_action([] {
    glyphsortorder = eGlyphsortorder((glyphsortorder+6+(shiftmul>0?1:-1)) % gsoMAX);
    });

  const char *glyphmodenames[3] = {"letters", "auto", "images"};
  dialog::addSelItem(XLAT("inventory/kill mode"), XLAT(glyphmodenames[vid.graphglyph]), 'd');
  dialog::add_action([] {
    vid.graphglyph = (1+vid.graphglyph)%3;
    });    

  dialog::addSelItem(XLAT("draw crosshair"), crosshair_size > 0 ? fts(crosshair_size) : ONOFF(false), 'x');
  dialog::add_action([] () { 
    dialog::editNumber(crosshair_size, 0, 100, 1, 10, XLAT("crosshair size"), XLAT(
      "Display a targetting reticle in the center of the screen. Might be useful when exploring 3D modes, "
      "as it precisely shows the direction we are going. However, the option is available in all modes."
      ));
    dialog::bound_low(0);
    dialog::extra_options = [] {
      dialog::addColorItem(XLAT("crosshair color"), crosshair_color, 'X');
      dialog::add_action([] { dialog::openColorDialog(crosshair_color); });
      };
    });
   
  dialog::addBreak(50);
  dialog::addBack();
  
  dialog::display();
  }

#if CAP_SDLJOY
EX void showJoyConfig() {
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
  dialog::addSelItem(XLAT("smoothen"), its(vid.joysmooth) + " ms", 'e');

  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    if(uni == 'p') autojoy = !autojoy;
    else if(uni == 'a') {
      dialog::editNumber(vid.joyvalue, 0, 32768, 100, 4800, XLAT("first joystick: movement threshold"), "");
      dialog::bound_low(0);
      }
    else if(uni == 'b') {
      dialog::editNumber(vid.joyvalue2, 0, 32768, 100, 5600, XLAT("first joystick: execute movement threshold"), "");
      dialog::bound_low(0);
      }
    else if(uni == 'c') {
      dialog::editNumber(vid.joypanthreshold, 0, 32768, 100, 2500, XLAT("second joystick: pan threshold"), "");
      dialog::bound_low(0);
      }
    else if(uni == 'd')
      dialog::editNumber(vid.joypanspeed, 0, 1e-2, 1e-5, 1e-4, XLAT("second joystick: panning speed"), "");
    else if(uni == 'e')
      dialog::editNumber(vid.joypanspeed, 0, 2000, 20, 200, XLAT("smoothen"), "large values help if the joystick is imprecise");
  
    else if(doexiton(sym, uni)) popScreen();
    };
  }
#endif

EX void projectionDialog() {
  vid.tc_alpha = ticks;
  dialog::editNumber(vpconf.alpha, -5, 5, .1, 1,
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
    if(GDIM == 2) dialog::addHelp(XLAT(
      "If we are viewing an equidistant g absolute units below a plane, "
      "from a point c absolute units above the plane, this corresponds "
      "to viewing a Minkowski hyperboloid from a point "
      "tanh(g)/tanh(c) units below the center. This in turn corresponds to "
      "the Poincaré model for g=c, and Klein-Beltrami model for g=0."));
    dialog::addSelItem(sphere ? "stereographic" : "Poincaré model", "1", 'P');
    dialog::add_action([] () { *dialog::ne.editwhat = 1; vpconf.scale = 1; dialog::ne.s = "1"; });
    dialog::addSelItem(sphere ? "gnomonic" : "Klein model", "0", 'K');
    dialog::add_action([] () { *dialog::ne.editwhat = 0; vpconf.scale = 1; dialog::ne.s = "0"; });
    if(hyperbolic) {
      dialog::addSelItem("inverted Poincaré model", "-1", 'I');
      dialog::add_action([] () { *dialog::ne.editwhat = -1; vpconf.scale = 1; dialog::ne.s = "-1"; });
      }
    dialog::addItem(sphere ? "orthographic" : "Gans model", 'O');
    dialog::add_action([] () { vpconf.alpha = vpconf.scale = 999; dialog::ne.s = dialog::disp(vpconf.alpha); });
    dialog::addItem(sphere ? "towards orthographic" : "towards Gans model", 'T');
    dialog::add_action([] () { double d = 1.1; vpconf.alpha *= d; vpconf.scale *= d; dialog::ne.s = dialog::disp(vpconf.alpha); });
    };
  }

EX void explain_detail() {
  dialog::addHelp(XLAT(
    "Objects at distance less than %1 absolute units "
    "from the center will be displayed with high "
    "detail, and at distance at least %2 with low detail.",
    fts(vid.highdetail), fts(vid.middetail)
    ));
  }

EX ld max_fov_angle() {
  if(panini_alpha >= 1 || panini_alpha <= -1) return 360;
  return acos(-panini_alpha) * 2 / degree;
  }

EX void add_edit_fov(char key IS('f'), bool pop IS(false)) {

  string sfov = fts(vid.fov) + "°";
  if(panini_alpha) {
    sfov += " / " + fts(max_fov_angle()) + "°";
    }
  dialog::addSelItem(XLAT("field of view"), sfov, key);
  dialog::add_action([=] {
    if(pop) popScreen();
    dialog::editNumber(vid.fov, 1, max_fov_angle(), 1, 45, "field of view", 
      XLAT(
        "Horizontal field of view, in angles. "
        "This affects the Hypersian Rug mode (even when stereo is OFF) "
        "and non-disk models.") + "\n\n" +
      XLAT(
        "Must be less than %1°. Panini projection can be used to get higher values.",
        fts(max_fov_angle())
        )
        );
    dialog::bound_low(1e-8);
    dialog::bound_up(max_fov_angle() - 0.01);
    dialog::extra_options = [] {
      dialog::addSelItem(XLAT("Panini projection"), fts(panini_alpha), 'P');
      dialog::add_action([] {
        popScreen();
        dialog::editNumber(panini_alpha, 0, 1, 0.1, 0, "Panini parameter", 
          XLAT(
            "The Panini projection is an alternative perspective projection "
            "which allows very wide field-of-view values. HyperRogue uses "
            "a quick implementation, so parameter values too close to 1 may "
            "be buggy; try e.g. 0.9 instead.")
            );
        dialog::reaction = ray::reset_raycaster;
        dialog::extra_options = [] {
          add_edit_fov('F', true);
          };
        });
      };
    });
  }

bool supported_ods() {
  if(!CAP_ODS) return false;
  return rug::rugged || (hyperbolic && GDIM == 3);
  }

EX void showStereo() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);
  dialog::init(XLAT("stereo vision config"));

  string modenames[4] = { "OFF", "anaglyph", "side-by-side", "ODS" };
  
  dialog::addSelItem(XLAT("stereo mode"), XLAT(modenames[vid.stereo_mode]), 'm');
  dialog::addSelItem(XLAT("pupillary distance"), fts(vid.ipd), 'e');
  
  switch(vid.stereo_mode) {
    case sAnaglyph:
      dialog::addSelItem(XLAT("distance between images"), fts(vid.anaglyph_eyewidth), 'd');
      break;
    case sLR:
      dialog::addSelItem(XLAT("distance between images"), fts(vid.lr_eyewidth), 'd');
      break;
    default:
      dialog::addBreak(100);
      break;
    }
  
  dialog::addSelItem(XLAT("desaturate colors"), its(vid.desaturate)+"%", 'c');
  dialog::add_action([] {
    dialog::editNumber(vid.desaturate, 0, 100, 10, 0, XLAT("desaturate colors"),
      XLAT("Make the game colors less saturated. This is useful in the anaglyph mode.")
      );    
    });
  
  add_edit_fov('f');

  dialog::addBack();
  dialog::display();

  keyhandler = [] (int sym, int uni) {
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

    if(uni == 'm') {
      vid.stereo_mode = eStereo((1 + vid.stereo_mode) % 4);
      if(vid.stereo_mode == sODS && !supported_ods()) vid.stereo_mode = sOFF;
      }
    
    else if(uni == 'e') 
      dialog::editNumber(vid.ipd, -10, 10, 0.01, 0, XLAT("pupillary distance"),
        help3 + 
        XLAT("The distance between your eyes in the represented 3D object. This is given in absolute units.")
        ), dialog::scaleSinh100();
      
    else if(uni == 'd' && vid.stereo_mode == sAnaglyph)
      dialog::editNumber(vid.anaglyph_eyewidth, -1, 1, 0.01, 0, XLAT("distance between images"),
        help3 +
        XLAT("The distance between your eyes. 1 is the width of the screen."));

    else if(uni == 'd' && vid.stereo_mode == sLR)
      dialog::editNumber(vid.lr_eyewidth, -1, 1, 0.01, 0, XLAT("distance between images"),
        help3 +
        XLAT("The distance between your eyes. 1 is the width of the screen."));
      
    else if(doexiton(sym, uni)) popScreen();
    };
  }

EX void config_camera_rotation() {
  dialog::editNumber(pconf.ballangle, 0, 90, 5, 0, XLAT("camera rotation in 3D models"), 
    "Rotate the camera in 3D models (ball model, hyperboloid, and hemisphere). "
    "Note that hyperboloid and hemisphere models are also available in the "
    "Hypersian Rug surfaces menu, but they are rendered differently there -- "
    "by making a flat picture first, then mapping it to a surface. "
    "This makes the output better in some ways, but 3D effects are lost. "
    "Hypersian Rug model also allows more camera freedom."
    );
  }

EX void add_edit_wall_quality(char c) {
  dialog::addSelItem(XLAT("wall quality"), its(vid.texture_step), c);
  dialog::add_action([] {
    dialog::editNumber(vid.texture_step, 1, 16, 1, 1, XLAT("wall quality"), 
      XLAT(
      "Controls the number of triangles used for wall surfaces. "
      "Higher numbers reduce the performance. "
      "This has a strong effect when the walls are curved indeed "
      "(floors in 2D geometries, honeycombs based on horospheres, and projections other than native perspective), "
      "but otherwise, usually it can be set to 1 without significant adverse effects other "
      "than slightly incorrect texturing."
      )
      );
    dialog::bound_low(1);
    dialog::bound_up(128);
    dialog::reaction = [] {
      #if MAXMDIM >= 4
      if(floor_textures) {
        delete floor_textures;
        floor_textures = NULL;
        }
      #endif
      };
    });
  }

EX void edit_levellines(char c) {
  if(levellines)
    dialog::addSelItem(XLAT("level lines"), fts(levellines), c);
  else
    dialog::addBoolItem(XLAT("level lines"), false, c);
  dialog::add_action([] {
    dialog::editNumber(levellines, 0, 100, 0.5, 0, XLAT("level lines"), 
      XLAT(
        "This feature superimposes level lines on the rendered screen. These lines depend on the Z coordinate. In 3D hyperbolic the Z coordinate is taken from the Klein model. "
        "Level lines can be used to observe the curvature: circles correspond to positive curvature, while hyperbolas correspond to negative. See e.g. the Hypersian Rug mode.")
      );
    dialog::reaction = ray::reset_raycaster;
    dialog::extra_options = [] {
      dialog::addBoolItem(XLAT("disable textures"), disable_texture, 'T');
      dialog::add_action([] { ray::reset_raycaster(); disable_texture = !disable_texture; });
      dialog::addItem(XLAT("disable level lines"), 'D');
      dialog::add_action([] { ray::reset_raycaster(); levellines = 0; popScreen(); });
      };
    dialog::bound_low(0);
    });
  }

EX void show3D() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);
  dialog::init(XLAT("3D configuration"));

#if MAXMDIM >= 4
  if(WDIM == 2) {
    dialog::addBoolItem(XLAT("use the full 3D models"), vid.always3, 'U');
    dialog::add_action(geom3::switch_always3);
    }
#endif
  if(vid.use_smart_range == 0 && GDIM == 2) {
    dialog::addSelItem(XLAT("High detail range"), fts(vid.highdetail), 'n');
    dialog::addSelItem(XLAT("Mid detail range"), fts(vid.middetail), 'm');
    dialog::addBreak(50);
    }
  
  if(WDIM == 2) {
    dialog::addSelItem(XLAT(GDIM == 2 ? "Camera level above the plane" : "Z shift"), fts(vid.camera), 'c');
    if(GDIM == 3)
      dialog::addSelItem(XLAT("Eye level"), fts(vid.eye), 'E');

    dialog::addSelItem(XLAT("Ground level below the plane"), fts(vid.depth), 'g');
    
  
    if(GDIM == 2)
      dialog::addSelItem(XLAT("Projection at the ground level"), fts(pconf.alpha), 'p');
    else if(!in_perspective())
      dialog::addSelItem(XLAT("Projection distance"), fts(pconf.alpha), 'p');
    
    dialog::addBreak(50);
    dialog::addSelItem(XLAT("Height of walls"), fts(vid.wall_height), 'w');
    
    dialog::addSelItem(XLAT("Rock-III to wall ratio"), fts(vid.rock_wall_ratio), 'r');
    dialog::addSelItem(XLAT("Human to wall ratio"), fts(vid.human_wall_ratio), 'h');
    dialog::addSelItem(XLAT("Level of water surface"), fts(vid.lake_top), 'l');
    dialog::addSelItem(XLAT("Level of water bottom"), fts(vid.lake_bottom), 'k');  
    if(scale_used())
      dialog::addSelItem(XLAT("Creature scale"), fts(vid.creature_scale), 'C');
    }
  else {
    dialog::addSelItem(XLAT("Creature scale"), fts(vid.creature_scale), 'c');
    dialog::addSelItem(XLAT("Height to width"), fts(vid.height_width), 'h');
    menuitem_sightrange('s');
    }

  dialog::addBreak(50);
  dialog::addSelItem(XLAT(GDIM == 3 && WDIM == 2 ? "Y shift" : "third person perspective"), fts(vid.yshift), 'y');
  if(GDIM == 3) {
    dialog::addSelItem(XLAT("mouse aiming sensitivity"), fts(mouseaim_sensitivity), 'a');
    dialog::add_action([] () { 
      dialog::editNumber(mouseaim_sensitivity, -1, 1, 0.002, 0.01, XLAT("mouse aiming sensitivity"), "set to 0 to disable");
      });
    }
  if(true) {
    dialog::addSelItem(XLAT("camera rotation"), fts(vpconf.camera_angle), 'S');
    dialog::add_action([] {
      dialog::editNumber(vpconf.camera_angle, -180, 180, 5, 0, XLAT("camera rotation"), 
        XLAT("Rotate the camera. Can be used to obtain a first person perspective, "
        "or third person perspective when combined with Y shift.")
        );
      });
    }
  if(GDIM == 2) {
    dialog::addSelItem(XLAT("fixed facing"), vid.fixed_facing ? fts(vid.fixed_facing_dir) : XLAT("OFF"), 'f');
    dialog::add_action([] () { vid.fixed_facing = !vid.fixed_facing; 
      if(vid.fixed_facing) {
        dialog::editNumber(vid.fixed_facing_dir, 0, 360, 15, 90, "", "");
        dialog::dialogflags |= sm::CENTER;
        }
      });
    }

  if((WDIM == 2 && GDIM == 3) || prod)
    dialog::addBoolItem_action(XLAT("fixed Y/Z rotation"), vid.fixed_yz, 'Z');

  if(true) {
    dialog::addBreak(50);
    dialog::addSelItem(XLAT("projection"), current_proj_name(), 'M');
    }
  #if MAXMDIM >= 4
  if(GDIM == 3) add_edit_fov('f');
  if(GDIM == 3) {
    dialog::addSelItem(XLAT("radar size"), fts(vid.radarsize), 'r');
    dialog::add_action([] () {
      dialog::editNumber(vid.radarsize, 0, 360, 15, 90, "", XLAT("set to 0 to disable"));
      dialog::extra_options = [] () { draw_radar(true); };
      });
    }
  
  if(WDIM == 3 && sphere && stretch::factor) {
    dialog::addItem(XLAT("Berger sphere limit"), berger_limit);
    dialog::add_action([] () {
      dialog::editNumber(berger_limit, 0, 10, 1, 2, "", 
        XLAT("Primitive-based rendering of Berger sphere is currently very slow and low quality. "
          "Here you can choose how many images to draw.")
        );
      });
    }
  
  #if CAP_RAY
  if(GDIM == 3) {
    dialog::addItem(XLAT("configure raycasting"), 'A');
    dialog::add_action_push(ray::configure);
    }
  #endif
  
  edit_levellines('L');
  
  if(WDIM == 3 || (GDIM == 3 && euclid)) {
    dialog::addSelItem(XLAT("radar range"), fts(vid.radarrange), 'R');
    dialog::add_action([] () {
      dialog::editNumber(vid.radarrange, 0, 10, 0.5, 2, "", XLAT(""));
      dialog::extra_options = [] () { draw_radar(true); };
      });
    }
  if(GDIM == 3) add_edit_wall_quality('W');
  #endif
  
  dialog::addBreak(50);
  #if CAP_RUG
  if(rug::rugged) {
    dialog::addBoolItem_action(XLAT("3D monsters/walls on the surface"), rug::spatial_rug, 'S');
    }
  #endif
  if(GDIM == 2) {
    dialog::addBoolItem(XLAT("configure TPP automatically"), pmodel == mdDisk && pconf.camera_angle, 'T');
    dialog::add_action(geom3::switch_tpp);
    }
  
#if MAXMDIM >=4
  if(WDIM == 2) {
    dialog::addBoolItem(XLAT("configure FPP automatically"), GDIM == 3, 'F');
    dialog::add_action(geom3::switch_fpp);
    }
#endif

  if(0);
  #if CAP_RUG
  else if(rug::rugged && !rug::spatial_rug)
    dialog::addBreak(100);
  #endif
  else if(GDIM == 2 && non_spatial_model())
    dialog::addInfo(XLAT("no 3D effects available in this projection"), 0xC00000);
  else if(GDIM == 2 && !spatial_graphics)
    dialog::addInfo(XLAT("set 3D monsters or walls in basic config first"));
  else if(geom3::invalid != "")
    dialog::addInfo(XLAT("error: "+geom3::invalid), 0xC00000);
  else
    dialog::addInfo(XLAT("parameters set correctly"));
  dialog::addBreak(50);
  dialog::addItem(XLAT("stereo vision config"), 'e');
  dialog::addBack();
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    using namespace geom3;
    dialog::handleNavigation(sym, uni);
    
    if(uni == 'n' && GDIM == 2) {
      dialog::editNumber(vid.highdetail, 0, 5, .5, 7, XLAT("High detail range"), "");
      dialog::extra_options = explain_detail;
      dialog::reaction = [] () {
        if(vid.highdetail > vid.middetail) vid.middetail = vid.highdetail;
        };
      }
    else if(uni == 'm' && GDIM == 2) {
      dialog::editNumber(vid.middetail, 0, 5, .5, 7, XLAT("Mid detail range"), "");
      dialog::extra_options = explain_detail;
      dialog::reaction = [] () {
        if(vid.highdetail > vid.middetail) vid.highdetail = vid.middetail;
        };
      }
    else if(uni == 'c' && WDIM == 2) 
      vid.tc_camera = ticks,
      dialog::editNumber(vid.camera, 0, 5, .1, 1, XLAT(GDIM == 2 ? "Camera level above the plane" : "Z shift"), ""),
      dialog::extra_options = [] {
        dialog::addHelp(GDIM == 2 ? XLAT(
          "Camera is placed %1 absolute units above a plane P in a three-dimensional "
          "world. Ground level is actually an equidistant surface, %2 absolute units "
          "below the plane P. The plane P (as well as the ground level or any "
          "other equidistant surface below it) is viewed at an angle of %3 "
          "(the tangent of the angle between the point in "
          "the center of your vision and a faraway location is 1/cosh(c) = %4).",
          fts(vid.camera),
          fts(vid.depth),
          fts(atan(1/cosh(vid.camera))*2/degree),
          fts(1/cosh(vid.camera))) : XLAT("Look from behind."));
        if(GDIM == 3 && pmodel == mdPerspective) dialog::extra_options = [] () {
          dialog::addBoolItem_action(XLAT("reduce if walls on the way"), vid.use_wall_radar, 'R');
          };
        };
    else if(uni == 'g' && WDIM == 2) 
      vid.tc_depth = ticks,
      dialog::editNumber(vid.depth, 0, 5, .1, 1, XLAT("Ground level below the plane"), ""),
      dialog::extra_options = [] {
        help = XLAT(
          "Ground level is actually an equidistant surface, "
          "%1 absolute units below the plane P. "
          "Theoretically, this value affects the world -- "
          "for example, eagles could fly %2 times faster by "
          "flying above the ground level, on the plane P -- "
          "but the actual game mechanics are not affected. ", fts(vid.depth), fts(cosh(vid.depth)));        
        if(GDIM == 2)
          help += XLAT(
            "(Distances reported by the vector graphics editor "
            "are not about points on the ground level, but "
            "about the matching points on the plane P -- "
            "divide them by the factor above to get actual "
            "distances.)"
            );
        if(GDIM == 3 && pmodel == mdPerspective && !euclid) {
          ld current_camera_level = hdist0(tC0(radar_transform));
          help += "\n\n";
          if(abs(current_camera_level) < 1e-6)
            help += XLAT(
              "The camera is currently exactly on the plane P. "
              "The horizon is seen as a straight line."
              );
          else help += XLAT(
              "The camera is currently %1 units above the plane P. "
              "This makes you see the floor level as in general perspective projection "
              "with parameter %2.", fts(current_camera_level), fts(tan_auto(vid.depth) / tan_auto(current_camera_level)));
          }
        dialog::addHelp(help);
        };
    else if(uni == 'E' && WDIM == 2 && GDIM == 3) 
      vid.tc_depth = ticks,
      dialog::editNumber(vid.eye, -5, 5, .1, 0, XLAT("eye level"), ""),
      dialog::dialogflags |= sm::CENTER,
      dialog::extra_options = [] {
      
        dialog::addHelp(XLAT("In the FPP mode, the camera will be set at this altitude (before applying shifts)."));

        dialog::addBoolItem(XLAT("auto-adjust to eyes on the player model"), vid.auto_eye, 'O');
        dialog::reaction = [] { vid.auto_eye = false; };
        dialog::add_action([] () {
          vid.auto_eye = !vid.auto_eye;
          geom3::do_auto_eye();
          });
        };
    else if(uni == 'p' && WDIM == 2) 
      projectionDialog();
    else if(uni == 'w' && WDIM == 2) {
      dialog::editNumber(vid.wall_height, 0, 1, .1, .3, XLAT("Height of walls"), "");
      dialog::extra_options = [] () {
        dialog::addHelp(GDIM == 3 ? "" : XLAT(
          "The height of walls, in absolute units. For the current values of g and c, "
          "wall height of %1 absolute units corresponds to projection value of %2.",
          fts(actual_wall_height()), fts(factor_to_projection(cgi.WALL))));
        dialog::addBoolItem(XLAT("auto-adjust in Goldberg grids"), vid.gp_autoscale_heights, 'O');
        dialog::add_action([] () {
          vid.gp_autoscale_heights = !vid.gp_autoscale_heights;
          });
        };
      }
    else if(uni == 'l' && WDIM == 2) 
      dialog::editNumber(vid.lake_top, 0, 1, .1, .25, XLAT("Level of water surface"), "");
    else if(uni == 'k' && WDIM == 2) 
      dialog::editNumber(vid.lake_bottom, 0, 1, .1, .9, XLAT("Level of water bottom"), "");
    else if(uni == 'r' && WDIM == 2) 
      dialog::editNumber(vid.rock_wall_ratio, 0, 1, .1, .9, XLAT("Rock-III to wall ratio"), ""),
      dialog::extra_options = [] { dialog::addHelp(XLAT(
        "The ratio of Rock III to walls is %1, so Rock III are %2 absolute units high. "
        "Length of paths on the Rock III level is %3 of the corresponding length on the "
        "ground level.",
        fts(vid.rock_wall_ratio), fts(vid.wall_height * vid.rock_wall_ratio),
        fts(cosh(vid.depth - vid.wall_height * vid.rock_wall_ratio) / cosh(vid.depth))));
        };
    else if(uni == 'h' && WDIM == 2)
      dialog::editNumber(vid.human_wall_ratio, 0, 1, .1, .7, XLAT("Human to wall ratio"), ""),
      dialog::extra_options = [] { dialog::addHelp(XLAT(
        "Humans are %1 "
        "absolute units high. Your head travels %2 times the distance travelled by your "
        "feet.",
        fts(vid.wall_height * vid.human_wall_ratio),
        fts(cosh(vid.depth - vid.wall_height * vid.human_wall_ratio) / cosh(vid.depth)))
        );
        };
    else if(uni == 'h' && WDIM == 3)
      dialog::editNumber(vid.height_width, 0, 1, .1, .7, XLAT("Height to width"), "");
    else if(uni == 'c' && WDIM == 3)
      dialog::editNumber(vid.creature_scale, 0, 1, .1, .7, XLAT("Creature scale"), "");
    else if(uni == 'C' && WDIM == 2 && scale_used())
      dialog::editNumber(vid.creature_scale, 0, 1, .1, .7, XLAT("Creature scale"), "");

    else if(uni == 'e')
      pushScreen(showStereo);
    
    else if(uni == 'y') {
      dialog::editNumber(vid.yshift, 0, 1, .1, 0, XLAT("Y shift"), 
        XLAT("Don't center on the player character.")
        );
      if(WDIM == 3 && pmodel == mdPerspective) dialog::extra_options = [] () {
        dialog::addBoolItem_action(XLAT("reduce if walls on the way"), vid.use_wall_radar, 'R');
        };
      }
    else if(uni == 'b') 
      config_camera_rotation();
    else if(uni == 'M') 
      pushScreen(models::model_menu);  
    else if(doexiton(sym, uni)) 
      popScreen();
    };
  }

EX void switchcolor(unsigned int& c, unsigned int* cs) {
  dialog::openColorDialog(c, cs);
  }

double cc_footphase;
int lmousex, lmousey;

EX void showCustomizeChar() {

  cc_footphase += hypot(mousex - lmousex, mousey - lmousey);
  lmousex = mousex; lmousey = mousey;

  gamescreen(4);
  dialog::init(XLAT("Customize character"));
  
  if(shmup::on || multi::players) multi::cpid = multi::cpid_edit % multi::players;
  charstyle& cs = getcs();
  
  dialog::addSelItem(XLAT("character"), csname(cs), 'g');
  dialog::addColorItem(XLAT("skin color"), cs.skincolor, 's');
  dialog::addColorItem(XLAT("eye color"), cs.eyecolor, 'e');
  dialog::addColorItem(XLAT("weapon color"), cs.swordcolor, 'w');
  dialog::addColorItem(XLAT("hair color"), cs.haircolor, 'h');
  
  if(cs.charid >= 1) dialog::addColorItem(XLAT("dress color"), cs.dresscolor, 'd');
  else dialog::addBreak(100);
  if(cs.charid == 3) dialog::addColorItem(XLAT("dress color II"), cs.dresscolor2, 'f');
  else dialog::addBreak(100);
  
  dialog::addColorItem(XLAT("movement color"), cs.uicolor, 'u');

  if(!shmup::on && multi::players == 1) dialog::addSelItem(XLAT("save whom"), XLAT1(minf[moPrincess].name), 'p');
  
  if(numplayers() > 1) dialog::addSelItem(XLAT("player"), its(multi::cpid+1), 'a');

  dialog::addBoolItem(XLAT("left-handed"), cs.lefthanded, 'l');
  
  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  
  int firsty = dialog::items[0].position / 2;
  int scale = firsty - 2 * vid.fsize;
  
  flat_model_enabler fme;

  initquickqueue();
  transmatrix V = atscreenpos(vid.xres/2, firsty, scale);
  
  double alpha = atan2(mousex - vid.xres/2, mousey - firsty) - M_PI/2;
  V = V * spin(alpha);
  drawMonsterType(moPlayer, NULL, shiftless(V), 0, cc_footphase / scale, NOCOLOR);
  quickqueue();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
  
    if(shmup::on || multi::players) multi::cpid = multi::cpid_edit % multi::players;
    charstyle& cs = getcs();
    bool cat = cs.charid >= 4;
    if(uni == 'a') { multi::cpid_edit++; multi::cpid_edit %= 60; }
    else if(uni == 'g') {
      cs.charid++;
      if(cs.charid == 2 && !princess::everSaved && !autocheat) cs.charid = 4;
      cs.charid %= 10;
      }
    else if(uni == 'p') vid.samegender = !vid.samegender;
    else if(uni == 's') switchcolor(cs.skincolor, cat ? haircolors : skincolors);
    else if(uni == 'h') switchcolor(cs.haircolor, haircolors);
    else if(uni == 'w') switchcolor(cs.swordcolor, swordcolors);
    else if(uni == 'd') switchcolor(cs.dresscolor, cat ? haircolors : dresscolors);
    else if(uni == 'f') switchcolor(cs.dresscolor2, dresscolors2);
    else if(uni == 'u') switchcolor(cs.uicolor, eyecolors);
    else if(uni == 'e') switchcolor(cs.eyecolor, eyecolors);
    else if(uni == 'l') cs.lefthanded = !cs.lefthanded;
    else if(doexiton(sym, uni)) popScreen();
    };
  }

EX void refresh_canvas() {
  manual_celllister cl;
  cl.add(cwt.at);
    
  int at = 0;
  while(at < isize(cl.lst)) {
    cell *c2 = cl.lst[at];
    c2->landparam = patterns::generateCanvas(c2);
    at++;
    
    forCellEx(c3, c2) cl.add(c3);
    }
  }

EX void edit_color_table(colortable& ct, const reaction_t& r IS(reaction_t()), bool has_bit IS(false)) {
  cmode = sm::SIDE;
  gamescreen(0);
  dialog::init(XLAT("colors & aura"));
  
  for(int i=0; i<isize(ct); i++) {
    dialog::addColorItem(its(i), ct[i] << 8, 'a'+i);
    if(WDIM == 3 && has_bit && !(ct[i] & 0x1000000)) dialog::lastItem().value = XLAT("(no wall)");
    dialog::add_action([i, &ct, r, has_bit] () { 
      if(WDIM == 3 && has_bit) {
        ct[i] ^= 0x1000000;
        if(!(ct[i] & 0x1000000)) return;
        }
      dialog::openColorDialog(ct[i]); 
      dialog::reaction = r; 
      dialog::colorAlpha = false;
      dialog::dialogflags |= sm::SIDE;
      });
    }

  dialog::addBack();
  dialog::display();
  }

EX void show_color_dialog() {
  cmode = sm::SIDE | sm::DIALOG_STRICT_X;
  getcstat = '-';
  gamescreen(0);
  dialog::init(XLAT("colors & aura"));

  dialog::addColorItem(XLAT("background"), backcolor << 8, 'b');
  dialog::add_action([] () { dialog::openColorDialog(backcolor); dialog::colorAlpha = false; dialog::dialogflags |= sm::SIDE; });
  
  if(WDIM == 2 && GDIM == 3 && hyperbolic)
    dialog::addBoolItem_action(XLAT("cool fog effect"), context_fog, 'B');

  dialog::addColorItem(XLAT("foreground"), forecolor << 8, 'f');
  dialog::add_action([] () { dialog::openColorDialog(forecolor); dialog::colorAlpha = false; dialog::dialogflags |= sm::SIDE; });

  dialog::addColorItem(XLAT("borders"), bordcolor << 8, 'o');
  dialog::add_action([] () { dialog::openColorDialog(bordcolor); dialog::colorAlpha = false; dialog::dialogflags |= sm::SIDE; });

  dialog::addColorItem(XLAT("projection boundary"), ringcolor, 'r');
  dialog::add_action([] () { dialog::openColorDialog(ringcolor); dialog::dialogflags |= sm::SIDE; });

  dialog::addSelItem(XLAT("boundary width multiplier"), fts(vid.multiplier_ring), 'R');
  dialog::add_action([] () { dialog::editNumber(vid.multiplier_ring, 0, 10, 1, 1, XLAT("boundary width multiplier"), ""); });

  dialog::addColorItem(XLAT("projection background"), modelcolor, 'c');
  dialog::add_action([] () { dialog::openColorDialog(modelcolor); dialog::dialogflags |= sm::SIDE; });

  dialog::addColorItem(XLAT("standard grid color"), stdgridcolor, 'g');
  dialog::add_action([] () { vid.grid = true; dialog::openColorDialog(stdgridcolor); dialog::dialogflags |= sm::SIDE; });
  
  dialog::addSelItem(XLAT("grid width multiplier"), fts(vid.multiplier_grid), 'G');
  dialog::add_action([] () { dialog::editNumber(vid.multiplier_grid, 0, 10, 1, 1, XLAT("grid width multiplier"), ""); });

  dialog::addSelItem(XLAT("brightness behind the sphere"), fts(backbrightness), 'i');
  dialog::add_action([] () { dialog::editNumber(backbrightness, 0, 1, .01, 0.25, XLAT("brightness behind the sphere"), 
    XLAT("In the orthogonal projection, objects on the other side of the sphere are drawn darker.")); dialog::bound_low(0); });

  dialog::addColorItem(XLAT("projection period"), periodcolor, 'p');
  dialog::add_action([] () { dialog::openColorDialog(periodcolor); dialog::dialogflags |= sm::SIDE; });

  dialog::addColorItem(XLAT("dialogs"), dialog::dialogcolor << 8, 'd');
  dialog::add_action([] () { dialog::openColorDialog(dialog::dialogcolor); dialog::colorAlpha = false; dialog::dialogflags |= sm::SIDE; });

  dialog::addBreak(50);
  if(specialland == laCanvas && colortables.count(patterns::whichCanvas)) {
    dialog::addItem(XLAT("pattern colors"), 'P');
    dialog::add_action_push([] { edit_color_table(colortables[patterns::whichCanvas], refresh_canvas, true); });
    }
 
  if(cwt.at->land == laMinefield) {
    dialog::addItem(XLAT("minefield colors"), 'm');
    dialog::add_action_push([] { edit_color_table(minecolors); });
    }
  
  if(viewdists) {
    dialog::addItem(XLAT("distance colors"), 'd');
    dialog::add_action_push([] () {edit_color_table(distcolors); });
    }
  
  #if CAP_CRYSTAL
  if(cryst && cheater) {
    dialog::addItem(XLAT("crystal coordinate colors"), 'C');
    dialog::add_action([] () { crystal::view_coordinates = true; pushScreen([] () { edit_color_table(crystal::coordcolors); });});
    }
  #endif

  if(cwt.at->land == laTortoise) {
    dialog::addBoolItem_action(XLAT("Galápagos shading"), tortoise::shading_enabled, 'T');
    }

  dialog::addInfo(XLAT("colors of some game objects can be edited by clicking them."));
  
  dialog::addBreak(50);

  dialog::addSelItem(XLAT("aura brightness"), its(vid.aurastr), 'a');
  dialog::add_action([] () { dialog::editNumber(vid.aurastr, 0, 256, 10, 128, XLAT("aura brightness"), ""); dialog::bound_low(0); });

  dialog::addSelItem(XLAT("aura smoothening factor"), its(vid.aurasmoothen), 's');
  dialog::add_action([] () { dialog::editNumber(vid.aurasmoothen, 1, 180, 1, 5, XLAT("aura smoothening factor"), ""); dialog::bound_low(1); });  

  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();

  keyhandler = [] (int sym, int uni) {
    if(uni == '-') {
      cell *c = mouseover;
      if(!c) return;
      else if(c == cwt.at) {
        pushScreen(showCustomizeChar);
        return;
        }
      else if(c->monst) 
        dialog::openColorDialog(minf[c->monst].color);
      else if(c->item) 
        dialog::openColorDialog(iinf[c->item].color);
      else if(c->wall) 
        dialog::openColorDialog(winf[c->wall == waMineMine ? waMineUnknown : c->wall].color);
      #if CAP_COMPLEX2
      else if(c->land == laBrownian) 
        dialog::openColorDialog(brownian::get_color_edit(c->landparam));
      #endif
      else 
        dialog::openColorDialog(floorcolors[c->land]);
      dialog::colorAlpha = false;
      dialog::dialogflags |= sm::SIDE;
      return;
      }
    else dialog::handleNavigation(sym, uni);
    if(doexiton(sym, uni)) popScreen();
    };
  }

#if CAP_CONFIG
EX void resetConfigMenu() {
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
      printf("resetting config\n");
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
EX void selectLanguageScreen() {
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

EX void configureMouse() {
  gamescreen(1);
  dialog::init(XLAT("mouse & touchscreen"));

  dialog::addBoolItem_action(XLAT("reverse pointer control"), (vid.revcontrol), 'r');
  
  dialog::addBoolItem_action(XLAT("draw circle around the target"), (vid.drawmousecircle), 'd');
  
  if(GDIM == 3) {
    dialog::addBoolItem_action(XLAT("highlight the cell forward"), vid.axes3, 'f');
    }
  
#if ISMOBILE
  dialog::addBoolItem(XLAT("targetting ranged Orbs long-click only"), (vid.shifttarget&2), 'i');
#else
  dialog::addBoolItem(XLAT("targetting ranged Orbs Shift+click only"), (vid.shifttarget&1), 'i');
#endif
  dialog::add_action([] {vid.shifttarget = vid.shifttarget^3; });    

  #if !ISMOBILE
  dialog::addBoolItem_action(XLAT("quick mouse"), vid.quickmouse, 'M');
  #endif

  dialog::addSelItem(XLAT("move by clicking on compass"), its(vid.mobilecompasssize), 'C');
  dialog::add_action([] {
    dialog::editNumber(vid.mobilecompasssize, 0, 100, 10, 20, XLAT("compass size"), XLAT("0 to disable"));
    // we need to check the moves
    dialog::reaction = checkmove;
    dialog::bound_low(0);
    });

  #if CAP_ORIENTATION
  if(GDIM == 2) {
    dialog::addSelItem(XLAT("scrolling by device rotation"), ors::choices[ors::mode], '1');  
    dialog::add_action_push(ors::show);
    }
  #endif

  dialog::addBack();
  dialog::display();
  }

EX void showSettings() {
  gamescreen(1);
  dialog::init(XLAT("settings"));

  dialog::addItem(XLAT("interface"), 'i');
  dialog::add_action_push(configureInterface);

  dialog::addItem(XLAT("general graphics"), 'g');
  dialog::add_action_push(showGraphConfig);

  dialog::addItem(XLAT("3D configuration"), '9');
  dialog::add_action_push(show3D);

  dialog::addItem(XLAT("quick options"), 'q');
  dialog::add_action_push(showGraphQuickKeys);

  dialog::addItem(XLAT("models & projections"), 'p');
  dialog::add_action_push(models::model_menu);

  dialog::addItem(XLAT("colors & aura"), 'c');
  dialog::add_action_push(show_color_dialog);

#if CAP_SHMUP && !ISMOBILE
  dialog::addSelItem(XLAT("keyboard & joysticks"), "", 'k');
  dialog::add_action(multi::configure);
#endif

  dialog::addSelItem(XLAT("mouse & touchscreen"), "", 'm');
  dialog::add_action_push(configureMouse);

  dialog::addItem(XLAT("other settings"), 'o');
  dialog::add_action_push(configureOther);
  
  dialog::addBreak(100);

#if CAP_CONFIG
  dialog::addItem(XLAT("save the current config"), 's');
  dialog::add_action(saveConfig);

  dialog::addItem(XLAT("reset all configuration"), 'R');
  dialog::add_action_push(resetConfigMenu);
#endif  
  
  if(getcstat == 's') mouseovers = XLAT("Config file: %1", conffile);
  
  dialog::addBack();
  dialog::display();
  }

#if CAP_COMMANDLINE

EX int read_color_args() {
  using namespace arg;

  if(argis("-back")) {
    PHASEFROM(2); shift(); backcolor = arghex();
    }
  else if(argis("-fillmodel")) {
    PHASEFROM(2); shift(); modelcolor = arghex();
    }
  else if(argis("-ring")) {
    PHASEFROM(2); shift(); ringcolor = arghex();
    }
  else if(argis("-ringw")) {
    PHASEFROM(2); shift_arg_formula(vid.multiplier_ring);
    }
  else if(argis("-stdgrid")) {
    PHASEFROM(2); shift(); stdgridcolor = arghex();
    }
  else if(argis("-gridw")) {
    PHASEFROM(2); shift_arg_formula(vid.multiplier_grid);
    }
  else if(argis("-period")) {
    PHASEFROM(2); shift(); periodcolor = arghex();
    }
  else if(argis("-crosshair")) {
    PHASEFROM(2); shift(); crosshair_color = arghex();
    shift_arg_formula(crosshair_size);
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
  else if(argis("-d:color"))
    launch_dialog(show_color_dialog);
  else return 1;
  return 0;
  }

EX int read_config_args() {
  using namespace arg;

  if(argis("-c")) { PHASE(1); shift(); conffile = argcs(); }
// change the configuration from the command line
  else if(argis("-aa")) { PHASEFROM(2); shift(); vid.antialias = argi(); }
  else if(argis("-lw")) { PHASEFROM(2); shift_arg_formula(vid.linewidth); }
  else if(argis("-wm")) { PHASEFROM(2); shift(); vid.wallmode = argi(); }
  else if(argis("-mm")) { PHASEFROM(2); shift(); vid.monmode = argi(); }

  else if(argis("-noshadow")) { noshadow = true; }
  else if(argis("-bright")) { bright = true; }
  else if(argis("-gridon")) { vid.grid = true; }
  else if(argis("-gridoff")) { vid.grid = false; }

// non-configurable options
  else if(argis("-vsync_off")) {
    #if CAP_SDL && CAP_GL
    vsync_off = true;
    if(curphase == 3) setvideomode();
    #endif
    }
  else if(argis("-aura")) {
    PHASEFROM(2);
    shift(); vid.aurastr = argi();
    shift(); vid.aurasmoothen = argi();
    }
  else if(argis("-nofps")) {
    PHASEFROM(2);
    nofps = true;
    }
  else if(argis("-nohud")) {
    PHASEFROM(2);
    nohud = true;
    }
  else if(argis("-nomenu")) {
    PHASEFROM(2);
    nomenukey = true;
    }
#if MAXMDIM >= 4
  else if(argis("-switch-fpp")) {
    PHASEFROM(2);
    geom3::switch_fpp();
    }
#endif
  else if(argis("-switch-tpp")) {
    PHASEFROM(2);
    geom3::switch_tpp();
    }
#if MAXMDIM >= 4
  else if(argis("-switch-3d")) {
    PHASEFROM(2);
    geom3::switch_always3();
    }
#endif
  else if(argis("-nohelp")) {
    PHASEFROM(2);
    nohelp = true;
    }
  else if(argis("-dont_face_pc")) {
    PHASEFROM(2);
    dont_face_pc = true;
    }

#if CAP_TRANS
  else if(argis("-lang")) { 
    PHASEFROM(2); shift(); vid.language = argi();
    }
#endif
  else if(argis("-vlq")) { 
    PHASEFROM(2); shift(); vid.linequality = argi();
    }
  else if(argis("-fov")) { 
    PHASEFROM(2); shift_arg_formula(vid.fov);
    }
  else if(argis("-r")) { 
    PHASEFROM(2);
    shift(); 
    int clWidth=0, clHeight=0, clFont=0;
    sscanf(argcs(), "%dx%dx%d", &clWidth, &clHeight, &clFont);
    if(clWidth) vid.xres = clWidth;
    if(clHeight) vid.yres = clHeight;
    if(clFont) vid.fsize = clFont;
    }    
  else if(argis("-msm")) {
    PHASEFROM(2); memory_saving_mode = true;
    }
  else if(argis("-mrsv")) {
    PHASEFROM(2); shift(); reserve_limit = argi(); apply_memory_reserve();
    }
  else if(argis("-yca")) {
    PHASEFROM(2); 
    shift_arg_formula(vid.yshift);
    shift_arg_formula(pconf.camera_angle);
    }
  else if(argis("-pside")) {
    PHASEFROM(2); 
    permaside = true;
    }
  else if(argis("-xy")) {
    PHASEFROM(2); 
    shift_arg_formula(pconf.xposition);
    shift_arg_formula(pconf.yposition);
    }
  else if(argis("-fixdir")) {
    PHASEFROM(2); 
    vid.fixed_facing = true;
    shift_arg_formula(vid.fixed_facing_dir);
    }
  else if(argis("-fixdiroff")) {
    PHASEFROM(2); 
    vid.fixed_facing = false;
    }
  else if(argis("-msmoff")) {
    PHASEFROM(2); memory_saving_mode = false;
    }
  else if(argis("-levellines")) {
    PHASEFROM(2); shift_arg_formula(levellines);
    }
  else if(argis("-level-notexture")) {
    PHASEFROM(2); disable_texture = true;
    }
  else if(argis("-level-texture")) {
    PHASEFROM(2); disable_texture = false;
    }
  else if(argis("-msens")) {
    PHASEFROM(2); shift_arg_formula(mouseaim_sensitivity);
    }
  TOGGLE('o', vid.usingGL, switchGL())
  TOGGLE('f', vid.full, switchFullscreen())
  else if(argis("-noshaders")) {
    PHASE(1);
    glhr::noshaders = true; 
    }
  else if(argis("-d:sight")) {
    PHASEFROM(2); launch_dialog(); edit_sightrange();
    }
  else if(argis("-d:char")) {
    PHASEFROM(2); launch_dialog(showCustomizeChar);
    }
  else if(argis("-d:3")) {
    PHASEFROM(2); launch_dialog(show3D);
    }
  else if(argis("-d:stereo")) {
    PHASEFROM(2); launch_dialog(showStereo);
    }
  else if(argis("-d:iface")) {
    PHASEFROM(2); launch_dialog(configureInterface);
    }
  else if(argis("-d:graph")) {
    PHASEFROM(2); launch_dialog(showGraphConfig);
    }
  else if(argis("-tstep")) {
    PHASEFROM(2); shift(); vid.texture_step = argi();
    }
  else if(argis("-csc")) {
    PHASEFROM(2); shift_arg_formula(vid.creature_scale);
    }
  else if(argis("-neon")) {
    PHASEFROM(2);
    shift(); neon_mode = eNeon(argi());
    }
  else if(argis("-smooths")) {
    PHASEFROM(2);
    shift(); smooth_scrolling = argi();
    }
  else if(argis("-via-shader")) {
    PHASEFROM(2);
    shift(); vid.consider_shader_projection = argi();
    }
  else if(argis("-neonnf")) {
    PHASEFROM(2);
    shift(); neon_nofill = argi();
    }
  else if(argis("-precw")) {
    PHASEFROM(2);
    shift_arg_formula(precise_width);
    }
  else if(argis("-char")) {
    auto& cs = vid.cs;
    shift(); cs.charid = argi();
    cs.lefthanded = cs.charid >= 10;
    cs.charid %= 10;
    }
  else return 1;
  return 0;
  }

// mode changes:

EX int read_gamemode_args() {
  using namespace arg;

  if(argis("-P")) { 
    PHASE(2); shift(); 
    stop_game_and_switch_mode(rg::nothing);
    multi::players = argi();
    }
  TOGGLE('C', chaosmode, stop_game_and_switch_mode(rg::chaos))
  TOGGLE('S', shmup::on, stop_game_and_switch_mode(rg::shmup))
  TOGGLE('H', hardcore, switchHardcore())
  TOGGLE('R', randomPatternsMode, stop_game_and_switch_mode(rg::randpattern))
  TOGGLE('i', inv::on, stop_game_and_switch_mode(rg::inv))
  
  else return 1;
  return 0;
  }

auto ah_config = addHook(hooks_args, 0, read_config_args) + addHook(hooks_args, 0, read_gamemode_args) + addHook(hooks_args, 0, read_color_args);
#endif

EX unordered_map<string, ld&> params = {
  {"linewidth", vid.linewidth},
  {"patternlinewidth", linepatterns::width},
  {"scale", pconf.scale},
  {"xposition", pconf.xposition},
  {"yposition", pconf.yposition},
  {"projection", pconf.alpha},
  {"sspeed", vid.sspeed},
  {"mspeed", vid.mspeed},
  {"ballangle", pconf.ballangle},
  {"yshift", vid.yshift},
  {"cameraangle", pconf.camera_angle},
  {"eye", vid.eye},
  {"depth", vid.depth},
  {"camera", vid.camera},
  {"wall_height", vid.wall_height},
  {"highdetail", vid.highdetail},
  {"middetail", vid.middetail},
  {"rock_wall_ratio", vid.rock_wall_ratio},
  {"human_wall_ratio", vid.human_wall_ratio},
  {"lake_top", vid.lake_top},
  {"lake_bottom", vid.lake_bottom},
  #if CAP_RUG
  {"rug_model_distance", rug::model_distance},
  #endif
  {"star", polygonal::STAR},
  {"lvspeed", history::lvspeed},
  {"rotation", models::rotation},
  {"mori", pconf.model_orientation},
  {"mori_yz", pconf.model_orientation_yz},
  {"clipmin", pconf.clip_min},
  {"clipmax", pconf.clip_max},
  {"topz", pconf.top_z},
  {"mtrans", pconf.model_transition},
  {"hp", pconf.halfplane_scale},
  {"back", backbrightness},
  {"ipd", vid.ipd},
  {"lr", vid.lr_eyewidth},
  {"anaglyph", vid.anaglyph_eyewidth},
  {"fov", vid.fov},
  {"ets", pconf.euclid_to_sphere},
  {"stretch", pconf.stretch},
  {"twopoint", pconf.twopoint_param},
  {"fisheye", pconf.fisheye_param},
  {"bwidth", vid.binary_width},
  #if CAP_ANIMATIONS
  {"aperiod", anims::period},
  {"acycle", anims::cycle_length},
  {"aparabolic", anims::parabolic_length},
  {"arugangle", anims::rug_angle},
  {"acradius", anims::circle_radius},
  {"acspins", anims::circle_spins},
  {"a", anims::a},
  {"b", anims::b},
  #endif
  {"mobius", pconf.skiprope},
  {"sang", pconf.spiral_angle},
  {"spiralx", pconf.spiral_x},
  {"spiraly", pconf.spiral_y},
  #if CAP_CRYSTAL
  {"cprob", crystal::compass_probability},
  #endif
  #if CAP_SHOT
  {"gamma", shot::gamma},
  {"fade", shot::fade},
  {"mgrid", vid.multiplier_grid},
  {"mring", vid.multiplier_ring},
  {"collignon", pconf.collignon_parameter},
  {"aitoff", pconf.aitoff_parameter},
  {"loxidromic", pconf.loximuthal_parameter},
  {"miller", pconf.miller_parameter},
  {"winkel", pconf.winkel_parameter},
  {"camspd", camera_speed},
  {"camrot", camera_rot_speed},
  {"levellines", levellines},
  #endif
  };

}
