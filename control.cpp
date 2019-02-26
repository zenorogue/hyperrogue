// Hyperbolic Rogue -- control
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

int frames;
bool outoffocus = false;

int mousex, mousey;
hyperpoint mouseh, mouseoh;

bool leftclick, rightclick, targetclick, hiliteclick, anyshiftclick, wheelclick,
  forcetarget, lshiftclick, lctrlclick, numlock_on;
bool gtouched;

bool holdmouse;

int getcstat, lgetcstat; ld getcshift; bool inslider;

function <void(int sym, int uni)> keyhandler = [] (int sym, int uni) {};
function <bool(SDL_Event &ev)> joyhandler = [] (SDL_Event &ev) {return false;};


// is the player using mouse? (used for auto-cross)
bool mousing = true;

// is the mouse button pressed?
bool mousepressed = false;
bool mousemoved = false;
bool actonrelease = false;

int timetowait;

#if CAP_SDLJOY
int joyx, joyy, panjoyx, panjoyy; 
movedir joydir;
#endif

movedir mousedest;
ld shiftmul = 1;

cell *mouseover, *mouseover2, *lmouseover;
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

  transmatrix U = ggmatrix(cwt.at);

  hyperpoint H = sphereflip * tC0(U);
  transmatrix Centered = sphereflip * rgpushxto0(H);

  ld binv = 99;
  
  ld dirdist[MAX_EDGE];

  for(int i=0; i<cwt.at->type; i++) {
    transmatrix T;
    if(compute_relamatrix((cwt+i).peek(), cwt.at, i, T)) {
      dirdist[i] = intval(U * T * C0, Centered * P);
      }
    //xspinpush0(-i * 2 * M_PI /cwt.at->type, .5), P);
    }
    
  movedir res;
  res.d = -1;
  
  for(int i=0; i<cwt.at->type; i++) {
    if(dirdist[i] < binv) {
      binv = dirdist[i];
      res.d = i;
      res.subdir = dirdist[(i+1)%cwt.at->type] < dirdist[(i+cwt.at->type-1)%cwt.at->type] ? 1 : -1;
      }
    }

  // if(euclid) bdir = (bdir + 3) % 6;
  return res;
  }

void remission() {
  if(!canmove && (cmode & sm::NORMAL)) showMissionScreen();
 }

hyperpoint move_destination_vec(int d) {
  if(DIM == 2) return spin(-d * M_PI/4) * tC0(pushone());
  else return cspin(0, 2, d * M_PI/4) * tC0(pushone());
  }

void movepckeydir(int d) {
  DEBB(DF_GRAPH, (debugfile,"movepckeydir\n"));
  // EUCLIDEAN
  
  movedir md = vectodir(move_destination_vec(d));
    
  if(!canmove) movepcto(md), remission(); else movepcto(md);
  }

void calcMousedest() {
  if(mouseout()) return;
  if(vid.revcontrol == true) { mouseh[0] = -mouseh[0]; mouseh[1] = -mouseh[1]; }
  ld mousedist = intval(mouseh, tC0(ggmatrix(cwt.at)));
  mousedest.d = -1;
  
  cellwalker bcwt = cwt;
  
  ld dists[MAX_EDGE];
  
  transmatrix U = ggmatrix(cwt.at);
  
  for(int i=0; i<cwt.at->type; i++) {
    transmatrix T;
    if(compute_relamatrix(cwt.at->move(i), cwt.at, i, T))
      dists[i] = intval(mouseh, U * T * C0);
    else
      dists[i] = HUGE_VAL;
    }
  // confusingGeometry() ? ggmatrix(cwt.at) * calc_relative_matrix(cwt.at->move(i), cwt.at, i) : shmup::ggmatrix(cwt.at->move(i))));
  
  /* printf("curcell = %Lf\n", mousedist);
  for(int i=0; i<cwt.at->type; i++)
    printf("d%d = %Lf\n", i, dists[i]); */

  for(int i=0; i<cwt.at->type; i++) if(dists[i] < mousedist) {
    mousedist = dists[i];
    mousedest.d = fixdir(i - cwt.spin, cwt.at);
    
    mousedest.subdir =
       dists[(i+1)%cwt.at->type] < dists[(i+cwt.at->type-1)%cwt.at->type] ? 1 : -1;

    if(cwt.mirrored) 
      mousedest.d = fixdir(-mousedest.d, cwt.at), 
      mousedest.subdir = -mousedest.subdir;
    }
  
  if(vid.revcontrol == true) { mouseh[0] = -mouseh[0]; mouseh[1] = -mouseh[1]; }
  cwt = bcwt;
  }

