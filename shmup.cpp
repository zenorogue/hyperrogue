// Hyperbolic Rogue -- shoot'em up mode
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file shmup.cpp
 *  \brief shoot'em up mode
 */

#include "hyper.h"
namespace hr {

// joysticks for controlling the mobile shmup mode
EX namespace shmupballs {
  EX int xmove, xfire, yb, rad;

  EX void calc() {      
    int rr = int(realradius());
    rad = int(rr * (vid.mobilecompasssize ? vid.mobilecompasssize : 14) / 100);
    xmove = max(current_display->xcenter - rr - rad, rad);
    xfire = min(current_display->xcenter + rr + rad, vid.xres - rad);
    yb = current_display->ycenter + rr - rad;
    }
EX }

ld sqdist(shiftpoint a, shiftpoint b) {
  if(gproduct) return pow(hdist(a, b), 2);
  else return intval(a.h, unshift(b, a.shift));
  }

#if HDR
#define SCALE cgi.scalefactor
#define SCALE2 (SCALE*SCALE)
#endif

EX namespace shmup {

#if HDR
struct monster {
  eMonster type;
  cell *base;      ///< on which base cell this monster currently is
  cell *torigin;   ///< tortoises: origin, butterflies: last position
  transmatrix at;  ///< position relative to base
  shiftmatrix pat; ///< position relative to current projection center
  transmatrix ori; ///< orientation for the product geometry
  eMonster stk;
  bool dead;
  bool notpushed;
  bool inBoat;
  bool no_targetting;
  monster *parent; ///< who shot this missile
  int nextshot;    ///< when will it be able to shot (players/flailers)
  int pid;         ///< player ID
  int hitpoints;   ///< hitpoints; or time elapsed in Asteroids
  int stunoff;     ///< when does the stun end
  int blowoff;     ///< when does the blow end
  int fragoff;     ///< when does the frag end in PvP
  double swordangle; ///< sword angle wrt at
  double vel;        ///< velocity, for flail balls
  double footphase;
  bool isVirtual;    ///< off the screen: gmatrix is unknown, and pat equals at
  hyperpoint inertia;///< for frictionless lands
  
  int refs;         ///< +1 for every reference (parent, lists of active monsters)

  int split_owner;  ///< in splitscreen mode, which player handles this
  int split_tick;   ///< in which tick was split_owner computed

  void reset() {
    nextshot = 0;
    stunoff = 0; blowoff = 0; fragoff = 0; footphase = 0;
    inertia = Hypc; ori = Id; vel = 0;
    swordangle = 0;
    }

  monster() {
    reset();
    refs = 1; split_tick = -1; split_owner = -1;
    no_targetting = false;
    dead = false; inBoat = false; parent = NULL;
    }

  eMonster get_parenttype() { return parent ? parent->type : moNone; }

  void store();
    
  void findpat();

  cell *findbase(const shiftmatrix& T, int maxsteps);

  void rebasePat(const shiftmatrix& new_pat, cell *tgt);
  
  void remove_reference() {
    refs--;
    if(!refs) {
      if(parent) parent->remove_reference();
      delete this;
      }
    }
  
  void set_parent(monster *par) {
    if(parent) parent->remove_reference();
    parent = par;
    parent->refs++;
    }

  };  
#endif

using namespace multi;

eItem targetRangedOrbKey(enum orbAction a);
eItem keyresult[MAXPLAYER];

ld fabsl(ld x) { return x>0?x:-x; }

EX bool on = false;
EX bool delayed_safety = false;
EX eLand delayed_safety_land;

bool lastdead = false;

EX multimap<cell*, monster*> monstersAt;

#if HDR
typedef multimap<cell*, monster*>::iterator mit;
#endif

vector<monster*> active, nonvirtual, additional;

cell *findbaseAround(shiftpoint p, cell *around, int maxsteps) {

  if(fake::split()) {
    auto p0 = inverse_shift(ggmatrix(around), p);
    virtualRebase(around, p0);
    return around;
    }

  cell *best = around;
  shiftmatrix T = ggmatrix(around);
  horo_distance d0(p, T);

  for(int k=0; k<maxsteps; k++) {
    for(int i=0; i<around->type; i++) {
      cell *c2 = around->move(i);
      if(c2) {
        shiftmatrix U = ggmatrix(c2);
        horo_distance d1(p, U);
        if(d1 < d0) { best = c2; d0 = d1; }
        }
      }
    if(best == around) break;
    around = best;
    }
  return around;
  }

cell *findbaseAround(const shiftmatrix& H, cell *around, int maxsteps) {
  return findbaseAround(H * tile_center(), around, maxsteps);
  }

/* double distance(hyperpoint h) {
  h = lspintox(h) * h;
  return asinh(h[2]);
  } */

void monster::store() {
  monstersAt.insert(make_pair(base, this));
  }

void monster::findpat() {
  isVirtual = !gmatrix.count(base) || invalid_matrix(gmatrix[base].T);
  if(!isVirtual) pat = gmatrix[base] * at;
  else pat = shiftless(at);
  }

cell *monster::findbase(const shiftmatrix& T, int maxsteps) {
  if(isVirtual) {
    cell *c = base;
    auto cT = T.T;
    virtualRebase(c, cT);
    return c;
    }
  else return findbaseAround(T, base, maxsteps);
  }

/** fix the matrix, including the appropriate fixes for nonisotropic, embedded_plane, and elliptic space */
void full_fix(transmatrix& T) {
  if(embedded_plane) {
    if(geom3::sph_in_low()) {
      for(int i=0; i<4; i++) T[i][3] = 0, T[3][i] = 0;
      T[3][3] = 1;
      fixmatrix(T);
      }
    else if(geom3::same_in_same()) {
      for(int i=0; i<4; i++) T[i][2] = 0, T[2][i] = 0;
      T[2][2] = 1;
      fixmatrix(T);
      }
    else if(gproduct) {
      fixmatrix(T);
      }
    else {
      hyperpoint h = tC0(T);
      transmatrix rot = iso_inverse(map_relative_push(h)) * T;
      fix_rotation(rot);
      if(geom3::hyp_in_solnih()) h[0] = 0;
      if(geom3::euc_in_nil()) h[1] = 0;
      T = map_relative_push(h) * rot;
      fixmatrix(T);
      }
    }
  else if(nonisotropic) {
    hyperpoint h = tC0(T);
    transmatrix rot = gpushxto0(h) * T;
    fix_rotation(rot);
    T = rgpushxto0(h) * rot;
    }
  else
    fixmatrix(T);
  fixelliptic(T);
  }

void monster::rebasePat(const shiftmatrix& new_pat, cell *c2) {
  if(isVirtual) {
    at = new_pat.T;
    virtualRebase(this);
    full_fix(at);
    pat = shiftless(at);
    if(multi::players == 1 && this == shmup::pc[0])
      current_display->which_copy = back_to_view(ggmatrix(base));
    return;
    }
  if(quotient || fake::split()) {
    at = inverse_shift(gmatrix[base], new_pat);
    transmatrix old_at = at;
    virtualRebase(this);
    full_fix(at);
    if(base != c2) {
      if(fake::split()) println(hlog, "fake error");
      else {
        auto T = calc_relative_matrix(c2, base, tC0(at));
        base = c2;
        at = iso_inverse(T) * at;
        }
      }
    if(multi::players == 1 && this == shmup::pc[0] && !eqmatrix(old_at, at))
      current_display->which_copy = current_display->which_copy * old_at * iso_inverse(at);
    return;
    }
  if(multi::players == 1 && this == shmup::pc[0])
    current_display->which_copy = current_display->which_copy * inverse_shift(gmatrix[base], gmatrix[c2]);
  pat = new_pat;
  base = c2;
  at = inverse_shift(gmatrix[c2], pat);
  full_fix(at);
  }

bool trackroute(monster *m, shiftmatrix goal, double spd) {
  cell *c = m->base;
  
  // queuepoly(goal, shGrail, 0xFFFFFFC0);

  transmatrix mat = inverse_shift(m->pat, goal);
  
  transmatrix mat2 = lspintox(mat*C0) * mat;
  
  double d = 0, dist = asinh(mat2[0][2]);

  while(d < dist) {
    d += spd;
    shiftmatrix nat = m->pat * lrspintox(mat * C0) * lxpush(d);

    // queuepoly(nat, cgi.shKnife, 0xFFFFFFC0);

    cell *c2 = findbaseAround(nat, c, 1);
    if(c2 != c && !passable_for(m->type, c2, c, P_CHAIN | P_ONPLAYER)) {
      return false;
      }
    c = c2;
    }
  return true;
  }

EX monster *pc[MAXPLAYER], *mousetarget, *lmousetarget;

EX int curtime, nextmove, nextdragon;

bool isBullet(monster *m) { 
  return isBulletType(m->type);
  }
bool isPlayer(monster *m) { return m->type == moPlayer; }
bool isMonster(monster *m) { return m->type != moPlayer && m->type != moBullet; }

EX hookset<bool(shmup::monster*)> hooks_kill;

void killMonster(monster* m, eMonster who_kills, flagtype flags = 0) {
  int tk = tkills();
  if(callhandlers(false, hooks_kill, m)) return;
  if(m->dead) return;
  m->dead = true;
  if(isPlayer(m) && m->fragoff < curtime) {
    if(multi::cpid == m->pid)
      multi::suicides[multi::cpid]++;
    else if(multi::cpid >= 0)
      multi::pkills[multi::cpid]++;
    }
  if(isBullet(m) || isPlayer(m)) return;
  m->stk = m->base->monst;
  if(m->inBoat && isWatery(m->base)) {
    m->base->wall = waBoat;
    m->base->mondir = 0;
    m->inBoat = false;
    }
  else if(m->inBoat && m->base->wall == waNone && (
    (m->base->land == laOcean || m->base->land == laLivefjord))) {
    m->base->wall = waStrandedBoat;
    m->base->mondir = 0;
    m->inBoat = false;
    }
  m->base->monst = m->type;
  killMonster(m->base, who_kills, flags);
  m->base->monst = m->stk;
  if(multi::cpid >= 0)
    multi::kills[multi::cpid] += tkills() - tk;
  }

EX void pushmonsters() {
  for(monster *m: nonvirtual) {
    m->notpushed = isPlayer(m) || m->dead || (m->base->monst && m->base->monst != m->type);
    if(!m->notpushed) {
      m->stk = m->base->monst;
      m->base->monst = m->type;
      }
    }
  }

EX void popmonsters() {
  for(int i=isize(nonvirtual)-1; i>=0; i--) {
    monster *m = nonvirtual[i];
    if(!m->notpushed) {
      if(m->type == m->base->monst)
        m->base->monst = m->stk;
      else {
        m->dead = true; // already killed
        // also kill all the other monsters pushed there
        for(int j=0; j<i; j++) {
          monster *m2 = active[j];
          if(m2->base == m->base && !m2->notpushed) 
            killMonster(m2, moNone);
          }
        }
      }
    }
  }

EX void degradeDemons() {
  for(monster* m: nonvirtual) {
    if(m->type == moGreater) m->type = moLesser;
    if(m->stk == moGreater) m->stk = moLesser;
    }
  }

// we need these for the Mimics!
EX double playerturn[MAXPLAYER], playergo[MAXPLAYER], playerstrafe[MAXPLAYER], playerturny[MAXPLAYER], playergoturn[MAXPLAYER], godir[MAXPLAYER];
EX transmatrix playersmallspin[MAXPLAYER];
bool playerfire[MAXPLAYER];

void awakenMimics(monster *m, cell *c2) {
  for(auto& mi: mirror::mirrors) {
    cell *c = mi.second.at;

    transmatrix mirrortrans = Id;    
    if(mi.second.mirrored) mirrortrans[0][0] = -1;

    if(!gmatrix.count(c)) continue;
    monster *m2 = new monster;
    m2->base = c;
    
    if(isBullet(m)) {
      m2->type = m->type;
      m2->vel = m->vel;
      m2->set_parent(m->parent);
      m2->pid = m->pid;
      }
    else
      m2->type = moMimic;
    
    hyperpoint H = inverse_shift(gmatrix[c2], tC0(gmatrix[c]));
    
    transmatrix xfer = rgpushxto0(H);

    if(mi.second.mirrored) {
      hyperpoint H2 = lspintox(H) * H;
      xfer = lrspintox(H) * rpushxto0(H2) * mirrortrans * lspintox(H);
      }

    m2->pat = gmatrix[c2] * xfer * inverse_shift(gmatrix[c2], m->pat);
      
    m2->at = inverse_shift(gmatrix[c], m2->pat);
    m2->pid = cpid;
    
    additional.push_back(m2);

    // if you don't understand it, don't worry,
    // I don't understand it either
    }
  mirror::mirrors.clear();
  }

int visibleAt;

EX void visibleFor(int t) {
  visibleAt = max(visibleAt, curtime + t);
  }

ld bullet_velocity(eMonster t) {
  switch(t) {
    case moBullet:
      return 1/300.;
    case moFireball:
      return 1/500.;
    case moCrushball:
      return 1/1000.;
    case moAirball:
      return 1/200.;
    case moArrowTrap:
      return 1/200.;
    case moTongue:
      return 1/1500.;
    default:
      return 1/300.;
    }
  }

int frontdir() { return WDIM == 2 ? 0 : 2; }

/** cannot hit yourself during first 100ms after shooting a bullet */
const int bullet_time = 300;

void shootBullet(monster *m) {
  monster* bullet = new monster;
  bullet->base = m->base;
  bullet->at = m->at;
  if(WDIM == 3) bullet->at = bullet->at * cpush(2, 0.15 * SCALE);
  if(gproduct) bullet->ori = m->ori;
  bullet->type = moBullet;
  bullet->set_parent(m);
  bullet->pid = m->pid;
  bullet->inertia = m->inertia;
  bullet->inertia[frontdir()] += bullet_velocity(m->type) * SCALE;
  bullet->hitpoints = 0;
  bullet->fragoff = curtime + bullet_time;

  additional.push_back(bullet);
  
  eItem orbdir[8] = {
    itNone, itOrbSide1, itOrbThorns, itOrbSide2, itOrbSide3, itOrbSide2, itOrbThorns, itOrbSide1
    };
  
  for(int i=1; i<8; i++) if(markOrb(orbdir[i])) {
    monster* bullet = new monster;
    bullet->base = m->base;
    bullet->at = m->at * cspin(0, WDIM-1, TAU * i/8);
    if(gproduct) bullet->ori = m->ori;
    if(WDIM == 3) bullet->at = bullet->at * cpush(2, 0.15 * SCALE);
    bullet->type = moBullet;
    bullet->set_parent(m);
    bullet->pid = m->pid;
    bullet->hitpoints = 0;
    bullet->fragoff = ticks + bullet_time;
    bullet->inertia = cspin(0, WDIM-1, -TAU * i/8) * m->inertia;
    bullet->inertia[frontdir()] += bullet_velocity(m->type) * SCALE;
    additional.push_back(bullet);
    }
  }

EX void killThePlayer(eMonster m) {
  if(cpid >= 0 && cpid < MAXPLAYER && pc[cpid])
    pc[cpid]->dead = true;
  }

monster *playerCrash(monster *who, shiftpoint where) {
  if(who->isVirtual) return NULL;
  for(int j=0; j<players; j++) if(pc[j] && pc[j]!=who) {
    if(pc[j]->isVirtual) continue;
    if(!gmatrix.count(pc[j]->base)) continue;
    double d = sqdist(pc[j]->pat*C0, where);
    /* crash into another player -- not taken into account in racing */
    if(d < 0.1 * SCALE2 && !racing::on) return pc[j];
    /* too far away -- irrelevant in split_screen */
    if(!split_screen && (d > 100 || (WDIM == 3 && hdist(tC0(pc[j]->pat), where) > sightranges[geometry]/2)))
      return pc[j];
    }
  return NULL;
  }

void oceanCurrents(shiftmatrix& nat, monster *m, int delta) {
  cell *c = m->base;
  if(c->land == laWhirlpool) {
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->move(i);
      if(!c2 || !gmatrix.count(c2)) continue;
      
      double spd = 0;
      
      if(celldistAlt(c2) < celldistAlt(c)) 
        spd = SCALE * delta / 3000.;
      else if(c2 == whirlpool::get(c, 1))
        spd = SCALE * delta / 900.;
        
      if(spd) {
        shiftpoint goal = tC0(gmatrix[c2]);

        // transmatrix t = lspintox(H) * lxpush(delta/300.) * lrspintox(H);

        hyperpoint H = inverse_shift(m->pat, goal);
        nat = nat * lrspintox(H);
        nat = nat * lxpush(spd);
        nat = nat * lspintox(H);
        }
      }
    }
  }

bool airCurrents(shiftmatrix& nat, monster *m, int delta) {
  bool carried = false;
  cell *c = m->base;
  #if CAP_COMPLEX2
  if(false && c->land == laWestWall) {
    cell *c2 = ts::left_of(c, westwall::coastvalEdge1);
      
    double spd = SCALE * delta / 900.;
        
    if(m->type == moVoidBeast) spd = -spd;
    if(spd) {
      shiftpoint goal = tC0(gmatrix[c2]);

      // transmatrix t = lspintox(H) * lxpush(delta/300.) * lrspintox(H);

      hyperpoint H = inverse_shift(m->pat, goal);
      nat = nat * lrspintox(H);
      nat = nat * lxpush(spd);
      nat = nat * lspintox(H);
      carried = true; 
      }
    }
  #endif
  if(c->land == laWhirlwind) {
    whirlwind::calcdirs(c);
    for(int i=0; i<whirlwind::qdirs; i++) {
      cell *c2 = c->move(whirlwind::dto[i]);
      if(!c2 || !gmatrix.count(c2)) continue;
      
      double spd = SCALE * delta / 900.;
        
      if(m->type == moVoidBeast) spd = -spd;
      if(spd) {
        shiftpoint goal = tC0(gmatrix[c2]);

        // transmatrix t = lspintox(H) * lxpush(delta/300.) * lrspintox(H);

        hyperpoint H = inverse_shift(m->pat, goal);
        nat = nat * lrspintox(H);
        nat = nat * lxpush(spd);
        nat = nat * lspintox(H);
        carried = true; 
        }
      }
    }
  #if CAP_FIELD
  if(c->land == laBlizzard) {
    int wmc = windmap::at(c);
    forCellEx(c2, c) { 
      if(!c2 || !gmatrix.count(c2)) continue;
      int z = (windmap::at(c2) - wmc) & 255;
      if(z >= 128) z -= 256;
      if(m->type == moVoidBeast) z = -z;
      if(z < windmap::NOWINDFROM && z > -windmap::NOWINDFROM) {
        shiftmatrix goal = gmatrix[c2];

        // transmatrix t = lspintox(H) * lxpush(delta/300.) * lrspintox(H);

        hyperpoint H = inverse_shift(m->pat, goal) * C0;
        nat = nat * lrspintox(H);
        nat = nat * lxpush(z * SCALE * delta / 50000.);
        nat = nat * lspintox(H);
        carried = true; 
        }
      }
    }
  #endif
  return carried;
  }

void roseCurrents(shiftmatrix& nat, monster *m, int delta) {
  if(ignoresSmell(m->type)) return;
  cell *c = m->base;
  
  int qty = 0;

  for(int i=0; i<c->type; i++) {
    cell *c2 = c->move(i);
    if(c2 && rosedist(c2) == 2) qty++;
    }
  
  for(int i=0; i<c->type; i++) {
    cell *c2 = c->move(i);
    if(!c2 || !gmatrix.count(c2)) continue;
    if(rosedist(c2) != 2) continue;
    
    double spd = SCALE * delta / 300. / qty;
        
    if(spd) {
      shiftpoint goal = tC0(gmatrix[c2]);

      // transmatrix t = lspintox(H) * lxpush(delta/300.) * lrspintox(H);

      hyperpoint H = inverse_shift(m->pat, goal);
      nat = nat * lrspintox(H);
      nat = nat * lxpush(spd);
      nat = nat * lspintox(H);
      }
    }
  }

shiftpoint keytarget(int i) {
  double d = 2 + sin(curtime / 350.);
  return pc[i]->pat * cpush0(WDIM == 3 ? 2 : 0, d * cgi.scalefactor);
  }

/* int charidof(int pid) {
  if(players == 1) return bak_charid;
  if(players == 2 || players == 4) return pid;
  if(players == 3) return pid < 2 ? pid : 2+(bak_charid&1);
  return 0;
  } */

ld getSwordSize() { return cgi.sword_size; }
ld getHornsSize() { return cgi.scalefactor * 0.33; }

// used in 3D
EX transmatrix swordmatrix[MAXPLAYER];

shiftpoint swordpos(int id, bool rev, double frac) {
  if(WDIM == 3)
    return pc[id]->pat * swordmatrix[id] * cpush0(2, (rev?-frac:frac) * getSwordSize());
  else
    return pc[id]->pat * xspinpush0(pc[id]->swordangle, (rev?-frac:frac) * getSwordSize());
  }

shiftpoint hornpos(int id) {
  return pc[id]->pat * lxpush0(getHornsSize());
  }

#define IGO 9

double igospan[IGO+1] = { 0, 
  A_PI/6, -A_PI/6,
  A_PI/4, -A_PI/4,
  A_PI/3, -A_PI/3,
  A_PI/2.1, -A_PI/2.1,
  0
  };

bool swordKills(eMonster m) {
  return 
    m != moHedge && m != moMetalBeast && m != moMetalBeast2
    && m != moTortoise && m != moGreater && m != moRoseBeauty
    && m != moReptile && !isBull(m) && m != moButterfly &&
    m != moSalamander && m != moTerraWarrior && m != moBrownBug;
  }

bool hornKills(eMonster m) {
  return 
    m != moHedge && m != moMetalBeast && m != moMetalBeast2
    && m != moTortoise && m != moGreater && m != moSkeleton
    && m != moDraugr && m != moRoseBeauty
    && m != moReptile && !isBull(m) && m != moButterfly && !isBulletType(m)
    && m != moPalace && m != moFatGuard && m != moVizier &&
    m != moSalamander && m != moTerraWarrior && m != moBrownBug;
  }

queue<pair<int, cell*>> traplist, firetraplist;

EX void activateArrow(cell *c) {
  if(isCentralTrap(c))
    traplist.emplace(ticks + 500, c);
  }

monster arrowtrap_fakeparent, dragon_fakeparent;

void doTraps() {
  while(true) { 
    if(traplist.empty()) break;
    auto t = traplist.front();
    if(t.first > ticks) break;
    int d = t.second->wparam;
    if(d == 2) {
      auto tl = traplimits(t.second);
      for(int i=1; i<4; i++) if(tl[i]) tl[i]->wparam = 3;
      traplist.emplace(t.first + 500, t.second);
      
      for(int i=0; i<5; i += 4) try {
        shiftmatrix& tu = gmatrix.at(tl[i]);
        shiftmatrix& tv = gmatrix.at(tl[4-i]);
        monster* bullet = new monster;
        bullet->base = tl[i];
        bullet->at = lrspintox(inverse_shift(tu, tC0(tv)));
        bullet->type = moArrowTrap;
        bullet->set_parent(&arrowtrap_fakeparent);
        arrowtrap_fakeparent.type = moArrowTrap;
        bullet->pid = 0;
        additional.push_back(bullet);
        }
      catch(out_of_range&) {}
      }
    else if(d == 3) {
      auto tl = traplimits(t.second);
      for(int i=1; i<4; i++) if(tl[i]) tl[i]->wparam = 0;
      }
    traplist.pop();
    }

  while(true) { 
    if(firetraplist.empty()) break;
    auto t = firetraplist.front();
    if(t.first > ticks) return;
    int d = t.second->wparam;
    if(d == 2) {
      t.second->wparam = 0;
      t.second->wall = waNone;
      explosion(t.second, 5, 10);
      }
    firetraplist.pop();
    }
  }

bool hornStuns(eMonster m) {
  return !isBulletType(m) && m != moRoseBeauty;
  }

bool noncrashable(monster *m, monster *by) {
  eMonster mt = m->type;
  if(mt == moGreater) return true;
  if(mt == moDraugr && by->type != moDraugr) return true;
  if(isBull(mt)) return true;
  if(mt == moReptile) return true;
  if(mt == moSalamander) return true;
  if(mt == moRoseBeauty && by->type != moRoseLady) return true;
  if(mt == moTortoise) return true;
  if(mt == moTerraWarrior) return true;
  if(mt == moSkeleton) return true;
  return false;
  }

int bulltime[MAXPLAYER];

// set to P_MIRRORWALL to allow the PCs to go through mirrors
static const int reflectflag = P_MIRRORWALL;

void movePlayer(monster *m, int delta) {

  bool falling = isGravityLand(m->base) && cellEdgeUnstable(m->base);
  if(m->base->land == laWestWall) falling = true;
  if(items[itOrbAether]) falling = false;

  bool inertia_based = falling || m->base->land == laAsteroids;

  cpid = m->pid;

  if(multi::players > 1 && multi::split_screen && cpid != subscreens::current_player) return;
  
  double mturn = 0, mgo = 0, mdx = 0, mdy = 0;
  
  bool shotkey = false, dropgreen = false, facemouse = false;
  if(facemouse) {
    // silence warning that facemouse unused
    }
  
  int b = 16*tableid[cpid];

    for(int i=(WDIM == 3 ? 4 : 0); i<8; i++) if(actionspressed[b+i]) playermoved = true;
  
  int jb = 4*tableid[cpid];
  for(int i=0; i<4; i++) if(axespressed[jb+i]) playermoved = true;
  
#if !ISMOBILE
  mgo = actionspressed[b+pcForward] - actionspressed[b+pcBackward] + axespressed[jb+2]/30000.;
  mturn = actionspressed[b+pcTurnLeft] - actionspressed[b+pcTurnRight] + axespressed[jb+3]/30000.;
  mdx = actionspressed[b+pcMoveRight] - actionspressed[b+pcMoveLeft] + axespressed[jb]/30000.;
  mdy = actionspressed[b+pcMoveDown] - actionspressed[b+pcMoveUp] + axespressed[jb+1]/30000.;
  
  shotkey = actionspressed[b+pcFire] || actionspressed[b+pcFaceFire];
  facemouse = actionspressed[b+pcFace] || actionspressed[b+pcFaceFire];
  dropgreen = actionspressed[b+pcDrop];
  
#else
  mdx = mdy = mgo = mturn = 0;
  facemouse = shotkey = false;
  dropgreen = getcstat == 'g';
  using namespace shmupballs;

  if(clicked && hypot(mousex - xfire, mousey - yb) < rad) {
    shotkey = true;
    mdx = (mousex - xfire) / (rad/2.);
    mdy = (mousey - yb) / (rad/2.);
    }
  if(clicked && hypot(mousex - xmove, mousey - yb) < rad) {
    mdx = (mousex - xmove) / (rad/2.);
    mdy = (mousey - yb) / (rad/2.);
    }
  #endif

  #if CAP_VR
  if(vrhr::active()) {
    if(GDIM == 3) {
      mouseaim_x += vrhr::vraim_x * delta / 400;
      mouseaim_y -= vrhr::vraim_y * delta / 400;
      mdy -= vrhr::vrgo_y;
      mdx += vrhr::vrgo_x;
      }
    else {
      println(hlog, "2dim");
      mturn -= vrhr::vraim_x + vrhr::vrgo_x;
      mgo += vrhr::vrgo_y + vrhr::vraim_y;
      }
    }
  #endif
  
  if(actionspressed[b+pcOrbPower] && !lactionpressed[b+pcOrbPower] && mouseover && !m->dead) {
    cwt.at = m->base;
    targetRangedOrb(mouseover, roKeyboard);
    }

#if !ISMOBILE
  if(haveRangedOrb() && !m->dead) {
    cwt.at = m->base;
    if(actionspressed[b+pcOrbKey] && !lactionpressed[b+pcOrbKey])
      keyresult[cpid] = targetRangedOrbKey(roKeyboard);
    else
      keyresult[cpid] = targetRangedOrbKey(roCheck);
    }
  else
#endif
    keyresult[cpid] = itNone;

  bool stdracing = racing::on && !inertia_based;

  if(actionspressed[b+pcCenter]) {
    if(!racing::on) {
      centerplayer = cpid; centerpc(100); playermoved = true; 
      }
    #if CAP_RACING
    if(racing::on) 
      racing::player_relative = !racing::player_relative;
    #endif
    }
  
  shiftmatrix nat = m->pat;
  
  // if(ka == b+pcOrbPower) dropgreen = true;
  
  // if(mturn > 1) mturn = 1;
  // if(mturn < -1) mturn = -1;
  
  #if CAP_RACING
  if(stdracing) {
    if(WDIM == 2) {
      if(abs(mdy) > abs(mgo)) mgo = -mdy;
      if(abs(mdx) > abs(mturn)) mturn = -mdx;
      mdx = mdy = 0;
      }
    facemouse = shotkey = dropgreen = false;
    if(!racing::started()) (WDIM == 2 ? mgo : mdy) = 0;
    }
  else {
    if(racing::on && !racing::started()) mgo = mdx = mdy = 0;
    }
  #endif
  
  playerturn[cpid] = mturn * delta / 150.0;
  
  godir[cpid] = 0;

  if(embedded_plane && vid.wall_height < 0) mdx = -mdx;

  if(WDIM == 2 && GDIM == 3 && (mdx || mdy)) {
    double mdd = hypot(mdx, mdy);
    godir[cpid] = -atan2(mdx, -mdy);
    mgo += mdd;
    }

  else if(WDIM == 2) {
    double mdd = hypot(mdx, mdy);
    
    if(mdd > 1e-6) {
      hyperpoint jh = point2(mdx/100.0, mdy/100.0);
      shiftpoint ctr = m->pat * C0;
  
      if(sphere && pconf.alpha > 1.001) for(int i=0; i<3; i++) ctr[i] = -ctr[i];
  
      hyperpoint h = inverse_shift(m->pat, rgpushxto0(ctr) * jh);
      
      playerturn[cpid] = -atan2(h[1], h[0]);
      mgo += mdd;
      }
    }

#if CAP_SDL
  const Uint8 *keystate = SDL12_GetKeyState(NULL);
  #if CAP_SDL2
  bool forcetarget = (keystate[SDL_SCANCODE_RSHIFT] | keystate[SDL_SCANCODE_LSHIFT]);
  #else
  bool forcetarget = (keystate[SDLK_RSHIFT] | keystate[SDLK_LSHIFT]);
  #endif
  if(((mousepressed && !forcetarget) || facemouse) && delta > 0 && !mouseout() && !stdracing && GDIM == 2) {
    // playermoved = true;
    hyperpoint h = inverse_shift(m->pat, mouseh);
    playerturn[cpid] = -atan2(h[1], h[0]);
    // nat = nat * spin(alpha);
    // mturn += alpha * 150. / delta;
    }
#endif

  bool blown = m->blowoff > curtime;

  #if CAP_MOUSEGRAB
  if(embedded_plane && !lctrlclick && cpid == 0) {
    if(!stdracing) playerturn[cpid] -= mouseaim_x;
    playerturny[cpid] -= mouseaim_y;
    mouseaim_x = 0;
    mouseaim_y = 0;
    }
  #endif

  if(embedded_plane && vid.wall_height < 0) playerturn[cpid] = -playerturn[cpid];
    
  if(playerturn[cpid] && canmove && !blown && WDIM == 2) {
    m->swordangle -= playerturn[cpid];
    rotate_object(nat.T, m->ori, spin(playerturn[cpid]));
    if(inertia_based) m->inertia = spin(-playerturn[cpid]) * m->inertia;
    }
  shiftmatrix nat0 = nat;
  
  if(m->base->land == laWhirlpool && !markOrb(itOrbWater))
    oceanCurrents(nat, m, delta);
    
  airCurrents(nat, m, delta);
    
  if(rosedist(m->base) == 1)
    roseCurrents(nat, m, delta);
    
  if(!canmove) mgo = 0;
  
  if(WDIM == 2) {
    if(mgo > 1) mgo = 1;
    if(mgo < -1) mgo = -1;
    if(stdracing) {
      // braking is more efficient
      if(m->vel * mgo < 0) mgo *= 3;
      m->vel += mgo * delta / 600;
      playergo[cpid] = m->vel * SCALE * delta / 600;
      }
    
    else {
      playergo[cpid] = mgo * SCALE * delta / 600;
      }
    }
  
  else if(WDIM == 3) {
    if(mdy > 1) mdy = 1;
    if(mdy < -1) mdy = -1;
    if(mdx > 1) mdx = 1;
    
    if(mdx < -1) mdx = -1;
    if(stdracing) {
      if(m->vel * -mdy < 0) mdy *= 3;
      m->vel += -mdy * delta / 600;
      playergo[cpid] = m->vel * SCALE * delta / 600;
      playerstrafe[cpid] = m->vel * mdx * SCALE * delta / 600;
      }
    else {
      playergo[cpid] = -mdy * SCALE * delta / 600;
      playerstrafe[cpid] = mdx * SCALE * delta / 600;
      }
    playerturn[cpid] = -mturn * SCALE * delta / 200;
    playerturny[cpid] = -mgo * SCALE * delta / 200;

    #if CAP_MOUSEGRAB
    if(!lctrlclick && cpid == 0) {
      playerturn[cpid] += mouseaim_x;
      playerturny[cpid] += mouseaim_y;
      mouseaim_x = mouseaim_y = 0;
      }
    #endif
    }
  
  if(playergo[cpid] && markOrb(itOrbDash)) playergo[cpid] *= 1.5;
  if(playergo[cpid] && markOrb(itCurseFatigue)) playergo[cpid] *= 0.75;

  bool go = false; 
  
  cell *c2 = m->base;
  
  if(blown) {
    playergo[cpid] = -SCALE * delta / 1000.;
    playerturn[cpid] = 0;
    }

  m->footphase += playergo[cpid];

  if(isReptile(m->base->wall)) m->base->wparam = reptilemax();
  
  int steps = 1 + abs(int(playergo[cpid] / (.2 * cgi.scalefactor)));
  
  playergo[cpid] /= steps;
  
  nextstep:

  shiftmatrix nat1 = nat;
  
  hyperpoint avg_inertia;
  
  if(inertia_based && canmove) {
    avg_inertia = m->inertia;
    ld coef = m->base->land == laWestWall ? 0.65 : falling ? 0.15 : 1;
    coef /= 1000;
    if(WDIM == 3) {
      m->inertia[frontdir()] += coef * playergo[cpid];
      avg_inertia[frontdir()] += coef * playergo[cpid] / 2;
      }
    else {
      m->inertia[0] += cos(godir[cpid]) * coef * playergo[cpid];
      avg_inertia[0] += cos(godir[cpid]) * coef * playergo[cpid] / 2;
      m->inertia[1] -= sin(godir[cpid]) * coef * playergo[cpid];
      avg_inertia[1] -= sin(godir[cpid]) * coef * playergo[cpid] / 2;
      }
    if(falling) {
      vector<cell*> below;
      manual_celllister mcl;
      mcl.add(m->base);
      for(int i=0; i<isize(mcl.lst); i++) {
        cell *c = mcl.lst[i];
        bool go = false;
        if(c->land == laMountain) {
          int d = celldistAlt(c);
          forCellEx(c2, c) if(celldistAlt(c2) > d && gmatrix.count(c2))
            go = true, mcl.add(c2);
          }
        else {
          int d = coastvalEdge(c);
          forCellEx(c2, c) if(coastvalEdge(c2) < d && gmatrix.count(c2))
            go = true, mcl.add(c2);
          }
        if(!go) below.push_back(c);
        }
      ld cinertia = hypot_d(WDIM, m->inertia);
      hyperpoint drag = m->inertia * cinertia * delta / -1. / SCALE;
      m->inertia += drag;
      avg_inertia += drag/2;
      ld xp = SCALE / 60000. / isize(below) * delta / 15;
      ld yp = 0;
      if(cwt.at->land == laDungeon) xp = -xp;
      if(cwt.at->land == laWestWall) yp = xp * 1, xp *= 0.7;
      for(cell *c2: below) if(c2 != m->base) {
        
        hyperpoint h = lrspintox(inverse_shift(m->pat, tC0(gmatrix[c2]))) * point2(xp, yp);
      
        m->inertia += h;
        avg_inertia += h/2;
        }
      }
    // if(inertia_based) m->inertia = spin(-playerturn[cpid]) * m->inertia;
    }
  
  int fspin = 0;
    
  for(int igo=0; igo<IGO && !go; igo++) {
  
    go = true;
    
    if(inertia_based) {
      playergoturn[cpid] = 0;
      if(igo) { go = false; break; }
      ld r = hypot_d(WDIM, avg_inertia);
      apply_shift_object(nat.T, m->ori, lrspintox(avg_inertia) * xtangent(r * delta));
      if(WDIM == 3) rotate_object(nat.T, m->ori, cspin(0, 2, playerturn[cpid]) * cspin(1, 2, playerturny[cpid]));
      m->vel = r * (600/SCALE);
      }
    else if(WDIM == 3) {
      playersmallspin[cpid] = Id;
      if(igo) {
        fspin += 30;
        playersmallspin[cpid] = cspin(0, 1, fspin) * cspin(2, 0, igospan[igo]);
        if(fspin < 360) igo--; else fspin = 0;
        }
      nat.T = shift_object(nat1.T, m->ori, playersmallspin[cpid] * point3(playerstrafe[cpid], 0, playergo[cpid]));
      rotate_object(nat.T, m->ori, cspin(0, 2, playerturn[cpid]) * cspin(1, 2, playerturny[cpid]));
      m->inertia[0] = playerstrafe[cpid] / delta;
      m->inertia[1] = 0;
      m->inertia[2] = playergo[cpid] / delta;
      }
    else if(playergo[cpid]) {
      playergoturn[cpid] = igospan[igo]+godir[cpid];    
      nat.T = shift_object(nat1.T, m->ori, cspin(0, 1, playergoturn[cpid]) * xtangent(playergo[cpid]));
      m->inertia = spin(playergoturn[cpid]) * xtangent(playergo[cpid] / delta);
      }
    
    // spin(span[igo]) * lxpush(playergo[cpid]) * spin(-span[igo]);
  
    c2 = m->findbase(nat, 1);
    if(reflectflag & P_MIRRORWALL) reflect(c2, m->base, nat);
    
    // don't have several players in one spot
    // also don't let them run too far from each other!
    monster* crashintomon = NULL;
    
    if(!m->isVirtual) {
      crashintomon = playerCrash(m, nat*C0);
      for(monster *m2: nonvirtual) if(m2!=m && m2->type == passive_switch) {
        double d = sqdist(m2->pat*C0, nat*C0);
        if(d < SCALE2 * 0.2) crashintomon = m2;
        }
      }
    if(crashintomon) go = false;
  
    if(go && c2 != m->base) {

      if(c2->wall == waLake && markOrb(itOrbWinter) && !nonAdjacent(c2, m->base)) {
        c2->wall = waFrozenLake;
        if(HEAT(c2) > .5) HEAT(c2) = .5;
        }
  
      else if(c2->wall == waBigStatue && canPushStatueOn(m->base, P_ISPLAYER) && !nonAdjacent(c2, m->base)) {
        visibleFor(300);
        c2->wall = m->base->wall;
        if(cellUnstable(cwt.at))
          m->base->wall = waChasm; 
        else {
          m->base->wall = waBigStatue;
          animateMovement(match(c2, m->base), LAYER_BOAT);
          }
        }
      else if(m->inBoat && !isWateryOrBoat(c2) && passable(c2, m->base, P_ISPLAYER | P_MIRROR | reflectflag)) {
        if(boatGoesThrough(c2) && markOrb(itOrbWater)) {
          collectItem(c2, m->base);
          c2->wall = isIcyLand(m->base) ? waLake : waSea;
          }
        else {
          if(isWatery(m->base)) 
            m->base->wall = waBoat, m->base->mondir = neighborId(m->base, c2);
          else if(boatStrandable(m->base))
            m->base->wall = waStrandedBoat;
          else if(boatStrandable(c2))
            m->base->wall = waStrandedBoat;
          m->inBoat = false;
          }
        }
      else if(isPushable(c2->wall) && !nonAdjacent(c2, m->base)) {
        int sd1 = neighborId(c2, m->base);
        int sd = m->base->c.spin(sd1);
        int subdir = 1;
        double bestd = 9999;
        for(int di=-1; di<2; di+=2) {
          cell *c = c2->modmove(sd+di);
          if(!c) continue;
          if(m->isVirtual || !gmatrix.count(c)) continue;
          double d = sqdist(gmatrix[c] * C0, m->pat * C0);
          if(d<bestd) bestd=d, subdir = di;
          }
        pushmonsters();
        auto mip = determinePush(cellwalker(c2, sd1)+wstep, subdir, [m] (movei mi) { return canPushThumperOn(mi, m->base); });
        visibleFor(300);
        if(!mip.proper()) go = false;
        else pushThumper(mip);
        popmonsters();
        }
      else if(c2->wall == waRose && !nonAdjacent(m->base, c2)) {
        m->dead = true;
        go = false;
        }
      else if(
        (blown ? !passable(c2, m->base, P_ISPLAYER | P_BLOW) : !passable(c2, m->base, P_ISPLAYER | P_MIRROR | reflectflag)) && 
        !(isWatery(c2) && m->inBoat && !nonAdjacent(m->base,c2)))
        go = false;
      
      }
    
    if(go && WDIM == 3) {
      for(int i=0; i<27; i++) {
        hyperpoint v;
        int i0 = i;
        for(int a=0; a<3; a++) v[a] = (i0 % 3) - 1, i0 /= 3;
        v = v * .1 / hypot_d(3, v);
        shiftmatrix T1 = (i == 13) ? nat : shiftless(shift_object(nat.T, m->ori, v), nat.shift);
        cell *c3 = c2;
        while(true) {
          cell *c4 = findbaseAround(tC0(T1), c3, 1);
          if(c4 == c3) break;
          if(!passable(c4, c3, P_ISPLAYER | P_MIRROR | reflectflag)) { go = false; break; }
          c3 = c4;
          }
        }
      }
    }
  
  if(!go || abs(playergo[cpid]) < 1e-3 || abs(playerturn[cpid]) > 1e-3) bulltime[cpid] = curtime;
  
  if(!go) {
    playergo[cpid] = playergoturn[cpid] = playerstrafe[cpid] = 0;
    if(WDIM == 3) playerturn[cpid] = playerturny[cpid] = 0;
    if(falling) m->inertia = m->inertia * -1;
    else m->inertia = Hypc;
    }
  
  if(go) {

    if(WDIM == 3) {
      swordmatrix[cpid] = 
        cspin(1, 2, -playerturny[cpid]) * cspin(0, 2, -playerturn[cpid]) * swordmatrix[cpid];
      m->inertia = cspin(1, 2, -playerturny[cpid]) * cspin(0, 2, -playerturn[cpid]) * m->inertia;
      }

    if(c2 != m->base) {
      doPickupItemsWithMagnetism(c2);

      if(cellUnstable(m->base) && !markOrb(itOrbAether))
        doesFallSound(m->base);
    
      if(items[itOrbFire]) {
        visibleFor(800);
        if(makeflame(m->base, 10, false)) markOrb(itOrbFire);
        }

      if(items[itCurseWater]) {
        visibleFor(800);
        if(makeshallow(m->base, 10, false)) markOrb(itCurseWater);
        }

      if(isIcyLand(m->base) && m->base->wall == waNone && markOrb(itOrbWinter)) {
        invismove = false;
        m->base->wall = waIcewall;
        }
      
      if(items[itOrbDigging]) {
        visibleFor(400);
        if(earthMove(match(m->base, c2))) markOrb(itOrbDigging);
        }
      
      cwt.at = c2; afterplayermoved();
      if(c2->item && c2->land == laAlchemist) c2->wall = m->base->wall;
      #if CAP_COMPLEX2
      if(m->base->wall == waRoundTable)
        camelot::roundTableMessage(c2);
      #endif
      if(c2->wall == waCloud || c2->wall == waMirror) {
        visibleFor(500);
        cellwalker cw(c2, 0, false);
        mirror::createHere(cw, cpid);
        mirror::breakMirror(cw, cpid);
        awakenMimics(m, c2);
        #if CAP_RACING
        if(racing::on) racing::race_won();
        #endif
        }
      if(c2->wall == waGlass && items[itOrbAether]) {
        items[itOrbAether] = 0;
        addMessage(XLAT("Your Aether powers are drained by %the1!", c2->wall));
        }
      movecost(m->base, c2, 1);

      if(c2->wall == waMineMine && !markOrb(itOrbAether) && !markOrb(itOrbWinter)) {
        items[itOrbLife] = 0;
        m->dead = true;
        }
      #if CAP_COMPLEX2
      mine::uncover_full(c2);
      #endif
      
      if(isWatery(c2) && isWatery(m->base) && m->inBoat)
        moveItem(m->base, c2, true);
  
      destroyWeakBranch(m->base, c2, moPlayer);

      if(c2->wall == waClosePlate || c2->wall == waOpenPlate)
        toggleGates(c2, c2->wall);

      if(c2->wall == waArrowTrap && c2->wparam == 0 && !markOrb(itOrbAether))
        activateArrowTrap(c2);
  
      if(c2->wall == waFireTrap && c2->wparam == 0 && !markOrb(itOrbAether)) {
        c2->wparam = 2;
        firetraplist.emplace(ticks + 800, c2);
        }
  
      if(c2->item == itOrbYendor && !peace::on) yendor::check(c2);
      collectItem(c2, m->base);
      movecost(m->base, c2, 2);
      }
    }

  if(go) m->rebasePat(nat, c2);
  else m->rebasePat(nat0, m->base);

  if(m->base->wall == waBoat && !m->inBoat) {
    m->inBoat = true; m->base->wall = waSea;
    }
  
  if(m->base->wall == waStrandedBoat && !m->inBoat && markOrb(itOrbWater)) {
    m->inBoat = true; m->base->wall = waSea;
    }

  if(m->inBoat && boatStrandable(c2)) {
    c2->wall = waStrandedBoat;
    m->inBoat = false;
    }

  if(!markOrb(itOrbAether)) {
    if(m->base->wall == waChasm || m->base->wall == waClosedGate)
      m->dead = true;

    if(isWatery(m->base) && !m->inBoat && !markOrb(itOrbFish))
      m->dead = true;

    if(isFireOrMagma(m->base) && !markOrb(itOrbWinter))
      m->dead = true;
    }

  landvisited[m->base->land] = true;
  
  playerfire[cpid] = false;
  
  if(items[itOrbHorns] && !m->isVirtual) {
    shiftpoint H = hornpos(cpid);

    for(monster *m2: nonvirtual) {
      if(m2 == m) continue;
      
      double d = sqdist(m2->pat*C0, H);
    
      if(d < SCALE2 * 0.1) {
        if(hornKills(m2->type))
          killMonster(m2, moPlayer);
        else if(hornStuns(m2->type))
          m2->stunoff = max(m2->stunoff, curtime + 150);
        }
      }
    }
    
  for(int b=0; b<2; b++) if(sword::orbcount(b) && !m->isVirtual) {
  
    for(double d=0; d<=1.001; d += .1) {
      shiftpoint H = swordpos(cpid, b, d);
  
      for(monster *m2: nonvirtual) {
        if(m2 == m) continue;
        
        double d = sqdist(m2->pat*C0, H);
      
        if(d < SCALE2 * 0.1) {
          if(swordKills(m2->type) && !(isBullet(m2) && m2->pid == cpid))
              killMonster(m2, moPlayer);
        }
      }
  
      cell *c3 = findbaseAround(H, m->base, 999);
      if(c3->wall == waSmallTree || c3->wall == waBigTree || c3->wall == waBarrowDig || c3->wall == waCavewall ||
        (c3->wall == waBarrowWall && items[itBarrow] >= 25))
        c3->wall = waNone;
      
      else if(isWall(c3)) break;
      }
    }

  if(go) {
    // printf("#%3d: at %s\n", steps, display(nat * C0));
    steps--;
    if(steps > 0) {
      nat0 = nat;
      go = false;
      goto nextstep;
      }
    }

  #if CAP_RACING  
  if(!go && stdracing) {
    if(GDIM == 3) m->vel = max(m->vel * -.5 - 0.1, 0.);
    else m->vel = 0;
    }
  #endif
  
  if(shotkey && canmove && curtime >= m->nextshot) {

    visibleFor(500);
    if(items[itOrbFlash]) {
      pushmonsters();
      killMonster(m->base, moNone);
      cwt.at = m->base;
      activateFlash();
      popmonsters();
      return;
      }
    
    if(items[itOrbLightning]) {
      pushmonsters();
      killMonster(m->base, moLightningBolt);
      cwt.at = m->base;
      activateLightning();
      popmonsters();
      return;
      }
    
    playerfire[cpid] = true;
    m->nextshot = curtime + (250 + 250 * players);
    
    turncount++;    
    shootBullet(m);
    }
  
  if(dropgreen && m->base->item == itNone)
    dropGreenStone(m->base);
  }

EX monster *getPlayer() {
  return pc[cpid];
  }

void virtualize(monster *m) {
  if(doall) forCellCM(c2, m->base) if(!gmatrix.count(c2)) {
    m->isVirtual = true;
    m->pat = shiftless(m->at);
    return;
    }
  }

bool reflectmatrix(shiftmatrix& M, cell *c1, cell *c2, bool onlypos) {
  if(!gmatrix.count(c1) || !gmatrix.count(c2)) return false;
  transmatrix H = inverse_shift(gmatrix[c1], gmatrix[c2]);
  transmatrix S = lspintox(tC0(H));
  ld d = hdist0(tC0(H));
  transmatrix T = lxpush(-d/2) * S * inverse_shift(gmatrix[c1], M);
  if(onlypos && tC0(T)[0] < 0) return false;
  M = gmatrix[c1] * iso_inverse(S) * lxpush(d/2) * MirrorX * T;
  return true;
  }

EX int reflect(cell*& c2, cell*& mbase, shiftmatrix& nat) {
  int reflections = 0;
  if(c2 != mbase && c2->wall == waMirrorWall && inmirror(c2)) {
    if(reflectmatrix(nat, mbase, c2, false)) {
      c2 = mbase;
      reflections++;
      }
    }

  if(c2 == mbase && inmirror(c2)) {
    forCellEx(c3, c2) if(c3->land == laMirrorWall) {
      cell *c1 = mbase;
      mbase = c3;
      reflect(c3, mbase, nat);
      mbase = c1;
      c2 = c3;
      reflections++;
      }
    }
  
  if(c2 == mbase && c2->wall == waMirrorWall && c2->land == laMirrorWall) {
    int d = mirror::mirrordir(c2);
    if(d != -1) {
      for(int k=0; k<7; k++) {
        cell *ca = c2->cmodmove(d-k);
        cell *cb = c2->cmodmove(d+k);
        if(ca->land == laMirror && inmirror(cb)) {
          reflectmatrix(nat, ca, cb, true);
          reflections++;
          break;
          }
        }
      }
    else {
      for(int k=0; k<6; k++) {
        cell *cb = c2->cmodmove(k+1);
        cell *cc = c2->cmodmove(k+2);
        if(cb->land != laMirrorWall || cc->land != laMirrorWall) continue;
        cell *ca = c2->cmodmove(k);
        cell *cd = c2->cmodmove(k+3);
        if(reflectmatrix(nat, cc, ca, true)) reflections++;
        for(int limit=0; limit<10 && reflectmatrix(nat, cb, cd, true) && (reflections++, reflectmatrix(nat, cc, ca, true)); limit++) reflections+=2;
        }
      } 
    }
  return reflections;
  }

void moveMimic(monster *m) {
  if(multi::players > 1 && multi::split_screen && cpid != subscreens::current_player) return;
  virtualize(m);
  shiftmatrix nat = m->pat;
  cpid = m->pid;
  m->footphase = getPlayer()->footphase;
  
  // no need to care about Mirror images, as they already have their 'at' matrix reversed :|

  if(WDIM == 3) {
    nat.T = shift_object(nat.T, m->ori, playersmallspin[cpid] * point3(playerstrafe[cpid], 0, playergo[cpid]));
    rotate_object(nat.T, m->ori, cspin(0, 2, playerturn[cpid]) * cspin(1, 2, playerturny[cpid]));
    }
  else
    nat = nat * spin(playerturn[cpid] + playergoturn[cpid]) * lxpush(playergo[cpid]) * spin(-playergoturn[cpid]);

  cell *c2 = m->findbase(nat, 1);
  reflect(c2, m->base, nat);
  if(c2 != m->base && !passable(c2, m->base, P_ISPLAYER | P_MIRROR | P_MIRRORWALL))
    killMonster(m, moNone);
  else {
    m->rebasePat(nat, c2);
    if(playerfire[cpid]) shootBullet(m);
    }  

  if(c2->wall == waCloud || c2->wall == waMirror) {
    cellwalker cw(c2, 0, false);
    mirror::createHere(cw, cpid);
    mirror::breakMirror(cw, -1);
    awakenMimics(m, c2);
    }

  if(!doall && c2->cpdist >= 6)
    m->dead = true;
  }

bool isPlayerOrImage(eMonster m) {
  return isMimic(m) || m == moPlayer;
  }

monster *parentOrSelf(monster *m) {
  return m->parent ? m->parent : m;
  }

EX bool verifyTeleport() {
  if(!on) return true;
  if(playerCrash(pc[cpid], mouseh)) return false;
  return true;
  }

void destroyMimics() {
  for(monster *m: active)
    if(isMimic(m->type)) 
      m->dead = true;
  }

EX void teleported() {
  monster *m = pc[cpid];
  m->base = cwt.at;
  m->at = rgpushxto0(inverse_shift(gmatrix[cwt.at], mouseh)) * random_spin();
  m->findpat();
  destroyMimics();
  }

void shoot(eItem it, monster *m) {
  monster* bullet = new monster;
  bullet->base = m->base;
  bullet->at = m->at * lrspintox(inverse_shift(m->pat, mouseh));
  /* ori */
  if(WDIM == 3) bullet->at = bullet->at * cpush(2, 0.15 * SCALE);
  bullet->type = it == itOrbDragon ? moFireball : it == itOrbAir ? moAirball : moBullet;
  bullet->set_parent(m);
  bullet->pid = m->pid;
  items[it]--;
  additional.push_back(bullet);
  }

eItem targetRangedOrbKey(orbAction a) {
  shiftpoint h = mouseh;
  cell *b = mouseover;
  monster *mt = mousetarget;

  mouseh = keytarget(cpid);
  
  mouseover = pc[cpid]->base;
  
  mouseover = findbaseAround(mouseh, mouseover, 999);
  mousetarget = NULL;

  for(monster *m2: nonvirtual) {
    if(m2->dead) continue;
    if(m2->no_targetting) continue;
    if(!mousetarget || sqdist(mouseh, mousetarget->pat*C0) > sqdist(mouseh, m2->pat*C0)) 
      mousetarget = m2;
    }

  eItem r = targetRangedOrb(mouseover, a);
  // printf("A%d i %d h %p t %p ov %s => %s\n", a, cpid, mouseover, mousetarget, display(mouseh), dnameof(r));
  
  mouseh = h;
  mousetarget = mt;
  mouseover = b;
  return r;
  }

EX eItem targetRangedOrb(orbAction a) {
  if(!on) return itNone;
  monster *wpc = pc[cpid];
  if(a != roCheck && !wpc) return itNone;  
  
  if(items[itOrbPsi] && shmup::mousetarget && sqdist(mouseh, shmup::mousetarget->pat*C0) < SCALE2 * .1) {
    if(a == roCheck) return itOrbPsi;
    addMessage(XLAT("You kill %the1 with a mental blast!", mousetarget->type));
    killMonster(mousetarget, moNone);
    items[itOrbPsi] -= 30;
    if(items[itOrbPsi]<0) items[itOrbPsi] = 0;
    return itOrbPsi;
    }
  
  if(items[itOrbStunning] && shmup::mousetarget && sqdist(mouseh, shmup::mousetarget->pat*C0) < SCALE2 * .1) {
    if(a == roCheck) return itOrbStunning;
    mousetarget->stunoff = curtime + 1000;
    items[itOrbStunning] -= 10;
    if(items[itOrbStunning]<0) items[itOrbStunning] = 0;
    return itOrbStunning;
    }
  
  if(on && items[itOrbDragon]) {
    if(a == roCheck) return itOrbDragon;
    shoot(itOrbDragon, wpc);
    return itOrbDragon;
    }
  
  if(on && items[itOrbAir]) {
    if(a == roCheck) return itOrbAir;
    shoot(itOrbAir, wpc);
    return itOrbAir;
    }
  
  if(on && items[itOrbIllusion]) {
    if(a == roCheck) return itOrbIllusion;
    shoot(itOrbIllusion, wpc);
    return itOrbIllusion;
    }

  return itNone;
  }

int speedfactor() {
  return items[itOrbSpeed]?2:1;
  }

int bulletdir() { 
  return (WDIM == 2) ? 0 : 2;
  }
  
hyperpoint fronttangent(ld x) {
  if(WDIM == 2) return xtangent(x);
  else return ztangent(x);
  }

ld collision_distance(monster *bullet, monster *target) {
  if(target->type == moAsteroid)
    return SCALE * 0.15 + cgi.asteroid_size[target->hitpoints & 7];
  return SCALE * 0.3;
  }

void spawn_asteroids(monster *bullet, monster *target) {
  if(target->hitpoints <= 1) return;
  hyperpoint rnd = random_spin() * point2(SCALE/3000., 0);
  
  hyperpoint bullet_inertia = inverse_shift(target->pat, bullet->pat * bullet->inertia);

  for(int i=0; i<2; i++) {
    monster* child = new monster;
    child->base = target->base;
    child->at = target->at;
    child->ori = target->ori;
    child->type = target->type;
    child->pid = target->pid;
    child->inertia = target->inertia;
    child->inertia += bullet_inertia / 5;
    child->hitpoints = target->hitpoints - 1;
    if(i == 0) child->inertia += rnd;
    if(i == 1) child->inertia -= rnd;  
    additional.push_back(child);
    }
  }

EX int protect_pid(int i) {
  if(i < 0 || i >= players) return 0;
  return i;
  }

EX bool check_split(monster *m) {
  if(!(split_screen && multi::players > 1)) return true;
  if(m->split_tick != ticks) {
    m->split_tick = ticks;
    ld min_dist = HUGE_VAL;
    int i = 0;
    for(auto& d: subscreens::player_displays) {
      for(const shiftmatrix& V : hr::span_at(d.all_drawn_copies, m->base)) {
        ld dist = hdist0(tC0(V * m->at));
        if(dist < min_dist) min_dist = dist, m->split_owner = i;
        }
      i++;
      }
    }
  return m->split_owner == subscreens::current_player;
  }

void moveBullet(monster *m, int delta) {
  if(!check_split(m)) return;

  cpid = protect_pid(m->pid);
  m->findpat();
  virtualize(m);
  
  shiftmatrix nat0 = m->pat;
  shiftmatrix nat = m->pat;
  
  bool inertia_based = m->base->land == laAsteroids;
  
  if(m->base->land == laAsteroids) {
    m->hitpoints += delta;
    if(m->hitpoints >= (WDIM == 3 ? 750 : 500)) m->dead = true;
    }

  if(isReptile(m->base->wall)) m->base->wparam = reptilemax();
    
  if(m->type == moFlailBullet) {
    m->vel -= delta  / speedfactor() / 600000.0;
    if(m->vel < 0 && m->parent) {
      // return to the flailer!
      nat = spin_towards(m->pat, m->ori, m->parent->pat * C0, bulletdir(), -1);
      }
    }
  else m->vel = bullet_velocity(m->type);
  
  if(m->type == moTongue && (m->isVirtual || !m->parent || sqdist(nat*C0, m->parent->pat*C0) > SCALE2 * 0.4))
    m->dead = true;

  if(inertia_based) {
    nat.T = shift_object(nat.T, m->ori, m->inertia * delta);
    }
  else 
    nat.T = shift_object(nat.T, m->ori, fronttangent(delta * SCALE * m->vel / speedfactor()));
  cell *c2 = m->findbase(nat, fake::split() ? 10 : 1);

  if(m->parent && isPlayer(m->parent) && markOrb(itOrbLava) && c2 != m->base && !isPlayerOn(m->base)) 
    makeflame(m->base, 5, false);

  if(isActivable(c2)) activateActiv(c2, true);
  
  // knives break mirrors and clouds
  if(c2->wall == waCloud || c2->wall == waMirror) {
    cellwalker cw(c2, 0, false);
    mirror::createHere(cw, cpid);
    mirror::breakMirror(cw, -1);
    awakenMimics(m, c2);
    }
  
  reflect(c2, m->base, nat);
  
  bool godragon = m->type == moFireball && isDragon(c2->monst);
  
  eMonster ptype = parentOrSelf(m)->type;
  bool slayer = m->type == moCrushball ||
    (markOrb(itOrbSlaying) && (markOrb(itOrbEmpathy) ? isPlayerOrImage(ptype) : ptype == moPlayer));
  bool weak = m->type == moAirball || 
    (markOrb(itCurseWeakness) && (markOrb(itOrbEmpathy) ? isPlayerOrImage(ptype) : ptype == moPlayer));
      
  if(m->type != moTongue && !(godragon || (c2==m->base && m->type == moArrowTrap) || passable(c2, m->base, P_BULLET | P_MIRRORWALL))) {
    m->dead = true;
    if(!weak && (!isDie(c2->monst) || slayer))
      killMonster(c2, m->get_parenttype());
    // cell *c = m->base;
    if(m->parent && isPlayer(m->parent)) {
      if(c2->wall == waBigTree) {
        addMessage(XLAT("You start chopping down the tree."));
        c2->wall = waSmallTree;
        }
      else if(c2->wall == waSmallTree) {
        addMessage(XLAT("You chop down the tree."));
        c2->wall = waNone;
        }
      else if(isActivable(c2)) 
        activateActiv(c2, true);
      else if(c2->wall == waExplosiveBarrel)
        explodeBarrel(c2);
      }
    if(m->type == moCrushball && c2->wall == waRuinWall)
      c2->wall = waNone;
    if(m->type == moFireball) {
      makeflame(c2, 20, false) || makeflame(m->base, 20, false);
      }
    }
  m->rebasePat(nat, c2);
  
  // destroy stray bullets
  if(!doall) for(int i=0; i<m->base->type; i++) 
    if(!m->base->move(i) || !gmatrix.count(m->base->move(i)))
      m->dead = true;

  // items[itOrbWinter] = 100; items[itOrbLife] = 100;
  
  bool no_self_hits = (m->type != moFlailBullet && !multi::self_hits) || m->fragoff > curtime;

  if(!m->isVirtual) for(monster* m2: nonvirtual) {
    if(m2 == m) continue;
    if((m2 == m->parent && no_self_hits) || (m2->parent == m->parent && no_self_hits)) continue;
    
    if(m2->dead) continue;

    // Flailers only killable by themselves
    if(m2->type == moFlailer && m2 != m->parent) continue;
    // be nice to your images! would be too hard otherwise...
    if(isPlayerOrImage(parentOrSelf(m)->type) && isPlayerOrImage(parentOrSelf(m2)->type) &&
      m2->pid == m->pid && no_self_hits)
      continue;
    if(isPlayer(parentOrSelf(m)) && isPlayer(m2) && m2->pid != m->pid && !friendly_fire)
      continue;
    // fireballs/airballs don't collide
    if(m->type == moFireball && m2->type == moFireball) continue;
    if(m->type == moAirball && m2->type == moAirball) continue;
    double d = hdist(m2->pat*C0, m->pat*C0);
    
    if(d < collision_distance(m, m2)) {

      if(m2->type == passive_switch) { m->dead = true; continue; }
      
      if(weak && isBlowableMonster(m2->type)) {

        if(m2->blowoff < curtime) {
          hyperpoint h = inverse_shift(m2->pat, nat0 * C0);
          if(WDIM == 3)
           swordmatrix[m2->pid] = lspintox(h) * swordmatrix[m2->pid];
          else
            m2->swordangle += atan2(h[1], h[0]);
          m2->rebasePat(m2->pat * lrspintox(h), m2->base);
          }
        m2->blowoff = curtime + 1000;
        continue;
        }
      // Hedgehog Warriors only killable outside of the 45 degree angle
      if(m2->type == moHedge && !peace::on && !slayer) {
        hyperpoint h = inverse_shift(m2->pat, m->pat * C0);
        if(h[0] > fabsl(h[1])) { m->dead = true; continue; }
        }
      if(peace::on && !isIvy(m2->type)) {
        m->dead = true;
        m2->stunoff = curtime + 600;
        continue;
        }
      // multi-HP monsters
      if((m2->type == moPalace || m2->type == moFatGuard || m2->type == moSkeleton ||
        m2->type == moVizier || isMetalBeast(m2->type) || m2->type == moTortoise || m2->type == moBrownBug || 
        m2->type == moReptile || m2->type == moSalamander || m2->type == moTerraWarrior) && m2->hitpoints > 1 && !slayer) {
        m2->rebasePat(spin_towards(m2->pat, m->ori, nat0 * C0, 0, 1), m2->base);
        if(m2->type != moSkeleton && !isMetalBeast(m2->type) && m2->type != moReptile && m2->type != moSalamander && m2->type != moBrownBug) 
          m2->hitpoints--;
        m->dead = true;
        if(m2->type == moVizier) ;
        else if(m2->type == moFatGuard)
          m2->stunoff = curtime + 600;
        else if(m2->type == moTerraWarrior)
          m2->stunoff = curtime + 300 * (6 - m2->hitpoints);
        else if(m2->type == moMetalBeast || m2->type == moMetalBeast2)
          m2->stunoff = curtime + 3000;
        else if(m2->type == moReptile)
          m2->stunoff = curtime + 3000;
        else if(m2->type == moTortoise)
          m2->stunoff = curtime + 3000;
        else if(m2->type == moSkeleton && m2->base->land != laPalace)
          m2->stunoff = curtime + 2100;
        else
          m2->stunoff = curtime + 900;
        continue;
        }
      // conventional missiles cannot hurt some monsters
      bool conv = (m->type == moBullet || m->type == moFlailBullet || m->type == moTongue || m->type == moArrowTrap) && !slayer;

      // Raiders are unaffected
      if((m2->type == moCrusher || m2->type == moPair || m2->type == moMonk ||
        m2->type == moAltDemon || m2->type == moHexDemon) && conv) {
        m->dead = true;
        continue;
        }
      if(m2->type == moGreater && conv) {
        m->dead = true;
        continue;
        }
      if(m2->type == moRoseBeauty && conv && !markOrb(itOrbBeauty)) {
        m->dead = true;
        continue;
        }
      if(m2->type == moDraugr && conv) {
        m->dead = true;
        continue;
        }
      if(m2->type == moButterfly && conv) {
        m->dead = true;
        continue;
        }
      if(isBull(m2->type) && conv) {
        m->dead = true;
        // enrage herd bulls, awaken sleeping bulls
        m2->type = moRagingBull;
        continue;
        }
      // Knights reflect bullets
      if(m2->type == moKnight) {
        if(m->parent && m->parent != &arrowtrap_fakeparent) {
          nat = spin_towards(nat, m->ori, tC0(m->parent->pat), bulletdir(), 1);
          m->rebasePat(nat, m->base);
          }
        m->set_parent(m2);
        continue;
        }
      m->dead = true;
      if(m->type == moFireball) makeflame(m->base, 20, false);
      // Orb of Winter protects from fireballs
      if(m->type == moFireball && ((isPlayer(m2) && markOrb(itOrbWinter)) || m2->type == moWitchWinter)) 
        continue;
      int ms = mirrorspirits;
      killMonster(m2, m->get_parenttype());
      if(mirrorspirits > ms) {
        multi::kills[cpid]--;
        }
      if(m2->dead && m2->type == moAsteroid) {
        gainItem(itAsteroid);
        spawn_asteroids(m, m2);
        }
      }
    }
  }

shiftpoint closerTo;

bool closer(monster *m1, monster *m2) {
  return sqdist(m1->pat*C0,  closerTo) < sqdist(m2->pat*C0, closerTo);
  }

EX monster *create_bullet(monster *m, eMonster type) {
  monster* bullet = new monster;
  bullet->base = m->base;
  bullet->at = m->at;
  bullet->ori = m->ori;
  bullet->type = type;
  bullet->set_parent(m);
  additional.push_back(bullet);
  bullet->fragoff = curtime + bullet_time;
  return bullet;
  }

EX bool dragonbreath(cell *dragon) {
  int randplayer = hrand(numplayers());
  monster* bullet = new monster;
  bullet->base = dragon;
  bullet->at = spin_towards(Id, bullet->ori, inverse_shift(gmatrix[dragon], tC0(pc[randplayer]->pat)), bulletdir(), 1);
  bullet->type = moFireball;
  bullet->set_parent(&dragon_fakeparent); dragon_fakeparent.type = moDragonHead;
  bullet->pid = randplayer;
  additional.push_back(bullet);
  return true;
  }

#define CHARGING (-777)
#define BULLSTUN (1500)

void moveMonster(monster *m, int delta) {

  if(!check_split(m)) return;

  bool inertia_based = m->type == moAsteroid || m->type == moRogueviz;
  
  bool stunned = m->stunoff > curtime || m->blowoff > curtime;
  
  if(stunned && cellUnstable(m->base))
    doesFallSound(m->base);

  if(isReptile(m->base->wall)) m->base->wparam = reptilemax();
  
  if(m->base->wall == waChasm && !survivesChasm(m->type) && m->type != moReptile)
    killMonster(m, moNone, AF_FALL);

  if(m->base->wall == waRose && !survivesThorns(m->type))
    killMonster(m, moNone);
  
  if(among(m->type, moGreaterShark, moShark, moCShark) && !isWateryOrBoat(m->base)) {
    if(m->type == moGreaterShark) m->type = moGreaterM;
    else killMonster(m, moNone, AF_CRUSH);
    }


  if(isWatery(m->base) && !survivesWater(m->type) && !m->inBoat && m->type != moReptile)
    killMonster(m, moNone);

  if(isFireOrMagma(m->base)) {
    if(m->type == moSalamander)
      m->stunoff = max(curtime+500, m->stunoff);
    else if(!survivesFire(m->type))
      killMonster(m, moNone);
    }
  
  if(m->base->wall == waClosedGate && !survivesWall(m->type))
    killMonster(m, moNone);

  if(m->dead) return;
    
  cell *c = m->base;
  shiftmatrix goal = gmatrix[c];
  
  bool direct = false; // is there a direct path to the target?
  int directi = 0; // which player has direct path (to set as pid in missiles)
  
  double step = SCALE * delta/1000.0;
  if(m->type == moWitchSpeed)
    step *= 2;
  else if(m->type == moEagle)
    step *= 1.6;
  else if(m->type == moHunterDog)
    step *= (1 + .5 / numplayers());
  else if(m->type == moLancer)
    step *= 1.25;
  else if(isDemon(m->type)) {
    if(m->type == moLesserM) m->type = moLesser;
    if(m->type == moGreaterM) m->type = moGreater;
    step /= 2;
    }
  else if(m->type == moMetalBeast || m->type == moMetalBeast2) 
    step /= 2;
  else if(m->type == moTortoise && peace::on)
    step = 0;
  else if(m->type == moTortoise)
    step /= 3;
  else if(isBull(m->type))
    step *= 1.5;
  else if(m->type == moAltDemon || m->type == moHexDemon || m->type == moCrusher || m->type == moMonk)
    step *= 1.4;

  if(m->type == passive_switch) step = 0;
  
  if(items[itOrbBeauty] && !m->isVirtual) {
    bool nearplayer = false;
    for(int pid=0; pid<players; pid++) if(!pc[pid]->isVirtual) {
      double dist = sqdist(pc[pid]->pat*C0, m->pat*C0);
      if(dist < SCALE2) nearplayer = true;
      }
    if(nearplayer) markOrb(itOrbBeauty), step /= 2;
    }

  if(m->isVirtual) {
    if(inertia_based) {
      ld r = hypot_d(WDIM, m->inertia);
      shiftmatrix nat = m->pat * lrspintox(m->inertia) * lxpush(r * delta) * lspintox(m->inertia);
      m->rebasePat(nat, m->base);
      }
    return;
    }
  shiftmatrix nat = m->pat;

  if(stunned) {
    if(m->blowoff > curtime) {
      step = SCALE * -delta / 1000.;
      }
    else if(m->type == moFatGuard || m->type == moTortoise || m->type == moRagingBull || m->type == moTerraWarrior)
      step = 0;
    else if(m->type == moReptile)
      step = SCALE * -delta / 1000. * (m->stunoff - curtime) / 3000.;
    else if(m->type == moSalamander) {
      if(isFireOrMagma(m->base)) step = 0;
      else
        step = SCALE * -delta / 2000.;
      }
    else step = SCALE * -delta/2000.;
    }
  
  else if(m->type == moRagingBull && m->stunoff == CHARGING) ;
  
  else if(inertia_based) ;

  else {
  
    if(m->type == moSleepBull && !m->isVirtual) {
      for(monster *m2: nonvirtual) if(m2!=m && m2->type != moBullet && m2->type != moArrowTrap) {
        double d = sqdist(m2->pat*C0, nat*C0);
        if(d < SCALE2*3 && m2->type == moPlayer) m->type = moRagingBull;
        }
      }
    
    if(m->type == moWitchFlash) for(int pid=0; pid<players; pid++) {
      if(pc[pid]->isVirtual) continue;
      if(m->isVirtual) continue;
      bool okay = sqdist(pc[pid]->pat*C0, m->pat*C0) < 2 * SCALE2;
      for(monster *m2: nonvirtual) {
        if(m2 != m && isWitch(m2->type) && sqdist(m2->pat*C0, m->pat*C0) < 2 * SCALE2)
          okay = false;
        }
      if(okay) {
        addMessage(XLAT("%The1 activates her Flash spell!", m->type));
        pushmonsters();
        activateFlashFrom(m->base, moWitchFlash, AF_MAGIC | AF_GETPLAYER | AF_MSG);
        popmonsters();
        m->type = moWitch;
        pc[pid]->dead = true;
        }
      }
    if(isBug(m->type)) {
      vector<monster*> bugtargets;
      for(monster *m2: nonvirtual) 
        if(!isBullet(m2))
        if(m2->type != m->type)
        if(!isPlayer(m2) || !invismove)
        if(!m2->dead)
          bugtargets.push_back(m2);
      
      closerTo = m->pat * C0;
      sort(bugtargets.begin(), bugtargets.end(), closer);
  
      if(step) for(monster *m2: bugtargets)
        if(trackroute(m, m2->pat, step)) {
          goal = m2->pat;
          direct = true;
          break;
          }
      }
    else if(m->type == moWolf && !peace::on) {
      cell *cnext = c;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->move(i);
        if(c2 && gmatrix.count(c2) && (c2->land == laVolcano || (isIcyLand(c2) && HEAT(c2) > HEAT(c))) && passable(c2, c, 0))
          cnext = c2;
        }
      goal = gmatrix[cnext];
      direct = true;
      directi = 0;
      }
    #if CAP_FIELD
    else if(m->type == moHerdBull) {
      cell *cnext = prairie::next(c);
      if(cnext && gmatrix.count(cnext)) {
        goal = gmatrix[cnext];
        direct = true;
        directi = 0;
        }
      else m->dead = true;
      }
    #endif
    else if(m->type == moButterfly) {
      int d = neighborId(m->base, m->torigin);
      cell *cnext = NULL;
      for(int u=2; u<m->base->type; u++) {
        cell *c2 = createMov(m->base, (d+u) % m->base->type);
        if(passable_for(m->type, c2, m->base, P_ONPLAYER)) {
          cnext = c2;
          break;
          }
        }

      if(cnext && gmatrix.count(cnext)) {
        goal = gmatrix[cnext];
        direct = true;
        directi = 0;
        }
      }
    else if(!direct && !invismove && !peace::on) {
      for(int i=0; i<players; i++) 
        if(step && trackroute(m, pc[i]->pat, step) && (!direct || sqdist(pc[i]->pat*C0, m->pat*C0) < sqdist(goal*C0,m->pat*C0))) {
          goal = pc[i]->pat;
          direct = true;
          directi = i;
          // m->trackrouteView(pc->pat, step);
          }
        }
  
    if(!direct && !peace::on) while(true) {
      if(step && trackroute(m, gmatrix[c], step))
        goal = gmatrix[c];
      cell *cnext = c;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->move(i);
        if(c2 && gmatrix.count(c2) && c2->pathdist < c->pathdist &&
          passable_for(m->type, c2, c, P_CHAIN | P_ONPLAYER))
          cnext = c2;
        }
      if(cnext == c) break;
      c = cnext;
      }

