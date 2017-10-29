static int orbid = 0;

void debugScreen();

eItem nextOrb() {
  orbid++;
  eItem i = eItem(orbid % ittypes);
  if(itemclass(i) == IC_ORB) return i;
  else return nextOrb();
  }

eItem randomTreasure() {
  eItem i = eItem(hrand(ittypes));
  if(itemclass(i) == IC_TREASURE) return i;
  else return randomTreasure();
  }

eItem randomTreasure2(int cv) {
  int bq = 60000, cq = 0;
  eItem best = itDiamond;
  eItem lt = localTreasureType();
  for(int a=1; a<ittypes; a++) {
    eItem i = eItem(a);
    if(itemclass(i) != IC_TREASURE) continue;
    int q = 2*items[i];
    if(a == lt) q -= (2*cv-1);
    if(a == itEmerald && bearsCamelot(cwt.c->land)) q -= 8;
    if(a == itElixir && isCrossroads(cwt.c->land)) q -= 7;
    if(a == itIvory && isCrossroads(cwt.c->land)) q -= 6;
    if(a == itPalace && isCrossroads(cwt.c->land)) q -= 5;
    if(a == itIvory && cwt.c->land == laJungle) q -= 5;
    if(a == itIvory && cwt.c->land == laPalace) q -= 5;
    if(q < bq) bq = q, cq = 0;
    if(q == bq) { cq++; if(hrand(cq) == 0) best = i; }
    }
  return best;
  }

bool isTechnicalLand(eLand l) {
  return l == laNone || l == laOceanWall || l == laBarrier || l == laCanvas ||
    l == laHauntedWall || l == laHauntedBorder || l == laCA ||
    l == laMirrorWall || l == laMirrored || l == laMirrored2;
  }

eLand cheatdest;

void cheatMoveTo(eLand l) {
  cheatdest = l;
  if(l == laCrossroads5) l = laCrossroads;
  activateSafety(l);
  cheatdest = laNone;
  }

