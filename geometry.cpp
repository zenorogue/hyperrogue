// Hyperbolic Rogue
// geometrical constants

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

// the results are:
// hexf = 0.378077 hcrossf = 0.620672 tessf = 1.090550
// hexhexdist = 0.566256

ld hcrossf7 = 0.620672;
ld hexf7 = 0.378077;

// the distance between two hexagon centers

void geometry_information::prepare_basics() {

  DEBBI(DF_INIT | DF_POLY | DF_GEOM, ("prepare_basics"));
  
  hexshift = 0;

  ld fmin, fmax;  

  if(archimedean) 
    ginf[gArchimedean].cclass = gcHyperbolic;

  if(euclid) {
    // dynamicval<eGeometry> g(geometry, gNormal);
    // for(int i=0; i<S84; i++) spinmatrix[i] = spin(i * M_PI / S42);
    if(a4 && !BITRUNCATED) {
      crossf = .5;
      hexf = .5;
      hcrossf = crossf * sqrt(2) / 2;
      hexhexdist = crossf;
      hexvdist = hexf;
      hepvdist = hexf;
      rhexf = crossf * sqrt(2) / 2;
      tessf = crossf;
      }
    else if(a4 && BITRUNCATED) {
      ld s2 = sqrt(2);
      ld xx = 1 - s2 / 2;
      crossf = .5;
      tessf = crossf * s2;
      hexf = .5 * xx * s2;
      hcrossf = crossf;
      hexhexdist = crossf * s2;
      hexvdist = crossf * hypot(1-xx, xx);
      hepvdist = crossf;
      rhexf = hexf;
      tessf = crossf;
      }
    else {
      crossf = .5;
      tessf = crossf * sqrt(3);
      hexf = tessf/3;
      hcrossf = crossf;
      hexhexdist = crossf;
      hexvdist = hexf;
      hepvdist = crossf;
      rhexf = hexf;
      }
    goto finish;
    }
  
  if((sphere || hyperbolic) && WDIM == 3 && !binarytiling) {
    rhexf = hexf = 0.378077;
    crossf = hcrossf = 0.620672;
    tessf = 1.090550;
    hexhexdist = 0.566256;
    goto finish;
    }

  tessf = edge_of_triangle_with_angles(2*M_PI/S3, M_PI/S7, M_PI/S7);
  
  if(elliptic && S7 == 4) tessf = M_PI/2;
  
  hcrossf = edge_of_triangle_with_angles(M_PI/2, M_PI/S7, M_PI/S3);

  crossf = BITRUNCATED ? hcrossf : tessf;
  
  fmin = 0, fmax = tessf;
  for(int p=0; p<100; p++) {
    ld f =  (fmin+fmax) / 2;
    hyperpoint H = xpush0(f);
    hyperpoint H1 = spin(2*M_PI/S7) * H;
    hyperpoint H2 = xpush0(tessf-f);
    ld v1 = intval(H, H1), v2 = intval(H, H2);
    if(v1 < v2) fmin = f; else fmax = f;
    }
  hexf = fmin;
  
  rhexf = BITRUNCATED ? hexf : hcrossf;
  
  if(!euclid && BITRUNCATED && !(S7&1))
    hexshift = ALPHA/2 + ALPHA * ((S7-1)/2) + M_PI;

  finish:
  
  for(int d=0; d<S7; d++)
    heptmove[d] = spin(-d * ALPHA) * xpush(tessf) * spin(M_PI);
    
  for(int d=0; d<S7; d++) 
    hexmove[d] = spin(hexshift-d * ALPHA) * xpush(-crossf)* spin(M_PI);  

  for(int d=0; d<S7; d++) invheptmove[d] = inverse(heptmove[d]);
  for(int d=0; d<S7; d++) invhexmove[d] = inverse(hexmove[d]);

  hexhexdist = hdist(xpush0(crossf), xspinpush0(M_PI*2/S7, crossf));
  
  hexvdist = hdist(xpush0(hexf), xspinpush0(ALPHA/2, hcrossf));

  DEBB(DF_GEOM | DF_POLY,
    (format("S7=%d S6=%d hexf = " LDF" hcross = " LDF" tessf = " LDF" hexshift = " LDF " hexhex = " LDF " hexv = " LDF "\n", S7, S6, hexf, hcrossf, tessf, hexshift, 
    hexhexdist, hexvdist)));  
  
  base_distlimit = ginf[geometry].distlimit[!BITRUNCATED];

  #if CAP_GP
  gp::compute_geometry();  
  #endif
  #if CAP_IRR
  irr::compute_geometry();
  #endif
  #if CAP_ARCM
  if(archimedean) {
    arcm::current.compute_geometry();
    crossf = hcrossf7 * arcm::current.scale();
    hexvdist = arcm::current.scale() * .5;
    rhexf = arcm::current.scale() * .5;
    }
  #endif
  #if CAP_BT
  if(binarytiling) hexvdist = rhexf = 1, tessf = 1, scalefactor = 1, crossf = hcrossf7;
  if(geometry == gHoroRec) hexvdist = rhexf = .5, tessf = .5, scalefactor = .5, crossf = hcrossf7/2;
  #endif
  #if CAP_BT && MAXMDIM >= 4
  if(binarytiling && WDIM == 3) binary::build_tmatrix();
  #endif
  
  scalefactor = crossf / hcrossf7;
  orbsize = crossf;

  if(WDIM == 3) scalefactor *= vid.creature_scale;

  zhexf = BITRUNCATED ? hexf : crossf* .55;
  if(WDIM == 3) zhexf *= vid.creature_scale;
  if(WDIM == 2 && GDIM == 3) zhexf *= 1.5, orbsize *= 1.2;

  floorrad0 = hexvdist* (GDIM == 3 ? 1 : 0.92);
  floorrad1 = rhexf * (GDIM == 3 ? 1 : 0.94);
  
  if(euclid4) {
    if(!BITRUNCATED)
      floorrad0 = floorrad1 = rhexf * (GDIM == 3 ? 1 : .94);
    else
      floorrad0 = hexvdist * (GDIM == 3 ? 1 : .9),
      floorrad1 = rhexf * (GDIM == 3 ? 1 : .8);
    }
  
  set_sibling_limit();
  
  prepare_compute3();
  if(hyperbolic && &currfp != &fieldpattern::fp_invalid)
    currfp.analyze(); 
  }