    if(m->type == moHedge) {
      hyperpoint h = inverse_shift(m->pat, tC0(goal));
      if(h[1] < 0)
        nat = nat * spin(A_PI * delta / 3000 / speedfactor());
      else
        nat = nat * spin(A_PI * -delta / 3000 / speedfactor());
      m->rebasePat(nat, m->base);
      // at most 45 degrees
      if(h[0] < fabsl(h[1])) return;
      }
    else if(!peace::on) {
      nat = spin_towards(m->pat, m->ori, tC0(goal), 0, 1);
      }
    }
  
  if(m->type == moVampire && !m->isVirtual) for(int i=0; i<players; i++) 
  if(!pc[i]->isVirtual && sqdist(m->pat*C0, pc[i]->pat*C0) < SCALE2 * 2) {
    for(int i=0; i<ittypes; i++)
      if(itemclass(eItem(i)) == IC_ORB && items[i] && items[itOrbTime] && !orbused[i])
        orbused[i] = true;
    step = 0;
    }

  bool carried = false;

  if(c->land == laWhirlpool && (m->type == moShark || m->type == moCShark || m->type == moPirate))
    oceanCurrents(nat, m, delta), carried = true;

  if(m->type != moGhost && m->type != moFriendlyGhost && m->type != moAirElemental)
    carried |= airCurrents(nat, m, delta);
  
  if(rosedist(m->base) == 1)
    roseCurrents(nat, m, delta), carried = true;

  step /= speedfactor();

  if(WDIM == 3) step /= 3;
  
  int igo = 0;
  
  shiftmatrix nat0 = nat;
  
  igo_retry:
  
  if(igo == IGO && peace::on) 
    nat0 = nat0 * spin(rand() % 16); 
  
  else if(igo >= IGO) {
    if(m->type == moHerdBull) m->type = moRagingBull;
    return;
    }
  
  if(igo == 1 && m->type == moRagingBull && m->stunoff == CHARGING) {
    m->stunoff = curtime + BULLSTUN;
    return;
    }
  
  if(inertia_based) {
    if(igo) return;
    nat.T = shift_object(nat.T, m->ori, m->inertia * delta);
    }
  else if(WDIM == 3 && igo) {
    ld fspin = rand() % 1000;  
    nat.T = shift_object(nat0.T, m->ori, cspin(1,2,fspin) * spin(igospan[igo]) * xtangent(step));
    }
  else {
    nat.T = shift_object(nat0.T, m->ori, spin(igospan[igo]) * xtangent(step));
    }

  if(m->type != moRagingBull && !peace::on)
  if(sqdist(nat*C0, goal*C0) >= sqdist(m->pat*C0, goal*C0) && !stunned && !carried && !inertia_based) {
    igo++; goto igo_retry; }

  for(int i=0; i<multi::players; i++) for(int b=0; b<2; b++) if(sword::orbcount(b)) {  
    if(pc[i]->isVirtual) continue;
    shiftpoint H = swordpos(i, b, 1);
    double d = sqdist(H, nat*C0);
    if(d < SCALE2 * 0.12) { igo++; goto igo_retry; }
    }

  m->footphase += step;

  monster* crashintomon = NULL;
  
  if(!m->isVirtual && !inertia_based) for(monster *m2: nonvirtual) if(m2!=m && m2->type != moBullet && m2->type != moArrowTrap) {
    double d = sqdist(m2->pat*C0, nat*C0);
    if(d < SCALE2 * 0.1) crashintomon = m2;
    }
  
  if(inertia_based) for(int i=0; i<players; i++) if(pc[i] && hdist(tC0(pc[i]->pat), tC0(m->pat)) < collision_distance(pc[i], m))
    crashintomon = pc[i];
  
  if(!peace::on) 
  for(int i=0; i<players; i++) 
    if(crashintomon == pc[i]) 
      pc[i]->dead = true;

  if(peace::on) ; 

  else if(crashintomon && isMimic(crashintomon->type)) {
    killMonster(crashintomon, m->type);
    crashintomon = NULL;
    }
  
  else if(crashintomon && (
    items[itOrbDiscord] || isBull(m->type) ||
       ((isBug(m->type) || isBug(crashintomon->type)) && m->type != crashintomon->type)) 
    && !isBullet(crashintomon)) {
    if(noncrashable(crashintomon, m)) {
      if(isBull(crashintomon->type)) crashintomon->type = moRagingBull;
      }
    else {
      killMonster(crashintomon, m->type, isBull(m->type) ? AF_BULL : 0);
      crashintomon = NULL;
      }
    }
  
  if(crashintomon && !inertia_based) { igo++; goto igo_retry; }

  cell *c2 = m->findbase(nat, 1);
  if(reflectflag & P_MIRRORWALL) reflect(c2, m->base, nat);

  if(m->type == moButterfly && !passable_for(m->type, c2, m->base, P_CHAIN | reflectflag)) {
    igo++; goto igo_retry;
    }

  if(isPlayerOn(c2) && !peace::on) {
    bool usetongue = false;
    if(isSlimeMover(m->type) || m->type == moWaterElemental) usetongue = true;
    if(isWatery(c2) && !survivesWater(m->type) && !m->inBoat) usetongue = true;
    if(c2->wall == waChasm && !survivesChasm(m->type)) usetongue = true;
    if(isFireOrMagma(c2) && !survivesFire(m->type) && !m->inBoat) usetongue = true;
    if(isBird(m->type) && !passable_for(moEagle, c2, c, 0)) usetongue = true;
    if((m->type == moMonk || m->type == moAltDemon || m->type == moHexDemon) && !passable_for(m->type, c2, c, 0))
      usetongue = true;
    if(usetongue) {
      if(curtime < m->nextshot) return;
      // m->nextshot = curtime + 25;
      monster* bullet = create_bullet(m, moTongue);
      bullet->pid = whichPlayerOn(c2);
      return;
      }
    }

  if(!ignoresPlates(m->type)) destroyWeakBranch(m->base, c2, m->type);

  if(c2 != m->base && (c2->wall == waClosePlate || c2->wall == waOpenPlate) && !ignoresPlates(m->type))
    toggleGates(c2, c2->wall, 3);

  if(c2 != m->base && c2->wall == waArrowTrap && c2->wparam == 0 && !ignoresPlates(m->type))
    activateArrowTrap(c2);

  if(c2 != m->base && c2->wall == waFireTrap && c2->wparam == 0 && !ignoresPlates(m->type)) {
    c2->wparam = 2;
    firetraplist.emplace(curtime + 800, c2);
    }

  if(c2 != m->base && mayExplodeMine(c2, m->type)) 
    killMonster(m, moNone);
  
  if(c2 != m->base && c2->wall == waRose && !nonAdjacent(m->base, c2) && !survivesThorns(m->type))
    killMonster(m, moNone);
  
  if(c2 != m->base && cellUnstable(m->base) && !ignoresPlates(m->type))
    doesFallSound(m->base);

  if(m->type == moWolf && c2->land == laVolcano) m->type = moLavaWolf;
  if(m->type == moLavaWolf && isIcyLand(c2)) m->type = moWolf;
  
  if(c2 != m->base && m->type == moWitchFire) makeflame(m->base, 10, false);
  if(c2 != m->base && m->type == moFireElemental) makeflame(m->base, 20, false);
  if(c2 != m->base && m->type == moWaterElemental) placeWater(c2, m->base);
  if(c2 != m->base && m->type == moEarthElemental) {
    earthMove(match(m->base, c2));
    }
  
  if(m->type == moReptile && c2 != m->base) {
    if(c2->wall == waChasm) {
      c2->wall = waReptile;
      c2->wparam = reptilemax();
      playSound(c, "click");
      m->dead = true;
      }
    else if(isChasmy(c2) || isWatery(c2)) {
      c2->wall = waReptileBridge;
      c2->item = itNone;
      c2->wparam = reptilemax();
      playSound(c, "click");
      m->dead = true;
      }
    }
  
  if(c2 != m->base && m->type == moNecromancer && !c2->monst) {
    for(int i=0; i<m->base->type; i++) {
      cell *c3 = m->base->move(i);
      if(neighborId(c3, c2) != -1 && c3->wall == waFreshGrave && gmatrix.count(c3)) {
        bool monstersNear = false;
        for(monster *m2: nonvirtual) {
          if(m2 != m && sqdist(m2->pat*C0, gmatrix[c3]*C0) < SCALE2 * .3)
            monstersNear = true;
          if(m2 != m && sqdist(m2->pat*C0, gmatrix[c2]*C0) < SCALE2 * .3)
            monstersNear = true;
          }
        if(!monstersNear) {

          monster* undead = new monster;
          undead->base = c2;
          undead->at = Id;
          undead->type = moZombie;
          undead->set_parent(m);
          undead->pid = 0;
          undead->findpat();
          additional.push_back(undead);

          undead = new monster;
          undead->base = c3;
          undead->at = Id;
          undead->type = moGhost;
          undead->set_parent(m);
          undead->findpat();
          undead->pid = 0;
          additional.push_back(undead);

          c3->wall = waAncientGrave;
          addMessage(XLAT("%The1 raises some undead!", m->type));
          }
        }
      }
    }
  if(m->type == moGreaterShark) {
    if(c2->wall == waBoat)
      c2->wall = waNone;
    if(c2->wall == waFrozenLake)
      c2->wall = waLake;
    }
  if(m->type == moDarkTroll && c2->wall == waCavefloor) {
    m->type = moTroll;
    }
  if(isLeader(m->type)) {
    if(c2 != m->base) {
      if(c2->wall == waBigStatue && canPushStatueOn(m->base, 0)) {
        c2->wall = m->base->wall;
        if(cellUnstable(m->base))
          m->base->wall = waChasm;
        else
          m->base->wall = waBigStatue;
        animateMovement(match(c2, m->base), LAYER_BOAT);
        }
      if(passable_for(m->type, c2, m->base, P_CHAIN | P_ONPLAYER | reflectflag) && !isWatery(c2) && m->inBoat) {
        if(isWatery(m->base)) 
          m->base->wall = waBoat, m->base->mondir = neighborId(m->base, c2);
        else if(boatStrandable(c2)) c2->wall = waStrandedBoat;
        else if(boatStrandable(m->base)) m->base->wall = waStrandedBoat;
        m->inBoat = false;
        }
      if(isWatery(c2) && isWatery(m->base) && m->inBoat)
        moveItem(m->base, c2, true);
      }
    if(c2->wall == waBoat && !m->inBoat) {
      m->inBoat = true; c2->wall = waSea;
      m->base = c2;
      }
    }
  
  if(peace::on && c2->mpdist > 7) return;
  
  if(!(m->type == moRoseBeauty && c2->land != laRose)) {
    if(stunned ? passable(c2, m->base, P_BLOW | reflectflag) : passable_for(m->type, c2, m->base, P_CHAIN | reflectflag)) {
      if(c2 != m->base && m->type == moButterfly) 
        m->torigin = m->base;
      m->rebasePat(nat, c2);
      if(m->type == moRagingBull && step > 1e-6) m->stunoff = CHARGING;
      }
    else {
      if(peace::on) { igo++; goto igo_retry; }
      if(m->type == moRagingBull && m->stunoff == CHARGING) {
        auto old = m->base->monst;
        m->base->monst = m->type;
        beastcrash(c2, m->base);
        if(m->base->monst != m->type) m->dead = true;
        m->base->monst = old;
        m->stunoff = curtime + BULLSTUN;
        }
      }
    }

  if(direct) {
    if((m->type == moPyroCultist || m->type == moCrystalSage) && curtime >= m->nextshot) {
      monster* bullet = create_bullet(m, moFireball);
      bullet->pid = directi;
      if(m->type == moPyroCultist) 
        m->type = moCultist;
      else
        m->nextshot = curtime + 100;
      }
    if(m->type == moOutlaw && curtime >= m->nextshot) {
      monster* bullet = create_bullet(m, moBullet);
      bullet->pid = directi;
      m->nextshot = curtime + 1500;
      }
    for(int i=0; i<players; i++) if(!pc[i]->isVirtual)
    if((m->type == moAirElemental) && curtime >= m->nextshot && sqdist(m->pat*C0, pc[i]->pat*C0) < SCALE2 * 2) {
      monster* bullet = create_bullet(m, moAirball);
      bullet->pid = i;
      m->nextshot = curtime + 1500;
      }
    for(int i=0; i<players; i++) if(!pc[i]->isVirtual)
      if(m->type == moTortoise && tortoise::seek() && !tortoise::diff(getBits(m->torigin)) && sqdist(m->pat*C0, pc[i]->pat*C0) < SCALE2) {
        items[itBabyTortoise] += 4;
        m->dead = true;
        tortoise_hero_message(m->base);
        }
    for(int i=0; i<players; i++) if(!pc[i]->isVirtual)
    if(m->type == moFlailer && curtime >= m->nextshot && 
      sqdist(m->pat*C0, pc[i]->pat*C0) < SCALE2 * 2) {
      m->nextshot = curtime + 3500;
      monster* bullet = create_bullet(m, moFlailBullet);
      bullet->vel = 1/400.0;
      bullet->pid = i;
      break;
      }
    for(int i=0; i<players; i++) if(!pc[i]->isVirtual)
    if(m->type == moCrusher && sqdist(m->pat*C0, pc[i]->pat*C0) < SCALE2 * .75) {    
      m->stunoff = curtime + 1500;
      monster* bullet = create_bullet(m, moCrushball);
      bullet->pid = i;
      break;
      }
    for(int i=0; i<players; i++) if(!pc[i]->isVirtual)
    if(m->type == moHexer && m->base->item && (classflag(m->base->item) & IF_CURSE) && sqdist(m->pat*C0, pc[i]->pat*C0) < SCALE2 * 2) {
      addMessage(XLAT("%The1 curses you with %the2!", m->type, m->base->item));
      items[m->base->item] += orbcharges(m->base->item);
      m->base->item = itNone;
      m->stunoff = curtime + 250;
      break;
      }
    }
  }

