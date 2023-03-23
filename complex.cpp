// Hyperbolic Rogue -- Complex features
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file complex.cpp 
 *  \brief This file implements the gameplay/generation for the more complex lands and mechanics.
 *
 *  Includes: whirlwind, whirlpool, elec, princess, clearing, mirror, hive, heat + livecaves, etc.
 */

#include "hyper.h"
namespace hr {

EX namespace whirlwind {

  EX int fzebra3(cell *c) {
    if(arcm::in()) return 0;
    if(euclid) {
      if(closed_manifold) return 0;
      auto co = euc2_coordinates(c);
      int y = co.second;
      return 1+((((signed short)(y)+int(50000))/3)%3);
      }
    if(S7 == 5) return getHemisphere(c, 0) > 0 ? 1 : 2;
    if(S7 < 5) {
      int d = celldistance(currentmap->gamestart(), c);
      if(d == 0) return 0;
      if(S7 == 4 && d == 3) return 0;
      if(S7 == 3 && d == 2 && !ctof(c)) return 0;
      return 1;
      }
    return zebra3(c);
    }

  EX void switchTreasure(cell *c) {
    c->item = itNone;
    if(safety) return;
    if(hrand(5000) < PT(100 + 2 * (kills[moAirElemental] + kills[moWindCrow]), 200) && notDippingFor(itWindstone)
      && (!shmup::on || getGhostcount() < 2))
      c->item = itWindstone;
    else if(hrand(5000) < 20*PRIZEMUL)
      placeLocalOrbs(c);
    }

  EX int cat(cell *c) {
    if(c->land != laWhirlwind) return 0;
    if(c->wall != waNone && c->wall != waChasm && 
      c->wall != waSea && !isAlchAny(c) &&
      c->wall != waMineMine && c->wall != waFire) return 0;
    if(c->item == itKey || c->item == itOrbYendor) return 0;
    if(airdist(c) < 3) return 0;
    if(c->monst == moHexSnake || c->monst == moHexSnakeTail) return 0;
    return fzebra3(c);
    }
  
  cell *where;
  EX int dfrom[2], dto[2];
  EX int qdirs;
  
  int gdist(int d, int e) { return dirdiff(d-e, where->type); }
  
  EX void calcdirs(cell *c) {
    where = c;
    int d = cat(c);
    qdirs = 0;
    if(d == 0) return;
    int qdf = 0, qdt = 0;
    vector<int> cats(c->type);
    for(int i=0; i<c->type; i++) 
      cats[i] = cat(createMov(c,i));
    for(int i=0; i<c->type; i++)
      if(cats[i] == d) {
        bool c1 = (cats[(i+1)%c->type] != d);
        bool c2 = (cats[(i+c->type-1)%c->type] != d);
        if(c1 && !c2) dto[qdt++] = i;
        if(c2 && !c1) dfrom[qdf++] = i;
        }
    qdirs = qdf;
    if(qdirs == 2) {
      int cur = gdist(dfrom[0], dto[0]) + gdist(dfrom[1], dto[1]);
      int alt = gdist(dfrom[0], dto[1]) + gdist(dfrom[1], dto[0]);
      if(alt < cur) swap(dto[0], dto[1]);
      }
    }

  int mindist(int d, int *tab) {
    if(qdirs == 0) return NODIR;
    if(qdirs == 1) return gdist(d, tab[0]);
    return min(gdist(d, tab[0]), gdist(d, tab[1]));
    }

  EX int winddir(int d) {
    if(d == -1) return 0;
    int mdf = mindist(d, dfrom);
    int mdt = mindist(d, dto);
    // printf("dir = %d mdf = %d mdt = %d\n", d, mdf, mdt);
    if(mdf < mdt) return -1;
    if(mdf > mdt) return 1;
    return 0;
    }
  
  void build(vector<cell*>& whirlline, int d) {
    again: 
    cell *at = whirlline[isize(whirlline)-1];
    cell *prev = whirlline[isize(whirlline)-2];
    if(looped(whirlline)) return;
    for(int i=0; i<at->type; i++) 
      if(at->move(i) && (euclid || at->move(i)->master->alt) && celldistAlt(at->move(i)) == d && at->move(i) != prev) {
        whirlline.push_back(at->move(i));
        goto again;
        }
    }
  
  void moveAt(cell *c, manual_celllister& cl) {
    if(cl.listed(c)) return;
    calcdirs(c);
    if(qdirs != 1) return;
    vector<cell*> whirlline;
    whirlline.push_back(c);
    cell *prev = c;
    cell *c2 = c->move(dfrom[0]);
    while(true) {
      // printf("c = %p dist = %d\n", c2, c2->mpdist);
      if(c == c2) break;
      calcdirs(c2);
      if(qdirs == 0) break;
      cell *cc2 = c2;
      if(qdirs == 1) whirlline.push_back(c2), c2 = c2->move(dfrom[0]);
      else if(c2->move(dto[0]) == prev) 
        c2 = c2->move(dfrom[1]);
      else
        c2 = c2->move(dfrom[0]);
      prev = cc2;
      }
    int z = isize(whirlline);
    // printf("Cycle built from %p, length = %d\n", c, z);
    for(int i=0; i<z; i++) {
      // printf("%d%c", whirlline[i]->mpdist, whirlline[i]->item ? '*' : ' ');
      cl.add(whirlline[i]);
      if(whirlline[i]->mpdist == BARLEV)
        switchTreasure(whirlline[i]);
      }
    for(int i=0; i<z-1; i++) {
      moveItem(whirlline[i], whirlline[i+1], true);
      if(whirlline[i]->item)
        animateMovement(match(whirlline[i+1], whirlline[i]), LAYER_BOAT);
      }
    for(int i=0; i<z; i++) 
      pickupMovedItems(whirlline[i], i==z-1 ? whirlline[0] : whirlline[i+1]);
    }
  
  EX void move() {
    manual_celllister cl;
    for(int i=0; i<isize(dcal); i++) {
      cell *c = dcal[i];
      moveAt(c, cl);
      }
    // Keys and Orbs of Yendor always move
    using namespace yendor;
    for(int i=0; i<isize(yi); i++) {
      moveAt(yi[i].path[0], cl);
      moveAt(yi[i].key(), cl);
      }
    }
  
  EX cell *jumpFromWhereTo(cell *c, bool player) {
    for(int i=0; i<2; i++) {
      calcdirs(c);
      if(qdirs != 1) return NULL;
      cell *c2 = c->move(dfrom[0]);
      if(!passable(c, c2, P_JUMP1)) return NULL;
      if(player && i == 0 && !passable(c, c2, P_ISPLAYER)) return NULL;
      c = c2;
      }
    calcdirs(c);
    if(qdirs != 1) return NULL;
    return c;
    }

  EX cell *jumpDestination(cell *c) {
    for(int i=0; i<2; i++) {
      calcdirs(c);
      if(qdirs != 1) return NULL;
      c = c->move(dto[0]);
      }
    calcdirs(c);
    if(qdirs != 1) return NULL;
    return c;
    }
EX }

EX namespace elec { 

  EX bool havecharge, haveelec, havethunder;
  EX bool afterOrb; // extra charge from the Orb of Lightning

  #if HDR
  enum eCharge {
    ecCharged, ecGrounded, ecIsolator, ecConductor
    };
  #endif
  
  bool conduct(eCharge cf, eCharge ct) {
    if(ct == ecIsolator) return false;
    if(ct == ecConductor) return true;
    return ct != cf;
    }
  
  EX eCharge getCharge(cell *c) {
    bool ao = afterOrb && c->ligon;
    
    /* not yet generated */
    if(c->land == laNone) return ecIsolator;
    
    if(c->wall == waCharged) return ecCharged;
    if(c->wall == waSea || c->wall == waGrounded) return ecGrounded;
    if(c->wall == waSandstone || c->wall == waDeadTroll || 
      c->wall == waDeadTroll2 || 
      among(c->wall, waBigTree, waSmallTree, waExplosiveBarrel, waRed1, waRed2, waRed3) ||
      c->wall == waVinePlant ||
      c->wall == waMetal || isAlchAny(c)) 
        return isElectricLand(c) ? ecConductor : ecGrounded; 
    if(c->wall == waBarrier)
      return ecIsolator;
    if(c->wall == waChasm)
      return ecIsolator;

    if(shmup::on ? isPlayerOn(c) : (isPlayerOn(c) || (items[itOrbEmpathy] && isFriendly(c)))) {
      if(items[itOrbShield]) return ecIsolator;
      if(afterOrb) return ecIsolator;
      if(!items[itOrbAether]) return isElectricLand(c) ? ecConductor : ecGrounded;
      }
    
    // if(c->monst && stalemate::moveto) printf("%p: isKilled = %d\n", c, stalemate::isKilled(c));

    else if(c->monst 
      && c->monst != moGhost && c->monst != moIvyDead && c->monst != moIvyNext 
      && !(isDragon(c->monst) && !c->hitpoints)
      )
      return isElectricLand(c) ? (ao ? ecCharged : ecConductor) : ecGrounded;

    if(!isElectricLand(c))
      return ecGrounded;

    if(ao) return ecCharged;

    return ecIsolator;
    }
  
  // To process conductivity, consider the following graph:
  
  // - edges are between conductors and adjacent charged/grounded/conductor cells
  // - all charged cells are connected to one special cell '0'
  // - all grounded cells are connected to one special cell '1'
  // - cells '0' and '1' are connected
  
  // If A and B are in the same biconnected component, then there is a closed circuit,
  // consisting of all other cells in that component. 
  
  // To find biconnected components, we are using the Hopcroft-Tarjan algorithm.
  
  struct chargedata {
    cell *c;
    int otmp;
    int lowlink;
    bool instack;
    bool fire;
    };
  
  vector<chargedata> charges;
  
  vector<pair<int, int> > xstack;
  
  vector<cell*> chargecells;
  
  bool hasdata(cell *c) {
    return c->listindex >= 0 && c->listindex < isize(charges) && charges[c->listindex].c == c;
    }
  
  void connect(int from, cell *c) {
    if(hasdata(c)) {
      // seen again: set the lowlink
      if(!charges[c->listindex].instack) return;
      // printf("edge %d-%d\n", from, c->listindex);
      if(c->listindex < charges[from].lowlink)
        charges[from].lowlink = c->listindex;
      }
    else {
      int id = isize(charges);
      charges.push_back(chargedata());

      {chargedata& ch(charges[id]);
      ch.c = c; ch.otmp = c->listindex; ch.lowlink = id; c->listindex = id;
      ch.instack = true; ch.fire = false;
      }
      // c->landparam = id;
      
      // printf("edge %d-%d [%s]\n", from, id, dnameof(c->wall));
      
      xstack.push_back(make_pair(from, id));
      
      eCharge chh = getCharge(c);
      
      if(chh == ecGrounded) {
        xstack.push_back(make_pair(id, 0));
        charges[id].lowlink = 0;
        }
      else if(chh == ecCharged) {
        xstack.push_back(make_pair(id, 1));
        if(from != 1) charges[id].lowlink = 1;
        }
      
      for(cell *c2: adj_minefield_cells(c)) {
        if(c2->listindex == from) continue;
        eCharge ct = getCharge(c2);
        if(conduct(chh, ct))
          connect(id, c2);
        }
      
      // printf("lowlink of %d [%s] = %d\n", id, dnameof(c->wall), ch.lowlink);
      if(charges[id].lowlink < charges[from].lowlink)
        charges[from].lowlink = charges[id].lowlink;
  
      if(charges[id].lowlink >= from) {
        while(xstack.back().first != from || xstack.back().second != id) {
          // printf("bcc %d,%d\n", xstack.back().first, xstack.back().second);
          xstack.pop_back();
          }
        // printf("bcc %d,%d\n", xstack.back().first, xstack.back().second);
        xstack.pop_back();
        // printf("\n");
        }
      
      charges[id].instack = false;
      }
    }
  
  void affect(cell *c) {
    c->ligon = true;
    if(c->monst) {
      if(c->monst == moMetalBeast2 && !c->item)
        c->item = itFulgurite;
      killMonster(c, moLightningBolt);
      }
    if(isPlayerOn(c)) {
      killThePlayerAt(moLightningBolt, c, 0);
      }
    if(c->wall == waSandstone) {
      c->wall = waNone, drawParticles(c, winf[waSandstone].color, 16);
      if(c->land == laStorms)
        c->item = itFulgurite;
      }
    if(c->wall == waRed1) c->wall = waNone;
    if(c->wall == waRed2) c->wall = waRed1;
    if(c->wall == waRed3) c->wall = waRed2;
    if(c->wall == waDeadTroll) c->wall = waCavefloor;
    if(c->wall == waBigTree || c->wall == waSmallTree)
      makeflame(c, 10, false);
    if(c->wall == waExplosiveBarrel)
      explodeBarrel(c);
    if(c->wall == waDeadTroll2 || isAlchAny(c) || c->wall == waVinePlant)
      drawParticles(c, winf[c->wall].color, 16),
      c->wall = waNone;
    
    /* destroy charged walls on the border */
    if(c->wall == waCharged) forCellEx(c1, c) if(c1->wall == waGrounded)
      c->wall = waNone;
    /* if(c->wall == waCharged)
      c->wall = waMetal; */
    }
  
  void listChargedCells(cell *c, manual_celllister& cl, eCharge last = ecConductor) {
    if(cl.listed(c)) return;
    eCharge here = getCharge(c);
    /* if(c->cpdist <= 2) {
      printf("monst=%s ", dnameof(c->monst));
      printf("wall=%s ", dnameof(c->wall));
      printf("c=%p here=%d last=%d\n", c, here, last);
      } */
    if(here == ecIsolator) cl.add(c);
    if(!conduct(last, here)) return;
    if(here == ecCharged) chargecells.push_back(c);
    cl.add(c);
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->move(i);
      if(c2) listChargedCells(c2, cl, here);
      }
    }
  
  EX void init() {
    chargecells.clear();
    if(!haveelec && !afterOrb) return;
    if(1) {
      manual_celllister cl;
      for(int i=0; i<isize(dcal); i++) listChargedCells(dcal[i], cl);
      }
    
    charges.resize(2); 
    charges[0].lowlink = 0; charges[1].lowlink = 1;
    if(!havecharge) return;
    
    xstack.clear();
    
    for(int i=0; i<isize(chargecells); i++) 
      connect(1, chargecells[i]);
  
    havethunder = charges[1].lowlink == 0;
    if(havethunder) {
      for(int i=0; i<isize(xstack); i++) { 
        int k = xstack[i].first;
        int l = xstack[i].second;
        // printf("connected %d-%d\n", k, l);
        charges[k].fire = true;
        charges[l].fire = true;
        }
      }
    }
  
  void fire() {
    if(havethunder) {
      addMessage(XLAT("There is a flash of thunder!"));
      playSound(NULL, "storm");
      drawLightning();
      for(int i=2; i<isize(charges); i++) if(charges[i].fire)
        affect(charges[i].c);
      }
    }
  
  EX void cleanup() {
    for(int i=2; i<isize(charges); i++) 
      charges[i].c->listindex = charges[i].otmp;
    charges.resize(0); 
    }
  
  void draw(cell *c, eCharge what) {
    if(c->ligon) return;
    c->ligon = true;
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->move(i);
      if(!c2) continue;
      eCharge ch = getCharge(c2);
      if(conduct(what, ch))
        draw(c2, ch);
      }
    }
  
  void drawcharges() {
    for(int i=0; i<isize(dcal); i++) 
      if(getCharge(dcal[i]) == ecCharged)
        draw(dcal[i], ecCharged);
    }
  
  EX bool affected(cell *c) {
    if(c->listindex >= 0 && c->listindex < isize(charges) && charges[c->listindex].c == c) 
      return charges[c->listindex].fire;
    return false;
    }
  
  #if HDR
  struct builder {
    builder() { init(); }
    ~builder() { cleanup(); }
    };
  #endif

  EX void act() {
    int k = tkills();
    for(int i=0; i<numplayers(); i++)
      if(multi::playerActive(i) && isElectricLand(playerpos(i)) && !afterOrb) 
        markOrb(itOrbShield), markOrb(itOrbAether);
    builder b;
    fire();
    if(!afterOrb)
      achievement_count("ELEC", tkills() - k, 0);
    }
  
  // 0 = no close escape, 1 = close escape, 2 = message already shown
  EX int lightningfast;
  EX void checklightningfast() {
    changes.value_keep(lightningfast);
    if(lightningfast == 1) {
      addMessage(XLAT("Wow! That was close."));
      lightningfast = 2;
      }
    if(lightningfast > 1) return;
    builder b; 
    for(int i=0; i<numplayers(); i++)
      if(multi::playerActive(i) && elec::affected(playerpos(i))) 
        lightningfast = 1;
    }

  
  EX }

EX namespace princess {

#if HDR
#define OUT_OF_PRISON 200
#define OUT_OF_PALACE 250
#define PRADIUS0 (141)
#define PRADIUS1 (150)

struct info {
  int id;         // id of this info
  cell *prison;   // where was the Princess locked
  heptagon *alt;  // alt of the prison
  int bestdist;   // best dist achieved
  int bestnear;   // best dist achieved, by the player
  int value;      // number of Rugs at 120
  cell *princess; // where is the Princess currently
  int together;   // in which turn have we been together -- for the hug animation
  };
    
#endif

  EX gp::loc coords() { return gp::loc(39, 21); }

  EX bool generating = false;
  EX bool challenge = false;
  EX bool saved = false;
  EX bool everSaved = false;
  
  EX bool forceVizier = false;
  EX bool forceMouse = false;
  EX bool gotoPrincess = false;
  EX bool nodungeon = false;
  EX bool squeaked = false;

  EX int saveHP = 0;
  EX int saveArmedHP = 0;
  
  EX int reviveAt;
  
  EX vector<info*> infos;
  
  void assign(info *i) {
    if(i->alt) i->alt->emeraldval = i->id;
    }
  
  EX int newInfo(cell *c) {
    info *i = new info;
    i->prison = c;
    i->princess = c;
    i->alt = c->master->alt;
    i->id = isize(infos);
    i->bestdist = 0;
    i->bestnear = OUT_OF_PRISON;
    i->together = -INF;
    infos.push_back(i);
    assign(i);
    return i->id;
    }
  
  EX void newFakeInfo(cell *c) {
    info *i = new info;
    i->prison = NULL;
    i->princess = c;
    i->alt = NULL;
    i->id = isize(infos);
    i->bestdist = items[itSavedPrincess] ? OUT_OF_PALACE : OUT_OF_PRISON;
    i->bestnear = 0;
    i->together = -INF;
    infos.push_back(i);
    assign(i);
    }
  
  EX info *getPrisonInfo(cell *c) {
    if(euclid || quotient || sphere) return NULL;
    if(c->land != laPalace) return NULL;
    if(!c->master->alt) return NULL;
    int ev = c->master->alt->alt->emeraldval; // NEWYEARFIX
    if(ev < 0 || ev >= isize(infos)) return NULL;
    if(infos[ev]->alt != c->master->alt->alt) return NULL;
    return infos[ev];
    }
  
  EX info *getPrincessInfo(cell *c) {
    for(int i=0; i<isize(infos); i++) if(infos[i]->princess == c) {
      while(i) {
        infos[i]->id = i-1; assign(infos[i]);
        infos[i-1]->id = i; assign(infos[i-1]);
        swap(infos[i], infos[i-1]);
        i--;
        }
      return infos[i];
      }
    return NULL;
    }

  EX int dist(cell *c) {
    if(c->land != laPalace && c->land != laDungeon) return OUT_OF_PALACE;
    else if(quotient || sphere) return OUT_OF_PRISON;
    else if(euclid) return celldistAlt(c);
    else if(!c->master->alt) return OUT_OF_PRISON;
    else return celldistAlt(c);
    }
  
  void clear() {
    for(int i=0; i<isize(infos); i++) delete infos[i];
    infos.clear();
    }
 
  bool bringBackAt(cell *c) {
    if(!c) return false;
    if(!passable(c, NULL, 0)) return false;
    c->monst = moPrincessArmed;
    c->stuntime = 0;
    c->hitpoints = palaceHP();
    drawFlash(c);
    playSound(c, princessgender() ? "heal-princess" : "heal-prince");

    info *inf = NULL;
    for(int i=0; i<isize(infos); i++) {
      if(infos[i]->princess && infos[i]->bestdist == OUT_OF_PALACE && isPrincess(infos[i]->princess->monst))
        inf = infos[i];
      }
    if(inf) { inf->princess->monst = moNone; inf->princess = c; }
    else newFakeInfo(c);
    return true;
    }

  EX void bringBack() {
    if(bringBackAt(cwt.peek())) return;
    for(int i=1; i<isize(dcal); i++)
      if(bringBackAt(dcal[i])) return;
    }
  
  void setdist(info *i, int newdist) {
    changes.value_keep(*i);
    if(newdist < ALTDIST_ERROR && newdist > i->bestdist) {
      i->bestdist = newdist;
//    printf("Improved dist to %d\n", newdist);
      if(newdist == OUT_OF_PALACE) {
        if(!princess::saved)
#if CAP_INV
        if(!inv::on || !inv::usedForbidden)
#endif
          achievement_gain_once("PRINCESS1");
        changes.value_set(princess::saved, true);
        changes.value_set(princess::everSaved, true);
        if(inv::on && !princess::reviveAt)
          changes.value_set(princess::reviveAt, gold(NO_LOVE));
        items[itSavedPrincess]++;
        }
      if(newdist == OUT_OF_PRISON && princess::challenge) {
        addMessage(XLAT("Congratulations! Your score is %1.", its(i->value)));
        achievement_gain_once("PRINCESS2", rg::princess);
        if(!cheater) achievement_score(36, i->value);
        LATE( showMissionScreen(); )
        }
      }
    if(i->princess->land == laDungeon && !saved && !nodungeon) {
      addMessage(XLAT("%The1 says, \"not this place, it looks even worse...\"", moPrincess));
      nodungeon = true;
      }
    }

  EX void save(cell *princess) {
    if(euclid) return;
    princess::info *i = princess::getPrincessInfo(princess);
    if(!i || i->bestdist <= 3) changes.ccell(princess), princess->monst = moNone;
    else if(i) setdist(i, OUT_OF_PRISON);
    }

  EX void move(const movei& mi) {
    auto& cf = mi.s;
    auto& ct = mi.t;
    if(euclid) return;
    princess::info *i = princess::getPrincessInfo(cf);
    if(!i) {
      // note: OK if mapediting or loading
      printf("Warning: unknown princess\n"); 
      if(!cheater) 
        addMessage("Warning: unknown princess (that's a bug, please report)");
      newFakeInfo(ct);
      }
    else {
      changes.value_keep(*i);
      i->princess = ct;
      setdist(i, dist(ct));

      forCellIdEx(cp, j, ct) if(isPlayerOn(cp)) {
        bool safe = true;
        for(cell *test: {ct, cp}) forCellEx(c1, test) forCellEx(c2, test)
          if(isActiveEnemy(c1, moPlayer) || isActiveEnemy(c2, moPlayer))
            safe = false;
        if(safe) {
          if(turncount > i->together + 20) {
            animateHug(movei(ct, j), LAYER_SMALL);
            animateHug(movei(ct, j).rev(), LAYER_SMALL);
            }
          i->together = turncount;
          }
        }
      }
    }

  EX void mouseSqueak(cell *c) {
    eMonster m = c->monst;
    info *i = getPrisonInfo(c);
    int d = dist(c);

    playSound(c, "mousesqueak", 40);
    if(!i)
       addMessage(XLAT("%The1 squeaks in a confused way.", m));
    else if(i->bestdist >= 6)
       addMessage(XLAT("%The1 squeaks gratefully!", m));
    else if(!i->princess)
       addMessage(XLAT("%The1 squeaks hopelessly.", m));
    else if(d > 120)
       addMessage(XLAT("%The1 squeaks in despair.", m));
    else if(d > 90)
       addMessage(XLAT("%The1 squeaks sadly.", m));
    else if(d > 60)
       addMessage(XLAT("%The1 squeaks with hope!", m));
    else if(d > 30)
       addMessage(XLAT("%The1 squeaks happily!", m));
    else
       addMessage(XLAT("%The1 squeaks excitedly!", m));
    }
  
  EX void line(cell *c) {
    int d = (euclid || c->master->alt) ? celldistAlt(c) : 200;
    eMonster m = c->monst;
  
    static int msgid = 0;
    changes.value_keep(msgid);
  
    playSound(c, princessgender() ? "speak-princess" : "speak-prince");
    retry:
    if(msgid >= 127) msgid = 0;  
    
    bool inpalace = c->land == laPalace || c->land == laDungeon;
    
    if(msgid == 0 && d < 20 && inpalace) {
      addMessage(XLAT("%The1 kisses you, and begs you to bring %him1 away from here.", m));
      }
    else if(msgid == 1 && d >= 20 && inpalace && !peace::on) {
      if(m == moPrincess)
        addMessage(XLAT("\"I want my revenge. Stun a guard and leave him for me!\"", m));
      else
        addMessage(XLAT("\"That felt great. Thanks!\"", m));
      }
    else if(msgid  == 2 && d >= 70 && inpalace) {
      addMessage(XLAT("\"Bring me out of here please!\"", m));
      }
    else if(msgid == 3 && !inpalace) {
      addMessage(XLAT("%The1 kisses you, and thanks you for saving %him1.", m));
      }
    else if(msgid == 4 && !inpalace && m == moPrincess && !peace::on) {
      addMessage(XLAT("\"I have been trained to fight with a Hypersian scimitar, you know?\"", m));
      }
    else if(msgid == 16 && !inpalace) {
      addMessage(XLAT("\"I would love to come to your world with you!\"", m));
      }
    else if(msgid == 20 && !inpalace) {
      addMessage(XLAT("\"I do not like butterflies. They are treacherous.\"", m));
      }
    else if(msgid == 32 && !inpalace) {
      addMessage(XLAT("\"Straight lines stay close to each other forever, this is so romantic!\"", m));
      }
    else if(msgid == 40 && !inpalace) {
      addMessage(XLAT("\"I hate roses.\"", m));
      }
    else if(msgid == 48 && !inpalace) {
      addMessage(XLAT("\"Maps... Just like the world, but smaller... how is that even possible?!\"", m));
      }    
    else if(msgid == 64) {
      addMessage(XLAT("\"In this world there is plenty of space for everyone. We do not need wars.\"", m));
      }    
    else if(msgid == 65) {
      addMessage(XLAT("\"Only the stupid hyperbugs do not understand this.\"", m));
      }    
    else if(msgid == 72 && !inpalace) {
      addMessage(XLAT("\"I have once talked to a Yendorian researcher... he was only interested in infinite trees.\"", m));
      }
    else if(msgid == 73 && !inpalace) {
      addMessage(XLAT("\"Infinite trees are boring. I prefer other graphs.\"", m));
      }
    else if(msgid == 74 && !inpalace) {
      addMessage(XLAT("\"Did you know that the Cultists are relatives of the Desert Men?\"", m));
      }
    else if(msgid == 80) {
      addMessage(XLAT("\"Are there Temples of Cthulhu in your world? Why not?\"", m));
      }    
    else {
      msgid++; goto retry;
      }
  
    msgid++;
    }

  EX void playernear(cell *c) {
    info *i = getPrisonInfo(c);
    int d = dist(c);
    // if(i) printf("d=%d bn=%d\n", d, i->bestnear);
    if(i && d < i->bestnear) {
      changes.value_keep(*i);
      if(i->bestnear > 100 && d <= 100) {
        i->value = items[itPalace];
        if(princess::challenge) 
          addMessage(XLAT("Hardness frozen at %1.", its(i->value)));
        }
      i->bestnear = d;
      }
    }
    
EX }

EX namespace clearing {

  #if HDR
  struct clearingdata {
    cell *root;
    int dist;
    bool buggy;
    clearingdata() { root = nullptr; }
    };
  #endif

  EX std::map<heptagon*, clearingdata> bpdata;
  
  EX cell *current_root;
  
  EX void new_root() {
    if(!current_root || current_root->monst != moMutant) {
      auto& ac = currentmap->allcells();
      int iter = 0;
      while(!current_root || pseudohept(current_root) || current_root->cpdist < 3) {
        if(iter++ > 100) return;
        current_root = ac[hrand(isize(ac))];
        }
      current_root->monst = moMutant;
      current_root->mondir = NODIR;
      current_root->stuntime = (mutantphase + 1) & 15;
      }
    }
  
  int plantdir(cell *c) {
    if(have_alt(c))
      gen_alt_around(c);
    int d = celldistAlt(c);
    
    if(PURE) {
      forCellIdCM(c2, i, c) {
        if(!pseudohept(c2) && celldistAlt(c2) == d-1)
          return i;
        }
      forCellIdCM(c2, i, c) {
        if(celldistAlt(c2) == d-1)
          return geometry == gBinary4 ? i : (i+1) % c->type;
        }
      }

    forCellIdCM(c2, i, c) {
      if(!pseudohept(c2) && celldistAlt(c2) == d-1)
        return i;
      }

    int quseful = 0, tuseful = 0, tuseful2 = 0;

    forCellIdCM(c2, i, c) if(!pseudohept(c2)) {
      if(celldistAlt(c2) == d) {
        bool useful = false;
        for(int j=1; j<S6; j++) {
          cell *c3 = createMov(c2, j);
          if(celldistAlt(c3) == d-1)
            useful = true;
          }
        if(useful) quseful++, tuseful += (1<<i), tuseful2 = i;
        }
      }
    if(quseful == 1) return tuseful2;
    if(quseful == 2) {
      int i;
      if(tuseful == (1<<3)+(1<<5)) i = 3;
      if(tuseful == (1<<5)+(1<<1)) i = 5;
      if(tuseful == (1<<1)+(1<<3)) i = 1;
      if(tuseful == (1<<5)+(1<<7)) i = 5;
      if(tuseful == (1<<7)+(1<<1)) i = 7;
      if((d & 7) < 4) i = (i+2) % c->type;
      return i;
      }
    printf("error in plantdir\n");
    return 1;
    }

  vector<cell*> onpath;
  vector<int>   pdir;
  vector<cell*> rpath;
    
  EX void generate(cell *c) {
    if(euclid) {
      if(quotient) return; // fix cylinder
      if(pseudohept(c)) return;
      c->monst = moMutant;
      
      auto co = euc2_coordinates(c);
      int x = co.first, y = co.second;

      int xco = x * 2 + y + 1;
      c->stuntime = (8-xco/2) & 15;
      // 2, 4, 5, 7
      
      if(pseudohept(createMov(c, 0)))
        c->mondir = 1 + hrand(2) * 4;
      else
        c->mondir = 0;
      return;
      }
    
    if(!eubinary && !horo_ok()) return;
    // cell *oc = c;
    gen_alt(c);
    if(pseudohept(c)) return;
    heptagon *a = eubinary ? NULL : c->master->alt->alt;
    clearingdata& bd(bpdata[a]);
    if(!bd.root) { bd.root = c; bd.dist = 8; bd.buggy = false; }
    if(bd.buggy) return;
    
    onpath.clear(); pdir.clear(); rpath.clear();
    
    int steps = 0;
    
    int ds;
    
    int stepcount = 0;
    while(true) {
      if(c == bd.root) {ds = bd.dist; break; }

      // printf("R %4d C %4d\n", celldistAlt(bd.root), celldistAlt(c));
      if(celldistAlt(c) > celldistAlt(bd.root)) {
        stepcount++;
        if(stepcount > iteration_limit) {
          printf("buggy #1\n");
          bd.buggy = true;
          return;
          }

        if(c->mpdist <= 6) {
          if(c->monst != moMutant) return; // already cut!
          // ... else simply extend it
          ds = c->stuntime; break;
          }

        int d = plantdir(c);
        steps++;
        onpath.push_back(c); pdir.push_back(d);
        // printf("c [%4d] %p -> %p\n", celldistAlt(c), c, c->move(d));
        c = c->move(d);
        }
      else {
        bd.dist--;
        if(bd.dist < -iteration_limit) { 
          for(int i=0; i<steps; i++) 
            onpath[i]->item = itBuggy;
          for(int i=0; i<(int) rpath.size(); i++) 
            rpath[i]->item = itBuggy;
          printf("buggy #2\n");
          bd.buggy = true;
          return;
          }
        rpath.push_back(bd.root);
        // printf("r [%4d] %p -> %p\n", celldistAlt(bd.root), bd.root, bd.root->move(plantdir(bd.root)));
        bd.root = createMov(bd.root, plantdir(bd.root));
        }
      }
    
    // printf("steps = %d dist = %d [%d]\n", steps, bd.dist, oc->mpdist);
    
    onpath.push_back(c); pdir.push_back(plantdir(c));
    while(steps >= 0) {
      c = onpath[steps];
      if(steps == 0) {
        c->monst = moMutant;
        c->mondir = pdir[steps];
        if(pdir[steps] != plantdir(c)) {
          printf("pdir i/ plantdir\n");
          exit(1);
          }
        c->stuntime = ds & 15;
        }
      if(c->mpdist <= 7 && c->monst != moMutant)
        break;
      steps--; ds++;
      }
    }
  
  /** cells with the same celltype are likely to have the same number of descendant leaves */
  typedef tuple<int, int, int, int> celltype;
  
  /** stats about the number of descendant leaves for each celltype */
  map<celltype, pair<bignum, int> > stats;
  
  /** the total number of leaves killed, approximated from the actual numbers and Clearing structure */
  EX bignum imputed;

  /** the total number of leaves killed directly */
  EX int direct;
  
  map<cell*, pair<bignum, int> > score;
  
  celltype get_celltype(cell *c) {
    cell *c1 = c;
    if(c->mondir < c->type)
      c1 = c->move(c->mondir);
    auto& expansion = get_expansion();
    return make_tuple(
      celldistAlt(c), type_in(expansion, c, celldistAlt), 
      celldistAlt(c1), type_in(expansion, c1, celldistAlt)
      );
    }
  
  EX void imput(cell *c) {
    if(closed_manifold) return;
    if(score.count(c)) return;
    changes.map_value(score, c);
    auto& is = score[c];
    celltype t = get_celltype(c);
    changes.map_value(stats, t);
    auto& stat = stats[t];
    is.second = c->mondir;
    if(c->mpdist <= 6) {
      is.first = 1;
      forCellEx(c2, c) if(score.count(c2) && c2->move(score[c2].second) == c)
        is.first += score[c2].first;
      stat.first += is.first;
      stat.second++;
      }
    else {
      is.first = stat.second ? stat.first.randomized_div(stat.second) : bignum(1);
      imputed += is.first;
      }
    }

EX }

