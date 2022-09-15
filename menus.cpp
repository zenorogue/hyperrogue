// Hyperbolic Rogue -- menus
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file menus.cpp
 *  \brief generic menus (start menu, main menu, Overview, etc.)
 */

// -- overview --

#include "hyper.h"
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
      mouseovers += XLAT(" Hell: %1/%2", its(orbsUnlocked()), its(lands_for_hell()));
    }
  
  bool pages = false;
  
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
    if(displayfrZH(xr*24-c8*6, i0, 1, vf-4, (required_for_hyperstones(it) ? "" : "*") + its(items[it]), col, 16))
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
      if(getcstat == 3000+waMirror) {
        string olrdesc = olrDescriptions[getOLR(io, cwt.at->land)];
        mouseovers = XLAT(olrdesc, cwt.at->land, it, treasureTypeUnlock(curland, io));
        }
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
      if(getcstat == 2000+io) {
        string olrdesc = olrDescriptions[getOLR(io, curland)];
        mouseovers = XLAT(olrdesc, curland, it, treasureTypeUnlock(curland, io));
        }
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
        dialog::bound_low(0);
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

// -- display modes --

EX void editScale() {
  dialog::editNumber(vpconf.scale, .001, 1000, .1, 1, XLAT("scale factor"), 
    XLAT("Scale the displayed model."));
  dialog::scaleSinh();
  }

EX const char *wdmodes[7] = {"ASCII", "black", "plain", "Escher", "plain/3D", "Escher/3D", "ASCII/3D"};
EX const char *mdmodes[6] = {"ASCII", "items only", "items and monsters", "3D", "?", "?"};
EX const char *hlmodes[3] = {"press Alt", "highlight", "super-highlight"};

EX void showGraphQuickKeys() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();

  dialog::init(XLAT("quick options"));
  
  dialog::addItem("quick projection", '1');

  dialog::addSelItem(XLAT("wall display mode"), XLAT(wdmodes[vid.wallmode]), '5');

  dialog::addSelItem(XLAT("monster display mode"), XLAT(mdmodes[vid.monmode]), '8');

  dialog::addSelItem(XLAT("highlight stuff"), XLAT(hlmodes[vid.highlightmode]), 508);

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
#if !ISMOBILE
    addMessage(XLAT("Shift+F, Shift+O, Shift+T, Shift+L, Shift+U, etc."));
#endif
    popScreen();
    });
  else {
    popScreen();
    specialland = firstland = princess::challenge ? laPalace : laIce;
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
      addMessage(XLAT("One wrong move and it is game over!"));
  else
      addMessage(XLAT("Not so hardcore?"));
  if(pureHardcore()) popScreenAll();
  }

EX void switch_casual() {
  if(savecount > 0) {
    dialog::push_confirm_dialog([] {
      restart_game();
      casual = !casual;
      }, XLAT("Switching casual allowed only before saving the game. Do you want to restart?"));
    return;
    }
  else
    casual = !casual;
  if(casual) {
    addMessage(XLAT("You are in the casual mode! Achievements are disabled.")); 
    addMessage(XLAT("Collect an Orb of Safety to save a checkpoint."));
    }
  popScreenAll();
  }

EX void showCreative() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("creative mode"));

#if CAP_EDIT
  dialog::addItem(XLAT("map editor"), 'm');
  dialog::add_action([] {
    dialog::cheat_if_confirmed([] {
      cheater++;
      pushScreen(mapeditor::showMapEditor);
      lastexplore = turncount;
      addMessage(XLAT("You activate your terraforming powers!"));
      });
    });
#endif

#if CAP_EDIT
  dialog::addItem(XLAT("shape editor"), 'g');
  dialog::add_action([] {
    mapeditor::drawing_tool = false;
    mapeditor::intexture = false;
    pushScreen(mapeditor::showDrawEditor);
    mapeditor::initdraw(cwt.at);
    });
#endif

