// HyperRogue menus
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

#include "dialogs.cpp"

#define BLACKISH 0x404040
#define REDDISH 0x400000

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
      displayfrZ(1, i0, 1, vf-4, "*", 0xFFFFFF, 0);
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
  }

int PREC(ld x) {
  ld sh = shiftmul;
  if(sh > -.2 && sh < .2) x = 10.01; 
  return int(shiftmul * x);
  }

void handleOverview(int sym, int uni) {
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
      cmode = emNormal;
      canmove = true;
      if(princ) fullcenter();
      }
    else {
      lastmode = cmode;
      cmode = emHelp; help = generateHelpForLand(eLand(umod));
      }
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
    else {
      lastmode = cmode;
      cmode = emHelp; help = generateHelpForItem(eItem(umod));
      }
    }
  else if(udiv == 3 && umod < walltypes) {
    lastmode = cmode;
    cmode = emHelp; help = generateHelpForWall(eWall(umod));
    }
  else if(uni == SDLK_F1) {
    lastmode = cmode;
    cmode = emHelp; 
    help = 
      "This displays all lands available in the game. "
      "Bonus lands (available only in separate challenges) "
      "are not included. Lands written in dark have to be "
      "unlocked, and lands in dark red are unavailable "
      "because of using special options. Click on any "
      "land or item to get information about it. Hover over "
      "an Orb to know its relation to the current land. "
      "Cheaters can click to move between lands, and use the "
      "mousewheel to gain or lose treasures and orbs quickly (Ctrl = precise, Shift = reverse).";
    }
  else if(dialog::handlePageButtons(uni)) ;
  else if(uni) cmode = emNormal;
  }

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

  getcstat = ' ';
  
  dialog::init(XLAT("HyperRogue %1", VER), 0xC00000, 200, 100);

  dialog::addItem(XLAT("basic configuration"), 'b');
  dialog::addItem(XLAT("advanced configuration"), 'a');
#ifndef NOSAVE
  dialog::addItem(XLAT("local highscores"), 't');
#endif
  dialog::addItem(XLAT("help"), 'h'); dialog::lastItem().keycaption += " / F1";
  if(cheater)
    dialog::addItem(XLAT("cheats"), 'c');
  else dialog::addBreak(100);
  dialog::addItem(XLAT("restart game"), 'r'); dialog::lastItem().keycaption += " / F5";
  dialog::addItem(XLAT("special game modes"), 'm');
  
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
    q = "review the scene";
  dialog::addItem(XLAT(q), SDLK_ESCAPE);
  dialog::addItem(XLAT("world overview"), 'o');    

  if(checkHalloweenDate()) dialog::addItem(XLAT("Halloween mini-game"), 'y'-96);

#ifdef ROGUEVIZ
  dialog::addItem(XLAT("rogueviz menu"), 'g'); 
#endif
  
#ifdef MOBILE
#ifdef HAVE_ACHIEVEMENTS
  dialog::addItem(XLAT("leaderboards/achievements"), '3'); 
#endif
#endif

#ifdef ANDROIDSHARE
  dialog::addItem("SHARE", 's'-96);
#endif
  
  if(!canmove) q = "game over screen";
  else if(turncount > 0) q = "continue game";
  else q = "play the game!";
  
  dialog::addItem(XLAT(q), ' ');
  dialog::display();
  }

void handleMenuKey(int sym, int uni) {
  dialog::handleNavigation(sym, uni);
  if(sym == SDLK_F1 || sym == 'h') {
    lastmode = cmode;
    cmode = emHelp;
    }
  else if(sym == 'c' && cheater)
    cmode = emCheatMenu;
  else if(sym == 'b') cmode = emVisual1;
  else if(sym == 'a') cmode = emVisual2;
  else if(sym == 'm') cmode = emChangeMode;
#ifndef NOSAVE
  else if(sym == 't') loadScores();
#endif
  else if(uni == 'y'-96) {
    if(!sphere) {
      euclidland = laHalloween;
      restartGame('E');
      vid.alpha = 999;
      vid.scale = 998;
      cmode = emNormal;
      }
    else {
      restartGame('E');
      vid.alpha = 1;
      vid.scale = 1;
      cmode = emNormal;
      }
    }
  else if(sym == 'r' || sym == SDLK_F5) {
    restartGame();
    cmode = emNormal;
    }
  else if(sym == 'q' || sym == SDLK_F10) 
    quitmainloop = true;
  else if(sym == 'o') {
    clearMessages();
    cmode = emOverview;
    }
  else if(sym == SDLK_ESCAPE) {
    cmode = emQuit;
    achievement_final(false);
    }
#ifdef MOBILE
#ifdef HAVE_ACHIEVEMENTS
  else if(sym == '3') {
    achievement_final(false);
    cmode = emLeader;
    }
#endif
#endif
#ifdef ROGUEVIZ
  else if(uni == 'g') cmode = emRogueviz;
#endif
  else if(uni || sym == SDLK_F10) {
    cmode = emNormal;
    msgs.clear();
    }
  }

