// Hyperbolic Rogue -- control
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

int frames;
bool outoffocus = false;

int mousex, mousey;
hyperpoint mouseh, mouseoh;

bool leftclick, rightclick, targetclick, hiliteclick, anyshiftclick, wheelclick,
  forcetarget, lshiftclick, lctrlclick;
bool gtouched;

bool holdmouse;

int getcstat, lgetcstat; ld getcshift; bool inslider;

function <void(int sym, int uni)> keyhandler = [] (int sym, int uni) {};

// is the player using mouse? (used for auto-cross)
bool mousing = true;

// is the mouse button pressed?
bool mousepressed = false;
bool mousemoved = false;
bool actonrelease = false;

#if CAP_SDLJOY
int joyx, joyy, panjoyx, panjoyy; 
movedir joydir;
#endif

movedir mousedest;
ld shiftmul = 1;

cell *mouseover, *mouseover2, *lmouseover;
cellwalker centerover;
ld modist, modist2, centdist;

int lastt;

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
  
  if(euclid) Centered = pispin * Centered;
  
  ld dirdist[MAX_EDGE];
  for(int i=0; i<cwt.c->type; i++) {
    dirdist[i] = intval(Centered * ddspin(cwt.c, i, 0) * xpush(-.5) * C0, P);
    //xspinpush0(-i * 2 * M_PI /cwt.c->type, .5), P);
    }
    
  movedir res;
  res.d = -1;
  
  for(int i=0; i<cwt.c->type; i++) {
    if(dirdist[i] < binv) {
      binv = dirdist[i];
      res.d = i;
      res.subdir = dirdist[(i+1)%cwt.c->type] < dirdist[(i+cwt.c->type-1)%cwt.c->type] ? 1 : -1;
      }
    }
  
  // if(euclid) bdir = (bdir + 3) % 6;
  return res;
  }

void remission() {
  if(!canmove && (cmode & sm::NORMAL)) showMissionScreen();
 }
 
void movepckeydir(int d) {
  DEBB(DF_GRAPH, (debugfile,"movepckeydir\n"));
  // EUCLIDEAN

  movedir md = 
    vectodir(spin(-d * M_PI/4) * tC0(pushone()));
    
  if(!canmove) movepcto(md), remission(); else movepcto(md);
  }

void calcMousedest() {
  if(mouseout()) return;
  if(vid.revcontrol == true) { mouseh[0] = -mouseh[0]; mouseh[1] = -mouseh[1]; }
  ld mousedist = intval(mouseh, tC0(shmup::ggmatrix(cwt.c)));
  mousedest.d = -1;
  
  cellwalker bcwt = cwt;
  
  ld dists[MAX_EDGE];
  
  for(int i=0; i<cwt.c->type; i++)
    dists[i] = intval(mouseh, tC0(confusingGeometry() ? shmup::ggmatrix(cwt.c) * shmup::calc_relative_matrix(cwt.c->mov[i], cwt.c, i) : shmup::ggmatrix(cwt.c->mov[i])));
  
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
    }
  
  if(vid.revcontrol == true) { mouseh[0] = -mouseh[0]; mouseh[1] = -mouseh[1]; }
  cwt = bcwt;
  }

void mousemovement() {
  calcMousedest();
    if(!canmove) movepcto(mousedest), remission(); else movepcto(mousedest);
  lmouseover = NULL;
  }

#if CAP_SDLJOY
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
  if(sym == PSEUDOKEY_RELEASE) return false;
  if(uni != 0) return true;
  return false;
  }

bool didsomething;

typedef SDL_Event eventtype;