EX namespace whirlpool {

  EX bool escaped = false; // escaped the Whirlpool?

  // next == +1 -> next
  // next == -1 -> prev
  EX cell *get(cell *c, int next) {
    int i = 0;
    if(!eubinary && !c->master->alt) return NULL;
    int d = celldistAlt(c);
    int d2;
    while(true) {
      if(i == c->type) return NULL;
      if(c->move(i) && (d2 = celldistAlt(c->move(i))) != d)
        break;
      i++;
      }
    if(i == c->type) return NULL;
    if(d>d2) next = -next;
    for(int j=1; j<c->type; j++) {
      cell *c2 = c->modmove(i+next*j);
      if(celldistAlt(c2) == d) return c2;
      }
    return NULL;
    }
  
  void build(vector<cell*>& whirlline, int d) {
    again: 
    cell *at = whirlline[isize(whirlline)-1];
    cell *prev = whirlline[isize(whirlline)-2];
    if(looped(whirlline)) return;
    for(int i=0; i<at->type; i++) 
      if(at->move(i) && (eubinary || at->move(i)->master->alt) && celldistAlt(at->move(i)) == d && at->move(i) != prev) {
        whirlline.push_back(at->move(i));
        goto again;
        }
    }
  
  EX void generate(cell *wto) {
    if(wto->wall == waBoat || wto->monst)
      return;
    
    wto->wall = waSea;
    
    if(hrand(35000) < 40 + items[itWhirlpool] + yendor::hardness())
      wto->monst = moCShark;
    else if(hrand(5000) < 500)
       wto->wall = waBoat;
    
    if(wto->wall == waBoat && (eubinary || wto->master->alt)) { 
      int d = celldistAlt(wto);
      if(yendor::on) d -= 200;
      // 250 : hard
      if(hrand(5000) < 60 + 3 * items[itWhirlpool] + yendor::hardness())
        wto->monst = moPirate;
      if(hrand(5000) < 20 && d < -20 && !tactic::on && !inv::on)
        wto->item = itOrbSafety;
      else if(hrand(5000) < 20 && d < -20 && !tactic::on && markOrb(itOrbLuck))
        wto->item = itOrbSafety;
      else if(hrand(5000) < 20*PRIZEMUL && d < -20)
        placePrizeOrb(wto);
      else if(!inv::on && items[itWhirlpool] >= 10 && hrand(5000) < 20 && d < -15)
        wto->item = itOrbWater;
      else if(d<-10 && hrand(5000) < 1000-d)
        wto->item = itWhirlpool;
      }
    }
  
  void whirlMove(cell *wto, cell *wfrom) {
    // monsters don't move
    if(wfrom && (isPlayerOn(wfrom) || wfrom->monst))
      return;
    // disappear
    if(!wto) { wfrom->wall = waSea; wfrom->item = itNone; }
    
    if(wfrom && wto && wfrom->wall == waBoat && wto->wall == waSea && !wto->monst) {
      wfrom->wall = waSea; wto->wall = waBoat;
      wto->mondir = neighborId(wto, wfrom);
      animateMovement(moveimon(wto).rev(), LAYER_BOAT);
      }
    
    if(wfrom && wto && wfrom->item && !wto->item && wfrom->wall != waBoat) {
      // Keys and Orbs of Yendor never disappear!
      if(wfrom->item == itKey || wfrom->item == itOrbYendor)
        for(int i=0; i<wto->type; i++) createMov(wto, i);
      moveItem(wfrom, wto, false);
      pickupMovedItems(wfrom, wto);
      pickupMovedItems(wto, wfrom);
      }
    
    if(wto && !wfrom) 
      generate(wto);
    }
  
  void moveAt(cell *c, manual_celllister& cl) {
    if(c->land != laWhirlpool) return;
    if(cl.listed(c)) return;
    if(!(eubinary || c->master->alt)) return;
    cell *c2 = get(c, 1);
    if(!c2) return;
    int d = celldistAlt(c);
    vector<cell*> whirlline;
    whirlline.push_back(c);
    whirlline.push_back(c2);
    build(whirlline, d);
    reverse(whirlline.begin(), whirlline.end());
    build(whirlline, d);
    int z = isize(whirlline);
    
    for(int i=0; i<z; i++) 
      cl.add(whirlline[i]);
  
    whirlMove(NULL, whirlline[0]);
    
    for(int i=0; i<z-1; i++) 
      whirlMove(whirlline[i], whirlline[i+1]);
    
    whirlMove(whirlline[z-1], NULL);
    }
  
  EX void move() {
    manual_celllister cl;
    for(int i=0; i<isize(dcal); i++) {
      cell *c = dcal[i];
      moveAt(c, cl);
      }
    // Keys and Orbs of Yendor always move
    using namespace yendor;
    for(int i=0; i<isize(yi); i++) {
      moveAt(yi[i].path[0], cl);
      moveAt(yi[i].path[YDIST-1], cl);
      }
    }
  EX }

EX namespace mirror {

  #if HDR
  static const int SPINSINGLE = 1;
  static const int SPINMULTI = 2;
  static const int GO = 4;
  static const int ATTACK = 8;
  #endif
  
  EX bool build(cell *c) {
    if(aperiodic || sol) return false;
    #if CAP_GP
    if(GOLDBERG) {
      if(c == c->master->c7) {
        c->wall = ((gp::param.second == 0 || gp::param.first == gp::param.second) && hrand(2)) ? waMirror : waCloud;
        return true;
        }
      return false;
      }
    #endif
    #if CAP_ARCM
    if(arcm::in()) {
      c->wall = hrand(2) ? waMirror : waCloud;
      return true;
      }
    #endif
    if(bt::in() || IRREGULAR) {
      // mirrors not supported
      if(is_mirrorland(c)) {
        c->item = itShard;
        return true;
        }
      return false;
      }
    if(PURE?pseudohept(c):!ishept(c)) {
      c->wall = hrand(2) ? waMirror : waCloud;
      return true;
      }
    return false;
    }

  EX vector<pair<int, cellwalker>> mirrors;
  #if HDR
  constexpr int LIGHTNING = -1; // passed instead of cpid
  #endif
  
  bool noMirrorOn(cell *c) {
    return c->monst || (!shmup::on && isPlayerOn(c)) || (!closed_or_bounded && c->cpdist > gamerange());
    }

  bool cellMirrorable(cell *c) {
    if(noMirrorOn(c)) return false;
    return 
      c->wall == waNone || c->wall == waCavefloor || isAlchAny(c) ||
      c->wall == waFrozenLake || c->wall == waDeadfloor || c->wall == waDeadfloor2 ||
      c->wall == waGiantRug || c->wall == waCIsland || c->wall == waCIsland2 ||
      c->wall == waGargoyleFloor || c->wall == waRubble ||
      c->wall == waGargoyleBridge || c->wall == waTempFloor || c->wall == waTempBridge ||
      c->wall == waMirrorWall || c->wall == waPetrifiedBridge;
    }

  EX void destroyKilled() {
    int j = 0;
    for(int i=0; i<isize(mirrors); i++)
      if(mirrors[i].second.at->monst == moMimic)
        mirrors[j++] = mirrors[i];
    mirrors.resize(j);
    }
  
  void unlist() {
    for(auto& m: mirrors)
      if(m.second.at->monst == moMimic)
        changes.ccell(m.second.at),
        m.second.at->monst = moNone;
    }
  
  void list() {
    for(auto& m: mirrors)
      changes.ccell(m.second.at),
      m.second.at->monst = moMimic;
    }

  EX void destroyAll() {
    changes.value_keep(mirrors);
    unlist();
    mirrors.clear();
    }
  
  EX void createMirror(cellwalker cw, int cpid) {
    if(!shmup::on && inmirror(cw))
      cw = reflect(cw);
    changes.ccell(cw.at);
    if(cpid == LIGHTNING)
      castLightningBolt(cw);
    else if(cellMirrorable(cw.at)) {
      for(auto& m: mirrors)
        if(m == make_pair(cpid,cw))
          return;
      mirrors.emplace_back(cpid, cw);
      }
    }

  #if CAP_ARCM
  // we go by heptagons in Archimedean, 
  bool equal(heptspin h1, heptspin h2, int lev) {
    if(h1.at->degree() != h2.at->degree()) return false;
    if(arcm::is_vertex(h1.at) != arcm::is_vertex(h2.at)) return false;
    if(lev) for(int i=0; i<h1.at->degree(); i++) {
      heptspin h11 = h1 + i + wstep;
      heptspin h21 = h2 + i + wstep;
      if(!equal(h11, h21, lev-1)) return false;
      }
    return true;
    }
  
  int create_archimedean_rec(heptspin hs, int cpid, heptspin hs0, int lev) {
    int result = 0;
    for(int i=0; i<hs.at->degree(); i++) {
      heptspin hs1 = hs + i;
      if(lev == 0) {
        if(hs1.at != hs0.at && equal(hs1, hs0, 3)) {
          createMirror(hs1 + cth, cpid);
          result++;
          }
        }
      else result += create_archimedean_rec(hs1 + wstep, cpid, hs0, lev-1);
      }
    return result;
    }

  void create_archimedean(cellwalker cw, int cpid, bool mirrored) {
    heptspin hs = cw + cth;
    heptspin hsx = hs;
    if(mirrored) hsx += wmirror;
    if(create_archimedean_rec(hsx, cpid, hs, 2)) return;
    if(create_archimedean_rec(hsx, cpid, hs, 3)) return;
    if(create_archimedean_rec(hsx, cpid, hs, 4)) return;
    }
  #endif
  
  EX void createMirrors(cellwalker cw, int cpid) {
  
    if(aperiodic || sol) return;
    
    #if CAP_ARCM
    if(arcm::in()) {
      create_archimedean(cw, cpid, true);
      return;
      }
    #endif

    cw.mirrored = !cw.mirrored;
    cell *c = cw.at;
    
    #if MAXMDIM >= 4
    if(WDIM == 3 && !bt::in()) {
      if(shmup::on) for(int i=0; i<cw.at->type; i++)
        createMirror(cw + i + wstep - i, cpid);
      return;
      }
    #endif

    #if CAP_GP
    if(GOLDBERG) {
      for(int i=0; i<cw.at->type; i++) {
        createMirror(cw + cth + i + wstep + i - (gp::param.first == gp::param.second ? 1 : 0) + cth, cpid);
        }
      return;
      }
    #endif
    for(int i=0; i<cw.at->type; i++) {
      auto cws = cw + wstep;
      if(cws.at->type == c->type) 
        createMirror(cws+i, cpid);
      cw += 1;
      }
    }
  
  EX void createMirages(cellwalker cw, int cpid) {
    #if CAP_ARCM
    if(arcm::in()) {
      create_archimedean(cw, cpid, false);
      return;
      }
    #endif
    #if MAXMDIM >= 4
    if(WDIM == 3 && !bt::in()) {
      if(shmup::on) for(int i=0; i<cw.at->type; i++)
        createMirror(cw + i + wstep - i, cpid);
      return;
      }
    #endif
    #if CAP_GP
    if(GOLDBERG && !(S7 & 1)) {
      for(int i=0; i<cw.at->type; i++) {
        createMirror(cw + cth + i + wstep + 1 + wstep + 1 + (S7/2) - i + 1 + cth, cpid);
        }
      return;
      }
    if(GOLDBERG && (S7 & 1)) {
      for(int i=0; i<cw.at->type; i++) {
        createMirror(cw + cth + i + wstep + (S7/2) + wstep - 2 + wstep + (S7/2) - i + cth, cpid);
        }
      return;
      }
    #endif
    if(PURE && !(S7 & 1)) {
      for(int i=0; i<cw.at->type; i++)
        createMirror(cw + i + wstep + 1 + wstep + 1 + (S7/2) - i, cpid);
      return;
      }
    if(PURE && (S7 & 1) && (S3 == 4)) {
      for(int i=0; i<cw.at->type; i++) 
        createMirror(cw + i + wstep + 1 + wstep - (S7/2) + wstep - (S7/2) - i, cpid);
      return;
      }
    if(PURE && (S7 & 1)) {
      for(int i=0; i<cw.at->type; i++)
        createMirror(cw + i + wstep + (S7/2) + wstep - 2 + wstep + (S7/2) - i, cpid);
      return;
      }
    for(int i=0; i<S6; i++) {
      auto cw0 = cw + i + wstep;
      if(!ctof(cw0.at)) {
        createMirror(cw0 + 2 + wstep - (2+i), cpid);
        createMirror(cw0 - 2 + wstep + (2-i), cpid);
        }
      }
    }

  EX void createHere(cellwalker cw, int cpid) {
    if(!cw.at) return;
    if(cw.at->wall == waCloud)
      createMirages(cw, cpid);
    if(cw.at->wall == waMirror)
      createMirrors(cw, cpid);
    }
  
  EX void breakMirror(cellwalker cw, int pid) {
    if(!cw.at) return;
    cell *c = cw.at;
    if(c->wall == waMirror || c->wall == waCloud) {
      drawParticles(c, winf[c->wall].color, 16);
      playSound(c, "pickup-mirror", 50);
      if(pid >= 0 && (cw.at->land == laMirror || cw.at->land == laMirrorOld)) {
        dynamicval<int> d(multi::cpid, pid);
        gainShard(cw.at, c->wall == waMirror ? "The mirror shatters!" : "The cloud turns into a bunch of images!");
        }
      changes.ccell(c);
      c->wall = waNone;
      }
    }
  
  EX bool isKilledByMirror(cell *c) {
    for(auto& m: mirrors) {
      cell *c1 = (m.second + wstep).at;
      if(inmirror(c1)) c1 = reflect(cellwalker(c1, 0, false)).at;
      if(c1 == c && canAttack(m.second.at, moMimic, c, c->monst, 0))
        return true;
      }
    return false;
    }
   
  void go(bool fwd) {
    changes.value_keep(mirrors);
    int tk = tkills();
    int nummirage = 0;    
    int j = 0;
    
    for(int i=0; i<isize(mirrors); i++) {
      auto& m = mirrors[i];
      bool survive = true;
      if(m.first == multi::cpid) {
        cell *c = m.second.at;
        changes.ccell(c);
        if(!m.second.mirrored) nummirage++;
        auto cw2 = m.second + wstep;
        bool thru = inmirror(cw2);
        if(thru) cw2 = reflect(cw2);
        cell *c2 = cw2.at;
        changes.ccell(c2);
        if(c2->monst) {
          c->monst = moMimic;
          eMonster m2 = c2->monst;
          if(!peace::on && canAttack(c,moMimic,c2,m2, 0)) {
            attackMonster(c2, AF_NORMAL | AF_MSG, moMimic);
            if(!fwd) produceGhost(c2, m2, moMimic);
            sideAttack(c, m.second.spin, m2, 0);
            }
          c->monst = moNone;
          }
        if(c2->wall == waBigTree)
          c2->wall = waSmallTree;
        else if(c2->wall == waSmallTree)
          c2->wall = waNone;
        if(fwd) {
          if(thru && c == c2 && isAlchAny(c) && !checkflags(P_ISFRIEND | P_MONSTER | P_MIRROR | P_MIRRORWALL, P_AETHER)) {
            survive = false;
            continue;
            }
          if(noMirrorOn(c2) || !passable_for(moMimic, c2, c, P_ISFRIEND | P_MONSTER | P_MIRROR | P_MIRRORWALL)) {
            survive = false;
            continue;
            }
          c->monst = moMimic;
          moveMonster(m.second);
          c2->monst = moNone;
          empathyMove(m.second);
          m.second = cw2;
          }
        }
      if(survive) {
        mirrors[j++] = m;
        }
      }
    mirrors.resize(j);
    achievement_count("MIRRORKILL", tkills(), tk);
    achievement_count("MIRAGE", nummirage, 0);
    }

  EX void act(int d, int flags) {
    changes.value_keep(mirrors);
    destroyKilled();
    unlist();
    if(multi::players == 1) multi::cpid = 0;
    bool spinning =  
      flags & (multi::players > 1 ? SPINMULTI : SPINSINGLE);
    if(spinning && d) {
      for(auto& m: mirrors)
        if(m.first == multi::cpid)
          m.second += d;
      }
    if(flags & ATTACK)
      go(flags & GO);
    list();
    }
  
  EX void breakAll() {
    changes.value_keep(mirrors);
    destroyKilled();
    unlist();
    if(numplayers() == 1)
      createHere(cwt, 0);
    else for(int i=0; i<numplayers(); i++)
      createHere(multi::player[i], i);
    for(int i=0; i<isize(mirrors); i++)
      createHere(mirrors[i].second, mirrors[i].first);
    if(numplayers() == 1)
      breakMirror(cwt, 0);
    else for(int i=0; i<numplayers(); i++)
      breakMirror(multi::player[i], i);
    for(int i=0; i<isize(mirrors); i++)
      breakMirror(mirrors[i].second, -1);
    list();
    }
  
  EX int mirrordir(cell *c) {
    if(c->type == 7) return c->bardir;

   int icount = 0, isum = 0;
   for(int i=0; i<6; i+=2) {
     if(createMov(c, i)->bardir == c->c.spin(i))
       icount++, isum+=i;
     }
   if(icount != 1) return -1;
   return isum;
   }
  
  pair<bool, cellwalker> traceback(vector<int>& v, cellwalker cw) {
    bool goout = false;
    for(int i=isize(v)-1; i>=0; i--) {
      if(v[i]) cw -= v[i];
      else { 
        cw += wstep;
        if(cw.at->land == laMirrorWall || cw.at->land == laMirror) goout = true;
        }
      }
    return make_pair(goout, cw);
    }
  
  int depth(cell *c) { return c->landparam & 255; }

  cellwalker reflect0(cellwalker cw) {
    int stepcount = 0;
    cellwalker cwcopy = cw;
    static vector<int> v;
    v.clear();
    while(true) {
      if(!inmirror(cw)) break;
      stepcount++; if(stepcount > 10000) {
         return cw; 
         }
      cell *c0 = (cw+wstep).at;
      int go = 0;
      if(!inmirror(c0)) go = 2;
      else if(depth(c0) && depth(c0) < depth(cw.at)) go = 1;
      if(go) {
        v.push_back(0);
        cw += wstep;
        if(go == 2) break;
        }
      else {
        v.push_back(1);
        cw += 1;
        }        
      }
    if(cw.at->land == laMirrorWall || cw.at->land == laMirrorWall2) {
      if(cw.at->type == 7) {
        while(cw.spin != cw.at->bardir) {
          cw += 1;
          v.push_back(1);
          stepcount++; if(stepcount > 10000) { printf("failhep\n"); return cw; }
          }
        if(PURE && (cw+wstep).at == cwcopy.at)
          v.pop_back();
        if(PURE && (cw+3+wstep).at->land == laMirrored && (cw+2+wstep).at->land == laMirrorWall) {
          cw += wmirror;
          auto p = traceback(v, cw);
          if(p.first) return p.second;
          cw += 2;
          v.push_back(2);
          cw += wstep;
          v.push_back(0);
          cw += 3;
          v.push_back(3);
          }
        }
      else {
        while((cw+wstep).at->type != 7) {
          cw ++;
          v.push_back(1);
          }
        int icount = 0;
        for(int i=0; i<3; i++) {
          if((cw+wstep).at->bardir == cw.at->c.spin(cw.spin))
            icount++;
          cw += 2;
          }
        if(icount >= 2) {
          cellwalker cwcopy = cw;
          for(int a=0; a<3; a++) for(int m=0; m<2; m++) {
            cellwalker cw = cwcopy;
            if(m) cw.mirrored = !cw.mirrored;
            cw += (a*2);
            auto p = traceback(v,cw);
            if(p.first) return p.second;
            }
          printf("icount >= 2 but failed\n");
          return cw;
          }
        while((cw+wstep).at->bardir != cw.at->c.spin(cw.spin)) {
          stepcount++; if(stepcount > 10000) { printf("fail2\n"); return cw; }
          cw += 2;
          v.push_back(1);
          v.push_back(1);
          }
        }
      }
    else v.pop_back();
    cw.mirrored = !cw.mirrored;
    cw = traceback(v,cw).second;
    return cw;
    }

  static const int CACHESIZE = 1<<12; // must be a power of 2
  static const int CACHEMASK = CACHESIZE-1;
    
  pair<cell*, cellwalker> cache[CACHESIZE];
  int nextcache;
  
  void clearcache() {
    for(int i=0; i<CACHESIZE; i++) cache[i].first = NULL;
    }

  EX cellwalker reflect(const cellwalker& cw) {
    if(!cw.at) return cw;
    if((cw.at->landparam & 255) == 0) {
      bool cando = false;
      forCellEx(c2, cw.at) if(c2->landparam & 255) cando = true;
      if(cando) buildEquidistant(cw.at);
      }
    if((cw.at->landparam & 255) == 0) return cw;
    int cid = (cw.at->landparam >> 8) & CACHEMASK;
    if(cache[cid].first != cw.at) {
      cid = nextcache++;
      nextcache &= CACHEMASK;
      cw.at->landparam &= ~ (CACHEMASK << 8);
      cw.at->landparam |= (cid << 8);
      cache[cid].first = cw.at;
      cellwalker cw0(cw.at, 0, false);
      cache[cid].second = reflect0(cw0);
      int tries = 64;
      while(inmirror(cache[cid].second.at) && tries--)
        cache[cid].second = reflect0(cache[cid].second);
      }
    cellwalker res = cache[cid].second + cw.spin;
    if(cw.mirrored) res.mirrored = !res.mirrored;
    return res;
    }

  EX }

EX namespace hive {

  EX int hivehard() {
    return items[itRoyalJelly] + hardness_empty();
    // 0, 5, 40, 135
    }

  EX eMonster randomHyperbug() {
    int h = hivehard();
    if(hrand(200) < h)
      return moBug2;
    return eMonster(moBug0 + hrand(BUGCOLORS));
    // 50: 25/25/50
    // 100: 
    }
  
  struct buginfo_t {
    cell *where;
    short dist[BUGCOLORS];
    };
  
  vector<buginfo_t> buginfo;
  
  vector<int> bugqueue[BUGCOLORS];
  vector<int> bugqueue4[BUGCOLORS];
  
  struct bugtomove_t {
    int dist, moves, index;
    bugtomove_t(int d, int m, int i) { dist=d; moves=m; index=i; }
    };
  
  bool operator < (const bugtomove_t& m1, const bugtomove_t& m2) {
    if(m1.dist != m2.dist) return m1.dist < m2.dist;
    if(m1.moves != m2.moves) return m1.moves < m2.moves;
    return false;
    }
  
  vector<bugtomove_t> bugtomove;
  vector<cell*> deadbug;
  vector<cell*> bugcellq;
  
  EX int bugcount[BUGCOLORS];
  
  bool isBugEnemy(cell *c, int k) {
    if(isPlayerOn(c) && !invismove && !peace::on) return true;
    if(!c->monst) return false;
    if(c->monst == moBug0+k) return false;
    if(isIvy(c)) return false;
    return true;
    }
  
  // list bugs and targets for each color
  #define BUGINF 29999
  
  void bugQueueInsert(int k, int i, int d) {
    if(buginfo[i].dist[k] > d) {
      if(buginfo[i].dist[k] != BUGINF) {
        printf("%d -> %d\n", buginfo[i].dist[k], d);
        }
      buginfo[i].dist[k] = d;
      bugqueue[k].push_back(i);
      }
    }
  
  void bugcell(cell *c) {
    int& i(c->listindex);
    if(i >= 0 && i < isize(buginfo) && buginfo[i].where == c)
      return;
    i = isize(buginfo);
    buginfo.resize(i+1);
    buginfo_t& b(buginfo[i]);
    b.where = c;
    for(int k=0; k<BUGCOLORS; k++) {
      b.dist[k] = BUGINF;
      bool havebug = false, haveother = false;
      for(int dir=0; dir<c->type; dir++) {
        cell *c2 = c->move(dir);
        if(c2 && isBugEnemy(c2,k) && canAttack(c,eMonster(moBug0+k),c2,c2->monst, AF_TOUGH | AF_NOSHIELD | AF_GETPLAYER)) {
          if(isBug(c2)) havebug = true;
          else haveother = true;
          }
        }
      if(havebug) bugQueueInsert(k, i, 0);
      else if(haveother) bugqueue4[k].push_back(i);
      }
  /*// bugs communicate if the distance is at most 2
    // also all nearby cells are inserted to the buginfo structure
    if(isize(buginfo) < 30000) {
      for(int dir=0; dir<c->type; dir++) {
        cell *c2 = c->move(dir);
        if(c2) {
          // if(isBug(c)) bugcellq.push_back(c2); => does not help...
          for(int t=0; t<c2->type; t++)
            if(c2->move(t) && isBug(c2->move(t)))
              bugcellq.push_back(c2), 
              bugcellq.push_back(c2->move(t));
          }
        }
      }*/
    
    // use pheromones!
    if(c->land == laHive && c->landparam > 1 && c->wall != waWaxWall) {
      c->landparam --;
      for(int dir=0; dir<c->type; dir++) {
        cell *c2 = c->move(dir);
        if(c2) {
          for(int t=0; t<c2->type; t++)
            if(c2->move(t))
              bugcellq.push_back(c2), 
              bugcellq.push_back(c2->move(t));
          }
        }
      }
    }
  
  int last_d = -1;
  
  void handleBugQueue(int k, int t) {
    int i = bugqueue[k][t];
    buginfo_t& b(buginfo[i]);
    cell *c = b.where;
    int d = b.dist[k];
    last_d = d;
    int goodmoves = 0;
    for(int dir=0; dir<c->type; dir++) {
      cell *c2 = c->move(dir);
      if(!c2) continue;
      if(c2->listindex < 0 || c2->listindex >= isize(buginfo)) continue;
      if(!passable(c, c2, P_MONSTER)) continue;
      int j = c2->listindex;
      if(buginfo[j].where != c2) continue;
      if(buginfo[j].dist[k] < d) goodmoves++;
      bugQueueInsert(k, j, d+1);
      }
    if(isBug(c) && c->monst == moBug0+k) {
      bugcount[c->monst - moBug0]++;
      bugtomove.push_back(bugtomove_t(d,goodmoves,i));
      }
    }
  
  bool fightspam(cell *c) {
    return c->cpdist >= gamerange() || 
      isMetalBeast(c->monst) || c->monst == moSkeleton ||
      isIvy(c->monst) || isMutantIvy(c->monst);
    }
  
  EX void movebugs() {
    buginfo.clear();
    for(int k=0; k<BUGCOLORS; k++) bugqueue[k].clear();
    for(int k=0; k<BUGCOLORS; k++) bugqueue4[k].clear();
    for(int k=0; k<BUGCOLORS; k++) bugcount[k] = 0;
    bugtomove.clear();
    deadbug.clear();
    
    int xdcs = isize(dcal); for(int i=0; i<xdcs; i++) bugcell(dcal[i]);
    for(int i=0; i<isize(bugcellq); i++) bugcell(bugcellq[i]);
    bugcellq.clear();
    
    // printf("buginfo = %d\n", isize(buginfo));
    
    for(int k=0; k<BUGCOLORS; k++) {
      int t = 0;
      last_d = -1;
      int invadist = 4 - (items[itRoyalJelly]+10) / 20;
      if(invadist<0) invadist = 0;
      for(; t<isize(bugqueue[k]) && last_d < invadist-1; t++) handleBugQueue(k, t);
      for(int u=0; u<isize(bugqueue4[k]); u++)
        bugQueueInsert(k, bugqueue4[k][u], invadist);
      bugqueue4[k].clear();
      for(; t<isize(bugqueue[k]); t++) handleBugQueue(k, t);
      }
    
    for(int k=0; k<BUGCOLORS; k++) {
      set<int> check;
      for(int t=0; t<isize(bugqueue[k]); t++) {
        if(check.count(bugqueue[k][t])) {
          printf("REPETITION! [%d]\n", t);
          }
        check.insert(bugqueue[k][t]);
        }
      }
    
    hrandom_shuffle(bugtomove);
    sort(bugtomove.begin(), bugtomove.end());
    
    int battlecount = 0;
    for(int t=0; t<isize(bugtomove); t++) {
      bugtomove_t& bm(bugtomove[t]);
      int i = bm.index;
        
      buginfo_t& b(buginfo[i]);
      cell *c = b.where;
      if(!isBug(c)) continue;
      if(c->stuntime) continue;
      eMonster m = c->monst;
      int k = (m - moBug0) % BUGCOLORS;
      int gmoves[8], q=0, bqual = -1;
      
      if(againstRose(c, NULL)) bqual = -40;

      auto getplayer = peace::on ? AF_GETPLAYER : 0;
  
      for(int dir=0; dir<c->type; dir++) {
        cell *c2 = c->move(dir);
        int qual = -10;
        if(!c2) continue;
        else if(againstRose(c, c2)) qual = -50;
        else if(isPlayerOn(c2) && peace::on) qual = -45;
        else if(canAttack(c, m, c2, c2->monst, getplayer))
          qual = c2->monst == moDeadBug ? -60: isBugEnemy(c2,k) ? 2 : -20;
        else if(!passable(c2, c, 0)) 
          qual = passable(c2, c, P_DEADLY) ? -30 : -60;
        else if(c2->listindex < 0 || c2->listindex >= isize(buginfo)) qual = -15;
        else if(buginfo[c2->listindex].where != c2) qual = -15;
        else if(buginfo[c2->listindex].dist[k] < b.dist[k])
          qual = 1;
        else if(buginfo[c2->listindex].dist[k] == b.dist[k])
          qual = 0;
        // printf("%d->#%d %d: %d\n", i, dir, c2->tmp, qual);
        if(qual > bqual) bqual = qual, q=0;
        if(qual == bqual) gmoves[q++] = dir;
        }
      
      if(!q) { if(c->land == laHive) c->landparam += 3; continue; }
      int d = gmoves[hrand(q)];
      movei mi(c, d);
      auto& c2 = mi.t;
      if(c2->monst || (isPlayerOn(c2) && !peace::on)) {
        eMonster killed = c2->monst;
        if(isPlayerOn(c2) && !peace::on) killed = moPlayer;
        if(isBug(killed)) battlecount++;
        else if(killed != moPlayer && !fightspam(c2))
          addMessage(XLAT("%The1 fights with %the2!", c->monst, killed));
        attackMonster(c2, AF_NORMAL | getplayer, c->monst);
        // killMonster(c);
        if(isBug(killed)) {
          c2->monst = moDeadBug, deadbug.push_back(c2);
          bugcount[killed - moBug0]--;
          }
  //    c->monst = moDeadBug, deadbug.push_back(c);
        }
      else {
        moveMonster(mi);
        // pheromones!
        if(c->land == laHive && c->landparam < 90) c->landparam += 5;
        if(c2->land == laHive && c2->landparam < 90) c2->landparam += 5;
  /*      if(c2->item == itRoyalJelly && !isQueen(m)) {
          // advance!
          c2->monst = eMonster(m+BUGCOLORS);
          c2->item = itNone;
          } */
        }
      }
    
    // cleanup
    for(int i=0; i<isize(deadbug); i++) deadbug[i]->monst = moNone;
    if(battlecount)
      addMessage(XLAT("The Hyperbugs are fighting!"));
      
    int maxbug = 0;
    for(int k=0; k<BUGCOLORS; k++) if(bugcount[k] > maxbug) maxbug = bugcount[k];
    
    achievement_count("BUG", maxbug, 0);
    }
  
  void bugcitycell(cell *c, int d) {
    int& i = c->listindex;
    if(i >= 0 && i < isize(buginfo) && buginfo[i].where == c)
      return;
    i = isize(buginfo);
    buginfo_t b;
    b.where = c;
    b.dist[0] = d;
    buginfo.push_back(b);
    }
  
  EX void createBugArmy(cell *c) {
    int k = randomHyperbug() - moBug0;
    int minbugs = 50, maxbugs = 50;
    int var = 5 + items[itRoyalJelly];
    if(var>25) var=25;
    // minbugs += 100; maxbugs += 100;
    minbugs -= var; maxbugs += var;
    maxbugs += items[itRoyalJelly];
    int numbugs = minbugs + hrand(maxbugs - minbugs + 1);
    
    /* int i = items[itRoyalJelly];
    int chance = 20 + 25 * i + 9000;
    // i=0: 16%
    // i=10: 73%
    // i=50: 1270 vs 6000
    eMonster m = eMonster(moBug0 + hrand(BUGCOLORS));
    if(c->wall) return;
    for(int i=0; i<c->type; i++) {
      cell *c2 = createMov(c,i);
      if(hrand(100+chance) < chance) {
        if(!c2->wall) c2->monst = m;
        for(int j=2; j<=c2->type-2; j++) {
          int jj = (j+c->spn[i]) % c2->type;
          cell *c3 = createMov(c2, jj);
          if(hrand(6000+chance) < chance && !c3->wall)
            c3->monst = m;
          }
        }
      }
    c->monst = eMonster(m + BUGCOLORS); */
  
    int gdir = -1;
    for(int i=0; i<c->type; i++) {
      if(c->move(i) && c->move(i)->mpdist < c->mpdist) gdir = i;
      }
    if(!gdir) return;
    cellwalker bf(c, gdir);
    int radius = 9;
    if(getDistLimit() <= 6) radius = 6;
    if(ls::any_chaos()) radius = 5;
    if(ls::any_chaos() && getDistLimit() <= 5) radius = 4;
    if(getDistLimit() <= 3) radius = 3;

    for(int i=(ls::any_chaos()?0:2); i<radius; i++) 
      bf += revstep;
    cell *citycenter = bf.at;
    buginfo.clear();
    
    
    // mark the area with BFS
    bugcitycell(citycenter, 0);
    for(int i=0; i<isize(buginfo); i++) {
      buginfo_t& b(buginfo[i]);
      cell *c = b.where;
      int d = b.dist[0];
      // ERRORS!
      if(c->land != laHive && c->land != laNone) return;
      if(c->bardir != NODIR) return;
      if(c->land == laHive && c->landparam >= 100) return;
      // bfs
      if(d < radius) for(int t=0; t<c->type; t++)
        bugcitycell(createMov(c,t), d+1);
      }
  
    // place everything
    for(int i=0; i<isize(buginfo); i++) {
      buginfo_t& b(buginfo[i]);
      cell *c = b.where;
      int d = b.dist[0];
      if(d <= 1 && c->wall == waNone)
        c->item = itRoyalJelly;
      preventbarriers(c);
      if(d == 9 || d == 6 || d == 3)
        c->barleft = eLand(d/3),
        c->barright = eLand(k);
      else
        c->barleft = laNone;
      if(numbugs && c->wall == waNone)
        c->monst = eMonster(moBug0 + k), numbugs--;
      c->land = laHive;
      // prevent barriers
      if(c->mpdist == INFD) c->mpdist = BUGLEV;
      }
    }

  EX }

