// HyperRogue menus
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// -- overview --

#define BLACKISH 0x404040
#define REDDISH 0x400000

namespace hr {
EX ld whatever[16];
EX int whateveri[16];

int PREC(ld x) {
  ld sh = shiftmul;
  if(sh > -.2 && sh < .2) x = 10.01; 
  return int(shiftmul * x);
  }

EX void showOverview() {
  cmode = sm::ZOOMABLE | sm::OVERVIEW;  
  DEBBI(DF_GRAPH, ("show overview"));

  if(dialog::infix != "")
    mouseovers = dialog::infix;
  else {
    mouseovers = XLAT("world overview");
    mouseovers += "       ";
    mouseovers += XLAT(" kills: %1/%2", its(tkills()), its(killtypes()));
    mouseovers += XLAT(" $$$: %1", its(gold()));
    if(randomPatternsMode) ;
    else if(landUnlocked(laHell)) {
      int i1, i2; countHyperstoneQuest(i1, i2);
      mouseovers += XLAT(" Hyperstone: %1/%2", its(i1), its(i2));
      }
    else
      mouseovers += XLAT(" Hell: %1/9", its(orbsUnlocked()));
    }
  
  bool pages;
  
  {
  dynamicval<int> ds(dual::state, dual::state ? 2 : 0);
  generateLandList(isLandIngame);
  }
  
  bool not_in_game = false;
  
  if(dialog::infix != "") {
    auto land_matches = [] (eLand l) {
      string s = dnameof(l);
      s += "@";
      s += dnameof(treasureType(l));
      s += "@";
      s += dnameof(nativeOrbType(l));
      return dialog::hasInfix(s);
      };
    
    vector<eLand> filtered;
    for(eLand l: landlist) if(land_matches(l)) filtered.push_back(l);
    if(filtered.size())
      landlist = filtered;
    else {
      for(int i=0; i<landtypes; i++) if(land_matches(eLand(i))) filtered.push_back(eLand(i));
      if(filtered.size()) landlist = filtered, not_in_game = true;
      }
    }
  
  int nl = isize(landlist), nlm;
  
  int lstart = 0;
  
  if(nl > 30) {
    pages = true;
    lstart += dialog::handlePage(nl, nlm, (nl+1)/2);
    }
  else nlm = nl;
  
  int vf = min((vid.yres-64-vid.fsize*2) / nlm + (not_in_game ? 1 : 0), vid.xres/40);

  eLand curland = getLandForList(cwt.at);
  
  getcstat = '0';
  
  if(not_in_game) {
    int i0 = 56 + vid.fsize + nl * vf;
    displayfrZ(64, i0, 1, vf-4, "(these lands are not in game)", forecolor, 0);
    }
  
  for(int i=0; i<nl; i++) {
    eLand l = landlist[lstart + i];
    int xr = vid.xres / 64;
    int i0 = 56 + vid.fsize + i * vf;
    color_t col;
    if(landUnlocked(l)) col = linf[l].color; else col = 0x404040;
    if(l == curland)
      displayfrZ(1, i0, 1, vf-4, "*", forecolor, 0);
    if(displayfrZH(xr*1, i0, 1, vf-4, XLAT1(linf[l].name), col, 0))
      getcstat = 1000 + l;
    eItem it = treasureType(l);
    int lv = items[it] * landMultiplier(l);
    if(lv >= 25) col = 0xFFD500;
    else if(lv && it == itSavedPrincess) col = 0xFFD500;
    else if(lv >= 10) col = 0x00D500;
    else if(items[it]) col = 0xC0C0C0;
    else col = BLACKISH;
    int c8 = (vf+2)/3;
    if(displayfrZH(xr*24-c8*6, i0, 1, vf-4, its(items[it]), col, 16))
      getcstat = 2000+it;
    if(!cheater)
    if(displayfrZH(xr*24, i0, 1, vf-4, its(hiitems[modecode()][it]), col, 16))
      getcstat = 2000+it;
    if(items[it]) col = iinf[it].color; else col = BLACKISH;
    if(displayfrZH(xr*24+c8*4, i0, 1, vf-4, s0 + iinf[it].glyph, col, 16))
      getcstat = 2000+it;
    if(displayfrZH(xr*24+c8*5, i0, 1, vf-4, XLAT1(iinf[it].name), col, 0))
      getcstat = 2000+it;
    eItem io = nativeOrbType(l);
    if(io == itShard) {
      if(items[it] >= 10) col = winf[waMirror].color; else col = BLACKISH;
      if(displayfrZH(xr*46, i0, 1, vf-4, XLAT1(winf[waMirror].name), col, 0))
        getcstat = 3000+waMirror;
      if(getcstat == 3000+waMirror)
        mouseovers = XLAT(
          olrDescriptions[getOLR(io, cwt.at->land)], cwt.at->land, it, treasureTypeUnlock(curland, io));
      }
    else if(io) {
      if(lv >= 25) col = 0xFFD500;
      else if(lv >= 10) col = 0xC0C0C0;
      else col = BLACKISH;
      if(displayfrZH(xr*46-c8*4, i0, 1, vf-4, its(items[io]), col, 16))
        getcstat = 2000+io;
      if(lv >= 10) col = iinf[io].color; else col = BLACKISH;
      if(displayfrZH(xr*46-c8, i0, 1, vf-4, s0 + iinf[io].glyph, col, 16))
        getcstat = 2000+io;
      if(displayfrZH(xr*46, i0, 1, vf-4, XLAT1(iinf[io].name), col, 0))
        getcstat = 2000+io;
      if(getcstat == 2000+io)
        mouseovers = XLAT(
          olrDescriptions[getOLR(io, curland)], curland, it, treasureTypeUnlock(curland, io));
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
    else if(dialog::editInfix(uni)) ;
    else if(doexiton(sym, uni)) popScreen();
    };
  }

// -- main menu --

EX purehookset hooks_mainmenu;

EX void showMainMenu() {
  gamescreen(2);

  getcstat = ' ';
  
  dialog::init(XLAT("HyperRogue %1", VER), 0xC00000, 200, 100);

  dialog::addItem(XLAT("settings"), 's');
  dialog::add_action_push(showSettings);
  dialog::addItem(XLAT("special modes"), 'm');

#if CAP_SAVE
  dialog::addItem(XLAT("local highscores"), 't');
#endif
  dialog::addHelp();
  if(cheater)
    dialog::addItem(XLAT("cheats"), 'c');
  else dialog::addBreak(100);
  dialog::addItem(XLAT("restart game"), 'r'); dialog::lastItem().keycaption += " / F5";

  dialog::addItem(XLAT(inSpecialMode() ? "reset special modes" : "back to the start menu"), 'R');
  
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
  dialog::addItem(get_o_key().first, 'o');    

  if(inv::on)
    dialog::addItem(XLAT("inventory"), 'i');    

#if ISMOBILE==1
#if CAP_ACHIEVE
  dialog::addItem(XLAT("leaderboards/achievements"), '3'); 
#endif
#endif

#if CAP_ANDROIDSHARE
  dialog::addItem("SHARE", 's'-96);
#endif

  callhooks(hooks_mainmenu);
  
  if(!canmove) q = "review the scene";
  else if(turncount > 0) q = "continue game";
  else q = "play the game!";
  
  dialog::addItem(XLAT(q), ' ');
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    if(sym == SDLK_F1 || uni == 'h') gotoHelp("@");
    else if(uni == 'c' && cheater) pushScreen(showCheatMenu);
    else if(uni == 'm') pushScreen(showChangeMode);
    else if(uni == 'R') dialog::do_if_confirmed([] {
      #if CAP_STARTANIM
      startanims::pick();
      #endif
      popScreenAll(), pushScreen(showStartMenu);
      });
  #if CAP_SAVE
    else if(uni == 't') scores::load();
  #endif
    else if(uni == 'r' || sym == SDLK_F5) dialog::do_if_confirmed([] {
      restart_game();
      });
    else if(uni == 'q' || sym == SDLK_F10) {
      if(needConfirmation()) dialog::do_if_confirmed([] {
#if ISMOBILE
        extern void openURL();
        openURL();
#else
        quitmainloop = true;
#endif
        });
      else quitmainloop = true;
      }
    else if(uni == 'o') {
      clearMessages();
      get_o_key().second();
      }
#if CAP_INV
    else if(uni == 'i') {
      clearMessages();
      pushScreen(inv::show);
      }
#endif
    else if(sym == SDLK_ESCAPE) 
      showMissionScreen();
  #if ISMOBILE==1
  #ifdef HAVE_ACHIEVEMENTS
    else if(NUMBERKEY == '3') {
      achievement_final(false);
      pushScreen(leader::showMenu);
      }
  #endif
  #endif
    else if(doexiton(sym, uni)) {
      popScreenAll();
      msgs.clear();
      }
    };
  }

// -- display modes --

EX void editScale() {
  dialog::editNumber(vid.scale, .001, 1000, .1, 1, XLAT("scale factor"), 
    XLAT("Scale the displayed model."));
  dialog::scaleSinh();
  }

EX void showGraphQuickKeys() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);

  dialog::init(XLAT("quick options"));

  if(DIM == 2) {
    dialog::addBoolItem(XLAT("orthogonal projection"), vid.alpha >= 500, '1');
    dialog::addBoolItem(XLAT(sphere ? "stereographic projection" : euclid ? "zoomed out" : "small Poincaré model"), vid.alpha == 1 && vid.scale < 1, '2');
    dialog::addBoolItem(XLAT(sphere ? "zoomed stereographic projection" : euclid ? "zoomed in" : "big Poincaré model"), vid.alpha == 1 && vid.scale >= 1, '3');
    dialog::addBoolItem(XLAT((sphere || euclid) ? "gnomonic projection" : "Klein-Beltrami model"), vid.alpha == 0, '4');
    }
  else {
    dialog::addBoolItem(XLAT("first person perspective"), vid.yshift == 0 && vid.sspeed > -5, '1');
    dialog::addBoolItem(XLAT("fixed point of view"), vid.sspeed <= -5, '2');
    dialog::addBoolItem(XLAT("third person perspective"), vid.yshift > 0 && vid.sspeed > -5, '3');
    }

  const char *wdmodes[6] = {"ASCII", "black", "plain", "Escher", "plain/3D", "Escher/3D"};
  dialog::addSelItem(XLAT("wall display mode"), XLAT(wdmodes[vid.wallmode]), '5');

  const char *mdmodes[6] = {"ASCII", "items only", "items and monsters", "high contrast",
    "3D", "high contrast/3D"};
  dialog::addSelItem(XLAT("monster display mode"), XLAT(mdmodes[vid.monmode]), '8');

  dialog::addBoolItem(XLAT("draw the grid"), (vid.grid), '6');
  dialog::addBoolItem(XLAT("mark heptagons"), (vid.darkhepta), '7');

  dialog::addBreak(50);  
  dialog::addInfo(XLAT("Hint: these keys usually work during the game"));
  dialog::addInfo(XLAT("also hold Alt during the game to toggle high contrast"));
 
  dialog::addBreak(50);  
  dialog::addBack();
  dialog::display();

  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    
    if(gmodekeys(sym, uni)) ;
    else if(doexiton(sym, uni)) popScreen();
    };
  }

