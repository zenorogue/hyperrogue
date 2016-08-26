// HyperRogue menus
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

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
  
  int nl = LAND_OVER; eLand *landtab = land_over;
  if(randomPatternsMode) { nl = RANDLANDS; landtab = randlands; }
  
  int vf = min((vid.yres-64) / nl, vid.xres/40);
  
  eLand curland = cwt.c->land;
  if(curland == laPalace && princess::dist(cwt.c) < OUT_OF_PRISON)
    curland = laPrincessQuest;
  if(isElemental(curland)) curland = laElementalWall;
  
  getcstat = '0';
  
  for(int i=0; i<nl; i++) {
    eLand l = landtab[i];
    int xr = vid.xres / 64;
    int i0 = 56 + i * vf;
    int col;
    if(landUnlocked(l)) col = linf[l].color; else col = 0x202020;
    if(chaosmode && noChaos(l)) col = 0x200000;
    if(l == curland)
      displayfr(1, i0, 1, vf-4, "*", 0xFFFFFF, 0);
    if(displayfr(xr*1, i0, 1, vf-4, XLAT1(linf[l].name), col, 0))
      getcstat = 1000 + l;
    eItem it = treasureType(l);
    int lv = items[it] * landMultiplier(l);
    if(lv >= 25) col = 0xFFD500;
    else if(lv >= 10) col = 0x00D500;
    else if(items[it]) col = 0xC0C0C0;
    else col = 0x202020;
    if(chaosmode && noChaos(l)) col = 0x200000;
    if(displayfr(xr*24-48, i0, 1, vf-4, its(items[it]), col, 16))
      getcstat = 2000+it;
    if(!cheater)
    if(displayfr(xr*24, i0, 1, vf-4, its(hiitems[modecode()][it]), col, 16))
      getcstat = 2000+it;
    if(items[it]) col = iinf[it].color; else col = 0x202020;
    if(chaosmode && noChaos(l)) col = 0x200000;
    if(displayfr(xr*24+32, i0, 1, vf-4, s0 + iinf[it].glyph, col, 16))
      getcstat = 2000+it;
    if(displayfr(xr*24+40, i0, 1, vf-4, XLAT1(iinf[it].name), col, 0))
      getcstat = 2000+it;
    eItem io = orbType(l);
    if(io == itShard) {
      if(items[it] >= 10) col = winf[waMirror].color; else col = 0x202020;
      if(chaosmode && noChaos(l)) col = 0x200000;
      if(displayfr(xr*46, i0, 1, vf-4, XLAT1(winf[waMirror].name), col, 0))
        getcstat = 3000+waMirror;
      if(getcstat == 3000+waMirror)
        mouseovers = XLAT(
          olrDescriptions[getOLR(io, cwt.c->land)], cwt.c->land, it, treasureType(cwt.c->land));
      }
    else if(io) {
      if(lv >= 25) col = 0xFFD500;
      else if(lv >= 10) col = 0xC0C0C0;
      else col = 0x202020;
      if(chaosmode && noChaos(l)) col = 0x200000;
      if(displayfr(xr*46-32, i0, 1, vf-4, its(items[io]), col, 16))
        getcstat = 2000+io;
      if(lv >= 10) col = iinf[io].color; else col = 0x202020;
      if(chaosmode && noChaos(l)) col = 0x200000;
      if(displayfr(xr*46-8, i0, 1, vf-4, s0 + iinf[io].glyph, col, 16))
        getcstat = 2000+io;
      if(displayfr(xr*46, i0, 1, vf-4, XLAT1(iinf[io].name), col, 0))
        getcstat = 2000+io;
      if(getcstat == 2000+io)
        mouseovers = XLAT(
          olrDescriptions[getOLR(io, curland)], curland, it, treasureType(curland));
      }
    }
  }

