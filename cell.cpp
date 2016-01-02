// Hyperbolic Rogue
// Copyright (C) 2011-2012 Zeno Rogue, see 'hyper.cpp' for details

// cells the game is played on

int fix6(int a) { return (a+96)% 6; }
int fix7(int a) { return (a+84)% 7; }

struct cell : gcell {
  char type; // 6 for hexagons, 7 for heptagons
  unsigned char spn[7];
  heptagon *master;
  cell *mov[7]; // meaning very similar to heptagon::move
  };

int fixdir(int a, cell *c) { a %= c->type; if(a<0) a += c->type; return a; }

int cellcount = 0;

void initcell(cell *c); // from game.cpp

cell *newCell(int type, heptagon *master) {
  cell *c = new cell;
  cellcount++;
  c->type = type;
  c->master = master;
  for(int i=0; i<7; i++) c->mov[i] = NULL;
  initcell(c);
  return c;
  }

void merge(cell *c, int d, cell *c2, int d2) {
  c->mov[d] = c2;
  c->spn[d] = d2;
  c2->mov[d2] = c;
  c2->spn[d2] = d;
  }

typedef unsigned short eucoord;

cell*& euclideanAtCreate(eucoord x, eucoord y);

union heptacoder {
  heptagon *h;
  struct { eucoord x; eucoord y; } c;
  };

void decodeMaster(heptagon *h, eucoord& x, eucoord& y) {
  heptacoder u;
  u.h = h; x = u.c.x; y = u.c.y;
  }

heptagon* encodeMaster(eucoord x, eucoord y) {
  heptacoder u;
  u.c.x = x; u.c.y = y;
  return u.h;
  }

// very similar to createMove in heptagon.cpp
cell *createMov(cell *c, int d) {

  if(euclid && !c->mov[d]) {
    eucoord x, y;
    decodeMaster(c->master, x, y);
    for(int dx=-1; dx<=1; dx++)
    for(int dy=-1; dy<=1; dy++)
      euclideanAtCreate(x+dx, y+dy);
    if(!c->mov[d]) { printf("fail!\n"); }
    }
  
  if(c->mov[d]) return c->mov[d];
  else if(c->type == 7) {
    cell *n = newCell(6, c->master);

    c->mov[d] = n; n->mov[0] = c;
    c->spn[d] = 0; n->spn[0] = d;
    
    heptspin hs; hs.h = c->master; hs.spin = d;
    
    heptspin hs2 = hsstep(hsspin(hs, 3), 3);
    
    // merge(hs2.h->c7, hs2.spin, n, 2);
    
    hs2.h->c7->mov[hs2.spin] = n; n->mov[2] = hs2.h->c7;
    hs2.h->c7->spn[hs2.spin] = 2; n->spn[2] = hs2.spin;
    
    hs2 = hsstep(hsspin(hs, 4), 4);
    // merge(hs2.h->c7, hs2.spin, n, 4);
    hs2.h->c7->mov[hs2.spin] = n; n->mov[4] = hs2.h->c7;
    hs2.h->c7->spn[hs2.spin] = 4; n->spn[4] = hs2.spin;
    
    }
  
  else if(d == 5) {
    int di = fixrot(c->spn[0]+1);
    cell *c2 = createMov(c->mov[0], di);
    merge(c, 5, c2, fix6(c->mov[0]->spn[di] + 1));
    
    // c->mov[5] = c->mov[0]->mov[fixrot(c->spn[0]+1)]; 
    // c->spn[5] = fix6(c->mov[0]->spn[fixrot(c->spn[0]+1)] + 1);
    }
  
  else if(d == 1) {
    int di = fixrot(c->spn[0]-1);
    cell *c2 = createMov(c->mov[0], di);
    merge(c, 1, c2, fix6(c->mov[0]->spn[di] - 1));
    
    // c->mov[1] = c->mov[0]->mov[fixrot(c->spn[0]-1)]; 
    // c->spn[1] = fix6(c->mov[0]->spn[fixrot(c->spn[0]-1)] - 1);
    }
  
  else if(d == 3) {
    int di = fixrot(c->spn[2]-1);
    cell *c2 = createMov(c->mov[2], di);
    merge(c, 3, c2, fix6(c->mov[2]->spn[di] - 1));
    // c->mov[3] = c->mov[2]->mov[fixrot(c->spn[2]-1)];
    // c->spn[3] = fix6(c->mov[2]->spn[fixrot(c->spn[2]-1)] - 1);
    }
  return c->mov[d];
  }