void activateMonstersAt(cell *c) {
  pair<mit, mit> p = 
    monstersAt.equal_range(c);
  for(mit it = p.first; it != p.second;) {
    mit itplus = it;
    itplus++;
    active.push_back(it->second);
    monstersAt.erase(it);
    it = itplus;
    }
  if(c->monst && isMimic(c->monst)) c->monst = moNone;
  // mimics are awakened by awakenMimics
  if(c->monst && !isIvy(c) && !isWorm(c) && !isMutantIvy(c) && !isKraken(c->monst) && c->monst != moPrincess && c->monst != moHunterGuard && !isDie(c->monst)) {
    // awaken as a monster
    monster *enemy = new monster;
    enemy->at = Id;
    enemy->base = c;
    if(enemy->type == moButterfly)
      enemy->torigin = createMov(c, (c->mondir + 419) % c->type);
    enemy->torigin = c;
    enemy->type = c->monst;
    enemy->hitpoints = c->hitpoints;
    if(c->wall == waBoat && isLeader(c->monst)) 
      enemy->inBoat = true, c->wall = waSea;
    if(c->monst == moAsteroid) {
      enemy->inertia = random_spin() * point2(SCALE/3000., 0);
      }
    c->monst = moNone;
    active.push_back(enemy);
    }
  }

