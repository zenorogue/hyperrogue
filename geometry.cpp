// Hyperbolic Rogue
// geometrical constants

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

bool debug_geometry = false;

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

hyperpoint Crad[MAX_S84];

transmatrix heptmove[MAX_EDGE], hexmove[MAX_EDGE];
transmatrix invheptmove[MAX_EDGE], invhexmove[MAX_EDGE];

ld hexshift;

// the results are:
// hexf = 0.378077 hcrossf = 0.620672 tessf = 1.090550
// hexhexdist = 0.566256

ld hcrossf7 = 0.620672;
ld hexf7 = 0.378077;

ld scalefactor, orbsize, floorrad0, floorrad1, zhexf;

// the distance between two hexagon centers

void precalc() {

  DEBB(DF_INIT, (debugfile,"precalc\n"));
  
  hexshift = 0;

  int vertexdegree = S6/2;
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

  fmin = 0, fmax = 3;
  
  for(int p=0; p<100; p++) {
    ld f =  (fmin+fmax) / 2;
    ld v1=0, v2=0;
    if(vertexdegree == 3) {
      hyperpoint H = xpush0(f);
      v1 = intval(H, C0), v2 = intval(H, spin(2*M_PI/S7)*H);
      }
    else if(vertexdegree == 4) {
      hyperpoint H = xpush0(f);
      ld opposite = hdist(H, spin(2*M_PI/S7)*H);
      hyperpoint Hopposite = xspinpush0(M_PI/S7, opposite);
      v2 = intval(H, Hopposite), v1 = intval(H, C0);
      }
    if(sphere ? v1 < v2 : v1 > v2) fmin = f; else fmax = f;
    }
  tessf = fmin;
  if(elliptic && S7 == 4) tessf = M_PI/2;
  
  if(vertexdegree == 3) {
    fmin = 0, fmax = sphere ? M_PI / 2 : 2;
    for(int p=0; p<100; p++) {
      ld f =  (fmin+fmax) / 2;
      hyperpoint H = xspinpush0(M_PI/S7, f);
      ld v1 = intval(H, C0), v2 = intval(H, xpush0(tessf));
      if(v1 < v2) fmin = f; else fmax = f;
      }
    hcrossf = fmin;
    }
  else {
    hcrossf = hdist(xpush0(tessf), xspinpush0(2*M_PI/S7, tessf)) / 2;
    }
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
  
  for(int i=0; i<S42; i++)
    Crad[i] = xspinpush0(2*M_PI*i/S42, .4);
  for(int d=0; d<S7; d++)
    heptmove[d] = spin(-d * ALPHA) * xpush(tessf) * spin(M_PI);
    
  for(int d=0; d<S7; d++) 
    hexmove[d] = spin(hexshift-d * ALPHA) * xpush(-crossf)* spin(M_PI);  

  for(int d=0; d<S7; d++) invheptmove[d] = inverse(heptmove[d]);
  for(int d=0; d<S7; d++) invhexmove[d] = inverse(hexmove[d]);

  hexhexdist = hdist(xpush0(crossf), xspinpush0(M_PI*2/S7, crossf));
  
  hexvdist = hdist(xpush0(hexf), xspinpush0(ALPHA/2, hcrossf));

  if(debug_geometry) 
  printf("S7=%d S6=%d hexf = " LDF" hcross = " LDF" tessf = " LDF" hexshift = " LDF " hexhex = " LDF " hexv = " LDF "\n", S7, S6, hexf, hcrossf, tessf, hexshift, 
    hexhexdist, hexvdist);  
  
  base_distlimit = ginf[geometry].distlimit[!BITRUNCATED];

  gp::compute_geometry();  
  irr::compute_geometry();
  if(archimedean) {
    arcm::current.compute_geometry();
    crossf = hcrossf7 * arcm::current.scale();
    hexvdist = arcm::current.scale() * .5;
    rhexf = arcm::current.scale() * .5;
    }
  if(binarytiling) hexvdist = rhexf = 1, tessf = 1, scalefactor = 1, crossf = hcrossf7;

  scalefactor = crossf / hcrossf7;
  orbsize = crossf;

  zhexf = BITRUNCATED ? hexf : crossf* .55;

  floorrad0 = hexvdist* 0.92;
  floorrad1 = rhexf * 0.94;
  
  if(euclid4) {
    if(!BITRUNCATED)
      floorrad0 = floorrad1 = rhexf * .94;
    else
      floorrad0 = hexvdist * .9,
      floorrad1 = rhexf * .8;
    }
  }