cell *createMovR(cell *c, int d) {
  d %= 42; d += 42; d %= c->type;
  return createMov(c, d);
  }

cell *getMovR(cell *c, int d) {
  d %= 42; d += 42; d %= c->type;
  return c->mov[d];
  }

// similar to heptspin from heptagon.cpp
struct cellwalker {
  cell *c;
  int spin;
  cellwalker(cell *c, int spin) : c(c), spin(spin) {}
  cellwalker() {}
  };

void cwspin(cellwalker& cw, int d) {
  cw.spin = (cw.spin+d + 42) % cw.c->type;
  }

bool cwstepcreates(cellwalker& cw) {
  return cw.c->mov[cw.spin] == NULL;
  }

cell *cwpeek(cellwalker cw, int dir) {
  return createMov(cw.c, (cw.spin+42+dir) % cw.c->type);
  }

void cwstep(cellwalker& cw) {
  createMov(cw.c, cw.spin);
  int nspin = cw.c->spn[cw.spin];
  cw.c = cw.c->mov[cw.spin];
  cw.spin = nspin;
  }

void eumerge(cell* c1, cell *c2, int s1, int s2) {
  if(!c2) return;
  c1->mov[s1] = c2; c1->spn[s1] = s2;
  c2->mov[s2] = c1; c2->spn[s2] = s1;
  }       

struct euclideanSlab {
  cell* a[256][256];
  euclideanSlab() {
    for(int y=0; y<256; y++) for(int x=0; x<256; x++)
      a[y][x] = NULL;
    }
  ~euclideanSlab() {
    for(int y=0; y<256; y++) for(int x=0; x<256; x++)
      if(a[y][x]) delete a[y][x];
    }
  };

euclideanSlab* euclidean[256][256];

//  map<pair<eucoord, eucoord>, cell*> euclidean;

cell*& euclideanAt(eucoord x, eucoord y) {
  euclideanSlab*& slab(euclidean[y>>8][x>>8]);
  if(!slab) slab = new euclideanSlab;
  return slab->a[y&255][x&255];
  }

cell*& euclideanAtCreate(eucoord x, eucoord y) {
  cell*& c ( euclideanAt(x,y) );
  if(!c) {
    c = newCell(6, &origin);
    c->master = encodeMaster(x,y);
    euclideanAt(x,y) = c;
    eumerge(c, euclideanAt(x+1,y), 0, 3);
    eumerge(c, euclideanAt(x,y+1), 1, 4);
    eumerge(c, euclideanAt(x-1,y+1), 2, 5);
    eumerge(c, euclideanAt(x-1,y), 3, 0);
    eumerge(c, euclideanAt(x,y-1), 4, 1);
    eumerge(c, euclideanAt(x+1,y-1), 5, 2);
    }
  return c;
  }


// initializer (also inits origin from heptagon.cpp)
void initcells() {

  origin.s = hsOrigin;
  origin.emeraldval = 98;
  origin.zebraval = 40;
  origin.fiftyval = 0;
  for(int i=0; i<7; i++) origin.move[i] = NULL;
  origin.alt = NULL;
  origin.distance = 0;
  if(euclid)
    origin.c7 = euclideanAtCreate(0,0);
  else
    origin.c7 = newCell(7, &origin);
  
  // origin.emeraldval = 
  }

#define DEBMEM(x) // { x fflush(stdout); }

void clearcell(cell *c) {
  if(!c) return;
  DEBMEM ( printf("c%d %p\n", c->type, c); )
  for(int t=0; t<c->type; t++) if(c->mov[t]) {
    DEBMEM ( printf("mov %p [%p] S%d\n", c->mov[t], c->mov[t]->mov[c->spn[t]], c->spn[t]); )
    if(c->mov[t]->mov[c->spn[t]] != NULL &&
      c->mov[t]->mov[c->spn[t]] != c) {
        printf("cell error\n");
        exit(1);
        }
    c->mov[t]->mov[c->spn[t]] = NULL;
    }
  DEBMEM ( printf("DEL %p\n", c); )
  delete c;
  }

heptagon deletion_marker;

