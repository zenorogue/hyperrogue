int frames;
bool outoffocus = false;

int mousex, mousey, joyx, joyy, panjoyx, panjoyy;
hyperpoint mouseh, mouseoh;

bool leftclick, rightclick, targetclick, hiliteclick, anyshiftclick, wheelclick,
  forcetarget, lshiftclick, lctrlclick;
bool gtouched;

int getcstat, lgetcstat; ld getcshift; bool inslider;

function <void(int sym, int uni)> keyhandler;

// is the player using mouse? (used for auto-cross)
bool mousing = true;

// is the mouse button pressed?
bool mousepressed = false;
bool mousemoved = false;
bool actonrelease = false;

ld shiftmul = 1;

cell *mouseover, *mouseover2, *lmouseover, *centerover;
ld modist, modist2, centdist;
movedir mousedest, joydir;

int lastt;

#ifdef WEB
Uint8 *SDL_GetKeyState(void *v) { static Uint8 tab[1024]; return tab; }
#endif

bool mouseout() {
  if((getcstat != '-' && getcstat) || (lgetcstat && lgetcstat != '-')) return true;
  return outofmap(mouseh);
  }

bool mouseout2() {
  if((getcstat && getcstat != '-') || (lgetcstat && lgetcstat != '-')) return true;
  return outofmap(mouseh) || outofmap(mouseoh);
  }

movedir vectodir(const hyperpoint& P) {

  hyperpoint H = sphereflip * tC0(cwtV);
  ld R = sqrt(H[0] * H[0] + H[1] * H[1]);
  transmatrix Centered = sphereflip * cwtV;
  if(!euclid)
    Centered = gpushxto0(H) * Centered;
  else if(R > 1e-9)
    Centered = eupush(-H[0], -H[1]) * Centered;
  ld binv = 99;
  
  ld dirdist[7];
  for(int i=0; i<cwt.c->type; i++) {
    dirdist[i] = intval(Centered * xspinpush0(-i * 2 * M_PI /cwt.c->type, .5), P);
    }
    
  movedir res;
  res.d = -1;
  
  for(int i=0; i<cwt.c->type; i++) {
    if(dirdist[i] < binv) {
      binv = dirdist[i];
      res.d = i;
      res.subdir = dirdist[(i+1)%cwt.c->type] < dirdist[(i+cwt.c->type-1)%cwt.c->type] ? 1 : -1;
      if(sphere) res.subdir = -res.subdir;
      }
    }
  
  // if(euclid) bdir = (bdir + 3) % 6;
  return res;
  }

void movepckeydir(int d) {
  DEBB(DF_GRAPH, (debugfile,"movepckeydir\n"));
  // EUCLIDEAN

  movedir md = 
    vectodir(spin(-d * M_PI/4) * tC0(pushone()));
    
  movepcto(md);
  }

void calcMousedest() {
  if(mouseout()) return;
  if(vid.revcontrol == true) { mouseh[0] = -mouseh[0]; mouseh[1] = -mouseh[1]; }
  ld mousedist = intval(mouseh, tC0(shmup::ggmatrix(cwt.c)));
  mousedest.d = -1;
  
  cellwalker bcwt = cwt;
  
  ld dists[7];
  
  for(int i=0; i<cwt.c->type; i++)
    dists[i] = intval(mouseh, tC0(shmup::ggmatrix(cwt.c->mov[i])));
  
  /* printf("curcell = %Lf\n", mousedist);
  for(int i=0; i<cwt.c->type; i++)
    printf("d%d = %Lf\n", i, dists[i]); */

  for(int i=0; i<cwt.c->type; i++) if(dists[i] < mousedist) {
    mousedist = dists[i];
    mousedest.d = fixdir(i - cwt.spin, cwt.c);
    
    mousedest.subdir =
       dists[(i+1)%cwt.c->type] < dists[(i+cwt.c->type-1)%cwt.c->type] ? 1 : -1;

    if(cwt.mirrored) 
      mousedest.d = fixdir(-mousedest.d, cwt.c), 
      mousedest.subdir = -mousedest.subdir;
    
    if(sphere) mousedest.subdir = -mousedest.subdir;
    }
  
  if(vid.revcontrol == true) { mouseh[0] = -mouseh[0]; mouseh[1] = -mouseh[1]; }
  cwt = bcwt;
  }

void mousemovement() {
  calcMousedest();
  movepcto(mousedest);
  lmouseover = NULL;
  }