transmatrix xspinpush(ld dir, ld dist) {
  if(euclid)
    return eupush(cos(dir) * dist, -sin(dir) * dist);
  else
    return spin(dir) * xpush(dist) * spin(-dir);
  }

purehookset hooks_swapdim;

namespace geom3 {
  
  // Here we convert between the following parameters:
  
  // abslev: level below the plane
  // lev: level above the world (abslev = depth-lev)
  // projection: projection parameter
  // factor: zoom factor
  
  ld abslev_to_projection(ld abslev) {
    if(sphere || euclid) return vid.camera+abslev;
    return tanh(abslev) / tanh(vid.camera);
    }
  
  ld projection_to_abslev(ld proj) {
    if(sphere || euclid) return proj-vid.camera;
    // tanh(abslev) / tanh(camera) = proj
    return atanh(proj * tanh(vid.camera));
    }
  
  ld lev_to_projection(ld lev) {
    return abslev_to_projection(vid.depth - lev);
    }
  
  ld projection_to_factor(ld proj) {
    return lev_to_projection(0) / proj;
    }
  
  ld factor_to_projection(ld fac) {
    return lev_to_projection(0) / fac;
    }
  
  ld lev_to_factor(ld lev) { 
    if(WDIM == 3) return lev;
    if(GDIM == 3) return vid.depth - lev;
    return projection_to_factor(lev_to_projection(lev)); 
    }
  ld factor_to_lev(ld fac) { 
    if(DIM == 3) return fac;
    return vid.depth - projection_to_abslev(factor_to_projection(fac)); 
    }
  
  void do_auto_eye() {
    if(!vid.auto_eye) return;
    auto& cs = getcs();
    if(cs.charid < 4)
      vid.eye = cgi.eyelevel_human;
    else if(cs.charid < 8)
      vid.eye = cgi.eyelevel_dog;
    else if(cs.charid == 8)
      vid.eye = cgi.eyelevel_familiar;
    }
  
  // how should we scale at level lev
  ld scale_at_lev(ld lev) { 
    if(sphere || euclid) return 1;
    return cosh(vid.depth - lev); 
    }
  
  string invalid;
  