void handleOverview(int uni) {
  int umod = uni % 1000;
  int udiv = uni / 1000;
  if(udiv == 1 && umod < landtypes) {
    if(cheater) {
      eLand l = eLand(umod);
      cheater++;
      if(l == laPrincessQuest) {
        if(kills[moVizier] == 0) kills[moVizier] = 1;
        princess::forceMouse = true;
        princess::gotoPrincess = true;
        l = laPalace;
        }
      activateSafety(l);
      cmode = emNormal;
      canmove = true;
      }
    else {
      lastmode = cmode;
      cmode = emHelp; help = generateHelpForLand(eLand(umod));
      }
    }
  else if(udiv == 2 && umod < ittypes) {
    if(cheater) {
      cheater++;
      double shiftmul = 1.001;
      if(anyshiftclick) shiftmul *= -1;
      if(rightclick) shiftmul /= 10;
      int ic = itemclass(eItem(umod));
      if(ic == IC_TREASURE) items[umod] += int(10*shiftmul);
      if(ic == IC_ORB) items[umod] += int(60*shiftmul);
      if(umod == itGreenStone) items[umod] += int(100*shiftmul);
      else if(ic == IC_OTHER) items[umod] += (shiftmul>0?1:-1);
      if(items[umod] < 0) items[umod] = 0;
      if(hardcore) canmove = true;
      else checkmove();
      }
    else {
      lastmode = cmode;
      cmode = emHelp; help = generateHelpForItem(eItem(umod));
      if(hardcore) canmove = true;
      else checkmove();
      }
    }
  else if(udiv == 3 && umod < walltypes) {
    lastmode = cmode;
    cmode = emHelp; help = generateHelpForWall(eWall(umod));
    }
  else if(uni) cmode = emNormal;
  }

void showMainMenu() {
  int y = vid.yres * .5 - vid.fsize * 10.5;
  displayfr(vid.xres/2, y-vid.fsize * 2, 4, vid.fsize*2, 
    XLAT("HyperRogue %1", VER), 0xC00000, 8
    );

  displayButton(vid.xres/2, y + vid.fsize*2, ifMousing("b", "basic configuration"), 'b', 8, 2);
  displayButton(vid.xres/2, y + vid.fsize*4, ifMousing("a", "advanced configuration"), 'a', 8, 2);
                                             
#ifndef ANDROID
  displayButton(vid.xres/2, y + vid.fsize*6, ifMousing("t", "local highscores"), 't', 8, 2);
#endif
  displayButton(vid.xres/2, y + vid.fsize*8, ifMousing("h, F1", "help"), 'h', 8, 2);
  if(cheater)
     displayButton(vid.xres/2, y + vid.fsize*10, ifMousing("c", "cheats"), 'c', 8, 2);

  displayButton(vid.xres/2, y + vid.fsize*12, ifMousing("r, F5", "restart game"), 'r', 8, 2);
  displayButton(vid.xres/2, y + vid.fsize*14, ifMousing("m", "special game modes"), 'm', 8, 2);
  
  string q;
  
#ifndef ANDROID
  q = (items[itOrbSafety] && havesave) ? "save" : "quit"; 
#ifdef IOS
  q = q + " and visit the website";
#else
  q = q + " the game";
#endif
  displayButton(vid.xres/2, y + vid.fsize*17, ifMousing("q, F10", q), 'q', 8, 2);
#endif

  if(canmove)
    q = "review your quest";
  else
    q = "review the scene";
  
  displayButton(vid.xres/2, y + vid.fsize*20, ifMousing("ESC", q), SDLK_ESCAPE, 8, 2);

  displayButton(vid.xres/2, y + vid.fsize*22, ifMousing("o", "world overview"), 'o', 8, 2);
  
  if(!canmove) q = "game over screen";
  else if(turncount > 0) q = "continue game";
  else q = "play the game!";
  
  displayButton(vid.xres/2, y + vid.fsize*25, ifMousing(XLAT("other"), q), ' ', 8, 2);
  }

void loadScores();

