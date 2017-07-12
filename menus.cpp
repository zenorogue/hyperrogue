// HyperRogue menus
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

#include "dialogs.cpp"

// -- overview --

#define BLACKISH 0x404040
#define REDDISH 0x400000

ld whatever = 0;

int PREC(ld x) {
  ld sh = shiftmul;
  if(sh > -.2 && sh < .2) x = 10.01; 
  return int(shiftmul * x);
  }

void showOverview() {
  DEBB(DF_GRAPH, (debugfile,"show overview\n"));
  mouseovers = XLAT("world overview");
  mouseovers += "       ";
  mouseovers += XLAT(" kills: %1/%2", its(tkills()), its(killtypes()));
  mouseovers += XLAT(" $$$: %1", its(gold()));
  if(hellUnlocked()) {
    int i1, i2; countHyperstoneQuest(i1, i2);
    mouseovers += XLAT(" Hyperstone: %1/%2", its(i1), its(i2));
    }
  else
    mouseovers += XLAT(" Hell: %1/9", its(orbsUnlocked()));
  
  bool pages;
  
  int nl = LAND_OVER, nlm;
  eLand *landtab = land_over;
  if(randomPatternsMode) { nl = nlm = RANDLANDS; landtab = randlands; }
  else {
    pages = true;
    landtab += dialog::handlePage(nl, nlm, (nl+1)/2);
    }
  
  int vf = min((vid.yres-64-vid.fsize*2) / nlm, vid.xres/40);

  eLand curland = cwt.c->land;
  if(curland == laPalace && princess::dist(cwt.c) < OUT_OF_PRISON)
    curland = laPrincessQuest;
  if(isElemental(curland)) curland = laElementalWall;
  
  getcstat = '0';
  
  for(int i=0; i<nl; i++) {
    eLand l = landtab[i];
    int xr = vid.xres / 64;
    int i0 = 56 + vid.fsize + i * vf;
    int col;
    if(landUnlocked(l)) col = linf[l].color; else col = 0x404040;
    if(chaosmode && noChaos(l)) col = 0x400000;
    if(l == curland)
      displayfrZ(1, i0, 1, vf-4, "*", forecolor, 0);
    if(displayfrZ(xr*1, i0, 1, vf-4, XLAT1(linf[l].name), col, 0))
      getcstat = 1000 + l;
    eItem it = treasureType(l);
    int lv = items[it] * landMultiplier(l);
    if(lv >= 25) col = 0xFFD500;
    else if(lv >= 10) col = 0x00D500;
    else if(items[it]) col = 0xC0C0C0;
    else col = BLACKISH;
    if(chaosmode && noChaos(l)) col = REDDISH;
    int c8 = (vf+2)/3;
    if(displayfrZ(xr*24-c8*6, i0, 1, vf-4, its(items[it]), col, 16))
      getcstat = 2000+it;
    if(!cheater)
    if(displayfrZ(xr*24, i0, 1, vf-4, its(hiitems[modecode()][it]), col, 16))
      getcstat = 2000+it;
    if(items[it]) col = iinf[it].color; else col = BLACKISH;
    if(chaosmode && noChaos(l)) col = REDDISH;
    if(displayfrZ(xr*24+c8*4, i0, 1, vf-4, s0 + iinf[it].glyph, col, 16))
      getcstat = 2000+it;
    if(displayfrZ(xr*24+c8*5, i0, 1, vf-4, XLAT1(iinf[it].name), col, 0))
      getcstat = 2000+it;
    eItem io = orbType(l);
    if(io == itShard) {
      if(items[it] >= 10) col = winf[waMirror].color; else col = BLACKISH;
      if(chaosmode && noChaos(l)) col = REDDISH;
      if(displayfrZ(xr*46, i0, 1, vf-4, XLAT1(winf[waMirror].name), col, 0))
        getcstat = 3000+waMirror;
      if(getcstat == 3000+waMirror)
        mouseovers = XLAT(
          olrDescriptions[getOLR(io, cwt.c->land)], cwt.c->land, it, treasureType(cwt.c->land));
      }
    else if(io) {
      if(lv >= 25) col = 0xFFD500;
      else if(lv >= 10) col = 0xC0C0C0;
      else col = BLACKISH;
      if(chaosmode && noChaos(l)) col = REDDISH;
      if(displayfrZ(xr*46-c8*4, i0, 1, vf-4, its(items[io]), col, 16))
        getcstat = 2000+io;
      if(lv >= 10) col = iinf[io].color; else col = BLACKISH;
      if(chaosmode && noChaos(l)) col = REDDISH;
      if(displayfrZ(xr*46-c8, i0, 1, vf-4, s0 + iinf[io].glyph, col, 16))
        getcstat = 2000+io;
      if(displayfrZ(xr*46, i0, 1, vf-4, XLAT1(iinf[io].name), col, 0))
        getcstat = 2000+io;
      if(getcstat == 2000+io)
        mouseovers = XLAT(
          olrDescriptions[getOLR(io, curland)], curland, it, treasureType(curland));
      }
    }

  dialog::displayPageButtons(3, pages);
  
  keyhandler = [] (int sym, int uni) {
    int umod = uni % 1000;
    int udiv = uni / 1000;
    if(udiv == 1 && umod < landtypes) {
      if(cheater && !hiliteclick) {
        eLand l = eLand(umod);
        cheater++;
        bool princ = (l == laPrincessQuest);
        if(princ) {
          if(kills[moVizier] == 0) kills[moVizier] = 1;
          princess::forceMouse = true;
          princess::gotoPrincess = true;
          l = laPalace;
          }
        cheatMoveTo(l);
        popScreen();
        canmove = true;
        if(princ) fullcenter();
        }
      else gotoHelp(generateHelpForLand(eLand(umod)));
      }
    else if(udiv == 2 && umod < ittypes) {
      if(cheater && !hiliteclick) {
        cheater++;
        int ic = itemclass(eItem(umod));
        if(ic == IC_TREASURE) items[umod] += PREC(10);
        if(ic == IC_ORB) items[umod] += PREC(60);
        if(umod == itGreenStone) items[umod] += PREC(100);
        else if(ic == IC_OTHER) items[umod] += (shiftmul>0?1:-1);
        if(items[umod] < 0) items[umod] = 0;
        if(hardcore) canmove = true;
        else checkmove();
        }
      else gotoHelp(generateHelpForItem(eItem(umod)));
      }
    else if(udiv == 3 && umod < walltypes) gotoHelp(generateHelpForWall(eWall(umod)));
    else if(uni == SDLK_F1) gotoHelp(
      "This displays all lands available in the game. "
      "Bonus lands (available only in separate challenges) "
      "are not included. Lands written in dark have to be "
      "unlocked, and lands in dark red are unavailable "
      "because of using special options. Click on any "
      "land or item to get information about it. Hover over "
      "an Orb to know its relation to the current land. "
      "Cheaters can click to move between lands, and use the "
      "mousewheel to gain or lose treasures and orbs quickly (Ctrl = precise, Shift = reverse)."
      );
    else if(dialog::handlePageButtons(uni)) ;
    else if(doexiton(sym, uni)) popScreen();
    };
  }