void showVisual1() {
  dialog::init(XLAT("basic configuration"));
      
#ifndef MOBILE
  dialog::addSelItem(XLAT("video resolution"), its(vid.xres) + "x"+its(vid.yres), 0);
#endif
#ifndef IOS
  dialog::addBoolItem(XLAT("fullscreen mode"), (vid.full), 'f');
#endif
  dialog::addSelItem(XLAT("scrolling speed"), fts(vid.sspeed), 'a');
  dialog::addSelItem(XLAT("movement animation speed"), fts(vid.mspeed), 'r');
  dialog::addSelItem(XLAT("projection"), fts(vid.alpha), 'p');
  dialog::addSelItem(XLAT("scale factor"), fts(vid.scale), 'z');

  const char *wdmodes[6] = {"ASCII", "black", "plain", "Escher", "plain/3D", "Escher/3D"};
  const char *mdmodes[6] = {"ASCII", "items only", "items and monsters", "high contrast",
    "3D", "high contrast/3D"};

  const char *glyphsortnames[6] = {
    "first on top", "first on bottom", 
    "last on top", "last on bottom",
    "by land", "by number"
    };


  dialog::addBoolItem(XLAT("mark heptagons"), (vid.darkhepta), '7');
  dialog::addBoolItem(XLAT("draw the grid"), (vid.grid), '6');
  dialog::addBoolItem(XLAT("extra graphical effects"), (vid.particles), 'u');
  
  dialog::addSelItem(XLAT("wall display mode"), XLAT(wdmodes[vid.wallmode]), 'w');
  dialog::addSelItem(XLAT("monster display mode"), XLAT(mdmodes[vid.monmode]), 'm');
  const char *axmodes[5] = {"OFF", "auto", "light", "heavy", "arrows"};
  dialog::addSelItem(XLAT("help for keyboard users"), XLAT(axmodes[vid.axes]), 'c');
#ifndef NOAUDIO
  dialog::addSelItem(XLAT("background music volume"), its(musicvolume), 'b');
  dialog::addSelItem(XLAT("sound effects volume"), its(effvolume), 'e');
#endif
#ifndef NOTRANS
  dialog::addSelItem(XLAT("language"), XLAT("EN"), 'l');
#endif
  dialog::addSelItem(XLAT("player character"), numplayers() > 1 ? "" : csname(vid.cs), 'g');

  dialog::addSelItem(XLAT("inventory/kill sorting"), XLAT(glyphsortnames[glyphsortorder]), 'k');

  dialog::addBreak(50);
  dialog::addItem(XLAT("exit configuration"), 'v');
#ifndef NOCONFIG
  dialog::addItem(XLAT("save the current config"), 's');
#endif

  if(lang() != 0) {
    string tw = "";
    string s = XLAT("TRANSLATIONWARNING");
    if(s != "" && s != "TRANSLATIONWARNING") tw += s;
    s = XLAT("TRANSLATIONWARNING2");
    if(s != "" && s != "TRANSLATIONWARNING2") { if(tw != "") tw += " "; tw += s; }
    if(tw != "") {
      dialog::addBreak(50);
      dialog::addHelp(tw);
      dialog::lastItem().color = 0xFF0000;
      }
    }

  dialog::display();
  }

void projectionDialog() {
  geom3::tc_alpha = ticks;
  dialog::editNumber(vid.alpha, -5, 5, .1, 1,
    XLAT("projection"),
    XLAT("HyperRogue uses the Minkowski hyperboloid model internally. "
    "Klein and Poincaré models can be obtained by perspective, "
    "and the Gans model is obtained by orthogonal projection. "
//  "This parameter specifies the distance from the hyperboloid center "
//  "to the eye. "
    "See also the conformal mode (in the special modes menu) "
    "for more models."));
  dialog::sidedialog = true;
  }

void handleVisual1(int sym, int uni) {
  dialog::handleNavigation(sym, uni);
    
  char xuni = uni | 96;
  
  if(uni >= 32 && uni < 64) xuni = uni;
  
  if(xuni == 'p') projectionDialog();
  
  if(xuni == '6') vid.grid = !vid.grid;
  if(xuni == 'u') vid.particles = !vid.particles;
      
  if(xuni == 'z') {
    dialog::editNumber(vid.scale, .001, 1000, .1, 1, XLAT("scale factor"), 
      XLAT("Scale the displayed model."));
    dialog::scaleLog();
    dialog::sidedialog = true;
    }
  
  if(xuni == 'a') dialog::editNumber(vid.sspeed, -5, 5, 1, 0, 
    XLAT("scrolling speed"),
    XLAT("+5 = center instantly, -5 = do not center the map"));

  if(xuni == 'r') dialog::editNumber(vid.mspeed, -5, 5, 1, 0, 
    XLAT("movement animation speed"),
    XLAT("+5 = move instantly"));

  if(xuni == 'k') {
    glyphsortorder = eGlyphsortorder((glyphsortorder+6+(shiftmul>0?1:-1)) % gsoMAX);
    }
  
  if(xuni == 'f') {
    vid.full = !vid.full;
#ifdef ANDROID
    addMessage(XLAT("Reenter HyperRogue to apply this setting"));
    settingsChanged = true;
#endif
#ifndef MOBILE
    if(true) {
      vid.xres = vid.full ? vid.xscr : 9999;
      vid.yres = vid.full ? vid.yscr : 9999;
      extern bool setfsize;
      setfsize = true;
      }
    setvideomode();
#endif
    }
  
  if(xuni == 'v') cmode = emNormal;
  if(sym == SDLK_F2) cmode = emVisual2;
#ifndef NOCONFIG
  if(xuni == 's') saveConfig();
#endif
  
  if(xuni == '7') { vid.darkhepta = !vid.darkhepta; }
  if(xuni == 'w') { vid.wallmode += 60 + (shiftmul > 0 ? 1 : -1); vid.wallmode %= 6; }
  if(xuni == 'm') { vid.monmode += 60 + (shiftmul > 0 ? 1 : -1); vid.monmode %= 6; }
  if(xuni == 'c') { vid.axes += 60 + (shiftmul > 0 ? 1 : -1); vid.axes %= 5; }
#ifndef NOAUDIO
  if(xuni == 'b') {
    dialog::editNumber(musicvolume, 0, 128, 10, 60, XLAT("background music volume"), "");
    }
  if(xuni == 'e') {
    dialog::editNumber(effvolume, 0, 128, 10, 60, XLAT("sound effects volume"), "");
    }
#endif
  
  if(sym == SDLK_ESCAPE) cmode = emNormal;
  
#ifndef NOTRANS
  if(xuni == 'l') {
    vid.language += (shiftmul>0?1:-1);
    vid.language %= NUMLAN;
    if(vid.language < 0) vid.language += NUMLAN;
#ifdef ANDROID
    settingsChanged = true;
#endif
    }
#endif
  
  if(xuni == 'g') cmode = emCustomizeChar;

#ifdef LOCAL  
  extern void process_local(int);
  process_local(sym);
#endif
  }

void showJoystickConfig() {

  dialog::init(XLAT("joystick configuration"));
  
  dialog::addSelItem(XLAT("first joystick position (movement)"), its(joyx)+","+its(joyy), 0);
  dialog::addSelItem(XLAT("second joystick position (panning)"), its(panjoyx)+","+its(panjoyy), 0);
  
#ifndef MOBWEB
  dialog::addSelItem(XLAT("joystick mode"), XLAT(autojoy ? "automatic" : "manual"), 'p');
    
  dialog::addSelItem(XLAT("first joystick: movement threshold"), its(vid.joyvalue), 'a');
  dialog::addSelItem(XLAT("first joystick: execute movement threshold"), its(vid.joyvalue2), 'b');
  dialog::addSelItem(XLAT("second joystick: pan threshold"), its(vid.joypanthreshold), 'c');
  dialog::addSelItem(XLAT("second joystick: panning speed"), fts(vid.joypanspeed * 1000), 'd');
#endif

  dialog::addItem(XLAT("back"), 'v');
  dialog::display();
  }

