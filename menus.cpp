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
  cmode = sm::ZOOMABLE | sm::OVERVIEW;  
  DEBB(DF_GRAPH, (debugfile,"show overview\n"));

  if(mapeditor::infix != "")
    mouseovers = mapeditor::infix;
  else {
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
    }
  
  bool pages;
  
  int nl = LAND_OVER, nlm;
  eLand *landtab = land_over;
  if(randomPatternsMode) { nl = RANDLANDS; landtab = randlands; }
  
  if(mapeditor::infix != "") {
    static eLand filteredLands[landtypes];
    int nlid = 0;
    for(int i=0; i<nl; i++) {
      eLand l = landtab[i];
      string s = dnameof(l);
      s += "@";
      s += dnameof(treasureType(l));
      s += "@";
      s += dnameof(nativeOrbType(l));
      if(mapeditor::hasInfix(s))
        filteredLands[nlid++] = l;
      }
    if(nlid) {
      nl = nlid; landtab = filteredLands;
      }
    }
  
  if(nl > 30) {
    pages = true;
    landtab += dialog::handlePage(nl, nlm, (nl+1)/2);
    }
  else nlm = nl;
  
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
    eItem io = nativeOrbType(l);
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
      const eLand l = eLand(umod);
      gotoHelp(""); gotoHelpFor(l);
      if(cheater) {
        help_extension hex;
        hex.key = 't';
        hex.text = XLAT("teleport");
        hex.action = [l] () {
          cheater++;
          bool princ = (l == laPrincessQuest);
          if(princ) {
            if(kills[moVizier] == 0) kills[moVizier] = 1;
            princess::forceMouse = true;
            princess::gotoPrincess = true;
            cheatMoveTo(laPalace);
            }
          else cheatMoveTo(l);
          canmove = true;
          if(princ) fullcenter();
          popScreen();
          popScreen();
          };
        help_extensions.push_back(hex);
        }
      }
    else if(udiv == 2 && umod < ittypes) {
      gotoHelp(generateHelpForItem(eItem(umod)));
      if(cheater) {
        dialog::helpToEdit(items[umod], 0, 200, 10, 10);
        dialog::reaction = [] () {
          if(hardcore) canmove = true;
          else checkmove();
          cheater++;
          };
        }
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
    else if(mapeditor::editInfix(uni)) ;
    else if(doexiton(sym, uni)) popScreen();
    };
  }

// -- main menu --