EX void fixStorage() {

  vector<monster*> restore;

  for(auto it = monstersAt.begin(); it != monstersAt.end(); it++) 
    restore.push_back(it->second);

  monstersAt.clear();

  for(monster *m: restore) m->store();
  }

EX hookset<bool(int)> hooks_turn;

/** the amount of time chars are disabled in PvP */
EX int pvp_delay = 2000;

EX int count_pauses;
EX bool in_pause;

EX void turn(int delta) {

  if(split_screen && subscreens::split( [delta] () { turn(delta); })) return;
  
  int id = 0;
  #if CAP_MOUSEGRAB
  ld maimx = mouseaim_x;
  ld maimy = mouseaim_y;
  #else
  ld maimx, maimy;
  #endif
  
  if(dual::split( [&id, maimx, maimy, delta] () {
    turn(delta); id++; 
    #if CAP_MOUSEGRAB
    if(id==1) mouseaim_x = maimx, mouseaim_y = maimy;
    #endif
    })) return;

  if(callhandlers(false, hooks_turn, delta)) return;

  lmousetarget = NULL;
  if(mousetarget && !mousetarget->isVirtual && sqdist(mouseh, mousetarget->pat*C0) < 0.1)
    lmousetarget = mousetarget;

  if(!shmup::on) return;  
  if(!(cmode & sm::NORMAL)) {
    #if CAP_RACING
    if(!in_pause) {
      in_pause = true;
      if(!(racing::on && !racing::started() && !racing::finished())) {
        count_pauses++;
        if(racing::on) addMessage(XLAT("Pauses: %1 of %2 allowed", its(count_pauses), its(racing::pause_limit)));
        }
      }
    if(racing::on) {
      if(racing::race_start_tick) racing::race_start_tick += delta;
      for(int& i: racing::race_finish_tick) if(i) i += delta;
      }
    #endif
    return;
    }
  else in_pause = false;

  passive_switch = (gold() & 1) ? moSwitch1 : moSwitch2;
  
  if(delta > 1000) delta = 1000;
  
  if(delta > 200) { turn(200); delta -= 200; if(!delta) return; }

  curtime += delta;

  handleInput(delta);
  
  invismove = (curtime >= visibleAt) && markOrb(itOrbInvis);

  // detect active monsters
  if(doall)
    for(cell *c: currentmap->allcells()) activateMonstersAt(c);
  else
    for(map<cell*, shiftmatrix>::iterator it = gmatrix.begin(); it != gmatrix.end(); it++) 
      activateMonstersAt(it->first);
  
  /* printf("size: gmatrix = %ld, active = %ld, monstersAt = %ld, delta = %d\n", 
    gmatrix.size(), active.size(), monstersAt.size(),
    delta); */
  
  bool exists[motypes];
  
  for(int i=0; i<motypes; i++) exists[i] = false;

  nonvirtual.clear();
  for(monster *m: active) {
    m->findpat();
    if(m->isVirtual) continue;
    else nonvirtual.push_back(m);
    exists[movegroup(m->type)] = true;
    }
  
  for(monster *m: active) {
    
    switch(m->type) {
      case moPlayer: 
        movePlayer(m, delta);
        break;
      
      case moBullet: case moFlailBullet: case moFireball: case moTongue: case moAirball:
      case moArrowTrap: case moCrushball:
        moveBullet(m, delta);
        break;
      
      default: ;
      }
    }

  for(monster *m: active) {
    if(isMimic(m->type))
      moveMimic(m);
    }

  for(int t=1; t<motypes; t++) if(exists[t]) {
  
    pathdata pd(1);
        
    // build the path data
    
    for(cell *c: targets)
      onpath(c, isPlayerOn(c) ? 0 : 1);

    int qb = 0;
    for(qb=0; qb < isize(pathq); qb++) {
      cell *c = pathq[qb].at;
      int d = c->pathdist;
      if(d == PINFD-1) continue;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->move(i);
        // printf("i=%d cd=%d\n", i, c->move(i)->cpdist);
        if(c2 && c2->pathdist == PINFD && gmatrix.count(c2) && 
          (passable_for(eMonster(t), c, c2, P_CHAIN | P_ONPLAYER) || c->wall == waThumperOn)) {
          onpath(c2, d+1);
          }
        }
      }
    
    // printf("time %d, t=%d, q=%d\n", curtime, t, qb);
  
    // move monsters of this type
    
    for(monster *m: nonvirtual)
      if(movegroup(m->type) == t)
        moveMonster(m, delta);
    }
  
  if(shmup::on) {

    doTraps();

    bool tick = curtime >= nextmove;
    keepLightning = ticks <= lightat + 1000;
    cwt.at = pc[0]->base;
    bfs(); moverefresh(tick);
    countLocalTreasure();
    pushmonsters();
    if(items[itOrbFreedom])
      for(int i=0; i<players; i++)
        checkFreedom(pc[i]->base);
    heat::processheat(delta / 350.0);
    markOrb(itOrbSpeed);
    
    if((havewhat&HF_DRAGON) && curtime >= nextdragon) {
      groupmove(moDragonHead, 0);
      nextdragon = curtime + 1500;
      }
  
    if(tick) {
      nextmove += 1000;
      flashMessages();
      reduceOrbPowers();
      if(items[itOrbBull]) for(int p=0; p<players; p++) 
        if(bulltime[p] < curtime - 600) orbbull::gainBullPowers();
        
      if(!((items[itOrbSpeed]/players) & 1)) {
        if(havewhat&HF_KRAKEN) kraken::attacks(), groupmove(moKrakenH, 0);     
        moveworms();
        moveivy();
        movemutant();
        if(havewhat&HF_HEX) movehex_all();
        if(havewhat & HF_DICE) groupmove(moAnimatedDie, 0);    
        wandering();
        livecaves();
        #if CAP_INV
        if(inv::on) inv::compute();
        #endif
        #if CAP_COMPLEX2
        terracotta::check();
        #endif
        heat::processfires();
        if(havewhat&HF_WHIRLPOOL) whirlpool::move();
        if(havewhat&HF_WHIRLWIND) whirlwind::move();
        #if CAP_COMPLEX2
        if(havewhat & HF_WESTWALL) westwall::move();
        #endif
        buildRosemap();
        if(havewhat&HF_RIVER) prairie::move();
        }
      if(recallCell.at && !markOrb(itOrbRecall)) activateRecall();
      save_memory();
      }
    if(elec::havecharge) elec::act();
    popmonsters();
    
    bool lastcanmove = canmove;
    
    canmove = true;
    
    for(int i=0; i<players; i++) {
      if(pc[i]->dead && items[itOrbShield]) {
        pc[i]->dead = false;
        orbused[itOrbShield] = true;
        }
    
      if(pc[i]->dead && items[itOrbFlash]) {
        pc[i]->dead = false;
        pushmonsters();
        killMonster(pc[i]->base, moNone);
        activateFlash();
        popmonsters();
        }
        
      if(pc[i]->dead && items[itOrbLightning]) {
        pc[i]->dead = false;
        pushmonsters();
        killMonster(pc[i]->base, moLightningBolt);
        activateLightning();
        popmonsters();
        }
      
      if(pc[i]->dead && items[itOrbShell]) {
        pc[i]->dead = false;
        useupOrb(itOrbShell, 10);
        items[itOrbShield] = 1;
        orbused[itOrbShield] = true;
        }

      if(pc[i]->dead && pvp_mode) {
        pc[i]->dead = false;
        if(curtime > pc[i]->fragoff) {
          pc[i]->fragoff = curtime + pvp_delay;
          pc[i]->nextshot = min(pc[i]->nextshot, pc[i]->fragoff);
          multi::deaths[i]++;
          }
        }
    
      if(pc[i]->dead && items[itOrbLife]) {
        multi::deaths[i]++;
        items[itOrbLife]--;
        items[itOrbShield] += 3;
        items[itOrbAether] += 3;
        pc[i]->dead = false;
        orbused[itOrbShield] = true;
        }
    
      if(pc[i]->dead && !lastdead) {
        multi::deaths[i]++;
        achievement_final(true);
        }
      lastdead = pc[i]->dead;
      
      if(lastcanmove && pc[i]->dead) showMissionScreen();
        
      canmove = canmove && !pc[i]->dead;
      }
    }
  
  for(monster *m: additional) 
    active.push_back(m);
  additional.clear();
  
  if(delayed_safety) { 
    activateSafety(delayed_safety_land);
    delayed_safety = false;
    }

  // deactivate all monsters
  for(monster *m: active)
    if(m->dead && m->type != moPlayer) {
      if(m == mousetarget) mousetarget = NULL;
      if(m == lmousetarget) lmousetarget = NULL;
      m->remove_reference();
      }
    else {
      m->store();
      }
    
  active.clear();
  }