#if HDR
inline float& HEAT(cell *c) { return c->LHU.heat; }
#endif

EX namespace heat {
  
  EX void affect(cell *c, double delta) {
    if(isIcyLand(c)) HEAT(c) += delta;
    }
  
  EX double absheat(cell *c) {
    if(c->land == laCocytus) return HEAT(c) -.6;
    if(c->land == laIce || c->land == laBlizzard || c->land == laEclectic) return HEAT(c) -.4;
    return 0;
    }
  
  EX double celsius(cell *c) { return absheat(c) * 60; }
  
  // adjust to the improved heat transfer algorithm in 9.4
  const float FIX94 = 1.5;

  EX vector<cell*> offscreen_heat, offscreen_fire; // offscreen cells to take care off

  EX void processheat(double rate IS(1)) {
    if(markOrb(itOrbSpeed)) rate /= 2;
    if(racing::on) return;
    int oldmelt = kills[0];    
    
    vector<cell*> offscreen2;
    
    manual_celllister cl;
    
    int gr = gamerange();
    
    for(cell *c: offscreen_heat) {
      if(c->cpdist > gr && !doall) {
        if(!cl.add(c)) continue; 
        if(isIcyLand(c)) {
          if(HEAT(c) < .01 && HEAT(c) > -.01)
            HEAT(c) = 0;
          else {
            HEAT(c) *= 1 - rate/10;
            offscreen2.push_back(c);
            }
          }
        }
      }
    
    offscreen_heat = std::move(offscreen2);
  
    for(int i=0; i<numplayers(); i++) {
      cell *c = playerpos(i);
      if(!c) continue;
      double xrate = (c->land == laCocytus && shmup::on) ? rate/3 : rate;
      if(PURE) xrate *= 1.7; // todo-variation
      if(!shmup::on) xrate /= FIX94;
      if(isIcyLand(c))
        HEAT(c) += (markOrb(itOrbWinter) ? -1.2 : 1.2) * xrate;
      }
    
    vector<cell*>& allcells = currentmap->allcells();

    int dcs = isize(allcells);
    
    vector<ld> hmods(dcs, 0);
    
    int divby = 10;
    if(S7 > 10) divby *= 2;
    if(arcm::in()) divby *= 2;
    if(WDIM == 3) divby *= 2;
    
    for(int i=0; i<dcs; i++) {
      cell *c = allcells[i];
      double xrate = (c->land == laCocytus && shmup::on) ? 1/3. : 1;
      if(PURE) xrate *= 1.7; // todo-variation
      if(!shmup::on) xrate /= FIX94;
      if(c->cpdist > gr && !doall) break;
  
      if(isIcyLand(c)) {
        ld hmod = 0;

        if(c->monst == moRanger) hmod += 3 * xrate;
        if(c->monst == moDesertman) hmod += 4 * xrate;
        if(c->monst == moAngryDie) hmod += 4 * xrate;
        if(c->monst == moMonkey) hmod += xrate;
        if(c->wall == waDeadTroll) hmod -= 2 * xrate;
        if(c->wall == waDeadTroll2) hmod -= 1.5 * xrate;
        if(c->wall == waBigStatue) hmod -= .5 * xrate;
        if(c->monst == moLesser || c->monst == moLesserM || c->monst == moGreater || c->monst == moGreaterM)
          hmod += (c->land == laCocytus ? 1.5 : 10) * xrate;
        if(c->monst == moGreaterShark)
          hmod += 2 * xrate;
        if(c->monst == moCultist) hmod += 3 * xrate;
        if(c->monst == moCultistLeader) hmod += 4 * xrate;
        if(c->monst == moPyroCultist) hmod += 6 * xrate;
        if(c->monst == moFireFairy) hmod += 6 * xrate;
        if(c->monst == moFireElemental) hmod += 8 * xrate;
        if(isDragon(c->monst)) hmod += 2 * xrate;
        if(c->monst == moGhost) hmod -= xrate;
        if(c->monst == moFriendlyGhost) hmod -= xrate;
        if(c->monst == moSkeleton) hmod -= .2 * xrate;
        if(c->monst == moDraugr) hmod -= .75 * xrate;
        if(c->monst == moWaterElemental) hmod -= xrate;
        if(c->monst == moAirElemental) hmod -= .4 * xrate;
        if(isFire(c)) hmod += 4 * xrate;
        if(isPrincess(c->monst)) hmod += (markEmpathy(itOrbWinter) ? -1.2 : 1.2) * xrate;
        
        auto ls = adj_minefield_cells(c);

        for(cell* ct: ls) {
          if(!isIcyLand(ct) && isFire(ct)) 
            hmod += xrate*.1;
          if(ct->land == laVolcano) 
            hmod += xrate * (ct->wall == waMagma ? .4 : .2);
          }
        
        for(cell* ct: ls) {
          if(!isIcyLand(ct)) {
            // make sure that we can still enter Cocytus,
            // it won't heat up right away even without Orb of Winter or Orb of Speed
            if(isPlayerOn(ct) && (c->land != laCocytus || markOrb(itOrbWinter))) 
              hmod += (markOrb(itOrbWinter) ? -1.2 : 1.2) / 4 * xrate;
            continue;
            }
          ld hdiff = absheat(ct) - absheat(c);
          hdiff /= divby;

          #if CAP_FIELD
          if(ct->land == laBlizzard && c->land == laBlizzard) {
            int v = (windmap::at(ct) - windmap::at(c)) & 255;
            if(v > 128) v -= 256;
            if(v < windmap::NOWINDFROM && v > -windmap::NOWINDFROM)
              hdiff = hdiff * (1 - v * 5. / windmap::NOWINDFROM);
            }
          #endif

          if(shmup::on && (c->land == laCocytus || ct->land == laCocytus))
            hdiff /= 3;
          // if(c->move(j)->cpdist > 7 && !quotient) hdiff += -HEAT(c) / 30;
          hmod += hdiff;
          }
        // printf("%d ", vsum);
        
        hmods[i] = hmod;
        }
      
      if(HEAT(c) && !doall)
        offscreen_heat.push_back(c);
      }
    
    #if HDR
    #define MELTCOLOR 0xA04040
    #endif
    
    for(int i=0; i<dcs; i++) {
      cell *c = allcells[i];
      if(!isIcyLand(c)) continue; 
      HEAT(c) += hmods[i] * rate;
      if(c->monst == moCrystalSage && HEAT(c) >= SAGEMELT) {
        addMessage(XLAT("%The1 melts away!", c->monst));
        fallMonster(c);
        }
      if(c->wall == waIcewall && HEAT(c) > .4) 
        drawParticles(c, MELTCOLOR, 4, 60),
        c->wall = waNone, kills[0]++;
      if(c->wall == waFrozenLake && HEAT(c) > (c->land == laCocytus ? .6 : .4)) 
        drawParticles(c, MELTCOLOR, 4, 60),
        playSound(c, "trapdoor", 50),
        c->wall = waLake, kills[0]++;
  
      if(c->wall == waLake && HEAT(c) < (c->land == laCocytus ? -.4 : .4) && c->monst != moGreaterShark) {
        c->wall = waFrozenLake;
        if(c->monst == moShark || c->monst == moCShark) {
          addMessage(XLAT("%The1 is frozen!", c->monst));
          fallMonster(c);
          }
        }
      }
  
    if(kills[0] != oldmelt) bfs();
    }

  vector<pair<cell*, int> > newfires;

  EX void processfires() {
    newfires.clear();
    
    vector<cell*> offscreen2;

    manual_celllister cl;
    
    vector<cell*>& allcells = currentmap->allcells();

    for(int x: {0,1}) for(cell *c: x==0 ? allcells : offscreen_fire) {
      if(!cl.add(c)) continue;
      
      if(isFireOrMagma(c)) {
        if(c->wall == waMagma) c->wparam = 20;
      
        cell *last = c->move(c->type-1);
        
        auto ls = adj_minefield_cells(c);
        for(cell* c2: ls) {
          
          if(c->wall == waPartialFire) {
            // two partial fires adjacent are necessary to spread
            bool ok = false;
            forCellEx(c3, c2) if(c3 != c && c3->wall == waPartialFire)
              ok = true;
            if(!ok) continue;
            }

          if(c2->wall == waNone && c2->land == laRose && c->wparam >= 10)
            newfires.emplace_back(c2, c->wparam);
          if(c2->wall == waFire && c2->land == laRose && c->wparam >= 10 && c2->wparam < c->wparam/2)
            newfires.emplace_back(c2, c->wparam);
          if(againstWind(c, c2) && c->wall != waEternalFire && c->wparam >= 10) {
            if(isFire(c2)) {
              if(c2->wparam < c->wparam/2) 
                newfires.emplace_back(c2, c->wparam);
              }
            else {
              newfires.emplace_back(c2, c->wparam);
              useup(c);
              }
            }
          if(c2->land == laDryForest && c2->wall != waEternalFire) {
            c2->landparam++;
            if(c2->landparam >= (isStandardTree(c2) ? 1 : 10)) newfires.emplace_back(c2, 12);
            else offscreen2.push_back(c);
            }
          else if(c2->wall == waVinePlant || c2->wall == waRose || c2->wall == waSaloon ||
            c2->wall == waWeakBranch || c2->wall == waCanopy || c2->wall == waTrunk || c2->wall == waSolidBranch ||
            c2->wall == waBigBush || c2->wall == waSmallBush || c2->wall == waBonfireOff || c2->wall == waSmallTree ||
            c2->wall == waDock)
            newfires.emplace_back(c2, 12);
          else if(cellHalfvine(c2) && last && last->wall == c2->wall)
            newfires.emplace_back(c2, 12);
          else if(c2->wall == waExplosiveBarrel)
            newfires.emplace_back(c2, 12);
          else if(c2->wall == waFireTrap)
            newfires.emplace_back(c2, 12);
          // both halfvines have to be near fire at once
          last = c2;
          }
        
        }

      if(hasTimeout(c)) {
        if(c->mpdist == 8 && (c->land == laWineyard || c->land == laEndorian || c->land == laDocks)) {
          // do not expire, do not store in 'offscreen', do not generate more land
          }
        else {
          useup(c);
          offscreen2.push_back(c);
          }
        }
      
      if(c->wall == waArrowTrap && c->wparam && !shmup::on) {
        c->wparam++;
        if(c->wparam == 3) {
          if(canAttack(c, moArrowTrap, c, c->monst, AF_GETPLAYER))
            attackMonster(c, AF_NORMAL | AF_MSG | AF_GETPLAYER, moArrowTrap);
          }
        if(c->wparam == 4) c->wparam = 0;
        }

      if(c->wall == waFireTrap && c->wparam && !shmup::on) {
        c->wparam++;
        if(c->wparam == 3) {
          c->wall = waNone;
          explosion(c, 5, 10);
          }
        }
      }

   for(int i=0; i<isize(newfires); i++) {
      cell* c = newfires[i].first;
      int qty = newfires[i].second;
      qty /= 2;
      if(c->wall == waExplosiveBarrel) explodeBarrel(c);
      else if(c->wall == waFireTrap) {
        if(c->wparam == 0) c->wparam = 1;
        continue;
        }
      else if(c->wall == waBonfireOff) activateActiv(c, false);
      else if(cellHalfvine(c)) destroyHalfvine(c, waPartialFire, 6);
      else makeflame(c, qty, false);
      if(c->wparam < qty) c->wparam = qty;
      offscreen2.push_back(c);
      if(c->land == laRose || c->land == laWildWest || c->land == laOvergrown || isHaunted(c->land) || c->land == laMountain || c->land == laIce) {
        for(int j=c->mpdist-1; j>=7; j--) setdist(c, j, NULL);
        }
      }
   
    offscreen_fire = std::move(offscreen2);
    }  

EX }

EX void livecaves() {
  vector<cell*>& allcells = currentmap->allcells();
  int dcs = isize(allcells);
  
  vector<cell*> bringlife;
  int gr = gamerange();
  
  std::vector<int> heatvals(dcs);
  
  for(int i=0; i<dcs; i++) {
    cell *c = allcells[i];
    if(!doall && c->cpdist > gr+1) break;
    
    int & hv = heatvals[i];
    
    if(c->wall == waCavefloor || c->wall == waCavewall || c->wall == waDeadTroll) {
      hv = 0;
      if(c->monst == moDarkTroll) c->monst = moTroll;
      if(c->item || c->monst || c->cpdist == 0) continue;
      for(cell *c2: adj_minefield_cells(c)) {
        eWall w = c2->wall;
        if(w == waDeadfloor) hv++, bringlife.push_back(c2);
        else if(w == waDeadwall || (w == waDeadfloor2 && !c2->monst && !isPlayerOn(c2)))
          hv--, bringlife.push_back(c2);
        else if(w == waCavefloor) hv++;
        else if(w == waCavewall) hv--;
        else if(w == waRubble) hv--;
        else if(w == waGargoyle) hv--;
        else if(w == waGargoyleFloor) hv--;
        else if(w == waGargoyleBridge) hv--;
        else if(w == waStone) ;
        else if(w == waDeadTroll) hv -= 5;
        else if(w == waDeadTroll2) hv -= 3;
        else if(w == waPetrified || w == waPetrifiedBridge) hv -= 2;
        else if(w == waVinePlant) hv--;
        else if(ls::any_chaos() && c2->land != laCaves && c2->land != laEmerald) ;
        else if(c2->land == laTrollheim) ; // trollheim floor does not count
        else if(w != waBarrier) hv += 5;
        
        if(sword::at(c)) hv += 500;

        if(c2->cpdist == 0 && markOrb(itOrbDigging)) hv+=100;
        if(items[itOrbEmpathy] && isFriendly(c2) && markEmpathy(itOrbDigging))
          hv+=100;
        if(w == waThumperOn) hv+=100;
        if(w == waFire) hv+=100;
        if(w == waBigStatue) hv-=100;
        if(c2->item && !peace::on) hv+=2;
        if(c2->monst == moZombie) hv += 10;
        if(c2->monst == moGhost) hv += 10;
        if(c2->monst == moTentacleGhost) hv += 10;
        if(c2->monst == moFriendlyGhost) hv += 10;
        if(c2->monst == moSkeleton) hv ++;
        if(c2->monst == moGargoyle) hv--;
        if(c2->monst == moDraugr) hv--;
        if(isDragon(c2->monst)) hv++;
        if(c2->monst == moNecromancer) hv += 10;
        if(c2->monst == moWormtail) hv++;
        if(c2->monst == moTentacletail) hv-=2;
        if(c2->monst == moAngryDie) hv++;
        if(isIvy(c2)) hv--;
        if(isDemon(c2)) hv-=3;
        // if(c2->monst) c->tmp++;
        // if(c2->monst == moTroll) c->tmp -= 3;
        }
      }
    else if(c->land == laLivefjord) {
      hv = 0;
      if(c->monst == moWaterElemental)
        hv += 1000;
      if(isPlayerInBoatOn(c) && markOrb(itOrbWater))
        hv += 1000;
      if(c->monst == moEarthElemental)
        hv -= 1000;
      if(isPlayerOn(c) && markOrb(itOrbDigging))
        hv -= 1000;
      for(int j=0; j<c->type; j++) if(c->move(j)) {
        cell *c2 = c->move(j);
        if(c2->wall == waNone || c2->wall == waStrandedBoat)
          hv -= (c2->land == laLivefjord ? 1 : 100);
        if(c2->wall == waTempFloor || c2->wall == waTempBridge || c2->wall == waTempBridgeBlocked)
          ;
        else if(c2->wall == waDeadTroll || c2->wall == waDeadTroll2 || c2->wall == waThumperOn || isFire(c2) || snakelevel(c2))
          hv -= 10;
        else if(c2->wall == waPetrified || c2->wall == waPetrifiedBridge)
          hv -= 10;
        if(c2->wall == waBigStatue)
          hv -= 10;
        if(c2->wall == waSea || c2->wall == waBoat)
          hv += (c2->land == laLivefjord ? 1 : 100);
        if(c2->monst == moWaterElemental)
          hv += 1000;
        if(isPlayerOn(c2) && c2->wall == waBoat && markOrb(itOrbWater))
          hv += 1000;
        if(c2->monst == moEarthElemental)
          hv -= 1000;
        if(isPlayerOn(c2) && markOrb(itOrbDigging))
          hv -= 1000;
        if(items[itOrbEmpathy] && isFriendly(c2) && markEmpathy(itOrbDigging))
          hv -= 1000;

        if(c2->wall == waBarrier) {
          bool landbar = false;
          for(int k=0; k<c2->type; k++)
            if(c2->move(k)) {
              cell *c3 = c2->move(k);
              if(!isSealand(c3->land))
                landbar = true;
              }
          if(landbar) hv -= 5;
          else hv += 5;
          }
        }
      }
    }

  for(int i=0; i<dcs; i++) {
    cell *c = allcells[i];
    if(!doall && c->cpdist > gr+1) break;
    int hv = heatvals[i];

    if(c->wall == waCavefloor || c->wall == waCavewall) {
  //  if(c->land != laCaves) continue;
  //  if(c->wall == waThumper || c->wall == waBonfire) continue;
      
      if(hv > 0) c->wall = waCavefloor;
      if(hv < 0) {
        c->wall = waCavewall;
        if(c->land != laCaves && c->land != laDeadCaves && c->land != laEmerald)
          achievement_gain_once("GARDENER");
        }
      }
    else if(c->land == laLivefjord) {
      if(hv > 0 && c->wall == waStrandedBoat) c->wall = waBoat;
      if(hv > 0 && c->wall == waNone) {
        if(c->item && c->cpdist == 1 && markOrb(itOrbWater)) {
          bool saf = c->item == itOrbSafety;
          collectItem(c, c);
          if(saf) return;
          }
        c->wall = waSea;
        }
      if(hv < 0 && c->wall == waBoat) c->wall = waStrandedBoat;
      if(hv < 0 && c->wall == waSea) c->wall = waNone;
      }
    }
  
  for(int i=0; i<isize(bringlife); i++) {
    cell *c = bringlife[i];
    if(c->land == laDeadCaves)
      achievement_gain_once("LIFEBRINGER");
    if(c->wall == waDeadfloor) c->wall = waCavefloor;
    if(c->wall == waDeadfloor2) c->wall = waCavewall;
    if(c->wall == waDeadwall) c->wall = waCavewall;
    if(c->wall == waCavewall && c->item) c->wall = waCavefloor;
    if(c->land == laDeadCaves) c->land = laCaves;
    if(c->item == itSilver) c->item = itGold;
    if(c->item == itGreenStone && !inv::on) c->item = itOrbLife;
    if(c->monst == moEarthElemental) {
      addMessage(XLAT("%The1 is destroyed by the forces of Life!", c->monst));
      fallMonster(c);
      c->item = itOrbDigging;
      }
    }
  }

/* evolver */

EX namespace tortoise {
  EX map<cell*, int> emap;
  EX map<cell*, int> babymap;
  EX int last;
  
  #if HDR
  enum tflag {
    tfShell, tfScute0, tfScute1, tfScute2, tfScute3,
    tfEdge1, tfEdge, tfEdge3,
    tfLongNeck, tfFront, tfRear, tfTail,
    tfEyeHue, tfShellHue, tfScuteHue, tfSkinHue,
    tfShellSat, tfScuteSat, tfSkinSat,
    tfShellDark, tfSkinDark,
    tfCOUNT
    };
  #endif

  EX const int numbits = (int) tfCOUNT;
  EX const int mask = (1<<numbits)-1;

  EX int getb(cell *where) {
    if(emap.count(where)) return emap[where];
    return getBits(where) & ((1<<numbits)-1);
    }

  EX int countBits(int c) {
    int bi = 0;
    for(int i=0; i<numbits; i++) if((c >> i)&1) bi++;
    return bi;
    }
  
  EX int getBit(int bits, int id) { return (bits >> id) & 1; }

  EX int getRandomBits() { return hrand(1 << numbits); }
  
  EX bool seek() { return items[itBabyTortoise] % 5; }
  EX int seekbits;
  double seekval[numbits];
  double currval[numbits];
  
  void update(double& val, double target, int delta) {
    double d = delta / 300.;
    if(d>1) d = 1;
    if(target>val+d) val += d;
    else if(target<val-d) val -= d;
    else val = target;
    }
  
  EX bool shading_enabled = true;
  
  EX bool shading_on() {
    return shading_enabled && seek();
    }
  
  EX void updateVals(int delta) {
    int currbits = getBits(cwt.at);
    for(int i=0; i<numbits; i++)
      update(seekval[i], shading_on() ? getBit(seekbits, i) : .5, delta);
    for(int i=0; i<numbits; i++)
      update(currval[i], getBit(currbits, i), delta);
    }
  
  EX double getScent(int bits) {
    double res = 0;
    for(int i=0; i<numbits; i++)
      /* if(getBit(bits, i) != getBit(getBits(cwt.at), i))
        res += (1 - 2*getBit(bits, i)); */
      res += (2* seekval[i] - 1) * (getBit(bits, i) - currval[i]);
    
    // seek curr bit => res
    //    1    1   1 => 0
    //    1    1   0 => -1
    //    1    0   1 => +1
    //    1    0   0 => 0
    //    0    1   1 => 0
    //    0    1   0 => +1
    //    0    0   1 => -1
    //    0    0   0 => 0
    return res;
    }
  
  EX int diff(int bits) { return countBits(bits ^ tortoise::seekbits); }
  int progress(int bits) { return numbits - diff(bits); }
  
  EX string measure(int bits) {
    return "(" + its(progress(bits)) + "/" + its(tortoise::numbits) + ")";
    }    

  EX void move_baby(cell *c1, cell *c2) {
    swap_data(babymap, c1, c2);
    }

  EX void move_adult(cell *c1, cell *c2) {
    swap_data(emap, c1, c2);
    }
EX }

EX namespace dragon {
 
  EX int whichturn; // which turn has the target been set on
  EX cell *target; // actually for all Orb of Domination

  void pullback(cell *c) {
    int maxlen = iteration_limit;
    while(maxlen-->0) {
      auto mi = moveimon(c).rev();
      mountmove(mi, true);
      c->monst = mi.s->monst;
      c->hitpoints = mi.s->hitpoints;
      animateMovement(mi, LAYER_BIG);
      c->stuntime = 2;
      if(mi.s->mondir == NODIR) { c->mondir = NODIR; mi.s->monst = moNone; return; }
      c = mi.s;
      }
    }
  
  bool dragbugs = false;
  
  EX cell *findhead(cell *c) {
    cell *cor = c;
    int maxlen=iteration_limit;
    findhead:  
    if(maxlen--<0) return c;
    if(c->monst == moDragonHead) return c;
    for(int i=0; i<c->type; i++)
      if(c->move(i) && isDragon(c->move(i)->monst) && c->move(i)->mondir == c->c.spin(i)) {
        c = c->move(i); goto findhead;
        }
    if(cmode & sm::MAP) return c;
    if(!history::includeHistory) {
      printf("dragon bug #3 (%p -> %p)\n", hr::voidp(cor), hr::voidp(c)); 
      dragbugs = true;
      }
    c->monst = moDragonHead; return c;
    }
  
  void validate(const char *where) {
    dragbugs = false;
    for(int i=0; i<isize(dcal); i++)
      if(dcal[i]->monst == moDragonTail)
        findhead(dcal[i]);
    if(dragbugs) {
      printf("DRAGON BUG in %s\n", where);
      exit(1);
      }
    }
  
  EX int bodypart(cell *c, cell *head) {
    int i = 0, j = 0;
    int maxlen = iteration_limit;
    while(maxlen-->0) {
      if(head == c) i = j;
      j++;
      if(head->mondir == NODIR) break;
      head = head->move(head->mondir);
      }
    if(i == 0) return 'h';
    if(i == 1) return 'l';
    if(i == j-2) return '2';
    if(i == j-1) return 't';
    if(i == 2) return 'w';
    return 0;
    }
  
  EX void kill(cell *c, eMonster who) {
    int delay = false;
    kills[moDragonHead]++;
    int penalty = 0;
    int maxlen = iteration_limit;
    while(maxlen-->0) {
      changes.ccell(c);
      makeflame(c, 5, false);
      eMonster m = c->monst;
      drawFireParticles(c, 16);
      c->monst = moNone;
      if(checkOrb(who, itOrbUndeath)) 
        c->monst = moFriendlyGhost;
      if(checkOrb(who, itOrbStone)) 
        c->wparam = m, c->wall = waPetrified;
      else if(c->wall == waFire) {
        if(delay) delay = false;
        else {
          if(c->land != laDragon) penalty += 3;
          if(penalty) penalty--;
          else {
            c->item = itDragon;
            c->landparam = shmup::on ? shmup::curtime : turncount;
            delay = true;
            }
          }
        }
      if(c->mondir == NODIR) break;
      c = c->move(c->mondir);
      }
    }
  
  EX int totalhp(cell *c) {
    int total = 0;
    int maxlen = iteration_limit;
    while(maxlen-->0) {
      if(!isDragon(c->monst)) {
        if(!history::includeHistory) printf("dragon bug #4\n");
        return total; 
        }
      total += c->hitpoints;
      if(c->mondir == NODIR) return total;
      c = c->move(c->mondir);
      if(!c) return total;
      }
    return total;
    }
  
  #define SWAPBITFIELD(x,y,t) { t bak=x; x=y; y=bak; }
  
  void pullfront(cell *c, cell *until) {
    int maxlen = iteration_limit;
    static vector<cell*> allcells;
    allcells.clear();
    while(maxlen-->0) {
      allcells.push_back(c);
      // SWAPBITFIELD(c->monst, buffer->monst, eMonster);
      // SWAPBITFIELD(c->hitpoints, buffer->hitpoints, int);
      c->stuntime = 2;
      if(c == until) {
        for(int i=isize(allcells)-2; i>=0; i--) {
          cell *cmt = allcells[i+1];
          cell *cft = allcells[i];
          cmt->hitpoints = cft->hitpoints;
          cmt->monst = cft->monst;
          cft->monst = moNone;
          auto mi = moveimon(cft);
          mountmove(mi, true);
          animateMovement(mi, LAYER_BIG);
          }
        while(c->mondir != NODIR) {
          c = c->move(c->mondir);
          if(!c) return;
          c->stuntime = 2;
          }
        break;
        }
      if(c->mondir == NODIR) { printf("dragon bug\n"); break; }
      c = c->move(c->mondir);
      if(!c) { 
        if(!history::includeHistory) printf("dragon bug #2\n"); 
        break; 
        }
      }
    }
  
  EX bool move(const movei& mi) {
    auto& df = mi.s;
    auto& dt = mi.t;
    if(df->monst == moDragonHead) {
      dt->mondir = neighborId(dt,df);
//    printf("pull back\n");
      pullback(dt);
      dt->stuntime = 2;
      return true;
      }
    if(df->monst == moDragonTail && df->stuntime == 0) {
      cell *head = findhead(df);
      if(df->mondir == NODIR) {
        df->mondir = neighborId(df,dt);
        dt->mondir = NODIR;
//      printf("pull all: head = %p, df=%p, dt=%p\n", head, df, dt);
        pullfront(head, dt);
        }
      else {
        cell *c2 = df->move(df->mondir);
        if(!c2) return false;
        int id = neighborId(dt, c2);
        if(id == -1) return false;
        dt->mondir = id;
        df->mondir = neighborId(df, dt);
//      printf("pull front: head = %p, df=%p, dt=%p\n", head, df, dt);
        pullfront(head, dt);
        }
      return true;
      }
    return false;
    }

EX }

EX namespace sword {

  #if HDR
  struct sworddir {
    int angle;
    transmatrix T;
    };
  
  /** dimensions available to the Sword */
  #define SWORDDIM (mhybrid ? 2 : WDIM)
    
  #endif

  EX int sword_angles;
  
  EX array<sworddir, MAXPLAYER> dir;

  void possible_divisor(int s) { sword_angles *= s / gcd(sword_angles, s); }

  EX void determine_sword_angles() {
    sword_angles = 2;
    if(SWORDDIM == 3) sword_angles = 1;
    #if CAP_IRR
    else if(IRREGULAR) sword_angles = 840;
    #endif
    #if CAP_BT
    else if(bt::in()) sword_angles = 42;
    #endif
    #if CAP_ARCM
    else if(arcm::in()) {
      if(!PURE) possible_divisor((BITRUNCATED ? 2 : 1) * isize(arcm::current.faces));
      if(!DUAL) for(int f: arcm::current.faces) possible_divisor(f);
      }
    #endif
    else {
      possible_divisor(S7);
      if(BITRUNCATED) possible_divisor(S3);
      }
    }
  
  EX cell *pos2(cell *c, int s) {
    int t = c->type;
    if(mhybrid) t -= 2;
    s *= 2;
    s += sword_angles/t;
    s %= (2 * sword_angles);
    if(s<0) s += sword_angles * 2;
    s *= t;
    s /= (sword_angles * 2);
    auto c1 = c->move(s);
    if(inmirror(c1)) c1 = mirror::reflect(c1).at;
    return c1;
    }
  
  EX cell *pos(cell *c, const sworddir& sd, bool rev) {
    if(SWORDDIM == 2) 
      return pos2(c, sd.angle + (rev ? sword_angles/2 : 0));
    else {
      cell *best = NULL;
      ld bdist = HUGE_VAL;
      for(int i=0; i<S7; i++) {
        ld dist = hdist(sd.T * xpush0(rev?-0.1:0.1), tC0(currentmap->relative_matrix(c->move(i)->master, c->master, C0)));
        if(dist < bdist) bdist = dist, best = c->move(i);
        }
      return best;
      }
    }
  
  eItem orbof(bool rev) { return rev ? itOrbSword2 : itOrbSword; }
  EX int orbcount(bool rev) { return items[orbof(rev)]; }
  
  EX cell *pos(int id, bool rev) {
    if(!orbcount(rev)) return NULL;
    return pos(playerpos(id), dir[id], rev);
    }
  
  EX bool at(cell *where, bool noplayer IS(false)) {
    if(noplayer) return false;
    if(!orbcount(0) && !orbcount(1)) return false;
    for(int i=0; i<numplayers(); i++) if(multi::playerActive(i)) for(int b=0; b<2; b++)
      if(pos(i,b) == where) return true;
    return false;
    }
  
  EX bool isnear(cell *where) {
    if(at(where, false)) return true;
    forCellEx(w2, where) if(at(w2, false)) return true;
    return false;
    }

  // from c1 to c2
  EX sworddir shift(movei mi, sworddir d) {
    cell *c1 = mi.s;
    cell *c2 = mi.t;
    if(!mi.proper()) return d;
    int s1 = mi.d;
    int s2 = mi.rev_dir_force();
    neighborId(c2, c1);
    if(s1 < 0 || s2 < 0) return d;
    if(SWORDDIM == 2) {
      int sub = (mhybrid) ? 2 : 0;
      int t2 = c2->type - sub;
      int t1 = c1->type - sub;
      if(t1 == 0 || t2 == 0) return d;
      if(c1->c.mirror(s1))
        d.angle = ((s2*sword_angles/t2 - d.angle + s1*sword_angles/t1) + sword_angles/2) % sword_angles;
      else
        d.angle = ((s2*sword_angles/t2 - s1*sword_angles/t1) + sword_angles/2 + d.angle) % sword_angles;
      }
    else {
      transmatrix T = currentmap->relative_matrix(c1, c2, C0);
      T = gpushxto0(tC0(T)) * T;
      d.T = T * d.T;
      fixmatrix(d.T);
      }
    return d;
    }
  
  transmatrix initial_matrix() {
    if(S7 == 6) return Id;
    return cspin(0, 1, 0.1) * cspin(0, 2, 0.1) * cspin(1, 2, 0.1) * Id;
    }

  EX sworddir initial(cell *c) {
    sworddir res;
    res.angle = (sword::sword_angles / cwt.at->type + 1) / 2;
    if(SWORDDIM == 3) res.T = initial_matrix();
    return res;
    }

  void shuffle(int i) {
    changes.value_keep(dir[i].angle);
    changes.value_keep(dir[i].T);
    dir[i].angle = hrand(sword_angles);
    if(SWORDDIM == 3) dir[i].T = initial_matrix();
    }
  
  EX void reset() {
    items[itOrbSword] = items[itOrbSword2] = 0;
    shuffle(multi::cpid);
    }

  void shuffle() {
    for(int i=0; i<MAXPLAYER; i++) shuffle(i);
    }
EX }