#if CAP_EDIT
  dialog::addItem(XLAT("drawing tool"), 'd');
  dialog::add_action([] {
    dialog::cheat_if_confirmed([] {
      mapeditor::drawing_tool = true;
      mapeditor::intexture = false;
      pushScreen(mapeditor::showDrawEditor);
      mapeditor::initdraw(cwt.at);
      });
    });
#endif

  // display modes  
#if CAP_MODEL
  if(GDIM == 2) {
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

  dialog::addBoolItem(XLAT("history mode"), history::on || history::includeHistory, 'h');
  dialog::add_action_push(history::history_menu);

#if CAP_TEXTURE
  if(GDIM == 2) {
    dialog::addBoolItem(XLAT("texture mode"), texture::config.tstate == texture::tsActive, 't');
    dialog::add_action_push(texture::showMenu);
    }
#endif

//  dialog::addBoolItem(XLAT("expansion"), viewdists, 'x');
  
  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  }

EX void show_chaos() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("land structure"));
  chaosUnlocked = chaosUnlocked || autocheat;

  dialog::addHelp(
    "In the Chaos mode, lands change very often, and "
    "there are no walls between them. "
    "Some lands are incompatible with this."
    "\n\nYou need to reach Crossroads IV to unlock the Chaos mode."
    );
  
  dialog::addBreak(100);
  
  char key = 'a';
  for(int i=0; i<lsGUARD; i++) {
    dynamicval<eLandStructure> dls(land_structure);
    auto li = eLandStructure(i);
    land_structure = li;
    fix_land_structure_choice();
    if(ls::any_chaos() && !chaosUnlocked) continue;
    if(li == lsNoWalls && geometry == gNormal && !chaosUnlocked) continue;
    if(land_structure == i) {
      dialog::addBoolItem(land_structure_name(false), land_structure == dls.backup, key + i);
      dialog::add_action(dual::mayboth([li] {
        dialog::do_if_confirmed([li] {
          stop_game();
          land_structure = li;
          start_game();
          });
        }));
      }
    }
  
  dialog::addBreak(100);
  dialog::addSelItem(XLAT("land"), XLAT1(linf[specialland].name), 'l');
  dialog::add_action(activate_ge_land_selection);

  dialog::addBreak(100);
  if(ineligible_starting_land)
    dialog::addInfo("this starting land is not eligible for achievements");
  else if(land_structure == lsNiceWalls)
    dialog::addInfo("eligible for most achievements");
  else if(land_structure == lsChaos)
    dialog::addInfo("eligible for Chaos mode achievements");
  else if(land_structure == lsSingle)
    dialog::addInfo("eligible for special achievements");
  else
    dialog::addInfo("not eligible for achievements");
  if(cheater) dialog::addInfo("(but the cheat mode is on)");
  if(casual) dialog::addInfo("(but the casual mode is on)");

  dialog::addBreak(100);
  dialog::addBack();
  dialog::display();
  }

