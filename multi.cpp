// Hyperbolic Rogue -- multiplayer features
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file multi.cpp
 *  \brief multiplayer features, also input configuration
 */

#include "hyper.h"
namespace hr {

EX namespace multi {

  #if HDR
  static const int MAXJOY = 8;
  static const int MAXBUTTON = 64;
  static const int MAXAXE = 16;
  static const int MAXHAT = 4;
  
  struct config {
    char keyaction[512];
    char joyaction[MAXJOY][MAXBUTTON];
    char axeaction[MAXJOY][MAXAXE];
    char hataction[MAXJOY][MAXHAT][4];
    int  deadzoneval[MAXJOY][MAXAXE];
    };
  #endif
  
  EX config scfg;  
  EX charstyle scs[MAXPLAYER];

  EX bool split_screen;
  EX bool pvp_mode;
  EX bool friendly_fire = true;
  EX bool self_hits;
  EX bool two_focus;

  EX int players = 1;
  EX cellwalker player[MAXPLAYER];
  EX vector<int> revive_queue; // queue for revival
  
  EX cell *origpos[MAXPLAYER], *origtarget[MAXPLAYER];

  EX bool flipped[MAXPLAYER];
  
  // treasure collection, kill, and death statistics
  EX int treasures[MAXPLAYER], kills[MAXPLAYER], deaths[MAXPLAYER], pkills[MAXPLAYER], suicides[MAXPLAYER];
  
  EX bool alwaysuse = false;

  EX void recall() {
    for(int i=0; i<numplayers(); i++) {
        int idir = (3 * i) % cwt.at->type;
        cell *c2 = cwt.at->move(idir);
        makeEmpty(c2);
        if(!passable(c2, NULL, P_ISPLAYER)) c2 = cwt.at;
        multi::player[i].at = c2;
        multi::player[i].spin = 0;
        
        multi::flipped[i] = true;
        multi::whereto[i].d = MD_UNDECIDED;
        }
    }
  
  EX shiftmatrix whereis[MAXPLAYER];
  EX shiftmatrix crosscenter[MAXPLAYER];
  EX double ccdist[MAXPLAYER];
  EX cell *ccat[MAXPLAYER];
  
  bool combo[MAXPLAYER];

  EX int cpid; // player id -- an extra parameter for player-related functions
  EX int cpid_edit; // cpid currently being edited
  
  EX movedir whereto[MAXPLAYER]; // player's target cell  

  EX double mdx[MAXPLAYER], mdy[MAXPLAYER]; // movement vector for the next move
  
  static const int CMDS = 15;
  static const int CMDS_PAN = 11;

  vector<string> playercmds_shmup = {
    "forward", "backward", "turn left", "turn right",
    "move up", "move right", "move down", "move left", 
    "throw a knife", "face the pointer", "throw at the pointer", 
    "drop Dead Orb", "center the map on me", "Orb power (target: mouse)",
    "Orb power (target: facing)"
    };

  vector<string> playercmds_shmup3 = {
    "rotate up", "rotate down", "rotate left", "rotate right",
    "move forward", "strafe right", "move backward", "strafe left", 
    "throw a knife", "face the pointer", "throw at the pointer", 
    "drop Dead Orb", "center the map on me", "Orb power (target: mouse)",
    "Orb power (target: facing)"
    };
  
  vector<string> playercmds_turn = {
    "move up-right", "move up-left", "move down-right", "move down-left", 
    "move up", "move right", "move down", "move left", 
    "stay in place (left + right)", "cancel move", "leave the game", 
    "drop Dead Orb (up + down)", "center the map on me", "",
    ""
    };
  
  vector<string> pancmds = {
    "pan up", "pan right", "pan down", "pan left",
    "rotate left", "rotate right", "home",
    "world overview", "review your quest", "inventory", "main menu"
    };

  vector<string> pancmds3 = {
    "look up", "look right", "look down", "look left",
    "rotate left", "rotate right", "home",
    "world overview", "review your quest", "inventory", "main menu",
    "scroll forward", "scroll backward"
    };

#if HDR
#define SHMUPAXES_BASE 4
#define SHMUPAXES ((SHMUPAXES_BASE) + 4 * (MAXPLAYER))
#define SHMUPAXES_CUR ((SHMUPAXES_BASE) + 4 * playercfg)
#endif

EX const char* axemodes[SHMUPAXES] = {
  "do nothing", 
  "rotate view",
  "panning X",
  "panning Y",
  "player 1 X", 
  "player 1 Y", 
  "player 1 go", 
  "player 1 spin", 
  "player 2 X", 
  "player 2 Y", 
  "player 2 go", 
  "player 2 spin",
  "player 3 X", 
  "player 3 Y", 
  "player 3 go", 
  "player 3 spin",
  "player 4 X", 
  "player 4 Y", 
  "player 4 go", 
  "player 4 spin",
  "player 5 X", 
  "player 5 Y", 
  "player 5 go", 
  "player 5 spin",
  "player 6 X", 
  "player 6 Y", 
  "player 6 go", 
  "player 6 spin",
  "player 7 X", 
  "player 7 Y", 
  "player 7 go", 
  "player 7 spin"
  };

EX const char* axemodes3[4] = {
  "do nothing",
  "camera forward",
  "camera rotate X",
  "camera rotate Y"
  };

EX int centerplayer = -1;

char* axeconfigs[24]; int numaxeconfigs;
int* dzconfigs[24];

string listkeys(int id) {
#if CAP_SDL
  string lk = "";
  for(int i=0; i<512; i++)
    if(scfg.keyaction[i] == id)
      #if CAP_SDL2
      lk = lk + " " + SDL_GetScancodeName(SDL_Scancode(i));
      #else
      lk = lk + " " + SDL_GetKeyName(SDLKey(i));
      #endif
#if CAP_SDLJOY
  for(int i=0; i<numsticks; i++) for(int k=0; k<SDL_JoystickNumButtons(sticks[i]) && k<MAXBUTTON; k++)
    if(scfg.joyaction[i][k] == id) {
      lk = lk + " " + cts('A'+i)+"-B"+its(k);
      }
  for(int i=0; i<numsticks; i++) for(int k=0; k<SDL_JoystickNumHats(sticks[i]) && k<MAXHAT; k++)
    for(int d=0; d<4; d++)
      if(scfg.hataction[i][k][d] == id) {
        lk = lk + " " + cts('A'+i)+"-"+"URDL"[d];
        }
#endif
  return lk;
#else
  return "";
#endif
  }

#define SCJOY 16

string dsc(int id) {
  string buf = XLAT(" (%1 $$$, %2 kills, %3 deaths)",
    its(multi::treasures[id]),
    its(multi::kills[id]),
    its(multi::deaths[id])
    );
  if(friendly_fire)
    buf += XLAT(" (%1 pkills)", its(multi::pkills[id]));
  if(self_hits)
    buf += XLAT(" (%1 self)", its(multi::suicides[id]));
  return buf;
  }

EX void resetScores() {
  for(int i=0; i<MAXPLAYER; i++)
    multi::treasures[i] = multi::kills[i] = multi::deaths[i] = multi::pkills[i] = multi::suicides[i] = 0;
  }
 
bool configdead;

void handleConfig(int sym, int uni);

EX string player_count_name(int p) {
  return 
    p == 2 ? XLAT("two players") : 
    p == 3 ? XLAT("three players") : 
    p == 4 ? XLAT("four players") : 
    p == 5 ? XLAT("five players") : 
    p == 6 ? XLAT("six players") : 
    p == 7 ? XLAT("seven players") : 
    XLAT("one player");
  }

struct key_configurer {

  int sc;
  vector<string>& shmupcmdtable;
  string caption;
  int setwhat;

  key_configurer(int sc, vector<string>& sct, const string& caption) : sc(sc), shmupcmdtable(sct), caption(caption), setwhat(0) {
    }

  void operator() () {
      
    dialog::init(caption);
  
    getcstat = ' ';
    
    for(int i=0; i<isize(shmupcmdtable); i++) if(shmupcmdtable[i][0])
      dialog::addSelItem(XLAT(shmupcmdtable[i]), listkeys(16*sc+i),
        setwhat ? (setwhat>1 && i == (setwhat&15) ? '?' : 0) : 'a'+i);
      else dialog::addBreak(100);
  
    if(setwhat == 1)
      dialog::addItem(XLAT("press a key to unassign"), 0);
    else if(setwhat)
      dialog::addItem(XLAT("press a key for '%1'", XLAT(shmupcmdtable[setwhat&15])), 0);
    else
      dialog::addItem(XLAT("unassign a key"), 'z');
    
    dialog::display();
  
    keyhandler = [this] (int sym, int uni) {
      if(!setwhat) dialog::handleNavigation(sym, uni);
      if(sym) {
        if(setwhat) {
          scfg.keyaction[sym] = setwhat;
          setwhat = 0;
          }
        else if(uni >= 'a' && uni < 'a' + isize(shmupcmdtable) && shmupcmdtable[uni-'a'][0])
          setwhat = 16*sc+uni - 'a';
        else if(uni == 'z')
          setwhat = 1;
        else if(doexiton(sym, uni))
          popScreen();
        }
      };

#if CAP_SDLJOY    
    joyhandler = [this] (SDL_Event& ev) { 
      if(ev.type == SDL_JOYBUTTONDOWN && setwhat) {
        int joyid = ev.jbutton.which;
        int button = ev.jbutton.button;
        if(joyid < 8 && button < 32)
           scfg.joyaction[joyid][button] = setwhat;
        setwhat = 0;
        return true;
        }
  
      else if(ev.type == SDL_JOYHATMOTION && setwhat) {
        int joyid = ev.jhat.which;
        int hat = ev.jhat.hat;
        int dir = 4;
        if(ev.jhat.value == SDL_HAT_UP) dir = 0;
        if(ev.jhat.value == SDL_HAT_RIGHT) dir = 1;
        if(ev.jhat.value == SDL_HAT_DOWN) dir = 2;
        if(ev.jhat.value == SDL_HAT_LEFT) dir = 3;
        printf("%d %d %d\n", joyid, hat, dir);
        if(joyid < 8 && hat < 4 && dir < 4) {
          scfg.hataction[joyid][hat][dir] = setwhat;
          setwhat = 0;
          return true;
          }
        }
      return false;
      };
#endif
    }
  };

EX reaction_t get_key_configurer(int sc, vector<string>& sct, string caption) { 
  return key_configurer(sc, sct, caption); 
  }

EX reaction_t get_key_configurer(int sc, vector<string>& sct) { 
  return key_configurer(sc, sct, sc == 1 ? XLAT("configure player 1") :
    sc == 2 ? XLAT("configure player 2") :
    sc == 3 ? XLAT("configure panning") :
    sc == 4 ? XLAT("configure player 3") :
    sc == 5 ? XLAT("configure player 4") :
    sc == 6 ? XLAT("configure player 5") :
    sc == 7 ? XLAT("configure player 6") :
    sc == 8 ? XLAT("configure player 7") : ""
    ); 
  }

#if CAP_SDLJOY
struct joy_configurer {

  bool shmupcfg, racecfg;
  int playercfg;
  joy_configurer(int playercfg) : playercfg(playercfg) {}

  void operator() () {
    dialog::init();
    getcstat = ' ';
    numaxeconfigs = 0;
    for(int j=0; j<numsticks; j++) {
      for(int ax=0; ax<SDL_JoystickNumAxes(sticks[j]) && ax < MAXAXE; ax++) if(numaxeconfigs<24) {
        int y = SDL_JoystickGetAxis(sticks[j], ax);
        string buf = " ";
        if(configdead)
          buf += its(y);
        else {
          while(y >  10000) buf += "+", y -= 10000;
          while(y < -10000) buf += "-", y += 10000;
          if(y>0) buf += "+";
          if(y<0) buf += "-";
          }
        axeconfigs[numaxeconfigs] = &(scfg.axeaction[j][ax]);
        dzconfigs[numaxeconfigs] = &(scfg.deadzoneval[j][ax]);
        char aa = *axeconfigs[numaxeconfigs];
        string what = configdead ? its(scfg.deadzoneval[j][ax]) : 
          (GDIM == 3 && (aa%SHMUPAXES < 4)) ? XLAT(axemodes3[aa%SHMUPAXES]) :
          XLAT(axemodes[aa%SHMUPAXES]);
        dialog::addSelItem(XLAT("Joystick %1, axis %2", cts('A'+j), its(ax)) + buf, 
          what, 'a'+numaxeconfigs);
        numaxeconfigs++;
        }
      }
    
    dialog::addBoolItem(XLAT("Configure dead zones"), (configdead), 'z');
    dialog::display();

    keyhandler = [this] (int sym, int uni) { 
      dialog::handleNavigation(sym, uni);
      if(sym) {
        char xuni = uni | 96;
        if(xuni >= 'a' && xuni < 'a' + numaxeconfigs) {
          if(configdead) 
            dialog::editNumber( (*dzconfigs[xuni - 'a']), 0, 65536, 100, 0, XLAT("Configure dead zones"), "");
          else {
            int v = (*axeconfigs[xuni - 'a']);
            v += (shiftmul>0?1:-1);
            v += SHMUPAXES_CUR;
            v %= SHMUPAXES_CUR;
            (*axeconfigs[xuni - 'a']) = v;
            }
          }
        else if(xuni == 'z')
          configdead = !configdead;
        else if(doexiton(sym, uni))
          popScreen();
        }
      };
    }
  };
#endif

EX const char *axmodes[7] = {"OFF", "auto", "light", "heavy", "arrows", "WASD keys", "VI keys"};

struct shmup_configurer {

  void operator()() {
  #if CAP_SDL
    cmode = sm::SHMUPCONFIG | sm::SIDE | sm::DARKEN;
    gamescreen();
    dialog::init(XLAT("keyboard & joysticks"));
    
    bool haveconfig = shmup::on || players > 1 || multi::alwaysuse;
  
    if(haveconfig)
      dialog::addItem(XLAT("configure player 1"), '1');
    else
      dialog::addBreak(100);
    if(players > 1)
      dialog::addItem(XLAT("configure player 2"), '2');
    else if(players == 1 && !shmup::on)
      dialog::addSelItem(XLAT("input"), multi::alwaysuse ? XLAT("config") : XLAT("default"), 'a');
    else
      dialog::addBreak(100);
    if(players > 2)
      dialog::addItem(XLAT("configure player 3"), '3');
  #if CAP_SDLJOY
    else if(!haveconfig)
      dialog::addItem(XLAT("old style joystick configuration"), 'b');
  #endif
    else dialog::addBreak(100);
    if(players > 3)
      dialog::addItem(XLAT("configure player 4"), '4');
    else if(!shmup::on && !multi::alwaysuse) {
      dialog::addBoolItem(XLAT("smooth scrolling"), smooth_scrolling, 'c');
      }
    else if(alwaysuse)
      dialog::addInfo(XLAT("note: configured input is designed for"));
    else dialog::addBreak(100);
      
    if(players > 4)
      dialog::addItem(XLAT("configure player 5"), '5');
    else if(!shmup::on && !multi::alwaysuse) {
      if(GDIM == 2) {
        dialog::addSelItem(XLAT("help for keyboard users"), XLAT(axmodes[vid.axes]), 'h');
        dialog::add_action([] {vid.axes += 70 + (shiftmul > 0 ? 1 : -1); vid.axes %= 7; } );
        }
      else dialog::addBreak(100);
      }
    else if(alwaysuse)
      dialog::addInfo(XLAT("multiplayer and shmup mode; some features"));
    else dialog::addBreak(100);
  
    if(players > 5)
      dialog::addItem(XLAT("configure player 6"), '6');
    else if(alwaysuse)
      dialog::addInfo(XLAT("work worse if you use it."));
    else dialog::addBreak(100);
  
    if(players > 6)
      dialog::addItem(XLAT("configure player 7"), '7');
    else dialog::addBreak(100);
      
    if(shmup::on || multi::alwaysuse || players > 1)
      dialog::addItem(XLAT("configure panning and general keys"), 'p');
    else dialog::addBreak(100);
  
  #if CAP_SDLJOY
    if(numsticks > 0) {
      if(shmup::on || multi::alwaysuse || players > 1) 
        dialog::addItem(XLAT("configure joystick axes"), 'j');
      else dialog::addBreak(100);
      }
  #endif
  
    dialog::addBreak(50);
  
    dialog::addHelp();
  
    dialog::addBack();
    dialog::display();
    
    keyhandler = [this] (int sym, int uni) { return handleConfig(sym, uni); };
  #endif
    }

  void handleConfig(int sym, int uni) {
    auto& cmdlist = shmup::on ? (WDIM == 3 ? playercmds_shmup3 : playercmds_shmup) : playercmds_turn;
    dialog::handleNavigation(sym, uni);
    
    if(0) ;
    #if CAP_SDL
    else if(uni == '1') pushScreen(get_key_configurer(1, cmdlist));
    else if(uni == '2') pushScreen(get_key_configurer(2, cmdlist));
    else if(uni == 'p') pushScreen(get_key_configurer(3, GDIM == 3 ? pancmds3 : pancmds));
    else if(uni == '3') pushScreen(get_key_configurer(4, cmdlist));
    else if(uni == '4') pushScreen(get_key_configurer(5, cmdlist));
    else if(uni == '5') pushScreen(get_key_configurer(6, cmdlist));
    else if(uni == '6') pushScreen(get_key_configurer(7, cmdlist));
    else if(uni == '7') pushScreen(get_key_configurer(8, cmdlist));
  #if CAP_SDLJOY
    else if(uni == 'j') pushScreen(joy_configurer(players));
  #endif
    else if(uni == 'a') multi::alwaysuse = !multi::alwaysuse;
  #if CAP_SDLJOY
    else if(uni == 'b') pushScreen(showJoyConfig);
  #endif
    else if(uni == 'c') smooth_scrolling = !smooth_scrolling;
    #endif
    else if(doexiton(sym, uni)) popScreen();
    }
  };

EX void configure() {
  pushScreen(shmup_configurer());
  }

EX void showConfigureMultiplayer() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init("multiplayer");
  
  for(int i=1; i <= MAXPLAYER; i++) {
    string s = player_count_name(i);
    if(i <= players) s += dsc(i-1);
    dialog::addBoolItem(s, i == multi::players, '0' + i);
    if(!dual::state) dialog::add_action([i] {
      dialog::do_if_confirmed([i] {
        stop_game();
        players = i;
        start_game();
        });
      });
    }

  add_edit(self_hits);
  if(multi::players > 1) {
    dialog::addItem(XLAT("reset per-player statistics"), 'r');
    dialog::add_action([] {
      for(int i=0; i<MAXPLAYER; i++) 
        kills[i] = deaths[i] = treasures[i] = 0;
      });

    dialog::addSelItem(XLAT("keyboard & joysticks"), "", 'k');
    dialog::add_action(multi::configure);
    add_edit(split_screen);
    if(shmup::on && !racing::on) {
      add_edit(pvp_mode);
      add_edit(friendly_fire);
      if(pvp_mode)
        dialog::addInfo(XLAT("PvP grants infinite lives -- achievements disabled"));
      else if(friendly_fire)
        dialog::addInfo(XLAT("friendly fire off -- achievements disabled"));
      else if(split_screen)
        dialog::addInfo(XLAT("achievements disabled in split screen"));
      else
        dialog::addBreak(100);
      }
    else {
      dialog::addInfo(XLAT("PvP available only in shmup"));
      dialog::addBreak(400);
      }
    if(multi::players == 2 && !split_screen)
      add_edit(two_focus);
    else
      dialog::addBreak(100);
    }
  else dialog::addBreak(600);
  
  dialog::addBack();
  dialog::display();
  }

#if HDR
#define NUMACT 128

enum pcmds {
  pcForward, pcBackward, pcTurnLeft, pcTurnRight,
  pcMoveUp, pcMoveRight, pcMoveDown, pcMoveLeft,
  pcFire, pcFace, pcFaceFire,
  pcDrop, pcCenter, pcOrbPower, pcOrbKey
  };
#endif
  
EX int actionspressed[NUMACT], axespressed[SHMUPAXES], lactionpressed[NUMACT];

void pressaction(int id) {
  if(id >= 0 && id < NUMACT)
    actionspressed[id]++;
  }

EX bool notremapped(int sym) {
  int k = scfg.keyaction[sym];
  if(k == 0) return true;
  k /= 16;
  if(k > 3) k--; else if(k==3) k = 0;
  return k > multi::players;
  }

EX void initConfig() {
  
  char* t = scfg.keyaction;
  
  #if CAP_SDL2

  t[SDL_SCANCODE_W] = 16 + 4;
  t[SDL_SCANCODE_D] = 16 + 5;
  t[SDL_SCANCODE_S] = 16 + 6;
  t[SDL_SCANCODE_A] = 16 + 7;

  t[SDL_SCANCODE_KP_8] = 16 + 4;
  t[SDL_SCANCODE_KP_6] = 16 + 5;
  t[SDL_SCANCODE_KP_2] = 16 + 6;
  t[SDL_SCANCODE_KP_4] = 16 + 7;

  t[SDL_SCANCODE_F] = 16 + pcFire;
  t[SDL_SCANCODE_G] = 16 + pcFace;
  t[SDL_SCANCODE_H] = 16 + pcFaceFire;
  t[SDL_SCANCODE_R] = 16 + pcDrop;
  t[SDL_SCANCODE_T] = 16 + pcOrbPower;
  t[SDL_SCANCODE_Y] = 16 + pcCenter;

  t[SDL_SCANCODE_I] = 32 + 4;
  t[SDL_SCANCODE_L] = 32 + 5;
  t[SDL_SCANCODE_K] = 32 + 6;
  t[SDL_SCANCODE_J] = 32 + 7;
  t[SDL_SCANCODE_SEMICOLON] = 32 + 8;
  t[SDL_SCANCODE_APOSTROPHE] = 32 + 9;
  t[SDL_SCANCODE_P] = 32 + 10;
  t[SDL_SCANCODE_LEFTBRACKET] = 32 + pcCenter;

  t[SDL_SCANCODE_UP] = 48 ;
  t[SDL_SCANCODE_RIGHT] = 48 + 1;
  t[SDL_SCANCODE_DOWN] = 48 + 2;
  t[SDL_SCANCODE_LEFT] = 48 + 3;
  t[SDL_SCANCODE_PAGEUP] = 48 + 4;
  t[SDL_SCANCODE_PAGEDOWN] = 48 + 5;
  t[SDL_SCANCODE_HOME] = 48 + 6;
  
  #else  
  t[(int)'w'] = 16 + 4;
  t[(int)'d'] = 16 + 5;
  t[(int)'s'] = 16 + 6;
  t[(int)'a'] = 16 + 7;

#if !ISMOBILE
  t[SDLK_KP8] = 16 + 4;
  t[SDLK_KP6] = 16 + 5;
  t[SDLK_KP2] = 16 + 6;
  t[SDLK_KP4] = 16 + 7;
#endif

  t[(int)'f'] = 16 + pcFire;
  t[(int)'g'] = 16 + pcFace;
  t[(int)'h'] = 16 + pcFaceFire;
  t[(int)'r'] = 16 + pcDrop;
  t[(int)'t'] = 16 + pcOrbPower;
  t[(int)'y'] = 16 + pcCenter;

  t[(int)'i'] = 32 + 4;
  t[(int)'l'] = 32 + 5;
  t[(int)'k'] = 32 + 6;
  t[(int)'j'] = 32 + 7;
  t[(int)';'] = 32 + 8;
  t[(int)'\''] = 32 + 9;
  t[(int)'p'] = 32 + 10;
  t[(int)'['] = 32 + pcCenter;

#if !ISMOBILE
  t[SDLK_UP] = 48 ;
  t[SDLK_RIGHT] = 48 + 1;
  t[SDLK_DOWN] = 48 + 2;
  t[SDLK_LEFT] = 48 + 3;
  t[SDLK_PAGEUP] = 48 + 4;
  t[SDLK_PAGEDOWN] = 48 + 5;
  t[SDLK_HOME] = 48 + 6;
#endif
  #endif

  scfg.joyaction[0][0] = 16 + pcFire;
  scfg.joyaction[0][1] = 16 + pcOrbPower;
  scfg.joyaction[0][2] = 16 + pcDrop;
  scfg.joyaction[0][3] = 16 + pcCenter;
  scfg.joyaction[0][4] = 16 + pcFace;
  scfg.joyaction[0][5] = 16 + pcFaceFire;

  scfg.joyaction[1][0] = 32 + pcFire;
  scfg.joyaction[1][1] = 32 + pcOrbPower;
  scfg.joyaction[1][2] = 32 + pcDrop;
  scfg.joyaction[1][3] = 32 + pcCenter;
  scfg.joyaction[1][4] = 32 + pcFace;
  scfg.joyaction[1][5] = 32 + pcFaceFire;

  scfg.axeaction[0][0] = 4;
  scfg.axeaction[0][1] = 5;
  scfg.axeaction[0][3] = 2;
  scfg.axeaction[0][4] = 3;

  scfg.axeaction[1][0] = 8;
  scfg.axeaction[1][1] = 9;
  
  // ULRD
  scfg.hataction[0][0][0] = 16 + 0;
  scfg.hataction[0][0][1] = 16 + 3;
  scfg.hataction[0][0][2] = 16 + 1;
  scfg.hataction[0][0][3] = 16 + 2;
  scfg.hataction[0][1][0] = 16 + 4;
  scfg.hataction[0][1][1] = 16 + 7;
  scfg.hataction[0][1][2] = 16 + 5;
  scfg.hataction[0][1][3] = 16 + 6;

  scfg.hataction[1][0][0] = 32 + 0;
  scfg.hataction[1][0][1] = 32 + 3;
  scfg.hataction[1][0][2] = 32 + 1;
  scfg.hataction[1][0][3] = 32 + 2;
  scfg.hataction[1][1][0] = 32 + 4;
  scfg.hataction[1][1][1] = 32 + 7;
  scfg.hataction[1][1][2] = 32 + 5;
  scfg.hataction[1][1][3] = 32 + 6;

  int charidtable[MAXPLAYER] = {0, 1, 4, 6, 2, 3, 0};
    
  for(int i=0; i<MAXPLAYER; i++) {
    initcs(multi::scs[i]); 
    multi::scs[i].charid = charidtable[i];
    }
  
  multi::scs[0].uicolor = 0xC00000FF;
  multi::scs[1].uicolor = 0x00C000FF;
  multi::scs[2].uicolor = 0x0000C0FF;
  multi::scs[3].uicolor = 0xC0C000FF;
  multi::scs[4].uicolor = 0xC000C0FF;
  multi::scs[5].uicolor = 0x00C0C0FF;
  multi::scs[6].uicolor = 0xC0C0C0FF;
  
  #if CAP_CONFIG
  addsaver(multi::players, "mode-number of players");
  param_b(multi::split_screen, "splitscreen", false)
    ->editable("split screen mode", 's');
  param_b(multi::pvp_mode, "pvp_mode", false)
    ->editable("player vs player", 'v');
  param_b(multi::friendly_fire, "friendly_fire", true)
    ->editable("friendly fire", 'f');
  param_b(multi::self_hits, "self_hits", false)
    ->editable("self hits", 'h');
  param_b(multi::two_focus, "two_focus", false)
    ->editable("auto-adjust dual-focus projections", 'f');
  addsaver(alwaysuse, "use configured keys");  
  // unfortunately we cannot use key names here because SDL is not yet initialized
  for(int i=0; i<512; i++)
    addsaver(scfg.keyaction[i], string("key:")+its(i));
  for(int i=0; i<MAXJOY; i++) {
    string pre = "joystick "+cts('A'+i);
    for(int j=0; j<MAXBUTTON; j++) 
      addsaver(scfg.joyaction[i][j], pre+"-B"+its(j));
    for(int j=0; j<MAXAXE; j++) {
      addsaver(scfg.axeaction[i][j], pre+" axis "+its(j));
      addsaver(scfg.deadzoneval[i][j], pre+" deadzone "+its(j));
      }
    for(int j=0; j<MAXHAT; j++) for(int k=0; k<4; k++) {
      addsaver(scfg.hataction[i][j][k], pre+" hat "+its(j)+" "+"URDL"[k]);
      }
    }
  for(int i=0; i<7; i++) addsaver(multi::scs[i], "player"+its(i));
  #endif
  }

EX void get_actions() {
  #if !ISMOBILE
  const Uint8 *keystate = SDL12_GetKeyState(NULL);

  for(int i=0; i<NUMACT; i++) 
    lactionpressed[i] = actionspressed[i],
    actionspressed[i] = 0;

  for(int i=0; i<SHMUPAXES; i++) axespressed[i] = 0;
  
  for(int i=0; i<KEYSTATES; i++) if(keystate[i]) 
    pressaction(scfg.keyaction[i]);

#if CAP_SDLJOY  
  for(int j=0; j<numsticks; j++) {

    for(int b=0; b<SDL_JoystickNumButtons(sticks[j]) && b<MAXBUTTON; b++)
      if(SDL_JoystickGetButton(sticks[j], b))
        pressaction(scfg.joyaction[j][b]);

    for(int b=0; b<SDL_JoystickNumHats(sticks[j]) && b<MAXHAT; b++) {
      int stat = SDL_JoystickGetHat(sticks[j], b);
      if(stat & SDL_HAT_UP) pressaction(scfg.hataction[j][b][0]);
      if(stat & SDL_HAT_RIGHT) pressaction(scfg.hataction[j][b][1]);
      if(stat & SDL_HAT_DOWN) pressaction(scfg.hataction[j][b][2]);
      if(stat & SDL_HAT_LEFT) pressaction(scfg.hataction[j][b][3]);
      }
    
    for(int b=0; b<SDL_JoystickNumAxes(sticks[j]) && b<MAXAXE; b++) {
      int value = SDL_JoystickGetAxis(sticks[j], b);
      int dz = scfg.deadzoneval[j][b];
      if(value > dz) value -= dz; else if(value < -dz) value += dz;
      else value = 0;
      axespressed[scfg.axeaction[j][b] % SHMUPAXES] += value;
      }
    }
#endif
#endif
  }

EX void handleInput(int delta) {
#if CAP_SDL
  double d = delta / 500.;

  get_actions();

  const Uint8 *keystate = SDL12_GetKeyState(NULL);
  if(keystate[SDLK_LCTRL] || keystate[SDLK_RCTRL]) d /= 5;
  
  double panx = 
    actionspressed[49] - actionspressed[51] + axespressed[2] / 32000.0;
  double pany = 
    actionspressed[50] - actionspressed[48] + axespressed[3] / 32000.0;
    
  double panspin = actionspressed[52] - actionspressed[53];
  
  double panmove = actionspressed[59] - actionspressed[60];
  
  if(GDIM == 3)
    panmove += axespressed[1] / 32000.0;
  else
    panspin += axespressed[1] / 32000.0;
  
  if(actionspressed[54]) { centerplayer = -1, playermoved = true; centerpc(100); }

  if(actionspressed[55] && !lactionpressed[55]) 
    get_o_key().second();
  
  if(actionspressed[56] && !lactionpressed[56]) 
    showMissionScreen();
  
#if CAP_INV
  if(actionspressed[57] && !lactionpressed[57] && inv::on) 
    pushScreen(inv::show);
#endif
  
  if(actionspressed[58] && !lactionpressed[58]) 
    pushScreen(showGameMenu);
    
  panx *= d;
  pany *= d;
  panspin *= d;
  panmove *= d;
  
  #if CAP_MOUSEGRAB
  if(lctrlclick) {
    panx += mouseaim_x / 2;
    pany += mouseaim_y / 2;
    mouseaim_x = mouseaim_y = 0;
    }
  #endif

  if(panx || pany || panspin || (GDIM == 3 && panmove)) {
    if(GDIM == 2) {
      View = xpush(-panx) * ypush(-pany) * spin(panspin) * View;
      playermoved = false;
      }
    else {
      View = cspin(0, 2, -panx) * cspin(1, 2, -pany) * spin(panspin) * cpush(2, panmove) * View;
      if(panmove) playermoved = false;
      }
    }
#endif
  }

