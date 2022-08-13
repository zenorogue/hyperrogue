// Hyperbolic Rogue -- the Tutorial/presentation
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file tour.cpp
 *  \brief the Tutorial and presentation system
 */

#include "hyper.h"
namespace hr { 

#if !CAP_TOUR
EX namespace tour {
  EX always_false on;
EX }
#endif

#if CAP_TOUR

/** \brief Variables and function related to Guided Tour and other presentations. */
EX namespace tour {

/** \brief are we currently in a presentation */
EX bool on;

/** \brief should the presentation texts be shown */
EX bool texts = true;

EX string tourhelp;

/** \brief index of the current slide */
EX int currentslide;

#if HDR
/** \brief a parameter for the slides' action function */
enum presmode { 
  pmStartAll = 0,
  pmStart = 1, pmFrame = 2, pmStop = 3, pmKey = 4, pmRestart = 5,
  pmAfterFrame = 6, pmHelpEx = 7,
  pmGeometry = 11, pmGeometryReset = 13, pmGeometryStart = 15,
  pmGeometrySpecial = 16
  };

/** \brief slide definition */
struct slide { 
  /** \brief title of this slide */
  string name; 
  /** \brief ID (currently unused */
  int unused_id; 
  /** \brief various flags */
  flagtype flags; 
  /** \brief the helptext */
  string help; 
  /** \brief This function is called while this slide is displayed. Parameter hr::tour::presmode mode says what should be done */
  function<void(presmode mode)> action;
  };  

/** \brief in which geometries does this slide work */
namespace LEGAL {
  enum flagtype { NONE, UNLIMITED, HYPERBOLIC, ANY, NONEUC, SPECIAL };
  }

/** \brief when Enter pressed while showing the text, skip to the next slide immediately */
static const flagtype QUICKSKIP=8;
/** \brief The final slide. Shows where the presentation ends */
static const flagtype FINALSLIDE=16;
/** \brief Pressing Enter while in another geometry should change slides immediately */
static const flagtype QUICKGEO=32;
/** \brief This slide should be displayed in sidescreen mode */
static const flagtype SIDESCREEN = 64;
/** \brief When changing geometries, show the name of the slide, instead of the current land */
static const flagtype USE_SLIDE_NAME = 128;
/** \brief do not display any help line */
static const flagtype NOTITLE = 256;
/** \brief always display the text, even if going back or texts are disabled */
static const flagtype ALWAYS_TEXT = 256;
#endif

EX vector<reaction_t> restorers;

#if HDR
template<class T, class U> void slide_backup(T& what, U value) {
  T backup = what;
  restorers.push_back([&what, backup] { what = backup; });
  what = value;
  }

template<class T> void slide_backup(T& what) { slide_backup(what, what); }
#endif

EX void on_restore(const reaction_t& t) {
  restorers.push_back(t);
  }

EX void slide_restore_all() {
  while(!restorers.empty()) {
    restorers.back()();
    restorers.pop_back();
    }
  }

EX void slide_url(presmode mode, char key, string text, string url) {
  if(mode == pmHelpEx)
    help_extensions.push_back(help_extension{key, text, [url] () { 
      open_url(url);
      }});
  }

EX void slide_action(presmode mode, char key, string text, reaction_t act) {
  if(mode == pmHelpEx)
    help_extensions.push_back(help_extension{key, text, act});
  }

EX void enable_canvas_backup(char canv) {
  slide_backup(patterns::whichCanvas, canv);
  slide_backup(firstland, laCanvas);
  slide_backup(specialland, laCanvas);
  slide_backup(land_structure);
  slide_backup(randomPatternsMode);
  enable_canvas();
  }

/** \brief an auxiliary function to enable a visualization in the Canvas land */
EX void setCanvas(presmode mode, char canv) {
  if(mode == pmStart) {
    gamestack::push();
    enable_canvas_backup(canv);
    start_game();
    resetview();
    }
  if(mode == pmStop) {
    gamestack::pop();
    slide_restore_all();
    }
  }

/** \brief static mode: we get Orbs of Teleport to use them instead of movement */
bool sickmode;

EX function<eLand(eLand)> getNext;
EX function<bool(eLand)> quickfind;
EX function<bool(eLand)> showland;

#define GETNEXT getNext = [](eLand old)
#define QUICKFIND quickfind = [](eLand l)
#define SHOWLAND(f) showland = [](eLand l) { return f; }

/** \brief the caption of the special command (executed by pressing '5') in the current slide */
EX string slidecommand;

/** \brief hooks to execute after calling presentation */
EX hookset<void(int)> hooks_slide;

/** \brief call action(mode) for the current slide. Also sets up some default stuff */
EX void presentation(presmode mode) {

  cheater = 0;
  
  if(mode == pmStart) tourhelp = XLAT(slides[currentslide].name);
  
  if(sickmode && !items[itOrbTeleport]) items[itOrbTeleport] = 1;
  if(mode == pmStart) slidecommand = "";

  GETNEXT { return laNone; };
  QUICKFIND { return false; };
  SHOWLAND(true);
  
  slides[currentslide].action(mode);
  callhooks(hooks_slide, mode);
  if(mode == pmStop) slide_restore_all();
  }

string parent_folder(const string& s) {
  for(int k=isize(s)-2; k>=0; k--) if(s[k] == '/') return s.substr(0, k+1);
  return "";
  }

string get_slidename(const string& s) {
  int i = 0;
  for(int k=0; k<isize(s); k++) if(s[k] == '/') i = k+1;
  return s.substr(i);
  }

string get_foldername(const string& s) {
  int i = 0;
  for(int k=0; k<isize(s); k++) if(s[k] == '/') i = k+1;
  return s.substr(0, i);
  }

bool in_folder(const string& s, const string& folder) {
  return s.substr(0, isize(folder)) == folder;
  }

string get_subname(const string& s, const string& folder) {
  for(int k=isize(folder); k<isize(s); k++) if(s[k] == '/') 
    return s.substr(isize(folder), k+1 - isize(folder));
  return s.substr(isize(folder));
  }

/** \brief display the help text for the current slide if texts enabled */
EX void slidehelp() {
  if(!slides[currentslide].help[0]) return;
  string slidename = get_slidename(slides[currentslide].name);
  gotoHelp(
    help =
      helptitle(XLAT(slidename), 0xFF8000) +
      XLAT(slides[currentslide].help)
    );
  presentation(pmHelpEx);
  }

/** \brief return from a subgame launched while in presentation */
void return_geometry() {
  gamestack::pop();
  pconf.scale = 1; pconf.alpha = 1;
  presentation(pmGeometryReset);
  addMessage(XLAT("Returned to your game."));
  }

EX bool next_slide() {
  flagtype flags = slides[currentslide].flags;
  popScreenAll();
  if(gamestack::pushed()) {
    return_geometry();
    if(!(flags & QUICKGEO)) return true;
    }
  if(flags & FINALSLIDE) return true;
  presentation(pmStop);
  slide_restore_all();
  currentslide++;
  presentation(pmStart);
  if(texts) slidehelp();
  return true;
  }

bool handleKeyTour(int sym, int uni) {
  if(!tour::on) return false;
  if(!(cmode & sm::DOTOUR)) return false;
  bool inhelp = cmode & sm::HELP;
  flagtype flags = slides[currentslide].flags;
  if((sym == SDLK_RETURN || sym == SDLK_KP_ENTER) && (!inhelp || (flags & QUICKSKIP)))
    return next_slide();
  if(sym == SDLK_BACKSPACE) {
    if(gamestack::pushed()) { 
      gamestack::pop();
      if(!(flags & QUICKGEO)) return true;
      }
    if(currentslide == 0) { slidehelp(); return true; }
    presentation(pmStop);
    currentslide--;
    presentation(pmStart);
    popScreenAll();
    if(inhelp || (flags & ALWAYS_TEXT)) slidehelp();
    return true;
    }
  int legal = slides[currentslide].flags & 7;
  if((NUMBERKEY == '1' || NUMBERKEY == '2') && (legal != LEGAL::NONE)) {
    
    if(legal == LEGAL::SPECIAL) {
      presentation(pmGeometrySpecial);
      return true;
      }

    if(legal == LEGAL::NONE || legal == LEGAL::HYPERBOLIC) {
      addMessage(XLAT("You cannot change geometry in this slide."));
      return true;
      }
    if(legal == LEGAL::UNLIMITED && NUMBERKEY == '1') {
      addMessage(XLAT("This does not work in bounded geometries."));
      return true;
      }
    if(legal == LEGAL::NONEUC && NUMBERKEY == '2') {
      addMessage(XLAT("This does not work in Euclidean geometry."));
      return true;
      }
    if(legal == LEGAL::HYPERBOLIC && NUMBERKEY != '3') {
      addMessage(XLAT("This works only in hyperbolic geometry."));
      return true;
      }
    
    if(NUMBERKEY == '2') {
      dynamicval<eGeometry> g(geometry, gEuclid);
      if(cwt.at->land != laCanvas && !land_validity(cwt.at->land).quality_level) {
        addMessage(XLAT("This land has no Euclidean version."));
        return true;
        }
      }
    
    if(NUMBERKEY == '1') {
      dynamicval<eGeometry> g(geometry, gSphere);
      if(cwt.at->land != laCanvas && !land_validity(cwt.at->land).quality_level) {
        addMessage(XLAT("This land has no spherical version."));
        return true;
        }
      }
    
    if(geometry || CHANGED_VARIATION) {
      return_geometry();
      return true;
      }

    presentation(pmGeometry);

    firstland = specialland = cwt.at->land;
    gamestack::push();
    switch(NUMBERKEY) {
      case '3': 
        set_variation(eVariation::pure);
        break;
      case '1':
        set_geometry(gSphere);
        pconf.alpha = 1, pconf.scale = .5;
        break;
      case '2':
        set_geometry(gEuclid);
        pconf.alpha = 1, pconf.scale = .5;
        break;
      }      
    start_game();
    resetview();
    presentation(pmGeometryStart);
    string x;
    if(slides[currentslide].flags & USE_SLIDE_NAME) {
      if(NUMBERKEY == '1') x = XLAT("Spherical version of %the1. ", s0 + "'" + get_slidename(slides[currentslide].name) + "'");
      if(NUMBERKEY == '2') x = XLAT("Euclidean version of %the1. ", s0 + "'" + get_slidename(slides[currentslide].name) + "'");
      }
    else {
      if(NUMBERKEY == '1') x = XLAT("Spherical version of %the1. ", cwt.at->land);
      if(NUMBERKEY == '2') x = XLAT("Euclidean version of %the1. ", cwt.at->land);
      }
    if(mousing)
      addMessage(x + XLAT("Click again to go back to your game."));
    else
      addMessage(x + XLAT("Press %1 again to go back to your game.", dialog::keyname(sym)));
    return true;
    }
  if(NUMBERKEY == '3' && sphere) {
    if(pconf.alpha < 2) pconf.scale = 400, pconf.alpha = 400; else pconf.scale = .5, pconf.alpha = 1;
    addMessage(XLAT("Changed the projection."));
    return true;
    }
  if(NUMBERKEY == '4') {
    popScreenAll();
    if(items[itOrbTeleport]) goto give_aether;
    items[itOrbTeleport] = 1;
    checkmove();
    if(!canmove) {
      if(items[itOrbAether]) goto give_flash;
      give_aether:
      items[itOrbAether] = 10;
      checkmove();
      if(!canmove) {
        give_flash:
        activateFlash();
        canmove = true;
        }
      }
    else {
      bool shift = vid.shifttarget & 1;
      addMessage(
        shift ? 
          XLAT("Shift-click a location to teleport there.")
        : XLAT("Click a location to teleport there.")
        );
      }
    return true;
    }
  if(NUMBERKEY == '5') {
    presentation(pmKey);
    return true;
    }
  if(NUMBERKEY == '6') {
    sickmode = !sickmode;
    static ld spd;
    if(sickmode == true) {
      spd = vid.sspeed, vid.sspeed = 5;
      addMessage(XLAT("Static mode enabled."));
      }
    else {
      vid.sspeed = spd;
      addMessage(XLAT("Static mode disabled."));
      }
    return true;
    }
  if(NUMBERKEY == '7') {
    texts = !texts;
    if(texts) slidehelp();
    else addMessage("Help texts disabled.");
    return true;
    }
  if(NUMBERKEY == '8') {
    history::includeHistory = !history::includeHistory;
    return true;
    }
  if(NUMBERKEY == '9') {
    ss::current_folder = get_foldername(slides[currentslide].name);
    pushScreen(ss::showMenu);
    return true;
    }
  if(NUMBERKEY == '0') {
    tour::start();
    pushScreen(showStartMenu);
    return true;
    }
  return false;
  }

EX void checkGoodLand(eLand l) {
  if(!showland(l) && texts) 
    gotoHelp(XLAT(
      "This guided tour is different than most other game tutorials -- "
      "you are not forced to do anything, and you can go wherever you want.\n\n"
      "However, %the1 is not what we are talking about now. "
      "We will not explain this land at the moment, and you could potentially "
      "get lost there.\n\n"
      "Remember that you can get to the next slide by pressing Enter.",
      l
      ) +
      XLAT(" This tour will not advance on its own -- you have to press Enter (not while reading help text).")
      );
  }

EX namespace ss {
  EX slide *wts;
  
  EX string current_folder;

  string slidechars = "abcdefghijklmnopqrsvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ23456789!@#$%^&*(";
    
  #if HDR
  using slideshow_callback = function<void(string, slide*, char)>;
  #endif
  
  EX hookset<void(slideshow_callback)> hooks_extra_slideshows;
  
  EX void for_all_slideshows(const slideshow_callback& cb) {
    cb(XLAT("HyperRogue Guided Tour"), default_slides, 't');
    callhooks(hooks_extra_slideshows, cb);
    }
  
  EX void slideshow_menu() {
    dialog::init(XLAT("slideshows"), forecolor, 150, 100);
    for_all_slideshows([] (string title, slide *sl, char ch) {
      dialog::addBoolItem(title, wts == sl, ch);
      dialog::add_action([sl] { wts = sl; popScreen(); });
      });
    dialog::addBack();
    dialog::display();
    }
  
  EX void showMenu() {
    if(!wts) wts = slides; 

    dialog::init(XLAT("slides"), forecolor, 150, 100);
    
    string cftrans, cftransbuild;
    for(char c: current_folder)
      if(c == '/') {
        cftrans += XLAT(cftransbuild);
        cftrans += "/";
        cftransbuild = "";
        }
      else cftransbuild += c;    
    
    if(current_folder != "") {
      dialog::addTitle(cftrans, 0xFFFFFFFF, 120);
      dialog::addItem(XLAT("go up"), 'u');
      dialog::add_action([] { current_folder = parent_folder(current_folder); });
      dialog::addBreak(100);
      }
    
    string last = "";
    
    int key = 0;
    
    for(int i=0; (i==0 || !(wts[i-1].flags & FINALSLIDE)); i++) {
      if(!in_folder(wts[i].name, current_folder)) continue;
      string sf = get_subname(wts[i].name, current_folder);
      if(sf == last) continue;
      last = sf;
      string sfd;
      
      if(sf.back() == '/') {
        sfd = sf.substr(0, isize(sf)-1);
        sfd = XLAT(sfd) + "/  ";
        }
      else sfd = XLAT(sf);

      dialog::addBoolItem(sfd, wts == slides && in_folder(slides[currentslide].name, current_folder+sf), slidechars[key++]);
      dialog::add_action([i, sf] {
        if(sf.back() == '/') {
          current_folder += sf;
          return;
          }
        if(gamestack::pushed()) {
          gamestack::pop();
          presentation(pmGeometryReset);
          }
        if(slides != wts) {
          while(tour::on) restart_game(rg::tour);
          slides = wts;
          tour::start();
          }
        presentation(pmStop);
        currentslide = i;
        popScreenAll();
        presentation(pmStart);
        if(texts) slidehelp();
        });
      }
    dialog::addBreak(50);
    int cnt = 0;

    for_all_slideshows([&] (string title, slide *sl, char ch) { cnt++; });

    if(cnt > 1) {
      dialog::addItem(XLAT("change slideshow"), '1');
      dialog::add_action_push(slideshow_menu);
      }
    dialog::addBack();
    dialog::display();
    }

  EX }

EX void initialize_slides() {
  dynamicval<int> cs(currentslide, 0);
  for(currentslide=0;; currentslide++) {
    presentation(pmStartAll);
    if(slides[currentslide].flags & FINALSLIDE) break;
    }
  }

EX void print() {
  dynamicval<int> cs(currentslide, 0);
  for(currentslide=0;; currentslide++) {
    auto& sl = slides[currentslide];
    println(hlog, sl.name);
    string str = sl.name;
    for(char& c: str) c = '=';
    println(hlog, str);
    println(hlog);
    println(hlog, sl.help);
    println(hlog);
    if(slides[currentslide].flags & FINALSLIDE) break;
    }
  }
  
EX void start() {
  currentslide = 0;
  pconf.scale = 1;
  pconf.alpha = 1;
  pmodel = mdDisk;
  if(!tour::on) {
    initialize_slides();
    }
  else {
    presentation(pmStop);
    stop_game();
    firstland = specialland = laIce;
    }
  restart_game(rg::tour);
  if(tour::on) {
    if(texts) slidehelp();
    presentation(pmStart);
    }
  }

string bog = "Basics of gameplay/";
string shapes = "Hyperbolic shapes/";
string models = "Projections of hyperbolic space/";
string pcg = "Procedural generation/";

/** \brief the default presentation (the Guided Tour) */
EX slide default_slides[] = {
#if ISMOBILE
  {"Note for mobiles", 10, LEGAL::NONE | QUICKSKIP,
    "This tour is designed for computers, "
    "and keys are given for all actions. It will "
    "work without a keyboard though, although less "
    "comfortably -- just ignore the keys "
    "given and select options from MENU.\n\n"
    "Select 'next slide' from MENU.",
    
    [] (presmode mode) {
      if(mode == pmStartAll) firstland = specialland = laIce;
      if(mode == 1) {
        if(tour::texts) addMessage(XLAT("Welcome to the HyperRogue Guided Tour!"));
        else clearMessages();  
        }
      SHOWLAND( l == laIce );
      }
    },
#endif
  {"Introduction", 10, LEGAL::NONE | QUICKSKIP,
    "This tour is mostly aimed to show what is "
    "special about the geometry used by HyperRogue. "
    "It also shows the basics of gameplay, and "
    "how is it affected by geometry.\n\n"
    "You decide when you want to stop playing with the "
    "current \"slide\" and go to the next one, by pressing Enter. You can also "
    "press ESC to see a "
    "menu with other options.",
    [] (presmode mode) {
      if(mode == pmStartAll) firstland = specialland = laIce;
      if(mode == 1) {
        if(tour::texts) addMessage(XLAT("Welcome to the HyperRogue Guided Tour!"));
        else clearMessages();  
        }
      SHOWLAND( l == laIce );
      }
    },
  {bog+"Basics of gameplay", 11, LEGAL::ANY,
    "The game starts in the Icy Lands. Collect the Ice Diamonds "
    "(press F1 if you do not know how to move or the end goal). "
    "After you collect many of them, monsters will start to pose a challenge.\n"
    "As is typical in roguelikes and other games based on tactical skill rather "
    "than story, if you lose, you have to start a new one from the start. "
    "However, in this tutorial, you can simply press '4' "
    "to teleport away from a bad situation."
    "In general, the tutorial is rigged to show you what it "
    "wants -- for example, in this slide, you can press '5' to get "
    "lots of Ice Diamonds quickly.",
    [] (presmode mode) {
      slidecommand = "gain Ice Diamonds";
      if(mode == 4)
        forCellEx(c2, cwt.at) 
        forCellEx(c3, c2)
        if(c3->wall == waNone && c3->item == itNone && c3->monst == moNone && c3 != cwt.at) 
          c3->item = itDiamond;
      SHOWLAND( l == laIce );
      }
    }, 
  {bog+"Hyperbolic Soccerball", 23, LEGAL::ANY | USE_SLIDE_NAME, 
    "New players think that the action of HyperRogue takes place on a sphere. "
    "This is not true -- in fact, just the opposite is true!\n\n"
    "A sphere can be tiled with hexagons and pentagons (the classic soccer ball); "
    "pentagons cause the world to close.\n\n"
    "The world of HyperRogue is tiled with hexagons "
    "and heptagons; heptagons give extra space!\n\n",
    [] (presmode mode) {
      setCanvas(mode, 'F');
      if(mode == 5) {
        cwt.at->move(0)->monst = moRunDog;
        cwt.at->move(1)->monst = moGoblin;
        }
      SHOWLAND( l == laCanvas );
      }
    },
  {bog+"Hypersian Rug model", 21, LEGAL::HYPERBOLIC,
#if CAP_RUG
    "The next slide will show the rendering of the surface HyperRogue "
    "actually takes place on.\n\n"
    "Use arrow keys to rotate the model, and Page Up/Down to zoom.\n\n"
    "If you do not see anything, press '5' to try a safer renderer.",
#else
    "This is not true -- the Tutorial in the native desktop version shows "
    "the surface HyperRogue actually takes place on.",
#endif
    [] (presmode mode) {
#if CAP_RUG
      if(mode == 1)
        rug::init();
      if(mode == 3) 
        rug::close();
      slidecommand = "switch renderer";
      if(mode == 4) {
        rug::close();
        rug::rendernogl = !rug::rendernogl;
        rug::init();
        }
#endif
      SHOWLAND( l == laIce );
      }
    },
  {bog+"Expansion", 22, LEGAL::ANY | USE_SLIDE_NAME,
    "The next slide shows the number of cells in distance 1, 2, 3, ... from you. "
    "It grows exponentially: there are more than 10^100 cells "
    "in radius 1000 around you, and you will move further away during the game!\n\n"
    "This is extremely important in the design of HyperRogue. "
    "HyperRogue has many navigational puzzles -- what would be simple in Euclidean world "
    "is extremely tricky "
    "in hyperbolic geometry (you want to reach a specific location 20 cells away, "
    "which of the thousands of possible directions should you take?); however, other things virtually impossible in Euclidean "
    "world become easy in HyperRogue. "
    "HyperRogue had to be specially designed so that it is impossible to grind the "
    "infinite world. There are almost no permanent upgrades; collecting treasures "
    "brings you benefits, but trying to get too many of the same kind is extremely dangerous.",
    [] (presmode mode) {
      if(mode == 1) viewdists = true;
      if(mode == 2 && !canmove) viewdists = false; // disable when killed
      if(mode == 3) viewdists = false;
      SHOWLAND( l == laIce );
      }
    },
  {bog+"Tiling and Tactics", 23, LEGAL::ANY | USE_SLIDE_NAME, 
    "The tactics of fighting simple monsters, such as the Yetis from the Icy Lands, "
    "might appear shallow, but hyperbolic geometry is essential even there. "
    "In the next slide, you are attacked by two monsters at once. "
    "You can make them line up simply by "
    "running away in a straight line. "
    "Press '2' to try the same in the Euclidean world -- it is impossible.",
    [] (presmode mode) {
      setCanvas(mode, 'F');
      if(mode == 5) {
        cwt.at->move(0)->monst = moRunDog;
        cwt.at->move(1)->monst = moGoblin;
        }
      SHOWLAND( l == laCanvas );
      }
    },
  {shapes+"Straight Lines", 24, LEGAL::ANY, 
    "Hyperbolic geometry has been discovered by the 19th century mathematicians who "
    "wondered about the nature of paralellness. Take a line L and a point A. "
    "Can a world exist where there is more than one line passing through A "
    "which does not cross L?\n\n"
    "The Icy Land will be very dangerous if you have lots of Ice Diamonds -- "
    "lots of Yetis and Ice Wolves hunting you! But the other lands, where "
    "you have no treasures yet, will still be (relatively) safe.\n\n"
    "Wander further, and you should find Crossroads quickly -- "
    "the Great Walls are straight lines, and indeed, they work differently than in "
    "Euclidean. On the other side of Great Walls, you see other lands -- "
    "there are about 60 lands in HyperRogue, based "
    "on different mechanics and aspects of hyperbolic geometry.",
    [] (presmode mode) {
      GETNEXT {
        if(isCrossroads(old)) 
          return pick(
            pick(laRedRock, laWarpCoast, laMirror),
            pick(laLivefjord, laAlchemist, laHell),
            pick(laJungle, laDesert, laRose),
            pick(laGraveyard, laMotion, laDryForest)
            );
        else return laCrossroads;
        };
      SHOWLAND( l == laCrossroads || l == laIce );
      }
    },
  {shapes+"Running Dogs", 25, LEGAL::ANY,
    "To learn more about straight lines, "
    "wander further, and you should find the Land of Eternal Motion. "
    "Try to run in a straight line, with a Running Dog next to you. "
    "Even though the Running Dog runs at the same speed as you, "
    "it will appear to go slower -- this is because you are running "
    "in a straight line, and the Running Dog has to run in a curve "
    "called an equidistant.\n\n"
#if ISMAC
    "Remember that you can click with right Shift on anything to get more information.",
#else
    "Remember that you can right click on anything to get more information.",
#endif
    [] (presmode mode) {
      GETNEXT {
        if(isCrossroads(old)) return pick(laMotion, laNone);
        else if(old == laMotion) return laCrossroads;
        else return laMotion;
        };
      SHOWLAND( l == laCrossroads || l == laMotion );
      }
    },
  {shapes+"Equidistants", 27, LEGAL::ANY,
    "Equidistants are curves which are at some fixed distance from a "
    "straight line. Some lands in HyperRogue are based on equidistants; "
    "you should see them after wandering a bit more.\n\n"
    "This tutorial gives you freedom to go wherever you choose, "
    "but we do not recommend going deep into the Dungeon or the Ocean -- "
    "getting back might be difficult.",
    [] (presmode mode) {
      GETNEXT {
        if(isCrossroads(old))
          return hrand(100) < 20 ? laNone : 
            pick(laOcean, laIvoryTower, laDungeon, laEndorian);
        else return laCrossroads;
        };
      SHOWLAND( l == laCrossroads || l == laDungeon || l == laOcean || l == laIvoryTower || l == laEndorian );
      }
    },
  {shapes+"Circles", 26, LEGAL::ANY,
    "Circles are strange in hyperbolic geometry too. "
    "Look for the Castle of Camelot in the Crossroads; "
    "the Round Table inside is a circle of radius 28. "
    "Finding its center is a difficult challenge.\n\n"
    "Press '5' to cheat by seeing the smaller circles too.\n\n"
    "Note: Camelot and some other lands are unlocked earlier in the Tutorial than in a real game.",
    [] (presmode mode) {
      slidecommand = 
        camelotcheat ? XLAT("enable the Camelot cheat")
        : XLAT("disable the Camelot cheat");
      if(mode == 4)
        camelotcheat = !camelotcheat;
      GETNEXT {
        if(!isCrossroads(old)) return laCrossroads;
        return laNone;
        };
      QUICKFIND {
        return (l == laCamelot);
        };
      SHOWLAND( l == laCrossroads || l == laCamelot );
      }
    },
  {shapes+"Horocycles", 28, LEGAL::ANY,
    "Horocycles are similar to circles, but you cannot reach their center at all -- "
    "they can be understood as limit circles of infinite radius centered in some point "
    "in infinity (also called an ideal point).\n\n"
    "Go to R'Lyeh, and you should quickly find a Temple of Cthulhu there. "
    "Each circle of columns is actually a horocycle. Horocycles in a given "
    "temple are concentric, and there is an infinite number of them.",
    [] (presmode mode) {
      GETNEXT {
        if(isCrossroads(old))
          return pick(laRlyeh, laNone, laNone);
        else return pick(laCrossroads, old == laRlyeh ? laNone : laRlyeh);
        };
      QUICKFIND {
        return (l == laTemple);
        };
      SHOWLAND ( l == laCrossroads || l == laRlyeh || l == laTemple );
      }
    },
  {shapes+"Half-plane model", 47, LEGAL::HYPERBOLIC,
    "The game is normally displayed in the so called Poincaré disk model, "
    "which is a kind of a map of the infinite hyperbolic world. "
    "There are many projections of Earth, but since Earth is curved, "
    "all of them have to distort distances or angles in some way -- "
    "the same is true in hyperbolic geometry. "
    "The next slide shows another model, called the Poincaré upper half-plane model. In this model, "
    "horocycles centered at one specific ideal point are drawn as straight lines.",
    [] (presmode mode) {
      static int smart;
      if(mode == 1) 
        pmodel = mdHalfplane, smart = vid.use_smart_range, vid.use_smart_range = 2;
      if(mode == 2) 
        models::rotation = cwt.at->land == laDungeon ? 0 : 2;
      if(mode == 3) pmodel = mdDisk, models::rotation = 0, vid.use_smart_range = smart;
      }
    },
  {"Curvature", 29, LEGAL::ANY,
    "Now, go to the Burial Grounds and find an Orb of the Sword. The Sword appears to "
    "always be facing in the same direction whatever you do, and it appears that "
    "you have to rotate the sword to excavate the treasures; "
    "yet, it is possible to excavate them! You might have already noticed "
    "that the world rotates after you move around a loop and return to an old "
    "place.\n\n"
    "This is related to the fact that the world of HyperRogue is curved, and "
    "the sum of angles in a triangle is not equal to 180 degrees.",
    [] (presmode mode) {
      slidecommand = XLAT("gain Orb of the Sword");
      if(mode == 4)
        items[itOrbSword] = 90;
      GETNEXT {
        if(isCrossroads(old))
          return pick(laBurial, laNone, laNone);
        else return pick(laCrossroads, old == laBurial ? laNone : laBurial);
        };
      QUICKFIND {
        return (l == laBurial && !items[itOrbSword]);
        };
      SHOWLAND ( l == laRlyeh || l == laCrossroads || l == laBurial );
      }
    },
  {pcg+"Periodic patterns", 30, LEGAL::UNLIMITED | USE_SLIDE_NAME,
    "Hyperbolic geometry yields much more interesting periodic patterns "
    "than Euclidean.",
    [] (presmode mode) {
      setCanvas(mode, 't');
      if(mode == 1) 
        patterns::displaycodes = true,
        patterns::whichPattern = patterns::PAT_ZEBRA;
      if(mode == 3) 
        patterns::displaycodes = false,
        patterns::whichPattern = patterns::PAT_NONE;
      SHOWLAND ( l == laCanvas );
      }
    },
  {pcg+"Periodic patterns: application", 31, LEGAL::ANY,
    "Many lands in HyperRogue are based around periodic patterns. "
    "For example, both Zebra and Windy Plains are based on the pattern "
    "shown in the previous slide. "
    "Such lands often have tree-like nature.",
    [] (presmode mode) {
      GETNEXT {
        if(isCrossroads(old))
          return pick(
            pick(laWineyard, laEmerald, laPower),
            pick(laZebra, laWhirlwind),
            laPalace, laNone
            );
        else return laCrossroads;
        };
      SHOWLAND ( l == laCrossroads ||
      l == laZebra || l == laWhirlwind || l == laPalace || l == laPrairie ||
      l == laEmerald || l == laWineyard || l == laPower );
      }
    },
  {pcg+"Fractal landscapes", 32, LEGAL::UNLIMITED | USE_SLIDE_NAME,
    "On the following slide, the colors change smoothly in the whole infinite world. "
    "Again, this works better than in Euclidean geometry.",
    [] (presmode mode) {
      setCanvas(mode, 'l');
      SHOWLAND ( l == laCanvas );
      }
    },
  {pcg+"Fractal landscapes: application", 33, LEGAL::ANY,
    "This is applied in HyperRogue to create landscapes, such as the chasms in the "
    "land of Reptiles or the Dragon Chasms, which you should find quickly. "
    "Also in the Dragon Chasms, you can find a Baby Tortoise, and try to find "
    "a matching adult tortoise in the Galápagos. "
    "There are over two millions of species, but since there is so much space in "
    "hyperbolic geometry, finding a matching tortoise is possible. The brighter "
    "the color in Galápagos is, the more aspects of the tortoises in the given "
    "area are matching.",
    [] (presmode mode) {
      slidecommand = "create a baby tortoise";
      if(mode == 4) {
        cell *c = cwt.at->move(0);
        c->item = itBabyTortoise;
        tortoise::babymap[c] = tortoise::getb(c) ^ tortoise::getRandomBits();
        }
      GETNEXT {
        if(old == laDragon) return pick(laTortoise, laTortoise, laCrossroads);
        else if(isCrossroads(old))
          return pick(laDragon, laReptile, laNone);
        return laNone;
        };
      QUICKFIND {
        return l == laTortoise && !items[itBabyTortoise];
        };
      SHOWLAND ( l == laCrossroads || l == laReptile || l == laDragon || l == laTortoise );
      }
    },
  {models+"Poincaré Ball model", 41, LEGAL::HYPERBOLIC,
    "The Poincaré disk model is a model of a hyperbolic *plane* -- you "
    "might wonder why are the walls rendered in 3D then.\n\n"
    "HyperRogue actually assumes that the floor level is an equidistant surface "
    "in a three-dimensional hyperbolic world, and the camera is placed above the "
    "plane that the surface is equidistant to (which boils down to showing "
    "the floor level in Poincaré disk model).\n\n"
    "This is shown on the next slide, in the Poincaré ball model, which is "
    "the 3D analog of the Poincaré disk model.",
    [] (presmode mode) {
      if(mode == 1) pmodel = mdBall;
      if(mode == 3) pmodel = mdDisk;
      }
    },
  {models+"Hyperboloid model", 42, LEGAL::ANY,
    "Let's see more models of the hyperbolic plane. "
    "This model uses a hyperboloid in the Minkowski geometry; "
    "it is used internally by HyperRogue.",
    [] (presmode mode) {
      if(mode == 1) pmodel = mdHyperboloid;
      if(mode == 3) pmodel = mdDisk;
      }
    },
  {models+"Beltrami-Klein model", 43, LEGAL::ANY | USE_SLIDE_NAME,
    "This model renders straight lines as straight, but it distorts angles.",
    [] (presmode mode) {
      if(mode == 1 || mode == pmGeometryReset || mode == pmGeometry) pconf.alpha = 0;
      if(mode == 3) pconf.alpha = 1;
      }
    },
  {models+"Gans model", 44, LEGAL::ANY | USE_SLIDE_NAME,
    "Yet another model, which corresponds to orthographic projection of the "
    "sphere. Poincaré disk model, Beltrami-Klein model, and the Gans "
    "model are all obtained by looking at either the hyperboloid model or an "
    "equidistant surface from various distances.",
    [] (presmode mode) {
      if(mode == 1 || mode == pmGeometryReset || mode == pmGeometry) pconf.alpha = 400, pconf.scale = 150;
      if(mode == 3) pconf.alpha = pconf.scale = 1;
      }
    },
  {models+"Band model", 45, LEGAL::NONEUC | USE_SLIDE_NAME, 
    "The band model is the hyperbolic analog of the Mercator projection of the sphere: "
    "a given straight line is rendered as a straight line, and the rest of the "
    "world is mapped conformally, that is, angles are not distorted. "
    "Here, we take the straight line connecting your starting point and your "
    "current position -- usually the path taken by the player is surprisingly "
    "close to a straight line. Press '8' to see this path.\n\n"
    "If you want, press '5' to see it rendered as a spiral, although it takes lots of time and "
    "memory.",
    [] (presmode mode) {
      static int smart;
      if(mode == 1) pmodel = mdBand, history::create_playerpath(), models::rotation = 0,
        smart = vid.use_smart_range, vid.use_smart_range = 2;
      if(mode == 3) {
        history::clear(), pmodel = mdDisk;
        resetview();
        drawthemap();
        centerpc(INF);
        history::includeHistory = false;
        vid.use_smart_range = smart;
        }
#if CAP_SDL
      slidecommand = "render spiral";
      if(mode == 4) history::open_filedialog_to_create_image(true);
      if(mode == 11) history::create_playerpath();
      if(mode == 13) history::clear();
#endif
      }
    },
/*{"Conformal square model", 46, LEGAL::HYPERBOLIC,
    "The world can be mapped conformally to a square too.",
    [] (presmode mode) {
      if(mode == 1) pmodel = mdPolygonal, polygonal::solve();
      if(mode == 3) pmodel = mdDisk;
      }
    }, */
#if !ISWEB
  {"Shoot'em up mode", 52, LEGAL::NONE | USE_SLIDE_NAME,
    "In the shoot'em up mode, space and time is continuous. "
    "You attack by throwing knives. "
    "Very fun with two players!\n\n"
    "There are other special modes too which change the gameplay or "
    "focus on a particular challenge.",
    [] (presmode mode) {
      if(mode == 1) {
        firstland = cwt.at->land;
        gamestack::push();
        switch_game_mode(rg::shmup);
        start_game();
        resetview();
        }
      if(mode == 3) {
        shmup::clearMonsters();
        gamestack::pop();
        }    
      }
    },
#endif
  {"THE END", 99, LEGAL::ANY | FINALSLIDE,
    "This tour shows just a small part of what you can see in the world of HyperRogue. "
    "For example, "
    "hyperbolic mazes are much nicer than their Euclidean counterparts. "
    "Have fun exploring!\n\n"
    "Press '5' to leave the tour mode.",
    [] (presmode mode) {
      slidecommand = XLAT("leave the tour mode");
      if(mode == 4) restart_game(rg::tour);
      }
    }
  };

/** \brief currently used set of slides */
EX slide *slides = default_slides;

auto a1 = addHook(hooks_frame, 100, [] () { if(tour::on) tour::presentation(tour::pmFrame); });
auto a2 = addHook(hooks_handleKey, 100, handleKeyTour);
auto a3 = addHook(hooks_nextland, 100, [] (eLand l) { return tour::on ? getNext(l) : laNone; });

EX }
#endif
}