// -- main menu --

bool checkHalloweenDate() {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  int month = tm.tm_mon + 1;
  int day = tm.tm_mday;
  if(month == 10 && day >= 24) return true;
  if(month == 11 && day <= 7) return true;
  return false;
  }

void showMainMenu() {
  gamescreen(2);

  getcstat = ' ';
  
  dialog::init(XLAT("HyperRogue %1", VER), 0xC00000, 200, 100);

  dialog::addItem(XLAT("basic configuration"), 'b');
  dialog::addItem(XLAT("graphics configuration"), 'g');
  dialog::addItem(XLAT("special display modes"), 'd');
  dialog::addItem(XLAT("special game modes"), 'm');

#ifndef NOSAVE
  dialog::addItem(XLAT("local highscores"), 't');
#endif
  dialog::addItem(XLAT("help"), 'h'); dialog::lastItem().keycaption += " / F1";
  if(cheater)
    dialog::addItem(XLAT("cheats"), 'c');
  else dialog::addBreak(100);
  dialog::addItem(XLAT("restart game"), 'r'); dialog::lastItem().keycaption += " / F5";
  
  string q;
  #ifdef MOBILE
  dialog::addItem(XLAT("visit the website"), 'q');
  #else
  q = quitsaves() ? "save" : "quit"; 
  q = q + " the game";
  dialog::addItem(XLAT(q), 'q'); dialog::lastItem().keycaption += " / F10";
  #endif

  if(canmove)                                     
    q = "review your quest";
  else
    q = "game over screen";
  dialog::addItem(XLAT(q), SDLK_ESCAPE);
  dialog::addItem(XLAT("world overview"), 'o');    

  if(inv::on)
    dialog::addItem(XLAT("inventory"), 'i');    

  if(checkHalloweenDate()) dialog::addItem(XLAT("Halloween mini-game"), 'y'-96);

#ifdef ROGUEVIZ
  dialog::addItem(XLAT("rogueviz menu"), 'u'); 
#endif
  
#ifdef MOBILE
#ifdef HAVE_ACHIEVEMENTS
  dialog::addItem(XLAT("leaderboards/achievements"), '3'); 
#endif
#endif

#ifdef ANDROIDSHARE
  dialog::addItem("SHARE", 's'-96);
#endif
  
  if(!canmove) q = "review the scene";
  else if(turncount > 0) q = "continue game";
  else q = "play the game!";
  
  dialog::addItem(XLAT(q), ' ');
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    if(sym == SDLK_F1 || sym == 'h') gotoHelp(help);
    else if(sym == 'c' && cheater) pushScreen(showCheatMenu);
    else if(sym == 'b') pushScreen(showBasicConfig);
    else if(sym == 'g') pushScreen(showGraphConfig);
    else if(sym == 'd') pushScreen(showDisplayMode);
    else if(sym == 'm') pushScreen(showChangeMode);
  #ifndef NOSAVE
    else if(sym == 't') scores::load();
  #endif
    else if(uni == 'y'-96) {
      if(!sphere) {
        euclidland = laHalloween;
        restartGame('E');
        vid.alpha = 999;
        vid.scale = 998;
        popScreen();
        }
      else {
        restartGame('E');
        vid.alpha = 1;
        vid.scale = 1;
        popScreen();
        }
      }
    else if(sym == 'r' || sym == SDLK_F5) {
      restartGame();
      popScreen();
      }
    else if(sym == 'q' || sym == SDLK_F10) 
      quitmainloop = true;
    else if(sym == 'o') {
      clearMessages();
      pushScreen(showOverview);
      }
#ifdef INV
    else if(sym == 'i') {
      clearMessages();
      pushScreen(inv::show);
      }
#endif
    else if(sym == SDLK_ESCAPE) 
      showMissionScreen();
  #ifdef MOBILE
  #ifdef HAVE_ACHIEVEMENTS
    else if(sym == '3') {
      achievement_final(false);
      pushScreen(leader::showMenu);
      }
  #endif
  #endif
  #ifdef ROGUEVIZ
    else if(uni == 'u') pushScreen(rogueviz::showMenu);
  #endif
    else if(doexiton(sym, uni)) {
      popScreenAll();
      msgs.clear();
      }
    };
  }