#ifndef NOSDL
SDL_Joystick* sticks[8];
int numsticks;

void initJoysticks() {
  DEBB(DF_INIT, (debugfile,"init joysticks\n"));
  numsticks = SDL_NumJoysticks();
  if(numsticks > 8) numsticks = 8;
  for(int i=0; i<numsticks; i++) {
    sticks[i] = SDL_JoystickOpen(i);
    /* printf("axes = %d, balls = %d, buttons = %d, hats = %d\n",
      SDL_JoystickNumAxes(sticks[i]),
      SDL_JoystickNumBalls(sticks[i]),
      SDL_JoystickNumButtons(sticks[i]),
      SDL_JoystickNumHats(sticks[i])
      ); */
    }
  }

void closeJoysticks() {
  DEBB(DF_INIT, (debugfile,"close joysticks\n"));
  for(int i=0; i<numsticks; i++) {
    SDL_JoystickClose(sticks[i]), sticks[i] = NULL;
    }
  numsticks = 0;
  }

void checkjoy() {
  DEBB(DF_GRAPH, (debugfile,"check joy\n"));
  if(!DEFAULTCONTROL) return;
  ld joyvalue1 = sqr(vid.joyvalue);
  ld joyvalue2 = sqr(vid.joyvalue2);
  
  ld jx = joyx;
  ld jy = joyy;
  ld sq = jx*jx+jy*jy;

  static int laststate = 0;
  int curstate = sq < joyvalue1 ? 0 : sq < joyvalue2 ? 1 : 2;
  if(curstate != laststate) flashMessages(), laststate = curstate;
  
  if(autojoy) {
    if(sq < joyvalue1) { if(joydir.d >= 0) movepcto(joydir); joydir.d = -1; return; }
    if(sq < joyvalue2 && joydir.d == -1) return;
    }
  else {
    if(sq < joyvalue1) { joydir.d = -1; return; }
    }
  
  joydir = vectodir(hpxy(jx, jy));
  }

void checkpanjoy(double t) {
 
  if(shmup::on) return;
  
  if(vid.joypanspeed < 1e-7) return;
  
  if(sqr(panjoyx) + sqr(panjoyy) < sqr(vid.joypanthreshold))
    return;
  
  ld jx = panjoyx * t * vid.joypanspeed;
  ld jy = panjoyy * t * vid.joypanspeed;
  
  playermoved = false;
  View = gpushxto0(hpxy(jx, jy)) * View;
  }

#endif

bool quitmainloop = false;

bool doexiton(int sym, int uni) {
  if(sym == SDLK_ESCAPE) return true;
  if(sym == SDLK_F10) return true;
  if(uni != 0) return true;
  return false;
  }

bool didsomething;

#ifdef MOBILE
typedef int eventtype;
#else
typedef SDL_Event eventtype;
#endif

void handlePanning(int sym, int uni) {
  if(rug::rugged) return;

#ifndef PANDORA
  if(sym == SDLK_RIGHT) { 
    if(conformal::on)
      conformal::lvspeed += 0.1 * shiftmul;
    else
      View = xpush(-0.2*shiftmul) * View, playermoved = false, didsomething = true;
    }
  if(sym == SDLK_LEFT) {
    if(conformal::on)
      conformal::lvspeed -= 0.1 * shiftmul;
    else
      View = xpush(+0.2*shiftmul) * View, playermoved = false, didsomething = true;
    }
  if(sym == SDLK_UP) {
    if(conformal::on)
      conformal::lvspeed += 0.1 * shiftmul;
    else
      View = ypush(+0.2*shiftmul) * View, playermoved = false, didsomething = true;
    }
  if(sym == SDLK_DOWN) {
    if(conformal::on)
      conformal::lvspeed -= 0.1 * shiftmul;
    else
      View = ypush(-0.2*shiftmul) * View, playermoved = false, didsomething = true;
    }
#endif
  if(sym == SDLK_PAGEUP) {
    if(conformal::on)
      conformal::rotation++;
    else
      View = spin(M_PI/S21*shiftmul) * View, didsomething = true;
    }
  if(sym == SDLK_PAGEDOWN) {
    if(conformal::on)
      conformal::rotation++;
    else
      View = spin(-M_PI/S21*shiftmul) * View, didsomething = true;
    }
  
  if(sym == SDLK_PAGEUP || sym == SDLK_PAGEDOWN) 
    if(isGravityLand(cwt.c->land)) playermoved = false;

  if(sym == PSEUDOKEY_WHEELUP) {
    ld jx = (mousex - vid.xcenter - .0) / vid.radius / 10;
    ld jy = (mousey - vid.ycenter - .0) / vid.radius / 10;
    playermoved = false;
    View = gpushxto0(hpxy(jx, jy)) * View;
    sym = 1;
    }
  }
  