bool handleMenuKey(int sym, bool mdown) {
  if(sym == SDLK_F1 || sym == 'h') {
    lastmode = cmode;
    cmode = emHelp;
    }
  else if(sym == 'c' && cheater)
    cmode = emCheatMenu;
  else if(sym == 'b') cmode = emVisual1;
  else if(sym == 'a') cmode = emVisual2;
  else if(sym == 'm') cmode = emChangeMode;
#ifndef ANDROID
  else if(sym == 't') loadScores();
#endif
  else if(sym == 'r' || sym == SDLK_F5) {
    restartGame();
    cmode = emNormal;
    }
  else if(sym == 'q' || sym == SDLK_F10) return true;
  else if(sym == 'o') cmode = emOverview;
  else if(sym == SDLK_ESCAPE) cmode = emQuit;
  else if((sym != 0 && sym != SDLK_F12) || mdown) {
    cmode = emNormal;
    msgs.clear();
    }
  return false;
  }

void showVisual1() {
#ifndef MOBILE
  displayStat(2, XLAT("video resolution"), its(vid.xres) + "x"+its(vid.yres), 'r');
  displayStat(3, XLAT("fullscreen mode"), ONOFF(vid.full), 'f');
#endif
  displayStat(4, XLAT("animation speed"), fts(vid.aspeed), 'a');
  displayStat(5, XLAT("dist from hyperboloid ctr"), fts(vid.alpha), 'p');
  displayStat(6, XLAT("scale factor"), fts(vid.scale), 'z');

  const char *wdmodes[4] = {"ASCII", "black", "plain", "Escher"};
  const char *mdmodes[4] = {"ASCII", "items only", "items and monsters", "high contrast"};

  displayStat(7, XLAT("draw the heptagons darker"), ONOFF(vid.darkhepta), '7');
  
  displayStat(8, XLAT("wall display mode"), 
    XLAT(wdmodes[vid.wallmode]), 'w');
  displayStat(9, XLAT("monster display mode"), 
    XLAT(mdmodes[vid.monmode]), 'm');
#ifndef MOBILE
  const char *axmodes[4] = {"no axes", "auto", "light", "heavy"};
  displayStat(10, XLAT("cross display mode"), 
    XLAT(axmodes[vid.axes]), 'c');
#endif
#ifndef MOBILE
  displayStat(11, XLAT("background music volume"), 
    its(audiovolume), 'b');
#endif
  if(lang() != 0) {
    string s = XLAT("TRANSLATIONWARNING");
    if(s != "" && s != "TRANSLATIONWARNING") {
      int dy = vid.fsize * 23 + vid.yres/4;
      int dx = vid.xres/2;
      displaystr(dx, dy, 0, vid.fsize, s, 0xFF0000, 8);
      }
    s = XLAT("TRANSLATIONWARNING2");
    if(s != "" && s != "TRANSLATIONWARNING2") {
      int dy = vid.fsize * 24 + vid.yres/4;
      int dx = vid.xres/2;
      displaystr(dx, dy, 0, vid.fsize, s, 0xFF0000, 8);
      }
      }
  displayStat(13, XLAT("language"), XLAT("EN"), 'l');
  displayStat(14, XLAT("player character"), 
  numplayers() > 1 ? "" : csname(vid.cs), 'g');
  }

