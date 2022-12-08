// Hyperbolic Rogue -- control
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file control.cpp 
 *  \brief Routines related to controlling the game
 */

#include "hyper.h"
namespace hr {

EX int frames;
EX bool outoffocus = false;

EX int mousex, mousey;
EX shiftpoint mouseh, mouseoh;

EX bool pandora_leftclick, pandora_rightclick;

EX bool lshiftclick, rshiftclick, lctrlclick, rctrlclick, anyshiftclick, anyctrlclick, wheelclick;

EX bool targetclick, hiliteclick, forcetarget, numlock_on;
EX bool gtouched;

EX bool holdmouse;

EX int getcstat, lgetcstat;
EX ld getcshift;
EX bool inslider;
EX bool invslider;
EX int slider_x;

EX function <void(int sym, int uni)> keyhandler = [] (int sym, int uni) {};
EX function <bool(SDL_Event &ev)> joyhandler = [] (SDL_Event &ev) {return false;};

#if HDR
// what part of the compass does 'skip turn'
static const auto SKIPFAC = .4;
#endif

// is the player using mouse? (used for auto-cross)
EX bool mousing = true;

/** /brief 0 for the system pointer, or VR controller ID */

EX int which_pointer = 0;

// is the mouse button pressed?
EX bool mousepressed = false;
EX bool mousemoved = false;
EX bool actonrelease = false;

EX bool mousepan, oldmousepan;
#if CAP_MOUSEGRAB
EX ld mouseaim_x, mouseaim_y;
#endif
EX ld mouseaim_sensitivity = 0.01;

EX int timetowait;

#if CAP_SDLJOY
EX int joyx, joyy, panjoyx, panjoyy; 
EX movedir joydir;
#endif

EX movedir mousedest;
EX ld shiftmul = 1;

EX cell *mouseover, *mouseover2, *lmouseover, *lmouseover_distant;
EX ld modist, modist2;

EX int lastt;

EX bool mouseout() {
  if((getcstat != '-' && getcstat) || (lgetcstat && lgetcstat != '-')) return true;
  return outofmap(mouseh.h);
  }

EX bool mouseout2() {
  if((getcstat && getcstat != '-') || (lgetcstat && lgetcstat != '-')) return true;
  return outofmap(mouseh.h) || outofmap(mouseoh.h);
  }

EX movedir vectodir(hyperpoint P) {

  transmatrix U = unshift(ggmatrix(cwt.at));
  if(embedded_plane && geom3::same_in_same())  U = current_display->radar_transform * U;

  P = direct_exp(lp_iapply(P));

  hyperpoint H = sphereflip * tC0(U);
  transmatrix Centered = sphereflip * rgpushxto0(H);

  ld binv = 99;
  
  vector<ld> dirdist(cwt.at->type);

  auto TC0 = tile_center();

  for(int i=0; i<cwt.at->type; i++) {
    transmatrix T = currentmap->adj(cwt.at, (cwt + i).spin);
    ld d1 = geo_dist(U * T * TC0, Centered * P);
    ld d2 = geo_dist(U * T * TC0, Centered * TC0);
    dirdist[i] = d1 - d2;
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

EX void remission() {
  if(!canmove && (cmode & sm::NORMAL) && !game_keys_scroll) showMissionScreen();
 }

EX hyperpoint move_destination_vec(int d) {
  if(WDIM == 2) return spin(-d * 45._deg) * smalltangent();
  else if(d&1) return cspin(0, 1, d > 4 ? 45._deg : -45._deg) * smalltangent();
  else return cspin(0, 2, d * 45._deg) * smalltangent();
  }

EX void movepckeydir(int d) {
  DEBB(DF_GRAPH, ("movepckeydir\n"));
  // EUCLIDEAN
  
  if(protect_memory()) return;
  
  movedir md = vectodir(move_destination_vec(d));
    
  if(!canmove) movepcto(md), remission(); else movepcto(md);
  }

EX void movevrdir(hyperpoint vec) {
  movedir md = vectodir(vec);    
  if(!canmove) movepcto(md), remission(); else movepcto(md);
  }

EX void calcMousedest() {
  if(mouseout()) return;
  if(vid.revcontrol == true) { mouseh[0] = -mouseh[0]; mouseh[1] = -mouseh[1]; }
  ld mousedist = hdist(mouseh, tC0(ggmatrix(cwt.at)));
  mousedest.d = -1;
  
  cellwalker bcwt = cwt;
  
  vector<ld> dists(cwt.at->type);
  
  shiftmatrix U = ggmatrix(cwt.at);
  
  for(int i=0; i<cwt.at->type; i++) {
    transmatrix T = currentmap->adj(cwt.at, i);
    dists[i] = hdist(mouseh, U * T * C0);
    }
  
  for(int i=0; i<cwt.at->type; i++) if(dists[i] < mousedist) {
    mousedist = dists[i];
    mousedest.d = cwt.at->c.fix(i - cwt.spin);
    
    mousedest.subdir =
       dists[cwt.at->c.fix(i+1)] < dists[cwt.at->c.fix(i-1)] ? 1 : -1;

    if(cwt.mirrored) 
      mousedest.d = cwt.at->c.fix(-mousedest.d), 
      mousedest.subdir = -mousedest.subdir;
    }
  
  if(vid.revcontrol == true) { mouseh[0] = -mouseh[0]; mouseh[1] = -mouseh[1]; }
  cwt = bcwt;
  }

EX void mousemovement() {

  #if CAP_VR
  if(WDIM == 3 && vrhr::active() && which_pointer) {
    movevrdir(vrhr::vr_direction);
    return;
    }
  #endif

  if(GDIM == 3 && !which_pointer) {
    if(WDIM == 2) {
      if(View[2][2] < -0.75) 
        movepcto(MD_DROP, 1);
      else if(View[2][2] > 0.75) 
        movepcto(-1, 1);
      else
        movepckeydir(6);
      return;
      }
    movepckeydir(6);
    return;
    }

  if(protect_memory()) return;

  calcMousedest();
    if(!canmove) movepcto(mousedest), remission(); else movepcto(mousedest);
  lmouseover = NULL;
  }

#if CAP_SDLJOY
EX SDL_Joystick* sticks[8];
EX int numsticks;

EX void initJoysticks() {

  if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) == -1)
  {
    printf("Failed to initialize joysticks.\n");
    numsticks = 0;
    return;
  }

  DEBB(DF_INIT, ("init joysticks"));
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

EX void closeJoysticks() {
  DEBB(DF_INIT, ("close joysticks"));
  for(int i=0; i<numsticks; i++) {
    SDL_JoystickClose(sticks[i]), sticks[i] = NULL;
    }
  numsticks = 0;
  }

int joytime;
EX bool joy_ignore_next = false;

EX void checkjoy() {
  DEBB(DF_GRAPH, ("check joy"));
  if(!DEFAULTCONTROL) return;
  ld joyvalue1 = sqr(vid.joyvalue);
  ld joyvalue2 = sqr(vid.joyvalue2);
  
  ld jx = joyx;
  ld jy = joyy;
  ld sq = jx*jx+jy*jy;

  static int laststate = 0;
  int curstate = sq < joyvalue1 ? 0 : sq < joyvalue2 ? 1 : 2;
  if(curstate != laststate) flashMessages(), laststate = curstate;
  
  static int lt = ticks;
  int delta = ticks - lt;
  lt = ticks;
  
  if(autojoy) {
    if(sq < joyvalue1) { if(joydir.d >= 0 && !joy_ignore_next) movepcto(joydir); joydir.d = -1; joytime = 0; joy_ignore_next = false; return; }
    if(sq < joyvalue2 && joydir.d == -1) return;
    }
  else {
    if(sq < joyvalue1) { joydir.d = -1; return; }
    }
  
  auto new_joydir = vectodir(tangent_length(point2(jx, jy), 0.01)); 
  if(new_joydir.d == joydir.d && new_joydir.subdir == joydir.subdir) {
    joytime += delta;
    if(joytime > vid.joysmooth) joytime = vid.joysmooth;
    }
  else {
    joytime -= delta;
    if(joytime < 0) joytime = -joytime, joydir = new_joydir;
    }
  }

EX void checkpanjoy(double t) {
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

EX bool quitmainloop = false;

EX bool doexiton(int sym, int uni) {
  if(sym == SDLK_ESCAPE) return true;
  if(sym == SDLK_F10) return true;
  if(sym == PSEUDOKEY_EXIT) return true;
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

EX bool didsomething;

typedef SDL_Event eventtype;

EX bool smooth_scrolling = false;

transmatrix zforward_push(ld z) {
  if(!sl2) return zpush(z);
  transmatrix T = Id;
  T[2][3] = z;
  return T;
  }

EX void zoom_or_fov(ld t) {
  if(in_perspective_v()) {
    vid.fov *= 180 / max_fov_angle();
    auto tanfov = tan(vid.fov * degree / 2);
    tanfov *= t;
    vid.fov = atan(tanfov) * 2 / degree;
    vid.fov *= max_fov_angle() / 180;
    }
  else
    vpconf.scale *= t;
  }

EX ld camera_speed = 1;
EX ld camera_rot_speed = 1;

EX void full_forward_camera(ld t) {
  if(anyshiftclick) 
    zoom_or_fov(exp(-t/10.));
  else if(GDIM == 3) {
    shift_view(ctangent(2, t * camera_speed));
    didsomething = true;
    playermoved = false;
    }
  }

EX void full_cstrafe_camera(int dir, ld t) {
  if(GDIM == 3) {
    shift_view(ctangent(dir, t * camera_speed));
    didsomething = true;
    playermoved = false;
    }
  }

EX void full_strafe_camera(ld t) { full_cstrafe_camera(0, t); }

EX void full_ystrafe_camera(ld t) {
  if(walking::on) { walking::eye_level += t; if(walking::eye_level < 1e-3) walking::eye_level = 1e-3; }
  else full_cstrafe_camera(1, t);
  }


EX ld third_person_rotation = 0;

EX void full_rotate_camera(int dir, ld val) {
  if(rug::rug_control() && lshiftclick) {
    val *= camera_rot_speed;
    hyperpoint h;
    if(nonisotropic) {
      transmatrix T2 = eupush( tC0(view_inverse(View)) );
      transmatrix nlp = View * T2;  
      auto rV = ortho_inverse(nlp) * View;
      h = nlp * inverse_exp(shiftless(tC0(rV)));
      }
    else h = inverse_exp(shiftless(tC0(View)));
    shift_view(-h);
    rotate_view(cspin(dir, 2, val));
    shift_view(h);    
    }
  else if(history::on)
    history::lvspeed += (dir?1:-1) * val / 2;
  else if(GDIM == 3 && rshiftclick)
    shift_view(ctangent(dir, -val * camera_speed)), didsomething = true, playermoved = false; /* -val because shift reverses */
  #if CAP_CRYSTAL && CAP_RUG
  else if(rug::rug_control() && rug::in_crystal())
    crystal::apply_rotation(cspin(dir, 2, val * camera_rot_speed));
  #endif
  else if(GDIM == 3) {
    val *= camera_rot_speed;
    if(third_person_rotation) shift_view(ctangent(2, -third_person_rotation)), didsomething = true, playermoved = false;
    ld max_angle = 90._deg - 1e-4;
    if(walking::on && dir == 1) {
      max_angle /= degree;
      walking::eye_angle += val * walking::eye_angle_scale / degree;
      if(walking::eye_angle > max_angle) walking::eye_angle = max_angle;
      if(walking::eye_angle < -max_angle) walking::eye_angle = -max_angle;
      }
    else if(keep_vertical()) {
      hyperpoint vv = vertical_vector();
      ld alpha = -atan2(vv[2], vv[1]);
      rotate_view(cspin(2, 1, alpha));
      if(dir == 1 && alpha + val > max_angle)
        val = max_angle - alpha;
      if(dir == 1 && alpha + val < -max_angle)
        val = -max_angle - alpha;
      rotate_view(cspin(dir, 2, val));
      rotate_view(cspin(1, 2, alpha));
      }
    else
      rotate_view(cspin(dir, 2, val));
    if(third_person_rotation) shift_view(ctangent(2, third_person_rotation));
    if(!rug::rug_control()) didsomething = true;
    }
  else
    shift_view(ctangent(dir, val * camera_speed)), playermoved = false, didsomething = true;      
  }

EX void full_rotate_view(ld h, ld v) {
  if(history::on && !rug::rug_control())
    models::rotation += h * camera_rot_speed;
  else {
    rotate_view(spin(v * camera_rot_speed));
    didsomething = true;
    if(isGravityLand(cwt.at->land) && !rug::rug_control())
      playermoved = false;
    }
  }

EX void handlePanning(int sym, int uni) {
  if(mousepan && dual::split([=] { handlePanning(sym, uni); })) return;
  if(GDIM == 3) {
    if(sym == PSEUDOKEY_WHEELUP) shift_view(ztangent(-0.05*shiftmul) * camera_speed), didsomething = true, playermoved = false;
    if(sym == PSEUDOKEY_WHEELDOWN) shift_view(ztangent(0.05*shiftmul) * camera_speed), didsomething = true, playermoved = false;
    }

  #if CAP_RUG
  rug::using_rugview urv;
  #endif
    
#if !ISPANDORA
  if(!smooth_scrolling) {
    if(sym == SDLK_END) full_forward_camera(-0.2*shiftmul);
    if(sym == SDLK_HOME) full_forward_camera(0.2*shiftmul);
    if(sym == SDLK_RIGHT) full_rotate_camera(0, -0.2*shiftmul);
    if(sym == SDLK_LEFT) full_rotate_camera(0, 0.2*shiftmul);
    if(sym == SDLK_UP) full_rotate_camera(1, 0.2*shiftmul);
    if(sym == SDLK_DOWN) full_rotate_camera(1, -0.2*shiftmul);
    }
#endif
  if(!smooth_scrolling) {
    if(sym == SDLK_PAGEUP) full_rotate_view(1, cgi.S_step*shiftmul);
    if(sym == SDLK_PAGEDOWN) full_rotate_view(-1, -cgi.S_step*shiftmul);
    if(sym == SDLK_PAGEUP || sym == SDLK_PAGEDOWN) 
      if(isGravityLand(cwt.at->land) && !rug::rug_control()) playermoved = false;
    }

  if(sym == PSEUDOKEY_WHEELUP && GDIM == 2) {
    ld jx = (mousex - current_display->xcenter - .0) / current_display->radius / 10;
    ld jy = (mousey - current_display->ycenter - .0) / current_display->radius / 10;
    playermoved = false;
    rotate_view(gpushxto0(hpxy(jx * camera_speed, jy * camera_speed)));
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
  println(hlog, spaced(bscale7, brot7, bscale6, brot6));
  return true;
  }
#endif

EX purehookset hooks_fixticks;

EX array<int, 8> keys_vi = {{'l', 'n', 'j', 'b', 'h', 'y', 'k', 'u'}};
EX array<int, 8> keys_wasd = {{'d', 'c', 'x', 'z', 'a', 'q', 'w', 'e'}};
EX array<int, 8> keys_numpad = {{SDLK_KP6, SDLK_KP3, SDLK_KP2, SDLK_KP1, SDLK_KP4, SDLK_KP7, SDLK_KP8, SDLK_KP9}};
  
EX void handleKeyNormal(int sym, int uni) {

  if(cheater && sym < 256 && sym > 0) {
    if(applyCheat(uni))
      uni = sym = 0;
    }

  #if CAP_SHOT
  if(uni == 'A') { pushScreen(shot::menu); uni = sym = 0; }
  #endif

  if(DEFAULTNOR(sym)) handlePanning(sym, uni);
  
#ifdef SCALETUNER
  if(handleTune(sym, uni)) return;
#endif

  if(!(uni >= 'A' && uni <= 'Z') && DEFAULTCONTROL && !game_keys_scroll) {
    for(int i=0; i<8; i++)
      if(among(sym, keys_vi[i], keys_wasd[i], keys_numpad[i]))
        movepckeydir(i);
    }

#if ISPANDORA
  if(DEFAULTCONTROL) {
    if(sym == SDLK_RIGHT) movepckeydir(0);
    if(sym == SDLK_LEFT) movepckeydir(4);
    if(sym == SDLK_DOWN) movepckeydir(2 + (pandora_leftclick?1:0) - (pandora_rightclick?1:0));
    if(sym == SDLK_UP) movepckeydir(6 - (pandora_leftclick?1:0) + (pandora_rightclick?1:0));
    }
#endif

  #if CAP_COMPLEX2
  if(DEFAULTNOR(sym)) {
    gmodekeys(sym, uni);
    if(uni == 'm' && canmove && (centerover == cwt.at ? mouseover : centerover))
      mine::performMarkCommand(mouseover);
    }
  #endif
  
  if(DEFAULTCONTROL && !game_keys_scroll) {
    if(sym == '.' || sym == 's') movepcto(-1, 1);
    if((sym == SDLK_DELETE || sym == SDLK_KP_PERIOD || sym == 'g') && uni != 'G' && uni != 'G'-64) 
      movepcto(MD_DROP, 1);
    if(sym == 't' && uni != 'T' && uni != 'T'-64 && canmove) {
      cell *target = GDIM == 3 ? mouseover : centerover;
      if(playermoved && items[itStrongWind]) { 
        cell *c = whirlwind::jumpDestination(cwt.at);
        if(c) target = c;
        }
      targetRangedOrb(target, roKeyboard);
      sym = 0; uni = 0;
      }
    }

  if(sym == SDLK_KP5 && DEFAULTCONTROL && !game_keys_scroll) movepcto(-1, 1);

  if(sym == SDLK_F5) {
    #if CAP_DAILY
    if(daily::on) daily::handleQuit(1);
    else
    #endif
    if(needConfirmation()) 
      pushScreen(showGameMenu);
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
    if(needConfirmation()) pushScreen(showGameMenu);
    else quitmainloop = true;
    }
  
  if(uni == 'o' && DEFAULTNOR(sym)) get_o_key().second();
#if CAP_INV
  if(uni == 'i' && DEFAULTNOR(sym) && inv::on && !game_keys_scroll)
    pushScreen(inv::show);
#endif
  
  if((sym == SDLK_F3 || sym == ' ') && DEFAULTNOR(sym)) {
    if(rug::rug_control())
      rug::reset_view();
    else
      fullcenter();
    }
  
  if(sym == 'v' && DEFAULTNOR(sym)) 
    showMissionScreen();

  if(sym == PSEUDOKEY_MENU) 
    showMissionScreen();
  
  if(sym == PSEUDOKEY_NOHINT)
    no_find_player = true;

  if(sym == '-' || sym == PSEUDOKEY_WHEELDOWN) {
    actonrelease = false;
    
    multi::cpid = 0;
    if(mouseover && 
      targetclick && (shmup::on ? numplayers() == 1 && !shmup::pc[0]->dead : true) && targetRangedOrb(mouseover, forcetarget ? roMouseForce : roMouse)) {
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

  if(sym == PSEUDOKEY_MEMORY) pushScreen(show_memory_menu);
  }

EX bool need_mouseh = false;

EX void fix_mouseh() {
  if(0) ;
#if CAP_RUG
  else if(rug::rugged) {
    if(need_mouseh || (vrhr::active() && which_pointer))
      mouseh = rug::gethyper(mousex, mousey);
    }
#endif
#if CAP_VR
  else if(vrhr::active() && which_pointer && !vrhr::targeting_menu)
    vrhr::compute_point(which_pointer, mouseh, mouseover, vrhr::pointer_distance);
#endif
  else {
    if(dual::state) {
      if(cmode & (sm::NORMAL | sm::DRAW | sm::MAP)) {
        dual::main_side = (mousex >= current_display->xcenter);
        dual::switch_to(dual::main_side);
        }
      dual::in_subscreen([=] () { calcparam(); mouseh = gethyper(mousex, mousey); });
      }
    else mouseh = gethyper(mousex, mousey);
    }
  need_mouseh = false;
  }

EX void handlekey(int sym, int uni) {

  if(callhandlers(false, hooks_handleKey, sym, uni)) return;

  keyhandler(sym, uni);
  }

EX void resize_screen_to(int x, int y);

#if !CAP_SDL
EX void mainloopiter() { printf("(compiled without SDL -- no action)\n"); quitmainloop = true; }
#endif

#if CAP_SDL

// Warning: a very long function! todo: refactor

int cframelimit = 1000;

EX void resize_screen_to(int x, int y) {
  dual::split_or_do([&] {
    vid.killreduction = 0;
    if(vid.want_fullscreen) return;
    if(vid.relative_window_size) {
      vid.window_rel_x = x * 1. / vid.xscr;
      vid.window_rel_y = y * 1. / vid.yscr;
      }
    else {
      vid.window_x = x;
      vid.window_y = y;
      }
    });
  apply_screen_settings();
  }

int lastframe;

EX int sc_ticks, sc_ticks2;

EX bool mouseaiming(bool shmupon) {
  return
    (GDIM == 3 && !shmupon) || (rug::rugged && (lctrlclick ^ rug::mouse_control_rug));
  }

/* visualization only -- the HyperRogue movement keys should move the camera */
EX bool game_keys_scroll;

EX void mainloopiter() {
  GLWRAP;
  DEBB(DF_GRAPH, ("main loop\n"));

  #if !CAP_SDLGFX && !CAP_GL 
  vid.wallmode = 0;
  vid.monmode = 0;
  #endif

  #if CAP_VR
  vrhr::vr_shift();
  #endif  

  optimizeview();
  
  models::configure();

  lastt = ticks;
  ticks = SDL_GetTicks();
  callhooks(hooks_fixticks);
    
  timetowait = lastframe + 1000 / cframelimit - ticks;

  cframelimit = vid.framelimit;
  if(outoffocus && cframelimit > 10) cframelimit = 10;  
  
  bool normal = cmode & sm::NORMAL;
  
  shmup::turn(ticks - lastt);
    
  if(!shmup::on && (multi::alwaysuse || multi::players > 1) && normal)
    timetowait = 0, multi::handleMulti(ticks - lastt);

  if(vid.sspeed >= 5 && gmatrix.count(cwt.at) && !elliptic && !shmup::on) {
    cwtV = gmatrix[cwt.at] * ddspin(cwt.at, cwt.spin);
    if(cwt.mirrored) playerV = playerV * Mirror;
    }
  
  mousepan = cmode & sm::NORMAL;
  if((cmode & sm::PANNING) && !hiliteclick) mousepan = true;
  if(cmode & sm::SHOWCURSOR) mousepan = false;
  mousepan = mousepan && mouseaiming(false) && mouseaim_sensitivity;
  if(mousepan != oldmousepan) {
    oldmousepan = mousepan;
    #if CAP_MOUSEGRAB
    if(mousepan) {    
      #if CAP_SDL2
      SDL_SetRelativeMouseMode(SDL_TRUE);
      #else
      SDL_WM_GrabInput(SDL_GRAB_ON);
      SDL_ShowCursor(SDL_DISABLE);
      #endif
      mouseaim_x = mouseaim_y = 0;
      }
    else {
      #if CAP_SDL2
      SDL_SetRelativeMouseMode(SDL_FALSE);
      #else
      SDL_WM_GrabInput( SDL_GRAB_OFF );
      SDL_ShowCursor(SDL_ENABLE);
      SDL_WarpMouse(vid.xres/2, vid.yres/2);
      #endif
      mouseaim_x = mouseaim_y = 0;      
      }
    #endif
    }

#if ISWEB
  timetowait = 0;
#endif

  if(timetowait > 0)
    SDL_Delay(timetowait);
  else {
    ors::check_orientation();
    if(cmode & sm::CENTER) {
      ld aspd = (ticks - lastt) / 1000.0 * exp(vid.sspeed);
      if(playermoved && vid.sspeed > -4.99 && !outoffocus)
        centerpc(aspd);
      else if(GDIM == 3)
        spinEdge(aspd);
#if CAP_SDLJOY
      if(panjoyx || panjoyy) 
        checkpanjoy((ticks - lastt) / 1000.0);
#endif
      }
    tortoise::updateVals(ticks - lastt);
    frames++;
    if(!outoffocus) {
      drawscreen();
      need_refresh = false;
      }
    lastframe = ticks;
    }      

  wheelclick = false;

  getcshift = 1;
  
  #if CAP_SDL2
  
  const Uint8 *keystate = SDL_GetKeyboardState(NULL);

  pandora_rightclick = keystate[SDL_SCANCODE_RCTRL];
  pandora_leftclick = keystate[SDL_SCANCODE_RSHIFT];

  lshiftclick = keystate[SDL_SCANCODE_LSHIFT];
  rshiftclick = keystate[SDL_SCANCODE_RSHIFT];

  lctrlclick = keystate[SDL_SCANCODE_LCTRL];
  rctrlclick = keystate[SDL_SCANCODE_RCTRL];

  hiliteclick = keystate[SDL_SCANCODE_LALT] | keystate[SDL_SCANCODE_RALT];
  if(keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT]) getcshift = -1;
  if(keystate[SDL_SCANCODE_LCTRL] || keystate[SDL_SCANCODE_RCTRL]) getcshift /= 10;
  if(keystate[SDL_SCANCODE_LALT] || keystate[SDL_SCANCODE_RALT]) getcshift *= 10;

  #else

  Uint8 *keystate = SDL_GetKeyState(NULL);

  pandora_rightclick = keystate[SDLK_RCTRL];
  pandora_leftclick = keystate[SDLK_RSHIFT];

  lshiftclick = keystate[SDLK_LSHIFT];
  rshiftclick = keystate[SDLK_RSHIFT];

  lctrlclick = keystate[SDLK_LCTRL];
  rctrlclick = keystate[SDLK_RCTRL];

  hiliteclick = keystate[SDLK_LALT] | keystate[SDLK_RALT];
  if(keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]) getcshift = -1;
  if(keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL]) getcshift /= 10;
  if(keystate[SDLK_LALT] || keystate[SDLK_RALT]) getcshift *= 10;
  
  #endif

  anyshiftclick = lshiftclick | rshiftclick;
  anyctrlclick = lctrlclick | rctrlclick;
  
  forcetarget = anyshiftclick;
  
  didsomething = false;
  
  if(vid.shifttarget&1) {
    #if ISPANDORA
    targetclick = pandora_leftclick | pandora_rightclick;
    pandora_leftclick = pandora_rightclick = 0;
    #else
    targetclick = keystate[SDLK_RSHIFT] | keystate[SDLK_LSHIFT];
    #endif
    }
  else {
    targetclick = true;
    }
  
#if CAP_SDLAUDIO
  if(audio) handlemusic();
#endif
  apply_memory_reserve();
  SDL_Event ev;
  DEBB(DF_GRAPH, ("polling for events\n"));
  
  #if CAP_VR
  if(vrhr::active() && !shmup::on) {
    static int lastticks = ticks;
    ld t = (ticks - lastticks) * shiftmul / 400;
    lastticks = ticks;

    rug::using_rugview urv;
    dynamicval<bool> ds(didsomething, didsomething);
    using namespace vrhr;
    if(vrhr::hsm == vrhr::eHeadset::model_viewing) {
      E4;
      transmatrix T = hmd_at * inverse(hmd_ref_at);

      T = 
        cspin(0, 2, -vraim_x * camera_speed * t) * 
        cspin(1, 2, vraim_y * camera_speed * t) *
        cpush(0, -vrgo_x * camera_speed * t) *
        cpush(2, -vrgo_y * camera_speed * t) *
        T;
      
      hmd_ref_at = inverse(T) * hmd_at;
      }
    else if(in_perspective_v()) {
      if(vraim_x) full_rotate_camera(0, -vraim_x * t);
      if(vraim_y) full_rotate_camera(1, vraim_y * t);
      if(vrgo_y) full_forward_camera(-vrgo_y * t);
      if(vrgo_x) full_strafe_camera(-vrgo_x * t);
      }
    }
  #endif
  
  if(mouseaiming(shmup::on)) {
    #if CAP_MOUSEGRAB
    rug::using_rugview urv;
    dynamicval<bool> ds(didsomething, didsomething);
    full_rotate_camera(0, -mouseaim_x);
    full_rotate_camera(1, -mouseaim_y);
    mouseaim_x = mouseaim_y = 0;
    #endif
    }
  
  if(smooth_scrolling && !shmup::on && (cmode & (sm::NORMAL | sm::PANNING))) {
    rug::using_rugview urv;
    auto& lastticks = sc_ticks;
    ld t = (ticks - lastticks) * shiftmul / 1000.;
    lastticks = ticks;
    
    #if CAP_SDL2
    if(keystate[SDL_SCANCODE_END] && GDIM == 3 && DEFAULTNOR(SDL_SCANCODE_END)) full_forward_camera(-t);
    if(keystate[SDL_SCANCODE_HOME] && GDIM == 3 && DEFAULTNOR(SDL_SCANCODE_HOME)) full_forward_camera(t);
    if(keystate[SDL_SCANCODE_RIGHT] && DEFAULTNOR(SDL_SCANCODE_RIGHT)) full_rotate_camera(0, -t);
    if(keystate[SDL_SCANCODE_LEFT] && DEFAULTNOR(SDL_SCANCODE_LEFT)) full_rotate_camera(0, t);
    if(keystate[SDL_SCANCODE_UP] && DEFAULTNOR(SDL_SCANCODE_UP)) full_rotate_camera(1, t);
    if(keystate[SDL_SCANCODE_DOWN] && DEFAULTNOR(SDL_SCANCODE_DOWN)) full_rotate_camera(1, -t);
    if(keystate[SDL_SCANCODE_PAGEUP] && DEFAULTNOR(SDL_SCANCODE_PAGEUP)) full_rotate_view(t / degree, t);
    if(keystate[SDL_SCANCODE_PAGEDOWN] && DEFAULTNOR(SDL_SCANCODE_PAGEDOWN)) full_rotate_view(-t / degree, -t);

    #else

    if(keystate[SDLK_END] && GDIM == 3 && DEFAULTNOR(SDLK_END)) full_forward_camera(-t);
    if(keystate[SDLK_HOME] && GDIM == 3 && DEFAULTNOR(SDLK_HOME)) full_forward_camera(t);
    if(keystate[SDLK_RIGHT] && DEFAULTNOR(SDLK_RIGHT)) full_rotate_camera(0, -t);
    if(keystate[SDLK_LEFT] && DEFAULTNOR(SDLK_LEFT)) full_rotate_camera(0, t);
    if(keystate[SDLK_UP] && DEFAULTNOR(SDLK_UP)) full_rotate_camera(1, t);
    if(keystate[SDLK_DOWN] && DEFAULTNOR(SDLK_DOWN)) full_rotate_camera(1, -t);
    if(keystate[SDLK_PAGEUP] && DEFAULTNOR(SDLK_PAGEUP)) full_rotate_view(t / degree, t);
    if(keystate[SDLK_PAGEDOWN] && DEFAULTNOR(SDLK_PAGEDOWN)) full_rotate_view(-t / degree, -t);
    #endif
    }
  else sc_ticks = ticks;

  if(game_keys_scroll && !shmup::on && (cmode & sm::NORMAL) && !keystate[SDLK_LALT] && !keystate[SDLK_RALT]) {
    rug::using_rugview urv;
    auto& lastticks = sc_ticks2;
    ld t = (ticks - lastticks) * shiftmul / 1000.;
    lastticks = ticks;

    if(keystate['d'] && DEFAULTNOR('d')) full_rotate_camera(0, -t);
    if(keystate['a'] && DEFAULTNOR('a')) full_rotate_camera(0, t);
    if(keystate['w'] && DEFAULTNOR('w')) full_rotate_camera(1, t);
    if(keystate['s'] && DEFAULTNOR('s')) full_rotate_camera(1, -t);
    if(keystate['q'] && DEFAULTNOR('q')) full_rotate_view(t / degree, t);
    if(keystate['e'] && DEFAULTNOR('e')) full_rotate_view(-t / degree, -t);

    if(keystate['i'] && GDIM == 3 && DEFAULTNOR('i')) full_forward_camera(-t);
    if(keystate['k'] && GDIM == 3 && DEFAULTNOR('k')) full_forward_camera(t);
    if(keystate['l'] && GDIM == 3 && DEFAULTNOR('l')) full_strafe_camera(-t);
    if(keystate['j'] && GDIM == 3 && DEFAULTNOR('j')) full_strafe_camera(t);
    if(keystate['h'] && GDIM == 3 && DEFAULTNOR('h')) full_ystrafe_camera(-t);
    if(keystate['y'] && GDIM == 3 && DEFAULTNOR('y')) full_ystrafe_camera(t);
    }

  #if CAP_VR
  vrhr::vr_control();
  #endif
  achievement_pump();  

  for(auto d: dialog::key_queue) {
    println(hlog, "handling key ", d);
    handlekey(d, d);
    dialog::key_queue.erase(dialog::key_queue.begin());
    break;
    }
      
  while(SDL_PollEvent(&ev)) handle_event(ev);
  fix_mouseh();
  #if CAP_SDLJOY
  if(joydir.d != -1) checkjoy();
  #endif
  }

EX bool need_refresh;

EX void handle_event(SDL_Event& ev) {
  bool normal = cmode & sm::NORMAL;
    DEBB(DF_GRAPH, ("got event type #%d\n", ev.type));
    int sym = 0;
    int uni = 0;
    shiftmul = 1;
    
/*    if(ev.type == SDL_JOYDEVICEADDED || ev.type == SDL_JOYDEVICEREMOVED) {
      joyx = joyy = 0;
      panjoyx = panjoyy = 0;
      closeJoysticks();
      initJoysticks();
      } */

    #if CAP_SDL2
    if(ev.type == SDL_WINDOWEVENT) {
      auto w = ev.window.event;
      if(w == SDL_WINDOWEVENT_ENTER)
        outoffocus = false;
      if(w == SDL_WINDOWEVENT_LEAVE)
        outoffocus = true;
      if(w == SDL_WINDOWEVENT_EXPOSED)
        drawscreen();
      if(w == SDL_WINDOWEVENT_RESIZED)
        resize_screen_to(ev.window.data1, ev.window.data2);
      }
    
    #else
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
    #endif

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
      joy_ignore_next = true;
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
      #if CAP_SDL2
      uni = ev.key.keysym.sym;
      if(uni >= 'a' && uni <= 'z') {
        if(ev.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) uni -= 32;
        else if(ev.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) uni -= 96;        
        }      
      #else
      uni = ev.key.keysym.unicode;
      if(uni == 0 && (sym >= 'a' && sym <= 'z')) {
        if(ev.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) uni = sym - 96;
        }
      if(ev.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT)) shiftmul = -1;
      if(ev.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) shiftmul /= 10;
      #endif
      numlock_on = ev.key.keysym.mod & KMOD_NUM;
      if(sym == SDLK_RETURN && (ev.key.keysym.mod & (KMOD_LALT | KMOD_RALT))) {
        sym = 0; uni = 0;
        vid.want_fullscreen = !vid.want_fullscreen;
        apply_screen_settings();
        }
      }
    
    dialog::handleZooming(ev);
    
    if(sym == SDLK_F1 && normal && playermoved)
      help = "@";
    
    bool rollchange = (cmode & sm::OVERVIEW) && getcstat >= 2000 && cheater;

    if(ev.type == SDL_MOUSEBUTTONDOWN || ev.type == SDL_MOUSEBUTTONUP SDL12(, || ev.type == SDL_MOUSEWHEEL)) {
      mousepressed = ev.type == SDL_MOUSEBUTTONDOWN;
      if(mousepressed) flashMessages();
      mousing = true;
      which_pointer = 0;
      bool was_holdmouse = holdmouse;
      holdmouse = false;
      invslider = false;
      
      bool down = ev.type == SDL_MOUSEBUTTONDOWN SDL12(, || ev.type == SDL_MOUSEWHEEL);
      bool up = ev.type == SDL_MOUSEBUTTONUP;
      
      bool act = false;
      
      if(vid.quickmouse) {
        act = down;
        }
      else {
        act = actonrelease && up;
        actonrelease = down;
        }      
      
      fix_mouseh();
      
      if(was_holdmouse && up)
        sym = uni = PSEUDOKEY_RELEASE;
      
      /* simulate RMB and MMB for Mac users etc. */
      if(ev.button.button == SDL_BUTTON_LEFT) {
        if(ISPANDORA ? pandora_rightclick : lctrlclick)
          ev.button.button = SDL_BUTTON_MIDDLE;
        else if((ISPANDORA ? pandora_leftclick : lshiftclick) && !(vid.shifttarget&1) && !(cmode & sm::MAP))
          ev.button.button = SDL_BUTTON_RIGHT;
        }
      
      if(!act) ;

      else if(ev.button.button==SDL_BUTTON_RIGHT)
        sym = SDLK_F1;
      else if(ev.button.button==SDL_BUTTON_MIDDLE)
        sym = 1, didsomething = true;
      else if(ev.button.button == SDL_BUTTON_LEFT) {
        sym = getcstat, uni = getcstat, shiftmul = getcshift;
        }
      
      else if(SDL12(ev.button.button==SDL_BUTTON_WHEELDOWN || ev.button.button == SDL_BUTTON_WHEELUP, ev.type == SDL_MOUSEWHEEL)) {
        #if CAP_SDL2
        ld dir = ev.wheel.y * 0.25;
        #else
        ld dir = ev.button.button == SDL_BUTTON_WHEELUP ? 0.25 : -0.25;
        #endif
        if(lshiftclick && rshiftclick && !rug::rugged && GDIM == 2) {
          mapeditor::scaleall(pow(2, dir), lctrlclick);
          pconf.alpha *= pow(2, dir);
          }
        else if(lshiftclick && GDIM == 2)
          mapeditor::scaleall(pow(2, dir), lctrlclick);
        else if(rshiftclick && !rug::rugged && GDIM == 2)
          pconf.alpha -= dir;
        else if(lctrlclick) {
          if(dir>0) {
            pconf.xposition += (.0 + mousex - current_display->xcenter) / vpconf.scale / current_display->scrsize;
            pconf.yposition += (.0 + mousey - current_display->ycenter) / vpconf.scale / current_display->scrsize;
            }
          else
            pconf.xposition = pconf.yposition = 0;
          }
        else if(rollchange) {
          sym = getcstat, uni = getcstat, shiftmul = -dir*4*getcshift, wheelclick = true;
          }
        else {
          sym = uni = dir > 0 ? PSEUDOKEY_WHEELUP : PSEUDOKEY_WHEELDOWN;
          }
        }
      }

    if(ev.type == SDL_MOUSEMOTION) {
      mouseoh = mouseh;
      
      int lmousex = mousex, lmousey = mousey;
      
      mousing = true;
      which_pointer = 0;
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

      if(((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_MMASK)) && !mouseout2()) {
        fix_mouseh();
        if(lctrlclick) {
          pconf.xposition += (mousex - lmousex) * 1. / current_display->scrsize,
          pconf.yposition += (mousey - lmousey) * 1. / current_display->scrsize;
          }
        else if(mouseh[LDIM] < 50 && mouseoh[LDIM] < 50) {
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
    
    if(sym == SDLK_F4 && anyshiftclick) {
      nomap = !nomap;
      sym = 0;
      }
      
    if(sym == SDLK_F2 && anyshiftclick) {
      nohud = !nohud;
      sym = 0;
      }
      
    if(sym == SDLK_F3 && anyshiftclick) {
      nofps = !nofps;
      sym = 0;
      }
    
    if(sym || uni) {
      if(need_refresh) {
        just_refreshing = true;
        screens.back()();
        just_refreshing = false;
        }
      need_refresh = true;
      }
      
    handlekey(sym, uni);
    }
#endif

EX void mainloop() {
  if(noGUI) return;
  lastt = 0;
#if ISWEB
  initweb();
  emscripten_set_main_loop(mainloopiter, 0, true);
#else
  while(!quitmainloop) mainloopiter();
#endif
  }

#if ISMOBILE
EX void displayabutton(int px, int py, string s, int col) {
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

EX bool interpret_as_direction(int sym, int uni) {
  #ifdef FAKE_SDL
  return false;
  #else
  return (sym >= SDLK_KP0 && sym <= SDLK_KP9 && !numlock_on);
  #endif
  }

EX int get_direction_key(int sym, int uni) {
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

EX bool gmodekeys(int sym, int uni) {
  #if CAP_RUG
  if(rug::rugged && rug::handlekeys(sym, uni)) return true;
  #endif

  if(NUMBERKEY == '6') { vid.grid = !vid.grid; return true; }
  if(NUMBERKEY == '7') { vid.darkhepta = !vid.darkhepta; return true; }

  if(NUMBERKEY == '1') 
    pushScreen(models::quick_model);
  
  if(GDIM == 2) {
    if(NUMBERKEY == '5') { vid.wallmode += 60 + (shiftmul > 0 ? 1 : -1); vid.wallmode %= 7; }
    else if((NUMBERKEY == '8' && hiliteclick) || NUMBERKEY == 508) { 
      vid.highlightmode += 60 + (shiftmul > 0 ? 1 : -1); vid.highlightmode %= 3; 
      }
    else if(NUMBERKEY == '8') { 
      vid.monmode += 60 + (shiftmul > 0 ? 1 : -1); vid.monmode %= 4; 
      }
       
    else if(uni == '%') { 
      if(vid.wallmode == 0) vid.wallmode = 6;
      vid.wallmode--;
      }
    else return false;
    return true;
    }
  else {
    if(NUMBERKEY == '5') { vid.wallmode = vid.wallmode == 5 ? 4 : 5; }
    else if(NUMBERKEY == '8') { 
      if(vid.monmode == 0) vid.monmode = 1;
      else if(vid.monmode == 1) vid.monmode = 3;
      else vid.monmode = 0;
      }
    else return false;
    return true;
    }
  }

EX bool haveMobileCompass() {
#if ISMOBILE
  if(longclick) return false;
#else
  if(forcetarget) return false;
#endif
  if(GDIM == 3) return false;
  return canmove && !shmup::on && vid.mobilecompasssize > 0 && isize(screens) == 1;
  }
  
EX bool handleCompass() {
  if(!haveMobileCompass()) return false;
  
  using namespace shmupballs;

  int dx = mousex - xmove;
  int dy = mousey - yb;
  int h = hypot(dx, dy);
  if(h < rad) {
    if(h < rad*SKIPFAC) movepcto(MD_WAIT);
    else {
      hyperpoint param = tangent_length(point2(dx, dy), .01);

      movedir md = vectodir(param);
    
      if(!canmove) movepcto(md), remission(); else movepcto(md);
      }
    getcstat = 0;
    return true;
    }

  return false;
  }

#if CAP_ORIENTATION
EX transmatrix getOrientation();
#endif

// orientation sensitivity
EX namespace ors {

EX int mode;
double sensitivity = 1;

int when_enabled;
transmatrix last_orientation;
transmatrix relative_matrix = Id;

EX string choices[3] = {"OFF", "relative", "absolute"};

#if CAP_ORIENTATION
EX transmatrix getOrientation() {
  return MirrorX * hr::getOrientation() * MirrorX;
  }
#endif

EX void reset() {
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

EX void show() {
#if CAP_ORIENTATION
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();

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
  dialog::add_action_push(showStereo);

  dialog::addItem(XLAT("experiment with geometry"), 'g');
  dialog::add_action([] () { runGeometryExperiments(); });

  dialog::addSelItem(XLAT("projection"), fts(vpconf.alpha), 'p');
  dialog::add_action([] () { projectionDialog(); });

  dialog::addSelItem(XLAT("scale factor"), fts(vpconf.scale), 'z');
  dialog::add_action([] () { editScale(); });

  dialog::addItem(XLAT("spherical VR"), 'v');
  dialog::add_action([] () { 
    if(!sphere) set_geometry(gSphere), start_game();
    mode = 0; fullcenter();
    mode = 2; sensitivity = 1;
    vid.stereo_mode = sLR; vid.ipd = 0.2;
    vpconf.alpha = 0; vpconf.scale = 1;
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

EX void unrotate(transmatrix& T) {
  if(mode == 1) T = inverse(relative_matrix) * T;
  }

EX void rerotate(transmatrix& T) {
  if(mode == 1) T = (relative_matrix) * T;
  }

int first_check, last_check;

EX void check_orientation() {
#if CAP_ORIENTATION
  if(!mode) return;

  if(ticks > last_check + 2000) first_check = ticks;
  last_check = ticks;

  if(ticks < first_check + 500) {
    last_orientation = getOrientation();
    return;
    }
  transmatrix next_orientation = MirrorX * getOrientation();
  transmatrix T = inverse(next_orientation) * last_orientation;
  if(mode == 1) unrotate(View), unrotate(cwtV.T);
  relative_matrix = change_geometry(T);
  if(mode == 1) rerotate(View), rerotate(cwtV.T);
  if(mode == 2) View = relative_matrix * View, last_orientation = next_orientation;
#endif
  }

EX }

}