  EX int tableid[7] = {1, 2, 4, 5, 6, 7, 8};

  EX void leaveGame(int i) {
    multi::player[i].at = NULL;
    multi::deaths[i]++;
    revive_queue.push_back(i);
    checklastmove();
    }

  EX bool playerActive(int p) {
    if(multi::players == 1 || shmup::on) return true;
    return player[p].at;
    }
  
  EX int activePlayers() {
    int q = 0;
    for(int i=0; i<players; i++) if(playerActive(i)) q++;
    return q;
    }
  
  EX cell *multiPlayerTarget(int i) {
    cellwalker cwti = multi::player[i];
    if(!cwti.at) return NULL;
    int dir = multi::whereto[i].d;
    if(dir == MD_UNDECIDED) return NULL;
    if(dir == MD_USE_ORB) return multi::whereto[i].tgt;
    if(dir >= 0) 
      cwti = cwti + dir + wstep;
    return cwti.at;
    }
  
  EX void checklastmove() {
    for(int i: player_indices()) {
      multi::cpid = i;
      cwt = multi::player[i]; break;
      }
    if(multi::activePlayers() == 1) {
      multi::checkonly = true;
      checkmove();
      multi::checkonly = false;
      }
    }

  bool needinput = true;
  
  EX void handleMulti(int delta) {
    multi::handleInput(delta);
    
    shiftmatrix bcwtV = cwtV;
    cellwalker bcwt = cwt;
    
    bool alldecided = !needinput;
    
    if(multi::players == 1) {
      multi::cpid = 0;
      multi::whereis[0] = cwtV;
      multi::player[0] = cwt;
      }
    
    for(int i: player_indices()) {
    
      using namespace multi;
      
  // todo refactor
  
      cpid = i;
      
      int b = 16*tableid[cpid];
      for(int ik=0; ik<8; ik++) if(actionspressed[b+ik]) playermoved = true;
      for(int ik=0; ik<16; ik++) if(actionspressed[b+ik] && !lactionpressed[b+ik]) 
        multi::combo[i] = false;
          
      bool anypressed = false;
      
      int jb = 4*tableid[cpid];
      for(int ik=0; ik<4; ik++) 
        if(axespressed[jb+ik]) 
          anypressed = true, playermoved = true, multi::combo[i] = false;
      
      double mdx = 
        (actionspressed[b+0] + actionspressed[b+2] - actionspressed[b+1] - actionspressed[b+3]) * .7 +
        actionspressed[b+pcMoveRight] - actionspressed[b+pcMoveLeft] + axespressed[jb]/30000.;
      double mdy = 
        (actionspressed[b+3] + actionspressed[b+2] - actionspressed[b+1] - actionspressed[b+0]) * .7 +
        actionspressed[b+pcMoveDown] - actionspressed[b+pcMoveUp] + axespressed[jb+1]/30000.;
      
      if((actionspressed[b+pcMoveRight] && actionspressed[b+pcMoveLeft]) ||
        (actionspressed[b+pcMoveUp] && actionspressed[b+pcMoveDown]))
          multi::mdx[i] = multi::mdy[i] = 0;
        
      multi::mdx[i] = multi::mdx[i] * (1 - delta / 1000.) + mdx * delta / 2000.;
      multi::mdy[i] = multi::mdy[i] * (1 - delta / 1000.) + mdy * delta / 2000.;
  
      if(WDIM == 2) {
        if(mdx != 0 || mdy != 0) if(!multi::combo[i]) {
          cwtV = multi::whereis[i]; cwt = multi::player[i];
          flipplayer = multi::flipped[i];
          multi::whereto[i] = vectodir(hpxy(multi::mdx[i], multi::mdy[i]));
          }
        }
      
      if(multi::actionspressed[b+pcFire] || 
        (multi::actionspressed[b+pcMoveLeft] && multi::actionspressed[b+pcMoveRight]))
        multi::combo[i] = true, multi::whereto[i].d = MD_WAIT;
  
      if(multi::actionspressed[b+pcFace])
        multi::whereto[i].d = MD_UNDECIDED;
      
      cwt.at = multi::player[i].at;      
      if(multi::ccat[i] && !multi::combo[i] && targetRangedOrb(multi::ccat[i], roMultiCheck)) {
        multi::whereto[i].d = MD_USE_ORB;
        multi::whereto[i].tgt = multi::ccat[i];
        }

      if(multi::actionspressed[b+pcFaceFire] && activePlayers() > 1) {
        addMessage(XLAT("Left the game."));
        multi::leaveGame(i);
        }
  
      if(actionspressed[b+pcDrop] || 
        (multi::actionspressed[b+pcMoveUp] && multi::actionspressed[b+pcMoveDown]))
        multi::combo[i] = true, multi::whereto[i].d = MD_DROP;
  
      if(actionspressed[b+pcCenter]) {
        centerplayer = cpid; centerpc(100); playermoved = true; 
        }
  
      if(multi::whereto[i].d == MD_UNDECIDED) alldecided = false;
      
      for(int ik=0; ik<16; ik++) if(actionspressed[b+ik]) anypressed = true;

      if(anypressed) alldecided = false, needinput = false;
      else multi::mdx[i] = multi::mdy[i] = 0;
      }
      
    cwtV = bcwtV;
    cwt = bcwt;
    
    if(alldecided) {
      flashMessages();
      // check for crashes
      needinput = true;

      for(int i: player_indices()) {
        origpos[i] = player[i].at;
        origtarget[i] = multiPlayerTarget(i);
        }
  
      for(int i: player_indices())
      for(int j: player_indices()) if(i != j) {
        if(origtarget[i] == origtarget[j]) {
          addMessage("Two players cannot move/attack the same location!");
          return;
          }
/*      if(multiPlayerTarget(i) == multi::player[j].at) {
          addMessage("Cannot move into the current location of another player!");
          return;
          }
        if(celldistance(multiPlayerTarget(i), multiPlayerTarget(j)) > 8) {
          addMessage("Players cannot get that far away!");
          return;
          } */
        }
  
      if(multi::players == 1) {
        if(movepcto(multi::whereto[0]))
          multi::whereto[0].d = MD_UNDECIDED;
        return;
        }
      
      multi::cpid = 0;
      if(multimove()) {      
        multi::aftermove = false;
        if(shmup::delayed_safety) {
          activateSafety(shmup::delayed_safety_land);
          shmup::delayed_safety = false;
          checklastmove();
          }
        else {          
          monstersTurn();
          checklastmove();
          }
        }
      }
    }
  