EX void recall() {
  for(int i=0; i<players; i++) {
    pc[i]->base = cwt.at;
    if(players == 1)
      pc[i]->at = Id;
    else
      pc[i]->at = spin(TAU*i/players) * lxpush(firstland == laMotion ? .5 : .3) * Id;
    /* ggmatrix(cwt.at);
    display(gmatrix[cwt.at]);
    pc[i]->findpat(); */
    }
  destroyMimics();
  }

EX void init() {

  for(int i=0; i<players; i++) pc[i] = NULL;
  
  for(int i=0; i<players; i++) {
    pc[i] = new monster;
    pc[i]->type = moPlayer;
    pc[i]->pid = i;
    if(players == 1)
      pc[i]->at = Id;
    else
      pc[i]->at = spin(TAU*i/players) * lxpush(firstland == laMotion ? .5 : .3) * Id;
    pc[i]->pat = shiftless(pc[i]->at);
    pc[i]->base = cwt.at;
    pc[i]->vel = 0;
    pc[i]->inBoat = (firstland == laCaribbean || firstland == laOcean || firstland == laLivefjord ||
      firstland == laWhirlpool);
    pc[i]->store();
    swordmatrix[i] = Id;
    }
  
  if(!safety) {
    items[itOrbLife] = 3;
    if(!racing::on) 
      addMessage(XLAT("Welcome to the Shoot'em Up mode!"));
    // addMessage(XLAT("F/;/Space/Enter/KP5 = fire, WASD/IJKL/Numpad = move"));
    }
  delayed_safety = false;
  }