void showMainMenu() {
  gamescreen(2);

  getcstat = ' ';
  
  dialog::init(XLAT("HyperRogue %1", VER), 0xC00000, 200, 100);

  dialog::addItem(XLAT("basic configuration"), 'b');
  dialog::addItem(XLAT("graphics configuration"), 'g');
  dialog::addItem(XLAT("special display modes"), 'd');
  dialog::addItem(XLAT("special game modes"), 'm');

#if CAP_SAVE
  dialog::addItem(XLAT("local highscores"), 't');
#endif
  dialog::addItem(XLAT("help"), 'h'); dialog::lastItem().keycaption += " / F1";
  if(cheater)
    dialog::addItem(XLAT("cheats"), 'c');
  else dialog::addBreak(100);
  dialog::addItem(XLAT("restart game"), 'r'); dialog::lastItem().keycaption += " / F5";
  
  string q;
  #if ISMOBILE==1
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

#if CAP_ROGUEVIZ
  dialog::addItem(XLAT("rogueviz menu"), 'u'); 
#endif
  
#if ISMOBILE==1
#if CAP_ACHIEVE
  dialog::addItem(XLAT("leaderboards/achievements"), '3'); 
#endif
#endif

#if CAP_ANDROIDSHARE
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
  #if CAP_SAVE
    else if(sym == 't') scores::load();
  #endif
    else if(sym == 'r' || sym == SDLK_F5) {
      restartGame();
      }
    else if(sym == 'q' || sym == SDLK_F10) {
#if ISMOBILE
      extern void openURL();
      openURL();
#else
      quitmainloop = true;
#endif
      }
    else if(sym == 'o') {
      clearMessages();
      setAppropriateOverview();
      }
#if CAP_INV
    else if(sym == 'i') {
      clearMessages();
      pushScreen(inv::show);
      }
#endif
    else if(sym == SDLK_ESCAPE) 
      showMissionScreen();
  #if ISMOBILE==1
  #ifdef HAVE_ACHIEVEMENTS
    else if(sym == '3') {
      achievement_final(false);
      pushScreen(leader::showMenu);
      }
  #endif
  #endif
  #if CAP_ROGUEVIZ
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
  cmode = sm::SIDE;
  gamescreen(0);

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

#if CAP_EDIT
  dialog::addBoolItem(XLAT("vector graphics editor"), (false), 'g');
#endif

  // display modes  
#if CAP_RUG
  dialog::addBoolItem(XLAT("hypersian rug mode"), (rug::rugged), 'u');
#endif
#if CAP_MODEL
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
      }
    
    if(xuni == 'm') { vid.monmode += 60 + (shiftmul > 0 ? 1 : -1); vid.monmode %= 6; }
  
    if(xuni == '9') pushScreen(show3D);
    
  #if CAP_EDIT
    else if(xuni == 'g') {
      pushScreen(mapeditor::showDrawEditor);
      mapeditor::initdraw(cwt.c);
      }
  #endif
  
    else if(xuni == 'x') {
      viewdists = !viewdists;
      }
  #if CAP_RUG
    else if(xuni == 'u') {
      if(sphere) projectionDialog();
      else rug::select();
      }
  #endif
    else if(uni == 'a')
      pushScreen(conformal::show);
  
  #if CAP_MODEL
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
    }
  else if(hardcore && canmove) { hardcore = false; }
  else { hardcore = true; canmove = true; hardcoreAt = turncount; }
  if(hardcore)
      addMessage("One wrong move, and it is game over!");
  else
      addMessage("Not so hardcore?");
  if(pureHardcore()) popScreenAll();
  }

