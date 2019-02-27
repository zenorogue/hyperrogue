// Hyperbolic Rogue -- cells
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// cells the game is played on

namespace hr {

int fix6(int a) { return (a+MODFIXER)%S6; }
int fix7(int a) { return (a+MODFIXER)%S7; }

int dirdiff(int dd, int t) {
  dd %= t;
  if(dd<0) dd += t;
  if(t-dd < dd) dd = t-dd;
  return dd;
  }

int fixdir(int a, cell *c) { a %= c->type; if(a<0) a += c->type; return a; }

int cellcount = 0;

void initcell(cell *c); // from game.cpp

cell *newCell(int type, heptagon *master) {
  cell *c = tailored_alloc<cell> (type);
  c->type = type;
  c->master = master;
  initcell(c);
  return c;
  }

struct cdata {
  int val[4];
  int bits;
  };

// -- hrmap ---

hrmap *currentmap;
vector<hrmap*> allmaps;

// --- auxiliary hyperbolic map for horocycles ---
struct hrmap_alternate : hrmap {
  heptagon *origin;
  hrmap_alternate(heptagon *o) { origin = o; }
  ~hrmap_alternate() { clearfrom(origin); }
  };

hrmap *newAltMap(heptagon *o) { return new hrmap_alternate(o); }
// --- hyperbolic geometry ---

hrmap_hyperbolic::hrmap_hyperbolic() {
  // printf("Creating hyperbolic map: %p\n", this);
  origin = tailored_alloc<heptagon> (S7);
  heptagon& h = *origin;
  h.s = hsOrigin;
  h.emeraldval = a46 ? 0 : 98;
  h.zebraval = 40;
  h.fiftyval = 0;
  h.fieldval = 0;
  h.rval0 = h.rval1 = 0;
  h.cdata = NULL;
  h.alt = NULL;
  h.distance = 0;
  mvar = variation;
  if(0);
  #if CAP_BT
  else if(binarytiling) {
    #if DEBUG_BINARY_TILING
    binary::xcode.clear();
    binary::rxcode.clear();
    binary::xcode[&h] = (1 << 16);
    binary::rxcode[1<<16] = &h;
    #endif
    h.zebraval = 0,
    h.c7 = newCell(DIM == 3 ? 9 : 6, origin);
    }
  #endif
  #if CAP_IRR
  else if(IRREGULAR)
    irr::link_start(origin);
  #endif
  else
    h.c7 = newCell(S7, origin);
  }

// very similar to createMove in heptagon.cpp
cell *createMov(cell *c, int d) {
  if(d<0 || d>= c->type) {
    printf("ERROR createmov\n");
    }

  if(masterless && !c->move(d)) {
    int id = decodeId(c->master);
    for(int dx=-1; dx<=1; dx++)
    for(int dy=-1; dy<=1; dy++)
      euclideanAtCreate(id + pair_to_vec(dx, dy));
    if(!c->move(d)) { 
      println(hlog, "id = ", id, " vec_to_pair(id) = ", vec_to_pair(id), ": failed to create move ", d, " in Euclidean\n");
      exit(0);
      }
    }
  
  if(c->move(d)) return c->move(d);
  #if CAP_IRR
  else if(IRREGULAR) {
    irr::link_cell(c, d);
    }
  #endif
  #if CAP_GP
  else if(GOLDBERG) {
    gp::extend_map(c, d);
    if(!c->move(d)) {
      printf("extend failed to create for %p/%d\n", c, d);
      exit(1);
      }
    }
  #endif
  #if CAP_ARCM
  else if(archimedean && PURE) {
    if(arcm::id_of(c->master) < arcm::current.N * 2) {
      heptspin hs = heptspin(c->master, d) + wstep + 2 + wstep + 1;
      c->c.connect(d, hs.at->c7, hs.spin, hs.mirrored);
      }
    else c->c.connect(d, c, d, false);
    }
  else if(archimedean && DUAL) {
    if(arcm::id_of(c->master) >= arcm::current.N * 2) {
      heptagon *h2 = createStep(c->master, d*2);
      int d1 = c->master->c.spin(d*2);
      c->c.connect(d, h2->c7, d1/2, false);
      }
    else {
      printf("bad connection\n");
      c->c.connect(d,c,d,false);
      }
    }
  #endif
  else if(archimedean || PURE) {
    heptagon *h2 = createStep(c->master, d);
    c->c.connect(d, h2->c7,c->master->c.spin(d),false);
    }
  else if(c == c->master->c7) {
    
    cell *n = newCell(S6, c->master);
    
    heptspin hs(c->master, d, false);
    
    int alt3 = c->type/2;
    int alt4 = alt3+1;
        
    for(int u=0; u<S6; u+=2) {
      if(hs.mirrored && geometry == gSmallElliptic) hs+=1;
      hs.at->c7->c.connect(hs.spin, n, u, hs.mirrored);
      if(hs.mirrored && geometry == gSmallElliptic) hs+=-1;
      hs = hs + alt3 + wstep - alt4;
      }
    extern void verifycell(cell *c);
    verifycell(n);
    }

  else {
    cellwalker cw(c, d, false);
    cellwalker cw2 = cw - 1 + wstep - 1 + wstep - 1;
    c->c.connect(d, cw2);
    }
  return c->move(d);
  }

cell *createMovR(cell *c, int d) {
  d %= MODFIXER; d += MODFIXER; d %= c->type;
  return createMov(c, d);
  }

cell *getMovR(cell *c, int d) {
  d %= MODFIXER; d += MODFIXER; d %= c->type;
  return c->move(d);
  }

void eumerge(cell* c1, int s1, cell *c2, int s2, bool mirror) {
  if(!c2) return;
  c1->move(s1) = c2; c1->c.setspin(s1, s2, mirror);
  c2->move(s2) = c1; c2->c.setspin(s2, s1, mirror);
  }

//  map<pair<eucoord, eucoord>, cell*> euclidean;

euc_pointer euclideanAt(int vec) {
  if(fulltorus) { printf("euclideanAt called\n"); exit(1); }
  hrmap_euclidean* euc = dynamic_cast<hrmap_euclidean*> (currentmap);
  return euc->at(vec);
  }

euc_pointer euclideanAtCreate(int vec) {
  euc_pointer ep = euclideanAt(vec);
  cell*& c = *ep.first;
  if(!c) {
    if(euwrap) {
      int x, y;
      tie(x, y) = vec_to_pair(vec);
      torusconfig::be_canonical(x, y);
      vec = pair_to_vec(x, y);
      }
    c = newCell(8, encodeId(vec));
    // euclideanAt(vec) = c;
    build_euclidean_moves(c, vec, [c,vec] (int delta, int d, int d2) { 
      euc_pointer ep2 = euclideanAt(vec + delta);
      cell* c2 = *ep2.first;
      if(!c2) return;
      // if(ep.second) d = c->c.fix(torusconfig::mobius_dir(c) - d);
      if(ep2.second) d2 = c2->c.fix(torusconfig::mobius_dir(c2) - d2);
      eumerge(c, d, c2, d2, ep2.second);
      });
    }
  return ep;
  }

hookset<hrmap*()> *hooks_newmap;

// initializer (also inits origin from heptagon.cpp)
void initcells() {
  DEBB(DF_INIT, (debugfile,"initcells\n"));
  
  hrmap* res = callhandlers((hrmap*)nullptr, hooks_newmap);
  if(res) currentmap = res;  
  #if CAP_CRYSTAL
  else if(geometry == gCrystal) currentmap = crystal::new_map();
  #endif
  #if CAP_ARCM
  else if(archimedean) currentmap = arcm::new_map();
  #endif
  else if(fulltorus) currentmap = new hrmap_torus;
  else if(euclid && DIM == 3) currentmap = euclid3::new_map();
  else if(euclid) currentmap = new hrmap_euclidean;
  else if(sphere && DIM == 3) currentmap = new sphere3::hrmap_spherical3;
  else if(sphere) currentmap = new hrmap_spherical;
  else if(quotient) currentmap = new quotientspace::hrmap_quotient;
  else currentmap = new hrmap_hyperbolic;
  
  allmaps.push_back(currentmap);

  #if CAP_FIELD
  windmap::create();  
  #endif
  
  // origin->emeraldval = 
  }

void clearcell(cell *c) {
  if(!c) return;
  DEBMEM ( printf("c%d %p\n", c->type, c); )
  for(int t=0; t<c->type; t++) if(c->move(t)) {
    DEBMEM ( printf("mov %p [%p] S%d\n", c->move(t), c->move(t)->move(c->c.spin(t)), c->c.spin(t)); )
    if(c->move(t)->move(c->c.spin(t)) != NULL &&
      c->move(t)->move(c->c.spin(t)) != c) {
        printf("type = %d %d -> %d\n", c->type, t, c->c.spin(t));
        printf("cell error\n");
        exit(1);
        }
    c->move(t)->move(c->c.spin(t)) = NULL;
    }
  DEBMEM ( printf("DEL %p\n", c); )
  tailored_delete(c);
  }

heptagon deletion_marker;

template<class T> void subcell(cell *c, const T& t) {
  if(GOLDBERG) {
    forCellEx(c2, c) if(c2->move(0) == c && c2 != c2->master->c7) {
      subcell(c2, t);
      }
    }
  else if(BITRUNCATED && !archimedean && !binarytiling)
    forCellEx(c2, c) t(c2);
  t(c);
  }

void clearHexes(heptagon *at) {
  if(at->c7 && at->cdata) {
    delete at->cdata;
    at->cdata = NULL;
    }
  if(0);
  #if CAP_IRR
  else if(IRREGULAR) irr::clear_links(at);
  #endif
  else if(at->c7) subcell(at->c7, clearcell);
  }

void unlink_cdata(heptagon *h) {
  if(h->alt && h->c7) {
    if(h->alt->cdata == (cdata*) h)
      h->alt->cdata = NULL;
    }
  }

void clearfrom(heptagon *at) {
  queue<heptagon*> q;
  unlink_cdata(at);
  q.push(at);
  at->alt = &deletion_marker;
//int maxq = 0;
  while(!q.empty()) {
    at = q.front(); 
//  if(q.size() > maxq) maxq = q.size();
    q.pop();
    DEBMEM ( printf("from %p\n", at); )
    if(!at->c7) {
      heptagon *h = (heptagon*) at->cdata;
      if(h) {
        if(h->alt != at) printf("alt error :: h->alt = %p\n", h->alt);
        cell *c = h->c7;
        subcell(c, destroycellcontents);
        h->alt = NULL;
        at->cdata = NULL;
        }
      }
    int edges = at->degree();
    if(binarytiling) edges = at->c7->type;
    for(int i=0; i<edges; i++) if(at->move(i)) {
      if(at->move(i)->alt != &deletion_marker)
        q.push(at->move(i));    
      unlink_cdata(at->move(i));
      at->move(i)->alt = &deletion_marker;
      DEBMEM ( printf("!mov %p [%p]\n", at->move(i), at->move(i)->move[at->c.spin(i)]); )
      if(at->move(i)->move(at->c.spin(i)) != NULL &&
        at->move(i)->move(at->c.spin(i)) != at) {
          printf("hept error\n");
          exit(1);
          }
      at->move(i)->move(at->c.spin(i)) = NULL;
      at->move(i) = NULL;
      }
    clearHexes(at);
    tailored_delete(at);
    }
//printf("maxq = %d\n", maxq);
  }

void verifycell(cell *c) {
  int t = c->type;
  for(int i=0; i<t; i++) {
    cell *c2 = c->move(i);
    if(c2) {
      if(!masterless && BITRUNCATED && c == c->master->c7) verifycell(c2);
      if(c2->move(c->c.spin(i)) && c2->move(c->c.spin(i)) != c) {
        printf("cell error %p:%d [%d] %p:%d [%d]\n", c, i, c->type, c2, c->c.spin(i), c2->type);
        exit(1);
        }
      }
    }
  }

void verifycells(heptagon *at) {
  if(GOLDBERG || IRREGULAR || archimedean) return;
  for(int i=0; i<S7; i++) if(at->move(i) && at->move(i)->move(at->c.spin(i)) && at->move(i)->move(at->c.spin(i)) != at) {
    printf("hexmix error %p [%d s=%d] %p %p\n", at, i, at->c.spin(i), at->move(i), at->move(i)->move(at->c.spin(i)));
    }
  if(!sphere && !quotient) 
    for(int i=0; i<S7; i++) if(at->move(i) && at->c.spin(i) == 0 && at->s != hsOrigin)
      verifycells(at->move(i));
  verifycell(at->c7);
  }

int eudist(int sx, int sy) {
  int z0 = abs(sx);
  int z1 = abs(sy);
  if(a4 && BITRUNCATED)
    return (z0 == z1 && z0 > 0) ? z0+1: max(z0, z1);
  if(a4) return z0 + z1;
  int z2 = abs(sx+sy);
  return max(max(z0,z1), z2);
  }

int eudist(int vec) {
  auto p = vec_to_pair(vec);
  return eudist(p.first, p.second);
  }

int compdist(int dx[]) {
  int mi = dx[0];
  for(int u=0; u<S3; u++) mi = min(mi, dx[u]);
  for(int u=0; u<S3; u++) 
    if(dx[u] > mi+2)
      return -1; // { printf("cycle error!\n"); exit(1); }
  for(int u=0; u<S3; u++) 
    if(dx[u] == mi+2)
      return mi+1;
  int cnt = 0;
  for(int u=0; u<S3; u++) 
    if(dx[u] == mi) cnt++;
  if(cnt < 2)
    return mi+1;
  return mi;
  }

int celldist(cell *c) {
  if(fulltorus) 
    return torusmap()->dists[decodeId(c->master)];
  if(euwrap)
    return torusconfig::cyldist(decodeId(c->master), 0);
  if(masterless)
    return eudist(decodeId(c->master));
  if(sphere || binarytiling || DIM == 3 || geometry == gCrystal) return celldistance(c, currentmap->gamestart());
  #if CAP_IRR
  if(IRREGULAR) return irr::celldist(c, false);
  #endif
  if(archimedean || ctof(c)) return c->master->distance;
  #if CAP_GP
  if(GOLDBERG) return gp::compute_dist(c, celldist);
  #endif
  int dx[MAX_S3];
  for(int u=0; u<S3; u++)
    dx[u] = createMov(c, u+u)->master->distance;
  return compdist(dx);
  }

#define ALTDIST_BOUNDARY 99999
#define ALTDIST_UNKNOWN 99998

#define ALTDIST_ERROR 90000

int celldistAlt(cell *c) {
  if(masterless) {
    if(fulltorus) return celldist(c);
    if(euwrap) return cylinder_alt(c);
    int x, y;
    tie(x,y) = vec_to_pair(decodeId(c->master));
    return euclidAlt(x, y);
    }
  #if CAP_BT
  if(binarytiling) return c->master->distance + (specialland == laCamelot && !tactic::on? 30 : 0);
  #endif
  #if CAP_CRYSTAL
  if(geometry == gCrystal) 
    return crystal::dist_alt(c);
  #endif
  if(sphere || quotient) {
    return celldist(c) - 3;
    }
  #if MAXDIM == 4
  if(euclid && DIM == 3) return euclid3::dist_alt(c);
  #endif
  if(!c->master->alt) return 0;
  #if CAP_IRR
  if(IRREGULAR) return irr::celldist(c, true);
  #endif
  if(ctof(c)) return c->master->alt->distance;
  #if CAP_GP
  if(GOLDBERG) return gp::compute_dist(c, celldistAlt);
  #endif
  int dx[MAX_S3]; dx[0] = 0;
  for(int u=0; u<S3; u++) if(createMov(c, u+u)->master->alt == NULL)
    return ALTDIST_UNKNOWN;
  for(int u=0; u<S3; u++)
    dx[u] = createMov(c, u+u)->master->alt->distance;
  // return compdist(dx); -> not OK because of boundary conditions
  int mi = dx[0];
  for(int i=1; i<S3; i++) mi = min(mi, dx[i]);
  for(int i=0; i<S3; i++) if(dx[i] > mi+2)
    return ALTDIST_BOUNDARY; // { printf("cycle error!\n"); exit(1); }
  for(int i=0; i<S3; i++) if(dx[i] == mi+2)
    return mi+1;
  return mi;
  }

int dirfromto(cell *cfrom, cell *cto) {
  for(int i=0; i<cfrom->type; i++) if(cfrom->move(i) == cto) return i;
  return -1;
  }

#define RPV_MODULO 5

#define RPV_RAND 0
#define RPV_ZEBRA 1
#define RPV_EMERALD 2
#define RPV_PALACE 3
#define RPV_CYCLE 4

int getCdata(cell *c, int j);

// x mod 5 = pattern type
// x mod (powers of 2) = pattern type specific
// (x/5) mod 15 = picture for drawing floors
// x mod 7 = chance of pattern-specific pic
// whole = randomization

bool randpattern(cell *c, int rval) {
  int i, sw=0;
  switch(rval%5) {
    case 0:
      if(rval&1) {
        return hrandpos() < rval;
        }
      else {
        int cd = getCdata(c, 0);
        return !((cd/(((rval/2)&15)+1))&1);
        }
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
      i >>= 2; i--;
      if(rval & (16<<i)) sw^=1;
      return sw;
    case 3:
      if(polara50(c)) { if(rval&4) sw^=1; }
      if(polarb50(c)) { if(rval&8) sw^=1; }
      i = fiftyval049(c); i += 6; i /= 7;
      if(rval & (16<<i)) sw^=1;
      return sw;
    case 4:
      i = (rval&3);
      if(i == 1 && (celldist(c)&1)) sw ^= 1;
      if(i == 2 && (celldist(c)&2)) sw ^= 1;
      if(i == 3 && ((celldist(c)/3)&1)) sw ^= 1;
      if(rval & (4<<towerval(c, celldist))) sw ^= 1;
      return sw;
    }
  return 0;
  }

string describeRPM(eLand l) {
  int rval = randompattern[l];
  switch(rval%5) {
    case 0:
      if(rval&1)
        return "R:"+its(rval/(HRANDMAX/100))+"%";
      else
        return "Landscape/"+its(((rval/2)&15)+1);
    case 1:
      return "Z/"+its((rval>>2)&3)+"/"+its((rval>>4)&15);
    case 2:
      return "E/"+its((rval>>2)&3)+"/"+its((rval>>4)&2047);
    case 3:
      return "P/"+its((rval>>2)&3)+"/"+its((rval>>4)&255);
    case 4:
      return "C/"+its(rval&3)+"/"+its((rval>>2)&65535);
    }
  return "?";
  }

int randpatternCode(cell *c, int rval) {
  switch(rval % RPV_MODULO) {
    case 1:
      return zebra40(c);
    case 2:
      return emeraldval(c);
    case 3:
      return fiftyval049(c) + (polara50(c)?50:0) + (polarb50(c)?1000:0);
    case 4:
      return towerval(c, celldist) * 6 + celldist(c) % 6;
    }
  return 0;  
  }

#define RANDITER 31

char rpm_memoize[3][256][RANDITER+1];

void clearMemoRPM() {
  for(int a=0; a<3; a++) for(int b=0; b<256; b++) for(int i=0; i<RANDITER+1; i++)
    rpm_memoize[a][b][i] = 2;
  }

bool randpatternMajority(cell *c, int ival, int iterations) {
  int rval = 0;
  if(ival == 0) rval = randompattern[laCaves];
  if(ival == 1) rval = randompattern[laLivefjord];
  if(ival == 2) rval = randompattern[laEmerald];
  if(rval%RPV_MODULO == RPV_RAND) return randpattern(c, rval);
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

map<heptagon*, int> spins;

#define RVAL_MASK 0x10000000
#define DATA_MASK 0x20000000

cdata orig_cdata;

void affect(cdata& d, short rv, signed char signum) {
  if(rv&1) d.val[0]+=signum; else d.val[0]-=signum;
  if(rv&2) d.val[1]+=signum; else d.val[1]-=signum;
  if(rv&4) d.val[2]+=signum; else d.val[2]-=signum;
  if(rv&8) d.val[3]+=signum; else d.val[3]-=signum;
  int id = (rv>>4) & 63;
  if(id < 32) 
    d.bits ^= (1 << id);
  }

void setHeptagonRval(heptagon *h) {
  if(!(h->rval0 || h->rval1)) {
    h->rval0 = hrand(0x10000);
    h->rval1 = hrand(0x10000);
    }
  }

cdata *getHeptagonCdata(heptagon *h) {
  if(h->cdata) return h->cdata;

  if(sphere || quotient) h = currentmap->gamestart()->master;

  if(h == currentmap->gamestart()->master) {
    h->cdata = new cdata(orig_cdata);
    for(int& v: h->cdata->val) v = 0;
    h->cdata->bits = reptilecheat ? (1 << 21) - 1 : 0;
    if(yendor::on && specialland == laVariant) h->cdata->bits |= (1 << 8) | (1 << 9) | (1 << 12);
    return h->cdata;
    }
  
  cdata mydata = *getHeptagonCdata(h->move(0));

  for(int di=3; di<5; di++) {
    heptspin hs(h, di, false);
    int signum = +1;
    while(true) {
      heptspin hstab[15];
      hstab[7] = hs;
      
      for(int i=8; i<12; i++) {
        hstab[i] = hstab[i-1];
        hstab[i] += ((i&1) ? 4 : 3);
        hstab[i] += wstep;
        hstab[i] += ((i&1) ? 3 : 4);
        }

      for(int i=6; i>=3; i--) {
        hstab[i] = hstab[i+1];
        hstab[i] += ((i&1) ? 3 : 4);
        hstab[i] += wstep;
        hstab[i] += ((i&1) ? 4 : 3);
        }
      
      if(hstab[3].at->distance < hstab[7].at->distance) {
        hs = hstab[3]; continue;
        }

      if(hstab[11].at->distance < hstab[7].at->distance) {
        hs = hstab[11]; continue;
        }
      
      int jj = 7;
      for(int k=3; k<12; k++) if(hstab[k].at->distance < hstab[jj].at->distance) jj = k;
      
      int ties = 0, tiespos = 0;
      for(int k=3; k<12; k++) if(hstab[k].at->distance == hstab[jj].at->distance) 
        ties++, tiespos += (k-jj);
        
      // printf("ties=%d tiespos=%d jj=%d\n", ties, tiespos, jj);
      if(ties == 2) jj += tiespos/2;
      
      if(jj&1) signum = -1;
      hs = hstab[jj];
      
      break;
      }
    hs = hs + 3 + wstep;
    setHeptagonRval(hs.at);
    
    affect(mydata, hs.spin ? hs.at->rval0 : hs.at->rval1, signum);

    /* if(!(spins[hs.at] & hs.spin)) {
      spins[hs.at] |= (1<<hs.spin);
      int t = 0;
      for(int k=0; k<7; k++) if(spins[hs.at] & (1<<k)) t++;
      static bool wast[256];
      if(!wast[spins[hs.at]]) {
        printf("%p %4x\n", hs.at, spins[hs.at]);
        wast[spins[hs.at]] = true;
        }
      } */
    }

  return h->cdata = new cdata(mydata);
  }

cdata *getEuclidCdata(int h) {

  if(euwrap) { // fix cylinder?
    static cdata xx;
    return &xx;
    }
  
  int x, y;
  hrmap_euclidean* euc = dynamic_cast<hrmap_euclidean*> (currentmap);
  if(euc->eucdata.count(h)) return &(euc->eucdata[h]);
  
  tie(x,y) = vec_to_pair(h);

  if(x == 0 && y == 0) {
    cdata xx;
    for(int i=0; i<4; i++) xx.val[i] = 0;
    xx.bits = 0;
    return &(euc->eucdata[h] = xx);
    }
  int ord = 1, bid = 0;
  while(!((x|y)&ord)) ord <<= 1, bid++;
  
  for(int k=0; k<3; k++) {
    int x1 = x + (k<2 ? ord : 0);
    int y1 = y - (k>0 ? ord : 0);
    if((x1&ord) || (y1&ord)) continue;
    int x2 = x - (k<2 ? ord : 0);
    int y2 = y + (k>0 ? ord : 0);

    cdata *d1 = getEuclidCdata(pair_to_vec(x1,y1));
    cdata *d2 = getEuclidCdata(pair_to_vec(x2,y2));
    cdata xx;
    double disp = pow(2, bid/2.) * 6;
    
    for(int i=0; i<4; i++) {
      double dv = (d1->val[i] + d2->val[i])/2 + (hrand(1000) - hrand(1000))/1000. * disp;
      xx.val[i] = floor(dv);
      if(hrand(1000) / 1000. < dv - floor(dv)) xx.val[i]++;
      }
    xx.bits = 0;

    for(int b=0; b<32; b++) {
      bool gbit = ((hrand(2)?d1:d2)->bits >> b) & 1;
      int flipchance = (1<<bid);
      if(flipchance > 512) flipchance = 512;
      if(hrand(1024) < flipchance) gbit = !gbit;
      if(gbit) xx.bits |= (1<<b);
      }
    
    return &(euc->eucdata[h] = xx);
    }
  
  // impossible!
  return NULL;
  }

int getCdata(cell *c, int j) {
  if(masterless) return getEuclidCdata(decodeId(c->master))->val[j];
  else if(geometry) return 0;
  else if(ctof(c)) return getHeptagonCdata(c->master)->val[j]*3;
  else {
    int jj = 0;
    auto ar = gp::get_masters(c);
    for(int k=0; k<3; k++)
      jj += getHeptagonCdata(ar[k])->val[j];
    return jj;
    }
  }

int getBits(cell *c) {
  if(masterless) return getEuclidCdata(decodeId(c->master))->bits;
  else if(geometry) return 0;
  else if(c->type != 6) return getHeptagonCdata(c->master)->bits;
  else {
    auto ar = gp::get_masters(c);
    int b0 = getHeptagonCdata(ar[0])->bits;
    int b1 = getHeptagonCdata(ar[1])->bits;
    int b2 = getHeptagonCdata(ar[2])->bits;
    return (b0 & b1) | (b1 & b2) | (b2 & b0);
    }
  }

cell *heptatdir(cell *c, int d) {
  if(d&1) {
    cell *c2 = createMov(c, d);
    int s = c->c.spin(d);
    s += 3; s %= 6;
    return createMov(c2, s);
    }
  else return createMov(c, d);
  }

int heptdistance(heptagon *h1, heptagon *h2) {
  // very rough distance
  int d = 0;
  #if CAP_CRYSTAL
  if(geometry == gCrystal) return crystal::space_distance(h1->c7, h2->c7);
  #endif
  while(true) {
    if(h1 == h2) return d;
    for(int i=0; i<S7; i++) if(h1->move(i) == h2) return d + 1;
    int d1 = h1->distance, d2 = h2->distance;
    if(d1 >= d2) d++, h1 = createStep(h1, binarytiling ? 5 : 0);
    if(d2 >  d1) d++, h2 = createStep(h2, binarytiling ? 5 : 0);
    }
  }

int heptdistance(cell *c1, cell *c2) {
  #if CAP_CRYSTAL
  if(geometry == gCrystal) return crystal::space_distance(c1, c2);
  #endif
  if(!hyperbolic || quotient || DIM == 3) return celldistance(c1, c2);
  else return heptdistance(c1->master, c2->master);
  }

map<pair<cell*, cell*>, int> saved_distances;

int celldistance(cell *c1, cell *c2) {
  
  if((masterless) && (euclid6 || (euclid4 && PURE))) {
    if(!euwrap)
      return eudist(decodeId(c1->master) - decodeId(c2->master)); // fix cylinder
    else if(euwrap && torusconfig::torus_mode == 0) 
      return torusmap()->dists[torusconfig::vec_to_id(decodeId(c1->master)-decodeId(c2->master))];
    else if(euwrap && !fulltorus)
      return torusconfig::cyldist(decodeId(c1->master), decodeId(c2->master));
    }
  
  #if CAP_FIELD
  if(geometry == gFieldQuotient && !GOLDBERG)
    return currfp.getdist(fieldpattern::fieldval(c1), fieldpattern::fieldval(c2));
  #endif
  
  if(bounded) {
    
    if(saved_distances.count(make_pair(c1,c2)))
      return saved_distances[make_pair(c1,c2)];

    celllister cl(c1, 100, 100000000, NULL);
    for(int i=0; i<isize(cl.lst); i++)
      saved_distances[make_pair(c1, cl.lst[i])] = cl.dists[i];

    if(saved_distances.count(make_pair(c1,c2)))
      return saved_distances[make_pair(c1,c2)];

    return 64;
    }
  
  #if CAP_CRYSTAL
  if(geometry == gCrystal) return crystal::precise_distance(c1, c2);
  #endif
  
  if(masterless || archimedean || quotient) {
    
    if(saved_distances.count(make_pair(c1,c2)))
      return saved_distances[make_pair(c1,c2)];
      
    if(isize(saved_distances) > 1000000) saved_distances.clear();

    celllister cl(c1, 64, 1000, c2);

    for(int i=0; i<isize(cl.lst); i++)
      saved_distances[make_pair(c1, cl.lst[i])] = cl.dists[i];

    if(saved_distances.count(make_pair(c1,c2)))
      return saved_distances[make_pair(c1,c2)];

    return 64;
    }

  if(binarytiling && DIM == 3) 
    return binary::celldistance3(c1, c2);

  if(euclid && DIM == 3) 
    return euclid3::celldistance(c1, c2);

  return hyperbolic_celldistance(c1, c2);
  }

vector<cell*> build_shortest_path(cell *c1, cell *c2) {
  #if CAP_CRYSTAL
  if(geometry == gCrystal) return crystal::build_shortest_path(c1, c2);
  #endif
  vector<cell*> p;
  if(euclid && DIM == 2) {
    using namespace hyperpoint_vec;
    p.push_back(c1);
    hyperpoint h = tC0(calc_relative_matrix(c2, c1, C0)) - C0;
    cell *x = c1;
    hyperpoint h1 = C0; 
    int d = celldistance(c1, c2);
    for(int i=0; i<=d * 10; i++) {
      h1 += h / d / 10.;
      virtualRebase(x, h1,  true);
      if(x != p.back()) p.push_back(x);
      }
    if(isize(p) != d + 1)
      println(hlog, "warning: path size ", isize(p), " should be ", d+1);
    }
  else if(c2 == currentmap->gamestart()) {
    while(c1 != c2) {
      p.push_back(c1);
      forCellCM(c, c1) if(celldist(c) < celldist(c1)) { c1 = c; goto next1; }
      println(hlog, "could not build_shortest_path"); exit(1);
      next1: ;
      }
    p.push_back(c1);
    }
  else if(c1 == currentmap->gamestart()) {
    p = build_shortest_path(c2, c1);
    reverse(p.begin(), p.end());
    }
  else {
    while(c1 != c2) {
      p.push_back(c1);
      forCellCM(c, c1) if(celldistance(c, c2) < celldistance(c1, c2)) { c1 = c; goto next; }
      println(hlog, "could not build_shortest_path"); exit(1);
      next: ;
      }
    p.push_back(c1);
    }
  return p;
  }

void clearCellMemory() {
  for(int i=0; i<isize(allmaps); i++) 
    if(allmaps[i])
      delete allmaps[i];
  allmaps.clear();
  last_cleared = NULL;
  saved_distances.clear();
  pd_from = NULL;
  }

auto cellhooks = addHook(clearmemory, 500, clearCellMemory);

}
