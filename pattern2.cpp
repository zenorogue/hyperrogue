// HyperRogue patterns: compute codes for actual cells

// Copyright (C) 2011-2017 Zeno Rogue, see 'hyper.cpp' for details

int eupattern(cell *c) {
  if(torus) return (decodeId(c->master)*2) % 3;
  eucoord x, y;
  decodeMaster(c->master, x, y);
  short z = (short(y+2*x))%3;
  z %= 3;
  if(z<0) z += 3;
  return z;
  }

bool ishept(cell *c) {
  // EUCLIDEAN
  if(euclid) return eupattern(c) == 0;
  else return c->type != S6;
  }

bool ishex1(cell *c) {
  // EUCLIDEAN
  if(euclid) return eupattern(c) == 1;
  else return c->type != S6;
  }

int emeraldval(cell *c) {
  if(euclid) return eupattern(c);
  if(sphere) return 0;
  if(ctof(c))
    return c->master->emeraldval >> 3;
  else {
    return emerald_hexagon(
      emeraldval(createMov(c,0)),
      emeraldval(createMov(c,2)),
      emeraldval(createMov(c,4))
      );
    }
  }

// === FIFTYVALS ===

unsigned bitmajority(unsigned a, unsigned b, unsigned c) {
  return (a&b) | ((a^b)&c);
  }

int eufifty(cell *c) {
  eucoord x, y;
  if(torus) {
    if(c->land == laWildWest) return decodeId(c->master) % 37;
    else return decodeId(c->master) % 27;
    }
  decodeMaster(c->master, x, y);
  int ix = short(x) + 99999 + short(y);
  int iy = short(y) + 99999;
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
    return bitmajority(
      fiftyval(createMov(c,0)), 
      fiftyval(createMov(c,2)), 
      fiftyval(createMov(c,4))) + 512;
    }
  }

int cdist50(cell *c) {
  if(sphere || S7>7 || S6>6) return 0;
  if(euclid) {
    if(c->land == laWildWest) 
      return "0123333332112332223322233211233333322"[eufifty(c)] - '0';
    else return "012333321112322232222321123"[eufifty(c)] - '0';
    }
  if(c->type != 6) return cdist50(fiftyval(c));
  int a0 = cdist50(createMov(c,0));
  int a1 = cdist50(createMov(c,2));
  int a2 = cdist50(createMov(c,4));
  if(a0 == 0 || a1 == 0 || a2 == 0) return 1;
  return a0+a1+a2-5;
  }

int land50(cell *c) {
  if(c->type != 6) return land50(fiftyval(c));
  else if(sphere || euclid) return 0;
  else {
    if(cdist50(createMov(c,0)) < 3) return land50(createMov(c,0));
    if(cdist50(createMov(c,2)) < 3) return land50(createMov(c,2));
    if(cdist50(createMov(c,4)) < 3) return land50(createMov(c,4));
    return 0;
    }
  }

int polara50(cell *c) {
  if(c->type != 6) return polara50(fiftyval(c));
  else if(sphere || euclid || S7>7 || S6>6) return 0;
  else {
    if(cdist50(createMov(c,0)) < 3) return polara50(createMov(c,0));
    if(cdist50(createMov(c,2)) < 3) return polara50(createMov(c,2));
    if(cdist50(createMov(c,4)) < 3) return polara50(createMov(c,4));
    return 0;
    }
  }

