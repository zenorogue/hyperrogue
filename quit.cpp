// Hyperbolic Rogue -- the mission screen
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file quit.cpp
 *  \brief the mission screen, and routines related to it
 */

#include "hyper.h"
namespace hr {

EX bool quitsaves() { 
  if(casual) return false;
  return (items[itOrbSafety] && CAP_SAVE && !arcm::in()); 
  }

EX bool needConfirmationEvenIfSaved() {
  return canmove && (gold() >= 30 || tkills() >= 50) && !cheater;
  }

EX bool needConfirmation() {
  if(casual) return needConfirmationEvenIfSaved() && turncount > save_turns + 10;
  else return needConfirmationEvenIfSaved() && !quitsaves();
  }

EX int getgametime() {
  return (int) (savetime + (timerstopped ? 0 : (time(NULL) - timerstart)));
  }

EX string getgametime_s(int timespent IS(getgametime())) {
  char buf[20];
  sprintf(buf, "%d:%02d", timespent/60, timespent % 60);
  return buf;
  }

EX bool display_yasc_codes;

string timeline() {
  string s;
  if(shmup::on)
    s = XLAT("%1 knives (%2)", its(turncount), getgametime_s());
  else {
    s = XLAT("%1 turns (%2)", its(turncount), getgametime_s());
    if(display_yasc_codes)
      s+= XLAT(" YASC code: ") + its(yasc_code);
    }
  return s;
  }

EX void noaction() {}

EX function<void()> cancel = noaction;

#if HDR
struct hint {
  time_t last;
  function<bool()> usable;
  function<void()> display;
  function<void()> action;  
  };
#endif

EX hint hints[] = {

  {
    0,
    []() { 
      return !inv::on && items[localTreasureType()] >= 18 && in_full_game();
      },    
    []() { 
       dialog::addHelp(XLAT(
        "If you collect too many treasures in a given land, it will become "
        "extremely dangerous. Try other lands once you have enough!"));
      },
    noaction},

  {
    0,
    []() { 
      return !ISMOBILE;
      },    
    []() { 
       dialog::addHelp(XLAT(
        "Remember that you can right click almost anything for more information."));
#if ISMAC
       dialog::addHelp(XLAT(
         "(You can also use right Shift)\n\n"));
#endif
      },
    noaction},

  {
    0,
    []() { return !canmove; },
    []() { 
      dialog::addHelp(XLAT(
        "Want to understand the geometry in HyperRogue? Try the Guided Tour!"
        ));
      dialog::addBreak(50);
      dialog::addItem(XLAT("guided tour"), 'z');
      },
    []() {
#if CAP_TOUR    
      tour::start();
#else
      addMessage("Not in this version");
#endif
      }},

  {
    0,
    []() { return !inv::on && in_full_game(); },
    []() { 
      dialog::addHelp(XLAT(
        "Collecting 25 treasures in a given land may be dangerous, "
        "but allows magical Orbs of this land to appear in other places!"
        ));
      },
    noaction},

  {
    0,
    []() { return !canmove; },
    []() { 
      dialog::addInfo(XLAT(
        "Press ESC to view this screen during the game."
        ));
      },
    noaction
    },
  {
    0,
    []() { return in_full_game(); },
    []() { 
      dialog::addInfo(
#if ISMOBILE
        XLAT("The 'world overview' shows all the lands in HyperRogue.")
#else
        XLAT("Press 'o' to see all the lands in HyperRogue.")
#endif
        );
      dialog::addBreak(50);
      dialog::addItem(XLAT("world overview") + " ", 'z');
      },
    []() {
      pushScreen(showOverview);
      }},
  {
    0,
    []() { return !canmove; },
    []() { 
      dialog::addHelp(XLAT(
        "Want another type of game? Want more challenge?\n"
        "HyperRogue has many special modes and challenges that "
        "significantly change the gameplay. Try them!"
        ));
      dialog::addBreak(50);
      dialog::addItem(XLAT("special game modes"), 'z');
      },
    []() {
      pushScreen(showChangeMode);
      }},

  {
    0,
    []() { return true; },
    []() { 
      dialog::addInfo(XLAT(
        "Hyperbolic geometry can be shown in many ways."
        ));
      dialog::addBreak(50);
      dialog::addItem(XLAT("special display modes"), 'z');
      },
    []() {
      pushScreen(models::model_menu);
      }},

  {
    0,
    []() { return !canmove && !inv::on; },
    []() { 
      dialog::addHelp(XLAT(
        "You do not want to lose the game from a single mistake?\n"
        "Do you want to use the Orbs strategically?\n"
        "Try the Orb Strategy mode!")
        );
      dialog::addBreak(50);
      dialog::addItem(XLAT("Orb Strategy mode"), 'z');
      },
    []() {
#if CAP_INV
      restart_game(rg::inv);
#endif
      }
    },
  {
    0,
    []() { return CAP_RUG && geometry == gNormal; },
    []() { 
      dialog::addHelp(XLAT(
        "Do you think you are playing on a ball? "
        "This is far from the truth!\n"
        ));
      dialog::addBreak(50);
      dialog::addItem(XLAT("hypersian rug mode"), 'z');
      },
    [] () {
#if CAP_RUG
      popScreen();
      int wm, mm;
      rug::init();
      wm = vid.wallmode;
      mm = vid.monmode;
      vid.wallmode = 3;
      vid.monmode = 2;
      cancel = [wm, mm] () { 
        rug::close();
        vid.wallmode = wm;
        vid.monmode = mm;
        };
#endif
      }
    },

  {
    0,
    []() { return !canmove && geometry == gNormal && celldist(cwt.at) >= 50; },
    []() { 
      dialog::addHelp(XLAT(
        "Did you know that the path you take during the game "
        "is usually very close to a straight line?\n"
        ));
      dialog::addBreak(50);
      dialog::addItem(XLAT("Show me!"), 'z');
      },
    [] () {
      popScreen();
      auto m = pmodel;
      pmodel = mdBand;
      int r = models::rotation;
      bool h = history::includeHistory;
      models::rotation = 0;
      history::includeHistory = true;
      history::create_playerpath();
      cancel = [m,r,h] () { 
        history::clear(); pmodel = m; 
        models::rotation = r;
        history::includeHistory = h;
        fullcenter(); };
      }
    },

  {
    0,
    []() { return !canmove && sizes_known() && celldist(cwt.at) >= 50; },
    []() {
      int d = celldist(cwt.at);
      string s = get_expansion().approximate_descendants(d, 10000);
      dialog::addHelp(XLAT(
        "You are %1 cells away from the starting point, or "
        "the place where you used an Orb of Safety last time. "
        "There are %2 such cells.\n",
        its(d), s
        ));
      dialog::addBreak(50);
      dialog::addItem(XLAT("expansion"), 'z');
      },
    [] () {
      viewdists = !viewdists;
      popScreen();
      cancel = [] () { viewdists = false; };
      }
    },

  {
    0,
    []() { return !canmove && showHalloween(); },
    []() {
      dialog::addItem(XLAT("Halloween mini-game"), 'z');
      },
    [] () {
      if(!sphere) {
        resetModes();
        stop_game();
        specialland = laHalloween;
        set_geometry(gSphere);
        start_game();
        pconf.alpha = 999;
        pconf.scale = 998;
        }
      else {
        resetModes();
        pconf.alpha = 1;
        pconf.scale = 1;
        }
      }
    },

  {-1, []() { return false; }, noaction, noaction}
  }; 

EX int hinttoshow;

string contstr() {
  return canmove ? XLAT("continue") : XLAT("see how it ended");
  }

eLand nextHyperstone() {
  generateLandList(isLandIngame);
  for(eLand l: landlist)
    if(items[treasureType(l)] < R10 && !isCrossroads(l) && l != laPrincessQuest && l != laCamelot)
      return l;
  if(items[itHyperstone] >= 10) return laNone;
  return laCrossroads;
  }

EX void showGameMenu() {

  cmode = sm::DOTOUR | sm::MISSION | sm::CENTER | sm::MAYDARK | sm::SIDE;
  gamescreen(); drawStats();
  getcstat = SDLK_ESCAPE;

  dialog::init(
#if CAP_TOUR
    tour::on ? (canmove ? XLAT("guided tour") : XLAT("GAME OVER")) :
#endif
    (cheater && !autocheat)? XLAT("It is a shame to cheat!") : 
    racing::on ? "racing mode" :
    (canmove && princess::challenge) ? XLAT("%1 Challenge", moPrincess) :
    canmove ? XLAT("Quest status") : 
    XLAT("GAME OVER"), 
    0xC00000, 200, 100
    );

  #if CAP_COMPLEX2
  bool sweeper = mine::in_minesweeper();
  #else
  const bool sweeper = false;
  #endif

  if(!peace::on && !racing::on && !sweeper && !in_lovasz())
    dialog::addInfo(XLAT("Your score: %1", its(gold())));
  if(!peace::on && !racing::on && !sweeper && !in_lovasz())
    dialog::addInfo(XLAT("Enemies killed: %1", its(tkills())));

#if CAP_TOUR
  if(tour::on) ; else 
#endif
  if(items[itOrbYendor]) {
    dialog::addInfo(XLAT("Orbs of Yendor found: %1", its(items[itOrbYendor])), iinf[itOrbYendor].color);
    dialog::addInfo(XLAT("CONGRATULATIONS!"), iinf[itOrbYendor].color);
    }
  #if CAP_COMPLEX2
  else if(mine::in_minesweeper()) {
    int to_uncover = kills[moBomberbird];
    if(to_uncover) {
      dialog::addInfo(XLAT("Uncover all cells which do not contain mines"));
      dialog::addInfo(XLAT("Cells to uncover: %1", its(to_uncover)));
      }
    else {
      dialog::addInfo(XLAT("CONGRATULATIONS!"), iinf[itOrbYendor].color);
      dialog::addInfo(XLAT("You won in %1", getgametime_s(mine::victory_time)));      
      }
    }
  #endif
  else if(in_lovasz()) {
    int score = 0, all = 0;
    for(cell *c: currentmap->allcells()) {
      if(c->wall == waChasm || c->item == itOrbInvis)
        score++;
      all++;
      }
    dialog::addInfo(XLAT("Dropped floors: %1/%2", its(score), its(all)));
    if(score == all) dialog::addInfo(XLAT("CONGRATULATIONS!"), iinf[itOrbYendor].color);
    if(score == all && geometry == gKleinQuartic && variation == eVariation::untruncated && gp::param == gp::loc(1,1) && !disksize)
      achievement_gain_once("LOVASZ", rg::special_geometry);      
    }
  else {  
    if(0)
      ;
#if CAP_TOUR
    else if(tour::on)
      ;
#endif
    else if(racing::on) ;
    else if(princess::challenge) 
      dialog::addInfo(XLAT("Follow the Mouse and escape with %the1!", moPrincess));
    else if(!in_full_game()) ;
    else if(casual && savecount == 0) {
      dialog::addInfo(XLAT("Find an Orb of Safety to save your game"));
      }
    else if(gold() < R30)
      dialog::addInfo(XLAT("Collect %1 $$$ to access more worlds", its(R30)));
    else if(gold() < R60)
      dialog::addInfo(XLAT("Collect %1 $$$ to access even more lands", its(R60)));
    else if(!landUnlocked(laHell))
      dialog::addInfo(XLAT("Collect at least %1 treasures in each of %2 types to access Hell", its(R10), its(lands_for_hell())));
    else if(items[itHell] < R10)
      dialog::addInfo(XLAT("Collect at least %1 Demon Daisies to find the Orbs of Yendor", its(R10)));
    else if(isize(yendor::yi) == 0)
      dialog::addInfo(XLAT("Look for the Orbs of Yendor in Hell or in the Crossroads!"));
    else 
      dialog::addInfo(XLAT("Unlock the Orb of Yendor!"));
    }
  
  if(!timerstopped && !canmove) {
    savetime += time(NULL) - timerstart;
    timerstopped = true;
    }
  if(canmove && !timerstart)
    timerstart = time(NULL);
  
  if(princess::challenge) ;
#if CAP_TOUR
  else if(tour::on) ;
#endif
  else if(peace::on) ;
  else if(racing::on) ;
  else if(!in_full_game()) ;
  else if(tkills() < R100)
    dialog::addInfo(XLAT("Defeat %1 enemies to access the Graveyard", its(R100)));
  else if(kills[moVizier] == 0 && (items[itFernFlower] < U5 || items[itGold] < U5))
    dialog::addInfo(XLAT("Kill a Vizier in the Palace to access Emerald Mine"));
  else if(items[itEmerald] < U5)
    dialog::addInfo(XLAT("Collect 5 Emeralds to access Camelot"));
  else if(landUnlocked(laHell) && ls::any_order()) {
    eLand l = nextHyperstone();
    if(l) 
        dialog::addInfo(          
            l ? XLAT("Hyperstone Quest: collect at least %3 points in %the2", treasureType(l), l, its(R10))
              : XLAT("Hyperstone Quest: collect at least %3 %1 in %the2", treasureType(l), l, its(R10))
            );
    else
      dialog::addInfo(XLAT("Hyperstone Quest completed!"), iinf[itHyperstone].color);
    }
  else dialog::addInfo(XLAT("Some lands unlock at specific treasures or kills"));
  if(cheater && !autocheat) {
    dialog::addInfo(XLAT("you have cheated %1 times", its(cheater)), 0xFF2020);
    }
  else if(!racing::on) {
    dialog::addInfo(timeline(), dialog::dialogcolor);
    }
  
  dialog::addBreak(100);

#if CAP_TOUR  
  if(!tour::on) {
    hints[hinttoshow].display();
    dialog::addBreak(100);
    }
#endif
  
  bool intour = false;
  
#if CAP_TOUR
  intour = tour::on;
#endif

  if(intour) {
#if CAP_TOUR
    if(canmove) {
      if(sphere) {
        dialog::addItem(XLAT("return to your game"), '1');
        dialog::addItem(pconf.alpha < 2 ? XLAT("orthogonal projection") : XLAT("stereographic projection"), '3');
        }
      else if(euclid) {
        dialog::addItem(XLAT("return to your game"), '2');
        dialog::addBreak(100);
        }
      else {       
        dialog::addItem(XLAT("spherical geometry"), '1');
        dialog::addItem(XLAT("Euclidean geometry"), '2');
        }
      // dialog::addItem(XLAT("more curved hyperbolic geometry"), '3');
      }
    if(!items[itOrbTeleport])
      dialog::addItem(XLAT("teleport away"), '4');
    else if(!items[itOrbAether])
      dialog::addItem(XLAT("move through walls"), '4');
    else
      dialog::addItem(XLAT("flash"), '4');
    if(canmove) {
      if(tour::slidecommand != "") 
        dialog::addItem(tour::slidecommand, '5');
      dialog::addItem(XLAT("static mode"), '6');
      dialog::addItem(XLAT("enable/disable texts"), '7');
      dialog::addItem(XLAT("next slide"), SDLK_RETURN);
      dialog::addItem(XLAT("previous slide"), SDLK_BACKSPACE);
      dialog::addItem(XLAT("list of slides"), '9');
      dialog::addItem(XLAT("leave the tour mode"), '0');
      }
    else
      dialog::addBreak(200);
    dialog::addItem("continue", SDLK_ESCAPE);
#endif
    }
  else {
    dialog::addItem(contstr(), SDLK_ESCAPE);
    dialog::addItem(get_o_key().first, 'o');
    dialog::add_action([] {
      clearMessages();
      get_o_key().second();
      });
    #if CAP_INV
    if(inv::on && items[itInventory]) {
      dialog::addItem(XLAT("inventory"), 'i');
      dialog::add_action([] {
        clearMessages();
        pushScreen(inv::show);
        });
      }
    #endif
    if(casual || ISMOBILE) {
      if(casual && savecount)
        dialog::addItem(XLAT("load (%1 turns passed)", its(turncount - save_turns)), SDLK_F9);
      else
        dialog::addItem(XLAT("how to find an Orb of Safety?"), SDLK_F9);
      }
#if CAP_ANDROIDSHARE
    dialog::addItem(XLAT("SHARE"), 's'-96);
#endif
    }
  dialog::addItem(XLAT("message log"), 'l');
  
  if(cheater) {
    dialog::addItem(XLAT("cheats"), 'C');
    dialog::add_action_push(showCheatMenu);
    }
  dialog::addItem(XLAT("settings"), 's');
  dialog::add_action_push(showSettings);
  dialog::addItem(XLAT("creative mode"), 'c');
  dialog::add_action_push(showCreative);
  dialog::addItem(XLAT("special modes"), 'm');
  dialog::add_action_push(showChangeMode);
#if CAP_SAVE
  dialog::addItem(XLAT("local highscores"), 't');
  dialog::add_action([] { scores::load(); });
#endif
  #if ISMOBILE
  dialog::addItem(XLAT("visit the website"), 'q');
  dialog::add_action([] {
    extern void openURL();
    openURL();
    });
  #endif
#if ISMOBILE
#if CAP_ACHIEVE
  dialog::addItem(XLAT("leaderboards/achievements"), '3'); 
  dialog::add_action([] {
    achievement_final(false);
    pushScreen(leader::showMenu);
    });
#endif
#endif
  dialog::addHelp();
  dialog::add_action([] { buildHelpText(); gotoHelp(help); });
  dialog::addItem(XLAT("restart"), SDLK_F5);
  dialog::addItem(inSpecialMode() ? XLAT("reset special modes") : XLAT("back to the start menu"), 'R');
  dialog::add_action([] {
    dialog::do_if_confirmed([] {
      #if CAP_STARTANIM
      startanims::pick();
      #endif
      popScreenAll(), pushScreen(showStartMenu);
      });
    });
#if !ISMOBILE
  dialog::addItem(quitsaves() ? XLAT("save") : XLAT("quit"), SDLK_F10);
#endif
  
  keyhandler = handleKeyQuit;
  dialog::display();
  }

EX string safety_help() {
  return XLAT(
    "To save the game you need an Orb of Safety.\n\n"
    "Orbs of Safety appear:\n\n"
    "* in the Crossroads and the Land of Eternal Motion, after you collect %1 Phoenix Feathers in the Land of Eternal Motion\n\n"
    "* in the Ocean after you unlock it (%2 treasures)\n\n"
    "* in the Prairie after you unlock it (%3 treasures)\n\n",
    its(inv::on ? 25 : 10),
    its(R30), its(R90)
    );
  }
   

EX void handleKeyQuit(int sym, int uni) {
  dialog::handleNavigation(sym, uni);
  // ignore the camera movement keys

#if CAP_RUG
  if(rug::rugged && (sym == SDLK_UP || sym == SDLK_DOWN || sym == SDLK_PAGEUP || sym == SDLK_PAGEDOWN ||
    sym == SDLK_RIGHT || sym == SDLK_LEFT))
    sym = 0;
#endif

  if(sym == SDLK_RETURN || sym == SDLK_KP_ENTER || sym == SDLK_F10) {
    if(needConfirmation()) pushScreen([] { 
      dialog::confirm_dialog(
        XLAT("This will exit HyperRogue without saving your current game. Are you sure?") + "\n\n" +
        safety_help(),
        [] { 
        quitmainloop = true; 
        });
      });
    else quitmainloop = true;
    }
  else if(uni == 'r' || sym == SDLK_F5) dialog::do_if_confirmed([] {
    restart_game(rg::nothing);
    msgs.clear();
    });
  else if(uni == 'l') popScreenAll(), pushScreen(showMessageLog), messagelogpos = isize(gamelog);
  else if(uni == 'z') hints[hinttoshow].action();
  #if CAP_SAVE
  else if(sym == SDLK_F9) {
    if(casual && savecount) {
      stop_game();
      load_last_save();
      start_game();
      }
    else
      gotoHelp(safety_help());
    }
  #endif
  else if(sym == SDLK_F3 || (sym == ' ' || sym == SDLK_HOME)) 
    fullcenter();
  else if(uni == 'o') get_o_key().second();
#if CAP_INV
  else if(uni == 'i' && inv::on) 
    pushScreen(inv::show);
#endif
#if CAP_SAVE
  else if(uni == 't') {
    msgs.clear();
    scores::load();
    }
  #endif
  
  else if(doexiton(sym, uni) && !didsomething) {
    popScreen();
    msgs.clear();
    if(!canmove) {
      addMessage(XLAT("GAME OVER"));
      addMessage(timeline());
      }
    }
  }

EX int counthints() {
  for(int h=0;; h++) if(hints[h].last < 0) return h;
  }

EX void showMissionScreen() {
  cancel(); cancel = noaction;
  popScreenAll();
  achievement_final(false);

  if(daily::on) {
    #if CAP_DAILY
    pushScreen(daily::showMenu);
    #endif
    }
  else
    pushScreen(showGameMenu);

#if CAP_TOUR
  if(!tour::on)
#endif
  {
    int ch = counthints();
    hinttoshow = ch;
    int h;
    for(h=0; h < ch; h++) {
      if(!hints[h].usable()) continue;
      if(hinttoshow == ch || hints[h].last < hints[hinttoshow].last) hinttoshow = h;
      }
    hints[hinttoshow].last = time(NULL);
    }
  
  dialog::highlight_text = contstr();
  }

}