void handlePanning(int sym, int uni) {
  if(rug::rugged) return;

#if !ISPANDORA
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
      View = spin(M_PI/S21/2*shiftmul) * View, didsomething = true;
    }
  if(sym == SDLK_PAGEDOWN) {
    if(conformal::on)
      conformal::rotation++;
    else
      View = spin(-M_PI/S21/2*shiftmul) * View, didsomething = true;
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

#ifdef SCALETUNER
double tunev = .1;

bool handleTune(int sym, int uni) {
  if(uni == 'q') { tunev *= .5; return true; }
  else if(uni == 'e') { tunev *= 2; return true; }
  else if(uni == 'w') bscale7 += tunev*shiftmul;
  else if(uni == 's') bscale7 -= tunev*shiftmul;
  else if(uni == 'a') brot7 -= tunev*shiftmul;
  else if(uni == 'd') brot7 += tunev*shiftmul;
  else if(uni == 'i') bscale6 += tunev*shiftmul;
  else if(uni == 'k') bscale6 -= tunev*shiftmul;
  else if(uni == 'j') brot6 -= tunev*shiftmul;
  else if(uni == 'l') brot6 += tunev*shiftmul;
  else if(uni == 'z')
    bscale7 = bscale6 = 1, brot7 = brot6 = 0;
  else return false;
  printf("s7 %lf r7 %lf s6 %lf r6 %lf\n", bscale7, brot7, bscale6, brot6);
  resetGeometry();
  return true;
  }
#endif

purehookset hooks_fixticks;
  
void handleKeyNormal(int sym, int uni) {

  if(cheater && sym < 256 && sym > 0) {
    if(applyCheat(uni, mouseover))
      sym = 0;
    }

  if(DEFAULTNOR(sym)) handlePanning(sym, uni);
  
#ifdef SCALETUNER
  if(handleTune(sym, uni)) return;
#endif

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

#if ISPANDORA
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
    if(sym == 'm' && canmove && (centerover == cwt ? mouseover : centerover.c))
      performMarkCommand(mouseover);
    }
  
  if(DEFAULTCONTROL) {
    if(sym == '.' || sym == 's') movepcto(-1, 1);
    if((sym == SDLK_DELETE || sym == SDLK_KP_PERIOD || sym == 'g') && uni != 'G' && uni != 'G'-64) 
      movepcto(MD_DROP, 1);
    if(sym == 't' && uni != 'T' && uni != 'T'-64 && canmove) {
      if(playermoved && items[itStrongWind]) { 
        cell *c = whirlwind::jumpDestination(cwt.c);
        if(c) centerover.c = c;
        }
      targetRangedOrb(centerover.c, roKeyboard);
      sym = 0; uni = 0;
      }
    }

  if(sym == SDLK_KP5 && DEFAULTCONTROL) movepcto(-1, 1);

  if(sym == SDLK_F5) {
    #if CAP_DAILY
    if(daily::on) daily::handleQuit(1);
    else
    #endif
    if(needConfirmation()) 
      pushScreen(showMission);
    else restart_game();
    }

  if(sym == SDLK_ESCAPE) {
    if(viewdists)
      viewdists = false;
    else
      showMissionScreen();
    }

  if(sym == SDLK_F10) {
    #if CAP_DAILY
    if(daily::on) daily::handleQuit(2);
    else
    #endif
    if(needConfirmation()) pushScreen(showMission);
    else quitmainloop = true;
    }
  
  if(uni == 'o' && DEFAULTNOR(sym)) setAppropriateOverview();
#if CAP_INV
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
    else if(rug::rugged && rug::renderonce)
      ;
    else if(!DEFAULTCONTROL) {
      if(!shmup::on)
        multi::mousemovement(mouseover);
      }
    else if(handleCompass()) ;
    else
      mousemovement();
    }

  if(sym == SDLK_F1) gotoHelp(help);

#if CAP_ROGUEVIZ
  rogueviz::processKey(sym, uni);
#endif
  }

bool need_mouseh = false;

void fix_mouseh() {
  if(!need_mouseh) ;
#if CAP_RUG
  else if(rug::rugged)
    mouseh = rug::gethyper(mousex, mousey);
#endif
  else
    mouseh = gethyper(mousex, mousey);
  need_mouseh = false;
  }

void handlekey(int sym, int uni) {

  if(callhandlers(false, hooks_handleKey, sym, uni)) return;

  keyhandler(sym, uni);
  }

#if !CAP_SDL
void mainloopiter() { printf("(compiled without SDL -- no action)\n"); quitmainloop = true; }
#else

// Warning: a very long function! todo: refactor

int cframelimit = 1000;

void resize_screen_to(int x, int y) {
  vid.xres = x;
  vid.yres = y;
  vid.killreduction = 0;
  extern bool setfsize;
  setfsize = true;
  setvideomode();
  }

void mainloopiter() {

  DEBB(DF_GRAPH, (debugfile,"main loop\n"));

  #if !CAP_SDLGFX && !CAP_GL 
  vid.wallmode = 0;
  vid.monmode = 0;
  #endif

  optimizeview();

  if(conformal::on) conformal::apply();
  
  ticks = SDL_GetTicks();
  callhooks(hooks_fixticks);
    
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

#if ISWEB
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
  
#if CAP_SDLAUDIO
  if(audio) handlemusic();
#endif
  SDL_Event ev;
  DEBB(DF_GRAPH, (debugfile,"polling for events\n"));
  
  achievement_pump();  
  while(SDL_PollEvent(&ev)) handle_event(ev);
  fix_mouseh();
  }
  