EX namespace kraken {

  EX cell *head(cell *c) {
    if(c->monst == moKrakenH) return c;
    if(c->monst == moKrakenT) return c->move(c->mondir);
    return NULL;
    }
 
  EX void kill(cell *c, eMonster who) {
    changes.ccell(c);
    drawParticles(c, minf[moKrakenH].color, 16);
    c->monst = moNone;
    if(checkOrb(who, itOrbUndeath)) c->monst = moFriendlyGhost;
    if(c->land == laKraken && !c->item) c->item = itKraken;
    kills[moKrakenH]++;
    if(checkOrb(who, itOrbStone)) c->wall = waNone;
    forCellEx(c1, c) 
      if(c1->monst == moKrakenT) {
        changes.ccell(c1);        
        drawParticles(c, minf[moKrakenT].color, 16);
        c1->monst = moNone;
        if(checkOrb(who, itOrbStone)) {
          if(isWatery(c1))
            c1->wall = waNone;
          else
            c1->wall = waPetrified, c1->wparam = moKrakenT;
          }
        }
    }
  
  EX int totalhp(cell *c) {
    int total = 0;
    forCellEx(c1, c)
      if(c1->monst == moKrakenT)
        total += c1->hitpoints;
    return total;
    }
  
  void trymove(cell *c);
  
  void sleep(cell *c) {
    if(c->monst == moKrakenT) c = c->move(c->mondir);
    c->stuntime = 1;
    forCellEx(c2, c) c2->stuntime = 1;
    }
  
  EX void attacks() {
    bool offboat[MAXPLAYER];
    for(int i=0; i<MAXPLAYER; i++) offboat[i] = false;
    for(int i=0; i<isize(dcal); i++) {
      cell *c = dcal[i];
      if(c->monst == moKrakenT && !c->stuntime) forCellEx(c2, c) {
        if (!logical_adjacent(c2,moKrakenT,c)) continue;
        bool dboat = false;
        if(c2->monst && canAttack(c, moKrakenT, c2, c2->monst, AF_ONLY_FBUG)) {
          attackMonster(c2, AF_NORMAL | AF_MSG, c->monst);
          sleep(c);
          }
        else for(int i=0; i<numplayers(); i++) if(playerpos(i) == c2 && !peace::on) {
          if(isPlayerInBoatOn(c2, i)) {
            addMessage(XLAT("%The1 destroys your boat!", moKrakenH));
            dboat = true;
            offboat[i] = true;
            }
          else if(offboat[i]) ;
          else killThePlayer(moKrakenH, i, 0);
          sleep(c);
          }
        if(dboat) destroyBoats(c2, c, true);
        }
      }
    for(int i=0; i<isize(dcal); i++) {
      cell *c = dcal[i];
      if(c->monst == moKrakenH && !c->stuntime && !isWateryOrBoat(c)) {
        vector<cell*> ctab;
        forCellEx(c2, c) if(isWatery(c2)) ctab.push_back(c2);
        hrandom_shuffle(ctab);
        for(auto& cc: ctab) trymove(cc);
        }
      }
    }

  // c is the tentacle which will be the head after the move
  EX void trymove(cell *c) {
    if(kraken_pseudohept(c)) return;
    cell *c2 = c->move(c->mondir);
    if(!isWatery(c)) return;
    if(againstCurrent(c, c2)) return;
    forCellIdEx(c3, i, c) {
      if(c3->monst && c3 != c2 && !(c3->mondir < c3->type &&
        c3->move(c3->mondir) == c2))
        return;
      if(isPlayerOn(c3)) return;
      if(sword::at(c3)) return;
      if(!passable(c3, c, P_FISH | P_MONSTER)) return;
      }
    if(c2->stuntime) return;
    int hpcount[10];
    forCellIdEx(c3, i, c2) {
      hpcount[i] = c3->hitpoints;
      c3->monst = moNone;
      }
    c->monst = moKrakenH;
    vector<pair<cell*, cell*> > acells;
    acells.push_back(make_pair(c2, c));
    forCellIdEx(c3, i, c) {
      c3->monst = moKrakenT;
      c3->mondir = c->c.spin(i);
      c3->monmirror = c->monmirror ^ c->c.mirror(i);
      int i0 = (i+c->c.spin(c->mondir)-c->mondir+96+c->type/2) % c2->type;
      c3->hitpoints = hpcount[i0];
      acells.push_back(make_pair(c2->move(i0), c3));
      if(c3->wall == waBoat) {
        addMessage(XLAT("%The1 destroys %the2!", moKrakenH, waBoat));
        c3->wall = waSea;
        }
      if(c3->wall == waStrandedBoat) {
        addMessage(XLAT("%The1 destroys %the2!", moKrakenH, waBoat));
        c3->wall = waNone;
        }
      }
    
    while(isize(acells)) {
      // bool found = false;
      for(int i=0; i<isize(acells); i++) {
        /* bool noconflict = true;
        for(int j=0; j<isize(acells); j++) 
          if(acells[i].second == acells[j].first)
            noconflict = false; */
        /* if(noconflict) */ {
          // found = true;
          indAnimateMovement(match(acells[i].first, acells[i].second), LAYER_BIG);
          acells[i] = acells[isize(acells)-1];
          acells.resize(isize(acells)-1);
          i--;
          }
        }
      }
    commitAnimations(LAYER_BIG);
    sleep(c);
    return;
    }
  
EX }

bool barrierhept(cell *c) {
  return c->bardir != NOBARRIERS && c->bardir != NODIR;
  }

#if CAP_FIELD
EX namespace prairie {

  using namespace fieldpattern;
  
  int getfval(cell *c3) {
    if(barrierhept(c3)) return btspin(fieldval(c3).first, c3->bardir)+1;
    return 0;
    }
  
  EX void spread(cell *c, cell *from) {
    int rd;
    
    c->LHU.fi.flowerdist = 8;
    c->LHU.fi.walldist = 8;
    c->LHU.fi.walldist2 = 8;

    if(ls::any_chaos()) {
      c->LHU.fi.rval = 0;
      }    
    else if(quotient && geometry != gFieldQuotient) { // fix cylinder
      c->LHU.fi.rval = 0;
      }
    else if(euclid) {
      auto co = euc2_coordinates(c);
      int y = co.second;
      c->LHU.fi.rval = (y&15);
      }
    else if(sphere) {
      c->LHU.fi.rval = celldistance(c, cwt.at) + 8 - (PURE ? 2 : 3);
      }
    else if(weirdhyperbolic) {
      c->LHU.fi.rval = max(celldist(c), 15);
      }
    else {
      if(geometry == gFieldQuotient || !from) {
        c->fval = currfp.distflower0;
        }
      else if(from && from->land == laPrairie && from->fval)
        c->fval = from->fval;
      else {
        forCellEx(c2, c) if(c2->land == laPrairie && c2->fval)
          c->fval = c2->fval;
        if(!c->fval) forCellEx(c2, c) if(!c->fval) c->fval = getfval(c2);
        if(!c->fval) forCellEx(c2, c) forCellEx(c3, c2) if(!c->fval) c->fval = getfval(c3);
        if(!c->fval) {
  
          int barclose = 0;
  
          forCellEx(c2, c) if(barrierhept(c2)) barclose++;
  
          forCellEx(c2, c) forCellEx(c3, c2)
          if(barrierhept(c3)) barclose++;
    
          printf("c = %p bc = %d\n", hr::voidp(c), barclose);
        
          raiseBuggyGeneration(c, "could not set river fval");
          }
        }
      pair<int,bool> fv = fieldpattern::fieldval(c);
      fv = currfp.gmul(fv, currfp.inverses[c->fval-1]);
      
      rd = currfp.getdist(fv, currfp.distriver);
      int rl = currfp.getdist(fv, currfp.distriverleft);
      int rr = currfp.getdist(fv, currfp.distriverright);

      c->LHU.fi.flowerdist = currfp.getdist(fv, currfp.distflower);
      c->LHU.fi.walldist   = currfp.getdist(fv, currfp.distwall);
      c->LHU.fi.walldist2  = currfp.getdist(fv, currfp.distwall2);

      c->LHU.fi.rval = 0;
      if(rd <= 7 && rl < rr) 
        c->LHU.fi.rval = 8 + rd;
      if(rd <= 7 && rl > rr) 
        c->LHU.fi.rval = 7 - rd;
      }
    
    if(c->LHU.fi.flowerdist == 0 && c->type != 6) c->item = itOrbSafety;
    
    if(c->LHU.fi.walldist == 0) c->wall = waBarrier;

    if(0) if(c->type == 7) for(int i=0; i<7; i++) {
      eItem m = currfp.markers[fieldpattern::btspin(c->master->fieldval,i)];
      if(m) {
        if(c->item) c->item = itBuggy2;
        else c->item = m, c->mondir = i;
        if(c->wall == waSea) c->wall = waBoat;
        }
      }
    }
  
  #define RLOW (sphere?(PURE?7:6):PURE?4:2)
  #define RHIGH (sphere?(PURE?8:9):PURE?11:13)
  
  EX bool no_worms(cell *c) {
    if(c->land != laPrairie) return false;
    int rv = c->LHU.fi.rval;
    return rv > RLOW+1 && rv < RHIGH-1;
    }

  EX bool isriver(cell *c) {
    return c->land == laPrairie && c->LHU.fi.rval <= RHIGH && c->LHU.fi.rval >= RLOW;
    }

  bool mainriver(cell *c) {
    return c->LHU.fi.rval <= 8 && c->LHU.fi.rval >= 7;
    }

  EX bool nearriver(cell *c) {
    return c->LHU.fi.rval == RHIGH+1 || c->LHU.fi.rval == RLOW-1;
    }

  cell *enter;
  
  bool opposite(cell *c) {
    return (c->LHU.fi.rval ^ enter->LHU.fi.rval) & 8;
    }

  bool isleft(cell *c) {
    return c->LHU.fi.rval & 8;
    }

  int towerleft(cell *c) { 
    return c->LHU.fi.rval;
    }
  
  int towerright(cell *c) { 
    return 15^c->LHU.fi.rval;
    }
  
  EX cell *next(cell *c, int pv IS(1)) {
    for(int i=0; i<c->type; i++) {
      cell *c1 = createMov(c, i);
      cell *c2 = createMov(c, (i+pv+c->type)%c->type);
      if(c1 && c1->LHU.fi.rval == c->LHU.fi.rval)
      if(c2 && c2->LHU.fi.rval == c->LHU.fi.rval+1)
      if(isNeighbor(c1,c2))
        return c1;
      }
    return NULL;
    }
  
  cell *prev(cell *c) { return next(c, -1); }
  
  int beastdist(cell *c, int dir) { 
    cell *cx = c; int n=0;
    while(true) { 
      if(cx->monst == moHerdBull) return n;
      cx = next(cx, dir); 
      n++; 
      if(!cx || n >= 1000) return 1000; 
      }
    }
  
  EX vector<cell*> beaststogen;
    
  EX void generateBeast(cell *c) {
    int beastdistance = min(beastdist(c, 1), beastdist(c, -1));
    if(hrand(1000) >= 15 * beastdistance + 2 * items[itGreenGrass]) return;
    c->monst = moHerdBull;
    cell *c2 = prev(c);
    if(c2) c->mondir = neighborId(c, c2);
    }   

  void moveAt(cell *c, manual_celllister& cl) {
    if(!cl.add(c)) return;
    vector<cell*> whirlline;
    whirlline.push_back(c);
    cell *c2 = prev(c);
    while(c2 && cl.add(c2)) {
      whirlline.push_back(c2);
      c2 = prev(c2);
      // in sphere/quotient geometries, never break before a bull
      if((sphere || quotient) && !c2->monst) break;
      }
    reverse(whirlline.begin(), whirlline.end());
    c2 = next(c); 
    while(c2 && cl.add(c2)) whirlline.push_back(c2), c2 = next(c2);
    int qty = isize(whirlline);
    if(shmup::on) {
      for(int i=0; i<qty; i++) if(whirlline[i]->cpdist <= gamerange()) {
        generateBeast(whirlline[i]);
        break;
        }
      }
    else {
      c2 = whirlline[qty-1];
      if(c2->monst == moHerdBull) c2->monst = moNone;
      if(!shmup::on) for(int q=qty-2; q>=0; q--) {
        cell *cp = whirlline[q];
        cell *cn = whirlline[q+1];
        /* just pretend the roadblocks disappear */
        if(cn->monst == moRagingBull && cn->cpdist == INFD && cn->stuntime) {
          cn->stuntime--;
          if(cn->stuntime == 0) cn->monst = moNone;
          }
        if(cp->monst == moHerdBull && !cp->stuntime) {
          cp->mondir = neighborId(cp, cn);
          beastAttack(cp, true, true);

          if(!cn->monst && !isPlayerOn(cn) && passable_for(cp->monst, cn, cp, P_DEADLY))
            moveMonster(movei(cp, cn, TELEPORT));
          else {
            playSound(NULL, "hit-axe"+pick123());
            beastcrash(cn, cp);
            cp->monst = moRagingBull;
            cp->stuntime = 3;
            cp->mondir = NODIR;
            }
          }
        }
      if(!sphere && !quotient) generateBeast(whirlline[0]); 
      }
    }
          
  EX void move() {
    if(ls::any_chaos()) return;
    manual_celllister cl;
    for(int i=0; i<isize(dcal); i++) {
      cell *c = dcal[i];
      if(isriver(c)) moveAt(c, cl);
      }
    for(int i=0; i<isize(beaststogen); i++)
      generateBeast(beaststogen[i]);
    beaststogen.clear();
    }
  
  vector<cell*> tchoices;
  
  cell *lasttreasure;
  
  // vector<cell*> orbs;
  
  bool thisriver(cell *c) {
    forCellEx(c2, c) if(c2->mpdist < c->mpdist && !isriver(c2)) return false;
    forCellEx(c2, c) if(c2->mpdist < c->mpdist) return thisriver(c2);
    return true;
    }
  
  EX void generateTreasure(cell *c) { 
//    if(nearriver(c) && op
    if(enter && nearriver(c) && opposite(c) && thisriver(c)) {
      int hr = hrand(100);
      if(hr == 0 && items[itGreenGrass] >= 10 && !inv::on) {
        c->item = itOrbBull;
        // orbs.push_back(c); 
        }
      else if(hr < 1+PRIZEMUL) {
        placePrizeOrb(c);
        // if(c->item) orbs.push_back(c);
        }
      else tchoices.push_back(c);
      } 
    }
  
  EX void treasures() {
    if(enter && !isriver(cwt.at)) enter = NULL;
    else if(!enter && isriver(cwt.at)) enter = cwt.at;
    if(isize(tchoices)) {
      if(lasttreasure && lasttreasure->item == itGreenGrass) {
        if(celldistance(lasttreasure, cwt.at) >= (PURE ? 7 : 10)) {
          lasttreasure->item = itNone;
          forCellEx(c2, lasttreasure) if(c2->item == itGreenGrass) c2->item = itNone;
          }
        else { tchoices.clear(); return; }
        }
      if(isize(tchoices) < 3) { tchoices.clear(); return; }
      lasttreasure = tchoices[hrand(isize(tchoices))];
      lasttreasure->item = itGreenGrass;
      for(int i=0; i<isize(tchoices); i++) if(isNeighbor(tchoices[i], lasttreasure))
        tchoices[i]->item = itGreenGrass;
      tchoices.clear();
      }
    }  
EX }
#else
namespace prairie { 
  bool no_worms(cell *c) { return false; } 
  void treasures() { }
  void move() { } 
  bool isriver(cell *c) { return false; }
  }
#endif

EX namespace ca {
  EX ld prob = .2;
  static const int MAX_NEIGHBOR = 60; /* may be larger than MAX_EDGE due to mineadj */
  string carule[MAX_NEIGHBOR][2];
  
  EX eWall wlive = waFloorA;
  
  EX set<cell*> changed;

  EX void list_adj(cell *c) {
    changed.insert(c);
    for(cell* c1: adj_minefield_cells(c))
      changed.insert(c1);
    }

  // you can also do -mineadj
  
  EX string fillup(string s) {
    while(isize(s) < MAX_NEIGHBOR) s += '0';
    return s;
    }
  
  EX void init() {
    // hexagonal variant of Game of Life, as suggested by Wikipedia
    for(int i=0; i<MAX_NEIGHBOR; i++) 
      carule[i][0] = fillup("00100"),
      carule[i][1] = fillup("00011");
    }

#if CAP_COMMANDLINE
  bool readArg() {
    using namespace arg;
    if(argis("-caprob")) {
      shift(); prob = argf();
      return 0;
      }
    if(argis("-calive")) {
      shift(); wlive = eWall(argi());
      return 0;
      }
    if(argis("-carun")) {
      shift(); int iter = argi();
      start_game();
      for(int i=0; i<iter; i++) simulate();
      return 0;
      }
    if(args()[0] != '-') return 1;
    if(args()[1] != 'c') return 1;
    int livedead = args()[2] - '0';
    if(livedead < 0 || livedead > 1) return 1;
    int nei = -1;
    if(args()[3]) {
      nei = atoi(argcs()+3);
      if(nei < 0 || nei >= MAX_NEIGHBOR) return 1;
      }
    shift();
    string s = args();
    if(nei == -1) for(int i=0; i<MAX_NEIGHBOR; i++)
      carule[i][livedead] = fillup(s);
    else 
      carule[nei][livedead] = fillup(s);
    return 0;
    }
  
  auto ah = addHook(hooks_args, 0, readArg);
#endif

  EX void simulate() {
    if(cwt.at->land != laCA) return;
    if(items[itOrbAether] < 2) items[itOrbAether] = 2;
    vector<cell*> allcells;
    for(cell *c: changed) allcells.push_back(c);
    changed.clear();
    int dcs = isize(allcells);
    std::vector<bool> willlive(dcs);
    int old = 0, xold = 0;
    for(int i=0; i<dcs; i++) {
      cell *c = allcells[i];
      if(c->land != laCA) continue;
      int nei = 0, live = 0;
      for(cell *c2: adj_minefield_cells(c)) {
        nei++; if(c2->wall == wlive) live++;
        }
      int welive = 0; if(c->wall == wlive) welive++;
      willlive[i] = carule[nei][welive][live] == '1';
      old += welive, xold += live;
      }
    for(int i=0; i<dcs; i++) {
      cell *c = allcells[i];
      auto last = c->wall;
      c->wall = willlive[i] ? wlive : waNone;
      if(c->wall != last) {
        dynamicval<ld> d(prob, 0);
        setdist(c, 7, nullptr);
        list_adj(c);
        }
      }
    println(hlog, make_tuple(dcs, old, xold, isize(changed)));
    }
EX }

auto ccm = addHook(hooks_clearmemory, 0, [] () {
  heat::offscreen_heat.clear();
  heat::offscreen_fire.clear();
  princess::clear();
  mirror::mirrors.clear();
  clearing::bpdata.clear();
  clearing::stats.clear();
  clearing::score.clear();
  tortoise::emap.clear();
  tortoise::babymap.clear();
  dragon::target = NULL;
  #if CAP_FIELD
  prairie::lasttreasure = NULL;
  prairie::enter = NULL;
  prairie::tchoices.clear();
  prairie::beaststogen.clear();
  #endif
  mirror::clearcache();
  }) +
  addHook(hooks_gamedata, 0, [] (gamedata* gd) {
    gd->store(heat::offscreen_heat);
    gd->store(heat::offscreen_fire);
    gd->store(princess::infos);
    gd->store(mirror::mirrors);
    gd->store(clearing::bpdata);
    gd->store(tortoise::emap);
    gd->store(tortoise::babymap);
    gd->store(prairie::lasttreasure);
    gd->store(prairie::enter);
    gd->store(prairie::tchoices);
    gd->store(prairie::beaststogen);
    gd->store(sword::dir);
    gd->store(elec::haveelec);
    gd->store(elec::havecharge);
    gd->store(elec::lightningfast);
    gd->store(elec::havethunder);
    gd->store(elec::afterOrb);
    }) +
  addHook(hooks_removecells, 0, [] () {
    for(cell *c: removed_cells) clearing::score.erase(c);
    for(auto& am: adj_memo) am.clear();
    eliminate_if(heat::offscreen_heat, is_cell_removed);
    eliminate_if(heat::offscreen_fire, is_cell_removed);
    eliminate_if(princess::infos, [] (princess::info*& i) { 
      if(is_cell_removed(i->princess) || is_cell_removed(i->prison)) { 
        DEBB(DF_MEMORY, ("removing a princess"))
        if(i->princess && !is_cell_removed(i->princess)) {
          DEBB(DF_MEMORY, ("faking a princess"))
          princess::newFakeInfo(i->princess);
          }
        delete i; 
        return true;
        }
      return false; 
      });
    set_if_removed(dragon::target, NULL);
    #if CAP_FIELD
    set_if_removed(prairie::lasttreasure, NULL);
    set_if_removed(prairie::enter, NULL);
    eliminate_if(prairie::tchoices, is_cell_removed);
    eliminate_if(prairie::beaststogen, is_cell_removed);
    #endif
    for(auto& bpd: clearing::bpdata) set_if_removed(bpd.second.root, NULL);
    });

#if CAP_FIELD
// windcode arrays precomputed for speed