EX void enable_cheat() {
  if(tactic::on && gold()) {
    addMessage(XLAT("Not available in the pure tactics mode!"));
    }
  else if(daily::on) {
    addMessage(XLAT("Not available in the daily challenge!"));
    }
  else if(!cheater) dialog::cheat_if_confirmed([] {
    cheater++;
    addMessage(XLAT("You activate your demonic powers!"));
#if ISMOBILE==0
    addMessage(XLAT("Shift+F, Shift+O, Shift+T, Shift+L, Shift+U, etc."));
#endif
    popScreen();
    });
  else {
    popScreen();
    firstland = princess::challenge ? laPalace : laIce;
    restart_game();
    }
  }
  
// -- game modes -- 

EX void switchHardcore() {
  if(hardcore && !canmove) { 
    restart_game();
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

EX void showCreative() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(3);
  dialog::init(XLAT("creative mode"));

#if CAP_EDIT
  dialog::addItem(XLAT("map editor"), 'm');
  dialog::add_action([] {
    if(tactic::on) 
      addMessage(XLAT("Not available in the pure tactics mode!"));
    else if(daily::on) {
      addMessage(XLAT("Not available in the daily challenge!"));
      }
    else dialog::cheat_if_confirmed([] {
      cheater++;
      pushScreen(mapeditor::showMapEditor);
      lastexplore = turncount;
      addMessage(XLAT("You activate your terraforming powers!"));
      });
    });
#endif

#if CAP_EDIT
  dialog::addItem(XLAT("vector graphics editor"), 'g');
  dialog::add_action([] {
    pushScreen(mapeditor::showDrawEditor);
    mapeditor::initdraw(cwt.at);
    });
#endif

  // display modes  
#if CAP_MODEL
  if(DIM == 2) {
    dialog::addItem(XLAT("paper model creator"), 'n');
    dialog::add_action([] { netgen::run(); });
    }
#endif

  #if CAP_SHOT
  dialog::addItem(XLAT("screenshots"), 's');
  dialog::add_action_push(shot::menu);
  #endif
     
  #if CAP_ANIMATIONS
  dialog::addBoolItem(XLAT("animations"), anims::any_on(), 'a');
  dialog::add_action_push(anims::show);
  #endif

  dialog::addBoolItem(XLAT("history mode"), conformal::on || conformal::includeHistory, 'h');
  dialog::add_action_push(conformal::history_menu);

#if CAP_TEXTURE
  if(DIM == 2) {
    dialog::addBoolItem(XLAT("texture mode"), texture::config.tstate == texture::tsActive, 't');
    dialog::add_action_push(texture::showMenu);
    }
#endif

  dialog::addBoolItem(XLAT("cheat mode"), (cheater), 'c');
  dialog::add_action(enable_cheat);

//  dialog::addBoolItem(XLAT("expansion"), viewdists, 'x');
  
  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  }

EX void show_chaos() {
  gamescreen(3);
  dialog::init(XLAT("Chaos mode"));
  chaosUnlocked = chaosUnlocked || autocheat;

  dialog::addHelp(
    "In the Chaos mode, lands change very often, and "
    "there are no walls between them. "
    "Some lands are incompatible with this."
    "\n\nYou need to reach Crossroads IV to unlock the Chaos mode."
    );
  
  dialog::addBreak(100);
  
  dialog::addBoolItem(XLAT("Chaos mode") + " " + ONOFF(false), !chaosmode, 'A');
  dialog::add_action([] { dialog::do_if_confirmed([] { if(chaosUnlocked) restart_game(rg::chaos); }); });
  
  if(chaosUnlocked) for(int a=1; a<5; a++) {
    if(a > 1 && ISWEB) continue;
    dialog::addBoolItem(
      a == 1 ? XLATN("Crossroads IV") : 
      a == 2 ? XLATN("Palace") : 
      a == 3 ? XLAT("total chaos") :
      XLAT("random walk"), 
      chaosmode == a, 'A' + a);
    dialog::add_action([a] { dialog::do_if_confirmed([a] { 
      int cm = chaosmode;
      stop_game_and_switch_mode(rg::chaos); 
      if(!chaosmode && cm != a) switch_game_mode(rg::chaos); 
      if(chaosmode) chaosmode = a; 
      start_game(); 
      }); });
    }

  dialog::addBreak(100);
  dialog::addBack();
  dialog::display();
  }
  
EX void showChangeMode() {
  gamescreen(3);
  dialog::init(XLAT("special modes"));
                               // gameplay modes

#if CAP_TOUR
  dialog::addBoolItem(XLAT("Tutorial"), tour::on, 'T');
#endif
  dialog::addBoolItem(XLAT("creative mode"), (false), 'c');
  dialog::add_action_push(showCreative);
  dialog::addBoolItem(XLAT("experiment with geometry"), geometry || CHANGED_VARIATION || viewdists, 'e');

  dialog::addBreak(100);

  dialog::addBoolItem(XLAT(SHMUPTITLE), shmup::on, 's');
  dialog::addBoolItem(XLAT("multiplayer"), multi::players > 1, 'm');
  dialog::add_action_push(multi::showConfigureMultiplayer);
  if(!shmup::on) dialog::addSelItem(XLAT("hardcore mode"),
    hardcore && !pureHardcore() ? XLAT("PARTIAL") : ONOFF(hardcore), 'h');
  if(getcstat == 'h')
    mouseovers = XLAT("One wrong move and it is game over!");
  
  multi::cpid = 0;
  dialog::addBoolItem(XLAT("Chaos mode"), (chaosmode), 'C');
  dialog::add_action_push(show_chaos);
  dialog::addBoolItem(XLAT("peaceful mode"), peace::on, 'p');
  dialog::addBoolItem(XLAT("Orb Strategy mode"), (inv::on), 'i');
  dialog::addBoolItem(XLAT("pure tactics mode"), (tactic::on), 't');
  dialog::addBoolItem(XLAT("Yendor Challenge"), (yendor::on), 'y');
  dialog::addBoolItem(XLAT("%1 Challenge", moPrincess), (princess::challenge), 'P');
  dialog::addBoolItem(XLAT("random pattern mode"), (randomPatternsMode), 'r');
#if CAP_RACING
  dialog::addBoolItem(XLAT("racing mode"), racing::on, 'R');
#endif
#if CAP_ARCM && !ISWEB
  if(multi::players == 1) {
    dialog::addBoolItem(XLAT("dual geometry mode"), dual::state, 'D');
    dialog::add_action([] { dialog::do_if_confirmed([] { restart_game(rg::dualmode); }); });
    }
#endif
#if CAP_DAILY
  dialog::addBoolItem(XLAT("Strange Challenge"), daily::on, 'z');
#endif

  dialog::addBreak(50);
  
  dialog::addBack();
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);    
    char xuni = uni;

    if(xuni == ' ' || sym == SDLK_ESCAPE) popScreen();
    
    #if CAP_DAILY
    else if(uni == 'z')
      pushScreen(daily::showMenu);
    #endif
    
    else if(xuni == 'e') 
      runGeometryExperiments();
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
    else if(xuni == 'p')
      pushScreen(peace::showMenu);
    #if CAP_RACING
    else if(xuni == 'R')
      racing::configure_race();
    #endif
    else if(xuni == 'i') dialog::do_if_confirmed([] {
      restart_game(rg::inv);
      });
  #if CAP_TOUR
    else if(uni == 'T') dialog::do_if_confirmed([] {
      tour::start();
      });
  #endif
    else if(xuni == 'P') {
      if(!princess::everSaved)
        addMessage(XLAT("Save %the1 first to unlock this challenge!", moPrincess));
      else
        dialog::do_if_confirmed([] { restart_game(rg::princess); });
      }
    else if(xuni == 's') 
      dialog::do_if_confirmed(shmup::switch_shmup);

    else if(xuni == 'h' && !shmup::on) 
      switchHardcore();
    else if(xuni == 'r') {
      dialog::do_if_confirmed([] { 
        stop_game();
        firstland = laIce;
        restart_game(rg::randpattern); 
        });
      }
    else if(doexiton(sym, uni))
      popScreen();
    };
  }