// -- display modes --

void showDisplayMode() {
  gamescreen(3);

  dialog::init(XLAT("special display modes"));

  const char *wdmodes[6] = {"ASCII", "black", "plain", "Escher", "plain/3D", "Escher/3D"};
  const char *mdmodes[6] = {"ASCII", "items only", "items and monsters", "high contrast",
    "3D", "high contrast/3D"};

  dialog::addBoolItem(XLAT("orthogonal projection"), vid.alpha >= 500, '1');
  dialog::addBoolItem(XLAT("small Poincaré model"), vid.alpha == 1 && vid.scale < 1, '2');
  dialog::addBoolItem(XLAT("big Poincaré model"), vid.alpha == 1 && vid.scale >= 1, '3');
  dialog::addBoolItem(XLAT("Klein-Beltrami model"), vid.alpha == 0, '4');
  dialog::addSelItem(XLAT("wall display mode"), XLAT(wdmodes[vid.wallmode]), '5');
  if(getcstat == '5')
    mouseovers = XLAT("also hold Alt during the game to toggle high contrast");
  dialog::addBoolItem(XLAT("draw the grid"), (vid.grid), '6');
  dialog::addBoolItem(XLAT("mark heptagons"), (vid.darkhepta), '7');
  dialog::addSelItem(XLAT("3D configuration"), "", '9');

  dialog::addSelItem(XLAT("scale factor"), fts(vid.scale), 'z');
  dialog::addSelItem(XLAT("monster display mode"), XLAT(mdmodes[vid.monmode]), 'm');

  dialog::addBreak(50);

#ifndef NOEDIT
  dialog::addBoolItem(XLAT("vector graphics editor"), (false), 'g');
#endif

  // display modes  
#ifndef NORUG
  dialog::addBoolItem(XLAT("hypersian rug mode"), (rug::rugged), 'u');
#endif
#ifndef NOMODEL
  dialog::addBoolItem(XLAT("paper model creator"), (false), 'n');
#endif
  dialog::addBoolItem(XLAT("conformal/history mode"), (conformal::on), 'a');
  dialog::addBoolItem(XLAT("expansion"), viewdists, 'x');
  
  showAllConfig();
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    char xuni = uni;
    if((xuni >= 'A' && xuni <= 'Z') || (xuni >= 1 && xuni <= 26)) xuni |= 32;
  
    if(xuni == 'p') projectionDialog();
    
    if(xuni == 'z') {
      dialog::editNumber(vid.scale, .001, 1000, .1, 1, XLAT("scale factor"), 
        XLAT("Scale the displayed model."));
      dialog::scaleLog();
      dialog::sidedialog = true;
      }
    
    if(xuni == 'm') { vid.monmode += 60 + (shiftmul > 0 ? 1 : -1); vid.monmode %= 6; }
  
    if(xuni == '9') pushScreen(show3D);
    
  #ifndef NOEDIT
    else if(xuni == 'g') {
      pushScreen(mapeditor::showDrawEditor);
      mapeditor::initdraw(cwt.c);
      }
  #endif
  
    else if(xuni == 'x') {
      viewdists = !viewdists;
      popScreen();
      }
  #ifndef NORUG
    else if(xuni == 'u') {
      if(sphere) projectionDialog();
      else rug::select();
      }
  #endif
    else if(uni == 'a')
      pushScreen(conformal::show);
  
  #ifndef NOMODEL
    else if(xuni == 'n')
      pushScreen(netgen::show);
  #endif
  
    else gmodekeys(sym, uni);
  
    handleAllConfig(sym, xuni);
    };
  }

