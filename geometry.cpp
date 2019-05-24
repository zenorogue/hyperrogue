// Hyperbolic Rogue
// geometrical constants

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

ld tessf, crossf, hexf, hcrossf, hexhexdist, hexvdist, hepvdist, rhexf;

// tessf: distance from heptagon center to another heptagon center
// hexf: distance from heptagon center to small heptagon vertex
// hcrossf: distance from heptagon center to big heptagon vertex
// crossf: distance from heptagon center to adjacent cell center (either hcrossf or tessf)
// hexhexdist: distance between adjacent hexagon vertices
// hexvdist: distance between hexagon vertex and hexagon center
// hepvdist: distance between heptagon vertex and hexagon center (either hcrossf or something else)
// rhexf: distance from heptagon center to heptagon vertex (either hexf or hcrossf)

int base_distlimit;

transmatrix heptmove[MAX_EDGE], hexmove[MAX_EDGE];
transmatrix invheptmove[MAX_EDGE], invhexmove[MAX_EDGE];

ld hexshift;

ld sword_size = 0;

ld corner_bonus = 0;

ld asteroid_size[8];

// the results are:
// hexf = 0.378077 hcrossf = 0.620672 tessf = 1.090550
// hexhexdist = 0.566256

ld hcrossf7 = 0.620672;
ld hexf7 = 0.378077;

ld scalefactor, orbsize, floorrad0, floorrad1, zhexf;

// the distance between two hexagon centers

