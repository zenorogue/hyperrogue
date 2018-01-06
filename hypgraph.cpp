ld ghx, ghy, ghgx, ghgy;
hyperpoint ghpm = C0;

void ghcheck(hyperpoint &ret, const hyperpoint &H) {
  if(hypot(ret[0]-ghx, ret[1]-ghy) < hypot(ghgx-ghx, ghgy-ghy)) {
    ghpm = H; ghgx = ret[0]; ghgy = ret[1];
    }
  }

void camrotate(ld& hx, ld& hy) {
  ld cam = vid.camera_angle * M_PI / 180;
  GLfloat cc = cos(cam);
  GLfloat ss = sin(cam);
  ld ux = hx, uy = hy * cc + ss, uz = cc - ss * hy;
  hx = ux / uz, hy = uy / uz;
  }

hyperpoint gethyper(ld x, ld y) {

  ld hx = (x - vid.xcenter) / vid.radius;
  ld hy = (y - vid.ycenter) / vid.radius;
  
  if(pmodel) {
    ghx = hx, ghy = hy;
    return ghpm;
    }
  
  if(euclid)
    return hpxy(hx * (EUCSCALE + vid.alphax), hy * (EUCSCALE + vid.alphax));
    
  if(vid.camera_angle) camrotate(hx, hy);
  
  ld hr = hx*hx+hy*hy;
  
  if(hr > .9999 && !sphere) return Hypc;
  
  // hz*hz-(hx/(hz+alpha))^2 - (hy/(hz+alpha))^2 =
  
  // hz*hz-hr*(hz+alpha)^2 == 1
  // hz*hz - hr*hr*hz*Hz
  
  
  ld A, B, C;
  
  ld curv = sphere ? 1 : -1;
  
  A = 1+curv*hr;
  B = 2*hr*vid.alphax*-curv;
  C = 1 - curv*hr*vid.alphax*vid.alphax;
  
  // Az^2 - Bz = C
  B /= A; C /= A;
  
  // z^2 - Bz = C
  // z^2 - Bz + (B^2/4) = C + (B^2/4)
  // z = (B/2) + sqrt(C + B^2/4)
  
  ld rootsign = 1;
  if(sphere && vid.alphax > 1) rootsign = -1;
  
  ld hz = B / 2 + rootsign * sqrt(C + B*B/4);
  
  hyperpoint H;
  H[0] = hx * (hz+vid.alphax);
  H[1] = hy * (hz+vid.alphax);
  H[2] = hz;
  
  return H;
  }

void ballmodel(hyperpoint& ret, double alpha, double d, double zl) {
  hyperpoint H = ypush(geom3::camera) * xpush(d) * ypush(zl) * C0;
  ld tzh = vid.ballproj + H[2];
  ld ax = H[0] / tzh;
  ld ay = H[1] / tzh;
  ld ball = vid.ballangle * M_PI / 180;
  
  ld ca = cos(alpha), sa = sin(alpha);
  ld cb = cos(ball), sb = sin(ball);
  
  ret[0] = ax * ca;
  ret[1] = ay * cb + ax * sa * sb;
  ret[2] = - ax * sa * cb - ay * sb;
  }

