// Hyperbolic Rogue -- the Tutorial/presentation
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr { namespace tour {

bool on;

bool texts = true;

string tourhelp;

int currentslide;

// modes: 
//   1 - enter the slide
//   2 - each frame
//   3 - leave the slide
//   4 - quicken or modify the slide
//   5 - on initgame

void setCanvas(presmode mode, char canv) {
  static char wc;
  static eLand ld;
  if(mode == pmStart) {
    push_game();
    wc = patterns::whichCanvas;
    patterns::whichCanvas = canv;
    ld = firstland;
    firstland = laCanvas;
    start_game();
    }
  if(mode == pmStop) {
    patterns::whichCanvas = wc;
    firstland = ld;
    pop_game();
    }
  }

bool sickmode;

function<eLand(eLand)> getNext;
function<bool(eLand)> quickfind;
function<bool(eLand)> showland;

#define GETNEXT getNext = [](eLand old)
#define QUICKFIND quickfind = [](eLand l)
#define SHOWLAND(f) showland = [](eLand l) { return f; }

string slidecommand;

hookset<void(int)> *hooks_slide;

void presentation(presmode mode) {

  cheater = 0;
  
  if(mode == pmStart) tourhelp = XLAT(slides[currentslide].name);
  
  if(sickmode && !items[itOrbTeleport]) items[itOrbTeleport] = 1;
  if(mode == pmStart) slidecommand = "";

  GETNEXT { return laNone; };
  QUICKFIND { return false; };
  SHOWLAND(true);
  
  slides[currentslide].action(mode);
  callhooks(hooks_slide, mode);
  }

void slidehelp() {
  if(texts && slides[currentslide].help[0])
    gotoHelp(
      help = 
        helptitle(XLAT(slides[currentslide].name), 0xFF8000) + 
        XLAT(slides[currentslide].help)
      );
  }

void return_geometry() {
  pop_game();
  vid.scale = 1; vid.alpha = 1;
  presentation(pmGeometryReset);
  addMessage(XLAT("Returned to your game."));
  }

bool handleKeyTour(int sym, int uni) {
  if(!tour::on) return false;
  if(!(cmode & sm::DOTOUR)) return false;
  bool inhelp = cmode & sm::HELP;
  int flags = slides[currentslide].flags;
  if((sym == SDLK_RETURN || sym == SDLK_KP_ENTER) && (!inhelp || (flags & QUICKSKIP))) {
    popScreenAll();
    if(gamestack::pushed()) { 
      return_geometry();
      if(!(flags & QUICKGEO)) return true; 
      }
    if(flags & FINALSLIDE) return true;
    presentation(pmStop);
    currentslide++;
    presentation(pmStart);
    slidehelp();
    return true;
    }
  if(sym == SDLK_BACKSPACE) {
    if(gamestack::pushed()) { 
      pop_game();
      if(!(flags & QUICKGEO)) return true;
      }
    if(currentslide == 0) { slidehelp(); return true; }
    presentation(pmStop);
    currentslide--;
    presentation(pmStart);
    popScreenAll();
    if(inhelp) slidehelp();
    return true;
    }
  if(NUMBERKEY == '1' || NUMBERKEY == '2') {
    int legal = slides[currentslide].flags & 7;

    if(legal == LEGAL_NONE || legal == LEGAL_HYPERBOLIC) {
      addMessage(XLAT("You cannot change geometry in this slide."));
      return true;
      }
    if(legal == LEGAL_UNLIMITED && NUMBERKEY == '1') {
      addMessage(XLAT("This does not work in bounded geometries."));
      return true;
      }
    if(legal == LEGAL_NONEUC && NUMBERKEY == '2') {
      addMessage(XLAT("This does not work in Euclidean geometry."));
      return true;
      }
    if(legal == LEGAL_HYPERBOLIC && NUMBERKEY != '3') {
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
    push_game();
    switch(NUMBERKEY) {
      case '3': 
        set_variation(eVariation::pure);
        break;
      case '1':
        set_geometry(gSphere);
        vid.alpha = 1, vid.scale = .5;
        break;
      case '2':
        set_geometry(gEuclid);
        vid.alpha = 1, vid.scale = .5;
        break;
      }      
    start_game();
    presentation(pmGeometryStart);
    string x;
    if(slides[currentslide].flags & USE_SLIDE_NAME) {
      if(NUMBERKEY == '1') x = XLAT("Spherical version of %the1. ", s0 + "'" + slides[currentslide].name + "'");
      if(NUMBERKEY == '2') x = XLAT("Euclidean version of %the1. ", s0 + "'" + slides[currentslide].name + "'");
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
    if(vid.alpha < 2) vid.scale = 400, vid.alpha = 400; else vid.scale = .5, vid.alpha = 1;
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
    else addMessage(XLAT(
      (vid.shifttarget&1) ? 
        "Shift-click a location to teleport there."
      : "Click a location to teleport there."
      ));
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
    conformal::includeHistory = !conformal::includeHistory;
    return true;
    }
  if(NUMBERKEY == '9') {
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

void checkGoodLand(eLand l) {
  if(!showland(l) && texts) 
    gotoHelp(XLAT(
      "This tutorial is different than most other game tutorials -- "
      "you are not forced to do anything, and you can go wherever you want.\n\n"
      "However, %the1 is not what we are talking about now. "
      "We will not explain this land at the moment, and you could potentially "
      "get lost there.\n\n"
      "Remember that you can get to the next slide by pressing Enter.",
      l
      ) +
      XLAT(" This tutorial will not advance on its own -- you have to press Enter (not while reading help text).")
      );
  }

namespace ss {
  vector<slide*> slideshows;
  slide *wts;

  void list(slide *ss) {
    for(auto s: slideshows) if (s == ss) return;
    slideshows.push_back(ss);
    }

  int sssize;
   
  void showMenu() {
    if(!wts) wts = slides; 

    dialog::init(XLAT("slides"), forecolor, 150, 100);
    
    for(sssize=0;; sssize++) {
      int i = sssize;
      dialog::addBoolItem(XLAT(wts[i].name), wts == slides && i == currentslide, 'a'+i);
      if(wts[i].flags & FINALSLIDE) break;
      }
    dialog::addBreak(50);
    if(isize(slideshows) > 1) dialog::addItem(XLAT("change slideshow"), '1');
    dialog::addBack();
    dialog::display();
    keyhandler = [] (int sym, int uni) {
      if(uni >= 'a' && uni < 'a' + sssize) {
        if(geometry || CHANGED_VARIATION) {
          pop_game();
          presentation(pmGeometryReset);
          }
        if(slides != wts) {
          while(tour::on) restart_game(rg::tour);
          slides = wts;
          tour::start();
          }
        presentation(pmStop);
        currentslide = uni - 'a';
        popScreenAll();
        presentation(pmStart);
        slidehelp();
        }
      else if(uni == '1') {
        list(wts);
        for(int i=0; i<isize(slideshows)-1; i++) if(slideshows[i] == wts) {
          wts = slideshows[i+1]; return;
          }
        wts = slideshows[0];
        }
      else if(doexiton(sym, uni)) { wts = NULL; popScreen(); }
      };
    }

  }
  
void start() {
  ss::list(default_slides);
  currentslide = 0;
  vid.scale = 1;
  vid.alpha = 1;
  pmodel = mdDisk;
  if(!tour::on) presentation(pmStartAll);
  else {
    presentation(pmStop);
    stop_game();
    firstland = specialland = laIce;
    }
  restart_game(rg::tour);
  if(tour::on) {
    slidehelp();
    presentation(pmStart);
    }
  }

slide default_slides[] = {
#if ISMOBILE
  {"Note for mobiles", 10, LEGAL_NONE | QUICKSKIP,
    "This tutorial is designed for computers, "
    "and keys are given for all actions. It will "
    "work without a keyboard though, although less "
    "comfortably -- just ignore the keys "
    "given and select options from MENU.\n\n"
    "Select 'next slide' from MENU.",
    
    [] (presmode mode) {
      if(mode == pmStartAll) firstland = specialland = laIce;
      if(mode == 1) {
        if(tour::texts) addMessage(XLAT("Welcome to the HyperRogue tutorial!"));
        else clearMessages();  
        }
      SHOWLAND( l == laIce );
      }
    },
#endif
  {"Introduction", 10, LEGAL_NONE | QUICKSKIP,
    "This tutorial is mostly aimed to show what is "
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
        if(tour::texts) addMessage(XLAT("Welcome to the HyperRogue tutorial!"));
        else clearMessages();  
        }
      SHOWLAND( l == laIce );
      }
    },
  {"Basics of gameplay", 11, LEGAL_ANY,
    "The game starts in the Icy Lands. Collect the Ice Diamonds "
    "(press F1 if you do not know how to move). "
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
  {"Hypersian Rug model", 21, LEGAL_HYPERBOLIC,
    "New players think that the action of HyperRogue takes place on a sphere. "
#if CAP_RUG
    "This is not true -- the next slide will show the surface HyperRogue "
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
  {"Expansion", 22, LEGAL_ANY | USE_SLIDE_NAME,
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
      if(mode == 2) viewdists = canmove; // disable when killed
      if(mode == 3) viewdists = false;
      SHOWLAND( l == laIce );
      }
    },
  {"Tiling and Tactics", 23, LEGAL_ANY | USE_SLIDE_NAME, 
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
  {"Straight Lines", 24, LEGAL_ANY, 
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
    "there are about 50 lands in HyperRogue, based "
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
  {"Running Dogs", 25, LEGAL_ANY,
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
  {"Equidistants", 27, LEGAL_ANY,
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
  {"Circles", 26, LEGAL_ANY,
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
  {"Horocycles", 28, LEGAL_ANY,
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
  {"Half-plane model", 47, LEGAL_HYPERBOLIC,
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
        conformal::rotation = cwt.at->land == laDungeon ? 0 : 2;
      if(mode == 3) pmodel = mdDisk, conformal::rotation = 0, vid.use_smart_range = smart;
      }
    },
  {"Curvature", 29, LEGAL_ANY,
    "Now, go to the Burial Grounds and find an Orb of the Sword. The Sword appears to "
    "always be facing in the same direction whatever you do, and it appears that "
    "you have to rotate the sword to excavate the treasures; "
    "yet, it is possible to excavate them! You migth have already noticed "
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
      SHOWLAND ( l == laCrossroads || l == laBurial );
      }
    },
  {"Periodic patterns", 30, LEGAL_UNLIMITED | USE_SLIDE_NAME,
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
  {"Periodic patterns: application", 31, LEGAL_ANY,
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
  {"Fractal landscapes", 32, LEGAL_UNLIMITED | USE_SLIDE_NAME,
    "On the following slide, the colors change smoothly in the whole infinite world. "
    "Again, this works better than in Euclidean geometry.",
    [] (presmode mode) {
      setCanvas(mode, 'l');
      SHOWLAND ( l == laCanvas );
      }
    },
  {"Fractal landscapes: application", 33, LEGAL_ANY,
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
        tortoise::babymap[c] = getBits(c) ^ tortoise::getRandomBits();
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
  {"Poincaré Ball model", 41, LEGAL_HYPERBOLIC,
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
  {"Hyperboloid model", 42, LEGAL_ANY,
    "Let's see more models of the hyperbolic plane. "
    "This model uses a hyperboloid in the Minkowski geometry; "
    "it is used internally by HyperRogue.",
    [] (presmode mode) {
      if(mode == 1) pmodel = mdHyperboloid;
      if(mode == 3) pmodel = mdDisk;
      }
    },
  {"Beltrami-Klein model", 43, LEGAL_ANY | USE_SLIDE_NAME,
    "This model renders straight lines as straight, but it distorts angles.",
    [] (presmode mode) {
      if(mode == 1 || mode == pmGeometryReset || mode == pmGeometry) vid.alpha = 0;
      if(mode == 3) vid.alpha = 1;
      }
    },
  {"Gans model", 44, LEGAL_ANY | USE_SLIDE_NAME,
    "Yet another model, which corresponds to orthographic projection of the "
    "sphere. Poincaré disk model, Beltrami-Klein model, and the Gans "
    "model are all obtained by looking at either the hyperboloid model or an "
    "equidistant surface from various distances.",
    [] (presmode mode) {
      if(mode == 1 || mode == pmGeometryReset || mode == pmGeometry) vid.alpha = 400, vid.scale = 150;
      if(mode == 3) vid.alpha = vid.scale = 1;
      }
    },
  {"Band model", 45, LEGAL_NONEUC | USE_SLIDE_NAME, 
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
      if(mode == 1) pmodel = mdBand, conformal::create(), conformal::rotation = 0,
        smart = vid.use_smart_range, vid.use_smart_range = 2;
      if(mode == 3) {
        conformal::clear(), pmodel = mdDisk;
        resetview();
        drawthemap();
        centerpc(INF);
        conformal::includeHistory = false;
        vid.use_smart_range = smart;
        }
#if CAP_SDL
      slidecommand = "render spiral";
      if(mode == 4) conformal::createImage(true);
      if(mode == 11) conformal::create();
      if(mode == 13) conformal::clear();
#endif
      }
    },
/*{"Conformal square model", 46, LEGAL_HYPERBOLIC,
    "The world can be mapped conformally to a square too.",
    [] (presmode mode) {
      if(mode == 1) pmodel = mdPolygonal, polygonal::solve();
      if(mode == 3) pmodel = mdDisk;
      }
    }, */
#if !ISWEB
  {"Shoot'em up mode", 52, LEGAL_NONE | USE_SLIDE_NAME,
    "In the shoot'em up mode, space and time is continuous. "
    "You attack by throwing knives. "
    "Very fun with two players!\n\n"
    "There are other special modes too which change the gameplay or "
    "focus on a particular challenge.",
    [] (presmode mode) {
      if(mode == 1) {
        firstland = cwt.at->land;
        push_game();
        switch_game_mode(rg::shmup);
        start_game();
        }
      if(mode == 3) {
        shmup::clearMonsters();
        pop_game();
        }    
      }
    },
#endif
  {"THE END", 99, LEGAL_ANY | FINALSLIDE,
    "This tour shows just a small part of what you can see in the world of HyperRogue. "
    "For example, "
    "hyperbolic mazes are much nicer than their Euclidean counterparts. "
    "Have fun exploring!\n\n"
    "Press '5' to leave the tutorial mode.",
    [] (presmode mode) {
      slidecommand = "leave the Tutorial";
      if(mode == 4) restart_game(rg::tour);
      }
    }
  };

slide *slides = default_slides;

auto a1 = addHook(hooks_frame, 100, [] () { if(tour::on) tour::presentation(tour::pmFrame); });
auto a2 = addHook(hooks_handleKey, 100, handleKeyTour);
auto a3 = addHook(hooks_nextland, 100, [] (eLand l) { return tour::on ? getNext(l) : laNone; });

}}
