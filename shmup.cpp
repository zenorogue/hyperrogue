#include <map>

const char *lastprofile = "";
int lt = 0;

#ifndef MOBILE
void profile(const char *buf) {
  int gt = SDL_GetTicks();
  printf("%4d %s\n", gt - lt, lastprofile);
  lt = gt;
  lastprofile = buf;
  }
#endif
                                                                             
bool drawMonsterType(eMonster m, cell *where, const transmatrix& V, int col);
void drawPlayerEffects(const transmatrix& V, cell *c);

namespace shmup {

long double fabsl(long double x) { return x>0?x:-x; }

enum pcmds {
  pcForward, pcBackward, pcTurnLeft, pcTurnRight,
  pcMoveUp, pcMoveRight, pcMoveDown, pcMoveLeft,
  pcFire, pcFace, pcFaceFire,
  pcDrop, pcCenter, pcOrbPower
  };
  
const char* playercmds[14] = {
  "forward", "backward", "turn left", "turn right",
  "move up", "move right", "move down", "move left", 
  "throw a knife", "face the pointer", "throw at the pointer", 
  "drop Dead Orb", "center the map on me", "activate Orb power"
  };

const char* pancmds[7] = {
  "pan up", "pan right", "pan down", "pan left",
  "rotate left", "rotate right", "home"
  };

#define SHMUPAXES 12

const char* axemodes[SHMUPAXES] = { 
  "do nothing", 
  "rotate view",
  "panning X",
  "panning Y",
  "player 1 X", 
  "player 1 Y", 
  "player 1 go", 
  "player 1 spin", 
  "player 2 X", 
  "player 2 Y", 
  "player 2 go", 
  "player 2 spin"
  };

int players;
int cpid; // player id -- an extra parameter for player-related functions
int centerplayer = -1;

bool on = false, safety = false;

bool lastdead = false;

struct monster;

multimap<cell*, monster*> monstersAt;

typedef multimap<cell*, monster*>::iterator mit;

vector<monster*> active;

map<cell*, transmatrix> gmatrix;

cell *findbaseAround(hyperpoint p, cell *around) {
  cell *best = around;
  double d0 = intval(p, gmatrix[around] * C0);
  for(int i=0; i<around->type; i++) {
    cell *c2 = around->mov[i];
    if(c2 && gmatrix.count(c2)) {
      double d1 = intval(p, gmatrix[c2] * C0);
      if(d1 < d0) { best = c2; d0 = d1; }
      }
    }
  return best;
  }

/* double distance(hyperpoint h) {
  h = spintox(h) * h;
  return inverse_sinh(h[2]);
  } */

struct monster {
  eMonster type;
  cell *base;
  transmatrix at;
  transmatrix pat;
  eMonster stk;
  bool dead;
  bool inBoat;
  monster *parent; // who shot this missile
  eMonster parenttype; // type of the parent
  int nextshot;    // when will it be able to shot (players/flailers)
  char pid;         // player ID
  char hitpoints;
  int stunoff;
  int blowoff;
  double vel;      // velocity, for flail balls
  
  monster() { 
    dead = false; inBoat = false; parent = NULL; nextshot = 0; 
    stunoff = 0; blowoff = 0;
    }

  void store() {
    monstersAt.insert(make_pair(base, this));
    }
    
  void findpat() {
    pat = gmatrix[base] * at;
    }

  cell *findbase(hyperpoint p) {
    return findbaseAround(p, base);
    }
  
  void rebasePat(transmatrix new_pat) {
    pat = new_pat;
    cell *c2 = findbase(pat*C0);
    // if(c2 != base) printf("rebase %p -> %p\n", base, c2);
    base = c2;
    at = inverse(gmatrix[c2]) * pat;
    fixmatrix(at);
    }

  void rebaseAt(transmatrix new_at) {
    rebasePat(gmatrix[base] * new_at);
    }

  bool trackroute(transmatrix goal, double spd) {
    cell *c = base;
    
    // queuepoly(goal, shGrail, 0xFFFFFFC0);

    transmatrix mat = inverse(pat) * goal;
    
    transmatrix mat2 = spintox(mat*C0) * mat;
    
    double d = 0, dist = inverse_sinh(mat2[0][2]);

    while(d < dist) {
      d += spd;
      transmatrix nat = pat * rspintox(mat * C0) * xpush(d); 

      // queuepoly(nat, shKnife, 0xFFFFFFC0);

      cell *c2 = findbaseAround(nat*C0, c);
      if(c2 != c && !passable_for(type, c2, c, false)) {
        return false;
        }
      c = c2;
      }
    return true;
    }