EX void mode_higlights() {
  cmode = sm::NOSCR;
  gamescreen();
  dialog::init(XLAT("highlights & achievements"));
  
  dialog::addBigItem(XLATN("Space Rocks"), 'r');
  dialog::add_action(dialog::add_confirmation([] {
    popScreenAll();
    stop_game();
    resetModes();
    specialland = laAsteroids;
    set_geometry(gKleinQuartic);
    set_variation(eVariation::bitruncated);
    land_structure = lsSingle;
    shmup::on = true;
    start_game();
    }));
  dialog::addInfo(XLAT("classic game except hyperbolic"));
  dialog::extend();

  dialog::addBigItem(XLATN("hyperbolic Minesweeper"), 'e');
  dialog::add_action([] { 
    dialog::do_if_confirmed([] {
      resetModes();
      specialland = firstland = laMinefield;
      if(!closed_or_bounded) {
        set_geometry(gBring);
        gp::param = gp::loc(2, 1);
        set_variation(eVariation::goldberg);
        mine_adjacency_rule = true;
        bounded_mine_percentage = .2;
        }
      start_game();
      popScreenAll();
      });
    });
  dialog::addInfo(XLAT("yet another classic game"));
  dialog::extend();

  #if CAP_RACING && MAXMDIM >= 4
  dialog::addBigItem(XLAT("Racing in Thurston geometries"), 't'-96);
  dialog::add_action(dialog::add_confirmation(racing::start_thurston));
  dialog::addInfo(XLAT("race through a maze in exotic 3D geometry!"));
  dialog::extend();
  #endif

  dialog::addBigItem(XLAT1("Halloween"), 'Z');
  dialog::add_action(dialog::add_confirmation(halloween::start_all));
  dialog::addInfo(XLAT("Halloween mini-game"));
  dialog::extend();

  dialog::addBigItem(XLAT1("heptagonal mode"), 'H');
  dialog::add_action(dialog::add_confirmation([] {
    popScreenAll();
    resetModes('7');
    land_structure = lsNiceWalls;
    start_game();
    clearMessages();
    welcomeMessage();
    }));
  dialog::addInfo(XLAT("can you find the Heptagonal Grail?"));
  dialog::extend();

  dialog::addBreak(100);
  dialog::addBigItem(XLAT1("other achievements:"), 0);
  dialog::addItem(XLAT("General Euclid"), 'e');
  dialog::add_action(dialog::add_confirmation([] {
    popScreenAll();
    resetModes();
    set_geometry(gEuclid);
    firstland = specialland = laMirrorOld;
    land_structure = lsSingle;
    start_game();
    clearMessages();
    welcomeMessage();
    }));

  dialog::addItem(XLAT("Worm of the World"), 'w');
  dialog::add_action(dialog::add_confirmation([] {
    popScreenAll();
    resetModes();
    set_geometry(gZebraQuotient);
    firstland = specialland = laDesert;
    land_structure = lsSingle;
    start_game();
    clearMessages();
    welcomeMessage();
    }));

  dialog::addItem(XLAT("Lovász Conjecture"), 'L');
  dialog::add_action(dialog::add_confirmation([] {
    popScreenAll();
    resetModes();
    set_geometry(gKleinQuartic);
    gp::param = gp::loc(1, 1);
    set_variation(eVariation::untruncated);
    
    firstland = specialland = laMotion;
    land_structure = lsSingle;
    start_game();
    clearMessages();
    welcomeMessage();
    }));

  #if CAP_CRYSTAL
  if(hiitemsMax(itHolyGrail) || cheater || autocheat) {
    dialog::addItem(XLAT("Knight of the 16-Cell Table"), '1');
    dialog::add_action(dialog::add_confirmation([] {
      popScreenAll();
      resetModes();
      crystal::set_crystal(8);
      firstland = specialland = laCamelot;
      land_structure = lsSingle;
      start_game();
      clearMessages();
      welcomeMessage();
      pushScreen(crystal::crystal_knight_help);
      }));
  
    dialog::addItem(XLAT("Knight of the 3-Spherical Table"), '3');
    dialog::add_action(dialog::add_confirmation([] {
      popScreenAll();
      resetModes();
      crystal::set_crystal(8);
      set_variation(eVariation::bitruncated);
      set_variation(eVariation::bitruncated);
      land_structure = lsSingle;
      firstland = specialland = laCamelot;
      start_game();
      clearMessages();
      welcomeMessage();
      pushScreen(crystal::crystal_knight_help);
      }));
    }
  else {
    dialog::addItem("(locked until you find a Holy Grail)", 0);
    }
  #endif

  #if MAXMDIM >= 4
  dialog::addBreak(100);
  dialog::addBigItem(XLAT1("some cool visualizations"), 0);
  dialog::addItem(XLAT("Emerald Mine in {5,3,4}"), '5');
  dialog::add_action(dialog::add_confirmation([] {
    popScreenAll();
    resetModes();
    specialland = laEmerald;
    land_structure = lsSingle;
    set_geometry(gSpace534);
    check_cgi();
    cgi.require_basics();
    cgi.require_shapes();
    fieldpattern::field_from_current();
    set_geometry(gFieldQuotient);
    int p = 2;
    for(;; p++) { currfp.Prime = p; currfp.force_hash = 0x72414D0C; if(!currfp.solve()) break; }
    start_game();
    clearMessages();
    welcomeMessage();
    }));
  #endif

  dialog::addBreak(100);
  dialog::addBack();
  dialog::display();
  }  