void mousemovement() {
  if(DIM == 3) return;
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
  #ifndef FAKE_SDL
  if(sym == SDLK_LSHIFT) return false;
  if(sym == SDLK_RSHIFT) return false;
  if(sym == SDLK_LCTRL) return false;
  if(sym == SDLK_RCTRL) return false;
  if(sym == SDLK_LALT) return false;
  if(sym == SDLK_RALT) return false;
  #endif
  if(uni != 0) return true;
  return false;
  }

bool didsomething;

typedef SDL_Event eventtype;

bool smooth_scrolling = false;

void handlePanning(int sym, int uni) {
  if(rug::rugged || smooth_scrolling) return;
  
#if !ISPANDORA
  if(sym == SDLK_END && DIM == 3) { 
    View = cpush(2, -0.2*shiftmul) * View, didsomething = true, playermoved = false;
    }
  if(sym == SDLK_HOME && DIM == 3) { 
    View = cpush(2, 0.2*shiftmul) * View, didsomething = true, playermoved = false;
    }
  if(sym == SDLK_RIGHT) { 
    if(conformal::on)
      conformal::lvspeed += 0.1 * shiftmul;
    else if(DIM == 3)
      View = cspin(0, 2, -0.2*shiftmul) * View, didsomething = true;
    else
      View = xpush(-0.2*shiftmul) * View, playermoved = false, didsomething = true;
    }
  if(sym == SDLK_LEFT) {
    if(conformal::on)
      conformal::lvspeed -= 0.1 * shiftmul;
    else if(DIM == 3)
      View = cspin(0, 2, 0.2*shiftmul) * View, didsomething = true;
    else
      View = xpush(+0.2*shiftmul) * View, playermoved = false, didsomething = true;
    }
  if(sym == SDLK_UP) {
    if(conformal::on)
      conformal::lvspeed += 0.1 * shiftmul;
    else if(DIM == 3)
      View = cspin(1, 2, 0.2*shiftmul) * View, didsomething = true;
    else
      View = ypush(+0.2*shiftmul) * View, playermoved = false, didsomething = true;
    }
  if(sym == SDLK_DOWN) {
    if(conformal::on)
      conformal::lvspeed -= 0.1 * shiftmul;
    else if(DIM == 3)
      View = cspin(1, 2, -0.2*shiftmul) * View, didsomething = true;
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
    if(isGravityLand(cwt.at->land)) playermoved = false;

  if(sym == PSEUDOKEY_WHEELUP) {
    ld jx = (mousex - current_display->xcenter - .0) / current_display->radius / 10;
    ld jy = (mousey - current_display->ycenter - .0) / current_display->radius / 10;
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
      uni = sym = 0;
    }

  #if CAP_SHOT
  if(uni == 'A') { pushScreen(shot::menu); uni = sym = 0; }
  #endif

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

  if(DEFAULTNOR(sym)) {
    gmodekeys(sym, uni);
    if(uni == 'm' && canmove && (centerover == cwt ? mouseover : centerover.at))
      performMarkCommand(mouseover);
    }
  
  if(DEFAULTCONTROL) {
    if(sym == '.' || sym == 's') movepcto(-1, 1);
    if((sym == SDLK_DELETE || sym == SDLK_KP_PERIOD || sym == 'g') && uni != 'G' && uni != 'G'-64) 
      movepcto(MD_DROP, 1);
    if(sym == 't' && uni != 'T' && uni != 'T'-64 && canmove) {
      if(playermoved && items[itStrongWind]) { 
        cell *c = whirlwind::jumpDestination(cwt.at);
        if(c) centerover.at = c;
        }
      targetRangedOrb(centerover.at, roKeyboard);
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
  
  if(uni == 'o' && DEFAULTNOR(sym)) get_o_key().second();
#if CAP_INV
  if(uni == 'i' && DEFAULTNOR(sym) && inv::on) 
    pushScreen(inv::show);
#endif
  
  if(((sym == SDLK_HOME && DIM == 2) || sym == SDLK_F3 || sym == ' ') && DEFAULTNOR(sym)) 
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

bool mousepan, oldmousepan;
ld mouseaim_x, mouseaim_y;
ld mouseaim_sensitivity = 0.01;

void mainloopiter() {

  DEBB(DF_GRAPH, (debugfile,"main loop\n"));

  #if !CAP_SDLGFX && !CAP_GL 
  vid.wallmode = 0;
  vid.monmode = 0;
  #endif

  optimizeview();
  
  conformal::configure();

  ticks = SDL_GetTicks();
  callhooks(hooks_fixticks);
    
  timetowait = lastt + 1000 / cframelimit - ticks;

  cframelimit = vid.framelimit;
  if(outoffocus && cframelimit > 10) cframelimit = 10;  
  
  bool normal = cmode & sm::NORMAL;

  shmup::turn(ticks - lastt);
    
  if(!shmup::on && (multi::alwaysuse || multi::players > 1) && normal)
    timetowait = 0, multi::handleMulti(ticks - lastt);

  if(vid.sspeed >= 5 && gmatrix.count(cwt.at) && !elliptic) {
    cwtV = gmatrix[cwt.at] * ddspin(cwt.at, cwt.spin);
    if(cwt.mirrored) playerV = playerV * Mirror;
    }
  
  mousepan = normal && DIM == 3 && mouseaim_sensitivity;
  if(mousepan != oldmousepan) {
    oldmousepan = mousepan;
    if(mousepan) {    
      SDL_WM_GrabInput(SDL_GRAB_ON);
      SDL_ShowCursor(SDL_DISABLE);
      mouseaim_x = mouseaim_y = 0;
      }
    else {
      SDL_WM_GrabInput( SDL_GRAB_OFF );
      SDL_ShowCursor(SDL_ENABLE);
      SDL_WarpMouse(vid.xres/2, vid.yres/2);
      mouseaim_x = mouseaim_y = 0;      
      }
    }

#if ISWEB
  timetowait = 0;
#endif

  if(timetowait > 0)
    SDL_Delay(timetowait);
  else {
    ors::check_orientation();
    if(cmode & sm::CENTER) {
      if(playermoved && vid.sspeed > -4.99 && !outoffocus)
        centerpc((ticks - lastt) / 1000.0 * exp(vid.sspeed));
#if CAP_SDLJOY
      if(panjoyx || panjoyy) 
        checkpanjoy((ticks - lastt) / 1000.0);
#endif
      }
    tortoise::updateVals(ticks - lastt);
    frames++;
    if(!outoffocus) {
      drawscreen();
      }
    lastt = ticks;
    }      

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
  
  if(DIM == 3 && !shmup::on && !rug::rugged) {
    View = cspin(0, 2, -mouseaim_x) * cspin(1, 2, -mouseaim_y) * View;
    mouseaim_x = mouseaim_y = 0;
    }
  
  if(smooth_scrolling && !shmup::on && !rug::rugged) {
    static int lastticks;
    ld t = (ticks - lastticks) * shiftmul / 1000.;
    lastticks = ticks;
    Uint8 *keystate = SDL_GetKeyState(NULL);
    if(keystate[SDLK_END] && DIM == 3 && DEFAULTNOR(SDLK_END))
      View = cpush(2, -t) * View, didsomething = true, playermoved = false;
    if(keystate[SDLK_HOME] && DIM == 3 && DEFAULTNOR(SDLK_HOME))
      View = cpush(2, t) * View, didsomething = true, playermoved = false;
    if(keystate[SDLK_RIGHT] && DEFAULTNOR(SDLK_RIGHT))
      View = (DIM == 2 ? xpush(-t) : cspin(0, 2, -t)) * View, didsomething = true, playermoved = playermoved && DIM == 3;
    if(keystate[SDLK_LEFT] && DEFAULTNOR(SDLK_LEFT))
      View = (DIM == 2 ? xpush(t) : cspin(0, 2, t)) * View, didsomething = true, playermoved = playermoved && DIM == 3;
    if(keystate[SDLK_UP] && DEFAULTNOR(SDLK_UP))
      View = (DIM == 2 ? ypush(t) : cspin(1, 2, t)) * View, didsomething = true, playermoved = playermoved && DIM == 3;
    if(keystate[SDLK_DOWN] && DEFAULTNOR(SDLK_DOWN))
      View = (DIM == 2 ? ypush(-t) : cspin(1, 2, -t)) * View, didsomething = true, playermoved = playermoved && DIM == 3;
    if(keystate[SDLK_PAGEUP] && DEFAULTNOR(SDLK_PAGEUP)) {
      if(conformal::on)
        conformal::rotation+=t;
      else
        View = spin(t) * View, didsomething = true;
      }
    if(keystate[SDLK_PAGEDOWN] && DEFAULTNOR(SDLK_PAGEDOWN)) {
      if(conformal::on)
        conformal::rotation-=t;
      else
        View = spin(-t) * View, didsomething = true;
      }
    }

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
    
    if(joyhandler && joyhandler(ev)) ;

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
      numlock_on = ev.key.keysym.mod & KMOD_NUM;
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
      
      if(mousepan) {
        mousex = vid.xres/2;
        mousey = vid.yres/2;      
        mouseaim_x += ev.motion.xrel * mouseaim_sensitivity;
        mouseaim_y += ev.motion.yrel * mouseaim_sensitivity;
        }
      
      need_mouseh = true;

      if(holdmouse && getcstat == '-') sym = uni = getcstat, fix_mouseh();

      if((rightclick || (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_MMASK)) && !mouseout2()) {
        fix_mouseh();
        if(anyctrl) {
          vid.xposition += (mousex - lmousex) * 1. / current_display->scrsize,
          vid.yposition += (mousey - lmousey) * 1. / current_display->scrsize;
          }
        else if(mouseh[DIM] < 50 && mouseoh[DIM] < 50) {
          panning(mouseoh, mouseh);
          }
        }

#ifdef SIMULATE_JOYSTICK
      // pretend that both joysticks are present
      stick = panstick = (SDL_Joystick*) (&vid);
      panjoyx = 20 * (mousex - current_display->xcenter);
      panjoyy = 20 * (mousey - current_display->ycenter);
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
  if(vid.stereo_mode == sLR) rad = 99999;
  int vrx = min(rad, vid.xres/2 - 40);
  int vry = min(rad, min(current_display->ycenter, vid.yres - current_display->ycenter) - 20);
  int x = current_display->xcenter + px * vrx;
  int y = current_display->ycenter + py * (vry - siz/2);
  int vrr = int(hypot(vrx, vry) * sqrt(2.));
  if(gtouched && !mouseover
    && abs(mousex - current_display->xcenter) < vrr
    && abs(mousey - current_display->ycenter) < vrr
    && hypot(mousex-current_display->xcenter, mousey-current_display->ycenter) > vrr
    && px == (mousex > current_display->xcenter ? 1 : -1)
    && py == (mousey > current_display->ycenter ? 1 : -1)
    ) col = 0xFF0000;
  if(displayfr(x, y, 0, siz, s, col, 8+8*px))
    buttonclicked = true;
  }
#endif

bool interpret_as_direction(int sym, int uni) {
  #ifdef FAKE_SDL
  return false;
  #else
  return (sym >= SDLK_KP0 && sym <= SDLK_KP9 && !numlock_on);
  #endif
  }

int get_direction_key(int sym, int uni) {
  if(interpret_as_direction(sym, uni)) {
    #ifndef FAKE_SDL
    if(sym == SDLK_KP1) return SDLK_END;
    if(sym == SDLK_KP2) return SDLK_DOWN;
    if(sym == SDLK_KP3) return SDLK_PAGEDOWN;
    if(sym == SDLK_KP4) return SDLK_LEFT;
    if(sym == SDLK_KP6) return SDLK_RIGHT;
    if(sym == SDLK_KP7) return SDLK_HOME;
    if(sym == SDLK_KP8) return SDLK_UP;
    if(sym == SDLK_KP8) return SDLK_PAGEUP;
    #endif
    return 0;
    }
  return sym;
  }

void gmodekeys(int sym, int uni) {
  #if CAP_RUG
  if(rug::rugged) rug::handlekeys(sym, uni);
  #endif
  if(DIM == 2) {
    if(NUMBERKEY == '1' && !rug::rugged) { vid.alpha = 999; vid.scale = 998; vid.xposition = vid.yposition = 0; }
    if(NUMBERKEY == '2' && !rug::rugged) { vid.alpha = 1; vid.scale = 0.4; vid.xposition = vid.yposition = 0; }
    if(NUMBERKEY == '3' && !rug::rugged) { vid.alpha = 1; vid.scale = 1; vid.xposition = vid.yposition = 0; }
    if(NUMBERKEY == '4' && !rug::rugged) { vid.alpha = 0; vid.scale = 1; vid.xposition = vid.yposition = 0; }
    if(NUMBERKEY == '5') { vid.wallmode += 60 + (shiftmul > 0 ? 1 : -1); vid.wallmode %= 6; }
    if(uni == '%') { 
      if(vid.wallmode == 0) vid.wallmode = 6;
      vid.wallmode--;
      }
    }
  if(NUMBERKEY == '6') vid.grid = !vid.grid;
  if(NUMBERKEY == '7') { vid.darkhepta = !vid.darkhepta; }
  }

bool haveMobileCompass() {
#if ISMOBILE
  if(longclick) return false;
#else
  if(forcetarget) return false;
#endif
  if(DIM == 3) return false;
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
      hyperpoint param = hpxy(dx * 1. / rad, dy * 1. / rad);

      movedir md = vectodir(param);
    
      if(!canmove) movepcto(md), remission(); else movepcto(md);
      }
    getcstat = 0;
    return true;
    }

  return false;
  }

// orientation sensitivity
namespace ors {

int mode;
double sensitivity = 1;

int when_enabled;
transmatrix last_orientation;
transmatrix relative_matrix = Id;

string choices[3] = {"OFF", "relative", "absolute"};

#if CAP_ORIENTATION
transmatrix getOrientation() {
  return MirrorX * MirrorZ * hr::getOrientation() * MirrorX * MirrorZ;
  }
#endif

void reset() {
#if CAP_ORIENTATION
  if(mode) last_orientation = getOrientation();
  relative_matrix = Id;
#endif
  }

void delayed_reset() {
#if CAP_ORIENTATION
  relative_matrix = Id; when_enabled = ticks;
#endif
  }

void show() {
#if CAP_ORIENTATION
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);

  dialog::init(XLAT("scrolling by device rotation"));
  
  dialog::addHelp(XLAT(
    "This lets you scroll the map by rotating your device. It can be e.g. used to "
    "play the spherical mode of HyperRogue in mobile VR goggles -- the \"spherical VR\" "
    "button configures this; this VR mode can be disabled by touching the screen for 1 second."));
  
  dialog::addSelItem(XLAT("mode"), choices[mode], 'm');
  dialog::add_action([] () { int m = (mode + 1) % 3; mode = 0; fullcenter(); mode = m; delayed_reset(); });
  dialog::addSelItem(XLAT("sensitivity"), fts(sensitivity), 's');
  dialog::add_action([] () { 
    dialog::editNumber(sensitivity, -10, 10, 1, 1, XLAT("sensitivity"), 
        XLAT("1 means that rotating the device by 1 radian corresponds to scrolling by 1 unit. In spherical geometry, 1 unit = 1 radian."));
    });
  
  dialog::addBreak(100);

  dialog::addItem(XLAT("stereo vision config"), 'e');
  dialog::add_action([] () { pushScreen(showStereo); });

  dialog::addItem(XLAT("experiment with geometry"), 'g');
  dialog::add_action([] () { runGeometryExperiments(); });

  dialog::addSelItem(XLAT("projection"), fts(vid.alpha), 'p');
  dialog::add_action([] () { projectionDialog(); });

  dialog::addSelItem(XLAT("scale factor"), fts(vid.scale), 'z');
  dialog::add_action([] () { editScale(); });

  dialog::addItem(XLAT("spherical VR"), 'v');
  dialog::add_action([] () { 
    if(!sphere) set_geometry(gSphere);
    mode = 0; fullcenter();
    mode = 2; sensitivity = 1;
    vid.stereo_mode = sLR; vid.ipd = 0.2;
    vid.alpha = 0; vid.scale = 1;
    });

  dialog::addBreak(100);

  dialog::addBack();

  dialog::display();
#endif
  }

void relative_apply() {
  if(ors::mode == 1) View = relative_matrix * View;
  }

void relative_unapply() {
  if(ors::mode == 1) View = inverse(relative_matrix) * View;
  }

transmatrix change_geometry(const transmatrix& T) {
  if(sphere && sensitivity == 1) return T;
  ld alpha, beta, push;
  
  {
  dynamicval<eGeometry> g(geometry, gSphere);
  hyperpoint h = T * C0;
  push = hdist0(h);
  alpha = atan2(h[1], h[0]);
  if(push == 0) alpha = 0;
  hyperpoint spinpoint = gpushxto0(h) * T * xpush0(1);
  beta = atan2(spinpoint[1], spinpoint[0]);
  }
  
  // gpushxto0(h) * T * xpush(1) * C0 == spin(beta) * xpush(1) * C0
  // gpushxto0(h) * T == spin(beta)
  // T = rgpushxto0(h) * spin(beta)

  
  transmatrix U = spin(-alpha) * xpush(push * sensitivity) * spin(-beta+alpha);

  return U;
  }

void unrotate(transmatrix& T) {
  if(mode == 1) T = inverse(relative_matrix) * T;
  }

void rerotate(transmatrix& T) {
  if(mode == 1) T = (relative_matrix) * T;
  }

void check_orientation() {
#if CAP_ORIENTATION
  if(!mode) return;
  if(ticks < when_enabled + 500) {
    last_orientation = getOrientation();
    return;
    }
  transmatrix next_orientation = MirrorX * getOrientation();
  transmatrix T = inverse(next_orientation) * last_orientation;
  if(mode == 1) unrotate(View), unrotate(cwtV);
  relative_matrix = change_geometry(T);
  if(mode == 1) rerotate(View), rerotate(cwtV);
  if(mode == 2) View = relative_matrix * View, last_orientation = next_orientation;
#endif
  }

}

}