EX bool boatAt(cell *c) {
  pair<mit, mit> p = 
    monstersAt.equal_range(c);
  for(mit it = p.first; it != p.second; it++) {
    monster* m = it->second;
    if(m->inBoat) return true;
    }
  return false;
  }

EX hookset<bool(const shiftmatrix&, cell*, shmup::monster*)> hooks_draw;

EX void clearMonsters() {
  for(mit it = monstersAt.begin(); it != monstersAt.end(); it++)
    delete(it->second);
  for(monster *m: active) m->remove_reference();
  mousetarget = NULL;
  lmousetarget = NULL;
  monstersAt.clear();
  active.clear();
  }

EX void clearMemory() {
  clearMonsters();
  while(!traplist.empty()) traplist.pop();
  curtime = 0;
  nextmove = 0;
  nextdragon = 0;
  visibleAt = 0;
  for(int i=0; i<MAXPLAYER; i++) pc[i] = NULL;
  }

void gamedata(hr::gamedata* gd) { 
  if(true) {
    gd->store(pc[0]); // assuming 1 player!
    gd->store(nextmove);
    gd->store(curtime);
    gd->store(nextdragon);
    gd->store(visibleAt);
    gd->store(traplist);
    gd->store(monstersAt);
    gd->store(active);
    gd->store(mousetarget);
    gd->store(lmousetarget);
    gd->store(nonvirtual);
    gd->store(additional);
    if(WDIM == 3) gd->store(swordmatrix[0]); // assuming 1 player!
    gd->store(traplist);
    gd->store(firetraplist);
    }
  }