bool applyCheat(char u, cell *c = NULL) {

  if(u == 'L') {
    do {
      if(firstland == eLand(landtypes-1))
        firstland = eLand(2);
      else
        firstland = eLand(firstland+1);
      }
    while(isTechnicalLand(firstland) || isCyclic(firstland));
    specialland = firstland;
    cheater++; addMessage(XLAT("You will now start your games in %1", firstland));
    return true;
    }
  if(u == 'C') {
    cheater++; 
    cheatMoveTo(laCrossroads);
    addMessage(XLAT("Activated the Hyperstone Quest!"));

    for(int t=1; t<ittypes; t++) 
      if(t != itHyperstone && t != itBounty && itemclass(eItem(t)) == IC_TREASURE) {
        items[t] = inv::on ? 50 : 10;
        }
    int qkills = inv::on ? 1000 : 200;
    kills[moYeti] = qkills;
    kills[moDesertman] = qkills;
    kills[moRunDog] = qkills;
    kills[moZombie] = qkills;
    kills[moMonkey] = qkills;
    kills[moCultist] = qkills;
    kills[moTroll] = qkills;
    return true;
    }
  if(u == 'M') {
    for(int i=0; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_ORB) 
        items[i] = 0;
    cheater++; addMessage(XLAT("Orb power depleted!"));
    return true;
    }
  if(u == 'O') {
    cheater++; addMessage(XLAT("Orbs summoned!"));
    for(int i=0; i<cwt.c->type; i++) 
      if(passable(cwt.c->mov[i], NULL, 0)) {
        eItem it = nextOrb();
        cwt.c->mov[i]->item = it;
        }
    return true;
    }
  if(u == 'F') {
    if(hardcore && !canmove) { 
      canmove = true; 
      addMessage(XLAT("Revived!"));
      }
    else {
      items[itOrbFlash] += 1;
      items[itOrbTeleport] += 1;
      items[itOrbLightning] += 1;
      items[itOrbSpeed] += 1;
      items[itOrbShield] += 1;
      kills[moPlayer] = 0;
      cheater++; addMessage(XLAT("Orb power gained!"));
      canmove = true;
      }
    return true;
    }
  if(u == 'D') {
    items[itGreenStone] += 10;
    cheater++; addMessage(XLAT("Dead orbs gained!"));
    return true;
    }
  if(u == 'R'-64) buildRosemap();
#if CAP_EDIT
  if(u == 'A') {
    lastexplore = turncount;
    pushScreen(mapeditor::showMapEditor);
    return true;
    }
  if(u == 'A'-64) {
    mapeditor::drawcell = mouseover ? mouseover : cwt.c;
    pushScreen(mapeditor::showDrawEditor);
    return true;
    }
#endif
  if(u == 'T') {
    items[randomTreasure2(10)] += 10;
    cheater++; addMessage(XLAT("Treasure gained!"));
    return true;
    }
  if(u == 'T'-64) {
    items[randomTreasure2(100)] += 100;
    cheater++; addMessage(XLAT("Lots of treasure gained!"));
    return true;
    }
  if(u == 'I'-64) {
    items[randomTreasure2(10)] += 25;
    cheater++; addMessage(XLAT("Treasure gained!"));
    return true;
    }
  if(u == 'U'-64) {
    items[randomTreasure2(10)] += 50;
    cheater++; addMessage(XLAT("Treasure gained!"));
    return true;
    }
  if(u == 'Z') {
    flipplayer = false;
    mirror::act(1, mirror::SPINSINGLE);
    cwspin(cwt, 1);
    return true;
    }
  if(u == 'J') {
    if(items[localTreasureType()] > 0)
      items[localTreasureType()] = 0;
    else for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_TREASURE) 
        items[i] = 0;
    cheater++; addMessage(XLAT("Treasure lost!"));
    return true;
    }
  if(u == 'K') {
    for(int i=0; i<motypes; i++) kills[i] += 10;
    kills[moPlayer] = 0;
    cheater++; addMessage(XLAT("Kills gained!"));
    return true;
    }
  if(u == 'Y') {
    for(auto& y: yendor::yi) {
      if(y.path[YDIST-1]->item == itKey)
        y.path[YDIST-1]->item = itNone;
      if(!y.found) items[itKey]++;
      y.found = true;
      }
    cheater++; addMessage(XLAT("Collected the keys!"));
    }
  if(u == 'P') {
    items[itSavedPrincess]++;
    princess::saved = true;
    princess::everSaved = true;
    if(inv::on && !princess::reviveAt)
      princess::reviveAt = gold(NO_LOVE);
    cheater++; addMessage(XLAT("Saved the Princess!"));
    }
  if(u == 'S') {
    canmove = true;
    cheatMoveTo(cwt.c->land);
    items[itOrbSafety] += 3;
    cheater++; addMessage(XLAT("Activated Orb of Safety!"));
    return true;
    }
  if(u == 'U') {
    canmove = true;
    cheatMoveTo(firstland);
    cheater++; addMessage(XLAT("Teleported to %1!", firstland));
    return true;
    }
  if(u == 'W'-64) {
    pushScreen(linepatterns::showMenu);
    return true;
    }
  if(u == 'G'-64) {
    timerghost = !timerghost;
    cheater++; 
    addMessage(XLAT("turn count = %1 last exploration = %2 ghost timer = %3",
      its(turncount), its(lastexplore), ONOFF(timerghost)));
    return true;
    }
  if(u == 'L'-64) {
    cell *c = mouseover;
    describeCell(c);
    printf("Neighbors:"); for(int i=0; i<c->type; i++) printf("%p ",  c->mov[i]);
    printf("Barrier: dir=%d left=%d right=%d\n",
      c->bardir, c->barleft, c->barright);
    return true;
    }
  if(u == 'C'-64) {
    cblind = !cblind;
    return true;
    }
  if(u == 'G') {
    pushScreen(debugScreen);
    return true;
    }
  if(u == 'P'-64) 
    peace::on = !peace::on;
#ifdef CHEAT_DISABLE_ALLOWED
  if(u == 'D'-64) {
    cheater = 0; autocheat = 0;
    return true;
    }
#endif
  return false;
  }

template<class T> string dnameof2(T x) {
  string s = dnameof(x);
  return s + " (" + its(x) + ")";
  }

template<class T> string dnameof2(T x, int p) {
  string s = dnameof(x);
  return s + " (" + its(x) + "/" + its(p) + ")";
  }

vector<pair<cellwalker,int> > drawbugs;

bool debugmode = false;