void handleVisual1(int sym, int uni) {
    
  char xuni = uni | 96;
  
  if(uni >= 32 && uni < 64) xuni = uni;
  
  if(xuni == 'p') vid.alpha += shiftmul * 0.1;
  if(xuni == 'z') vid.scale += shiftmul * 0.1;
  
  if(xuni == 'i') {
    double d = exp(shiftmul/10);
    vid.alpha *= d;
    vid.scale *= d;
    }
  
  if(xuni == 'a') vid.aspeed += shiftmul;
#ifndef MOBILE
  if(xuni == 'f') {
    vid.full = !vid.full;
    if(shiftmul > 0) {
      vid.xres = vid.full ? vid.xscr : 9999;
      vid.yres = vid.full ? vid.yscr : 9999;
      extern bool setfsize;
      setfsize = true;
      }
    setvideomode();
    }
#endif
  
  if(xuni == 'v' || sym == SDLK_F2) cmode = emNormal;
#ifndef ANDROID
  if(xuni == 's') saveConfig();
#endif
  
  if(xuni == '7') { vid.darkhepta = !vid.darkhepta; }
  if(xuni == 'w') { vid.wallmode += 60 + (shiftmul > 0 ? 1 : -1); vid.wallmode %= 4; }
  if(xuni == 'm') { vid.monmode += 60 + (shiftmul > 0 ? 1 : -1); vid.monmode %= 4; }
  if(xuni == 'c') { vid.axes += 60 + (shiftmul > 0 ? 1 : -1); vid.axes %= 4; }
#ifndef MOBILE
  if(xuni == 'b') {
    audiovolume += int(10.5 * shiftmul);
    if(audiovolume < 0) audiovolume = 0;
    if(audiovolume > MIX_MAX_VOLUME) audiovolume = MIX_MAX_VOLUME;
    Mix_VolumeMusic(audiovolume);
    }
#endif
  
  if(sym == SDLK_ESCAPE) cmode = emNormal;
  
  if(xuni == 'l') {
    vid.language += (shiftmul>0?1:-1);
    vid.language %= NUMLAN;
    if(vid.language < 0) vid.language += NUMLAN;
    }
  
  if(xuni == 'g') cmode = emCustomizeChar;
  }

void showVisual2() {
  #ifndef MOBILE
  #ifdef GL
  displayStat(2, XLAT("openGL & antialiasing mode"), vid.usingGL ? "OpenGL" : vid.usingAA ? "AA" : "OFF", 'o');
  #endif
  #endif
  displayStat(3, XLAT("distance between eyes"), fts(vid.eye * 10), 'e');
  #ifndef MOBILE
  displayStat(4, XLAT("framerate limit"), its(vid.framelimit), 'f');
  #endif

#ifndef MOBILE
  displayStat(6, XLAT("joystick mode"), XLAT(autojoy ? "automatic" : "manual"), 'p');
    
  displayStat(7, XLAT("first joystick: movement threshold"), its(vid.joyvalue), 'a');
  displayStat(8, XLAT("first joystick: execute movement threshold"), its(vid.joyvalue2), 'b');
  displayStat(9, XLAT("second joystick: pan threshold"), its(vid.joypanthreshold), 'c');
  displayStat(10, XLAT("second joystick: panning speed"), fts(vid.joypanspeed * 1000), 'd');
#endif

#ifdef MOBILE
  displayStat(6, XLAT("sight range"), its(sightrange), 'a');
  extern int fontscale;
  displayStat(7, XLAT("font scale"), its(fontscale), 'b');
#endif
  
  displayStat(12, XLAT("message flash time"), its(vid.flashtime), 't');
#ifndef MOBILE
  displayStat(13, XLAT("targetting ranged Orbs Shift+click only"), ONOFF(vid.shifttarget), 'i');
#endif
#ifdef STEAM
  displayStat(14, XLAT("send scores to Steam leaderboards"), ONOFF(vid.steamscore), 'l');
#endif
  }