EX eLandStructure default_land_structure() {
  if(closed_or_bounded) return lsSingle;
  if(tactic::on || princess::challenge) return lsSingle;
  if(yendor::on) return yendor::get_land_structure();
  if(specialland == laCanvas) return lsSingle;
  if(nice_walls_available()) return lsNiceWalls;
  return lsNoWalls;
  }

EX void menuitem_land_structure(char key) {

  if(default_land_structure() == land_structure && !ineligible_starting_land)
    dialog::addBoolItem(XLAT("land structure"), false, key);
  else
    dialog::addSelItem(XLAT("land structure"), land_structure_name(true), key);
  dialog::add_action_push(show_chaos);
  }
  
EX void showChangeMode() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("special modes"));

  dialog::addBoolItem(XLAT("experiment with geometry"), geometry || CHANGED_VARIATION || viewdists, 'e');
  dialog::add_action(runGeometryExperiments);

  dialog::addBoolItem(XLAT(SHMUPTITLE), shmup::on, 's');
  dialog::add_action_confirmed(shmup::switch_shmup);

  dialog::addBoolItem(XLAT("multiplayer"), multi::players > 1, 'm');
  dialog::add_action_push(multi::showConfigureMultiplayer);

  #if CAP_SAVE
  dialog::addSelItem(XLAT("casual mode"), ONOFF(casual), 'C');
  dialog::add_action(switch_casual);
  #endif
  
  if(!shmup::on) {
    dialog::addSelItem(XLAT("hardcore mode"),
    hardcore && !pureHardcore() ? XLAT("PARTIAL") : ONOFF(hardcore), 'h');
    dialog::add_action(switchHardcore);
    }
  if(getcstat == 'h')
    mouseovers = XLAT("One wrong move and it is game over!");
  
  multi::cpid = 0;
  menuitem_land_structure('l');

  dialog::addBoolItem(XLAT("puzzle/exploration mode"), peace::on, 'p');
  dialog::add_action_push(peace::showMenu);

  dialog::addBoolItem(XLAT("Orb Strategy mode"), (inv::on), 'i');
  dialog::add_action_confirmed([] { restart_game(rg::inv); });

  dialog::addBoolItem(XLAT("random pattern mode"), (randomPatternsMode), 'r');
  dialog::add_action_confirmed([] { 
    stop_game();
    firstland = laIce;
    restart_game(rg::randpattern); 
    });

#if CAP_ARCM && !ISWEB
  if(multi::players == 1) {
    dialog::addBoolItem(XLAT("dual geometry mode"), dual::state, 'D');
    dialog::add_action_confirmed([] { restart_game(rg::dualmode); });
    }
#endif

  dialog::addBoolItem(XLAT("cheat mode"), (cheater), 'c');
  dialog::add_action(enable_cheat);

  dialog::addBreak(50);
#if CAP_TOUR
  dialog::addBoolItem(XLAT("guided tour"), tour::on, 'T');
  dialog::add_action_confirmed(tour::start);
#endif
#if CAP_DAILY
  dialog::addBoolItem(XLAT("Strange Challenge"), daily::on, 'z');
  dialog::add_action_push(daily::showMenu);    
#endif
  dialog::addBoolItem(XLAT("%1 Challenge", moPrincess), (princess::challenge), 'P');
  dialog::add_action_confirmed([] {
    if(!princess::everSaved)
      addMessage(XLAT("Save %the1 first to unlock this challenge!", moPrincess));
    else restart_game(rg::princess);
    });  
  dialog::addBoolItem(XLAT("pure tactics mode"), (tactic::on), 't');
  dialog::add_action(tactic::start);

  dialog::addBoolItem(XLAT("Yendor Challenge"), (yendor::on), 'y');
  dialog::add_action([] {
    clearMessages();
    if(yendor::everwon || autocheat)
      pushScreen(yendor::showMenu);
    else gotoHelp(yendor::chelp);
    });