void handleJoystickConfig(int sym, int uni) {
  dialog::handleNavigation(sym, uni);
  char xuni = uni | 96;
  if(xuni == 'p') autojoy = !autojoy;
  if(xuni == 'a') 
    dialog::editNumber(vid.joyvalue, 0, 32768, 100, 4800, XLAT("first joystick: movement threshold"), "");
  if(xuni == 'b') 
    dialog::editNumber(vid.joyvalue2, 0, 32768, 100, 5600, XLAT("first joystick: execute movement threshold"), "");
  if(xuni == 'c')
    dialog::editNumber(vid.joypanthreshold, 0, 32768, 100, 2500, XLAT("second joystick: pan threshold"), "");
  if(xuni == 'd') 
    dialog::editNumber(vid.joypanspeed, 0, 1e-2, 1e-5, 1e-4, XLAT("second joystick: panning speed"), "");

  if(xuni == 'v') 
    cmode = emShmupConfig;
  
  if(sym == SDLK_F10) cmode = emNormal;
  if(uni) cmode = emShmupConfig;
  }

void show3D() {
  using namespace geom3;
  dialog::init(XLAT("3D configuration"));

  dialog::addSelItem(XLAT("High detail range"), fts(highdetail), 'n');
  dialog::addSelItem(XLAT("Mid detail range"), fts(middetail), 'm');
  
  dialog::addBreak(50);
  dialog::addSelItem(XLAT("Camera level above the plane"), fts3(camera), 'c');
  dialog::addSelItem(XLAT("Ground level below the plane"), fts3(depth), 'g');

  dialog::addSelItem(XLAT("Projection at the ground level"), fts3(vid.alpha), 'a');
  dialog::addBreak(50);
  dialog::addSelItem(XLAT("Height of walls"), fts3(wall_height), 'w');
  
  dialog::addSelItem(XLAT("Rock-III to wall ratio"), fts3(rock_wall_ratio), 'r');
  dialog::addSelItem(XLAT("Human to wall ratio"), fts3(human_wall_ratio), 'h');
  dialog::addSelItem(XLAT("Level of water surface"), fts3(lake_top), 'l');
  dialog::addSelItem(XLAT("Level of water bottom"), fts3(lake_bottom), 'k');  

  dialog::addBreak(50);
  dialog::addSelItem(XLAT("Y shift"), fts3(vid.yshift), 'y');
  dialog::addSelItem(XLAT("camera rotation"), fts3(vid.camera_angle), 's');
  dialog::addBreak(50);
  dialog::addBoolItem(XLAT("ball model"), pmodel == mdBall, 'B');
  dialog::addBoolItem(XLAT("hyperboloid model"), pmodel == mdHyperboloid, 'M');
  dialog::addSelItem(XLAT("camera rotation in ball model"), fts3(vid.ballangle), 'b');
  dialog::addSelItem(XLAT("projection in ball model"), fts3(vid.ballproj), 'x');

  dialog::addBreak(50);
  if(!(wmspatial || mmspatial))
    dialog::addInfo(XLAT("set 3D monsters or walls in basic config first"));
  else if(invalid != "")
    dialog::addInfo(XLAT("error: "+invalid));
  else
    dialog::addInfo(XLAT("parameters set correctly"));
  dialog::addBreak(50);
  dialog::addItem(XLAT("exit 3D configuration"), 'v');
  dialog::display();
  }

string explain3D(ld *param) {
  using namespace geom3;
  if(param == &highdetail || param == &middetail)
    return 
      XLAT(
        "Objects at distance less than %1 absolute units "
        "from the center will be displayed with high "
        "detail, and at distance at least %2 with low detail.",
        fts3(highdetail), fts3(middetail)
        );
      
  if(param == &camera)
    return
      XLAT(
        "Camera is placed %1 absolute units above a plane P in a three-dimensional "
        "world. Ground level is actually an equidistant surface, %2 absolute units "
        "below the plane P. The plane P (as well as the ground level or any "
        "other equidistant surface below it) is viewed at an angle of %3 "
        "(the tangent of the angle between the point in "
        "the center of your vision and a faraway location is 1/cosh(c) = %4).",
        fts3(camera),
        fts3(depth),
        fts3(atan(1/cosh(camera))*2*180/M_PI),
        fts3(1/cosh(camera)));
  if(param == &depth)
    return
      XLAT(
        "Ground level is actually an equidistant surface, "
        "%1 absolute units below the plane P. "
        "Theoretically, this value affects the world -- "
        "for example, eagles could fly %2 times faster by "
        "flying above the ground level, on the plane P -- "
        "but the actual game mechanics are not affected. "
        "(Distances reported by the vector graphics editor "
        "are not about points on the ground level, but "
        "about the matching points on the plane P -- "
        "divide them by the factor above to get actual "
        "distances.)"
        
        ,
        fts3(depth), fts3(cosh(depth)));
        // mention absolute units
        
  if(param == &vid.alpha)
    return
      XLAT(
        "If we are viewing an equidistant g absolute units below a plane, "
        "from a point c absolute units above the plane, this corresponds "
        "to viewing a Minkowski hyperboloid from a point "
        "tanh(g)/tanh(c) units below the center. This in turn corresponds to "
        "the Poincaré model for g=c, and Klein-Beltrami model for g=0.");
      
  if(param == &wall_height)
    return
      XLAT(
        "The height of walls, in absolute units. For the current values of g and c, "
        "wall height of %1 absolute units corresponds to projection value of %2.",
        fts3(wall_height), fts3(factor_to_projection(geom3::WALL)));

  if(param == &rock_wall_ratio)
    return
      XLAT(
        "The ratio of Rock III to walls is %1, so Rock III are %2 absolute units high. "
        "Length of paths on the Rock III level is %3 of the corresponding length on the "
        "ground level.",
        fts3(rock_wall_ratio), fts3(wall_height * rock_wall_ratio),
        fts3(cosh(depth - wall_height * rock_wall_ratio) / cosh(depth)));

  if(param == &human_wall_ratio)
    return
      XLAT(
        "Humans are %1 "
        "absolute units high. Your head travels %2 times the distance travelled by your "
        "feet.",
        fts3(wall_height * human_wall_ratio),
        fts3(cosh(depth - wall_height * human_wall_ratio) / cosh(depth)));
        
  return "";
  }

