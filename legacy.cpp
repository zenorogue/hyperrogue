#if CAP_LEGACY
namespace hr {

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

}
#endif
