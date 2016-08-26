// Hyperbolic Rogue
// geometrical constants

// Copyright (C) 2011-2012 Zeno Rogue, see 'hyper.cpp' for details


ld tessf, crossf, hexf, hcrossf;

// tessf: distance from heptagon center to another heptagon center
// hexf: distance from heptagon center to heptagon vertex
// crossf: distance from heptagon center to adjacent hexagon center

#define ALPHA (M_PI*2/7)

hyperpoint Crad[42];

transmatrix heptmove[7], hexmove[7];
transmatrix invheptmove[7], invhexmove[7];

// the results are:
// hexf = 0.378077 hcrossf = 0.620672 tessf = 1.090550

void precalc() {

  DEBB(DF_INIT, (debugfile,"precalc\n"));

  if(euclid) return;

  ld fmin = 1, fmax = 2;
  
  for(int p=0; p<100; p++) {
    ld f =  (fmin+fmax) / 2;
    hyperpoint H = xpush(f) * C0;
    ld v1 = intval(H, C0), v2 = intval(H, spin(2*M_PI/7)*H);
    if(v1 > v2) fmin = f; else fmax = f;
    }
  tessf = fmin;
  
  fmin = 0, fmax = 2;
  for(int p=0; p<100; p++) {
    ld f =  (fmin+fmax) / 2;
    hyperpoint H = spin(M_PI/7) * xpush(f) * C0;
    ld v1 = intval(H, C0), v2 = intval(H, xpush(tessf) * C0);
    if(v1 < v2) fmin = f; else fmax = f;
    }
  hcrossf = fmin;
  crossf = purehepta ? tessf : hcrossf;
  
  fmin = 0, fmax = tessf;
  for(int p=0; p<100; p++) {
    ld f =  (fmin+fmax) / 2;
    hyperpoint H = xpush(f) * C0;
    hyperpoint H1 = spin(2*M_PI/7) * H;
    hyperpoint H2 = xpush(tessf-f) * C0;
    ld v1 = intval(H, H1), v2 = intval(H, H2);
    if(v1 < v2) fmin = f; else fmax = f;
    }
  hexf = fmin;
  
  // printf("hexf = %.6Lf cross = %.6Lf tessf = %.6Lf\n", hexf, crossf, tessf);
  
  for(int i=0; i<42; i++)
    Crad[i] = spin(2*M_PI*i/42) * xpush(.4) * C0;
  for(int d=0; d<7; d++) 
    heptmove[d] = spin(-d * ALPHA) * xpush(tessf) * spin(M_PI);
  for(int d=0; d<7; d++) 
    hexmove[d] = spin(-d * ALPHA) * xpush(-crossf)* spin(M_PI);  

  for(int d=0; d<7; d++) invheptmove[d] = inverse(heptmove[d]);
  for(int d=0; d<7; d++) invhexmove[d] = inverse(hexmove[d]);
  }

transmatrix ddi(ld dir, ld dist) {
  // EUCLIDEAN
  if(euclid)
    return eupush(cos(M_PI*dir/42) * dist, -sin(M_PI*dir/42) * dist);
  else
    return spin(M_PI*dir/42) * xpush(dist) * spin(-M_PI*dir/42);
  }

// tesselation drawing

#define NUMFACE 500
transmatrix tess[NUMFACE];

void genTesselation() {
  int N = 1;
  tess[0] = Id;
  for(int i=0; i<N; i++) {
    for(int t=0; t<7; t++) {
      ld trot = (t % 8) * M_PI * 2 / 7.0;
      transmatrix T = spin(trot) * xpush(tessf) * /*spin(-trot) */ spin(M_PI) * tess[i];
      for(int j=0; j<N; j++) if(intval(T*C0, tess[j]*C0) < 0.1) goto nextt;
      // printf("%d:%d -> %d\n", i,t, N);
      tess[N] = T; N++;
      if(N == NUMFACE) return;
      nextt: ;
      }
    }
  }

struct ltd {
  hyperpoint P1;
  hyperpoint P2;
  int col;
  };

vector<ltd> lines;

void addline(hyperpoint P1, hyperpoint P2, int col) {
  ltd L;
  L.P1 = P1; L.P2 = P2; L.col = col;
  lines.push_back(L);
  }

void addlines() {

  // change the if(0) conditions to see the underlying structure
  
  if(0) for(int t =0; t<NUMFACE; t++) for(int u=1; u<8; u++) {
    addline(View * tess[t] * C0, View * tess[t] * tess[u] * C0, u==1 ? 0xA000 : 0x4000);
    }
  
  if(0) for(int t =0; t<NUMFACE; t++) for(int r=0; r<7; r++) {
    addline(
      View * tess[t] * spin((2*r+1)*M_PI/7) * xpush(crossf) * C0, 
      View * tess[t] * spin((2*r+3)*M_PI/7) * xpush(crossf) * C0, 
      0x808080);
    }
  
  if(1) for(int t =0; t<NUMFACE; t++) for(int r=0; r<7; r++) {
    addline(tess[t] * spin(M_PI*2*(r+1)/7) * xpush(hexf) * C0, tess[t] * spin(M_PI*2*r/7) * xpush(hexf) * C0, 0x404040);
    addline(tess[t] * spin(M_PI*2*r/7) * xpush(hexf) * C0, tess[t] * spin(M_PI*2*r/7) * xpush(tessf/2) * C0, 0x404040);
    }

  }

void initgeo() {
  precalc();
  genTesselation();
  addlines();
  }