void applymodel(hyperpoint H, hyperpoint& ret) {
  
  ld tz = euclid ? (EUCSCALE+vid.alphax) : vid.alphax+H[2];
  if(tz < BEHIND_LIMIT && tz > -BEHIND_LIMIT) tz = BEHIND_LIMIT;
  
  if(pmodel == mdUnchanged) { 
    for(int i=0; i<3; i++) ret[i] = H[i] / vid.radius;
    return; 
    }
  
  if(pmodel == mdBall) {
    ld zlev = zlevel(H);
    using namespace hyperpoint_vec;
    H = H / zlev;
    
    ld zl = geom3::depth-geom3::factor_to_lev(zlev);
    double alpha = atan2(H[1], H[0]);
    double d = hdist0(H);

    ballmodel(ret, alpha, d, zl);
    ghcheck(ret,H);
    
    return;
    }
  
  if(pmodel == mdHyperboloid) {

    ld ball = vid.ballangle * M_PI / 180;
    ld cb = cos(ball), sb = sin(ball);

    ret[0] = H[0] / 3;
    ret[1] = (1 - H[2]) / 3 * cb + H[1] / 3 * sb;
    ret[2] = H[1] / 3 * cb - (1 - H[2]) / 3 * sb;

    ghcheck(ret,H);
    return;
    }
  
  if(pmodel == mdDisk) {
  
    if(!vid.camera_angle) {
      ret[0] = H[0] / tz;    
      ret[1] = H[1] / tz;
      ret[2] = (1 - vid.beta / tz);
      }
    else {
      ld tx = H[0];
      ld ty = H[1];
      ld cam = vid.camera_angle * M_PI / 180;
      GLfloat cc = cos(cam);
      GLfloat ss = sin(cam);
      ld ux = tx, uy = ty * cc - ss * tz, uz = tz * cc + ss * ty;
      ret[0] = ux / uz;
      ret[1] = uy / uz;
      ret[2] = 1 - vid.beta / uz;
      }
    return;
    }

  ld zlev = 1;

  if(wmspatial || mmspatial) {
    zlev = zlevel(H);
    using namespace hyperpoint_vec;
    H = H / zlev;
    }

  if(mdEqui()) {
    ld rad = sqrt(H[0] * H[0] + H[1] * H[1]);
    if(rad == 0) rad = 1;
    ld d = hdist0(H);
    
    // 4 pi / 2pi = M_PI 
    
    if(pmodel == 6 && sphere)
      d = sqrt(2*(1 - cos(d))) * M_PI / 2;
    else if(pmodel == 6 && !euclid)
      d = sqrt(2*(cosh(d) - 1)) / 1.5;
    ret[0] = d * H[0] / rad / M_PI;
    ret[1] = d * H[1] / rad / M_PI;
    ret[2] = 0; 
    if(zlev != 1 && vid.goteyes) 
      ret[2] = geom3::factor_to_lev(zlev);
    ghcheck(ret,H);

    return;
    }
  
  tz = H[2]+vid.alphax;

  if(pmodel == mdPolygonal || pmodel == mdPolynomial) {
    pair<long double, long double> p = polygonal::compute(H[0]/tz, H[1]/tz);
    ret[0] = p.first;
    ret[1] = p.second;
    ret[2] = 0;
    ghcheck(ret,H);
    return;
    }
  
  // Poincare to half-plane
  
  ld x0, y0;  
  x0 = H[0] / tz;
  y0 = H[1] / tz;
  y0 += 1;
  double rad = x0*x0 + y0*y0;
  y0 /= rad;
  x0 /= rad;
  y0 -= .5;
  
  if(pmodel == mdHalfplane) {
    ret[0] = x0;
    if(wmspatial || mmspatial) y0 *= zlev;
    ret[1] = 1 - y0;
    ret[2] = 0;
    if(zlev != 1 && vid.goteyes) 
      ret[2] = y0 * geom3::factor_to_lev(zlev);
    ghcheck(ret,H);
    return;
    }

  // center
  x0 *= 2; y0 *= 2;
  
  // half-plane to band
  double tau = (log((x0+1)*(x0+1) + y0*y0) - log((x0-1)*(x0-1) + y0*y0)) / 2;
  double u=(1-x0*x0-y0*y0);
  u = (1 - x0*x0 - y0*y0 + sqrt(u*u+4*y0*y0));
  double yv = 2*y0 / u;
  double sigma = 2 * atan(yv * zlev) - M_PI/2;
  
  x0 = tau; y0 = sigma;
  
  /* if(zlev != 1) {
    double alp = (y0 * y0) / (1-y0*y0);
    double gx = alp + sqrt(alp*alp-1);
    double gy = y0 * (gx+1);
    double yr = zlev * gy / (zlev * gx + 1);
    printf("zlev = %10.5lf y0 = %20.10lf yr = %20.10lf\n", double(zlev), (double)y0, yr);
    y0 = yr;
    } */
  
  ret[0] = x0/M_PI*2;
  ret[1] = -y0/M_PI*2;
  ret[2] = 0; 

  if(zlev != 1 && vid.goteyes) 
    ret[2] = geom3::factor_to_lev(zlev) / (1 + yv * yv);

  ghcheck(ret,H);
  }

// game-related graphics

transmatrix View; // current rotation, relative to viewctr
transmatrix cwtV; // player-relative view
transmatrix sphereflip; // on the sphere, flip
heptspin viewctr; // heptagon and rotation where the view is centered at
bool playerfound; // has player been found in the last drawing?