EX cell *playerpos(int i) {
  if(!pc[i]) return NULL;
  return pc[i]->base;
  }

EX bool playerInBoat(int i) {
  if(!pc[i]) return false;
  return pc[i]->inBoat;
  }

EX void destroyBoats(cell *c) {
  for(monster *m: active)
    if(m->base == c && m->inBoat)
      m->inBoat = false;
  }

EX void virtualRebase(shmup::monster *m) {
  virtualRebase(m->base, m->at);
  }

EX hookset<bool(shmup::monster*, string&)> hooks_describe;

EX void addShmupHelp(string& out) {
  if(shmup::mousetarget && sqdist(mouseh, tC0(shmup::mousetarget->pat)) < .1) {
    if(callhandlers(false, hooks_describe, shmup::mousetarget, out)) return;
    out += ", ";
    out += XLAT1(minf[shmup::mousetarget->type].name);
    help = generateHelpForMonster(shmup::mousetarget->type);
    }
  }

auto hooks = addHook(hooks_clearmemory, 0, shmup::clearMemory) +
  addHook(hooks_gamedata, 0, shmup::gamedata) +
  addHook(hooks_removecells, 0, [] () {
    for(mit it = monstersAt.begin(); it != monstersAt.end();) {
      if(is_cell_removed(it->first)) {
        monstersAt.insert(make_pair(nullptr, it->second));
        auto it0 = it; it++;
        monstersAt.erase(it0);
        }
      else it++;
      }
    });