void precalc() {

  DEBBI(DF_INIT | DF_POLY | DF_GEOM, ("precalc"));
  
  hexshift = 0;

  ld fmin, fmax;  

  if(archimedean) 
    ginf[gArchimedean].cclass = gcHyperbolic;

  if(euclid) {
    // dynamicval<eGeometry> g(geometry, gNormal);
    // precalc(); }
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

  if(WDIM == 3) scalefactor *= geom3::creature_scale;

  zhexf = BITRUNCATED ? hexf : crossf* .55;
  if(WDIM == 3) zhexf *= geom3::creature_scale;
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
  }

transmatrix xspinpush(ld dir, ld dist) {
  if(euclid)
    return eupush(cos(dir) * dist, -sin(dir) * dist);
  else
    return spin(dir) * xpush(dist) * spin(-dir);
  }

purehookset hooks_swapdim;

namespace geom3 {

  bool always3 = false; 
  int tc_alpha=3, tc_depth=1, tc_camera=2;
  
  ld depth = 1;        // world below the plane
  ld camera = 1;       // camera above the plane
  ld wall_height = .3;
  ld slev = .08;
  ld lake_top = .25, lake_bottom = .9;
  ld rock_wall_ratio = .9;
  ld human_wall_ratio = .7;
  ld human_height;
  bool gp_autoscale_heights = true;
  
  ld creature_scale, height_width;
  
  ld highdetail = 8, middetail = 8;
  
  // Here we convert between the following parameters:
  
  // abslev: level below the plane
  // lev: level above the world (abslev = depth-lev)
  // projection: projection parameter
  // factor: zoom factor
  
  ld abslev_to_projection(ld abslev) {
    if(sphere || euclid) return camera+abslev;
    return tanh(abslev) / tanh(camera);
    }
  
  ld projection_to_abslev(ld proj) {
    if(sphere || euclid) return proj-camera;
    // tanh(abslev) / tanh(camera) = proj
    return atanh(proj * tanh(camera));
    }
  
  ld lev_to_projection(ld lev) {
    return abslev_to_projection(depth - lev);
    }
  
  ld projection_to_factor(ld proj) {
    return lev_to_projection(0) / proj;
    }
  
  ld factor_to_projection(ld fac) {
    return lev_to_projection(0) / fac;
    }
  
  ld lev_to_factor(ld lev) { 
    if(WDIM == 3) return lev;
    if(GDIM == 3) return depth - lev;
    return projection_to_factor(lev_to_projection(lev)); 
    }
  ld factor_to_lev(ld fac) { 
    if(DIM == 3) return fac;
    return depth - projection_to_abslev(factor_to_projection(fac)); 
    }
  
  // how should we scale at level lev
  ld scale_at_lev(ld lev) { 
    if(sphere || euclid) return 1;
    return cosh(depth - lev); 
    }
  
  ld INFDEEP, BOTTOM, HELLSPIKE, LAKE, WALL, FLOOR, STUFF,
    SLEV[4], FLATEYE,
    LEG0, LEG1, LEG, LEG3, GROIN, GROIN1, GHOST,
    BODY, BODY1, BODY2, BODY3,
    NECK1, NECK, NECK3, HEAD, HEAD1, HEAD2, HEAD3,
    ALEG0, ALEG, ABODY, AHEAD, BIRD, LOWSKY, SKY, HIGH, HIGH2;
  
  string invalid;
  
  ld actual_wall_height() {
      #if CAP_GP
      if(GOLDBERG && gp_autoscale_heights) 
        return wall_height * min<ld>(4 / hypot_d(2, gp::next), 1);
      #endif
      return wall_height;
      }
  
  void compute() {
    DEBBI(DF_INIT | DF_POLY | DF_GEOM, ("geom3::compute"));
    // tanh(depth) / tanh(camera) == vid.alpha
    invalid = "";
    
    if(GDIM == 3) ;
    else if(tc_alpha < tc_depth && tc_alpha < tc_camera)
      vid.alpha = tan_auto(depth) / tan_auto(camera);
    else if(tc_depth < tc_alpha && tc_depth < tc_camera) {
      ld v = vid.alpha * tan_auto(camera);
      if(hyperbolic && (v<1e-6-12 || v>1-1e-12)) invalid = "cannot adjust depth", depth = camera;
      else depth = atan_auto(v);
      }
    else {
      ld v = tan_auto(depth) / vid.alpha;
      if(hyperbolic && (v<1e-12-1 || v>1-1e-12)) invalid = "cannot adjust camera", camera = depth;
      else camera = atan_auto(v);
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
      INFDEEP = GDIM == 3 ? (sphere ? M_PI/2 : +5) : (euclid || sphere) ? 0.01 : lev_to_projection(0) * tanh(camera);
      ld wh = actual_wall_height();
      WALL = lev_to_factor(wh);
      FLOOR = lev_to_factor(0);
      
      human_height = human_wall_ratio * wh;
      if(WDIM == 3) human_height = scalefactor * height_width / 2;
      
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
      BIRD = lev_to_factor(WDIM == 3 ? 0 : (human_wall_ratio+1)/2 * wh * .8);
      GHOST = lev_to_factor(WDIM == 3 ? 0 : human_height * .5);
      FLATEYE = lev_to_factor(human_height * .15);
      
      slev = rock_wall_ratio * wh / 3;
      for(int s=0; s<=3; s++)
        SLEV[s] = lev_to_factor(rock_wall_ratio * wh * s/3);
      LAKE = lev_to_factor(-lake_top);
      HELLSPIKE = lev_to_factor(-(lake_top+lake_bottom)/2);
      BOTTOM = lev_to_factor(-lake_bottom);
      LOWSKY = lev_to_factor((1 + rock_wall_ratio) * wh);
      HIGH = LOWSKY;
      HIGH2 = lev_to_factor((2 + rock_wall_ratio) * wh);
      SKY = LOWSKY - 5;
      }
    }    

  #if MAXMDIM >= 4
void switch_always3() {
    if(rug::rugged) rug::close();
    geom3::always3 = !geom3::always3;
    need_reset_geometry = true;
    swapmatrix(View);
    callhooks(hooks_swapdim);
    }
#endif

  void switch_tpp() {
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
    if(!geom3::always3) {
      geom3::always3 = true;
      geom3::wall_height = 1.5;
      if(sphere) {
        geom3::depth = M_PI / 6;
        geom3::wall_height = M_PI / 3;
        }
      geom3::human_wall_ratio = 0.8;
      geom3::camera = 0;
      if(pmodel == mdDisk) pmodel = mdPerspective;
      need_reset_geometry = true;
      swapmatrix(View);
      callhooks(hooks_swapdim);
#if CAP_RACING
      racing::player_relative = true;
#endif
      }
    else {
      geom3::always3 = false;
      geom3::wall_height = .3;
      geom3::human_wall_ratio = .7;
      geom3::camera = 1;
      geom3::depth = 1;
      if(pmodel == mdPerspective) pmodel = mdDisk;
      need_reset_geometry = true;
      swapmatrix(View);
      callhooks(hooks_swapdim);
      }
#endif
   }

  }

void initgeo() {
  // printf("%Lf\n", (ld) hdist0(xpush(-1)*ypush(0.01)*xpush(1)*C0));
  precalc();
  }
}