EX bool showstartmenu;

EX bool showHalloween() {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  int month = tm.tm_mon + 1;
  int day = tm.tm_mday;
  if(month == 10 && day >= 24) return true;
  if(month == 11 && day <= 7) return true;
  return false;
  }

int daily_mode;

purehookset hooks_startmenu;

EX void showStartMenu() {
  if(!daily_mode) {
    daily_mode = hrand(10) + 1;
    if(showHalloween())
      daily_mode = 20;
    }
  
  #if CAP_STARTANIM
  startanims::current();
  #endif

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

#if CAP_TOUR
  dialog::addBreak(100);
  dialog::addBigItem(XLAT("tutorial"), 't');
  dialog::addInfo(XLAT("learn about hyperbolic geometry!"));
#endif

  switch(daily_mode) { 
    case 1:
      #if CAP_SHMUP_GOOD
        dialog::addBreak(100);
        dialog::addBigItem(XLAT("shoot'em up mode"), 's');
        dialog::addInfo(XLAT("continuous spacetime"));
      #if CAP_ACHIEVE
        dialog::addInfo(XLAT("(most achievements are not available)"));
      #endif
      #endif
      break;
    
    case 2: 
      dialog::addBreak(100);
      dialog::addBigItem(XLAT("heptagonal mode"), '7');
      dialog::addInfo(XLAT("more curvature"));
      dialog::addInfo(XLAT("(most achievements are not available)"));
      break;      
    
    case 3: 
      dialog::addBreak(100);
      dialog::addBigItem(XLAT("geometry experiments"), 'g');
      dialog::addInfo(XLAT("(most achievements are not available)"));
      break;      
    
    case 4: 
      if(chaosUnlocked) {
        dialog::addBreak(100);
        dialog::addBigItem(XLAT("Chaos mode"), 'C');
        dialog::addInfo(XLAT("(most achievements are not available)"));
        }
      break;      

#if CAP_RUG    
    case 5:
      dialog::addBreak(100);
      dialog::addBigItem(XLAT("hypersian rug mode"), 'M');
      dialog::addInfo(XLAT("see the true form"));
      break;
#endif

#if CAP_TEXTURE && CAP_EDIT
    case 6:
      dialog::addBreak(100);
      dialog::addBigItem(XLAT("texture mode"), 'T');
      dialog::addInfo(XLAT("paint pictures"));
      break;
#endif

#if CAP_DAILY
    case 7:
      dialog::addBreak(100);
      dialog::addBigItem(XLAT("Strange Challenge"), 'z');
      dialog::addInfo(XLAT("compete with other players on random lands in random geometries"));
      break;
#endif
    
    case 20:
      dialog::addBreak(100);
      dialog::addBigItem(XLAT1("Halloween"), 'Z');
      dialog::addInfo(XLAT("Halloween mini-game"));
      break;
    }

  callhooks(hooks_startmenu);

  if(have_current_settings()) {
    dialog::addBreak(100);
    dialog::addBigItem(XLAT1("use current/saved settings"), SDLK_ESCAPE);
    }
  
  if(have_current_graph_settings()) {
    dialog::addBreak(100);
    dialog::addBigItem(XLAT1("reset the graphics settings"), 'r');
    dialog::add_action([] () { reset_graph_settings(); });
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
#if CAP_RUG
    else if(uni == 'M') {
      rug::init();
      popScreenAll();
      resetModes('c');
      clearMessages();
      welcomeMessage();
      vid.wallmode = 3;
      vid.monmode = 2;
      rug::model_distance *= 2;
      rug::init();
      }
#endif
#if CAP_TEXTURE && CAP_EDIT
    else if(uni == 'T') {
      popScreenAll();
      pushScreen(texture::showMenu);
      resetModes('c');
      stop_game();
      firstland = specialland = laCanvas;
      cheater = true;
      patterns::canvasback = 0xFFFFFF;
      mapeditor::drawplayer = false;
      start_game();
      clearMessages();
      welcomeMessage();
      calcparam();
      drawthemap();
      texture::start_editor();
      }
#endif
    else if(uni == 'g') {
      popScreenAll();
      resetModes('c');
      clearMessages();
      welcomeMessage();
      pushScreen(showEuclideanMenu);
      }
    else if(uni == 'c' || uni == 'i' || uni == 's' || uni == 'C' || uni == '7') {
      if(uni == 'C' && !chaosUnlocked) {
        return;
        }
      popScreenAll();
      resetModes(uni);
      clearMessages();
      welcomeMessage();
      stampbase = ticks;
      if(uni == 's') 
        shmup::configure();
      }
    else if(uni == 'Z') {
      popScreenAll();
      resetModes('g');
      stampbase = ticks;
      if(!sphere) {
        stop_game();
        specialland = laHalloween;
        set_geometry(gSphere);
        start_game();
        vid.alpha = 999;
        vid.scale = 998;
        }
      }
#if CAP_TOUR
    else if(uni == 't') {
      popScreenAll();
      resetModes('c');
      tour::start();
      }
#endif
#if CAP_DAILY
    else if(uni == 'z') {
      popScreenAll();
      pushScreen(daily::showMenu);
      }
#endif
    else if(uni == 'm') {
      popScreen();
      pushScreen(showMainMenu);
      }
    else if(sym == SDLK_F10)
      quitmainloop = true;
    else if(sym == SDLK_F1)
      gotoHelp("@");
    else if(sym == SDLK_ESCAPE || uni == ' ') {
      popScreen();
      timerstart = time(NULL);
      stampbase = ticks;
      clearMessages();
      welcomeMessage();
      }
    #if CAP_STARTANIM
    else if(sym == SDLK_F5) startanims::pick();
    #endif
    };
  }
 