// -- game modes -- 

void switchHardcore() {
  if(hardcore && !canmove) { 
    restartGame();
    hardcore = false;
    popScreen();
    }
  else if(hardcore && canmove) { hardcore = false; }
  else { hardcore = true; canmove = true; hardcoreAt = turncount; }
  if(hardcore)
      addMessage("One wrong move, and it is game over!");
  else
      addMessage("Not so hardcore?");
  if(pureHardcore()) popScreen();
  }

void showChangeMode() {
  gamescreen(3);
  dialog::init(XLAT("special game modes"));
  
  // gameplay modes

#ifdef TOUR
  dialog::addBoolItem(XLAT("Tutorial"), tour::on, 'T');
#endif

  dialog::addBoolItem(XLAT("Euclidean/elliptic mode"), (euclid || sphere), 'e');
  dialog::addBoolItem(XLAT(SHMUPTITLE), (shmup::on || multi::players > 1), 's');
  if(!shmup::on) dialog::addSelItem(XLAT("hardcore mode"),
    hardcore && !pureHardcore() ? XLAT("PARTIAL") : ONOFF(hardcore), 'h');
  if(getcstat == 'h')
    mouseovers = XLAT("One wrong move and it is game over!");
  
  multi::cpid = 0;
  dialog::addBoolItem(XLAT("heptagonal mode"), (purehepta), '7');
  dialog::addBoolItem(XLAT("Chaos mode"), (chaosmode), 'C');
  dialog::addBoolItem(XLAT("peaceful mode"), (chaosmode), 'p');
  dialog::addBoolItem(XLAT("inventory mode"), (inv::on), 'i');
  dialog::addBoolItem(XLAT("pure tactics mode"), (tactic::on), 't');
  dialog::addBoolItem(XLAT("Yendor Challenge"), (yendor::on), 'y');
  dialog::addBoolItem(XLAT("%1 Challenge", moPrincess), (princess::challenge), 'P');
  dialog::addBoolItem(XLAT("random pattern mode"), (randomPatternsMode), 'r');

  dialog::addBreak(50);
  // cheating and map editor

  dialog::addBoolItem(XLAT("cheat mode"), (cheater), 'c');
#ifndef NOEDIT
  dialog::addBoolItem(XLAT("map editor"), (false), 'm');
#endif

  dialog::addBreak(50);
  
  dialog::addItem(XLAT("return to the game"), 'v');
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);    
    char xuni = uni;
    
    if(xuni == 'v' || sym == SDLK_ESCAPE) popScreen();
    
    else if(uni == 'c') {
      if(tactic::on && gold()) {
        addMessage(XLAT("Not available in the pure tactics mode!"));
        }
      else if(!cheater) {
        cheater++;
        addMessage(XLAT("You activate your demonic powers!"));
  #ifndef MOBILE
        addMessage(XLAT("Shift+F, Shift+O, Shift+T, Shift+L, Shift+U, etc."));
  #endif
        popScreen();
        }
      else {
        popScreen();
        firstland = princess::challenge ? laPalace : laIce;
        restartGame();
        }
      }
    
    else if(xuni == 'e') 
      pushScreen(showEuclideanMenu);
    else if(xuni == 't') {
      clearMessages();
      pushScreen(tactic::showMenu);
    }
    else if(xuni == 'y') {
      clearMessages();
      if(yendor::everwon || autocheat)
        pushScreen(yendor::showMenu);
      else gotoHelp(yendor::chelp);
      }
    else if(xuni == '7')
      restartGame('7');
    else if(xuni == 'p')
      pushScreen(peace::showMenu);
    else if(xuni == 'i') {
      restartGame('i');
      }
  #ifdef TOUR
    else if(uni == 'T') {
      tour::start();
      }
  #endif
    else if(uni == 'C') {
      if(chaosUnlocked) restartGame('C');
      if(chaosmode) gotoHelp(
        "In the Chaos mode, lands change very often, and "
        "there are no walls between them. "
        "Some lands are incompatible with this."
        "\n\nYou need to reach Crossroads IV to unlock the Chaos mode."
        );
      }
    else if(xuni == 'p') {
      if(!princess::everSaved)
        addMessage(XLAT("Save %the1 first to unlock this challenge!", moPrincess));
      else
        restartGame('P');
      }
  #ifndef NOEDIT
    else if(xuni == 'm') {
      if(tactic::on) 
        addMessage(XLAT("Not available in the pure tactics mode!"));
      else {
        cheater++;
        pushScreen(mapeditor::showMapEditor);
        lastexplore = turncount;
        addMessage(XLAT("You activate your terraforming powers!"));
        }
      }
  #endif
    else if(xuni == 's') {
  #ifdef MOBILE
      restartGame('s');
  #else
      multi::shmupcfg = shmup::on;
      pushScreen(shmup::showShmupConfig);
  #endif
      }
    else if(xuni == 'h' && !shmup::on) 
      switchHardcore();
    else if(xuni == 'r') {
      firstland = laIce;
      restartGame('r');
      }
    else if(doexiton(sym, uni))
      popScreen();
    };
  }