void handle3D(int sym, int uni) {
  using namespace geom3;
  dialog::handleNavigation(sym, uni);
  
  if(uni == 'n') 
    dialog::editNumber(geom3::highdetail, 0, 5, .5, 7, XLAT("High detail range"), "");
  else if(uni == 'm') 
    dialog::editNumber(geom3::middetail, 0, 5, .5, 7, XLAT("Mid detail range"), "");
  else if(uni == 'c') 
    tc_camera = ticks,
    dialog::editNumber(geom3::camera, 0, 5, .1, 1, XLAT("Camera level above the plane"), "");
  else if(uni == 'g') 
    tc_depth = ticks,
    dialog::editNumber(geom3::depth, 0, 5, .1, 1, XLAT("Ground level below the plane"), "");
  else if(uni == 'a') 
    projectionDialog();
  else if(uni == 'w') 
    dialog::editNumber(geom3::wall_height, 0, 1, .1, .3, XLAT("Height of walls"), "");
  else if(uni == 'l') 
    dialog::editNumber(geom3::lake_top, 0, 1, .1, .25, XLAT("Level of water surface"), "");
  else if(uni == 'k') 
    dialog::editNumber(geom3::lake_bottom, 0, 1, .1, .9, XLAT("Level of water bottom"), "");
  else if(uni == 'r') 
    dialog::editNumber(geom3::rock_wall_ratio, 0, 1, .1, .9, XLAT("Rock-III to wall ratio"), "");
  else if(uni == 'h') 
    dialog::editNumber(geom3::human_wall_ratio, 0, 1, .1, .7, XLAT("Human to wall ratio"), "");
  else if(uni == 'y') 
    dialog::editNumber(vid.yshift, 0, 1, .1, 0, XLAT("Y shift"), 
      "Don't center on the player character."
      );
  else if(uni == 's') 
    dialog::editNumber(vid.camera_angle, -180, 180, 5, 0, XLAT("camera rotation"), 
      "Rotate the camera. Can be used to obtain a first person perspective, "
      "or third person perspective when combined with Y shift."
      );
  else if(uni == 'b') 
    dialog::editNumber(vid.ballangle, 0, 90, 5, 0, XLAT("camera rotation in ball model"), 
      "Rotate the camera in ball/hyperboloid model.");
  else if(uni == 'x') 
    dialog::editNumber(vid.ballproj, 0, 100, .1, 0, XLAT("projection in ball model"), 
      "This parameter affects the ball model the same way as the projection parameter affects the disk model.");
  else if(uni == 'B') 
    pmodel = (pmodel == mdBall ? mdDisk : mdBall);
  else if(uni == 'M') 
    pmodel = (pmodel == mdHyperboloid ? mdDisk : mdHyperboloid);

  else if(uni) cmode = emVisual2;
  
  if(cmode == emNumber) dialog::sidedialog = true;
  }

void showVisual2() {

  dialog::init(XLAT("advanced configuration"));

  #ifndef ONEGRAPH
  #ifdef GL
  dialog::addSelItem(XLAT("openGL & antialiasing mode"), vid.usingGL ? "OpenGL" : vid.usingAA ? "AA" : "OFF", 'o');
  #endif
  #endif
  dialog::addSelItem(XLAT("distance between eyes"), fts3(vid.eye), 'e');
  #ifndef MOBWEB
  dialog::addSelItem(XLAT("framerate limit"), its(vid.framelimit), 'f');
  #endif

#ifndef MOBILE
  dialog::addSelItem(XLAT("configure input"), "", 'p');
#endif

#ifdef MOBILE
  dialog::addSelItem(XLAT("font scale"), its(fontscale), 'b');
#endif

  dialog::addSelItem(XLAT("sight range"), its(sightrange), 'a');
  dialog::addBoolItem(XLAT("reverse pointer control"), (revcontrol), 'r');
  dialog::addBoolItem(XLAT("draw circle around the target"), (vid.drawmousecircle), 'd');
  
  dialog::addSelItem(XLAT("message flash time"), its(vid.flashtime), 't');
#ifdef MOBILE
  dialog::addBoolItem(XLAT("targetting ranged Orbs long-click only"), (vid.shifttarget&2), 'i');
#else
  dialog::addBoolItem(XLAT("targetting ranged Orbs Shift+click only"), (vid.shifttarget&1), 'i');
#endif
#ifdef STEAM
  dialog::addBoolItem(XLAT("send scores to Steam leaderboards"), (vid.steamscore&1), 'l');
#endif
  dialog::addSelItem(XLAT("3D configuration"), "", '3');
#ifdef MOBILE
  dialog::addSelItem(XLAT("compass size"), its(vid.mobilecompasssize), 'c');
#endif

  dialog::addBreak(50);
  dialog::addItem(XLAT("exit configuration"), 'v');
#ifndef NOCONFIG
  dialog::addItem(XLAT("save the current config"), 's');
#endif

  dialog::display();
  }


