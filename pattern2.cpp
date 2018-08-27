// HyperRogue patterns: compute codes for actual cells

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

int gp_threecolor() {
  if(!gp::on) return 0;
  if(S3 == 3 && (gp::param.first - gp::param.second) % 3 == 0) return 2;
  return 1;
  }

int eupattern(cell *c) {
  int v = cell_to_vec(c);
  if(a4) {
    int x, y;
    tie(x,y) = vec_to_pair(v);
    return ((x&1) + 2*(y&1)) % 3;
    }
  else {
    return gmod(v*2, 3);
    }
  }

int eupattern4(cell *c) {
  int v = cell_to_vec(c);
  int x, y;
  tie(x,y) = vec_to_pair(v);
  return (x&1) + ((y&1)) * 2;
  }

bool ishept(cell *c) {
  // EUCLIDEAN
  if(euclid) return eupattern(c) == 0;
  else return c->type == S7;
  }

bool ishex1(cell *c) {
  // EUCLIDEAN
  if(euclid) return eupattern(c) == 1;
  else if(gp::on) return c->master->c7 != c && !pseudohept(c->move(0));
  else return c->type != S6;
  }

bool ishex2(cell *c) {
  // EUCLIDEAN
  if(euclid) return eupattern(c) == 1;
  else if(gp::on) return c->master->c7 != c && gp::pseudohept_val(c) == 1;
  else return c->type != S6;
  }

int chessvalue(cell *c) {
  if(archimedean) 
    return arcm::chessvalue(c);
  else
    return celldist(c) & 1;
  }

int emeraldval(heptagon *h) { return h->emeraldval >> 3; }