// -- overview --

#if HDR
typedef pair<string, reaction_t> named_functionality;
inline named_functionality named_dialog(string x, reaction_t dialog) { return named_functionality(x, [dialog] () { pushScreen(dialog); }); }
#endif

EX hookset<named_functionality()> *hooks_o_key;

EX named_functionality get_o_key() {

  if(hooks_o_key) for(auto& h: *hooks_o_key) {
    auto res = h.second();
    if(res.first != "") return res;
    }
  
#if CAP_DAILY
  if(daily::on) 
    return named_functionality(XLAT("Strange Challenge"), [] () {
      achievement_final(false);
      pushScreen(daily::showMenu);
      });
#endif

  if(viewdists)
    return named_functionality(XLAT("geometry experiments"), runGeometryExperiments);

  if(tactic::on)
    return named_dialog(XLAT("Pure Tactics mode"), tactic::showMenu);
    
  if(yendor::on)
    return named_dialog(XLAT("Yendor Challenge"), yendor::showMenu);

  if(peace::on)
    return named_dialog(XLAT("peaceful mode"), peace::showMenu);

  dialog::infix = "";

  if(in_full_game())
    return named_dialog(XLAT("world overview"), showOverview);
  
  if(geometry != gNormal || NONSTDVAR)
    return named_functionality(XLAT("geometry experiments"), runGeometryExperiments);

  return named_dialog(XLAT("world overview"), showOverview);
  }