int windcodes18920[] = {164,182,171,158,148,155,163,174,188,205,193,160,167,160,140,128,146,153,159,161,167,180,190,186,202,229,234,24,120,146,165,177,179,169,146,125,113,93,131,157,147,155,158,161,162,157,157,162,173,187,204,198,176,163,242,249,234,238,1,30,77,120,140,153,165,173,173,204,200,184,165,150,130,119,104,86,70,71,54,208,168,141,133,167,169,158,161,161,166,171,165,142,144,153,159,166,175,186,203,225,221,216,191,168,149,114,70,26,10,241,220,229,244,251,7,19,50,85,110,129,143,148,145,173,180,178,174,166,144,54,248,215,182,170,166,153,134,116,118,122,114,96,77,57,47,69,78,62,18,228,189,164,148,126,110,81,250,217,169,151,156,163,168,156,154,170,183,192,182,149,118,120,138,152,158,156,158,164,169,171,181,202,217,234,252,241,243,235,206,182,169,159,149,125,97,77,53,40,28,24,4,237,218,192,204,220,244,246,248,250,1,4,12,31,56,98,120,127,129,134,148,156,161,139,105,153,209,205,187,177,180,177,174,167,152,117,81,44,4,232,201,160,162,161,168,174,165,153,139,115,94,103,120,129,127,125,118,94,66,50,40,28,10,50,78,89,97,81,76,54,16,230,195,174,162,157,151,139,122,98,81,58,27,249,245,2,7,89,127,132,146,158,162,174,191,230,64,124,150,173,193,209,224,211,203,174,146,119,87,90,93,118,139,159,167,164,157,149,150,155,165,172,169,159,161,172,195,214,219,221,206,26,19,252,253,11,9,253,225,206,187,170,168,166,155,149,141,124,101,83,81,68,55,48,45,38,29,29,43,24,254,232,219,199,171,159,174,187,208,249,17,219,241,250,248,241,244,248,1,1,3,17,30,37,63,128,134,147,141,135,125,124,134,150,157,161,173,186,161,114,79,52,50,236,241,243,236,163,169,168,181,186,184,184,178,171,172,170,149,131,108,91,73,64,41,10,233,216,202,112,119,144,154,152,161,177,186,182,176,166,152,145,133,106,85,67,76,85,112,135,142,129,124,134,132,139,125,89,71,42,32,26,32,45,227,220,243,26,71,91,97,112,127,106,89,83,88,88,65,30,255,229,201,174,171,164,157,154,161,153,146,142,139,137,93,78,75,67,57,36,3,232,233,241,255,26,30,48,73,93,105,116,121,132,147,161,169,157,151,180,201,219,232,6,32,94,129,142,159,177,190,207,220,241,252,237,226,223,212,193,170,144,119,95,72,56,60,62,70,96,124,136,149,170,177,183,178,171,154,149,144,141,144,150,157,168,175,184,178,167,146,139,143,148,180,196,208,228,229,232,231,209,170,117,73,52,48,251,241,251,6,26,41,37,26,242,232,217,212,191,169,155,160,167,174,177,161,147,145,144,150,147,130,100,75,65,83,79,79,75,40,45,49,48,49,46,45,35,18,19,41,83,56,19,255,239,220,221,219,208,184,155,138,133,145,160,167,179,185,226,14,46,119,196,237,243,251,8,1,243,232,231,234,235,249,9,5,248,245,247,7,18,21,21,21,29,33,223,187,158,154,168,167,153,142,128,115,118,114,116,133,167,159,162,162,170,184,194,216,208,209,109,92,65,45,19,13,255,241,247,253,255,14,26,82,139,164,157,144,167,183,193,192,188,192,197,189,179,173,167,169,173,184,191,172,151,138,123,108,98,87,81,75,78,70,51,29,248,222,213,198,230,39,67,83,91,123,146,157,156,140,140,151,176,195,198,187,185,185,182,175,159,145,144,146,150,141,96,68,57,51,50,54,54,63,99,129,153,157,160,140,124,114,89,153,144,133,140,158,162,146,59,58,72,60,18,3,6,6,16,22,45,75,123,170,197,219,228,245,50,102,98,102,98,100,110,149,166,132,108,84,80,82,87,90,102,120,108,92,254,11,248,236,226,205,171,147,159,161,163,163,153,145,141,154,185,165,146,146,144,142,143,150,174,224,57,69,68,66,63,68,69,64,49,42,15,226,206,217,228,232,231,245,27,49,47,43,37,50,71,90,102,100,103,106,117,113,117,126,136,148,165,177,199,246,63,107,124,217,215,225,230,225,221,238,2,15,13,1,171,134,132,135,147,165,179,185,191,202,212,227,244,9,18,24,1,248,240,242,233,221,220,209,192,178,161,143,125,108,96,79,64,53,37,29,34,38,51,53,68,86,116,140,130,136,154,173,189,190,194,193,191,191,189,164,139,145,149,144,141,140,129,128,143,154,145,159,174,173,173,186,203,193,183,177,158,129,126,120,121,116,113,165,195,207,214,207,201,20,239,229,239,251,248,225,198,178,149,133,93,76,72,73,101,137,214,221,227,242,253,1,14,31,51,79,73,78,114,201,238,243,210,212,220,226,209,189,167,147,142,145,156,166,173,173,194,199,171,152,142,137,138,134,134,154,165,166,152,141,118,88,64,35,15,101,91,76,72,78,103,163,251,11,33,45,52,56,42,20,87,58,42,49,57,23,244,2,14,8,221,138,125,95,86,40,255,4,246,224,206,203,213,221,224,220,204,185,165,146,124,108,115,117,127,127,138,148,161,165,163,152,165,201,243,35,54,79,118,160,201,238,20,248,228,253,10,24,25,255,234,231,221,224,221,223,224,223,233,254,16,28,21,7,236,227,232,236,241,254,15,26,22,18,13,10,9,19,19,12,254,254,228,205,183,163,150,161,188,194,194,173,160,147,131,118,105,99,114,119,108,97,93,54,215,185,161,156,163,167,162,158,164,176,197,196,209,229,246,238,238,0,23,54,135,85,38,49,35,13,249,251,243,244,233,224,253,0,254,1,2,10,25,34,59,92,114,146,167,181,171,126,109,136,177,193,199,207,201,196,188,183,193,206,210,204,194,183,165,172,169,158,161,169,159,171,190,209,214,199,174,153,140,143,143,120,114,115,102,83,84,83,75,72,79,90,87,90,81,73,49,254,228,215,209,208,202,180,148,66,58,49,52,66,60,63,66,94,122,144,155,168,166,169,208,49,109,129,126,172,199,215,215,212,193,183,183,184,187,188,195,196,181,166,152,141,131,135,142,143,152,185,246,69,117,77,26,31,37,51,45,43,40,38,28,31,35,73,105,127,150,169,177,177,179,172,153,132,114,112,106,89,47,244,180,142,120,125,138,155,172,179,191,196,247,30,39,34,56,97,88,29,250,235,220,235,245,252,2,7,5,15,37,61,78,99,125,135,165,198,212,216,219,220,221,217,221,150,130,111,112,113,102,97,101,104,90,114,213,202,216,168,126,122,119,84,52,69,73,78,88,91,85,80,100,125,149,147,135,142,165,198,4,41,13,235,241,239,232,217,189,156,128,101,150,154,154,155,160,174,164,156,147,139,126,129,137,146,230,228,212,133,137,141,144,145,143,141,140,142,149,157,171,208,242,16,58,67,64,63,68,62,51,52,77,80,78,79,65,51,46,58,50,245,184,186,177,197,204,220,233,234,237,219,195,201,47,50,54,71,69,64,54,24,29,39,55,72,87,99,106,119,107,97,96,95,93,114,132,146,66,94,114,126,131,135,139,151,175,180,166,197,248,15,25,35,63,86,80,44,3,222,224,238,247,238,227,221,211,202,216,239,9,18,22,24,6,247,217,178,148,131,125,131,130,131,139,156,174,187,185,184,193,201,205,205,212,221,233,249,8,18,37,58,86,4,7,6,1,236,248,9,0,239,217,216,226,233,221,217,208,193,180,170,159,153,135,125,110,103,96,82,74,69,57,52,50,42,25,15,255,5,11,33,40,44,45,45,50,56,71,108,144,168,161,90,117,125,139,157,178,193,205,205,206,209,201,196,191,197,203,212,213,182,151,119,110,130,150,159,146,137,134,142,146,142,126,100,113,146,166,170,149,128,121,187,178,176,177,169,169,188,210,232,211,197,195,189,185,178,152,117,95,108,115,110,109,102,89,96,83,71,243,207,203,210,226,231,233,216,173,116,69,51,2,213,220,234,0,25,14,7,245,232,213,195,190,164,113,136,133,49,67,75,77,78,75,89,124,135,149,174,197,202,212,216,230,242,252,252,245,4,18,24,31,50,81,119,108,97,97,103,124,154,174,204,2,25,75,167,189,210,223,229,240,242,227,209,188,169,135,115,145,134,129,130,143,157,161,180,187,166,162,242,230,228,195,166,150,143,139,131,130,135,131,124,120,110,139,168,180,191,176,164,161,155,138,132,114,77,55,28,1,250,221,188,151,115,93,77,65,64,57,63,92,131,165,199,214,238,254,16,42,56,39,52,72,72,56,22,244,187,130,79,44,20,31,52,64,82,99,196,220,237,249,0,34,43,219,213,195,160,144,134,118,100,101,124,212,227,3,24,10,250,230,205,194,192,185,199,215,226,228,228,230,237,221,205,192,159,161,163,142,115,85,92,96,99,106,118,121,120,114,121,128,132,153,166,168,163,155,143,129,128,141,181,208,233,17,62,74,69,60,80,123,146,179,204,199,233,30,57,78,169,196,237,9,14,15,18,35,45,57,42,216,227,235,223,210,207,218,223,213,211,216,217,219,215,213,217,242,6,22,35,43,43,34,22,248,218,210,203,214,224,238,237,230,241,4,21,31,44,27,22,20,14,14,3,251,253,7,14,21,27,4,246,237,6,39,9,243,222,210,200,193,177,156,122,144,165,187,219,223,224,216,191,175,171,159,149,141,127,117,111,108,80,71,111,124,133,124,106,89,78,84,74,52,14,228,201,169,155,142,147,163,169,170,175,168,154,148,158,157,141,216,214,197,189,200,216,234,251,19,13,251,248,254,18,23,17,4,218,181,177,1,7,14,46,65,36,255,241,234,234,236,232,228,232,229,219,219,213,185,83,21,254,246,255,9,0,4,6,18,36,36,37,43,80,107,110,115,126,177,182,193,199,195,202,27,71,79,100,111,145,211,208,210,206,213,222,214,207,203,196,188,181,171,180,209,222,219,220,217,217,207,201,194,166,122,170,179,172,163,145,144,150,171,184,112,145,170,191,213,238,234,228,225,222,197,173,150,131,126,130,160,167,166,73,100,117,135,133,107,73,64,76,85,88,86,77,64,63,75,86,100,106,97,96,102,103,100,106,93,100,2,230,222,208,209,212,194,202,202,206,206,179,147,121,97,79,71,46,33,31,53,66,66,46,37,39,42,60,73,89,123,160,159,148,163,191,179,170,166,179,209,244,26,79,119,134,123,102,82,252,216,219,229,234,233,245,206,192,184,175,177,179,175,185,191,188,179,192,224,217,197,183,170,163,159,145,141,132,120,116,135,147,145,139,134,131,178,244,28,43,30,202,177,168,244,252,251,0,12,57,63,50,34,36,49,38,26,29,24,18,1,2,2,21,51,84,116,132,144,158,167,180,184,201,198,191,186,187,185,166,148,131,113,98,105,107,114,116,104,73,37,5,223,192,163,133,111,87,101,118,134,155,164,169,189,190,201,210,196,231,21,23,32,36,30,26,15,7,228,152,153,187,3,11,23,201,209,199,186,204,219,235,249,251,248,253,1,252,249,254,2,23,56,66,69,77,87,95,108,115,109,127,162,199,230,223,217,217,215,218,226,219,211,205,204,209,198,187,165,138,112,107,119,128,126,108,98,91,90,98,117,147,68,59,72,47,15,239,227,233,247,10,101,112,120,125,132,139,120,20,4,52,71,63,65,75,84,95,100,98,85,65,56,85,116,138,150,174,177,178,162,159,148,150,154,141,154,2,30,80,111,199,218,236,251,249,248,243,238,230,230,208,179,151,118,77,33,253,176,162,153,147,149,152,148,127,195,195,175,161,155,153,144,136,125,100,103,120,145,142,104,63,5,242,245,1,47,96,125,140,137,136,142,149,150,143,145,145,135,137,137,138,148,154,153,145,151,189,220,235,1,255,238,64,78,71,62,58,56,54,70,79,70,35,28,16,60,106,95,89,86,91,97,78,70,49,34,33,57,76,79,86,79,145,146,158,164,145,151,188,187,191,210,227,241,243,236,236,246,252,164,142,161,165,139,105,71,54,44,56,100,97,84,73,79,105,10,243,5,21,34,44,57,69,77,88,101,102,108,120,135,139,122,104,86,89,95,91,84,75,84,123,143,162,182,223,17,62,94,116,123,130,133,132,127,128,134,138,141,143,235,209,185,146,111,23,15,17,28,35,29,11,11,35,48,65,76,85,64,32,24,36,214,205,211,254,5,21,6,236,226,225,225,218,207,195,183,189,209,232,2,26,26,26,27,38,44,24,255,244,236,228,204,175,152,137,131,117,111,126,136,136,124,116,120,134,146,165,179,188,202,195,186,181,179,184,198,207,211,209,201,200,205,218,222,222,226,254,16,18,12,13,30,51,73,108,142,179,236,5,5,10,26,48,213,208,239,8,33,36,6,239,9,198,196,209,226,239,250,244,228,217,222,227,210,195,185,178,175,172,162,162,169,148,119,123,114,99,97,105,105,97,81,69,70,75,67,55,48,47,43,54,55,33,20,20,10,249,233,233,235,234,13,51,47,34,35,40,42,47,40,38,41,33,44,55,77,104,155,179,191,196,243,41,87,107,116,117,125,139,151,168,186,192,202,209,224,211,214,219,221,218,211,202,198,198,188,182,204,211,217,233,238,245,213,185,160,141,92,80,83,107,130,164,174,178,159,143,134,126,125,133,145,156,153,150,149,161,73,57,75,87,184,184,199,226,146,153,146,122,85,19,214,187,171,170,179,182,186,178,165,145,155,198,209,245,14,254,230,214,179,191,220,186,179,192,197,200,178,153,102,72,60,83,102,114,123,93,101,109,101,77,61,82,92,90,68,53,23,251,228,207,199,197,206,222,239,249,1,5,246,6,177,157,135,98,70,61,70,86,170,180,194,225,224,224,243,34,68,40,23,16,13,2,243,238,227,203,190,194,202,209,246,66,63,98,153,173,213,7,39,70,89,75,73,88,85,73,67,49,131,146,143,146,144,142,160,177,182,187,195,199,205,205,211,221,229,239,1,10,2,235,220,243,4,13,19,21,22,22,39,65,81,124,174,146,130,114,110,99,97,110,114,125,146,163,161,163,191,34,47,45,62,88,123,159,172,191,218,227,233,228,236,252,255,1,249,230,216,202,187,175,188,141,72,14,179,155,138,116,113,111,122,131,142,175,157,131,175,195,201,215,186,152,127,73,20,5,1,2,239,217,189,155,136,134,140,145,145,130,119,120,120,134,139,133,124,118,114,104,82,77,114,152,184,196,205,225,223,150,160,163,169,182,174,134,128,137,141,160,79,67,61,47,23,247,233,236,227,209,200,199,187,170,144,111,98,86,75,60,53,61,53,30,38,49,65,84,118,146,131,218,224,211,219,226,236,250,7,10,11,89,92,31,1,218,137,104,97,72,78,67,242,252,250,221,190,163,152,110,76,51,13,242,1,23,44,80,70,77,91,103,133,189,209,210,221,230,235,236,237,252,55,95,143,186,205,216,220,219,191,164,143,138,142,136,116,94,88,90,115,140,171,197,194,246,28,46,44,34,14,248,241,212,182,188,190,184,191,178,160,178,194,208,221,232,238,233,227,223,230,242,5,248,212,203,215,235,151,127,131,203,182,147,138,142,53,30,70,102,94,83,79,84,104,116,126,124,122,118,110,101,107,114,119,108,111,147,161,170,175,176,174,162,153,143,135,126,116,107,96,106,105,158,200,192,197,217,1,37,66,97,106,116,68,41,37,24,119,137,140,147,157,205,230,212,190,156,64,56,57,73,86,105,119,144,174,222,255,19,27,26,26,12,7,15,26,56,62,66,78,87,126,169,205,229,245,251,231,213,203,194,191,207,226,237,222,202,198,197,207,216,215,215,219,221,210,200,190,197,229,254,9,13,24,38,48,47,49,53,61,58,55,37,4,233,212,193,192,185,185,196,205,213,231,251,249,240,226,213,224,232,247,17,34,27,54,85,56,32,19,21,26,16,7,12,20,25,245,230,233,250,2,9,13,16,19,35,61,21,236,235,222,207,233,41,80,89,48,12,247,230,219,215,207,203,209,213,179,189,194,95,71,116,144,151,162,178,212,247,2,0,255,232,238,217,188,175,177,184,171,154,138,153,152,140,124,112,103,106,123,130,100,44,31,2,105,133,132,136,147,148,133,116,93,80,65,54,80,85,78,72,63,35,19,4,235,215,176,151,144,132,117,126,136,158,173,174,174,165,170,186,186,165,140,135,134,150,163,168,133,105,58,251,221,214,204,182,171,186,203,212,222,236,247,252,15,49,52,46,7,248,248,247,3,25,40,40,40,16,251,228,213,201,183,173,184,227,238,0,7,7,234,129,99,59,35,23,194,217,221,227,233,228,226,225,220,218,215,226,230,227,218,206,208,216,227,226,193,143,106,44,3,242,245,237,233,243,31,24,253,246,253,7,250,2,27,47,53,45,39,49,21,26,116,116,126,118,117,109,87,30,203,202,205,204,211,208,209,210,200,202,234,10,30,53,63,78,82,82,91,112,122,50,22,239,217,223,214,209,219,231,241,226,219,216,212,206,201,199,190,180,185,173,153,154,151,201,250,225,225,230,225,218,221,229,230,215,209,212,208,205,204,231,65,81,87,3,206,183,170,171,165,138,123,127,124,131,171,191,218,13,66,108,136,159,173,183,197,215,236,7,17,241,214,228,239,248,244,219,195,180,163,150,132,110,106,104,109,140,188,195,198,220,243,3,62,102,110,125,152,166,170,207,17,44,39,46,64,74,82,95,95,93,90,86,67,44,49,49,67,79,82,91,110,120,121,107,97,96,101,112,113,118,116,121,129,122,110,114,149,180,221,214,209,202,188,191,202,226,253,179,170,194,204,202,194,199,235,236,14,91,126,130,119,84,84,88,88,91,64,30,16,3,9,34,60,73,84,84,56,30,25,11,13,12,26,49,62,68,61,63,87,97,186,200,184,162,111,136,183,223,226,198,181,172,159,156,160,185,214,226,231,246,6,47,88,117,145,154,134,113,107,98,66,33,8,236,221,221,229,232,243,1,243,251,18,36,122,170,186,181,176,164,165,175,179,177,163,164,191,202,200,191,180,166,157,117,28,3,220,209,196,189,179,170,155,153,176,153,132,137,151,138,129,123,118,103,76,114,155,155,150,151,160,123,117,121,115,23,57,70,84,33,230,209,194,191,190,205,225,245,247,241,220,222,212,176,100,81,75,67,58,46,23,6,38,75,67,45,23,5,19,25,16,13,19,21,197,223,234,237,248,15,29,61,83,101,134,140,148,146,151,165,169,176,192,191,182,194,223,229,213,197,186,183,188,197,199,203,182,163,156,148,133,110,93,80,82,96,100,97,112,128,143,131,119,99,80,53,32,15,251,245,217,195,177,151,133,125,106,75,52,64,93,113,121,127,145,172,180,152,151,185,213,205,194,214,226,236,209,181,154,58,44,34,28,23,38,44,39,23,16,19,18,6,249,229,213,197,178,181,186,208,226,245,0,14,55,119,165,181,185,180,178,141,175,199,193,204,245,250,2,4,249,239,237,0,6,254,242,239,240,245,246,249,246,251,243,71,80,67,62,68,73,80,86,90,89,97,118,111,98,87,94,97,126,159,201,246,9,252,230,211,213,227,216,208,209,221,243,238,218,203,198,192,194,197,204,208,210,210,206,195,187,218,61,95,89,91,120,138,145,140,146,122,98,95,98,91,80,76,76,89,112,139,167,197,239,25,37,59,63,61,48,38,15,253,237,233,236,236,248,24,70,107,113,103,109,117,126,132,135,142,156,165,191,219,237,221,211,125,90,79,45,42,57,72,83,83,83,101,113,113,113,98,91,72,46,32,20,55,81,107,138,150,149,167,180,187,205,208,195,163,130,143,156,145,123,105,109,127,89,50,33,255,167,134,146,166,198,213,227,246,1,10,4,0,252,252,251,248,236,228,235,7,230,203,183,164,151,140,113,60,25,4,251,236,212,181,165,156,152,147,134,139,147,157,153,139,127,90,30,228,208,187,169,156,154,155,152,164,155,139,135,131,129,118,66,62,83,96,124,158,184,225,55,79,80,56,221,241,247,242,245,0,13,31,64,96,111,133,153,152,140,131,126,138,147,156,158,161,144,135,142,155,156,139,115,132,138,135,130,130,144,153,154,165,187,112,124,127,134,170,213,234,239,245,3,27,23,243,193,146,106,99,82,70,68,60,48,56,56,49,22,66,89,95,98,120,24,241,223,142,131,124,114,104,88,84,93,100,106,131,95,65,76,86,66,39,21,5,0,55,77,80,82,101,106,108,96,106,121,132,130,128,135,163,153,116,107,67,223,190,169,171,177,197,211,222,234,252,3,251,236,233,232,236,246,12,43,74,102,109,122,139,161,186,169,147,128,99,69,49,43,37,20,224,136,122,116,125,111,88,52,69,111,132,161,188,212,210,229,252,13,32,33,35,40,45,59,77,75,75,85,106,116,106,99,99,110,123,131,145,158,158,141,125,107,86,68,71,81,96,101,94,80,70,62,66,66,34,203,160,166,184,182,184,194,210,234,255,31,57,78,102,136,127,127,133,133,137,144,137,126,130,143,142,117,121,113,72,44,0,221,202,169,133,110,82,54,23,13,20,30,27,19,54,56,63,237,240,240,7,22,32,39,52,62,67,88,107,93,72,41,15,15,41,64,123,185,202,180,114,34,26,19,21,56,49,14,234,209,214,216,225,233,228,223,214,208,199,185,176,168,171,165,180,200,229,0,19,36,46,41,35,24,25,36,53,61,69,50,35,9,243,237,233,239,242,243,225,201,187,159,144,130,126,132,123,104,94,89,114,125,140,152,147,126,136,97,92,105,126,140,134,136,158,178,185,187,199,212,221,213,199,182,176,176,181,175,171,180,204,220,222,220,215,217,207,192,189,190,199,211,225,235,225,213,211,210,228,21,8,6,0,2,23,36,51,76,107,140,151,164,164,169,197,251,10,20,9,252,251,30,60,80,91,119,152,181,227,251,11,34,62,48,62,95,196,219,251,42,108,163,178,193,203,218,230,241,1,9,6,1,233,217,206,207,213,251,251,229,208,189,184,182,182,173,171,177,190,159,151,162,178,192,178,159,120,77,109,136,122,106,86,81,85,107,121,117,115,104,81,62,53,62,60,80,85,78,66,49,45,44,43,37,28,35,75,69,71,65,247,18,32,23,10,254,237,226,218,219,215,214,203,193,236,27,69,92,73,51,18,12,42,39,32,39,52,56,49,37,27,28,37,31,13,16,30,38,56,66,59,21,204,203,199,198,215,246,27,66,103,116,117,114,112,110,113,115,126,139,144,157,179,196,197,194,202,212,207,225,245,253,222,198,218,233,234,236,226,222,219,220,210,197,191,202,210,191,167,143,187,227,217,219,218,225,246,2,10,20,9,241,210,188,159,139,154,170,253,47,52,59,56,84,102,111,119,210,200,195,196,190,176,154,134,131,124,115,115,116,128,128,132,145,161,175,168,158,147,153,164,184,200,234,17,34,43,50,36,7,227,195,192,209,242,24,65,120,145,160,164,148,128,98,72,35,240,203,190,181,174,156,152,162,196,186,186,197,195,184,184,204,111,100,55,231,213,193,173,3,35,51,37,15,230,241,10,174,149,151,100,151,174,193,206,209,218,225,204,182,182,160,57,56,38,37,24,50,76,92,100,104,138,168,11,57,83,104,122,117,95,56,35,24,57,81,97,106,106,82,58,41,29,17,6,250,242,233,211,191,200,197,187,193,211,224,238,251,251,9,21,38,31,248,1,18,48,105,150,160,147,122,98,79,62,52,47,66,76,82,98,126,158,173,172,153,56,248,224,217,209,214,240,84,88,117,101,18,29,29,13,11,24,19,254,236,237,246,246,245,214,180,178,191,196,199,207,217,237,4,26,39,35,34,55,65,239,204,196,194,226,9,17,9,51,100,113,99,71,44,80,122,104,96,71,67,67,47,5,209,166,152,153,153,154,151,149,141,128,127,143,176,191,178,176,172,182,210,187,192,198,198,197,202,208,216,221,221,231,234,226,209,192,195,221,249,14,15,16,21,22,20,18,19,20,9,11,25,67,74,77,63,227,215,201,183,159,132,121,118,125,123,85,81,86,105,118,103,105,118,146,165,175,166,155,146,131,131,97,67,59,72,71,64,65,73,87,107,124,141,160,167,167,187,235,240,233,245,251,223,221,230,249,10,9,10,4,8,13,16,249,231,216,210,203,197,180,164,155,247,240,45,34,23,249,214,182,171,161,144,117,99,96,90,83,107,125,128,130,125,124,225,243,76,84,84,179,201,206,206,223,247,243,216,181,138,114,97,80,41,2,11,22,24,28,26,3,241,232,218,203,172,143,125,105,115,122,141,152,153,155,155,136,111,105,114,104,96,118,135,145,148,138,129,118,113,118,117,108,104,94,63,41,38,79,125,155,182,200,204,216,214,221,240,13,70,124,143,152,156,159,169,173,217,222,226,125,104,114,115,130,145,143,157,210,2,53,73,65,63,60,53,29,246,213,215,215,226,230,228,221,200,188,188,198,203,208,189,170,181,158,120,95,100,101,81,68,65,47,39,37,54,70,67,41,244,3,28,47,53,51,50,70,110,146,173,156,56,44,9,240,245,232,202,185,240,211,215,233,246,3,5,18,132,129,193,220,213,210,183,139,128,137,190,33,73,81,91,107,155,205,242,3,14,8,241,217,193,158,161,174,189,138,84,74,61,32,246,227,202,220,235,3,6,224,134,100,77,71,74,86,97,101,108,110,102,15,224,212,204,196,206,223,234,231,226,222,226,224,230,221,216,137,109,136,147,168,186,194,205,216,223,228,246,245,216,189,171,149,129,132,133,141,152,152,153,135,110,81,70,70,69,81,104,130,141,157,196,194,183,165,148,57,41,64,69,49,55,59,51,33,242,237,249,11,230,183,160,144,214,204,175,171,164,242,186,153,132,136,155,174,199,206,211,222,231,235,240,254,251,229,233,217,214,224,234,244,4,31,37,49,209,186,183,218,234,244,24,75,118,110,92,45,4,224,198,160,127,119,125,154,178,215,245,238,3,107,129,121,103,89,85,58,54,57,90,117,111,116,128,141,131,127,125,122,120,113,97,87,95,98,94,110,134,111,76,78,53,147,164,162,168,174,174,175,186,189,188,173,154,155,150,140,133,148,116,114,128,116,94,76,76,81,87,85,81,74,247,229,190,175,170,186,202,226,12,31,45,63,90,122,135,151,174,171,246,194,155,145,138,140,148,149,143,125,35,251,4,249,211,201,200,170,135,99,69,59,58,68,74,77,91,111,124,111,110,108,133,153,198,235,0,21,26,30,39,37,40,45,27,255,253,250,255,2,14,34,85,92,76,67,75,88,93,100,110,131,153,182,209,226,237,254,19,20,246,219,203,205,206,192,178,180,173,185,199,222,248,2,242,214,198,188,181,181,189,195,209,225,221,210,209,211,216,226,237,221,209,195,180,159,164,174,206,233,254,15,16,8,8,12,32,44,55,58,56,51,46,49,55,64,73,75,78,85,68,51,20,253,239,222,212,195,172,171,174,165,167,173,188,190,190,194,203,220,239,3,18,13,4,242,229,209,194,200,211,220,228,232,246,33,45,53,42,20,245,149,120,85,56,34,15,3,14,24,23,9,10,8,11,24,45,81,170,194,200,206,239,6,2,3,3,12,17,14,14,13,4,26,55,82,100,108,200,219,224,221,208,197,179,175,133,64,80,111,122,129,117,21,13,15,240,233,224,221,219,220,214,206,197,198,210,244,56,140,150,194,237,239,249,36,39,32,59,155,155,146,146,149,160,161,188,219,243,17,22,38,31,35,49,225,237,255,3,248,221,203,188,178,163,142,119,113,192,172,157,146,132,119,114,106,94,76,84,138,148,161,148,122,117,201,248,250,224,189,167,146,155,150,141,133,140,158,164,173,160,144,119,103,84,70,59,50,46,22,31,120,109,88,72,80,87,99,56,37,23,14,26,12,248,228,229,222,136,140,138,141,133,126,111,92,100,111,117,135,155,175,185,182,183,185,171,152,153,162,182,201,204,212,195,146,122,119,116,113,131,143,153,164,99,110,87,74,45,238,209,201,225,219,196,163,153,147,167,179,195,206,209,215,221,225,248,254,252,238,254,30,57,82,85,97,101,4,248,236,242,0,247,235,241,1,35,50,53,56,62,82,39,0,7,249,214,210,210,210,200,185,176,165,149,110,13,235,212,217,34,8,1,10,28,2,228,186,143,118,79,38,17,26,44,88,140,185,205,205,216,224,233,242,233,220,221,218,218,218,217,209,203,199,211,221,231,235,228,222,216,205,189,197,196,206,221,238,246,251,224,169,132,133,128,104,255,250,232,222,239,249,244,222,218,198,148,79,85,124,199,234,238,234,240,255,21,25,206,227,239,14,42,58,66,67,56,48,27,31,64,134,136,144,135,126,120,122,122,103,77,46,2,230,201,196,208,221,231,219,215,221,223,215,204,207,221,238,171,184,194,234,2,12,11,9,32,42,56,72,87,84,71,53,67,79,92,104,117,96,49,55,59,31,234,194,209,255,236,216,199,201,220,239,251,6,1,238,221,218,222,221,220,212,203,202,200,201,206,200,191,177,159,227,189,156,141,136,138,133,122,119,42,38,45,145,206,232,242,250,220,214,213,216,227,245,247,247,223,211,209,211,222,222,214,206,202,207,220,220,1,48,44,115,88,57,29,239,205,183,169,160,166,178,186,189,100,106,110,112,115,106,98,76,170,190,194,207,217,241,28,53,64,83,108,134,161,171,170,175,184,182,187,225,230,239,1,28,34,56,74,169,200,220,227,242,5,19,11,8,243,185,170,157,156,154,137,117,94,79,83,83,91,91,100,98,194,222,217,220,214,227,245,241,242,235,228,219,129,108,105,109,115,136,168,184,195,193,211,233,246,1,11,16,8,22,33,54,66,68,70,76,92,108,105,101,98,91,88,94,100,89,39,14,32,39,36,36,55,82,86,85,78,80,100,118,129,141,136,136,123,109,93,91,94,96,104,113,126,123,121,145,144,147,128,100,32,192,154,152,200,242,223,200,199,200,198,186,171,169,171,153,233,230,250,40,99,144,141,155,186,205,213,209,199,184,167,117,20,9,1,13,23,51,79,96,116,133,148,193,37,58,73,86,93,101,97,99,118,102,45,11,253,240,226,237,247,16,27,41,71,91,70,90,124,104,68,42,17,9,17,245,246,237,250,7,50,51,53,66,79,88,54,20,49,71,82,87,81,36,244,218,67,65,89,99,161,223,251,10,2,221,190,177,178,178,165,145,137,135,145,162,186,216,245,233,225,221,226,233,235,252,24,61,98,125,147,170,174,182,158,142,117,128,139,48,35,21,22,21,250,228,220,215,215,228,244,241,205,233,16,56,237,245,249,12,35,40,53,77,106,134,162,186,202,172,164,186,238,78,133,163,183,184,181,183,176,157,130,106,200,190,205,225,219,207,187,181,175,165,152,151,136,105,82,60,69,226,202,208,217,205,195,194,185,176,171,159,144,127,72,228,206,171,137,108,110,146,179,168,147,131,127,122,120,127,126,109,79,36,11,190,177,164,160,153,148,151,162,180,205,37,83,104,119,128,40,62,80,93,115,122,124,202,210,214,213,199,188,192,200,196,195,198,204,210,244,255,248,242,245,228,181,194,198,187,177,153,143,128,102,87,80,73,70,65,58,57,42,15,254,222,205,115,93,90,85,58,34,16,255,233,242,25,36,28,8,0,2,6,25,48,97,146,183,207,222,231,224,226,222,245,31,26,13,19,97,82,72,117,159,153,152,157,161,153,146,143,157,178,178,172,169,182,206,215,209,168,173,184,213,245,254,14,245,218,195,180,178,181,180,185,191,206,211,211,218,224,202,172,149,151,157,160,149,135,122,95,82,72,72,61,69,90,99,103,94,78,94,113,131,154,170,161,144,128,115,113,105,92,60,34,22,31,34,8,255,211,195,197,191,181,164,136,117,138,131,108,81,51,35,15,38,113,114,117,116,124,135,146,193,210,226,63,108,117,175,192,204,222,190,164,251,245,250,10,7,229,199,177,151,104,61,45,48,59,34,29,10,55,62,55,50,33,14,3,253,13,25,25,19,4,243,228,211,209,207,210,204,193,189,190,187,198,212,221,224,232,250,246,247,250,21,55,97,137,154,166,168,169,172,166,167,183,182,100,90,79,240,255,248,253,9,18,12,2,253,233,217,208,236,16,22,20,7,250,233,224,227,232,246,243,235,239,251,239,229,248,246,232,204,163,115,103,82,51,59,68,71,75,84,88,93,94,88,89,76,57,137,141,109,87,81,76,74,76,93,120,140,166,173,7,30,25,42,30,21,251,226,195,186,222,2,243,222,197,197,214,240,9,10,9,229,221,200,184,188,188,183,181,184,190,201,203,203,207,219,216,210,208,201,204,215,245,93,87,81,68,70,66,97,132,154,165,155,145,152,167,170,145,108,75,78,89,104,112,96,84,78,59,57,54,71,81,93,113,134,191,209,230,255,12,18,20,41,67,72,65,75,69,55,49,47,35,21,254,238,230,232,235,243,237,220,229,235,2,34,68,108,124,124,129,89,75,94,107,114,125,135,138,134,128,140,147,144,166,179,187,182,187,226,220,203,190,161,132,106,104,116,63,5,16,14,33,54,78,84,91,91,81,63,75,116,126,132,126,129,135,108,96,100,104,84,59,37,11,250,235,254,29,59,85,96,126,162,166,164,145,133,158,188,193,170,150,241,240,240,218,131,108,102,115,159,178,158,130,99,87,86,83,69,83,173,153,67,70,77,57,244,208,178,162,151,148,156,162,182,201,208,215,212,216,250,255,250,16,27,18,10,6,1,253,249,1,6,7,0,245,206,163,69,45,3,232,215,205,180,168,150,147,156,152,155,194,14,14,250,249,245,245,237,236,233,229,214,195,177,162,155,149,146,154,157,139,114,120,134,142,155,167,168,164,145,134,137,134,81,43,6,231,217,221,202,183,174,160,149,160,165,148,135,210,177,150,131,127,129,128,129,131,131,136,142,222,11,19,50,68,86,93,92,183,192,209,219,236,0,24,69,104,86,89,133,173,202,225,251,6,242,234,237,245,254,24,37,54,87,115,105,100,120,146,178,170,161,145,135,126,119,108,127,165,165,170,181,175,148,129,128,123,149,179,168,164,162,170,88,140,143,146,139,120,119,122,119,141,156,166,152,152,165,192,230,16,62,97,120,114,119,211,231,236,249,248,249,241,241,254,19,46,72,53,222,208,170,154,136,130,120,113,98,78,61,61,71,75,76,31,19,90,67,53,57,68,29,226,222,146,115,110,94,100,121,149,174,201,218,193,177,154,135,126,132,133,124,117,101,83,70,69,92,107,106,99,120,149,170,181,19,51,72,88,102,108,89,64,31,6,250,234,231,150,102,95,88,61,45,132,130,126,115,132,111,89,90,103,117,123,126,134,138,104,182,186,195,104,91,95,89,86,72,20,228,202,171,149,146,153,159,175,191,221,227,228,240,6,18,27,12,3,239,221,226,231,228,226,231,234,241,8,44,68,83,99,105,95,102,121,159,196,175,163,148,142,125,110,82,57,35,33,39,43,44,26,245,203,170,142,128,113,118,132,150,142,128,131,40,19,26,247,153,155,196,187,194,191,191,184,201,209,229,250,255,38,55,49,5,22,39,44,36,37,50,78,100,83,72,70,68,74,71,105,139,105,100,91,88,86,113,131,138,134,124,157,167,182,179,176,157,145,141,136,74,62,48,51,53,75,100,110,116,108,100,86,71,64,55,49,59,63,73,66,232,185,144,154,195,205,194,188,185,175,179,188,206,218,227,238,3,14,36,64,73,78,88,125,174,171,98,119,132,139,139,127,134,150,164,153,122,117,124,143,169,222,63,96,111,124,144,73,74,79,84,48,21,245,229,218,189,158,133,112,100,87,74,53,33,15,9,4,9,19,35,50,65,225,185,108,75,62,73,95,144,189,213,213,213,243,26,24,30,30,39,52,59,53,41,81,109,130,124,119,111,76,44,17,245,31,60,55,52,78,118,166,201,218,222,127,101,82,66,44,33,31,31,22,248,156,100,87,56,17,218,195,181,194,203,204,213,225,240,248,239,233,209,204,215,207,192,180,167,159,156,167,166,156,145,149,166,179,207,230,250,15,20,30,41,54,62,59,53,47,28,12,17,19,29,46,62,78,86,92,81,69,60,48,18,250,237,231,227,236,244,250,254,254,2,228,213,204,197,183,138,127,122,113,115,113,131,93,76,67,48,74,117,113,116,138,160,172,171,191,107,99,132,252,45,67,105,122,136,156,159,135,113,117,151,174,191,192,191,183,181,194,224,235,238,223,207,186,172,173,166,167,174,189,194,169,156,154,159,187,215,227,244,240,236,223,216,218,224,229,220,202,179,174,175,176,186,196,206,212,214,215,255,5,238,220,202,195,200,202,202,204,255,254,2,2,249,238,229,19,37,28,33,49,63,80,105,139,163,174,178,176,167,170,158,157,160,189,33,41,30,32,37,27,4,235,226,45,75,103,92,89,103,105,106,103,132,151,217,242,248,2,3,26,61,90,103,62,104,163,195,202,204,223,244,12,36,90,116,136,159,173,181,188,193,205,213,237,0,20,19,18,13,19,20,20,249,223,213,206,198,193,194,175,138,51,32,6,193,175,179,178,180,183,191,183,164,176,188,208,236,77,122,138,157,184,196,205,212,204,194,183,130,88,56,30,17,181,150,132,121,111,91,65,75,71,51,132,135,128,128,134,123,113,97,75,52,35,35,57,64,37,14,93,97,88,89,87,76,53,35,40,41,41,42,44,44,36,24,0,170,115,84,73,72,95,148,191,233,40,47,57,41,19,14,4,1,249,211,213,212,209,208,209,206,195,179,173,149,166,247,3,48,83,116,130,113,87,66,229,166,75,52,42,25,6,246,83,68,62,68,60,53,50,28,13,6,23,36,46,40,5,252,250,246,6,20,29,40,59,77,69,52,27,214,197,208,207,198,187,194,212,222,250,24,49,67,90,127,133,138,128,121,111,109,109,108,106,107,106,108,111,127,144,145,137,137,149,172,196,210,209,207,190,198,204,211,242,208,178,215,251,3,1,148,174,199,222,243,0,248,249,250,237,223,221,231,219,221,232,224,203,190,190,208,222,231,213,194,166,133,96,91,16,2,229,220,220,226,234,255,12,19,23,41,43,26,21,12,242,205,196,198,178,144,120,103,200,221,252,22,38,44,44,38,26,39,66,91,109,111,100,76,40,1,226,221,211,212,207,193,201,198,178,153,132,117,127,134,123,113,108,107,104,100,114,129,135,122,117,126,135,152,165,191,177,164,146,130,147,156,161,177,199,205,205,211,232,244,13,40,35,25,14,9,3,223,203,182,180,203,234,9,27,51,45,34,118,143,168,183,179,170,157,148,121,101,83,77,53,254,232,210,179,183,182,181,168,138,131,123,76,237,214,197,184,178,184,211,217,202,183,178,170,207,11,32,56,76,58,29,12,244,222,207,198,168,126,81,44,67,74,85,62,57,59,96,182,199,232,21,72,104,134,136,133,138,128,138,153,176,202,210,214,217,223,232,240,0,235,214,193,185,200,215,249,24,27,36,28,22,15,10,5,255,7,52,84,94,98,97,93,180,183,206,10,43,49,61,84,104,122,143,137,137,120,98,32,1,246,235,9,35,107,115,121,121,128,107,81,63,56,39,17,20,19,16,7,254,250,248,245,222,186,150,184,220,209,198,186,177,172,172,206,215,224,236,255,12,3,250,252,13,80,123,207,227,248,8,23,25,135,153,166,176,164,156,138,116,97,77,66,60,57,47,27,18,57,82,85,69,75,87,97,109,135,155,169,180,178,170,153,28,230,213,215,216,210,192,194,127,112,115,131,154,162,202,233,13,52,48,23,2,254,253,18,42,41,40,14,242,240,249,252,2,8,5,3,170,148,157,166,185,197,198,195,194,200,207,218,232,246,250,14,19,40,11,253,74,67,26,244,229,227,210,180,166,183,236,26,34,29,8,236,216,130,123,125,139,128,120,97,74,35,247,242,168,163,73,33,74,82,67,51,26,249,220,195,188,170,140,137,169,159,162,166,156,145,153,158,147,134,114,110,118,123,131,216,188,168,167,169,163,155,170,19,56,148,179,193,196,196,194,192,190,195,198,202,205,211,217,221,218,204,220,240,245,224,201,196,184,173,172,170,193,227,251,14,34,25,18,14,16,27,27,24,23,18,15,14,18,24,22,11,235,197,111,87,71,70,84,89,76,25,247,230,211,206,216,203,183,136,124,121,118,116,120,133,149,171,11,50,61,59,71,101,121,60,88,91,96,104,135,162,172,186,195,177,164,159,148,139,125,113,105,96,81,77,72,66,60,68,66,62,64,72,78,89,105,118,125,132,145,162,173,173,154,141,154,156,77,19,236,2,18,34,238,192,200,207,222,16,23,19,4,1,4,10,20,40,49,11,245,237,224,209,209,212,211,212,210,193,180,162,125,109,80,42,18,8,255,242,219,185,167,174,176,177,166,154,130,101,89,83,92,87,77,58,118,144,134,124,135,143,119,103,64,37,64,88,64,38,6,223,216,211,215,213,254,17,15,5,248,224,196,155,121,106,89,92,94,94,83,245,227,18,34,43,46,52,47,49,25,8,247,233,235,239,235,230,220,187,131,128,86,77,94,98,103,124,149,159,164,160,158,154,163,174,156,121,86,121,131,101,70,70,94,122,143,148,137,133,127,116,99,104,116,132,128,95,107,111,86,51,25,255,2,255,32,54,111,138,142,189,210,212,203,208,221,235,5,36,63,104,136,139,143,152,154,152,150,148,169,182,165,136,56,18,2,255,1,38,73,80,95,105,175,147,133,126,183,247,252,6,20,81,53,52,75,64,56,67,57,39,252,202,182,189,196,202,215,216,224,234,230,177,168,174,178,190,199,208,221,233,217,184,144,85,238,203,179,121,89,69,78,110,119,122,100,71,65,74,29,17,20,22,32,42,60,86,91,89,118,197,205,222,229,1,30,57,63,53,54,32,12,34,55,88,118,146,182,205,202,29,56,47,22,253,232,217,42,13,238,196,151,84,20,223,177,183,195,220,234,220,26,4,254,11,30,145,160,157,167,171,181,188,191,208,217,218,222,134,122,130,123,125,178,208,233,46,99,94,87,85,83,113,137,151,173,198,9,21,33,26,24,7,229,221,225,8,106,139,155,165,180,197,233,240,54,63,73,78,72,69,56,19,214,206,195,222,254,11,6,242,217,191,167,132,95,82,71,67,63,68,82,95,107,98,100,109,112,111,106,95,69,29,246,224,216,210,204,191,179,183,239,233,219,211,214,219,216,210,223,225,220,179,183,106,109,121,170,174,168,169,179,195,192,188,195,203,216,230,229,216,249,220,203,189,181,157,130,113,114,122,129,127,132,148,162,159,159,155,140,124,81,70,61,45,50,53,59,64,69,91,118,144,148,141,177,168,165,162,138,87,36,40,66,98,98,117,41,38,80,87,122,195,215,226,236,243,5,41,67,153,172,173,150,126,88,48,171,155,157,156,130,72,38,32,110,137,125,106,111,147,158,185,217,216,210,202,204,225,239,237,238,230,234,246,11,25,15,187,202,207,213,223,230,236,240,253,18,40,54,62,81,150,174,11,250,241,225,250,35,73,107,131,123,107,91,77,61,34,238,221,193,168,135,105,93,87,65,153,162,178,184,195,219,229,183,157,151,137,139,118,97,83,79,83,119,221,19,30,38,46,134,147,123,111,127,144,159,157,142,129,127,128,127,124,120,75,71,81,102,101,88,66,77,110,132,162,192,37,56,67,52,22,253,222,186,173,166,166,169,172,180,179,176,171,166,179,199,208,201,199,149,159,156,136,123,113,128,181,193,21,74,126,151,143,124,88,61,41,54,64,83,57,64,78,80,77,78,68,38,252,237,145,153,149,155,158,175,204,205,199,212,61,52,38,32,50,60,69,85,126,143,140,153,177,200,205,202,220,236,241,192,178,160,145,137,134,135,143,158,159,148,150,148,126,86,43,11,254,8,30,28,42,127,180,212,231,155,130,101,72,64,68,54,49,51,65,80,77,66,70,89,108,126,137,146,135,95,90,91,73,98,121,126,143,178,226,247,4,32,27,32,28,38,56,45,36,46,59,58,60,17,232,234,240,239,245,246,240,236,250,5,22,23,9,62,53,69,82,91,97,102,104,100,103,113,118,130,145,164,178,196,216,233,232,25,51,52,48,16,247,224,200,185,199,171,164,170,172,164,156,165,176,194,208,242,23,15,17,21,14,219,202,190,190,194,161,156,163,182,192,241,221,203,199,201,203,210,210,213,228,249,9,238,219,214,212,201,187,177,158,138,150,177,188,213,238,0,25,36,25,251,250,20,42,53,60,69,65,61,60,49,38,41,46,54,60,68,77,83,77,84,92,102,106,99,82,74,4,247,237,226,216,216,219,208,195,174,150,152,151,162,167,156,143,177,187,197,193,177,179,180,187,196,210,219,227,243,15,23,35,38,33,235,238,236,213,180,171,189,193,199,230,222,222,211,121,71,61,64,63,63,244,207,180,152,135,113,95,72,50,41,25,10,248,241,252,21,46,26,16,22,9,1,251,9,13,15,43,76,115,147,177,178,169,168,172,195,200,25,30,8,3,4,6,255,250,11,7,22,17,253,231,250,22,51,105,119,122,144,200,218,217,218,220,221,218,206,186,178,171,166,161,138,106,82,144,155,146,151,152,177,206,243,12,30,61,166,217,231,229,218,216,216,219,225,229,220,217,213,207,183,187,196,228,37,67,94,111,131,136,120,33,27,13,1,228,195,243,29,43,24,253,2,235,203,175,162,149,140,137,134,141,154,138,163,198,227,244,10,31,41,39,47,65,60,52,55,59,91,140,185,222,251,17,16,14,18,26,64,222,213,202,188,180,176,158,138,124,105,93,69,26,231,199,180,170,162,150,138,127,117,108,110,111,110,94,71,45,32,239,164,172,184,183,163,131,107,103,143,182,215,236,245,236,227,181,150,153,150,151,161,174,168,167,155,140,119,123,145,195,186,176,166,135,107,108,106,95,75,68,59,49,40,41,47,41,22,243,232,192,161,150,131,81,62,57,69,84,95,110,131,147,231,17,23,60,47,27,251,225,216,220,242,6,127,143,142,138,129,122,116,105,88,62,76,84,92,95,106,119,131,152,172,184,186,195,199,185,164,149,130,131,136,153,175,196,214,211,214,229,233,63,107,111,112,106,102,96,91,112,145,141,147,155,151,114,140,132,90,83,89,83,80,156,192,198,202,186,209,199,121,130,126,114,140,156,163,173,192,204,209,208,203,209,214,215,227,21,15,11,9,241,205,221,240,17,42,54,141,133,146,177,223,249,234,217,224,237,4,37,7,232,218,222,224,75,59,52,60,71,65,89,125,138,217,228,253,11,26,205,203,218,228,195,176,178,182,164,146,136,128,114,74,37,251,94,33,8,254,248,245,28,68,53,27,255,230,206,174,153,33,5,249,2,18,37,55,67,93,118,159,181,196,203,190,196,222,225,216,226,252,223,211,207,214,209,211,213,220,184,181,198,215,217,225,238,242,246,234,225,222,220,218,212,189,160,176,188,193,185,187,205,221,234,246,252,6,10,26,8,250,215,108,84,160,186,211,225,227,218,205,191,220,240,0,196,216,218,202,182,157,115,95,88,118,131,144,162,187,206,229,220,225,228,240,0,32,63,114,165,200,203,205,221,253,32,57,60,62,69,83,83,80,63,55,60,61,236,0,4,230,164,137,144,153,164,151,131,134,124,115,120,122,132,144,126,99,72,52,27,254,226,222,207,191,178,183,204,249,254,237,220,215,218,225,235,240,233,213,198,185,201,222,241,23,85,146,172,180,169,140,47,16,15,12,16,19,23,37,62,68,69,86,107,98,94,78,49,12,37,59,73,86,82,83,95,122,144,185,8,28,54,71,78,150,169,183,181,104,54,12,241,224,182,174,182,204,220,236,252,5,17,22,207,211,214,224,229,228,228,228,200,203,195,195,204,213,214,210,204,192,180,160,120,65,20,230,194,157,123,139,138,125,123,127,127,114,101,43,82,118,152,186,221,242,253,5,19,38,142,192,209,218,211,204,210,219,19,1,230,207,203,206,205,203,219,217,204,196,194,202,211,226,238,50,16,208,150,119,70,64,59,60,14,232,219,201,180,163,155,153,162,182,199,216,244,20,61,84,89,100,112,106,96,106,115,104,245,208,200,200,205,211,211,210,215,223,7,44,56,60,57,65,85,129,157,182,185,181,163,160,172,192,7,246,234,231,230,248,19,43,48,45,94,125,154,177,201,217,226,226,222,225,237,21,30,40,35,24,21,18,7,183,166,154,151,161,166,171,146,121,110,113,86,57,44,59,69,74,77,82,81,87,87,76,57,21,239,235,220,223,237,252,17,2,243,238,224,223,209,172,129,109,99,103,101,99,100,106,113,140,167,188,211,210,209,206,248,250,252,255,250,0,3,244,242,251,8,16,35,49,64,78,68,63,59,67,68,77,94,112,107,102,94,84,78,86,95,106,113,113,127,207,238,236,9,31,35,33,29,27,26,25,73,117,98,85,87,103,80,53,64,137,149,158,153,147,146,142,136,132,122,77,76,83,96,95,89,91,97,103,111,123,143,143,123,119,152,146,156,165,167,154,137,139,42,8,246,242,152,138,138,127,114,32,19,246,216,187,176,184,189,204,202,189,179,172,151,106,56,7,236,234,240,14,50,83,119,134,138,121,116,131,188,205,216,217,224,220,216,206,197,185,164,144,23,9,18,30,22,11,27,53,73,81,80,82,99,115,137,150,165,182,203,243,37,58,58,71,92,103,94,84,125,133,70,94,127,145,146,251,232,223,210,202,206,219,225,245,11,56,102,115,160,251,11,179,173,151,118,78,62,52,30,3,245,245,10,229,226,212,217,230,226,194,96,69,48,39,54,59,60,94,124,217,221,245,80,73,86,96,96,76,41,19,238,47,48,67,72,251,244,247,8,30,58,196,170,170,176,185,192,180,173,161,136,113,112,113,125,138,150,148,160,7,22,216,210,215,217,219,213,217,58,81,106,126,147,155,172,185,195,200,202,204,181,155,141,145,166,177,218,10,26,16,6,15,33,46,203,215,78,148,164,209,235,39,71,108,156,206,240,50,41,43,52,56,58,84,106,104,113,131,160,188,206,229,165,212,247,29,65,185,196,194,192,185,181,192,192,184,174,155,120,27,13,102,163,199,246,231,223,198,176,166,154,137,138,159,87,89,87,93,189,189,193,190,212,251,219,196,190,195,176,166,177,175,162,149,145,10,240,158,91,87,70,188,185,159,137,119,117,108,122,136,138,136,127,103,13,246,219,221,206,180,169,166,165,163,157,150,143,142,153,167,174,182,200,227,251,24,51,67,83,101,126,137,51,64,81,88,102,137,158,144,138,126,136,165,196,213,206,217,225,221,205,183,196,208,197,199,200,193,189,206,35,21,11,245,248,8,55,135,141,167,179,189,186,159,139,134,145,119,95,82,74,73,74,75,69,67,65,59,64,252,6,229,204,181,155,131,105,82,93,110,110,26,20,250,234,219,108,69,50,248,238,242,246,253,83,108,130,158,170,179,198,227,215,208,199,73,45,23,255,236,188,141,104,76,57,37,29,206,187,160,153,161,165,140,165,183,168,158,160,172,200,224,226,235,246,47,132,139,152,4,9,11,32,56,34,249,220,207,173,192,179,173,179,187,187,185,218,224,226,239,253,175,178,152,120,136,140,156,161,154,142,133,57,49,70,91,55,25,41,58,83,99,101,106,112,71,93,118,132,150,173,183,199,183,185,173,152,129,109,103,119,121,111,84,53,24,13,12,12,25,33,35,45,125,175,177,207,214,203,174,163,120,104,88,105,193,160,134,122,107,37,18,0,234,243,18,108,113,106,115,121,131,136,127,111,27,237,229,230,10,57,78,84,65,31,205,199,197,190,223,133,100,69,37,8,255,1,39,48,249,215,198,186,177,169,152,120,79,44,33,26,16,61,83,112,210,240,228,149,99,76,64,68,61,41,28,19,0,245,233,231,41,34,31,22,28,27,209,194,196,195,194,212,218,221,210,196,191,192,195,199,188,180,185,209,216,221,233,228,226,222,228,20,14,228,227,234,231,88,127,159,167,172,175,166,163,161,162,146,148,154,173,190,74,51,33,7,249,241,243,242,245,13,29,38,33,14,254,255,8,12,245,222,176,203,241,24,55,49,41,7,255,243,216,220,241,9,0,234,216,220,229,250,12,10,191,205,243,9,1,243,228,208,189,186,174,148,127,113,101,44,59,72,73,68,68,71,80,94,88,90,97,105,104,83,85,97,105,81,43,15,228,184,180,149,89,78,64,69,69,71,72,64,74,95,110,134,154,168,169,145,100,59,76,61,53,39,14,253,239,205,172,172,154,136,55,39,253,1,243,175,169,214,241,1,18,31,29,43,101,202,230,243,225,200,174,181,186,185,178,176,185,197,209,208,205,199,198,222,219,216,207,204,203,203,201,217,216,83,67,50,53,50,39,45,70,102,145,193,197,175,160,141,134,149,185,193,72,38,53,62,74,87,110,71,84,116,44,36,40,33,52,72,85,80,77,85,118,137,204,208,221,238,249,7,12,8,6,4,96,90,47,52,59,49,41,49,12,240,249,226,217,215,34,0,229,191,197,235,227,217,234,3,36,70,104,138,160,184,3,39,52,111,109,116,126,138,148,141,137,132,109,109,135,130,125,170,193,195,189,176,163,229,213,194,181,166,109,76,103,131,228,224,232,250,252,248,5,24,41,70,85,100,105,105,91,73,40,145,148,132,153,73,87,110,127,111,66,58,50,226,209,198,221,242,10,34,104,94,79,100,195,188,186,166,146,125,109,211,210,220,190,158,133,96,72,48,20,12,8,242,99,85,243,207,185,169,143,56,52,126,81,69,33,245,204,254,84,121,122,158,220,214,212,194,173,150,147,148,155,166,160,156,169,197,214,222,223,211,33,14,218,0,41,42,46,32,26,9,250,238,241,6,18,18,24,15,3,252,247,160,127,92,80,228,208,211,167,182,151,121,143,155,177,215,236,248,0,249,230,242,236,226,239,243,234,215,201,187,180,169,152,157,153,142,141,144,139,146,151,167,170,183,126,133,143,151,171,179,0,38,36,20,239,247,229,184,173,175,168,159,166,182,189,154,126,94,50,136,118,117,122,122,121,122,127,126,131,132,133,144,169,202,236,245,49,67,83,106,75,69,62,19,233,224,238,246,129,156,129,42,70,85,113,145,161,176,203,214,224,31,5,231,219,221,234,243,241,22,23,27,24,82,112,133,149,120,104,126,134,179,224,193,180,172,171,156,136,122,84,107,177,171,195,192,212,157,122,106,120,134,128,104,72,225,195,182,172,165,206,247,149,153,160,156,152,126,94,98,114,65,145,150,161,176,195,163,138,138,141,156,246,253,21,49,114,110,103,95,77,33,243,239,241,248,1,7,221,237,247,10,23,28,153,195,224,198,162,135,143,136,139,132,121,121,119,109,88,67,52,71,81,83,85,99,109,192,224,54,63,201,202,202,194,166,131,128,117,77,97,162,204,193,172,175,172,166,153,128,115,117,125,137,146,150,126,114,102,93,72,55,40,113,127,119,141,161,172,190,195,222,1,33,53,63,74,87,104,110,114,138,119,94,77,51,24,3,234,233,210,130,105,105,108,114,73,34,25,3,226,185,158,155,140,111,106,185,167,135,67,81,94,112,122,119,122,121,116,121,170,228,232,207,214,241,16,51,87,96,88,101,89,47,251,211,206,195,151,142,135,132,152,160,173,192,230,229,217,222,241,6,24,31,44,51,31,12,8,13,251,208,224,238,234,226,221,228,231,229,83,79,62,72,85,126,159,190,175,171,159,151,155,149,136,135,131,67,56,39,12,17,17,31,48,62,217,188,173,134,139,157,176,168,165,149,172,213,250,253,8,15,11,250,203,249,228,196,185,191,195,181,242,215,144,221,226,55,55,42,27,17,126,130,101,74,66,65,62,59,66,72,70,58,32,196,104,107,95,82,65,148,139,146,151,158,148,103,71,230,188,174,205,193,190,184,170,145,157,165,59,43,30,33,33,98,118,122,125,113,106,102,92,77,65,56,54,51,37,58,56,64,80,104,146,106,126,213,193,190,176,162,165,170,169,189,213,204,214,13,10,3,12,86,86,79,68,99,152,190,220,226,11,57,140,138,146,146,152,146,97,111,129,151,173,190,175,117,113,176,203,8,40,69,120,132,147,178,240,94,106,233,243,240,236,213,192,171,150,129,86,69,86,87,73,59,40,30,20,255,0,253,254,37,51,116,148,176,184,168,151,145,52,92,120,157,185,198,201,192,192,187,204,230,12,24,30,26,57,70,63,250,152,116,130,177,159,113,65,52,33,19,247,85,84,35,47,65,103,173,199,216,218,226,242,17,54,93,104,85,60,11,201,178,106,68,224,194,189,172,164,150,172,183,193,213,230,245,5,7,251,243,237,191,205,238,229,200,185,187,139,143,159,176,176,161,146,140,129,129,122,137,189,217,29,31,34,39,52,68,72,72,67,75,63,9,13,33,50,56,62,80,98,104,115,112,92,83,81,76,69,39,8,247,233,228,235,226,243,250,248,248,9,14,232,238,215,215,202,200,206,210,181,76,101,109,102,96,121,5,16,253,180,131,92,98,100,119,142,167,190,190,169,193,212,67,100,0,16,4,99,119,121,127,186,150,113,79,78,175,194,205,192,183,170,165,229,233,233,6,9,248,234,213,189,169,158,159,165,180,199,208,225,212,125,196,226,231,231,6,0,254,238,220,209,212,218,218,238,235,227,209,182,160,163,160,162,207,212,208,180,125,43,39,233,219,208,183,173,204,198,247,248,251,2,7,1,247,236,221,216,199,186,102,4,5,30,48,58,60,65,131,161,165,201,188,158,128,203,163,143,129,135,103,66,60,45,40,41,21,200,94,120,152,142,60,76,119,110,88,68,66,94,112,99,67,248,255,1,10,255,65,90,105,118,135,92,181,204,197,193,195,202,13,215,123,115,118,129,142,151,167,178,177,181,198,213,212,251,18,44,37,27,21,20,14,12,27,35,40,47,30,202,206,203,197,186,168,148,131,33,173,173,177,178,188,205,203,158,182,186,192,200,214,238,31,73,112,122,139,152,169,214,229,198,195,220,47,201,174,145,131,135,132,118,111,67,28,47,63,86,145,245,146,149,136,128,131,141,152,12,41,1,252,226,94,81,83,91,108,19,18,30,41,42,38,36,38,45,45,47,53,44,25,224,198,155,115,65,54,72,107,132,158,172,187,202,136,93,45,6,248,14,1,1,254,212,219,214,200,197,212,207,192,176,153,137,114,107,61,12,236,250,82,76,151,165,146,141,121,101,188,76,48,28,239,162,131,107,68,49,72,59,62,70,52,20,253,28,34,54,214,246,2,244,235,227,225,14,23,22,28,28,39,58,85,107,98,61,55,38,178,217,194,188,175,179,192,219,249,16,40,50,76,103,134,130,114,101,106,107,106,106,112,104,102,95,103,110,103,92,106,137,151,140,123,118,172,215,216,222,252,174,169,206,25,32,124,149,166,5,27,15,12,9,1,171,185,197,205,19,6,255,255,245,226,209,206,218,234,245,248,249,206,190,189,210,244,227,222,208,162,147,121,90,54,64,68,60,41,19,248,210,238,245,25,82,63,62,45,26,19,26,32,36,18,197,221,205,173,133,99,75,41,205,226,254,22,30,31,38,13,254,11,26,48,75,96,107,119,127,129,100,55,36,234,229,228,215,196,179,181,223,221,219,155,90,159,114,107,102,104,108,103,98,86,86,96,112,132,104,98,109,120,127,138,159,212,200,184,171,150,127,101,105,164,177,192,216,218,218,212,205,199,198,229,74,60,60,225,218,207,182,161,163,159,181,202,226,2,26,38,54,72,245,203,176,202,216,195,184,181,183,112,94,83,74,79,220,232,237,152,165,176,183,180,142,114,114,114,117,100,66,26,233,220,183,162,193,232,241,246,228,173,37,31,41,36,65,69,45,30,13,8,10,20,255,200,212,195,160,130,107,98,74,49,86,54,83,100,104,131,164,240,42,76,111,112,143,127,151,113,132,150,170,193,217,222,217,216,222,216,220,229,249,6,22,27,11,245,178,173,189,19,29,17,7,22,23,23,242,242,249,3,255,205,113,85,103,90,206,201,186,173,85,55,42,46,66,89,105,121,141,162,165,154,145,128,127,127,215,198,194,225,19,131,128,128,137,149,144,129,64,74,46,13,248,255,12,17,22,19,13,249,252,249,244,195,144,105,87,255,237,229,223,215,199,182,177,178,177,208,200,231,223,218,29,22,5,249,224,242,253,8,19,31,36,141,156,173,182,192,193,178,168,167,132,114,109,101,80,62,59,249,235,3,100,86,96,107,87,101,121,160,170,175,182,191,190,183,172,163,251,197,185,179,207,213,220,128,133,146,174,218,214,223,196,108,252,243,241,234,233,0,32,58,62,58,64,62,38,5,250,0,252,251,13,4,16,47,91,129,125,129,142,151,168,202,192,187,188,216,219,227,247,7,249,242,0,38,79,61,222,225,193,153,108,221,241,243,59,37,39,53,241,241,221,212,125,126,133,142,117,103,94,239,203,206,194,186,192,200,230,238,73,214,193,167,193,94,38,210,191,163,150,162,179,152,154,145,139,123,99,90,91,104,118,127,122,230,202,150,152,130,118,78,62,94,161,181,188,187,183,188,191,192,198,203,204,206,211,216,225,207,176,179,220,245,6,11,242,210,174,166,143,253,13,27,16,9,39,31,26,27,29,23,16,12,32,33,33,50,61,68,119,127,33,17,1,247,237,238,8,98,117,124,120,115,113,107,112,221,9,44,53,48,81,114,43,76,94,88,77,80,80,94,123,156,179,218,156,145,135,123,97,76,70,71,74,74,75,72,66,50,52,59,56,60,56,60,70,78,73,122,132,151,124,118,128,147,153,137,117,55,235,11,38,51,63,62,67,150,166,173,182,187,204,215,26,28,11,8,10,38,69,76,89,243,244,245,236,216,198,199,202,207,219,216,207,175,8,11,7,2,255,144,188,185,176,172,154,139,128,78,65,70,66,76,90,94,87,73,141,178,171,140,100,121,148,152,166,96,92,5,25,111,125,78,45,33,19,22,16,248,233,219,210,215,226,58,40,30,28,40,34,253,2,15,4,242,212,162,133,117,102,95,82,70,101,120,157,194,29,66,61,66,77,61,41,25,10,249,237,223,217,228,238,248,215,133,150,241,35,80,90,107,122,141,182,174,173,172,168,165,165,153,141,155,167,180,201,190,175,49,135,153,155,141,48,29,42,47,69,83,101,126,149,145,123,134,135,132,95,64,84,96,112,135,151,147,138,131,109,86,72,22,6,245,223,223,224,216,234,16,7,217,120,204,190,202,217,231,160,137,155,153,147,138,124,135,162,191,213,187,140,74,56,51,35,23,11,244,64,96,221,200,115,118,106,90,85,19,18,11,10,16,7,253,71,34,17,223,122,88,60,44,51,166,157,224,224,206,211,218,230,249,242,156,157,158,164,171,176,184,195,199,206,240,210,175,139,110,192,49,38,53,140,141,136,142,141,127,98,72,107,109,3,13,26,34,32,174,200,199,198,199,206,229,1,24,84,38,241,233,251,57,75,98,77,81,76,207,181,137,90,60,27,3,225,62,35,242,158,184,198,240,18,150,165,182,177,176,179,163,153,169,159,173,202,212,219,224,229,39,99,105,115,131,137,215,212,137,129,90,101,94,55,151,177,191,32,28,67,45,32,28,34,42,52,221,212,54,158,151,158,175,186,184,187,231,36,61,67,75,186,184,213,242,16,27,26,13,252,204,206,197,75,72,66,64,54,63,73,83,93,114,79,92,97,93,81,213,212,211,208,205,207,176,169,166,170,246,247,213,198,194,202,203,192,203,243,233,222,217,221,28,83,97,81,68,172,209,207,209,182,170,184,200,219,204,183,201,214,233,247,245,239,219,192,174,205,204,197,189,194,200,178,156,131,103,93,108,104,108,123,136,132,117,120,124,132,168,175,167,163,163,153,40,60,72,75,60,41,48,59,62,55,53,52,74,174,156,148,139,37,248,5,19,55,126,116,122,161,215,12,28,101,106,117,144,153,161,184,208,239,237,240,249,24,64,148,180,192,182,166,125,117,95,67,73,131,154,140,66,47,38,44,50,76,102,79,87,89,142,166,196,231,251,238,227,214,196,232,244,13,248,228,250,232,241,254,14,38,57,61,150,174,215,211,194,199,213,234,235,235,234,241,134,119,126,100,220,186,212,226,15,31,74,126,116,99,85,72,47,27,233,212,188,171,148,125,111,86,70,53,12,218,174,176,187,194,188,185,181,183,203,227,241,178,176,172,168,146,127,147,172,141,70,73,108,161,13,8,24,37,33,5,220,185,177,149,107,133,147,161,178,175,170,156,130,131,132,131,123,118,62,68,56,57,101,122,114,114,99,73,130,145,30,6,251,211,202,185,171,167,168,171,169,172,173,162,208,231,240,181,201,208,154,173,100,208,225,255,29,165,163,151,141,106,68,46,24,44,21,60,82,87,81,71,67,75,86,20,30,128,144,130,144,157,218,174,174,158,44,59,62,53,149,158,160,148,138,126,205,230,217,207,212,222,243,246,222,239,187,148,132,170,179,163,166,143,247,236,0,38,57,48,158,206,253,77,87,41,54,77,101,92,108,120,126,155,162,156,151,159,52,82,108,108,108,121,133,144,248,235,32,43,36,49,74,81,63,38,16,45,224,226,227,230,217,219,232,243,248,8,20,35,29,9,21,40,58,67,78,91,97,91,95,161,166,175,186,204,218,229,248,244,48,74,88,84,77,53,33,12,248,227,208,187,175,159,161,153,151,155,161,173,184,164,73,40,29,26,18,3,32,78,109,186,206,12,246,207,188,194,218,21,46,37,2,226,207,206,215,222,216,206,194,180,177,109,131,172,203,174,182,208,233,246,38,60,60,34,234,217,253,52,63,64,63,76,82,77,68,61,46,37,25,28,35,43,40,48,59,60,62,66,86,73,73,84,115,82,250,207,204,221,234,204,181,154,132,133,138,131,191,198,211,213,171,175,206,208,214,221,220,220,40,35,24,31,51,49,58,255,235,187,141,142,137,185,191,191,189,188,227,213,210,212,137,97,74,65,68,76,64,59,78,206,193,172,158,144,130,125,115,101,83,62,42,35,45,37,21,227,79,247,0,28,1,100,129,159,179,185,173,159,144,132,207,191,168,120,70,51,20,241,37,42,28,255,229,202,232,34,54,131,129,122,111,29,243,227,222,165,162,159,146,91,75,177,179,165,157,157,158,160,178,201,208,245,165,194,216,229,237,242,237,225,217,218,219,217,213,170,185,70,66,80,129,237,39,133,150,167,12,36,244,237,221,195,157,151,139,123,169,200,42,45,55,56,54,54,60,66,61,60,157,169,16,72,237,219,216,175,176,189,154,130,123,119,108,90,81,51,13,237,220,216,201,130,124,119,99,107,115,117,48,247,215,185,188,196,206,220,197,171,150,127,98,72,247,232,226,239,1,87,132,137,146,166,175,98,127,222,203,199,187,153,82,101,111,57,65,63,42,33,34,42,55,220,200,250,30,44,104,106,118,132,147,149,173,191,214,246,78,99,89,68,53,21,195,209,230,246,10,147,157,150,144,143,135,128,120,114,117,66,36,28,48,68,88,104,113,115,117,120,140,160,171,192,209,208,214,207,200,167,161,151,136,109,102,110,115,131,146,165,178,191,215,242,219,203,243,109,115,108,107,109,81,68,78,92,134,175,178,149,86,100,98,184,190,200,206,200,219,27,87,118,118,113,100,163,155,151,157,161,211,206,195,192,202,209,218,136,55,53,39,172,190,49,41,162,153,153,211,8,197,210,210,218,223,196,250,203,207,200,209,215,209,101,46,54,76,65,94,127,156,156,216,163,168,170,172,183,198,193,129,133,125,97,74,123,206,245,243,245,254,243,208,123,98,77,68,52,27,4,240,228,220,211,191,235,237,228,235,239,0,15,28,65,122,152,216,202,175,160,146,231,242,235,212,198,49,216,205,209,199,193,196,201,204,206,218,163,165,178,191,212,231,223,209,242,228,123,124,181,207,197,170,166,208,225,234,243,17,29,5,255,26,61,70,48,187,191,221,202,224,241,133,164,1,237,223,206,189,135,146,149,150,150,155,172,189,196,62,85,101,196,186,174,56,66,97,87,66,54,137,233,250,208,149,169,104,101,122,152,37,25,5,214,202,205,149,155,157,218,245,255,255,183,232,235,249,23,68,117,149,159,139,112,22,2,3,14,75,93,80,69,110,132,126,110,104,108,213,91,103,85,67,187,204,213,242,5,40,54,69,83,84,152,160,174,178,177,161,130,97,228,233,250,3,12,24,34,31,29,191,201,204,202,215,227,233,235,232,234,235,237,207,186,219,217,216,203,190,170,117,66,54,45,27,4,231,196,160,114,27,55,87,109,131,252,72,134,162,173,195,211,222,239,38,134,178,195,211,217,45,30,9,238,206,184,193,198,204,197,186,202,212,197,195,186,196,202,205,206,219,7,39,19,248,209,161,130,170,207,36,57,71,93,68,224,218,145,142,165,16,254,39,56,84,84,102,123,93,72,232,214,202,212,218,213,203,198,204,219,53,61,63,66,58,47,45,47,202,207,207,197,173,151,145,144,154,165,186,10,252,239,228,241,9,44,58,60,61,54,153,164,176,191,221,227,230,230,236,227,216,218,217,203,152,62,19,25,29,94,116,152,20,19,17,30,66,69,76,77,73,75,254,217,225,241,0,16,40,5,234,228,217,209,194,154,111,97,92,97,174,127,25,235,215,208,254,252,249,253,12,6,240,222,241,4,24,40,58,68,70,57,59,70,97,117,105,68,66,75,88,217,239,14,55,42,33,29,242,171,158,123,70,73,90,153,161,152,146,147,144,138,140,158,56,77,100,84,89,162,134,133,180,181,181,222,4,6,247,246,73,111,130,152,130,126,235,204,171,173,170,173,193,178,146,58,71,46,30,13,243,225,223,222,152,147,129,104,98,86,79,223,228,220,223,152,142,248,35,52,41,247,5,16,74,66,64,145,152,159,190,217,103,121,133,181,227,30,66,84,120,154,157,160,162,240,220,205,197,189,195,207,210,223,241,10,146,115,108,157,211,225,4,14,11,230,187,216,30,61,57,13,244,236,219,220,224,251,18,221,222,213,189,192,192,207,215,226,226,203,152,105,79,57,43,32,63,27,227,121,149,112,99,81,97,113,112,112,102,15,69,76,252,252,11,39,170,140,146,174,176,166,175,178,175,155,130,99,113,126,151,156,153,130,41,32,43,203,208,193,201,74,86,95,163,166,192,196,199,172,150,139,125,140,171,164,192,232,18,6,2,250,243,188,109,116,114,116,100,206,103,104,116,147,182,32,40,52,58,59,59,40,117,119,108,94,95,105,111,134,159,185,205,209,219,233,248,9,30,201,201,184,218,197,205,182,168,161,26,26,45,170,15,248,240,241,222,180,162,160,102,51,83,177,198,169,254,179,188,190,156,185,168,150,247,97,86,197,206,209,180,157,141,122,147,150,143,141,245,168,162,156,146,174,192,26,41,57,112,145,149,61,91,190,174,118,208,238,231,231,192,201,171,74,52,39,29,5,34,110,113,132,189,126,115,113,122,78,71,67,77,82,64,225,0,52,147,138,127,84,46,70,83,101,130,139,51,38,253,230,227,217,213,236,225,223,229,232,245,253,53,82,107,116,170,162,176,206,44,25,8,246,204,154,124,101,94,22,9,145,118,164,171,155,216,233,241,247,14,50,87,105,123,125,136,25,29,11,11,24,50,70,99,26,237,204,203,183,216,218,170,172,184,194,201,162,224,229,227,222,227,236,12,148,196,116,126,128,166,172,226,43,93,143,238,2,30,44,104,109,58,89,111,166,187,237,204,180,204,209,189,133,114,91,86,81,106,136,76,247,251,254,6,10,10,253,36,59,175,184,161,129,32,239,225,213,35,80,83,71,66,67,28,246,228,138,128,131,129,123,207,220,253,128,101,72,4,248,233,217,79,83,74,60,56,42,37,82,85,66,49,47,251,255,3,94,198,186,177,42,228,140,107,113,193,187,169,165,141,109,70,32,13,42,38,47,33,19,9,24,67,188,173,177,202,221,211,192,178,179,164,170,250,223,220,203,75,50,56,108,139,183,174,161,157,158,156,127,125,180,240,235,226,41,60,57,39,234,236,14,24,33,28,3,233,246,32,86,72,61,202,206,224,237,5,41,31,91,135,169,178,219,1,20,243,231,209,186,115,65,81,83,61,63,63,93,81,85,134,90,50,129,114,208,191,183,51,61,62,64,69,78,76,63,52,55,83,88,81,113,179,182,168,142,101,105,90,78,77,63,30,12,5,180,145,144,162,166,86,129,220,242,255,10,29,145,123,118,0,254,254,10,25,52,47,34,41,74,115,160,15,0,229,182,175,192,197,205,222,219,185,213,204,196,184,185,205,81,65,41,28,44,46,43,68,84,117,193,188,136,133,119,114,195,205,212,255,17,61,96,28,30,57,44,90,138,142,149,199,211,231,2,14,14,18,12,115,44,54,62,76,68,184,250,6,15,90,28,251,244,203,13,251,222,196,208,225,242,16,35,56,83,106,230,6,17,95,105,153,137,131,134,80,74,81,39,74,206,194,188,146,137,247,225,179,171,18,208,209,247,237,7,27,62,82,117,103,92,163,243,38,109,124,148,163,204,28,53,188,206,223,209,144,130,111,248,214,66,73,77,6,251,3,207,164,165,248,216,156,203,206,142,145,163,177,183,168,231,232,237,40,184,168,75,61,65,248,220,26,38,40,16,253,253,254,250,250,165,149,136,121,110,110,181,202,119,147,144,161,182,231,231,252,248,1,250,228,210,186,182,164,136,166,126,161,178,181,167,133,140,207,223,232,251,18,47,41,27,72,21,3,0,5,90,147,171,171,167,167,175,187,209,121,120,103,85,67,115,100,98,123,122,121,111,122,131,135,138,165,51,65,65,241,32,59,80,14,225,224,196,29,54,64,96,156,158,156,179,228,218,215,192,194,205,223,237,246,254,14,11,248,173,133,123,136,164,185,178,117,108,27,19,245,214,190,177,174,177,183,173,150,59,67,77,147,132,100,66,193,190,179,174,165,156,165,173,170,167,168,108,64,51,216,161,178,202,134,254,241,249,48,104,110,100,85,70,70,7,228,4,25,28,28,165,186,164,118,99,158,139,132,119,98,80,95,86,78,100,113,146,54,190,200,203,188,162,138,131,158,148,171,169,168,201,116,120,128,150,157,164,109,102,44,24,179,162,176,196,99,118,123,118,96,81,164,100,86,69,44,102,105,116,118,25,245,234,188,186,83,112,125,126,114,97,96,222,241,251,223,207,213,248,250,167,111,106,253,224,182,133,135,127,126,151,156,189,239,242,220,204,206,221,241,9,32,41,53,62,66,59,2,235,226,227,229,229,46,15,133,149,171,188,167,167,189,173,157,152,25,252,9,253,167,163,166,179,182,200,20,16,25,233,190,224,201,180,202,213,192,181,36,193,209,210,59,51,41,155,64,57,52,53,62,84,69,75,71,42,21,246,113,107,95,159,148,178,126,66,229,207,184,237,213,192,152,171,185,193,50,45,20,23,116,104,105,84,53,26,51,65,123,139,12,117,172,159,145,152,156,162,158,49,57,34,148,3,160,154,38,78,104,208,196,124,133,142,152,119,250,193,176,48,7,94,76,61,16,8,254,248,61,138,145,159,175,168,74,98,181,173,173,169,176,179,0,16,24,31,37,21,197,146,222,219,199,219,46,58,25,9,245,29,31,46,17,228,227,221,215,33,102,177,181,188,186,186,148,140,121,119,153,170,181,201,210,235,251,7,29,25,20,248,247,240,11,6,213,173,174,184,121,149,154,168,193,196,151,119,119,111,206,29,35,38,78,85,72,8,10,65,85,98,118,130,104,88,81,87,86,244,233,230,223,218,251,27,12,253,26,42,62,137,174,150,228,207,189,170,211,222,224,37,69,86,79,76,70,104,112,137,111,146,201,214,218,253,237,203,129,127,185,53,34,213,196,188,174,140,232,202,154,57,30,28,146,213,216,228,250,5,14,254,236,206,198,227,215,249,210,183,141,153,211,143,97,73,64,69,247,11,1,249,203,198,188,174,34,62,210,175,146,127,116,97,83,55,62,69,39,36,162,186,200,60,139,123,55,23,32,100,108,99,139,149,75,29,239,228,185,255,198,120,114,123,179,187,183,179,199,215,219,13,29,26,62,48,36,30,15,30,18,3,247,14,43,71,201,200,150,142,132,165,167,199,216,215,222,137,172,204,194,185,63,157,147,182,220,195,170,125,78,116,156,195,230,153,144,146,160,249,241,229,69,77,5,51,31,26,32,52,47,47,47,52,64,57,31,224,192,163,21,44,163,167,164,169,134,217,16,9,4,1,207,220,203,184,224,142,118,102,176,157,151,78,106,159,153,149,134,127,138,86,72,41,228,222,12,21,65,89,121,80,222,251,15,29,79,95,107,106,106,94,110,126,100,144,131,111,218,235,15,26,57,40,14,234,44,22,179,187,186,185,12,0,184,247,242,248,2,95,190,226,236,250,123,83,54,77,242,234,66,67,5,8,18,38,46,53,55,186,157,63,250,210,186,157,31,18,25,43,37,31,22,238,242,248,9,21,90,49,4,244,228,169,240,247,102,80,83,81,95,124,84,89,102,113,127,237,231,163,149,129,104,77,66,74,180,187,199,220,245,238,231,199,192,97,98,143,158,183,202,41,50,60,220,251,188,192,86,58,71,140,144,155,196,121,107,86,88,96,110,223,42,20,4,30,37,71,4,253,59,52,196,159,100,14,104,89,105,135,65,190,206,209,223,236,25,32,25,12,252,226,236,255,15,6,253,110,84,210,169,59,31,30,177,146,156,157,200,113,156,147,138,136,167,102,108,249,239,229,34,28,1,0,245,165,108,97,76,58,35,255,235,230,233,230,182,181,252,234,36,44,26,244,249,25,32,125,135,158,176,191,174,80,60,91,109,177,191,179,169,168,171,158,155,206,212,135,135,159,191,221,235,216,19,52,81,68,63,73,88,83,63,57,25,9,0,252,251,7,127,107,107,147,154,179,182,221,216,228,254,24,34,8,248,252,234,249,94,213,13,24,32,156,202,116,129,128,137,148,167,184,205,226,173,138,108,41,247,222,214,190,166,142,130,143,177,65,95,78,45,5,192,194,187,200,205,117,41,248,5,15,26,40,30,27,34,42,47,55,46,48,147,8,11,114,129,140,235,68,62,49,74,155,74,99,112,102,64,70,185,195,157,150,142,138,68,63,63,70,81,76,68,80,73,30,52,48,52,68,81,96,72,143,119,102,95,161,158,217,244,16,42,68,164,22,22,66,108,114,252,252,251,229,221,222,8,2,15,127,188,173,32,54,61,51,52,100,101,98,143,180,210,78,43,164,158,163,174,115,163,164,16,39,245,233,220,83,40,66,81,212,16,108,143,160,171,64,59,14,240,246,129,142,170,226,171,116,145,157,179,213,167,166,176,13,24,163,121,141,147,159,2,40,82,95,157,165,163,127,118,105,98,186,231,233,229,194,58,108,208,183,198,153,153,176,203,229,0,81,73,65,248,204,104,39,24,5,250,241,11,242,75,54,33,150,10,233,202,180,1,20,9,140,156,180,170,174,192,198,196,199,156,127,120,115,45,27,157,150,145,148,134,86,121,112,254,8,21,38,218,192,182,216,25,224,71,83,125,113,116,128,148,114,127,157,182,205,189,213,86,109,97,150,142,101,134,137,150,172,190,201,193,181,176,161,23,99,75,182,174,46,6,74,66,59,62,49,94,65,42,96,145,159,157,161,167,0,18,2,164,174,176,189,189,183,214,216,2,11,91,233,181,191,190,6,188,196,179,158,134,95,87,92,113,169,25,42,53,71,63,39,236,1,152,128,104,99,137,134,177,149,140,2,40,72,104,39,130,176,200,239,2,236,247,254,252,14,44,65,202,170,178,191,208,234,233,194,191,137,40,20,96,77,61,51,202,51,30,249,148,200,212,204,197,193,166,189,139,118,162,92,30,245,192,68,92,140,154,181,179,172,133,132,141,120,115,64,80,93,53,6,125,248,216,165,176,171,165,164,252,159,191,227,244,14,175,172,163,161,156,126,90,62,19,66,66,128,141,62,67,161,148,207,73,205,241,28,68,132,42,29,115,90,138,94,122,137,129,240,205,62,69,78,100,7,243,221,223,225,222,202,209,218,235,230,210,22,5,44,11,12,31,48,104,109,93,166,179,176,221,237,44,60,90,105,128,196,148,160,146,22,25,34,42,116,141,168,196,231,70,207,227,236,226,201,197,185,72,91,152,248,239,98,86,216,80,85,96,89,97,31,17,30,187,172,121,198,199,202,220,60,47,248,57,19,196,121,100,70,69,96,163,123,125,130,128,23,36,233,240,109,159,173,166,158,226,46,19,193,126,91,50,142,77,161,160,162,161,75,62,204,159,161,162,204,219,219,187,170,120,209,219,250,230,221,223,224,225,154,173,76,64,41,139,105,38,192,75,47,53,165,159,145,218,127,38,5,227,122,130,252,226,207,214,8,30,133,118,103,175,82,84,75,56,218,190,188,235,131,121,123,106,89,47,191,201,121,14,90,107,218,226,27,130,87,78,72,84,7,119,55,51,184,183,191,49,131,113,145,149,143,195,163,159,211,204,172,190,197,191,183,193,214,44,46,42,7,92,133,165,164,177,190,152,106,87,88,154,152,137,77,30,238,228,223,227,220,226,25,159,203,185,105,138,182,181,189,198,196,204,144,157,164,165,168,243,138,145,232,233,235,244,197,228,238,107,74,44,31,7,244,223,207,143,155,63,50,62,138,175,186,130,130,113,243,166,253,131,92,75,146,111,125,110,117,212,220,205,203,227,4,68,65,93,148,144,172,131,116,3,2,8,41,40,180,194,200,222,198,185,13,29,62,224,172,100,132,182,232,3,56,174,188,203,60,4,173,196,182,205,191,189,188,193,255,31,196,197,229,188,62,20,216,79,56,163,246,250,245,239,57,79,253,3,0,62,57,244,219,211,208,209,120,123,85,63,202,201,4,153,247,19,30,8,128,53,58,198,154,143,138,85,95,111,80,90,94,207,164,155,143,123,84,242,129,58,37,10,154,186,172,170,217,199,182,181,203,198,30,234,252,194,168,171,175,187,224,184,153,129,44,98,110,239,241,119,151,173,80,79,196,140,139,123,101,107,138,166,196,224,127,121,86,82,105,143,176,199,148,126,79,90,132,222,242,12,203,165,23,21,243,247,195,122,146,162,194,186,179,99,88,226,234,240,221,197,142,152,34,54,190,229,197,109,83,60,54,52,209,153,142,158,241,240,69,82,170,89,88,210,218,228,215,210,235,252,51,74,254,243,107,8,162,250,14,50,120,121,121,58,9,82,193,202,204,217,225,24,249,248,125,117,146,109,211,75,54,56,34,250,2,2,6,121,111,20,55,52,36,20,114,146,161,210,179,166,185,0,36,42,46,120,142,160,170,243,65,66,146,63,48,22,71,109,91,189,194,111,113,253,11,1,249,80,71,29,168,165,120,83,10,246,165,147,147,185,195,211,232,68,18,30,61,22,248,207,31,89,92,29,217,26,8,222,14,44,72,12,38,118,132,146,163,182,157,226,193,1,122,81,217,26,38,4,232,3,251,173,123,130,153,160,217,202,204,20,107,69,60,6,114,100,89,78,44,207,176,101,230,103,251,73,236,220,56,13,35,152,172,72,206,209,110,106,112,144,82,94,10,23,197,228,201,199,180,138,202,31,49,65,139,151,190,25,24,35,60,219,200,180,167,51,46,39,49,54,24,155,16,193,37,31,41,30,2,65,112,140,124,137,139,134,52,246,159,160,71,162,158,245,39,230,228,254,32,23,108,159,165,174,6,55,164,21,8,218,88,202,45,127,198,190,117,28,48,113,89,53,166,155,52,158,25,195,39,35,20,2,229,185,147,10,13,50,78,11,217,120,120,90,159,150,18,99,220,185,173,252,245,14,65,70,22,40,73,187,21,194,136,224,37,28,90,12,19,22,24,168,147,39,237,244,32,131,62,71,91,183,44,20,13,195,162,135,12,123,118,91,232,167,119,32,58,53,127,130,117,153,12,53,37,77,97,175,180,77,89,47,127,39,108,20,236,228,16,165,173,89,126,167,95,150,61,50,26,6,249,230,225,141,49,41,134,145,163,167,250,254,122,136,212,182,186,69,58,157,159,168,185,190,217,169,70,78,34,61,104,39,229,177,51,251,149,181,114,107,133,218,228,177,7,176,175,223,151,139,104,117,222,10,199,218,133,160,251,205,112,116,43,151,163,166,70,42,219,223,83,48,121,195,252,177,125,192,189,239,31,209,158,77,134,46,52,198,205,198,158,21,43,230,178,114,29,192,243,228,8,191,52,41,74,205,2,27,170,189,148,22,74,128,188,};
int windcodes5676[] = {152,138,172,172,141,158,157,124,119,130,125,143,190,206,205,187,171,175,136,86,111,80,0,210,177,155,125,96,80,66,162,135,121,119,150,136,86,91,93,193,201,212,203,213,227,230,243,228,210,186,138,68,242,217,216,172,145,199,31,43,57,82,110,119,95,49,31,4,13,17,254,198,173,161,182,204,108,133,122,87,88,103,64,47,44,29,26,255,235,192,133,122,156,154,84,67,68,129,169,191,181,182,140,91,49,51,62,52,76,79,52,19,241,207,175,208,225,235,225,202,207,193,192,235,236,236,224,242,250,237,252,23,13,250,231,230,217,194,193,209,173,105,76,52,39,11,246,236,233,245,240,1,246,206,177,151,91,98,170,212,207,238,5,22,29,34,43,44,45,57,74,80,85,142,153,157,129,126,110,52,13,27,44,41,2,241,248,5,11,18,34,58,59,37,16,216,191,168,172,169,165,167,137,127,231,222,228,251,24,47,81,113,147,163,142,122,82,38,59,80,81,109,154,156,55,33,36,19,42,43,30,16,12,15,24,33,20,13,2,3,6,5,252,231,191,139,91,90,82,103,158,195,205,223,229,57,90,55,42,16,20,30,37,72,128,161,184,207,200,213,220,204,198,206,212,213,185,153,132,103,75,49,23,21,23,24,50,63,53,40,25,25,45,77,92,96,75,69,54,25,23,23,21,2,243,236,220,180,133,134,162,212,241,238,247,236,246,252,250,249,234,196,169,203,221,220,206,162,168,161,106,11,247,241,246,253,242,213,17,235,193,193,240,1,13,22,0,241,228,215,238,10,29,53,31,53,47,12,9,254,234,232,235,249,234,235,228,218,205,167,182,199,208,174,255,247,237,151,184,16,74,73,59,38,23,46,76,10,31,24,252,232,240,253,252,5,222,199,228,9,13,253,241,242,3,42,27,1,12,21,242,213,177,159,186,191,167,112,58,28,47,46,67,119,196,222,238,231,219,195,195,191,16,249,250,3,12,15,18,22,34,31,24,26,41,44,39,45,44,36,23,45,54,44,43,50,88,115,38,53,38,64,105,158,192,191,194,161,189,186,161,152,91,158,144,155,162,111,71,28,1,230,234,7,28,21,28,46,88,76,42,1,224,238,227,233,224,235,251,8,4,2,4,9,26,22,1,20,54,58,57,83,97,120,83,38,31,32,36,109,195,200,202,208,216,130,148,166,191,168,169,180,169,158,161,168,182,197,158,125,131,93,52,34,17,17,250,231,231,243,230,218,235,47,30,16,25,30,21,41,55,96,104,102,113,133,180,196,216,185,142,113,145,150,108,105,63,34,253,250,13,46,63,73,111,85,74,54,77,103,159,197,219,133,191,216,243,16,26,14,15,21,28,38,205,244,32,48,58,44,44,52,40,22,247,254,5,251,8,19,16,255,4,25,35,43,51,34,1,12,20,15,9,255,247,1,17,16,25,4,10,18,14,12,45,17,245,239,228,220,174,150,124,94,87,41,51,68,80,52,45,80,89,104,87,249,196,205,223,224,237,246,240,228,218,211,3,32,62,154,137,79,47,22,24,24,25,6,237,229,234,7,19,32,30,26,12,19,36,85,117,158,175,180,173,157,244,221,220,234,222,204,196,209,233,239,242,232,220,223,196,183,208,206,231,234,224,203,221,237,233,221,209,179,160,138,137,142,132,118,91,76,78,71,56,31,19,9,2,11,14,17,11,3,0,249,19,35,37,80,79,65,53,51,39,26,22,19,12,5,13,16,19,22,45,66,113,102,103,113,135,104,73,60,63,82,65,74,90,50,21,19,1,3,18,24,17,19,35,38,48,249,240,252,244,224,246,253,244,247,223,206,179,138,92,118,139,89,120,126,162,214,243,252,3,3,2,253,236,210,45,25,194,226,255,3,2,254,1,8,5,5,5,14,245,240,248,234,148,161,120,121,180,224,219,230,236,233,225,229,217,233,211,103,110,143,171,157,184,146,102,86,46,16,6,236,250,241,230,238,250,255,253,6,13,5,252,244,229,161,106,53,29,15,246,204,175,173,144,146,247,250,9,2,243,24,30,31,40,52,21,4,246,242,237,237,238,237,191,158,202,239,232,3,14,19,28,38,52,98,152,244,249,43,63,94,100,93,39,21,6,253,2,23,36,50,207,225,213,233,245,226,221,233,14,3,236,198,228,1,251,228,226,220,205,222,228,226,188,129,89,185,167,171,213,211,218,245,36,137,131,85,48,11,241,250,11,14,34,63,87,133,177,193,217,235,8,54,101,90,76,62,57,67,59,41,13,1,6,23,29,41,80,134,168,223,11,31,31,41,38,27,23,16,236,201,194,222,233,237,0,10,4,249,251,14,40,31,7,236,167,136,172,171,15,28,40,44,36,14,5,14,253,234,234,242,244,239,230,0,34,74,88,69,69,16,245,253,248,222,55,61,91,4,5,252,239,217,213,200,150,131,139,145,160,185,209,220,228,210,202,170,131,108,11,246,242,14,27,26,25,28,34,45,60,64,90,151,246,229,236,255,247,253,2,243,235,229,244,10,177,173,195,198,185,202,187,135,103,81,31,246,232,245,241,250,246,251,10,17,16,6,15,21,10,10,19,18,23,29,59,46,31,19,14,20,24,22,10,24,62,57,58,45,28,27,42,58,48,43,53,38,47,38,10,249,15,17,36,68,69,47,29,31,47,12,17,31,48,52,62,121,140,174,210,254,22,47,56,29,2,234,53,39,48,80,88,121,162,179,203,214,223,214,212,229,254,215,158,112,161,208,228,222,218,184,171,158,174,217,242,51,32,156,130,127,156,189,199,210,194,157,105,77,94,64,37,18,14,6,230,217,207,203,224,224,201,230,11,18,38,55,33,18,3,9,16,34,19,218,145,145,120,90,85,66,51,39,20,238,209,173,180,253,245,219,218,237,238,231,232,197,184,236,254,248,240,252,14,30,13,254,4,255,246,254,8,253,4,233,7,36,53,36,33,30,9,12,33,65,85,80,71,52,39,47,61,77,121,112,92,153,144,154,148,115,63,8,20,25,25,19,41,34,30,15,207,101,75,170,169,188,209,201,201,177,207,217,216,199,214,229,254,28,56,102,137,163,167,149,205,163,130,145,159,191,203,188,171,194,164,148,165,128,141,179,189,150,165,170,184,226,234,221,193,227,60,88,134,165,150,143,102,59,43,29,20,17,22,28,23,21,23,27,45,39,20,224,225,237,227,225,243,255,253,252,231,188,180,214,211,218,229,153,124,78,33,17,13,3,3,25,31,46,33,7,251,3,22,30,31,24,40,71,122,79,15,177,111,86,100,117,105,169,207,199,206,220,237,251,247,231,221,177,178,98,98,60,73,134,182,173,182,176,144,64,60,113,149,97,47,36,38,24,254,218,225,242,241,229,221,166,76,39,25,45,69,109,156,187,87,77,86,89,91,31,17,33,67,62,65,67,106,136,204,213,210,207,216,11,17,36,72,101,15,233,224,231,243,250,251,252,25,28,23,11,1,11,4,253,16,21,11,4,13,29,53,69,109,171,198,237,229,18,55,52,68,37,48,111,68,45,41,33,30,45,63,77,67,59,27,26,39,94,189,218,234,238,6,10,244,246,25,24,20,41,34,6,255,248,246,234,229,12,34,36,29,36,56,38,38,47,74,72,56,39,39,190,224,254,12,12,20,36,24,7,11,22,47,220,205,50,30,7,207,201,20,26,32,28,13,236,67,19,2,243,254,3,13,19,16,29,30,31,8,253,1,29,79,113,71,72,225,234,252,2,254,234,220,237,0,3,226,209,181,132,133,155,150,98,84,57,76,129,169,10,25,6,16,17,33,49,72,101,101,65,40,30,22,10,16,52,95,93,84,78,80,104,138,124,79,46,25,251,211,171,183,224,207,207,230,255,243,229,238,242,251,0,244,232,228,233,226,219,224,187,169,159,254,2,3,15,22,61,47,247,190,169,183,189,161,105,67,52,63,40,19,247,4,14,9,255,30,30,20,36,32,21,236,243,238,215,198,188,197,189,239,245,254,12,5,34,30,46,33,23,27,33,25,18,7,246,244,241,4,12,16,47,87,108,120,105,115,159,201,212,218,171,99,10,214,113,65,22,27,6,232,201,202,213,3,252,238,238,244,230,220,186,175,181,190,182,223,229,231,5,247,1,5,9,242,233,237,249,225,201,235,0,236,215,182,170,164,148,196,229,239,202,177,218,244,249,3,6,228,182,166,188,245,247,247,251,248,251,234,225,241,238,1,189,197,180,167,175,162,144,133,124,117,129,147,145,169,134,123,154,123,123,124,80,61,77,53,58,78,97,90,58,63,74,50,43,36,9,7,4,9,5,0,242,241,0,5,14,15,10,12,21,22,20,8,242,241,22,248,245,235,219,203,19,8,21,32,39,46,63,253,88,68,96,94,61,49,40,66,49,28,73,61,23,21,21,25,12,18,15,12,25,22,14,0,254,242,233,3,43,25,16,10,7,10,5,2,15,25,38,42,35,62,91,160,170,126,109,93,95,107,118,112,147,122,86,68,56,49,47,59,53,48,46,75,123,102,54,27,45,79,116,122,127,90,52,36,8,229,18,43,18,4,1,231,238,254,1,29,46,26,16,12,17,10,10,5,32,74,39,20,35,69,113,99,201,230,223,230,241,13,14,10,239,211,192,207,223,4,27,22,240,243,248,8,23,18,226,217,220,227,214,201,200,178,151,77,66,80,104,144,200,241,45,40,52,93,116,119,123,94,103,140,180,216,235,249,3,248,12,20,12,11,10,2,12,4,10,26,10,15,4,243,237,223,191,162,110,76,90,66,57,100,157,173,184,222,245,25,7,5,9,11,12,2,252,244,0,240,252,26,27,8,4,5,11,17,10,4,248,17,50,40,3,234,230,232,1,231,250,47,107,125,169,211,116,104,86,86,75,89,125,193,228,1,253,227,211,219,225,248,241,245,246,238,251,239,193,208,253,246,2,185,204,243,253,0,245,251,21,39,83,94,68,74,126,154,109,9,205,153,209,203,181,164,125,85,76,88,97,95,71,23,9,10,11,21,26,251,193,200,247,255,11,255,242,225,219,222,222,236,243,246,2,8,18,251,246,1,255,9,14,14,22,26,18,21,5,243,250,4,255,254,251,217,121,132,111,99,87,70,33,28,15,33,43,36,10,0,255,233,188,155,138,144,220,206,146,125,88,87,94,191,253,231,230,27,37,17,8,0,251,198,142,56,37,44,46,21,31,34,47,36,33,58,92,84,44,15,10,0,7,1,242,230,237,229,246,226,233,237,252,224,239,247,253,20,8,209,152,139,119,102,141,195,0,173,213,248,23,24,18,10,13,21,23,24,36,43,44,43,25,40,80,113,102,132,144,175,215,245,250,242,216,206,130,57,62,104,113,96,130,150,133,132,129,72,31,254,11,18,15,7,8,241,228,246,246,16,37,25,39,65,65,77,82,140,160,172,193,228,243,160,173,224,238,244,9,9,250,223,208,203,203,215,223,244,14,36,34,17,3,5,241,201,160,142,186,225,3,27,20,10,28,1,222,234,227,224,231,205,182,180,210,230,244,250,254,241,214,203,169,130,90,72,41,27,0,233,204,177,133,120,74,244,218,197,206,247,254,10,29,44,93,149,171,233,46,70,87,85,69,46,39,29,1,232,215,220,242,37,25,37,55,63,61,65,87,103,148,179,195,207,195,164,205,220,211,202,241,238,242,13,25,51,105,137,97,80,85,77,73,73,44,39,40,61,63,85,87,70,47,39,31,46,83,169,222,22,27,26,22,31,51,87,126,143,174,177,207,227,246,10,29,37,53,41,31,19,21,55,46,43,24,14,24,37,39,47,24,3,245,226,196,152,142,162,186,210,221,224,231,204,234,3,13,10,8,26,27,1,251,2,236,254,10,16,35,56,82,55,35,26,106,69,88,151,169,151,146,140,88,48,23,30,69,61,51,40,49,85,74,59,28,16,254,242,11,40,32,17,3,251,234,206,217,196,7,247,239,245,249,242,246,211,191,214,3,9,19,43,94,79,79,101,122,102,47,18,252,224,188,235,7,9,1,251,230,195,182,123,73,72,92,124,153,12,11,7,4,253,249,252,247,229,190,190,224,226,221,228,201,174,75,91,103,97,173,127,133,123,148,174,202,174,189,237,251,237,244,253,241,228,212,220,213,219,7,90,120,116,124,112,107,246,235,221,237,190,222,254,21,32,26,14,33,20,20,21,24,35,41,53,45,42,43,25,184,143,76,56,244,235,218,231,2,18,21,12,246,20,11,252,253,218,230,195,223,246,252,8,32,99,130,158,193,158,136,229,225,209,208,199,176,203,161,95,87,92,120,55,44,24,8,231,188,134,43,8,245,236,232,241,5,248,230,237,251,12,16,29,29,21,27,23,3,252,0,17,47,31,18,10,249,4,248,22,10,236,59,74,97,75,44,47,41,18,6,12,8,6,17,27,31,29,19,8,22,44,234,5,45,94,104,62,59,54,55,91,63,7,13,20,18,20,23,48,51,76,51,30,31,27,52,83,59,52,23,14,48,38,17,250,205,234,29,100,86,68,59,41,32,20,11,8,82,103,11,247,243,244,247,6,17,27,47,50,35,30,24,43,77,139,173,167,150,162,204,190,207,237,254,252,253,36,47,75,90,67,28,13,188,130,32,12,18,9,106,90,61,65,90,92,99,141,186,165,155,196,230,224,215,253,217,209,206,201,231,255,10,40,47,19,245,207,172,177,77,60,89,181,190,224,248,255,249,244,238,240,151,175,183,220,65,135,187,209,253,255,240,23,26,45,39,10,120,146,117,227,177,218,226,230,209,207,217,197,139,115,76,58,101,111,70,34,29,27,19,4,6,20,14,12,234,197,187,202,182,161,200,237,227,207,178,160,176,36,13,7,24,46,38,55,78,95,56,26,3,8,22,49,57,254,226,160,162,174,163,180,145,171,124,66,54,124,84,70,55,44,40,46,249,215,229,231,146,127,147,181,190,151,88,27,4,234,249,231,223,224,224,243,254,241,132,133,57,17,6,15,13,2,247,249,226,208,4,40,45,55,17,1,217,246,12,1,2,245,237,230,245,6,1,243,243,246,45,138,176,228,251,21,92,48,44,31,46,28,6,21,15,24,40,84,115,118,68,116,108,83,61,55,47,38,61,64,46,54,72,62,83,140,127,123,54,6,203,155,133,128,165,182,206,178,134,101,101,96,16,22,36,32,11,247,67,66,39,44,24,31,30,30,15,252,223,171,59,44,170,179,174,163,187,218,253,206,215,220,207,185,132,112,18,222,224,241,184,171,195,207,226,216,212,10,16,28,25,27,51,75,116,128,147,192,170,192,184,150,104,126,92,87,132,117,146,222,204,177,159,168,219,237,232,91,236,217,160,109,66,95,156,186,208,212,226,43,97,164,155,157,118,25,245,1,250,235,223,216,253,7,20,49,70,83,93,109,222,106,74,26,19,23,22,5,5,12,242,252,46,45,35,17,33,22,37,22,46,70,67,62,68,87,180,211,227,229,217,255,197,213,219,3,253,248,8,240,196,131,133,154,236,226,174,175,208,233,218,188,144,147,58,36,5,3,16,26,7,4,237,221,5,49,43,19,62,59,69,19,0,236,230,248,246,26,33,22,0,13,19,140,122,117,69,24,255,235,206,128,94,77,63,69,66,79,107,69,68,3,233,241,228,199,217,223,208,239,251,7,3,9,15,9,247,244,0,9,247,186,157,101,121,28,24,24,24,98,138,186,223,219,197,182,172,217,183,166,217,145,216,134,169,199,201,162,91,61,26,25,23,31,41,34,31,11,0,212,205,216,218,223,247,253,251,218,216,200,143,121,129,84,54,23,13,249,37,35,39,35,134,158,229,255,53,76,63,132,155,170,245,255,10,1,250,71,76,55,43,49,55,45,82,126,132,95,89,227,244,250,223,218,203,190,40,22,19,18,33,38,41,108,116,75,24,255,251,246,233,14,241,250,247,3,29,54,51,18,13,37,28,3,241,251,16,252,234,10,41,25,17,29,19,223,10,26,252,254,66,84,59,100,113,242,220,203,205,222,52,57,90,73,98,109,60,30,8,135,140,155,48,21,21,51,244,9,18,21,42,52,70,98,101,66,90,68,33,10,4,15,25,26,25,55,205,233,228,215,226,14,252,221,235,26,25,16,254,34,89,245,240,255,236,233,194,222,247,24,33,47,44,37,44,37,21,8,34,76,62,39,95,67,74,88,74,35,19,57,88,120,144,173,212,238,9,18,27,63,38,32,9,6,15,29,30,45,150,187,177,172,138,118,48,58,55,158,174,156,92,32,17,24,48,24,29,35,31,22,252,170,138,3,11,245,2,30,8,0,13,39,44,43,43,53,40,235,246,251,244,21,49,101,130,177,113,70,96,135,148,159,0,9,12,15,14,5,247,239,239,241,3,36,24,29,107,109,152,183,191,178,108,56,93,47,28,32,40,40,83,142,224,6,28,32,13,239,255,255,253,12,8,12,26,28,52,146,135,143,85,50,18,246,0,2,244,19,36,74,127,109,103,90,81,68,72,85,166,132,81,39,47,45,35,33,31,225,221,205,183,167,129,11,113,183,208,222,7,240,223,227,252,227,11,10,3,238,249,11,242,137,190,159,12,0,247,239,254,254,195,92,42,204,211,206,219,215,199,156,127,30,22,250,162,124,219,34,27,11,10,242,205,229,42,46,42,5,22,52,52,49,56,47,253,234,204,183,148,183,178,126,141,85,9,251,242,247,243,12,12,32,62,77,53,30,20,40,41,36,25,19,15,21,19,251,9,208,240,221,2,3,3,30,63,85,107,114,113,134,112,66,70,113,141,171,215,210,248,214,103,78,75,58,19,38,4,250,232,197,156,180,57,46,22,0,248,251,229,244,248,242,230,239,241,213,163,152,145,155,203,206,193,0,158,235,1,7,7,12,22,245,244,0,254,162,156,188,223,254,11,251,243,239,227,229,213,136,168,120,96,109,165,217,222,244,187,154,118,138,200,203,249,32,24,17,19,21,5,251,5,3,252,2,6,9,1,206,240,12,255,243,250,106,178,208,160,147,195,170,166,162,142,133,114,154,116,138,189,152,119,94,193,179,127,107,168,130,32,31,30,28,37,38,37,48,108,126,100,117,70,33,29,47,40,54,41,32,252,2,245,5,234,225,208,237,6,2,14,11,22,19,10,255,17,22,30,212,207,228,61,160,219,252,251,201,201,170,241,249,10,114,60,47,91,130,85,49,39,36,112,76,30,22,0,36,3,4,254,33,23,12,254,251,14,43,22,44,17,253,252,243,243,7,1,200,189,211,9,45,86,73,10,252,8,16,7,0,17,15,15,18,24,41,41,41,30,11,245,92,72,73,158,125,106,91,127,118,58,117,109,98,122,149,118,92,86,72,45,58,72,86,52,40,62,58,26,102,151,182,147,77,14,2,11,27,3,161,128,156,178,130,33,53,54,63,23,169,97,72,70,230,236,255,3,254,40,48,92,5,254,1,19,13,11,255,253,6,233,20,92,119,61,14,252,2,71,121,137,201,228,242,214,227,2,45,24,26,9,254,218,217,207,183,100,63,44,59,100,201,243,238,237,244,253,62,94,196,209,192,44,183,210,210,192,109,17,21,16,117,98,107,145,66,82,63,162,243,8,2,241,19,11,20,7,9,16,18,11,26,253,10,4,247,229,244,238,234,158,78,65,98,79,69,40,47,184,172,183,142,112,231,3,255,12,8,4,255,243,227,226,251,14,200,224,53,36,249,12,7,45,7,9,225,230,252,45,60,81,90,65,228,238,252,45,146,232,246,2,38,72,108,65,77,58,56,68,63,83,234,246,4,17,26,226,213,195,213,221,254,222,224,23,47,71,163,243,89,141,103,31,20,13,6,12,10,255,242,85,106,117,67,136,176,204,143,82,137,68,54,54,99,122,102,13,243,236,240,17,16,39,99,18,250,242,7,34,30,205,204,219,213,222,230,244,12,24,23,5,219,16,253,247,244,13,10,23,49,34,225,228,232,17,23,105,149,147,126,108,87,176,1,31,21,12,17,245,51,1,253,241,235,18,89,139,143,126,250,226,177,246,128,81,44,26,19,12,253,216,152,39,68,61,65,35,8,51,30,24,34,8,129,41,26,253,19,14,18,249,221,211,230,3,243,239,225,227,251,161,180,254,248,255,254,7,222,227,172,122,98,136,101,99,155,223,8,27,12,247,248,37,23,13,30,43,38,67,56,8,37,73,62,70,114,145,120,129,153,209,254,2,248,251,82,207,161,75,57,189,188,146,168,139,96,60,255,11,25,22,10,3,1,224,198,59,38,12,28,46,67,128,90,60,117,227,68,130,179,214,247,250,0,18,13,216,8,248,21,30,17,9,74,1,211,201,159,189,146,219,249,237,195,207,99,58,40,1,237,255,251,243,178,147,120,94,98,21,206,6,19,239,242,21,130,173,227,20,47,64,131,125,31,32,56,53,40,42,195,31,1,31,61,78,78,66,67,94,117,186,203,187,14,231,215,209,225,218,209,236,10,24,40,81,143,84,87,54,76,60,73,68,20,25,92,82,106,117,36,26,16,31,56,206,33,33,95,95,125,157,218,236,10,10,19,22,59,44,33,20,7,37,65,26,13,251,4,45,47,47,52,255,192,160,129,118,145,194,184,194,223,154,208,228,23,14,253,240,208,122,29,230,25,7,4,19,15,40,65,175,198,161,135,113,111,57,52,47,98,68,33,38,29,119,127,112,245,234,217,243,17,44,241,207,222,39,3,10,4,223,244,253,0,244,230,251,186,169,184,7,18,7,67,116,136,20,17,14,172,124,135,249,9,13,5,242,238,70,42,53,94,19,17,11,2,0,1,8,6,18,254,143,145,2,239,192,202,170,104,76,215,130,22,231,242,249,1,14,219,5,32,29,27,15,33,63,201,179,252,203,200,17,16,29,15,17,44,239,36,169,217,147,4,33,142,100,105,124,102,8,189,95,117,147,20,43,50,36,14,172,136,162,153,90,22,225,223,34,237,221,209,218,21,56,39,25,12,27,43,250,253,204,111,53,9,255,234,246,35,228,183,27,23,35,52,249,21,38,24,7,248,12,40,79,121,135,70,73,65,153,241,5,20,3,21,25,5,0,26,28,130,124,76,43,46,33,15,39,40,46,6,253,3,193,247,223,241,216,250,252,231,67,44,19,24,18,50,177,33,8,106,164,250,8,6,233,239,47,26,27,128,12,241,13,251,240,135,120,81,24,70,103,86,109,100,144,193,246,179,192,203,182,153,18,9,9,18,35,76,47,225,241,40,0,13,15,254,178,188,199,230,226,229,235,90,12,18,33,26,7,12,19,234,205,189,171,189,244,215,168,241,245,16,47,138,81,90,104,3,12,232,172,199,170,147,79,111,219,52,70,72,50,61,81,240,205,206,2,106,178,168,207,92,58,21,8,13,18,31,15,189,155,46,71,64,14,33,77,6,19,239,224,239,255,138,177,119,247,152,17,29,150,58,42,59,41,74,58,37,35,26,246,157,3,216,227,246,88,136,59,55,107,97,71,20,61,33,29,234,207,26,160,177,172,139,49,51,132,234,89,108,33,202,201,9,26,14,9,35,124,118,24,225,236,244,36,55,52,140,47,13,13,229,248,255,250,13,36,26,17,236,200,250,56,3,2,31,60,21,73,72,82,164,254,238,199,34,29,90,102,66,81,238,248,177,161,60,15,248,238,247,24,161,200,52,211,12,31,80,57,154,248,10,6,32,21,3,235,229,31,132,9,9,98,102,248,150,160,73,6,201,224,41,63,67,15,52,27,197,200,201,142,36,21,57,76,82,71,2,249,18,229,51,128,119,4,254,251,19,29,13,42,236,16,236,66,15,178,247,211,91,115,111,77,57,10,134,236,34,40,220,230,1,63,97,206,2,214,235,213,251,4,46,44,50,49,30,23,221,0,14,63,97,181,21,82,113,134,144,255,6,8,24,23,20,18,209,169,80,98,116,180,170,153,70,45,14,15,22,31,41,5,244,54,47,45,52,254,253,241,24,142,27,117,137,151,11,39,17,9,14,177,200,200,43,33,20,37,24,11,23,35,244,13,46,9,14,241,244,8,32,51,46,234,16,23,30,96,17,60,127,30,16,48,45,54,63,65,190,180,152,207,229,219,233,222,136,237,249,8,116,181,185,36,28,17,40,74,81,10,235,163,217,43,49,32,17,4,250,37,11,26,242,8,244,39,103,132,143,150,136,236,54,31,219,186,114,105,69,34,7,251,6,213,204,185,225,165,200,216,11,8,18,248,92,181,9,16,243,231,248,154,185,123,74,242,11,8,237,141,161,121,221,172,143,13,24,23,2,30,31,55,77,31,253,17,10,167,207,140,38,242,224,243,246,15,193,120,161,17,19,121,84,218,2,6,225,125,41,13,23,7,229,23,1,213,245,244,212,233,216,133,113,64,39,46,221,38,81,66,62,253,16,192,226,2,51,52,95,29,214,185,86,221,42,89,5,38,46,68,37,44,43,59,248,237,222,171,63,59,52,35,4,1,224,216,168,25,33,34,29,32,221,40,38,75,220,26,74,181,105,32,7,245,247,142,59,110,198,49,162,56,23,51,17,17,194,151,224,21,180,168,69,62,109,176,222,64,93,121,73,177,118,255,253,48,61,56,9,185,41,9,254,196,43,42,136,33,41,33,216,194,169,29,14,0,32,32,30,65,128,120,144,101,39,241,127,15,64,240,248,124,40,119,226,2,118,173,100,41,39,166,15,21,37,9,60,24,239,166,139,7,235,142,88,39,};