void handleVisual2(int sym, int uni) {
  dialog::handleNavigation(sym, uni);
  char xuni = uni;
  if((xuni >= 'A' && xuni <= 'Z') || (xuni >= 1 && xuni <= 26)) xuni |= 32;
  
  if(xuni == 'v' || sym == SDLK_F2 || sym == SDLK_ESCAPE) cmode = emNormal;
#ifndef NOCONFIG
  if(xuni == 's') saveConfig();
#endif
  
  if(xuni == '3') cmode = em3D;
  
  if(xuni == 'c') 
    dialog::editNumber(vid.mobilecompasssize, 0, 100, 10, 20, XLAT("compass size"), "");
  
  if(sym == SDLK_F1 || sym == 'h')
    lastmode = cmode, cmode = emHelp;

#ifndef ONEGRAPH
  if(xuni == 'o' && shiftmul > 0) {
    vid.usingGL = !vid.usingGL;
    if(vid.usingGL) addMessage(XLAT("openGL mode enabled"));
    if(!vid.usingGL) addMessage(XLAT("openGL mode disabled"));
#ifndef ANDROID
    if(!vid.usingGL) addMessage(XLAT("shift+O to switch anti-aliasing"));
#endif
#ifdef ANDROID
    settingsChanged = true;
#else
#ifndef FAKEMOBILE
    setvideomode();
#endif
#endif
    }

#ifndef MOBILE
  if(xuni == 'o' && shiftmul < 0 && !vid.usingGL) {
    vid.usingAA = !vid.usingAA;
    if(vid.usingAA) addMessage(XLAT("anti-aliasing enabled"));
    if(!vid.usingAA) addMessage(XLAT("anti-aliasing disabled"));
    }
#endif
#endif

#ifndef MOBILE
  if(xuni == 'p') {
    cmode = emShmupConfig; 
    multi::shmupcfg = shmup::on;
    }
#endif
  
  if(xuni == 'f') 
    dialog::editNumber(vid.framelimit, 5, 300, 10, 300, XLAT("framerate limit"), "");
  
  if(xuni == 'a') {
    dialog::editNumber(sightrange, 4, cheater ? 10 : 7, 1, 7, XLAT("sight range"), 
      XLAT("Roughly 42% cells are on the edge of your sight range. Reducing "
      "the sight range makes HyperRogue work faster, but also makes "
      "the game effectively harder."));
    dialog::sidedialog = true;
    }

  if(xuni == 'r') revcontrol = !revcontrol;
  if(xuni == 'd') vid.drawmousecircle = !vid.drawmousecircle;

#ifdef MOBILE
  if(xuni =='b') 
    dialog::editNumber(fontscale, 0, 400, 10, 100, XLAT("font scale"), "");
#endif

  if(xuni == 'e') {
    dialog::editNumber(vid.eye, -10, 10, 0.01, 0, XLAT("distance between eyes"),
      XLAT("Watch the Minkowski hyperboloid or the hypersian rug mode with the "
      "red/cyan 3D glasses."));
    dialog::sidedialog = true;
    }

#ifdef STEAM
  if(xuni == 'l') vid.steamscore = vid.steamscore^1;
#endif
  if(xuni == 't') 
    dialog::editNumber(vid.flashtime, 0, 64, 1, 8, XLAT("message flash time"),
      XLAT("How long should the messages stay on the screen."));
  
  if(xuni == 'i') { vid.shifttarget = vid.shifttarget^3; }
  }

void showChangeMode() {
  dialog::init(XLAT("special game modes"));
  
  // gameplay modes

#ifdef TOUR
  dialog::addBoolItem(XLAT("tutorial"), tour::on, 'T');
#endif

  dialog::addBoolItem(XLAT("Euclidean/elliptic mode"), (euclid || sphere), 'e');
  dialog::addBoolItem(XLAT(SHMUPTITLE), (shmup::on || multi::players > 1), 's');
  if(!shmup::on) dialog::addSelItem(XLAT("hardcore mode"),
    hardcore && !pureHardcore() ? XLAT("PARTIAL") : ONOFF(hardcore), 'h');
  
  multi::cpid = 0;
  dialog::addBoolItem(XLAT("%1 Challenge", moPrincess), (princess::challenge), 'p');
  dialog::addBoolItem(XLAT("random pattern mode"), (randomPatternsMode), 'r');
  dialog::addBoolItem(XLAT("Yendor Challenge"), (yendor::on), 'y');
  dialog::addBoolItem(XLAT("pure tactics mode"), (tactic::on), 't');
  dialog::addBoolItem(XLAT("heptagonal mode"), (purehepta), '7');
  dialog::addBoolItem(XLAT("Chaos mode"), (chaosmode), 'C');

  dialog::addBreak(50);
  // cheating and map editor

  dialog::addBoolItem(XLAT("cheat mode"), (cheater), 'c');
#ifndef NOEDIT
  dialog::addBoolItem(XLAT("map editor"), (false), 'm');
  dialog::addBreak(50);
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
  
  dialog::addBreak(50);
  
  dialog::addItem(XLAT("return to the game"), 'v');
  dialog::display();
  }

void handleChangeMode(int sym, int uni) {
  dialog::handleNavigation(sym, uni);    
  char xuni = uni;
  
  if(xuni == 'v' || sym == SDLK_F2 || sym == SDLK_ESCAPE) cmode = emNormal;
  
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
      cmode = emNormal;
      }
    else {
      cmode = emNormal;
      firstland = princess::challenge ? laPalace : laIce;
      restartGame();
      }
    }
  
#ifndef NOEDIT
  else if(xuni == 'g') {
    cmode = emDraw;
    mapeditor::drawcell = cwt.c;
  }
#endif
  else if(xuni == 'e') {
    cmode = emPickEuclidean;
  }
  else if(xuni == 'x') {
    viewdists = !viewdists;
    cmode = emNormal;
    }
  else if(xuni == 't') {
    clearMessages();
    cmode = emTactic;
  }
#ifndef NORUG
  else if(xuni == 'u') {
    if(sphere) projectionDialog();
    else rug::select();
    }
#endif
  else if(xuni == 'y') {
    clearMessages();
    if(yendor::everwon || autocheat)
        cmode = emYendor;
    else {
      cmode = emHelp;
      help = yendor::chelp;
      lastmode = emChangeMode;
      }
    }
  else if(xuni == '7')
      restartGame('7');
  else if(uni == 'a')
      cmode = emConformal;
#ifdef TOUR
  else if(uni == 'T') {
    cmode = emNormal;
    tour::start();
    }
#endif
  else if(uni == 'C') {
    if(!chaosmode) {
      cmode = emHelp;
      help =
      "In the Chaos mode, lands change very often, and "
      "there are no walls between them. "
      "Some lands are incompatible with this."
      "\n\nYou need to reach Crossroads IV to unlock the Chaos mode.";
      lastmode = chaosUnlocked ? emNormal : emChangeMode;
      }
    if(chaosUnlocked) restartGame('C');
    }
  else if(xuni == 'p') {
    if(!princess::everSaved)
      addMessage(XLAT("Save %the1 first to unlock this challenge!", moPrincess));
    else {
      restartGame('p');
      cmode = emNormal;
      }
    }