  bool trackrouteView(transmatrix goal, double spd) {
    cell *c = base;
    
    queuepoly(goal, shGrail, 0xFFFFFFC0);

    transmatrix mat = inverse(pat) * goal;
    
    transmatrix mat2 = spintox(mat*C0) * mat;
    
    double d = 0, dist = inverse_sinh(mat2[0][2]);

    while(d < dist) {
      d += spd;
      transmatrix nat = pat * rspintox(mat * C0) * xpush(d); 

      // queuepoly(nat, shKnife, 0xFFFFFFC0);

      cell *c2 = findbaseAround(nat*C0, c);
      if(c2 != c) {
        if(0) printf("old dist: %lf\n", (double) intval(nat*C0, gmatrix[c]*C0));
        if(0) printf("new dist: %lf\n", (double) intval(nat*C0, gmatrix[c2]*C0));
        }
      queuepoly(gmatrix[c2], shKnife, 0xFF0000FF);
      if(c2 != c && !passable_for(type, c2, c, false))
        return false;
      c = c2;
      }
    if(0) printf("dist = %lf, d = %lf, spd = %lf, lint = %lf, lcd = %lf\n", dist, d, spd, 
      (double) intval(pat * rspintox(mat * C0) * xpush(d)*C0, goal*C0),
      (double) intval(pat * rspintox(mat * C0) * xpush(d)*C0, gmatrix[c]*C0)
      );
    return true;
    }

  };

monster *pc[2], *mousetarget;

int curtime, nextmove, invisible_at;

bool isBullet(monster *m) { 
  return 
    m->type == moBullet || m->type == moFlailBullet || 
    m->type == moFireball || m->type == moTongue || m->type == moAirball; 
  }
bool isPlayer(monster *m) { return m->type == moPlayer; }
bool isMonster(monster *m) { return m->type != moPlayer && m->type != moBullet; }

void killMonster(monster* m) {
  if(m->dead) return;
  m->dead = true;
  if(isBullet(m) || isPlayer(m)) return;
  m->stk = m->base->monst;
  if(m->inBoat && isWatery(m->base) && !m->base->item) {
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
  killMonster(m->base);
  m->base->monst = m->stk;
  }

void pushmonsters() {
  for(int i=0; i<size(active); i++) {
    monster *m = active[i];
    if(!isPlayer(m) && !m->dead) {
      m->stk = m->base->monst;
      m->base->monst = m->type;
      }
    }
  }

void popmonsters() {
  for(int i=size(active)-1; i>=0; i--) {
    monster *m = active[i];
    if(!isPlayer(m) && !m->dead) {
      if(m->type == m->base->monst)
        m->base->monst = m->stk;
      else {
        m->dead = true; // already killed
        // also kill all the other monsters there
        for(int j=0; j<i; j++) {
          monster *m2 = active[j];
          if(m2->base == m->base && !isPlayer(m2) && !m2->dead) 
            killMonster(m2);
          }
        }
      }
    }
  }

void degradeDemons() {
  for(int i=0; i<size(active); i++) {
    monster *m = active[i];
    if(m->type == moGreater) m->type = moLesser;
    if(m->stk == moGreater) m->type = moLesser;
    }
  }

// we need these for the Mimics!
double playerturn[4], playergo[4];
bool playerfire[4];

void awakenMimics(monster *m, cell *c2) {
  for(int i=0; i<size(dcal); i++) {
    cell *c = dcal[i];
    if(isMimic(c->monst)) {
      // straight
      int i = 0;
      if(m->type == moMirror) i++;
      if(c->monst == moMirror) i++;
      
      transmatrix mirrortrans = Id;    
      if(i == 1) mirrortrans[0][0] = -1;

      if(!gmatrix.count(c)) continue;
      monster *m2 = new monster;
      m2->type = c->monst;
      c->monst = moNone;
      m2->base = c;
      
      hyperpoint H = inverse(gmatrix[c2]) * gmatrix[c] * C0;
      
      transmatrix xfer = rgpushxto0(H);

      if(i == 1) {
        hyperpoint H2 = spintox(H) * H;
        xfer = rspintox(H) * rpushxto0(H2) * mirrortrans * spintox(H);
        }

      m2->pat = gmatrix[c2] * xfer * inverse(gmatrix[c2]) * m->pat;
        
      m2->at = inverse(gmatrix[c]) * m2->pat * mirrortrans;
      m2->pid = cpid;
      active.push_back(m2);

      // if you don't understand it, don't worry,
      // I don't understand it either
      }
    }
  }

int visibleAt;

void visibleFor(int t) {
  visibleAt = max(visibleAt, curtime + t);
  }

void shootBullet(monster *m) {
  monster* bullet = new monster;
  bullet->base = m->base;
  bullet->at = m->at;
  bullet->type = moBullet;
  bullet->parent = m;
  bullet->pid = m->pid;
  bullet->parenttype = m->type;
  active.push_back(bullet);
  
  if(markOrb(itOrbThorns)) {
    monster* bullet = new monster;
    bullet->base = m->base;
    bullet->at = m->at * spin(M_PI/2);
    bullet->type = moBullet;
    bullet->parent = m;
    bullet->pid = m->pid;
    bullet->parenttype = m->type;
    active.push_back(bullet);
    }

  if(markOrb(itOrbThorns)) {
    monster* bullet = new monster;
    bullet->base = m->base;
    bullet->at = m->at * spin(-M_PI/2);
    bullet->type = moBullet;
    bullet->parent = m;
    bullet->pid = m->pid;
    bullet->parenttype = m->type;
    active.push_back(bullet);
    }
  
/*   if(markOrb(itOrbTrickery)) {
    monster* bullet = new monster;
    bullet->base = m->base;
    bullet->at = m->at * spin(M_PI);
    bullet->type = moBullet;
    bullet->parent = m;
    bullet->parenttype = m->type;
    active.push_back(bullet);
    } */
  }

void killThePlayer(eMonster m) {
  pc[cpid]->dead = true;
  }

#ifndef MOBILE

void oceanCurrents(transmatrix& nat, monster *m, int delta) {
  cell *c = m->base;
  if(c->land == laWhirlpool) {
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->mov[i];
      if(!c2 || !gmatrix.count(c2)) continue;
      
      double spd = 0;
      
      if(celldistAlt(c2) < celldistAlt(c)) 
        spd = delta / 3000.;
      else if(c2 == getWhirlpool(c, 1))
        spd = delta / 900.;
        
      if(spd) {
        transmatrix goal = gmatrix[c2];

        // transmatrix t = spintox(H) * xpush(delta/300.) * rspintox(H);

        hyperpoint H = inverse(m->pat) * goal * C0;
        nat = nat * rspintox(H);
        nat = nat * xpush(spd);
        nat = nat * spintox(H);
        }
      }
    }
  }

int actionspressed[64], axespressed[SHMUPAXES], lactionpressed[64];

void pressaction(int id) {
  if(id >= 0 && id < 64)
    actionspressed[id]++;
  }

void handleInput(int delta) {
  double d = delta / 500.;

  Uint8 *keystate = SDL_GetKeyState(NULL);

  for(int i=0; i<64; i++) 
    lactionpressed[i] = actionspressed[i],
    actionspressed[i] = 0;

  for(int i=0; i<SHMUPAXES; i++) axespressed[i] = 0;
  
  for(int i=0; i<SDLK_LAST; i++) if(keystate[i]) 
    pressaction(vid.scfg.keyaction[i]);
  
  for(int j=0; j<numsticks; j++) {

    for(int b=0; b<SDL_JoystickNumButtons(sticks[j]) && b<MAXBUTTON; b++)
      if(SDL_JoystickGetButton(sticks[j], b))
        pressaction(vid.scfg.joyaction[j][b]);

    for(int b=0; b<SDL_JoystickNumHats(sticks[j]) && b<MAXHAT; b++) {
      int stat = SDL_JoystickGetHat(sticks[j], b);
      if(stat & SDL_HAT_UP) pressaction(vid.scfg.hataction[j][b][0]);
      if(stat & SDL_HAT_RIGHT) pressaction(vid.scfg.hataction[j][b][1]);
      if(stat & SDL_HAT_DOWN) pressaction(vid.scfg.hataction[j][b][2]);
      if(stat & SDL_HAT_LEFT) pressaction(vid.scfg.hataction[j][b][3]);
      }
    
    for(int b=0; b<SDL_JoystickNumAxes(sticks[j]) && b<MAXAXE; b++) {
      int value = SDL_JoystickGetAxis(sticks[j], b);
      axespressed[vid.scfg.axeaction[j][b] % SHMUPAXES] += value;
      }
    }

  if(keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL]) d /= 5;
  
  double panx = 
    actionspressed[49] - actionspressed[51] + axespressed[2] / 32000.0;
  double pany = 
    actionspressed[48] - actionspressed[50] + axespressed[3] / 20000.0;
    
  double panspin = actionspressed[52] - actionspressed[53] + axespressed[1] / 20000.0;
  
  if(actionspressed[54]) { centerplayer = -1, playermoved = true; centerpc(100); }
  
  if(panx || pany || panspin) {
    View = xpush(-panx * d) * ypush(-pany * d) * spin(panspin * d) * View;
    playermoved = false;
    }
  }
        