#include <queue>
void clearfrom(heptagon *at) {
  queue<heptagon*> q;
  q.push(at);
  at->alt = &deletion_marker;
//int maxq = 0;
  while(!q.empty()) {
    at = q.front(); 
//  if(q.size() > maxq) maxq = q.size();
    q.pop();
    DEBMEM ( printf("from %p\n", at); )
    for(int i=0; i<7; i++) if(at->move[i]) {
      if(at->move[i]->alt != &deletion_marker)
        q.push(at->move[i]);    
      at->move[i]->alt = &deletion_marker;
      DEBMEM ( printf("!mov %p [%p]\n", at->move[i], at->move[i]->move[at->spin[i]]); )
      if(at->move[i]->move[at->spin[i]] != NULL &&
        at->move[i]->move[at->spin[i]] != at) {
          printf("hept error\n");
          exit(1);
          }
      at->move[i]->move[at->spin[i]] = NULL;
      at->move[i] = NULL;
      }
    DEBMEM ( printf("at %p\n", at); )
    if(at->c7) {
      for(int i=0; i<7; i++)
        clearcell(at->c7->mov[i]);
      clearcell(at->c7);
      }
    DEBMEM ( printf("!DEL %p\n", at); )
    if(at != &origin) delete at;
    }
//printf("maxq = %d\n", maxq);
  }

void verifycell(cell *c) {
  int t = c->type;
  for(int i=0; i<t; i++) {
    cell *c2 = c->mov[i];
    if(c2) {
      if(t == 7) verifycell(c2);
      if(c2->mov[c->spn[i]] && c2->mov[c->spn[i]] != c) 
        printf("cell error %p %p\n", c, c2);
      }
    }
  }

void verifycells(heptagon *at) {
  for(int i=0; i<7; i++) if(at->move[i] && at->spin[i] == 0 && at->move[i] != &origin)
    verifycells(at->move[i]);
  for(int i=0; i<7; i++) if(at->move[i] && at->move[i]->move[at->spin[i]] && at->move[i]->move[at->spin[i]] != at) {
    printf("hexmix error %p %p %p\n", at, at->move[i], at->move[i]->move[at->spin[i]]);
    }
  verifycell(at->c7);
  }

bool ishept(cell *c) {
  // EUCLIDEAN
  if(euclid) {
    eucoord x, y;
    decodeMaster(c->master, x, y);
    return (short(y+2*x))%3 == 0;
    }
  else return c->type == 7;
  }

bool ishex1(cell *c) {
  // EUCLIDEAN
  if(euclid) {
    eucoord x, y;
    decodeMaster(c->master, x, y);
    short z = (short(y+2*x))%3;
    if(z<0) z += 3;
    return z == 1;
    }
  else return c->type == 7;
  }

void clearMemory() {
  extern void clearGameMemory();
  clearGameMemory();
  if(shmup::on) shmup::clearMemory();
  cleargraphmemory();
#ifndef MOBILE
  mapeditor::clearModelCells();
#endif
  // EUCLIDEAN
  if(euclid) { 
    for(int y=0; y<256; y++) for(int x=0; x<256; x++)
      if(euclidean[y][x]) { 
        delete euclidean[y][x];
        euclidean[y][x] = NULL;
        }
    }
  else {
    DEBMEM ( verifycells(&origin); )
    clearfrom(&origin);
    for(int i=0; i<size(allAlts); i++) clearfrom(allAlts[i]);
    allAlts.clear();
    }
  DEBMEM ( printf("ok\n"); )
  }

int emeraldval(cell *c) {
  if(euclid) return 0;
  if(c->type == 7)
    return c->master->emeraldval >> 3;
  else {
    return emerald_hexagon(
      emeraldval(createMov(c,0)),
      emeraldval(createMov(c,2)),
      emeraldval(createMov(c,4))
      );
    }
  }

int eudist(short sx, short sy) {
  int z0 = abs(sx);
  int z1 = abs(sy);
  int z2 = abs(sx+sy);
  return max(max(z0,z1), z2);
  }

int celldist(cell *c) {
  if(euclid) {
    eucoord x, y;
    decodeMaster(c->master, x, y);
    return eudist(x, y);
    }
  if(c->type == 7) return c->master->distance;
  int dx[3];
  for(int u=0; u<3; u++)
    dx[u] = createMov(c, u+u)->master->distance;
  int mi = min(min(dx[0], dx[1]), dx[2]);
  if(dx[0] > mi+2 || dx[1] > mi+2 || dx[2] > mi+2)
    return -1; // { printf("cycle error!\n"); exit(1); }
  if(dx[0] == mi+2 || dx[1] == mi+2 || dx[2] == mi+2)
    return mi+1;
  return mi;
  }

#define ALTDIST_BOUNDARY 99999
#define ALTDIST_UNKNOWN 99998

#define ALTDIST_ERROR 90000

// defined in 'game'
int euclidAlt(short x, short y);