void handle_event(SDL_Event& ev) {
  bool normal = cmode & sm::NORMAL;
  Uint8 *keystate = SDL_GetKeyState(NULL);

    DEBB(DF_GRAPH, (debugfile,"got event type #%d\n", ev.type));
    int sym = 0;
    int uni = 0;
    shiftmul = 1;
    
/*    if(ev.type == SDL_JOYDEVICEADDED || ev.type == SDL_JOYDEVICEREMOVED) {
      joyx = joyy = 0;
      panjoyx = panjoyy = 0;
      closeJoysticks();
      initJoysticks();
      } */

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
    
    if(ev.type == SDL_VIDEORESIZE) 
      resize_screen_to(ev.resize.w, ev.resize.h);
    
    if(ev.type == SDL_VIDEOEXPOSE) {
      drawscreen();
      }

#if CAP_SDLJOY    
    if(ev.type == SDL_JOYAXISMOTION && normal && DEFAULTCONTROL) {
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
      printf("%d %d %d\n", joyid, hat, dir);
      if(joyid < 8 && hat < 4 && dir < 4) {
        vid.scfg.hataction[joyid][hat][dir] = vid.scfg.setwhat;
        vid.scfg.setwhat = 0;
        }
      }

    else if(ev.type == SDL_JOYHATMOTION && !normal) {
      if(ev.jhat.value == SDL_HAT_UP) sym = SDLK_UP;
      if(ev.jhat.value == SDL_HAT_DOWN) sym = SDLK_DOWN;
      if(ev.jhat.value == SDL_HAT_LEFT) sym = SDLK_LEFT;
      if(ev.jhat.value == SDL_HAT_RIGHT) sym = SDLK_RIGHT;
      }

    else if(ev.type == SDL_JOYBUTTONDOWN && normal && DEFAULTCONTROL) {
      flashMessages();
      movepcto(joydir);
      checkjoy();
      }

    else if(ev.type == SDL_JOYBUTTONDOWN && !normal) {
      sym = uni = SDLK_RETURN;
      }
#endif

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

    bool anyctrl = keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL];
    bool anyshift = keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT];

    if(ev.type == SDL_MOUSEBUTTONDOWN || ev.type == SDL_MOUSEBUTTONUP) {
      mousepressed = ev.type == SDL_MOUSEBUTTONDOWN;
      if(mousepressed) flashMessages();
      mousing = true;
      bool was_holdmouse = holdmouse;
      holdmouse = false;
      
      bool act = false;
      
      if(vid.quickmouse) {
        act = ev.type == SDL_MOUSEBUTTONDOWN;
        }
      else {
        act = actonrelease && ev.type == SDL_MOUSEBUTTONUP;
        actonrelease = ev.type == SDL_MOUSEBUTTONDOWN;
        }      
      
      fix_mouseh();
      
      if(was_holdmouse && ev.type == SDL_MOUSEBUTTONUP)
        sym = uni = PSEUDOKEY_RELEASE;
      
      if(!act) ;

      else if(ev.button.button==SDL_BUTTON_RIGHT || leftclick) 
        sym = SDLK_F1;
      else if(ev.button.button==SDL_BUTTON_MIDDLE || rightclick) {
        sym = 1, didsomething = true;
        if(anyshift)
          vid.xposition = vid.yposition = 0;
        }
      else if(ev.button.button == SDL_BUTTON_LEFT) {
        sym = getcstat, uni = getcstat, shiftmul = getcshift;
        }
      
      else if(ev.button.button==SDL_BUTTON_WHEELDOWN) {
        if(anyctrl && anyshift && !rug::rugged) {
          mapeditor::scaleall(1/1.2);
          vid.alpha /= 1.2;
          }
        else if(anyctrl && !rug::rugged)
          mapeditor::scaleall(pow(2, -.25));
        else if(anyshift && !rug::rugged)
          vid.alpha -= 0.25;
        else if(rollchange) {
          sym = getcstat, uni = getcstat, shiftmul = getcshift, wheelclick = true;
          }
        else {
          sym = uni = PSEUDOKEY_WHEELDOWN;
          }
        }
      if(ev.button.button==SDL_BUTTON_WHEELUP) {
        if(anyctrl && anyshift && !rug::rugged) {
          mapeditor::scaleall(1.2);
          vid.alpha *= 1.2;
          }
        else if(anyctrl && !rug::rugged)
          mapeditor::scaleall(pow(2, .25));
        else if(anyshift && !rug::rugged)
          vid.alpha += 0.25;
        else if(rollchange) {
          sym = getcstat, uni = getcstat, shiftmul = -getcshift, wheelclick = true;
          }
        else {
          sym = uni = PSEUDOKEY_WHEELUP;
          }
        }
      }

    if(ev.type == SDL_MOUSEMOTION) {
      mouseoh = mouseh;
      
      int lmousex = mousex, lmousey = mousey;
      
      mousing = true;
      mousemoved = true;
      mousex = ev.motion.x;
      mousey = ev.motion.y;
      
      need_mouseh = true;

      if(holdmouse && getcstat == '-') sym = uni = getcstat, fix_mouseh();

      if((rightclick || (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_MMASK)) && !mouseout2()) {
        fix_mouseh();
        if(anyctrl) {
          vid.xposition += (mousex - lmousex) * 1. / vid.scrsize,
          vid.yposition += (mousey - lmousey) * 1. / vid.scrsize;
          }
        else if(mouseh[2] < 50 && mouseoh[2] < 50) {
          panning(mouseoh, mouseh);
          }
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
      #if CAP_DAILY
      if(daily::on) daily::handleQuit(3);
      else
      #endif
      if(needConfirmation() && !(cmode & sm::MISSION)) showMissionScreen();
      else quitmainloop = true;
      }
    
    if(sym == SDLK_F4 && anyshift) {
      nomap = !nomap;
      sym = 0;
      }
      
    if(sym == SDLK_F2 && anyshift) {
      nohud = !nohud;
      sym = 0;
      }
      
    if(sym == SDLK_F3 && anyshift) {
      nofps = !nofps;
      sym = 0;
      }
      
    handlekey(sym, uni);
    }