void showChangeMode() {
  gamescreen(3);
  dialog::init(XLAT("special game modes"));
  
  // gameplay modes

#if CAP_TOUR
  dialog::addBoolItem(XLAT("Tutorial"), tour::on, 'T');
#endif

  dialog::addBoolItem(XLAT("experiment with geometry"), (euclid || sphere), 'e');
  dialog::addBoolItem(XLAT(SHMUPTITLE), (shmup::on || multi::players > 1), 's');
  if(!shmup::on) dialog::addSelItem(XLAT("hardcore mode"),
    hardcore && !pureHardcore() ? XLAT("PARTIAL") : ONOFF(hardcore), 'h');
  if(getcstat == 'h')
    mouseovers = XLAT("One wrong move and it is game over!");
  
  multi::cpid = 0;
  dialog::addBoolItem(XLAT("heptagonal mode"), (purehepta), '7');
  dialog::addBoolItem(XLAT("Chaos mode"), (chaosmode), 'C');
  dialog::addBoolItem(XLAT("peaceful mode"), peace::on, 'p');
  dialog::addBoolItem(XLAT("Orb Strategy mode"), (inv::on), 'i');
  dialog::addBoolItem(XLAT("pure tactics mode"), (tactic::on), 't');
  dialog::addBoolItem(XLAT("Yendor Challenge"), (yendor::on), 'y');
  dialog::addBoolItem(XLAT("%1 Challenge", moPrincess), (princess::challenge), 'P');
  dialog::addBoolItem(XLAT("random pattern mode"), (randomPatternsMode), 'r');

  dialog::addBreak(50);
  // cheating and map editor

  dialog::addBoolItem(XLAT("cheat mode"), (cheater), 'c');
#if CAP_EDIT
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
  #if ISMOBILE==0
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
    
    else if(xuni == 'e') {
      targettrunc = purehepta;
      pushScreen(showEuclideanMenu);
      }
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
  #if CAP_TOUR
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
    else if(xuni == 'P') {
      if(!princess::everSaved)
        addMessage(XLAT("Save %the1 first to unlock this challenge!", moPrincess));
      else
        restartGame('p');
      }
  #if CAP_EDIT
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
  #if ISMOBILE==1
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

// -- geometry menu --

int eupage = 0;
int euperpage = 21;

// test all the floor patterns!

#ifdef SCALETUNER
#define LAND_SPHEUC 26

eLand land_spheuc[LAND_SPHEUC] = {
  laDesert,
  laBull,
  laPalace,
  laWildWest,
  laPower,
  laStorms,
  laHell,
  laWhirlwind,
  laGraveyard,
  laTrollheim,
  laBurial,
  laVolcano,
  laRlyeh,
  laTortoise,
  laRose,
  laCrossroads,
  laCaves,
  laOvergrown,
  laAlchemist,
  laJungle,
  laMotion,
  laIce,
  laDragon,
  laKraken,
  laWarpCoast,
  laCaribbean
  };

#else

#define tweirdhyperbolic (targetgeometry == gOctagon || targetgeometry == g45 || targetgeometry == g46 || targetgeometry == g47)

#define LAND_SPHEUC ((tweirdhyperbolic) ? LAND_OCT : (targetgeometry > 1) ? LAND_SPH : LAND_EUC)
#define land_spheuc ((tweirdhyperbolic) ? land_oct : (targetgeometry > 1) ? land_sph : land_euc)
#endif

const char *curvenames[8] = {
  "0", "1", "2", "extremely hyperbolic", "strongly hyperbolic", "strongly hyperbolic", "moderately hyperbolic", "weakly hyperbolic"
  };

void useHyperrogueGeometry() {
  targetgeometry = geometry;
  restartGame('g');
  if(targettrunc != purehepta) restartGame('7');
  }

string euchelp =
  "If you want to know how much the gameplay is affected by the "
  "hyperbolic geometry in HyperRogue, this mode is for you!\n\n"
  
  "You can try many different geometries here. We start by gluing "
  "n-gons in such a way that k of them meet in every vertex. "
  "Depending on n and k, this either folds into a sphere, unfolds into a plane, "
  "or requires a hyperbolic space. The result may be then 'truncated' by "
  "replacing each vertex by a 2k-gon. Furthermore, you can play "
  "with quotient geometries. For example, the elliptic geometry is "
  "obtained from the sphere by making the antipodes be the same point, "
  "so you return to the same spot (but as a mirror image) after going there. "
  "Halloween is a land specially designed for the 'sphere' geometry; "
  "several more tilings can be played on by playing Graveyard or Warped Coast "
  "in a truncated tiling. " 
  "Have fun experimenting! Not all graphics and lands are implemented in "
  "all geometries, but lots of them are.";

void showPickGeometry() {
  int ts = ginf[targetgeometry].sides;
  int tv = ginf[targetgeometry].vertex;
  int tq = ginf[targetgeometry].quotientstyle;
  int nom = (targettrunc ? tv : tv+ts) * ((tq & qELLIP) ? 2 : 4);
  int denom = (2*ts + 2*tv - ts * tv);
  
  gamescreen(4);
  dialog::init(XLAT("select the geometry"));
  for(int i=0; i<gGUARD; i++)
    dialog::addBoolItem(XLAT(ginf[i].name), targetgeometry == i, 'a'+i);
  
  dialog::addBreak(50);

  if(ts == 6 && tv == 3)
    dialog::addSelItem("truncated", "does not matter", 't');
  else
    dialog::addBoolItem("truncated", !targettrunc, 't');

  dialog::addBreak(50);

  int worldsize = denom ? nom/denom : 0;
  if(tq & qTORUS) worldsize = torusconfig::qty;
  if(tq & qZEBRA) worldsize = targettrunc ? 12 : 40;
  if(tq & qFIELD) {
    worldsize = size(currfp.matrices) / ts;
    if(!targettrunc) worldsize = (10*worldsize) / 3;
    }

  dialog::addSelItem("sides per face", its(ts), 0);
  dialog::addSelItem("faces per vertex", its(tv), 0);

  string qstring = "none";
  if(tq & qZEBRA) qstring = "zebra";

  else if(tq & qFIELD) qstring = "field";

  else if(tq & qELLIP) qstring = "torus";

  else if(tq & qTORUS) qstring = "torus";

  dialog::addSelItem("quotient space", qstring, 0);

  dialog::addSelItem("size of the world", 
    XLAT(
      worldsize == 0 ? "infinite" : 
      worldsize > 0 ? "finite (%1)" :
      "exponentially infinite (%1)", its(worldsize)),
    0);
  
  switch(ginf[targetgeometry].cclass) {
    case 0:
      dialog::addSelItem("curvature", curvenames[ginf[targetgeometry].distlimit[targettrunc]], 0);
      break;
    
    case 1: 
      dialog::addSelItem("curvature", "flat", 0);
      break;
    
    case 2:
      dialog::addSelItem("curvature", "spherical", 0);
      break;
    }
  
  dialog::addItem(XLAT("help"), SDLK_F1);  
  dialog::addItem(XLAT("done"), '0');  
  dialog::display();

  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    if(uni >= 'a' && uni < 'a'+gGUARD)
      targetgeometry = eGeometry(uni - 'a');
    else if(uni == 't')
      targettrunc = !targettrunc;
    else if(uni == '2' || sym == SDLK_F1) gotoHelp(euchelp);
    else if(doexiton(sym, uni)) {
      popScreen();
      if(targetgeometry == gEuclid) targettrunc = false;
      if(targetgeometry == gNormal) popScreen(), useHyperrogueGeometry();      
      }
    };
  }