// -- cheat menu --

void showCheatMenu() {
  gamescreen(1);
  dialog::init("cheat menu");
  dialog::addItem(XLAT("return to the game"), ' ');
  dialog::addBreak(50);
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
  dialog::addItem(XLAT("summon a Golem"), 'G');
  dialog::addItem(XLAT("summon Sandworm"), 'W');
  dialog::addItem(XLAT("summon Ivy"), 'I');
  dialog::addItem(XLAT("summon a Monster"), 'E');
  dialog::addItem(XLAT("summon Thumpers"), 'H');
  dialog::addItem(XLAT("summon Bonfire"), 'B');
  dialog::addItem(XLAT("summon Mimics"), 'M');
  dialog::addItem(XLAT("deplete orb powers"), 'P');
  dialog::addItem(XLAT("summon Orb of Yendor"), 'Y'-64);
  dialog::addItem(XLAT("switch ghost timer"), 'G'-64);
  dialog::addItem(XLAT("switch web display"), 'W'-64);
  dialog::display();
  keyhandler = []   (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    if(uni != 0) {
      applyCheat(uni);
      if(uni == 'F' || uni == 'C' || uni == 'O' ||
        uni == 'S' || uni == 'U' || uni == 'G' ||
        uni == 'W' || uni == 'I' || uni == 'E' ||
        uni == 'H' || uni == 'B' || uni == 'M' ||
        uni == 'P' || uni == 'Y'-64 || uni == 'G'-64 ||
        uni == ' ' || uni == 8 || uni == 13 ||
        uni == SDLK_ESCAPE || uni == 'q' || uni == 'v' || sym == SDLK_ESCAPE ||
        sym == SDLK_F10) 
        popScreen();
      }
    };
  }