void movePlayer(monster *m, int delta) {
  
  cpid = m->pid;
  if(players > 1) vid.female = (cpid == 1);
  
  double mturn = 0, mgo = 0, mdx = 0, mdy = 0;
  
  bool shotkey = false, facemouse = false, dropgreen = false;
  
  int b = 16*(cpid+1);
    for(int i=0; i<8; i++) if(actionspressed[b+i]) playermoved = true;
  
  int jb = 4*(cpid+1);
  for(int i=0; i<4; i++) if(axespressed[jb+i]) playermoved = true;
  
  mgo = actionspressed[b+pcForward] - actionspressed[b+pcBackward] + axespressed[jb+2]/30000.;
  mturn = actionspressed[b+pcTurnLeft] - actionspressed[b+pcTurnRight] + axespressed[jb+3]/30000.;
  mdx = actionspressed[b+pcMoveRight] - actionspressed[b+pcMoveLeft] + axespressed[jb]/30000.;
  mdy = actionspressed[b+pcMoveDown] - actionspressed[b+pcMoveUp] + axespressed[jb+1]/30000.;
  
  shotkey = actionspressed[b+pcFire] || actionspressed[b+pcFaceFire];
  facemouse = actionspressed[b+pcFace] || actionspressed[b+pcFaceFire];
  dropgreen = actionspressed[b+pcDrop];
  
  if(actionspressed[b+pcOrbPower] && !lactionpressed[b+pcOrbPower]) {
    cwt.c = m->base;
    targetRangedOrb(mouseover, roKeyboard);
    }
    
  if(actionspressed[b+pcCenter]) {
    centerplayer = cpid; centerpc(100); playermoved = true; 
    }
  
  transmatrix nat = m->pat;
  
  if(m->base->land == laWhirlpool && !markOrb(itOrbWater)) 
    oceanCurrents(nat, m, delta);
  
  // if(ka == b+pcOrbPower) dropgreen = true;
  
  // if(mturn > 1) mturn = 1;
  // if(mturn < -1) mturn = -1;
  
  playerturn[cpid] = mturn * delta / 150.0;

  double mdd = hypot(mdx, mdy);
  if(mdd > 1e-6) {
    hyperpoint jh = hpxy(mdx/100.0, mdy/100.0);

    hyperpoint h = inverse(m->pat) * rgpushxto0(m->pat * C0) * jh;
    
    playerturn[cpid] = -atan2(h[1], h[0]);
    mgo += mdd;
    }

  Uint8 *keystate = SDL_GetKeyState(NULL);
  bool forcetarget = (keystate[SDLK_RSHIFT] | keystate[SDLK_LSHIFT]);
  if(((mousepressed && !forcetarget) || facemouse) && delta > 0 && !outofmap(mouseh)) {
    // playermoved = true;
    hyperpoint h = inverse(m->pat) * mouseh;
    playerturn[cpid] = -atan2(h[1], h[0]);
    // nat = nat * spin(alpha);
    // mturn += alpha * 150. / delta;
    }

  bool blown = m->blowoff > curtime;
  
  if(playerturn[cpid] && canmove && !blown) nat = nat * spin(playerturn[cpid]);
  transmatrix nat0 = nat;
  
  if(mgo > 1) mgo = 1;
  if(mgo < -1) mgo = -1;
  if(!canmove) mgo = 0;
  
  playergo[cpid] = mgo * delta / 600;
  
  bool go = false; 
  
  cell *c2 = m->base;
  
  if(blown) {
    playergo[cpid] = -delta / 1000.;
    playerturn[cpid] = 0;
    }

  for(int igo=0; igo<9 && !go; igo++) {
  
    go = true;
    
    double span[9] = { 0, 
      M_PI/6, -M_PI/6, 
      M_PI/4, -M_PI/4,
      M_PI/3, -M_PI/3,
      M_PI/2.1, -M_PI/2.1
      };

    if(playergo[cpid]) nat = nat0 * spin(span[igo]) * xpush(playergo[cpid]) * spin(-span[igo]);

    // spin(span[igo]) * xpush(playergo[cpid]) * spin(-span[igo]);
  
    c2 = m->findbase(nat*C0);
    
    // don't have several players in one spot
    // also don't let them run too far from each other!
    monster* crashintomon = NULL;  
    for(int j=0; j<players; j++) if(pc[j]!=m) {
      double d = intval(pc[j]->pat*C0, nat*C0);
      if(d < 0.1 || d > 100) crashintomon = pc[j];
      }
    if(crashintomon) go = false;
  
    if(go && c2 != m->base) {

      if(c2->wall == waLake && markOrb(itOrbWinter)) {
        c2->wall = waFrozenLake;
        if(HEAT(c2) > .5) HEAT(c2) = .5;
        }
  
      else if(c2->wall == waBigStatue && canPushStatueOn(m->base)) {
        visibleFor(300);
        c2->wall = m->base->wall;
        if(cellUnstable(cwt.c))
          m->base->wall = waChasm;
        else
          m->base->wall = waBigStatue;
        }
      else if(m->inBoat && !isWateryOrBoat(c2) && player_passable(c2, m->base, false)) {
        if(boatGoesThrough(c2) && markOrb(itOrbWater)) {
          c2->wall = isIcyLand(m->base) ? waLake : waSea;
          }
        else {
          if(isWatery(m->base)) 
            m->base->wall = waBoat, m->base->mondir = dirfromto(m->base, c2);
          else if(boatStrandable(m->base))
            m->base->wall = waStrandedBoat;
          else if(boatStrandable(c2))
            m->base->wall = waStrandedBoat;
          m->inBoat = false;
          }
        }
      else if(c2->wall == waThumperOn) {
        int sd = dirfromto(c2, m->base);
        int subdir = 1;
        double bestd = 9999;
        for(int di=-1; di<2; di+=2) {
          cell *c = getMovR(c2, sd+di);
          if(!c) continue;
          if(!gmatrix.count(c)) continue;
          double d = intval(gmatrix[c] * C0, m->pat * C0);
          // printf("di=%d d=%lf\n", di, d);
          if(d<bestd) bestd=d, subdir = di;
          }
        visibleFor(300);
        cellwalker push(c2, dirfromto(c2, m->base));
        cwspin(push, 3 * -subdir); cwstep(push);
        if(!canPushThumperOn(push.c, c2, m->base) && c2->type == 7) {
          cwstep(push);
          cwspin(push, 1 * -subdir);
          cwstep(push);
          }
        if(!canPushThumperOn(push.c, c2, m->base)) {
          go = false;
          }
        else pushThumper(c2, push.c);
        }
      else if((blown ? !blowable(c2, m->base) : !player_passable(c2, m->base, false)) && !(isWatery(c2) && m->inBoat))
        go = false;
      
      }
    }
  
  if(go) {
    if(c2 != m->base) {
      if(cellUnstable(m->base)) m->base->wall = waChasm;
    
      if(items[itOrbFire]) {
        visibleFor(800);
        if(makeflame(m->base, 10, false)) markOrb(itOrbFire);
        }

      if(isIcyLand(m->base) && m->base->wall == waNone && markOrb(itOrbWinter)) {
        invismove = false;
        m->base->wall = waIcewall;
        }
      
      if(items[itOrbDigging]) {
        visibleFor(400);
        int d = dirfromto(m->base, c2);
        if(d >= 0 && earthMove(m->base, d)) markOrb(itOrbDigging);
        }
      
      setdist(c2, 0, NULL);
      if(c2->item && c2->land == laAlchemist) c2->wall = m->base->wall;
      if(m->base->wall == waRoundTable)
        roundTableMessage(c2);
      if(c2->wall == waCloud) {
        visibleFor(500);
        createMirages(c2, 0, moMirage);
        awakenMimics(m, c2);
        c2->wall = waNone;
        items[itShard]++;
        }
      if(c2->wall == waMirror) {
        visibleFor(500);
        cwt.c = c2;
        createMirrors(c2, 0, moMirage);
        awakenMimics(m, c2);
        c2->wall = waNone;
        items[itShard]++;
        }
      if(c2->wall == waGlass && items[itOrbGhost]) {
        items[itOrbGhost] = 0;
        addMessage(XLAT("Your Aether powers are drained by %the1!", c2->wall));
        }
      movecost(m->base, c2);

      bool nomine = (c2->wall == waMineMine || c2->wall == waMineUnknown)  && markOrb(itOrbGhost);
      
      if(!nomine) {
        uncoverMines(c2,
          items[itBombEgg] < 10 ? 0 :
          items[itBombEgg] < 20 ? 1 :
          items[itBombEgg] < 30 ? 2 :
          3
          );
        if(c2->wall == waMineMine && !markOrb(itOrbWinter)) {
          items[itOrbLife] = 0;
          m->dead = true;
          }
        explodeMine(c2);
        }
  
      if(c2->wall == waClosePlate || c2->wall == waOpenPlate)
        toggleGates(c2, c2->wall, 3);
  
      if(c2->item == itOrbYendor && !checkYendor(c2, false)) 
        ;
      else collectItem(c2);
      }
    }

  if(go) m->rebasePat(nat);
  else m->rebasePat(nat0);

  if(m->base->wall == waBoat && !m->inBoat) {
    m->inBoat = true; m->base->wall = waSea;
    }

  if(m->inBoat && boatStrandable(c2)) {
    c2->wall = waStrandedBoat;
    m->inBoat = false;
    }

  if(!markOrb(itOrbGhost)) {
    if(m->base->wall == waChasm || m->base->wall == waClosedGate)
      m->dead = true;

    if(isWatery(m->base) && !m->inBoat && !markOrb(itOrbFish))
      m->dead = true;

    if(isFire(m->base) && !markOrb(itOrbWinter))
      m->dead = true;
    }

  landvisited[m->base->land] = true;
  
  playerfire[cpid] = false;
  
  if(shotkey && canmove && curtime >= m->nextshot) {

    visibleFor(500);
    if(items[itOrbFlash]) {
      pushmonsters();
      killMonster(m->base);
      cwt.c = m->base;
      activateFlash();
      popmonsters();
      return;
      }
    
    if(items[itOrbLightning]) {
      pushmonsters();
      killMonster(m->base);
      cwt.c = m->base;
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
#endif

void moveMimic(monster *m) {
  transmatrix nat = m->pat;
  cpid = m->pid;
  
  // no need to care about Mirror images, as they already have their 'at' matrix reversed :|
  nat = nat * spin(playerturn[cpid]) * xpush(playergo[cpid]);

  cell *c2 = m->findbase(nat*C0);  
  if(c2 != m->base && !player_passable(c2, m->base, false))
    m->dead = true;
  else {
    m->rebasePat(nat);
    if(playerfire[cpid]) shootBullet(m);
    }  

  if(c2->wall == waCloud) {
    createMirages(c2, 0, m->type);
    awakenMimics(m, c2);
    c2->wall = waNone;
    }

  if(c2->wall == waMirror) {
    createMirrors(c2, 0, m->type);
    awakenMimics(m, c2);
    c2->wall = waNone;
    }

  if(c2->cpdist >= 6)
    m->dead = true;
  }

bool isPlayerOrImage(eMonster m) {
  return isMimic(m) || m == moPlayer;
  }

monster *parentOrSelf(monster *m) {
  return m->parent ? m->parent : m;
  }

bool bulletpassable(cell *c, cell *from) {
  if(c != from && thruVine(c, from)) return false;
  return eaglepassable(c, from) || c->wall == waRoundTable || 
    isFire(c) || c->wall == waBonfireOff || 
    cellHalfvine(c) || c->wall == waAncientGrave || c->wall == waFreshGrave;
  }
  
void teleported() {
  monster *m = pc[cpid];
  m->base = cwt.c;
  m->at = rgpushxto0(inverse(gmatrix[cwt.c]) * mouseh) * spin(rand() % 1000 * M_PI / 2000);
  m->findpat();
  for(int i=0; i<size(active); i++)
    if(isMimic(active[i]->type)) 
      active[i]->dead = true;
  }

void shoot(eItem it, monster *m) {
  monster* bullet = new monster;
  bullet->base = m->base;
  bullet->at = m->at * rspintox(inverse(m->pat) * mouseh);
  bullet->type = it == itOrbDragon ? moFireball : it == itOrbAir ? moAirball : moBullet;
  bullet->parent = m;
  bullet->pid = m->pid;
  bullet->parenttype = m->type;
  items[it]--;
  active.push_back(bullet);
  }

eItem targetRangedOrb(orbAction a) {
  if(!on) return itNone;
  monster *wpc = pc[cpid];
  if(a != roCheck && !wpc) return itNone;  
  
  if(items[itOrbPsi] && shmup::mousetarget && intval(mouseh, shmup::mousetarget->pat*C0) < .1) {
    if(a == roCheck) return itOrbPsi;
    addMessage(XLAT("You kill %the1 with a mental blast!", mousetarget->type));
    killMonster(mousetarget);
    items[itOrbPsi] -= 30;
    if(items[itOrbPsi]<0) items[itOrbPsi] = 0;
    return itOrbPsi;
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

void moveBullet(monster *m, int delta) {
  vid.female = (m->pid == 1);
  m->findpat();
  transmatrix nat0 = m->pat;
  transmatrix nat = m->pat;
  if(m->type == moFlailBullet) {
    m->vel -= delta / 600000.0;
    if(m->vel < 0 && m->parent) {
      // return to the flailer!
      nat = nat * rspintox(inverse(m->pat) * m->parent->pat * C0) * spin(M_PI);
      }
    }
  else if(m->type == moBullet)
    m->vel = 1/300.;
  else if(m->type == moFireball)
    m->vel = 1/500.;
  else if(m->type == moAirball)
    m->vel = 1/200.;
  else if(m->type == moTongue) {
    m->vel = 1/1500.;
    if(!m->parent || intval(nat*C0, m->parent->pat*C0) > 0.4)
      m->dead = true;
    }
  nat = nat * xpush(delta * m->vel);
  cell *c2 = m->findbase(nat*C0);

  if(isActivable(c2)) activateActiv(c2, true);
  
  // knives break mirrors and clouds
  if(c2->wall == waCloud || c2->wall == waMirror)
    c2->wall = waNone;
  
  if(m->type != moTongue && !bulletpassable(c2, m->base)) {
    m->dead = true;
    if(m->type != moAirball) killMonster(c2);
    // cell *c = m->base;
    if(m->parent && isPlayer(m->parent)) {
      if(c2->wall == waDryTree) {
        addMessage(XLAT("You start cutting down the tree."));
        c2->wall = waWetTree;
        }
      else if(c2->wall == waWetTree) {
        addMessage(XLAT("You cut down the tree."));
        c2->wall = waNone;
        }
      else if(isActivable(c2)) 
        activateActiv(c2, true);
      }
    if(m->type == moFireball) {
      makeflame(c2, 20, false) || makeflame(m->base, 20, false);
      }
    }
  m->rebasePat(nat);
  
  // destroy stray bullets
  for(int i=0; i<m->base->type; i++) 
    if(!m->base->mov[i] || !gmatrix.count(m->base->mov[i]))
      m->dead = true;

  // items[itOrbWinter] = 100; items[itOrbLife] = 100;
  
  for(int j=0; j<size(active); j++) {
    monster* m2 = active[j];
    if(m2 == m || (m2 == m->parent && m->vel >= 0) || m2->parent == m->parent) continue;
    
    // Flailers only killable by themselves
    if(m2->type == moFlailer && m2 != m->parent) continue;
    // be nice to your images! would be too hard otherwise...
    if(isPlayerOrImage(parentOrSelf(m)->type) && isPlayerOrImage(parentOrSelf(m2)->type) &&
      m2->pid == m->pid)
      continue;
    // fireballs/airballs don't collide
    if(m->type == moFireball && m2->type == moFireball) continue;
    if(m->type == moAirball && m2->type == moAirball) continue;
    double d = intval(m2->pat*C0, m->pat*C0);
    
    if(d < 0.1) {
      if(m->type == moAirball && isBlowableMonster(m2->type)) {

        if(m2->blowoff < curtime)
          m2->rebasePat(m2->pat * rspintox(inverse(m2->pat) * nat0 * C0));
        m2->blowoff = curtime + 1000;
        continue;
        }
      // Hedgehog Warriors only killable outside of the 45 degree angle
      if(m2->type == moHedge) {
        hyperpoint h = inverse(m2->pat) * m->pat * C0;
        if(h[0] > fabsl(h[1])) { m->dead = true; continue; }
        }
      // 
      if((m2->type == moPalace || m2->type == moFatGuard || m2->type == moSkeleton ||
        m2->type == moVizier) && m2->hitpoints > 1) {
        m2->rebasePat(m2->pat * rspintox(inverse(m2->pat) * nat0 * C0));
        if(m2->type != moSkeleton) m2->hitpoints--;
        m->dead = true;
        if(m2->type == moVizier) ;
        else if(m2->type == moFatGuard)
          m2->stunoff = curtime + 600;
        else if(m2->type == moSkeleton && m2->base->land != laPalace)
          m2->stunoff = curtime + 2100;
        else
          m2->stunoff = curtime + 900;
        continue;
        }
      // Greater Demons not killable
      if(m2->type == moGreater) {
        m->dead = true;
        continue;
        }
      // Knights reflect bullets
      if(m2->type == moKnight) {
        if(m->parent) {
          nat = nat * rspintox(inverse(m->pat) * m->parent->pat * C0);
          m->rebasePat(nat);
          }
        m->parent = m2;
        continue;
        }
      m->dead = true;
      if(m->type == moFireball) makeflame(m->base, 20, false);
      // Orb of Winter protects from fireballs
      if(m->type == moFireball && ((isPlayer(m2) && markOrb(itOrbWinter)) || m2->type == moWitchWinter)) 
        continue;
      killMonster(m2);
      }
    }
  }

hyperpoint closerTo;

bool closer(monster *m1, monster *m2) {
  return intval(m1->pat*C0,  closerTo) < intval(m2->pat*C0, closerTo);
  }

void moveMonster(monster *m, int delta) {

  bool stunned = (isStunnable(m->type) && m->stunoff > curtime) || m->blowoff > curtime;
  
  if(stunned && cellUnstable(m->base))
    m->base->wall = waChasm;
  
  if(m->base->wall == waChasm && !survivesChasm(m->type))
    killMonster(m);

  if(isWatery(m->base) && !survivesWater(m->type) && !m->inBoat)
    killMonster(m);

  if(isFire(m->base) && !survivesFire(m->type))
    killMonster(m);

  if(m->base->wall == waClosedGate && !survivesWall(m->type))
    killMonster(m);

  if(m->dead) return;
  
  
  cell *c = m->base;
  transmatrix goal = gmatrix[c];
  
  bool direct = false;
  
  double step = delta/1000.0;
  if(m->type == moWitchSpeed)
    step *= 2;
  else if(m->type == moEagle)
    step *= 1.6;
  else if(m->type == moLancer)
    step *= 1.25;
  else if(isDemon(m->type)) {
    if(m->type == moLesserM) m->type = moLesser;
    if(m->type == moGreaterM) m->type = moGreater;
    step /= 2;
    }
  
  transmatrix nat = m->pat;

  if(stunned) {
    if(m->blowoff > curtime) {
      step = -delta / 1000.;
      }
    else if(m->type == moFatGuard)
      step = 0;
    else step = -delta/2000.;
    }

  else {
    
    if(m->type == moWitchFlash) for(int pid=0; pid<players; pid++) {
      bool okay = intval(pc[pid]->pat*C0, m->pat*C0) < 2;
      for(int i=0; i<size(active); i++) {
        monster *m2 = active[i];
        if(m2 != m && isWitch(m2->type) && intval(m2->pat*C0, m->pat*C0) < 2)
          okay = false;
        }
      if(okay) {
        addMessage(XLAT("%The1 activates her Flash spell!", m->type));
        pushmonsters();
        activateFlashFrom(m->base);
        popmonsters();
        m->type = moWitch;
        pc[pid]->dead = true;
        }
      }
    if(isBug(m->type)) {
      vector<monster*> bugtargets;
      for(int i=0; i<size(active); i++)
        if(!isBullet(active[i]))
        if(active[i]->type != m->type)
        if(!isPlayer(active[i]) || !invismove)
        if(!active[i]->dead)
          bugtargets.push_back(active[i]);
      
      closerTo = m->pat * C0;
      sort(bugtargets.begin(), bugtargets.end(), closer);
  
      for(int i=0; i<size(bugtargets); i++)
        if(m->trackroute(bugtargets[i]->pat, step)) {
          goal = bugtargets[i]->pat;
          direct = true;
          break;
          }
      }
    else if(m->type == moWolf) {
      cell *cnext = c;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if(c2 && gmatrix.count(c2) && HEAT(c2) > HEAT(c) && isIcyLand(c2) &&
          passable(c2, c, false, false, false))
          cnext = c2;
        }
      goal = gmatrix[cnext];
      direct = true;
      }
    else if(!direct && !invismove) {
      for(int i=0; i<players; i++) 
        if(m->trackroute(pc[i]->pat, step) && (!direct || intval(pc[i]->pat*C0, m->pat*C0) < intval(goal*C0,m->pat*C0))) {
          goal = pc[i]->pat;
          direct = true;
          // m->trackrouteView(pc->pat, step);
          }
        }
  
    if(!direct) while(true) {
      if(m->trackroute(gmatrix[c], step))
        goal = gmatrix[c];
      cell *cnext = c;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if(c2 && gmatrix.count(c2) && c2->pathdist < c->pathdist &&
          passable_for(m->type, c2, c, false))
          cnext = c2;
        }
      if(cnext == c) break;
      c = cnext;
      }

    if(m->type == moHedge) {
      hyperpoint h = inverse(m->pat) * goal * C0;
      if(h[1] < 0)
        nat = nat * spin(M_PI * delta / 3000);
      else
        nat = nat * spin(M_PI * -delta / 3000);
      m->rebasePat(nat);
      // at most 45 degrees
      if(h[0] < fabsl(h[1])) return;
      }
    else {
      nat = nat * rspintox(inverse(m->pat) * goal * C0);
      }
    }
  
//if(c->land == laWhirlpool && (m->type == moShark || m->type == moCShark || m->type == moPirate))
//  oceanCurrents(nat, m, delta);
  

  if(items[itOrbSpeed])
    step /= 2;
  
  nat = nat * xpush(step); // * spintox(wherePC);
  if(intval(nat*C0, goal*C0) >= intval(m->pat*C0, goal*C0) && !stunned) 
    return;

  monster* crashintomon = NULL;
  
  for(int j=0; j<size(active); j++) if(active[j]!=m && active[j]->type != moBullet) {
    monster* m2 = active[j];
    double d = intval(m2->pat*C0, nat*C0);
    if(d < 0.1) crashintomon = active[j];
    }
  
  for(int i=0; i<players; i++) if(crashintomon == pc[i]) pc[i]->dead = true;

  else if(crashintomon && isMimic(crashintomon->type)) {
    crashintomon->dead = true;
    crashintomon = NULL;
    }
  
  else if(crashintomon && (isBug(m->type) || isBug(crashintomon->type)) && m->type != crashintomon->type &&
    !isBullet(crashintomon)) {
    crashintomon->dead = true;
    crashintomon = NULL;
    }
  
  if(crashintomon) return;

  cell *c2 = m->findbase(nat*C0);
  
  if(isPlayerOn(c2)) {
    bool usetongue = false;
    if(isSlimeMover(m->type) || m->type == moWaterElemental) usetongue = true;
    if(isWatery(c2) && !survivesWater(m->type) && !m->inBoat) usetongue = true;
    if(c2->wall == waChasm && !survivesChasm(m->type)) usetongue = true;
    if(isFire(c2) && !survivesFire(m->type) && !m->inBoat) usetongue = true;
    if(usetongue) {
      if(curtime < m->nextshot) return;
      // m->nextshot = curtime + 25;
      monster* bullet = new monster;
      bullet->base = m->base;
      bullet->at = m->at;
      bullet->type = moTongue;
      bullet->parent = m;
      bullet->parenttype = m->type;
      active.push_back(bullet);
      return;
      }
    }

  if(c2 != m->base && (c2->wall == waClosePlate || c2->wall == waOpenPlate))
    toggleGates(c2, c2->wall, 3);
  
  if(c2 != m->base && c2->wall == waMineMine && !survivesMine(m->type)) {
    explodeMine(c2);
    m->dead = true;
    }

  if(c2 != m->base && cellUnstable(m->base) && m->type != moEagle && m->type != moGhost)
    m->base->wall = waChasm;
  if(c2 != m->base && m->type == moWitchFire) makeflame(m->base, 10, false);
  if(c2 != m->base && m->type == moFireElemental) makeflame(m->base, 20, false);
  if(c2 != m->base && m->type == moWaterElemental) placeWater(c2, m->base);
  if(c2 != m->base && m->type == moEarthElemental) {
    int d = dirfromto(m->base, c2);
    if(d >= 0) earthMove(m->base, d);
    }
  
  // to do necro

  if(c2 != m->base && m->type == moNecromancer && !c2->monst) {
    for(int i=0; i<m->base->type; i++) {
      cell *c3 = m->base->mov[i];
      if(dirfromto(c3, c2) != -1 && c3->wall == waFreshGrave && gmatrix.count(c3)) {
        bool monstersNear = false;
        for(int i=0; i<size(active); i++) {
          if(active[i] != m && intval(active[i]->pat*C0, gmatrix[c3]*C0) < .3)
            monstersNear = true;
          if(active[i] != m && intval(active[i]->pat*C0, gmatrix[c2]*C0) < .3)
            monstersNear = true;
          }
        if(!monstersNear) {

          monster* undead = new monster;
          undead->base = c2;
          undead->at = Id;
          undead->type = moZombie;
          undead->parent = m;
          undead->parenttype = m->type;
          undead->findpat();
          active.push_back(undead);

          undead = new monster;
          undead->base = c3;
          undead->at = Id;
          undead->type = moGhost;
          undead->parent = m;
          undead->parenttype = m->type;
          undead->findpat();
          active.push_back(undead);

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
      if(c2->wall == waBigStatue && canPushStatueOn(m->base)) {
        c2->wall = m->base->wall;
        if(cellUnstable(m->base))
          m->base->wall = waChasm;
        else
          m->base->wall = waBigStatue;
        }
      if(passable_for(m->type, c2, m->base, false) && !isWatery(c2) && m->inBoat) {
        if(isWatery(m->base)) 
          m->base->wall = waBoat, m->base->mondir = dirfromto(m->base, c2);
        else if(boatStrandable(c2)) c2->wall = waStrandedBoat;
        else if(boatStrandable(m->base)) m->base->wall = waStrandedBoat;
        m->inBoat = false;
        }
      }
    if(c2->wall == waBoat && !m->inBoat) {
      m->inBoat = true; c2->wall = waSea;
      m->base = c2;
      }
    }

  if(stunned ? blowable(c2, m->base) : passable_for(m->type, c2, m->base, false)) 
    m->rebasePat(nat);

  if(direct) {
    if((m->type == moPyroCultist || m->type == moCrystalSage) && curtime >= m->nextshot) {
      monster* bullet = new monster;
      bullet->base = m->base;
      bullet->at = m->at;
      bullet->type = moFireball;
      bullet->parent = m;
      active.push_back(bullet);
      if(m->type == moPyroCultist) 
        m->type = moCultist;
      else
        m->nextshot = curtime + 100;
      }
    for(int i=0; i<players; i++)
    if((m->type == moAirElemental) && curtime >= m->nextshot && intval(m->pat*C0, pc[i]->pat*C0) < 2) {
      monster* bullet = new monster;
      bullet->base = m->base;
      bullet->at = m->at;
      bullet->type = moAirball;
      bullet->parent = m;
      active.push_back(bullet);
      m->nextshot = curtime + 1500;
      }
    for(int i=0; i<players; i++)
    if(m->type == moFlailer && curtime >= m->nextshot && 
      intval(m->pat*C0, pc[i]->pat*C0) < 2) {
      m->nextshot = curtime + 3500;
      monster* bullet = new monster;
      bullet->base = m->base;
      bullet->at = m->at;
      bullet->type = moFlailBullet;
      bullet->parent = m;
      bullet->vel = 1/400.0;
      active.push_back(bullet);
      break;
      }
    }
  }

#ifndef MOBILE
void turn(int delta) {

  bool b = vid.female;
  
  if(delta > 1000) delta = 1000;
  
  if(delta > 200) { turn(200); delta -= 200; if(!delta) return; }

  curtime += delta;

  handleInput(delta);
  
  invismove = (curtime >= visibleAt) && markOrb(itOrbInvis);

  // detect active monsters
  for(map<cell*, transmatrix>::iterator it = gmatrix.begin(); it != gmatrix.end(); it++) {
    cell *c = it->first;
    pair<mit, mit> p = 
      monstersAt.equal_range(c);
    for(mit it = p.first; it != p.second;) {
      mit itplus = it;
      itplus++;
      active.push_back(it->second);      
      monstersAt.erase(it);
      it = itplus;
      }
    if(c->monst && !isIvy(c) && !isWorm(c)) {
      monster *enemy = new monster;
      enemy->at = Id;
      enemy->base = c;
      enemy->type = c->monst;
      enemy->hitpoints = c->hitpoints;
      if(c->wall == waBoat && isLeader(c->monst)) 
        enemy->inBoat = true, c->wall = waNone;
      c->monst = moNone;
      active.push_back(enemy);
      }
    }

  /* printf("size: gmatrix = %ld, active = %ld, monstersAt = %ld, delta = %d\n", 
    gmatrix.size(), active.size(), monstersAt.size(),
    delta); */
  
  bool exists[motypes];
  
  for(int i=0; i<motypes; i++) exists[i] = false;

  for(int i=0; i<size(active); i++) {
    monster* m = active[i];
    m->findpat();
    exists[movegroup(m->type)] = true;
    }
  
  for(int i=0; i<size(active); i++) {
    monster* m = active[i];
    
    switch(m->type) {
      case moPlayer: 
        movePlayer(m, delta);
        break;
      
      case moBullet: case moFlailBullet: case moFireball: case moTongue: case moAirball:
        moveBullet(m, delta);
        break;
      
      default: ;
      }
    }

  for(int i=0; i<size(active); i++) {
    monster* m = active[i];
    if(isMimic(m->type))
      moveMimic(m);
    }

  for(int t=1; t<motypes; t++) if(exists[t]) {
  
    // build the path data
    
    int pqs = size(pathq);
    for(int i=0; i<pqs; i++) {
      pathq[i]->pathdist = INFD;
      }
    pathq.clear(); 

    for(int i=0; i<size(targets); i++) {
      targets[i]->pathdist = isPlayerOn(targets[i]) ? 0 : 1;
      pathq.push_back(targets[i]);
      }

    int qb = 0;
    for(qb=0; qb < size(pathq); qb++) {
      cell *c = pathq[qb];
      int d = c->pathdist;
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        // printf("i=%d cd=%d\n", i, c->mov[i]->cpdist);
        if(c2 && c2->pathdist == INFD && gmatrix.count(c2) && 
          (passable_for(eMonster(t), c, c2, false) || c->wall == waThumperOn)) {
          c2->pathdist = d+1;
          pathq.push_back(c2);
          }
        }
      }
    
    // printf("time %d, t=%d, q=%d\n", curtime, t, qb);
  
    // move monsters of this type
    
    for(int i=0; i<size(active); i++) 
     if(movegroup(active[i]->type) == t)
        moveMonster(active[i], delta);
    }
  
  bool tick = curtime >= nextmove;
  keepLightning = ticks <= lightat + 1000;
  cwt.c = pc[0]->base;
  bfs(tick); moverefresh();
  countLocalTreasure();
  pushmonsters();
  processheat(delta / 350.0, tick);
  markOrb(itOrbSpeed);
  if(tick) {
    nextmove += 1000;
    reduceOrbPowers();
    if(!(items[itOrbSpeed] & 1)) {
      moveworms();
      moveivy();
      if(havehex) movehex();
      wandering();
      livecaves();
      dryforest();
      if(havewhirlpool) movewhirlpool();
      }
    }
  popmonsters();
  
  gmatrix.clear();
  
  canmove = true;
  
  for(int i=0; i<players; i++) {
    if(pc[i]->dead && items[itOrbLife] && !items[itOrbShield]) {
      items[itOrbLife]--;
      items[itOrbShield] += 3;
      items[itOrbGhost] += 3;
      orbused[itOrbShield] = true;
      }
  
    if(pc[i]->dead && items[itOrbShield]) {
      pc[i]->dead = false;
      orbused[itOrbShield] = true;
      }
  
    if(pc[i]->dead && items[itOrbFlash]) {
      pc[i]->dead = false;
      pushmonsters();
      killMonster(pc[i]->base);
      activateFlash();
      popmonsters();
      }
      
    if(pc[i]->dead && items[itOrbLightning]) {
      pc[i]->dead = false;
      pushmonsters();
      killMonster(pc[i]->base);
      activateLightning();
      popmonsters();
      }
    
    if(pc[i]->dead && !lastdead)
      achievement_final(true);
    lastdead = pc[i]->dead;
      
    canmove = canmove && !pc[i]->dead;
    }
  
  // deactivate all monsters
  for(int i=0; i<size(active); i++)
    if(active[i]->dead && active[i] != pc[0] && active[i] != pc[1]) {
      for(int j=0; j<size(active); j++) if(active[j]->parent == active[i])
        active[j]->parent = active[i]->parent;
      delete active[i];
      }
    else active[i]->store();
    
  active.clear();

  if(safety) { 
    safety = false;
    activateSafety(pc[0]->base->land);
    }

  vid.female = b;
  }
#endif

void init() {

  players = vid.scfg.players;

  pc[0] = new monster;
  pc[0]->type = moPlayer;
  pc[0]->pid = 0;
  pc[0]->at = xpush(players == 2 ? -.3:0) * spin(players == 2 ? M_PI : 0) * Id;
  pc[0]->base = cwt.c;
  pc[0]->inBoat = (firstland == laCaribbean || firstland == laOcean || firstland == laLivefjord);
  pc[0]->store();
  
  if(players == 2) {
    pc[1] = new monster;
    pc[1]->type = moPlayer;
    pc[1]->pid = 1;
    pc[1]->at = xpush(.3) * Id;
    pc[1]->base = cwt.c;
    pc[1]->inBoat = (firstland == laCaribbean || firstland == laOcean || firstland == laLivefjord);
    pc[1]->store();
    }
  else pc[1] = NULL;
  
  if(!safety) {
    items[itOrbLife] = 3;
    addMessage(XLAT("Welcome to the Shoot'em Up mode!"));
    // addMessage(XLAT("F/;/Space/Enter/KP5 = fire, WASD/IJKL/Numpad = move"));
    }
  }

monster *getPlayer() {
  return pc[cpid];
  }

bool drawMonster(const transmatrix& V, cell *c) {
  gmatrix[c] = V;

  pair<mit, mit> p = 
    monstersAt.equal_range(c);

  bool b = vid.female;

  for(mit it = p.first; it != p.second; it++) {
    monster* m = it->second;
    m->pat = V * m->at;
    
    if(!outofmap(mouseh))
      if(!mousetarget || intval(mouseh, mousetarget->pat*C0) > intval(mouseh, m->pat*C0))
        mousetarget = m;
    
    if(m->inBoat) {
      if(m->type == moPlayer && items[itOrbWater]) {
        queuepoly(m->pat, shBoatOuter, watercolor(0));
        queuepoly(m->pat, shBoatInner, 0x0060C0FF);
        }
      else {
        queuepoly(m->pat, shBoatOuter, 0xC06000FF);
        queuepoly(m->pat, shBoatInner, 0x804000FF);
        }
      }

    if(doHighlight())
      poly_outline = 
        isBullet(m) ? 0x00FFFFFF :
        (isFriendly(m->type) || m->type == moPlayer) ? 0x00FF00FF : 0xFF0000FF;

    switch(m->type) {
      case moPlayer: 
        drawPlayerEffects(m->pat, c);
        cpid = m->pid; if(players == 2) vid.female = cpid;
        if(mapeditor::drawplayer) drawMonsterType(moPlayer, c, m->pat, 0xFFFFFFC0);
        vid.female = b;
        break;
      case moBullet: {
        int col;
        if(m->parenttype == moPlayer)
          col = vid.swordcolor;
        else
          col = (minf[m->parenttype].color << 8) | 0xFF;
        queuepoly(m->pat * spin(curtime / 50.0), shKnife, col);
        break;
        }
      case moTongue:
        queuepoly(m->pat, shTongue, (minf[m->parenttype].color << 8) | 0xFF);
        break;
      case moFireball:  case moAirball:
        queuepoly(m->pat, shPHead, (minf[m->type].color << 8) | 0xFF);
        break;
      case moFlailBullet: 
        queuepoly(m->pat * spin(curtime / 50.0), shFlailMissile, (minf[m->type].color << 8) | 0xFF);
        break;

      default:
        int col = minf[m->type].color;
        if(m->type == moSlime) {
          col = winf[c->wall].color;
          col |= (col >> 1);
          }
        bool b = vid.female; cpid = m->pid; 
        if(players == 2 && isMimic(m->type)) vid.female = cpid;
        if(isStunnable(m->type) && m->stunoff > curtime)
          c->stuntime = 1 + (m->stunoff - curtime-1)/300;
        if(hasHitpoints(m->type))
          c->hitpoints = m->hitpoints;
        drawMonsterType(m->type, c, m->pat, col);
        vid.female = b;
        break;
      }
    }

  return false;
  }

void clearMemory() {
  for(mit it = monstersAt.begin(); it != monstersAt.end(); it++)
    delete(it->second);
  for(int i=0; i<size(active); i++) delete active[i];
  monstersAt.clear();
  gmatrix.clear();
  curtime = 0;
  nextmove = 0;
  invisible_at = 0;
  }

void initConfig() {
#ifndef MOBILE
  vid.scfg.players = 1;
  
  char* t = vid.scfg.keyaction;
  
  t['w'] = 16 + 0;
  t['s'] = 16 + 1;
  t['a'] = 16 + 2;
  t['d'] = 16 + 3;

  t[SDLK_KP8] = 16 + 4;
  t[SDLK_KP6] = 16 + 5;
  t[SDLK_KP2] = 16 + 6;
  t[SDLK_KP4] = 16 + 7;

  t['f'] = 16 + pcFire;
  t['g'] = 16 + pcFace;
  t['h'] = 16 + pcFaceFire;
  t['r'] = 16 + pcDrop;
  t['t'] = 16 + pcOrbPower;
  t['y'] = 16 + pcCenter;

  t['i'] = 32 + 0;
  t['k'] = 32 + 1;
  t['j'] = 32 + 2;
  t['l'] = 32 + 3;
  t[';'] = 32 + 8;
  t['\''] = 32 + 9;
  t['p'] = 32 + 10;
  t['['] = 32 + pcCenter;

  t[SDLK_UP] = 48 ;
  t[SDLK_RIGHT] = 48 + 1;
  t[SDLK_DOWN] = 48 + 2;
  t[SDLK_LEFT] = 48 + 3;
  t[SDLK_PAGEUP] = 48 + 4;
  t[SDLK_PAGEDOWN] = 48 + 5;
  t[SDLK_HOME] = 48 + 6;

  vid.scfg.joyaction[0][0] = 16 + pcFire;
  vid.scfg.joyaction[0][1] = 16 + pcOrbPower;
  vid.scfg.joyaction[0][2] = 16 + pcDrop;
  vid.scfg.joyaction[0][3] = 16 + pcCenter;
  vid.scfg.joyaction[0][4] = 16 + pcFace;
  vid.scfg.joyaction[0][5] = 16 + pcFaceFire;

  vid.scfg.joyaction[1][0] = 32 + pcFire;
  vid.scfg.joyaction[1][1] = 32 + pcOrbPower;
  vid.scfg.joyaction[1][2] = 32 + pcDrop;
  vid.scfg.joyaction[1][3] = 32 + pcCenter;
  vid.scfg.joyaction[1][4] = 32 + pcFace;
  vid.scfg.joyaction[1][5] = 32 + pcFaceFire;

  vid.scfg.axeaction[0][0] = 4;
  vid.scfg.axeaction[0][1] = 5;
  vid.scfg.axeaction[0][3] = 2;
  vid.scfg.axeaction[0][4] = 3;

  vid.scfg.axeaction[1][0] = 8;
  vid.scfg.axeaction[1][1] = 9;
  
  // ULRD
  vid.scfg.hataction[0][0][0] = 16 + 0;
  vid.scfg.hataction[0][0][1] = 16 + 3;
  vid.scfg.hataction[0][0][2] = 16 + 1;
  vid.scfg.hataction[0][0][3] = 16 + 2;
  vid.scfg.hataction[0][1][0] = 16 + 4;
  vid.scfg.hataction[0][1][1] = 16 + 7;
  vid.scfg.hataction[0][1][2] = 16 + 5;
  vid.scfg.hataction[0][1][3] = 16 + 6;

  vid.scfg.hataction[1][0][0] = 32 + 0;
  vid.scfg.hataction[1][0][1] = 32 + 3;
  vid.scfg.hataction[1][0][2] = 32 + 1;
  vid.scfg.hataction[1][0][3] = 32 + 2;
  vid.scfg.hataction[1][1][0] = 32 + 4;
  vid.scfg.hataction[1][1][1] = 32 + 7;
  vid.scfg.hataction[1][1][2] = 32 + 5;
  vid.scfg.hataction[1][1][3] = 32 + 6;
#endif
  }

cell *playerpos(int i) {
  return pc[i]->base;
  }

bool playerInBoat(int i) {
  return pc[i]->inBoat;
  }

void saveConfig(FILE *f) {
  fprintf(f, "%d %d", VERNUM, vid.scfg.players);
  for(int i=0; i<512; i++) fprintf(f, " %d", vid.scfg.keyaction[i]);
  for(int i=0; i<8; i++) for(int j=0; j<MAXBUTTON; j++) fprintf(f, " %d", vid.scfg.joyaction[i][j]);
  for(int i=0; i<8; i++) for(int j=0; j<MAXAXE; j++) fprintf(f, " %d", vid.scfg.axeaction[i][j]);
  for(int i=0; i<8; i++) for(int j=0; j<MAXHAT; j++) for(int k=0; k<4; k++)
    fprintf(f, " %d", vid.scfg.hataction[i][j][k]);
  fprintf(f, "\n");
  }

void scanchar(FILE *f, char& c) {
  int i = c;
  int err = fscanf(f, "%d", &i);
  if(err == 1) c = i;
  }
  
void loadConfig(FILE *f) {
  int xvernum;
  int err = fscanf(f, "%d %d", &xvernum, &vid.scfg.players);
  if(err != 2) return;
  for(int i=0; i<512; i++) scanchar(f, vid.scfg.keyaction[i]);
  for(int i=0; i<8; i++) for(int j=0; j<MAXBUTTON; j++) scanchar(f, vid.scfg.joyaction[i][j]);
  for(int i=0; i<8; i++) for(int j=0; j<MAXAXE; j++) scanchar(f, vid.scfg.axeaction[i][j]);
  for(int i=0; i<8; i++) for(int j=0; j<MAXHAT; j++) for(int k=0; k<4; k++)
    scanchar(f, vid.scfg.hataction[i][j][k]);
  }

int shmupnumkeys;
const char** shmupcmdtable;

char* axeconfigs[24]; int numaxeconfigs;

string listkeys(int id) {
#ifndef MOBILE
  string lk = "";
  for(int i=0; i<512; i++)
    if(vid.scfg.keyaction[i] == id)
      lk = lk + " " + SDL_GetKeyName(SDLKey(i));
  for(int i=0; i<numsticks; i++) for(int k=0; k<SDL_JoystickNumButtons(sticks[i]) && k<MAXBUTTON; k++)
    if(vid.scfg.joyaction[i][k] == id) {
      lk = lk + " " + cts('A'+i)+"-B"+its(k);
      }
  for(int i=0; i<numsticks; i++) for(int k=0; k<SDL_JoystickNumHats(sticks[i]) && k<MAXHAT; k++)
    for(int d=0; d<4; d++)
      if(vid.scfg.hataction[i][k][d] == id) {
        lk = lk + " " + cts('A'+i)+"-"+"URDL"[d];
        }
  return lk;
#endif
  }

void showShmupConfig() {
#ifndef MOBILE

  int sc = vid.scfg.subconfig;

  if(sc == 1 || sc == 2) {
    shmupnumkeys = 14;
    shmupcmdtable = shmup::playercmds;
    }
  else if(sc == 3) {
    shmupnumkeys = 7;
    shmupcmdtable = shmup::pancmds;
    }
  else if(sc == 4) {
    getcstat = ' ';
    numaxeconfigs = 0;
    for(int j=0; j<numsticks; j++) {
      for(int ax=0; ax<SDL_JoystickNumAxes(sticks[j]) && ax < MAXAXE; ax++) if(numaxeconfigs<24) {
        int y = SDL_JoystickGetAxis(sticks[j], ax);
        string buf = " ";
        while(y >  10000) buf += "+", y -= 10000;
        while(y < -10000) buf += "-", y += 10000;
        if(y>0) buf += "+";
        if(y<0) buf += "-";
        axeconfigs[numaxeconfigs] = &(vid.scfg.axeaction[j][ax]);
        char aa = *axeconfigs[numaxeconfigs];
        string what = XLAT(shmup::axemodes[aa%SHMUPAXES]);
        displayStat(numaxeconfigs, XLAT("Joystick %1, axis %2", cts('A'+j), its(ax)) + buf, 
          what, 'a'+numaxeconfigs);
        numaxeconfigs++;
        }
      }
    }
  else if(sc == 0) {
    int vy = vid.yres/2 - vid.fsize * 6;
    
    displayButton(vid.xres/2, vy, 
      ifMousing("n", vid.scfg.players == 2 ? "two players" : "one player"), 'n', 8, 2);
    displayButton(vid.xres/2, vy+vid.fsize*2, 
      ifMousing("1", "configure player 1"), '1', 8, 2);
    if(vid.scfg.players > 1)
      displayButton(vid.xres/2, vy+vid.fsize*3, 
        ifMousing("2", "configure player 2"), '2', 8, 2);
    displayButton(vid.xres/2, vy+vid.fsize*5, 
      ifMousing("p", "configure panning"), 'p', 8, 2);
    if(numsticks > 0) 
      displayButton(vid.xres/2, vy+vid.fsize*7, 
        ifMousing("j", "configure joystick axes"), 'j', 8, 2);
    displayButton(vid.xres/2, vy+vid.fsize*9, 
      ifMousing("s", 
        shmup::on && shmup::players == vid.scfg.players ?
          "continue playing"
        : "start playing in the shmup mode"), 's', 8, 2);
    displayButton(vid.xres/2, vy+vid.fsize*10, 
      ifMousing("t", "return to the turn-based mode"), 't', 8, 2);
    displayButton(vid.xres/2, vy+vid.fsize*11, 
      ifMousing("c", "save the configuration"), 'c', 8, 2);
    }
  
  if(sc >= 1 && sc <= 3) {
    getcstat = ' ';

    for(int i=0; i<shmupnumkeys; i++)
      displayStat(i, XLAT(shmupcmdtable[i]), listkeys(16*sc+i),
        vid.scfg.setwhat ? (vid.scfg.setwhat>1 && i == (vid.scfg.setwhat&15) ? '?' : ' ') : 'a'+i);

    displayStat(-2,
      XLAT(sc == 1 ? "configure player 1" :
      sc == 2 ? "configure player 2" :
      "configure panning"), "", ' ');

    if(vid.scfg.setwhat == 1)
      displayStat(16, XLAT("press a key to unassign"), "", ' ');
    else if(vid.scfg.setwhat)
      displayStat(16, XLAT("press a key for '%1'", XLAT(shmupcmdtable[vid.scfg.setwhat&15])), "", ' ');
    else
      displayStat(16, XLAT("unassign a key"), "", 'z');
    }
#endif
  }

void handleConfig(int uni, int sym) {
#ifndef MOBILE
  int sc = vid.scfg.subconfig;
  if(sc == 0) {
    if(uni == '1') vid.scfg.subconfig = 1;
    else if(uni == '2') vid.scfg.subconfig = 2;
    else if(uni == 'p') vid.scfg.subconfig = 3;
    else if(uni == 'j') vid.scfg.subconfig = 4;
    else if(uni == 's') {
      cmode = emNormal;
      if(!shmup::on) restartGame('s');
      else if(vid.scfg.players != shmup::players) restartGame();
      }
    else if(uni == 't') {
      cmode = emNormal;
      if(shmup::on) restartGame('s');
      }
    else if(uni == 'c')
      ::saveConfig();
    else if(uni == 'n')
      vid.scfg.players = 1 + (vid.scfg.players&1);
    }
  else if(sc == 4) {
    if(sym) {
      if(uni >= 'a' && uni < 'a' + numaxeconfigs) {
        (*axeconfigs[uni - 'a'])++;
        (*axeconfigs[uni - 'a']) %= SHMUPAXES;
        }
      else
        vid.scfg.subconfig = 0;
      }
    }
  else {
    if(sym) {
      if(vid.scfg.setwhat) {
        vid.scfg.keyaction[sym] = vid.scfg.setwhat;
        vid.scfg.setwhat = 0;
        }
      else if(uni >= 'a' && uni < 'a' + shmupnumkeys)
        vid.scfg.setwhat = 16*sc+uni - 'a';
      else if(uni == 'z')
        vid.scfg.setwhat = 1;
      else
        vid.scfg.subconfig = 0;
      }
    }
#endif
  }
}