#define eurad crossf

double q3 = sqrt(double(3));

bool outofmap(hyperpoint h) {
  if(euclid) 
    return h[2] < .5; // false; // h[0] * h[0] + h[1] * h[1] > 15 * eurad;
  else if(sphere)
    return h[2] < .1 && h[2] > -.1 && h[1] > -.1 && h[1] < .1 && h[0] > -.1 && h[0] < .1;
  else
    return h[2] < .5;
  }

hyperpoint mirrorif(const hyperpoint& V, bool b) {
  if(b) return Mirror*V;
  else return V;
  }

transmatrix mirrorif(const transmatrix& V, bool b) {
  if(b) return V*Mirror;
  else return V;
  }

// -1 if away, 0 if not away
int away(const transmatrix& V2) {
  return (intval(C0, V2 * xpush0(.1)) > intval(C0, tC0(V2))) ? -1 : 0;
  }

/* double zgrad(double f1, double f2, int nom, int den) {
  using namespace geom3;
  ld fo1 = factor_to_lev(f1);
  ld fo2 = factor_to_lev(f2);
  return lev_to_factor(fo1 + (fo2-fo1) * nom / den);
  } */

double zgrad0(double l1, double l2, int nom, int den) {
  using namespace geom3;
  return lev_to_factor(l1 + (l2-l1) * nom / den);
  }

bool behindsphere(const hyperpoint& h) {
  if(!sphere) return false;

  if(vid.alpha > 1) {
     if(h[2] > -1/vid.alpha) return true;
     }  
  
  if(vid.alpha <= 1) {
    if(h[2] < -.8) return true;
    }
  
  return false;
  }

ld to01(ld a0, ld a1, ld x) {
  if(x < a0) return 0;
  if(x > a1) return 1;
  return (x-a0) / (a1-a0);
  }

ld spherity(const hyperpoint& h) {
  if(!sphere) return 1;
  
  if(vid.alpha > 1) {
    return to01(1/vid.alpha, 1, -h[2]);
    }  
  
  if(vid.alpha <= 1) {
    return to01(-1.5, 1, h[2]);
    }
      
  return 1;
  }

bool behindsphere(const transmatrix& V) {
  return behindsphere(tC0(V));
  }

ld spherity(const transmatrix& V) {
  return spherity(tC0(V));
  }

bool confusingGeometry() {
  return elliptic || quotient == 1 || torus;
  }

transmatrix actualV(const heptspin& hs, const transmatrix& V) {
  return (hs.spin || nonbitrunc) ? V * spin(hs.spin*2*M_PI/S7 + (nonbitrunc ? M_PI:0)) : V;
  }

void drawrec(const heptspin& hs, int lev, hstate s, const transmatrix& V) {

  // shmup::calc_relative_matrix(cwt.c, hs.h);
    
  cell *c = hs.h->c7;
  
  transmatrix V10;
  const transmatrix& V1 = hs.mirrored ? (V10 = V * Mirror) : V;
  
  if(dodrawcell(c)) {
    reclevel = maxreclevel - lev;
    drawcell(c, actualV(hs, V1), 0, hs.mirrored);
    }
  
  if(lev <= 0) return;
  
  if(!nonbitrunc) for(int d=0; d<S7; d++) {
    int ds = fixrot(hs.spin + d);
    reclevel = maxreclevel - lev + 1;
    // createMov(c, ds);
    if(c->mov[ds] && c->spn(ds) == 0 && dodrawcell(c->mov[ds])) {
      drawcell(c->mov[ds], V1 * hexmove[d], 0, hs.mirrored ^ c->mirror(ds));
      }
    }

  if(lev <= 1) return;
  
  for(int d=0; d<S7; d++) {
    hstate s2 = transition(s, d);
    if(s2 == hsError) continue;
    heptspin hs2 = hsstep(hsspin(hs, d), 0);
    drawrec(hs2, lev-2, s2, V * heptmove[d]);
    }
  
  }

int mindx=-7, mindy=-7, maxdx=7, maxdy=7;
  