int emeraldval(cell *c) {
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
  if(torus) {
    if(c->land == laWildWest) return cell_to_vec(c) % 37;
    else return cell_to_vec(c) % 27;
    }
  int x, y;
  tie(x,y) = cell_to_pair(c);
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

int cdist50(cell *c) {
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

int polara50(cell *c) {
  if(sphere || euclid || S7>7 || S6>6) return 0;
  else if(gp::on || irr::on) return polara50(fiftyval(c->master->c7));
  else if(ctof(c)) return polara50(fiftyval(c));
  else {
    auto ar = gp::get_masters(c);
    for(int i=0; i<3; i++)
      if(cdist50(ar[i]->fiftyval) < 3) return polara50(ar[i]->fiftyval);
    return 0;
    }
  }

int polarb50(cell *c) {
  if(euclid) return true;
  if(sphere || euclid || S7>7 || S6>6) return true;
  else if(gp::on || irr::on) return polarb50(fiftyval(c->master->c7));
  else if(ctof(c)) return polarb50(fiftyval(c));
  else {
    auto ar = gp::get_masters(c);
    for(int i=0; i<3; i++)
      if(cdist50(ar[i]->fiftyval) < 3) return polarb50(ar[i]->fiftyval);
    return 0;
    }
  }

int elhextable[28][3] = {
  {0,1,2}, {1,2,9}, {1,9,-1}, {1,8,-1}, {1,-1,-1}
  };

int fiftyval049(heptagon *h) {
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

int fiftyval049(cell *c) {
  if(euclid) return fiftyval(c) / 32;
  else if(ctof(c)) return fiftyval049(c->master);
  else if(sphere) return 0;
  else {
    int a[3], qa=0;
    int pa = polara50(c), pb = polarb50(c);
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

int fiftyval200(cell *c) {
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
  if(gp::on) return c->master->zebraval / 10;
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
  else if(irr::on) return c->master->zebraval/10;
  else if(a46) {
    int v = val46(c);
    if(v<4) return v;
    else return 4+(v-4)/2;
    }
  else if(ctof(c)) return (c->master->zebraval/10);
  else if(a4) {
    if(gp::on) return zebra40(c->master->c7);
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
  else if(euclid) return eupattern(c);
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

int zebra3(cell *c) {
  if(ctof(c)) return (c->master->zebraval/10)/4;
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

namespace fieldpattern {

pair<int, bool> fieldval(cell *c) {
  if(ctof(c)) return make_pair(c->master->fieldval, false);
  else return make_pair(btspin(c->master->fieldval, c->c.spin(0)), true);
  }

int fieldval_uniq(cell *c) {
  if(sphere) {
    if(archimedean) return c->master->fiftyval;
    else if(irr::on) return irr::cellindex[c];
    else if(gp::on) return (get_code(gp::get_local_info(c)) << 8) | (c->master->fieldval / S7);
    if(ctof(c)) return c->master->fieldval;
    else return createMov(c, 0)->master->fieldval + 256 * createMov(c,2)->master->fieldval + (1<<16) * createMov(c,4)->master->fieldval;
    }
  else if(torus) {
    return decodeId(c->master);
    }
  else if(euclid) {
    auto p = cell_to_pair(c);
    return gmod(p.first * torusconfig::dx + p.second * torusconfig::dy, torusconfig::qty);
    }
  else if(binarytiling || archimedean) return 0;
  else if(ctof(c) || gp::on || irr::on) return c->master->fieldval/S7;
  else {
    int z = 0;
    for(int u=0; u<S6; u+=2) 
      z = max(z, btspin(createMov(c, u)->master->fieldval, c->c.spin(u)));
    return -1-z;
    }
  }

int fieldval_uniq_rand(cell *c, int randval) {
  if(sphere || torus || euclid || gp::on || irr::on) 
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

int getHemisphere(heptagon *h, int which) {
  int id = h->fiftyval;
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

int getHemisphere(cell *c, int which) {
  if(torus) return 0;
  if(which == 0 && gp::on && has_nice_dual()) {
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
    if(gp::on) {
      auto li = gp::get_local_info(c);
      gp::be_in_triangle(li);
      auto corner = gp::corners * gp::loctoh_ort(li.relative);
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
    else if(irr::on) {
      auto m = irr::get_masters(c);
      for(int i=0; i<3; i++)
        score += getHemisphere(m[i], which);
      return score / 3;
      }
    else {
      for(int i=0; i<6; i+=2) 
        score += getHemisphere(c->move(i), which) * (c->c.mirror(i) ? -1 : 1);
      return score/3;
      }
    }
  }

namespace patterns {

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
  
  int downdir(cell *c, cellfunction *cf) {
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
  
  void val38(cell *c, patterninfo &si, int sub, int pat) {
    bool symRotation = sub & SPF_ROT;

    if(ctof(c)) {
      if(!symRotation) 
        si.id = (c->master->fiftyval >> 1) & 3;
      else
        si.id = 0;
      if(nonbitrunc && gp_threecolor() != 2)
        si.id *= 4;
      else 
        si.id += 4;
      si.dir = (pat == PAT_COLORING && !nonbitrunc ? 1 : 0) + (c->master->fiftyval | (c->master->fiftyval & 8 ? 0 : 2));
      si.symmetries = 2;
      si.id += 8;
      si.id %= 12;
      applyAlt(si, sub, pat);
      if((sub & SPF_DOCKS) && (c->master->fiftyval & 32))
        si.id += 16, si.symmetries = 4;
      }
    else {
      int sp = c->c.spin(0);
      if(gp::on) {
        sp = gp::last_dir(c);
        sp ^= int(ishex2(c));
        }
      if(geometry == gBolza2 && (!gp::on || gp_threecolor() == 2)) {
        patterninfo si0;
        patterninfo si1;
        if(gp::on) {
          auto li = gp::get_local_info(c);
          val38(c->master->c7, si0, 0, PAT_COLORING);
          val38(c->master->move(li.last_dir)->c7, si1, 0, PAT_COLORING);
          }
        else {
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
      if(gp::on && pseudohept(c)) si.id = 4;
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
      if(gp::on && pat == PAT_COLORING) 
        for(int i=0; i<c->type; i++) {
          cell *c2 = createMov(c, i);
          int id2 = 4;
          if(!pseudohept(c2)) {
            int sp2 = c2->c.spin(0);
            if(gp::on) {
              sp2 = gp::last_dir(c2);
              sp2 ^= int(ishex2(c2));
              }
            id2 = 8 * ((c2->master->fiftyval & 1) ^ (sp2 & 1));
            }
//          printf("%p %2d : %d %d\n", c, si.id, i, id2);
          if((id2+4) % 12 == si.id) si.dir = i;
          }
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
    if(euclid6 && (sub & SPF_FULLSYM))
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
    if(irr::on || archimedean || binarytiling) si.symmetries = 1;
    else if(a46) val46(c, si, sub, pat);
    else if(a38) val38(c, si, sub, pat);
    else if(sphere) valSibling(c, si, sub, pat);
    else if(euclid4) valEuclid4(c, si, sub);
    else if(euclid) valEuclid6(c, si, sub);
    else if(a4) val457(c, si, sub);
    else si.symmetries = ctof(c) ? 1 : 2;
    }

  void val_warped(cell *c, patterninfo& si) {
    int u = ishept(c)?1:0;
    if(S3 != 3 || S7 != 7 || gp::on || irr::on) {
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
    if((gp::on? (S3==3) : !weirdhyperbolic) && isWarped(c)) 
      val_warped(c, si);
    else {
      si.id = pseudohept(c) ? 1 : 0;
      if(euclid) {
        si.dir = ishex1(c) ? 0 : 3;
        if(ctof(c)) si.symmetries = 3;
        if(subpattern_flags & SPF_EXTRASYM) 
          si.symmetries /= 3;
        if(subpattern_flags & SPF_FULLSYM) 
          si.symmetries = 1;
        }      
      if(sphere && !(nonbitrunc) && !(S7 == 3))
        si.symmetries = ctof(c) ? 1 : 2;
      if(sphere && (sub & SPF_EXTRASYM)) {
        si.symmetries = ctof(c) ? 1 : 2;
        }
      if(a38)
        si.symmetries = (ctof(c) && !nonbitrunc) ? 1 : 2;
      if(a457) {
        si.symmetries = ctof(c) ? 1 : 2;
        if(!ctof(c)) si.dir = 0;
        }
      if(a46) {
        si.symmetries = ctof(c) ? 1 : 2;
        }
      }
    
    if(gp::on && has_nice_dual() && !ishept(c) && ishex1(c)) si.dir = fix6(si.dir+3);
    }
  
  char whichPattern = 0;

  int subpattern_flags;
 
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
    if(euclid6 && (sub & SPF_CHANGEROT))
      si.dir = (zebra40(c)*4) % 6;
    if(sub & SPF_ROT) si.id = 1;
    if(euclid6 && !(sub & SPF_EXTRASYM)) {
      si.symmetries = 6;
      }
    if(euclid6 && (sub & SPF_FULLSYM))
      si.symmetries = 1;
    applyAlt(si, sub, PAT_COLORING);
    }
  
  patterninfo getpatterninfo(cell *c, char pat, int sub) {
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
    
    if(archimedean && pat == 0) {
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

    if(pat == PAT_ZEBRA && stdhyperbolic) {

      si.id = zebra40(c); // 4 to 43
      int t4 = si.id>>2, tcdir = 0;
      
      if(nonbitrunc) tcdir = si.id^1;
      
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
        look_for = si.id + (nonbitrunc ? 4 : 60);
        if(symRotation) si.symmetries = 1;
        }
      else if(inr(si.id, 4, 32)) look_for = si.id + (nonbitrunc ? 28 : 168);
      else if(inr(si.id, 32, 60)) look_for = si.id + (nonbitrunc ? -28 : 112);
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
      si.id = towerval(c);
      si.dir = downdir(c);
      }
    
    else if(pat == PAT_FIELD) {
      if(euclid)
        // use the torus ID
        si.id = fieldpattern::fieldval_uniq(c);
      else if(nonbitrunc && !archimedean && !gp::on && !irr::on)
        // use the actual field codes
        si.id = fieldpattern::fieldval(c).first;
      else          
        // use the small numbers from windmap
        si.id = windmap::getId(c); 
      // todo dir
      }
    
    else if(sphere && pat == PAT_SIBLING) {
      val_all(c, si, sub, pat);
      }

    else if(a457 && pat == PAT_ZEBRA) {
      val_all(c, si, sub, pat);
      }
    
    else if(gp::on) {
      bool football = (pat == PAT_COLORING && (sub & SPF_FOOTBALL)) || pat == 0;
      if(football) val_nopattern(c, si, sub);
      else val_threecolors(c, si, sub);
      }

    else if(pat == PAT_COLORING && (S7 == 4 || euclid || (a38 && gp_threecolor() == 1) || archimedean)) {
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

  }

bool geosupport_chessboard() {
  return archimedean ? arcm::current.support_chessboard() : (nonbitrunc && S3 == 4);
  }

int geosupport_threecolor() {
  if(irr::on) return 0;
  if(!nonbitrunc && S3 == 3) {
    if(S7 % 2) return 1;
    return 2;
    }
  if((S7 % 2 == 0) && (S3 == 3))
    return 2;
  if(a46 && nonbitrunc)
    return 1;
  if(archimedean) return arcm::current.support_threecolor();
  return 0;
  }

int geosupport_football() {
  // always works in bitrunc geometries
  if(!nonbitrunc) return 2;

  if(archimedean) return arcm::current.support_football();

  if(irr::on) return irr::bitruncations_performed ? 2 : 1;
  
  // always works in patterns supporting three-color
  int tc = max(geosupport_threecolor(), gp_threecolor());
  if(tc) return tc;
  
  if(S3 == 3 && S7 == 7) return 1;
  // nice chessboard pattern, but not the actual Graveyard
  if(S3 == 4 && !(S7&1)) return 1;
  if(S3 == 4 && gp::on) return 1;
  return 0;
  }

int pattern_threecolor(cell *c) {
  if(archimedean) return arcm::threecolor(arcm::id_of(c->master));
  if(irr::on || binarytiling) return !pseudohept(c);
  if(S3 == 3 && !(S7&1) && gp_threecolor() == 1 && c->master->c7 != c) {
    auto li = gp::get_local_info(c);
    int rel = (li.relative.first - li.relative.second + MODFIXER) % 3;
    int par = (gp::param.first - gp::param.second + MODFIXER) % 3;
    if(rel == 0)
      return pattern_threecolor(c->master->c7);
    else if(rel == par)
      return pattern_threecolor(createStep(c->master, li.last_dir)->c7);
    else
      return pattern_threecolor(createStep(c->master, fix7(li.last_dir+1))->c7);
    }
  if(a38) {
    // if(gp::on && gp_threecolor() == 2 && gp::pseudohept_val(c) == 0) return 0;
    patterns::patterninfo si;
    patterns::val38(c, si, nonbitrunc ? 0 : patterns::SPF_ROT, patterns::PAT_COLORING);
    return si.id >> 2;
    }
  if(a4 && gp::on) {
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
  if(a46 && !nonbitrunc) {
    patterns::patterninfo si;
    patterns::val46(c, si, 0, patterns::PAT_COLORING);
    int i = si.id;
    return i >> 2;
    }
  if(euclid) {
    if(a4 && nonbitrunc) return eupattern4(c);
    return eupattern(c) % 3;
    }
  if(S7 == 4 && S3 == 3) {
    int codesN[6] = {0,1,2,1,2,0};
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
    if(nonbitrunc) 
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
  if(stdhyperbolic && nonbitrunc) {
    int z = zebra40(c);
    if(z == 5 || z == 8 || z == 15) return 0;
    if(c->land == laSnakeNest) {
      if(z == 10 || z == 12 || z == 7) return 2;
      if(z == 6 || z == 9) return 3;
      if(z == 14 || z == 11) return 4;
      }
    return 1;
    }
  if(a46 && nonbitrunc) {
    patterns::patterninfo si;
    patterns::val46(c, si, 0, patterns::PAT_COLORING);
    return si.id;
    }
  if(S7 == 5 && nonbitrunc && S3 == 3) {
    const int codes[12] = {1, 2, 0, 3, 2, 0, 0, 1, 3, 1, 2, 3};
    return codes[c->master->fiftyval];
    }
  if(S7 == 3 && nonbitrunc)
    return c->master->fiftyval;
  if(gp_threecolor() && (S7&1))
    return gp::pseudohept_val(c) > 0;
  return !ishept(c);
  }
  
// returns ishept in the normal tiling;
// in the 'pure heptagonal' tiling, returns true for a set of cells
// which roughly corresponds to the heptagons in the normal tiling
bool pseudohept(cell *c) {
  if(irr::on) return irr::pseudohept(c);
  if(binarytiling) return c->type & c->master->distance & 1;
  if(archimedean) return arcm::pseudohept(arcm::id_of(c->master));
  if(gp::on && gp_threecolor() == 2)
    return gp::pseudohept_val(c) == 0;
  if(gp::on && gp_threecolor() == 1 && (S7&1) && (S3 == 3))
    return gp::pseudohept_val(c) == 0;
  return pattern_threecolor(c) == 0;
  }

// while Krakens movement is usually restricted to non-pseudohept cells,
// there is one special case when this does not work (because non-pseudohept cells have varying degrees)
bool kraken_pseudohept(cell *c) {
  if(!euclid && S3 == 4 && gp::on && (gp::param.first % 2 || gp::param.second % 2 || S7 % 2))
    return ishept(c);
  else if(irr::on)
    return c->type != 6;
  else if(archimedean)
    return c->type == isize(arcm::current.triangles[0]);
  else if(!euclid && S3 == 3 && !(S7&1) && gp_threecolor() == 1)
    return ishept(c);
  else
    return pseudohept(c);
  }

bool warptype(cell *c) {
  if(geosupport_chessboard()) 
    return chessvalue(c);
  else if(gp::on || irr::on)
    return pseudohept(c);
  else
    return pattern_threecolor(c) == 0;
  }

namespace patterns {
  int canvasback = linf[laCanvas].color >> 2;
  int subcanvas;
  bool displaycodes;
  char whichShape = 0;
  char whichCanvas = 0;

  int generateCanvas(cell *c) {
    if(whichCanvas == 'A' && archimedean) {
      int gcolors[16] = {
        0xF04040, 0x40F040, 0x4040F0,
        0xD0D000, 0xD000D0, 0x00D0D0,
        0xC0C0C0, 0x404040, 0x808080,
        0xF08040, 0xF04080, 0x40F080,
        0x4080F0, 0x8040F0, 0x80F040,
        0xFFD500 };
      return gcolors[arcm::current.tilegroup[arcm::id_of(c->master)] & 15];
      }
    if(whichCanvas == 'B') {
      int gcolors[16] = {
        // trying to get colors as in Wikipedia [ https://en.wikipedia.org/wiki/Euclidean_tilings_by_convex_regular_polygons#k-uniform_tilings ]
        0, 0, 0xFFFFFF, 0xFFFF00, 
        0xFF0000, 0xC000C0 /* unknown5 */, 0x00FF00, 0x00C0C0 /* unknown7 */, 0xFF8000, 
        0xFFFF80, 0xC040C0, 0xFFD500, 0x000080,
        0x404040, 0x606060, 0x808080
        };
      return gcolors[c->type & 15];
      }
    if(whichCanvas == 'C' && hyperbolic) {
      using namespace fieldpattern;
      int z = currfp.getdist(fieldval(c), make_pair(0,false));
      if(z < currfp.circrad) return 0x00C000;
      int z2 = currfp.getdist(fieldval(c), make_pair(currfp.otherpole,false));
      if(z2 < currfp.disthep[currfp.otherpole] - currfp.circrad)
        return 0x3000;
      return 0x6000;
      }
    if(whichCanvas == 'D' && hyperbolic) {
      using namespace fieldpattern;
      int z = currfp.getdist(fieldval(c), make_pair(0,false));
      return 255 * (currfp.maxdist+1-z) / currfp.maxdist;
      }
    if(whichCanvas == 'N' && hyperbolic) {
      using namespace fieldpattern;
      int z = currfp.getdist(fieldval(c), make_pair(0,false));
      int z2 = currfp.getdist(fieldval(c), make_pair(currfp.otherpole,false));
      if(z < z2) return 0x00C000;
      if(z > z2) return 0xC00000;
      return 0xCCCC00;
      }
    if(whichCanvas == 'M') {
      int cd = celldist(c);
      return gradient(0, canvasback, 0, min(1.8/(1+cd), 1.), 1);
      }
    if(whichCanvas == 'S' && hyperbolic) {
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
    if(whichCanvas == 'c') {
      return chessvalue(c) ? 0xC0C0C0 : 0x202020;
      }
    if(whichCanvas == 'F') {
      return pseudohept(c) ? 0x202020 : 0xC0C0C0;
      }
    if(whichCanvas == 'T') {
      int fv = pattern_threecolor(c);
      return nestcolors[fv&7];
      }
    return canvasback;
    }

  void showPrePattern() {
    cmode = sm::SIDE | sm::MAYDARK;
    gamescreen(0);

    dialog::init("predesigned patterns");
    dialog::addItem(XLAT("single color"), 'g');
    dialog::addItem(XLAT("random colors"), 'r');
    dialog::addItem(XLAT("distance from origin"), 'M');
    
    if(stdeuc) {
      dialog::addItem(XLAT("rainbow landscape"), 'l');
      dialog::addItem(XLAT("dark rainbow landscape"), 'd');
      }

    dialog::addItem(XLAT("football"), 'F');
    if(geosupport_chessboard())
      dialog::addItem(XLAT("chessboard"), 'c');

    dialog::addItem(XLAT("nice coloring"), 'T');

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

    if(!sphere) {
      dialog::addSelItem(XLAT("field pattern C"), "field", 'C');
      dialog::addSelItem(XLAT("field pattern D"), "field", 'D');
      dialog::addSelItem(XLAT("field pattern N"), "field", 'N');
      dialog::addSelItem(XLAT("field pattern S"), "field", 'S');
      }
    
    if(archimedean)
      dialog::addSelItem(XLAT("Archimedean"), "Archimedean", 'A');

    dialog::addSelItem(XLAT("sides"), "sides", 'B');

    dialog::addBreak(100);
    dialog::addBoolItem(XLATN(winf[waInvisibleFloor].name), canvas_invisible, 'i');

    dialog::display();
    
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      if(uni == 'g') {
        static unsigned c = (canvasback << 8) | 0xFF;
        static unsigned canvasbacks[] = {
          6, 0xFFFFFFFF, 0x101010FF, 0x404040FF, 0x808080FF, 0x800000FF, unsigned(linf[laCanvas].color >> 2) << 8
          }; 
        dialog::openColorDialog(c, canvasbacks);
        dialog::reaction = [] () {
          stop_game();
          whichCanvas = 'g';
          canvasback = c >> 8;
          firstland = specialland = laCanvas;
          randomPatternsMode = false;
          start_game();
          };
        }
      else if(uni == 'i') {
        stop_game();
        canvas_invisible = !canvas_invisible;
        firstland = specialland = laCanvas; 
        randomPatternsMode = false;
        start_game();
        }
      else if((uni >= 'a' && uni <= 'z') || (uni >= 'A' && uni <= 'Z')) {
        stop_game();
        whichCanvas = uni;
        subcanvas = rand();
        firstland = specialland = laCanvas; 
        randomPatternsMode = false;
        start_game();
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

#if CAP_TEXTURE
#define REMAP_TEXTURE texture::config.remap()
#else
#define REMAP_TEXTURE
#endif
  
  void showPattern() {
    cmode = sm::SIDE | sm::MAYDARK;
    {
    dynamicval<bool> dc(displaycodes, true);
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
    
    if(geosupport_chessboard())
      dialog::addBoolItem(XLAT("chessboard"), (whichPattern == PAT_CHESS), PAT_CHESS);

    if(geosupport_threecolor() == 2)
      dialog::addBoolItem(XLAT("coloring"), (whichPattern == PAT_COLORING), PAT_COLORING);
    
    if(sphere_narcm)
      dialog::addBoolItem(XLAT("siblings"), (whichPattern == PAT_SIBLING), PAT_SIBLING);
    
    if(euclid)
      dialog::addBoolItem(XLAT("torus pattern"), (whichPattern == PAT_FIELD), PAT_FIELD);
    else if(sphere)
      dialog::addBoolItem(XLAT("single cells"), (whichPattern == PAT_FIELD), PAT_FIELD);
    else
      dialog::addBoolItem(XLAT("field pattern"), (whichPattern == PAT_FIELD), PAT_FIELD);
    
    dialog::addBoolItem(XLAT("single type"), (whichPattern == PAT_SINGLETYPE), PAT_SINGLETYPE);

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
      (a4 && nonbitrunc && whichPattern == PAT_COLORING && !a46))
      dialog::addBoolItem(XLAT("edit all three colors"), subpattern_flags & SPF_ROT, '0');

    if(euclid && whichPattern == PAT_COLORING)
      dialog::addBoolItem(XLAT("rotate the color groups"), subpattern_flags & SPF_CHANGEROT, '4');

    if(a46 && whichPattern == PAT_COLORING)
      dialog::addBoolItem(XLAT("rotate the color groups"), subpattern_flags & SPF_CHANGEROT, '4');

    if(a46 && whichPattern == PAT_COLORING && !nonbitrunc)
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
    if(euclid && among(whichPattern, PAT_COLORING, 0) && !archimedean)
      dialog::addBoolItem(XLAT("extra symmetries"), subpattern_flags & SPF_EXTRASYM, '=');
    
    if(archimedean && arcm::current.have_symmetry && whichPattern == 0)
      dialog::addBoolItem(XLAT("extra symmetries"), subpattern_flags & SPF_EXTRASYM, '=');

    if(whichPattern == PAT_SINGLETYPE) {
      dialog::addBoolItem(XLAT("odd/even"), subpattern_flags & SPF_TWOCOL, '5');
      dialog::addBoolItem(XLAT("extra symmetries"), subpattern_flags & SPF_EXTRASYM, '=');
      }

    if(euclid && among(whichPattern, PAT_COLORING, 0))
      dialog::addBoolItem(XLAT("full symmetry"), subpattern_flags & SPF_FULLSYM, '!');

    if(a38 && nonbitrunc && whichPattern == 0) {
      dialog::addBoolItem(XLAT("extra symmetries"), subpattern_flags & SPF_EXTRASYM, '=');
      }

    if(a46 && nonbitrunc && whichPattern == PAT_COLORING) {
      dialog::addBoolItem(XLAT("extra symmetries"), subpattern_flags & SPF_EXTRASYM, '=');
      }

    if((whichPattern == PAT_COLORING) || (whichPattern == 0 && archimedean)) {
      dialog::addBoolItem(XLAT("alternate coloring"), subpattern_flags & SPF_ALTERNATE, '\'');
      dialog::addBoolItem(XLAT("football pattern"), subpattern_flags & SPF_FOOTBALL, '*');
      }
    
    if(a38 && whichPattern == PAT_COLORING)
      dialog::addBoolItem(XLAT("Docks pattern"), subpattern_flags & SPF_DOCKS, '@');
    
    dialog::addBoolItem(XLAT("display pattern codes (full)"), displaycodes, 'd');

    dialog::addBoolItem(XLAT("display only hexagons"), (whichShape == '6'), '6');
    dialog::addBoolItem(XLAT("display only heptagons"), (whichShape == '7'), '7');
    dialog::addBoolItem(XLAT("display the triheptagonal grid"), (whichShape == '8'), '8');
    dialog::addBoolItem(XLAT("display full floors"), (whichShape == '9'), '9');

    if(cheater || autocheat) dialog::addItem(XLAT("line patterns"), 'l');
    else dialog::addInfo("enable the cheat mode to use line patterns");

    if(!needConfirmation()) dialog::addItem(XLAT("predesigned patterns"), 'r');
    else dialog::addInfo("start a new game to use predesigned patterns");

    dialog::display();
    
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      
      if(among(uni, PAT_EMERALD, PAT_PALACE, PAT_ZEBRA, PAT_DOWN, PAT_FIELD, PAT_COLORING, PAT_SIBLING, PAT_CHESS, PAT_SINGLETYPE)) {
        if(whichPattern == uni) whichPattern = 0;
        else whichPattern = uni;
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

      else if(uni == '6' || uni == '7' || uni == '8' || uni == '9') {
        if(whichShape == uni) whichShape = 0;
        else whichShape = uni;
        }
      else if(uni == 'd') displaycodes = !displaycodes;
      
      else if(uni == 'l' && (cheater || autocheat))
        pushScreen(linepatterns::showMenu);

      else if(uni == 'r' && !needConfirmation()) pushScreen(showPrePattern);
      
      else if(doexiton(sym, uni)) popScreen();
      };
    }
  
  bool compatible(cpatterntype oldp, cpatterntype newp) {
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
    bool nonbitru;
    char whichPattern;
    int subpattern_flags;
    };
  
  struct changeable_pattern {
    string name;
    vector<changeable_pattern_geometry> geometries;
    };
  
  vector<changeable_pattern> cpatterns = {
    {"football", {
      {gNormal, false, 0, 0}, 
      {gSphere, false, 0, 0}, 
      {gEuclid, false, 0, SPF_EXTRASYM}, 
      {gOctagon, false, 0, 0}, 
      {gOctagon, true, PAT_COLORING, SPF_FOOTBALL | SPF_EXTRASYM},
      {g45, false, 0, 0}, 
      {g46, false, 0, SPF_EXTRASYM}, 
      {g47, false, 0, 0},
      {gSmallSphere, false, 0, 0},
      {gSmallSphere, true, PAT_COLORING, SPF_FOOTBALL | SPF_EXTRASYM},
      {gTinySphere, false, 0, SPF_EXTRASYM},
      {gEuclidSquare, false, 0, SPF_EXTRASYM},
      }},
    {"three colors", {
      {gEuclid, false, PAT_COLORING, SPF_SYM0123 | SPF_EXTRASYM},
      {gSmallSphere, false, PAT_COLORING, 0},
      {gSmallSphere, false, PAT_COLORING, SPF_ALTERNATE},
      {gSmallSphere, true, PAT_COLORING, 0},
      {gOctagon, false, PAT_COLORING, SPF_ROT | SPF_EXTRASYM},
      {gOctagon, false, PAT_COLORING, SPF_ROT | SPF_EXTRASYM | SPF_ALTERNATE},
      {gOctagon, true, PAT_COLORING, 0},
      {gEuclidSquare, false, PAT_COLORING, SPF_SYM03 | SPF_EXTRASYM},
      {gEuclidSquare, false, PAT_COLORING, SPF_SYM03 | SPF_EXTRASYM | SPF_ALTERNATE},
      {g46, false, PAT_COLORING, SPF_SYM0123},
      {g46, false, PAT_COLORING, SPF_SYM0123 | SPF_EXTRASYM | SPF_ALTERNATE}
      }},
    {"chessboard", {
      {gEuclidSquare, true, PAT_CHESS, SPF_EXTRASYM}, 
      {g45, true, PAT_CHESS, 0}, 
      {g46, true, PAT_CHESS, 0}, 
      {g47, true, PAT_CHESS, 0}
      }},
    {"single type", {
      {gNormal, true, PAT_SINGLETYPE, 0}, 
      {gSphere, true, PAT_SINGLETYPE, 0}, 
      {gEuclid, false, PAT_SINGLETYPE, 0}, 
      {gOctagon, true, PAT_SINGLETYPE, 0}, 
      {g45, true, PAT_SINGLETYPE, 0}, 
      {g46, true, PAT_SINGLETYPE, 0}, 
      {g47, true, PAT_SINGLETYPE, 0},
      {gSmallSphere, true, PAT_SINGLETYPE, 0},
      {gTinySphere, true, PAT_SINGLETYPE, 0},
      {gEuclidSquare, true, PAT_SINGLETYPE, 0},
      }},
    {"single type+symmetry", {
      {gNormal, true, PAT_SINGLETYPE, SPF_EXTRASYM}, 
      {gSphere, true, PAT_SINGLETYPE, SPF_EXTRASYM}, 
      {gEuclid, false, PAT_SINGLETYPE, SPF_EXTRASYM}, 
      {gOctagon, true, PAT_SINGLETYPE, SPF_EXTRASYM}, 
      {g45, true, PAT_SINGLETYPE, SPF_EXTRASYM}, 
      {g46, true, PAT_SINGLETYPE, SPF_EXTRASYM}, 
      {g47, true, PAT_SINGLETYPE, SPF_EXTRASYM},
      {gSmallSphere, true, PAT_SINGLETYPE, SPF_EXTRASYM},
      {gTinySphere, true, PAT_SINGLETYPE, SPF_EXTRASYM},
      {gEuclidSquare, true, PAT_SINGLETYPE, SPF_EXTRASYM},
      }},
    {"odd/even", {
      {gNormal, false, PAT_SINGLETYPE, SPF_TWOCOL}, 
      {gSphere, false, PAT_SINGLETYPE, SPF_TWOCOL}, 
      {g45, true, PAT_SINGLETYPE, SPF_TWOCOL}, 
      {g47, true, PAT_SINGLETYPE, SPF_TWOCOL}
      }},
    {"large picture", {
      {gNormal, false, PAT_PALACE, SPF_SYM0123},
      {gNormal, true, PAT_PALACE, SPF_SYM0123},
      {gSphere, false, PAT_FIELD, 0},
      {gSphere, true, PAT_FIELD, 0},
      {gElliptic, false, PAT_FIELD, 0},
      {gElliptic, true, PAT_FIELD, 0},
      {gEuclid, false, PAT_PALACE, 0}
      }},
    {"periodic patterns", {
      {gNormal, false, PAT_ZEBRA, SPF_SYM0123 | SPF_ROT},
      {gNormal, false, PAT_PALACE, SPF_SYM0123 | SPF_ROT},
      {gNormal, false, PAT_EMERALD, SPF_SYM0123 | SPF_ROT},
      {g46, true, PAT_COLORING, SPF_SYM0123 | SPF_CHANGEROT},
      {g45, true, PAT_ZEBRA, SPF_SYM0123 | SPF_ROT},
      {g47, true, PAT_ZEBRA, SPF_SYM0123 | SPF_ROT},
      {gOctagon, true, PAT_COLORING, SPF_DOCKS},
      }}
    };
  
  cpatterntype cgroup, old_cgroup;
  
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
        string s = XLAT(ginf[g.geo].name);
        s += bitruncnames[g.nonbitru];
        if(g.subpattern_flags & SPF_ALTERNATE) s += " (alt)";
        if(g.subpattern_flags & SPF_DOCKS) s += " (Docks)";
        if(cgroup == cpZebra) {
          if(g.whichPattern == PAT_PALACE) s += " (Palace)";
          else if(g.whichPattern == PAT_EMERALD) s += " (Emerald)";
          else s += " (Zebra)";
          }
        dialog::addBoolItem(s, geometry == g.geo && nonbitrunc == g.nonbitru && whichPattern == g.whichPattern && subpattern_flags == g.subpattern_flags, 'a'+j);
        }
    bool have_goldberg = (S3 == 3 && among(cgroup, cpFootball, cpThree) && !euclid);
    bool have_variations = (among(cgroup, cpSingle, cpSingleSym) && !euclid);
    if(!(S7&1) && !nonbitrunc) have_goldberg = false; // always start from non-bitruncated
    if(have_goldberg) {
      dialog::addBoolItem(XLAT("Goldberg"), gp::on, 'G');
      dialog::lastItem().value = gp::operation_name();
      }
    if(have_variations) {
      dialog::addBoolItem(XLAT("variations"), gp::on, 'G');
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
        if(g.geo != geometry) { targetgeometry = g.geo; stop_game_and_switch_mode(rg::geometry); }
        if(gp::on) stop_game_and_switch_mode(rg::gp);
        if(g.nonbitru != nonbitrunc) stop_game_and_switch_mode(rg::bitrunc);;
        whichPattern = g.whichPattern;
        subpattern_flags = g.subpattern_flags;
        bool not_restarted = game_active;
        start_game();
        if(not_restarted) REMAP_TEXTURE;
        }
      else if(uni == 'G' && (have_goldberg || have_variations))
        gp::configure();
      else if(doexiton(sym, uni))
        popScreen();
      };
    }
  
  void computeCgroup() {
    cgroup = cpUnknown;
    if(whichPattern == PAT_SINGLETYPE) {
      cgroup = cpSingle;
      return;
      }
    if(archimedean) {
      if(whichPattern == PAT_COLORING) {
        if(subpattern_flags & SPF_FOOTBALL) cgroup = cpFootball;
        else cgroup = cpThree;
        }
      else if(whichPattern == PAT_CHESS && arcm::current.support_chessboard()) cgroup = cpChess;
      else if(whichPattern == 0 && (subpattern_flags & SPF_FOOTBALL) && arcm::current.support_football()) cgroup = cpFootball;
      return;
      }
    for(int i=0; i<isize(cpatterns); i++)
      for(int j=0; j<isize(cpatterns[i].geometries); j++) {
        auto &g = cpatterns[i].geometries[j];
        bool xnonbitrunc = gp::on ? gp_threecolor() : nonbitrunc;
        if(geometry == g.geo && xnonbitrunc == g.nonbitru && whichPattern == g.whichPattern && subpattern_flags == g.subpattern_flags)
          cgroup = cpatterntype(i);
        }
    old_cgroup = cgroup;
    }

  void pushChangeablePatterns() {
    pushScreen(showChangeablePatterns);
    computeCgroup(); 
    }  

  int subcode(cell *c, const patterninfo& si) {
    if(irr::on)
      return irr::cellindex[c] << 8;
    else if(archimedean)
      return arcm::id_of(c->master) << 8;
    else if(!gp::on) return 0;
    else if(c == c->master->c7) return (fixdir(si.dir, c) << 8);
    else return (get_code(gp::get_local_info(c)) << 16) | (fixdir(si.dir, c) << 8);
    }  
  }

bool is_master(cell *c) { 
  if(euclid) return pseudohept(c);
  else return c->master->c7 == c;
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
    {patHorocycles, "horocycles", 0xd060d000},
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
  
  void queuelinef(const hyperpoint& h1, const hyperpoint& h2, int col, int par) {
    if(!elliptic)
      queueline(h1, h2, col, par);
    else {
      ld cros = h1[0]*h2[0] + h1[1]*h2[1] + h1[2]*h2[2];
      using namespace hyperpoint_vec;
      if(cros > 0)
        queueline(h1, h2, col, par),
        queueline(-1*h1, -1*h2, col, par);
      else
        queueline(h1, -1*h2, col, par),
        queueline(-1*h1, h2, col, par);
      }
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
            queueline(tri[i], tri[(i+1)%3], col, 3 + vid.linequality);
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
          
          double x = hexhexdist / 2; // sphere?.3651:euclid?.2611:.2849;
          
          queuelinef(V * ddspin(c,i,-M_PI/S3) * xpush0(x), 
            V * ddspin(c,i,M_PI/S3) * xpush0(x), 
            col, 1 + vid.linequality);
          }
        break;
      
      case patNormal: {
        for(int t=0; t<c->type; t++)
          if(c->move(t) && c->move(t) < c)
          queueline(V * get_corner_position(c, t),
                    V * get_corner_position(c, (t+1)%c->type),
                    col, 1 + vid.linequality);
        break;
        }
      
      case patTrihepta:
        if(pseudohept(c)) for(int t=0; t<c->type; t++)
          queueline(V * get_warp_corner(c, t%c->type),
                    V * get_warp_corner(c, (t+1)%c->type),
                    col, 1 + vid.linequality);
        break;
      
      case patTriNet:
        forCellEx(c2, c) if(c2 > c) if(gmatrix.count(c2)) if(celldist(c) != celldist(c2)) {
          queuelinef(tC0(V), gmatrix[c2]*C0, col, 2 + vid.linequality);
          }
        break;

      case patTriRings:
        forCellIdEx(c2, i, c) {
          if(S3 == 4) c2 = (cellwalker(c, i) + wstep + 1).cpeek();
          if(c2 > c) if(gmatrix.count(c2) && celldist(c) == celldist(c2)) 
            queuelinef(tC0(V), gmatrix[c2]*C0, col, 2 + vid.linequality);
          }
        break;

      case patHepta:
        forCellEx(c2, c) if(c2 > c) if(gmatrix.count(c2) && pseudohept(c) == pseudohept(c2)) 
          queuelinef(tC0(V), gmatrix[c2]*C0, col, 2 + vid.linequality);
        break;

      case patRhomb:
        forCellEx(c2, c) if(c2 > c) if(gmatrix.count(c2) && pseudohept(c) != pseudohept(c2)) 
          queuelinef(tC0(V), gmatrix[c2]*C0, col, 2 + vid.linequality);
        break;
      
      case patPalace: {
        int a = polarb50(c);
        if(pseudohept(c)) for(int i=0; i<7; i++) {
            cell *c1 = createMov(c, (i+3) % 7);
            cell *c2 = createMov(c, (i+4) % 7);
            if(polarb50(c1) != a && polarb50(c2) != a)
                queuelinef(V * ddspin(c,i,M_PI*5/7) * xpush0(tessf/2),
                          V * ddspin(c,i,M_PI*9/7) * xpush0(tessf/2),
                                    col, 1 + vid.linequality);
            }
        break;
        }
      
      case patPalacelike:
        if(pseudohept(c)) for(int i=0; i<7; i++) 
          queuelinef(V * ddspin(c,i,M_PI*5/7) * xpush0(tessf/2),
                    V * ddspin(c,i,M_PI*9/7) * xpush0(tessf/2),
                              col, 1 + vid.linequality);
        break;
      
      case patBigTriangles: {
       if(is_master(c) && !euclid) for(int i=0; i<S7; i++) 
          if(c->master->move(i) && c->master->move(i) < c->master) {
            queuelinef(tC0(V), V*xspinpush0(-2*M_PI*i/S7 - master_to_c7_angle(), tessf), col, 2 + vid.linequality);
            }
        break;
        }
        
      case patBigRings: {
        if(is_master(c) && !euclid) for(int i=0; i<S7; i++) 
          if(c->master->move(i) && c->master->move(i) < c->master && c->master->move(i)->dm4 == c->master->dm4)
            queuelinef(tC0(V), V*xspinpush0(-2*M_PI*i/S7 - master_to_c7_angle(), tessf), col, 2 + vid.linequality);
            // V*xspinpush0((nonbitrunc?M_PI:0) -2*M_PI*i/S7
        break;
        }
        
      case patTree:
        if(is_master(c)) {
          cell *c2 = c->master->move(binarytiling ? 5 : 0)->c7;
          if(gmatrix.count(c2)) queuelinef(tC0(V), gmatrix[c2]*C0, col, 2 + vid.linequality);
          }
        break;
      
      case patHorocycles:
        if(c->master->alt) {
          int d = celldistAlt(c);
          forCellEx(c2, c) if(c2 > c && c2->master->alt && celldistAlt(c2) == d && gmatrix.count(c2))
            queuelinef(tC0(V), gmatrix[c2]*C0, 
              darkena(backcolor ^ 0xFFFFFF, 0, col),
              2 + vid.linequality);
          }
        break;

      case patAltTree:
        if(is_master(c) && !euclid && c->master->alt) {
          for(int i=0; i<S7; i++)
            if(c->master->move(i) && c->master->move(i)->alt == c->master->alt->move(0)) {
              cell *c2 = c->master->move(i)->c7;
              if(gmatrix.count(c2)) queuelinef(tC0(V), gmatrix[c2]*C0, col, 2 + vid.linequality);
              }
          }
        break;
      
      case patVine: {
        if(gp::on) {
          if(c->master->c7 != c) if(gmatrix.count(c->move(0)))
            queuelinef(tC0(V), gmatrix[c->move(0)]*C0, 
              darkena(backcolor ^ 0xFFFFFF, 0, col),
              2 + vid.linequality);
          }
        else if(irr::on) {
          if(c->master->c7 != c) if(gmatrix.count(c->master->c7))
            queuelinef(tC0(V), gmatrix[c->master->c7]*C0, 
              darkena(backcolor ^ 0xFFFFFF, 0, col),
              2 + vid.linequality);
          }
        else {
          int p = emeraldval(c);
          double hdist = hdist0(heptmove[0] * heptmove[2] * C0);
          if(pseudohept(c) && (p/4 == 10 || p/4 == 8))
          for(int i=0; i<S7; i++) if(c->move(i) && emeraldval(c->move(i)) == p-4) {
            queuelinef(tC0(V), V*tC0(heptmove[i]), col, 2 + vid.linequality);
            queuelinef(tC0(V), V*xspinpush0(-i * ALPHA, -hdist/2), col, 2 + vid.linequality);
            }
          }
        break;
        }
      
      case patPower: {
        if(gp::on) {
          for(int i=0; i<S7; i++) if(c->move(i) && c->move(i)->master != c->master && gmatrix.count(c->move(i)))
            queuelinef(tC0(V), gmatrix[c->move(i)]*C0, 
              col,
              1 + vid.linequality);
          }
        else if(archimedean) {
          if(!pseudohept(c)) for(int i=0; i<c->type; i++) if(c->move(i) && c < c->move(i) && !pseudohept(c->move(i)) && gmatrix.count(c->move(i))) 
            queuelinef(tC0(V), gmatrix[c->move(i)]*C0, 
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
                  queuelinef(V * ddspin(c,i,M_PI*5/7) * xpush0(tessf/2),
                            V * ddspin(c,i,M_PI*9/7) * xpush0(tessf/2),
                                      col, 1 + vid.linequality);
              }
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
      dialog::addColorItem(among(patterns[numpat].id, patVine, patPower) && gp::on ? XLAT("Goldberg") + (patterns[numpat].id == patVine ? " " : ""): XLAT(patterns[numpat].lpname), patterns[numpat].color, 'a'+numpat);
  
    dialog::addBreak(50);
    dialog::addBack();
    
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
    whichPattern = 0;
    while(*c) { 
      if(*c >= '0' && *c <= '9') subpattern_flags ^= 1 << (*c - '0'); 
      else if(*c == '@') subpattern_flags ^= 1 << 10; 
      else if(*c == '-') subpattern_flags ^= 1 << 11; 
      else if(*c == '~') subpattern_flags ^= 1 << 12; 
      else whichPattern = *c;
      c++; 
      }
    }

  else if(argis("-wsh")) { shift(); patterns::whichShape = args()[0]; }

  else if(argis("-pal")) {
    PHASEFROM(2); cheat();
    shift(); int id = argi();
    shift(); linepatterns::patterns[id].color |= argi();
    }
  else if(argis("-palrgba")) {
    PHASEFROM(2); cheat();
    shift(); int id = argi();
    shift(); linepatterns::patterns[id].color = arghex();
    }

  else return 1;
  return 0;
  }

auto ah_pattern = addHook(hooks_args, 0, read_pattern_args);
#endif

}