#if CAP_RACING
  dialog::addBoolItem(XLAT("racing mode"), racing::on, 'R');
  dialog::add_action(racing::configure_race);
#endif  

  dialog::addBreak(50);

  dialog::addItem(XLAT("highlights & achievements"), 'h');
  dialog::add_action_push(mode_higlights);
  
  dialog::addBack();
  dialog::display();  
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

void announce_random() {
  dialog::addBreak(100);
  dialog::addTitle("(random option)", 0x808080, 50);
  }

void announce_nothing() {
  dialog::addBreak(100);
  dialog::addTitle("", 0x808080, 50);
  }

void announce_seasonal() {
  dialog::addBreak(100);
  dialog::addTitle("(seasonal option)", 0x808080, 50);
  }

EX void showStartMenu() {
  if(!daily_mode) {
    daily_mode = hrand(10) + 1;
    if(showHalloween())
      daily_mode = 20;
    }
  
  getcstat = ' ';
  
  #if CAP_STARTANIM
  cmode = sm::DARKEN;
  startanims::display();
  #endif

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
  dialog::addBigItem(XLAT("guided tour"), 't');
  dialog::addInfo(XLAT("learn about hyperbolic geometry!"));
#endif

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

  switch(daily_mode) { 
    case 1:
      #if CAP_SHMUP_GOOD
        announce_random();
        dialog::addBigItem(XLAT("shoot'em up mode"), 's');
        dialog::addInfo(XLAT("continuous spacetime"));
      #if CAP_ACHIEVE
        dialog::addInfo(XLAT("(most achievements are not available)"));
      #endif
      #endif
      break;
    
    case 2: 
      announce_random();
      dialog::addBigItem(XLAT("heptagonal mode"), '7');
      dialog::addInfo(XLAT("more curvature"));
      dialog::addInfo(XLAT("(most achievements are not available)"));
      break;      
    
    case 3: 
      announce_random();
      dialog::addBigItem(XLAT("experiment with geometry"), 'g');
      dialog::addInfo(XLAT("(most achievements are not available)"));
      break;      
    
    case 4: 
      if(chaosUnlocked) {
        announce_random();
        dialog::addBigItem(XLAT("Chaos mode"), 'C');
        dialog::addInfo(XLAT("(most achievements are not available)"));
        }
      break;      

#if CAP_RUG    
    case 5:
      announce_random();
      dialog::addBigItem(XLAT("hypersian rug mode"), 'M');
      dialog::addInfo(XLAT("see the true form"));
      break;
#endif

#if CAP_TEXTURE && CAP_EDIT
    case 6:
      announce_random();
      dialog::addBigItem(XLAT("texture mode"), 'T');
      dialog::addInfo(XLAT("paint pictures"));
      break;
#endif

#if CAP_DAILY
    case 7:
      announce_random();
      dialog::addBigItem(XLAT("Strange Challenge"), 'z');
      dialog::addInfo(XLAT("compete with other players on random lands in random geometries"));
      break;
#endif

#if CAP_RACING
    case 8:
      announce_random();
      dialog::addBigItem(XLAT("Racing"), 'r'-96);
      dialog::addInfo(XLAT("how fast can you reach the end?"));
      break;

    case 9:
      announce_random();
      dialog::addBigItem(XLAT("Racing in Thurston geometries"), 't'-96);
      dialog::addInfo(XLAT("race through a maze in exotic 3D geometry!"));
      break;
#endif

    case 20:
      announce_seasonal();
      dialog::addBigItem(XLAT1("Halloween"), 'Z');
      dialog::addInfo(XLAT("Halloween mini-game"));
      break;

    default:
      announce_nothing();
      dialog::addBigItem("", 0);
      dialog::addInfo("");
      break;    
    }

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
#if CAP_STARTANIM
    else if(uni == startanims::EXPLORE_START_ANIMATION)
      startanims::explore();
#endif
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
      enable_canvas();
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
      progress_warning();
      stampbase = ticks;
      if(uni == 's') 
        multi::configure();
      }
    else if(uni == 'Z') 
      halloween::start_all();