// -- geometry menu --

int eupage = 0;
int euperpage = 21;

#define LAND_SPHEUC ((targetgeometry > 1) ? LAND_SPH : LAND_EUC)
#define land_spheuc ((targetgeometry > 1) ? land_sph : land_euc)

const char* geometrynames[gGUARD] = {
  "hyperbolic", "Euclidean", "spherical", "elliptic", 
  "Zebra quotient", "field quotient", "torus"
  };  

void showEuclideanMenu() {
  gamescreen(4);
  int s = vid.fsize;
  vid.fsize = vid.fsize * 4/5;
  dialog::init(XLAT("Euclidean/elliptic mode"));
  if(cheater) for(int i=0; i<landtypes; i++) landvisited[i] = true;
  for(int i=0; i<landtypes; i++)
    if(hiitemsMax(treasureType(eLand(i))) >= 25) landvisited[i] = true;
  landvisited[laCrossroads] = true;
  landvisited[laIce] = true;
  landvisited[laMirror] = true;
  landvisited[laPrincessQuest] = cheater || princess::everSaved;
  landvisited[laWildWest] = true;
  landvisited[laHalloween] = true;
  landvisited[laCA] = true;
  // for(int i=2; i<lt; i++) landvisited[i] = true;

  dialog::addSelItem(XLAT("geometry"), XLAT(geometrynames[targetgeometry]), '5');
  dialog::addBreak(50);

  for(int i=0; i<euperpage; i++) {
    if(euperpage * eupage + i >= LAND_SPHEUC) { dialog::addBreak(100); break; }
    eLand l = land_spheuc[euperpage * eupage + i];
    if(landvisited[l]) {
      char ch;
      if(i < 26) ch = 'a' + i;
      else ch = 'A' + (i-26);
      dialog::addItem(XLAT1(linf[l].name), ch);
      }
    else dialog::addBreak(100);
    }
  dialog::addBreak(50);
  dialog::addItem(XLAT("Return to the hyperbolic world"), '0');
  dialog::addItem(XLAT("next page"), '-');

  dialog::addBreak(100);
  dialog::addHelp(
    XLAT("Choose from the lands visited this game.") + " " +
    XLAT("Scores and achievements are not") + " " + XLAT("saved in the Euclidean mode!")
    );
  
  dialog::display();

  vid.fsize = s;
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    int lid;
    if(uni >= 'a' && uni <= 'z') lid = uni - 'a';
    else if(uni >= 'A' && uni <= 'Z') lid = 26 + uni - 'A';
    else lid = -1;
    
    if(lid >= 0) lid += euperpage * eupage;
    
    if(uni == '0') {
      targetgeometry = geometry;
      restartGame('g');
      popScreen();
      }
    else if(uni == '5') {
      targetgeometry = eGeometry(1+targetgeometry);
      if(targetgeometry == gGUARD) targetgeometry = gEuclid;
      }
    else if(uni == '-' || uni == PSEUDOKEY_WHEELUP || uni == PSEUDOKEY_WHEELDOWN) {
      eupage++;
      if(eupage * euperpage >= LAND_SPHEUC) eupage = 0;
      }
    else if(lid >= 0 && lid < LAND_SPHEUC) {
      euclidland = land_spheuc[lid];
      if(landvisited[euclidland] && euclidland != laOceanWall) {
        if(targetgeometry != geometry)
          restartGame('g');
        else
          restartGame(tactic::on ? 't' : 0);
        // disable PTM if chosen a land from the Euclidean menu
        if(tactic::on) restartGame('t');
        popScreen();
        }
      else euclidland = laIce;
      }
    else if(uni == '2' || sym == SDLK_F1) gotoHelp(
      "If you want to know how much the gameplay is affected by the "
      "hyperbolic geometry in HyperRogue, this mode is for you!\n\n"
      
      "You can play an Euclidean version of each of the lands in "
      "HyperRogue. Lands which include horocycles (Temple, Caribbean, "
      "Whirlpool), infinite trees (Zebra, Emerald), or networks of "
      "ultraparallel lines (Crossroads, Vineyard, Palace) cannot be "
      "faithfully represented in Euclidean, so yo get more "
      "or less simplified versions of them. Choose Crossroads to play a game "
      "where many different lands appear."
      );
    else if(doexiton(sym, uni)) popScreen();
    };
  }