#endif

void mainloop() {
  if(noGUI) return;
  lastt = 0;
#if ISWEB
  initweb();
  emscripten_set_main_loop(mainloopiter, 0, true);
#else
  while(!quitmainloop) mainloopiter();
#endif
  }

#if ISMOBILE==1
void displayabutton(int px, int py, string s, int col) {
  // TMP
  int siz = vid.yres > vid.xres ? vid.fsize*2 : vid.fsize * 3/2;
  int rad = (int) realradius();
  if(stereo::mode == stereo::sLR) rad = 99999;
  int vrx = min(rad, vid.xres/2 - 40);
  int vry = min(rad, min(vid.ycenter, vid.yres - vid.ycenter) - 20);
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
  #if CAP_RUG
  if(rug::rugged) rug::handlekeys(sym, uni);
  #endif
  if(uni == '1' && !rug::rugged) { vid.alpha = 999; vid.scale = 998; vid.xposition = vid.yposition = 0; }
  if(uni == '2' && !rug::rugged) { vid.alpha = 1; vid.scale = 0.4; vid.xposition = vid.yposition = 0; }
  if(uni == '3' && !rug::rugged) { vid.alpha = 1; vid.scale = 1; vid.xposition = vid.yposition = 0; }
  if(uni == '4' && !rug::rugged) { vid.alpha = 0; vid.scale = 1; vid.xposition = vid.yposition = 0; }
  if(uni == '5') { vid.wallmode += 60 + (shiftmul > 0 ? 1 : -1); vid.wallmode %= 6; }
  if(uni == '6') vid.grid = !vid.grid;
  if(uni == '7') { vid.darkhepta = !vid.darkhepta; }
  if(uni == '%' && sym == '5') { 
    if(vid.wallmode == 0) vid.wallmode = 6;
    vid.wallmode--;
    }
  }

bool haveMobileCompass() {
#if CAP_MOBILE
  if(andmode || useRangedOrb) return false;
#else
  if(forcetarget) return false;
#endif
  return canmove && !shmup::on && vid.mobilecompasssize > 0 && isize(screens) == 1;
  }
  
bool handleCompass() {
  if(!haveMobileCompass()) return false;
  
  using namespace shmupballs;

  int dx = mousex - xmove;
  int dy = mousey - yb;
  int h = hypot(dx, dy);
  if(h < rad) {
    if(h < rad*SKIPFAC) movepcto(MD_WAIT);
    else {
      double d = vid.revcontrol ? -1 : 1;
      mouseh = hpxy(dx * d / rad, dy * d / rad);
      mousemovement();
      }
    getcstat = 0;
    return true;
    }

  return false;
  }

}