transmatrix xspinpush(ld dir, ld dist) {
  if(euclid)
    return eupush(cos(dir) * dist, -sin(dir) * dist);
  else
    return spin(dir) * xpush(dist) * spin(-dir);
  }

namespace geom3 {

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
    return projection_to_factor(lev_to_projection(lev)); 
    }
  ld factor_to_lev(ld fac) { 
    return depth - projection_to_abslev(factor_to_projection(fac)); 
    }
  
  // how should we scale at level lev
  ld scale_at_lev(ld lev) { 
    if(sphere || euclid) return 1;
    return cosh(depth - lev); 
    }
  
  ld INFDEEP, BOTTOM, HELLSPIKE, LAKE, WALL, 
    SLEV[4], FLATEYE,
    LEG1, LEG, LEG3, GROIN, GROIN1, GHOST,
    BODY, NECK1, NECK, NECK3, HEAD,
    ABODY, AHEAD, BIRD;
  
  string invalid;
  
  ld actual_wall_height() {
      if(GOLDBERG && gp_autoscale_heights) 
        return wall_height * min<ld>(4 / hypot2(gp::next), 1);
      return wall_height;
      }
  
  void compute() {
    // tanh(depth) / tanh(camera) == vid.alpha
    invalid = "";
    
    if(tc_alpha < tc_depth && tc_alpha < tc_camera)
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
      NECK1 = 1.16;
      NECK = 1.17;
      NECK3 = 1.18;
      HEAD = 1.19;
      ABODY = 1.08;
      AHEAD = 1.12;
      BIRD = 1.20;
      }
    else {
      INFDEEP = (euclid || sphere) ? 0.01 : lev_to_projection(0) * tanh(camera);
      ld wh = actual_wall_height();
      WALL = lev_to_factor(wh);
      
      human_height = human_wall_ratio * wh;

      LEG1  = lev_to_factor(human_height * .1);
      LEG   = lev_to_factor(human_height * .2);
      LEG3  = lev_to_factor(human_height * .3);
      GROIN = lev_to_factor(human_height * .4);
      GROIN1= lev_to_factor(human_height * .5);
      BODY  = lev_to_factor(human_height * .6);
      NECK1 = lev_to_factor(human_height * .7);
      NECK  = lev_to_factor(human_height * .8);
      NECK3 = lev_to_factor(human_height * .9);
      HEAD  = lev_to_factor(human_height);
      
      ABODY = lev_to_factor(human_height * .4);
      AHEAD = lev_to_factor(human_height * .6);
      BIRD = lev_to_factor((human_wall_ratio+1)/2 * wh * .8);
      GHOST = lev_to_factor(human_height * .5);
      FLATEYE = lev_to_factor(human_height * .15);
      
      slev = rock_wall_ratio * wh / 3;
      for(int s=0; s<=3; s++)
        SLEV[s] = lev_to_factor(rock_wall_ratio * wh * s/3);
      LAKE = lev_to_factor(-lake_top);
      HELLSPIKE = lev_to_factor(-(lake_top+lake_bottom)/2);
      BOTTOM = lev_to_factor(-lake_bottom);
      }
    }    
  }

void initgeo() {
  // printf("%Lf\n", (ld) hdist0(xpush(-1)*ypush(0.01)*xpush(1)*C0));
  precalc();
  }
}