EX namespace windmap {

  #if HDR
  static const int NOWINDBELOW = 8;
  static const int NOWINDFROM = 120;  
  #endif

  map<int, int> getid;
  vector<vector<int> > neighbors;
  vector<cellwalker> samples;

  EX int getId(cell *c) {
    auto i = fieldpattern::fieldval_uniq(c);
    return getid[i]-1;
    }
  
  int getId(cellwalker cw) {
    auto i = fieldpattern::fieldval_uniq(cw.at);
    auto &id = getid[i];
    if(id == 0) { samples.push_back(cw); id = isize(samples); }
    return id-1;
    }
  
  EX vector<unsigned char> windcodes;

  void wcheck(cell *a, cell *b) {
    int i = getId(a);
    if(b) {
      int j = getId(b);
      for(auto nn: neighbors[i]) if(nn == j) return;
      printf("neigbor not found!\n");
      }
    else {
      printf("%d", windcodes[i]);
      for(auto nn: neighbors[i]) printf("n%d ", windcodes[nn]);
      printf("\n");
      }
    }

  EX void create() {
    if(disable_bigstuff) return;
    if(cgflags & qPORTALSPACE) return;
    if(hat::in()) return;
    samples.clear();
    neighbors.clear();
    getid.clear();
    getId(cellwalker(currentmap->gamestart(), 0));
    for(int k=0; k<isize(samples); k++) {
      cellwalker cw = samples[k];
      // cw.spin = 0;
      neighbors.emplace_back();
      auto &v = neighbors.back();
      if(NONSTDVAR && !sphere && !arcm::in() && !mhybrid && !INVERSE)
        for(int l=0; l<S7; l++) {
          v.push_back(getId(cw + cth + l + wstep + cth));
          }
      else
        for(int l=0; l<cw.at->type; l++) v.push_back(getId(cw+l+wstep));
      }
    
    int N = isize(samples);
    
    int* precomp = NULL;

    windcodes.resize(N);
    
    if(N == 18920) precomp = windcodes18920;
    if(N == 5676) precomp = windcodes5676;
    
    if(precomp && (hyperbolic || mhybrid) && isize(currfp.matrices)) {
      int randval = hrand(isize(currfp.matrices));
      for(int i=0; i<N; i++)
        windcodes[i] = precomp[getid[fieldpattern::fieldval_uniq_rand(samples[i].at, randval)]-1];
      return;
      }
    
    int tries = 0;
    int maxtries = specialland == laVolcano || specialland == laBlizzard || ls::any_chaos() ? 20 : 1;
    tryagain:

    for(int i=0; i<N; i++) windcodes[i] = hrand(256);
    
    vector<bool> inqueue(N, true);
    vector<int> tocheck;
    for(int i=0; i<N; i++) tocheck.push_back(i);
    hrandom_shuffle(tocheck);
    
    for(int a=0; a<isize(tocheck); a++) {
      if(a >= 200*N) { printf("does not converge\n"); break; }
      int bestval = 1000000000, best = 0;
      int i = tocheck[a];
      for(int k=0; k<256; k++) {
        int j = windcodes[i] + k;
        int cval = 0;
        for(int c2: neighbors[i]) {
          int d = (j - windcodes[c2]) & 255;
          if(d > 128) d = 256 - d;
          cval += d*d;
          }
        if(cval < bestval) bestval = cval, best = j;
        }
      if(windcodes[i] != best) 
        for(int c2: neighbors[i]) 
          if(!inqueue[c2])
            inqueue[c2] = true, tocheck.push_back(c2);
      inqueue[i] = false;
      windcodes[i] = best;
      }
    
    int ingroup[4];
    for(int u=0; u<4; u++) ingroup[u] = 0;
    for(int i=0; i<N; i++) ingroup[windcodes[i] >> 6]++;
    for(int u=0; u<4; u++) if(!ingroup[u]) {
      tries++;
      if(tries < maxtries) goto tryagain;
      }
    if(tries >= maxtries && maxtries >= 20) {
      addMessage("Failed to generate an interesting wind/lava pattern.");
      }
    else if(false) {
      printf("tocheck size = %d\n", isize(tocheck));
      printf("if(N == %d) {\n", N);
      printf("  windcodes = {");
      for(int i=0; i<N; i++) printf("%d,", windcodes[i]);
      printf("};\n");
      printf("  return;\n");
      printf("  }\n");
      }
    }
  
  EX int at(cell *c) {
    return windmap::windcodes[windmap::getId(c)];
    }

EX }
#endif