EX void switch_shmup() { 
  stop_game();
  switch_game_mode(rg::shmup);
  resetScores();
  start_game();
  }

#if MAXMDIM >= 4
auto hooksw = addHook(hooks_swapdim, 100, [] {
  for(auto& p: monstersAt) swapmatrix(p.second->at);
  });
#endif
    
}

shiftmatrix at_missile_level(const shiftmatrix& T) {
  if(WDIM == 3) return T;
  if(GDIM == 3) return orthogonal_move(T, cgi.BODY);
  return at_smart_lof(T, 1.15);
  }

bool celldrawer::draw_shmup_monster() {
  using namespace shmup;
  #if CAP_SHAPES

  pair<mit, mit> p = 
    monstersAt.equal_range(c);
    
  if(p.first == p.second) return false;
  ld zlev = -geom3::factor_to_lev(zlevel(tC0(Vd.T)));
   
  vector<monster*> monsters;

  for(mit it = p.first; it != p.second; it++) {
    monster* m = it->second;
    if(c != m->base) continue; // may happen in RogueViz Collatz
    m->pat = ggmatrix(m->base) * m->at;
    shiftmatrix view = V * m->at;

    bool half_elliptic = elliptic && GDIM == 3 && WDIM == 2;
    bool mirrored = det(view.T) > 0;
    if(half_elliptic && mirrored) continue;
    
    if(!mouseout()) {
      if(m->no_targetting) ; else
      if(mapeditor::drawplayer || m->type != moPlayer)
      if(!mousetarget || sqdist(mouseh, mousetarget->pat*C0) > sqdist(mouseh, m->pat*C0)) 
        mousetarget = m;
      }
    
    if(m->inBoat) {
      view = m->pat;
      if(WDIM == 2) Vboat = view;
      if(WDIM == 3) Vboat = view * spin270();
      
      bool magic = m->type == moPlayer && items[itOrbWater];
      color_t outcolor = magic ? watercolor(0) : 0xC06000FF;
      color_t incolor = magic ? 0x0060C0FF : 0x804000FF;
      
      if(WDIM == 2) {
        queuepoly(Vboat, cgi.shBoatOuter, outcolor);
        queuepoly(Vboat, cgi.shBoatInner, incolor);
        }
      if(WDIM == 3) {
        queuepoly(scale_matrix(Vboat, cgi.scalefactor/2), cgi.shBoatOuter, outcolor);
        queuepoly(scale_matrix(Vboat, cgi.scalefactor/2-0.01), cgi.shBoatInner, incolor);
        }
      }

    if(doHighlight())
      poly_outline = 
        isBullet(m) ? 0x00FFFFFF :
        (isFriendly(m->type) || m->type == moPlayer) ? 0x00FF00FF : 0xFF0000FF;

    int q = isize(ptds);
    if(q != isize(ptds) && !m->inBoat) pushdown(c, q, view, zlev, true, false);

    if(callhandlers(false, hooks_draw, V, c, m)) continue;

    switch(m->type) {
      case moPlayer: {
        playerfound = true;

        dynamicval<int> d(cpid, m->pid);
        
        bool h = hide_player();
        bool ths = subscreens::is_current_player(m->pid);
        
        if(!ths || !h) {
          drawPlayerEffects(view, V, c, m->type);
          if(WDIM == 3) {
            if(gproduct) {
              hyperpoint h = m->ori * C0; // ztangent(1)
              view = view * spin(-atan2(h[1], h[0]));
              }
            else {
              view = view * spin270() * cspin90(2, 0);
              }
            }
          if(m->inBoat) m->footphase = 0;
          if(mapeditor::drawplayer) {
            if(m->fragoff > curtime)
              drawShield(view, itWarning);
            drawMonsterType(moPlayer, c, view, 0xFFFFFFC0, m->footphase, 0xFFFFFFC0);
            }
          }

        if(ths && h) first_cell_to_draw = false;

        if(ths && h && WDIM == 3) {
          if(items[itOrbSword])
            queuestr(swordpos(m->pid, false, 1), vid.fsize * 2, "+", iinf[itOrbSword].color);
          if(items[itOrbSword2])
            queuestr(swordpos(m->pid, true, 1), vid.fsize * 2, "+", iinf[itOrbSword2].color);
          }

        if(ths && keyresult[cpid]) {
          shiftpoint h = keytarget(cpid);
          if(WDIM == 2) 
            queuestr(h, vid.fsize, "+", iinf[keyresult[cpid]].color);
          else {
            dynamicval<color_t> p(poly_outline, darkena(iinf[keyresult[cpid]].color, 0, 255));
            queuepoly(rgpushxto0(h) * cspin(0, 1, ticks / 140.), cgi.shGem[1], 0);
            }
          }

        break;
        }
      case moBullet: {
        color_t col;
        cpid = m->pid;
        if(m->get_parenttype() == moPlayer)
          col = getcs().swordcolor;
        else if(m->get_parenttype() == moMimic)
          col = (mirrorcolor(det(view.T) < 0) << 8) | 0xFF;
        else
          col = (minf[m->get_parenttype()].color << 8) | 0xFF;
        if(getcs().charid >= 4) {
          queuepoly(at_missile_level(view), cgi.shPHead, col);
          ShadowV(view, cgi.shPHead);
          }
        else if(peace::on) {
          queuepolyat(at_missile_level(view), cgi.shDisk, col, PPR::MISSILE);
          ShadowV(view, cgi.shPHead);
          }
        else {
          shiftmatrix t = view * spin(curtime / 50.0);
          queuepoly(at_missile_level(t), cgi.shKnife, col);
          ShadowV(t, cgi.shKnife);
          }
        break;
        }
      case moArrowTrap: {
        queuepoly(at_missile_level(view), cgi.shTrapArrow, 0xFFFFFFFF);
        ShadowV(view, cgi.shTrapArrow);
        break;
        }
      case moTongue: {
        queuepoly(at_missile_level(view), cgi.shTongue, (minf[m->get_parenttype()].color << 8) | 0xFF);
        ShadowV(view, cgi.shTongue);
        break;
        }
      case moFireball:  case moAirball: { // case moLightningBolt:
        queuepoly(at_missile_level(view), cgi.shPHead, (minf[m->type].color << 8) | 0xFF);
        ShadowV(view, cgi.shPHead);
        break;
        }
      case moFlailBullet: case moCrushball: {
        shiftmatrix t = view * spin(curtime / 50.0);
        queuepoly(at_missile_level(t), cgi.shFlailMissile, (minf[m->type].color << 8) | 0xFF);
        ShadowV(view, cgi.shFlailMissile);
        break;        
        }
      case moAsteroid: {
        if(GDIM == 3) addradar(view, '*', 0xFFFFFF, 0xC0C0C0FF);
        shiftmatrix t = view;
        if(WDIM == 3) t = face_the_player(t);
        t = t * spin(curtime / 500.0);
        ShadowV(t, cgi.shAsteroid[m->hitpoints & 7]);
        t = at_missile_level(t);
        color_t col = WDIM == 3 ? 0xFFFFFF : minf[m->type].color;
        col <<= 8;
        queuepoly(t, cgi.shAsteroid[m->hitpoints & 7], col | 0xFF);
        break;
        }

      default:
        if(WDIM == 3) {
          if(gproduct) {
            hyperpoint h = m->ori * xtangent(1);
            view = view * spin(-atan2(h[1], h[0]));
            }
          }
        if(m->inBoat) m->footphase = 0;
        color_t col = minf[m->type].color;
        if(m->type == moMimic) 
          col = mirrorcolor(det(view.T) < 0);
        if(m->type == moSlime) {
          col = winf[c->wall].color;
          col |= (col >> 1);
          }
        cpid = protect_pid(m->pid); 
        if(m->stunoff > curtime)
          c->stuntime = 1 + (m->stunoff - curtime-1)/300;
        if(hasHitpoints(m->type))
          c->hitpoints = m->hitpoints;
        if(m->type == moTortoise) tortoise::emap[c] = getBits(m->torigin) & ((1<<tortoise::numbits)-1);
        drawMonsterType(m->type, c, view, col, m->footphase, col);
        if(m->type == moTortoise) tortoise::emap.erase(c);
        break;
      }
    }

  #endif
  return false;
  }

}
