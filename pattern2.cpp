// Hyperbolic Rogue -- patterns
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file pattern2.cpp
 *  \brief various functions computing the standings of individual cells with respect to various patterns
 *
 *  Patterns include simple ones (hexagon or heptagon, three-colorings, Chessboard)
 *  as well as more complex ones (Emerald, Palace, Field Pattern)
 */

#include "hyper.h"
namespace hr {

EX int ctof(cell *c) {
  #if CAP_IRR
  if(IRREGULAR) return irr::ctof(c);
  #endif
  if(PURE) return 1;
  // if(euclid) return 0;
  if(!c) return 1;
  if(binarytiling) return c->type == 7;
  return ishept(c) ? 1 : 0;
  // c->type == 6 ? 0 : 1;
  }

EX int ctof012(cell *c) {
  return ishept(c)?1:ishex1(c)?0:2;
  }

int gp_threecolor() {
  if(!GOLDBERG) return 0;
  #if CAP_GP
  if(S3 == 3 && (gp::param.first - gp::param.second) % 3 == 0) return 2;
  #endif
  return 1;
  }

int eupattern(cell *c) {
  auto co = euc2_coordinates(c);
  int x = co.first, y = co.second;
  if(a4)
    return ((x&1) + 2*(y&1)) % 3;
  else
    return gmod(y-x, 3);
  }

int eupattern4(cell *c) {
  auto co = euc2_coordinates(c);
  int x = co.first, y = co.second;
  return (x&1) + ((y&1)) * 2;
  }

EX bool ishept(cell *c) {
  // EUCLIDEAN
  if(euclid && PURE) return eupattern(c) == 0;
  else if(hybri) { cell *c1 = hybrid::get_where(c).first; return c1 == c1->master->c7; }
  else return c == c->master->c7;
  }

EX bool ishex1(cell *c) {
  // EUCLIDEAN
  if(euclid && PURE) return eupattern(c) == 1;
  #if CAP_GP
  else if(GOLDBERG) return c->master->c7 != c && !pseudohept(c->move(0));
  #endif
  else return c->master->c7 != c;
  }

bool ishex2(cell *c) {
  // EUCLIDEAN
  if(euclid && PURE) return eupattern(c) == 1;
  #if CAP_GP
  else if(GOLDBERG) return c->master->c7 != c && gp::pseudohept_val(c) == 1;
  #endif
  else return c->master->c7 != c;
  }

int chessvalue(cell *c) {
  #if CAP_ARCM
  if(arcm::in()) 
    return arcm::chessvalue(c);
  else
  #endif
    return celldist(c) & 1;
  }

EX int emeraldval(heptagon *h) { return h->emeraldval >> 3; }

EX int emeraldval(cell *c) {
  if(euclid) return eupattern(c);
  if(sphere) return 0;
  if(ctof(c))
    return emeraldval(c->master);
  else {
    auto ar = gp::get_masters(c);
    return emerald_hexagon(
      emeraldval(ar[0]),
      emeraldval(ar[1]),
      emeraldval(ar[2])
      );
    }
  }

// === FIFTYVALS ===

unsigned bitmajority(unsigned a, unsigned b, unsigned c) {
  return (a&b) | ((a^b)&c);
  }

int eufifty(cell *c) {
  auto co = euc2_coordinates(c);
  int x = co.first, y = co.second;
  int ix = x + 99999 + y;
  int iy = y + 99999;
  if(c->land == laWildWest) 
    return (ix + iy * 26 + 28) % 37;
  else {
    ix += (iy/3) * 3;
    iy %= 3; ix %= 9;
    return iy * 9 + ix;
    }
  }

int fiftyval(cell *c) {
  if(euclid) return eufifty(c) * 32;
  if(sphere || S7>7 || S6>6) return 0;
  if(ctof(c))
    return c->master->fiftyval;
  else {
    auto ar = gp::get_masters(c);
    return bitmajority(
      ar[0]->fiftyval, 
      ar[1]->fiftyval, 
      ar[2]->fiftyval) + 512;
    }
  }

EX int cdist50(cell *c) {
  if(sphere || S7>7 || S6>6) return 0;
  if(euclid) {
    if(c->land == laWildWest) 
      return "0123333332112332223322233211233333322"[eufifty(c)] - '0';
    else return "012333321112322232222321123"[eufifty(c)] - '0';
    }
  if(ctof(c)) return cdist50(c->master->fiftyval);
  auto ar = gp::get_masters(c);
  int a0 = cdist50(ar[0]->fiftyval);
  int a1 = cdist50(ar[1]->fiftyval);
  int a2 = cdist50(ar[2]->fiftyval);
  if(a0 == 0 || a1 == 0 || a2 == 0) return 1;
  return a0+a1+a2-5;
  }

int land50(cell *c) {
  if(sphere || euclid) return 0;
  else if(ctof(c)) return land50(fiftyval(c));
  else {
    auto ar = gp::get_masters(c);
    for(int i=0; i<3; i++)
      if(cdist50(ar[i]->fiftyval) < 3) return land50(ar[i]->fiftyval);
    return 0;
    }
  }

EX bool polara50(cell *c) {
  if(sphere || euclid || S7>7 || S6>6) return false;
  else if(NONSTDVAR) return polara50(fiftyval(c->master->c7));
  else if(ctof(c)) return polara50(fiftyval(c));
  else {
    auto ar = gp::get_masters(c);
    for(int i=0; i<3; i++)
      if(cdist50(ar[i]->fiftyval) < 3) return polara50(ar[i]->fiftyval);
    return false;
    }
  }

EX bool polarb50(cell *c) {
  if(euclid) return true;
  if(sphere || euclid || S7>7 || S6>6) return true;
  else if(NONSTDVAR) return polarb50(fiftyval(c->master->c7));
  else if(ctof(c)) return polarb50(fiftyval(c));
  else {
    auto ar = gp::get_masters(c);
    for(int i=0; i<3; i++)
      if(cdist50(ar[i]->fiftyval) < 3) return polarb50(ar[i]->fiftyval);
    return false;
    }
  }

int elhextable[28][3] = {
  {0,1,2}, {1,2,9}, {1,9,-1}, {1,8,-1}, {1,-1,-1}
  };

EX int fiftyval049(heptagon *h) {
  int i = h->fiftyval / 32;
  if(i <= 7) return i;
  if(quotient) return 0;
  vector<int> allcodes;
  for(int k=0; k<7; k++) {
    heptagon *h2 = createStep(h, k);
    if(polara50(h2->fiftyval) == polara50(h->fiftyval) && polarb50(h2->fiftyval) == polarb50(h->fiftyval))
      allcodes.push_back(fiftyval049(h2));
    }
  int d = allcodes[1] - allcodes[0];
  if(d == -1 || d == 6) swap(allcodes[0], allcodes[1]);
  // printf("%d,%d: %d\n", allcodes[0], allcodes[1], allcodes[0] + 7);
  return allcodes[0] + 7;
  }

EX int fiftyval049(cell *c) {
  if(euclid) return fiftyval(c) / 32;
  else if(ctof(c)) return fiftyval049(c->master);
  else if(sphere) return 0;
  else {
    int a[3], qa=0;
    bool pa = polara50(c);
    bool pb = polarb50(c);
    auto ar = gp::get_masters(c);
    for(int i=0; i<3; i++)
      if(polara50(ar[i]->fiftyval) == pa && polarb50(ar[i]->fiftyval) == pb)
        a[qa++] = fiftyval049(ar[i]);
    // 0-1-2
    sort(a, a+qa);
    if(qa == 1) return 43+a[0]-1;
    if(qa == 2 && a[1] == a[0]+7) return 36+a[0]-1;
    if(qa == 2 && a[1] != a[0]+7) return 29+a[0]-1;
    // 3: zgodnie
    // 1: zgodnie
    // 0: przeciwnie
    // 2: przeciwnie
    // 168: 
    if(a[1] == 1 && a[2] == 7) 
      return 15 + 6; // (polarb50(c) ? 0 : 6);
    if(a[2] >= 1 && a[2] <= 7) {
      return 15 + a[1] - 1; // (polarb50(c) ? a[1]%7 : a[1]-1);
      }
    if(a[0] == 1 && a[1] == 7 && a[2] == 8)
      return 22;
    if(a[0] == 1 && a[1] == 7 && a[2] == 14)
      return 22; 
    if(a[1] <= 7 && a[2] >= 8)
      return 22 + a[1]-1;
    return 0;
    }
  }

EX int fiftyval200(cell *c) {
  int i = fiftyval049(c);
  i *= 4;
  if(polara50(c)) i|=1;
  if(polarb50(c)) i|=2;
  return i;
  }

/*
{0,1,2} 15+0..15+6
{1,2,9},22+0..22+6
{1,9}   29+0..29+6
{1,8}   36+0..36+6
{1}     43+0..43+6
*/

// zebraval

int dir_bitrunc457(cell *c) {
  if(GOLDBERG) return c->master->zebraval / 10;
  int wset = 0;
  int has1 = 0;
  for(int i=0; i<4; i++) {
    int z = zebra40(createMov(c, i*2));
    if(z&1) has1 = 1;
    if(z&2) wset |= (1<<i);
    }
  if(wset == 0) return -8-has1;
  if(wset == 15) return -10-has1;
  if(wset == 3) return 1;
  if(wset == 6) return 3;
  if(wset == 12) return 5;
  if(wset == 9) return 7;
  return 0;
  }

int val46(cell *c);

EX int zebra40(cell *c) {
  if(euclid) return pattern_threecolor(c);
  else if(IRREGULAR) return c->master->zebraval/10;
  else if(a46) {
    int v = val46(c);
    if(v<4) return v;
    else return 4+(v-4)/2;
    }
  else if(ctof(c)) return (c->master->zebraval/10);
  else if(a4) {
    if(GOLDBERG) return zebra40(c->master->c7);
    int ws = dir_bitrunc457(c);
    if(ws < 0) return -ws;
    int tot = 0; 
    array<int, 4> zebras;
    for(int i=0; i<4; i++) {
      zebras[i] = zebra40(createMov(c, i*2));
      tot += zebras[i];
      }

    // break cycles
    int cod = 0;
    int mo = 0; for(int i=0; i<4; i++) if(zebras[i] < zebras[mo]) mo = i;
    for(int i=0; i<4; i++) for(int j=1; j<i; j++)
      if(zebras[(mo+i)&3] < zebras[(mo+j)&3]) cod ^= 4;
      
    if(tot == 0+2+4+6) return 16+cod;
    if(tot == 1+3+5+7) return 19+cod;
    if(tot == 0+1+2+3) return 18+cod;
    if(tot == 4+5+6+7) return 17+cod;
    return 24;
    }
  else if(sphere) return 0;
  else if(S3 == 4 && S7 == 6) {
    return 8 + ((c->master->zebraval / 10 + c->c.spin(0))%2) * 2;
    }
  else {
    int ii[3], z;
    auto ar = gp::get_masters(c);
    ii[0] = (ar[0]->zebraval/10);
    ii[1] = (ar[1]->zebraval/10);
    ii[2] = (ar[2]->zebraval/10);
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

EX int zebra3(cell *c) {
  if(euclid) return 0;
  else if(ctof(c)) return (c->master->zebraval/10)/4;
  else if(euclid || sphere || S7>7 || S6>6) return 0;
  else { 
    int ii[3];
    auto ar = gp::get_masters(c);
    ii[0] = (ar[0]->zebraval/10)/4;
    ii[1] = (ar[1]->zebraval/10)/4;
    ii[2] = (ar[2]->zebraval/10)/4;
    if(ii[0] == ii[1]) return ii[0];
    if(ii[1] == ii[2]) return ii[1];
    if(ii[2] == ii[0]) return ii[2];
    return 0;
    }
  }

#if CAP_FIELD
EX namespace fieldpattern {

EX pair<int, bool> fieldval(cell *c) {
  if(ctof(c)) return make_pair(int(c->master->fieldval), false);
  else return make_pair(btspin(c->master->fieldval, c->c.spin(0)), true);
  }

EX int fieldval_uniq(cell *c) {
  if(experimental) return 0;
  else if(hybri) { 
    auto c1 = hybrid::get_where(c).first; 
    return PIU ( fieldval_uniq(c1) );
    }
  else if(sphere) {
    if(arcm::in()) return c->master->fiftyval;
    #if CAP_IRR
    else if(IRREGULAR) return irr::cellindex[c];
    #endif
    #if CAP_GP
    else if(GOLDBERG) return (get_code(gp::get_local_info(c)) << 8) | (c->master->fieldval / S7);
    #endif
    if(ctof(c)) return c->master->fieldval;
    else return createMov(c, 0)->master->fieldval + 256 * createMov(c,2)->master->fieldval + (1<<16) * createMov(c,4)->master->fieldval;
    }
  else if(euclid && !penrose && !arcm::in()) {
    auto p = euc2_coordinates(c);
    if(bounded) return p.first + (p.second << 16);
    return gmod(p.first - 22 * p.second, 3*127);
    }
  else if(binarytiling || arcm::in() || nil || S3 >= OINF || (cgflags & qIDEAL)) return 0;
  else if(&currfp == &fp_invalid) return 0;
  else if(WDIM == 3) return c->master->fieldval;
  else if(ctof(c) || NONSTDVAR) return c->master->fieldval/S7;
  else {
    int z = 0;
    for(int u=0; u<S6; u+=2) 
      z = max(z, btspin(createMov(c, u)->master->fieldval, c->c.spin(u)));
    return -1-z;
    }
  }

EX int fieldval_uniq_rand(cell *c, int randval) {
  if(hybri) { 
    auto c1 = hybrid::get_where(c).first; 
    return PIU ( fieldval_uniq_rand(c1, randval) );
    }
  if(sphere || euclid || NONSTDVAR) 
    // we do not care in these cases
    return fieldval_uniq(c);
  if(ctof(c)) return currfp.gmul(c->master->fieldval, randval)/7;
  else {
    int z = 0;
    for(int u=0; u<6; u+=2) 
      z = max(z, btspin(currfp.gmul(createMov(c, u)->master->fieldval, randval), c->c.spin(u)));
    return -1-z;
    }
  }

pair<int, int> subval(cell *c, int _subpathid = subpathid, int _subpathorder = subpathorder) {
  if(!ctof(c)) {
    auto m = subval(createMov(c, 0));
    for(int u=2; u<S6; u+=2)
      m = min(m, subval(createMov(c, u)));
    return m;
    }
  else {
    pair<int, int> pbest, pcur;
    pcur.first = c->master->fieldval;
    pcur.second = 0;
    pbest = pcur;
    for(int i=0; i<_subpathorder; i++) {
      pcur.first = currfp.gmul(pcur.first, _subpathid);
      pcur.second++;
      if(pcur < pbest) pbest = pcur;
      }
    return pbest;
    }
  }

EX }
#endif

EX int getHemisphere(heptagon *h, int which) {
  int id = h->fiftyval;
  if(S7 == 5) {
    int hemitable[3][12] = {
      { 6, 3, 3, 3, 3, 3,-6,-3,-3,-3,-3,-3},
      { 6, 3, 6, 3, 0, 0,-6,-3,-6,-3, 0, 0},
      {-3, 0, 3, 0,-6,-6, 3, 0,-3, 0, 6, 6}
      };
    return hemitable[which][id];
    }
  else if(S7 == 4 && which < 6) {
    int hemitable[3][6] = {
      { 2, 2, 2,-1,-1,-1},
      { 2,-1, 2, 2,-1,-1},
      { 2,-1,-1, 2, 2,-1},
      };
    return hemitable[which][id];
    }
  else if(S7 == 3) {
    int hemitable[3][4] = {
      { 2, 2,-1,-1},
      { 2,-1, 2,-1},
      { 2,-1,-1, 2},
      };
    return hemitable[which][id];
    }
  else return 0;
  }

EX int getHemisphere(cell *c, int which) {
  if(euclid && quotient) return 0;
  if(hybri) { auto d = hybrid::get_where(c); return PIU(getHemisphere(d.first, which)); }
  if(WDIM == 3 && !hybri) {
    hyperpoint p = tC0(calc_relative_matrix(c, currentmap->gamestart(), C0));
    return int(p[which] * 6  + 10.5) - 10;
    }
  if(which == 0 && GOLDBERG && has_nice_dual()) {
    set<cell*> visited;
    vector<cell*> q;
    vector<int> type;
    auto visit = [&] (cell *c, int t) {
      if(visited.count(c)) return;
      visited.insert(c);
      q.push_back(c);
      type.push_back(t);
      };
      
    cellwalker cw(currentmap->gamestart(), 0);
    int ct = 1;
    visit(cw.at, ct);
    do {
      cw = cw + wstep;
      visit(cw.at, -ct);
      cw = cw + (2*ct) + wstep + ct;
      ct = -ct;
      }
    while(cw.at != currentmap->gamestart());    
    for(int i=0; i<isize(q); i++)
      forCellCM(c2, q[i])
        if(pseudohept(q[i]) || pseudohept(c2))
          visit(c2, type[i]);
    for(int i=0; i<isize(q); i++) if(q[i] == c) return type[i];
    return 0;
    }
  if(ctof(c)) 
    return getHemisphere(c->master, which);
  else {
    int score = 0;
    if(0) ;
    #if CAP_GP
    else if(GOLDBERG) {
      auto li = gp::get_local_info(c);
      gp::be_in_triangle(li);
      auto corner = cgi.gpdata->corners * gp::loctoh_ort(li.relative);
      ld scored = 
        corner[0] * getHemisphere(c->master->c7, which)
      + corner[1] * getHemisphere(c->master->move(li.last_dir)->c7, which)
      + corner[2] * getHemisphere(c->master->modmove(li.last_dir+1)->c7, which);
      int score = int(scored + 10.5) - 10;
      ld error = scored - score;
      if(score == 0 && error > .001) score++;
      if(score == 0 && error < -.001) score--;
      return score;
      }
    #endif
    #if CAP_IRR
    else if(IRREGULAR) {
      auto m = irr::get_masters(c);
      for(int i=0; i<3; i++)
        score += getHemisphere(m[i], which);
      return score / 3;
      }
    #endif
    else {
      for(int i=0; i<6; i+=2) 
        score += getHemisphere(c->move(i), which) * (c->c.mirror(i) ? -1 : 1);
      return score/3;
      }
    }
  }

EX namespace patterns {

  #if HDR
  enum ePattern : char {
    PAT_NONE = 0,
    PAT_TYPES = 'T',
    PAT_ZEBRA = 'z',
    PAT_EMERALD = 'f',
    PAT_PALACE = 'p',
    PAT_FIELD = 'F',
    PAT_DOWN = 'H',
    PAT_COLORING = 'C',
    PAT_SIBLING = 'S',
    PAT_CHESS = 'c',
    PAT_SINGLETYPE = 't'
    };  

  static const int SPF_ROT = 1;
  static const int SPF_SYM01 = 2;
  static const int SPF_SYM02 = 4;
  static const int SPF_SYM03 = 8;
  static const int SPF_CHANGEROT = 16;
  static const int SPF_TWOCOL = 32;
  static const int SPF_EXTRASYM = 64;
  static const int SPF_ALTERNATE = 128;
  static const int SPF_FOOTBALL = 256;
  static const int SPF_FULLSYM = 512;
  static const int SPF_DOCKS = 1024;
  static const int SPF_NO_SUBCODES = 2048;

  static const int SPF_SYM0123 = SPF_SYM01 | SPF_SYM02 | SPF_SYM03;

  struct patterninfo {
    int id;
    int dir;
    bool reflect;
    int symmetries;
    };
  #endif

  void valSibling(cell *c, patterninfo& si, int sub, int pat) {
    if(ctof(c)) {
      int d = c->master->fieldval;
      si.id = (d < siblings[d]) ? 0 : 1;
      if(sub & SPF_ROT) si.id = 0;
      for(int i=0; i<S7; i++) {
        int di = c->master->move(i)->fieldval;
        if(di == siblings[d]) si.dir = i;
        }
      si.reflect = false;
      }
    else {
      int ids = 0, tids = 0, td = 0;
      for(int i=0; i<S3; i++) {
        int d = c->move(2*i)->master->fieldval;
        ids |= (1<<d); tids += d;
        }
      for(int i=0; i<S3; i++) {
        int d = c->move(2*i)->master->fieldval;
        if(ids & (1<<siblings[d])) td += d;
        }
      if(td) {
        si.id = 4;
        for(int i=0; i<S3; i++) {
          int d = c->move(2*i)->master->fieldval;
          if(!(ids & (1<<siblings[d]))) si.dir = 2*i;
          }
        /* if(!(sub & SPF_ROT)) {
          int d0 = c->modmove(si.dir+2)->master->fieldval;
          if(d0 < siblings[d0]) si.id += 8;
          } */
        si.reflect = false;
        }
      else {
        si.id = 8;
        si.dir = 0; // whatever 
        patterninfo si2;
        valSibling(c->move(0), si2, sub, pat);
        int di = si2.dir - c->c.spin(0);
        di %= S7; 
        if(di<0) di += S7;
        if(pat == PAT_SIBLING) si.reflect = di > S7/2;
        if(sub & SPF_ROT) si.symmetries = 2;
        }
      }
    }
  
  EX int downdir(cell *c, const cellfunction& cf) {
    return parent_id(c, 1, cf) + 1;
    }

  void applySym0123(int& i, int sub) {
    bool sym01 = sub & SPF_SYM01;
    bool sym02 = sub & SPF_SYM02;
    bool sym03 = sub & SPF_SYM03;
    if((sym01?1:0)+(sym02?1:0)+(sym03?1:0) >= 2) i &= ~3;
    if(sym01 && (i&1)) i ^= 1;
    if(sym02 && (i&2)) i ^= 2;
    if(sym03 && (i&2)) i ^= 3;
    }

  void applyAlt(patterninfo& si, int sub, int pat) {
    if(sub & SPF_ALTERNATE) {
      si.id += 4;
      si.id %= 12;
      }
    if(pat == PAT_COLORING && (sub & SPF_FOOTBALL)) {
      if(si.id == 4) si.dir++;
      si.id = !si.id;
      if(si.id && (sub & SPF_EXTRASYM))
        si.symmetries = si.id ? 1 : 2;
      return;
      }
    }

  void val46(cell *c, patterninfo &si, int sub, int pat) {
    if(ctof(c)) {
      si.id = c->master->emeraldval >> 1;
      applySym0123(si.id, sub);
      if(sub & SPF_CHANGEROT)
        si.dir = (c->master->emeraldval&1);
      else
        si.dir = (c->master->emeraldval&1) ^ (c->master->emeraldval>>1);
      si.symmetries = 2;
      applyAlt(si, sub, pat);
      /* printf("[%3d] ", c->master->emeraldval);
      for(int i=0; i<6; i++) printf("%2d", val46(createMov(c, i)));
      printf("\n"); */
      }
    else {
      si.id = ((c->master->emeraldval & 1) ^ ((c->master->emeraldval & 2)>>1) ^ (c->c.spin(0)&1)) ? 8 : 4;
      si.dir = ((c->move(0)->master->emeraldval + c->c.spin(0)) & 1) ? 2 : 0;
      if(createMov(c, si.dir)->master->emeraldval & 4)
        si.dir += 4;

      if((sub & SPF_TWOCOL) && (pat == PAT_COLORING)) si.id = 4;
      else if(pat == PAT_COLORING && si.id == 4) si.dir++;
      
      if(sub & SPF_SYM01) si.symmetries = 2;
      else if(sub & SPF_SYM03) si.symmetries = 2;
      else if(sub & SPF_SYM02) si.symmetries = 4;
      
      applyAlt(si, sub, pat);
      }
    }
  
  // if(a46) return patterndir46(c, w == PAT_ZEBRA ? 3 : w == PAT_PALACE ? 2 : 1);

  int inr(int a, int b, int c) { return a >= b && a < c; }

  void val457(cell *c, patterninfo &si, int sub) {
    si.id = zebra40(c);
    if(inr(si.id, 8, 12)) si.symmetries = 4;
    applySym0123(si.id, sub);
    if(sub & SPF_ROT) {
      if(si.id >= 4 && si.id < 7) si.id -= 4;
      if(si.id >= 20 && si.id < 23) si.id -= 4;
      }
    if(ctof(c)) {
      for(int i=0; i<c->type; i++)
        if((zebra40(createStep(c->master, i + S7/2)->c7)&2) == (zebra40(createStep(c->master, i + 1 + S7/2)->c7)&2))
          si.dir = i;
      }
    else {
      int d = dir_bitrunc457(c);
      if(d >= 0) si.dir = d;
      else si.dir = (zebra40(createMov(c, 0)) & 4) ? 2 : 0;
      }
    }
  
  EX void val38(cell *c, patterninfo &si, int sub, int pat) {
    bool symRotation = sub & SPF_ROT;

    if(ctof(c)) {
      if(!symRotation) 
        si.id = (c->master->fiftyval >> 1) & 3;
      else
        si.id = 0;
      if(!BITRUNCATED && gp_threecolor() != 2)
        si.id *= 4;
      else 
        si.id += 4;
      si.dir = (pat == PAT_COLORING && BITRUNCATED ? 1 : 0) + (c->master->fiftyval | (c->master->fiftyval & 8 ? 0 : 2));
      si.symmetries = 2;
      si.id += 8;
      si.id %= 12;
      applyAlt(si, sub, pat);
      if((sub & SPF_DOCKS) && (c->master->fiftyval & 32))
        si.id += 16, si.symmetries = 4;
      }
    else {
      int sp = c->c.spin(0);
      #if CAP_GP
      if(GOLDBERG) {
        sp = gp::last_dir(c);
        sp ^= int(ishex2(c));
        }
      #endif
      if(geometry == gBolza2 && (!GOLDBERG || gp_threecolor() == 2)) {
        patterninfo si0;
        patterninfo si1;
        #if CAP_GP
        if(GOLDBERG) {
          auto li = gp::get_local_info(c);
          val38(c->master->c7, si0, 0, PAT_COLORING);
          val38(c->master->move(li.last_dir)->c7, si1, 0, PAT_COLORING);
          }
        else 
        #endif
        {
          val38(c->move(0), si0, 0, PAT_COLORING);
          val38(c->move(2), si1, 0, PAT_COLORING);
          }          
        if((si0.id+1) % 3 == (si1.id) % 3)
          si.id = 8;
        else
          si.id = 0;
        }
      else 
        si.id = 8 * ((c->master->fiftyval & 1) ^ (sp & 1));
      #if CAP_GP
      if(GOLDBERG && pseudohept(c)) si.id = 4;
      #endif
      bool dock = false;
      for(int i=0; i<c->type; i+=2) {
        int fiv = createMov(c, i)->master->fiftyval;
        int fv = (fiv >> 1) & 3;
        if(fv == 0) {
          si.dir = (si.id == 8 && pat == PAT_COLORING ? 1 : 0) + i;
          if(fiv & 32) dock = true;
          }
        }
      if(symRotation) si.symmetries = 2;
      si.id += 8;
      si.id %= 12;
      #if CAP_GP
      if(GOLDBERG && pat == PAT_COLORING) 
        for(int i=0; i<c->type; i++) {
          cell *c2 = createMov(c, i);
          int id2 = 4;
          if(!pseudohept(c2)) {
            int sp2 = c2->c.spin(0);
            if(GOLDBERG) {
              sp2 = gp::last_dir(c2);
              sp2 ^= int(ishex2(c2));
              }
            id2 = 8 * ((c2->master->fiftyval & 1) ^ (sp2 & 1));
            }
//          printf("%p %2d : %d %d\n", c, si.id, i, id2);
          if((id2+4) % 12 == si.id) si.dir = i;
          }
      #endif
      applyAlt(si, sub, pat);
      if(dock && (sub & SPF_DOCKS)) si.id += 16;
      }
    }
  
  void valEuclid6(cell *c, patterninfo &si, int sub) {
    bool symRotation = sub & SPF_ROT;
    si.id = ishept(c) ? 4 : ishex1(c) ? 8 : 0;
    if(sub & SPF_CHANGEROT) {
      si.dir = (zebra40(c)*4) % 6;
      }
    if(symRotation) si.id = 1;
    if(euc::in(2,6) && (sub & SPF_FULLSYM))
      si.symmetries = 1;
    }

  void valEuclid4(cell *c, patterninfo &si, int sub) {
    si.id = eupattern4(c);
    applySym0123(si.id, sub);
    if(sub & SPF_CHANGEROT) {
      int dirt[] = {0,1,3,2};
      si.dir = dirt[si.id];
      if(c->type == 8) si.dir *= 2;
      }
    if(sub & SPF_SYM03) {
      si.id *= 4;
      applyAlt(si, sub, PAT_COLORING);
      }
    else
      si.symmetries = (sub & SPF_EXTRASYM) ? c->type/4 : c->type;
    }

  void val_all(cell *c, patterninfo &si, int sub, int pat) {
    if(IRREGULAR || arcm::in() || binarytiling || WDIM == 3) si.symmetries = 1;
    else if(a46) val46(c, si, sub, pat);
    else if(a38) val38(c, si, sub, pat);
    else if(sphere && S3 == 3) valSibling(c, si, sub, pat);
    else if(euc::in(2,4)) valEuclid4(c, si, sub);
    else if(euc::in(2,6)) valEuclid6(c, si, sub);
    else if(a4) val457(c, si, sub);
    else si.symmetries = ctof(c) ? 1 : 2;
    }

  void val_warped(cell *c, patterninfo& si) {
    int u = ishept(c)?1:0;
    if(S3 != 3 || S7 != 7 || NONSTDVAR) {
      si.id = u;
      si.dir = 1;
      return;
      }
    int qhex = 0;
    for(int v=0; v<c->type; v++) if(c->move(v) && !isWarped(c->move(v))) {
      u += 2;
      if(!ishept(c->move(v))) qhex++;
      }
    if(u == 8 && qhex == 2) u = 12;
    else if(u == 2 && qhex == 1) u = 8;
    else if(u == 6 && qhex == 2) u = 10;
    si.id = u;

    if(u == 6) {
        for(int i=1; i<c->type; i+=2) if(!isWarped(createMov(c,i)))
            si.dir = i;
        }
    
    else if(u == 2 || u == 3 || u == 8) {
        for(int i=0; i<c->type; i++) if(!isWarped(createMov(c,i)))
            si.dir = i;
        }
    
    else if(u == 4 || u == 10) {
        for(int i=0; i<c->type; i+=2) if(!isWarped(createMov(c,i)))
            si.dir = i;
        if(u == 4)
          si.reflect = !isWarped(createMov(c, (si.dir+1)%S6));
        }
    
    else if(u == 6) {
        for(int i=1; i<c->type; i+=2) if(!isWarped(createMov(c,i))) 
            si.dir = i;
        }
    
    else if(u == 5) {
        for(int i=0; i<c->type; i++) if(!isWarped(createMov(c,(i+S7/2)%S7)) && !isWarped(createMov(c,(i+(S7+1)/2)%S7))) 
            si.dir = i;
        }
    
    else if(u == 9) {
        for(int i=0; i<c->type; i++) if(!isWarped(createMov(c,(i+2)%S7)) && !isWarped(createMov(c,(i+S7-2)%S7))) 
            si.dir = i;
        }
    
    else if(u == 11) {
        for(int i=0; i<c->type; i++) if(isWarped(createMov(c,(i)%S7)) && isWarped(createMov(c,(i+1)%S7))) 
            si.dir = i;
        }
    
    else if(u == 12) {
        for(int i=0; i<c->type; i+=2) if(isWarped(createMov(c,i))) {
            si.dir = i;
            si.reflect = !isWarped(createMov(c, (i+1)%S6));
            }
        }

    else if(u == 7) {
        for(int i=0; i<c->type; i++) if(!isWarped(createMov(c,(i+1)%S7)) && !isWarped(createMov(c,(i+S7-1)%S7))) 
            si.dir = i;
        }
    }

  void val_nopattern(cell *c, patterninfo& si, int sub) {
    // use val_all for nicer rotation
    val_all(c, si, 0, 0);
    
    // get id:
    if((GOLDBERG? (S3==3) : !weirdhyperbolic) && isWarped(c)) 
      val_warped(c, si);
    else {
      si.id = pseudohept(c) ? 1 : 0;
      if(euc::in()) {
        si.dir = ishex1(c) ? 0 : 3;
        if(ctof(c)) si.symmetries = 3;
        if(subpattern_flags & SPF_EXTRASYM) 
          si.symmetries /= 3;
        if(subpattern_flags & SPF_FULLSYM) 
          si.symmetries = 1;
        }      
      if(sphere && BITRUNCATED && !(S7 == 3))
        si.symmetries = ctof(c) ? 1 : 2;
      if(sphere && (sub & SPF_EXTRASYM)) {
        si.symmetries = ctof(c) ? 1 : 2;
        }
      if(a38)
        si.symmetries = (ctof(c) && BITRUNCATED) ? 1 : 2;
      if(a457) {
        si.symmetries = ctof(c) ? 1 : 2;
        if(!ctof(c)) si.dir = 0;
        }
      if(a46) {
        si.symmetries = ctof(c) ? 1 : 2;
        }
      }
    
    if(GOLDBERG && has_nice_dual() && !ishept(c) && ishex1(c)) si.dir = gmod(si.dir+3, S6);
    }
  
  EX ePattern whichPattern = PAT_NONE;

  EX int subpattern_flags;
 
  void val_threecolors(cell *c, patterninfo& si, int sub) {
    int pcol = pattern_threecolor(c);
    si.id = pcol * 4;
    pcol = (pcol+1) % 3;
    si.dir = -1;
    for(int i=0; i<c->type; i++) 
      if(pattern_threecolor(createMov(c, i)) == pcol) {
        if(si.dir == -1) si.dir = i;
        else {
          si.symmetries = i - si.dir;
          break;
          }
        }
    if(euc::in(2,6) && (sub & SPF_CHANGEROT)) {
      if(GOLDBERG) {
        auto li = gp::get_local_info(c);
        if(li.first_dir >= 0)
          si.dir = gmod(zebra40(c)*4 - li.total_dir - li.last_dir, 6);
        else
          si.dir = gmod(zebra40(c)*4, 6);
        }
      else if(c == c->master->c7)
        si.dir = (zebra40(c)*4) % 6;
      else 
        si.dir = (zebra40(c)*4 + 9 - c->c.spin(0)) % 6;
      }
    if(sub & SPF_ROT) si.id = 1;
    if(euc::in(2,6) && !(sub & SPF_EXTRASYM)) {
      si.symmetries = 6;
      }
    if(euc::in(2,6) && (sub & SPF_FULLSYM))
      si.symmetries = 1;
    applyAlt(si, sub, PAT_COLORING);
    }
  
  EX patterninfo getpatterninfo(cell *c, ePattern pat, int sub) {
    if(!(sub & SPF_NO_SUBCODES)) {
      auto si = getpatterninfo(c, pat, sub | SPF_NO_SUBCODES);
      if(1) ;
      #if CAP_IRR
      else if(IRREGULAR)
        si.id += irr::cellindex[c] << 8;
      #endif
      #if CAP_ARCM
      else if(arcm::in())
        si.id += (arcm::id_of(c->master) << 8) + (arcm::parent_index_of(c->master) << 16);
      #endif
      #if CAP_GP
      else if(GOLDBERG) {
        if(c == c->master->c7) si.id += (c->c.fix(si.dir) << 8);
        else si.id += (get_code(gp::get_local_info(c)) << 16) | (c->c.fix(si.dir) << 8);
        }
      #endif
      return si;
      }
    bool symRotation = sub & SPF_ROT;
    // bool sym0 = sub & (SPF_SYM01 | SPF_SYM02 | SPF_SYM03);
    
    patterninfo si;
    si.dir = 0; si.reflect = false; si.id = ctof(c);
    si.symmetries = c->type;
    
    if(binarytiling) {
      if(pat == PAT_SINGLETYPE) si.id = 0;
      else si.id = c->type & 1;
      si.dir = 2;
      return si;
      }
    
    if(pat == PAT_SINGLETYPE) {
      si.id = 0; si.symmetries = 1;
      if(sub & SPF_TWOCOL) si.id = c->type & 1;
      if(sub & SPF_EXTRASYM) si.reflect = true;
      return si;
      }
    
    #if CAP_ARCM
    if(arcm::in() && pat == 0) {
      if(sub & SPF_FOOTBALL) {
        val_threecolors(c, si, sub);
        return si;
        }
      int id = arcm::id_of(c->master);
      auto& ca = arcm::current;
      si.id = ca.tilegroup[id];
      si.symmetries = ca.periods[si.id];
      si.dir = ca.groupoffset[id];
      if((sub & SPF_EXTRASYM) && ca.have_symmetry && ca.tilegroup[id^1] < ca.tilegroup[id])
        si.id = ca.tilegroup[id^1],
        si.reflect = true;
      return si;
      }
    #endif

    if(pat == PAT_ZEBRA && stdhyperbolic) {

      si.id = zebra40(c); // 4 to 43
      int t4 = si.id>>2, tcdir = 0;
      
      if(PURE) tcdir = si.id^1;
      
      else if(t4 == 10) tcdir = si.id-20;
      else if(t4 >= 4 && t4 < 7) tcdir = 40 + (si.id&3);
      else if(t4 >= 1 && t4 < 4) tcdir = si.id+12;
      else if(t4 >= 7 && t4 < 10) tcdir = si.id-24;
      
      for(int i=0; i<c->type; i++) if(c->move(i) && zebra40(c->move(i)) == tcdir)
        si.dir = i;
      
      applySym0123(si.id, sub);
      
      if(symRotation) {
        if(si.id >= 8 && si.id < 12) si.id -= 4;
        if(si.id >= 12 && si.id < 16) si.id -= 8;
        if(si.id >= 20 && si.id < 24) si.id -= 4;
        if(si.id >= 24 && si.id < 28) si.id -= 8;
        if(si.id >= 32 && si.id < 36) si.id -= 4;
        if(si.id >= 36 && si.id < 40) si.id -= 8;
        }
      
      if(si.id >= 40 && si.id < 44 && symRotation)
        si.symmetries = 2;

      if(si.id >= 40 && si.id < 44 && (sub & (SPF_SYM02 | SPF_SYM03)))
        si.symmetries = 2;
      }
    
    else if(pat == PAT_EMERALD && (stdhyperbolic || a38)) {
      si.id = emeraldval(c); // 44 to 99
      if(!euclid) {
        int tcdir = 0, tbest = (si.id&3);
        for(int i=0; i<c->type; i++) {
          cell *c2 = c->move(i);
          if(c2) {
            int t2 = emeraldval(c2);
            if((si.id&3) == (t2&3) && t2 > tbest)
              tbest = t2, tcdir = i;
            }
          }
        si.dir = tcdir;
        }
      applySym0123(si.id, sub);
      if(si.id >= 44 && si.id < 48)
        si.symmetries = 2;
      }
    
    else if(pat == PAT_PALACE && stdhyperbolic) {
      si.id = fiftyval200(c);

      si.reflect = polara50(c);
    
      int look_for = -1;
      int shft = 0;
      if(inr(si.id, 0, 4)) {
        look_for = si.id + (PURE ? 4 : 60);
        if(symRotation) si.symmetries = 1;
        }
      else if(inr(si.id, 4, 32)) look_for = si.id + (PURE ? 28 : 168);
      else if(inr(si.id, 32, 60)) look_for = si.id + (PURE ? -28 : 112);
      else if(inr(si.id, 60, 88)) look_for = si.id - 56, shft = si.reflect ? 1 : 5;
      else if(inr(si.id, 88, 116)) look_for = si.id - 84, shft = 3;
      else if(inr(si.id, 116, 144)) look_for = si.id + 56;
      else if(inr(si.id, 144, 172)) look_for = si.id + 28;
      else if(inr(si.id, 172, 200)) look_for = si.id - 28;

      si.dir = -1;
      for(int i=0; i<c->type; i++) {
          cell *c2 = createMov(c, i);
          if(fiftyval200(c2) == look_for)
            si.dir = (i + shft) % c->type;
          }
      
      if(si.dir == -1) {
        si.dir = 0;
        if(c->cpdist <= 1) printf("Not found for ID = %d (lf=%d)\n", si.id, look_for);
        c->item = itBuggy;
        }
      applySym0123(si.id, sub);

      if(symRotation && si.id >= 4)
        si.id -= ((si.id/4-1) % 7) * 4; 
      }
    
    else if(pat == PAT_PALACE && euclid) {
      si.id = fiftyval049(c);
      si.symmetries = 6;
      }

    else if(pat == PAT_PALACE) {
      val_nopattern(c, si, sub);
      si.id = c->master->fiftyval;
      }
    
    else if(pat == PAT_DOWN) {
      si.id = towerval(c, coastvalEdge);
      si.dir = downdir(c, coastvalEdge);
      }
    
    #if CAP_FIELD
    else if(pat == PAT_FIELD) {
      if(euclid)
        // use the torus ID
        si.id = fieldpattern::fieldval_uniq(c);
      else if(PURE && !arcm::in())
        // use the actual field codes
        si.id = fieldpattern::fieldval(c).first;
      else          
        // use the small numbers from windmap
        si.id = windmap::getId(c); 
      // todo dir
      }
    #endif
    
    else if(sphere && pat == PAT_SIBLING) {
      val_all(c, si, sub, pat);
      }

    else if(a457 && pat == PAT_ZEBRA) {
      val_all(c, si, sub, pat);
      }
    
    else if(GOLDBERG) {
      bool football = (pat == PAT_COLORING && (sub & SPF_FOOTBALL)) || pat == 0;
      if(football) val_nopattern(c, si, sub);
      else val_threecolors(c, si, sub);
      }

    else if(pat == PAT_COLORING && (S7 == 4 || euclid || (a38 && gp_threecolor() == 1) || arcm::in())) {
      val_threecolors(c, si, sub);
      }

    else if(pat == PAT_COLORING && (a46 || a38)) {
      val_all(c, si, sub, pat);
      }
    
    else if(pat == PAT_CHESS) {
      val_nopattern(c, si, sub);
      si.id = chessvalue(c);
      }

    else 
      val_nopattern(c, si, sub);
    
    return si;
    }

  #if HDR
  inline patterninfo getpatterninfo0(cell *c) {
    return getpatterninfo(c, whichPattern, subpattern_flags);
    }
  #endif
  
  EX }

EX bool geosupport_chessboard() {
  return 
#if CAP_ARCM
    (arcm::in() && PURE) ? arcm::current.support_chessboard() : 
    (arcm::in() && DUAL) ? arcm::current.support_threecolor_bitruncated() :
#endif
    (binarytiling || penrose) ? 0 :
    (S3 >= OINF) ? true :
    (VALENCE % 2 == 0);
  }

EX int geosupport_threecolor() {
  if(IRREGULAR) return 0;
  if(penrose || binarytiling) return 0;
  #if CAP_ARCM
  if(arcm::in() && PURE) return arcm::current.support_threecolor();
  if(arcm::in() && BITRUNCATED) return arcm::current.support_threecolor_bitruncated();
  if(arcm::in() && DUAL) return 0; // it sometimes does support threecolor, but it can be obtained in other ways then
  #endif
  if(BITRUNCATED && S3 == 3) {
    if(S7 % 2) return 1;
    return 2;
    }
  if(S3 >= OINF) return 0;
  if((S7 % 2 == 0) && (S3 == 3))
    return 2;
  if(a46 && PURE)
    return 1;
  return 0;
  }

EX int geosupport_football() {
  // always works in bitrunc geometries
  if(BITRUNCATED) return 2;
  if(binarytiling || penrose) return 0;

#if CAP_ARCM  
  if(arcm::in() && DUAL) return false;
  // it sometimes does support football, but it can be obtained in other ways then

  if(arcm::in() /* PURE */) return arcm::current.support_football();
#endif

#if CAP_IRR
  if(IRREGULAR) return irr::bitruncations_performed ? 2 : 1;
#endif
  
  // always works in patterns supporting three-color
  int tc = max(geosupport_threecolor(), gp_threecolor());
  if(tc) return tc;
  
  if(S3 == 3 && S7 == 7) return 1;
  // nice chessboard pattern, but not the actual Graveyard
  if(S3 == 4 && !(S7&1)) return 1;
  if(S3 == 4 && GOLDBERG) return 1;
  return 0;
  }

EX int pattern_threecolor(cell *c) {
  #if CAP_ARCM
  if(arcm::in()) {
    if(PURE)
      return arcm::threecolor(c);
    else /* if(BITRUNCATED) */
      return c->master->rval1;
    }
  #endif
  if(arb::in()) return 0;
  if(IRREGULAR || binarytiling) return !pseudohept(c);
  #if CAP_GP
  if(S3 == 3 && !(S7&1) && gp_threecolor() == 1 && c->master->c7 != c) {
    auto li = gp::get_local_info(c);
    int rel = (li.relative.first - li.relative.second + MODFIXER) % 3;
    int par = (gp::param.first - gp::param.second + MODFIXER) % 3;
    if(rel == 0)
      return pattern_threecolor(c->master->c7);
    else if(rel == par)
      return pattern_threecolor(c->master->cmove(li.last_dir)->c7);
    else
      return pattern_threecolor(c->master->cmodmove(li.last_dir+1)->c7);
    }
  #endif
  if(a38) {
    // if(GOLDBERG && gp_threecolor() == 2 && gp::pseudohept_val(c) == 0) return 0;
    patterns::patterninfo si;
    patterns::val38(c, si, !BITRUNCATED ? 0 : patterns::SPF_ROT, patterns::PAT_COLORING);
    return si.id >> 2;
    }
  if(euc::in(2,6) && gp_threecolor() == 2) {
    auto li = gp::get_local_info(c);
    int rel = gmod(li.relative.first - li.relative.second, 3);
    if(rel && (li.last_dir&1)) rel = 3 - rel;
    return rel;
    }
  #if CAP_GP
  if(a4 && GOLDBERG) {
    patterns::patterninfo si;
    auto li = gp::get_local_info(c);
    if(S7 & 1) return (li.relative.first&1) + (li.relative.second&1)*2;
    patterns::val46(c->master->c7, si, 0, patterns::PAT_COLORING);
    int i = si.id;
    if(i&2) i ^= 1;
    patterns::val46(createStep(c->master, li.last_dir)->c7, si, 0, patterns::PAT_COLORING);
    if(si.id&2) si.id ^= 1;
    int i2 = i ^ si.id;
    int i3 = 3 - i2;
    if(gp::param.first % 2 == 0 && gp::param.second % 2 == 0) i = 0;
    if(li.relative.first & 1) i ^= i3;
    if(li.relative.second & 1) i ^= i2;
    return i;
    }
  #endif
  if(a46 && BITRUNCATED) {
    patterns::patterninfo si;
    patterns::val46(c, si, 0, patterns::PAT_COLORING);
    int i = si.id;
    return i >> 2;
    }
  if(euclid) {
    if(a4 && PURE) return eupattern4(c);
    if(euc::in(2,6) && !BITRUNCATED) return eupattern(c) % 3;
    return c == c->master->c7 ? 0 : (c->c.spin(0)&1) ? 1 : 2;
    }
  if(S3 >= OINF) return c->master->distance % 3;
  if(S7 == 4 && S3 == 3) {
    int codesN[6] = {0,1,2,1,2,0};
    #if CAP_GP
    if(gp_threecolor() == 2) {
      auto li = gp::get_local_info(c);
      int sp = (MODFIXER + li.relative.first + 2 * li.relative.second) % 3;
      if(sp != 0) {
        if(li.last_dir & 1) 
          sp = 3 - sp;
        if(among(c->master->fiftyval, 1, 3, 5))
          sp = 3 - sp;
        }
      return sp;
      }
    #endif
    if(PURE)
      return codesN[c->master->fiftyval];
    if(ctof(c))
      return 0;
    else for(int i=0; i<3; i++) {
      cell *c2 = c->move(i);
      if(c2->master->fiftyval == 0)
        return 1 + (c->c.spin(i)&1);
      if(c2->master->fiftyval == 5)
        return 2 - (c->c.spin(i)&1);
      }
    }
  if(stdhyperbolic && PURE) {
    int z = zebra40(c);
    if(z == 5 || z == 8 || z == 15) return 0;
    if(c->land == laSnakeNest) {
      if(z == 10 || z == 12 || z == 7) return 2;
      if(z == 6 || z == 9) return 3;
      if(z == 14 || z == 11) return 4;
      }
    return 1;
    }
  if(a46 && !BITRUNCATED) {
    patterns::patterninfo si;
    patterns::val46(c, si, 0, patterns::PAT_COLORING);
    return si.id;
    }
  if(S7 == 5 && PURE && S3 == 3) {
    const int codes[12] = {1, 2, 0, 3, 2, 0, 0, 1, 3, 1, 2, 3};
    return codes[c->master->fiftyval];
    }
  if(S7 == 3 && PURE)
    return c->master->fiftyval;
  #if CAP_GP
  if(gp_threecolor() && (S7&1))
    return gp::pseudohept_val(c) > 0;
  #endif
  return !ishept(c);
  }
  
// returns ishept in the normal tiling;
// in the 'pure heptagonal' tiling, returns true for a set of cells
// which roughly corresponds to the heptagons in the normal tiling
EX bool pseudohept(cell *c) {
  #if CAP_IRR
  if(IRREGULAR) return irr::pseudohept(c);
  #endif
  if(hybri) { auto d = hybrid::get_where(c); return ((!prod) || (d.second & 1)) && PIU(pseudohept(d.first)); }
  #if CAP_BT
  if(nil) return c->master->zebraval & c->master->emeraldval & c->master->fieldval & 1;
  if(sol) return (c->master->emeraldval % 3 == 2) && (c->master->zebraval % 3 == 2) && (c->master->distance % 2);
  if(nih) return c->master->zebraval % 3 == 2 && c->master->emeraldval % 2 == 1 && (c->master->distance % 2);
  if(penrose) return kite::getshape(c->master) == kite::pDart;
  if(binarytiling) return binary::pseudohept(c);
  #endif
  if(S3 >= OINF) return c->master->distance % 3 == 1;
  #if MAXMDIM == 4
  if(WDIM == 3) {
    if(geometry == gField435) return false;
    else if(euclid) return euc::pseudohept(c);
    else return reg3::pseudohept(c);
    }
  #endif
  #if CAP_ARCM
  if(arcm::in()) return arcm::pseudohept(c);
  #endif
  #if CAP_GP
  if(GOLDBERG && gp_threecolor() == 2)
    return gp::pseudohept_val(c) == 0;
  if(GOLDBERG && gp_threecolor() == 1 && (S7&1) && (S3 == 3))
    return gp::pseudohept_val(c) == 0;
  #endif
  return pattern_threecolor(c) == 0;
  }

// while Krakens movement is usually restricted to non-pseudohept cells,
// there is one special case when this does not work (because non-pseudohept cells have varying degrees)
EX bool kraken_pseudohept(cell *c) {
  if(0);
  #if CAP_GP
  else if(!euclid && S3 == 4 && GOLDBERG && (gp::param.first % 2 || gp::param.second % 2 || S7 % 2))
    return ishept(c);
  #endif
  #if CAP_IRR
  else if(IRREGULAR)
    return c->type != 6;
  #endif
  #if CAP_ARCM
  else if(arcm::in() && PURE)
    return c->type != isize(arcm::current.triangles[0]);
  else if(arcm::in() && BITRUNCATED)
    return pseudohept(c);
  else if(arcm::in() && DUAL)
    return false;
  #endif
  else if(!euclid && S3 == 3 && !(S7&1) && gp_threecolor() == 1)
    return ishept(c);
  else
    return pseudohept(c);
  }

EX bool warptype(cell *c) {
  if(geosupport_chessboard()) 
    return chessvalue(c);
  else if(NONSTDVAR)
    return pseudohept(c);
  else
    return pattern_threecolor(c) == 0;
  }

EX map<char, colortable> colortables = {
  {'A', {
    0xF04040, 0x40F040, 0x4040F0,
    0xD0D000, 0xD000D0, 0x00D0D0,
    0xC0C0C0, 0x404040, 0x808080,
    0xF08040, 0xF04080, 0x40F080,
    0x4080F0, 0x8040F0, 0x80F040,
    0xFFD500 }},
  {'B', {
    // trying to get colors as in Wikipedia [ https://en.wikipedia.org/wiki/Euclidean_tilings_by_convex_regular_polygons#k-uniform_tilings ]
    0, 0, 0xFFFFFF, 0xFFFF00, 
    0xFF0000, 0xC000C0 /* unknown5 */, 0x00FF00, 0x00C0C0 /* unknown7 */, 0xFF8000, 
    0xFFFF80, 0xC040C0, 0xFFD500, 0x000080,
    0x404040, 0x606060, 0x808080
    }},
  {'a', {0x800000, 0x503000, 0x206000, 0x007010, 0x004040, 0x001070, 0x200060, 0x500030}},
  {'e', {0x404040, 0x1800000, 0x1008000, 0x000080 }},
  {'b', {0x404040, 0x1800000, 0x1008000, 0x000080 }},
  {'z', {0x1C0C0C0, 0x1E0E0E0, 0x404040, 0x606060 }},
  {'x', {0xC0C0C0, 0x1800000, 0x1008000, 0x000080 }},
  {'t', {0x804040, 0x1408040, 0x404080, 0x1808040 }},
  {'c', {0x202020, 0x1C0C0C0}},
  {'F', {0x1C0C0C0, 0x202020}},
  {'w', {0x303030, 0x1C0C0C0}},
  {'v', {0xC00000, 0xC08000, 0xC0C000, 0x00C000, 0xC0C0, 0x00C0, 0xC000C0}},
  {'j', {0x100FFFF, 0x100FF00, 0x1FFFF00, 0x1FF8000, 0x1FF0000, 0x1FF00FF}},  
  {'!', {0x1202020, 0x1000080, 0x1008000, 0x1008080, 0x1800000, 0x1800080, 0x1808000, 0x1C0C0C0,
         0x1808080, 0x10000FF, 0x100FF00, 0x100FFFF, 0x1FF0000, 0x1FF00FF, 0x1FFFF00, 0x1FFFFFF}},
  };

color_t random_landscape(cell *c, int mul, int div, int step, color_t base) {
  int col[4];
  for(int j=0; j<4; j++) {
    col[j] = getCdata(c, j);
    col[j] *= mul;
    col[j] %= 240;
    if(col[j] > 120) col[j] = 240 - col[j];
    if(col[j] < -120) col[j] = -240 - col[j];
    }
  col[0] /= div;
  col[1] /= div;
  col[2] /= div;
  if(ISWEB) for(int a=0; a<3; a++) col[a] = (col[a] + step/2) / step * step;
  color_t res = base + col[0] + (col[1] << 8) + (col[2] << 16);
  if(WDIM == 3 && (getBits(c) & 1)) res |= 0x1000000;
  return res;
  }

EX namespace patterns {
  EX int canvasback = linf[laCanvas].color >> 2;
  EX int subcanvas;
  EX bool displaycodes;
  EX char whichShape = 0;
  EX char whichCanvas = 0;
  EX bool innerwalls;
  
  int sevenval(cell *c) {
    if(!euclid) return 0;
    auto p = euc2_coordinates(c);
    return gmod(p.first - p.second * 2, 7);
    }

  EX string color_formula = "to01(rgb(x,y,z))";
  
  cld compute_map_function(cell *c, int p, const string& formula) {
    exp_parser ep;
    ep.extra_params["p"] = p;

    hyperpoint h = calc_relative_matrix(c, currentmap->gamestart(), C0) * C0;
    ep.extra_params["x"] = h[0];
    ep.extra_params["y"] = h[1];
    ep.extra_params["z"] = h[2];
    ep.extra_params["w"] = h[3];
    ep.extra_params["z40"] = zebra40(c);
    ep.extra_params["z3"] = zebra3(c);
    ep.extra_params["ev"] = emeraldval(c);
    ep.extra_params["fv50"] = fiftyval(c);
    ep.extra_params["pa"] = polara50(c);
    ep.extra_params["pb"] = polarb50(c);
    ep.extra_params["pd"] = cdist50(c);
    ep.extra_params["fu"] = fieldpattern::fieldval_uniq(c);
    ep.extra_params["threecolor"] = pattern_threecolor(c);
    ep.extra_params["chess"] = chessvalue(c);
    ep.extra_params["ph"] = pseudohept(c);
    ep.extra_params["kph"] = kraken_pseudohept(c);
    if(true) {
      ep.extra_params["md"] = c->master->distance;
      ep.extra_params["me"] = c->master->emeraldval;
      ep.extra_params["mf"] = c->master->fieldval;
      ep.extra_params["mz"] = c->master->zebraval;
      }

    if(sphere) {
      ep.extra_params["h0"] = getHemisphere(c, 0);
      ep.extra_params["h1"] = getHemisphere(c, 1);
      ep.extra_params["h2"] = getHemisphere(c, 2);
      }
    if(euclid) {
      auto co = euc2_coordinates(c);
      int x = co.first, y = co.second;
      ep.extra_params["ex"] = x;
      ep.extra_params["ey"] = y;
      if(S7 == 6) ep.extra_params["ez"] = -x-y;
      }
    if(cryst) {
      crystal::ldcoord co = crystal::get_ldcoord(c);
      for(int i=0; i<crystal::MAXDIM; i++)
        ep.extra_params["x"+its(i)] = co[i];
      }
    if(asonov::in()) {
      auto co = asonov::get_coord(c->master);
      ep.extra_params["ax"] = szgmod(co[0], asonov::period_xy);
      ep.extra_params["ay"] = szgmod(co[1], asonov::period_xy);
      ep.extra_params["az"] = szgmod(co[2], asonov::period_z);      
      }
    if(nil) {
      auto co = nilv::get_coord(c->master);
      ep.extra_params["nx"] = szgmod(co[0], nilv::nilperiod[0]);
      ep.extra_params["ny"] = szgmod(co[1], nilv::nilperiod[1]);
      ep.extra_params["nz"] = szgmod(co[2], nilv::nilperiod[2]);      
      }
    if(hybri)
      ep.extra_params["level"] = hybrid::get_where(c).second;

    if(geometry_supports_cdata()) {
      ep.extra_params["d0"] = getCdata(c, 0);
      ep.extra_params["d1"] = getCdata(c, 1);
      ep.extra_params["d2"] = getCdata(c, 2);
      ep.extra_params["d3"] = getCdata(c, 3);
      }
        
    ep.s = formula;
    return ep.parse();
    }
  
  EX hookset<int(cell*)> *hooks_generate_canvas;

  EX int generateCanvas(cell *c) {
    
    int i = callhandlers(-1, hooks_generate_canvas, c);
    if(i != -1) return i;
    switch(whichCanvas) {
      #if CAP_CRYSTAL
      case 'K': case '#': case '=': case 'O': case '/':
        if(nil)
          return nilv::colorize(c, whichCanvas);
        else
          return crystal::colorize(c, whichCanvas);
      #endif
      case 'A':
        #if CAP_ARCM
        if(arcm::in()) return colortables['A'][arcm::current.tilegroup[arcm::id_of(c->master)]];
        #endif
        if(arb::in()) return colortables['A'][c->master->zebraval];
      case 'B':
        return colortables['B'][c->type & 15];
      #if CAP_FIELD
      case 'C': {
        if(!hyperbolic) return canvasback;
        using namespace fieldpattern;
        int z = currfp.getdist(fieldval(c), make_pair(0,false));
        if(z < currfp.circrad) return 0x00C000;
        int z2 = currfp.getdist(fieldval(c), make_pair(currfp.otherpole,false));
        if(z2 < currfp.disthep[currfp.otherpole] - currfp.circrad)
          return 0x3000;
        return 0x6000;
        }
      case 'D': {
        if(!hyperbolic) return canvasback;
        using namespace fieldpattern;
        int z = currfp.getdist(fieldval(c), make_pair(0,false));
        return 255 * (currfp.maxdist+1-z) / currfp.maxdist;
        }
      case 'N': {
        if(!hyperbolic) return canvasback;
        using namespace fieldpattern;
        int z = currfp.getdist(fieldval(c), make_pair(0,false));
        int z2 = currfp.getdist(fieldval(c), make_pair(currfp.otherpole,false));
        if(z < z2) return 0x00C000;
        if(z > z2) return 0xC00000;
        return 0xCCCC00;
        }
      #endif
      case 'M': {
        int d = celldist(c);
        color_t res = gradient(0, canvasback, 0, min(1.8/(1+d), 1.), 1);
        if(d > 3) res |= 0x1000000;
        return res;
        }
      case 'P': {
        int d = celldistance(c, currentmap->gamestart()->move(0));
        color_t res = gradient(0, canvasback, 0, min(1.8/(1+d), 1.), 1);
        if(d > 3) res |= 0x1000000;
        return res;
        }
      #if CAP_FIELD
      case 'S': 
        if(!hyperbolic) return canvasback;
        return 0x3F1F0F * fieldpattern::subval(c).second + 0x000080;
      #endif
      case 'g':
        return canvasback;
      case 'r': {
        color_t r = hrand(0x1FFFFFF + 1);
        if(c == cwt.at) r &= 0xFFFFFF;
        return r;
        }
      case '^': {
        int x = c->master->fieldval & 4095;
        int y = (c->master->fieldval >> 12) & 4095;
        ignore(x);
        if(c->master->distance % 3) return 0;
        if(c->c.spin(binary::updir()) != 1) return 0;
        // if(c->master->distance % 2 == 0) return 0;
        if(hrand(100) == 0) return 0;
        return 0x1000000 | (0xFFFFFF & (0x671349 + y * 0x512369));
        if(hrand(100) >= 1) return 0;
        return 0x1000000 | hrand(0x1000000);
//        if(c->master->distance == 1) return 0x1FF0000;
//        if(c->master->distance == -1) return 0x100FF00;
//        return 0;        
        }
      case '!': {
        if(c == currentmap->gamestart()) return 0;
        for(int a=0; a<S7; a++) if(c == currentmap->gamestart()->move(a))
          {
          if(among(a,4,5,10,11)) return 0;
          return colortables['!'][a];
          }
        return hrand(0x1000000) | 0x1000000;
        }
        
      case 'e':
        return colortables['e'][emeraldval(c)];
      case 'a': {
        color_t col = colortables['a'][land50(c)];
        if(polara50(c)) col += 0x181818;
        return col;
        }
      case 'b':
        return colortables['b'][polara50(c) + 2 * polarb50(c)];
      case 'z':
        return colortables['z'][zebra40(c)];
      case 't': {
        int fv = zebra40(c);
        if(fv/4 == 4 || fv/4 == 6 || fv/4 == 5 || fv/4 == 10) fv ^= 2;
        return colortables['t'][fv];
        }
      case 'x':
        return colortables['x'][zebra3(c)];
      case 'w':
        return colortables['w'][randpattern(c, subcanvas) ? 1 : 0];
      case 'l': 
        return random_landscape(c, 3, 1, 17, 0x808080);
      case 'd':
        return random_landscape(c, 6, 8, 2, 0x101010);
      case 'h':
        return random_landscape(c, 6, 4, 4, 0x202020);
      case 'c':
        return colortables['c'][chessvalue(c)];
      case 'F':
        return colortables['F'][pseudohept(c)];
      case 'T':
        return nestcolors[pattern_threecolor(c)];
      case 'v':
        return colortables['v'][sevenval(c)];
      case 'j': {
        int d = c->master->distance;
        if(d % 2 == 0 || d < -5 || d > 5) return 0;
        return colortables['j'][(d+5)/2];
        }
      case 'f': {
        color_t res;
        for(int i=0; i<4; i++) {
          ld v = real(compute_map_function(c, 1+i, color_formula));
          if(i == 3) part(res, i) = (v > 0);
          else if(v < 0) part(res, i) = 0;
          else if(v > 1) part(res, i) = 255;
          else part(res, i) = int(v * 255 + .5);
          }
        return res;
        }
      case 'k': {
        /* just keep the old color */
        return c->landparam;
        }
      }
    return canvasback;
    }

  void showPrePatternP(bool instant) {
    cmode = sm::SIDE | sm::MAYDARK;
    gamescreen(0);

    dialog::init("predesigned patterns");
    dialog::addItem(WDIM == 3 ? XLAT("empty") : XLAT("single color"), 'g');
    dialog::addItem(XLAT("random colors"), 'r');

    if(WDIM == 2) dialog::addItem(XLAT("distance from origin"), 'M');
    dialog::addSelItem(XLAT("rainbow by distance"), "binary/Solv", 'j');
    
    if(geometry_supports_cdata()) {
      dialog::addItem(XLAT("rainbow landscape"), 'l');
      dialog::addItem(XLAT("dark rainbow landscape"), 'd');
      }

    dialog::addItem(XLAT("football"), 'F');
    if(geosupport_chessboard())
      dialog::addItem(XLAT("chessboard"), 'c');

    dialog::addItem(XLAT("nice coloring"), 'T');

    if(euc::in(2,6))
      dialog::addItem(XLAT("seven-coloring"), 'v');

    if(stdhyperbolic) {      
      dialog::addSelItem(XLAT("emerald pattern"), "emerald", 'e');
      dialog::addSelItem(XLAT("four elements"), "palace", 'b');
      dialog::addSelItem(XLAT("eight domains"), "palace", 'a');
      dialog::addSelItem(XLAT("zebra pattern"), "zebra", 'z');
      dialog::addSelItem(XLAT("four triangles"), "zebra", 't');
      dialog::addSelItem(XLAT("three stripes"), "zebra", 'x');
      }
    
    if(a4)
      dialog::addSelItem(XLAT("zebra pattern"), "coloring", 'z');

    dialog::addSelItem(XLAT("random black-and-white"), "current", 'w');

    #if CAP_FIELD
    if(!sphere) {
      dialog::addSelItem(XLAT("field pattern C"), "field", 'C');
      dialog::addSelItem(XLAT("field pattern D"), "field", 'D');
      dialog::addSelItem(XLAT("field pattern N"), "field", 'N');
      dialog::addSelItem(XLAT("field pattern S"), "field", 'S');
      }
    #endif
    
    if(arcm::in())
      dialog::addSelItem(XLAT("Archimedean"), "Archimedean", 'A');

    if(cryst)
      dialog::addSelItem(XLAT("Crystal coordinates"), "Crystal", 'K');
    
    if(cryst || geometry == gSpace344) {
      dialog::addSelItem(XLAT("Cage"), "Crystal", '#');
      dialog::addSelItem(XLAT("Hyperplanes"), "Crystal", '=');
      dialog::addSelItem(XLAT("Honeycomb"), "Crystal", 'O');
      dialog::addSelItem(XLAT("Diagonal"), "Crystal", '/');
      }
    if(nil) {
      dialog::addSelItem(XLAT("Penrose staircase"), "Nil", '/');
      }

    dialog::addSelItem(XLAT("sides"), "sides", 'B');

    if(!ISMOBILE)
      dialog::addSelItem(XLAT("formula"), "formula", 'f');

    dialog::addBreak(100);

    dialog::addBoolItem_action(XLAT("display the inner walls"), innerwalls, '5');
    dialog::addBoolItem(XLAT("display only hexagons"), (whichShape == '6'), '6');
    dialog::addBoolItem(XLAT("display only heptagons"), (whichShape == '7'), '7');
    dialog::addBoolItem(XLAT("display the triheptagonal grid"), (whichShape == '8'), '8');
    dialog::addBoolItem(XLAT("display full floors"), (whichShape == '9'), '9');
    dialog::addBoolItem(XLATN(winf[waInvisibleFloor].name), canvas_invisible, 'i');

    if(cheater || autocheat) dialog::addItem(XLAT("line patterns"), 'L');
    else dialog::addInfo("enable the cheat mode to use line patterns");
    
    dialog::addBack();
    dialog::display();
    
    keyhandler = [instant] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      if(uni == 'g') {
        static unsigned c = (canvasback << 8) | 0xFF;
        static unsigned canvasbacks[] = {
          6, 0xFFFFFFFF, 0x101010FF, 0x404040FF, 0x808080FF, 0x800000FF, unsigned(linf[laCanvas].color >> 2) << 8
          }; 
        dialog::openColorDialog(c, canvasbacks);
        dialog::reaction = [instant] () {
          if(instant) {
            stop_game();
            whichCanvas = 'g';
            canvasback = c >> 8;
            firstland = specialland = laCanvas;
            randomPatternsMode = false;
            start_game();
            }
          else {
            whichCanvas = 'g';
            canvasback = c >> 8;
            }
          };
        }
      else if(uni == 'i') {
        if(instant) {
          stop_game();
          canvas_invisible = !canvas_invisible;
          firstland = specialland = laCanvas; 
          randomPatternsMode = false;
          start_game();
          }
        else canvas_invisible = !canvas_invisible;
        }
      
      else if(uni == '6' || uni == '7' || uni == '8' || uni == '9') {
        if(whichShape == uni) whichShape = 0;
        else whichShape = uni;
        }
      else if(uni == 'L' && (cheater || autocheat))
        pushScreen(linepatterns::showMenu);

      else if(uni == 'f') {
        string s = XLAT(
          "This lets you specify the color pattern as a function of the cell. "
          "Available parameters:\n\n"
          "x, y, z (hyperboloid/sphere/plane coordinates in non-crystal geometries)\n"
          "ex, ey, ez (in Euclidean geometries)\n"
          "x0, x1, x2... (crystal geometry only)\n"
          "0 is black, 1 is white, rgb(1,0,0) is red, ifp(p-2,1,0) is blue (p=1 for red, 2 for green, 3 for blue).");
        
        if(MDIM == 4) s += XLAT(
          "w (fourth coordinate)\n"
          "wallif(condition, color)\n"
          );
        
        s += "see compute_map_function in pattern2.cpp for more\n";

        s += "\n\n" + parser_help();

        dialog::edit_string(color_formula, "formula", s);
        dialog::reaction_final = [instant] () { 
          if(instant) stop_game();
          whichCanvas = 'f';
          if(instant) {
            firstland = specialland = laCanvas; 
            randomPatternsMode = false;
            start_game();
            }
          };
        }
      
      else if((uni >= 'a' && uni <= 'z') || (uni >= 'A' && uni <= 'Z') || among(uni, '#', '=', '/')) {
        if(instant)
          stop_game();
        whichCanvas = uni;
        subcanvas = rand();
        if(instant) {
          firstland = specialland = laCanvas; 
          randomPatternsMode = false;
          start_game();
          }
        if(uni == 'x' || uni == 'z' || uni == 't')
          whichPattern = PAT_ZEBRA, subpattern_flags = SPF_SYM0123 | SPF_ROT;
        if(uni == 'e')
          whichPattern = PAT_EMERALD, subpattern_flags = SPF_SYM0123 | SPF_ROT;
        if(uni == 'b')
          whichPattern = PAT_PALACE, subpattern_flags = SPF_SYM0123 | SPF_ROT;
        if(uni == 'z' && a46)
          whichPattern = PAT_COLORING, subpattern_flags = SPF_CHANGEROT | SPF_SYM0123;
        }
      else if(doexiton(sym, uni)) popScreen();
      };    
    }
  
  EX void showPrePattern() { showPrePatternP(true); }
  EX void showPrePatternNoninstant() { showPrePatternP(false); }


#if CAP_TEXTURE
#define REMAP_TEXTURE texture::config.remap()
#else
#define REMAP_TEXTURE (void)0
#endif
  
  EX void showPattern() {
    cmode = sm::SIDE | sm::MAYDARK;
    {
    dynamicval<bool> dc(displaycodes, whichPattern);
    gamescreen(0);
    }
    dialog::init();

    dialog::addBoolItem(XLAT("cell types"), (whichPattern == PAT_TYPES), PAT_TYPES);
    
    if(stdhyperbolic || a4)
      dialog::addBoolItem(XLAT("Zebra Pattern"), (whichPattern == PAT_ZEBRA), PAT_ZEBRA);
    
    if(stdhyperbolic)
      dialog::addBoolItem(XLAT("Emerald Pattern"), (whichPattern == PAT_EMERALD), PAT_EMERALD);
    else if(a38)
      dialog::addBoolItem(XLAT("broken Emerald Pattern"), (whichPattern == PAT_EMERALD), PAT_EMERALD);
    
    if(stdhyperbolic || euclid)
      dialog::addBoolItem(XLAT("Palace Pattern"), (whichPattern == PAT_PALACE), PAT_PALACE);
    
    if(geosupport_chessboard())
      dialog::addBoolItem(XLAT("chessboard"), (whichPattern == PAT_CHESS), PAT_CHESS);

    if(geosupport_threecolor() == 2)
      dialog::addBoolItem(XLAT("coloring"), (whichPattern == PAT_COLORING), PAT_COLORING);
    
    if(sphere_narcm)
      dialog::addBoolItem(XLAT("siblings"), (whichPattern == PAT_SIBLING), PAT_SIBLING);
    
    if(euclid)
      dialog::addBoolItem(XLAT("torus"), (whichPattern == PAT_FIELD), PAT_FIELD);
    else if(sphere)
      dialog::addBoolItem(XLAT("single cells"), (whichPattern == PAT_FIELD), PAT_FIELD);
    else
      dialog::addBoolItem(XLAT("field pattern"), (whichPattern == PAT_FIELD), PAT_FIELD);
    
    dialog::addBoolItem(XLAT("single type"), (whichPattern == PAT_SINGLETYPE), PAT_SINGLETYPE);

    dialog::addBreak(50);

    if(
      (whichPattern == PAT_EMERALD && (stdhyperbolic || a38)) ||
      (whichPattern == PAT_PALACE && stdhyperbolic) ||
      (whichPattern == PAT_ZEBRA && stdhyperbolic) ||
      (whichPattern == PAT_SIBLING && sphere) ||
      (whichPattern == PAT_ZEBRA && a457)) {
      dialog::addBoolItem(XLAT("rotational symmetry"), subpattern_flags & SPF_ROT, '0');
      }
    
    if((euclid && whichPattern == PAT_COLORING) ||
      (a38 && whichPattern == PAT_COLORING) ||
      (a4 && !BITRUNCATED && whichPattern == PAT_COLORING && !a46))
      dialog::addBoolItem(XLAT("edit all three colors"), subpattern_flags & SPF_ROT, '0');

    if(euclid && whichPattern == PAT_COLORING)
      dialog::addBoolItem(XLAT("rotate the color groups"), subpattern_flags & SPF_CHANGEROT, '4');

    if(a46 && whichPattern == PAT_COLORING)
      dialog::addBoolItem(XLAT("rotate the color groups"), subpattern_flags & SPF_CHANGEROT, '4');

    if(a46 && whichPattern == PAT_COLORING && BITRUNCATED)
      dialog::addBoolItem(XLAT("edit both bitrunc colors"), subpattern_flags & SPF_TWOCOL, '5');

    if(
      (whichPattern == PAT_EMERALD && (stdhyperbolic || a38)) ||
      (whichPattern == PAT_PALACE && stdhyperbolic) ||
      (whichPattern == PAT_ZEBRA && stdhyperbolic) ||
      (whichPattern == PAT_COLORING && a46) ||
      (whichPattern == PAT_ZEBRA && a457)
      ) {
      dialog::addBoolItem(XLAT("symmetry 0-1"), subpattern_flags & SPF_SYM01, '1');
      dialog::addBoolItem(XLAT("symmetry 0-2"), subpattern_flags & SPF_SYM02, '2');
      dialog::addBoolItem(XLAT("symmetry 0-3"), subpattern_flags & SPF_SYM03, '3');
      }
    if(euclid && among(whichPattern, PAT_COLORING, PAT_TYPES) && !arcm::in())
      dialog::addBoolItem(XLAT("extra symmetries"), subpattern_flags & SPF_EXTRASYM, '=');
    
    #if CAP_ARCM
    if(arcm::in() && arcm::current.have_symmetry && whichPattern == PAT_TYPES)
      dialog::addBoolItem(XLAT("extra symmetries"), subpattern_flags & SPF_EXTRASYM, '=');
    #endif

    if(whichPattern == PAT_SINGLETYPE) {
      dialog::addBoolItem(XLAT("odd/even"), subpattern_flags & SPF_TWOCOL, '5');
      dialog::addBoolItem(XLAT("extra symmetries"), subpattern_flags & SPF_EXTRASYM, '=');
      }

    if(euclid && among(whichPattern, PAT_COLORING, 0))
      dialog::addBoolItem(XLAT("full symmetry"), subpattern_flags & SPF_FULLSYM, '!');

    if(a38 && PURE && whichPattern == PAT_TYPES) {
      dialog::addBoolItem(XLAT("extra symmetries"), subpattern_flags & SPF_EXTRASYM, '=');
      }

    if(a46 && PURE && whichPattern == PAT_COLORING) {
      dialog::addBoolItem(XLAT("extra symmetries"), subpattern_flags & SPF_EXTRASYM, '=');
      }

    if((whichPattern == PAT_COLORING) || (whichPattern == PAT_TYPES && arcm::in())) {
      dialog::addBoolItem(XLAT("alternate coloring"), subpattern_flags & SPF_ALTERNATE, '\'');
      dialog::addBoolItem(XLAT("football"), subpattern_flags & SPF_FOOTBALL, '*');
      }
    
    if(a38 && whichPattern == PAT_COLORING)
      dialog::addBoolItem(XLAT("Docks pattern"), subpattern_flags & SPF_DOCKS, '@');

    if(whichPattern && (IRREGULAR || GOLDBERG || arcm::in()))
      dialog::addBoolItem(XLAT("remove complete classification"), subpattern_flags & SPF_NO_SUBCODES, '#');
    
    dialog::addBreak(50);
    
    dialog::addBoolItem(XLAT("display pattern codes (full)"), displaycodes, 'd');

    if(!needConfirmation()) dialog::addItem(XLAT("predesigned patterns"), 'r');
    else dialog::addInfo("start a new game to use predesigned patterns");

    dialog::display();
    
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      
      if(among(uni, PAT_EMERALD, PAT_PALACE, PAT_ZEBRA, PAT_DOWN, PAT_FIELD, PAT_COLORING, PAT_SIBLING, PAT_CHESS, PAT_SINGLETYPE, PAT_TYPES)) {
        if(whichPattern == uni) whichPattern = PAT_NONE;
        else whichPattern = ePattern(uni);
        #if CAP_EDIT
        mapeditor::modelcell.clear();
        #endif
        REMAP_TEXTURE;
        }
      
      else if(uni >= '0' && uni <= '5') {
        subpattern_flags ^= (1 << (uni - '0'));
        REMAP_TEXTURE;
        }

      else if(uni == '=') {
        subpattern_flags ^= SPF_EXTRASYM;
        REMAP_TEXTURE;
        }

      else if(uni == '\'') {
        subpattern_flags ^= SPF_ALTERNATE;
        // subpattern_flags &= ~SPF_FOOTBALL;
        REMAP_TEXTURE;
        }

      else if(uni == '*') {
        subpattern_flags ^= SPF_FOOTBALL;
        // subpattern_flags &= ~SPF_ALTERNATE;
        REMAP_TEXTURE;
        }

      else if(uni == '!') {
        subpattern_flags ^= SPF_FULLSYM;
        REMAP_TEXTURE;
        }

      else if(uni == '@') {
        subpattern_flags ^= SPF_DOCKS;
        REMAP_TEXTURE;
        }

      else if(uni == '#') {
        subpattern_flags ^= SPF_NO_SUBCODES;
        REMAP_TEXTURE;
        }

      else if(uni == 'd') displaycodes = !displaycodes;
      
      else if(uni == 'r' && !needConfirmation()) pushScreen(showPrePattern);
      
      else if(doexiton(sym, uni)) popScreen();
      };
    }
  
  EX bool compatible(cpatterntype oldp, cpatterntype newp) {
    // larges are not incompatible between themselves
    if(newp == cpLarge || newp == cpZebra) 
      return false;
    // other cps are compatible with themselves
    if(newp == oldp) return true;
    // Single can be upgraded to everything
    if(oldp == cpSingle) return true;
    // Football can be upgraded to Three colors
    if(oldp == cpFootball) return newp == cpThree;
    // incompatible otherwise
    return false;
    }
  
  struct changeable_pattern_geometry {
    eGeometry geo;
    eVariation var;
    ePattern whichPattern;
    int subpattern_flags;
    };
  
  struct changeable_pattern {
    string name;
    vector<changeable_pattern_geometry> geometries;
    };
  
  vector<changeable_pattern> cpatterns = {
    {"football", {
      {gNormal, eVariation::bitruncated, PAT_TYPES, 0}, 
      {gSphere, eVariation::bitruncated, PAT_TYPES, 0}, 
      {gEuclid, eVariation::bitruncated, PAT_TYPES, SPF_EXTRASYM}, 
      {gOctagon, eVariation::bitruncated, PAT_TYPES, 0}, 
      {gOctagon, eVariation::pure, PAT_COLORING, SPF_FOOTBALL | SPF_EXTRASYM},
      {g45, eVariation::bitruncated, PAT_TYPES, 0}, 
      {g46, eVariation::bitruncated, PAT_TYPES, SPF_EXTRASYM}, 
      {g47, eVariation::bitruncated, PAT_TYPES, 0},
      {gSmallSphere, eVariation::bitruncated, PAT_TYPES, 0},
      {gSmallSphere, eVariation::pure, PAT_COLORING, SPF_FOOTBALL | SPF_EXTRASYM},
      {gTinySphere, eVariation::bitruncated, PAT_TYPES, SPF_EXTRASYM},
      {gEuclidSquare, eVariation::bitruncated, PAT_TYPES, SPF_EXTRASYM},
      }},
    {"three colors", {
      {gEuclid, eVariation::bitruncated, PAT_COLORING, SPF_SYM0123 | SPF_EXTRASYM},
      {gSmallSphere, eVariation::bitruncated, PAT_COLORING, 0},
      {gSmallSphere, eVariation::bitruncated, PAT_COLORING, SPF_ALTERNATE},
      {gSmallSphere, eVariation::pure, PAT_COLORING, 0},
      {gOctagon, eVariation::bitruncated, PAT_COLORING, SPF_ROT | SPF_EXTRASYM},
      {gOctagon, eVariation::bitruncated, PAT_COLORING, SPF_ROT | SPF_EXTRASYM | SPF_ALTERNATE},
      {gOctagon, eVariation::pure, PAT_COLORING, 0},
      {gEuclidSquare, eVariation::bitruncated, PAT_COLORING, SPF_SYM03 | SPF_EXTRASYM},
      {gEuclidSquare, eVariation::bitruncated, PAT_COLORING, SPF_SYM03 | SPF_EXTRASYM | SPF_ALTERNATE},
      {g46, eVariation::bitruncated, PAT_COLORING, SPF_SYM0123},
      {g46, eVariation::bitruncated, PAT_COLORING, SPF_SYM0123 | SPF_EXTRASYM | SPF_ALTERNATE}
      }},
    {"chessboard", {
      {gEuclidSquare, eVariation::pure, PAT_CHESS, SPF_EXTRASYM}, 
      {g45, eVariation::pure, PAT_CHESS, 0}, 
      {g46, eVariation::pure, PAT_CHESS, 0}, 
      {g47, eVariation::pure, PAT_CHESS, 0}
      }},
    {"single type", {
      {gNormal, eVariation::pure, PAT_SINGLETYPE, 0}, 
      {gSphere, eVariation::pure, PAT_SINGLETYPE, 0}, 
      {gEuclid, eVariation::bitruncated, PAT_SINGLETYPE, 0}, 
      {gOctagon, eVariation::pure, PAT_SINGLETYPE, 0}, 
      {g45, eVariation::pure, PAT_SINGLETYPE, 0}, 
      {g46, eVariation::pure, PAT_SINGLETYPE, 0}, 
      {g47, eVariation::pure, PAT_SINGLETYPE, 0},
      {gSmallSphere, eVariation::pure, PAT_SINGLETYPE, 0},
      {gTinySphere, eVariation::pure, PAT_SINGLETYPE, 0},
      {gEuclidSquare, eVariation::pure, PAT_SINGLETYPE, 0},
      }},
    {"single type+symmetry", {
      {gNormal, eVariation::pure, PAT_SINGLETYPE, SPF_EXTRASYM}, 
      {gSphere, eVariation::pure, PAT_SINGLETYPE, SPF_EXTRASYM}, 
      {gEuclid, eVariation::bitruncated, PAT_SINGLETYPE, SPF_EXTRASYM}, 
      {gOctagon, eVariation::pure, PAT_SINGLETYPE, SPF_EXTRASYM}, 
      {g45, eVariation::pure, PAT_SINGLETYPE, SPF_EXTRASYM}, 
      {g46, eVariation::pure, PAT_SINGLETYPE, SPF_EXTRASYM}, 
      {g47, eVariation::pure, PAT_SINGLETYPE, SPF_EXTRASYM},
      {gSmallSphere, eVariation::pure, PAT_SINGLETYPE, SPF_EXTRASYM},
      {gTinySphere, eVariation::pure, PAT_SINGLETYPE, SPF_EXTRASYM},
      {gEuclidSquare, eVariation::pure, PAT_SINGLETYPE, SPF_EXTRASYM},
      }},
    {"odd/even", {
      {gNormal, eVariation::bitruncated, PAT_SINGLETYPE, SPF_TWOCOL}, 
      {gSphere, eVariation::bitruncated, PAT_SINGLETYPE, SPF_TWOCOL}, 
      {g45, eVariation::pure, PAT_SINGLETYPE, SPF_TWOCOL}, 
      {g47, eVariation::pure, PAT_SINGLETYPE, SPF_TWOCOL}
      }},
    {"large picture", {
      {gNormal, eVariation::bitruncated, PAT_PALACE, SPF_SYM0123},
      {gNormal, eVariation::pure, PAT_PALACE, SPF_SYM0123},
      {gSphere, eVariation::bitruncated, PAT_FIELD, 0},
      {gSphere, eVariation::pure, PAT_FIELD, 0},
      {gElliptic, eVariation::bitruncated, PAT_FIELD, 0},
      {gElliptic, eVariation::pure, PAT_FIELD, 0},
      {gEuclid, eVariation::bitruncated, PAT_PALACE, 0}
      }},
    {"periodic patterns", {
      {gNormal, eVariation::bitruncated, PAT_ZEBRA, SPF_SYM0123 | SPF_ROT},
      {gNormal, eVariation::bitruncated, PAT_PALACE, SPF_SYM0123 | SPF_ROT},
      {gNormal, eVariation::bitruncated, PAT_EMERALD, SPF_SYM0123 | SPF_ROT},
      {g46, eVariation::pure, PAT_COLORING, SPF_SYM0123 | SPF_CHANGEROT},
      {g45, eVariation::pure, PAT_ZEBRA, SPF_SYM0123 | SPF_ROT},
      {g47, eVariation::pure, PAT_ZEBRA, SPF_SYM0123 | SPF_ROT},
      {gOctagon, eVariation::pure, PAT_COLORING, SPF_DOCKS},
      }}
    };
  
  EX cpatterntype cgroup, old_cgroup;
  
  void showChangeablePatterns() {
    cmode = sm::SIDE | sm::MAYDARK;
    {
    dynamicval<bool> dc(displaycodes, true);
    gamescreen(0);
    }
    dialog::init();
    for(int i=0; i<isize(cpatterns); i++) {
      dialog::addBoolItem(XLAT(cpatterns[i].name), cgroup == i, '0'+i);
#if CAP_TEXTURE
      if(texture::config.tstate == texture::tsActive && !compatible(texture::cgroup, (cpatterntype) i))
        dialog::lastItem().value = XLAT("BAD");
#endif
      }
    dialog::addBreak(100);
    if(cgroup != cpUnknown && cgroup < isize(cpatterns))
      for(int j=0; j<isize(cpatterns[cgroup].geometries); j++) {
        auto &g = cpatterns[cgroup].geometries[j];
        string s = XLAT(ginf[g.geo].tiling_name);
        s += bitruncnames[int(g.var)];
        if(g.subpattern_flags & SPF_ALTERNATE) s += " (alt)";
        if(g.subpattern_flags & SPF_DOCKS) s += " (Docks)";
        if(cgroup == cpZebra) {
          if(g.whichPattern == PAT_PALACE) s += " (Palace)";
          else if(g.whichPattern == PAT_EMERALD) s += " (Emerald)";
          else s += " (Zebra)";
          }
        dialog::addBoolItem(s, geometry == g.geo && variation == g.var && whichPattern == g.whichPattern && subpattern_flags == g.subpattern_flags, 'a'+j);
        }
    bool have_goldberg = (S3 == 3 && among(cgroup, cpFootball, cpThree) && !euclid);
    bool have_variations = (among(cgroup, cpSingle, cpSingleSym) && !euclid);
    if(!(S7&1) && BITRUNCATED) have_goldberg = false; // always start from pure
    if(have_goldberg) {
      dialog::addBoolItem(XLAT("Goldberg"), GOLDBERG, 'G');
      dialog::lastItem().value = gp::operation_name();
      }
    if(have_variations) {
      dialog::addBoolItem(XLAT("variations"), GOLDBERG, 'G');
      dialog::lastItem().value = gp::operation_name();
      }
    else dialog::addBreak(100);
    dialog::addItem(XLAT("more tuning"), 'r');
    dialog::addBack();
    dialog::display();
  
    keyhandler = [have_goldberg, have_variations] (int sym, int uni) {
      if(uni == 'r')
        pushScreen(showPattern);
      else if(uni >= '0' && uni < '0' + isize(cpatterns))
        cgroup = cpatterntype(uni - '0');
      else if(cgroup != cpUnknown && uni >= 'a' && uni < 'a' + isize(cpatterns[cgroup].geometries)) {
        auto &g = cpatterns[cgroup].geometries[uni - 'a'];
        if(g.geo != geometry) set_geometry(g.geo);
        if(g.var != variation) set_variation(g.var);
        whichPattern = g.whichPattern;
        subpattern_flags = g.subpattern_flags;
        bool not_restarted = game_active;
        start_game();
        if(not_restarted) REMAP_TEXTURE;
        }
      #if CAP_GP
      else if(uni == 'G' && (have_goldberg || have_variations))
        gp::configure();
      #endif
      else if(doexiton(sym, uni))
        popScreen();
      };
    }
  
  EX void computeCgroup() {
    cgroup = cpUnknown;
    if(whichPattern == PAT_SINGLETYPE) {
      cgroup = cpSingle;
      return;
      }
    if(arcm::in()) {
      if(whichPattern == PAT_COLORING && geosupport_threecolor()) {
        if(subpattern_flags & SPF_FOOTBALL) cgroup = cpFootball;
        else cgroup = cpThree;
        }
      else if(whichPattern == PAT_CHESS && geosupport_chessboard()) cgroup = cpChess;
      else if(whichPattern == PAT_TYPES && (subpattern_flags & SPF_FOOTBALL) && geosupport_football()) cgroup = cpFootball;
      return;
      }
    for(int i=0; i<isize(cpatterns); i++)
      for(int j=0; j<isize(cpatterns[i].geometries); j++) {
        auto &g = cpatterns[i].geometries[j];
        eVariation xvar = variation;
        if(GOLDBERG && gp_threecolor()) xvar = eVariation::pure;
        if(geometry == g.geo && xvar == g.var && whichPattern == g.whichPattern && subpattern_flags == g.subpattern_flags)
          cgroup = cpatterntype(i);
        }
    old_cgroup = cgroup;
    }

  EX void pushChangeablePatterns() {
    pushScreen(showChangeablePatterns);
    computeCgroup(); 
    }
  EX }

EX bool is_master(cell *c) { 
  if(euclid) return pseudohept(c);
  else return c->master->c7 == c;
  }

EX namespace linepatterns {

  #if HDR
  enum ePattern {
    patPalacelike,
    patPalace,
    patZebraTriangles,
    patZebraLines,
    patTriTree,
    patTriRings,
    patHepta,
    patRhomb,
    patTree,
    patAltTree,
    patVine,
    patPower,
    patNormal,
    patTrihepta,
    patBigTriangles,
    patBigRings,
    patHorocycles,
    patTriOther,
    patDual,
    patMeridians,
    patParallels,
    patCircles,
    patRadii
    };

  struct linepattern {
    int id;
    const char *lpname;
    color_t color;
    ld multiplier;
    };
  #endif

  color_t lessalpha(color_t col, int m) {
    part(col, 0) /= m;
    return col;
    }
  
  color_t lessalphaif(color_t col, bool b) {
    return b?lessalpha(col, 4):col;
    }
    
  color_t lessalphaif(color_t col, bool b1, bool b2) {
    if(b1) col = lessalpha(col, 2);
    if(b2) col = lessalpha(col, 2);
    return col;
    }
    
  EX vector<linepattern> patterns = {

    {patDual, "dual grid", 0xFFFFFF00, 1},

    {patHepta, "heptagonal grid", 0x0000C000, 1},
    {patRhomb, "rhombic tesselation", 0x0000C000, 1},
    {patTrihepta, "triheptagonal tesselation", 0x0000C000, 1},
    {patNormal, "normal tesselation", 0x0000C000, 1},
    {patBigTriangles, "big triangular grid", 0x00606000, 1},
    {patBigRings, "big triangles: rings", 0x0000C000, 1},
    
    {patTree, "underlying tree", 0x00d0d000, 1},
    {patAltTree, "circle/horocycle tree", 0xd000d000, 1},

    {patZebraTriangles, "zebra triangles", 0x40FF4000, 1},
    {patZebraLines, "zebra lines", 0xFF000000, 1},
    {patVine, "vineyard pattern", 0x8438A400, 1},
    {patPalacelike, "firewall lines", 0xFF400000, 1},
    {patPalace, "firewall lines: Palace", 0xFFD50000, 1},
    {patPower, "firewall lines: Power", 0xFFFF0000, 1},
    {patHorocycles, "horocycles", 0xd060d000, 1},

    {patTriRings, "triangle grid: rings", 0xFFFFFF00, 1},
    {patTriTree, "triangle grid: tree edges", 0xFFFFFF00, 1},
    {patTriOther, "triangle grid: other edges", 0xFFFFFF00, 1},

    {patCircles, "circles", 0xFFFFFF00, 1},
    {patRadii, "radii", 0xFFFFFF00, 1},
    {patMeridians, "meridians", 0xFFFFFF00, 1},
    {patParallels, "parallels", 0xFFFFFF00, 1},
    };

  EX void clearAll() {
    for(auto& lp: patterns) lp.color &= ~255;
    }

  bool any() {
    for(auto& lp: patterns) if(lp.color & 255) return true;
    return false;
    }

  EX void setColor(ePattern id, color_t col) {
    for(auto& lp: patterns)
      if(lp.id == id) lp.color = col;
    }
  
  EX void switchAlpha(ePattern id, color_t col) {
    for(auto& lp: patterns)
      if(lp.id == id) lp.color ^= col;
    }
  
  void gridlinef(const transmatrix& V1, const hyperpoint& h1, const transmatrix& V2, const hyperpoint& h2, color_t col, int par) {
    if(!elliptic)
      gridline(V1, h1, V2, h2, col, par);
    else {
      hyperpoint vh1 = V1 * h1;
      hyperpoint vh2 = V2 * h2;
      ld cros = vh1[0]*vh2[0] + vh1[1]*vh2[1] + vh1[2]*vh2[2];
      if(cros > 0)
        gridline(V1, h1, V2, h2, col, par),
        gridline(V1, -1*h1, V2, -1*h2, col, par);
      else
        gridline(V1, h1, V2, -1*h2, col, par),
        gridline(V1, -1*h1, V2, h2, col, par);
      }
    }

  void gridlinef(const transmatrix& V, const hyperpoint& h1, const hyperpoint& h2, color_t col, int par) { gridlinef(V, h1, V, h2, col, par); }
  
  void drawPattern(int id, color_t col, cell *c, const transmatrix& V) {

    switch(id) {
    
      case patZebraTriangles:
        if(euc::in(2,6)) {
          if(c != c->master->c7 || patterns::sevenval(c)) break;
          gridline(V, C0, tC0(euc::eumove(gp::loc(-1, +3))), col, 3 + vid.linequality);
          gridline(V, C0, tC0(euc::eumove(gp::loc(-3, +2))), col, 3 + vid.linequality);
          gridline(V, C0, tC0(euc::eumove(gp::loc(-2, -1))), col, 3 + vid.linequality);
          gridline(V, C0, tC0(euc::eumove(gp::loc(+1, -3))), col, 3 + vid.linequality);
          gridline(V, C0, tC0(euc::eumove(gp::loc(+3, -2))), col, 3 + vid.linequality);
          gridline(V, C0, tC0(euc::eumove(gp::loc(+2, +1))), col, 3 + vid.linequality);
          break;
          }
        if(zebra40(c) / 4 == 10) {
          bool all = true;
          transmatrix tri[3];
          for(int i=0; i<3; i++)
            tri[i] = V * currentmap->adj(c, i*2);
          
          if(all) for(int i=0; i<3; i++)
            gridline(tri[i], C0, tri[(i+1)%3], C0, col, 3 + vid.linequality);
          }
        break;
      
      case patZebraLines:
        if(!pseudohept(c)) for(int i=0; i<c->type; i+=2) {
          cell *c2 = createMov(c, i);
          int fv1 = zebra40(c);
          if(fv1/4 == 4 || fv1/4 == 6 || fv1/4 == 5 || fv1/4 == 10) fv1 ^= 2;
          int fv2 = zebra40(c2);
          if(fv2/4 == 4 || fv2/4 == 6 || fv2/4 == 5 || fv2/4 == 10) fv2 ^= 2;
          if((fv1&1) == (fv2&1)) continue;
          
          double x = cgi.hexhexdist / 2; // sphere?.3651:euclid?.2611:.2849;
          
          gridlinef(V, ddspin(c,i,-M_PI/S3) * xpush0(x), 
            ddspin(c,i,M_PI/S3) * xpush0(x), 
            col, 1 + vid.linequality);
          }
        break;
      
      case patNormal: {
        for(int t=0; t<c->type; t++)
          if(c->move(t) && c->move(t) < c)
          gridline(V, get_corner_position(c, t),
                    get_corner_position(c, (t+1)%c->type),
                    col, 1 + vid.linequality);
        break;
        }
      
      case patTrihepta:
        if(pseudohept(c)) for(int t=0; t<c->type; t++)
          gridline(V, get_warp_corner(c, t%c->type),
                    get_warp_corner(c, (t+1)%c->type),
                    col, 1 + vid.linequality);
        break;
      
      case patDual:
        forCellIdEx(c2, i, c) if(c2 > c) {
          gridlinef(V, C0, V * currentmap->adj(c, i), C0, col, 2 + vid.linequality);
          }
        break;

      case patTriRings:
        forCellIdEx(c2, i, c) {
          if(S3 == 4) c2 = (cellwalker(c, i) + wstep + 1).cpeek();
          if(c2 > c) if(curr_dist(c) == curr_dist(c2)) 
            gridlinef(V, C0, V * currentmap->adj(c, i), C0, col, 2 + vid.linequality);
          }
        break;

      case patTriTree: {
        cell *parent = ts::right_parent(c, curr_dist);
        if(gmatrix.count(parent)) 
          gridlinef(V, C0, V * currentmap->adj(c, neighborId(c, parent)), C0, col, 2 + vid.linequality);
        break;
        }
      
      case patTriOther: {
        cell *parent = ts::right_parent(c, curr_dist);
        forCellIdEx(c2, i, c) if(curr_dist(c2) < curr_dist(c) && c2 != parent)
          gridlinef(V, C0, V * currentmap->adj(c, i), C0, col, 2 + vid.linequality);
        break;
        }

      case patHepta:
        forCellIdEx(c2, i, c) if(c2 > c) if(pseudohept(c) == pseudohept(c2)) 
          gridlinef(V, C0, V * currentmap->adj(c, i), C0, col, 2 + vid.linequality);
        break;

      case patRhomb:
        forCellIdEx(c2, i, c) if(c2 > c) if(pseudohept(c) != pseudohept(c2)) 
          gridlinef(V, C0, V * currentmap->adj(c, i), C0, col, 2 + vid.linequality);
        break;
      
      case patPalace: {
        bool a = polarb50(c);
        if(pseudohept(c)) for(int i=0; i<7; i++) {
            cell *c1 = createMov(c, (i+3) % 7);
            cell *c2 = createMov(c, (i+4) % 7);
            if(polarb50(c1) != a && polarb50(c2) != a)
                gridlinef(V, ddspin(c,i,M_PI*5/7) * xpush0(cgi.tessf/2),
                          ddspin(c,i,M_PI*9/7) * xpush0(cgi.tessf/2),
                                    col, 1 + vid.linequality);
            }
        break;
        }
      
      case patPalacelike:
        if(pseudohept(c)) for(int i=0; i<7; i++) 
          gridlinef(V, ddspin(c,i,M_PI*5/7) * xpush0(cgi.tessf/2),
                       ddspin(c,i,M_PI*9/7) * xpush0(cgi.tessf/2),
                              col, 1 + vid.linequality);
        break;
      
      case patBigTriangles: {
       if(is_master(c) && !euclid) for(int i=0; i<S7; i++) 
          if(c->master->move(i) && c->master->move(i) < c->master) {
            gridlinef(V, C0, xspinpush0(-2*M_PI*i/S7 - master_to_c7_angle(), cgi.tessf), col, 2 + vid.linequality);
            }
        break;
        }
        
      case patBigRings: {
        if(is_master(c) && !euclid) for(int i=0; i<S7; i++) 
          if(c->master->move(i) && c->master->move(i) < c->master && c->master->move(i)->dm4 == c->master->dm4)
            gridlinef(V, C0, xspinpush0(-2*M_PI*i/S7 - master_to_c7_angle(), cgi.tessf), col, 2 + vid.linequality);
        break;
        }
        
      case patTree:
        if(is_master(c)) {
          int dir = binarytiling ? binary::updir() : 0;
          cell *c2 = c->master->move(dir)->c7;
          if(gmatrix.count(c2)) {
            if(S3 >= OINF)
              gridlinef(V, C0, Id, mid(tC0(V), tC0(V * currentmap->adj(c, dir))), col, 2 + vid.linequality);
            else 
              gridlinef(V, C0, V * currentmap->adj(c, dir), C0, col, 2 + vid.linequality);
            }
          }
        break;
      
      case patHorocycles:
        if(c->master->alt) {
          int d = celldistAlt(c);
          forCellIdEx(c2, i, c) if(c2 > c && c2->master->alt && celldistAlt(c2) == d)
            gridlinef(V, C0, V * currentmap->adj(c, i), C0, 
              darkena(backcolor ^ 0xFFFFFF, 0, col),
              2 + vid.linequality);
          }
        break;

      case patAltTree:
        if(is_master(c) && !euclid && c->master->alt) {
          for(int i=0; i<S7; i++)
            if(c->master->move(i) && c->master->move(i)->alt == c->master->alt->move(0)) {
              cell *c2 = c->master->move(i)->c7;
              if(gmatrix.count(c2)) {
                if(S3 >= OINF) {
                  gridlinef(V, C0, Id, mid(tC0(V), tC0(gmatrix[c2])), col, 2 + vid.linequality);
                  }
                else 
                  gridlinef(V, C0, V*currentmap->adj(c->master,i), C0, col, 2 + vid.linequality);
                }
              }
          }
        break;
      
      case patVine: {
        if(GOLDBERG) {
          if(c->master->c7 != c) if(gmatrix.count(c->move(0)))
            gridlinef(V, C0, V*currentmap->adj(c,0), C0, 
              darkena(backcolor ^ 0xFFFFFF, 0, col),
              2 + vid.linequality);
          }
        else if(IRREGULAR) {
          if(c->master->c7 != c) if(gmatrix.count(c->master->c7))
            gridlinef(V, C0, V*master_relative(c, true), C0, 
              darkena(backcolor ^ 0xFFFFFF, 0, col),
              2 + vid.linequality);
          }
        else {
          int p = emeraldval(c);
          double hdist = hdist0(cgi.heptmove[0] * cgi.heptmove[2] * C0);
          if(pseudohept(c) && (p/4 == 10 || p/4 == 8))
          for(int i=0; i<S7; i++) if(c->move(i) && emeraldval(c->move(i)) == p-4) {
            gridlinef(V, C0, tC0(cgi.heptmove[i]), col, 2 + vid.linequality);
            gridlinef(V, C0, xspinpush0(-i * ALPHA, -hdist/2), col, 2 + vid.linequality);
            }
          }
        break;
        }
      
      case patPower: {
        if(GOLDBERG) {
          forCellIdEx(c2, i, c) if(c2->master != c->master)
            gridlinef(V, C0, V*currentmap->adj(c, i), C0, 
              col,
              1 + vid.linequality);
          }
        else if(arcm::in()) {
          if(!pseudohept(c)) forCellIdEx(c2, i, c) if(c < c2 && !pseudohept(c2)) 
            gridlinef(V, C0, V*currentmap->adj(c, i), C0, 
              col,
              1 + vid.linequality);
          }
        else {
          int a = emeraldval(c);
          if(pseudohept(c) && a/4 == 8) for(int i=0; i<7; i++) {
              heptagon *h1 = c->master->modmove(i+1);
              heptagon *h2 = c->master->modmove(i-1);
              if(!h1 || !h2) continue;
              if(emeraldval(h1->c7)/4 == 8 && emeraldval(h2->c7)/4 == 8)
                  gridlinef(V, ddspin(c,i,M_PI*5/7) * xpush0(cgi.tessf/2),
                            ddspin(c,i,M_PI*9/7) * xpush0(cgi.tessf/2),
                                      col, 1 + vid.linequality);
              }
          }
        break;
        }
      }
    }  

  EX ld width = 1;

  EX void drawAll() {
  
    vid.linewidth *= width;

    if(any()) for(auto& p: current_display->all_drawn_copies) for(auto& V: p.second) {
      cell* c = p.first;
      
      for(auto& lp: patterns) {
        color_t col = lp.color;
        vid.linewidth *= lp.multiplier;
        if(!(col & 255)) continue;        
        drawPattern(lp.id, col, c, V);
        vid.linewidth /= lp.multiplier;
        }
      }
    
    transmatrix V = gmatrix[cwt.at];
    for(auto& lp: patterns) {
      color_t col = lp.color;  
      vid.linewidth *= lp.multiplier;
      if(!(col & 255)) continue;        
      if(lp.id == patCircles) 
        for(int i=15; i<=180; i+=15) {
          for(int j=0; j<360; j+=15) {
            for(int k=0; k<=15; k++) 
              curvepoint(xspinpush0((j+k) * degree, i * degree));
            queuecurve(col, 0, PPR::LINE).V=V;
            }
          }
      if(lp.id == patRadii)
        for(int i=0; i<360; i+=15) {
          for(int j=0; j<180; j+=15) {
            for(int k=0; k<=15; k++)
              curvepoint(xspinpush0(i * degree, (j+k) * degree));
            queuecurve(col, 0, PPR::LINE).V=V;
            }
          }
      if(lp.id == patMeridians) {
        for(int j=-180; j<=180; j+=15) {
          for(int i=-90; i<90; i+=15) {
            for(int k=0; k<=15; k++)
              curvepoint(V * xpush(j * degree) * ypush0((i+k) * degree));
            queuecurve(col, 0, PPR::LINE).V=V;
            }
          }
        }
      if(lp.id == patParallels) {
        for(int i=-90; i<=90; i += 15) {
          for(int j=-180; j<180; j+=15) {
            for(int k=0; k<=15; k++) 
              curvepoint(V * xpush((j+k) * degree) * ypush0(i * degree));
            queuecurve(col, 0, PPR::LINE).V=V;
            }
          }
        }
      vid.linewidth /= lp.multiplier;
      }

    vid.linewidth /= width;
    }
  
  int numpat = 0;
  
  bool indiv;
  
  EX void showMenu() {
    cmode = sm::SIDE | sm::MAYDARK;
    gamescreen(0);

    dialog::init(XLAT("line patterns"));

    int id = 0;    
    for(auto& lp: patterns) {
      string name = XLAT(lp.lpname);
      if(GOLDBERG && among(lp.id, patVine, patPower)) name = XLAT("Goldberg");
      if(!indiv) {
        dialog::addColorItem(name, lp.color, 'a'+(id++));
        dialog::add_action([&lp] () { 
          dialog::openColorDialog(lp.color, NULL);
          dialog::dialogflags |= sm::MAYDARK | sm::SIDE;
          });
        }
      else {
        dialog::addSelItem(name, fts(lp.multiplier), 'a'+(id++));
        dialog::add_action([&lp] () { dialog::editNumber(lp.multiplier, 0.001, 10, 0.1, 1, XLAT("line width"), ""), dialog::scaleLog(); });
        }
      }
  
    dialog::addBreak(50);
    dialog::addBack();

    dialog::addSelItem("line width", fts(width), 'W');
    dialog::add_action([] () { 
      dialog::editNumber(width, 0, 10, 1, 1, XLAT("line width"), "");
      });

    dialog::addBoolItem_action("edit widths individually", indiv, 'I');
    
    dialog::addBreak(50);
    dialog::addInfo("change the alpha parameter to show the lines");
  
    dialog::display();
    }
    
  };

int val46(cell *c) {
  patterns::patterninfo si;
  patterns::val46(c, si, 0, 0);
  return si.id;
  }

#if CAP_COMMANDLINE

int read_pattern_args() {
  using namespace arg;
  if(argis("-pattern")) {
    PHASEFROM(2);
    shift();
    const char *c = argcs();
    using namespace patterns;
    subpattern_flags = 0;
    whichPattern = PAT_NONE;
    while(*c) { 
      if(*c >= '0' && *c <= '9') subpattern_flags ^= 1 << (*c - '0'); 
      else if(*c == '@') subpattern_flags ^= 1 << 10; 
      else if(*c == '-') subpattern_flags ^= 1 << 11; 
      else if(*c == '~') subpattern_flags ^= 1 << 12; 
      else whichPattern = ePattern(*c);
      c++; 
      }
    }

  else if(argis("-wsh")) { start_game(); shift(); patterns::whichShape = args()[0]; }

  else if(argis("-pal")) {
    PHASEFROM(2); cheat();
    shift(); string ss = args();
    shift(); 
    for(auto& lp: linepatterns::patterns)
      if(appears(lp.lpname, ss))
        lp.color |= argi();
    }
  else if(argis("-palrgba")) {
    PHASEFROM(2); cheat();
    shift(); string ss = args();
    shift(); 
    for(auto& lp: linepatterns::patterns)
      if(appears(lp.lpname, ss))
        lp.color = arghex();
    }

  else if(argis("-palw")) {
    PHASEFROM(2); 
    shift(); string ss = args();
    for(auto& lp: linepatterns::patterns)
      if(appears(lp.lpname, ss)) {
        shift_arg_formula(lp.multiplier);
        return 0;
        }
    println(hlog, "linepattern not found in -palw: ", ss);
    shift();  
    }

  else if(argis("-palgw")) shift_arg_formula(linepatterns::width);

  else if(argis("-noplayer")) mapeditor::drawplayer = !mapeditor::drawplayer;
  else if(argis("-pcol")) {
    shift();
    colortable *ct = &(colortables[patterns::whichCanvas]);
    if(args()[0] > '9') {
      char c = args()[0];
      if(c == 't') ct = &nestcolors;
      else if(c == 'd') ct = &distcolors;
      else if(c == 'm') ct = &minecolors;
      else if(c == 'E') { shift(); int d = argi(); shift(); expcolors[d] = arghex(); return 0; }      
      else if(c == 'P') { 
        shift(); int d = argi(); shift();
        color_t h = arghex();
        if(d >= 0 && d < 7)
          ((color_t*)(&vid.cs.skincolor)) [d] = h;
        return 0;
        }
      else ct = &(colortables[patterns::whichCanvas]);
      shift();
      }
    int d = argi();
    shift(); (*ct)[d] = arghex();
    }
  else if(argis("-canvas")) {
    PHASEFROM(2);
    stop_game();
    firstland = specialland = laCanvas;
    shift();
    if(args() == "i") canvas_invisible = !canvas_invisible;
    else if(args().size() == 1) patterns::whichCanvas = args()[0];
    else patterns::canvasback = arghex();
    stop_game_and_switch_mode(rg::nothing);
    }
  else if(argis("-cformula")) {
    PHASEFROM(2);
    stop_game();
    firstland = specialland = laCanvas;
    patterns::whichCanvas = 'f';
    shift(); patterns::color_formula = args();
    }
  else if(argis("-innerwall")) {
    PHASEFROM(2);
    patterns::innerwalls = true;
    }
  else if(argis("-noinnerwall")) {
    PHASEFROM(2);
    patterns::innerwalls = false;
    }
  else if(argis("-d:line")) 
    launch_dialog(linepatterns::showMenu);

  else if(argis("-d:reg"))
    launch_dialog(patterns::showPrePattern);

  else if(argis("-d:pattern"))
    launch_dialog(patterns::showPattern);

  else return 1;
  return 0;
  }

auto ah_pattern = addHook(hooks_args, 0, read_pattern_args);
#endif

}