// -- demo --

#ifdef DEMO
bool demoanim;

void toggleanim(bool v) {
  demoanim = v;
  if(v) {
    sightrange = 4;
    vid.wallmode = 5;
    vid.particles = true;
    vid.sspeed = -1;
    vid.mspeed = -1;
    geom3::highdetail = geom3::middetail = 5;
    }
  else {
    sightrange = 7;
    vid.sspeed = 5;
    vid.mspeed = 5;
    vid.particles = false;
    vid.wallmode = 3;
    geom3::highdetail = geom3::middetail = -1;
    }
  }

void showDemo() {
  gamescreen(2);

  getcstat = ' ';
  
  dialog::init(XLAT("HyperRogue %1: online demo", VER), 0xC00000, 200, 100);
  dialog::addBreak(50);

  dialog::addItem(XLAT("play"), 'f');
  // dialog::addItem(XLAT("tutorial"), 'T');
  dialog::addItem(XLAT("help"), 'h'); dialog::lastItem().keycaption += " / F1";
  dialog::addItem(XLAT("toggle high detail"), 'a');
  dialog::addBreak(100);

  dialog::addTitle("highlights", 0xC00000, 120);
  dialog::addItem(XLAT("Temple of Cthulhu"), 't');
  dialog::addItem(XLAT("Land of Storms"), 'l');
  dialog::addItem(XLAT("Burial Grounds"), 'b');

  dialog::display();
  }

void handleDemoKey(int sym, int uni) {
  dialog::handleNavigation(sym, uni);
  if(sym == SDLK_F1 || sym == 'h') gotoHelp(help);
  else if(sym == 'a') {
    toggleanim(!demoanim);
    popScreen();
    }
  else if(sym == 'f') {
    firstland = laIce;
    if(tactic::on) restartGame('t');
    else restartGame();
    popScreen();
    }
  else if(sym == 'T') {
    firstland = laIce;
    if(!tour::on) tour::start();
    popScreen();
    }
  else if(sym == 't') {
    firstland = laTemple;
    if(!tactic::on) restartGame('t');
    else restartGame();
    popScreen();
    }
  else if(sym == 'l') {
    firstland = laStorms;
    if(!tactic::on) restartGame('t');
    else restartGame();
    popScreen();
    }
  else if(sym == 'b') {
    firstland = laBurial;
    if(!tactic::on) restartGame('t');
    else restartGame();
    items[itOrbSword] = 60;
    popScreen();
    }
  }
#endif
 
// -- overview --

void setAppropriateOverview() {
  clearMessages();
  if(tactic::on)
    pushScreen(tactic::showMenu);
  else if(yendor::on)
    pushScreen(yendor::showMenu);
  else if(peace::on)
    pushScreen(peace::showMenu);
  else if(geometry != gNormal)
    pushScreen(showEuclideanMenu);
  else 
    pushScreen(showOverview);
  }