void debugScreen() {
  cmode = vid.xres > vid.yres * 1.4 ? sm::SIDE : 0;
  gamescreen(0);
  dialog::init(XLAT("debug"));
  
  for(auto& p: drawbugs)
    drawBug(p.first, p.second);
  
  if(mouseover) {
    drawBug(cellwalker(mouseover, 0), 0x80808080);
    char buf[200];
    sprintf(buf, "%p", mouseover);
    dialog::addSelItem("pointer", buf, 0);
    dialog::addSelItem("mpdist", its(mouseover->mpdist), 0);
    dialog::addSelItem("land", dnameof2(mouseover->land), 0);
    dialog::addSelItem("land param (int)", its(mouseover->landparam), 0);
    dialog::addSelItem("land param (hex)", itsh8(mouseover->landparam), 0);
    dialog::addSelItem("land param (heat)", fts(HEAT(mouseover)), 0);
    dialog::addSelItem("land flags", its(mouseover->landflags)+"/"+itsh2(mouseover->landflags), 0);
    dialog::addSelItem("barrier dir", its(mouseover->bardir), 0);
    dialog::addSelItem("barrier left", dnameof2(mouseover->barleft), 0);
    dialog::addSelItem("barrier right", dnameof2(mouseover->barright), 0);
    dialog::addBreak(50);
    dialog::addSelItem("wall", dnameof2(mouseover->wall, mouseover->wparam), 0);
    dialog::addSelItem("item", dnameof(mouseover->item), 0);
    dialog::addBreak(50);
    dialog::addSelItem("cpdist", its(mouseover->cpdist), 0);
    dialog::addSelItem("pathdist", its(mouseover->pathdist), 0);
    dialog::addSelItem("temporary", its(mouseover->aitmp), 0);
    dialog::addBreak(50);
    dialog::addSelItem("monster", dnameof2(mouseover->monst, mouseover->mondir), 0);
    dialog::addSelItem("stuntime/hitpoints", its(mouseover->stuntime)+"/"+its(mouseover->hitpoints), 0);
    }
  dialog::display();

  keyhandler = [] (int sym, int uni) {
    handlePanning(sym, uni);
    dialog::handleNavigation(sym, uni);
    if(applyCheat(uni, mouseover)) ;
    else if(sym == PSEUDOKEY_WHEELUP || sym == PSEUDOKEY_WHEELDOWN) ;
    else if(doexiton(sym, uni)) {
      popScreen();
      if(debugmode) quitmainloop = true;
      }
    };  
  }

// -- cheat menu --

void showCheatMenu() {
  gamescreen(1);
  dialog::init("cheat menu");
  dialog::addItem(XLAT("gain orb powers"), 'F');
  dialog::addItem(XLAT("summon treasure"), 'T');
  dialog::addItem(XLAT("summon dead orbs"), 'D');
  dialog::addItem(XLAT("lose all treasure"), 'J');
  dialog::addItem(XLAT("gain kills"), 'K');
  dialog::addItem(XLAT("Hyperstone Quest"), 'C');
  dialog::addItem(XLAT("summon orbs"), 'O');
  dialog::addItem(XLAT("gain Orb of Yendor"), 'Y');
  dialog::addItem(XLAT("summon lots of treasure"), 'T'-64);
  dialog::addItem(XLAT("Safety (quick save)"), 'S');
  dialog::addItem(XLAT("Select the land ---"), 'L');
  dialog::addItem(XLAT("--- and teleport there"), 'U');
  dialog::addItem(XLAT("rotate the character"), 'Z');
  dialog::addItem(XLAT("deplete orb powers"), 'M');
  dialog::addItem(XLAT("save a Princess"), 'P');
  dialog::addItem(XLAT("unlock Orbs of Yendor"), 'Y');
  dialog::addItem(XLAT("switch ghost timer"), 'G'-64);
  dialog::addItem(XLAT("switch web display"), 'W'-64);
  dialog::addItem(XLAT("peaceful mode"), 'P'-64);
  dialog::addBreak(50);
  dialog::addItem(XLAT("go back"), ' ');
  dialog::display();
  keyhandler = []   (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    if(uni != 0) {
      applyCheat(uni);
      if(uni == 'F' || uni == 'C' || uni == 'O' ||
        uni == 'S' || uni == 'U' || uni == 'G' ||
        uni == 'W' || uni == 'I' || uni == 'E' ||
        uni == 'H' || uni == 'B' || uni == 'M' ||
        uni == 'M' || uni == 'Y'-64 || uni == 'G'-64 ||
        uni == ' ' || uni == 8 || uni == 13 ||
        uni == SDLK_ESCAPE || uni == 'q' || uni == 'v' || sym == SDLK_ESCAPE ||
        sym == SDLK_F10) 
        popScreen();
      }
    };
  }


#ifdef BACKTRACE
#include <execinfo.h>
#endif

void modalDebug(cell *c) {
  viewctr.h = c->master;
  pushScreen(debugScreen);
  debugmode = true;
  mainloop();
  debugmode = false;
  quitmainloop = false;
  }

void raiseBuggyGeneration(cell *c, const char *s) {

  printf("procgen error (%p): %s\n", c, s);
  
  if(!errorReported) {
    addMessage(string("something strange happened in: ") + s);
    errorReported = true;
    }

#ifdef BACKTRACE
  void *array[1000];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 1000);

  // print out all the frames to stderr
  backtrace_symbols_fd(array, size, STDERR_FILENO);
#endif

  // return;
  
  if(cheater) {
    drawbugs.emplace_back(cellwalker(c,0), 0xFF000080);
    modalDebug(c);
    drawbugs.pop_back();
    }
  else
    c->item = itBuggy;
  }
