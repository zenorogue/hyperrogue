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

int val46(cell *c) {
  return ctof(c) ? c->master->emeraldval :
    ((c->master->emeraldval & 1) ^ ((c->master->emeraldval & 2)>>1) ^ (c->spin(0)&1)) ? 8 : 4;
  }

int emeraldval(cell *c) {
  if(euclid) return eupattern(c);
  if(a46) return val46(c);
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

int val38(cell *c) {
  if(ctof(c)) return (c->master->fiftyval >> 1) & 3;
  else return 4 ^ (c->master->fiftyval & 1) ^ (c->spin(0) & 1);
  }
  
int fiftyval(cell *c) {
  if(a38) return val38(c);
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
  for(int i=0; i<4; i++) 
    if(zebra40(createMov(c, i*2))&2) wset |= (1<<i);
  if(wset == 0) return -8;
  if(wset == 15) return -10;
  if(wset == 3) return 1;
  if(wset == 6) return 3;
  if(wset == 12) return 5;
  if(wset == 9) return 7;
  return 0;
  }

int zebra40(cell *c) {
  if(euclid) return eupattern(c);
  else if(a46) return val46(c);
  else if(ctof(c)) return (c->master->zebraval/10);
  else if(a4) {
    int ws = dir_truncated457(c);
    if(ws < 0) return -ws;
    return 16 + (ws/2);
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

struct sphereinfo {
  int id;
  int dir;
  bool reflect;
  };

sphereinfo valsphere(cell *c) {
  sphereinfo si;
  if(ctof(c)) {
    int d = c->master->fieldval;
    si.id = (d < siblings[d]) ? 0 : 1;
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
      sphereinfo si2 = valsphere(c->mov[0]);
      int di = si2.dir - c->spin(0);
      di %= S7; 
      if(di<0) di += S7;
      si.reflect = di > S7/2;
      }
    }
  return si;
  }

namespace mapeditor {

  int nopattern(cell *c) {
    if(isWarped(c) && !euclid) {
      int u = ishept(c)?1:0;
      int qhex = 0;
      for(int v=0; v<c->type; v++) if(c->mov[v] && !isWarped(c->mov[v])) {
        u += 2;
        if(!ishept(c->mov[v])) qhex++;
        }
      if(u == 8 && qhex == 2) return 12;
      if(u == 2 && qhex == 1) return 8;
      if(u == 6 && qhex == 2) return 10;
      return u;
      }
    return ishept(c) ? 1 : ishex1(c) ? 2 : 0; // 0 to 1
    }
  
  int downdir(cell *c, cellfunction *cf = coastvalEdge) {
    cell *c2 = chosenDown(c, 1, 1, cf);
    if(!c2) return 0;
    return neighborId(c, c2);
    }

  int realpattern(cell *c, char code) {
    switch(code) { 
      case 'z':
        return zebra40(c); // 4 to 43
      case 'f':
        return emeraldval(c); // 44 to 99
      case 'p': {
        if(a46) return val46(c);
        if(a38) return val38(c);
        if(sphere) return valsphere(c).id;
        int i = fiftyval049(c);
        i *= 4;
        if(polara50(c)) i|=1;
        if(polarb50(c)) i|=2;
        return i;
        }
      case 'H': 
        return towerval(c);
      case 'F': {
        if(euclid)
          // use the torus ID
          return fieldpattern::fieldval_uniq(c);
        else if(nontruncated)
          // use the actual field codes
          return fieldpattern::fieldval(c).first;
        else          
          // use the small numbers from windmap
          return windmap::getId(c);
        }
      }
    return nopattern(c);
    }

    int patterndir46(cell *c, int bits) {
      if(ctof(c)) {
        int b = c->master->emeraldval & bits;
        return (b&1) ^ (b & 2 ? 1 : 0);
        }
      else
        return ((c->mov[0]->master->emeraldval + c->spin(0)) & 1) ? 2 : 0;
      }
    
    int patterndir38(cell *c) {
      if(ctof(c)) return c->master->fiftyval | (c->master->fiftyval & 8 ? 0 : 2);
      return 0;
      }
          
    int patterndir457(cell *c) {
      if(!ctof(c)) {
        int d = dir_truncated457(c);
        if(d >= 0) return d;
        return 0;
        }
      for(int i=0; i<c->type; i++)
        if((zebra40(createStep(c->master, i + S7/2)->c7)&2) == (zebra40(createStep(c->master, i + 1 + S7/2)->c7)&2))
          return i;
      return 0;
      }
    
    bool reflectPatternAt(cell *c, char p) {
      if(p == 'p' && sphere) return valsphere(c).reflect;
      if(p == 'p' && polarb50(c)) return true;
      if(p == 0) {
        int np = nopattern(c);
        if(np == 4) {
          int d = patterndir(c);
          return !isWarped(createMov(c, (d+1)%6));
          }
        if(np == 12) {
          int d = patterndir(c);
          return !isWarped(createMov(c, (d+1)%6));
          }
        }
      return false;
      }

    int patterndir(cell *c, char w) {
        if(w != 'H') {
          if(a46) return patterndir46(c, w == 'z' ? 3 : w == 'p' ? 2 : 1);
          if(a4) return patterndir457(c);
          if(a38) return patterndir38(c);
          if(sphere) return valsphere(c).dir;
          }
        switch(w) {
            case 'z': {
                int t = zebra40(c);                

                if(euclid) return (t*4) % 6;
                
                int t4 = t>>2, tcdir = 0;
                
                if(nontruncated) tcdir = t^1;
                
                else if(t4 == 10) tcdir = t-20;
                else if(t4 >= 4 && t4 < 7) tcdir = 40 + (t&3);
                else if(t4 >= 1 && t4 < 4) tcdir = t+12;
                else if(t4 >= 7 && t4 < 10) tcdir = t-24;
                
                for(int i=0; i<c->type; i++) if(c->mov[i] && zebra40(c->mov[i]) == tcdir)
                    return i;
                
                // printf("fail to fintd %d -> %d\n", t, tcdir);
                
                return 0;
            }
                
            case 'f': {
                int t = emeraldval(c);
                if(euclid) return 0;
                int tcdir = 0, tbest = (t&3);
                for(int i=0; i<c->type; i++) {
                    cell *c2 = c->mov[i];
                    if(c2) {
                        int t2 = emeraldval(c2);
                        if((t&3) == (t2&3) && t2 > tbest)
                            tbest = t2, tcdir = i;
                    }
                }
                return tcdir;
            }
                
            case 'p': {
                int tcdir = -1, tbest = -1;
                int pa = polara50(c);
                int pb = polarb50(c);
                for(int i=0; i<c->type; i++) {
                    cell *c2 = c->mov[i];
                    if(c2 && polara50(c2) == pa && polarb50(c2) == pb) {
                        int t2 = fiftyval049(c2);
                        if(t2 > tbest) tbest = t2, tcdir = i;
                    }
                }
                return tcdir;
            }
                
            case 'H':
                return downdir(c);
                
            case 0: {
                if(euclid) return 0;
                int u = nopattern(c);
                
                if(u == 6) {
                    for(int i=1; i<c->type; i+=2) if(!isWarped(createMov(c,i)))
                        return i;
                    }
                
                else if(u == 2 || u == 3 || u == 8) {
                    for(int i=0; i<c->type; i++) if(!isWarped(createMov(c,i)))
                        return i;
                    }
                
                else if(u == 4 || u == 10) {
                    for(int i=0; i<c->type; i+=2) if(!isWarped(createMov(c,i)))
                        return i;
                    }
                
                else if(u == 6) {
                    for(int i=1; i<c->type; i+=2) if(!isWarped(createMov(c,i))) 
                        return i;
                    }
                
                else if(u == 5) {
                    for(int i=0; i<c->type; i++) if(!isWarped(createMov(c,(i+3)%7)) && !isWarped(createMov(c,(i+4)%7))) 
                        return i;
                    }
                
                else if(u == 9) {
                    for(int i=0; i<c->type; i++) if(!isWarped(createMov(c,(i+2)%7)) && !isWarped(createMov(c,(i+5)%7))) 
                        return i;
                    }
                
                else if(u == 11) {
                    for(int i=0; i<c->type; i++) if(isWarped(createMov(c,(i)%7)) && isWarped(createMov(c,(i+1)%7))) 
                        return i;
                    }
                
                else if(u == 12) {
                    for(int i=0; i<c->type; i+=2) if(isWarped(createMov(c,i)))
                        return i;
                    }

                else if(u == 7) {
                    for(int i=0; i<c->type; i++) if(!isWarped(createMov(c,(i+1)%7)) && !isWarped(createMov(c,(i+6)%7))) 
                        return i;
                    }
            
                else if(u < 2) return 0;

#if LOCAL                
                printf("unhandled: u=%d\n", u);
#endif
            }
        }
        return 0;
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
    int i = val38(c);
    if(nontruncated) return i;
    else return i < 4 ? 0 : (1+(i&1));
    }
  if(a46 && !nontruncated) {
    int i = val46(c);
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
  return 3;
  }
  
// returns ishept in the normal tiling;
// in the 'pure heptagonal' tiling, returns true for a set of cells
// which roughly corresponds to the heptagons in the normal tiling
bool pseudohept(cell *c) {
  if(nontruncated) {
    if(bigsphere) 
      return 
        c->master == getDodecahedron(3) ||
        c->master == getDodecahedron(5) ||
        c->master == getDodecahedron(6);
    if(S7 == 3)
      return c->master == getDodecahedron(0);
    else
      return pattern_threecolor(c) == 0;
    }
  else return ishept(c);
  }