#ifndef NOEDIT
  else if(xuni == 'm') {
    if(tactic::on) 
      addMessage(XLAT("Not available in the pure tactics mode!"));
    else {
      cheater++;
      cmode = emMapEditor;
      lastexplore = turncount;
      addMessage(XLAT("You activate your terraforming powers!"));
      }
    }
#endif
  else if(xuni == 's') {
#ifdef MOBILE
    restartGame('s');
    cmode = emNormal;
#else
    multi::shmupcfg = shmup::on;
    cmode = emShmupConfig;
#endif
    }
#ifndef NOMODEL
  else if(xuni == 'n')
    cmode = emNetgen;
#endif
  else if(xuni == 'h' && !shmup::on) {
    if(hardcore && !canmove) { 
      restartGame();
      hardcore = false;
      cmode = emNormal;
      }
    else if(hardcore && canmove) { hardcore = false; }
    else { hardcore = true; canmove = true; hardcoreAt = turncount; }
    if(hardcore)
        addMessage("One wrong move, and it is game over!");
    else
        addMessage("Not so hardcore?");
    if(pureHardcore()) cmode = emNormal;
    }
  else if(xuni == 'r') {
    firstland = laIce;
    restartGame('r');
    cmode = emNormal;
    }
  else if(uni || sym == SDLK_F10)
    cmode = emMenu;
  }

void showCheatMenu() {
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
  }

void handleCheatMenu(int sym, int uni) {
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
      cmode = emNormal;
    }
  }

void showCustomizeChar() {
  dialog::init(XLAT("Customize character"));
  
  if(shmup::on || multi::players) shmup::cpid = shmup::cpid_edit % shmup::players;
  charstyle& cs = getcs();
  
  dialog::addSelItem(XLAT("character"), csname(cs), 'g');
  dialog::addColorItem(XLAT("skin color"), cs.skincolor, 's');
  dialog::addColorItem(XLAT("weapon color"), cs.swordcolor, 'w');
  dialog::addColorItem(XLAT("hair color"), cs.haircolor, 'h');
  
  if(cs.charid >= 1) dialog::addColorItem(XLAT("dress color"), cs.dresscolor, 'd');
  else dialog::addBreak(100);
  if(cs.charid == 3) dialog::addColorItem(XLAT("dress color II"), cs.dresscolor2, 'f');
  else dialog::addBreak(100);
  
  dialog::addColorItem(XLAT("movement color"), cs.uicolor, 'u');

  if(!shmup::on && multi::players == 1) dialog::addSelItem(XLAT("save whom"), XLAT1(minf[moPrincess].name), 'p');
  
  if(numplayers() > 1) dialog::addSelItem(XLAT("player"), its(shmup::cpid+1), 'a');
  
  dialog::addBreak(50);
  dialog::addItem(XLAT("return to the game"), 'v');
  dialog::display();
  }

void switchcolor(int& c, unsigned int* cs) {
  dialog::openColorDialog(c, cs);
  }

void handleCustomizeChar(int sym, int uni) {
  dialog::handleNavigation(sym, uni);
  char xuni = uni | 96;

  if(shmup::on || multi::players) shmup::cpid = shmup::cpid_edit % shmup::players;
  charstyle& cs = getcs();
  if(xuni == 'a') { shmup::cpid_edit++; shmup::cpid_edit %= 60; }
  if(xuni == 'g') {
    cs.charid++;
    if(cs.charid == 2 && !princess::everSaved) cs.charid = 4;
    cs.charid %= 10;
    }
  if(xuni == 'p') vid.samegender = !vid.samegender;
  bool cat = cs.charid >= 4;
  if(xuni == 's') switchcolor(cs.skincolor, cat ? haircolors : skincolors);
  if(xuni == 'h') switchcolor(cs.haircolor, haircolors);
  if(xuni == 'w') switchcolor(cs.swordcolor, cat ? eyecolors : swordcolors);
  if(xuni == 'd') switchcolor(cs.dresscolor, cat ? haircolors : dresscolors);
  if(xuni == 'f') switchcolor(cs.dresscolor2, dresscolors2);
  if(xuni == 'u') switchcolor(cs.uicolor, eyecolors);
  if(xuni == 'v' || sym == SDLK_F2 || sym == SDLK_ESCAPE) cmode = emNormal;
  }

int eupage = 0;
int euperpage = 21;

#define LAND_SPHEUC ((targetgeometry > 1) ? LAND_SPH : LAND_EUC)
#define land_spheuc ((targetgeometry > 1) ? land_sph : land_euc)

const char* geometrynames[gGUARD] = {
  "hyperbolic", "Euclidean", "spherical", "elliptic", 
  "Zebra quotient", "field quotient"
  };  

void showEuclideanMenu() {
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
  }

void handleEuclidean(int sym, int uni) {
  dialog::handleNavigation(sym, uni);
  int lid;
  if(uni >= 'a' && uni <= 'z') lid = uni - 'a';
  else if(uni >= 'A' && uni <= 'Z') lid = 26 + uni - 'A';
  else lid = -1;
  
  if(lid >= 0) lid += euperpage * eupage;
  
  if(uni == '0') {
    targetgeometry = geometry;
    restartGame('g');
    cmode = emNormal;
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
      // disable PTM if chosen a land from the Euclidean menu
      if(tactic::on) restartGame('t');
      cmode = emNormal;
      }
    else euclidland = laIce;
    }
  else if(uni == '2' || sym == SDLK_F1) {
    help =
    "If you want to know how much the gameplay is affected by the "
    "hyperbolic geometry in HyperRogue, this mode is for you!\n\n"
    
    "You can play an Euclidean version of each of the lands in "
    "HyperRogue. Lands which include horocycles (Temple, Caribbean, "
    "Whirlpool), infinite trees (Zebra, Emerald), or networks of "
    "ultraparallel lines (Crossroads, Vineyard, Palace) cannot be "
    "faithfully represented in Euclidean, so yo get more "
    "or less simplified versions of them. Choose Crossroads to play a game "
    "where many different lands appear.";
    cmode = emHelp;
    lastmode = emPickEuclidean;
    }
  else if(uni || sym == SDLK_F10) cmode = emNormal;
  }

#ifdef MOBILE
namespace leader { void showMenu(); void handleKey(int sym, int uni); }
#endif