int messagelogpos;
int timeformat;
int stampbase;

EX string gettimestamp(msginfo& m) {
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
  
EX void showMessageLog() {
  DEBBI(DF_GRAPH, ("show message log"));

  int lines = vid.yres / vid.fsize - 2;
  int maxpos = isize(gamelog) - lines;
  messagelogpos = min(messagelogpos, maxpos);
  messagelogpos = max(messagelogpos, 0);
    
  for(int y=0; y<lines && messagelogpos+y < isize(gamelog); y++) {
    msginfo& m = gamelog[messagelogpos+y];
    displaystr(vid.fsize*8, vid.fsize*(y+1), 0, vid.fsize, fullmsg(m), 0xC0C0C0, 0);
    displaystr(vid.fsize*7, vid.fsize*(y+1), 0, vid.fsize, gettimestamp(m), 0xC0C0C0, 16);
    }

  int i0 = vid.yres - vid.fsize;
  int xr = vid.xres / 80;
  
  string timeformats[5] = {"turns", "real time", "game time", "precise", "no time"};
  
  displayButton(xr*70, i0, IFM(dialog::keyname(SDLK_ESCAPE) + " - ") + XLAT("go back"), SDLK_ESCAPE, 8);
  displayButton(xr*10, i0, IFM("c - ") + XLAT("clear"), 'c', 8);
  displayButton(xr*40, i0, IFM("t - ") + XLAT(timeformats[timeformat]), 't', 8);
  
  keyhandler = [lines] (int sym, int uni) {
    if(uni == PSEUDOKEY_WHEELDOWN) messagelogpos++;
    else if(uni == PSEUDOKEY_WHEELUP) messagelogpos--;
    else if(DKEY == SDLK_DOWN) messagelogpos++;
    else if(DKEY == SDLK_UP) messagelogpos--;
    else if(DKEY == SDLK_PAGEUP) messagelogpos -= lines;
    else if(DKEY == SDLK_PAGEDOWN) messagelogpos -= lines;
    else if(uni == 'c') gamelog.clear();
    else if(uni == 't') { timeformat++; timeformat %= 5; }
    else if(doexiton(sym, uni)) popScreen();
    };
  }

#if CAP_COMMANDLINE

int read_menu_args() {
  using namespace arg;

  if(argis("-d:over")) {
    PHASEFROM(2); launch_dialog(showOverview);
    }
  else if(argis("-d:main")) {
    PHASEFROM(2); launch_dialog(showMainMenu);
    }
  else if(argis("-d:mode")) {
    PHASEFROM(2); launch_dialog(showChangeMode);
    }
  else if(argis("-d:shmup")) {
    PHASEFROM(2); launch_dialog(); shmup::configure();
    }
  else return 1;
  return 0;
  }

auto ah_menu = addHook(hooks_args, 0, read_menu_args);

#endif

}