  ld actual_wall_height() {
      #if CAP_GP
      if(GOLDBERG && vid.gp_autoscale_heights) 
        return vid.wall_height * min<ld>(4 / hypot_d(2, gp::next), 1);
      #endif
      return vid.wall_height;
      }
  }
  
  void geometry_information::prepare_compute3() {
    using namespace geom3;
    DEBBI(DF_INIT | DF_POLY | DF_GEOM, ("geom3::compute"));
    // tanh(depth) / tanh(camera) == vid.alpha
    invalid = "";
    
    if(GDIM == 3) ;
    else if(vid.tc_alpha < vid.tc_depth && vid.tc_alpha < vid.tc_camera)
      vid.alpha = tan_auto(vid.depth) / tan_auto(vid.camera);
    else if(vid.tc_depth < vid.tc_alpha && vid.tc_depth < vid.tc_camera) {
      ld v = vid.alpha * tan_auto(vid.camera);
      if(hyperbolic && (v<1e-6-12 || v>1-1e-12)) invalid = "cannot adjust depth", vid.depth = vid.camera;
      else vid.depth = atan_auto(v);
      }
    else {
      ld v = tan_auto(vid.depth) / vid.alpha;
      if(hyperbolic && (v<1e-12-1 || v>1-1e-12)) invalid = "cannot adjust camera", vid.camera = vid.depth;
      else vid.camera = atan_auto(v);
      }
    
    if(fabs(vid.alpha) < 1e-6) invalid = "does not work with perfect Klein";
  
    if(invalid != "") {
      INFDEEP = .7;
      BOTTOM = .8;
      HELLSPIKE = .85;
      LAKE = .9;
      FLOOR = 1;
      WALL = 1.25;
      SLEV[0] = 1;
      SLEV[1] = 1.08;
      SLEV[2] = 1.16;
      SLEV[3] = 1.24;
      FLATEYE = 1.03;
      LEG1 = 1.025;
      LEG = 1.05;
      LEG3 = 1.075;
      GROIN = 1.09;
      GROIN1 = 1.105;
      GHOST = 1.1;
      BODY = 1.15;
      BODY1 = 1.151;
      BODY2 = 1.152;
      BODY3 = 1.153;
      NECK1 = 1.16;
      NECK = 1.17;
      NECK3 = 1.18;
      HEAD = 1.188;
      HEAD1= 1.189;
      HEAD2= 1.190;
      HEAD3= 1.191;
      ABODY = 1.08;
      AHEAD = 1.12;
      BIRD = 1.20;
      }
    else {
      INFDEEP = GDIM == 3 ? (sphere ? M_PI/2 : +5) : (euclid || sphere) ? 0.01 : lev_to_projection(0) * tanh(vid.camera);
      ld wh = actual_wall_height();
      WALL = lev_to_factor(wh);
      FLOOR = lev_to_factor(0);
      
      human_height = vid.human_wall_ratio * wh;
      if(WDIM == 3) human_height = scalefactor * vid.height_width / 2;
      
      ld reduce = (WDIM == 3 ? human_height / 2 : 0);
      
      LEG0  = lev_to_factor(human_height * .0 - reduce);
      LEG1  = lev_to_factor(human_height * .1 - reduce);
      LEG   = lev_to_factor(human_height * .2 - reduce);
      LEG3  = lev_to_factor(human_height * .3 - reduce);
      GROIN = lev_to_factor(human_height * .4 - reduce);
      GROIN1= lev_to_factor(human_height * .5 - reduce);
      BODY  = lev_to_factor(human_height * .6 - reduce);
      BODY1 = lev_to_factor(human_height * .61 - reduce);
      BODY2 = lev_to_factor(human_height * .62 - reduce);
      BODY3 = lev_to_factor(human_height * .63 - reduce);
      NECK1 = lev_to_factor(human_height * .7 - reduce);
      NECK  = lev_to_factor(human_height * .8 - reduce);
      NECK3 = lev_to_factor(human_height * .9 - reduce);
      HEAD  = lev_to_factor(human_height * .97 - reduce);
      HEAD1 = lev_to_factor(human_height * .98 - reduce);
      HEAD2 = lev_to_factor(human_height * .99 - reduce);
      HEAD3 = lev_to_factor(human_height - reduce);
      
      reduce = (DIM == 3 ? human_height * .3 : 0);
      
      STUFF = lev_to_factor(0) - max(orbsize * 0.3, zhexf * .6);
      
      ABODY = lev_to_factor(human_height * .4 - reduce);
      ALEG0 = lev_to_factor(human_height * .0 - reduce);
      ALEG  = lev_to_factor(human_height * .2 - reduce);
      AHEAD = lev_to_factor(human_height * .6 - reduce);
      BIRD = lev_to_factor(WDIM == 3 ? 0 : (vid.human_wall_ratio+1)/2 * wh * .8);
      GHOST = lev_to_factor(WDIM == 3 ? 0 : human_height * .5);
      FLATEYE = lev_to_factor(human_height * .15);
      
      slev = vid.rock_wall_ratio * wh / 3;
      for(int s=0; s<=3; s++)
        SLEV[s] = lev_to_factor(vid.rock_wall_ratio * wh * s/3);
      LAKE = lev_to_factor(-vid.lake_top);
      HELLSPIKE = lev_to_factor(-(vid.lake_top+vid.lake_bottom)/2);
      BOTTOM = lev_to_factor(-vid.lake_bottom);
      LOWSKY = lev_to_factor(2 * wh);
      HIGH = LOWSKY;
      HIGH2 = lev_to_factor(3 * wh);
      SKY = LOWSKY - 5;
      }
    }    