void handleKeyNormal(int sym, int uni) {

  if(cheater) {
    if(applyCheat(uni, mouseover))
      sym = 0;
    }

  if(DEFAULTNOR(sym)) handlePanning(sym, uni);

  if(!(uni >= 'A' && uni <= 'Z') && DEFAULTCONTROL) {
    if(sym == 'l' || sym == 'd' || sym == SDLK_KP6) movepckeydir(0);
    if(sym == 'n' || sym == 'c' || sym == SDLK_KP3) movepckeydir(1);
    if(sym == 'j' || sym == 'x' || sym == SDLK_KP2) movepckeydir(2);
    if(sym == 'b' || sym == 'z' || sym == SDLK_KP1) movepckeydir(3);
    if(sym == 'h' || sym == 'a' || sym == SDLK_KP4) movepckeydir(4);
    if(sym == 'y' || sym == 'q' || sym == SDLK_KP7) movepckeydir(5);
    if(sym == 'k' || sym == 'w' || sym == SDLK_KP8) movepckeydir(6);
    if(sym == 'u' || sym == 'e' || sym == SDLK_KP9) movepckeydir(7);
    }

#ifdef PANDORA
  if(DEFAULTCONTROL) {
    if(sym == SDLK_RIGHT) movepckeydir(0);
    if(sym == SDLK_LEFT) movepckeydir(4);
    if(sym == SDLK_DOWN) movepckeydir(2 + (leftclick?1:0) - (rightclick?1:0));
    if(sym == SDLK_UP) movepckeydir(6 - (leftclick?1:0) + (rightclick?1:0));
    }
#endif

  if(uni == sym && DEFAULTNOR(sym)) {
    gmodekeys(sym, uni);
    if(sym == '8') {
      backcolor = backcolor ^ 0xFFFFFF;
      bordcolor = bordcolor ^ 0xFFFFFF;
      forecolor = forecolor ^ 0xFFFFFF;
      printf("back = %x\n", backcolor);
      }
    if(sym == '9') {
      pmodel = eModel(8 - pmodel);
      // vid.yshift = 1 - vid.yshift;
      // vid.drawmousecircle = true;
      }
    if(sym == 'm' && canmove && (centerover == cwt.c ? mouseover : centerover))
      performMarkCommand(mouseover);
    }
  
  if(DEFAULTCONTROL) {  
    if(sym == '.' || sym == 's') movepcto(-1, 1);
    if((sym == SDLK_DELETE || sym == SDLK_KP_PERIOD || sym == 'g') && uni != 'G' && uni != 'G'-64) 
      movepcto(MD_DROP, 1);
    if(sym == 't' && uni != 'T' && uni != 'T'-64 && canmove) {
      if(playermoved && items[itStrongWind]) {
        cell *c = whirlwind::jumpDestination(cwt.c);
        if(c) centerover = c;
        }
      targetRangedOrb(centerover, roKeyboard);
      sym = 0; uni = 0;
      }
    }

  if(sym == SDLK_KP5 && DEFAULTCONTROL) movepcto(-1, 1);

  // if(sym == SDLK_F4)  restartGameSwitchEuclid();

  if(sym == SDLK_F5) {
    if(needConfirmation()) 
      pushScreen(showMission);
    else restartGame();
    }

  if(sym == SDLK_ESCAPE)
    showMissionScreen();

  if(sym == SDLK_F10) {
    if(needConfirmation()) pushScreen(showMission);
    else quitmainloop = true;
    }
  
  if(uni == 'o' && DEFAULTNOR(sym)) setAppropriateOverview();
#ifdef INV
  if(uni == 'i' && DEFAULTNOR(sym) && inv::on) 
    pushScreen(inv::show);
#endif
  
  if((sym == SDLK_HOME || sym == SDLK_F3 || sym == ' ') && DEFAULTNOR(sym)) 
    fullcenter();
  
  if(sym == 'v' && DEFAULTNOR(sym)) 
    pushScreen(showMainMenu);
  
  if(sym == '-' || sym == PSEUDOKEY_WHEELDOWN) {
    actonrelease = false;
    
    shmup::cpid = 0;
    if(mouseover && 
      targetclick && (!shmup::on || numplayers() == 1) && targetRangedOrb(mouseover, forcetarget ? roMouseForce : roMouse)) {
      }
    else if(forcetarget)
      ;
    else if(!DEFAULTCONTROL) {
      if(!shmup::on)
        multi::mousemovement(mouseover);
      }
    else mousemovement();
    }

  if(sym == SDLK_F1) gotoHelp(help);

#ifdef ROGUEVIZ
  rogueviz::processKey(sym, uni);
#endif
  }