#ifndef NOSAVE
void showScores() {
  int y = vid.fsize * 7/2;
  int bx = vid.fsize;
  getcstat = 1;
  
  string modes = 
    scoremode == 0 ? XLAT(", m - mode: normal") :
    scoremode == 1 ? XLAT(", m - mode: shoot'em up") :
    scoremode == 2 ? XLAT(", m - mode: hardcore only") :
    "?";

  if(euclid) modes += XLAT(" (E:%1)", euclidland);


  mouseovers = XLAT("t/left/right - change display, up/down - scroll, s - sort by") + modes;

  displaystr(bx*4, vid.fsize*2, 0, vid.fsize, "#", 0xFFFFFF, 16);
  displaystr(bx*8, vid.fsize*2, 0, vid.fsize, "$$$", 0xFFFFFF, 16);
  displaystr(bx*12, vid.fsize*2, 0, vid.fsize, XLAT("kills"), 0xFFFFFF, 16);
  displaystr(bx*18, vid.fsize*2, 0, vid.fsize, XLAT("time"), 0xFFFFFF, 16);
  displaystr(bx*22, vid.fsize*2, 0, vid.fsize, XLAT("ver"), 0xFFFFFF, 16);
  displaystr(bx*23, vid.fsize*2, 0, vid.fsize, displayfor(NULL), 0xFFFFFF, 0);
  if(scorefrom < 0) scorefrom = 0;
  int id = 0;
  int omit = scorefrom;
  int rank = 0;
  while(y < (ISMOBILE ? vid.yres - 5*vid.fsize : vid.yres - 2 * vid.fsize)) { 
    if(id >= size(scores)) break;
        
    score& S(scores[id]);
    
    bool wrongtype = false;
    
    wrongtype |= (euclid && (!S.box[116] || S.box[120] != euclidland));
    wrongtype |= (!euclid && S.box[116]);

    wrongtype |= (scoremode == 1 && !S.box[119]);
    wrongtype |= (scoremode != 1 && S.box[119]);
    wrongtype |= (scoremode == 2 && (!S.box[117] || S.box[118] >= PUREHARDCORE_LEVEL));
    
    if(wrongtype) { id++; continue; }

    if(omit) { omit--; rank++; id++; continue; }

    char buf[16];
    
    rank++; sprintf(buf, "%d", rank);
    displaystr(bx*4,  y, 0, vid.fsize, buf, 0xC0C0C0, 16);
    
    sprintf(buf, "%d", S.box[2]);
    displaystr(bx*8,  y, 0, vid.fsize, buf, 0xC0C0C0, 16);
    
    sprintf(buf, "%d", S.box[3]);
    displaystr(bx*12,  y, 0, vid.fsize, buf, 0xC0C0C0, 16);

    sprintf(buf, "%d:%02d", S.box[0]/60, S.box[0] % 60);
    displaystr(bx*18,  y, 0, vid.fsize, buf, 0xC0C0C0, 16);

    displaystr(bx*22,  y, 0, vid.fsize, S.ver, 0xC0C0C0, 16);

    displaystr(bx*23, y, 0, vid.fsize, displayfor(&S), 0xC0C0C0, 0);
    
    y += vid.fsize*5/4; id++;
    }

#ifdef MOBILE
  buttonclicked = false;
  displayabutton(-1, +1, XLAT("SORT"), BTON);
  displayabutton( 0, +1, XLAT("PICK"), BTON);
  displayabutton(+1, +1, XLAT("PLAY"), BTON);
#endif
  }

void sortScores() {
  if(scorerev) reverse(scores.begin(), scores.end());
  else {
    scorerev = true;
    scoresort = scoredisplay; 
    stable_sort(scores.begin(), scores.end(), scorecompare);
    }
  }

void shiftScoreDisplay(int delta) {
  scoredisplay = (scoredisplay + POSSCORE + delta) % POSSCORE, scorerev = false;
  if(fakescore()) shiftScoreDisplay(delta);
  }

void handleScoreKeys(int sym) {
#ifndef MOBILE
  if(sym == SDLK_LEFT || sym == SDLK_KP4 || sym == 'h' || sym == 'a')
    shiftScoreDisplay(-1);
  else if(sym == SDLK_RIGHT || sym == SDLK_KP6 || sym == 'l' || sym == 'd')
    shiftScoreDisplay(1);
  else if(sym == 't') { mapeditor::infix = ""; cmode = emPickScores; }
  else if(sym == SDLK_UP || sym == 'k' || sym == 'w')
    scorefrom -= 5;
  else if(sym == SDLK_DOWN || sym == 'j' || sym == 'x')
    scorefrom += 5;
  else if(sym == PSEUDOKEY_WHEELUP)
    scorefrom--;
  else if(sym == PSEUDOKEY_WHEELDOWN)
    scorefrom++;
  else if(sym == 's') sortScores(); 
  else if(sym == 'm') { scoremode++; scoremode %= 3; }
  else if(sym != 0) cmode = emNormal;
#else
  static int scoredragx, scoredragy;
  extern bool clicked, lclicked;
  extern int andmode;
  
  if(andmode) { 
    if(!clicked && !lclicked) {
      andmode = 0;
      scoredragx = mousex;
      scoredragy = mousey;
      }
    }

  else {
  
    if(clicked && !lclicked)
      scoredragx = mousex, scoredragy = mousey;
  
    else if(lclicked && !clicked) {
      if(mousey > vid.ycenter - 2 * vid.fsize) {
        if(mousex < vid.xcenter*2/3) sortScores();
        else if(mousex < vid.xcenter*4/3)
          cmode = emPickScores;
        else andmode = 0, cmode = emNormal;
        }
      }
    
    else if(clicked && lclicked) {
  //  if(mousex > scoredragx + 80) scoredragx += 80, shiftScoreDisplay(1); 
  //  if(mousex < scoredragx - 80) scoredragx -= 80, shiftScoreDisplay(-1); 
      while(mousey > scoredragy + vid.fsize) scoredragy += vid.fsize, scorefrom--;
      while(mousey < scoredragy - vid.fsize) scoredragy -= vid.fsize, scorefrom++;
      }
    }
#endif
  }
        
bool monsterpage = false;