transmatrix eumove(ld x, ld y) {
  transmatrix Mat = Id;
  Mat[2][2] = 1;
  
  if(a4) {
    Mat[0][2] += x * eurad;
    Mat[1][2] += y * eurad;
    }
  else {
    Mat[0][2] += (x + y * .5) * eurad;
    // Mat[2][0] += (x + y * .5) * eurad;
    Mat[1][2] += y * q3 /2 * eurad;
    // Mat[2][1] += y * q3 /2 * eurad;
    }
  
  ld v = a4 ? 1 : q3;

  while(Mat[0][2] <= -16384 * eurad) Mat[0][2] += 32768 * eurad;
  while(Mat[0][2] >= 16384 * eurad) Mat[0][2] -= 32768 * eurad;
  while(Mat[1][2] <= -16384 * v * eurad) Mat[1][2] += 32768 * v * eurad;
  while(Mat[1][2] >= 16384 * v * eurad) Mat[1][2] -= 32768 * v * eurad;
  return Mat;
  }

transmatrix eumove(int vec) {
  int x, y;
  tie(x,y) = vec_to_pair(vec);
  return eumove(x, y);
  }

transmatrix eumovedir(int d) {
  if(a4) {
    d = d & 3;
    switch(d) {
      case 0: return eumove(1,0);
      case 1: return eumove(0,1);
      case 2: return eumove(-1,0);
      case 3: return eumove(0,-1);
      }
    }
  else {
    d = fix6(d);
    switch(d) {
      case 0: return eumove(1,0);
      case 1: return eumove(0,1);
      case 2: return eumove(-1,1);
      case 3: return eumove(-1,0);
      case 4: return eumove(0,-1);
      case 5: return eumove(1,-1);
      }
    }
  return eumove(0,0);
  }

ld matrixnorm(const transmatrix& Mat) {
  return Mat[0][2] * Mat[0][2] + Mat[1][2] * Mat[1][2];
  }
  
void drawEuclidean() {
  DEBB(DF_GRAPH, (debugfile,"drawEuclidean\n"));
  sphereflip = Id;
  if(!centerover.c) centerover = cwt;
  // printf("centerover = %p player = %p [%d,%d]-[%d,%d]\n", lcenterover, cwt.c,
  //   mindx, mindy, maxdx, maxdy);
  int pvec = cellwalker_to_vec(centerover);
    
  int minsx = mindx-1, maxsx=maxdx+1, minsy=mindy-1, maxsy=maxdy+1;
  mindx=maxdx=mindy=maxdy=0;
  
  transmatrix View0 = View;
  
  ld cellrad = vid.radius / (EUCSCALE + vid.alphax);
  
  ld centerd = matrixnorm(View0);
  
  for(int dx=minsx; dx<=maxsx; dx++)
  for(int dy=minsy; dy<=maxsy; dy++) {
    torusconfig::torus_cx = dx;
    torusconfig::torus_cy = dy;
    reclevel = eudist(dx, dy);
    cellwalker cw = vec_to_cellwalker(pvec + euclid_getvec(dx, dy));
    transmatrix Mat = eumove(dx,dy);
    
    if(!cw.c) continue;

    Mat = View0 * Mat;
    
    if(true) {
      ld locald = matrixnorm(Mat);
      if(locald < centerd) centerd = locald, centerover = cw, View = View0 * eumove(dx, dy);
      }
    
    // Mat[0][0] = -1;
    // Mat[1][1] = -1;
    
    // Mat[2][0] = x*x/10;
    // Mat[2][1] = y*y/10;
    // Mat = Mat * xpush(x-30) * ypush(y-30);

    int cx, cy, shift;
    getcoord0(tC0(Mat), cx, cy, shift);
    if(cx >= 0 && cy >= 0 && cx < vid.xres && cy < vid.yres) {
      if(dx < mindx) mindx = dx;
      if(dy < mindy) mindy = dy;
      if(dx > maxdx) maxdx = dx;
      if(dy > maxdy) maxdy = dy;
      }
    if(cx >= -cellrad && cy >= -cellrad && cx < vid.xres+cellrad && cy < vid.yres+cellrad)
      if(dodrawcell(cw.c)) {
        drawcell(cw.c, cw.mirrored ? Mat * Mirror : Mat, cw.spin, cw.mirrored);
        }
    }
  }

void spinEdge(ld aspd) { 
  if(downspin >  aspd) downspin =  aspd;
  if(downspin < -aspd) downspin = -aspd;
  View = spin(downspin) * View;
  }