// Halloween namespace

EX namespace halloween {
  EX cell *dragoncells[4];
  vector<cell*> srch;

  EX void generate() {
    auto lst = currentmap->allcells();
    for(cell *c: lst)
      setdist(c, 7, nullptr);

    halloween::dragoncells[0] = NULL;

    if(sphere && geometry == gSphere) {
      for(cell *c: lst) {
        if(GOLDBERG) {
          int fv = c->master->fiftyval;
          if(fv == 1 || fv == 4 || fv == 10)
            c->wall = waChasm;
          if(c == c->master->c7 && fv == 3)
            c->item = itTreat;
          }
        else if(!BITRUNCATED) {
          int fv = c->master->fiftyval;
          if(fv == 1 || fv == 4 || fv == 2)
            c->wall = waChasm;
          if(fv == 3) c->item = itTreat;
          }
        else {
          if(c->type == 5) {
            int fv = c->master->fiftyval;
            if(fv == 3 || fv == 4 || fv == 2 || fv == 5)
              c->wall = waChasm;
            if(fv == 2) halloween::dragoncells[0] = c;
            if(fv == 5) halloween::dragoncells[3] = c;
            if(fv == 1) c->item = itTreat;
            }
          if(c->type == 6) {
            int fvset = 0;
            for(int i=0; i<6; i+=2) fvset |= 1 << createMov(c, i)->master->fiftyval;
            if(fvset == 35 || fvset == 7) c->wall = waChasm;
            if(fvset == 7) halloween::dragoncells[1] = c;
            if(fvset == 35) halloween::dragoncells[2] = c;
            }
          }
        }
      }
    else {
      for(int i=1; i<isize(lst); i+=60) lst[i]->item = itTreat;
      for(int i=2; i<isize(lst); i+=6) lst[i]->wall = waChasm;
      }
    }

  cell *farempty(bool lastresort = false) {
    int maxdist = 0;
    vector<cell*> validcells;
    int firstfar1 = 0;
    for(int i=1; i<isize(dcal); i++) {
      bool okay = 
        dcal[i]->item == itNone && dcal[i]->monst == moNone && dcal[i]->wall == waNone;
      if(lastresort)
        okay = dcal[i]->wall != waChasm && !isMultitile(dcal[i]->monst);
      if(okay) {
        if(dcal[i]->cpdist > maxdist) 
          firstfar1 = isize(validcells),
          maxdist = dcal[i]->cpdist; 
        validcells.push_back(dcal[i]);
        }
      }
    
    int qvc = isize(validcells);
    if(qvc == firstfar1) return farempty(true);
    
    return validcells[firstfar1 + hrand(qvc - firstfar1)];
    }
  
  int demoncount;
  int swordpower;
  int dragoncount;

  void reset() {
    demoncount = 0;
    swordpower = 0;
    dragoncount = 0;
    }
  
  eMonster nextDemon() {
    demoncount++;
    if(demoncount % 9 == 0) return moGreater;
    return moLesser;
    }
  
  void putMonster(eMonster m) {
    cell *c = farempty();
    changes.ccell(c);
    c->hitpoints = 3;
    c->monst = m;
    playSeenSound(c);
    if(!kills[m]) switch(m) {
      case moGhost:
        addMessage(XLAT("Ghosts can move through chasms!"));
        break;
      case moSkeleton:
        addMessage(XLAT("Push Skeletons into the holes!"));
        break;
      case moDraugr:
        addMessage(XLAT("You'll need your magical sword against the Draugar!"));
        break;
      case moLesser:
        addMessage(XLAT("Demons are slow, but you'll need the experience against stronger ones..."));
        break;
      case moGreater:
        addMessage(XLAT("You will need more experience to defeat the Greater Demon!"));
        break;
      case moPyroCultist:
        addMessage(XLAT("Cultists throw fire at you from distance!"));
        break;
      case moFlailer:
        addMessage(XLAT("Defeat Flail Guards by moving away from them."));
        break;
      case moVampire:
        addMessage(XLAT("Vampire Bats drain your magical powers!"));
        break;
      default: break;
      }
    c->stuntime = 2;
    }
  
  EX void getTreat(cell *where) {
    if(!items[itTreat]) reset();
    gainItem(itTreat);
    changes.at_commit(after_treat);
    }
  
  EX void after_treat() {
    farempty()->item = itTreat;
    int itr = items[itTreat];
    items[itOrbTime] += 30;
    changes.value_keep(dragoncount);
    changes.value_keep(demoncount);
    changes.value_keep(swordpower);

    int monpower = 19 + itr + hrand(itr);
    int mcount = 0;
    while(monpower > 0) {
      int id = hrand(10000);
    
#define CHANCE(x) (id -= x, id < 0)
      if(CHANCE(400) && itr >= 5) {
        putMonster(moGhost);
        monpower -= 30;
        mcount++;
        }
      else if(CHANCE(400)) {
        putMonster(pick(moWitch, moZombie));
        monpower -= 20;
        mcount++;
        }
      else if(CHANCE(400) && itr >= 5) {
        putMonster(nextDemon());
        monpower -= 10;
        mcount++;
        }
      else if(CHANCE(100) && itr >= 12) {
        putMonster(moVampire);
        monpower -= 10;
        swordpower -= 5;
        if(swordpower < 0) swordpower = 0;
        mcount++;
        }
      else if(CHANCE(400) && swordpower > 0 && !mcount && itr >= 15) {
        putMonster(moDraugr);
        swordpower -= 3;
        monpower -= 100;
        mcount++;
        }
      else if(CHANCE(400) && itr >= 10 && !mcount && itr >= 10) {
        putMonster(moSkeleton);
        monpower -= 60;
        mcount++;
        }
      else if(CHANCE(10) && itr >= 15) {
        putMonster(moWitchFire);
        monpower -= 35;
        mcount++;
        }
      else if(CHANCE(100) && itr >= 5) {
        putMonster(moFlailer);
        monpower -= 35;
        mcount++;
        }
      else if(CHANCE(100) && itr >= 5) {
        putMonster(moPyroCultist);
        monpower -= 35;
        mcount++;
        }        
      else if(CHANCE(5) && itr >= 20 && kills[moSkeleton]) {
        putMonster(moFatGuard);
        monpower -= 35;
        mcount++;
        }        
      else if(CHANCE(5) && itr >= 30 && kills[moFlailer]) {
        putMonster(moHedge);
        monpower -= 35;
        mcount++;
        }        
      else if(CHANCE(5) && itr >= 40 && kills[moHedge]) {
        putMonster(moLancer);
        monpower -= 60;
        mcount++;
        }        
      else if(CHANCE(1) && itr >= 50 && kills[moHedge]) {
        putMonster(moFireFairy);
        monpower -= 40;
        mcount++;
        }        
      else if(CHANCE(5) && itr >= 60) {
        putMonster(moBomberbird);
        monpower -= 25;
        mcount++;
        }        
      else if(CHANCE(5) && itr >= 60) {
        putMonster(moRatlingAvenger);
        monpower -= 30;
        mcount++;
        }        
      else if(CHANCE(5) && itr >= 60) {
        putMonster(moVineBeast);
        monpower -= 30;
        mcount++;
        }        
      else if(CHANCE(5) && itr >= 60) {
        dragoncount++;
        }
      else if(dragoncount && BITRUNCATED && geometry == gSphere && !mcount) {
        bool fill = false;
        for(int i=0; i<4; i++) 
          if(!dragoncells[i] || dragoncells[i]->monst)
            fill = true;
        swap(dragoncells[0], dragoncells[3]);
        swap(dragoncells[1], dragoncells[2]);
        if(fill) continue;
        for(int i=0; i<4; i++) {
          dragoncells[i]->monst = i ? moDragonTail : moDragonHead;
          dragoncells[i]->mondir = i==3 ? NODIR : neighborId(dragoncells[i], dragoncells[i+1]);
          dragoncells[i]->hitpoints = 1;
          dragoncells[i]->stuntime = 1;          
          playSeenSound(dragoncells[i]);
          }
        monpower -= 200;
        mcount++;
        dragoncount--;
        }        
      }
    int id = hrand(100);
    if(items[itTreat] == 1) {
#if !ISMOBILE
      addMessage(XLAT("Hint: use arrow keys to scroll."));
#endif
      }
    else if(items[itTreat] == 2) {
#if !ISMOBILE
      addMessage(XLAT("Hint: press 1 to change the projection."));
#endif
      }
    else if(items[itTreat] == 3) {
      items[itOrbShell] += 20;
      addMessage(XLAT("You gain a protective Shell!"));
      }                                         
    else if(items[itTreat] == 4) {
      items[itOrbShell] += 10;
      addMessage(XLAT("Hint: magical powers from Treats expire after a number of uses."));
      }
    else if(kills[moSkeleton] && CHANCE(10)) {
      addMessage(XLAT("A magical energy sword. Don't waste its power!"));
      items[itOrbSword] += 5; // todo facing
      swordpower += 5;
      }
    else if(kills[moDraugr] && items[itOrbSword] && CHANCE(10)) {
      addMessage(XLAT("Another energy sword!"));
      items[itOrbSword2] += 5;
      swordpower += 5;
      }
    else if(kills[moFlailer] && CHANCE(10)) {
      items[itOrbThorns] += 5;
      addMessage(XLAT("You got Thorns! Stab monsters by moving around them."));
      }
    else if(kills[moGhost] && CHANCE(10)) {
      items[itOrbAether] += 5;
      addMessage(XLAT("Aethereal powers let you go through the holes."));
      }
    else { 
      if(items[itOrbShell] > ORBBASE)
        addMessage(XLAT("The tasty treat increases your protection."));
      else 
        addMessage(XLAT("You gain your protective Shell back!"));
      items[itOrbShell] += 5;
      }
    }

  EX void start_all() {
      popScreenAll();
      resetModes('g');
      stampbase = ticks;
      if(!sphere) {
        stop_game();
        specialland = laHalloween;
        set_geometry(gSphere);
        start_game();
        pconf.alpha = 999;
        pconf.scale = 998;
        }
      }

EX }