bool targettrunc;

void showEuclideanMenu() {
  gamescreen(4);
  int s = vid.fsize;
  vid.fsize = vid.fsize * 4/5;
  dialog::init(XLAT("experiment with geometry"));
  if(cheater) for(int i=0; i<landtypes; i++) landvisited[i] = true;
  for(int i=0; i<landtypes; i++)
    if(hiitemsMax(treasureType(eLand(i))) >= 25) landvisited[i] = true;
  landvisited[laCrossroads] = true;
  landvisited[laIce] = true;
  landvisited[laMirrorOld] = true;
  landvisited[laPrincessQuest] = cheater || princess::everSaved;
  landvisited[laWildWest] = true;
  landvisited[laHalloween] = true;
  landvisited[laWarpCoast] = true;
  landvisited[laGraveyard] = true;
  landvisited[laDual] = true;
  landvisited[laCA] = true;
  // for(int i=2; i<lt; i++) landvisited[i] = true;
  
  string truncatenames[2] = {" (t)", " (n)"};

  dialog::addSelItem(XLAT("geometry"), XLAT(ginf[targetgeometry].name) + truncatenames[targettrunc], '5');
  dialog::addBreak(50);
  
  eLand current = (geometry == targetgeometry) ? specialland : laNone;

  for(int i=0; i<euperpage; i++) {
    if(euperpage * eupage + i >= LAND_SPHEUC) { dialog::addBreak(100); break; }
    eLand l = land_spheuc[euperpage * eupage + i];
    if(landvisited[l]) {
      char ch;
      if(i < 26) ch = 'a' + i;
      else ch = 'A' + (i-26);
      dialog::addBoolItem(XLAT1(linf[l].name), l == current, ch);
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
    
    if(uni == '0') 
      useHyperrogueGeometry();
    else if(uni == '5') {
      pushScreen(showPickGeometry);
      }
    else if(uni == '-' || uni == PSEUDOKEY_WHEELUP || uni == PSEUDOKEY_WHEELDOWN) {
      eupage++;
      if(eupage * euperpage >= LAND_SPHEUC) eupage = 0;
      }
    else if(lid >= 0 && lid < LAND_SPHEUC) {
      specialland = land_spheuc[lid];
      if(landvisited[specialland] && specialland != laOceanWall) {
        if(targetgeometry != geometry)
          restartGame('g');
        else
          restartGame(tactic::on ? 't' : 0);
        // switch the truncated if necessary
        if(targettrunc != purehepta)
          restartGame('7');
        // disable PTM if chosen a land from the Euclidean menu
        if(tactic::on) restartGame('t');
        }
      else specialland = laIce;
      }
    else if(uni == '2' || sym == SDLK_F1) gotoHelp(euchelp);
    else if(doexiton(sym, uni)) popScreen();
    };
  }

bool showstartmenu;

bool showHalloween() {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  int month = tm.tm_mon + 1;
  int day = tm.tm_mday;
  if(month == 10 && day >= 24) return true;
  if(month == 11 && day <= 7) return true;
  return false;
  }

void showStartMenu() {
  gamescreen(2);

  getcstat = ' ';
  
  dialog::init();
  
  dialog::addInfo(XLAT("Welcome to HyperRogue!"));
  dialog::addBreak(100);

  dialog::addBigItem(XLAT("HyperRogue classic"), 'c');
  dialog::addInfo(XLAT("explore the world, collect treasures"));
  dialog::addInfo(XLAT("do not get checkmated"));

#if CAP_INV
  dialog::addBreak(100);
  dialog::addBigItem(XLAT("Orb Strategy mode"), 'i');
  dialog::addInfo(XLAT("use your Orbs in tough situations"));
#endif

#if CAP_SHMUP_GOOD
  dialog::addBreak(100);
  dialog::addBigItem(XLAT("shoot'em up mode"), 's');
  dialog::addInfo(XLAT("continuous spacetime"));
#if CAP_ACHIEVE
  dialog::addInfo(XLAT("(most achievements are not available)"));
#endif
#endif

#if CAP_TOUR
  dialog::addBreak(100);
  dialog::addBigItem(XLAT("tutorial"), 't');
  dialog::addInfo(XLAT("learn about hyperbolic geometry!"));
#endif

#if CAP_ROGUEVIZ && CAP_TOUR
  dialog::addBreak(100);
  dialog::addBigItem(XLAT("RogueViz"), 'r');
  dialog::addInfo(XLAT("see the visualizations"));
#endif

  if(showHalloween()) {
    dialog::addBreak(100);
    dialog::addBigItem(XLAT1("Halloween"), 'z');
    dialog::addInfo(XLAT("Halloween mini-game"));
    }

  dialog::addBreak(100);
  dialog::addBigItem(XLAT("main menu"), 'm');
  dialog::addInfo(XLAT("more options"));

  dialog::display();
  clearMessages();

  timerstart = time(NULL);
  
  /*
  initquickqueue();
  int siz = min(vid.xres, vid.yres) / 8;
  drawMonsterType(moPrincess, NULL, atscreenpos(siz,siz,siz) * spin(-M_PI/4), 0, 0);
  drawMonsterType(moKnight, NULL, atscreenpos(vid.xres-siz,siz,siz) * spin(-3*M_PI/4), 0, 0);
  drawItemType(itOrbYendor, NULL, atscreenpos(siz,vid.yres-siz,siz) * spin(M_PI/4), iinf[itOrbYendor].color, 0, false);
  drawItemType(itKey, NULL, atscreenpos(siz,vid.yres-siz,siz) * spin(M_PI/4), iinf[itKey].color, 0, false);
  drawItemType(itHyperstone, NULL, atscreenpos(vid.xres-siz,vid.yres-siz,siz) * spin(3*M_PI/4), iinf[itHyperstone].color, 0, false);
  quickqueue();
  */

  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    if(uni == 'o') uni = 'i';
    if(uni == 'c' || uni == 'i' || uni == 's') {
      popScreenAll();
      if(inv::on != (uni == 'i')) restartGame('i');
      if(shmup::on != (uni == 's')) restartGame('s');
      clearMessages();
      welcomeMessage();
      stampbase = ticks;
      }
    else if(uni == 'z') {
      popScreenAll();
      resetModes();
      stampbase = ticks;
      if(!sphere) {
        specialland = laHalloween;
        targetgeometry = gSphere;
        restartGame('g');
        vid.alpha = 999;
        vid.scale = 998;
        }
      }
#if CAP_TOUR
    else if(uni == 't') {
      popScreenAll();
      tour::start();
      }
#endif
#if CAP_ROGUEVIZ && CAP_TOUR
    else if(uni == 'r') {
      tour::slides = rogueviz::rvtour::rvslides;
      popScreenAll();
      tour::start();
      }
#endif
    else if(sym == 'm') {
      popScreen();
      pushScreen(showMainMenu);
      }
    else if(sym == SDLK_F10)
      quitmainloop = true;
    else if(sym == SDLK_F1)
      gotoHelp(help);
    else if(sym == SDLK_ESCAPE || sym == ' ') {
      popScreen();
      timerstart = time(NULL);
      stampbase = ticks;
      clearMessages();
      welcomeMessage();
      }
    };
  }
 