void handlePickScoreKeys(int uni) {
  extern int andmode;
  andmode = 2;
  if(uni == 'm') monsterpage = !monsterpage; else
  if(uni >= '1' && uni <= '9') uni = uni + 1000 - '1';
  else if(uni >= 1000 && uni < 1000 + size(pickscore_options)) { 
    cmode = emScores; 
    scoredisplay = pickscore_options[uni - 1000].second;
    }
  else if(mapeditor::editInfix(uni)) ;
  else if(uni) cmode = emScores;
  }
        
void showPickScores() {

  getcstat = '0';  
  int d = scoredisplay;
  
  pickscore_options.clear();

  scorerev = false;

  for(int i=0; i<POSSCORE; i++) {
    scoredisplay = i;
    if(!fakescore()) {
      string s = displayfor(NULL);
      if(mapeditor::hasInfix(s))
        if(monsbox[scoredisplay] == monsterpage)
          pickscore_options.push_back(make_pair(s, i));
      }
    }
  sort(pickscore_options.begin(), pickscore_options.end());
  
  int q = (int) pickscore_options.size();
  int percolumn = vid.yres / (vid.fsize+3) - 4;
  int columns = 1 + (q-1) / percolumn;
  
  for(int i=0; i<q; i++) {
    int x = 16 + (vid.xres * (i/percolumn)) / columns;
    int y = (vid.fsize+3) * (i % percolumn) + vid.fsize*2;
    
    scoredisplay = pickscore_options[i].second;
    if(q <= 9)
      pickscore_options[i].first = pickscore_options[i].first + " [" + its(i+1) + "]";
    if(!fakescore())
      displayButton(x, y, pickscore_options[i].first, 1000+i, 0);
    }
  
  displayButton(vid.xres/2, vid.yres - vid.fsize*2, "kills", 'm', 8);

  scoredisplay = d;

  mouseovers = mapeditor::infix;
  }
#endif

void showHelp() {
    
    getcstat = SDLK_ESCAPE;
    if(help == "@") help = buildHelpText();
    
    string help2;
    if(help[0] == '@') {
      int iv = help.find("\t");
      int id = help.find("\n");
      dialog::init(help.substr(iv+1, id-iv-1), atoi(help.c_str()+1), 120, 100);
      dialog::addHelp(help.substr(id+1));
      }
    else {
      dialog::init("help", 0xFFFFFF, 120, 100);
      dialog::addHelp(help);
      }
    
    if(help == buildHelpText()) dialog::addItem("credits", 'c');
    
    dialog::display();
    }

void handleHelp(int sym, int uni) {
  dialog::handleNavigation(sym, uni);
  if(sym == SDLK_F1 && help != "@") 
    help = "@";
  else if(uni == 'c')
    help = buildCredits();
  else if(sym != 0 && sym != SDLK_F12)
    cmode = lastmode;
  }

void handleQuit(int sym, int uni) {
  if(uni == 'r' || sym == SDLK_F5) {
    restartGame(), cmode = emNormal;
    msgs.clear();
    }
  else if(uni == 'v') cmode = emMenu;
  else if(uni == SDLK_ESCAPE) cmode = canmove ? emNormal : emQuit;
  else if(uni == 'o') setAppropriateOverview();
#ifndef NOSAVE
  else if(uni == 't') {
    if(!canmove) restartGame();
    loadScores();
    msgs.clear();
    }
#endif
  
  }

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

  getcstat = ' ';
  
  dialog::init(XLAT("HyperRogue %1: online demo", VER), 0xC00000, 200, 100);
  dialog::addBreak(50);

  dialog::addItem(XLAT("play"), 'f');
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
  if(sym == SDLK_F1 || sym == 'h') {
    lastmode = cmode;
    cmode = emHelp;
    }
  else if(sym == 'a') {
    toggleanim(!demoanim);
    cmode = emNormal;
    }
  else if(sym == 'f') {
    firstland = laIce;
    if(tactic::on) restartGame('t');
    else restartGame();
    cmode = emNormal;
    }
  else if(sym == 't') {
    firstland = laTemple;
    if(!tactic::on) restartGame('t');
    else restartGame();
    cmode = emNormal;
    }
  else if(sym == 'l') {
    firstland = laStorms;
    if(!tactic::on) restartGame('t');
    else restartGame();
    cmode = emNormal;
    }
  else if(sym == 'b') {
    firstland = laBurial;
    if(!tactic::on) restartGame('t');
    else restartGame();
    items[itOrbSword] = 60;
    cmode = emNormal;
    }
  }
#endif

void displayMenus() {
  #ifdef DEMO
  if(cmode == emOverview || cmode == emMenu) {
    showDemo();
    return;
    }
  #endif
  if(cmode == emJoyConfig) showJoystickConfig();
  if(cmode == emOverview) showOverview();
  if(cmode == emYendor) yendor::showMenu();
  if(cmode == emChangeMode) showChangeMode();
  if(cmode == emCustomizeChar) showCustomizeChar();
  if(cmode == emShmupConfig) shmup::showShmupConfig();
  if(cmode == emConformal) conformal::show();
  if(cmode == emTactic) tactic::showMenu();
  if(cmode == emPickEuclidean) showEuclideanMenu();
  if(cmode == emMenu) showMainMenu();
  if(cmode == emCheatMenu) showCheatMenu();
  if(cmode == emVisual1) showVisual1();
  if(cmode == emVisual2) showVisual2();
  if(cmode == emColor) dialog::drawColorDialog(*dialog::colorPointer);
  if(cmode == emNumber) dialog::drawNumberDialog();

#ifndef NOMODEL
  if(cmode == emNetgen) netgen::show();
#endif
#ifndef NORUG
  if(cmode == emRugConfig) rug::show();
#endif
#ifndef NOEDIT
  if(cmode == emMapEditor) mapeditor::showMapEditor();
  if(cmode == emDraw) mapeditor::showDrawEditor();
#endif

#ifndef NOSAVE
  if(cmode == emScores) showScores();
  if(cmode == emPickScores) showPickScores();
#endif
  if(cmode == emHelp) showHelp();
  if(cmode == em3D) show3D();
#ifdef MOBILE
#ifdef HAVE_ACHIEVEMENTS
  if(cmode == emLeader) leader::showMenu();
#endif
#endif
#ifdef ROGUEVIZ
  if(cmode == emRogueviz) rogueviz::showMenu();
#endif
  }