namespace geom3 {
  #if MAXMDIM >= 4
void switch_always3() {
    if(dual::split(switch_always3)) return;
    if(rug::rugged) rug::close();
    vid.always3 = !vid.always3;
    swapmatrix(View);
    callhooks(hooks_swapdim);
    }
#endif

  void switch_tpp() {
    if(dual::split(switch_fpp)) return;
    if(pmodel == mdDisk && vid.camera_angle) {
      vid.yshift = 0;
      vid.camera_angle = 0;
      vid.xposition = 0;
      vid.yposition = 0;
      vid.scale = 1;      
      vid.fixed_facing = false;
      }
    else {
      vid.yshift = -0.3;
      vid.camera_angle = -45;
      vid.scale = 18/16. * vid.xres / vid.yres / multi::players;
      vid.xposition = 0;
      vid.yposition = -0.9;
      vid.fixed_facing = true;
      vid.fixed_facing_dir = 90;
      }
    }
    
  void switch_fpp() {
#if MAXMDIM >= 4
    if(rug::rugged) rug::close();
    if(dual::split(switch_fpp)) return;
    check_cgi(); cgi.require_basics();
    View = inverse(conformal::rotmatrix()) * View;
    if(!vid.always3) {
      vid.always3 = true;
      ld ms = min<ld>(cgi.scalefactor, 1);
      vid.wall_height = 1.5 * ms;
      if(sphere) {
        vid.depth = M_PI / 6;
        vid.wall_height = M_PI / 3;
        }
      vid.human_wall_ratio = 0.8;
      if(euclid && allowIncreasedSight() && vid.use_smart_range == 0) {
        genrange_bonus = gamerange_bonus = sightrange_bonus = cgi.base_distlimit * 3/2;
        }
      vid.camera = 0;
      vid.depth = ms;
      if(pmodel == mdDisk) pmodel = mdPerspective;
      swapmatrix(View);
      callhooks(hooks_swapdim);
#if CAP_RACING
      racing::player_relative = true;
#endif
      }
    else {
      vid.always3 = false;
      vid.wall_height = .3;
      vid.human_wall_ratio = .7;
      vid.camera = 1;
      vid.depth = 1;
      if(pmodel == mdPerspective) pmodel = mdDisk;
      swapmatrix(View);
      callhooks(hooks_swapdim);
      }
    View = conformal::rotmatrix() * View;
#endif
    }

  }

geometry_information *cgip;
map<string, geometry_information> cgis;

void check_cgi() {
  string s;
  auto V = [&] (string a, string b) { s += a; s += ": "; s += b; s += "; "; };
  V("GEO", its(int(geometry)));
  V("VAR", its(int(variation)));
  
  if(GOLDBERG) V("GP", its(gp::param.first) + "," + its(gp::param.second));
  if(IRREGULAR) V("IRR", its(irr::irrid));

  if(geometry == gArchimedean) V("ARCM", arcm::current.symbol);
  
  if(geometry == gCrystal) V("CRYSTAL", its(ginf[gCrystal].sides) + its(ginf[gCrystal].vertex));
  
  if(binarytiling || DIM == 3) V("WQ", its(vid.texture_step));
  
  if(binarytiling) V("BT", fts(vid.binary_width));
  
  if(GDIM == 2) { 
    V("CAMERA", fts(vid.camera));
    }
  
  if(WDIM == 2) {
    V("WH", fts(vid.wall_height));
    V("HW", fts(vid.human_wall_ratio));
    V("RW", fts(vid.rock_wall_ratio));
    V("DEPTH", fts(vid.depth));
    V("ASH", ONOFF(vid.gp_autoscale_heights));
    V("LT", fts(vid.lake_top));
    V("LB", fts(vid.lake_bottom));
    }

  V("3D", ONOFF(vid.always3));
  
  if(WDIM == 3) {
    V("CS", fts(vid.creature_scale));
    V("HTW", fts(vid.height_width));
    }
  
  V("LQ", its(vid.linequality));
  
  cgip = &cgis[s];
  }

}