// ... also includes the Ivory Tower

EX namespace dungeon {

  void towerError(cell *c) {
    // only care in the standard geometry -- weird ones are intentionally left buggy
    if(!weirdhyperbolic && !sphere && !quotient) 
      raiseBuggyGeneration(c, "ivory tower/dungeon generation error");
    }

  void buildIvoryTower(cell *c) {
    /* if(int(c->landparam) % 5 == 0) 
      c->wall = waCamelot;
      */
    
    if(euclid) {
      if(quotient) return;
      auto co = euc2_coordinates(c);
      int x = co.first, y = co.second;
      string tab6[] = {
        ".####...",
        "L...L...",
        ".L..L...",
        "L...L...",
        "........",
        "........"
        };
      string tab4[] = {
        "L####L..",
        "L....L..",
        "L....L..",
        "L....L..",
        "........",
        "........"
        };
      int y0 = y; if(y>32768) y0 -= 65536;
      
      y0 += 5; y0 %= 12; if(y0<0) y0+=12;
      
      if(y0 >= 6) { y0 -= 6; x += 4; }
      
      char ch = euc::in(2,6) ? tab6[y0][(x+(y+1)/2)&7] : tab4[y0][x&7];
      
      if(ch == '#')
        c->wall = waPlatform;
      else if(ch == 'L')
        c->wall = waLadder;
      }
    
    else if(WDIM == 3) {    
      int cnt = 0;
      int below = 0;
      manual_celllister cl;
      cl.add(c);
      for(int i=0; i<isize(cl.lst); i++) {
        cell *c1 = cl.lst[i];
        generate_around(c1);
        if(coastvalEdge(c1) == coastvalEdge(c) - 3) {
          if(c1->landflags == 3) cnt++;
          continue;
          }
        if(c1->landflags == 3) below++;
        forCellEx(c2, c1) if(coastvalEdge(c2) < coastvalEdge(c1))
          cl.add(c2);
        }
      if(cnt) c->wall = waPlatform;
      else if(below && coastvalEdge(c) < 3) c->wall = waPlatform;
      }
    
    else if(true) {
      
      cell *c2 = c;
      cell *c3 = c;
      
      bool rdepths[5];
      for(int i=0; i<5; i++) {
        if(coastvalEdge(c2) == 0) { 
          rdepths[i] = false;
          }
        else {
          cell *c4 = c2;
          if(c2 != c3 && !isNeighbor(c2, c3)) {
            for(int i=0; i<c2->type; i++) if(c2->move(i) && isNeighbor(c2->move(i), c3))
              c4 = c2->move(i);
            }
          rdepths[i] = c2 && c3 && c4 && (c2->landflags == 3 || c3->landflags == 3 || c4->landflags == 3);
          if(c2) generate_around(c2);
          if(c3) generate_around(c3);
          c2 = ts::left_parent(c2, coastvalEdge);
          c3 = ts::right_parent(c3, coastvalEdge);
          if(!c2) { towerError(c); return; }
          if(!c3) { towerError(c); return; }
          }
        }
      
      if(rdepths[3]) {
        c->wall = waPlatform;
  //        if(!c4->item) c4->item = itPalace;
        }
      else if(!rdepths[2] && !rdepths[4] && !rdepths[1]) {
        c2 = c;
        c3 = c;
        generate_around(c);
        cell *c4 = ts::left_of(c, coastvalEdge);
        cell *c5 = ts::right_of(c, coastvalEdge);
        for(int i=0; i<3; i++) {
          if(coastvalEdge(c2) == 0) break;
          for(cell *cx: {c2, c3, c4, c5}) if(cx) generate_around(cx);

          if(c2 && c4 && c4->landflags == 3 && c2->landflags != 3 && c4 == ts::left_of(c2, coastvalEdge))
            c->wall = waLadder;
          if(c3 && c5 && c5->landflags == 3 && c3->landflags != 3 && c5 == ts::right_of(c3, coastvalEdge))
            c->wall = waLadder;
          buildEquidistant(c4); buildEquidistant(c5);
          if(c2) c2 = ts::left_parent(c2, coastvalEdge);
          if(c3) c3 = ts::right_parent(c3, coastvalEdge);
          if(c4) c4 = ts::left_parent(c4, coastvalEdge);
          if(c5) c5 = ts::right_parent(c5, coastvalEdge);
          }
        }
      }
    else c->wall = waCIsland;
    }
  
  int dungeonFlags(cell *c) {
    if(!c) return 0;
    buildEquidistant(c);
    bool rdepths[5];
    int switchcount = 0;
    
    if(WDIM == 3) {
      for(int i=0; i<5; i++) rdepths[i] = false;
      
      manual_celllister cl;
      cl.add(c);
      int d = coastvalEdge(c);

      for(int i=0; i<isize(cl.lst); i++) {
        cell *c1 = cl.lst[i];
        generate_around(c1);
        int d1 = d - coastvalEdge(c);
        if(c1->landflags == 3) rdepths[d1] = true;
        if(c1->landflags == 1) switchcount++;
        if(d1 == 4) break;
        forCellEx(c2, c1) if(coastvalEdge(c2) < coastvalEdge(c1))
          cl.add(c2);
        }
      }
    
    else {
      
      cell *c2 = c;
      cell *c3 = c;
        
      for(int i=0; i<5; i++) {
        if(coastvalEdge(c2) == 0) { 
          rdepths[i] = false;
          }
        else {
          cell *c4 = c2;
          if(c2 != c3 && !isNeighbor(c2, c3)) {
            for(int i=0; i<c2->type; i++) if(c2->move(i) && isNeighbor(c2->move(i), c3))
              c4 = c2->move(i);
            }
          rdepths[i] = c2 && c3 && c4 && (c2->landflags == 3 || c3->landflags == 3 || c4->landflags == 3);
          if((c2&&c2->landflags == 1) || (c3&&c3->landflags == 1) || (c4&&c4->landflags == 1))
            switchcount++;
          generate_around(c2);
          generate_around(c3);
          c2 = ts::left_parent(c2, coastvalEdge);
          c3 = ts::right_parent(c3, coastvalEdge);
          if(!c2) { towerError(c); return 0; }
          if(!c3) { towerError(c); return 0; }
          }
        }
      }
    
    int res = 0;
    
    if(rdepths[3]) res |= 1;
    if(rdepths[2]) res |= 2;
    if(switchcount&1) res |= 4;
    
    return res;
    }
  
  void placeGate(cell *c, eWall w) {
    if(w == waOpenGate) {
      c->wall = waClosedGate;
      toggleGates(c, waOpenPlate, 0);
      }
    if(w == waClosedGate) {
      c->wall = waOpenGate;
      toggleGates(c, waClosePlate, 0);
      }
    }
  
  bool isGate(eWall w) {
    return w == waOpenGate || w == waClosedGate;
    }
  
  void placeRandomGate(cell *c) {
    placeGate(c, hrand(2) ? waOpenGate : waClosedGate);
    }
  
  cell *random_child(cell *c, const cellfunction& cf) {
    generate_around(c);
    vector<cell*> children;
    forCellEx(c2, c) if(cf(c2) > cf(c)) children.push_back(c2);
    if(!isize(children)) return NULL;
    return children[hrand(isize(children))];
    }
  
  void build(cell *c) {
    /* if(int(c->landparam) % 5 == 0) 
      c->wall = waCamelot;
      */
    
    if(true) {
      
      if(coastvalEdge(c) == 1) forCellEx(c2, c)
        if(c2->land != laBarrier && c2->land != laDungeon) {
          c->wall = waLadder;
          c->wparam = 3;
          }
        
      int df = dungeonFlags(c);
      
      if(df&1) {
        generate_around(c);
        int df1 = WDIM == 3 ? 0 : dungeonFlags(ts::left_of(c, coastvalEdge));
        int df2 = WDIM == 3 ? 0 : dungeonFlags(ts::right_of(c, coastvalEdge));
        
        c->wparam = 0;
        if(hrand(100) < (c->landparam % 5 == 0 ? 80 : 20)) {
          if(!(df1&1)) c->wparam = 1;
          if(!(df2&1)) c->wparam = 2;
          }
  
        if(df&4) 
          placeRandomGate(c);
        else if(c->wparam == 0 && c->landparam % 5 == 0 &&  hrand(100) < 10) {
          c->wall = waLadder;
          c->wparam = 3 + hrand(2);
          }
        else 
          c->wall = waPlatform;
        }
  
      if(c->wparam) {
        cell *c2 = 
          WDIM == 3 ? random_child(c, coastvalEdge) :
          c->wparam == 1 ? ts::add(c, 1, 2, coastvalEdge) :
          c->wparam == 2 ? ts::add(c, -1, -2, coastvalEdge) :
          c->wparam == 3 ? ts::add(c, 1, 3, coastvalEdge) :
          c->wparam == 4 ? ts::add(c, -1, -3, coastvalEdge) :
          NULL;
        
        if(c2) {
          c2->wall = c->wall, c2->wparam = c->wparam;
          if(c2->wall == waPlatform && hrand(10) < 2) 
            placeRandomGate(c2);
          if(isGate(c2->wall) && hrand(10) < 2) 
            c2->wall = waPlatform;
          }
        }
      }
    else c->wall = waCIsland;
    }
  
  void buildPlates(cell *c) {
    if(c->wall) return;
    int neargate = 0;
    int neargateDown = 0;
    int neargateEq = 0;
    int qup = 0;
    forCellEx(c2, c) {
      int d = coastvalEdge(c2) - coastvalEdge(c);
      if(isGate(c2->wall)) {
        neargate++;
        if(d>0) neargateDown++;
        if(d==0) neargateEq = 0;
        }
      if(d<0) qup++;
      }
    
    if(!neargate) return;
    
    int hr = 99;
    
    if(neargate == neargateDown && qup == 1)
      hr = hrand(12);
    else if((zebra40(c) >= 40) && !(neargateEq && neargateDown)) 
      hr = hrand(36);
    else if(zebra40(c) >= 40)
      hr = hrand(5000);
      
    if(hr < 5) c->wall = waClosePlate;
    else if(hr < 10) c->wall = waOpenPlate;
    }

  bool is02(int i) { return i == 0 || i == 2; }
  
  EX void all(cell *c, int d) {
    if(d == 8 && (c->land == laIvoryTower || c->land == laDungeon) && !euclid) {

      if(hrand(1000) < 75 && (WDIM == 3 || (c->landparam & 1))) {
        c->landflags = 3;
        }
      else c->landflags = 0;
      }
    
    if(d == 8 && c->land == laDungeon && !euclid) {
      if(hrand(1000) < 240 && is02(c->landparam%5) ) {
        c->landflags = 3;
        }
      else if(hrand(1000) < 90)
        c->landflags = 1;
      else c->landflags = 0;
      }

    if(d == 7 && c->land == laIvoryTower) buildIvoryTower(c);
    if(d == (BARLEV == 8 ? 7 : 8) && c->land == laDungeon) build(c);
    if(d == 7 && c->land == laDungeon) buildPlates(c);
    }
EX }

}