void handleVisual2(int sym, int uni) {
  char xuni = uni | 96;
  
  if(xuni == 'v' || sym == SDLK_F2 || sym == SDLK_ESCAPE) cmode = emNormal;
#ifndef ANDROID
  if(xuni == 's') saveConfig();
#endif
  
  if(sym == SDLK_F1 || sym == 'h')
    lastmode = cmode, cmode = emHelp;
#ifdef GL
  
#ifndef MOBILE
  if(xuni == 'o' && shiftmul > 0) {
    vid.usingGL = !vid.usingGL;
    if(vid.usingGL) addMessage(XLAT("openGL mode enabled"));
    if(!vid.usingGL) addMessage(XLAT("openGL mode disabled"));
    setvideomode();
    }
#endif
  
  if(xuni == 'o' && shiftmul < 0 && !vid.usingGL) {
    vid.usingAA = !vid.usingAA;
    if(vid.usingAA) addMessage(XLAT("anti-aliasing enabled"));
    if(!vid.usingAA) addMessage(XLAT("anti-aliasing disabled"));
    }
#endif
  
  if(xuni == 'f') {
    vid.framelimit += int(10.5 * shiftmul);
    if(vid.framelimit < 5) vid.framelimit = 5;
    }
  
#ifdef MOBILE
  if(xuni == 'a') {
    sightrange += shiftmul>0?1:-1;
    if(sightrange < 4) sightrange = 4;
    if(sightrange > 7) sightrange = 7;
    }

  if(xuni =='b') {
    extern int fontscale;
    fontscale += int(shiftmul * 10);
    }
#endif

  if(xuni == 'a') vid.joyvalue += int(shiftmul * 100);
  if(xuni == 'b') vid.joyvalue2 += int(shiftmul * 100);
  if(xuni == 'c') vid.joypanthreshold += int(shiftmul * 100);
  if(xuni == 'd') vid.joypanspeed += shiftmul / 50000;
  if(xuni == 'e') vid.eye += shiftmul * 0.01;
#ifdef STEAM
  if(xuni == 'l') vid.steamscore = !vid.steamscore;
#endif
  if(xuni == 't') vid.flashtime += shiftmul>0?1:-1;
  
  if(xuni == 'p') autojoy = !autojoy;
  if(xuni == 'i') { vid.shifttarget = !vid.shifttarget; }
  }

void showChangeMode() {
#ifndef MOBILE
  displayStat(2, XLAT("vector graphics editor"), "", 'g');
  displayStat(3, XLAT("map editor"), ONOFF(false), 'm');
#endif
  displayStat(4, XLAT("cheat mode"), ONOFF(cheater), 'c');
  
  displayStat(6, XLAT("Euclidean mode"), ONOFF(euclid), 'e');
#ifndef MOBILE
  displayStat(7, 
    XLAT("shoot'em up mode") + " " + XLAT("(includes co-op)"), 
    ONOFF(shmup::on), 's');
#endif
  if(!shmup::on) displayStat(8, XLAT("hardcore mode"),
    hardcore && !pureHardcore() ? XLAT("PARTIAL") : ONOFF(hardcore), 'h');
  
  displayStat(9, XLAT("%1 Challenge", moPrincess), ONOFF(princess::challenge), 'p');
  displayStat(10, XLAT("random pattern mode"), ONOFF(randomPatternsMode), 'r');
  displayStat(11, XLAT("Yendor Challenge"), ONOFF(yendor::on), 'y');
#ifndef ANDROID
  displayStat(12, XLAT("pure tactics mode"), ONOFF(tactic::on), 't');
#endif
#ifndef MOBILE
  displayStat(13, XLAT("hypersian rug mode"), ONOFF(rug::rugged), 'u');
#endif
  displayStat(14, XLAT("heptagonal mode"), ONOFF(purehepta), '7');
  displayStat(15, XLAT("Chaos mode"), ONOFF(chaosmode), 'C');
  
#ifndef MOBILE
  displayStat(16, XLAT("paper model creator"), ONOFF(false), 'n');
#endif
  displayStat(17, XLAT("conformal/history mode"), ONOFF(conformal::on), 'a');
  
  displayStat(19, XLAT("return to the game"), "", 'v');
  }