void handlekey(int sym, int uni) {

  if(callhandlers(false, hooks_handleKey, sym, uni)) return;

  keyhandler(sym, uni);
  }

#ifdef NOSDL
void mainloopiter() { printf("(compiled without SDL -- no action)\n"); quitmainloop = true; }
#else

// Warning: a very long function! todo: refactor

int cframelimit = 1000;

void mainloopiter() {

  DEBB(DF_GRAPH, (debugfile,"main loop\n"));

  #ifndef GFX
  #ifndef GL
  vid.wallmode = 0;
  vid.monmode = 0;
  #endif
  #endif

  optimizeview();

  if(conformal::on) conformal::apply();
  
  ticks = SDL_GetTicks();
    
  int timetowait = lastt + 1000 / cframelimit - ticks;

  cframelimit = vid.framelimit;
  if(outoffocus && cframelimit > 10) cframelimit = 10;  
  
  bool normal = cmode & sm::NORMAL;

  if(DOSHMUP && normal) 
    timetowait = 0, shmup::turn(ticks - lastt);
    
  if(!DOSHMUP && (multi::alwaysuse || multi::players > 1) && normal)
    timetowait = 0, multi::handleMulti(ticks - lastt);

  if(vid.sspeed >= 5 && gmatrix.count(cwt.c) && !elliptic) {
    cwtV = gmatrix[cwt.c] * ddspin(cwt.c, cwt.spin);
    if(cwt.mirrored) playerV = playerV * Mirror;
    }

#ifdef WEB
  if(playermoved && vid.sspeed > -4.99 && !outoffocus) {
    centerpc((ticks - lastt) / 1000.0 * exp(vid.sspeed));
    }
  if(!outoffocus) drawscreen();
#else
  if(timetowait > 0)
    SDL_Delay(timetowait);
  else {
    if(cmode & sm::CENTER) {
      if(playermoved && vid.sspeed > -4.99 && !outoffocus)
        centerpc((ticks - lastt) / 1000.0 * exp(vid.sspeed));
      if(panjoyx || panjoyy) 
        checkpanjoy((ticks - lastt) / 1000.0);
      }
    tortoise::updateVals(ticks - lastt);
    frames++;
    if(!outoffocus) {
      drawscreen();
      }
    lastt = ticks;
    }      
#endif

  Uint8 *keystate = SDL_GetKeyState(NULL);
  rightclick = keystate[SDLK_RCTRL];
  leftclick = keystate[SDLK_RSHIFT];
  lctrlclick = keystate[SDLK_LCTRL];
  lshiftclick = keystate[SDLK_LSHIFT];
  forcetarget = (keystate[SDLK_RSHIFT] | keystate[SDLK_LSHIFT]);
  hiliteclick = keystate[SDLK_LALT] | keystate[SDLK_RALT];
  anyshiftclick = keystate[SDLK_LSHIFT] | keystate[SDLK_RSHIFT];
  wheelclick = false;

  getcshift = 1;
  if(keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]) getcshift = -1;
  if(keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL]) getcshift /= 10;
  if(keystate[SDLK_LALT] || keystate[SDLK_RALT]) getcshift *= 10;
  
  didsomething = false;
  
  if(vid.shifttarget&1) {
    leftclick = false;
    targetclick = keystate[SDLK_RSHIFT] | keystate[SDLK_LSHIFT];
    }
  else {
    leftclick = keystate[SDLK_RSHIFT];
    targetclick = true;
    }
  
#ifdef SDLAUDIO
  if(audio) handlemusic();