  EX void mousemovement(cell *c) {
    if(!c) return;
    int countplayers = 0;
    int countplayers_undecided = 0;
    for(int i=0; i<multi::players; i++)
      if(multi::playerActive(i) && (playerpos(i) == c || isNeighbor(c, playerpos(i)))) {
        countplayers++;
        if(multi::whereto[i].d == MD_UNDECIDED) countplayers_undecided++;
        }
  
    for(int i=0; i<multi::players; i++)
      if(multi::playerActive(i) && (playerpos(i) == c || isNeighbor(c, playerpos(i)))) {
        int& cdir = multi::whereto[i].d;
        int scdir = cdir;
        bool isUndecided = cdir == MD_UNDECIDED;
        if(countplayers_undecided > 0 && ! isUndecided) continue;
        if(playerpos(i) == c)
          multi::whereto[i].d = MD_WAIT;
        else {
          for(int d=0; d<playerpos(i)->type; d++) {
            cdir = d;
            if(multi::multiPlayerTarget(i) == c) break;
            cdir = scdir;
            cwt = multi::player[i];
            calcMousedest();
            auto& sd = multi::whereto[i].subdir;
            sd = mousedest.subdir;
            if(sd == 0) sd = 1;
            }
          }
        }
    
    needinput = 
      ((countplayers == 2 && !countplayers_undecided) || countplayers_undecided >= 2);
    }
  
  EX }

}
