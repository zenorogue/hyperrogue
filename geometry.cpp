// Hyperbolic Rogue
// geometrical constants

// Copyright (C) 2011-2012 Zeno Rogue, see 'hyper.cpp' for details


ld tessf, crossf, hexf, hcrossf, hexhexdist;

// tessf: distance from heptagon center to another heptagon center
// hexf: distance from heptagon center to heptagon vertex
// crossf: distance from heptagon center to adjacent hexagon center
// hexhexdist: distance between adjacent hexagon vertices

#define ALPHA (M_PI*2/S7)

hyperpoint Crad[42];

transmatrix heptmove[7], hexmove[7];
transmatrix invheptmove[7], invhexmove[7];

transmatrix spinmatrix[84];

const transmatrix& getspinmatrix(int id) {
  while(id>=S84) id -= S84;
  while(id<0) id += S84;
  return spinmatrix[id];
  }

// the results are:
// hexf = 0.378077 hcrossf = 0.620672 tessf = 1.090550
// hexhexdist = 0.566256

// the distance between two hexagon centers

void precalc() {

  DEBB(DF_INIT, (debugfile,"precalc\n"));

  if(euclid) return;

  ld fmin = 1, fmax = 2;
  
  for(int p=0; p<100; p++) {
    ld f =  (fmin+fmax) / 2;
    hyperpoint H = xpush(f) * C0;
    ld v1 = intval(H, C0), v2 = intval(H, spin(2*M_PI/S7)*H);
    if(sphere ? v1 < v2 : v1 > v2) fmin = f; else fmax = f;
    }
  tessf = fmin;
  
  fmin = 0, fmax = sphere ? M_PI / 2 : 2;
  for(int p=0; p<100; p++) {
    ld f =  (fmin+fmax) / 2;
    hyperpoint H = spin(M_PI/S7) * xpush(f) * C0;
    ld v1 = intval(H, C0), v2 = intval(H, xpush(tessf) * C0);
    if(v1 < v2) fmin = f; else fmax = f;
    }
  hcrossf = fmin;
  crossf = purehepta ? tessf : hcrossf;
  
  fmin = 0, fmax = tessf;
  for(int p=0; p<100; p++) {
    ld f =  (fmin+fmax) / 2;
    hyperpoint H = xpush(f) * C0;
    hyperpoint H1 = spin(2*M_PI/S7) * H;
    hyperpoint H2 = xpush(tessf-f) * C0;
    ld v1 = intval(H, H1), v2 = intval(H, H2);
    if(v1 < v2) fmin = f; else fmax = f;
    }
  hexf = fmin;
  
  // printf("hexf = %.6Lf cross = %.6Lf tessf = %.6Lf\n", hexf, crossf, tessf);
  
  for(int i=0; i<S42; i++)
    Crad[i] = spin(2*M_PI*i/S42) * xpush(.4) * C0;
  for(int d=0; d<S7; d++)
    heptmove[d] = spin(-d * ALPHA) * xpush(tessf) * spin(M_PI);
  for(int d=0; d<S7; d++) 
    hexmove[d] = spin(-d * ALPHA) * xpush(-crossf)* spin(M_PI);  

  for(int d=0; d<S7; d++) invheptmove[d] = inverse(heptmove[d]);
  for(int d=0; d<S7; d++) invhexmove[d] = inverse(hexmove[d]);

  hexhexdist = hdist(xpush(crossf) * C0, spin(M_PI*2/S7) * xpush(crossf) * C0);
  
  for(int i=0; i<S84; i++) spinmatrix[i] = spin(i * M_PI / S42);
  }

transmatrix ddi(ld dir, ld dist) {
  if(euclid)
    return eupush(cos(M_PI*dir/S42) * dist, -sin(M_PI*dir/S42) * dist);
  else
    return spin(M_PI*dir/S42) * xpush(dist) * spin(-M_PI*dir/S42);
  }

hyperpoint ddi0(ld dir, ld dist) {
  if(euclid)
    return hpxy(cos(M_PI*dir/S42) * dist, -sin(M_PI*dir/S42) * dist);
  else
    return xspinpush0(M_PI*dir/S42, dist);
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
  
  void compute() {
    // tanh(depth) / tanh(camera) == vid.alpha
    invalid = "";
    
    if(tc_alpha < tc_depth && tc_alpha < tc_camera)
      vid.alpha = tanh(depth) / tanh(camera);
    else if(tc_depth < tc_alpha && tc_depth < tc_camera) {
      ld v = vid.alpha * tanh(camera);
      if(v<-1 || v>1) invalid = "cannot adjust depth", depth = camera;
      else depth = atanh(v);
      }
    else {
      ld v = tanh(depth) / vid.alpha;
      if(v<-1 || v>1) invalid = "cannot adjust camera", camera = depth;
      else camera = atanh(v);
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
      WALL = lev_to_factor(wall_height);
      
      human_height = human_wall_ratio * wall_height;

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
      BIRD = lev_to_factor((human_wall_ratio+1)/2 * wall_height * .8);
      GHOST = lev_to_factor(human_height * .5);
      FLATEYE = lev_to_factor(human_height * .15);
      
      slev = rock_wall_ratio * wall_height / 3;
      for(int s=0; s<=3; s++)
        SLEV[s] = lev_to_factor(rock_wall_ratio * wall_height * s/3);
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
