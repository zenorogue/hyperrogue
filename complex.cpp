// Hyperbolic Rogue

// namespaces for complex features (whirlwind, whirlpool, elec, princess, clearing, 
// mirror, hive, heat + livecaves, etc.)

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

#include <map>

namespace whirlwind {

  int fzebra3(cell *c) {
    if(euclid) {
      if(torus) return 0;
      int x, y;
      tie(x,y) = cell_to_pair(c);
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

  void switchTreasure(cell *c) {
    c->item = itNone;
    if(safety) return;
    if(hrand(5000) < PT(100 + 2 * (kills[moAirElemental] + kills[moWindCrow]), 200) && notDippingFor(itWindstone)
      && getGhostcount() < 2)
      c->item = itWindstone;
    else if(hrand(5000) < 20*PRIZEMUL)
      placeLocalOrbs(c);
    }

  int cat(cell *c) {
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
  int dfrom[2], dto[2], qdirs;
  
  int gdist(int d, int e) { return dirdiff(d-e, where->type); }
  
  void calcdirs(cell *c) {
    where = c;
    int d = cat(c);
    qdirs = 0;
    if(d == 0) return;
    int qdf = 0, qdt = 0;
    int cats[MAX_EDGE];
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

  int winddir(int d) {
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
    cell *at = whirlline[size(whirlline)-1];
    cell *prev = whirlline[size(whirlline)-2];
    for(int i=0; i<at->type; i++) 
      if(at->mov[i] && (euclid || at->mov[i]->master->alt) && celldistAlt(at->mov[i]) == d && at->mov[i] != prev) {
        whirlline.push_back(at->mov[i]);
        goto again;
        }
    }
  
  void moveAt(cell *c) {
    if(eq(c->aitmp, sval)) return;
    calcdirs(c);
    if(qdirs != 1) return;
    vector<cell*> whirlline;
    whirlline.push_back(c);
    cell *prev = c;
    cell *c2 = c->mov[dfrom[0]];
    while(true) {
      // printf("c = %p dist = %d\n", c2, c2->mpdist);
      if(c == c2) break;
      calcdirs(c2);
      if(qdirs == 0) break;
      cell *cc2 = c2;
      if(qdirs == 1) whirlline.push_back(c2), c2 = c2->mov[dfrom[0]];
      else if(c2->mov[dto[0]] == prev) 
        c2 = c2->mov[dfrom[1]];
      else
        c2 = c2->mov[dfrom[0]];
      prev = cc2;
      }
    int z = size(whirlline);
    // printf("Cycle built from %p, length = %d\n", c, z);
    for(int i=0; i<z; i++) {
      // printf("%d%c", whirlline[i]->mpdist, whirlline[i]->item ? '*' : ' ');
      whirlline[i]->aitmp = sval;
      if(whirlline[i]->mpdist == BARLEV)
        switchTreasure(whirlline[i]);
      }
    for(int i=0; i<z-1; i++) {
      moveItem(whirlline[i], whirlline[i+1], true);
      if(whirlline[i]->item)
        animateMovement(whirlline[i+1], whirlline[i], LAYER_BOAT);
      }
    for(int i=0; i<z; i++) 
      pickupMovedItems(whirlline[i]);
    }
  
  void move() {
    sval++;
    for(int i=0; i<size(dcal); i++) {
      cell *c = dcal[i];
      moveAt(c);
      }
    // Keys and Orbs of Yendor always move
    using namespace yendor;
    for(int i=0; i<size(yi); i++) {
      moveAt(yi[i].path[0]);
      moveAt(yi[i].path[YDIST-1]);
      }
    }
  
  cell *jumpFromWhereTo(cell *c, bool player) {
    for(int i=0; i<2; i++) {
      calcdirs(c);
      if(qdirs != 1) return NULL;
      cell *c2 = c->mov[dfrom[0]];
      if(!passable(c, c2, P_JUMP1)) return NULL;
      if(player && i == 0 && !passable(c, c2, P_ISPLAYER)) return NULL;
      c = c2;
      }
    calcdirs(c);
    if(qdirs != 1) return NULL;
    return c;
    }

  cell *jumpDestination(cell *c) {
    for(int i=0; i<2; i++) {
      calcdirs(c);
      if(qdirs != 1) return NULL;
      c = c->mov[dto[0]];
      }
    calcdirs(c);
    if(qdirs != 1) return NULL;
    return c;
    }
  }

namespace elec { 

  bool havecharge, haveelec, havethunder;
  bool afterOrb; // extra charge from the Orb of Lightning

  enum eCharge {
    ecCharged, ecGrounded, ecIsolator, ecConductor
    };
  
  bool conduct(eCharge cf, eCharge ct) {
    if(ct == ecIsolator) return false;
    if(ct == ecConductor) return true;
    return ct != cf;
    }
  
  eCharge getCharge(cell *c) {
    bool ao = afterOrb && c->ligon;
    
    if(c->wall == waCharged) return ecCharged;
    if(c->wall == waSea || c->wall == waGrounded) return ecGrounded;
    if(c->wall == waSandstone || c->wall == waDeadTroll || 
      c->wall == waDeadTroll2 || 
      c->wall == waVinePlant ||
      c->wall == waMetal || isAlchAny(c)) 
        return c->land == laStorms ? ecConductor : ecGrounded; 
    if(c->wall == waBarrier)
      return ecIsolator;
    if(c->wall == waChasm)
      return ecIsolator;

    if(shmup::on ? isPlayerOn(c) : (isPlayerOn(c) || stalemate::isMoveto(c) || (items[itOrbEmpathy] && isFriendly(c)))) {
      if(items[itOrbShield]) return ecIsolator;
      if(afterOrb) return ecIsolator;
      if(!items[itOrbAether]) return c->land == laStorms ? ecConductor : ecGrounded;
      }
    
    // if(c->monst && stalemate::moveto) printf("%p: isKilled = %d\n", c, stalemate::isKilled(c));

    else if(
      (c->monst || stalemate::isPushto(c))
      && 
      (stalemate::isPushto(c) || !stalemate::isKilled(c)) 
      && 
      c->monst != moGhost && c->monst != moIvyDead && c->monst != moIvyNext 
      && !(isDragon(c->monst) && !c->hitpoints)
      )
      return c->land == laStorms ? (ao ? ecCharged : ecConductor) : ecGrounded;

    if(c->land != laStorms)
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
    return c->aitmp >= 0 && c->aitmp < size(charges) && charges[c->aitmp].c == c;
    }
  
  void connect(int from, cell *c) {
    if(hasdata(c)) {
      // seen again: set the lowlink
      if(!charges[c->aitmp].instack) return;
      // printf("edge %d-%d\n", from, c->aitmp);
      if(c->aitmp < charges[from].lowlink)
        charges[from].lowlink = c->aitmp;
      }
    else {
      int id = size(charges);
      charges.push_back(chargedata());

      {chargedata& ch(charges[id]);
      ch.c = c; ch.otmp = c->aitmp; ch.lowlink = id; c->aitmp = id;
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
      
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->mov[i];
        if(!c2) continue;
        if(c2->aitmp == from) continue;
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
    if(c->wall == waSandstone)
      c->wall = waNone, c->item = itFulgurite,
      drawParticles(c, winf[waSandstone].color, 16);
    if(c->wall == waDeadTroll) c->wall = waCavefloor;
    if(c->wall == waDeadTroll2 || isAlchAny(c) || c->wall == waVinePlant)
      drawParticles(c, winf[c->wall].color, 16),
      c->wall = waNone;
    /* if(c->wall == waCharged)
      c->wall = waMetal; */
    }
  
  void listChargedCells(cell *c, eCharge last = ecConductor) {
    if(eq(c->aitmp, sval)) return;
    eCharge here = getCharge(c);
    /* if(c->cpdist <= 2) {
      printf("monst=%s ", dnameof(c->monst));
      printf("wall=%s ", dnameof(c->wall));
      printf("c=%p here=%d last=%d\n", c, here, last);
      } */
    if(here == ecIsolator) c->aitmp = sval;
    if(!conduct(last, here)) return;
    if(here == ecCharged) chargecells.push_back(c);
    c->aitmp = sval;
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->mov[i];
      if(c2) listChargedCells(c2, here);
      }
    }
  
  void init() {
    chargecells.clear();
    if(!haveelec && !afterOrb) return;
    sval++;
    for(int i=0; i<size(dcal); i++) listChargedCells(dcal[i]);
    
    charges.resize(2); 
    charges[0].lowlink = 0; charges[1].lowlink = 1;
    if(!havecharge) return;
    
    xstack.clear();
    
    for(int i=0; i<size(chargecells); i++) 
      connect(1, chargecells[i]);
  
    havethunder = charges[1].lowlink == 0;
    if(havethunder) {
      for(int i=0; i<size(xstack); i++) { 
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
      for(int i=2; i<size(charges); i++) if(charges[i].fire)
        affect(charges[i].c);
      }
    }
  
  void cleanup() {
    for(int i=2; i<size(charges); i++) 
      charges[i].c->aitmp = charges[i].otmp;
    charges.resize(0); 
    }
  
  void draw(cell *c, eCharge what) {
    if(c->ligon) return;
    c->ligon = true;
    for(int i=0; i<c->type; i++) {
      cell *c2 = c->mov[i];
      if(!c2) continue;
      eCharge ch = getCharge(c2);
      if(conduct(what, ch))
        draw(c2, ch);
      }
    }
  
  void drawcharges() {
    for(int i=0; i<size(dcal); i++) 
      if(getCharge(dcal[i]) == ecCharged)
        draw(dcal[i], ecCharged);
    }
  
  bool affected(cell *c) {
    if(c->aitmp >= 0 && c->aitmp < size(charges) && charges[c->aitmp].c == c) 
      return charges[c->aitmp].fire;
    return false;
    }
  
  struct builder {
    builder() { init(); }
    ~builder() { cleanup(); }
    };

  void act() {
    int k = tkills();
    for(int i=0; i<numplayers(); i++)
      if(multi::playerActive(i) && playerpos(i)->land == laStorms && !afterOrb) 
        markOrb(itOrbShield), markOrb(itOrbAether);
    builder b;
    fire();
    if(!afterOrb)
      achievement_count("ELEC", tkills() - k, 0);
    }
  
  // 0 = no close escape, 1 = close escape, 2 = message already shown
  int lightningfast;
  void checklightningfast() {
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

  
  };

namespace princess {

  bool generating = false;
  bool challenge = false;
  bool saved = false;
  bool everSaved = false;
  
  bool forceVizier = false;
  bool forceMouse = false;
  bool gotoPrincess = false;
  bool nodungeon = false;
  bool squeaked = false;

  int saveHP = 0, saveArmedHP = 0;
  
  int reviveAt;
  
  vector<info*> infos;
  
  void assign(info *i) {
    if(i->alt) i->alt->emeraldval = i->id;
    }
  
  int newInfo(cell *c) {
    info *i = new info;
    i->prison = c;
    i->princess = c;
    i->alt = c->master->alt;
    i->id = size(infos);
    i->bestdist = 0;
    i->bestnear = OUT_OF_PRISON;
    infos.push_back(i);
    assign(i);
    return i->id;
    }
  
  void newFakeInfo(cell *c) {
    info *i = new info;
    i->prison = NULL;
    i->princess = c;
    i->alt = NULL;
    i->id = size(infos);
    i->bestdist = items[itSavedPrincess] ? OUT_OF_PALACE : OUT_OF_PRISON;
    i->bestnear = 0;
    infos.push_back(i);
    assign(i);
    }
  
  info *getPrisonInfo(cell *c) {
    if(euclid || quotient || sphere) return NULL;
    if(c->land != laPalace) return NULL;
    if(!c->master->alt) return NULL;
    int ev = c->master->alt->alt->emeraldval; // NEWYEARFIX
    if(ev < 0 || ev >= size(infos)) return NULL;
    if(infos[ev]->alt != c->master->alt->alt) return NULL;
    return infos[ev];
    }
  
  info *getPrincessInfo(cell *c) {
    for(int i=0; i<size(infos); i++) if(infos[i]->princess == c) {
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

  int dist(cell *c) {
    if(c->land != laPalace && c->land != laDungeon) return OUT_OF_PALACE;
    else if(quotient || sphere || torus) return OUT_OF_PRISON;
    else if(euclid) return celldistAlt(c);
    else if(!c->master->alt) return OUT_OF_PRISON;
    else return celldistAlt(c);
    }
  
  void clear() {
    for(int i=0; i<size(infos); i++) delete infos[i];
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
    for(int i=0; i<size(infos); i++) {
      if(infos[i]->princess && infos[i]->bestdist == OUT_OF_PALACE && isPrincess(infos[i]->princess->monst))
        inf = infos[i];
      }
    if(inf) { inf->princess->monst = moNone; inf->princess = c; }
    else newFakeInfo(c);
    return true;
    }

  void bringBack() {
    if(bringBackAt(cwt.c->mov[cwt.spin])) return;
    for(int i=1; i<size(dcal); i++)
      if(bringBackAt(dcal[i])) return;
    }
  
  void setdist(info *i, int newdist) {
    if(newdist < ALTDIST_ERROR && newdist > i->bestdist) {
      i->bestdist = newdist;
//    printf("Improved dist to %d\n", newdist);
      if(newdist == OUT_OF_PALACE) {
        if(!princess::saved)
#if CAP_INV
        if(!inv::on || !inv::usedForbidden)
#endif
          achievement_gain("PRINCESS1");
        princess::saved = true;
        princess::everSaved = true;
        if(inv::on && !princess::reviveAt)
          princess::reviveAt = gold(NO_LOVE);
        items[itSavedPrincess]++;
        }
      if(newdist == OUT_OF_PRISON && princess::challenge) {
        addMessage(XLAT("Congratulations! Your score is %1.", its(i->value)));
        achievement_gain("PRINCESS2");
        if(!cheater) achievement_score(36, i->value);
        showMissionScreen();
        }
      }
    if(i->princess->land == laDungeon && !saved && !nodungeon) {
      addMessage(XLAT("%The1 says, \"not this place, it looks even worse...\"", moPrincess));
      nodungeon = true;
      }
    }

  void save(cell *princess) {
    if(euclid) return;
    princess::info *i = princess::getPrincessInfo(princess);
    if(!i || i->bestdist <= 3) princess->monst = moNone;
    else if(i) setdist(i, OUT_OF_PRISON);
    }

  void move(cell *ct, cell *cf) {
    if(euclid) return;
    princess::info *i = princess::getPrincessInfo(cf);
    if(!i) {
      // note: OK if mapediting or loading
      printf("Warning: unknown princess\n"); 
      if(!cheater) 
        addMessage("Warning: unknown princess (that's a bug, please report)");
      }
    else {
      i->princess = ct;
      setdist(i, dist(ct));
      // printf("newdist = %d (vs %d)\n", newdist, i->bestdist);
      }
    }

  void mouseSqueak(cell *c) {
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
  
  void line(cell *c) {
    int d = (euclid || c->master->alt) ? celldistAlt(c) : 200;
    eMonster m = c->monst;
  
    static int msgid = 0;
  
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
    else if(msgid == 80) {
      addMessage(XLAT("\"Are there Temples of Cthulhu in your world? Why not?\"", m));
      }    
    else {
      msgid++; goto retry;
      }
  
    msgid++;
    }

  void playernear(cell *c) {
    info *i = getPrisonInfo(c);
    int d = dist(c);
    // if(i) printf("d=%d bn=%d\n", d, i->bestnear);
    if(i && d < i->bestnear) {
      if(i->bestnear > 100 && d <= 100) {
        i->value = items[itPalace];
        if(princess::challenge) 
          addMessage(XLAT("Hardness frozen at %1.", its(i->value)));
        }
      i->bestnear = d;
      }
    }
    
  }

namespace clearing {

  bool buggyplant = false;
  
  std::map<heptagon*, clearingdata> bpdata;
  
  int plantdir(cell *c) {
    if(!quotient) {
      generateAlts(c->master);
      for(int i=0; i<S7; i++)
        generateAlts(c->master->move[i]);
      }
    int d = celldistAlt(c);
    
    if(nonbitrunc) {
      for(int i=0; i<S7; i++) {
        cell *c2 = createMov(c, i);
        if(!pseudohept(c2) && celldistAlt(c2) == d-1)
          return i;
        }
      for(int i=0; i<S7; i++) {
        cell *c2 = createMov(c, i);
        if(celldistAlt(c2) == d-1)
          return (i+1) % S7;
        }
      }

    for(int i=1; i<S6; i+=2) {
      cell *c2 = createMov(c, i);
      if(celldistAlt(c2) == d-1)
        return i;
      }

    int quseful = 0, tuseful = 0, tuseful2 = 0;
    for(int i=1; i<S6; i+=2) {
      cell *c2 = c->mov[i];
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
      if((d & 7) < 4) i = (i+2) % S6;
      return i;
      }
    printf("error in plantdir\n");
    return 1;
    }

  vector<cell*> onpath;
  vector<int>   pdir;
  vector<cell*> rpath;
    
  void generate(cell *c) {
    if(buggyplant) return;
    
    if(sphere) return;
    
    if(euclid) {
      if(torus) return;
      if(pseudohept(c)) return;
      c->monst = moMutant;
      
      int x, y;
      tie(x,y) = cell_to_pair(c);

      int xco = x * 2 + y + 1;
      c->stuntime = (8-xco/2) & 15;
      // 2, 4, 5, 7
      
      if(pseudohept(createMov(c, 0)))
        c->mondir = 1 + hrand(2) * 4;
      else
        c->mondir = 0;
      return;
      }
    
    // cell *oc = c;
    if(!euclid) generateAlts(c->master);
    if(pseudohept(c)) return;
    heptagon *a = euclid ? NULL : c->master->alt->alt;
    clearingdata& bd(bpdata[a]);
    if(!bd.root) { bd.root = c; bd.dist = 8; }
    
    onpath.clear(); pdir.clear(); rpath.clear();
    
    int steps = 0;
    
    int ds;
    
    int stepcount = 0;
    while(true) {
      if(c == bd.root) {ds = bd.dist; break; }

      // printf("R %4d C %4d\n", celldistAlt(bd.root), celldistAlt(c));
      if(celldistAlt(c) > celldistAlt(bd.root)) {
        stepcount++;
        if(stepcount > 1000) {
          printf("buggy #1\n");
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
        // printf("c [%4d] %p -> %p\n", celldistAlt(c), c, c->mov[d]);
        c = c->mov[d];
        }
      else {
        bd.dist--;
        if(bd.dist < -1000) { 
          for(int i=0; i<steps; i++) 
            onpath[i]->item = itBuggy;
          for(int i=0; i<(int) rpath.size(); i++) 
            rpath[i]->item = itBuggy;
          buggyplant = true;
          printf("buggygen\n");
          return;
          }
        rpath.push_back(bd.root);
        // printf("r [%4d] %p -> %p\n", celldistAlt(bd.root), bd.root, bd.root->mov[plantdir(bd.root)]);
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
        c->stuntime = ds;
        }
      if(c->mpdist <= 7 && c->monst != moMutant)
        break;
      steps--; ds++;
      }
    }
  }

namespace whirlpool {

  bool escaped = false; // escaped the Whirlpool?

  // next == +1 -> next
  // next == -1 -> prev
  cell *get(cell *c, int next) {
    int i = 0;
    if(!euclid && !c->master->alt) return NULL;
    int d = celldistAlt(c);
    int d2;
    while(true) {
      if(i == c->type) return NULL;
      if(c->mov[i] && (d2 = celldistAlt(c->mov[i])) != d)
        break;
      i++;
      }
    if(i == c->type) return NULL;
    if(d>d2) next = -next;
    for(int j=1; j<c->type; j++) {
      cell *c2 = c->mov[(i+MODFIXER+next*j) % c->type];
      if(celldistAlt(c2) == d) return c2;
      }
    return NULL;
    }
  
  void build(vector<cell*>& whirlline, int d) {
    again: 
    cell *at = whirlline[size(whirlline)-1];
    cell *prev = whirlline[size(whirlline)-2];
    for(int i=0; i<at->type; i++) 
      if(at->mov[i] && (euclid || at->mov[i]->master->alt) && celldistAlt(at->mov[i]) == d && at->mov[i] != prev) {
        if(at->mov[i] == whirlline[0]) return; // loops in weird geometries?
        if(at->mov[i] == whirlline[size(whirlline)/2]) return; // even weirder geometry?
        whirlline.push_back(at->mov[i]);
        goto again;
        }
    }
  
  void generate(cell *wto) {
    if(wto->wall == waBoat || wto->monst)
      return;
    
    wto->wall = waSea;
    
    if(hrand(35000) < 40 + items[itWhirlpool] + yendor::hardness())
      wto->monst = moCShark;
    else if(hrand(5000) < 500)
       wto->wall = waBoat;
    
    if(wto->wall == waBoat && (euclid || wto->master->alt)) { 
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
      animateMovement(wfrom, wto, LAYER_BOAT);
      }
    
    if(wfrom && wto && wfrom->item && !wto->item && wfrom->wall != waBoat) {
      // Keys and Orbs of Yendor never disappear!
      if(wfrom->item == itKey || wfrom->item == itOrbYendor)
        for(int i=0; i<wto->type; i++) createMov(wto, i);
      moveItem(wfrom, wto, false);
      pickupMovedItems(wfrom);
      pickupMovedItems(wto);
      }
    
    if(wto && !wfrom) 
      generate(wto);
    }
  
  void moveAt(cell *c) {
    if(c->land != laWhirlpool) return;
    if(eq(c->aitmp, sval)) return;
    if(!(euclid || c->master->alt)) return;
    cell *c2 = get(c, 1);
    if(!c2) return;
    int d = celldistAlt(c);
    vector<cell*> whirlline;
    whirlline.push_back(c);
    whirlline.push_back(c2);
    build(whirlline, d);
    reverse(whirlline.begin(), whirlline.end());
    build(whirlline, d);
    int z = size(whirlline);
    
    for(int i=0; i<z; i++) 
      whirlline[i]->aitmp = sval;
  
    whirlMove(NULL, whirlline[0]);
    
    for(int i=0; i<z-1; i++) 
      whirlMove(whirlline[i], whirlline[i+1]);
    
    whirlMove(whirlline[z-1], NULL);
    }
  
  void move() {
    sval++;
    for(int i=0; i<size(dcal); i++) {
      cell *c = dcal[i];
      moveAt(c);
      }
    // Keys and Orbs of Yendor always move
    using namespace yendor;
    for(int i=0; i<size(yi); i++) {
      moveAt(yi[i].path[0]);
      moveAt(yi[i].path[YDIST-1]);
      }
    }
  }

bool operator == (const cellwalker& c1, const cellwalker& c2) {
  return c1.c == c2.c && c1.spin == c2.spin && c1.mirrored == c2.mirrored;
  }

namespace mirror {

  vector<pair<int, cellwalker>> mirrors;
  static const int LIGHTNING = -1; // passed instead of cpid
  
  bool noMirrorOn(cell *c) {
    return c->monst || (!shmup::on && isPlayerOn(c)) || (geometry != gQuotient2 && geometry != gTorus && c->cpdist > gamerange());
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

  void destroyKilled() {
    int j = 0;
    for(int i=0; i<size(mirrors); i++)
      if(mirrors[i].second.c->monst == moMimic)
        mirrors[j++] = mirrors[i];
    mirrors.resize(j);
    }
  
  void unlist() {
    for(auto& m: mirrors)
      if(m.second.c->monst == moMimic)
        m.second.c->monst = moNone;
    }
  
  void list() {
    for(auto& m: mirrors)
      m.second.c->monst = moMimic;
    }

  void destroyAll() {
    unlist();
    mirrors.clear();
    }
  
  void createMirror(cellwalker cw, int cpid) {
    if(!shmup::on && inmirror(cw))
      cw = reflect(cw);
    if(cpid == LIGHTNING)
      castLightningBolt(cw);
    else if(cellMirrorable(cw.c)) {
      for(auto& m: mirrors)
        if(m == make_pair(cpid,cw))
          return;
      mirrors.emplace_back(cpid, cw);
      }
    }
  
  void createMirrors(cellwalker cw, int cpid) {
    cw.mirrored = !cw.mirrored;
    cell *c = cw.c;
    
    for(int i=0; i<cw.c->type; i++) {
      auto cws = cw + wstep;
      if(cws.c->type == c->type) 
        createMirror(cws+i, cpid);
      cw += 1;
      }
    }
  
  void createMirages(cellwalker cw, int cpid) {
    if(nonbitrunc) {
      for(int i=0; i<cw.c->type; i++)
        createMirror(cw + i + wstep + 3 + wstep + 5 + wstep + 3 - i, cpid);
      return;
      }
    for(int i=0; i<S6; i++) {
      auto cw0 = cw + i + wstep;
      if(!ctof(cw0.c)) {
        createMirror(cw0 + 2 + wstep - (2+i), cpid);
        createMirror(cw0 - 2 + wstep + (2-i), cpid);
        }
      }
    }

  void createHere(cellwalker cw, int cpid) {
    if(!cw.c) return;
    if(cw.c->wall == waCloud)
      createMirages(cw, cpid);
    if(cw.c->wall == waMirror)
      createMirrors(cw, cpid);
    }
  
  void breakMirror(cellwalker cw, int pid) {
    if(!cw.c) return;
    cell *c = cw.c;
    if(c->wall == waMirror || c->wall == waCloud) {
      drawParticles(c, winf[c->wall].color, 16);
      playSound(c, "pickup-mirror", 50);
      if(pid >= 0 && (cw.c->land == laMirror || cw.c->land == laMirrorOld)) {
        dynamicval<int> d(multi::cpid, pid);
        gainShard(cw.c, c->wall == waMirror ? "The mirror shatters!" : "The cloud turns into a bunch of images!");
        }
      c->wall = waNone;
      }
    }
  
  bool isKilledByMirror(cell *c) {
    for(auto& m: mirrors) {
      cell *c1 = (m.second + wstep).c;
      if(inmirror(c1)) c1 = reflect(cellwalker(c1, 0, false)).c;
      if(c1 == c && canAttack(m.second.c, moMimic, c, c->monst, 0))
        return true;
      }
    return false;
    }
   
  void go(bool fwd) {
    int tk = tkills();
    int nummirage = 0;    
    int j = 0;
    
    for(int i=0; i<size(mirrors); i++) {
      auto& m = mirrors[i];
      bool survive = true;
      if(m.first == multi::cpid) {
        cell *c = m.second.c;
        if(!m.second.mirrored) nummirage++;
        auto cw2 = m.second + wstep;
        if(inmirror(cw2)) cw2 = reflect(cw2);
        cell *c2 = cw2.c;
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
          if(noMirrorOn(c2) || !passable_for(moMimic, c2, c, P_MONSTER | P_MIRROR | P_MIRRORWALL)) {
            survive = false;
            continue;
            }
          c->monst = moMimic;
          moveMonster(c2, c);
          c2->monst = moNone;
          empathyMove(c, c2, neighborId(c2, c));
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

  void act(int d, int flags) {
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
  
  void breakAll() {
    destroyKilled();
    unlist();
    if(numplayers() == 1)
      createHere(cwt, 0);
    else for(int i=0; i<numplayers(); i++)
      createHere(multi::player[i], i);
    for(int i=0; i<size(mirrors); i++)
      createHere(mirrors[i].second, mirrors[i].first);
    if(numplayers() == 1)
      breakMirror(cwt, 0);
    else for(int i=0; i<numplayers(); i++)
      breakMirror(multi::player[i], i);
    for(int i=0; i<size(mirrors); i++)
      breakMirror(mirrors[i].second, -1);
    list();
    }
  
  int mirrordir(cell *c) {
    if(c->type == 7) return c->bardir;

   int icount = 0, isum = 0;
   for(int i=0; i<6; i+=2) {
     if(createMov(c, i)->bardir == c->spin(i))
       icount++, isum+=i;
     }
   if(icount != 1) return -1;
   return isum;
   }
  
  pair<bool, cellwalker> traceback(vector<int>& v, cellwalker cw) {
    bool goout = false;
    for(int i=size(v)-1; i>=0; i--) {
      if(v[i]) cw -= v[i];
      else { 
        cw += wstep;
        if(cw.c->land == laMirrorWall || cw.c->land == laMirror) goout = true;
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
      cell *c0 = (cw+wstep).c;
      int go = 0;
      if(!inmirror(c0)) go = 2;
      else if(depth(c0) && depth(c0) < depth(cw.c)) go = 1;
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
    if(cw.c->land == laMirrorWall || cw.c->land == laMirrorWall2) {
      if(cw.c->type == 7) {
        while(cw.spin != cw.c->bardir) {
          cw += 1;
          v.push_back(1);
          stepcount++; if(stepcount > 10000) { printf("failhep\n"); return cw; }
          }
        if(nonbitrunc && (cw+wstep).c == cwcopy.c)
          v.pop_back();
        if(nonbitrunc && (cw+3+wstep).c->land == laMirrored && (cw+2+wstep).c->land == laMirrorWall) {
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
        while((cw+wstep).c->type != 7) {
          cw ++;
          v.push_back(1);
          }
        int icount = 0;
        for(int i=0; i<3; i++) {
          if((cw+wstep).c->bardir == cw.c->spin(cw.spin))
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
        while((cw+wstep).c->bardir != cw.c->spin(cw.spin)) {
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

  cellwalker reflect(const cellwalker& cw) {
    if(!cw.c) return cw;
    if((cw.c->landparam & 255) == 0) {
      bool cando = false;
      forCellEx(c2, cw.c) if(c2->landparam & 255) cando = true;
      if(cando) buildEquidistant(cw.c);
      }
    if((cw.c->landparam & 255) == 0) return cw;
    int cid = (cw.c->landparam >> 8) & CACHEMASK;
    if(cache[cid].first != cw.c) {
      cid = nextcache++;
      nextcache &= CACHEMASK;
      cw.c->landparam &= ~ (CACHEMASK << 8);
      cw.c->landparam |= (cid << 8);
      cache[cid].first = cw.c;
      cellwalker cw0(cw.c, 0, false);
      cache[cid].second = reflect0(cw0);
      int tries = 64;
      while(inmirror(cache[cid].second.c) && tries--)
        cache[cid].second = reflect0(cache[cid].second);
      }
    cellwalker res = cache[cid].second + cw.spin;
    if(cw.mirrored) res.mirrored = !res.mirrored;
    return res;
    }

  }

namespace hive {

  int hivehard() {
    return items[itRoyalJelly] + hardness_empty();
    // 0, 5, 40, 135
    }

  eMonster randomHyperbug() {
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
  
  int bugcount[BUGCOLORS];
  
  bool isBugEnemy(cell *c, int k) {
    if(isPlayerOn(c) && !invismove) return true;
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
    short& i(c->aitmp);
    if(i >= 0 && i < size(buginfo) && buginfo[i].where == c)
      return;
    i = size(buginfo);
    buginfo.resize(i+1);
    buginfo_t& b(buginfo[i]);
    b.where = c;
    for(int k=0; k<BUGCOLORS; k++) {
      b.dist[k] = BUGINF;
      bool havebug = false, haveother = false;
      for(int dir=0; dir<c->type; dir++) {
        cell *c2 = c->mov[dir];
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
    if(size(buginfo) < 30000) {
      for(int dir=0; dir<c->type; dir++) {
        cell *c2 = c->mov[dir];
        if(c2) {
          // if(isBug(c)) bugcellq.push_back(c2); => does not help...
          for(int t=0; t<c2->type; t++)
            if(c2->mov[t] && isBug(c2->mov[t]))
              bugcellq.push_back(c2), 
              bugcellq.push_back(c2->mov[t]);
          }
        }
      }*/
    
    // use pheromones!
    if(c->land == laHive && c->landparam > 1 && c->wall != waWaxWall) {
      c->landparam --;
      for(int dir=0; dir<c->type; dir++) {
        cell *c2 = c->mov[dir];
        if(c2) {
          for(int t=0; t<c2->type; t++)
            if(c2->mov[t])
              bugcellq.push_back(c2), 
              bugcellq.push_back(c2->mov[t]);
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
      cell *c2 = c->mov[dir];
      if(!c2) continue;
      if(c2->aitmp < 0 || c2->aitmp >= size(buginfo)) continue;
      if(!passable(c, c2, P_MONSTER)) continue;
      int j = c2->aitmp;
      if(buginfo[j].where != c2) continue;
      if(buginfo[j].dist[k] < d) goodmoves++;
      bugQueueInsert(k, j, d+1);
      }
    if(isBug(c) && c->monst == moBug0+k) {
      bugcount[c->monst - moBug0]++;
      bugtomove.push_back(bugtomove_t(d,goodmoves,i));
      }
    }
  
  #include <set>
  
  bool fightspam(cell *c) {
    return c->cpdist >= gamerange() || 
      isMetalBeast(c->monst) || c->monst == moSkeleton ||
      isIvy(c->monst) || isMutantIvy(c->monst);
    }
  
  void movebugs() {
    buginfo.clear();
    for(int k=0; k<BUGCOLORS; k++) bugqueue[k].clear();
    for(int k=0; k<BUGCOLORS; k++) bugqueue4[k].clear();
    for(int k=0; k<BUGCOLORS; k++) bugcount[k] = 0;
    bugtomove.clear();
    deadbug.clear();
    
    int xdcs = size(dcal); for(int i=0; i<xdcs; i++) bugcell(dcal[i]);
    for(int i=0; i<size(bugcellq); i++) bugcell(bugcellq[i]);
    bugcellq.clear();
    
    // printf("buginfo = %d\n", size(buginfo));
    
    for(int k=0; k<BUGCOLORS; k++) {
      int t = 0;
      last_d = -1;
      int invadist = 4 - (items[itRoyalJelly]+10) / 20;
      if(invadist<0) invadist = 0;
      for(; t<size(bugqueue[k]) && last_d < invadist-1; t++) handleBugQueue(k, t);
      for(int u=0; u<size(bugqueue4[k]); u++)
        bugQueueInsert(k, bugqueue4[k][u], invadist);
      bugqueue4[k].clear();
      for(; t<size(bugqueue[k]); t++) handleBugQueue(k, t);
      }
    
    for(int k=0; k<BUGCOLORS; k++) {
      set<int> check;
      for(int t=0; t<size(bugqueue[k]); t++) {
        if(check.count(bugqueue[k][t])) {
          printf("REPETITION! [%d]\n", t);
          }
        check.insert(bugqueue[k][t]);
        }
      }
    
    hrandom_shuffle(&bugtomove[0], size(bugtomove));
    sort(bugtomove.begin(), bugtomove.end());
    
    int battlecount = 0;
    for(int t=0; t<size(bugtomove); t++) {
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
  
      for(int dir=0; dir<c->type; dir++) {
        cell *c2 = c->mov[dir];
        int qual = -10;
        if(!c2) continue;
        else if(againstRose(c, c2)) qual = -50;
        else if(canAttack(c, m, c2, c2->monst, AF_GETPLAYER))
          qual = c2->monst == moDeadBug ? -60: isBugEnemy(c2,k) ? 2 : -20;
        else if(!passable(c2, c, 0)) 
          qual = passable(c2, c, P_DEADLY) ? -30 : -60;
        else if(c2->aitmp < 0 || c2->aitmp >= size(buginfo)) qual = -15;
        else if(buginfo[c2->aitmp].where != c2) qual = -15;
        else if(buginfo[c2->aitmp].dist[k] < b.dist[k])
          qual = 1;
        else if(buginfo[c2->aitmp].dist[k] == b.dist[k])
          qual = 0;
        // printf("%d->#%d %d: %d\n", i, dir, c2->tmp, qual);
        if(qual > bqual) bqual = qual, q=0;
        if(qual == bqual) gmoves[q++] = dir;
        }
      
      if(!q) { if(c->land == laHive) c->landparam += 3; continue; }
      int d = gmoves[hrand(q)];
      cell *c2 = c->mov[d];
      if(c2->monst || isPlayerOn(c2)) {
        eMonster killed = c2->monst;
        if(isPlayerOn(c2)) killed = moPlayer;
        if(isBug(killed)) battlecount++;
        else if(killed != moPlayer && !fightspam(c2))
          addMessage(XLAT("%The1 fights with %the2!", c->monst, killed));
        attackMonster(c2, AF_NORMAL | AF_GETPLAYER, c->monst);
        // killMonster(c);
        if(isBug(killed)) {
          c2->monst = moDeadBug, deadbug.push_back(c2);
          bugcount[killed - moBug0]--;
          }
  //    c->monst = moDeadBug, deadbug.push_back(c);
        }
      else {
        moveMonster(c2, c);
        // pheromones!
        if(c->land == laHive && c->landparam < 90) c->landparam += 5;
        if(c2->land == laHive && c2->landparam < 90) c2->landparam += 5;
        // if(isHive(c2->land)) c2->land = eLand(laHive0+k);
  /*      if(c2->item == itRoyalJelly && !isQueen(m)) {
          // advance!
          c2->monst = eMonster(m+BUGCOLORS);
          c2->item = itNone;
          } */
        }
      }
    
    // cleanup
    for(int i=0; i<size(deadbug); i++) deadbug[i]->monst = moNone;
    if(battlecount)
      addMessage(XLAT("The Hyperbugs are fighting!"));
      
    int maxbug = 0;
    for(int k=0; k<BUGCOLORS; k++) if(bugcount[k] > maxbug) maxbug = bugcount[k];
    
    achievement_count("BUG", maxbug, 0);
    }
  
  void bugcitycell(cell *c, int d) {
    short& i = c->aitmp;
    if(i >= 0 && i < size(buginfo) && buginfo[i].where == c)
      return;
    i = size(buginfo);
    buginfo_t b;
    b.where = c;
    b.dist[0] = d;
    buginfo.push_back(b);
    }
  
  void createBugArmy(cell *c) {
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
      if(c->mov[i] && c->mov[i]->mpdist < c->mpdist) gdir = i;
      }
    if(!gdir) return;
    cellwalker bf(c, gdir);
    int radius = 9;
    if(getDistLimit() <= 6) radius = 6;
    if(chaosmode) radius = 5;
    if(chaosmode && getDistLimit() <= 5) radius = 4;
    if(getDistLimit() <= 3) radius = 3;

    for(int i=2; i<radius; i++) {
      if(bf.c->type == 6)
        bf += 3;
      else
        bf += 3 + hrand(2);
      bf += wstep;
      }
    cell *citycenter = bf.c;
    buginfo.clear();
    
    
    // mark the area with BFS
    bugcitycell(citycenter, 0);
    for(int i=0; i<size(buginfo); i++) {
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
    for(int i=0; i<size(buginfo); i++) {
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

  }

inline float& HEAT(cell *c) { return c->LHU.heat; }

namespace heat {
  
  void affect(cell *c, double delta) {
    if(isIcyLand(c)) HEAT(c) += delta;
    }
  
  double absheat(cell *c) {
    if(c->land == laCocytus) return HEAT(c) -.6;
    if(c->land == laIce || c->land == laBlizzard) return HEAT(c) -.4;
    return 0;
    }
  
  double celsius(cell *c) { return absheat(c) * 60; }
  
  // adjust to the improved heat transfer algorithm in 9.4
  const float FIX94 = 1.5;

  vector<cell*> offscreen_heat, offscreen_fire; // offscreen cells to take care off

  void processheat(double rate = 1) {
    if(markOrb(itOrbSpeed)) rate /= 2;
    int oldmelt = kills[0];    
    
    vector<cell*> offscreen2;
    
    sval++;
    
    int gr = gamerange();
    
    for(cell *c: offscreen_heat) {
      if(c->cpdist > gr && !doall) {
        if(eq(c->aitmp, sval)) continue; 
        c->aitmp = sval;
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
    
    offscreen_heat = move(offscreen2);
  
    for(int i=0; i<numplayers(); i++) {
      cell *c = playerpos(i);
      if(!c) continue;
      double xrate = (c->land == laCocytus && shmup::on) ? rate/3 : rate;
      if(nonbitrunc) xrate *= 1.7;
      if(!shmup::on) xrate /= FIX94;
      if(isIcyLand(c))
        HEAT(c) += (markOrb(itOrbWinter) ? -1.2 : 1.2) * xrate;
      }
    
    vector<cell*>& allcells = currentmap->allcells();

    int dcs = size(allcells);
    
    vector<ld> hmods(dcs, 0);
    
    for(int i=0; i<dcs; i++) {
      cell *c = allcells[i];
      double xrate = (c->land == laCocytus && shmup::on) ? 1/3. : 1;
      if(nonbitrunc) xrate *= 1.7;
      if(!shmup::on) xrate /= FIX94;
      if(c->cpdist > gr && !doall) break;
  
      if(isIcyLand(c)) {
        ld hmod = 0;

        if(c->monst == moRanger) hmod += 3 * xrate;
        if(c->monst == moDesertman) hmod += 4 * xrate;
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
        
        forCellEx(ct, c) {
          if(!isIcyLand(ct) && isFire(ct)) 
            hmod += xrate*.1;
          if(ct->land == laVolcano) 
            hmod += xrate * (ct->wall == waMagma ? .4 : .2);
          }
        
        forCellEx(ct, c) {
          if(!isIcyLand(ct)) {
            // make sure that we can still enter Cocytus,
            // it won't heat up right away even without Orb of Winter or Orb of Speed
            if(isPlayerOn(ct) && (c->land != laCocytus || markOrb(itOrbWinter))) 
              hmod += (markOrb(itOrbWinter) ? -1.2 : 1.2) / 4 * xrate;
            continue;
            }
          ld hdiff = absheat(ct) - absheat(c);
          hdiff /= 10;

          if(ct->land == laBlizzard) {
            int v = (windmap::at(ct) - windmap::at(c)) & 255;
            if(v > 128) v -= 256;
            if(v < windmap::NOWINDFROM && v > -windmap::NOWINDFROM)
              hdiff = hdiff * (1 - v * 5. / windmap::NOWINDFROM);
            }

          if(shmup::on && (c->land == laCocytus || ct->land == laCocytus))
            hdiff /= 3;
          // if(c->mov[j]->cpdist > 7 && !quotient) hdiff += -HEAT(c) / 30;
          hmod += hdiff;
          }
        // printf("%d ", vsum);
        
        hmods[i] = hmod;
        }
      
      if(HEAT(c) && !doall)
        offscreen_heat.push_back(c);
      }
    
    #define MELTCOLOR 0xA04040
    
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

  void processfires() {
    newfires.clear();
    
    vector<cell*> offscreen2;

    sval++;
    
    vector<cell*>& allcells = currentmap->allcells();

    for(int x: {0,1}) for(cell *c: x==0 ? allcells : offscreen_fire) {
      if(eq(c->aitmp, sval)) continue;
      c->aitmp = sval;
      
      if(isFireOrMagma(c)) {
        if(c->wall == waMagma) c->wparam = 20;
      
        cell *last = c->mov[c->type-1];
        
        forCellEx(c2, c) {
          
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
      }

   for(int i=0; i<size(newfires); i++) {
      cell* c = newfires[i].first;
      int qty = newfires[i].second;
      qty /= 2;
      if(c->wall == waBonfireOff) activateActiv(c, false);
      else if(cellHalfvine(c)) destroyHalfvine(c, waPartialFire, 6);
      else makeflame(c, qty, false);
      if(c->wparam < qty) c->wparam = qty;
      offscreen2.push_back(c);
      if(c->land == laRose || c->land == laWildWest || c->land == laOvergrown || isHaunted(c->land) || c->land == laMountain || c->land == laIce) {
        for(int j=c->mpdist-1; j>=7; j--) setdist(c, j, NULL);
        }
      }
   
    offscreen_fire = move(offscreen2);
    }  

  }

bool gardener = false;

bool lifebrought = false; // was Life brought to the Dead Caves?

void livecaves() {
  vector<cell*>& allcells = currentmap->allcells();
  int dcs = size(allcells);
  
  vector<cell*> bringlife;
  int gr = gamerange();
  
  for(int i=0; i<dcs; i++) {
    cell *c = allcells[i];
    if(!doall && c->cpdist > gr+1) break;
    
    if(c->wall == waCavefloor || c->wall == waCavewall || c->wall == waDeadTroll) {
      c->aitmp = 0;
      if(c->monst == moDarkTroll) c->monst = moTroll;
      if(c->item || c->monst || c->cpdist == 0) continue;
      forCellEx(c2, c) {
        eWall w = c2->wall;
        if(w == waDeadfloor) c->aitmp++, bringlife.push_back(c2);
        else if(w == waDeadwall || (w == waDeadfloor2 && !c2->monst))
          c->aitmp--, bringlife.push_back(c2);
        else if(w == waCavefloor) c->aitmp++;
        else if(w == waCavewall) c->aitmp--;
        else if(w == waRubble) c->aitmp--;
        else if(w == waGargoyle) c->aitmp--;
        else if(w == waGargoyleFloor) c->aitmp--;
        else if(w == waGargoyleBridge) c->aitmp--;
        else if(w == waStone) ;
        else if(w == waDeadTroll) c->aitmp -= 5;
        else if(w == waDeadTroll2) c->aitmp -= 3;
        else if(w == waPetrified || w == waPetrifiedBridge) c->aitmp -= 2;
        else if(w == waVinePlant) c->aitmp--;
        else if(chaosmode && c2->land != laCaves && c2->land != laEmerald) ;
        else if(c2->land == laTrollheim) ; // trollheim floor does not count
        else if(w != waBarrier) c->aitmp += 5;
        
        if(sword::at(c)) c->aitmp += 500;

        if(c2->cpdist == 0 && markOrb(itOrbDigging)) c->aitmp+=100;
        if(items[itOrbEmpathy] && isFriendly(c2) && markEmpathy(itOrbDigging))
          c->aitmp+=100;
        if(w == waThumperOn) c->aitmp+=100;
        if(w == waFire) c->aitmp+=100;
        if(w == waBigStatue) c->aitmp-=100;
        if(c2->item && !peace::on) c->aitmp+=2;
        if(c2->monst == moZombie) c->aitmp += 10;
        if(c2->monst == moGhost) c->aitmp += 10;
        if(c2->monst == moTentacleGhost) c->aitmp += 10;
        if(c2->monst == moFriendlyGhost) c->aitmp += 10;
        if(c2->monst == moSkeleton) c->aitmp ++;
        if(c2->monst == moGargoyle) c->aitmp--;
        if(c2->monst == moDraugr) c->aitmp--;
        if(isDragon(c2->monst)) c->aitmp++;
        if(c2->monst == moNecromancer) c->aitmp += 10;
        if(c2->monst == moWormtail) c->aitmp++;
        if(c2->monst == moTentacletail) c->aitmp-=2;
        if(isIvy(c2)) c->aitmp--;
        if(isDemon(c2)) c->aitmp-=3;
        // if(c2->monst) c->tmp++;
        // if(c2->monst == moTroll) c->tmp -= 3;
        }
      }
    else if(c->land == laLivefjord) {
      c->aitmp = 0;
      if(c->monst == moWaterElemental)
        c->aitmp += 1000;
      if(isPlayerInBoatOn(c) && markOrb(itOrbWater))
        c->aitmp += 1000;
      if(c->monst == moEarthElemental)
        c->aitmp -= 1000;
      if(isPlayerOn(c) && markOrb(itOrbDigging))
        c->aitmp -= 1000;
      for(int j=0; j<c->type; j++) if(c->mov[j]) {
        cell *c2 = c->mov[j];
        if(c2->wall == waNone || c2->wall == waStrandedBoat)
          c->aitmp -= (c2->land == laLivefjord ? 1 : 100);
        if(c2->wall == waTempFloor || c2->wall == waTempBridge || c2->wall == waTempBridgeBlocked)
          ;
        else if(c2->wall == waDeadTroll || c2->wall == waDeadTroll2 || c2->wall == waThumperOn || isFire(c2) || snakelevel(c2))
          c->aitmp -= 10;
        else if(c2->wall == waPetrified || c2->wall == waPetrifiedBridge)
          c->aitmp -= 10;
        if(c2->wall == waBigStatue)
          c->aitmp -= 10;
        if(c2->wall == waSea || c2->wall == waBoat)
          c->aitmp += (c2->land == laLivefjord ? 1 : 100);
        if(c2->monst == moWaterElemental)
          c->aitmp += 1000;
        if(isPlayerOn(c2) && c2->wall == waBoat && markOrb(itOrbWater))
          c->aitmp += 1000;
        if(c2->monst == moEarthElemental)
          c->aitmp -= 1000;
        if(isPlayerOn(c2) && markOrb(itOrbDigging))
          c->aitmp -= 1000;
        if(items[itOrbEmpathy] && isFriendly(c2) && markEmpathy(itOrbDigging))
          c->aitmp -= 1000;

        if(c2->wall == waBarrier) {
          bool landbar = false;
          for(int k=0; k<c2->type; k++)
            if(c2->mov[k]) {
              cell *c3 = c2->mov[k];
              if(!isSealand(c3->land))
                landbar = true;
              }
          if(landbar) c->aitmp -= 5;
          else c->aitmp += 5;
          }
        }
      }
    }

  for(int i=0; i<dcs; i++) {
    cell *c = allcells[i];
    if(!doall && c->cpdist > gr+1) break;

    if(c->wall == waCavefloor || c->wall == waCavewall) {
  //  if(c->land != laCaves) continue;
  //  if(c->wall == waThumper || c->wall == waBonfire) continue;
      
      if(c->aitmp > 0) c->wall = waCavefloor;
      if(c->aitmp < 0) {
        c->wall = waCavewall;
        if(c->land != laCaves && c->land != laDeadCaves && c->land != laEmerald && !gardener) {
          gardener = true;
          achievement_gain("GARDENER");
          }
        }
      }
    else if(c->land == laLivefjord) {
      if(c->aitmp > 0 && c->wall == waStrandedBoat) c->wall = waBoat;
      if(c->aitmp > 0 && c->wall == waNone) {
        if(c->item && c->cpdist == 1 && markOrb(itOrbWater))
          collectItem(c);
        c->wall = waSea;
        }
      if(c->aitmp < 0 && c->wall == waBoat) c->wall = waStrandedBoat;
      if(c->aitmp < 0 && c->wall == waSea) c->wall = waNone;
      }
    }
  
  for(int i=0; i<size(bringlife); i++) {
    cell *c = bringlife[i];
    if(c->land == laDeadCaves && !lifebrought) { 
      lifebrought = true;
      achievement_gain("LIFEBRINGER");
      }
    if(c->wall == waDeadfloor) c->wall = waCavefloor;
    if(c->wall == waDeadfloor2) c->wall = waCavewall;
    if(c->wall == waDeadwall) c->wall = waCavewall;
    if(c->wall == waCavewall && c->item) c->wall = waCavefloor;
    if(c->land == laDeadCaves) c->land = laCaves;
    if(c->item == itSilver) c->item = itGold;
    if(c->item == itGreenStone) c->item = itOrbLife;
    if(c->monst == moEarthElemental) {
      addMessage(XLAT("%The1 is destroyed by the forces of Life!", c->monst));
      fallMonster(c);
      c->item = itOrbDigging;
      }
    }
  }

/* evolver */

namespace tortoise {
  map<cell*, cell*> emap;
  map<cell*, int> babymap;
  int last;
  
  enum tflag {
    tfShell, tfScute0, tfScute1, tfScute2, tfScute3,
    tfEdge1, tfEdge, tfEdge3,
    tfLongNeck, tfFront, tfRear, tfTail,
    tfEyeHue, tfShellHue, tfScuteHue, tfSkinHue,
    tfShellSat, tfScuteSat, tfSkinSat,
    tfShellDark, tfSkinDark,
    tfCOUNT
    };

  const int numbits = (int) tfCOUNT;
  const int mask = (1<<numbits)-1;

  cell *get(cell *where) {
    if(emap.count(where)) return emap[where];
    return where;
    }

  int getb(cell *where) { return getBits(get(where)); }
  
  int countBits(int c) {
    int bi = 0;
    for(int i=0; i<numbits; i++) if((c >> i)&1) bi++;
    return bi;
    }
  
  int getBit(int bits, int id) { return (bits >> id) & 1; }

  int getRandomBits() { return hrand(1 << numbits); }
  
  bool seek() { return items[itBabyTortoise] % 5; }
  int seekbits;
  double seekval[numbits];
  double currval[numbits];
  
  void update(double& val, double target, int delta) {
    double d = delta / 300.;
    if(d>1) d = 1;
    if(target>val+d) val += d;
    else if(target<val-d) val -= d;
    else val = target;
    }
  
  void updateVals(int delta) {
    int currbits = getBits(cwt.c);
    for(int i=0; i<numbits; i++)
      update(seekval[i], seek() && !(peace::on && !peace::hint) ? getBit(seekbits, i) : .5, delta);
    for(int i=0; i<numbits; i++)
      update(currval[i], getBit(currbits, i), delta);
    }
  
  double getScent(int bits) {
    double res = 0;
    for(int i=0; i<numbits; i++)
      /* if(getBit(bits, i) != getBit(getBits(cwt.c), i))
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
  
  int diff(int bits) { return countBits(bits ^ tortoise::seekbits); }
  int progress(int bits) { return numbits - diff(bits); }
  
  string measure(int bits) {
    return "(" + its(progress(bits)) + "/" + its(tortoise::numbits) + ")";
    }    
  }

namespace dragon {
 
  int whichturn; // which turn has the target been set on
  cell *target; // actually for all Orb of Control

  void pullback(cell *c) {
    int maxlen = 1000;
    while(maxlen-->0) {
      cell *c2 = c->mov[c->mondir];
      mountmove(c, c->mondir, true, c2);
      c->monst = c2->monst;
      c->hitpoints = c2->hitpoints;
      animateMovement(c2, c, LAYER_BIG);
      c->stuntime = 2;
      if(c2->mondir == NODIR) { c->mondir = NODIR; c2->monst = moNone; return; }
      c = c2;
      }
    }
  
  bool dragbugs = false;
  
  cell *findhead(cell *c) {
    cell *cor = c;
    int maxlen=1000;
    findhead:  
    if(maxlen--<0) return c;
    if(c->monst == moDragonHead) return c;
    for(int i=0; i<c->type; i++)
      if(c->mov[i] && isDragon(c->mov[i]->monst) && c->mov[i]->mondir == c->spn(i)) {
        c = c->mov[i]; goto findhead;
        }
    if(cmode & sm::MAP) return c;
    if(!conformal::includeHistory) {
      printf("dragon bug #3 (%p -> %p)\n", cor, c); 
      dragbugs = true;
      }
    c->monst = moDragonHead; return c;
    }
  
  void validate(const char *where) {
    dragbugs = false;
    for(int i=0; i<size(dcal); i++)
      if(dcal[i]->monst == moDragonTail)
        findhead(dcal[i]);
    if(dragbugs) {
      printf("DRAGON BUG in %s\n", where);
      exit(1);
      }
    }
  
  int bodypart(cell *c, cell *head) {
    int i = 0, j = 0;
    int maxlen = 1000;
    while(maxlen-->0) {
      if(head == c) i = j;
      j++;
      if(head->mondir == NODIR) break;
      head = head->mov[head->mondir];
      }
    if(i == 0) return 'h';
    if(i == 1) return 'l';
    if(i == j-2) return '2';
    if(i == j-1) return 't';
    if(i == 2) return 'w';
    return 0;
    }
  
  void kill(cell *c, eMonster who) {
    int delay = false;
    kills[moDragonHead]++;
    int penalty = 0;
    int maxlen = 1000;
    while(maxlen-->0) {
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
      c = c->mov[c->mondir];
      }
    }
  
  int totalhp(cell *c) {
    int total = 0;
    int maxlen = 1000;
    while(maxlen-->0) {
      if(!isDragon(c->monst)) {
        if(!conformal::includeHistory) printf("dragon bug #4\n");
        return total; 
        }
      total += c->hitpoints;
      if(c->mondir == NODIR) return total;
      c = c->mov[c->mondir];
      }
    return total;
    }
  
  #define SWAPBITFIELD(x,y,t) { t bak=x; x=y; y=bak; }
  
  void pullfront(cell *c, cell *until) {
    int maxlen = 1000;
    static vector<cell*> allcells;
    allcells.clear();
    while(maxlen-->0) {
      allcells.push_back(c);
      // SWAPBITFIELD(c->monst, buffer->monst, eMonster);
      // SWAPBITFIELD(c->hitpoints, buffer->hitpoints, int);
      c->stuntime = 2;
      if(c == until) {
        for(int i=size(allcells)-2; i>=0; i--) {
          cell *cmt = allcells[i+1];
          cell *cft = allcells[i];
          cmt->hitpoints = cft->hitpoints;
          cmt->monst = cft->monst;
          cft->monst = moNone;
          mountmove(cmt, cmt->mondir, true, cft);
          animateMovement(cft, cmt, LAYER_BIG);
          }
        while(c->mondir != NODIR) {
          c = c->mov[c->mondir];
          c->stuntime = 2;
          }
        break;
        }
      if(c->mondir == NODIR) { printf("dragon bug\n"); break; }
      c = c->mov[c->mondir];
      if(!c) { 
        if(!conformal::includeHistory) printf("dragon bug #2\n"); 
        break; 
        }
      }
    }
  
  bool move(cell *dt, cell *df) {
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
        cell *c2 = df->mov[df->mondir];
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

  }

namespace sword {
  int angle[MAXPLAYER];

  cell *pos(cell *c, int s) {
    int t = c->type;
    s *= 2;
    s += S42/t;
    s %= S84;
    if(s<0) s += S84;
    s /= (S84/t);
    return c->mov[s];
    }
  
  eItem orbof(bool rev) { return rev ? itOrbSword2 : itOrbSword; }
  int orbcount(bool rev) { return items[orbof(rev)]; }
  
  cell *pos(int id, bool rev) {
    if(!orbcount(rev)) return NULL;
    return pos(playerpos(id), angle[id] + (rev ? S21 : 0));
    }
  
  bool at(cell *where, bool noplayer) {
    if(noplayer) return false;
    if(!orbcount(0) && !orbcount(1)) return false;
    for(int i=0; i<numplayers(); i++) if(multi::playerActive(i)) for(int b=0; b<2; b++)
      if(pos(i,b) == where) return true;
    return false;
    }
  
  bool near(cell *where) {
    if(at(where, false)) return true;
    forCellEx(w2, where) if(at(w2, false)) return true;
    return false;
    }

  // from c1 to c2
  int shift(cell *c1, cell *c2, int angle) {
    if(!c1 || !c2) return 0;
    int s1 = neighborId(c1, c2);
    int s2 = neighborId(c2, c1);
    if(s1 < 0 || s2 < 0) return angle;
    if(c1->mirror(s1))
      return ((s2*S42/c2->type - angle + s1*S42/c1->type) + S21) % S42;
    else
      return ((s2*S42/c2->type - s1*S42/c1->type) + S21 + angle) % S42;
    }

  void shuffle(int i) {
    sword::angle[i] = euclid ? S7*hrand(6) : nonbitrunc ? 3*hrand(S14)+1 : hrand(S42);
    }
  
  void reset() {
    items[itOrbSword] = items[itOrbSword2] = 0;
    shuffle(multi::cpid);
    }

  void shuffle() {
    for(int i=0; i<MAXPLAYER; i++) shuffle(i);
    }
  };

namespace kraken {

  cell *head(cell *c) {
    if(c->monst == moKrakenH) return c;
    if(c->monst == moKrakenT) return c->mov[c->mondir];
    return NULL;
    }
 
  void kill(cell *c, eMonster who) {
    drawParticles(c, minf[moKrakenH].color, 16);
    c->monst = moNone;
    if(checkOrb(who, itOrbUndeath)) c->monst = moFriendlyGhost;
    if(c->land == laKraken && !c->item) c->item = itKraken;
    kills[moKrakenH]++;
    if(checkOrb(who, itOrbStone)) c->wall = waNone;
    for(int i=0; i<c->type; i++)
      if(c->mov[i]->monst == moKrakenT) {
        drawParticles(c, minf[moKrakenT].color, 16);
        c->mov[i]->monst = moNone;
        if(checkOrb(who, itOrbStone)) {
          if(isWatery(c->mov[i]))
            c->mov[i]->wall = waNone;
          else
            c->mov[i]->wall = waPetrified, c->mov[i]->wparam = moKrakenT;
          }
        }
    }
  
  int totalhp(cell *c) {
    int total = 0;
    for(int i=0; i<c->type; i++)
      if(c->mov[i]->monst == moKrakenT)
        total += c->mov[i]->hitpoints;
    return total;
    }
  
  void trymove(cell *c);
  
  void sleep(cell *c) {
    if(c->monst == moKrakenT) c = c->mov[c->mondir];
    c->stuntime = 1;
    forCellEx(c2, c) c2->stuntime = 1;
    }
  
  void attacks() {
    bool offboat[MAXPLAYER];
    for(int i=0; i<MAXPLAYER; i++) offboat[i] = false;
    for(int i=0; i<size(dcal); i++) {
      cell *c = dcal[i];
      if(c->monst == moKrakenT && !c->stuntime) forCellEx(c2, c) {
        bool dboat = false;
        if(c2->monst && canAttack(c, moKrakenT, c2, c2->monst, AF_ONLY_FBUG)) {
          attackMonster(c2, AF_NORMAL | AF_MSG, c->monst);
          sleep(c);
          }
        else for(int i=0; i<numplayers(); i++) if(playerpos(i) == c2) {
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
    for(int i=0; i<size(dcal); i++) {
      cell *c = dcal[i];
      if(c->monst == moKrakenH && !c->stuntime && !isWateryOrBoat(c)) {
        int qdir = 0;
        cell *ctab[MAX_EDGE];
        forCellEx(c2, c) if(isWatery(c2)) ctab[qdir++] = c2;
        hrandom_shuffle(ctab, qdir);
        while(qdir--) trymove(ctab[qdir]);
        }
      }
    }

  // c is the tentacle which will be the head after the move
  void trymove(cell *c) {
    if(pseudohept(c)) return;
    cell *c2 = c->mov[c->mondir];
    if(!isWatery(c)) return;
    if(againstCurrent(c, c2)) return;
    forCellIdEx(c3, i, c) {
      if(c3->monst && c3 != c2 && !(c3->mondir >= 0 && c3->mondir < c3->type &&
        c3->mov[c3->mondir] == c2))
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
      c3->monst = moKrakenT, c3->mondir = c->spn(i),
      c3->aitmp = sval;
      int i0 = (i+c->spn(c->mondir)-c->mondir+99) % c2->type;
      c3->hitpoints = hpcount[i0];
      acells.push_back(make_pair(c2->mov[i0], c3));
      if(c3->wall == waBoat) {
        addMessage(XLAT("%The1 destroys %the2!", moKrakenH, waBoat));
        c3->wall = waSea;
        }
      if(c3->wall == waStrandedBoat) {
        addMessage(XLAT("%The1 destroys %the2!", moKrakenH, waBoat));
        c3->wall = waNone;
        }
      }
    
    while(size(acells)) {
      // bool found = false;
      for(int i=0; i<size(acells); i++) {
        /* bool noconflict = true;
        for(int j=0; j<size(acells); j++) 
          if(acells[i].second == acells[j].first)
            noconflict = false; */
        /* if(noconflict) */ {
          // found = true;
          indAnimateMovement(acells[i].first, acells[i].second, LAYER_BIG);
          acells[i] = acells[size(acells)-1];
          acells.resize(size(acells)-1);
          i--;
          }
        }
      }
    commitAnimations(LAYER_BIG);
    sleep(c);
    c->aitmp = sval;
    return;
    }
  
  }

bool barrierhept(cell *c) {
  return c->bardir != NOBARRIERS && c->bardir != NODIR;
  }

namespace prairie {

  using namespace fieldpattern;
  
  int getfval(cell *c3) {
    if(barrierhept(c3)) return btspin(fieldval(c3).first, c3->bardir)+1;
    return 0;
    }
  
  void spread(cell *c, cell *from) {
    int rd;
    
    c->LHU.fi.flowerdist = 8;
    c->LHU.fi.walldist = 8;
    c->LHU.fi.walldist2 = 8;
    
    if(torus) {
      c->LHU.fi.rval = 0;
      }
    else if(euclid) {
      int x, y;
      tie(x,y) = cell_to_pair(c);
      c->LHU.fi.rval = (y&15);
      }
    else if(sphere) {
      c->LHU.fi.rval = celldistance(c, cwt.c) + 8 - (nonbitrunc ? 2 : 3);
      }
    else if(weirdhyperbolic) {
      c->LHU.fi.rval = max(celldist(c), 15);
      }
    else {
      if(quotient == 2 || !from) {
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
    
          printf("c = %p bc = %d\n", c, barclose);
        
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
  
  #define RLOW (sphere?(nonbitrunc?7:6):nonbitrunc?4:2)
  #define RHIGH (sphere?(nonbitrunc?8:9):nonbitrunc?11:13)
  
  bool no_worms(cell *c) {
    int rv = c->LHU.fi.rval;
    return rv > RLOW+1 && rv < RHIGH-1;
    }

  bool isriver(cell *c) {
    return c->land == laPrairie && c->LHU.fi.rval <= RHIGH && c->LHU.fi.rval >= RLOW;
    }

  bool mainriver(cell *c) {
    return c->LHU.fi.rval <= 8 && c->LHU.fi.rval >= 7;
    }

  bool nearriver(cell *c) {
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
  
  cell *next(cell *c, int pv=1) {
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
  
  vector<cell*> beaststogen;
    
  void generateBeast(cell *c) {
    int beastdistance = min(beastdist(c, 1), beastdist(c, -1));
    if(hrand(1000) >= 15 * beastdistance + 2 * items[itGreenGrass]) return;
    c->monst = moHerdBull;
    cell *c2 = prev(c);
    if(c2) c->mondir = neighborId(c, c2);
    }   

  void moveAt(cell *c) {
    if(eq(c->aitmp, sval)) return;
    vector<cell*> whirlline;
    whirlline.push_back(c);
    c->aitmp = sval; 
    cell *c2 = prev(c);
    while(c2 && !eq(c2->aitmp, sval)) {
      whirlline.push_back(c2), c2->aitmp = sval;
      c2 = prev(c2);
      // in sphere/quotient geometries, never break before a bull
      if((sphere || quotient) && !c2->monst) break;
      }
    reverse(whirlline.begin(), whirlline.end());
    c2 = next(c); 
    while(c2 && !eq(c2->aitmp, sval)) whirlline.push_back(c2), c2->aitmp = sval, c2 = next(c2);
    int qty = size(whirlline);
    // for(int i=0; i<qty; i++) whirlline[i]->aitmp = sval;
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
        if(cp->monst == moHerdBull && !cp->stuntime) {
          forCellEx(c2, cp) {
            int flags = AF_GETPLAYER | AF_BULL;
            if(c2 != cn) flags |= AF_ONLY_FBUG;
            if(canAttack(c, moHerdBull, c2, c2->monst, flags))
              attackMonster(c2, flags | AF_MSG, moHerdBull);
            }
              
          if(!cn->monst && !isPlayerOn(cn) && passable_for(cp->monst, cn, cp, P_DEADLY))
            moveMonster(cn, cp);
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
          
  void move() {
    sval++;
    for(int i=0; i<size(dcal); i++) {
      cell *c = dcal[i];
      if(isriver(c)) moveAt(c);
      }
    for(int i=0; i<size(beaststogen); i++)
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
  
  void generateTreasure(cell *c) { 
//    if(nearriver(c) && op
    if(enter && nearriver(c) && opposite(c) && thisriver(c)) {
      int hr = hrand(100);
      if(hr == 0 && items[itGreenGrass] >= 10) {
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
  
  void treasures() {
    if(enter && !isriver(cwt.c)) enter = NULL;
    else if(!enter && isriver(cwt.c)) enter = cwt.c;
    if(size(tchoices)) {
      if(lasttreasure && lasttreasure->item == itGreenGrass) {
        if(celldistance(lasttreasure, cwt.c) >= (nonbitrunc ? 7 : 10)) {
          lasttreasure->item = itNone;
          forCellEx(c2, lasttreasure) if(c2->item == itGreenGrass) c2->item = itNone;
          }
        else { tchoices.clear(); return; }
        }
      if(size(tchoices) < 3) { tchoices.clear(); return; }
      lasttreasure = tchoices[hrand(size(tchoices))];
      lasttreasure->item = itGreenGrass;
      for(int i=0; i<size(tchoices); i++) if(isNeighbor(tchoices[i], lasttreasure))
        tchoices[i]->item = itGreenGrass;
      tchoices.clear();
      }
    }  
  }

namespace ca {
  ld prob = .2;
  string carule[8][2];
  
  void init() {
    // hexagonal variant of Game of Life, as suggested by Wikipedia
    for(int i=0; i<8; i++) 
      carule[i][0] = "00100000",
      carule[i][1] = "00011000";
    }

#if CAP_COMMANDLINE
  bool readArg() {
    using namespace arg;
    if(argis("-caprob")) {
      shift(); prob = argf();
      return true;
      }
    if(args()[0] != '-') return false;
    if(args()[1] != 'c') return false;
    int livedead = args()[2] - '0';
    if(livedead < 0 || livedead > 1) return false;
    int nei = -1;
    if(args()[3]) {
      nei = args()[3] - '0';
      if(nei < 0 || nei > 7) return false;
      if(args()[4]) return false;
      }
    shift();
    string s = args(); s += "00000000";
    if(nei == -1) for(int i=0; i<8; i++)
      carule[i][livedead] = s;
    else 
      carule[nei][livedead] = s;
    return true;
    }
#endif
  
  void simulate() {
    if(cwt.c->land != laCA) return;
    vector<cell*>& allcells = currentmap->allcells();
    int dcs = size(allcells);
    bool willlive[dcs];
    for(int i=0; i<dcs; i++) {
      cell *c = allcells[i];
      if(c->land != laCA) return;
      int nei = 0, live = 0;
      forCellEx(c2, c) if(c2->land == laCA) {
        nei++; if(c2->wall == waFloorA) live++;
        }
      int welive = 0; if(c->wall == waFloorA) welive++;
      willlive[i] = carule[nei][welive][live] == '1';
      }
    for(int i=0; i<dcs; i++) {
      cell *c = allcells[i];
      c->wall = willlive[i] ? waFloorA : waNone;
      }
    }
  }

auto ccm = addHook(clearmemory, 0, [] () {
  heat::offscreen_heat.clear();
  heat::offscreen_fire.clear();
  princess::clear();
  mirror::mirrors.clear();
  clearing::bpdata.clear();
  tortoise::emap.clear();
  tortoise::babymap.clear();
  prairie::lasttreasure = NULL;
  prairie::enter = NULL;
  prairie::tchoices.clear();
  prairie::beaststogen.clear();
  mirror::clearcache();
  }) +
  addHook(hooks_removecells, 0, [] () {
    eliminate_if(heat::offscreen_heat, is_cell_removed);
    eliminate_if(heat::offscreen_fire, is_cell_removed);
    eliminate_if(princess::infos, [] (princess::info*& i) { 
      if(is_cell_removed(i->princess) || is_cell_removed(i->prison)) { 
        DEBSM(printf("removing a princess\n");)
        if(i->princess && !is_cell_removed(i->princess)) {
          DEBSM(printf("faking a princess\n");)
          princess::newFakeInfo(i->princess);
          }
        delete i; 
        return true;
        }
      return false; 
      });
    set_if_removed(prairie::lasttreasure, NULL);
    set_if_removed(prairie::enter, NULL);
    eliminate_if(prairie::tchoices, is_cell_removed);
    eliminate_if(prairie::beaststogen, is_cell_removed);
    for(auto& bpd: clearing::bpdata) set_if_removed(bpd.second.root, NULL);
    });

// windcode arrays precomputed for speed

int windcodes18920[] = {164,182,171,158,148,155,163,174,188,205,193,160,167,160,140,128,146,153,159,161,167,180,186,202,190,229,234,24,120,146,165,177,179,169,146,125,113,93,131,157,147,155,158,161,162,157,157,162,173,187,198,176,163,242,204,234,238,249,1,30,77,120,153,165,140,173,173,204,200,165,150,184,130,119,104,86,71,54,70,208,168,141,133,169,158,167,161,161,166,171,142,144,165,153,159,166,175,203,186,216,191,221,168,149,114,70,26,225,241,220,229,244,10,251,7,19,50,85,110,129,148,145,173,180,143,178,174,166,144,54,248,215,170,166,153,134,182,116,118,122,114,96,77,57,69,78,62,18,47,228,189,164,148,126,110,81,217,169,151,156,250,163,168,156,154,170,183,192,149,118,120,138,182,152,158,156,158,164,169,171,202,217,181,243,235,206,182,241,159,149,169,125,97,77,53,28,40,252,234,237,218,4,192,204,220,244,246,24,248,250,1,12,31,4,56,98,120,127,134,129,161,139,156,105,153,209,205,187,148,177,180,177,167,152,174,117,81,44,4,201,232,161,168,162,174,165,153,139,115,160,94,103,120,127,125,129,118,94,66,50,28,40,78,89,50,97,81,76,54,16,10,230,195,174,157,151,162,139,122,98,81,27,58,2,7,245,89,127,132,146,158,249,162,174,191,64,124,230,150,173,193,209,211,224,146,119,174,87,90,93,118,139,203,159,167,164,149,150,157,155,165,172,169,161,159,214,219,195,221,172,11,9,253,253,225,206,187,170,252,166,155,149,141,168,124,101,83,81,68,55,48,38,29,45,26,19,206,254,232,219,199,24,159,174,171,187,208,249,17,219,241,43,29,250,248,241,244,248,1,3,17,30,1,37,63,128,134,147,141,135,124,134,125,173,186,161,114,161,52,50,79,236,241,243,236,163,169,157,150,168,181,186,184,184,171,172,170,149,178,131,108,91,73,64,41,10,216,202,233,154,152,161,177,144,182,176,186,166,152,145,133,106,85,119,112,67,76,85,112,135,129,124,134,132,142,139,125,89,71,42,32,26,45,227,32,71,91,97,112,26,106,89,127,83,88,88,65,30,255,243,220,229,201,174,171,164,154,161,153,146,157,142,139,137,93,78,75,67,36,3,57,255,26,30,48,241,93,105,73,116,121,132,147,161,169,233,232,157,151,180,201,219,6,32,94,129,232,142,159,177,190,207,220,241,237,226,252,170,144,119,95,193,56,60,72,62,70,96,124,136,149,212,223,170,177,183,178,171,149,144,141,144,154,150,157,168,175,184,178,167,139,143,146,208,228,229,232,196,209,231,180,148,6,26,41,37,251,242,232,26,217,212,191,169,160,155,241,251,177,161,174,147,145,144,150,147,167,130,100,75,83,79,65,79,75,40,45,48,49,45,35,46,18,49,73,52,117,48,170,255,239,19,220,221,219,208,184,56,138,133,145,160,155,167,179,185,226,14,46,119,196,237,243,41,83,19,8,1,251,243,232,231,234,249,235,248,245,5,247,7,18,21,21,9,21,29,33,187,158,223,154,168,167,153,128,142,114,116,118,133,115,184,194,170,216,208,209,109,92,162,45,19,13,255,65,241,247,253,255,14,26,82,139,164,157,159,162,167,167,183,144,193,192,188,192,189,197,167,169,173,173,184,191,172,151,179,138,123,108,87,81,98,75,78,70,51,248,29,198,230,213,39,222,157,156,146,140,140,151,176,195,123,187,185,185,182,198,175,159,145,144,146,150,141,96,68,57,83,91,67,50,54,51,54,63,99,129,157,153,124,114,140,89,153,144,133,140,160,158,162,146,58,72,59,60,18,3,6,16,6,75,123,45,170,22,102,98,50,102,98,100,110,149,245,132,108,84,80,166,82,87,90,102,120,108,92,254,11,248,219,228,197,226,205,236,171,147,159,161,163,163,141,154,145,185,165,146,146,144,153,142,143,150,224,57,174,69,68,66,63,69,68,42,15,49,226,64,245,27,231,49,47,43,37,50,232,90,102,100,103,71,106,117,113,117,126,136,148,165,177,199,217,228,206,63,107,246,124,217,215,225,225,230,2,15,238,13,1,171,134,132,221,135,147,165,185,191,179,202,212,227,244,18,9,248,240,1,242,24,178,161,192,143,125,108,96,79,209,53,37,29,34,64,38,51,53,68,86,116,140,130,136,154,221,220,233,189,190,173,194,193,191,191,164,189,149,144,145,141,140,129,128,143,139,154,145,159,173,173,174,186,203,193,183,158,177,120,121,126,116,129,207,201,214,20,239,229,239,251,207,225,198,248,165,195,113,1,14,253,31,51,79,73,78,242,201,238,243,210,114,212,220,226,209,189,167,147,145,156,142,221,227,214,194,199,171,152,173,137,138,142,134,134,154,165,166,152,173,166,141,118,88,64,35,101,91,76,72,15,78,103,163,251,11,33,45,56,42,52,42,49,57,23,58,2,244,87,20,93,76,72,73,133,137,101,149,178,255,4,246,224,40,203,213,206,221,224,220,204,165,185,86,95,108,115,124,117,127,127,138,148,146,161,165,163,165,201,152,243,35,54,79,118,160,201,238,20,248,228,221,138,8,125,14,10,24,25,255,253,234,231,221,224,221,223,224,233,254,223,7,236,227,232,21,241,254,236,15,26,22,18,13,10,28,16,9,19,19,12,254,228,205,183,163,254,150,161,188,194,194,173,160,131,118,147,119,108,97,93,114,215,54,99,105,176,197,196,209,164,246,238,229,238,0,23,54,85,135,158,162,35,13,49,249,251,243,244,233,38,224,253,0,1,2,254,10,25,34,59,92,114,146,167,181,171,126,156,163,161,167,185,136,177,193,199,109,207,201,196,188,183,193,206,204,194,210,169,158,161,169,172,171,190,159,209,214,199,174,153,140,165,183,143,143,120,114,115,83,84,83,75,102,72,79,90,87,90,81,73,254,228,49,202,180,148,66,208,49,58,209,215,155,168,166,169,144,49,109,208,129,126,172,199,215,215,122,94,183,183,193,184,187,188,195,196,212,181,166,152,131,135,141,142,143,152,185,246,69,117,77,26,31,37,60,63,66,66,52,45,43,40,38,51,28,31,35,73,105,127,150,177,177,169,132,114,112,106,153,47,244,89,180,142,120,125,138,155,172,179,172,179,191,196,247,39,34,56,97,30,88,29,250,235,220,235,245,2,7,252,61,78,99,125,37,165,135,15,5,150,130,111,112,221,102,97,113,101,104,90,114,202,213,217,221,126,122,168,119,84,52,69,73,216,78,88,91,80,100,85,125,149,147,135,142,165,198,4,41,13,235,216,219,212,220,198,239,232,217,189,241,156,128,101,150,154,154,155,174,164,160,126,129,137,146,139,228,212,230,133,137,141,144,145,143,147,156,141,140,142,149,157,208,242,16,58,171,67,64,63,68,62,51,52,80,78,77,46,58,50,245,51,186,184,65,79,195,201,47,50,219,71,69,54,64,54,24,29,55,39,237,234,99,106,87,119,107,97,96,95,72,93,114,132,66,94,146,114,126,131,135,139,151,175,180,166,197,248,204,220,197,233,177,25,35,63,86,15,80,44,3,222,224,238,247,227,221,238,239,9,18,22,216,6,247,24,217,178,148,131,125,131,202,211,130,131,139,156,174,185,184,193,201,187,205,205,212,221,233,249,8,37,58,18,6,1,236,248,7,0,9,4,86,193,180,170,159,208,135,125,153,110,103,96,82,69,74,217,221,50,42,52,25,15,255,5,11,57,33,40,44,45,50,45,56,71,108,144,168,161,90,117,125,139,157,216,226,217,233,239,193,205,205,206,178,209,201,196,191,197,203,212,182,151,213,150,159,146,137,130,142,146,134,142,126,100,113,146,166,110,119,170,149,128,121,187,176,177,169,169,178,188,210,232,211,197,195,189,178,152,185,115,110,109,102,108,96,89,95,117,233,216,173,116,231,51,2,69,213,220,234,0,14,25,226,210,232,213,245,195,7,243,207,71,203,83,252,245,4,18,252,31,50,24,81,119,108,97,103,97,242,230,174,204,154,2,25,75,167,189,124,210,223,229,242,227,240,209,188,169,135,145,115,130,143,129,157,134,202,212,197,216,174,242,230,162,228,195,166,150,143,166,131,130,135,131,139,124,120,110,139,168,180,191,176,164,161,180,187,161,138,132,155,114,77,55,28,250,1,151,115,188,93,77,65,64,57,221,63,92,131,199,214,165,238,254,16,42,39,56,72,56,72,22,52,20,31,44,52,64,82,99,196,79,237,249,220,187,130,244,49,67,133,75,77,78,75,89,136,135,149,124,164,113,190,227,3,212,24,10,250,230,205,124,192,185,199,215,194,226,228,228,230,237,221,205,159,161,192,100,101,118,85,92,96,99,115,118,121,106,120,114,121,128,132,153,142,163,166,168,163,155,143,128,141,181,208,129,233,17,62,74,69,60,80,123,146,179,199,233,204,30,57,78,169,237,196,219,213,195,160,43,134,144,34,0,15,18,14,35,45,57,42,216,9,227,235,223,207,218,210,223,213,211,216,219,217,217,242,213,6,215,22,248,34,218,210,203,214,224,43,237,230,241,4,238,21,31,44,27,22,20,14,14,3,251,35,43,22,7,14,253,21,27,4,246,6,237,243,222,9,210,200,193,177,156,39,122,144,165,219,223,187,224,216,191,175,159,171,127,117,141,111,149,133,124,124,106,89,78,84,74,111,14,228,52,80,71,108,141,216,157,214,197,189,200,216,158,251,19,13,251,234,248,254,18,23,17,4,218,177,1,181,154,148,168,65,36,255,241,46,234,236,234,232,228,232,229,219,219,14,7,213,185,83,21,254,255,9,0,4,246,6,18,36,36,37,43,80,107,110,115,177,182,126,193,199,195,202,71,27,142,147,163,169,155,175,170,169,201,111,145,100,211,208,210,206,213,79,222,214,207,196,188,203,181,171,180,209,219,222,217,207,217,201,220,172,163,179,145,144,150,171,184,170,145,170,191,213,112,238,234,228,225,222,197,173,150,131,126,166,122,194,160,167,130,166,73,100,117,133,135,64,76,73,85,88,86,77,64,107,63,75,86,106,97,100,96,102,103,100,93,106,230,222,2,208,100,206,206,202,179,147,121,97,79,202,46,33,71,212,194,209,148,163,159,191,179,170,166,179,160,244,26,79,119,209,134,123,102,82,252,216,219,234,233,229,89,123,73,184,175,177,179,192,185,191,175,188,179,192,224,217,197,206,245,183,170,163,159,145,132,120,116,135,141,147,145,139,134,131,178,244,28,43,30,177,168,202,244,252,251,0,57,12,66,46,37,39,66,60,42,53,31,34,36,50,49,38,26,29,24,63,18,1,2,21,51,2,84,116,132,144,167,158,201,198,184,191,180,131,113,148,98,105,107,114,116,166,73,37,5,223,104,192,163,133,111,87,101,118,134,155,164,187,185,186,189,190,169,201,210,196,231,23,21,30,26,36,15,7,228,152,153,32,187,3,11,201,209,23,199,186,204,219,249,235,253,1,248,252,251,66,69,56,77,87,95,108,115,23,127,162,109,254,2,249,187,165,198,138,112,107,119,128,209,108,98,91,90,126,98,117,147,68,59,72,47,239,227,15,205,204,211,101,112,120,125,10,139,120,132,20,4,52,71,63,65,247,233,75,84,95,100,98,65,56,85,116,85,138,150,174,177,178,162,159,148,150,154,154,2,141,30,80,111,199,236,218,217,217,215,218,223,219,226,230,199,248,243,249,238,230,230,208,179,251,151,118,77,253,176,33,162,153,147,149,148,152,195,175,195,161,127,100,103,125,120,145,142,104,63,136,242,245,1,47,5,96,125,140,137,136,142,149,150,143,145,153,144,155,135,137,145,137,138,148,154,145,153,220,235,189,1,255,238,64,78,151,71,62,58,54,70,56,79,70,35,28,60,16,89,86,95,91,106,33,57,34,76,79,86,79,145,49,158,164,146,78,70,97,142,161,164,165,139,105,71,54,252,56,100,97,84,44,73,79,105,10,243,5,21,44,57,34,236,246,236,101,102,108,120,88,139,122,135,104,86,89,95,91,84,77,69,75,84,123,143,162,223,17,62,94,182,116,123,130,133,132,127,128,134,138,141,235,209,143,185,146,111,23,17,15,187,191,210,227,188,243,241,151,145,29,11,35,11,35,48,65,76,28,85,64,32,36,214,24,205,211,254,5,6,21,225,225,226,218,236,232,2,209,26,26,26,27,38,189,24,255,244,236,44,228,204,175,152,137,131,117,111,126,136,195,183,207,124,116,136,120,134,146,165,188,179,186,181,195,179,184,198,207,211,202,209,201,200,218,222,205,222,226,254,16,12,18,51,73,30,108,13,10,26,5,48,213,208,239,8,5,36,6,33,179,236,142,195,185,210,178,175,172,162,162,227,148,119,123,114,169,99,97,105,105,97,81,69,75,67,70,217,222,228,43,54,55,33,47,20,10,20,249,233,233,235,234,13,48,55,51,47,34,35,40,47,40,38,41,42,33,44,55,77,104,155,179,191,196,243,87,107,41,116,117,125,139,168,151,196,209,226,239,198,244,250,9,239,202,209,192,224,211,214,219,221,186,218,211,202,198,188,198,182,204,211,217,238,233,185,160,213,141,245,164,174,130,178,159,143,134,126,107,133,145,156,153,125,150,149,161,73,57,75,87,184,184,199,80,83,92,146,153,226,146,122,85,19,187,214,179,182,170,186,178,165,145,155,171,198,209,245,254,230,14,214,179,191,220,179,186,200,178,197,153,192,114,123,102,93,101,109,101,77,83,82,92,61,72,60,102,5,246,1,6,177,157,135,98,249,61,70,86,170,70,180,194,225,224,224,243,34,40,23,68,222,239,206,243,238,227,203,2,194,190,13,16,23,251,228,207,53,197,199,68,90,2,235,10,220,243,4,13,19,1,22,22,39,65,21,81,124,174,146,130,114,110,97,110,99,229,239,221,163,161,163,191,146,47,45,34,62,88,123,159,172,191,125,114,218,227,233,228,236,255,1,249,230,252,216,202,187,175,188,141,72,179,155,14,111,122,131,142,113,157,175,116,138,187,195,199,205,182,211,205,177,160,73,20,5,1,127,239,217,2,189,155,136,134,145,140,152,186,119,120,130,120,134,139,133,124,145,118,114,104,77,114,82,152,184,196,205,225,223,150,160,163,169,182,195,201,175,215,131,134,128,137,141,174,160,79,67,61,47,23,247,236,227,233,187,170,144,111,199,86,75,98,60,53,61,53,30,38,200,209,49,65,84,118,146,218,224,211,219,131,226,236,250,7,10,11,89,31,1,92,97,72,78,67,104,252,242,137,218,13,242,1,23,51,80,70,44,77,91,103,133,209,189,76,110,230,235,221,236,210,190,163,221,152,250,213,7,39,70,173,75,73,89,88,85,73,67,131,49,153,98,146,144,143,142,146,246,66,209,63,202,197,194,246,28,171,44,34,46,14,248,241,212,188,182,140,115,191,178,184,160,178,194,208,221,190,232,238,233,223,230,227,242,5,248,212,215,203,127,131,151,203,235,94,88,116,90,136,30,70,53,102,94,83,79,84,142,116,126,124,122,104,118,110,101,107,114,119,108,111,147,161,147,138,182,175,176,170,174,162,153,143,126,135,96,106,107,105,158,200,192,197,116,217,1,37,97,106,66,116,68,41,37,24,119,137,140,147,157,205,212,190,156,64,230,56,57,73,86,105,119,144,222,255,174,186,205,143,216,220,219,191,164,95,138,142,143,252,55,237,26,12,7,15,26,56,62,26,66,78,87,126,169,205,27,19,229,245,251,231,213,194,191,207,226,203,237,222,202,198,197,207,216,215,219,215,190,197,229,254,200,13,9,210,221,55,37,4,233,58,193,192,212,185,185,196,205,231,213,61,53,240,226,249,213,224,232,247,17,251,34,27,54,56,32,85,19,21,26,16,7,12,20,25,245,230,233,48,47,38,49,24,2,9,13,16,250,19,35,61,21,236,235,222,233,41,207,12,247,230,219,48,207,203,215,209,213,179,189,194,95,89,80,71,116,144,151,162,212,247,2,0,178,255,232,238,217,188,175,177,171,154,184,140,124,112,103,152,123,106,153,138,136,147,148,133,132,93,80,116,65,54,80,85,72,78,133,105,19,4,35,235,63,44,31,100,2,130,133,105,58,251,168,214,204,221,182,171,186,203,222,212,163,150,252,15,247,49,52,46,7,248,236,248,247,3,40,40,25,40,16,251,228,201,213,184,227,173,238,183,140,135,165,134,186,99,59,129,35,23,194,217,221,234,233,228,226,225,227,220,218,215,226,230,227,218,206,208,216,7,7,0,226,193,227,143,106,44,3,245,242,243,31,233,24,253,246,253,7,237,250,2,27,53,45,47,39,49,21,26,116,116,126,118,117,109,87,203,202,205,204,30,211,208,209,210,200,202,234,30,53,10,117,126,132,136,158,173,174,174,144,170,186,165,176,151,215,82,91,112,122,82,22,239,50,217,223,214,209,219,231,78,63,241,226,219,216,212,201,199,190,180,206,185,173,153,154,151,201,250,225,230,225,229,230,215,209,221,208,212,218,225,183,170,171,165,206,123,127,138,124,131,171,191,13,218,3,87,136,159,108,173,183,197,215,236,66,7,17,241,228,239,214,248,244,219,195,180,163,150,132,110,106,104,231,65,204,81,205,140,188,195,198,109,220,243,3,62,102,110,125,166,170,152,39,46,64,74,44,95,95,82,93,90,86,67,44,49,17,207,49,67,79,82,91,120,121,107,97,110,96,101,112,113,118,116,121,122,110,129,221,214,209,202,180,191,188,149,114,194,199,235,236,202,91,126,14,130,119,84,84,88,88,204,194,30,16,64,3,91,253,179,226,170,202,162,111,136,183,184,226,198,223,181,172,159,156,185,160,200,186,231,246,226,6,47,88,117,145,214,154,134,113,98,66,107,33,8,236,221,229,221,1,243,243,251,232,63,87,61,97,68,181,176,186,164,165,175,179,177,170,164,191,202,200,163,191,180,166,157,117,28,3,220,209,196,36,122,18,179,170,189,155,153,176,153,137,132,129,123,138,118,103,76,114,155,151,155,150,151,123,117,160,121,115,23,57,70,84,33,230,194,191,190,205,209,225,245,247,241,220,222,212,100,81,176,84,56,84,30,25,11,13,12,73,49,62,26,34,60,9,46,23,6,38,58,67,45,75,23,5,19,25,16,13,67,75,19,21,197,223,234,248,15,29,61,237,83,101,134,140,148,146,151,169,176,165,194,223,229,213,182,186,197,191,192,148,133,110,93,156,82,96,80,100,97,112,128,131,143,163,182,80,53,99,32,15,251,245,217,119,195,177,151,125,106,133,75,52,64,93,113,121,127,145,172,180,152,197,199,188,203,183,185,213,205,194,151,214,226,236,209,181,154,58,34,28,44,39,23,16,19,44,6,249,18,229,213,197,178,181,186,38,23,208,226,245,0,14,119,165,181,185,55,180,178,141,175,199,193,204,250,2,245,237,0,6,254,239,239,242,249,4,80,67,62,68,71,80,86,73,90,89,97,118,98,111,243,251,97,126,94,159,87,246,249,245,246,240,210,206,195,187,210,61,95,218,89,91,120,138,140,145,208,204,98,95,122,98,91,80,76,76,146,89,112,139,197,239,167,25,37,59,63,48,61,253,237,15,233,38,192,194,198,197,203,107,113,70,103,109,117,126,132,24,142,156,165,191,135,219,237,221,211,125,90,79,45,42,57,236,248,236,83,83,72,83,101,113,113,98,113,46,32,72,20,55,81,107,138,91,150,149,167,187,205,180,208,195,163,130,143,156,145,123,109,127,89,50,105,33,255,167,134,146,166,198,227,246,213,211,213,230,227,216,208,209,221,252,238,218,243,246,9,201,0,252,252,251,4,236,228,248,235,7,230,203,183,164,10,1,151,140,113,60,25,251,236,212,181,4,165,156,152,147,134,139,147,153,139,157,228,208,187,169,30,154,156,90,127,118,66,62,83,129,124,158,96,184,225,55,79,56,80,131,135,247,242,241,245,0,13,31,64,221,96,111,133,152,140,153,131,126,138,147,156,158,161,144,135,142,155,164,155,152,139,155,139,115,132,138,156,135,130,130,144,153,154,165,112,124,187,213,234,239,245,170,27,23,3,243,193,146,106,99,82,134,127,70,68,60,48,56,49,22,66,89,56,95,98,120,24,223,142,241,104,88,84,93,114,106,100,124,131,5,0,55,77,21,82,101,80,106,108,96,106,132,121,39,66,135,163,128,153,130,65,76,95,86,131,102,109,122,139,74,186,169,161,147,128,99,69,43,49,43,12,224,136,20,122,116,125,111,88,37,52,69,111,161,188,132,212,210,229,252,32,13,40,45,35,59,33,232,236,233,246,236,116,106,106,99,99,110,123,131,85,158,158,141,125,145,107,86,68,71,81,96,101,94,80,70,75,75,77,66,66,62,34,203,160,166,182,184,210,234,194,255,31,57,78,102,184,136,127,127,133,137,133,144,137,126,130,143,142,117,121,72,44,0,221,113,202,169,133,110,82,54,23,20,30,13,169,171,190,177,197,211,222,234,223,3,251,252,107,67,116,56,63,237,240,54,7,22,240,32,39,52,62,67,88,19,27,107,93,72,41,15,41,64,123,185,15,202,180,114,34,26,19,21,49,14,56,216,225,233,228,214,214,223,209,234,200,229,0,19,180,46,41,36,35,24,25,36,61,53,165,171,35,9,50,243,237,233,239,242,69,243,225,201,159,144,187,130,126,132,123,104,94,89,114,125,140,152,185,176,199,168,208,126,136,97,92,147,105,126,140,134,136,158,178,187,199,185,199,182,176,176,213,175,171,181,180,204,220,222,220,215,221,212,217,207,192,189,190,211,225,235,225,199,213,211,210,228,8,6,21,36,51,76,107,23,151,140,2,0,252,251,30,60,9,91,119,80,152,181,227,251,34,11,20,10,62,95,48,196,62,169,197,164,251,164,208,189,184,182,229,173,171,182,177,190,159,151,178,162,251,251,159,120,178,77,109,136,122,106,192,86,81,85,121,117,107,115,104,81,62,62,53,85,78,80,66,60,206,207,217,213,233,28,35,37,75,69,71,65,247,43,32,23,10,254,18,237,226,218,219,215,214,203,193,236,27,45,44,49,92,73,69,51,18,12,42,32,39,56,49,52,37,27,28,37,31,39,13,16,30,56,66,38,59,21,204,203,199,198,215,246,66,103,116,117,27,114,112,110,113,115,126,139,157,179,144,178,193,163,203,218,230,241,1,108,6,1,9,251,42,219,202,212,207,225,194,253,222,245,198,218,233,234,236,226,197,196,222,219,220,210,197,202,210,191,167,191,143,187,227,217,219,218,225,2,10,246,210,188,159,139,241,170,154,9,20,119,210,200,195,111,190,176,196,154,134,131,124,115,115,102,84,128,132,128,145,161,175,168,158,116,147,153,164,200,234,184,17,34,43,50,36,7,227,195,192,209,242,52,59,47,56,253,65,120,145,160,24,164,148,128,98,72,35,240,190,181,203,162,196,186,186,152,195,184,197,184,204,111,100,55,231,156,174,213,193,173,3,35,37,15,230,241,51,10,174,149,151,151,174,100,218,225,204,182,209,160,182,206,193,100,104,138,168,92,57,83,11,104,122,117,95,35,56,76,50,81,97,57,106,24,38,37,56,24,57,38,31,248,1,21,48,105,18,150,160,147,122,79,98,9,251,47,66,52,76,82,98,126,158,62,173,172,153,248,224,56,217,209,214,240,88,84,18,29,101,29,117,224,238,211,251,193,236,237,254,246,246,245,214,180,19,191,196,178,11,24,13,17,6,29,250,242,233,211,191,41,197,187,200,82,58,106,226,209,195,221,192,249,14,15,16,22,21,19,20,18,9,11,25,67,74,20,77,63,227,201,183,215,159,132,121,118,123,125,86,105,81,118,85,221,231,221,234,216,175,166,165,155,146,131,131,97,146,59,72,71,64,67,65,73,87,107,124,141,160,167,167,187,105,118,103,240,233,235,245,251,223,221,249,230,10,4,9,8,13,16,249,231,10,216,210,203,180,164,197,155,247,240,45,23,34,182,171,214,161,249,83,107,90,125,128,130,125,124,96,243,76,225,117,99,144,172,182,176,210,187,192,198,198,178,202,208,197,176,191,143,80,41,97,2,11,22,24,28,114,3,241,232,218,26,203,172,143,125,105,115,122,152,153,141,181,138,216,111,105,114,104,136,118,135,96,145,148,138,129,118,113,155,155,118,117,108,104,94,41,38,79,125,63,155,182,200,204,216,214,221,240,13,70,143,152,124,156,159,169,173,222,217,201,206,206,223,179,243,247,84,84,104,114,125,115,130,145,143,157,226,210,2,53,65,63,73,60,53,29,246,215,213,230,228,226,221,215,208,189,203,170,181,158,120,95,198,101,81,68,65,100,47,39,37,54,70,67,41,244,3,28,188,188,200,53,51,47,50,70,110,146,156,173,9,240,44,245,232,202,185,240,56,211,215,233,3,5,246,18,193,220,129,213,132,190,33,137,73,81,91,107,155,128,242,3,205,183,139,210,246,227,32,202,220,235,3,6,61,134,100,77,71,224,74,86,97,101,108,110,102,224,212,15,84,74,138,223,234,231,226,206,226,222,196,204,217,193,158,161,241,189,174,8,14,194,226,196,9,17,9,51,100,204,99,71,44,80,113,122,104,96,71,67,67,47,209,166,5,65,239,55,154,151,149,141,153,127,128,153,152,237,4,26,39,217,34,35,207,199,194,183,196,165,148,57,41,64,157,49,55,59,51,69,33,242,237,249,11,230,183,144,214,160,130,141,104,164,242,186,153,171,136,155,132,174,199,206,211,222,231,175,204,235,240,254,251,229,217,214,224,234,233,244,4,31,37,49,209,186,218,234,183,118,110,92,45,75,224,4,24,244,110,81,70,70,135,81,69,153,152,245,238,3,107,215,121,103,129,89,85,58,54,90,57,178,154,116,128,111,141,131,127,125,122,117,120,113,97,95,98,87,94,110,134,111,76,78,53,147,164,162,168,127,119,160,125,198,174,175,186,189,174,188,173,154,155,150,140,133,116,114,148,76,76,81,87,94,81,74,85,247,229,190,175,170,186,116,128,202,226,12,31,45,90,122,135,151,63,174,171,246,194,155,138,140,145,148,149,143,125,251,35,211,201,249,200,170,135,99,69,4,59,58,68,77,91,74,111,124,111,110,133,108,235,0,198,21,153,168,186,194,205,147,223,228,216,246,245,216,189,149,171,136,109,133,141,132,152,129,221,216,230,137,224,45,27,40,255,253,250,255,2,37,34,85,92,76,14,67,75,88,93,100,110,131,153,182,209,30,39,26,237,254,226,19,20,246,219,205,203,178,180,192,173,185,199,222,248,206,2,242,214,188,181,198,181,189,195,209,221,225,211,216,209,226,210,159,164,180,174,206,233,254,15,195,8,8,16,221,209,237,85,68,78,51,20,253,239,222,75,195,172,171,174,212,165,167,173,188,190,190,194,220,239,203,64,73,55,4,242,229,209,13,200,211,194,220,228,232,246,33,45,18,3,53,42,20,245,149,85,56,34,15,120,3,14,24,23,9,10,8,24,45,11,81,170,194,200,239,206,55,58,56,51,44,49,46,32,12,3,3,2,12,17,14,14,13,6,4,26,55,100,108,82,200,219,224,221,197,208,133,64,175,80,179,13,15,21,240,233,224,221,219,117,214,206,197,198,220,210,244,56,140,150,194,237,239,249,36,122,129,111,32,59,39,155,155,146,146,160,149,219,243,188,17,22,38,31,35,161,49,225,237,3,248,255,221,178,163,188,142,203,146,132,157,119,114,106,94,76,172,138,148,84,113,192,119,133,140,141,158,164,173,160,144,150,103,84,70,59,119,50,46,22,31,120,109,88,80,87,72,146,155,167,23,14,26,12,37,228,248,56,99,117,201,248,250,122,189,224,148,161,99,110,87,74,45,238,201,225,219,196,209,163,153,147,167,179,195,206,215,221,209,153,164,143,252,238,254,30,254,82,85,57,97,101,4,248,236,242,248,225,0,247,235,241,1,50,53,56,62,35,82,39,0,7,249,214,210,210,200,210,149,110,13,235,165,217,212,176,185,146,122,119,116,195,131,113,212,204,143,118,79,38,186,26,44,17,88,140,185,205,216,205,228,2,242,233,233,220,221,218,218,218,224,217,209,203,211,221,199,231,235,228,222,216,205,189,197,196,206,221,1,10,8,28,34,246,251,224,169,238,132,133,128,104,255,250,232,239,249,222,198,148,79,85,218,199,234,124,238,234,240,255,21,25,222,244,206,227,239,14,42,66,67,56,48,58,27,31,64,134,136,135,126,144,120,122,122,103,46,77,201,196,230,208,221,231,219,215,2,221,223,215,207,221,204,238,171,184,194,2,234,9,32,11,42,12,111,92,100,111,126,135,155,117,175,185,182,183,171,185,133,141,162,182,153,201,152,136,140,222,138,229,53,67,71,79,92,104,117,96,84,55,59,31,234,49,194,209,255,236,216,199,201,220,239,251,72,87,56,1,238,6,221,218,222,221,212,220,200,201,202,206,200,191,177,159,203,227,189,156,136,138,141,133,122,119,42,45,38,232,242,206,250,145,245,247,227,247,223,211,209,211,216,222,214,222,214,213,220,183,169,205,160,166,178,186,189,239,106,110,112,115,100,106,98,76,170,190,194,207,241,28,217,57,29,88,108,134,161,171,83,175,184,170,182,187,225,230,239,1,64,53,28,34,56,74,169,220,227,242,5,200,19,11,8,243,185,170,157,154,137,156,117,94,79,83,91,83,220,220,1,48,207,115,44,202,206,98,194,100,222,217,220,214,227,91,245,241,242,228,219,235,129,108,105,109,136,115,195,193,184,211,168,8,22,16,33,54,66,68,70,11,92,108,105,101,76,98,91,88,94,100,89,39,14,32,39,246,1,233,36,55,36,82,86,85,78,100,80,141,136,129,136,123,109,93,91,118,94,96,104,126,123,113,121,147,128,144,100,145,242,223,200,200,199,200,198,186,152,169,171,171,192,154,32,184,167,199,117,20,9,1,13,209,51,79,96,116,23,133,148,193,37,58,73,86,101,97,93,205,213,186,45,11,253,240,102,237,226,118,99,250,40,99,144,230,155,141,233,153,67,65,89,99,161,223,10,2,221,190,251,177,178,178,165,145,137,135,162,186,145,244,218,36,225,221,226,233,233,252,24,235,61,98,125,147,170,174,245,216,182,158,142,117,128,139,48,35,21,22,21,250,228,220,215,228,244,215,16,56,237,245,233,12,249,205,241,20,49,71,82,54,81,87,88,79,202,172,164,186,186,78,133,238,163,183,184,181,176,183,162,134,106,200,130,190,205,225,219,207,157,187,181,175,152,151,165,136,105,82,60,69,226,202,208,217,205,195,53,77,40,106,35,185,176,171,159,194,144,127,72,228,206,171,137,110,146,108,131,127,122,120,147,126,109,127,79,36,11,190,177,164,168,179,160,153,148,151,162,205,37,83,104,180,119,128,40,80,93,62,115,122,124,202,214,210,188,192,199,200,196,195,198,204,213,210,244,255,242,245,248,228,181,194,198,177,187,128,102,143,87,153,124,104,68,42,90,9,17,17,245,246,250,237,70,91,51,53,50,66,7,27,41,16,71,247,57,42,58,15,254,222,205,115,65,90,85,58,34,93,16,255,233,242,25,36,28,8,0,2,73,70,80,25,48,6,97,146,183,207,231,222,222,245,226,31,26,13,19,97,224,82,72,117,153,152,159,157,161,153,146,157,143,172,169,178,182,178,184,213,173,245,254,14,245,218,168,180,178,195,215,209,206,160,149,157,135,122,95,82,72,151,61,69,90,99,72,103,94,78,94,113,131,154,161,144,170,172,149,202,105,92,60,34,113,31,34,22,8,255,211,195,115,128,197,191,181,164,136,138,131,108,81,117,51,35,15,38,113,114,117,124,135,116,146,193,210,226,108,63,191,206,211,211,185,224,218,180,181,192,204,175,222,190,164,117,251,245,250,7,229,10,199,177,151,104,45,61,34,29,59,48,50,33,55,14,3,253,13,25,62,19,4,243,228,25,211,209,207,210,204,193,189,190,187,198,10,55,221,224,212,232,250,246,247,21,250,137,154,97,166,168,169,172,166,55,167,183,182,90,79,100,240,253,9,248,18,255,208,236,217,16,22,20,7,250,233,224,227,233,2,253,12,103,82,115,51,59,68,163,75,84,88,93,71,94,88,89,76,57,137,141,87,81,109,232,204,246,93,120,76,166,140,74,76,235,239,251,239,243,248,229,246,232,219,216,210,208,201,204,245,93,87,81,215,68,70,66,97,132,154,165,145,152,155,203,207,203,108,75,78,89,145,112,96,104,84,78,59,57,54,71,170,167,81,93,113,134,191,209,230,255,12,18,20,41,67,72,65,69,55,75,21,254,238,230,35,235,232,47,49,188,183,181,184,188,201,190,184,200,108,124,124,129,68,75,94,89,107,114,125,135,134,138,34,2,147,144,140,166,179,187,182,187,128,226,203,190,220,161,132,106,104,116,63,5,16,14,33,54,220,229,237,235,243,84,91,91,81,78,63,75,116,126,132,126,129,108,96,135,59,37,11,250,84,254,29,235,59,85,96,126,162,166,104,100,164,145,133,158,188,170,150,241,240,193,240,218,131,102,115,108,159,178,158,130,87,99,69,83,83,173,153,67,70,77,86,57,244,208,162,151,178,148,156,162,182,208,201,216,250,212,255,215,226,195,186,222,251,243,222,2,197,197,240,214,21,30,9,229,10,221,9,30,25,7,42,173,6,1,10,253,249,1,6,7,18,245,206,0,163,69,45,3,232,215,205,180,168,150,16,27,250,156,152,147,155,194,14,14,249,250,237,236,245,233,229,214,195,177,245,162,155,149,154,157,146,139,114,120,134,155,142,164,145,168,134,167,231,217,6,221,202,183,174,160,43,149,134,81,137,136,142,131,222,11,19,50,68,131,93,92,183,192,86,209,219,236,0,24,69,104,89,133,86,128,129,129,251,6,242,234,225,245,254,237,24,37,54,87,202,173,115,105,100,120,146,170,161,145,135,178,126,119,108,127,165,165,170,175,148,181,129,128,123,149,168,179,135,210,177,150,148,127,131,165,160,170,88,162,140,143,146,164,139,120,119,119,141,122,156,166,152,152,192,165,62,97,16,230,231,236,211,249,248,249,241,241,119,19,46,72,53,254,222,208,170,154,136,130,120,113,98,78,120,114,61,71,61,75,76,31,19,67,90,68,29,57,226,222,146,115,110,53,94,100,121,174,201,149,193,177,218,154,117,101,124,83,70,69,92,107,133,99,120,106,126,132,135,234,231,250,150,102,95,6,61,45,132,130,88,126,115,132,111,89,90,103,123,126,117,64,31,89,182,186,104,104,195,138,134,19,51,72,88,181,108,102,170,149,83,99,105,95,102,121,196,175,159,163,148,142,125,110,82,57,33,39,35,44,68,8,245,203,170,142,26,113,118,128,132,150,142,128,131,40,44,43,19,26,247,153,155,196,187,194,191,191,184,201,209,229,250,38,55,255,39,44,36,37,22,78,50,5,49,231,228,226,231,226,241,234,221,239,139,105,105,91,88,100,86,113,131,138,124,134,71,74,182,179,167,176,157,145,141,136,157,74,48,51,62,53,75,100,110,116,108,100,86,71,64,55,72,70,83,68,100,59,63,73,66,49,232,185,144,154,195,194,188,205,188,206,218,227,179,3,14,238,36,64,73,78,88,125,175,185,174,171,98,119,132,139,127,134,150,139,164,153,122,124,143,117,169,222,63,96,124,111,74,79,73,84,48,21,245,229,144,218,189,158,112,100,133,87,74,53,33,9,15,19,35,9,50,4,171,149,146,153,202,175,191,159,221,227,240,228,228,20,27,12,18,3,6,89,86,95,72,91,62,73,75,95,144,189,213,213,108,243,26,213,24,30,30,39,52,59,53,41,81,109,225,185,65,124,119,130,111,76,44,17,245,60,55,31,52,78,118,166,201,218,222,127,82,66,101,44,33,31,31,248,22,87,56,100,17,156,204,213,203,225,240,248,239,233,194,209,195,181,218,179,207,166,230,250,15,20,30,149,54,62,59,53,41,47,28,12,17,19,29,46,78,86,62,156,145,166,60,48,18,250,69,231,227,237,236,244,250,254,81,92,254,2,228,213,204,183,138,127,122,197,113,115,113,131,93,76,67,74,117,48,113,116,138,160,171,172,192,180,167,159,207,167,156,215,204,99,132,107,252,45,67,191,105,122,136,159,135,156,113,117,151,174,192,191,181,194,183,191,207,186,223,172,173,166,167,174,238,194,169,156,154,189,159,187,215,227,244,240,236,223,216,218,224,235,229,220,224,202,179,174,175,186,176,212,214,206,215,255,5,238,220,196,202,195,200,202,204,202,254,2,255,2,28,33,37,49,63,80,105,139,19,174,178,163,238,229,249,235,226,4,45,75,103,27,89,103,105,106,92,103,132,151,217,242,248,2,26,61,3,32,37,30,104,163,62,202,195,103,90,158,157,160,189,170,41,33,167,176,193,175,179,178,180,183,183,164,191,176,188,208,236,77,122,138,184,196,157,32,6,51,194,183,130,88,204,30,17,56,181,150,132,121,111,91,212,205,65,75,71,51,132,135,128,128,134,123,113,97,75,52,35,57,64,35,97,88,89,87,93,53,76,14,37,206,198,193,194,213,138,175,223,249,24,0,36,115,84,170,73,72,95,148,233,191,44,44,57,41,47,19,14,4,1,249,40,211,212,209,213,208,209,206,195,179,173,149,166,247,3,48,41,41,40,42,35,116,130,113,87,83,66,229,166,75,52,25,6,42,62,68,60,53,68,28,13,50,6,23,36,46,40,5,83,246,252,250,246,6,20,40,59,77,69,29,52,27,214,208,207,197,198,187,194,212,250,222,67,90,49,127,133,138,128,121,24,111,109,109,106,107,108,106,108,111,127,145,144,149,172,137,196,137,159,173,181,188,136,205,213,193,237,0,19,20,116,90,19,20,13,20,18,244,12,223,36,204,204,211,198,242,208,178,215,251,190,1,148,3,174,199,222,243,0,248,249,250,237,223,209,207,210,231,219,221,221,232,224,203,190,208,222,190,231,213,194,166,133,96,91,16,229,220,2,220,226,234,19,23,12,41,255,205,196,242,198,178,144,120,103,12,200,26,21,43,76,40,100,1,226,221,211,212,111,193,201,198,178,207,153,132,117,127,134,123,113,107,104,108,91,109,66,135,122,117,126,129,152,165,135,191,177,164,146,114,100,130,147,156,161,177,205,205,211,232,199,244,13,40,35,25,14,9,223,203,3,182,180,203,234,27,9,38,44,44,38,22,39,26,252,221,34,118,45,143,168,183,51,179,170,157,121,101,148,83,77,53,254,210,232,182,181,183,179,76,237,123,214,197,184,178,184,131,217,202,183,178,211,170,207,11,32,56,76,58,29,12,244,168,138,207,198,222,168,126,81,44,74,67,57,59,62,96,182,199,232,21,85,72,104,134,133,138,136,138,153,128,176,232,240,223,0,235,214,193,185,217,215,249,200,210,214,202,97,93,98,180,183,206,94,43,49,61,84,10,104,122,143,137,137,120,98,1,246,32,52,84,7,107,115,35,121,121,9,235,28,22,15,10,36,255,5,27,24,80,123,207,227,248,8,25,135,23,153,166,176,164,156,138,116,77,66,97,252,13,250,27,18,57,82,47,69,75,85,87,97,109,135,155,169,57,60,180,178,170,153,28,230,213,215,216,210,192,194,112,115,127,202,233,13,52,162,23,48,154,131,215,224,236,255,206,3,12,172,172,242,240,14,252,2,249,8,5,3,170,157,148,40,41,197,198,185,195,166,253,18,254,42,2,19,16,7,254,20,248,245,250,222,186,184,150,17,39,198,186,209,177,220,81,63,107,56,128,201,196,184,172,170,193,227,173,251,14,34,25,18,14,16,27,24,27,14,18,24,22,15,11,235,197,111,87,71,70,18,23,84,89,76,25,247,211,206,216,203,230,183,136,124,121,118,116,120,149,171,133,59,71,101,121,61,50,11,218,204,220,240,221,224,245,217,211,186,195,177,164,172,148,139,159,125,113,105,96,77,81,162,135,60,68,66,66,72,62,64,72,89,105,78,118,125,132,145,162,173,173,154,141,154,156,91,96,88,104,60,19,236,77,2,18,34,238,192,200,207,222,23,19,4,1,16,10,20,4,40,49,11,245,237,224,209,209,212,211,212,193,180,162,125,210,109,80,42,18,8,185,167,174,176,219,166,177,242,255,58,118,77,134,124,144,135,143,119,103,37,64,87,92,64,101,89,130,83,154,169,163,155,170,167,56,148,19,179,193,196,196,192,194,168,188,198,202,195,205,190,131,216,123,94,94,83,245,92,227,18,34,43,46,47,52,89,106,8,247,25,233,235,239,235,230,49,220,187,131,128,86,77,94,98,124,103,164,160,159,158,149,196,155,224,121,248,121,86,121,131,101,70,156,94,122,143,70,148,137,133,127,116,99,104,163,174,154,132,128,116,95,107,86,111,255,2,25,255,32,54,111,138,51,142,189,210,203,208,212,221,235,5,36,63,104,139,143,152,154,136,152,150,148,169,182,165,136,18,2,56,216,211,223,215,213,254,6,15,5,17,64,38,88,73,80,95,105,38,175,147,133,126,183,1,255,247,252,6,20,81,53,52,75,64,56,67,57,39,252,202,189,196,182,224,234,230,216,215,202,221,233,217,184,208,85,238,144,203,179,121,89,78,69,199,190,122,100,119,71,65,74,110,29,17,20,32,42,22,60,86,91,89,118,197,205,222,229,1,30,168,174,177,178,63,53,54,32,57,12,34,55,88,118,146,182,202,205,47,22,253,232,56,42,13,217,238,196,151,84,20,223,29,177,183,195,220,234,26,4,254,11,220,30,167,171,181,188,157,208,191,160,145,178,208,233,46,125,94,87,99,85,83,113,137,173,151,123,130,9,21,198,222,134,218,122,217,19,214,206,195,56,222,254,11,242,6,69,72,167,132,191,95,82,71,67,63,217,68,82,95,98,100,107,109,112,111,106,69,95,224,216,246,210,29,63,73,54,78,240,239,233,219,211,183,219,216,214,191,179,204,221,225,229,8,106,139,155,165,7,197,233,180,26,24,33,180,166,183,236,210,34,29,26,8,236,216,130,125,123,227,229,120,97,128,74,35,247,242,168,139,163,73,33,74,82,67,26,51,195,188,220,170,249,67,26,74,244,253,166,156,162,145,153,158,147,134,159,110,118,114,137,169,140,246,250,232,14,218,40,11,19,200,207,194,177,168,162,138,165,87,36,40,66,98,98,38,41,80,87,117,122,195,215,236,243,226,5,41,67,153,173,172,88,48,126,150,144,148,118,141,91,130,72,156,38,32,110,137,125,157,111,147,106,158,185,217,216,210,202,204,225,239,237,171,155,230,234,238,246,11,25,15,187,202,207,213,223,230,236,240,253,18,54,62,40,81,11,250,174,241,150,131,123,107,107,91,77,61,34,73,238,250,35,225,124,81,140,70,61,45,50,53,155,64,69,59,159,229,219,183,157,195,137,139,118,97,151,83,79,83,119,221,19,30,46,134,38,178,184,162,127,144,111,157,142,159,129,127,128,127,124,120,123,147,75,81,102,101,88,71,66,77,110,132,162,192,37,56,67,52,253,222,22,186,173,166,166,172,169,135,105,93,87,168,153,65,193,221,176,171,179,166,179,199,208,201,180,199,149,159,156,136,123,128,113,21,74,193,126,181,41,54,61,64,83,57,64,78,88,77,78,68,38,80,252,237,145,153,149,155,158,175,143,124,151,205,199,204,212,61,52,38,50,32,85,126,69,143,140,153,177,200,60,205,202,220,241,236,192,178,160,137,134,135,143,145,158,159,148,150,148,126,86,11,254,43,42,127,180,212,28,231,155,130,101,72,64,68,30,8,54,49,51,65,80,66,70,89,108,77,126,137,146,135,95,90,91,98,121,73,226,247,4,32,178,143,126,169,179,168,195,192,188,195,203,174,230,229,216,249,216,220,203,189,157,130,181,121,170,109,129,127,132,148,122,159,162,114,113,220,179,225,106,183,223,210,59,58,60,17,46,234,240,232,239,245,246,240,250,236,36,45,23,9,22,62,5,53,69,82,97,102,91,104,100,103,113,118,130,145,164,178,196,216,28,38,32,56,27,232,25,233,51,52,48,16,247,224,200,185,171,164,170,172,199,156,165,164,176,194,208,242,23,15,17,21,14,219,202,190,194,161,156,190,163,182,192,241,221,203,210,210,213,201,249,228,199,203,158,138,177,177,188,150,213,238,0,25,25,36,187,201,251,219,214,238,212,9,102,106,99,82,92,74,4,247,237,226,216,216,84,77,195,174,208,150,152,151,162,167,219,156,143,177,187,197,193,177,179,187,180,219,227,210,243,196,68,77,60,83,54,33,235,238,236,213,180,38,189,193,199,171,230,222,222,211,121,71,61,23,35,15,63,63,64,244,207,152,180,95,72,113,50,41,25,10,248,135,241,252,21,26,16,46,22,9,1,251,13,15,43,76,9,115,147,177,178,169,168,172,200,25,195,69,65,60,61,60,49,53,41,46,38,20,42,250,4,6,255,250,3,11,7,22,17,253,8,30,231,250,22,51,105,119,122,144,200,218,217,218,220,221,218,186,178,206,138,106,82,161,166,171,243,12,30,61,206,217,231,166,229,218,216,216,225,219,177,152,217,213,220,207,183,187,229,196,228,37,94,111,67,131,136,120,33,27,13,1,228,195,243,29,155,146,144,151,24,253,2,235,43,203,175,162,149,140,137,134,154,141,198,227,244,10,163,41,39,31,47,65,60,52,55,59,138,91,140,185,222,251,16,14,18,26,17,64,188,180,176,158,202,124,138,213,222,162,150,138,127,170,108,110,117,111,110,94,71,32,45,180,199,164,172,239,69,26,93,231,105,140,119,123,145,155,195,186,176,135,166,167,168,106,95,108,75,68,59,49,40,107,41,47,41,243,232,22,192,161,150,131,62,81,84,95,69,110,57,151,161,150,174,153,23,60,47,27,17,225,216,251,147,231,131,103,143,107,182,215,236,245,236,131,181,150,227,183,163,184,114,132,90,140,83,89,83,80,192,156,186,202,209,199,198,121,130,126,140,156,114,163,173,192,204,208,209,214,215,209,203,155,151,147,141,9,241,11,205,221,240,17,42,15,54,141,133,146,177,223,249,234,217,224,237,227,21,37,7,4,232,218,222,224,75,59,52,60,71,65,89,125,138,217,253,11,228,26,218,228,203,195,205,136,128,146,114,74,37,251,164,178,182,176,63,107,233,111,112,106,102,96,229,112,145,91,214,153,174,33,5,206,2,18,37,55,249,67,93,118,159,181,196,203,196,222,190,255,230,27,252,223,226,207,214,211,209,211,213,220,216,225,184,198,215,217,225,181,238,242,246,234,225,222,220,218,212,189,176,188,160,193,185,187,205,234,221,254,248,245,28,8,53,68,33,94,6,10,252,26,8,250,215,108,246,84,160,186,211,225,227,205,218,240,0,220,191,182,157,202,115,95,88,118,131,218,162,187,206,229,144,220,225,228,240,0,32,63,114,196,216,200,203,165,205,221,253,32,60,57,83,83,69,80,63,55,60,61,62,236,0,4,164,230,137,144,153,151,131,134,124,164,115,120,122,132,144,126,99,52,27,72,207,191,178,183,222,204,249,254,237,220,215,218,226,254,225,235,240,233,213,185,201,222,241,198,23,85,146,172,180,169,140,16,15,47,23,37,19,16,12,105,88,116,62,76,84,92,95,122,119,131,152,172,106,184,186,195,185,164,199,138,129,142,136,153,175,196,131,211,214,130,149,6,143,127,242,220,49,12,37,59,78,86,82,73,83,95,122,144,8,185,94,98,71,78,54,150,28,169,183,181,54,12,104,241,224,182,174,182,204,220,236,252,5,17,69,86,68,107,62,22,207,211,214,224,229,228,228,228,203,195,195,204,200,214,210,213,204,192,180,160,120,65,20,230,194,157,123,138,125,123,127,139,127,114,101,43,82,221,242,253,5,186,38,19,152,118,219,1,230,19,207,203,206,205,219,203,210,204,217,209,218,192,211,142,201,180,163,155,219,153,162,182,199,216,20,244,232,14,89,100,84,112,106,96,106,115,61,104,245,208,200,200,205,211,211,223,7,215,44,210,64,59,70,60,119,85,129,157,182,185,181,65,160,172,192,163,7,246,234,231,230,248,19,60,57,56,48,45,43,94,125,177,154,226,226,217,222,225,237,21,30,201,40,35,24,18,7,21,183,166,154,151,166,171,146,121,161,110,113,86,57,44,59,69,77,82,74,226,238,211,50,202,208,150,16,196,194,204,76,57,21,239,87,235,220,223,237,252,87,81,17,2,243,238,224,223,209,172,129,109,99,103,101,99,100,113,140,106,210,209,206,211,188,167,244,242,251,8,3,35,49,16,64,78,68,63,67,59,0,250,94,112,77,107,68,102,94,84,86,95,78,106,113,113,127,207,238,236,9,31,35,33,250,252,248,255,27,26,25,73,29,117,98,85,87,103,80,53,64,149,158,153,147,137,142,136,146,132,122,77,76,83,96,95,89,91,97,103,123,143,143,123,111,119,165,167,154,137,156,42,139,146,152,32,19,246,216,114,176,184,187,189,204,202,189,172,179,127,138,151,242,152,246,138,8,197,185,164,144,206,23,9,18,22,30,216,220,53,73,27,81,80,82,99,115,11,137,150,165,203,243,182,37,58,58,71,103,92,125,133,84,70,94,216,217,205,224,188,251,232,223,210,146,206,219,202,127,145,94,240,14,234,50,83,119,134,138,236,116,131,121,56,7,106,47,48,67,72,251,247,244,58,30,196,170,8,170,176,185,180,173,192,161,136,113,112,125,113,148,160,150,138,19,238,41,76,216,210,215,217,219,213,217,58,81,106,126,147,155,172,185,195,200,7,22,204,181,202,155,141,145,166,177,218,10,26,16,6,15,33,46,215,203,39,71,235,108,156,206,240,209,148,164,78,221,245,217,80,73,124,96,96,86,94,229,206,165,212,188,29,65,247,185,196,194,192,185,181,192,184,192,131,160,113,27,13,120,163,199,102,246,231,223,198,155,174,176,154,137,138,159,166,87,89,87,93,189,193,190,189,212,251,219,196,195,190,52,56,58,84,43,104,106,41,50,176,166,177,175,162,149,145,10,240,158,87,70,91,119,117,137,108,122,159,138,136,127,103,136,13,246,219,221,206,180,169,166,188,185,163,157,165,150,143,142,153,174,167,227,251,200,24,51,67,83,101,182,126,137,51,81,88,102,137,64,158,144,138,126,136,165,196,206,217,213,183,196,205,208,197,199,200,193,221,225,189,206,35,21,11,248,8,245,55,135,141,167,179,189,186,159,119,95,145,134,139,173,151,179,118,78,62,52,30,11,245,245,10,3,229,226,217,230,212,160,251,115,69,48,39,54,96,60,59,194,226,11,102,56,245,225,59,64,65,252,6,229,204,181,155,105,131,67,69,110,110,93,26,82,20,250,234,219,108,69,50,248,238,242,246,73,74,74,75,82,253,83,108,130,158,170,179,198,227,208,199,215,73,45,23,255,236,188,141,104,76,57,37,29,206,160,153,161,165,187,140,165,168,158,160,200,172,183,152,9,11,4,32,56,34,249,207,220,139,132,173,235,246,226,47,224,161,154,156,142,133,57,70,49,140,136,25,41,55,58,83,99,101,106,91,112,71,93,118,132,150,183,173,185,173,183,152,199,178,152,175,120,253,121,111,84,53,24,13,119,12,25,12,33,35,45,125,175,177,109,103,129,214,203,207,174,163,104,120,193,160,105,134,122,107,88,37,18,0,243,18,234,108,113,115,121,131,136,106,127,111,27,237,229,230,10,78,84,57,187,185,187,218,179,226,239,224,179,173,192,199,197,190,205,223,133,100,69,31,65,37,8,255,1,39,48,249,215,198,186,177,169,152,120,79,33,26,44,112,210,83,61,16,68,61,41,28,64,0,245,19,233,231,34,41,76,99,28,27,22,209,31,194,196,195,212,218,194,221,210,196,191,192,195,199,188,180,185,209,240,228,149,221,233,228,226,216,222,228,20,14,228,227,234,231,127,159,167,172,88,166,163,175,161,162,146,148,154,173,190,51,33,74,7,242,245,13,29,243,33,38,241,249,176,203,241,55,49,24,41,7,243,255,222,245,216,255,8,254,12,14,127,113,101,148,44,59,73,72,174,186,71,80,68,94,88,90,97,105,68,104,83,85,105,81,97,43,15,228,184,149,180,64,69,78,69,89,228,208,243,189,1,74,95,110,154,168,134,72,64,71,216,220,234,229,250,12,10,191,0,243,9,205,241,9,220,222,219,216,207,204,203,203,216,217,83,67,201,50,53,50,45,70,39,102,145,193,175,197,134,149,141,160,199,198,205,208,72,38,53,62,74,87,110,71,84,116,44,36,40,33,52,72,185,193,80,77,85,85,118,137,204,208,221,238,249,7,12,8,6,4,90,96,49,12,41,240,249,226,217,49,52,59,47,186,185,181,178,176,174,197,209,185,200,138,104,160,184,70,39,52,3,111,109,116,126,138,148,137,132,141,3,36,234,130,125,135,193,195,170,189,176,163,109,109,213,194,181,166,229,109,76,103,131,228,232,250,224,252,248,5,24,70,41,229,191,197,235,0,217,227,34,215,85,100,105,105,91,73,40,145,148,132,73,87,153,58,50,66,226,209,111,221,242,10,34,198,104,94,79,100,195,188,110,127,166,146,186,125,109,210,211,158,133,190,96,72,48,20,12,220,8,242,99,85,243,207,185,169,143,56,52,33,245,69,204,254,84,121,122,81,126,158,220,214,212,194,150,147,173,148,155,166,160,156,169,197,214,33,14,211,223,222,239,205,253,172,172,154,136,55,14,253,1,243,39,175,169,241,1,214,53,39,61,43,101,202,230,29,225,243,31,18,100,76,59,145,169,9,250,238,241,6,18,18,15,24,26,32,247,252,3,160,127,92,80,228,208,211,167,182,151,121,41,42,0,46,218,143,155,177,215,236,248,0,249,230,236,226,242,239,243,234,215,201,187,180,169,152,157,153,142,141,139,146,144,151,167,183,133,126,170,20,247,229,239,184,173,175,168,159,36,38,171,179,151,0,143,144,169,133,202,236,245,67,49,132,131,75,69,106,62,19,233,83,224,238,246,129,129,156,85,113,70,145,42,122,127,121,126,122,224,31,5,231,219,221,214,243,241,234,22,23,27,24,82,112,176,203,161,149,120,133,104,126,179,134,180,172,193,171,156,136,224,122,84,107,177,171,192,212,157,122,195,106,120,134,128,104,72,225,182,172,195,94,50,126,136,154,117,122,118,182,189,166,206,247,149,153,160,156,165,152,126,94,98,114,65,145,150,161,176,195,163,138,138,141,156,21,49,253,246,243,239,241,248,33,1,7,237,221,77,95,23,28,10,153,247,195,224,198,135,143,162,136,139,132,121,121,119,109,88,67,114,110,103,52,71,81,83,85,99,109,192,224,63,201,54,202,194,202,166,131,128,117,77,97,204,162,193,172,175,172,153,166,126,114,102,72,55,93,40,113,119,127,150,146,141,117,125,115,137,128,210,233,130,105,108,105,234,3,34,25,73,3,226,185,158,155,114,140,111,106,167,135,185,67,81,112,94,122,121,119,116,122,77,51,94,24,119,232,207,214,16,51,241,170,228,121,1,33,222,53,63,74,87,104,195,114,138,110,172,190,161,79,62,72,126,85,190,175,159,171,159,151,149,136,155,135,131,67,39,56,17,31,17,12,83,217,188,173,134,139,157,176,168,165,149,172,213,250,48,62,8,15,253,11,250,203,249,228,196,185,191,195,181,215,242,55,55,42,27,17,221,226,144,238,234,224,226,221,208,231,229,228,251,196,104,32,95,82,107,65,148,139,146,151,158,103,71,148,70,58,72,205,193,174,184,170,190,145,157,165,188,230,43,30,33,33,59,98,118,122,125,113,102,92,106,77,65,56,54,37,51,74,66,65,62,101,66,59,130,126,58,56,64,80,104,146,106,126,193,190,213,169,189,170,213,204,165,13,10,3,12,214,86,86,79,68,99,152,176,162,220,226,190,11,57,138,140,152,146,146,97,111,129,151,173,146,190,175,117,113,176,203,8,40,69,120,132,94,106,240,233,243,240,178,147,236,213,192,171,150,86,69,129,86,87,73,59,40,30,20,255,37,51,254,253,0,151,142,195,135,132,206,160,173,192,152,230,229,222,241,217,251,211,47,44,51,31,12,31,13,8,24,6,88,89,101,96,87,52,92,120,157,185,198,201,192,192,145,151,230,204,187,12,24,30,26,57,70,63,250,152,116,130,176,184,148,168,116,177,159,113,65,52,33,19,247,85,84,35,47,65,103,173,199,216,218,226,242,17,54,93,85,60,104,11,201,106,224,68,178,193,230,245,213,5,7,251,243,237,183,172,172,164,189,150,194,189,217,137,29,34,31,122,129,68,72,52,72,67,75,39,63,9,13,33,56,50,98,104,80,115,62,146,140,161,129,176,76,69,39,8,247,233,81,235,226,228,243,250,248,248,9,14,92,83,112,238,232,215,202,215,210,181,206,76,101,109,200,102,96,121,16,253,180,131,5,92,98,100,119,142,167,190,169,193,190,185,187,200,139,229,159,176,143,205,238,191,67,100,0,16,4,99,212,119,121,127,186,150,113,79,78,175,194,205,170,165,183,192,234,213,189,169,248,158,159,180,165,9,6,225,212,208,125,199,196,231,231,226,6,0,254,238,220,209,212,218,218,229,233,233,238,235,227,209,182,160,163,160,162,212,208,207,125,43,180,39,233,219,208,183,173,198,204,247,248,251,2,1,7,4,5,30,58,60,48,65,131,165,161,102,186,201,221,216,236,199,247,200,21,94,120,142,152,76,119,110,88,68,60,66,94,112,67,248,99,255,1,255,10,65,90,40,41,45,92,181,204,197,118,135,105,163,143,129,135,103,203,60,66,158,128,188,173,173,177,188,178,203,158,205,182,186,192,214,238,200,31,73,112,139,122,214,169,152,33,198,195,220,47,201,174,145,131,135,132,118,111,67,229,47,63,28,86,145,245,146,149,136,128,131,141,152,41,12,94,81,83,91,108,252,226,1,206,203,202,197,186,148,131,168,25,224,44,155,115,198,65,54,72,107,132,158,187,202,172,47,53,45,45,248,14,6,1,1,254,93,136,219,214,200,197,212,212,207,192,176,153,114,107,137,61,12,236,250,76,82,41,42,38,36,30,45,38,18,19,151,165,146,141,121,101,188,76,28,239,48,49,72,68,59,62,107,52,20,253,70,28,34,54,214,246,162,131,244,235,2,227,225,23,14,28,39,28,58,85,107,98,61,22,55,38,178,217,194,188,175,179,192,219,249,76,103,50,134,130,114,40,16,101,106,107,106,106,104,102,112,95,103,110,103,92,106,137,151,172,118,123,140,151,167,142,178,177,129,198,213,212,181,251,18,37,27,44,115,118,123,12,27,35,40,14,30,47,20,21,202,215,13,195,193,206,25,32,124,149,166,5,15,27,169,174,1,9,12,171,185,197,205,19,6,255,255,245,226,209,216,222,215,252,206,218,234,245,248,249,206,190,189,210,244,227,222,208,162,147,121,90,54,64,68,60,41,248,210,19,238,25,63,82,245,18,221,205,197,173,133,99,75,41,36,32,26,19,45,26,62,55,36,100,234,228,229,129,127,179,181,196,223,221,219,215,155,90,159,114,102,107,103,98,108,86,104,96,107,75,119,48,104,98,109,120,132,138,159,127,212,200,184,171,150,127,96,112,86,105,101,164,192,177,218,212,218,205,199,198,216,229,74,60,225,218,207,182,60,161,163,159,181,202,226,2,38,54,26,31,38,30,13,22,11,26,254,226,254,205,245,203,176,202,216,195,72,184,181,183,112,94,83,74,79,220,232,237,176,183,165,152,100,66,26,233,117,220,183,193,162,114,114,246,228,241,173,232,37,41,36,31,65,69,45,30,13,8,10,20,255,180,142,114,200,212,195,160,130,107,98,74,49,54,83,86,104,131,100,164,240,42,76,111,112,127,143,151,113,132,150,193,170,229,249,6,27,11,22,245,178,189,173,220,216,19,217,216,222,222,217,90,103,206,201,173,186,55,42,46,66,89,85,105,121,141,165,154,162,145,128,127,127,215,198,113,85,205,131,128,128,137,225,19,194,23,23,242,242,249,22,255,3,17,7,29,224,242,253,19,8,36,141,31,156,173,182,193,178,192,168,167,132,109,114,62,80,101,249,249,235,3,100,86,96,107,87,101,121,160,170,175,59,191,190,182,183,172,163,251,197,185,179,207,213,220,128,218,214,223,196,108,146,174,133,200,231,208,223,218,22,5,29,5,250,38,252,251,0,13,4,16,47,91,129,129,142,125,64,62,58,202,187,192,168,151,234,233,0,32,241,62,58,243,252,17,22,12,19,13,255,252,249,244,249,195,144,87,255,105,13,248,46,215,199,182,177,223,177,178,229,237,129,74,64,144,149,174,166,143,253,13,27,16,9,31,26,39,27,32,33,33,12,50,61,23,16,29,68,119,127,33,17,1,247,237,238,8,98,117,120,115,124,113,107,112,221,81,114,48,44,53,9,207,176,179,220,245,6,225,242,210,11,211,216,206,218,156,145,135,123,97,76,71,74,70,156,179,123,66,50,52,72,59,75,74,56,60,56,60,70,73,78,122,132,151,124,118,128,147,137,153,94,88,77,80,76,94,80,43,55,235,117,11,38,51,62,67,63,150,166,173,182,204,187,215,26,28,11,8,10,38,69,76,89,243,244,245,236,216,199,202,198,207,219,216,207,175,8,11,7,144,185,176,188,255,2,141,73,171,140,100,121,178,148,152,166,96,92,25,5,94,87,90,111,128,78,65,70,139,76,66,154,172,152,130,118,78,150,62,94,161,181,188,187,202,192,198,191,204,203,188,183,230,122,127,101,120,157,194,73,29,61,66,66,82,70,95,25,10,249,237,41,217,228,223,238,248,61,77,215,133,150,241,35,80,107,122,90,173,172,168,165,174,153,165,182,141,242,212,162,133,4,102,117,15,2,49,135,153,155,141,48,42,29,175,190,83,101,69,126,149,47,145,123,134,135,132,95,64,84,96,167,180,155,201,141,135,151,147,112,138,109,86,131,245,223,223,224,6,234,16,216,7,217,22,72,120,204,190,202,217,231,137,155,160,153,147,138,135,162,124,191,213,187,140,51,35,56,23,74,233,219,210,215,248,226,40,58,16,22,40,34,28,253,30,45,33,78,19,125,64,96,221,200,115,118,106,90,85,19,244,11,11,10,18,16,7,253,71,34,17,223,122,88,60,44,51,166,157,218,230,211,249,242,206,224,224,240,210,175,206,110,139,192,38,53,49,195,199,184,142,141,127,98,136,72,107,109,141,140,3,26,34,32,13,174,199,198,200,199,206,229,1,24,156,157,158,164,176,171,84,38,241,233,251,57,75,98,76,81,207,77,137,90,60,27,181,3,225,62,35,242,158,184,240,18,198,179,163,176,153,169,159,177,202,212,173,165,182,150,215,212,137,90,101,129,94,55,151,177,137,131,28,32,191,39,99,229,115,105,224,219,186,184,213,242,16,27,26,252,13,197,206,75,72,66,64,54,204,63,73,83,93,114,79,92,93,81,97,213,212,208,211,36,61,67,75,246,247,213,198,202,194,170,203,176,169,207,166,205,212,221,54,158,151,175,158,52,42,187,231,184,186,32,28,45,34,67,59,39,53,37,241,241,221,212,125,133,142,126,241,243,221,117,103,94,239,203,206,194,186,192,200,230,238,193,167,193,214,108,153,193,225,179,152,162,154,145,123,139,150,163,91,104,90,118,99,38,210,94,191,247,7,249,227,242,219,216,79,61,38,222,0,188,37,248,5,19,55,126,116,12,28,215,101,106,117,161,122,144,153,161,184,208,239,237,240,249,24,64,148,192,182,180,95,67,117,125,166,174,156,139,148,74,66,47,44,50,38,76,102,140,89,87,79,142,166,196,251,238,231,227,214,196,232,244,13,248,228,73,131,154,232,250,241,254,14,38,57,61,150,174,215,194,199,211,213,234,235,235,234,241,100,126,220,119,134,126,99,85,116,72,47,27,74,226,15,212,31,186,40,60,153,75,60,72,41,48,163,55,53,62,52,59,227,241,203,178,172,176,183,181,127,147,146,172,141,168,70,73,108,161,13,24,8,5,220,33,185,37,194,188,187,185,176,133,147,161,175,170,156,178,130,131,132,131,123,118,149,107,177,62,57,101,56,122,114,114,99,73,68,130,145,30,251,211,6,202,185,171,167,168,169,172,171,125,111,148,86,70,53,171,218,174,12,212,188,233,173,162,208,231,240,181,201,208,154,173,100,225,255,29,208,24,44,46,21,60,87,82,68,106,67,75,71,86,81,20,30,128,144,130,144,157,163,151,165,141,218,174,174,158,44,53,149,62,160,148,158,138,126,205,230,59,217,207,212,222,243,222,239,246,187,148,132,170,179,163,166,143,247,236,158,206,253,48,77,87,38,57,0,41,54,77,101,92,108,120,126,155,162,151,159,156,52,82,108,108,248,235,144,121,133,108,170,184,182,219,204,200,183,201,209,207,247,245,233,239,219,192,174,214,205,204,197,194,189,156,131,178,103,200,68,172,81,209,97,136,132,123,117,120,124,132,168,108,167,163,175,108,104,93,217,222,233,28,83,221,203,243,192,45,224,226,227,230,217,232,243,219,38,16,63,35,29,9,20,21,8,248,40,58,67,78,91,91,97,95,161,166,175,186,204,229,218,36,49,43,81,74,32,244,48,248,74,88,84,53,33,77,12,248,227,208,175,187,159,161,153,151,155,161,173,184,164,73,40,29,26,3,32,18,78,109,186,206,12,246,207,218,21,194,188,109,177,172,203,174,182,131,208,233,246,38,60,34,60,194,180,206,234,226,207,206,215,2,216,222,37,46,115,82,250,204,221,207,73,84,73,204,181,154,132,133,138,234,131,191,198,211,213,171,175,214,221,220,220,208,206,62,66,60,86,59,48,255,235,187,142,141,58,49,191,191,185,189,188,137,227,213,210,212,137,97,74,65,68,24,31,35,51,40,64,59,78,76,206,172,158,193,115,101,83,62,125,35,45,42,37,21,130,144,227,79,247,0,1,100,28,129,159,179,185,173,159,144,132,168,120,191,70,207,76,82,77,68,63,61,37,46,64,63,35,43,28,40,25,253,52,217,241,37,42,28,255,229,20,51,202,232,34,54,131,129,122,111,29,243,227,222,146,91,75,159,165,162,245,208,194,165,229,237,216,178,201,160,217,218,219,217,225,213,170,185,237,242,70,80,129,66,237,39,133,150,167,12,36,177,179,165,157,158,157,244,237,221,195,157,151,139,200,169,123,45,55,56,54,42,54,60,66,61,60,157,169,16,72,175,176,189,154,123,119,130,219,216,237,130,124,99,107,119,115,117,48,247,201,216,188,185,215,51,13,81,220,237,90,108,98,127,222,203,199,153,187,111,101,57,65,63,42,33,82,34,42,55,220,200,30,44,250,104,106,132,118,137,146,166,175,78,99,89,68,21,53,246,195,173,191,149,214,147,72,98,247,226,232,127,150,87,132,1,239,220,197,206,171,196,86,100,98,184,206,200,219,27,200,190,87,118,118,113,100,163,155,151,157,161,211,206,209,218,202,192,195,149,178,172,190,49,39,41,162,153,153,211,8,197,210,210,218,223,136,55,53,250,196,203,207,200,209,215,209,101,46,54,76,65,94,127,156,156,163,216,129,125,97,133,74,193,172,183,170,198,168,109,108,107,115,109,81,243,92,134,78,175,68,191,237,235,211,220,239,0,235,15,28,228,65,122,152,202,216,146,231,160,242,175,4,240,27,228,52,49,216,205,199,193,209,196,201,204,206,218,212,198,235,163,191,212,178,231,223,209,165,242,228,124,181,123,207,197,170,166,208,234,243,225,245,254,243,243,208,123,245,77,68,98,123,206,17,29,5,255,26,61,70,48,187,191,221,224,241,202,189,135,206,149,146,223,237,155,172,150,189,150,196,62,85,101,133,164,1,196,186,174,66,87,66,97,54,137,56,233,250,208,149,169,104,101,122,152,37,25,149,155,157,205,218,214,202,5,245,255,255,183,232,235,249,23,68,117,159,149,139,112,22,75,3,14,2,66,117,28,48,36,68,88,114,120,115,117,113,120,140,160,171,104,192,208,209,200,167,207,161,214,143,135,144,128,150,115,131,110,146,165,178,191,215,102,219,203,242,136,109,151,10,147,157,230,246,209,213,91,103,85,67,187,213,242,204,104,108,110,69,83,84,54,152,40,5,160,174,178,177,161,97,130,228,233,250,3,12,24,31,34,69,110,80,126,132,93,29,191,201,204,215,227,202,233,235,232,235,234,237,207,186,219,217,216,203,190,170,117,66,54,27,4,45,231,196,160,114,27,55,87,252,72,131,109,217,30,9,238,206,45,184,193,198,204,197,202,186,195,211,178,212,195,211,222,239,173,134,38,162,134,145,142,165,254,39,16,224,218,68,84,84,102,123,93,72,56,232,214,202,212,218,198,204,219,53,203,213,57,71,36,93,207,170,202,207,207,173,197,47,45,144,154,145,165,186,151,10,252,239,228,241,9,44,66,58,63,47,61,60,61,54,58,164,176,153,230,230,236,227,227,218,217,216,203,152,221,191,62,19,25,29,94,116,152,20,19,17,30,69,76,66,77,206,219,7,205,39,202,196,209,161,248,130,19,195,186,197,254,217,225,241,0,16,75,73,40,5,234,228,217,209,194,154,111,97,92,97,235,215,208,25,174,127,24,4,58,68,40,222,241,240,70,97,117,59,105,57,70,68,75,88,66,217,239,14,55,42,33,29,254,252,249,253,6,12,242,171,158,123,70,73,90,161,153,146,147,144,138,152,140,158,56,77,100,84,89,180,181,222,4,181,134,133,162,235,204,173,170,171,173,193,178,126,130,146,73,111,246,152,130,247,6,152,142,248,35,41,52,16,5,74,66,64,247,145,152,159,190,217,121,133,103,181,227,66,30,223,228,220,223,240,220,205,197,195,189,162,207,154,157,120,160,84,222,223,152,129,147,225,243,86,79,98,104,46,30,71,13,58,69,76,252,39,170,140,146,11,252,174,176,166,175,178,175,155,130,99,113,153,130,156,151,126,15,102,203,208,193,201,74,86,95,163,166,192,196,41,32,43,172,199,150,139,125,140,171,164,192,232,18,6,2,250,243,188,103,116,147,104,182,206,114,116,116,100,109,149,99,112,121,113,112,97,112,81,209,233,219,205,185,30,9,248,201,201,184,205,197,218,111,134,105,159,95,26,26,45,170,15,248,240,241,222,168,161,182,180,160,102,51,162,83,198,169,177,254,179,190,188,58,59,52,59,40,117,40,108,94,119,32,156,185,168,150,247,97,86,122,141,157,180,150,143,147,141,245,168,197,206,209,162,156,146,192,26,41,57,112,174,145,149,61,91,190,174,118,208,238,192,201,171,231,231,74,52,39,29,5,34,110,132,113,189,122,115,113,126,187,230,30,61,216,57,13,11,14,219,220,236,224,251,18,244,221,213,222,192,207,192,215,189,211,225,157,4,108,79,57,105,43,32,152,27,227,63,226,203,226,10,146,115,223,241,210,225,0,52,147,127,84,138,64,101,130,139,83,51,70,46,38,253,230,227,217,213,236,225,223,229,245,232,67,77,71,82,78,253,53,82,107,116,170,162,176,206,44,25,8,246,204,154,124,101,94,22,9,145,118,164,171,216,155,136,29,11,11,24,25,50,70,99,26,237,203,204,123,125,105,183,241,247,14,87,50,233,166,172,226,43,93,128,126,238,2,30,44,104,109,143,58,89,111,166,187,180,204,209,189,204,237,148,196,116,12,76,247,251,6,254,136,106,253,10,10,36,59,175,184,161,129,91,86,114,81,133,239,225,213,32,80,83,35,28,246,228,138,67,131,129,128,123,66,71,207,220,253,128,101,72,4,248,233,217,83,74,79,60,194,201,162,184,224,172,170,222,227,227,236,229,218,216,37,82,85,66,49,42,56,47,251,255,3,94,198,186,177,42,113,193,107,228,140,13,32,42,38,109,70,141,9,24,67,19,188,33,47,173,202,221,177,211,192,178,179,164,170,250,187,165,169,223,220,203,75,50,56,139,108,174,161,157,158,183,156,127,125,180,235,226,60,57,41,240,246,86,72,32,61,233,3,202,14,24,236,28,33,234,39,115,65,83,81,63,63,93,81,85,61,134,90,50,129,114,191,183,208,51,61,64,62,243,231,209,186,55,83,88,113,81,179,76,63,78,52,69,91,169,135,31,1,20,219,178,237,5,224,41,206,213,204,196,205,81,65,41,185,184,28,44,46,43,68,84,117,193,119,114,133,136,188,185,219,255,17,61,96,28,30,195,205,212,57,44,90,138,142,149,199,211,231,2,14,14,18,12,115,184,250,6,15,68,54,62,44,76,182,175,197,205,192,222,106,230,83,56,17,6,95,105,153,134,131,137,242,16,225,35,208,39,74,206,194,188,146,137,74,81,80,225,179,171,247,18,209,247,208,237,7,62,27,244,203,251,13,251,28,196,222,90,82,117,103,92,163,243,38,53,28,204,163,206,223,188,209,109,124,148,144,130,111,214,66,73,77,248,6,251,3,207,248,216,156,165,164,203,206,142,145,163,183,177,168,40,232,237,231,180,5,144,162,145,166,86,12,30,242,255,220,10,29,129,145,118,123,254,254,0,10,77,63,78,90,41,74,34,115,160,47,0,229,15,52,25,101,105,168,142,182,248,220,26,40,16,38,250,250,254,253,253,165,149,136,121,110,110,181,202,119,75,61,168,65,184,147,144,161,182,231,231,252,248,1,250,228,210,186,182,164,136,166,126,161,178,181,133,167,27,21,3,0,5,72,90,147,171,171,167,167,47,41,18,207,223,251,232,140,138,165,51,65,135,131,241,32,59,80,14,65,225,224,196,64,96,54,29,111,122,121,215,192,194,223,205,218,228,254,246,237,14,11,248,173,133,123,156,179,158,156,164,185,178,136,108,27,117,190,177,174,214,183,173,177,150,245,19,59,67,77,147,132,100,66,190,179,193,174,103,85,67,120,115,121,209,98,123,100,122,187,175,156,165,173,170,167,165,168,108,64,51,216,161,178,202,134,249,48,241,254,7,70,70,85,25,28,28,4,165,228,186,118,99,164,158,139,132,119,98,80,104,100,110,95,86,78,100,113,146,54,200,203,188,162,190,138,131,158,148,169,168,171,109,44,24,102,164,157,179,116,120,150,128,201,102,116,105,25,118,245,234,188,186,112,125,83,126,114,96,97,100,86,69,44,251,223,207,248,213,250,222,241,99,123,118,81,164,96,118,176,196,162,46,15,133,188,167,167,171,149,189,173,157,152,25,9,253,252,167,163,166,179,182,200,20,16,25,233,190,224,201,180,202,213,192,181,36,59,51,41,209,210,193,235,226,229,229,227,246,113,21,42,95,107,159,148,178,66,126,75,71,69,84,237,213,192,152,171,185,193,207,184,229,45,20,23,50,116,105,84,104,26,53,64,57,52,62,53,155,51,65,123,139,12,117,158,162,156,152,57,34,49,148,172,159,145,3,154,38,78,104,160,208,196,124,133,119,250,152,142,193,176,48,7,94,61,76,16,61,254,248,8,133,127,126,135,182,151,156,189,239,220,242,206,221,204,241,253,224,62,66,53,59,41,2,32,9,111,106,167,74,98,173,173,181,179,176,169,0,16,24,31,37,21,197,146,222,159,175,145,168,138,219,199,219,46,58,25,9,245,29,31,46,17,228,227,221,215,33,102,177,181,186,188,210,235,251,7,201,29,25,20,248,247,240,170,181,153,148,140,119,121,186,206,29,35,111,119,78,85,72,38,8,10,98,118,85,65,151,119,196,244,233,230,218,223,86,87,251,27,12,253,26,42,62,88,81,104,130,174,150,137,207,189,228,224,222,37,69,86,211,170,79,76,70,104,112,137,146,201,111,214,173,174,184,213,121,6,11,154,168,149,193,253,237,203,218,129,127,185,53,34,213,174,140,188,196,146,30,28,57,228,250,5,216,213,254,236,14,206,198,227,215,232,154,202,249,210,183,141,153,211,143,97,73,64,69,247,11,1,34,62,174,210,203,188,198,249,162,200,186,60,139,123,55,32,23,100,99,108,39,36,75,29,239,228,139,149,116,83,97,69,62,55,146,127,175,165,167,199,222,137,172,215,216,204,194,185,63,147,157,182,220,195,170,125,78,116,156,195,230,153,144,146,160,69,77,241,229,249,201,200,142,132,150,31,224,57,64,163,192,21,44,163,164,167,47,52,47,47,217,16,9,4,1,134,169,220,203,184,207,224,142,102,118,51,31,26,52,32,5,176,157,151,78,106,138,127,134,72,41,86,228,159,153,149,222,21,65,89,121,12,80,222,251,79,95,29,15,107,106,106,94,110,126,100,131,111,144,179,183,179,187,123,199,215,219,13,26,29,48,36,62,30,120,114,247,14,3,43,18,71,30,15,255,198,185,26,40,14,57,22,44,234,179,187,186,185,12,0,184,218,235,15,247,242,248,2,95,190,226,236,250,123,83,54,77,242,66,234,157,63,250,210,186,186,157,31,18,53,55,46,5,8,38,18,67,49,4,244,169,240,228,247,102,90,84,89,102,127,113,124,237,231,163,149,129,104,81,95,83,80,66,74,77,187,199,180,231,238,199,192,245,220,97,98,143,158,183,202,50,60,41,31,22,37,238,43,25,248,9,242,21,220,251,188,192,86,58,71,155,196,144,140,223,96,110,88,4,20,42,37,71,30,4,253,59,52,121,86,107,196,159,100,14,104,89,105,135,65,190,206,25,32,223,236,209,210,169,59,31,30,177,146,157,156,110,84,156,147,138,136,200,113,226,255,236,253,6,15,12,252,25,244,249,25,125,135,32,158,176,191,174,60,80,91,109,177,191,179,169,168,171,158,155,206,212,191,221,135,159,135,252,234,44,26,36,25,9,57,63,252,0,251,7,127,107,107,88,83,73,63,179,154,147,216,19,52,68,81,235,34,28,229,1,0,245,165,97,108,58,35,76,255,249,239,230,182,233,181,230,235,102,108,167,248,5,15,26,34,42,47,55,46,48,27,30,40,147,8,11,114,140,129,235,74,155,49,68,62,117,41,205,200,157,150,142,138,68,70,63,81,63,185,195,73,30,80,52,68,76,52,48,81,68,72,96,143,119,102,95,161,158,112,102,99,64,74,70,217,244,16,42,68,164,22,22,66,108,114,252,252,251,229,221,222,2,8,127,188,15,180,143,210,78,43,164,158,163,174,163,115,101,98,100,164,32,54,61,52,51,173,78,45,5,187,194,192,143,160,171,59,64,14,240,246,129,142,170,171,226,16,108,167,166,176,13,24,213,163,121,141,147,159,2,40,95,82,157,179,145,116,157,165,163,118,105,127,229,233,231,194,58,98,186,108,208,183,198,153,153,176,203,229,0,65,73,81,245,233,220,83,39,16,40,66,81,212,104,204,248,39,24,5,250,241,11,242,75,54,33,150,20,9,1,180,202,10,233,156,199,115,120,127,27,45,198,192,196,174,145,148,134,86,150,121,112,157,254,21,38,8,218,192,182,216,25,140,148,156,180,170,224,71,83,116,128,113,125,148,114,95,65,127,157,182,189,213,213,205,142,150,101,86,109,97,182,174,46,6,74,66,59,62,49,94,65,42,96,23,99,75,0,18,2,164,174,176,189,167,189,145,159,161,157,134,137,172,190,150,176,161,181,193,201,32,156,24,202,116,137,128,129,13,167,184,205,226,173,138,108,177,143,142,130,166,41,247,222,190,214,254,24,228,34,8,216,248,221,94,249,234,252,182,236,1,152,128,104,134,137,99,177,149,140,2,40,72,104,39,130,176,200,239,2,236,247,254,14,44,252,65,202,170,178,191,208,234,233,96,77,61,51,191,137,40,20,194,25,42,53,63,39,71,188,189,166,139,118,162,92,30,245,212,204,197,193,200,140,154,181,179,172,133,132,141,120,115,68,92,192,64,6,53,125,93,80,216,248,165,165,164,171,176,30,51,148,249,202,252,159,191,227,244,14,19,90,62,126,66,66,128,175,172,163,156,161,141,67,62,161,148,207,73,205,132,42,29,115,68,28,241,90,138,94,240,205,129,122,137,181,191,190,233,6,179,158,134,196,113,92,87,169,95,214,216,11,91,2,183,221,223,225,222,202,235,218,230,209,7,243,44,11,5,12,22,210,48,31,104,93,109,166,179,176,221,237,78,69,62,100,44,60,90,105,128,196,148,160,146,22,25,34,42,116,141,168,231,196,91,72,152,248,239,86,98,197,185,201,216,207,227,226,236,70,172,187,121,198,199,202,220,19,57,196,121,100,70,47,248,60,69,96,163,128,130,125,23,36,123,233,240,109,159,173,166,158,226,96,89,97,31,85,80,17,30,19,46,193,126,91,50,142,77,75,62,161,162,161,160,219,120,170,187,219,209,204,219,221,223,224,225,230,154,173,250,76,41,64,139,105,38,204,159,161,162,192,75,47,53,165,159,145,127,218,130,122,38,5,227,103,175,82,84,75,56,218,118,131,121,123,106,89,47,235,191,190,188,252,207,214,226,30,133,8,184,49,191,183,131,113,145,149,143,195,163,159,211,204,172,190,197,183,193,191,214,44,46,42,7,92,133,152,106,87,88,164,177,190,165,119,51,55,227,223,220,226,25,159,203,30,238,228,105,138,182,181,189,198,196,204,185,144,168,165,243,164,157,138,145,235,244,233,232,137,152,77,154,197,228,238,207,244,223,7,155,143,107,74,31,44,63,50,62,138,130,130,113,186,175,243,166,131,253,14,90,121,107,226,27,130,218,84,72,78,7,87,201,110,117,212,203,205,227,220,111,125,65,93,68,148,4,144,172,116,131,3,2,8,41,40,75,92,146,180,194,200,222,198,185,13,29,62,224,172,100,132,60,4,173,182,196,188,203,174,205,232,3,56,182,197,196,229,188,216,20,79,62,56,163,239,245,250,246,57,79,253,3,0,189,188,193,191,255,31,57,62,244,219,211,208,209,120,202,201,63,85,123,30,19,247,8,153,128,53,58,198,4,154,143,138,85,95,111,207,80,90,94,129,58,37,10,154,242,217,199,182,181,170,203,186,172,164,143,123,155,84,119,241,239,151,173,80,79,196,140,139,123,107,101,138,166,196,224,105,143,176,199,121,86,82,127,110,98,242,222,12,203,90,132,23,21,243,247,165,195,162,146,122,179,186,194,126,148,79,99,88,142,197,152,226,234,221,240,34,54,190,197,229,109,83,234,252,30,194,171,175,187,168,129,153,184,44,224,198,209,153,158,241,142,52,82,170,69,89,240,88,210,218,228,215,210,235,252,54,60,51,74,254,243,107,8,162,58,9,82,121,121,120,193,250,14,50,24,249,248,146,117,109,125,211,75,54,202,204,217,225,34,56,250,2,2,6,20,111,121,20,36,52,55,114,146,161,210,179,166,185,0,36,42,46,65,66,146,243,22,71,109,91,189,63,48,142,160,120,170,165,168,120,83,10,246,165,147,147,185,195,211,232,18,30,61,68,89,92,207,31,29,217,222,8,26,72,44,14,248,22,12,38,182,118,132,163,146,226,157,193,1,111,113,253,11,1,249,29,71,80,194,217,26,38,4,251,3,232,173,123,130,81,122,153,160,217,202,60,6,114,107,69,20,204,44,207,176,101,230,103,251,100,89,78,73,56,220,236,13,35,152,172,72,206,209,110,106,112,10,23,197,94,201,199,228,144,82,151,139,190,25,24,60,35,219,200,180,167,51,46,24,155,54,16,193,41,31,37,2,30,49,39,65,112,139,140,137,124,134,52,246,138,202,65,49,31,180,71,162,158,245,39,230,160,159,228,254,32,23,6,55,165,174,159,108,21,8,218,88,202,45,164,127,117,190,198,28,48,113,166,155,52,25,158,89,53,229,185,147,78,50,11,217,120,120,13,10,90,150,159,18,99,195,39,2,20,35,185,173,220,252,245,70,65,14,40,73,187,21,22,194,224,136,37,28,90,22,24,147,168,12,19,32,131,91,71,62,39,244,237,32,53,58,153,135,12,53,37,77,97,175,180,127,130,117,20,44,77,89,47,127,20,108,39,16,12,165,173,89,126,167,95,236,228,150,61,50,26,6,249,230,49,41,141,225,145,163,167,250,254,134,122,69,58,186,168,185,157,159,136,212,182,118,123,91,119,167,232,13,162,195,183,104,61,39,229,177,34,251,149,181,114,107,228,218,133,177,51,7,223,175,176,139,151,169,70,78,217,190,222,10,199,218,104,117,133,160,205,251,116,112,43,151,70,42,166,163,223,83,219,48,121,177,125,192,189,239,209,31,158,252,195,134,46,52,77,198,205,198,21,158,43,230,178,114,29,228,8,191,52,243,192,41,74,205,2,27,170,148,22,189,74,128,188,};
int windcodes5676[] = {152,138,172,172,141,158,157,124,119,130,125,143,190,206,205,187,171,175,136,86,111,80,0,210,177,155,125,96,80,66,162,135,121,119,150,136,86,91,93,193,201,212,203,213,227,230,243,228,210,186,138,68,242,217,216,172,145,199,31,43,57,82,110,119,95,49,31,4,13,17,254,198,173,161,182,204,108,133,122,87,88,103,64,47,44,29,26,255,235,192,133,122,156,154,84,67,68,129,169,191,181,182,140,91,49,51,62,52,76,79,52,19,241,207,175,208,225,235,225,202,207,193,192,235,236,236,224,242,250,237,252,23,13,250,231,230,217,194,193,209,173,105,76,52,39,11,246,236,233,245,240,1,246,206,177,151,91,98,170,212,207,238,5,22,29,34,43,44,45,57,74,80,85,142,153,157,129,126,110,52,13,27,44,41,2,241,248,5,11,18,34,58,59,37,16,216,191,168,172,169,165,167,137,127,231,222,228,251,24,47,81,113,147,163,142,122,82,38,59,80,81,109,154,156,55,33,36,19,42,43,30,16,12,15,24,33,20,13,2,3,6,5,252,231,191,139,91,90,82,103,158,195,205,223,229,57,90,55,42,16,20,30,37,72,128,161,184,207,200,213,220,204,198,206,212,213,185,153,132,103,75,49,23,21,23,24,50,63,53,40,25,25,45,77,92,96,75,69,54,25,23,23,21,2,243,236,220,180,133,134,162,212,241,238,247,236,246,252,250,249,234,196,169,203,221,220,206,162,168,161,106,11,247,241,246,253,242,213,17,235,193,193,240,1,13,22,0,241,228,215,238,10,29,53,31,53,47,12,9,254,234,232,235,249,234,235,228,218,205,167,182,199,208,174,255,247,237,151,184,16,74,73,59,38,23,46,76,10,31,24,252,232,240,253,252,5,222,199,228,9,13,253,241,242,3,42,27,1,12,21,242,213,177,159,186,191,167,112,58,28,47,46,67,119,196,222,238,231,219,195,195,191,16,249,250,3,12,15,18,22,34,31,24,26,41,44,39,45,44,36,23,45,54,44,43,50,88,115,38,53,38,64,105,158,192,191,194,161,189,186,161,152,91,158,144,155,162,111,71,28,1,230,234,7,28,21,28,46,88,76,42,1,224,238,227,233,224,235,251,8,4,2,4,9,26,22,1,20,54,58,57,83,97,120,83,38,31,32,36,109,195,200,202,208,216,130,148,166,191,168,169,180,169,158,161,168,182,197,158,125,131,93,52,34,17,17,250,231,231,243,230,218,235,47,30,16,25,30,21,41,55,96,104,102,113,133,180,196,216,185,142,113,145,150,108,105,63,34,253,250,13,46,63,73,111,85,74,54,77,103,159,197,219,133,191,216,243,16,26,14,15,21,28,38,205,244,32,48,58,44,44,52,40,22,247,254,5,251,8,19,16,255,4,25,35,43,51,34,1,12,20,15,9,255,247,1,17,16,25,4,10,18,14,12,45,17,245,239,228,220,174,150,124,94,87,41,51,68,80,52,45,80,89,104,87,249,196,205,223,224,237,246,240,228,218,211,3,32,62,154,137,79,47,22,24,24,25,6,237,229,234,7,19,32,30,26,12,19,36,85,117,158,175,180,173,157,244,221,220,234,222,204,196,209,233,239,242,232,220,223,196,183,208,206,231,234,224,203,221,237,233,221,209,179,160,138,137,142,132,118,91,76,78,71,56,31,19,9,2,11,14,17,11,3,0,249,19,35,37,80,79,65,53,51,39,26,22,19,12,5,13,16,19,22,45,66,113,102,103,113,135,104,73,60,63,82,65,74,90,50,21,19,1,3,18,24,17,19,35,38,48,249,240,252,244,224,246,253,244,247,223,206,179,138,92,118,139,89,120,126,162,214,243,252,3,3,2,253,236,210,45,25,194,226,255,3,2,254,1,8,5,5,5,14,245,240,248,234,148,161,120,121,180,224,219,230,236,233,225,229,217,233,211,103,110,143,171,157,184,146,102,86,46,16,6,236,250,241,230,238,250,255,253,6,13,5,252,244,229,161,106,53,29,15,246,204,175,173,144,146,247,250,9,2,243,24,30,31,40,52,21,4,246,242,237,237,238,237,191,158,202,239,232,3,14,19,28,38,52,98,152,244,249,43,63,94,100,93,39,21,6,253,2,23,36,50,207,225,213,233,245,226,221,233,14,3,236,198,228,1,251,228,226,220,205,222,228,226,188,129,89,185,167,171,213,211,218,245,36,137,131,85,48,11,241,250,11,14,34,63,87,133,177,193,217,235,8,54,101,90,76,62,57,67,59,41,13,1,6,23,29,41,80,134,168,223,11,31,31,41,38,27,23,16,236,201,194,222,233,237,0,10,4,249,251,14,40,31,7,236,167,136,172,171,15,28,40,44,36,14,5,14,253,234,234,242,244,239,230,0,34,74,88,69,69,16,245,253,248,222,55,61,91,4,5,252,239,217,213,200,150,131,139,145,160,185,209,220,228,210,202,170,131,108,11,246,242,14,27,26,25,28,34,45,60,64,90,151,246,229,236,255,247,253,2,243,235,229,244,10,177,173,195,198,185,202,187,135,103,81,31,246,232,245,241,250,246,251,10,17,16,6,15,21,10,10,19,18,23,29,59,46,31,19,14,20,24,22,10,24,62,57,58,45,28,27,42,58,48,43,53,38,47,38,10,249,15,17,36,68,69,47,29,31,47,12,17,31,48,52,62,121,140,174,210,254,22,47,56,29,2,234,53,39,48,80,88,121,162,179,203,214,223,214,212,229,254,215,158,112,161,208,228,222,218,184,171,158,174,217,242,51,32,156,130,127,156,189,199,210,194,157,105,77,94,64,37,18,14,6,230,217,207,203,224,224,201,230,11,18,38,55,33,18,3,9,16,34,19,218,145,145,120,90,85,66,51,39,20,238,209,173,180,253,245,219,218,237,238,231,232,197,184,236,254,248,240,252,14,30,13,254,4,255,246,254,8,253,4,233,7,36,53,36,33,30,9,12,33,65,85,80,71,52,39,47,61,77,121,112,92,153,144,154,148,115,63,8,20,25,25,19,41,34,30,15,207,101,75,170,169,188,209,201,201,177,207,217,216,199,214,229,254,28,56,102,137,163,167,149,205,163,130,145,159,191,203,188,171,194,164,148,165,128,141,179,189,150,165,170,184,226,234,221,193,227,60,88,134,165,150,143,102,59,43,29,20,17,22,28,23,21,23,27,45,39,20,224,225,237,227,225,243,255,253,252,231,188,180,214,211,218,229,153,124,78,33,17,13,3,3,25,31,46,33,7,251,3,22,30,31,24,40,71,122,79,15,177,111,86,100,117,105,169,207,199,206,220,237,251,247,231,221,177,178,98,98,60,73,134,182,173,182,176,144,64,60,113,149,97,47,36,38,24,254,218,225,242,241,229,221,166,76,39,25,45,69,109,156,187,87,77,86,89,91,31,17,33,67,62,65,67,106,136,204,213,210,207,216,11,17,36,72,101,15,233,224,231,243,250,251,252,25,28,23,11,1,11,4,253,16,21,11,4,13,29,53,69,109,171,198,237,229,18,55,52,68,37,48,111,68,45,41,33,30,45,63,77,67,59,27,26,39,94,189,218,234,238,6,10,244,246,25,24,20,41,34,6,255,248,246,234,229,12,34,36,29,36,56,38,38,47,74,72,56,39,39,190,224,254,12,12,20,36,24,7,11,22,47,220,205,50,30,7,207,201,20,26,32,28,13,236,67,19,2,243,254,3,13,19,16,29,30,31,8,253,1,29,79,113,71,72,225,234,252,2,254,234,220,237,0,3,226,209,181,132,133,155,150,98,84,57,76,129,169,10,25,6,16,17,33,49,72,101,101,65,40,30,22,10,16,52,95,93,84,78,80,104,138,124,79,46,25,251,211,171,183,224,207,207,230,255,243,229,238,242,251,0,244,232,228,233,226,219,224,187,169,159,254,2,3,15,22,61,47,247,190,169,183,189,161,105,67,52,63,40,19,247,4,14,9,255,30,30,20,36,32,21,236,243,238,215,198,188,197,189,239,245,254,12,5,34,30,46,33,23,27,33,25,18,7,246,244,241,4,12,16,47,87,108,120,105,115,159,201,212,218,171,99,10,214,113,65,22,27,6,232,201,202,213,3,252,238,238,244,230,220,186,175,181,190,182,223,229,231,5,247,1,5,9,242,233,237,249,225,201,235,0,236,215,182,170,164,148,196,229,239,202,177,218,244,249,3,6,228,182,166,188,245,247,247,251,248,251,234,225,241,238,1,189,197,180,167,175,162,144,133,124,117,129,147,145,169,134,123,154,123,123,124,80,61,77,53,58,78,97,90,58,63,74,50,43,36,9,7,4,9,5,0,242,241,0,5,14,15,10,12,21,22,20,8,242,241,22,248,245,235,219,203,19,8,21,32,39,46,63,253,88,68,96,94,61,49,40,66,49,28,73,61,23,21,21,25,12,18,15,12,25,22,14,0,254,242,233,3,43,25,16,10,7,10,5,2,15,25,38,42,35,62,91,160,170,126,109,93,95,107,118,112,147,122,86,68,56,49,47,59,53,48,46,75,123,102,54,27,45,79,116,122,127,90,52,36,8,229,18,43,18,4,1,231,238,254,1,29,46,26,16,12,17,10,10,5,32,74,39,20,35,69,113,99,201,230,223,230,241,13,14,10,239,211,192,207,223,4,27,22,240,243,248,8,23,18,226,217,220,227,214,201,200,178,151,77,66,80,104,144,200,241,45,40,52,93,116,119,123,94,103,140,180,216,235,249,3,248,12,20,12,11,10,2,12,4,10,26,10,15,4,243,237,223,191,162,110,76,90,66,57,100,157,173,184,222,245,25,7,5,9,11,12,2,252,244,0,240,252,26,27,8,4,5,11,17,10,4,248,17,50,40,3,234,230,232,1,231,250,47,107,125,169,211,116,104,86,86,75,89,125,193,228,1,253,227,211,219,225,248,241,245,246,238,251,239,193,208,253,246,2,185,204,243,253,0,245,251,21,39,83,94,68,74,126,154,109,9,205,153,209,203,181,164,125,85,76,88,97,95,71,23,9,10,11,21,26,251,193,200,247,255,11,255,242,225,219,222,222,236,243,246,2,8,18,251,246,1,255,9,14,14,22,26,18,21,5,243,250,4,255,254,251,217,121,132,111,99,87,70,33,28,15,33,43,36,10,0,255,233,188,155,138,144,220,206,146,125,88,87,94,191,253,231,230,27,37,17,8,0,251,198,142,56,37,44,46,21,31,34,47,36,33,58,92,84,44,15,10,0,7,1,242,230,237,229,246,226,233,237,252,224,239,247,253,20,8,209,152,139,119,102,141,195,0,173,213,248,23,24,18,10,13,21,23,24,36,43,44,43,25,40,80,113,102,132,144,175,215,245,250,242,216,206,130,57,62,104,113,96,130,150,133,132,129,72,31,254,11,18,15,7,8,241,228,246,246,16,37,25,39,65,65,77,82,140,160,172,193,228,243,160,173,224,238,244,9,9,250,223,208,203,203,215,223,244,14,36,34,17,3,5,241,201,160,142,186,225,3,27,20,10,28,1,222,234,227,224,231,205,182,180,210,230,244,250,254,241,214,203,169,130,90,72,41,27,0,233,204,177,133,120,74,244,218,197,206,247,254,10,29,44,93,149,171,233,46,70,87,85,69,46,39,29,1,232,215,220,242,37,25,37,55,63,61,65,87,103,148,179,195,207,195,164,205,220,211,202,241,238,242,13,25,51,105,137,97,80,85,77,73,73,44,39,40,61,63,85,87,70,47,39,31,46,83,169,222,22,27,26,22,31,51,87,126,143,174,177,207,227,246,10,29,37,53,41,31,19,21,55,46,43,24,14,24,37,39,47,24,3,245,226,196,152,142,162,186,210,221,224,231,204,234,3,13,10,8,26,27,1,251,2,236,254,10,16,35,56,82,55,35,26,106,69,88,151,169,151,146,140,88,48,23,30,69,61,51,40,49,85,74,59,28,16,254,242,11,40,32,17,3,251,234,206,217,196,7,247,239,245,249,242,246,211,191,214,3,9,19,43,94,79,79,101,122,102,47,18,252,224,188,235,7,9,1,251,230,195,182,123,73,72,92,124,153,12,11,7,4,253,249,252,247,229,190,190,224,226,221,228,201,174,75,91,103,97,173,127,133,123,148,174,202,174,189,237,251,237,244,253,241,228,212,220,213,219,7,90,120,116,124,112,107,246,235,221,237,190,222,254,21,32,26,14,33,20,20,21,24,35,41,53,45,42,43,25,184,143,76,56,244,235,218,231,2,18,21,12,246,20,11,252,253,218,230,195,223,246,252,8,32,99,130,158,193,158,136,229,225,209,208,199,176,203,161,95,87,92,120,55,44,24,8,231,188,134,43,8,245,236,232,241,5,248,230,237,251,12,16,29,29,21,27,23,3,252,0,17,47,31,18,10,249,4,248,22,10,236,59,74,97,75,44,47,41,18,6,12,8,6,17,27,31,29,19,8,22,44,234,5,45,94,104,62,59,54,55,91,63,7,13,20,18,20,23,48,51,76,51,30,31,27,52,83,59,52,23,14,48,38,17,250,205,234,29,100,86,68,59,41,32,20,11,8,82,103,11,247,243,244,247,6,17,27,47,50,35,30,24,43,77,139,173,167,150,162,204,190,207,237,254,252,253,36,47,75,90,67,28,13,188,130,32,12,18,9,106,90,61,65,90,92,99,141,186,165,155,196,230,224,215,253,217,209,206,201,231,255,10,40,47,19,245,207,172,177,77,60,89,181,190,224,248,255,249,244,238,240,151,175,183,220,65,135,187,209,253,255,240,23,26,45,39,10,120,146,117,227,177,218,226,230,209,207,217,197,139,115,76,58,101,111,70,34,29,27,19,4,6,20,14,12,234,197,187,202,182,161,200,237,227,207,178,160,176,36,13,7,24,46,38,55,78,95,56,26,3,8,22,49,57,254,226,160,162,174,163,180,145,171,124,66,54,124,84,70,55,44,40,46,249,215,229,231,146,127,147,181,190,151,88,27,4,234,249,231,223,224,224,243,254,241,132,133,57,17,6,15,13,2,247,249,226,208,4,40,45,55,17,1,217,246,12,1,2,245,237,230,245,6,1,243,243,246,45,138,176,228,251,21,92,48,44,31,46,28,6,21,15,24,40,84,115,118,68,116,108,83,61,55,47,38,61,64,46,54,72,62,83,140,127,123,54,6,203,155,133,128,165,182,206,178,134,101,101,96,16,22,36,32,11,247,67,66,39,44,24,31,30,30,15,252,223,171,59,44,170,179,174,163,187,218,253,206,215,220,207,185,132,112,18,222,224,241,184,171,195,207,226,216,212,10,16,28,25,27,51,75,116,128,147,192,170,192,184,150,104,126,92,87,132,117,146,222,204,177,159,168,219,237,232,91,236,217,160,109,66,95,156,186,208,212,226,43,97,164,155,157,118,25,245,1,250,235,223,216,253,7,20,49,70,83,93,109,222,106,74,26,19,23,22,5,5,12,242,252,46,45,35,17,33,22,37,22,46,70,67,62,68,87,180,211,227,229,217,255,197,213,219,3,253,248,8,240,196,131,133,154,236,226,174,175,208,233,218,188,144,147,58,36,5,3,16,26,7,4,237,221,5,49,43,19,62,59,69,19,0,236,230,248,246,26,33,22,0,13,19,140,122,117,69,24,255,235,206,128,94,77,63,69,66,79,107,69,68,3,233,241,228,199,217,223,208,239,251,7,3,9,15,9,247,244,0,9,247,186,157,101,121,28,24,24,24,98,138,186,223,219,197,182,172,217,183,166,217,145,216,134,169,199,201,162,91,61,26,25,23,31,41,34,31,11,0,212,205,216,218,223,247,253,251,218,216,200,143,121,129,84,54,23,13,249,37,35,39,35,134,158,229,255,53,76,63,132,155,170,245,255,10,1,250,71,76,55,43,49,55,45,82,126,132,95,89,227,244,250,223,218,203,190,40,22,19,18,33,38,41,108,116,75,24,255,251,246,233,14,241,250,247,3,29,54,51,18,13,37,28,3,241,251,16,252,234,10,41,25,17,29,19,223,10,26,252,254,66,84,59,100,113,242,220,203,205,222,52,57,90,73,98,109,60,30,8,135,140,155,48,21,21,51,244,9,18,21,42,52,70,98,101,66,90,68,33,10,4,15,25,26,25,55,205,233,228,215,226,14,252,221,235,26,25,16,254,34,89,245,240,255,236,233,194,222,247,24,33,47,44,37,44,37,21,8,34,76,62,39,95,67,74,88,74,35,19,57,88,120,144,173,212,238,9,18,27,63,38,32,9,6,15,29,30,45,150,187,177,172,138,118,48,58,55,158,174,156,92,32,17,24,48,24,29,35,31,22,252,170,138,3,11,245,2,30,8,0,13,39,44,43,43,53,40,235,246,251,244,21,49,101,130,177,113,70,96,135,148,159,0,9,12,15,14,5,247,239,239,241,3,36,24,29,107,109,152,183,191,178,108,56,93,47,28,32,40,40,83,142,224,6,28,32,13,239,255,255,253,12,8,12,26,28,52,146,135,143,85,50,18,246,0,2,244,19,36,74,127,109,103,90,81,68,72,85,166,132,81,39,47,45,35,33,31,225,221,205,183,167,129,11,113,183,208,222,7,240,223,227,252,227,11,10,3,238,249,11,242,137,190,159,12,0,247,239,254,254,195,92,42,204,211,206,219,215,199,156,127,30,22,250,162,124,219,34,27,11,10,242,205,229,42,46,42,5,22,52,52,49,56,47,253,234,204,183,148,183,178,126,141,85,9,251,242,247,243,12,12,32,62,77,53,30,20,40,41,36,25,19,15,21,19,251,9,208,240,221,2,3,3,30,63,85,107,114,113,134,112,66,70,113,141,171,215,210,248,214,103,78,75,58,19,38,4,250,232,197,156,180,57,46,22,0,248,251,229,244,248,242,230,239,241,213,163,152,145,155,203,206,193,0,158,235,1,7,7,12,22,245,244,0,254,162,156,188,223,254,11,251,243,239,227,229,213,136,168,120,96,109,165,217,222,244,187,154,118,138,200,203,249,32,24,17,19,21,5,251,5,3,252,2,6,9,1,206,240,12,255,243,250,106,178,208,160,147,195,170,166,162,142,133,114,154,116,138,189,152,119,94,193,179,127,107,168,130,32,31,30,28,37,38,37,48,108,126,100,117,70,33,29,47,40,54,41,32,252,2,245,5,234,225,208,237,6,2,14,11,22,19,10,255,17,22,30,212,207,228,61,160,219,252,251,201,201,170,241,249,10,114,60,47,91,130,85,49,39,36,112,76,30,22,0,36,3,4,254,33,23,12,254,251,14,43,22,44,17,253,252,243,243,7,1,200,189,211,9,45,86,73,10,252,8,16,7,0,17,15,15,18,24,41,41,41,30,11,245,92,72,73,158,125,106,91,127,118,58,117,109,98,122,149,118,92,86,72,45,58,72,86,52,40,62,58,26,102,151,182,147,77,14,2,11,27,3,161,128,156,178,130,33,53,54,63,23,169,97,72,70,230,236,255,3,254,40,48,92,5,254,1,19,13,11,255,253,6,233,20,92,119,61,14,252,2,71,121,137,201,228,242,214,227,2,45,24,26,9,254,218,217,207,183,100,63,44,59,100,201,243,238,237,244,253,62,94,196,209,192,44,183,210,210,192,109,17,21,16,117,98,107,145,66,82,63,162,243,8,2,241,19,11,20,7,9,16,18,11,26,253,10,4,247,229,244,238,234,158,78,65,98,79,69,40,47,184,172,183,142,112,231,3,255,12,8,4,255,243,227,226,251,14,200,224,53,36,249,12,7,45,7,9,225,230,252,45,60,81,90,65,228,238,252,45,146,232,246,2,38,72,108,65,77,58,56,68,63,83,234,246,4,17,26,226,213,195,213,221,254,222,224,23,47,71,163,243,89,141,103,31,20,13,6,12,10,255,242,85,106,117,67,136,176,204,143,82,137,68,54,54,99,122,102,13,243,236,240,17,16,39,99,18,250,242,7,34,30,205,204,219,213,222,230,244,12,24,23,5,219,16,253,247,244,13,10,23,49,34,225,228,232,17,23,105,149,147,126,108,87,176,1,31,21,12,17,245,51,1,253,241,235,18,89,139,143,126,250,226,177,246,128,81,44,26,19,12,253,216,152,39,68,61,65,35,8,51,30,24,34,8,129,41,26,253,19,14,18,249,221,211,230,3,243,239,225,227,251,161,180,254,248,255,254,7,222,227,172,122,98,136,101,99,155,223,8,27,12,247,248,37,23,13,30,43,38,67,56,8,37,73,62,70,114,145,120,129,153,209,254,2,248,251,82,207,161,75,57,189,188,146,168,139,96,60,255,11,25,22,10,3,1,224,198,59,38,12,28,46,67,128,90,60,117,227,68,130,179,214,247,250,0,18,13,216,8,248,21,30,17,9,74,1,211,201,159,189,146,219,249,237,195,207,99,58,40,1,237,255,251,243,178,147,120,94,98,21,206,6,19,239,242,21,130,173,227,20,47,64,131,125,31,32,56,53,40,42,195,31,1,31,61,78,78,66,67,94,117,186,203,187,14,231,215,209,225,218,209,236,10,24,40,81,143,84,87,54,76,60,73,68,20,25,92,82,106,117,36,26,16,31,56,206,33,33,95,95,125,157,218,236,10,10,19,22,59,44,33,20,7,37,65,26,13,251,4,45,47,47,52,255,192,160,129,118,145,194,184,194,223,154,208,228,23,14,253,240,208,122,29,230,25,7,4,19,15,40,65,175,198,161,135,113,111,57,52,47,98,68,33,38,29,119,127,112,245,234,217,243,17,44,241,207,222,39,3,10,4,223,244,253,0,244,230,251,186,169,184,7,18,7,67,116,136,20,17,14,172,124,135,249,9,13,5,242,238,70,42,53,94,19,17,11,2,0,1,8,6,18,254,143,145,2,239,192,202,170,104,76,215,130,22,231,242,249,1,14,219,5,32,29,27,15,33,63,201,179,252,203,200,17,16,29,15,17,44,239,36,169,217,147,4,33,142,100,105,124,102,8,189,95,117,147,20,43,50,36,14,172,136,162,153,90,22,225,223,34,237,221,209,218,21,56,39,25,12,27,43,250,253,204,111,53,9,255,234,246,35,228,183,27,23,35,52,249,21,38,24,7,248,12,40,79,121,135,70,73,65,153,241,5,20,3,21,25,5,0,26,28,130,124,76,43,46,33,15,39,40,46,6,253,3,193,247,223,241,216,250,252,231,67,44,19,24,18,50,177,33,8,106,164,250,8,6,233,239,47,26,27,128,12,241,13,251,240,135,120,81,24,70,103,86,109,100,144,193,246,179,192,203,182,153,18,9,9,18,35,76,47,225,241,40,0,13,15,254,178,188,199,230,226,229,235,90,12,18,33,26,7,12,19,234,205,189,171,189,244,215,168,241,245,16,47,138,81,90,104,3,12,232,172,199,170,147,79,111,219,52,70,72,50,61,81,240,205,206,2,106,178,168,207,92,58,21,8,13,18,31,15,189,155,46,71,64,14,33,77,6,19,239,224,239,255,138,177,119,247,152,17,29,150,58,42,59,41,74,58,37,35,26,246,157,3,216,227,246,88,136,59,55,107,97,71,20,61,33,29,234,207,26,160,177,172,139,49,51,132,234,89,108,33,202,201,9,26,14,9,35,124,118,24,225,236,244,36,55,52,140,47,13,13,229,248,255,250,13,36,26,17,236,200,250,56,3,2,31,60,21,73,72,82,164,254,238,199,34,29,90,102,66,81,238,248,177,161,60,15,248,238,247,24,161,200,52,211,12,31,80,57,154,248,10,6,32,21,3,235,229,31,132,9,9,98,102,248,150,160,73,6,201,224,41,63,67,15,52,27,197,200,201,142,36,21,57,76,82,71,2,249,18,229,51,128,119,4,254,251,19,29,13,42,236,16,236,66,15,178,247,211,91,115,111,77,57,10,134,236,34,40,220,230,1,63,97,206,2,214,235,213,251,4,46,44,50,49,30,23,221,0,14,63,97,181,21,82,113,134,144,255,6,8,24,23,20,18,209,169,80,98,116,180,170,153,70,45,14,15,22,31,41,5,244,54,47,45,52,254,253,241,24,142,27,117,137,151,11,39,17,9,14,177,200,200,43,33,20,37,24,11,23,35,244,13,46,9,14,241,244,8,32,51,46,234,16,23,30,96,17,60,127,30,16,48,45,54,63,65,190,180,152,207,229,219,233,222,136,237,249,8,116,181,185,36,28,17,40,74,81,10,235,163,217,43,49,32,17,4,250,37,11,26,242,8,244,39,103,132,143,150,136,236,54,31,219,186,114,105,69,34,7,251,6,213,204,185,225,165,200,216,11,8,18,248,92,181,9,16,243,231,248,154,185,123,74,242,11,8,237,141,161,121,221,172,143,13,24,23,2,30,31,55,77,31,253,17,10,167,207,140,38,242,224,243,246,15,193,120,161,17,19,121,84,218,2,6,225,125,41,13,23,7,229,23,1,213,245,244,212,233,216,133,113,64,39,46,221,38,81,66,62,253,16,192,226,2,51,52,95,29,214,185,86,221,42,89,5,38,46,68,37,44,43,59,248,237,222,171,63,59,52,35,4,1,224,216,168,25,33,34,29,32,221,40,38,75,220,26,74,181,105,32,7,245,247,142,59,110,198,49,162,56,23,51,17,17,194,151,224,21,180,168,69,62,109,176,222,64,93,121,73,177,118,255,253,48,61,56,9,185,41,9,254,196,43,42,136,33,41,33,216,194,169,29,14,0,32,32,30,65,128,120,144,101,39,241,127,15,64,240,248,124,40,119,226,2,118,173,100,41,39,166,15,21,37,9,60,24,239,166,139,7,235,142,88,39,};

namespace windmap {
  map<int, int> getid;
  vector<vector<int> > neighbors;
  vector<cell*> samples;
  
  int getId(cell *c) {
    auto i = fieldpattern::fieldval_uniq(c);
    auto &id = getid[i];
    if(id == 0) { samples.push_back(c); id = size(samples); }
    return id-1;
    }
  
  vector<unsigned char> windcodes;

  void create() {
    samples.clear();
    neighbors.clear();
    getid.clear();
    getId(currentmap->gamestart());
    for(int k=0; k<size(samples); k++) {
      cell *c = samples[k];
      neighbors.emplace_back();
      auto &v = neighbors.back();
      for(int l=0; l<c->type; l++) v.push_back(getId(createMov(c, l)));
      }
    
    int N = size(samples);
    
    int* precomp = NULL;

    windcodes.resize(N);
    
    if(N == 18920) precomp = windcodes18920;
    if(N == 5676) precomp = windcodes5676;
    
    if(precomp && size(currfp.matrices)) {
      int randval = hrand(size(currfp.matrices));
      for(int i=0; i<N; i++)
        windcodes[i] = precomp[getid[fieldpattern::fieldval_uniq_rand(samples[i], randval)]-1];
      return;
      }
    
    int tries = 0;
    int maxtries = specialland == laVolcano || specialland == laBlizzard || chaosmode ? 20 : 1;
    tryagain:

    for(int i=0; i<N; i++) windcodes[i] = hrand(256);
    
    bool inqueue[N];
    vector<int> tocheck;
    for(int i=0; i<N; i++) tocheck.push_back(i), inqueue[i] = true;
    hrandom_shuffle(&tocheck[0], size(tocheck));
    
    for(int a=0; a<size(tocheck); a++) {
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
      printf("tocheck size = %d\n", size(tocheck));
      printf("if(N == %d) {\n", N);
      printf("  windcodes = {");
      for(int i=0; i<N; i++) printf("%d,", windcodes[i]);
      printf("};\n");
      printf("  return;\n");
      printf("  }\n");
      }
    }
  
  int at(cell *c) {
    return windmap::windcodes[windmap::getId(c)];
    }

  }

// Halloween namespace

namespace halloween {
  cell *dragoncells[4];
  vector<cell*> srch;

  cell *farempty(bool lastresort = false) {
    int maxdist = 0;
    vector<cell*> validcells;
    int firstfar1 = 0;
    for(int i=1; i<size(dcal); i++) {
      bool okay = 
        dcal[i]->item == itNone && dcal[i]->monst == moNone && dcal[i]->wall == waNone;
      if(lastresort)
        okay = dcal[i]->wall != waChasm && !isMultitile(dcal[i]->monst);
      if(okay) {
        if(dcal[i]->cpdist > maxdist) 
          firstfar1 = size(validcells),
          maxdist = dcal[i]->cpdist; 
        validcells.push_back(dcal[i]);
        }
      }
    
    int qvc = size(validcells);
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
  
  void getTreat(cell *where) {
    if(!items[itTreat]) reset();
    gainItem(itTreat);
    farempty()->item = itTreat;
    int itr = items[itTreat];
    items[itOrbTime] += 30;
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
      else if(dragoncount && !nonbitrunc && !mcount) {
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
#if ISMOBILE==0
      addMessage(XLAT("Hint: use arrow keys to scroll."));
#endif
      }
    else if(items[itTreat] == 2) {
#if ISMOBILE==0
      addMessage(XLAT("Hint: press 1 2 3 4 to change the projection."));
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
  }

// ... also includes the Ivory Tower

namespace dungeon {

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
      if(torus) return;
      int x, y;
      tie(x, y) = cell_to_pair(c);
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
      
      char ch = euclid6 ? tab6[y0][(x+(y+1)/2)&7] : tab4[y0][x&7];
      
      if(ch == '#')
        c->wall = waPlatform;
      else if(ch == 'L')
        c->wall = waLadder;
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
            for(int i=0; i<c2->type; i++) if(c2->mov[i] && isNeighbor(c2->mov[i], c3))
              c4 = c2->mov[i];
            }
          rdepths[i] = c2 && c3 && c4 && (c2->landflags == 3 || c3->landflags == 3 || c4->landflags == 3);
          c2 = chosenDown(c2, 1, 0); // if(!c2) break;
          c3 = chosenDown(c3, -1, 0);
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
        cell *c4 = chosenDown(c, 1, 1);
        cell *c5 = chosenDown(c, -1, -1);
        for(int i=0; i<3; i++) {
          if(coastvalEdge(c2) == 0) break;
          if(c2 && c4 && c4->landflags == 3 && c2->landflags != 3 && c4 == chosenDown(c2, 1, 1)) 
            c->wall = waLadder;
          if(c3 && c5 && c5->landflags == 3 && c3->landflags != 3 && c5 == chosenDown(c3, -1, -1)) 
            c->wall = waLadder;
          buildEquidistant(c4); buildEquidistant(c5);
          if(c2) c2 = chosenDown(c2,  1, 0);
          if(c3) c3 = chosenDown(c3, -1, 0);
          if(c4) c4 = chosenDown(c4,  1, 0);
          if(c5) c5 = chosenDown(c5, -1, 0);
          }
        }
      }
    else c->wall = waCIsland;
    }
  
  int dungeonFlags(cell *c) {
    if(!c) return 0;
    buildEquidistant(c);
    bool rdepths[5];
  
    cell *c2 = c;
    cell *c3 = c;
      
    int switchcount = 0;
    for(int i=0; i<5; i++) {
      if(coastvalEdge(c2) == 0) { 
        rdepths[i] = false;
        }
      else {
        cell *c4 = c2;
        if(c2 != c3 && !isNeighbor(c2, c3)) {
          for(int i=0; i<c2->type; i++) if(c2->mov[i] && isNeighbor(c2->mov[i], c3))
            c4 = c2->mov[i];
          }
        rdepths[i] = c2 && c3 && c4 && (c2->landflags == 3 || c3->landflags == 3 || c4->landflags == 3);
        if((c2&&c2->landflags == 1) || (c3&&c3->landflags == 1) || (c4&&c4->landflags == 1))
          switchcount++;
        c2 = chosenDown(c2, 1, 0); // if(!c2) break;
        c3 = chosenDown(c3, -1, 0);
        if(!c2) { towerError(c); return 0; }
        if(!c3) { towerError(c); return 0; }
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
        int df1 = dungeonFlags(chosenDown(c,1,1));
        int df2 = dungeonFlags(chosenDown(c,-1,-1));
        
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
        /* int q = 0;
        cell* downs[MAX_EDGE];
        forCellEx(c2, c) {
          buildEquidistant(c2);
          if(coastvalEdge(c2) > coastvalEdge(c)) downs[q++] = c2;
          }
        if(q) downs[hrand(q)]->wall = waLadder;
        */
        cell *c2 = 
          c->wparam == 1 ? chosenDown(c, 1, 2) :
          c->wparam == 2 ? chosenDown(c, -1, -2) :
          c->wparam == 3 ? chosenDown(c, 1, 3) :
          c->wparam == 4 ? chosenDown(c, -1, -3) :
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
  
  void all(cell *c, int d) {
    if(d == 8 && (c->land == laIvoryTower || c->land == laDungeon) && !euclid) {

      if(hrand(1000) < 75 && (c->landparam & 1) ) {
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
    if(d == 8 && c->land == laDungeon) build(c);
    if(d == 7 && c->land == laDungeon) buildPlates(c);
    }
  }