#endif
  SDL_Event ev;
  DEBB(DF_GRAPH, (debugfile,"polling for events\n"));
  
  achievement_pump();
  while(SDL_PollEvent(&ev)) {
    DEBB(DF_GRAPH, (debugfile,"got event type #%d\n", ev.type));
    int sym = 0;
    int uni = 0;
    shiftmul = 1;
    
/*    if(ev.type == SDL_JOYDEVICEADDED || ev.type == SDL_JOYDEVICEREMOVED) {
      joyx = joyy = 0;
      panjoyx = panjoyy = 0;
      closeJoysticks();
      initJoysticks();
      }*/

    if(ev.type == SDL_ACTIVEEVENT) {
      if(ev.active.state & SDL_APPINPUTFOCUS) {
        if(ev.active.gain) {
          outoffocus = false;
          }
        else {
          outoffocus = true;
          }
        }
      }
    
    if(ev.type == SDL_VIDEORESIZE) {
      vid.xres = ev.resize.w;
      vid.yres = ev.resize.h;
      vid.killreduction = 0;
      extern bool setfsize;
      setfsize = true;
      setvideomode();
#ifdef GL
      if(vid.usingGL) glViewport(0, 0, vid.xres, vid.yres);
#endif
      }
    
    if(ev.type == SDL_VIDEOEXPOSE) {
      drawscreen();
      }
    
    if(ev.type == SDL_JOYAXISMOTION) {
      if(ev.jaxis.which == 0) {
        if(ev.jaxis.axis == 0)
          joyx = ev.jaxis.value;
        else if(ev.jaxis.axis == 1)
          joyy = ev.jaxis.value;
        else if(ev.jaxis.axis == 3)
          panjoyx = ev.jaxis.value;
        else if(ev.jaxis.axis == 4)
          panjoyy = ev.jaxis.value;
        checkjoy();
        // printf("panjoy = %d,%d\n", panjoyx, panjoyy);
        }
      else {
        if(ev.jaxis.axis == 0)
          panjoyx = ev.jaxis.value;
        else 
          panjoyy = ev.jaxis.value;
        }
      }
    
    bool shmupconf = cmode & sm::SHMUPCONFIG;

    if(ev.type == SDL_JOYBUTTONDOWN && shmupconf && vid.scfg.setwhat) {
      int joyid = ev.jbutton.which;
      int button = ev.jbutton.button;
      if(joyid < 8 && button < 32)
         vid.scfg.joyaction[joyid][button] = vid.scfg.setwhat;
      vid.scfg.setwhat = 0;
      }

    else if(ev.type == SDL_JOYHATMOTION && shmupconf && vid.scfg.setwhat) {
      int joyid = ev.jhat.which;
      int hat = ev.jhat.hat;
      int dir = 4;
      if(ev.jhat.value == SDL_HAT_UP) dir = 0;
      if(ev.jhat.value == SDL_HAT_RIGHT) dir = 1;
      if(ev.jhat.value == SDL_HAT_DOWN) dir = 2;
      if(ev.jhat.value == SDL_HAT_LEFT) dir = 3;
      if(joyid < 8 && hat < 4 && dir < 4) {
        vid.scfg.hataction[joyid][hat][dir] = vid.scfg.setwhat;
        vid.scfg.setwhat = 0;
        }
      }

    else if(ev.type == SDL_JOYBUTTONDOWN && DEFAULTCONTROL) {
      flashMessages();
      movepcto(joydir);
      checkjoy();
      }

    if(ev.type == SDL_KEYDOWN) {
      flashMessages();
      mousing = false;
      sym = ev.key.keysym.sym;
      uni = ev.key.keysym.unicode;
      if(ev.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) shiftmul = -1;
      if(ev.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) shiftmul /= 10;
      if(sym == SDLK_RETURN && (ev.key.keysym.mod & (KMOD_LALT | KMOD_RALT))) {
        sym = 0; uni = 0;
        switchFullscreen();
        }
      }
    
    dialog::handleZooming(ev);
    
    if(sym == SDLK_F1 && normal && playermoved)
      help = "@";
    
    bool rollchange = (cmode & sm::OVERVIEW) && getcstat >= 2000 && cheater;

    if(ev.type == SDL_MOUSEBUTTONDOWN) {
      flashMessages();
      mousepressed = true;
      mousing = true;
      actonrelease = true;
      
      if(ev.button.button==SDL_BUTTON_WHEELDOWN) {
        sym = uni = PSEUDOKEY_WHEELDOWN;
        }
      if(ev.button.button==SDL_BUTTON_WHEELUP) {
        sym = uni = PSEUDOKEY_WHEELUP;
        }
      else if(ev.button.button == SDL_BUTTON_RIGHT) {
        sym = 1; didsomething = true;
        }
      else if(ev.button.button == SDL_BUTTON_MIDDLE) {
        sym = 2; didsomething = true;
        }
      }

    if(ev.type == SDL_MOUSEBUTTONUP) {
      mousepressed = false;
      mousing = true;
      if(ev.button.button==SDL_BUTTON_RIGHT || leftclick) 
        sym = SDLK_F1;
      else if(ev.button.button==SDL_BUTTON_MIDDLE || rightclick) 
        sym = 1, didsomething = true;
      else if(ev.button.button == SDL_BUTTON_LEFT && actonrelease) {
        sym = getcstat, uni = getcstat, shiftmul = getcshift;
        }
      else if(ev.button.button == SDL_BUTTON_WHEELUP && rollchange) {
        sym = getcstat, uni = getcstat, shiftmul = getcshift, wheelclick = true;
        }
      else if(ev.button.button == SDL_BUTTON_WHEELDOWN && rollchange) {
        sym = getcstat, uni = getcstat, shiftmul = -getcshift, wheelclick = true;
        }
      }

    if(ev.type == SDL_MOUSEMOTION) {
      mouseoh = mouseh;
      
      mousing = true;
      mousemoved = true;
      mousex = ev.motion.x;
      mousey = ev.motion.y;

#ifndef NORUG
      if(rug::rugged)
        mouseh = rug::gethyper(mousex, mousey);
      else
#endif
        mouseh = gethyper(mousex, mousey);

      if((rightclick || (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_MMASK)) && !mouseout2() && 
         mouseh[2] < 50 && mouseoh[2] < 50) {
        panning(mouseoh, mouseh);
        }

#ifdef SIMULATE_JOYSTICK
      // pretend that both joysticks are present
      stick = panstick = (SDL_Joystick*) (&vid);
      panjoyx = 20 * (mousex - vid.xcenter);
      panjoyy = 20 * (mousey - vid.ycenter);
      checkjoy();
#endif

      if(mousepressed && inslider) {
        sym = getcstat, uni = getcstat, shiftmul = getcshift;
        }
      }

    if(ev.type == SDL_QUIT) {
      if(needConfirmation() && !(cmode & sm::MISSION)) showMissionScreen();
      else quitmainloop = true;
      }

    handlekey(sym, uni);
    }
  }
