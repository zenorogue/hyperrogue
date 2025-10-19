// Hyperbolic Rogue -- old config file format (disabled by default)
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

#include "hyper.h"
namespace hr {

#if CAP_LEGACY

#if HDR
/** legacy name */
constexpr eGeometry gRotSpace = gTwistedProduct;
#endif

namespace shmup {

void scanchar(FILE *f, char& c) {
  int i = c;
  int err = fscanf(f, "%d", &i);
  if(err == 1) c = i;
  }
  
void loadConfig(FILE *f) {
  int xvernum;
  int err = fscanf(f, "%d %d", &xvernum, &multi::players);
  if(multi::players < 1 || multi::players > MAXPLAYER)
    multi::players = 1;
  if(err != 2) return;
  if(xvernum >= 8990) { int b=alwaysuse; err=fscanf(f, " %d", &b); alwaysuse = b; }
  for(int i=0; i<512; i++) scanchar(f, vid.scfg.keyaction[i]);
  for(int i=0; i<MAXJOY; i++) for(int j=0; j<MAXBUTTON; j++) scanchar(f, vid.scfg.joyaction[i][j]);
  for(int i=0; i<MAXJOY; i++) for(int j=0; j<MAXAXE; j++) scanchar(f, vid.scfg.axeaction[i][j]);
  
  if(xvernum >= 9007)
    for(int i=0; i<MAXJOY; i++) for(int j=0; j<MAXAXE; j++) err = fscanf(f, " %d", &vid.scfg.deadzoneval[i][j]);
  for(int i=0; i<MAXJOY; i++) for(int j=0; j<MAXHAT; j++) for(int k=0; k<4; k++)
    scanchar(f, vid.scfg.hataction[i][j][k]);
  for(int i=0; i<(xvernum < 8990 ? 4 : 7); i++) loadcs(f, scs[i], xvernum);
  }

}

void loadcs(FILE *f, charstyle& cs, int xvernum) {
  int gflags, err = 
  fscanf(f, "%d%d%x%x%x%x", &gflags, &vid.language, &cs.skincolor, &cs.haircolor, &cs.swordcolor, &cs.dresscolor);

  if(err) cs.charid = gflags & 15;
  if(err) vid.samegender = (gflags & 16) ? true : false;
  if(cs.charid == 3) hr::ignore(fscanf(f, "%x", &cs.dresscolor2));
  if(xvernum >= 8990) hr::ignore(fscanf(f, "%x", &cs.uicolor));
  }

void loadOldConfig(FILE *f) {
  int gl=1, aa=1, bb=1, cc, dd;
  int err;
  float a, b, c, d;
  err=fscanf(f, "%f%f%f%f\n", &a, &b, &c, &d);
  if(err == 4) {
    vid.scale = a; pconf.alpha = c; vid.sspeed = d;
    }
  err=fscanf(f, "%d%d%d%d%d%d%d", &vid.wallmode, &vid.monmode, &vid.axes, &musicvolume, &vid.framelimit, &gl, &vid.want_antialias);
  vid.usingGL = gl;
  if(vid.want_antialias == 0) vid.want_antialias = AA_VERSION | AA_LINES | AA_LINEWIDTH;
  if(vid.want_antialias == 1) vid.want_antialias = AA_NOGL | AA_VERSION | AA_LINES | AA_LINEWIDTH | AA_FONT;
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

  aa = rug::renderonce; bb = rug::rendernogl; dd = ls::any_chaos();
  int ee = vid.steamscore;
  #if CAP_RUG
  double rs = 2/rug::model_distance;
  #else
  double rs = 0;
  #endif
  err=fscanf(f, "%d%d%d%d%lf%d%d", &aa, &bb, &rug::texturesize, &cc, &rs, &ee, &dd);
  rug::renderonce = aa; rug::rendernogl = bb; 
  land_structure = (eLandStructure) dd; vid.steamscore = ee;
  #if CAP_RUG
  rug::model_distance = 2/rs;
  #endif

  aa=history::autobandhistory;
  double ps = polygonal::STAR, lv = history::lvspeed;
  int pmb = pmodel;
  err=fscanf(f, "%d%d%lf%d%d%lf",
    &pmb, &polygonal::SI, &ps, &polygonal::deg,
    &aa, &lv);
  pmodel = eModel(pmb);
  history::autobandhistory = aa; polygonal::STAR = ps; history::lvspeed = lv;
  
  aa=history::autoband; bb=history::autobandhistory; cc=history::dospiral;    
  int crot;
  err=fscanf(f, "%d%d%d%d%d%d", 
    &history::bandhalf, &history::bandsegment, &crot,
    &aa, &bb, &cc);
  history::autoband = aa; history::autobandhistory = bb; history::dospiral = cc;
  models::rotation = crot * 90;
  
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
#endif

// Identifiers for the current combinations of game modes
// These are recorded in the save file, so it is somewhat
// important that they do not change for already existing
// modes, as otherwise the scores would be lost. 
// Unfortunately, the codes assigned when HyperRogue had
// just a few special modes did not really follow a specific
// rule, so this system has grown rather ugly as the
// number of special modes kept growing...

// mode codes for 'old' modes, from 0 to 255

int modecodetable[42][6] = {
  {  0, 38, 39, 40, 41, 42}, // softcore hyperbolic
  {  7, 43, 44, 45, 46, 47}, // hardcore hyperbolic
  {  2,  4,  9, 11, 48, 49}, // shmup hyperbolic
  { 13, 50, 51, 52, 53, 54}, // softcore heptagonal hyperbolic
  { 16, 55, 56, 57, 58, 59}, // hardcore heptagonal hyperbolic
  { 14, 15, 17, 18, 60, 61}, // shmup heptagonal hyperbolic
  {  1, 62, 63, 64, 65, 66}, // softcore euclidean
  {  8, 67, 68, 69, 70, 71}, // hardcore euclidean
  {  3,  5, 10, 12, 72, 73}, // shmup euclidean
  {110,111,112,113,114,115}, // softcore spherical
  {116,117,118,119,120,121}, // hardcore spherical
  {122,123,124,125,126,127}, // shmup spherical
  {128,129,130,131,132,133}, // softcore heptagonal spherical
  {134,135,136,137,138,139}, // hardcore heptagonal spherical
  {140,141,142,143,144,145}, // shmup heptagonal spherical
  {146,147,148,149,150,151}, // softcore elliptic
  {152,153,154,155,156,157}, // hardcore elliptic
  {158,159,160,161,162,163}, // shmup elliptic
  {164,165,166,167,168,169}, // softcore heptagonal elliptic
  {170,171,172,173,174,175}, // hardcore heptagonal elliptic
  {176,177,178,179,180,181}, // shmup heptagonal elliptic
  { 19, 74, 75, 76, 77, 78}, // softcore hyperbolic chaosmode
  { 26, 79, 80, 81, 82, 83}, // hardcore hyperbolic chaosmode
  { 21, 23, 28, 30, 84, 85}, // shmup hyperbolic chaosmode
  { 32, 86, 87, 88, 89, 90}, // softcore heptagonal hyperbolic chaosmode
  { 35, 91, 92, 93, 94, 95}, // hardcore heptagonal hyperbolic chaosmode
  { 33, 34, 36, 37, 96, 97}, // shmup heptagonal hyperbolic chaosmode
  { 20, 98, 99,100,101,102}, // softcore euclidean chaosmode
  { 27,103,104,105,106,107}, // hardcore euclidean chaosmode
  { 22, 24, 29, 31,108,109}, // shmup euclidean chaosmode
  {182,183,184,185,186,187}, // softcore spherical chaosmode
  {188,189,190,191,192,193}, // hardcore spherical chaosmode
  {194,195,196,197,198,199}, // shmup spherical chaosmode
  {200,201,202,203,204,205}, // softcore heptagonal spherical chaosmode
  {206,207,208,209,210,211}, // hardcore heptagonal spherical chaosmode
  {212,213,214,215,216,217}, // shmup heptagonal spherical chaosmode
  {218,219,220,221,222,223}, // softcore elliptic chaosmode
  {224,225,226,227,228,229}, // hardcore elliptic chaosmode
  {230,231,232,233,234,235}, // shmup elliptic chaosmode
  {236,237,238,239,240,241}, // softcore heptagonal elliptic chaosmode
  {242,243,244,245,246,247}, // hardcore heptagonal elliptic chaosmode
  {248,249,250,251,252,253}, // shmup heptagonal elliptic chaosmode
  };
// unused codes: 6 (cheat/tampered), 25, 254, 255

EX modecode_t legacy_modecode() {
  if(int(geometry) > 3 || int(variation) > 1) return UNKNOWN;
  if(casual) return UNKNOWN;
  if(bow::weapon) return UNKNOWN;
  if(use_custom_land_list) return UNKNOWN;

  bool is_default_land_structure = land_structure == get_default_land_structure();

  if(!is_default_land_structure && !ls::std_chaos()) return UNKNOWN;

  // compute the old code
  int xcode = 0;

  if(shmup::on) xcode += 2;
  else if(pureHardcore()) xcode ++;
  
  if(euclid) xcode += 6;
  else if(!BITRUNCATED) xcode += 3;
  
  if(sphere) {
    xcode += 9;
    if(elliptic) xcode += 6;
    }
  
  if(ls::any_chaos() && !yendor::on) xcode += 21;
  
  int np = numplayers()-1; if(np<0 || np>5) np=5;

  // bits: 0 to 7
  modecode_t mct = modecodetable[xcode][np];

#if CAP_INV
  if(inv::on) mct += (1<<11);
#endif
  if(peace::on) mct += (1<<12);
#if CAP_TOUR
  if(tour::on) mct += (1<<13);
#endif
  if(numplayers() == 7) mct += (1<<14);
  
  return mct;
  }

EX void legacy_modecode_read(modecode_t mc) {
  if(mc >= FIRST_MODECODE) throw hr_exception("not a legacy modecode");

  inv::on = (mc & (1<<11));
  peace::on = (mc & (1<<12));
  tour::on = (mc & (1<<13));
  bool seven = (mc & (1<<14));
  mc &= (1<<11)-1;

  for(int xcode=0; xcode<42;xcode++) for(int pl=0; pl<6; pl++) if(modecodetable[xcode][pl] == mc) {
    if(seven) pl = 7; else pl++;
    multi::players = pl;
    land_structure = xcode >= 21 ? lsChaos : get_default_land_structure();
    xcode %= 21;
    shmup::on = 0; hardcore = false;
    if(xcode % 3 == 2) shmup::on = true;
    if(xcode % 3 == 1) hardcore = true;
    xcode /= 3;
    geometry = gNormal; variation = eVariation::bitruncated;
    if(xcode == 1) variation = eVariation::pure;
    if(xcode == 2) geometry = gEuclid;
    if(xcode == 3) geometry = gSphere;
    if(xcode == 4) geometry = gSphere, variation = eVariation::pure;
    if(xcode == 5) geometry = gElliptic;
    if(xcode == 6) geometry = gElliptic, variation = eVariation::pure;

    use_custom_land_list = false;
    bow::weapon = bow::wBlade;
    casual = false;
    return;
    }

  throw hr_exception("legacy code not recognized");
  }

#if CAP_RACING
EX bool legacy_racing() {
  return racing::on && geometry == gNormal && BITRUNCATED;
  }

EX bool rcheck(string which, int qty, int x) {
  return hrand(qty) < x;
  }

EX int wallchance_legacy(cell *c, bool deepOcean) {
  eLand l = c->land;
  return
    inmirror(c) ? 0 :
    isElemental(l) ? 4000 :
    l == laCrossroads ? 5000 :
    (l == laMirror && !yendor::generating) ? 6000 :
    l == laTerracotta ? 250 :
    0;
  }

EX void buildBigStuff_legacy(cell *c, cell *from) {
  int chaosmode = 0;

  bool deepOcean = false;

  if(geometry == gNormal && celldist(c) < 3 && !GOLDBERG) {
    if(top_land && c == cwt.at->master->move(3)->c7) {
      buildBarrierStrong(c, 6, true, top_land);
      }
    }

  else if(good_for_wall(c) && rcheck("D", 10000, 20) && !generatingEquidistant && !yendor::on && !tactic::on && !racing::on) {}

  else if(ctof(c) && c->land && rcheck("F", 10000, wallchance_legacy(c, deepOcean))) {
    int bd = 2 + hrand(2) * 3;
    buildBarrier(c, bd);
    }

  if((!chaosmode) && bearsCamelot(c->land) && is_master(c) && !bt::in() &&
    (quickfind(laCamelot) || peace::on || (hrand(2000) < (c->land == laCrossroads4 ? 800 : 200) && horo_ok() && false))) {
    }

  if(!chaosmode && c->land == laJungle && ctof(c) &&
    (quickfind(laMountain) || (hrand(2000) < 100 && horo_ok() &&
    !randomPatternsMode && !tactic::on && !yendor::on && !racing::on && landUnlocked(laMountain)))) {}

  if(hasbardir(c)) extendBarrier(c);
  }
#endif

#if CAP_COMMANDLINE
/* legacy options */
int read_legacy_args() {
  using namespace arg;
  if(0);
  else if(argis("-chaos-circle")) {
    PHASEFROM(2);
    stop_game();
    land_structure = lsPatchedChaos;
    }
  else if(argis("-chaos-total")) {
    PHASEFROM(2);
    stop_game();
    land_structure = lsTotalChaos;
    }
  else if(argis("-chaos-random")) {
    PHASEFROM(2);
    stop_game();
    land_structure = lsChaosRW;
    }
  else if(argis("-C") || argis("-C1")) {
    PHASEFROM(2);
    stop_game();
    land_structure = lsChaos;
    }
  else if(argis("-C0")) {
    PHASEFROM(2);
    stop_game();
    land_structure = lsNiceWalls;
    }
  else if(argis("-grotate")) {
    PHASE(3);  start_game();
    shift(); int i = argi();
    shift(); int j = argi();
    shift(); View = View * cspin(i, j, argf());
    playermoved = false;
    }
  else if(argis("-grotatei")) {
    PHASE(3);  start_game();
    shift(); int i = argi();
    shift(); int j = argi();
    shift(); rotate_view(cspin(i, j, argf()));
    playermoved = false;
    }
  else if(argis("-rotate")) {
    PHASE(3);  start_game();
    shift(); ld a = argf();
    shift(); ld b = argf();
    View = View * spin(TAU * a / b);
    playermoved = false;
    }
  else if(argis("-rotate3")) {
    PHASE(3);  start_game();
    shift(); ld a = argf();
    shift(); ld b = argf();
    View = View * cspin(1, 2, TAU * a / b);
    playermoved = false;
    }
  else if(argis("-cview")) {
    PHASE(3);  start_game();
    View = Id; playermoved = false;
    }
  else if(argis("-ballangle")) {
    PHASEFROM(2);
    shift(); println(hlog, "-ballangle not implemented");
    // shift_arg_formula(vpconf.ballangle);
    }
  else if(argis("-crot")) {
    PHASEFROM(2);
    shift(); shift(); shift(); println(hlog, "-crot not implemented");
    /* shift_arg_formula(models::rotation);
    if(GDIM == 3) shift_arg_formula(models::rotation_xz);
    if(GDIM == 3) shift_arg_formula(models::rotation_xy2); */
    }
  else if(argis("-nilh")) {
    PHASEFROM(2);
    stop_game();
    shift();
    nilv::nil_structure_index = argi() == 8 ? 1 : 0;
    nilv::set_flags();
    start_game();
    }
  else if(argis("-yca")) {
    PHASEFROM(2);
    shift_arg_formula(vid.yshift);
    shift(); ld angle = argf();
    pconf.cam() = cspin(0, 2, angle);
    }
  else return 1;
  return 0;
  }

struct legacy_flag {
  char key;
  int level;
  debugflag *df;
  };

vector<legacy_flag> legacy_flags = {
  // INIT
  legacy_flag{'i', 1, &debug_init},
  legacy_flag{'i', 1, &debug_init_music},
  legacy_flag{'i', 1, &debug_init_graph},
  legacy_flag{'i', 1, &debug_init_joy},
  legacy_flag{'i', 1, &debug_init_cells},
  legacy_flag{'i', 1, &debug_init_config},
  legacy_flag{'i', 1, &debug_init_font},
  // MSG
  legacy_flag{'m', 1, &debug_messages},
  // WARN
  legacy_flag{'w', 1, &debug_warnings},
  legacy_flag{'w', 1, &debug_map_warnings},
  // ERROR
  legacy_flag{'e', 1, &debug_errors},
  legacy_flag{'e', 1, &debug_music_error},
  legacy_flag{'e', 1, &debug_joy_error},
  // STEAM
  legacy_flag{'s', 3, &debug_achievements},
  // GRAPH
  legacy_flag{'x', 4, &debug_aura},
  legacy_flag{'x', 4, &debug_map},
  legacy_flag{'x', 4, &debug_calcparam},
  legacy_flag{'x', 4, &debug_graph},
  legacy_flag{'x', 4, &debug_joy},
  legacy_flag{'x', 4, &debug_control},
  // TURN
  legacy_flag{'u', 3, &debug_turn},
  // FIELD
  legacy_flag{'f', 2, &fieldpattern::debug_field},
  // GEOM
  legacy_flag{'g', 2, &debug_geometry},
  legacy_flag{'g', 2, &debug_map_create},
  legacy_flag{'g', 4, &arcm::debug_archimedean_map},
  // MEMORY
  legacy_flag{'b', 4, &debug_memory},
  legacy_flag{'b', 4, &debug_graph_memory},
  legacy_flag{'b', 4, &debug_memory_cell},
  // TIME
  legacy_flag{'t', 9, &debug_stamps},
  // GP
  legacy_flag{'o', 2, &gp::debug_gp},
  // POLY
  legacy_flag{'p', 2, &debug_poly},
  // VERTEX
  legacy_flag{'v', 9, &debug_vertex}
  };

void set_legacy_flags(char key = 0, int level = 0, bool val = true) {
  for(auto& lf: legacy_flags) if((!key || lf.key == key) && lf.level <= level && lf.df)
    lf.df->enabled = val;
  }

int read_legacy_args_anim() {
  using namespace anims;
  using namespace arg;
  ld movement_angle, shift_angle, rug_angle; /* TODO link to anims::angle's */
  if(0);
  else if(argis("-animperiod")) {
    PHASEFROM(2); shift_arg_formula(period);
    }
  else if(argis("-animcircle")) {
    PHASE(3); start_game();
    ma = maCircle; ma_reaction();
    shift_arg_formula(circle_spins);
    shift_arg_formula(circle_radius);
    shift(); circle_display_color = argcolor(24);
    }
  else if(argis("-animmove")) {
    ma = maTranslation; 
    shift_arg_formula(cycle_length);
    shift_arg_formula(shift_angle);
    shift_arg_formula(movement_angle);
    }
  else if(argis("-animmoverot")) {
    ma = maTranslationRotation; 
    shift_arg_formula(cycle_length);
    shift_arg_formula(shift_angle);
    shift_arg_formula(movement_angle);
    }
  else if(argis("-wallopt")) {
    wallopt = true;
    }
  else if(argis("-animpar")) {
    ma = maParabolic; 
    shift_arg_formula(parabolic_length);
    shift_arg_formula(shift_angle);
    shift_arg_formula(movement_angle);
    }
  else if(argis("-animclear")) { clearup = true; }
  else if(argis("-animrot")) {
    ma = maRotation;
    if(GDIM == 3) {
      shift_arg_formula(movement_angle);
      shift_arg_formula(normal_angle);
      }
    }
  else if(argis("-innerwall")) {
    PHASEFROM(2);
    patterns::innerwalls = true;
    }
  else if(argis("-noinnerwall")) {
    PHASEFROM(2);
    patterns::innerwalls = false;
    }
  else if(argis("-animrotd")) {
    start_game();
    ma = maRotation;
    shift_arg_formula(rotation_distance);
    }
  else if(argis("-animrug")) {
    shift_arg_formula(rug_rotation1);
    shift_arg_formula(rug_angle);
    shift_arg_formula(rug_rotation2);
    }
  else if(argis("-animenv")) {
    shift_arg_formula(env_ocean);
    shift_arg_formula(env_volcano);
    }
  else if(argis("-animball")) {
    shift(); println(hlog, "-animball removed");
    }
  else if(argis("-animj")) {
    shift(); println(hlog, "-animj removed");
/*  to recreate: if(joukowsky_anim) {
    ld t = ticks / period;
    t = t - floor(t);
    if(pmodel == mdBand) {
      vpconf.model_transition = t * 4 - 1;
      }
    else {
      vpconf.model_transition = t / 1.1;
      vpconf.scale = (1 - vpconf.model_transition) / 2.;
      } */
/* skiprope:legacy.cpp  pconf.skiprope += skiprope_rotation * t * TAU / period; */
    }
  else if(argis("-palpha")) {
    PHASEFROM(2);
    #if CAP_GL
    shift_arg_formula(vid.stereo_param, reset_all_shaders);
    #else
    shift_arg_formula(vid.stereo_param);
    #endif
    vid.stereo_mode = sPanini;
    }
  else if(argis("-salpha")) {
    PHASEFROM(2);
    #if CAP_GL
    shift_arg_formula(vid.stereo_param, reset_all_shaders);
    #else
    shift_arg_formula(vid.stereo_param);
    #endif
    vid.stereo_mode = sStereographic;
    }
  else if(argis("-debf")) {
    shift();
    string s = args();
    for(char c: s) {
      if(c >= 'a' && c <= 'z')
        set_legacy_flags(c, 0, true);
      else if(c >= '0' && c <= '9') {
        set_legacy_flags(0, 8, false);
        set_legacy_flags(0, c - '0', true);
        }
      else if(c == '+') {
        if(debugfile) fclose(debugfile);
        shift();
        println(hlog, "writing to ", argcs());
        debugfile = fopen(argcs(), "at");
        }
      else if(c == '@') {
        if(debugfile) fclose(debugfile);
        shift();
        println(hlog, "writing to ", argcs());
        debugfile = fopen(argcs(), "wt");
        }
      }
    }
  else if(argis("-no-stamp")) {
    debug_stamps.flip();
    }
  else return 1;
  return 0;
  }

auto ah_legacy = addHook(hooks_args, 0, read_legacy_args) + addHook(hooks_args, 0, read_legacy_args_anim);
#endif


}