int celldistAlt(cell *c) {
  if(euclid) {
    eucoord x, y;
    decodeMaster(c->master, x, y);
    return euclidAlt(x, y);
    }
  if(c->type == 7) return c->master->alt->distance;
  int dx[3];
  for(int u=0; u<3; u++) if(createMov(c, u+u)->master->alt == NULL)
    return ALTDIST_UNKNOWN;
  for(int u=0; u<3; u++)
    dx[u] = createMov(c, u+u)->master->alt->distance;
  int mi = min(min(dx[0], dx[1]), dx[2]);
  if(dx[0] > mi+2 || dx[1] > mi+2 || dx[2] > mi+2)
    return ALTDIST_BOUNDARY; // { printf("cycle error!\n"); exit(1); }
  if(dx[0] == mi+2 || dx[1] == mi+2 || dx[2] == mi+2)
    return mi+1;
  return mi;
  }

#define GRAIL_FOUND 0x4000
#define GRAIL_RADIUS_MASK 0x3FFF

int dirfromto(cell *cfrom, cell *cto) {
  for(int i=0; i<cfrom->type; i++) if(cfrom->mov[i] == cto) return i;
  return -1;
  }

// === FIFTYVALS ===

unsigned bitmajority(unsigned a, unsigned b, unsigned c) {
  return (a&b) | ((a^b)&c);
  }

int fiftyval(cell *c) {
  if(euclid) return 0;
  if(c->type == 7)
    return c->master->fiftyval;
  else {
    return bitmajority(
      fiftyval(createMov(c,0)), 
      fiftyval(createMov(c,2)), 
      fiftyval(createMov(c,4))) + 512;
    }
  }

int cdist50(cell *c) {
  if(euclid) {
    eucoord x, y;
    decodeMaster(c->master, x, y);
    int ix = short(x) + 99999 + short(y);
    int iy = short(y) + 99999;
    char palacemap[3][10] = {
      "012333321",
      "112322232",
      "222321123"
      };
    ix += (iy/3) * 3;
    iy %= 3; ix %= 9;
    return palacemap[iy][ix] - '0';
    }
  if(c->type == 7) return cdist50(fiftyval(c));
  int a0 = cdist50(createMov(c,0));
  int a1 = cdist50(createMov(c,2));
  int a2 = cdist50(createMov(c,4));
  if(a0 == 0 || a1 == 0 || a2 == 0) return 1;
  return a0+a1+a2-5;
  }

int land50(cell *c) {
  if(c->type == 7) return land50(fiftyval(c));
  else {
    if(cdist50(createMov(c,0)) < 3) return land50(createMov(c,0));
    if(cdist50(createMov(c,2)) < 3) return land50(createMov(c,2));
    if(cdist50(createMov(c,4)) < 3) return land50(createMov(c,4));
    return 0;
    }
  }

int polara50(cell *c) {
  if(c->type == 7) return polara50(fiftyval(c));
  else {
    if(cdist50(createMov(c,0)) < 3) return polara50(createMov(c,0));
    if(cdist50(createMov(c,2)) < 3) return polara50(createMov(c,2));
    if(cdist50(createMov(c,4)) < 3) return polara50(createMov(c,4));
    return 0;
    }
  }

int polarb50(cell *c) {
  if(euclid) return true;
  if(c->type == 7) return polarb50(fiftyval(c));
  else {
    if(cdist50(createMov(c,0)) < 3) return polarb50(createMov(c,0));
    if(cdist50(createMov(c,2)) < 3) return polarb50(createMov(c,2));
    if(cdist50(createMov(c,4)) < 3) return polarb50(createMov(c,4));
    return 0;
    }
  }

int elhextable[28][3] = {
  {0,1,2}, {1,2,9}, {1,9,-1}, {1,8,-1}, {1,-1,-1}
  };

int fiftyval049(cell *c) {
  if(c->type == 7) return fiftyval(c) / 32;
  else {
    int a[3], qa=0;
    int pa = polara50(c), pb = polarb50(c);
    for(int i=0; i<6; i+=2) {
      cell *c2 = c->mov[i];
      if(polara50(c2) == pa && polarb50(c2) == pb)
        a[qa++] = fiftyval049(c2);
      }
    // 0-1-2
    sort(a, a+qa);
    if(qa == 1) return 43+a[0]-1;
    if(qa == 2 && a[1] == a[0]+7) return 36+a[0]-1;
    if(qa == 2 && a[1] != a[0]+7) return 29+a[0]-1;
    if(a[1] == 1 && a[2] == 7)
      return 15 + 6;
    if(a[2] >= 1 && a[2] <= 7)
      return 15 + a[1]-1;
    if(a[0] == 1 && a[1] == 7 && a[2] == 8)
      return 22;
    if(a[1] <= 7 && a[2] >= 8)
      return 22 + a[1]-1;
    return 0;
    }
  }