// -- overview --

void setAppropriateOverview() {
  clearMessages();
  if(tactic::on)
    pushScreen(tactic::showMenu);
  else if(yendor::on)
    pushScreen(yendor::showMenu);
  else if(peace::on)
    pushScreen(peace::showMenu);
  else if(geometry != gNormal) {
    targettrunc = purehepta;
    pushScreen(showEuclideanMenu);
    }
  else {
    mapeditor::infix = "";
    pushScreen(showOverview);
    }
  }

int messagelogpos;
int timeformat;
int stampbase;

string gettimestamp(msginfo& m) {
  char buf[128]; 
  switch(timeformat) {
    case 0:
      return its(m.turnstamp);
    case 1:
      strftime(buf, 128, "%H:%M:%S", localtime(&m.rtstamp));
      return buf;
    case 2:
      snprintf(buf, 128, "%2d:%02d", m.gtstamp/60, m.gtstamp % 60);
      return buf;
    case 3:
      { int t = m.stamp - stampbase; 
      bool sign = t < 0;
      if(sign) t = -t;
      snprintf(buf, 128, "%2d:%02d.%03d", t/60000, (t/1000) % 60, t % 1000);
      string s = buf;
      if(sign) s = "-"+s;
      return s;
      }
    }     
  return "";
  }
  
