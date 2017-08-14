namespace inv {

  bool on;
  int usedup[ittypes];
  int remaining[ittypes];

  int rseed;
  bool usedForbidden;

    
  void init() {
    rseed = hrandpos();
    usedForbidden = false;
    for(int i=0; i<ittypes; i++) usedup[i] = 0;
    }
  
  static const int MIRRORED = 1000;
  static const int TESTMIRRORED = 900;

  struct lateextraorb {
    eItem treasure;
    eItem orb;
    int at;
    };
  
  vector<lateextraorb> lateextraorbs = {
    {itPower, itOrbFlash},
    {itPower, itOrbSpeed},
    {itPower, itOrbAether},
    {itPower, itOrbWinter},
  
    {itTrollEgg, itOrbFish},
    {itTrollEgg, itOrbStunning},
    {itTrollEgg, itOrbLuck},
    {itTrollEgg, itOrbLife},
    {itTrollEgg, itOrbDigging},
    {itTrollEgg, itOrbSpace},
  
    {itFulgurite, itOrbLightning},
    {itWindstone, itOrbSpeed},
    {itDragon, itOrbDragon},
    {itSlime, itOrbFlash},
    {itDodeca, itOrbShield}
    };

  int mirrorqty0(eItem orb) {
    if(shmup::on && isShmupLifeOrb(orb)) 
      return 3;
    if(orb == itOrbWater) return 10;
    if(orb == itOrbSummon) return 9;
    if(orb == itOrbEmpathy) return 9;
    if(orb == itOrbMatter) return 9;
    if(orb == itOrbLuck) return 8;
    if(orb == itOrbSpace) return 7;

    if(orb == itOrbWinter) return 6;
    if(orb == itOrbLife) return 6;
    if(orb == itOrbLove) return 6;
    if(orb == itOrbRecall) return 6;
    if(orb == itOrbDigging) return 6;
    
    if(orb == itOrbTime) return 5;
    if(orb == itOrbAir) return 5;
    if(orb == itOrbFish) return 5;
    if(orb == itOrbStunning) return 5;
    if(orb == itOrbUndeath) return 5;
    if(orb == itOrb37) return 5;
    if(orb == itOrbDomination) return 5;
    if(orb == itOrbBull) return 5;
    if(orb == itOrbHorns) return 5;

    if(orb == itOrbAether) return 4;
    if(orb == itOrbInvis) return 4;
    if(orb == itOrbFire) return 4;
    if(orb == itOrbDragon) return 4;
    if(orb == itOrbIllusion) return 4;
    if(orb == itOrbDiscord) return 4;
    if(orb == itOrbBeauty) return 4;

    if(orb == itOrbMirror) return 1;
    return 3;
    }
  
  int mirrorqty(eItem orb) {
    return int(mirrorqty0(orb) * sqrt(1.000001+items[itPower]/20.));
    }
    
  struct nextinfo { int min, real, max; };
  
  nextinfo next[ittypes];

  mt19937 invr;
  
  void sirand(int i) {
    invr.seed(i);
    }
  
  int irand(int i) {    
    return invr() % i;
    }
    
  void gainOrbs(eItem it, eItem o) {
    auto& nx = next[o == itHyperstone ? o : it];
    int qty = items[it];
    if(it == itHolyGrail) {
      remaining[itOrbIllusion] += qty;
      nx = {qty+1, qty+1, qty+1};
      }
    else {
      bool nextfound = false;
      if(qty >= 10) remaining[o]++;
      else nx = {10,10,10}, nextfound = true;
      int last = 10;
      for(int k=0; k<30 || !nextfound; k++) {
        int maxstep = 15 + 5 * k;
        if(o == itOrbMirror)
          maxstep += 5 * (k-1) * (k-2);
        else
          maxstep += (k-1) * (k-2);
        int xnext;
        if(k >= 30 || o == itOrbMirror) {
          xnext = last + maxstep/2; last = xnext-1;
          maxstep = 1;
          }
        else 
          xnext = last + 1 + irand(maxstep);
        if(xnext > qty && !nextfound) 
          nx = { last+1, xnext, last + maxstep }, nextfound = true;
        if(xnext <= qty) remaining[o]++; 
        last = xnext;
        }
      }
    }

  int nextp2(int i) {
    int z = 1;
    while(z <= i) z <<= 1;
    return z;
    }
    
  void gainMirrors(int qtl) {
    while(qtl > 0) qtl >>= 1, remaining[itOrbMirror]++;
    }

  vector<eItem> offensiveOrbs = {
    itOrbFlash, itOrbLightning, itOrbPsi, itOrbThorns,
    itOrbFreedom, itOrbSword, itOrbSword2,
    itOrbHorns, itOrbDragon, itOrbStunning
    };
  
  vector<eItem> elementalOrbs = {itOrbFire, itOrbWater, itOrbDigging, itOrbAir};

  vector<eItem> demonicOrbs = {itOrbFire, itOrbHorns, itOrbSummon};
  
  bool isIn(eItem o, vector<eItem>& l) {
    for(auto it: l) if(it == o) return true;
    return false;
    }
  
  void gainRandomOrbs(vector<eItem> orblist, eItem which, int each, int reduce) {
    const int qoff = size(orblist);
    for(int i=1; i<qoff; i++) swap(orblist[i], orblist[irand(1+i)]);
    for(int i=0; i<20; i++) {
      if((i+1)*each <= items[which] - reduce)
      remaining[orblist[i%qoff]]++;
      }
    }
  
  void compute() {
    for(int i=0; i<ittypes; i++) remaining[i] = -usedup[i];
    for(int i=0; i<ittypes; i++) if(usedup[i] >= TESTMIRRORED) {
      remaining[i] += MIRRORED;
      remaining[i] -= mirrorqty0(eItem(i));
      remaining[i] += mirrorqty(eItem(i));
      }
    
    sirand(rseed);
    
    vector<pair<eItem, eItem>> itempairs;
    
    for(int k=0; k<ORBLINES; k++) {
      auto oi = orbinfos[k];
      eLand l = oi.l;
      eItem it = treasureType(l);
      eItem o = oi.orb;
      if(oi.gchance) itempairs.emplace_back(it, o);
      else if(items[it] >= 10) remaining[o]++;
      }
    
    sort(itempairs.begin(), itempairs.end());
    
    gainOrbs(itShard, itOrbMirror);
    gainOrbs(itHyperstone, itOrbMirror);
    for(auto p: itempairs)
      gainOrbs(p.first, p.second);
    if(items[itOrbYendor]) remaining[itOrbMirror]++;

    gainMirrors(items[itOrbYendor]);
    gainMirrors(items[itHolyGrail]);
    
    if(princess::reviveAt) {
      remaining[itOrbLove]++;
      int s = gold(NO_LOVE);
      int last = princess::reviveAt;
      for(int k=0;; k++) {
        int nextstep = 50 + 20 * k;
        last += nextstep;
        if(last > s) {
          next[itSavedPrincess] = {last, last, last};
          break;
          }
        else { last += nextstep; remaining[itOrbLove]++; }
        }
      }
    
    gainRandomOrbs(offensiveOrbs, itBone, 25, 0);
    gainRandomOrbs(elementalOrbs, itElemental, 12, 0);
    gainRandomOrbs(demonicOrbs, itHell, 20, 100);
    
    for(auto& it: lateextraorbs) {
      it.at = 10 + irand(41);
      if(items[it.treasure] >= it.at) remaining[it.orb]++;
      }
    
    if(items[itOrbLove] && !items[itSavedPrincess]) items[itSavedPrincess] = 1;
    
    int& r = remaining[itGreenStone];
    
    if(items[itBone] >= 0) {
      for(int i=0; i<ittypes; i++) if(i != itGreenStone) {
        r += usedup[i];
        if(usedup[i] >= TESTMIRRORED) r -= (MIRRORED - mirrorqty0(eItem(i)));
        }
      }
    
    items[itGreenStone] += r;
    usedup[itGreenStone] += r;
    r = 0;
    
    if(shmup::on) for(int i=0; i<ittypes; i++) {
      if(remaining[i] && isShmupLifeOrb(eItem(i))) {
        gainLife();
        remaining[i]--;
        usedup[i]++;
        }
      }

    items[itInventory] = 0;
    for(int i=0; i<ittypes; i++) 
      if(i != itGreenStone && i != itOrbYendor) 
        items[itInventory] += remaining[i];
    
    }
  
  map<char, eItem> orbmap;
  string orbkeys = "zfwplSetsTaMIYgCcPOWAFydLGRUuouE.,bVNhDwWZnrvhBm0123456789";
  typedef pair<int, int> pxy;
  vector<pxy> orbcoord;

  int sq(pxy p) {
    int zz = (2*p.first+p.second)*(2*p.first+p.second) + 3*p.second*p.second;
    zz *= 20; zz += abs(p.second);
    zz *= 20; zz += abs(p.first);
    zz *= 4; zz += (p.first>0)*2+(p.second>0);
    return zz;
    }

  bool plain;      

  eItem which;
  
  bool mirroring;
  
  const char* helptext = 
    "You are playing in the Orb Strategy Mode. Collecting treasure "
    "gives you access to magical Orb powers. In this mode, "
    "unlocking requirements are generally higher, and "
    "several quests and lands "
    "give you extremely powerful Orbs of the Mirror.\n";

  string extraline(eItem it, string s) {
    return " "+XLAT1(iinf[it].name) + " ("+s+")";
    }

  void show() {
  
    gamescreen(2);
    cmode = sm::CENTER;

    orbcoord.clear();
    for(int y=-3; y<=3; y++) for(int x=-4; x<=4; x++) if(x+y<=4 && x+y >= -4 && (x||y))
      orbcoord.emplace_back(x,y);
    sort(orbcoord.begin(), orbcoord.end(), [](pxy p1, pxy p2) {
      return sq(p1) < sq(p2); });
    
    ld rad = min(vid.xres, vid.yres) / 20;
    ld rad3 = int(rad * sqrt(3));
    
    compute();
    orbmap.clear();
    which = itNone;
        
    if(plain) dialog::init(XLAT(mirroring ? "mirror what?" : "inventory"), forecolor, 150, 100);

    int j = 0, oc = 6;
    for(int i=0; i<ittypes; i++) {
      eItem o = eItem(i);
      if(itemclass(o) == IC_ORB) {
        char c = orbkeys[j++];
        if(remaining[i] || usedup[i]) {
          orbmap[c] = o;
          if(plain) 
            dialog::addSelItem(XLAT1(iinf[o].name), its(remaining[i]), c);
          else {
            auto pos = orbcoord[oc++];
            ld px = vid.xcenter + 2*rad*pos.first + rad*pos.second;
            ld py = vid.ycenter + pos.second * rad3;
            int icol = iinf[o].color;
            if(!remaining[i]) icol = gradient(icol, 0, 0, .5, 1);
            bool gg = graphglyph();
            
            if(!hiliteclick) {
              if(gg) {
                initquickqueue();
                transmatrix V = atscreenpos(px, py, rad*2);
                drawItemType(o, NULL, V, icol, ticks/3 + i * 137, false);
                quickqueue();
                }
              
              int tcol = remaining[i] ? darkenedby(icol, 1) : 0;
  
              if(remaining[i] != 1 || !gg)
                displaystr(px, py, 2, gg?rad:rad*3/2, remaining[i] <= 0 ? "X" : remaining[i] == 1 ? "o" : its(remaining[i]), tcol, 8);
              }
            
            bool b = hypot(mousex-px, mousey-py) < rad;
            if(b) {
              getcstat = c, 
              which = o;
              }
            }
          }
        }
      }

    if(plain) {
      dialog::addBreak(750);
      dialog::addItem(XLAT("help"), SDLK_F1);
      dialog::addItem(XLAT("return to the game"), 'i');
      dialog::display();
      which = orbmap[getcstat];
      }
    else {
      if(which == itNone) {
        displaystr(vid.xres/2, vid.fsize*2, 2, vid.fsize*2, XLAT("Which orb to use?"), 0xC0C0C0, 8);
        }
      else {
        int icol = iinf[which].color;
        displaystr(vid.xres/2, vid.fsize*2, 2, vid.fsize*2, XLAT1(iinf[which].name), icol, 8);
        auto oi = getOrbInfo(which);
        
        if(mirroring)
          displaystr(vid.xres/2, vid.fsize*4, 2, vid.fsize, usedup[which] >= TESTMIRRORED ? XLAT("already mirrored") : XLAT("Uses to gain: %1", its(mirrorqty(which))), icol, 8);
        else {
          eItem t = treasureType(oi.l);
          string s = XLAT("Unlocked by: %1 in %2", t, oi.l);
          if(next[t].min == next[t].max)
            s += XLAT(" (next at %1)", its(next[t].min));
          else
            s += XLAT(" (next at %1 to %2)", its(next[t].min), its(next[t].max));

          displaystr(vid.xres/2, vid.fsize*4, 2, vid.fsize, s, icol, 8);
          
          
          string extras = "";
          for(int k=0; k<ORBLINES; k++) {
            auto oi = orbinfos[k];
            if(oi.gchance || oi.orb != which) continue;
            eItem it = treasureType(oi.l);
            extras += extraline(it, "10");
            }
          
          if(which == itOrbMirror) {
            extras += extraline(itOrbYendor, its(nextp2(items[itOrbYendor])));
            extras += extraline(itHolyGrail, its(nextp2(items[itHolyGrail])));
            auto& nx = next[itHyperstone];
            extras += extraline(itHyperstone, its(nx.min));
            }
          if(isIn(which, offensiveOrbs)) extras += extraline(itBone, its(items[itBone]/25*25+25) + "?");
          if(isIn(which, elementalOrbs)) extras += extraline(itElemental, its(items[itBone]/20*20+20) + "?");
          if(isIn(which, demonicOrbs)) extras += extraline(itHell, its(max(125, items[itHell]/25*25+25)) + "?");
          for(auto& a: lateextraorbs) if(a.orb == which)
            extras += extraline(a.treasure, items[a.treasure] >= a.at ? (its(a.at)+"!") : "10-50");

          if(extras != "")
            displaystr(vid.xres/2, vid.fsize*5, 2, vid.fsize, XLAT("Extras:")+extras, icol, 8);
          }

        if(remaining[which] != 1 || usedup[which]) {
          string s = XLAT("Number of uses left: %1", its(remaining[which]));
          int us = usedup[which];
          if(us >= TESTMIRRORED) s += XLAT(" (mirrored)"), us = us - MIRRORED + mirrorqty0(which);
          if(us) s += XLAT(" (used %1 times)", its(us));
          displaystr(vid.xres/2, vid.yres - vid.fsize*6, 2, vid.fsize, s, icol, 8);
          }

#if ISMOBILE==0
        string hot = XLAT1("Hotkey: "); hot += getcstat;
        displaystr(vid.xres/2, vid.yres - vid.fsize*5, 2, vid.fsize, hot, icol, 8);
#endif
  
        eOrbLandRelation olr = getOLR(oi.orb, getPrizeLand());
        eItem tr = treasureType(oi.l);
        
        int col = 0;
        if(olr == olrDangerous) col = 0xC00000;
        if(olr == olrUseless) col = 0x808080;
        if(olr == olrForbidden) col = 0x804000;
  
        if(col)
          displaystr(vid.xres/2, vid.yres - vid.fsize*4, 2, vid.fsize, XLAT(olrDescriptions[olr], cwt.c->land, tr, treasureType(cwt.c->land)), col, 8);
  
        }
      }
    dialog::displayPageButtons(3, 0);
    mouseovers = "";
    keyhandler = [] (int sym, int uni) {
      if(plain) dialog::handleNavigation(sym, uni);
      
      if(orbmap.count(uni)) {
        eItem orb = orbmap[uni];
        if(remaining[orb] <= 0) ;
        else if(orb == itOrbMirror) {
          mirroring = !mirroring;
          // an amusing message
          if(remaining[itOrbMirror] >= 2 && !mirroring) 
            addMessage(XLAT("You mirror %the1.", orb));
          if(mirroring) {
            bool next = false;
            forCellEx(c2, cwt.c) if(c2->wall == waMirror || c2->wall == waCloud || c2->wall == waMirrorWall)
              next = true;
            if(!next) {
              addMessage(XLAT("You need to stand next to a magic mirror or cloud to use %the1.", itOrbMirror));
              mirroring = false;
              }
            }
          }
        else if(mirroring) {
          if(usedup[orb] >= TESTMIRRORED) {
            addMessage(XLAT("Each orb type can be mirrored only once."));
            mirroring = false;
            }
          else if(remaining[orb] > 0) {
            usedup[itOrbMirror]++;
            usedup[orb] += MIRRORED;
            usedup[orb] -= mirrorqty0(orb);
            addMessage(XLAT("You mirror %the1.", orb));
            mirroring = false;
            }
          else mirroring = false;
          }
        else if((isHaunted(cwt.c->land) || cwt.c->land == laDungeon) && orb == itOrbSafety) {
          addMessage(XLAT("This would only move you deeper into the trap!"));
          }
        else {
          eItem it = cwt.c->item; 
          cwt.c->item = orbmap[uni];
          collectItem(cwt.c, true);
          if(!cwt.c->item) usedup[orbmap[uni]]++;
          if(getOLR(it, getPrizeLand()))
            usedForbidden = true;
          cwt.c->item = it;
          checkmove();
          popScreenAll();
          }
        }
      
      else if(uni == '1') plain = !plain;
      else if(sym == SDLK_F1) 
        gotoHelp(which ? generateHelpForItem(which) : NODESCYET);
      else if(doexiton(sym, uni)) {
        if(mirroring) mirroring = false;
        popScreen();
        }
      };
    }

  void applyBox(eItem it) {
    applyBoxNum(usedup[it]);
    }
  
  int incheck;
  
  void check(int delta) {
    incheck += delta;
    for(int i=0; i<ittypes; i++) {
      eItem it = eItem(i);
      if(itemclass(it) == IC_ORB)
        items[it] += delta * remaining[it] * orbcharges(it);
      }
    }

  }

  