/*
{0,1,2} 15+0..15+6
{1,2,9},22+0..22+6
{1,9}   29+0..29+6
{1,8}   36+0..36+6
{1}     43+0..43+6
*/

// zebraval

int zebra40(cell *c) {
  if(c->type == 7) return (c->master->zebraval/10);
  else { 
    int ii[3], z;
    ii[0] = (c->mov[0]->master->zebraval/10);
    ii[1] = (c->mov[2]->master->zebraval/10);
    ii[2] = (c->mov[4]->master->zebraval/10);
    for(int r=0; r<2; r++) 
      if(ii[1] < ii[0] || ii[2] < ii[0]) 
        z = ii[0], ii[0] = ii[1], ii[1] = ii[2], ii[2] = z;
    for(int i=0; i<28; i++)
      if(zebratable6[i][0] == ii[0] && zebratable6[i][1] == ii[1] && 
         zebratable6[i][2] == ii[2]) {
           int ans = 16+i;
           // if(ans >= 40) ans ^= 2;
           // if(ans >= 4  && ans < 16) ans ^= 2;
           return ans;
           }
    return 0;
    }
  }

int zebra3(cell *c) {
  if(c->type == 7) return (c->master->zebraval/10)/4;
  else { 
    int ii[3];
    ii[0] = (c->mov[0]->master->zebraval/10)/4;
    ii[1] = (c->mov[2]->master->zebraval/10)/4;
    ii[2] = (c->mov[4]->master->zebraval/10)/4;
    if(ii[0] == ii[1]) return ii[0];
    if(ii[1] == ii[2]) return ii[1];
    if(ii[2] == ii[0]) return ii[2];
    return 0;
    }
  }

bool randpattern(cell *c, int rval) {
  int i, sw=0;
  switch(rval & 3) {
    case 0:
      return rand() < rval;
    case 1:
      i = zebra40(c);
      if(i&1) { if(rval&4) sw^=1; i &= ~1; }
      if(i&2) { if(rval&8) sw^=1; i &= ~2; }
      i >>= 2;
      i--; i /= 3;
      if(rval & (16<<i)) sw^=1;
      return sw;
    case 2:
      i = emeraldval(c);
      if(i&1) { if(rval&4) sw^=1; i &= ~1; }
      if(i&2) { if(rval&8) sw^=1; i &= ~2; }
      if(rval & (16<<i)) sw^=1;
      return sw;
    case 3:
      if(polara50(c)) { if(rval&4) sw^=1; }
      if(polarb50(c)) { if(rval&8) sw^=1; }
      int i = fiftyval049(c); i += 6; i /= 7;
      if(rval & (16<<i)) sw^=1;
      return sw;
    }
  return 0;
  }

int randpatternCode(cell *c, int rval) {
  switch(rval & 3) {
    case 1:
      return zebra40(c);
    case 2:
      return emeraldval(c);
    case 3:
      return fiftyval049(c) + (polara50(c)?50:0) + (polarb50(c)?1000:0);
    }
  return 0;  
  }

#define RANDITER 31

char rpm_memoize[3][256][RANDITER+1];

void clearMemoRPM() {
  for(int a=0; a<3; a++) for(int b=0; b<256; b++) for(int i=0; i<RANDITER+1; i++)
    rpm_memoize[a][b][i] = 2;
  }

extern int randompattern[landtypes];

bool randpatternMajority(cell *c, int ival, int iterations) {
  int rval = 0;
  if(ival == 0) rval = randompattern[laCaves];
  if(ival == 1) rval = randompattern[laLivefjord];
  if(ival == 2) rval = randompattern[laEmerald];
  if((rval&3) == 0) return randpattern(c, rval);
  int code = randpatternCode(c, rval);
  char& memo(rpm_memoize[ival][code][iterations]);
  if(memo < 2) return memo;
  int z = 0;
  if(iterations) for(int i=0; i<c->type; i++) {
    if(randpatternMajority(createMov(c,i), ival, iterations-1))
      z++;
    else
      z--;
    }
  if(z!=0) memo = (z>0);
  else memo = randpattern(c, rval);
  // printf("%p] rval = %X code = %d iterations = %d result = %d\n", c, rval, code, iterations, memo);
  return memo;
  }