void handleChangeMode(int sym, int uni) {
    
  char xuni = uni;
  
  if((uni >= 'A' && uni <= 'Z') || (uni >= 1 && uni <= 26)) xuni |= 96;
  
  if(xuni == 'v' || sym == SDLK_F2 || sym == SDLK_ESCAPE) cmode = emNormal;
  
  if(uni == 'c') {
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
  
#ifndef MOBILE
  if(xuni == 'g') {
    cmode = emDraw;
    mapeditor::drawcell = cwt.c;
  }
#endif
  if(xuni == 'e') {
    cmode = emPickEuclidean;
  }
  if(xuni == 't') {
    cmode = emTactic;
  }
#ifndef MOBILE
  if(xuni == 'u')
    rug::select();
#endif
  if(xuni == 'y') {
    if(yendor::everwon)
        cmode = emYendor;
    else {
      cmode = emHelp;
      help = yendor::chelp;
      lastmode = emChangeMode;
      }
    }
  if(xuni == '7')
      restartGame('7');
  if(uni == 'a')
      cmode = emConformal;
  if(uni == 'C') {
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
  if(xuni == 'p') {
    if(!princess::everSaved)
      addMessage(XLAT("Save %the1 first to unlock this challenge!", moPrincess));
    else {
      restartGame('p');
      cmode = emNormal;
      }
    }
#ifndef MOBILE
  if(xuni == 'm') {
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
  if(xuni == 's')
    cmode = emShmupConfig;
  if(xuni == 'n')
    cmode = emNetgen;
  if(xuni == 'h' && !shmup::on) {
    if(hardcore && !canmove) { }
    else if(hardcore && canmove) { hardcore = false; }
    else { hardcore = true; canmove = true; hardcoreAt = turncount; }
    if(hardcore)
        addMessage("One wrong move, and it is game over!");
    else
        addMessage("Not so hardcore?");
    if(pureHardcore()) cmode = emNormal;
    }
  if(xuni == 'r') {
    firstland = laIce;
    restartGame('r');
    cmode = emNormal;
    }
  }

void showCheatMenu() {
  displayStat(0, XLAT("gain orb powers"), "", 'F');
  displayStat(1, XLAT("summon treasure"), "", 'T');
  displayStat(2, XLAT("summon dead orbs"), "", 'D');
  displayStat(3, XLAT("lose all treasure"), "", 'J');
  displayStat(4, XLAT("gain kills"), "", 'K');
  displayStat(5, XLAT("Hyperstone Quest"), "", 'C');
  displayStat(6, XLAT("summon orbs"), "", 'O');
  displayStat(7, XLAT("gain Orb of Yendor"), "", 'Y');
  displayStat(8, XLAT("summon lots of treasure"), "", 'T'-64);
  displayStat(9, XLAT("Safety (quick save)"), "", 'S');
  displayStat(10, XLAT("Select the land ---"), "", 'L');
  displayStat(11, XLAT("--- and teleport there"), "", 'U');
  displayStat(12, XLAT("rotate the character"), "", 'Z');
  displayStat(13, XLAT("summon a Golem"), "", 'G');
  displayStat(14, XLAT("summon Sandworm"), "", 'W');
  displayStat(15, XLAT("summon Ivy"), "", 'I');
  displayStat(16, XLAT("summon a Monster"), "", 'E');
  displayStat(17, XLAT("summon Thumpers"), "", 'H');
  displayStat(18, XLAT("summon Bonfire"), "", 'B');
  displayStat(19, XLAT("summon Mimics"), "", 'M');
  displayStat(20, XLAT("deplete orb powers"), "", 'P');
  displayStat(21, XLAT("summon Orb of Yendor"), "", 'Y'-64);
  displayStat(22, XLAT("switch ghost timer"), "", 'G'-64);
  displayStat(23, XLAT("switch web display"), "", 'W'-64);
  displayStat(-2, XLAT("return to the game"), "", ' ');
  }

void handleCheatMenu(int uni) {
  if(uni != 0) {
    applyCheat(uni);
    if(uni == 'F' || uni == 'C' || uni == 'O' ||
      uni == 'S' || uni == 'U' || uni == 'G' ||
      uni == 'W' || uni == 'I' || uni == 'E' ||
      uni == 'H' || uni == 'B' || uni == 'M' ||
      uni == 'P' || uni == 'Y'-64 || uni == 'G'-64 ||
      uni == ' ') 
      cmode = emNormal;
    }
  }

void showCustomizeChar() {
  displayStatHelp(0, XLAT("Customize character"));
  
  if(shmup::on) shmup::cpid = shmup::cpid_edit % shmup::players;
  charstyle& cs = getcs();
  
  displayStat(2, XLAT("character"), csname(cs), 'g');
  displayStat(3, XLAT("skin color"), "?", 's');
  displayStat(4, XLAT("weapon color"), "?", 'w');
  displayStat(5, XLAT("hair color"), "?", 'h');

  if(cs.charid >= 1) displayStat(6, XLAT("dress color"), "?", 'd');
  if(cs.charid == 3) displayStat(7, XLAT("dress color II"), "?", 'f');
  
  if(!shmup::on) displayStat(8, XLAT("save whom"), XLAT1(minf[moPrincess].name), 'p');
  
  if(numplayers() > 1) displayStat(8, XLAT("player"), its(shmup::cpid+1), 'a');
  
  displayStatHelp(16, XLAT("Shift=random, Ctrl=mix"));
  
  displayStat(19, XLAT("return to the game"), "", 'v');
  }

void switchcolor(int& c, unsigned int* cs, int mod) {
  int id = 0;
  int q = cs[0]; cs++;
  for(int i=0; i<q; i++) if(c == (int) cs[i]) id = i;
  if(mod == 1)
    c = ((rand() % 0x1000000) << 8) | 0xFF;
  else if(mod == 2)
    c = (gradient(cs[rand() % q] >> 8, cs[rand() % q] >> 8, 0, rand() % 101, 100) << 8) + 0xFF;
  else
    c = cs[(id+1) % q];
  }

void handleCustomizeChar(int sym, int uni, int mod) {

  char xuni = uni | 96;
  if(shiftmul < -.5) mod = 1;
  else if(shiftmul > -.2 && shiftmul < .2) mod = 2;
  else mod = 0;

  if(shmup::on) shmup::cpid = shmup::cpid_edit % shmup::players;
  charstyle& cs = getcs();
  if(xuni == 'a') { shmup::cpid_edit++; shmup::cpid_edit %= 60; }
  if(xuni == 'g') {
    cs.charid++;
    if(cs.charid == 2 && !princess::everSaved) cs.charid = 4;
    cs.charid %= 8;
    }
  if(xuni == 'p') vid.samegender = !vid.samegender;
  bool cat = cs.charid >= 4;
  if(xuni == 's') switchcolor(cs.skincolor, cat ? haircolors : skincolors, mod);
  if(xuni == 'h') switchcolor(cs.haircolor, haircolors, mod);
  if(xuni == 'w') switchcolor(cs.swordcolor, cat ? eyecolors : swordcolors, mod);
  if(xuni == 'd') switchcolor(cs.dresscolor, cat ? haircolors : dresscolors, mod);
  if(xuni == 'f') switchcolor(cs.dresscolor2, dresscolors2, mod);
  if(xuni == 'v' || sym == SDLK_F2 || sym == SDLK_ESCAPE) cmode = emNormal;
  }

int eupage = 0;
int euperpage = 21;

void showEuclideanMenu() {
  int s = vid.fsize;
  vid.fsize = vid.fsize * 4/5;
  displayStatHelp(-8, XLAT("Euclidean mode"));
  if(cheater) for(int i=0; i<landtypes; i++) landvisited[i] = true;
  for(int i=0; i<landtypes; i++)
    if(hiitemsMax(treasureType(eLand(i))) >= 25) landvisited[i] = true;
  landvisited[laCrossroads] = true;
  landvisited[laIce] = true;
  landvisited[laMirror] = true;
  landvisited[laPrincessQuest] = princess::everSaved;
  landvisited[laWildWest] = true;
  // for(int i=2; i<lt; i++) landvisited[i] = true;
  for(int i=0; i<euperpage; i++) {
    if(euperpage * eupage + i >= LAND_EUC) break;
    eLand l = land_euc[euperpage * eupage + i];
    if(landvisited[l]) {
      char ch;
      if(i < 26) ch = 'a' + i;
      else ch = 'A' + (i-26);
      displayStat(i-6, XLAT1(linf[l].name), "", ch);
      }
    }
  displayStat(euperpage+1-6, XLAT("Return to the hyperbolic world"), "", '0');
  displayStat(euperpage+2-6, XLAT("Next page"), "", '-');
  displayStatHelp(euperpage+4-6, XLAT("Choose from the lands visited this game."));
#ifdef HAVE_ACHIEVEMENTS
  displayStatHelp(euperpage+6-6, XLAT("Scores and achievements are not"));
  displayStatHelp(euperpage+7-6, XLAT("saved in the Euclidean mode!"));
#endif
  vid.fsize = s;
  }

void handleEuclidean(int sym, int uni) {
  int lid;
  if(uni >= 'a' && uni <= 'z') lid = uni - 'a';
  else if(uni >= 'A' && uni <= 'Z') lid = 26 + uni - 'A';
  else lid = -1;
  
  if(lid >= 0) lid += euperpage * eupage;
  
  if(uni == '0') {
    if(euclid) restartGame('e');
    cmode = emNormal;
    }
  else if(uni == '-') {
    eupage++;
    if(eupage * euperpage >= LAND_EUC) eupage = 0;
    }
  else if(lid >= 0 && lid < LAND_EUC) {
    euclidland = land_euc[lid];
    if(landvisited[euclidland] && euclidland != laOceanWall) {
      if(euclid) restartGame();
      else restartGame('e');
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
  else if(uni) cmode = emNormal;
  }

void showConfig() {
  displayStatHelp(0, XLAT("Configuration:"));
  
  if(cmode == emVisual1) showVisual1(); else showVisual2();

#ifndef MOBILE
  displayStatHelp(16, XLAT("use Shift to decrease and Ctrl to fine tune "));
  displayStatHelp(17, XLAT("(e.g. Shift+Ctrl+Z)"));
#endif
    
  displayStat(19, XLAT("exit configuration"), "", 'v');
#ifdef ANDROID
  displayStat(16, XLAT("settings set here won't be saved"), "", 's');
  displayStat(17, XLAT("-- use the Android menu instead"), "", 's');
#else
  displayStat(21, XLAT("save the current config"), "", 's');
#endif
  }

void handleMenus(int sym, int uni, int mod) {
  if(cmode == emOverview) handleOverview(uni);
  else if(cmode == emYendor) yendor::handleKey(uni, sym);
  else if(cmode == emChangeMode) handleChangeMode(uni, sym);
  else if(cmode == emVisual1) handleVisual1(uni, sym);
  else if(cmode == emMenu) handleMenuKey(uni, false);
  else if(cmode == emCheatMenu) handleCheatMenu(uni);
  else if(cmode == emVisual2) handleVisual2(uni, sym);
  else if(cmode == emPickEuclidean) handleEuclidean(uni, sym);
  else if(cmode == emCustomizeChar) handleCustomizeChar(uni, sym, 0);
  else if(cmode == emTactic) tactic::handleKey(uni, sym);
  else if(cmode == emConformal) conformal::handleKey(uni, sym);
  }

void displayMenus() {
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
  if(cmode == emVisual1 || cmode == emVisual2) showConfig();    

#ifndef MOBILE
  if(cmode == emNetgen) netgen::show();
  if(cmode == emRugConfig) rug::show();
  if(cmode == emMapEditor) mapeditor::showMapEditor();
  if(cmode == emDraw) mapeditor::showDrawEditor();
#endif

#ifndef ANDROID
  if(cmode == emScores) showScores();
  if(cmode == emPickScores) showPickScores();
#endif
  }