#if CAP_RACING && MAXMDIM >= 4
    else if(uni == 'r' - 96) {
      popScreenAll();
      resetModes();
      stop_game();
      switch_game_mode(rg::racing);
      specialland = racing::race_lands[rand() % isize(racing::race_lands)];
      start_game();
      pmodel = mdBand;
      pconf.model_orientation = racing::race_angle;
      racing::race_advance = 1;
      vid.yshift = 0;
      pconf.camera_angle = 0;
      pconf.xposition = 0;
      pconf.yposition = 0;
      pconf.scale = 1;
      vid.use_smart_range = 1;
      vid.smart_range_detail = 3;
      }
    else if(uni == 't' - 96) 
      racing::start_thurston();
#endif
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
      pushScreen(showGameMenu);
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
      progress_warning();
      }
    else if(sym == SDLK_F5) { 
      #if CAP_STARTANIM
      startanims::pick(); 
      #endif
      daily_mode = 0; 
      }
    };
  }
 
// -- overview --

#if HDR
struct named_functionality {
    std::string first;
    reaction_t second;
    explicit named_functionality() = default;
    explicit named_functionality(std::string s, reaction_t r) : first(std::move(s)), second(std::move(r)) {}
    friend bool operator==(const named_functionality& a, const named_functionality& b) { return a.first == b.first; }
    friend bool operator!=(const named_functionality& a, const named_functionality& b) { return a.first != b.first; }
};
inline named_functionality named_dialog(string x, reaction_t dialog) { return named_functionality(x, [dialog] () { pushScreen(dialog); }); }
#endif

#if HDR
using o_funcs = vector<named_functionality>;
#endif

EX hookset<void(o_funcs&)> hooks_o_key;

EX named_functionality get_o_key() {
  vector<named_functionality> res;
  callhooks(hooks_o_key, res);

  if(in_full_game() && !yendor::on)
    res.push_back(named_dialog(XLAT("world overview"), showOverview));
  
#if CAP_DAILY
  if(daily::on) 
    res.push_back(named_functionality(XLAT("Strange Challenge"), [] () {
      achievement_final(false);
      pushScreen(daily::showMenu);
      }));
#endif

  if(viewdists)
    res.push_back(named_functionality(XLAT("experiment with geometry"), runGeometryExperiments));

  if(tactic::on)
    res.push_back(named_dialog(XLAT("pure tactics mode"), tactic::showMenu));
    
  if(yendor::on)
    res.push_back(named_dialog(XLAT("Yendor Challenge"), yendor::showMenu));

  if(peace::on)
    res.push_back(named_dialog(XLAT("puzzles and exploration"), peace::showMenu));
  
  #if CAP_TEXTURE
  if(texture::config.tstate)
    res.push_back(named_dialog(XLAT("texture mode"), texture::showMenu));
  #endif

  dialog::infix = "";

  if((geometry != gNormal || NONSTDVAR || disksize) && !daily::on)
    res.push_back(named_functionality(XLAT("experiment with geometry"), runGeometryExperiments));
  
  if(res.empty()) return named_dialog(XLAT("world overview"), showOverview);
  
  if(isize(res) == 1) return res[0];
  
  return named_dialog(res[0].first + "/...", [res] {
    emptyscreen();
    dialog::init();
    char id = 'o';
    for(auto& r: res) {
      dialog::addItem(r.first, id++);
      dialog::add_action([r] { popScreen(); r.second(); });
      }
    dialog::display();
    });
  }

EX int messagelogpos;
EX int timeformat;
EX int stampbase;

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
    PHASEFROM(2); launch_dialog(showGameMenu);
    }
  else if(argis("-d:mode")) {
    PHASEFROM(2); launch_dialog(showChangeMode);
    }
  else if(argis("-d:history")) {
    PHASEFROM(2); launch_dialog(history::history_menu);
    }
  else if(argis("-d:shmup")) {
    PHASEFROM(2); launch_dialog(); multi::configure();
    }
  else return 1;
  return 0;
  }

auto ah_menu = addHook(hooks_args, 0, read_menu_args);

#endif

}