void centerpc(ld aspd) { 
  if(vid.sspeed >= 4.99) aspd = 1000;
  DEBB(DF_GRAPH, (debugfile,"center pc\n"));
  hyperpoint H = ypush(-vid.yshift) * sphereflip * tC0(cwtV);
  if(H[0] == 0 && H[1] == 0) {
    return; // either already centered or direction unknown
    }
  ld R = hdist0(H); // = sqrt(H[0] * H[0] + H[1] * H[1]);
  if(R < 1e-9) {
    /* if(playerfoundL && playerfoundR) {
      
      } */
    spinEdge(aspd);
    fixmatrix(View);
    return;
    }
  
  if(euclid) {
    // Euclidean
    aspd *= (2+3*R*R);
    if(aspd > R) aspd = R;
    
    View[0][2] -= cwtV[0][2] * aspd / R;
    View[1][2] -= cwtV[1][2] * aspd / R;
        
    }
  
  else {
    aspd *= (1+R+(shmup::on?1:0));

    if(R < aspd) {
      View = gpushxto0(H) * View;
      }
    else 
      View = rspintox(H) * xpush(-aspd) * spintox(H) * View;
      
    fixmatrix(View);
    spinEdge(aspd);
    }
  }

void optimizeview() {
  
  DEBB(DF_GRAPH, (debugfile,"optimize view\n"));
  int turn = 0;
  ld best = INF;
  
  transmatrix TB = Id;
  
  for(int i=-1; i<S7; i++) {

    ld trot = -i * M_PI * 2 / (S7+.0);
    transmatrix T = i < 0 ? Id : spin(trot) * xpush(tessf) * pispin;
    hyperpoint H = View * tC0(T);
    if(H[2] < best) best = H[2], turn = i, TB = T;
    }

  if(turn >= 0) {
    View = View * TB;
    fixmatrix(View);
    viewctr = hsspin(viewctr, turn);
    viewctr = hsstep(viewctr, 0);
    }
  }

void addball(ld a, ld b, ld c) {
  hyperpoint h;
  ballmodel(h, a, b, c);
  for(int i=0; i<3; i++) h[i] *= vid.radius;
  curvepoint(h);
  }

void ballgeometry() {
  queuereset(vid.usingGL ? mdDisk : mdUnchanged, PPR_CIRCLE);
  for(int i=0; i<60; i++)
    addball(i * M_PI/30, 10, 0);
  for(double d=10; d>=-10; d-=.2)
    addball(0, d, 0);
  for(double d=-10; d<=10; d+=.2)
    addball(0, d, geom3::depth);
  addball(0, 0, -geom3::camera);
  addball(0, 0, geom3::depth);
  addball(0, 0, -geom3::camera);
  addball(0, -10, 0);
  addball(0, 0, -geom3::camera);
  queuecurve(darkena(0xFF, 0, 0x80), 0, PPR_CIRCLE);
  queuereset(pmodel, PPR_CIRCLE);
  }

void resetview() {
  DEBB(DF_GRAPH, (debugfile,"reset view\n"));
  View = Id;
  // EUCLIDEAN
  if(!euclid) 
    viewctr.h = cwt.c->master,
    viewctr.spin = cwt.spin;
  else centerover = cwt;
  // SDL_LockSurface(s);
  // SDL_UnlockSurface(s);
  }


void panning(hyperpoint hf, hyperpoint ht) {
  View = 
    rgpushxto0(hf) * rgpushxto0(gpushxto0(hf) * ht) * gpushxto0(hf) * View;
  playermoved = false;
  }

void fullcenter() {
  if(playerfound && false) centerpc(INF);
  else {
    bfs();
    resetview();
    drawthemap();
    centerpc(INF);
    }
  playermoved = true;
  }

transmatrix screenpos(ld x, ld y) {
  transmatrix V = Id;
  V[0][2] += (x - vid.xcenter) / vid.radius * (1+vid.alphax);
  V[1][2] += (y - vid.ycenter) / vid.radius * (1+vid.alphax);
  return V;
  }

transmatrix atscreenpos(ld x, ld y, ld size) {
  transmatrix V = Id;

  V[0][2] += (x - vid.xcenter);
  V[1][2] += (y - vid.ycenter);
  V[0][0] = size * 2 * hcrossf / crossf;
  V[1][1] = size * 2 * hcrossf / crossf;
  V[2][2] = vid.scrdist;
  if(euclid) V[2][2] /= EUCSCALE;

  return V;
  }