#endif

void mainloop() {
  lastt = 0;
#ifdef WEB
  initweb();
  emscripten_set_main_loop(mainloopiter, 0, true);
#else
  while(!quitmainloop) mainloopiter();
#endif
  }

#ifdef MOBILE
void displayabutton(int px, int py, string s, int col) {
  // TMP
  int siz = vid.yres > vid.xres ? vid.fsize*2 : vid.fsize * 3/2;
  int vrx = min(vid.radius, vid.xres/2 - 40);
  int vry = min(vid.radius, min(vid.ycenter, vid.yres - vid.ycenter) - 20);
  int x = vid.xcenter + px * vrx;
  int y = vid.ycenter + py * (vry - siz/2);
  int vrr = int(hypot(vrx, vry) * sqrt(2.));
  if(gtouched && !mouseover
    && abs(mousex - vid.xcenter) < vrr
    && abs(mousey - vid.ycenter) < vrr
    && hypot(mousex-vid.xcenter, mousey-vid.ycenter) > vrr
    && px == (mousex > vid.xcenter ? 1 : -1)
    && py == (mousey > vid.ycenter ? 1 : -1)
    ) col = 0xFF0000;
  if(displayfr(x, y, 0, siz, s, col, 8+8*px))
    buttonclicked = true;
  }
#endif

void gmodekeys(int sym, int uni) {
  if(uni == '1') { vid.alpha = 999; vid.scale = 998; }
  if(uni == '2') { vid.alpha = 1; vid.scale = 0.4; }
  if(uni == '3') { vid.alpha = 1; vid.scale = 1; }
  if(uni == '4') { vid.alpha = 0; vid.scale = 1; }
  if(uni == '5') { vid.wallmode += 60 + (shiftmul > 0 ? 1 : -1); vid.wallmode %= 6; }
  if(uni == '6') vid.grid = !vid.grid;
  if(uni == '7') { vid.darkhepta = !vid.darkhepta; }
  if(uni == '%' && sym == '5') { 
    if(vid.wallmode == 0) vid.wallmode = 6;
    vid.wallmode--;
    }
  }