void showMessageLog() {
  DEBB(DF_GRAPH, (debugfile,"show message log\n"));

  int lines = vid.yres / vid.fsize - 2;
  int maxpos = size(gamelog) - lines;
  messagelogpos = min(messagelogpos, maxpos);
  messagelogpos = max(messagelogpos, 0);
    
  for(int y=0; y<lines && messagelogpos+y < size(gamelog); y++) {
    msginfo& m = gamelog[messagelogpos+y];
    displaystr(vid.fsize*8, vid.fsize*(y+1), 0, vid.fsize, fullmsg(m), 0xC0C0C0, 0);
    displaystr(vid.fsize*7, vid.fsize*(y+1), 0, vid.fsize, gettimestamp(m), 0xC0C0C0, 16);
    }

  int i0 = vid.yres - vid.fsize;
  int xr = vid.xres / 80;
  
  string timeformats[5] = {"turns", "real time", "game time", "precise", "no time"};
  
  displayButton(xr*70, i0, IFM("0 - ") + XLAT("back"), '0', 8);
  displayButton(xr*10, i0, IFM("c - ") + XLAT("clear"), '0', 8);
  displayButton(xr*40, i0, IFM("t - ") + XLAT(timeformats[timeformat]), '0', 8);
  
  keyhandler = [lines, maxpos] (int sym, int uni) {
    if(uni == PSEUDOKEY_WHEELDOWN) messagelogpos++;
    else if(uni == PSEUDOKEY_WHEELUP) messagelogpos--;
    else if(uni == SDLK_DOWN || uni == SDLK_KP2) messagelogpos++;
    else if(uni == SDLK_UP || uni == SDLK_KP8) messagelogpos--;
    else if(uni == SDLK_PAGEUP || uni == SDLK_KP9) messagelogpos -= lines;
    else if(uni == SDLK_PAGEDOWN || uni == SDLK_KP3) messagelogpos -= lines;
    else if(uni == 'c') gamelog.clear();
    else if(uni == 't') { timeformat++; timeformat %= 5; }
    else if(doexiton(sym, uni)) popScreen();
    };
  }