int polarb50(cell *c) {
  if(euclid) return true;
  if(c->type != 6) return polarb50(fiftyval(c));
  else if(sphere || euclid || S7>7 || S6>6) return true;
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
  if(c->type != 6 || euclid) return fiftyval(c) / 32;
  else if(sphere) return 0;
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

int dir_truncated457(cell *c) {
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

int zebra40(cell *c) {
  if(euclid) return eupattern(c);
  else if(a46) {
    int v = val46(c);
    if(v<4) return v;
    else return 4+(v-4)/2;
    }
  else if(ctof(c)) return (c->master->zebraval/10);
  else if(a4) {
    int ws = dir_truncated457(c);
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
  else if(euclid) return eupattern(c);
  else if(S3 == 4 && S7 == 6) {
    return 8 + ((c->master->zebraval / 10 + c->spin(0))%2) * 2;
    }
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
  if(c->type != 6) return (c->master->zebraval/10)/4;
  else if(sphere || S7>7 || S6>6) return 0;
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

namespace fieldpattern {

pair<int, bool> fieldval(cell *c) {
  if(ctof(c)) return make_pair(c->master->fieldval, false);
  else return make_pair(btspin(c->master->fieldval, c->spin(0)), true);
  }

int fieldval_uniq(cell *c) {
  if(sphere) {
    if(ctof(c)) return c->master->fieldval;
    else return createMov(c, 0)->master->fieldval + 256 * createMov(c,2)->master->fieldval + (1<<16) * createMov(c,4)->master->fieldval;
    }
  else if(torus) {
    return decodeId(c->master);
    }
  else if(euclid) {
    eucoord x, y;
    decodeMaster(c->master, x, y);
    int i = (short int)(x) * torusconfig::dx + (short int)(y) * torusconfig::dy;
    i %= torusconfig::qty;
    if(i<0) i += torusconfig::qty;
    return i;
    }
  if(ctof(c)) return c->master->fieldval/S7;
  else {
    int z = 0;
    for(int u=0; u<S6; u+=2) 
      z = max(z, btspin(createMov(c, u)->master->fieldval, c->spin(u)));
    return -1-z;
    }
  }

int fieldval_uniq_rand(cell *c, int randval) {
  if(sphere || torus || euclid) 
    // we do not care in these cases
    return fieldval_uniq(c);
  if(ctof(c)) return currfp.gmul(c->master->fieldval, randval)/7;
  else {
    int z = 0;
    for(int u=0; u<6; u+=2) 
      z = max(z, btspin(currfp.gmul(createMov(c, u)->master->fieldval, randval), c->spin(u)));
    return -1-z;
    }
  }

int subpathid = currfp.matcode[currfp.strtomatrix("RRRPRRRRRPRRRP")];
int subpathorder = currfp.order(currfp.matrices[subpathid]);

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

}

int getHemisphere(cell *c, int which) {
  if(torus) return 0;
  if(ctof(c)) {
    int id = c->master->fiftyval;
    if(S7 == 5) {
      int hemitable[3][12] = {
        { 6, 3, 3, 3, 3, 3,-6,-3,-3,-3,-3,-3},
        { 6, 3, 6, 3, 0, 0,-6,-3,-6,-3, 0, 0},
        {-3, 0, 3, 0,-6,-6, 3, 0,-3, 0, 6, 6}
        };
      return hemitable[which][id];
      }
    else if(S7 == 4) {
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
  else {
    int score = 0;
    for(int i=0; i<6; i+=2) 
      score += getHemisphere(c->mov[i], which) * (c->mirror(i) ? -1 : 1);
    return score/3;
    }
  }

namespace patterns {

  void valSibling(cell *c, patterninfo& si, int sub) {
    if(ctof(c)) {
      int d = c->master->fieldval;
      si.id = (d < siblings[d]) ? 0 : 1;
      if(sub & SPF_ROT) si.id = 0;
      for(int i=0; i<S7; i++) {
        int di = c->master->move[i]->fieldval;
        if(di == siblings[d]) si.dir = i;
        }
      si.reflect = false;
      }
    else {
      int ids = 0, tids = 0, td = 0;
      for(int i=0; i<S3; i++) {
        int d = c->mov[i*2]->master->fieldval;
        ids |= (1<<d); tids += d;
        }
      for(int i=0; i<S3; i++) {
        int d = c->mov[i*2]->master->fieldval;
        if(ids & (1<<siblings[d])) td += d;
        }
      if(td) {
        si.id = 4;
        for(int i=0; i<S3; i++) {
          int d = c->mov[i*2]->master->fieldval;
          if(!(ids & (1<<siblings[d]))) si.dir = 2*i;
          }
        si.reflect = false;
        }
      else {
        si.id = 8;
        si.dir = 0; // whatever 
        patterninfo si2;
        valSibling(c->mov[0], si2, sub);
        int di = si2.dir - c->spin(0);
        di %= S7; 
        if(di<0) di += S7;
        si.reflect = di > S7/2;
        }
      }
    }
  
  int downdir(cell *c, cellfunction *cf = coastvalEdge) {
    cell *c2 = chosenDown(c, 1, 1, cf);
    if(!c2) return 0;
    return neighborId(c, c2);
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

  void val46(cell *c, patterninfo &si, int sub) {
    int bits = (sub & SPF_CHANGEROT) ? 1 : 2;
    if(ctof(c)) {
      si.id = c->master->emeraldval >> 1;
      applySym0123(si.id, sub);
      int b = c->master->emeraldval & bits;
      si.dir = (b&1) ^ (b & 2 ? 1 : 0);
      }
    else {
      if(sub & SPF_TWOCOL) si.id = 4;
      else
        si.id = ((c->master->emeraldval & 1) ^ ((c->master->emeraldval & 2)>>1) ^ (c->spin(0)&1)) ? 8 : 4;
      si.dir = ((c->mov[0]->master->emeraldval + c->spin(0)) & 1) ? 2 : 0;
      if(createMov(c, si.dir)->master->emeraldval & 4)
        si.dir += 4;
      }
    }
  
  // if(a46) return patterndir46(c, w == PAT_ZEBRA ? 3 : w == PAT_PALACE ? 2 : 1);

  void val457(cell *c, patterninfo &si, int sub) {
    si.id = zebra40(c);
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
      int d = dir_truncated457(c);
      if(d >= 0) si.dir = d;
      else si.dir = (zebra40(createMov(c, 0)) & 4) ? 2 : 0;
      }
    }

  void val38(cell *c, patterninfo &si, int sub) {
    bool symRotation = sub & SPF_ROT;
    bool sym01 = sub & SPF_TWOCOL;

    if(ctof(c)) {
      if(!symRotation) 
        si.id = (c->master->fiftyval >> 1) & 3;
      si.dir = c->master->fiftyval | (c->master->fiftyval & 8 ? 0 : 2);
      }
    else {
      if(sym01)
        si.id = 4;
      else
        si.id = 4 ^ (c->master->fiftyval & 1) ^ (c->spin(0) & 1);
      }
    }
  
  void valEuclid(cell *c, patterninfo &si, int sub) {
    bool symRotation = sub & SPF_ROT;
    si.id = ishept(c) ? 1 : ishex1(c) ? 2 : 0;
    if(sub & SPF_CHANGEROT)
      si.dir = (zebra40(c)*4) % 6;
    if(symRotation) si.id = 0;
    }

  void val_all(cell *c, patterninfo &si, int sub) {
    if(a46) val46(c, si, sub);
    else if(a38) val38(c, si, sub);
    else if(sphere) valSibling(c, si, sub);
    else if(euclid) valEuclid(c, si, sub);
    else if(a4) val457(c, si, sub);
    }

  void val_warped(cell *c, patterninfo& si) {
    // use val_all for nicer rotation
    val_all(c, si, 0);
    
    // get id:
    if(stdhyperbolic && isWarped(c)) {
      int u = ishept(c)?1:0;
      int qhex = 0;
      for(int v=0; v<c->type; v++) if(c->mov[v] && !isWarped(c->mov[v])) {
        u += 2;
        if(!ishept(c->mov[v])) qhex++;
        }
      if(u == 8 && qhex == 2) si.id = 12;
      else if(u == 2 && qhex == 1) si.id = 8;
      else if(u == 6 && qhex == 2) si.id = 10;
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
            si.reflect = !isWarped(createMov(c, (si.dir+1)%6));
          }
      
      else if(u == 6) {
          for(int i=1; i<c->type; i+=2) if(!isWarped(createMov(c,i))) 
              si.dir = i;
          }
      
      else if(u == 5) {
          for(int i=0; i<c->type; i++) if(!isWarped(createMov(c,(i+3)%7)) && !isWarped(createMov(c,(i+4)%7))) 
              si.dir = i;
          }
      
      else if(u == 9) {
          for(int i=0; i<c->type; i++) if(!isWarped(createMov(c,(i+2)%7)) && !isWarped(createMov(c,(i+5)%7))) 
              si.dir = i;
          }
      
      else if(u == 11) {
          for(int i=0; i<c->type; i++) if(isWarped(createMov(c,(i)%7)) && isWarped(createMov(c,(i+1)%7))) 
              si.dir = i;
          }
      
      else if(u == 12) {
          for(int i=0; i<c->type; i+=2) if(isWarped(createMov(c,i))) {
              si.dir = i;
              si.reflect = !isWarped(createMov(c, (i+1)%6));
              }
          }
  
      else if(u == 7) {
          for(int i=0; i<c->type; i++) if(!isWarped(createMov(c,(i+1)%7)) && !isWarped(createMov(c,(i+6)%7))) 
              si.dir = i;
          }
  
      }
    else {
      si.id = ishept(c) ? 1 : 0;
      if(euclid) si.dir = ishex1(c) ? 3 : 0;
      }
    }
  
  char whichPattern = 0;

  int subpattern_flags;
  
  patterninfo getpatterninfo(cell *c, char pat, int sub) {
    bool symRotation = sub & SPF_ROT;

    patterninfo si;
    si.dir = 0; si.reflect = false; si.id = ctof(c);
    
    if(pat == PAT_ZEBRA && stdhyperbolic) {

      si.id = zebra40(c); // 4 to 43
      int t4 = si.id>>2, tcdir = 0;
      
      if(nontruncated) tcdir = si.id^1;
      
      else if(t4 == 10) tcdir = si.id-20;
      else if(t4 >= 4 && t4 < 7) tcdir = 40 + (si.id&3);
      else if(t4 >= 1 && t4 < 4) tcdir = si.id+12;
      else if(t4 >= 7 && t4 < 10) tcdir = si.id-24;
      
      for(int i=0; i<c->type; i++) if(c->mov[i] && zebra40(c->mov[i]) == tcdir)
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
      }
    
    else if(pat == PAT_EMERALD && (stdhyperbolic || a38)) {
      si.id = emeraldval(c); // 44 to 99
      if(!euclid) {
        int tcdir = 0, tbest = (si.id&3);
        for(int i=0; i<c->type; i++) {
          cell *c2 = c->mov[i];
          if(c2) {
            int t2 = emeraldval(c2);
            if((si.id&3) == (t2&3) && t2 > tbest)
              tbest = t2, tcdir = i;
            }
          }
        si.dir = tcdir;
        }
      applySym0123(si.id, sub);
      }
    
    else if(pat == PAT_PALACE && stdhyperbolic) {
      int i = fiftyval049(c);
      i *= 4;
      if(polara50(c)) i|=1;
      if(polarb50(c)) i|=2;
      si.id = i;

      int tcdir = -1, tbest = -1;
      int pa = polara50(c);
      int pb = polarb50(c);
      si.reflect = pb;
      for(int i=0; i<c->type; i++) {
          cell *c2 = c->mov[i];
          if(c2 && polara50(c2) == pa && polarb50(c2) == pb) {
              int t2 = fiftyval049(c2);
              if(t2 > tbest) tbest = t2, tcdir = i;
          }
      }
      si.dir = tcdir;
      applySym0123(si.id, sub);

      if(symRotation && si.id >= 3)
        si.id -= ((si.id/4-1) % 7) * 4; 
      }
    
    else if(pat == PAT_PALACE && euclid) {
      si.id = fiftyval049(c);
      }

    else if(pat == PAT_DOWN) {
      si.id = towerval(c);
      si.dir = downdir(c);
      }
    
    else if(pat == PAT_FIELD) {
      if(euclid)
        // use the torus ID
        si.id = fieldpattern::fieldval_uniq(c);
      else if(nontruncated)
        // use the actual field codes
        si.id = fieldpattern::fieldval(c).first;
      else          
        // use the small numbers from windmap
        si.id = windmap::getId(c); 
      // todo dir
      }
    
    else if(sphere && pat == PAT_SIBLING) {
      val_all(c, si, sub);
      }

    else if(a457 && pat == PAT_ZEBRA) {
      val_all(c, si, sub);
      }

    else if(pat == PAT_COLORING && (a46 || euclid)) {
      val_all(c, si, sub);
      }

    else 
      val_warped(c, si);
    
    return si;
    }

  }

int geosupport_threecolor() {
  if(!nontruncated) {
    if(S7 % 2) return 1;
    return 2;
    }
  if((S7 % 2 == 0) && (S3 == 3))
    return 2;
  return 0;
  }

int geosupport_graveyard() {
  // always works in truncated geometries
  if(!nontruncated) return 2;
  
  // always works in patterns supporting three-color
  return geosupport_threecolor();
  }

int pattern_threecolor(cell *c) {
  if(a38) {
    patterns::patterninfo si;
    patterns::val38(c, si, 0);
    int i = si.id;
    if(nontruncated) return i;
    else return i < 4 ? 0 : (1+(i&1));
    }
  if(a46 && !nontruncated) {
    patterns::patterninfo si;
    patterns::val46(c, si, 0);
    int i = si.id;
    return i >> 2;
    }
  if(S7 == 4) {
    int codesN[6] = {0,1,2,1,2,0};
    if(nontruncated) 
      return codesN[c->master->fiftyval];
    if(ctof(c))
      return 0;
    else for(int i=0; i<3; i++) {
      cell *c2 = c->mov[i];
      if(c2->master->fiftyval == 0)
        return 1 + (c->spin(i)&1);
      if(c2->master->fiftyval == 5)
        return 2 - (c->spin(i)&1);
      }
    }
  if(stdhyperbolic && nontruncated) {
    int z = zebra40(c);
    if(z == 5 || z == 8 || z == 15) return 0;
    if(z == 10 || z == 12 || z == 7) return 2;
    if(z == 6 || z == 9) return 3;
    if(z == 14 || z == 11) return 4;
    return 1;
    }
  if(S7 == 5 && nontruncated) {
    const int codes[12] = {1, 2, 0, 3, 2, 0, 0, 1, 3, 1, 2, 3};
    return codes[c->master->fiftyval];
    }
  if(S7 == 3 && nontruncated)
    return c->master->fiftyval;
  if(euclid) return eupattern(c);
  return !ishept(c);
  }
  
// returns ishept in the normal tiling;
// in the 'pure heptagonal' tiling, returns true for a set of cells
// which roughly corresponds to the heptagons in the normal tiling
bool pseudohept(cell *c) {
  return pattern_threecolor(c) == 0;
  }

namespace patterns {
  int canvasback = linf[laCanvas].color >> 2;
  int subcanvas;
  int displaycodes;
  char whichShape = 0;
  char whichCanvas = 0;

  int generateCanvas(cell *c) {
    if(whichCanvas == 'C' && !torus) {
      using namespace fieldpattern;
      int z = currfp.getdist(fieldval(c), make_pair(0,false));
      if(z < currfp.circrad) return 0x00C000;
      int z2 = currfp.getdist(fieldval(c), make_pair(currfp.otherpole,false));
      if(z2 < currfp.disthep[currfp.otherpole] - currfp.circrad)
        return 0x3000;
      return 0x6000;
      }
    if(whichCanvas == 'D' && !torus) {
      using namespace fieldpattern;
      int z = currfp.getdist(fieldval(c), make_pair(0,false));
      return 255 * (currfp.maxdist+1-z) / currfp.maxdist;
      }
    if(whichCanvas == 'N' && !torus) {
      using namespace fieldpattern;
      int z = currfp.getdist(fieldval(c), make_pair(0,false));
      int z2 = currfp.getdist(fieldval(c), make_pair(currfp.otherpole,false));
      if(z < z2) return 0x00C000;
      if(z > z2) return 0xC00000;
      return 0xCCCC00;
      }
    if(whichCanvas == 'S' && !torus) {
      return 0x3F1F0F * fieldpattern::subval(c).second + 0x000080;
      }
    if(whichCanvas == 'g')
      return canvasback;
    if(whichCanvas == 'r')
      return hrand(0xFFFFFF + 1);
    if(whichCanvas == 'e') {
      static unsigned int fcol[4] = { 0x404040, 0x800000, 0x008000, 0x000080 };
      int fv = emeraldval(c);
      return fcol[fv&3];
      }
    if(whichCanvas == 'a') {
      static unsigned int fcol8[8] = { 
        0x800000,
        0x503000,
        0x206000,
        0x007010,
        0x004040,
        0x001070,
        0x200060,
        0x500030
        };
        
      if(c->wall == waNone) {
        int col = fcol8[land50(c)];
        if(polara50(c)) col += 0x181818;
        return col;
        }
      }
    if(whichCanvas == 'b') {
      static unsigned int fcol[4] = { 0x404040, 0x800000, 0x008000, 0x000080 };
      return fcol[polara50(c) + 2 * polarb50(c)];
      }
    if(whichCanvas == 'z') {
      static unsigned int fcol[4] = { 0xC0C0C0, 0xE0E0E0, 0x404040, 0x606060 };
      int fv = zebra40(c);
      return fcol[fv&3];
      }
    if(whichCanvas == 't') {
      static unsigned int fcol[4] = { 0x804040, 0x408040, 0x404080, 0x808040 };
      int fv = zebra40(c);
      if(fv/4 == 4 || fv/4 == 6 || fv/4 == 5 || fv/4 == 10) fv ^= 2;
      return fcol[fv&3];
      }
    if(whichCanvas == 'x') {
      static unsigned int fcol[4] = { 0xC0C0C0, 0x800000, 0x008000, 0x000080 };
      return fcol[zebra3(c)];
      }
    if(whichCanvas == 'w') {
      static unsigned int fcol[2] = { 0x303030, 0xC0C0C0 };
      return fcol[randpattern(c, subcanvas) ? 1 : 0];
      }
    if(whichCanvas == 'l') {
      int col[4];
      bool err = false;
      for(int j=0; j<4; j++) {
        col[j] = getCdata(c, j);
        col[j] *= 3;
        col[j] %= 240;
        if(col[j] > 120) col[j] = 240 - col[j];
        if(col[j] < -120) col[j] = -240 - col[j];
        }
      return (0x808080 + col[0] + (col[1] << 8) + (col[2] << 16)) >> (err?2:0);
      }
    if(whichCanvas == 'd') {
      int col[4];
      bool err = false;
      for(int j=0; j<4; j++) {
        col[j] = getCdata(c, j);
        col[j] *= 6;
        col[j] %= 240;
        if(col[j] > 120) col[j] = 240 - col[j];
        if(col[j] < -120) col[j] = -240 - col[j];
        }
      col[0] /= 8;
      col[1] /= 8;
      col[2] /= 8;
      return (0x101010 + col[0] + (col[1] << 8) + (col[2] << 16)) >> (err?2:0);
      }
    if(whichCanvas == 'h') {
      int col[4];
      bool err = false;
      for(int j=0; j<4; j++) {
        col[j] = getCdata(c, j);
        col[j] *= 6;
        col[j] %= 240;
        if(col[j] > 120) col[j] = 240 - col[j];
        if(col[j] < -120) col[j] = -240 - col[j];
        }
      col[0] /= 4;
      col[1] /= 4;
      col[2] /= 4;
      return (0x202020 + col[0] + (col[1] << 8) + (col[2] << 16)) >> (err?2:0);
      }
    if(whichCanvas == PAT_FIELD) {
      return pseudohept(c) ? 0x202020 : 0xC0C0C0;
      }
    if(whichCanvas == 'T') {
      int fv = pattern_threecolor(c);
      return nestcolors[fv&7];
      }
    return canvasback;
    }

  void showPrePattern() {
    dialog::init("predesigned patterns");
    dialog::addItem(XLAT("Gameboard"), 'g');
    dialog::addItem(XLAT("random colors"), 'r');
    dialog::addItem(XLAT("rainbow landscape"), 'l');
    dialog::addItem(XLAT("dark rainbow landscape"), 'd');
    dialog::addItem(XLAT("football"), 'F');

    dialog::addItem(XLAT("nice coloring"), 'T');

    dialog::addSelItem(XLAT("emerald pattern"), "emerald", 'e');

    dialog::addSelItem(XLAT("four elements"), "palace", 'b');
    dialog::addSelItem(XLAT("eight domains"), "palace", 'a');

    dialog::addSelItem(XLAT("zebra pattern"), "zebra", 'z');
    dialog::addSelItem(XLAT("four triangles"), "zebra", 't');
    dialog::addSelItem(XLAT("three stripes"), "zebra", 'x');

    dialog::addSelItem(XLAT("random black-and-white"), "current", 'w');

    dialog::addSelItem(XLAT("field pattern C"), "field", 'C');
    dialog::addSelItem(XLAT("field pattern D"), "field", 'D');
    dialog::addSelItem(XLAT("field pattern N"), "field", 'N');
    dialog::addSelItem(XLAT("field pattern S"), "field", 'S');

    dialog::display();
    
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      if((uni >= 'a' && uni <= 'z') || (uni >= 'A' && uni <= 'Z')) {
        whichCanvas = uni;
        subcanvas = rand();
        firstland = specialland = laCanvas; 
        randomPatternsMode = false;
        restartGame();
        }
      else if(doexiton(sym, uni)) popScreen();
      };    
    }
  
  void showPattern() {
    cmode = sm::SIDE | sm::MAYDARK;
    {
    dynamicval<int> dc(displaycodes, displaycodes ? displaycodes : 2);
    gamescreen(0);
    }
    dialog::init();
    
    if(stdhyperbolic || a4)
      dialog::addBoolItem(XLAT("Zebra Pattern"), (whichPattern == PAT_ZEBRA), PAT_ZEBRA);
    
    if(stdhyperbolic)
      dialog::addBoolItem(XLAT("Emerald Pattern"), (whichPattern == PAT_EMERALD), PAT_EMERALD);
    else if(a38)
      dialog::addBoolItem(XLAT("broken Emerald Pattern"), (whichPattern == PAT_EMERALD), PAT_EMERALD);
    
    if(stdhyperbolic || euclid)
      dialog::addBoolItem(XLAT("Palace Pattern"), (whichPattern == PAT_PALACE), PAT_PALACE);

    if(a38 || a46 || euclid)
      dialog::addBoolItem(XLAT("coloring"), (whichPattern == PAT_COLORING), PAT_COLORING);
    
    if(sphere)
      dialog::addBoolItem(XLAT("siblings"), (whichPattern == PAT_SIBLING), PAT_SIBLING);

    if(euclid)
      dialog::addBoolItem(XLAT("torus pattern"), (whichPattern == PAT_FIELD), PAT_FIELD);
    else if(sphere)
      dialog::addBoolItem(XLAT("single cells"), (whichPattern == PAT_FIELD), PAT_FIELD);
    else
      dialog::addBoolItem(XLAT("field pattern"), (whichPattern == PAT_FIELD), PAT_FIELD);

    if(
      (whichPattern == PAT_EMERALD && (stdhyperbolic || a38)) ||
      (whichPattern == PAT_PALACE && stdhyperbolic) ||
      (whichPattern == PAT_ZEBRA && stdhyperbolic) ||
      (whichPattern == PAT_SIBLING && sphere) ||
      (whichPattern == PAT_ZEBRA && a457)) {
      dialog::addBoolItem(XLAT("rotational symmetry"), subpattern_flags & SPF_ROT, '0');
      }
    
    if((euclid && whichPattern == PAT_COLORING) ||
      (a38 && whichPattern == PAT_COLORING))
      dialog::addBoolItem(XLAT("edit all three colors"), subpattern_flags & SPF_ROT, '0');

    if(euclid && whichPattern == PAT_COLORING)
      dialog::addBoolItem(XLAT("rotate the color groups"), subpattern_flags & SPF_CHANGEROT, '4');

    if((a38 && whichPattern == PAT_COLORING && !nontruncated) ||
      (a46 && whichPattern == PAT_COLORING && !nontruncated)
      )
      dialog::addBoolItem(XLAT("edit both truncated colors"), subpattern_flags & SPF_TWOCOL, '5');

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
    
    dialog::addBoolItem(XLAT("display pattern codes (full)"), (displaycodes == 1), 'd');
    dialog::addBoolItem(XLAT("display pattern codes (simplified)"), (displaycodes == 2), 's');

    dialog::addBoolItem(XLAT("display only hexagons"), (whichShape == '6'), '6');
    dialog::addBoolItem(XLAT("display only heptagons"), (whichShape == '7'), '7');
    dialog::addBoolItem(XLAT("display the triheptagonal grid"), (whichShape == '8'), '8');
    if(cheater || autocheat) dialog::addItem(XLAT("line patterns"), 'l');
    else dialog::addInfo("enable the cheat mode to use line patterns");

    if(!needConfirmation()) dialog::addItem(XLAT("predesigned patterns"), 'r');
    else dialog::addInfo("start a new game to use predesigned patterns");

    dialog::display();
    
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      if(among(uni, PAT_EMERALD, PAT_PALACE, PAT_ZEBRA, PAT_DOWN, PAT_FIELD, PAT_COLORING, PAT_SIBLING)) {
        if(whichPattern == uni) whichPattern = 0;
        else whichPattern = uni;
        mapeditor::modelcell.clear();
        }
      
      else if(uni >= '0' && uni <= '5') 
        subpattern_flags ^= (1 << (uni - '0'));

      else if(uni == '6' || uni == '7' || uni == '8') {
        if(whichShape == uni) whichShape = 0;
        else whichShape = uni;
        }
      else if(uni == 'd') displaycodes = displaycodes == 1 ? 0 : 1;
      else if(uni == 's') displaycodes = displaycodes == 2 ? 0 : 2;
      
      else if(uni == 'l' && (cheater || autocheat))
        pushScreen(linepatterns::showMenu);

      else if(uni == 'r' && !needConfirmation()) pushScreen(showPrePattern);
      
      else if(doexiton(sym, uni)) popScreen();
      };
    }
  
  }

namespace linepatterns {

  int lessalpha(int col, int m) {
    part(col, 0) /= m;
    return col;
    }
  
  int lessalphaif(int col, bool b) {
    return b?lessalpha(col, 4):col;
    }
    
  int lessalphaif(int col, bool b1, bool b2) {
    if(b1) col = lessalpha(col, 2);
    if(b2) col = lessalpha(col, 2);
    return col;
    }
    
  struct {
    int id;
    const char *lpname;
    unsigned int color;
    } patterns[] = {

    {patTriNet, "triangle grid: not rings", 0xFFFFFF00},
    {patTriRings, "triangle grid: rings", 0xFFFFFF00},
    {patHepta, "heptagonal grid", 0x0000C000},
    {patRhomb, "rhombic tesselation", 0x0000C000},
    {patTrihepta, "triheptagonal tesselation", 0x0000C000},
    {patNormal, "normal tesselation", 0x0000C000},
    {patBigTriangles, "big triangular grid", 0x00606000},
    {patBigRings, "big triangles: rings", 0x0000C000},
    
    {patTree, "underlying tree", 0x00d0d000},
    {patAltTree, "circle/horocycle tree", 0xd000d000},

    {patZebraTriangles, "zebra triangles", 0x40FF4000},
    {patZebraLines, "zebra lines", 0xFF000000},
    {patVine, "vineyard pattern", 0x8438A400},
    {patPalacelike, "firewall lines", 0xFF400000},
    {patPalace, "firewall lines: Palace", 0xFFD50000},
    {patPower, "firewall lines: Power", 0xFFFF0000},
    {0, NULL, 0}
    };

  void clearAll() {
    for(int k=0; patterns[k].lpname; k++) patterns[k].color &= ~255;
    }

  bool any() {
    for(int k=0; patterns[k].lpname; k++) if(patterns[k].color & 255) return true;
    return false;
    }

  void setColor(ePattern id, int col) {
    for(int k=0; patterns[k].lpname; k++)
      if(patterns[k].id == id) patterns[k].color = col;
    }
  
  void switchAlpha(ePattern id, int col) {
    for(int k=0; patterns[k].lpname; k++)
      if(patterns[k].id == id) patterns[k].color ^= col;
    }

  void drawPattern(int id, int col, cell *c, const transmatrix& V) {

    switch(id) {

      case patZebraTriangles:
        if(zebra40(c) / 4 == 10) {
          bool all = true;
          hyperpoint tri[3];
          for(int i=0; i<3; i++) {
            cell *c2 = createMov(c, i*2);
            if(!gmatrix.count(c2)) all = false;
            else tri[i] = tC0(gmatrix[c2]);
            }
          
          if(all) for(int i=0; i<3; i++)
            queueline(tri[i], tri[(i+1)%3], col, 3);
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
          
          double x = sphere?.3651:euclid?.2611:.2849;

          queueline(V * ddspin(c,i,-S14) * xpush0(x), 
            V * ddspin(c,i,+S14) * xpush0(x), 
            col, 1);
          }
        break;
      
      case patNormal: {
        double x = sphere?.401:euclid?.3 : .328;
        if(euclid || !pseudohept(c)) for(int t=0; t<c->type; t++) 
          if(euclid ? c->mov[t]<c : (((t^1)&1) || c->mov[t] < c))
            queueline(V * ddspin(c,t,-S7) * xpush0(x), 
                V * ddspin(c,t,+S7) * xpush0(x), 
                col, 1);
        break;
        }
      
      case patTrihepta:
        if(!pseudohept(c)) for(int i=0; i<6; i++) {
          cell *c2 = c->mov[i];
          if(!c2 || !pseudohept(c2)) continue;
          double x = sphere?.3651:euclid?.2611:.2849;
          queueline(V * ddspin(c,i,-S14) * xpush0(x), 
            V * ddspin(c,i,+S14) * xpush0(x), 
            col, 1);
          }
        break;
      
      case patTriNet:
        forCellEx(c2, c) if(c2 > c) if(gmatrix.count(c2)) if(celldist(c) != celldist(c2)) {
          queueline(tC0(V), gmatrix[c2]*C0, 
            darkena(backcolor ^ 0xFFFFFF, 0, col),
            2);
          }
        break;

      case patTriRings:
        forCellEx(c2, c) if(c2 > c) if(gmatrix.count(c2) && celldist(c) == celldist(c2)) 
          queueline(tC0(V), gmatrix[c2]*C0, 
            darkena(backcolor ^ 0xFFFFFF, 0, col),
            2);
        break;

      case patHepta:
        forCellEx(c2, c) if(c2 > c) if(gmatrix.count(c2) && pseudohept(c) == pseudohept(c2)) 
          queueline(tC0(V), gmatrix[c2]*C0, 
            darkena(backcolor ^ 0xFFFFFF, 0, col),
            2);
        break;

      case patRhomb:
        forCellEx(c2, c) if(c2 > c) if(gmatrix.count(c2) && pseudohept(c) != pseudohept(c2)) 
          queueline(tC0(V), gmatrix[c2]*C0, 
            darkena(backcolor ^ 0xFFFFFF, 0, col),
            2);
        break;
      
      case patPalace: {
        int a = polarb50(c);
        if(pseudohept(c)) for(int i=0; i<7; i++) {
            cell *c1 = createMov(c, (i+3) % 7);
            cell *c2 = createMov(c, (i+4) % 7);
            if(polarb50(c1) != a && polarb50(c2) != a)
                queueline(V * ddspin(c,i,84*5/14) * xpush0(tessf/2),
                          V * ddspin(c,i,84*9/14) * xpush0(tessf/2),
                                    col, 1);
            }
        break;
        }
      
      case patPalacelike:
        if(pseudohept(c)) for(int i=0; i<7; i++) 
          queueline(V * ddspin(c,i,84*5/14) * xpush0(tessf/2),
                    V * ddspin(c,i,84*9/14) * xpush0(tessf/2),
                              col, 1);
        break;
      
      case patBigTriangles: {
        if(pseudohept(c) && !euclid) for(int i=0; i<S7; i++) 
          if(c->master->move[i] < c->master) {
            queueline(tC0(V), V*xspinpush0((nontruncated?M_PI:0) -2*M_PI*i/S7, tessf), col, 2);
            }
        break;
        }
        
      case patBigRings: {
        if(pseudohept(c) && !euclid) for(int i=0; i<S7; i++) 
          if(c->master->move[i] && c->master->move[i] < c->master && c->master->move[i]->dm4 == c->master->dm4)
            queueline(tC0(V), V*xspinpush0((nontruncated?M_PI:0) -2*M_PI*i/S7, tessf), col, 2);
        break;
        }
        
      case patTree:
        if(ctof(c) && !euclid) 
          queueline(tC0(V), V*ddi0(nontruncated?S42:0, tessf), col, 2);
        break;
      
      case patAltTree:
        if(ctof(c) && !euclid && c->master->alt) {
          for(int i=0; i<S7; i++)
            if(c->master->move[i] && c->master->move[i]->alt == c->master->alt->move[0])
              queueline(tC0(V), V*xspinpush0((nontruncated?M_PI:0) -2*M_PI*i/S7, tessf), col, 2);
          }
        break;
      
      case patVine: {
        int p = emeraldval(c);
        double hdist = hdist0(heptmove[0] * heptmove[2] * C0);
        if(pseudohept(c) && (p/4 == 10 || p/4 == 8))
        for(int i=0; i<S7; i++) if(c->mov[i] && emeraldval(c->mov[i]) == p-4) {
          queueline(tC0(V), V*tC0(heptmove[i]), col, 2);
          queueline(tC0(V), V*tC0(spin(-i * ALPHA) * xpush(-hdist/2)), col, 2);
          }
        break;
        }
      
      case patPower: {
        int a = emeraldval(c);
        if(pseudohept(c) && a/4 == 8) for(int i=0; i<7; i++) {
            heptagon *h1 = c->master->move[(i+1)%7];
            heptagon *h2 = c->master->move[(i+6)%7];
            if(!h1 || !h2) continue;
            if(emeraldval(h1->c7)/4 == 8 && emeraldval(h2->c7)/4 == 8)
                queueline(V * ddspin(c,i,84*5/14) * xpush0(tessf/2),
                          V * ddspin(c,i,84*9/14) * xpush0(tessf/2),
                                    col, 1);
            }
        break;
        }
      }
    }  

  void drawAll() {

    if(any()) for(map<cell*, transmatrix>::iterator it = gmatrix.begin(); it != gmatrix.end(); it++) {
      cell *c = it->first;
      transmatrix& V = it->second;
      
      for(int k=0; patterns[k].lpname; k++) {
        int col = patterns[k].color;
        if(!(col & 255)) continue;
        int id = patterns[k].id;
        
        drawPattern(id, col, c, V);
        }
      }
    }
  
  int numpat = 0;
  
  void showMenu() {
    cmode = sm::SIDE | sm::MAYDARK;
    gamescreen(0);

    dialog::init(XLAT("line patterns"));
    
    for(numpat=0; patterns[numpat].lpname; numpat++)
      dialog::addColorItem(XLAT(patterns[numpat].lpname), patterns[numpat].color, 'a'+numpat);
  
    dialog::addBreak(50);
    dialog::addItem(XLAT("exit menu"), 'v');
    
    dialog::addBreak(50);
    dialog::addInfo("change the alpha parameter to show the lines");
  
    dialog::display();
    
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      if(uni >= 'a' && uni < 'a' + numpat) {
        dialog::openColorDialog(patterns[uni - 'a'].color, NULL);
        dialog::dialogflags |= sm::MAYDARK | sm::SIDE;
        }
      else if(doexiton(sym,uni)) popScreen();
      }; 
    }
  
  };

int val46(cell *c) {
  patterns::patterninfo si;
  patterns::val46(c, si, 0);
  return si.id;
  }

